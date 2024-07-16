/*
*********************************************************************
* @file    class2.c
* @brief�� class2����
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
// class2
//

OOP_DAR_E buf_to_data_class2(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    return dar;
}

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class2)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class2_all(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;
    uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;
    uint32 offsetNum = 0;
    uint32 num = 0;

    /*��μ��*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class2 error, ERR_PTR\n");
        return ERR_PTR;
    }

    switch (pOadInfoTmp->pTab->eData)
    {
    case E_OOP_DEMAND_T:                      //0xxx-������(��������)
    {
        OOP_DEMAND_T oopdemand;
        if (dsdata->srcLen != sizeof(OOP_DEMAND_T))
        {
            nRet = ERR_OPTION;
            break;
        }
        memcpy(&oopdemand, srcDataTmp, dsdata->srcLen);

        dstBufTmp[offset++] = pOadInfoTmp->pTab->dataType;

        offsetNum = offset++;

        for (i = 0; i < oopdemand.nNum; i++)
        {
            if (0 == get_bit_value((uint8*)&oopdemand.rsv, 32, i))
            {
                continue;
            }
            num++;

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
        }

        dstBufTmp[offsetNum] = num;
        
        nRet = ERR_OK;
     
    }
    break;
    case E_OOP_DEMANDS_T:                      //������(������)
    {
        OOP_DEMANDS_T oopdemands;
        if (dsdata->srcLen != sizeof(OOP_DEMANDS_T))
        {
            nRet = ERR_OPTION;
            break;
        }
        memcpy(&oopdemands, srcDataTmp, dsdata->srcLen);

        dstBufTmp[offset++] = pOadInfoTmp->pTab->dataType;

        offsetNum = offset++;

        for (i = 0; i < oopdemands.nNum; i++)
        {
            if (0 == get_bit_value((uint8*)&oopdemands.rsv, 32, i))
            {
                continue;
            }

            num++;

            dstBufTmp[offset++] = DT_STRUCTURE;
            dstBufTmp[offset++] = 0x02;
            if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, &oopdemands.demand[i].nValue, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            
            if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dsdata->dstLen-offset, &offset, &oopdemands.demand[i].time, sizeof(oopdemands.demand[i].time)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
        }

        dstBufTmp[offsetNum] = num;
        
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

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class2)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class2_one(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    OOP_DAR_E  dar = DATA_SUCCESS;
	int32      nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;
    uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;

    /*��μ��*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class2 error, ERR_PTR\n");
        return ERR_PTR;
    }

    switch (pOadInfoTmp->pTab->eData)
    {
    case E_OOP_DEMAND_T:                      //0xxx-������(��������)
    {
        OOP_DEMAND_T oopdemand;

        if ((pOadInfo->nIndex > OOP_FULL_RATES) || (pOadInfo->nIndex == 0))
        {
            return ERR_NORMAL;
        }
        if (dsdata->srcLen != sizeof(OOP_DEMAND_T))
        {
            nRet = ERR_OPTION;
            break;
        }

        memcpy(&oopdemand, srcDataTmp, dsdata->srcLen);

        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 0x02;
        i = pOadInfo->nIndex - 1;
        dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, &oopdemand.demand[i].nValue, sizeof(uint32));
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oopdemand.demand[i].nValue(%d)\n", i, dar, oopdemand.demand[i].nValue);
            return DATA_TYPE_UNMATCHED;
        }
        
        dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dsdata->dstLen-offset, &offset, &oopdemand.demand[i].time, sizeof(oopdemand.demand[i].time));
        if (dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oopdemand.demand[i].time(%d)\n", i, dar, oopdemand.demand[i].time);
            return DATA_TYPE_UNMATCHED;
        }
        nRet = ERR_OK;
    }
	break;
    case E_OOP_DEMANDS_T:                      //������(������)
    {
        OOP_DEMANDS_T oopdemands;

        if ((pOadInfo->nIndex > OOP_FULL_RATES) || (pOadInfo->nIndex == 0))
        {
            return ERR_NORMAL;
        }
        if (dsdata->srcLen != sizeof(OOP_DEMANDS_T))
        {
            nRet = ERR_OPTION;
            break;
        }
        memcpy(&oopdemands, srcDataTmp, dsdata->srcLen);

        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 0x02;
        i = pOadInfo->nIndex - 1;
        dar = basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, &oopdemands.demand[i].nValue, sizeof(uint32));
        if ( dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oopdemands.demand[i].nValue(%d)\n", i, dar, oopdemands.demand[i].nValue);
            return DATA_TYPE_UNMATCHED;
        }
        
        dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dsdata->dstLen-offset, &offset, &oopdemands.demand[i].time, sizeof(oopdemands.demand[i].time));
        if( dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oopdemands.demand[i].time(%d)\n", i, dar, oopdemands.demand[i].time);
            return DATA_TYPE_UNMATCHED;
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

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class2)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class2(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    if (pOadInfo->nIndex == 0)
    {
        return data_to_buf_class2_all(pOadInfo, dsdata);
    }
    else
    {
        return data_to_buf_class2_one(pOadInfo, dsdata);
    }
}

#if DESC("class2����ӿں���",1)

/**
*********************************************************************
* @name��       class2_invoke_set
* @brief��      class2��SET����
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
OOP_DAR_E class2_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}

/**
*********************************************************************
* @name��       class2_invoke_get
* @brief��      class2��GET����
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
OOP_DAR_E class2_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_NORMAL;
	uint32 offset = 0;
	uint32 sourceoffset = 0;
	uint32 desoffset = 0;
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
    NormalOad.oad.value = (pOdaInfo->pTab->oad.value & 0xffffff00);

    ret = db_read_nomal(pApduInfo->hUdp, &NormalOad, BUF_LEN_MAX, srcData, &srcLen);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, NormalOad.logicId, NormalOad.infoNum, NormalOad.oad.value);
        
        //�������ĳ�����NULL
        return oop_null_array_res(pOdaInfo, pApduInfo, inData, inLen, outData, outDataMax);
    }
	
	dsdata.srcBuf= srcData;
	dsdata.srcLen= srcLen;
	dsdata.sOffset= &sourceoffset;
	dsdata.dstBuf= dstbuf;
	dsdata.dstLen= 1000;
	dsdata.dOffset= &desoffset;
	
    PRTL_FMT_LOGCC("class2_invoke_get len is %d\n", srcLen);
    PRTL_FMT_LOGCC("class2_invoke_get pOdaInfo->pTab->oad is %x\n", pOdaInfo->pTab->oad.value);
    PRTL_FMT_LOGCC("class2_invoke_get pOdaInfo->pTab->eData is %d\n", pOdaInfo->pTab->eData);

    ret = data_to_buf_class2(pOdaInfo, &dsdata);
    outLen=*dsdata.dOffset;
    if((ret == ERR_OK) && (outLen > 0))
    {
        memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
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
* @name��       class2_invoke_act
* @brief��      class2��ACTION����
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
OOP_DAR_E class2_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}


#endif
