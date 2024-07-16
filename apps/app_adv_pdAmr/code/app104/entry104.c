/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   104规约解析模块
* @date：    2020-3-9
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "db_api.h"
#include "task_id.h"
#include "framecomm.h"

#include "protocol104.h"
#include "app104_base.h"
#include "app104_ipc.h"
#include "app104_msg.h"
#include "app104_xml.h"

#include "pw_main.h"

#define APP_TRHEAD_FLAG_MUX       60

typedef void *(*EntryFun)(void *);
typedef struct tagApp698Thread
{
    pthread_t          nThreadId;   /* 线程ID */
    EntryFun           pEntryFunc;  /* 线程入口函数 */
    void              *pArg;        /* 函数入参 */
    uint8             *pRunFlag;    /* 线程运行标记, 线程执行设置为0 */
}APP104_TRHEAD_T;


uint8        g_IpcRecvFlag = 0;
IPC_THREAD_T g_t104IpcRecv = {&g_IpcRecvFlag};

uint8        g_OutMsgFlag = 0;
APPOUTMSG_THREAD_T g_tOutMsg = {&g_OutMsgFlag};

APP104_TRHEAD_T g_tThread[] = 
{/*  线程ID, 入口函数,              入参,                     线程运行标记 */
    {0,      advPd_appipc_entry,    &g_t104IpcRecv,           &g_IpcRecvFlag},
    {0,      advPd_app104msg_entry, &g_tOutMsg,               &g_OutMsgFlag},
    {0,      up_recv_deal_thread,   &g_sUpRecvDealThreadArg,  &g_nUpRecvDealThreadFlag},
    {0,      down_device_thread,    &g_sDownDeviceThreadArg,  &g_nDownDeviceThreadFlag},
    {0,      monit_stat_thread,     &g_sMonitStatThreadArg,   &g_nMonitStatThreadFlag},
    
};
uint8 g_TrheadNum = sizeof(g_tThread)/sizeof(APP104_TRHEAD_T);


/**********************************************************************
* @name      : advPd_app104task_getarg
* @brief     ：104规约调度参数获取
* @param[in] ：char *filename   配置文件
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-7
* @Update    :
**********************************************************************/
void advPd_app104task_getarg(char *filename)
{
}

/**********************************************************************
* @name      : advPd_app104task_entry
* @brief     ：104调度模块入口函数
* @param[in] ：void *pArg    参数
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-9
* @Update    :
**********************************************************************/
int advPd_app104task_entry(void *pArg)
{
    uint8   i;
    int     errCode = 0;
    APP104_TRHEAD_T *pThread = NULL;

    /* 初始化 */
    advPd_comm_msginit(); //缓存消息初始化
    
    sleep(1); //等待一会儿, 等待通信模块
    advPd_appipc_init();  //通信模块交互初始化
    pw_main_init();  //配网104协议基本参数初始化

    while(1)
    {
        for(i = 0; i < g_TrheadNum; i++)
        {
            pThread = &(g_tThread[i]);
            if(pThread->nThreadId <= 0)
            {
                /* 启动线程 */
                errCode = pthread_create(&pThread->nThreadId, NULL, pThread->pEntryFunc, pThread->pArg);
                if (errCode != 0)
                {
                    APP104_FMT_DEBUG("Thread %d Create error! err:%d\n", i, errCode);
                    pThread->nThreadId = 0;
                    continue;
                }
            }
            (*pThread->pRunFlag)++;
            if(*pThread->pRunFlag < APP_TRHEAD_FLAG_MUX)
            {
                continue;
            }
            /* 关闭线程 */
            pthread_cancel(pThread->nThreadId);
            errCode = pthread_join(pThread->nThreadId, NULL);
            if(errCode != 0)
            {
                APP104_FMT_DEBUG("cancel thread error %d!\n", i);
            }
            pThread->nThreadId = 0;
        }
        sleep(1);
    }
}

