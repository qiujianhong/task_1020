/*
*********************************************************************
* @file    class4.c
* @brief�� class4����
* @author��
* @date��   2019.12.06
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"

//
// class4
//

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class4)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class4(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
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
    uint8 nIndex=0;

    /*��μ��*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class2 error, ERR_PTR\n");
        return ERR_PTR;
    }

    switch (pOadInfoTmp->pTab->eData)
    {
    case E_OOP_INT4V_T:                      
    {
        if(pOadInfo->nIndex==0)
        {
            OOP_INT4V_T INT4V_T;
            if (dsdata->srcLen != sizeof(OOP_INT4V_T))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(&INT4V_T, srcDataTmp, dsdata->srcLen);
            offlen = set_len_offset(INT4V_T.nNum, lenBuf);

            dstBufTmp[offset] = pOadInfoTmp->pTab->dataType;
            offset ++;
            memcpy(&dstBufTmp[offset], lenBuf, offlen); 
            offset += offlen;

            for (i = 0; i < INT4V_T.nNum; i++)
            {
                if(0==get_bit_value((uint8*)&INT4V_T.rsv,32,i))
                {
                    dstBufTmp[offset++] = DT_NULL;
                    continue;
                }
                if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen, &offset, &INT4V_T.nValue[i], sizeof(int32)) < 0)
    		    {
    		    	PRTL_FMT_DEBUG("basic_data_to_buf err offset is %d\n",offset);
    		        nRet = ERR_NORMAL;
    		        return nRet;
    		    }
            }
            nRet = ERR_OK;
        }
        else
        {
            if(pOadInfo->nIndex>5)
            {
                return ERR_NORMAL;
            }
            OOP_INT4V_T INT4V_T;
            if (dsdata->srcLen != sizeof(OOP_INT4V_T))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(&INT4V_T, srcDataTmp, dsdata->srcLen);
            nIndex = pOadInfo->nIndex-1;

            if(0==get_bit_value((uint8*)&INT4V_T.rsv,32,nIndex))
            {
                dstBufTmp[offset++] = DT_NULL;
            }
            else if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen, &offset, &INT4V_T.nValue[nIndex], sizeof(int32)) < 0)
		    {
		    	PRTL_FMT_DEBUG("basic_data_to_buf err offset is %d\n", offset);
		        nRet = ERR_NORMAL;
		        return nRet;
		    }
            nRet = ERR_OK;
        }
     
    }
	break;
    case E_OOP_LONG4V_T:                      
    {
        if(pOadInfo->nIndex==0)
        {
            OOP_LONG4V_T LONG4V_T;
            if (dsdata->srcLen != sizeof(OOP_LONG4V_T))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(&LONG4V_T, srcDataTmp, dsdata->srcLen);
            offlen = set_len_offset(LONG4V_T.nNum, lenBuf);

            dstBufTmp[offset] = pOadInfoTmp->pTab->dataType;
            offset ++;
            memcpy(&dstBufTmp[offset], lenBuf, offlen); 
            offset += offlen;

            for (i = 0; i < LONG4V_T.nNum; i++)
            {
                if(0==get_bit_value((uint8*)&LONG4V_T.rsv,32,i))
                {
                    dstBufTmp[offset++] = DT_NULL;
                    continue;
                }
                if(basic_data_to_buf(E_LONG, dstBufTmp, dsdata->dstLen, &offset, &LONG4V_T.nValue[i], sizeof(int16)) < 0)
    		    {
    		        nRet = ERR_NORMAL;
    		        return nRet;
    		    }

            }
            nRet = ERR_OK;
        }
        else
        {
            if(pOadInfo->nIndex>5)
            {
                return ERR_NORMAL;
            }
            OOP_LONG4V_T LONG4V_T;
            if (dsdata->srcLen != sizeof(OOP_LONG4V_T))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(&LONG4V_T, srcDataTmp, dsdata->srcLen);
            nIndex = pOadInfo->nIndex-1;
            if(0==get_bit_value((uint8*)&LONG4V_T.rsv,32,nIndex))
            {
                dstBufTmp[offset++] = DT_NULL;
                    
            }
            else if(basic_data_to_buf(E_LONG, dstBufTmp, dsdata->dstLen, &offset, &LONG4V_T.nValue[nIndex], sizeof(int16)) < 0)
		    {
		        nRet = ERR_NORMAL;
		        return nRet;
		    }
            nRet = ERR_OK;
        }
       
    }
	 break;
    default:
        nRet = ERR_OPTION;
        break;
    }

    *dsdata->dOffset = offset;

    return nRet;
}

OOP_DAR_E buf_to_data_class4(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return DATA_SUCCESS;
}


#if DESC("class4����ӿں���",1)

/**
*********************************************************************
* @name��       class4_invoke_set
* @brief��      class4��SET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E class4_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}

/**
*********************************************************************
* @name��       class4_invoke_get
* @brief��      class4��GET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E class4_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
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

    NormalOad.logicId = 1;
    NormalOad.infoNum = 0;
    NormalOad.oad.value = pOdaInfo->pTab->oad.value;

    ret = db_read_nomal(pApduInfo->hUdp, &NormalOad, BUF_LEN_MAX, srcData, &srcLen);
    if((ret != 0)||(srcLen==0))
    {
        PRTL_FMT_DEBUG("class4_invoke_get db_read_nomal err = %d\n", ret);
        
        //�������ĳ�����NULL
        return oop_null_array_res(pOdaInfo, pApduInfo, inData, inLen, outData, outDataMax);
    }
	
	dsdata.srcBuf= srcData;
	dsdata.srcLen= srcLen;
	dsdata.sOffset= &sourceoffset;
	dsdata.dstBuf= dstbuf;
	dsdata.dstLen= 1000;
	dsdata.dOffset= &desoffset;
	
    PRTL_FMT_LOGCC("class4_invoke_get len is %d\n", srcLen);
    PRTL_FMT_LOGCC("class4_invoke_get pOdaInfo->pTab->oad is %x\n", pOdaInfo->pTab->oad.value);
    PRTL_FMT_LOGCC("class4_invoke_get pOdaInfo->pTab->eData is %d\n", pOdaInfo->pTab->eData);

    ret = data_to_buf_class4(pOdaInfo, &dsdata);
	outLen=*dsdata.dOffset;
    if((ret == ERR_OK) && (outLen > 0))
    {	
        memcpy(&outData->pResultData[offset], inData, 4);
        offset += sizeof(OOP_OAD_U);
        outData->pResultData[offset] = 0x01;
        offset ++;
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
* @name��       class4_invoke_act
* @brief��      class4��ACTION����
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E class4_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}


#endif
