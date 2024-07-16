/*
*********************************************************************
* @file    class12.c
* @brief： class12处理
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
// class12
//

/**
*********************************************************************
* @brief：      脉冲地址
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_get_pulse_addr(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_OCTETVAR16_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data)); 

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //时间转换
    dar = basic_data_to_buf(E_OOP_OCTETVAR16_T, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d)\n", i, dar);
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
* @brief：      脉冲PTCT
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_get_pulse_ptct(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_PTCT_T data;
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
            
    //PT
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.pt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),pt(0x%x)\n", dar, data.pt);
        return DATA_TYPE_UNMATCHED;
    }
    
    //CT
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.ct, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),ct(0x%x)\n", dar, data.ct);
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
* @brief：      脉冲配置
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_get_pulse_cfg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8   offlen    = 0;
    uint8   lenBuf[3] = {0};
    uint32  i = 0;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    OOP_PULSESETS_T data;
    MEMZERO(&data, sizeof(OOP_PULSESETS_T));

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_get_pulse_cfg error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if(convert->srcLen != sizeof(data))
    {
        dar = DATA_OTHER_REASON;
        return dar;
    }
    memcpy((uint8*)&data, &srcBuf[sOffset], convert->srcLen);
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
        dstBuf[dOffset++] = 3;

        //port
        dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.pulse[i].port, sizeof(data.pulse[i].port));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_get_pulse_cfg failed. [%d] dar(%d)\n", i, dar);
            return DATA_TYPE_UNMATCHED;
        }   

        //property
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.pulse[i].property, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_get_pulse_cfg failed. [%d] dar(%d)\n", i, dar);
            return DATA_TYPE_UNMATCHED;
        }      

        //K
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.pulse[i].K, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_get_pulse_cfg failed. [%d] dar(%d)\n", i, dar);
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
* @brief：      脉冲配置
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_get_pulse_bigcfg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8   offlen    = 0;
    uint8   lenBuf[3] = {0};
    uint32  i = 0;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    OOP_PULSESETS_B_T data;
    MEMZERO(&data, sizeof(OOP_PULSESETS_B_T));

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_get_pulse_cfg error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if(convert->srcLen != sizeof(data))
    {
        dar = DATA_OTHER_REASON;
        return dar;
    }
    memcpy((uint8*)&data, &srcBuf[sOffset], convert->srcLen);
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
        dstBuf[dOffset++] = 3;

        //port
        dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.pulse[i].port, sizeof(data.pulse[i].port));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_get_pulse_cfg failed. [%d] dar(%d)\n", i, dar);
            return DATA_TYPE_UNMATCHED;
        }   

        //property
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.pulse[i].property, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_get_pulse_cfg failed. [%d] dar(%d)\n", i, dar);
            return DATA_TYPE_UNMATCHED;
        }      

        //K
        dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.pulse[i].K, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_get_pulse_cfg failed. [%d] dar(%d)\n", i, dar);
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
* @brief：      脉冲功率
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_get_pulse_power(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    uint32 data;
    MEMZERO(&data, sizeof(data));

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_get_pulse_power error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if(convert->srcLen != sizeof(data))
    {
        dar = DATA_OTHER_REASON;
        return dar;
    }
    memcpy((uint8*)&data, &srcBuf[sOffset], convert->srcLen);
    sOffset += sizeof(data);
   

    //功率
    dar = basic_data_to_buf(E_DOUBLE_LONG, dstBuf, dstLen, &dOffset, &data, sizeof(int32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_get_pulse_power failed. dar(%d)\n", dar);
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
* @brief：      脉冲电量
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_get_pulse_energy(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8   offlen    = 0;
    uint8   lenBuf[3] = {0};
    uint32  i = 0;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    OOP_ENERGY_T data;
    MEMZERO(&data, sizeof(data));

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_get_pulse_power error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if(convert->srcLen != sizeof(data))
    {
        dar = DATA_OTHER_REASON;
        return dar;
    }
    memcpy((uint8*)&data, &srcBuf[sOffset], convert->srcLen);
    sOffset += sizeof(data);
   

    //数组类型
    dstBuf[dOffset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(data.nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {     
        //电能量
        dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_get_pulse_cfg failed. [%d] dar(%d)\n", i, dar);
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
* @brief：     从数据中心的数据转698格式报文(class12)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_get_pulse(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x02:
        {
            return data_to_buf_get_pulse_addr(pOadInfo,  convert);
        }break;
        case 0x03:
        {
            return data_to_buf_get_pulse_ptct(pOadInfo,  convert);
        }break;
        case 0x04:
        {
            return data_to_buf_get_pulse_cfg(pOadInfo,  convert);
        }break;
        case 0x05:
        case 0x06:
        {
            return data_to_buf_get_pulse_power(pOadInfo,  convert);
        }break;
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
        case 0x0e:
        case 0x0f:
        case 0x10:
        case 0x11:
        case 0x12:
        {
            return data_to_buf_get_pulse_energy(pOadInfo,  convert);
        }break;
        case 0x1C:
        {
            return data_to_buf_get_pulse_bigcfg(pOadInfo,  convert);
        }break;
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class12)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class12(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
        case 0x2401:
        case 0x2402:
        case 0x2403:
        case 0x2404:
        case 0x2405:
        case 0x2406:
        case 0x2407:
        case 0x2408:
        {
            return data_to_buf_get_pulse(pOadInfo,  convert);
        }break;
        
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

OOP_DAR_E buf_to_data_class12(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：     设置脉冲地址
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
OOP_DAR_E buf_to_data_set_pulse_addr(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_OCTETVAR16_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //地址
    dar = basic_buf_to_data(E_OOP_OCTETVAR16_T, srcBuf, srcLen, &sOffset, &data, sizeof(data));
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
* @brief：     设置脉冲地址
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
OOP_DAR_E buf_to_data_set_pulse_ptct(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_PTCT_T data;
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
    
    //pt
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.pt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //ct
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.ct, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
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

    PRTL_FMT_LOGCC("buf_to_data_input_flag : data.pt = %d\n", data.pt);
    PRTL_FMT_LOGCC("buf_to_data_input_flag : data.ct = %d\n", data.ct);
     
    return dar;
}

/**
*********************************************************************
* @brief：     设置脉冲配置
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
OOP_DAR_E buf_to_data_set_pulse_cfg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_PULSESETS_T data;
    uint8 offlen = 0;      //数组数量长度
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));


    //数组类型
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组成员个数
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //结构体
        if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //结构体元素数量
        if(srcBuf[sOffset++] != 3)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //通道OAD
        dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.pulse[i].port, sizeof(OOP_OAD_U));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }    

        //property
        dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.pulse[i].property, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //k
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.pulse[i].K, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }   
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
* @brief：     设置脉冲配置
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
OOP_DAR_E buf_to_data_set_pulse_bigcfg(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_PULSESETS_B_T data;
    uint8 offlen = 0;      //数组数量长度
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));


    //数组类型
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组成员个数
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //结构体
        if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //结构体元素数量
        if(srcBuf[sOffset++] != 3)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //通道OAD
        dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.pulse[i].port, sizeof(OOP_OAD_U));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }    

        //property
        dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.pulse[i].property, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //k
        dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.pulse[i].K, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }   
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
* @brief：     从698格式报文转数据中心的数据(class12)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_set_pulse(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x02:
        {
            return buf_to_data_set_pulse_addr(pOadInfo,  convert);
        }break;

        case 0x03:
        {
            return buf_to_data_set_pulse_ptct(pOadInfo,  convert);
        }break;

        case 0x04:
        {
            return buf_to_data_set_pulse_cfg(pOadInfo,  convert);
        }break;

        case 0x1C:
        {
            return buf_to_data_set_pulse_bigcfg(pOadInfo,  convert);
        }break; 
        
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：     从698格式报文转数据中心的数据(class12)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class12_set(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
        case 0x2401:
        case 0x2402:
        case 0x2403:
        case 0x2404:
        case 0x2405:
        case 0x2406:
        case 0x2407:
        case 0x2408:
        {
            return buf_to_data_set_pulse(pOadInfo,  convert);
        }break;
        
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      class12_invoke_set_plus
* @brief：     class12的SET服务
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
OOP_DAR_E class12_invoke_set_plus_addr(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class12_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        {
            OOP_OCTETVAR16_T dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;
            PRTL_FMT_LOGCC("class12_invoke_set_plus_addr : dataSet.nNum = %d\n", dataSet.nNum);
            PRTL_BUF_LOGCC(dataSet.value, dataSet.nNum, "class12_invoke_set_plus_addr value:\n");

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_OCTETVAR16_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;  
}

/**
*********************************************************************
* @name：      class12_invoke_set_plus_ptct
* @brief：     class12的SET服务
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
OOP_DAR_E class12_invoke_set_plus_ptct(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class12_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x03:
        {
            OOP_PTCT_T dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;
            PRTL_FMT_LOGCC("class12_invoke_set_plus_ptct : dataSet.pt = %d\n", dataSet.pt);
            PRTL_FMT_LOGCC("class12_invoke_set_plus_ptct : dataSet.ct = %d\n", dataSet.ct);

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_PTCT_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;  
}

/**
*********************************************************************
* @name：      class12_invoke_set_plus_ptct
* @brief：     class12的SET服务
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
OOP_DAR_E class12_invoke_set_plus_cfg(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class12_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x04:
        {
            uint32 i = 0;
            OOP_PULSESETS_T dataSet;
            OOP_PULSESETS_B_T dataBSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            memset(&dataBSet, 0, sizeof(dataBSet));
            dataBSet.nNum = dataSet.nNum;
            for (i = 0; i < dataSet.nNum; i++)
            {
                PRTL_FMT_LOGCC("class12_invoke_set_plus_cfg : dataSet.pulse[%d].port.value = 0x%08x\n", i, dataSet.pulse[i].port.value);
                PRTL_FMT_LOGCC("class12_invoke_set_plus_cfg : dataSet.pulse[%d].property = %d\n", i, dataSet.pulse[i].property);
                PRTL_FMT_LOGCC("class12_invoke_set_plus_cfg : dataSet.pulse[%d].k = %d\n", i, dataSet.pulse[i].K);
                dataBSet.pulse[i].port.value = dataSet.pulse[i].port.value;
                dataBSet.pulse[i].property = dataSet.pulse[i].property;
                dataBSet.pulse[i].K = dataSet.pulse[i].K;
            }
            /* 增加属性28 */
            normal.oad.value = pOadInfo->pTab->oad.value - 0x0400 + 0x1C00;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataBSet, sizeof(OOP_PULSESETS_B_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
            sleep(1);//加个延时让数据中心先进行大脉冲更新

            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_PULSESETS_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }

            //配置总加组单元时，默认下发冻结配置
            if(dataSet.nNum > 0)
            {
                ret = frozen_pulse_objattr_add(pApduInfo->hUdp, normal.oad);
                if(ret != 0)
                {
                    PRTL_FMT_DEBUG("frozen_pulse_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, normal.oad.nObjID);
                }
                ret = extreme_pulse_objattr_add(pApduInfo->hUdp, normal.oad);
                if(ret != 0)
                {
                    PRTL_FMT_DEBUG("extreme_pulse_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, normal.oad.nObjID);
                }
            }
            else
            {
                ret = frozen_pulse_objattr_del(pApduInfo->hUdp, normal.oad);
                if(ret != 0)
                {
                    PRTL_FMT_DEBUG("frozen_pulse_objattr_del failed. ret(%d), OI(0x%04x)\n", ret, normal.oad.nObjID);
                }
                ret = extreme_pulse_objattr_del(pApduInfo->hUdp, normal.oad);
                if(ret != 0)
                {
                    PRTL_FMT_DEBUG("extreme_pulse_objattr_del failed. ret(%d), OI(0x%04x)\n", ret, normal.oad.nObjID);
                }
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;  
}

/**
*********************************************************************
* @name：      class12_invoke_set_plus_bigptct
* @brief：     class12的SET服务
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
OOP_DAR_E class12_invoke_set_plus_bigcfg(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class12_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x1C:
        {
            uint32 i = 0;
            OOP_PULSESETS_T   dataSet;
            OOP_PULSESETS_B_T dataBSet;
            memcpy(&dataBSet, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            /* 脉冲配置检查 */
            memset(&dataSet, 0, sizeof(dataSet));
            dataSet.nNum = dataBSet.nNum;
            dataSet.rsv = 0;
            for (i = 0; i < dataBSet.nNum; i++)
            {
                PRTL_FMT_LOGCC("class12_invoke_set_plus_cfg : dataSet.pulse[%d].port.value = 0x%08x\n", i, dataBSet.pulse[i].port.value);
                PRTL_FMT_LOGCC("class12_invoke_set_plus_cfg : dataSet.pulse[%d].property = %d\n", i, dataBSet.pulse[i].property);
                PRTL_FMT_LOGCC("class12_invoke_set_plus_cfg : dataSet.pulse[%d].k = %d\n", i, dataBSet.pulse[i].K);
                dataSet.pulse[i].port.value = dataBSet.pulse[i].port.value;
                dataSet.pulse[i].property = dataBSet.pulse[i].property;
                if(dataBSet.pulse[i].K >= 0xFFFF)
                {
                    dataSet.rsv = 1;
                    dataSet.pulse[i].K = 0xFFFF;
                }
                else
                {
                    dataSet.pulse[i].K = (uint16)dataBSet.pulse[i].K;
                }
            }

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataBSet, sizeof(OOP_PULSESETS_B_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
            sleep(1);//加个延时让数据中心先进行大脉冲更新
            
            /* 刷新一下属性4 */
            normal.oad.value = pOadInfo->pTab->oad.value - 0x1C00 + 0x0400;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_PULSESETS_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }

            //配置总加组单元时，默认下发冻结配置
            if(dataBSet.nNum > 0)
            {
                ret = frozen_pulse_objattr_add(pApduInfo->hUdp, normal.oad);
                if(ret != 0)
                {
                    PRTL_FMT_DEBUG("frozen_pulse_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, normal.oad.nObjID);
                }
                ret = extreme_pulse_objattr_add(pApduInfo->hUdp, normal.oad);
                if(ret != 0)
                {
                    PRTL_FMT_DEBUG("extreme_pulse_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, normal.oad.nObjID);
                }
            }
            else
            {
                ret = frozen_pulse_objattr_del(pApduInfo->hUdp, normal.oad);
                if(ret != 0)
                {
                    PRTL_FMT_DEBUG("frozen_pulse_objattr_del failed. ret(%d), OI(0x%04x)\n", ret, normal.oad.nObjID);
                }
               ret = extreme_pulse_objattr_del(pApduInfo->hUdp, normal.oad);
                if(ret != 0)
                {
                    PRTL_FMT_DEBUG("extreme_pulse_objattr_del failed. ret(%d), OI(0x%04x)\n", ret, normal.oad.nObjID);
                }
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;  
}


/**
*********************************************************************
* @name：      class12_invoke_set_plus
* @brief：     class12的SET服务
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
OOP_DAR_E class12_invoke_set_plus(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        return class12_invoke_set_plus_addr(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax); 
    case 0x03:
        return class12_invoke_set_plus_ptct(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);  
    case 0x04:
        return class12_invoke_set_plus_cfg(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
    case 0x1C:
        return class12_invoke_set_plus_bigcfg(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：      class12_invoke_get_pluse_addr
* @brief：     class12的GET服务
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
OOP_DAR_E class12_invoke_get_pluse_addr(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x02:
        {
            OOP_OCTETVAR16_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            PRTL_FMT_LOGCC("class8_invoke_get_addr_octet : data.nNum = %d\n", data.nNum);
            PRTL_BUF_LOGCC(&data.value[0], data.nNum, "class8_invoke_get_addr_octet : data.value:");

            //转换成报文数据
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
    dar = data_to_buf_class12(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class12 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class12_invoke_get_pluse_addr
* @brief：     class12的GET服务
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
OOP_DAR_E class12_invoke_get_pluse_ptct(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x03:
        {
            OOP_PTCT_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            PRTL_FMT_LOGCC("class12_invoke_get_pluse_ptct : data.pt = %d\n", data.pt);
            PRTL_FMT_LOGCC("class12_invoke_get_pluse_ptct : data.ct = %d\n", data.ct);

            //转换成报文数据
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
    dar = data_to_buf_class12(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class12 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class12_invoke_get_pluse_cfg
* @brief：     class12的GET服务
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
OOP_DAR_E class12_invoke_get_pluse_cfg(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x04:
        {
            uint32 i = 0;
            OOP_PULSESETS_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class12_invoke_set_plus_cfg : data.pulse[%d].port.value = 0x%08x\n", i, data.pulse[i].port.value);
                PRTL_FMT_LOGCC("class12_invoke_set_plus_cfg : data.pulse[%d].property = %d\n", i, data.pulse[i].property);
                PRTL_FMT_LOGCC("class12_invoke_set_plus_cfg : data.pulse[%d].k = %d\n", i, data.pulse[i].K);
            }

            if(0 != data.rsv)
            {
                /* 大脉冲属性4读不出来 */
                return DATA_DATA_BLOCK_UNAVAILABLE;
            }

            //转换成报文数据
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
    dar = data_to_buf_class12(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class12 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class12_invoke_get_pluse_bigcfg
* @brief：     class12的GET服务
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
OOP_DAR_E class12_invoke_get_pluse_bigcfg(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x1C:
        {
            uint32 i = 0;
            OOP_PULSESETS_B_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class12_invoke_set_plus_cfg : data.pulse[%d].port.value = 0x%08x\n", i, data.pulse[i].port.value);
                PRTL_FMT_LOGCC("class12_invoke_set_plus_cfg : data.pulse[%d].property = %d\n", i, data.pulse[i].property);
                PRTL_FMT_LOGCC("class12_invoke_set_plus_cfg : data.pulse[%d].k = %d\n", i, data.pulse[i].K);
            }

            //转换成报文数据
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
    dar = data_to_buf_class12(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class12 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class12_invoke_get_pluse_power
* @brief：     class12的GET服务
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
OOP_DAR_E class12_invoke_get_pluse_power(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x05:
    case 0x06:
        {
            int32 data = 0;
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            PRTL_FMT_LOGCC("class12_invoke_set_plus_power : data = %d\n", data);

            //转换成报文数据
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
    dar = data_to_buf_class12(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class12 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class12_invoke_get_pluse_energy
* @brief：     class12的GET服务
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
OOP_DAR_E class12_invoke_get_pluse_energy(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int i = 0;
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x0d:
    case 0x0e:
    case 0x0f:
    case 0x10:
    case 0x11:
    case 0x12:
        {
            OOP_ENERGY_T data;
            MEMZERO(&data, sizeof(data));

            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            PRTL_FMT_LOGCC("class12_invoke_set_plus_power : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class12_invoke_set_plus_power : data.nValue[%d] = %d\n", i, data.nValue[i]);
            }

            //转换成报文数据
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
    dar = data_to_buf_class12(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class12 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class12_invoke_get_pluse
* @brief：     class12的GET服务
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
OOP_DAR_E class12_invoke_get_pluse(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        return class12_invoke_get_pluse_addr(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x03:
        return class12_invoke_get_pluse_ptct(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x04:
        return class12_invoke_get_pluse_cfg(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x05:
    case 0x06:
        return class12_invoke_get_pluse_power(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x0d:
    case 0x0e:
    {
        sleep(1);//延时2s等待统计结束
        return class12_invoke_get_pluse_energy(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x0f:
    case 0x10:
    case 0x11:
    case 0x12:
        return class12_invoke_get_pluse_energy(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x1C:
        return class12_invoke_get_pluse_bigcfg(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @brief：     设置脉冲地址
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
OOP_DAR_E buf_to_data_act_pulse_cfg_add(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_PULSECFG_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //结构体
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构体元素数量
    if(srcBuf[sOffset++] != 3)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //通道OAD
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.port, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    

    //property
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.property, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //k
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.K, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
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
* @brief：     设置脉冲地址
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
OOP_DAR_E buf_to_data_act_pulse_cfg_bigadd(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_PULSECFG_B_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //结构体
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构体元素数量
    if(srcBuf[sOffset++] != 3)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //通道OAD
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.port, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    

    //property
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.property, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //k
    dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.K, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
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
* @brief：     设置脉冲地址
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
OOP_DAR_E buf_to_data_act_pulse_cfg_del(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_PULSECFG_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //通道OAD
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.port, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
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
* @brief：     从698格式报文转数据中心的数据(class12)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_act_pulse(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x03:
        {
            return buf_to_data_act_pulse_cfg_add(pOadInfo,  convert);
        }break;

        case 0x04:
        {
            return buf_to_data_act_pulse_cfg_del(pOadInfo,  convert);
        }break; 

        case 0x1C:
        {
            return buf_to_data_act_pulse_cfg_bigadd(pOadInfo,  convert);
        }break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：     从698格式报文转数据中心的数据(class12)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class12_act(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
        case 0x2401:
        case 0x2402:
        case 0x2403:
        case 0x2404:
        case 0x2405:
        case 0x2406:
        case 0x2407:
        case 0x2408:
        {
            return buf_to_data_act_pulse(pOadInfo,  convert);
        }break;
        
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/* 添加所有脉冲配置 */
int class_invoke_act_add_all_pulse(int cid, uint32 actOMD, OOP_PULSECFG_T *pdata)
{
    int    ret = 0;
    uint32 len = 0;
    uint32  i = 0;
    OOP_PULSESETS_B_T    dataList;
    OOP_PULSESETS_T      dataSList;
    NOMAL_OAD_T          normal;
    uint8                bSave = FALSE;

    memset(&normal, 0, sizeof(NOMAL_OAD_T));
    normal.oad.value = actOMD;
    normal.oad.attID = 0x1C;
    memset(&dataList, 0, sizeof(OOP_PULSESETS_B_T));
    ret = db_read_nomal(cid, &normal, sizeof(OOP_PULSESETS_B_T), (uint8*)&dataList, &len);
    if((ret != ERR_OK) || (len == 0))
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        memset(&dataList, 0, sizeof(OOP_PULSESETS_B_T));
    }

    for(i = 0; i < dataList.nNum; i++)
    {
        if(dataList.pulse[i].port.value == pdata->port.value)
        {
            dataList.pulse[i].property = pdata->property;
            dataList.pulse[i].K = pdata->K;
            bSave = TRUE;
            break;
        }
    }

    if(i == dataList.nNum)
    {
        if(dataList.nNum > OOP_MAX_PULSESETS)
        {
            return DATA_DATA_BLOCK_UNAVAILABLE;
        }
        dataList.nNum += 1;
        dataList.pulse[i].port.value = pdata->port.value;
        dataList.pulse[i].property = pdata->property;
        dataList.pulse[i].K = pdata->K;
        bSave = TRUE;
    }
    
    if(TRUE == bSave)
    {
        normal.classtag = CLASS_DATA_INIT;
        ret = db_write_nomal(cid, &normal, (uint8*)&dataList, sizeof(OOP_PULSESETS_B_T));
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return DATA_OTHER_REASON;
        }

        normal.oad.attID = 0x04;
        dataSList.nNum = dataList.nNum;
        dataSList.rsv = 0;
        for (i = 0; i < dataList.nNum; i++)
        {
            dataSList.pulse[i].port.value = dataList.pulse[i].port.value;
            dataSList.pulse[i].property = dataList.pulse[i].property;
            if(dataList.pulse[i].K >= 0xFFFF)
            {
                dataSList.rsv = 1;
                dataSList.pulse[i].K = 0xFFFF;
            }
            else
            {
                dataSList.pulse[i].K = (uint16)dataList.pulse[i].K;
            }
        }
        ret = db_write_nomal(cid, &normal, (uint8*)&dataSList, sizeof(OOP_PULSESETS_T));
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return DATA_OTHER_REASON;
        }
    }

    return ERR_OK;
}

/**
*********************************************************************
* @name：      class12_invoke_act
* @brief：     class12的ACTION服务
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
OOP_DAR_E class12_invoke_act_pluse_add(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class12_act(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x24010300:
        case 0x24020300:
        case 0x24030300:
        case 0x24040300:
        case 0x24050300:
        case 0x24060300:
        case 0x24070300:
        case 0x24080300:
        {
            OOP_PULSECFG_T     sdata;
            MEMZERO(&sdata, sizeof(sdata));
            memcpy(&sdata, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("class12_invoke_act_pluse_add : data.port.value = 0x%08x\n", sdata.port.value);
            PRTL_FMT_LOGCC("class12_invoke_act_pluse_add : data.property = %d\n", sdata.property);
            PRTL_FMT_LOGCC("class12_invoke_act_pluse_add : data.K = %d\n", sdata.K);

            if(sdata.port.value < 0xF20A0201 || sdata.port.value > 0xF20A0208)
            {
                return DATA_DATA_BLOCK_UNAVAILABLE;
            }

            /* 先添加大脉冲 */
            ret = class_invoke_act_add_all_pulse(pApduInfo->hUdp, pOadInfo->pTab->oad.value, &sdata);
            if(0 != ret)
            {
                return ret;
            }
            
            //配置总加组单元时，默认下发冻结配置
            ret = frozen_pulse_objattr_add(pApduInfo->hUdp, pOadInfo->pTab->oad);
            if(ret != 0)
            {
                PRTL_FMT_DEBUG("frozen_pulse_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, pOadInfo->pTab->oad.nObjID);
            }
            ret = extreme_pulse_objattr_add(pApduInfo->hUdp, pOadInfo->pTab->oad);
            if(ret != 0)
            {
                PRTL_FMT_DEBUG("extreme_pulse_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, pOadInfo->pTab->oad.nObjID);
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：      class12_invoke_act
* @brief：     class12的ACTION服务
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
OOP_DAR_E class12_invoke_act_pluse_bigadd(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal;
    uint8 tdata[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&tdata, sizeof(tdata)); 
    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&normal, sizeof(normal)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &tdata;
    convert.dstLen = sizeof(tdata);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class12_act(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.nObjID)
    {
        case 0x2401:
        case 0x2402:
        case 0x2403:
        case 0x2404:
        case 0x2405:
        case 0x2406:
        case 0x2407:
        case 0x2408:
        {
            OOP_PULSESETS_T      dataSList;
            OOP_PULSESETS_B_T    dataList;
            OOP_PULSECFG_B_T     data;
            uint32               len = 0;
            uint32               i = 0;
            BOOL                 bSave = FALSE;

            MEMZERO(&dataList, sizeof(dataList));
            MEMZERO(&dataSList, sizeof(dataSList));
            MEMZERO(&data, sizeof(data));

            memcpy(&data, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("class12_invoke_act_pluse_add : data.port.value = 0x%08x\n", data.port.value);
            PRTL_FMT_LOGCC("class12_invoke_act_pluse_add : data.property = %d\n", data.property);
            PRTL_FMT_LOGCC("class12_invoke_act_pluse_add : data.K = %d\n", data.K);

            if(data.port.value < 0xF20A0201 || data.port.value > 0xF20A0208)
            {
                return DATA_DATA_BLOCK_UNAVAILABLE;
            }

            normal.oad.nObjID = pOadInfo->pTab->oad.nObjID;
            normal.oad.attID = 0x1C;
            PRTL_FMT_LOGCC("db_read_nomal: normal.oad = 0x%08x\n", normal.oad.value);
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_PULSESETS_B_T), (uint8*)&dataList, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                               ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            PRTL_FMT_LOGCC("class12_invoke_act_pluse_add : dataList.nNum = %d\n", dataList.nNum);
            for (i = 0; i < dataList.nNum; i++)
            {
                PRTL_FMT_LOGCC("class12_invoke_act_pluse_add : dataList.pulse[%d].port.value = 0x%08x\n", i, dataList.pulse[i].port.value);
                PRTL_FMT_LOGCC("class12_invoke_act_pluse_add : dataList.pulse[%d].property = %d\n", i, dataList.pulse[i].property);
                PRTL_FMT_LOGCC("class12_invoke_act_pluse_add : dataList.pulse[%d].K = %d\n", i, dataList.pulse[i].K);

                //如果要添加的PORT已经存在，则修改其内容
                if (dataList.pulse[i].port.value == data.port.value)
                {
                    dataList.pulse[i] = data;
                    bSave = TRUE;
                    break;
                }
            }

            //没有找到相同的PORT，则往后添加
            if (i == dataList.nNum)
            {
                //数据超过最大范围
                if (dataList.nNum >= OOP_MAX_PULSESETS)
                {
                    return DATA_DATA_BLOCK_UNAVAILABLE;
                }
                dataList.pulse[i] = data;
                bSave = TRUE;
                dataList.nNum ++;
                PRTL_FMT_LOGCC("no same port class12_invoke_act_pluse_add : dataList.nNum = %d\n", dataList.nNum);
                for (i = 0; i < dataList.nNum; i++)
                {
                    PRTL_FMT_LOGCC("no same port class12_invoke_act_pluse_add : dataList.pulse[%d].port.value = 0x%08x\n", i, dataList.pulse[i].port.value);
                    PRTL_FMT_LOGCC("no same port class12_invoke_act_pluse_add : dataList.pulse[%d].property = %d\n", i, dataList.pulse[i].property);
                    PRTL_FMT_LOGCC("no same port class12_invoke_act_pluse_add : dataList.pulse[%d].K = %d\n", i, dataList.pulse[i].K);
                }
            }

            if (bSave)
            {
                normal.oad.nObjID = pOadInfo->pTab->oad.nObjID;
                normal.oad.attID = 0x1C;
                normal.classtag = CLASS_DATA_INIT;
                PRTL_FMT_LOGCC("db_read_nomal : normal.oad = 0x%08x\n", normal.oad.value);
                ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataList, sizeof(OOP_PULSESETS_B_T));
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, normal.logicId, normal.infoNum, normal.oad.value);
                    return DATA_OTHER_REASON;
                }

                normal.oad.attID = 0x04;
                dataSList.nNum = dataList.nNum;
                dataSList.rsv = 0;
                for (i = 0; i < dataList.nNum; i++)
                {
                    dataSList.pulse[i].port.value = dataList.pulse[i].port.value;
                    dataSList.pulse[i].property = dataList.pulse[i].property;
                    if(dataList.pulse[i].K >= 0xFFFF)
                    {
                        dataSList.rsv = 1;
                        dataSList.pulse[i].K = 0xFFFF;
                    }
                    else
                    {
                        dataSList.pulse[i].K = (uint16)dataList.pulse[i].K;
                    }
                }
                ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSList, sizeof(OOP_PULSESETS_T));
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, normal.logicId, normal.infoNum, normal.oad.value);
                    return DATA_OTHER_REASON;
                }
            }

            //配置总加组单元时，默认下发冻结配置
            ret = frozen_pulse_objattr_add(pApduInfo->hUdp, normal.oad);
            if(ret != 0)
            {
                PRTL_FMT_DEBUG("frozen_pulse_objattr_add failed. ret(%d), OI(0x%04x)\n", ret, normal.oad.nObjID);
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/* 删除大脉冲 */
int class_innvvoke_act_del_big_pulse(int cid, uint32 actOMD, uint32 delOAD)
{
    int    ret = 0;
    uint32 len = 0;
    uint32  i = 0;
    OOP_PULSESETS_B_T    dataList;
    OOP_PULSESETS_B_T    newdataList;
    NOMAL_OAD_T          normal;

    memset(&normal, 0, sizeof(NOMAL_OAD_T));
    normal.oad.value = actOMD;
    normal.oad.attID = 0x1C;
    memset(&dataList, 0, sizeof(OOP_PULSESETS_B_T));
    ret = db_read_nomal(cid, &normal, sizeof(OOP_PULSESETS_B_T), (uint8*)&dataList, &len);
    if((ret != ERR_OK) || (len == 0))
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ERR_OK;
    }

    memset(&newdataList, 0, sizeof(OOP_PULSESETS_B_T));
    for(i = 0; i < dataList.nNum; i++)
    {
        if(dataList.pulse[i].port.value != delOAD)
        {
            newdataList.pulse[newdataList.nNum] = dataList.pulse[i];
            newdataList.nNum += 1;
        }
    }
    if(0 != memcmp(&newdataList, &dataList, sizeof(OOP_PULSESETS_B_T)))
    {
        normal.classtag = CLASS_DATA_INIT;
        ret = db_write_nomal(cid, &normal, (uint8*)&newdataList, sizeof(OOP_PULSESETS_B_T));
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return DATA_OTHER_REASON;
        }
    }

    return 0;
}

/**
*********************************************************************
* @name：      class12_invoke_act
* @brief：     class12的ACTION服务
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
OOP_DAR_E class12_invoke_act_pluse_del(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&normal, sizeof(normal)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class12_act(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x24010400:
        case 0x24020400:
        case 0x24030400:
        case 0x24040400:
        case 0x24050400:
        case 0x24060400:
        case 0x24070400:
        case 0x24080400:
        {
            OOP_PULSESETS_T    dataList;
            OOP_OAD_U          data;
            uint32             len = 0;
            uint32             i = 0;
            uint32             j = 0;
            uint32             k = 0;
            BOOL               bSave = FALSE;

            MEMZERO(&dataList, sizeof(dataList));
            MEMZERO(&data, sizeof(data));

            memcpy(&data, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("class12_invoke_act_pluse_del : data.value = 0x%08x\n", data.value);

            /* 删除大脉冲 */
            ret = class_innvvoke_act_del_big_pulse(pApduInfo->hUdp, pOadInfo->pTab->oad.value, data.value);
            if(ret != ERR_OK)
            {
                return DATA_OTHER_REASON;
            }

            normal.oad.nObjID = pOadInfo->pTab->oad.nObjID;
            normal.oad.attID = 0x04;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_PULSESETS_T), (uint8*)&dataList, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            PRTL_FMT_LOGCC("class12_invoke_act_pluse_del : dataList.nNum = %d\n", dataList.nNum);
            for (i = 0; i < dataList.nNum; i++)
            {
                PRTL_FMT_LOGCC("class12_invoke_act_pluse_del : dataList.pulse[%d].port.value = 0x%08x\n", dataList.pulse[i].port.value);
                PRTL_FMT_LOGCC("class12_invoke_act_pluse_del : dataList.pulse[%d].property = %d\n", dataList.pulse[i].property);
                PRTL_FMT_LOGCC("class12_invoke_act_pluse_del : dataList.pulse[%d].K = %d\n", dataList.pulse[i].K);

                //删除相同的脉冲配置，然后后面的配置往前移到一位
                if (dataList.pulse[i].port.value == data.value)
                {
                    j = i + 1;
                    k = i;
                    while (j < dataList.nNum)
                    {
                        dataList.pulse[k] = dataList.pulse[j];
                        j ++;
                        k ++;
                    }

                    //总数减1
                    dataList.nNum --;
                    
                    bSave = TRUE;
                    break;
                }
            }

            if (i == dataList.nNum)
            {
                PRTL_FMT_LOGCC("class12_invoke_act_pluse_del : find no same port\n");
            }

            if (bSave)
            {
                normal.classtag = CLASS_DATA_INIT;
                ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataList, sizeof(OOP_PULSESETS_T));
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, normal.logicId, normal.infoNum, normal.oad.value);
                    return DATA_OTHER_REASON;
                }
            }

            //删除脉冲单元时，删除冻结配置
            if(dataList.nNum == 0)
            {
                ret = frozen_pulse_objattr_del(pApduInfo->hUdp, normal.oad);
                if(ret != 0)
                {
                    PRTL_FMT_DEBUG("frozen_pulse_objattr_del failed. ret(%d), OI(0x%04x)\n", ret, normal.oad.nObjID);
                }
                ret = extreme_pulse_objattr_del(pApduInfo->hUdp, normal.oad);
                if(ret != 0)
                {
                    PRTL_FMT_DEBUG("extreme_pulse_objattr_del failed. ret(%d), OI(0x%04x)\n", ret, normal.oad.nObjID);
                }
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       class12_invoke_act
* @brief：      class12的ACTION服务
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
OOP_DAR_E class12_invoke_act_pluse(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OMD_U omd;
    omd.value = pOadInfo->pTab->oad.value;

    switch (omd.nModID)
    {
        case 0x03:
            return class12_invoke_act_pluse_add(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        case 0x04:
            return class12_invoke_act_pluse_del(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        case 0x1C:
            return class12_invoke_act_pluse_bigadd(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        default:
            return DATA_OBJECT_UNAVAILABLE;
    }
}

#if DESC("class12对外接口函数",1)

/**
*********************************************************************
* @name：      class12_invoke_set
* @brief：     class12的SET服务
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
OOP_DAR_E class12_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x2401:
    case 0x2402:
    case 0x2403:
    case 0x2404:
    case 0x2405:
    case 0x2406:
    case 0x2407:
    case 0x2408:
        return class12_invoke_set_plus(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：      class12_invoke_get
* @brief：     class12的GET服务
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
OOP_DAR_E class12_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x2401:
    case 0x2402:
    case 0x2403:
    case 0x2404:
    case 0x2405:
    case 0x2406:
    case 0x2407:
    case 0x2408:
        return class12_invoke_get_pluse(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：       class12_invoke_act
* @brief：      class12的ACTION服务
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
OOP_DAR_E class12_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x2401:
    case 0x2402:
    case 0x2403:
    case 0x2404:
    case 0x2405:
    case 0x2406:
    case 0x2407:
    case 0x2408:
        return class12_invoke_act_pluse(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break; 
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}


#endif
