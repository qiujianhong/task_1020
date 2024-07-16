/*
*********************************************************************
* @file    class2.c
* @brief： class2处理
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
// class2
//

OOP_DAR_E buf_to_data_class2(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    return dar;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class2)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class2(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
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
        ACUDP_FMT_TRACE("data_to_buf_class2() 入参检查 error, ERR_PTR\n");
        return ERR_PTR;
    }

    switch (pOadInfoTmp->pTab->eData)
    {
    case E_OOP_DEMAND_T:  //需量(不带符号)
    {
    	ACUDP_FMT_TRACE("data_to_buf_class2() case E_OOP_DEMAND_T:  //需量(不带符号) \n");
        OOP_DEMAND_T oopdemand;
        if (dsdata->srcLen != sizeof(OOP_DEMAND_T))
        {
            nRet = ERR_OPTION;
            break;
        }
        memcpy(&oopdemand, srcDataTmp, dsdata->srcLen);
        offlen = set_len_offset(oopdemand.nNum, lenBuf);

        dstBufTmp[offset] = pOadInfoTmp->pTab->dataType;
        offset ++;
        memcpy(&dstBufTmp[1], lenBuf, offlen); 
        offset += offlen;

        for (i = 0; i < oopdemand.nNum; i++)
        {
            dstBufTmp[offset++] = DT_STRUCTURE;
            dstBufTmp[offset++] = 0x02;
            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &oopdemand.demand[i].nValue, sizeof(uint32)) < 0)
		    {
		        nRet = ERR_NORMAL;
		        return nRet;
		    }
			
			if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dsdata->dstLen-offset, &offset, &oopdemand.demand[i].time, sizeof(oopdemand.demand[i].time)) < 0)
		    {
		        nRet = ERR_NORMAL;
		        return nRet;
		    }
			//dstBufTmp[offset++] =0;
        }
        nRet = ERR_OK;
     
    }
	break;
    case E_OOP_DEMANDS_T:  //需量(带符号)
    {
    	ACUDP_FMT_TRACE("data_to_buf_class2() case E_OOP_DEMANDS_T:  //需量(带符号) \n");
        OOP_DEMANDS_T oopdemands;
        if (dsdata->srcLen != sizeof(OOP_DEMANDS_T))
        {
            nRet = ERR_OPTION;
            break;
        }
        memcpy(&oopdemands, srcDataTmp, dsdata->srcLen);
        offlen = set_len_offset(oopdemands.nNum, lenBuf);

        dstBufTmp[offset] = pOadInfoTmp->pTab->dataType;
        offset ++;
        memcpy(&dstBufTmp[1], lenBuf, offlen); 
        offset += offlen;

        for (i = 0; i < oopdemands.nNum; i++)
        {
            dstBufTmp[offset++] = DT_STRUCTURE;
            dstBufTmp[offset++] = 0x02;
            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &oopdemands.demand[i].nValue, sizeof(uint32)) < 0)
		    {
		        nRet = ERR_NORMAL;
		        return nRet;
		    }
			
			if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dsdata->dstLen-offset, &offset, &oopdemands.demand[i].time, sizeof(oopdemands.demand[i].time)) < 0)
		    {
		        nRet = ERR_NORMAL;
		        return nRet;
		    }
			//dstBufTmp[offset++] =0;
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


