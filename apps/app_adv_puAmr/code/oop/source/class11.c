/*
*********************************************************************
* @file    class11.c
* @brief： class11处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "hal.h"
#include "devinfo.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "msg_id.h"
#include "area.h"
#include "msgSecApp.h"
#include "appcomm.h"
#include "statlib.h"
#include "AutoTask.h"
//extern OOP_DAR_E area_class11_invoke_act_oop_meter(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);

//
// class11
//
#define OOP_APP_NO_LEN       48
#define OOP_APP_LICENCE_LEN  36
extern uint16 app_get_seek_meter_count(void);
extern uint16 app_get_seek_across_count(void);
typedef uint8 (*CTRL_SHOW_HINT)(uint8, uint8, uint8, uint8*, uint16, uint16*);
extern void appctrl_alarm_data(uint8 ctrltype, uint8 group, uint8 times, uint8 Voindex, CTRL_SHOW_HINT pfunc);
extern void appctrl_db_chg_event(uint8 type, uint8 dbtype, uint32 value);
extern int sta_basic_1minen_clear(const STA_MTR_INFO_T * mtrInfo);
#define APP_UPDATE_PATH      "/data/app/download/"
/**
*********************************************************************
* @brief：     设置搜表参数
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
OOP_DAR_E buf_to_data_class11_set_searchmeter_param(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SEARCHPARAM_T data;
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
    if(srcBuf[sOffset++] != 4)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //是否启用每天周期搜表
    dar = basic_buf_to_data(E_BOOL, srcBuf, srcLen, &sOffset, &data.periodSearch, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //自动更新采集档案
    dar = basic_buf_to_data(E_BOOL, srcBuf, srcLen, &sOffset, &data.atuoUpdate, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //是否产生搜表事件
    dar = basic_buf_to_data(E_BOOL, srcBuf, srcLen, &sOffset, &data.touchEvent, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    //清空搜表结果选项
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.clearChoice, sizeof(uint8));
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
* @brief：     设置定时搜表参数
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
OOP_DAR_E buf_to_data_class11_set_time_searchmeter_param(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    uint32 i = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TIMINGSEARCH_T data;
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
        dar = basic_buf_to_data(E_TIME, srcBuf, srcLen, &sOffset, &data.period[i].start, sizeof(data.period[i].start));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        
        //搜表时长
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period[i].time, sizeof(uint16));
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
* @brief：     设置定时搜表参数
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
OOP_DAR_E buf_to_data_class11_set_acorss_searchmete(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    uint32 i = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_ACROSSAREA_RES_T data;
//    uint8 offlen = 0;      //数组数量长度
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));



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
        
        
           //表计地址
            dar = basic_buf_to_data(E_TSA, srcBuf, srcLen, &sOffset, &data.commAdd, sizeof(data.commAdd));
            if(dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            }
                   //表计地址
            dar = basic_buf_to_data(E_TSA, srcBuf, srcLen, &sOffset, &data.netAdd, sizeof(data.netAdd));
            if(dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            }
        //搜表时长
        dar = basic_buf_to_data(E_DATETIME_S, srcBuf, srcLen, &sOffset, &data.change, sizeof(data.change));
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
* @brief：     从698格式报文转数据中心的数据(class11)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class11_set(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60020800:   //搜表参数
        {
            return buf_to_data_class11_set_searchmeter_param(pOadInfo,  convert);
        }break;

        case 0x60020900:   //定时搜表参数
        {
            return buf_to_data_class11_set_time_searchmeter_param(pOadInfo,  convert);
        }break;
        case 0x60020500:   //定时搜表参数
        {
            return buf_to_data_class11_set_acorss_searchmete(pOadInfo,  convert);
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
* @brief：     启动搜表
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
OOP_DAR_E buf_to_data_class11_act_searchmeter_start(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint16 data = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //搜表时间
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data, sizeof(data));
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
* @brief：     从698格式报文转数据中心的数据(class11)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class11_act(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60027f00:   //启动搜表
#ifdef AREA_HUNAN
        case 0x60f17f00:   //启动拓扑
#endif
        {
            return buf_to_data_class11_act_searchmeter_start(pOadInfo,  convert);
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
* @name：      class11_invoke_set_searchmeter_param
* @brief：     class11的ACTION服务
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
OOP_DAR_E class11_invoke_set_searchmeter_param(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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

    convert.srcBuf = (void*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class11_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60020800:
        {
            OOP_SEARCHPARAM_T data;
            MEMZERO(&data, sizeof(data));

            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class11_invoke_set_searchmeter_param : data.periodSearch = %d\n", data.periodSearch);
            PRTL_FMT_LOGCC("class11_invoke_set_searchmeter_param : data.atuoUpdate = %d\n", data.atuoUpdate);
            PRTL_FMT_LOGCC("class11_invoke_set_searchmeter_param : data.touchEvent = %d\n", data.touchEvent);
            PRTL_FMT_LOGCC("class11_invoke_set_searchmeter_param : data.clearChoice = %d\n", data.clearChoice);

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
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
* @name：      class11_invoke_set_searchmeter_param
* @brief：     class11的ACTION服务
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
OOP_DAR_E class11_invoke_set_acorssmeter(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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

    convert.srcBuf = (void*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class11_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60020500:
        {
            OOP_ACROSSAREA_RES_T data;
            MEMZERO(&data, sizeof(data));

            memcpy(&data, convert.dstBuf, dOffset);
            normal.infoNum =1;
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = 3;

            PRTL_FMT_LOGCC("class11_invoke_set_searchmeter_param : data.periodSearch\n");


            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
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
* @name：      class11_invoke_set_time_searchmeter_param
* @brief：     class11的ACTION服务
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
OOP_DAR_E class11_invoke_set_time_searchmeter_param(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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

    convert.srcBuf = (void*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class11_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class11_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60020900:
        {
            uint32 i = 0;
            OOP_TIMINGSEARCH_T data;
            MEMZERO(&data, sizeof(data));

            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class11_invoke_set_time_searchmeter_param : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class11_invoke_set_time_searchmeter_param : data.period[%d].start.hour = %d\n", i, data.period[i].start.hour);
                PRTL_FMT_LOGCC("class11_invoke_set_time_searchmeter_param : data.period[%d].start.minute = %d\n", i, data.period[i].start.minute);
                PRTL_FMT_LOGCC("class11_invoke_set_time_searchmeter_param : data.period[%d].start.second = %d\n", i, data.period[i].start.second);
                PRTL_FMT_LOGCC("class11_invoke_set_time_searchmeter_param : data.period[%d].time = %d\n", i, data.period[i].time);
            }

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
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
* @name：      class11_invoke_set_searchmeter_res
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo       OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
*********************************************************************
*/
OOP_DAR_E class11_invoke_set_searchmeter_res(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60020500:
    return class11_invoke_set_acorssmeter(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
    break;  
    case 0x60020800:
        return class11_invoke_set_searchmeter_param(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;   

    case 0x60020900:
        return class11_invoke_set_time_searchmeter_param(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  

    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class11)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class11_searchmeter_all_num(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint16 data = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);
            
    //数量
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),data(%d)\n", dar, data);
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
* @brief：     从数据中心的数据转698格式报文(class11)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class11_searchmeter_acrossarea_num(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint16 data = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);
            
    //数量
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),data(%d)\n", dar, data);
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
* @brief：     从数据中心的数据转698格式报文(class11)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class11_searchmeter_param(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SEARCHPARAM_T data;
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
    dstBuf[dOffset++] = 4;
            
    //是否启用每天周期搜表
    dar = basic_data_to_buf(E_BOOL, dstBuf, dstLen, &dOffset, &data.periodSearch, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),data.periodSearch(%d)\n", dar, data.periodSearch);
        return DATA_TYPE_UNMATCHED;
    }     

    //自动更新采集档案
    dar = basic_data_to_buf(E_BOOL, dstBuf, dstLen, &dOffset, &data.atuoUpdate, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),data.atuoUpdate(%d)\n", dar, data.atuoUpdate);
        return DATA_TYPE_UNMATCHED;
    }  

    //是否产生搜表事件
    dar = basic_data_to_buf(E_BOOL, dstBuf, dstLen, &dOffset, &data.touchEvent, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),data.touchEvent(%d)\n", dar, data.touchEvent);
        return DATA_TYPE_UNMATCHED;
    }  

    //清空搜表结果选项
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.clearChoice, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),data.clearChoice(%d)\n", dar, data.clearChoice);
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
* @brief：     从数据中心的数据转698格式报文(class11)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class11_time_searchmeter_param(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TIMINGSEARCH_T data;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));
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
                
        //开始时间
        dar = basic_data_to_buf(E_TIME, dstBuf, dstLen, &dOffset, &data.period[i].start, sizeof(data.period[i].start));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.period.start(%d)\n", i, dar, data.period[i].start);
            return DATA_TYPE_UNMATCHED;
        }

        //搜表时长
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.period[i].time, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.period.time(%d)\n", i, dar, data.period[i].time);
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
* @brief：     从数据中心的数据转698格式报文(class11)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class11_searchmeter_status(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //搜表状态
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.period.start(%d)\n", i, dar, data);
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
* @brief：     从数据中心的数据转698格式报文(class11)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class11(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60020400:
    case 0x60020600:
        return data_to_buf_class11_searchmeter_all_num(pOadInfo, convert);
        break;

    case 0x60020700:
        return data_to_buf_class11_searchmeter_acrossarea_num(pOadInfo, convert);
        break;

    case 0x60020800:
        return data_to_buf_class11_searchmeter_param(pOadInfo, convert);
        break;
        
    case 0x60020900:
        return data_to_buf_class11_time_searchmeter_param(pOadInfo, convert);
        break;
    
    case 0x60020A00:
        return data_to_buf_class11_searchmeter_status(pOadInfo, convert);
        break;

    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

OOP_DAR_E buf_to_data_class11(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：       frozen_objattr_set
* @brief：      设置关联对象属性
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E report_plan_add(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_PLAN_REPORT_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint8 offlen = 0;      //数组数量长度
    uint8 i = 0;
    uint32 num = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    if(inData[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(inData, InDataLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //数组成员个数
    num = get_len_offset((void*)&inData[sOffset], &offlen);
    if(num > OOP_MAX_OADS)
    {
        return DATA_TYPE_UNMATCHED;
    }
    
    sOffset += offlen;
    
    PRTL_FMT_TRACE("report_plan_add array num(%d)\n", num);

    //写入数据
    normal.oad.value = 0x601C0200;
    
    for(i = 0; i < num; i++)
    {
        //每次目的偏移清零，循环一次解一个data结构
        dOffset = 0;
        
        //从报文中提取数据
        dar = buf_to_data_report_plan(pOadInfo, &convert);
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_report_plan failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
            return dar;
        }

        PRTL_FMT_LOGCC("*******************上报方案数据打印*********************\n");
        PRTL_FMT_LOGCC("num = %d, i = %d\n", num, i);
        PRTL_FMT_LOGCC("data.planID = %d\n", data.planID);
        PRTL_FMT_LOGCC("data.timeout.value = %d\n", data.timeout.value);
        PRTL_FMT_LOGCC("data.timeout.unit = %d\n", data.timeout.unit);
        PRTL_FMT_LOGCC("data.retry = %d\n", data.retry);
        PRTL_FMT_LOGCC("*******************************************************\n");
        if(data.data.choice == 1 && data.data.record.tRCSD.nNum > OOP_MAX_CSDS)
        {
            return DATA_TYPE_UNMATCHED;
        }
    
        normal.infoNum = data.planID;
        normal.classtag = CLASS_DATA_INIT;
        ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_PLAN_REPORT_T));
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), sizeof(OOP_PLAN_REPORT_T) = \n",
                ret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(OOP_PLAN_REPORT_T));
            return DATA_OTHER_REASON;
        }
    }
    
    return dar;
}

/**
*********************************************************************
* @name：       report_plan_del
* @brief：      删除采集方案
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E report_plan_del(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int    i = 0;
    int    ret = ERR_NORMAL;
    uint32 offset = 4;
    uint32 num = 0;
    uint16 task_id = 0;
    CLEAR_DATA_T clearData;

    MEMZERO(&clearData, sizeof(clearData));
    
    if (inData[offset++]!=DT_ARRAY)
    {
        return ERR_OPTION;
    }

    num = inData[offset++];
    if (num == 0x81)
    {
        num = inData[offset++];
    }
    else if(num == 0x82)
    {
        num = inData[offset++];
        num <<= 8;
        num += inData[offset++];
    }
    else if(num > 0x82 || num == 0x80)
    {
        return ERR_OPTION;
    }

    for (i = 0; i < num; i++)
    {
        offset++;
        task_id = inData[offset++];

        PRTL_FMT_DEBUG("删除上报方案[%d]\n", task_id);
        
        clearData.recordOAD.logicId = 0x00;
        clearData.recordOAD.optional = 0;
        clearData.recordOAD.road.oadMain.value = 0x00;
        clearData.recordOAD.road.oadCols.nNum = 1;
        clearData.recordOAD.road.oadCols.oad[0].value = 0x601C0200;
        clearData.bClassTag = FALSE;
		clearData.recordOAD.classtag= CLASS_DATA_INIT;
        
        clearData.recordOAD.infoNum = task_id;
        
        ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("删除上报方案[%d] err = %d\n", task_id, ret);
        }
        else
        {
            PRTL_FMT_LOGCC("删除上报方案[%d] ok = %d\n", task_id, ret);
        }
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      report_plan_clear
* @brief：     清空采集方案
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E report_plan_clear(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_NORMAL;
    CLEAR_DATA_T clearData;

    MEMZERO(&clearData, sizeof(clearData));
    PRTL_FMT_LOGCC("report_plan_clear 清空上报方案\n");

    /*清空档案操作*/
    clearData.recordOAD.logicId = 0x00;
    clearData.recordOAD.optional = 0;
    clearData.recordOAD.road.oadMain.value = 0x00;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = 0x601C0200;
	clearData.recordOAD.classtag = CLASS_DATA_INIT;
    clearData.bClassTag = FALSE;

    clearData.recordOAD.infoNum = 0xffff;
    ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("清空所有上报方案序 err = %d\n", ret);
    }
    else
    {
        PRTL_FMT_LOGCC("清空所有上报方案序 ok = %d\n", ret);
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      acq_rule_add
* @brief：     设置关联对象属性
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E acq_rule_add(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_ACQRULE_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint8 offlen = 0;      //数组数量长度
    uint8 i = 0;
    uint32 num = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    if(inData[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(inData, InDataLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //数组成员个数
    num = get_len_offset((void*)&inData[sOffset], &offlen);
    sOffset += offlen;
    
    PRTL_FMT_TRACE("acq_rule_add array num(%d)\n", num);

    //写入数据
    normal.oad.value = 0x601E0200;
    normal.classtag = CLASS_DATA_INIT;
    
    for(i = 0; i < num; i++)
    {
        //每次目的偏移清零，循环一次解一个data结构
        dOffset = 0;
        
        //从报文中提取数据
        dar = buf_to_data_acq_rule(pOadInfo, &convert);
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_acq_rule failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
            return dar;
        }

        normal.infoNum = i + 1;
        ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_ACQRULE_T));
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return DATA_OTHER_REASON;
        }
    }
    
    return dar;
}

/**
*********************************************************************
* @brief：      记录列选择
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E buf_to_data_record_col(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_RECORD_COL_T data;
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
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组成员个数
    data.num = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    for(i = 0; i < data.num; i++)
    {
        dar = basic_buf_to_data(E_CSD, srcBuf, srcLen, &sOffset, &data.csd[i], sizeof(OOP_CSD_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "[i] basic_data_to_buf failed. dar(%d), sOffset(%d)\n", i, dar, sOffset);
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
* @brief：      普通采集方案重置CSD的结构
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E buf_to_data_ntask_130(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOPM_NTASK_130_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 unitDoffset = 0;
    DATA_CONVERT_T unitConvert;

    MEMZERO(&data, sizeof(data));
    memcpy(&unitConvert, convert, sizeof(DATA_CONVERT_T));
    
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

    //方案编号
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.index, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //记录列选择
    *unitConvert.sOffset = sOffset;
    unitConvert.dstBuf = &data.col;
    unitConvert.dstLen = sizeof(OOP_RECORD_COL_T);
    unitConvert.dOffset = &unitDoffset;  
    
    dar = buf_to_data_record_col(pOadInfo, &unitConvert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "buf_to_data_record_col failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return dar;
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
* @name：       normal_colscheme_csd_reset
* @brief：      重置普通采集方案的CSD
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-03-26
*********************************************************************
*/
OOP_DAR_E normal_colscheme_csd_reset(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_PLAN_NORMAL_T data;
    OOPM_NTASK_130_T newData;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 len = 0;
    OOP_OAD_U oad = {0};

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //普通采集方案
    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 2;

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &newData;
    convert.dstLen = sizeof(newData);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_ntask_130(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_ntask_130 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    normal.oad.value = oad.value;
    normal.infoNum = newData.index;

    //写单元数据，先读取全量信息
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("normal colscheme read failed, can not reset csd. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ret;
    }

    //覆盖原CSD
    memcpy(&data.recdCol, &newData.col, sizeof(OOP_RECORD_COL_T));

    //写入数据
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @brief：      清除指定电能表的数据
* @param[in]： fd  数据中心句柄
* @param[in]： oldMeter   老电表档案数据
* @return：     =0, 成功 <0 失败
*********************************************************************
*/
int meter_data_clear(DB_CLIENT fd, uint16 infoNum)
{
    int ret = ERR_OK;
    CLEAR_DATA_T clear;
    STA_MTR_INFO_T mtrInfo;

    MEMZERO(&clear, sizeof(clear));
    MEMZERO(&mtrInfo, sizeof(mtrInfo));

    clear.bClassTag = TRUE;
    clear.recordOAD.infoNum = infoNum;
    clear.recordOAD.classtag = CLASS_DATA_COLLEC;
    
    ret = db_clear_oad_data(fd, &clear);

    PRTL_FMT_DEBUG("meter data clear. ret(%d), infoNum(%d), classtag(%d)\n", 
        ret, clear.recordOAD.infoNum, clear.recordOAD.classtag);     

    //清除分钟电量计算底数
    mtrInfo.type = MTR_485;
    mtrInfo.nIndex = infoNum;
    sta_basic_1minen_clear(&mtrInfo);

    return ret;
}

/**
*********************************************************************
* @brief：      有条件清除指定电能表的数据
* @param[in]： pApduInfo  apdu信息
* @param[in]： oldMeter   老电表档案数据
* @param[in]： curMeter   新电表档案数据
* @return：     =0, 成功 <0 失败
*********************************************************************
*/
int meter_data_if_clear(APDU_INFO_T *pApduInfo, OOP_METER_T *oldMeter, OOP_METER_T *curMeter)
{
    int ret = ERR_OK;
    CLEAR_DATA_T clear;

    MEMZERO(&clear, sizeof(clear));

    if((curMeter->nIndex != oldMeter->nIndex)||(!oop_tsa_equal(&curMeter->basic.tsa, &oldMeter->basic.tsa))||
        (curMeter->basic.port.value != oldMeter->basic.port.value)||
        (curMeter->basic.protocol != oldMeter->basic.protocol)||(curMeter->basic.userType != oldMeter->basic.userType))
    {
        PRTL_FMT_TRACE("meter changed, and collect data clear.\n");

        PRTL_BUF_TRACE(&oldMeter->basic.tsa, sizeof(OOP_TSA_T), "old->[index:(%d), port:(0x%08x), protocol:(%d), userType:(%d), tsa:]\n",
            oldMeter->nIndex, oldMeter->basic.port.value, oldMeter->basic.protocol,  oldMeter->basic.userType);

        PRTL_BUF_TRACE(&curMeter->basic.tsa, sizeof(OOP_TSA_T), "cur->[index:(%d), port:(0x%08x), protocol:(%d), userType:(%d), tsa:]\n",
            curMeter->nIndex, curMeter->basic.port.value, curMeter->basic.protocol,  curMeter->basic.userType);
        
        return meter_data_clear(pApduInfo->hUdp, oldMeter->nIndex);
    }

    return ret;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读该测量点
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue_oop_meter(BP_INFO_T* bpInfo, uint16 i)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续往下

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->index != 0)
        {
            if(i != bpInfo->index)
            {
                nRet = 1;//抄读下一个电表档案
            }
            else
            {
                /*查到断点*/
                bpInfo->index = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue_oi_rights(BP_INFO_T* bpInfo, uint16 i)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续往下

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->index != 0)
        {
            if(i != bpInfo->index)
            {
                nRet = 1;//抄读下一个电表档案
            }
            else
            {
                /*查到断点*/
                bpInfo->index = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读该测量点
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue_task(BP_INFO_T* bpInfo, uint16 i)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续往下

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->index != 0)
        {
            if(i != bpInfo->index)
            {
                nRet = 1;//抄读下一个电表档案
            }
            else
            {
                /*查到断点*/
                bpInfo->index = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读该测量点
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue(BP_INFO_T* bpInfo, uint16 i)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续往下

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->index != 0)
        {
            if(i != bpInfo->index)
            {
                nRet = 1;//抄读下一个电表档案
            }
            else
            {
                /*查到断点*/
                bpInfo->index = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读该测量点
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue_event_scheme(BP_INFO_T* bpInfo, uint16 i)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续往下

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->index != 0)
        {
            if(i != bpInfo->index)
            {
                nRet = 1;//抄读下一个电表档案
            }
            else
            {
                /*查到断点*/
                bpInfo->index = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读该测量点
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue_report_scheme(BP_INFO_T* bpInfo, uint16 i)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续往下

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->index != 0)
        {
            if(i != bpInfo->index)
            {
                nRet = 1;//抄读下一个电表档案
            }
            else
            {
                /*查到断点*/
                bpInfo->index = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读该测量点
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue_transparent_scheme(BP_INFO_T* bpInfo, uint16 i)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续往下

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->index != 0)
        {
            if(i != bpInfo->index)
            {
                nRet = 1;//抄读下一个电表档案
            }
            else
            {
                /*查到断点*/
                bpInfo->index = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}


/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读该测量点
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个电表信息
*********************************************************************
*/
uint8 slicing_is_comtinue_normal_scheme(BP_INFO_T* bpInfo, uint16 i)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续往下

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->index != 0)
        {
            if(i != bpInfo->index)
            {
                nRet = 1;//抄读下一个电表档案
            }
            else
            {
                /*查到断点*/
                bpInfo->index = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个
*********************************************************************
*/
uint8 slicing_is_comtinue_searchmeter_res_all(BP_INFO_T* bpInfo, uint16 i)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续往下

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->index != 0)
        {
            if(i != bpInfo->index)
            {
                nRet = 1;//抄读下一个
            }
            else
            {
                /*查到断点*/
                bpInfo->index = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个
*********************************************************************
*/
uint8 slicing_is_comtinue_searchmeter_res_acrossarea(BP_INFO_T* bpInfo, uint16 i)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续往下

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->index != 0)
        {
            if(i != bpInfo->index)
            {
                nRet = 1;//抄读下一个
            }
            else
            {
                /*查到断点*/
                bpInfo->index = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个
*********************************************************************
*/
uint8 slicing_is_comtinue_acq_monitor(BP_INFO_T* bpInfo, uint16 i)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续往下

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->index != 0)
        {
            if(i != bpInfo->index)
            {
                nRet = 1;//抄读下一个
            }
            else
            {
                /*查到断点*/
                bpInfo->index = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     应用分帧情况下判断是否继续抄读
* @param[in]： bpInfo      断点信息
* @param[in]： metsn       测量点号
* @param[out]：
* @return：    0           继续抄读
               1           抄读下一个
*********************************************************************
*/
uint8 slicing_is_comtinue_acq_rule(BP_INFO_T* bpInfo, uint16 i)
{
    int nRet = 0;

    if(bpInfo == NULL)
        return 0;//继续往下

    if(bpInfo->appSlicingFlag != 0)
    {
        if(bpInfo->index != 0)
        {
            if(i != bpInfo->index)
            {
                nRet = 1;//抄读下一个
            }
            else
            {
                /*查到断点*/
                bpInfo->index = 0;
                nRet = 0;
            }
        }
    }

    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter_normal
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_oop_meter_normal(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
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

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOdaInfo->pTab->oad.value)
    {
        case 0x60000200:
        {
            DATA_CONVERT_T convert;
            OOP_METER_T oopMeter = {0};                                        //每个电表的档案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopMeterNum = 0;
            uint32 oopMeterNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));

            #if METER_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOdaInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取所有档案\n");

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            oopMeterNumOffset = offset;                                         //档案数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;
            for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                if (slicing_is_comtinue_oop_meter(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                #if METER_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(pApduInfo->hUdp, &nomalOad, sizeof(OOP_METER_T), (uint8*)&oopMeter, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_METER_T)))
                #else
                ret = app_get_meter_list(i, &oopMeter);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到档案 序号为%d\n", oopMeter.nIndex);

                //转换成报文数据
                convert.srcBuf = &oopMeter;
                convert.srcLen = sizeof(oopMeter);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;
                #if AREA_ZHEJIANG
                nRet = area_data_to_buf_oop_meter(pOdaInfo, &convert); 
                #else
                nRet = data_to_buf_oop_meter(pOdaInfo, &convert);  //入参
                #endif
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

            PRTL_BUF_LOGCC(outData->pResultData, offset, "整个档案报文:");
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
                result = DATA_SUCCESS;
                outData->pResultData[oopMeterNumOffset] = 0;   //重新记档案数量
                outData->resultDataLen = offset;
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
* @brief：     获取读记录档案参数
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 read_meter_param_get(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, READ_METER_T *pInReadMeter, RELY_INFO_T *pRelyInfo)
{
    int32 nRet = ERR_NORMAL;
    READ_METER_T   inReadMeter = {0};
    RELY_INFO_T    relyInfo = {0};

    if (((pOopRcsd->nNum != 1) || (pOopRcsd->cols[0].oad.value != 0x60010200)) && 
        (pOopRcsd->nNum != 0))
    {
        return ERR_OPTION;
    }

    switch (pOopRsd->choice)
    {
    case 0://方法0
    case 1://方法1
    case 2://方法2
    case 3://方法3
        {
            if (pOopRsd->choice == 0)
            {
                inReadMeter.startInfoNum = 1;
                inReadMeter.endInfoNum = NO_VIR_MTR_MAX_NUM;
            }       
            else if(pOopRsd->choice == 1)
            {
                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inReadMeter.startInfoNum = pOopRsd->sel1.value.wVal;
                    inReadMeter.endInfoNum = pOopRsd->sel1.value.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }
            }
            else if(pOopRsd->choice == 2)//前闭后开
            {
                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inReadMeter.startInfoNum = pOopRsd->sel2.from.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }

                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inReadMeter.endInfoNum = pOopRsd->sel2.to.wVal - 1;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }
            }
            else if(pOopRsd->choice == 3)
            {
                uint32 i     = 0;
                uint32 j     = 0;
                uint32 start = 0;
                uint32 end   = 0;
                inReadMeter.startInfoNum = 1;
                inReadMeter.endInfoNum = NO_VIR_MTR_MAX_NUM;
                for (i = 0; i < NO_VIR_MTR_MAX_NUM + 1; i++)
                {
                    inReadMeter.isValid[i] = 1;
                }

                for (i = 0; i < pOopRsd->sel3.nNum; i++)
                {
                    if (pOopRsd->sel3.item[i].from.type == DT_LONG_UNSIGNED)
                    {
                        start = pOopRsd->sel3.item[i].from.wVal;
                    }
                    else
                    {
                        PRTL_FMT_DEBUG("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                    }

                    if (pOopRsd->sel3.item[i].to.type == DT_LONG_UNSIGNED)
                    {
                        end = pOopRsd->sel3.item[i].to.wVal - 1;
                    }
                    else
                    {
                        PRTL_FMT_DEBUG("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                    }
                    for (j = start; j <= end; j++)
                    {
                        inReadMeter.isValid[j] = 0;
                    }
                }
            }
            else
            {
                nRet =  ERR_NORMAL;
            }
            nRet = ERR_OK;
        }
        break;

    default:
        nRet = ERR_NORMAL;
        break;
    }

    if (nRet == ERR_OK)
    {
        relyInfo.oopRcsd = *pOopRcsd;
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        *pInReadMeter = inReadMeter;
        *pRelyInfo     = relyInfo;
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     获取读记录档案参数
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 read_searchmeter_res_all_param_get(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, READ_NOR_BP_T *pInReadNor, RELY_INFO_T *pRelyInfo)
{
    int32 nRet = ERR_NORMAL;
    READ_NOR_BP_T   inReadNor = {0};
    RELY_INFO_T    relyInfo = {0};

    if (((pOopRcsd->nNum != 1) || (pOopRcsd->cols[0].oad.value != 0x60010200)) && 
        (pOopRcsd->nNum != 0))
    {
        return ERR_OPTION;
    }

    switch (pOopRsd->choice)
    {
    case 0://方法0
    case 1://方法1
    case 2://方法2
        {
            if (pOopRsd->choice == 0)
            {
                inReadNor.startInfoNum = 1;
                inReadNor.endInfoNum = NO_VIR_MTR_MAX_NUM;
            }       
            else if(pOopRsd->choice == 1)
            {
                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inReadNor.startInfoNum = pOopRsd->sel1.value.wVal;
                    inReadNor.endInfoNum = pOopRsd->sel1.value.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }
            }
            else if(pOopRsd->choice == 2)//前闭后开
            {
                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inReadNor.startInfoNum = pOopRsd->sel2.from.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }

                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inReadNor.endInfoNum = pOopRsd->sel2.to.wVal - 1;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }
            }
            else
            {
                nRet =  ERR_NORMAL;
            }
            nRet = ERR_OK;
        }
        break;

    default:
        nRet = ERR_NORMAL;
        break;
    }

    if (nRet == ERR_OK)
    {
        relyInfo.oopRcsd = *pOopRcsd;
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        *pInReadNor = inReadNor;
        *pRelyInfo     = relyInfo;
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     获取读记录档案参数
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 read_nor_param_get(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, READ_NOR_BP_T *pInReadNor, RELY_INFO_T *pRelyInfo)
{
    int32 nRet = ERR_NORMAL;
    READ_NOR_BP_T   inReadNor = {0};
    RELY_INFO_T    relyInfo = {0};

    if ((pOopRcsd->nNum != 1)&& 
        (pOopRcsd->nNum != 0))
    {
        return ERR_OPTION;
    }

    switch (pOopRsd->choice)
    {
    case 0://方法0
    case 1://方法1
    case 2://方法2
        {
            if (pOopRsd->choice == 0)
            {
                inReadNor.startInfoNum = 1;
                inReadNor.endInfoNum = 255;
            }       
            else if(pOopRsd->choice == 1)
            {
                if ((pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)||(pOopRsd->sel1.value.type ==DT_UNSIGNED))
                {
                    inReadNor.startInfoNum = pOopRsd->sel1.value.wVal;
                    inReadNor.endInfoNum = pOopRsd->sel1.value.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }
            }
            else if(pOopRsd->choice == 2)//前闭后开
            {
                if ((pOopRsd->sel2.from.type == DT_LONG_UNSIGNED)||(pOopRsd->sel2.from.type ==DT_UNSIGNED))
                {
                    inReadNor.startInfoNum = pOopRsd->sel2.from.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }

                if ((pOopRsd->sel2.to.type == DT_LONG_UNSIGNED)||(pOopRsd->sel2.to.type ==DT_UNSIGNED))
                {
                    inReadNor.endInfoNum = pOopRsd->sel2.to.wVal - 1;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }
            }
            else
            {
                nRet =  ERR_NORMAL;
            }
            nRet = ERR_OK;
        }
        break;

    default:
        nRet = ERR_NORMAL;
        break;
    }

    if (nRet == ERR_OK)
    {
        relyInfo.oopRcsd = *pOopRcsd;
        relyInfo.oopRsd = pOopRsd;
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        *pInReadNor = inReadNor;
        *pRelyInfo     = relyInfo;
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     获取读记录档案参数
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 read_oct_param_get(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, READ_NOR_OCT_T *pInReadNor, RELY_INFO_T *pRelyInfo)
{
    int32 nRet = ERR_NORMAL;
    READ_NOR_OCT_T inReadNor = {0};
    RELY_INFO_T    relyInfo = {0};

    if ((pOopRcsd->nNum != 1)&& 
        (pOopRcsd->nNum != 0))
    {
        return ERR_OPTION;
    }

    switch (pOopRsd->choice)
    {
    case 0://方法0
    case 1://方法1
    case 2://方法2
        {
            if (pOopRsd->choice == 0)
            {
                inReadNor.startInfoNum = 1;
                inReadNor.endInfoNum = 255;
            }       
            else if(pOopRsd->choice == 1)
            {
                if ((pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)||(pOopRsd->sel1.value.type ==DT_UNSIGNED))
                {
                    inReadNor.startInfoNum = pOopRsd->sel1.value.wVal;
                    inReadNor.endInfoNum = pOopRsd->sel1.value.wVal;
                }
                else if ((pOopRsd->sel1.value.type == DT_VISIBLE_STRING) || (pOopRsd->sel1.value.type == DT_OCTET_STRING))
                {
                    memcpy(&inReadNor.octVal, &pOopRsd->sel1.value.octVal, sizeof(inReadNor.octVal));
                }
                else
                {
                    PRTL_FMT_DEBUG("报文中的类型不支持 type[%d]\n", pOopRsd->sel1.value.type);
                }
            }
            else if(pOopRsd->choice == 2)//前闭后开
            {
                if ((pOopRsd->sel2.from.type == DT_LONG_UNSIGNED)||(pOopRsd->sel2.from.type ==DT_UNSIGNED))
                {
                    inReadNor.startInfoNum = pOopRsd->sel2.from.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }

                if ((pOopRsd->sel2.to.type == DT_LONG_UNSIGNED)||(pOopRsd->sel2.to.type ==DT_UNSIGNED))
                {
                    inReadNor.endInfoNum = pOopRsd->sel2.to.wVal - 1;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }
            }
            else
            {
                nRet =  ERR_NORMAL;
            }
            nRet = ERR_OK;
        }
        break;

    default:
        nRet = ERR_NORMAL;
        break;
    }

    if (nRet == ERR_OK)
    {
        relyInfo.oopRcsd = *pOopRcsd;
        relyInfo.oopRsd = pOopRsd;
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        *pInReadNor = inReadNor;
        *pRelyInfo     = relyInfo;
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     获取读记录任务监控参数
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 read_acq_monitor_param_get(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, READ_NOR_BP_T *pInReadNorBp, RELY_INFO_T *pRelyInfo)
{
    int32 nRet = ERR_NORMAL;
    READ_NOR_BP_T   inReadNorBp = {0};
    RELY_INFO_T    relyInfo = {0};

    switch (pOopRsd->choice)
    {
    case 1://方法1
    case 2://方法2
        {       
            if(pOopRsd->choice == 1)
            {
                if ((pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                    || (pOopRsd->sel1.value.type == DT_UNSIGNED))
                {
                    inReadNorBp.startInfoNum = pOopRsd->sel1.value.wVal;
                    inReadNorBp.endInfoNum = pOopRsd->sel1.value.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }
            }
            else if(pOopRsd->choice == 2)//前闭后开
            {
                if ((pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                    || (pOopRsd->sel1.value.type == DT_UNSIGNED))
                {
                    inReadNorBp.startInfoNum = pOopRsd->sel2.from.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }

                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inReadNorBp.endInfoNum = pOopRsd->sel2.to.wVal - 1;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }
            }
            else
            {
                nRet =  ERR_NORMAL;
            }
            nRet = ERR_OK;
        }
        break;

    default:
        nRet = ERR_NORMAL;
        break;
    }

    if (nRet == ERR_OK)
    {
        relyInfo.oopRcsd = *pOopRcsd;
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        *pInReadNorBp = inReadNorBp;
        *pRelyInfo     = relyInfo;
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     获取读记录任务监控参数
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 read_acq_rule_param_get(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, READ_NOR_BP_T *pInRead, RELY_INFO_T *pRelyInfo)
{
    int32 nRet = ERR_NORMAL;
    READ_NOR_BP_T   inRead = {0};
    RELY_INFO_T    relyInfo = {0};

    if ((pOopRcsd->nNum != 1) || (pOopRcsd->cols[0].oad.value != 0x60010200))
    {
        return ERR_OPTION;
    }

    switch (pOopRsd->choice)
    {
    case 1://方法1
    case 2://方法2
        {       
            if(pOopRsd->choice == 1)
            {
                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inRead.startInfoNum = pOopRsd->sel1.value.wVal;
                    inRead.endInfoNum = pOopRsd->sel1.value.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }
            }
            else if(pOopRsd->choice == 2)//前闭后开
            {
                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inRead.startInfoNum = pOopRsd->sel2.from.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }

                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inRead.endInfoNum = pOopRsd->sel2.to.wVal - 1;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }
            }
            else
            {
                nRet =  ERR_NORMAL;
            }
            nRet = ERR_OK;
        }
        break;

    default:
        nRet = ERR_NORMAL;
        break;
    }

    if (nRet == ERR_OK)
    {
        relyInfo.oopRcsd = *pOopRcsd;
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        *pInRead = inRead;
        *pRelyInfo     = relyInfo;
    }

    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter_record
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_oop_meter_record(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    /* 类型不存在 */
    if (pOdaInfo->pTab->oad.nObjID != 0x6000)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOdaInfo->pTab->oad.attID)
    {
    case 2:
        {
            OOP_PRSD_T oopRsd;
            OOP_RCSD_T oopRcsd;
            uint16 headLen = 0;
            READ_METER_T inReadMeter;
            RELY_INFO_T  relyInfo;
            RECORD_BUF_T outRecordBuf;

            /* 清0处理 */
            MEMZERO(&oopRsd, sizeof(oopRsd));
            MEMZERO(&oopRcsd, sizeof(oopRcsd));
            MEMZERO(&inReadMeter, sizeof(inReadMeter));
            MEMZERO(&relyInfo, sizeof(relyInfo));
            MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

            /*处理RSD*/
            nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
            PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
            PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
            PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
            PRTL_FMT_LOGCC("offset = %d\n", offset);
            if (nRet != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("RSD解析错误\n");

                /*应对电科院协议一致性测试*/
                uint16 testOffset = 4;
                uint16 inCsdOff = 0, inCsdLen = 0;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
                inCsdOff = testOffset;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
                inCsdLen = testOffset - inCsdOff;
                memcpy(outData->pResultData, inData, 4);
                memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
                outData->resultDataLen = 4 + inCsdLen;

                outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
                if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
                {
                    outData->pResultData[outData->resultDataLen++] = 0;

                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    for (i = 0; i < oopRcsd.nNum; i++)
                    {
                        outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                    }
                }

                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_SUCCESS;
            }
            else
            {
                inDataLenTmp -= offset;  //除去RSD和OAD后的长度
            }

            /*处理RCSD*/
            csdStart = offset;
            if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("CSD解析错误!\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_TYPE_UNMATCHED;
            }

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;

            if (csdLen == 1)
            {
                //如果rcsd为空，填充OAD60010200
                outData->pResultData[outOffset ++] = 0x01;
                outData->pResultData[outOffset ++] = 0x00;
                outData->pResultData[outOffset ++] = 0x60;
                outData->pResultData[outOffset ++] = 0x01;
                outData->pResultData[outOffset ++] = 0x02;
                outData->pResultData[outOffset ++] = 0x00;
            }
            else
            {
                memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
                outOffset += csdLen;
            }
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;
            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*记录型档案接口提供输入参数*/
            if (read_meter_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadMeter, &relyInfo) != ERR_OK)
            {
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /* 依赖信息中存下OAD列表的指针 */
            relyInfo.pOadInfo = pOdaInfo;

            PRTL_FMT_LOGCC("****************************************************\n");
            PRTL_FMT_LOGCC("inReadMeter.startInfoNum = %d\n", inReadMeter.startInfoNum);
            PRTL_FMT_LOGCC("inReadMeter.endInfoNum = %d\n", inReadMeter.endInfoNum);
            PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                      relyInfo.oopRcsd.cols[i].road.oadMain.value);
                    for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                    {
                        PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                                  relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                    }
                }
            }
            PRTL_FMT_LOGCC("****************************************************\n");

            /*读记录档案接口，可以读出apdu数据域，可返回断点信息*/
            if (record_meter_read(pApduInfo->hUdp, inReadMeter, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
            {
                nRet = DATA_READ_WRITE_DENIED;
            }
            else
            {
                memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
                outData->resultDataLen += outRecordBuf.bufLen;

                if (outRecordBuf.breakInfo.appSlicingFlag == 1)
                {
                    /*记下断点信息*/
                    PRTL_FMT_LOGCC("记下断点信息\n");
                    outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
                    memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
                }
                else
                {
                    /*清除断点信息*/
                    PRTL_FMT_LOGCC("清除断点信息\n");
                    memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                }
                
                nRet = DATA_SUCCESS;
            }
            
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_SUCCESS;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_oop_meter(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
    case 0x06:
        return class11_invoke_get_oop_meter_normal(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_oop_meter_record(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：      oop_searchmeter_res_num
* @brief：     获取搜表结果数
* @param[in] ：fd             数据库操作句柄
* @param[out]：num            搜表结果    
* @return    ：错误码
* @author    : 
* @Date      ：2020-01-12
*********************************************************************
*/
int32 oop_searchmeter_res_num(DB_CLIENT fd, uint32 *num)
{
    int32  ret = ERR_NORMAL;
    uint16 data = 0;
    uint32 len = 0;
    NOMAL_OAD_T normal;

    MEMZERO(&normal, sizeof(normal));

    //读出数据
    normal.oad.value = 0x60020600;
    
    ret = db_read_nomal(fd, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ERR_ITEM;
    }

    if (data <= 0)
    {
        return ERR_NORMAL;
    }

    *num = data;

    return ERR_OK;
}

/**
*********************************************************************
* @name：      class11_invoke_get_searchmeter_res_acrossarea
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_searchmeter_res_acrossarea(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint16    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60020500:
        {
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            OOP_ACROSSAREA_RES_T data;
            DATA_CONVERT_T convert;
            uint32 searcherMeterNum = 0;
            uint32 searcherMeterNumOffset = 0;

            MEMZERO(&data, sizeof(data));
            MEMZERO(&convert, sizeof(convert));

            #if SEEK_ACROSS_MOD == 1
            uint32 len = 0;
            NOMAL_OAD_T normal;
            MEMZERO(&normal, sizeof(normal));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOdaInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取所有搜表结果\n");

            //获取搜表结果数量，暂时不用，先直接搜索2040个
            // if (oop_searchmeter_res_num(pApduInfo->hUdp, &searcherMeterNum) < ERR_OK)
            // {
            //  return DATA_DATA_BLOCK_UNAVAILABLE;
            // }

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            searcherMeterNumOffset = offset;                                    //档案数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;
            for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                if (slicing_is_comtinue_searchmeter_res_acrossarea(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                #if SEEK_ACROSS_MOD == 1
                normal.infoNum = i; 
                ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_ACROSSAREA_RES_T)))
                #else
                ret = app_get_seek_across_list(i, &data);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到找到搜表结果 序号为%d\n", i);

                //转换成报文数据
                convert.srcBuf = &data;
                convert.srcLen = sizeof(data);
                convert.sOffset = &sOffset;
                
                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                dar = data_to_buf_class8_searchmeter_res_acrossarea(pOadInfo, &convert);
                if (dar == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个搜表报文:", i);
                        searcherMeterNum ++;
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

            PRTL_BUF_LOGCC(outData->pResultData, offset, "整个档案报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

            if (searcherMeterNum > 0)
            {
                result = DATA_SUCCESS;
                outData->pResultData[searcherMeterNumOffset] = searcherMeterNum;   //重新记档案数量
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
* @name：      class11_invoke_get_searchmeter_res_all_nor
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_searchmeter_res_all_nor(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint16    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60020200:
        {
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            OOP_SEARCHMET_RES_T data;
            DATA_CONVERT_T convert;
            uint32 searcherMeterNum = 0;
            uint32 searcherMeterNumOffset = 0;

            MEMZERO(&data, sizeof(data));
            MEMZERO(&convert, sizeof(convert));

            #if SEEK_METER_MOD == 1
            uint32 len = 0;
            NOMAL_OAD_T normal;
            MEMZERO(&normal, sizeof(normal));
            nomalOad.logicId = 0x00;
            normal.oad.value = pOadInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取所有搜表结果\n");

            //获取搜表结果数量，暂时不用，先直接搜索2040个
            // if (oop_searchmeter_res_num(pApduInfo->hUdp, &searcherMeterNum) < ERR_OK)
            // {
            //  return DATA_DATA_BLOCK_UNAVAILABLE;
            // }

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            searcherMeterNumOffset = offset;                                    //档案数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;
            for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                if (slicing_is_comtinue_searchmeter_res_all(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                #if SEEK_METER_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_SEARCHMET_RES_T)))
                #else
                ret = app_get_seek_meter_list(i, &data);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }
                PRTL_FMT_LOGCC("找到找到搜表结果 序号为%d\n", i);

                //转换成报文数据
                convert.srcBuf = &data;
                convert.srcLen = sizeof(data);
                convert.sOffset = &sOffset;
                
                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                dar = data_to_buf_class8_searchmeter_res_all(pOadInfo, &convert);
                if (dar == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个搜表报文:", i);
                        searcherMeterNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("class11_invoke_get_searchmeter_res_all_nor isBufFull == TRUE isBufFull = %d\n", isBufFull);
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

            PRTL_BUF_LOGCC(outData->pResultData, offset, "整个搜表报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

//            if (searcherMeterNum > 0)
            {
                result = DATA_SUCCESS;
                outData->pResultData[searcherMeterNumOffset] = searcherMeterNum;   //重新记档案数量
                outData->resultDataLen = offset;
            }
//            else
//            {
//                outData->resultDataLen = 0;
//                return DATA_READ_WRITE_DENIED;
//            }
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
* @name：      class11_invoke_get_searchmeter_res_all_rec
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_searchmeter_res_all_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    /* 类型不存在 */
    if (pOdaInfo->pTab->oad.nObjID != 0x6002)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOdaInfo->pTab->oad.attID)
    {
    case 2:
        {
            OOP_PRSD_T oopRsd;
            OOP_RCSD_T oopRcsd;
            uint16 headLen = 0;
            READ_NOR_BP_T inReadNor;
            RELY_INFO_T  relyInfo;
            RECORD_BUF_T outRecordBuf;

            /* 清0处理 */
            MEMZERO(&oopRsd, sizeof(oopRsd));
            MEMZERO(&oopRcsd, sizeof(oopRcsd));
            MEMZERO(&inReadNor, sizeof(inReadNor));
            MEMZERO(&relyInfo, sizeof(relyInfo));
            MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

            /*处理RSD*/
            nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
            PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
            PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
            PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
            PRTL_FMT_LOGCC("offset = %d\n", offset);
            if (nRet != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("RSD解析错误\n");

                /*应对电科院协议一致性测试*/
                uint16 testOffset = 4;
                uint16 inCsdOff = 0, inCsdLen = 0;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
                inCsdOff = testOffset;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
                inCsdLen = testOffset - inCsdOff;
                memcpy(outData->pResultData, inData, 4);
                memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
                outData->resultDataLen = 4 + inCsdLen;

                outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
                if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
                {
                    outData->pResultData[outData->resultDataLen++] = 0;

                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    for (i = 0; i < oopRcsd.nNum; i++)
                    {
                        outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                    }
                }

                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_SUCCESS;
            }
            else
            {
                inDataLenTmp -= offset;  //除去RSD和OAD后的长度
            }

            /*处理RCSD*/
            csdStart = offset;
            if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("CSD解析错误!\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_TYPE_UNMATCHED;
            }

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;

            if (csdLen == 1)
            {
                //如果rcsd为空，填充OAD60010200
                outData->pResultData[outOffset ++] = 0x01;
                outData->pResultData[outOffset ++] = 0x00;
                outData->pResultData[outOffset ++] = 0x60;
                outData->pResultData[outOffset ++] = 0x03;
                outData->pResultData[outOffset ++] = 0x02;
                outData->pResultData[outOffset ++] = 0x00;
            }
            else
            {
                memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
                outOffset += csdLen;
            }
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;
            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*记录型档案接口提供输入参数*/
            if (read_searchmeter_res_all_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
            {
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /* 依赖信息中存下OAD列表的指针 */
            relyInfo.pOadInfo = pOdaInfo;

            PRTL_FMT_LOGCC("****************************************************\n");
            PRTL_FMT_LOGCC("inReadNor.startInfoNum = %d\n", inReadNor.startInfoNum);
            PRTL_FMT_LOGCC("inReadNor.endInfoNum = %d\n", inReadNor.endInfoNum);
            PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                      relyInfo.oopRcsd.cols[i].road.oadMain.value);
                    for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                    {
                        PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                                  relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                    }
                }
            }
            PRTL_FMT_LOGCC("****************************************************\n");

            /*读记录接口，可以读出apdu数据域，可返回断点信息*/
            if (record_searchmeter_res_all_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
            {
                nRet = DATA_READ_WRITE_DENIED;
            }
            else
            {
                memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
                outData->resultDataLen += outRecordBuf.bufLen;

                if (outRecordBuf.breakInfo.appSlicingFlag == 1)
                {
                    /*记下断点信息*/
                    PRTL_FMT_LOGCC("记下断点信息\n");
                    outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
                    memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
                }
                else
                {
                    /*清除断点信息*/
                    PRTL_FMT_LOGCC("清除断点信息\n");
                    memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                }
                
                nRet = DATA_SUCCESS;
            }
            
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_SUCCESS;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}


/**
*********************************************************************
* @name：      class11_invoke_get_searchmeter_res_all
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_searchmeter_res_all(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
    case 0x06:
        return class11_invoke_get_searchmeter_res_all_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_searchmeter_res_all_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：      class8_invoke_get
* @brief：     class8的GET服务
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
OOP_DAR_E class11_invoke_get_searchmeter_all_num(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    //NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    //uint32 len = 0;  

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60020600:
        {
            uint16 data = 0;
//            normal.oad.value = pOadInfo->pTab->oad.value;
//            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
//            if((ret != ERR_OK) || (len == 0))
//            {
//                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
//                    ret, normal.logicId, normal.infoNum, normal.oad.value);
//                return DATA_OTHER_REASON;
//            }
            data = app_get_seek_meter_count();
            PRTL_FMT_LOGCC("class11_invoke_get_searchmeter_all_num : data = %d\n", data);

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
    dar = data_to_buf_class11(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class11 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class8_invoke_get
* @brief：     class8的GET服务
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
OOP_DAR_E class11_invoke_get_searchmeter_acrossarea_num(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    //NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    //uint32 len = 0;  

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60020700:
        {
            uint16 data = 0;
//            normal.oad.value = pOadInfo->pTab->oad.value;
//            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
//            if((ret != ERR_OK) || (len == 0))
//            {
//                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
//                    ret, normal.logicId, normal.infoNum, normal.oad.value);
//                return DATA_OTHER_REASON;
//            }
            data = app_get_seek_across_count();
            PRTL_FMT_LOGCC("class11_invoke_get_searchmeter_acrossarea_num : data = %d\n", data);

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
    dar = data_to_buf_class11(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class11 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class11_invoke_get
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_searchmeter_param(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    case 0x60020800:
        {
            OOP_SEARCHPARAM_T data;
            MEMZERO(&data, sizeof(data));

            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
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

            PRTL_FMT_LOGCC("class11_invoke_get_searchmeter_param : data.periodSearch = %d\n", data.periodSearch);
            PRTL_FMT_LOGCC("class11_invoke_get_searchmeter_param : data.atuoUpdate = %d\n", data.atuoUpdate);
            PRTL_FMT_LOGCC("class11_invoke_get_searchmeter_param : data.touchEvent = %d\n", data.touchEvent);
            PRTL_FMT_LOGCC("class11_invoke_get_searchmeter_param : data.clearChoice = %d\n", data.clearChoice);

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
    dar = data_to_buf_class11(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class11 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class11_invoke_get
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_time_searchmeter_param(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0; 
    uint32 i = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60020900:
        {
            OOP_TIMINGSEARCH_T data;
            MEMZERO(&data, sizeof(data));

            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
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

            PRTL_FMT_LOGCC("class11_invoke_get_time_searchmeter_param : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class11_invoke_get_time_searchmeter_param : data.period[%d].start.hour = %d\n", i, data.period[i].start.hour);
                PRTL_FMT_LOGCC("class11_invoke_get_time_searchmeter_param : data.period[%d].start.minute = %d\n", i, data.period[i].start.minute);
                PRTL_FMT_LOGCC("class11_invoke_get_time_searchmeter_param : data.period[%d].start.second = %d\n", i, data.period[i].start.second);
                PRTL_FMT_LOGCC("class11_invoke_get_time_searchmeter_param : data.period[%d].time = %d\n", i, data.period[i].time);
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
    dar = data_to_buf_class11(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class11 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class11_invoke_get
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_searchmeter_status(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    case 0x60020A00:
        {
            uint8 data;
            MEMZERO(&data, sizeof(data));

            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
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

            PRTL_FMT_LOGCC("class11_invoke_get_time_searchmeter_param : data = %d\n", data);

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
    dar = data_to_buf_class11(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class11 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}


OOP_DAR_E class11_invoke_get_searchmeter_max_count(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
     //int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    //uint32 len = 0;  

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60020400:
        {
            uint16 data = NO_VIR_MTR_MAX_NUM;

            PRTL_FMT_LOGCC("class11_invoke_get_searchmeter_max_count : data = %d\n", data);

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
    dar = data_to_buf_class11(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class11 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class11_invoke_get_searchmeter_res
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_searchmeter_res(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60020200:
        return class11_invoke_get_searchmeter_res_all(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x60020400:
        return class11_invoke_get_searchmeter_max_count(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x60020500:
        return class11_invoke_get_searchmeter_res_acrossarea(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;

    case 0x60020600:
        return class11_invoke_get_searchmeter_all_num(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;

    case 0x60020700:
        return class11_invoke_get_searchmeter_acrossarea_num(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;

    case 0x60020800:
        return class11_invoke_get_searchmeter_param(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;

    case 0x60020900:
        return class11_invoke_get_time_searchmeter_param(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x60020A00:
        return class11_invoke_get_searchmeter_status(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;

    default:
        return DATA_OBJECT_UNDEFINED;
        break;
    }
}

/**
*********************************************************************
* @name：      class11_invoke_get_normal_scheme_nor
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_normal_scheme_nor(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int result = DATA_OTHER_REASON;
    uint32 len;
    uint32 offset = 0;
    uint16 i;
    
    int32 ret = ERR_NORMAL;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    OOP_PLAN_NORMAL_T PLAN_NORMAL_T;
    NOMAL_OAD_T NormalOad ;
    DATA_CONVERT_T convert;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;
    
    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T)); 
   
    
 
    switch (pOdaInfo->pTab->oad.nObjID)
    {
        case 0x6014:
        {
            if(pOdaInfo->nIndex==0x00)
            {
                uint8 normalschemenum = 0;
                uint32 oopnormalschemeNumOffset = 0;
                memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;                             //结果
                outData->pResultData[offset ++] = DT_ARRAY;                         //类型
                oopnormalschemeNumOffset = offset;                                         //任务数量偏移
                outData->pResultData[offset ++] = 0x00;                             //数量，先填0
                bufAllLen = offset;
                for(i=1;i<=255;i++)
                { 
                    if (slicing_is_comtinue_normal_scheme(pApduInfo->bpBreakInfo, i))
                    {
                        continue;
                    }
                    NormalOad.logicId = 0;
                    NormalOad.infoNum = i;
                    NormalOad.oad.value=0x60140200;

                    memset((uint8*)&PLAN_NORMAL_T,0x00,sizeof(OOP_PLAN_NORMAL_T));
                    #if NOR_PLAN_READ_MOD == 1
                    ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,sizeof(OOP_PLAN_NORMAL_T), (uint8*)&PLAN_NORMAL_T, &len);
                    if ((ret != ERR_OK) || (len != sizeof(PLAN_NORMAL_T)))
                    #else
                    ret = app_get_plan_normal_list(i, &PLAN_NORMAL_T);
                    if (ret != ERR_OK)
                    #endif
                    {
                        continue;
                    }

                    //转换成报文数据
                    convert.srcBuf = &PLAN_NORMAL_T;
                    convert.srcLen = sizeof(OOP_PLAN_NORMAL_T);
                    convert.sOffset = &sOffset;

                    convert.dstBuf = &outData->pResultData[offset];
                    convert.dstLen = outDataMax - offset;
                    convert.dOffset = &dOffset;

                    //记录断点信息
                    breakInfo.index = i;
                    
                    ret = data_to_buf_normal_scheme(pOdaInfo,&convert);
                    
                    if(ret == ERR_OK)
                    {
                        bufAllLen += dOffset;
                        if (bufAllLen <= maxBufLen)
                        {
                            memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                            offset += dOffset;
                            outData->resultDataLen = offset;
                            PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个普通方案报文:", i);
                            normalschemenum ++;
                            dOffset = 0;
                            sOffset = 0;
                        }
                        else
                        {
                            dOffset = 0;
                            sOffset = 0;
                            isBufFull = TRUE;
                            PRTL_FMT_LOGCC("class10_invoke_get_task_list_nor isBufFull == TRUE isBufFull = %d\n", isBufFull);
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
                if (normalschemenum > 0)
                {
                    outData->pResultData[oopnormalschemeNumOffset] = normalschemenum;   //重新记档案数量
                    outData->resultDataLen = offset;
                    result = DATA_SUCCESS;
                }
                else if(normalschemenum == 0)
                {
                     /* 填写报文开头处的OAD和数据结果0x01*/
                    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                    outData->pResultData[4]=DT_ARRAY;
                    outData->pResultData[5]=0x01;
                    outData->pResultData[6]=0x00;
                    outData->resultDataLen=7;
                    result = DATA_SUCCESS;
                }
                else
                {
                    result = DATA_SUCCESS;
                }
            }
            else
            {
                NormalOad.logicId = 0;
                NormalOad.infoNum = pOdaInfo->nIndex;
                NormalOad.oad.value=0x60140200;
                convert.dstBuf = &outData->pResultData[5];
                convert.dstLen = outDataMax - 5;
                convert.dOffset = &dOffset;
                memset((uint8*)&PLAN_NORMAL_T,0x00,sizeof(PLAN_NORMAL_T));
                #if NOR_PLAN_READ_MOD == 1
                ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,sizeof(PLAN_NORMAL_T), (uint8*)&PLAN_NORMAL_T, &len);
                if ((ret != ERR_OK) || (len != sizeof(PLAN_NORMAL_T)))
                #else
                ret = app_get_plan_normal_list(i, &PLAN_NORMAL_T);
                if (ret != ERR_OK)
                #endif
                {
                    return DATA_OBJECT_UNAVAILABLE;
                }

                //转换成报文数据
                convert.srcBuf = &PLAN_NORMAL_T;
                convert.srcLen = sizeof(OOP_PLAN_NORMAL_T);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;
                    
                result=data_to_buf_normal_scheme(pOdaInfo,&convert);
                if(result == ERR_OK)
                {
                    
                     /* 填写报文开头处的OAD和数据结果0x01*/
                    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                    outData->pResultData[4]=0x01;
                    outData->resultDataLen=dOffset+5;
                }
                else
                {
                     /* 填写报文开头处的OAD和数据结果0x01*/
                    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                    outData->pResultData[4]=0x00;
                    outData->pResultData[5]=DATA_OBJECT_UNAVAILABLE;
                    outData->resultDataLen=6;
                }
            }
        }
        break;
        default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }
    return result;
}

/**
*********************************************************************
* @name：      class11_invoke_get_normal_scheme_rec
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_normal_scheme_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    /* 类型不存在 */
    if (pOdaInfo->pTab->oad.nObjID != 0x6014)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOdaInfo->pTab->oad.attID)
    {
    case 2:
        {
            OOP_PRSD_T oopRsd;
            OOP_RCSD_T oopRcsd;
            uint16 headLen = 0;
            READ_NOR_BP_T inReadNor;
            RELY_INFO_T  relyInfo;
            RECORD_BUF_T outRecordBuf;

            /* 清0处理 */
            MEMZERO(&oopRsd, sizeof(oopRsd));
            MEMZERO(&oopRcsd, sizeof(oopRcsd));
            MEMZERO(&inReadNor, sizeof(inReadNor));
            MEMZERO(&relyInfo, sizeof(relyInfo));
            MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

            /*处理RSD*/
            nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
            PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
            PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
            PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
            PRTL_FMT_LOGCC("offset = %d\n", offset);
            if (nRet != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("RSD解析错误\n");

                /*应对电科院协议一致性测试*/
                uint16 testOffset = 4;
                uint16 inCsdOff = 0, inCsdLen = 0;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
                inCsdOff = testOffset;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
                inCsdLen = testOffset - inCsdOff;
                memcpy(outData->pResultData, inData, 4);
                memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
                outData->resultDataLen = 4 + inCsdLen;

                outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
                if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
                {
                    outData->pResultData[outData->resultDataLen++] = 0;

                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    for (i = 0; i < oopRcsd.nNum; i++)
                    {
                        outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                    }
                }

                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_SUCCESS;
            }
            else
            {
                inDataLenTmp -= offset;  //除去RSD和OAD后的长度
            }

            /*处理RCSD*/
            csdStart = offset;
            if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("CSD解析错误!\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_TYPE_UNMATCHED;
            }

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;

            if (csdLen == 1)
            {
                //如果rcsd为空，填充OAD60010200
                outData->pResultData[outOffset ++] = 0x01;
                outData->pResultData[outOffset ++] = 0x00;
                outData->pResultData[outOffset ++] = 0x60;
                outData->pResultData[outOffset ++] = 0x15;
                outData->pResultData[outOffset ++] = 0x02;
                outData->pResultData[outOffset ++] = 0x00;
            }
            else
            {
                memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
                outOffset += csdLen;
            }
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;
            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*记录型档案接口提供输入参数*/
            if (read_nor_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
            {
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /* 依赖信息中存下OAD列表的指针 */
            relyInfo.pOadInfo = pOdaInfo;

            PRTL_FMT_LOGCC("****************************************************\n");
            PRTL_FMT_LOGCC("inReadNor.startInfoNum = %d\n", inReadNor.startInfoNum);
            PRTL_FMT_LOGCC("inReadNor.endInfoNum = %d\n", inReadNor.endInfoNum);
            PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                      relyInfo.oopRcsd.cols[i].road.oadMain.value);
                    for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                    {
                        PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                                  relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                    }
                }
            }
            PRTL_FMT_LOGCC("****************************************************\n");

            /*读记录档案接口，可以读出apdu数据域，可返回断点信息*/
            if (record_normal_scheme_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
            {
                nRet = DATA_READ_WRITE_DENIED;
            }
            else
            {
                memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
                outData->resultDataLen += outRecordBuf.bufLen;

                if (outRecordBuf.breakInfo.appSlicingFlag == 1)
                {
                    /*记下断点信息*/
                    PRTL_FMT_LOGCC("记下断点信息\n");
                    outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
                    memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
                }
                else
                {
                    /*清除断点信息*/
                    PRTL_FMT_LOGCC("清除断点信息\n");
                    memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                }
                
                nRet = DATA_SUCCESS;
            }
            
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_SUCCESS;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}


/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_normal_scheme(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
    case 0x06:
        return class11_invoke_get_normal_scheme_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_normal_scheme_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：       class11_invoke_get
* @brief：      class11的GET服务
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
OOP_DAR_E class11_invoke_get_event_scheme_nor(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int result = DATA_OTHER_REASON;
    uint32 ret,len;
    uint32 offset=0;
    uint16 i;
    
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    OOP_PLAN_EVENT_T PLAN_EVENT_T;
    NOMAL_OAD_T NormalOad ;
    DATA_CONVERT_T convert;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;
    
    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
   

    PRTL_FMT_LOGCC("class11_invoke_get_event_scheme\n");
    switch (pOdaInfo->pTab->oad.nObjID)
    {
        case 0x6016:
        {
            if(pOdaInfo->nIndex==0x00)
            {
                uint8 eventschemenum = 0;
                uint32 oopeventschemeNumOffset = 0;
                memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;                             //结果
                outData->pResultData[offset ++] = DT_ARRAY;                         //类型
                oopeventschemeNumOffset = offset;                                         //任务数量偏移
                outData->pResultData[offset ++] = 0x00;                             //数量，先填0
                bufAllLen = offset;
                for(i=1;i<=255;i++)
                { 
                    if (slicing_is_comtinue_event_scheme(pApduInfo->bpBreakInfo, i))
                    {
                        continue;
                    }
                    NormalOad.logicId = 0;
                    NormalOad.infoNum = i;
                    NormalOad.oad.value=0x60160200;

                    memset((uint8*)&PLAN_EVENT_T,0x00,sizeof(OOP_PLAN_EVENT_T));
                    #if EVENT_PLAN_READ_MOD == 1
                    ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,sizeof(OOP_PLAN_EVENT_T), (uint8*)&PLAN_EVENT_T, &len);
                    if ((ret != ERR_OK) || (len != sizeof(PLAN_EVENT_T)))
                    #else
                    ret = app_get_plan_event_list(i, &PLAN_EVENT_T);
                    if (ret != ERR_OK)
                    #endif
                    {
                        continue;
                    }

                     //转换成报文数据
                    convert.srcBuf = &PLAN_EVENT_T;
                    convert.srcLen = sizeof(OOP_PLAN_EVENT_T);
                    convert.sOffset = &sOffset;

                    convert.dstBuf = &outData->pResultData[offset];
                    convert.dstLen = outDataMax - offset;
                    convert.dOffset = &dOffset;

                    //记录断点信息
                    breakInfo.index = i;
                    
                    result=data_to_buf_event_scheme(pOdaInfo,&convert);
                    PRTL_FMT_LOGCC("result is %d\n",result);
                    if(result == ERR_OK)
                    {
                        bufAllLen += dOffset;
                        if (bufAllLen <= maxBufLen)
                        {
                            memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                            offset += dOffset;
                            outData->resultDataLen = offset;
                            PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个任务报文:", i);
                            eventschemenum ++;
                            dOffset = 0;
                            sOffset = 0;
                        }
                        else
                        {
                            dOffset = 0;
                            sOffset = 0;
                            isBufFull = TRUE;
                            PRTL_FMT_LOGCC("class10_invoke_get_task_list_nor isBufFull == TRUE isBufFull = %d\n", isBufFull);
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

                PRTL_BUF_LOGCC(outData->pResultData, offset, "整个任务报文:");
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

                if (eventschemenum > 0)
                {
                    outData->pResultData[oopeventschemeNumOffset] = eventschemenum;   //重新记档案数量
                    outData->resultDataLen = offset;
                    result = DATA_SUCCESS;
                }
                else if(eventschemenum == 0)
                {
                    memcpy_r(outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
                    outData->pResultData[4]=DT_ARRAY;
                    outData->pResultData[5]=0x01;
                    outData->pResultData[6]=0x00;
                    outData->resultDataLen=7;
                    result = DATA_SUCCESS;
                }
                else
                {
                    result = DATA_SUCCESS;
                }
                
                

            }
            else
            {
                NormalOad.logicId = 0;
                NormalOad.infoNum = pOdaInfo->nIndex;
                NormalOad.oad.value=0x60160200;
                convert.dstBuf = &outData->pResultData[5];
                convert.dstLen = outDataMax - 5;
                convert.dOffset = &dOffset;
                memset((uint8*)&PLAN_EVENT_T,0x00,sizeof(PLAN_EVENT_T));
                #if EVENT_PLAN_READ_MOD == 1
                ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,sizeof(PLAN_EVENT_T), (uint8*)&PLAN_EVENT_T, &len);
                if ((ret != ERR_OK) || (len != sizeof(PLAN_EVENT_T)))
                #else
                ret = app_get_plan_event_list(i, &PLAN_EVENT_T);
                if (ret != ERR_OK)
                #endif
                {
                    return DATA_OBJECT_UNAVAILABLE;
                }

                //转换成报文数据
                convert.srcBuf = &PLAN_EVENT_T;
                convert.srcLen = sizeof(OOP_PLAN_EVENT_T);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;
                
                result=data_to_buf_event_scheme(pOdaInfo,&convert);
                 if(result == ERR_OK)
                {
                   
                    memcpy_r(outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
                    outData->pResultData[4]=0x01;
                    outData->resultDataLen=dOffset+5;
                }
                else
                {
                    memcpy_r(outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
                    outData->pResultData[4]=0x00;
                    outData->pResultData[5]=DATA_OBJECT_UNAVAILABLE;
                    outData->resultDataLen=6;
                }
            }
        }
        break;
        default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }
    return result;
}

/**
*********************************************************************
* @name：      class11_invoke_get_event_scheme_rec
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_transparent_scheme_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    /* 类型不存在 */
    if (pOdaInfo->pTab->oad.nObjID != 0x6018)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOdaInfo->pTab->oad.attID)
    {
    case 2:
        {
            OOP_PRSD_T oopRsd;
            OOP_RCSD_T oopRcsd;
            uint16 headLen = 0;
            READ_NOR_BP_T inReadNor;
            RELY_INFO_T  relyInfo;
            RECORD_BUF_T outRecordBuf;

            /* 清0处理 */
            MEMZERO(&oopRsd, sizeof(oopRsd));
            MEMZERO(&oopRcsd, sizeof(oopRcsd));
            MEMZERO(&inReadNor, sizeof(inReadNor));
            MEMZERO(&relyInfo, sizeof(relyInfo));
            MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

            /*处理RSD*/
            nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
            PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
            PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
            PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
            PRTL_FMT_LOGCC("offset = %d\n", offset);
            if (nRet != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("RSD解析错误\n");

                /*应对电科院协议一致性测试*/
                uint16 testOffset = 4;
                uint16 inCsdOff = 0, inCsdLen = 0;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
                inCsdOff = testOffset;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
                inCsdLen = testOffset - inCsdOff;
                memcpy(outData->pResultData, inData, 4);
                memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
                outData->resultDataLen = 4 + inCsdLen;

                outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
                if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
                {
                    outData->pResultData[outData->resultDataLen++] = 0;

                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    for (i = 0; i < oopRcsd.nNum; i++)
                    {
                        outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                    }
                }

                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_SUCCESS;
            }
            else
            {
                inDataLenTmp -= offset;  //除去RSD和OAD后的长度
            }

            /*处理RCSD*/
            csdStart = offset;
            if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("CSD解析错误!\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_TYPE_UNMATCHED;
            }

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;

            if (csdLen == 1)
            {
                //如果rcsd为空，填充OAD60010200
                outData->pResultData[outOffset ++] = 0x01;
                outData->pResultData[outOffset ++] = 0x00;
                outData->pResultData[outOffset ++] = 0x60;
                outData->pResultData[outOffset ++] = 0x19;
                outData->pResultData[outOffset ++] = 0x02;
                outData->pResultData[outOffset ++] = 0x00;
            }
            else
            {
                memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
                outOffset += csdLen;
            }
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;
            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*记录型档案接口提供输入参数*/
            if (read_nor_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
            {
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /* 依赖信息中存下OAD列表的指针 */
            relyInfo.pOadInfo = pOdaInfo;

            PRTL_FMT_LOGCC("****************************************************\n");
            PRTL_FMT_LOGCC("inReadNor.startInfoNum = %d\n", inReadNor.startInfoNum);
            PRTL_FMT_LOGCC("inReadNor.endInfoNum = %d\n", inReadNor.endInfoNum);
            PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                      relyInfo.oopRcsd.cols[i].road.oadMain.value);
                    for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                    {
                        PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                                  relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                    }
                }
            }
            PRTL_FMT_LOGCC("****************************************************\n");

            /*读记录档案接口，可以读出apdu数据域，可返回断点信息*/
            if (record_trans_scheme_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
            {
                nRet = DATA_READ_WRITE_DENIED;
            }
            else
            {
                memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
                outData->resultDataLen += outRecordBuf.bufLen;

                if (outRecordBuf.breakInfo.appSlicingFlag == 1)
                {
                    /*记下断点信息*/
                    PRTL_FMT_LOGCC("记下断点信息\n");
                    outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
                    memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
                }
                else
                {
                    /*清除断点信息*/
                    PRTL_FMT_LOGCC("清除断点信息\n");
                    memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                }
                
                nRet = DATA_SUCCESS;
            }
            
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_SUCCESS;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}

/**
*********************************************************************
* @name：       class11_invoke_get
* @brief：      class11的GET服务
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
OOP_DAR_E class11_invoke_get_report_scheme_nor(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int result = DATA_OTHER_REASON;
    uint32 ret,len;
    uint32 offset=0;
    uint16 i;
    
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    OOP_PLAN_REPORT_T PLAN_REPORT_T;
    NOMAL_OAD_T NormalOad ;
    DATA_CONVERT_T convert;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;
    
    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   

    PRTL_FMT_LOGCC("class11_invoke_get_report_scheme_nor\n");
    switch (pOdaInfo->pTab->oad.nObjID)
    {
        case 0x601C:
        {
            if(pOdaInfo->nIndex==0x00)
            {
                uint8 reportplannum = 0;
                uint32 oopreportplanNumOffset = 0;
                memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;                             //结果
                outData->pResultData[offset ++] = DT_ARRAY;                         //类型
                oopreportplanNumOffset = offset;                                         //任务数量偏移
                outData->pResultData[offset ++] = 0x00;                             //数量，先填0
                bufAllLen = offset;
                for(i=1;i<=255;i++)
                { 
                    if (slicing_is_comtinue(pApduInfo->bpBreakInfo, i))
                    {
                        continue;
                    }
                    NormalOad.logicId = 0;
                    NormalOad.infoNum = i;
                    NormalOad.oad.value=0x601C0200;

                    memset((uint8*)&PLAN_REPORT_T,0x00,sizeof(PLAN_REPORT_T));
                    #if REPORT_PLAN_READ_MOD == 1
                    ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,sizeof(PLAN_REPORT_T), (uint8*)&PLAN_REPORT_T, &len);
                    if ((ret != ERR_OK) || (len != sizeof(PLAN_REPORT_T)))
                    #else
                    ret = app_get_plan_report_list(i, &PLAN_REPORT_T);
                    if (ret != ERR_OK)
                    #endif
                    {
                        continue;
                    }
                    //转换成报文数据
                    convert.srcBuf = &PLAN_REPORT_T;
                    convert.srcLen = sizeof(PLAN_REPORT_T);
                    convert.sOffset = &sOffset;

                    convert.dstBuf = &outData->pResultData[offset];
                    convert.dstLen = outDataMax - offset;
                    convert.dOffset = &dOffset;

                    //记录断点信息
                    breakInfo.index = i;
                    
                    result=data_to_buf_report_plan(pOdaInfo,&convert);
                    PRTL_FMT_LOGCC("result is %d\n",result);
                    if(result == ERR_OK)
                    {
                        bufAllLen += dOffset;
                        if (bufAllLen <= maxBufLen)
                        {
                            memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                            offset += dOffset;
                            outData->resultDataLen = offset;
                            PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个任务报文:", i);
                            reportplannum ++;
                            dOffset = 0;
                            sOffset = 0;
                        }
                        else
                        {
                            dOffset = 0;
                            sOffset = 0;
                            isBufFull = TRUE;
                            PRTL_FMT_LOGCC("class10_invoke_get_task_list_nor isBufFull == TRUE isBufFull = %d\n", isBufFull);
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

                PRTL_BUF_LOGCC(outData->pResultData, offset, "整个任务报文:");
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

                if (reportplannum > 0)
                {
                    outData->pResultData[oopreportplanNumOffset] = reportplannum;   //重新记档案数量
                    outData->resultDataLen = offset;
                    result = DATA_SUCCESS;
                }
                else if(reportplannum == 0)
                {
                    memcpy_r(outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
                    outData->pResultData[4]=DT_ARRAY;
                    outData->pResultData[5]=0x01;
                    outData->pResultData[6]=0x00;
                    outData->resultDataLen=7;
                    result = DATA_SUCCESS;
                }
                else
                {
                    result = DATA_SUCCESS;
                }
                

            }
            else
            {
                NormalOad.logicId = 0;
                NormalOad.infoNum = pOdaInfo->nIndex;
                NormalOad.oad.value=0x601C0200;

                convert.dstBuf = &outData->pResultData[5];
                convert.dstLen = outDataMax - 5;
                convert.dOffset = &dOffset;

                memset((uint8*)&PLAN_REPORT_T,0x00,sizeof(PLAN_REPORT_T));
                #if REPORT_PLAN_READ_MOD == 1
                ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,sizeof(PLAN_REPORT_T), (uint8*)&PLAN_REPORT_T, &len);
                if ((ret != ERR_OK) || (len != sizeof(PLAN_REPORT_T)))
                #else
                ret = app_get_plan_report_list(i, &PLAN_REPORT_T);
                if (ret != ERR_OK)
                #endif
                {
                    return DATA_OBJECT_UNAVAILABLE;
                }

                //转换成报文数据
                convert.srcBuf = &PLAN_REPORT_T;
                convert.srcLen = sizeof(PLAN_REPORT_T);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;
                
                result=data_to_buf_report_plan(pOdaInfo,&convert);
                if(result == ERR_OK)
                {
                    memcpy(outData->pResultData,inData,4);
                    outData->pResultData[4]=0x01;
                    outData->resultDataLen=dOffset+5;
                }
                else
                {
                    memcpy(outData->pResultData,inData,4);
                    outData->pResultData[4]=0x00;
                    outData->pResultData[5]=DATA_OBJECT_UNAVAILABLE;
                    outData->resultDataLen=6;
                }
            }
        }
        break;
        default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }
    return result;
}

/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter
* @brief：     class11的GET服务
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
//OOP_DAR_E class11_invoke_get_normal_scheme(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
//{
//    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
//    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
//    {
//        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
//    }
//
//    switch (pApduInfo->reqType)
//    {
//    case 0x01:
//    case 0x02:
//        return class11_invoke_get_normal_scheme_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
//        break;
//    case 0x03:
//    case 0x04:
//        return class11_invoke_get_normal_scheme_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
//        break;  
//    default:
//        break;
//    }
//
//    return DATA_OBJECT_UNDEFINED;
//}

/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_transparent_scheme(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
    case 0x06:
        return class11_invoke_get_transparent_scheme_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_transparent_scheme_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：       class11_invoke_get
* @brief：      class11的GET服务
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
//OOP_DAR_E class11_invoke_get_event_scheme_nor(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
//{
//    int result = DATA_OTHER_REASON;
//    uint32 ret,len;
//    uint16 i;
//    uint8 num=0;
//    uint32 sOffset = 0;
//    uint32 dOffset = 0;
//    OOP_PLAN_EVENT_T PLAN_EVENT_T;
//    NOMAL_OAD_T NormalOad ;
//    DATA_CONVERT_T convert;
//    
//   
//    //转换成报文数据
//    convert.srcBuf = &PLAN_EVENT_T;
//    convert.srcLen = sizeof(OOP_PLAN_EVENT_T);
//    convert.sOffset = &sOffset;
//
//    convert.dstBuf = &outData->pResultData[7];
//    convert.dstLen = outDataMax - 7;
//    convert.dOffset = &dOffset;
//
//    PRTL_FMT_LOGCC("class11_invoke_get_event_scheme\n");
//    switch (pOdaInfo->pTab->oad.nObjID)
//    {
//        case 0x6016:
//        {
//            if(pOdaInfo->pTab->oad.nIndex==0x00)
//            {
//                for(i=0;i<=255;i++)
//                { 
//                    NormalOad.logicId = 0;
//                    NormalOad.infoNum = i;
//                    NormalOad.oad.value=0x60160200;
//
//                    memset((uint8*)&PLAN_EVENT_T,0x00,sizeof(OOP_PLAN_EVENT_T));
//                    ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,sizeof(OOP_PLAN_EVENT_T), (uint8*)&PLAN_EVENT_T, &len);
//                    if((ret !=0)||(len==0))
//                    {
//                        continue;
//                    }
//                    result=data_to_buf_event_scheme(pOdaInfo,&convert);
//                    PRTL_FMT_LOGCC("result is %d\n",result);
//                    if(result==ERR_OK)
//                    {
//                        num++;
//                        memcpy_r(outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
//                        outData->pResultData[4]=0x01;
//                        outData->pResultData[5]=DT_ARRAY;
//                        outData->pResultData[6]=num;
//                        outData->resultDataLen=*convert.dOffset+7;
//                    }
//                }
//                PRTL_FMT_LOGCC("num is %d\n",num);
//                if(num==0)
//                {
//                    memcpy_r(outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
//                    outData->pResultData[4]=0x01;
//                    outData->pResultData[5]=0x01;
//                    outData->pResultData[6]=0x00;
//                    outData->resultDataLen=7;
//                    result = DATA_SUCCESS;
//                }
//                else
//                {
//                    result = DATA_SUCCESS;
//                }
//                PRTL_FMT_LOGCC("outData->resultDataLen is %d\n",outData->resultDataLen);
//                PRTL_BUF_LOGCC(outData->pResultData,outData->resultDataLen,"事件方案");
//                
//
//            }
//            else
//            {
//                result=data_to_buf_event_scheme(pOdaInfo,&convert);
//            }
//        }
//        break;
//    }
//    return result;
//}

/**
*********************************************************************
* @name：      class11_invoke_get_event_scheme_rec
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_event_scheme_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    /* 类型不存在 */
    if (pOdaInfo->pTab->oad.nObjID != 0x6016)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOdaInfo->pTab->oad.attID)
    {
    case 2:
        {
            OOP_PRSD_T oopRsd;
            OOP_RCSD_T oopRcsd;
            uint16 headLen = 0;
            READ_NOR_BP_T inReadNor;
            RELY_INFO_T  relyInfo;
            RECORD_BUF_T outRecordBuf;

            /* 清0处理 */
            MEMZERO(&oopRsd, sizeof(oopRsd));
            MEMZERO(&oopRcsd, sizeof(oopRcsd));
            MEMZERO(&inReadNor, sizeof(inReadNor));
            MEMZERO(&relyInfo, sizeof(relyInfo));
            MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

            /*处理RSD*/
            nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
            PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
            PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
            PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
            PRTL_FMT_LOGCC("offset = %d\n", offset);
            if (nRet != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("RSD解析错误\n");

                /*应对电科院协议一致性测试*/
                uint16 testOffset = 4;
                uint16 inCsdOff = 0, inCsdLen = 0;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
                inCsdOff = testOffset;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
                inCsdLen = testOffset - inCsdOff;
                memcpy(outData->pResultData, inData, 4);
                memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
                outData->resultDataLen = 4 + inCsdLen;

                outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
                if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
                {
                    outData->pResultData[outData->resultDataLen++] = 0;

                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    for (i = 0; i < oopRcsd.nNum; i++)
                    {
                        outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                    }
                }

                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_SUCCESS;
            }
            else
            {
                inDataLenTmp -= offset;  //除去RSD和OAD后的长度
            }

            /*处理RCSD*/
            csdStart = offset;
            if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("CSD解析错误!\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_TYPE_UNMATCHED;
            }

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;

            if (csdLen == 1)
            {
                //如果rcsd为空，填充OAD60010200
                outData->pResultData[outOffset ++] = 0x01;
                outData->pResultData[outOffset ++] = 0x00;
                outData->pResultData[outOffset ++] = 0x60;
                outData->pResultData[outOffset ++] = 0x17;
                outData->pResultData[outOffset ++] = 0x02;
                outData->pResultData[outOffset ++] = 0x00;
            }
            else
            {
                memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
                outOffset += csdLen;
            }
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;
            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*记录型档案接口提供输入参数*/
            if (read_nor_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
            {
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /* 依赖信息中存下OAD列表的指针 */
            relyInfo.pOadInfo = pOdaInfo;

            PRTL_FMT_LOGCC("****************************************************\n");
            PRTL_FMT_LOGCC("inReadNor.startInfoNum = %d\n", inReadNor.startInfoNum);
            PRTL_FMT_LOGCC("inReadNor.endInfoNum = %d\n", inReadNor.endInfoNum);
            PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                      relyInfo.oopRcsd.cols[i].road.oadMain.value);
                    for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                    {
                        PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                                  relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                    }
                }
            }
            PRTL_FMT_LOGCC("****************************************************\n");

            /*读记录档案接口，可以读出apdu数据域，可返回断点信息*/
            if (record_event_scheme_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
            {
                nRet = DATA_READ_WRITE_DENIED;
            }
            else
            {
                memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
                outData->resultDataLen += outRecordBuf.bufLen;

                if (outRecordBuf.breakInfo.appSlicingFlag == 1)
                {
                    /*记下断点信息*/
                    PRTL_FMT_LOGCC("记下断点信息\n");
                    outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
                    memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
                }
                else
                {
                    /*清除断点信息*/
                    PRTL_FMT_LOGCC("清除断点信息\n");
                    memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                }
                
                nRet = DATA_SUCCESS;
            }
            
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_SUCCESS;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_report_scheme_rec
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_report_scheme_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    /* 类型不存在 */
    if (pOdaInfo->pTab->oad.nObjID != 0x601C)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOdaInfo->pTab->oad.attID)
    {
    case 2:
        {
            OOP_PRSD_T oopRsd;
            OOP_RCSD_T oopRcsd;
            uint16 headLen = 0;
            READ_NOR_BP_T inReadNor;
            RELY_INFO_T  relyInfo;
            RECORD_BUF_T outRecordBuf;

            /* 清0处理 */
            MEMZERO(&oopRsd, sizeof(oopRsd));
            MEMZERO(&oopRcsd, sizeof(oopRcsd));
            MEMZERO(&inReadNor, sizeof(inReadNor));
            MEMZERO(&relyInfo, sizeof(relyInfo));
            MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

            /*处理RSD*/
            nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
            PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
            PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
            PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
            PRTL_FMT_LOGCC("offset = %d\n", offset);
            if (nRet != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("RSD解析错误\n");

                /*应对电科院协议一致性测试*/
                uint16 testOffset = 4;
                uint16 inCsdOff = 0, inCsdLen = 0;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
                inCsdOff = testOffset;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
                inCsdLen = testOffset - inCsdOff;
                memcpy(outData->pResultData, inData, 4);
                memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
                outData->resultDataLen = 4 + inCsdLen;

                outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
                if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
                {
                    outData->pResultData[outData->resultDataLen++] = 0;

                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    for (i = 0; i < oopRcsd.nNum; i++)
                    {
                        outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                    }
                }

                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_SUCCESS;
            }
            else
            {
                inDataLenTmp -= offset;  //除去RSD和OAD后的长度
            }

            /*处理RCSD*/
            csdStart = offset;
            if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("CSD解析错误!\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_TYPE_UNMATCHED;
            }

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;

            if (csdLen == 1)
            {
                //如果rcsd为空，填充OAD60010200
                outData->pResultData[outOffset ++] = 0x01;
                outData->pResultData[outOffset ++] = 0x00;
                outData->pResultData[outOffset ++] = 0x60;
                outData->pResultData[outOffset ++] = 0x1D;
                outData->pResultData[outOffset ++] = 0x02;
                outData->pResultData[outOffset ++] = 0x00;
            }
            else
            {
                memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
                outOffset += csdLen;
            }
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;
            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*记录型档案接口提供输入参数*/
            if (read_nor_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
            {
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /* 依赖信息中存下OAD列表的指针 */
            relyInfo.pOadInfo = pOdaInfo;

            PRTL_FMT_LOGCC("****************************************************\n");
            PRTL_FMT_LOGCC("inReadNor.startInfoNum = %d\n", inReadNor.startInfoNum);
            PRTL_FMT_LOGCC("inReadNor.endInfoNum = %d\n", inReadNor.endInfoNum);
            PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                      relyInfo.oopRcsd.cols[i].road.oadMain.value);
                    for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                    {
                        PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                                  relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                    }
                }
            }
            PRTL_FMT_LOGCC("****************************************************\n");

            /*读记录档案接口，可以读出apdu数据域，可返回断点信息, 应用分帧长度判断需要优化修改，目前直接减30*/
            if (record_report_scheme_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - 3 - 30, &outRecordBuf))
            {
                nRet = DATA_READ_WRITE_DENIED;
            }
            else
            {
                memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
                outData->resultDataLen += outRecordBuf.bufLen;

                if (outRecordBuf.breakInfo.appSlicingFlag == 1)
                {
                    /*记下断点信息*/
                    PRTL_FMT_LOGCC("记下断点信息\n");
                    outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
                    memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
                }
                else
                {
                    /*清除断点信息*/
                    PRTL_FMT_LOGCC("清除断点信息\n");
                    memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                }
                
                nRet = DATA_SUCCESS;
            }
            
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_SUCCESS;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}


/**
*********************************************************************
* @name：      class11_invoke_get_oop_meter
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_event_scheme(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
    case 0x06:
        return class11_invoke_get_event_scheme_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_event_scheme_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：      class11_invoke_get_report_scheme
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_report_scheme(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
    case 0x06:
        return class11_invoke_get_report_scheme_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_report_scheme_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}


/**
*********************************************************************
* @name：       class11_invoke_get
* @brief：      class11的GET服务
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
OOP_DAR_E class11_invoke_get_transparent_scheme_nor(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E result = DATA_OTHER_REASON,ret;
    uint32 len=0;
    uint32 offset=0;
    uint16 i;
    
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    OOP_PLAN_TRANS_T PLAN_TRANS_T;
    NOMAL_OAD_T NormalOad ;
    DATA_CONVERT_T convert;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;
    
    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

     /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
       
    switch (pOdaInfo->pTab->oad.nObjID)
    {
        case 0x6018:
        {
            if(pOdaInfo->nIndex==0x00)
            {
                uint8 transparentschemenum = 0;
                uint32 ooptransparentschemeNumOffset = 0;
                memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;                             //结果
                outData->pResultData[offset ++] = DT_ARRAY;                         //类型
                ooptransparentschemeNumOffset = offset;                                         //任务数量偏移
                outData->pResultData[offset ++] = 0x00;                             //数量，先填0
                bufAllLen = offset;
                for(i=1;i<=255;i++)
                { 
                    if (slicing_is_comtinue(pApduInfo->bpBreakInfo, i))
                    {
                        continue;
                    }
                    NormalOad.logicId = 0;
                    NormalOad.infoNum = i;
                    NormalOad.oad.value=0x60180200;

                    memset((uint8*)&PLAN_TRANS_T,0x00,sizeof(OOP_PLAN_TRANS_T));
                    #if TRANSPARENT_PLAN_READ_MOD == 1
                    ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,sizeof(OOP_PLAN_TRANS_T), (uint8*)&PLAN_TRANS_T, &len);
                    if ((ret != ERR_OK) || (len != sizeof(PLAN_TRANS_T)))
                    #else
                    ret = app_get_plan_transparent_list(i, &PLAN_TRANS_T);
                    if (ret != ERR_OK)
                    #endif
                    {
                        continue;
                    }

                    convert.srcBuf = &PLAN_TRANS_T;
                    convert.srcLen = sizeof(OOP_PLAN_TRANS_T);
                    convert.sOffset = &sOffset;

                    convert.dstBuf = &outData->pResultData[offset];
                    convert.dstLen = outDataMax - offset;
                    convert.dOffset = &dOffset;

                    //记录断点信息
                    breakInfo.index = i;
                    
                    PRTL_FMT_LOGCC("class11_invoke_get_transparent_scheme :NormalOad.infoNum is %d\n",NormalOad.infoNum);
                    result=data_to_buf_transparent_scheme(pOdaInfo,&convert);
                    
                    if(result == ERR_OK)
                    {
                        bufAllLen += dOffset;
                        if (bufAllLen <= maxBufLen)
                        {
                            memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                            offset += dOffset;
                            outData->resultDataLen = offset;
                            PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个任务报文:", i);
                            transparentschemenum ++;
                            dOffset = 0;
                            sOffset = 0;
                        }
                        else
                        {
                            dOffset = 0;
                            sOffset = 0;
                            isBufFull = TRUE;
                            PRTL_FMT_LOGCC("class10_invoke_get_task_list_nor isBufFull == TRUE isBufFull = %d\n", isBufFull);
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

                PRTL_BUF_LOGCC(outData->pResultData, offset, "整个任务报文:");
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

                if (transparentschemenum > 0)
                {
                    outData->pResultData[ooptransparentschemeNumOffset] = transparentschemenum;   //重新记档案数量
                    outData->resultDataLen = offset;
                    result = DATA_SUCCESS;
                }
                else if(transparentschemenum == 0)
                {
                    memcpy_r(outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
                    outData->pResultData[4]=DT_ARRAY;
                    outData->pResultData[5]=0x01;
                    outData->pResultData[6]=0x00;
                    outData->resultDataLen=7;
                    result = DATA_SUCCESS;
                }
                else
                {
                    result = DATA_SUCCESS;
                }
            }
            else
            {  
                NormalOad.logicId = 0;
                NormalOad.infoNum = pOdaInfo->nIndex;
                NormalOad.oad.value=0x60180200;

                convert.dstBuf = &outData->pResultData[5];
                convert.dstLen = outDataMax - 5;
                convert.dOffset = &dOffset;

                memset((uint8*)&PLAN_TRANS_T,0x00,sizeof(PLAN_TRANS_T));
                #if TRANSPARENT_PLAN_READ_MOD == 1
                ret=db_read_nomal(pApduInfo->hUdp, &NormalOad,sizeof(PLAN_TRANS_T), (uint8*)&PLAN_TRANS_T, &len);
                if ((ret != ERR_OK) || (len != sizeof(PLAN_TRANS_T)))
                #else
                ret = app_get_plan_transparent_list(i, &PLAN_TRANS_T);
                if (ret != ERR_OK)
                #endif
                {
                   return DATA_OBJECT_UNAVAILABLE;
                }

                convert.srcBuf = &PLAN_TRANS_T;
                convert.srcLen = sizeof(OOP_PLAN_TRANS_T);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;
                
                result=data_to_buf_transparent_scheme(pOdaInfo,&convert);
                if(result == ERR_OK)
                {
                    
                    memcpy(outData->pResultData,inData,4);
                    outData->pResultData[4]=0x01;
                    outData->resultDataLen=dOffset+5;
                }
                else
                {
                    memcpy(outData->pResultData,inData,4);
                    outData->pResultData[4]=0x00;
                    outData->pResultData[5]=DATA_OBJECT_UNAVAILABLE;
                    outData->resultDataLen=6;
                }
            }
        }
        break;
    }
    return result;
}

/**
*********************************************************************
* @name：       class11_invoke_get
* @brief：      class11的GET服务
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
OOP_DAR_E class11_invoke_get_transparent_result(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint16 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;
    READ_RECORD_T inReadRecord;
    RELY_INFO_T   relyInfo;
    uint32        readRecNum;
    uint8 mn;

    /* 类型不存在 */
    if (pOadInfo->pTab->oad.nObjID != 0x601A)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOadInfo->pTab->oad.attID)
    {
    case 2:
        {
            CSD_T *pCSD = NULL, *pCSDHead = NULL;
            RSD_T *pRSD = (RSD_T *)malloc(sizeof(RSD_T));
            pRSD->StartData = NULL;
            pRSD->EndData = NULL;
            MS_T *ms = NULL;
            uint16 headLen = 0;
            RECORD_BUF_T outRecordBuf = {0};

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.sendFrameLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.sendFrameLen);
            outRecordBuf.tmEndSeg = 0;

            /*处理RSD*/
            memset(pRSD, 0, sizeof(RSD_T)); //清0
            offset = rsd_get(pApduInfo->hUdp, (uint8 *)(inData + offset), inDataLenTmp - 4, pRSD);
            if (offset == 0)
            {
                PRTL_FMT_TRACE("RSD解析错误\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                if (pRSD->StartData != NULL)
                {
                    free(pRSD->StartData);
                }
                if (pRSD->EndData!= NULL)
                {
                    free(pRSD->EndData);
                }
                if (pRSD->Ms != NULL)
                {
                    ms_destroy(pRSD->Ms);
                }
                free(pRSD);
                pRSD = NULL;

                /*应对电科院协议一致性测试*/
                uint16 testOffset = 4;
                uint16 inCsdOff = 0, inCsdLen = 0;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
                inCsdOff = testOffset;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
                inCsdLen = testOffset - inCsdOff;
                memcpy(outData->pResultData, inData, 4);
                memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
                outData->resultDataLen = 4 + inCsdLen;

                outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
                pCSDHead = rcsd_get((uint8 *)inData, InDataLen, &offset, FALSE);
                if (!pCSDHead || !(pCSDHead->next))
                {
                    outData->pResultData[outData->resultDataLen++] = 0;
                    rcsd_destroy(pCSDHead);
                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    pCSD = pCSDHead->next;
                    while(pCSD)
                    {
                        outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                        pCSD = pCSD->next;
                    }
                }

                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_SUCCESS;
            }
            else
            {
                offset += 4;             //加上原本OAD长度
                inDataLenTmp -= offset;  //除去RSD和OAD后的长度
                ms = pRSD->Ms;
            }

            if (pRSD != NULL)
            {
                if (pRSD->selId == 0)
                {
                    //return enumDarDataTypeErr;
                    //如果是方法10，就回所有表的上一次日冻结电能
                    uint16 mmoffset = 0;
                    uint8 msssbuff[10] = {0};
                    msssbuff[0] = 1;
                    pRSD->selId = 10;
                    pRSD->LastN = 1;
                    pRSD->Ms = mtr_list_get(pApduInfo->hUdp, msssbuff, &mmoffset, FALSE);
                    if (!pRSD->Ms)
                    {
                        free(pRSD);
                        free(outRecordBuf.recordBuf);
                        outRecordBuf.recordBuf = NULL;
                        return DATA_TYPE_UNMATCHED;
                    }
                    ms = pRSD->Ms;
                }
            }

            /*处理RCSD*/
            csdStart = offset;
            for(mn=0;mn<255;mn++)
            {
            pCSDHead = transparent_rcsd_get(mn);


            pCSD = pCSDHead->next;      //头指针没用实际的RCSD

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;
            memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
            outOffset += csdLen;
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;
            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*给读任务记录表接口提供输入参数*/
            pCSD = pCSDHead->next;
            if (read_record_transparent_param_get(pRSD, pCSD, pApduInfo, &inReadRecord, &relyInfo, &readRecNum) != ERR_OK)
            {
                rcsd_destroy(pCSDHead);
                if ((pRSD->selId >= 4) && (pRSD->selId <= 8))
                {
                    free(pRSD->StartData);
                    free(pRSD->EndData);
                }
                ms_destroy(pRSD->Ms);
                free(pRSD);
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }
            
            PRTL_FMT_LOGCC("**************打印任务记录表接口提供输入参数*****************\n");
            char strT[128];
            switch(inReadRecord.cType)
            {
                case COL_TM_START: {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_TM_START");     break;}
                case COL_TM_END:   {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_TM_END");       break;}
                case COL_TM_STORE: {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_TM_STORE");     break;}
                case COL_NO_STORE: {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_NO_STORE");     break;}
                case COL_NO_INFO:  {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_NO_INFO");      break;}
                case COL_NOT:      {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_NOT");          break;}
                default:           {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "NO CHOOSE_TYPE_E"); break;}
            }
            strftime(strT, sizeof(strT), "%Y-%m-%d %H:%M:%S", localtime((time_t*)&inReadRecord.cStart));
            PRTL_FMT_LOGCC("inReadRecord.cStart:%s time[s]:%d\n",strT, inReadRecord.cStart);
            strftime(strT, sizeof(strT), "%Y-%m-%d %H:%M:%S", localtime((time_t*)&inReadRecord.cEnd));
            PRTL_FMT_LOGCC("inReadRecord.cEnd:%s time[s]:%d\n",strT, inReadRecord.cEnd);
            PRTL_FMT_LOGCC("inReadRecord.ti.unit = %d\n", inReadRecord.ti.unit);
            PRTL_FMT_LOGCC("inReadRecord.ti.value = %d\n", inReadRecord.ti.value);
            PRTL_FMT_LOGCC("readRecNum = %d\n", readRecNum);
            PRTL_BUF_LOGCC(&inReadRecord.MAC, 6, "inReadRecord.MAC");
            PRTL_FMT_LOGCC("inReadRecord.recordOAD.logicId = %d\n", inReadRecord.recordOAD.logicId);
            PRTL_FMT_LOGCC("inReadRecord.recordOAD.infoNum = %d\n", inReadRecord.recordOAD.infoNum);
            PRTL_FMT_LOGCC("inReadRecord.recordOAD.oadMain = 0x%08x\n", inReadRecord.recordOAD.road.oadMain.value);
            PRTL_FMT_LOGCC("inReadRecord.recordOAD.road.oadCols.nNum = %d\n", inReadRecord.recordOAD.road.oadCols.nNum);
            for(i = 0; i < inReadRecord.recordOAD.road.oadCols.nNum; i++)
            {
                PRTL_FMT_LOGCC("inReadRecord.recordOAD.road.oad[%d] = 0x%08x\n", i, inReadRecord.recordOAD.road.oadCols.oad[i]);
            }
            
            PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", relyInfo.oopRcsd.cols[i].choice, i,  
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", relyInfo.oopRcsd.cols[i].choice, i,
                                                                                                      relyInfo.oopRcsd.cols[i].road.oadMain.value);
                    for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                    {
                        PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", relyInfo.oopRcsd.cols[i].choice, i, j, 
                                                                                                                  relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                    }
                }
            }
            ms = relyInfo.ms;
            while (ms)
            {
                PRTL_BUF_LOGCC(&ms->oopMeter.basic.tsa.add[0], 6, "ms->oopMeter.basic.tsa.add[0]:");
                PRTL_FMT_LOGCC("ms->MtrNo = %d\n", ms->MtrNo);
                PRTL_FMT_LOGCC("ms->next = %p\n", ms->next);
                ms = ms->next;
            }
            PRTL_FMT_LOGCC("**************打印任务记录表接口提供输入参数结束******************\n");

            /*读任务记录表接口，可以读出apdu数据域，可返回断点信息*/
            if (record_transparent_task_table_read(pApduInfo->hUdp, inReadRecord, relyInfo, readRecNum, pApduInfo->connectionInfo.sendFrameLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
            {
                /*清除断点信息*/
                PRTL_FMT_LOGCC("清除断点信息\n");
                memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                nRet = DATA_READ_WRITE_DENIED;
            }
            else
            {
                memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
                outData->resultDataLen += outRecordBuf.bufLen;

                if (outRecordBuf.breakInfo.appSlicingFlag == 1)
                {
                    /*记下断点信息*/
                    PRTL_FMT_LOGCC("记下断点信息\n");
                    outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，60120300属于记录对象
                    memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
                }
                else
                {
                    /*清除断点信息*/
                    PRTL_FMT_LOGCC("清除断点信息\n");
                    memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                }
                
                nRet = DATA_SUCCESS;
            }
        }
            rcsd_destroy(pCSDHead);
            if ((pRSD->selId >= 4) && (pRSD->selId <= 8))
            {
                free(pRSD->StartData);
                free(pRSD->EndData);
            }
            ms_destroy(pRSD->Ms);
            free(pRSD);
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}


/**
*********************************************************************
* @name：      class11_invoke_get_acq_monitor_normal
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_acq_monitor_normal(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint16    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOdaInfo->pTab->oad.value)
    {
        case 0x60340200:
        {
            DATA_CONVERT_T convert;
            OOP_ACQ_MONITOR_T acqMonitor = {0};                                   //每个任务监控信息                                                
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopMonitorNum = 0;
            uint32 oopMeterNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));

            #if ACQ_MONITOR_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOdaInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取所有采集任务状态\n");

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            oopMeterNumOffset = offset;                                         //档案数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;

            for (i = 1; i <= 255; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                if (slicing_is_comtinue_acq_monitor(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                #if ACQ_MONITOR_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(pApduInfo->hUdp, &nomalOad, sizeof(OOP_ACQ_MONITOR_T), (uint8*)&acqMonitor, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_ACQ_MONITOR_T)))
                #else
                //ret = app_get_acq_monitor_list(i, &acqMonitor);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("class11_invoke_get_acq_monitor_normal nomalOad.infoNum = %d\n", nomalOad.infoNum);
                PRTL_FMT_LOGCC("class11_invoke_get_acq_monitor_normal acqMonitor.id = %d\n", acqMonitor.id);
                PRTL_FMT_LOGCC("class11_invoke_get_acq_monitor_normal acqMonitor.taskState = %d\n", acqMonitor.taskState);
                PRTL_FMT_LOGCC("class11_invoke_get_acq_monitor_normal acqMonitor.startTime.year = %d\n", acqMonitor.startTime.year);
                PRTL_FMT_LOGCC("class11_invoke_get_acq_monitor_normal acqMonitor.endTime.year = %d\n", acqMonitor.endTime.year);
                PRTL_FMT_LOGCC("class11_invoke_get_acq_monitor_normal acqMonitor.sum = %d\n", acqMonitor.sum);
                PRTL_FMT_LOGCC("class11_invoke_get_acq_monitor_normal acqMonitor.success = %d\n", acqMonitor.success);
                PRTL_FMT_LOGCC("class11_invoke_get_acq_monitor_normal acqMonitor.send = %d\n", acqMonitor.send);
                PRTL_FMT_LOGCC("class11_invoke_get_acq_monitor_normal acqMonitor.receive = %d\n", acqMonitor.receive);
                PRTL_FMT_LOGCC("class11_invoke_get_acq_monitor_normal acqMonitor.bValid = %d\n", acqMonitor.bValid);

                PRTL_FMT_LOGCC("找到采集任务状态 序号为%d\n", acqMonitor.id);

                //转换成报文数据
                convert.srcBuf = &acqMonitor;
                convert.srcLen = sizeof(acqMonitor);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_class8_acq_monitor(pOdaInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个采集任务状态报文:", i);
                        oopMonitorNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("class11_invoke_get_acq_monitor_normal isBufFull == TRUE isBufFull = %d\n", isBufFull);
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

            PRTL_BUF_LOGCC(outData->pResultData, offset, "整个采集任务状态报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

            if (oopMonitorNum > 0)
            {
                result = DATA_SUCCESS;
                outData->pResultData[oopMeterNumOffset] = oopMonitorNum;   //重新记档案数量
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
* @name：      class11_invoke_get_acq_monitor_normal
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_acq_state_normal(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
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

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOdaInfo->pTab->oad.value)
    {
        case 0x60320200:
        {
            DATA_CONVERT_T convert;
            OOP_ACQ_RLUESTATE_T acqstate;                                        //每个电表的档案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopMeterNum = 0;
            uint32 oopMeterNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));
            
            MEMZERO(&acqstate, sizeof(acqstate));

        
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOdaInfo->pTab->oad.value;
            

            PRTL_FMT_LOGCC("获取所有档案\n");

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            oopMeterNumOffset = offset;                                         //档案数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;
            for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                if (slicing_is_comtinue_oop_meter(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                nomalOad.infoNum = i;
                ret = db_read_nomal(pApduInfo->hUdp, &nomalOad, sizeof(OOP_ACQ_RLUESTATE_T), (uint8*)&acqstate, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_ACQ_RLUESTATE_T)))
           
                {
                    continue;
                }

                //转换成报文数据
                convert.srcBuf = &acqstate;
                convert.srcLen = sizeof(acqstate);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_acq_state(pOdaInfo, &convert);  //入参
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

            PRTL_BUF_LOGCC(outData->pResultData, offset, "整个档案报文:");
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
* @name：      class11_invoke_get_acq_rule_normal
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_acq_rule_normal(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint16    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOdaInfo->pTab->oad.value)
    {
        case 0x60340200:
        {
            DATA_CONVERT_T convert;
            OOP_ACQRULE_T acqRule;                                   //每个任务监控信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopRuleNum = 0;
            uint32 oopMeterNumOffset = 0;

            MEMZERO(&acqRule, sizeof(acqRule));
            MEMZERO(&convert, sizeof(convert));

            #if ACQ_MONITOR_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOdaInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取所有采集任务状态\n");

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            oopMeterNumOffset = offset;                                         //档案数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;

            for (i = 1; i <= 255; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                if (slicing_is_comtinue_acq_rule(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                #if ACQ_MONITOR_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(pApduInfo->hUdp, &nomalOad, sizeof(OOP_ACQRULE_T), (uint8*)&acqRule, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_ACQRULE_T)))
                #else
                //ret = app_get_acq_monitor_list(i, &acqRule);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到采集规则 序号为%d\n", i);

                //转换成报文数据
                convert.srcBuf = &acqRule;
                convert.srcLen = sizeof(acqRule);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_acq_rule(pOdaInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个采集任务状态报文:", i);
                        oopRuleNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("class11_invoke_get_acq_monitor_normal isBufFull == TRUE isBufFull = %d\n", isBufFull);
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

            PRTL_BUF_LOGCC(outData->pResultData, offset, "整个采集规则报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

            if (oopRuleNum > 0)
            {
                result = DATA_SUCCESS;
                outData->pResultData[oopMeterNumOffset] = oopRuleNum;   //重新记档案数量
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
* @name：      class11_invoke_get_acq_monitor_record
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_acq_monitor_record(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    /* 类型不存在 */
    if (pOdaInfo->pTab->oad.nObjID != 0x6034)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOdaInfo->pTab->oad.attID)
    {
    case 2:
        {
            OOP_PRSD_T oopRsd;
            OOP_RCSD_T oopRcsd;
            uint16 headLen = 0;
            READ_NOR_BP_T inReadAcqMonitor;
            RELY_INFO_T  relyInfo;
            RECORD_BUF_T outRecordBuf;

            /* 清0处理 */
            MEMZERO(&oopRsd, sizeof(oopRsd));
            MEMZERO(&oopRcsd, sizeof(oopRcsd));
            MEMZERO(&inReadAcqMonitor, sizeof(inReadAcqMonitor));
            MEMZERO(&relyInfo, sizeof(relyInfo));
            MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

            /*处理RSD*/
            nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
            PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
            PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
            PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
            PRTL_FMT_LOGCC("offset = %d\n", offset);
            if (nRet != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("RSD解析错误\n");

                /*应对电科院协议一致性测试*/
                uint16 testOffset = 4;
                uint16 inCsdOff = 0, inCsdLen = 0;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
                inCsdOff = testOffset;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
                inCsdLen = testOffset - inCsdOff;
                memcpy(outData->pResultData, inData, 4);
                memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
                outData->resultDataLen = 4 + inCsdLen;

                outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
                if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
                {
                    outData->pResultData[outData->resultDataLen++] = 0;

                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    for (i = 0; i < oopRcsd.nNum; i++)
                    {
                        outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                    }
                }

                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_SUCCESS;
            }
            else
            {
                inDataLenTmp -= offset;  //除去RSD和OAD后的长度
            }

            /*处理RCSD*/
            csdStart = offset;
            if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("CSD解析错误!\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_TYPE_UNMATCHED;
            }

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;

            if (csdLen == 1)
            {
                //如果rcsd为空，填充OAD60350200
                outData->pResultData[outOffset ++] = 0x01;
                outData->pResultData[outOffset ++] = 0x00;
                outData->pResultData[outOffset ++] = 0x60;
                outData->pResultData[outOffset ++] = 0x35;
                outData->pResultData[outOffset ++] = 0x02;
                outData->pResultData[outOffset ++] = 0x00;
            }
            else
            {
                memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
                outOffset += csdLen;
            }
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;

            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*记录型档案接口提供输入参数*/
            if (read_acq_monitor_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadAcqMonitor, &relyInfo) != ERR_OK)
            {
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /* 依赖信息中存下OAD列表的指针 */
            relyInfo.pOadInfo = pOdaInfo;

            PRTL_FMT_LOGCC("****************************************************\n");
            PRTL_FMT_LOGCC("inReadAcqMonitor.startInfoNum = %d\n", inReadAcqMonitor.startInfoNum);
            PRTL_FMT_LOGCC("inReadAcqMonitor.endInfoNum = %d\n", inReadAcqMonitor.endInfoNum);
            PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                      relyInfo.oopRcsd.cols[i].road.oadMain.value);
                    for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                    {
                        PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                                  relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                    }
                }
            }
            PRTL_FMT_LOGCC("****************************************************\n");

            /*读记录任务监控接口，可以读出apdu数据域，可返回断点信息*/
            if (record_acq_monitor_read(pApduInfo->hUdp, inReadAcqMonitor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
            {
                nRet = DATA_READ_WRITE_DENIED;
            }
            else
            {
                memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
                outData->resultDataLen += outRecordBuf.bufLen;

                if (outRecordBuf.breakInfo.appSlicingFlag == 1)
                {
                    /*记下断点信息*/
                    PRTL_FMT_LOGCC("记下断点信息\n");
                    outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
                    memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
                }
                else
                {
                    /*清除断点信息*/
                    PRTL_FMT_LOGCC("清除断点信息\n");
                    memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                }
                
                nRet = DATA_SUCCESS;
            }
            
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_SUCCESS;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_acq_monitor_record
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_acq_state_record(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    /* 类型不存在 */
    if (pOdaInfo->pTab->oad.nObjID != 0x6032)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOdaInfo->pTab->oad.attID)
    {
    case 2:
        {
            OOP_PRSD_T oopRsd;
            OOP_RCSD_T oopRcsd;
            uint16 headLen = 0;
            READ_NOR_BP_T inReadNor;
            RELY_INFO_T  relyInfo;
            RECORD_BUF_T outRecordBuf;

            /* 清0处理 */
            MEMZERO(&oopRsd, sizeof(oopRsd));
            MEMZERO(&oopRcsd, sizeof(oopRcsd));
            MEMZERO(&inReadNor, sizeof(inReadNor));
            MEMZERO(&relyInfo, sizeof(relyInfo));
            MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

            /*处理RSD*/
            nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
            PRTL_FMT_DEBUG("筛选方法[%d]\n", oopRsd.choice);
            switch (oopRsd.choice)
            {
            case 1:
                {
                    PRTL_FMT_DEBUG("方法1 OAD = 0x%08x\n", oopRsd.sel1.oad.value);
                    PRTL_FMT_DEBUG("方法1 type = %d\n", oopRsd.sel1.value.type);
                    PRTL_FMT_DEBUG("方法1 value = 0x%04x\n", oopRsd.sel1.value.oi);
                }
                break;
            default:
                break;
            }

            if (nRet != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("RSD解析错误\n");

                /*应对电科院协议一致性测试*/
                uint16 testOffset = 4;
                uint16 inCsdOff = 0, inCsdLen = 0;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
                inCsdOff = testOffset;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
                inCsdLen = testOffset - inCsdOff;
                memcpy(outData->pResultData, inData, 4);
                memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
                outData->resultDataLen = 4 + inCsdLen;

                outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
                if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
                {
                    outData->pResultData[outData->resultDataLen++] = 0;

                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    for (i = 0; i < oopRcsd.nNum; i++)
                    {
                        outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                    }
                }

                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_SUCCESS;
            }
            else
            {
                inDataLenTmp -= offset;  //除去RSD和OAD后的长度
            }

            /*处理RCSD*/
            csdStart = offset;
            if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("CSD解析错误!\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_TYPE_UNMATCHED;
            }

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;

            if (csdLen == 1)
            {
                //如果rcsd为空，填充OAD60010200
                outData->pResultData[outOffset ++] = 0x01;
                outData->pResultData[outOffset ++] = 0x00;
                outData->pResultData[outOffset ++] = 0x60;
                outData->pResultData[outOffset ++] = 0x33;
                outData->pResultData[outOffset ++] = 0x02;
                outData->pResultData[outOffset ++] = 0x00;
            }
            else
            {
                memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
                outOffset += csdLen;
            }
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;
            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*记录型档案接口提供输入参数*/
            if (read_nor_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
            {
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /* 依赖信息中存下OAD列表的指针 */
            relyInfo.pOadInfo = pOdaInfo;

            PRTL_FMT_DEBUG("******************转换内部帅选参数开始********************\n");
            PRTL_FMT_LOGCC("筛选起始[%d]\n", inReadNor.startInfoNum);
            PRTL_FMT_LOGCC("筛选结束[%d]\n", inReadNor.endInfoNum);
            PRTL_FMT_LOGCC("筛选数据数量[%d]\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                      relyInfo.oopRcsd.cols[i].road.oadMain.value);
                    for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                    {
                        PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                                  relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                    }
                }
            }
            PRTL_FMT_LOGCC("****************************************************\n");

            /*读记录档案接口，可以读出apdu数据域，可返回断点信息*/
            if (record_meter_acq_state(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
            {
                nRet = DATA_READ_WRITE_DENIED;
            }
            else
            {
                memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
                outData->resultDataLen += outRecordBuf.bufLen;

                if (outRecordBuf.breakInfo.appSlicingFlag == 1)
                {
                    /*记下断点信息*/
                    PRTL_FMT_LOGCC("记下断点信息\n");
                    outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
                    memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
                }
                else
                {
                    /*清除断点信息*/
                    PRTL_FMT_LOGCC("清除断点信息\n");
                    memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                }
                
                nRet = DATA_SUCCESS;
            }
            
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_SUCCESS;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}



/**
*********************************************************************
* @name：      class11_invoke_get_acq_monitor_record
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_acq_rule_record(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    /* 类型不存在 */
    if (pOdaInfo->pTab->oad.nObjID != 0x6034)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOdaInfo->pTab->oad.attID)
    {
    case 2:
        {
            OOP_PRSD_T oopRsd;
            OOP_RCSD_T oopRcsd;
            uint16 headLen = 0;
            READ_NOR_BP_T readNorBp;
            RELY_INFO_T  relyInfo;
            RECORD_BUF_T outRecordBuf;

            /* 清0处理 */
            MEMZERO(&oopRsd, sizeof(oopRsd));
            MEMZERO(&oopRcsd, sizeof(oopRcsd));
            MEMZERO(&readNorBp, sizeof(readNorBp));
            MEMZERO(&relyInfo, sizeof(relyInfo));
            MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

            /*处理RSD*/
            nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
            PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
            PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
            PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
            PRTL_FMT_LOGCC("offset = %d\n", offset);
            if (nRet != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("RSD解析错误\n");

                /*应对电科院协议一致性测试*/
                uint16 testOffset = 4;
                uint16 inCsdOff = 0, inCsdLen = 0;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
                inCsdOff = testOffset;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
                inCsdLen = testOffset - inCsdOff;
                memcpy(outData->pResultData, inData, 4);
                memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
                outData->resultDataLen = 4 + inCsdLen;

                outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
                if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
                {
                    outData->pResultData[outData->resultDataLen++] = 0;

                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    for (i = 0; i < oopRcsd.nNum; i++)
                    {
                        outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                    }
                }

                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_SUCCESS;
            }
            else
            {
                inDataLenTmp -= offset;  //除去RSD和OAD后的长度
            }

            /*处理RCSD*/
            csdStart = offset;
            if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("CSD解析错误!\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_TYPE_UNMATCHED;
            }

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;
            memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
            outOffset += csdLen;
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;
            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*记录型档案接口提供输入参数*/
            if (read_acq_rule_param_get(&oopRsd, &oopRcsd, pApduInfo, &readNorBp, &relyInfo) != ERR_OK)
            {
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /* 依赖信息中存下OAD列表的指针 */
            relyInfo.pOadInfo = pOdaInfo;

            PRTL_FMT_LOGCC("****************************************************\n");
            PRTL_FMT_LOGCC("readNorBp.startInfoNum = %d\n", readNorBp.startInfoNum);
            PRTL_FMT_LOGCC("readNorBp.endInfoNum = %d\n", readNorBp.endInfoNum);
            PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                      relyInfo.oopRcsd.cols[i].road.oadMain.value);
                    for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                    {
                        PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                                  relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                    }
                }
            }
            PRTL_FMT_LOGCC("****************************************************\n");

            /*读记录任务监控接口，可以读出apdu数据域，可返回断点信息*/
            if (record_acq_rule_read(pApduInfo->hUdp, readNorBp, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
            {
                nRet = DATA_READ_WRITE_DENIED;
            }
            else
            {
                memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
                outData->resultDataLen += outRecordBuf.bufLen;

                if (outRecordBuf.breakInfo.appSlicingFlag == 1)
                {
                    /*记下断点信息*/
                    PRTL_FMT_LOGCC("记下断点信息\n");
                    outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
                    memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
                }
                else
                {
                    /*清除断点信息*/
                    PRTL_FMT_LOGCC("清除断点信息\n");
                    memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                }
                
                nRet = DATA_SUCCESS;
            }
            
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_SUCCESS;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_acq_monitor
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_acq_monitor(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x06:
        return class11_invoke_get_acq_monitor_normal(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
        return class11_invoke_get_acq_monitor_record(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：      class11_invoke_get_acq_monitor
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_acq_state(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x06:
        return class11_invoke_get_acq_state_normal(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
        return class11_invoke_get_acq_state_record(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：      class11_invoke_get_acq_monitor
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_acq_rule(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x06:
        return class11_invoke_get_acq_rule_normal(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
        return class11_invoke_get_acq_rule_record(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：      wait_oop_meter_data_update
* @brief：     等待数据更新完成
* @param[in] ：pOdaInfo        OAD信息
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E wait_oop_meter_data_update(uint32 infoNum)
{
    int32 i = 0;

    while (0 != is_meter_update_ok(infoNum))
    {
        if(i++ > 500)
        {
            PRTL_FMT_DEBUG("meter data update err\n");
            return DATA_READ_WRITE_DENIED;
        }
        usleep(10000);
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      class11_invoke_act_oop_meter
* @brief：     class11的ACTION服务
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
OOP_DAR_E class11_invoke_act_oop_meter(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint32 offset = 0;
    int result = DATA_OTHER_REASON;
    int ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    NOMAL_OAD_T nomalOad = {0};
    OOP_METER_T oopMeter;
    OOP_METER_T oldOopMeter;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 len = 0;
    int meterret = ERR_OK;

    /*类型不存在*/
    if ((pOdaInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    MEMZERO(&oopMeter, sizeof(OOP_METER_T));
    
    convert.srcBuf = (void*)inData;
    convert.srcLen = inLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &oopMeter;
    convert.dstLen = sizeof(oopMeter);
    convert.dOffset = &dOffset;  

    PRTL_FMT_LOGCC("pOdaInfo->pTab->oad.value is %x\n",pOdaInfo->pTab->oad.value);
    switch (pOdaInfo->pTab->oad.value)
    {
        case 0x60007F00:
        {
            /*下发一个档案单元*/
            PRTL_FMT_LOGCC("class11_invoke_act 下发一个档案单元\n");

            nRet = buf_to_data_oop_meter(pOdaInfo, &convert);

            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            //检测档案数据是否有效
            if ((oopMeter.basic.baud > 19 && oopMeter.basic.baud != 255) ||
                (oopMeter.basic.pwrType > 3))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            MEMZERO(&oldOopMeter, sizeof(OOP_METER_T));
            
            nomalOad.logicId = 0x00;
            nomalOad.infoNum = oopMeter.nIndex;
            nomalOad.oad.value = 0x60000200;
            nomalOad.classtag = CLASS_DATA_INIT;

            //先读出原电表信息
            meterret = app_get_meter_list(nomalOad.infoNum, &oldOopMeter);
            if(meterret != ERR_OK)
            {
                PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), outLen\n",
                    meterret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value, sizeof(OOP_METER_T), len);
            }

            //置档案数据更新标志
            app_set_meter_update_start(nomalOad.infoNum);
            
            ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&oopMeter, sizeof(OOP_METER_T));
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("class11_invoke_act_oop_meter db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                return DATA_READ_WRITE_DENIED;
            }
            appctrl_db_chg_event(0, 21, oopMeter.nIndex);  //档案更新

            //等待数据更新完成
            if (wait_oop_meter_data_update(nomalOad.infoNum) != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("app_set_meter_invalid err\n");
                return DATA_READ_WRITE_DENIED;
            }

            //删档案，或通信地址，端口，规约类型，用户类型发生变化，则清电表数据
            if(meterret == ERR_OK)
            {
                ret = meter_data_if_clear(pApduInfo, &oldOopMeter, &oopMeter);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                    return DATA_READ_WRITE_DENIED;
                }
            }
            
            result = DATA_SUCCESS;                                             //填写成功标识
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008000:
        {
            /*下发多个档案单元*/
            uint32      oopMeterNum;
            OOP_METER_T meterList[100];
            memset(meterList,0,sizeof(meterList));

            PRTL_FMT_LOGCC("class11_invoke_act 下发多个档案单元\n");

            if(inData[sOffset ++] != DT_ARRAY)
            {
                return ERR_OPTION;
            }
            oopMeterNum = inData[sOffset++];
            PRTL_FMT_DEBUG("oopMeterNum is %d\n",oopMeterNum);
            for(i = 0; i < oopMeterNum; i++)
            {
                nRet = buf_to_data_oop_meter(pOdaInfo, &convert);
                if (nRet != DATA_SUCCESS)
                {
                    PRTL_FMT_LOGCC("buf_to_data_oop_meter err = %d\n", ret);
                    return DATA_READ_WRITE_DENIED;
                }
                else
                {
                    dOffset = 0;
                }

                //检测档案数据是否有效
                if ((oopMeter.basic.baud > 19 && oopMeter.basic.baud != 255) ||
                    (oopMeter.basic.pwrType > 3))
                {
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }

                //PRTL_FMT_LOGCC("oopMeter.nIndex = %d\n", oopMeter.nIndex);
                //PRTL_FMT_LOGCC("oopMeter.basic.protocol = %d\n", oopMeter.basic.protocol);
                //PRTL_BUF_LOGCC(oopMeter.basic.tsa.add, oopMeter.basic.tsa.len + 1, "地址:");
                //台体抄表参数项目 下完立刻就会读 所以这边判下个数 超过20个才缓存
                if(oopMeterNum >=20)
                {
                    if(i>=100)
                        continue;
                   // PRTL_FMT_DEBUG("PN is %d\n",oopMeter.nIndex);
                    memcpy(&meterList[i],&oopMeter,sizeof(oopMeter));
                }else
                {
                    MEMZERO(&oldOopMeter, sizeof(OOP_METER_T));

                    nomalOad.logicId = 0x00;
                    nomalOad.infoNum = oopMeter.nIndex;
                    nomalOad.oad.value = 0x60000200;
                    nomalOad.classtag = CLASS_DATA_INIT;
                    
                    //先读出原电表信息
                    meterret = app_get_meter_list(nomalOad.infoNum, &oldOopMeter);
                    if(meterret != ERR_OK)
                    {
                        PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                            meterret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                    }

                    app_set_meter_update_start(nomalOad.infoNum);

                    ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&oopMeter, sizeof(OOP_METER_T));
                    if (ret != ERR_OK)
                    {
                        PRTL_FMT_DEBUG("class11_invoke_act_oop_meter db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                            ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                        return DATA_READ_WRITE_DENIED;
                    }

                    //等待数据更新完成
                    if (wait_oop_meter_data_update(nomalOad.infoNum) != DATA_SUCCESS)
                    {
                        PRTL_FMT_DEBUG("app_set_meter_invalid err\n");
                        return DATA_READ_WRITE_DENIED;
                    }

                    //删档案，或通信地址，端口，规约类型，用户类型发生变化，则清电表数据
                    if(meterret == ERR_OK)
                    {
                        ret = meter_data_if_clear(pApduInfo, &oldOopMeter, &oopMeter);
                        if(ret != ERR_OK)
                        {
                            PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                            return DATA_READ_WRITE_DENIED;
                        }
                    }
                }
            }
            if(oopMeterNum >=20)
                cache_meter_add(meterList,MIN(oopMeterNum,100));

            result = DATA_SUCCESS;                                             //填写成功标识

            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008100:
        {
            OOPM_METER_129_T data;

            MEMZERO(&data, sizeof(data));

            convert.srcBuf = (void*)inData;
            convert.srcLen = inLen;
            convert.sOffset = &sOffset;

            convert.dstBuf = &data;
            convert.dstLen = sizeof(data);
            convert.dOffset = &dOffset; 

            /*更新档案单元（129方法）*/
            PRTL_FMT_LOGCC("class11_invoke_act 更新一个档案单元（方法129）\n");
            nRet = buf_to_data_oop_meter_basic_object(pOdaInfo, &convert);
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            MEMZERO(&oldOopMeter, sizeof(OOP_METER_T));
            
            nomalOad.logicId = 0x00;
            nomalOad.infoNum = data.index;
            nomalOad.oad.value = 0x60000200;
            nomalOad.classtag = CLASS_DATA_INIT;

            if (data.index > NO_VIR_MTR_MAX_NUM)
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            //先读出原电表信息
            meterret = app_get_meter_list(nomalOad.infoNum, &oldOopMeter);
            if(meterret != ERR_OK)
            {
                PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), outLen\n",
                    meterret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value, sizeof(OOP_METER_T), len);
            }

            oopMeter = oldOopMeter;
            oopMeter.basic = data.basic;
            
            app_set_meter_update_start(nomalOad.infoNum);

            ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&oopMeter, sizeof(OOP_METER_T));
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("class11_invoke_act_oop_meter db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                return DATA_READ_WRITE_DENIED;
            }
            appctrl_db_chg_event(0, 21, oopMeter.nIndex);  //档案更新

            //等待数据更新完成
            if (wait_oop_meter_data_update(nomalOad.infoNum) != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("app_set_meter_invalid err\n");
                return DATA_READ_WRITE_DENIED;
            }

            //删档案，或通信地址，端口，规约类型，用户类型发生变化，则清电表数据
            if(meterret == ERR_OK)
            {
                ret = meter_data_if_clear(pApduInfo, &oldOopMeter, &oopMeter);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                    return DATA_READ_WRITE_DENIED;
                }
            }
            
            result = DATA_SUCCESS;                                             //填写成功标识
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008200:
        {
            OOPM_METER_130_T data;

            MEMZERO(&data, sizeof(data));

            convert.srcBuf = (void*)inData;
            convert.srcLen = inLen;
            convert.sOffset = &sOffset;

            convert.dstBuf = &data;
            convert.dstLen = sizeof(data);
            convert.dOffset = &dOffset; 

            /*更新档案单元（130方法）*/
            PRTL_FMT_LOGCC("class11_invoke_act 更新一个档案单元（方法130）\n");

            nRet = buf_to_data_oop_meter_extend_object(pOdaInfo, &convert);

            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            MEMZERO(&oldOopMeter, sizeof(OOP_METER_T));
            
            nomalOad.logicId = 0x00;
            nomalOad.infoNum = data.index;
            nomalOad.oad.value = 0x60000200;
            nomalOad.classtag = CLASS_DATA_INIT;

            if (data.index > NO_VIR_MTR_MAX_NUM)
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            //先读出原电表信息
            meterret = app_get_meter_list(nomalOad.infoNum, &oldOopMeter);
            if(meterret != ERR_OK)
            {
                PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), outLen\n",
                    meterret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value, sizeof(OOP_METER_T), len);
            }

            oopMeter = oldOopMeter;
            oopMeter.extend = data.ext;
            oopMeter.annex = data.annex;
            
            //置档案数据更新标志
            app_set_meter_update_start(nomalOad.infoNum);

            ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&oopMeter, sizeof(OOP_METER_T));
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("class11_invoke_act_oop_meter db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                return DATA_READ_WRITE_DENIED;
            }
            appctrl_db_chg_event(0, 21, oopMeter.nIndex);  //档案更新

            //等待数据更新完成
            if (wait_oop_meter_data_update(nomalOad.infoNum) != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("app_set_meter_invalid err\n");
                return DATA_READ_WRITE_DENIED;
            }

            //删档案，或通信地址，端口，规约类型，用户类型发生变化，则清电表数据
            if(meterret == ERR_OK)
            {
                ret = meter_data_if_clear(pApduInfo, &oldOopMeter, &oopMeter);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                    return DATA_READ_WRITE_DENIED;
                }
            }
            
            result = DATA_SUCCESS;                                             //填写成功标识
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008600:
        {
            /*清空档案*/
            CLEAR_DATA_T clearData;

            MEMZERO(&clearData, sizeof(clearData));
            PRTL_FMT_LOGCC("class11_invoke_act 清空档案\n");

            if(inData[sOffset ++] != DT_NULL)
            {
                return DATA_TYPE_UNMATCHED;
            }

            /*清空档案操作*/
            clearData.recordOAD.logicId = 0x00;
            clearData.recordOAD.optional = 0;
            clearData.recordOAD.road.oadMain.value = 0x00;
            clearData.recordOAD.road.oadCols.nNum = 1;
            clearData.recordOAD.road.oadCols.oad[0].value = 0x60000200;
            clearData.bClassTag = FALSE;
			clearData.recordOAD.classtag = CLASS_DATA_INIT;
            clearData.recordOAD.infoNum = 0xffff;
            ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
            if (ret != ERR_OK)
            {
                PRTL_FMT_LOGCC("删除档案全部失败 err = %d\n", ret);
            }
            else
            {
                PRTL_FMT_LOGCC("删除档案全部成功\n");
            }

            ret = meter_data_clear(pApduInfo->hUdp, clearData.recordOAD.infoNum);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                return DATA_READ_WRITE_DENIED;
            }
            appctrl_db_chg_event(0, 21, 0);  //档案更新
            result = DATA_SUCCESS;                                             //填写成功标识

            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008300:
        {
            /*删除一个档案*/
            CLEAR_DATA_T delData;
            uint16  data = 0;

            MEMZERO(&delData, sizeof(delData));

            convert.dstBuf = &data;
            convert.dstLen = sizeof(data);
            convert.dOffset = &dOffset;  
            PRTL_FMT_LOGCC("class11_invoke_act 删除一个档案\n");
            nRet = buf_to_data_del_one_meter(pOdaInfo, &convert);
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            PRTL_FMT_LOGCC("删除序号为%d的档案\n", data);

            /*清空档案操作*/
            delData.recordOAD.logicId = 0x00;
            delData.recordOAD.optional = 0;
            delData.recordOAD.road.oadMain.value = 0x00;
            delData.recordOAD.road.oadCols.nNum = 1;
            delData.recordOAD.road.oadCols.oad[0].value = 0x60000200;
            delData.bClassTag = FALSE;
			delData.recordOAD.classtag = CLASS_DATA_INIT;
            delData.recordOAD.infoNum = data;
            ret = db_clear_oad_data(pApduInfo->hUdp, &delData);
            if (ret != ERR_OK)
            {
                PRTL_FMT_LOGCC("删除序号档案%d err = %d\n", i, ret);
                return DATA_READ_WRITE_DENIED;
            }

            ret = meter_data_clear(pApduInfo->hUdp, delData.recordOAD.infoNum);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                return DATA_READ_WRITE_DENIED;
            }
            appctrl_db_chg_event(0, 21, delData.recordOAD.infoNum);  //档案更新

            result = DATA_SUCCESS;                                             //填写成功标识

            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008400:
        {
            /*删除档案，通过基本信息*/
            CLEAR_DATA_T delData;
            OOP_METER_BASIC_T  data;

            MEMZERO(&delData, sizeof(delData));
            MEMZERO(&data, sizeof(data));

            convert.dstBuf = &data;
            convert.dstLen = sizeof(data);
            convert.dOffset = &dOffset;  
            PRTL_FMT_LOGCC("class11_invoke_act 删除档案act132\n");
            nRet = buf_to_data_del_one_meter_by_basic(pOdaInfo, &convert);
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            /*清空档案操作*/
            delData.recordOAD.logicId = 0x00;
            delData.recordOAD.optional = 0;
            delData.recordOAD.road.oadMain.value = 0x00;
            delData.recordOAD.road.oadCols.nNum = 1;
            delData.recordOAD.road.oadCols.oad[0].value = 0x60000200;
            delData.bClassTag = FALSE;
			delData.recordOAD.classtag = CLASS_DATA_INIT;

            for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
            {
                delData.recordOAD.infoNum = i;

                //先读出原电表信息
                meterret = app_get_meter_list(i, &oldOopMeter);
                if(meterret != ERR_OK)
                {
                    PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        meterret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                    continue;
                }

                if (memcmp(&data, &oldOopMeter.basic, sizeof(data)) == 0)
                {
                    PRTL_FMT_LOGCC("删除序号为%d的档案\n", i);
                }
                else
                {
                    continue;
                }
                

                ret = db_clear_oad_data(pApduInfo->hUdp, &delData);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_LOGCC("删除序号档案%d err = %d\n", i, ret);
                    return DATA_READ_WRITE_DENIED;
                }

                ret = meter_data_clear(pApduInfo->hUdp, delData.recordOAD.infoNum);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                    return DATA_READ_WRITE_DENIED;
                }
                appctrl_db_chg_event(0, 21, delData.recordOAD.infoNum);  //档案更新
            }

            result = DATA_SUCCESS;                                             //填写成功标识

            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008500:
        {
            /*删除档案，通过基本信息*/
            CLEAR_DATA_T delData;
            OOP_METER_M133_T  data;

            MEMZERO(&delData, sizeof(delData));
            MEMZERO(&data, sizeof(data));

            convert.dstBuf = &data;
            convert.dstLen = sizeof(data);
            convert.dOffset = &dOffset;  
            PRTL_FMT_LOGCC("class11_invoke_act 删除档案act133\n");
            nRet = buf_to_data_del_one_meter_by_act133(pOdaInfo, &convert);
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            /*清空档案操作*/
            delData.recordOAD.logicId = 0x00;
            delData.recordOAD.optional = 0;
			delData.recordOAD.classtag = CLASS_DATA_INIT;
            delData.recordOAD.road.oadMain.value = 0x00;
            delData.recordOAD.road.oadCols.nNum = 1;
            delData.recordOAD.road.oadCols.oad[0].value = 0x60000200;
            delData.bClassTag = FALSE;
			

            for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
            {
                delData.recordOAD.infoNum = i;

                //先读出原电表信息
                meterret = app_get_meter_list(i, &oldOopMeter);
                if(meterret != ERR_OK)
                {
                    PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        meterret, nomalOad.logicId, i, nomalOad.oad.value);
                    continue;
                }

                if ((memcmp(&data.add, &oldOopMeter.basic.tsa, sizeof(data.add)) == 0) && 
                   (memcmp(&data.port, &oldOopMeter.basic.port, sizeof(data.port)) == 0))
                {
                    PRTL_FMT_LOGCC("删除序号为%d的档案\n", i);
                }
                else
                {
                    continue;
                }
                
                ret = db_clear_oad_data(pApduInfo->hUdp, &delData);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_LOGCC("删除序号档案%d err = %d\n", i, ret);
                    return DATA_READ_WRITE_DENIED;
                }

                ret = meter_data_clear(pApduInfo->hUdp, delData.recordOAD.infoNum);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("meter data clear failed. ret(%d), omd(0x%08x)\n", ret, pOdaInfo->pTab->oad.value);
                    return DATA_READ_WRITE_DENIED;
                }
                appctrl_db_chg_event(0, 21, delData.recordOAD.infoNum);  //档案更新
                
            }

            result = DATA_SUCCESS;                                             //填写成功标识

            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
        case 0x60008700:
        {
            OOPM_METER_135_T data;

            MEMZERO(&data, sizeof(data));

            convert.srcBuf = (void*)inData;
            convert.srcLen = inLen;
            convert.sOffset = &sOffset;

            convert.dstBuf = &data;
            convert.dstLen = sizeof(data);
            convert.dOffset = &dOffset; 

            /*更新档案附属信息（135方法）*/
            PRTL_FMT_LOGCC("class11_invoke_act 更新更新档案附属信息（方法135）\n");

            nRet = buf_to_data_oop_meter_annex_object(pOdaInfo, &convert);

            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            else
            {
                dOffset = 0;
            }

            MEMZERO(&oldOopMeter, sizeof(OOP_METER_T));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = 0x60000200;
            nomalOad.classtag = CLASS_DATA_INIT;
            for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
            {
                nomalOad.infoNum = i;

                //先读出原电表信息
                meterret = app_get_meter_list(i, &oldOopMeter);
                if(meterret != ERR_OK)
                {
                    PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        meterret, nomalOad.logicId, i, nomalOad.oad.value);
                    continue;
                }

                if ((memcmp(&data.add, &oldOopMeter.basic.tsa, sizeof(data.add)) == 0) )
                {
                    PRTL_FMT_DEBUG("方法135更新序号为 %d 的档案\n", i);
                }
                else
                {
                    continue;
                }
                
                oopMeter = oldOopMeter;
                oopMeter.annex = data.annex;
                
                //置档案数据更新标志
                app_set_meter_update_start(nomalOad.infoNum);
                
                ret = db_write_nomal(pApduInfo->hUdp, &nomalOad, (uint8*)&oopMeter, sizeof(OOP_METER_T));
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("class11_invoke_act_oop_meter db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                        ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                    return DATA_READ_WRITE_DENIED;
                }
                appctrl_db_chg_event(0, 21, nomalOad.infoNum);  //档案更新
                //等待数据更新完成
                if (wait_oop_meter_data_update(nomalOad.infoNum) != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("app_set_meter_invalid err\n");
                    return DATA_READ_WRITE_DENIED;
                }
                break;
            }

            result = DATA_SUCCESS;                                             //填写成功标识
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
        break;
    }
    return result;
}

/**
*********************************************************************
* @name：      class11_invoke_act_searchmeter_start
* @brief：     class11的ACTION服务
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
OOP_DAR_E class11_invoke_act_searchmeter_start(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class11_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class11_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60027F00:
        {
            uint16 data = 0;
            memcpy(&data, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("class11_invoke_act_searchmeter_start : data = %d\n", data);

            ret = seek_meter_to_taskManager(data);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("seek_meter_to_taskManager err\n");
                return FALSE;
            }
            PRTL_FMT_DEBUG("seek_meter_to_taskManager ok\n");
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
* @name：      class19_invoke_act_data_clear
* @brief：     class19的ACTION服务
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
OOP_DAR_E class11_invoke_act_searchmeter_all_clear(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset  = 0;

    //数据初始化操作
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60028000:
        {
            uint32 i = 0;
            CLEAR_DATA_T clearData;
            MEMZERO(&clearData, sizeof(clearData));

            clearData.recordOAD.logicId = 0;
            clearData.recordOAD.optional = 0;
			clearData.recordOAD.classtag = CLASS_DATA_INIT;
            clearData.recordOAD.road.oadMain.value = 0;
            clearData.recordOAD.road.oadCols.nNum = 1;
            clearData.recordOAD.road.oadCols.oad[0].value = 0x60020200;
            clearData.bClassTag = FALSE;
            for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
            {
                //判断是否有效
                if (!seekMeterList.isVaild[i])
                {
                    continue;
                }

                clearData.recordOAD.infoNum = i;
                ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), recordOad.infoNum(%d)\n",
                        ret, clearData.recordOAD.infoNum);
                    continue;
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
* @name：      class19_invoke_act_data_clear
* @brief：     class19的ACTION服务
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
OOP_DAR_E class11_invoke_act_searchmeter_acrossarea_clear(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset  = 0;

    //数据初始化操作
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60028100:
        {
            uint32 i = 0;
            CLEAR_DATA_T clrData;
            MEMZERO(&clrData, sizeof(clrData));

            clrData.recordOAD.logicId = 0;
            clrData.recordOAD.optional = 0;
			clrData.recordOAD.classtag = CLASS_DATA_INIT;
            clrData.recordOAD.road.oadMain.value = 0;
            clrData.recordOAD.road.oadCols.nNum = 1;
            clrData.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;
            clrData.bClassTag = FALSE;
            for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
            {
                //判断是否有效
                if (!seekAcrossList.isVaild[i])
                {
                    continue;
                }

                clrData.recordOAD.infoNum = i;
                ret = db_clear_oad_data(pApduInfo->hUdp, &clrData);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), recordOad.infoNum(%d)\n",
                        ret, clrData.recordOAD.infoNum);
                    continue;
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
* @name：      class11_invoke_act_searchmeter_res
* @brief：     class11的ACTION服务
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
OOP_DAR_E class11_invoke_act_searchmeter_res(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60027f00:
        return class11_invoke_act_searchmeter_start(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;

    case 0x60028000:
        return class11_invoke_act_searchmeter_all_clear(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;

    case 0x60028100:
        return class11_invoke_act_searchmeter_acrossarea_clear(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    
    default:
        return DATA_OBJECT_UNDEFINED;
        break;
    }
}

/**
*********************************************************************
* @name：      class11_invoke_act_normal_scheme
* @brief：     class11的ACTION服务
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
OOP_DAR_E class11_invoke_act_normal_scheme(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    int ret;
    uint32 offset=4;
    uint8 normal_num=0;
    OOP_DAR_E result = DATA_OTHER_REASON;

    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60147F00:
        {
            
            if(inData[offset++]!=DT_ARRAY)
            {
                return ERR_OPTION;
            }
            normal_num=inData[offset++];
            PRTL_FMT_LOGCC("normal_num is %d\n",normal_num);
            for(i=0;i<normal_num;i++)
            {
                result = buf_to_data_class8_normal_scheme(pOadInfo, (void*)inData, inLen, &offset,pApduInfo->hUdp);
            }
            memcpy_r((uint8*)outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
            if(result == ERR_OK)
            {
                result=DATA_SUCCESS;
                outData->pResultData[4]=DATA_SUCCESS;
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;
            }
            else
            {
                result=DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
                outData->resultDataLen = 0x05;
            }
        }
        break;
        case 0x60148000:
        {
            uint8 normal_num = 0;
            uint8 planid = 0;
            CLEAR_DATA_T RecordDel;

            MEMZERO(&RecordDel, sizeof(RecordDel));
            if(inData[offset++]!=DT_ARRAY)
            {
                return DATA_TYPE_UNMATCHED;
            }
            normal_num=inData[offset++];
            PRTL_FMT_LOGCC("normal_num is %d\n",normal_num);
            for(i=0;i<normal_num;i++)
            {
                offset++;
                planid=inData[offset++];
                //删除存储的临时数据
                RecordDel.recordOAD.logicId = 0;
                RecordDel.recordOAD.infoNum = planid;
                RecordDel.recordOAD.optional = 0;
				RecordDel.recordOAD.classtag = CLASS_DATA_INIT;
                RecordDel.recordOAD.road.oadMain.value = 0;
                RecordDel.recordOAD.road.oadCols.nNum = 1;
                RecordDel.recordOAD.road.oadCols.oad[0].value = 0x60140200;
                RecordDel.bClassTag = FALSE;
                ret = db_clear_oad_data(pApduInfo->hUdp,&RecordDel);
                if(ret==0)
                {
                    PRTL_FMT_LOGCC("数据删除成功！\n");
                }
                else
                {
                    PRTL_FMT_DEBUG("数据删除失败！\n");
                }
    
            }
            memcpy((uint8*)&outData->pResultData[0], (uint8*)&inData[0], 4);
            if(ret == ERR_OK)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;                  //DAR type
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;                    
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
                outData->resultDataLen=0x05;
            }
        }
        break;
        case 0x60148100:
        {
            CLEAR_DATA_T RecordDel;
        
            MEMZERO(&RecordDel, sizeof(RecordDel));

            //删除存储的临时数据
            RecordDel.recordOAD.logicId = 0;
            RecordDel.recordOAD.infoNum = 0xffff;
            RecordDel.recordOAD.optional = 0;
			RecordDel.recordOAD.classtag = CLASS_DATA_INIT;
            RecordDel.recordOAD.road.oadMain.value = 0;
            RecordDel.recordOAD.road.oadCols.nNum = 1;
            RecordDel.recordOAD.road.oadCols.oad[0].value = 0x60140200;
            RecordDel.bClassTag = FALSE;

            ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
            if(ret == 0)
            {
                PRTL_FMT_LOGCC("删除全部普通采集方案成功！\n");
            }
            else
            {
                PRTL_FMT_DEBUG("删除全部普通采集方案失败！ret = %d\n", ret);
            }
            
            memcpy((uint8*)&outData->pResultData[0], (uint8*)&inData[0], 4);
            if(ret == ERR_OK)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;                  //DAR type
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;                    
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
                outData->resultDataLen=0x05;
            }
        }
        break;
        case 0x60148200:  //重置CSD
        {
            result = normal_colscheme_csd_reset(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
            if(result != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(inData, inLen, "normal_colscheme_csd_reset failed. dar(%d), oad(0x%08x)\n", result, pOadInfo->pTab->oad.value);
            }
        }
        break;

    }
    return result;
}

/**
*********************************************************************
* @name：      class11_invoke_act_event_scheme
* @brief：     class11的ACTION服务
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
OOP_DAR_E class11_invoke_act_event_scheme(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    int ret;
    uint32 offset=4;
    uint8 event_num=0;
    int result = DATA_OTHER_REASON;

    /*类型不存在*/
    if ((pOdaInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOdaInfo->pTab->oad.value)
    {
        case 0x60167F00:
        {
            printf("class10_invoke_act offset is %d\n",offset);
            if(inData[offset++]!=DT_ARRAY)
            {
                return DATA_TYPE_UNMATCHED;
            }
            event_num=inData[offset++];
            printf("normal_num is %d\n",event_num);
            for(i=0;i<event_num;i++)
            {
                result=buf_to_data_class8_event_scheme(pOdaInfo, inData, inLen, &offset,pApduInfo->hUdp);
            }
            memcpy_r((uint8*)outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
            if(result==ERR_OK)
            {
                result=DATA_SUCCESS;
                outData->pResultData[4]=DATA_SUCCESS;
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;   
            }
            else
            {
                result=DATA_READ_WRITE_DENIED;
                outData->pResultData[4]=DATA_READ_WRITE_DENIED;
                outData->resultDataLen=0x05;
            }
            
        }
        break;
        case 0x60168000:
        {
            uint8 event_num=0;
            uint8 event_plan=0;
            CLEAR_DATA_T RecordDel;

            MEMZERO(&RecordDel, sizeof(RecordDel));

            if(inData[offset++]!=DT_ARRAY)
            {
                return DATA_TYPE_UNMATCHED;
            }

            event_num=inData[offset++];
            PRTL_FMT_LOGCC("event_num is %d\n",event_num);
            for(i=0;i<event_num;i++)
            {
                offset++;
                event_plan=inData[offset++];

                //删除存储的临时数据
                RecordDel.recordOAD.logicId = 0;
                RecordDel.recordOAD.infoNum = event_plan;
                RecordDel.recordOAD.optional = 0;
				RecordDel.recordOAD.classtag = CLASS_DATA_INIT;
                RecordDel.recordOAD.road.oadMain.value = 0;
                RecordDel.recordOAD.road.oadCols.nNum = 1;
                RecordDel.recordOAD.road.oadCols.oad[0].value = 0x60160200;
                RecordDel.bClassTag = FALSE;

                ret = db_clear_oad_data(pApduInfo->hUdp,&RecordDel);
                if(ret==0)
                {
                    PRTL_FMT_LOGCC("数据删除成功！\n");
                }
                else
                {
                    PRTL_FMT_DEBUG("数据删除失败！\n");
                }
    
            }
            memcpy((uint8*)&outData->pResultData[0], (uint8*)&inData[0], 4);
            if(ret == ERR_OK)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;                  //DAR type
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;                    
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
                outData->resultDataLen=0x05;
            }
        }
        break;
        case 0x60168100:
        {
            CLEAR_DATA_T RecordDel;

            MEMZERO(&RecordDel, sizeof(RecordDel));
            ret = ERR_OK;
            for(i = 1; i < 256; i++)
            {
                //判断事件方案是否有效
                if (!oopPlanEventList.isVaild[i])
                {
                    continue;
                }

                //删除存储的临时数据
                RecordDel.recordOAD.logicId = 0;
                RecordDel.recordOAD.infoNum = i;
                RecordDel.recordOAD.optional = 0;
				RecordDel.recordOAD.classtag = CLASS_DATA_INIT;
                RecordDel.recordOAD.road.oadMain.value = 0;
                RecordDel.recordOAD.road.oadCols.nNum = 1;
                RecordDel.recordOAD.road.oadCols.oad[0].value = 0x60160200;
                RecordDel.bClassTag = FALSE;
                PRTL_FMT_LOGCC("删除事件方案[%d]数据！！！\n", i);
                ret = db_clear_oad_data(pApduInfo->hUdp,&RecordDel);
                if (ret == 0)
                {
                    PRTL_FMT_LOGCC("数据删除成功！\n");
                }
                else
                {
                    PRTL_FMT_LOGCC("数据删除失败！\n");
                }
    
            }
            memcpy((uint8*)&outData->pResultData[0], (uint8*)&inData[0], 4);
            if(ret == ERR_OK)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;                  //DAR type
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;                    
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
            }
        }
        break;
        case 0x60168200:
        {
            ret =buf_to_data_class8_update_event_scheme(pOdaInfo, inData, inLen, &offset,pApduInfo->hUdp);
            if(ret == ERR_OK)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;                  //DAR type
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;                    
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
                outData->resultDataLen=0x05;
            }
        }
        break;

    }
    return result;
}

/**
*********************************************************************
* @name：      class11_invoke_act_event_scheme
* @brief：     class11的ACTION服务
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
OOP_DAR_E class11_invoke_act_transparent_scheme(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
 //   uint16 i = 0;
    uint32 offset=4;
//    uint8 normal_num=0;
    int result = DATA_OTHER_REASON;
    int ret;

    /*类型不存在*/
    if ((pOdaInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOdaInfo->pTab->oad.value)
    {  
        case 0x60187F00:  //下发透明方案
        {
            PRTL_FMT_LOGCC("下发透明任务\n");
            //if(inData[offset++]!=DT_ARRAY)
            {
            //    return ERR_OPTION;
            }
           // normal_num=inData[offset++];
            
           // PRTL_FMT_LOGCC("normal_num is %d\n",normal_num);
            //for(i=0;i<normal_num;i++)
            {
                result = buf_to_data_class8_transparent_scheme(pOdaInfo, (void*)inData, inLen, &offset,pApduInfo->hUdp);
            }
            PRTL_FMT_LOGCC("下发透明任务 result is %d,pOdaInfo->pTab->oad.value is %x\n",result,pOdaInfo->pTab->oad.value);
            memcpy_r((uint8*)outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
            if(result == ERR_OK)
            {
                result=DATA_SUCCESS;
                outData->pResultData[4]=DATA_SUCCESS;
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;
            }
            else
            {
                result=DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
                outData->resultDataLen = 0x05;
            }
        }
        break;
        case 0x60188000:  //添加一组报文
        {
           
            result =buf_to_data_class8_Add_Meter_Frame(pOdaInfo, (void*)inData, inLen, &offset,pApduInfo->hUdp);
            PRTL_FMT_LOGCC("添加一组报文 result is %d,pOdaInfo->pTab->oad.value is %x\n",result,pOdaInfo->pTab->oad.value);
            memcpy_r((uint8*)outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
            if(result == ERR_OK)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;                  //DAR type
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;                    
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
                outData->resultDataLen=0x05;
            }
        }
        break;
        case 0x60188300:  //下发透明方案
        {
            CLEAR_DATA_T RecordDel;
        
            MEMZERO(&RecordDel, sizeof(RecordDel));

            //删除存储的临时数据
            RecordDel.recordOAD.logicId = 0;
            RecordDel.recordOAD.infoNum = 0xffff;
            RecordDel.recordOAD.optional = 0;
            RecordDel.recordOAD.classtag = CLASS_DATA_INIT;
            RecordDel.recordOAD.road.oadMain.value = 0;
            RecordDel.recordOAD.road.oadCols.nNum = 1;
            RecordDel.recordOAD.road.oadCols.oad[0].value = 0x60180200;
            RecordDel.bClassTag = FALSE;

            ret = db_clear_oad_data(pApduInfo->hUdp, &RecordDel);
            if(ret == 0)
            {
                PRTL_FMT_LOGCC("删除全部透明采集方案成功！\n");
            }
            else
            {
                PRTL_FMT_DEBUG("删除全部透明采集方案失败！ret = %d\n", ret);
            }
            
            memcpy_r((uint8*)outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
            if(ret == ERR_OK)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;                  //DAR type
                outData->pResultData[5] = 0x00;                          //DATA:NULL
                outData->resultDataLen = 0x06;                    
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
                outData->resultDataLen=0x05;
            }
        }
        break;
    }
    return result;
}


/**
*********************************************************************
* @name：      class11_invoke_act_report_plan
* @brief：     class11的ACTION服务
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
OOP_DAR_E class11_invoke_act_report_plan(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int result = DATA_OTHER_REASON;

    /*类型不存在*/
    if ((pOdaInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOdaInfo->pTab->oad.value)
    {
        case 0x601C7F00: //上报方案
        {

            result = report_plan_add(pOdaInfo,  pApduInfo, inData, inLen, outData, outDataMax);
            
            memcpy_r((uint8*)outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
            if(result==ERR_OK)
            {
                result=DATA_SUCCESS;
                outData->pResultData[4]=DATA_SUCCESS;
            }
            else
            {
                result=DATA_READ_WRITE_DENIED;
                outData->pResultData[4]=DATA_READ_WRITE_DENIED;
            }
            outData->pResultData[5] = 0x00;                            //DATA:NULL
            outData->resultDataLen = 0x06;
        }
        break;
        
        case 0x601C8000: //清空上报方案
        {
            result = report_plan_del(pOdaInfo,  pApduInfo, inData, inLen, outData, outDataMax);

            memcpy_r((uint8*)outData->pResultData, (uint8*)&pOdaInfo->pTab->oad.value, 4);
            if(result == DATA_SUCCESS)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
            }
            outData->pResultData[5] = 0x00;                            //DATA:NULL
            outData->resultDataLen = 0x06;

            result = DATA_SUCCESS;                                     //填写成功标识
        }
        break;

        case 0x601C8100: //清空上报方案
        {
            result = report_plan_clear(pOdaInfo,  pApduInfo, inData, inLen, outData, outDataMax);

            memcpy_r((uint8*)outData->pResultData, (uint8*)&pOdaInfo->pTab->oad.value, 4);
            if(result == DATA_SUCCESS)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
            }
            outData->pResultData[5] = 0x00;                            //DATA:NULL
            outData->resultDataLen = 0x06;

            result = DATA_SUCCESS;                                     //填写成功标识
        }
        break;

    }
    return result;
}

/**
*********************************************************************
* @name：      class11_invoke_act_acq_rule
* @brief：     class11的ACTION服务
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
OOP_DAR_E class11_invoke_act_acq_rule(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int result = DATA_OTHER_REASON;

    /*类型不存在*/
    if ((pOdaInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOdaInfo->pTab->oad.value)
    {
        case 0x601E7F00: //采集规库
        {

            result = acq_rule_add(pOdaInfo,  pApduInfo, inData, inLen, outData, outDataMax);
            
            memcpy_r((uint8*)outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
            if(result == ERR_OK)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;
                outData->pResultData[5] = 0x00;                            //DATA:NULL
                outData->resultDataLen = 0x06;
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
            }
        }
        break;

    }
    return result;
}

/**********************************************************************
* @name      : class11_set_applic
* @brief     ：设置app许可
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-19
* @Update    : 
**********************************************************************/
int class11_set_applic(char *appno, char *applic, uint32 len)
{
#ifdef PRODUCT_ECU
    int ret = 0;
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*)hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        return 1;
    }
    else
    {
        ret = dev->devinfo_write_applicense(dev, appno, applic, ADV_APP_NAME, len);
        hal_device_release((HW_DEVICE *)dev);
        return ret;
    }
#else
    return 0;
#endif
}

/**
*********************************************************************
* @name：      class11_invoke_act_app_liecnce
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo       OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @param[out]：
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2021-3-16
*********************************************************************
*/
OOP_DAR_E class11_invoke_act_app_liecnce(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen,uint16 *len,uint8 *sendbuf)
{
    uint16  offset = 4,index=0;  //前4个字节是OAD
    uint16  number = 0;
    uint16  i = 0;
    OOP_VISIBLEVAR_T appno;
    OOP_VISIBLEVAR_T appname;
    OOP_VISIBLEVAR_T applic;
    
    /*类型不存在*/
    if((pOdaInfo == NULL) || (pApduInfo == NULL) || (inData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    if(DT_ARRAY != inData[offset++])
    {
        return DATA_TYPE_UNMATCHED;
    }
    number = inData[offset++];
    sendbuf[index++] = number;
    for(i = 0; i < number; i++)
    {
        if(DT_STRUCTURE != inData[offset++])
        {
            return DATA_TYPE_UNMATCHED;
        }
        if(3 != inData[offset++])
        {
            return DATA_TYPE_UNMATCHED;
        }
        /* APP编号 */
        memset(&appno, 0, sizeof(OOP_VISIBLEVAR_T));
        if(DT_VISIBLE_STRING != inData[offset++])
        {
            return DATA_TYPE_UNMATCHED;
        }
        appno.nNum = inData[offset++];
        if(OOP_APP_NO_LEN != appno.nNum)
        {
            return DATA_OTHER_REASON;
        }
        memcpy(appno.value, inData + offset, appno.nNum);
        offset += appno.nNum;
        
        /* APP名称 */
        memset(&appname, 0, sizeof(OOP_VISIBLEVAR_T));
        if(DT_VISIBLE_STRING != inData[offset++])
        {
            return DATA_TYPE_UNMATCHED;
        }
        appname.nNum = inData[offset++];
        if(appname.nNum > 63)
        {
            return DATA_OTHER_REASON;
        }
        memcpy(appname.value, inData + offset, appname.nNum);
        offset += appname.nNum;

        /* APP许可 */
        memset(&applic, 0, sizeof(applic));
        if(DT_OCTET_STRING != inData[offset++])
        {
            return DATA_TYPE_UNMATCHED;
        }
        applic.nNum =  inData[offset++];
        if(OOP_APP_LICENCE_LEN != applic.nNum)
        {
            return DATA_OTHER_REASON;
        }
        memcpy(applic.value, inData + offset, applic.nNum);
        offset += applic.nNum;
        
        if(offset > inLen)
        {
            return DATA_SCOPE_OF_ACCESS_VIOLATED;
        }
		sendbuf[index++] = appno.nNum;
	    memcpy(&sendbuf[index],appno.value,appno.nNum);
	    index+=appno.nNum;
	    sendbuf[index++] = appname.nNum;
	    memcpy(&sendbuf[index],appname.value,appname.nNum);
	    index+=appname.nNum;
		sendbuf[index++] = applic.nNum;
	    memcpy(&sendbuf[index],applic.value,applic.nNum);
	    index+=applic.nNum;
    }   
    *len = index;
    return DATA_SUCCESS;
}

#if DESC("转加密方法操作", 1)
/**
*********************************************************************
* @name：      encryption_plan_add
* @brief：     转加密方案添加
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E encryption_plan_add(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_PLAN_ENCRYPT_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint8 offlen = 0;      //数组数量长度
    uint8 i = 0;
    uint32 num = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    if(inData[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(inData, InDataLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //数组成员个数
    num = get_len_offset((void*)&inData[sOffset], &offlen);
    sOffset += offlen;
    
    PRTL_FMT_TRACE("report_plan_add array num(%d)\n", num);

    //写入数据
    normal.oad.value = 0x601E0200;
    
    for(i = 0; i < num; i++)
    {
        //每次目的偏移清零，循环一次解一个data结构
        dOffset = 0;
        
        //从报文中提取数据
        dar = buf_to_data_encryption_plan(pOadInfo, &convert);
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_report_plan failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
            return dar;
        }

        PRTL_FMT_LOGCC("*******************转加密方案数据打印*********************\n");
        PRTL_FMT_LOGCC("num = %d, i = %d\n", num, i);
        PRTL_FMT_LOGCC("data.planID = %d\n", data.planID);
        PRTL_FMT_LOGCC("data.planType = %d\n", data.planType);
        PRTL_FMT_LOGCC("*******************************************************\n");

        normal.infoNum = data.planID;
        normal.classtag = CLASS_DATA_INIT;
        ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_PLAN_ENCRYPT_T));
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), sizeof(OOP_PLAN_ENCRYPT_T) = \n",
                ret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(OOP_PLAN_ENCRYPT_T));
            return DATA_OTHER_REASON;
        }
    }
    
    return dar;
}

/**
*********************************************************************
* @name：      encryption_plan_delete
* @brief：     删除转加密方案
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E encryption_plan_delete(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_NORMAL;
    CLEAR_DATA_T clearData;
    uint32 sOffset = 4;
    uint8 planNum = 0;
    uint8 planID = 0;
    uint8 i = 0;

    MEMZERO(&clearData, sizeof(clearData));

    if (inData[sOffset++] != DT_ARRAY)
    {
        return DATA_TYPE_UNMATCHED;
    }
    
    planNum=inData[sOffset++];
    PRTL_FMT_LOGCC("encryption_plan_delete 删除转加密方案, %d\n", planNum);
    
    for (i = 0; i < planNum; i++)
    {
        sOffset++;
        planID = inData[sOffset++];
    
        //删除存储的临时数据
        clearData.recordOAD.logicId = 0;
        clearData.recordOAD.infoNum = planID;
        clearData.recordOAD.optional = 0;
        clearData.recordOAD.road.oadMain.value = 0;
        clearData.recordOAD.road.oadCols.nNum = 1;
        clearData.recordOAD.road.oadCols.oad[0].value = 0x601E0200;
        clearData.bClassTag = FALSE;
    
        ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
        if(ret == 0)
        {
            PRTL_FMT_LOGCC("转加密方案删除成功！\n");
        }
        else
        {
            PRTL_FMT_DEBUG("转加密方案删除失败！\n");
        }
    
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      encryption_plan_clear
* @brief：     清空转加密方案
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E encryption_plan_clear(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_NORMAL;
    CLEAR_DATA_T clearData;

    MEMZERO(&clearData, sizeof(clearData));
    PRTL_FMT_LOGCC("encryption_plan_clear 清空转加密方案\n");

    /*清空转加密操作*/
    clearData.recordOAD.logicId = 0x00;
    clearData.recordOAD.optional = 0;
    clearData.recordOAD.road.oadMain.value = 0x00;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = 0x601E0200;
    clearData.bClassTag = FALSE;

    clearData.recordOAD.infoNum = 0xffff;
    ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("清空所有转加密方案 err = %d\n", ret);
    }
    else
    {
        PRTL_FMT_LOGCC("清空所有转加密方案 ok = %d\n", ret);
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      class11_invoke_act_report_plan
* @brief：     class11的ACTION服务
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
OOP_DAR_E class11_invoke_act_encryption_plan(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int result = DATA_OTHER_REASON;

    /*类型不存在*/
    if ((pOdaInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOdaInfo->pTab->oad.value)
    {
        case 0x601E7F00: //添加转加密方案
        {
            result = encryption_plan_add(pOdaInfo,  pApduInfo, inData, inLen, outData, outDataMax);
            
            memcpy_r((uint8*)outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
            if(result==ERR_OK)
            {
                result=DATA_SUCCESS;
                outData->pResultData[4]=DATA_SUCCESS;
            }
            else
            {
                result=DATA_READ_WRITE_DENIED;
                outData->pResultData[4]=DATA_READ_WRITE_DENIED;
            }
            outData->pResultData[5] = 0x00;                            //DATA:NULL
            outData->resultDataLen = 0x06;
        }
        break;

        case 0x601E8000: //删除转加密方案
        {
            result = encryption_plan_delete(pOdaInfo,  pApduInfo, inData, inLen, outData, outDataMax);
            
            memcpy_r((uint8*)outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
            if(result==ERR_OK)
            {
                result=DATA_SUCCESS;
                outData->pResultData[4]=DATA_SUCCESS;
            }
            else
            {
                result=DATA_READ_WRITE_DENIED;
                outData->pResultData[4]=DATA_READ_WRITE_DENIED;
            }
            outData->pResultData[5] = 0x00;                            //DATA:NULL
            outData->resultDataLen = 0x06;
        }
        break;

        case 0x601E8100: //清空转加密方案
        {
            result = encryption_plan_clear(pOdaInfo,  pApduInfo, inData, inLen, outData, outDataMax);

            memcpy_r((uint8*)outData->pResultData, (uint8*)&pOdaInfo->pTab->oad.value, 4);
            if(result == DATA_SUCCESS)
            {
                result = DATA_SUCCESS;
                outData->pResultData[4] = DATA_SUCCESS;
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
                outData->pResultData[4] = DATA_READ_WRITE_DENIED;
            }
            outData->pResultData[5] = 0x00;                            //DATA:NULL
            outData->resultDataLen = 0x06;

            result = DATA_SUCCESS;                                     //填写成功标识
        }
        break;

    }
    return result;
}
#endif

#if DESC("转加密方法读取", 1)
/**
*********************************************************************
* @name：      class11_invoke_get_encryption_res_all_nor
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_encryption_res_all_nor(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint16    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60200200:
        {
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            OOP_ENCRYPTRES_T data;
            DATA_CONVERT_T convert;
            uint32 searcherMeterNum = 0;
            uint32 searcherMeterNumOffset = 0;

            MEMZERO(&data, sizeof(data));
            MEMZERO(&convert, sizeof(convert));

            #if ENCRYPTION_MOD == 1
            uint32 len = 0;
            NOMAL_OAD_T normal;
            MEMZERO(&normal, sizeof(normal));
            normal.logicId = 0x00;
            normal.oad.value = pOadInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取所有搜表结果\n");

            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x01;                             //结果
            outData->pResultData[offset ++] = DT_ARRAY;                         //类型
            searcherMeterNumOffset = offset;                                    //档案数量偏移
            outData->pResultData[offset ++] = 0x00;                             //数量，先填0
            bufAllLen = offset;
            for (i = 1; i <= 255; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                if (slicing_is_comtinue_searchmeter_res_all(pApduInfo->bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                #if ENCRYPTION_MOD == 1
                normal.infoNum = i;
                ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_ENCRYPTRES_T)))
                #else
                ret = app_get_encryption_res_list(i, &data);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }
                PRTL_FMT_LOGCC("找到转加密结果 序号为%d\n", i);

                //转换成报文数据
                convert.srcBuf = &data;
                convert.srcLen = sizeof(data);
                convert.sOffset = &sOffset;
                
                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                dar = data_to_buf_class8_encryption_res_all(pOadInfo, &convert);
                if (dar == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个转加密报文:", i);
                        searcherMeterNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("data_to_buf_class8_encryption_res_all isBufFull == TRUE isBufFull = %d\n", isBufFull);
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

            PRTL_BUF_LOGCC(outData->pResultData, offset, "整个转加密报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

            result = DATA_SUCCESS;
            outData->pResultData[searcherMeterNumOffset] = searcherMeterNum;   //重新记数量
            outData->resultDataLen = offset;
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
* @name：      class11_invoke_get_encryption_res_all_rec
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_encryption_res_all_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    /* 类型不存在 */
    if (pOdaInfo->pTab->oad.nObjID != 0x6020)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOdaInfo->pTab->oad.attID)
    {
    case 2:
        {
            OOP_PRSD_T oopRsd;
            OOP_RCSD_T oopRcsd;
            uint16 headLen = 0;
            READ_NOR_BP_T inReadNor;
            RELY_INFO_T  relyInfo;
            RECORD_BUF_T outRecordBuf;

            /* 清0处理 */
            MEMZERO(&oopRsd, sizeof(oopRsd));
            MEMZERO(&oopRcsd, sizeof(oopRcsd));
            MEMZERO(&inReadNor, sizeof(inReadNor));
            MEMZERO(&relyInfo, sizeof(relyInfo));
            MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

            /*处理RSD*/
            nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
            PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
            PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
            PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
            PRTL_FMT_LOGCC("offset = %d\n", offset);
            if (nRet != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("RSD解析错误\n");

                /*应对电科院协议一致性测试*/
                uint16 testOffset = 4;
                uint16 inCsdOff = 0, inCsdLen = 0;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
                inCsdOff = testOffset;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
                inCsdLen = testOffset - inCsdOff;
                memcpy(outData->pResultData, inData, 4);
                memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
                outData->resultDataLen = 4 + inCsdLen;

                outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
                if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
                {
                    outData->pResultData[outData->resultDataLen++] = 0;

                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    for (i = 0; i < oopRcsd.nNum; i++)
                    {
                        outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                    }
                }

                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_SUCCESS;
            }
            else
            {
                inDataLenTmp -= offset;  //除去RSD和OAD后的长度
            }

            /*处理RCSD*/
            csdStart = offset;
            if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("CSD解析错误!\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_TYPE_UNMATCHED;
            }

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;

            if (csdLen == 1)
            {
                //如果rcsd为空，填充OAD601f0300
                outData->pResultData[outOffset ++] = 0x01;
                outData->pResultData[outOffset ++] = 0x00;
                outData->pResultData[outOffset ++] = 0x60;
                outData->pResultData[outOffset ++] = 0x1f;
                outData->pResultData[outOffset ++] = 0x03;
                outData->pResultData[outOffset ++] = 0x00;
            }
            else
            {
                memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
                outOffset += csdLen;
            }
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;
            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*记录型档案接口提供输入参数*/
            if (read_nor_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
            {
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /* 依赖信息中存下OAD列表的指针 */
            relyInfo.pOadInfo = pOdaInfo;

            PRTL_FMT_LOGCC("****************************************************\n");
            PRTL_FMT_LOGCC("inReadNor.startInfoNum = %d\n", inReadNor.startInfoNum);
            PRTL_FMT_LOGCC("inReadNor.endInfoNum = %d\n", inReadNor.endInfoNum);
            PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                      relyInfo.oopRcsd.cols[i].road.oadMain.value);
                    for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                    {
                        PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                                  relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                    }
                }
            }
            PRTL_FMT_LOGCC("****************************************************\n");

            /*读记录接口，可以读出apdu数据域，可返回断点信息*/
            if (record_encryption_res_all_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
            {
                nRet = DATA_READ_WRITE_DENIED;
            }
            else
            {
                memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
                outData->resultDataLen += outRecordBuf.bufLen;

                if (outRecordBuf.breakInfo.appSlicingFlag == 1)
                {
                    /*记下断点信息*/
                    PRTL_FMT_LOGCC("记下断点信息\n");
                    outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
                    memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
                }
                else
                {
                    /*清除断点信息*/
                    PRTL_FMT_LOGCC("清除断点信息\n");
                    memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                }
                
                nRet = DATA_SUCCESS;
            }
            
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_SUCCESS;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_searchmeter_res_all
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_encryption_res_all(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
        return class11_invoke_get_encryption_res_all_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_encryption_res_all_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：      class11_invoke_get_encryption_res
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_encryption_res(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x60200200:
        return class11_invoke_get_encryption_res_all(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;

    default:
        return DATA_OBJECT_UNDEFINED;
        break;
    }
}

/**
*********************************************************************
* @name：       class11_invoke_get
* @brief：      class11的GET服务
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
OOP_DAR_E class11_invoke_get_encryption_scheme_nor(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int result = DATA_OTHER_REASON;
    uint32 ret,len;
    uint32 offset=0;
    uint16 i;
    
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    OOP_PLAN_ENCRYPT_T PLAN_ENCRYPT_T;
    NOMAL_OAD_T NormalOad ;
    DATA_CONVERT_T convert;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;
    
    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    PRTL_FMT_LOGCC("class11_invoke_get_encryption_scheme_nor\n");
    switch (pOdaInfo->pTab->oad.nObjID)
    {
        case 0x601E:
        {
            if (pOdaInfo->nIndex == 0x00)
            {
                uint8 reportplannum = 0;
                uint32 oopreportplanNumOffset = 0;
                memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;                             //结果
                outData->pResultData[offset ++] = DT_ARRAY;                         //类型
                oopreportplanNumOffset = offset;                                         //任务数量偏移
                outData->pResultData[offset ++] = 0x00;                             //数量，先填0
                bufAllLen = offset;
                for(i = 1; i <= 255; i++)
                { 
                    if (slicing_is_comtinue(pApduInfo->bpBreakInfo, i))
                    {
                        continue;
                    }
                    NormalOad.logicId = 0;
                    NormalOad.infoNum = i;
                    NormalOad.oad.value = 0x601E0200;

                    memset((uint8*)&PLAN_ENCRYPT_T, 0x00, sizeof(PLAN_ENCRYPT_T));

                    ret = db_read_nomal(pApduInfo->hUdp, &NormalOad,sizeof(PLAN_ENCRYPT_T), (uint8*)&PLAN_ENCRYPT_T, &len);
                    if ((ret != ERR_OK) || (len != sizeof(PLAN_ENCRYPT_T)))
                    {
                        continue;
                    }
                    //转换成报文数据
                    convert.srcBuf = &PLAN_ENCRYPT_T;
                    convert.srcLen = sizeof(PLAN_ENCRYPT_T);
                    convert.sOffset = &sOffset;

                    convert.dstBuf = &outData->pResultData[offset];
                    convert.dstLen = outDataMax - offset;
                    convert.dOffset = &dOffset;

                    //记录断点信息
                    breakInfo.index = i;
                    
                    result = data_to_buf_encryption_plan(pOdaInfo,&convert);
                    PRTL_FMT_LOGCC("result is %d\n",result);
                    if (result == ERR_OK)
                    {
                        bufAllLen += dOffset;
                        if (bufAllLen <= maxBufLen)
                        {
                            memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                            offset += dOffset;
                            outData->resultDataLen = offset;
                            PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个任务报文:", i);
                            reportplannum ++;
                            dOffset = 0;
                            sOffset = 0;
                        }
                        else
                        {
                            dOffset = 0;
                            sOffset = 0;
                            isBufFull = TRUE;
                            PRTL_FMT_LOGCC("class10_invoke_get_task_list_nor isBufFull == TRUE isBufFull = %d\n", isBufFull);
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

                PRTL_BUF_LOGCC(outData->pResultData, offset, "整个任务报文:");
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
                PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);

                if (reportplannum > 0)
                {
                    outData->pResultData[oopreportplanNumOffset] = reportplannum;   //重新记档案数量
                    outData->resultDataLen = offset;
                    result = DATA_SUCCESS;
                }
                else if(reportplannum == 0)
                {
                    memcpy_r(outData->pResultData,(uint8*)&pOdaInfo->pTab->oad.value,4);
                    outData->pResultData[4]=DT_ARRAY;
                    outData->pResultData[5]=0x01;
                    outData->pResultData[6]=0x00;
                    outData->resultDataLen=7;
                    result = DATA_SUCCESS;
                }
                else
                {
                    result = DATA_SUCCESS;
                }
                

            }
            else
            {
                NormalOad.logicId = 0;
                NormalOad.infoNum = pOdaInfo->nIndex;
                NormalOad.oad.value = 0x601E0200;

                convert.dstBuf = &outData->pResultData[5];
                convert.dstLen = outDataMax - 5;
                convert.dOffset = &dOffset;

                memset((uint8*)&PLAN_ENCRYPT_T, 0x00, sizeof(PLAN_ENCRYPT_T));
                ret=db_read_nomal(pApduInfo->hUdp, &NormalOad, sizeof(PLAN_ENCRYPT_T), (uint8*)&PLAN_ENCRYPT_T, &len);
                if ((ret != ERR_OK) || (len != sizeof(PLAN_ENCRYPT_T)))
                {
                    return DATA_OBJECT_UNAVAILABLE;
                }

                //转换成报文数据
                convert.srcBuf = &PLAN_ENCRYPT_T;
                convert.srcLen = sizeof(PLAN_ENCRYPT_T);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outData->pResultData[offset];
                convert.dstLen = outDataMax - offset;
                convert.dOffset = &dOffset;
                
                result = data_to_buf_encryption_plan(pOdaInfo,&convert);
                if(result == ERR_OK)
                {
                    memcpy(outData->pResultData,inData,4);
                    outData->pResultData[4]=0x01;
                    outData->resultDataLen=dOffset+5;
                }
                else
                {
                    memcpy(outData->pResultData,inData,4);
                    outData->pResultData[4]=0x00;
                    outData->pResultData[5]=DATA_OBJECT_UNAVAILABLE;
                    outData->resultDataLen=6;
                }
            }
        }
        break;
        default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }
    return result;
}

/**
*********************************************************************
* @name：      class11_invoke_get_encryption_scheme_rec
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_encryption_scheme_rec(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    /* 类型不存在 */
    if (pOdaInfo->pTab->oad.nObjID != 0x601E)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    switch (pOdaInfo->pTab->oad.attID)
    {
    case 2:
        {
            OOP_PRSD_T oopRsd;
            OOP_RCSD_T oopRcsd;
            uint16 headLen = 0;
            READ_NOR_BP_T inReadNor;
            RELY_INFO_T  relyInfo;
            RECORD_BUF_T outRecordBuf;

            /* 清0处理 */
            MEMZERO(&oopRsd, sizeof(oopRsd));
            MEMZERO(&oopRcsd, sizeof(oopRcsd));
            MEMZERO(&inReadNor, sizeof(inReadNor));
            MEMZERO(&relyInfo, sizeof(relyInfo));
            MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

            outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
            memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

            /*处理RSD*/
            nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
            PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
            PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
            PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
            PRTL_FMT_LOGCC("offset = %d\n", offset);
            if (nRet != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("RSD解析错误\n");

                /*应对电科院协议一致性测试*/
                uint16 testOffset = 4;
                uint16 inCsdOff = 0, inCsdLen = 0;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
                inCsdOff = testOffset;
                apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
                inCsdLen = testOffset - inCsdOff;
                memcpy(outData->pResultData, inData, 4);
                memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
                outData->resultDataLen = 4 + inCsdLen;

                outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
                if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
                {
                    outData->pResultData[outData->resultDataLen++] = 0;

                }
                else
                {
                    outData->pResultData[outData->resultDataLen++] = 1;
                    for (i = 0; i < oopRcsd.nNum; i++)
                    {
                        outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                    }
                }

                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_SUCCESS;
            }
            else
            {
                inDataLenTmp -= offset;  //除去RSD和OAD后的长度
            }

            /*处理RCSD*/
            csdStart = offset;
            if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
            {
                PRTL_FMT_TRACE("CSD解析错误!\n");
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_TYPE_UNMATCHED;
            }

            /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
            outData->resultDataLen = 0;
            csdLen = offset-csdStart;
            memcpy(outData->pResultData, inData, 4);
            outOffset = 4;

            if (csdLen == 1)
            {
                //如果rcsd为空，填充OAD601F0200
                outData->pResultData[outOffset ++] = 0x01;
                outData->pResultData[outOffset ++] = 0x00;
                outData->pResultData[outOffset ++] = 0x60;
                outData->pResultData[outOffset ++] = 0x1F;
                outData->pResultData[outOffset ++] = 0x02;
                outData->pResultData[outOffset ++] = 0x00;
            }
            else
            {
                memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
                outOffset += csdLen;
            }
            outData->pResultData[outOffset++] = 1;
            outData->resultDataLen += outOffset;
            headLen = outOffset;
            PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

            /*记录型档案接口提供输入参数*/
            if (read_nor_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
            {
                free(outRecordBuf.recordBuf);
                outRecordBuf.recordBuf = NULL;
                return DATA_READ_WRITE_DENIED;
            }

            /* 依赖信息中存下OAD列表的指针 */
            relyInfo.pOadInfo = pOdaInfo;

            PRTL_FMT_LOGCC("****************************************************\n");
            PRTL_FMT_LOGCC("inReadNor.startInfoNum = %d\n", inReadNor.startInfoNum);
            PRTL_FMT_LOGCC("inReadNor.endInfoNum = %d\n", inReadNor.endInfoNum);
            PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
            for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
            {
                if (relyInfo.oopRcsd.cols[i].choice == 0)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].oad.value);
                }
                else if(relyInfo.oopRcsd.cols[i].choice == 1)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                      relyInfo.oopRcsd.cols[i].road.oadMain.value);
                    for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                    {
                        PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                                  relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                    }
                }
            }
            PRTL_FMT_LOGCC("****************************************************\n");

            /*读记录档案接口，可以读出apdu数据域，可返回断点信息, 应用分帧长度判断需要优化修改，目前直接减30*/
            if (record_report_scheme_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - 3 - 30, &outRecordBuf))
            {
                nRet = DATA_READ_WRITE_DENIED;
            }
            else
            {
                memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
                outData->resultDataLen += outRecordBuf.bufLen;

                if (outRecordBuf.breakInfo.appSlicingFlag == 1)
                {
                    /*记下断点信息*/
                    PRTL_FMT_LOGCC("记下断点信息\n");
                    outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
                    memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
                }
                else
                {
                    /*清除断点信息*/
                    PRTL_FMT_LOGCC("清除断点信息\n");
                    memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
                }
                
                nRet = DATA_SUCCESS;
            }
            
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_SUCCESS;
        }
        break;
    default:
        {
            return DATA_OBJECT_UNAVAILABLE;
        }
        break;
    }

    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_encryption_scheme
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_encryption_scheme(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
        return class11_invoke_get_encryption_scheme_nor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_encryption_scheme_rec(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

#endif

#if DESC("class11对外接口函数",1)

/**
*********************************************************************
* @name：      class11_invoke_set
* @brief：     class11的SET服务
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
OOP_DAR_E class11_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x6002:
        return class11_invoke_set_searchmeter_res(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    case 0x60F1:
        return class11_invoke_set_searchmeter_res(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  

    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}
/**
*********************************************************************
* @name：      get_container_info
* @brief：     从系统管理器获取容器信息
* @param[in] ：

* @param[out]：containerinfo    容器信息
* @return    ：容器个数
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
uint8 get_container_info(OOP_CONTAINER_T* containerinfo,BOOL Isrecord,uint8* containername,uint32 namelen)
{
    int ret;
    uint8 recvbuf[2000]={0};
	uint8 sendbuf[32]={0};
    uint16 recvlen;
    uint16 index=0;
    uint8 count,i;
    uint16 len;
    uint8 lenoffset;
    uint8 offset=0;
    PRTL_FMT_DEBUG("开始获取容器信息get_container_info \n");
    if(Isrecord == TRUE)
    {
        sendbuf[offset++] = 0x01;
        sendbuf[offset++] = namelen;
        memcpy((uint8*)&sendbuf[offset],containername,namelen);
        offset =offset+namelen;
    }
    else
    {
        offset =1;
    }
    ret = appmain_send_syncmsg_time(MSG_APPAMIN_GET_CONTAINERINFO,sendbuf,offset,recvbuf,sizeof(recvbuf),&recvlen, 60);
    if(ret!=ERR_OK)
    {
        PRTL_FMT_DEBUG("获取容器信息失败 ret %d\n", ret);
        return 0;
    }
    count = recvbuf[index++];
    for(i=0;i<count;i++)
    {
        containerinfo[i].containerid.nNum = recvbuf[index++];
        memcpy(containerinfo[i].containerid.value,&recvbuf[index],containerinfo[i].containerid.nNum);
        index+=containerinfo[i].containerid.nNum;
        len = get_len_offset(&recvbuf[index],&lenoffset);
        index+=lenoffset;
        containerinfo[i].containername.nNum = len;
        memcpy(containerinfo[i].containername.value,&recvbuf[index],len);
        index+=len;
        
        containerinfo[i].con_config.cpucores = recvbuf[index++];
		memcpy_r(&containerinfo[i].con_config.cputhreshold,&recvbuf[index],sizeof(uint16));
		index+=sizeof(uint16);
		memcpy_r(&containerinfo[i].con_config.memlimit,&recvbuf[index],sizeof(uint16));
        index+=sizeof(uint16);
        memcpy_r(&containerinfo[i].con_config.memthreshold,&recvbuf[index],sizeof(uint16));
        index+=sizeof(uint16);
        memcpy_r(&containerinfo[i].con_config.disklimit,&recvbuf[index],sizeof(uint16));
        index+=sizeof(uint16);
        memcpy_r(&containerinfo[i].con_config.diskthreshold,&recvbuf[index],sizeof(uint16));
        index+=sizeof(uint16);
        containerinfo[i].con_config.hostfile.nNum = recvbuf[index++];
        memcpy(containerinfo[i].con_config.hostfile.value,&recvbuf[index],containerinfo[i].con_config.hostfile.nNum);
        index+=containerinfo[i].con_config.hostfile.nNum;
        containerinfo[i].con_config.savefilepath.nNum = recvbuf[index++];
        memcpy(containerinfo[i].con_config.savefilepath.value,&recvbuf[index],containerinfo[i].con_config.savefilepath.nNum);
        index+=containerinfo[i].con_config.savefilepath.nNum;
        containerinfo[i].con_config.hardinterface.nNum = recvbuf[index++];
        memcpy(containerinfo[i].con_config.hardinterface.value,&recvbuf[index],containerinfo[i].con_config.hardinterface.nNum);
        index+=containerinfo[i].con_config.hardinterface.nNum;
        containerinfo[i].con_config.portconfig.nNum = recvbuf[index++];
        memcpy(containerinfo[i].con_config.portconfig.value,&recvbuf[index],containerinfo[i].con_config.portconfig.nNum);
        index+=containerinfo[i].con_config.portconfig.nNum;
        len = get_len_offset(&recvbuf[index],&lenoffset);
        index+=lenoffset;
        containerinfo[i].containerstate.nNum = len;
        memcpy(containerinfo[i].containerstate.value,&recvbuf[index],len);
        index+=len;

        memcpy_r(&containerinfo[i].cpuusage,&recvbuf[index],sizeof(uint16));
        index+=sizeof(uint16);
        memcpy_r(&containerinfo[i].memusage,&recvbuf[index],sizeof(uint16));
        index+=sizeof(uint16);
        memcpy_r(&containerinfo[i].storageusage,&recvbuf[index],sizeof(uint16));
        index+=sizeof(uint16);

		len = get_len_offset(&recvbuf[index],&lenoffset);
		index += lenoffset;
		if(len > 4)
		{
			memcpy(containerinfo[i].ipaddress, &recvbuf[index], 4);
		}
		else
		{
			memcpy(containerinfo[i].ipaddress, &recvbuf[index], len);
		}
		index+=len;

        containerinfo[i].imagename.nNum = recvbuf[index++];
        memcpy(containerinfo[i].imagename.value,&recvbuf[index],containerinfo[i].imagename.nNum);
        index+=containerinfo[i].imagename.nNum;

        memcpy_r(&containerinfo[i].createtime.year,&recvbuf[index],sizeof(uint16));
        index+=sizeof(uint16);
        containerinfo[i].createtime.month = recvbuf[index++];
        containerinfo[i].createtime.day = recvbuf[index++];
        containerinfo[i].createtime.hour = recvbuf[index++];
        containerinfo[i].createtime.minute = recvbuf[index++];
        containerinfo[i].createtime.second = recvbuf[index++];
        memcpy_r(&containerinfo[i].starttime.year,&recvbuf[index],sizeof(uint16));
        index+=sizeof(uint16);
        containerinfo[i].starttime.month = recvbuf[index++];
        containerinfo[i].starttime.day = recvbuf[index++];
        containerinfo[i].starttime.hour = recvbuf[index++];
        containerinfo[i].starttime.minute = recvbuf[index++];
        containerinfo[i].starttime.second = recvbuf[index++];
        memcpy_r(&containerinfo[i].runtime,&recvbuf[index],sizeof(uint32));
        index+=sizeof(uint32);
        
    }
    return count;
}

OOP_DAR_E data_to_buf_container_info(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CONTAINER_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);
    
    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 12;
    //容器id
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.containerid, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //容器名称
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.containername, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //容器名称
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.containerstate, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //容器CPU使用率
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.cpuusage, sizeof(data.cpuusage));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //容器内存使用率
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.memusage, sizeof(data.memusage));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //容器存储使用率
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.storageusage, sizeof(data.storageusage));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //IP地址
    dstBuf[dOffset++] = DT_OCTET_STRING;
    dstBuf[dOffset++] = 4;                              //密码最大支持长度为8
    memcpy(&dstBuf[dOffset], data.ipaddress, 4);
    dOffset += 4;
    //镜像名称
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.imagename, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //容器创建时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.createtime, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //容器启动时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.starttime, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //容器运行时间
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.runtime, sizeof(data.runtime));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

	//容器配置信息
	dstBuf[dOffset++] = DT_STRUCTURE;
    //结构成员数
    dstBuf[dOffset++] = 10;
	dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.con_config.cpucores, sizeof(data.con_config.cpucores));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
	dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.con_config.cputhreshold, sizeof(data.con_config.cputhreshold));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
	dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.con_config.memlimit, sizeof(data.con_config.memlimit));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
	dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.con_config.memthreshold, sizeof(data.con_config.memthreshold));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
	dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.con_config.disklimit, sizeof(data.con_config.disklimit));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
	dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.con_config.diskthreshold, sizeof(data.con_config.diskthreshold));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
	dar = basic_data_to_buf(E_OOP_VISIBLESTR256_T, dstBuf, dstLen, &dOffset, &data.con_config.hostfile, sizeof(OOP_VISIBLEVAR256_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
	dar = basic_data_to_buf(E_OOP_VISIBLESTR256_T, dstBuf, dstLen, &dOffset, &data.con_config.savefilepath, sizeof(OOP_VISIBLEVAR256_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
	dar = basic_data_to_buf(E_OOP_VISIBLESTR256_T, dstBuf, dstLen, &dOffset, &data.con_config.hardinterface, sizeof(OOP_VISIBLEVAR256_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
	dar = basic_data_to_buf(E_OOP_VISIBLESTR256_T, dstBuf, dstLen, &dOffset, &data.con_config.portconfig, sizeof(OOP_VISIBLEVAR256_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
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
* @name：      class11_invoke_get_container_normal
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_container_normal(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{

    uint8 count,i;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32 offset = 0;
    uint32 containerNum = 0;
    uint32 containernumoffset;
    OOP_CONTAINER_T containerlist[20];
    MEMZERO(containerlist, sizeof(containerlist));
    DATA_CONVERT_T convert;
    MEMZERO(&convert, sizeof(DATA_CONVERT_T));
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    BP_INFO_T breakInfo;
    MEMZERO(&breakInfo, sizeof(BP_INFO_T));
    uint32    bufAllLen = 0;
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    if(pOadInfo->pTab->oad.value!=0x73000200)
        return DATA_READ_WRITE_DENIED;
    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    count = get_container_info(containerlist,FALSE,NULL,0);
    if(count==0)
    {
        PRTL_FMT_DEBUG("获取容器信息失败 \n");
        return DATA_OTHER_REASON;
    }
    PRTL_FMT_DEBUG("容器个数 %d \n",count);
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;                             //结果
    outData->pResultData[offset ++] = DT_ARRAY;                         //类型
    containernumoffset = offset;                                        
    outData->pResultData[offset ++] = 0x00;                             //数量，先填0
    bufAllLen = offset;
    for(i=0;i<count;i++)
    {
        if (slicing_is_comtinue_oop_meter(pApduInfo->bpBreakInfo, i))
        {
            continue;
        }   
        //转换成报文数据
        convert.srcBuf = &containerlist[i];
        convert.srcLen = sizeof(OOP_CONTAINER_T);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outData->pResultData[offset];
        convert.dstLen = outDataMax - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = i;

        nRet = data_to_buf_container_info(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outData->resultDataLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个容器信息报文:", i);
                containerNum ++;
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_LOGCC("class11_invoke_get_container isBufFull == TRUE \n");
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

    PRTL_BUF_DEBUG(outData->pResultData, offset, "整个容器信息报文:");
    PRTL_FMT_DEBUG("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
    PRTL_FMT_DEBUG("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
    PRTL_FMT_DEBUG("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
    PRTL_FMT_DEBUG("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
    PRTL_FMT_DEBUG("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
    PRTL_FMT_DEBUG("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);
    if (containerNum > 0)
    {
        nRet = DATA_SUCCESS;
        outData->pResultData[containernumoffset] = containerNum;   
        outData->resultDataLen = offset;
    }
    else
    {
        nRet = DATA_SUCCESS;
        outData->pResultData[containernumoffset] = 0;  
        outData->resultDataLen = offset;
    }
    return nRet;
}
/**
*********************************************************************
* @brief：     记录型读取容器信息
* @param[in]： inReadNor  - 筛选需要的数据

               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_container_read(const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    uint8 count;
    OOP_CONTAINER_T containerlist[20];
    MEMZERO(containerlist, sizeof(containerlist));
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo ;                                                //断点信息
    BOOL      isBufFull = FALSE;
    uint32 offset = 0;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;
    MEMZERO(&breakInfo, sizeof(BP_INFO_T));
    
    count = get_container_info(containerlist,TRUE,relyInfo.oopRsd->sel1.value.octVal.value,relyInfo.oopRsd->sel1.value.octVal.nNum);
    if(count==0)
    {
        PRTL_FMT_DEBUG("获取容器信息失败 \n");
        return -1;
    }
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x73000200:
    {
        DATA_CONVERT_T convert;
        MEMZERO(&convert, sizeof(DATA_CONVERT_T));
        uint32 sOffset = 0;
        uint32 dOffset = 0;
        uint32 containerNum = 0;
        uint32 containernumoffset;
        uint8 endindex;
        uint8 i;
        containernumoffset = offset;                                         //档案数量偏移
        outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
        bufAllLen = offset;
        if(inReadNor.startInfoNum>=count)
        {
            return DATA_READ_WRITE_DENIED;
        }
        if(inReadNor.endInfoNum>=count)
        {
            endindex = count;
        }else
        {
            endindex = inReadNor.endInfoNum+1;
        }
        for(i=inReadNor.startInfoNum;i<endindex;i++)
        {
            if (slicing_is_comtinue_oop_meter(relyInfo.bpBreakInfo, i))
            {
                continue;
            }   
            //转换成报文数据
            convert.srcBuf = &containerlist[i];
            convert.srcLen = sizeof(OOP_CONTAINER_T);
            convert.sOffset = &sOffset;

            convert.dstBuf = &outRecordBuf->recordBuf[offset];
            convert.dstLen = maxBufLen - offset;
            convert.dOffset = &dOffset;

            //记录断点信息
            breakInfo.index = i;

            nRet = data_to_buf_container_info(pOadInfo, &convert);  //入参
            if (nRet == DATA_SUCCESS)
            {
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outRecordBuf->bufLen= offset;
                    PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个容器信息报文:", i);
                    containerNum ++;
                    dOffset = 0;
                    sOffset = 0;
                }
                else
                {
                    dOffset = 0;
                    sOffset = 0;
                    isBufFull = TRUE;
                    PRTL_FMT_LOGCC("class11_invoke_get_container isBufFull == TRUE \n");
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
            if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
            {
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x01;                           //末尾帧
                breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
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
            breakInfo.reqType = 0x03;
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x00;                              //不是末尾帧
            breakInfo.objPre = 0x01;                             //对象属性
            if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
            {
                breakInfo.seq = BRK_SEQ_START;                   //首次序号
            }
            else
            {
                breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
            }
        }

        //刷新断点信息
        memcpy(relyInfo.bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

        if (containerNum > 0)
        {
            nRet = DATA_SUCCESS;
            outRecordBuf->recordBuf[containernumoffset] = containerNum;   
            outRecordBuf->bufLen = offset;
        }
        else
        {
            outRecordBuf->bufLen = 0;
            return DATA_READ_WRITE_DENIED;
        }
    }
        break;
    default:
        return DATA_READ_WRITE_DENIED;
    }
    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_container_record
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_container_record(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = inLen;

    /* 类型不存在 */
    if (pOadInfo->pTab->oad.nObjID != 0x7300)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }
    switch (pOadInfo->pTab->oad.attID)
    {
    case 2:
    {
        OOP_PRSD_T oopRsd;
        OOP_RCSD_T oopRcsd;
        uint16 headLen = 0;
        READ_NOR_BP_T inReadNor;
        RELY_INFO_T  relyInfo;
        RECORD_BUF_T outRecordBuf;

        /* 清0处理 */
        MEMZERO(&oopRsd, sizeof(oopRsd));
        MEMZERO(&oopRcsd, sizeof(oopRcsd));
        MEMZERO(&inReadNor, sizeof(inReadNor));
        MEMZERO(&relyInfo, sizeof(relyInfo));
        MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

        outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
        memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

                        /*处理RSD*/
        nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
        PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
        PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
        PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
        PRTL_FMT_LOGCC("offset = %d\n", offset);
        if (nRet != DATA_SUCCESS)
        {
            PRTL_FMT_TRACE("RSD解析错误\n");

            /*应对电科院协议一致性测试*/
            uint16 testOffset = 4;
            uint16 inCsdOff = 0, inCsdLen = 0;
            apdu_next_dpos_get(inData, &testOffset, inLen, DT_RSD, TRUE);
            inCsdOff = testOffset;
            apdu_next_dpos_get(inData, &testOffset, inLen, DT_RCSD, TRUE);
            inCsdLen = testOffset - inCsdOff;
            memcpy(outData->pResultData, inData, 4);
            memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
            outData->resultDataLen = 4 + inCsdLen;

            outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
            if (apdu_rcsd_fromfmt698((uint8 *)inData, inLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
            {
                outData->pResultData[outData->resultDataLen++] = 0;

            }
            else
            {
                outData->pResultData[outData->resultDataLen++] = 1;
                for (i = 0; i < oopRcsd.nNum; i++)
                {
                    outData->pResultData[outData->resultDataLen++] = NULL_DATA;
                }
            }

            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_SUCCESS;
        }
        else
        {
            inDataLenTmp -= offset;  //除去RSD和OAD后的长度
        }
        /*处理RCSD*/
        csdStart = offset;
        if (apdu_rcsd_fromfmt698(inData, inLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
        {
            PRTL_FMT_TRACE("CSD解析错误!\n");
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_TYPE_UNMATCHED;
        }

        /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
        outData->resultDataLen = 0;
        csdLen = offset-csdStart;
        memcpy(outData->pResultData, inData, 4);
        outOffset = 4;
        
        if (csdLen == 1)
        {
            //如果rcsd为空，填充OADF4010200
            outData->pResultData[outOffset ++] = 0x01;
            outData->pResultData[outOffset ++] = 0x00;
            outData->pResultData[outOffset ++] = 0xF4;
            outData->pResultData[outOffset ++] = 0x01;
            outData->pResultData[outOffset ++] = 0x02;
            outData->pResultData[outOffset ++] = 0x00;
        }
        else
        {
            memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
            outOffset += csdLen;
        }
        outData->pResultData[outOffset++] = 1;
        outData->resultDataLen += outOffset;
        headLen = outOffset;
        PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

        /*记录型档案接口提供输入参数*/
        if (read_nor_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
        {
            free(outRecordBuf.recordBuf);
            outRecordBuf.recordBuf = NULL;
            return DATA_READ_WRITE_DENIED;
        }

        /* 依赖信息中存下OAD列表的指针 */
        relyInfo.pOadInfo = pOadInfo;

        PRTL_FMT_LOGCC("****************************************************\n");
        PRTL_FMT_LOGCC("inReadNor.startInfoNum = %d\n", inReadNor.startInfoNum);
        PRTL_FMT_LOGCC("inReadNor.endInfoNum = %d\n", inReadNor.endInfoNum);
        PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
        for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
        {
            if (relyInfo.oopRcsd.cols[i].choice == 0)
            {
                PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                          relyInfo.oopRcsd.cols[i].oad.value);
            }
            else if(relyInfo.oopRcsd.cols[i].choice == 1)
            {
                PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                  relyInfo.oopRcsd.cols[i].road.oadMain.value);
                for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
                {
                    PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                              relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
                }
            }
        }
        PRTL_FMT_LOGCC("****************************************************\n");

        /*读记录档案接口，可以读出apdu数据域，可返回断点信息*/
        if (record_container_read(inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
        {
        	outData->pResultData[outOffset++] =0x00;
			outData->pResultData[outOffset++] =0x00;
            nRet = DATA_SUCCESS;
        }else
        {
            memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
            outData->resultDataLen += outRecordBuf.bufLen;

            if (outRecordBuf.breakInfo.appSlicingFlag == 1)
            {
                /*记下断点信息*/
                PRTL_FMT_LOGCC("记下断点信息\n");
                outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
                memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
            }
            else
            {
                /*清除断点信息*/
                PRTL_FMT_LOGCC("清除断点信息\n");
                memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
            }
            
            nRet = DATA_SUCCESS;
        }
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
    }
        break;
    default:
         return DATA_OBJECT_UNAVAILABLE;
    }
    return nRet;
}
/**
*********************************************************************
* @name：      class11_invoke_get_container
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_container(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
    case 0x06:
        return class11_invoke_get_container_normal(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_container_record(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}
/**
*********************************************************************
* @name：      get_app_info
* @brief：     从系统管理器获取应用信息
* @param[in] ：

* @param[out]：containerinfo    容器信息
* @return    ：容器个数
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
uint8 get_app_info(OOP_APP_INFO_T* appinfo)
{
    int ret;
    uint8 recvbuf[2000]={0};
    uint16 recvlen;
    uint16 index=0;
    uint8 count,i,j;
    uint8 sendbuf[100]={0};
    OOP_CONTAINER_T containerlist[20];
    MEMZERO(containerlist, sizeof(containerlist));
    PRTL_FMT_DEBUG("开始获取所有APP信息get_app_info \n");
    sendbuf[0] = 0;
    sendbuf[1] = 0;//所有容器中的所有app信息
    ret = appmain_send_syncmsg_time(MSG_APPAMIN_GET_APPINFO,sendbuf,2,recvbuf,sizeof(recvbuf),&recvlen, 60);
    if(ret!=ERR_OK)
    {
        PRTL_FMT_DEBUG("获取容器中APP信息失败 \n");
        return 0;
    }
    count = recvbuf[index++];
    //依次获取每个容器内的APP信息
    for(i=0;i<count;i++)
    {
       appinfo[i].appindex = recvbuf[index++];
       appinfo[i].appname.nNum = recvbuf[index++];
       memcpy(appinfo[i].appname.value,&recvbuf[index],appinfo[i].appname.nNum);
       index+= appinfo[i].appname.nNum;
       appinfo[i].containername.nNum = recvbuf[index++];
       memcpy(appinfo[i].containername.value,&recvbuf[index],appinfo[i].containername.nNum);
       index+= appinfo[i].containername.nNum;
       appinfo[i].appversion.nNum = recvbuf[index++];
       memcpy(appinfo[i].appversion.value,&recvbuf[index],appinfo[i].appversion.nNum);
       index+= appinfo[i].appversion.nNum;
       appinfo[i].apphash.nNum = recvbuf[index++];
       memcpy(appinfo[i].apphash.value,&recvbuf[index],appinfo[i].apphash.nNum);
       index+= appinfo[i].apphash.nNum;
       appinfo[i].appservicenum = recvbuf[index++];
       for(j=0;j<appinfo[i].appservicenum;j++)
       {
            appinfo[i].appservicelist[j].serviceindex = recvbuf[index++];
            appinfo[i].appservicelist[j].servicename.nNum = recvbuf[index++];
            memcpy(appinfo[i].appservicelist[j].servicename.value,&recvbuf[index],appinfo[i].appservicelist[j].servicename.nNum);
            index+= appinfo[i].appservicelist[j].servicename.nNum;
            appinfo[i].appservicelist[j].serviceenable = recvbuf[index++];
            appinfo[i].appservicelist[j].servicestate = recvbuf[index++];
            appinfo[i].appservicelist[j].cpulimit = 9000;
            memcpy_r(&appinfo[i].appservicelist[j].cpuusage,&recvbuf[index],sizeof(uint16));
            index+=sizeof(uint16);
            memcpy_r(&appinfo[i].appservicelist[j].memusage,&recvbuf[index],sizeof(uint16));
            index+=sizeof(uint16);
            memcpy_r(&appinfo[i].appservicelist[j].starttime.year,&recvbuf[index],sizeof(uint16));
            index+=sizeof(uint16);
            appinfo[i].appservicelist[j].starttime.month = recvbuf[index++];
            appinfo[i].appservicelist[j].starttime.day = recvbuf[index++];
            appinfo[i].appservicelist[j].starttime.hour = recvbuf[index++];
            appinfo[i].appservicelist[j].starttime.minute = recvbuf[index++];
            appinfo[i].appservicelist[j].starttime.second = recvbuf[index++];
       }
       appinfo[i].appcfg.cpus = recvbuf[index++];
       memcpy_r(&appinfo[i].appcfg.cpulmt,&recvbuf[index],sizeof(uint16));
       index+=sizeof(uint16);
       memcpy_r(&appinfo[i].appcfg.memory,&recvbuf[index],sizeof(uint16));
       index+=sizeof(uint16);
       memcpy_r(&appinfo[i].appcfg.memlmt,&recvbuf[index],sizeof(uint16));
       index+=sizeof(uint16);
    }
    return count;

}

OOP_DAR_E data_to_buf_app_info(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_APP_INFO_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint8 i;
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);
    
    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 7;
    //应用序号
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.appindex, sizeof(data.appindex));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //应用名称
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.appname, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //容器名称
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.containername, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //应用版本
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.appversion, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //应用哈希值
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.apphash, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    dstBuf[dOffset++] = DT_ARRAY;
    dstBuf[dOffset++] = data.appservicenum;
    for(i = 0; i < data.appservicenum; i++)
    {
        dstBuf[dOffset++] = DT_STRUCTURE;
        //结构成员数
        dstBuf[dOffset++] = 8;
        //服务索引
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.appservicelist[i].serviceindex, sizeof(data.appservicelist[i].serviceindex));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        //服务名称
        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.appservicelist[i].servicename, sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        //服务是否使能
        dar = basic_data_to_buf(E_BOOL, dstBuf, dstLen, &dOffset, &data.appservicelist[i].serviceenable, sizeof(data.appservicelist[i].serviceenable));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        //服务状态
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.appservicelist[i].servicestate, sizeof(data.appservicelist[i].servicestate));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        //CPU使用率限制
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.appservicelist[i].cpulimit, sizeof(data.appservicelist[i].cpulimit));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        //CPU使用率
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.appservicelist[i].cpuusage, sizeof(data.appservicelist[i].cpuusage));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        //内存使用率
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.appservicelist[i].memusage, sizeof(data.appservicelist[i].memusage));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        //启动时间
        dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.appservicelist[i].starttime, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
    }
    //应用配置信息
    dstBuf[dOffset++] = DT_STRUCTURE;
    dstBuf[dOffset++] = 4;
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.appcfg.cpus, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.appcfg.cpulmt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.appcfg.memory, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.appcfg.memlmt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

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
* @name：      class11_invoke_get_app_normal
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_app_normal(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{

    uint8 count,i;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32 offset = 0;
    uint32 appNum = 0;
    uint32 appnumoffset;
    OOP_APP_INFO_T applist[20];
    MEMZERO(applist, sizeof(applist));
    DATA_CONVERT_T convert;
    MEMZERO(&convert, sizeof(DATA_CONVERT_T));
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    BP_INFO_T breakInfo;
    MEMZERO(&breakInfo, sizeof(BP_INFO_T));
    uint32    bufAllLen = 0;
    uint32    maxBufLen = 0;
    BOOL      isBufFull = FALSE;

    if(pOadInfo->pTab->oad.value!=0x73010200)
        return DATA_READ_WRITE_DENIED;
    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    PRTL_FMT_DEBUG("class11_invoke_act 应用查询\n");
    count = get_app_info(applist);
    if(count==0)
    {
        PRTL_FMT_DEBUG("获取容器内APP信息失败 \n");
        return DATA_OTHER_REASON;
    }
    PRTL_FMT_DEBUG("app个数 %d \n",count);
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;                             //结果
    outData->pResultData[offset ++] = DT_ARRAY;                         //类型
    appnumoffset = offset;                                        
    outData->pResultData[offset ++] = 0x00;                             //数量，先填0
    bufAllLen = offset;
    for(i=0;i<count;i++)
    {
        if (slicing_is_comtinue_oop_meter(pApduInfo->bpBreakInfo, i))
        {
            continue;
        }   
        //转换成报文数据
        convert.srcBuf = &applist[i];
        convert.srcLen = sizeof(OOP_APP_INFO_T);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outData->pResultData[offset];
        convert.dstLen = outDataMax - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = i;

        nRet = data_to_buf_app_info(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outData->resultDataLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个app信息报文:", i);
                appNum ++;
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_LOGCC("class11_invoke_get_container isBufFull == TRUE \n");
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

    if (appNum > 0)
    {
        nRet = DATA_SUCCESS;
        outData->pResultData[appnumoffset] = appNum;   
        outData->resultDataLen = offset;
    }
    else
    {
        nRet = DATA_SUCCESS;
        outData->pResultData[appnumoffset] = 0;  
        outData->resultDataLen = offset;
    }
    PRTL_BUF_DEBUG(outData->pResultData, offset, "整个app信息报文:");
    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_app_licence
* @brief：     class11的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度
* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_app_licence_normal(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint32 offset = 0;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint8 recvbuf[2048] = {0};
    uint16 recvlen = 0,count = 0,i=0;
    uint16 index = 0;
	uint8 sendbuf[32]={0};
    OOP_APP_LICENSE_ALL_T applicce;
    memset(&applicce,0,sizeof(applicce));
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;                             //结果
    outData->pResultData[offset++] = DT_ARRAY;                         //类型                                       
                                 //数量，先填0

    nRet = appmain_send_syncmsg(MSG_APPAMIN_GET_LISIENSE,sendbuf,1,recvbuf,sizeof(recvbuf),&recvlen);
    if(nRet!=ERR_OK)
    {
        PRTL_FMT_DEBUG("软件注册码获取失败 \n");
        return 0;
    }
    count = recvbuf[index++];
    applicce.num = count;
    for(i=0;i<count;i++)
    {
        applicce.AppLicen[i].appno.nNum = recvbuf[index++];
        memcpy(applicce.AppLicen[i].appno.value,&recvbuf[index],applicce.AppLicen[i].appno.nNum);
        index+=applicce.AppLicen[i].appno.nNum;
        applicce.AppLicen[i].appname.nNum = recvbuf[index++];
        memcpy(applicce.AppLicen[i].appname.value,&recvbuf[index],applicce.AppLicen[i].appname.nNum);
        index+=applicce.AppLicen[i].appname.nNum;
        applicce.AppLicen[i].applicense.nNum = recvbuf[index++];
        memcpy(applicce.AppLicen[i].applicense.value,&recvbuf[index],applicce.AppLicen[i].applicense.nNum);
        index+=applicce.AppLicen[i].applicense.nNum;
    }
    if(0 == nRet)
    {
        outData->pResultData[offset++] = count;                   //数量
        for(i = 0; i < count; i++)
        {
            outData->pResultData[offset++] = DT_STRUCTURE;
            outData->pResultData[offset++] = 3;

            /* 编号 */
            outData->pResultData[offset++] = DT_VISIBLE_STRING;
            outData->pResultData[offset++] = applicce.AppLicen[i].appno.nNum;
            memcpy(&outData->pResultData[offset],applicce.AppLicen[i].appno.value,applicce.AppLicen[i].appno.nNum);
            offset+=applicce.AppLicen[i].appno.nNum;
            /* 名称 */
            outData->pResultData[offset++] = DT_VISIBLE_STRING;
            outData->pResultData[offset++] = applicce.AppLicen[i].appname.nNum;
            memcpy(&outData->pResultData[offset],applicce.AppLicen[i].appname.value,applicce.AppLicen[i].appname.nNum);
            offset+=applicce.AppLicen[i].appname.nNum;

            /* 许可 */
            outData->pResultData[offset++] = DT_OCTET_STRING;
            outData->pResultData[offset++] = applicce.AppLicen[i].applicense.nNum;
            memcpy(&outData->pResultData[offset],applicce.AppLicen[i].applicense.value,applicce.AppLicen[i].applicense.nNum);
            offset+=applicce.AppLicen[i].applicense.nNum;

        }
    }
    else
    {
        outData->pResultData[offset++] = 0;                   //数量
    }
#if 0    
#ifdef PRODUCT_ECU
    uint32 count,i;
    

    char *appNameList[5] = {"smiOS", "mapManager", "wirelessDCM", "dbCenter", "ccoRouter"};
    NOMAL_OAD_T tOad;
    char  nameIndex[5] = {0};
    int         ret = 0;
    uint32      readlen = 0;

    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*)hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        return DATA_READ_WRITE_DENIED;
    }

    count = dev->devinfo_applicense_count(dev);
    PRTL_FMT_DEBUG("获取APP许可数量为%d\n", count);
    if(0 == count)
    {
        outData->resultDataLen = offset;
        hal_device_release((HW_DEVICE *)dev);
        return DATA_SUCCESS;
    }

    memset(&tOad, 0, sizeof(tOad));
    tOad.oad.value = 0x73010500;
    
    for(i = 0; i < count; i++)
    {
        memset(appno, 0, OOP_APP_NO_LEN + 1);
        memset(&appname, 0, sizeof(appname));
        memset(applic, 0, OOP_APP_LICENCE_LEN);
        if(0 != dev->devinfo_read_applicense_index(dev, i, appno, (char *)applic, OOP_APP_LICENCE_LEN))
        {
            continue;
        }
        appNum++;
        memcpy(nameIndex, appno + 10, 4);
        tOad.infoNum = atoi(nameIndex);
        ret = db_read_nomal(pApduInfo->hUdp, &tOad, sizeof(OOP_VISIBLEVAR_T), (uint8*)&appname, &readlen);
        if(0 != ret || readlen != sizeof(OOP_VISIBLEVAR_T))
        {
            if(tOad.infoNum - 1 < 5)
            {
                appname.nNum = strlen(appNameList[tOad.infoNum - 1]);
                memcpy(appname.value, appNameList[tOad.infoNum - 1], appname.nNum);
            }
        }

        /* 返回消息 */
        outData->pResultData[offset++] = DT_STRUCTURE;
        outData->pResultData[offset++] = 3;
        
        outData->pResultData[offset++] = DT_VISIBLE_STRING;
        outData->pResultData[offset++] = strlen(appno);
        memcpy(outData->pResultData + offset, appno, strlen(appno));
        offset += strlen(appno);

        outData->pResultData[offset++] = DT_VISIBLE_STRING;
        outData->pResultData[offset++] = appname.nNum;
        if(appname.nNum > 0)
        {
            memcpy(outData->pResultData + offset, appname.value, appname.nNum);
            offset += appname.nNum;
        }

        outData->pResultData[offset++] = DT_OCTET_STRING;
        outData->pResultData[offset++] = OOP_APP_LICENCE_LEN;
        memcpy(outData->pResultData + offset, applic, OOP_APP_LICENCE_LEN);
        offset += OOP_APP_LICENCE_LEN;
    }

    hal_device_release((HW_DEVICE *)dev);
#else
#endif
#endif
    outData->resultDataLen = offset;
    return DATA_SUCCESS;
}

OOP_DAR_E data_to_buf_licence_info(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_APP_LICENSE_T data;
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
    
    //编号
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.appno, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    
    //名称
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.appname, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    
    //licence
    dar = basic_data_to_buf(E_OOP_OCTETVAR64_T, dstBuf, dstLen, &dOffset, &data.applicense, sizeof(OOP_OCTETVAR64_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
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
* @brief：     记录型读取容器信息
* @param[in]： inReadNor  - 筛选需要的数据

               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_licence_read(const READ_NOR_OCT_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    uint8 count;
    OOP_APP_LICENSE_ALL_T applicce;
    
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo ;                                                //断点信息
    BOOL      isBufFull = FALSE;
    uint32    offset = 0;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 licenceNum = 0;
    uint32 licencenumoffset;
    uint16  index = 0;
    uint8  i;

    uint8 sendbuf[32] = {0};
    uint8 recvbuf[2048] = {0};
    uint16 recvlen = 0;

    MEMZERO(&applicce, sizeof(applicce));
    MEMZERO(&breakInfo, sizeof(breakInfo));
    MEMZERO(&convert, sizeof(convert));

    nRet = appmain_send_syncmsg(MSG_APPAMIN_GET_LISIENSE, sendbuf, 1, recvbuf, sizeof(recvbuf), &recvlen);
    if (nRet!=ERR_OK)
    {
        PRTL_FMT_DEBUG("软件注册码获取失败 \n");
        return 0;
    }

    count = recvbuf[index++];
    applicce.num = count;
    for (i = 0; i < count; i++)
    {
        applicce.AppLicen[i].appno.nNum = recvbuf[index++];
        memcpy(applicce.AppLicen[i].appno.value, &recvbuf[index], applicce.AppLicen[i].appno.nNum);
        index+=applicce.AppLicen[i].appno.nNum;
        applicce.AppLicen[i].appname.nNum = recvbuf[index++];
        memcpy(applicce.AppLicen[i].appname.value, &recvbuf[index], applicce.AppLicen[i].appname.nNum);
        index+=applicce.AppLicen[i].appname.nNum;
        applicce.AppLicen[i].applicense.nNum = recvbuf[index++];
        memcpy(applicce.AppLicen[i].applicense.value, &recvbuf[index], applicce.AppLicen[i].applicense.nNum);
        index+=applicce.AppLicen[i].applicense.nNum;
    }
    
    licencenumoffset = offset;                                         //档案数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;
    
    for(i = 0; i < count; i++)
    {
        //应用分帧处理断点
        if (slicing_is_comtinue_oop_meter(relyInfo.bpBreakInfo, i))
        {
            continue;
        }

        //按条件排查
        if (strstr((char *)inReadNor.octVal.value, applicce.AppLicen[i].appname.value) == NULL)
        {
            continue;
        }
        
        //转换成报文数据
        convert.srcBuf = &applicce.AppLicen[i];
        convert.srcLen = sizeof(OOP_APP_LICENSE_T);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outRecordBuf->recordBuf[offset];
        convert.dstLen = maxBufLen - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = i;

        nRet = data_to_buf_licence_info(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outRecordBuf->bufLen= offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个许可信息报文:", i);
                licenceNum ++;
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_LOGCC("获取许可达到分帧条件 licenceNum[%d]\n", licenceNum);
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
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
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
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(relyInfo.bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    if (licenceNum > 0)
    {
        nRet = DATA_SUCCESS;
        outRecordBuf->recordBuf[licencenumoffset] = licenceNum;   
        outRecordBuf->bufLen = offset;
    }
    else
    {
        outRecordBuf->bufLen = 0;
        return DATA_READ_WRITE_DENIED;
    }

    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_container_record
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_app_licence_record(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = inLen;

    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;
    uint16 headLen = 0;
    READ_NOR_OCT_T inReadNor;
    RELY_INFO_T  relyInfo;
    RECORD_BUF_T outRecordBuf;

    /* 清0处理 */
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    MEMZERO(&inReadNor, sizeof(inReadNor));
    MEMZERO(&relyInfo, sizeof(relyInfo));
    MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

    outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
    memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

    /* 类型不存在 */
    if (pOadInfo->pTab->oad.value != 0x73010500)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    /*处理RSD*/
    nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
    PRTL_FMT_LOGCC("oopRsd.choice = %d\n", oopRsd.choice);
    PRTL_FMT_LOGCC("oopRsd.sel1.oad.value = 0x%08x\n", oopRsd.sel1.oad.value);
    PRTL_FMT_LOGCC("oopRsd.sel1.value.wVal = %d\n", oopRsd.sel1.value.wVal);
    PRTL_FMT_LOGCC("offset = %d\n", offset);
    if (nRet != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RSD解析错误\n");

        /*应对电科院协议一致性测试*/
        uint16 testOffset = 4;
        uint16 inCsdOff = 0, inCsdLen = 0;
        apdu_next_dpos_get(inData, &testOffset, inLen, DT_RSD, TRUE);
        inCsdOff = testOffset;
        apdu_next_dpos_get(inData, &testOffset, inLen, DT_RCSD, TRUE);
        inCsdLen = testOffset - inCsdOff;
        memcpy(outData->pResultData, inData, 4);
        memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
        outData->resultDataLen = 4 + inCsdLen;

        outData->pResultData[outData->resultDataLen++] = 1;                   //后面优化下，outData->resultDataLen与下面统一
        if (apdu_rcsd_fromfmt698((uint8 *)inData, inLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
        {
            outData->pResultData[outData->resultDataLen++] = 0;

        }
        else
        {
            outData->pResultData[outData->resultDataLen++] = 1;
            for (i = 0; i < oopRcsd.nNum; i++)
            {
                outData->pResultData[outData->resultDataLen++] = NULL_DATA;
            }
        }

        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_SUCCESS;
    }
    else
    {
        inDataLenTmp -= offset;  //除去RSD和OAD后的长度
    }

    /*处理RCSD*/
    csdStart = offset;
    if (apdu_rcsd_fromfmt698(inData, inLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("CSD解析错误!\n");
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_TYPE_UNMATCHED;
    }

    /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
    outData->resultDataLen = 0;
    csdLen = offset-csdStart;
    memcpy(outData->pResultData, inData, 4);
    outOffset = 4;
    
    if (csdLen == 1)
    {
        //如果rcsd为空，填充OADF4030200
        outData->pResultData[outOffset ++] = 0x01;
        outData->pResultData[outOffset ++] = 0x00;
        outData->pResultData[outOffset ++] = 0xF4;
        outData->pResultData[outOffset ++] = 0x03;
        outData->pResultData[outOffset ++] = 0x02;
        outData->pResultData[outOffset ++] = 0x00;
    }
    else
    {
        memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
        outOffset += csdLen;
    }
    outData->pResultData[outOffset++] = 1;
    outData->resultDataLen += outOffset;
    headLen = outOffset;
    PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "填写OAD+RCSD+记录结果1 并记下偏移headLen head:");

    /*记录型档案接口提供输入参数*/
    if (read_oct_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
    {
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_READ_WRITE_DENIED;
    }

    /* 依赖信息中存下OAD列表的指针 */
    relyInfo.pOadInfo = pOadInfo;

    PRTL_FMT_LOGCC("****************************************************\n");
    PRTL_FMT_LOGCC("inReadNor.startInfoNum = %d\n", inReadNor.startInfoNum);
    PRTL_FMT_LOGCC("inReadNor.endInfoNum = %d\n", inReadNor.endInfoNum);
    PRTL_FMT_LOGCC("relyInfo.oopRcsd.nNum = %d\n", relyInfo.oopRcsd.nNum);
    for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
    {
        if (relyInfo.oopRcsd.cols[i].choice == 0)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                      relyInfo.oopRcsd.cols[i].oad.value);
        }
        else if(relyInfo.oopRcsd.cols[i].choice == 1)
        {
            PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].choice, 
                                                                                              relyInfo.oopRcsd.cols[i].road.oadMain.value);
            for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
            {
                PRTL_FMT_LOGCC("choice = %d relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, relyInfo.oopRcsd.cols[i].choice, 
                                                                                                          relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
            }
        }
    }
    PRTL_FMT_LOGCC("****************************************************\n");

    /*读记录接口，可以读出apdu数据域，可返回断点信息*/
    if (record_licence_read(inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
    {
        outData->pResultData[outOffset++] =0x00;
        outData->pResultData[outOffset++] =0x00;
        outData->resultDataLen += 2;
        nRet = DATA_SUCCESS;
    }
    else
    {
        memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
        outData->resultDataLen += outRecordBuf.bufLen;

        if (outRecordBuf.breakInfo.appSlicingFlag == 1)
        {
            /*记下断点信息*/
            PRTL_FMT_LOGCC("记下断点信息\n");
            outRecordBuf.breakInfo.objPre = 0x02;   //对象属性，记录型数据
            memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
        }
        else
        {
            /*清除断点信息*/
            PRTL_FMT_LOGCC("清除断点信息\n");
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        }
        
        nRet = DATA_SUCCESS;
    }
    free(outRecordBuf.recordBuf);
    outRecordBuf.recordBuf = NULL;

    return nRet;
}

/**
*********************************************************************
* @name：      class11_invoke_get_container
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_app_licence(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
    case 0x06:
        return class11_invoke_get_app_licence_normal(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class11_invoke_get_app_licence_record(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name：      class11_invoke_get_container
* @brief：     class11的GET服务
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
OOP_DAR_E class11_invoke_get_app(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //如果是应用分帧状态，将apdu[1]修改成起始帧方式
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }
    switch(pOadInfo->pTab->oad.value)
    {
        case 0x73010200: 
        {
            return class11_invoke_get_app_normal(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        }
        case 0x73010500: 
        {
            return class11_invoke_get_app_licence(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        }
        default:
            break;
    }

    return DATA_OBJECT_UNDEFINED;
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
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E data_to_buf_node_topology_comm(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_NodeTopologyInfo1_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //数组类型

    //数组成员个数

        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 4;
                
        //本节点设备地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.NodeDevAddr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
        PRTL_BUF_DEBUG(data.NodeDevAddr.add, OOP_TSA_SIZE-1, "basic_data_to_buf failed. dar(%d)\n",dar);
            return DATA_TYPE_UNMATCHED;
        }
        
//        //本节点相位信息
//    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.NodePhaseInfo, sizeof(uint8));
//        if(dar != DATA_SUCCESS)
//        {
//        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n",dar, data.NodePhaseInfo);
//            return DATA_TYPE_UNMATCHED;
//        }
            //设备类型
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.DevType, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n",dar, data.NodePhaseInfo);
            return DATA_TYPE_UNMATCHED;
        }
        //父节点设备地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.ParentNodeDevAddr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
        PRTL_BUF_DEBUG(data.ParentNodeDevAddr.add, OOP_TSA_SIZE-1, "basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }   

        //父节点线路计量地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.ParentNodeLineAddr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
        PRTL_BUF_DEBUG(data.ParentNodeLineAddr.add, OOP_TSA_SIZE-1, "basic_data_to_buf failed. dar(%d)\n", dar);
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
* @name：       class11_NodeTopology_get
* @brief：      获取节点拓扑信息
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_nodetopology_get_comm(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    OOP_NodeTopologyInfo1_T data;
    uint32    realNum = 0;    //数组由于分帧导致的每帧实际成员数
    uint32    numOffset = 0;  //数组成员数在报文中的位置
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOLEAN   isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
    memset(&data, 0, sizeof(data));
    MEMZERO(&convert, sizeof(convert)); 

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    //读出数据
    PRTL_FMT_DEBUG("read node topology start\n");
    inReadRecord.recordOAD.logicId = 0;

    if(pOadInfo->nIndex == 0)
    {
        inReadRecord.recordOAD.infoNum = 0xffff;
    }
    else
    {
        inReadRecord.recordOAD.infoNum = pOadInfo->nIndex;
    }
    
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;
    stepReadRecord.MAC.nNum = 0;
    if(pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = pApduInfo->bpBreakInfo->tmStartSeg;    
    }
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;                             //结果
    if(pOadInfo->nIndex == 0)
    {
        outData->pResultData[offset ++] = DT_ARRAY;                         //类型
        numOffset = offset;                                                 //数量偏移
        outData->pResultData[offset ++] = 0x00;                             //数量，先填0
    }
    bufAllLen = offset;
    ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("class11_nodetopology_get db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("class11_nodetopology_get db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("class11_nodetopology_get db_read_record_step err = %d!\n", ret);
                continue;
                //数据中心出错返回NULL
            }

            //OAD与返回数据
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_NodeTopologyInfo1_T)))
            {
                PRTL_FMT_DEBUG("class11_nodetopology_get data err !\n");
                PRTL_FMT_DEBUG("class11_nodetopology_get stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("class11_nodetopology_get stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("class11_nodetopology_get stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("class11_nodetopology_get sizeof(OOP_METER_T) = %d\n", 
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

            breakInfo.tmStartSeg = stepRecordOut.infoNum;
            dar = data_to_buf_node_topology_comm(pOadInfo, &convert);
            if(dar == DATA_SUCCESS)
            {                  
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outData->resultDataLen = offset;
                    PRTL_FMT_DEBUG(convert.dstBuf, dOffset, "第%d个物理节点:", i);
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
    PRTL_FMT_DEBUG("read node topology end.\n");
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
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    if(pOadInfo->nIndex == 0)
    {
        outData->pResultData[numOffset] = realNum;   //重新记数量
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       class11_invoke_get_info
* @brief：      获取中文信息
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 王津剑
* @Date      ：2022-7-19
*********************************************************************
*/
OOP_DAR_E class11_invoke_get_info(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint32 offset  = 0;
    uint32 len = 0;  
    uint32 i = 0;

    OOP_CHINESEINFOS_T chinese_info;
    MEMZERO(&chinese_info, sizeof(OOP_CHINESEINFOS_T));

    if(0x80030200 != pOadInfo->pTab->oad.value && 0x80040200 != pOadInfo->pTab->oad.value)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    OOP_SEARCHPARAM_T data;
    MEMZERO(&data, sizeof(data));

    normal.oad.value = pOadInfo->pTab->oad.value;
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_CHINESEINFOS_T), (uint8*)&chinese_info, &len);
    if((ret != ERR_OK) || (len == 0))
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset++] = 0x01;
        outData->pResultData[offset++] = 0x00;
        outData->resultDataLen += offset;
        return DATA_SUCCESS;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;

    //转换成报文数据
    outData->pResultData[offset++] = DT_ARRAY;
    outData->pResultData[offset++] = chinese_info.nNum;
    for(i = 0; i < chinese_info.nNum; i++)
    {
        outData->pResultData[offset++] = DT_STRUCTURE;
        outData->pResultData[offset++] = 4;

        outData->pResultData[offset++] = DT_UNSIGNED;
        outData->pResultData[offset++] = chinese_info.item[i].index;

        outData->pResultData[offset++] = DT_DATETIME_S;
        MEMCPY_R(outData->pResultData + offset, &chinese_info.item[i].time.year, 2);
        offset += 2;
        outData->pResultData[offset++] = chinese_info.item[i].time.month;
        outData->pResultData[offset++] = chinese_info.item[i].time.day;
        outData->pResultData[offset++] = chinese_info.item[i].time.hour;
        outData->pResultData[offset++] = chinese_info.item[i].time.minute;
        outData->pResultData[offset++] = chinese_info.item[i].time.second;

        outData->pResultData[offset++] = DT_BOOL;
        outData->pResultData[offset++] = chinese_info.item[i].bRead;

        outData->pResultData[offset++] = DT_VISIBLE_STRING;
        if(chinese_info.item[i].info.nNum > 128)
        {
            outData->pResultData[offset++] = 0x81;
            outData->pResultData[offset++] = chinese_info.item[i].info.nNum;
        }
        else
        {
            outData->pResultData[offset++] = chinese_info.item[i].info.nNum;
        }
        memcpy(outData->pResultData + offset, chinese_info.item[i].info.value, chinese_info.item[i].info.nNum);
        offset += chinese_info.item[i].info.nNum;
    }
    
    outData->resultDataLen += offset;

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
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E data_to_buf_node_topology(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_NodeTopologyInfo_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //数组类型

    //数组成员个数

        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 4;
                
        //本节点设备地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.NodeDevAddr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
        PRTL_BUF_DEBUG(data.NodeDevAddr.add, OOP_TSA_SIZE-1, "basic_data_to_buf failed. dar(%d)\n",dar);
            return DATA_TYPE_UNMATCHED;
        }
        
        //本节点相位信息
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.NodePhaseInfo, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n",dar, data.NodePhaseInfo);
            return DATA_TYPE_UNMATCHED;
        }
        //父节点设备地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.ParentNodeDevAddr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
        PRTL_BUF_DEBUG(data.ParentNodeDevAddr.add, OOP_TSA_SIZE-1, "basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }   

        //父节点线路计量地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.ParentNodeLineAddr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
        PRTL_BUF_DEBUG(data.ParentNodeLineAddr.add, OOP_TSA_SIZE-1, "basic_data_to_buf failed. dar(%d)\n", dar);
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
* @name：       class11_NodeTopology_get
* @brief：      获取节点拓扑信息
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class11_nodetopology_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    OOP_NodeTopologyInfo_T data;
    uint32    realNum = 0;    //数组由于分帧导致的每帧实际成员数
    uint32    numOffset = 0;  //数组成员数在报文中的位置
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    uint32    maxBufLen = 0;
    BOOLEAN   isBufFull = FALSE;

    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
    memset(&data, 0, sizeof(data));
    MEMZERO(&convert, sizeof(convert)); 

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    //读出数据
    PRTL_FMT_DEBUG("read node topology start\n");
    inReadRecord.recordOAD.logicId = 0;

    if(pOadInfo->nIndex == 0)
    {
        inReadRecord.recordOAD.infoNum = 0xffff;
    }
    else
    {
        inReadRecord.recordOAD.infoNum = pOadInfo->nIndex;
    }
    
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = pOadInfo->pTab->oad.value;
    stepReadRecord.MAC.nNum = 0;
    if(pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        inReadRecord.cStart = pApduInfo->bpBreakInfo->tmStartSeg;    
    }
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;                             //结果
    if(pOadInfo->nIndex == 0)
    {
        outData->pResultData[offset ++] = DT_ARRAY;                         //类型
        numOffset = offset;                                                 //数量偏移
        outData->pResultData[offset ++] = 0x00;                             //数量，先填0
    }
    bufAllLen = offset;
    ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        PRTL_FMT_DEBUG("class11_nodetopology_get db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        PRTL_FMT_DEBUG("class11_nodetopology_get db_read_record_start ok ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
        
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("class11_nodetopology_get db_read_record_step err = %d!\n", ret);
                continue;
                //数据中心出错返回NULL
            }

            //OAD与返回数据
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_NodeTopologyInfo_T)))
            {
                PRTL_FMT_DEBUG("class11_nodetopology_get data err !\n");
                PRTL_FMT_DEBUG("class11_nodetopology_get stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                PRTL_FMT_DEBUG("class11_nodetopology_get stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n", 
                    stepRecordOut.record[0].road.oadCols.oad[0].value);
                PRTL_FMT_DEBUG("class11_nodetopology_get stepRecordOut.record[0].outDatalen = %d\n", 
                    stepRecordOut.record[0].outDatalen);
                PRTL_FMT_DEBUG("class11_nodetopology_get sizeof(OOP_METER_T) = %d\n", 
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

            breakInfo.tmStartSeg = stepRecordOut.infoNum;
            dar = data_to_buf_node_topology(pOadInfo, &convert);
            if(dar == DATA_SUCCESS)
            {                  
                bufAllLen += dOffset;
                if (bufAllLen <= maxBufLen)
                {
                    memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                    offset += dOffset;
                    outData->resultDataLen = offset;
                    PRTL_FMT_DEBUG(convert.dstBuf, dOffset, "第%d个物理节点:", i);
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
    PRTL_FMT_DEBUG("read node topology end.\n");
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
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    if(pOadInfo->nIndex == 0)
    {
        outData->pResultData[numOffset] = realNum;   //重新记数量
    }

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       class11_invoke_get
* @brief：      class11的GET服务
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
OOP_DAR_E class11_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x6000:
        return class11_invoke_get_oop_meter(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6002:
        return class11_invoke_get_searchmeter_res(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6014:
        return class11_invoke_get_normal_scheme(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;  
    case 0x6016:
        return class11_invoke_get_event_scheme(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6018:
        return class11_invoke_get_transparent_scheme(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x601C:
        return class11_invoke_get_report_scheme(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6020:
        return class11_invoke_get_encryption_res(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break; 
    case 0x6032:
        return class11_invoke_get_acq_state(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6034:
        return class11_invoke_get_acq_monitor(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x601A:
        return transparent_result_rec_get(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x601E:
        #if 0
        return class11_invoke_get_acq_rule(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        #else
        return class11_invoke_get_encryption_scheme(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        #endif
        break;
    case 0x7300:
        return class11_invoke_get_container(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x7301:
        return class11_invoke_get_app(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
#ifdef AREA_HUNAN
    case 0x60F1:
        return class11_nodetopology_get(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
#else
    case 0x60F1:
        return class11_nodetopology_get_comm(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
#endif
#ifdef AREA_FUJIAN
    case 0x6E05:
        return class11_invoke_get_group_meter(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
       break;
    case 0x6E11:
        return class11_invoke_get_transparent_task(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6E13:
        return class11_invoke_get_transparent_task_node(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6E15:
        return class11_invoke_get_transparent_results(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6E17:
        return class11_invoke_get_report_plan_fujian(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6E20:
        return class11_invoke_get_transparent_acq_status(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6E23:
        return class11_invoke_get_acq_status(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
    case 0x6E31:
        return class11_invoke_get_transparent_event(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        break;
#endif
    case 0x8003:
    case 0x8004:
        return class11_invoke_get_info(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @brief：      读取冻结记录
* @param[in]： clientfd       - 数据中心句柄
               recordInfo    - 读记录信息
* @param[out]：outRecord      - 输出记录信息
* @return：    =0             - 成功
              <0             - 错误码
*********************************************************************
*/
int transparent_result_read(DB_CLIENT clientfd, RECORD_INFO_T *recordInfo, BUFFER_INFO_T *outRecord)
{
    int ret = 0;
    STEP_READ_RECORD_T stepRead;
    RECORD_UNIT_T      stepOut;
//    READ_RECORD_T readRecord[MAX_SELECTOR2_NUM];
//    uint32 stepHandle[MAX_SELECTOR2_NUM] = 0;  //单步读句柄，最多4轮  
//    uint32 recNum[MAX_SELECTOR2_NUM]     = 0;  //记录条数   
    READ_RECORD_T readRecord;
    uint32 stepHandle = 0;  //单步读句柄，最多4轮  
    uint32 recNum     = 0;  //记录条数   
    uint32 outRecNumAll = 0;  //总的要输出的记录条数
    uint32 ctrlRecNum = 0xffffffff;
    uint32 stepNum = 0; //单步读轮数    
    uint32 offset     = *outRecord->offset;  //输出报文的偏移
    uint32 numOffset  = 0;  //写记录数的偏移    
    uint32 i          = 0;
    uint32 outRecNum  = 0; //实际输出的记录条数
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOLEAN               isBufFull = FALSE;
    uint8 m=0;
    uint8  offlen = 0;
    uint8  lenBuf[3] = {0};
    uint8 *outBuffer = (uint8*)outRecord->buffer;
    OOP_Transparent_InfoNum_LIST_T Transparent_InfoNum;

    /*入参检查*/
    if((recordInfo == NULL) || (outRecord == NULL))
    {
        PRTL_FMT_DEBUG("record_stat_read param check failed. recordInfo(%d), outBuffer(%d)\n", recordInfo, outRecord);
        return ERR_PTR;
    }

    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&stepRead, sizeof(stepRead));
    MEMZERO(&stepOut, sizeof(stepOut));
    MEMZERO(&Transparent_InfoNum,sizeof(OOP_Transparent_InfoNum_LIST_T));

    PRTL_FMT_DEBUG("transparent_result_read\n");
   
    //获取数据中心读取参数
    ret = transparent_read_param_get(recordInfo, &readRecord, &stepNum,&Transparent_InfoNum);
    if(ret == ERR_O_RANGE) //超出范围的OAD
    {
        ctrlRecNum = 0;
    }
    else if(ret != ERR_OK)
    {
        return DATA_TYPE_UNMATCHED;
    }

    PRTL_FMT_TRACE("\r\nreadRecord.recordOAD.logicId = %d, readRecord.recordOAD.infoNum = %d, readRecord.recordOAD.road.oadMain = 0x%08x\
                    \r\nreadRecord.recordOAD.road.oadCols.nNum = %d \n",
                    readRecord.recordOAD.logicId, readRecord.recordOAD.infoNum, 
                    readRecord.recordOAD.road.oadMain, readRecord.recordOAD.road.oadCols.nNum);
    
    for(i = 0; i < readRecord.recordOAD.road.oadCols.nNum; i++)
    {
        PRTL_FMT_TRACE("readRecord.recordOAD.road.oadCols.oad[%d] = %lu(0x%08x)\n", i, readRecord.recordOAD.road.oadCols.oad[i].value, readRecord.recordOAD.road.oadCols.oad[i].value);
    }

    //如果有断点，由于是倒序，则截止到上次中断的时间点
    if(recordInfo->append.bpInfo->appSlicingFlag == 1)
    {
        readRecord.cEnd = recordInfo->append.bpInfo->tmStartSeg;    
    }

    PRTL_FMT_TRACE("readRecord.ti.unit = %d, readRecord.ti.value = %d, readRecord.cType = %d, readRecord.tmStart = %d, readRecord.tmEnd = %d, readRecord.sortType = %d\n", 
        readRecord.ti.unit, readRecord.ti.value,
        readRecord.cType, readRecord.cStart, readRecord.cEnd, readRecord.sortType);

    PRTL_FMT_DEBUG("Transparent_InfoNum.meternum is %d\n",Transparent_InfoNum.meternum);
    for(m=0;m<Transparent_InfoNum.meternum;m++)
    {
        readRecord.recordOAD.infoNum=Transparent_InfoNum.infomun[m].pn;
        memcpy(readRecord.MAC.value,Transparent_InfoNum.infomun[m].add,6);
        readRecord.cStart = Transparent_InfoNum.infomun[m].pn;
        readRecord.cEnd = Transparent_InfoNum.infomun[m].pn;
    
    /*开始读记录*/
    ret = db_read_record_start(clientfd, &readRecord, &stepHandle, &recNum);
    if (ret != ERR_OK)
    {
        PRTL_FMT_LOGCC("db_read_record_start failed. ret(%d), reNum(%d)\n", ret, recNum);
        return ERR_START_REC;
    }
    if(Transparent_InfoNum.PLAN_TRANS_T.value.info[0].flag.bReport==FALSE)
    {
        if(recNum>1)
        {
            recNum=1;
        }
    }
    outRecNumAll = MIN(recNum, ctrlRecNum); 
    PRTL_FMT_TRACE("outRecNumAll(%lu), MIN(recNum(%lu), ctrlRecNum(%lu)\n", outRecNumAll, recNum, ctrlRecNum);

    //MAC校验
    memcpy(&stepRead.MAC, &readRecord.MAC, sizeof(OOP_OCTETVAR64_T));

    for(i = 0; i < 6; i++)
    {
        PRTL_FMT_TRACE("stepRead.MAC[%d] = %02x\n", i, stepRead.MAC.value[i]);
    }

    numOffset = offset;  //记录数或者数组成员数的位置
    
    /*M 条记录 [1] SEQUENCE OF A-RecordRow*/
    outBuffer[offset++] = outRecNumAll;
    
    bufAllLen = *outRecord->offset = offset;

    //由于统计的特殊性，由于1条记录，所以只支持方法9，上第1条
    for(i = 0; i < outRecNumAll; i++)
    {
        ret = db_read_record_step(clientfd, stepHandle, &stepRead, &stepOut);
        if ((ret < ERR_OK) || (stepOut.recordnum == 0))
        {
            PRTL_BUF_DEBUG(stepRead.MAC.value, 6, "db_read_record_step failed. ret(%d), recordnum(%d), mainOAD(0x%08x)\n", 
                ret, stepOut.recordnum, readRecord.recordOAD.road.oadMain.value);
            continue;
        }

        //方法2：判断范围正确性
        if(recordInfo->oopRsd.choice == 2)
        {
            /* 判断是否是所需记录单元 */
            ret = record_data_is_valid(readRecord, stepOut);
            if (ret < ERR_OK)
            {
                continue;
            }  
        }

        //方法9：读取上第n次记录
        if((recordInfo->oopRsd.choice == 9) && (outRecNum+1 != recordInfo->oopRsd.sel9.nLast))
        {
            continue;
        }

        /* 
        A-RecordRow∷=SEQUENCE
        {
         第 1 列数据 Data，
         第 2 列数据 Data，
         …
         第 N 列数据 Data
        }*/        

        bufAllLenLast = *outRecord->offset; //存储上次的长度

        ret = transparent_unit2fmt698(recordInfo, &stepOut, outRecord);
        if (ret != ERR_OK)
        {
            db_read_record_end(clientfd, stepHandle);          
            return ERR_NORMAL;
        } 
        
        bufAllLen = *outRecord->offset;

        if(bufAllLen > outRecord->bufLen) //超长，需要分帧，记下尚未处理的记录序号和尚未处理的时间，如果第一个就超长？
        {
            recordInfo->append.bpInfo->recNumSeg = outRecNum; //未处理的记录序号，同时表示已处理的记录数
            //方法4 5 6 7 8
            switch (readRecord.cType)
            {
                case COL_TM_START:
                    recordInfo->append.bpInfo->tmStartSeg = stepOut.colStartTm;
                    break;
                case COL_TM_END:
                    recordInfo->append.bpInfo->tmStartSeg = stepOut.colEndTm;
                    break;
                case COL_TM_STORE:
                    recordInfo->append.bpInfo->tmStartSeg = stepOut.colStoreTm;
                    break;       
                default:
                    return ERR_PTR;
                break;
            }
            
            isBufFull = TRUE;
            recordInfo->append.bpInfo->appSlicingFlag = 1;

            break;
        }

        outRecNum++;
    }

    /*结束读记录*/
    db_read_record_end(clientfd, stepHandle);

    if (!isBufFull)
    {
        *outRecord->offset = bufAllLen;
        
        if (recordInfo->append.bpInfo->appSlicingFlag == 0x01)
        {
            recordInfo->append.bpInfo->appSlicingFlag = 0x01;
            recordInfo->append.bpInfo->isEnd = 0x01;                          //末尾帧
            recordInfo->append.bpInfo->seq++;   //序号加1
        }
        else
        {
            /*清除断点信息*/
            memset(recordInfo->append.bpInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        *outRecord->offset = bufAllLenLast;
            
        if(recordInfo->append.bpInfo->appSlicingFlag == 0x00)
        {
            recordInfo->append.bpInfo->seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            recordInfo->append.bpInfo->seq++;   //序号加1
        }
        
        recordInfo->append.bpInfo->appSlicingFlag = 0x01;
        recordInfo->append.bpInfo->isEnd = 0x00;                              //不是末尾帧
        break;
    }

        }
    //长度域超过后也要采用A-XDR编码
    if (outRecNum > 0x80)
    {
        offlen = set_len_offset(outRecNum, lenBuf);
        memmove(&((uint8*)outRecord->buffer)[numOffset+offlen], &((uint8*)outRecord->buffer)[numOffset+1], *outRecord->offset - numOffset - 1);
        memcpy(&((uint8*)outRecord->buffer)[numOffset], lenBuf, offlen);
        *outRecord->offset += (offlen - 1); //减掉原来预留的1字节长度
    }
    else
    {
        //填写总记录数 
        ((uint8*)outRecord->buffer)[numOffset] = outRecNum;   
    }      

    return ERR_OK;
}

/**
*********************************************************************
* @name：       frozen_rectable_rec_get
* @brief：      读记录方式获取冻结记录表
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E transparent_result_rec_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E  dar = DATA_SUCCESS;
    int nRet = ERR_OK;
    uint32 offset = 0;   //输入报文buf的偏移
    uint32 csdStart = 0;
    RECORD_INFO_T recordInfo;
    BUFFER_INFO_T outRecord;
    OOP_OAD_U oopOad;
    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;   
    uint16 innerOffset = 0;

    //参数打印
    PRTL_BUF_TRACE(inData, InDataLen, "frozen_rectable_rec_get. oad(0x%08x)\n", pOadInfo->pTab->oad.value);

    //初始化
    MEMZERO(&recordInfo, sizeof(recordInfo));
    MEMZERO(&outRecord, sizeof(outRecord));
    MEMZERO(&oopOad, sizeof(oopOad));
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));

    //报文中提取OAD
    memcpy_r(&oopOad, &inData[offset], sizeof(uint32));
    offset += sizeof(uint32);

    //报文中提取RSD
    dar = apdu_rsd_fromfmt698(inData, InDataLen, &offset, &oopRsd, sizeof(oopRsd));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RSD解析错误\n");
        return dar;
    }

    //报文中提取RCSD
    csdStart = offset;
    dar = apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RCSD解析错误\n");
        return dar;
    }

    if(oopRcsd.nNum == 0)
    {
        //TODO,读所有列
        pOadInfo->noParse = TRUE;
        frozen_all_rcsd_get(pOadInfo, pApduInfo, &innerOffset, (uint8*)&oopRcsd, sizeof(OOP_RCSD_T));
    }

    //先封装记录响应头
    read_record_response(pOadInfo, pApduInfo, inData, InDataLen, offset, csdStart, NULL, outData, outDataMax);

    //分配读取记录的空间
    recordInfo.oopOad = oopOad;
    recordInfo.oopRsd = oopRsd;   
    recordInfo.oopRcsd = oopRcsd;
    recordInfo.append.pOadInfo = pOadInfo;
    recordInfo.append.bpInfo = pApduInfo->bpBreakInfo;
  
    outRecord.buffer = outData->pResultData;  
    outRecord.offset = &outData->resultDataLen;
    outRecord.bufLen = pApduInfo->connectionInfo.ApplSplitLen - outData->resultDataLen - cal_excludeLen(pApduInfo->connectionInfo);

    PRTL_FMT_LOGCC("transparent_result_read\n");
    /*读统计记录表接口，可以读出apdu数据域，可返回断点信息*/
    nRet = transparent_result_read(pApduInfo->hUdp, &recordInfo, &outRecord);
    if (nRet != 0)
    {
        PRTL_FMT_DEBUG("record_tmn_frozen_read failed. nRet(%d)\n", nRet);    
        return nRet;
    }
    
    return dar;
}
/**
*********************************************************************
* @name：      exec_system_cmd
* @brief：     执行system命令
* @param[in] ：buf 要执行的命令

* @param[out]：
* @return    ：0 成功 <0失败
* @author    : 
* @Date      ：
*********************************************************************
*/
uint8 exec_system_cmd_standard(const char* cmd)
{
    int ret;
    uint8 recvbuf[2000]={0};
    uint16 recvlen;
    uint8 sendbuf[500]={0};
    uint16 sendlen=0;
    uint16 index=0;
    uint32 result;
    uint8 lenoffset;
    PRTL_FMT_DEBUG("开始执行system命令 %s\n",cmd);
    sendbuf[sendlen++] = 1;   //一次执行一个
    lenoffset = set_len_offset(strlen(cmd),&sendbuf[sendlen]);
    sendlen+=lenoffset;
    memcpy(&sendbuf[sendlen],cmd,strlen(cmd));
    sendlen+=strlen(cmd);
    ret = appmain_send_syncmsg(MSG_APPAMIN_SYSTEM_CMD,sendbuf,sendlen,recvbuf,sizeof(recvbuf),&recvlen);
    if(ret!=ERR_OK)
    {
        PRTL_FMT_DEBUG("执行system命令失败 \n");
        return -1;
    }
    if(recvbuf[index++]!=1)
        return -1;
    memcpy_r(&result,&recvbuf[index],4);
    if(result==0)
        return 0;
    else
        return -1;
}

/**
*********************************************************************
* @name：      exec_system_cmd
* @brief：     执行system命令
* @param[in] ：buf 要执行的命令

* @param[out]：
* @return    ：0 成功 <0失败
* @author    : 
* @Date      ：
*********************************************************************
*/
uint8 exec_system_cmd(const char* cmd)
{
    //在系统管理器支持这个命令之前，先用交采APP执行system命令
    #if 1
    return system_cmd_sent_to_acmeter(cmd);
    #else
    return exec_system_cmd_standard(cmd);
    #endif
}

/**
*********************************************************************
* @brief：     从698格式报文转结构体参数(创建容器)
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
OOP_DAR_E buf_to_data_config_container(const uint8 *srcBuf, uint16 srcLen,OOP_MODIFY_CONFIG_T * data)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 sOffset = 4;

    MEMZERO(data, sizeof(OOP_CONTAINER_PARAM_T));
    
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

    //容器名称
    dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->con_name, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

        //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if(srcBuf[sOffset++] != 10)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
        //CPU核数
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data->con_config.cpucores, sizeof(data->con_config.cpucores));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

        //CPU监控阈值
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->con_config.cputhreshold, sizeof(data->con_config.cputhreshold));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

        //内存限值
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->con_config.memlimit, sizeof(data->con_config.memlimit));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

            //内存阈值
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->con_config.memthreshold, sizeof(data->con_config.memthreshold));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

            //存储限值
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->con_config.disklimit, sizeof(data->con_config.disklimit));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

            //存储阈值
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->con_config.diskthreshold, sizeof(data->con_config.diskthreshold));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //挂载的宿主机文件夹
    dar = basic_buf_to_data(E_OOP_VISIBLESTR256_T, srcBuf, srcLen, &sOffset, &data->con_config.hostfile, sizeof(OOP_VISIBLEVAR256_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    
    //应用下载文件及路径
    dar = basic_buf_to_data(E_OOP_VISIBLESTR256_T, srcBuf, srcLen, &sOffset, &data->con_config.savefilepath, sizeof(OOP_VISIBLEVAR256_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

	//映射的硬件接口
    dar = basic_buf_to_data(E_OOP_VISIBLESTR256_T, srcBuf, srcLen, &sOffset, &data->con_config.hardinterface, sizeof(OOP_VISIBLEVAR256_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    dar = basic_buf_to_data(E_OOP_VISIBLESTR256_T, srcBuf, srcLen, &sOffset, &data->con_config.portconfig, sizeof(OOP_VISIBLEVAR256_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
        //避免每步都判断，放最后检查，但检查前可能已经越界
    if(sOffset > srcLen)
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }
    
     
    return dar;
}
/**
*********************************************************************
* @brief：     从698格式报文转结构体参数(创建容器)
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
OOP_DAR_E buf_to_data_create_App(const uint8 *srcBuf, uint16 srcLen,OOP_APP_IINSTALL_T * data)
{
    OOP_DAR_E nRet = DATA_SUCCESS;
    uint32 sOffset = 4;

    MEMZERO(data, sizeof(OOP_CONTAINER_PARAM_T));
    
    //结构类型
      if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        return DATA_READ_WRITE_DENIED;
    }
    if(srcBuf[sOffset++] != 3)
    {
        return DATA_READ_WRITE_DENIED;
    }
    nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->containername, sizeof(OOP_VISIBLEVAR_T));
    if (nRet != DATA_SUCCESS)
    {
        return DATA_READ_WRITE_DENIED;
    }
    nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->appname, sizeof(OOP_VISIBLEVAR_T));
    if (nRet != DATA_SUCCESS)
    {
        return DATA_READ_WRITE_DENIED;
    }

	if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        return DATA_READ_WRITE_DENIED;
    }
    if(srcBuf[sOffset++] != 3)
    {
        return DATA_READ_WRITE_DENIED;
    }
    nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->appinfo.appversion, sizeof(data->appinfo.appversion));
    if (nRet != DATA_SUCCESS)
    {
        return DATA_READ_WRITE_DENIED;
    }
	if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        return DATA_READ_WRITE_DENIED;
    }
	if(srcBuf[sOffset++] != 6)
    {
        return DATA_READ_WRITE_DENIED;
    }
	nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->appinfo.fileinfo.name, sizeof(data->appinfo.fileinfo.name));
    if (nRet != DATA_SUCCESS)
    {
        return DATA_READ_WRITE_DENIED;
    }
	nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->appinfo.fileinfo.type, sizeof(data->appinfo.fileinfo.name));
    if (nRet != DATA_SUCCESS)
    {
        return DATA_READ_WRITE_DENIED;
    }
	nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->appinfo.fileinfo.path, sizeof(data->appinfo.fileinfo.path));
    if (nRet != DATA_SUCCESS)
    {
        return DATA_READ_WRITE_DENIED;
    }
	nRet = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->appinfo.fileinfo.size, sizeof(data->appinfo.fileinfo.size));
    if (nRet != DATA_SUCCESS)
    {
        return DATA_READ_WRITE_DENIED;
    }
    
    uint8 checktype = srcBuf[sOffset];
    if(checktype == DT_CHECKSUM)
    {
        nRet = basic_buf_to_data(E_CHECKSUM, srcBuf, srcLen,&sOffset, &data->appinfo.fileinfo.checkcode.checksum, sizeof(data->appinfo.fileinfo.checkcode.checksum));
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }
    }
    else
    {
        nRet = basic_buf_to_data(E_OCTET_STRING, srcBuf, srcLen,&sOffset, &data->appinfo.fileinfo.checkcode.md5, sizeof(data->appinfo.fileinfo.checkcode.md5));
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }
    }
    data->appinfo.fileinfo.checkcode.choice = checktype; 
    
    if(srcBuf[sOffset] ==DT_NULL)
    {
        sOffset++;
    }
    else
    {
    	if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            return DATA_READ_WRITE_DENIED;
        }
        if(srcBuf[sOffset++] != 4)
        {
            return DATA_READ_WRITE_DENIED;
        }
        nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->appinfo.fileinfo.sign.name, sizeof(data->appinfo.fileinfo.sign.name));
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }
    	nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->appinfo.fileinfo.sign.path, sizeof(data->appinfo.fileinfo.sign.path));
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }
    	nRet = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->appinfo.fileinfo.sign.size, sizeof(data->appinfo.fileinfo.sign.size));
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }

        checktype = srcBuf[sOffset];
        if(checktype == DT_CHECKSUM)
        {
            nRet = basic_buf_to_data(E_CHECKSUM, srcBuf, srcLen,&sOffset, &data->appinfo.fileinfo.sign.checkcode.checksum, sizeof(data->appinfo.fileinfo.sign.checkcode.checksum));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
        }
        else
        {
            nRet = basic_buf_to_data(E_OCTET_STRING, srcBuf, srcLen,&sOffset, &data->appinfo.fileinfo.sign.checkcode.md5, sizeof(data->appinfo.fileinfo.sign.checkcode.md5));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
        }
        data->appinfo.fileinfo.sign.checkcode.choice = checktype; 
    }
    if(srcBuf[sOffset] ==DT_NULL)
    {
        sOffset++;
    }
    else
    {
        if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            return DATA_READ_WRITE_DENIED;
        }
        if(srcBuf[sOffset++] != 4)
        {
            return DATA_READ_WRITE_DENIED;
        }
        nRet = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data->appinfo.appconfig.cpus, sizeof(data->appinfo.appconfig.cpus));
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }
        nRet = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->appinfo.appconfig.cpulmt, sizeof(data->appinfo.appconfig.cpulmt));
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }
        nRet = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->appinfo.appconfig.memory, sizeof(data->appinfo.appconfig.memory));
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }
        nRet = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->appinfo.appconfig.memlmt, sizeof(data->appinfo.appconfig.memlmt));
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }
    }
    //避免每步都判断，放最后检查，但检查前可能已经越界
    if(sOffset > srcLen)
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }
    
     
    return nRet;
}


/**
*********************************************************************
* @brief：     从698格式报文转结构体参数(创建容器)
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
OOP_DAR_E buf_to_data_create_container(const uint8 *srcBuf, uint16 srcLen,OOP_CONTAINER_PARAM_T * data)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 sOffset = 4;

    MEMZERO(data, sizeof(OOP_CONTAINER_PARAM_T));
    
    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if(srcBuf[sOffset++] != 5)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //容器名称
    dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->containername, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //应用名称
    dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->appname, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
        //结构类型
    uint16 offset = sOffset;
    if(srcBuf[offset] != DT_NULL)
    {
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
        dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->image_t.imagename, sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->image_t.imagepath, sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
    }else
        {
        sOffset++;
    }
/*    
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
    }*/

    offset = sOffset;
    if(srcBuf[offset]!=DT_NULL)
    {
                        //结构类型              
        if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

            //结构成员数
        if(srcBuf[sOffset++] != 10)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        
        //CPU核数
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data->con_config.cpucores, sizeof(data->con_config.cpucores));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }

            //CPU监控阈值
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->con_config.cputhreshold, sizeof(data->con_config.cputhreshold));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }

            //内存限值
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->con_config.memlimit, sizeof(data->con_config.memlimit));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }

                //内存阈值
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->con_config.memthreshold, sizeof(data->con_config.memthreshold));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }

                //存储限值
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->con_config.disklimit, sizeof(data->con_config.disklimit));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }

                //存储阈值
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->con_config.diskthreshold, sizeof(data->con_config.diskthreshold));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }

        //挂载的宿主机文件夹
        dar = basic_buf_to_data(E_OOP_VISIBLESTR256_T, srcBuf, srcLen, &sOffset, &data->con_config.hostfile, sizeof(OOP_VISIBLEVAR256_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        
        //应用下载文件及路径
        dar = basic_buf_to_data(E_OOP_VISIBLESTR256_T, srcBuf, srcLen, &sOffset, &data->con_config.savefilepath, sizeof(OOP_VISIBLEVAR256_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }

    	//映射的硬件接口
        dar = basic_buf_to_data(E_OOP_VISIBLESTR256_T, srcBuf, srcLen, &sOffset, &data->con_config.hardinterface, sizeof(OOP_VISIBLEVAR256_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        dar = basic_buf_to_data(E_OOP_VISIBLESTR256_T, srcBuf, srcLen, &sOffset, &data->con_config.portconfig, sizeof(OOP_VISIBLEVAR256_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }

    }else
        {
        sOffset++;
    }

    offset = sOffset;
    if(srcBuf[offset]!=DT_NULL)
    {
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
        
        dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->app_info.appversion, sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
         offset = sOffset;
         if(srcBuf[offset]!=DT_NULL)
         {
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
             dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->app_info.fileinfo.name, sizeof(OOP_VISIBLEVAR_T));
             if(dar != DATA_SUCCESS)
             {
                 PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
                 return DATA_TYPE_UNMATCHED;
             }

             dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->app_info.fileinfo.type, sizeof(OOP_VISIBLEVAR_T));
             if(dar != DATA_SUCCESS)
             {
                 PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
                 return DATA_TYPE_UNMATCHED;
             }

             dar = basic_buf_to_data(E_OOP_VISIBLESTR256_T, srcBuf, srcLen, &sOffset, &data->app_info.fileinfo.path, sizeof(OOP_VISIBLEVAR256_T));
             if(dar != DATA_SUCCESS)
             {
                 PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
                 return DATA_TYPE_UNMATCHED;
             }

             dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->app_info.fileinfo.size, sizeof(uint32));
             if(dar != DATA_SUCCESS)
             {
                 PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
                 return DATA_TYPE_UNMATCHED;
             }

//             dar = basic_buf_to_data(E_OOP_OCTETVAR64_T, srcBuf, srcLen, &sOffset, &data->app_info.fileinfo.checkcode.md5, sizeof(OOP_OCTETVAR64_T));
//             if(dar != DATA_SUCCESS)
//             {
//                 PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
//                 return DATA_TYPE_UNMATCHED;
//             }
            uint8 checktype = srcBuf[sOffset];
            if(checktype == DT_CHECKSUM)
            {
                dar = basic_buf_to_data(E_CHECKSUM, srcBuf, srcLen,&sOffset, &data->app_info.fileinfo.checkcode.checksum, sizeof(data->app_info.fileinfo.checkcode.checksum));
                if (dar != DATA_SUCCESS)
                {
                    return DATA_READ_WRITE_DENIED;
                }
            }
            else
            {
                dar = basic_buf_to_data(E_OCTET_STRING, srcBuf, srcLen,&sOffset, &data->app_info.fileinfo.checkcode.md5, sizeof(data->app_info.fileinfo.checkcode.md5));
                if (dar != DATA_SUCCESS)
                {
                    return DATA_READ_WRITE_DENIED;
                }
            }
            data->app_info.fileinfo.checkcode.choice = checktype;
            
            offset=sOffset;
            if(srcBuf[offset]!=DT_NULL)
            {
                if(srcBuf[sOffset++] != DT_STRUCTURE)
                {
                    PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
                    return DATA_TYPE_UNMATCHED;
                }
                        //结构成员数
                if(srcBuf[sOffset++] != 4)
                {
                    PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
                    return DATA_TYPE_UNMATCHED;
                }
                dar = basic_buf_to_data(E_OOP_VISIBLEVAR_T, srcBuf, srcLen, &sOffset, &data->app_info.fileinfo.sign.name, sizeof(OOP_VISIBLEVAR_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
                    return DATA_TYPE_UNMATCHED;
                }
   
                dar = basic_buf_to_data(E_OOP_VISIBLESTR256_T, srcBuf, srcLen, &sOffset, &data->app_info.fileinfo.sign.path, sizeof(OOP_VISIBLEVAR256_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
                    return DATA_TYPE_UNMATCHED;
                }
   
                dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->app_info.fileinfo.sign.size, sizeof(uint32));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
                    return DATA_TYPE_UNMATCHED;
                }
   
//                dar = basic_buf_to_data(E_OOP_OCTETVAR64_T, srcBuf, srcLen, &sOffset, &data->app_info.fileinfo.sign.md5, sizeof(OOP_OCTETVAR64_T));
//                if(dar != DATA_SUCCESS)
//                {
//                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
//                    return DATA_TYPE_UNMATCHED;
//                }
                checktype = srcBuf[sOffset];
                if(checktype == DT_CHECKSUM)
                {
                    dar = basic_buf_to_data(E_CHECKSUM, srcBuf, srcLen,&sOffset, &data->app_info.fileinfo.sign.checkcode.checksum, sizeof(data->app_info.fileinfo.sign.checkcode.checksum));
                    if (dar != DATA_SUCCESS)
                    {
                        return DATA_READ_WRITE_DENIED;
                    }
                }
                else
                {
                    dar = basic_buf_to_data(E_OCTET_STRING, srcBuf, srcLen,&sOffset, &data->app_info.fileinfo.sign.checkcode.md5, sizeof(data->app_info.fileinfo.sign.checkcode.md5));
                    if (dar != DATA_SUCCESS)
                    {
                        return DATA_READ_WRITE_DENIED;
                    }
                }
                data->app_info.fileinfo.sign.checkcode.choice = checktype;
            }else
                {
                sOffset++;
            }
        }else
            {
            sOffset++;
        }
         offset = sOffset;
         if(srcBuf[offset]!=DT_NULL)
         {  
            if(srcBuf[sOffset++] != DT_STRUCTURE)
            {
                PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
                return DATA_TYPE_UNMATCHED;
            }

                //结构成员数
            if(srcBuf[sOffset++] != 4)
            {
                PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
                return DATA_TYPE_UNMATCHED;
            }

            dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data->app_info.appconfig.cpus, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
                return DATA_TYPE_UNMATCHED;
            }
            dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->app_info.appconfig.cpulmt, sizeof(uint16));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
                return DATA_TYPE_UNMATCHED;
            }
     //       PRTL_FMT_DEBUG("come he!!!!! %s\n",srcBuf+sOffset);
            dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->app_info.appconfig.memory, sizeof(uint16));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
                return DATA_TYPE_UNMATCHED;
            }
            dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data->app_info.appconfig.memlmt, sizeof(uint16));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
                return DATA_TYPE_UNMATCHED;
            }
         }else
         {
            sOffset++;
         }
    }else
    {
        sOffset++;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if(sOffset > srcLen)
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }
    
     
    return dar;
}

uint8 appmin_msg_config_container(OOP_MODIFY_CONFIG_T *con,uint8 *sendbuf)
{
    uint16 index=0;
    sendbuf[index++] = con->con_name.nNum;
    memcpy(&sendbuf[index],con->con_name.value,con->con_name.nNum);
    index+=con->con_name.nNum;
    sendbuf[index++] = con->con_config.cpucores;
    memcpy_r(&sendbuf[index],&con->con_config.cputhreshold,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&sendbuf[index],&con->con_config.memlimit,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&sendbuf[index],&con->con_config.memthreshold,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&sendbuf[index],&con->con_config.disklimit,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&sendbuf[index],&con->con_config.diskthreshold,sizeof(uint16));
    index+=sizeof(uint16);
    sendbuf[index++]=con->con_config.hostfile.nNum;
    if(con->con_config.hostfile.nNum!=0)
    {
        memcpy(&sendbuf[index],con->con_config.hostfile.value,con->con_config.hostfile.nNum);
        index+=con->con_config.hostfile.nNum;
    }
    sendbuf[index++]=con->con_config.savefilepath.nNum;
    if(con->con_config.savefilepath.nNum!=0)
    {
        memcpy(&sendbuf[index],con->con_config.savefilepath.value,con->con_config.savefilepath.nNum);
        index+=con->con_config.savefilepath.nNum;
    }
    sendbuf[index++]=con->con_config.hardinterface.nNum;
    if(con->con_config.hardinterface.nNum!=0)
    {
        memcpy(&sendbuf[index],con->con_config.hardinterface.value,con->con_config.hardinterface.nNum);
        index+=con->con_config.hardinterface.nNum;
    }
    sendbuf[index++]=con->con_config.portconfig.nNum;
    if(con->con_config.portconfig.nNum!=0)
    {
        memcpy(&sendbuf[index],con->con_config.portconfig.value,con->con_config.portconfig.nNum);
        index+=con->con_config.portconfig.nNum;
    }
    return index;
}

uint8 appmin_msg_creat_App(OOP_APP_IINSTALL_T *app,uint8 *sendbuf)
{
    uint16 index=0;
    sendbuf[index++] = app->containername.nNum;
    memcpy(&sendbuf[index],app->containername.value,app->containername.nNum);
    index+=app->containername.nNum;
    sendbuf[index++]=app->appname.nNum;
    memcpy(&sendbuf[index],app->appname.value,app->appname.nNum);
    index+=app->appname.nNum;
    sendbuf[index++]=app->appinfo.appversion.nNum;
    memcpy(&sendbuf[index],app->appinfo.appversion.value,app->appinfo.appversion.nNum);
    index+=app->appinfo.appversion.nNum;
    OOP_APPFILE_INFO_T *file = &app->appinfo.fileinfo;
    sendbuf[index++]=file->name.nNum;
    memcpy(&sendbuf[index],file->name.value,file->name.nNum);
    index+=file->name.nNum;
    sendbuf[index++]=file->type.nNum;
    memcpy(&sendbuf[index],file->type.value,file->type.nNum);
    index+=file->type.nNum;
    sendbuf[index++]=file->path.nNum;
    memcpy(&sendbuf[index],file->path.value,file->path.nNum);
    index+=file->path.nNum;
    memcpy_r(&sendbuf[index],&file->size,sizeof(uint32));
    index+=sizeof(uint32);
    
//    sendbuf[index++]=file->md5.nNum;
//    memcpy(&sendbuf[index],file->md5.value,file->md5.nNum);
//    index+=file->md5.nNum;

    if(file->checkcode.choice == DT_OCTET_STRING)
    {
        sendbuf[index++]=file->checkcode.md5.nNum;
        memcpy(&sendbuf[index],file->checkcode.md5.value,file->checkcode.md5.nNum);
        index+=file->checkcode.md5.nNum;
    }
    else if(file->checkcode.choice == DT_CHECKSUM)
    {
        //sendbuf[index++]=file->checkcode.checksum.checktype;
        sendbuf[index++]=file->checkcode.checksum.checkcode.nNum;
        memcpy(&sendbuf[index],file->checkcode.checksum.checkcode.value,file->checkcode.checksum.checkcode.nNum);
        index+=file->checkcode.checksum.checkcode.nNum;
    }
    else
    {
        PRTL_FMT_DEBUG("校验类型错误choice = %d\n", file->checkcode.choice);
        sendbuf[index++] = 0;
    }
    
    OOP_APPFILE_SIGN_T *sign = &file->sign;
    if(sign->name.nNum==0 || sign->path.nNum == 0 || sign->name.value==NULL || sign->path.value == NULL)
    {
        sendbuf[index++] = 0;
    }
    else
    {
        sendbuf[index++] = 1;
        sendbuf[index++]=sign->name.nNum;
        memcpy(&sendbuf,sign->name.value,sign->name.nNum);
        index+=sign->name.nNum;
        sendbuf[index++]=sign->path.nNum;
        memcpy(&sendbuf,sign->path.value,sign->path.nNum);
        index+=sign->path.nNum;
        memcpy_r(&sendbuf,&sign->size,sizeof(uint32));
        index+=sizeof(uint32);
        
//        sendbuf[index++]=sign->md5.nNum;
//        memcpy(&sendbuf,sign->md5.value,sign->md5.nNum);
//        index+=sign->md5.nNum;
        if(sign->checkcode.choice == DT_OCTET_STRING)
        {
            sendbuf[index++]=sign->checkcode.md5.nNum;
            memcpy(&sendbuf[index],sign->checkcode.md5.value,sign->checkcode.md5.nNum);
            index+=sign->checkcode.md5.nNum;
        }
        else if(sign->checkcode.choice == DT_CHECKSUM)
        {
            //sendbuf[index++]=sign->checkcode.checksum.checktype;
            sendbuf[index++]=sign->checkcode.checksum.checkcode.nNum;
            memcpy(&sendbuf[index],sign->checkcode.checksum.checkcode.value,sign->checkcode.checksum.checkcode.nNum);
            index+=sign->checkcode.checksum.checkcode.nNum;
        }
        else
        {
            PRTL_FMT_DEBUG("校验类型错误choice = %d\n", sign->checkcode.choice);
            sendbuf[index++] = 0;
        }
    }
    OOP_APPCFG_INFO_T *appcfg = &app->appinfo.appconfig;
    sendbuf[index++] = appcfg->cpus;
    appcfg->cpulmt = appcfg->cpulmt/100;
    appcfg->memlmt = appcfg->memlmt/100;
    memcpy_r(&sendbuf[index],&appcfg->cpulmt,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&sendbuf[index],&appcfg->memory,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&sendbuf[index],&appcfg->memlmt,sizeof(uint16));
    index+=sizeof(uint16);
    
    return index;
}


uint8 appmin_msg_creat_container(OOP_CONTAINER_PARAM_T *con,uint8 *sendbuf)
{
    uint16 index=0;
    sendbuf[index++] = con->containername.nNum;
    memcpy(&sendbuf[index],con->containername.value,con->containername.nNum);
    index+=con->containername.nNum;
    if(con->appname.nNum == 0 || con->appname.value == NULL)
    {
       sendbuf[index++] = 0; //应用名称     visible-string OPTIONAL 
    }
    else
    {
        sendbuf[index++] = 1;
        sendbuf[index++]=con->appname.nNum;
        memcpy(&sendbuf[index],con->appname.value,con->appname.nNum);
        index+=con->appname.nNum;
    }
    if(con->image_t.imagename.nNum == 0 || con->image_t.imagename.value == NULL || con->image_t.imagepath.nNum == 0 || con->image_t.imagepath.value == NULL)
    {
       sendbuf[index++] = 0; //container-image OPTIONAL 
    }
    else
    {
        sendbuf[index++] = 1;
        sendbuf[index++]=con->image_t.imagename.nNum;
        memcpy(&sendbuf[index],con->image_t.imagename.value,con->image_t.imagename.nNum);
        index+=con->image_t.imagename.nNum;
        sendbuf[index++]=con->image_t.imagepath.nNum;
        memcpy(&sendbuf[index],con->image_t.imagepath.value,con->image_t.imagepath.nNum);
        index+=con->image_t.imagepath.nNum;
    }
    sendbuf[index++] = con->con_config.cpucores;
    memcpy_r(&sendbuf[index],&con->con_config.cputhreshold,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&sendbuf[index],&con->con_config.memlimit,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&sendbuf[index],&con->con_config.memthreshold,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&sendbuf[index],&con->con_config.disklimit,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&sendbuf[index],&con->con_config.diskthreshold,sizeof(uint16));
    index+=sizeof(uint16);
    sendbuf[index++]=con->con_config.hostfile.nNum;
    memcpy(&sendbuf[index],con->con_config.hostfile.value,con->con_config.hostfile.nNum);
    index+=con->con_config.hostfile.nNum;
    sendbuf[index++]=con->con_config.savefilepath.nNum;
    memcpy(&sendbuf[index],con->con_config.savefilepath.value,con->con_config.savefilepath.nNum);
    index+=con->con_config.savefilepath.nNum;
    sendbuf[index++]=con->con_config.hardinterface.nNum;
    memcpy(&sendbuf[index],con->con_config.hardinterface.value,con->con_config.hardinterface.nNum);
    index+=con->con_config.hardinterface.nNum;
    sendbuf[index++]=con->con_config.portconfig.nNum;
    memcpy(&sendbuf[index],con->con_config.portconfig.value,con->con_config.portconfig.nNum);
    index+=con->con_config.portconfig.nNum;
    if(con->appname.nNum == 0 || con->appname.value == NULL || con->app_info.fileinfo.path.nNum == 0 ||  con->app_info.fileinfo.path.value == NULL)
    {
       sendbuf[index++] = 0; //应用信息     app-info  OPTIONAL 
    }
    else
    {
        sendbuf[index++] = 1;
        sendbuf[index++]=con->app_info.appversion.nNum;
        memcpy(&sendbuf[index],con->app_info.appversion.value,con->app_info.appversion.nNum);
        index+=con->app_info.appversion.nNum;
        sendbuf[index++]=con->app_info.fileinfo.name.nNum;
        memcpy(&sendbuf[index],con->app_info.fileinfo.name.value,con->app_info.fileinfo.name.nNum);
        index+=con->app_info.fileinfo.name.nNum;
        sendbuf[index++]=con->app_info.fileinfo.type.nNum;
        memcpy(&sendbuf[index],con->app_info.fileinfo.type.value,con->app_info.fileinfo.type.nNum);
        index+=con->app_info.fileinfo.type.nNum;
        sendbuf[index++]=con->app_info.fileinfo.path.nNum;
        memcpy(&sendbuf[index],con->app_info.fileinfo.path.value,con->app_info.fileinfo.path.nNum);
        index+=con->app_info.fileinfo.path.nNum;
        memcpy_r(&sendbuf[index],&con->app_info.fileinfo.size,sizeof(uint32));
        index+=sizeof(uint32);
        
//        sendbuf[index++]=con->app_info.fileinfo.md5.nNum;
//        memcpy(&sendbuf[index],con->app_info.fileinfo.md5.value,con->app_info.fileinfo.md5.nNum);
//        index+=con->app_info.fileinfo.md5.nNum;

        if(con->app_info.fileinfo.checkcode.choice == DT_OCTET_STRING)
        {
            sendbuf[index++]=con->app_info.fileinfo.checkcode.md5.nNum;
            memcpy(&sendbuf[index],con->app_info.fileinfo.checkcode.md5.value,con->app_info.fileinfo.checkcode.md5.nNum);
            index+=con->app_info.fileinfo.checkcode.md5.nNum;
        }
        else if(con->app_info.fileinfo.checkcode.choice == DT_CHECKSUM)
        {
            //sendbuf[index++]=con->app_info.fileinfo.checkcode.checksum.checktype;
            sendbuf[index++]=con->app_info.fileinfo.checkcode.checksum.checkcode.nNum;
            memcpy(&sendbuf[index],con->app_info.fileinfo.checkcode.checksum.checkcode.value,con->app_info.fileinfo.checkcode.checksum.checkcode.nNum);
            index+=con->app_info.fileinfo.checkcode.checksum.checkcode.nNum;
        }
        else
        {
            PRTL_FMT_DEBUG("校验类型错误choice = %d\n", con->app_info.fileinfo.checkcode.choice);
            sendbuf[index++] = 0;
        }
        if(con->app_info.fileinfo.sign.name.nNum == 0 || con->app_info.fileinfo.sign.path.nNum == 0 || con->app_info.fileinfo.sign.name.value == NULL || con->app_info.fileinfo.sign.path.value == NULL)
        {
            sendbuf[index++] = 0;
        }
        else
        {
            sendbuf[index++] = 1;
            sendbuf[index++]=con->app_info.fileinfo.sign.name.nNum;
            memcpy(&sendbuf[index],con->app_info.fileinfo.sign.name.value,con->app_info.fileinfo.sign.name.nNum);
            index+=con->app_info.fileinfo.sign.name.nNum;
            sendbuf[index++]=con->app_info.fileinfo.sign.path.nNum;
            memcpy(&sendbuf[index],con->app_info.fileinfo.sign.path.value,con->app_info.fileinfo.sign.path.nNum);
            index+=con->app_info.fileinfo.path.nNum;
            memcpy_r(&sendbuf[index],&con->app_info.fileinfo.sign.size,sizeof(uint32));
            index+=sizeof(uint32);
            
//            sendbuf[index++]=con->app_info.fileinfo.sign.md5.nNum;
//            memcpy(&sendbuf[index],con->app_info.fileinfo.sign.md5.value,con->app_info.fileinfo.sign.md5.nNum);
//            index+=con->app_info.fileinfo.sign.md5.nNum;

            if(con->app_info.fileinfo.sign.checkcode.choice == DT_OCTET_STRING)
            {
                sendbuf[index++]=con->app_info.fileinfo.sign.checkcode.md5.nNum;
                memcpy(&sendbuf[index],con->app_info.fileinfo.sign.checkcode.md5.value,con->app_info.fileinfo.sign.checkcode.md5.nNum);
                index+=con->app_info.fileinfo.sign.checkcode.md5.nNum;
            }
            else if(con->app_info.fileinfo.sign.checkcode.choice == DT_CHECKSUM)
            {
                //sendbuf[index++]=con->app_info.fileinfo.sign.checkcode.checksum.checktype;
                sendbuf[index++]=con->app_info.fileinfo.sign.checkcode.checksum.checkcode.nNum;
                memcpy(&sendbuf[index],con->app_info.fileinfo.sign.checkcode.checksum.checkcode.value,con->app_info.fileinfo.sign.checkcode.checksum.checkcode.nNum);
                index+=con->app_info.fileinfo.sign.checkcode.checksum.checkcode.nNum;
            }
            else
            {
                PRTL_FMT_DEBUG("校验类型错误choice = %d\n", con->app_info.fileinfo.sign.checkcode.choice);
                sendbuf[index++] = 0;
            }
        }
       sendbuf[index++] =  con->app_info.appconfig.cpus;
       memcpy_r(&sendbuf[index],&con->app_info.appconfig.cpulmt,sizeof(uint16));
       index+=sizeof(uint16);
       memcpy_r(&sendbuf[index],&con->app_info.appconfig.memory,sizeof(uint16));
       index+=sizeof(uint16);;
       memcpy_r(&sendbuf[index],&con->app_info.appconfig.memlmt,sizeof(uint16));
       index+=sizeof(uint16);;
    }
    return index;
}


/**
*********************************************************************
* @name：      class11_invoke_act_container
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：
*********************************************************************
*/
OOP_DAR_E class11_invoke_act_filegather(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 recvlen = 0;
    uint32 offset = 0,sendlen = 0;
    int result = DATA_OTHER_REASON;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32 sOffset = 4;
    uint8 sendbuf[2048]= {0};
    uint8 recvbuf[100] = {0};
    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }
    
    PRTL_FMT_DEBUG("pOdaInfo->pTab->oad.value is %x\n",pOadInfo->pTab->oad.value);
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x70007F00:
    {
        PRTL_FMT_DEBUG("class11_invoke_act 创建容器\n");
        OOP_CONTAINER_PARAM_T containerparam;
        MEMZERO(&containerparam, sizeof(OOP_CONTAINER_PARAM_T)); 
        nRet = buf_to_data_create_container(inData,inLen,&containerparam);
        if (nRet != DATA_SUCCESS || recvbuf[0] == 0)
        {
            return DATA_READ_WRITE_DENIED;
        }
        sendlen = appmin_msg_creat_container(&containerparam,sendbuf);
        nRet = appmain_send_syncmsg(MSG_APPAMIN_INSTLL_CON,sendbuf,sendlen,recvbuf,sizeof(recvbuf),&recvlen);
        if(nRet!=ERR_OK || recvbuf[0] == 0)
        {
            PRTL_FMT_DEBUG("安装容器失败 \n");
            return DATA_READ_WRITE_DENIED;
        }
#if 0        
        OOP_CONTAINER_T  containerlist[20];
        uint8 count;
        uint8 flag = 0;//标记要添加的容器是否已存在
        count = get_container_info(containerlist);
        for(i=0;i<count;i++)
        {
            if(strcmp(containerlist[i].containername.value,containerparam.containername.value)==0)
            {
                flag = 1;
                break;
            }
        }
        if(flag==1)
        {
            PRTL_FMT_DEBUG("容器已存在 直接安装应用\n");
            sprintf(cmd,"appm -i -c %s -n %s -f /usr/local/default/apps/%s.tar",containerparam.containername.value,
                containerparam.appname.value,containerparam.appname.value);
        }else
        {
            PRTL_FMT_DEBUG("容器不存在 创建容器\n");
            if(containerparam.hardinterface.nNum==0)
            {
                sprintf(cmd,"container install %s %s.tar -disk %s -v /lib/hal_lib:/lib/hal_lib -v /dev:/dev -v /data/app:/data/app -v /sys:/sys -v /tmp/dev:/tmp/dev -dev-dir",
                    containerparam.containername.value,containerparam.appname.value,containerparam.storagelimit.value);
            }else
            {
                sprintf(cmd,"container install %s %s.tar -disk %s -p %s -v /lib/hal_lib:/lib/hal_lib -v /dev:/dev -v /data/app:/data/app -v /sys:/sys -v /tmp/dev:/tmp/dev -dev-dir",
                    containerparam.containername.value,containerparam.appname.value,containerparam.storagelimit.value,containerparam.hardinterface.value);
            }
        }
        PRTL_FMT_DEBUG("cmd %s \n",cmd);
        if(exec_system_cmd(cmd)!=0)
        {
            PRTL_FMT_DEBUG("命令执行失败\n");
            return DATA_OTHER_REASON;
        }
#endif
        result = DATA_SUCCESS;                                             //填写成功标识
        offset = 0;
        memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset ++] = 0x00;                            //DAR type
        outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
        outData->resultDataLen = offset;
    }
        break;
    case 0x73008000:
    {
        OOP_MODIFY_CONFIG_T con_cfg;
        memset(&con_cfg,0,sizeof(con_cfg));
        nRet = buf_to_data_config_container(inData,inLen,&con_cfg);
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }
        sendlen = appmin_msg_config_container(&con_cfg,sendbuf);       
        nRet = appmain_send_syncmsg(MSG_APPAMIN_CONFIG_CON,sendbuf,sendlen,recvbuf,sizeof(recvbuf),&recvlen);
        if(nRet!=ERR_OK || recvbuf[0] == 0)
        {
            PRTL_FMT_DEBUG("容器参数配置失败 \n");
            return DATA_READ_WRITE_DENIED;
        }
        result = DATA_SUCCESS;                                             //填写成功标识
        offset = 0;
        memcpy(&outData->pResultData[offset],inData, sizeof(OOP_OAD_U));  //OMD
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset ++] = 0x00;                            //DAR type
        outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
        outData->resultDataLen = offset;
    }
        break;
    case 0x70008100:
    {
        uint16 index=0,namenum = 0;
        if(inData[sOffset++] != DT_VISIBLE_STRING)
        {
            return DATA_OBJECT_UNDEFINED;
        }
        sendbuf[index++] = 0;
        namenum = inData[sOffset++];
        sendbuf[index++] = namenum;
        memcpy(&sendbuf[index],&inData[sOffset],namenum);
        index+=namenum;
        
        nRet = appmain_send_syncmsg(MSG_APPAMIN_FILE_DELECT,sendbuf,index,recvbuf,sizeof(recvbuf),&recvlen);
        if(nRet==ERR_OK && recvbuf[0]==1)
        {
            return DATA_SUCCESS;
        }
        else
        {
            PRTL_FMT_DEBUG("日志删除失败\n");
            return DATA_READ_WRITE_DENIED;
        }
        
    }
    case 0x73008200:
    case 0x70008300:
    {
        uint16 index = 0;
        OOP_VISIBLEVAR_T tpathname;
        OOP_LOG_INFO_T loginfo;
        MEMZERO(&loginfo, sizeof(loginfo));
        MEMZERO(&tpathname, sizeof(OOP_VISIBLEVAR_T));
        nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, inData, inLen, &sOffset, &tpathname, sizeof(OOP_VISIBLEVAR_T));
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }
        sendbuf[index++] = 0;
        sendbuf[index++] = tpathname.nNum;
        memcpy(&sendbuf[index],tpathname.value,tpathname.nNum);
        index+=tpathname.nNum;
        nRet = appmain_send_syncmsg(MSG_APPAMIN_LOGINFO_GET,sendbuf,index,recvbuf,sizeof(recvbuf),&recvlen);
        if(nRet!=ERR_OK|| recvbuf[0]!=1)
        {
            PRTL_FMT_DEBUG("文件集合中文件信息获取失败 \n");
            return DATA_READ_WRITE_DENIED;
        }
        index=1;
        loginfo.logpathname.nNum = recvbuf[index++];
        if(loginfo.logpathname.nNum==0)
            return DATA_OBJECT_UNDEFINED;
        memcpy(loginfo.logpathname.value,&recvbuf[index],loginfo.logpathname.nNum);
        index+=loginfo.logpathname.nNum;
        memcpy_r(&loginfo.logsize,&recvbuf[index],sizeof(uint32));
        index+=sizeof(uint32);
        if(recvbuf[index++]==1)
        {
//            loginfo.logmd5.nNum = recvbuf[index++];
//            memcpy(loginfo.logmd5.value,&recvbuf[index],loginfo.logmd5.nNum);
//            index+=loginfo.logmd5.nNum;   
            int checkNum = recvbuf[index++];
            if(checkNum == 32)   //当前处理 若校验长度为32位 默认为SM3编码
            {
                loginfo.logcheck.choice = DT_CHECKSUM;
                loginfo.logcheck.checksum.checktype = 3;  //SM3编码
                loginfo.logcheck.checksum.checkcode.nNum = checkNum;
                memcpy(loginfo.logcheck.checksum.checkcode.value,&recvbuf[index],checkNum);
                index += checkNum;
            }
            else
            {
                loginfo.logcheck.choice = DT_OCTET_STRING;
                loginfo.logcheck.md5.nNum = checkNum;
                memcpy(loginfo.logcheck.md5.value,&recvbuf[index],checkNum);
                index += checkNum;
            }
            
        }
        memcpy_r(&loginfo.creattime.year,&recvbuf[index],sizeof(uint16));
        index+=sizeof(uint16);
        loginfo.creattime.month = recvbuf[index++];
        loginfo.creattime.day = recvbuf[index++];
        loginfo.creattime.hour = recvbuf[index++];
        loginfo.creattime.minute = recvbuf[index++];
        loginfo.creattime.second = recvbuf[index++];
        loginfo.logtype = recvbuf[index++];
        uint8 option = recvbuf[index++];
        if(option != 0)
        {
         loginfo.conname.nNum = recvbuf[index++];
         memcpy(loginfo.conname.value,&recvbuf[index],loginfo.conname.nNum);
         index+=loginfo.conname.nNum;
        }
        option = recvbuf[index++];
        if(option != 0)
        {
         loginfo.appname.nNum = recvbuf[index++];
         memcpy(loginfo.appname.value,&recvbuf[index],loginfo.appname.nNum);
         index+=loginfo.appname.nNum;
        }
        
        result = DATA_SUCCESS;                                             //填写成功标识
        index = 0;
        memcpy(&outData->pResultData[index], inData, sizeof(OOP_OAD_U));  //OMD
        index += sizeof(OOP_OAD_U);
        outData->pResultData[index ++] = 0x00;                            //DAR type
        outData->pResultData[index ++] = 0x01;                            //DATA:NULL
        outData->pResultData[index ++] = DT_STRUCTURE;
        outData->pResultData[index ++] = 5;
        outData->pResultData[index ++] = DT_VISIBLE_STRING;
        outData->pResultData[index ++] = loginfo.logpathname.nNum;
        memcpy(&outData->pResultData[index],loginfo.logpathname.value,loginfo.logpathname.nNum);
        index+=loginfo.logpathname.nNum;
        outData->pResultData[index ++] = DT_DOUBLE_LONG_UNSIGNED;
        //loginfo.logsize= loginfo.logsize*1024;
        memcpy_r(&outData->pResultData[index],&loginfo.logsize,sizeof(uint32));
        index+=sizeof(uint32);
        outData->pResultData[index ++] = DT_DATETIME_S;
        memcpy_r(&outData->pResultData[index],&loginfo.creattime.year,sizeof(uint16));
        index+=sizeof(uint16);
        outData->pResultData[index ++] = loginfo.creattime.month;
        outData->pResultData[index ++] = loginfo.creattime.day;
        outData->pResultData[index ++] = loginfo.creattime.hour;
        outData->pResultData[index ++] = loginfo.creattime.minute;
        outData->pResultData[index ++] = loginfo.creattime.second;
        outData->pResultData[index ++] = DT_DATETIME_S;
        memcpy_r(&outData->pResultData[index],&loginfo.creattime.year,sizeof(uint16));
        index+=sizeof(uint16);
        outData->pResultData[index ++] = loginfo.creattime.month;
        outData->pResultData[index ++] = loginfo.creattime.day;
        outData->pResultData[index ++] = loginfo.creattime.hour;
        outData->pResultData[index ++] = loginfo.creattime.minute;
        outData->pResultData[index ++] = loginfo.creattime.second;

//        outData->pResultData[index ++] = DT_OCTET_STRING;
//        if(loginfo.logmd5.nNum != 0)
//        {
//            outData->pResultData[index ++] = loginfo.logmd5.nNum;
//            memcpy(&outData->pResultData[index],loginfo.logmd5.value,loginfo.logmd5.nNum);
//            index+=loginfo.logmd5.nNum;
//        }
//        else
//        {
//            outData->pResultData[index ++] = loginfo.logmd5.nNum;
//        }

        if(loginfo.logcheck.choice == DT_CHECKSUM)
        {
            outData->pResultData[index++] = DT_CHECKSUM;
            
            if(loginfo.logcheck.checksum.checkcode.nNum != 0)
            {
                outData->pResultData[index++] = loginfo.logcheck.checksum.checktype;
                outData->pResultData[index++] = loginfo.logcheck.checksum.checkcode.nNum;
                memcpy(&outData->pResultData[index],loginfo.logcheck.checksum.checkcode.value,loginfo.logcheck.checksum.checkcode.nNum);
                index+=loginfo.logcheck.checksum.checkcode.nNum;
                PRTL_BUF_DEBUG(loginfo.logcheck.checksum.checkcode.value, loginfo.logcheck.checksum.checkcode.nNum,"文件集合中文件信息SM3编码:");
            }
            else
            {
                outData->pResultData[index++] = loginfo.logcheck.checksum.checktype;
                outData->pResultData[index++] = loginfo.logcheck.checksum.checkcode.nNum;
            }
        }
        else
        {
            outData->pResultData[index++] = DT_OCTET_STRING;
            
            if(loginfo.logcheck.md5.nNum != 0)
            {
                outData->pResultData[index++] = loginfo.logcheck.md5.nNum;
                memcpy(&outData->pResultData[index],loginfo.logcheck.md5.value,loginfo.logcheck.md5.nNum);
                index+=loginfo.logcheck.md5.nNum;
            }
            else
            {
                outData->pResultData[index++] = loginfo.logcheck.md5.nNum;
            }
        }
        outData->resultDataLen = index;
    }
        break;
    default:
        return DATA_READ_WRITE_DENIED;
    }
    return result;

}


/**
*********************************************************************
* @name：      class11_invoke_act_container
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：
*********************************************************************
*/
OOP_DAR_E class11_invoke_act_container(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 recvlen = 0;
    uint32 offset = 0,sendlen = 0;
    int result = DATA_OTHER_REASON;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32 sOffset = 4;
    uint8 sendbuf[2048]= {0};
    uint8 recvbuf[100] = {0};
    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }
    
    PRTL_FMT_DEBUG("pOdaInfo->pTab->oad.value is %x\n",pOadInfo->pTab->oad.value);
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x73007F00:
    {
        PRTL_FMT_DEBUG("class11_invoke_act 创建容器\n");
        OOP_CONTAINER_PARAM_T containerparam;
        MEMZERO(&containerparam, sizeof(OOP_CONTAINER_PARAM_T)); 
        nRet = buf_to_data_create_container(inData,inLen,&containerparam);
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }
        sendlen = appmin_msg_creat_container(&containerparam,sendbuf);
        nRet = appmain_send_syncmsg_time(MSG_APPAMIN_INSTLL_CON,sendbuf,sendlen,recvbuf,sizeof(recvbuf),&recvlen,25);
        if(nRet!=ERR_OK || recvbuf[0] == 0)
        {
            PRTL_FMT_DEBUG("安装容器失败 \n");
            return DATA_READ_WRITE_DENIED;
        }
#if 0        
        OOP_CONTAINER_T  containerlist[20];
        uint8 count;
        uint8 flag = 0;//标记要添加的容器是否已存在
        count = get_container_info(containerlist);
        for(i=0;i<count;i++)
        {
            if(strcmp(containerlist[i].containername.value,containerparam.containername.value)==0)
            {
                flag = 1;
                break;
            }
        }
        if(flag==1)
        {
            PRTL_FMT_DEBUG("容器已存在 直接安装应用\n");
            sprintf(cmd,"appm -i -c %s -n %s -f /usr/local/default/apps/%s.tar",containerparam.containername.value,
                containerparam.appname.value,containerparam.appname.value);
        }else
        {
            PRTL_FMT_DEBUG("容器不存在 创建容器\n");
            if(containerparam.hardinterface.nNum==0)
            {
                sprintf(cmd,"container install %s %s.tar -disk %s -v /lib/hal_lib:/lib/hal_lib -v /dev:/dev -v /data/app:/data/app -v /sys:/sys -v /tmp/dev:/tmp/dev -dev-dir",
                    containerparam.containername.value,containerparam.appname.value,containerparam.storagelimit.value);
            }else
            {
                sprintf(cmd,"container install %s %s.tar -disk %s -p %s -v /lib/hal_lib:/lib/hal_lib -v /dev:/dev -v /data/app:/data/app -v /sys:/sys -v /tmp/dev:/tmp/dev -dev-dir",
                    containerparam.containername.value,containerparam.appname.value,containerparam.storagelimit.value,containerparam.hardinterface.value);
            }
        }
        PRTL_FMT_DEBUG("cmd %s \n",cmd);
        if(exec_system_cmd(cmd)!=0)
        {
            PRTL_FMT_DEBUG("命令执行失败\n");
            return DATA_OTHER_REASON;
        }
#endif
        result = DATA_SUCCESS;                                             //填写成功标识
        offset = 0;
        memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset ++] = 0x00;                            //DAR type
        outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
        outData->resultDataLen = offset;
    }
        break;
    case 0x73008000:
    {
        OOP_MODIFY_CONFIG_T con_cfg;
        memset(&con_cfg,0,sizeof(con_cfg));
        nRet = buf_to_data_config_container(inData,inLen,&con_cfg);
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }
        sendlen = appmin_msg_config_container(&con_cfg,sendbuf);       
        nRet = appmain_send_syncmsg_time(MSG_APPAMIN_CONFIG_CON,sendbuf,sendlen,recvbuf,sizeof(recvbuf),&recvlen,15);
        if(nRet!=ERR_OK || recvbuf[0] == 0)
        {
            PRTL_FMT_DEBUG("容器参数配置失败 \n");
            return DATA_READ_WRITE_DENIED;
        }
        result = DATA_SUCCESS;                                             //填写成功标识
        offset = 0;
        memcpy(&outData->pResultData[offset],inData, sizeof(OOP_OAD_U));  //OMD
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset ++] = 0x00;                            //DAR type
        outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
        outData->resultDataLen = offset;
    }
        break;
    case 0x73008100:
    case 0x73008200:
    case 0x73008300:
    {
        uint16 index = 0;
        OOP_VISIBLEVAR_T containername;
        MEMZERO(&containername, sizeof(OOP_VISIBLEVAR_T));
        nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, inData, inLen, &sOffset, &containername, sizeof(OOP_VISIBLEVAR_T));
        if (nRet != DATA_SUCCESS)
        {
            return DATA_READ_WRITE_DENIED;
        }
        sendbuf[index++] = containername.nNum;
        memcpy(&sendbuf[index],containername.value,containername.nNum);
        index+=containername.nNum;
        if(pOadInfo->pTab->oad.value ==0x73008100 )
        {
            sendbuf[index++] = 0;
        }
        else if(pOadInfo->pTab->oad.value ==0x73008200 )
        {
            sendbuf[index++] = 1;
        }
        else if(pOadInfo->pTab->oad.value ==0x73008300 )
        {
            sendbuf[index++] = 2;
        }
        nRet = appmain_send_syncmsg_time(MSG_APPAMIN_EXEC_CON,sendbuf,index,recvbuf,sizeof(recvbuf),&recvlen, 15);
        if(nRet!=ERR_OK || recvbuf[0] == 0)
        {
            PRTL_FMT_DEBUG("容器执行失败 \n");
            return DATA_READ_WRITE_DENIED;
        }
        result = DATA_SUCCESS;                                             //填写成功标识
        offset = 0;
        memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset ++] = 0x00;                            //DAR type
        outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
        outData->resultDataLen = offset;
    }
        break;
    default:
        return DATA_READ_WRITE_DENIED;
    }
    return result;

}
/**
*********************************************************************
* @name：      class11_invoke_act_app
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：
*********************************************************************
*/
OOP_DAR_E class11_invoke_act_app(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{

    uint32 offset = 0;
    int result = DATA_OTHER_REASON;
    OOP_DAR_E nRet = DATA_OTHER_REASON;

    uint32 sOffset = 4;
    uint16 recvlen=0,sendlen = 0;
    uint8 sendbuf[2048]={0};
    uint8 recvbuf[100]={0};
    
    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }
    
    PRTL_FMT_DEBUG("pOdaInfo->pTab->oad.value is %x\n",pOadInfo->pTab->oad.value);

    switch(pOadInfo->pTab->oad.value)
    {
        case 0x73017f00:
        case 0x73018500:
        {
            OOP_APP_IINSTALL_T App_info;;
            MEMZERO(&App_info, sizeof(App_info));           
            PRTL_FMT_DEBUG("class11_invoke_act 创建APP\n");
            nRet = buf_to_data_create_App(inData,inLen,&App_info);
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            sendlen = appmin_msg_creat_App(&App_info,sendbuf);
            nRet = appmain_send_syncmsg_time(MSG_APPMAIN_INSTALL_APP,sendbuf,sendlen,recvbuf,sizeof(recvbuf),&recvlen,15);
            if(nRet!=ERR_OK || recvbuf[0] == 0)
            {
                PRTL_FMT_DEBUG("安装App失败 \n");
                return DATA_READ_WRITE_DENIED;
            }

            result = DATA_SUCCESS;                                             //填写成功标识
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
            break;
        case 0x73018000://启动
        case 0x73018100://停止
        case 0x73018200://卸载
        case 0x73018600://使能
        case 0x73018700://去使能
        {
            OOP_VISIBLEVAR_T containername, appname;
            MEMZERO(&containername, sizeof(containername));
            MEMZERO(&appname, sizeof(appname));
            if(inData[sOffset++]!=DT_STRUCTURE)
                return DATA_READ_WRITE_DENIED;
            if(inData[sOffset++]!=2)
                return DATA_READ_WRITE_DENIED;
            nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, inData, inLen, &sOffset, &containername, sizeof(OOP_VISIBLEVAR_T));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, inData, inLen, &sOffset, &appname, sizeof(OOP_VISIBLEVAR_T));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            uint8 type;
            if(pOadInfo->pTab->oad.value ==0x73018000)
            {
                type = 0;
                PRTL_FMT_DEBUG("启动APP %s\n", appname.value);
            }
            else if(pOadInfo->pTab->oad.value ==0x73018100)
            {
                type = 1;
                PRTL_FMT_DEBUG("停止APP %s\n", appname.value);
            }
            else if(pOadInfo->pTab->oad.value ==0x73018200)
            {
                type = 2;
                PRTL_FMT_DEBUG("卸载APP %s\n", appname.value);
            }
            else if(pOadInfo->pTab->oad.value ==0x73018600)
            {
                type = 1;
                PRTL_FMT_DEBUG("使能APP %s\n", appname.value);
            }
            else if(pOadInfo->pTab->oad.value ==0x73018700)
            {
                type = 2;
                PRTL_FMT_DEBUG("去使能APP %s\n", appname.value);
            }
            uint16 index=0;
            sendbuf[index++] = containername.nNum;
            memcpy(&sendbuf[index],containername.value,containername.nNum);
            index+=containername.nNum;
            sendbuf[index++] = appname.nNum;
            memcpy(&sendbuf[index],appname.value,appname.nNum);
            index+=appname.nNum;
            sendbuf[index++] = type;
            sendlen = index;
            nRet = appmain_send_syncmsg_time(MSG_APPAMIN_EXEC_APP,sendbuf,sendlen,recvbuf,sizeof(recvbuf),&recvlen,15);
            if(nRet!=ERR_OK || recvbuf[0] == 0)
            {
                PRTL_FMT_DEBUG("操作App失败 \n");
                return DATA_READ_WRITE_DENIED;
            }

            result = DATA_SUCCESS;                                             //填写成功标识
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }
            break;
        case 0x73018300://更改应用资源配置
        {
            OOP_VISIBLEVAR_T containername,appname;
            uint8  cpus;
            uint16 cpulimit, memory, memlimit,index=0;
            MEMZERO(&containername, sizeof(OOP_VISIBLEVAR_T));
            MEMZERO(&appname, sizeof(OOP_VISIBLEVAR_T));
            
            if(inData[sOffset++] != DT_STRUCTURE)
            {
                return DATA_READ_WRITE_DENIED;
            }
            if(inData[sOffset++] != 3)
            {
                return DATA_READ_WRITE_DENIED;
            }
            nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, inData, inLen, &sOffset, &containername, sizeof(OOP_VISIBLEVAR_T));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            nRet = basic_buf_to_data(E_OOP_VISIBLEVAR_T, inData, inLen, &sOffset, &appname, sizeof(OOP_VISIBLEVAR_T));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }

            if(inData[sOffset++] != DT_STRUCTURE)
            {
                return DATA_READ_WRITE_DENIED;
            }
            if(inData[sOffset++] != 4)
            {
                return DATA_READ_WRITE_DENIED;
            }
            nRet = basic_buf_to_data(E_UNSIGNED, inData, inLen, &sOffset, &cpus, sizeof(cpus));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            nRet = basic_buf_to_data(E_LONG_UNSIGNED, inData, inLen, &sOffset, &cpulimit, sizeof(cpulimit));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            cpulimit = cpulimit/100;
            nRet = basic_buf_to_data(E_LONG_UNSIGNED, inData, inLen, &sOffset, &memory, sizeof(memory));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            nRet = basic_buf_to_data(E_LONG_UNSIGNED, inData, inLen, &sOffset, &memlimit, sizeof(memlimit));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            memlimit = memlimit/100;
            sendbuf[index++] = containername.nNum;
            memcpy(&sendbuf[index],containername.value,containername.nNum);
            index+=containername.nNum;
            sendbuf[index++] = appname.nNum;
            memcpy(&sendbuf[index],appname.value,appname.nNum);
            index+=appname.nNum;
            sendbuf[index++] = cpus;
            memcpy_r(&sendbuf[index],&cpulimit,sizeof(uint16));
            index+=sizeof(uint16);
            memcpy_r(&sendbuf[index],&memory,sizeof(uint16));
            index+=sizeof(uint16);
            memcpy_r(&sendbuf[index],&memlimit,sizeof(uint16));
            index+=sizeof(uint16);
            sendlen = index;            
            nRet = appmain_send_syncmsg_time(MSG_APPAMIN_CONFIG_APP,sendbuf,sendlen,recvbuf,sizeof(recvbuf),&recvlen,15);
            if(nRet!=ERR_OK || recvbuf[0] == 0)
            {
                PRTL_FMT_DEBUG("参数配置App失败 \n");
                return DATA_READ_WRITE_DENIED;
            }
            result = DATA_SUCCESS;                                             //填写成功标识
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
        }break;
        case 0x73018400:
        {
            /* 设置软件许可 */
            result = class11_invoke_act_app_liecnce(pOadInfo, pApduInfo, inData, inLen,&sendlen,sendbuf);
            if(DATA_SUCCESS != result)
            {
                return result;
            }
            nRet = appmain_send_syncmsg_time(MSG_APPAMIN_SETLISIENSE_APP,sendbuf,sendlen,recvbuf,sizeof(recvbuf),&recvlen,15);
            if(nRet!=ERR_OK || recvbuf[0] == 0)
            {
                PRTL_FMT_DEBUG("参数配置App失败 \n");
                return DATA_READ_WRITE_DENIED;
            }
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset++] = 0x00;                            //DAR type
            outData->pResultData[offset++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
            break;
        }
        default:
            return DATA_READ_WRITE_DENIED;
    }
    return result;
}

#if DESC("软件摘要信息", 1)

/**
*********************************************************************
* @brief：     从698格式报文转结构体参数
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
OOP_DAR_E buf_to_data_soft_summary_req(const uint8 *srcBuf, uint16 srcLen, OOP_SOFT_SUMMARYS_REQ *data)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32    sOffset = 4;
    uint32    i = 0;
    uint8     offlen = 0;      //数组数量长度

    //数组类型
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组成员个数
    data->nNum = get_len_offset((uint8 *)&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    for(i = 0; i < data->nNum; i++)
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
        
        //cpu编号
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data->item[i].cpuNo, sizeof(data->item[i].cpuNo));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        
        //名称
        dar = basic_buf_to_data(E_OOP_VISIBLESTR256_T, srcBuf, srcLen, &sOffset, &data->item[i].name, sizeof(data->item[i].name));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
    }
    
    //避免每步都判断，放最后检查，但检查前可能已经越界
    if(sOffset > srcLen)
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }
     
    return dar;
}

/**
*********************************************************************
* @name：      class11_invoke_act_soft_summary
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：
*********************************************************************
*/
OOP_DAR_E class11_invoke_act_soft_summary(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int       ret = 0;
    uint32    i   = 0;
    uint32    offset  = 0;
    OOP_DAR_E dar = DATA_SUCCESS;
    
    uint8 *pAddr = NULL;
    uint8  addrLen = 0;

    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码

    OOP_SOFT_SUMMARYS_REQ req;
    OOP_SOFT_SUMMARYS_RES res;
        
    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }

    if (pOadInfo->pTab->oad.value != 0x73028100)
    {
        PRTL_FMT_DEBUG("接口调用错误 oad[0x%08X]\n", pOadInfo->pTab->oad.value);
        return DATA_OBJECT_UNDEFINED;
    }

    MEMZERO(&req, sizeof(OOP_SOFT_SUMMARYS_REQ));
    MEMZERO(&res, sizeof(OOP_SOFT_SUMMARYS_RES));

    addrLen = app_get_localaddr(&pAddr);
    if (addrLen == 0)
    {
        PRTL_FMT_DEBUG("获取地址失败 oad[0x%08X]\n", pOadInfo->pTab->oad.value);
        return DATA_TYPE_UNMATCHED;
    }

    dar = buf_to_data_soft_summary_req(inData, inLen, &req);
    if (dar != DATA_SUCCESS)
    {
        return dar;
    }

    ret = sec_get_soft_summary(pAddr, addrLen, &req, &res);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("获取摘要失败 ret[%d], oad[0x%08X]\n", ret, pOadInfo->pTab->oad.value);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy_r(&outData->pResultData[offset], &pOadInfo->pTab->oad.value, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 0x00;                            //DAR type
    outData->pResultData[offset++] = 0x01;                            //DATA
    outData->pResultData[offset++] = DT_ARRAY;
    outData->pResultData[offset++] = res.nNum;
    for (i = 0; i < res.nNum; i++)
    {
        outData->pResultData[offset++] = DT_STRUCTURE;
        outData->pResultData[offset++] = 3;
        outData->pResultData[offset++] = DT_UNSIGNED;
        outData->pResultData[offset++] = res.item[i].cpuNo;
        outData->pResultData[offset++] = DT_VISIBLE_STRING;
        outData->pResultData[offset++] = res.item[i].name.nNum;
        memcpy(&outData->pResultData[offset], res.item[i].name.value, res.item[i].name.nNum);
        offset += res.item[i].name.nNum;
        outData->pResultData[offset++] = DT_OCTET_STRING;
        offlen = set_len_offset(res.item[i].data.nNum, lenBuf);
        memcpy(&outData->pResultData[offset], lenBuf, offlen);
        offset += offlen;
        memcpy(&outData->pResultData[offset], res.item[i].data.value, res.item[i].data.nNum);
        offset += res.item[i].data.nNum;
    }

    outData->resultDataLen = offset;

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：      class11_invoke_act_7302
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：
*********************************************************************
*/
OOP_DAR_E class11_invoke_act_7302(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    switch(pOadInfo->pTab->oad.value)
    {
        case 0x73028100:
            return class11_invoke_act_soft_summary(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
        default:
            return DATA_OBJECT_UNDEFINED;
    }
    return DATA_OTHER_REASON;
}
#endif

/**
*********************************************************************
* @name：      class11_invoke_act_info
* @brief：     class11的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：
*********************************************************************
*/
OOP_DAR_E class11_invoke_act_info(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint32 offset = 0;
    int result = DATA_OTHER_REASON;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32 sOffset = 4;
    OOP_CHINESEINFOS_T chinese_info;
    NOMAL_OAD_T normal = {0};
    uint32 i = 0;
    int    ret = 0;
    uint32 len = 0;
    uint8  bAdd = FALSE;
    OOP_VISIBLEVAR1K_T visbleData;

    MEMZERO(&chinese_info, sizeof(OOP_CHINESEINFOS_T));
    /*类型不存在*/
    if ((pOadInfo == NULL) || (pApduInfo == NULL) || (inData == NULL) || (outData == NULL))
    {
        return DATA_READ_WRITE_DENIED;
    }
    
    PRTL_FMT_DEBUG("pOdaInfo->pTab->oad.value is %x\n",pOadInfo->pTab->oad.value);

    switch(pOadInfo->pTab->oad.value)
    {
        case 0x80037f00:
        case 0x80047f00:
        {
            OOPM_ADD_CHINESEINFO_T addinfo;
            MEMZERO(&addinfo, sizeof(OOPM_ADD_CHINESEINFO_T));
            
            if(inData[sOffset++] != DT_STRUCTURE)
            {
                return DATA_READ_WRITE_DENIED;
            }
            if(inData[sOffset++] != 3)
            {
                return DATA_READ_WRITE_DENIED;
            }
            
            nRet = basic_buf_to_data(E_UNSIGNED, inData, inLen, &sOffset, &addinfo.index, sizeof(uint8));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            nRet = basic_buf_to_data(E_DATETIME_S, inData, inLen, &sOffset, &addinfo.time, sizeof(OOP_DATETIME_S_T));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            //对时间是否合理进行判断
            if(!DT_DateTimeShortIsValid(&addinfo.time))
            {
                return DATA_READ_WRITE_DENIED;
            }
            nRet = basic_buf_to_data(E_VISIBLE_STRING, inData, inLen, &sOffset, &visbleData, sizeof(visbleData));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }
            addinfo.info.nNum = MIN(visbleData.nNum, 200);
            memcpy(addinfo.info.value, visbleData.value, addinfo.info.nNum);

            normal.oad.value = 0x80037f00 == pOadInfo->pTab->oad.value ? 0x80030200 : 0x80040200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_CHINESEINFOS_T), (uint8*)&chinese_info, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                MEMZERO(&chinese_info, sizeof(OOP_CHINESEINFOS_T));
            }
            for(i = 0; i < chinese_info.nNum; i++)
            {
                if(chinese_info.item[i].index == addinfo.index)
                {
                    bAdd = TRUE;
                    memcpy(&(chinese_info.item[i].time), &(addinfo.time), sizeof(OOP_DATETIME_S_T));
                    memcpy(&(chinese_info.item[i].info), &(addinfo.info), sizeof(OOP_VISIBLEVAR256_T));
                    chinese_info.item[i].bRead = FALSE;
                }
            }
            if(FALSE == bAdd && chinese_info.nNum < OOP_MAX_CHINESEINFO)
            {
                bAdd = TRUE;
                i = chinese_info.nNum;
                chinese_info.item[i].index = addinfo.index;
                memcpy(&(chinese_info.item[i].time), &(addinfo.time), sizeof(OOP_DATETIME_S_T));
                memcpy(&(chinese_info.item[i].info), &(addinfo.info), sizeof(OOP_VISIBLEVAR256_T));
                chinese_info.item[i].bRead = FALSE;
                chinese_info.nNum += 1;
            }

            if(TRUE == bAdd)
            {
                normal.classtag = CLASS_DATA_INIT;
                ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&chinese_info, sizeof(OOP_CHINESEINFOS_T));
                if(0 != ret)
                {
                     PRTL_FMT_DEBUG("db_write_nomal %08X failed %d\n", normal.oad.value, ret);
                     bAdd = FALSE;
                }
#if defined PRODUCT_ZCU_1 || defined PRODUCT_SEU
                appctrl_alarm_data(255, 0, 3, 2, NULL);
#endif
                appctrl_db_chg_event(0, 20, 0x80037f00 == pOadInfo->pTab->oad.value ? 0 : 1); //中文信息
            }
            
            PRTL_FMT_DEBUG("(%04d-%02d-%02d %02d:%02d:%02d)添加%s中文信息(序号%d)%s：%s\n",
                            addinfo.time.year, addinfo.time.month, addinfo.time.day, addinfo.time.hour, 
                            addinfo.time.minute, addinfo.time.second, 
                            0x80037f00 == pOadInfo->pTab->oad.value ? "一般" : "中文", addinfo.index,
                            addinfo.info.value, bAdd == TRUE ? "成功" : "失败");
                            
            result = DATA_SUCCESS;                                             //填写成功标识
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = TRUE == bAdd ? 0x00 : DATA_SCOPE_OF_ACCESS_VIOLATED;//DAR  type
            outData->pResultData[offset ++] = 0x00;                            //DATA:OPTIONAL
            outData->resultDataLen = offset;
            break;
        }
        case 0x80038000:
        case 0x80048000:
        {
            uint8 index = 0;
            uint32 num = 0;
            OOP_CHINESEINFOS_T newinfo;
            MEMZERO(&newinfo, sizeof(OOP_CHINESEINFOS_T));

            nRet = basic_buf_to_data(E_UNSIGNED, inData, inLen, &sOffset, &index, sizeof(uint8));
            if (nRet != DATA_SUCCESS)
            {
                return DATA_READ_WRITE_DENIED;
            }

            normal.oad.value = 0x80038000 == pOadInfo->pTab->oad.value ? 0x80030200 : 0x80040200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_CHINESEINFOS_T), (uint8*)&chinese_info, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                MEMZERO(&chinese_info, sizeof(OOP_CHINESEINFOS_T));
            }
            
            for(i = 0; i < chinese_info.nNum; i++)
            {
                if(chinese_info.item[i].index == index)
                {
                    bAdd = TRUE;
                    continue;
                }
                memcpy(&(newinfo.item[num]), &(chinese_info.item[i]), sizeof(CHINESEINFO));
                num++;
            }
            newinfo.nNum = num;

            if(TRUE == bAdd)
            {
                normal.classtag = CLASS_DATA_INIT;
                ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&newinfo, sizeof(OOP_CHINESEINFOS_T));
                if(0 != ret)
                {
                     PRTL_FMT_DEBUG("db_write_nomal %08X failed %d\n", normal.oad.value, ret);
                     bAdd = FALSE;
                }
            }
            
            PRTL_FMT_DEBUG("删除%s中文信息(序号%d)%s\n", 0x80038000 == pOadInfo->pTab->oad.value ? "一般" : "中文",
                           index, bAdd == TRUE ? "成功" : "失败");
                            
            result = DATA_SUCCESS;                                             //填写成功标识
            offset = 0;
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = TRUE == bAdd ? 0x00 : DATA_SCOPE_OF_ACCESS_VIOLATED;//DAR  type
            outData->pResultData[offset ++] = 0x00;                            //DATA:OPTIONAL
            outData->resultDataLen = offset;
            break;
        }
        default:
            return DATA_READ_WRITE_DENIED;
    }
    return result;
}


/**
*********************************************************************
* @name：       class11_invoke_act
* @brief：      class11的ACTION服务
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
OOP_DAR_E class11_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x6000:
    #if AREA_ZHEJIANG
        return area_class11_invoke_act_oop_meter(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
    #else
        return class11_invoke_act_oop_meter(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
   #endif
        break;
    case 0x6002:
        return class11_invoke_act_searchmeter_res(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
    case 0x6014:
        return class11_invoke_act_normal_scheme(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
    case 0x6016:
        return class11_invoke_act_event_scheme(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
    case 0x6018:
        return class11_invoke_act_transparent_scheme(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
    case 0x601c:
        return class11_invoke_act_report_plan(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
    case 0x601E:
        #if 0
        return class11_invoke_act_acq_rule(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        #else
        return class11_invoke_act_encryption_plan(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        #endif
        break;
    case 0x7000:
        return class11_invoke_act_filegather(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;        
    case 0x7300:
        return class11_invoke_act_container(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
    case 0x7301:
        return class11_invoke_act_app(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
    case 0x7302:
        return class11_invoke_act_7302(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
#ifdef AREA_HUNAN
    case 0x60F1:
        return class11_invoke_act_topology(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
#endif
#ifdef AREA_FUJIAN
    case 0x6E05:
        return class11_invoke_act_group_meter(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
    case 0x6E11:
        return class11_invoke_act_transparent_task(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
    case 0x6E13:
        return class11_invoke_act_transparent_task_node(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
    case 0x6E17:
        return class11_invoke_act_transparent_report(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
    case 0x6E31:
        return class11_invoke_act_transparent_event(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        break;
#endif
    case 0x8003:
    case 0x8004:
        return class11_invoke_act_info(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

#endif
