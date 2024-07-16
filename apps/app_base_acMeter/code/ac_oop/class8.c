/*
*********************************************************************
* @file    class8.c
* @brief： class8处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "commClass.h"
#include "oopservice.h"
#include "ac_sample.h"
#include "ac_main.h"

extern void sta_time_update_fset(BOOLEAN isUpdate);

//
// class8
//

/**
*********************************************************************
* @brief：      class8 分项数据
* @param[in] ： 无
* @param[out]   pDesbuffer 原始数据结构体
*               nDesLen    原始数据长度
*               OAD        分项数据标识
* @return：     >0            - 成功
               <0            - 错误码
*********************************************************************
*/
int combine_sub_data_class8_buff(uint8 *pDesbuffer, uint16 *nDesLen, uint32 OAD)
{
    int index = 0;

    switch(OAD)
    {
        /*class 8*/
        case 0x40160201:               //当前套日时段表 第1套
        case 0x40160202:               //当前套日时段表 第2套
        case 0x40160203:               //当前套日时段表 第3套
        case 0x40160204:               //当前套日时段表 第4套
        case 0x40160205:               //当前套日时段表 第5套
        case 0x40160206:               //当前套日时段表 第6套
        case 0x40160207:               //当前套日时段表 第7套
        case 0x40160208:               //当前套日时段表 第8套
        {
            OOP_DAYPERIOD_T data = {0};
            data.nNum = 1;
            index = OAD - 0x40160201;

            if(0 == TermSharedData.g_nCurrDayPeriLabel)//当前套是第一套
            {
                memcpy(&data.dPeriod[index], &TermSharedData.CurrDayPeriTableRD.dPeriod[index], sizeof(DAYPERIOD));
            }
            else
            {
                memcpy(&data.dPeriod[index], &TermSharedData.AddiDayPeriTableRD.dPeriod[index], sizeof(DAYPERIOD));
            }
            *nDesLen = sizeof(OOP_DAYPERIOD_T);
       		memcpy(pDesbuffer, &data, sizeof(OOP_DAYPERIOD_T));
        }
            break;

        default:
			return -1;
            break;
    }

    return 1;
}


/**
*********************************************************************
* @brief：     获取系统时间
* @param[in]： 
* @param[out]：DateTime_s格式时间
* @return：    null
*********************************************************************
*/
void systime_get(DateTimeHex_t *pDateTime)
{
    time_t linuxTime=time(NULL);

    DT_Time2DateTimeHex(linuxTime,pDateTime);
    //memrev((uint8*)pDateTime,7);
}

/**
*********************************************************************
* @brief：      交采时间
* @param[in] ： 无
* @param[out]   pDesbuffer 原始数据结构体
*               nDesLen    原始数据长度
* @return：     >0            - 成功
               <0            - 错误码
*********************************************************************
*/
int combine_time_normal_buff(uint8 *pDesbuffer, uint16 *nDesLen)
{
    OOP_DATETIME_S_T data = {0};
    DateTimeHex_t dataHex;
    memset(&dataHex,0,sizeof(DateTimeHex_t));
    systime_get((DateTimeHex_t*)&dataHex);
    memcpy(&data, &dataHex, sizeof(DateTimeHex_t));
    memrev((uint8*)&data, 2);      //OOP_DATETIME_S_T的年高底位反转
    ACUDP_FMT_LOGCC("combine_time_normal_buff : data.year = %d\n", data.year);
    ACUDP_FMT_LOGCC("combine_time_normal_buff : data.month = %d\n", data.month);
    ACUDP_FMT_LOGCC("combine_time_normal_buff : data.day = %d\n", data.day);
    ACUDP_FMT_LOGCC("combine_time_normal_buff : data.hour = %d\n", data.hour);
    ACUDP_FMT_LOGCC("combine_time_normal_buff : data.minute = %d\n", data.minute);
    ACUDP_FMT_LOGCC("combine_time_normal_buff : data.second = %d\n", data.second);

    *nDesLen = sizeof(DateTimeHex_t);
	memcpy(pDesbuffer, &data, sizeof(DateTimeHex_t));

    return 1;
}


