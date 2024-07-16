/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块串口(蓝牙)通信
* @date：    2019-10-31
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "hal.h"
#include "uart.h"
#include "ble.h"
#include "comm.h"
#include "uart_api.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "appmsg_split.h"
#include "sys_cfg.h"
#include "appcomm.h"
#include "oopVarUpdate.h"
#include "rs232_comm.h"
#include "statlib.h"


#define COMM_485_NAME           "GBR"       //RS485通信模块名称检索
extern STA_PORT_AUTHORIZE_INFO g_portRS485;
extern int g_msgMonitor;

#if AREA_SHANDONG
#define RS485_SEND_MAX    2048
#else
#define RS485_SEND_MAX    512
#endif
extern uint8 gdelay;

/**********************************************************************
* @name      : check_07_frame
* @brief     ：07规约检车
* @param[in] ：uint8 *pMsg       消息
               uint16 msglen     消息长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-4-26
* @Update    :
**********************************************************************/
uint8 check_07_frame(uint8 *pMsg, uint16 msglen)
{
    uint16 len = 0;
    uint8 *temp = pMsg;
    uint8  CS = 0;
    uint16 i = 0;
    uint16 leftlen = msglen;
    
    if(NULL == pMsg || msglen < 12)
    {
        return FALSE;
    }

    while(0x68 != *temp && 0 != leftlen)
    {
        temp++;
        leftlen--;
    }
    if(leftlen < 12)
    {
        return FALSE;
    }
    
    if(0x68 == temp[0] && 0x68 == temp[7])
    {
        len = temp[9];
        if(len + 12 <= leftlen && 0x16 == temp[len + 9 + 2])
        {
            /* 校验判断 */
            for(i = 0; i < len + 10; i++)
            {
                CS += temp[i];
            }
            if(temp[len + 9 + 1] == CS)
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/**********************************************************************
* @name      : local_RS232_recv
* @brief     ：串口数据接收接口
* @param[in] ：UART_DEVICE_T* dev   串口句柄
               int nBufLen                  最大接收长度
* @param[out]：uint8* pBuffer               接收数据缓冲区
* @return    ：接收到数据的长度
* @Create    : 王津剑
* @Date      ：2020-3-18
* @Update    :
**********************************************************************/
int local_RS232_recv(UART_DEVICE_T* dev, uint8* pBuffer, int nBufLen)
{
    int recv_len = 0;        /*接收到数据长度*/
    fd_set set;
    struct timeval timeout;
    int n = 0;
    int fd = 0;

    fd = dev->uart_get_handle(dev);
    if(fd <= 0)
    {
        return 0;
    }

    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    n = select(fd + 1, &set, NULL, NULL, &timeout);
    if(n <= 0)
    {
        return 0;
    }

    if (n > 0 && FD_ISSET(fd, &set))
    {  
        recv_len = dev->uart_data_recv(dev, pBuffer, nBufLen);
    }
    return recv_len;
}

/**********************************************************************
* @name      : local_read_serial
* @brief     ：读串口数据
* @param[in] ：void *pDev       串口句柄
               uint32 baudRate  串口波特率
               uint32 bufLen    缓冲区大小
* @param[out]：uint8 *pBuf      接收数据缓冲区
* @return    ：接收数据长度
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
uint32 local_read_serial(void *pDev, uint32 baudRate, uint8 *pBuf, uint32 bufLen)
{
    int delay;
    int recv_len;
    uint32 data_len;

    /*串口接收数据超时时间按20个字节间隔时间来计算，300bps 发送一个字节需要36667us
        一个字节包括起始位、停止位、校验位、8位数据 1000/(300/11) = 36.666ms*/
    if ((baudRate%300 != 0 )&&(baudRate <= 0))	/*若波特率不对，默认9600*/
    {
        delay = 20*36667/(9600/300);
    }
    else
    {
        delay = 20*36667/(baudRate/300);
    }
	
    data_len = 0;
    recv_len = 0;
    recv_len = local_RS232_recv(pDev, pBuf, bufLen);	/*调用read()，进行串口数据接收*/
    while (recv_len > 0)		/*read()在没有数据时，返回-1*/
    {
        usleep(delay);
        data_len += recv_len;
        recv_len = local_RS232_recv(pDev, pBuf+data_len, bufLen-data_len);
        if (recv_len == bufLen - data_len)
        {
            data_len = bufLen;
            break;
        }
    }
    return data_len;	
}

/**********************************************************************
* @name      : local_send_serial
* @brief     ：串口数据发送
* @param[in] ：void *pDev       串口句柄
               uint8* pBuffer   发送数据缓冲区
               int nBufLen      发送长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-10
* @Update    :
**********************************************************************/
int local_send_serial(void *pDev, uint8* pBuffer, int nBufLen)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T*)pDev;
    if(0 == dev->uart_data_send(dev, pBuffer, nBufLen))
    {
        return 0;
    }
    return -1;
}

/**********************************************************************
* @name      : comm_getcheck_bluetooth
* @brief     ：获取并检查蓝牙配置是否发生变化
* @param[in] ：int sockdbfd      通信句柄
               void *pCfg        配置存储地址
* @param[out]：void *pCfg        配置存储地址
               uint8 *pRestart   线程重启标记
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-2
* @Update    :
**********************************************************************/
int comm_getcheck_bluetooth(int sockdbfd, void *pCfg, uint8 *pRestart)
{
    /* 目前不需要参数获取 */
    COMM_BLUE_HANDLE_T *ptHandle = (COMM_BLUE_HANDLE_T *)pCfg;
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    OOP_OCTETVAR16_T tLocalAddr;
    static uint8     s_bluepint[1] = {0};
    
    ptHandle->port1 = 0;
    ptHandle->port2 = 1;
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    /* 获取终端地址 */
    tOAD.oad.value = 0x40010200;
    memset(&tLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
    ret = db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&tLocalAddr, &readlen);
    if(0 == ret && readlen == sizeof(OOP_OCTETVAR16_T) && tLocalAddr.nNum > 0)
    {
        ptHandle->bGetCfg = TRUE;
        ble_addr_to_mac(tLocalAddr.value, tLocalAddr.nNum, BLE_MAC_GATHER, ptHandle->YCMAC);
        if(tLocalAddr.nNum > 2)
        {
            ptHandle->name[2] = (tLocalAddr.value[tLocalAddr.nNum - 1] & 0x0F) + '0';
            ptHandle->name[1] = ((tLocalAddr.value[tLocalAddr.nNum - 1] >> 4) & 0x0F) + '0';
            ptHandle->name[0] = (tLocalAddr.value[tLocalAddr.nNum - 2] & 0x0F) + '0';
        }
        else
        {
            ptHandle->name[0] = ptHandle->name[1] = ptHandle->name[2] = '0';
        }
        ptHandle->name[3] = 0;
        if(0 != memcmp(ptHandle->YCMAC, ptHandle->btCfg.MAC, 6) && COMM_BT_STATUS_WORK == ptHandle->btStatus)
        {
            ptHandle->btStatus = COMM_BT_STATUS_GETAPP;
            COMM_FMT_DEBUG("MAC变化，重置蓝牙\n");
        }
    }
    else
    {
        ptHandle->bGetCfg = FALSE;
    }

    tOAD.oad.value = 0xF20B0600;
    memset(&ptHandle->tBlueConf, 0, sizeof(OOP_BTWORKPARAMS_T));
    ret = db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_BTWORKPARAMS_T), (uint8 *)&ptHandle->tBlueConf, &readlen);
    if(0 != ret || readlen != sizeof(OOP_BTWORKPARAMS_T))
    {
        if(FALSE == s_bluepint[0])
        {
            COMM_FMT_DEBUG("没有蓝牙配置参数\n");
            s_bluepint[0] = TRUE;
        }
        memset(&ptHandle->tBlueConf, 0, sizeof(OOP_BTWORKPARAMS_T));
    }
    
    return 0;
}

/**********************************************************************
* @name      : comm_timer_bt_status
* @brief     ：蓝牙状态机切换
* @param[in] ：int socketfd    task消息句柄
               void *pArg      入参
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2021-8-2
* @Update    :
**********************************************************************/
void comm_timer_bt_status(int socketfd, void *pArg)
{
    int                 ret = 0;
    COMM_BLUE_HANDLE_T *ptHandle = (COMM_BLUE_HANDLE_T *)pArg;

    if(socketfd <= 0 || NULL == pArg || FALSE == ptHandle->bGetCfg || 0 == *ptHandle->pIPCok)
    {
        return;
    }
    
    /* 串口相关操作 */
    switch(ptHandle->btStatus)
    {
        case COMM_BT_STATUS_GETAPP:
        {
            /* 查询APP */
            ret = comm_send_label_msg(MSG_COMM_GETBTCFG, 1, NULL, 0);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("Comm get bt app send failed %d\n", ret);
            }
            /* 清空队列 */
            comm_clean_queue(ptHandle->ptRecvList);
            comm_clean_queue(ptHandle->ptSendList);
            break;
        }
        case COMM_BT_STATUS_SETCFG:
        {
            uint8  msgbuf[128] = {0};
            uint16 msglen = 0;
            
            memcpy(ptHandle->btCfg.MAC, ptHandle->YCMAC, 6);
            memcpy(ptHandle->btCfg.name, ptHandle->name, strlen(ptHandle->name));
            ptHandle->btCfg.len = strlen(ptHandle->name);
            /* 设置新参数 */
            if(ptHandle->tBlueConf.nNum > 0)
            {
                ptHandle->btCfg.broadcast = ptHandle->tBlueConf.param[0].nBroadCastInterval;
                ptHandle->btCfg.scan = ptHandle->tBlueConf.param[0].nScanPeriod;
                ptHandle->btCfg.power = ptHandle->tBlueConf.param[0].nPowerLevel;
            }
            else
            {
                ptHandle->btCfg.broadcast = 40;
                ptHandle->btCfg.scan = 100;
                ptHandle->btCfg.power = 0;
            }
            
            COMM_FMT_DEBUG("设置蓝牙名称%s, MAC %02X:%02X:%02X:%02X:%02X:%02X, 功率%d, 广播时间%dms,"
                           " 扫描时间%dms\n", ptHandle->btCfg.name, ptHandle->btCfg.MAC[0], ptHandle->btCfg.MAC[1], 
                           ptHandle->btCfg.MAC[2], ptHandle->btCfg.MAC[3], ptHandle->btCfg.MAC[4],
                           ptHandle->btCfg.MAC[5], ptHandle->btCfg.power, ptHandle->btCfg.broadcast, 
                           ptHandle->btCfg.scan);
            msglen = ble_config_package(&(ptHandle->btCfg), msgbuf, 128);
            if(0 == msglen)
            {
                COMM_FMT_DEBUG("Comm package bt cfg msg failed\n");
                break;
            }
            /* 设置新参数 */
            ret = comm_send_label_msg(MSG_COMM_SETBTCFG, 1, msgbuf, msglen);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("Comm set bt cfg send failed %d\n", ret);
            }
            break;
        }
        case COMM_BT_STATUS_GETPORT:
        {
            ret = comm_send_label_msg(MSG_COMM_GETBTPORT, 1, NULL, 0);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("Comm get uart port send failed %d\n", ret);
            }
            break;
        }
        case COMM_BT_STATUS_SETPORT:
        {
            uint8  msgbuf[128] = {0};
            uint16 msglen = 0;
            BLE_PORTS_CONF_T tPortConf;
            memset(&tPortConf, 0, sizeof(BLE_PORTS_CONF_T));
            tPortConf.num = 1;
            tPortConf.tConf[0].port = ptHandle->port1;
            tPortConf.tConf[0].mode = BLE_MODE_MASTER;
            memset(tPortConf.tConf[0].MAC, 0xFF, 6);
            tPortConf.tConf[0].match.mode = BLE_MATCH_JW;/* 不需要密码 */

            //PIN码 需要密码
            if (get_pin_flag_ble())
            {
                tPortConf.tConf[0].match.mode = BLE_MATCH_PE;/* 需要密码 */
                tPortConf.tConf[0].match.len = 6;
                strcpy(&tPortConf.tConf[0].match.pwd[0], "123456");
            }

            /* 设置端口配置 */
            msglen = ble_port_config_package(&tPortConf, msgbuf, 128);
            if(0 == msglen)
            {
                COMM_FMT_DEBUG("Comm package bt port msg failed\n");
                break;
            }
            
            ret = comm_send_label_msg(MSG_COMM_SETBTPORT, 1, msgbuf, msglen);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("Comm set uart port send failed %d\n", ret);
            }
            break;
        }
        case COMM_BT_STATUS_PREPARE:
        {
            /* 通知端口打开 */
            uint8 msgBuf[512] = {0};
            uint16 msgLen = 0;
            COMM_IPCMSG_T *ptIPCMsg = NULL;
            CHANNEL_INFO_T *ptChanInfo = NULL;
            
            msgLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
            ptIPCMsg = (COMM_IPCMSG_T *)msgBuf;
            ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
            ptChanInfo->workmode = 2;
            ptChanInfo->medium = COMM_MEDIA_BLUETOOTH;
            ptChanInfo->connType = 0;
            ptChanInfo->bFirst = TRUE;
            ptChanInfo->heart = 0;
            ptIPCMsg->id = LOCATE_BLUE;
            ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
            ptChanInfo->dailcnt = 3;
            ptChanInfo->dailtime = 60;
            ptChanInfo->ip = 0;
            ptChanInfo->port = 0;
            if(TRUE == ptHandle->bMac1Con)
            {
                ptIPCMsg->id = LOCATE_BLUE;
                comm_write_queue(ptHandle->ptRecvList, msgBuf, msgLen);
                COMM_FMT_DEBUG("蓝牙通道%d打开\n", ptIPCMsg->id);
            }
            if(TRUE == ptHandle->bMac2Con)
            {
                ptIPCMsg->id = LOCATE_BLUE + 1;
                comm_write_queue(ptHandle->ptRecvList, msgBuf, msgLen);
                COMM_FMT_DEBUG("蓝牙通道%d打开\n", ptIPCMsg->id);
            }
                
            ptHandle->btStatus = COMM_BT_STATUS_WORK;
            break;
        }
        case COMM_BT_STATUS_WORK:
        {
            break;
        }
        default:
        {
            ptHandle->btStatus = COMM_BT_STATUS_GETAPP;
            break;
        }
    }
}

/**********************************************************************
* @name      : comm_bt_getcfgmsg
* @brief     ：蓝牙参数获取
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-8-2
* @Update    : 
**********************************************************************/
int comm_bt_getcfgmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_BLUE_HANDLE_T *ptHandle = (COMM_BLUE_HANDLE_T *)pArg;
    BLE_CONFIG_T tConfig;
    int ret = 0;

    if(NULL == playload || 0 == msglen || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_bt_getcfgmsg err input\n");
        return -1;
    }
    
    memset(&tConfig, 0, sizeof(BLE_CONFIG_T));
    ret = ble_config_split(playload, msglen, &tConfig);
    if(0 != ret)
    {
        COMM_BUF_DEBUG(playload, msglen, "Comm get bt cfg error(%d)\n", ret);
        return ret;
    }
    memcpy(&(ptHandle->btCfg), &tConfig, sizeof(BLE_CONFIG_T));
    
    if(COMM_BT_STATUS_GETAPP != ptHandle->btStatus)
    {
        return 0;
    }

    COMM_FMT_DEBUG("获取蓝牙名称%s, MAC %02X:%02X:%02X:%02X:%02X:%02X, 功率%d, 广播时间%dms, 扫描时间%dms\n", 
                   tConfig.name, tConfig.MAC[0], tConfig.MAC[1], tConfig.MAC[2], tConfig.MAC[3], tConfig.MAC[4],
                   tConfig.MAC[5], tConfig.power, tConfig.broadcast, tConfig.scan);
    
    if(0 == memcmp(ptHandle->YCMAC, ptHandle->btCfg.MAC, 6) && 0 == tConfig.power)
    {
        /* 蓝牙地址同终端, 直接进入端口查询 */
        ptHandle->btStatus = COMM_BT_STATUS_GETPORT;
    }
    else
    {
        ptHandle->btStatus = COMM_BT_STATUS_SETCFG;
    }
    return 0;    
}

/**********************************************************************
* @name      : comm_bt_setcfgmsg
* @brief     ：蓝牙参数设置
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-8-2
* @Update    : 
**********************************************************************/
int comm_bt_setcfgmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_BLUE_HANDLE_T *ptHandle = (COMM_BLUE_HANDLE_T *)pArg;
    BLE_CONFIG_T *ptConfig = &(ptHandle->btCfg);

    if(NULL == playload || 0 == msglen || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_bt_setcfgmsg err input\n");
        return -1;
    }

    COMM_FMT_DEBUG("设置蓝牙参数%s, 名称%s, MAC %02X:%02X:%02X:%02X:%02X:%02X, 功率%d, 广播时间%dms, 扫描时间%dms\n", 
                   0 == *playload ? " failed" : " success", ptConfig->name, ptConfig->MAC[0], ptConfig->MAC[1], 
                   ptConfig->MAC[2], ptConfig->MAC[3], ptConfig->MAC[4],ptConfig->MAC[5], 
                   ptConfig->power, ptConfig->broadcast, ptConfig->scan);

    if(COMM_BT_STATUS_SETCFG == ptHandle->btStatus)
    {
        ptHandle->btStatus = COMM_BT_STATUS_GETPORT;
    }
    return 0;  
}

/**********************************************************************
* @name      : comm_bt_getportmsg
* @brief     ：蓝牙连接获取
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-8-2
* @Update    : 
**********************************************************************/
int comm_bt_getportmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_BLUE_HANDLE_T *ptHandle = (COMM_BLUE_HANDLE_T *)pArg;
    BLE_CONNECT_INFO_T tConnect;
    uint32 i = 0;
    int ret = 0;
    uint8 bExist = FALSE;
    
    if(NULL == playload || 0 == msglen || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_bt_getportmsg err input\n");
        return -1;
    }
    
    ret = ble_connect_split(playload, msglen, &tConnect);
    if(0 != ret)
    {
        COMM_BUF_DEBUG(playload, msglen, "Comm get bt link error(%d)\n", ret);
        return ret;
    }

    for(i = 0; i < tConnect.num; i++)
    {
        if(BLE_MODE_MASTER == tConnect.list[i].mode)
        {
            if(ptHandle->port1 == tConnect.list[i].port)
            {
                bExist = TRUE;
                if(FALSE != tConnect.list[i].bConnect)
                {
                    memcpy(ptHandle->MasterMAC1, tConnect.list[i].MAC, 6);
                    ptHandle->bMac1Con = TRUE;
                }
            }
            if(ptHandle->port2 == tConnect.list[i].port)
            {
                if(FALSE != tConnect.list[i].bConnect)
                {
                    memcpy(ptHandle->MasterMAC2, tConnect.list[i].MAC, 6);
                    ptHandle->bMac2Con = TRUE;
                }
            }
        }
        
    }

    //强制设置端口参数
    bExist = FALSE;
    
    if(COMM_BT_STATUS_GETPORT == ptHandle->btStatus)
    {
        COMM_FMT_DEBUG("%s获取蓝牙端口%d\n", FALSE == bExist ? "未" : "", ptHandle->port1);
        ptHandle->btStatus = FALSE == bExist ? COMM_BT_STATUS_SETPORT : COMM_BT_STATUS_PREPARE;
    }
    
    return 0;    
}

