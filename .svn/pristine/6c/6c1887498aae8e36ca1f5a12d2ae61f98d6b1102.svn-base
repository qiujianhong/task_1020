/*
*********************************************************************
* @file    class21.c
* @brief： class21处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "macro.h"

//
// class21
//

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(证书版本)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class21_Licensever(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    OOP_LICENSEVER_T oopLicensever;
    MEMZERO(&oopLicensever, sizeof(oopLicensever));

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if(convert->srcLen != sizeof(OOP_LICENSEVER_T))
    {
        dar = DATA_OTHER_REASON;
        return dar;
    }
    memcpy((uint8*)&oopLicensever, &srcBuf[sOffset], convert->srcLen);
    sOffset += sizeof(OOP_LICENSEVER_T);

    /* 结构类型 */
    dstBuf[dOffset++] = DT_STRUCTURE;

    /* 结构成员数 */
    dstBuf[dOffset++] = 2;

    /* 终端证书版本 */
    dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBuf, convert->dstLen, &dOffset, &oopLicensever.data1, sizeof(OOP_OCTETVAR16_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("获取终端证书版本信息data-->buf err\n");
        return dar;
    }

    /* 主站证书版本 */
    dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBuf, convert->dstLen, &dOffset, &oopLicensever.data2, sizeof(OOP_OCTETVAR16_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("获取主站证书版本信息data-->buf err\n");
        return dar;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}
