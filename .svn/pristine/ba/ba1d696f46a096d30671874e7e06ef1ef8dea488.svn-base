/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#include "ac_sample.h"
#include "ac_lib.h"
#include "ac_main.h"
#include "ac_commdrv.h"
#include "ac_mqtt.h"
#include "ac_save.h"
#include "ac_698msg.h"
#include "ac_report.h"
#include "ac_lib.h"
#include "ac_sample698.h"

//交采模块通配地址
uint8 gAddr[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

//报文帧序列号
uint8           g_AcPiid = 0;   

//在最大值约束下增加值
#define AC_ADD_VALUE_CLEAR(count, mcount, value) ((count) + (value) >= (mcount) ? 0 : (count) + (value))


/**********************************************************************
* @name：      make_connect_to_apdu
* @brief：     组织应用连接报文
* @param[in] ：pResponse - 源数据
*              maxLen    - 输出缓存最大长度
* @param[out]：sendbuf   - 输出报文
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint32 make_connect_to_apdu(CONNECT_REQUEST_T* pResponse, uint8* sendbuf, uint32 maxLen)
{
    uint8* pApdu = sendbuf;
    uint32 offset = 0;

    if ((sendbuf == NULL) || (pResponse == NULL))
    {
        ACMSG_FMT_DEBUG("make_connect_to_apdu err pResponse[%p]. sendbuf[%p]\n", pResponse, sendbuf);
        return 0;
    }

    g_AcPiid = ACSAMP_ADD_VALUE_CLEAR(g_AcPiid, 63, 1);

    pApdu[offset++] = 0x02;
    pApdu[offset++] = g_AcPiid;

    memcpy_r(&pApdu[offset], &pResponse->version, sizeof(pResponse->version));
    offset += sizeof(pResponse->version);

    memcpy(&pApdu[offset], &pResponse->prtlConf[0], sizeof(pResponse->prtlConf));
    offset += sizeof(pResponse->prtlConf);

    memcpy(&pApdu[offset], &pResponse->funcConf[0], sizeof(pResponse->funcConf));
    offset += sizeof(pResponse->funcConf);

    memcpy_r(&pApdu[offset], &pResponse->clientSFML, sizeof(pResponse->clientSFML));
    offset += sizeof(pResponse->clientSFML);

    memcpy_r(&pApdu[offset], &pResponse->clientRFML, sizeof(pResponse->clientRFML));
    offset += sizeof(pResponse->clientRFML);

    memcpy_r(&pApdu[offset], &pResponse->clientWS, sizeof(pResponse->clientWS));
    offset += sizeof(pResponse->clientWS);

    memcpy_r(&pApdu[offset], &pResponse->clientPFML, sizeof(pResponse->clientPFML));
    offset += sizeof(pResponse->clientPFML);

    memcpy_r(&pApdu[offset], &pResponse->timeOut, sizeof(pResponse->timeOut));
    offset += sizeof(pResponse->timeOut);

    if (pResponse->pMechains == NULL)
    {
        pApdu[offset++] = 0x00;
    }
    else
    {
        //暂不需要，后续有需要补充
    }

    pApdu[offset++] = 0x00;

    //防止缓存溢出
    if (offset > maxLen)
    {
        ACMSG_FMT_DEBUG("mmake_connect_to_apdu err. offset[%d]. maxLen[%d]\n",
            offset, maxLen);
        return 0;
    }

    return offset;
}

/**********************************************************************
* @name：      ac_connect_package
* @brief：     组织应用连接数据
* @param[in] ：pResponse - 源数据
*              bufLenMax - 最大长度
* @param[out]：sendbuf   - 报文
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_connect_package(CONNECT_REQUEST_T* pResponse, uint8* sendbuf, uint32 bufLenMax)
{
    uint8        apdu[SEND_APDU_LEN] = { 0 };
    uint32       outMsgLen = 0;
    uint32       apduLen = 0;

    if ((sendbuf == NULL) || (pResponse == NULL))
    {
        ACMSG_FMT_DEBUG("ac_connect_package err pResponse[%p]. sendbuf[%p]\n", pResponse, sendbuf);
        return -1;
    }

    apduLen = make_connect_to_apdu(pResponse, apdu, SEND_APDU_LEN);
    if (apduLen == 0)
    {
        return -2;
    }

    outMsgLen = make_apdu_to_package(apdu, apduLen, sendbuf, bufLenMax, 0);
    if (outMsgLen == 0)
    {
        return -3;
    }

    return outMsgLen;
}

/**********************************************************************
* @name：      make_get_frame_to_apdu
* @brief：     组织读该数据类型报文
* @param[in] ：oad     - 数据类型
*              maxLen  - 输出缓存最大长度
* @param[out]：sendbuf - 输出报文
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint32 make_get_frame_to_apdu(OOP_OAD_U oad, uint8* sendbuf, uint32 maxLen)
{
    uint8* pApdu = sendbuf;
    uint32 offset = 0;

    if (sendbuf == NULL)
    {
        ACMSG_FMT_DEBUG("make frame err oad[%d]. sendbuf[%p]\n", oad.value, sendbuf);
        return 0;
    }

    g_AcPiid = ACSAMP_ADD_VALUE_CLEAR(g_AcPiid, 63, 1);

    pApdu[offset++] = 0x05;
    pApdu[offset++] = 0x01;
    pApdu[offset++] = g_AcPiid;

    memcpy_r(&pApdu[offset], &oad.value, sizeof(oad.value));

    //防止缓存溢出
    if (5 + offset > maxLen)
    {
        ACMSG_FMT_DEBUG("make frame err. offset[%d]. maxLen[%d]\n",
            offset, maxLen);
        return 0;
    }

    offset += 4;
    pApdu[offset++] = 0x00;

    return offset;
}

/**********************************************************************
* @name：      ac_get_msg_package
* @brief：     组织数据
* @param[in] ：oad       - 数据类型
*              bufLenMax - 最大长度
* @param[out]：sendbuf   - 报文
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_get_msg_package(OOP_OAD_U oad, uint8* sendbuf, uint32 bufLenMax, uint8 logic)
{
    uint8        apdu[SEND_APDU_LEN] = { 0 };
    uint32       outMsgLen = 0;
    uint32       apduLen = 0;

    if (sendbuf == NULL)
    {
        return -1;
    }

    apduLen = make_get_frame_to_apdu(oad, apdu, SEND_APDU_LEN);
    if (apduLen == 0)
    {
        return -2;
    }

    outMsgLen = make_apdu_to_package(apdu, apduLen, sendbuf, bufLenMax, logic);
    if (outMsgLen == 0)
    {
        return -3;
    }

    return outMsgLen;
}

/**********************************************************************
* @name：      make_act_frame_to_apdu
* @brief：     组织操作该数据类型报文
* @param[in] ：oad     - 数据类型
*              maxLen  - 输出缓存最大长度
* @param[out]：sendbuf - 输出报文
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint32 make_act_frame_to_apdu(OOP_OMD_U omd, uint8* inData, uint32 inLen, uint8* outData, uint32 maxLen)
{
    uint8* pApdu = outData;
    uint32 offset = 0;

    if ((inData == NULL) || (outData == NULL))
    {
        ACMSG_FMT_DEBUG("make frame err omd[%d]. inData[%p] outData[%p]\n", 
            omd.value, inData, outData);
        return 0;
    }

    g_AcPiid = ACSAMP_ADD_VALUE_CLEAR(g_AcPiid, 63, 1);

    pApdu[offset++] = 0x07;
    pApdu[offset++] = 0x01;
    pApdu[offset++] = g_AcPiid;

    //防止缓存溢出
    if (5 + inLen + offset > maxLen)
    {
        ACMSG_FMT_DEBUG("make frame err. offset[%d]. maxLen[%d]\n",
            offset, maxLen);
        return 0;
    }

    memcpy_r(&pApdu[offset], &omd.value, sizeof(omd.value));
    offset += sizeof(omd.value);

    memcpy(&pApdu[offset], inData, inLen);
    offset += inLen;

    pApdu[offset++] = 0x00;

    return offset;
}

/**********************************************************************
* @name：      ac_get_msg_package
* @brief：     组织数据
* @param[in] ：oad       - 数据类型
*              inData    - 输入数据
*              inLen     - 数据长度
*              bufLenMax - 最大长度
* @param[out]：sendbuf   - 报文
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int ac_act_msg_package(OOP_OMD_U omd, uint8* inData, uint32 inLen, uint8* sendbuf, uint32 bufLenMax, uint8 logic)
{
    uint8        apdu[SEND_APDU_LEN] = { 0 };
    uint32       outMsgLen = 0;
    uint32       apduLen = 0;

    if (sendbuf == NULL)
    {
        return -1;
    }

    apduLen = make_act_frame_to_apdu(omd, inData, inLen, apdu, SEND_APDU_LEN);
    if (apduLen == 0)
    {
        return -2;
    }

    outMsgLen = make_apdu_to_package(apdu, apduLen, sendbuf, bufLenMax, logic);
    if (outMsgLen == 0)
    {
        return -3;
    }

    return outMsgLen;
}

/**
*********************************************************************
* @name：      make_698msg_ac_set
* @brief：     组织设置报文
* @param[in] ：
* @param[out]：outData     输出报文
* @return    ：错误码
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
int make_698msg_ac_set(OOP_OAD_U oad, uint8 *inData, uint32 inLen, uint8 *outMsg, uint32 *outLen)
{
    int           offset = 0;
    FRAME_HEAD_T  tFrame;
    OAD_INFO_T    pstOadInfo = {0};
    OOP_DAR_E     dar = DATA_SUCCESS;
    RESULT_DATA_T outData = {0};
    uint32        sendLen = 0;

    if ((outMsg == NULL) || (inData == NULL))
    {
        ACMSG_FMT_DEBUG("outMsg or inData nill\n");
        return ERR_PTR;
    }

    if (!class_oadinfo_get(oad, &pstOadInfo))
    {
        ACMSG_FMT_TRACE("class_oadinfo_get err! oad[0x%08x]\n", oad.value);
        return ERR_NORMAL;
    }

    dar = get_convert_by_oad(inData, inLen, &pstOadInfo, &outData, 1024, FALSE);
    if (dar != DATA_SUCCESS)
    {
        ACMSG_BUF_DEBUG(inData, inLen, "get_convert_by_oad err dar[%d], indata:", dar);
        return ERR_NORMAL;
    }

    //去掉第一个数据0x01
    memmove(&outData.pResultData[7], &outData.pResultData[0], outData.resultDataLen);

    //填服务类型和PIID
    g_AcPiid = AC_ADD_VALUE_CLEAR(g_AcPiid, 63, 1);
    outData.pResultData[offset ++] = 0x06;
    outData.pResultData[offset ++] = 0x01;
    outData.pResultData[offset ++] = g_AcPiid;

    //填OAD
    memcpy_r(&outData.pResultData[offset], &oad.value, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    //补充剩余长度
    offset += outData.resultDataLen;

    //无时间标签
    outData.pResultData[offset++] = 0x00;

    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    tFrame.dir = 0;
    tFrame.prm = 1;
    tFrame.funCode = 3;
    tFrame.apduLen = offset;
    tFrame.pApdu = outData.pResultData;

    //填写通配地址
    tFrame.sAddr.len = 6;
    memcpy(tFrame.sAddr.addr, gAddr, 6);
    tFrame.sAddr.type = 1;
    tFrame.sAddr.logicAddr = 1;

    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

    sendLen = app_frame_to_package(&tFrame, outMsg, APP_RECV_MSG_MAXLEN - 2, TRUE);

    *outLen = sendLen;

    return ERR_OK;
}


/**
*********************************************************************
* @name：      make_698msg_ac_date_time
* @brief：     组织对时报文
* @param[in] ：
* @param[out]：outData     输出报文
* @return    ：错误码
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
int make_698msg_ac_date_time(uint8 *outMsg, uint32 *outLen)
{
    time_t           linuxTime = time(NULL);
    DateTimeHex_t    dateTime = {0};
    OOP_DATETIME_S_T data = {0};
    OOP_OAD_U        oad = {0};

    if (outMsg == NULL)
    {
        ACMSG_FMT_DEBUG("outMsg nill\n");
        return ERR_PTR;
    }

    oad.value = 0x40000200;

    DT_Time2DateTimeHex(linuxTime, &dateTime);
    memcpy(&data, &dateTime, sizeof(DateTimeHex_t));

    //OOP_DATETIME_S_T的年高底位反转
    memrev((uint8*)&data, 2);  

    return make_698msg_ac_set(oad, (uint8 *)&data, sizeof(OOP_DATETIME_S_T), outMsg, outLen);
}

/*
*********************************************************************
* @name      : app698msg_revsnd_data_unit
* @brief     ：与交采模块交互的同步接口
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int app698msg_revsnd_data_unit(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint32 *rcvLen)
{
    int ret = ERR_NORMAL;
    int i = 1200;
    int sendLen = 0;

    if ((sndData == NULL) || (pOutBuf == NULL) || (rcvLen == NULL))
    {
        return ERR_OPTION;
    }

    ret = PlcWriteQueue(pTimeRecvDown, sndData, sndLen);
    if (ret == 0)
    {
        ACMSG_BUF_DEBUG(sndData, sndLen, "udp send queue full. outMsg:");
    }
    else
    {
        ACMSG_BUF_DEBUG(sndData, sndLen, "send jc module:");
    }

    while (i--)
    {
        sendLen = PlcReadQueue(pTimeSendDown, pOutBuf);
        if (sendLen > 0)
        {
            ACMSG_BUF_DEBUG(pOutBuf, sendLen, "recv jc module:");
            break;
        }
        usleep(5000);
    }

    if (i <= 0)
    {
        return ERR_TIMEOUT;
    }

    *rcvLen = sendLen;

    return ERR_OK;
}

/*
*********************************************************************
* @name      : app698msg_revsnd_data
* @brief     ：与交采模块交互的同步接口
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int app698msg_revsnd_data(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint32 *rcvLen)
{
    uint32 i = 0;
    int8   ret = ERR_NORMAL;

    while (i < 2)
    {
        ret = app698msg_revsnd_data_unit(sndData, sndLen, pOutBuf, rcvLen);
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

/*
*********************************************************************
* @name      : app698msg_time_set
* @brief     ：设置时间
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
OOP_DAR_E app698msg_time_set(void)
{
    int    ret = 0;
    uint32 sndLen = 0;
    uint8  sndData[SEND_BUF_LEN] = {0};
    uint32 recvLen = 0;
    uint8  recvBuf[RECV_BUF_LEN] = {0};
    FRAME_HEAD_T tFrame = {0};
    uint8  *ptr = NULL;
    uint32  outLen = 0;
    uint32  offset = 0;
    uint8   recvBufTmp[RECV_BUF_LEN] = {0};

    ret = make_698msg_ac_date_time(sndData, &sndLen);
    if (ret != ERR_OK)
    {
        ACMSG_BUF_DEBUG(sndData, sndLen, "make_698msg_ac_date_time err ret[%d] sndData:", ret);
        return DATA_OTHER_REASON;
    }

    ret = app698msg_revsnd_data(sndData, sndLen, recvBuf, &recvLen);
    if (ret != ERR_OK)
    {
        ACMSG_BUF_DEBUG(recvBuf, recvLen, "app698msg_revsnd_data err ret[%d] recvBuf:", ret);
        return DATA_OTHER_REASON;
    }

    while ((ptr = get_68ptr_from_buf(recvBuf, recvLen, &offset, &outLen)) != NULL)
    {
        memcpy(recvBufTmp, ptr, outLen);

        recvLen -= (offset + outLen);
        memmove(recvBuf, (ptr + outLen), recvLen);

        /* 合法性检查 */
        ret = app_check_message(recvBufTmp, outLen);
        if (0 != ret)
        {
            ACMSG_BUF_DEBUG(recvBufTmp, outLen, "app_check_message error[%d] msg recv", ret);
            continue;
        }

        /* 解析帧头 */
        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
        ret = app_get_frame(recvBufTmp, outLen, &tFrame, TRUE);
        if (0 != ret)
        {
            ACMSG_BUF_DEBUG(recvBufTmp, outLen, "app_get_frame error[%d] msg recv", ret);
            continue;
        }

        if (tFrame.pApdu[7] != DATA_SUCCESS)
        {
            return tFrame.pApdu[7];
        }
    }

    return DATA_SUCCESS;
}

/*******************************************************************************
* 函数名称: ac_act_frame_factory_reset
* 函数功能: 恢复出厂
* 输入参数:
* 输出参数: sendbuf - 发送报文
* 返 回 值: 大于 0    发送长度
*           小于 0     失败
*******************************************************************************/
int ac_act_frame_factory_reset(uint8* sendbuf, uint32 maxLen)
{
    OOP_OMD_U   omd = {0};
    uint8       inData[256] = {0};
    uint32      inLen = 0;
    uint32      offset = 0;

    omd.value = 0x43000400;

    inData[offset++] = 0;

    inLen = offset;

    return ac_act_msg_package(omd, inData, inLen, sendbuf, maxLen, 0);
}

/*******************************************************************************
* 函数名称: ac_act_frame_power_off
* 函数功能: 电源掉电
* 输入参数:
* 输出参数: sendbuf - 发送报文
* 返 回 值: 大于 0    发送长度
*           小于 0     失败
*******************************************************************************/
int ac_act_frame_power_off(uint8* sendbuf, uint32 maxLen)
{
    OOP_OMD_U   omd = {0};
    uint8       inData[256] = {0};
    uint32      inLen = 0;
    uint32      offset = 0;

    omd.value = 0xfff67f00;

    //通知掉电
    inData[offset++] = DT_ENUM;
    inData[offset++] = 0;

    inLen = offset;

    return ac_act_msg_package(omd, inData, inLen, sendbuf, maxLen, 0);
}