/**********************************************************************
* @name      : comm_bt_setportmsg
* @brief     ：蓝牙端口配置
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-8-2
* @Update    : 
**********************************************************************/
int comm_bt_setportmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_BLUE_HANDLE_T *ptHandle = (COMM_BLUE_HANDLE_T *)pArg;
    
    if(NULL == playload || 0 == msglen || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_bt_setportmsg err input\n");
        return -1;
    }
    
    COMM_FMT_DEBUG("设置蓝牙端口%s\n", 0 == *playload ? "失败" : "成功");
    if(COMM_BT_STATUS_SETPORT == ptHandle->btStatus && 0 != *playload)
    {
        ptHandle->btStatus = COMM_BT_STATUS_PREPARE;
    }
    return 0; 
}

/**********************************************************************
* @name      : comm_bt_recvmsg
* @brief     ：蓝牙接收消息
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-8-2
* @Update    : 
**********************************************************************/
int comm_bt_recvmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_BLUE_HANDLE_T *ptHandle = (COMM_BLUE_HANDLE_T *)pArg;
    uint8  dataBuf[2048 + 8] = {0};
    uint16 dataLen = 0;
    int    ret = 0;
    BLE_MSG_EVENT_T tMsg;
    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_485_recvmsg input null\n");
        return -1;
    }

    ret = ble_msg_event_split(playload, msglen, &tMsg);
    if(0 != ret)
    {
        COMM_BUF_DEBUG(playload, msglen, "Comm recv bt msg error(%d)\n", ret);
        return ret;
    }
    
    if(COMM_BT_STATUS_WORK != ptHandle->btStatus)
    {
        return 0;
    }
    
    memcpy(dataBuf, tMsg.msg, MIN(tMsg.len, 2048));
    dataLen = MIN(tMsg.len, 2048);

    if(tMsg.port == ptHandle->port1 && TRUE == ptHandle->bMac1Con)
    {
        if(0 == memcmp(tMsg.MAC, ptHandle->MasterMAC1, 6))
        {
            comm_add_channel(LOCATE_BLUE, dataBuf, dataLen);
    
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen + 4))
            {
                COMM_FMT_TRACE("Bluetooth queue is full\n");
            }
            COMM_BUF_DEBUG(dataBuf + 4, dataLen, "蓝牙1(%02X:%02X:%02X:%02X:%02X:%02X)接收", tMsg.MAC[0], 
                                                 tMsg.MAC[1], tMsg.MAC[2], tMsg.MAC[3], tMsg.MAC[4], tMsg.MAC[5]);
        }
        else
        {
            COMM_BUF_DEBUG(dataBuf, dataLen, "蓝牙1(其他MAC%02X:%02X:%02X:%02X:%02X:%02X)接收, 丢弃", tMsg.MAC[0], 
                                                 tMsg.MAC[1], tMsg.MAC[2], tMsg.MAC[3], tMsg.MAC[4], tMsg.MAC[5]);
        }
    }
    if(tMsg.port == ptHandle->port2 && TRUE == ptHandle->bMac2Con)
    {
        if(0 == memcmp(tMsg.MAC, ptHandle->MasterMAC2, 6))
        {
            comm_add_channel(LOCATE_BLUE + 1, dataBuf, dataLen);
        
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen + 4))
            {
                COMM_FMT_TRACE("Bluetooth queue is full\n");
            }
            COMM_BUF_DEBUG(dataBuf + 4, dataLen, "蓝牙2(%02X:%02X:%02X:%02X:%02X:%02X)接收", tMsg.MAC[0], 
                                                 tMsg.MAC[1], tMsg.MAC[2], tMsg.MAC[3], tMsg.MAC[4], tMsg.MAC[5]);
        }
        else
        {
            COMM_BUF_DEBUG(dataBuf, dataLen, "蓝牙2(其他MAC%02X:%02X:%02X:%02X:%02X:%02X)接收, 丢弃", tMsg.MAC[0], 
                                             tMsg.MAC[1], tMsg.MAC[2], tMsg.MAC[3], tMsg.MAC[4], tMsg.MAC[5]);
        }
    }
    return 0;
}

/**********************************************************************
* @name      : comm_bt_portevent
* @brief     ：蓝牙端口事件
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-8-2
* @Update    : 
**********************************************************************/
int comm_bt_portevent(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_BLUE_HANDLE_T *ptHandle = (COMM_BLUE_HANDLE_T *)pArg;
    BLE_PORT_EVENT_T tPort;
    int ret = 0;
    uint8 msgBuf[512] = {0};
    uint16 msgLen = 0;
    COMM_IPCMSG_T *ptIPCMsg = NULL;
    CHANNEL_INFO_T *ptChanInfo = NULL;
    
    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_485_recvmsg input null\n");
        return -1;
    }

    ret = ble_port_event_split(playload, msglen, &tPort);
    if(0 != ret)
    {
        COMM_BUF_DEBUG(playload, msglen, "Comm recv bt port event error(%d)\n", ret);
        return ret;
    }

    if(COMM_BT_STATUS_WORK != ptHandle->btStatus || BLE_MODE_SLAVE == tPort.mode)
    {
        return 0;
    }

    if(tPort.port < 2)
    {
        COMM_FMT_DEBUG("蓝牙端口%X%s, MAC %02X:%02X:%02X:%02X:%02X:%02X, 模式 %d\n", tPort.port, 
                       FALSE == tPort.bConnect ? "断连" : "连接", 
                       tPort.MAC[0], tPort.MAC[1], tPort.MAC[2], tPort.MAC[3], tPort.MAC[4], tPort.MAC[5], tPort.mode);
    }
    /* 通知端口动作 */
    msgLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
    ptIPCMsg = (COMM_IPCMSG_T *)msgBuf;
    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
    ptChanInfo->workmode = 2;
    ptChanInfo->medium = COMM_MEDIA_BLUETOOTH;
    ptChanInfo->connType = 0;
    ptChanInfo->bFirst = TRUE;
    ptChanInfo->heart = 0;
    ptChanInfo->dailcnt = 3;
    ptChanInfo->dailtime = 60;
    ptChanInfo->ip = 0;
    ptChanInfo->port = 0;

    if(ptHandle->port1 == tPort.port)
    {
        ptIPCMsg->id = LOCATE_BLUE;
        if(FALSE == tPort.bConnect)
        {
            memset(ptHandle->MasterMAC1, 0, 6);
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            COMM_FMT_DEBUG("蓝牙通道%d关闭\n", ptIPCMsg->id);
        }
        else
        {
            memcpy(ptHandle->MasterMAC1,tPort.MAC, 6);
            ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
            COMM_FMT_DEBUG("蓝牙通道%d打开\n", ptIPCMsg->id);
        }
        ptHandle->bMac1Con = FALSE == tPort.bConnect ? FALSE : TRUE;
    }
    else if(ptHandle->port2 == tPort.port)
    {
        ptIPCMsg->id = LOCATE_BLUE + 1;
        if(FALSE == tPort.bConnect)
        {
            memset(ptHandle->MasterMAC2, 0, 6);
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            COMM_FMT_DEBUG("蓝牙通道%d关闭\n", ptIPCMsg->id);
        }
        else
        {
            memcpy(ptHandle->MasterMAC2,tPort.MAC, 6);
            ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
            COMM_FMT_DEBUG("蓝牙通道%d打开\n", ptIPCMsg->id);
        }
        ptHandle->bMac2Con = FALSE == tPort.bConnect ? FALSE : TRUE;
    }
    comm_write_queue(ptHandle->ptRecvList, msgBuf, msgLen);
    return 0;    
}


