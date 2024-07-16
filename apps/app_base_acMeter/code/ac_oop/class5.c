/*
*********************************************************************
* @file    class5.c
* @brief�� class5����
* @author��
* @date��   2019.12.06
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
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
* @brief��     ���������ĵ�����ת698��ʽ����(class5)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
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

    /*��μ��*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        ACUDP_FMT_TRACE("data_to_buf_class5() ��μ�� error, ERR_PTR\n");
        return ERR_PTR;
    }

    switch (pOadInfoTmp->pTab->eData)
    {
	    case E_OOP_HARMONIC_T://г��������
	    {
	    	ACUDP_FMT_TRACE("data_to_buf_class5() case E_OOP_HARMONIC_T://г�������� \n");
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
		case E_OOP_HARMONIC_U_VALUE_T://��ѹг������
	    {
	    	ACUDP_FMT_TRACE("data_to_buf_class5() case E_OOP_HARMONIC_U_VALUE_T://��ѹг������ \n");
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
		case E_OOP_HARMONIC_I_VALUE_T://����г������
	    {
	    	ACUDP_FMT_TRACE("data_to_buf_class5() case E_OOP_HARMONIC_I_VALUE_T://����г������ \n");
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


