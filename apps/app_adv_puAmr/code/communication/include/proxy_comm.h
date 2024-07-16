/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 通信代理头文件
* @date：    2019-10-31
* @history： 
*********************************************************************
*/
#ifndef	__PROXY_COMM_H
#define __PROXY_COMM_H

#ifdef __cplusplus
        extern "C" {
#endif

#define ETH_CONNECT_MASTER     0       /* 连接主站 */
#define ETH_SEND_DATA          1       /* 数据发送 */
#define ETH_RECV_DATA          2       /* 数据接收 */
#define ETH_CLOSE_DEVICE       3       /* 关闭 */

/* OAD数据 */
typedef struct tagCommProxy
{
    OOP_ETHCONFIG_T    tEthCfg;
    OOP_MASTERPARAMS_T tMaster;
    OOP_ETHIP_T        tEthIP;
    OOP_ETHMAC_T       tEthMac;
}COMM_PROXY_T;

/* 通信代理线程句柄 */
typedef struct tagCommProxyHandle
{
    uint8              bGetCfg;    /* 是否获取到配置 */
    uint8              bProxyLink; /* 代理是否连接主站 */    
    uint8              res[2];
    COMM_QUEUE_LIST_T *ptRecvList;
    COMM_QUEUE_LIST_T *ptSendList;

    int                fdListen;
    int                fdClient;
    
    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
}COMM_PROXY_HANDLE_T;

BOOLEAN comm_proxy_start(void *pArg);
int comm_proxy_connect(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_proxy_data(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
ssize_t comm_proxy_send(int sockfd, const void *buf, size_t len, int flags);
ssize_t comm_proxy_recv(int sockfd, void *buf, size_t len, int flags);
void *comm_proxy_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif
