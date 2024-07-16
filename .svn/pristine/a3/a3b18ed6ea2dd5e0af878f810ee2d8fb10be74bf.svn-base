/*
*********************************************************************
* Copyright(C) 2022 南京新联电子股份有限公司
* All rights reserved.
* @brief：   福建规约
* @date：    2022-03-19
* @history：
*********************************************************************
*/


#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "oopVarUpdate.h"
#include "area.h"

#if DESC("终端本体功能扩展内容组织", 1)
/**
*********************************************************************
* @brief：     从698格式报文转数据中心的数据(福建停电工作电池状态)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_battery_status(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_BATTERY_STATUS_T data;

    if (pOadInfo->nIndex != 0)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;

    //停电工作电池有效容量
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.capacity, sizeof(data.capacity));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //停电工作电池电压
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.voltage, sizeof(data.voltage));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建现场信号信息)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_singal_info(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_SINGAL_INFO_T data;

    if (pOadInfo->nIndex != 0)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 5;

    //基站号LAC
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.lac, sizeof(data.lac));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

	//小区号CELL_ID
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.cell_id, sizeof(data.cell_id));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

	//频点号ARFCN
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.arfcn, sizeof(data.arfcn));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

	//接收信号强度
    dar = basic_data_to_buf(E_LONG, dstBuf, dstLen, &dOffset, &data.rx_level, sizeof(data.rx_level));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

	//当前发射功率
    dar = basic_data_to_buf(E_LONG, dstBuf, dstLen, &dOffset, &data.power, sizeof(data.power));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

#endif

#if DESC("透明方案结果内容组织", 1)
/**
*********************************************************************
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_results_all(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_TRANS_TASK_RESULT_T data;

    if (pOadInfo->nIndex != 0)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 11;

    //透明任务方案号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.transTaskid, sizeof(data.transTaskid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //具体任务编号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nodeid, sizeof(data.nodeid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //通信地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.groupAdd, sizeof(data.groupAdd));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(data.groupAdd.add, OOP_TSA_SIZE - 1, "basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //通信对象类型
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.commType, sizeof(data.commType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //采集存储时标
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.storeTime, sizeof(data.storeTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //终端抄表时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.readTime, sizeof(data.readTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //存储序号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.storeNO, sizeof(data.storeNO));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //重试轮次
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.retryTurn, sizeof(data.retryTurn));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //任务结果
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.result, sizeof(data.result));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //相应报文格式
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.resultMessType, sizeof(data.resultMessType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;

    //错误码 OOP_FJ_ERR_E
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.transMsg.errCode, sizeof(data.transMsg.errCode));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //返回数据
    dar = basic_data_to_buf(E_OOP_OCTETVAR2K_T, dstBuf, dstLen, &dOffset, &data.transMsg.resultData, sizeof(data.transMsg.resultData));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_results_long_unsigned(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    uint16 data;

    if ((pOadInfo->nIndex != 1) && (pOadInfo->nIndex != 2) && (pOadInfo->nIndex != 7))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_results_tsa(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_TSA_T data;

    if (pOadInfo->nIndex != 3)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //通信地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(data.add, OOP_TSA_SIZE - 1, "basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_results_unsigned(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    uint8 data;

    if ((pOadInfo->nIndex != 4) && ((pOadInfo->nIndex != 8)))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_results_datatime(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_DATETIME_S_T data;

    if ((pOadInfo->nIndex != 5) && (pOadInfo->nIndex != 6))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_results_enum(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    uint8 data;

    if ((pOadInfo->nIndex != 9) && (pOadInfo->nIndex != 10))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_results_msg(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_TRANS_RES_MSG data;

    if (pOadInfo->nIndex != 11)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;

    //错误码 OOP_FJ_ERR_E
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.errCode, sizeof(data.errCode));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //返回数据
    dar = basic_data_to_buf(E_OOP_OCTETVAR2K_T, dstBuf, dstLen, &dOffset, &data.resultData, sizeof(data.resultData));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_results(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    switch (pOadInfo->nIndex)
    {
    case 0:
        return data_to_buf_transparent_results_all(pOadInfo, convert);
        break;
    case 1:
    case 2:
    case 7:
        return data_to_buf_transparent_results_long_unsigned(pOadInfo, convert);
        break;
    case 3:
        return data_to_buf_transparent_results_tsa(pOadInfo, convert);
        break;
    case 4:
    case 8:
        return data_to_buf_transparent_results_unsigned(pOadInfo, convert);
        break;
    case 5:
    case 6:
        return data_to_buf_transparent_results_datatime(pOadInfo, convert);
        break;
    case 9:
    case 10:
        return data_to_buf_transparent_results_enum(pOadInfo, convert);
        break;
    case 11:
        return data_to_buf_transparent_results_msg(pOadInfo, convert);
        break;
    default:
        break;
    }

    return dar;
}
#endif

#if DESC("外部设备数据采集模式", 1)
/**
*********************************************************************
* @brief：     设置外部设备数据采集模式
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
OOP_DAR_E buf_to_data_acquisition_mode(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_GATHER_E data = 0;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //外部设备数据采集模式
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    PRTL_FMT_LOGCC("buf_to_data_class8_security_flag : data = 0x%x\n", data);

    return dar;
}

/**
*********************************************************************
* @brief：      读取外部设备数据采集模式
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_acquisition_mode(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_GATHER_E data = 0;
    uint8 i = 0;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //外部设备数据采集模式
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data(%d)\n", i, dar, data);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     写保护剩余时间
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
OOP_DAR_E buf_to_data_write_protect_time(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8  data = 0;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //写保护剩余时间
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    PRTL_FMT_LOGCC("buf_to_data_class8_security_flag : data = 0x%x\n", data);

    return dar;
}

/**
*********************************************************************
* @brief：      写保护剩余时间
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_write_protect_time(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8 i = 0;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //写保护剩余时间
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data(%d)\n", i, dar, data);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     非增补代理任务优先级
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
OOP_DAR_E buf_to_data_agent_priority(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //非增补代理任务优先级
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    PRTL_FMT_LOGCC("buf_to_data_class8_security_flag : data = 0x%x\n", data);

    return dar;
}

/**
*********************************************************************
* @brief：      非增补代理任务优先级
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_agent_priority(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //非增补代理任务优先级
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),nDepth(%d)\n", dar, data);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @name：       class8_invoke_get
* @brief：      class8的GET服务
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
OOP_DAR_E class8_invoke_get_oia_6e000200_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = { 0 };
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset = 0;
    uint32 len = 0;

    MEMZERO(&convert, sizeof(convert));

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e000200:
    {
        OOP_GATHER_E dataGet = 1;
        normal.oad.value = 0x6e000200;
        ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&dataGet, &len);
        if ((ret != ERR_OK) || (len == 0))
        {
            PRTL_FMT_DEBUG("读数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
        }

        PRTL_FMT_LOGCC("读外部设备采集模式[%d]\n", dataGet);

        //转换成报文数据
        convert.srcBuf = &dataGet;
        convert.srcLen = sizeof(dataGet);
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
    dar = data_to_buf_acquisition_mode(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：       class8_invoke_get_oia_6e000300_fj
* @brief：      class8的GET服务
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
OOP_DAR_E class8_invoke_get_oia_6e000300_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = { 0 };
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset = 0;
    uint32 len = 0;

    MEMZERO(&convert, sizeof(convert));

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e000300:
    {
        uint8 dataGet = 0;
        normal.oad.value = 0x6e000300;
        ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&dataGet, &len);
        if ((ret != ERR_OK) || (1 != len))
        {
            PRTL_FMT_DEBUG("读数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), used 255\n",
                            ret, normal.logicId, normal.infoNum, normal.oad.value);
            dataGet = 255;
        }

        PRTL_FMT_LOGCC("写保护剩余运行时间[%d]\n", dataGet);

        //转换成报文数据
        convert.srcBuf = &dataGet;
        convert.srcLen = sizeof(dataGet);
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
    dar = data_to_buf_write_protect_time(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：       class8_invoke_get_oia_6e000400_fj
* @brief：      class8的GET服务
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
OOP_DAR_E class8_invoke_get_oia_6e000400_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = { 0 };
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset = 0;
    uint32 len = 0;

    MEMZERO(&convert, sizeof(convert));

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e000400:
    {
        uint8 dataGet = 0;
        normal.oad.value = 0x6e000400;
        ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&dataGet, &len);
        if ((ret != ERR_OK) || (len == 0))
        {
            PRTL_FMT_DEBUG("读数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
        }

        PRTL_FMT_LOGCC("非增补代理任务优先级[%d]\n", dataGet);
        if(dataGet==0)
            dataGet = 20;
        //转换成报文数据
        convert.srcBuf = &dataGet;
        convert.srcLen = sizeof(dataGet);
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
    dar = data_to_buf_agent_priority(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}


/**
*********************************************************************
* @name：      class8_invoke_get_oia_6e00_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_get_oia_6e00_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
    {
        return class8_invoke_get_oia_6e000200_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x03:
    {
        return class8_invoke_get_oia_6e000300_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x04:
    {
        return class8_invoke_get_oia_6e000400_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    default:
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：      class8_invoke_set_oia_6e00_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_oia_6e000200_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = { 0 };
    uint8 data[BUF_LEN_MAX] = { 0 };
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset = 0;

    MEMZERO(&data, sizeof(data));
    MEMZERO(&convert, sizeof(convert));

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;

    //从报文中提取数据
    dar = buf_to_data_acquisition_mode(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen - sOffset, "外部设备采集模式报文解析失败. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e000200:
    {
        OOP_GATHER_E dataSet;
        memcpy(&dataSet, convert.dstBuf, dOffset);
        normal.oad.value = 0x6e000200;
        normal.classtag = CLASS_DATA_UNINIT;
        PRTL_FMT_LOGCC("设置外部设备采集模式0x%x\n", dataSet);
        
        ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("数据中心写入失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return DATA_OTHER_REASON;
        }
        /*重启*/
        PRTL_FMT_DEBUG("设置外部设备采集模式0x%x 成功,重启终端\n", dataSet);
        reboot_sent_to_smiOS();
    }
    break;

    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x00;                            //DAR type
    outData->pResultData[offset++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;
    //reboot_sent_to_smiOS();
    return dar;
}

