/*
*********************************************************************
* @file    class19.c
* @brief： class19处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "storage.h"
#include "area.h"
#include "msg_id.h"
#include "appoutmsg.h"
    
//
// class19
//

extern int statis_clear_data(DB_CLIENT fd, CLEAR_DATA_T *ptData);
extern int statis_recover_default(DB_CLIENT fd, uint8 logicId);
extern void sta_data_init(uint8 type, DB_CLIENT fd);
extern void rpt_data_init(uint8 type, DB_CLIENT fd);

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
OOP_DAR_E data_to_buf_class19_term_var(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    OOP_VERSION_T data;
    MEMZERO(&data, sizeof(data));

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if(convert->srcLen != sizeof(OOP_VERSION_T))
    {
        dar = DATA_OTHER_REASON;
        return dar;
    }
    memcpy((uint8*)&data, &srcBuf[sOffset], convert->srcLen);
    sOffset += sizeof(OOP_VERSION_T);

    /* 结构类型 */
    dstBuf[dOffset++] = DT_STRUCTURE;

    /* 结构成员数 */
    dstBuf[dOffset++] = 6;

    /* 厂商代码 */
    dar = basic_data_to_buf(E_OOP_VISIBLESTR4_T, dstBuf, convert->dstLen, &dOffset, &data.szFactory, sizeof(OOP_VISIBLESTR4_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("获取厂商代码data-->buf err\n");
        return dar;
    }

    /* 软件版本号 */
    dar = basic_data_to_buf(E_OOP_VISIBLESTR4_T, dstBuf, convert->dstLen, &dOffset, &data.szSoftVer, sizeof(OOP_VISIBLESTR4_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("获取软件版本号data-->buf err\n");
        return dar;
    }

    /* 软件版本日期 */
    dar = basic_data_to_buf(E_OOP_VISIBLESTR6_T, dstBuf, convert->dstLen, &dOffset, &data.szSoftDate, sizeof(OOP_VISIBLESTR6_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("获取软件版本日期data-->buf err\n");
        return dar;
    }

    /* 硬件版本号 */
    dar = basic_data_to_buf(E_OOP_VISIBLESTR4_T, dstBuf, convert->dstLen, &dOffset, &data.szHardVer, sizeof(OOP_VISIBLESTR4_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("获取硬件版本号data-->buf err\n");
        return dar;
    }


    /* 硬件版本日期 */
    dar = basic_data_to_buf(E_OOP_VISIBLESTR6_T, dstBuf, convert->dstLen, &dOffset, &data.szHardDate, sizeof(OOP_VISIBLESTR6_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("获取硬件版本日期data-->buf err\n");
        return dar;
    }

    /* 厂家扩展信息 */
    dar = basic_data_to_buf(E_OOP_VISIBLESTR8_T, dstBuf, convert->dstLen, &dOffset, &data.szExtend, sizeof(OOP_VISIBLESTR8_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("获取厂家扩展信息信息data-->buf err\n");
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
* @brief：     从数据中心的数据转698格式报文(支持规约列表)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class19_term_prtl(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
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

    OOP_PROTOCOL_T data = {0};

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class19_term_prtl error, ERR_PTR\n");
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
        //规约类型
        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.value[i], sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(&data.value[i].value[0], data.value[i].nNum, "data_to_buf_class19_term_prtl failed. [%d] dar(%d),value:\n", i, dar);
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
* @brief：     从数据中心的数据转698格式报文(支持规约列表)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class19_report_channel(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
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

    OOP_CHANNEL_T data = {0};

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class19_report_channel error, ERR_PTR\n");
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
        //通道
        dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.port[i], sizeof(data.port[i]));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_class19_report_channel failed. [%d] dar(%d)\n", i, dar);
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
* @brief：     从数据中心的数据转698格式报文(子设备列表)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class19_ois(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8   offlen    = 0;
    uint8   lenBuf[3] = {0};
    uint32  i = 0;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    OOP_OIS_T data = {0};

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class19_ois error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if(convert->srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("data_to_buf_class19_ois error, DATA_OTHER_REASON\n");
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
        //通道OI
        dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.oi[i], sizeof(data.oi[i]));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_class19_ois failed. [%d] dar(%d)\n", i, dar);
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
* @brief：      服务授权结构
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
OOP_DAR_E data_to_buf_service_auth(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SERVICES_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint8   offlen    = 0;
    uint8   lenBuf[3] = {0};

    MEMZERO(&data, sizeof(data));

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_service_auth error, ERR_PTR\n");
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
        /* 结构类型 */
        dstBuf[dOffset++] = DT_STRUCTURE;

        /* 结构成员数 */
        dstBuf[dOffset++] = 2;
        
        //服务序号
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.item[i].type, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, dOffset);
            return DATA_TYPE_UNMATCHED;
        }
        
        //授权时长
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.item[i].times, sizeof(uint8));
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
* @brief：     从数据中心的数据转698格式报文(class19)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class19_4300(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class19_term_name error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOadInfo->pTab->eData)
    {    
        case E_OOP_VERSION_T:
        {
            /* 电气设备版本信息 */
            return data_to_buf_class19_term_var(pOadInfo, convert);
            break;
        }
        case E_OOP_PROTOCOL_T:
        {
            /* 电气设备规约列表 */
            return data_to_buf_class19_term_prtl(pOadInfo, convert);
            break;
        }
        case E_OOP_CHANNEL_T:
        {
            /* 上报通道 */
            return data_to_buf_class19_report_channel(pOadInfo, convert);
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
        *convert->dOffset = dOffset;
        *convert->sOffset = sOffset;
    }

    return dar;
}
OOP_DAR_E data_to_buf_device_info(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_DEVICERUNSTAT_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);
    
    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 15;
    //CPU核数
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.cpuCoreNum, sizeof(data.cpuCoreNum));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //cpu主频
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.cpuMainFrq, sizeof(data.cpuMainFrq));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //cpu缓存
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.cpuCache, sizeof(data.cpuCache));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //cpu架构
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.cpuArchi, sizeof(data.cpuArchi));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //物理内存
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.phyMem, sizeof(data.phyMem));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //虚拟内存
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.virMem, sizeof(data.virMem));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //磁盘总空间
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.diskSpace, sizeof(data.diskSpace));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //内存使用率
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.memUsage, sizeof(data.memUsage));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //磁盘使用率
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.diskUsage, sizeof(data.diskUsage));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //cpu使用率
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.cpuUsage, sizeof(data.cpuUsage));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //操作系统名称
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.osName, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //操作系统版本
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.osVer, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //操作系统内核
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.osKernel, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //系统启动时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.sysStrtm, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

	//温度
    dar = basic_data_to_buf(E_LONG, dstBuf, dstLen, &dOffset, &data.tempBroad, sizeof(int16));
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

