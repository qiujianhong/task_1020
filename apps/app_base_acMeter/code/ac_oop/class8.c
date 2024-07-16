/*
*********************************************************************
* @file    class8.c
* @brief�� class8����
* @author��
* @date��   2019.12.06
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "commClass.h"
#include "oopservice.h"
#include "ac_sample.h"
#include "ac_main.h"

extern void sta_time_update_fset(BOOLEAN isUpdate);

//
// class8
//

/**
*********************************************************************
* @brief��      class8 ��������
* @param[in] �� ��
* @param[out]   pDesbuffer ԭʼ���ݽṹ��
*               nDesLen    ԭʼ���ݳ���
*               OAD        �������ݱ�ʶ
* @return��     >0            - �ɹ�
               <0            - ������
*********************************************************************
*/
int combine_sub_data_class8_buff(uint8 *pDesbuffer, uint16 *nDesLen, uint32 OAD)
{
    int index = 0;

    switch(OAD)
    {
        /*class 8*/
        case 0x40160201:               //��ǰ����ʱ�α� ��1��
        case 0x40160202:               //��ǰ����ʱ�α� ��2��
        case 0x40160203:               //��ǰ����ʱ�α� ��3��
        case 0x40160204:               //��ǰ����ʱ�α� ��4��
        case 0x40160205:               //��ǰ����ʱ�α� ��5��
        case 0x40160206:               //��ǰ����ʱ�α� ��6��
        case 0x40160207:               //��ǰ����ʱ�α� ��7��
        case 0x40160208:               //��ǰ����ʱ�α� ��8��
        {
            OOP_DAYPERIOD_T data = {0};
            data.nNum = 1;
            index = OAD - 0x40160201;

            if(0 == TermSharedData.g_nCurrDayPeriLabel)//��ǰ���ǵ�һ��
            {
                memcpy(&data.dPeriod[index], &TermSharedData.CurrDayPeriTableRD.dPeriod[index], sizeof(DAYPERIOD));
            }
            else
            {
                memcpy(&data.dPeriod[index], &TermSharedData.AddiDayPeriTableRD.dPeriod[index], sizeof(DAYPERIOD));
            }
            *nDesLen = sizeof(OOP_DAYPERIOD_T);
       		memcpy(pDesbuffer, &data, sizeof(OOP_DAYPERIOD_T));
        }
            break;

        default:
			return -1;
            break;
    }

    return 1;
}


/**
*********************************************************************
* @brief��     ��ȡϵͳʱ��
* @param[in]�� 
* @param[out]��DateTime_s��ʽʱ��
* @return��    null
*********************************************************************
*/
void systime_get(DateTimeHex_t *pDateTime)
{
    time_t linuxTime=time(NULL);

    DT_Time2DateTimeHex(linuxTime,pDateTime);
    //memrev((uint8*)pDateTime,7);
}

/**
*********************************************************************
* @brief��      ����ʱ��
* @param[in] �� ��
* @param[out]   pDesbuffer ԭʼ���ݽṹ��
*               nDesLen    ԭʼ���ݳ���
* @return��     >0            - �ɹ�
               <0            - ������
*********************************************************************
*/
int combine_time_normal_buff(uint8 *pDesbuffer, uint16 *nDesLen)
{
    OOP_DATETIME_S_T data = {0};
    DateTimeHex_t dataHex;
    memset(&dataHex,0,sizeof(DateTimeHex_t));
    systime_get((DateTimeHex_t*)&dataHex);
    memcpy(&data, &dataHex, sizeof(DateTimeHex_t));
    memrev((uint8*)&data, 2);      //OOP_DATETIME_S_T����ߵ�λ��ת
    ACUDP_FMT_LOGCC("combine_time_normal_buff : data.year = %d\n", data.year);
    ACUDP_FMT_LOGCC("combine_time_normal_buff : data.month = %d\n", data.month);
    ACUDP_FMT_LOGCC("combine_time_normal_buff : data.day = %d\n", data.day);
    ACUDP_FMT_LOGCC("combine_time_normal_buff : data.hour = %d\n", data.hour);
    ACUDP_FMT_LOGCC("combine_time_normal_buff : data.minute = %d\n", data.minute);
    ACUDP_FMT_LOGCC("combine_time_normal_buff : data.second = %d\n", data.second);

    *nDesLen = sizeof(DateTimeHex_t);
	memcpy(pDesbuffer, &data, sizeof(DateTimeHex_t));

    return 1;
}