/**
*********************************************************************
* @name：      class8_invoke_set_oia_6e000300_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_oia_6e000300_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = { 0 };
    uint8 data[BUF_LEN_MAX] = { 0 };
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset = 0;

    MEMZERO(&data, sizeof(data));
    MEMZERO(&convert, sizeof(convert));

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;

    //从报文中提取数据
    dar = buf_to_data_write_protect_time(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen - sOffset, "写保护剩余允许时间报文解析失败. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x6e000300:
        {
            uint8 dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = 0x6e000300;
            normal.classtag = CLASS_DATA_INIT;
            PRTL_FMT_DEBUG("写保护剩余允许时间%d\n", dataSet);
            ret = area_write_protect_reduce(pApduInfo->hUdp, 0, dataSet);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("数据中心写入失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
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
    outData->pResultData[offset++] = 0x00;                            //DAR type
    outData->pResultData[offset++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：      class8_invoke_set_oia_6e00_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_oia_6e000400_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = { 0 };
    uint8 data[BUF_LEN_MAX] = { 0 };
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset = 0;

    MEMZERO(&data, sizeof(data));
    MEMZERO(&convert, sizeof(convert));

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;

    //从报文中提取数据
    dar = buf_to_data_agent_priority(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen - sOffset, "非增补代理任务优先级报文解析失败. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e000400:
    {
        uint8 dataSet;
        memcpy(&dataSet, convert.dstBuf, dOffset);
        normal.oad.value = 0x6e000400;
        normal.classtag = CLASS_DATA_INIT;
        PRTL_FMT_LOGCC("设置外部设备采集模式0x%x\n", dataSet);

        ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("数据中心写入失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
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
    outData->pResultData[offset++] = 0x00;                            //DAR type
    outData->pResultData[offset++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}


/**
*********************************************************************
* @name：      class8_invoke_set_oia_6e00_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_oia_6e00_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
    {
        return class8_invoke_set_oia_6e000200_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x03:
    {
        return class8_invoke_set_oia_6e000300_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x04:
    {
        return class8_invoke_set_oia_6e000400_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    default:
        break;
    }
    
    return DATA_OBJECT_UNAVAILABLE;
}

#endif

#if DESC("端口超时相应时间", 1)
/**
*********************************************************************
* @brief：      端口超时相应时间
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
OOP_DAR_E buf_to_data_port_time_out(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_PORTS_TI_T data;
    uint8 offlen = 0;      //数组数量长度
    uint8 i = 0;

    memset(&data, 0, sizeof(data));

    //数组类型
    if (srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组成员个数
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    for (i = 0; i < data.nNum; i++)
    {
        //结构类型
        if (srcBuf[sOffset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //结构成员数
        if (srcBuf[sOffset++] != 2)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //OAD
        dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.param[i].oad.value, sizeof(data.param[i].oad.value));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //TI
        dar = basic_buf_to_data(E_TI, srcBuf, srcLen, &sOffset, &data.param[i].ti, sizeof(data.param[i].ti));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      端口超时相应时间
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
OOP_DAR_E data_to_buf_port_time_out(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_PORTS_TI_T data;

    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = { 0 }; //数组数量编码
    uint8 i = 0;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //数组类型
    dstBuf[dOffset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(data.nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for (i = 0; i < data.nNum; i++)
    {
        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 2;

        //OAD
        dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.param[i].oad.value, sizeof(data.param[i].oad.value));
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), i[%d] transTaskid(%d)\n", dar, i, data.param[i].oad.value);
            return DATA_TYPE_UNMATCHED;
        }

        //TI
        dar = basic_data_to_buf(E_TI, dstBuf, dstLen, &dOffset, &data.param[i].ti, sizeof(data.param[i].ti));
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), i(%d), nodeid(%d)\n", dar, i, data.param[i].ti.value);
            return DATA_TYPE_UNMATCHED;
        }
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @name：      class8_invoke_set_oia_6e00_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_oia_6e030200_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = { 0 };
    uint8 data[BUF_LEN_MAX] = { 0 };
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset = 0;

    MEMZERO(&data, sizeof(data));
    MEMZERO(&convert, sizeof(convert));

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;

    //从报文中提取数据
    dar = buf_to_data_port_time_out(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen - sOffset, "端口超时时间报文解析失败. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e030200:
    {
        OOP_PORTS_TI_T dataSet;
        memcpy(&dataSet, convert.dstBuf, dOffset);
        normal.oad.value = 0x6e030200;
        normal.classtag = CLASS_DATA_INIT;

        ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(dataSet));
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("数据中心写入失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
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
    outData->pResultData[offset++] = 0x00;                            //DAR type
    outData->pResultData[offset++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：      class8_invoke_set_oia_6e030300_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_oia_6e030300_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = { 0 };
    uint8  status = 0;
    uint32 offset = 0;

    if(0x6e030300 != pOadInfo->pTab->oad.value)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    if(InDataLen < 6 || DT_ENUM != inData[4] || inData[5] > 1)
    {
        return DATA_TYPE_UNMATCHED;
    }

    status = inData[5];

    //写入数据
    normal.oad.value = 0x6e030300;
    normal.classtag = CLASS_DATA_INIT;

    ret = db_write_nomal(pApduInfo->hUdp, &normal, &status, sizeof(status));
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("数据中心写入失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x00;                            //DAR type
    outData->pResultData[offset++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

    
/**
*********************************************************************
* @name：       class8_invoke_get_oia_6e000400_fj
* @brief：      class8的GET服务
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
OOP_DAR_E class8_invoke_get_oia_6e030200_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = { 0 };
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset = 0;
    uint32 len = 0;

    MEMZERO(&convert, sizeof(convert));

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e030200:
    {
        OOP_PORTS_TI_T dataGet = {0};
        normal.oad.value = 0x6e030200;
        ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&dataGet, &len);
        if ((ret != ERR_OK) || (len == 0))
        {
            PRTL_FMT_DEBUG("读数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
        }

        //转换成报文数据
        convert.srcBuf = &dataGet;
        convert.srcLen = sizeof(dataGet);
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
    dar = data_to_buf_port_time_out(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：       class8_invoke_get_oia_6e030300_fj
* @brief：      class8的GET服务
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
OOP_DAR_E class8_invoke_get_oia_6e030300_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = { 0 };
    uint32 offset = 0;
    uint32 len = 0;

    //读出数据
    uint8 status = 0;
    normal.oad.value = 0x6e030300;
    ret = db_read_nomal(pApduInfo->hUdp, &normal, 1, &status, &len);
    if ((ret != ERR_OK) || (len != 1))
    {
        status = 0;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->pResultData[offset++] = DT_ENUM;
    outData->pResultData[offset++] = status;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：      class8_invoke_get_oia_6e01_fj
* @brief：     class8的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2022-04-15
*********************************************************************
*/
OOP_DAR_E class8_invoke_get_oia_6e01_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    uint32 offset = 0;
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e010200:
    {
        memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset++] = 0x01;
        outData->pResultData[offset++] = DT_ENUM;
        outData->pResultData[offset++] = 0x01;
        outData->resultDataLen += offset;
        return DATA_SUCCESS;
    }break;
    default:
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}


/**
*********************************************************************
* @name：      class8_invoke_get_oia_6e02_fj
* @brief：     class8的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    :
* @Date      ：2022-04-15
*********************************************************************
*/
OOP_DAR_E class8_invoke_get_oia_6e02_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    uint32 offset = 0;
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e020200:
    {
        memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset++] = 0x01;
        outData->pResultData[offset++] = DT_ENUM;
        outData->pResultData[offset++] = 0x01;
        outData->resultDataLen +=offset;
        return DATA_SUCCESS;
    }break;
    default:
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：      class8_invoke_get_oia_6e03_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_get_oia_6e03_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x02:
        {
            return class8_invoke_get_oia_6e030200_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
            break;
        }
        case 0x03:
        {
            return class8_invoke_get_oia_6e030300_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
            break;
        }
        default:
            break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：      class8_invoke_set_oia_6e03_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_oia_6e03_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x02:
        {
            return class8_invoke_set_oia_6e030200_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
            break;
        }
        case 0x03:
        {
            return class8_invoke_set_oia_6e030300_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
            break;
        }
        default:
            break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

#endif

