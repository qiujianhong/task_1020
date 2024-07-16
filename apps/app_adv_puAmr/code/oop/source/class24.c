/*
*********************************************************************
* @file    class24.c
* @brief： class24处理
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
// class24
//

#if DESC("事件基础单元D2B",1)

/**
*********************************************************************
* @brief：       3000-电能表失压事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg3000(OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3000_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 4;
         
    //电压触发上限
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.wVUp, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //电压恢复下限
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.wVDown, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    //电流触发下限
    if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &evtcfg.nADown, sizeof(int32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    //判定延时时间
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       3001-电能表欠压事件
*               3002-电能表过压事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfgwd(OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_WD_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 2;
         
    //电压触发上限/下限
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.wValue, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //判定延时时间
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       3003-电能表断相事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg3003(OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3003_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 3;
         
    //电压触发上限
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.wVUp, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }      

    //电流触发上限
    if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &evtcfg.nAUp, sizeof(int32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    //判定延时时间
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       3004-电能表失流事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg3004(OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3004_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 4;
         

    //电压触发下限
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.wVDown, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    //电流触发上限
    if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &evtcfg.nAUp, sizeof(int32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }   

    //电流触发下限
    if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &evtcfg.nADown, sizeof(int32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    //判定延时时间
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       3006-电能表断流事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfg3006(OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3006_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 3;
         

    //电压触发下限
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.wVDown, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    //电流触发上限
    if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &evtcfg.nAUp, sizeof(int32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }      

    //判定延时时间
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief        配置参数-
*               3005-电能表过流事件
*               3007-电能表功率反向事件
*               3008-电能表过载事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfgid(OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_ID_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 2;
           

    //电流触发下限、有功功率触发下限
    if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &evtcfg.nValue, sizeof(int32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }      

    //判定延时时间
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief        配置参数-
*               3009-电能表正向有功需量超限事件
*               300A-电能表反向有功需量超限事件
*               300B-电能表无功需量超限事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfgdd(OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_DD_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 2;
           

    //触发下限
    if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.dwValue, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }      

    //判定延时时间
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  

    *pOffset = offset;
     
    return ERR_OK;
}

#endif

#if DESC("事件基础单元B2D",1)
/**
*********************************************************************
* @brief：       3000-电能表失压事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_evt_cfg3000(OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3000_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 4)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }

    //电压触发上限
    if(basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.wVUp, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //电压恢复下限
    if(basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.wVDown, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    //电流触发下限
    if(basic_buf_to_data(E_DOUBLE_LONG, srcBufTmp, srcLen-offset, &offset, &evtcfg.nADown, sizeof(int32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    //判定延时时间
    if(basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }     

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       3001-电能表欠压事件
*               3002-电能表过压事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_evt_cfgwd(OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_WD_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 4)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }

    //电压触发上限
    if(basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, dstLen-offset, &offset, &evtcfg.wValue, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }


    //判定延时时间
    if(basic_buf_to_data(E_UNSIGNED, srcBufTmp, dstLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }     

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       3003-电能表断相事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_evt_cfg3003(OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3003_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 3)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }

    //电压触发上限
    if(basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.wVUp, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    //电流触发上限
    if(basic_buf_to_data(E_DOUBLE_LONG, srcBufTmp, srcLen-offset, &offset, &evtcfg.nAUp, sizeof(int32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    //判定延时时间
    if(basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }     

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       3004-电能表失流事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_evt_cfg3004(OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3004_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 4)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }

    //电压触发下限
    if(basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.wVDown, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    //电流触发上限
    if(basic_buf_to_data(E_DOUBLE_LONG, srcBufTmp, srcLen-offset, &offset, &evtcfg.nAUp, sizeof(int32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }   

    //电流触发下限
    if(basic_buf_to_data(E_DOUBLE_LONG, srcBufTmp, srcLen-offset, &offset, &evtcfg.nADown, sizeof(int32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    //判定延时时间
    if(basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }     

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief：       3006-电能表断流事件配置参数
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_evt_cfg3006(OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_3006_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 3)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }

    //电压触发下限
    if(basic_buf_to_data(E_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.wVDown, sizeof(uint16)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }        

    //电流触发上限
    if(basic_buf_to_data(E_DOUBLE_LONG, srcBufTmp, srcLen-offset, &offset, &evtcfg.nAUp, sizeof(int32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  
    
    //判定延时时间
    if(basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }     

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief        配置参数-
*               3005-电能表过流事件
*               3007-电能表功率反向事件
*               3008-电能表过载事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_evt_cfgid(OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_ID_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 2)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }      

    //电流触发下限、有功功率触发下限
    if(basic_buf_to_data(E_DOUBLE_LONG, srcBufTmp, srcLen-offset, &offset, &evtcfg.nValue, sizeof(int32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  
    
    //判定延时时间
    if(basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }     

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief        配置参数-
*               3009-电能表正向有功需量超限事件
*               300A-电能表反向有功需量超限事件
*               300B-电能表无功需量超限事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 buf_to_data_evt_cfgdd(OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_DD_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }
    
     //结构成员数
     if(srcBufTmp[offset++] != 2)
     {
         nRet = ERR_NORMAL;
         return nRet;
     }      

    //触发下限
    if(basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.dwValue, sizeof(uint32)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  
    
    //判定延时时间
    if(basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen-offset, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }     

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return ERR_OK;
}

#endif

#if DESC("事件记录单元D2B",1)
/**
*********************************************************************
* @brief：      分项事件记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_mevent(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_MEVENT_T event;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL) )
    {
        PRTL_FMT_DEBUG("data_to_buf_class8_oop_meter param err\n");
        return ERR_PTR;
    }

    if (pOadInfo->pTab->eData != E_OOP_MEVENT_T)
    {
        PRTL_FMT_DEBUG("check data type failed.");
        return DATA_TYPE_UNMATCHED;
    }

    if (srcLen != sizeof(event))
    {
        PRTL_FMT_DEBUG("check data len failed.");
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&event, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 4 + event.data.oadNum;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    //事件记录序号 20220200
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, &dstBufTmp[offset], dstLen-offset, &offset, &event.nIndex, sizeof(event.nIndex));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //事件发生时间 201E0200
    dar = basic_data_to_buf(E_DATETIME_S, &dstBufTmp[offset], dstLen-offset, &offset, &event.tmStart, sizeof(event.tmStart));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }      

    //事件结束时间 20200200
    dar = basic_data_to_buf(E_DATETIME_S, &dstBufTmp[offset], dstLen-offset, &offset, &event.tmEnd, sizeof(event.tmEnd));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //事件上报状态 33000200
    dar = data_to_buf_evt_report(pOadInfo, &dstBufTmp[offset], dstLen-offset, &offset, &event.state, sizeof(event.state));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    } 

    //*********************************************以下为不同事件的特殊数据******************************************/

    //关联对象属性数据
    dar = data_to_buf_evt_coload(pOadInfo, &dstBufTmp[offset], dstLen-offset, &offset, &event.data, sizeof(event.data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;

    return ERR_OK;
}

#endif

#if DESC("数据结构转换D2B&B2D",1)

OOP_DAR_E data_to_buf_mevtstat(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;

    uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
    uint8 index = 0;
    MEVTSTATE *data = NULL;
         
    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_mevtstat error, ERR_PTR\n");
        return ERR_PTR;
    }


    if (pOadInfo->pTab->eData != E_OOP_MEVTSTAT_T)
    {
         PRTL_FMT_DEBUG("data_to_buf_mevtstat check data type failed. expect: E_OOP_EVTSTAT_T<->value: %d\n", pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_MEVTSTAT_T evtstat;

    if (dsdata->srcLen != sizeof(evtstat))
    {
         PRTL_FMT_DEBUG("data_to_buf_mevtstat check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&evtstat, srcDataTmp, dsdata->srcLen);

    if(pOadInfo->nIndex == 0)
    {
        dstBufTmp[offset++] = DT_ARRAY;
        dstBufTmp[offset++] = OOP_MAX_EVTSTAT;
        for(i = 0;i<OOP_MAX_EVTSTAT;i++)
        {
            if(0 == get_bit_value((uint8*)&evtstat.index,32,i))
            {
                dstBufTmp[offset++] = DT_NULL;
                continue;
            }
            
            data = &evtstat.stat[i];
            dstBufTmp[offset++] = DT_STRUCTURE;
            dstBufTmp[offset++] = 2;
            //事件发生次数
            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &data->dwCount, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

            //事件累计时间
            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &data->dwTimes, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
        }
    }else
    {
        index = pOadInfo->nIndex - 1;
        if(index >= OOP_MAX_EVTSTAT)
        {
            return ERR_NORMAL;
        }
        if(0 == get_bit_value((uint8*)&evtstat.index,32,index))
        {
            dstBufTmp[offset++] = DT_NULL;
            *dsdata->dOffset = offset;
        
             return ERR_OK;
        }
        data = &evtstat.stat[index];

        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 2;

        //事件发生次数
        if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &data->dwCount, sizeof(uint32)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }

        //事件累计时间
        if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &data->dwTimes, sizeof(uint32)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }
    }
    
    *dsdata->dOffset = offset;

    return ERR_OK;
}

OOP_DAR_E data_to_buf_evtcfg_3003(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;

    //uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
         
    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_mevttime error, ERR_PTR\n");
        return ERR_PTR;
    }


    if (pOadInfo->pTab->eData != E_OOP_EVTCFG_3003_T)
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data type failed. expect: E_OOP_EVTCFG_WD_T<->value: %d\n", pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_EVTCFG_3003_T evtcfg;

    if (dsdata->srcLen != sizeof(evtcfg))
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&evtcfg, srcDataTmp, dsdata->srcLen);

    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 0x03;

    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.wVUp, sizeof(evtcfg.wVUp)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.nAUp, sizeof(evtcfg.nAUp)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.nDelay, sizeof(evtcfg.nDelay)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    *dsdata->dOffset = offset;

    return ERR_OK;
}

OOP_DAR_E data_to_buf_evtcfg_3000(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;

    //uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
         
    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_mevttime error, ERR_PTR\n");
        return ERR_PTR;
    }


    if (pOadInfo->pTab->eData != E_OOP_EVTCFG_3000_T)
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data type failed. expect: E_OOP_EVTCFG_WD_T<->value: %d\n", pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_EVTCFG_3000_T evtcfg;

    if (dsdata->srcLen != sizeof(evtcfg))
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&evtcfg, srcDataTmp, dsdata->srcLen);

    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 0x04;

    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.wVUp, sizeof(evtcfg.wVUp)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.wVDown, sizeof(evtcfg.wVDown)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.nADown, sizeof(evtcfg.nADown)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.nDelay, sizeof(evtcfg.nDelay)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    *dsdata->dOffset = offset;

    return ERR_OK;
}

OOP_DAR_E data_to_buf_evtcfg_3004(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;

    //uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
         
    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_mevttime error, ERR_PTR\n");
        return ERR_PTR;
    }


    if (pOadInfo->pTab->eData != E_OOP_EVTCFG_3004_T)
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data type failed. expect: E_OOP_EVTCFG_WD_T<->value: %d\n", pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_EVTCFG_3004_T evtcfg;

    if (dsdata->srcLen != sizeof(evtcfg))
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&evtcfg, srcDataTmp, dsdata->srcLen);

    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 0x04;

    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.wVDown, sizeof(evtcfg.wVDown)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.nAUp, sizeof(evtcfg.nAUp)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.nADown, sizeof(evtcfg.nADown)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.nDelay, sizeof(evtcfg.nDelay)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    *dsdata->dOffset = offset;

    return ERR_OK;
}

OOP_DAR_E data_to_buf_evtcfg_3006(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;

    //uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
         
    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_mevttime error, ERR_PTR\n");
        return ERR_PTR;
    }


    if (pOadInfo->pTab->eData != E_OOP_EVTCFG_3006_T)
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data type failed. expect: E_OOP_EVTCFG_WD_T<->value: %d\n", pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_EVTCFG_3006_T evtcfg;

    if (dsdata->srcLen != sizeof(evtcfg))
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&evtcfg, srcDataTmp, dsdata->srcLen);

    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 0x03;

    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.wVDown, sizeof(evtcfg.wVDown)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }

    if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.nAUp, sizeof(evtcfg.nAUp)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.nDelay, sizeof(evtcfg.nDelay)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    *dsdata->dOffset = offset;

    return ERR_OK;
}

OOP_DAR_E data_to_buf_evtcfg_id(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;

    //uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
         
    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_mevttime error, ERR_PTR\n");
        return ERR_PTR;
    }


    if (pOadInfo->pTab->eData != E_OOP_EVTCFG_ID_T)
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data type failed. expect: E_OOP_EVTCFG_WD_T<->value: %d\n", pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_EVTCFG_ID_T evtcfg;

    if (dsdata->srcLen != sizeof(evtcfg))
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&evtcfg, srcDataTmp, dsdata->srcLen);

    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 0x02;

    if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.nValue, sizeof(evtcfg.nValue)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.nDelay, sizeof(evtcfg.nDelay)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    *dsdata->dOffset = offset;

    return ERR_OK;
}


OOP_DAR_E data_to_buf_evt_oadss(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{

    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;

    //uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
         
    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_mevttime error, ERR_PTR\n");
        return ERR_PTR;
    }


    if (pOadInfo->pTab->eData != E_OOP_OADS_T)
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data type failed. expect: E_OOP_EVTCFG_WD_T<->value: %d\n", pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_OADS_T oopLog;

    if (dsdata->srcLen != sizeof(oopLog))
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&oopLog, srcDataTmp, dsdata->srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_ARRAY;
    dstBufTmp[offset++] = oopLog.nNum;

    for(uint32 i =0; i < oopLog.nNum; i++)
    {
        if (basic_data_to_buf(E_OAD, dstBufTmp, dsdata->dstLen-offset, &offset, &oopLog.oad[i], sizeof(oopLog.oad[i])) < 0)
        {
            
            return DATA_TYPE_UNMATCHED;
        }
    }
    
    *dsdata->dOffset = offset;

    return ERR_OK;
    
}


OOP_DAR_E data_to_buf_evtcfg_wd(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;

    //uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
         
    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_mevttime error, ERR_PTR\n");
        return ERR_PTR;
    }


    if (pOadInfo->pTab->eData != E_OOP_EVTCFG_WD_T)
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data type failed. expect: E_OOP_EVTCFG_WD_T<->value: %d\n", pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_EVTCFG_WD_T evtcfg;

    if (dsdata->srcLen != sizeof(evtcfg))
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&evtcfg, srcDataTmp, dsdata->srcLen);

    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 0x02;

    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.wValue, sizeof(evtcfg.wValue)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &evtcfg.nDelay, sizeof(evtcfg.nDelay)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }
    
    *dsdata->dOffset = offset;

    return ERR_OK;
}

OOP_DAR_E data_to_buf_mevttime(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;

    uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
    uint8 index = 0;
    MEVTTIME *data = NULL;
         
    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_mevttime error, ERR_PTR\n");
        return ERR_PTR;
    }


    if (pOadInfo->pTab->eData != E_OOP_MEVTTIME_T)
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data type failed. expect: E_OOP_MEVTTIME_T<->value: %d\n", pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_MEVTTIME_T evttime;

    if (dsdata->srcLen != sizeof(evttime))
    {
         PRTL_FMT_DEBUG("data_to_buf_mevttime check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&evttime, srcDataTmp, dsdata->srcLen);

    if(pOadInfo->nIndex == 0)
    {
        dstBufTmp[offset++] = DT_ARRAY;
        dstBufTmp[offset++] = OOP_MAX_EVTSTAT;
        for(i = 0;i<OOP_MAX_EVTSTAT;i++)
        {
            if(0 == get_bit_value((uint8*)&evttime.index,32,i))
            {
                dstBufTmp[offset++] = DT_NULL;
                continue;
            }
            
            data = &evttime.stat[i];
            dstBufTmp[offset++] = DT_STRUCTURE;
            dstBufTmp[offset++] = 2;
            //最近一次发生时间
            if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dsdata->dstLen-offset, &offset, &data->tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            
            //最近一次结束时间
            if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dsdata->dstLen-offset, &offset, &data->tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

        }

    }else
    {
        index = pOadInfo->nIndex - 1;
        if(index >= OOP_MAX_EVTSTAT)
        {
            return ERR_NORMAL;
        }
        data = &evttime.stat[index];
        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 2;
       //最近一次发生时间
        if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dsdata->dstLen-offset, &offset, &data->tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }

        //最近一次结束时间
        if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dsdata->dstLen-offset, &offset, &data->tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }
    }
    
    *dsdata->dOffset = offset;

    return ERR_OK;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class24)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class24(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_MEVTSTAT_T:
        {
            return data_to_buf_mevtstat(pOadInfo, dsdata);
        }
        break;
        case E_OOP_MEVTTIME_T:
        {
            return data_to_buf_mevttime(pOadInfo, dsdata);
        }
            break;
        case E_OOP_OADS_T://一组对象属性描述符
            return data_to_buf_evt_oadss(pOadInfo, dsdata);
        break; 
        case E_OOP_EVTCFG_WD_T:
        {
            return data_to_buf_evtcfg_wd(pOadInfo, dsdata);
        }
            break;
        case E_OOP_EVTCFG_ID_T:
        {
            return data_to_buf_evtcfg_id(pOadInfo, dsdata);
        }
            break;
        case E_OOP_EVTCFG_3000_T:
        {
            return data_to_buf_evtcfg_3000(pOadInfo, dsdata);
        }
        case E_OOP_EVTCFG_3003_T:
        {
            return data_to_buf_evtcfg_3003(pOadInfo, dsdata);
        }
        case E_OOP_EVTCFG_3004_T:
        {
            return data_to_buf_evtcfg_3004(pOadInfo, dsdata);
        }
        case E_OOP_EVTCFG_3006_T:
        {
            return data_to_buf_evtcfg_3006(pOadInfo, dsdata);
        }
            break;
        default:
        {
            return basic_db_to_fmt698(pOadInfo,  dsdata);
        }
            break;
    }

    return 0;
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
OOP_DAR_E buf_to_data_class24(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}

#endif

#if DESC("SET服务的内部接口",1)

/**
*********************************************************************
* @name：       evt_cfg3000_set
* @brief       3000-电能表失压事件配置参数
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3000_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_3000_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfg3000(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_3000_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg3000 failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_3000_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg3000 db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfgwd_set
* @brief：      配置参数- 
               3001-电能表欠压事件
*              3002-电能表过压事件
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgwd_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_WD_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfgwd(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_WD_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfgwd failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_WD_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfgwd db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfg3003_set
* @brief：      3003-电能表断相事件配置参数
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3003_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_3003_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfg3003(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_3003_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg3003 failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_3003_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg3003 db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfg3004_set
* @brief：      3004-电能表失流事件配置参数
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3004_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_3004_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfg3004(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_3004_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg3004 failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_3004_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg3004 db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfg3006_set
* @brief：      3006-电能表断流事件配置参数
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3006_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_3006_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfg3006(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_3006_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg3006 failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_3006_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg3006 db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfgid_set
* @brief：      配置参数- 
*              3005-电能表过流事件
*              3007-电能表功率反向事件
*              3008-电能表过载事件
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgid_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_ID_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfgid(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_ID_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg3006 failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_ID_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg3006 db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfgdd_set
* @brief：      配置参数- 
*              3009-电能表正向有功需量超限事件
*              300A-电能表反向有功需量超限事件
*              300B-电能表无功需量超限事件
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgdd_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_DD_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfgdd(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_DD_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_cfg3006 failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_DD_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_cfg3006 db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       分项事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_mcfgparam_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    const uint8 *bufData = inData + 4; //偏掉OAD
    uint16 bufLen = InDataLen - 4;
    uint32 offset = 0;

    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_EVTCFG_3000_T://电能表失压事件配置参数
            dar = evt_cfg3000_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break;
        case E_OOP_EVTCFG_WD_T:  //电能表欠压事件
            dar = evt_cfgwd_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break;          
        case E_OOP_EVTCFG_3003_T://电能表断相事件配置参数
            dar = evt_cfg3003_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 
        case E_OOP_EVTCFG_3004_T://电能表失流事件配置参数
            dar = evt_cfg3004_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 
        case E_OOP_EVTCFG_3006_T://电能表断流事件配置参数
            dar = evt_cfg3006_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 
        case E_OOP_EVTCFG_ID_T:  //电能表过流事件
            dar = evt_cfgid_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 
        case E_OOP_EVTCFG_DD_T:  //电能表正向有功需量超限事件
            dar = evt_cfgdd_set(pOadInfo, pApduInfo, bufData, bufLen, &offset);
        break; 
        default:
            dar = DATA_READ_WRITE_DENIED;
        break;
    }

    if(offset > bufLen)
    {
        dar = DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    return dar;
}


#endif

#if DESC("GET服务的内部接口",1)
/**
*********************************************************************
* @name：       evt_cfg3000_get
* @brief       3000-电能表失压事件配置参数
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3000_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_3000_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_3000_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_3000_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
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

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg3000(pOadInfo, outData->pResultData, outData->resultDataLen, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_cfg3000 failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfgwd_get
* @brief       3000-电能表失压事件配置参数
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgwd_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_WD_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_WD_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_WD_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
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

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfgwd(pOadInfo, outData->pResultData, outData->resultDataLen, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_cfgwd failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfg3003_get
* @brief       3000-电能表失压事件配置参数
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3003_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_3003_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_3003_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_3003_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
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

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg3003(pOadInfo, outData->pResultData, outData->resultDataLen, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_cfg3003 failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfg3004_get
* @brief       3000-电能表失压事件配置参数
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3004_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_3004_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_3004_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_3004_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
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

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg3004(pOadInfo, outData->pResultData, outData->resultDataLen, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_cfg3004 failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfg3006_get
* @brief       3000-电能表失压事件配置参数
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfg3006_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_3006_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_3006_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_3006_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
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

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfg3006(pOadInfo, outData->pResultData, outData->resultDataLen, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_cfg3006 failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfgid_get
* @brief       3000-电能表失压事件配置参数
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgid_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_ID_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_ID_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_ID_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
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

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfgid(pOadInfo, outData->pResultData, outData->resultDataLen, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_cfgid failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       evt_cfgdd_get
* @brief       3000-电能表失压事件配置参数
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgdd_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_DD_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_DD_T));

    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_EVTCFG_DD_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
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

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfgdd(pOadInfo, outData->pResultData, outData->resultDataLen, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_cfgid failed. dar(%d), oad(0x%08x)", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       evt_rectable_get
* @brief：      读关联对象属性表
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
OOP_DAR_E evt_mcfgparam_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_EVTCFG_3000_T://电能表失压事件配置参数
            dar = evt_cfg3000_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
        case E_OOP_EVTCFG_WD_T:  //电能表欠压事件
            dar = evt_cfgwd_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break;          
        case E_OOP_EVTCFG_3003_T://电能表断相事件配置参数
            dar = evt_cfg3003_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
        case E_OOP_EVTCFG_3004_T://电能表失流事件配置参数
            dar = evt_cfg3004_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
        case E_OOP_EVTCFG_3006_T://电能表断流事件配置参数
            dar = evt_cfg3006_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
        case E_OOP_EVTCFG_ID_T:  //电能表过流事件
            dar = evt_cfgid_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
        case E_OOP_EVTCFG_DD_T:  //电能表正向有功需量超限事件
            dar = evt_cfgdd_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 

        default:
            dar = DATA_READ_WRITE_DENIED;
        break;
    }

    return dar;
}


#endif

#if DESC("class24对外接口函数",1)

/**
*********************************************************************
* @name：       class24_invoke_set
* @brief：      class24的SET服务
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
OOP_DAR_E class24_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 2:   //关联对象属性表
        {
            dar = evt_asobjattr_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;        
        case 5:   //配置参数
        {
            dar = evt_mcfgparam_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;
        case 11:   //上报标识
        {
            dar = evt_report_flag_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;        
        case 12:   //有效标识
        {
            dar = evt_valid_flag_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        default: 
            dar = DATA_READ_WRITE_DENIED; 
        break;
    }

    if(dar == DATA_SUCCESS)
    {
        //拷贝OAD
        memcpy_r(outData->pResultData, inData, sizeof(uint32));
        outData->resultDataLen += sizeof(uint32);

        //不包含DAR,统一外面处理
    }

    return dar;
}


/**
*********************************************************************
* @name：       class24_invoke_get
* @brief：      class24的GET服务
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
OOP_DAR_E class24_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    
    switch (pOadInfo->pTab->oad.attID)
    {
        case 2:  //关联对象属性表
        {
            return evt_objattr_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case 5:  //配置参数
        {
            return evt_mcfgparam_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case 6:  //事件记录表1
        case 7:  //事件记录表2
        case 8:  //事件记录表3
        case 9:  //事件记录表4
        {
            return evt_rectable_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case 11:  //上报标识
        {
            return evt_report_flag_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case 12:  //有效标识
        {
            return evt_valid_flag_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
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
* @name：       class24_invoke_act
* @brief：      class24的ACTION服务
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
OOP_DAR_E class24_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 1:   //复位
        {
            dar = evt_reset(pOadInfo,  pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break; 
        
        case 4:   //添加一个事件关联对象属性
        {
            dar = evt_objattr_unit_add(pOadInfo,  pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;   
        
        case 5:   //删除一个事件关联对象属性
        {
            dar = evt_objattr_unit_del(pOadInfo,  pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;
        
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;
}


#endif
