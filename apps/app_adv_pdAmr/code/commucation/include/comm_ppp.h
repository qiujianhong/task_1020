/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 无线通信头文件
* @date：    2020-3-7
* @history： 
*********************************************************************
*/
#ifndef	__COMM_PPP_H
#define __COMM_PPP_H

#ifdef __cplusplus
        extern "C" {
#endif

#define PPP_CONNECT_MASTER     0       /* 连接主站 */
#define PPP_SEND_DATA          1       /* 数据发送 */
#define PPP_RECV_DATA          2       /* 数据接收 */
#define PPP_CLOSE_DEVICE       3       /* 关闭 */

/* OAD数据 和 无线拨号查询的数据 */
typedef struct tagCommPPPCfg
{
    OOP_GPRSCONFIG_T   tOopGprs;  /* 通信配置 */
    OOP_MASTERPARAMS_T tOopMaster;/* 主站通信参数 */
}COMM_PPPCFG_T;

/* PPP线程句柄 */
typedef struct tagCommPPPHandle
{
    uint8              no;          /* 编号 */
    uint8              bGetCfg;     /* 获取配置成功 */
    uint8              bGetDCM;     /* 获取无线模块成功 */
    uint8              bDiagOK;     /* 拨号成功 */

    COMM_PPPCFG_T      tGprsCfg;    //配置
    
    COMM_QUEUE_LIST_T *ptRecvList;  /* 接收队列 */
    COMM_QUEUE_LIST_T *ptSendList;  /* 发送队列 */

    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;

    time_t             getApnTime;  //获取APN的时间

    uint8              bChangeParam;/* 是否发生不影响连接的参数变化，需要通知规约修改参数 */ 
    uint8              bClose;       //是否关闭功能
    uint8              masterNUM;    //连接主站的数量
    uint8              masterStart;  //客户端 主备模式 使用

    uint32             ip[OOP_MAX_HOST];
    uint16             port[OOP_MAX_HOST];
    int                connect_fd[OOP_MAX_HOST];
    uint32             connet_time[OOP_MAX_HOST]; /* 客户端连接的时间 */
}COMM_PPP_HANDLE_T;

void comm_timer_getDCM(int socketfd, void *pArg);
int comm_ppp_dcmmsg(uint8 *playload, uint16 msglen, void *pArg);
int comm_ppp_eventmsg(uint8 *playload, uint16 msglen, void *pArg);
int comm_getcheck_pppcfg(int sockdbfd, void *pCfg, uint8 *pRestart);
BOOLEAN comm_ppp_start(void *pArg);
void *comm_ppp_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif

