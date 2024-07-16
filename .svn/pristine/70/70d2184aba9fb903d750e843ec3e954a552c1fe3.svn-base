
#include "pdsAll.h"
#include "pdsDB_update.h"
#include "pdsCommon.h"
#include "area.h"
#define UPDATE_ALL     0xffff
#define UPOAD_NULL     0
#define UPMASK_ALL     0xffffffff
#define UPINIT_ALL     0xff
#define UPCLASS_ALL    0xffff

DB_UPDATE_T g_dbUpdateInfo[] = 
{/* 逻辑设备号,  信息点号,   筛选数据,   掩码      , 普通数据更新函数,    记录型数据更新, 初始化函数 */
    //{UPDATE_ALL, UPDATE_ALL, 0x0,        UPMASK_ALL, NULL, UpdateLatestMeterReadDataList, InitLatestMeterReadDataLock},
    {UPDATE_ALL, UPDATE_ALL, 0x60000200, UPMASK_ALL,DBGetMeterFile ,NULL,  NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x40010200, UPMASK_ALL, ReloadLocalAddr, NULL, NULL},
#ifdef AREA_HUNAN
    {UPDATE_ALL, UPDATE_ALL, 0x60510400, UPMASK_ALL, ReloadLineLossPeriod, NULL, NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x31F20600, UPMASK_ALL, ReloadImpedenceLimit, NULL, NULL},
#endif
	{UPDATE_ALL, UPDATE_ALL, 0x60510400, UPMASK_ALL, ReloadLineLossPeriod, NULL, NULL},
	{UPDATE_ALL, UPDATE_ALL, 0x60120200, UPMASK_ALL, appdb_get_task_list, NULL, 		  task_list_lock_init}, 
	{UPDATE_ALL, UPDATE_ALL, 0x60140200, UPMASK_ALL, appdb_get_plan_normal_list, NULL,	  plan_normal_list_lock_init},
#ifdef AREA_JIANGXI
    //{UPDATE_ALL, UPDATE_ALL, 0x4E070200, UPMASK_ALL, ReadTopoData, NULL, NULL},
    {UPDATE_ALL, UPDATE_ALL, 0x60510400, UPMASK_ALL, ReloadLineLossPeriod, NULL, NULL},
#endif
};


uint16 g_dbUpdateNum = sizeof(g_dbUpdateInfo)/sizeof(DB_UPDATE_T);

DBINIT_EVENT_T g_dbInitInfo[] = 
{/*  初始化类型, 逻辑设备号, 数据类标识, 回调函数,           初始化函数 */
    //{0,          UPINIT_ALL, UPDATE_ALL, DB_INIT_INFO,       DB_INIT_INIT},
   // {1,          UPINIT_ALL, UPDATE_ALL, DB_Default_INFO,    DB_Default_INIT},
};

uint16 g_dbInitNum = sizeof(g_dbInitInfo)/sizeof(DBINIT_EVENT_T);

DBUPDATE_LIST_T g_dbUpdateList;
extern uint8 gdelay;
/**********************************************************************
* @name      : db_updata_init
* @brief     ：数据更新初始化 主要是锁
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
void db_update_init()
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
* @name      : db_update_list_add
* @brief     ：增加数据中心更新列表
* @param[in] ：uint8 type    类型 0,1
               void *pMsg    消息 DB_UPDATE_MSG_T 或 DBINIT_EVENT_MSG_T
* @param[out]：
* @return    ：0-成功/错误

**********************************************************************/
int db_update_list_add(uint8 type, void *pMsg)
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
* @name      : db_update_list_get
* @brief     ：获取数据中心更新列表
* @param[in] ：
* @param[out]：DBUPDATE_NODE_T *pNode   消息节点
* @return    ：0-成功/错误
**********************************************************************/
int db_update_list_get(DBUPDATE_NODE_T *pNode)
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
* @name      : db_update_func
* @brief     ：db操作消息处理
* @param[in] ：MESSAGE_INFO_T *pMsg 消息结构体
* @param[out]：
* @return    ：0-成功/错误码

