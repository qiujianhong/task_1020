/*
*********************************************************************
* @file    class17.c
* @brief�� class17����
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
// class17
//

OOP_DAR_E data_to_buf_class17_f3010200(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_OBJ_SHOW_T data = {0};
    //uint8 offlen = 0;      //������������
    //uint8 lenBuf[3] = {0}; //������������
    //uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    dstBuf[dOffset++] = data.nNum;

    for(uint32 i =0; i < data.nNum; i++)
    {
        dstBuf[dOffset++] = DT_STRUCTURE;
        dstBuf[dOffset++] = 0x02;

        dar = basic_data_to_buf(E_CSD, dstBuf, dstLen, &dOffset, &data.objs[i].cols, sizeof(data.objs[i].cols));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.objs[i].cols);
            return DATA_TYPE_UNMATCHED;
        }

        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.objs[i].nNum, sizeof(data.objs[i].nNum));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.objs[i].nNum);
            return DATA_TYPE_UNMATCHED;
        }
    }
    

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

OOP_DAR_E data_to_buf_class17_f3010400(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_OBJ_SHOW_PARAM_T data = {0};
    //uint8 offlen = 0;      //������������
    //uint8 lenBuf[3] = {0}; //������������
    //uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_STRUCTURE;

    //�����Ա����
    dstBuf[dOffset++] = 0x02;

    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.objNumAll, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        //PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.objNumAll);
        return DATA_TYPE_UNMATCHED;
    }

    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.objNumMax, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        //PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.objNumMax);
        return DATA_TYPE_UNMATCHED;
    }

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        //PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class17)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class17(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0xF3010400:
        case 0xf3000400:
        {
           return data_to_buf_class17_f3010400(pOadInfo,  convert);
        }break;

        case 0xf3010200:
        {
           return data_to_buf_class17_f3010200(pOadInfo,  convert);
        }break;
        //......
        
        default:
        {
            return basic_db_to_fmt698(pOadInfo,  convert);
        }break;
    }
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief��      ���������ĵ�����ת698��ʽ����(class24)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class17(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}


#if DESC("class17����ӿں���",1)

/**
*********************************************************************
* @name��       class17_invoke_set
* @brief��      class17��SET����
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
OOP_DAR_E class17_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}

/**
*********************************************************************
* @name��       class17_invoke_get
* @brief��      class17��GET����
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
OOP_DAR_E class17_invoke_get(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}

/**
*********************************************************************
* @name��       class17_invoke_act
* @brief��      class17��ACTION����
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
OOP_DAR_E class17_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}


#endif
