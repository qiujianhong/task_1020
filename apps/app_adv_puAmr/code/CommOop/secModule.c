/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 安全软算法模块
* @date：   2022-05-13
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
#include "appcrc16.h"
#include "secModule.h"
#include "appcomm.h"
#include "light_sm.h"
#include "sm4Callback.h"

#if DESC("内部使用接口", 1)
/*
*********************************************************************
* @name      : get_extModule_devid_packaged
* @brief     ：封装获取扩展模块esam序列号的报文
* @param[in] ：tsa      扩展模块地址
               apdu    698报文中的数据域
               apduLen 数据域长度
* @param[out]：sendbuf  发送报文
               bufLen  报文长度
* @return    ：错误码
*********************************************************************
*/
uint32 make_apdu_to_package(OOP_TSA_T tsa, uint8 *apdu, uint32 apduLen, uint8 *outMsg, uint32 maxLen)
{
    uint32       SendLen = 0;
    FRAME_HEAD_T tFrame = {0};

    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    tFrame.dir = 0;
    tFrame.prm = 1;
    tFrame.funCode = 3;
    tFrame.apduLen = apduLen;
    tFrame.pApdu = apdu;

    //填写地址
    tFrame.sAddr.len = 6;
    memcpy(tFrame.sAddr.addr, tsa.add, 6);
    tFrame.sAddr.type = 0;
    tFrame.sAddr.logicAddr = 0;

    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

    SendLen = app_frame_to_package(&tFrame, outMsg, SEND_APDU_LEN - 2, TRUE);
    if ((SendLen > maxLen) || (SendLen == 0))
    {
        SM_FMT_DEBUG("make_apdu_to_package err SendLen[%d]. maxLen[%d]\n", SendLen, maxLen);
        return 0;
    }

    return SendLen;
}