/**********************************************************************
* @name：      ac_act_parser_factory_reset
* @brief：     数据解析
* @param[in] ：recvBuf - 报文输入
*              recvLen - 报文长度
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
int ac_act_parser_factory_reset(uint8* recvBuf, uint32 recvLen)
{
    int    ret = -1;
    FRAME_HEAD_T tFrame = {0};
    uint8* ptr = NULL;
    uint32  outLen = 0;
    uint32  offset = 0;
    uint8   recvBufTmp[RECV_BUF_LEN] = { 0 };

    while ((ptr = get_68ptr_from_buf(recvBuf, recvLen, &offset, &outLen)) != NULL)
    {
        memcpy(recvBufTmp, ptr, outLen);

        recvLen -= (offset + outLen);
        memmove(recvBuf, (ptr + outLen), recvLen);

        /* 合法性检查 */
        ret = app_check_message(recvBufTmp, outLen);
        if (0 != ret)
        {
            ACMSG_BUF_DEBUG(recvBufTmp, outLen, "app_check_message error[%d] msg recv", ret);
            continue;
        }

        /* 解析帧头 */
        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
        ret = app_get_frame(recvBufTmp, outLen, &tFrame, TRUE);
        if (0 != ret)
        {
            ACMSG_BUF_DEBUG(recvBufTmp, outLen, "app_get_frame error[%d] msg recv", ret);
            continue;
        }

        if (tFrame.pApdu[7] != DATA_SUCCESS)
        {
            return tFrame.pApdu[7];
        }
        else
        {
            break;
        }
    }

    if (ptr == NULL)
    {
        return ERR_ITEM;
    }

    return ERR_OK;
}

