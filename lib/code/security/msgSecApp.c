/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#include "OopAll.h"
#include "framecomm.h"
#include "oadInfoList.h"
#include "security_msg.h"
#include "task_msg.h"
#include "gen_auth_data.h"
#include "commClass.h"
#include "security_msg.h"
#include "msgSecApp.h"
#include "appcrc16.h"

//UDP相关的句柄和锁
int             g_fUdpSecApp = 0;
pthread_mutex_t g_fUdpSecAppLock;

static char m_AppName[256] = {0};

//安全加固相关crc16
extern unsigned int sec_crc16(const char *buf, int len); 

/*
*********************************************************************
* @name      : sec_smiSsal_fd_init
* @brief     ：创建与安全APP交互的句柄
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int sec_smiSsal_fd_init(char *appName)
{
    if (appName != NULL)
    {
        memcpy(m_AppName, appName, strlen(appName) + 1);
    }

    if (access(SMISSAL_APP_DEST, F_OK) != 0)
    {
        TESAM_FMT_DEBUG("安全管理设备[%s]不存在\n", SMISSAL_APP_DEST);
        return ERR_NODEV;
    }

    if (g_fUdpSecApp <= 0)
    {
        char fileName[SMISSAL_FILEPATH_LEN] = {0};
        sprintf(fileName, "%s/%s_smiSsal", SMISSAL_MSG_PATH, m_AppName);
        g_fUdpSecApp = task_pfmsg_init(fileName);
        pthread_mutex_init(&g_fUdpSecAppLock, NULL);
    }

    if (g_fUdpSecApp <= 0)
    {
        TESAM_FMT_DEBUG("安全管理初始化 fd[%d]无效\n", g_fUdpSecApp);
        return ERR_NO_HANDLE;
    }

    return ERR_OK;
}

/*
*********************************************************************
* @name      : sec_smiSsal_revsnd_unit
* @brief     ：与安全加固交互的同步接口
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int sec_smiSsal_revsnd_unit(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen)
{
    int32 i = 0;
    int32 iRet = 0;
    uint8 czBuf[SMISSAL_MSG_MAX_LEN] = {0};
    uint8 sndBuf[SMISSAL_MSG_MAX_LEN] = {0};
    char  sender[SMISSAL_FILEPATH_LEN] = {0};

    if (sndLen > sizeof(sndBuf))
    {
        return ERR_OPTION;
    }

    if (g_fUdpSecApp <= 0)
    {
        if (0 != sec_smiSsal_fd_init(NULL))
        {
            return ERR_NO_HANDLE;
        } 
    }

    /* 尝试加锁 */
    while (0 != pthread_mutex_trylock(&g_fUdpSecAppLock))
    {
        if(i++ > 500)
        {
            TESAM_FMT_DEBUG("安全管理通信时尝试加锁失败 fd[%d]\n", g_fUdpSecAppLock);
            return ERR_NORESOURCE;
        }
        usleep(10000);
    }

    i = 0;
    /* 1. 1ms内收到消息, 等待0.1s的时间清空队列 */
    task_msg_settime(g_fUdpSecApp, 0, 1000);
    iRet = task_pfmsg_recv(&g_fUdpSecApp, czBuf, SMISSAL_MSG_MAX_LEN, sender, SMISSAL_FILEPATH_LEN);
    if (iRet > 0)
    {
        while (i < 100)
        {
            i++;
            task_pfmsg_recv(&g_fUdpSecApp, czBuf, SMISSAL_MSG_MAX_LEN, sender, SMISSAL_FILEPATH_LEN);
        }
    }

    /* 2. 发送消息 */
    memcpy(sndBuf, sndData, sndLen);
    TESAM_BUF_DEBUG(sndBuf, sndLen, "SEC SEND:");
    iRet = task_pfmsg_send(g_fUdpSecApp, sndBuf, sndLen, SMISSAL_APP_DEST);
    if (0 != iRet)
    {
        pthread_mutex_unlock(&g_fUdpSecAppLock);
        TESAM_FMT_DEBUG("发送消息失败, 错误码%d\n", iRet);
        close(g_fUdpSecApp);
        g_fUdpSecApp = -1;
        return ERR_NORESOURCE;
    }

    /* 3. 接收 2s超时 */
    task_msg_settime(g_fUdpSecApp, 2, 0);
    iRet = task_pfmsg_recv(&g_fUdpSecApp, czBuf, SMISSAL_MSG_MAX_LEN, sender, SMISSAL_FILEPATH_LEN);
    pthread_mutex_unlock(&g_fUdpSecAppLock);
    if (iRet < 0)
    {
        TESAM_FMT_DEBUG("接收消息失败, 错误码%d\n", iRet);
        close(g_fUdpSecApp);
        g_fUdpSecApp = -1;
        return ERR_NORESOURCE;
    }

    TESAM_BUF_DEBUG(czBuf, iRet, "SEC RECV:");

    *rcvLen = iRet;
    memcpy(pOutBuf, czBuf, *rcvLen);

    return ERR_OK;
}

