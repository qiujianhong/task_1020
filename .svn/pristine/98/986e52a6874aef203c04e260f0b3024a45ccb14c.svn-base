/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 以太网通信头文件
* @date：    2019-10-31
* @history： 
*********************************************************************
*/
#ifndef	__ETH_COMM_H
#define __ETH_COMM_H

#ifdef __cplusplus
        extern "C" {
#endif

#define ETH_CONNECT_NUM        5      /* 端口监听,支持最大连接数量 */

#define ETH_CONNECT_MASTER     0       /* 连接主站 */
#define ETH_SEND_DATA          1       /* 数据发送 */
#define ETH_RECV_DATA          2       /* 数据接收 */
#define ETH_CLOSE_DEVICE       3       /* 关闭 */


/* OAD数据 */
typedef struct tagCommEth
{
    OOP_ETHCONFIG_T    tEthCfg;
    OOP_MASTERPARAMS_T tMaster;
    OOP_ETHIP_T        tEthIP;
    OOP_ETHMAC_T       tEthMac;
}COMM_ETH_T;

/* 本地以太网监听线程句柄 */
typedef struct tagCommEthHandle
{
    uint8              no;
    uint8              bGetCfg;      /* 是否获取到配置 */
    uint8              bEthLink;     /* 以太网口是否连接 */
    uint8              bChangeParam; /* 是否发生不影响连接的参数变化，需要通知规约修改参数 */  

    COMM_ETH_T         tEth;
    
    COMM_QUEUE_LIST_T *ptRecvList;
    COMM_QUEUE_LIST_T *ptRecvListBak;
    COMM_QUEUE_LIST_T *ptSendList;

    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
    uint16            *MixHeart;

    uint8              bMasterGui;  /* 工具栏GUI刷新状态 0-不需要刷新 1-链接 2-断开 */
    uint8              masterStart;  //客户端、混合模式 主备模式 使用
    uint8              masterNUM;   /* 连接主站的数量 */
    uint8              listenNum;   /* 侦听端口数量 */

    uint32             proxyip;
    uint16             proxyport;
    uint8              bProxySSAL;
    uint8              res[0];

    uint32             masterIP[OOP_MAX_HOST];    /* 选择的主站IP */
    uint16             masterPort[OOP_MAX_HOST];  /* 选择的主站PORT */
    int                temp_fd[OOP_MAX_HOST];     /* 临时句柄, 用于中间状态缓存 */
    uint32             temp_time[OOP_MAX_HOST];   /* 临时时间连接的时间 */
    int                client_fd[OOP_MAX_HOST];   /* 客户端混合模式下句柄 */
    uint32             connet_time[OOP_MAX_HOST]; /* 混合模式下连接的时间 */

    int                listen_fd[OOP_MAX_LISTENS]; /* 侦听句柄 */
    int                accept_fd[OOP_MAX_LISTENS][ETH_CONNECT_NUM]; /* 连接句柄 */
    uint32             accept_ip[OOP_MAX_LISTENS][ETH_CONNECT_NUM];
    uint32             accept_port[OOP_MAX_LISTENS][ETH_CONNECT_NUM];
    uint8              bSStatus[OOP_MAX_LISTENS][ETH_CONNECT_NUM]; /* 服务器模式主站连接状态 */
}COMM_ETH_HANDLE_T;

void comm_set_ethname(uint8 ifno, char *ifname);
void comm_timer_getETH(int socketfd, void *pArg);
void comm_timer_mastergui(int socketfd, void *pArg);
int comm_eth_statusmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_eth_eventmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_eth_ipchangemsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_eth_guishow(uint8 id, uint8 protocol, void *pArg);
int comm_getcheck_eth0cfg(int sockdbfd, void *pCfg, uint8 *pRestart);
int comm_getcheck_eth1cfg(int sockdbfd, void *pCfg, uint8 *pRestart);
BOOLEAN comm_eth_start(void *pArg);
void *comm_eth0_entry(void *pArg);
void *comm_eth1_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif
