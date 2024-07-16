/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 无线通信头文件
* @date：    2019-10-31
* @history： 
*********************************************************************
*/
#ifndef	__PPP_COMM_H
#define __PPP_COMM_H

#include "comm.h"

#ifdef __cplusplus
        extern "C" {
#endif

#define PPP_CONNECT_MASTER     0       /* 连接主站 */
#define PPP_SEND_DATA          1       /* 数据发送 */
#define PPP_RECV_DATA          2       /* 数据接收 */
#define PPP_CLOSE_DEVICE       3       /* 关闭 */

#define PPP_CONNECT_NUM        5      /* 端口监听,支持最大连接数量 */

typedef struct tagCommPortFlow
{ 
    uint32 applyRxFlow;         /* 应用层日流量 */
    uint32 applyTxFlow;         /* 应用层日流量 */

    uint64 ifnetRxFlow;         /* 网口日流量 */
    uint64 ifnetTxFlow;         /* 网口日流量 */
}COMM_PORTFLOW_T;

/* OAD数据 和 无线拨号查询的数据 */
typedef struct tagCommGprs
{
    OOP_GPRSCONFIG_T   tOopGprs;  /* 通信配置 */
    OOP_MASTERPARAMS_T tOopMaster;/* 主站通信参数 */
    OOP_APNGROUP_T     tAPN;      /* 无线公网备用通道 */

    /* 江苏配置 */
    uint8              chooseType; /* 公专切换模式 */
    OOP_PRIVATE_NET_T  tPrivate;   /* 专网软号 */
}COMM_GPRS_T;

/* PPP线程句柄 */
typedef struct tagCommPPPHandle
{
    uint8              no;          /* 编号 */
    uint8              bGetCfg;     /* 获取配置成功 */
    uint8              bGetDCM;     /* 获取无线模块成功 */
    uint8              bDiagOK;     /* 拨号成功 */

    char               curApn[64];  /* 当前APN */
    char               targetApn[64];/* 目标APN */

    uint8              operat;      /* 运营商 */
    uint8              nettype;     /* 网络类型 */
    uint8              apnIndex;    /* apn序号, 0-一般通信配置中, 其他-备用通道 */
    uint8              bChangeApn;  /* 是否在切换APN过程中 */

    uint8              bFirstRun;   //首次运行
    uint8              bGetApn;     //获取APN
    uint8              bSingleCfg;  //单网配置
    uint8              bMoreCfg;    //多网配置

    uint8              stack;       /* 协议栈 */
    uint8              bMasterGui;  /* 工具栏GUI刷新状态 0-不需要刷新 1-链接 2-断开 */
    uint8              masterStart; //客户端、混合模式使用, 主备模式连接起始序号
    uint8              res[1];
    uint16             proxyport;
    uint32             proxyip;
    time_t             getApnTime;  //获取APN的时间
    time_t             getDiagTime; //获取APN的时间

    COMM_GPRS_T        tGprsCfg;

    OOP_VERSION_T      version;     //无线模块版本号
    OOP_VISIBLEVAR_T   DevIMEI;     //无线模块IMEI
    OOP_VISIBLEVAR_T   OldDescript; //旧无线模块描述，应用于生成通信模块变更事件
    COMM_PORTFLOW_T    tFlow;       //流量统计
    
    uint8             *pETHStatus;  /* 以太网连接状态 */
    OOP_MASTERPARAMS_T *pETHMaster; /* 以太网状态 */

    COMM_QUEUE_LIST_T *ptRecvList;  /* 接收队列 */
    COMM_QUEUE_LIST_T *ptRecvListBak; /* 接收后备队列 */
    COMM_QUEUE_LIST_T *ptSendList;  /* 发送队列 */

    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
    uint16            *MixHeart;    //混合模式心跳周期

    uint8              res1[1];
    uint8              bProxySSAL;
    uint8              bChangeParam;/* 是否发生不影响连接的参数变化，需要通知规约修改参数 */ 
    uint8              masterNUM;   /* 连接主站的数量 */
    uint8              listenNum;   /* 侦听端口数量 */

    uint32             masterIP[OOP_MAX_HOST];    /* 客户机、混合模式,UDP选择的主站IP */
    uint16             masterPort[OOP_MAX_HOST];  /* 客户机、混合模式,UDP选择的主站PORT */
    int                temp_fd[OOP_MAX_HOST];     /* 临时句柄, 用于中间状态缓存 */
    uint32             temp_time[OOP_MAX_HOST];   /* 临时时间连接的时间 */
    int                client_fd[OOP_MAX_HOST];   /* 客户机、混合模式,UDP下句柄 */
    uint32             connet_time[OOP_MAX_HOST]; /* 客户机、混合模式连接的时间 */

    int                listen_fd[OOP_MAX_LISTENS]; /* 侦听句柄 */
    int                accept_fd[OOP_MAX_LISTENS][PPP_CONNECT_NUM]; /* 连接句柄 */
    uint32             accept_ip[OOP_MAX_LISTENS][PPP_CONNECT_NUM];
    uint32             accept_port[OOP_MAX_LISTENS][PPP_CONNECT_NUM];
}COMM_PPP_HANDLE_T;


int comm_getcheck_ppp0cfg(int sockdbfd, void *pCfg, uint8 *pRestart);
int comm_getcheck_ppp1cfg(int sockdbfd, void *pCfg, uint8 *pRestart);
void comm_timer_rediag(int socketfd, void *pArg);
void comm_timer_getDCM(int socketfd, void *pArg);
void comm_timer_getMode(int socketfd, void *pArg);
void comm_timer_pppmastergui(int socketfd, void *pArg);
int comm_ppp_dcmmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_ppp_modemsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_ppp_guishow(uint8 id, uint8 protocol, void *pArg);
int comm_ppp_eventmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
void comm_timer_nextapn(int socketfd, void *pArg);
int comm_ppp_apnmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_ppp_gui_toolmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
BOOLEAN comm_ppp_start(void *pArg);
void *comm_ppp0_entry(void *pArg);
void *comm_ppp1_entry(void *pArg);



#ifdef __cplusplus
}
#endif

#endif