/*
*********************************************************************
* @name      : sec_smiSsal_revsnd_unit
* @brief     ：与安全加固交互的同步接口
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int sec_smiSsal_revsnd(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen)
{
    uint32 i = 0;
    int8   ret = ESAM_FAIL;

    while (i < 2)
    {
        ret = sec_smiSsal_revsnd_unit(sndData, sndLen, pOutBuf, rcvLen);
        if (ERR_OK == ret)
        {
            break;
        }
        else
        {
            i++;
            usleep(10000);
        }
    }

    return ret;
}
#if SEC_NONSTANDARD == 1
/*
*********************************************************************
* @name      : sec_app_msg_package
* @brief     ：组织安全报文
* @param[in] ：sndData  - 数据源
* @param[out]：pSendBuf - 安全报文
               sendLen  - 安全报文长度
* @return    ：错误码
*********************************************************************
*/
int sec_app_msg_package(SEC_DATA_T *sndData, uint8 *pSendBuf, uint16 *sendLen)
{
    ST_APP_OBJECT_T data;

    if (pSendBuf == NULL)
    {
        TESAM_FMT_DEBUG("sec_app_msg_package ptr[%p] err\n", pSendBuf);
        return ERR_PTR;
    }

    memset(&data, 0, sizeof(data));
    
    data.DataType = sndData->dataType;
    data.CryptType = sndData->cryptType;
    data.app_type = sndData->appType;
    memcpy(data.time, sndData->time, sizeof(sndData->time));
    memcpy(&data.dev_addr_len, &sndData->devAddrLen, sizeof(sndData->devAddrLen));
    data.dev_addr = NULL;
    memcpy(&data.dst_ip, sndData->dstIp, sizeof(sndData->dstIp));
    memcpy(&data.policy, &sndData->policy, sizeof(sndData->policy));
    memcpy(&data.LenData, &sndData->lenData, sizeof(sndData->lenData));
    memcpy(&data.ApduData[0], sndData->data, sndData->lenData);

    data.crc16 = sec_crc16((const char *)&data, sizeof(data) - 4);

    memcpy(pSendBuf, &data, sizeof(data));
    *sendLen = sizeof(data);

    return ERR_OK;
}

/*
*********************************************************************
* @name      : sec_app_msg_split
* @brief     ：解析安全报文
* @param[in] ：sndData - 数据源
* @param[out]：pOutBuf - 安全报文
               rcvLen  - 安全报文长度
* @return    ：错误码
*********************************************************************
*/
int sec_app_msg_split(uint8 *pInBuf, uint16 inlen, SEC_DATA_T *outData)
{
    uint32 CRC = 0;

    ST_APP_OBJECT_T data;

    if ((pInBuf == NULL) || (outData == NULL) || (outData->data == NULL))
    {
        TESAM_FMT_DEBUG("sec_app_msg_split pInBuf[%p] outData[%p] outData->data[%p] err\n", 
            pInBuf, outData, outData->data);
        return ERR_PTR;
    }

    memset(&data, 0, sizeof(data));

    memcpy(&data, pInBuf, sizeof(data));

    outData->CRC = data.crc16;
    memcpy(&outData->CRC, &data.crc16, 2);
    
    CRC = sec_crc16((const char *)&pInBuf[0], sizeof(data) - 4);
    if (data.crc16 != CRC)
    {
        TESAM_FMT_DEBUG("app_crc16_get err data.crc16[0x%08x]. outData->CRC[0x%04x] CRC[0x%04x] err\n", 
                    data.crc16, outData->CRC, CRC);
        return ERR_CHK;
    }

    outData->dataType = data.DataType;
    outData->cryptType = data.CryptType;
    outData->appType = data.app_type;
    memcpy(outData->time, data.time, sizeof(outData->time));
    memcpy(&outData->devAddrLen, &data.dev_addr_len, sizeof(outData->devAddrLen));
    memset(&outData->devAddr, 0, sizeof(outData->devAddr));
    memcpy(outData->dstIp, &data.dst_ip, sizeof(outData->dstIp));
    memcpy(&outData->policy, &data.policy, sizeof(outData->policy));
    memcpy(&outData->lenData, &data.LenData, sizeof(outData->lenData));
    memcpy(outData->data, &data.ApduData[0], data.LenData);

    return ERR_OK;
}
#endif

#if SEC_NONSTANDARD == 0
/*
*********************************************************************
* @name      : sec_app_msg_package
* @brief     ：组织安全报文
* @param[in] ：sndData  - 数据源
* @param[out]：pSendBuf - 安全报文
               sendLen  - 安全报文长度
* @return    ：错误码
*********************************************************************
*/
int sec_app_msg_package(SEC_DATA_T *sndData, uint8 *pSendBuf, uint16 *sendLen)
{
    uint32 offset = 0;
    uint8  msg[SMISSAL_MSG_MAX_LEN] = {0};

    if (pSendBuf == NULL)
    {
        TESAM_FMT_DEBUG("安全报文参数错误 ptr[%p]\n", pSendBuf);
        return ERR_PTR;
    }

    //数据类型
    msg[offset ++] = sndData->dataType;

    //加密类型
    msg[offset ++] = sndData->cryptType;
    
    //应用报文类型
    msg[offset ++] = sndData->appType;

    //终端地址长度
    msg[offset ++] = sndData->devAddrLen;

    //终端地址
    if (sndData->devAddrLen > 0)
    {
        memcpy_r(&msg[offset], sndData->devAddr, sndData->devAddrLen);
        offset += sndData->devAddrLen;
    }

    //数据长度
    memcpy(&msg[offset], &sndData->lenData, sizeof(sndData->lenData));
    offset += sizeof(sndData->lenData);

    //数据内容
    if ((sndData->lenData > 0) && (sndData->data != NULL))
    {
        memcpy(&msg[offset], sndData->data, sndData->lenData);
        offset += sndData->lenData;
    }

    //校验
    sndData->CRC = sec_crc16((const char *)&msg[0], offset);
    memcpy(&msg[offset], &sndData->CRC, sizeof(sndData->CRC));
    offset += sizeof(sndData->CRC);

    memcpy(pSendBuf, msg, offset);
    *sendLen = offset;

    return ERR_OK;
}

/*
*********************************************************************
* @name      : sec_app_msg_split
* @brief     ：解析安全报文
* @param[in] ：sndData - 数据源
* @param[out]：pOutBuf - 安全报文
               rcvLen  - 安全报文长度
* @return    ：错误码
*********************************************************************
*/
int sec_app_msg_split(uint8 *pInBuf, uint16 inlen, SEC_DATA_T *outData)
{
    uint32 CRC = 0;
    uint32 offset = 0;

    if ((pInBuf == NULL) || (outData == NULL) || (outData->data == NULL))
    {
        TESAM_FMT_DEBUG("安全报文参数错误 pInBuf[%p] outData[%p] outData->data[%p]\n", 
            pInBuf, outData, outData->data);
        return ERR_PTR;
    }

    //数据类型
    outData->dataType = pInBuf[offset ++];

    //加密类型
    outData->cryptType = pInBuf[offset ++];

    //应用报文类型
    outData->appType = pInBuf[offset ++];

    //终端地址长度
    outData->devAddrLen = pInBuf[offset ++];

    //终端地址
    if (outData->devAddrLen > 0)
    {
        memcpy_r(outData->devAddr, &pInBuf[offset], outData->devAddrLen);
        offset += outData->devAddrLen;
    }

    //数据长度
    memcpy(&outData->lenData, &pInBuf[offset], sizeof(outData->lenData));
    offset += sizeof(outData->lenData);

    //数据内容
    if ((outData->lenData > 0) && ((offset + outData->lenData) <= inlen))
    {
        memcpy(outData->data, &pInBuf[offset], outData->lenData);
        offset += outData->lenData;
    }

    memcpy(&outData->CRC, &pInBuf[offset], 2);
    CRC = sec_crc16((const char *)&pInBuf[0], offset);
    if (outData->CRC != CRC)
    {
        TESAM_FMT_DEBUG("安全报文校验错误  outData->CRC[0x%04x] CRC[0x%04x] err\n", 
            outData->CRC, CRC);
        return ERR_CHK;
    }

    return ERR_OK;
}
#endif

//
///*
//*********************************************************************
//* @name      : sec_app_msg_package
//* @brief     ：组织安全报文
//* @param[in] ：sndData  - 数据源
//* @param[out]：pSendBuf - 安全报文
//               sendLen  - 安全报文长度
//* @return    ：错误码
//*********************************************************************
//*/
//int sec_app_msg_package(SEC_DATA_T *sndData, uint8 *pSendBuf, uint16 *sendLen)
//{
//    uint32 offset = 0;
//    uint8  msg[SMISSAL_MSG_MAX_LEN] = {0};
//
//    if (pSendBuf == NULL)
//    {
//        TESAM_FMT_DEBUG("安全报文参数错误 ptr[%p]\n", pSendBuf);
//        return ERR_PTR;
//    }
//
//    //数据类型
//    msg[offset ++] = sndData->dataType;
//
//    //加密类型
//    msg[offset ++] = sndData->cryptType;
//    
//    //应用报文类型
//    msg[offset ++] = sndData->appType;
//
//    //终端地址长度
//    msg[offset ++] = sndData->devAddrLen;
//
//    //终端地址
//    if (sndData->devAddrLen > 0)
//    {
//        memcpy_r(&msg[offset], sndData->devAddr, sndData->devAddrLen);
//        offset += sndData->devAddrLen;
//    }
//
//    //数据长度
//    memcpy(&msg[offset], &sndData->lenData, sizeof(sndData->lenData));
//    offset += sizeof(sndData->lenData);
//
//    //数据内容
//    if ((sndData->lenData > 0) && (sndData->data != NULL))
//    {
//        memcpy(&msg[offset], sndData->data, sndData->lenData);
//        offset += sndData->lenData;
//    }
//
//    //校验
//    sndData->CRC = sec_crc16((const char *)&msg[0], offset);
//    memcpy(&msg[offset], &sndData->CRC, sizeof(sndData->CRC));
//    offset += sizeof(sndData->CRC);
//
//    memcpy(pSendBuf, msg, offset);
//    *sendLen = offset;
//
//    return ERR_OK;
//}
//
///*
//*********************************************************************
//* @name      : sec_app_msg_split
//* @brief     ：解析安全报文
//* @param[in] ：sndData - 数据源
//* @param[out]：pOutBuf - 安全报文
//               rcvLen  - 安全报文长度
//* @return    ：错误码
//*********************************************************************
//*/
//int sec_app_msg_split(uint8 *pInBuf, uint16 inlen, SEC_DATA_T *outData)
//{
//    uint32 CRC = 0;
//    uint32 offset = 0;
//
//    if ((pInBuf == NULL) || (outData == NULL) || (outData->data == NULL))
//    {
//        TESAM_FMT_DEBUG("安全报文参数错误 pInBuf[%p] outData[%p] outData->data[%p]\n", 
//            pInBuf, outData, outData->data);
//        return ERR_PTR;
//    }
//
//    //数据类型
//    outData->dataType = pInBuf[offset ++];
//
//    //加密类型
//    outData->cryptType = pInBuf[offset ++];
//
//    //应用报文类型
//    outData->appType = pInBuf[offset ++];
//
//    //终端地址长度
//    outData->devAddrLen = pInBuf[offset ++];
//
//    //终端地址
//    if (outData->devAddrLen > 0)
//    {
//        memcpy_r(outData->devAddr, &pInBuf[offset], outData->devAddrLen);
//        offset += outData->devAddrLen;
//    }
//
//    //数据长度
//    memcpy(&outData->lenData, &pInBuf[offset], sizeof(outData->lenData));
//    offset += sizeof(outData->lenData);
//
//    //数据内容
//    if ((outData->lenData > 0) && ((offset + outData->lenData) <= inlen))
//    {
//        memcpy(outData->data, &pInBuf[offset], outData->lenData);
//        offset += outData->lenData;
//    }
//
//    memcpy(&outData->CRC, &pInBuf[offset], 2);
//    CRC = sec_crc16((const char *)&pInBuf[0], offset);
//    if (outData->CRC != CRC)
//    {
//        TESAM_FMT_DEBUG("安全报文校验错误  outData->CRC[0x%04x] CRC[0x%04x] err\n", 
//            outData->CRC, CRC);
//        return ERR_CHK;
//    }
//
//    return ERR_OK;
//}

/*
*********************************************************************
* @name      : sec_app_send_recv_data
* @brief     ：同步接口
* @param[in] ：sndData - 数据源
* @param[out]：pOutBuf - 目的数据
* @return    ：错误码
*********************************************************************
*/
int sec_app_send_recv_data(SEC_DATA_T *sendData, SEC_DATA_T *recvData)
{
    int    ret = 0;
    uint16 sendLen  = 0;
    uint8  sendBuf[SMISSAL_MSG_MAX_LEN] = {0};
    uint16 recvLen  = 0;
    uint8  recvBuf[SMISSAL_MSG_MAX_LEN] = {0};

    if ((sendData == NULL) || (recvData == NULL) || (recvData->data == NULL))
    {
        TESAM_FMT_DEBUG("安全报文参数错误 sendData[%p] recvData[%p] sendData->data[%p] recvData->data[%p] err\n", 
            sendData, recvData, sendData->data, recvData->data);
        return ERR_PTR;
    }

    ret = sec_app_msg_package(sendData, sendBuf, &sendLen);
    if (ret != ERR_OK)
    {
        TESAM_FMT_DEBUG("安全报文组织错误 错误码[%d]\n", ret);
        return ERR_IFACE;
    }
    
    ret = sec_smiSsal_revsnd(sendBuf, sendLen, recvBuf, &recvLen);
    if (ret != ERR_OK)
    {
        MSGAC_BUF_DEBUG(sendBuf, sendLen, "安全报文传输错误 错误码(%d)\n", ret);
        return ERR_NORMAL;
    }

    ret = sec_app_msg_split(recvBuf, recvLen, recvData);
    if (ret != ERR_OK)
    {
        TESAM_FMT_DEBUG("安全报文解析错误 错误码[%d]\n", ret);
        return ERR_IFACE;
    }

    return ERR_OK;
}

/*
*********************************************************************
* @name      : sec_info_management
* @brief     ：smiSsal交互信息
* @param[in] ：sndApdu - 输入APDU
               sndLen  - 长度
* @param[out]：rcvApdu - 输出APDU
               rcvLen  - 报文长度
* @return    ：错误码
*********************************************************************
*/
int sec_info_management(uint8 *send, uint16 sndLen, uint8 *recv, uint16 *rcvLen)
{
    int ret = ERR_OK;
    SEC_DATA_T sndData;
    SEC_DATA_T rcvData;

    if ((send == NULL) || (recv == NULL) || (rcvLen == NULL))
    {
        TESAM_FMT_DEBUG("安全报文参数错误 sndApdu[%p] rcvApdu[%p] rcvLen[%p] err\n", 
            send, recv, rcvLen);
        return ERR_PTR;
    }

    memset(&sndData, 0, sizeof(sndData));
    memset(&rcvData, 0, sizeof(rcvData));

    sndData.data = send;
    sndData.lenData = sndLen;

    rcvData.data = recv;

    ret = sec_app_send_recv_data(&sndData, &rcvData);
    if (ret != ERR_OK)
    {
        TESAM_FMT_DEBUG("安全报文传输错误 错误码[%d]\n", ret);
        return ERR_IFACE;
    }

    *rcvLen = rcvData.lenData;

    return ERR_OK;
}

/*
*********************************************************************
* @name      : sec_get_soft_summary
* @brief     ：smiSsal交互信息
* @param[in] ：addr     - 终端地址
               addrLen - 长度
               req       请求信息
* @param[out]：res        响应信息
* @return    ：错误码
*********************************************************************
*/
int sec_get_soft_summary(uint8 *addr, uint8 addrLen, OOP_SOFT_SUMMARYS_REQ *req, OOP_SOFT_SUMMARYS_RES *res)
{
    int i = 0;
    int ret = ERR_OK;
    SEC_DATA_T sndData;
    SEC_DATA_T rcvData;

    if ((req == NULL) || (res == NULL) || (addr == NULL))
    {
        TESAM_FMT_DEBUG("安全报文参数错误 req[%p] res[%p] err\n", req, res);
        return ERR_PTR;
    }

    res->nNum = req->nNum;
    
    for (i = 0; i < req->nNum; i++)
    {
        res->item[i].cpuNo = req->item[i].cpuNo;
        memcpy(&res->item[i].name, &req->item[i].name, sizeof(req->item[i].name));
        
        memset(&sndData, 0, sizeof(sndData));
        memset(&rcvData, 0, sizeof(rcvData));

        sndData.dataType = SEC_DATA_SUMMARY;
        sndData.devAddrLen = addrLen;
        if (addrLen > 0)
        {
            memcpy(sndData.devAddr, addr, MIN(16, addrLen));
        }
        sndData.data = (uint8 *)&req->item[i].name.value[0];
        sndData.lenData = req->item[i].name.nNum;

        rcvData.data = (uint8 *)&res->item[i].data.value[0];

        ret = sec_app_send_recv_data(&sndData, &rcvData);
        if (ret != ERR_OK)
        {
            TESAM_FMT_DEBUG("安全报文传输错误 错误码[%d]\n", ret);
            return ERR_IFACE;
        }

        res->item[i].data.nNum = rcvData.lenData;
    }

    return ERR_OK;
}