/*
*********************************************************************
* @name：      make_act_frame_to_apdu
* @brief：     组织操作该数据类型报文
* @param[in] ：omd     -  数据类型
               inData    
*              maxLen  - 输出缓存最大长度
* @param[out]：sendbuf - 输出报文
* @return    ：
* @Date      ：2019-12-6
*********************************************************************
*/
uint32 make_act_frame_to_apdu(OOP_OMD_U omd, uint8* inData, uint32 inLen, uint8* outData, uint32 maxLen)
{
    uint8* pApdu = outData;
    uint32 offset = 0;

    if ((inData == NULL) || (outData == NULL))
    {
        SM_FMT_DEBUG("make frame err omd[%d]. inData[%p] outData[%p]\n", 
            omd.value, inData, outData);
        return 0;
    }

    pApdu[offset++] = 0x07;
    pApdu[offset++] = 0x01;
    pApdu[offset++] = 0x00;

    //防止缓存溢出
    if (5 + inLen + offset > maxLen)
    {
        SM_FMT_DEBUG("make frame err. offset[%d]. maxLen[%d]\n",
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

/*
*********************************************************************
* @name：      sm_get_msg_package
* @brief：     组织数据
* @param[in] ：oad       - 数据类型
*              inData    - 输入数据
*              inLen     - 数据长度
*              bufLenMax - 最大长度
* @param[out]：sendbuf   - 报文
* @return    ：
* @Date      ：2019-12-6
*********************************************************************
*/
int sm_act_msg_package(OOP_TSA_T tsa, OOP_OMD_U omd, uint8* inData, uint32 inLen, uint8* sendbuf, uint32 bufLenMax)
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

    outMsgLen = make_apdu_to_package(tsa, apdu, apduLen, sendbuf, bufLenMax);
    if (outMsgLen == 0)
    {
        return -3;
    }

    return outMsgLen;
}

/**********************************************************************
* @name：       make_get_frame_to_apdu
* @brief：      组织读该数据类型报文
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
        SM_FMT_DEBUG("make frame err oad[%d]. sendbuf[%p]\n", oad.value, sendbuf);
        return 0;
    }

    pApdu[offset++] = 0x05;
    pApdu[offset++] = 0x01;
    pApdu[offset++] = 0x00;

    memcpy_r(&pApdu[offset], &oad.value, sizeof(oad.value));

    //防止缓存溢出
    if (5 + offset > maxLen)
    {
        SM_FMT_DEBUG("make frame err. offset[%d]. maxLen[%d]\n",
            offset, maxLen);
        return 0;
    }

    offset += 4;
    pApdu[offset++] = 0x00;

    return offset;
}

/**********************************************************************
* @name：       make_get_list_frame_to_apdu
* @brief：      组织读该数据类型报文
* @param[in] ：oad      - 数据
               oad2    - 数据
*              maxLen  - 输出缓存最大长度
* @param[out]：sendbuf - 输出报文
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint32 make_get_list_frame_to_apdu(OOP_OAD_U oad, OOP_OAD_U oad2, uint8* sendbuf, uint32 maxLen)
{
    uint8* pApdu = sendbuf;
    uint32 offset = 0;

    if (sendbuf == NULL)
    {
        SM_FMT_DEBUG("make frame err oad[%d]. sendbuf[%p]\n", oad.value, sendbuf);
        return 0;
    }

    pApdu[offset++] = 0x05;
    pApdu[offset++] = 0x02;
    pApdu[offset++] = 0x00;
    pApdu[offset++] = 0x02;

    memcpy_r(&pApdu[offset], &oad.value, sizeof(oad.value));
    offset += 4;
    
    memcpy_r(&pApdu[offset], &oad2.value, sizeof(oad2.value));

    //防止缓存溢出
    if (5 + offset > maxLen)
    {
        SM_FMT_DEBUG("make frame err. offset[%d]. maxLen[%d]\n",
            offset, maxLen);
        return 0;
    }

    offset += 4;
    pApdu[offset++] = 0x00;

    return offset;
}

/**********************************************************************
* @name：      sm_get_msg_package
* @brief：     组织数据
* @param[in] ：oad       - 数据类型
*              bufLenMax - 最大长度
* @param[out]：sendbuf   - 报文
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int sm_get_msg_package(OOP_TSA_T tsa, OOP_OAD_U oad, uint8* sendbuf, uint32 bufLenMax)
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

    outMsgLen = make_apdu_to_package(tsa, apdu, apduLen, sendbuf, bufLenMax);
    if (outMsgLen == 0)
    {
        return -3;
    }

    return outMsgLen;
}

/**********************************************************************
* @name：      sm_get_list_msg_package
* @brief：     组织数据
* @param[in] ：oad       - 数据类型
               oad2      - 数据类型
*              bufLenMax - 最大长度
* @param[out]：sendbuf   - 报文
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
int sm_get_list_msg_package(OOP_TSA_T tsa, OOP_OAD_U oad, OOP_OAD_U oad2,uint8* sendbuf, uint32 bufLenMax)
{
    uint8        apdu[SEND_APDU_LEN] = { 0 };
    uint32       outMsgLen = 0;
    uint32       apduLen = 0;

    if (sendbuf == NULL)
    {
        return -1;
    }

    apduLen = make_get_list_frame_to_apdu(oad, oad2, apdu, SEND_APDU_LEN);
    if (apduLen == 0)
    {
        return -2;
    }

    outMsgLen = make_apdu_to_package(tsa, apdu, apduLen, sendbuf, bufLenMax);
    if (outMsgLen == 0)
    {
        return -3;
    }

    return outMsgLen;
}

#endif

#if DESC("遥信获取", 1)
/*
*********************************************************************
* @name      : get_extModule_yx_st_packaged
* @brief     ：封装获取扩展模块遥信的报文
* @param[in] ：tsa      扩展模块地址
* @param[out]：sendbuf  发送报文
               bufLen  报文长度
* @return    ：错误码
*********************************************************************
*/
int get_extModule_yx_st_fg_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen)
{
    int32     ret = 0;
    OOP_OAD_U oad = { 0 };
    OOP_OAD_U oad2 = { 0 };

    if ((sendbuf == NULL) || (buflen == NULL))
    {
        return ERR_PTR;
    }

    oad.value = 0xf2030200;
    oad2.value = 0xf2030400;

    ret = sm_get_list_msg_package(tsa, oad, oad2, sendbuf, SEND_BUF_LEN);
    if (ret <= 0)
    {
        SM_FMT_DEBUG("读操作组码错误 oad[0x%08x]\n", oad.value);
        return ret;
    }

    *buflen = ret;
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : get_extModule_yx_st_packaged
* @brief     ：封装获取扩展模块遥信的报文
* @param[in] ：tsa      扩展模块地址
* @param[out]：sendbuf  发送报文
               bufLen  报文长度
* @return    ：错误码
*********************************************************************
*/
int get_extModule_yx_st_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen)
{
    int32     ret = 0;
    OOP_OAD_U oad = { 0 };

    if ((sendbuf == NULL) || (buflen == NULL))
    {
        return ERR_PTR;
    }

    oad.value = 0xf2030200;

    ret = sm_get_msg_package(tsa, oad, sendbuf, SEND_BUF_LEN);
    if (ret <= 0)
    {
        SM_FMT_DEBUG("读操作组码错误 oad[0x%08x]\n", oad.value);
        return ret;
    }

    *buflen = ret;
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : get_extModule_yx_st_parse
* @brief     ：解析获取扩展模块遥信的报文
* @param[in] ：rcvbuf  待解析报文
               buflen  待解析报文报文长度
* @param[out]：devid    设备指纹(esam序列号)
               idlen   设备指纹长度
* @return    ：错误码
*********************************************************************
*/
int get_extModule_yx_st_parse(uint8 *rcvbuf, uint32 buflen, OOP_SWITCHIN_STATE_T *st)
{
    int          i = 0;
    int          ret = 0;
    uint16       offset = 0;
    uint16       num = 0;
    FRAME_HEAD_T tFrame = {0};
    
    if ((rcvbuf == NULL) || (st == NULL))
    {
        return ERR_PTR;
    }

    if ((rcvbuf[0] != 0x68) && (rcvbuf[buflen - 1] != 0x16))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_OPTION;
    }

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(rcvbuf, buflen, &tFrame, TRUE);
    if (0 != ret)
    {
        ACSAMP_BUF_DEBUG(rcvbuf, buflen, "输入报文有误 错误码[%d]:", ret);
        return ERR_OPTION;
    }

    if ((tFrame.pApdu[0] != 0x85) ||
        (tFrame.pApdu[1] != 0x01) ||
        (tFrame.pApdu[3] != 0xf2) ||
        (tFrame.pApdu[4] != 0x03) ||
        (tFrame.pApdu[5] != 0x02) ||
        (tFrame.pApdu[6] != 0x00) ||
        (tFrame.pApdu[8] != 0x01))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_ITEM;
    }

    offset = 0x09;
    num = tFrame.pApdu[offset++];
    if (num == 0x81)
    {
        num = tFrame.pApdu[offset++];
    }
    else if (num == 0x82)
    {
        num = tFrame.pApdu[offset++];
        num <<= 8;
        num += tFrame.pApdu[offset++];
    }
    else if (num > 0x82 || num == 0x80)
    {
        return ERR_TYPE;
    }

    if (num > OOP_MAX_SWITCHIN)
    {
        return ERR_O_RANGE;
    }

    st->nNum = num;
    for (i = 0; i < num; i++)
    {
        st->state[i].st = tFrame.pApdu[offset++];
        st->state[i].cd = tFrame.pApdu[offset++];
    }
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : get_extModule_yx_fg_packaged
* @brief     ：封装获取扩展模块遥信配置的报文
* @param[in] ：tsa      扩展模块地址
* @param[out]：sendbuf  发送报文
               bufLen  报文长度
* @return    ：错误码
*********************************************************************
*/
int get_extModule_yx_fg_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen)
{
    int32     ret = 0;
    OOP_OAD_U oad = { 0 };

    if ((sendbuf == NULL) || (buflen == NULL))
    {
        return ERR_PTR;
    }

    oad.value = 0xf2030400;

    ret = sm_get_msg_package(tsa, oad, sendbuf, SEND_BUF_LEN);
    if (ret <= 0)
    {
        SM_FMT_DEBUG("读操作组码错误 oad[0x%08x]\n", oad.value);
        return ret;
    }

    *buflen = ret;
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : get_extModule_yx_fg_parse
* @brief     ：解析获取扩展模块遥信配置的报文
* @param[in] ：rcvbuf  待解析报文
               buflen  待解析报文报文长度
* @param[out]：devid    设备指纹(esam序列号)
               idlen   设备指纹长度
* @return    ：错误码
*********************************************************************
*/
int get_extModule_yx_fg_parse(uint8 *rcvbuf, uint32 buflen, OOP_SWITCHIN_FLAG_T *fg)
{
    int          ret = 0;
    uint32       sOffset = 0;
    FRAME_HEAD_T tFrame = {0};
    OOP_DAR_E    dar = DATA_SUCCESS;
    uint8       *srcBuf = tFrame.pApdu;
    uint32       srcLen = tFrame.apduLen;
    
    if ((rcvbuf == NULL) || (fg == NULL))
    {
        return ERR_PTR;
    }

    if ((rcvbuf[0] != 0x68) && (rcvbuf[buflen - 1] != 0x16))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_OPTION;
    }

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(rcvbuf, buflen, &tFrame, TRUE);
    if (0 != ret)
    {
        ACSAMP_BUF_DEBUG(rcvbuf, buflen, "输入报文有误 错误码[%d]:", ret);
        return ERR_OPTION;
    }

    if ((tFrame.pApdu[0] != 0x85) ||
        (tFrame.pApdu[1] != 0x01) ||
        (tFrame.pApdu[3] != 0xf2) ||
        (tFrame.pApdu[4] != 0x03) ||
        (tFrame.pApdu[5] != 0x04) ||
        (tFrame.pApdu[6] != 0x00))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_ITEM;
    }

    sOffset = 0x08;
        
    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if(srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //开关量接入标志
    dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &fg->inputFlag, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //开关量属性标志
    dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &fg->attrFlag, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   
    
    return ERR_OK;
}

#endif

