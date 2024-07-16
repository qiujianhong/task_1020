/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块其他功能：外部消息收发\配置操作
* @date：    2020-3-6
* @history： 
*********************************************************************
*/

#include "baseCommon.h"
#include "task_msg.h"
#include "db_api.h"
#include "task_id.h"
#include "msg_id.h"

#include "common_base.h"
#include "comm_db.h"

pthread_mutex_t  g_fCommCfg;
uint8            g_bCommChange = FALSE;
        
/**********************************************************************
* @name      : comm_change_init
* @brief     ：通信模块数据更新初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-8-28
* @Update    : 
**********************************************************************/
void comm_change_init()
{
    pthread_mutex_init(&g_fCommCfg, NULL);
    g_bCommChange = FALSE;
}

/**********************************************************************
* @name      : comm_change_setflag
* @brief     ：通信模块数据更新设置
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-8-28
* @Update    :
**********************************************************************/
void comm_change_setflag(uint8 flag)
{
    PTHREAD_LOCK_SAFE(&g_fCommCfg);
    g_bCommChange = flag;
    PTHREAD_UNLOCK_SAFE(&g_fCommCfg);
}

/**********************************************************************
* @name      : comm_change_getflag
* @brief     ：获取通信模块数据更新设置
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-8-28
* @Update    :
**********************************************************************/
uint8 comm_change_getflag()
{
    uint8 flag = 0;
    PTHREAD_LOCK_SAFE(&g_fCommCfg);
    flag = g_bCommChange;
    PTHREAD_UNLOCK_SAFE(&g_fCommCfg);
    return flag;
}

/**********************************************************************
* @name      : comm_change_update
* @brief     ：数据更新
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-8-28
* @Update    :
**********************************************************************/
int comm_change_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    if(PD_LOGIC_ID != logicId)
    {
        /* 逻辑设备号为7 配网使用 */
        return 1;
    }

    if(0x45300200 || 0x45000200 == value || 0x45000300 == value || 0x45010200 == value || 0x45010300 == value
       || 0x45100200 == value || 0x45100300 == value || 0x45110200 == value || 0x45110300 == value)
    {
        comm_change_setflag(TRUE);
    }
    return 0;
}

/**********************************************************************
* @name      : comm_dbcheck_add
* @brief     ：配置消息收发线程
* @param[in] ：COMM_DB_HANDLE_T *ptHandle
               COMM_DBCHECK_T pFunc  判断函数
               void *pArg            存储地址
               uint8 *pRestart       外部重启标记
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
void comm_dbcheck_add(COMM_DB_HANDLE_T *ptHandle, CheckDBFUN pFunc, void *pArg, uint8 *pRestart)
{
    if(ptHandle->unNum >= 8 || NULL == pFunc)
    {
        COMM_FMT_DEBUG("comm_cfgmsg_add failed\n");
        return;
    }

    ptHandle->tCheck[ptHandle->unNum].pCheckFunc = pFunc;
    ptHandle->tCheck[ptHandle->unNum].pCfg = pArg;
    ptHandle->tCheck[ptHandle->unNum].pRestart = pRestart;

    ptHandle->unNum++;
}

/**********************************************************************
* @name      : comm_db_entry
* @brief     ：配置消息收发线程
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-8-28
* @Update    :
**********************************************************************/
void *comm_db_entry(void *pArg)
{
    uint8  i;
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    COMM_DB_HANDLE_T *ptHandle = (COMM_DB_HANDLE_T *)pArg;

    ptHandle->dbclient = db_client_create(DB_PD_AGENT, DB_PD_CLIENT_COMM);
    if(ptHandle->dbclient <= 0)
    {
        *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
        COMM_FMT_DEBUG("Comm db_client_create create failed!");
        return 0;
    }

    /* 初始化读取一次 */
    for(i = 0; i < ptHandle->unNum; i++)
    {
        *ptHandle->pRunFlag = 0;
        if(ptHandle->tCheck[i].pCheckFunc)
        {
            ptHandle->tCheck[i].pCheckFunc(ptHandle->dbclient, ptHandle->tCheck[i].pCfg, 
                                           ptHandle->tCheck[i].pRestart);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    while(1)
    {
        *ptHandle->pRunFlag = 0;
        
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        if(curtime.tv_sec - oldtime.tv_sec > 5 && TRUE == comm_change_getflag())
        {
            /* 配置查询与变更, 防止数据更新消息丢失 */
            comm_change_setflag(FALSE);
            for(i = 0; i < ptHandle->unNum; i++)
            {
                *ptHandle->pRunFlag = 0;
                if(ptHandle->tCheck[i].pCheckFunc)
                {
                    ptHandle->tCheck[i].pCheckFunc(ptHandle->dbclient, ptHandle->tCheck[i].pCfg, 
                                                   ptHandle->tCheck[i].pRestart);
                }
            }
            oldtime = curtime;
        }
        else if(curtime.tv_sec - oldtime.tv_sec > 60)
        {
            /* 防止数据中心启动时没有参数返回，1min读取一次 */
            for(i = 0; i < ptHandle->unNum; i++)
            {
                *ptHandle->pRunFlag = 0;
                if(ptHandle->tCheck[i].pCheckFunc)
                {
                    ptHandle->tCheck[i].pCheckFunc(ptHandle->dbclient, ptHandle->tCheck[i].pCfg, 
                                                   ptHandle->tCheck[i].pRestart);
                }
            }
            oldtime = curtime;
        }
        
        sleep(1);
    }
    pthread_exit(NULL);
}