/**
*********************************************************************
* @brief：      安全存储对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_oad_array(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_OADS_T oadsets;
    uint8 offlen = 0; //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    memcpy(&oadsets, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(oadsets.nNum, lenBuf);
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    for(i = 0; i < oadsets.nNum; i++)
    {       
        //OAD
        if(basic_data_to_buf(E_OAD, dstBufTmp, dstLen-offset, &offset, &oadsets.oad[i], sizeof(OOP_OAD_U)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }
    }

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(全模式参数)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class21_securitymods(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SCURITYMODE_T data;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //数组类型
    dstBuf[dOffset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(data.nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 2;
                
        //对象标识
        dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.mode[i].object, sizeof(OI));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),object(%d)\n", i, dar, data.mode[i].object);
            return DATA_TYPE_UNMATCHED;
        }
        
        //关联对象属性
        dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.mode[i].access, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.mode[i].access);
            return DATA_TYPE_UNMATCHED;
        } 
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(安全存储对象列表)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class21_esam_oads(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    OOP_OADS_T oopOadList = {0};

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if(convert->srcLen != sizeof(OOP_OADS_T))
    {
        dar = DATA_OTHER_REASON;
        return dar;
    }
    memcpy(&oopOadList, srcBuf+sOffset, convert->srcLen);
    sOffset += sizeof(OOP_OADS_T);

    /* 安全存储对象列表 */
    dar = data_to_buf_oad_array(pOadInfo, dstBuf, convert->dstLen, &dOffset, &oopOadList, sizeof(OOP_OADS_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("获取安全存储对象列表信息data-->buf err\n");
        return dar;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

OOP_DAR_E data_to_buf_class21_terminal(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TERMINAL_LIMIT_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;
            
   
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.ntmp1, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),object(%d)\n", i, dar, data.ntmp1);
        return DATA_TYPE_UNMATCHED;
    }
    

    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.ntmp2, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.ntmp2);
        return DATA_TYPE_UNMATCHED;
    }


    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(当前计数器)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class21_cur_count(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CUR_COUNT_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 3;
            
    //单地址应用协商计数器
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.asctrCnt, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),object(%d)\n", i, dar, data.asctrCnt);
        return DATA_TYPE_UNMATCHED;
    }
    
    //主动上报计数器
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.asctrCnt, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.asctrCnt);
        return DATA_TYPE_UNMATCHED;
    }
        
    //应用广播通信序列号
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.agseq, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.agseq);
        return DATA_TYPE_UNMATCHED;
    } 

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class21)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class21_f100(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOadInfo->pTab->eData)
    {
        case E_OOP_LICENSEVER_T:                     
        {
            /* 证书版本 */
            return data_to_buf_class21_Licensever(pOadInfo, convert);
            break;
        }
        break;

        case E_OOP_OADS_T:
        {
            /* 安全存储对象列表 */
            return data_to_buf_class21_esam_oads(pOadInfo, convert);
            break;
        }

        case E_OOP_SCURITYMODE_T:
        {
            /* 全模式参数 */
            return data_to_buf_class21_securitymods(pOadInfo, convert);
            break;
        }

        case E_OOP_CUR_COUNT_T:
        {
            /* 当前计数器 */
            return data_to_buf_class21_cur_count(pOadInfo, convert);
            break;
        }

        case E_OOP_TERMINAL_LIMIT_T:
        {
            /* 当前计数器 */
            return data_to_buf_class21_terminal(pOadInfo, convert);
            break;
        }
        
        default:
        {
            if (convert->srcLen != pOadInfo->pTab->dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            dar = basic_data_to_buf(pOadInfo->pTab->eData, dstBuf, convert->dstLen, &dOffset, &srcBuf[sOffset], pOadInfo->pTab->dataLen);
            if (dar == DATA_SUCCESS)
            {
                sOffset += pOadInfo->pTab->dataLen;
            }
            break;
        }
    }

    if (dar == DATA_SUCCESS)
    {
        //避免每步都判断，放最后检查，但检查前可能已经越界
        if((sOffset > srcLen) || (dOffset > dstLen))
        {
            PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
            return DATA_SCOPE_OF_ACCESS_VIOLATED;        
        }

        *convert->dOffset = dOffset;
        *convert->sOffset = sOffset;
    }

    return dar;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class21)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class21(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0xF1000200:   //获取ESAM序列号
        case 0xF1000300:   //获取ESAM版本号
        case 0xF1000400:   //获取ESAM对称密钥版本
        case 0xF1000500:   //获取ESAM会话时效阈值
        case 0xF1000600:   //获取ESAM会话时效剩余时间
        case 0xF1000700:   //获取ESAM当前计数器
        case 0xF1000800:   //证书版本
        case 0xF1000900:   //终端证书序列号
        case 0xF1000A00:   //获取终端证书
        case 0xF1000B00:   //获取主站证书序列号
        case 0xF1000C00:   //获取主站证书
        case 0xF1000E00:
        case 0xF1001500:
        case 0xF1001100:
        {
           return data_to_buf_class21_f100(pOadInfo,  convert);
        }break;
        
        //......
        
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      class21_invoke_get_esam_serial
* @brief：     class21的GET服务
* @param[in] ：pOdaInfo       OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_get_esam_serial(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    TESAMInfo_t tESAMInfoBuff;
    uint8       tESAMBuf[2048] = {0};
    uint16      tESAMInfoBuffLen = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&tESAMInfoBuff, sizeof(TESAMInfo_t));

    /* 先读出ESAM信息 */
    if(pOadInfo->pTab->oad.attID >= 2 && pOadInfo->pTab->oad.attID <= 12)
    {   
        if(NO_ESAM ==Init_Fd_Esam())
        {
            PRTL_FMT_DEBUG("Init_Fd_Esam err\n");
            return DATA_OTHER_REASON;
        }

        if(ESAM_FAIL == GetTEsam_Info(0xFF, (uint8*)&tESAMBuf[0], &tESAMInfoBuffLen))
        {
            PRTL_FMT_DEBUG("GetTEsam_Info err\n");
            return DATA_OTHER_REASON;
        }   

        memcpy(&tESAMInfoBuff, tESAMBuf, sizeof(TESAMInfo_t));
    } 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000200:   //ESAM序列号
        {
            OOP_OCTETVAR1K_T  octerVar = {0};
            PRTL_BUF_LOGCC(&tESAMInfoBuff.esamSerial[0], sizeof(tESAMInfoBuff.esamSerial), "class21_invoke_get_f100 : tESAMInfoBuff.esamSerial:");

            //转换成E_OOP_OCTETVAR1K_T格式
            octerVar.nNum =  sizeof(tESAMInfoBuff.esamSerial);
            memcpy(&octerVar.value[0], &tESAMInfoBuff.esamSerial[0], sizeof(tESAMInfoBuff.esamSerial));

            //长度容错
            if (len + sizeof(uint32) > 256)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_f100 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return DATA_OTHER_REASON;
            }

            convert.srcBuf = &octerVar;
            convert.srcLen = sizeof(octerVar);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class21(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：      class21_invoke_get_esam_ver
* @brief：     class21的GET服务
* @param[in] ：pOdaInfo       OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_get_esam_ver(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    TESAMInfo_t tESAMInfoBuff;
    uint8       tESAMBuf[2048] = {0};  
    uint16      tESAMInfoBuffLen = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&tESAMInfoBuff, sizeof(TESAMInfo_t));

    /* 先读出ESAM信息 */
    if(pOadInfo->pTab->oad.attID >= 2 && pOadInfo->pTab->oad.attID <= 12)
    {   
        if(NO_ESAM ==Init_Fd_Esam())
        {
            PRTL_FMT_DEBUG("Init_Fd_Esam err\n");
            return DATA_OTHER_REASON;
        }

        if(ESAM_FAIL == GetTEsam_Info(0xFF, (uint8*)&tESAMBuf[0], &tESAMInfoBuffLen))
        {
            PRTL_FMT_DEBUG("GetTEsam_Info err\n");
            return DATA_OTHER_REASON;
        }   

        memcpy(&tESAMInfoBuff, tESAMBuf, sizeof(TESAMInfo_t));
    } 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000300:   //ESAM版本号
        {
            OOP_OCTETVAR1K_T  octerVar = {0};
            PRTL_BUF_LOGCC(&tESAMInfoBuff.esamVersion[0], sizeof(tESAMInfoBuff.esamVersion), "class21_invoke_get_f100 : tESAMInfoBuff.esamVersion:");

            //转换成E_OOP_OCTETVAR1K_T格式
            octerVar.nNum =  sizeof(tESAMInfoBuff.esamVersion);
            memcpy(&octerVar.value[0], &tESAMInfoBuff.esamVersion[0], sizeof(tESAMInfoBuff.esamVersion));

            //长度容错
            if (len + sizeof(uint32) > 256)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_f100 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return DATA_OTHER_REASON;
            }

            convert.srcBuf = &octerVar;
            convert.srcLen = sizeof(octerVar);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class21(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：      class21_invoke_get_key_var
* @brief：     class21的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_get_key_var(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    TESAMInfo_t tESAMInfoBuff;
    uint8       tESAMBuf[2048] = {0};
    uint16      tESAMInfoBuffLen = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&tESAMInfoBuff, sizeof(TESAMInfo_t));

    /* 先读出ESAM信息 */
    if(pOadInfo->pTab->oad.attID >= 2 && pOadInfo->pTab->oad.attID <= 12)
    {   
        if(NO_ESAM ==Init_Fd_Esam())
        {
            PRTL_FMT_DEBUG("Init_Fd_Esam err\n");
            return DATA_OTHER_REASON;
        }

        if(ESAM_FAIL == GetTEsam_Info(0xFF, (uint8*)&tESAMBuf[0], &tESAMInfoBuffLen))
        {
            PRTL_FMT_DEBUG("GetTEsam_Info err\n");
            return DATA_OTHER_REASON;
        }   

        memcpy(&tESAMInfoBuff, tESAMBuf, sizeof(TESAMInfo_t));
    } 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000400:   //对称密钥版本
        {
            OOP_OCTETVAR1K_T  octerVar = {0};
            PRTL_BUF_LOGCC(&tESAMInfoBuff.symKeyVersion[0], sizeof(tESAMInfoBuff.symKeyVersion), "class21_invoke_get_f100 : tESAMInfoBuff.symKeyVersion:");

            //转换成E_OOP_OCTETVAR1K_T格式
            octerVar.nNum =  sizeof(tESAMInfoBuff.symKeyVersion);
            memcpy(&octerVar.value[0], &tESAMInfoBuff.symKeyVersion[0], sizeof(tESAMInfoBuff.symKeyVersion));

            //长度容错
            if (len + sizeof(uint32) > 256)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_f100 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return DATA_OTHER_REASON;
            }

            convert.srcBuf = &octerVar;
            convert.srcLen = sizeof(octerVar);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class21(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：      class21_invoke_get_authTimeThr
* @brief：     class21的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_get_authTimeThr(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    TESAMInfo_t tESAMInfoBuff;
    uint8       tESAMBuf[2048] = {0};
    uint16      tESAMInfoBuffLen = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&tESAMInfoBuff, sizeof(TESAMInfo_t));

    /* 先读出ESAM信息 */
    if(pOadInfo->pTab->oad.attID >= 2 && pOadInfo->pTab->oad.attID <= 12)
    {   
        if(NO_ESAM ==Init_Fd_Esam())
        {
            PRTL_FMT_DEBUG("Init_Fd_Esam err\n");
            return DATA_OTHER_REASON;
        }

        if(ESAM_FAIL == GetTEsam_Info(0xFF, (uint8*)&tESAMBuf[0], &tESAMInfoBuffLen))
        {
            PRTL_FMT_DEBUG("GetTEsam_Info err\n");
            return DATA_OTHER_REASON;
        }   

        memcpy(&tESAMInfoBuff, tESAMBuf, sizeof(TESAMInfo_t));
    } 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000500:   //会话时效门限
        {
            uint32  data = 0;
            PRTL_BUF_LOGCC(&tESAMInfoBuff.authTimeThr[0], sizeof(tESAMInfoBuff.authTimeThr), "class21_invoke_get_authTimeThr : tESAMInfoBuff.authTimeThr:");

            //转换成E_DOUBLE_LONG_UNSIGNED格式
            memcpy_r(&data, &tESAMInfoBuff.authTimeThr[0], sizeof(tESAMInfoBuff.authTimeThr));
            PRTL_FMT_LOGCC("authTimeThr:%d\n", data);

            //长度容错
            if (len + sizeof(uint32) > 256)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_authTimeThr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return DATA_OTHER_REASON;
            }

            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class21(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01 */
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：      class21_invoke_get_authTimeCnt
* @brief：     class21的GET服务
* @param[in] ：pOdaInfo       OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_get_authTimeCnt(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    TESAMInfo_t tESAMInfoBuff;
    uint8       tESAMBuf[2048] = {0};
    uint16      tESAMInfoBuffLen = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&tESAMInfoBuff, sizeof(TESAMInfo_t));

    /* 先读出ESAM信息 */
    if(pOadInfo->pTab->oad.attID >= 2 && pOadInfo->pTab->oad.attID <= 12)
    {   
        if(NO_ESAM ==Init_Fd_Esam())
        {
            PRTL_FMT_DEBUG("Init_Fd_Esam err\n");
            return DATA_OTHER_REASON;
        }

        if(ESAM_FAIL == GetTEsam_Info(0xFF, (uint8*)&tESAMBuf[0], &tESAMInfoBuffLen))
        {
            PRTL_FMT_DEBUG("GetTEsam_Info err\n");
            return DATA_OTHER_REASON;
        }   

        memcpy(&tESAMInfoBuff, tESAMBuf, sizeof(TESAMInfo_t));
    } 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000600:   //会话时效剩余时间
        {
            uint32  data = 0;
            PRTL_BUF_LOGCC(&tESAMInfoBuff.authTimeThr[0], sizeof(tESAMInfoBuff.authTimeThr), "class21_invoke_get_f100 : tESAMInfoBuff.authTimeThr:");

            //转换成E_DOUBLE_LONG_UNSIGNED格式
            memcpy_r(&data, &tESAMInfoBuff.authTimeThr[0], sizeof(tESAMInfoBuff.authTimeThr));

            //长度容错
            if (len + sizeof(uint32) > 256)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_f100 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return DATA_OTHER_REASON;
            }

            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class21(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：      class21_invoke_get_curCount
* @brief：     class21的GET服务
* @param[in] ：pOdaInfo       OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_get_curCount(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    TESAMInfo_t tESAMInfoBuff;
    uint8       tESAMBuf[2048] = {0};
    uint16      tESAMInfoBuffLen = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&tESAMInfoBuff, sizeof(TESAMInfo_t));

    /* 先读出ESAM信息 */
    if(pOadInfo->pTab->oad.attID >= 2 && pOadInfo->pTab->oad.attID <= 12)
    {   
        if(NO_ESAM ==Init_Fd_Esam())
        {
            PRTL_FMT_DEBUG("Init_Fd_Esam err\n");
            return DATA_OTHER_REASON;
        }

        if(ESAM_FAIL == GetTEsam_Info(0xFF, (uint8*)&tESAMBuf[0], &tESAMInfoBuffLen))
        {
            PRTL_FMT_DEBUG("GetTEsam_Info err\n");
            return DATA_OTHER_REASON;
        }   

        memcpy(&tESAMInfoBuff, tESAMBuf, sizeof(TESAMInfo_t));
    } 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000700:   //当前计数器
        {
            OOP_CUR_COUNT_T  data = {0};
            PRTL_BUF_LOGCC(&tESAMInfoBuff.asctrCnt[0], sizeof(tESAMInfoBuff.asctrCnt), "class21_invoke_get_curCount : tESAMInfoBuff.asctrCnt:");
            PRTL_BUF_LOGCC(&tESAMInfoBuff.asctrReportCnt[0], sizeof(tESAMInfoBuff.asctrReportCnt), "class21_invoke_get_curCount : tESAMInfoBuff.asctrReportCnt:");
            PRTL_BUF_LOGCC(&tESAMInfoBuff.agseq[0], sizeof(tESAMInfoBuff.agseq), "class21_invoke_get_curCount : tESAMInfoBuff.agseq:");

            memcpy_r(&data.asctrCnt, &tESAMInfoBuff.asctrCnt[0], sizeof(tESAMInfoBuff.asctrCnt));
            memcpy_r(&data.asctrReportCnt, &tESAMInfoBuff.asctrReportCnt[0], sizeof(tESAMInfoBuff.asctrReportCnt));
            memcpy_r(&data.agseq, &tESAMInfoBuff.agseq[0], sizeof(tESAMInfoBuff.agseq));

            PRTL_FMT_LOGCC("class21_invoke_get_curCount : data.asctrCnt:%d\n", data.asctrCnt);
            PRTL_FMT_LOGCC("class21_invoke_get_curCount : data.asctrReportCnt:%d\n", data.asctrReportCnt);
            PRTL_FMT_LOGCC("class21_invoke_get_curCount : data.agseq:%d\n", data.agseq);

            //长度容错
            if (len + sizeof(uint32) > 256)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_f100 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return DATA_OTHER_REASON;
            }

            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class21(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：      class21_invoke_get_lic_ver
* @brief：     class21的GET服务
* @param[in] ：pOdaInfo       OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_get_lic_ver(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0; 

    TESAMInfo_t tESAMInfoBuff;
    uint8       tESAMBuf[2048] = {0};
    uint16      tESAMInfoBuffLen = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&tESAMInfoBuff, sizeof(TESAMInfo_t));

    /* 先读出ESAM信息 */
    if(pOadInfo->pTab->oad.attID >= 2 && pOadInfo->pTab->oad.attID <= 12)
    {   
        if(NO_ESAM ==Init_Fd_Esam())
        {
            PRTL_FMT_DEBUG("Init_Fd_Esam err\n");
            return DATA_OTHER_REASON;
        }

        if(ESAM_FAIL == GetTEsam_Info(0xFF, (uint8*)&tESAMBuf[0], &tESAMInfoBuffLen))
        {
            PRTL_FMT_DEBUG("GetTEsam_Info err\n");
            return DATA_OTHER_REASON;
        }   

        memcpy(&tESAMInfoBuff, tESAMBuf, sizeof(TESAMInfo_t));
    } 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000800:   //当前计数器
        {
            OOP_LICENSEVER_T  data = {0};

            data.data1.nNum = 1;
            data.data1.value[0] = tESAMInfoBuff.masterCertVer[0];
            data.data2.nNum = 1;
            data.data2.value[0] = tESAMInfoBuff.tmnCertVer[0];
            
            PRTL_BUF_DEBUG(data.data1.value, data.data1.nNum, "主站证书版本:");
            PRTL_BUF_DEBUG(data.data2.value, data.data2.nNum, "终端证书版本:");

            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class21(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：      class21_invoke_get_term_cert_no
* @brief：     class21的GET服务
* @param[in] ：pOdaInfo       OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_get_term_cert_no(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0; 

    TESAMInfo_t tESAMInfoBuff;
    uint8       tESAMBuf[2048] = {0};
    uint16      tESAMInfoBuffLen = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&tESAMInfoBuff, sizeof(TESAMInfo_t));

    /* 先读出ESAM信息 */
    if(pOadInfo->pTab->oad.attID >= 2 && pOadInfo->pTab->oad.attID <= 12)
    {   
        if(NO_ESAM ==Init_Fd_Esam())
        {
            PRTL_FMT_DEBUG("Init_Fd_Esam err\n");
            return DATA_OTHER_REASON;
        }

        if(ESAM_FAIL == GetTEsam_Info(0xFF, (uint8*)&tESAMBuf[0], &tESAMInfoBuffLen))
        {
            PRTL_FMT_DEBUG("GetTEsam_Info err\n");
            return DATA_OTHER_REASON;
        }   

        memcpy(&tESAMInfoBuff, tESAMBuf, sizeof(TESAMInfo_t));
    } 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000900:
        {
            OOP_OCTETVAR1K_T data = {0};

            data.nNum = 16;
            memcpy(data.value, tESAMInfoBuff.tmnCertSeriNo, data.nNum);
            
            PRTL_BUF_DEBUG(data.value, data.nNum, "终端证书序列号:");

            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class21(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：      class21_invoke_get_mast_cert_no
* @brief：     class21的GET服务
* @param[in] ：pOdaInfo       OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_get_mast_cert_no(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0; 

    TESAMInfo_t tESAMInfoBuff;
    uint8       tESAMBuf[2048] = {0};
    uint16      tESAMInfoBuffLen = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&tESAMInfoBuff, sizeof(TESAMInfo_t));

    /* 先读出ESAM信息 */
    if(pOadInfo->pTab->oad.attID >= 2 && pOadInfo->pTab->oad.attID <= 12)
    {   
        if(NO_ESAM ==Init_Fd_Esam())
        {
            PRTL_FMT_DEBUG("Init_Fd_Esam err\n");
            return DATA_OTHER_REASON;
        }

        if(ESAM_FAIL == GetTEsam_Info(0xFF, (uint8*)&tESAMBuf[0], &tESAMInfoBuffLen))
        {
            PRTL_FMT_DEBUG("GetTEsam_Info err\n");
            return DATA_OTHER_REASON;
        }   

        memcpy(&tESAMInfoBuff, tESAMBuf, sizeof(TESAMInfo_t));
    } 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000B00:
        {
            OOP_OCTETVAR1K_T data = {0};

            data.nNum = 16;
            memcpy(data.value, tESAMInfoBuff.masterCertSeriNo, data.nNum);
            
            PRTL_BUF_DEBUG(data.value, data.nNum, "主站证书序列号:");

            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class21(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：      class21_invoke_get_cert_cert
* @brief：     class21的GET服务
* @param[in] ：pOdaInfo       OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_get_term_cert(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    TESAMInfo_t tESAMInfoBuff;
    uint8       tESAMBuf[2048] = {0};
    uint16      tESAMInfoBuffLen = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&tESAMInfoBuff, sizeof(TESAMInfo_t));

    /* 先读出ESAM信息 */
    if(pOadInfo->pTab->oad.attID >= 2 && pOadInfo->pTab->oad.attID <= 12)
    {   
        if(NO_ESAM ==Init_Fd_Esam())
        {
            PRTL_FMT_DEBUG("Init_Fd_Esam err\n");
            return DATA_OTHER_REASON;
        }

        if(ESAM_FAIL == GetTEsam_Info(0xFF, (uint8*)&tESAMBuf[0], &tESAMInfoBuffLen))
        {
            PRTL_FMT_DEBUG("GetTEsam_Info err\n");
            return DATA_OTHER_REASON;
        }   

        memcpy(&tESAMInfoBuff, tESAMBuf, sizeof(TESAMInfo_t));
    }

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000A00:   //终端证书
        {
            OOP_OCTETVAR2K_T  data = {0};

            uint8  TmnAppCert[2000];
            uint16 TmnAppCertLen=0;
            GetTEsam_Info(0x0B, TmnAppCert, &TmnAppCertLen);
            if (TmnAppCertLen > 2048)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_mast_cert : MastAppCertLen is too long:%d\n", TmnAppCertLen);
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            PRTL_BUF_LOGCC(&TmnAppCert[0], TmnAppCertLen, "class21_invoke_get_curCount : TmnAppCert:");

            data.nNum = TmnAppCertLen;
            memcpy(&data.value[0], &TmnAppCert[0], TmnAppCertLen);

            PRTL_FMT_LOGCC("class21_invoke_get_term_cert : data.nNum:%d\n", data.nNum);
            PRTL_BUF_LOGCC(&data.value[0], TmnAppCertLen, "class21_invoke_get_term_cert : data:");

            //长度容错
            if (len + sizeof(uint32) > 256)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_term_cert failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return DATA_OTHER_REASON;
            }

            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class21(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}


/**
*********************************************************************
* @name：      class21_invoke_get_mast_cert
* @brief：     class21的GET服务
* @param[in] ：pOdaInfo       OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_get_mast_cert(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    TESAMInfo_t tESAMInfoBuff;
    uint8       tESAMBuf[2048] = {0};
    uint16      tESAMInfoBuffLen = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&tESAMInfoBuff, sizeof(TESAMInfo_t));

    /* 先读出ESAM信息 */
    if(pOadInfo->pTab->oad.attID >= 2 && pOadInfo->pTab->oad.attID <= 12)
    {   
        if(NO_ESAM ==Init_Fd_Esam())
        {
            PRTL_FMT_DEBUG("Init_Fd_Esam err\n");
            return DATA_OTHER_REASON;
        }

        if(ESAM_FAIL == GetTEsam_Info(0xFF, (uint8*)&tESAMBuf[0], &tESAMInfoBuffLen))
        {
            PRTL_FMT_DEBUG("GetTEsam_Info err\n");
            return DATA_OTHER_REASON;
        }   

        memcpy(&tESAMInfoBuff, tESAMBuf, sizeof(TESAMInfo_t));
    }

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000C00:   //主站证书
        {
            OOP_OCTETVAR2K_T  data = {0};

            uint8  MastAppCert[2000];
            uint16 MastAppCertLen=0;
            GetTEsam_Info(0x0C, MastAppCert, &MastAppCertLen); 
            if (MastAppCertLen > 2048)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_mast_cert : MastAppCertLen is too long:%d\n", MastAppCertLen);
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            PRTL_BUF_LOGCC(&MastAppCert[0], MastAppCertLen, "class21_invoke_get_curCount : MastAppCert:");

            data.nNum = MastAppCertLen;
            memcpy(&data.value[0], &MastAppCert[0], MastAppCertLen);

            PRTL_FMT_LOGCC("class21_invoke_get_mast_cert : data.nNum:%d\n", data.nNum);
            PRTL_BUF_LOGCC(&data.value[0], MastAppCertLen, "class21_invoke_get_mast_cert : data:");

            //长度容错
            if (len + sizeof(uint32) > 256)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_mast_cert failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return DATA_OTHER_REASON;
            }

            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class21(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：       class21_invoke_get
* @brief：      class21的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_get_f100(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    TESAMInfo_t tESAMInfoBuff;
    uint8       tESAMBuf[2048] = {0};
    uint16      tESAMInfoBuffLen = 0;

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&tESAMInfoBuff, sizeof(TESAMInfo_t));

    /* 先读出ESAM信息 */
    if(pOadInfo->pTab->oad.attID >= 2 && pOadInfo->pTab->oad.attID <= 12)
    {   
        if(NO_ESAM ==Init_Fd_Esam())
        {
            PRTL_FMT_DEBUG("Init_Fd_Esam err\n");
            return DATA_OTHER_REASON;
        }

        if(ESAM_FAIL == GetTEsam_Info(0xFF, (uint8*)&tESAMBuf[0], &tESAMInfoBuffLen))
        {
            PRTL_FMT_DEBUG("GetTEsam_Info err\n");
            return DATA_OTHER_REASON;
        }   

        memcpy(&tESAMInfoBuff, tESAMBuf, sizeof(TESAMInfo_t));
    } 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000200:   //ESAM序列号
        {
            OOP_OCTETVAR1K_T  octerVar = {0};
            PRTL_BUF_LOGCC(&tESAMInfoBuff.esamSerial[0], sizeof(tESAMInfoBuff.esamSerial), "class21_invoke_get_f100 : tESAMInfoBuff.esamSerial:");

            //转换成E_OOP_OCTETVAR1K_T格式
            octerVar.nNum =  sizeof(tESAMInfoBuff.esamSerial);
            memcpy(&octerVar.value[0], &tESAMInfoBuff.esamSerial[0], sizeof(tESAMInfoBuff.esamSerial));

            //长度容错
            if (len + sizeof(uint32) > 256)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_f100 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return DATA_OTHER_REASON;
            }

            convert.srcBuf = &octerVar;
            convert.srcLen = sizeof(octerVar);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;

    case 0xF1000300:   //ESAM版本号
        {
            OOP_OCTETVAR1K_T  octerVar = {0};
            PRTL_BUF_LOGCC(&tESAMInfoBuff.esamVersion[0], sizeof(tESAMInfoBuff.esamVersion), "class21_invoke_get_f100 : tESAMInfoBuff.esamVersion:");

            //转换成E_OOP_OCTETVAR1K_T格式
            octerVar.nNum =  sizeof(tESAMInfoBuff.esamVersion);
            memcpy(&octerVar.value[0], &tESAMInfoBuff.esamVersion[0], sizeof(tESAMInfoBuff.esamVersion));

            //长度容错
            if (len + sizeof(uint32) > 256)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_f100 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return DATA_OTHER_REASON;
            }

            convert.srcBuf = &octerVar;
            convert.srcLen = sizeof(octerVar);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;

    case 0xF1000400:   //对称密钥版本
        {
            OOP_OCTETVAR1K_T  octerVar = {0};
            PRTL_BUF_LOGCC(&tESAMInfoBuff.symKeyVersion[0], sizeof(tESAMInfoBuff.symKeyVersion), "class21_invoke_get_f100 : tESAMInfoBuff.symKeyVersion:");

            //转换成E_OOP_OCTETVAR1K_T格式
            octerVar.nNum =  sizeof(tESAMInfoBuff.symKeyVersion);
            memcpy(&octerVar.value[0], &tESAMInfoBuff.symKeyVersion[0], sizeof(tESAMInfoBuff.symKeyVersion));

            //长度容错
            if (len + sizeof(uint32) > 256)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_f100 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return DATA_OTHER_REASON;
            }

            convert.srcBuf = &octerVar;
            convert.srcLen = sizeof(octerVar);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;

    case 0xF1000500:   //会话时效门限
        {
            OOP_OCTETVAR1K_T  octerVar = {0};
            PRTL_BUF_LOGCC(&tESAMInfoBuff.authTimeThr[0], sizeof(tESAMInfoBuff.authTimeThr), "class21_invoke_get_f100 : tESAMInfoBuff.authTimeThr:");

            //转换成E_OOP_OCTETVAR1K_T格式
            octerVar.nNum =  sizeof(tESAMInfoBuff.authTimeThr);
            memcpy(&octerVar.value[0], &tESAMInfoBuff.authTimeThr[0], sizeof(tESAMInfoBuff.authTimeThr));

            //长度容错
            if (len + sizeof(uint32) > 256)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_f100 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return DATA_OTHER_REASON;
            }

            convert.srcBuf = &octerVar;
            convert.srcLen = sizeof(octerVar);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;

    case 0xF1000600:   //会话时效剩余时间
        {
            OOP_OCTETVAR1K_T  octerVar = {0};
            PRTL_BUF_LOGCC(&tESAMInfoBuff.authTimeThr[0], sizeof(tESAMInfoBuff.authTimeThr), "class21_invoke_get_f100 : tESAMInfoBuff.authTimeThr:");

            //转换成E_OOP_OCTETVAR1K_T格式
            octerVar.nNum =  sizeof(tESAMInfoBuff.authTimeThr);
            memcpy(&octerVar.value[0], &tESAMInfoBuff.authTimeThr[0], sizeof(tESAMInfoBuff.authTimeThr));

            //长度容错
            if (len + sizeof(uint32) > 256)
            {
                PRTL_FMT_DEBUG("class21_invoke_get_f100 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                return DATA_OTHER_REASON;
            }

            convert.srcBuf = &octerVar;
            convert.srcLen = sizeof(octerVar);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class21(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @brief：     密钥更新
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_key_update(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOPM_ESAM_7_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

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
    
    //密钥密文
    dar = basic_buf_to_data(E_OCTET_STRING_2K, srcBuf, srcLen, &sOffset, &data.data, sizeof(OOP_OCTETVAR2K_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //数据验证码
    //结构成员数
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    if(srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //SID
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    if(srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //SID标识
    if(srcBuf[sOffset++] != DT_DOUBLE_LONG_UNSIGNED)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_buf_to_data(DT_DOUBLE_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.sidMac.sid.nKNum, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //SID附加数据
    if(srcBuf[sOffset++] != DT_OCTET_STRING)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(data.sidMac.sid.keyData, srcBuf+sOffset, sizeof(data.sidMac.sid.keyData));
    sOffset += sizeof(data.sidMac.sid.keyData);

    if(srcBuf[sOffset++] != DT_MAC)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_buf_to_data(DT_MAC, srcBuf, srcLen, &sOffset, &data.sidMac.mac, sizeof(data.sidMac.mac));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief：     密钥更新
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_cert_update(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOPM_ESAM_7_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

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
    
    //密钥密文
    dar = basic_buf_to_data(E_OCTET_STRING_2K, srcBuf, srcLen, &sOffset, &data.data, sizeof(OOP_OCTETVAR2K_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //数据验证码

    //SID
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    if(srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //SID标识
    if(srcBuf[sOffset++] != DT_DOUBLE_LONG_UNSIGNED)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_buf_to_data(DT_DOUBLE_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.sidMac.sid.nKNum, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //SID附加数据
    if(srcBuf[sOffset++] != DT_OCTET_STRING)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(data.sidMac.sid.keyData, srcBuf+sOffset, sizeof(data.sidMac.sid.keyData));
    sOffset += sizeof(data.sidMac.sid.keyData);

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief：      从698格式报文转数据中心的数据
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class21(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}

#if DESC("ACT服务数据接口",1)
/**
*********************************************************************
* @name：      class21_invoke_act_key_update
* @brief：     密钥更新
* @param[in] ：pOdaInfo       OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_act_key_update(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //CTESAM tESAM;
    uint32 offset  = 0;
    int8 rslt = ESAM_FAIL;
    OOP_DAR_E dar = DATA_OTHER_REASON;
    uint8 *pKey;
    uint16 certiLen;
    SID_t sid;
    uint8 *pAttachData;
    uint8 attachLen;
    uint8 mac[4];
    inData += 4;
    
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000700:          //密钥更新 此段代码与老终端代码一致
        {
            if(*inData++ != DT_STRUCTURE || *inData++ != 2)
                return(DATA_TYPE_UNMATCHED);//数据格式错误
            
            if(*inData++ != DT_OCTET_STRING)
                return(DATA_TYPE_UNMATCHED);//数据格式错误

            certiLen = *inData++;
            if(certiLen == 0x81)
                certiLen = *inData++;
            else if(certiLen == 0x82)
            {
                certiLen = *inData++;
                certiLen <<= 8;
                certiLen += *inData++;
            }
            else if(certiLen > 0x82 || certiLen== 0x80)
                return DATA_TYPE_UNMATCHED;

            pKey = (uint8*)inData;
            inData += certiLen;
            if(*inData++ != DT_SIDMAC)
                return(DATA_TYPE_UNMATCHED);//数据格式错误

            memcpy((uint8*)&sid, inData, 4);
            inData += 4;
            attachLen = *inData++;
            pAttachData = (uint8*)inData;
            inData += attachLen;    
            if(*inData++ != 4)//MAC为4个字节
                return(DATA_TYPE_UNMATCHED);//数据格式错误

            memcpy(mac, inData, 4);
            inData += 4;
            rslt = TESAMDataUpdate(sid, pAttachData, attachLen, pKey, certiLen, mac);
            if(rslt!=ESAM_OK)
            {
                PRTL_FMT_DEBUG("密钥更新失败， rslt = %d\n", rslt);
                return(DATA_OTHER_REASON);//密钥更新失败
            }
            dar = DATA_SUCCESS;
        }
        break;
    
    default:
        break;
    }
    PRTL_FMT_LOGCC("密钥更新成功dar = %d\n", dar);

    SaveTESAMInfo();

    memcpy_r(&outData->pResultData[offset], &pOadInfo->pTab->oad.value, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}


/**
*********************************************************************
* @name：      class21_invoke_act_cert_update
* @brief：     证书更新
* @param[in] ：pOdaInfo       OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_act_cert_update(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //CTESAM tESAM;
    uint32 offset  = 0;
    int8 rslt = ESAM_FAIL;
    OOP_DAR_E dar = DATA_OTHER_REASON;
    uint8  certi[2000];
    uint16 certiLen;
    SID_t sid;
    uint8 *pAttachData;
    uint8 attachLen;
    inData += 4;
    
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000800:          //密钥更新 此段代码与老终端代码一致
        {
            if(*inData++ != DT_STRUCTURE || *inData++ != 2)
                return(DATA_TYPE_UNMATCHED);//数据格式错误

            if(*inData++ != DT_OCTET_STRING)
                return(DATA_TYPE_UNMATCHED);//数据格式错误

            certiLen = *inData++;
            if(certiLen >= 0x80)
            {
                if(certiLen == 0x81)
                    certiLen = *inData++;
                else if(certiLen == 0x82)
                {
                    certiLen = *inData++;
                    certiLen <<= 8;
                    certiLen += *inData++;
                }
                else
                    return(DATA_TYPE_UNMATCHED);//数据格式错误
            }
            if(certiLen > sizeof(certi))
                return(DATA_OTHER_REASON);//数据溢出

            memcpy(certi, inData, certiLen);
            inData += certiLen;
            if(*inData++ != DT_SID)
                return(DATA_TYPE_UNMATCHED);//数据格式错误

            memcpy((uint8*)&sid, inData, 4);
            inData += 4;
            //if(*inData++ != 2)//附加数据长度暂定为2个字节
            //  return(DATA_TYPE_UNMATCHED);//数据格式错误
            //memcpy(attachData,inData,2);  
            attachLen = *inData++;
            pAttachData = (uint8*)inData;
            inData += attachLen;
            
            rslt = TESAMDataUpdate(sid, pAttachData, attachLen, certi, certiLen, NULL);
            if(rslt!=ESAM_OK)
            {
                PRTL_FMT_DEBUG("证书更新失败 rslt = %d\n", rslt);
                return(DATA_OTHER_REASON);//证书更新失败
            }
            dar = DATA_SUCCESS;
        }
        break;
    
    default:
        {
            dar = DATA_READ_WRITE_DENIED;
        }
        break;
    }

    SaveTESAMInfo();

    memcpy_r(&outData->pResultData[offset], &pOadInfo->pTab->oad.value, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;  
}

/**
*********************************************************************
* @name：      class21_invoke_act_param_update
* @brief：      参数更新
* @param[in] ：pOdaInfo       OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_act_param_update(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //CTESAM tESAM;
    uint32 offset  = 0;
    int8 rslt = ESAM_FAIL;
    OOP_DAR_E dar = DATA_OTHER_REASON;
    uint8  param[2000];
    uint16 paramLen;
    SID_t sid;
    uint8 *pAttachData;
    uint8 attachLen;
    inData += 4;
    
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF1000900:          //参数更新 此段代码与老终端代码一致
        {
            if(*inData++ != DT_STRUCTURE || *inData++ != 2)
                return(DATA_TYPE_UNMATCHED);//数据格式错误

            if(*inData++ != DT_OCTET_STRING)
                return(DATA_TYPE_UNMATCHED);//数据格式错误

            paramLen = *inData++;
            if(paramLen >= 0x80)
            {
                if(paramLen == 0x81)
                    paramLen = *inData++;
                else if(paramLen == 0x82)
                {
                    paramLen = *inData++;
                    paramLen <<= 8;
                    paramLen += *inData++;
                }
                else
                    return(DATA_TYPE_UNMATCHED);//数据格式错误
            }
            if(paramLen > sizeof(param))
                return(DATA_OTHER_REASON);//数据溢出

            memcpy(param, inData, paramLen);
            inData += paramLen;
            if(*inData++ != DT_SID)
                return(DATA_TYPE_UNMATCHED);//数据格式错误

            memcpy((uint8*)&sid, inData, 4);
            inData += 4;
            //if(*inData++ != 2)//附加数据长度暂定为2个字节
            //  return(DATA_TYPE_UNMATCHED);//数据格式错误
            //memcpy(attachData,inData,2);  
            attachLen = *inData++;
            pAttachData = (uint8*)inData;
            inData += attachLen;
            
            rslt = TESAMDataUpdate(sid, pAttachData, attachLen, param, paramLen, NULL);
            if(rslt!=ESAM_OK)
            {
                PRTL_FMT_DEBUG("参数更新失败 rslt = %d\n", rslt);
                return(DATA_OTHER_REASON);//证书更新失败
            }
            dar = DATA_SUCCESS;
        }
        break;
    
    default:
        {
            dar = DATA_READ_WRITE_DENIED;
        }
        break;
    }

    SaveTESAMInfo();

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;  
}

#endif

#if DESC("class21对外接口函数",1)

/**
*********************************************************************
* @name：       class21_invoke_set
* @brief：      class21的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}

/**
*********************************************************************
* @name：       class21_invoke_get
* @brief：      class21的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0xF100:
        {
            switch (pOadInfo->pTab->oad.attID)
            {
            case 2:
                return class21_invoke_get_esam_serial(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                break;
            case 3:
                return class21_invoke_get_esam_ver(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                break;
            case 4:
                return class21_invoke_get_key_var(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                break;
            case 5:
                return class21_invoke_get_authTimeThr(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                break;
            case 6:
                return class21_invoke_get_authTimeCnt(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                break;
            case 7:
                return class21_invoke_get_curCount(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                break;
            case 8:
                return class21_invoke_get_lic_ver(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                break;
            case 9:
                return class21_invoke_get_term_cert_no(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                break;
            case 10:
                return class21_invoke_get_term_cert(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                break;
            case 11:
                return class21_invoke_get_mast_cert_no(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                break;
            case 12:
                return class21_invoke_get_mast_cert(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
                break;
            default:
                return DATA_OBJECT_UNAVAILABLE;
                break;
            }
        }
        break;
    
    default:
        break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：       class21_invoke_act
* @brief：      class21的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class21_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 7:   //密钥更新
        {
            dar = class21_invoke_act_key_update(pOadInfo,  pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        }break;   

        case 8:   //证书更新
        {
            dar = class21_invoke_act_cert_update(pOadInfo,  pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        }break;
        
        case 9:   //参数更新
        {
            dar = class21_invoke_act_param_update(pOadInfo,  pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        }break; 

        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;
}


#endif
