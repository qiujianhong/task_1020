/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块主线程功能
* @date：    2020-3-6
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "msg_id.h"
#include "iniparser.h"

#include "common_base.h"
#include "comm_ipc.h"
#include "comm_msg.h"
#include "comm_eth.h"
#include "comm_ppp.h"
#include "comm_listen.h"
#include "comm_db.h"

uint16 g_104Port = 2404;
uint8  g_104ListenNum = COMM_LISTEN_MAX;

uint8 g_nIPCRun = 0;
COMM_IPC_HANDLE_T g_tIPCHandle;

uint8 g_nOutMsgRun = 0;
COMM_OUTMSG_HANDLE_T g_tCommOutMsg;

uint8 g_nCommDBRun = 0;
COMM_DB_HANDLE_T g_CommCBCheck;

uint8 g_nListenRun = 0;
COMM_QUEUE_LIST_T g_tListenRecv;
COMM_QUEUE_LIST_T g_tListenSend;
COMM_LISTEN_HANDLE_T g_tListenHandle;

uint8 g_nPPP0Run = 0;
uint8 g_nPPP0Rest = 0;
COMM_QUEUE_LIST_T g_tPPP0Recv;
COMM_QUEUE_LIST_T g_tPPP0Send;
COMM_PPP_HANDLE_T g_tPPP0Handle;

uint8 g_nPPP1Run = 0;
uint8 g_nPPP1Rest = 0;
COMM_QUEUE_LIST_T g_tPPP1Recv;
COMM_QUEUE_LIST_T g_tPPP1Send;
COMM_PPP_HANDLE_T g_tPPP1Handle;

uint8 g_nETH0Run = 0;
uint8 g_nETH0Rest = 0;
COMM_QUEUE_LIST_T g_tETH0Recv;
COMM_QUEUE_LIST_T g_tETH0Send;
COMM_ETH_HANDLE_T g_tETH0Handle;

uint8 g_nETH1Run = 0;
uint8 g_nETH1Rest = 0;
COMM_QUEUE_LIST_T g_tETH1Recv;
COMM_QUEUE_LIST_T g_tETH1Send;
COMM_ETH_HANDLE_T g_tETH1Handle;

uint8 g_nIPCok = 0;
uint8 g_AppStar[5] = {0};


COMM_TRHEAD_T g_tCommThread[] = 
{/*  线程ID, 启动判断函数,      入口函数,         入参,             线程运行标记, 重启标记位 */
    {0,      NULL,              ipc_sock_entry,    &g_tIPCHandle,   &g_nIPCRun,   NULL},
    {0,      NULL,              comm_outmsg_entry, &g_tCommOutMsg,  &g_nOutMsgRun,NULL},
    {0,      NULL,              comm_db_entry,     &g_CommCBCheck,  &g_nCommDBRun,NULL},
    {0,      comm_listen_start, comm_listen_entry, &g_tListenHandle,&g_nListenRun,NULL},
    {0,      comm_ppp_start,    comm_ppp_entry ,   &g_tPPP0Handle,  &g_nPPP0Run,  &g_nPPP0Rest},
    {0,      comm_ppp_start,    comm_ppp_entry ,   &g_tPPP1Handle,  &g_nPPP1Run,  &g_nPPP1Rest},
    {0,      comm_eth_start,    comm_eth_entry ,   &g_tETH0Handle,  &g_nETH0Run,  &g_nETH0Rest},
    {0,      comm_eth_start,    comm_eth_entry ,   &g_tETH1Handle,  &g_nETH1Run,  &g_nETH1Rest},
};
uint8 g_CommThreadNum = sizeof(g_tCommThread)/sizeof(COMM_TRHEAD_T);


/**********************************************************************
* @name      : comm_task_getarg
* @brief     ：通信模块获取配置文件中的通信参数
* @param[in] ：char *filename   配置文件
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-7
* @Update    :
**********************************************************************/
void comm_task_getarg(char *filename)
{
    dictionary *ini = NULL;
    int    intvalue = 0;
    char  *stringvalue = NULL;

    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        intvalue = iniparser_getint(ini, "LISTEN_PORT", 2404);
        g_104Port = intvalue;
        intvalue = iniparser_getint(ini, "LISTEN_NUM", COMM_LISTEN_MAX);
        g_104ListenNum = intvalue;
        
        /* 获取以太网口名称 */
        stringvalue = iniparser_getstring(ini, "ETH_CONF:ETH1_NAME", "FE0");
        if(NULL != stringvalue)
        {
            eth_set_name(0, stringvalue);
        }
        stringvalue = iniparser_getstring(ini, "ETH_CONF:ETH2_NAME", "FE1");
        if(NULL != stringvalue)
        {
            eth_set_name(1, stringvalue);
        }

        iniparser_freedict(ini);
    }
}

/**********************************************************************
* @name      : comm_get_listen_master
* @brief     ：获取监听到的主站
* @param[in] ：uint8 Max    最大缓存数量
* @param[out]：uint32 *IP   地址
               uint16 *PORT 端口
* @return    ：链接数量
* @Create    : 王津剑
* @Date      ：2020-6-4
* @Update    : 
**********************************************************************/
uint8 comm_get_listen_master(uint32 *IP, uint16 *PORT, uint8 Max)
{
    uint8 i = 0;
    uint8 num = 0;

    for(i = 0; i < g_tListenHandle.num; i++)
    {
        if(g_tListenHandle.accept_fd[i] > 0)
        {
            IP[num] = g_tListenHandle.cIP[num];
            PORT[num] = g_tListenHandle.cPort[num];
            num++;
            if(num >= Max)
            {
                break;
            }
        }
    }
    return num;
}

/**********************************************************************
* @name      : comm_check_ppp_online
* @brief     ：判断主站是否连接
* @param[in] ：uint8 id     设备id 0-1
               uint32 ip
               uint16 port
* @param[out]：
* @return    ：TRUE-FALSE
* @Create    : 王津剑
* @Date      ：2020-8-29
* @Update    : 
**********************************************************************/
uint8 comm_check_ppp_online(uint8 id, uint32 ip, uint16 port)
{
    uint8 i = 0;
    
    if(0 == id)
    {
        for(i = 0; i < g_tPPP0Handle.masterNUM; i++)
        {
            if(ip == g_tPPP0Handle.ip[i] && port == g_tPPP0Handle.port[i] && g_tPPP0Handle.connect_fd[i] > 0)
            {
                return TRUE;
            }
        }
    }
    else if(1 == id)
    {
        for(i = 0; i < g_tPPP1Handle.masterNUM; i++)
        {
            if(ip == g_tPPP1Handle.ip[i] && port == g_tPPP1Handle.port[i] && g_tPPP1Handle.connect_fd[i] > 0)
            {
                return TRUE;
            }
        }
    }
    
    return FALSE;
}

/**********************************************************************
* @name      : comm_check_eth_online
* @brief     ：判断主站是否连接
* @param[in] ：uint8 id     设备id 0-1
               uint32 ip
               uint16 port
* @param[out]：
* @return    ：TRUE-FALSE
* @Create    : 王津剑
* @Date      ：2020-8-29
* @Update    : 
**********************************************************************/
uint8 comm_check_eth_online(uint8 id, uint32 ip, uint16 port)
{
    uint8 i = 0;
    
    if(0 == id)
    {
        for(i = 0; i < g_tETH0Handle.masterNUM; i++)
        {
            if(ip == g_tETH0Handle.ip[i] && port == g_tETH0Handle.port[i] && g_tETH0Handle.connect_fd[i] > 0)
            {
                return TRUE;
            }
        }
    }
    else if(1 == id)
    {
        for(i = 0; i < g_tETH1Handle.masterNUM; i++)
        {
            if(ip == g_tETH1Handle.ip[i] && port == g_tETH1Handle.port[i] && g_tETH1Handle.connect_fd[i] > 0)
            {
                return TRUE;
            }
        }
    }
    
    return FALSE;
}