/**
*********************************************************************
* @brief��      ��ȫģʽѡ��
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_time(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_DATETIME_S_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data)); 

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //ʱ��ת��
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),nDepth(%d)\n", i, dar, data);
        return DATA_TYPE_UNMATCHED;
    }        

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        //return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief��       ʱ��ʱ����
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_time_region(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_PERIOD_T data = {0};
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //�ṹ����
    dstBuf[dOffset++] = DT_STRUCTURE;

    //�ṹ��Ա��
    dstBuf[dOffset++] = 5;

    //��ʱ����
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.yPeriod, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),data.height(%d)\n", dar, data.yPeriod);
        return DATA_TYPE_UNMATCHED;
    }

    //��ʱ�α���
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriodMet, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),data.height(%d)\n", dar, data.dPeriodMet);
        return DATA_TYPE_UNMATCHED;
    }

    //��ʱ����
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),data.height(%d)\n", dar, data.dPeriodMet);
        return DATA_TYPE_UNMATCHED;
    }

    //������
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.rate, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),data.height(%d)\n", dar, data.rate);
        return DATA_TYPE_UNMATCHED;
    }

    //����������
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.pubHoliday, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),data.height(%d)\n", dar, data.rate);
        return DATA_TYPE_UNMATCHED;
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
* @brief��      ��ǰ��ʱ�α�
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_time_zone(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TIMEZONE_T data = {0};
    uint8 offlen = 0;      //������������
    uint8 lenBuf[3] = {0}; //������������
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    offlen = set_len_offset(data.nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //�ṹ����
        dstBuf[dOffset++] = DT_STRUCTURE;

        //�ṹ��Ա��
        dstBuf[dOffset++] = 3;

        //��
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.period[i].mon, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.period.mon(%d)\n", i, dar, data.period[i].mon);
            return DATA_TYPE_UNMATCHED;
        }

        //��
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.period[i].day, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.period.day(%d)\n", i, dar, data.period[i].day);
            return DATA_TYPE_UNMATCHED;
        }

        //��ʱ�α��
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.period[i].meterNum, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.period.meterNum(%d)\n", i, dar, data.period[i].meterNum);
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
* @brief��      ��ǰ����ʱ�α�
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_day_period(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_DAYPERIOD_T data = {0};
    uint8 offlen = 0;      //������������
    uint8 lenBuf[3] = {0}; //������������
    uint8 i = 0;
    uint8 j = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    offlen = set_len_offset(data.nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //��������
        dstBuf[dOffset++] = DT_ARRAY;

        //�����Ա����
        offlen = set_len_offset(data.dPeriod[i].nNum, lenBuf);
        memcpy(&dstBuf[dOffset], lenBuf, offlen);
        dOffset += offlen;

        for(j = 0; j < data.dPeriod[i].nNum; j++)
        {
            //�ṹ����
            dstBuf[dOffset++] = DT_STRUCTURE;

            //�ṹ��Ա��
            dstBuf[dOffset++] = 3;
            
            //ʱ
            dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod[i].period[j].hour, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.dPeriod[i].period[j].hour(%d)\n", i, dar, data.dPeriod[i].period[j].hour);
                return DATA_TYPE_UNMATCHED;
            }

            //��
            dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod[i].period[j].min, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.dPeriod[i].period[j].min(%d)\n", i, dar, data.dPeriod[i].period[j].min);
                return DATA_TYPE_UNMATCHED;
            }

            //���ʺ�
            dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod[i].period[j].rateNO, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d),data.dPeriod[i].period[j].rateNO(%d)\n", i, dar, data.dPeriod[i].period[j].rateNO);
                return DATA_TYPE_UNMATCHED;
            }
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
* @brief��      ��ʽ��ȫģʽ����
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_day_period_one(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_DAYPERIOD_T data = {0};
    uint8 offlen = 0;      //������������
    uint8 lenBuf[3] = {0}; //������������
    uint8 i = 0;
    uint8 j = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    if ((pOadInfo->nIndex > OOP_MAX_DAYPERIODS) || (pOadInfo->nIndex == 0))
    {
        return ERR_NORMAL;
    }

    i = pOadInfo->nIndex - 1;

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    offlen = set_len_offset(data.dPeriod[i].nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for(j = 0; j < data.dPeriod[i].nNum; j++)
    {
        //�ṹ����
        dstBuf[dOffset++] = DT_STRUCTURE;

        //�ṹ��Ա��
        dstBuf[dOffset++] = 3;
        
        //ʱ
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod[i].period[j].hour, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.dPeriod[i].period[j].hour(%d)\n", i, dar, data.dPeriod[i].period[j].hour);
            return DATA_TYPE_UNMATCHED;
        }

        //��
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod[i].period[j].min, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.dPeriod[i].period[j].min(%d)\n", i, dar, data.dPeriod[i].period[j].min);
            return DATA_TYPE_UNMATCHED;
        }

        //���ʺ�
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dPeriod[i].period[j].rateNO, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.dPeriod[i].period[j].rateNO(%d)\n", i, dar, data.dPeriod[i].period[j].rateNO);
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


/**
*********************************************************************
* @brief��      �����������
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_demand_period(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8  data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data)); 

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //�ʲ�����
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d)\n", 0, dar);
        return DATA_TYPE_UNMATCHED;
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
* @brief��      ����ʱ��
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8_slip_time(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8  data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data)); 

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //ʱ��
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. [%d] dar(%d)\n", 0, dar);
        return DATA_TYPE_UNMATCHED;
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
* @brief��     ���������ĵ�����ת698��ʽ����(class8)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class8(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x40000200:               //��ȡϵͳʱ��
        case 0x40080200:			   //������ʱ�����л�ʱ��
        case 0x40090200:			   //��������ʱ���л�ʱ��
        {
           return data_to_buf_class8_time(pOadInfo,  convert);
        }break;
        
        case 0x400C0200:			   //ʱ��ʱ����
        {
           return data_to_buf_class8_time_region(pOadInfo,  convert);
        }break;
        
        case 0x40140200:   //��ǰ��ʱ����
        case 0x40150200:   //������ʱ����
        {
           return data_to_buf_class8_time_zone(pOadInfo,  convert);
        }break;

        case 0x40160200:   //��ǰ����ʱ�α�
        case 0x40170200:   //��������ʱ�α�
        {
           return data_to_buf_class8_day_period(pOadInfo,  convert);
        }break;

        case 0x40160201:               //��ǰ����ʱ�α� ��1��
        case 0x40160202:               //��ǰ����ʱ�α� ��2��
        case 0x40160203:               //��ǰ����ʱ�α� ��3��
        case 0x40160204:               //��ǰ����ʱ�α� ��4��
        case 0x40160205:               //��ǰ����ʱ�α� ��5��
        case 0x40160206:               //��ǰ����ʱ�α� ��6��
        case 0x40160207:               //��ǰ����ʱ�α� ��7��
        case 0x40160208:               //��ǰ����ʱ�α� ��8��
        {
           return data_to_buf_class8_day_period_one(pOadInfo,  convert);
        }break;

        case 0x41000200:   //��������
        {
           return data_to_buf_class8_demand_period(pOadInfo,  convert);
        }break;

        case 0x41010200:   //����ʱ��
        {
           return data_to_buf_class8_slip_time(pOadInfo,  convert);
        }break;

        case 0x41120200:			   //�й���Ϸ�ʽ������
        case 0x41130200:			   //�޹���Ϸ�ʽ1������
        case 0x41140200:			   //�޹���Ϸ�ʽ2������
        {
           return basic_db_to_fmt698(pOadInfo,  convert);
        }break;

        //......
        
        default:
        {
            ACUDP_FMT_TRACE("data_to_buf not support. oad(0x%08x)\n", pOadInfo->pTab->oad.value);
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief��      ����ʱ��
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_time(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_DATETIME_S_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    
    //ʱ��
    dar = basic_buf_to_data(E_DATETIME_S, srcBuf, srcLen, &sOffset, &data, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief��      ʱ��ʱ����
* @param[in]�� ppOadInfo    - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf       - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��    =0           - �ɹ�
               !=0          - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_time_region(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_PERIOD_T data = {0};
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ṹ����
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ա��
    if(srcBuf[sOffset++] != 5)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //��ʱ����
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.yPeriod, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //��ʱ�α���
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.dPeriodMet, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //��ʱ����
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.dPeriod, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //������
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.rate, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //����������
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.pubHoliday, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief��     ��ǰ��ʱ����
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_time_zone(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TIMEZONE_T data = {0};
    uint8 offlen = 0;      //������������
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //��������
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�����Ա����
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //�ṹ����
        if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }
        
        //�ṹ��Ա��
        if(srcBuf[sOffset++] != 3)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }  
        
        //��
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period[i].mon, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //��
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period[i].day, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }     

        //��ʱ�α��
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period[i].meterNum, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }           
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief��     ��ǰ����ʱ�α�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_day_period_all(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_DAYPERIOD_T data = {0};
    uint8 offlen = 0;      //������������
    uint8 i = 0;
    uint8 j = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //��������
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�����Ա����
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {   
        //��������
        if(srcBuf[sOffset++] != DT_ARRAY)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //�����Ա����
        data.dPeriod[i].nNum = get_len_offset(&srcBuf[sOffset], &offlen);
        sOffset += offlen;

        for(j = 0; j < data.dPeriod[i].nNum; j++)
        {
            //�ṹ����
            if(srcBuf[sOffset++] != DT_STRUCTURE)
            {
                ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
                return DATA_TYPE_UNMATCHED;
            }

            //�ṹ��Ա��
            if(srcBuf[sOffset++] != 3)
            {
                ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
                return DATA_TYPE_UNMATCHED;
            }  

            //ʱ
            dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.dPeriod[i].period[j].hour, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            } 

            //��
            dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.dPeriod[i].period[j].min, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            } 

            //��
            dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.dPeriod[i].period[j].rateNO, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            } 
        }     
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief��     ��ǰ����ʱ�α�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_day_period_one(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    DAYPERIOD data = {0};
    uint8 offlen = 0;      //������������
    uint8 i = 0;
    uint8 j = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //��������
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�����Ա����
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    for(j = 0; j < data.nNum; j++)
    {
        //�ṹ����
        if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //�ṹ��Ա��
        if(srcBuf[sOffset++] != 3)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }  

        //ʱ
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period[j].hour, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //��
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period[j].min, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //��
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.period[j].rateNO, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 
    }     

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief��     ��ǰ����ʱ�α�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_day_period(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    if (pOadInfo->nIndex == 0)
    {
        return buf_to_data_class8_day_period_all(pOadInfo, convert);
    }
    else
    {
        return buf_to_data_class8_day_period_one(pOadInfo, convert);
    }
}

/**
*********************************************************************
* @brief��      �����������
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_demand_max(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //Уʱģʽ
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    ACUDP_FMT_TRACE("buf_to_data_class8_demand_max : data = 0x%x\n", data);
     
    return dar;
}

/**
*********************************************************************
* @brief��      ����ʱ��
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_slip_time(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    
    //Уʱģʽ
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    ACUDP_FMT_TRACE("buf_to_data_class8_demand_max : data = 0x%x\n", data);
     
    return dar;
}

/**
*********************************************************************
* @brief��      ������
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_feature(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    
    //Уʱģʽ
    dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_buf_to_data failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;

    ACUDP_FMT_TRACE("buf_to_data_class8_feature : data = 0x%x\n", data);
     
    return dar;
}


/**
*********************************************************************
* @brief��     ���������
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_combine_word(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_BITSTR8_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //������
    dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &data, sizeof(data));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }     

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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

/**
*********************************************************************
* @brief��     ���õ�ѹ�ϸ��ʲ���
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_volt_rate(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_VOLTPARAM_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ṹ����
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ա��
    if(srcBuf[sOffset++] != 4)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //��ѹ��������
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.uupVolt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //��ѹ��������
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.lldVolt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //��ѹ�ϸ�����
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.upVolt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }     

    //��ѹ�ϸ�����
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.ldVolt, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class8_set(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x40000200:   //����ʱ��
        case 0x40080200:   
        case 0x40090200:   
        case 0x400A0200:  
        {
            return buf_to_data_class8_time(pOadInfo,  convert);
        }break;

        case 0x400C0200:   //ʱ��ʱ����
        {
            return buf_to_data_class8_time_region(pOadInfo,  convert);
        }break;

        case 0x40140200:   //��ǰ��ʱ����
        case 0x40150200:   //������ʱ����
        {
            return buf_to_data_class8_time_zone(pOadInfo,  convert);
        }break;

        case 0x40160200:   //��ǰ����ʱ�α�
        case 0x40170200:   //��ǰ����ʱ�α�
        {
            return buf_to_data_class8_day_period(pOadInfo,  convert);
        }break;

        case 0x40300200:   //��ѹ�ϸ���
        {
            return buf_to_data_class8_volt_rate(pOadInfo,  convert);
        }break;

        case 0x41000200:   //��������
        {
            return buf_to_data_class8_demand_max(pOadInfo,  convert);
        }break;
        
        case 0x41010200:   //����ʱ��
        {
            return buf_to_data_class8_slip_time(pOadInfo,  convert);
        }break;
        
        case 0x41120200:   //���������
        case 0x41130200:   //���������
        case 0x41140200:   //���������
        {
            return buf_to_data_class8_combine_word(pOadInfo,  convert);
        }break;
        
        //......
        
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name��      class8_invoke_set_volt_rate
* @brief��     class8��SET����
* @param[in] ��pOdaInfo       OAD��Ϣ
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
OOP_DAR_E class8_invoke_set_volt_rate(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x40300200:
        {
            OOP_VOLTPARAM_T data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.logicId = 1;//��������
            normal.classtag = CLASS_DATA_INIT;
            ACUDP_FMT_TRACE("class8_invoke_set_volt_rate : data.uupVolt = %d\n", data.uupVolt);
            ACUDP_FMT_TRACE("class8_invoke_set_volt_rate : data.lldVolt = %d\n", data.lldVolt);
            ACUDP_FMT_TRACE("class8_invoke_set_volt_rate : data.upVolt = %d\n", data.upVolt);
            ACUDP_FMT_TRACE("class8_invoke_set_volt_rate : data.ldVolt = %d\n", data.ldVolt);

            ret = db_write_nomal(gDbUdpClient, &normal, (uint8*)&data, sizeof(data));
            if(ret != ERR_OK)
            {
                ACUDP_FMT_TRACE("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}


/**
*********************************************************************
* @name��      class8_invoke_set_security_flag
* @brief��     class8��GET����
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
OOP_DAR_E class8_invoke_set_time(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x40000200:
        {
            uint32 offset = 0;
            OOP_DATETIME_S_T data = {0};
            DateTimeHex_t dateHex ;
            memset(&dateHex,0,sizeof(DateTimeHex_t));
            
            time_t timep;
            time(&timep);
            OOP_DATETIME_S_T curData = {0};
            DT_Time2DateTimeShort(timep, &curData);
            
            memcpy(&data, convert.dstBuf, dOffset);
            ACUDP_FMT_TRACE("class8_invoke_set_time : curData.year = %hu\n", curData.year);
            ACUDP_FMT_TRACE("class8_invoke_set_time : curData.month = %d\n", curData.month);
            ACUDP_FMT_TRACE("class8_invoke_set_time : curData.day = %d\n", curData.day);
            ACUDP_FMT_TRACE("class8_invoke_set_time : curData.hour = %d\n", curData.hour);
            ACUDP_FMT_TRACE("class8_invoke_set_time : curData.minute = %d\n", curData.minute);
            ACUDP_FMT_TRACE("class8_invoke_set_time : curData.second = %d\n", curData.second);
            datetime_s_data_to_buf(data, &offset, (uint8 *)&dateHex);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.year = 0x%02x\n", dateHex.year_h);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.year = 0x%02x\n", dateHex.year_l);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.month = %d\n", dateHex.month);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.day = %d\n", dateHex.day);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.hour = %d\n", dateHex.hour);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.minute = %d\n", dateHex.min);
            ACUDP_FMT_TRACE("class8_invoke_set_time : dateHex.second = %d\n", dateHex.sec);

            if ((dateHex.month > 12) 
                ||(dateHex.day > 31)
                ||(dateHex.month == 2 && dateHex.day > 29)
                ||(dateHex.hour > 23)
                ||(dateHex.min > 59)
                ||(dateHex.sec > 59))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            sta_time_update_fset(TRUE);

            //��������
            memcpy(&TermSharedData.DateTimeSet, &data, sizeof(OOP_DATETIME_S_T));
            TermSharedData.DateSetFlag = TRUE;
            TermSharedData.TimeSetFlag = TRUE;
            g_nAcSetDateFlag = 1;
            g_nAcSetTimeFlag = 1;
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��      class8_invoke_set_security_flag
* @brief��     class8��GET����
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
OOP_DAR_E class8_invoke_set_change_time(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x40080200:			   //������ʱ�����л�ʱ��
    case 0x40090200:			   //��������ʱ���л�ʱ��
        {
            uint32 offset = 0;
            OOP_DATETIME_S_T data = {0};
            DateTimeHex_t dateHex ;
            memset(&dateHex,0,sizeof(DateTimeHex_t));
            
            time_t timep;
            time(&timep);
            OOP_DATETIME_S_T curData = {0};
            DT_Time2DateTimeShort(timep, &curData);
            
            memcpy(&data, convert.dstBuf, dOffset);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : curData.year = %hu\n", curData.year);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : curData.month = %d\n", curData.month);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : curData.day = %d\n", curData.day);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : curData.hour = %d\n", curData.hour);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : curData.minute = %d\n", curData.minute);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : curData.second = %d\n", curData.second);
            datetime_s_data_to_buf(data, &offset, (uint8 *)&dateHex);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.year = 0x%02x\n", dateHex.year_h);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.year = 0x%02x\n", dateHex.year_l);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.month = %d\n", dateHex.month);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.day = %d\n", dateHex.day);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.hour = %d\n", dateHex.hour);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.minute = %d\n", dateHex.min);
            ACUDP_FMT_TRACE("class8_invoke_set_change_time : dateHex.second = %d\n", dateHex.sec);

            if(0xff == dateHex.sec)
            {
                dateHex.sec = 0;
            }
            
            if ((dateHex.month > 12) 
                ||(dateHex.day > 31)
                ||(dateHex.month == 2 && dateHex.day > 29)
                ||(dateHex.hour > 23)
                ||(dateHex.min > 59)
                ||(dateHex.sec > 59))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            //��������
            if(0x40080200 == pOadInfo->pTab->oad.value)
            {
                memcpy(&TermSharedData.AddiZoneChangeTimeSet, &data, sizeof(OOP_DATETIME_S_T));
                TermSharedData.AddiZoneChangeTimeSetFlag = TRUE;
                ACUDP_FMT_TRACE("case 0x40080200://������ʱ�����л�ʱ�� AddiZoneChangeTimeSetFlag=%d\n", 
                    TermSharedData.AddiZoneChangeTimeSetFlag);
            }
            else if(0x40090200 == pOadInfo->pTab->oad.value)
            {
                memcpy(&TermSharedData.AddiPeriChangeTimeSet, &data, sizeof(OOP_DATETIME_S_T));
                TermSharedData.AddiPeriChangeTimeSetFlag = TRUE;
                ACUDP_FMT_TRACE("case 0x40090200://��������ʱ�α��л�ʱ�� AddiPeriChangeTimeSetFlag=%d\n", 
                    TermSharedData.AddiPeriChangeTimeSetFlag);
            }
            
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}


/**
*********************************************************************
* @name��      class8_invoke_set_group_addr
* @brief��     class8��SET����
* @param[in] ��pOdaInfo       OAD��Ϣ
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
OOP_DAR_E class8_invoke_set_time_zone(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x40140200:
    case 0x40150200:
        {
            uint32 i = 0;
            OOP_TIMEZONE_T data;
            memcpy(&data, convert.dstBuf, dOffset);

            ACUDP_FMT_TRACE("class8_invoke_set_time_zone : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                    ACUDP_FMT_TRACE("class8_invoke_set_time_zone : data.period[%d].mon:%d\n", 
                        i,  data.period[i].mon);
                    ACUDP_FMT_TRACE("class8_invoke_set_time_zone : data.period[%d].day:%d\n", 
                        i, data.period[i].day);
                    ACUDP_FMT_TRACE("class8_invoke_set_time_zone : data.period[%d].meterNum:%d\n", 
                        i, data.period[i].meterNum);
            }

            //��������
            if(0x40140200 == pOadInfo->pTab->oad.value)
            {
                memcpy(&TermSharedData.CurrZoneTableSet, &data, sizeof(OOP_TIMEZONE_T));
                TermSharedData.CurrZoneNumSetFlag = TRUE;
                TermSharedData.CurrZoneTableSetFlag = TRUE;
            }
            else if(0x40150200 == pOadInfo->pTab->oad.value)
            {
                memcpy(&TermSharedData.AddiZoneTableSet, &data, sizeof(OOP_TIMEZONE_T));
                TermSharedData.AddiZoneNumSetFlag = TRUE;
                TermSharedData.AddiZoneTableSetFlag = TRUE;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��      class8_invoke_set_day_period_all
* @brief��     class8��SET����
* @param[in] ��pOdaInfo       OAD��Ϣ
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
OOP_DAR_E class8_invoke_set_day_period_all(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x40160200:
    case 0x40170200:
        {
            uint32 i = 0;
            uint32 j = 0;
            OOP_DAYPERIOD_T data;
            memcpy(&data, convert.dstBuf, dOffset);

            ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                for (j = 0; j < data.dPeriod[i].nNum; j++)
                {
                    ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.dPeriod[%d].period[%d].hour:%d\n", 
                        i, j, data.dPeriod[i].period[j].hour);
                    ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.dPeriod[%d].period[%d].min:%d\n", 
                        i, j, data.dPeriod[i].period[j].min);
                    ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.dPeriod[%d].period[%d].rateNO:%d\n", 
                        i, j, data.dPeriod[i].period[j].rateNO);
                }
            }

            //���Ȳ�ѯ��ǰ���ǵڼ���
            time_t nStartTime = time(NULL);
            g_nReadCurrDayPeriLabelFlag = 1;
            g_nRefreshCurrDayPeriLabelFlag = 0;
            while(abs(time(NULL) - nStartTime) < 3)//��ʱʱ��
            {
                if( (0 == g_nReadCurrDayPeriLabelFlag)
                    && (1 == g_nRefreshCurrDayPeriLabelFlag) )
                {
                    ACUDP_FMT_TRACE("class8_invoke_set_day_period() g_nCurrDayPeriLabel=%d \n", TermSharedData.g_nCurrDayPeriLabel);
                    break;
                }
                usleep(50000);
            }

            //��������(����Ӧ����: ������ʱ�α�ʱ, �Զ�������ʱ����)
            if(data.nNum > 0)
            {
                TermSharedData.ZonePeriNumSet.dPeriodMet = data.nNum;
                TermSharedData.DayPeriTableNumSetFlag = TRUE;
                TermSharedData.ZonePeriNumSet.dPeriod = data.dPeriod[0].nNum;
                TermSharedData.DayPeriNumSetFlag = TRUE;

                ACUDP_FMT_TRACE("class8_invoke_set_day_period() ZoneTableNum=%d, DayPeriTableNum=%d, DayPeriNum=%d, CostRateNum=%d, HolidayNum=%d...\n", 
                    TermSharedData.ZonePeriNumSet.yPeriod, TermSharedData.ZonePeriNumSet.dPeriodMet, TermSharedData.ZonePeriNumSet.dPeriod, 
                    TermSharedData.ZonePeriNumSet.rate, TermSharedData.ZonePeriNumSet.pubHoliday);
                
                ACUDP_FMT_TRACE("class8_invoke_set_day_period() ZoneTableNumSetFlag=%d, DayPeriTableNumSetFlag=%d, DayPeriNumSetFlag=%d, CostRateNumSetFlag=%d, HolidayNumSetFlag=%d...\n", 
                    TermSharedData.ZoneTableNumSetFlag, TermSharedData.DayPeriTableNumSetFlag, TermSharedData.DayPeriNumSetFlag, 
                    TermSharedData.CostRateNumSetFlag, TermSharedData.HolidayNumSetFlag);
            }

            //��������
            if(0 == TermSharedData.g_nCurrDayPeriLabel)//��ǰ���ǵ�һ��
            {
                if(0x40160200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.CurrDayPeriTableSet, &data, sizeof(OOP_DAYPERIOD_T));
                    
                    TermSharedData.CurrDayTableNumSetFlag = TRUE;
                    TermSharedData.CurrDayPeriNumSetFlag = TRUE;
                    memset(TermSharedData.CurrDayPeriNumSetFlagArr, 0x01, TermSharedData.CurrDayPeriTableSet.nNum);
                    
                    TermSharedData.CurrDayPeriTableSetFlag = TRUE;
                    memset(TermSharedData.CurrDayPeriSetFlagArr, 0x01, TermSharedData.CurrDayPeriTableSet.nNum);

                    ACUDP_FMT_TRACE("���ɵ�ǰ���е�1��, ���õ�ǰ�׼����õ�1�� \n");
                }
                else if(0x40170200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.AddiDayPeriTableSet, &data, sizeof(OOP_DAYPERIOD_T));
                    
                    TermSharedData.AddiDayTalbeNumSetFlag = TRUE;
                    TermSharedData.AddiDayPeriNumSetFlag = TRUE;
                    memset(TermSharedData.AddiDayPeriNumSetFlagArr, 0x01, TermSharedData.AddiDayPeriTableSet.nNum);
                    
                    TermSharedData.AddiDayPeriTableSetFlag = TRUE;
                    memset(TermSharedData.AddiDayPeriSetFlagArr, 0x01, TermSharedData.AddiDayPeriTableSet.nNum);

                    ACUDP_FMT_TRACE("���ɵ�ǰ���е�1��, ���ñ����׼����õ�2�� \n");
                }
            }
            else if(1 == TermSharedData.g_nCurrDayPeriLabel)//��ǰ���ǵڶ���
            {
                if(0x40170200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.CurrDayPeriTableSet, &data, sizeof(OOP_DAYPERIOD_T));
                    
                    TermSharedData.CurrDayTableNumSetFlag = TRUE;
                    TermSharedData.CurrDayPeriNumSetFlag = TRUE;
                    memset(TermSharedData.CurrDayPeriNumSetFlagArr, 0x01, TermSharedData.CurrDayPeriTableSet.nNum);
                    
                    TermSharedData.CurrDayPeriTableSetFlag = TRUE;
                    memset(TermSharedData.CurrDayPeriSetFlagArr, 0x01, TermSharedData.CurrDayPeriTableSet.nNum);

                    ACUDP_FMT_TRACE("���ɵ�ǰ���е�2��, ���ñ����׼����õ�1�� \n");
                }
                else if(0x40160200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.AddiDayPeriTableSet, &data, sizeof(OOP_DAYPERIOD_T));
                    
                    TermSharedData.AddiDayTalbeNumSetFlag = TRUE;
                    TermSharedData.AddiDayPeriNumSetFlag = TRUE;
                    memset(TermSharedData.AddiDayPeriNumSetFlagArr, 0x01, TermSharedData.AddiDayPeriTableSet.nNum);
                    
                    TermSharedData.AddiDayPeriTableSetFlag = TRUE;
                    memset(TermSharedData.AddiDayPeriSetFlagArr, 0x01, TermSharedData.AddiDayPeriTableSet.nNum);

                    ACUDP_FMT_TRACE("���ɵ�ǰ���е�2��, ���õ�ǰ�׼����õ�2�� \n");
                }
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��      wait_day_period_update
* @brief��     �ȴ����ݸ������
* @param[in] ��pOdaInfo        OAD��Ϣ
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E wait_day_period_update(uint8 isCur)
{
    int32 i = 0;

    if (isCur == 0)
    {
        while (FALSE != TermSharedData.CurrDayPeriTableSetFlag || 0 != g_nAcReadCurrDayPeriFlag)
        {
            if(i++ > 500)
            {
                PRTL_FMT_DEBUG("wait_day_period_update err\n");
                return DATA_READ_WRITE_DENIED;
            }
            usleep(10000);
        }
    }
    else if (isCur == 1)
    {
        while (FALSE != TermSharedData.AddiDayPeriTableSetFlag || 0 != g_nAcReadAddiDayPeriFlag)
        {
            if(i++ > 500)
            {
                PRTL_FMT_DEBUG("wait_day_period_update err\n");
                return DATA_READ_WRITE_DENIED;
            }
            usleep(10000);
        }
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name��      class8_invoke_set_day_period_one
* @brief��     class8��SET����
* @param[in] ��pOdaInfo       OAD��Ϣ
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
OOP_DAR_E class8_invoke_set_day_period_one(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x40160200:
    case 0x40170200:
        {
            uint32 j = 0;
            uint32 index = 0;
            DAYPERIOD data = {0};

            index = pOadInfo->nIndex - 1;
            memcpy(&data, convert.dstBuf, dOffset);

            ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.nNum = %d\n", data.nNum);

            for (j = 0; j < data.nNum; j++)
            {
                ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.period[%d].hour:%d\n", 
                    j, data.period[j].hour);
                ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.period[%d].min:%d\n", 
                    j, data.period[j].min);
                ACUDP_FMT_TRACE("class8_invoke_set_day_period : data.period[%d].rateNO:%d\n", 
                    j, data.period[j].rateNO);
            }

            //���Ȳ�ѯ��ǰ���ǵڼ���
            time_t nStartTime = time(NULL);
            g_nReadCurrDayPeriLabelFlag = 1;
            g_nRefreshCurrDayPeriLabelFlag = 0;
            while(abs(time(NULL) - nStartTime) < 3)//��ʱʱ��
            {
                if( (0 == g_nReadCurrDayPeriLabelFlag)
                    && (1 == g_nRefreshCurrDayPeriLabelFlag) )
                {
                    ACUDP_FMT_TRACE("class8_invoke_set_day_period() g_nCurrDayPeriLabel=%d \n", TermSharedData.g_nCurrDayPeriLabel);
                    break;
                }
                usleep(50000);
            }

            //��������(����Ӧ����: ������ʱ�α�ʱ, �Զ�������ʱ����)
            if (data.nNum > 0)
            {
                TermSharedData.ZonePeriNumSet.dPeriodMet = OOP_MAX_DAYPERIODS;
                TermSharedData.DayPeriTableNumSetFlag = TRUE;

                TermSharedData.ZonePeriNumSet.dPeriod = data.nNum;
                TermSharedData.DayPeriNumSetFlag = TRUE;

                ACUDP_FMT_TRACE("class8_invoke_set_day_period() ZoneTableNum=%d, DayPeriTableNum=%d, DayPeriNum=%d, CostRateNum=%d, HolidayNum=%d...\n", 
                    TermSharedData.ZonePeriNumSet.yPeriod, TermSharedData.ZonePeriNumSet.dPeriodMet, TermSharedData.ZonePeriNumSet.dPeriod, 
                    TermSharedData.ZonePeriNumSet.rate, TermSharedData.ZonePeriNumSet.pubHoliday);
                
                ACUDP_FMT_TRACE("class8_invoke_set_day_period() ZoneTableNumSetFlag=%d, DayPeriTableNumSetFlag=%d, DayPeriNumSetFlag=%d, CostRateNumSetFlag=%d, HolidayNumSetFlag=%d...\n", 
                    TermSharedData.ZoneTableNumSetFlag, TermSharedData.DayPeriTableNumSetFlag, TermSharedData.DayPeriNumSetFlag, 
                    TermSharedData.CostRateNumSetFlag, TermSharedData.HolidayNumSetFlag);
            }

            //��������
            if(0 == TermSharedData.g_nCurrDayPeriLabel)//��ǰ���ǵ�һ��
            {
                if(0x40160200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.CurrDayPeriTableSet, &TermSharedData.CurrDayPeriTableRD, sizeof(OOP_DAYPERIOD_T));
                    memcpy(&TermSharedData.CurrDayPeriTableSet.dPeriod[index], &data, sizeof(DAYPERIOD));
                    
                    TermSharedData.CurrDayPeriTableSetFlag = TRUE;
                    TermSharedData.CurrDayPeriSetFlagArr[index] = 0x01;

                    ACUDP_FMT_TRACE("���ɵ�ǰ���е�1��, ���õ�ǰ�׼����õ�1�� \n");

                    //�ȴ����ݸ������
                    if (wait_day_period_update(0) != DATA_SUCCESS)
                    {
                        ACUDP_FMT_DEBUG("���ɵ�ǰ���е�1��, ���õ�ǰ�׼����õ�1�� ʧ��\n");
                        return DATA_READ_WRITE_DENIED;
                    }
                }
                else if(0x40170200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.AddiDayPeriTableSet, &TermSharedData.AddiDayPeriTableRD, sizeof(OOP_DAYPERIOD_T));
                    memcpy(&TermSharedData.AddiDayPeriTableSet.dPeriod[index], &data, sizeof(DAYPERIOD));
                    
                    TermSharedData.AddiDayPeriTableSetFlag = TRUE;
                    TermSharedData.AddiDayPeriSetFlagArr[index] = 0x01;

                    ACUDP_FMT_TRACE("���ɵ�ǰ���е�1��, ���ñ����׼����õ�2�� \n");

                    //�ȴ����ݸ������
                    if (wait_day_period_update(1) != DATA_SUCCESS)
                    {
                        ACUDP_FMT_DEBUG("���ɵ�ǰ���е�1��, ���ñ����׼����õ�2�� ʧ��\n");
                        return DATA_READ_WRITE_DENIED;
                    }
                }
            }
            else if(1 == TermSharedData.g_nCurrDayPeriLabel)//��ǰ���ǵڶ���
            {
                if(0x40170200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.CurrDayPeriTableSet, &TermSharedData.CurrDayPeriTableRD, sizeof(OOP_DAYPERIOD_T));
                    memcpy(&TermSharedData.CurrDayPeriTableSet.dPeriod[index], &data, sizeof(DAYPERIOD));
                    
                    TermSharedData.CurrDayPeriTableSetFlag = TRUE;
                    TermSharedData.CurrDayPeriSetFlagArr[index] = 0x01;

                    ACUDP_FMT_TRACE("���ɵ�ǰ���е�2��, ���ñ����׼����õ�1�� \n");

                    //�ȴ����ݸ������
                    if (wait_day_period_update(0) != DATA_SUCCESS)
                    {
                        ACUDP_FMT_DEBUG("���ɵ�ǰ���е�1��, ���ñ����׼����õ�2�� ʧ��\n");
                        return DATA_READ_WRITE_DENIED;
                    }
                }
                else if(0x40160200 == pOadInfo->pTab->oad.value)
                {
                    memcpy(&TermSharedData.AddiDayPeriTableSet, &TermSharedData.AddiDayPeriTableRD, sizeof(OOP_DAYPERIOD_T));
                    memcpy(&TermSharedData.AddiDayPeriTableSet.dPeriod[index], &data, sizeof(DAYPERIOD));
                    
                    TermSharedData.AddiDayPeriTableSetFlag = TRUE;
                    TermSharedData.AddiDayPeriSetFlagArr[index] = 0x01;

                    ACUDP_FMT_TRACE("���ɵ�ǰ���е�2��, ���õ�ǰ�׼����õ�2�� \n");

                    //�ȴ����ݸ������
                    if (wait_day_period_update(1) != DATA_SUCCESS)
                    {
                        ACUDP_FMT_DEBUG("���ɵ�ǰ���е�2��, ���õ�ǰ�׼����õ�2�� ʧ��\n");
                        return DATA_READ_WRITE_DENIED;
                    }
                }
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��      class8_invoke_set_group_addr
* @brief��     class8��SET����
* @param[in] ��pOdaInfo       OAD��Ϣ
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
OOP_DAR_E class8_invoke_set_day_period(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    if (pOadInfo->nIndex == 0)
    {
        return class8_invoke_set_day_period_all(pOadInfo, inData, InDataLen, outData, outDataMax);
    }
    else
    {
        return class8_invoke_set_day_period_one(pOadInfo, inData, InDataLen, outData, outDataMax);
    }
}

/**
*********************************************************************
* @name��      class8_invoke_set_demand_period
* @brief��     class8��SET����
* @param[in] ��pOdaInfo       OAD��Ϣ
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
OOP_DAR_E class8_invoke_set_demand_period(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x41000200:
        {
            uint8 data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            
            //��������
            memcpy(&TermSharedData.MaxDemandPeriodSet, &data, sizeof(uint8));
            TermSharedData.MaxDemandPeriodSetFlag = TRUE;
            ACUDP_FMT_TRACE("class8_invoke_set_demand_period() MaxDemandPeriodSet=%d, MaxDemandPeriodSetFlag=%d...\n", 
                TermSharedData.MaxDemandPeriodSet, TermSharedData.MaxDemandPeriodSetFlag);
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��      class8_invoke_set_demand_period
* @brief��     class8��SET����
* @param[in] ��pOdaInfo       OAD��Ϣ
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
OOP_DAR_E class8_invoke_set_slip_time(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x41010200:
        {
            uint8 data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            
            //��������
            memcpy(&TermSharedData.SlipDiffTimeSet, &data, sizeof(uint8));
            TermSharedData.SlipDiffTimeSetFlag = TRUE;
            ACUDP_FMT_TRACE("class8_invoke_set_slip_time() SlipDiffTimeSet=%d, SlipDiffTimeSetFlag=%d...\n", 
                TermSharedData.SlipDiffTimeSet, TermSharedData.SlipDiffTimeSetFlag);
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��      class8_invoke_set_time_region
* @brief��     class8��SET����
* @param[in] ��pOdaInfo       OAD��Ϣ
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
OOP_DAR_E class8_invoke_set_time_region(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x400C0200:
        {
            OOP_PERIOD_T data;
            memcpy(&data, convert.dstBuf, dOffset);

            //��������
            memcpy(&TermSharedData.ZonePeriNumSet, &data, sizeof(OOP_PERIOD_T));
            TermSharedData.ZoneTableNumSetFlag = TRUE;
            TermSharedData.DayPeriTableNumSetFlag = TRUE;
            TermSharedData.DayPeriNumSetFlag = TRUE;
            TermSharedData.CostRateNumSetFlag = TRUE;
            TermSharedData.HolidayNumSetFlag = TRUE;

            ACUDP_FMT_TRACE("class8_invoke_set_time_region() ZoneTableNum=%d, DayPeriTableNum=%d, DayPeriNum=%d, CostRateNum=%d, HolidayNum=%d...\n", 
                TermSharedData.ZonePeriNumSet.yPeriod, TermSharedData.ZonePeriNumSet.dPeriodMet, TermSharedData.ZonePeriNumSet.dPeriod, 
                TermSharedData.ZonePeriNumSet.rate, TermSharedData.ZonePeriNumSet.pubHoliday);
            
            ACUDP_FMT_TRACE("class8_invoke_set_time_region() ZoneTableNumSetFlag=%d, DayPeriTableNumSetFlag=%d, DayPeriNumSetFlag=%d, CostRateNumSetFlag=%d, HolidayNumSetFlag=%d...\n", 
                TermSharedData.ZoneTableNumSetFlag, TermSharedData.DayPeriTableNumSetFlag, TermSharedData.DayPeriNumSetFlag, 
                TermSharedData.CostRateNumSetFlag, TermSharedData.HolidayNumSetFlag);
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��      class8_invoke_set_feature
* @brief��     class8��SET����
* @param[in] ��pOdaInfo       OAD��Ϣ
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
OOP_DAR_E class8_invoke_set_feature(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8*)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class8_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class8_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x41120200:			   //�й���Ϸ�ʽ������
        {
            uint8 data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            
            //��������
            memcpy(&TermSharedData.ActPowCombWordSet, &data, sizeof(uint8));
            TermSharedData.ActPowCombWordSetFlag = TRUE;
            ACUDP_FMT_TRACE("class8_invoke_set_feature() ActPowCombWordSet=%d, ActPowCombWordSetFlag=%d...\n", 
                TermSharedData.ActPowCombWordSet, TermSharedData.ActPowCombWordSetFlag);
        }
        break;
    case 0x41130200:			   //�޹���Ϸ�ʽ1������
        {
            uint8 data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            
            //��������
            memcpy(&TermSharedData.ReaPowComb1WordSet, &data, sizeof(uint8));
            TermSharedData.ReaPowComb1WordSetFlag = TRUE;
            ACUDP_FMT_TRACE("class8_invoke_set_feature() ReaPowComb1WordSet=%d, ReaPowComb1WordSetFlag=%d...\n", 
                TermSharedData.ReaPowComb1WordSet, TermSharedData.ReaPowComb1WordSetFlag);
        }
        break;
    case 0x41140200:			   //�޹���Ϸ�ʽ2������
        {
            uint8 data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            
            //��������
            memcpy(&TermSharedData.ReaPowComb2WordSet, &data, sizeof(uint8));
            TermSharedData.ReaPowComb2WordSetFlag = TRUE;
            ACUDP_FMT_TRACE("class8_invoke_set_feature() ReaPowComb2WordSet=%d, ReaPowComb2WordSetFlag=%d...\n", 
                TermSharedData.ReaPowComb2WordSet, TermSharedData.ReaPowComb2WordSetFlag);
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}



