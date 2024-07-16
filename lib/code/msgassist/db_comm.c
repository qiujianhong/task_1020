/*
*********************************************************************
* @filename: db_comm.c
* @brief   : 消息通信 DB操作
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2019-12-27    create
*********************************************************************
*/
#include "baseCommon.h"
#include "debug_comm.h"
#include "db_comm.h"
#include "appmsg_split.h"

uint8             g_DBClientNum = 0;
uint8             g_DBClientMAX = 0;
pthread_mutex_t   g_Clientlock;          //客户端信息锁
DBCLIENT_CACHE_T *g_pDBClientInfo = NULL;


/**********************************************************************
* @name      : db_clientinfo_init
* @brief     ：db客户端信息初始化
* @param[in] ：uint8 clientnum     客户端容量 
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_clientinfo_init(uint8 clientnum)
{
    g_pDBClientInfo = (DBCLIENT_CACHE_T *)malloc(clientnum * sizeof(DBCLIENT_CACHE_T));
    if(NULL == g_pDBClientInfo)
    {
        return DBMSG_ERR_MALLOC;
    }

    memset(g_pDBClientInfo, 0, clientnum * sizeof(DBCLIENT_CACHE_T));
    g_DBClientMAX = clientnum;
    g_DBClientNum = 0;
    pthread_mutex_init(&g_Clientlock, NULL);
    return DBMSG_SUCCESS;
}

/**********************************************************************
* @name      : db_clientinfo_free
* @brief     ：db客户端信息释放
* @param[in] ：uint8 clientnum     客户端容量 
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
void db_clientinfo_free()
{
    uint8 i;
    DBCLIENT_CACHE_T *ptInfo = NULL;
    
    if(g_pDBClientInfo)
    {
        pthread_mutex_lock(&g_Clientlock);
        for(i = 0; i < g_DBClientNum; i++)
        {
            ptInfo = &(g_pDBClientInfo[i]);
            pthread_mutex_lock(&(ptInfo->lock));
            pthread_mutex_destroy(&(ptInfo->lock));
            pthread_cond_destroy(&(ptInfo->notify));
        }
        free(g_pDBClientInfo);
        g_pDBClientInfo = NULL;
        g_DBClientMAX = 0;
        g_DBClientNum = 0;
        pthread_mutex_destroy(&g_Clientlock);
    }
}

/**********************************************************************
* @name      : db_get_clientinfo
* @brief     ：获取一个client的信息, 不存在就分配一个
* @param[in] ：uint8 clientnum     客户端容量 
* @param[out]：uint8 *clientid     内容索引
* @return    ：DBCLIENT_CACHE_T *  客户端内容
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
DBCLIENT_CACHE_T *db_get_clientinfo(uint32 taskid, uint8 *clientid)
{
    uint8 i;
    pthread_condattr_t  tAttr;

    if(0 == g_DBClientMAX)
    {
        DPRINT("dbAgent not init!\n");
        return NULL;
    }

    pthread_mutex_lock(&g_Clientlock);

    for(i = 0; i < g_DBClientNum; i++)
    {
        if(taskid == g_pDBClientInfo[i].taskid)
        {
            if(clientid)
            {
                *clientid = i;
            }
            pthread_mutex_unlock(&g_Clientlock);
            return &(g_pDBClientInfo[i]);
        }
    }
    if(g_DBClientNum >= g_DBClientMAX)
    {
        DPRINT("dbClient out of maxsize!\n");
        pthread_mutex_unlock(&g_Clientlock);
        return NULL;
    }
    
    pthread_mutex_init(&(g_pDBClientInfo[i].lock), NULL);

    /* pthread_cond_timedwait相对时间模式 */
    pthread_condattr_setclock(&tAttr, CLOCK_MONOTONIC);
    pthread_cond_init(&(g_pDBClientInfo[i].notify), &tAttr);

    g_pDBClientInfo[i].taskid = taskid;
    g_DBClientNum++;
    
    if(clientid)
    {
        *clientid = i;
    }

    pthread_mutex_unlock(&g_Clientlock);
    return &(g_pDBClientInfo[i]);
}

/**********************************************************************
* @name      : db_get_clientinfo_byid
* @brief     ：获取一个client的信息
* @param[in] ：DB_CLIENT clientid     内容索引
* @param[out]：
* @return    ：DBCLIENT_CACHE_T *  客户端内容
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
DBCLIENT_CACHE_T *db_get_clientinfo_byid(DB_CLIENT clientid)
{
    if(clientid > g_DBClientNum || clientid <= 0)
    {
        return NULL;
    }
    return &(g_pDBClientInfo[clientid - 1]);
}

/**********************************************************************
* @name      : db_clear_clientinfo
* @brief     ：清空客户端关键信息
* @param[in] ：DBCLIENT_CACHE_T *ptInfo  客户端内容
* @param[out]：
* @return    ：DBCLIENT_CACHE_T *  客户端内容
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
void db_clear_clientinfo(DBCLIENT_CACHE_T *ptInfo)
{
    if(NULL == ptInfo)
    {
        return;
    }
    pthread_mutex_lock(&(ptInfo->lock));
    ptInfo->ackbuflen = 0;
    ptInfo->bwait = 0;
    ptInfo->msgindex = 0;
    ptInfo->taskindex = 0;
    pthread_mutex_unlock(&(ptInfo->lock));
}

/**********************************************************************
* @name      : send_read_nomal_data
* @brief     ：发送查询普通数据消息
* @param[in] ：DBMSG_SEND_FUN func    发送回调函数
               uint16 label           消息label
               NOMAL_OAD_T *nomalOAD  oad
* @param[out]：uint16 *index          发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    : 2020-4-28
**********************************************************************/
int send_read_nomal_data(DBMSG_SEND_FUN func, uint16 label, NOMAL_OAD_T *nomalOAD, uint16 *index)
{
    uint16      bufLen = 0;
    uint8       playBuf[10] = {0};

    playBuf[bufLen++] = nomalOAD->logicId;                                  //逻辑设备号
    appmsg_memcpy_r(playBuf + bufLen, &(nomalOAD->infoNum), sizeof(uint16));//信息点号
    bufLen += 2;
    playBuf[bufLen++] = 1;                                                  //数量
    appmsg_memcpy_r(playBuf + bufLen, &(nomalOAD->oad), sizeof(OOP_OAD_U)); //数据项标识
    bufLen += sizeof(OOP_OAD_U);

    return func(label, DB_IOP_READ_NORMAL_DATA, playBuf, bufLen, index);
}

/**********************************************************************
* @name      : send_write_nomal_data
* @brief     ：发送写普通数据消息
* @param[in] ：DBMSG_SEND_FUN func    发送回调函数
               uint16 label           消息label
               NOMAL_OAD_T *nomalOAD  oad
               uint8 *pData           内容
               uint32 dataLen         长度
* @param[out]：uint16 *index          发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int send_write_nomal_data(DBMSG_SEND_FUN func, uint16 label, NOMAL_OAD_T *nomalOAD, uint8 *pData, 
                          uint32 dataLen, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    bytes = appmsg_set_len_offset((uint16)dataLen, bufLen);
    msglen = 1 + 2 + 1 + 4 + 2 + bytes + dataLen;
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    playload[i++] = nomalOAD->logicId;                                   //逻辑设备号
    appmsg_memcpy_r(playload + i, &(nomalOAD->infoNum), sizeof(uint16)); //信息点号
    i += 2;
    playload[i++] = 1;                                                   //普通数据数量
    appmsg_memcpy_r(playload + i, &(nomalOAD->oad), sizeof(OOP_OAD_U));  //数据项标识
    i += sizeof(OOP_OAD_U);
    if(0 == nomalOAD->classtag)
    {
        uint16 tempclass = CLASS_DATA_NORMAL;
        appmsg_memcpy_r(playload + i, &tempclass, sizeof(uint16));//数据类标识 为0，填写成CLASS_DATA_NORMAL
    }
    else
    {
        appmsg_memcpy_r(playload + i, &(nomalOAD->classtag), sizeof(uint16));//数据类标识
    }
    i += 2;

    /* 数据缓冲 */
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, pData, dataLen);
    i += dataLen;
    
    ret = func(label, DB_IOP_WRITE_NORMAL_DATA, playload, msglen, index);

    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_read_nomal_datas
* @brief     ：发送查询多个普通数据消息
* @param[in] ：DBMSG_SEND_FUN func     发送回调函数
               uint16 label            消息label
               NOMAL_OADS_T *nomalOADs oad列表
* @param[out]：uint16 *index           发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2020-4-28
* @Update    : 
**********************************************************************/
int send_read_nomal_datas(DBMSG_SEND_FUN func, uint16 label, NOMAL_OADS_T *nomalOADs, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 offset = 0;
    uint8 *playload = NULL;
    uint8  i = 0;

    /* 计算长度 */
    msglen = 1 + 2 + 1 + 4 * nomalOADs->num;
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    playload[offset++] = nomalOADs->logicId;                                  //逻辑设备号
    appmsg_memcpy_r(playload + offset, &(nomalOADs->infoNum), sizeof(uint16));//信息点号
    offset += 2;
    playload[offset++] = nomalOADs->num;                                      //数量
    
    for(i = 0; i < nomalOADs->num; i++)
    {
        appmsg_memcpy_r(playload + offset, &(nomalOADs->oad[i]), sizeof(OOP_OAD_U)); //数据项标识
        offset += sizeof(OOP_OAD_U);
    }
    ret = func(label, DB_IOP_READ_NORMAL_DATA, playload, msglen, index);

    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_write_nomal_datas
* @brief     ：发送写多个普通数据消息
* @param[in] ：DBMSG_SEND_FUN func       发送回调函数
               uint16 label              消息label
               uint8 logic               逻辑设备号
               uint16 info               信息点号
               NOMAL_OAD_DATE_T *inDatas 多条数据
               uint8 inDataNum           数据数量
* @param[out]：uint16 *index             发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2020-4-28
* @Update    :
**********************************************************************/
int send_write_nomal_datas(DBMSG_SEND_FUN func, uint16 label, uint8 logic, uint16 info, NOMAL_OAD_DATE_T *inDatas, 
                           uint8 inDataNum, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint8  i = 0;
    uint16 offset = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;
    uint16 tempclass = CLASS_DATA_NORMAL;

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
    
    ret = func(label, DB_IOP_WRITE_NORMAL_DATA, playload, msglen, index);

    free(playload);
    return ret;
}

/**********************************************************************
* @name      : start_record_get_len
* @brief     ：开始读记录型数据长度获取
* @param[in] ：READ_RECORD_T *startRecord 记录型数据刷选条件
* @param[out]：
* @return    ：长度
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
uint16 start_record_get_len(READ_RECORD_T *startRecord)
{
    uint16 len = 0;
    uint8 offset = 0;
    uint8 lenBuf[3] = {0};

    /*     逻辑设备号  信息点号   筛选方式    排序方式  主OPTIONAL */
    len = 0x01    + 0x02    + 0x01    + 0x01   + 0x01;

    if(COL_NOT != startRecord->cType)
    {
        /*      时间起始点   时间结束点*/
        len += 0x04   +   0x04;
    }
    //主OAD非0,强制有效
    if(1 == startRecord->recordOAD.optional || 0 != startRecord->recordOAD.road.oadMain.value)
    {
        len += 0x04;   //主数据DI
    }

    //表地址
    if(FALSE != startRecord->bTable)
    {
        len += 1 + startRecord->table.nNum;
    }
    
    offset = appmsg_set_len_offset(startRecord->recordOAD.road.oadCols.nNum, lenBuf);
    len += offset;                                                         //次数据DI数量长度
    len += sizeof(uint32) * startRecord->recordOAD.road.oadCols.nNum;      //所有次数据DI内容长度
    return len;
}