#if DESC("连接准备流程", 1)
/*
*********************************************************************
* @name      : get_extModule_devid_packaged
* @brief     ：封装获取扩展模块esam序列号的报文
* @param[in] ：tsa      扩展模块地址
* @param[out]：sendbuf  发送报文
               bufLen  报文长度
* @return    ：错误码
*********************************************************************
*/
int get_extModule_devid_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen)
{
    int32     ret = 0;
    OOP_OAD_U oad = { 0 };

    if ((sendbuf == NULL) || (buflen == NULL))
    {
        return ERR_PTR;
    }

    oad.value = 0xf1000200;

    ret = sm_get_msg_package(tsa, oad, sendbuf, SEND_BUF_LEN);
    if (ret <= 0)
    {
        SM_FMT_DEBUG("读操作组码错误 oad[0x%08x]\n", oad.value);
        return ret;
    }

    *buflen = ret;
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : get_extModule_devid_parse
* @brief     ：解析获取扩展模块esam序列号的报文
* @param[in] ：rcvbuf  待解析报文
               buflen  待解析报文报文长度
* @param[out]：devid    设备指纹(esam序列号)
               idlen   设备指纹长度
* @return    ：错误码
*********************************************************************
*/
int get_extModule_devid_parse(uint8 *rcvbuf, uint32 buflen, uint8 *devid, uint32 *idlen)
{
    int          ret = 0;
    uint16       offset = 0;
    uint16       num = 0;
    FRAME_HEAD_T tFrame = {0};
    
    if ((rcvbuf == NULL) || (devid == NULL) || (idlen == NULL))
    {
        return ERR_PTR;
    }

    if ((rcvbuf[0] != 0x68) && (rcvbuf[buflen - 1] != 0x16))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_OPTION;
    }

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(rcvbuf, buflen, &tFrame, TRUE);
    if (0 != ret)
    {
        ACSAMP_BUF_DEBUG(rcvbuf, buflen, "输入报文有误 错误码[%d]:", ret);
        return ERR_OPTION;
    }

    if ((tFrame.pApdu[0] != 0x85) ||
        (tFrame.pApdu[1] != 0x01) ||
        (tFrame.pApdu[3] != 0xf1) ||
        (tFrame.pApdu[4] != 0x00) ||
        (tFrame.pApdu[5] != 0x02) ||
        (tFrame.pApdu[6] != 0x00) ||
        (tFrame.pApdu[8] != 0x09))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_ITEM;
    }

    offset = 0x09;
    num = tFrame.pApdu[offset++];
    if (num == 0x81)
    {
        num = tFrame.pApdu[offset++];
    }
    else if (num == 0x82)
    {
        num = tFrame.pApdu[offset++];
        num <<= 8;
        num += tFrame.pApdu[offset++];
    }
    else if (num > 0x82 || num == 0x80)
    {
        return ERR_TYPE;
    }

    memcpy(devid, &tFrame.pApdu[offset], num);
    *idlen = num;
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : get_esam_factory_ik
* @brief     ：通过终端esam获取出厂密钥(IK)
* @param[in] ：devid    设备指纹(esam序列号)，从扩展模块获取
               idlen   设备指纹长度
* @param[out]：ik       出厂密钥(IK)，从esam获取
               iklen   出厂密钥长度
* @return    ：错误码
*********************************************************************
*/
int get_esam_factory_ik(uint8 *devid, uint32 idlen, uint8 *ik, uint32 *iklen)
{
    uint16 offset = 0;
    uint32 len = 0;

    if ((devid  == NULL) || (ik == NULL) || (iklen == NULL))
    {
        return ERR_PTR;
    }

    if (ESAM_OK != GetTEsam_IKB_Info(devid, idlen, &ik[offset], &len))
    {
        return -1;
    }

    offset += len;

    if (ESAM_OK != GetTEsam_IKC_Info(devid, idlen, &ik[offset], &len))
    {
        return -2;
    }

    offset += len;

    if (ESAM_OK != GetTEsam_IKA_Info(devid, idlen, &ik[offset], &len))
    {
        return -2;
    }

    offset += len;

    *iklen = offset;

    return ERR_OK;
}

/*
*********************************************************************
* @name      : set_terminal_id_and_factory_ik
* @brief     ：设置终端的devid和预置出厂IK
* @param[in] ：devid    设备指纹(esam序列号)，这个devid从哪里获取，从电科院申请？
               idlen   设备指纹长度
               ik      出厂密钥(IK)，从esam获取
               iklen   出厂密钥长度
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int set_terminal_id_and_factory_ik(uint8 *devid, uint32 idlen, uint8 *ik, uint32 iklen)
{
    int32 ret  = 0;
    if (!SetID(devid, idlen))
    {
        SM_BUF_DEBUG(devid, idlen, "预置devid错误:");
        return ERR_EXECFAIL;
    }

    ret = Master_SM4SetIK(ik);
    if (ret != TRUE)
    {
        SM_BUF_DEBUG(ik, iklen, "设置公钥错误:");
        return ERR_EXECFAIL;
    }

    if (getStatus(devid, idlen) != TEST_STAT)
    {
        SM_FMT_DEBUG("终端处于私钥状态\n");
    }
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : act_extModule_init_packaged
* @brief     ：封装软加密初始化报文给扩展模块
* @param[in] ：tsa       扩展模块地址
* @param[out]：sendbuf   发送报文
               bufLen   报文长度
* @return    ：错误码
*********************************************************************
*/
int act_extModule_init_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen)
{
    int32  ret = 0;
    uint8  Pinfo[64] = {0};
    uint32 PInfolen =  sizeof(Pinfo);
    
    OOP_OMD_U omd = {0};
    uint8     inData[256] = {0};
    uint32    inLen = 0;
    uint32    offset = 0;

    ret = getConnection(Pinfo, &PInfolen);
    if (ret != TRUE)
    {
        SM_BUF_DEBUG(Pinfo, MIN(64, PInfolen), "获取连接请求数据失败 ret[%d] Pinfo", ret);
        return ERR_EXECFAIL;
    }

    omd.value = 0xf1008000;

    inData[offset ++] = 0x09;
    if (PInfolen < 0x80)
    {
        inData[offset ++] = PInfolen;
    }
    else
    {
        inData[offset++] = 0x82;
        inData[offset++] = (uint8)(PInfolen >> 8);
        inData[offset++] = (uint8)PInfolen;
    }

    memcpy(&inData[offset], Pinfo, PInfolen);
    offset += PInfolen;;

    //inData[offset ++] = 0;

    inLen = offset;

    ret = sm_act_msg_package(tsa, omd, inData, inLen, sendbuf, SEND_BUF_LEN);
    if (ret < 0)
    {
        SM_BUF_DEBUG(inData, inLen, "报文组织失败 错误码[%d]", ret);
        return ERR_NORMAL;
    }

    *buflen = ret;
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : act_extModule_init_parse
* @brief     ：解析软加密初始化给扩展模块的回码，成功还是失败
* @param[in] ：rcvbuf  待解析报文
               buflen  待解析报文报文长度
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int act_extModule_init_parse(uint8 *rcvbuf, uint32 buflen)
{
    int          ret = 0;
    FRAME_HEAD_T tFrame = {0};
    
    if (rcvbuf == NULL)
    {
        return ERR_PTR;
    }

    if ((rcvbuf[0] != 0x68) && (rcvbuf[buflen - 1] != 0x16))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_OPTION;
    }

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(rcvbuf, buflen, &tFrame, TRUE);
    if (0 != ret)
    {
        ACSAMP_BUF_DEBUG(rcvbuf, buflen, "输入报文有误 错误码[%d]:", ret);
        return ERR_OPTION;
    }

    if ((tFrame.pApdu[0] != 0x87) ||
        (tFrame.pApdu[1] != 0x01) ||
        (tFrame.pApdu[3] != 0xf1) ||
        (tFrame.pApdu[4] != 0x00) ||
        (tFrame.pApdu[5] != 0x80) ||
        (tFrame.pApdu[6] != 0x00))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_ITEM;
    }

    if (tFrame.pApdu[7] != DATA_SUCCESS)
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return tFrame.pApdu[7];
    }
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : get_extModule_keyStatus_packaged
* @brief     ：封装获取扩展模块密钥状态的报文
* @param[in] ：tsa      扩展模块地址
* @param[out]：sendbuf  发送报文
               bufLen  报文长度
