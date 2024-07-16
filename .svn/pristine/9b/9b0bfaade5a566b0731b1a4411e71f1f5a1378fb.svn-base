/*
*********************************************************************
* @file    class3.c
* @brief： class3处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "area.h"

//
// class3
//

OOP_DAR_E buf_to_data_class3(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    return dar;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class3)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3_oop_word3v(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    ARRAY_INFO_T arrayInfo;

    MEMZERO(&arrayInfo, sizeof(arrayInfo));

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class3_oop_word3v error, ERR_PTR\n");
        return ERR_PTR;
    }
    OOP_WORD3V_T WORD3V_T;

    //注意：传进来的srcLen可能只是数组成员的长度
    if (dsdata->srcLen == sizeof(OOP_WORD3V_T))
    {
        memcpy(&WORD3V_T, srcDataTmp, dsdata->srcLen);
    }
    
    arrayInfo.arrLen = sizeof(OOP_WORD3V_T);
    arrayInfo.pArray = WORD3V_T.nValue;
    arrayInfo.maxNum = 3;
    arrayInfo.rsv = WORD3V_T.rsv;
    arrayInfo.unitNum = WORD3V_T.nNum;
    arrayInfo.unitSize = sizeof(uint16);
    arrayInfo.func = basic_db_to_fmt698;
    
    pOadInfoTmp->subeData = E_LONG_UNSIGNED; //重要，基本数据一定要填子结构的数据类型
    
    nRet = class_array_tofmt698(pOadInfoTmp, &arrayInfo, dsdata);

    return nRet;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class3)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3_oop_int3v(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    ARRAY_INFO_T arrayInfo;

    MEMZERO(&arrayInfo, sizeof(arrayInfo));

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class3_oop_int3v error, ERR_PTR\n");
        return ERR_PTR;
    }
    OOP_INT3V_T INT3V_T;

    //注意：传进来的srcLen可能只是数组成员的长度
    if (dsdata->srcLen == sizeof(OOP_INT3V_T))
    {
        memcpy(&INT3V_T, srcDataTmp, dsdata->srcLen);
    }
    
    arrayInfo.arrLen = sizeof(OOP_WORD3V_T);
    arrayInfo.pArray = INT3V_T.nValue;
    arrayInfo.maxNum = 3;
    arrayInfo.rsv = INT3V_T.rsv;
    arrayInfo.unitNum = INT3V_T.nNum;
    arrayInfo.unitSize = sizeof(int32);
    arrayInfo.func = basic_db_to_fmt698;
    
    pOadInfoTmp->subeData = E_DOUBLE_LONG; //重要，基本数据一定要填子结构的数据类型
    
    nRet = class_array_tofmt698(pOadInfoTmp, &arrayInfo, dsdata);

    return nRet;
}


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class3)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3_oop_long3v(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    ARRAY_INFO_T arrayInfo;
    
    MEMZERO(&arrayInfo, sizeof(arrayInfo));

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class3_oop_long3v error, ERR_PTR\n");
        return ERR_PTR;
    }

    OOP_LONG3V_T LONG3V_T;

    if (dsdata->srcLen == sizeof(OOP_LONG3V_T))
    {
        memcpy(&LONG3V_T, srcDataTmp, dsdata->srcLen);
    }
    
    arrayInfo.arrLen = sizeof(OOP_LONG3V_T);
    arrayInfo.pArray = LONG3V_T.nValue;
    arrayInfo.maxNum = 3;
    arrayInfo.rsv = LONG3V_T.rsv;
    arrayInfo.unitNum = LONG3V_T.nNum;
    arrayInfo.unitSize = sizeof(int16);
    arrayInfo.func = basic_db_to_fmt698;

    pOadInfoTmp->subeData = E_LONG; //重要，基本数据一定要填子结构的数据类型
    
    nRet = class_array_tofmt698(pOadInfoTmp, &arrayInfo, dsdata);    

    return nRet;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class3)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3_oop_dword3v(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    ARRAY_INFO_T arrayInfo;

    MEMZERO(&arrayInfo, sizeof(arrayInfo));

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class3_oop_dword3v error, ERR_PTR\n");
        return ERR_PTR;
    }
    OOP_DWORD3V_T DWORD3V_T;

    //注意：传进来的srcLen可能只是数组成员的长度
    if (dsdata->srcLen == sizeof(OOP_DWORD3V_T))
    {
        memcpy(&DWORD3V_T, srcDataTmp, dsdata->srcLen);
    }
    
    arrayInfo.arrLen = sizeof(OOP_DWORD3V_T);
    arrayInfo.pArray = DWORD3V_T.nValue;
    arrayInfo.maxNum = 3;
    arrayInfo.rsv = DWORD3V_T.rsv;
    arrayInfo.unitNum = DWORD3V_T.nNum;
    arrayInfo.unitSize = sizeof(uint32);
    arrayInfo.func = basic_db_to_fmt698;
    
    pOadInfoTmp->subeData = E_DOUBLE_LONG_UNSIGNED; //重要，基本数据一定要填子结构的数据类型
    
    nRet = class_array_tofmt698(pOadInfoTmp, &arrayInfo, dsdata);

    return nRet;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class3)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    pOadInfoTmp = pOadInfo;


    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class3 error, ERR_PTR\n");
        return ERR_PTR;
    }
    
    switch (pOadInfoTmp->pTab->eData)
    {
        case E_OOP_WORD3V_T: 
            return data_to_buf_class3_oop_word3v(pOadInfo, dsdata);
        break;
        case E_OOP_INT3V_T:
            return data_to_buf_class3_oop_int3v(pOadInfo, dsdata);
        break;
        case E_OOP_LONG3V_T:
            return data_to_buf_class3_oop_long3v(pOadInfo, dsdata);
        break;   
        case E_OOP_DWORD3V_T:
            return data_to_buf_class3_oop_dword3v(pOadInfo, dsdata);
        break;       

        default:
            return basic_db_to_fmt698(pOadInfo, dsdata);
        break;
    }

    return nRet;
}

#if DESC("class3对外接口函数",1)

/**
*********************************************************************
* @name：       class3_invoke_set
* @brief：      class3的SET服务
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
OOP_DAR_E class3_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}

/**
*********************************************************************
* @name：       class3_invoke_get
* @brief：      class3的GET服务
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
OOP_DAR_E class3_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_NORMAL;
    uint32 offset = 0;
    uint32 sourceoffset=0;
    uint32 desoffset=0;
    uint32 srcLen = 0;
    int nRet = ERR_NORMAL;
    uint8 srcData[BUF_LEN_MAX] = {0};
    uint8 dstbuf[2*BUF_LEN_MAX] = {0};
    NOMAL_OAD_T NormalOad = {0};
//    uint32 oadInfoOffset = 0;
    uint32 outLen = 0;
    DATA_CONVERT_T dsdata;

    if(pOadInfo->pTab->oad.value != 0x2a030200)
    {
        NormalOad.logicId = 1;
    }
    
    NormalOad.infoNum = 0;
    NormalOad.oad.value = pOadInfo->pTab->oad.value;
    NormalOad.oad.nIndex = 0;

    PRTL_FMT_LOGCC("class3_invoke_get11\n");
    ret = db_read_nomal(pApduInfo->hUdp, &NormalOad, BUF_LEN_MAX, srcData, &srcLen);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("class2_invoke_get db_read_nomal err = %d\n", ret);
        
        //数据中心出错返回NULL
        return oop_null_array_res(pOadInfo, pApduInfo, inData, inLen, outData, outDataMax);
    }
    
    PRTL_FMT_LOGCC("class3_invoke_get22\n");
    dsdata.srcBuf= srcData;
    dsdata.srcLen= srcLen;
    dsdata.sOffset= &sourceoffset;
    dsdata.dstBuf= dstbuf;
    dsdata.dstLen= 1000;
    dsdata.dOffset= &desoffset;
    
    PRTL_FMT_LOGCC("class2_invoke_get len is %d\n", srcLen);
    PRTL_FMT_LOGCC("class2_invoke_get pOdaInfo->pTab->oad is %x\n", pOadInfo->pTab->oad.value);
    PRTL_FMT_LOGCC("class2_invoke_get pOdaInfo->pTab->eData is %d\n", pOadInfo->pTab->eData);

    ret = data_to_buf_class3(pOadInfo, &dsdata);
    outLen=*dsdata.dOffset;
    if((ret == ERR_OK) && (outLen > 0))
    {
        if(!pOadInfo->noParse)
        {
            memcpy(&outData->pResultData[offset], inData, 4); //回码从报文中拷贝，oad信息中可能是模糊查表结果
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset] = 0x01;
            offset ++;
        }
        
        memcpy(&outData->pResultData[offset], dsdata.dstBuf, outLen);
        outData->resultDataLen = offset + outLen;
        nRet = DATA_SUCCESS;
    }
    else
    {
        nRet = DATA_READ_WRITE_DENIED;
    }

    return nRet;
}

/**
*********************************************************************
* @name：       class3_invoke_act
* @brief：      class3的ACTION服务
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
OOP_DAR_E class3_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}


#endif
