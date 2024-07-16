/*
*********************************************************************
* @file    class3.c
* @brief�� class3����
* @author��
* @date��   2019.12.06
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
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
* @brief��     ���������ĵ�����ת698��ʽ����(class3)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3_oop_word3v(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    ARRAY_INFO_T arrayInfo;

    MEMZERO(&arrayInfo, sizeof(arrayInfo));

    /*��μ��*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class3_oop_word3v error, ERR_PTR\n");
        return ERR_PTR;
    }
    OOP_WORD3V_T WORD3V_T;

    //ע�⣺��������srcLen����ֻ�������Ա�ĳ���
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
    
    pOadInfoTmp->subeData = E_LONG_UNSIGNED; //��Ҫ����������һ��Ҫ���ӽṹ����������
    
    nRet = class_array_tofmt698(pOadInfoTmp, &arrayInfo, dsdata);

    return nRet;
}

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class3)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3_oop_int3v(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    ARRAY_INFO_T arrayInfo;

    MEMZERO(&arrayInfo, sizeof(arrayInfo));

    /*��μ��*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class3_oop_int3v error, ERR_PTR\n");
        return ERR_PTR;
    }
    OOP_INT3V_T INT3V_T;

    //ע�⣺��������srcLen����ֻ�������Ա�ĳ���
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
    
    pOadInfoTmp->subeData = E_DOUBLE_LONG; //��Ҫ����������һ��Ҫ���ӽṹ����������
    
    nRet = class_array_tofmt698(pOadInfoTmp, &arrayInfo, dsdata);

    return nRet;
}


/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class3)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3_oop_long3v(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    ARRAY_INFO_T arrayInfo;
    
    MEMZERO(&arrayInfo, sizeof(arrayInfo));

    /*��μ��*/
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

    pOadInfoTmp->subeData = E_LONG; //��Ҫ����������һ��Ҫ���ӽṹ����������
    
    nRet = class_array_tofmt698(pOadInfoTmp, &arrayInfo, dsdata);    

    return nRet;
}

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class3)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3_oop_dword3v(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    ARRAY_INFO_T arrayInfo;

    MEMZERO(&arrayInfo, sizeof(arrayInfo));

    /*��μ��*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class3_oop_dword3v error, ERR_PTR\n");
        return ERR_PTR;
    }
    OOP_DWORD3V_T DWORD3V_T;

    //ע�⣺��������srcLen����ֻ�������Ա�ĳ���
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
    
    pOadInfoTmp->subeData = E_DOUBLE_LONG_UNSIGNED; //��Ҫ����������һ��Ҫ���ӽṹ����������
    
    nRet = class_array_tofmt698(pOadInfoTmp, &arrayInfo, dsdata);

    return nRet;
}

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class3)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class3(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    pOadInfoTmp = pOadInfo;


    /*��μ��*/
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

#if DESC("class3����ӿں���",1)

/**
*********************************************************************
* @name��       class3_invoke_set
* @brief��      class3��SET����
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
OOP_DAR_E class3_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}

/**
*********************************************************************
* @name��       class3_invoke_get
* @brief��      class3��GET����
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
        
        //�������ĳ�����NULL
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
            memcpy(&outData->pResultData[offset], inData, 4); //����ӱ����п�����oad��Ϣ�п�����ģ�������
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
* @name��       class3_invoke_act
* @brief��      class3��ACTION����
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
OOP_DAR_E class3_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}


#endif
