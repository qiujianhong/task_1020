/*
*********************************************************************
* @file    class22.c
* @brief： class22处理
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
#include "oopStorage.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"
#include "out_msg.h"
#include "ble.h"
#include "hal.h"
#include "devinfo.h"

//
// class22
//
extern ProxyLockQueue ProxyQueue;

#if DESC("class22 data to buf 接口函数",1)

/**
*********************************************************************
* @brief：      RS232设备对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_rs232(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SERIALPORT_T data;
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
        dstBuf[dOffset++] = 3;
                
        //端口描述符
        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.com[i].name, sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nPeriod(%d)\n", i, dar, data.com[i].name);
            return DATA_TYPE_UNMATCHED;
        }
        
        //端口参数
        dar = basic_data_to_buf(E_COMDCB, dstBuf, dstLen, &dOffset, &data.com[i].param, sizeof(OOP_COMDCB_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.com[i].param);
            return DATA_TYPE_UNMATCHED;
        }

        //端口功能
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.com[i].func, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nDepth(%d)\n", i, dar, data.com[i].func);
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
OOP_DAR_E data_to_buf_rs232_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SERIALPORT_T data;
    uint8 i = pOadInfo->nIndex - 1;
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
            
    //端口描述符
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.com[i].name, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nPeriod(%d)\n", i, dar, data.com[i].name);
        return DATA_TYPE_UNMATCHED;
    }
    
    //端口参数
    dar = basic_data_to_buf(E_COMDCB, dstBuf, dstLen, &dOffset, &data.com[i].param, sizeof(OOP_COMDCB_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.com[i].param);
        return DATA_TYPE_UNMATCHED;
    }

    //端口功能
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.com[i].func, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nDepth(%d)\n", i, dar, data.com[i].func);
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

OOP_DAR_E data_to_buf_hplc(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_ROUTEMODULE_T data;
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
        dstBuf[dOffset++] = 3;
                
        //端口描述符
        dar = basic_data_to_buf(E_OOP_VISIBLESTR256_T, dstBuf, dstLen, &dOffset, &data.item[i].name, sizeof(data.item[i].name));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d)\n", i, dar);
            return DATA_TYPE_UNMATCHED;
        }
        
        //端口参数
        dar = basic_data_to_buf(E_COMDCB, dstBuf, dstLen, &dOffset, &data.item[i].dcb, sizeof(OOP_COMDCB_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d)\n", i, dar);
            return DATA_TYPE_UNMATCHED;
        }
        dstBuf[dOffset++] = DT_STRUCTURE;
        //结构成员数
        dstBuf[dOffset++] = 4;

        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.item[i].verInfo.factory, sizeof(data.item[i].verInfo.factory));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d) \n", i, dar );
            return DATA_TYPE_UNMATCHED;
        }        
        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.item[i].verInfo.ic,sizeof(data.item[i].verInfo.ic));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d) \n", i, dar );
            return DATA_TYPE_UNMATCHED;
        }   
        dar = basic_data_to_buf(E_DATE, dstBuf, dstLen, &dOffset, &data.item[i].verInfo.dtVer,sizeof(data.item[i].verInfo.dtVer));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d) \n", i, dar );
            return DATA_TYPE_UNMATCHED;
        }   
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.item[i].verInfo.wVer,sizeof(data.item[i].verInfo.wVer));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d) \n", i, dar );
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

OOP_DAR_E data_to_buf_hplc_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_ROUTEMODULE_T data;

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
            
    //端口描述符
    //dar = basic_data_to_buf(E_OOP_VISIBLESTR256_T, dstBuf, dstLen, &dOffset, &data.item[i].name, sizeof(data.item[i].name));
    //if(dar != DATA_SUCCESS)
    //{
    //    PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d)\n", i, dar);
    //    return DATA_TYPE_UNMATCHED;
    //}
    dstBuf[dOffset++] = DT_NULL;
    
    //端口参数
    dar = basic_data_to_buf(E_COMDCB, dstBuf, dstLen, &dOffset, &data.item[i].dcb, sizeof(OOP_COMDCB_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d)\n", i, dar);
        return DATA_TYPE_UNMATCHED;
    }
    dstBuf[dOffset++] = DT_STRUCTURE;
    //结构成员数
    dstBuf[dOffset++] = 4;

    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.item[i].verInfo.factory, sizeof(data.item[i].verInfo.factory));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d) \n", i, dar );
        return DATA_TYPE_UNMATCHED;
    }        
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.item[i].verInfo.ic,sizeof(data.item[i].verInfo.ic));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d) \n", i, dar );
        return DATA_TYPE_UNMATCHED;
    }   
    dar = basic_data_to_buf(E_DATE, dstBuf, dstLen, &dOffset, &data.item[i].verInfo.dtVer,sizeof(data.item[i].verInfo.dtVer));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d) \n", i, dar );
        return DATA_TYPE_UNMATCHED;
    }   
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.item[i].verInfo.wVer,sizeof(data.item[i].verInfo.wVer));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d) \n", i, dar );
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
* @brief：      RS485设备对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_rs485(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    //同RS232
    return data_to_buf_rs232(pOadInfo, convert);
}

/**
*********************************************************************
* @brief：      RS232设备对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_ir(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_IR_T data;
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
                
        //端口描述符
        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.ir[i].name, sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nPeriod(%d)\n", i, dar, data.ir[i].name);
            return DATA_TYPE_UNMATCHED;
        }
        
        //端口参数
        dar = basic_data_to_buf(E_COMDCB, dstBuf, dstLen, &dOffset, &data.ir[i].param, sizeof(OOP_COMDCB_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.ir[i].param);
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
* @brief：      红外设备对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_infrared(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_IR_T data;
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
                
        //端口描述符
        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.ir[i].name, sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nPeriod(%d)\n", i, dar, data.ir[i].name);
            return DATA_TYPE_UNMATCHED;
        }
        
        //端口参数
        dar = basic_data_to_buf(E_COMDCB, dstBuf, dstLen, &dOffset, &data.ir[i].param, sizeof(OOP_COMDCB_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.ir[i].param);
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
* @brief：      开关量输入设备对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_input_state(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SWITCHIN_STATE_T data;
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
                
        //状态ST
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.state[i].st, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.state[i].st(%d)\n", i, dar, data.state[i].st);
            return DATA_TYPE_UNMATCHED;
        }
        
        //变位CD
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.state[i].cd, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.state[i].cd(%d)\n", i, dar, data.state[i].cd);
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
* @brief：      直流模拟量配置
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_dc_analog_flag(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_ANALOG_CONFIG_T data;
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
        dstBuf[dOffset++] = 3;
                
        //量程起始值
        dar = basic_data_to_buf(E_DOUBLE_LONG, dstBuf, dstLen, &dOffset, &data.param[i].startValue, sizeof(int32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nPeriod(%d)\n", i, dar, data.param[i].startValue);
            return DATA_TYPE_UNMATCHED;
        }
        
        //量程结束值
        dar = basic_data_to_buf(E_DOUBLE_LONG, dstBuf, dstLen, &dOffset, &data.param[i].endValue, sizeof(int32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.param[i].endValue);
            return DATA_TYPE_UNMATCHED;
        }    

        //单位换算
        dar = basic_data_to_buf(E_SCALER_UNIT, dstBuf, dstLen, &dOffset, &data.param[i].value, sizeof(OOP_SCALER_UNIT_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.unit3(%d)\n", i, dar, data.param[i].value);
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
* @brief：      开关量输入设备对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_input_flag(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SWITCHIN_FLAG_T data;
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
            
    //开关量接入标志
#if defined PRODUCT_ZCU_1
    dstBuf[dOffset++] = DT_BIT_STRING;
    dstBuf[dOffset++] = 8;
    dstBuf[dOffset] = data.inputFlag.bitValue;
    membitrev(dstBuf + dOffset, 8);
    dOffset++;

    dstBuf[dOffset++] = DT_BIT_STRING;
    dstBuf[dOffset++] = 8;
    dstBuf[dOffset] = data.attrFlag.bitValue;
    membitrev(dstBuf + dOffset, 8);
    dOffset++;
#else
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.inputFlag, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),inputFlag(0x%x)\n", dar, data.inputFlag);
        return DATA_TYPE_UNMATCHED;
    }
    
    //开关量属性标志
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.attrFlag, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),attrFlag(0x%x)\n", dar, data.attrFlag);
        return DATA_TYPE_UNMATCHED;
    }      
#endif
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
* @brief：      开关量输入设备对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_warn_out(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_WARNINGOUT_T data;
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
        //告警输出
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.state[i], sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.state[i].st(%d)\n", i, dar, data.state[i]);
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
* @brief：      继电器设备对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_switch(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SWITCHOUT_T data;
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
        dstBuf[dOffset++] = 4;
                
        //端口描述符
        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.value[i].name, sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nPeriod(%d)\n", i, dar, data.value[i].name);
            return DATA_TYPE_UNMATCHED;
        }
        
        //当前状态
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.value[i].current, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.value[i].current);
            return DATA_TYPE_UNMATCHED;
        } 

        //开关属性
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.value[i].attribute, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.value[i].attribute);
            return DATA_TYPE_UNMATCHED;
        }  

        //接线状态
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.value[i].input, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.value[i].input);
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
* @brief：      继电器设备对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_switch_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    SWITCHOUT data;
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
    dstBuf[dOffset++] = 4;
            
    //端口描述符
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.name, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nPeriod(%d)\n", i, dar, data.name);
        return DATA_TYPE_UNMATCHED;
    }
    
    //当前状态
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.current, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.current);
        return DATA_TYPE_UNMATCHED;
    } 

    //开关属性
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.attribute, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.attribute);
        return DATA_TYPE_UNMATCHED;
    }  

    //接线状态
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.input, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.input);
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
* @brief：      开关量输入设备对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_warn_period(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_WARNINGTIME_T data;
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
                
        //起始时间
        dar = basic_data_to_buf(E_TIME, dstBuf, dstLen, &dOffset, &data.period[i].startTime, sizeof(OOP_TIME_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.period[i].startTime(%d)\n", i, dar, data.period[i].startTime);
            return DATA_TYPE_UNMATCHED;
        }
        
        //结束时间
        dar = basic_data_to_buf(E_TIME, dstBuf, dstLen, &dOffset, &data.period[i].endTime, sizeof(OOP_TIME_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.speriod[i].endTime(%d)\n", i, dar, data.period[i].endTime);
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
* @brief：      本地设备对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_local_module_list(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_ROUTEMODULE_T data;
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
        dstBuf[dOffset++] = 3;
                
        //端口描述符
//        dar = basic_data_to_buf(E_OOP_VISIBLESTR256_T, dstBuf, dstLen, &dOffset, &data.item[i].name, sizeof(OOP_VISIBLEVAR256_T));
//        if(dar != DATA_SUCCESS)
//        {
//            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nPeriod(%d)\n", i, dar, data.item[i].name);
//            return DATA_TYPE_UNMATCHED;
//        }
        dstBuf[dOffset++] = DT_VISIBLE_STRING;
        offlen = set_len_offset(data.item[i].name.nNum, lenBuf);
        memcpy((uint8*)&dstBuf[dOffset],lenBuf,offlen);
        dOffset += offlen;
        memcpy((uint8*)&dstBuf[dOffset],data.item[i].name.value,data.item[i].name.nNum);
        dOffset += data.item[i].name.nNum;
        //端口参数
        dar = basic_data_to_buf(E_COMDCB, dstBuf, dstLen, &dOffset, &data.item[i].dcb, sizeof(OOP_COMDCB_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.item[i].dcb.baud(%d)\n", i, dar, data.item[i].dcb.baud);
            return DATA_TYPE_UNMATCHED;
        }

        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 4;  

        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.item[i].verInfo.factory, sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed11. [%d] dar(%d), sOffset(%d)\n", i, dar, dOffset);
            return DATA_TYPE_UNMATCHED;
        }    

        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.item[i].verInfo.ic, sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed22. [%d] dar(%d), sOffset(%d)\n", i, dar, dOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        dar = basic_data_to_buf(E_DATE, dstBuf, dstLen, &dOffset, &data.item[i].verInfo.dtVer, sizeof(OOP_DATE_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed33. [%d] dar(%d), sOffset(%d)\n", i, dar, dOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.item[i].verInfo.wVer, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed44. [%d] dar(%d), sOffset(%d)\n", i, dar, dOffset);
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
* @brief：      APP服务组件信息
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_reg_app_info(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_REG_APP_INFOS_T data;
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
        dstBuf[dOffset++] = 4;
                
        //组件名称
        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.item[i].appName, sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nPeriod(%d)\n", i, dar, data.item[i].appName);
            return DATA_TYPE_UNMATCHED;
        }
        
        //版本信息
        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.item[i].version, sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),nDepth(%d)\n", dar, data);
            return DATA_TYPE_UNMATCHED;
        } 

        //发布日期
        dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.item[i].dataTime, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //APP_ID
        if(basic_data_to_buf(E_OOP_OCTETVAR256_T, dstBuf, dstLen, &dOffset, &data.item[i].appID, sizeof(OOP_OCTETVAR256_T)) < 0)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
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
* @brief：      网络拓扑对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_net_topology_info(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_NETTOPOLOGYINFO_LIST_T data;
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
        dstBuf[dOffset++] = 0x05;
                
        //从节点地址
        dstBuf[dOffset++] = DT_OCTET_STRING;

        dstBuf[dOffset++] = 0x06;

        memcpy(&dstBuf[dOffset],data.item[i].addr.nValue,6);

        dOffset=dOffset+6;
        
        //节点类型
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.item[i].type, 1);
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.item[i].type(%d)\n", i, dar,data.item[i].type);
            return DATA_TYPE_UNMATCHED;
        }

        //节点标识
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.item[i].nid, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, dOffset);
            return DATA_TYPE_UNMATCHED;
        }    

        //代理节点标识
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.item[i].proxynid, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, dOffset);
            return DATA_TYPE_UNMATCHED;
        }   

        //节点标识
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.item[i].info, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, dOffset);
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
* @brief：      网络拓扑对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_net_topology(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_NETTOPOLOGYINFO_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 0x05;
            
    //从节点地址
    dstBuf[dOffset++] = DT_OCTET_STRING;

    dstBuf[dOffset++] = 0x06;

    memcpy(&dstBuf[dOffset],data.addr.nValue,6);

    dOffset=dOffset+6;

    //节点类型
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.type, 1);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),data.type(%d)\n", dar,data.type);
        return DATA_TYPE_UNMATCHED;
    }

    //节点标识
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nid, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, dOffset);
        return DATA_TYPE_UNMATCHED;
    }    

    //代理节点标识
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.proxynid, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, dOffset);
        return DATA_TYPE_UNMATCHED;
    }   

    //节点信息
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.info, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, dOffset);
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
* @brief：      网络拓扑对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_nets_info(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_NETSINFO_T data;
//    uint8 offlen = 0;      //数组数量长度
//    uint8 lenBuf[3] = {0}; //数组数量编码
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
    dstBuf[dOffset++] = 0x03;
            
    //从节点地址
    dstBuf[dOffset++] = DT_OCTET_STRING;

    dstBuf[dOffset++] = 0x03;

    //本节点网络标识号
    memcpy_r(&dstBuf[dOffset],data.nodenid.nValue,3);

    dOffset=dOffset+3;

    //本节点主节点地址
    dstBuf[dOffset++] = DT_OCTET_STRING;

    dstBuf[dOffset++] = 0x06;
    memcpy(&dstBuf[dOffset],data.addr.nValue,6);

    dOffset=dOffset+6;

    //邻居节点网络标识号
    dstBuf[dOffset++] = DT_ARRAY;

    dstBuf[dOffset++] = data.nNum;
    
    for(i=0;i<data.nNum;i++)
    {

        dstBuf[dOffset++] = DT_OCTET_STRING;

        dstBuf[dOffset++] = 0x03;
        
        memcpy_r(&dstBuf[dOffset],data.neighbour[i].nValue,3);

        dOffset=dOffset+3;
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
* @brief：      网络拓扑对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_broadband_frequency(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data;
//    uint8 offlen = 0;      //数组数量长度
//    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);


    //宽带载波频段序号
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, dOffset);
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
* @brief：      网络拓扑对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_across_area_flag(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data;
//    uint8 offlen = 0;      //数组数量长度
//    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);


     dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, dOffset);
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
* @brief：      载波节点信息
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_node_version_info(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_NODEVERSIONINFO_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data)); 

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 6;
            
    //节点地址
    dar = basic_data_to_buf(E_OCTET_STR6_T, dstBuf, dstLen, &dOffset, &data.addr, sizeof(OOP_OCTETSTR6_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),data.addr(%d)\n", dar, data.addr);
        return DATA_TYPE_UNMATCHED;
    }
    
    //节点类型
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.type, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),data.type(%d)\n", dar, data.type);
        return DATA_TYPE_UNMATCHED;
    }

    //软件版本号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wVer, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),data.wVer(%d)\n", dar, data.wVer);
        return DATA_TYPE_UNMATCHED;
    }        

    //运行区软件发布日期
    dar = basic_data_to_buf(E_DATE, dstBuf, dstLen, &dOffset, &data.dtVer, sizeof(OOP_DATE_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),data.dtVer(%d)\n", dar, data.dtVer);
        return DATA_TYPE_UNMATCHED;
    }     

    //模块厂商代码
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.factory, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),data.dtVer(%d)\n", dar, data.factory);
        return DATA_TYPE_UNMATCHED;
    }   

    //芯片代码
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.ic, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),data.dtVer(%d)\n", dar, data.ic);
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
* @brief：      网络拓扑对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_nets_base_info(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_NETBASEINFO_T data;
//    uint8 offlen = 0;      //数组数量长度
//    uint8 lenBuf[3] = {0}; //数组数量编码
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
    dstBuf[dOffset++] = 0x08;
            
    //网络节点总数
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wNetNum, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 
    //在线站点个数
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wOnLineNum, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //网络启动组网时长
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dwStartTime, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //组网时长
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wNetTime, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //信标周期
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nBeacon, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //路由周期
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wRoute, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //拓扑变更次数
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wTopCnt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

     //层级信息
    dstBuf[dOffset++] = DT_ARRAY;

    dstBuf[dOffset++] = data.nLevelNum;

    for(i=0;i<OOP_MAX_LEVELNUM;i++)
    {
        dstBuf[dOffset++] = DT_STRUCTURE;

        dstBuf[dOffset++] = 0x02;
        //所在层级
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.levelInfo[i].nLevel, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.levelInfo[i].wLevelNum, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
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
* @brief：      网络拓扑对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_chip_node(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CHILDNODE_T data;
//    uint8 offlen = 0;      //数组数量长度
//    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);



    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数 HPLC深化应用测试只有三项
    dstBuf[dOffset++] = 0x03;
            
    //从节点序号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.index, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //从节点通信地址
    dstBuf[dOffset++] = DT_OCTET_STRING;

    dstBuf[dOffset++] =data.add.len;

    memcpy(dstBuf+dOffset,data.add.add,data.add.len);

    dOffset=dOffset+data.add.len;

    //从节点描述符
    dstBuf[dOffset++] = DT_VISIBLE_STRING;

    dstBuf[dOffset++] =data.id.nNum;

    memcpy(dstBuf+dOffset,data.id.value,data.id.nNum);

    dOffset=dOffset+data.id.nNum;

//    //从节点响应时长
//    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wResponse, sizeof(uint16));
//    if(dar != DATA_SUCCESS)
//    {
//        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
//        return DATA_TYPE_UNMATCHED;
//    }
//    //从节点最近一次通信成功时间
//    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.dtLastOk, sizeof(OOP_DATETIME_S_T));
//    if(dar != DATA_SUCCESS)
//    {
//        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
//        return DATA_TYPE_UNMATCHED;
//    }
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
* @brief：      网络拓扑对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_refuse_node(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_REFUSESLAVE_T data;
//    uint8 offlen = 0;      //数组数量长度
//    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);


    for(i=0;i<data.nNum;i++)
    {
        //结构类型
        dstBuf[dOffset++] = DT_OCTET_STRING;

        //结构成员数 HPLC深化应用测试只有三项
        dstBuf[dOffset++] = 0x06;
                
        //从节点序号
        memcpy((uint8*)&dstBuf[dOffset],data.sa[i].nValue,6);

        dOffset = dOffset +6;
        
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
* @brief：      网络拓扑对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_node_phase_info(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_NODEPHASEINFO_T data;
//    uint8 offlen = 0;      //数组数量长度
//    uint8 lenBuf[3] = {0}; //数组数量编码
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
    dstBuf[dOffset++] = 0x03;
            
    //从节点序号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.index, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //从节点通信地址
    dstBuf[dOffset++] = DT_OCTET_STRING;

    dstBuf[dOffset++] =data.addr.nNum;

    memcpy(dstBuf+dOffset,data.addr.value,data.addr.nNum);

    dOffset=dOffset+data.addr.nNum;

    //从节点相位信息
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.info, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
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
* @brief：      网络拓扑对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_net_node_info(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_NETNODEINFO_T data;

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
    dstBuf[dOffset++] = 0x14;
            
    //从节点序号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.wIndex, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //从节点通信地址
    dstBuf[dOffset++] = DT_OCTET_STRING;

    dstBuf[dOffset++] =0x06;

    memcpy(dstBuf+dOffset,data.nodeAddr.nValue,6);

    dOffset=dOffset+6;
    //节点拓扑信息
    dstBuf[dOffset++] = DT_STRUCTURE;

    dstBuf[dOffset++] =0x03;
    //节点标识
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.topInfo.nid, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    //代理节点标识
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.topInfo.proxynid, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    //节点信息
    dstBuf[dOffset++] = DT_BIT_STRING;
    dstBuf[dOffset++] = 0x08;
    dstBuf[dOffset++] = data.topInfo.info.nValue;
    
     //网络信息
    dstBuf[dOffset++] = DT_BIT_STRING;
    dstBuf[dOffset++] = 0x08;
    dstBuf[dOffset++] = data.netInfo.nValue;
    
     //设备类型
    dstBuf[dOffset++] = DT_BIT_STRING;
    dstBuf[dOffset++] = 0x10;
    memcpy(&dstBuf[dOffset],&data.device,2);
    dOffset= dOffset+2;

    //相位信息
    dstBuf[dOffset++] = DT_BIT_STRING;
    dstBuf[dOffset++] = 0x08;
    dstBuf[dOffset++] = data.phase.nValue;

    //代理变更次数
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wProxyCnt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    //站点离线次数
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wOffLineCnt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    //站点离线时间
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dwOffLineTime, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    //站点离线最大时间
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dwMaxOffLine, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    //上行通信成功率
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dwSuccessRateUp, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    //下行通信成功率
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dwSuccessRateDw, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    //主版本号
    dstBuf[dOffset++] = DT_OCTET_STRING;
    dstBuf[dOffset++] = 0x03;
    memcpy(&dstBuf[dOffset],&data.device,3);
    dOffset= dOffset+3;
    //次版本号
    dstBuf[dOffset++] = DT_OCTET_STRING;
    dstBuf[dOffset++] = 0x02;
    memcpy(&dstBuf[dOffset],&data.device,2);
    dOffset= dOffset+2;
    //下一跳信息
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wSkipInfo, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    //信道类型
    dstBuf[dOffset++] = DT_BIT_STRING;
    dstBuf[dOffset++] = 0x10;
    memcpy(&dstBuf[dOffset],&data.channel,2);
    dOffset= dOffset+2;
    //规约类型
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nProtocol, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    //台区状态
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nAreaState, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    //台区地址
    dstBuf[dOffset++] = DT_OCTET_STRING;
    dstBuf[dOffset++] = 0x06;
    memcpy(&dstBuf[dOffset],data.areaAddr.nValue,6);
    dOffset= dOffset+6;
    for(i=0;i<data.nDownNum;i++)
    {
        dstBuf[dOffset++] = DT_STRUCTURE;
        dstBuf[dOffset++] = 0x03;
        //下接从节点地址
        dstBuf[dOffset++] = DT_OCTET_STRING;
        dstBuf[dOffset++] = 0x06;
        memcpy(&dstBuf[dOffset],data.downInfo[i].addr.nValue,6);
        dOffset= dOffset+6;
        //下接从节点通信协议类型
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.downInfo[i].nProtocol, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        //下接从节点设备类型
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.downInfo[i].nDevice, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
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
* @brief：      开关量输入设备对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_local_module_refresh_ti(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TI_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);
            
    dar = basic_data_to_buf(E_TI, dstBuf, dstLen, &dOffset, &data, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.period[i].data.value(%d)\n", i, dar, data.value);
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
* @brief：      直流模拟量对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_analog(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_ANALOG_T data;
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

        //模拟量值
        dar = basic_data_to_buf(E_DOUBLE_LONG, dstBuf, dstLen, &dOffset, &data.value[i], sizeof(int32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nPeriod(%d)\n", i, dar, data.value[i]);
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
* @brief：      直流模拟量对象列表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
// int32 data_to_buf_analog(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
// {

// }


/**
*********************************************************************
* @name：       evt_rectable_get
* @brief：      读事件记录表
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E dev_param_get(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2000200:
        return data_to_buf_rs232(pOadInfo, convert);
        break;
    case 0xF2010200:
        return data_to_buf_rs485(pOadInfo, convert);
        break;   
    case 0xF2030400:
        return data_to_buf_input_flag(pOadInfo, convert);
        break;   
    default:
        return DATA_OBJECT_UNDEFINED;
        break;
    }
}


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class22)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
// OOP_DAR_E data_to_buf_class22(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
// {
//     switch (pOadInfo->pTab->oad.attID)
//     {
//         case 2:   //设备对象列表
//         case 4:
//         {
//            return dev_param_get(pOadInfo,  convert);
//         }break;
        
//         //......
        
//         default:
//         {
//             return DATA_READ_WRITE_DENIED;
//         }break;
//     }

//     return DATA_SUCCESS;
// }
OOP_DAR_E data_to_buf_class22(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2000200:
    case 0xF2010200:
        if (pOadInfo->nIndex == 0)
        {
            return data_to_buf_rs232(pOadInfo, convert);
        }
        else
        {
            return data_to_buf_rs232_unit(pOadInfo, convert);
        }
        //return data_to_buf_rs232(pOadInfo, convert);
        break;
//    case 0xF2010200:
//        return data_to_buf_rs485(pOadInfo, convert);
//        break;   
    case 0xF2020200:
        return data_to_buf_ir(pOadInfo, convert);
        break; 
    case 0xF2030200:
        return data_to_buf_input_state(pOadInfo, convert);
        break; 
    case 0xF2030400:
    case 0xF2031800:
        return data_to_buf_input_flag(pOadInfo, convert);
        break;  
    case 0xF2040200:
        return data_to_buf_analog(pOadInfo, convert);
        break; 
    case 0xF2040400:
        return data_to_buf_dc_analog_flag(pOadInfo, convert);
        break;      
    case 0xF2060200:
        return data_to_buf_warn_out(pOadInfo, convert);
        break; 
    case 0xF2050200:
        if (pOadInfo->nIndex == 0)
        {
            return data_to_buf_switch(pOadInfo, convert);
        }
        else
        {
            return data_to_buf_switch_unit(pOadInfo, convert);
        }
        break; 
    case 0xF2060400:
        return data_to_buf_warn_period(pOadInfo, convert);
        break;
    case 0xF2090200:
        if (pOadInfo->nIndex == 0)
        {
            return data_to_buf_hplc(pOadInfo, convert);
        }
        else
        {
            return data_to_buf_hplc_unit(pOadInfo, convert);
        }
        break;
    default:
        return DATA_OBJECT_UNDEFINED;
        break;
    }

    return DATA_SUCCESS;
}

#endif


#if DESC("class22 buf to data 接口函数",1)
/**
*********************************************************************
* @brief：      冻结关联对象属性表
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
OOP_DAR_E buf_to_data_check_meter(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOPM_BLUETOOTHERRCHK_T data;
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
    if(srcBuf[sOffset++] != 6)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //端口号
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.oad, sizeof(data.oad));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //脉冲类型enum
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.nType, sizeof(data.nType));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //信道发送间隔
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.interval, sizeof(data.interval));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //校表发射功率
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.power, sizeof(data.power));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //电能表通信地址
    dar = basic_buf_to_data(E_OOP_OCTETVAR16_T, srcBuf, srcLen, &sOffset, &data.addr, sizeof(data.addr));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构类型
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    data.nNum = srcBuf[sOffset++];

    for (i = 0; i < MIN(data.nNum, 5); i++)
    {
        //发射频率
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.nFreq[i], sizeof(data.nFreq[i]));
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
* @brief：      冻结关联对象属性表
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
OOP_DAR_E buf_to_data_rs232(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOPM_SERIALPORT_CONFIG_T data;
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
    if(srcBuf[sOffset++] != 3)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //端口号
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.port, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //端口参数
    dar = basic_buf_to_data(E_COMDCB, srcBuf, srcLen, &sOffset, &data.param, sizeof(OOP_COMDCB_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //端口功能
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.func, sizeof(uint8));
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

    PRTL_FMT_LOGCC("buf_to_data_rs232 : data.port.value = 0x%08x\n", data.port.value);
    PRTL_FMT_LOGCC("buf_to_data_rs232 : data.param.baud = %d\n", data.param.baud);
    PRTL_FMT_LOGCC("buf_to_data_rs232 : data.param.parity = %d\n", data.param.parity);
    PRTL_FMT_LOGCC("buf_to_data_rs232 : data.param.databits = %d\n", data.param.databits);
    PRTL_FMT_LOGCC("buf_to_data_rs232 : data.param.stopbits = %d\n", data.param.stopbits);
    PRTL_FMT_LOGCC("buf_to_data_rs232 : data.param.flowctrl = %d\n", data.param.flowctrl);
    PRTL_FMT_LOGCC("buf_to_data_rs232 : data.func = %d\n", data.func);
     
    return dar;
}

/**
*********************************************************************
* @brief：      冻结关联对象属性表
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
OOP_DAR_E buf_to_data_rs485(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOPM_SERIALPORT_CONFIG_T data;
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
    if(srcBuf[sOffset++] != 3)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //端口号
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.port, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //端口参数
    dar = basic_buf_to_data(E_COMDCB, srcBuf, srcLen, &sOffset, &data.param, sizeof(OOP_COMDCB_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //端口功能
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.func, sizeof(uint8));
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

    PRTL_FMT_LOGCC("buf_to_data_rs485 : data.port.value = 0x%08x\n", data.port.value);
    PRTL_FMT_LOGCC("buf_to_data_rs485 : data.param.baud = %d\n", data.param.baud);
    PRTL_FMT_LOGCC("buf_to_data_rs485 : data.param.parity = %d\n", data.param.parity);
    PRTL_FMT_LOGCC("buf_to_data_rs485 : data.param.databits = %d\n", data.param.databits);
    PRTL_FMT_LOGCC("buf_to_data_rs485 : data.param.stopbits = %d\n", data.param.stopbits);
    PRTL_FMT_LOGCC("buf_to_data_rs485 : data.param.flowctrl = %d\n", data.param.flowctrl);
    PRTL_FMT_LOGCC("buf_to_data_rs485 : data.func = %d\n", data.func);
     
    return dar;
}

/**
*********************************************************************
* @brief：     buf_to_data_ir
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
OOP_DAR_E buf_to_data_ir(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOPM_IR_CONFIG_T data;
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
    
    //端口号
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.oad, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //端口参数
    dar = basic_buf_to_data(E_COMDCB, srcBuf, srcLen, &sOffset, &data.param, sizeof(OOP_COMDCB_T));
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

    PRTL_FMT_LOGCC("buf_to_data_ir : data.port.value = 0x%08x\n", data.oad.value);
    PRTL_FMT_LOGCC("buf_to_data_ir : data.param.baud = %d\n", data.param.baud);
    PRTL_FMT_LOGCC("buf_to_data_ir : data.param.parity = %d\n", data.param.parity);
    PRTL_FMT_LOGCC("buf_to_data_ir : data.param.databits = %d\n", data.param.databits);
    PRTL_FMT_LOGCC("buf_to_data_ir : data.param.stopbits = %d\n", data.param.stopbits);
    PRTL_FMT_LOGCC("buf_to_data_ir : data.param.flowctrl = %d\n", data.param.flowctrl);
     
    return dar;
}


/**
*********************************************************************
* @brief：     buf_to_data_ir
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
OOP_DAR_E buf_to_data_switch(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOPM_SWITCHOUT_127_T data;
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
    
    //端口号
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.idx, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //端口参数
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.property, sizeof(uint8));
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

    PRTL_FMT_LOGCC("buf_to_data_switch : data.data.idx.value = 0x%08x\n", data.idx.value);
    PRTL_FMT_LOGCC("buf_to_data_switch : data.property = %d\n", data.property);
     
    return dar;
}

/**
*********************************************************************
* @brief：      冻结关联对象属性表
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
OOP_DAR_E buf_to_data_input_flag(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SWITCHIN_FLAG_T data;
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

    //开关量接入标志
#ifdef PRODUCT_ZCU_1
    if(srcBuf[sOffset] != DT_BIT_STRING || srcBuf[sOffset + 1] > OOP_MAX_SWITCHIN)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    sOffset++;
    if(srcBuf[sOffset++] <= 8)
    {
        uint8 temp = srcBuf[sOffset++];
        membitrev(&temp, 8);
        data.inputFlag.bitValue = temp;
    }
    else
    {
        uint16 temp = 0;
        memcpy(&temp, srcBuf + sOffset, 2);
        membitrev((uint8*)&temp, 16);
        data.inputFlag.bitValue = temp;
        sOffset += 2;
    }
#else
    dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &data.inputFlag, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
#endif

    //开关量属性标志
#ifdef PRODUCT_ZCU_1
    if(srcBuf[sOffset] != DT_BIT_STRING || srcBuf[sOffset + 1] > OOP_MAX_SWITCHIN)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    sOffset++;
    if(srcBuf[sOffset++] <= 8)
    {
        uint8 temp = srcBuf[sOffset++];
        membitrev(&temp, 8);
        data.attrFlag.bitValue = temp;
    }
    else
    {
        uint16 temp = 0;
        memcpy(&temp, srcBuf + sOffset, 2);
        membitrev((uint8*)&temp, 16);
        data.attrFlag.bitValue = temp;
        sOffset += 2;
    }
#else
    dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &data.attrFlag, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
#endif
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

    PRTL_FMT_LOGCC("buf_to_data_input_flag : data.inputFlag = 0x%x\n", data.inputFlag);
    PRTL_FMT_LOGCC("buf_to_data_input_flag : data.attrFlag = 0x%x\n", data.attrFlag);
     
    return dar;
}

/**
*********************************************************************
* @brief：      buf_to_data_dc_analog_flag
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
OOP_DAR_E buf_to_data_dc_analog_flag(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_ANALOG_CONFIG_T data = {0};
    uint8 offlen = 0;      //数组数量长度
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //结构类型
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组成员个数
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    if (data.nNum > OOP_MAX_DCANALOG)
    {
        PRTL_FMT_DEBUG("data.nNum[%d] > OOP_MAX_DCANALOG[%d]\n", data.nNum, OOP_MAX_DCANALOG);
        return DATA_TYPE_UNMATCHED;
    }

    for(i = 0; i < data.nNum; i++)
    {
        //结构类型
        if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //结构成员数
        if(srcBuf[sOffset++] != 3)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }  
        
        //量程起始值
        dar = basic_buf_to_data(E_DOUBLE_LONG, srcBuf, srcLen, &sOffset, &data.param[i].startValue, sizeof(int32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        
        //量程结束值
        dar = basic_buf_to_data(E_DOUBLE_LONG, srcBuf, srcLen, &sOffset, &data.param[i].endValue, sizeof(int32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //单位换算
        dar = basic_buf_to_data(E_SCALER_UNIT, srcBuf, srcLen, &sOffset, &data.param[i].value, sizeof(OOP_SCALER_UNIT_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
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
* @brief：     允许告警时段
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
OOP_DAR_E buf_to_data_warn_period(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    uint32 i = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_WARNINGTIME_T data;
    uint8 offlen = 0;      //数组数量长度
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
        
        //开始时间
        dar = basic_buf_to_data(E_TIME, srcBuf, srcLen, &sOffset, &data.period[i].startTime, sizeof(data.period[i].startTime));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        
        //结束时间
        dar = basic_buf_to_data(E_TIME, srcBuf, srcLen, &sOffset, &data.period[i].endTime, sizeof(data.period[i].endTime));
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
* @brief：     设置对象列表刷新周期
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
OOP_DAR_E buf_to_data_class22_local_module_refresh_ti(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    uint32 i = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TI_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //开始时间
    dar = basic_buf_to_data(E_TI, srcBuf, srcLen, &sOffset, &data, sizeof(data));
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
* @brief：     设置对象列表刷新周期
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
OOP_DAR_E buf_to_data_class22_across_area_flag(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    uint32 i = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data[2];
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //开始时间
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data, 2);
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
* @brief：     设置对象列表刷新周期
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
OOP_DAR_E buf_to_data_class22_refuse_node_report(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    uint32 i = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data[2];
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //开始时间
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data, 2);
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
* @brief：     设置对象列表刷新周期
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
OOP_DAR_E buf_to_data_class22_broadband_frequency(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    uint32 i = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data[2];
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //开始时间
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data, 2);
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
* @brief：      端口授权结构
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
OOP_DAR_E buf_to_data_port_auth(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOPM_ADDPORT_T data;
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
    
    //端口序号
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.index, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //授权时长
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.times, sizeof(uint8));
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
* @name：       evt_rectable_get
* @brief：      读事件记录表
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E dev_param_act(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2007F00:
        return buf_to_data_rs232(pOadInfo, convert);
        break;
    case 0xF2017F00:
        return buf_to_data_rs485(pOadInfo, convert);
        break;
    case 0xF2027F00:
        return buf_to_data_ir(pOadInfo, convert);
        break;
    case 0xF2057F00:
        return buf_to_data_switch(pOadInfo, convert);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @brief：     从698格式报文转数据中心的数据(class22)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_class22_act(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_OMD_U omd = (OOP_OMD_U )(pOadInfo->pTab->oad.value);
    switch (omd.nModID)
    {
        case 3:   //端口授权
        {
            return buf_to_data_port_auth(pOadInfo,  convert);
        }break;
        
        case 127:
        {
            return dev_param_act(pOadInfo,  convert);
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
* @brief：     从698格式报文转数据中心的数据(class22)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_class22_set(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0xF2030400:
        case 0xF2031800:
            return buf_to_data_input_flag(pOadInfo, convert);
        case 0xF2040400:
            return buf_to_data_dc_analog_flag(pOadInfo, convert);
        case 0xF2060400:
            return buf_to_data_warn_period(pOadInfo, convert);
        default:
            return DATA_OBJECT_UNAVAILABLE;
    }
    return DATA_SUCCESS;
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
OOP_DAR_E buf_to_data_class22(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}

#endif


/**
*********************************************************************
* @name：       class22_invoke_act
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_set_input(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    dar = buf_to_data_class22_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
        case 0xF2030400:
        {
            uint8 i = 0;
            uint8 Num = 0;
            uint8  maxport = 8;
            OOP_SWITCHIN_FLAG_T dataSet;
            
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = 0xF2030400;

#ifdef PRODUCT_ZCU_1
            uint32 len = 0;
            OOP_SWITCHIN_FLAG_T dataOld;
            memset(&dataOld, 0, sizeof(dataOld));
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_SWITCHIN_FLAG_T), (uint8*)&dataOld, &len);
            if(ret == ERR_OK && len == sizeof(OOP_SWITCHIN_FLAG_T))
            {
                uint16 temp = 0;
                temp = (dataSet.inputFlag.bitValue & 0x00FF) + (dataOld.inputFlag.bitValue & 0xFF00);
                dataSet.inputFlag.bitValue = temp;
                temp = (dataSet.attrFlag.bitValue & 0x00FF) + (dataOld.attrFlag.bitValue & 0xFF00);
                dataSet.attrFlag.bitValue = temp;
            }
            maxport = 16;
#endif

            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_SWITCHIN_FLAG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }

            for(i = 0; i < maxport; i++)
            {
                if(dataSet.inputFlag.bitValue & (0x01 < i))
                {
                    Num++;
                }
            }
            normal.oad.value = 0xF2030300;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, &Num, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
            break;
        }
        case 0xF2031800:
        {
            uint8 i = 0;
            uint8 Num = 0;
            OOP_SWITCHIN_FLAG_T dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);

#ifdef PRODUCT_ZCU_1
            uint32 len = 0;
            OOP_SWITCHIN_FLAG_T dataOld;
            memset(&dataOld, 0, sizeof(dataOld));
            normal.oad.value = 0xF2030400;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_SWITCHIN_FLAG_T), (uint8*)&dataOld, &len);
            if(ret != ERR_OK || len != sizeof(OOP_SWITCHIN_FLAG_T))
            {
                memset(&dataOld, 0, sizeof(dataOld));
            }
            uint16 temp = 0;
            temp = ((dataSet.inputFlag.bitValue << 8) & 0xFF00) + (dataOld.inputFlag.bitValue & 0x00FF);
            dataOld.inputFlag.bitValue = temp;
            temp = ((dataSet.attrFlag.bitValue << 8) & 0xFF00) + (dataOld.attrFlag.bitValue & 0x00FF);
            dataOld.attrFlag.bitValue = temp;
            
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataOld, sizeof(OOP_SWITCHIN_FLAG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            for(i = 0; i < 8; i++)
            {
                if(dataOld.inputFlag.bitValue & (0x01 < i))
                {
                    Num++;
                }
            }
#endif
            normal.oad.value = 0xF2031800;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_SWITCHIN_FLAG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }

            for(i = 0; i < 8; i++)
            {
                if(dataSet.inputFlag.bitValue & (0x01 < i))
                {
                    Num++;
                }
            }
            normal.oad.value = 0xF2030300;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, &Num, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
            break;
        }
        default: return DATA_OBJECT_UNAVAILABLE;
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
* @name：       class22_invoke_act
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_set_dc_analog(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int i = 0;
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
    dar = buf_to_data_class22_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2040400:
        {
            OOP_ANALOG_CONFIG_T dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = 0xF2040400;
            normal.classtag = CLASS_DATA_INIT;

            for (i = 0; i < dataSet.nNum; i++)
            {
                PRTL_FMT_LOGCC("class22_invoke_set_dc_analog : dataSet.param[%d].startValue = %d\n", i, dataSet.param[i].startValue);
                PRTL_FMT_LOGCC("class22_invoke_set_dc_analog : dataSet.param[%d].endValue = %d\n", i, dataSet.param[i].endValue);
                PRTL_FMT_LOGCC("class22_invoke_set_dc_analog : dataSet.param[%d].value.scaler = %d\n", i, dataSet.param[i].value.scaler);
                PRTL_FMT_LOGCC("class22_invoke_set_dc_analog : dataSet.param[%d].value.unit = %d\n", i, dataSet.param[i].value.unit);
            }

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_ANALOG_CONFIG_T));
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
* @name：       class22_invoke_set_warn_period
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_set_warn_period(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    dar = buf_to_data_class22_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2060400:
        {
            OOP_WARNINGTIME_T dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = 0xF2060400;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class22_invoke_set_warn_period : dataSet.nNum = 0x%x\n", dataSet.nNum);

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_WARNINGTIME_T));
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
* @name：       class22_invoke_set_warn_period
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_set_local_module_refresh_ti(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    dar = buf_to_data_class22_local_module_refresh_ti(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2090600:
        {
            OOP_TI_T dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = 0xF2090600;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class22_invoke_set_local_module_refresh_ti : dataSet.value = 0x%x\n", dataSet.value);
            PRTL_FMT_LOGCC("class22_invoke_set_local_module_refresh_ti : dataSet.unit = 0x%x\n", dataSet.unit);

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_TI_T));
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
* @name：       class22_invoke_set_warn_period
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_set_across_area_flag(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    dar = buf_to_data_class22_across_area_flag(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2090B00:
        {
            uint8 dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = 0xF2090B00;
            normal.classtag = CLASS_DATA_INIT;

            
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(dataSet));
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
* @name：       class22_invoke_set_warn_period
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_set_refuse_node_report(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    dar = buf_to_data_class22_refuse_node_report(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2091600:
        {
            uint8 dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = 0xF2091600;
            normal.classtag = CLASS_DATA_INIT;

            
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(dataSet));
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
* @name：       class22_invoke_set_warn_period
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_set_broadband_frequency(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    dar = buf_to_data_class22_broadband_frequency(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2090900:
        {
            uint8 dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = 0xF2090900;
            normal.classtag = CLASS_DATA_INIT;

            
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(dataSet));
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
* @name：       class22_invoke_set_local_module
* @brief：      class22的SET服务
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
OOP_DAR_E class22_invoke_set_local_module(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    { 
    case 0xF2090600:
        return class22_invoke_set_local_module_refresh_ti(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
   case 0xF2090B00:
        return class22_invoke_set_across_area_flag(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
   case 0xF2090900:
        return class22_invoke_set_broadband_frequency(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2091600:
        return class22_invoke_set_refuse_node_report(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name：       class22_invoke_set_F20B0600
* @brief：      class22的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2021-11-25
*********************************************************************
*/
#if 0
OOP_DAR_E class22_invoke_set_F20B0B00(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = 0;
    int offset = 0;
    int sOffset = 4;
    
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8 sn[BLE_SN_MAX] = {0};
    uint32 snLen = 0;

    uint8  sendbuf[128] = {0};
    uint16 sendlen = 0;
    uint8  recvbuf[4] = {0};
    uint16 recvlen = 0;

    if (inData[sOffset++] != DT_OCTET_STRING)
    {
        return DATA_TYPE_UNMATCHED;
    }

    snLen = inData[sOffset++];
    if (snLen > BLE_SN_MAX)
    {
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    memcpy(sn, &inData[sOffset], snLen);
    sOffset = 0;

    sendbuf[sOffset++] = snLen;
    memcpy(&sendbuf[sOffset], sn, snLen);
    sOffset += snLen;

    sendlen = sOffset;
    
    ret = appmain_set_bluetooth_sn(sendbuf,sendlen, recvbuf, 4, &recvlen);
    if ((ret != 0) && (recvbuf[0] != 1))
    {
        return DATA_READ_WRITE_DENIED;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}
#else
//之前理解不对，台体下发的F20B0B00不是SN号，是另一个号，下发到终端，后面台体再读出来跟SN进行校验
//对终端来说，只需要把这个号存下来即可
OOP_DAR_E class22_invoke_set_F20B0B00(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = 0;
    int offset = 0;
    int sOffset = 4;
    
    OOP_DAR_E dar = DATA_SUCCESS;

    BLE_SN_T tSn = {0};
    uint32   snLen = 0;

    NOMAL_OAD_T normal = {0};

    if (inData[sOffset++] != DT_OCTET_STRING)
    {
        return DATA_TYPE_UNMATCHED;
    }

    snLen = inData[sOffset++];
    if (snLen > BLE_SN_MAX)
    {
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    tSn.num = snLen;
    memcpy(tSn.sn, &inData[sOffset], snLen);
    PRTL_BUF_DEBUG(tSn.sn, MIN(BLE_SN_MAX, tSn.num), "设置蓝牙CSN");

    normal.oad.value = 0xF20B0B00;
    normal.classtag = CLASS_DATA_UNINIT;
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&tSn, sizeof(tSn));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}
#endif

/**
*********************************************************************
* @name：       class22_invoke_set_local_module
* @brief：      class22的SET服务
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
OOP_DAR_E class22_invoke_set_bluetooth(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    { 
        case 0xF20B0B00:
            return class22_invoke_set_F20B0B00(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        default:
            return DATA_OBJECT_UNAVAILABLE;
    }
}

/**
*********************************************************************
* @name：       class22_invoke_set
* @brief：      class22的SET服务
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
OOP_DAR_E class22_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0xF203:
        return class22_invoke_set_input(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;   
    case 0xF204:
        return class22_invoke_set_dc_analog(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    case 0xF206:
        return class22_invoke_set_warn_period(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    case 0xF209:
        return class22_invoke_set_local_module(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    case 0xF20B:
        return class22_invoke_set_bluetooth(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_rs232(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2000200:
        {
            SERIALPORT dataGet;
            OOP_SERIALPORT_T dataList;
            memset(&dataGet, 0, sizeof(dataGet));
            memset(&dataList, 0, sizeof(dataList));
            uint32 i = 0;
            for (i = 0; i < 3; i++)
            {
                normal.oad.value = 0xF2000201 + i;
                ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(SERIALPORT), (uint8*)&dataGet, &len);
                if((ret != ERR_OK) || (len == 0))
                {
                    PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, normal.logicId, normal.infoNum, normal.oad.value);
                    return DATA_OTHER_REASON;
                }
                dataList.com[i] = dataGet;

                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.portname = %s\n", dataGet.name.value);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.baud = %d\n", dataGet.param.baud);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.parity = %d\n", dataGet.param.parity);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.databits = %d\n", dataGet.param.databits);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.stopbits = %d\n", dataGet.param.stopbits);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.flowctrl = %d\n", dataGet.param.flowctrl);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.func = %d\n", dataGet.func); 

            }
            dataList.nNum = 0x03;

            //转换成报文数据
            convert.srcBuf = &dataList;
            convert.srcLen = sizeof(dataList);
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
    dar = data_to_buf_class22(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class22 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_rs485(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2010200:
        {
            SERIALPORT dataGet;
            OOP_SERIALPORT_T dataList;
            memset(&dataGet, 0, sizeof(dataGet));
            memset(&dataList, 0, sizeof(dataList));
            uint32 i = 0;
            for (i = 0; i < 3; i++)
            {
                normal.oad.value = 0xF2010201 + i;
                ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(SERIALPORT), (uint8*)&dataGet, &len);
                if((ret != ERR_OK) || (len == 0))
                {
                    PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, normal.logicId, normal.infoNum, normal.oad.value);
                    return DATA_OTHER_REASON;
                }
                dataList.com[i] = dataGet;

                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.portname = %s\n", dataGet.name.value);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.baud = %d\n", dataGet.param.baud);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.parity = %d\n", dataGet.param.parity);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.databits = %d\n", dataGet.param.databits);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.stopbits = %d\n", dataGet.param.stopbits);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.flowctrl = %d\n", dataGet.param.flowctrl);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.func = %d\n", dataGet.func); 

            }
            dataList.nNum = 0x03;

            //转换成报文数据
            convert.srcBuf = &dataList;
            convert.srcLen = sizeof(dataList);
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
    dar = data_to_buf_class22(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class22 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_ir(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2020200:
        {
            OOP_PORTCFG_T dataGet;
            OOP_IR_T dataList;
            memset(&dataGet, 0, sizeof(dataGet));
            memset(&dataList, 0, sizeof(dataList));
            uint32 i = 0;
            for (i = 0; i < OOP_MAX_IR; i++)
            {
                normal.oad.value = 0xF2020201 + i;
                ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_PORTCFG_T), (uint8*)&dataGet, &len);
                if((ret != ERR_OK) || (len == 0))
                {
                    PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, normal.logicId, normal.infoNum, normal.oad.value);
                    return DATA_OTHER_REASON;
                }
                dataList.ir[i] = dataGet;

                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.portname = %s\n", dataGet.name.value);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.baud = %d\n", dataGet.param.baud);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.parity = %d\n", dataGet.param.parity);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.databits = %d\n", dataGet.param.databits);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.stopbits = %d\n", dataGet.param.stopbits);
                PRTL_FMT_LOGCC("class22_invoke_get : dataGet.param.flowctrl = %d\n", dataGet.param.flowctrl);

            }
            dataList.nNum = 0x01;

            //转换成报文数据
            convert.srcBuf = &dataList;
            convert.srcLen = sizeof(dataList);
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
    dar = data_to_buf_class22(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class22 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_input(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    switch (pOadInfo->pTab->oad.value)
    {
        case 0xF2030200:
        {
            OOP_SWITCHIN_STATE_T dataList;
            OOP_SWITCHIN_STATE_T dataListTmp;

            memset(&dataList, 0, sizeof(dataList));

            normal.oad.value = 0xF2030200;
            BOOL   ifClearCD = FALSE;
            uint32 i = 0;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_SWITCHIN_STATE_T), (uint8*)&dataList, &len);
            if ((ret != ERR_OK) || (len == 0))
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

            //dataList.nNum目前固定是8路
            PRTL_FMT_LOGCC("class22_invoke_get_input : dataList.nNum = %d\n", dataList.nNum);
            dataListTmp = dataList;
            for (i = 0; i < dataListTmp.nNum; i++)
            {
                PRTL_FMT_LOGCC("class22_invoke_get_input : dataList.state[%d].st = %d\n", i, dataListTmp.state[i].st);
                PRTL_FMT_LOGCC("class22_invoke_get_input : dataList.state[%d].cd = %d\n", i, dataListTmp.state[i].cd);
                if (dataListTmp.state[i].cd == 1)
                {
                    dataListTmp.state[i].cd = 0;
                    ifClearCD = TRUE;
                }
            }

#if defined PRODUCT_CCU
            dataList.nNum = 2;
#elif defined PRODUCT_ZCU_1
    #ifdef AREA_JIANGSU
            dataList.nNum = 16;
    #else
            dataList.nNum = 8;
    #endif
#else
            dataList.nNum = 4;
#endif
            //转换成报文数据
            convert.srcBuf = &dataList;
            convert.srcLen = sizeof(dataList);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;

            //清遥信变位信息
            if (ifClearCD)
            {
                ifClearCD = FALSE;
                normal.classtag = CLASS_DATA_NORMAL;
                normal.oad.value = 0xF2030200;
                ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataListTmp, sizeof(OOP_SWITCHIN_STATE_T));
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, normal.logicId, normal.infoNum, normal.oad.value);
                    return DATA_OTHER_REASON;
                }
            }
            break;
        }
        case 0xF2030400:
        {
            OOP_SWITCHIN_FLAG_T data;
            memset(&data, 0, sizeof(data));
            normal.oad.value = 0xF2030400;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_SWITCHIN_FLAG_T), (uint8*)&data, &len);
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

            PRTL_FMT_LOGCC("class22_invoke_get_input : membitrev:data.attrFlag = 0x%x\n", data.attrFlag);
            PRTL_FMT_LOGCC("class22_invoke_get_input : membitrev:data.inputFlag = 0x%x\n", data.inputFlag);

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
            break;
        }
        case 0xF2030600:
        {
            OOP_STATUS_SIGNAL_T data;
            memset(&data, 0, sizeof(data));
            normal.oad.value = 0xF2030600;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_STATUS_SIGNAL_T), (uint8*)&data, &len);
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

            memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset++] = 0x01;
            outData->pResultData[offset++] = DT_STRUCTURE;
            outData->pResultData[offset++] = 2;
            outData->pResultData[offset++] = DT_BIT_STRING;
            outData->pResultData[offset++] = data.nNum;
            outData->pResultData[offset++] = data.ST.nValue;
            membitrev(outData->pResultData + offset, data.nNum);
            outData->pResultData[offset++] = DT_BIT_STRING;
            outData->pResultData[offset++] = data.nNum;
            outData->pResultData[offset++] = data.CT.nValue;
            membitrev(outData->pResultData + offset, data.nNum);

            /*清空变位*/
            data.CT.nValue = 0;
            normal.classtag = CLASS_DATA_NORMAL;
            db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_STATUS_SIGNAL_T));

            outData->resultDataLen += offset;
            return dar;
        }
	    case 0xF2031800:
        {
            OOP_SWITCHIN_FLAG_T data;
            memset(&data, 0, sizeof(data));
            normal.oad.value = 0xF2031800;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_SWITCHIN_FLAG_T), (uint8*)&data, &len);
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

            membitrev((uint8*)&data.inputFlag.bitValue, sizeof(data.inputFlag));
            membitrev((uint8*)&data.attrFlag.bitValue, sizeof(data.attrFlag));

            PRTL_FMT_LOGCC("class22_invoke_get_input : membitrev:data.attrFlag = 0x%x\n", data.attrFlag);
            PRTL_FMT_LOGCC("class22_invoke_get_input : membitrev:data.inputFlag = 0x%x\n", data.inputFlag);

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
            break;   
        }
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class22(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class22 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_analog(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2040200:
        {
            OOP_ANALOG_T dataList;
            memset(&dataList, 0, sizeof(dataList));
            normal.oad.value = 0xF2040200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_ANALOG_T), (uint8*)&dataList, &len);
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
            
            //转换成报文数据
            convert.srcBuf = &dataList;
            convert.srcLen = sizeof(dataList);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;

            PRTL_FMT_TRACE("class22_invoke_get_analog : dataList.nNum = %d\n", dataList.nNum);
            for (i = 0; i < dataList.nNum; i++)
            {
                PRTL_FMT_TRACE("class22_invoke_get_analog : dataList.value[%d] = %d\n", i, dataList.value[i]);
            }
        }
        break;
    case 0xF2040400:
        {
            OOP_ANALOG_CONFIG_T dataList;
            memset(&dataList, 0, sizeof(dataList));
            normal.oad.value = 0xF2040400;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_ANALOG_CONFIG_T), (uint8*)&dataList, &len);
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
            
            //转换成报文数据
            convert.srcBuf = &dataList;
            convert.srcLen = sizeof(dataList);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;

            PRTL_FMT_TRACE("class22_invoke_get_analog : dataList.nNum = %d\n", dataList.nNum);
            for (i = 0; i < dataList.nNum; i++)
            {
                PRTL_FMT_TRACE("class22_invoke_get_analog : dataList.param[%d].startValue = %d\n", i, dataList.param[i].startValue);
                PRTL_FMT_TRACE("class22_invoke_get_analog : dataList.param[%d].endValue = %d\n", i, dataList.param[i].endValue);
                PRTL_FMT_TRACE("class22_invoke_get_analog : dataList.param[%d].value.scaler = %d\n", i, dataList.param[i].value.scaler);
                PRTL_FMT_TRACE("class22_invoke_get_analog : dataList.param[%d].value.unit = %d\n", i, dataList.param[i].value.unit);
            }
        }
        break;
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class22(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class22 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_switch(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    switch (pOadInfo->pTab->oad.value)
    {
        case 0xF2050200:
        {
            OOP_SWITCHOUT_T dataList;
            memset(&dataList, 0, sizeof(dataList));
            uint32 i = 0;

            normal.oad.value = 0xF2050200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_SWITCHOUT_T), (uint8*)&dataList, &len);
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

            for (i = 0; i < dataList.nNum; i++)
            {
                if (dataList.value[i].name.nNum == 0)
                {
                    sprintf(dataList.value[i].name.value, "Relay-%d", i + 1);
                    dataList.value[i].name.nNum = strlen(dataList.value[i].name.value);
                }
                PRTL_FMT_LOGCC("class22_invoke_get : dataList.value[i].portname = %s\n", dataList.value[i].name.value);
                PRTL_FMT_LOGCC("class22_invoke_get : dataList.value[i].attribute = %d\n", dataList.value[i].attribute);
                PRTL_FMT_LOGCC("class22_invoke_get : dataList.value[i].current = %d\n", dataList.value[i].current);
                PRTL_FMT_LOGCC("class22_invoke_get : dataList.value[i].input = %d\n", dataList.value[i].input);
            }

            //转换成报文数据
            convert.srcBuf = &dataList;
            convert.srcLen = sizeof(dataList);
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
    dar = data_to_buf_class22(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class22 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_switch_unit(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    switch (pOadInfo->pTab->oad.nObjID)
    {
        case 0xF205:
        {
            OOP_SWITCHOUT_T dataList;
            SWITCHOUT data;
            memset(&dataList, 0, sizeof(dataList));
            memset(&data, 0, sizeof(data));
            uint32 i = 0;

            normal.oad.value = 0xF2050200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_SWITCHOUT_T), (uint8*)&dataList, &len);
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

            for (i = 0; i < dataList.nNum; i++)
            {
                if (dataList.value[i].name.nNum == 0)
                {
                    sprintf(dataList.value[i].name.value, "Relay-%d", i + 1);
                    dataList.value[i].name.nNum = strlen(dataList.value[i].name.value);
                }
                if (pOadInfo->nIndex == (i + 1))
                {
                    data = dataList.value[i];
                }
                PRTL_FMT_LOGCC("class22_invoke_get : dataList.value[i].portname = %s\n", dataList.value[i].name.value);
                PRTL_FMT_LOGCC("class22_invoke_get : dataList.value[i].attribute = %d\n", dataList.value[i].attribute);
                PRTL_FMT_LOGCC("class22_invoke_get : dataList.value[i].current = %d\n", dataList.value[i].current);
                PRTL_FMT_LOGCC("class22_invoke_get : dataList.value[i].input = %d\n", dataList.value[i].input);
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
    dar = data_to_buf_class22(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class22 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_warn(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2060200:
        {
            OOP_WARNINGOUT_T dataList;
            memset(&dataList, 0, sizeof(dataList));
            normal.oad.value = 0xF2060200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_WARNINGOUT_T), (uint8*)&dataList, &len);
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
            
            //转换成报文数据
            convert.srcBuf = &dataList;
            convert.srcLen = sizeof(dataList);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;

            PRTL_FMT_LOGCC("class22_invoke_get_warn_out : dataList.nNum = %d\n", dataList.nNum);
        }
        break;
    case 0xF2060400:
        {
            OOP_WARNINGTIME_T data;
            memset(&data, 0, sizeof(data));
            normal.oad.value = 0xF2060400;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_WARNINGTIME_T), (uint8*)&data, &len);
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

            PRTL_FMT_LOGCC("class22_invoke_get_warn_out : data.nNum = 0x%x\n", data.nNum);

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
    dar = data_to_buf_class22(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class22 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_local_module_list(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2090200:
        {
            OOP_ROUTEMODULE_T datalist;
            ROUTEMODULE data;
            memset(&data, 0, sizeof(data));
            memset(&datalist, 0, sizeof(datalist));
            normal.oad.value = 0xF2090200;

            for (i = 0; i < OOP_MAX_PLC; i++)
            {
                normal.infoNum = i + 1;
                ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(ROUTEMODULE), (uint8*)&data, &len);
                if((ret != ERR_OK) || (len == 0))
                {
                    PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, normal.logicId, normal.infoNum, normal.oad.value);
                    continue;
                }
                else
                {
                    datalist.item[datalist.nNum] = data;
                    datalist.nNum ++;
                }
            }
            
            //转换成报文数据
            convert.srcBuf = &datalist;
            convert.srcLen = sizeof(datalist);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;

            for (i = 0; i < datalist.nNum; i++)
            {
                PRTL_FMT_LOGCC("class22_invoke_get_local_module_list : datalist.item[%d].name: %s\n", i, datalist.item[i].name.value);
                PRTL_FMT_LOGCC("class22_invoke_get_local_module_list : datalist.item[%d].dcb.baud = %d\n", i, datalist.item[i].dcb.baud);
                PRTL_BUF_LOGCC(datalist.item[i].verInfo.factory.value, 2, "class22_invoke_get_local_module_list : data.item[%d].verInfo.factory:", i);
                PRTL_BUF_LOGCC(datalist.item[i].verInfo.ic.value, 2, "class22_invoke_get_local_module_list : data.item[%d].verInfo.ic:", i);
                PRTL_FMT_LOGCC("class22_invoke_get_local_module_list : datalist.item[%d].verInfo.dtVer.year: %d\n", i, datalist.item[i].verInfo.dtVer.year);
                PRTL_FMT_LOGCC("class22_invoke_get_local_module_list : datalist.item[%d].verInfo.dtVer.month: %d\n", i, datalist.item[i].verInfo.dtVer.month);
                PRTL_FMT_LOGCC("class22_invoke_get_local_module_list : datalist.item[%d].verInfo.dtVer.mday: %d\n", i, datalist.item[i].verInfo.dtVer.mday);
                PRTL_FMT_LOGCC("class22_invoke_get_local_module_list : datalist.item[%d].verInfo.wVer: %d\n", i, datalist.item[i].verInfo.wVer);
            }
        }
        break;   
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_local_module_list(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_local_module_list failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**********************************************************************
* @name      : class22_get_appid
* @brief     ：获取APPID
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-19
* @Update    : 
**********************************************************************/
int class22_get_appid(char *appname, uint8 *appid, int bufmax)
{
    int ret = 0;
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        return 0;
    }
    else
    {
        ret = dev->devinfo_read_appid(dev, appname, (char*)appid, bufmax);
        hal_device_release((HW_DEVICE *)dev);
    }
    return 0 == ret ? 24 : 0;
}

/**
*********************************************************************
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_reg_app_info(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int i = 0;
    int ret = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 delay = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    OOP_REG_APP_INFOS_T datalist;
    memset(&datalist, 0, sizeof(datalist));

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF20F0200:
        {
            uint8 appidSec[32] = {0};
            
            regapp_callin_stat_set(1);
            ret = advance_seek_app();
            if (0 != ret)
            {
                PRTL_FMT_DEBUG("advance_seek_app failed %d\n", ret);
            }

            //延时等待查询完毕
            while (delay < 50)
            {
                usleep(100000);
                if (regapp_callin_stat_get() == 0)
                {
                    break;
                }
                delay ++;
            }
            if ((delay >= 50) || (regapp_callin_stat_get() == 1))
            {
                regapp_callin_stat_set(0);
                PRTL_FMT_DEBUG("class22_invoke_get_reg_app_info timeout\n");
                return DATA_TEMPORARY_FAILURE;
            }

            /* 安全不注册, 从HAL获取 */
            if(0 != class22_get_appid("secReinf", appidSec, 32))
            {
                oopRegAppInfoList.appInfo[oopRegAppInfoList.appNum].appName.nNum = strlen("secReinf");
                memcpy(oopRegAppInfoList.appInfo[oopRegAppInfoList.appNum].appName.value, "secReinf", strlen("secReinf"));

                oopRegAppInfoList.appInfo[oopRegAppInfoList.appNum].version.nNum = strlen("1.0.0.7");
                memcpy(oopRegAppInfoList.appInfo[oopRegAppInfoList.appNum].version.value, "1.0.0.7", strlen("1.0.0.7"));

                oopRegAppInfoList.appInfo[oopRegAppInfoList.appNum].dataTime.year = 2022;
                oopRegAppInfoList.appInfo[oopRegAppInfoList.appNum].dataTime.month = 8;
                oopRegAppInfoList.appInfo[oopRegAppInfoList.appNum].dataTime.day = 20;
                oopRegAppInfoList.appInfo[oopRegAppInfoList.appNum].dataTime.hour = 0;
                oopRegAppInfoList.appInfo[oopRegAppInfoList.appNum].dataTime.minute = 0;
                oopRegAppInfoList.appInfo[oopRegAppInfoList.appNum].dataTime.second = 0;
                
                memcpy(oopRegAppInfoList.appInfo[oopRegAppInfoList.appNum].appID.value, appidSec, 24);
                oopRegAppInfoList.appInfo[oopRegAppInfoList.appNum].appID.nNum = 24;

                oopRegAppInfoList.appNum += 1;
                
            }

            datalist.nNum = oopRegAppInfoList.appNum;
            memcpy(&datalist.item[0], &oopRegAppInfoList.appInfo[0], oopRegAppInfoList.appNum*sizeof(OOP_REG_APP_INFO_T));

            for (i = 0; i < datalist.nNum; i++)
            {
                PRTL_BUF_DEBUG(datalist.item[i].appID.value, datalist.item[i].appID.nNum, 
                               "app: %s, version: %s, time: %04d-%02d-%02d %02d:%02d:%02d, APPID: ", 
                               datalist.item[i].appName.value, datalist.item[i].version.value,
                               datalist.item[i].dataTime.year, datalist.item[i].dataTime.month, 
                               datalist.item[i].dataTime.day, datalist.item[i].dataTime.hour,
                               datalist.item[i].dataTime.minute, datalist.item[i].dataTime.second);
            }
            
            //转换成报文数据
            convert.srcBuf = &datalist;
            convert.srcLen = sizeof(datalist);
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
    dar = data_to_buf_reg_app_info(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_reg_app_info failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
//OOP_DAR_E class22_invoke_get_net_topology_info(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
//{
//    int i = 0;
//    int ret = ERR_OK;
//    OOP_DAR_E dar = DATA_SUCCESS;
//    NOMAL_OAD_T normal = {0};
//    DATA_CONVERT_T convert;
//    uint32 sOffset = 0;
//    uint32 offset  = 0;
//    uint32 len = 0;    
//
//    MEMZERO(&convert, sizeof(convert)); 
//
//    //读出数据
//    switch (pOadInfo->pTab->oad.value)
//    {
//    case 0xF2090700:
//        {
//            OOP_NETTOPOLOGYINFO_LIST_T datalist;
//            OOP_NETTOPOLOGYINFO_T data;
//            memset(&data, 0, sizeof(data));
//            memset(&datalist, 0, sizeof(datalist));
//            normal.oad.value = 0xF2090700;
//
//            for (i = 0; i < OOP_MAX_PLNODE; i++)
//            {
//                normal.infoNum = i + 1;
//                ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_NETTOPOLOGYINFO_T), (uint8*)&data, &len);
//                if((ret != ERR_OK) || (len == 0))
//                {
//                    PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
//                        ret, normal.logicId, normal.infoNum, normal.oad.value);
//                    continue;
//                }
//                else
//                {
//                    datalist.item[datalist.nNum] = data;
//                    datalist.nNum ++;
//                    PRTL_FMT_LOGCC("获取到的网络节点 i is %d\n",i);
//                }
//            }
//            
//            //转换成报文数据
//            convert.srcBuf = &datalist;
//            convert.srcLen = sizeof(datalist);
//            convert.sOffset = &sOffset;
//            
//            convert.dstBuf = &outData->pResultData[5];
//            convert.dstLen = outDataMax;
//            convert.dOffset = &outData->resultDataLen;
//
//        }
//        break;   
//    default:
//        break;
//    }
//    
//    /* 数据和报文转换 */
//    dar = data_to_buf_net_topology_info(pOadInfo, &convert);
//    if(dar != DATA_SUCCESS)
//    {
//        PRTL_FMT_DEBUG("data_to_buf_local_module_list failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
//        return dar;
//    }
//
//    /* 填写报文开头处的OAD和数据结果0x01*/
//    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
//    offset += sizeof(OOP_OAD_U);
//    outData->pResultData[offset ++] = 0x01;
//    outData->resultDataLen += offset;
//
//    return dar;
//}


/**
*********************************************************************
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_net_topology_info(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
 
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 offset  = 0;
      
    uint32             i = 0;
 
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_NETTOPOLOGYINFO_T data;
    uint32    realNum = 0;    //数组由于分帧导致的每帧实际成员数
    uint32    numOffset = 0;  //数组成员数在报文中的位置
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOLEAN   isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));

    memset(&data, 0, sizeof(data));
    MEMZERO(&convert, sizeof(convert)); 
 
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    PRTL_FMT_DEBUG("Initializing net_topology. please wait......\n");
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0xF2090700;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;

    //如果有断点，由于是升序，则开始为上次中断的点，前闭后开
    if(pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = pApduInfo->bpBreakInfo->tmStartSeg;    
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;                             //结果
    outData->pResultData[offset ++] = DT_ARRAY;                         //类型
    numOffset = offset;                                                 //数量偏移
    outData->pResultData[offset ++] = 0x00;                             //数量，先填0
    bufAllLen = offset;

    ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("appdb_meter_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("appdb_meter_list_record_init db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("appdb_meter_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0xF2090700)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_NETTOPOLOGYINFO_T)))
            {
                PRTL_FMT_DEBUG("appdb_meter_list_record_init data err !\n");
                PRTL_FMT_DEBUG("appdb_meter_list_record_init stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("appdb_meter_list_record_init stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("appdb_meter_list_record_init stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("appdb_meter_list_record_init sizeof(OOP_METER_T) = %d\n", 
                    sizeof(OOP_METER_T));
                continue;
            }

            memcpy(&data, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
            PRTL_FMT_DEBUG("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[offset];
            convert.dstLen = outDataMax - offset;
            convert.dOffset = &dOffset;

            //记录断点信息
            breakInfo.tmStartSeg = stepRecordOut.infoNum;
            
            /* 数据和报文转换 */
            dar = data_to_buf_net_topology(pOadInfo, &convert);
            if(dar == DATA_SUCCESS)
            {                  
                bufAllLen += dOffset;
                
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outData->resultDataLen = offset;
                    PRTL_FMT_DEBUG(convert.dstBuf, dOffset, "第%d个网络节点:", i);
                    realNum ++;
                    dOffset = 0;
                    sOffset = 0;
                }
                else
                {
                    dOffset = 0;
                    sOffset = 0;
                    isBufFull = TRUE;
                }
            }
            else
            {
                continue;
            }
            
            if ((isBufFull) && (offset > 0))
            {
                break;
            }                            
        }
    }
    db_read_record_end(pApduInfo->hUdp, handle);
    
    PRTL_FMT_DEBUG("Initialize meters end.\n");
   
    if (!isBufFull)
    {
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = pApduInfo->reqType;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
        }
    }
    
    //刷新断点信息
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));
    
    outData->pResultData[numOffset] = realNum;   //重新记数量
    outData->resultDataLen = offset;


    return dar;
}


/**
*********************************************************************
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_nets_info(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //int i = 0;
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;    

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2090800:
        {

            OOP_NETSINFO_T data;
            memset(&data, 0, sizeof(data));

            normal.oad.value = 0xF2090800;


            normal.infoNum = 1;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_NETSINFO_T), (uint8*)&data, &len);
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
    dar = data_to_buf_nets_info(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_nets_info failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_broadband_frequency(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //int i = 0;
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;    

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2090900:
        {

            uint8 data;
            memset(&data, 0, sizeof(data));

            normal.oad.value = 0xF2090900;


            normal.infoNum = 0;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_NETSINFO_T), (uint8*)&data, &len);
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
    dar = data_to_buf_broadband_frequency(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_local_module_list failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_chip_node(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint8 tmpoffset=0;
    uint16    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - 3 - 30;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0xF2090500:
        {
            DATA_CONVERT_T convert;
            OOP_CHILDNODE_T chipnode;                                        //每个电表的档案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopMeterNum = 0;
            uint32 oopMeterNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));
            MEMZERO(&chipnode, sizeof(chipnode));

            #if CHILDNODE_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取所有网络节点信息\n");

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            oopMeterNumOffset = offset;                                         //档案数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;
            for (i = 1; i <= OOP_MAX_PLNODE; i++)
            {
                
                if (slicing_is_comtinue_oop_meter(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }

                #if CHILDNODE_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(pApduInfo->hUdp, &nomalOad, sizeof(chipnode), (uint8*)&chipnode, &len);
                if ((ret < ERR_OK) || (len != sizeof(chipnode)))
                #else
                ret = app_get_childnode_list(i, &chipnode);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                //转换成报文数据
                convert.srcBuf = &chipnode;
                convert.srcLen = sizeof(chipnode);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_chip_node(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个档案报文:", i);
                        oopMeterNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("class11_invoke_get_oop_meter isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = pApduInfo->reqType;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

            if (oopMeterNum > 0)
            {
                result = DATA_SUCCESS;
                outData->pResultData[oopMeterNumOffset] = oopMeterNum;   //重新记档案数量
                outData->resultDataLen = offset;
            }
            else
            {
                memcpy(&outData->pResultData[tmpoffset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
                tmpoffset += sizeof(OOP_OAD_U);
                outData->pResultData[tmpoffset++]=0x01;
                outData->pResultData[tmpoffset++]=0x00;
                outData->resultDataLen = tmpoffset;
                return DATA_SUCCESS;
            }
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }
    return result;
}



/**
*********************************************************************
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_node_phase_info(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int i = 0;
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   
    OOP_NODEPHASEINFO_T data;
    uint32    realNum = 0;    //数组由于分帧导致的每帧实际成员数
    uint32    numOffset = 0;  //数组成员数在报文中的位置
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&normal, sizeof(normal)); 
    normal.oad.value = 0xF2090A00;

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2090A00:
        {

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            numOffset = offset;                                                 //数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;

            for(i=0;i<OOP_MAX_PLNODE;i++)
            {
                if (slicing_is_comtinue(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }
            
                memset(&data, 0x00, sizeof(data));
                normal.infoNum = i;
                ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_NODEPHASEINFO_T), (uint8*)&data, &len);
                if((ret != ERR_OK) || (len == 0))
                {
                    PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, normal.logicId, normal.infoNum, normal.oad.value);
                    continue ;
                }

                //转换成报文数据
                convert.srcBuf = &data;
                convert.srcLen = sizeof(data);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;
                
                /* 数据和报文转换 */
                dar = data_to_buf_node_phase_info(pOadInfo, &convert);
                if(dar == DATA_SUCCESS)
                {                  
                    bufAllLen += dOffset;
                    
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_FMT_DEBUG(convert.dstBuf, dOffset, "第%d个子节点:", i);
                        realNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }                
            }

            if (!isBufFull)
            {
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = pApduInfo->reqType;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

            outData->pResultData[numOffset] = realNum;   //重新记数量
            outData->resultDataLen = offset;
        }
        break;   
    default:
        break;
    }
    
    return dar;
}

