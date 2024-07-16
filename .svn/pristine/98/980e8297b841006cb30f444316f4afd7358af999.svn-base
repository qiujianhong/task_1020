/*
*********************************************************************
* @filename: db_agent.c
* @brief   : 消息通信 DB操作
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2019-12-17    create
*********************************************************************
*/
#include "baseCommon.h"
#include "task_msg.h"
#include "dbmsg_id.h"
#include "appmsg_split.h"

#include "db_comm.h"

/* db代理类型, 默认是进程级别 */
uint8           g_dbAgentType = AGENT_TYPE_PROCESS;

/* 对外发送消息回调函数 */
DBMSG_SEND_FUN  g_SendFunc = NULL;

pthread_t       g_tAgentid = 0;

uint8 db_get_agent_type()
{
    return g_dbAgentType;
}

DBMSG_SEND_FUN db_get_send_func()
{
    return g_SendFunc;
}

/**********************************************************************
* @name      : db_taskmsg_read_nomal
* @brief     ：db操作读取普通数据
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_taskmsg_read_nomal(uint8 *pMsg, uint32 sender, void *func)
{
    int               ret = 0;
    TASK_MSG_t       *pInMsg = (TASK_MSG_t *)pMsg;
    READ_NOMAL_REQ_T *pReq = NULL;
    DBCLIENT_CACHE_T *ptClientInfo = NULL;
    uint16            index = 0;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }

    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    pReq = (READ_NOMAL_REQ_T *)pInMsg->msg;

    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);
    
    pthread_mutex_lock(&(ptClientInfo->lock));
    ret = send_read_nomal_data(func, (uint16)sender, &(pReq->tOad), &index);
    DPRINT("Client 0x%x read normal data: oad 0x%x, index %u, ret %d\n", sender, pReq->tOad.oad.value, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
        ptClientInfo->ackbuflen = pReq->maxLen;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}

/**********************************************************************
* @name      : db_taskmsg_write_nomal
* @brief     ：db操作写普通数据
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-25
* @Update    :
**********************************************************************/
int db_taskmsg_write_nomal(uint8 *pMsg, uint32 sender, void *func)
{
    int                ret = 0;
    TASK_MSG_t        *pInMsg = (TASK_MSG_t *)pMsg;
    WRITE_NOMAL_REQ_T *pReq = NULL;
    DBCLIENT_CACHE_T  *ptClientInfo = NULL;
    uint16             index = 0;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }
    
    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    pReq = (WRITE_NOMAL_REQ_T *)pInMsg->msg;

    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);

    pthread_mutex_lock(&(ptClientInfo->lock));
    ret = send_write_nomal_data(func, (uint16)sender, &(pReq->tOad), pReq->data, pReq->dataLen, &index);
    DPRINT("Client 0x%x write normal data: oad 0x%x, index %u, ret %d\n", sender, pReq->tOad.oad.value, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}

/**********************************************************************
* @name      : db_taskmsg_read_nomals
* @brief     ：db操作读取普通数据
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-28
* @Update    :
**********************************************************************/
int db_taskmsg_read_nomals(uint8 *pMsg, uint32 sender, void *func)
{
    int                ret = 0;
    TASK_MSG_t        *pInMsg = (TASK_MSG_t *)pMsg;
    READ_NOMALS_REQ_T *pReq = NULL;
    DBCLIENT_CACHE_T  *ptClientInfo = NULL;
    uint16             index = 0;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }

    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    pReq = (READ_NOMALS_REQ_T *)pInMsg->msg;

    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);
    
    pthread_mutex_lock(&(ptClientInfo->lock));
    ret = send_read_nomal_datas(func, (uint16)sender, &(pReq->tOads), &index);
    DPRINT("Client 0x%x read normal datas: first oad 0x%x, index %u, ret %d\n", sender, pReq->tOads.oad[0].value, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}

/**********************************************************************
* @name      : db_taskmsg_write_nomals
* @brief     ：db操作写普通数据
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-28
* @Update    :
**********************************************************************/
int db_taskmsg_write_nomals(uint8 *pMsg, uint32 sender, void *func)
{
    int               ret = 0;
    TASK_MSG_t       *pInMsg = (TASK_MSG_t *)pMsg;
    DBCLIENT_CACHE_T *ptClientInfo = NULL;
    uint16            index = 0;
    DBMSG_SEND_FUN    sendfun = (DBMSG_SEND_FUN)func;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }
    
    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);

    pthread_mutex_lock(&(ptClientInfo->lock));

    /* 写多个记录长度太大，封装成playout格式发送 */
    ret = sendfun((uint16)sender, DB_IOP_WRITE_NORMAL_DATA, pInMsg->msg, pInMsg->msglen, &index);
    DPRINT("Client 0x%x write normal datas: index %u, ret %d\n", sender, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}

/**********************************************************************
* @name      : db_taskmsg_read_record_start
* @brief     ：db操作开始读记录型数据
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_taskmsg_read_record_start(uint8 *pMsg, uint32 sender, void *func)
{
    int                      ret = 0;
    TASK_MSG_t              *pInMsg = (TASK_MSG_t *)pMsg;
    READ_RECORD_START_REQ_T *pReq = NULL;
    DBCLIENT_CACHE_T        *ptClientInfo = NULL;
    uint16                   index = 0;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }
    
    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    pReq = (READ_RECORD_START_REQ_T *)pInMsg->msg;

    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);

    pthread_mutex_lock(&(ptClientInfo->lock));
    ret = send_read_record_start(func, (uint16)sender, &(pReq->tRecord), &index);
    DPRINT("Client 0x%x start read record: main oad 0x%x, index %u, ret %d\n", sender, 
            pReq->tRecord.recordOAD.road.oadMain.value, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}

/**********************************************************************
* @name      : db_taskmsg_read_record_step
* @brief     ：db操作单步读记录型数据
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_taskmsg_read_record_step(uint8 *pMsg, uint32 sender, void *func)
{
    int                     ret = 0;
    TASK_MSG_t             *pInMsg = (TASK_MSG_t *)pMsg;
    READ_RECORD_STEP_REQ_T *pReq = NULL;
    DBCLIENT_CACHE_T       *ptClientInfo = NULL;
    uint16                  index = 0;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }
    
    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    pReq = (READ_RECORD_STEP_REQ_T *)pInMsg->msg;

    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);

    pthread_mutex_lock(&(ptClientInfo->lock));
    ret = send_read_record_step(func, (uint16)sender, pReq->handle, &(pReq->tStepRecord), &index);
    DPRINT("Client 0x%x step read record: handle %u, index %u, ret %d\n", sender, pReq->handle, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}

/**********************************************************************
* @name      : db_taskmsg_read_record_end
* @brief     ：db操作结束记录型数据读取
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_taskmsg_read_record_end(uint8 *pMsg, uint32 sender, void *func)
{
    int                    ret = 0;
    TASK_MSG_t            *pInMsg = (TASK_MSG_t *)pMsg;
    READ_RECORD_END_REQ_T *pReq = NULL;
    DBCLIENT_CACHE_T      *ptClientInfo = NULL;
    uint16                 index = 0;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }
    
    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    pReq = (READ_RECORD_END_REQ_T *)pInMsg->msg;
    
    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);

    pthread_mutex_lock(&(ptClientInfo->lock));
    ret = send_read_record_end(func, (uint16)sender, pReq->handle, &index);
    DPRINT("Client 0x%x end read record: handle %u, index %u, ret %d\n", sender, pReq->handle, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}

/**********************************************************************
* @name      : db_taskmsg_write_record
* @brief     ：db操作写记录型数据
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_taskmsg_write_record(uint8 *pMsg, uint32 sender, void *func)
{
    int                 ret = 0;
    TASK_MSG_t         *pInMsg = (TASK_MSG_t *)pMsg;
    WTITE_RECORD_REQ_T *pReq = NULL;
    DBCLIENT_CACHE_T   *ptClientInfo = NULL;
    uint16              index = 0;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }
    
    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    pReq = (WTITE_RECORD_REQ_T *)pInMsg->msg;
    
    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);

    pthread_mutex_lock(&(ptClientInfo->lock));
    ret = send_write_record(func, (uint16)sender, &(pReq->tRecord), &index);
    DPRINT("Client 0x%x write record: main oad 0x%x, index %u, ret %d\n", sender, 
           pReq->tRecord.recordOAD.road.oadMain.value, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}

/**********************************************************************
* @name      : db_taskmsg_update_record
* @brief     ：db操作更新记录数据
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
int db_taskmsg_update_record(uint8 *pMsg, uint32 sender, void *func)
{
    int                  ret = 0;
    TASK_MSG_t          *pInMsg = (TASK_MSG_t *)pMsg;
    UPDATE_RECORD_REQ_T *pReq = NULL;
    DBCLIENT_CACHE_T   *ptClientInfo = NULL;
    uint16              index = 0;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }
    
    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    pReq = (UPDATE_RECORD_REQ_T *)pInMsg->msg;
    
    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);

    pthread_mutex_lock(&(ptClientInfo->lock));
    ret = send_update_record(func, (uint16)sender, &(pReq->tRecord), &index);
    DPRINT("Client 0x%x update record: main oad 0x%x, index %u, ret %d\n", sender, 
           pReq->tRecord.recordOAD.road.oadMain.value, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}

/**********************************************************************
* @name      : db_taskmsg_get_record_num
* @brief     ：db操作获取存储记录条数
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_taskmsg_get_record_num(uint8 *pMsg, uint32 sender, void *func)
{
    int                  ret = 0;
    TASK_MSG_t          *pInMsg = (TASK_MSG_t *)pMsg;
    GET_RECORDNUM_REQ_T *pReq = NULL;
    DBCLIENT_CACHE_T    *ptClientInfo = NULL;
    uint16               index = 0;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }
    
    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    pReq = (GET_RECORDNUM_REQ_T *)pInMsg->msg;
    
    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);

    pthread_mutex_lock(&(ptClientInfo->lock));
    ret = send_get_record_num(func, (uint16)sender, &(pReq->tRecord), &index);
    DPRINT("Client 0x%x get record num: main oad 0x%x, index %u, ret %d\n", sender, 
           pReq->tRecord.recordOAD.road.oadMain.value, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}

/**********************************************************************
* @name      : db_taskmsg_get_record_depth
* @brief     ：db操作获取存储记录深度
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_taskmsg_get_record_depth(uint8 *pMsg, uint32 sender, void *func)
{
    int               ret = 0;
    TASK_MSG_t       *pInMsg = (TASK_MSG_t *)pMsg;
    GET_RECORD_REQ_T *pReq = NULL;
    DBCLIENT_CACHE_T *ptClientInfo = NULL;
    uint16            index = 0;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }
    
    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    pReq = (GET_RECORD_REQ_T *)pInMsg->msg;
    
    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);

    pthread_mutex_lock(&(ptClientInfo->lock));
    ret = send_get_record_depth(func, (uint16)sender, &(pReq->tRoad), &index);
    DPRINT("Client 0x%x get record depth: main oad 0x%x, index %u, ret %d\n", sender, 
           pReq->tRoad.road.oadMain.value, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;

}

/**********************************************************************
* @name      : db_taskmsg_clear_data
* @brief     ：db操作数据初始化
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_taskmsg_clear_data(uint8 *pMsg, uint32 sender, void *func)
{
    int               ret = 0;
    TASK_MSG_t       *pInMsg = (TASK_MSG_t *)pMsg;
    CLEAR_DATA_REQ_T *pReq = NULL;
    DBCLIENT_CACHE_T *ptClientInfo = NULL;
    uint16            index = 0;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }
    
    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    pReq = (CLEAR_DATA_REQ_T *)pInMsg->msg;
    
    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);

    pthread_mutex_lock(&(ptClientInfo->lock));
    ret = send_clear_data(func, (uint16)sender, &(pReq->tData), &index);
    DPRINT("Client 0x%x clear data: logicId %d, index %u, ret %d\n", sender, pReq->tData.recordOAD.logicId, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}

/**********************************************************************
* @name      : db_taskmsg_clear_oad_data
* @brief     ：db操作指定数据初始化
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_taskmsg_clear_oad_data(uint8 *pMsg, uint32 sender, void *func)
{
    int                   ret = 0;
    TASK_MSG_t           *pInMsg = (TASK_MSG_t *)pMsg;
    CLEAR_DATA_REQ_T     *pReq = NULL; 
    DBCLIENT_CACHE_T     *ptClientInfo = NULL;
    uint16                index = 0;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }
    
    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    pReq = (CLEAR_DATA_REQ_T *)pInMsg->msg;
    
    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);

    pthread_mutex_lock(&(ptClientInfo->lock));
    ret = send_clear_oad_data(func, (uint16)sender, &(pReq->tData), &index);
    DPRINT("Client 0x%x clear oaddata: main oad 0x%x, index %u, ret %d\n", sender, 
           pReq->tData.recordOAD.road.oadMain.value, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}

/**********************************************************************
* @name      : db_taskmsg_recover_recover
* @brief     ：db操作恢复出厂设置
* @param[in] ：uint8 *pMsg     消息
               uint32 sender   发送者
               void *func      发送消息函数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int db_taskmsg_recover_recover(uint8 *pMsg, uint32 sender, void *func)
{
    int               ret = 0;
    TASK_MSG_t       *pInMsg = (TASK_MSG_t *)pMsg;
    uint8             logicId = 0;
    DBCLIENT_CACHE_T *ptClientInfo = NULL;
    uint16            index = 0;

    if(NULL == pMsg || 0 == sender || NULL == func)
    {
        return DBMSG_ERR_INPUT;
    }
    
    ptClientInfo = db_get_clientinfo(sender, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }

    logicId = *pInMsg->msg;
    
    /* 先清空客户端缓存 */
    db_clear_clientinfo(ptClientInfo);

    pthread_mutex_lock(&(ptClientInfo->lock));
    ret = send_recover_default(func, (uint16)sender, logicId, &index);
    DPRINT("Client 0x%x recover default: logicId %d, index %u, ret %d\n", sender, logicId, index, ret);
    if(0 == ret)
    {
        /* 成功 */
        ptClientInfo->msgid = pInMsg->msg_id;
        ptClientInfo->taskindex = pInMsg->syncindex;
        ptClientInfo->msgindex = index;
        ptClientInfo->bwait = 1;
    }
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}



DB_TASKMSG_PROC_T  g_DBTaskMsgProc[] = 
{
    {MSG_DB_READ_NOMAL       , db_taskmsg_read_nomal},
    {MSG_DB_WRITE_NOMAL      , db_taskmsg_write_nomal},
    {MSG_DB_READ_NOMALS      , db_taskmsg_read_nomals},
    {MSG_DB_WRITE_NOMALS     , db_taskmsg_write_nomals},
    {MSG_DB_READ_RECORD_START, db_taskmsg_read_record_start},
    {MSG_DB_READ_RECORD_STEP , db_taskmsg_read_record_step},
    {MSG_DB_READ_RECORD_END  , db_taskmsg_read_record_end},
    {MSG_DB_READ_RECORD      , NULL}, //组合操作完成
    {MSG_DB_WRITE_RECORD     , db_taskmsg_write_record},
    {MSG_DB_UPDATE_RECORD    , db_taskmsg_update_record},
    {MSG_DB_GET_RECORD_NUM   , db_taskmsg_get_record_num},
    {MSG_DB_GET_RECORD_DEPTH , db_taskmsg_get_record_depth},
    {MSG_DB_CLEAR_DATA       , db_taskmsg_clear_data},
    {MSG_DB_CLEAR_OAD_DATA   , db_taskmsg_clear_oad_data},
    {MSG_DB_RECOVER_DEFAULT  , db_taskmsg_recover_recover},

};
uint8 g_DBTaskMsgProcNum = sizeof(g_DBTaskMsgProc)/sizeof(DB_TASKMSG_PROC_T);

