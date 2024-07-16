/*
*********************************************************************
* @file    class17.c
* @brief： class17处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"

//
// class17
//

OOP_DAR_E data_to_buf_class17_f3010200(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_OBJ_SHOW_T data = {0};
    //uint8 offlen = 0;      //数组数量长度
    //uint8 lenBuf[3] = {0}; //数组数量编码
    //uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //数组类型
    dstBuf[dOffset++] = DT_ARRAY;

    //数组成员个数
    dstBuf[dOffset++] = data.nNum;

    for(uint32 i =0; i < data.nNum; i++)
    {
        dstBuf[dOffset++] = DT_STRUCTURE;
        dstBuf[dOffset++] = 0x02;

        dar = basic_data_to_buf(E_CSD, dstBuf, dstLen, &dOffset, &data.objs[i].cols, sizeof(data.objs[i].cols));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.objs[i].cols);
            return DATA_TYPE_UNMATCHED;
        }

        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.objs[i].nNum, sizeof(data.objs[i].nNum));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.objs[i].nNum);
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

OOP_DAR_E data_to_buf_class17_f3010400(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_OBJ_SHOW_PARAM_T data = {0};
    //uint8 offlen = 0;      //数组数量长度
    //uint8 lenBuf[3] = {0}; //数组数量编码
    //uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //数组类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //数组成员个数
    dstBuf[dOffset++] = 0x02;

    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.objNumAll, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        //PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.objNumAll);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.objNumMax, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        //PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.objNumMax);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        //PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class17)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class17(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0xF3010400:
        case 0xf3000400:
        {
           return data_to_buf_class17_f3010400(pOadInfo,  convert);
        }break;

        case 0xf3010200:
        {
           return data_to_buf_class17_f3010200(pOadInfo,  convert);
        }break;
        //......
        
        default:
        {
            return basic_db_to_fmt698(pOadInfo,  convert);
        }break;
    }
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：      从数据中心的数据转698格式报文(class24)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class17(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}


#if DESC("class17对外接口函数",1)

/**
*********************************************************************
* @name：       class17_invoke_set
* @brief：      class17的SET服务
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
OOP_DAR_E class17_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}

/**
*********************************************************************
* @name：       class17_invoke_get
* @brief：      class17的GET服务
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
OOP_DAR_E class17_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}

/**
*********************************************************************
* @name：       class17_invoke_act
* @brief：      class17的ACTION服务
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
OOP_DAR_E class17_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}


#endif