* @return    ：错误码
*********************************************************************
*/
int get_extModule_keyStatus_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen)
{
    int32     ret = 0;
    OOP_OAD_U oad = { 0 };

    if ((sendbuf == NULL) || (buflen == NULL))
    {
        return ERR_PTR;
    }

    oad.value = 0xf1000400;

    ret = sm_get_msg_package(tsa, oad, sendbuf, SEND_BUF_LEN);
    if (ret <= 0)
    {
        SM_FMT_DEBUG("读操作组码错误 oad[0x%08x]\n", oad.value);
        return ret;
    }

    *buflen = ret;
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : get_extModule_keyStatus_parse
* @brief     ：解析获取扩展密钥状态的报文
* @param[in] ：rcvbuf       待解析报文
               buflen      待解析报文报文长度
* @param[out]：keyStatus    密钥状态(0x10公钥 0x11私钥)
* @return    ：错误码
*********************************************************************
*/
int get_extModule_keyStatus_parse(uint8 *rcvbuf, uint32 buflen, uint8 *keyStatus)
{
    int          ret = 0;
    uint16       offset = 0;
    uint16       num = 0;
    FRAME_HEAD_T tFrame = {0};
    
    uint8  status[128] = {0};
    uint8  keyCmp[128] = {0};
    
    if ((rcvbuf == NULL) || (keyStatus == NULL))
    {
        return ERR_PTR;
    }

    if ((rcvbuf[0] != 0x68) && (rcvbuf[buflen - 1] != 0x16))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_OPTION;
    }

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(rcvbuf, buflen, &tFrame, TRUE);
    if (0 != ret)
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误 错误码[%d]:", ret);
        return ERR_OPTION;
    }

    if ((tFrame.pApdu[0] != 0x85) ||
        (tFrame.pApdu[1] != 0x01) ||
        (tFrame.pApdu[3] != 0xf1) ||
        (tFrame.pApdu[4] != 0x00) ||
        (tFrame.pApdu[5] != 0x04) ||
        (tFrame.pApdu[6] != 0x00) ||
        (tFrame.pApdu[8] != 0x09))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_ITEM;
    }

    offset = 0x09;
    num = tFrame.pApdu[offset++];
    if (num == 0x81)
    {
        num = tFrame.pApdu[offset++];
    }
    else if (num == 0x82)
    {
        num = tFrame.pApdu[offset++];
        num <<= 8;
        num += tFrame.pApdu[offset++];
    }
    else if (num > 0x82 || num == 0x80)
    {
        return ERR_TYPE;
    }

    memcpy(status, &tFrame.pApdu[offset], num);

    memset(keyCmp, 0x00, num);
    if (memcmp(status, keyCmp, num) == 0)
    {
        *keyStatus = 0x10;
    }

    memset(keyCmp, 0xff, num);
    if (memcmp(status, keyCmp, num) == 0)
    {
        *keyStatus = 0x11;
    }
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : update_terminal_keyStatus
* @brief     ：终端根据模块的密钥状态调整自身密钥状态(双方需要相同的密钥状态，同公钥或者同私钥)
* @param[in] ：modKeyStatus    模块密钥状态(0x10公钥 0x11私钥)
               devid          设备指纹(esam序列号)，从扩展模块获取
               idlen          设备指纹长度
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int update_terminal_keyStatus(uint8 modKeyStatus, uint8 *devid, uint32 idlen)
{
    int32 ret = 0;
    uint8 termKeyStatus = 0;
    uint8  sik[128] = {0};
    uint32 sikLen = 0;

    termKeyStatus = getStatus(devid, idlen);
    if ((termKeyStatus == 0x10) && (modKeyStatus == 0x10))
    {
        SM_FMT_DEBUG("密钥状态相同，无需更新 termKeyStatus[%d], modKeyStatus[%d]\n", termKeyStatus, modKeyStatus);
        return ERR_OK;
    }
    else if ((termKeyStatus == 0x10) && (modKeyStatus == 0x11))
    {
        //获取更新密码，代码暂时缺失
        ret  = Master_SM4SetSIK(sik);
        if (ret != TRUE)
        {
            SM_BUF_DEBUG(sik, sikLen, "更新私钥失败");
            return ERR_NORESOURCE;
        }

        ret = setStatus(NORM_STAT);
        if (ret != TRUE)
        {
            SM_BUF_DEBUG(devid, idlen, "更新密钥状态失败");
            return ERR_NORESOURCE;
        }

        ret = getStatus(devid, idlen);
        if (ret != NORM_STAT)
        {
            SM_BUF_DEBUG(devid, idlen, "更新密钥状态失败");
            return ERR_NORESOURCE;
        }
    }
    return ERR_OK;
}
#endif

#if DESC("身份认证会话协商流程", 1)
/*
*********************************************************************
* @name      : negotiate_begin_packaged
* @brief     ：组织开始协商报文
* @param[in] ：tsa      扩展模块地址
* @param[out]：sendbuf  发送报文
               bufLen  报文长度
* @return    ：错误码
*********************************************************************
*/
int negotiate_begin_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen)
{
    int32 ret = 0;
    uint8 rand_a[32];

    OOP_OMD_U omd = {0};
    uint8     inData[256] = {0};
    uint32    inLen = 0;
    uint32    offset = 0;

    ivecReset(TRUE);

    memset(rand_a, 0x00, sizeof(rand_a));
    callback_getRandomWords((uint32 *)rand_a, 8);
    ret = Master_SM4UpdateBegin(rand_a);
    if (ret != TRUE) 
    {
        SM_FMT_DEBUG("协商失败，获取密文1错误[%d]\n", ret);
        return ERR_NORESOURCE;
    }
    
    omd.value = 0xf100b000;

    inData[offset ++] = DT_RN;
    inData[offset ++] = 32;

    memcpy(&inData[offset], rand_a, 32);
    offset += 32;

    //inData[offset ++] = 0;

    inLen = offset;

    ret = sm_act_msg_package(tsa, omd, inData, inLen, sendbuf, SEND_BUF_LEN);
    if (ret < 0)
    {
        SM_FMT_DEBUG("报文组织失败\n");
        return ERR_NORMAL;
    }

    *buflen = ret;
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : negotiate_begin_parse
* @brief     ：解析获取扩展模块esam序列号的报文
* @param[in] ：rcvbuf  待解析报文
               buflen  待解析报文报文长度
* @param[out]：cRanCat     来自扩展模块所返回的 48 字节密文数据
               cRanCat_len 来自扩展模块所返回的 48 字节密文数据长度。
* @return    ：错误码
*********************************************************************
*/
int negotiate_begin_parse(uint8 *rcvbuf, uint32 buflen, uint8 *cRanCat, uint32 *cRanCat_len)
{
    int          ret = 0;
    uint16       offset = 0;
    FRAME_HEAD_T tFrame = {0};

    uint8 len;
    uint8 rand_b[48];
    
    if ((rcvbuf == NULL) || (cRanCat == NULL) || (cRanCat_len == NULL))
    {
        return ERR_PTR;
    }

    if ((rcvbuf[0] != 0x68) && (rcvbuf[buflen - 1] != 0x16))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_OPTION;
    }

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(rcvbuf, buflen, &tFrame, TRUE);
    if (0 != ret)
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误 错误码[%d]:", ret);
        return ERR_OPTION;
    }

    if ((tFrame.pApdu[0] != 0x87) ||
        (tFrame.pApdu[1] != 0x01) ||
        (tFrame.pApdu[3] != 0xf1) ||
        (tFrame.pApdu[4] != 0x00) ||
        (tFrame.pApdu[5] != 0x0b) ||
        (tFrame.pApdu[6] != 0x00))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_ITEM;
    }

    offset = 0x07;
    if (tFrame.pApdu[offset ++] != DATA_SUCCESS)
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "报文返回失败:");
        return ERR_IFACE;
    }

    if (tFrame.pApdu[offset ++] == 0)
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "报文返回失败:");
        return ERR_IFACE;
    }

    if ((tFrame.pApdu[offset] == DT_STRUCTURE) && (tFrame.pApdu[offset + 1] != 4))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "报文返回错误:");
        return ERR_IFACE;
    }
    offset += 2;

    //表号
    len = tFrame.pApdu[offset + 1];
    offset += (len + 2);

    //ESAM序列号
    len = tFrame.pApdu[offset + 1];
    offset += (len + 2);

    //随机数1
    len = tFrame.pApdu[offset + 1];
    memcpy(rand_b, &tFrame.pApdu[offset + 2], 24);
    offset += (len + 2);

    //随机数2
    len = tFrame.pApdu[offset + 1];
    memcpy(rand_b + 24, &tFrame.pApdu[offset + 2], 24);
    offset += (len+2);

    memcpy(cRanCat, rand_b, 48);
    *cRanCat_len = 48;
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : negotiate_end_packaged
* @brief     ：组织开始协商报文
* @param[in] ：tsa          扩展模块地址
               cRanCat     来自扩展模块所返回的 48 字节密文数据
               cRanCat_len 来自扩展模块所返回的 48 字节密文数据长度。
* @param[out]：sendbuf  发送报文
               bufLen  报文长度
* @return    ：错误码
*********************************************************************
*/
int negotiate_end_packaged(OOP_TSA_T tsa, uint8 *cRanCat, uint32 cRanCat_len, uint8 *sendbuf, uint32 *buflen)
{
    int32 ret = 0;

    OOP_OMD_U omd = {0};
    uint8     inData[256] = {0};
    uint32    inLen = 0;
    uint32    offset = 0;
    uint8     rand_c[16] = {0};

    if ((cRanCat == NULL) || (sendbuf == NULL) || (buflen == NULL))
    {
        return ERR_PTR;
    }

    ret = Master_SM4UpdateEnd(cRanCat, 48, rand_c, 16);
    if(ret != TRUE) 
    {
        printf("Master_SM4UpdateEnd failed**\n");
        return ERR_EXECFAIL;
    }
    
    omd.value = 0xf100C000;

    inData[offset ++] = DT_OCTET_STRING;
    inData[offset ++] = 16;

    memcpy(&inData[offset], rand_c, 16);
    offset += 16;

    //inData[offset ++] = 0;

    inLen = offset;

    ret = sm_act_msg_package(tsa, omd, inData, inLen, sendbuf, SEND_BUF_LEN);
    if (ret < 0)
    {
        SM_FMT_DEBUG("报文组织失败\n");
        return ERR_NORMAL;
    }

    *buflen = ret;
    
    return ERR_OK;

}

/*
*********************************************************************
* @name      : negotiate_end_parse
* @brief     ：解析获取扩展模块esam序列号的报文
* @param[in] ：rcvbuf  待解析报文
               buflen  待解析报文报文长度
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int negotiate_end_parse(uint8 *rcvbuf, uint32 buflen)
{
    int          ret = 0;
    uint16       offset = 0;
    FRAME_HEAD_T tFrame = {0};
    
    if (rcvbuf == NULL)
    {
        return ERR_PTR;
    }

    if ((rcvbuf[0] != 0x68) && (rcvbuf[buflen - 1] != 0x16))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_OPTION;
    }

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(rcvbuf, buflen, &tFrame, TRUE);
    if (0 != ret)
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误 错误码[%d]:", ret);
        return ERR_OPTION;
    }

    if ((tFrame.pApdu[0] != 0x87) ||
        (tFrame.pApdu[1] != 0x01) ||
        (tFrame.pApdu[3] != 0xf1) ||
        (tFrame.pApdu[4] != 0x00) ||
        (tFrame.pApdu[5] != 0x0c) ||
        (tFrame.pApdu[6] != 0x00))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_ITEM;
    }

    offset = 0x07;
    if (tFrame.pApdu[offset ++] != DATA_SUCCESS)
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "报文返回失败:");
        return ERR_IFACE;
    }
    
    return ERR_OK;
}

#endif

#if DESC("密文安全传输流程", 1)
/*
*********************************************************************
* @name      : cipher_encryption_packaged
* @brief     ：组织密文报文
* @param[in] ：tsa      扩展模块地址
               apdu    待加密的明文APDU
               apduLen 待加密的明文APDU长度
               isMac   是否输出MAC
* @param[out]：sendbuf  发送报文
               bufLen  报文长度
* @return    ：错误码
*********************************************************************
*/
int cipher_encryption_packaged(OOP_TSA_T tsa, uint8 *apdu, uint32 apduLen, uint8 isMac, uint8 *sendbuf, 
    uint32 *buflen)
{
    int32  ret = 0;
    uint8  cipher[SEND_APDU_LEN] = {0};
    uint32 cipherLen = 0;
    uint8  inData[SEND_APDU_LEN] = {0};
    uint32 offset = 0;
    uint32 len = 0;

    if ((apdu == NULL) || (sendbuf == NULL))
    {
        return ERR_PTR;
    }

    ret = Master_SM4Encrypt(isMac, apdu, apduLen, cipher, &cipherLen);
    if (ret != TRUE)
    {
        SM_BUF_DEBUG(apdu, apduLen, "报文加密失败:");
        return ERR_AUTH;
    }

    inData[offset ++] = 0x10;
    inData[offset ++] = 0x01;
    if (cipherLen < 0x80)
    {
        inData[offset ++] = cipherLen;
    }
    else
    {
        inData[offset++] = 0x82;
        inData[offset++] = (uint8)(cipherLen>>8);
        inData[offset++] = (uint8)(cipherLen);
    }
    memcpy(&inData[offset], cipher, cipherLen);
    offset += cipherLen;
    if (isMac)
    {
        //不知道怎么获取MAC?
        inData[offset++] = 0x03; //验证方式
        inData[offset++] = 0x00;
        inData[offset++] = 0x00;
        inData[offset++] = 0x00;
        inData[offset++] = 0x00;
        inData[offset++] = 0x02;
        inData[offset++] = 0x00;
        inData[offset++] = 0x00;
    }
    else
    {
        inData[offset++] = 0x03; //验证方式
        inData[offset++] = 0x00;
        inData[offset++] = 0x00;
        inData[offset++] = 0x00;
        inData[offset++] = 0x00;
        inData[offset++] = 0x02;
        inData[offset++] = 0x00;
        inData[offset++] = 0x00;
    }

    len = make_apdu_to_package(tsa, inData, offset, sendbuf, SEND_BUF_LEN);
    if (len <= 0)
    {
        SM_BUF_DEBUG(inData, offset, "报文组织失败:");
        return ERR_NORMAL;
    }
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : cipher_encryption_parse
* @brief     ：解析密文
* @param[in] ：rcvbuf  待解析报文
               buflen  待解析报文报文长度
* @param[out]：apdu     解析出来的明文APDU
               apduLen 解析出来的明文APDU长度
* @return    ：错误码
*********************************************************************
*/
int cipher_encryption_parse(uint8 *rcvbuf, uint32 buflen, uint8 *apdu, uint32 *apduLen)
{
    int          ret = 0;
    uint16       offset = 0;
    FRAME_HEAD_T tFrame = {0};

    uint8  cipher[SEND_APDU_LEN] = {0};
    uint32 cipherLen = 0;
    uint8  isMac = 0;
    uint8  mac[4] = {0};
    
    if ((rcvbuf == NULL) || (apdu == NULL) || (apduLen == NULL))
    {
        return ERR_PTR;
    }

    if ((rcvbuf[0] != 0x68) && (rcvbuf[buflen - 1] != 0x16))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_OPTION;
    }

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(rcvbuf, buflen, &tFrame, TRUE);
    if (0 != ret)
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误 错误码[%d]:", ret);
        return ERR_OPTION;
    }

    if ((tFrame.pApdu[offset ++] != 0x90) ||
        (tFrame.pApdu[offset ++] != 0x01))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_ITEM;
    }

    cipherLen = tFrame.pApdu[offset ++];
    if (cipherLen == 0x81)
    {
        cipherLen = tFrame.pApdu[offset++];
    }
    else if (cipherLen == 0x82)
    {
        cipherLen = tFrame.pApdu[offset++];
        cipherLen <<= 8;
        cipherLen += tFrame.pApdu[offset++];
    }
    else if (cipherLen > 0x82 || cipherLen == 0x80)
    {
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(cipher, &tFrame.pApdu[offset], cipherLen);
    offset  += cipherLen;

    isMac = tFrame.pApdu[offset ++];
    if (isMac)
    {
        if (tFrame.pApdu[offset ++] != 0)
        {
            return DATA_TYPE_UNMATCHED;
        }

        memcpy(mac, &tFrame.pApdu[offset], 4);
        offset += 4;

        //带MAC如何解密？
    }
    else
    {
        ret = Master_SM4Decrypt(FALSE, cipher, (uint32)cipherLen, apdu, apduLen);
        if(ret != TRUE)
        {
            SM_BUF_DEBUG(cipher, cipherLen, "解密有误 错误码[%d]", ret);
            return ERR_AUTH;
        }
    }

    return ERR_OK;
}

