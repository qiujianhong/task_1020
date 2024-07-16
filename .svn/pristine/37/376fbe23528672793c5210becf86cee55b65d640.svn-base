/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 校表通道
* @date： 2019-10-25
*********************************************************************
*/
#include "message.h"
#include "errCode.h"
#include "ac_mqtt.h"
#include "ac_lib.h"
#include "ac_main.h"
#include "mqtt_client.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"
#include "db_api.h"
#include "ac_485comm.h"
#include <sys/un.h>
#include "uart_api.h"

pthread_mutex_t  g_UartAPPSyncLock;
int              g_UartAPPSyncFd = 0;
COMM_485_INFO_T  g_Comm485Info = {0};

/**********************************************************************
* @name：      get_head_from_buf
* @brief：     找合法的报文68开头，主要指回码报文
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint8 *get_07ptr_from_buf_n(uint8* buf, uint32 maxLen, uint32* offset, uint32* outLen)
{
    int i;
    uint8* head;
    uint8* pBuf = NULL;
    uint32 off = 0;
    uint8  frameCs;
    uint16 frameLen;
    uint16 DataLen = 0;

    if ((buf == NULL) || (maxLen <= 9))
        return NULL;

    head = buf;
    for (i = 0; i < maxLen - 9; i++)
    {
        if ((*head == 0x68) && (*(head + 7) == 0x68))
        {
            pBuf = head;

            //先用2个字节长度校验
            DataLen = pBuf[9] + (pBuf[10] << 8);
            frameLen = 0x0d + DataLen;
            if (frameLen > maxLen)
            {
                //用一个字节长度校验
                DataLen = pBuf[9];
                frameLen = 0x0c + DataLen;
                if (frameLen > maxLen)
                {
                    AC485COM_FMT_DEBUG(PORT_485, "frameLen=%d, bufLen=%d \n", frameLen, maxLen);
                    continue;
                }

                frameCs = CalCs(pBuf, frameLen - 2);

                if ((pBuf[0] == 0x68)
                    && (pBuf[7] == pBuf[0]) /*-北京系统电表地址补0或AA可变-*/
                    && (pBuf[frameLen - 2] == frameCs)
                    && (pBuf[frameLen - 1] == 0x16))
                {
                    *offset = off;
                    *outLen = frameLen;
                    ACSAMP_FMT_TRACE("get_07ptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                    return head;
                }
                else
                {
                    continue;
                }
            }

            frameCs = CalCs(pBuf, frameLen - 2);

            if ((pBuf[0] == 0x68)
                && (pBuf[7] == pBuf[0]) /*-北京系统电表地址补0或AA可变-*/
                && (pBuf[frameLen - 2] == frameCs)
                && (pBuf[frameLen - 1] == 0x16))
            {
                *offset = off;
                *outLen = frameLen;
                ACSAMP_FMT_TRACE("get_07ptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                return head;
            }
            else
            {
                //用一个字节长度校验
                DataLen = pBuf[9];
                frameLen = 0x0c + DataLen;
                if (frameLen > maxLen)
                {
                    AC485COM_FMT_DEBUG(PORT_485, "frameLen=%d, bufLen=%d \n", frameLen, maxLen);
                    continue;
                }

                frameCs = CalCs(pBuf, frameLen - 2);

                if ((pBuf[0] == 0x68)
                    && (pBuf[7] == pBuf[0]) /*-北京系统电表地址补0或AA可变-*/
                    && (pBuf[frameLen - 2] == frameCs)
                    && (pBuf[frameLen - 1] == 0x16))
                {
                    *offset = off;
                    *outLen = frameLen;
                    ACSAMP_FMT_TRACE("get_07ptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                    return head;
                }
                else
                {
                    continue;
                }
            }
        }
        off++;
        head++;
    }
    return NULL;
}

/**********************************************************************
* @name      : ac_485comm_msg_init
* @brief     ：初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/
int ac_485comm_msg_init(void)
{
    if(g_UartAPPSyncFd <= 0)
    {
        g_UartAPPSyncFd = task_msg_init(UART_SYNCMSG);
        if(g_UartAPPSyncFd <= 0)
        {
            INIT_FMT_DEBUG("Appmain task msg init failed!\n");
            return ERR_NORMAL;
        }
        pthread_mutex_init(&g_UartAPPSyncLock, NULL);
    }

    return ERR_OK;
}

/**********************************************************************
* @name      : ac_485comm_mode_proc
* @brief     ：模式变更通知
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/
int ac_485comm_mode_proc(MESSAGE_INFO_T* pMsg)
{
    int    ret = ERR_NORMAL;
    uint8* pTemp = NULL;
    uint32 port = 0;
    char* modestr[] = { "only", "master", "slave" };

    if (NULL == pMsg)
    {
        return ERR_NORMAL;
    }

    //先判断消息是否本函数处理
    if (MESSAGE_IID_UART != pMsg->IID || UART_IOP_NOTIFY_MODE != pMsg->IOP)
    {
        return DB_AGENT_UNKONW_MSG;
    }

    pTemp = pMsg->playload;

    memcpy_r(&port, pTemp, 4);
    pTemp += 4;

    if ((port & 0x000f) != PORT_485)
    {
        AC485COM_FMT_DEBUG(PORT_485, "收到485模式上送 端口错误 len[%d] port[%d]\n", pMsg->msglen, port & 0x000f);
        return ERR_OPTION;
    }

    ret = uart_axdr_notify_mode_split(pMsg, &g_Comm485Info.tUartMode);
    if (0 != ret)
    {
        AC485COM_FMT_DEBUG(PORT_485, "uart_axdr_notify_mode_split failed %d\n", ret);
        return ret;
    }

    AC485COM_FMT_DEBUG(PORT_485, "Uart port %s-%d, mode %s, olnyApp %s\n",
        UART_TYPE_485 == g_Comm485Info.tUartMode.port.portType ? "RS485" : "RS232", g_Comm485Info.tUartMode.port.portNo,
        g_Comm485Info.tUartMode.mode <= UART_MODE_SLAVE ? modestr[g_Comm485Info.tUartMode.mode] : "unknow", g_Comm485Info.tUartMode.curApp);

    //重新注册串口服务
    g_Comm485Info.isGetInfo = 0;
    g_Comm485Info.isSubcribe = 0;

    return ERR_OK;
}

/**********************************************************************
* @name      : ac_485comm_recv_proc
* @brief     ：485数据通知
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/
int ac_485comm_recv_proc(MESSAGE_INFO_T *pMsg)
{
    uint8  *pTemp = NULL;
    uint32 port = 0; 
    uint8  prm = 0;
    uint8  state = 0;
    uint16 recvLen = 0;  
    uint16 offset = 0;   
    int32  ret = 0;  
    uint8* ptr = NULL;
    uint32 outLen = 0;
    uint32 off = 0;

    if (NULL == pMsg)
    {
        return ERR_NORMAL;
    }

    //先判断消息是否本函数处理
    if (MESSAGE_IID_UART != pMsg->IID || UART_IOP_NOTIFY_RECV != pMsg->IOP)
    {
        return DB_AGENT_UNKONW_MSG;
    }

    pTemp = pMsg->playload;

    memcpy_r(&port, pTemp, 4);
    pTemp += 4;

    if ((port & 0x000f) != PORT_485)
    {
        AC485COM_FMT_DEBUG(PORT_485, "485口收到报文 端口错误port[%d]. len[%d]\n", port & 0x000f, pMsg->msglen);
        return ERR_OPTION;
    }

    prm = *(uint8 *)pTemp;
    pTemp += 1;
    ACMQTT_FMT_TRACE("485口收到报文. 启动标志prm[%d]\n", prm);

    state = *(uint8 *)pTemp;
    pTemp += 1;
    ACMQTT_FMT_TRACE("485口收到报文. 状态state[%d]\n", state);

    BuffToChangeLen(pTemp, &offset, &recvLen);
    pTemp += offset;

    if (pMsg->msglen < 4 + offset + recvLen)
    {
        AC485COM_FMT_DEBUG(PORT_485, "485口收到报文 长度解析错误. len[%d]\n", pMsg->msglen);
        return ERR_OVERFLOW;
    }

    ptr = get_07ptr_from_buf_n(pTemp, recvLen, &off, &outLen);
    if (ptr != NULL)
    {
        ret = PlcWriteQueue(pDealRecvDown[0], pTemp, recvLen);
        if (ret == 0)
        {
            AC485COM_BUF_DEBUG(PORT_485, pTemp, recvLen, "485口收到报文. 队列满\n");
            return ERR_OVERFLOW;
        }
        else
        {
            AC485COM_BUF_DEBUG(PORT_485, pTemp, recvLen, "串口[%d]接收报文", port & 0x000f);
        }
    }
    else
    {
        AC485COM_BUF_DEBUG(PORT_485, pTemp, recvLen, "串口[%d]接收非07报文", port & 0x000f);
        return ERR_FORMAT;
    }


    return ERR_OK;
}

/**********************************************************************
* @name      : ac_485comm_send_proc
* @brief     ：485数据通知
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/
int ac_485comm_send_proc(MESSAGE_INFO_T *pMsgInfo)
{
    uint32 msgid = 0;
    int    ret = 0;
    uint8  label = 0;

    label = (uint8)pMsgInfo->label;
    msgid = 0;
    ret = task_ack_syncmsg(g_UartAPPSyncFd, msgid, pMsgInfo->playload, pMsgInfo->msglen, UART_SYNCMSG, label);
    ACMQTT_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", UART_SYNCMSG, msgid, label, ret);
    return ret;
}

/**********************************************************************
* @name      : ac_addport_proc
* @brief     ：增加端口
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/
// int ac_485comm_add_port(MESSAGE_INFO_T *pMsg)
// {
//     //先判断消息是否本函数处理
//     if (MESSAGE_IID_UART != pMsg->IID || UART_IOP_ADD_PORT != pMsg->IOP)
//     {
//         return DB_AGENT_UNKONW_MSG;
//     }

//     if (pMsg->msglen > 0)
//     {
//         g_MqttAcInfo.datalen = pMsg->msglen;
//         memcpy(g_MqttAcInfo.data, pMsg->playload, pMsg->msglen);
//     }

//     pthread_cond_signal(&(g_MqttAcInfo.notify));

//     return ERR_OK;
// }

/**********************************************************************
* @name      : ac_485comm_msg_proc
* @brief     ：485数据通知
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/
int ac_485comm_msg_proc(MESSAGE_INFO_T *pMsgInfo)
{
    if (NULL == pMsgInfo)
    {
        return ERR_NORMAL;
    }

    //先判断消息是否本函数处理
    if (MESSAGE_IID_UART != pMsgInfo->IID)
    {
        return DB_AGENT_UNKONW_MSG;
    }

    switch (pMsgInfo->IOP)
    {
    //模式变更
    case UART_IOP_NOTIFY_MODE:
        return ac_485comm_mode_proc(pMsgInfo);
        break;

    //数据接收
    case UART_IOP_NOTIFY_RECV:
        return ac_485comm_recv_proc(pMsgInfo);
        break;

    //串口信息查询
    case UART_IOP_GET_INFO:

    //数据订阅
    case UART_IOP_SET_LISTEN:

    //数据发送
    case UART_IOP_SEND_DATA:
        return ac_485comm_send_proc(pMsgInfo);
        break;

    default:
        break;
    }

    return ERR_OK;
}

/**********************************************************************
* @name      : task_syncmsg_safe
* @brief     ：任务发送接收同步消息, 安全接口, 避免和异步接口使用同一个sockfd
               上层调用需要加锁保证收发唯一性
* @param[in] ：int sockfd      socket句柄
               mqttClient      mqtt的句柄
               sendmqtt        发送的消息
               uint16 buflen   返回消息缓存大小
               uint8 outtime   超时时间 单位s
* @param[out]：uint8 *msgbuf   返回消息
* @return    ：0-成功/错误码
* @Date      ：2020-10-28
* @Update    :
**********************************************************************/
int task_syncmqtt_proc(int sockfd, MQTT_CLIENT_T *mqttClient, MESSAGE_MQTT_T *sendmqtt,
                      uint8 *msgbuf, uint16 buflen, uint8 outtime)
{
    int  ret = 0;
    int  errcode = 0;
    int  len = 0;
    struct sockaddr_un addr;
    TASK_MSG_t *pmsg = NULL;
    uint32 sender = 0;

    struct timespec ctime = {0};
    struct timespec stime = {0};
    struct timeval tv = {0, 100000};   /* 一次接收超时时间0.1s */
    
    if(sockfd <= 0)
    {
        return MSG_ERR_INPUT;
    }

    pmsg = (TASK_MSG_t *)malloc(TASK_MSG_MAX_LEN);  //申请最大容量消息
    if(NULL == pmsg)
    {
        return MSG_ERR_MALLOC;
    }

    ret = mqtt_msg_send(mqttClient, sendmqtt, 0);
    if(ret < 0)
    {
        free(pmsg);
        return MSG_ERR_SEND_FAILED;
    }
    
    /* 2. 获取本socket绑定的地址 */
    memset(&addr, 0, sizeof(struct sockaddr_un));
    getsockname(sockfd, (struct sockaddr *)&addr, (socklen_t *)&len);

    /* 3. 设置超时时间 */
    ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
    if(0 != ret)
    {
        errcode = MSG_ERR_SOCKET;
        goto end;
    }

    /* 4. 接收等待 */
    clock_gettime(CLOCK_MONOTONIC, &stime);
    for(;;)
    {
        memset(pmsg, 0, TASK_MSG_MAX_LEN);
        ret = task_recv_msg(sockfd, (uint8 *)pmsg, TASK_MSG_MAX_LEN, &sender);
        if(ret > 0)
        {
            if(TRUE == pmsg->syncflag)
            {
                if(buflen < ret)
                {
                    errcode = MSG_ERR_TOO_LONG;
                }
                else
                {
                    memcpy(msgbuf, pmsg, ret);
                }
                goto end;
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &ctime);
        if(ctime.tv_sec - stime.tv_sec > outtime)
        {
            errcode = MSG_ERR_OUT_TIME;
            goto end;
        }
    }

end:
    free(pmsg);
    return errcode;
}

/**********************************************************************
* @name      : ac_485_send_syncmsg
* @brief     ：发送同步消息消息, 
               目前适配消息 MSG_APPAMIN_RCT MSG_APPAMIN_REBOOT MSG_APPAMIN_SET_IPADDR MSG_APPAMIN_CTS_SEEK_METER
* @param[in] ：uint32 msgid    消息ID
               uint8 *sendbuf  发送内容
               uint16 sendlen  发送长度
               uint16 buflen   接收缓存大小
* @param[out]：uint8 *recvbuf  接收内容
               uint16 *recvLen 接收长度
* @return    ：0-成功/错误码
* @Date      ：2020-3-19
* @Update    :
**********************************************************************/
int ac_485_send_syncmsg(MESSAGE_MQTT_T *msgMqttInfo, uint8 *recvbuf, uint16 buflen, uint16 *recvLen)
{
    int           ret = 0;
    uint8         *pRecvBuf = recvbuf;
    TASK_MSG_t    *pMsg = (TASK_MSG_t *)pRecvBuf;

    ret = task_syncmqtt_proc(g_UartAPPSyncFd, g_MqttClient, msgMqttInfo, recvbuf, buflen, 4);
    if (0 == ret)
    {
        if(buflen < pMsg->msglen)
        {
            return -100;
        }
        memcpy(recvbuf, pMsg->msg, pMsg->msglen);
        *recvLen = pMsg->msglen;
    }

    return ret;
}

/**********************************************************************
* @name      : ac_485_param_set
* @brief     ：485参数设置
* @param[in] ：comdcb   串口参数
               port     端口
* @param[out]：
* @return    ：0-成功/错误码
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/
int ac_485param_set(uint32 port, OOP_COMDCB_T comdcb)
{
    MESSAGE_MQTT_T msgMqttInfo = {0};
    uint8          recvbuf[64] = {0};
    uint16         recvLen = 0;
    int            ret = 0;
    int            nRet = 0;

    //组织消息内容
    
    ret = ac_485_send_syncmsg(&msgMqttInfo, recvbuf, 64, &recvLen);
    if ((ret != ERR_OK) || (recvLen != 1) || (recvbuf[0] != 1))
    {
        AC485COM_FMT_DEBUG(PORT_485, "ac_485_param_set err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}

/**********************************************************************
* @name      : ac_send_uart_syncmsg
* @brief     ：向串口服务发同步消息
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-24
* @Update    :
**********************************************************************/
int ac_send_uart_syncmsg(uint16 iop, uint8* playload, uint16 length, uint8 *pRecvbuf, uint16 buflen, uint8 outtime, uint16 *recvLen)
{
    int ret = 0;
    MESSAGE_MQTT_T *ptMqttMsg = NULL;
    MESSAGE_INFO_T *ptAXDR = NULL;
    char           *dstApp = "M-uartManager";
    uint16 msglen = sizeof(MESSAGE_MQTT_T) + length;
    uint8  recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;

    ptMqttMsg = (MESSAGE_MQTT_T *)malloc(msglen);
    if (NULL == ptMqttMsg)
    {
        AC485COM_FMT_DEBUG(PORT_485, "Send mqtt msg malloc %d failed\n", msglen);
        return -1;
    }
    
    memset(ptMqttMsg, 0, msglen);

    ptMqttMsg->msgtype = MESSAGE_TYPE_A_XDR;
    
    ptAXDR = &(ptMqttMsg->tAXDR);
    ptAXDR->rpm = 1;
    ptAXDR->priority = 2;
    ptAXDR->index = GetInfoIndex();
    ptAXDR->label = 0;
    
    ptAXDR->IID = MESSAGE_IID_UART;
    ptAXDR->IOP = iop;
    
    ptAXDR->sourlen = strlen(appName);
    memcpy(ptAXDR->souraddr, appName, ptAXDR->sourlen);
    ptAXDR->destlen = strlen(dstApp);
    memcpy(ptAXDR->destaddr, dstApp, ptAXDR->destlen);

    ptAXDR->msglen = length;
    memcpy(ptAXDR->playload, playload, length);

    PTHREAD_LOCK_SAFE(&g_UartAPPSyncLock);
    ret = task_syncmqtt_proc(g_UartAPPSyncFd, g_MqttClient, ptMqttMsg, recvBuf, TASK_MSG_MAX_LEN, outtime);
    PTHREAD_UNLOCK_SAFE(&g_UartAPPSyncLock);
    if(0 == ret)
    {
        memcpy(pRecvbuf, pMsg->msg, pMsg->msglen);
        *recvLen = pMsg->msglen;
    }

    free(ptMqttMsg);

    return ret;
}


/**********************************************************************
* @name      : ac_485comm_send
* @brief     ：串口数据发送接口
* @param[in] ：void *pDev                   串口句柄
               int nBufLen                  最大接收长度
               int rsptime                  数据返回时间(20ms)
* @param[out]：
* @return    ：接收到数据的长度
* @Date      ：2020-3-13
* @Update    :
**********************************************************************/
int ac_485comm_send(uint32 port, uint8* pSend, int sendLen)
{
    uint8  msgdata[2048] = {0};
    uint8  msgRecv[2048] = {0};
    uint16 recvLen = 0;
    uint16 msglen = 0;
    uint16 byteNum = 0;
    uint16 packDelayms = 1000;
    uint16 byteDelayms = 120;
    uint16 maxDelayms = 2048;
    uint16 maxRecvBytes = 2048;
    uint32 portOAD = 0xf2010200 + port;
    
    //端口号
    memcpy_r(&msgdata[msglen], &portOAD, sizeof(uint32));
    msglen += sizeof(uint32);

    //启动标识
    //memcpy_r(&msgdata[msglen], &port, sizeof(uint32));
    //msglen += sizeof(uint32);
    msgdata[msglen++] = 0;

    //优先级
    msgdata[msglen++] = 0;

    //帧超时时间
    memcpy_r(&msgdata[msglen], &packDelayms, sizeof(uint16));
    msglen += sizeof(uint16);
    
    //字符超时时间
    memcpy_r(&msgdata[msglen], &byteDelayms, sizeof(uint16));
    msglen += sizeof(uint16);

    //最大超时时间
    memcpy_r(&msgdata[msglen], &maxRecvBytes, sizeof(uint16));
    msglen += sizeof(uint16);

    //通信参数
    msgdata[msglen++] = 0; //OPTIONAL=0

    // //波特率
    // msgdata[msglen++] = 6; //9600

    // //控制位
    // msgdata[msglen++] = UART_CTRLBITS_SET(8, 0, 1, 0);

    //预计最大接收字节数
    memcpy_r(&msgdata[msglen], &maxDelayms, sizeof(uint16));
    msglen += sizeof(uint16);

    //发送帧
    SET_LEN_OFFSET(sendLen, &msgdata[msglen], byteNum);
    msglen += byteNum;

    memcpy(&msgdata[msglen], pSend, sendLen);
    msglen +=  sendLen;

    return ac_send_uart_syncmsg(UART_IOP_SEND_DATA, msgdata, msglen, msgRecv, 2048, 4, &recvLen);
}


/**********************************************************************
* @name      : ac_485comm_subscribe
* @brief     ：数据监听订阅
* @param[in] ：port 端口
* @param[out]：
* @return    ：错误码
* @Date      ：2020-3-13
* @Update    :
**********************************************************************/
int ac_485comm_subscribe(uint32 port)
{
    uint8  msgdata[1024] = {0};
    uint8  msgRecv[1024] = {0};
    uint16 recvLen = 0;
    uint16 msglen = 0;
    uint32 portOAD = 0xf2010200 + port;
    
    //端口号
    memcpy_r(&msgdata[msglen], &portOAD, sizeof(uint32));
    msglen += sizeof(uint32);

    return ac_send_uart_syncmsg(UART_IOP_SET_LISTEN, msgdata, msglen, msgRecv, 1024, 4, &recvLen);
}

/**********************************************************************
* @name      : ac_485comm_getinfo
* @brief     ：串口信息查询
* @param[in] ：port 端口
* @param[out]：
* @return    ：错误码
* @Date      ：2020-3-13
* @Update    :
**********************************************************************/
int ac_485comm_getinfo(uint32 port)
{
    uint8  msgdata[1024] = {0};
    uint8  msgRecv[1024] = {0};
    uint16 recvLen = 0;
    uint16 msglen = 0;
    int    ret = ERR_NORMAL;
    uint32 portOAD = 0xf2010200 + port;

    MESSAGE_INFO_T     *pMsgInfo = NULL;
    char* modestr[] = { "only", "master", "slave" };

    //端口号
    memcpy_r(&msgdata[msglen], &portOAD, sizeof(uint32));
    msglen += sizeof(uint32);

    ret = ac_send_uart_syncmsg(UART_IOP_GET_INFO, msgdata, msglen, msgRecv, 1024, 4, &recvLen);
    if ((ret == ERR_OK) && (recvLen > 0))
    {
        pMsgInfo = (MESSAGE_INFO_T *)malloc(sizeof(MESSAGE_INFO_T) + recvLen);
        if (pMsgInfo == NULL)
        {
            AC485COM_BUF_DEBUG(port, msgRecv, recvLen, "comm_485_statusmsg split malloc err");
            return ERR_NORESOURCE;
        }

        pMsgInfo->IID = MESSAGE_IID_UART;
        pMsgInfo->IOP = UART_IOP_GET_INFO;
        pMsgInfo->msglen = recvLen;
        memcpy(pMsgInfo->playload, msgRecv, recvLen);

        ret = uart_axdr_get_info_res_split(pMsgInfo, &g_Comm485Info.tModeInfo);
        if (ERR_OK != ret)
        {
            AC485COM_BUF_DEBUG(port, msgRecv, recvLen, "comm_485_statusmsg split error");
            free(pMsgInfo);
            pMsgInfo = NULL;
            return ERR_INVALID;
        }

        AC485COM_FMT_DEBUG(port, "Uart port %s-%d, mode %s, olnyApp %s\n",
            UART_TYPE_485 == g_Comm485Info.tModeInfo.port.portType ? "RS485" : "RS232", g_Comm485Info.tModeInfo.port.portNo,
            g_Comm485Info.tModeInfo.mode <= UART_MODE_SLAVE ? modestr[g_Comm485Info.tModeInfo.mode] : "unknow", g_Comm485Info.tModeInfo.onlyApp);
    }

    free(pMsgInfo);
    pMsgInfo = NULL;

    return ret;
}

/**********************************************************************
* @name      : ac_485comm_data_convert
* @brief     ：串口服务代码从SCU移植过，存在入参不一致的情况，需要转换下接口
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：
**********************************************************************/
void ac_485comm_data_convert(MSG_INFO_T *ptMsgInfo  ,MESSAGE_INFO_T *ptMessageInfo)
{
    if ((ptMsgInfo == NULL) && (ptMessageInfo == NULL))
    {
        return;
    }
    
    ptMessageInfo->rpm = ptMsgInfo->MsgRPM;
    ptMessageInfo->priority = ptMsgInfo->MsgPRIORITY;
    ptMessageInfo->sourlen = ptMsgInfo->SourLen;
    memcpy(ptMessageInfo->souraddr, ptMsgInfo->SourAddr, ptMsgInfo->SourLen);
    ptMessageInfo->destlen = ptMsgInfo->DestLen;
    memcpy(ptMessageInfo->souraddr, ptMsgInfo->DestAddr, ptMsgInfo->DestLen);
    ptMessageInfo->index = ptMsgInfo->MsgIndex;
    ptMessageInfo->label = ptMsgInfo->MsgLabel;
    ptMessageInfo->IID = ptMsgInfo->IID;
    ptMessageInfo->IOP = ptMsgInfo->IOP;
    ptMessageInfo->msglen = ptMsgInfo->MsgLen;
    memcpy(ptMessageInfo->playload, ptMsgInfo->MsgData, ptMsgInfo->MsgLen);

    return;
}