/**********************************************************************
* @name      : local_232_start
* @brief     ：232串口 蓝牙 启动判断
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
BOOLEAN bluetooth_start(void *pArg)
{
    COMM_BLUE_HANDLE_T *ptHandle = (COMM_BLUE_HANDLE_T *)pArg;

    if(0 == *ptHandle->pIPCok || COMM_BT_STATUS_WORK != ptHandle->btStatus)
    {
        return FALSE;
    }
    return TRUE;
}

/**********************************************************************
* @name      : local_232_entry
* @brief     ：232串口 蓝牙 线程入口
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
void *bluetooth_entry(void *pArg)
{
    int        ret = 0;
    uint8      stateLocal = CHANNEL_STATE_INIT;  /* 运行状态 */
    BLE_MSG_T  tMsgOut;
    uint8      msgBuf[SERIAL_BUF_SIZE + 128] = {0}; 
    uint32     msgLen = 0;
    uint8      recv[8] = {0};
    uint16     recvlen = 0;
    COMM_IPCMSG_T   *ptIPCMsg = NULL;
    COMM_BLUE_HANDLE_T *ptHandle = (COMM_BLUE_HANDLE_T *)pArg;
    
    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }

    COMM_FMT_DEBUG("蓝牙通信打开\n");
    tMsgOut.port = ptHandle->port1;

    while(1)
    {
        *ptHandle->pRunFlag = 0;

        /* Appmain重启 */
        if((NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart) || COMM_BT_STATUS_WORK != ptHandle->btStatus)
        {
            /* 通知主应用调度线程通道打开 蓝牙默认开启 */
            msgLen = sizeof(COMM_IPCMSG_T);
            ptIPCMsg = (COMM_IPCMSG_T *)msgBuf;
            ptIPCMsg->id = LOCATE_BLUE;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            comm_write_queue(ptHandle->ptRecvList, msgBuf, msgLen);
            COMM_FMT_DEBUG("蓝牙通道%d关闭\n", ptIPCMsg->id);

            ptIPCMsg->id = LOCATE_BLUE + 1;
            comm_write_queue(ptHandle->ptRecvList, msgBuf, msgLen);
            COMM_FMT_DEBUG("蓝牙通道%d关闭\n", ptIPCMsg->id);

            *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
            return 0;
        }
        
        switch(stateLocal)
        {
            case CHANNEL_STATE_INIT:
            {
                /* 通知主应用调度线程通道打开 蓝牙默认开启 */
                stateLocal = CHANNEL_STATE_SEND;
                break;
            }
            case CHANNEL_STATE_SEND:
            {
                tMsgOut.len = comm_read_queue(ptHandle->ptSendList, tMsgOut.msg);
                if(tMsgOut.len > 4)
                {
                    tMsgOut.port = tMsgOut.msg[0] == LOCATE_BLUE ? ptHandle->port1 : ptHandle->port2;

                    if((tMsgOut.port == ptHandle->port1 && ptHandle->bMac1Con == TRUE) || 
                       (tMsgOut.port == ptHandle->port2 && ptHandle->bMac2Con == TRUE))
                    {
                        /* 前4个字节作为发送标记 */
                        memset(tMsgOut.msg, 0xFE, 4);
                        msgLen = ble_msg_package(&tMsgOut, msgBuf, SERIAL_BUF_SIZE + 128);
                        ret = comm_send_syncmsg(MSG_COMM_BTSENDMSG, msgBuf, msgLen, recv, 8, &recvlen);
                        if(0 != ret || recvlen != 1 || 0 == recv[0])
                        {
                            ret = -1;
                        }
                        COMM_BUF_DEBUG(tMsgOut.msg, tMsgOut.len, "蓝牙%d发送%s", tMsgOut.port + 1, 0 == ret ? "" : "失败");
                    }
                    memset(tMsgOut.msg, 0 , tMsgOut.len + 4);
                    memset(msgBuf, 0 , msgLen);
                }
                stateLocal = CHANNEL_STATE_SEND;
                break;
            }
            case CHANNEL_STATE_CLOSE:
            {
                stateLocal = CHANNEL_STATE_INIT;
                break;
            }
            default:
            {
                stateLocal = CHANNEL_STATE_INIT;
                break;
            }
        }
        usleep(20000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}

//#ifdef PRODUCT_ECU
#if 0
extern int app_get_meter_list(uint32 infoNum, OOP_METER_T *oopMeter);
#define MTR_ID_485			1					//485计量点号起始

/**********************************************************************
* @name      : comm_timer_485_status
* @brief     ：485状态机切换
* @param[in] ：int socketfd    task消息句柄
               void *pArg      入参
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-7-17
* @Update    :
**********************************************************************/
void comm_timer_485_status(int socketfd, void *pArg)
{
    mtrnum_t    mtrId = MTR_ID_485;
    OOP_METER_T oopMeter;
    uint8       bCommucation = TRUE;

    COMM_485_HANDLE_T *ptHandle = (COMM_485_HANDLE_T *)pArg;

    if(socketfd <= 0 || NULL == pArg)
    {
        return;
    }

    /* 判断是否存在档案配置在4路485 后期考虑在界面配置 F201为RS485 */
    while (mtrId <= MTR485_MAX_NUM)
	{
		if (0 == app_get_meter_list(mtrId, &oopMeter))
		{
			if(0xF2010202 == oopMeter.basic.port.value)
            {
                bCommucation = FALSE;
                break;
            }
		}
        mtrId++;
	}
    if(ptHandle->bNoMeter != bCommucation)
    {
        COMM_FMT_DEBUG("Comm set 485 nometer status %d\n", bCommucation);
        ptHandle->bNoMeter = bCommucation;
    }
}

/**********************************************************************
* @name      : comm_rs485_start
* @brief     ：485 启动判断
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-17
* @Update    :
**********************************************************************/
BOOLEAN comm_rs485_start(void *pArg)
{
    COMM_485_HANDLE_T *ptHandle = (COMM_485_HANDLE_T *)pArg;

    if(0 == *ptHandle->pIPCok || FALSE == ptHandle->bNoMeter)
    {
        return FALSE;
    }
    return TRUE;
}

/**********************************************************************
* @name      : comm_rs485_entry
* @brief     ：485 线程入口
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-17
* @Update    :
**********************************************************************/
void *comm_rs485_entry(void *pArg)
{
    UART_DEVICE_T  *dev = NULL;
    COMM_485_HANDLE_T *ptHandle = (COMM_485_HANDLE_T *)pArg;

    uint8   stateLocal = CHANNEL_STATE_INIT;  /* 运行状态 */
    uint8   dataBuf[SERIAL_BUF_SIZE + 8] = {0}; 
    uint32  dataLen = 0;
    CHANNEL_INFO_T *ptChanInfo = NULL;
    COMM_IPCMSG_T   *ptIPCMsg = NULL;
    char    devName[48] = {0};
    int ret = 0;
    uint8 sendmsg[1] = {2}; //发送非法接入消息
    uint16 msglen = 1;
    uint16 templen = 0;
    uint8 *temp = NULL;

    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }

    if(ptHandle->pUart)
    {
        hal_device_release((HW_DEVICE *)ptHandle->pUart);
        ptHandle->pUart = NULL;
    }

    sleep(5);    //等待配置刷新后续操作

    /* 清空队列 */
    comm_clean_queue(ptHandle->ptRecvList);
    comm_clean_queue(ptHandle->ptSendList);

    sprintf(devName, "%s", UART_RS4852_DEVICE_ID_STR);
    while(1)
    {
        *ptHandle->pRunFlag = 0;

        /* Appmain重启 */
        if(1 == *ptHandle->pAppReStart || FALSE == ptHandle->bNoMeter)
        {
            uint8   buf[52] = {0};
            uint16  len = 0;
            uint32  timelong = 60;

            /* 通知主应用调度线程通道打开 蓝牙默认开启 */
            dataLen = sizeof(COMM_IPCMSG_T);
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = JOIN485_1;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
            COMM_FMT_DEBUG("Local 485 channel id %d close\n", ptIPCMsg->id);

            ptIPCMsg->id = CHL_ACMETER;
            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
            COMM_FMT_DEBUG("ACMETER %s id %d close\n", devName, ptIPCMsg->id);

            /* 状态栏 */
            appmsg_memcpy_r(buf, &timelong, sizeof(uint32));
            len = sprintf((char *)buf + 5, "485-%d%s", ptHandle->chanDefend, "端口断开链接");
            buf[4] = len;
            comm_send_msg(MSG_COMM_GUISTATUS, buf, len + 5);

            hal_device_release((HW_DEVICE *)ptHandle->pUart);
            ptHandle->pUart = NULL;
            *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
            return 0;
        }
        
        switch(stateLocal)
        {
            case CHANNEL_STATE_INIT:
            {
                if(NULL != dev)
                {
                    stateLocal = CHANNEL_STATE_RECV;
                    break;
                }
                dev = (UART_DEVICE_T*)hal_device_get(devName);
                if(dev)
                {
                    if(0 != dev->uart_param_set(dev, 9600, 8, 1, PARITY_EVEN))
                    {
                        COMM_FMT_DEBUG("Local 485 %s set failed!\n", devName);
                        hal_device_release((HW_DEVICE *)dev);
                        dev = NULL;
                        sleep(10);
                        break;
                    }
                    uint8   buf[52] = {0};
                    uint16  len = 0;
                    uint32  timelong = 60;

                    /* 通知主应用调度线程通道打开 蓝牙默认开启 */
                    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                    ptChanInfo->workmode = 2;
                    ptChanInfo->medium = COMM_MEDIA_485;
                    ptChanInfo->connType = 0;
                    ptChanInfo->bFirst = TRUE;
                    ptChanInfo->heart = 0;
                    ptChanInfo->dailcnt = 3;
                    ptChanInfo->dailtime = 60;
                    ptIPCMsg->id = JOIN485_1;
                    ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
                    ptChanInfo->ip = 0;
                    ptChanInfo->port = 0;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("Local 485 %s id %d open\n", devName, ptIPCMsg->id);
                    
                    ptIPCMsg->id = CHL_ACMETER;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("ACMETER %s id %d open\n", devName, ptIPCMsg->id);

                    /* 状态栏 */
                    appmsg_memcpy_r(buf, &timelong, sizeof(uint32));
                    len = sprintf((char *)buf + 5, "485-%d%s", 2, "端口链接");
                    buf[4] = len;
                    comm_send_msg(MSG_COMM_GUISTATUS, buf, len + 5);

                    ptHandle->pUart = dev;
                    memset(dataBuf, 0, SERIAL_BUF_SIZE);
                    dataLen = 0;
                    stateLocal = CHANNEL_STATE_RECV;
                }
                else
                {
                    COMM_FMT_DEBUG("Local 485 %s open failed!\n", devName);
                    sleep(10);
                }
                break;
            }
            case CHANNEL_STATE_RECV:
            {
                dataLen = local_read_serial(dev, 9600, dataBuf, SERIAL_BUF_SIZE-4); //预留4个字节作为通道头
                if (dataLen > 0)
                {
                    COMM_BUF_DEBUG(dataBuf + 4, dataLen, "485-%d recv", ptHandle->chanDefend);

                    /* 判断是否645消息, 如果是需要走交采透传通道 */
                    if (TRUE == check_07_frame(dataBuf, dataLen))
                    {
                        comm_add_channel(CHL_ACMETER, dataBuf, dataLen);
                    }
                    else
                    {
                        comm_add_channel(JOIN485_1, dataBuf, dataLen);
                    }
                    
                    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen + 4))
                    {
                        COMM_FMT_TRACE("Local 485 queue is full\n");
                    }
                    
                    memset(dataBuf, 0 , dataLen + 4);
                }
                stateLocal = CHANNEL_STATE_SEND;
                break;
            }
            case CHANNEL_STATE_SEND:
            {
                dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
                if(dataLen > 0)
                {
                    //维护口授权关闭时，只响应与手持终端的安全认证交互命令,
                    if(app_secure_defend_isuse()&&(g_portRS485.status == STA_PORT_AUTH_FORBIDDEN)&&(g_portRS485.isAuthedcmd == FALSE))
                    {
                        ret = task_send_labelmsg(g_msgMonitor, MSG_APPAMIN_PORT485_AUTHORIZE, 0, sendmsg, msglen, APPMAIN_MSG);
                        COMM_FMT_TRACE("portRS485 Send back to 0x%x: msg 0x%x, label %d, ret %d\n", APPMAIN_MSG, MSG_APPAMIN_UDISK_AUTHORIZE, 0, ret); 
                    
                        stateLocal = CHANNEL_STATE_RECV;
                        break;
                    }
                    
                    /* 485最大一次发送512字节 */
                    temp = dataBuf + 4;
                    templen = dataLen - 4;
                    while(templen > 0 && NULL != temp)
                    {
                        if(templen > RS485_SEND_MAX)
                        {
                            local_send_serial(dev, temp, RS485_SEND_MAX);
                            COMM_BUF_DEBUG(temp, RS485_SEND_MAX, "485-%d send", ptHandle->chanDefend);
                            templen -= RS485_SEND_MAX;
                            temp += RS485_SEND_MAX;
                            usleep(800000);
                        }
                        else
                        {
                            local_send_serial(dev, temp, templen);
                            COMM_BUF_DEBUG(temp, templen, "485-%d send", ptHandle->chanDefend);
                            templen = 0;
                            temp = NULL;
                        }
                    }
                    memset(dataBuf, 0 , dataLen);
                }
                stateLocal = CHANNEL_STATE_RECV;
                break;
            }
            default:
            {
                stateLocal = CHANNEL_STATE_INIT;
                break;
            }
        }
        usleep(10000);
    }

    pthread_exit(NULL);
}