/**
*********************************************************************
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_across_area_flag(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //int i = 0;
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;    

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2090B00:
        {

            uint8 data;
            memset(&data, 0, sizeof(data));

            normal.oad.value = 0xF2090B00;


            normal.infoNum = 0;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_NETSINFO_T), (uint8*)&data, &len);
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
    dar = data_to_buf_across_area_flag(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_local_module_list failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_node_version_info(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint8     tmpoffset=0;
    uint16    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - 3 - 30;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0xF2090C00:
        {
            DATA_CONVERT_T convert;
            OOP_NODEVERSIONINFO_T data;                                        //每个节点版本信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopMeterNum = 0;
            uint32 oopMeterNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));
            MEMZERO(&data, sizeof(data));

            #if NODEVERSION_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取所有节点版本信息\n");

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            oopMeterNumOffset = offset;                                         //数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;
            for (i = 0; i <= NO_VIR_MTR_MAX_NUM; i++)
            {
                if (slicing_is_comtinue(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }

                #if NODEVERSION_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(pApduInfo->hUdp, &nomalOad, sizeof(data), (uint8*)&data, &len);
                if ((ret < ERR_OK) || (len != sizeof(data)))
                #else
                ret = app_get_nodeversion_list(i, &data);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                //转换成报文数据
                convert.srcBuf = &data;
                convert.srcLen = sizeof(data);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_node_version_info(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个节点版本信息:", i);
                        oopMeterNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("class22_invoke_get_node_version_info isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = pApduInfo->reqType;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

            if (oopMeterNum > 0)
            {
                result = DATA_SUCCESS;
                outData->pResultData[oopMeterNumOffset] = oopMeterNum;   //重新记档案数量
                outData->resultDataLen = offset;
            }
            else
            {
                memcpy(&outData->pResultData[tmpoffset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
                tmpoffset += sizeof(OOP_OAD_U);
                outData->pResultData[tmpoffset++]=0x01;
                outData->pResultData[tmpoffset++]=0x00;
                outData->resultDataLen = tmpoffset;
                return DATA_SUCCESS;
            }
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }
    return result;
}

/**
*********************************************************************
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_net_size(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
//    int i = 0;
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
//    DATA_CONVERT_T convert;
//    uint32 sOffset = 0;
//    uint32 dOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0; 
    uint16 netsize=0;


    MEMZERO(&normal, sizeof(normal)); 
    normal.oad.value = 0xF2091400;
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2091400:
        {
            normal.infoNum = 1;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(uint16), (uint8*)&netsize, &len);
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
            else
            {
                memcpy_r(outData->pResultData+offset,(uint8*)&pOadInfo->pTab->oad.value,4);
                offset=offset+4;
                outData->pResultData[offset++]=0x01;
                outData->pResultData[offset++]=DT_LONG_UNSIGNED;
                memcpy_r(&outData->pResultData[offset],(uint8*)&netsize,sizeof(uint16));
                offset=offset+sizeof(uint16);
                outData->resultDataLen=offset;
                dar = DATA_SUCCESS;

            }
            
         }
        break;   
    default:
        break;
    }
    
    return dar;
}


/**
*********************************************************************
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_refuse_node(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint8 tmpoffset=0;
    uint16    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - 3 - 30;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0xF2091500:
        {
            DATA_CONVERT_T convert;
            OOP_REFUSESLAVE_T refusenode;                                        //每个电表的档案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopMeterNum = 0;
            uint32 oopMeterNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));
            MEMZERO(&refusenode, sizeof(refusenode));

            
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;
            

            PRTL_FMT_LOGCC("获取所有网络节点信息\n");

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            oopMeterNumOffset = offset;                                         //档案数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;
            for (i = 1; i <= OOP_MAX_PLNODE; i++)
            {
                
                if (slicing_is_comtinue_oop_meter(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }

                nomalOad.infoNum = i;
                ret = db_read_nomal(pApduInfo->hUdp, &nomalOad, sizeof(refusenode), (uint8*)&refusenode, &len); 
                if(ret !=0)
                {
                    continue;
                }

                //转换成报文数据
                convert.srcBuf = &refusenode;
                convert.srcLen = sizeof(refusenode);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_refuse_node(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个档案报文:", i);
                        oopMeterNum = refusenode.nNum + oopMeterNum;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("class11_invoke_get_oop_meter isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = pApduInfo->reqType;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

            if (oopMeterNum > 0)
            {
                result = DATA_SUCCESS;
                outData->pResultData[oopMeterNumOffset] = oopMeterNum;   //重新记档案数量
                outData->resultDataLen = offset;
            }
            else
            {
                memcpy(&outData->pResultData[tmpoffset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
                tmpoffset += sizeof(OOP_OAD_U);
                outData->pResultData[tmpoffset++]=0x01;
                outData->pResultData[tmpoffset++]=0x00;
                outData->resultDataLen = tmpoffset;
                return DATA_SUCCESS;
            }
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }
    return result;
}


/**
*********************************************************************
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_refuse_node_report(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
//    int i = 0;
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
//    DATA_CONVERT_T convert;
//    uint32 sOffset = 0;
//    uint32 dOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0; 
    uint16 refusenode=0;


    MEMZERO(&normal, sizeof(normal)); 
    normal.oad.value = 0xF2091600;
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2091600:
        {
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(uint8), (uint8*)&refusenode, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
				outData->pResultData[4] = 0x01;
				outData->pResultData[5] = 00;
				outData->resultDataLen = 6;
                return DATA_SUCCESS;
            }
            else
            {
                memcpy_r(outData->pResultData+offset,(uint8*)&pOadInfo->pTab->oad.value,4);
                offset=offset+4;
                outData->pResultData[offset++]=0x01;
                outData->pResultData[offset++]=DT_ENUM;
                memcpy_r(&outData->pResultData[offset],(uint8*)&refusenode,sizeof(uint8));
                offset=offset+sizeof(uint8);
                outData->resultDataLen=offset;
                dar = DATA_SUCCESS;

            }
            
         }
        break;   
    default:
        break;
    }
    
    return dar;
}

/**
*********************************************************************
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_net_base_info(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //int i = 0;
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;    

    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&normal, sizeof(normal));

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2091E00:
        {

            OOP_NETBASEINFO_T data;
            memset(&data, 0, sizeof(data));

            normal.oad.value = 0xF2091E00;


            normal.infoNum = 1;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_NETSINFO_T), (uint8*)&data, &len);
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
    dar = data_to_buf_nets_base_info(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_local_module_list failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_net_node_info(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint16    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - 3 - 30;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0xF2091F00:
        {
            DATA_CONVERT_T convert;
            OOP_NETNODEINFO_T netnodeinfo;                                        //每个电表的档案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopMeterNum = 0;
            uint32 oopMeterNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));
            MEMZERO(&netnodeinfo, sizeof(netnodeinfo));
           
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;


            PRTL_FMT_LOGCC("获取所有网络节点信息\n");

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            oopMeterNumOffset = offset;                                         //档案数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;
            for (i = 1; i <= OOP_MAX_PLNODE; i++)
            {
                
                if (slicing_is_comtinue_oop_meter(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }
        
                nomalOad.infoNum = i;
                ret = db_read_nomal(pApduInfo->hUdp, &nomalOad, sizeof(netnodeinfo), (uint8*)&netnodeinfo, &len);
                if ((ret != ERR_OK) || (len != sizeof(OOP_NETNODEINFO_T)))
                {
                    continue;
                }

                PRTL_FMT_LOGCC("获取到的网络节点 i is %d\n",i);

                //转换成报文数据
                convert.srcBuf = &netnodeinfo;
                convert.srcLen = sizeof(netnodeinfo);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_net_node_info(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个档案报文:", i);
                        oopMeterNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("class11_invoke_get_oop_meter isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = pApduInfo->reqType;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

            if (oopMeterNum > 0)
            {
                result = DATA_SUCCESS;
                outData->pResultData[oopMeterNumOffset] = oopMeterNum;   //重新记档案数量
                outData->resultDataLen = offset;
            }
            else
            {
                outData->resultDataLen = 0;
                return DATA_READ_WRITE_DENIED;
            }
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }
    return result;
}


/**
*********************************************************************
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_local_module_refresh_ti(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2090600:
        {
            OOP_TI_T data;
            memset(&data, 0, sizeof(data));
            normal.oad.value = 0xF2090600;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_TI_T), (uint8*)&data, &len);
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
            
            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;

            PRTL_FMT_LOGCC("class22_invoke_get_local_module_refresh_ti : data.value = %d\n", data.value);
            PRTL_FMT_LOGCC("class22_invoke_get_local_module_refresh_ti : data.unit = %d\n", data.unit);
        }
        break;   
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_local_module_refresh_ti(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class22 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class22_invoke_get_local_module
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_local_module(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2090200:
        return class22_invoke_get_local_module_list(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2090500:
        return class22_invoke_get_chip_node(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2090600:
        return class22_invoke_get_local_module_refresh_ti(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2090700:
        return class22_invoke_get_net_topology_info(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2090800:
        return class22_invoke_get_nets_info(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2090900:
        return class22_invoke_get_broadband_frequency(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2090A00:
        return class22_invoke_get_node_phase_info(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2090B00:
        return class22_invoke_get_across_area_flag(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2090C00:
        return class22_invoke_get_node_version_info(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2091400:
        return class22_invoke_get_net_size(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2091500:
        return class22_invoke_get_refuse_node(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2091600:
        return class22_invoke_get_refuse_node_report(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2091E00:
        return class22_invoke_get_net_base_info(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF2091F00:
        return class22_invoke_get_net_node_info(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name：       class22_invoke_get_bluetoth
* @brief：      class22的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2021-8-2
*********************************************************************
*/
OOP_DAR_E class22_invoke_get_F20B0200(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int i = 0;
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = 0;
    uint32 len = 0;    
    OOP_BLUETOOTHMODS_T tBlueConfig;
    uint8 *temp = outData->pResultData + 5;
    uint8  recvbuf[512] = {0};
    BLE_CONFIG_T tConfig;
    BLE_SN_T tSn;
    
    uint8  sn[BLE_SN_MAX] = {0x00};
    
    if(0xF20B0200 != pOadInfo->pTab->oad.value)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    memset(&tBlueConfig, 0, sizeof(OOP_BLUETOOTHMODS_T));
    ret = appmain_get_bluetooth_cfg(recvbuf, 512);
    if(ret > 0)
    {
        len = ret;
        memset(&tConfig, 0, sizeof(BLE_CONFIG_T));
        ret = ble_config_split(recvbuf, len, &tConfig);
        if(0 == ret)
        {
            tBlueConfig.nNum = 1;
            tBlueConfig.value[0].nIdx = 1;
            tBlueConfig.value[0].info.info.nNum = tConfig.len;
            memcpy(tBlueConfig.value[0].info.info.value, tConfig.name, tConfig.len);
            tBlueConfig.value[0].info.mac.nNum = 6;
            memcpy(tBlueConfig.value[0].info.mac.value, tConfig.MAC, 6);
            tBlueConfig.value[0].pass.nNum = 0;
            tBlueConfig.value[0].nMod = BLE_MATCH_JW;
        }
    }

    ret = appmain_get_bluetooth_sn(recvbuf, 512);
    if (ret > 0)
    {
        len = ret;
        memset(&tSn, 0, sizeof(tSn));
        ret = ble_sn_split(recvbuf, len, &tSn);
        if(0 == ret)
        {
            tBlueConfig.value[0].sn.nNum = tSn.num;
            memcpy(&tBlueConfig.value[0].sn.value[0], tSn.sn, tSn.num);
        }
    }
    else
    {
        tBlueConfig.value[0].sn.nNum = 20;
        memcpy(&tBlueConfig.value[0].sn.value[0], sn, 20);
    }
    
    temp[offset++] = DT_ARRAY;
    temp[offset++] = tBlueConfig.nNum;
    for(i = 0; i < tBlueConfig.nNum; i++)
    {
        temp[offset++] = DT_STRUCTURE;
        temp[offset++] = 5;

        /* 模块序号 */
        temp[offset++] = DT_UNSIGNED;
        temp[offset++] = tBlueConfig.value[i].nIdx;

        /* 蓝牙信息 */
        temp[offset++] = DT_STRUCTURE;
        temp[offset++] = 2;
        temp[offset++] = DT_VISIBLE_STRING;
        temp[offset++] = tBlueConfig.value[i].info.info.nNum;
        if(tBlueConfig.value[i].info.info.nNum > 0)
        {
            memcpy(temp + offset, tBlueConfig.value[i].info.info.value, tBlueConfig.value[i].info.info.nNum);
            offset += tBlueConfig.value[i].info.info.nNum;
        }
        temp[offset++] = DT_OCTET_STRING;
        temp[offset++] = tBlueConfig.value[i].info.mac.nNum;
        if(tBlueConfig.value[i].info.mac.nNum > 0)
        {
            memcpy(temp + offset, tBlueConfig.value[i].info.mac.value, tBlueConfig.value[i].info.mac.nNum);
            offset += tBlueConfig.value[i].info.mac.nNum;
        }

        /* 配对密码 */
        temp[offset++] = DT_VISIBLE_STRING;
        temp[offset++] = tBlueConfig.value[i].pass.nNum;
        if(tBlueConfig.value[i].pass.nNum > 0)
        {
            memcpy(temp + offset, tBlueConfig.value[i].pass.value, tBlueConfig.value[i].pass.nNum);
            offset += tBlueConfig.value[i].pass.nNum;
        }

        /* 配对模式 */
        temp[offset++] = DT_ENUM;
        temp[offset++] = tBlueConfig.value[i].nMod;

        /* 蓝牙序列号 */
        temp[offset++] = DT_OCTET_STRING;
        temp[offset++] = tBlueConfig.value[i].sn.nNum;
        if(tBlueConfig.value[i].sn.nNum > 0)
        {
            memcpy(temp + offset, tBlueConfig.value[i].sn.value, tBlueConfig.value[i].sn.nNum);
            offset += tBlueConfig.value[i].sn.nNum;
        }
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[4] = 0x01;
    outData->resultDataLen = offset + 1;
    return dar;
}

/**
*********************************************************************
* @name：       class22_invoke_get_bluetoth
* @brief：      class22的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2021-8-2
*********************************************************************
*/
OOP_DAR_E class22_invoke_get_F20B0300(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = 0;
    uint32 len = 0;
    uint8 *temp = outData->pResultData + 5;
    uint8  recvbuf[512] = {0};
    BLE_CONFIG_T tConfig;

    if(0xF20B0300 != pOadInfo->pTab->oad.value)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }
    
    temp[offset++] = DT_UNSIGNED;
    temp[offset] = 0;
    ret = appmain_get_bluetooth_cfg(recvbuf, 512);
    if(ret > 0)
    {
        len = ret;
        memset(&tConfig, 0, sizeof(BLE_CONFIG_T));
        ret = ble_config_split(recvbuf, len, &tConfig);
        if(0 == ret)
        {
            temp[offset] = 1;
        }
    }
    offset++;

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[4] = 0x01;
    outData->resultDataLen = offset + 1;
    return dar;
}

/**
*********************************************************************
* @name：       class22_invoke_get_F20B0400
* @brief：      class22的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2021-11-25
*********************************************************************
*/
OOP_DAR_E class22_invoke_get_F20B0400(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = 0;
    uint32 len = 0;    
    OOP_BLUETOOTHSLAVES_T tBlueSlave;
    uint8 *temp = outData->pResultData + 5;
    uint8  recvbuf[1024] = {0};
    BLE_CONNECT_INFO_T tPort;
    uint32 i = 0;
    uint32 num = 0;
    
    if(0xF20B0400 != pOadInfo->pTab->oad.value)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    memset(&tBlueSlave, 0, sizeof(OOP_BLUETOOTHSLAVES_T));
    ret = appmain_get_bluetooth_connect(recvbuf, 1024);
    if(ret > 0)
    {
        len = ret;
        ret = ble_connect_split(recvbuf, len, &tPort);
        if(0 == ret)
        {
            for(i = 0; i < tPort.num; i++)
            {
                if(tPort.list[i].mode == BLE_MODE_SLAVE && FALSE != tPort.list[i].bConnect)
                {
                    tBlueSlave.value[num].mac.nNum = 6;
                    memcpy(tBlueSlave.value[num].mac.value, tPort.list[i].MAC, 6);
                    num++;
                }
            }
        }
    }
    tBlueSlave.nNum = num;
    
    temp[offset++] = DT_ARRAY;
    temp[offset] = tBlueSlave.nNum;
    for(i = 0; i < tBlueSlave.nNum; i++)
    {
        temp[offset++] = DT_STRUCTURE;
        temp[offset] = 2;
        
        /* MAC地址 */
        temp[offset++] = DT_OCTET_STRING;
        temp[offset++] = tBlueSlave.value[i].mac.nNum;
        if(tBlueSlave.value[i].mac.nNum > 0)
        {
            memcpy(temp + offset, tBlueSlave.value[i].mac.value, tBlueSlave.value[i].mac.nNum);
            offset += tBlueSlave.value[i].mac.nNum;
        }

        /* 资产号 */
        temp[offset++] = DT_OCTET_STRING;
        temp[offset++] = tBlueSlave.value[i].assetNo.nNum;
        if(tBlueSlave.value[i].assetNo.nNum > 0)
        {
            memcpy(temp + offset, tBlueSlave.value[i].assetNo.value, tBlueSlave.value[i].assetNo.nNum);
            offset += tBlueSlave.value[i].assetNo.nNum;
        }
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[4] = 0x01;
    outData->resultDataLen = offset + 1;
    return dar;
}

/**
*********************************************************************
* @name：       class22_invoke_get_F20B0500
* @brief：      class22的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2021-11-25
*********************************************************************
*/
OOP_DAR_E class22_invoke_get_F20B0500(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = 0;
    uint32 len = 0;    
    OOP_BTCONNECTINFO_T tBlue;
    uint8 *temp = outData->pResultData + 5;
    uint8  recvbuf[1024] = {0};
    BLE_CONNECT_INFO_T tPort;
    uint32 i = 0;
    uint32 numM = 0;
    uint32 numS = 0;
    
    if(0xF20B0500 != pOadInfo->pTab->oad.value)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    memset(&tBlue, 0, sizeof(OOP_BTCONNECTINFO_T));
    ret = appmain_get_bluetooth_connect(recvbuf, 1024);
    if(ret > 0)
    {
        len = ret;
        ret = ble_connect_split(recvbuf, len, &tPort);
        if(0 == ret)
        {
            for(i = 0; i < tPort.num; i++)
            {
                if(tPort.list[i].mode == BLE_MODE_MASTER&& FALSE != tPort.list[i].bConnect)
                {
                    tBlue.master[numM].mac.nNum = 6;
                    memcpy(tBlue.master[numM].mac.value, tPort.list[i].MAC, 6);
                    numM++;
                }
                else if(tPort.list[i].mode == BLE_MODE_SLAVE && FALSE != tPort.list[i].bConnect)
                {
                    tBlue.slave[numS].mac.nNum = 6;
                    memcpy(tBlue.slave[numS].mac.value, tPort.list[i].MAC, 6);
                    numS++;
                }
            }
        }
    }
    tBlue.oad.value = 0xF20B0200;
    tBlue.nMasterNum = numM;
    tBlue.nSlaveNum = numS;
    
    temp[offset++] = DT_ARRAY;
    temp[offset++] = 1;
    temp[offset++] = DT_STRUCTURE;
    temp[offset++] = 3;

    /* 端口号 */
    temp[offset++] = DT_OAD;
    MEMCPY_R(temp + offset, &(tBlue.oad), sizeof(OOP_OAD_U));
    offset += 4;
    
    /* 主设备 */
    temp[offset++] = DT_ARRAY;
    temp[offset++] = tBlue.nMasterNum;
    for(i = 0; i < tBlue.nMasterNum; i++)
    {
        temp[offset++] = DT_STRUCTURE;
        temp[offset] = 2;

        /* 蓝牙描述符 */
        temp[offset++] = DT_VISIBLE_STRING;
        temp[offset++] = tBlue.master[i].info.nNum;
        if(tBlue.master[i].info.nNum > 0)
        {
            memcpy(temp + offset, tBlue.master[i].info.value, tBlue.master[i].info.nNum);
            offset += tBlue.master[i].info.nNum;
        }
        
        /* MAC地址 */
        temp[offset++] = DT_OCTET_STRING;
        temp[offset++] = tBlue.master[i].mac.nNum;
        if(tBlue.master[i].mac.nNum > 0)
        {
            memcpy(temp + offset, tBlue.master[i].mac.value, tBlue.master[i].mac.nNum);
            offset += tBlue.master[i].mac.nNum;
        }
    }

    /* 从设备 */
    temp[offset++] = DT_ARRAY;
    temp[offset++] = tBlue.nSlaveNum;
    for(i = 0; i < tBlue.nSlaveNum; i++)
    {
        temp[offset++] = DT_STRUCTURE;
        temp[offset] = 2;

        /* 蓝牙描述符 */
        temp[offset++] = DT_VISIBLE_STRING;
        temp[offset++] = tBlue.slave[i].info.nNum;
        if(tBlue.slave[i].info.nNum > 0)
        {
            memcpy(temp + offset, tBlue.slave[i].info.value, tBlue.slave[i].info.nNum);
            offset += tBlue.slave[i].info.nNum;
        }
        
        /* MAC地址 */
        temp[offset++] = DT_OCTET_STRING;
        temp[offset++] = tBlue.slave[i].mac.nNum;
        if(tBlue.slave[i].mac.nNum > 0)
        {
            memcpy(temp + offset, tBlue.slave[i].mac.value, tBlue.slave[i].mac.nNum);
            offset += tBlue.slave[i].mac.nNum;
        }
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[4] = 0x01;
    outData->resultDataLen = offset + 1;
    return dar;
}

/**
*********************************************************************
* @name：       class22_invoke_get_F20B0600
* @brief：      class22的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2021-11-25
*********************************************************************
*/
OOP_DAR_E class22_invoke_get_F20B0600(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = 0;
    uint32 len = 0;    
    OOP_BTWORKPARAMS_T tBlueConfigs;
    uint8 *temp = outData->pResultData + 5;
    uint8  recvbuf[512] = {0};
    BLE_CONFIG_T tConfig;
    NOMAL_OAD_T normal = {0};
    uint32 i = 0;
    
    if(0xF20B0600 != pOadInfo->pTab->oad.value)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    memset(&tBlueConfigs, 0, sizeof(OOP_BTWORKPARAMS_T));
    ret = appmain_get_bluetooth_cfg(recvbuf, 512);
    if(ret > 0)
    {
        len = ret;
        memset(&tConfig, 0, sizeof(BLE_CONFIG_T));
        ret = ble_config_split(recvbuf, len, &tConfig);
        if(0 == ret)
        {
            tBlueConfigs.nNum = 1;
            tBlueConfigs.param[0].oad.value = 0xF20B0200;
            tBlueConfigs.param[0].nPowerLevel = tConfig.power;
            tBlueConfigs.param[0].nBroadCastInterval = tConfig.broadcast;
            tBlueConfigs.param[0].nScanPeriod = tConfig.scan;
        }
    }
    else
    {
        memset(&normal, 0, sizeof(NOMAL_OAD_T));
        normal.oad.value = 0xF20B0600;
        ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_BTWORKPARAMS_T), (uint8*)&tBlueConfigs, &len);
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
    }

    temp[offset++] = DT_ARRAY;
    temp[offset++] = tBlueConfigs.nNum;
    for(i = 0; i < tBlueConfigs.nNum; i++)
    {
        temp[offset++] = DT_STRUCTURE;
        temp[offset++] = 4;

        /* 端口号 */
        temp[offset++] = DT_OAD;
        MEMCPY_R(temp + offset, &(tBlueConfigs.param[i].oad), sizeof(OOP_OAD_U));
        offset += 4;

        /* 发射功率 */
        temp[offset++] = DT_UNSIGNED;
        temp[offset++] = tBlueConfigs.param[i].nPowerLevel;

        /* 广播间隔 */
        temp[offset++] = DT_LONG_UNSIGNED;
        MEMCPY_R(temp + offset, &(tBlueConfigs.param[i].nBroadCastInterval), sizeof(uint16));
        offset += 2;

        /* 扫描间隔 */
        temp[offset++] = DT_LONG_UNSIGNED;
        MEMCPY_R(temp + offset, &(tBlueConfigs.param[i].nScanPeriod), sizeof(uint16));
        offset += 2;
    }
    
    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[4] = 0x01;
    outData->resultDataLen = offset + 1;
    return dar;
}