#endif

#if DESC("明文+Mac 安全传输流程", 1)
/*
*********************************************************************
* @name      : cipher_encryption_packaged
* @brief     ：组织密文报文
* @param[in] ：tsa      扩展模块地址
               apdu    待加密的明文APDU
               apduLen 待加密的明文APDU长度
* @param[out]：sendbuf  发送报文
               bufLen  报文长度
* @return    ：错误码
*********************************************************************
*/
int plainMac_encryption_packaged(OOP_TSA_T tsa, uint8 *apdu, uint32 apduLen, uint8 *sendbuf, uint32 *buflen)
{
    int32  ret = 0;
    uint8  mac[64] = {0};
    uint32 macLen = 0;
    uint8  inData[SEND_APDU_LEN] = {0};
    uint32 offset = 0;
    uint32 len = 0;

    if ((apdu == NULL) || (sendbuf == NULL))
    {
        return ERR_PTR;
    }

    ret = Master_SM4Mac(apdu, apduLen, mac, &macLen);
    if (ret != TRUE)
    {
        SM_BUF_DEBUG(apdu, apduLen, "报文计算MAC失败 错误码[%d]", ret);
        return ERR_AUTH;
    }

    inData[offset ++] = 0x10;
    inData[offset ++] = 0x00;
    if (apduLen < 0x80)
    {
        inData[offset ++] = apduLen;
    }
    else
    {
        inData[offset++] = 0x82;
        inData[offset++] = (uint8)(apduLen>>8);
        inData[offset++] = (uint8)(apduLen);
    }
    memcpy(&inData[offset], apdu, apduLen);
    offset += apduLen;

    inData[offset++] = 0x00; //验证方式
    inData[offset++] = 0x00;
    inData[offset++] = 0x00;
    inData[offset++] = 0x00;
    inData[offset++] = 0x00;
    inData[offset++] = 0x02;
    inData[offset++] = 0x00;
    inData[offset++] = 0x00;
    inData[offset++] = macLen;
    memcpy(&inData[offset], mac, macLen);
    offset += macLen;

    len = make_apdu_to_package(tsa, inData, offset, sendbuf, SEND_BUF_LEN);
    if (len <= 0)
    {
        SM_BUF_DEBUG(inData, offset, "报文组织失败:");
        return ERR_NORMAL;
    }
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : cipher_encryption_parse
* @brief     ：解析密文
* @param[in] ：rcvbuf  待解析报文
               buflen  待解析报文报文长度
* @param[out]：apdu     解析出来的明文APDU
               apduLen 解析出来的明文APDU长度
* @return    ：错误码
*********************************************************************
*/
int plainMac_encryption_parse(uint8 *rcvbuf, uint32 buflen, uint8 *apdu, uint32 *apduLen)
{
    int          ret = 0;
    uint16       offset = 0;
    FRAME_HEAD_T tFrame = {0};

    uint8  plain[SEND_APDU_LEN] = {0};
    uint32 plainLen = 0;
    uint8  isMac = 0;
    uint8  mac[64] = {0};
    uint32 macLen = 0;
    
    if ((rcvbuf == NULL) || (apdu == NULL) || (apduLen == NULL))
    {
        return ERR_PTR;
    }

    if ((rcvbuf[0] != 0x68) && (rcvbuf[buflen - 1] != 0x16))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_OPTION;
    }

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(rcvbuf, buflen, &tFrame, TRUE);
    if (0 != ret)
    {
        ACSAMP_BUF_DEBUG(rcvbuf, buflen, "输入报文有误 错误码[%d]:", ret);
        return ERR_OPTION;
    }

    if ((tFrame.pApdu[offset ++] != 0x90) ||
        (tFrame.pApdu[offset ++] != 0x00))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_ITEM;
    }

    plainLen = tFrame.pApdu[offset ++];
    if (plainLen == 0x81)
    {
        plainLen = tFrame.pApdu[offset++];
    }
    else if (plainLen == 0x82)
    {
        plainLen = tFrame.pApdu[offset++];
        plainLen <<= 8;
        plainLen += tFrame.pApdu[offset++];
    }
    else if (plainLen > 0x82 || plainLen == 0x80)
    {
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(plain, &tFrame.pApdu[offset], plainLen);
    offset  += plainLen;

    isMac = tFrame.pApdu[offset ++];
    if (isMac)
    {
        if (tFrame.pApdu[offset ++] != 0)
        {
            return DATA_TYPE_UNMATCHED;
        }

        macLen = tFrame.pApdu[offset ++];
        memcpy(mac, &tFrame.pApdu[offset], macLen);
        offset += macLen;

        ret = Master_SM4MacVerify(plain, plainLen, mac, macLen);
        if (ret != TRUE)
        {
            SM_BUF_DEBUG(plain, plainLen, "报文验证MAC失败 错误码[%d]", ret);
            return ERR_AUTH;
        }
    }

    memcpy(apdu, plain, plainLen);
    *apduLen = plainLen;

    return ERR_OK;
}
#endif