#if DESC("通信状态信息", 1)
/**
*********************************************************************
* @brief：      链路测试
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
OOP_DAR_E buf_to_data_link_testing(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_LINK_TEST_T data = {0};
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
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

    //下行报文
    dar = basic_buf_to_data(E_OCTET_STRING_2K, srcBuf, srcLen, &sOffset, &data.msg, sizeof(data.msg));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //响应长度
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.resLen, sizeof(data.resLen));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @name：       class8_invoke_get_oia_6e080200_fj
* @brief：      class8的GET服务
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
OOP_DAR_E class8_invoke_get_oia_6e080200_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset = 0;

    MEMZERO(&convert, sizeof(convert));

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e080200:
    {
        OOP_DATETIME_S_T dataGet = *pApduInfo->pLastRecvTime;
        
        //转换成报文数据
        convert.srcBuf = &dataGet;
        convert.srcLen = sizeof(dataGet);
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
    dar = data_to_buf_class8_time(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class8_time failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->resultDataLen += offset;

    return dar;

}

/**
*********************************************************************
* @name：      class8_invoke_get_oia_6e03_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_get_oia_6e08_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x02:
        {
            return class8_invoke_get_oia_6e080200_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
            break;
        }
        default:
            break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：      class8_invoke_act_oia_6e08_fj
* @brief：     class8的ACT服务
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
OOP_DAR_E class8_invoke_act_oia_6e08_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = { 0 };
    DATA_CONVERT_T   convert;
    OOP_LINK_TEST_T  dataSet;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset = 0;
    uint32 maxBufLen = 0;
    uint32 len = 0;
    uint32 type = 0;

    MEMZERO(&data, sizeof(data));
    MEMZERO(&dataSet, sizeof(dataSet));
    MEMZERO(&convert, sizeof(convert));

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;

    //从报文中提取数据
    dar = buf_to_data_link_testing(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen - sOffset, "链路测试报文解析错误. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e087f00:
    {
        memcpy(&dataSet, convert.dstBuf, dOffset);

        PRTL_FMT_DEBUG("收到链路测试报文 需要响应的长度[%d]\n", dataSet.resLen);

        if (dataSet.resLen > pApduInfo->connectionInfo.ApplSplitLen)
        {
            PRTL_FMT_DEBUG("需要响应的长度 resLen[%d] maxBufLen[%d]\n", dataSet.resLen, pApduInfo->connectionInfo.ApplSplitLen);
            dataSet.resLen = pApduInfo->connectionInfo.ApplSplitLen;
        }

        maxBufLen = dataSet.resLen - cal_excludeLen(pApduInfo->connectionInfo) + 5;
    }
    break;

    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x00;                            //DAR type
    outData->pResultData[offset++] = 0x01;                            //DATA
    outData->pResultData[offset++] = DT_OCTET_STRING;

    len = maxBufLen - offset;

    if (len -1 < 0x80)
    {
        type = 1;
    }
    else if (len -2 <= 0xff)
    {
        type = 2;
    }
    else if (len - 3 <= 0xffff)
    {
        type = 3;
    }

    len = maxBufLen - offset -type;

    switch (type)
    {
    case 1:
    {
        outData->pResultData[offset++] = len;
        break;
    }
    case 2:
    {
        outData->pResultData[offset++] = 0x81;
        outData->pResultData[offset++] = len;
    }
    case 3:
    {
        outData->pResultData[offset++] = 0x82;
        outData->pResultData[offset++] = (uint8)((len >> 8) & 0xff);
        outData->pResultData[offset++] = (uint8)(len & 0xff);
        break;
    }
    default:
        break;
    }
    
    memset(&outData->pResultData[offset], 0xff, len);
    offset += len;

    outData->resultDataLen = offset;

    return dar;
}

#endif

#if DESC("主站上报方案单元", 1)
/**
*********************************************************************
* @brief：      buf_to_data_report_plan_fujian
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
OOP_DAR_E buf_to_data_report_plan_fujian(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TRANS_REPORT_T data;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if (srcBuf[sOffset++] != 8)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //方案编号
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.reportid, sizeof(data.reportid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), planID(%d)\n", dar, data.reportid);
        return DATA_TYPE_UNMATCHED;
    }

    //上报通道
    dar = buf_to_data_evt_asobjattr(pOadInfo, srcBuf, srcLen, &sOffset, &data.channel, sizeof(OOP_OADS_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), channel.nNum(%d)\n", dar, data.channel.nNum);
        return DATA_TYPE_UNMATCHED;
    }

    //上报优先级
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.priority, sizeof(data.priority));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), planID(%d)\n", dar, data.priority);
        return DATA_TYPE_UNMATCHED;
    }

    //响应超时时间
    dar = basic_buf_to_data(E_TI, srcBuf, srcLen, &sOffset, &data.timeout, sizeof(OOP_TI_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), ti.unit(%d), ti.value(%d)\n", dar, data.timeout.unit, data.timeout.value);
        return DATA_TYPE_UNMATCHED;
    }

    //最大重试次数
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.retry, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), retry(%d)\n", dar, data.retry);
        return DATA_TYPE_UNMATCHED;
    }

    //结果验证标识
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.resultFlag, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), retry(%d)\n", dar, data.resultFlag);
        return DATA_TYPE_UNMATCHED;
    }

    //上报时刻
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.reportTime, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), retry(%d)\n", dar, data.reportTime);
        return DATA_TYPE_UNMATCHED;
    }

    //上报内容
    dar = buf_to_data_report_context(pOadInfo, srcBuf, srcLen, &sOffset, &data.data, sizeof(REPORT_CONTEXT));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), choice(%d)\n", dar, data.data.choice);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      data_to_buf_report_plan_fujian
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
OOP_DAR_E data_to_buf_report_plan_fujian(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TRANS_REPORT_T data;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 8;

    //方案编号
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.reportid, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), reportid(%d)\n", dar, data.reportid);
        return DATA_TYPE_UNMATCHED;
    }

    //上报通道
    dar = data_to_buf_evt_asobjattr(pOadInfo, dstBuf, dstLen, &dOffset, &data.channel, sizeof(OOP_OADS_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), channel.nNum(%d)\n", dar, data.channel.nNum);
        return DATA_TYPE_UNMATCHED;
    }

    //上报优先级
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.priority, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), priority(%d)\n", dar, data.priority);
        return DATA_TYPE_UNMATCHED;
    }

    //响应超时时间
    dar = basic_data_to_buf(E_TI, dstBuf, dstLen, &dOffset, &data.timeout, sizeof(OOP_TI_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), ti.unit(%d), ti.value(%d)\n", dar, data.timeout.unit, data.timeout.value);
        return DATA_TYPE_UNMATCHED;
    }

    //最大重试次数
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.retry, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), retry(%d)\n", dar, data.retry);
        return DATA_TYPE_UNMATCHED;
    }

    //结果验证标识
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.resultFlag, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), priority(%d)\n", dar, data.priority);
        return DATA_TYPE_UNMATCHED;
    }

    //上报时刻
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.reportTime, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), priority(%d)\n", dar, data.priority);
        return DATA_TYPE_UNMATCHED;
    }

    //上报内容
    dar = data_to_buf_report_context(pOadInfo, dstBuf, dstLen, &dOffset, &data.data, sizeof(REPORT_CONTEXT));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), choice(%d)\n", dar, data.data.choice);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}


#endif

#if DESC("组群档案单元", 1)

/**
*********************************************************************
* @brief：      组群档案单元
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
OOP_DAR_E buf_to_data_group_meter(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;
    
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_GROUP_METER_T data;

    memset(&data, 0, sizeof(data));

    //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if (srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //TSA
    dar = basic_buf_to_data(E_TSA, srcBuf, srcLen, &sOffset, &data.groupAdd, sizeof(data.groupAdd));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //MS
    dar = basic_buf_to_data(E_MS_F, srcBuf, srcLen, &sOffset, &data.ms, sizeof(data.ms));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
	
    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      群组档案
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
OOP_DAR_E data_to_buf_group_meter(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_GROUP_METER_T data;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;

    //TSA
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.groupAdd, sizeof(data.groupAdd));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), groupAdd.type(%d)\n", dar, data.groupAdd.type);
        return DATA_TYPE_UNMATCHED;
    }

    //MS
    dar = basic_data_to_buf(E_MS_F, dstBuf, dstLen, &dOffset, &data.ms, sizeof(data.ms));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), ms.choice(%d)\n", dar, data.ms.choice);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      组群档案单元
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
OOP_DAR_E buf_to_data_group_meter_num(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_GROUP_NUM_T data;

    memset(&data, 0, sizeof(data));

    //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if (srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //TSA
    dar = basic_buf_to_data(E_TSA, srcBuf, srcLen, &sOffset, &data.groupAdd, sizeof(data.groupAdd));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //num
    dar = basic_buf_to_data(E_MS, srcBuf, srcLen, &sOffset, &data.num, sizeof(data.num));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      群组档案
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
OOP_DAR_E data_to_buf_group_meter_num(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_GROUP_NUM_T data;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;

    //TSA
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.groupAdd, sizeof(data.groupAdd));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), groupAdd.type(%d)\n", dar, data.groupAdd.type);
        return DATA_TYPE_UNMATCHED;
    }

    //num
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.num, sizeof(data.num));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.num(%d)\n", dar, data.num);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}


#endif

#if DESC("全局透明任务方案单元", 1)
/**
*********************************************************************
* @brief：     任务类别
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
OOP_DAR_E buf_to_data_trans_task_type_trigger(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_TRIGGER_T data;

    memset(&data, 0, sizeof(data));

    //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if (srcBuf[sOffset++] != 4)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //触发类型
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.type, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    switch (data.type)
    {
    case 0:
    case 2:
    case 3:
    {
        //NULL
        if (srcBuf[sOffset++] != DT_NULL)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    case 1:
    {
        //遥信变位
        dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &data.data.yx, sizeof(uint8));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        break;
    }
    case 4:
    {
        //结构类型
        if (srcBuf[sOffset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //结构成员数
        if (srcBuf[sOffset++] != 3)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //截取开始
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.data.features.start, sizeof(data.data.features.start));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //截取结束
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.data.features.end, sizeof(data.data.features.end));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //特征字节
        dar = basic_buf_to_data(E_OOP_OCTETVAR256_T, srcBuf, srcLen, &sOffset, &data.data.features.features, sizeof(data.data.features.features));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
    }
    default:
        break;
    }

    //任务对象
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.object, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //触发后任务方案有效时长
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.time, sizeof(uint16));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     任务类别
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
OOP_DAR_E buf_to_data_trans_task_type_data(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;
    uint32 dOffsetTmp = 0;

    OOP_TASK_TYPE_T data;
    DATA_CONVERT_T converttmp;

    memset(&data, 0, sizeof(data));
    memset(&converttmp, 0, sizeof(converttmp));

    //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if (srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //任务类别
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.taskType, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    switch (data.taskType)
    {
    case 0:
    case 2:
    case 3:
    {
        //NULL
        if (srcBuf[sOffset++] != DT_NULL)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    case 1:
    {
        //广播, 单位秒
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.addi_param.broadcast, sizeof(uint8));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    case 4:
    {
        //通信日志, 最大记录条数
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.addi_param.lognum, sizeof(uint16));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    case 5:
    {
        //条件触发
        converttmp.srcBuf = (uint8*)srcBuf;
        converttmp.srcLen = srcLen;
        converttmp.sOffset = &sOffset;

        converttmp.dstBuf = &data.addi_param.condition;
        converttmp.dstLen = sizeof(data);
        converttmp.dOffset = &dOffsetTmp;

        dar = buf_to_data_trans_task_type_trigger(pOadInfo, &converttmp);
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "buf_to_data_trans_task_type_trigger failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    default:
        break;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
OOP_DAR_E buf_to_data_trans_task(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;
    uint32 dOffsetTmp = 0;

    OOP_TRANS_TASK_T data;
    DATA_CONVERT_T converttmp;

    memset(&data, 0, sizeof(data));
    memset(&converttmp, 0, sizeof(converttmp));

    //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if (srcBuf[sOffset++] != 19)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //透明任务方案号
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.taskid, sizeof(uint16));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //端口
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.oad, sizeof(data.oad));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //任务类别
    converttmp.srcBuf = (uint8*)srcBuf;
    converttmp.srcLen = srcLen;
    converttmp.sOffset = &sOffset;

    converttmp.dstBuf = &data.taskTypeData;
    converttmp.dstLen = sizeof(data.taskTypeData);
    converttmp.dOffset = &dOffsetTmp;

    dar = buf_to_data_trans_task_type_data(pOadInfo, &converttmp);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "buf_to_data_trans_task_type_data failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //执行优先级
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.priority, sizeof(data.priority));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //执行独占性
    dar = basic_buf_to_data(E_BOOL, srcBuf, srcLen, &sOffset, &data.bmonopoly, sizeof(data.bmonopoly));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //开始时间
    dar = basic_buf_to_data(E_DATETIME_S, srcBuf, srcLen, &sOffset, &data.startTime, sizeof(data.startTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结束时间
    dar = basic_buf_to_data(E_DATETIME_S, srcBuf, srcLen, &sOffset, &data.endTime, sizeof(data.endTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //任务周期
    dar = basic_buf_to_data(E_TI, srcBuf, srcLen, &sOffset, &data.cycle, sizeof(data.cycle));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //失败重试周期
    dar = basic_buf_to_data(E_TI, srcBuf, srcLen, &sOffset, &data.retryCycle, sizeof(data.retryCycle));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //每轮次失败重试次数
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.turnRetryNum, sizeof(data.turnRetryNum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //失败允许重试轮次
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.turnNum, sizeof(data.turnNum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //协议封拆包方式
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.packType, sizeof(data.packType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //多帧关联任务
    dar = basic_buf_to_data(E_BOOL, srcBuf, srcLen, &sOffset, &data.bRelation, sizeof(data.bRelation));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //通信对象类型
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.commType, sizeof(data.commType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数据存储次数
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.storeNum, sizeof(data.storeNum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //上报方案编号
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.reportid, sizeof(data.reportid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数据加工方式
        //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if (srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //加工方式
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.machtype.type, sizeof(data.machtype.type));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //NULL
    if (srcBuf[sOffset++] != DT_NULL)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //任务开始前脚本ID
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.startScriptID, sizeof(data.startScriptID));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //任务结束后脚本ID
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.endScriptID, sizeof(data.endScriptID));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      任务类别
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
OOP_DAR_E data_to_buf_trans_task_type_trigger(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_TRIGGER_T data;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 4;

    //触发类型
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.type, sizeof(data.type));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), taskType(%d)\n", dar, data.type);
        return DATA_TYPE_UNMATCHED;
    }

    //触发雷内容
    switch (data.type)
    {
    case 0:
    case 2:
    case 3:
    {
        dstBuf[dOffset++] = DT_NULL;
        break;
    }
    case 1:
    {
        //遥信变位
        dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.data.yx, sizeof(data.data.yx));
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.data.yx(%d)\n", dar, data.data.yx);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    case 4:
    {
        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 3;

        //截取开始
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.data.features.start, sizeof(data.data.features.start));
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.data.features.start(%d)\n", dar, data.data.features.start);
            return DATA_TYPE_UNMATCHED;
        }

        //截取结束
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.data.features.end, sizeof(data.data.features.end));
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.data.features.end(%d)\n", dar, data.data.features.end);
            return DATA_TYPE_UNMATCHED;
        }

        //特征字节
        dar = basic_data_to_buf(E_OOP_OCTETVAR256_T, dstBuf, dstLen, &dOffset, &data.data.features.features, sizeof(data.data.features.features));
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.data.features.features.num(%d)\n", dar, data.data.features.features.nNum);
            return DATA_TYPE_UNMATCHED;
        }

        break;
    }
    default:
        break;
    }

    //任务对象
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.object, sizeof(data.object));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), object(%d)\n", dar, data.object);
        return DATA_TYPE_UNMATCHED;
    }

    //触发后任务方案有效时长
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.time, sizeof(data.time));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), time(%d)\n", dar, data.time);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      任务类别
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
OOP_DAR_E data_to_buf_trans_task_type_data(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;
    uint32 sOffsetTmp = 0;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_TASK_TYPE_T data;
    DATA_CONVERT_T converttmp;

    memset(&converttmp, 0, sizeof(converttmp));

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;

    //任务类别
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.taskType, sizeof(data.taskType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), taskType(%d)\n", dar, data.taskType);
        return DATA_TYPE_UNMATCHED;
    }

    //data
    switch (data.taskType)
    {
    case 0:
    case 2:
    case 3:
    {
        dstBuf[dOffset++] = DT_NULL;
        break;
    }
    case 1:
    {
        //广播
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.addi_param.broadcast, sizeof(data.addi_param.broadcast));
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.addi_param.broadcast(%d)\n", dar, data.addi_param.broadcast);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    case 4:
    {
        //通信日志
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.addi_param.lognum, sizeof(data.addi_param.lognum));
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.addi_param.lognum(%d)\n", dar, data.addi_param.lognum);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    case 5:
    {
        converttmp.srcBuf = &data.addi_param.condition;
        converttmp.srcLen = sizeof(data.addi_param.condition);
        converttmp.sOffset = &sOffsetTmp;

        converttmp.dstBuf = dstBuf;
        converttmp.dstLen = dstLen - dOffset;
        converttmp.dOffset = &dOffset;

        dar = data_to_buf_trans_task_type_trigger(pOadInfo, &converttmp);
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_trans_task_type_trigger failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    default:
        break;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
OOP_DAR_E data_to_buf_trans_task(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;
    uint32 sOffsetTmp = 0;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_TRANS_TASK_T data;
    DATA_CONVERT_T converttmp;

    memset(&converttmp, 0, sizeof(converttmp));

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 19;

    //透明任务方案号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.taskid, sizeof(data.taskid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), taskid(%d)\n", dar, data.taskid);
        return DATA_TYPE_UNMATCHED;
    }

    //端口
    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.oad, sizeof(data.oad));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), oad(0x%08x)\n", dar, data.oad.value);
        return DATA_TYPE_UNMATCHED;
    }

    //任务类别
    converttmp.srcBuf = &data.taskTypeData;
    converttmp.srcLen = sizeof(data.taskTypeData);
    converttmp.sOffset = &sOffsetTmp;

    converttmp.dstBuf = dstBuf;
    converttmp.dstLen = dstLen - dOffset;
    converttmp.dOffset = &dOffset;

    dar = data_to_buf_trans_task_type_data(pOadInfo, &converttmp);
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_trans_task_type_trigger failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //执行优先级
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.priority, sizeof(data.priority));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.priority(%d)\n", dar, data.priority);
        return DATA_TYPE_UNMATCHED;
    }

    //执行独占性
    dar = basic_data_to_buf(E_BOOL, dstBuf, dstLen, &dOffset, &data.bmonopoly, sizeof(data.bmonopoly));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), bmonopoly(%d)\n", dar, data.bmonopoly);
        return DATA_TYPE_UNMATCHED;
    }

    //开始时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.startTime, sizeof(data.startTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), startTime(%d)\n", dar, data.startTime);
        return DATA_TYPE_UNMATCHED;
    }

    //结束时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.endTime, sizeof(data.endTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), resMessType(%d)\n", dar, data.endTime);
        return DATA_TYPE_UNMATCHED;
    }

    //任务周期
    dar = basic_data_to_buf(E_TI, dstBuf, dstLen, &dOffset, &data.cycle, sizeof(data.cycle));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.cycle(%d)\n", dar, data.cycle.value);
        return DATA_TYPE_UNMATCHED;
    }

    //失败重试周期
    dar = basic_data_to_buf(E_TI, dstBuf, dstLen, &dOffset, &data.retryCycle, sizeof(data.retryCycle));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.retryCycle.value(%d)\n", dar, data.retryCycle.value);
        return DATA_TYPE_UNMATCHED;
    }

    //每轮次失败重试次数
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.turnRetryNum, sizeof(data.turnRetryNum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.turnRetryNum(%d)\n", dar, data.turnRetryNum);
        return DATA_TYPE_UNMATCHED;
    }

    //失败允许重试轮次
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.turnNum, sizeof(data.turnNum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.turnNum(%d)\n", dar, data.turnNum);
        return DATA_TYPE_UNMATCHED;
    }

    //协议封拆包方式
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.packType, sizeof(data.packType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.packType(%d)\n", dar, data.packType);
        return DATA_TYPE_UNMATCHED;
    }

    //多帧关联任务
    dar = basic_data_to_buf(E_BOOL, dstBuf, dstLen, &dOffset, &data.bRelation, sizeof(data.bRelation));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.bRelation(%d)\n", dar, data.bRelation);
        return DATA_TYPE_UNMATCHED;
    }

    //通信对象类型
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.commType, sizeof(data.commType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.commType(%d)\n", dar, data.commType);
        return DATA_TYPE_UNMATCHED;
    }

    //数据存储次数
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.storeNum, sizeof(data.storeNum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.storeNum(%d)\n", dar, data.storeNum);
        return DATA_TYPE_UNMATCHED;
    }

    //上报方案编号
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.reportid, sizeof(data.reportid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.reportid(%d)\n", dar, data.reportid);
        return DATA_TYPE_UNMATCHED;
    }

    //数据加工方式
    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;

    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.machtype.type, sizeof(data.machtype.type));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.machtype.type(%d)\n", dar, data.machtype.type);
        return DATA_TYPE_UNMATCHED;
    }

    dstBuf[dOffset++] = DT_NULL;

    //任务开始前脚本ID
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.startScriptID, sizeof(data.startScriptID));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.startScriptID(%d)\n", dar, data.startScriptID);
        return DATA_TYPE_UNMATCHED;
    }

    //任务结束后脚本ID
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.endScriptID, sizeof(data.endScriptID));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.endScriptID(%d)\n", dar, data.endScriptID);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}
#endif

#if DESC("全局透明具体任务单元", 1)
/**
*********************************************************************
* @brief：     buf_to_data_trans_dataCompare
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
OOP_DAR_E buf_to_data_trans_dataCompare(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    uint32 j = 0;
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_DATACOMP_PARAM_T data;

    memset(&data, 0, sizeof(data));

    //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if (srcBuf[sOffset++] != 5)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //特征字节
    dar = basic_buf_to_data(E_OOP_OCTETVAR256_T, srcBuf, srcLen, &sOffset, &data.feature, sizeof(data.feature));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //截取开始
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.start, sizeof(data.start));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //截取长度
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.len, sizeof(data.len));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数据格式类型
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.datatype, sizeof(data.datatype));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构类型
    if (srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组数量
    data.num = srcBuf[sOffset++];

    for (j = 0; j < data.num; j++)
    {
         //结构类型
        if (srcBuf[sOffset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //结构成员数
        if (srcBuf[sOffset++] != 3)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }
    
        //验证通过数据区间1
        dar = basic_buf_to_data(E_OOP_OCTETVAR256_T, srcBuf, srcLen, &sOffset, &data.param[j].section1, sizeof(data.param[j].section1));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //验证通过数据区间2
        dar = basic_buf_to_data(E_OOP_OCTETVAR256_T, srcBuf, srcLen, &sOffset, &data.param[j].section2, sizeof(data.param[j].section2));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //触发任务方案号
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.param[j].taskid, sizeof(data.param[j].taskid));
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      buf_to_data_trans_messCompare
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
OOP_DAR_E buf_to_data_trans_messCompare(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_MESSCOM_PARAM_T data;

    memset(&data, 0, sizeof(data));

    //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if (srcBuf[sOffset++] != 4)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //特征字节
    dar = basic_buf_to_data(E_OOP_OCTETVAR256_T, srcBuf, srcLen, &sOffset, &data.feature, sizeof(data.feature));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //截取开始
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.start, sizeof(data.start));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //截取长度
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.len, sizeof(data.len));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //触发任务方案号
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.taskid, sizeof(data.taskid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      buf_to_data_trans_task_recv_msg_proc
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
OOP_DAR_E buf_to_data_trans_task_recv_msg_proc(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;
    uint32 dOffsetTmp = 0;

    OOP_RECV_MESS_PROC_T data;
    DATA_CONVERT_T       converttmp;

    memset(&data, 0, sizeof(data));
    memset(&converttmp, 0, sizeof(converttmp));

    //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if (srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //验证方式
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.type, sizeof(data.type));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    switch (data.type)
    {
    case 0:
    case 4:
    {
        //DT_NULL
        if (srcBuf[sOffset++] != DT_NULL)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    case 1:
    case 2:
    {
        /*< type == 1, 报文比对不一致验证;        type == 2, 对数据进行判断后触发相应方案*/
        converttmp.srcBuf = (uint8*)srcBuf;
        converttmp.srcLen = srcLen;
        converttmp.sOffset = &sOffset;
        
        converttmp.dstBuf = (uint8*)&data.messCompare;
        converttmp.dstLen = sizeof(data.messCompare);
        converttmp.dOffset = &dOffsetTmp;

        dar = buf_to_data_trans_messCompare(pOadInfo, &converttmp);
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    case 3:
    {
        /*< type == 1, 报文比对不一致验证;        type == 2, 对数据进行判断后触发相应方案*/
        converttmp.srcBuf = (uint8*)srcBuf;
        converttmp.srcLen = srcLen;
        converttmp.sOffset = &sOffset;

        converttmp.dstBuf = (uint8*)&data.dataCompare;
        converttmp.dstLen = sizeof(data.dataCompare);
        converttmp.dOffset = &dOffsetTmp;

        dar = buf_to_data_trans_dataCompare(pOadInfo, &converttmp);
        if (dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    default:
        break;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      buf_to_data_trans_task_node
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
OOP_DAR_E buf_to_data_trans_task_node(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;
    uint32 dOffsetTmp = 0;

    OOP_TRANS_TASK_NODE_T data;
    DATA_CONVERT_T converttmp;

    memset(&data, 0, sizeof(data));
    memset(&converttmp, 0, sizeof(converttmp));

    //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if (srcBuf[sOffset++] != 10)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //透明任务方案号
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.transTaskid, sizeof(data.transTaskid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //具体任务序号
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.nodeid, sizeof(data.nodeid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //通信地址集合
    dar = basic_buf_to_data(E_MS, srcBuf, srcLen, &sOffset, &data.ms, sizeof(data.ms));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //端口通信控制块
    dar = basic_buf_to_data(E_COMDCB, srcBuf, srcLen, &sOffset, &data.uartParam, sizeof(data.uartParam));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //请求报文格式字
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.reqMessType, sizeof(data.reqMessType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //请求报文内容
    dar = basic_buf_to_data(E_OCTET_STRING_2K, srcBuf, srcLen, &sOffset, &data.reqMess, sizeof(data.reqMess));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //响应报文格式字
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.resMessType, sizeof(data.resMessType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //响应报文预计最长长度
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.resMaxLen, sizeof(data.resMaxLen));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //成功响应报文最短有效长度
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.resMinLen, sizeof(data.resMinLen));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //任务类别
    converttmp.srcBuf = (uint8*)srcBuf;
    converttmp.srcLen = srcLen;
    converttmp.sOffset = &sOffset;

    converttmp.dstBuf = &data.resMessProc;
    converttmp.dstLen = sizeof(data.resMessProc);
    converttmp.dOffset = &dOffsetTmp;

    dar = buf_to_data_trans_task_recv_msg_proc(pOadInfo, &converttmp);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "buf_to_data_trans_task_type_data failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      data_to_buf_trans_task_node
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
OOP_DAR_E data_to_buf_trans_dataCompare(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    uint32 i = 0;
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_DATACOMP_PARAM_T data;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 5;

    //特征字节
    dar = basic_data_to_buf(E_OOP_OCTETVAR256_T, dstBuf, dstLen, &dOffset, &data.feature, sizeof(data.feature));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.feature.nNum(%d)\n", dar, data.feature.nNum);
        return DATA_TYPE_UNMATCHED;
    }

    //截取开始
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.start, sizeof(data.start));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.start(%d)\n", dar, data.start);
        return DATA_TYPE_UNMATCHED;
    }

    //截取长度
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.len, sizeof(data.len));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.len(%d)\n", dar, data.len);
        return DATA_TYPE_UNMATCHED;
    }

    //数据格式类型
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.datatype, sizeof(data.datatype));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.datatype(%d)\n", dar, data.datatype);
        return DATA_TYPE_UNMATCHED;
    }

    //验证区间组
    dstBuf[dOffset++] = DT_ARRAY;

    dstBuf[dOffset++] = data.num;

    for (i = 0; i < data.num; i++)
    {
        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 3;

        //验证通过数据区间1
        dar = basic_data_to_buf(E_OOP_OCTETVAR256_T, dstBuf, dstLen, &dOffset, &data.param[i].section1, sizeof(data.param[i].section1));
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.param[%d].section1(%d)\n", dar, i, data.param[i].section1);
            return DATA_TYPE_UNMATCHED;
        }

        //验证通过数据区间2
        dar = basic_data_to_buf(E_OOP_OCTETVAR256_T, dstBuf, dstLen, &dOffset, &data.param[i].section2, sizeof(data.param[i].section2));
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.param[%d].section2(%d)\n", dar, i, data.param[i].section2);
            return DATA_TYPE_UNMATCHED;
        }

        //触发任务方案号
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.param[i].taskid, sizeof(data.param[i].taskid));
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.param[%d].taskid(%d)\n", dar, i, data.param[i].taskid);
            return DATA_TYPE_UNMATCHED;
        }
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      data_to_buf_trans_task_node
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
OOP_DAR_E data_to_buf_trans_messCompare(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_MESSCOM_PARAM_T data;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 4;

    //特征字节
    dar = basic_data_to_buf(E_OOP_OCTETVAR256_T, dstBuf, dstLen, &dOffset, &data.feature, sizeof(data.feature));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.feature.nNum(%d)\n", dar, data.feature.nNum);
        return DATA_TYPE_UNMATCHED;
    }

    //截取开始
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.start, sizeof(data.start));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.start(%d)\n", dar, data.start);
        return DATA_TYPE_UNMATCHED;
    }

    //截取长度
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.len, sizeof(data.len));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.len(%d)\n", dar, data.len);
        return DATA_TYPE_UNMATCHED;
    }

    //触发任务方案号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.taskid, sizeof(data.taskid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.datatype(%d)\n", dar, data.taskid);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      data_to_buf_trans_task_node
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
OOP_DAR_E data_to_buf_trans_task_recv_msg_proc(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;
    uint32 sOffsetTmp = 0;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_RECV_MESS_PROC_T data;
    DATA_CONVERT_T converttmp;

    memset(&converttmp, 0, sizeof(converttmp));

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;

    //透明任务方案号
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.type, sizeof(data.type));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), type(%d)\n", dar, data.type);
        return DATA_TYPE_UNMATCHED;
    }

    //DATA
    switch (data.type)
    {
    case 0:
    case 4:
    {
        dstBuf[dOffset++] = DT_NULL;
        break;
    }
    case 1:
    case 2:
    {
        converttmp.srcBuf = &data.messCompare;
        converttmp.srcLen = sizeof(data.messCompare);
        converttmp.sOffset = &sOffsetTmp;

        converttmp.dstBuf = dstBuf;
        converttmp.dstLen = dstLen - dOffset;
        converttmp.dOffset = &dOffset;

        dar = data_to_buf_trans_messCompare(pOadInfo, &converttmp);
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_trans_messCompare failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    case 3:
    {
        converttmp.srcBuf = &data.dataCompare;
        converttmp.srcLen = sizeof(data.dataCompare);
        converttmp.sOffset = &sOffsetTmp;

        converttmp.dstBuf = dstBuf;
        converttmp.dstLen = dstLen - dOffset;
        converttmp.dOffset = &dOffset;

        dar = data_to_buf_trans_dataCompare(pOadInfo, &converttmp);
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_trans_dataCompare failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    default:
        break;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      data_to_buf_trans_task_node
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
OOP_DAR_E data_to_buf_trans_task_node(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;
    uint32 sOffsetTmp = 0;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_TRANS_TASK_NODE_T data;
    DATA_CONVERT_T converttmp;

    memset(&converttmp, 0, sizeof(converttmp));

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 10;

    //透明任务方案号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.transTaskid, sizeof(data.transTaskid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), transTaskid(%d)\n", dar, data.transTaskid);
        return DATA_TYPE_UNMATCHED;
    }

    //具体任务序号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nodeid, sizeof(data.nodeid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nodeid(%d)\n", dar, data.nodeid);
        return DATA_TYPE_UNMATCHED;
    }

    //通信地址集合
    dar = basic_data_to_buf(E_MS_C, dstBuf, dstLen, &dOffset, &data.ms, sizeof(data.ms));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.ms.choice(%d)\n", dar, data.ms.choice);
        return DATA_TYPE_UNMATCHED;
    }

    //端口通信控制块
    dar = basic_data_to_buf(E_COMDCB, dstBuf, dstLen, &dOffset, &data.uartParam, sizeof(data.uartParam));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.uartParam.baud(%d)\n", dar, data.uartParam.baud);
        return DATA_TYPE_UNMATCHED;
    }

    //请求报文格式字
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.reqMessType, sizeof(data.reqMessType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), reqMessType(%d)\n", dar, data.reqMessType);
        return DATA_TYPE_UNMATCHED;
    }

    //请求报文内容
    dar = basic_data_to_buf(E_OOP_OCTETVAR2K_T, dstBuf, dstLen, &dOffset, &data.reqMess, sizeof(data.reqMess));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.reqMess.nNum(%d)\n", dar, data.reqMess.nNum);
        return DATA_TYPE_UNMATCHED;
    }

    //响应报文格式字
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.resMessType, sizeof(data.resMessType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.resMessType(%d)\n", dar, data.resMessType);
        return DATA_TYPE_UNMATCHED;
    }

    //响应报文预计最长长度
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.resMaxLen, sizeof(data.resMaxLen));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.resMaxLen(%d)\n", dar, data.resMaxLen);
        return DATA_TYPE_UNMATCHED;
    }

    //成功响应报文最短有效长度
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.resMinLen, sizeof(data.resMinLen));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.resMinLen(%d)\n", dar, data.resMinLen);
        return DATA_TYPE_UNMATCHED;
    }

    //任务类别
    converttmp.srcBuf = &data.resMessProc;
    converttmp.srcLen = sizeof(data.resMessProc);
    converttmp.sOffset = &sOffsetTmp;

    converttmp.dstBuf = dstBuf;
    converttmp.dstLen = dstLen - dOffset;
    converttmp.dOffset = &dOffset;

    dar = data_to_buf_trans_task_recv_msg_proc(pOadInfo, &converttmp);
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_trans_task_type_trigger failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