/**
*********************************************************************
* @name：       class22_invoke_get_F20B0B00
* @brief：      class22的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2021-11-25
*********************************************************************
*/
#if 0
OOP_DAR_E class22_invoke_get_F20B0B00(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int       ret = 0;
    uint32    offset = 0;
    uint32    len = 0; 
    uint8     sn[BLE_SN_MAX] = {0x00};
    uint32    snLen = 0x14;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8     recvbuf[512] = {0};

    BLE_SN_T tSn;
    memset(&tSn, 0, sizeof(tSn));

    ret = appmain_get_bluetooth_sn(recvbuf, 512);
    if (ret > 0)
    {
        len = ret;
        memset(&tSn, 0, sizeof(tSn));
        ret = ble_sn_split(recvbuf, len, &tSn);
        if (0 == ret)
        {
            snLen = tSn.num;
            memcpy(&sn[0], tSn.sn, snLen);
        }
    }
    
    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->pResultData[offset++] = 0x09;
    outData->pResultData[offset++] = snLen;
    memcpy(&outData->pResultData[offset], sn, snLen);
    offset += snLen;
    outData->resultDataLen = offset;
    return dar;
}
#else
//之前理解不对，台体下发的F20B0B00不是SN号，是另一个号，下发到终端，后面台体再读出来跟SN进行校验
//对终端来说，只需要把这个号存下来即可
OOP_DAR_E class22_invoke_get_F20B0B00(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int       ret = 0;
    uint32    offset = 0;
    uint32    len = 0; 
    OOP_DAR_E dar = DATA_SUCCESS;

    NOMAL_OAD_T normal = {0};
    
    BLE_SN_T tSn;
    memset(&tSn, 0, sizeof(tSn));

    normal.oad.value = 0xF20B0B00;
    normal.infoNum = 0;
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(BLE_SN_T), (uint8*)&tSn, &len);
    if ((ret != ERR_OK) || (len == 0))
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

    PRTL_BUF_DEBUG(tSn.sn, MIN(BLE_SN_MAX, tSn.num), "读取蓝牙CSN");
    
    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->pResultData[offset++] = 0x09;
    outData->pResultData[offset++] = MIN(tSn.num, BLE_SN_MAX);
    memcpy(&outData->pResultData[offset], tSn.sn, MIN(tSn.num, BLE_SN_MAX));
    offset += MIN(tSn.num, BLE_SN_MAX);
    outData->resultDataLen = offset;
    return dar;
}
#endif