#else
extern int app_get_meter_list(uint32 infoNum, OOP_METER_T *oopMeter);
#define MTR_ID_485			1					//485计量点号起始

/**********************************************************************
* @name      : comm_timer_mastergui
* @brief     ：主站链接定时同时显示
* @param[in] ：pModeSet    串口模式
               *pArg      入参
* @param[out]：
* @return    ：
* @Create    : 
**********************************************************************/
int comm_rs485_setMode(void *pArg, UART_MODESET_T *pModeSet)
{
    int     ret = 0;
    uint8   msg[128] = {0};
    uint16  msglen = 0;
    uint8   recvBuf[8] = {0}; 
    uint16  recvLen = 0;
    char *modestr[] = {"only", "master", "slave"};

    if ((pArg == NULL) && (pModeSet == NULL))
    {
        return ERR_PTR;
    }

    if (pModeSet->mode > UART_MODE_SLAVE)
    {
        return ERR_OPTION;
    }

    msglen = uart_axdr_set_mode_req_package(pModeSet, msg, 128);
    if (msglen > 0)
    {
        ret = comm_send_syncmsg(MSG_COMM_UART_MODESET, msg, msglen, recvBuf, 8, &recvLen);
        if(0 == ret && (1 != recvLen || UART_ACK_OK != recvBuf[0]))
        {
            ret = -1;
        }
        COMM_BUF_DEBUG(msg, msglen, "485-%d模式变更, 变更%s 返回[%d]", pModeSet->port.portNo, modestr[pModeSet->mode], ret);
    }

    return ret;
}

/**********************************************************************
* @name      : comm_timer_485_status
* @brief     ：485状态机切换
* @param[in] ：int socketfd    task消息句柄
               void *pArg      入参
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-7-17
* @Update    :
**********************************************************************/
void comm_timer_485_status(int socketfd, void *pArg)
{
    mtrnum_t    mtrId = MTR_ID_485;
    int         ret = 0;
    uint8       i = 0;
    UART_MODE_E        mode = UART_MODE_UNLL;
    COMM_485_HANDLE_T *ptHandle = (COMM_485_HANDLE_T *)pArg;
    OOP_METER_T        oopMeter = {0};
    UART_MODESET_T     modeSet = {0};

    if(socketfd <= 0 || NULL == pArg || 0 == *ptHandle->pIPCok)
    {
        return;
    }

    /* 串口相关操作 */
    switch(ptHandle->uartStatus)
    {
        case COMM_UART_STATUS_GETAPP:
        {
            /* 查询APP */
            uint8       msg[8] = {0};
            uint16      msglen = 0;
            uint8       bContinue = FALSE;
            
            for(i = 0; i < COMM_UART_ID_MAX; i++)
            {
                if(TRUE != ptHandle->uartMode[i].bGet)
                {
                    msglen = uart_axdr_get_info_req_package(&ptHandle->uartMode[i].modeinfo.port, msg, 8);
                    ret = comm_send_label_msg(MSG_COMM_GETUART, i, msg, msglen);
                    if(0 != ret)
                    {
                        COMM_FMT_DEBUG("Comm get uart app send failed %d\n", ret);
                    }
                    bContinue = TRUE;
                }
            }
            
            if(FALSE == bContinue && COMM_UART_STATUS_GETAPP == ptHandle->uartStatus)
            {
                ptHandle->uartStatus = COMM_UART_STATUS_CHECK;
            }
            
            break;
        }
        case COMM_UART_STATUS_CHECK:
        {
            /* 首先调度独占模式 */
            uint8 bContinue = FALSE;
            for(i = 0; i < COMM_UART_ID_MAX; i++)
            {
                if(TRUE == ptHandle->uartMode[i].bGet && TRUE == ptHandle->uartMode[i].bOnly)
                {
                    ptHandle->uartStatus = COMM_UART_STATUS_SETUART;
                    ptHandle->workNO = i;
                    bContinue = TRUE;
                    break;
                }
            }
            if(FALSE == bContinue)
            {
                /* 从模式只适配第四路485 */
                if(TRUE == ptHandle->uartMode[COMM_UART_ID].bSlave)
                {
                    ptHandle->workNO = COMM_UART_ID;
                    ptHandle->uartStatus = COMM_UART_STATUS_SETUART;
                }
                else
                {
                    ptHandle->workNO = 255;
                }
            }
            break;
        }
        case COMM_UART_STATUS_SETUART:
        {
            if(ptHandle->workNO > COMM_UART_ID)
            {
                ptHandle->uartStatus = COMM_UART_STATUS_CHECK;
            }
            else if(TRUE == ptHandle->uartMode[ptHandle->workNO].bOnly)
            {
                uint8         msg[128] = {0};
                uint16        msglen = 0;
                UART_APPREG_T tAppReg;
                
                tAppReg.port = ptHandle->uartMode[ptHandle->workNO].modeinfo.port;
                tAppReg.param = ptHandle->param;
                msglen = uart_axdr_set_param_req_package(&tAppReg, msg, 128);
                ret = comm_send_label_msg(MSG_COMM_SETUART, ptHandle->workNO, msg, msglen);
                if(0 != ret)
                {
                    COMM_FMT_DEBUG("Comm get uart app send failed %d\n", ret);
                }
            }
            else if(COMM_UART_ID == ptHandle->workNO && TRUE == ptHandle->uartMode[ptHandle->workNO].bSlave && ptHandle->bNoMeter == TRUE)
            {
                /* 第四路485注册 */
                uint8         msg[64] = {0};
                uint16        msglen = 0;
                UART_APPREG_T tPort;
                
                tPort.port = ptHandle->uartMode[ptHandle->workNO].modeinfo.port;
                msglen = uart_axdr_set_monitor_req_package(&tPort, msg, 64);
                ret = comm_send_label_msg(MSG_COMM_REGAPP, ptHandle->workNO, msg, msglen);
                if(0 != ret)
                {
                    COMM_FMT_DEBUG("Comm get uart app send failed %d\n", ret);
                }
            }
            break;
        }

        case COMM_UART_STATUS_WORK:
        {
            if(ptHandle->workNO > COMM_UART_ID)
            {
                ptHandle->uartStatus = COMM_UART_STATUS_CHECK;
            }
            else if(FALSE == ptHandle->uartMode[ptHandle->workNO].bOnly)
            {
                if(COMM_UART_ID == ptHandle->workNO && TRUE == ptHandle->uartMode[ptHandle->workNO].bSlave)
                {
                    break;
                }
                else
                {
                    ptHandle->workNO = 255;
                    ptHandle->uartStatus = COMM_UART_STATUS_CHECK;
                }
            }
            break;
        }
        default:
        {
            ptHandle->uartStatus = COMM_UART_STATUS_GETAPP;
            break;
        }
    }

    /* 判断是否存在档案配置在2路485 后期考虑在界面配置 F201为RS485 */
    ptHandle->bNoMeter = TRUE;
    while (mtrId <= NO_VIR_MTR_MAX_NUM)
    {
        if (0 == app_get_meter_list(mtrId, &oopMeter))
        {
            if (0xF2010202 == oopMeter.basic.port.value)
            {
                //485 2口有档案
                ptHandle->bNoMeter = FALSE;
                break;
            }
        }
        mtrId++;
    }

    mode = ptHandle->uartMode[COMM_UART_ID].modeinfo.mode;
    if (mode == UART_MODE_SLAVE && ptHandle->bNoMeter == FALSE)
    {
        COMM_FMT_DEBUG("从模式，但2口有表，切换主模式\n");
                
        //从模式，但2口有表，切换主模式
        modeSet.port.portType = UART_TYPE_485;
        modeSet.port.portNo = 2;
        modeSet.mode = UART_MODE_MASTER;
        ret = comm_rs485_setMode(ptHandle, &modeSet);
        if (ret == ERR_OK)
        {
            ptHandle->uartStatus = COMM_UART_STATUS_GETAPP;
            ptHandle->uartMode[COMM_UART_ID].modeinfo.mode = UART_MODE_UNLL;
            ptHandle->uartMode[COMM_UART_ID].bGet = 0;
        }
    }
    else if (mode != UART_MODE_SLAVE && ptHandle->bNoMeter == TRUE)
    {
        COMM_FMT_DEBUG("主模式，但2口无表，切换从模式\n");
                
        //主模式，但2口无表，切换从模式
        modeSet.port.portType = UART_TYPE_485;
        modeSet.port.portNo = 2;
        modeSet.mode = UART_MODE_SLAVE;
        modeSet.bParam = TRUE;
        modeSet.param.baudRate = 6;
        modeSet.param.parity = 2;
        modeSet.param.dataBits = 8;
        modeSet.param.stopBits = 1;
        modeSet.param.flowCtrl = 0;
        ret = comm_rs485_setMode(ptHandle, &modeSet);
        if (ret == ERR_OK)
        {
            ptHandle->uartStatus = COMM_UART_STATUS_GETAPP;
            ptHandle->uartMode[COMM_UART_ID].modeinfo.mode = UART_MODE_UNLL;
            ptHandle->uartMode[COMM_UART_ID].bGet = 0;
        }
    }
}