#endif

#if DESC("主动上报事件上报策略", 1)
/**
*********************************************************************
* @brief：      buf_to_data_report_event_fujian
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
OOP_DAR_E buf_to_data_report_event_fujian(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TRANS_EVENT_REPORT_T data;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if (srcBuf[sOffset++] != 4)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //上报通道
    dar = buf_to_data_evt_asobjattr(pOadInfo, srcBuf, srcLen, &sOffset, &data.channel, sizeof(OOP_OADS_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), channel.nNum(%d)\n", dar, data.channel.nNum);
        return DATA_TYPE_UNMATCHED;
    }

    //上报优先级
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.priority, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), priority(%d)\n", dar, data.priority);
        return DATA_TYPE_UNMATCHED;
    }

    //响应超时时间
    dar = basic_buf_to_data(E_TI, srcBuf, srcLen, &sOffset, &data.timeout, sizeof(OOP_TI_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), ti.unit(%d), ti.value(%d)\n", dar, data.timeout.unit, data.timeout.value);
        return DATA_TYPE_UNMATCHED;
    }

    //最大重试次数
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.retry, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), retry(%d)\n", dar, data.retry);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      data_to_buf_report_event_fujian
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
OOP_DAR_E data_to_buf_report_event_fujian(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TRANS_EVENT_REPORT_T data;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 4;

    //上报通道
    dar = data_to_buf_evt_asobjattr(pOadInfo, dstBuf, dstLen, &dOffset, &data.channel, sizeof(OOP_OADS_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), channel.nNum(%d)\n", dar, data.channel.nNum);
        return DATA_TYPE_UNMATCHED;
    }

    //上报优先级
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.priority, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), priority(%d)\n", dar, data.priority);
        return DATA_TYPE_UNMATCHED;
    }

    //响应超时时间
    dar = basic_data_to_buf(E_TI, dstBuf, dstLen, &dOffset, &data.timeout, sizeof(OOP_TI_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), ti.unit(%d), ti.value(%d)\n", dar, data.timeout.unit, data.timeout.value);
        return DATA_TYPE_UNMATCHED;
    }

    //最大重试次数
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.retry, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), retry(%d)\n", dar, data.retry);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @name：      class8_invoke_get_oia_6e33_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_get_oia_6e33_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = { 0 };
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset = 0;
    uint32 len = 0;

    MEMZERO(&convert, sizeof(convert));

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e330200:
    {
        OOP_TRANS_EVENT_REPORT_T dataGet = {0};
        normal.oad.value = 0x6e330200;
        ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&dataGet, &len);
        if ((ret != ERR_OK) || (len == 0))
        {
            PRTL_FMT_DEBUG("读数据中心失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
        }

        //转换成报文数据
        convert.srcBuf = &dataGet;
        convert.srcLen = sizeof(dataGet);
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
    dar = data_to_buf_report_event_fujian(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_report_event_fujian failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：      class8_invoke_set_oia_6e00_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_oia_6e33_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = { 0 };
    uint8 data[BUF_LEN_MAX] = { 0 };
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset = 0;

    MEMZERO(&data, sizeof(data));
    MEMZERO(&convert, sizeof(convert));

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;

    //从报文中提取数据
    dar = buf_to_data_report_event_fujian(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen - sOffset, "事件上报策略解析失败. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e330200:
    {
        OOP_TRANS_EVENT_REPORT_T dataSet;
        memcpy(&dataSet, convert.dstBuf, dOffset);
        normal.oad.value = 0x6e330200;
        normal.classtag = CLASS_DATA_INIT;

        ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_TRANS_EVENT_REPORT_T));
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("数据中心写入失败. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
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
    outData->pResultData[offset++] = 0x00;                            //DAR type
    outData->pResultData[offset++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

#endif

#if DESC("存储空间使用情况", 1)
/**
*********************************************************************
* @brief：      data_to_buf_storage_usage
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
OOP_DAR_E data_to_buf_storage_usage(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_STORE_SIZE_T data;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 4;

    //数据区大小
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dataSize, sizeof(uint32));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), dataSize(%d)\n", dar, data.dataSize);
        return DATA_TYPE_UNMATCHED;
    }

    //已经使用大小
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dataUsed, sizeof(uint32));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), dataUsed(%d)\n", dar, data.dataUsed);
        return DATA_TYPE_UNMATCHED;
    }

    //参数区大小
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.paramSize, sizeof(uint32));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), paramSize(%d)\n", dar, data.paramSize);
        return DATA_TYPE_UNMATCHED;
    }

    //已经使用大小
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.paramUsed, sizeof(uint32));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), paramUsed(%d)\n", dar, data.paramUsed);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @name：      class8_invoke_get_oia_6e25_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_get_oia_6e25_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset = 0;
    
    FILE  *fp = NULL;
    char   buf[512] = {0};
    
    char   Filesystem[64] = {0};
    uint32 Size           =  0;
    uint32 Used           =  0;
    uint32 Available      =  0;
    char   percentage[64] = {0};
    char   Mounted[64]    = {0};

    MEMZERO(&convert, sizeof(convert));

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x6e250200:
    {
        OOP_STORE_SIZE_T dataGet = {0};

        fp = popen("df -k | grep userdata", "r");
        if (fp == NULL)
        {
            return DATA_TEMPORARY_FAILURE;
        }

        while (fgets(buf, sizeof(buf), fp) != NULL)
        {
            PRTL_FMT_DEBUG("磁盘信息:%s", buf);
            if (strstr(buf, "userdata") != NULL)
            {
                sscanf(buf, "%s %d %d %d %s %s", Filesystem, &Size, &Used, &Available, percentage, Mounted);
                PRTL_FMT_DEBUG("总大小[%d] 已经使用的大小[%d]\n", Size, Used);
                break;
            }
        }

        fclose(fp);

        dataGet.dataSize = Size;
        dataGet.dataUsed = Used;
        dataGet.paramSize = Size;
        dataGet.paramUsed = Used;

        //转换成报文数据
        convert.srcBuf = &dataGet;
        convert.srcLen = sizeof(dataGet);
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
    dar = data_to_buf_storage_usage(pOadInfo, &convert);
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_report_event_fujian failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

#endif

#if DESC("一个采集状态", 1)
/**
*********************************************************************
* @brief：      buf_to_data_acq_status
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
OOP_DAR_E buf_to_data_acq_status(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TRANS_ACQ_RLUESTATE_T data;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //结构类型
    if (srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //结构成员数
    if (srcBuf[sOffset++] != 5)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //上报通道
    dar = basic_buf_to_data(E_TSA, srcBuf, srcLen, &sOffset, &data.add, sizeof(data.add));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.add.len(%d)\n", dar, data.add.len);
        return DATA_TYPE_UNMATCHED;
    }

    //通信对象类型
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.commType, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), commType(%d)\n", dar, data.commType);
        return DATA_TYPE_UNMATCHED;
    }

    //端口
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.oad, sizeof(data.oad));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.oad(0x%08x)\n", dar, data.oad.value);
        return DATA_TYPE_UNMATCHED;
    }

    //最后一次采集成功时间
    dar = basic_buf_to_data(E_DATETIME_S, srcBuf, srcLen, &sOffset, &data.lastSuccTime, sizeof(data.lastSuccTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.lastSuccTime(%d)\n", dar, data.lastSuccTime);
        return DATA_TYPE_UNMATCHED;
    }

    //最后一次采集成功时间
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.lastFailTime, sizeof(data.lastFailTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.lastFailTime(%d)\n", dar, data.lastFailTime);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：      data_to_buf_acq_status
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
OOP_DAR_E data_to_buf_acq_status(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TRANS_ACQ_RLUESTATE_T data;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 5;

    //通信地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.add, sizeof(data.add));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.add.len(%d)\n", dar, data.add.len);
        return DATA_TYPE_UNMATCHED;
    }

    //通信对象类型
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.commType, sizeof(uint8));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), commType(%d)\n", dar, data.commType);
        return DATA_TYPE_UNMATCHED;
    }

    //端口
    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.oad, sizeof(data.oad));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.oad.value(0x%08x)\n", dar, data.oad.value);
        return DATA_TYPE_UNMATCHED;
    }

    //最后一次采集成功时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.lastSuccTime, sizeof(data.lastSuccTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.lastSuccTime(%d)\n", dar, data.lastSuccTime);
        return DATA_TYPE_UNMATCHED;
    }

    //最后一次采集失败时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.lastFailTime, sizeof(data.lastFailTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.lastFailTime(%d)\n", dar, data.lastFailTime);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

#endif

#if DESC("通道采集状态", 1)
/**
*********************************************************************
* @brief：      data_to_buf_acq_status
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
OOP_DAR_E data_to_buf_channel_status(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CHAN_RLUESTATE_T data;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 4;

    //端口
    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.oad, sizeof(data.oad));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.oad.value(0x%08x)\n", dar, data.oad.value);
        return DATA_TYPE_UNMATCHED;
    }

    //当前执行的任务类别
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.taskType, sizeof(data.taskType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.taskType(%d)\n", dar, data.taskType);
        return DATA_TYPE_UNMATCHED;
    }

    //当前执行的透明任务方案号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.transTaskid, sizeof(data.transTaskid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.transTaskid(%d)\n", dar, data.transTaskid);
        return DATA_TYPE_UNMATCHED;
    }

    //执行优先级
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.priority, sizeof(data.priority));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.priority(%d)\n", dar, data.priority);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @name：      class8_invoke_get_oia_6e23_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_get_oia_6e22_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen,
    RESULT_DATA_T* outData, uint16 outDataMax)
{
        int ret = ERR_OK;
        OOP_DAR_E dar = DATA_SUCCESS;
        DATA_CONVERT_T convert;
        uint32 sOffset = 0;
        uint32 dOffset = 0;
        uint32 offset = 0;
        uint32             i = 0;
        READ_RECORD_T      inReadRecord;
        uint32             handle = 0;
        uint32             recNum = 0;
        STEP_READ_RECORD_T stepReadRecord;
        RECORD_UNIT_T      stepRecordOut;
        OOP_CHAN_RLUESTATE_T  data;
        uint32    realNum = 0;    //数组由于分帧导致的每帧实际成员数
        uint32    numOffset = 0;  //数组成员数在报文中的位置
        uint32    bufAllLen = 0;
        BP_INFO_T breakInfo = { 0 };                                                //断点信息
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
        inReadRecord.recordOAD.logicId = 0;
    
        if (pOadInfo->nIndex == 0)
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
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
        {
            inReadRecord.cStart = pApduInfo->bpBreakInfo->tmStartSeg;
        }
        memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset++] = 0x01;                              //结果
        if (pOadInfo->nIndex == 0)
        {
            outData->pResultData[offset++] = DT_ARRAY;                         //类型
            numOffset = offset;                                                //数量偏移
            outData->pResultData[offset++] = 0x00;                             //数量，先填0
        }
        bufAllLen = offset;
        ret = db_read_record_start(pApduInfo->hUdp, &inReadRecord, &handle, &recNum);
        if ((ret != ERR_OK) || (recNum == 0))
        {
            PRTL_FMT_DEBUG("通道状态开始读失败 ret = %d, infoNum = %d recNum = %d\n",
                ret, inReadRecord.recordOAD.infoNum, recNum);
        }
        else
        {
            PRTL_FMT_DEBUG("通道状态开始读成功 ret = %d, infoNum = %d recNum = %d\n",
                ret, inReadRecord.recordOAD.infoNum, recNum);
    
            for (i = 0; i < recNum; i++)
            {
                ret = db_read_record_step(pApduInfo->hUdp, handle, &stepReadRecord, &stepRecordOut);
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("通道状态单步读失败 db_read_record_step err = %d!\n", ret);
                    continue;
                    //数据中心出错返回NULL
                }
    
                //OAD与返回数据
                if ((stepRecordOut.recordnum != 1)
                    || (stepRecordOut.record[0].road.oadCols.oad[0].value != pOadInfo->pTab->oad.value)
                    || (stepRecordOut.record[0].outDatalen != sizeof(OOP_CHAN_RLUESTATE_T)))
                {
                    PRTL_FMT_DEBUG("读通道状态失败!\n");
                    PRTL_FMT_DEBUG("读通道状态 stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                    PRTL_FMT_DEBUG("读通道状态 stepRecordOut.record[0].road.oadCols.oad[0] = 0x%08x\n",
                        stepRecordOut.record[0].road.oadCols.oad[0].value);
                    PRTL_FMT_DEBUG("读通道状态 stepRecordOut.record[0].outDatalen = %d\n",
                        stepRecordOut.record[0].outDatalen);
                    PRTL_FMT_DEBUG("读通道状态 sizeof(OOP_CHAN_RLUESTATE_T) = %d\n",
                        sizeof(OOP_CHAN_RLUESTATE_T));
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
                dar = data_to_buf_channel_status(pOadInfo, &convert);
                if (dar == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outData->resultDataLen = offset;
                        PRTL_BUF_DEBUG(convert.dstBuf, dOffset, "第%d个通道状态:", i);
                        realNum++;
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
    
        if (pOadInfo->nIndex == 0)
        {
            outData->pResultData[numOffset] = realNum;   //重新记数量
        }
    
        outData->resultDataLen = offset;
    
        return dar;
}

#endif

#if DESC("透明事件记录单元", 1)
/**
*********************************************************************
* @brief：      data_to_buf_transparent_events
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
OOP_DAR_E data_to_buf_transparent_event_all(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TRANS_EVENT_T data;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 8;

    //当前事件计数器
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.curNum, sizeof(data.curNum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.curNum(%d)\n", dar, data.curNum);
        return DATA_TYPE_UNMATCHED;
    }

    //本事件存储序号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.selfStoreNo, sizeof(data.selfStoreNo));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), selfStoreNo(%d)\n", dar, data.selfStoreNo);
        return DATA_TYPE_UNMATCHED;
    }

    //事件上报时标
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.reportTime, sizeof(data.reportTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.reportTime(%d)\n", dar, data.reportTime);
        return DATA_TYPE_UNMATCHED;
    }

    //通信地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data.add, sizeof(data.add));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.add.len(%d)\n", dar, data.add.len);
        return DATA_TYPE_UNMATCHED;
    }

    //通信对象类型
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.commType, sizeof(data.commType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.commType(%d)\n", dar, data.commType);
        return DATA_TYPE_UNMATCHED;
    }

    //端口
    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.oad, sizeof(data.oad));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.oad.value(0x%08x)\n", dar, data.oad.value);
        return DATA_TYPE_UNMATCHED;
    }

    //报文格式字
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.messType, sizeof(data.messType));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.messType(%d)\n", dar, data.messType);
        return DATA_TYPE_UNMATCHED;
    }

    //事件内容
    dar = basic_data_to_buf(E_OOP_OCTETVAR2K_T, dstBuf, dstLen, &dOffset, &data.data, sizeof(data.data));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.data.nNum(%d)\n", dar, data.data.nNum);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_event_long_unsigned(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    uint16 data;

    if ((pOadInfo->nIndex != 1) && (pOadInfo->nIndex != 2) && (pOadInfo->nIndex != 7))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_event_datatime(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_DATETIME_S_T data;

    if ((pOadInfo->nIndex != 5) && (pOadInfo->nIndex != 6))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_event_tsa(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_TSA_T data;

    if (pOadInfo->nIndex != 3)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //通信地址
    dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if (dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(data.add, OOP_TSA_SIZE - 1, "basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_event_unsigned(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    uint8 data;

    if ((pOadInfo->nIndex != 4) && ((pOadInfo->nIndex != 8)))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_event_oad(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_OAD_U data;

    if ((pOadInfo->nIndex != 4) && ((pOadInfo->nIndex != 8)))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_event_enum(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    uint8 data;

    if ((pOadInfo->nIndex != 4) && ((pOadInfo->nIndex != 8)))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_event_data(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;

    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;

    OOP_OCTETVAR2K_T data;

    if ((pOadInfo->nIndex != 4) && ((pOadInfo->nIndex != 8)))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. pOadInfo->nIndex(%d)\n", pOadInfo->nIndex);
        return DATA_OBJECT_CLASS_INCONSISTENT;
    }

    if (srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d) dataLen(%d)\n", srcLen, sizeof(data));
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    memset(&data, 0, sizeof(data));
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    dar = basic_data_to_buf(E_OOP_OCTETVAR2K_T, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
* @brief：     从698格式报文转数据中心的数据(福建透明结果内容)
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf        - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
               sOffset       - 转换后源数据区的偏移
               dOffset       - 转换后目的数据区的偏移
* @return：     =0           - 成功
               !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_transparent_event(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E  dar = DATA_SUCCESS;

    switch (pOadInfo->nIndex)
    {
    case 0:
        return data_to_buf_transparent_event_all(pOadInfo, convert);
        break;
    case 1:
    case 2:
        return data_to_buf_transparent_event_long_unsigned(pOadInfo, convert);
        break;
    case 3:
        return data_to_buf_transparent_event_datatime(pOadInfo, convert);
        break;
    case 4:
        return data_to_buf_transparent_event_tsa(pOadInfo, convert);
        break;
    case 5:
        return data_to_buf_transparent_event_unsigned(pOadInfo, convert);
        break;
    case 6:
        return data_to_buf_transparent_event_oad(pOadInfo, convert);
        break;
    case 7:
        return data_to_buf_transparent_event_enum(pOadInfo, convert); 
        break;
    case 8:
        return data_to_buf_transparent_event_data(pOadInfo, convert);
        break;
    default:
        break;
    }

    return dar;
}

#endif

#if DESC("配置统计单元", 1)
/**
*********************************************************************
* @brief：      data_to_buf_config_stat
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
OOP_DAR_E data_to_buf_config_stat(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CONFIG_STAT_T data;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 7;

    //透明任务方案号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.taskId, sizeof(data.taskId));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.taskId(%d)\n", dar, data.taskId);
        return DATA_TYPE_UNMATCHED;
    }

    //端口
    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.port, sizeof(data.port));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), port(0x%08x)\n", dar, data.port.value);
        return DATA_TYPE_UNMATCHED;
    }

    //执行优先级
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.priority, sizeof(data.priority));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.priority(%d)\n", dar, data.priority);
        return DATA_TYPE_UNMATCHED;
    }

    //执行独占性
    dar = basic_data_to_buf(E_BOOL, dstBuf, dstLen, &dOffset, &data.isOnly, sizeof(data.isOnly));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.isOnly(%d)\n", dar, data.isOnly);
        return DATA_TYPE_UNMATCHED;
    }

    //已配置具体任务总数量
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nodeNum, sizeof(data.nodeNum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.nodeNum(%d)\n", dar, data.nodeNum);
        return DATA_TYPE_UNMATCHED;
    }

    //最小具体任务序号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nodeMinId, sizeof(data.nodeMinId));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.nodeMinId(%d)\n", dar, data.nodeMinId);
        return DATA_TYPE_UNMATCHED;
    }

    //最大具体任务序号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nodeMaxId, sizeof(data.nodeMaxId));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.nodeMaxId(%d)\n", dar, data.nodeMaxId);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

#endif

#if DESC("透明任务方案采集状态", 1)
/**
*********************************************************************
* @brief：      data_to_buf_transparent_acq_status
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
OOP_DAR_E data_to_buf_transparent_acq_status(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TRANS_TASK_RLUESTATE_T data;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 10;

    //透明任务方案号
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.transTaskid, sizeof(data.transTaskid));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.transTaskid(%d)\n", dar, data.transTaskid);
        return DATA_TYPE_UNMATCHED;
    }

    //采集存储时标
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.storeTime, sizeof(data.storeTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), storeTime.year(%d)\n", dar, data.storeTime.year);
        return DATA_TYPE_UNMATCHED;
    }

    //存储次数
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.storenum, sizeof(data.storenum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.storenum(%d)\n", dar, data.storenum);
        return DATA_TYPE_UNMATCHED;
    }

    //任务执行开始时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.firststartTime, sizeof(data.firststartTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.firststartTime.month(%d)\n", dar, data.firststartTime.month);
        return DATA_TYPE_UNMATCHED;
    }

    //任务最后执行时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.laststartTime, sizeof(data.laststartTime));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.laststartTime.day(%d)\n", dar, data.laststartTime.day);
        return DATA_TYPE_UNMATCHED;
    }

    //配置总数量
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.paramSum, sizeof(data.paramSum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.paramSum(%d)\n", dar, data.paramSum);
        return DATA_TYPE_UNMATCHED;
    }

    //采集成功数量
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.success, sizeof(data.success));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.success(%d)\n", dar, data.success);
        return DATA_TYPE_UNMATCHED;
    }

    //加工成功数量
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.machingSum, sizeof(data.machingSum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.machingSum(%d)\n", dar, data.machingSum);
        return DATA_TYPE_UNMATCHED;
    }

    //已发送报文条数
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.sendNum, sizeof(data.sendNum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.sendNum(%d)\n", dar, data.sendNum);
        return DATA_TYPE_UNMATCHED;
    }

    
    //已接收确认报文条数
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.receNum, sizeof(data.receNum));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), data.receNum(%d)\n", dar, data.receNum);
        return DATA_TYPE_UNMATCHED;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if ((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    return dar;
}

#endif

#if DESC("class8对外接口函数", 1)

/**
*********************************************************************
* @name：      class8_invoke_get
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
OOP_DAR_E class8_invoke_get_oia_4e_fj(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0; 
	uint8 structData[2048] = {0};
	uint16 rcvLen = 0;

    MEMZERO(&convert, sizeof(convert)); 

    normal.oad.value = pOadInfo->pTab->oad.value;

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x4e010200:
    case 0x4e020200:
		{
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, structData, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen = 6;
                return DATA_SUCCESS;
            }

            PRTL_FMT_LOGCC("class8_invoke_get_addr_octet : data.nNum = %d\n", len);
            PRTL_BUF_LOGCC(structData, len, "class8_invoke_get_addr_octet : data.value:");

            //转换成报文数据
            convert.srcBuf = structData;
            convert.srcLen = len;
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    case 0x4e030200:
        {
            rcvLen = appmain_get_singal_info(1, structData, 2048);
            if(rcvLen != sizeof(OOP_SINGAL_INFO_T))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen = 6;
                return DATA_SUCCESS;
            }

            PRTL_FMT_LOGCC("class8_invoke_get_addr_octet : data.nNum = %d\n", len);
            PRTL_BUF_LOGCC(structData, len, "class8_invoke_get_addr_octet : data.value:");

            //转换成报文数据
            convert.srcBuf = structData;
            convert.srcLen = rcvLen;
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
    dar = data_to_buf_class8(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：       class8_invoke_get
* @brief：      class8的GET服务
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
OOP_DAR_E class8_invoke_get_oia_6e_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x6e00:
    {
        return class8_invoke_get_oia_6e00_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x6e01:
    {
        return class8_invoke_get_oia_6e01_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x6e02:
    {
        return class8_invoke_get_oia_6e02_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x6e03:
    {
        return class8_invoke_get_oia_6e03_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x6e08:
    {
        return class8_invoke_get_oia_6e08_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x6e22:
    {
        return class8_invoke_get_oia_6e22_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x6e25:
    {
        return class8_invoke_get_oia_6e25_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x6e33:
    {
        return class8_invoke_get_oia_6e33_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    default:
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：      class8_invoke_set_oia_6e_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_set_oia_6e_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen, 
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x6e00:
    {
        return class8_invoke_set_oia_6e00_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x6e03:
    {
        return class8_invoke_set_oia_6e03_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    case 0x6e33:
    {
        return class8_invoke_set_oia_6e33_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    default:
        break;
    }
    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：      class8_invoke_act_oia_6e_fj
* @brief：     class8的SET服务
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
OOP_DAR_E class8_invoke_act_oia_6e_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen, 
    RESULT_DATA_T* outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x6e08:
    {
        return class8_invoke_act_oia_6e08_fj(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    }
    default:
        break;
    }
    return DATA_OBJECT_UNAVAILABLE;
}

#endif
