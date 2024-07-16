/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief：   高级APP对外通信模块 数据中心更新
* @date：    2020-3-5
* @history：
*********************************************************************
*/
#include "baseCommon.h"
#include "db_api.h"
#include "task_id.h"
#include "task_msg.h"
#include "msg_id.h"

#include "msg_base.h"
#include "db_update.h"


#define UPDATE_ALL     0xffff
#define UPOAD_NULL     0
#define UPMASK_ALL     0xffffffff
#define UPINIT_ALL     0xff
#define UPCLASS_ALL    0xffff

extern int GetEventNomalParam(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int GetInitEventInfo(int clientid, uint8 initType, uint8 logicId, uint16 classTag);
extern int UpdateLatestMeterReadDataList(int clientid, uint8 logicId, uint16 infoNum, uint32 oadMain, uint8 oadColsNum, uint32* oadCols);
extern void InitLatestMeterReadDataLock(void);
extern void Init_ac_data_lock(void);
extern int update_ac_data(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int comm_change_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern void comm_change_init();

/*
 * 数据中心更新
 * 逻辑设备号和信息点号为0xffff, 标识通配;
 * 筛选数据是OAD, 记录型数据更新匹配的主DI, 普通数据更新匹配的是第一个次DI
 * 掩码, 筛选数据和更新数据掩码部分相同, 则匹配
 * 需要关注的更新在下面数组中添加
*/
DB_UPDATE_T g_dbUpdateInfo[] =
{/* 逻辑设备号,  信息点号,   筛选数据,   掩码      , 普通数据更新函数,    记录型数据更新,   初始化函数 */
    {UPDATE_ALL, UPDATE_ALL, 0x0,        UPMASK_ALL, NULL,                UpdateLatestMeterReadDataList, InitLatestMeterReadDataLock},

#ifdef AREA_ZHEJIANG    
    {1         , 0         , 0x00100200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x00200200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x00500200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x00600200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x00700200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x00800200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x10100200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x10200200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x20000200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x20010200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x20020200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x20030200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x20140200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x200a0200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x200d0200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x200d0300, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x200d0400, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x200e0200, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x200e0300, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x200e0400, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    {1         , 0         , 0x20010600, UPMASK_ALL, update_ac_data,  NULL,             Init_ac_data_lock},
    
#else    
    {UPDATE_ALL, UPDATE_ALL, 0x20000200, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL}, //终端电压
    {UPDATE_ALL, UPDATE_ALL, 0x20010200, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL}, //终端电流
    {UPDATE_ALL, UPDATE_ALL, 0x30000800, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x30000900, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x30010800, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x30010900, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x30020800, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x30020900, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x30030800, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x30030900, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x30050800, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x30050900, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x301D0800, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x301D0900, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x301E0800, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x301E0900, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x31060600, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x31060800, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x31060900, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x31190800, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x31190900, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x300f0600, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x20140200, UPMASK_ALL, GetEventNomalParam,  NULL,             NULL},
#endif
    {7,          0,          0x45000200, 0xffc0ffff, comm_change_update,  NULL,             comm_change_init},
    {7,          0,          0x45000300, 0xffc0ffff, comm_change_update,  NULL,             NULL},
};

uint16 g_dbUpdateNum = sizeof(g_dbUpdateInfo)/sizeof(DB_UPDATE_T);

/*
 * 数据中心更新定制 针对没有主DI、只有一个次数据DI的类型
 * 数据类型标识 DATA_CLASS_E
 * 数据定制     TRUE-普通类型; FALSE-记录类型
 * 筛选数据
*/
DB_CUSTOM_T g_DBCustom[] = 
{/* 数据类型,  数据定制,   筛选数据 */

};
uint16 g_dbcustomNum = sizeof(g_DBCustom)/sizeof(DB_CUSTOM_T);


/**********************************************************************
* @name      : DB_INIT_INFO
* @brief     ：初始化事件
* @param[in] ：int clientid    db客户端
               uint8 initType  初始化类型
               uint8 logicId   逻辑设备号
               uint16 classTag 数据类型标记
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-03-6
* @Update    :
**********************************************************************/
int DB_INIT_INFO(int clientid, uint8 initType, uint8 logicId, uint16 classTag)
{
    return GetInitEventInfo(clientid, initType, logicId, classTag);
}

/**********************************************************************
* @name      : DB_Default_INFO
* @brief     ：初始化事件 恢复出厂设置
* @param[in] ：int clientid    db客户端
               uint8 initType  初始化类型
               uint8 logicId   逻辑设备号
               uint16 classTag 数据类型标记
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-03-6
* @Update    :
**********************************************************************/
int DB_Default_INFO(int clientid, uint8 initType, uint8 logicId, uint16 classTag)
{

    return 0;
}


/*
 * 数据中心初始化事件
 * 逻辑设备号为0xff, 标识通配
 * 数据类标识为0xffff, 标识通配
 * 需要关注的更新在下面数组中添加
*/
DBINIT_EVENT_T g_dbInitInfo[] =
{/*  逻辑设备号, 信息点号,   数据类标识, 回调函数,           初始化函数 */
    {0,          UPINIT_ALL, UPDATE_ALL, DB_INIT_INFO,       NULL},
    {1,          UPINIT_ALL, UPDATE_ALL, DB_Default_INFO,    NULL},
};

uint16 g_dbInitNum = sizeof(g_dbInitInfo)/sizeof(DBINIT_EVENT_T);


DBUPDATE_LIST_T g_dbUpdateList;

/**********************************************************************
* @name      : db_updata_init
* @brief     ：数据更新初始化 主要是锁
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-5
* @Update    :
**********************************************************************/
void db_updata_init()
{
    uint16 i;

    pthread_mutex_init(&g_dbUpdateList.flock, NULL);
    g_dbUpdateList.head = NULL;
    g_dbUpdateList.tail = NULL;

    for(i = 0; i < g_dbUpdateNum; i++)
    {
        if(g_dbUpdateInfo[i].initc)
        {
            g_dbUpdateInfo[i].initc();
        }
    }

    for(i = 0; i < g_dbInitNum; i++)
    {
        if(g_dbInitInfo[i].initc)
        {
            g_dbInitInfo[i].initc();
        }
    }
}

/**********************************************************************
* @name      : db_list_add
* @brief     ：增加数据中心更新列表
* @param[in] ：uint8 type    类型 0,1
               void *pMsg    消息 DB_UPDATE_MSG_T 或 DBINIT_EVENT_MSG_T
* @param[out]：
* @return    ：0-成功/错误
* @Create    : 王津剑
* @Date      ：2020-9-10
* @Update    :
**********************************************************************/
int db_list_add(uint8 type, void *pMsg)
{
    DBUPDATE_NODE_T *pNode;

    if(type > 1 || NULL == pMsg)
    {
        return -1;
    }

    pNode = (DBUPDATE_NODE_T *)malloc(sizeof(DBUPDATE_NODE_T));
    if(NULL == pNode)
    {
        return -2;
    }
    memset(pNode, 0, sizeof(DBUPDATE_NODE_T));

    pNode->type = type;
    if(0 == type)
    {
        memcpy(&(pNode->msg), pMsg, sizeof(DB_UPDATE_MSG_T));
    }
    else if(1 == type)
    {
        memcpy(&(pNode->msg), pMsg, sizeof(DBINIT_EVENT_MSG_T));
    }
    
    /* 添加到队列中 */
    pthread_mutex_lock(&g_dbUpdateList.flock);
    if(NULL == g_dbUpdateList.head)
    {
        g_dbUpdateList.head = pNode;
    }
    if(NULL != g_dbUpdateList.tail)
    {
        g_dbUpdateList.tail->pNext = pNode;
    }
    g_dbUpdateList.tail = pNode;

    pthread_mutex_unlock(&g_dbUpdateList.flock);
    return 0;
}

/**********************************************************************
* @name      : db_list_get
* @brief     ：获取数据中心更新列表
* @param[in] ：
* @param[out]：DBUPDATE_NODE_T *pNode   消息节点
* @return    ：0-成功/错误
* @Create    : 王津剑
* @Date      ：2020-9-10
* @Update    :
**********************************************************************/
int db_list_get(DBUPDATE_NODE_T *pNode)
{
    if(NULL == pNode)
    {
        return -1;
    }

    pthread_mutex_lock(&g_dbUpdateList.flock);
    if(NULL == g_dbUpdateList.head)
    {
        pthread_mutex_unlock(&g_dbUpdateList.flock);
        return -2;
    }

    memcpy(pNode, g_dbUpdateList.head, sizeof(DBUPDATE_NODE_T));

    free(g_dbUpdateList.head);
    g_dbUpdateList.head = pNode->pNext;
    if(NULL == g_dbUpdateList.head)
    {
        g_dbUpdateList.tail = NULL;
    }
    pthread_mutex_unlock(&g_dbUpdateList.flock);
    return 0;
}

/**********************************************************************
* @name      : db_update_entry
* @brief     ：启动更新数据消息任务
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-3-5
* @Update    :
**********************************************************************/
void *db_update_entry(void *pArg)
{
    int                 ret = 0;
    int                 recvret = 0;
    uint16              i;
    DB_CLIENT           clientid = 0;
    DBUPDATE_NODE_T     tMsgNode;
    DB_UPDATE_MSG_T    *pUpdateMsg = NULL;
    DBINIT_EVENT_MSG_T *pInitMsg = NULL;
    uint16           index = 0;

    clientid = db_client_create(DB_PD_AGENT, DB_PD_CLIENT_UPDATE);
    if(clientid <= 0)
    {
        PDDB_FMT_TRACE("Main update db db_client_create error!\n");
        usleep(100);
        exit(0); //进程退出
    }

    /* 循环收消息 */
    for(;;)
    {
        recvret = db_list_get(&tMsgNode);
        if(0 != recvret)
        {
            usleep(10000);
            continue;
        }

        if(0 == tMsgNode.type)
        {
            pUpdateMsg = &(tMsgNode.msg.tUpdate);
            if(TRUE == pUpdateMsg->bRecord)
            {
                /* 记录型数据更新 */
                index = pUpdateMsg->index[0];
                if(index < g_dbUpdateNum && NULL != g_dbUpdateInfo[index].rfunc)
                {
                    ret = g_dbUpdateInfo[index].rfunc(clientid, pUpdateMsg->logicId, pUpdateMsg->infoNum, 
                                                      pUpdateMsg->dataflag, pUpdateMsg->slavenum, pUpdateMsg->slavedata);
                    PDDB_FMT_TRACE("Update record 0x%x, ret %d, logic %d, infonum %d\n", 
                                   pUpdateMsg->dataflag, ret, pUpdateMsg->logicId, pUpdateMsg->infoNum);
                }
            }
            else
            {
                /* 普通型数据更新 */
                for(i = 0; i < pUpdateMsg->slavenum; i++)
                {
                    index = pUpdateMsg->index[i];
                    if(index < g_dbUpdateNum && NULL != g_dbUpdateInfo[index].func)
                    {
                        ret = g_dbUpdateInfo[index].func(clientid, pUpdateMsg->logicId, pUpdateMsg->infoNum, 
                                                         pUpdateMsg->slavedata[i]);
                        PDDB_FMT_TRACE("Update db 0x%x, ret %d, logic %d, infonum %d\n", 
                                       pUpdateMsg->slavedata[i], ret, pUpdateMsg->logicId, pUpdateMsg->infoNum);
                    }
                }
            }
        }
        if(1 == tMsgNode.type)
        {
            pInitMsg = &(tMsgNode.msg.tEvent);
            for(i = 0; i < g_dbInitNum; i++)
            {
                if(pInitMsg->initType == g_dbInitInfo[i].initType
                   && (pInitMsg->logicId == g_dbInitInfo[i].logicId || UPINIT_ALL == g_dbInitInfo[i].logicId)
                   && (pInitMsg->classTag == g_dbInitInfo[i].classTag || UPCLASS_ALL == g_dbInitInfo[i].classTag))
                {
                    if(g_dbInitInfo[i].func)
                    {
                        ret = g_dbInitInfo[i].func(clientid, pInitMsg->initType, pInitMsg->logicId,
                                                   pInitMsg->classTag);
                        PDDB_FMT_TRACE("INIT db ligic %d, ret %d, type %d, class %d\n",
                                        pInitMsg->logicId, ret, pInitMsg->initType, pInitMsg->classTag);
                    }
                    break;
                }
            }
        }
    }

    db_client_close(clientid);
}

char     g_updatastr[1024] = {0};
/**********************************************************************
* @name      : db_agent_msg_entry
* @brief     ：db操作消息处理
* @param[in] ：MESSAGE_INFO_T *pMsg 消息结构体
* @param[out]：
* @return    ：0-成功/错误码(返回DB_AGENT_UNKONW_MSG, 由其他消息函数处理)
* @Create    : 王津剑
* @Date      ：2019-3-5
* @Update    :
**********************************************************************/
int db_update_msg_proc(MESSAGE_INFO_T *pMsg)
{
    int                ret = 0;
    DB_UPDATE_MSG_T    tUpdateMsg;
    DBINIT_EVENT_MSG_T tInitMsg = {0};
    uint8             *pTemp = NULL;
    uint16             offset = 0;
    uint16             i, j;
    uint8              bIndexOK = FALSE;

    if(NULL == pMsg)
    {
        return -1;
    }

    /* 1 先判断消息IID是否本函数处理 */
    if(MESSAGE_IID_DB != pMsg->IID)
    {
        return DB_AGENT_UNKONW_MSG;
    }

    if(DB_IOP_UPDATE_EVENT == pMsg->IOP)
    {
        char   tempstring[512] = {0};
                
        pTemp = pMsg->playload;
        memset(&tUpdateMsg, 0, sizeof(DB_UPDATE_MSG_T));
        
        tUpdateMsg.logicId = *pTemp;
        pTemp++;
        
        MEMCPY_R(&tUpdateMsg.infoNum, pTemp, 2);
        pTemp += 2;

        MEMCPY_R(&tUpdateMsg.classtag, pTemp, 2);
        pTemp += 2;

        tUpdateMsg.bOADData = *pTemp;
        pTemp++;
        if(0 == tUpdateMsg.bOADData)
        {
            YCDB_FMT_TRACE("DB Update no data, logicId %d, infoNum %d, classtag %d\n", tUpdateMsg.logicId,
                           tUpdateMsg.infoNum, tUpdateMsg.classtag);
            return 0;
        }

        tUpdateMsg.bOptional = *pTemp;
        pTemp++;

        if(1 == tUpdateMsg.bOptional)
        {
            tUpdateMsg.dataflag = *(uint32 *)pTemp;
            pTemp += 4;
            tUpdateMsg.dataflag = ntohl(tUpdateMsg.dataflag);
        }
        else if(tUpdateMsg.bOptional > 1)
        {
            PDDB_FMT_TRACE("DB Update error msg %d %d, optional %d\n", tUpdateMsg.logicId, tUpdateMsg.infoNum, tUpdateMsg.bOptional);
            return -1;
        }
        
        tUpdateMsg.slavenum = MIN(*pTemp, DB_UPDATE_MAX_SLAVE);
        pTemp++;
        if(0 == tUpdateMsg.slavenum)
        {
            PDDB_FMT_TRACE("DB Update error msg %d %d master 0x%x, slavenum is 0\n", 
                           tUpdateMsg.logicId, tUpdateMsg.infoNum, tUpdateMsg.dataflag);
            return -1;
        }
        
        offset = sprintf(tempstring, "DB Update %d %d class %d, master 0x%x, slave:", tUpdateMsg.logicId, tUpdateMsg.infoNum, 
                         tUpdateMsg.classtag, tUpdateMsg.dataflag);
        for(i = 0; i < tUpdateMsg.slavenum; i++)
        {
            tUpdateMsg.slavedata[i] = *(uint32*)pTemp;
            pTemp += 4;
            tUpdateMsg.slavedata[i] = ntohl(tUpdateMsg.slavedata[i]);
            tUpdateMsg.index[i] = DB_DEFAULT_INDEX;

            offset += sprintf(tempstring + offset, " 0x%x", tUpdateMsg.slavedata[i]);
        }
        
        /* 判断是否记录数据 */
        if(1 == tUpdateMsg.bOptional || CLASS_DATA_EVENT == tUpdateMsg.classtag || CLASS_DATA_COLLEC == tUpdateMsg.classtag)
        {
            tUpdateMsg.bRecord = TRUE;
        }
        else
        {
            tUpdateMsg.bRecord = FALSE;
        }
        /* 定制化改变 */
        if(0 == tUpdateMsg.bOptional && 1 == tUpdateMsg.slavenum)
        {
            for(i = 0; i < g_dbcustomNum; i++)
            {
                if(g_DBCustom[i].classTag == tUpdateMsg.classtag && g_DBCustom[i].dataflag == tUpdateMsg.slavedata[0])
                {
                    tUpdateMsg.bRecord = ((TRUE == g_DBCustom[i].bNormal) ? FALSE : TRUE);
                    break;
                }
            }
        }

        PDDB_FMT_TRACE("%s %s\n", TRUE == tUpdateMsg.bRecord ? "Record" : "Normal", tempstring);

        if(TRUE == tUpdateMsg.bRecord)
        {
            /* 记录型获取index */
            for(i = 0; i < g_dbUpdateNum; i++)
            {
                if(NULL != g_dbUpdateInfo[i].rfunc &&
                   (tUpdateMsg.dataflag & g_dbUpdateInfo[i].maskInfo) == (g_dbUpdateInfo[i].dataflag & g_dbUpdateInfo[i].maskInfo)&&
                   (UPDATE_ALL == g_dbUpdateInfo[i].logicId || tUpdateMsg.logicId == g_dbUpdateInfo[i].logicId) &&
                   (UPDATE_ALL == g_dbUpdateInfo[i].infoNum || tUpdateMsg.infoNum == g_dbUpdateInfo[i].infoNum))
                {
                    tUpdateMsg.index[0] = i;
                    bIndexOK = TRUE;
                    break;
                }
            }
        }
        else
        {
            for(j = 0; j < tUpdateMsg.slavenum; j++)
            {
                for(i = 0; i < g_dbUpdateNum; i++)
                {
                    if(NULL != g_dbUpdateInfo[i].func &&
                       (tUpdateMsg.slavedata[j] & g_dbUpdateInfo[i].maskInfo) == (g_dbUpdateInfo[i].dataflag & g_dbUpdateInfo[i].maskInfo) && 
                       (UPDATE_ALL == g_dbUpdateInfo[i].logicId || tUpdateMsg.logicId == g_dbUpdateInfo[i].logicId) &&
                       (UPDATE_ALL == g_dbUpdateInfo[i].infoNum || tUpdateMsg.infoNum == g_dbUpdateInfo[i].infoNum))
                    {
                        tUpdateMsg.index[j] = i;
                        bIndexOK = TRUE;
                        break;
                    }
                }
            }
        }
        if(TRUE == bIndexOK)
        {
            /* 自己发给自己 */
            ret = db_list_add(0, &tUpdateMsg);
            if(0 != ret)
            {
                PDMSG_FMT_DEBUG("Update add failed, db 0x%x, logic %d, infonum %d\n", 
                                tUpdateMsg.dataflag, tUpdateMsg.logicId, tUpdateMsg.infoNum);
            }
            else
            {
                PDDB_FMT_TRACE("Update add, db 0x%x, logic %d, infonum %d\n", 
                                tUpdateMsg.dataflag, tUpdateMsg.logicId, tUpdateMsg.infoNum);
            }
        }
    }
    else if(DB_IOP_INIT_EVENT == pMsg->IOP)
    {
        pTemp = pMsg->playload;
        tInitMsg.initType = pTemp[offset++];
        if(0 == tInitMsg.initType)
        {
            tInitMsg.logicId = pTemp[offset++];
            if(1 == pTemp[offset++])
            {
                tInitMsg.classTag = *(uint16 *)(pTemp + offset);
                tInitMsg.classTag = ntohs(tInitMsg.classTag);
            }
        }
        else if(1 == tInitMsg.initType)
        {
            tInitMsg.logicId = pTemp[offset++];
        }
        else
        {
            PDDB_FMT_TRACE("Unknow dbinit msg type %d\n", tInitMsg.initType);
            return ret;
        }
        PDDB_FMT_TRACE("DB Init msg type %d, logic %d, class %d\n", tInitMsg.initType, tInitMsg.logicId,
                        tInitMsg.classTag);
        /* 自己发给自己 */
        ret = db_list_add(1, &tInitMsg);
        if(0 != ret)
        {
            PDDB_FMT_TRACE("Send dbinit msg failed, ret %d, logic %d, type %d\n", ret, tInitMsg.logicId,
                           tInitMsg.initType);
        }
    }
    else
    {
        return DB_AGENT_UNKONW_MSG;
    }
    return ret;
}