/**********************************************************************
* @name      : comm_485_statusmsg
* @brief     ：串口服务状态消息
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-7-31
* @Update    : 
**********************************************************************/
int comm_485_statusmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_485_HANDLE_T *ptHandle = (COMM_485_HANDLE_T *)pArg;
    MESSAGE_INFO_T    *pMsgInfo = (MESSAGE_INFO_T *)playload;
    int                ret = 0;
    UART_MODESTAUS_T   tModeInfo;
    char *modestr[] = {"only", "master", "slave"};
    
    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_485_statusmsg input null\n");
        return -1;
    }

    if(label > 3)
    {
        COMM_FMT_DEBUG("comm_485_statusmsg label %d\n", label);
        return -2;
    }

    ret = uart_axdr_get_info_res_split(pMsgInfo, &tModeInfo);
    if(0 != ret)
    {
        COMM_FMT_DEBUG("comm_485_statusmsg split error %d\n", label);
        return -2;
    }

    COMM_FMT_DEBUG("%s-%d模式: %s, 独占APP: %s label[%d] 波特率[%d] 校验[%d]\n", 
                   UART_TYPE_485 == tModeInfo.port.portType ? "RS485" : "RS232", tModeInfo.port.portNo,
                   tModeInfo.mode <= UART_MODE_SLAVE ? modestr[tModeInfo.mode] : "unknow", tModeInfo.onlyApp, 
                   label, tModeInfo.param.baudRate, tModeInfo.param.parity);

    if(ptHandle->uartMode[label].modeinfo.port.portType == tModeInfo.port.portType && 
       ptHandle->uartMode[label].modeinfo.port.portNo == tModeInfo.port.portNo)
    {
        ptHandle->uartMode[label].bGet = TRUE;
        ptHandle->uartMode[label].modeinfo.mode = tModeInfo.mode;
        memcpy(ptHandle->uartMode[label].modeinfo.curApp, tModeInfo.onlyApp, 64);
        if(UART_MODE_ONLY == tModeInfo.mode && 0 == strcmp(tModeInfo.onlyApp, ADV_APP_NAME))
        {
            ptHandle->uartMode[label].bOnly = TRUE;
        }
        else
        {
            ptHandle->uartMode[label].bOnly = FALSE;
        }
        ptHandle->uartMode[label].bSlave = UART_MODE_SLAVE == tModeInfo.mode ? TRUE : FALSE;
    }
    
    return 0;    
}

/**********************************************************************
* @name      : comm_485_setmsg
* @brief     ：串口模式消息
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-7-31
* @Update    : 
**********************************************************************/
int comm_485_setmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_485_HANDLE_T *ptHandle = (COMM_485_HANDLE_T *)pArg;
    MESSAGE_INFO_T    *pMsgInfo = (MESSAGE_INFO_T *)playload;
    uint8       bOK = UART_ACK_OTHER;
    int         ret = 0;

    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_485_setmsg input null\n");
        return -1;
    }

    if(label > COMM_UART_ID)
    {
        COMM_FMT_DEBUG("comm_485_setmsg label %d\n", label);
        return -2;
    }

    ret = uart_axdr_set_param_res_split(pMsgInfo, &bOK);
    if(0 != ret)
    {
        COMM_FMT_DEBUG("uart_axdr_set_param_res_split failed %d\n", ret);
        return ret;
    }
    
    if(label == ptHandle->workNO)
    {
        if(COMM_UART_STATUS_SETUART == ptHandle->uartStatus)
        {
            /* 设置新的状态 */
            if(UART_ACK_OK == bOK)
            {
                uint8 msgBuf[512] = {0};
                uint16 msgLen = 0;
                COMM_IPCMSG_T *ptIPCMsg = NULL;
                CHANNEL_INFO_T *ptChanInfo = NULL;

                COMM_FMT_DEBUG("设置485参数成功\n");
                msgLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                ptIPCMsg = (COMM_IPCMSG_T *)msgBuf;
                ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                ptChanInfo->workmode = 2;
                ptChanInfo->medium = COMM_MEDIA_485;
                ptChanInfo->connType = 0;
                ptChanInfo->bFirst = TRUE;
                ptChanInfo->heart = 0;
                ptIPCMsg->id = JOIN485_1;
                ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
                ptChanInfo->dailcnt = 3;
                ptChanInfo->dailtime = 60;
                ptChanInfo->ip = 0;
                ptChanInfo->port = 0;

                comm_write_queue(ptHandle->ptRecvList, msgBuf, msgLen);
                COMM_FMT_DEBUG("485维护口打开通道%d\n", ptIPCMsg->id);
                ptHandle->uartStatus = COMM_UART_STATUS_WORK;
            }
            else
            {
                COMM_FMT_DEBUG("设置485参数失败 错误码[%d]\n", bOK);
            }
        }
    }

    return 0;     
}

/**********************************************************************
* @name      : comm_485_regappmsg
* @brief     ：串口从模式注册消息
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-8-22
* @Update    : 
**********************************************************************/
int comm_485_regappmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_485_HANDLE_T *ptHandle = (COMM_485_HANDLE_T *)pArg;
    MESSAGE_INFO_T    *pMsgInfo = (MESSAGE_INFO_T *)playload;
    uint8       bOK = UART_ACK_OTHER;
    int         ret = 0;
    
    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_485_regappmsg input null\n");
        return -1;
    }

    if(label > COMM_UART_ID)
    {
        COMM_FMT_DEBUG("comm_485_regappmsg label %d\n", label);
        return -2;
    }

    ret = uart_axdr_set_monitor_res_split(pMsgInfo, &bOK);
    if(0 != ret)
    {
        COMM_FMT_DEBUG("uart_axdr_set_monitor_res_split failed %d\n", ret);
        return ret;
    }
    
    if(COMM_UART_ID == label)
    {
        if(COMM_UART_STATUS_SETUART == ptHandle->uartStatus)
        {
            /* 设置新的状态 */
            if(UART_ACK_OK == bOK)
            {
                uint8 msgBuf[512] = {0};
                uint16 msgLen = 0;
                COMM_IPCMSG_T *ptIPCMsg = NULL;
                CHANNEL_INFO_T *ptChanInfo = NULL;

                COMM_FMT_DEBUG("485-%d注册从模式成功\n", ptHandle->workNO+1);
                msgLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                ptIPCMsg = (COMM_IPCMSG_T *)msgBuf;
                ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                ptChanInfo->workmode = 2;
                ptChanInfo->medium = COMM_MEDIA_485;
                ptChanInfo->connType = 0;
                ptChanInfo->bFirst = TRUE;
                ptChanInfo->heart = 0;
                ptIPCMsg->id = JOIN485_1;
                ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
                ptChanInfo->dailcnt = 3;
                ptChanInfo->dailtime = 60;
                ptChanInfo->ip = 0;
                ptChanInfo->port = 0;

                comm_write_queue(ptHandle->ptRecvList, msgBuf, msgLen);
                COMM_FMT_DEBUG("485维护口打开通道%d\n", ptIPCMsg->id);
                ptHandle->uartStatus = COMM_UART_STATUS_WORK;
            }
            else
            {
                COMM_FMT_DEBUG("485-4注册从模式失败 错误码[%d]\n", bOK);
            }
        }
    }

    return 0;     
}


