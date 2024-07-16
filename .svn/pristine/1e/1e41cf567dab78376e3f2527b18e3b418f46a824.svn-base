/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块无线通信
* @date：    2019-10-31
* @history： 
*********************************************************************
*/

#include "baseCommon.h"
#include <net/if.h>
#include <error.h>
#include "msgUdp.h"
#include "storage.h"
#include <linux/tcp.h>
#include "task_msg.h"
#include "db_api.h"
#include "msg_id.h"
#include "task_id.h"

#include "comm.h" 
#include "proxy_comm.h"
#include "ppp_comm.h"

extern BOOLEAN app_comm_proxy_isuse();

BOOLEAN g_proxyOK = FALSE;
extern COMM_QUEUE_LIST_T g_tProxyRecv;

/**********************************************************************
* @name      : comm_proxy_start
* @brief     ：检查是否需要启动通信代理线程
* @param[in] ：
* @param[out]：
* @return    ：TRUE-启动/FALSE-不启动
* @Create    : 贺宁
* @Date      ：2020-04-15
* @Update    :
**********************************************************************/
BOOLEAN comm_proxy_start(void *pArg)
{
    COMM_PROXY_HANDLE_T *ptHandle = (COMM_PROXY_HANDLE_T *)pArg;

    if(NULL == pArg || 0 == *ptHandle->pIPCok)
    {
        return FALSE;
    }

    return app_comm_proxy_isuse();
}

/**********************************************************************
* @name      : comm_proxy_send
* @brief     ：发送协议数据给通信代理
* @param[in] ：
* @param[out]：
* @return    ：TRUE-启动/FALSE-不启动
* @Create    : 贺宁
* @Date      ：2020-04-15
* @Update    :
**********************************************************************/
ssize_t comm_proxy_send(int sockfd, const void *buf, size_t len, int flags)
{
    int   ret = ERR_OK;
    uint8 dataBuf[BUF_LENGTH] = {0};   /* 缓冲区 */
    uint8 lenBuf[3] = {0};
    uint8 bytes = 0;

    //4字节的内部协议不计算在报文长度之内
    bytes = appmsg_set_len_offset(len, lenBuf);
    if(0 == bytes)
    {
        COMM_FMT_DEBUG("comm_ppp_send msg is zero.\n");
        return -1;
    }
    
    //构造消息payload
    memcpy(dataBuf, lenBuf, bytes);
    memcpy(&dataBuf[bytes], buf, len);
    
    //将报文发给通信代理
    ret = comm_send_msg(MSG_COMM_PROXYSEND, dataBuf, bytes+len);
    if(ret != ERR_OK)
    {
        COMM_FMT_DEBUG("comm_proxy_send failed. ret(%d)\n", ret);
        return -1;
    }

    return len;
}

/**********************************************************************
* @name      : comm_proxy_recv
* @brief     ：从代理队列读取数据
* @param[in] ：
* @param[out]：
* @return    ：TRUE-启动/FALSE-不启动
* @Create    : 贺宁
* @Date      ：2020-04-15
* @Update    :
**********************************************************************/
ssize_t comm_proxy_recv(int sockfd, void *buf, size_t len, int flags)
{
    return comm_read_queue(&g_tProxyRecv, buf);
}

/**********************************************************************
* @name      : comm_proxy_connect
* @brief     ：通信代理模块的主站连接通知解析
* @param[in] ：void *playload    消息内容
               void *pArg        私有数据
               void *pQueueList  发送消息列表
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-04-15
* @Update    : 
**********************************************************************/
int comm_proxy_connect(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    uint8 status;
    uint8 ack = TRUE;
    
    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_proxy_connect input null\n");
        return -1;
    }

    status = playload[0];

    if(1 == status)
    {
        g_proxyOK = TRUE;
    }
    else
    {
        g_proxyOK = FALSE;
    }

    COMM_FMT_DEBUG("comm proxy connect: %d.\n", g_proxyOK);
    
    //回复确认消息
    return comm_send_respmsg(MSG_COMM_PROXYCONNECT, label, &ack, 1);
}

/**********************************************************************
* @name      : comm_proxy_prot_recv
* @brief     ：收到通信代理APP的协议数据
* @param[in] ：void *playload    消息内容
               void *pArg        私有数据
               void *pQueueList  发送消息列表
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-04-15
* @Update    : 
**********************************************************************/
int comm_proxy_data(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    uint8 bytes = 0;
    uint8 ack = TRUE;
    int   recvLen = 0;
    uint8 dataBuf[BUF_LENGTH] = {0};   /* 缓冲区 */
    
    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_proxy_recv input null\n");
        return -1;
    }

    //计算octet-string的长度
    recvLen = appmsg_get_len_offset(playload, &bytes); 
    if(recvLen == 0)
    {
        COMM_FMT_DEBUG("proxy recv zero len.\n");
        return -1;
    }

    //接收到确认消息，不需要再回复
    if((recvLen == 1) && (playload[0] == 1))
    {
        return 0;
    }

    memcpy(dataBuf, &playload[bytes], recvLen);

    //从消息中解析出报文，写入代理队列
    if(0 != comm_write_queue(&g_tProxyRecv, dataBuf, recvLen))
    {
        COMM_FMT_DEBUG("Msg Queue is full\n");
    }

    //回复确认消息
    return comm_send_respmsg(MSG_COMM_PROXYSEND, label, &ack, 1);
}

/**********************************************************************
* @name      : comm_proxy_entry
* @brief     ：通信代理消息收发线程
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-04-15
* @Update    :
**********************************************************************/
void *comm_proxy_entry(void *pArg)
{
    COMM_PROXY_HANDLE_T *ptHandle = (COMM_PROXY_HANDLE_T *)pArg;

    //物理连接OK，通知规约解析通道已打开
    if(ptHandle->bProxyLink == TRUE)
    {
        ;
    }
    
    return NULL;
}