/**********************************************************************
* @name      : dbagent_taskmsg_entry
* @brief     ：启动task处理dbagent消息
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-24
* @Update    :
**********************************************************************/
void *dbagent_taskmsg_entry(void *pArg)
{
    uint8       recvbuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvbuf;
    int         recvlen = 0;
    uint32      sender = 0;
    uint8       i;
    int         agentfd = *((int *)pArg);

    /* 循环收消息 */
    for(;;)
    {
        recvlen = task_recv_msg(agentfd, recvbuf, TASK_MSG_MAX_LEN, &sender);
        if(recvlen > 0)
        {
            for(i = 0; i < g_DBTaskMsgProcNum; i++)
            {
                if(pMsg->msg_id == g_DBTaskMsgProc[i].msgid && NULL != g_DBTaskMsgProc[i].pMsgFunc)
                {
                    g_DBTaskMsgProc[i].pMsgFunc((uint8 *)pMsg, sender, g_SendFunc);
                }
            }
        }
    }
}

/**********************************************************************
* @name      : db_agent_create
* @brief     ：创建db代理并初始化 一个APP只能存在一个
* @param[in] ：uint8 type          代理类型 AGENT_TYPE_PROCESS AGENT_TYPE_THREAD
               uint32 taskid       内部通信用的taskid 低16位有效
               DBMSG_SEND_FUN func 对外消息发送回调函数
               uint8 clientnum     最大支持client数量
* @param[out]：
* @return    ：DB_AGENT            代理句柄 大于0有效
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
DB_AGENT db_agent_create(uint8 type, uint32 taskid, DBMSG_SEND_FUN func, uint8 clientnum)
{
    DB_AGENT agentfd = 0;

    if(NULL == func || taskid > 0xffff)
    {
        DPRINT("db agent cheate error input, %p, 0x%x\n", func, taskid);
        return -1;
    }
    
    if(AGENT_TYPE_THREAD == type)
    {
        /* 线程级别 */
        if(0 != db_clientinfo_init(clientnum))
        {
            close(agentfd);
            return -1;
        }
        g_dbAgentType = type;
        g_SendFunc = func;
        return 1;
    }
    else if(AGENT_TYPE_PROCESS == type)
    {
        /* 进程级别 */
        agentfd = task_msg_init(taskid & 0xffff);
        if(agentfd <= 0)
        {
            return -1;
        }
        
        g_dbAgentType = type;
        g_SendFunc = func;
         
        if(0 != db_clientinfo_init(clientnum))
        {
            close(agentfd);
            return -1;
        }

        /* 内部消息接收线程启动 */
        if(0 != pthread_create(&g_tAgentid, NULL, dbagent_taskmsg_entry, &agentfd))
        {
            g_dbAgentType = AGENT_TYPE_THREAD;
            g_SendFunc = NULL;
            db_clientinfo_free();
            close(agentfd);
            return -1;
        }
        sleep(1);
    }
    else
    {
        agentfd = -1;
    }
    return agentfd;
}

