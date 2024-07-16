/*
*********************************************************************
* @file    class1.c
* @brief�� class1����
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
// class1
//


/**
*********************************************************************
* @brief��     698��ʽ����ת���������ĵ�����(class1)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class1(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
	return DATA_SUCCESS;
}


/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class1)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1_energy(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_ENERGY_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //�ṹ����
    dstBuf[dOffset++] = DT_ARRAY;

    //�ṹ��Ա��
    dstBuf[dOffset++] = data.nNum;

    for (i = 0; i < data.nNum; i++)
    {
        dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
            return DATA_TYPE_UNMATCHED;
        }   
    }     

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class1)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1_energys(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_ENERGYS_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //�ṹ����
    dstBuf[dOffset++] = DT_ARRAY;

    //�ṹ��Ա��
    dstBuf[dOffset++] = data.nNum;

    for (i = 0; i < data.nNum; i++)
    {
        dar = basic_data_to_buf(E_DOUBLE_LONG, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(int32));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
            return DATA_TYPE_UNMATCHED;
        }   
    }     

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class1)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1_henergy(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_HENERGY_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //�ṹ����
    dstBuf[dOffset++] = DT_ARRAY;

    //�ṹ��Ա��
    dstBuf[dOffset++] = data.nNum;

    for (i = 0; i < data.nNum; i++)
    {
        dar = basic_data_to_buf(E_LONG64_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(uint64));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
            return DATA_TYPE_UNMATCHED;
        }   
    }     

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}


/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class1)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1_henergys(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_HENERGYS_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //�ṹ����
    dstBuf[dOffset++] = DT_ARRAY;

    //�ṹ��Ա��
    dstBuf[dOffset++] = data.nNum;

    for (i = 0; i < data.nNum; i++)
    {
        dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
            return DATA_TYPE_UNMATCHED;
        }   
    }     

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}


/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class1)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->eData)
    {
        case E_OOP_ENERGY_T:   //������(��������)
        {
        	ACUDP_FMT_TRACE("data_to_buf_class1() case E_OOP_ENERGY_T:   //������(��������) \n");
           	return data_to_buf_class1_energy(pOadInfo,  convert);
        }break;

        case E_OOP_ENERGYS_T:   //������(������)
        {
        	ACUDP_FMT_TRACE("data_to_buf_class1() case E_OOP_ENERGYS_T:   //������(������) \n");
           	return data_to_buf_class1_energys(pOadInfo,  convert);
        }break;

        case E_OOP_HENERGY_T:   //�߾��ȵ�����(��������)
        {
        	ACUDP_FMT_TRACE("data_to_buf_class1() case E_OOP_HENERGY_T:   //�߾��ȵ�����(��������) \n");
           	return data_to_buf_class1_henergy(pOadInfo,  convert);
        }break;

        case E_OOP_HENERGYS_T:   //�߾��ȵ�����(������)
        {
        	ACUDP_FMT_TRACE("data_to_buf_class1() case E_OOP_HENERGYS_T:   //�߾��ȵ�����(������) \n");
           	return data_to_buf_class1_henergys(pOadInfo,  convert);
        }break;

        //......
        
        default:
        {
        	ACUDP_FMT_TRACE("data_to_buf_class1() δʶ����������� pOadInfo->pTab->eData=%d \n", pOadInfo->pTab->eData);
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}