/**********************************************************************
* @name      : comm_485_eventmsg
* @brief     ：串口通知消息
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-7-31
* @Update    : 
**********************************************************************/
int comm_485_eventmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    int                ret = 0;
    uint8              i = 0;
    MESSAGE_INFO_T    *pMsgInfo = (MESSAGE_INFO_T *)playload;
    COMM_485_HANDLE_T *ptHandle = (COMM_485_HANDLE_T *)pArg;
    UART_MODEINFO_T    tUartMode;
    char *modestr[] = {"only", "master", "slave"};
    char *portstr[] = {"RS485", "RS232", "CAN"};
    
    ret = uart_axdr_notify_mode_split(pMsgInfo, &tUartMode);
    if(0 != ret)
    {
        COMM_FMT_DEBUG("uart_axdr_notify_mode_split failed %d\n", ret);
        return ret;
    }

    COMM_FMT_DEBUG("%s-%d通知, 模式: %s, 独占APP: %s\n", portstr[tUartMode.port.portType], tUartMode.port.portNo,
                   tUartMode.mode <= UART_MODE_SLAVE ? modestr[tUartMode.mode] : "unknow", tUartMode.curApp);

    for(i = 0; i < COMM_UART_ID_MAX; i++)
    {
        if(0 == memcmp(&(ptHandle->uartMode[i].modeinfo.port), &tUartMode.port, sizeof(UART_PORT_T)))
        {
            ptHandle->uartMode[i].bGet = TRUE;
            ptHandle->uartMode[i].modeinfo.mode = tUartMode.mode;
            if(UART_MODE_ONLY == tUartMode.mode && 0 == strcmp(tUartMode.curApp, ADV_APP_NAME))
            {
                ptHandle->uartMode[i].bOnly = TRUE;
            }
            else
            {
                ptHandle->uartMode[i].bOnly = FALSE;
            }
            ptHandle->uartMode[i].bSlave = UART_MODE_SLAVE == tUartMode.mode ? TRUE : FALSE;

            /* 串口模式修改 */
            if(i == ptHandle->workNO)
            {
                ptHandle->uartStatus = COMM_UART_STATUS_GETAPP;
                ptHandle->workNO = 255;
            }
            break;
        }
    }
    
    return 0;    
}

/**********************************************************************
* @name      : comm_485_recvmsg
* @brief     ：串口接收消息
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-7-31
* @Update    : 
**********************************************************************/
int comm_485_recvmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_485_HANDLE_T *ptHandle = (COMM_485_HANDLE_T *)pArg;
    MESSAGE_INFO_T    *pMsgInfo = (MESSAGE_INFO_T *)playload;
    uint8  dataBuf[4096 + 8] = {0};
    uint16 dataLen = 0;
    UART_RECV_DATA_T tRecv;
    int    ret = 0;

    if(COMM_UART_STATUS_WORK != ptHandle->uartStatus || ptHandle->workNO > COMM_UART_ID)
    {
        return 0;
    }

    ret = uart_axdr_data_recv_split(pMsgInfo, &tRecv);
    if(0 != ret)
    {
        COMM_FMT_DEBUG("uart_axdr_data_recv_split failed %d\n", ret);
        return ret;
    }

    if(0 != memcmp(&(ptHandle->uartMode[ptHandle->workNO].modeinfo.port), &tRecv.port, sizeof(UART_PORT_T))
       || 0 == tRecv.datalen || UART_RESULT_OK != tRecv.status)
    {
        return 0;
    }

    dataLen = MIN(tRecv.datalen, 4096);
    memcpy(dataBuf, tRecv.data, dataLen);

    /* 07规约过滤 */
    if(TRUE == check_07_frame(dataBuf, dataLen))
    {
        COMM_BUF_DEBUG(dataBuf, dataLen, "485-%d收到07规约", tRecv.port.portNo);
        return 0;
    }
    
    comm_add_channel(JOIN485_1, dataBuf, dataLen);
    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen + 4))
    {
        COMM_FMT_DEBUG("Local 485 queue is full\n");
    }
    COMM_BUF_DEBUG(dataBuf + 4, dataLen, "485-%d接收", tRecv.port.portNo);
    
    return 0;    
}

/**********************************************************************
* @name      : comm_rs485_start
* @brief     ：485 启动判断
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-17
* @Update    :
**********************************************************************/
BOOLEAN comm_rs485_start(void *pArg)
{
    COMM_485_HANDLE_T *ptHandle = (COMM_485_HANDLE_T *)pArg;

    if(0 == *ptHandle->pIPCok || COMM_UART_STATUS_WORK != ptHandle->uartStatus || ptHandle->workNO > COMM_UART_ID)
    {
        return FALSE;
    }
    return TRUE;
}

/**********************************************************************
* @name      : comm_rs485_entry
* @brief     ：485 线程入口
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-17
* @Update    :
**********************************************************************/
void *comm_rs485_entry(void *pArg)
{
    COMM_485_HANDLE_T *ptHandle = (COMM_485_HANDLE_T *)pArg;

    uint8   dataBuf[SERIAL_BUF_SIZE + 128] = {0}; 
    uint32  dataLen = 0;
    uint8   recvBuf[8] = {0}; 
    uint16  recvLen = 0;
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    int ret = 0;
    uint8 sendmsg[1] = {2}; //发送非法接入消息
    uint16 msglen = 1;
    UART_SEND_DATA_T tUratSend;

    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }

    memset(&tUratSend, 0, sizeof(UART_SEND_DATA_T));
    tUratSend.bParam = 1;
    tUratSend.param = ptHandle->param;
    while(1)
    {
        *ptHandle->pRunFlag = 0;

        /* Appmain重启 */
        if(1 == *ptHandle->pAppReStart || COMM_UART_STATUS_WORK != ptHandle->uartStatus || ptHandle->workNO > COMM_UART_ID)
        {
            /* 通知主应用调度线程通道关闭 */
            dataLen = sizeof(COMM_IPCMSG_T);
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = JOIN485_1;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
            COMM_FMT_DEBUG("485通道%d关闭\n", ptIPCMsg->id);

            *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX + 25;
            return 0;
        }

        dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
        if(dataLen > 4)
        {
            //维护口授权关闭时，只响应与手持终端的安全认证交互命令,
            if(app_secure_defend_isuse()&&(g_portRS485.status == STA_PORT_AUTH_FORBIDDEN)&&(g_portRS485.isAuthedcmd == FALSE))
            {
                ret = task_send_labelmsg(g_msgMonitor, MSG_APPAMIN_PORT485_AUTHORIZE, 0, sendmsg, msglen, APPMAIN_MSG);
                COMM_FMT_TRACE("portRS485 Send back to 0x%x: msg 0x%x, label %d, ret %d\n", APPMAIN_MSG, MSG_APPAMIN_UDISK_AUTHORIZE, 0, ret); 
                COMM_BUF_DEBUG(dataBuf + 4, dataLen - 4, "485-%d丢弃(不安全)", tUratSend.port.portNo);
                continue;
            }
            
            /* 发送 */
            if(ptHandle->workNO <= COMM_UART_ID)
            {
                tUratSend.port = ptHandle->uartMode[ptHandle->workNO].modeinfo.port;
            }
            tUratSend.datalen = MIN(dataLen - 4, UART_DATA_MAXLEN);
            memcpy(tUratSend.data, dataBuf + 4, tUratSend.datalen);
            memset(dataBuf, 0 , dataLen);
            dataLen = 0;

            dataLen = uart_axdr_data_send_req_package(&tUratSend, dataBuf, SERIAL_BUF_SIZE + 128);
            if(dataLen > 0)
            {
                ret = comm_send_syncmsg(MSG_COMM_SENDUART, dataBuf, dataLen, recvBuf, 8,&recvLen);
                if(0 == ret && (1 != recvLen || UART_ACK_OK != recvBuf[0]))
                {
                    ret = -1;
                }
                COMM_BUF_DEBUG(tUratSend.data, tUratSend.datalen, "485-%d发送%s", tUratSend.port.portNo, 
                                                                                   0 != ret ? "失败" : "");
            }
        }

        usleep(20000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

    pthread_exit(NULL);
}

#endif

/**********************************************************************
* @name      : local_232_start
* @brief     ：232串口 蓝牙 启动判断
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
BOOLEAN comm_rs232_start(void *pArg)
{
    COMM_232_HANDLE_T *ptHandle = (COMM_232_HANDLE_T *)pArg;

    if(0 == *ptHandle->pIPCok)
    {
        return FALSE;
    }
    return TRUE;
}

/**********************************************************************
* @name      : local_232_entry
* @brief     ：232串口 蓝牙 线程入口
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
void *comm_rs232_entry(void *pArg)
{
    UART_DEVICE_T  *dev = NULL;
    uint8   stateLocal = CHANNEL_STATE_INIT;  /* 运行状态 */
    uint8   dataBuf[SERIAL_BUF_SIZE + 8] = {0}; 
    uint32  dataLen = 0;
    CHANNEL_INFO_T *ptChanInfo = NULL;
    COMM_IPCMSG_T   *ptIPCMsg = NULL;
    COMM_232_HANDLE_T *ptHandle = (COMM_232_HANDLE_T *)pArg;

    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }

    if(ptHandle->pUart)
    {
        hal_device_release((HW_DEVICE *)ptHandle->pUart);
        ptHandle->pUart = NULL;
    }

    /* 清空队列 */
    comm_clean_queue(ptHandle->ptRecvList);
    comm_clean_queue(ptHandle->ptSendList);

    while(1)
    {
        *ptHandle->pRunFlag = 0;

        /* Appmain重启 */
        if(NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart)
        {
            /* 通知主应用调度线程通道打开 蓝牙默认开启 */
            dataLen = sizeof(COMM_IPCMSG_T);
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = LOCATE_232;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
            COMM_FMT_DEBUG("232串口通道%d关闭\n", ptIPCMsg->id);

            hal_device_release((HW_DEVICE *)ptHandle->pUart);
            ptHandle->pUart = NULL;
            *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
            return 0;
        }

        if((dev != NULL)&&(dev->uart_get_status(dev)< 0))
        {
            /* 通知主应用调度线程通道打开 蓝牙默认关闭 */
            dataLen = sizeof(COMM_IPCMSG_T);
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = LOCATE_232;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
            COMM_FMT_DEBUG("232串口设备异常,通道%d关闭\n", ptIPCMsg->id);
            
            hal_device_release((HW_DEVICE *)ptHandle->pUart);
            ptHandle->pUart = NULL;
            stateLocal = CHANNEL_STATE_INIT;
        }
        
        switch(stateLocal)
        {
            case CHANNEL_STATE_INIT:
            {
                dev = (UART_DEVICE_T*)hal_device_get(UART_232_DEVICE_ID_STR);
                if(dev)
                {
                    #if defined (AREA_DEFAULT) && defined (PRODUCT_ZCU_1) && !defined (BOARD_801)
                    //重庆的232维护口由协处理器扩展而来
                    strcpy(dev->devName, "/dev/ttyRS232");
                    #endif
                    if(0 != dev->uart_param_set(dev, 115200, 8, 1, PARITY_EVEN))
                    {
                        COMM_FMT_DEBUG("232串口设置失败!\n");
                        hal_device_release((HW_DEVICE *)dev);
                        dev = NULL;
                        sleep(10);
                        break;
                    }

                    /* 通知主应用调度线程通道打开 蓝牙默认开启 */
                    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                    ptChanInfo->workmode = 2;
                    ptChanInfo->medium = COMM_MEDIA_232;
                    ptChanInfo->connType = 0;
                    ptChanInfo->bFirst = TRUE;
                    ptChanInfo->heart = 0;
                    ptIPCMsg->id = LOCATE_232;
                    ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
                    ptChanInfo->dailcnt = 3;
                    ptChanInfo->dailtime = 60;
                    ptChanInfo->ip = 0;
                    ptChanInfo->port = 0;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("232串口通道%d打开\n", ptIPCMsg->id);
                    ptHandle->pUart = dev;
                    memset(dataBuf, 0, SERIAL_BUF_SIZE);
                    dataLen = 0;
                    stateLocal = CHANNEL_STATE_RECV;
                }
                else
                {
                    COMM_FMT_DEBUG("232串口打开失败\n");
                    sleep(10);
                }
                break;
            }
            case CHANNEL_STATE_RECV:
            {
                dataLen = local_read_serial(dev, 115200, dataBuf, SERIAL_BUF_SIZE-4); //预留4个字节作为通道头
                if (dataLen > 0)
                {
                    /* 首次连接答复设备地址 */
                    comm_add_channel(LOCATE_232, dataBuf, dataLen);
                    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen+4))
                    {
                        COMM_FMT_TRACE("Local queue is full\n");
                    }
                    COMM_BUF_DEBUG(dataBuf + 4, dataLen, "232串口接收");
                    memset(dataBuf, 0 , dataLen+4);
                }

                stateLocal = CHANNEL_STATE_SEND;
                break;
            }
            case CHANNEL_STATE_SEND:
            {
                dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
                if(dataLen > 0)
                {
                    /* 前4个字节作为发送标记 */
                    if(0xFE != dataBuf[4])
                    {
                        dataBuf[0] = 0xFE;
                        dataBuf[1] = 0xFE;
                        dataBuf[2] = 0xFE;
                        dataBuf[3] = 0xFE;
                        local_send_serial(dev, dataBuf, dataLen);
                        COMM_BUF_DEBUG(dataBuf, dataLen, "232串口发送");
                    }
                    else
                    {
                        local_send_serial(dev, dataBuf + 4, dataLen - 4);
                        COMM_BUF_DEBUG(dataBuf + 4, dataLen - 4, "232串口发送");
                    }
                    memset(dataBuf, 0 , dataLen);
                }
                stateLocal = CHANNEL_STATE_RECV;
                break;
            }
            case CHANNEL_STATE_CLOSE:
            {
                hal_device_release((HW_DEVICE *)dev);
                ptHandle->pUart = NULL;
                stateLocal = CHANNEL_STATE_INIT;
                break;
            }
            default:
            {
                stateLocal = CHANNEL_STATE_RECV;
                break;
            }
        }
        usleep(20000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

    pthread_exit(NULL);
}

