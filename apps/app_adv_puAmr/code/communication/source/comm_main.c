/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块主线程功能
* @date：    2019-10-31
* @history： 
*********************************************************************
*/

#include "baseCommon.h"
#include "msg_id.h"
#include "uart_api.h"
#include "ble.h"
#include "radio.h"
#include "comm.h"
#include "app_comm.h"
#include "ppp_comm.h"
#include "eth_comm.h"
#include "ipc_comm.h"
#include "rs232_comm.h"
#include "msg_comm.h"
#include "proxy_comm.h"
#include "sms_comm.h"
#include "listen_comm.h"
#include "radio_comm.h"

uint8               *g_pETH0State = NULL; //以太网状态
OOP_MASTERPARAMS_T  *g_pETH0Master = NULL;

uint8 g_nPPP0Run = 0;
uint8 g_nPPP0Rest = 0;
COMM_QUEUE_LIST_T g_tPPP0Recv;
COMM_QUEUE_LIST_T g_tPPP0RecvBak;
COMM_QUEUE_LIST_T g_tPPP0Send;
COMM_PPP_HANDLE_T g_tPPP0Handle;

uint8 g_nPPP1Run = 0;
uint8 g_nPPP1Rest = 0;
COMM_QUEUE_LIST_T g_tPPP1Recv;
COMM_QUEUE_LIST_T g_tPPP1RecvBak;
COMM_QUEUE_LIST_T g_tPPP1Send;
COMM_PPP_HANDLE_T g_tPPP1Handle;

uint8 g_nETH0Run = 0;
uint8 g_nETH0Rest = 0;
COMM_QUEUE_LIST_T g_tETH0Recv;
COMM_QUEUE_LIST_T g_tETH0RecvBak;
COMM_QUEUE_LIST_T g_tETH0Send;
COMM_ETH_HANDLE_T g_tETH0Handle;

uint8 g_nETH1Run = 0;
uint8 g_nETH1Rest = 0;
COMM_QUEUE_LIST_T g_tETH1Recv;
COMM_QUEUE_LIST_T g_tETH1RecvBak;
COMM_QUEUE_LIST_T g_tETH1Send;
COMM_ETH_HANDLE_T g_tETH1Handle;

uint8 g_nBlueRun = 0;
uint8 g_nBlueRest = 0;
BOOL  g_nBlueStartup = TRUE;
BOOL  g_nBluePINen = FALSE;
COMM_QUEUE_LIST_T g_tBlueRecv;
COMM_QUEUE_LIST_T g_tBlueSend;
COMM_BLUE_HANDLE_T g_tBlueHandle;

uint8 g_nRadioRun = 0;
uint8 g_nRadioRest = 0;
BOOL  g_nRadioStartup = TRUE;
BOOL  g_nRadioPINen = FALSE;
COMM_QUEUE_LIST_T g_tRadioRecv;
COMM_QUEUE_LIST_T g_tRadioSend;
COMM_RADIO_HANDLE_T g_tRadioHandle;

uint8 g_nProxyRun = 0;
uint8 g_nProxyRest = 0;
COMM_QUEUE_LIST_T g_tProxyRecv;
COMM_QUEUE_LIST_T g_tProxySend;
COMM_PROXY_HANDLE_T g_tProxyHandle;

uint8 g_nListenRun = 0;
COMM_QUEUE_LIST_T g_tListenRecv;
COMM_QUEUE_LIST_T g_tListenSend;
uint16            g_EthHeart = 0;
uint16            g_PPPHeart = 0;
COMM_LISTEN_HANDLE_T g_tListenHandle;

uint8 g_n485Run = 0;
uint8 g_n485Rest = 0;
BOOL  g_n485Startup = TRUE;
COMM_QUEUE_LIST_T g_t485Recv;
COMM_QUEUE_LIST_T g_t485Send;
COMM_485_HANDLE_T g_t485Handle;

uint8 g_n232Run = 0;
uint8 g_n232Rest = 0;
COMM_QUEUE_LIST_T g_t232Recv;
COMM_QUEUE_LIST_T g_t232Send;
COMM_232_HANDLE_T g_t232Handle;

uint8 g_nSMS0Run = 0;
COMM_QUEUE_LIST_T g_tSMS0Recv;
COMM_QUEUE_LIST_T g_tSMS0Send;
COMM_SMS_HANDLE_T g_tSMS0Handle;

uint8 g_nSMS1Run = 0;
COMM_QUEUE_LIST_T g_tSMS1Recv;
COMM_QUEUE_LIST_T g_tSMS1Send;
COMM_SMS_HANDLE_T g_tSMS1Handle;

