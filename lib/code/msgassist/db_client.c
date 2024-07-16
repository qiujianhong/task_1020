/*
*********************************************************************
* @filename: dbCenter_msg.c
* @brief   : 消息通信 DB操作
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2019-12-18    create
*********************************************************************
*/
#include "baseCommon.h"
#include "task_msg.h"
#include "dbmsg_id.h"
#include "appmsg_split.h"

#include "db_comm.h"

uint32 g_ATaskid = 0;

#define DB_OPERATE_OUT_TIME      5
#define SMALL_MSG_BUF_LEN        128
extern int task_db_get_syncmsg(int sockfd, uint32 msgid, uint8 *msgdata, uint16 datalen, uint32 dest,
                               uint8 *msgbuf, uint16 buflen, uint8 outtime);

/**********************************************************************
* @name      : db_socket_create
* @brief     ：db操作句柄创建 
* @param[in] ：uint32 ataskid     代理的Taskid(低16位有效)
               uint32 ctaskid     客户端的Taskid(低16位有效)
* @param[out]：
* @return    ：DB_CLIENT  句柄
* @Create    : 王津剑
* @Date      ：2019-12-25
* @Update    :
**********************************************************************/
DB_CLIENT db_client_create(uint32 ataskid, uint32 ctaskid)
{
    DB_CLIENT clientfd = 0;
    uint8 clientid = 0;
    
    if(ataskid > 0xffff || ctaskid > 0xffff)
    {
        DPRINT("db client cheate error input, 0x%x, 0x%x\n", ataskid, ctaskid);
        return -1;
    }
    if(g_ATaskid > 0 && ataskid != g_ATaskid)
    {
        DPRINT("db client error: two agent 0x%x, 0x%x\n", g_ATaskid, ataskid);
        return -2;
    }

    if(AGENT_TYPE_THREAD == db_get_agent_type())
    {
        DBCLIENT_CACHE_T *ptdbClient = db_get_clientinfo(ctaskid, &clientid);
        if(NULL != ptdbClient)
        {
            /* 防止线程退出后死锁 */
            pthread_cond_signal(&(ptdbClient->notify));
            pthread_mutex_trylock(&(ptdbClient->lock));
            pthread_mutex_unlock(&(ptdbClient->lock));
            clientfd = (DB_CLIENT)clientid + 1;
        }
        else
        {
            clientfd = -1;
        }
    }
    else
    {
        clientfd = task_msg_init(ctaskid & 0xffff);

        if(clientfd > 0)
        {
            task_msg_settime(clientfd, 0, 100000);
        }
    }
    g_ATaskid = ataskid;
    return clientfd;
}

/**********************************************************************
* @name      : db_socket_create
* @brief     ：db操作句柄关闭
* @param[in] ：DB_CLIENT fd            客户端句柄  
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-25
* @Update    :
**********************************************************************/
void db_client_close(DB_CLIENT fd)
{
    if(AGENT_TYPE_THREAD == db_get_agent_type())
    {
        db_clear_clientinfo(db_get_clientinfo_byid(fd));
    }
    else
    {
        close(fd);
    }
}

/**********************************************************************
* @name      : db_read_nomal
* @brief     ：db操作读取普通数据 
* @param[in] ：DB_CLIENT fd            socket句柄
               NOMAL_OAD_T *ptOAD    OAD
               uint32 inMaxLen       最大缓存数量
               uint8 *outData        缓存
* @param[out]：uint32 *outLen        数据长度
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-24
* @Update    :
**********************************************************************/
int db_read_nomal(DB_CLIENT fd, NOMAL_OAD_T *ptOAD, uint32 inMaxLen, uint8 *outData, uint32 *outLen)
{
    int    ret = DBMSG_SUCCESS;
    uint8  recvbuf[TASK_MSG_MAX_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;
    
    if(fd <= 0 || NULL == ptOAD || 0 == inMaxLen || NULL == outData || NULL == outLen)
    {
        return DBMSG_ERR_INPUT;
    }
            
    if(AGENT_TYPE_THREAD == db_get_agent_type())  //线程级别内部交互
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;
        
        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }
        
        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_read_nomal_data(sendFunc, (uint16)pClientInfo->taskid, ptOAD, &index);
        DPRINT("Client 0x%x read normal data: oad 0x%x, index %u, ret %d\n", pClientInfo->taskid,
                ptOAD->oad.value, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_READ_NOMAL;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = TASK_MSG_MAX_LEN;
        
        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x read normal data time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        READ_NOMAL_REQ_T  tReq;
        TASK_MSG_t       *poutMsg = (TASK_MSG_t *)recvbuf;
        
        memcpy(&tReq.tOad, ptOAD, sizeof(NOMAL_OAD_T));
        tReq.maxLen = inMaxLen;

        ret = task_db_get_syncmsg(fd, MSG_DB_READ_NOMAL, (uint8 *)&tReq, sizeof(READ_NOMAL_REQ_T), g_ATaskid, 
                                  recvbuf, TASK_MSG_MAX_LEN, DB_OPERATE_OUT_TIME);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = (uint16)poutMsg->msglen;
        datastart = poutMsg->msg;
    }

    if(0 != ret)
    {
        return ret;
    }

    /* 数据处理 */
    uint8   bytes = 0;
    uint32  len = 0;
    uint32  oad = 0;
    uint16  classtag = 0;
    uint8  *pTemp = datastart;

    if (0 == recvlen || 1 != *pTemp)  //数量
    {
        return ERR_NOTEXIST;
    }
    
    pTemp++;
    appmsg_memcpy_r(&oad, pTemp, sizeof(uint32));
    pTemp += sizeof(uint32);
    if(oad != ptOAD->oad.value)
    {
        return ERR_INVALID;
    }

    appmsg_memcpy_r(&classtag, pTemp, sizeof(uint16));
    pTemp += sizeof(uint16);

    len = appmsg_get_len_offset(pTemp, &bytes); //计算octet-string的长度
    if(inMaxLen < len)
    {
        return ERR_OVERFLOW;
    }

    *outLen = len;
    if(len > 0)
    {
        memcpy(outData, pTemp + bytes, len);
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_wtite_nomal
* @brief     ：db操作写普通数据 
* @param[in] ：DB_CLIENT fd            socket句柄
               NOMAL_OAD_T *ptOAD    OAD
               uint8 *pData          数据
               uint32 datalen        数据长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-25
* @Update    :
**********************************************************************/
int db_write_nomal(DB_CLIENT fd, NOMAL_OAD_T *ptOAD, uint8 *pData, uint32 datalen)
{
    int    ret = DBMSG_SUCCESS;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;

    if(fd <= 0 || NULL == ptOAD || 0 == datalen || NULL == pData)
    {
        return DBMSG_ERR_INPUT;
    }

    if(AGENT_TYPE_THREAD == db_get_agent_type())  //线程级别内部交互
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;

        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }

        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_write_nomal_data(sendFunc, (uint16)pClientInfo->taskid, ptOAD, pData, datalen, &index);
        DPRINT("Client 0x%x write normal data: oad 0x%x, index %u, ret %d\n", pClientInfo->taskid, 
               ptOAD->oad.value, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_WRITE_NOMAL;
        pClientInfo->dbret = 0;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = SMALL_MSG_BUF_LEN;

        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x write normal data time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        WRITE_NOMAL_REQ_T *pReq = NULL;
        uint16             msgLen = 0;
        TASK_MSG_t        *pMsg = (TASK_MSG_t *)recvbuf;

        msgLen = sizeof(WRITE_NOMAL_REQ_T) + datalen;
        pReq = (WRITE_NOMAL_REQ_T *)malloc(msgLen);
        if(NULL == pReq)
        {
            return DBMSG_ERR_MALLOC;
        }
        memset(pReq, 0, msgLen);
        
        memcpy(&pReq->tOad, ptOAD, sizeof(NOMAL_OAD_T));
        pReq->dataLen = datalen;
        memcpy(pReq->data, pData, datalen);

        ret = task_db_get_syncmsg(fd, MSG_DB_WRITE_NOMAL, (uint8 *)pReq, msgLen, g_ATaskid, 
                                  recvbuf, SMALL_MSG_BUF_LEN, DB_OPERATE_OUT_TIME);
        free(pReq);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = (uint16)pMsg->msglen;
        datastart = pMsg->msg;
    }
    
    if(0 != ret)
    {
        return ret;
    }
    /* 数据处理 */
    if(0 == recvlen || TRUE != *datastart)
    {
        return ERR_INVALID;
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_read_nomal_more
* @brief     ：db操作读取普通数据(多条数据)
* @param[in] ：DB_CLIENT fd               DB客户端句柄
               NOMAL_OADS_T *ptOADs       OAD列表
               uint8 oadMaxNum            最大缓存数量
* @param[out]：NOMAL_OAD_DATE_T *outDatas 读取的普通数据缓存
               uint8 *dataNum             普通数据数量
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-27
* @Update    :
**********************************************************************/
int db_read_nomal_more(DB_CLIENT fd, NOMAL_OADS_T *ptOADs, uint8 oadMaxNum, NOMAL_OAD_DATE_T *outDatas, uint8 *dataNum)
{
    int    ret = DBMSG_SUCCESS;
    uint8  recvbuf[TASK_MSG_MAX_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;
    
    if(fd <= 0 || NULL == ptOADs || 0 == oadMaxNum || NULL == outDatas || NULL == dataNum)
    {
        return DBMSG_ERR_INPUT;
    }
            
    if(AGENT_TYPE_THREAD == db_get_agent_type())  //线程级别内部交互
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;
        
        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }
        
        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_read_nomal_datas(sendFunc, (uint16)pClientInfo->taskid, ptOADs, &index);
        DPRINT("Client 0x%x read normal datas: first oad 0x%x, index %u, ret %d\n", pClientInfo->taskid,
                ptOADs->oad[0].value, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_READ_NOMALS;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = TASK_MSG_MAX_LEN;
        
        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x read normal data time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        READ_NOMALS_REQ_T  tReq;
        TASK_MSG_t        *poutMsg = (TASK_MSG_t *)recvbuf;
        
        memcpy(&tReq.tOads, ptOADs, sizeof(NOMAL_OADS_T));

        ret = task_db_get_syncmsg(fd, MSG_DB_READ_NOMALS, (uint8 *)&tReq, sizeof(READ_NOMALS_REQ_T), g_ATaskid, 
                                  recvbuf, TASK_MSG_MAX_LEN, DB_OPERATE_OUT_TIME);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = (uint16)poutMsg->msglen;
        datastart = poutMsg->msg;
    }

    if(0 != ret)
    {
        return ret;
    }

    /* 数据处理 */
    uint8   bytes = 0;
    uint8   i = 0;
    uint8  *pTemp = datastart;

    if (0 == recvlen || 0 == *pTemp)
    {
        return ERR_NOTEXIST;
    }
    
    *dataNum = *pTemp;
    if(oadMaxNum < *dataNum)
    {
        return ERR_OVERFLOW;
    }

    pTemp++;
    for(i = 0; i < *dataNum; i++)
    {
        appmsg_memcpy_r(&(outDatas[i].oad), pTemp, sizeof(uint32));
        pTemp += sizeof(uint32);
        appmsg_memcpy_r(&(outDatas[i].classtag), pTemp, sizeof(uint16));
        pTemp += sizeof(uint16);
        
        outDatas[i].datalen = appmsg_get_len_offset(pTemp, &bytes); //计算octet-string的长度
        if(outDatas[i].datalen > DATA_LEN_MAX)
        {
            return ERR_OVERFLOW;
        }
        memcpy(outDatas[i].data, pTemp + bytes, outDatas[i].datalen);
        pTemp += bytes + outDatas[i].datalen;
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_write_nomal_more
* @brief     ：db操作开始读记录型数据(多条数据)
* @param[in] ：DB_CLIENT fd              DB客户端句柄
               uint8 logic               逻辑设备号
               uint16 info               信息点号
               NOMAL_OAD_DATE_T *inDatas 多条数据
               uint8 inDataNum           数据数量
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-27
* @Update    :
**********************************************************************/
int db_write_nomal_more(DB_CLIENT fd, uint8 logic, uint16 info, NOMAL_OAD_DATE_T *inDatas, uint8 inDataNum)
{
    int    ret = DBMSG_SUCCESS;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;

    if(fd <= 0 || NULL == inDatas || 0 == inDataNum)
    {
        return DBMSG_ERR_INPUT;
    }

    if(AGENT_TYPE_THREAD == db_get_agent_type())  //线程级别内部交互
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;

        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }

        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_write_nomal_datas(sendFunc, (uint16)pClientInfo->taskid, logic, info, inDatas, inDataNum, &index);
        DPRINT("Client 0x%x write normal datas: first oad 0x%x, index %u, ret %d\n", pClientInfo->taskid, 
               inDatas->oad.value, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_WRITE_NOMAL;
        pClientInfo->dbret = 0;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = SMALL_MSG_BUF_LEN;

        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x write normal data time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        /* 写多个记录长度太大，封装成playout格式发送 */
        uint16 msglen = 0;
        uint8  i = 0;
        uint16 offset = 0;
        uint8 *playload = NULL;
        uint8  bufLen[3] = {0};
        uint8  bytes = 0;
        uint16 tempclass = CLASS_DATA_NORMAL;
        TASK_MSG_t *poutMsg = (TASK_MSG_t *)recvbuf;

        msglen = 1 + 2 + 1;
        for(i = 0; i < inDataNum; i++)
        {
            bytes = appmsg_set_len_offset(inDatas[i].datalen, bufLen);
            msglen += 4 + 2 + bytes + inDatas[i].datalen;
        }
        
        playload = (uint8 *)malloc(msglen);
        if(NULL == playload)
        {
            return DBMSG_ERR_MALLOC;
        }
        memset(playload, 0, msglen);
    
        playload[offset++] = logic;                                   //逻辑设备号
        appmsg_memcpy_r(playload + offset, &(info), sizeof(uint16));  //信息点号
        offset += 2;
        playload[offset++] = inDataNum;                               //普通数据数量
    
        for(i = 0; i < inDataNum; i++)
        {
            appmsg_memcpy_r(playload + offset, &(inDatas[i].oad), 4); //数据项标识
            offset += 4;
    
            tempclass = (0 == inDatas[i].classtag) ? CLASS_DATA_NORMAL : inDatas[i].classtag;
            appmsg_memcpy_r(playload + offset, &tempclass, 2); //数据类标识
            offset += 2;
    
            bytes = appmsg_set_len_offset(inDatas[i].datalen, bufLen);
            memcpy(playload + offset, bufLen, bytes);
            offset += bytes;
            memcpy(playload + offset, inDatas[i].data, inDatas[i].datalen);
            offset += inDatas[i].datalen;
        }
        
        ret = task_db_get_syncmsg(fd, MSG_DB_WRITE_NOMALS, playload, msglen, g_ATaskid, 
                                  recvbuf, SMALL_MSG_BUF_LEN, DB_OPERATE_OUT_TIME);
        free(playload);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = (uint16)poutMsg->msglen;
        datastart = poutMsg->msg;
    }
    
    if(0 != ret)
    {
        return ret;
    }
    /* 数据处理 */
    if(0 == recvlen || TRUE != *datastart)
    {
        return ERR_INVALID;
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_read_record_start
* @brief     ：db操作开始读记录型数据
* @param[in] ：DB_CLIENT fd             socket句柄
               READ_RECORD_T *ptRecord  记录条件      主OAD为0表示非复合型数据
* @param[out]：uint32 *handle           数据中心句柄
               uint32 *recNum           记录条数
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_read_record_start(DB_CLIENT fd, READ_RECORD_T *ptRecord, uint32 *handle, uint32 *recNum)
{
    int  ret = DBMSG_SUCCESS;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;
    
    if(fd <= 0 || NULL == ptRecord || NULL == handle || NULL == recNum)
    {
        return DBMSG_ERR_INPUT;
    }

    if(AGENT_TYPE_THREAD == db_get_agent_type()) //线程级别内部交互
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;

        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }

        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_read_record_start(sendFunc, (uint16)pClientInfo->taskid, ptRecord, &index);
        DPRINT("Client 0x%x start read record: main oad 0x%x, index %u, ret %d\n", pClientInfo->taskid, 
                ptRecord->recordOAD.road.oadMain.value, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_READ_RECORD_START;
        pClientInfo->dbret = 0;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = SMALL_MSG_BUF_LEN;

        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x start read record time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }

        recvlen = pClientInfo->datalen;
        datastart = recvbuf;
        if(0 == recvlen)  ret = ERR_INVALID;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        READ_RECORD_START_REQ_T  tReq;
        TASK_MSG_t              *pMsg = (TASK_MSG_t *)recvbuf;
        
        memset(&tReq, 0, sizeof(READ_RECORD_START_REQ_T));
        memcpy(&tReq.tRecord, ptRecord, sizeof(READ_RECORD_T));
        ret = task_db_get_syncmsg(fd, MSG_DB_READ_RECORD_START, (uint8 *)&tReq, sizeof(READ_RECORD_START_REQ_T), 
                                  g_ATaskid, recvbuf, SMALL_MSG_BUF_LEN, DB_OPERATE_OUT_TIME);
        if(0 != ret)
        {
            return ret;
        }
        datastart = pMsg->msg;
        recvlen = pMsg->msglen;
        if(0 == recvlen)  ret = ERR_INVALID;
    }
    if(0 != ret)
    {
        return ret;
    }
    /* 数据处理 */
    appmsg_memcpy_r(handle, datastart, sizeof(uint32));
    appmsg_memcpy_r(recNum, datastart + sizeof(uint32), sizeof(uint32));
    return ret;
}

/**********************************************************************
* @name      : db_read_record_step
* @brief     ：db操作单步读记录型数据
* @param[in] ：DB_CLIENT fd                 socket句柄
               uint32 handle                数据中心句柄
               STEP_READ_RECORD_T *ptRecord 数据
* @param[out]：RECORD_UNIT_T *stepRecordOut 单步数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_read_record_step(DB_CLIENT fd, uint32 handle, STEP_READ_RECORD_T *ptRecord, RECORD_UNIT_T *stepRecordOut)
{
    int  ret = DBMSG_SUCCESS;
    uint8  recvbuf[TASK_MSG_MAX_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;
    
    if(fd <= 0 || NULL == ptRecord || NULL == stepRecordOut)
    {
        return DBMSG_ERR_INPUT;
    }

    if(AGENT_TYPE_THREAD == db_get_agent_type())  //线程级别内部交互
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;

        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }

        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_read_record_step(sendFunc, (uint16)pClientInfo->taskid, handle, ptRecord, &index);
        DPRINT("Client 0x%x step read record: handle %u, index %u, ret %d\n", pClientInfo->taskid, 
                handle, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_READ_RECORD_STEP;
        pClientInfo->dbret = 0;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = TASK_MSG_MAX_LEN;

        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x step read record time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
        if(0 == recvlen)  ret = ERR_INVALID;
        if(recvlen > TASK_MSG_MAX_LEN) {ret = ERR_OVERFLOW;}
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        READ_RECORD_STEP_REQ_T  tReq;
        TASK_MSG_t             *pMsg = (TASK_MSG_t *)recvbuf;
        
        tReq.handle = handle;
        memcpy(&tReq.tStepRecord, ptRecord, sizeof(STEP_READ_RECORD_T));
        ret = task_db_get_syncmsg(fd, MSG_DB_READ_RECORD_STEP, (uint8 *)&tReq, sizeof(READ_RECORD_STEP_REQ_T), 
                                  g_ATaskid, recvbuf, TASK_MSG_MAX_LEN, DB_OPERATE_OUT_TIME);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = pMsg->msglen;
        datastart = pMsg->msg;
        if(0 == recvlen)  ret = ERR_INVALID;
    }
    if(0 != ret)
    {
        return ret;
    }
    
    /* 数据处理 */
    uint16  offset = 0;
    uint16  i = 0;
    uint8   bytes = 0;
    RECORD_DATA_T *ptData = NULL;
    
    memset(stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    if(1 == datastart[offset++])
    {
        appmsg_memcpy_r(&(stepRecordOut->infoNum), datastart + offset, sizeof(uint16));   //信息点号          
        offset += sizeof(uint16);
        
        stepRecordOut->recordnum = appmsg_get_len_offset(datastart + offset, &bytes);     //记录数量
        offset += bytes;

        if(stepRecordOut->recordnum > RECORD_DATA_MAX_NUM)
        {
            return ERR_OVERFLOW;
        }
        
        for(i = 0; i < stepRecordOut->recordnum; i++)
        {
            ptData = &(stepRecordOut->record[i]);

            ptData->optional = datastart[offset++];
            if(1 == ptData->optional)
            {
                appmsg_memcpy_r(&(ptData->road.oadMain.value), datastart + offset, sizeof(uint32));//主数据DI
                offset += sizeof(uint32);
            }
            else if(0 == ptData->optional)
            {
                ptData->road.oadMain.value = 0;                                                    //主数据DI
            }
            else
            {
                memset(stepRecordOut, 0, sizeof(RECORD_UNIT_T));//格式不对，返回数据清0，长度为0
                return ERR_SETP_REC;
            }

            ptData->road.oadCols.nNum = 0x01;
            appmsg_memcpy_r(&(ptData->road.oadCols.oad[0].value), datastart + offset, sizeof(uint32));//次数据DI
            offset += sizeof(uint32);

            ptData->outDatalen = appmsg_get_len_offset(datastart + offset, &bytes); //长度
            offset += bytes;

            if(ptData->outDatalen > DATA_LEN_MAX)
            {
                return ERR_OVERFLOW;
            }
            
            memcpy(ptData->outDataBuf, datastart + offset, ptData->outDatalen);     //数据
            offset += ptData->outDatalen;
        }

        if(1 == datastart[offset])
        {
            offset++;
            appmsg_memcpy_r(&(stepRecordOut->colStartTm), datastart + offset, sizeof(uint32));//采集起始时间
            offset += sizeof(uint32);
            
            appmsg_memcpy_r(&(stepRecordOut->colEndTm), datastart + offset, sizeof(uint32));  //采集结束时间
            offset += sizeof(uint32);
            
            appmsg_memcpy_r(&(stepRecordOut->colStoreTm), datastart + offset, sizeof(uint32));//采集存储时间
            offset += sizeof(uint32);
        }
    }
    else
    {
        return ERR_NOTEXIST;
    }

    return ret;
}

/**********************************************************************
* @name      : db_read_record_end
* @brief     ：db操作结束记录型数据读取
* @param[in] ：DB_CLIENT fd                   socket句柄
               uint32 handle                数据中心句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_read_record_end(DB_CLIENT fd, uint32 handle)
{
    int  ret = DBMSG_SUCCESS;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;
    
    if(fd <= 0)
    {
        return DBMSG_ERR_INPUT;
    }
            
    if(AGENT_TYPE_THREAD == db_get_agent_type())  //线程级别内部交互
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;

        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }

        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_read_record_end(sendFunc, (uint16)pClientInfo->taskid, handle, &index);
        DPRINT("Client 0x%x end read record: handle %u, index %u, ret %d\n", pClientInfo->taskid, 
               handle, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_READ_RECORD_END;
        pClientInfo->dbret = 0;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = SMALL_MSG_BUF_LEN;

        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x end read record time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        READ_RECORD_END_REQ_T  tReq;
        TASK_MSG_t            *pMsg = (TASK_MSG_t *)recvbuf;

        tReq.handle = handle;
        ret = task_db_get_syncmsg(fd, MSG_DB_READ_RECORD_END, (uint8 *)&tReq, sizeof(READ_RECORD_END_REQ_T), g_ATaskid, 
                                  recvbuf, SMALL_MSG_BUF_LEN, DB_OPERATE_OUT_TIME);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = pMsg->msglen;
        datastart = pMsg->msg;
    }
    if(0 != ret)
    {
        return ret;
    }
    /* 数据处理 */
    if(0 == recvlen || TRUE != *datastart)
    {
        return ERR_INVALID;
    }
    return ret;
}

/**********************************************************************
* @name      : db_read_record
* @brief     ：db操作读最近的一条记录
* @param[in] ：DB_CLIENT fd             socket句柄
               READ_RECORD_T *ptRecord  记录条件
               uint32 bufLen            最大读取buf长度
* @param[out]：RECORD_UNIT_T *recordOut 数据记录
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_read_record(DB_CLIENT fd, READ_RECORD_T *ptRecord, uint32 bufLen, RECORD_UNIT_T *recordOut)
{
    int        ret = DBMSG_SUCCESS;
    READ_RECORD_T      inReadRecordTmp;
    STEP_READ_RECORD_T stepReadRecord;
    uint32 handle;
    uint32 recNum;
    int32  nRet = 0;
    
    if(fd <= 0 || NULL == ptRecord || NULL == recordOut)
    {
        return DBMSG_ERR_INPUT;
    }

    if (bufLen > DATA_LEN_MAX)
    {
        return ERR_O_RANGE;
    }

    /* 只读最近一条 */
    inReadRecordTmp = *ptRecord;
    inReadRecordTmp.sortType = 0x01;

    ret = db_read_record_start(fd, &inReadRecordTmp, &handle, &recNum);
    if (0 != ret)
    {
        return (0x10000000 + ret);
    }
    if(0 == recNum)
    {
        return ERR_NORESOURCE;
    }
    
    memcpy(&stepReadRecord.MAC, &inReadRecordTmp.MAC, sizeof(OOP_OCTETVAR64_T));

    /* 读最近一条记录 */
    ret = db_read_record_step(fd, handle, &stepReadRecord, recordOut);
    if (0 != ret)
    {
        return ret;
    }

    /*结束读记录*/
    ret = db_read_record_end(fd, handle);
    if (0 != ret)
    {
        return (0x20000000 + ret);
    }

    return nRet;
}

/**********************************************************************
* @name      : db_write_record
* @brief     ：db操作写记录型数据
* @param[in] ：DB_CLIENT fd             socket句柄
               WRITE_RECORD_T *ptRecord 记录
* @param[out]：uint16 *storeNo          存储序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_write_record(DB_CLIENT fd, WRITE_RECORD_T *ptRecord, uint32 *storeNo)
{
    int    ret = DBMSG_SUCCESS;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;
    
    if(fd <= 0 || NULL == ptRecord || NULL == storeNo)
    {
        return DBMSG_ERR_INPUT;
    }

    if(AGENT_TYPE_THREAD == db_get_agent_type())   //线程级别内部交互
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;

        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }

        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_write_record(sendFunc, (uint16)pClientInfo->taskid, ptRecord, &index);
        DPRINT("Client 0x%x write record: main oad 0x%x, index %u, ret %d\n", pClientInfo->taskid, 
               ptRecord->recordOAD.road.oadMain.value, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_WRITE_RECORD;
        pClientInfo->dbret = 0;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = SMALL_MSG_BUF_LEN;

        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x write record time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
        if(0 == recvlen)  ret = ERR_INVALID;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        WTITE_RECORD_REQ_T  tReq;
        TASK_MSG_t         *pMsg = (TASK_MSG_t *)recvbuf;
        
        memcpy(&tReq.tRecord, ptRecord, sizeof(WRITE_RECORD_T));
        ret = task_db_get_syncmsg(fd, MSG_DB_WRITE_RECORD, (uint8 *)&tReq, sizeof(WTITE_RECORD_REQ_T), g_ATaskid, 
                                  recvbuf, SMALL_MSG_BUF_LEN, DB_OPERATE_OUT_TIME);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = (uint16)pMsg->msglen;
        datastart = pMsg->msg;
        if(0 == recvlen)  ret = ERR_INVALID;
    }
    
    if(0 != ret)
    {
        return ret;
    }
    appmsg_memcpy_r(storeNo, datastart, sizeof(uint32));
    
    return ret;
}

/**********************************************************************
* @name      : db_write_records
* @brief     ：db操作写记录型数据
* @param[in] ：DB_CLIENT fd             socket句柄
               WRITE_RECORD_T *ptRecord 记录
* @param[out]：uint16 *storeNo          存储序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-3
* @Update    :
**********************************************************************/
int db_write_records(DB_CLIENT fd, WRITE_RECORDS_T *ptRecord, uint32 *storeNo)
{
    int    ret = DBMSG_SUCCESS;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;
    
    if(fd <= 0 || NULL == ptRecord || NULL == storeNo)
    {
        return DBMSG_ERR_INPUT;
    }

    if(AGENT_TYPE_THREAD == db_get_agent_type())   //线程级别内部交互
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;

        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }

        pthread_mutex_lock(&pClientInfo->lock);
        pClientInfo->msgindex += 1;
        ret = send_write_records(sendFunc, (uint16)pClientInfo->taskid, ptRecord, &index);
        DPRINT("Client 0x%x write records: main oad 0x%x, index %u, ret %d\n", pClientInfo->taskid, 
               ptRecord->recordOAD.road.oadMain.value, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_WRITE_RECORD;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = SMALL_MSG_BUF_LEN;

        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x write record time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
        if(0 == recvlen)  ret = ERR_INVALID;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        WTITE_RECORD_REQ_T  tReq;
        TASK_MSG_t         *pMsg = (TASK_MSG_t *)recvbuf;
        
        memcpy(&tReq.tRecord, ptRecord, sizeof(WRITE_RECORD_T));
        ret = task_db_get_syncmsg(fd, MSG_DB_WRITE_RECORD, (uint8 *)&tReq, sizeof(WTITE_RECORD_REQ_T), g_ATaskid, 
                                  recvbuf, SMALL_MSG_BUF_LEN, DB_OPERATE_OUT_TIME);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = (uint16)pMsg->msglen;
        datastart = pMsg->msg;
        if(0 == recvlen)  ret = ERR_INVALID;
    }
    
    if(0 != ret)
    {
        return ret;
    }
    appmsg_memcpy_r(storeNo, datastart, sizeof(uint32));
    
    return ret;
}

/**********************************************************************
* @name      : db_update_record
* @brief     ：db操作更新记录数据
* @param[in] ：DB_CLIENT fd              socket句柄
               UPDATE_RECORD_T *ptRecord 数据  主OAD为0表示非复合型数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_update_record(DB_CLIENT fd, UPDATE_RECORD_T *ptRecord)
{
    int   ret = DBMSG_SUCCESS;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;
    
    if(fd <= 0 || NULL == ptRecord)
    {
        return DBMSG_ERR_INPUT;
    }

    if(AGENT_TYPE_THREAD == db_get_agent_type())
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;

        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }

        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_update_record(sendFunc, (uint16)pClientInfo->taskid, ptRecord, &index);
        DPRINT("Client 0x%x update record: main oad 0x%x, index %u, ret %d\n", pClientInfo->taskid, 
               ptRecord->recordOAD.road.oadMain.value, index, ret);

        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_UPDATE_RECORD;
        pClientInfo->dbret = 0;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = SMALL_MSG_BUF_LEN;

        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x update record time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        UPDATE_RECORD_REQ_T  tReq;
        TASK_MSG_t          *pMsg = (TASK_MSG_t *)recvbuf;
        
        memcpy(&tReq.tRecord, ptRecord, sizeof(UPDATE_RECORD_T));
        ret = task_db_get_syncmsg(fd, MSG_DB_UPDATE_RECORD, (uint8 *)&tReq, sizeof(UPDATE_RECORD_REQ_T), g_ATaskid, 
                                  recvbuf, SMALL_MSG_BUF_LEN, DB_OPERATE_OUT_TIME);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = pMsg->msglen;
        datastart = pMsg->msg;
    }
    if(0 != ret)
    {
        return ret;
    }
    /* 数据处理 */
    if(0 == recvlen || TRUE != *datastart)
    {
        return ERR_INVALID;
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_get_record_num
* @brief     ：db操作获取存储记录条数
* @param[in] ：DB_CLIENT fd             socket句柄
               OOP_ROAD_T *pRecord      条件
* @param[out]：uint32 *recNum           数量
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_get_record_num(DB_CLIENT fd, GET_RECORD_NUM_T *pRecord, uint32 *recNum)
{
    int   ret = DBMSG_SUCCESS;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;

    if(fd <= 0 || NULL == pRecord || NULL == recNum)
    {
        return DBMSG_ERR_INPUT;
    }

    if(AGENT_TYPE_THREAD == db_get_agent_type())
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;

        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }

        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_get_record_num(sendFunc, (uint16)pClientInfo->taskid, pRecord, &index);
        DPRINT("Client 0x%x get record num: main oad 0x%x, index %u, ret %d\n", pClientInfo->taskid, 
               pRecord->recordOAD.road.oadMain.value, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_GET_RECORD_NUM;
        pClientInfo->dbret = 0;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = SMALL_MSG_BUF_LEN;

        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x get record num time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
        if(0 == recvlen)  ret = ERR_INVALID;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        GET_RECORDNUM_REQ_T  tReq;
        TASK_MSG_t       *pMsg = (TASK_MSG_t *)recvbuf;

        memcpy(&tReq.tRecord, pRecord, sizeof(GET_RECORD_NUM_T));
        ret = task_db_get_syncmsg(fd, MSG_DB_GET_RECORD_NUM, (uint8 *)&tReq, sizeof(GET_RECORDNUM_REQ_T), g_ATaskid, 
                                  recvbuf, SMALL_MSG_BUF_LEN, DB_OPERATE_OUT_TIME);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = pMsg->msglen;
        datastart = pMsg->msg;
        if(0 == recvlen)  ret = ERR_INVALID;
    }
    if(0 != ret)
    {
        return ret;
    }
    /* 数据处理 */
    appmsg_memcpy_r(recNum, datastart, sizeof(uint32));
    return ret;
}

/**********************************************************************
* @name      : db_get_record_depth
* @brief     ：db操作获取存储记录深度
* @param[in] ：DB_CLIENT fd             socket句柄
               OOP_ROAD_T *ptRroad      条件
* @param[out]：uint32 *recNum           数量
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_get_record_depth(DB_CLIENT fd, RECORD_OAD_T *ptRroad, uint32 *depthNum)
{
    int   ret = DBMSG_SUCCESS;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;

    if(fd <= 0 || NULL == ptRroad || NULL == depthNum)
    {
        return DBMSG_ERR_INPUT;
    }

    if(AGENT_TYPE_THREAD == db_get_agent_type())
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;

        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }

        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_get_record_depth(sendFunc, (uint16)pClientInfo->taskid, ptRroad, &index);
        DPRINT("Client 0x%x get record depth: main oad 0x%x, index %u, ret %d\n", pClientInfo->taskid, 
               ptRroad->road.oadMain.value, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_GET_RECORD_DEPTH;
        pClientInfo->dbret = 0;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = SMALL_MSG_BUF_LEN;

        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x get record depth time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
        if(0 == recvlen)  ret = ERR_INVALID;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        GET_RECORD_REQ_T  tReq;
        TASK_MSG_t       *pMsg = (TASK_MSG_t *)recvbuf;
        
        memcpy(&tReq.tRoad, ptRroad, sizeof(RECORD_OAD_T));
        ret = task_db_get_syncmsg(fd, MSG_DB_GET_RECORD_DEPTH, (uint8 *)&tReq, sizeof(GET_RECORD_REQ_T), g_ATaskid, 
                                  recvbuf, SMALL_MSG_BUF_LEN, DB_OPERATE_OUT_TIME);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = pMsg->msglen;
        datastart = pMsg->msg;
        if(0 == recvlen)  ret = ERR_INVALID;
    }
    if(0 != ret)
    {
        return ret;
    }
    /* 数据处理 */
    appmsg_memcpy_r(depthNum, datastart, sizeof(uint32));
    return ret;
}