/**********************************************************************
* @name      : send_read_record_start
* @brief     ：发送开始读记录型数据
* @param[in] ：DBMSG_SEND_FUN func        发送回调函数
               uint16 label               消息label
               READ_RECORD_T *startRecord 记录型数据刷选条件
* @param[out]：uint16 *index              发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int send_read_record_start(DBMSG_SEND_FUN func, uint16 label, READ_RECORD_T *startRecord, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8  j = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    msglen = start_record_get_len(startRecord);
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);
    
    playload[i++] = startRecord->recordOAD.logicId;                                 //逻辑设备号
    
    appmsg_memcpy_r(playload + i, &startRecord->recordOAD.infoNum, sizeof(uint16)); //信息点号
    i += sizeof(uint16);

    if(FALSE != startRecord->bTable)                                                //表地址
    {
        playload[i++] = startRecord->table.nNum;
        if(startRecord->table.nNum > 0)
        {
            memcpy(playload + i, startRecord->table.value, startRecord->table.nNum);
            i += startRecord->table.nNum;
        }
    }

    playload[i++] = startRecord->cType;                                             //筛选方式

    if(COL_NOT != startRecord->cType)
    {
        appmsg_memcpy_r(playload + i, &startRecord->cStart, sizeof(uint32));       //筛选区间开始
        i += sizeof(uint32);

        appmsg_memcpy_r(playload + i, &startRecord->cEnd, sizeof(uint32));        //筛选区间结束
        i += sizeof(uint32);
    }
    playload[i++] = startRecord->sortType;                                        //排序方式

    /* 主数据DI 主OAD非0,强制有效 */
    if(1 == startRecord->recordOAD.optional || 0 != startRecord->recordOAD.road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &startRecord->recordOAD.road.oadMain.value, sizeof(uint32));
        i += sizeof(uint32);
    }
    else
    {
        playload[i++] = 0;
    }

    /* 次数据DI */
    bytes = appmsg_set_len_offset(startRecord->recordOAD.road.oadCols.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    for(j = 0; j < startRecord->recordOAD.road.oadCols.nNum; j++)
    {
        appmsg_memcpy_r(playload + i, &(startRecord->recordOAD.road.oadCols.oad[j].value), 4);
        i += 4;
    }

    if(FALSE == startRecord->bTable)
    {
        ret = func(label, DB_IOP_READ_RECORD_START, playload, msglen, index);
    }
    else
    {
        ret = func(label, DB_IOP_READ_RECORD_START_FJ, playload, msglen, index);
    }
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_read_record_step
* @brief     ：发送单步读记录型数据
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               uint32 handle                  数据中心句柄
               STEP_READ_RECORD_T *stepRecord 数据条件
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int send_read_record_step(DBMSG_SEND_FUN func, uint16 label, uint32 handle, STEP_READ_RECORD_T *stepRecord, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    /*        数据库句柄           附加校验OPTIONAL */
    msglen = sizeof(uint32) + 1;
    
    if(0 != stepRecord->MAC.nNum)
    {
        bytes = appmsg_set_len_offset(stepRecord->MAC.nNum, bufLen);
        msglen += bytes + stepRecord->MAC.nNum;
    }

    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    appmsg_memcpy_r(playload, &handle, sizeof(uint32));    //数据库句柄
    i += sizeof(uint32);

    /* 附加校验 */
    if(0 != stepRecord->MAC.nNum)
    {
        playload[i++] = 1;
        memcpy(playload + i, bufLen, bytes);
        i += bytes;
        memcpy(playload + i, stepRecord->MAC.value, stepRecord->MAC.nNum);
        i += stepRecord->MAC.nNum;
    }
    else
    {
        playload[i++] = 0;
    }
    
    ret = func(label, DB_IOP_READ_RECORD_STEP, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_read_record_end
* @brief     ：发送结束记录型数据读取
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               uint32 handle                  数据中心句柄
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int send_read_record_end(DBMSG_SEND_FUN func, uint16 label, uint32 handle, uint16 *index)
{
    uint8  playload[4] = {0};
    
    appmsg_memcpy_r(playload, &handle, sizeof(uint32));
    return func(label, DB_IOP_READ_RECORD_END, playload, sizeof(uint32), index);
}

/**********************************************************************
* @name      : send_write_record
* @brief     ：发送写记录型数据
* @param[in] ：DBMSG_SEND_FUN func             发送回调函数
               uint16 label                   消息label
               WRITE_RECORD_T *record         记录内容
* @param[out]：uint16 *index                   发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    : 2022-6-1
**********************************************************************/
int send_write_record(DBMSG_SEND_FUN func, uint16 label, WRITE_RECORD_T *record, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    /*      逻辑设备号, 开始时间, 结束时间, 存储时间, 信息点, 数据类, OPTIONAL */
    msglen = 0x01       + 0x04    + 0x04   + 0x4      + 0x2   + 0x2   + 0x1;
    
    /* 主OAD非0,强制有效 */
    if(1 == record->recordOAD.optional || 0 != record->recordOAD.road.oadMain.value)
    {
        msglen += 0x04;    //主OAD
    }

    /* 次数据 */
    msglen += 1 + 0x04;

    /* 数据域 */
    bytes = appmsg_set_len_offset(record->inDatalen, bufLen);
    msglen += 1 + bytes + record->inDatalen;

    //附加校验
    bytes = appmsg_set_len_offset(record->MAC.nNum, bufLen);
    msglen += bytes + record->MAC.nNum;
    
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);
    
    playload[i++] = record->recordOAD.logicId;  //逻辑地址
    appmsg_memcpy_r(playload + i, &(record->colStartTm), sizeof(uint32));        //起始时间
    i += 4;
    
    appmsg_memcpy_r(playload + i, &(record->colEndTm), sizeof(uint32));          //结束时间
    i += 4;
    
    appmsg_memcpy_r(playload + i, &(record->colStoreTm), sizeof(uint32));         //存储时间
    i += 4;
    
    appmsg_memcpy_r(playload + i, &(record->recordOAD.infoNum), sizeof(uint16));  //信息点
    i += 2;

    /* 数据类标识 */
    if(0 == record->recordOAD.classtag)
    {
        uint16 tempclass = CLASS_DATA_NORMAL;
        appmsg_memcpy_r(playload + i, &tempclass, sizeof(uint16));//数据类标识 为0，填写成CLASS_DATA_NORMAL
    }
    else
    {
        appmsg_memcpy_r(playload + i, &(record->recordOAD.classtag), sizeof(uint16));//数据类标识
    }
    i += 2;

    /* 主DI */
    if(1 == record->recordOAD.optional || 0 != record->recordOAD.road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &(record->recordOAD.road.oadMain.value), sizeof(uint32));
        i += sizeof(uint32);
    }
    else
    {
        playload[i++] = 0;
    }

    /* 次DI */
    playload[i++] = 1;
    appmsg_memcpy_r(playload + i, &(record->recordOAD.road.oadCols.oad[0]), sizeof(uint32));//次数据
    i += 4;

    /* 数据缓冲 */
    playload[i++] = 1;
    bytes = appmsg_set_len_offset(record->inDatalen, bufLen);
    memcpy(playload + i,bufLen, bytes); 
    i += bytes;
    memcpy(playload + i, record->inDataBuf, record->inDatalen);
    i += record->inDatalen;
    /* 附加校验 */
    bytes = appmsg_set_len_offset(record->MAC.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, record->MAC.value, record->MAC.nNum);
    i += record->MAC.nNum;

    ret = func(label, DB_IOP_WRITE_RECORD, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_write_records
* @brief     ：发送写记录型数据
* @param[in] ：DBMSG_SEND_FUN func             发送回调函数
               uint16 label                   消息label
               WRITE_RECORD_T *record         记录内容
* @param[out]：uint16 *index                   发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2022-6-1
* @Update    :
**********************************************************************/
int send_write_records(DBMSG_SEND_FUN func, uint16 label, WRITE_RECORDS_T *record, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint16 j = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    /*     逻辑设备号 开始时间 结束时间 存储时标 信息点号 数据类标识 OPTIONAL */
    msglen = 0x01   + 0x04  +  0x04  +  0x04  +  0x02 +   0x2+       0x01 ;
    
    /* 主OAD非0,强制有效 */
    if(1 == record->recordOAD.optional || 0 != record->recordOAD.road.oadMain.value)
    {
        msglen += 0x04;    //主OAD
    }

    /* 数据集合数量 */
    bytes = appmsg_set_len_offset(record->recordOAD.road.oadCols.nNum, bufLen);
    msglen += bytes;
    //数据域也有sequence
    msglen += bytes;
    for(j = 0; j < record->recordOAD.road.oadCols.nNum; j++)
    {
        //octet-string的长度 
        bytes = appmsg_set_len_offset(record->datas[j].len, bufLen);
        msglen += 4 + bytes + record->datas[j].len;
    }

    //附加校验
    bytes = appmsg_set_len_offset(record->MAC.nNum, bufLen);
    msglen += bytes + record->MAC.nNum;
    
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    i = 0;
    playload[i++] = record->recordOAD.logicId;  //逻辑地址

    appmsg_memcpy_r(playload + i, &(record->colStartTm), sizeof(uint32));        //起始时间
    i += 4;

    appmsg_memcpy_r(playload + i, &(record->colEndTm), sizeof(uint32));          //结束时间
    i += 4;

    appmsg_memcpy_r(playload + i, &(record->colStoreTm), sizeof(uint32));         //存储时间
    i += 4;

    appmsg_memcpy_r(playload + i, &(record->recordOAD.infoNum), sizeof(uint16));  //信息点
    i += 2;

    if(0 == record->recordOAD.classtag)
    {
        uint16 tempclass = CLASS_DATA_NORMAL;
        appmsg_memcpy_r(playload + i, &tempclass, sizeof(uint16));//数据类标识 为0，填写成CLASS_DATA_NORMAL
    }
    else
    {
        appmsg_memcpy_r(playload + i, &(record->recordOAD.classtag), sizeof(uint16));//数据类标识
    }
    i += 2;

    /* 主DI */
    if(1 == record->recordOAD.optional || 0 != record->recordOAD.road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &(record->recordOAD.road.oadMain.value), sizeof(uint32));
        i += sizeof(uint32);
    }
    else
    {
        playload[i++] = 0;
    }

    /* 数量 */
    bytes = appmsg_set_len_offset(record->recordOAD.road.oadCols.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    
    for(j = 0; j < record->recordOAD.road.oadCols.nNum; j++)
    {
        appmsg_memcpy_r(playload + i, &(record->recordOAD.road.oadCols.oad[j]), sizeof(uint32));//次数据
        i += 4;
    }
    
    bytes = appmsg_set_len_offset(record->recordOAD.road.oadCols.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    for(j = 0; j < record->recordOAD.road.oadCols.nNum; j++)
    {
        /* 数据缓冲 */
        bytes = appmsg_set_len_offset(record->datas[j].len, bufLen);
        memcpy(playload + i, bufLen, bytes);
        i += bytes;
        memcpy(playload + i, record->datas[j].data, record->datas[j].len);
        i += record->datas[j].len;
    }
    /* 附加校验 */
    bytes = appmsg_set_len_offset(record->MAC.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, record->MAC.value, record->MAC.nNum);
    i += record->MAC.nNum;

    ret = func(label, DB_IOP_WRITE_RECORD, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : update_record_get_len
* @brief     ：更新记录数据长度获取
* @param[in] ：UPDATE_RECORD_T *pData   记录内容
* @param[out]：
* @return    ：长度
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
uint16 update_record_get_len(UPDATE_RECORD_T *pData)
{
    uint16 len = 0;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    if(1 == pData->filter.choice || 2 == pData->filter.choice)
    {
        /*    逻辑设备号   OPTIONAL  次OAD      刷选方式  刷选条件   信息点号*/
        len = 0x01    + 0x01  +  0x04 +   0x01 +   0x04   +  0x02;
    }
    else
    {
        return 0;
    }

    if(1 == pData->recordOAD.optional || 0 != pData->recordOAD.road.oadMain.value)
    {
        len += 0x4;
    }

    /* 附加校验 */
    bytes = appmsg_set_len_offset(pData->MAC.nNum, bufLen);
    len += bytes + pData->MAC.nNum;
    
    //octet-string的长度
    bytes = appmsg_set_len_offset(pData->inDatalen, bufLen);
    len += bytes + pData->inDatalen;          
    return len;
}

/**********************************************************************
* @name      : send_write_record
* @brief     ：发送更新记录数据
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               UPDATE_RECORD_T *pData         记录内容
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int send_update_record(DBMSG_SEND_FUN func, uint16 label, UPDATE_RECORD_T *pData, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    msglen = update_record_get_len(pData);
    if(0 == msglen)
    {
        return DBMSG_ERR_INPUT;
    }
    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    playload[i++] = pData->recordOAD.logicId;                                              //逻辑设备号

    if(1 == pData->recordOAD.optional || 0 != pData->recordOAD.road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &(pData->recordOAD.road.oadMain.value), sizeof(uint32)); //主数据DI
        i += 4;
    }
    else
    {
        playload[i++] = 0;
    }

    /* 次数据DI */
    appmsg_memcpy_r(playload + i, &(pData->recordOAD.road.oadCols.oad[0]), sizeof(uint32));
    i += 4;

    /* 筛选方式 */
    playload[i++] = pData->filter.choice;
    if(pData->filter.choice == 1)
    {
        appmsg_memcpy_r(playload + i, &(pData->filter.storeTm), sizeof(uint32));
        i += 4;
    }
    else if(pData->filter.choice == 2)
    {
        appmsg_memcpy_r(playload + i, &(pData->filter.storeNo), sizeof(uint32));
        i += 4;
    }
    else
    {
        free(playload);
        return DBMSG_ERR_INPUT;
    }

    appmsg_memcpy_r(playload + i, &(pData->recordOAD.infoNum), sizeof(uint16));   //信息点号
    i += 2;

    /* 附加校验 */
    bytes = appmsg_set_len_offset(pData->MAC.nNum, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, pData->MAC.value, pData->MAC.nNum);
    i += pData->MAC.nNum;

    /* 数据缓冲 */
    bytes = appmsg_set_len_offset(pData->inDatalen, bufLen);
    memcpy(playload + i, bufLen, bytes);
    i += bytes;
    memcpy(playload + i, pData->inDataBuf, pData->inDatalen);

    ret = func(label, DB_IOP_UPDATE_RECORD, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_write_record
* @brief     ：发送获取存储记录条数
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               GET_RECORD_NUM_T *pRecord      删选条件
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int send_get_record_num(DBMSG_SEND_FUN func, uint16 label, GET_RECORD_NUM_T *pRecord, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;

    /*        逻辑设备号 信息点号 OPTIONAL  次OAD      刷选方式 */
    msglen = 1   +    2     + 1   +    4    +     1 ;

    if(1 == pRecord->recordOAD.optional || 0 != pRecord->recordOAD.road.oadMain.value)
    {
        msglen += 4;  //主OAD
    }
    
    if(GET_INTER_RECORD == pRecord->filter)
    {
        msglen += 8;  //区间
    }

    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    playload[i++] = pRecord->recordOAD.logicId;   //逻辑设备号
    
    appmsg_memcpy_r(playload + i, &(pRecord->recordOAD.infoNum), sizeof(uint16)); //信息点号
    i += 2;

    if(1 == pRecord->recordOAD.optional || 0 != pRecord->recordOAD.road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &(pRecord->recordOAD.road.oadMain.value), sizeof(uint32));//主数据DI
        i += 4;
    }
    else
    {
        playload[i++] = 0;
    }

    /* 次数据DI */
    appmsg_memcpy_r(playload + i, &(pRecord->recordOAD.road.oadCols.oad[0]), sizeof(uint32));
    i += 4;

    playload[i++] = pRecord->filter;   //筛选方式

    if(GET_INTER_RECORD == pRecord->filter)
    {
        appmsg_memcpy_r(playload + i, &(pRecord->gStart), sizeof(uint32));
        i += 4;
        appmsg_memcpy_r(playload + i, &(pRecord->gEnd), sizeof(uint32));
    }

    ret = func(label, DB_IOP_GET_RECORD_NUM, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_get_record_depth
* @brief     ：发送获取存储记录深度
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               OOP_ROAD_T *pRoad              road
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int send_get_record_depth(DBMSG_SEND_FUN func, uint16 label, RECORD_OAD_T *pRoad, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;

    /*       OPTIONAL  次OAD*/
    msglen =0x01 +0x01 +   0x04;
    
    if(1 == pRoad->optional || 0 != pRoad->road.oadMain.value)
    {
        msglen += 0x04; //主OAD
    }

    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);
	
	playload[i++] = pRoad->logicId;
    /* 主OAD */
    if(1 == pRoad->optional || 0 != pRoad->road.oadMain.value)
    {
        playload[i++] = 1;
        appmsg_memcpy_r(playload + i, &(pRoad->road.oadMain.value), sizeof(uint32));
        i += 4;
    }
    else
    {
        playload[i++] = 0;
    }

    appmsg_memcpy_r(playload + i, &(pRoad->road.oadCols.oad[0]), sizeof(uint32)); //次OAD

    ret = func(label, DB_IOP_GET_RECORD_DEPTH, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_clear_data
* @brief     ：发送数据初始化
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               CLEAR_DATA_T *ptData           清除数据选项
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    : 2020-1-20
**********************************************************************/
int send_clear_data(DBMSG_SEND_FUN func, uint16 label, CLEAR_DATA_T *ptData, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;

    msglen = 1 + 1;
    if(TRUE == ptData->bClassTag)
    {
        msglen += 2;
    }

    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    playload[i++] = ptData->recordOAD.logicId;
    playload[i++] = ptData->bClassTag;
    if(TRUE == ptData->bClassTag)
    {
        appmsg_memcpy_r(playload + i, &(ptData->recordOAD.classtag), sizeof(uint16));
        i += 2;
    }
    
    ret = func(label, DB_IOP_INIT_DATA, playload, msglen, index);
    free(playload);
    return ret;
}

/**********************************************************************
* @name      : send_clear_data
* @brief     ：发送数据初始化
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               RECORD_OAD_T *pRecord          OAD
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int send_clear_oad_data(DBMSG_SEND_FUN func, uint16 label, CLEAR_DATA_T *ptData, uint16 *index)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint16 i = 0;
    uint8 *playload = NULL;
	
	//		逻辑设备号 信息点号
    msglen = 	1 		+ 2;
	
	if(FALSE == ptData->bFilter)
    {
    	msglen += 2;	//数据类型
        msglen += 1;	//OPTIONAL
        if(TRUE != ptData->bClassTag)
	    {
	        if(1 == ptData->recordOAD.optional || 0 != ptData->recordOAD.road.oadMain.value)
	        {
	            /*         OPTIONAL  主OAD   次OAD */
	            msglen += 0x01  + 0x04 + 0x04;
	        }
	        else
	        {
	            /*         OPTIONAL   次OAD */
	            msglen += 0x01  +  0x04;
	        }
	    }
    }
    else
    {
    	if(1 == ptData->recordOAD.optional || 0 != ptData->recordOAD.road.oadMain.value)
        {
            /*         OPTIONAL  主OAD   次OAD */
            msglen += 0x01  + 0x04 + 0x04;
        }
        else
        {
            /*         OPTIONAL   次OAD */
            msglen += 0x01  +  0x04;
        }
		
        if (0 == ptData->filterType)
        {
            msglen += 9;
        }
        else if (255 == ptData->filterType)
        {
            msglen += 1;
        }
        else
        {
            return DBMSG_ERR_UNKNOW;
        }
    }
	    

    playload = (uint8 *)malloc(msglen);
    if(NULL == playload)
    {
        return DBMSG_ERR_MALLOC;
    }
    memset(playload, 0, msglen);

    playload[i++] = ptData->recordOAD.logicId;                                    //逻辑设备号
    appmsg_memcpy_r(playload + i, &(ptData->recordOAD.infoNum), sizeof(uint16));  //信息点号
    i += 2;
    
	if(FALSE == ptData->bFilter)
    {
    	appmsg_memcpy_r(playload + i, &(ptData->recordOAD.classtag), sizeof(uint16));  //数据类标识
    	i += 2;
		
		if(TRUE == ptData->bClassTag)
	    {
	        playload[i++] = 0;
	    }
	    else
	    {
	        playload[i++] = 1;
	        
	        if(1 == ptData->recordOAD.optional || 0 != ptData->recordOAD.road.oadMain.value)
	        {
	            playload[i++] = 1;
	            appmsg_memcpy_r(playload + i, &(ptData->recordOAD.road.oadMain.value), sizeof(uint32));
	            i += 4;
	        }
	        else
	        {
	            playload[i++] = 0;
	        }
	        
	        appmsg_memcpy_r(playload + i, &(ptData->recordOAD.road.oadCols.oad[0].value), sizeof(uint32));
	    }
    }
    else
    {
        if(1 == ptData->recordOAD.optional || 0 != ptData->recordOAD.road.oadMain.value)
        {
            playload[i++] = 1;
            appmsg_memcpy_r(playload + i, &(ptData->recordOAD.road.oadMain.value), sizeof(uint32));
            i += 4;
        }
        else
        {
            playload[i++] = 0;
        }
        
        appmsg_memcpy_r(playload + i, &(ptData->recordOAD.road.oadCols.oad[0].value), sizeof(uint32));
        i += 4;

        if (0 == ptData->filterType)
        {
            playload[i++] = 0;
            
            appmsg_memcpy_r(playload + i, &(ptData->startFilter), sizeof(uint32));
            i += 4;
            appmsg_memcpy_r(playload + i, &(ptData->endFilter), sizeof(uint32));
            i += 4;
        }
        else if (255 == ptData->filterType)
        {
            playload[i++] = 255;
        }
    }

    ret = func(label, FALSE == ptData->bFilter ? DB_IOP_CLEAR_ONE_DATA : DB_IOP_CLEAR_ONE_DATA_FJ, playload, msglen, index);
    free(playload);
	    
    return ret;
}

/**********************************************************************
* @name      : send_recover_default
* @brief     ：发送恢复出厂
* @param[in] ：DBMSG_SEND_FUN func            发送回调函数
               uint16 label                   消息label
               uint8 logicId                  逻辑设备号
* @param[out]：uint16 *index                  发送消息的index
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int send_recover_default(DBMSG_SEND_FUN func, uint16 label, uint8 logicId, uint16 *index)
{
    return func(label, DB_IOP_RECOVERY_DEFAULT, &logicId, 1, index);
}

/**********************************************************************
* @name      : db_data_to_base64
* @brief     ：普通数据转Base64格式
* @param[in] ：uint8 *pData         数据
               uint32 datalen       数据长度
* @param[out]：
* @return    ：base64结果
* @Create    : 王津剑
* @Date      ：2020-10-15
* @Update    :
**********************************************************************/
char *db_data_to_base64(uint8 *pData, uint32 datalen)
{
    uint32 base64len = 0;
    uint32 i = 0;
    uint32 j = 0;
    char   *base64 = NULL;
    //定义base64编码表  
    uint8 base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    if(NULL == pData || 0 == datalen)
    {
        return NULL;
    }
    //计算经过base64编码后的字符串长度

    if(0 == datalen % 3)
    {
        base64len = (datalen / 3) * 4;
    }
    else
    {
        base64len = (datalen / 3 + 1) * 4;
    }
    
    base64 = (char *)malloc(base64len + 1);
    if(NULL == base64)
    {
        return NULL;
    }
    memset(base64, 0, base64len + 1);

    for(i = 0, j = 0; i < datalen && j + 3 < base64len; i += 3, j += 4)
    {
        base64[j] = base64_table[pData[i]>>2];
        if(i + 2 < datalen)
        {
            base64[j+1] = base64_table[((pData[i]&0x3)<<4) | (pData[i+1]>>4)];
            base64[j+2] = base64_table[((pData[i+1]&0xf)<<2) | (pData[i+2]>>6)];
            base64[j+3] = base64_table[pData[i+2]&0x3f];
        }
        else if(i + 1 < datalen)
        {
            base64[j+1] = base64_table[((pData[i]&0x3)<<4) | (pData[i+1]>>4)];
            base64[j+2] = base64_table[(pData[i+1]&0xf)<<2];
            base64[j+3] = '=';
        }
        else
        {
            base64[j+1] = base64_table[(pData[i]&0x3)<<4];
            base64[j+2] = '=';
            base64[j+3] = '=';
        }
    }

    return base64;
}

/**********************************************************************
* @name      : db_base64_to_data
* @brief     ：Base64格式专普通数据
* @param[in] ：uint8 *pBase         Base64数据
               uint32 buflen        输出结果缓存大小
* @param[out]：uint8 *pBuf          数据输出结果
               uint32 *datalen      输出结果实际长度
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-10-15
* @Update    :
**********************************************************************/
int db_base64_to_data(char *pBase, uint8 *pBuf, uint32 buflen, uint32 *datalen)
{
    uint32 dataLen = 0;
    uint32 i = 0;
    uint32 j = 0;
    uint32 num = 0;
    uint8  temp[3] = {0};
    uint8  a0, a1, a2, a3;
    uint32 len = 0;

    //根据base64表，以字符找到对应的十进制数据  
    uint8 table[]={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,62, 0, 0, 0,63,
                   52,53,54,55,56,57,58,59,60,61, 0, 0, 0, 0, 0, 0,
                    0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
                   15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0, 0,
                    0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
                   41,42,43,44,45,46,47,48,49,50,51}; 
    
    if(NULL == pBase || 0 == strlen(pBase) || NULL == pBuf || 0 == buflen || NULL == datalen)
    {
        return DBMSG_ERR_INPUT;
    }

    len = strlen(pBase);
    if(0 != len % 4)
    {
        return DBMSG_ERR_INPUT;
    }

    if('=' == pBase[len-1] && '=' == pBase[len-2])
    {
        dataLen = (len / 4) *3 -2;
    }
    else if('=' == pBase[len-1])
    {
        dataLen = (len / 4) *3 -1;
    }
    else
    {
        dataLen = (len / 4) *3;
    }
    *datalen = dataLen;

    for(i = 0; i < len - 2; i += 4)
    {
        if(pBase[i] > 'z' || pBase[i+1] > 'z' || pBase[i+2] > 'z' || pBase[i+3] > 'z')
        {
            return DBMSG_ERR_INPUT;
        }
        a0 = pBase[i];
        a1 = pBase[i+1];
        a2 = pBase[i+2];
        a3 = pBase[i+3];

        temp[0] = ((table[a0] << 2) | (table[a1] >> 4)); 
        temp[1] = ((table[a1] << 4) | (table[a2] >> 2));
        temp[2] = ((table[a2] << 6) | table[a3]);
        for(j = 0; j < 3; j++)
        {
            if(num >= buflen)
            {
                return 0;
            }
            pBuf[num] = temp[j];
            num++;
        }
    }
    return DBMSG_SUCCESS;
}