uint8 g_nIRRun = 0;
uint8 g_nIRRest = 0;
COMM_QUEUE_LIST_T g_tIRRecv;
COMM_QUEUE_LIST_T g_tIRSend;
COMM_232_HANDLE_T g_tIRHandle;

uint8 g_nAppTransRun = 0;
COMM_QUEUE_LIST_T g_tAppTransRecv;
COMM_QUEUE_LIST_T g_tAppTransSend;
COMM_APP_TRANS_HANDLE_T g_tAppTransHandle;

uint8 g_nIPCRun = 0;
COMM_IPC_HANDLE_T g_tIPCHandle;

uint8 g_nCfgMsgRun = 0;
COMM_CFGMSG_HANDLE_T g_tCfgMsg;

uint8 g_nOutMsgRun = 0;
COMM_OUTMSG_HANDLE_T g_tCommOutMsg;

uint8 g_nIPCok = 0;
uint8 g_AppStar[13] = {0};

COMM_TRHEAD_T g_tCommThread[] = 
{/*  线程ID, 启动判断函数,    入口函数,         入参,            线程运行标记, 重启标记位 */
    {0,      NULL,            comm_outmsg_entry,&g_tCommOutMsg,  &g_nOutMsgRun,NULL,          NULL},
    {0,      NULL,            ipc_sock_entry,   &g_tIPCHandle,   &g_nIPCRun,   NULL,          NULL},
    {0,      NULL,            comm_cfgmsg_entry,&g_tCfgMsg,      &g_nCfgMsgRun,NULL,          NULL},
    {0,      comm_ppp_start,  comm_ppp0_entry , &g_tPPP0Handle,  &g_nPPP0Run,  &g_nPPP0Rest,  NULL},
//    {0,      comm_ppp_start,  comm_ppp1_entry , &g_tPPP1Handle,  &g_nPPP1Run,  &g_nPPP1Rest,  NULL},
    {0,      comm_eth_start,  comm_eth0_entry , &g_tETH0Handle,  &g_nETH0Run,  &g_nETH0Rest,  NULL},
//    {0,      comm_eth_start,  comm_eth1_entry , &g_tETH1Handle,  &g_nETH1Run,  &g_nETH1Rest,  NULL},
#if defined PRODUCT_CCU || defined PRODUCT_ECU || defined PRODUCT_SCU
    {0,      bluetooth_start, bluetooth_entry , &g_tBlueHandle,  &g_nBlueRun,  &g_nBlueRest,  &g_nBlueStartup},
#endif
   // {0,      comm_listen_start,comm_listen_entry, &g_tListenHandle,&g_nListenRun,NULL,        NULL},
    {0,      comm_proxy_start,comm_proxy_entry, &g_tProxyHandle, &g_nProxyRun, &g_nProxyRest, NULL},
    {0,      comm_rs485_start,comm_rs485_entry, &g_t485Handle,   &g_n485Run,   &g_n485Rest,   &g_n485Startup},
    {0,      comm_smd_start,  comm_sms_entry,   &g_tSMS0Handle,  &g_nSMS0Run,  NULL,          NULL},
//    {0,      comm_smd_start,  comm_sms_entry,   &g_tSMS1Handle,  &g_nSMS1Run,  NULL,          NULL},
#if defined PRODUCT_CCU || defined PRODUCT_ZCU_3 || defined PRODUCT_ZCU_1
    {0,      comm_rs232_start,comm_rs232_entry, &g_t232Handle,   &g_n232Run,   &g_n232Rest,   NULL},
#endif
#if defined PRODUCT_CCU || defined PRODUCT_ZCU_3
    {0,      comm_infrared_start,comm_infrared_entry, &g_tIRHandle, &g_nIRRun,    NULL,       NULL},
#endif
#ifdef PRODUCT_ZCU_1
    {0,      comm_radio_start,comm_radio_entry, &g_tRadioHandle, &g_nRadioRun,    NULL,       NULL},
#endif

    {0,      comm_app_trans_start,comm_app_trans_entry, &g_tAppTransHandle, &g_nAppTransRun, NULL},
};
uint8 g_CommThreadNum = sizeof(g_tCommThread)/sizeof(COMM_TRHEAD_T);