/**********************************************************************
* @name      : comm_infrared_start
* @brief     ：红外启动判断
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
BOOLEAN comm_infrared_start(void *pArg)
{
    COMM_232_HANDLE_T *ptHandle = (COMM_232_HANDLE_T *)pArg;

    if(0 == *ptHandle->pIPCok)
    {
        return FALSE;
    }
    return TRUE;
}

/**********************************************************************
* @name      : local_232_entry
* @brief     ：232串口 蓝牙 线程入口
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
void *comm_infrared_entry(void *pArg)
{
    UART_DEVICE_T  *dev = NULL;
    uint8   stateLocal = CHANNEL_STATE_INIT;  /* 运行状态 */
    uint8   dataBuf[SERIAL_BUF_SIZE + 8] = {0}; 
    uint32  dataLen = 0;
    CHANNEL_INFO_T *ptChanInfo = NULL;
    COMM_IPCMSG_T   *ptIPCMsg = NULL;
    COMM_232_HANDLE_T *ptHandle = (COMM_232_HANDLE_T *)pArg;

    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }

    if(ptHandle->pUart)
    {
        hal_device_release((HW_DEVICE *)ptHandle->pUart);
        ptHandle->pUart = NULL;
    }

    /* 清空队列 */
    comm_clean_queue(ptHandle->ptRecvList);
    comm_clean_queue(ptHandle->ptSendList);

    while(1)
    {
        *ptHandle->pRunFlag = 0;

        /* Appmain重启 */
        if(NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart)
        {
            /* 通知主应用调度线程通道打开 蓝牙默认开启 */
            dataLen = sizeof(COMM_IPCMSG_T);
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = INFRARED;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
            COMM_FMT_DEBUG("红外端口%d关闭\n", ptIPCMsg->id);

            hal_device_release((HW_DEVICE *)ptHandle->pUart);
            ptHandle->pUart = NULL;
            *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
            return 0;
        }
        
        switch(stateLocal)
        {
            case CHANNEL_STATE_INIT:
            {
                dev = (UART_DEVICE_T*)hal_device_get(UART_IR_DEVICE_ID_STR);
                if(dev)
                {
                    if(0 != dev->uart_param_set(dev, 1200, 8, 1, PARITY_EVEN))
                    {
                        COMM_FMT_DEBUG("红外设置失败!\n");
                        hal_device_release((HW_DEVICE *)dev);
                        sleep(10);
                        break;
                    }

                    /* 通知主应用调度线程通道打开 蓝牙默认开启 */
                    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                    ptChanInfo->workmode = 2;
                    ptChanInfo->medium = COMM_MEDIA_IR;
                    ptChanInfo->connType = 0;
                    ptChanInfo->bFirst = TRUE;
                    ptChanInfo->heart = 0;
                    ptIPCMsg->id = INFRARED;
                    ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
                    ptChanInfo->dailcnt = 3;
                    ptChanInfo->dailtime = 60;
                    ptChanInfo->ip = 0;
                    ptChanInfo->port = 0;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("红外端口%d打开\n", ptIPCMsg->id);
                    ptHandle->pUart = dev;
                    memset(dataBuf, 0, SERIAL_BUF_SIZE);
                    dataLen = 0;
                    stateLocal = CHANNEL_STATE_RECV;
                }
                else
                {
                    COMM_FMT_DEBUG("红外打开失败!\n");
                    sleep(10);
                }
                break;
            }
            case CHANNEL_STATE_RECV:
            {
                dataLen = local_read_serial(dev, 1200, dataBuf, SERIAL_BUF_SIZE-4); //预留4个字节作为通道头
                if (dataLen > 0)
                {
                    /* 首次连接答复设备地址 */
                    comm_add_channel(INFRARED, dataBuf, dataLen);
                    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen+4))
                    {
                        COMM_FMT_TRACE("Local queue is full\n");
                    }
                    COMM_BUF_DEBUG(dataBuf + 4, dataLen, "红外接收");
                    memset(dataBuf, 0 , dataLen+4);
                }

                stateLocal = CHANNEL_STATE_SEND;
                break;
            }
            case CHANNEL_STATE_SEND:
            {
                dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
                if(dataLen > 0)
                {
                    /* 前4个字节作为发送标记 */
                    if(0xFE != dataBuf[4])
                    {
                        dataBuf[0] = 0xFE;
                        dataBuf[1] = 0xFE;
                        dataBuf[2] = 0xFE;
                        dataBuf[3] = 0xFE;
                        local_send_serial(dev, dataBuf, dataLen);
                        COMM_BUF_DEBUG(dataBuf, dataLen, "红外发送");
                    }
                    else
                    {
                        local_send_serial(dev, dataBuf + 4, dataLen - 4);
                        COMM_BUF_DEBUG(dataBuf + 4, dataLen - 4, "红外发送");
                    }
                    memset(dataBuf, 0 , dataLen);
                }
                stateLocal = CHANNEL_STATE_RECV;
                break;
            }
            case CHANNEL_STATE_CLOSE:
            {
                hal_device_release((HW_DEVICE *)dev);
                ptHandle->pUart = NULL;
                stateLocal = CHANNEL_STATE_INIT;
                break;
            }
            default:
            {
                stateLocal = CHANNEL_STATE_RECV;
                break;
            }
        }
        usleep(20000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

    pthread_exit(NULL);
}