/**********************************************************************
* @name      : db_agent_close
* @brief     ：db操作关闭代理
* @param[in] ：DB_AGENT fd         句柄  
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-27
* @Update    :
**********************************************************************/
void db_agent_close(DB_AGENT fd)
{
    if(AGENT_TYPE_PROCESS == g_dbAgentType)
    {
        pthread_cancel(g_tAgentid);
        pthread_join(g_tAgentid, NULL);
        close(fd);
        g_tAgentid = 0;
    }
    g_dbAgentType = AGENT_TYPE_THREAD;
    g_SendFunc = NULL;
    db_clientinfo_free();
}

/**********************************************************************
* @name      : db_agent_proc_read_nomal
* @brief     ：进程级dbagent处理读普通数据返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_proc_read_nomal(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int               ret = 0;
    
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else /* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_READ_NOMAL, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    return ret;
}

/**********************************************************************
* @name      : db_agent_proc_write_nomal
* @brief     ：进程级dbagent处理写普通数据返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_proc_write_nomal(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int                ret = 0;
    
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else/* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_WRITE_NOMAL, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_agent_proc_read_nomals
* @brief     ：进程级dbagent处理读多个普通数据返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-28
* @Update    :
**********************************************************************/
int db_agent_proc_read_nomals(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int               ret = 0;
    
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else /* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_READ_NOMALS, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    return ret;
}

/**********************************************************************
* @name      : db_agent_proc_write_nomals
* @brief     ：进程级dbagent处理写多个普通数据返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-28
* @Update    :
**********************************************************************/
int db_agent_proc_write_nomals(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int                ret = 0;
    
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else/* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_WRITE_NOMALS, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_agent_proc_read_start
* @brief     ：进程级dbagent处理开始读记录型数据返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_proc_read_start(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int ret = 0;
        
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else/* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_READ_RECORD_START, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_agent_proc_read_step
* @brief     ：进程级dbagent处理单步读记录型数据返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_proc_read_step(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int                     ret = 0;
        
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else/* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_READ_RECORD_STEP, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_agent_proc_read_end
* @brief     ：进程级dbagent处理结束记录型数据读取返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_proc_read_end(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int                      ret = 0;
        
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else/* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_READ_RECORD_END, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_agent_proc_wtite_record
* @brief     ：进程级dbagent处理写记录型数据返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_proc_wtite_record(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int                 ret = 0;
    
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else/* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_WRITE_RECORD, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_agent_proc_updata_record
* @brief     ：进程级dbagent处理更新记录数据返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_proc_updata_record(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int                 ret = 0;
    
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else/* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_UPDATE_RECORD, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_agent_proc_get_num
* @brief     ：进程级dbagent处理获取存储记录条数返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_proc_get_num(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int     ret = 0;
    
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else/* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_GET_RECORD_NUM, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_agent_proc_get_depth
* @brief     ：进程级dbagent处理获取存储记录深度返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_proc_get_depth(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int    ret = 0;
    
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else/* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_GET_RECORD_DEPTH, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_agent_proc_clear_data
* @brief     ：进程级dbagent处理数据初始化返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_proc_clear_data(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int  ret = 0;
    
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else/* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_CLEAR_DATA, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_agent_proc_clear_oad
* @brief     ：进程级dbagent处理指定数据初始化返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_proc_clear_oad(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int     ret = 0;
    
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else/* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_CLEAR_OAD_DATA, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    
    return ret;
}

/**********************************************************************
* @name      : db_agent_proc_recover_recover
* @brief     ：进程级dbagent处理恢复出厂返回信息
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_proc_recover_recover(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int    ret = 0;
    
    if(AGENT_TYPE_THREAD == g_dbAgentType)
    {
        /* 多线程方式 */
        if(pMsg->msglen > 0 && pMsg->msglen <= ptClientInfo->ackbuflen && NULL != ptClientInfo->data)
        {
            ptClientInfo->datalen = pMsg->msglen;
            memcpy(ptClientInfo->data, pMsg->playload, pMsg->msglen);
        }
        pthread_cond_signal(&(ptClientInfo->notify));
    }
    else/* 多进程方式 */
    {
        ret = task_ack_syncmsg(fd, MSG_DB_RECOVER_DEFAULT, pMsg->playload, pMsg->msglen, ptClientInfo->taskid, 
                               ptClientInfo->taskindex);
    }
    
    return ret;
}