/**
*********************************************************************
* @name：       class22_invoke_get_bluetoth
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_bluetooth(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_OBJECT_UNAVAILABLE;
    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
        case 0xF20B0200:
        {
            return class22_invoke_get_F20B0200(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }
        case 0xF20B0300:
        {
            return class22_invoke_get_F20B0300(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }
        case 0xF20B0400:
        {
            return class22_invoke_get_F20B0400(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }
        case 0xF20B0500:
        {
            return class22_invoke_get_F20B0500(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }
        case 0xF20B0600:
        {
            return class22_invoke_get_F20B0600(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }
        case 0xF20B0b00:
        {
            return class22_invoke_get_F20B0B00(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }
        default:  break;
    }
    return dar;
}

/**
*********************************************************************
* @name：       class22_invoke_get_service_info
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get_service_info(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF20F0200:
        return class22_invoke_get_reg_app_info(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @brief：      单个模块信息
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_mapinfo(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    uint8 buf[320] = {0};
    uint16 bufLen = 0;      //数组数量长度

    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    
    MOD_INFO_NEW_T *pModeInfo = (MOD_INFO_NEW_T *)convert->srcBuf;
    uint8 i = 0;

    //结构体类型
    buf[bufLen++] = DT_STRUCTURE;

    //数组成员个数
    buf[bufLen++] = 9;

    //槽位号
    buf[bufLen++] = DT_UNSIGNED;
    buf[bufLen++] = pModeInfo->port;

    //模块设备型号
    buf[bufLen++] = DT_VISIBLE_STRING;
    offlen = set_len_offset(pModeInfo->type.num, lenBuf);
    memcpy(buf + bufLen, lenBuf, offlen);
    bufLen += offlen;
    memcpy(buf + bufLen, pModeInfo->type.value, pModeInfo->type.num);
    bufLen += pModeInfo->type.num;

    //模块设备ID
    buf[bufLen++] = DT_VISIBLE_STRING;
    offlen = set_len_offset(pModeInfo->Id.num, lenBuf);
    memcpy(buf + bufLen, lenBuf, offlen);
    bufLen += offlen;
    memcpy(buf + bufLen, pModeInfo->Id.value, pModeInfo->Id.num);
    bufLen += pModeInfo->Id.num;

    //软件版本
    buf[bufLen++] = DT_VISIBLE_STRING;
    offlen = set_len_offset(pModeInfo->softVersion.num, lenBuf);
    memcpy(buf + bufLen, lenBuf, offlen);
    bufLen += offlen;
    memcpy(buf + bufLen, pModeInfo->softVersion.value, pModeInfo->softVersion.num);
    bufLen += pModeInfo->softVersion.num;

    //软件发布日期
    buf[bufLen++] = DT_VISIBLE_STRING;
    offlen = set_len_offset(pModeInfo->softDate.num, lenBuf);
    memcpy(buf + bufLen, lenBuf, offlen);
    bufLen += offlen;
    memcpy(buf + bufLen, pModeInfo->softDate.value, pModeInfo->softDate.num);
    bufLen += pModeInfo->softDate.num;

    //硬件版本
    buf[bufLen++] = DT_VISIBLE_STRING;
    offlen = set_len_offset(pModeInfo->hwVersion.num, lenBuf);
    memcpy(buf + bufLen, lenBuf, offlen);
    bufLen += offlen;
    memcpy(buf + bufLen, pModeInfo->hwVersion.value, pModeInfo->hwVersion.num);
    bufLen += pModeInfo->hwVersion.num;

    //硬件发布日期
    buf[bufLen++] = DT_VISIBLE_STRING;
    offlen = set_len_offset(pModeInfo->hwDate.num, lenBuf);
    memcpy(buf + bufLen, lenBuf, offlen);
    bufLen += offlen;
    memcpy(buf + bufLen, pModeInfo->hwDate.value, pModeInfo->hwDate.num);
    bufLen += pModeInfo->hwDate.num;

    //厂商编号
    buf[bufLen++] = DT_VISIBLE_STRING;
    offlen = set_len_offset(pModeInfo->manuCode.num, lenBuf);
    memcpy(buf + bufLen, lenBuf, offlen);
    bufLen += offlen;
    memcpy(buf + bufLen, pModeInfo->manuCode.value, pModeInfo->manuCode.num);
    bufLen += pModeInfo->manuCode.num;

    //通道配置
    buf[bufLen++] = DT_ARRAY;
    buf[bufLen++] = pModeInfo->num;
    for(i = 0; i < pModeInfo->num; i++)
    {
        buf[bufLen++] = DT_STRUCTURE;
        buf[bufLen++] = 2;

        //接口类型
        buf[bufLen++] = DT_UNSIGNED;
        buf[bufLen++] = pModeInfo->channel[i].type;
        //功能配置
        buf[bufLen++] = DT_UNSIGNED;
        buf[bufLen++] = pModeInfo->channel[i].function;
    }

    if(bufLen > convert->dstLen)
    {
        return DATA_SCOPE_OF_ACCESS_VIOLATED;   
    }

    memcpy(convert->dstBuf, buf, bufLen);

    *convert->dOffset += bufLen;

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      class22_invoke_get_mapmode
* @brief：     获取模组信息
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2020-5-30
*********************************************************************
*/
OOP_DAR_E class22_invoke_get_mapmode(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, 
                                     uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset  = 0;

    MEMZERO(&convert, sizeof(convert));

    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x02:
        {
            if(pOadInfo->nIndex > 5)
            {
                return DATA_HARDWARE_FAULT;
            }

            /* 查询单个设备 */
            if(0 != pOadInfo->nIndex)
            {
                uint8 buf[1024] = {0};
                uint16 msglen = 0;
                MODS_INFO_NEW_T tModes;

                msglen = appmain_get_map_info(pOadInfo->nIndex, buf, 1024);
                if(0 == msglen)
                {
                    PRTL_FMT_DEBUG("class22_invoke_get map_info failed, oad[0x%08x]\n", pOadInfo->pTab->oad.value);
                    return DATA_OBJECT_UNAVAILABLE;
                }
                memset(&tModes, 0, sizeof(MODS_INFO_NEW_T));
                appmsg_modsinfo_split_new(buf, &tModes);
                if(0 == tModes.num || 0 != tModes.info[0].status)
                {
                    PRTL_FMT_DEBUG("class22_invoke_get no map_info\n");
                    return DATA_OBJECT_UNAVAILABLE;
                }
                
                tModes.info[0].port = (uint8)pOadInfo->nIndex;

                //转换成报文数据
                convert.srcBuf = &(tModes.info[0]);
                convert.srcLen = sizeof(MOD_INFO_NEW_T);
                convert.sOffset = &sOffset;
                
                convert.dstBuf = &outData->pResultData[5];
                convert.dstLen = outDataMax;
                convert.dOffset = &outData->resultDataLen;

                dar = data_to_buf_mapinfo(pOadInfo, &convert);
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("data_to_buf_local_module_list failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
                    return dar;
                }
            }
            else
            {
                /* 多个设备 */
                uint8 buf[1024] = {0};
                uint16 msglen = 0;
                MODS_INFO_NEW_T tModes;
                uint8 i = 0;
                uint8 j = 0;
                uint8 num = 0;

                for(i = 0; i < 5; i++)
                {
                    bzero(buf, 1024);
                    msglen = appmain_get_map_info(i + 1, buf, 1024);
                    if(0 == msglen)
                    {
                        continue;
                    }
                    memset(&tModes, 0, sizeof(MODS_INFO_NEW_T));
                    appmsg_modsinfo_split_new(buf, &tModes);
                    if(0 == tModes.num)
                    {
                        continue;
                    }
                    for(j = 0; j< tModes.num; j++)
                    {
                        if(0 != tModes.info[j].status)
                        {
                            continue;
                        }
                        
                        if(TRUE == tModes.bSubMod)
                        {
                            tModes.info[j].port = tModes.info[j].subport;
                        }
                        tModes.info[j].port = (tModes.info[j].port << 4) + i + 1;
                        num++;

                        //转换成报文数据
                        convert.srcBuf = &(tModes.info[j]);
                        convert.srcLen = sizeof(MOD_INFO_NEW_T);
                        convert.sOffset = &sOffset;
                        
                        convert.dstBuf = outData->pResultData + 7 + outData->resultDataLen;
                        convert.dstLen = outDataMax;
                        convert.dOffset = &outData->resultDataLen;
        
                        dar = data_to_buf_mapinfo(pOadInfo, &convert);
                        if(dar != DATA_SUCCESS)
                        {
                            PRTL_FMT_DEBUG("data_to_buf_local_module_list failed. index(%d), oad(0x%08x)\n", i+1, pOadInfo->pTab->oad.value);
                            return dar;
                        }
                    }
                    //array
                    outData->pResultData[5] = DT_ARRAY;
                    outData->pResultData[6] = num;
                }
                if(0 == num)
                {
                    return DATA_OBJECT_UNAVAILABLE;
                }
                outData->resultDataLen += 2;
            }

            /* 填写报文开头处的OAD和数据结果0x01*/
            memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset++] = 0x01;
            outData->resultDataLen += offset;
            return dar;
        }
        default:
            return DATA_OBJECT_UNAVAILABLE;
    }
}


