/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 以太网通信头文件
* @date：    2020-3-7
* @history： 
*********************************************************************
*/
#ifndef	__COMM_ETH_H
#define __COMM_ETH_H

#ifdef __cplusplus
        extern "C" {
#endif

#define ETH_CONNECT_MASTER     0       /* 连接主站 */
#define ETH_SEND_DATA          1       /* 数据发送 */
#define ETH_RECV_DATA          2       /* 数据接收 */
#define ETH_CLOSE_DEVICE       3       /* 关闭 */

/* OAD数据 */
typedef struct tagCommEth
{
    OOP_ETHCONFIG_T    tEthCfg;
    OOP_MASTERPARAMS_T tMaster;
}COMM_ETH_T;

/* 本地以太网监听线程句柄 */
typedef struct tagCommEthHandle
{
    uint8              no;
    uint8              bGetCfg;      /* 是否获取到配置 */
    uint8              bEthLink;     /* 以太网口是否连接 */
    uint8              bChangeParam; /* 是否发生不影响连接的参数变化，需要通知规约修改参数 */

    COMM_ETH_T         tEthCfg;      //配置
    
    COMM_QUEUE_LIST_T *ptRecvList;
    COMM_QUEUE_LIST_T *ptSendList;

    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
    
    uint8              bClose;     //是否关闭功能
    uint8              masterNUM; //连接主站的数量
    uint8              masterStart;  //客户端 主备模式 使用
    uint8              res[1];

    uint32             ip[OOP_MAX_HOST];
    uint16             port[OOP_MAX_HOST];
    int                connect_fd[OOP_MAX_HOST];
    uint32             connet_time[OOP_MAX_HOST]; /* 客户端连接的时间 */
}COMM_ETH_HANDLE_T;

void eth_set_name(uint8 ifno, char *ifname);
void comm_timer_getETH(int socketfd, void *pArg);
int comm_eth_statusmsg(uint8 *playload, uint16 msglen, void *pArg);
int comm_eth_eventmsg(uint8 *playload, uint16 msglen, void *pArg);
int comm_getcheck_ethcfg(int sockdbfd, void *pCfg, uint8 *pRestart);
BOOLEAN comm_eth_start(void *pArg);
void *comm_eth_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif

