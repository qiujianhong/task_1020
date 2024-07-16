/*
*********************************************************************
* @file    oopSecurity.h
* @brief�� apdu��ȫ�������
* @author������
* @date��   2019.12.06
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/
#ifndef __OOP_SECURITY_H__
#define __OOP_SECURITY_H__

// �����ļӽ���
#define LONG_ENCRYPTION_EN 1

// ���ɴ����APDU����
#define APDU_BUFF_SIZE_MAX		4096

/**********************************************************************
* @name      : security_mod_get
* @brief     �����ݶ���OI��ȡĬ�ϵİ�ȫģʽ-�ն�
* @param[in] ��OI:        ����OI
* @param[out]��
* @return    ��SecModWd_t ��ȫģʽ
* @Create    : 
* @Date      ��2019-12-31
* @Update    :
**********************************************************************/
SecModWd_t security_mod_get(CHANNEL_CACHE_T *ptChannel, uint16 OI);

/**********************************************************************
* @name      : securityrequest_getapdu
* @brief     ����ȫ���� ������APDU���� ��������װ�ɱ���
* @param[in] ��FRAME_HEAD_T *ptFrame        ����֡ͷ
               uint16 outsize               ������Ϣ��󳤶�
               APP_CONTEXT_t *pConnect      Ӧ����������
* @param[out]��uint8 *pOutMsg               ������Ϣ
               uint8 *pSecurity             ��ȫ������
* @return    ����Ϣ����
* @Create    : 
* @Date      ��2019-12-5
* @Update    :
**********************************************************************/
uint32 protocol_securityrequest(FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint32 outsize, APP_CONTEXT_t *pContext, 
                                uint8 *pSecurity);

/**********************************************************
* �������ƣ�
* ����������
    ����һ����ȫAPDU

* ����������
* �������:

* ����ֵ��
**********************************************************/
uint16 GenSecurityApdu(uint8 *apduData, uint16 apduDataLen,SecurityMode_t SecMod,uint8 SecLev);

/**********************************************************
* �������ƣ�
* ����������
    ����һ����ȫAPDU

* ����������
* �������:

* ����ֵ��
**********************************************************/
uint32 GenSecurityApdu_long(uint8 *apduData, uint32 apduDataLen,SecurityMode_t SecMod,uint8 SecLev);

/**********************************************************
* �������ƣ�
* ����������
    ����һ���ϱ���ȫAPDU

* ����������
* �������:

* ����ֵ��
**********************************************************/
uint16 GenReportSecurityApdu(uint8 *apduData, uint16 apduDataLen);

/**********************************************************************
* @name      : esam_state_guishow
* @brief     ��esam��Կ�·�״̬
* @param[in] ��esamState �ڲ�Э��     
* @param[out]��
* @return    ��0-�ɹ�/������
**********************************************************************/
int esam_state_guishow(uint8 esamState);

#endif