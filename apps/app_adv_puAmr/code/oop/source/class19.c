/*
*********************************************************************
* @file    class19.c
* @brief�� class19����
* @author��
* @date��   2019.12.06
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "storage.h"
#include "area.h"
#include "msg_id.h"
#include "appoutmsg.h"
    
//
// class19
//

extern int statis_clear_data(DB_CLIENT fd, CLEAR_DATA_T *ptData);
extern int statis_recover_default(DB_CLIENT fd, uint8 logicId);
extern void sta_data_init(uint8 type, DB_CLIENT fd);
extern void rpt_data_init(uint8 type, DB_CLIENT fd);

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(֤��汾)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class19_term_var(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    OOP_VERSION_T data;
    MEMZERO(&data, sizeof(data));

    /* ��μ�� */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if(convert->srcLen != sizeof(OOP_VERSION_T))
    {
        dar = DATA_OTHER_REASON;
        return dar;
    }
    memcpy((uint8*)&data, &srcBuf[sOffset], convert->srcLen);
    sOffset += sizeof(OOP_VERSION_T);

    /* �ṹ���� */
    dstBuf[dOffset++] = DT_STRUCTURE;

    /* �ṹ��Ա�� */
    dstBuf[dOffset++] = 6;

    /* ���̴��� */
    dar = basic_data_to_buf(E_OOP_VISIBLESTR4_T, dstBuf, convert->dstLen, &dOffset, &data.szFactory, sizeof(OOP_VISIBLESTR4_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("��ȡ���̴���data-->buf err\n");
        return dar;
    }

    /* ����汾�� */
    dar = basic_data_to_buf(E_OOP_VISIBLESTR4_T, dstBuf, convert->dstLen, &dOffset, &data.szSoftVer, sizeof(OOP_VISIBLESTR4_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("��ȡ����汾��data-->buf err\n");
        return dar;
    }

    /* ����汾���� */
    dar = basic_data_to_buf(E_OOP_VISIBLESTR6_T, dstBuf, convert->dstLen, &dOffset, &data.szSoftDate, sizeof(OOP_VISIBLESTR6_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("��ȡ����汾����data-->buf err\n");
        return dar;
    }

    /* Ӳ���汾�� */
    dar = basic_data_to_buf(E_OOP_VISIBLESTR4_T, dstBuf, convert->dstLen, &dOffset, &data.szHardVer, sizeof(OOP_VISIBLESTR4_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("��ȡӲ���汾��data-->buf err\n");
        return dar;
    }


    /* Ӳ���汾���� */
    dar = basic_data_to_buf(E_OOP_VISIBLESTR6_T, dstBuf, convert->dstLen, &dOffset, &data.szHardDate, sizeof(OOP_VISIBLESTR6_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("��ȡӲ���汾����data-->buf err\n");
        return dar;
    }

    /* ������չ��Ϣ */
    dar = basic_data_to_buf(E_OOP_VISIBLESTR8_T, dstBuf, convert->dstLen, &dOffset, &data.szExtend, sizeof(OOP_VISIBLESTR8_T));
    if (dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("��ȡ������չ��Ϣ��Ϣdata-->buf err\n");
        return dar;
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
* @brief��     ���������ĵ�����ת698��ʽ����(֧�ֹ�Լ�б�)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class19_term_prtl(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8   offlen    = 0;
    uint8   lenBuf[3] = {0};
    uint32  i = 0;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    OOP_PROTOCOL_T data = {0};

    /* ��μ�� */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class19_term_prtl error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if(convert->srcLen != sizeof(data))
    {
        dar = DATA_OTHER_REASON;
        return dar;
    }
    memcpy((uint8*)&data, &srcBuf[sOffset], convert->srcLen);
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    offlen = set_len_offset(data.nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {               
        //��Լ����
        dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.value[i], sizeof(OOP_VISIBLEVAR_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(&data.value[i].value[0], data.value[i].nNum, "data_to_buf_class19_term_prtl failed. [%d] dar(%d),value:\n", i, dar);
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
* @brief��     ���������ĵ�����ת698��ʽ����(֧�ֹ�Լ�б�)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class19_report_channel(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8   offlen    = 0;
    uint8   lenBuf[3] = {0};
    uint32  i = 0;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    OOP_CHANNEL_T data = {0};

    /* ��μ�� */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class19_report_channel error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if(convert->srcLen != sizeof(data))
    {
        dar = DATA_OTHER_REASON;
        return dar;
    }
    memcpy((uint8*)&data, &srcBuf[sOffset], convert->srcLen);
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    offlen = set_len_offset(data.nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {               
        //ͨ��
        dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.port[i], sizeof(data.port[i]));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_class19_report_channel failed. [%d] dar(%d)\n", i, dar);
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
* @brief��     ���������ĵ�����ת698��ʽ����(���豸�б�)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class19_ois(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8   offlen    = 0;
    uint8   lenBuf[3] = {0};
    uint32  i = 0;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    OOP_OIS_T data = {0};

    /* ��μ�� */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class19_ois error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if(convert->srcLen != sizeof(data))
    {
        PRTL_FMT_DEBUG("data_to_buf_class19_ois error, DATA_OTHER_REASON\n");
        dar = DATA_OTHER_REASON;
        return dar;
    }
    memcpy((uint8*)&data, &srcBuf[sOffset], convert->srcLen);
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    offlen = set_len_offset(data.nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {               
        //ͨ��OI
        dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.oi[i], sizeof(data.oi[i]));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("data_to_buf_class19_ois failed. [%d] dar(%d)\n", i, dar);
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
* @brief��      ������Ȩ�ṹ
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
OOP_DAR_E data_to_buf_service_auth(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SERVICES_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint8   offlen    = 0;
    uint8   lenBuf[3] = {0};

    MEMZERO(&data, sizeof(data));

    /* ��μ�� */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_service_auth error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if(convert->srcLen != sizeof(data))
    {
        dar = DATA_OTHER_REASON;
        return dar;
    }
    
    memcpy((uint8*)&data, &srcBuf[sOffset], convert->srcLen);
    sOffset += sizeof(data);

     //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    offlen = set_len_offset(data.nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {       
        /* �ṹ���� */
        dstBuf[dOffset++] = DT_STRUCTURE;

        /* �ṹ��Ա�� */
        dstBuf[dOffset++] = 2;
        
        //�������
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.item[i].type, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, dOffset);
            return DATA_TYPE_UNMATCHED;
        }
        
        //��Ȩʱ��
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.item[i].times, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, dOffset);
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
* @brief��     ���������ĵ�����ת698��ʽ����(class19)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class19_4300(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar   = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8  *srcBuf = (uint8 *)convert->srcBuf;
    uint8  *dstBuf  = (uint8 *)convert->dstBuf;

    /* ��μ�� */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class19_term_name error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    switch (pOadInfo->pTab->eData)
    {    
        case E_OOP_VERSION_T:
        {
            /* �����豸�汾��Ϣ */
            return data_to_buf_class19_term_var(pOadInfo, convert);
            break;
        }
        case E_OOP_PROTOCOL_T:
        {
            /* �����豸��Լ�б� */
            return data_to_buf_class19_term_prtl(pOadInfo, convert);
            break;
        }
        case E_OOP_CHANNEL_T:
        {
            /* �ϱ�ͨ�� */
            return data_to_buf_class19_report_channel(pOadInfo, convert);
            break;
        }
        default:
        {
            if (convert->srcLen != pOadInfo->pTab->dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            dar = basic_data_to_buf(pOadInfo->pTab->eData, dstBuf, convert->dstLen, &dOffset, &srcBuf[sOffset], pOadInfo->pTab->dataLen);
            if (dar == DATA_SUCCESS)
            {
                sOffset += pOadInfo->pTab->dataLen;
            }
            break;
        }
    }

    if (dar == DATA_SUCCESS)
    {
        *convert->dOffset = dOffset;
        *convert->sOffset = sOffset;
    }

    return dar;
}
OOP_DAR_E data_to_buf_device_info(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_DEVICERUNSTAT_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);
    
    //�ṹ����
    dstBuf[dOffset++] = DT_STRUCTURE;

    //�ṹ��Ա��
    dstBuf[dOffset++] = 15;
    //CPU����
    dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.cpuCoreNum, sizeof(data.cpuCoreNum));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //cpu��Ƶ
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.cpuMainFrq, sizeof(data.cpuMainFrq));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //cpu����
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.cpuCache, sizeof(data.cpuCache));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //cpu�ܹ�
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.cpuArchi, sizeof(data.cpuArchi));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //�����ڴ�
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.phyMem, sizeof(data.phyMem));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //�����ڴ�
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.virMem, sizeof(data.virMem));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //�����ܿռ�
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.diskSpace, sizeof(data.diskSpace));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //�ڴ�ʹ����
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.memUsage, sizeof(data.memUsage));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //����ʹ����
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.diskUsage, sizeof(data.diskUsage));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //cpuʹ����
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.cpuUsage, sizeof(data.cpuUsage));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //����ϵͳ����
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.osName, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //����ϵͳ�汾
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.osVer, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //����ϵͳ�ں�
    dar = basic_data_to_buf(E_OOP_VISIBLEVAR_T, dstBuf, dstLen, &dOffset, &data.osKernel, sizeof(OOP_VISIBLEVAR_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }
    //ϵͳ����ʱ��
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.sysStrtm, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

	//�¶�
    dar = basic_data_to_buf(E_LONG, dstBuf, dstLen, &dOffset, &data.tempBroad, sizeof(int16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return DATA_TYPE_UNMATCHED;
    }

    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }
    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar; 
}

#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)|| (defined AREA_JIANGSU)
/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class19)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_line_lose(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_Line_Loss_T lineLose;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&lineLose, sizeof(lineLose)); 

    memcpy(&lineLose, &srcBuf[sOffset], sizeof(lineLose));
    sOffset += sizeof(lineLose);

    //�ṹ������
    dstBuf[dOffset++] = DT_STRUCTURE;
    dstBuf[dOffset++] = 2;
    //����ֵ
    dstBuf[dOffset++] = DT_DOUBLE_LONG;
    memcpy_r(&dstBuf[dOffset], &lineLose.linelossvalue, 4);
    dOffset += 4;    
    //������
    dstBuf[dOffset++] = DT_LONG;
    memcpy_r(&dstBuf[dOffset], &lineLose.linelossrate, 2);
    dOffset += 2;
    
    //����Խ��
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
* @brief��     ���������ĵ�����ת698��ʽ����(class19)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_switch_impedance(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_Switch_Impedance_T impedance;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    int i;

    MEMZERO(&impedance, sizeof(impedance)); 

    memcpy(&impedance, &srcBuf[sOffset], sizeof(impedance));
    sOffset += sizeof(impedance);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;
    dstBuf[dOffset++] = impedance.num;

    for (i = 0; i < impedance.num; i++)
    {
        //�ṹ������
        dstBuf[dOffset++] = DT_STRUCTURE;
        dstBuf[dOffset++] = 5;
        //�ڵ��ַ
        dar = basic_data_to_buf(E_TSA, dstBuf, dstLen, &dOffset, &impedance.switchhindrance[i].addr, sizeof(OOP_TSA_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        //�迹ֵ
        dstBuf[dOffset++] = DT_DOUBLE_LONG_UNSIGNED;
        memcpy_r(&dstBuf[dOffset], &impedance.switchhindrance[i].lineloss, 4);
        dOffset += 4;
        //�����仯ǰʱ��
        dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &impedance.switchhindrance[i].beforchangetime, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        //�����仯��ʱ��
        dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &impedance.switchhindrance[i].afterchangetime, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return DATA_TYPE_UNMATCHED;
        }
        //�仯����
        dstBuf[dOffset++] = DT_DOUBLE_LONG;
        memcpy_r(&dstBuf[dOffset], &impedance.switchhindrance[i].Current, 4);
        dOffset += 4;       
    }
    
    //����Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}
#endif

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class19)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class19(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x43000200:   //�����豸����
        case 0x43000300:   //�����豸�汾
        case 0x43000400:   //�����豸��������
        case 0x43000600:   //֧�ֹ�Լ�б�
        case 0x43000700:   //��������ϱ�
        case 0x43000800:   //���������ϱ�
        case 0x43000900:   //��������վͨ��
        case 0x43000a00:   //�ϱ�ͨ��
        {
            return data_to_buf_class19_4300(pOadInfo, convert);
        }break;

        case 0x43000500:   //�����豸��Լ�б�
        {
            return data_to_buf_class19_ois(pOadInfo, convert);
        }break;

        case 0x43000B00:   //������Ȩ״̬
        {
            return data_to_buf_service_auth(pOadInfo, convert);
        }break;

        case 0x43000c00:    //�豸������Ϣ
        {
            return data_to_buf_device_info(pOadInfo, convert);
        }
        break;
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
        case 0x43201300:    //���ܿ�������
        {
            return data_to_buf_line_lose(pOadInfo, convert);
        }
        break;
        case 0x43201700:    //���ܿ��ػ�·�迹
        case 0x43201701:
        case 0x43201702:
        case 0x43201703:
        {
            return data_to_buf_switch_impedance(pOadInfo, convert);
        }
        break;
#endif
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief��      ��698��ʽ����ת�������ĵ�����
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
OOP_DAR_E buf_to_data_class19(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}

/**
*********************************************************************
* @name��      class19_invoke_get_term_name
* @brief��     class19��GET����
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
OOP_DAR_E class19_invoke_get_term_name(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    //NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;  

    MEMZERO(&convert, sizeof(convert)); 

    switch (pOadInfo->pTab->oad.value)
    {
    /* �豸���� */
    case 0x43000200:
        {
            OOP_VISIBLEVAR32_T data = {0};
            uint8 name[] = "gwmdl_xl";

            memcpy(&data.value[0], &name[0], sizeof(name));
            data.nNum = sizeof(name);

            PRTL_FMT_LOGCC("class19_invoke_get_term_name : data = %d\n", data);

            //ת���ɱ�������
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
     
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name��      class21_invoke_get_esam_ver
* @brief��     class21��GET����
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
OOP_DAR_E class19_invoke_get_term_var(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;    

    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&normal, sizeof(normal));

    //��������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000300:
        {
            OOP_VERSION_T data;
            OOP_VISIBLESTR4_T factory;
            char ver[64] = {0};

            MEMZERO(&data, sizeof(data)); 
            MEMZERO(&factory, sizeof(factory));
            normal.oad.value = 0xfff00400;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_VISIBLESTR4_T), (uint8*)&factory, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                PRTL_FMT_LOGCC("fill in the default factory\n");
                memcpy(&data.szFactory.value[0], &g_tVer.factory[0], 4);
            }
            else
            {
                memcpy(&data.szFactory.value[0], factory.value, 4);
            }

            memcpy(&data.szSoftVer.value[0], &g_tVer.softVer[0], 4);
            memcpy(&data.szExtend.value[0], &g_tVer.extend[5], 3);

            memcpy(&data.szSoftDate.value[0], &g_tVer.softDate[0], 6);
            memcpy(&data.szHardVer.value[0], &g_tVer.hardVer[0], 4);
            memcpy(&data.szHardDate.value[0], &g_tVer.hardDate[0], 6);

            if (app_update_version_get(ver) == 0)
            {
                memcpy(&data.szSoftVer.value[0], ver, 4);
            }

            PRTL_BUF_LOGCC(&data.szFactory.value[0], 8, "class19_invoke_get_term_var : szFactory:");
            PRTL_BUF_LOGCC(&data.szSoftVer.value[0], 8, "class19_invoke_get_term_var : szSoftVer:");
            PRTL_BUF_LOGCC(&data.szSoftDate.value[0], 8, "class19_invoke_get_term_var : szSoftDate:");
            PRTL_BUF_LOGCC(&data.szHardVer.value[0], 8, "class19_invoke_get_term_var : szHardVer:");
            PRTL_BUF_LOGCC(&data.szHardDate.value[0], 8, "class19_invoke_get_term_var : szHardDate:");
            PRTL_BUF_LOGCC(&data.szExtend.value[0], 8, "class19_invoke_get_term_var : szExtend:");

            //ת���ɱ�������
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }

    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name��      class19_invoke_get_term_date
* @brief��     class19��GET����
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
OOP_DAR_E class19_invoke_get_term_date(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    //NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;  

    MEMZERO(&convert, sizeof(convert)); 

    switch (pOadInfo->pTab->oad.value)
    {
    /* �豸���� */
    case 0x43000400:
        {
            OOP_DATETIME_S_T data = {0};

            //��д��������
            data.year = 2019;
            data.month = 12;
            data.day = 31;
            data.hour = 14;
            data.minute = 42;
            data.second = 0;
            
            //ת���ɱ�������
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
     
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name��      class19_invoke_get_ois
* @brief��     class19��GET����
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
OOP_DAR_E class19_invoke_get_ois(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;    

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000500:
        {
            uint32 i = 0;
            OOP_OIS_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), len(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, len, normal.logicId, normal.infoNum, normal.oad.value);
            }

            PRTL_FMT_LOGCC("class19_invoke_get_ois data.nNum:%d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class19_invoke_get_ois data.oi[i].value:0x%04x\n", data.oi[i]);
            }

            //ת���ɱ�������
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }

    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name��      class19_invoke_get_term_date
* @brief��     class19��GET����
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
OOP_DAR_E class19_invoke_get_term_prtl(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    //NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0; 

    MEMZERO(&convert, sizeof(convert)); 

    switch (pOadInfo->pTab->oad.value)
    {
    /* �豸֧�ֵĹ�Լ�б� */
    case 0x43000600:
        {
            OOP_PROTOCOL_T data = {0};
            char prtl[] = {"DL/T698.45"};
            //��д��Լ�б�
            data.nNum = 1;
            data.value[0].nNum = sizeof(prtl);
            memcpy(&data.value[0].value[0], prtl, data.value[0].nNum);
            
            //ת���ɱ�������
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
     
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name��      class19_invoke_get_follow_allow
* @brief��     class19��GET����
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
OOP_DAR_E class19_invoke_get_follow_allow(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;    

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000700:
        {
            uint8 data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            PRTL_FMT_LOGCC("class19_invoke_get_report_allow data:%d\n", data);

            //ת���ɱ�������
            convert.srcBuf = &data;
            convert.srcLen = sizeof(uint8);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }

    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name��      class19_invoke_get_report_allow
* @brief��     class19��GET����
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
OOP_DAR_E class19_invoke_get_report_allow(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;    

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000800:
        {
            uint8 data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            PRTL_FMT_LOGCC("class19_invoke_get_report_allow data:%d\n", data);

            //ת���ɱ�������
            convert.srcBuf = &data;
            convert.srcLen = sizeof(uint8);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }

    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name��      class19_invoke_get_call_allow
* @brief��     class19��GET����
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
OOP_DAR_E class19_invoke_get_call_allow(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;    

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000900:
        {
            uint8 data = 0;
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            PRTL_FMT_LOGCC("class19_invoke_get_report_allow data:%d\n", data);

            //ת���ɱ�������
            convert.srcBuf = &data;
            convert.srcLen = sizeof(uint8);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }

    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name��      class19_invoke_get_report_channel
* @brief��     class19��GET����
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
OOP_DAR_E class19_invoke_get_report_channel(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;    

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000a00:
        {
            uint32 i = 0;
            OOP_CHANNEL_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), len(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, len, normal.logicId, normal.infoNum, normal.oad.value);
                
                data.nNum = 2;
                data.port[0].value = 0x45000000;
                data.port[1].value = 0x45100000;
            }

            PRTL_FMT_LOGCC("class19_invoke_get_report_allow data.nNum:%d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class19_invoke_get_report_allow data.port[i].value:0x%08x\n", data.port[i].value);
            }

            //ת���ɱ�������
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }

    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name��      class19_invoke_get_service_auth
* @brief��     class19��GET����
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
OOP_DAR_E class19_invoke_get_service_auth(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;    

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000B00:
        {
            OOP_SERVICES_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), len(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, len, normal.logicId, normal.infoNum, normal.oad.value);
            }

            //Ŀǰ������Ȩֻ��SSH�����Բ�����nNum
            if(data.nNum == 0)
            {
                data.nNum = 1;
            }

            //ת���ɱ�������
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }

    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

OOP_DAR_E class19_invoke_get_deviceruninfo(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&convert, sizeof(convert)); 
    MEMZERO(&normal, sizeof(normal));

    //��������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000C00:
        {
            OOP_DEVICERUNSTAT_T data;
            appmain_get_deviceinfo(&data);
            //ת���ɱ�������
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }

    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class19(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class19 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name��      class19_invoke_get_4300
* @brief��     class19��GET����
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
OOP_DAR_E class19_invoke_get_4300(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{        
    switch (pOadInfo->pTab->oad.attID)
    {
    case 2:
        return class19_invoke_get_term_name(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 3:
        return class19_invoke_get_term_var(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 4:
        return class19_invoke_get_term_date(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 5:
        return class19_invoke_get_ois(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 6:
        return class19_invoke_get_term_prtl(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 7:
        return class19_invoke_get_follow_allow(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 8:
        return class19_invoke_get_report_allow(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 9:
        return class19_invoke_get_call_allow(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 10:
        return class19_invoke_get_report_channel(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 11:
        return class19_invoke_get_service_auth(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;    
    case 12:
        return class19_invoke_get_deviceruninfo(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @brief��     ���õ����豸�汾��Ϣ
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
OOP_DAR_E buf_to_data_class19_set_term_ver(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_VERSION_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ṹ����
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ա��
    if(srcBuf[sOffset++] != 6)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //���̴���
    dar = basic_buf_to_data(E_OOP_VISIBLESTR4_T, srcBuf, srcLen, &sOffset, &data.szFactory, sizeof(OOP_VISIBLESTR4_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //����汾��
    dar = basic_buf_to_data(E_OOP_VISIBLESTR4_T, srcBuf, srcLen, &sOffset, &data.szSoftVer, sizeof(OOP_VISIBLESTR4_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //����汾����
    dar = basic_buf_to_data(E_OOP_VISIBLESTR6_T, srcBuf, srcLen, &sOffset, &data.szSoftDate, sizeof(OOP_VISIBLESTR6_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    //Ӳ���汾��
    dar = basic_buf_to_data(E_OOP_VISIBLESTR4_T, srcBuf, srcLen, &sOffset, &data.szHardVer, sizeof(OOP_VISIBLESTR4_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //Ӳ���汾����
    dar = basic_buf_to_data(E_OOP_VISIBLESTR6_T, srcBuf, srcLen, &sOffset, &data.szHardDate, sizeof(OOP_VISIBLESTR6_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //������չ��Ϣ
    dar = basic_buf_to_data(E_OOP_VISIBLESTR8_T, srcBuf, srcLen, &sOffset, &data.szExtend, sizeof(OOP_VISIBLESTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
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

    PRTL_BUF_LOGCC(&data.szFactory.value[0], 8, "buf_to_data_class19_set_term_ver : szFactory:");
    PRTL_BUF_LOGCC(&data.szSoftVer.value[0], 8, "buf_to_data_class19_set_term_ver : szSoftVer:");
    PRTL_BUF_LOGCC(&data.szSoftDate.value[0], 8, "buf_to_data_class19_set_term_ver : szSoftDate:");
    PRTL_BUF_LOGCC(&data.szHardVer.value[0], 8, "buf_to_data_class19_set_term_ver : szHardVer:");
    PRTL_BUF_LOGCC(&data.szHardDate.value[0], 8, "buf_to_data_class19_set_term_ver : szHardDate:");
    PRTL_BUF_LOGCC(&data.szExtend.value[0], 8, "buf_to_data_class19_set_term_ver : szExtend:");

    return dar;
}

/**
*********************************************************************
* @brief��     ��������ϱ�
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
OOP_DAR_E buf_to_data_class19_set_follow_allow(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));


    //�Ƿ������ϱ�
    dar = basic_buf_to_data(E_BOOL, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
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
* @brief��     ���õ����豸�汾��Ϣ
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
OOP_DAR_E buf_to_data_class19_set_report_allow(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));


    //�Ƿ������ϱ�
    dar = basic_buf_to_data(E_BOOL, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
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
* @brief��     ��������վͨ��
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
OOP_DAR_E buf_to_data_class19_set_call_allow(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8 data = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));


    //�Ƿ������ϱ�
    dar = basic_buf_to_data(E_BOOL, srcBuf, srcLen, &sOffset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
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
* @brief��     ���õ����豸�汾��Ϣ
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
OOP_DAR_E buf_to_data_class19_set_report_channel(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CHANNEL_T data = {0};
    uint8 offlen = 0;      //������������
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));


    //��������
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�����Ա����
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //ͨ��OAD
        dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.port[i], sizeof(OOP_OAD_U));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }      
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
* @brief��     �������豸�б�
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
OOP_DAR_E buf_to_data_class19_set_ois(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_OIS_T data = {0};
    uint8 offlen = 0;      //������������
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));


    //��������
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�����Ա����
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //ͨ��OAD
        dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.oi[i], sizeof(data.oi[i]));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }      
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
* @brief��     ��698��ʽ����ת�������ĵ�����(class192)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class19_set(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x43000300:   //���ð汾��Ϣ
        {
            return buf_to_data_class19_set_term_ver(pOadInfo,  convert);
        }break;

        case 0x43000700:   //��������ϱ�
        {
            return buf_to_data_class19_set_follow_allow(pOadInfo,  convert);
        }break;

        case 0x43000800:   //���������ϱ�
        {
            return buf_to_data_class19_set_report_allow(pOadInfo,  convert);
        }break;

        case 0x43000900:   //���������ϱ�
        {
            return buf_to_data_class19_set_call_allow(pOadInfo,  convert);
        }break;

        case 0x43000500:   //���豸�б�
        {
            return buf_to_data_class19_set_ois(pOadInfo,  convert);
        }break;

        case 0x43000a00:   //�ϱ�ͨ��
        {
            return buf_to_data_class19_set_report_channel(pOadInfo,  convert);
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
* @brief��      �˿���Ȩ�ṹ
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
OOP_DAR_E buf_to_data_service_auth(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_SERVICE_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));
    
    //�ṹ����
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ա��
    if(srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //�˿����
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.type, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //��Ȩʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.times, sizeof(uint8));
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
* @name��      class19_invoke_set_version
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_set_version(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;
 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class19_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000300:
        {
            OOP_VERSION_T data;
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = 0x43000300;
            normal.classtag = CLASS_DATA_UNINIT;

            PRTL_FMT_LOGCC("class19_invoke_set_version : data = 0x%08x\n", data);

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_VERSION_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
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
* @name��      class19_invoke_set_follow_allow
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_set_follow_allow(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class19_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000700:
        {
            uint8 data;
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class19_invoke_set_report_allow : data = 0x%x\n", data);

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
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
* @name��      class19_invoke_set_report_allow
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_set_report_allow(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class19_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000800:
        {
            uint8 data;
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class19_invoke_set_report_allow : data = 0x%x\n", data);

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
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
* @name��      class19_invoke_set_call_allow
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_set_call_allow(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class19_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000900:
        {
            uint8 data;
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class19_invoke_set_report_allow : data = 0x%x\n", data);

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
#ifdef PRODUCT_ZCU_1
            //רһ��Ҫ����̨����
            uint8 recvbuf[1024]={0};
            uint16 recvlen;
            recvlen = appmain_set_radio_speak(data,recvbuf,sizeof(recvbuf));
            if(recvlen == 1 && recvbuf[0] == 1)
            {
                PRTL_FMT_DEBUG("���õ�̨ͨ���ɹ�\n");
            }else
            {
                PRTL_FMT_DEBUG("���õ�̨ͨ��ʧ�� recvlen %d recvbuf[0] %d \n",recvlen,recvbuf[0]);
            }
#endif
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
* @name��      class19_invoke_set_report_allow
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_set_report_channel(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;
 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class19_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000a00:
        {
            uint32 i = 0;
            OOP_CHANNEL_T data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class19_invoke_set_report_channel : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class19_invoke_set_report_channel : data.port[%d].value = 0x%08x\n", i, data.port[i].value);
            }

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
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
* @name��      class19_invoke_set_ois
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_set_ois(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;
 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class19_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class19_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000500:
        {
            uint32 i = 0;
            OOP_OIS_T data = {0};
            memcpy(&data, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("class19_invoke_set_ois : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class19_invoke_set_ois : data.port[%d].value = 0x%08x\n", i, data.oi[i]);
            }

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
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
* @name��      class8_invoke_set__port_authorize
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
OOP_DAR_E class19_invoke_set_operate_passwd(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_VISIBLEVAR32_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = basic_data_from_fmt698(E_OOP_VISIBLEVAR32_T, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "operate passwd basic_data_from_fmt698 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_VISIBLEVAR32_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name��      class19_invoke_set_4300
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_set_4300(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000300:
        return class19_invoke_set_version(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;   
    case 0x43000500:
        return class19_invoke_set_ois(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x43000700:
        return class19_invoke_set_follow_allow(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x43000800:
        return class19_invoke_set_report_allow(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x43000900:
        return class19_invoke_set_call_allow(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x43000a00:
        return class19_invoke_set_report_channel(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��      inner_data_init
* @brief��     �ڲ����ݳ�ʼ������
* @return    ��
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
void inner_data_init(uint8 type, DB_CLIENT fd)
{
    rpt_data_init(type, fd);
    sta_data_init(type, fd);
}

/**
*********************************************************************
* @name��      class19_invoke_act_data_clear
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_act_data_clear(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    CLEAR_DATA_T clrData;
    uint32  offset  = 0;
    uint8 logicAddr = 0;

    MEMZERO(&clrData, sizeof(clrData));

    //���ݳ�ʼ������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000300:
        {
            if (pApduInfo->NoEsamMac)
            {
                ret = data_init_sent_to_acmeter();
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("data_init_sent_to_acmeter err. ret(%d), logicId(%d)\n",
                    ret, logicAddr);
                    return DATA_HARDWARE_FAULT;
                }
                break;
            }

            logicAddr = pApduInfo->logicAddr;
            clrData.recordOAD.logicId = logicAddr;
            clrData.bClassTag = FALSE;

            //clrData.recordOAD.classtag = CLASS_DATA_INIT;
            ret = statis_clear_data(pApduInfo->hUdp, &clrData);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_clear_data failed. ret(%d), logicId(%d)\n",
                    ret, logicAddr);

                //̨�������ʾ���ݳ�ʼ����ʱ��ʧ�ܣ���ע�͵�
                //return DATA_OTHER_REASON;
            }
            else
            {
                PRTL_FMT_DEBUG("db_clear_data ok. ret(%d), logicId(%d)\n",
                    ret, logicAddr);
            }

            // clrData.recordOAD.classtag = CLASS_DATA_NORMAL;
            // ret = db_clear_data(pApduInfo->hUdp, &clrData);
            // if (ret != ERR_OK)
            // {
            //     PRTL_FMT_DEBUG("db_clear_data failed. ret(%d), logicId(%d)\n",
            //         ret, logicAddr);
            //     return DATA_OTHER_REASON;
            // }
            // else
            // {
            //     PRTL_FMT_DEBUG("db_clear_data ok. ret(%d), logicId(%d)\n",
            //         ret, logicAddr);
            // }

            inner_data_init(PRTL_INIT_DATA, pApduInfo->hUdp);

            //���ݳ�ʼ��������ϵ���Ϣ
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));

            //reset_real_proc ��������
            set_reset_flag(pApduInfo->hUdp, TMN_DATA_INIT);
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
* @name��      class19_invoke_act_restore_settings
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_act_param_restore(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset  = 0;
    uint8 logicAddr = pApduInfo->logicAddr;

    ret = statis_recover_default(pApduInfo->hUdp, logicAddr);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_recover_default failed. ret(%d), logicId(%d)\n",
        ret, logicAddr);

        return DATA_OTHER_REASON;
    }
    else
    {
        PRTL_FMT_DEBUG("db_recover_default ok. ret(%d), logicId(%d)\n",
        ret, logicAddr);
    }

    inner_data_init(PRTL_INIT_PARAM, pApduInfo->hUdp);

    //��������Ĭ�ϲ����Ľӿ� �������
    app_setdefault_cfg_area(pApduInfo->hUdp);

    //��־���ݴ���
    diag_sig_handler(SIGTERM);

    //�ø�λ��� reset_real_proc ��������
    set_reset_flag(pApduInfo->hUdp, TMN_PARA_INIT);

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��      class19_invoke_act_event_clear
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_act_event_clear(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    CLEAR_DATA_T clrData;
    uint32  offset  = 0;
    uint8 logicAddr = 0;

    MEMZERO(&clrData, sizeof(clrData));

    //���ݳ�ʼ������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000500:
        {
            logicAddr = pApduInfo->logicAddr;
            clrData.recordOAD.logicId = logicAddr;
            clrData.bClassTag = TRUE;

            clrData.recordOAD.classtag = CLASS_DATA_EVENT;
            ret = db_clear_data(pApduInfo->hUdp, &clrData);
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_clear_data failed. ret(%d), logicId(%d)\n",
                    ret, logicAddr);

                //̨�������ʾ���ݳ�ʼ����ʱ��ʧ�ܣ���ע�͵�
                //return DATA_OTHER_REASON;
            }
            else
            {
                PRTL_FMT_DEBUG("db_clear_data ok. ret(%d), logicId(%d)\n",
                    ret, logicAddr);
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
* @name��      class19_invoke_act_demand_clear
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_act_demand_clear(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset  = 0;

    //������ʼ������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000600:
        {
            if (pApduInfo->NoEsamMac)
            {
                ret = demand_init_sent_to_acmeter();
                if (ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("demand_init_sent_to_acmeter err. ret(%d), logicId(%d)\n",
                        ret, pApduInfo->logicAddr);
                    return DATA_HARDWARE_FAULT;
                }
                break;
            }
            else
            {
                return DATA_OBJECT_UNAVAILABLE;
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
* @name��      class19_invoke_act_service_auth
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_act_service_auth(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;
    int i = 0;
    uint32 len = 0;
    uint8 msgdata[8] = {0};
    uint16 msglen=0;
    uint32 authtime = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    dar = buf_to_data_service_auth(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "class19_invoke_act_service_auth failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //���ݳ�ʼ������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000700:
        {
            OOP_SERVICE_T dataAct;
            OOP_SERVICES_T dataGet = {0};
            
            memcpy(&dataAct, convert.dstBuf, dOffset);
            //��Դ�������Ļ�����255��
#if !defined(PRODUCT_ECU)
            //����ϵͳ����������Ϣ��4���ֽ� �������ת��
            if(dataAct.times == 255)
                authtime = 0xffffffff;
            else
#endif
                authtime = dataAct.times;

            msgdata[msglen++] = 1;
            msgdata[msglen++] = dataAct.type;
            memcpy_r(&msgdata[msglen], &authtime, sizeof(uint32));
            msglen += sizeof(uint32);

            ret = appmain_send_msg(MSG_APPAMIN_SERVICE_START, 0, msgdata, msglen);
            if(ret != ERR_OK)
            {
                AUTH_FMT_DEBUG("auth msg send failed. ret(%d)\n",ret);
                return DATA_OTHER_REASON;
            }

            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(dataGet), (uint8*)&dataGet, &len);

            for(i = 0; i < dataGet.nNum; i++)
            {
                if(dataGet.item[i].type == dataAct.type)
                {
                    memcpy(&dataGet.item[i], &dataAct, sizeof(OOP_SERVICE_T));
                    break;
                }
            }

            if((i == dataGet.nNum) && (i < OOP_MAX_SERVICES))
            {
                memcpy(&dataGet.item[i], &dataAct, sizeof(OOP_SERVICE_T));
                dataGet.nNum++;
            }
            
            normal.oad.nObjID  = pOadInfo->pTab->oad.nObjID;
            normal.oad.attID = 11;
            normal.classtag = CLASS_DATA_UNINIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataGet, sizeof(OOP_SERVICES_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
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
* @name��      class19_invoke_act_screen_passwd
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_act_operate_passwd(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
//    NOMAL_OAD_T normal = {0};
    OOP_VISIBLEVAR32_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = basic_data_from_fmt698(E_OOP_VISIBLEVAR32_T, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "operate passwd basic_data_from_fmt698 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //����Һ�����룬ƫ��4������6
    ret = write_pdata_xram("^gui_paivate_file", data.value, 4, strlen(data.value));
    ret |= write_pdata_xram("/data/app/desktopGui/data/^gui_paivate_file", data.value, 4, strlen(data.value));
    if (ret < 0)
    {
        PRTL_FMT_DEBUG("operate_passwd err. ret[%d]\n", ret);
        return DATA_READ_WRITE_DENIED;
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
* @name��      class19_invoke_act_reboot
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_act_reboot(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E   dar = DATA_SUCCESS;
    uint32      offset  = 0;
    OOP_WORD2_T data;

    MEMZERO(&data, sizeof(data));

    //�ն˸�λ
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000100:
        {
            if(pApduInfo->logicAddr != 0)
            {
                return dar;
            }
            //��־���ݴ���
            diag_sig_handler(SIGTERM);
            
            //�ø�λ��� reset_real_proc ��������
            set_reset_flag(pApduInfo->hUdp, TMN_REBOOT);
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
* @name��      class19_invoke_act_4300
* @brief��     class19��ACTION����
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
OOP_DAR_E class19_invoke_act_4300(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x43000100: //��λ
        return class19_invoke_act_reboot(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;    
    case 0x43000300: //���ݳ�ʼ��
        return class19_invoke_act_data_clear(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x43000400: //�ָ���������
        return class19_invoke_act_param_restore(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;   
    case 0x43000500: //�¼���ʼ��
        return class19_invoke_act_event_clear(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x43000600: //������ʼ��
        return class19_invoke_act_demand_clear(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;   
    case 0x43000700: //������Ȩ
        return class19_invoke_act_service_auth(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;    
    case 0x43007f00: //����Һ����������
        return class19_invoke_act_operate_passwd(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;       
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

#if DESC("class19����ӿں���",1)

/**
*********************************************************************
* @name��      class19_invoke_set
* @brief��     class19��SET����
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
OOP_DAR_E class19_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x4300:
        return class19_invoke_set_4300(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name��      class19_invoke_get
* @brief��     class19��GET����
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
OOP_DAR_E class19_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x4300:
        {
            return class19_invoke_get_4300(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @name��       class19_invoke_act
* @brief��      class19��ACTION����
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
OOP_DAR_E class19_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x4300:
        return class19_invoke_act_4300(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break;    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    return DATA_OBJECT_UNAVAILABLE;
}


#endif