/**********************************************************************
* @name      : set_run_flag_ble
* @brief     ：线程启动标识
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
void set_startup_flag_ble(BOOL bRun)
{
    g_nBlueStartup = bRun;
    return;
}

/**********************************************************************
* @name      : set_run_flag_RS485
* @brief     ：线程启动标识
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
void set_startup_flag_RS485(BOOL bRun)
{
    g_n485Startup = bRun;
    return;
}

/**********************************************************************
* @name      : set_pin_flag_ble
* @brief     ：蓝牙PIN功能使能
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
void set_pin_flag_ble(BOOL bEn)
{
    g_nBluePINen = bEn;
    return;
}

/**********************************************************************
* @name      : get_pin_flag_ble
* @brief     ：蓝牙PIN功能使能
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
BOOL get_pin_flag_ble(void)
{
    return g_nBluePINen;
}

/**********************************************************************
* @name      : comm_data_initproc
* @brief     ：通信模块数据初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-23
* @Update    :
**********************************************************************/
int comm_data_initproc()
{
    g_pETH0State = &g_tETH0Handle.bEthLink;
    g_pETH0Master = &g_tETH0Handle.tEth.tMaster;

    /* PPP0初始化 */
    memset(&g_tPPP0Recv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tPPP0Send, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tPPP0Handle, 0, sizeof(COMM_PPP_HANDLE_T));
    pthread_mutex_init(&g_tPPP0Recv.f_lock, NULL);
    pthread_mutex_init(&g_tPPP0RecvBak.f_lock, NULL);
    pthread_mutex_init(&g_tPPP0Send.f_lock, NULL);
    g_tPPP0Handle.no            = 0;
    g_tPPP0Handle.ptRecvList    = &g_tPPP0Recv;
    g_tPPP0Handle.ptRecvListBak = &g_tPPP0RecvBak;
    g_tPPP0Handle.ptSendList    = &g_tPPP0Send;
    g_tPPP0Handle.pRunFlag      = &g_nPPP0Run;
    g_tPPP0Handle.pIPCok        = &g_nIPCok;
    g_tPPP0Handle.pAppReStart   = &g_AppStar[0];
    g_tPPP0Handle.pETHStatus    = g_pETH0State;
    g_tPPP0Handle.pETHMaster    = g_pETH0Master;
    g_tPPP0Handle.MixHeart      = &g_PPPHeart;

    /* PPP1初始化 */
    memset(&g_tPPP1Recv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tPPP1Send, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tPPP1Handle, 0, sizeof(COMM_PPP_HANDLE_T));
    pthread_mutex_init(&g_tPPP1Recv.f_lock, NULL);
    pthread_mutex_init(&g_tPPP1RecvBak.f_lock, NULL);
    pthread_mutex_init(&g_tPPP1Send.f_lock, NULL);
    g_tPPP1Handle.no            = 1;
    g_tPPP1Handle.ptRecvList    = &g_tPPP1Recv;
    g_tPPP1Handle.ptRecvListBak = &g_tPPP1RecvBak;
    g_tPPP1Handle.ptSendList    = &g_tPPP1Send;
    g_tPPP1Handle.pRunFlag      = &g_nPPP1Run;
    g_tPPP1Handle.pIPCok        = &g_nIPCok;
    g_tPPP1Handle.pAppReStart   = &g_AppStar[1];
    g_tPPP1Handle.pETHStatus    = g_pETH0State;
    g_tPPP1Handle.pETHMaster    = g_pETH0Master;
    g_tPPP1Handle.MixHeart      = &g_PPPHeart;

    /* ETH0初始化 */
    memset(&g_tETH0Recv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tETH0Send, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tETH0Handle, 0, sizeof(COMM_ETH_HANDLE_T));
    pthread_mutex_init(&g_tETH0Recv.f_lock, NULL);
    pthread_mutex_init(&g_tETH0RecvBak.f_lock, NULL);
    pthread_mutex_init(&g_tETH0Send.f_lock, NULL);
    g_tETH0Handle.no            = 0;
    g_tETH0Handle.ptRecvList    = &g_tETH0Recv;
    g_tETH0Handle.ptRecvListBak = &g_tETH0RecvBak;
    g_tETH0Handle.ptSendList    = &g_tETH0Send;
    g_tETH0Handle.pRunFlag      = &g_nETH0Run;
    g_tETH0Handle.pIPCok        = &g_nIPCok;
    g_tETH0Handle.MixHeart      = &g_EthHeart;;
    g_tETH0Handle.pAppReStart   = &g_AppStar[2];
    
    /* ETH1初始化 */
    memset(&g_tETH1Recv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tETH1Send, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tETH1Handle, 0, sizeof(COMM_ETH_HANDLE_T));
    pthread_mutex_init(&g_tETH1Recv.f_lock, NULL);
    pthread_mutex_init(&g_tETH1RecvBak.f_lock, NULL);
    pthread_mutex_init(&g_tETH1Send.f_lock, NULL);
    g_tETH1Handle.no            = 1;
    g_tETH1Handle.ptRecvList    = &g_tETH1Recv;
    g_tETH1Handle.ptRecvListBak = &g_tETH1RecvBak;
    g_tETH1Handle.ptSendList    = &g_tETH1Send;
    g_tETH1Handle.pRunFlag      = &g_nETH1Run;
    g_tETH1Handle.pIPCok        = &g_nIPCok;
    g_tETH1Handle.pAppReStart   = &g_AppStar[3];
    
    /* 蓝牙 串口初始化 */
    memset(&g_tBlueRecv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tBlueSend, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tBlueHandle, 0, sizeof(COMM_BLUE_HANDLE_T));
    pthread_mutex_init(&g_tBlueRecv.f_lock, NULL);
    pthread_mutex_init(&g_tBlueSend.f_lock, NULL);
    g_tBlueHandle.ptRecvList = &g_tBlueRecv;
    g_tBlueHandle.ptSendList = &g_tBlueSend;
    g_tBlueHandle.pRunFlag   = &g_nBlueRun;
    g_tBlueHandle.pIPCok     = &g_nIPCok;
    g_tBlueHandle.pAppReStart= &g_AppStar[4];

    /* 电台初始化 */
    memset(&g_tRadioRecv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tRadioSend, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tRadioHandle, 0, sizeof(COMM_RADIO_HANDLE_T));
    pthread_mutex_init(&g_tRadioRecv.f_lock, NULL);
    pthread_mutex_init(&g_tRadioSend.f_lock, NULL);
    g_tRadioHandle.ptRecvList = &g_tRadioRecv;
    g_tRadioHandle.ptSendList = &g_tRadioSend;
    g_tRadioHandle.pRunFlag   = &g_nRadioRun;
    g_tRadioHandle.pIPCok     = &g_nIPCok;
    g_tRadioHandle.pAppReStart= &g_AppStar[12];

    /* 以太网监听初始化 */
    memset(&g_tListenRecv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tListenSend, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tListenHandle, 0, sizeof(COMM_LISTEN_HANDLE_T));
    pthread_mutex_init(&g_tListenRecv.f_lock, NULL);
    pthread_mutex_init(&g_tListenSend.f_lock, NULL);
    g_tListenHandle.ptRecvList = &g_tListenRecv;
    g_tListenHandle.ptSendList = &g_tListenSend;
    g_tListenHandle.pRunFlag   = &g_nListenRun;
    g_tListenHandle.pIPCok     = &g_nIPCok;
    g_tListenHandle.ethHeart   = &g_EthHeart;
    g_tListenHandle.pppHeart   = &g_PPPHeart;
    g_tListenHandle.pAppReStart= &g_AppStar[5];
    g_tListenHandle.ethReStart = &g_AppStar[2];
    g_tListenHandle.pppReStart = &g_AppStar[0];

    /* 通信代理初始化 */
    memset(&g_tProxyRecv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tProxySend, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tProxyHandle, 0, sizeof(COMM_PROXY_HANDLE_T));
    pthread_mutex_init(&g_tProxyRecv.f_lock, NULL);
    pthread_mutex_init(&g_tProxySend.f_lock, NULL);
    g_tProxyHandle.ptSendList = &g_tProxySend;
    g_tProxyHandle.pRunFlag   = &g_nProxyRun;
    g_tProxyHandle.pIPCok     = &g_nIPCok;
    g_tProxyHandle.pAppReStart= &g_AppStar[6];
#if 0
    /* 第4路485 串口初始化 */
    memset(&g_t485Recv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_t485Send, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_t485Handle, 0, sizeof(COMM_485_HANDLE_T));
    pthread_mutex_init(&g_t485Recv.f_lock, NULL);
    pthread_mutex_init(&g_t485Send.f_lock, NULL);
    g_t485Handle.ptRecvList = &g_t485Recv;
    g_t485Handle.ptSendList = &g_t485Send;
    g_t485Handle.pRunFlag   = &g_n485Run;
    g_t485Handle.pIPCok     = &g_nIPCok;
    g_t485Handle.pAppReStart= &g_AppStar[7];
#else
    /* 第485 串口初始化 */
    memset(&g_t485Recv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_t485Send, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_t485Handle, 0, sizeof(COMM_485_HANDLE_T));
    pthread_mutex_init(&g_t485Recv.f_lock, NULL);
    pthread_mutex_init(&g_t485Send.f_lock, NULL);
    g_t485Handle.ptRecvList = &g_t485Recv;
    g_t485Handle.ptSendList = &g_t485Send;
    g_t485Handle.pRunFlag   = &g_n485Run;
    g_t485Handle.pIPCok     = &g_nIPCok;
    g_t485Handle.pAppReStart= &g_AppStar[7];
    g_t485Handle.workNO     = 255;
    g_t485Handle.uartMode[0].modeinfo.port.portType = UART_TYPE_485;
    g_t485Handle.uartMode[0].modeinfo.port.portNo   = 1;
    g_t485Handle.uartMode[1].modeinfo.port.portType = UART_TYPE_485;
    g_t485Handle.uartMode[1].modeinfo.port.portNo   = 2;
    g_t485Handle.param.baudRate = UART_BAUD_9600;
    g_t485Handle.param.dataBits = 8;
    g_t485Handle.param.parity = UART_PARITY_EVEN;
    g_t485Handle.param.stopBits = 1;
    g_t485Handle.param.flowCtrl = UART_FLOWCTRL_NONE;
#endif
    /* SMS0初始化 */
    memset(&g_tSMS0Recv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tSMS0Send, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tSMS0Handle, 0, sizeof(COMM_SMS_HANDLE_T));
    pthread_mutex_init(&g_tSMS0Recv.f_lock, NULL);
    pthread_mutex_init(&g_tSMS0Send.f_lock, NULL);
    g_tSMS0Handle.no            = 0;
    g_tSMS0Handle.ptRecvList    = &g_tSMS0Recv;
    g_tSMS0Handle.ptSendList    = &g_tSMS0Send;
    g_tSMS0Handle.pRunFlag      = &g_nSMS0Run;
    g_tSMS0Handle.pIPCok        = &g_nIPCok;
    g_tSMS0Handle.pAppReStart   = &g_AppStar[8];

    /* SMS1初始化 */
    memset(&g_tSMS1Recv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tSMS1Send, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tSMS1Handle, 0, sizeof(COMM_SMS_HANDLE_T));
    pthread_mutex_init(&g_tSMS1Recv.f_lock, NULL);
    pthread_mutex_init(&g_tSMS1Send.f_lock, NULL);
    g_tSMS1Handle.no            = 1;
    g_tSMS1Handle.ptRecvList    = &g_tSMS1Recv;
    g_tSMS1Handle.ptSendList    = &g_tSMS1Send;
    g_tSMS1Handle.pRunFlag      = &g_nSMS1Run;
    g_tSMS1Handle.pIPCok        = &g_nIPCok;
    g_tSMS1Handle.pAppReStart   = &g_AppStar[9];

    /* 232串口初始化 */
    memset(&g_t232Recv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_t232Send, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_t232Handle, 0, sizeof(COMM_232_HANDLE_T));
    pthread_mutex_init(&g_t232Recv.f_lock, NULL);
    pthread_mutex_init(&g_t232Send.f_lock, NULL);
    g_t232Handle.ptRecvList = &g_t232Recv;
    g_t232Handle.ptSendList = &g_t232Send;
    g_t232Handle.pRunFlag   = &g_n232Run;
    g_t232Handle.pIPCok     = &g_nIPCok;
    g_t232Handle.pAppReStart= &g_AppStar[10];

    /* 红外 串口初始化 */
    memset(&g_tIRRecv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tIRSend, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tIRHandle, 0, sizeof(COMM_232_HANDLE_T));
    pthread_mutex_init(&g_tIRRecv.f_lock, NULL);
    pthread_mutex_init(&g_tIRSend.f_lock, NULL);
    g_tIRHandle.ptRecvList = &g_tIRRecv;
    g_tIRHandle.ptSendList = &g_tIRSend;
    g_tIRHandle.pRunFlag   = &g_nIRRun;
    g_tIRHandle.pIPCok     = &g_nIPCok;
    g_tIRHandle.pAppReStart= &g_AppStar[11];

        /* APP透传 */
    memset(&g_tAppTransRecv, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tAppTransSend, 0, sizeof(COMM_QUEUE_LIST_T));
    memset(&g_tAppTransHandle, 0, sizeof(COMM_SMS_HANDLE_T));
    pthread_mutex_init(&g_tAppTransRecv.f_lock, NULL);
    pthread_mutex_init(&g_tAppTransSend.f_lock, NULL);
    g_tAppTransHandle.ptRecvList = &g_tAppTransRecv;
    g_tAppTransHandle.ptSendList = &g_tAppTransSend;
    g_tAppTransHandle.pRunFlag   = &g_nAppTransRun;
    g_tAppTransHandle.pIPCok     = &g_nIPCok;

    /* IPC初始化 */
    memset(&g_tIPCHandle, 0, sizeof(COMM_IPC_HANDLE_T));
    g_tIPCHandle.ptGPRS0Recv    = &g_tPPP0Recv;
    g_tIPCHandle.ptGPRS0RecvBak = &g_tPPP0RecvBak;
    g_tIPCHandle.ptGPRS0Send    = &g_tPPP0Send;
    g_tIPCHandle.ptGPRS1Recv    = &g_tPPP1Recv;
    g_tIPCHandle.ptGPRS1RecvBak = &g_tPPP1RecvBak;
    g_tIPCHandle.ptGPRS1Send    = &g_tPPP1Send;
    g_tIPCHandle.ptETH0Recv     = &g_tETH0Recv;
    g_tIPCHandle.ptETH0RecvBak  = &g_tETH0RecvBak;
    g_tIPCHandle.ptETH0Send     = &g_tETH0Send;
    g_tIPCHandle.ptETH1Recv     = &g_tETH1Recv;
    g_tIPCHandle.ptETH1RecvBak  = &g_tETH1RecvBak;
    g_tIPCHandle.ptETH1Send     = &g_tETH1Send;
    g_tIPCHandle.ptListenRecv   = &g_tListenRecv;
    g_tIPCHandle.ptListenSend   = &g_tListenSend;
    g_tIPCHandle.ptBlueRecv     = &g_tBlueRecv;
    g_tIPCHandle.ptBlueSend     = &g_tBlueSend;
    g_tIPCHandle.ptRadioRecv    = &g_tRadioRecv;
    g_tIPCHandle.ptRadioSend    = &g_tRadioSend;
    g_tIPCHandle.ptProxyRecv    = &g_tProxyRecv;
    g_tIPCHandle.ptProxySend    = &g_tProxySend; 
    g_tIPCHandle.pt485Recv      = &g_t485Recv;
    g_tIPCHandle.pt485Send      = &g_t485Send;
    g_tIPCHandle.ptSMS0Recv     = &g_tSMS0Recv;
    g_tIPCHandle.ptSMS0Send     = &g_tSMS0Send;
    g_tIPCHandle.ptSMS1Recv     = &g_tSMS1Recv;
    g_tIPCHandle.ptSMS1Send     = &g_tSMS1Send;
    g_tIPCHandle.pt232Recv      = &g_t232Recv;
    g_tIPCHandle.pt232Send      = &g_t232Send;
    g_tIPCHandle.ptIRRecv       = &g_tIRRecv;
    g_tIPCHandle.ptIRSend       = &g_tIRSend;
    g_tIPCHandle.ptAppTransRecv  = &g_tAppTransRecv;
    g_tIPCHandle.ptAppTransSend  = &g_tAppTransSend;

    g_tIPCHandle.pRunFlag       = &g_nIPCRun;
    g_tIPCHandle.pIPCok         = &g_nIPCok;
    g_tIPCHandle.pGprs0ReStart  = &g_AppStar[0];
    g_tIPCHandle.pGprs1ReStart  = &g_AppStar[1];
    g_tIPCHandle.pEth0ReStart   = &g_AppStar[2];
    g_tIPCHandle.pEth1ReStart   = &g_AppStar[3];
    g_tIPCHandle.pBlueReStart   = &g_AppStar[4];
    g_tIPCHandle.pEthListenReStart = &g_AppStar[5];
    g_tIPCHandle.pProxyReStart = &g_AppStar[6]; 
    g_tIPCHandle.p485ReStart   = &g_AppStar[7];
    g_tIPCHandle.pSMS0ReStart   = &g_AppStar[8];
    g_tIPCHandle.pSMS1ReStart   = &g_AppStar[9];
    g_tIPCHandle.p232ReStart    = &g_AppStar[10];
    g_tIPCHandle.pIRReStart     = &g_AppStar[11];
    g_tIPCHandle.pRadioReStart   = &g_AppStar[12];

    /* 短信初始化 */
    comm_sms_init(&g_tSMS0Handle, &g_tSMS1Handle);

    comm_channel_event_add(CHL_GPRS_1, comm_ppp_guishow, &g_tPPP0Handle, &g_tIPCHandle);
    //comm_channel_event_add(CHL_GPRS_2, comm_ppp_guishow, &g_tPPP1Handle, &g_tIPCHandle);
    comm_channel_event_add(CHL_ETH_1, comm_eth_guishow, &g_tETH0Handle, &g_tIPCHandle);
    //comm_channel_event_add(CHL_ETH_2, comm_eth_guishow, &g_tETH1Handle, &g_tIPCHandle);

    /* 配置查询检测初始化 */
    memset(&g_tCfgMsg, 0, sizeof(COMM_CFGMSG_HANDLE_T));
    g_tCfgMsg.pRunFlag = &g_nCfgMsgRun;
    comm_cfgmsg_add(&g_tCfgMsg, comm_getcheck_eth0cfg, &g_tETH0Handle, &g_nETH0Rest);
    //comm_cfgmsg_add(&g_tCfgMsg, comm_getcheck_eth1cfg, &g_tETH1Handle, &g_nETH1Rest);
    comm_cfgmsg_add(&g_tCfgMsg, comm_getcheck_ppp0cfg, &g_tPPP0Handle, &g_nPPP0Rest);
    //comm_cfgmsg_add(&g_tCfgMsg, comm_getcheck_ppp1cfg, &g_tPPP1Handle, &g_nPPP1Rest);
    comm_cfgmsg_add(&g_tCfgMsg, comm_getcheck_bluetooth, &g_tBlueHandle, &g_nBlueRest);
    comm_cfgmsg_add(&g_tCfgMsg, comm_getcheck_radio, &g_tRadioHandle, &g_nRadioRest);
    comm_cfgmsg_add(&g_tCfgMsg, comm_getcheck_smscfg,  &g_tSMS0Handle, NULL);
    //comm_cfgmsg_add(&g_tCfgMsg, comm_getcheck_smscfg,  &g_tSMS1Handle, NULL);

    /* 对外通信线程初始化 */
    memset(&g_tCommOutMsg, 0, sizeof(COMM_OUTMSG_HANDLE_T));
    g_tCommOutMsg.pRunFlag   = &g_nOutMsgRun;
    /* 对外通信线程定时器注册 */
    comm_outtimer_add(&g_tCommOutMsg, 30, comm_timer_getDCM, &g_tPPP0Handle);
    //comm_outtimer_add(&g_tCommOutMsg, 30, comm_timer_getDCM, &g_tPPP1Handle);
    comm_outtimer_add(&g_tCommOutMsg, 75, comm_timer_getMode, &g_tPPP0Handle);
    //comm_outtimer_add(&g_tCommOutMsg, 75, comm_timer_getMode, &g_tPPP1Handle);
    comm_outtimer_add(&g_tCommOutMsg, 2,  comm_timer_nextapn,&g_tPPP0Handle);
    //comm_outtimer_add(&g_tCommOutMsg, 2,  comm_timer_nextapn,&g_tPPP1Handle);
    comm_outtimer_add(&g_tCommOutMsg, 2,  comm_timer_getETH, &g_tETH0Handle);
    //comm_outtimer_add(&g_tCommOutMsg, 2,  comm_timer_getETH, &g_tETH1Handle);
    comm_outtimer_add(&g_tCommOutMsg, 20, comm_timer_mastergui, &g_tETH0Handle);
    comm_outtimer_add(&g_tCommOutMsg, 20, comm_timer_pppmastergui, &g_tPPP0Handle);
    //comm_outtimer_add(&g_tCommOutMsg, 20, comm_timer_pppmastergui, &g_tPPP1Handle);
    comm_outtimer_add(&g_tCommOutMsg, 10, comm_timer_485_status, &g_t485Handle);
    comm_outtimer_add(&g_tCommOutMsg, 2,  comm_timer_bt_status, &g_tBlueHandle);
    comm_outtimer_add(&g_tCommOutMsg, 2,  comm_timer_radio_status, &g_tRadioHandle);
    comm_outtimer_add(&g_tCommOutMsg, 1,  comm_timer_rediag, NULL);

    /* 对外通信线程消息处理注册 */
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETDIAL, 1, comm_ppp_dcmmsg, &g_tPPP0Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETDIAL, 2, comm_ppp_dcmmsg, &g_tPPP1Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETETH, 1, comm_eth_statusmsg, &g_tETH0Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETETH, 2, comm_eth_statusmsg, &g_tETH1Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_ETHEVENT, 0xffff, comm_eth_eventmsg, NULL);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_DIALEVENT, 0xffff, comm_ppp_eventmsg, NULL);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_DIALEVENT, 0xffff, comm_sms_eventmsg, NULL);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_PROXYCONNECT, 0xffff, comm_proxy_connect, &g_tPPP0Handle);    
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_PROXYSEND, 0xffff, comm_proxy_data, &g_tPPP0Handle);    
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_SETAPN, 1, comm_ppp_apnmsg, &g_tPPP0Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_SETAPN, 2, comm_ppp_apnmsg, &g_tPPP1Handle); 
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GET_MODULEINFO, 1, comm_ppp_modemsg, &g_tPPP0Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GET_MODULEINFO, 2, comm_ppp_modemsg, &g_tPPP1Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_ETHCHANGEIP, 0xffff, comm_eth_ipchangemsg, &g_tETH0Handle);

    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETUART, 0xffff, comm_485_statusmsg, &g_t485Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_SETUART, 0xffff, comm_485_setmsg, &g_t485Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_REGAPP, 0xffff, comm_485_regappmsg, &g_t485Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_UARTEVENT, 0xffff, comm_485_eventmsg, &g_t485Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_RECVUART, 0xffff, comm_485_recvmsg, &g_t485Handle);

    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_SMSNO, 1, comm_sms_smsnomsg, &g_tSMS0Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_SMSNO, 2, comm_sms_smsnomsg, &g_tSMS1Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETDIAL, 1, comm_sms_dcmmsg, &g_tSMS0Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETDIAL, 2, comm_sms_dcmmsg, &g_tSMS1Handle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETBTCFG, 0xffff, comm_bt_getcfgmsg, &g_tBlueHandle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_SETBTCFG, 0xffff, comm_bt_setcfgmsg, &g_tBlueHandle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETBTPORT, 0xffff, comm_bt_getportmsg, &g_tBlueHandle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_SETBTPORT, 0xffff, comm_bt_setportmsg, &g_tBlueHandle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_BTRECVMSG, 0xffff, comm_bt_recvmsg, &g_tBlueHandle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_BTPORTEVENT, 0xffff, comm_bt_portevent, &g_tBlueHandle);
    
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_GETRADIOCFG, 0xffff, comm_radio_getcfgmsg, &g_tRadioHandle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_SETRADIOCFG, 0xffff, comm_radio_setcfgmsg, &g_tRadioHandle);
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_RADIORECVMSG, 0xffff, comm_radio_recvmsg, &g_tRadioHandle);
    
    comm_outmsg_add(&g_tCommOutMsg, MSG_COMM_APP_TRANS, 0xffff, comm_app_trans_msg, &g_tAppTransHandle);

    return 0;
}

