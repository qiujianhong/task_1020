/*
*********************************************************************
* Copyright(C) 2021 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ������Ϣ�ӿ�
* @date��    2021-7-9
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "message.h"
#include "radio.h"

/**********************************************************************
* @name      : radio_config_split
* @brief     ���������ݽ���
* @param[in] ��uint8 *pMsg                  ��Ϣ����
               uint16 msglen               ��Ϣ����
* @param[out]��RADIO_CONFIG_T *pConfig      ����
* @return    ��0-�ɹ�/����-������
* @Create    : ����
* @Date      ��2021-7-9
* @Update    :
**********************************************************************/
int radio_config_split(uint8 *pMsg, uint16 msglen, RADIO_CONFIG_T *pConfig)
{
    uint16 offset = 0;
    uint8  bytesnum = 0;
    uint16 len = 0;
    
    if(NULL == pMsg || msglen < 7 || NULL == pConfig)
    {
        return -1;
    }

    /* Ƶ�� */
    pConfig->channel = pMsg[offset++];

    /* ���� */
    pConfig->power = pMsg[offset++];

    /* ���뼶�� */
    pConfig->noise = pMsg[offset++];

    #if 0
    /* ������ʽ */
    pConfig->mode = pMsg[offset++];

    /* Ƶ��ƫֵ */
    pConfig->offVaule = pMsg[offset++];
    #endif
    
    /* �������� */
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    offset += bytesnum;
    pConfig->areaNo.nNum = len;
    if (len > 0)
    {
        memcpy(pConfig->areaNo.value, pMsg + offset, len);
    }
    offset += len;

    /* �豸��ַ */
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    offset += bytesnum;
    pConfig->devAddr.nNum = len;
    if (len > 0)
    {
        memcpy(pConfig->devAddr.value, pMsg + offset, len);
    }
    offset += len;

    /* �ն˵�ַ */
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    offset += bytesnum;
    pConfig->addr.nNum = len;
    if (len > 0)
    {
        memcpy(pConfig->addr.value, pMsg + offset, len);
    }
    offset += len;
    
    return 0;
}

/**********************************************************************
* @name      : radio_config_package
* @brief     �������������ݷ�װ
* @param[in] ��BLE_MSG_T *ptMsg           ��������
               uint16 buflen              ���泤��
* @param[out]��uint8 *pBuf                ��Ϣ����
* @return    ����Ϣ����
* @Create    : ����
* @Date      ��2021-7-9
* @Update    :
**********************************************************************/
uint16 radio_config_package(RADIO_CONFIG_T *pConfig, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;

    if(NULL == pConfig || NULL == pBuf || buflen < 13)
    {
        return 0;
    }

    #if 0
    /* Ƶ�� */
    pBuf[offset++] = pConfig->channel;

    /* ���书�ʵ� */
    pBuf[offset++] = pConfig->power;
    
    /* ���뼶�� */
    pBuf[offset++] = pConfig->noise;
    
    /* ������ʽ */
    pBuf[offset++] = pConfig->mode;

    /* Ƶ��ƫֵ */
    pBuf[offset++] = pConfig->offVaule;
    #endif
    
    /* ������ */
    SET_LEN_OFFSET(pConfig->areaNo.nNum, byte, bytenum);
    if(offset + bytenum + pConfig->areaNo.nNum > buflen)
    {
        return 0;
    }
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;
    memcpy(pBuf + offset, pConfig->areaNo.value, pConfig->areaNo.nNum);
    offset += pConfig->areaNo.nNum;

    /* �豸��ַ */
    SET_LEN_OFFSET(pConfig->devAddr.nNum, byte, bytenum);
    if(offset + bytenum + pConfig->devAddr.nNum > buflen)
    {
        return 0;
    }
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;
    memcpy(pBuf + offset, pConfig->devAddr.value, pConfig->devAddr.nNum);
    offset += pConfig->devAddr.nNum;

    /* �ն˵�ַ */
    SET_LEN_OFFSET(pConfig->addr.nNum, byte, bytenum);
    if(offset + bytenum + pConfig->addr.nNum > buflen)
    {
        return 0;
    }
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;
    memcpy(pBuf + offset, pConfig->addr.value, pConfig->addr.nNum);
    offset += pConfig->addr.nNum;

    return offset;
}

/**********************************************************************
* @name      : radio_msg_event_split
* @brief     ������֪ͨ��Ϣ���ݽ���
* @param[in] ��uint8 *pMsg               ��Ϣ����
               uint16 msglen             ��Ϣ����
* @param[out]��RADIO_MSG_EVENT_T *pMsgOut  �������
* @return    ��0-�ɹ�/����-������
* @Create    : ����
* @Date      ��2021-7-9
* @Update    :
**********************************************************************/
int radio_msg_event_split(uint8 *pMsg, uint16 msglen, RADIO_MSG_EVENT_T *pMsgOut)
{
    uint16 offset = 0;
    uint8  bytesnum = 0;
    uint16 len = 0;
    
    if(NULL == pMsg || msglen < 1 || NULL == pMsgOut)
    {
        return -1;
    }

    /* ���� */
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    if(offset + bytesnum + len != msglen)
    {
        return -3;
    }
    offset += bytesnum;

    if(len > RADIO_MSG_MAX_LEN)
    {
        return -4;
    }
    pMsgOut->len = len;
    memcpy(pMsgOut->msg, pMsg + offset, len);

    return 0;
}

/**********************************************************************
* @name      : radio_msg_package
* @brief     �������������ݷ�װ
* @param[in] ��BLE_MSG_T *ptMsg           ��������
               uint16 buflen              ���泤��
* @param[out]��uint8 *pBuf                ��Ϣ����
* @return    ����Ϣ����
* @Create    : ����
* @Date      ��2021-7-9
* @Update    :
**********************************************************************/
uint16 radio_msg_package(RADIO_MSG_T *ptMsg, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;
    
    if(NULL == ptMsg || NULL == pBuf || buflen < 1)
    {
        return 0;
    }

    /* ���� */
    SET_LEN_OFFSET(ptMsg->len, byte, bytenum);
    if(offset + bytenum + ptMsg->len > buflen)
    {
        return 0;
    }
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;

    /* ���� */
    memcpy(pBuf + offset, ptMsg->msg, ptMsg->len);
    offset += ptMsg->len;

    return offset;
}

