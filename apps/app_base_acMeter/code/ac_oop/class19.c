/*
*********************************************************************
* @file    class19.c
* @brief�� class19����
* @author��
* @date��   2021.10.22
* @copyright 2021 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/

#include "baseCommon.h"
#include "oadInfoList.h"
#include "commClass.h"
#include "oopservice.h"
#include "ac_sample.h"
#include "ac_main.h"

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
OOP_DAR_E data_to_buf_class19_term_var(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    OOP_DAR_E dar = DATA_OTHER_REASON;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    OOP_VERSION_T data;
    MEMZERO(&data, sizeof(data));

    /* ��μ�� */
    if ((pOadInfo == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL) || (convert->sOffset == NULL))
    {
        PRTL_FMT_DEBUG("data_to_buf_class21 error, ERR_PTR\n");
        return DATA_READ_WRITE_DENIED;
    }

    if (convert->srcLen != sizeof(OOP_VERSION_T))
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
    if ((sOffset > srcLen) || (dOffset > dstLen))
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
OOP_DAR_E data_to_buf_class19(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert)
{
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x43000300:			   //�汾��Ϣ
        {
            return data_to_buf_class19_term_var(pOadInfo, convert);
        }break;

        //......

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}