**********************************************************************/
int db_update_func(MESSAGE_INFO_T *pMsg)
{
    int                ret = 0;
    DB_UPDATE_MSG_T    tUpdateMsg;
    DBINIT_EVENT_MSG_T tInitMsg = {0};
    uint8             *pTemp = NULL;
    uint16             offset = 0;
    uint16             i, j;
    uint8              bIndexOK = FALSE;
    char     g_updatastr[1024] = {0};
    
    if(NULL==pMsg)
    {
        return -1;
    }
    if(MESSAGE_IID_DB != pMsg->IID)
    {
        return -1;
    }
    if(DB_IOP_UPDATE_EVENT == pMsg->IOP)
    {
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
            return 0;
        }

        tUpdateMsg.bOptional = *pTemp;
        pTemp++;

        /* 逻辑设备7的属于配网，直接过滤 */
        if (tUpdateMsg.logicId == 7)
        {
            return -1;
        }

        if(1 == tUpdateMsg.bOptional)
        {
            tUpdateMsg.dataflag = *(uint32 *)pTemp;
            pTemp += 4;
            tUpdateMsg.dataflag = ntohl(tUpdateMsg.dataflag);
        }
        else if(tUpdateMsg.bOptional > 1)
        {
            return -1;
        }
        
        tUpdateMsg.slavenum = MIN(*pTemp, DB_UPDATE_MAX_SLAVE);
        pTemp++;
        if(0 == tUpdateMsg.slavenum)
        {
            return -1;
        }
        
        offset = sprintf(g_updatastr, "DB Update %d %d class %d, master 0x%x, slave:", tUpdateMsg.logicId, tUpdateMsg.infoNum, 
                         tUpdateMsg.classtag, tUpdateMsg.dataflag);
        for(i = 0; i < tUpdateMsg.slavenum; i++)
        {
            tUpdateMsg.slavedata[i] = *(uint32*)pTemp;
            pTemp += 4;
            tUpdateMsg.slavedata[i] = ntohl(tUpdateMsg.slavedata[i]);
            tUpdateMsg.index[i] = DB_DEFAULT_INDEX;

            offset += sprintf(g_updatastr + offset, " 0x%x", tUpdateMsg.slavedata[i]);
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

        //PDSDB_FMT_TRACE("%s %s\n", TRUE == tUpdateMsg.bRecord ? "Record" : "Normal", g_updatastr);
        memset(g_updatastr, 0, offset);
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
            ret = db_update_list_add(0, &tUpdateMsg);
            if(0 != ret)
            {
                PDSDB_FMT_DEBUG("Update add failed, db 0x%x, logic %d, infonum %d\n", 
                                tUpdateMsg.dataflag, tUpdateMsg.logicId, tUpdateMsg.infoNum);
            }
            else
            {
                PDSDB_FMT_DEBUG("Update add, db 0x%x, logic %d, infonum %d\n", 
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
 
            return -1;
        }
        PDSDB_FMT_TRACE("DB Init msg type %d, logic %d, class %d\n", tInitMsg.initType, tInitMsg.logicId, 
                        tInitMsg.classTag);

        /* 自己发给自己 */
        ret = db_update_list_add(1, &tInitMsg);
        if(0 != ret)
        {
            PDSDB_FMT_DEBUG("DB init add failed, type %d, logic %d, infonum %d\n",
                            tInitMsg.initType, tInitMsg.logicId, tInitMsg.initType);
        }
    }
    return 0;
}

/**********************************************************************
* @name      : db_update_entry
* @brief     ：启动更新数据消息任务
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
**********************************************************************/
void *db_update_entry(void *pArg)
{
    int              ret = 0;
    int              recvret = 0;
    uint16           i;
    DB_CLIENT        clientid = 0;
    DBUPDATE_NODE_T     tMsgNode;
    DB_UPDATE_MSG_T    *pUpdateMsg = NULL;
    DBINIT_EVENT_MSG_T *pInitMsg = NULL;
    uint16           index = 0;

    clientid = db_client_create(DB_PDS_AGENT, DB_PDS_CLIENT_DBUPDATE);
    if(clientid <= 0)
    {
        PDSDB_FMT_DEBUG("DB_PDS_CLIENT_DBUPDATE db_client_create ERROR \n");
        exit(0); 
    }

    memset(&tMsgNode, 0, sizeof(DBUPDATE_NODE_T));
    /* 循环收消息 */
    for(;;)
    {
        recvret = db_update_list_get(&tMsgNode);
        if(0 != recvret)
        {
            usleep(10000);
            continue;
        }

        if(0 == tMsgNode.type)    /* 数据更新 */
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
                    PDSDB_FMT_TRACE("Update record 0x%08x, di 0x%08x, ret %d, logic %d, infonum %d\n", 
                                   pUpdateMsg->dataflag, g_dbUpdateInfo[index].dataflag, ret, pUpdateMsg->logicId, pUpdateMsg->infoNum);
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
                        PDSDB_FMT_TRACE("Update normal 0x%08x, di 0x%08x, ret %d, logic %d, infonum %d\n", 
                                       pUpdateMsg->slavedata[i], g_dbUpdateInfo[index].dataflag, ret, pUpdateMsg->logicId, pUpdateMsg->infoNum);
                    }
                }
            }
        }
        else if(1 == tMsgNode.type)  /* 初始化 */
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
                        PDSDB_FMT_TRACE("INIT db ligic %d, ret %d, type %d, class %d\n", 
                                        pInitMsg->logicId, ret, pInitMsg->initType, pInitMsg->classTag);
                    }
                    break;
                }
            }

        }

        memset(&tMsgNode, 0, sizeof(DBUPDATE_NODE_T));
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }

    }
    
    db_client_close(clientid);
    return 0;
}