#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)|| (defined AREA_JIANGSU)
/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class19)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_line_lose(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_Line_Loss_T lineLose;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&lineLose, sizeof(lineLose)); 

    memcpy(&lineLose, &srcBuf[sOffset], sizeof(lineLose));
    sOffset += sizeof(lineLose);

    //结构体类型
    dstBuf[dOffset++] = DT_STRUCTURE;
    dstBuf[dOffset++] = 2;
    //线损值
    dstBuf[dOffset++] = DT_DOUBLE_LONG;
    memcpy_r(&dstBuf[dOffset], &lineLose.linelossvalue, 4);
    dOffset += 4;    
    //线损率
    dstBuf[dOffset++] = DT_LONG;
    memcpy_r(&dstBuf[dOffset], &lineLose.linelossrate, 2);
    dOffset += 2;
    
    //避免越界
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
* @brief：     从数据中心的数据转698格式报文(class19)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_switch_impedance(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_Switch_Impedance_T impedance;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    int i;

    MEMZERO(&impedance, sizeof(impedance)); 

    memcpy(&impedance, &srcBuf[sOffset], sizeof(impedance));
    sOffset += sizeof(impedance);

    //数组类型
    dstBuf[dOffset++] = DT_ARRAY;
    dstBuf[dOffset++] = impedance.num;

    for (i = 0; i < impedance.num; i++)
    {
        //结构体类型
        dstBuf[dOffset++] = DT_STRUCTURE;
        dstBuf[dOffset++] = 5;
        //节点地址
        dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &impedance.switchhindrance[i].addr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        //阻抗值
        dstBuf[dOffset++] = DT_DOUBLE_LONG_UNSIGNED;
        memcpy_r(&dstBuf[dOffset], &impedance.switchhindrance[i].lineloss, 4);
        dOffset += 4;
        //电流变化前时间
        dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &impedance.switchhindrance[i].beforchangetime, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        //电流变化后时间
        dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &impedance.switchhindrance[i].afterchangetime, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        //变化电流
        dstBuf[dOffset++] = DT_DOUBLE_LONG;
        memcpy_r(&dstBuf[dOffset], &impedance.switchhindrance[i].Current, 4);
        dOffset += 4;       
    }
    
    //避免越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}
#endif

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class19)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class19(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x43000200:   //电气设备描述
        case 0x43000300:   //电气设备版本
        case 0x43000400:   //电气设备生成日期
        case 0x43000600:   //支持规约列表
        case 0x43000700:   //允许跟随上报
        case 0x43000800:   //允许主动上报
        case 0x43000900:   //允许与主站通话
        case 0x43000a00:   //上报通道
        {
            return data_to_buf_class19_4300(pOadInfo, convert);
        }break;

        case 0x43000500:   //电气设备规约列表
        {
            return data_to_buf_class19_ois(pOadInfo, convert);
        }break;

        case 0x43000B00:   //服务授权状态
        {
            return data_to_buf_service_auth(pOadInfo, convert);
        }break;

        case 0x43000c00:    //设备运行信息
        {
            return data_to_buf_device_info(pOadInfo, convert);
        }
        break;
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
        case 0x43201300:    //智能开关线损
        {
            return data_to_buf_line_lose(pOadInfo, convert);
        }
        break;
        case 0x43201700:    //智能开关回路阻抗
        case 0x43201701:
        case 0x43201702:
        case 0x43201703:
        {
            return data_to_buf_switch_impedance(pOadInfo, convert);
        }
        break;
#endif
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
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
OOP_DAR_E buf_to_data_class19(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}

/**
*********************************************************************
* @name：      class19_invoke_get_term_name
* @brief：     class19的GET服务
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
OOP_DAR_E class19_invoke_get_term_name(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    //NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;  

    MEMZERO(&convert, sizeof(convert)); 

    switch (pOadInfo->pTab->oad.value)
    {
    /* 设备描述 */
    case 0x43000200:
        {
            OOP_VISIBLEVAR32_T data = {0};
            uint8 name[] = "gwmdl_xl";

            memcpy(&data.value[0], &name[0], sizeof(name));
            data.nNum = sizeof(name);

            PRTL_FMT_LOGCC("class19_invoke_get_term_name : data = %d\n", data);

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
     
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
OOP_DAR_E class19_invoke_get_term_var(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;    

    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&normal, sizeof(normal));

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000300:
        {
            OOP_VERSION_T data;
            OOP_VISIBLESTR4_T factory;
            char ver[64] = {0};

            MEMZERO(&data, sizeof(data)); 
            MEMZERO(&factory, sizeof(factory));
            normal.oad.value = 0xfff00400;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_VISIBLESTR4_T), (uint8*)&factory, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                PRTL_FMT_LOGCC("fill in the default factory\n");
                memcpy(&data.szFactory.value[0], &g_tVer.factory[0], 4);
            }
            else
            {
                memcpy(&data.szFactory.value[0], factory.value, 4);
            }

            memcpy(&data.szSoftVer.value[0], &g_tVer.softVer[0], 4);
            memcpy(&data.szExtend.value[0], &g_tVer.extend[5], 3);

            memcpy(&data.szSoftDate.value[0], &g_tVer.softDate[0], 6);
            memcpy(&data.szHardVer.value[0], &g_tVer.hardVer[0], 4);
            memcpy(&data.szHardDate.value[0], &g_tVer.hardDate[0], 6);

            if (app_update_version_get(ver) == 0)
            {
                memcpy(&data.szSoftVer.value[0], ver, 4);
            }

            PRTL_BUF_LOGCC(&data.szFactory.value[0], 8, "class19_invoke_get_term_var : szFactory:");
            PRTL_BUF_LOGCC(&data.szSoftVer.value[0], 8, "class19_invoke_get_term_var : szSoftVer:");
            PRTL_BUF_LOGCC(&data.szSoftDate.value[0], 8, "class19_invoke_get_term_var : szSoftDate:");
            PRTL_BUF_LOGCC(&data.szHardVer.value[0], 8, "class19_invoke_get_term_var : szHardVer:");
            PRTL_BUF_LOGCC(&data.szHardDate.value[0], 8, "class19_invoke_get_term_var : szHardDate:");
            PRTL_BUF_LOGCC(&data.szExtend.value[0], 8, "class19_invoke_get_term_var : szExtend:");

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
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class19_invoke_get_term_date
* @brief：     class19的GET服务
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
OOP_DAR_E class19_invoke_get_term_date(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    //NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;  

    MEMZERO(&convert, sizeof(convert)); 

    switch (pOadInfo->pTab->oad.value)
    {
    /* 设备描述 */
    case 0x43000400:
        {
            OOP_DATETIME_S_T data = {0};

            //填写生产日期
            data.year = 2019;
            data.month = 12;
            data.day = 31;
            data.hour = 14;
            data.minute = 42;
            data.second = 0;
            
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
     
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class19_invoke_get_ois
* @brief：     class19的GET服务
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
OOP_DAR_E class19_invoke_get_ois(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    case 0x43000500:
        {
            uint32 i = 0;
            OOP_OIS_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), len(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, len, normal.logicId, normal.infoNum, normal.oad.value);
            }

            PRTL_FMT_LOGCC("class19_invoke_get_ois data.nNum:%d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class19_invoke_get_ois data.oi[i].value:0x%04x\n", data.oi[i]);
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
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class19_invoke_get_term_date
* @brief：     class19的GET服务
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
OOP_DAR_E class19_invoke_get_term_prtl(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    //NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0; 

    MEMZERO(&convert, sizeof(convert)); 

    switch (pOadInfo->pTab->oad.value)
    {
    /* 设备支持的规约列表 */
    case 0x43000600:
        {
            OOP_PROTOCOL_T data = {0};
            char prtl[] = {"DL/T698.45"};
            //填写规约列表
            data.nNum = 1;
            data.value[0].nNum = sizeof(prtl);
            memcpy(&data.value[0].value[0], prtl, data.value[0].nNum);
            
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
     
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class19_invoke_get_follow_allow
* @brief：     class19的GET服务
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
OOP_DAR_E class19_invoke_get_follow_allow(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    case 0x43000700:
        {
            uint8 data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            PRTL_FMT_LOGCC("class19_invoke_get_report_allow data:%d\n", data);

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(uint8);
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
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class19_invoke_get_report_allow
* @brief：     class19的GET服务
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
OOP_DAR_E class19_invoke_get_report_allow(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    case 0x43000800:
        {
            uint8 data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            PRTL_FMT_LOGCC("class19_invoke_get_report_allow data:%d\n", data);

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(uint8);
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
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class19_invoke_get_call_allow
* @brief：     class19的GET服务
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
OOP_DAR_E class19_invoke_get_call_allow(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    case 0x43000900:
        {
            uint8 data = 0;
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            PRTL_FMT_LOGCC("class19_invoke_get_report_allow data:%d\n", data);

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(uint8);
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
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class19_invoke_get_report_channel
* @brief：     class19的GET服务
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
OOP_DAR_E class19_invoke_get_report_channel(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    case 0x43000a00:
        {
            uint32 i = 0;
            OOP_CHANNEL_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), len(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, len, normal.logicId, normal.infoNum, normal.oad.value);
                
                data.nNum = 2;
                data.port[0].value = 0x45000000;
                data.port[1].value = 0x45100000;
            }

            PRTL_FMT_LOGCC("class19_invoke_get_report_allow data.nNum:%d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class19_invoke_get_report_allow data.port[i].value:0x%08x\n", data.port[i].value);
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
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class19_invoke_get_service_auth
* @brief：     class19的GET服务
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
OOP_DAR_E class19_invoke_get_service_auth(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
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
    case 0x43000B00:
        {
            OOP_SERVICES_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), len(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, len, normal.logicId, normal.infoNum, normal.oad.value);
            }

            //目前服务授权只有SSH，所以不关心nNum
            if(data.nNum == 0)
            {
                data.nNum = 1;
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
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

OOP_DAR_E class19_invoke_get_deviceruninfo(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&normal, sizeof(normal));

    //读出数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000C00:
        {
            OOP_DEVICERUNSTAT_T data;
            appmain_get_deviceinfo(&data);
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
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name：      class19_invoke_get_4300
* @brief：     class19的GET服务
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
OOP_DAR_E class19_invoke_get_4300(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{        
    switch (pOadInfo->pTab->oad.attID)
    {
    case 2:
        return class19_invoke_get_term_name(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 3:
        return class19_invoke_get_term_var(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 4:
        return class19_invoke_get_term_date(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 5:
        return class19_invoke_get_ois(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 6:
        return class19_invoke_get_term_prtl(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 7:
        return class19_invoke_get_follow_allow(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 8:
        return class19_invoke_get_report_allow(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 9:
        return class19_invoke_get_call_allow(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 10:
        return class19_invoke_get_report_channel(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 11:
        return class19_invoke_get_service_auth(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;    
    case 12:
        return class19_invoke_get_deviceruninfo(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @brief：     设置电气设备版本信息
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
OOP_DAR_E buf_to_data_class19_set_term_ver(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_VERSION_T data;
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
    
    //厂商代码
    dar = basic_buf_to_data(E_OOP_VISIBLESTR4_T, srcBuf, srcLen, &sOffset, &data.szFactory, sizeof(OOP_VISIBLESTR4_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //软件版本号
    dar = basic_buf_to_data(E_OOP_VISIBLESTR4_T, srcBuf, srcLen, &sOffset, &data.szSoftVer, sizeof(OOP_VISIBLESTR4_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //软件版本日期
    dar = basic_buf_to_data(E_OOP_VISIBLESTR6_T, srcBuf, srcLen, &sOffset, &data.szSoftDate, sizeof(OOP_VISIBLESTR6_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    //硬件版本号
    dar = basic_buf_to_data(E_OOP_VISIBLESTR4_T, srcBuf, srcLen, &sOffset, &data.szHardVer, sizeof(OOP_VISIBLESTR4_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //硬件版本日期
    dar = basic_buf_to_data(E_OOP_VISIBLESTR6_T, srcBuf, srcLen, &sOffset, &data.szHardDate, sizeof(OOP_VISIBLESTR6_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //厂家扩展信息
    dar = basic_buf_to_data(E_OOP_VISIBLESTR8_T, srcBuf, srcLen, &sOffset, &data.szExtend, sizeof(OOP_VISIBLESTR8_T));
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

    PRTL_BUF_LOGCC(&data.szFactory.value[0], 8, "buf_to_data_class19_set_term_ver : szFactory:");
    PRTL_BUF_LOGCC(&data.szSoftVer.value[0], 8, "buf_to_data_class19_set_term_ver : szSoftVer:");
    PRTL_BUF_LOGCC(&data.szSoftDate.value[0], 8, "buf_to_data_class19_set_term_ver : szSoftDate:");
    PRTL_BUF_LOGCC(&data.szHardVer.value[0], 8, "buf_to_data_class19_set_term_ver : szHardVer:");
    PRTL_BUF_LOGCC(&data.szHardDate.value[0], 8, "buf_to_data_class19_set_term_ver : szHardDate:");
    PRTL_BUF_LOGCC(&data.szExtend.value[0], 8, "buf_to_data_class19_set_term_ver : szExtend:");

    return dar;
}

/**
*********************************************************************
* @brief：     允许跟随上报
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
OOP_DAR_E buf_to_data_class19_set_follow_allow(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));


    //是否允许上报
    dar = basic_buf_to_data(E_BOOL, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
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
* @brief：     设置电气设备版本信息
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
OOP_DAR_E buf_to_data_class19_set_report_allow(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));


    //是否允许上报
    dar = basic_buf_to_data(E_BOOL, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
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
* @brief：     允许与主站通话
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
OOP_DAR_E buf_to_data_class19_set_call_allow(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));


    //是否允许上报
    dar = basic_buf_to_data(E_BOOL, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
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
* @brief：     设置电气设备版本信息
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
OOP_DAR_E buf_to_data_class19_set_report_channel(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CHANNEL_T data = {0};
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
        //通道OAD
        dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.port[i], sizeof(OOP_OAD_U));
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
* @brief：     设置子设备列表
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
OOP_DAR_E buf_to_data_class19_set_ois(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_OIS_T data = {0};
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
        //通道OAD
        dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.oi[i], sizeof(data.oi[i]));
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
* @brief：     从698格式报文转数据中心的数据(class192)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class19_set(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x43000300:   //设置版本信息
        {
            return buf_to_data_class19_set_term_ver(pOadInfo,  convert);
        }break;

        case 0x43000700:   //允许跟随上报
        {
            return buf_to_data_class19_set_follow_allow(pOadInfo,  convert);
        }break;

        case 0x43000800:   //允许主动上报
        {
            return buf_to_data_class19_set_report_allow(pOadInfo,  convert);
        }break;

        case 0x43000900:   //允许主动上报
        {
            return buf_to_data_class19_set_call_allow(pOadInfo,  convert);
        }break;

        case 0x43000500:   //子设备列表
        {
            return buf_to_data_class19_set_ois(pOadInfo,  convert);
        }break;

        case 0x43000a00:   //上报通道
        {
            return buf_to_data_class19_set_report_channel(pOadInfo,  convert);
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
OOP_DAR_E buf_to_data_service_auth(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SERVICE_T data;
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
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.type, sizeof(uint8));
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
* @name：      class19_invoke_set_version
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
OOP_DAR_E class19_invoke_set_version(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;
 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class19_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000300:
        {
            OOP_VERSION_T data;
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = 0x43000300;
            normal.classtag = CLASS_DATA_UNINIT;

            PRTL_FMT_LOGCC("class19_invoke_set_version : data = 0x%08x\n", data);

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_VERSION_T));
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
* @name：      class19_invoke_set_follow_allow
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
OOP_DAR_E class19_invoke_set_follow_allow(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class19_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000700:
        {
            uint8 data;
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class19_invoke_set_report_allow : data = 0x%x\n", data);

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
* @name：      class19_invoke_set_report_allow
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
OOP_DAR_E class19_invoke_set_report_allow(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class19_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000800:
        {
            uint8 data;
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class19_invoke_set_report_allow : data = 0x%x\n", data);

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
* @name：      class19_invoke_set_call_allow
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
OOP_DAR_E class19_invoke_set_call_allow(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class19_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000900:
        {
            uint8 data;
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class19_invoke_set_report_allow : data = 0x%x\n", data);

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
#ifdef PRODUCT_ZCU_1
            //专一的要给电台设置
            uint8 recvbuf[1024]={0};
            uint16 recvlen;
            recvlen = appmain_set_radio_speak(data,recvbuf,sizeof(recvbuf));
            if(recvlen == 1 && recvbuf[0] == 1)
            {
                PRTL_FMT_DEBUG("设置电台通话成功\n");
            }else
            {
                PRTL_FMT_DEBUG("设置电台通话失败 recvlen %d recvbuf[0] %d \n",recvlen,recvbuf[0]);
            }
#endif
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
* @name：      class19_invoke_set_report_allow
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
OOP_DAR_E class19_invoke_set_report_channel(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;
 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class19_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000a00:
        {
            uint32 i = 0;
            OOP_CHANNEL_T data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class19_invoke_set_report_channel : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class19_invoke_set_report_channel : data.port[%d].value = 0x%08x\n", i, data.port[i].value);
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
* @name：      class19_invoke_set_ois
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
OOP_DAR_E class19_invoke_set_ois(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;
 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_class19_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000500:
        {
            uint32 i = 0;
            OOP_OIS_T data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class19_invoke_set_ois : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class19_invoke_set_ois : data.port[%d].value = 0x%08x\n", i, data.oi[i]);
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
* @name：      class8_invoke_set__port_authorize
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
OOP_DAR_E class19_invoke_set_operate_passwd(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_VISIBLEVAR32_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = basic_data_from_fmt698(E_OOP_VISIBLEVAR32_T, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "operate passwd basic_data_from_fmt698 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_VISIBLEVAR32_T));
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
* @name：      class19_invoke_set_4300
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
OOP_DAR_E class19_invoke_set_4300(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000300:
        return class19_invoke_set_version(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;   
    case 0x43000500:
        return class19_invoke_set_ois(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x43000700:
        return class19_invoke_set_follow_allow(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x43000800:
        return class19_invoke_set_report_allow(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x43000900:
        return class19_invoke_set_call_allow(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x43000a00:
        return class19_invoke_set_report_channel(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name：      inner_data_init
* @brief：     内部数据初始化处理
* @return    ：
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
void inner_data_init(uint8 type, DB_CLIENT fd)
{
    rpt_data_init(type, fd);
    sta_data_init(type, fd);
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
OOP_DAR_E class19_invoke_act_data_clear(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    CLEAR_DATA_T clrData;
    uint32  offset  = 0;
    uint8 logicAddr = 0;

    MEMZERO(&clrData, sizeof(clrData));

    //数据初始化操作
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000300:
        {
            if (pApduInfo->NoEsamMac)
            {
                ret = data_init_sent_to_acmeter();
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("data_init_sent_to_acmeter err. ret(%d), logicId(%d)\n",
                    ret, logicAddr);
                    return DATA_HARDWARE_FAULT;
                }
                break;
            }

            logicAddr = pApduInfo->logicAddr;
            clrData.recordOAD.logicId = logicAddr;
            clrData.bClassTag = FALSE;

            //clrData.recordOAD.classtag = CLASS_DATA_INIT;
            ret = statis_clear_data(pApduInfo->hUdp, &clrData);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_clear_data failed. ret(%d), logicId(%d)\n",
                    ret, logicAddr);

                //台体测试显示数据初始化有时会失败，先注释掉
                //return DATA_OTHER_REASON;
            }
            else
            {
                PRTL_FMT_DEBUG("db_clear_data ok. ret(%d), logicId(%d)\n",
                    ret, logicAddr);
            }

            // clrData.recordOAD.classtag = CLASS_DATA_NORMAL;
            // ret = db_clear_data(pApduInfo->hUdp, &clrData);
            // if (ret != ERR_OK)
            // {
            //     PRTL_FMT_DEBUG("db_clear_data failed. ret(%d), logicId(%d)\n",
            //         ret, logicAddr);
            //     return DATA_OTHER_REASON;
            // }
            // else
            // {
            //     PRTL_FMT_DEBUG("db_clear_data ok. ret(%d), logicId(%d)\n",
            //         ret, logicAddr);
            // }

            inner_data_init(PRTL_INIT_DATA, pApduInfo->hUdp);

            //数据初始化则清除断点信息
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));

            //reset_real_proc 后续处理
            set_reset_flag(pApduInfo->hUdp, TMN_DATA_INIT);
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
* @name：      class19_invoke_act_restore_settings
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
OOP_DAR_E class19_invoke_act_param_restore(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset  = 0;
    uint8 logicAddr = pApduInfo->logicAddr;

    ret = statis_recover_default(pApduInfo->hUdp, logicAddr);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_recover_default failed. ret(%d), logicId(%d)\n",
        ret, logicAddr);

        return DATA_OTHER_REASON;
    }
    else
    {
        PRTL_FMT_DEBUG("db_recover_default ok. ret(%d), logicId(%d)\n",
        ret, logicAddr);
    }

    inner_data_init(PRTL_INIT_PARAM, pApduInfo->hUdp);

    //调用设置默认参数的接口 地区相关
    app_setdefault_cfg_area(pApduInfo->hUdp);

    //日志备份处理
    diag_sig_handler(SIGTERM);

    //置复位标记 reset_real_proc 后续处理
    set_reset_flag(pApduInfo->hUdp, TMN_PARA_INIT);

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：      class19_invoke_act_event_clear
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
OOP_DAR_E class19_invoke_act_event_clear(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    CLEAR_DATA_T clrData;
    uint32  offset  = 0;
    uint8 logicAddr = 0;

    MEMZERO(&clrData, sizeof(clrData));

    //数据初始化操作
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000500:
        {
            logicAddr = pApduInfo->logicAddr;
            clrData.recordOAD.logicId = logicAddr;
            clrData.bClassTag = TRUE;

            clrData.recordOAD.classtag = CLASS_DATA_EVENT;
            ret = db_clear_data(pApduInfo->hUdp, &clrData);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_clear_data failed. ret(%d), logicId(%d)\n",
                    ret, logicAddr);

                //台体测试显示数据初始化有时会失败，先注释掉
                //return DATA_OTHER_REASON;
            }
            else
            {
                PRTL_FMT_DEBUG("db_clear_data ok. ret(%d), logicId(%d)\n",
                    ret, logicAddr);
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
* @name：      class19_invoke_act_demand_clear
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
OOP_DAR_E class19_invoke_act_demand_clear(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset  = 0;

    //需量初始化操作
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000600:
        {
            if (pApduInfo->NoEsamMac)
            {
                ret = demand_init_sent_to_acmeter();
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("demand_init_sent_to_acmeter err. ret(%d), logicId(%d)\n",
                        ret, pApduInfo->logicAddr);
                    return DATA_HARDWARE_FAULT;
                }
                break;
            }
            else
            {
                return DATA_OBJECT_UNAVAILABLE;
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
* @name：      class19_invoke_act_service_auth
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
OOP_DAR_E class19_invoke_act_service_auth(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;
    int i = 0;
    uint32 len = 0;
    uint8 msgdata[8] = {0};
    uint16 msglen=0;
    uint32 authtime = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    dar = buf_to_data_service_auth(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "class19_invoke_act_service_auth failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //数据初始化操作
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000700:
        {
            OOP_SERVICE_T dataAct;
            OOP_SERVICES_T dataGet = {0};
            
            memcpy(&dataAct, convert.dstBuf, dOffset);
            //能源控制器的还按照255发
#if !defined(PRODUCT_ECU)
            //发给系统管理器的消息是4个字节 这边做下转换
            if(dataAct.times == 255)
                authtime = 0xffffffff;
            else
#endif
                authtime = dataAct.times;

            msgdata[msglen++] = 1;
            msgdata[msglen++] = dataAct.type;
            memcpy_r(&msgdata[msglen], &authtime, sizeof(uint32));
            msglen += sizeof(uint32);

            ret = appmain_send_msg(MSG_APPAMIN_SERVICE_START, 0, msgdata, msglen);
            if(ret != ERR_OK)
            {
                AUTH_FMT_DEBUG("auth msg send failed. ret(%d)\n",ret);
                return DATA_OTHER_REASON;
            }

            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(dataGet), (uint8*)&dataGet, &len);

            for(i = 0; i < dataGet.nNum; i++)
            {
                if(dataGet.item[i].type == dataAct.type)
                {
                    memcpy(&dataGet.item[i], &dataAct, sizeof(OOP_SERVICE_T));
                    break;
                }
            }

            if((i == dataGet.nNum) && (i < OOP_MAX_SERVICES))
            {
                memcpy(&dataGet.item[i], &dataAct, sizeof(OOP_SERVICE_T));
                dataGet.nNum++;
            }
            
            normal.oad.nObjID  = pOadInfo->pTab->oad.nObjID;
            normal.oad.attID = 11;
            normal.classtag = CLASS_DATA_UNINIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataGet, sizeof(OOP_SERVICES_T));
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
* @name：      class19_invoke_act_screen_passwd
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
OOP_DAR_E class19_invoke_act_operate_passwd(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
//    NOMAL_OAD_T normal = {0};
    OOP_VISIBLEVAR32_T data;
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
    dar = basic_data_from_fmt698(E_OOP_VISIBLEVAR32_T, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "operate passwd basic_data_from_fmt698 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //更改液晶密码，偏移4，长度6
    ret = write_pdata_xram("^gui_paivate_file", data.value, 4, strlen(data.value));
    ret |= write_pdata_xram("/data/app/desktopGui/data/^gui_paivate_file", data.value, 4, strlen(data.value));
    if (ret < 0)
    {
        PRTL_FMT_DEBUG("operate_passwd err. ret[%d]\n", ret);
        return DATA_READ_WRITE_DENIED;
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
* @name：      class19_invoke_act_reboot
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
OOP_DAR_E class19_invoke_act_reboot(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E   dar = DATA_SUCCESS;
    uint32      offset  = 0;
    OOP_WORD2_T data;

    MEMZERO(&data, sizeof(data));

    //终端复位
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000100:
        {
            if(pApduInfo->logicAddr != 0)
            {
                return dar;
            }
            //日志备份处理
            diag_sig_handler(SIGTERM);
            
            //置复位标记 reset_real_proc 后续处理
            set_reset_flag(pApduInfo->hUdp, TMN_REBOOT);
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
* @name：      class19_invoke_act_4300
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
OOP_DAR_E class19_invoke_act_4300(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000100: //复位
        return class19_invoke_act_reboot(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;    
    case 0x43000300: //数据初始化
        return class19_invoke_act_data_clear(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x43000400: //恢复出厂参数
        return class19_invoke_act_param_restore(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;   
    case 0x43000500: //事件初始化
        return class19_invoke_act_event_clear(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x43000600: //需量初始化
        return class19_invoke_act_demand_clear(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;   
    case 0x43000700: //服务授权
        return class19_invoke_act_service_auth(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;    
    case 0x43007f00: //设置液晶操作密码
        return class19_invoke_act_operate_passwd(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;       
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

#if DESC("class19对外接口函数",1)

/**
*********************************************************************
* @name：      class19_invoke_set
* @brief：     class19的SET服务
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
OOP_DAR_E class19_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x4300:
        return class19_invoke_set_4300(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：      class19_invoke_get
* @brief：     class19的GET服务
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
OOP_DAR_E class19_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x4300:
        {
            return class19_invoke_get_4300(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name：       class19_invoke_act
* @brief：      class19的ACTION服务
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
OOP_DAR_E class19_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x4300:
        return class19_invoke_act_4300(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break;    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}


#endif