/**********************************************************************
* @name      : comm_task_entry
* @brief     ：通信模块线程入口
* @param[in] ：uint32 dbip
               uint16 dbport
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-23
* @Update    :
**********************************************************************/
int comm_task_entry(uint32 dbip, uint16 dbport)
{
    uint8 i;
    COMM_TRHEAD_T *pCommThread = NULL;
    uint8          closeFlag = 0;
    int            errCode = 0;

    /*1. 数据初始化 */
    comm_data_initproc();

    /* 2. 线程启动与监控 */
    while(1)
    {
        /* 进程状态检测 */
        for(i = 0; i < g_CommThreadNum; i++)
        {
            pCommThread = &g_tCommThread[i];

            //线程是否被stop
            if (NULL != pCommThread->pStartup)
            {
                if (FALSE == pCommThread->pStartup)
                {
                    continue;
                }
            }
            
            closeFlag = 0;
            /* 先判断线程是否需要停止 */
            if(pCommThread->pStartFlag)
            {
                closeFlag = *pCommThread->pStartFlag;
            }
            if(0 == closeFlag)
            {
                (*pCommThread->pRunFlag)++;
                closeFlag = (*pCommThread->pRunFlag >= COMM_TRHEAD_FLAG_RESTART) ? 1 : 0;
            }
            if(closeFlag && 0 != pCommThread->nThreadId)
            {
                COMM_FMT_TRACE("comm cancel thread %d!\n", i);
                pthread_cancel(pCommThread->nThreadId);
                errCode = pthread_join(pCommThread->nThreadId, NULL);
                if(errCode != 0)
                {
                    COMM_FMT_TRACE("comm cancel thread error %d!\n", i);
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

            COMM_FMT_TRACE("comm start thread %d!\n", i);
            errCode = pthread_create(&pCommThread->nThreadId, NULL, pCommThread->pEntryFunc, pCommThread->pArg);
            if (errCode != 0)
            {
                COMM_FMT_TRACE("comm Thread %d Create error! err:%d\n", i, errCode);
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

        /* 线程延时 */
        sleep(1);
    }
}

