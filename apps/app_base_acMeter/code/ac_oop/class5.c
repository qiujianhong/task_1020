/*
*********************************************************************
* @file    class5.c
* @brief： class5处理
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
// class5
//

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class5)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class5(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;
    uint8      offlen = 0;
    uint8      lenBuf[3] = {0};
    uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        ACUDP_FMT_TRACE("data_to_buf_class5() 入参检查 error, ERR_PTR\n");
        return ERR_PTR;
    }

    switch (pOadInfoTmp->pTab->eData)
    {
	    case E_OOP_HARMONIC_T://谐波含有率
	    {
	    	ACUDP_FMT_TRACE("data_to_buf_class5() case E_OOP_HARMONIC_T://谐波含有率 \n");
	        OOP_HARMONIC_T HARMONIC_T;
	        if(dsdata->srcLen != sizeof(OOP_HARMONIC_T))
	        {
	            nRet = ERR_OPTION;
	            break;
	        }
	        memcpy((uint8*)&HARMONIC_T,srcDataTmp,dsdata->srcLen);
	        offlen = set_len_offset(HARMONIC_T.nNum, lenBuf);

	        dstBufTmp[offset] = pOadInfoTmp->pTab->dataType;
	        offset ++;
	        memcpy(&dstBufTmp[1], lenBuf, offlen); 
	        offset += offlen;

	        for (i = 0; i < HARMONIC_T.nNum; i++)
	        {
	            if(basic_data_to_buf(E_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, &HARMONIC_T.nValue[i], sizeof(int16)) < 0)
			    {
			        nRet = ERR_NORMAL;
			        return nRet;
			    }
	        }
	        nRet = ERR_OK;
	     
	    }
			break;
		case E_OOP_HARMONIC_U_VALUE_T://电压谐波含量
	    {
	    	ACUDP_FMT_TRACE("data_to_buf_class5() case E_OOP_HARMONIC_U_VALUE_T://电压谐波含量 \n");
	        OOP_HARMONIC_VOL_T HARMONIC_T;
	        if(dsdata->srcLen != sizeof(OOP_HARMONIC_VOL_T))
	        {
	            nRet = ERR_OPTION;
	            break;
	        }
	        memcpy((uint8*)&HARMONIC_T,srcDataTmp,dsdata->srcLen);
	        offlen = set_len_offset(HARMONIC_T.nNum, lenBuf);

	        dstBufTmp[offset] = pOadInfoTmp->pTab->dataType;
	        offset ++;
	        memcpy(&dstBufTmp[1], lenBuf, offlen); 
	        offset += offlen;

			for (i = 0; i < HARMONIC_T.nNum; i++)
	        {
	            if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &HARMONIC_T.nValue[i], sizeof(uint16)) < 0)
			    {
			        nRet = ERR_NORMAL;
			        return nRet;
			    }
	        }
	        nRet = ERR_OK;
	     
	    }
			break;
		case E_OOP_HARMONIC_I_VALUE_T://电流谐波含量
	    {
	    	ACUDP_FMT_TRACE("data_to_buf_class5() case E_OOP_HARMONIC_I_VALUE_T://电流谐波含量 \n");
	        OOP_HARMONIC_I_VALUE_T HARMONIC_T;
	        if(dsdata->srcLen != sizeof(OOP_HARMONIC_I_VALUE_T))
	        {
	            nRet = ERR_OPTION;
	            break;
	        }
	        memcpy((uint8*)&HARMONIC_T,srcDataTmp,dsdata->srcLen);
	        offlen = set_len_offset(HARMONIC_T.nNum, lenBuf);

	        dstBufTmp[offset] = pOadInfoTmp->pTab->dataType;
	        offset ++;
	        memcpy(&dstBufTmp[1], lenBuf, offlen); 
	        offset += offlen;

	        for (i = 0; i < HARMONIC_T.nNum; i++)
	        {
	            if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, &HARMONIC_T.nValue[i], sizeof(int32)) < 0)
			    {
			        nRet = ERR_NORMAL;
			        return nRet;
			    }
	        }
	        nRet = ERR_OK;
	     
	    }
			break;
    default:
        nRet = ERR_OPTION;
        break;
    }

    *dsdata->dOffset = offset;

    return nRet;
}

OOP_DAR_E buf_to_data_class5(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return DATA_SUCCESS;
}