/**
*********************************************************************
* @brief：      安全模式选择
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_time(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_DATETIME_S_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data)); 

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //时间转换
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),nDepth(%d)\n", i, dar, data);
        return DATA_TYPE_UNMATCHED;
    }        

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        //return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief：       时区时段数
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_time_region(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_PERIOD_T data = {0};
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 5;

    //年时区数
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.yPeriod, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),data.height(%d)\n", dar, data.yPeriod);
        return DATA_TYPE_UNMATCHED;
    }

    //日时段表数
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriodMet, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),data.height(%d)\n", dar, data.dPeriodMet);
        return DATA_TYPE_UNMATCHED;
    }

    //日时段数
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),data.height(%d)\n", dar, data.dPeriodMet);
        return DATA_TYPE_UNMATCHED;
    }

    //费率数
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.rate, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),data.height(%d)\n", dar, data.rate);
        return DATA_TYPE_UNMATCHED;
    }

    //公共假日数
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.pubHoliday, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),data.height(%d)\n", dar, data.rate);
        return DATA_TYPE_UNMATCHED;
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
* @brief：      当前套时段表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_time_zone(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TIMEZONE_T data = {0};
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

        //月
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.period[i].mon, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.period.mon(%d)\n", i, dar, data.period[i].mon);
            return DATA_TYPE_UNMATCHED;
        }

        //日
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.period[i].day, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.period.day(%d)\n", i, dar, data.period[i].day);
            return DATA_TYPE_UNMATCHED;
        }

        //日时段表号
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.period[i].meterNum, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.period.meterNum(%d)\n", i, dar, data.period[i].meterNum);
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
* @brief：      当前套日时段表
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_day_period(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_DAYPERIOD_T data = {0};
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 j = 0;
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
        //数组类型
        dstBuf[dOffset++] = DT_ARRAY;

        //数组成员个数
        offlen = set_len_offset(data.dPeriod[i].nNum, lenBuf);
        memcpy(&dstBuf[dOffset], lenBuf, offlen);
        dOffset += offlen;

        for(j = 0; j < data.dPeriod[i].nNum; j++)
        {
            //结构类型
            dstBuf[dOffset++] = DT_STRUCTURE;

            //结构成员数
            dstBuf[dOffset++] = 3;
            
            //时
            dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod[i].period[j].hour, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.dPeriod[i].period[j].hour(%d)\n", i, dar, data.dPeriod[i].period[j].hour);
                return DATA_TYPE_UNMATCHED;
            }

            //分
            dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod[i].period[j].min, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.dPeriod[i].period[j].min(%d)\n", i, dar, data.dPeriod[i].period[j].min);
                return DATA_TYPE_UNMATCHED;
            }

            //费率号
            dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod[i].period[j].rateNO, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.dPeriod[i].period[j].rateNO(%d)\n", i, dar, data.dPeriod[i].period[j].rateNO);
                return DATA_TYPE_UNMATCHED;
            }
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
* @brief：      显式安全模式参数
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_day_period_one(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_DAYPERIOD_T data = {0};
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 j = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    if ((pOadInfo->nIndex > OOP_MAX_DAYPERIODS) || (pOadInfo->nIndex == 0))
    {
        return ERR_NORMAL;
    }

    i = pOadInfo->nIndex - 1;

    //数组类型
    dstBuf[dOffset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(data.dPeriod[i].nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for(j = 0; j < data.dPeriod[i].nNum; j++)
    {
        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 3;
        
        //时
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod[i].period[j].hour, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.dPeriod[i].period[j].hour(%d)\n", i, dar, data.dPeriod[i].period[j].hour);
            return DATA_TYPE_UNMATCHED;
        }

        //分
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod[i].period[j].min, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.dPeriod[i].period[j].min(%d)\n", i, dar, data.dPeriod[i].period[j].min);
            return DATA_TYPE_UNMATCHED;
        }

        //费率号
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod[i].period[j].rateNO, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.dPeriod[i].period[j].rateNO(%d)\n", i, dar, data.dPeriod[i].period[j].rateNO);
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
* @brief：      最大需量周期
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_demand_period(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8  data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data)); 

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //资产编码
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d)\n", 0, dar);
        return DATA_TYPE_UNMATCHED;
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
* @brief：      滑差时间
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_slip_time(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8  data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data)); 

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //时间
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d)\n", 0, dar);
        return DATA_TYPE_UNMATCHED;
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
* @brief：     从数据中心的数据转698格式报文(class8)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x40000200:               //获取系统时间
        case 0x40080200:			   //备用套时区表切换时间
        case 0x40090200:			   //备用套日时段切换时间
        {
           return data_to_buf_class8_time(pOadInfo,  convert);
        }break;
        
        case 0x400C0200:			   //时区时段数
        {
           return data_to_buf_class8_time_region(pOadInfo,  convert);
        }break;
        
        case 0x40140200:   //当前套时区表
        case 0x40150200:   //备用套时区表
        {
           return data_to_buf_class8_time_zone(pOadInfo,  convert);
        }break;

        case 0x40160200:   //当前套日时段表
        case 0x40170200:   //备用套日时段表
        {
           return data_to_buf_class8_day_period(pOadInfo,  convert);
        }break;

        case 0x40160201:               //当前套日时段表 第1套
        case 0x40160202:               //当前套日时段表 第2套
        case 0x40160203:               //当前套日时段表 第3套
        case 0x40160204:               //当前套日时段表 第4套
        case 0x40160205:               //当前套日时段表 第5套
        case 0x40160206:               //当前套日时段表 第6套
        case 0x40160207:               //当前套日时段表 第7套
        case 0x40160208:               //当前套日时段表 第8套
        {
           return data_to_buf_class8_day_period_one(pOadInfo,  convert);
        }break;

        case 0x41000200:   //需量周期
        {
           return data_to_buf_class8_demand_period(pOadInfo,  convert);
        }break;

        case 0x41010200:   //滑差时间
        {
           return data_to_buf_class8_slip_time(pOadInfo,  convert);
        }break;

        case 0x41120200:			   //有功组合方式特征字
        case 0x41130200:			   //无功组合方式1特征字
        case 0x41140200:			   //无功组合方式2特征字
        {
           return basic_db_to_fmt698(pOadInfo,  convert);
        }break;

        //......
        
        default:
        {
            ACUDP_FMT_TRACE("data_to_buf not support. oad(0x%08x)\n", pOadInfo->pTab->oad.value);
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：      设置时间
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
OOP_DAR_E buf_to_data_class8_time(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_DATETIME_S_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    
    //时间
    dar = basic_buf_to_data(E_DATETIME_S, srcBuf, srcLen, &sOffset, &data, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief：      时区时段数
* @param[in]： ppOadInfo    - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf       - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：    =0           - 成功
               !=0          - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_time_region(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_PERIOD_T data = {0};
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if(srcBuf[sOffset++] != 5)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //年时区数
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.yPeriod, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //日时段表数
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.dPeriodMet, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //日时段数
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.dPeriod, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //费率数
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.rate, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //公共假日数
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.pubHoliday, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief：     当前套时区表
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
OOP_DAR_E buf_to_data_class8_time_zone(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TIMEZONE_T data = {0};
    uint8 offlen = 0;      //数组数量长度
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //数组类型
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
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
            ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        
        //结构成员数
        if(srcBuf[sOffset++] != 3)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }  
        
        //月
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period[i].mon, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //日
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period[i].day, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }     

        //日时段表号
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period[i].meterNum, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }           
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief：     当前套日时段表
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
OOP_DAR_E buf_to_data_class8_day_period_all(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_DAYPERIOD_T data = {0};
    uint8 offlen = 0;      //数组数量长度
    uint8 i = 0;
    uint8 j = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //数组类型
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组成员个数
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {   
        //数组类型
        if(srcBuf[sOffset++] != DT_ARRAY)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //数组成员个数
        data.dPeriod[i].nNum = get_len_offset(&srcBuf[sOffset], &offlen);
        sOffset += offlen;

        for(j = 0; j < data.dPeriod[i].nNum; j++)
        {
            //结构类型
            if(srcBuf[sOffset++] != DT_STRUCTURE)
            {
                ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
                return DATA_TYPE_UNMATCHED;
            }

            //结构成员数
            if(srcBuf[sOffset++] != 3)
            {
                ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
                return DATA_TYPE_UNMATCHED;
            }  

            //时
            dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.dPeriod[i].period[j].hour, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            } 

            //分
            dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.dPeriod[i].period[j].min, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            } 

            //秒
            dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.dPeriod[i].period[j].rateNO, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            } 
        }     
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief：     当前套日时段表
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
OOP_DAR_E buf_to_data_class8_day_period_one(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    DAYPERIOD data = {0};
    uint8 offlen = 0;      //数组数量长度
    uint8 i = 0;
    uint8 j = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //数组类型
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组成员个数
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    for(j = 0; j < data.nNum; j++)
    {
        //结构类型
        if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //结构成员数
        if(srcBuf[sOffset++] != 3)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }  

        //时
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period[j].hour, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //分
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period[j].min, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //秒
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period[j].rateNO, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 
    }     

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief：     当前套日时段表
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
OOP_DAR_E buf_to_data_class8_day_period(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    if (pOadInfo->nIndex == 0)
    {
        return buf_to_data_class8_day_period_all(pOadInfo, convert);
    }
    else
    {
        return buf_to_data_class8_day_period_one(pOadInfo, convert);
    }
}

/**
*********************************************************************
* @brief：      最大需量周期
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
OOP_DAR_E buf_to_data_class8_demand_max(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //校时模式
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    ACUDP_FMT_TRACE("buf_to_data_class8_demand_max : data = 0x%x\n", data);
     
    return dar;
}

/**
*********************************************************************
* @brief：      滑差时间
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
OOP_DAR_E buf_to_data_class8_slip_time(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    
    //校时模式
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    ACUDP_FMT_TRACE("buf_to_data_class8_demand_max : data = 0x%x\n", data);
     
    return dar;
}

/**
*********************************************************************
* @brief：      特征字
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
OOP_DAR_E buf_to_data_class8_feature(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    
    //校时模式
    dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_buf_to_data failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    ACUDP_FMT_TRACE("buf_to_data_class8_feature : data = 0x%x\n", data);
     
    return dar;
}


/**
*********************************************************************
* @brief：     组合特征字
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
OOP_DAR_E buf_to_data_class8_combine_word(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_BITSTR8_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //特征字
    dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &data, sizeof(data));
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
* @brief：     设置电压合格率参数
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
OOP_DAR_E buf_to_data_class8_volt_rate(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_VOLTPARAM_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if(srcBuf[sOffset++] != 4)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //电压考核上限
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.uupVolt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //电压考核下限
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.lldVolt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //电压合格上限
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.upVolt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }     

    //电压合格下限
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.ldVolt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
OOP_DAR_E buf_to_data_class8_set(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x40000200:   //设置时间
        case 0x40080200:   
        case 0x40090200:   
        case 0x400A0200:  
        {
            return buf_to_data_class8_time(pOadInfo,  convert);
        }break;

        case 0x400C0200:   //时段时区数
        {
            return buf_to_data_class8_time_region(pOadInfo,  convert);
        }break;

        case 0x40140200:   //当前套时区表
        case 0x40150200:   //备用套时区表
        {
            return buf_to_data_class8_time_zone(pOadInfo,  convert);
        }break;

        case 0x40160200:   //当前套日时段表
        case 0x40170200:   //当前套日时段表
        {
            return buf_to_data_class8_day_period(pOadInfo,  convert);
        }break;

        case 0x40300200:   //电压合格率
        {
            return buf_to_data_class8_volt_rate(pOadInfo,  convert);
        }break;

        case 0x41000200:   //需量周期
        {
            return buf_to_data_class8_demand_max(pOadInfo,  convert);
        }break;
        
        case 0x41010200:   //滑差时间
        {
            return buf_to_data_class8_slip_time(pOadInfo,  convert);
        }break;
        
        case 0x41120200:   //组合特征字
        case 0x41130200:   //组合特征字
        case 0x41140200:   //组合特征字
        {
            return buf_to_data_class8_combine_word(pOadInfo,  convert);
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
* @name：      class8_invoke_set_volt_rate
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_volt_rate(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x40300200:
        {
            OOP_VOLTPARAM_T data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.logicId = 1;//交采数据
            normal.classtag = CLASS_DATA_INIT;
            ACUDP_FMT_TRACE("class8_invoke_set_volt_rate : data.uupVolt = %d\n", data.uupVolt);
            ACUDP_FMT_TRACE("class8_invoke_set_volt_rate : data.lldVolt = %d\n", data.lldVolt);
            ACUDP_FMT_TRACE("class8_invoke_set_volt_rate : data.upVolt = %d\n", data.upVolt);
            ACUDP_FMT_TRACE("class8_invoke_set_volt_rate : data.ldVolt = %d\n", data.ldVolt);

            ret = db_write_nomal(gDbUdpClient, &normal, (uint8*)&data, sizeof(data));
            if(ret != ERR_OK)
            {
                ACUDP_FMT_TRACE("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
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
* @name：      class8_invoke_set_security_flag
* @brief：     class8的GET服务
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
OOP_DAR_E class8_invoke_set_time(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x40000200:
        {
            uint32 offset = 0;
            OOP_DATETIME_S_T data = {0};
            DateTimeHex_t dateHex ;
            memset(&dateHex,0,sizeof(DateTimeHex_t));
            
            time_t timep;
            time(&timep);
            OOP_DATETIME_S_T curData = {0};
            DT_Time2DateTimeShort(timep, &curData);
            
            memcpy(&data, convert.dstBuf, dOffset);
            ACUDP_FMT_TRACE("class8_invoke_set_time : curData.year = %hu\n", curData.year);
            ACUDP_FMT_TRACE("class8_invoke_set_time : curData.month = %d\n", curData.month);
            ACUDP_FMT_TRACE("class8_invoke_set_time : curData.day = %d\n", curData.day);
            ACUDP_FMT_TRACE("class8_invoke_set_time : curData.hour = %d\n", curData.hour);
            ACUDP_FMT_TRACE("class8_invoke_set_time : curData.minute = %d\n", curData.minute);
            ACUDP_FMT_TRACE("class8_invoke_set_time : curData.second = %d\n", curData.second);
            datetime_s_data_to_buf(data, &offset, (uint8 *)&dateHex);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.year = 0x%02x\n", dateHex.year_h);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.year = 0x%02x\n", dateHex.year_l);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.month = %d\n", dateHex.month);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.day = %d\n", dateHex.day);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.hour = %d\n", dateHex.hour);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.minute = %d\n", dateHex.min);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.second = %d\n", dateHex.sec);

            if ((dateHex.month > 12) 
                ||(dateHex.day > 31)
                ||(dateHex.month == 2 && dateHex.day > 29)
                ||(dateHex.hour > 23)
                ||(dateHex.min > 59)
                ||(dateHex.sec > 59))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            sta_time_update_fset(TRUE);

            //更新数据
            memcpy(&TermSharedData.DateTimeSet, &data, sizeof(OOP_DATETIME_S_T));
            TermSharedData.DateSetFlag = TRUE;
            TermSharedData.TimeSetFlag = TRUE;
            g_nAcSetDateFlag = 1;
            g_nAcSetTimeFlag = 1;
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
* @name：      class8_invoke_set_security_flag
* @brief：     class8的GET服务
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
OOP_DAR_E class8_invoke_set_change_time(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x40080200:			   //备用套时区表切换时间
    case 0x40090200:			   //备用套日时段切换时间
        {
            uint32 offset = 0;
            OOP_DATETIME_S_T data = {0};
            DateTimeHex_t dateHex ;
            memset(&dateHex,0,sizeof(DateTimeHex_t));
            
            time_t timep;
            time(&timep);
            OOP_DATETIME_S_T curData = {0};
            DT_Time2DateTimeShort(timep, &curData);
            
            memcpy(&data, convert.dstBuf, dOffset);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : curData.year = %hu\n", curData.year);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : curData.month = %d\n", curData.month);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : curData.day = %d\n", curData.day);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : curData.hour = %d\n", curData.hour);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : curData.minute = %d\n", curData.minute);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : curData.second = %d\n", curData.second);
            datetime_s_data_to_buf(data, &offset, (uint8 *)&dateHex);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.year = 0x%02x\n", dateHex.year_h);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.year = 0x%02x\n", dateHex.year_l);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.month = %d\n", dateHex.month);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.day = %d\n", dateHex.day);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.hour = %d\n", dateHex.hour);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.minute = %d\n", dateHex.min);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.second = %d\n", dateHex.sec);

            if(0xff == dateHex.sec)
            {
                dateHex.sec = 0;
            }
            
            if ((dateHex.month > 12) 
                ||(dateHex.day > 31)
                ||(dateHex.month == 2 && dateHex.day > 29)
                ||(dateHex.hour > 23)
                ||(dateHex.min > 59)
                ||(dateHex.sec > 59))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            //更新数据
            if(0x40080200 == pOadInfo->pTab->oad.value)
            {
                memcpy(&TermSharedData.AddiZoneChangeTimeSet, &data, sizeof(OOP_DATETIME_S_T));
                TermSharedData.AddiZoneChangeTimeSetFlag = TRUE;
                ACUDP_FMT_TRACE("case 0x40080200://备用套时区表切换时间 AddiZoneChangeTimeSetFlag=%d\n", 
                    TermSharedData.AddiZoneChangeTimeSetFlag);
            }
            else if(0x40090200 == pOadInfo->pTab->oad.value)
            {
                memcpy(&TermSharedData.AddiPeriChangeTimeSet, &data, sizeof(OOP_DATETIME_S_T));
                TermSharedData.AddiPeriChangeTimeSetFlag = TRUE;
                ACUDP_FMT_TRACE("case 0x40090200://备用套日时段表切换时间 AddiPeriChangeTimeSetFlag=%d\n", 
                    TermSharedData.AddiPeriChangeTimeSetFlag);
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
* @name：      class8_invoke_set_group_addr
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_time_zone(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x40140200:
    case 0x40150200:
        {
            uint32 i = 0;
            OOP_TIMEZONE_T data;
            memcpy(&data, convert.dstBuf, dOffset);

            ACUDP_FMT_TRACE("class8_invoke_set_time_zone : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                    ACUDP_FMT_TRACE("class8_invoke_set_time_zone : data.period[%d].mon:%d\n", 
                        i,  data.period[i].mon);
                    ACUDP_FMT_TRACE("class8_invoke_set_time_zone : data.period[%d].day:%d\n", 
                        i, data.period[i].day);
                    ACUDP_FMT_TRACE("class8_invoke_set_time_zone : data.period[%d].meterNum:%d\n", 
                        i, data.period[i].meterNum);
            }

            //更新数据
            if(0x40140200 == pOadInfo->pTab->oad.value)
            {
                memcpy(&TermSharedData.CurrZoneTableSet, &data, sizeof(OOP_TIMEZONE_T));
                TermSharedData.CurrZoneNumSetFlag = TRUE;
                TermSharedData.CurrZoneTableSetFlag = TRUE;
            }
            else if(0x40150200 == pOadInfo->pTab->oad.value)
            {
                memcpy(&TermSharedData.AddiZoneTableSet, &data, sizeof(OOP_TIMEZONE_T));
                TermSharedData.AddiZoneNumSetFlag = TRUE;
                TermSharedData.AddiZoneTableSetFlag = TRUE;
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
* @name：      class8_invoke_set_day_period_all
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_day_period_all(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x40160200:
    case 0x40170200:
        {
            uint32 i = 0;
            uint32 j = 0;
            OOP_DAYPERIOD_T data;
            memcpy(&data, convert.dstBuf, dOffset);

            ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                for (j = 0; j < data.dPeriod[i].nNum; j++)
                {
                    ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.dPeriod[%d].period[%d].hour:%d\n", 
                        i, j, data.dPeriod[i].period[j].hour);
                    ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.dPeriod[%d].period[%d].min:%d\n", 
                        i, j, data.dPeriod[i].period[j].min);
                    ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.dPeriod[%d].period[%d].rateNO:%d\n", 
                        i, j, data.dPeriod[i].period[j].rateNO);
                }
            }

            //首先查询当前套是第几套
            time_t nStartTime = time(NULL);
            g_nReadCurrDayPeriLabelFlag = 1;
            g_nRefreshCurrDayPeriLabelFlag = 0;
            while(abs(time(NULL) - nStartTime) < 3)//超时时间
            {
                if( (0 == g_nReadCurrDayPeriLabelFlag)
                    && (1 == g_nRefreshCurrDayPeriLabelFlag) )
                {
                    ACUDP_FMT_TRACE("class8_invoke_set_day_period() g_nCurrDayPeriLabel=%d \n", TermSharedData.g_nCurrDayPeriLabel);
                    break;
                }
                usleep(50000);
            }

            //更新数据(自适应处理: 设置日时段表时, 自动设置日时段数)
            if(data.nNum > 0)
            {
                TermSharedData.ZonePeriNumSet.dPeriodMet = data.nNum;
                TermSharedData.DayPeriTableNumSetFlag = TRUE;
                TermSharedData.ZonePeriNumSet.dPeriod = data.dPeriod[0].nNum;
                TermSharedData.DayPeriNumSetFlag = TRUE;

                ACUDP_FMT_TRACE("class8_invoke_set_day_period() ZoneTableNum=%d, DayPeriTableNum=%d, DayPeriNum=%d, CostRateNum=%d, HolidayNum=%d...\n", 
                    TermSharedData.ZonePeriNumSet.yPeriod, TermSharedData.ZonePeriNumSet.dPeriodMet, TermSharedData.ZonePeriNumSet.dPeriod, 
                    TermSharedData.ZonePeriNumSet.rate, TermSharedData.ZonePeriNumSet.pubHoliday);
                
                ACUDP_FMT_TRACE("class8_invoke_set_day_period() ZoneTableNumSetFlag=%d, DayPeriTableNumSetFlag=%d, DayPeriNumSetFlag=%d, CostRateNumSetFlag=%d, HolidayNumSetFlag=%d...\n", 
                    TermSharedData.ZoneTableNumSetFlag, TermSharedData.DayPeriTableNumSetFlag, TermSharedData.DayPeriNumSetFlag, 
                    TermSharedData.CostRateNumSetFlag, TermSharedData.HolidayNumSetFlag);
            }

            //更新数据
            if(0 == TermSharedData.g_nCurrDayPeriLabel)//当前套是第一套
            {
                if(0x40160200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.CurrDayPeriTableSet, &data, sizeof(OOP_DAYPERIOD_T));
                    
                    TermSharedData.CurrDayTableNumSetFlag = TRUE;
                    TermSharedData.CurrDayPeriNumSetFlag = TRUE;
                    memset(TermSharedData.CurrDayPeriNumSetFlagArr, 0x01, TermSharedData.CurrDayPeriTableSet.nNum);
                    
                    TermSharedData.CurrDayPeriTableSetFlag = TRUE;
                    memset(TermSharedData.CurrDayPeriSetFlagArr, 0x01, TermSharedData.CurrDayPeriTableSet.nNum);

                    ACUDP_FMT_TRACE("交采当前运行第1套, 设置当前套即设置第1套 \n");
                }
                else if(0x40170200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.AddiDayPeriTableSet, &data, sizeof(OOP_DAYPERIOD_T));
                    
                    TermSharedData.AddiDayTalbeNumSetFlag = TRUE;
                    TermSharedData.AddiDayPeriNumSetFlag = TRUE;
                    memset(TermSharedData.AddiDayPeriNumSetFlagArr, 0x01, TermSharedData.AddiDayPeriTableSet.nNum);
                    
                    TermSharedData.AddiDayPeriTableSetFlag = TRUE;
                    memset(TermSharedData.AddiDayPeriSetFlagArr, 0x01, TermSharedData.AddiDayPeriTableSet.nNum);

                    ACUDP_FMT_TRACE("交采当前运行第1套, 设置备用套即设置第2套 \n");
                }
            }
            else if(1 == TermSharedData.g_nCurrDayPeriLabel)//当前套是第二套
            {
                if(0x40170200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.CurrDayPeriTableSet, &data, sizeof(OOP_DAYPERIOD_T));
                    
                    TermSharedData.CurrDayTableNumSetFlag = TRUE;
                    TermSharedData.CurrDayPeriNumSetFlag = TRUE;
                    memset(TermSharedData.CurrDayPeriNumSetFlagArr, 0x01, TermSharedData.CurrDayPeriTableSet.nNum);
                    
                    TermSharedData.CurrDayPeriTableSetFlag = TRUE;
                    memset(TermSharedData.CurrDayPeriSetFlagArr, 0x01, TermSharedData.CurrDayPeriTableSet.nNum);

                    ACUDP_FMT_TRACE("交采当前运行第2套, 设置备用套即设置第1套 \n");
                }
                else if(0x40160200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.AddiDayPeriTableSet, &data, sizeof(OOP_DAYPERIOD_T));
                    
                    TermSharedData.AddiDayTalbeNumSetFlag = TRUE;
                    TermSharedData.AddiDayPeriNumSetFlag = TRUE;
                    memset(TermSharedData.AddiDayPeriNumSetFlagArr, 0x01, TermSharedData.AddiDayPeriTableSet.nNum);
                    
                    TermSharedData.AddiDayPeriTableSetFlag = TRUE;
                    memset(TermSharedData.AddiDayPeriSetFlagArr, 0x01, TermSharedData.AddiDayPeriTableSet.nNum);

                    ACUDP_FMT_TRACE("交采当前运行第2套, 设置当前套即设置第2套 \n");
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
* @name：      wait_day_period_update
* @brief：     等待数据更新完成
* @param[in] ：pOdaInfo        OAD信息
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E wait_day_period_update(uint8 isCur)
{
    int32 i = 0;

    if (isCur == 0)
    {
        while (FALSE != TermSharedData.CurrDayPeriTableSetFlag || 0 != g_nAcReadCurrDayPeriFlag)
        {
            if(i++ > 500)
            {
                PRTL_FMT_DEBUG("wait_day_period_update err\n");
                return DATA_READ_WRITE_DENIED;
            }
            usleep(10000);
        }
    }
    else if (isCur == 1)
    {
        while (FALSE != TermSharedData.AddiDayPeriTableSetFlag || 0 != g_nAcReadAddiDayPeriFlag)
        {
            if(i++ > 500)
            {
                PRTL_FMT_DEBUG("wait_day_period_update err\n");
                return DATA_READ_WRITE_DENIED;
            }
            usleep(10000);
        }
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      class8_invoke_set_day_period_one
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_day_period_one(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x40160200:
    case 0x40170200:
        {
            uint32 j = 0;
            uint32 index = 0;
            DAYPERIOD data = {0};

            index = pOadInfo->nIndex - 1;
            memcpy(&data, convert.dstBuf, dOffset);

            ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.nNum = %d\n", data.nNum);

            for (j = 0; j < data.nNum; j++)
            {
                ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.period[%d].hour:%d\n", 
                    j, data.period[j].hour);
                ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.period[%d].min:%d\n", 
                    j, data.period[j].min);
                ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.period[%d].rateNO:%d\n", 
                    j, data.period[j].rateNO);
            }

            //首先查询当前套是第几套
            time_t nStartTime = time(NULL);
            g_nReadCurrDayPeriLabelFlag = 1;
            g_nRefreshCurrDayPeriLabelFlag = 0;
            while(abs(time(NULL) - nStartTime) < 3)//超时时间
            {
                if( (0 == g_nReadCurrDayPeriLabelFlag)
                    && (1 == g_nRefreshCurrDayPeriLabelFlag) )
                {
                    ACUDP_FMT_TRACE("class8_invoke_set_day_period() g_nCurrDayPeriLabel=%d \n", TermSharedData.g_nCurrDayPeriLabel);
                    break;
                }
                usleep(50000);
            }

            //更新数据(自适应处理: 设置日时段表时, 自动设置日时段数)
            if (data.nNum > 0)
            {
                TermSharedData.ZonePeriNumSet.dPeriodMet = OOP_MAX_DAYPERIODS;
                TermSharedData.DayPeriTableNumSetFlag = TRUE;

                TermSharedData.ZonePeriNumSet.dPeriod = data.nNum;
                TermSharedData.DayPeriNumSetFlag = TRUE;

                ACUDP_FMT_TRACE("class8_invoke_set_day_period() ZoneTableNum=%d, DayPeriTableNum=%d, DayPeriNum=%d, CostRateNum=%d, HolidayNum=%d...\n", 
                    TermSharedData.ZonePeriNumSet.yPeriod, TermSharedData.ZonePeriNumSet.dPeriodMet, TermSharedData.ZonePeriNumSet.dPeriod, 
                    TermSharedData.ZonePeriNumSet.rate, TermSharedData.ZonePeriNumSet.pubHoliday);
                
                ACUDP_FMT_TRACE("class8_invoke_set_day_period() ZoneTableNumSetFlag=%d, DayPeriTableNumSetFlag=%d, DayPeriNumSetFlag=%d, CostRateNumSetFlag=%d, HolidayNumSetFlag=%d...\n", 
                    TermSharedData.ZoneTableNumSetFlag, TermSharedData.DayPeriTableNumSetFlag, TermSharedData.DayPeriNumSetFlag, 
                    TermSharedData.CostRateNumSetFlag, TermSharedData.HolidayNumSetFlag);
            }

            //更新数据
            if(0 == TermSharedData.g_nCurrDayPeriLabel)//当前套是第一套
            {
                if(0x40160200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.CurrDayPeriTableSet, &TermSharedData.CurrDayPeriTableRD, sizeof(OOP_DAYPERIOD_T));
                    memcpy(&TermSharedData.CurrDayPeriTableSet.dPeriod[index], &data, sizeof(DAYPERIOD));
                    
                    TermSharedData.CurrDayPeriTableSetFlag = TRUE;
                    TermSharedData.CurrDayPeriSetFlagArr[index] = 0x01;

                    ACUDP_FMT_TRACE("交采当前运行第1套, 设置当前套即设置第1套 \n");

                    //等待数据更新完成
                    if (wait_day_period_update(0) != DATA_SUCCESS)
                    {
                        ACUDP_FMT_DEBUG("交采当前运行第1套, 设置当前套即设置第1套 失败\n");
                        return DATA_READ_WRITE_DENIED;
                    }
                }
                else if(0x40170200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.AddiDayPeriTableSet, &TermSharedData.AddiDayPeriTableRD, sizeof(OOP_DAYPERIOD_T));
                    memcpy(&TermSharedData.AddiDayPeriTableSet.dPeriod[index], &data, sizeof(DAYPERIOD));
                    
                    TermSharedData.AddiDayPeriTableSetFlag = TRUE;
                    TermSharedData.AddiDayPeriSetFlagArr[index] = 0x01;

                    ACUDP_FMT_TRACE("交采当前运行第1套, 设置备用套即设置第2套 \n");

                    //等待数据更新完成
                    if (wait_day_period_update(1) != DATA_SUCCESS)
                    {
                        ACUDP_FMT_DEBUG("交采当前运行第1套, 设置备用套即设置第2套 失败\n");
                        return DATA_READ_WRITE_DENIED;
                    }
                }
            }
            else if(1 == TermSharedData.g_nCurrDayPeriLabel)//当前套是第二套
            {
                if(0x40170200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.CurrDayPeriTableSet, &TermSharedData.CurrDayPeriTableRD, sizeof(OOP_DAYPERIOD_T));
                    memcpy(&TermSharedData.CurrDayPeriTableSet.dPeriod[index], &data, sizeof(DAYPERIOD));
                    
                    TermSharedData.CurrDayPeriTableSetFlag = TRUE;
                    TermSharedData.CurrDayPeriSetFlagArr[index] = 0x01;

                    ACUDP_FMT_TRACE("交采当前运行第2套, 设置备用套即设置第1套 \n");

                    //等待数据更新完成
                    if (wait_day_period_update(0) != DATA_SUCCESS)
                    {
                        ACUDP_FMT_DEBUG("交采当前运行第1套, 设置备用套即设置第2套 失败\n");
                        return DATA_READ_WRITE_DENIED;
                    }
                }
                else if(0x40160200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.AddiDayPeriTableSet, &TermSharedData.AddiDayPeriTableRD, sizeof(OOP_DAYPERIOD_T));
                    memcpy(&TermSharedData.AddiDayPeriTableSet.dPeriod[index], &data, sizeof(DAYPERIOD));
                    
                    TermSharedData.AddiDayPeriTableSetFlag = TRUE;
                    TermSharedData.AddiDayPeriSetFlagArr[index] = 0x01;

                    ACUDP_FMT_TRACE("交采当前运行第2套, 设置当前套即设置第2套 \n");

                    //等待数据更新完成
                    if (wait_day_period_update(1) != DATA_SUCCESS)
                    {
                        ACUDP_FMT_DEBUG("交采当前运行第2套, 设置当前套即设置第2套 失败\n");
                        return DATA_READ_WRITE_DENIED;
                    }
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
* @name：      class8_invoke_set_group_addr
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_day_period(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    if (pOadInfo->nIndex == 0)
    {
        return class8_invoke_set_day_period_all(pOadInfo, inData, InDataLen, outData, outDataMax);
    }
    else
    {
        return class8_invoke_set_day_period_one(pOadInfo, inData, InDataLen, outData, outDataMax);
    }
}

/**
*********************************************************************
* @name：      class8_invoke_set_demand_period
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_demand_period(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x41000200:
        {
            uint8 data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            
            //更新数据
            memcpy(&TermSharedData.MaxDemandPeriodSet, &data, sizeof(uint8));
            TermSharedData.MaxDemandPeriodSetFlag = TRUE;
            ACUDP_FMT_TRACE("class8_invoke_set_demand_period() MaxDemandPeriodSet=%d, MaxDemandPeriodSetFlag=%d...\n", 
                TermSharedData.MaxDemandPeriodSet, TermSharedData.MaxDemandPeriodSetFlag);
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
* @name：      class8_invoke_set_demand_period
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_slip_time(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x41010200:
        {
            uint8 data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            
            //更新数据
            memcpy(&TermSharedData.SlipDiffTimeSet, &data, sizeof(uint8));
            TermSharedData.SlipDiffTimeSetFlag = TRUE;
            ACUDP_FMT_TRACE("class8_invoke_set_slip_time() SlipDiffTimeSet=%d, SlipDiffTimeSetFlag=%d...\n", 
                TermSharedData.SlipDiffTimeSet, TermSharedData.SlipDiffTimeSetFlag);
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
* @name：      class8_invoke_set_time_region
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_time_region(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x400C0200:
        {
            OOP_PERIOD_T data;
            memcpy(&data, convert.dstBuf, dOffset);

            //更新数据
            memcpy(&TermSharedData.ZonePeriNumSet, &data, sizeof(OOP_PERIOD_T));
            TermSharedData.ZoneTableNumSetFlag = TRUE;
            TermSharedData.DayPeriTableNumSetFlag = TRUE;
            TermSharedData.DayPeriNumSetFlag = TRUE;
            TermSharedData.CostRateNumSetFlag = TRUE;
            TermSharedData.HolidayNumSetFlag = TRUE;

            ACUDP_FMT_TRACE("class8_invoke_set_time_region() ZoneTableNum=%d, DayPeriTableNum=%d, DayPeriNum=%d, CostRateNum=%d, HolidayNum=%d...\n", 
                TermSharedData.ZonePeriNumSet.yPeriod, TermSharedData.ZonePeriNumSet.dPeriodMet, TermSharedData.ZonePeriNumSet.dPeriod, 
                TermSharedData.ZonePeriNumSet.rate, TermSharedData.ZonePeriNumSet.pubHoliday);
            
            ACUDP_FMT_TRACE("class8_invoke_set_time_region() ZoneTableNumSetFlag=%d, DayPeriTableNumSetFlag=%d, DayPeriNumSetFlag=%d, CostRateNumSetFlag=%d, HolidayNumSetFlag=%d...\n", 
                TermSharedData.ZoneTableNumSetFlag, TermSharedData.DayPeriTableNumSetFlag, TermSharedData.DayPeriNumSetFlag, 
                TermSharedData.CostRateNumSetFlag, TermSharedData.HolidayNumSetFlag);
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
* @name：      class8_invoke_set_feature
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_feature(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x41120200:			   //有功组合方式特征字
        {
            uint8 data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            
            //更新数据
            memcpy(&TermSharedData.ActPowCombWordSet, &data, sizeof(uint8));
            TermSharedData.ActPowCombWordSetFlag = TRUE;
            ACUDP_FMT_TRACE("class8_invoke_set_feature() ActPowCombWordSet=%d, ActPowCombWordSetFlag=%d...\n", 
                TermSharedData.ActPowCombWordSet, TermSharedData.ActPowCombWordSetFlag);
        }
        break;
    case 0x41130200:			   //无功组合方式1特征字
        {
            uint8 data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            
            //更新数据
            memcpy(&TermSharedData.ReaPowComb1WordSet, &data, sizeof(uint8));
            TermSharedData.ReaPowComb1WordSetFlag = TRUE;
            ACUDP_FMT_TRACE("class8_invoke_set_feature() ReaPowComb1WordSet=%d, ReaPowComb1WordSetFlag=%d...\n", 
                TermSharedData.ReaPowComb1WordSet, TermSharedData.ReaPowComb1WordSetFlag);
        }
        break;
    case 0x41140200:			   //无功组合方式2特征字
        {
            uint8 data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            
            //更新数据
            memcpy(&TermSharedData.ReaPowComb2WordSet, &data, sizeof(uint8));
            TermSharedData.ReaPowComb2WordSetFlag = TRUE;
            ACUDP_FMT_TRACE("class8_invoke_set_feature() ReaPowComb2WordSet=%d, ReaPowComb2WordSetFlag=%d...\n", 
                TermSharedData.ReaPowComb2WordSet, TermSharedData.ReaPowComb2WordSetFlag);
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



