/*
*********************************************************************
* @file    class19.c
* @brief： class19处理
* @author：
* @date：   2021.10.22
* @copyright 2021 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#include "baseCommon.h"
#include "oadInfoList.h"
#include "commClass.h"
#include "oopservice.h"
#include "ac_sample.h"
#include "ac_main.h"

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
OOP_DAR_E data_to_buf_class19_term_var(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    OOP_VERSION_T data;
    MEMZERO(&data, sizeof(data));

    /* 入参检查 */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if (convert->srcLen != sizeof(OOP_VERSION_T))
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
OOP_DAR_E data_to_buf_class19(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x43000300:			   //版本信息
        {
            return data_to_buf_class19_term_var(pOadInfo, convert);
        }break;

        //......

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}