/**
*********************************************************************
* @name：       class22_invoke_get
* @brief：      class22的GET服务
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
OOP_DAR_E class22_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0xF200:
        return class22_invoke_get_rs232(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF201:
        return class22_invoke_get_rs485(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;    
    case 0xF202:
        return class22_invoke_get_ir(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0xF203:
        return class22_invoke_get_input(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0xF204:
        return class22_invoke_get_analog(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;    
    case 0xF205:
        sleep(2);//送检时，电科院测试主站发送完拉合闸命令之后立马读取继电器合闸状态，为了避免状态刷新不及时，加上1s延时回码,可以保证状态量刷新过
        if (pOadInfo->nIndex == 0)
        {
            return class22_invoke_get_switch(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }
        else
        {
            return class22_invoke_get_switch_unit(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }
        break;
    case 0xF206:
        return class22_invoke_get_warn(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF209:
        return class22_invoke_get_local_module(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0xF20B:
        return class22_invoke_get_bluetooth(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
    case 0xF20D:
        return class22_invoke_get_mapmode(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
    case 0xF20F:
        return class22_invoke_get_service_info(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name：       class22_invoke_act
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_act_rs232(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    dar = buf_to_data_class22_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2000300:
        {
            OOPM_ADDPORT_T dataAct;
            OOP_PORTS_T dataGet = {0};
            
            memcpy(&dataAct, convert.dstBuf, dOffset);

            dataGet.item[dataAct.index-1] = dataAct.times;

            normal.oad.nObjID  = pOadInfo->pTab->oad.nObjID;
            normal.oad.attID = 5;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataGet, sizeof(OOP_PORTS_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;    
    case 0xF2007F00:
        {
            char devName[50] = {0};
            OOPM_SERIALPORT_CONFIG_T dataAct;
            SERIALPORT               dataGet;
            memcpy(&dataAct, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.port.value = 0x%08x\n", dataAct.port.value);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.baud = %d\n", dataAct.param.baud);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.parity = %d\n", dataAct.param.parity);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.databits = %d\n", dataAct.param.databits);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.stopbits = %d\n", dataAct.param.stopbits);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.flowctrl = %d\n", dataAct.param.flowctrl);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.func = %d\n", dataAct.func);

            if ((dataAct.param.baud > 10 && dataAct.param.baud != 255) 
                ||(dataAct.param.parity > 2)
                ||(dataAct.param.databits > 8)
                ||(dataAct.param.stopbits > 2)
                ||(dataAct.param.flowctrl > 2))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            sprintf(devName, "RS232_%d", dataAct.port.nIndex);
            memcpy(&dataGet.name.value[0], devName, strlen(devName) + 1);
            dataGet.name.nNum = strlen(devName) + 1;
            dataGet.param = dataAct.param;
            dataGet.func  = dataAct.func;
            normal.oad = dataAct.port;   
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.portname = %s\n", dataGet.name.value);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.baud = %d\n", dataGet.param.baud);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.parity = %d\n", dataGet.param.parity);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.databits = %d\n", dataGet.param.databits);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.stopbits = %d\n", dataGet.param.stopbits);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.flowctrl = %d\n", dataGet.param.flowctrl);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.func = %d\n", dataGet.func); 

            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataGet, sizeof(SERIALPORT));
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
* @name：       class22_invoke_act
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_act_rs485(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    dar = buf_to_data_class22_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2010300:
        {
            OOPM_ADDPORT_T dataAct;
            OOP_PORTS_T dataGet = {0};
            
            memcpy(&dataAct, convert.dstBuf, dOffset);

            dataGet.item[dataAct.index-1] = dataAct.times;

            normal.oad.nObjID  = pOadInfo->pTab->oad.nObjID;
            normal.oad.attID = 5;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataGet, sizeof(OOP_PORTS_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;    
    case 0xF2017F00:
        {
            char devName[50] = {0};
            OOPM_SERIALPORT_CONFIG_T dataAct;
            SERIALPORT               dataGet;
            memcpy(&dataAct, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.port.value = 0x%08x\n", dataAct.port.value);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.baud = %d\n", dataAct.param.baud);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.parity = %d\n", dataAct.param.parity);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.databits = %d\n", dataAct.param.databits);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.stopbits = %d\n", dataAct.param.stopbits);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.flowctrl = %d\n", dataAct.param.flowctrl);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.func = %d\n", dataAct.func);
            sprintf(devName, "RS485_%d", dataAct.port.nIndex);
            memcpy(&dataGet.name.value[0], devName, strlen(devName) + 1);
            dataGet.name.nNum = strlen(devName) + 1;
            dataGet.param = dataAct.param;
            dataGet.func  = dataAct.func;
            normal.oad = dataAct.port;   
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.portname = %s\n", dataGet.name.value);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.baud = %d\n", dataGet.param.baud);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.parity = %d\n", dataGet.param.parity);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.databits = %d\n", dataGet.param.databits);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.stopbits = %d\n", dataGet.param.stopbits);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.flowctrl = %d\n", dataGet.param.flowctrl);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.func = %d\n", dataGet.func); 

            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataGet, sizeof(SERIALPORT));
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
* @name：       class22_invoke_act
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_act_ir(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    dar = buf_to_data_class22_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2020300:
        {
            OOPM_ADDPORT_T dataAct;
            OOP_PORTS_T dataGet = {0};
            
            memcpy(&dataAct, convert.dstBuf, dOffset);

            dataGet.item[dataAct.index-1] = dataAct.times;
            
            normal.oad.nObjID  = pOadInfo->pTab->oad.nObjID;
            normal.oad.attID = 5;            
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataGet, sizeof(OOP_PORTS_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;    
    case 0xF2027F00:
        {
            char devName[50] = {0};
            OOPM_IR_CONFIG_T dataAct;
            OOP_PORTCFG_T    dataGet;
            memcpy(&dataAct, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.oad.value = 0x%08x\n", dataAct.oad.value);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.baud = %d\n", dataAct.param.baud);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.parity = %d\n", dataAct.param.parity);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.databits = %d\n", dataAct.param.databits);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.stopbits = %d\n", dataAct.param.stopbits);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.param.flowctrl = %d\n", dataAct.param.flowctrl);
            sprintf(devName, "ir_%d", dataAct.oad.nIndex);
            memcpy(&dataGet.name.value[0], devName, strlen(devName) + 1);
            dataGet.name.nNum = strlen(devName) + 1;
            dataGet.param = dataAct.param;
            normal.oad = dataAct.oad;   
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.portname = %s\n", dataGet.name.value);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.baud = %d\n", dataGet.param.baud);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.parity = %d\n", dataGet.param.parity);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.databits = %d\n", dataGet.param.databits);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.stopbits = %d\n", dataGet.param.stopbits);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGet.param.flowctrl = %d\n", dataGet.param.flowctrl);

            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataGet, sizeof(OOP_PORTCFG_T));
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
* @name：       class22_invoke_act
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_act_switch(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class22_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2057F00:
        {
            uint32 i = 0;
            char devName[50] = {0};
            OOPM_SWITCHOUT_127_T dataAct;
            OOP_SWITCHOUT_T dataGetList;
            memcpy(&dataAct, convert.dstBuf, dOffset);
            MEMZERO(&dataGetList, sizeof(dataGetList));

            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.idx.value = 0x%08x\n", dataAct.idx.value);
            PRTL_FMT_LOGCC("class22_invoke_act : dataAct.property = %d\n", dataAct.property);

            normal.oad.value = 0xF2050200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_SWITCHOUT_T), (uint8*)&dataGetList, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            if ((dataAct.idx.value > 0xF2050208) || (dataAct.idx.value < 0xF2050201))
            {
                PRTL_FMT_DEBUG("class22_invoke_act_switch err[%d] dataAct.idx.value = 0x%08x\n",
                    DATA_OTHER_REASON, dataAct.idx.value);
                return DATA_OTHER_REASON;
            }

            i = dataAct.idx.value - 0xF2050201;
            if (dataGetList.nNum < i + 1)
            {
                dataGetList.nNum = i + 1;
            }

            sprintf(devName, "Relay-%d", dataAct.idx.nIndex);
            memcpy(&dataGetList.value[i].name.value[0], devName, strlen(devName) + 1);
            dataGetList.value[i].name.nNum = strlen(devName) + 1;
            dataGetList.value[i].attribute = dataAct.property;
            normal.oad.value = 0xF2050200;
            PRTL_FMT_LOGCC("class22_invoke_act : dataGetList.value[%d].portname = %s\n", i, dataGetList.value[i].name.value);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGetList.value[%d].attribute = %d\n", i, dataGetList.value[i].attribute);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGetList.value[%d].input = %d\n", i, dataGetList.value[i].input);
            PRTL_FMT_LOGCC("class22_invoke_act : dataGetList.value[%d].current = %d\n", i, dataGetList.value[i].current);

            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataGetList, sizeof(OOP_SWITCHOUT_T));
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
* @name：       class22_invoke_act
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_act_ble(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    dar = buf_to_data_check_meter(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_check_meter failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF20B8000:
        {
            OOPM_BLUETOOTHERRCHK_T dataAct;
            
            memset(&dataAct, 0, sizeof(dataAct));
            
            memcpy(&dataAct, convert.dstBuf, dOffset);

            ret = appmain_act_ble_checkmeter(&dataAct);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("设置蓝牙检定参数失败 ret(%d)\n",ret);
                return DATA_READ_WRITE_DENIED;
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
* @name：       class22_invoke_act
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_act_usb(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    dar = buf_to_data_class22_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF20E0300:
        {
            OOPM_ADDPORT_T dataAct;
            OOP_PORTS_T dataGet = {0};
            
            memcpy(&dataAct, convert.dstBuf, dOffset);

            dataGet.item[dataAct.index-1] = dataAct.times;

            normal.oad.nObjID  = pOadInfo->pTab->oad.nObjID;
            normal.oad.attID = 5;            
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataGet, sizeof(OOP_PORTS_T));
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

///**
//*********************************************************************
//* @name：       class22_invoke_act
//* @brief：      class22的ACTION服务
//* @param[in] ：pOdaInfo        OAD信息
//               pBpInfo        应用分帧断点信息
//               inData         输入报文，不含OAD
//               inLen          输入报文长度
//               outDataMax     输出报文缓冲区最大长度
//
//* @param[out]：outData     输出报文
//* @return    ：OOP_DAR_E
//* @author    : 
//* @Date      ：2019-12-6
//*********************************************************************
//*/
//OOP_DAR_E class22_invoke_act_transparent(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
//{
//    int ret = ERR_OK;
//    OOP_DAR_E dar = DATA_SUCCESS;
//    NOMAL_OAD_T normal = {0};
//    uint8 data[BUF_LEN_MAX] = {0};
//    DATA_CONVERT_T convert;
//    uint32 sOffset = 4;
//    uint32 dOffset = 0;
//    uint32 offset  = 0;
//    uint32 len = 0;   
//    Proxyresquest pProxyTaskLink;
//    
//  
//
//
//
//    //写入数据
//    switch (pOadInfo->pTab->oad.value)
//    {
//    case 0xF2097F00:
//        
//        break;
//    
//    default:
//        return DATA_OBJECT_UNAVAILABLE;
//        break;
//    }
//    
//    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
//    offset += sizeof(OOP_OAD_U);
//    outData->pResultData[offset ++] = 0x00;                            //DAR type
//    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
//    outData->resultDataLen = offset;
//
//    return dar;
//}


/**
*********************************************************************
* @name：       class22_invoke_act
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_act_plc(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
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
    dar = buf_to_data_class22_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class22_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0xF2097F00:
        {
           if(inData[sOffset]!=DT_STRUCTURE)
           {
				return DATA_TYPE_UNMATCHED;
           }
		   sOffset++;
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
* @name：       class22_invoke_act
* @brief：      class22的ACTION服务
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
OOP_DAR_E class22_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0xF200:
        return class22_invoke_act_rs232(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break;
    case 0xF201:
        return class22_invoke_act_rs485(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break; 
    case 0xF202:
        return class22_invoke_act_ir(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break; 
    case 0xF205:
        return class22_invoke_act_switch(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break; 
//    case 0xF209:
//        return class22_invoke_act_transparent(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
//        break;
    case 0xF20B:
        return class22_invoke_act_ble(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break; 
    case 0xF20E:
        return class22_invoke_act_usb(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break; 
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}