#if DESC("明文+RN 安全传输流程", 1)
/*
*********************************************************************
* @name      : cipher_encryption_packaged
* @brief     ：组织密文报文
* @param[in] ：tsa      扩展模块地址
               apdu    待加密的明文APDU
               apduLen 待加密的明文APDU长度
* @param[out]：sendbuf  发送报文
               bufLen  报文长度
* @return    ：错误码
*********************************************************************
*/
int plainRN_encryption_packaged(OOP_TSA_T tsa, uint8 *apdu, uint32 apduLen, uint8 *sendbuf, uint32 *buflen)
{
    uint8  inData[SEND_APDU_LEN] = {0};
    uint32 offset = 0;
    uint32 len = 0;
    
    uint8 rand_a[16] = {0};

    if ((apdu == NULL) || (sendbuf == NULL))
    {
        return ERR_PTR;
    }
    
    memset(rand_a, 0x00, sizeof(rand_a));
    callback_getRandomWords((uint32 *)rand_a, 4);

    inData[offset ++] = 0x10;
    inData[offset ++] = 0x00;
    if (apduLen < 0x80)
    {
        inData[offset ++] = apduLen;
    }
    else
    {
        inData[offset++] = 0x82;
        inData[offset++] = (uint8)(apduLen>>8);
        inData[offset++] = (uint8)(apduLen);
    }
    memcpy(&inData[offset], apdu, apduLen);
    offset += apduLen;

    inData[offset++] = 0x01; //验证方式
    inData[offset++] = 0x10;
    memcpy(&inData[offset], rand_a, 16);
    offset += 16;

    len = make_apdu_to_package(tsa, inData, offset, sendbuf, SEND_BUF_LEN);
    if (len <= 0)
    {
        SM_BUF_DEBUG(inData, offset, "报文组织失败:");
        return ERR_NORMAL;
    }
    
    return ERR_OK;
}

/*
*********************************************************************
* @name      : cipher_encryption_parse
* @brief     ：解析密文
* @param[in] ：rcvbuf  待解析报文
               buflen  待解析报文报文长度
* @param[out]：apdu     解析出来的明文APDU
               apduLen 解析出来的明文APDU长度
* @return    ：错误码
*********************************************************************
*/
int plainRN_encryption_parse(uint8 *rcvbuf, uint32 buflen, uint8 *apdu, uint32 *apduLen)
{
    int          ret = 0;
    uint16       offset = 0;
    FRAME_HEAD_T tFrame = {0};

    uint8  plain[SEND_APDU_LEN] = {0};
    uint32 plainLen = 0;
    uint8  isMac = 0;
    uint8  mac[64] = {0};
    uint32 macLen = 0;
    
    if ((rcvbuf == NULL) || (apdu == NULL) || (apduLen == NULL))
    {
        return ERR_PTR;
    }

    if ((rcvbuf[0] != 0x68) && (rcvbuf[buflen - 1] != 0x16))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_OPTION;
    }

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(rcvbuf, buflen, &tFrame, TRUE);
    if (0 != ret)
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误 错误码[%d]:", ret);
        return ERR_OPTION;
    }

    if ((tFrame.pApdu[offset ++] != 0x90) ||
        (tFrame.pApdu[offset ++] != 0x00))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_ITEM;
    }

    plainLen = tFrame.pApdu[offset ++];
    if (plainLen == 0x81)
    {
        plainLen = tFrame.pApdu[offset++];
    }
    else if (plainLen == 0x82)
    {
        plainLen = tFrame.pApdu[offset++];
        plainLen <<= 8;
        plainLen += tFrame.pApdu[offset++];
    }
    else if (plainLen > 0x82 || plainLen == 0x80)
    {
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(plain, &tFrame.pApdu[offset], plainLen);
    offset  += plainLen;

    isMac = tFrame.pApdu[offset ++];
    if (isMac)
    {
        if (tFrame.pApdu[offset ++] != 0)
        {
            return DATA_TYPE_UNMATCHED;
        }

        macLen = tFrame.pApdu[offset ++];
        memcpy(mac, &tFrame.pApdu[offset], macLen);
        offset += macLen;

        ret = Master_SM4MacVerify(plain, plainLen, mac, macLen);
        if (ret != TRUE)
        {
            SM_BUF_DEBUG(plain, plainLen, "报文验证MAC失败 错误码[%d]", ret);
            return ERR_AUTH;
        }
    }

    memcpy(apdu, plain, plainLen);
    *apduLen = plainLen;

    return ERR_OK;
}
#endif

#if DESC("明文+RN_MAC 安全传输流程", 1)

/*扩展控制模块主站上报的报文一般采用RN_MAC形式，终端需要先解析
  验证上报报文(带RN_MAC)，再组织上报确认报文(带MAC)*/
/*
*********************************************************************
* @name      : plainRN_MAC_encryption_parse
* @brief     ：解析密文
* @param[in] ：rcvbuf  待解析报文
               buflen  待解析报文报文长度
* @param[out]：apdu     解析出来的明文APDU
               apduLen 解析出来的明文APDU长度
* @return    ：错误码
*********************************************************************
*/
int plainRN_MAC_encryption_parse(uint8 *rcvbuf, uint32 buflen, uint8 *apdu, uint32 *apduLen)
{
    int          ret = 0;
    uint16       offset = 0;
    FRAME_HEAD_T tFrame = {0};

    uint8  plain[SEND_APDU_LEN] = {0};
    uint32 plainLen = 0;
    uint8  plainRn[SEND_APDU_LEN] = {0};
    uint32 plainRnLen = 0;
    uint8  choice = 0;
    uint8  rn[64] = {0};
    uint32 rnLen = 0;
    uint8  mac[64] = {0};
    uint32 macLen = 0;
    
    if ((rcvbuf == NULL) || (apdu == NULL) || (apduLen == NULL))
    {
        return ERR_PTR;
    }

    if ((rcvbuf[0] != 0x68) && (rcvbuf[buflen - 1] != 0x16))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_OPTION;
    }

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    ret = app_get_frame(rcvbuf, buflen, &tFrame, TRUE);
    if (0 != ret)
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误 错误码[%d]:", ret);
        return ERR_OPTION;
    }

    if ((tFrame.pApdu[offset ++] != 0x90) ||
        (tFrame.pApdu[offset ++] != 0x00))
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "输入报文有误:");
        return ERR_ITEM;
    }

    plainLen = tFrame.pApdu[offset ++];
    if (plainLen == 0x81)
    {
        plainLen = tFrame.pApdu[offset++];
    }
    else if (plainLen == 0x82)
    {
        plainLen = tFrame.pApdu[offset++];
        plainLen <<= 8;
        plainLen += tFrame.pApdu[offset++];
    }
    else if (plainLen > 0x82 || plainLen == 0x80)
    {
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(plain, &tFrame.pApdu[offset], plainLen);
    offset  += plainLen;

    choice = tFrame.pApdu[offset ++];
    if (choice == 2)
    {
        rnLen = tFrame.pApdu[offset ++];
        memcpy(rn, &tFrame.pApdu[offset], rnLen);
        offset += rnLen;
        
        macLen = tFrame.pApdu[offset ++];
        memcpy(mac, &tFrame.pApdu[offset], macLen);
        offset += macLen;

        memcpy(plainRn, plain, plainLen);
        memcpy(&plainRn[plainLen], rn, rnLen);
        plainRnLen = plainLen + rnLen;

        ret = Master_SM4MacVerify(plainRn, plainRnLen, mac, macLen);
        if (ret != TRUE)
        {
            SM_BUF_DEBUG(plainRn, plainRnLen, "报文验证MAC失败 错误码[%d]", ret);
            return ERR_AUTH;
        }
    }
    else
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "非RN_MAC类型报文");
    }

    memcpy(apdu, plain, plainLen);
    *apduLen = plainLen;

    return ERR_OK;

}