/*
*********************************************************************
* @name      : sec_get_app_tag
* @brief     ：smiSsal交互信息
* @param[in] ：addr     - 终端地址
               addrLen - 长度
* @param[out]：tag        响应信息
* @return    ：错误码
*********************************************************************
*/
int sec_get_app_tag(uint8 *addr, uint8 addrLen, OOP_OCTETVAR1K_T *tag)
{
    int ret = ERR_OK;
    SEC_DATA_T sndData;
    SEC_DATA_T rcvData;

    if ((tag == NULL) || (addr == NULL))
    {
        TESAM_FMT_DEBUG("安全报文参数错误 tag[%p] res[%p] addr\n", tag, addr);
        return ERR_PTR;
    }

    memset(&sndData, 0, sizeof(sndData));
    memset(&rcvData, 0, sizeof(rcvData));

    sndData.dataType = SEC_DATA_TAG;
    sndData.devAddrLen = addrLen;
    if (addrLen > 0)
    {
        memcpy(sndData.devAddr, addr, MIN(16, addrLen));
    }
    
    sndData.lenData = 0;
    rcvData.data = &tag->value[0];

    ret = sec_app_send_recv_data(&sndData, &rcvData);
    if (ret != ERR_OK)
    {
        TESAM_FMT_DEBUG("安全报文传输错误 错误码[%d]\n", ret);
        return ERR_IFACE;
    }

    tag->nNum = rcvData.lenData;

    return ERR_OK;
}
#if DESC("集抄APP和安全APP交互相关接口",1)
///**********************************************************************
//* @brief     ：集抄APP登录请求
//* @param[in] ：devAddr   - 终端地址
//               dstIp     - 目的IP地址
//               sndMsg    - 输入报文
//               sndLen    - 长度
//* @param[out]：rcvMsg     - 输出解析后报文
//               rcvLen    - 报文长度
//* @return    ：错误码
//**********************************************************************/
//int sec_app_login_req(SEC_APP_DATA_T *pdata)
//{
//    int ret = ERR_OK;
//    SEC_DATA_T sndData;
//    SEC_DATA_T rcvData;
//
//    if ((pdata == NULL) || (pdata->sndMsg == NULL) || (pdata->rcvMsg == NULL) || (pdata->devAddrLen > 16))
//    {
//        TESAM_FMT_DEBUG("esam_info_Get_and_Act sndMsg[%p] sndMsg[%p]err\n", 
//            pdata->sndMsg, pdata->rcvMsg);
//        return ERR_PTR;
//    }
//
//    memset(&sndData, 0, sizeof(sndData));
//    memset(&rcvData, 0, sizeof(rcvData));
//
//    sndData.dataType = 1;
//    sndData.appType = 1;
//    sndData.data = pdata->sndMsg;
//    sndData.lenData = pdata->sndLen;
//    memcpy(sndData.devAddr, pdata->devAddr, pdata->devAddrLen);
//    memcpy(sndData.dstIp, pdata->dstIp, sizeof(pdata->dstIp));
//    get_local_time(sndData.time);
//    
//    rcvData.data = pdata->rcvMsg;
//
//    ret = sec_app_send_recv_data(&sndData, &rcvData);
//    if (ret != ERR_OK)
//    {
//        TESAM_FMT_DEBUG("sec_app_login_req err ret[%d]\n", ret);
//        return ERR_IFACE;
//    }
//
//    pdata->rcvLen = rcvData.lenData;
//
//    return ERR_OK;
//}
//
///**********************************************************************
//* @brief     ：终端信息请求
//* @param[in] ：devAddr   - 终端地址
//               dstIp     - 目的IP地址
//               sndMsg    - 输入报文
//               sndLen    - 长度
//               ptime     - 网关时间
//* @param[out]：rcvMsg     - 输出解析后报文
//               rcvLen    - 报文长度
//* @return    ：错误码
//**********************************************************************/
//int sec_app_terminal_req(SEC_APP_DATA_T *pdata, uint8 *ptime, uint16 timeLen)
//{
//    int ret = ERR_OK;
//    SEC_DATA_T sndData;
//    SEC_DATA_T rcvData;
//
//    if ((pdata == NULL) || (pdata->sndMsg == NULL) || (pdata->rcvMsg == NULL) || (pdata->devAddrLen > 16) || timeLen < 7)
//    {
//        TESAM_FMT_DEBUG("esam_info_Get_and_Act sndMsg[%p] sndMsg[%p]err\n", 
//            pdata->sndMsg, pdata->rcvMsg);
//        return ERR_PTR;
//    }
//
//    memset(&sndData, 0, sizeof(sndData));
//    memset(&rcvData, 0, sizeof(rcvData));
//
//    sndData.dataType = 2;
//    sndData.data = pdata->sndMsg;
//    sndData.lenData = pdata->sndLen;
//    memcpy(sndData.devAddr, pdata->devAddr, pdata->devAddrLen);
//    memcpy(sndData.dstIp, pdata->dstIp, sizeof(pdata->dstIp));
//    memcpy(sndData.time, ptime, sizeof(sndData.time));
//    
//    rcvData.data = pdata->rcvMsg;
//
//    ret = sec_app_send_recv_data(&sndData, &rcvData);
//    if (ret != ERR_OK)
//    {
//        TESAM_FMT_DEBUG("sec_app_terminal_req err ret[%d]\n", ret);
//        return ERR_IFACE;
//    }
//
//    pdata->rcvLen = rcvData.lenData;
//
//    return ERR_OK;
//}
//
///**********************************************************************
//* @brief     ：网关协商请求
//* @param[in] ：devAddr   - 终端地址
//               dstIp     - 目的IP地址
//               sndMsg    - 输入报文
//               sndLen    - 长度
//* @param[out]：rcvMsg     - 输出解析后报文
//               rcvLen    - 报文长度
//* @return    ：错误码
//**********************************************************************/
//int sec_app_gateway_req(SEC_APP_DATA_T *pdata)
//{
//    int ret = ERR_OK;
//    SEC_DATA_T sndData;
//    SEC_DATA_T rcvData;
//
//    if ((pdata == NULL) || (pdata->sndMsg == NULL) || (pdata->rcvMsg == NULL) || (pdata->devAddrLen > 16))
//    {
//        TESAM_FMT_DEBUG("esam_info_Get_and_Act sndMsg[%p] sndMsg[%p]err\n", 
//            pdata->sndMsg, pdata->rcvMsg);
//        return ERR_PTR;
//    }
//
//    memset(&sndData, 0, sizeof(sndData));
//    memset(&rcvData, 0, sizeof(rcvData));
//
//    sndData.dataType = 2;
//    sndData.data = pdata->sndMsg;
//    sndData.lenData = pdata->sndLen;
//    memcpy(sndData.devAddr, pdata->devAddr, pdata->devAddrLen);
//    memcpy(sndData.dstIp, pdata->dstIp, sizeof(pdata->dstIp));
//    get_local_time(sndData.time);
//    
//    rcvData.data = pdata->rcvMsg;
//
//    ret = sec_app_send_recv_data(&sndData, &rcvData);
//    if (ret != ERR_OK)
//    {
//        TESAM_FMT_DEBUG("sec_app_gateway_req err ret[%d]\n", ret);
//        return ERR_IFACE;
//    }
//
//    pdata->rcvLen = rcvData.lenData;
//
//    return ERR_OK;
//}
//
///**********************************************************************
//* @brief     ：集抄APP封装心跳报文
//* @param[in] ：policy     - 处理策略
//               sndMsg    - 输入报文
//               sndLen    - 长度
//* @param[out]：rcvMsg     - 输出解析后报文
//               rcvLen    - 报文长度
//* @return    ：错误码
//**********************************************************************/
//int sec_app_heart(uint8 policy, SEC_APP_DATA_T *pdata)
//{
//    int ret = ERR_OK;
//    SEC_DATA_T sndData;
//    SEC_DATA_T rcvData;
//
//    if ((pdata == NULL) || (pdata->sndMsg == NULL) || (pdata->rcvMsg == NULL) || (pdata->devAddrLen > 16))
//    {
//        TESAM_FMT_DEBUG("esam_info_Get_and_Act sndMsg[%p] sndMsg[%p]err\n", 
//            pdata->sndMsg, pdata->rcvMsg);
//        return ERR_PTR;
//    }
//
//    memset(&sndData, 0, sizeof(sndData));
//    memset(&rcvData, 0, sizeof(rcvData));
//
//    sndData.dataType = 1;
//    sndData.policy = policy;
//    sndData.data = pdata->sndMsg;
//    sndData.lenData = pdata->sndLen;
//    memcpy(sndData.devAddr, pdata->devAddr, pdata->devAddrLen);
//    memcpy(sndData.dstIp, pdata->dstIp, sizeof(pdata->dstIp));
//    get_local_time(sndData.time);
//
//    rcvData.data = pdata->rcvMsg;
//
//    ret = sec_app_send_recv_data(&sndData, &rcvData);
//    if (ret != ERR_OK)
//    {
//        TESAM_FMT_DEBUG("sec_app_heart err ret[%d]\n", ret);
//        return ERR_IFACE;
//    }
//
//    pdata->rcvLen = rcvData.lenData;
//
//    return ERR_OK;
//}
//
///**********************************************************************
//* @brief     ：加密集抄APP需要封装成SSAL报文
//* @param[in] ：policy     - 处理策略
//               sndMsg    - 输入报文
//               sndLen    - 长度
//* @param[out]：rcvMsg     - 输出解析后报文
//               rcvLen    - 报文长度
//* @return    ：错误码
//**********************************************************************/
//int sec_app_encryption(uint8 policy, SEC_APP_DATA_T *pdata)
//{
//    int ret = ERR_OK;
//    SEC_DATA_T sndData;
//    SEC_DATA_T rcvData;
//
//    if ((pdata == NULL) || (pdata->sndMsg == NULL) || (pdata->rcvMsg == NULL) || (pdata->devAddrLen > 16))
//    {
//        TESAM_FMT_DEBUG("esam_info_Get_and_Act sndMsg[%p] sndMsg[%p]err\n", 
//            pdata->sndMsg, pdata->rcvMsg);
//        return ERR_PTR;
//    }
//
//    memset(&sndData, 0, sizeof(sndData));
//    memset(&rcvData, 0, sizeof(rcvData));
//
//    sndData.dataType = 1;
//    sndData.appType = 1;
//    sndData.policy = policy;
//    sndData.data = pdata->sndMsg;
//    sndData.lenData = pdata->sndLen;
//    memcpy(sndData.devAddr, pdata->devAddr, pdata->devAddrLen);
//    memcpy(sndData.dstIp, pdata->dstIp, sizeof(pdata->dstIp));
//    get_local_time(sndData.time);
//
//    rcvData.data = pdata->rcvMsg;
//
//    ret = sec_app_send_recv_data(&sndData, &rcvData);
//    if (ret != ERR_OK)
//    {
//        TESAM_FMT_DEBUG("sec_app_encryption err ret[%d]\n", ret);
//        return ERR_IFACE;
//    }
//
//    pdata->rcvLen = rcvData.lenData;
//
//    return ERR_OK;
//}
//
///**********************************************************************
//* @brief     ：集抄APP接收到下行网关报文时,把报文数据交给安全管理APP处理,解密
//* @param[in] ：policy     - 处理策略
//               sndMsg    - 输入报文
//               sndLen    - 长度
//* @param[out]：rcvMsg     - 输出解析后报文
//               rcvLen    - 报文长度
//* @return    ：错误码
//**********************************************************************/
//int sec_app_decryption(uint8 policy, SEC_APP_DATA_T *pdata)
//{
//    int ret = ERR_OK;
//    SEC_DATA_T sndData;
//    SEC_DATA_T rcvData;
//
//    if ((pdata == NULL) || (pdata->sndMsg == NULL) || (pdata->rcvMsg == NULL) || (pdata->devAddrLen > 16))
//    {
//        TESAM_FMT_DEBUG("esam_info_Get_and_Act sndMsg[%p] sndMsg[%p]err\n", 
//            pdata->sndMsg, pdata->rcvMsg);
//        return ERR_PTR;
//    }
//
//    memset(&sndData, 0, sizeof(sndData));
//    memset(&rcvData, 0, sizeof(rcvData));
//
//    sndData.dataType = 2;
//    sndData.policy = policy;
//    sndData.data = pdata->sndMsg;
//    sndData.lenData = pdata->sndLen;
//    memcpy(sndData.devAddr, pdata->devAddr, pdata->devAddrLen);
//    memcpy(sndData.dstIp, pdata->dstIp, sizeof(pdata->dstIp));
//    get_local_time(sndData.time);
//
//    rcvData.data = pdata->rcvMsg;
//
//    ret = sec_app_send_recv_data(&sndData, &rcvData);
//    if (ret != ERR_OK)
//    {
//        TESAM_FMT_DEBUG("sec_app_decryption err ret[%d]\n", ret);
//        return ERR_IFACE;
//    }
//
//    pdata->rcvLen = rcvData.lenData;
//
//    return ERR_OK;
//}

/**********************************************************************
* @brief     ：集抄APP收到SSAL给安全加固APP
* @param[in] ：policy     - 处理策略
               sndMsg    - 输入报文
               sndLen    - 长度
* @param[out]：rcvMsg     - 输出解析后报文
               rcvLen    - 报文长度
* @return    ：错误码
**********************************************************************/
int sec_ssal_en_decryption(SEC_DATA_T *sndData, SEC_DATA_T *rcvData)
{
    return sec_app_send_recv_data(sndData, rcvData);
}

#endif