/**********************************************************************
* @name      : comm_task_init
* @brief     ：通信模块数据初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-4-12
* @Update    :
**********************************************************************/
void comm_task_init()
{
    /* PPP0初始化 */
    memset(&g_tPPP0Recv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tPPP0Send, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tPPP0Handle, 0, sizeof(COMM_PPP_HANDLE_T));
    pthread_mutex_init(&g_tPPP0Recv.f_lock, NULL);
    pthread_mutex_init(&g_tPPP0Send.f_lock, NULL);
    g_tPPP0Handle.no            = 0;
    g_tPPP0Handle.ptRecvList    = &g_tPPP0Recv;
    g_tPPP0Handle.ptSendList    = &g_tPPP0Send;
    g_tPPP0Handle.pRunFlag      = &g_nPPP0Run;
    g_tPPP0Handle.pIPCok        = &g_nIPCok;
    g_tPPP0Handle.pAppReStart   = &g_AppStar[0];

    /* PPP1初始化 */
    memset(&g_tPPP1Recv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tPPP1Send, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tPPP1Handle, 0, sizeof(COMM_PPP_HANDLE_T));
    pthread_mutex_init(&g_tPPP1Recv.f_lock, NULL);
    pthread_mutex_init(&g_tPPP1Send.f_lock, NULL);
    g_tPPP1Handle.no            = 1;
    g_tPPP1Handle.ptRecvList    = &g_tPPP1Recv;
    g_tPPP1Handle.ptSendList    = &g_tPPP1Send;
    g_tPPP1Handle.pRunFlag      = &g_nPPP1Run;
    g_tPPP1Handle.pIPCok        = &g_nIPCok;
    g_tPPP1Handle.pAppReStart   = &g_AppStar[1];

    /* ETH0初始化 */
    memset(&g_tETH0Recv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tETH0Send, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tETH0Handle, 0, sizeof(COMM_PPP_HANDLE_T));
    pthread_mutex_init(&g_tETH0Recv.f_lock, NULL);
    pthread_mutex_init(&g_tETH0Send.f_lock, NULL);
    g_tETH0Handle.no            = 0;
    g_tETH0Handle.ptRecvList    = &g_tETH0Recv;
    g_tETH0Handle.ptSendList    = &g_tETH0Send;
    g_tETH0Handle.pRunFlag      = &g_nETH0Run;
    g_tETH0Handle.pIPCok        = &g_nIPCok;
    g_tETH0Handle.pAppReStart   = &g_AppStar[2];
    
    /* ETH1初始化 */
    memset(&g_tETH1Recv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tETH1Send, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tETH1Handle, 0, sizeof(COMM_PPP_HANDLE_T));
    pthread_mutex_init(&g_tETH1Recv.f_lock, NULL);
    pthread_mutex_init(&g_tETH1Send.f_lock, NULL);
    g_tETH1Handle.no            = 1;
    g_tETH1Handle.ptRecvList    = &g_tETH1Recv;
    g_tETH1Handle.ptSendList    = &g_tETH1Send;
    g_tETH1Handle.pRunFlag      = &g_nETH1Run;
    g_tETH1Handle.pIPCok        = &g_nIPCok;
    g_tETH1Handle.pAppReStart   = &g_AppStar[3];

    /* Listen初始化 */
    memset(&g_tListenRecv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tListenSend, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tListenHandle, 0, sizeof(COMM_LISTEN_HANDLE_T));
    pthread_mutex_init(&g_tListenRecv.f_lock, NULL);
    pthread_mutex_init(&g_tListenSend.f_lock, NULL);
    g_tListenHandle.port          = g_104Port;
    g_tListenHandle.num           = g_104ListenNum;
    g_tListenHandle.ptRecvList    = &g_tListenRecv;
    g_tListenHandle.ptSendList    = &g_tListenSend;
    g_tListenHandle.pRunFlag      = &g_nListenRun;
    g_tListenHandle.pIPCok        = &g_nIPCok;
    g_tListenHandle.pAppReStart   = &g_AppStar[4];

    /* IPC初始化 */
    memset(&g_tIPCHandle, 0, sizeof(COMM_IPC_HANDLE_T));
    g_tIPCHandle.ptGPRS0Recv    = &g_tPPP0Recv;
    g_tIPCHandle.ptGPRS0Send    = &g_tPPP0Send;
    g_tIPCHandle.ptGPRS1Recv    = &g_tPPP1Recv;
    g_tIPCHandle.ptGPRS1Send    = &g_tPPP1Send;
    g_tIPCHandle.ptETH0Recv     = &g_tETH0Recv;
    g_tIPCHandle.ptETH0Send     = &g_tETH0Send;
    g_tIPCHandle.ptETH1Recv     = &g_tETH1Recv;
    g_tIPCHandle.ptETH1Send     = &g_tETH1Send;
    g_tIPCHandle.ptListenRecv   = &g_tListenRecv;
    g_tIPCHandle.ptListenSend   = &g_tListenSend;

    g_tIPCHandle.pRunFlag       = &g_nIPCRun;
    g_tIPCHandle.pIPCok         = &g_nIPCok;
    g_tIPCHandle.pGprs0ReStart  = &g_AppStar[0];
    g_tIPCHandle.pGprs1ReStart  = &g_AppStar[1];
    g_tIPCHandle.pEth0ReStart   = &g_AppStar[2];
    g_tIPCHandle.pEth1ReStart   = &g_AppStar[3];
    g_tIPCHandle.pListenReStart = &g_AppStar[4];
        
    /* 对外通信线程初始化 */
    memset(&g_tCommOutMsg, 0, sizeof(COMM_OUTMSG_HANDLE_T));
    g_tCommOutMsg.pRunFlag   = &g_nOutMsgRun;
    comm_outtimer_add(&g_tCommOutMsg, 10,comm_timer_getDCM, &g_tPPP0Handle);
    comm_outtimer_add(&g_tCommOutMsg, 10,comm_timer_getDCM, &g_tPPP1Handle);
    comm_outtimer_add(&g_tCommOutMsg, 2, comm_timer_getETH, &g_tETH0Handle);
    comm_outtimer_add(&g_tCommOutMsg, 2, comm_timer_getETH, &g_tETH1Handle);

    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETDIAL, 1, comm_ppp_dcmmsg, &g_tPPP0Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETDIAL, 2, comm_ppp_dcmmsg, &g_tPPP1Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETETH, 1, comm_eth_statusmsg, &g_tETH0Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETETH, 2, comm_eth_statusmsg, &g_tETH1Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_ETHEVENT, 0xffff, comm_eth_eventmsg, NULL);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_DIALEVENT, 0xffff, comm_ppp_eventmsg, NULL);

    /* 数据中心更新初始化 */
    memset(&g_CommCBCheck, 0, sizeof(COMM_DB_HANDLE_T));
    g_CommCBCheck.pRunFlag   = &g_nCommDBRun;
    comm_dbcheck_add(&g_CommCBCheck, comm_getcheck_pppcfg, &g_tPPP0Handle, &g_nPPP0Rest);
    comm_dbcheck_add(&g_CommCBCheck, comm_getcheck_pppcfg, &g_tPPP1Handle, &g_nPPP1Rest);
    comm_dbcheck_add(&g_CommCBCheck, comm_getcheck_ethcfg, &g_tETH0Handle, &g_nETH0Rest);
    comm_dbcheck_add(&g_CommCBCheck, comm_getcheck_ethcfg, &g_tETH1Handle, &g_nETH1Rest);
}