/*
*********************************************************************
* @name      : plainRN_MAC_encryption_packaged
* @brief     ：组织报文
* @param[in] ：tsa      扩展模块地址
               apdu    待加密的明文APDU
               apduLen 待加密的明文APDU长度
* @param[out]：sendbuf  发送报文
               bufLen  报文长度
* @return    ：错误码
*********************************************************************
*/
int plainRN_MAC_encryption_packaged(OOP_TSA_T tsa, uint8 *apdu, uint32 apduLen, uint8 *sendbuf, uint32 *buflen)
{
    int32  ret = 0;
    uint8  mac[64] = {0};
    uint32 macLen = 0;
    uint8  inData[SEND_APDU_LEN] = {0};
    uint32 offset = 0;
    int32  len = 0;

    if ((apdu == NULL) || (sendbuf == NULL))
    {
        return ERR_PTR;
    }

    ret = Master_SM4Mac(apdu, apduLen, mac, &macLen);
    if (ret != TRUE)
    {
        SM_BUF_DEBUG(apdu, apduLen, "报文计算MAC失败 错误码[%d]", ret);
        return ERR_AUTH;
    }

    inData[offset ++] = 0x10;
    inData[offset ++] = 0x00;
    if (apduLen < 0x80)
    {
        inData[offset ++] = apduLen;
    }
    else
    {
        inData[offset++] = 0x82;
        inData[offset++] = (uint8)(apduLen>>8);
        inData[offset++] = (uint8)(apduLen);
    }
    memcpy(&inData[offset], apdu, apduLen);
    offset += apduLen;

    inData[offset++] = 0x00; //验证方式
    inData[offset++] = 0x00;
    inData[offset++] = 0x00;
    inData[offset++] = 0x00;
    inData[offset++] = 0x00;
    inData[offset++] = 0x02;
    inData[offset++] = 0x00;
    inData[offset++] = 0x00;
    inData[offset++] = macLen;
    memcpy(&inData[offset], mac, macLen);
    offset += macLen;

    len = make_apdu_to_package(tsa, inData, offset, sendbuf, SEND_BUF_LEN);
    if (len <= 0)
    {
        SM_BUF_DEBUG(inData, offset, "报文组织失败 错误码[%d]", len);
        return ERR_NORMAL;
    }
    
    return ERR_OK;

}

#endif

#if DESC("密钥下装流程", 1)
/*
*********************************************************************
* @name      : update_terminal_keyStatus
* @brief     ：更新扩展模块密钥报文
* @param[in] ：tsa      扩展模块地址
* @param[out]：sendbuf  发送报文
               bufLen  报文长度
* @return    ：错误码
*********************************************************************
*/
int act_extModule_key_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen)
{
    int32 ret = 0;
    uint8 sik[96] = {0};
    uint8 key[74] = {0};

    uint8     inData[SEND_APDU_LEN] = {0};
    uint32    inLen = 0;
    uint32    offset = 0;


    if ((buflen == NULL) || (sendbuf == NULL))
    {
        return ERR_PTR;
    }

    //sik如何获取，暂时不知道？

    ret = Master_SM4SetSIK(sik);
    if (ret != TRUE)
    {
        SM_BUF_DEBUG(sik, 96, "终端更新密钥失败 错误码[%d]", ret);
        return ERR_NORMAL;
    }

    ret = Master_SM4InitSIK(key);
    if (ret != TRUE)
    {
        SM_BUF_DEBUG(key, 74, "终端获取模块更新密钥失败 错误码[%d]", ret);
        return ERR_NORMAL;
    }

    inData[offset ++] = 0x07;
    inData[offset ++] = 0x01;
    inData[offset ++] = 0x00;
    inData[offset ++] = 0xf1;
    inData[offset ++] = 0x00;
    inData[offset ++] = 0x07;
    inData[offset ++] = 0x00;
    inData[offset ++] = 0x02;
    inData[offset ++] = DT_OCTET_STRING;
    inData[offset ++] = 64;
    memcpy(&inData[offset], &key[6], 64);
    offset += 64;
    inData[offset ++] = DT_SIDMAC;
    inData[offset ++] = key[0];
    inData[offset ++] = key[1];
    inData[offset ++] = key[2];
    inData[offset ++] = key[3];
    inData[offset ++] = 2;
    inData[offset ++] = key[4];
    inData[offset ++] = key[5];
    inData[offset ++] = 4;
    inData[offset ++] = key[70];
    inData[offset ++] = key[71];
    inData[offset ++] = key[72];
    inData[offset ++] = key[73];
    
    inData[offset ++] = 0;

    inLen = offset;

    ret = cipher_encryption_packaged(tsa, inData, inLen, TRUE, sendbuf, buflen);
    if (ret < 0)
    {
        SM_BUF_DEBUG(inData, inLen, "报文组织失败 错误码[%d]", ret);
        return ERR_NORMAL;
    }

    return ERR_OK;
}

/*
*********************************************************************
* @name      : act_extModule_key_parse
* @brief     ：解析更新扩展模块密钥回码
* @param[in] ：rcvbuf  待解析报文
               buflen  待解析报文报文长度
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int act_extModule_key_parse(uint8 *rcvbuf, uint32 buflen)
{
    int32  ret = 0;
    uint8  apdu[SEND_APDU_LEN] = {0};
    uint32 apduLen = 0;

    ret = cipher_encryption_parse(rcvbuf, buflen, apdu, &apduLen);
    if (ret < 0)
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "报文解密失败 错误码[%d]", ret);
        return ERR_NORMAL;
    }

    if (apdu[7] != DATA_SUCCESS)
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "报文返回失败 错误码[%d]", apdu[7]);
        return apdu[7];
    }

    ret = setStatus(NORM_STAT);
    if (ret != TRUE)
    {
        SM_BUF_DEBUG(rcvbuf, buflen, "终端设置密钥状态失败 错误码[%d]", ret);
        return ERR_NORMAL;
    }
    
    return ERR_OK;
}

#if DESC("安全软算法库回调初始化", 1)
/*
*********************************************************************
* @name      : init_extModule_callback_fun
* @brief     ：安全算法库回调函数初始化
* @param[in] ：
* @param[out]：
* @return    ：
*********************************************************************
*/
flash_set_zeros callback_flash_setWords_zero;
flash_set_words callback_flash_setWords;
flash_get_words callback_flash_getWords;
flash_clean_t callback_flash_cleanWords;

flash_set_bytes callback_flash_setBytes;
flash_get_bytes callback_flash_getBytes;
flash_clean_t callback_flash_cleanBytes;

random_gen_t callback_getRandomWords;
ikb_gen_t callback_getDataIKB;
ikc_gen_t callback_getDataIKC;
ika_gen_t callback_getDataIKA;
sikb_gen_t callback_getDataSIKB;
sikc_gen_t callback_getDataSIKC;
sika_gen_t callback_getDataSIKA;

void init_extModule_callback_fun(void)
{
    callback_flash_setWords_zero = Flash_set_zero_long;
    callback_flash_setWords = Flash_set_data_long;
    callback_flash_getWords = Flash_get_data_long;
    callback_flash_cleanWords = Flash_clean_long;

    callback_flash_setBytes = Flash_set_data_byte;
    callback_flash_getBytes = Flash_get_data_byte;
    callback_flash_cleanBytes = Flash_clean_bytes;

    callback_getRandomWords = my_getRand;
    callback_getDataIKB = my_getDataIKB;
    callback_getDataIKC = my_getDataIKC;
    callback_getDataIKA = my_getDataIKA;
    callback_getDataSIKB = my_getDataSIKB;
    callback_getDataSIKC = my_getDataSIKC;
    callback_getDataSIKA = my_getDataSIKA;

    return;
}

#endif

#endif