/**********************************************************************
* @name      : db_clear_data
* @brief     ：db操作数据初始化
* @param[in] ：DB_CLIENT fd         socket句柄
               CLEAR_DATA_T *ptData 清除选项
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_clear_data(DB_CLIENT fd, CLEAR_DATA_T *ptData)
{
    int   ret = DBMSG_SUCCESS;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;

    if(fd <= 0 || NULL == ptData)
    {
        return DBMSG_ERR_INPUT;
    }

    if(AGENT_TYPE_THREAD == db_get_agent_type())
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;

        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }

        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_clear_data(sendFunc, (uint16)pClientInfo->taskid, ptData, &index);
        DPRINT("Client 0x%x clear data: logicId %d, index %u, ret %d\n", pClientInfo->taskid, 
                ptData->recordOAD.logicId, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_CLEAR_DATA;
        pClientInfo->dbret = 0;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = SMALL_MSG_BUF_LEN;

        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += 5 * DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x clear data time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        CLEAR_DATA_REQ_T  tReq;
        TASK_MSG_t       *pMsg = (TASK_MSG_t *)recvbuf;

        memcpy(&tReq.tData, ptData, sizeof(CLEAR_DATA_T));
        ret = task_db_get_syncmsg(fd, MSG_DB_CLEAR_DATA, (uint8 *)&tReq, sizeof(CLEAR_DATA_REQ_T), g_ATaskid, 
                                  recvbuf, SMALL_MSG_BUF_LEN, 2 * DB_OPERATE_OUT_TIME);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = pMsg->msglen;
        datastart = pMsg->msg;
    }
    if(0 != ret)
    {
        return ret;
    }
    /* 数据处理 */
    if(0 == recvlen || TRUE != *datastart)
    {
        return ERR_INVALID;
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_clear_oad_data
* @brief     ：db操作指定数据初始化
* @param[in] ：DB_CLIENT fd         socket句柄
               CLEAR_DATA_T *ptData 清除选项
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_clear_oad_data(DB_CLIENT fd, CLEAR_DATA_T *ptData)
{
    int   ret = DBMSG_SUCCESS;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;

    if(fd <= 0 || NULL == ptData)
    {
        return DBMSG_ERR_INPUT;
    }

    if(AGENT_TYPE_THREAD == db_get_agent_type())
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;

        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }

        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_clear_oad_data(sendFunc, (uint16)pClientInfo->taskid, ptData, &index);
        DPRINT("Client 0x%x clear oaddata: main oad 0x%x, index %u, ret %d\n", pClientInfo->taskid, 
               ptData->recordOAD.road.oadMain.value, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_CLEAR_OAD_DATA;
        pClientInfo->dbret = 0;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = SMALL_MSG_BUF_LEN;

        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += 5 * DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x clear oaddata time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        CLEAR_DATA_REQ_T  tReq;
        TASK_MSG_t       *pMsg = (TASK_MSG_t *)recvbuf;

        memcpy(&tReq.tData, ptData, sizeof(CLEAR_DATA_T));
        ret = task_db_get_syncmsg(fd, MSG_DB_CLEAR_OAD_DATA, (uint8 *)&tReq, sizeof(CLEAR_DATA_REQ_T), g_ATaskid, 
                                  recvbuf, SMALL_MSG_BUF_LEN, 2 * DB_OPERATE_OUT_TIME);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = pMsg->msglen;
        datastart = pMsg->msg;
    }
    if(0 != ret)
    {
        return ret;
    }
    /* 数据处理 */
    if(0 == recvlen || TRUE != *datastart)
    {
        return ERR_INVALID;
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_recover_default
* @brief     ：db操作恢复出厂
* @param[in] ：DB_CLIENT fd       socket句柄
               uint8 logicId      逻辑设备号
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_recover_default(DB_CLIENT fd, uint8 logicId)
{
    int   ret = DBMSG_SUCCESS;
    uint8  recvbuf[SMALL_MSG_BUF_LEN] = {0};
    uint8 *datastart = NULL;
    uint16 recvlen = 0;

    if(fd <= 0)
    {
        return DBMSG_ERR_INPUT;
    }

    if(AGENT_TYPE_THREAD == db_get_agent_type())
    {
        DBMSG_SEND_FUN    sendFunc = db_get_send_func();
        DBCLIENT_CACHE_T *pClientInfo = db_get_clientinfo_byid(fd);
        uint16            index = 0;
        struct timespec   outtime;
        int               rettime = 0;

        if(NULL == pClientInfo || NULL == sendFunc)
        {
            return DBMSG_ERR_INPUT;
        }

        pthread_mutex_lock(&pClientInfo->lock);
        ret = send_recover_default(sendFunc, (uint16)pClientInfo->taskid, logicId, &index);
        DPRINT("Client 0x%x recover default: logicId %d, index %u, ret %d\n", pClientInfo->taskid,
                logicId, index, ret);
        if(0 != ret)
        {
            pthread_mutex_unlock(&pClientInfo->lock);
            return ret;
        }

        pClientInfo->msgindex = index;
        pClientInfo->bwait = TRUE;
        pClientInfo->msgid = MSG_DB_RECOVER_DEFAULT;
        pClientInfo->dbret = 0;
        pClientInfo->data = recvbuf;
        pClientInfo->datalen = 0;
        pClientInfo->ackbuflen = SMALL_MSG_BUF_LEN;

        /* 设置超时等待 */
        clock_gettime(CLOCK_MONOTONIC, &outtime);
        outtime.tv_sec += 5 * DB_OPERATE_OUT_TIME;
        rettime = pthread_cond_timedwait(&pClientInfo->notify, &pClientInfo->lock, &outtime);
        if(0 != rettime)
        {
            /* 超时 */
            DPRINT("Client 0x%x recover default time out\n", pClientInfo->taskid);
            ret = (ETIMEDOUT == rettime) ? DBMSG_ERR_OUT_TIME : DBMSG_ERR_UNKNOW;
            goto goret;
        }
        recvlen = (uint16)pClientInfo->datalen;
        datastart = recvbuf;
goret:
        pClientInfo->bwait = FALSE;
        pClientInfo->data = NULL;
        pthread_mutex_unlock(&pClientInfo->lock);
    }
    else
    {
        TASK_MSG_t      *pMsg = (TASK_MSG_t *)recvbuf;
        
        ret = task_db_get_syncmsg(fd, MSG_DB_RECOVER_DEFAULT, (uint8 *)&logicId, sizeof(uint8), g_ATaskid, 
                                  recvbuf, SMALL_MSG_BUF_LEN, 2 * DB_OPERATE_OUT_TIME);
        if(0 != ret)
        {
            return ret;
        }
        recvlen = pMsg->msglen;
        datastart = pMsg->msg;
    }
    if(0 != ret)
    {
        return ret;
    }
    /* 数据处理 */
    if(0 == recvlen || TRUE != *datastart)
    {
        return ERR_INVALID;
    }
    
    return ret;
}

