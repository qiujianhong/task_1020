/*
*********************************************************************
* @file    class1.c
* @brief： class1处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "commClass.h"
#include "oopservice.h"


//
// class1
//


/**
*********************************************************************
* @brief：     698格式报文转从数据中心的数据(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class1(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
	return DATA_SUCCESS;
}


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1_energy(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_ENERGY_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_ARRAY;

    //结构成员数
    dstBuf[dOffset++] = data.nNum;

    for (i = 0; i < data.nNum; i++)
    {
        dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
            return DATA_TYPE_UNMATCHED;
        }   
    }     

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1_energys(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_ENERGYS_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_ARRAY;

    //结构成员数
    dstBuf[dOffset++] = data.nNum;

    for (i = 0; i < data.nNum; i++)
    {
        dar = basic_data_to_buf(E_DOUBLE_LONG, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(int32));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
            return DATA_TYPE_UNMATCHED;
        }   
    }     

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1_henergy(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_HENERGY_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_ARRAY;

    //结构成员数
    dstBuf[dOffset++] = data.nNum;

    for (i = 0; i < data.nNum; i++)
    {
        dar = basic_data_to_buf(E_LONG64_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(uint64));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
            return DATA_TYPE_UNMATCHED;
        }   
    }     

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1_henergys(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_HENERGYS_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_ARRAY;

    //结构成员数
    dstBuf[dOffset++] = data.nNum;

    for (i = 0; i < data.nNum; i++)
    {
        dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
            return DATA_TYPE_UNMATCHED;
        }   
    }     

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->eData)
    {
        case E_OOP_ENERGY_T:   //电能量(不带符号)
        {
        	ACUDP_FMT_TRACE("data_to_buf_class1() case E_OOP_ENERGY_T:   //电能量(不带符号) \n");
           	return data_to_buf_class1_energy(pOadInfo,  convert);
        }break;

        case E_OOP_ENERGYS_T:   //电能量(带符号)
        {
        	ACUDP_FMT_TRACE("data_to_buf_class1() case E_OOP_ENERGYS_T:   //电能量(带符号) \n");
           	return data_to_buf_class1_energys(pOadInfo,  convert);
        }break;

        case E_OOP_HENERGY_T:   //高精度电能量(不带符号)
        {
        	ACUDP_FMT_TRACE("data_to_buf_class1() case E_OOP_HENERGY_T:   //高精度电能量(不带符号) \n");
           	return data_to_buf_class1_henergy(pOadInfo,  convert);
        }break;

        case E_OOP_HENERGYS_T:   //高精度电能量(带符号)
        {
        	ACUDP_FMT_TRACE("data_to_buf_class1() case E_OOP_HENERGYS_T:   //高精度电能量(带符号) \n");
           	return data_to_buf_class1_henergys(pOadInfo,  convert);
        }break;

        //......
        
        default:
        {
        	ACUDP_FMT_TRACE("data_to_buf_class1() 未识别的数据类型 pOadInfo->pTab->eData=%d \n", pOadInfo->pTab->eData);
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}