DB_MSG_PROC_T g_DBMsgProc[] = 
{
    {MSG_DB_READ_NOMAL,        DB_IOP_READ_NORMAL_DATA,  db_agent_proc_read_nomal},
    {MSG_DB_WRITE_NOMAL,       DB_IOP_WRITE_NORMAL_DATA, db_agent_proc_write_nomal},
    {MSG_DB_READ_NOMALS,       DB_IOP_READ_NORMAL_DATA,  db_agent_proc_read_nomal},
    {MSG_DB_WRITE_NOMALS,      DB_IOP_WRITE_NORMAL_DATA, db_agent_proc_write_nomal},
    {MSG_DB_READ_RECORD_START, DB_IOP_READ_RECORD_START, db_agent_proc_read_start},
    {MSG_DB_READ_RECORD_START, DB_IOP_READ_RECORD_START_FJ, db_agent_proc_read_start},
    {MSG_DB_READ_RECORD_STEP,  DB_IOP_READ_RECORD_STEP,  db_agent_proc_read_step},
    {MSG_DB_READ_RECORD_END,   DB_IOP_READ_RECORD_END,   db_agent_proc_read_end},
    {MSG_DB_READ_RECORD,       0,                        NULL},
    {MSG_DB_WRITE_RECORD,      DB_IOP_WRITE_RECORD,      db_agent_proc_wtite_record},
    {MSG_DB_UPDATE_RECORD,     DB_IOP_UPDATE_RECORD,     db_agent_proc_updata_record},
    {MSG_DB_GET_RECORD_NUM,    DB_IOP_GET_RECORD_NUM,    db_agent_proc_get_num},
    {MSG_DB_GET_RECORD_DEPTH,  DB_IOP_GET_RECORD_DEPTH,  db_agent_proc_get_depth},
    {MSG_DB_CLEAR_DATA,        DB_IOP_INIT_DATA,         db_agent_proc_clear_data},
    {MSG_DB_CLEAR_OAD_DATA,    DB_IOP_CLEAR_ONE_DATA,    db_agent_proc_clear_oad},
    {MSG_DB_CLEAR_OAD_DATA,    DB_IOP_CLEAR_ONE_DATA_FJ,    db_agent_proc_clear_oad},
    {MSG_DB_RECOVER_DEFAULT,   DB_IOP_RECOVERY_DEFAULT,  db_agent_proc_recover_recover},
};
uint8 g_DBMsgNum = sizeof(g_DBMsgProc)/sizeof(DB_MSG_PROC_T);