/**********************************************************************
* @name：       make_rpt_to_apdu
* @brief：      组织上报报文
* @param[in] ：isRec       源数据(1 - 普通数据, 2 - 记录数据)
*              piid       输出缓存最大长度
               oad        数据项
               lenMax     输出最大长度
* @param[out]：sendbuf   - 输出报文
* @return    ：长度或者错误码
* @Date      ：2022-08-03
**********************************************************************/
uint32 make_rpt_to_apdu(uint8 isRec, uint8 piid, OOP_OAD_U oad, uint8 *pSend, uint32 lenMax)
{
    uint32 offset = 0;

    if ((pSend == NULL) || (lenMax < 9))
    {
        return ERR_PTR;
    }

    pSend[offset++] = 0x08;
    pSend[offset++] = isRec;
    pSend[offset++] = piid;
    pSend[offset++] = 0x01;
    memcpy_r(&pSend[offset], &oad.value, sizeof(oad.value));
    offset += sizeof(oad.value);
    pSend[offset++] = 0x00;

    return offset;
}

/**********************************************************************
* @name：      make_rpt_apdu_to_package
* @brief：     组织完整698报文
* @param[in] ：apdu    - apdu
               apduLen - apdu长度
               maxLen  - 698报文最大长度
* @param[out]：outMsg  - 698报文
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint32 make_rpt_apdu_to_package(uint8 *Addr, uint8 *apdu, uint32 apduLen, uint8 *outMsg, uint32 maxLen)
{
    uint32       SendLen = 0;
    FRAME_HEAD_T tFrame = {0};

    if ((Addr == NULL) || (outMsg == NULL))
    {
        return 0;
    }

    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    tFrame.dir = 0;
    tFrame.prm = 0;
    tFrame.funCode = 3;
    tFrame.apduLen = apduLen;
    tFrame.pApdu = apdu;

    //填写地址
    tFrame.sAddr.len = 6;
    memcpy(tFrame.sAddr.addr, Addr, 6);
    tFrame.sAddr.type = 0;
    tFrame.sAddr.logicAddr = 0;

    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

    SendLen = app_frame_to_package(&tFrame, outMsg, SEND_APDU_LEN - 2, TRUE);
    if ((SendLen > maxLen) || (SendLen == 0))
    {
        ACSAMP_FMT_DEBUG("make_report_apdu_to_package err SendLen[%d]. maxLen[%d]\n", SendLen, maxLen);
        return 0;
    }

    return SendLen;
}

/**********************************************************************
* @name：       make_rpt_to_apdu
* @brief：      组织上报报文
* @param[in] ：isRec       源数据(1 - 普通数据, 2 - 记录数据)
*              piid       输出缓存最大长度
               oad        数据项
               lenMax     输出最大长度
* @param[out]：sendbuf   - 输出报文
* @return    ：长度或者错误码
* @Date      ：2022-08-03
**********************************************************************/
int ac_rpt_msg_package(uint8 *addr, uint8 isRec, uint8 piid, OOP_OAD_U oad, uint8 *pSend, uint32 lenMax)
{
    uint8        apdu[SEND_APDU_LEN] = { 0 };
    uint32       outMsgLen = 0;
    uint32       apduLen = 0;

    if (pSend == NULL)
    {
        return -1;
    }

    apduLen = make_rpt_to_apdu(isRec, piid, oad, apdu, SEND_APDU_LEN);
    if (apduLen <= 0)
    {
        return -2;
    }

    outMsgLen = make_rpt_apdu_to_package(addr, apdu, apduLen, pSend, lenMax);
    if (outMsgLen == 0)
    {
        return -3;
    }

    return outMsgLen;
}