/**********************************************************************
* @name      : comm_task_entry
* @brief     ：通信模块线程入口
* @param[in] ：void *pArg    参数
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-6
* @Update    :
**********************************************************************/
int comm_task_entry(void *pArg)
{
    uint8 i;
    COMM_TRHEAD_T *pCommThread = NULL;
    uint8          closeFlag = 0;
    int            errCode = 0;

    /*1. 数据初始化 */
    comm_task_init();

    /* 2. 线程启动与监控 */
    while(1)
    {
        for(i = 0; i < g_CommThreadNum; i++)
        {
            pCommThread = &g_tCommThread[i];
            
            closeFlag = 0;
            /* 判断线程是否需要停止 */
            if(pCommThread->pStartFlag)
            {
                closeFlag = *pCommThread->pStartFlag;
            }
            if(0 == closeFlag && 0 != pCommThread->nThreadId)
            {
                (*pCommThread->pRunFlag)++;
                closeFlag = (*pCommThread->pRunFlag >= COMM_TRHEAD_FLAG_RESTART) ? 1 : 0;
            }
            if(closeFlag && 0 != pCommThread->nThreadId)
            {
                pthread_cancel(pCommThread->nThreadId);
                errCode = pthread_join(pCommThread->nThreadId, NULL);
                if(errCode != 0)
                {
                    COMM_FMT_DEBUG("comm cancel thread error %d!\n", i);
                }
                pCommThread->nThreadId = 0;
            }

            /* 再判断启动 */
            if(0 != pCommThread->nThreadId)
            {
                continue;
            }
            if(NULL != pCommThread->pCheckFunc)
            {
                if(FALSE == pCommThread->pCheckFunc(pCommThread->pArg))
                {
                    continue;
                }
            }
            
            errCode = pthread_create(&pCommThread->nThreadId, NULL, pCommThread->pEntryFunc, pCommThread->pArg);
            if (errCode != 0)
            {
                COMM_FMT_DEBUG("comm Thread %d Create error! err:%d\n", i, errCode);
                pCommThread->nThreadId = 0;
            }
            else
            {
                if(pCommThread->pStartFlag)
                {
                    *pCommThread->pStartFlag = 0;
                }
                *pCommThread->pRunFlag = 0;

                COMM_FMT_TRACE("comm Thread %d create OK!\n", i);
            }
        }
        sleep(1);
    }

    return 0;
}