/**********************************************************************
* @name      : db_agent_msg_process
* @brief     ：进程级dbagent操作消息处理
* @param[in] ：DB_AGENT fd                    代理句柄
               MESSAGE_INFO_T *pMsg           消息结构体
               DBCLIENT_CACHE_T *ptClientInfo 客户端信息
* @param[out]：
* @return    ：0-成功/错误码(返回DB_AGENT_UNKONW_MSG, 由其他消息函数处理)
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_msg_process(DB_AGENT fd, MESSAGE_INFO_T *pMsg, DBCLIENT_CACHE_T *ptClientInfo)
{
    int ret = DB_AGENT_UNKONW_MSG;
    uint8 i;

    for(i = 0; i < g_DBMsgNum; i++)
    {
        if(g_DBMsgProc[i].iop == pMsg->IOP && NULL != g_DBMsgProc[i].pMsgFunc)
        {
            if(g_DBMsgProc[i].msgid == ptClientInfo->msgid)
            {
                ret = g_DBMsgProc[i].pMsgFunc(fd, pMsg, ptClientInfo);
                ptClientInfo->bwait = 0;
                return ret;
            }
        }
    }
    return ret;
}


/**********************************************************************
* @name      : db_agent_msg_entry
* @brief     ：db操作消息处理
* @param[in] ：DB_AGENT fd          代理句柄
               MESSAGE_INFO_T *pMsg 消息结构体
* @param[out]：
* @return    ：0-成功/错误码(返回DB_AGENT_UNKONW_MSG, 由其他消息函数处理)
* @Create    : 王津剑
* @Date      ：2019-12-30
* @Update    :
**********************************************************************/
int db_agent_msg_entry(DB_AGENT fd, MESSAGE_INFO_T *pMsg)
{
    int ret = 0;
    DBCLIENT_CACHE_T *ptClientInfo = NULL;
    uint32 ustime = 5;
    if(fd <= 0 && NULL == pMsg)
    {
        return DBMSG_ERR_INPUT;
    }

    /* 1 先判断消息IID是否本函数处理 */
    if(MESSAGE_IID_DB != pMsg->IID || pMsg->IOP < DB_IOP_READ_NORMAL_DATA)
    {
        return DB_AGENT_UNKONW_MSG;
    }

    /* 2 判断消息是否已经超时 */
    ptClientInfo = db_get_clientinfo((uint32)pMsg->label, NULL);
    if(NULL == ptClientInfo)
    {
        return DBMSG_ERR_REACH_MAX_CLIENT;
    }
    
    /* 3 2s加锁失败放弃消息 防止外部消息卡死 */
    if(0 != pthread_mutex_trylock(&(ptClientInfo->lock)))
    {
        while(ustime < 1000000)
        {
            usleep(ustime);
            if(0 == pthread_mutex_trylock(&(ptClientInfo->lock)))
            {
                goto lock;
            }
            ustime *= 10;
        }
        sleep(1);
        if(0 != pthread_mutex_trylock(&(ptClientInfo->lock)))
        {
            DPRINT("db msssage lock time out: client 0x%x, msgindex %u, index %u, taskindex %u\n", 
                   pMsg->label, pMsg->index, ptClientInfo->msgindex, ptClientInfo->taskindex);
            return DB_AGENT_DROP_MSG;
        }
    }

lock:
    if(FALSE == ptClientInfo->bwait || ptClientInfo->msgindex != pMsg->index)
    {
        pthread_mutex_unlock(&(ptClientInfo->lock));
        DPRINT("db msssage is time out: client 0x%x, msgindex %u, index %u, taskindex %u\n", 
               pMsg->label, pMsg->index, ptClientInfo->msgindex, ptClientInfo->taskindex);
        return DBMSG_ERR_OUT_TIME;
    }

    /* 4 消息处理*/
    ret = db_agent_msg_process(fd, pMsg, ptClientInfo);
    DPRINT("Agent proc db msg %d: client 0x%x, index %u, ret %d\n", pMsg->IOP, ptClientInfo->taskid, 
           pMsg->index, ret);
    pthread_mutex_unlock(&(ptClientInfo->lock));
    return ret;
}

