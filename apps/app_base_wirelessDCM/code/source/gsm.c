/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ����Զ�̲���ģ��GPRS��GSM����
* @date��    2019-10-29
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "hal.h"
#include "uart.h"
#include <arpa/inet.h>
#include "oopType.h"
#include "db_api.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"

#include "wireless.h"
#include "gprs_gsm.h"
#include "ppp_oper.h"
#include "at_oper.h"

#define WIRELESS_GPS_TIME   10

extern int wireless_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);
extern uint8 Isusb;

/**********************************************************************
* @name      : bytes_to_ascii
* @brief     ���ֽ�תascii
* @param[in] ��uint8 byte    �ֽ�����
* @param[out]��char *buflow  ������
               char *bufhigh ������
* @return    ��
* @Create    : ����
* @Date      ��2020-4-26
* @Update    :
**********************************************************************/
void bytes_to_ascii(uint8 byte, char *buflow, char *bufhigh)
{
    uint8 temp = 0;

    temp = ((byte >> 4) & 0xf);
    if(temp < 10)
    {
        *buflow = '0' + temp;
    }
    else
    {
        *buflow = 'A' + temp - 10;
    }

    temp = (byte & 0xf);
    if(temp < 10)
    {
        *bufhigh = '0' + temp;
    }
    else
    {
        *bufhigh = 'A' + temp - 10;
    }
}

/**********************************************************************
* @name      : ascii_to_byte
* @brief     ��asciiת�ֽ�
* @param[in] ��char ascii    ascii
* @param[out]��uint8 *byte   �ֽ�
* @return    ��0-�ɹ�/1-ʧ��
* @Create    : ����
* @Date      ��2020-4-26
* @Update    :
**********************************************************************/
int ascii_to_byte(char ascii, uint8 *byte)
{
    if('0' <= ascii && ascii <= '9')
    {
        *byte = ascii - '0';
    }
    else if('a' <= ascii && ascii <= 'f')
    {
        *byte = ascii - 'a' + 10;
    }
    else if('A' <= ascii && ascii <= 'F')
    {
        *byte = ascii - 'A' + 10;
    }
    else
    {
        return -1;
    }
    return 0;
}

/*-
***************************************************************************
*ģ���ţ�
*���ƣ���ģʽ����Ӧ������ת����BCD(10���Ƶ��ֻ���)
*���ܣ���ģʽ����Ӧ������ת����BCD
*���������Ŀ�����ݣ�*dest����Դ���ݣ�*src����Դ���ݳ��ȣ�len��
*���������ת��������ݡ�
*������ASCIIת����BCD
*���أ���
***************************************************************************
-*/
void ascii10_to_bcd(uint8 *asc, uint16 len, uint8 *bcd)
{
    uint8	i = 0, j, k;
	uint8   num = 0;
    uint8   temp = 0;

    /* ȥ�� +86 86 */
    if('+' == asc[0] && '8' == asc[1] && '6' == asc[2])
    {
        i += 3;
    }
    else if('8' == asc[0] && '6' == asc[1])
    {
        i += 2;
    }

    j = 0;
    k = 0;
    for(; i < len; i++)
    {
        if('0' <= asc[i] && '9' >= asc[i])
        {
            temp = asc[i] - '0';
        }
        else
        {
            continue;
        }
        if(0 == k)
        {
            num = (temp << 4);
            temp = 0;
            k++;
        }
        else if(1 == k)
        {
            num += temp;
            k = 0;
            bcd[j] = num;
            num = 0;
            j++;
        }
    }
    /* ���һλ��f */
	if(1 == k)
    {
        num += 0xf;
        bcd[j] = num;
    }
}

/*********************************************************************
    �������ܵ�Ԫ��GSM��̬��
    ������������ʼ�����Ͷ��ŵ�PDU�ṹ��
    ���������
        void *pDev            ���
        dest_phone_num        ����Ŀ�ĺ���
        send_buf              ��������
        send_len              �������ݳ���
        pdu_sms               ����PDU�ṹ��
        
    �������:
        ��
    ��������ֵ:
        ��
    �ڲ���������: ��д����PDU�ṹ��pdu_sms�����������ĺ��롢����Ŀ�ĺ����
                                ����д��PDU�ṹ��
********************************************************************/
void init_send_pdustr(uint8 *sms_center_s, uint8 *dest_phone_num, uint8 *send_buf, int send_len, SMS_PDU_SEND_T *pdu_sms)
{
    int i;
    int len = 0;
    int service_center_len = 0;
    int dest_phone_len = 0;
    uint8 sms_center[SMS_PDU_PHONE_LEN] = {0};
    uint8 service_center_num[SMS_PDU_PHONE_LEN];
    uint8 *start = NULL;

    memset(service_center_num, 0xff, SMS_PDU_PHONE_LEN);

    if(sms_center_s[0])
    {
        start = sms_center_s;

        if('+' == start[0])
        {
            pdu_sms->service_center_address.type = 0x91;
            start++;
            if('8' == start[0] && '6' == start[1])
            {
                start += 2;
            }
        }
        else
        {
            pdu_sms->service_center_address.type = 0xA1;
        }
        len = strlen((char*)start);
        i = 0;
        while(i < len && i < SMS_PDU_PHONE_LEN/2)
        {
            if('0' <= start[i] && start[i] <= '9')
            {
                sms_center[i] = start[i];
            }
            else
            {
                break;
            }
            i++;
        }
        
        if(i > 0)
        {
            ascii10_to_bcd(sms_center, strlen((char *)sms_center), service_center_num);
        }
    }

    /*ȷ���������ĵ绰���볤��*/
    service_center_len =0;    
    for(i = 0; i < SMS_PDU_PHONE_LEN; i++)
    {
        //printf("%02x %02x\n", service_center_num[i]&0xF0, service_center_num[i]&0x0F);
        if((service_center_num[i] & 0xf0) == 0xf0 )
        {
            break;
        }
        else
        {
            service_center_len++;
        }
        if ((service_center_num[i] & 0x0f) == 0x0f )
        {
            break;
        }
        else
        {
            service_center_len++;
        }        
    }
    
    MQTTMSG_FMT_TRACE("service_center_len:%d\n", service_center_len);
    service_center_len = service_center_len / 2 + service_center_len % 2;
    MQTTMSG_FMT_TRACE("service_center_len:%d\n", service_center_len);
    

    /*�������ĵ绰���볤��*/
    if(service_center_len > 0)
    {
        /*�������ĺ���*/
        memset(pdu_sms->service_center_address.addr, 0xFF, SMS_PDU_PHONE_LEN);
        if(0x91 == pdu_sms->service_center_address.type)
        {
            pdu_sms->service_center_address.len += service_center_len + 2;
            pdu_sms->service_center_address.addr[0] = 0x86;
            memcpy(pdu_sms->service_center_address.addr+1, service_center_num, service_center_len);
        }
        else
        {
            pdu_sms->service_center_address.len = service_center_len + 1;
            memcpy(pdu_sms->service_center_address.addr, service_center_num, service_center_len);
        }
    }
    else
    {
        pdu_sms->service_center_address.len = 0;
    }
    
    /*Э�����ݵ�Ԫ����*/
    pdu_sms->pdu_type = 0x11;
    
    /*SMS-SUBMIT�ο���Ŀ*/
    pdu_sms->message_reference = 0x00;
    
    /*ȷ�����շ��绰���볤��*/
    dest_phone_len =0;    
    for(i = 0; i < SMS_PDU_PHONE_LEN; i++)
    {
        if((dest_phone_num[i] & 0xf0) == 0xf0)
        {
            break;
        }
        else
        {
            dest_phone_len++;
        }
        if((dest_phone_num[i] & 0x0f) == 0x0f)
        {
            break;
        }
        else
        {
            dest_phone_len++;
        }          
    }    
    
    /*���շ��绰���볤��*/
    pdu_sms->destination_address.len = (uint8)dest_phone_len;    /*+2�ֽ�Ϊ86*/

    dest_phone_len = dest_phone_len/2 +dest_phone_len%2;
    
    /*���շ���������*/
//    pdu_sms->destination_address.type = 0x91;        /*���ʶ��Ÿ�ʽ +86*/
    pdu_sms->destination_address.type = 0xA1;            /*���ڶ��Ÿ�ʽ ���� +86*/    
    
    /*���շ��绰����*/
    memset(pdu_sms->destination_address.addr, 0xFF, SMS_PDU_PHONE_LEN);
//    pdu_sms->destination_address.addr[0] = 0x86;
    memcpy(pdu_sms->destination_address.addr, dest_phone_num, dest_phone_len);
    
    //putout(dest_phone_num, dest_phone_len);
    
    /*SMSC�Ժ��ַ�ʽ����SM (����FAX,��Voice��)*/    
    pdu_sms->protocol_identifier = 0x00;

    /*�û�����(UD)���뷽��*/
    pdu_sms->data_coding_scheme = 0x04;

    /*��Ϣ��SMSC�в�����Ч��ʱ��*/
    pdu_sms->validity_period[0] = 0xAA;
    
    /*�û����ݳ���*/
    pdu_sms->user_data_len = (uint8)send_len;
    
    /*�û�����*/    
    memcpy(pdu_sms->user_data, send_buf, send_len);
    
    return;
}

/*********************************************************************
    �������ܵ�Ԫ��GSM��̬��
    ������������ʾSMS_PDU_Send_str�ṹ
    ���������
        sms    ���͵�PDU�ṹ
    ���������
        ��
    ��������ֵ��
        ��
    �ڲ��������̣����SMS_PDU_Send_str�ṹ�ĸ�����Ա
********************************************************************/
void print_sms_send(SMS_PDU_SEND_T *sms)
{
    char strtemp[2048] = {0};
    uint16 len = 0;
    uint16 i = 0;
    len = sprintf(strtemp, "���Ͷ���:\nmessage center len: %02x\nmessage center type: %02x\nmessage center num: ", 
                  sms->service_center_address.len, sms->service_center_address.type);
    for(i = 0; i < SMS_PDU_PHONE_LEN; i++)
    {
        len += sprintf(strtemp + len, "%02x ", sms->service_center_address.addr[i]);
    }
    len += sprintf(strtemp + len, "\nPDUType: %02x\nmessage_reference: %02x\ndestination_address len: %02x\n"
                   "destination_address type: %02x\ndestination_address num: ", 
                   sms->pdu_type, sms->message_reference, sms->destination_address.len, sms->destination_address.type);
    for(i = 0; i < SMS_PDU_PHONE_LEN; i++)
    {
        len += sprintf(strtemp + len, "%02x ", sms->destination_address.addr[i]);
    }
    len += sprintf(strtemp + len, "\nProtocol_Identifier: %02x\nData_Coding_Scheme: %02x\nTime: ", 
                   sms->protocol_identifier, sms->data_coding_scheme);

    if (((sms->pdu_type)&0x18) == 0x18)
    {
        for(i = 0; i < 7; i++)
        {
            len += sprintf(strtemp + len, "%02x ", sms->validity_period[i]);
        }
    }
    else
    {
        len += sprintf(strtemp + len, "%02x ", sms->validity_period[0]);
    }
    len += sprintf(strtemp + len, "User Data Len: %d\nUser Data: ", sms->user_data_len);
    for(i = 0; i < sms->user_data_len; i++)
    {
        len += sprintf(strtemp + len, "%02x ", sms->user_data[i]);
    }
    
    MQTTMSG_FMT_TRACE("%s\n", strtemp);
    return;
}

/*********************************************************************
    �������ܵ�Ԫ��GSM��̬��
    ������������ʾSMS_PDU_Send_str�ṹ
    ���������
        sms    ���͵�PDU�ṹ
    ���������
        ��
    ��������ֵ��
        ��
    �ڲ��������̣����SMS_PDU_Send_str�ṹ�ĸ�����Ա
********************************************************************/
void print_sms_recv(SMS_PDU_RECV_T *sms)
{
    char strtemp[2048] = {0};
    uint16 len = 0;
    uint16 i = 0;
    len = sprintf(strtemp, "���ն���:\nmessage center len: %02x\nmessage center type: %02x\nmessage center num: ", 
                  sms->service_center_address.len, sms->service_center_address.type);
    for(i = 0; i < SMS_PDU_PHONE_LEN; i++)
    {
        len += sprintf(strtemp + len, "%02x ", sms->service_center_address.addr[i]);
    }
    len += sprintf(strtemp + len, "\nPDUType: %02x\nOriginator Address len: %02x\nOriginator Address type: %02x\n"
                   "Originator Address num: ", sms->pdu_type, sms->originator_address.len, sms->originator_address.type);
    for(i = 0; i < SMS_PDU_PHONE_LEN; i++)
    {
        len += sprintf(strtemp + len, "%02x ", sms->originator_address.addr[i]);
    }
    len += sprintf(strtemp + len, "\nProtocol_Identifier: %02x\nData_Coding_Scheme: %02x\nTime: ", 
                   sms->protocol_identifier, sms->data_coding_scheme);

    for(i = 0; i < 7; i++)
    {
        len += sprintf(strtemp + len, "%02x ", sms->service_time_stamp[i]);
    }
    len += sprintf(strtemp + len, "\nUser Data Len: %02x\nUser Data: ", sms->user_data_len);

    for(i = 0; i < sms->user_data_len; i++)
    {
        len += sprintf(strtemp + len, "%02x ", sms->user_data[i]);
    }
        
    MQTTMSG_FMT_DEBUG("%s\n", strtemp);
    return;
}

/*********************************************************************
�������ܵ�Ԫ��GSM��̬��
������������16������ת��������Ascii��
���������
bin        16������
high    Ascii���λ
low        Ascii���λ
�������:
��
��������ֵ:
��
�ڲ���������:��16�������������Ascii��
********************************************************************/
void wireless_binto_Ascii(uint8 bin, uint8 *high, uint8 *low)
{
    *high = bin>>4;
    *low = bin&0x0f;

    if ( *high <= 0x09 )
        *high += '0';
    else
    {
        *high += 'A' - 0x0A;
    }

    if ( *low <= 0x09 )
        *low += '0';
    else
        *low += 'A' - 0x0A;
    return;
}

/*********************************************************************
�������ܵ�Ԫ��GSM��̬��
����������������Ascii��ת����һ��16������
���������
high    ת��Ϊ16���Ƹ�λ
low        ת��Ϊ16���Ƶ�λ
���������
��
��������ֵ��
ת�����16������
�ڲ���������: ��highת���ɽ��Ƹ�λ, lowת����16���Ƶ�λ
********************************************************************/
uint8 wireless_Asciit_tobin(uint8 high, uint8 low)
{
    if( (low>='A')&&(low<='F') )
    {
        low = low - 'A' + 0x0a;
    }
    else if( (low>='a')&&(low<='f') )
    {
        low = low - 'a' + 0x0a;
    }
    else if( (low >='0')&&(low<='9') )
    {
        low = low - '0' + 0x00;
    }
    else
        return 0xFF;

    if( (high>='A')&&(high<='F') )
    {
        high = high - 'A' + 0x0a;
    }
    else if( (high>='a')&&(high<='f') )
    {
        high = high - 'a' + 0x0a;
    }
    else if( (high>='0')&&(high<='9') )
    {
        high = high - '0' + 0x00;
    }
    else
        return 0xFF;
    //      printf("low = %02x,    high = %02x\n", low, high);
    return ((0x0f)&low) | (high<<4);
}


/*********************************************************************
    �������ܵ�Ԫ��GSM��̬��
    ������������pdu�������ݰ�
    ���������
        sms_send    pdu�ṹ
        pdu_sms        �����õ�pdu���ݰ�
        pcgms_len    pdu���ݰ������峤��
        
    �������:
        ��
    ��������ֵ:
        pdu���ݰ�����
        
    �ڲ���������: ��pdu���ݽṹ�������pdu���ݰ�
********************************************************************/
int ping_send_pdu(SMS_PDU_SEND_T sms_send, uint8 *pdu_sms, int *pcgms_len)
{
    int i;
    int pdu_sms_len;
    int cgms_len;
    int temp;
    
    pdu_sms_len = 0;
    cgms_len = 0;
    temp = 0;
    
    /*�������ĵ�ַ*/
    wireless_binto_Ascii(sms_send.service_center_address.len, &pdu_sms[0], &pdu_sms[1]);
    pdu_sms_len += 2;
    pdu_sms += 2;
    
    if(sms_send.service_center_address.len > 0)
    {
        wireless_binto_Ascii(sms_send.service_center_address.type, &pdu_sms[0], &pdu_sms[1]);
        pdu_sms_len += 2;
        pdu_sms += 2;
        
        for (i=0; i<( (unsigned char)sms_send.service_center_address.len - 1 ); i++)
        {
            wireless_binto_Ascii(sms_send.service_center_address.addr[i], &pdu_sms[1], &pdu_sms[0]);
            pdu_sms_len += 2;
            pdu_sms += 2;            
        }    
    }
    /*Э�����ݵ�Ԫ����*/
    wireless_binto_Ascii(sms_send.pdu_type, &pdu_sms[0], &pdu_sms[1]);
    pdu_sms_len += 2;
    cgms_len += 1;
    pdu_sms += 2;    
    
    /*���гɹ���SMS-SUBMIT�ο���Ŀ(0..255)*/    
    wireless_binto_Ascii(sms_send.message_reference, &pdu_sms[0], &pdu_sms[1]);
    pdu_sms_len += 2;
    cgms_len += 1;
    pdu_sms += 2;
    
    /*���շ�SME�ĵ�ַ*/
    wireless_binto_Ascii(sms_send.destination_address.len, &pdu_sms[0], &pdu_sms[1]);
    pdu_sms_len += 2;
    cgms_len += 1;
    pdu_sms += 2;
    
    wireless_binto_Ascii(sms_send.destination_address.type, &pdu_sms[0], &pdu_sms[1]);
    cgms_len += 1;
    pdu_sms_len += 2;
    pdu_sms += 2;
    
    temp = ((unsigned char)sms_send.destination_address.len)%2 + ((unsigned char)sms_send.destination_address.len)/2;
//printf("temp = %d\n", temp);
    
    for (i=0; i<temp; i++)
    {
        wireless_binto_Ascii(sms_send.destination_address.addr[i], &pdu_sms[1], &pdu_sms[0]);
        cgms_len += 1;
        pdu_sms_len += 2;
        pdu_sms += 2;            
    }    
    
    /*������ʾSMSC�Ժ��ַ�ʽ����SM (����FAX,��Voice��)*/
    wireless_binto_Ascii(sms_send.protocol_identifier, &pdu_sms[0], &pdu_sms[1]);
    cgms_len += 1;
    pdu_sms_len += 2;
    pdu_sms += 2;                

    /*������ʾ�û�����(UD)���뷽��*/
    wireless_binto_Ascii(sms_send.data_coding_scheme, &pdu_sms[0], &pdu_sms[1]);
    cgms_len += 1;
    pdu_sms_len += 2;
    pdu_sms += 2;
    
    /*��Ϣ��SMSC�в�����Ч��ʱ��*/
    if ( ( (sms_send.pdu_type)&0x18 ) == 0x18 )    /* PDU_Type = 11 �þ���ֵ��ʾ��Чʱ��,PDU_Type = 10, */
                                                /* �����ֵ��ʾ��Чʱ�� */
    {
        for(i=0; i<7; i++)
        {
            wireless_binto_Ascii(sms_send.validity_period[i], &pdu_sms[0], &pdu_sms[1]);
            pdu_sms_len += 2;
            cgms_len += 1;
            pdu_sms += 2;
        }
    }
    else
    {
        wireless_binto_Ascii(sms_send.validity_period[0], &pdu_sms[0], &pdu_sms[1]);
        cgms_len += 1;
        pdu_sms_len += 2;
        pdu_sms += 2;        
    }
    /*�û����ݳ���*/
    wireless_binto_Ascii(sms_send.user_data_len, &pdu_sms[0], &pdu_sms[1]);
    cgms_len += 1;
    pdu_sms_len += 2;
    pdu_sms += 2;    
    
    /*�û�����*/
//    memcpy(pdu_sms, sms_send.user_data, (unsigned char)sms_send.user_data_len);
//    cgms_len += sms_send.user_data_len;

    for (i=0; i<(unsigned char)sms_send.user_data_len; i++)
    {
        wireless_binto_Ascii(sms_send.user_data[i], &pdu_sms[0], &pdu_sms[1]);
        cgms_len += 1;
        pdu_sms_len += 2;
        pdu_sms += 2;            
    }        
    pdu_sms[0] = 0x00;    
    *pcgms_len = cgms_len;
    
    return pdu_sms_len;
}

/*********************************************************************
�������ܵ�Ԫ��GSM��̬��
��������������PDU�������ݰ�
���������
    fd                ���ڲ�����
    pdu_data    pdu���ݰ�
    pdu_len        pdu���ݰ�����
    len_cgms    pdu���ݰ������峤��
    
�������:
    ��
��������ֵ:
    0    ���ͳɹ�
    -1    ����ʧ��
    
�ڲ���������: ������õ�pdu���ݰ��ö��ŷ��ͳ�ȥ
              ���ŷ������̣�AT+CMGS=len 0x0D ���ݰ� 0x1A    
********************************************************************/
int send_pduSms(uint8 ifnum, void *pDev, uint8 *pdu_data, int pdu_len, int len_cgms)
{
    uint8 *pcheck = NULL;
    int i = 0;
    int recv_len = 0;
    char send_buf[300] = {0};
    char recv_buf[300] = {0};
    int ret = 0;
    
    /* �ȼ���Ƿ����pduģʽ */
    ret = at_get_nolog(ifnum, pDev, "AT+CMGF?\r\n", recv_buf, 300);
    MQTTMSG_FMT_DEBUG("�豸%d ������ģʽ%s, ����AT+CMGF?\n����: %s\n", AT_DEV_NUM(ifnum), 0 == ret ? "" : "ʧ��", 
                      recv_buf);
    if(0 != ret || NULL == strstr(recv_buf, "+CMGF: 0"))
    {
        memset(recv_buf, 0, 300);
        ret = at_get_nolog(ifnum, pDev, "AT+CMGF=0\r\n", recv_buf, 300);
        MQTTMSG_FMT_DEBUG("�豸%d ���ö���ģʽ%s, ����AT+CMGF=0\n����: %s\n", AT_DEV_NUM(ifnum), 0 == ret ? "" : "ʧ��", 
                          recv_buf);
        if(0 != ret || NULL == strstr(recv_buf, "OK"))
        {
            MQTTMSG_FMT_DEBUG("%s%d���ö���PDUģʽʧ��\n", AT_DEV_NAMENUM(ifnum));
            return -1;
        }
    }

    /* ����TE�ַ��� */
    memset(recv_buf, 0, 300);
    ret = at_get_nolog(ifnum, pDev, "AT+CSCS=\"UCS2\"\r\n", recv_buf, 300);
    MQTTMSG_FMT_DEBUG("�豸%d ����TE�ַ���%s, ����AT+CSCS=\"UCS2\"\n����: %s\n", AT_DEV_NUM(ifnum), 0 == ret ? "" : "ʧ��", 
                      recv_buf);
    if(0 != ret || NULL == strstr(recv_buf, "OK"))
    {
        MQTTMSG_FMT_DEBUG("%s%d: ����UCS2�ַ���ʧ��\n", AT_DEV_NAMENUM(ifnum));
        return -1;
    }
    memset(recv_buf, 0, 300);

    /*����AT+CMGS=XX + 0D(&H)*/
    sprintf((char *)send_buf, "AT+CMGS=%d\x0d", len_cgms);
    wireless_uart_send(pDev, (uint8 *)send_buf, strlen(send_buf));

    /*��ģ��Ӧ��'>'��,��ʼ�������ݰ�*/
    while( (pcheck == NULL) && (i < 10) )
    {
        usleep(100000);
        recv_len = wireless_uart_recv(pDev, (uint8 *)recv_buf, sizeof(recv_buf));
        if ( (recv_len > 0) && (recv_len < sizeof(recv_buf)) )
            pcheck = memchr(recv_buf, '>', recv_len);
        i++;    
    }
    
    if (pcheck == NULL)
    {
        MQTTMSG_FMT_DEBUG("%s%d: ���Ͷ���ʧ��, ����%d\n", AT_DEV_NAMENUM(ifnum), len_cgms);
        return -1;
    }
    else
    {
        pcheck = NULL;
    }

    MQTTMSG_FMT_DEBUG("%s%d\n===============================\n"
                      "����: %s\n����: %s\n===============================\n",
                      AT_DEV_NAMENUM(ifnum), send_buf, recv_buf);

    /*����pdu���ݰ�*/
    wireless_uart_send(pDev, (uint8 *)pdu_data, pdu_len);
    
    /*����1A����*/
    send_buf[0] = 0x1A;
    wireless_uart_send(pDev, (uint8 *)send_buf, 1);    
    
    i = 0;
    pcheck = NULL;
    memset(recv_buf, 0, sizeof(recv_buf));
    recv_len = 0;
    
    /*�������Ƿ��ͳɹ�*/
    while( (pcheck == NULL) && (i < 30) )
    {
        usleep(400000);
        recv_len = wireless_uart_recv(pDev, (uint8 *)recv_buf+recv_len, sizeof(recv_buf));
        if ( recv_len > 0 )
            pcheck = (uint8 *)strstr((char *)recv_buf, "OK");
        i++;    
    }

    MQTTMSG_FMT_DEBUG("%s%d����\n===============================\n"
                      "����: %s\n����: %s\n===============================\n", 
                      AT_DEV_NAMENUM(ifnum), pdu_data, recv_buf);

    if (pcheck == NULL)
    {
        MQTTMSG_FMT_DEBUG("%s%d���Ͷ���ʧ��\n", AT_DEV_NAMENUM(ifnum));
        return -1;
    }
    else
    {
        pcheck = NULL;
        return 0;
    }
}

/**********************************************************************
* @name      : send_textSms
* @brief     ������text��ʽ����
* @param[in] ��void *pDev    �豸���
               uint8 *data   ����
               uint16 len    ����
               uint8 *dest   Ŀ���ַ
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-4-22
* @Update    :
**********************************************************************/
int send_textSms(uint8 ifnum, void *pDev, uint8 *data, uint16 len, uint8 *dest)
{
    int ret = 0;
    uint8 *pcheck = NULL;
    int i;
    int newlen = 0;
    int recv_len;
    char at_get_buf[GSM_LENGTH] = {0};/*AT���������*/
    char send_ascii[GSM_LENGTH + 1] = {0}; /*�����Ķ���*/

    /* TEXTģʽ���͵���ASCII�ַ��� */
    /* ���ڷ��Ͷ�����������ֽڴ�, text��֧�֣���Ҫת�����ֽ��� */
    newlen = 0;
    for(i = 0; i < len; i++)
    {
        send_ascii[newlen] = data[i];
        newlen++;
        if(newlen > GSM_LENGTH)
        {
            MQTTMSG_FMT_DEBUG("%s%d���Ͷ��ų�������\n", AT_DEV_NAMENUM(ifnum));
            return -1;
        }
    }

    /* �ȼ���Ƿ����textģʽ */
    ret = at_get_nolog(ifnum, pDev, "AT+CMGF?\r\n", at_get_buf, GSM_LENGTH);
    MQTTMSG_FMT_DEBUG("�豸%d ������ģʽ%s, ����AT+CMGF?\n����: %s\n", AT_DEV_NUM(ifnum), 0 == ret ? "" : "ʧ��", 
                      at_get_buf);
    if(0 != ret || NULL == strstr(at_get_buf, "+CMGF: 1"))
    {
        memset(at_get_buf, 0, GSM_LENGTH);
        ret = at_get_nolog(ifnum, pDev, "AT+CMGF=1\r\n", at_get_buf, GSM_LENGTH);
        MQTTMSG_FMT_DEBUG("�豸%d ���ö���ģʽ%s, ����AT+CMGF=1\n����: %s\n", AT_DEV_NUM(ifnum), 0 == ret ? "" : "ʧ��", 
                          at_get_buf);
        if(0 != ret || NULL == strstr(at_get_buf, "OK"))
        {
            MQTTMSG_FMT_DEBUG("%s%d���ö���textģʽʧ��\n", AT_DEV_NAMENUM(ifnum));
            return -2;
        }
    }

    /* ����TE�ַ��� */
    memset(at_get_buf, 0, GSM_LENGTH);
    ret = at_get_nolog(ifnum, pDev, "AT+CSCS=\"GSM\"\r\n", at_get_buf, GSM_LENGTH);
    MQTTMSG_FMT_DEBUG("�豸%d ����TE�ַ���%s, ����AT+CSCS=\"GSM\"\n����: %s\n", AT_DEV_NUM(ifnum), 0 == ret ? "" : "ʧ��", 
                      at_get_buf);
    if(0 != ret || NULL == strstr(at_get_buf, "OK"))
    {
        MQTTMSG_FMT_DEBUG("%s%d: ����GSM�ַ���ʧ��\n", AT_DEV_NAMENUM(ifnum));
        return -3;
    }

    memset(at_get_buf, 0, GSM_LENGTH);
    sprintf(at_get_buf, "AT+CMGS=\"%s\"\r", dest);
    wireless_uart_send(pDev, (uint8*)at_get_buf, strlen((char *)at_get_buf));
    /*��ģ��Ӧ��'>'��,��ʼ�������ݰ�*/
    memset(at_get_buf, 0, sizeof(at_get_buf));
    i = 0;
    while(i < 20)
    {
        usleep(100000);
        recv_len = wireless_uart_recv(pDev, (uint8*)at_get_buf, sizeof(at_get_buf));
        if (recv_len)
        {
            pcheck = (uint8 *)strstr((char *)at_get_buf, ">");
            break;
        }
        i++;  
    }
    MQTTMSG_FMT_DEBUG("%s%d\n===============================\n"
                      "����: AT+CMGS=\"%s\"\n����: %s\n===============================\n", 
                      AT_DEV_NAMENUM(ifnum), dest, at_get_buf);
    if (pcheck == NULL)
    {
        MQTTMSG_FMT_DEBUG("%s%d���Ͷ��Ÿ�%sʧ��\n", AT_DEV_NAMENUM(ifnum), dest);
        return -4;
    }

    /*����text���ݰ�*/
    if(0 != wireless_uart_send(pDev, (uint8 *)send_ascii, newlen))
    {
        MQTTMSG_FMT_DEBUG("%s%d���Ͷ��Ÿ�%sʧ��\n", AT_DEV_NAMENUM(ifnum), dest);
        return -5;
    }
    
    /*����1A����*/
    at_get_buf[0] = 0x1A;
    if(0 != wireless_uart_send(pDev, (uint8*)at_get_buf, 1))
    {
        MQTTMSG_FMT_DEBUG("%s%d����Ctrl+zʧ��\n", AT_DEV_NAMENUM(ifnum));
        return -6;
    }

    i = 0;
    pcheck = NULL;
    memset(at_get_buf, 0, sizeof(at_get_buf));
    recv_len = 0;
    
    /*�������Ƿ��ͳɹ�*/
    while( (pcheck == NULL) && (i < 30) )
    {
        usleep(400000);
        recv_len = wireless_uart_recv(pDev, (uint8*)at_get_buf+recv_len, sizeof(at_get_buf)-recv_len);
        if ( recv_len > 0 )
            pcheck = (uint8 *)strstr((char *)at_get_buf, "OK");
        i++;    
    }
    MQTTMSG_FMT_DEBUG("%s%d\n===============================\n"
                      "����: %s\n����: %s\n===============================\n", 
                      AT_DEV_NAMENUM(ifnum), send_ascii, at_get_buf);

    if (pcheck == NULL)
    {
        MQTTMSG_FMT_DEBUG("%s%d���Ͷ���ʧ��\n", AT_DEV_NAMENUM(ifnum));
        return -7;
    }
    else
    {
        pcheck = NULL;
        return 0;
    }
}

/*********************************************************************
�������ܵ�Ԫ��GSM��̬��
������������ģ���ж��ŵ�����
���������
-       void *pDev     ���ھ��

�������:
-       ��

��������ֵ:
-       >= 0    ��������
-       -1      ������

�ڲ���������: 
-       ��������AT+CPMS="ME","ME","ME(MC35ΪMT)",����������
-       ������Ӧ��+CPMS: 0,25,0,25,0,85
-       ����M23AӦ��+CPMS: 0,0,0,0,1,60
********************************************************************/
int get_message_num(uint8 ifnum, void *pDev, int *maxnum)
{
    int at_status;
    int message_num = 0;
    int message_max = 0;
    uint8 *pcpms;
    char   at_get_buf[GSM_LENGTH] = {0};
    uint8 *pring = NULL;

    /*����AT+CPMS="SM","SM","SM", ��ȡģ���ж��ŵĴ洢״��*/
    at_status = at_get_nolog(ifnum, pDev, "AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n", at_get_buf, sizeof(at_get_buf));
    MQTTMSG_FMT_DEBUG("�豸%d ��ȡ��������%s, ����AT+CPMS=\"SM\",\"SM\",\"SM\"\n����: %s\n", 
                      AT_DEV_NUM(ifnum), 0 == at_status ? "" : "ʧ��", at_get_buf);
    /*ģ���쳣,û��Ӧ��AT����*/
    if (0 == at_status)
    {        
        /*�ڻ��ģʽ��, ring ����gprs*/
        pring = (uint8 *)strstr((char *)at_get_buf, "RING");
        if (pring != NULL)
        {
            at_check(ifnum, pDev, "ATH\r\n", "OK");
            return -2;
        }

        pcpms = (uint8 *)strstr((char *)at_get_buf, "+CPMS:");    /*��AT����Ӧ���в���+CPMS:*/

        if ( (pcpms != NULL)&&(strlen((char *)pcpms) > 11) )
        {
            if (pcpms[8] == ',')
            {
                message_num = *(pcpms+7) - '0';
                message_max = (*(pcpms+9) - '0')*10 + *(pcpms+10) - '0';
            }
            else if (pcpms[9] == ',')
            {
                message_num = (*(pcpms+7) - '0')*10 + *(pcpms+8) - '0';
                message_max = (*(pcpms+10) - '0')*10 + *(pcpms+11) - '0';
            }
            else
                return 0;
        }
        *maxnum = message_max;
        return message_num;
    }
    else
    {    
        MQTTMSG_FMT_TRACE("GetMessageNum return -1\n");
        return -1;
    }
}

/*********************************************************************
�������ܵ�Ԫ��GSM��̬��
�����������������յ���pdu���ݰ�
���������
    message    ���յ�pdu�������ݰ�
    message_len    ���ų���
    sms            �������ݽṹָ��
    
�������:
    ��
��������ֵ:
    0  �ɹ�
    -1 ʧ��
    
�ڲ���������:�����Ű�pdu��ʽ���з�������ȡ�������ĺ��롢�����ߵ绰����Ȳ���
********************************************************************/
int parse_sms(uint8 *message, int message_len, SMS_PDU_RECV_T *sms)
{
    int temp;
    uint8 tempchar = 0;
    int i;
    uint8 j;
    uint8 phone[SMS_PDU_PHONE_LEN] = {0};

    //printf("in ParseSms\n");    
    sms->service_center_address.len = wireless_Asciit_tobin(message[0], message[1]);    /*ȡ�������ĺ��볤��*/
    //printf("Tel_len: %d\n",(int)sms->service_center_address.len);

    message += 2;
    message_len -= 2;

    /*�������ĺ��볤����Ϊ���������20���򰴳�����*/    
    if (sms->service_center_address.len <= 20 && sms->service_center_address.len > 0)
    {
        sms->service_center_address.type = wireless_Asciit_tobin(message[0], message[1]);
        message += 2;
        message_len -= 2;
        if (sms->service_center_address.type == 0x91)
        {
            /* ����ǰ���86 */
            message += 2;
            message_len -= 2;                    
            /*��ȡ�������ĺ���*/
            for (i=0; i<(sms->service_center_address.len) - 2; i++)
            {
                sms->service_center_address.addr[i] = wireless_Asciit_tobin(message[1], message[0]);
                message += 2;
                message_len -=2;
            }
            /*���벻��20λ������Ĳ�'F'*/
            while(i < SMS_PDU_PHONE_LEN)
            {
                sms->service_center_address.addr[i]    = 0xFF;
                i++;
            }
        }
        else
        {
            /*��ȡ�������ĺ���*/
            for (i=0; i<(sms->service_center_address.len) - 1; i++)
            {
                sms->service_center_address.addr[i] = wireless_Asciit_tobin(message[1], message[0]);
                message += 2;
                message_len -=2;
            }
            /*���벻��20λ������Ĳ�'F'*/
            while(i < SMS_PDU_PHONE_LEN)
            {
                sms->service_center_address.addr[i]    = 0xFF;
                i++;
            }                    
        }
    }
    else if (sms->service_center_address.len > 20)
    {
        return -1;
    }
    /*len = 0 û�ж������ĺ�����Ϊ��ȷ*/

    //printf("PDU_Addr len = %02x, type = %02x\n", sms->service_center_address.len, sms->service_center_address.type);    
    //printf("message center number:\n");
    //putout(sms->service_center_address.addr, SMS_PDU_PHONE_LEN);
    //printf("message:\n");
    //putout(message, message_len);


    /*Э�����ݵ�Ԫ���� PDU_type*/
    sms->pdu_type = wireless_Asciit_tobin(message[0], message[1]);
    message += 2;
    message_len -= 2;

    /*���ͷ��绰����*/
    sms->originator_address.len = wireless_Asciit_tobin(message[0], message[1]);
    message += 2;
    message_len -= 2;

    sms->originator_address.type = wireless_Asciit_tobin(message[0], message[1]);
    message += 2;
    message_len -= 2;

    temp = (sms->originator_address.len/2)+(sms->originator_address.len%2);
    //printf("temp = %d\n", temp);
    for(i = 0; i < temp; i++)
    {
        sms->originator_address.addr[i] = wireless_Asciit_tobin(message[1], message[0]);
        message += 2;
        message_len -= 2;
    }

    while(i < SMS_PDU_PHONE_LEN)
    {
        sms->originator_address.addr[i] = 0xFF;
        i++;
    }

    /*������ʾSMSC�Ժ��ַ�ʽ����SM (����FAX,��Voice��)*/
    sms->protocol_identifier = wireless_Asciit_tobin(message[0], message[1]);
    message += 2;
    message_len -= 2;

    /*������ʾ�û�����(UD)���뷽��*/
    sms->data_coding_scheme = wireless_Asciit_tobin(message[0], message[1]);
    message += 2;
    message_len -= 2;

    /*�յ���Ϣ��ʱ��*/
    for(i = 0; i < 7; i++)
    {
        sms->service_time_stamp[i] = wireless_Asciit_tobin(message[1], message[0]);
        message += 2;
        message_len -= 2;            
    }

    /*�û����ݳ���*/
    sms->user_data_len = wireless_Asciit_tobin(message[0], message[1]);
    message += 2;
    message_len -= 2;  

    /*�û�����*/
    for(i = 0; i < sms->user_data_len; i++)
    {
        sms->user_data[i] = wireless_Asciit_tobin(message[0], message[1]);
        message += 2;
        message_len -= 2;    
    }

    /* ����������ת����ASCII */
    if(sms->service_center_address.len > 0)
    {
        j = 0;
        for(i = 0; i < SMS_PDU_PHONE_LEN/2; i++)
        {
            if(0xff == sms->service_center_address.addr[i])
            {
                break;
            }
            /* ��λ��ǰ ��λ�ں� */
            tempchar = ((sms->service_center_address.addr[i] >> 4) & 0x0f);
            if(0xf != tempchar)
            {
                phone[j++] = '0' + tempchar;
            }
            tempchar = (sms->service_center_address.addr[i] & 0x0f);
            if(0xf != tempchar)
            {
                phone[j++] = '0' + tempchar;
            }
        }
        sms->service_center_address.len = j;
        memcpy(sms->service_center_address.addr, phone, SMS_PDU_PHONE_LEN);
    }
    else
    {
        memset(sms->service_center_address.addr, 0, SMS_PDU_PHONE_LEN);
    }

    if(sms->originator_address.len > 0)
    {
        j = 0;
        memset(phone, 0, SMS_PDU_PHONE_LEN);
        if(0x91 == sms->originator_address.type)
        {
            phone[0] = '+';
            j++;
        }
        for(i = 0; i < SMS_PDU_PHONE_LEN/2; i++)
        {
            if(0xff == sms->originator_address.addr[i])
            {
                break;
            }
            /* ��λ��ǰ ��λ�ں� */
            tempchar = ((sms->originator_address.addr[i] >> 4) & 0x0f);
            if(0xf != tempchar)
            {
                phone[j++] = '0' + tempchar;
            }
            tempchar = (sms->originator_address.addr[i] & 0x0f);
            if(0xf != tempchar)
            {
                phone[j++] = '0' + tempchar;
            }
        }
        sms->originator_address.len = j;
        memcpy(sms->originator_address.addr, phone, SMS_PDU_PHONE_LEN);
    }
    else 
    {
        memset(sms->originator_address.addr, 0, SMS_PDU_PHONE_LEN);
    }
    return 0;
}

/*********************************************************************
    �������ܵ�Ԫ��GSM��̬��
    ����������ɾ����index������
    ���������
        pDev    ���ڲ�����
        index    ���ű��
    ���������
        ��
    ��������ֵ��
        ��
    �ڲ��������̣����ڷ���AT+CMGD=index,ɾ����index������
********************************************************************/
void del_message(uint8 ifnum, void *pDev, int index)
{
    char  at_send[50] = {0};
    char  at_recv[AT_ANSWER_LENGTH] = {0};
    uint32 offset = 0;
    int   recv_len;
    int   ret = 0;
    uint8 i = 0;
    if (index < 0)
    {
        return;
    }
        
    sprintf(at_send, "AT+CMGD=%d\r\n", index);
    ret = wireless_uart_send(pDev, (uint8 *)at_send, strlen(at_send));    /*����AT����*/
    if(0 == ret)
    {
        ret = -1;
        while(i++ <= 50)
        {
            usleep(10000);
            recv_len = wireless_uart_recv(pDev, (uint8 *)at_recv + offset, AT_ANSWER_LENGTH - offset);
            if(recv_len > 0)
            {
                offset += recv_len;
                if(NULL != strstr(at_recv, "OK"))
                {
                     ret = 0;
                     break;
                }
                if(NULL != strstr(at_recv, "ERROR"))
                {
                     ret = 1;
                     break;
                }
            }
        }
    }
    MQTTMSG_FMT_DEBUG("ɾ����%d������%s, ��������\n%s\n", index, 0 == ret ? "�ɹ�" : "ʧ��", at_recv);
}

/*********************************************************************
�������ܵ�Ԫ��GSM��̬��
�����������ж��ŷ��ͷ��绰�����Ƿ�����̨�绰����һ��
���������
    pdu_addr    ���ͷ�pdu��ַ    
    phone_num    ��̨�绰����
    phone_len    ��̨�绰���볤��
    
�������:
    ��
��������ֵ:
    0   ������ͬ
    -1    ���벻��ͬ
    
�ڲ���������: 
********************************************************************/
int check_telephone(struct PDU_Addr_str *pdu_addr, uint8 *phone_num, int phone_len)
{
    int flags;
    int i;
    flags = 0;
    
    if (pdu_addr->addr[0] == 0x86)
    {
        memmove(pdu_addr->addr, (pdu_addr->addr)+1, phone_len-1);
        pdu_addr->addr[phone_len-1] = 0xff;
    }
    for (i=0; i<phone_len; i++)
    {
        if ((unsigned char)pdu_addr->addr[i] == (unsigned char)phone_num[i])
        {
            flags ++;
        }
    }
    if (flags == phone_len )
        return 0;
    else
        return -1;
}

/**********************************************************************
* @name      : get_text_message
* @brief     ������Э��ջAT_ST ״̬������
* @param[in] ��char *meaasge  ������Ϣ
* @param[out]��SMS_PDU_RECV_T *pRecvSms 
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-4-23
* @Update    : 2020-10-13
**********************************************************************/
int get_text_message(char *meaasge, SMS_PDU_RECV_T *pRecvSms)
{
    uint16 i = 0;
    uint16 len = 0;
    char *start = NULL;
    uint16 newlen = 0;

    if(NULL == meaasge || NULL == pRecvSms)
    {
        return -1;
    }

    /* ��ȡ�ֻ��� */
    start = strstr(meaasge, "\",\"");
    if(NULL == start)
    {
        return -2;
    }
    start += 3;

    len = strlen(start);
    memset(pRecvSms->originator_address.addr, 0, SMS_PDU_PHONE_LEN);
    pRecvSms->originator_address.len = 0;
    for(i = 0; i < len && i < SMS_PDU_PHONE_LEN; i++)
    {
        if('\"' == start[i] || ',' == start[i])
        {
            break;
        }
        pRecvSms->originator_address.addr[i] = start[i];
        pRecvSms->originator_address.len++;
    }
    
    if(i >= SMS_PDU_PHONE_LEN)
    {
        return -3;
    }
    
    /* ��ȡ�������ݿ�ʼ */
    while(i + 1 < len && 0x0D != start[i] && 0x0A != start[i+1])
    {
        i++;
    }
    if(i + 1 == len)
    {
        return -4;
    }
    start += i + 2;
    len = strlen(start);
    /* ʵ��ascii������Ҫת�����ֽ�����ʽ */
    for(i = 0; i < len; i++)
    {
        if(0x0D == start[i] && 0x0A == start[i+1])
        {
            break;
        }
        pRecvSms->user_data[newlen] = start[i];
        newlen++;
    }

    pRecvSms->user_data_len = newlen;
    return 0;
}

/**********************************************************************
* @name      : read_message
* @brief     �������ŶԶ��ŵķ�������ݡ���Чʱ�������֤;��ȡ���������Ķ��ŵ�
               ���ݣ��������ɾ������
* @param[in] ��uint8 ifnum �豸���
               void *pDev  �豸���
               int maxnum  ��������
               WIRELESS_HANDLE_T *ptHandle �߳̾��
* @param[out]��SMS_PDU_RECV_T *pRecvSms    ���ն���
* @return    ��0-�ɹ�/ʧ��
* @Create    : ����
* @Date      ��2020-4-24
* @Update    :
**********************************************************************/
int read_message(uint8 ifnum, void *pDev, int maxnum, SMS_PDU_RECV_T *pRecvSms, WIRELESS_HANDLE_T *ptHandle)
{
    int len;
    int i;
    int message_index = 0;    /*���ű��*/
    int message_frame_len = 0;/*����֡����*/
    int at_status;
    SMS_PDU_RECV_T recv_sms;
    char  at_get_buf[GSM_LENGTH] = {0};
    char  at_send_buf[50] = {0};
    uint8 *pcms = NULL;
    uint8 *pcmgr = NULL;
    uint8 *ppick = NULL;
    char  *pcpms = NULL;

    memset(&recv_sms, 0, sizeof(SMS_PDU_RECV_T));
    
/*�ӵ�һ����ʼ�����ţ�ֱ������һ������Ϊֹ
����AT����: AT+CMGR=n (n = 1,2,3...)
Ӧ���ʽ:
AT+CMGR=n
+CMGR: 1,,34
0891683108200505F0040D91683106154617F50000701090712450230F61F1985C369FD169F59ADD76BF01

OK
*/
    while(message_index <= maxnum)  /*MAX_MESSAGE_INDEX ģ���п��Դ洢������������*/
    {
        memset(at_get_buf, 0, sizeof(at_get_buf));
        memset(at_send_buf, 0, sizeof(at_send_buf));
        sprintf(at_send_buf, "AT+CMGR=%d\r\n", message_index);
        at_status = at_get_nolog(ifnum, pDev, at_send_buf, at_get_buf, GSM_LENGTH);
        if (0 == at_status)
        {
            pcpms = strstr(at_get_buf, "+CMGR:");
            if(NULL == pcpms)
            {
                del_message(ifnum, pDev, message_index); /*ɾ������*/
                message_index++;
                continue;
            }
            
            MQTTMSG_FMT_DEBUG("%s%d�������%d, ����:\n%s\n", AT_DEV_NAMENUM(ifnum), message_index, at_get_buf);
            MQTTMSG_BUF_DEBUG(at_get_buf, strlen(at_get_buf), "ʮ����������\n");

            pcpms = strstr(at_get_buf, "+CMGR: \"REC");
            /* �ı���ʽ TEXT */
            if(NULL != pcpms)
            {
                MQTTMSG_FMT_DEBUG("Text SMS\n");
                if(0 == get_text_message(pcpms, &recv_sms))
                {
                    /*��ȡ����*/
                    memcpy(pRecvSms, &recv_sms, sizeof(SMS_PDU_RECV_T));
                    if(recv_sms.user_data_len > 0)
                    {
                        del_message(ifnum, pDev, message_index); /*ɾ������*/
                        return (int)recv_sms.user_data_len;
                    }
                }
                del_message(ifnum, pDev, message_index); /*ɾ������*/
                message_index++;
                continue;
            }
            
            /*����λ���Ƿ��ж��Ŵ��*/
            pcms = (uint8 *)strstr((char *)at_get_buf, ",,");
            if (pcms != NULL)
            {
                if (pcms[2] != '0')
                {
                    pcms = NULL;
                }
            }
            
            if (pcms == NULL)
            {
                MQTTMSG_FMT_DEBUG("PDU SMS\n");
                pcmgr = (uint8 *)strstr((char *)at_get_buf, "+CMGR");
                if (pcmgr != NULL)
                {        
                    /*ȡ���ų���*/
                    MQTTMSG_FMT_TRACE("Read The message!\n");

                    i = 0;
                    len = strlen((char *)pcmgr);
                    ppick = pcmgr;

                    while(i + 1 < len && ppick[i] != 0x0d && ppick[i+1] != 0x0a)
                        i++;
                        
                    if (i < len)
                    {
                        ppick += i+2;        /*�õ������Ķ���֡����������ͷ*/
                        message_frame_len = strlen((char *)ppick);
                        
                        if (0 != parse_sms(ppick, message_frame_len, &recv_sms))
                        {
                            del_message(ifnum, pDev, message_index); /*ɾ������*/
                            message_index++;
                            continue;
                        }
                        print_sms_recv(&recv_sms);
                        
                        /*��ȡ����*/
                        memcpy(pRecvSms, &recv_sms, sizeof(SMS_PDU_RECV_T));
                        if(recv_sms.user_data_len > 0)
                        {
                            del_message(ifnum, pDev, message_index); /*ɾ������*/
                            return (int)recv_sms.user_data_len;
                        }
                    }
                }
            }
            del_message(ifnum, pDev, message_index); /*ɾ������*/
        }
        else
        {
            return -1;
        }
        message_index++;
    }
    
    /*�������ж��ŵ����������, ɾ�����ж���*/
#if 0
    if (message_index > MAX_MESSAGE_INDEX)
    {
        for(i = 0; i < MAX_MESSAGE_INDEX; i++)
        {
            del_message(ifnum, pDev, i);
        }
    }
    sleep(1);
#endif
    return 0;
}

/*04����֡ ff 68 39 00 39 00 68 4A 11 34 01 00 02 05 71 00 00 20 04 98 0A CE 16*/
/*�㽭�漤��֡ 68 99 01 01 00 5E 13 68-0F 09 00 00 21 01 00 00 00 00 00 00 16 16*/
/*�й�����05 ��Ϣ��DT 20 04*/
int8 check_active_frame(uint8 *data_frame, int32 len_frame)
{
    int i;
    uint8 *head_frame;

    if (( len_frame < 10 )||(data_frame == NULL))
    {
        return 0;
    }

    head_frame = data_frame;
    for(i=0; i<len_frame; i++)
    {
        if ((head_frame[0] == 0x68)&&(head_frame[5] == 0x68))
        {
            break;
        }
        head_frame++;
    }

    if (i >= len_frame)
    {
        return 0;
    }

    //DPRINT("in check active frame!\n");
    //putout(data_frame, len_frame);

    if( (head_frame[12] == 0x05)&&(head_frame[16] == 0x20)&&(head_frame[17] == 0x04) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/**********************************************************************
* @name      : wireless_gsm_send
* @brief     ��gsm���Ͷ���
* @param[in] ��void *pDev
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-10-28
* @Update    :
**********************************************************************/
uint8 wireless_gsm_send(uint8 ifnum, void *pDev, MESSAGE_INFO_T *ptMsgInfo, WIRELESS_HANDLE_T *ptHandle)
{
    uint16 send_len = 0;
    uint8  offset = 0;
    uint8 *pBuf = ptMsgInfo->playload + 1;    //ȥ���豸��
    
    SMS_PDU_SEND_T sms_send;
    uint8 send_pdu[GSM_LENGTH] = {0};        /*�����Ķ���*/
    uint8 phonelen = 0;
    uint8 dest_phone_num[SMS_PDU_PHONE_LEN + 1];
    uint8 dest_phone_bcd[SMS_PDU_PHONE_LEN];

    int   send_pdu_len;
    int   len_cmgs = 0;
    int send_flags = 0;

    uint32          msgLen = 0;
    uint8           retbool = 1;
    MESSAGE_INFO_T *ptMsgNew = NULL;
    char            uTopic[256] = {0};

    if(NULL == pDev)
    {
        return GMS_RECV_DEVICE;
    }
    
    memset(&sms_send, 0, sizeof(sms_send));

    phonelen = appmsg_get_len_offset(pBuf, &offset);
    pBuf += offset;

    /*�����յ����ŵĺ������Ϣ*/
    memset(dest_phone_num, 0, sizeof(dest_phone_num));
    memcpy(dest_phone_num, pBuf, MIN(SMS_PDU_PHONE_LEN, phonelen));  //�����ֻ���
    pBuf += phonelen;
    
    send_len = appmsg_get_len_offset(pBuf, &offset);
    pBuf += offset;

    if(phonelen > SMS_PDU_PHONE_LEN || send_len > 0xff)
    {
        MQTTMSG_FMT_DEBUG("���Ͷ��ź��볤�� %d, ���ݳ��� %d, ����֧�ַ�Χ\n", phonelen, send_len);
        send_len = 0;
    }

    if (send_len > 0)
    {
        MQTTMSG_BUF_DEBUG(pBuf, send_len, "���Ŷ˿�%d ���Ͷ��ŵ�%s, ����%s\n", 
                          AT_DEV_NUM(ifnum), dest_phone_num, 0 == ptHandle->ptNetPara->smsType ? "PDU" : "TEXT");

        if(0 == ptHandle->ptNetPara->smsType) //PDU
        {
            memset(&dest_phone_bcd, 0xff, sizeof(SMS_PDU_PHONE_LEN));
            ascii10_to_bcd(dest_phone_num, SMS_PDU_PHONE_LEN, dest_phone_bcd);

            /* ��ʼ������PDU�ṹ �������ĺ���ͨ�������ȡ */
            init_send_pdustr(ptHandle->ptNetPara->centerNnm, dest_phone_bcd, pBuf, send_len, &sms_send);
            
            /*��ӡ�ýṹ*/
            print_sms_send(&sms_send);

            /*ƴ����PDU��*/
            send_pdu_len = ping_send_pdu(sms_send, send_pdu, &len_cmgs);
            
            send_flags = send_pduSms(ifnum, pDev, send_pdu, send_pdu_len, len_cmgs);
        }
        else //TEXT ����
        {
            send_flags = send_textSms(ifnum, pDev, pBuf, send_len, dest_phone_num);
        }
        if (0 == send_flags)
        {
            MQTTMSG_FMT_DEBUG("���Ŷ˿�%d ���Ͷ��ųɹ�\n", AT_DEV_NUM(ifnum));
            retbool = 1;
        }
        else
        {
            MQTTMSG_FMT_DEBUG("���Ŷ˿�%d ���Ͷ���ʧ��%d\n", AT_DEV_NUM(ifnum), send_flags);
            retbool = 0;
        }
    }

    /* ���췵����Ϣ */
    msgLen = sizeof(MESSAGE_INFO_T) + 1;
    ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
    if(NULL == ptMsgNew)
    {
        MQTTMSG_FMT_DEBUG("wireless_sendsms_req malloc failed\n");
        return -1;
    }
    memset(ptMsgNew, 0, msgLen);

    ptMsgNew->rpm = 0;
    ptMsgNew->priority = ptMsgInfo->priority;
    ptMsgNew->index = ptMsgInfo->index;
    ptMsgNew->label = ptMsgInfo->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, ptMsgInfo->souraddr, ptMsgInfo->sourlen);
    ptMsgNew->destlen = ptMsgInfo->sourlen;

    ptMsgNew->IID = ptMsgInfo->IID;
    ptMsgNew->IOP = ptMsgInfo->IOP;

    ptMsgNew->msglen = 1;
    *ptMsgNew->playload = retbool;
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, ptMsgInfo->souraddr);
    wireless_send_msg(uTopic, ptMsgNew, 0);
    free(ptMsgNew);

    return GMS_RECV_DEVICE;
}

/**********************************************************************
* @name      : wireless_gsm_recv
* @brief     ��gsm���ն���
* @param[in] ��void *pDev
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-10-28
* @Update    :
**********************************************************************/
int wireless_gsm_recv(uint8 ifnum, void *pDev, SMS_PDU_RECV_T *pRecvSms, WIRELESS_HANDLE_T *ptHandle)
{
    int message_numble = 0;    /*��������*/
    int message_max = 0;       /*�������*/
    int message_len = 0;
    
    message_numble = get_message_num(ifnum, pDev, &message_max); /*��ȡ��������*/
    if (message_numble > 0)
    {
        message_len = read_message(ifnum, pDev, message_max, pRecvSms, ptHandle);
        
        return message_len;
    }
    else
    {
        if ( (message_numble != 0) && (message_numble != -1) )
        {
            MQTTMSG_FMT_DEBUG("DeviceRecv_gsm return %d\n", message_numble);
        }
        return message_numble;
    }
}

/**********************************************************************
* @name      : wireless_recv_smsback
* @brief     ��gsm�յ��Ľ����ŷ��ظ����ͷ�
* @param[in] ��MESSAGE_INFO_T *pMsg      �յ�����Ϣ
               SMS_PDU_RECV_T *pRecvSms  ��ѯ���Ķ���
* @param[out]��   
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2019-10-30
* @Update    : 2020-1-9
**********************************************************************/
int wireless_recv_smsback(MESSAGE_INFO_T *pMsg, SMS_PDU_RECV_T *pRecvSms)
{
    int             ret = 0;
    uint8          *pTemp = 0;
    MESSAGE_INFO_T *ptMsgNew = NULL;
    uint8           lenBuf[3] = {0};
    uint8           offset = 0;
    uint32          msgLen = 1;
    char            uTopic[256] = {0};

    if(NULL == pMsg || NULL == pRecvSms)
    {
        return -1;
    }

    if(pRecvSms->user_data_len > 0)
    {
        /* ������ų���ռλ */
        offset = appmsg_set_len_offset(pRecvSms->originator_address.len, lenBuf);
        msgLen += offset + pRecvSms->originator_address.len;
        
        offset = appmsg_set_len_offset(pRecvSms->user_data_len, lenBuf);
        msgLen += offset + pRecvSms->user_data_len;
    }

    ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen + sizeof(MESSAGE_INFO_T));
    if(NULL == ptMsgNew)
    {
        MQTTMSG_FMT_DEBUG("wireless_write_sms_queue malloc failed\n");
        return -1;
    }
    memset(ptMsgNew, 0, msgLen);
    
    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pMsg->priority;
    ptMsgNew->index = pMsg->index;
    ptMsgNew->label = pMsg->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
    ptMsgNew->destlen = pMsg->sourlen;

    ptMsgNew->IID = pMsg->IID;
    ptMsgNew->IOP = pMsg->IOP;

    ptMsgNew->msglen = msgLen;

    if(pRecvSms->user_data_len > 0)
    {
        *ptMsgNew->playload = 1;
        pTemp = ptMsgNew->playload + 1;

        offset = appmsg_set_len_offset(pRecvSms->originator_address.len, lenBuf);
        memcpy(pTemp, lenBuf, offset);
        pTemp += offset;

        memcpy(pTemp, pRecvSms->originator_address.addr, pRecvSms->originator_address.len);
        pTemp += pRecvSms->originator_address.len;

        offset = appmsg_set_len_offset(pRecvSms->user_data_len, lenBuf);
        memcpy(pTemp, lenBuf, offset);
        pTemp += offset;

        memcpy(pTemp, pRecvSms->user_data, pRecvSms->user_data_len);
        pTemp += pRecvSms->originator_address.len;
    }
    else
    {
        *ptMsgNew->playload = 0;
    }
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
    ret = wireless_send_msg(uTopic, ptMsgNew, 0);

    free(ptMsgNew);
    return ret;
}

/**********************************************************************
* @name      : wireless_get_gpsposition
* @brief     ����ѯGPSλ��
* @param[in] ��uint8 ifnum
               void *pDev
               uint8 bLog
* @param[out]��GPS_T *pGps
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-5-22
* @Update    :
**********************************************************************/
int wireless_get_gps(uint8 ifnum, void *pDev, GPS_T *pGps, uint8 bLog)
{
    char *ptemp = NULL;
    int   at_status = 0;
    char  at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    uint8 i = 0;
    uint16 len = 0;
    uint32 high = 10;
    char sTemp[16];
    float small = 0;
    
    memset(pGps, 0, sizeof(GPS_T));
    memset(&(pGps->utctime), 0xff, sizeof(OOP_DATETIME_T));
    pGps->mode = 4;
    pGps->ststus = 0;

/* NMEA-0183
$GNGGA ���Ļ�����ʽ���£����� M ָ��λ M��hh ָУ��ͣ�CR �� LF ����س����У���ͬ����
$GNGGA,(1),(2),(3),(4),(5),(6),(7),(8),(9),M,(10),M,(11),(12)*hh(CR)(LF)
$MYGPSPOS: $GPGGA,021812.000,3156.35964,N,11849.39925,E,1,06,3.2,18.2,M,0.8,M,,*5D
$MYGPSPOS: $GNGGA,120634.000,3156.36386,N,11849.44219,E,1,15,1.1,31.6,M,0.0,M,,*46

1.UTC ʱ�䣬��ʽΪ hhmmss.sss��
2.γ�ȣ���ʽΪ ddmm.mmmmm���ȷָ�ʽ����
3.γ�Ȱ���N �� S����γ����γ����
4.���ȣ���ʽΪ dddmm.mmmmm���ȷָ�ʽ����
5.���Ȱ���E �� W����������������
6.GPS ״̬����λ����ָʾ��0=��λ��Ч��1=��λ��Ч
7.����ʹ�õ����ڶ�λ������������00~12��
8.HDOP ˮƽ��ȷ�����ӣ�0.5~99.9��
9.���θ߶ȣ�-9999.9 �� 9999.9 �ף�
10.���ˮ׼��߶ȣ�-9999.9 �� 9999.9 �ף�
11.���ʱ�䣨�����һ�ν��յ�����źſ�ʼ���������ǲ�ֶ�λ������Ϊ�գ�
12.��ֲο���վ��ţ�0000 �� 1023����λ 0 Ҳ�����ͣ��ǲ�ֶ�λ������Ϊ��)
*/
    /* ��ȡ���θ߶� */
    memset(at_get_buf, 0, sizeof(at_get_buf));
    at_status = at_get_nolog(ifnum, pDev, "AT$MYGPSPOS=0\r\n", at_get_buf, sizeof(at_get_buf));
    if(FALSE == bLog)
    {
        MQTTMSG_FMT_TRACE("�豸%d ��ȡ�߶���Ϣ:\n===============================\n"
                          "����:AT$MYGPSPOS=0\n����:%s\n===============================\n", 
                          AT_DEV_NUM(ifnum), at_get_buf);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("�豸%d ��ȡ�߶���Ϣ:\n===============================\n"
                          "����:AT$MYGPSPOS=0\n����:%s\n===============================\n"
                          , AT_DEV_NUM(ifnum), at_get_buf);
    }
    
    if (0 != at_status || NULL != strstr(at_get_buf, "ERROR"))
    {
        return -1;
    }

    ptemp = strstr(at_get_buf, "MYGPSPOS");
    if(NULL == ptemp || NULL == strstr(ptemp, "OK"))
    {
        return 0;
    }

    len = strlen(ptemp);
    i = 0;
    while(i < len)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        i++;
    }
    ptemp = ptemp + i + 1;
    
    /* 9.���θ߶� */
    ptemp = strstr(ptemp, "M");
    if(NULL != ptemp)
    {
        ptemp -= 2;
        pGps->position.height = ((*ptemp) - '0') * high;
        for(i = 2; ',' !=  *(ptemp - i) && i < 10; i++)
        {
            if('-' ==  *(ptemp - i))
            {
                pGps->position.height = -pGps->position.height;
                break;
            }
            high *= 10;
            pGps->position.height += (*(ptemp - i) - '0') * high;
        }
    }

/* NMEA-0183
$GPRMC �Ƽ���С��������GPS��Ϣ��Recommended Minimum Specific GPS/TRANSIT Data��
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*<13><CR><LF>
$MYGPSPOS: $GPRMC,025944.000,A,3156.36431,N,11849.43602,E,0.00,0.00,200720,,,A,V*19
$MYGPSPOS: $GNRMC,121649.000,A,3156.36225,N,11849.44399,E,0.00,0.00,060820,,,A*72

<1> UTC��Coordinated Universal Time��ʱ�䣬hhmmss��ʱ���룩��ʽ
<2> ��λ״̬��A=��Ч��λ��V=��Ч��λ
<3> Latitude��γ��ddmm.mmmm���ȷ֣���ʽ��ǰ��λ��������0��
<4> γ�Ȱ���N�������򣩻�S���ϰ���
<5> Longitude������dddmm.mmmm���ȷ֣���ʽ��ǰ��λ��������0��
<6> ���Ȱ���E����������W��������
<7> �������ʣ�000.0~999.9�ڣ�Knot��ǰ��λ��������0��
<8> ���溽��000.0~359.9�ȣ����汱Ϊ�ο���׼��ǰ��λ��������0��
<9> UTC���ڣ�ddmmyy�������꣩��ʽ
<10> Magnetic Variation����ƫ�ǣ�000.0~180.0�ȣ�ǰ��λ��������0��
<11> Declination����ƫ�Ƿ���E��������W������
<12> Mode Indicator��ģʽָʾ����NMEA0183 3.00�汾�����A=������λ��D=��֣�E=���㣬N=������Ч��
<13> У��͡�
*/
    /* ��ȡλ����Ϣ UTC���� */
    at_status = at_get_nolog(ifnum, pDev, "AT$MYGPSPOS=3\r\n", at_get_buf, sizeof(at_get_buf));
    if(FALSE == bLog)
    {
        MQTTMSG_FMT_TRACE("�豸%d ��ȡ������Ϣ:\n==============================================\n"
                          "����:AT$MYGPSPOS=3\n����:%s\n==============================================\n", 
                          AT_DEV_NUM(ifnum), at_get_buf);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("�豸%d ��ȡ������Ϣ:\n==============================================\n"
                          "����:AT$MYGPSPOS=3\n����:%s\n==============================================\n", 
                          AT_DEV_NUM(ifnum), at_get_buf);
    }
    if (0 != at_status)
    {
        return -1;
    }

    ptemp = strstr(at_get_buf, "MYGPSPOS");
    if(NULL == ptemp || NULL == strstr(ptemp, "OK"))
    {
        return 0;
    }

    /* ��λ��ʽ */
    if(NULL != strstr(ptemp, "$GPRMC"))
    {
        pGps->system = 1;
    }
    else if(NULL != strstr(ptemp, "$BDRMC"))
    {
        pGps->system = 2;
    }
    else if(NULL != strstr(ptemp, "$GNRMC"))
    {
        pGps->system = 3;
    }
    else
    {
        pGps->system = 0;
    }

    len = strlen(ptemp);
    i = 0;
    while(i < len)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        i++;
    }
    if(i >= len)
    {
        return 0;
    }
    ptemp = ptemp + i + 1;

    /* 1. UTC ʱ�� */
    memset(sTemp, '0', 16);
    len = strlen(ptemp);
    for(i = 0; i < len; i++)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        sTemp[i] = ptemp[i];
    }
    if(0 != i)
    {
        pGps->utctime.hour = (sTemp[0] - '0') * 10 + (sTemp[1] - '0');
        pGps->utctime.minute = (sTemp[2] - '0') * 10 + (sTemp[3] - '0');
        pGps->utctime.second = (sTemp[4] - '0') * 10 + (sTemp[5] - '0');
        pGps->utctime.msec = (sTemp[7] - '0') * 100 + (sTemp[8] - '0') * 10 + (sTemp[9] - '0');
        memset(sTemp, '0', i);
    }

    /* 2.��λ״̬ */
    ptemp += i + 1;
    for(i = 0; i < len; i++)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        sTemp[i] = ptemp[i];
    }
    if(0 != i)
    {
        pGps->ststus = ('A' == sTemp[0]) ? 1 : 2;
        memset(sTemp, '0', i);
    }

    /* 3.γ�� */
    ptemp += i + 1;
    for(i = 0; i < len; i++)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        sTemp[i] = ptemp[i];
    }
    if(0 != i)
    {
        pGps->position.latitude.degre = (sTemp[0] -  '0') * 10 + (sTemp[1] - '0');
        pGps->position.latitude.branch = (sTemp[2] -  '0') * 10 + (sTemp[3] - '0');
        small = (sTemp[5] - '0') * 10000 + (sTemp[6] - '0') * 1000 + (sTemp[7] - '0') * 100 + 
                (sTemp[8] - '0') * 10 + (sTemp[9] - '0');
        pGps->position.latitude.branch += small / 100000.0;
        memset(sTemp, '0', i);
    }
    /* 4. γ�Ȱ��� */
    ptemp += i + 1;
    for(i = 0; i < len; i++)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        sTemp[i] = ptemp[i];
    }
    if(0 != i)
    {
        pGps->position.latitude.position = (('S' == sTemp[0]) ? 0 : 1);
        memset(sTemp, '0', i);
    }

    /* 5.���� */
    ptemp += i + 1;
    for(i = 0; i < len; i++)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        sTemp[i] = ptemp[i];
    }
    if(0 != i)
    {
        
        pGps->position.longitude.degre = (sTemp[0] -  '0') * 100 + (sTemp[1] - '0') * 10 + (sTemp[2] - '0');
        pGps->position.longitude.branch = (sTemp[3] -  '0') * 10 + (sTemp[4] - '0');
        small = (sTemp[6] - '0') * 10000 + (sTemp[7] - '0') * 1000 + (sTemp[8] - '0') * 100 + 
                (sTemp[9] - '0') * 10 + (sTemp[10] - '0');
        pGps->position.longitude.branch += small / 100000.0;
        memset(sTemp, '0', i);
    }
    /* 6.���Ȱ��� */
    ptemp += i + 1;
    for(i = 0; i < len; i++)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        sTemp[i] = ptemp[i];
    }
    if(0 != i)
    {
        pGps->position.longitude.position = (('E' == sTemp[0]) ? 0 : 1);
        memset(sTemp, '0', i);
    }

    /* 7. �������� */
    ptemp += i + 1;
    for(i = 0; i < len; i++)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        sTemp[i] = ptemp[i];
    }
    if(0 != i)
    {
        memset(sTemp, '0', i);
    }
    /* 8. ���溽�� */
    ptemp += i + 1;
    for(i = 0; i < len; i++)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        sTemp[i] = ptemp[i];
    }
    if(0 != i)
    {
        memset(sTemp, '0', i);
    }
    /* 9. UTC���� */
    ptemp += i + 1;
    for(i = 0; i < len; i++)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        sTemp[i] = ptemp[i];
    }
    if(0 != i)
    {
        pGps->utctime.mday = (sTemp[0] - '0') * 10 + (sTemp[1] - '0');
        pGps->utctime.month = (sTemp[2] - '0') * 10 + (sTemp[3] - '0');
        pGps->utctime.year = 2000 + (sTemp[4] - '0') * 10 + (sTemp[5] - '0');
        /* ���ڼ��� */
        uint32 d = pGps->utctime.mday, m = pGps->utctime.month, y = pGps->utctime.year;
        pGps->utctime.wday = ((d + 2*m + 3*(m+1)/5 + y + y/4 -y/100 + y/400) + 1) % 7;
        memset(sTemp, '0', i);
    }

    /* 10.��ƫ�� */
    ptemp += i + 1;
    for(i = 0; i < len; i++)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        sTemp[i] = ptemp[i];
    }
    if(0 != i)
    {
        memset(sTemp, '0', i);
    }

    /* 11. ��ƫ�Ƿ��� */
    ptemp += i + 1;
    for(i = 0; i < len; i++)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        sTemp[i] = ptemp[i];
    }
    if(0 != i)
    {
        memset(sTemp, '0', i);
    }

    /* 12.ģʽָʾ A=������λ��D=��֣�E=���㣬N=������Ч */
    ptemp += i + 1;
    for(i = 0; i < len; i++)
    {
        if(',' == ptemp[i])
        {
            break;
        }
        sTemp[i] = ptemp[i];
    }
    if(0 != i)
    {
        switch(sTemp[0])
        {
            case 'A': pGps->mode = 0; break;
            case 'D': pGps->mode = 1; break;
            case 'E': pGps->mode = 2; break;
            case 'N': pGps->mode = 4; break;
            default: pGps->mode = 4; break;
        }
        memset(sTemp, '0', i);
    }

    return 0;
}

/**********************************************************************
* @name      : wireless_get_gpsposition
* @brief     ����ѯGPSλ��
* @param[in] ��uint8 ifnum
               char *halname
               void *pDev
               uint8 bLog
* @param[out]��GPS_T *pGps
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-5-22
* @Update    :
**********************************************************************/
int wireless_get_gpsposition(uint8 ifnum, char *halname, void *pDev, GPS_T *pGps, uint8 bLog)
{
    void *pGPSDEV = NULL;
    GPS_T tGprs;
    int   ret = -1;
    
    memset(&tGprs, 0, sizeof(GPS_T));
    tGprs.ststus = 1;
    tGprs.mode = 4;
    
    if(halname[0])
    {
        pGPSDEV = wireless_uart_init(halname); /*�򿪴���*/
    }
    if(pGPSDEV)
    {
        ret = wireless_get_gps(ifnum, pGPSDEV, &tGprs, bLog);
        wireless_uart_uninit(pGPSDEV);
        pGPSDEV = NULL;
    }

    if(0 != ret)
    {
        ret = wireless_get_gps(ifnum, pDev, &tGprs, bLog);
    }

    memcpy(pGps, &tGprs, sizeof(GPS_T));
    if(FALSE == bLog)
    {
        MQTTMSG_FMT_TRACE("�豸%d ��ȡGPS��Ϣ:\n[%04d-%02d-%02d %02d:%02d:%02d.%03d] status: %d, mode: %d\n"
                          "γ��(%s): %d��%f��\n����(%s): %d��%f��\n�߶�: %d cm\n", 
                          AT_DEV_NUM(ifnum), pGps->utctime.year, pGps->utctime.month, pGps->utctime.mday, 
                          pGps->utctime.hour, pGps->utctime.minute, pGps->utctime.second, pGps->utctime.msec,
                          pGps->ststus, pGps->mode, 0 == pGps->position.latitude.position ? "��" : "��",
                          pGps->position.latitude.degre, pGps->position.latitude.branch,
                          0 == pGps->position.longitude.position ? "��" : "��",
                          pGps->position.longitude.degre, pGps->position.longitude.branch, pGps->position.height);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("�豸%d ��ȡGPS��Ϣ:\n[%04d-%02d-%02d %02d:%02d:%02d.%03d] status: %d, mode: %d\n"
                          "γ��(%s): %d��%f��\n����(%s): %d��%f��\n�߶�: %d cm\n", 
                          AT_DEV_NUM(ifnum), pGps->utctime.year, pGps->utctime.month, pGps->utctime.mday, 
                          pGps->utctime.hour, pGps->utctime.minute, pGps->utctime.second, pGps->utctime.msec,
                          pGps->ststus, pGps->mode, 0 == pGps->position.latitude.position ? "��" : "��",
                          pGps->position.latitude.degre, pGps->position.latitude.branch,
                          0 == pGps->position.longitude.position ? "��" : "��",
                          pGps->position.longitude.degre, pGps->position.longitude.branch, pGps->position.height);
    }

    return ret;
}

/**********************************************************************
* @name      : wireless_gps_back
* @brief     ��GPS���ظ����ͷ�
* @param[in] ��MESSAGE_INFO_T *pMsg   �յ�����Ϣ
               GPS_T *pGps            
* @param[out]��   
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-5-26
* @Update    : 
**********************************************************************/
int wireless_gps_back(MESSAGE_INFO_T *pMsg, GPS_T *pGps)
{
    int             ret = 0;
    uint32       msgLen = 0;
    MESSAGE_INFO_T *ptMsgNew = NULL;
    char    uTopic[256] = {0};
    uint8   palyout[512] = {0};
    uint16  outlen = 0;

    if(NULL == pMsg || NULL == pGps)
    {
        return -1;
    }

    outlen = appmsg_gps_package(pGps, palyout, 512, TRUE);
        
    /* ���췵����Ϣ */
    msgLen = sizeof(MESSAGE_INFO_T) + outlen;
    ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
    if(NULL == ptMsgNew)
    {
        MQTTMSG_FMT_DEBUG("wireless_gps_back malloc failed\n");
        return -1;
    }
    memset(ptMsgNew, 0, msgLen);

    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pMsg->priority;
    ptMsgNew->index = pMsg->index;
    ptMsgNew->label = pMsg->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
    ptMsgNew->destlen = pMsg->sourlen;

    ptMsgNew->IID = pMsg->IID;
    ptMsgNew->IOP = pMsg->IOP;

    ptMsgNew->msglen = outlen;
    memcpy(ptMsgNew->playload, palyout, outlen);
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
    ret = wireless_send_msg(uTopic, ptMsgNew, 0);
    free(ptMsgNew);
    return ret;
}

/**********************************************************************
* @name      : wireless_set_centernum
* @brief     �����ö������ĺ���
* @param[in] ��MESSAGE_INFO_T *pMsg        �յ�����Ϣ
               uint8 ifnum                 �豸���
               void *pDev                  �豸���
               WIRELESS_HANDLE_T *ptHandle �߳����ݾ�� 
* @param[out]��   
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-15
* @Update    : 
**********************************************************************/
int wireless_set_centernum(MESSAGE_INFO_T *pMsg, uint8 ifnum, void *pDev, WIRELESS_HANDLE_T *ptHandle)
{
    int             ret = 0;
    uint8         *temp = NULL;
    uint8          type = 0;
    uint16          len = 0;
    uint8 number[SMS_PDU_PHONE_LEN + 1] = {0};
    uint8       retbool = 1;
    uint32       msgLen = 0;
    MESSAGE_INFO_T *ptMsgNew = NULL;
    char    uTopic[256] = {0};
    char  at_get_buf[GSM_LENGTH];/*AT���������*/

    if(NULL == pMsg || NULL == ptHandle)
    {
        return -1;
    }
    
    temp = pMsg->playload;
    temp++;

    type = *temp;
    temp++;

    len = *temp;
    temp++;
    if(len > 0)
    {
        memcpy(number, temp, MIN(SMS_PDU_PHONE_LEN, len));
    }

    if(0 != number[0])
    {
        memset(at_get_buf, 0, sizeof(at_get_buf));
        sprintf(at_get_buf, "AT+CSCA=\"%s\",145\r\n", number);
        ret = at_check(AT_SMS_TYPE + ifnum, pDev, at_get_buf, "OK");
        if(0 != ret)
        {
            retbool = 0;
            WIRELESS_FMT_DEBUG("�豸%d ���ö������ĺ���%sʧ��\n", AT_DEV_NUM(ifnum), number);
        }
    }
    memset(at_get_buf, 0, sizeof(at_get_buf));
    sprintf(at_get_buf, "AT+CMGF=%d\r\n", type);
    ret = at_check(AT_SMS_TYPE + ifnum, pDev, at_get_buf, "OK");
    if(0 != ret)
    {
        retbool = 0;
        WIRELESS_FMT_DEBUG("�豸%d ���ö���ģʽ%sʧ��\n", AT_DEV_NUM(ifnum), 0 == type ? "PDU" : "TEXT");
    }
    
    if(0 != memcmp(number, ptHandle->ptNetPara->centerNnm, SMS_PDU_PHONE_LEN) || type != ptHandle->ptNetPara->smsType)
    {
        memcpy(ptHandle->ptNetPara->centerNnm, number, SMS_PDU_PHONE_LEN);
        ptHandle->ptNetPara->smsType = type;
        wireless_write_centernum(ptHandle->userfile, ifnum + 1, number, SMS_PDU_PHONE_LEN, type);
    }

    /* ���췵����Ϣ */
    msgLen = sizeof(MESSAGE_INFO_T) + 1;
    ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
    if(NULL == ptMsgNew)
    {
        MQTTMSG_FMT_DEBUG("wireless_smscenter_req malloc failed\n");
        return -1;
    }
    memset(ptMsgNew, 0, msgLen);

    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pMsg->priority;
    ptMsgNew->index = pMsg->index;
    ptMsgNew->label = pMsg->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
    ptMsgNew->destlen = pMsg->sourlen;

    ptMsgNew->IID = pMsg->IID;
    ptMsgNew->IOP = pMsg->IOP;

    ptMsgNew->msglen = 1;
    *ptMsgNew->playload = retbool;
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
    ret = wireless_send_msg(uTopic, ptMsgNew, 0);
    free(ptMsgNew);
    return ret;
}

/**********************************************************************
* @name      : wireless_trans_AT
* @brief     ��͸��ATָ��
* @param[in] ��MESSAGE_INFO_T *pMsg        �յ�����Ϣ
               uint8 ifnum                 �豸���
               void *pDev                  �豸���
               WIRELESS_HANDLE_T *ptHandle �߳����ݾ��
* @param[out]��   
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-1-25
* @Update    : 
**********************************************************************/
int wireless_trans_AT(MESSAGE_INFO_T *pMsg, uint8 ifnum, void *pDev, WIRELESS_HANDLE_T *ptHandle)
{
    uint8          *temp = NULL;
    uint8           no = 0;
    uint32          msgLen = 0;
    uint8           bytes = 0;
    uint16          ostrlen = 0;
    MESSAGE_INFO_T *ptMsgNew = NULL;
    char            uTopic[256] = {0};
    char            at_get_buf[GSM_LENGTH] = {0};/*AT���������*/
    char           *at_send_buf = NULL;
    uint16          getlen = 0;
    int             ret = 0;
    uint8           lenbuf[3] = {0};
    
    if(NULL == pMsg || NULL == ptHandle)
    {
        return -1;
    }
    
    temp = pMsg->playload;
    no = *temp;
    temp++;
    ostrlen = appmsg_get_len_offset(temp, &bytes);
    temp += bytes;
    
    at_send_buf = (char *)malloc(ostrlen + 3);
    if(NULL == at_send_buf)
    {
        goto end;
    }
    memset(at_send_buf, 0, ostrlen + 3);
    memcpy(at_send_buf, temp, ostrlen);
    if(at_send_buf[ostrlen - 2] != '\r' || at_send_buf[ostrlen - 1] != '\n')
    {
        at_send_buf[ostrlen] = '\r';
        at_send_buf[ostrlen + 1] = '\n';
    }

    MQTTMSG_FMT_DEBUG("�豸%d ATָ��͸�� ����:\n%s\n", ifnum + 1, at_send_buf);
    ret = at_get_nolog(AT_SMS_TYPE + ifnum, pDev, at_send_buf, at_get_buf, GSM_LENGTH);
    MQTTMSG_FMT_DEBUG("�豸%d ATָ��͸�� ����:\n%s\n", ifnum + 1, at_get_buf);
    free(at_send_buf);
    if(0 == ret)
    {
        getlen = strlen(at_get_buf);
    }
end:
    /* ���췵����Ϣ */
    msgLen = sizeof(MESSAGE_INFO_T) + getlen + 6;
    ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
    if(NULL == ptMsgNew)
    {
        MQTTMSG_FMT_DEBUG("wireless_smscenter_req malloc failed\n");
        return -1;
    }
    memset(ptMsgNew, 0, msgLen);

    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pMsg->priority;
    ptMsgNew->index = pMsg->index;
    ptMsgNew->label = pMsg->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
    ptMsgNew->destlen = pMsg->sourlen;

    ptMsgNew->IID = pMsg->IID;
    ptMsgNew->IOP = pMsg->IOP;

    temp = ptMsgNew->playload;
    *temp = no;
    temp++;
    if(getlen > 0)
    {
        bytes = appmsg_set_len_offset(getlen, lenbuf);
        ptMsgNew->msglen = 1 + bytes + getlen;
        memcpy(temp, lenbuf, bytes);
        temp += bytes;
        memcpy(temp, at_get_buf, getlen);
    }
    else
    {
        ptMsgNew->msglen = 2;
    }
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
    ret = wireless_send_msg(uTopic, ptMsgNew, 0);
    free(ptMsgNew);
    return ret;
}

/**********************************************************************
* @name      : wireless_set_getGPSmode
* @brief     �����ò�ѯGPSģ��
* @param[in] ��MESSAGE_INFO_T *pMsg        �յ�����Ϣ
               uint8 ifnum                 �豸���
               void *pDev                  �豸���
               WIRELESS_HANDLE_T *ptHandle �߳����ݾ��
* @param[out]��   
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2022-9-26
* @Update    : 
**********************************************************************/
int wireless_set_getGPSmode(uint8 ifnum, void *pDev, WIRELESS_HANDLE_T *ptHandle)
{
    /* GPS��λģʽ���� */
    if(0 != ptHandle->ptNetPara->pGpsSet->setcmd[0])
    {
        char send_cmd[128] = {0};
        char recv_cmd[128] = {0};
        char *temp = NULL;
        sprintf(send_cmd, "AT%s%d\r\n", ptHandle->ptNetPara->pGpsSet->setcmd, ptHandle->setGpsMode);
        at_check(ifnum, pDev, send_cmd, "OK");

        sleep(1);
        memset(send_cmd, 0, 128);
        sprintf(send_cmd, "AT%s?\r\n", ptHandle->ptNetPara->pGpsSet->getcmd);
        at_get(ifnum, pDev, send_cmd, recv_cmd, 127);
        if(NULL != strstr(recv_cmd, "OK"))
        {
            temp = strstr(recv_cmd, ptHandle->ptNetPara->pGpsSet->getcmd);
            if(temp)
            {
                temp += strlen(ptHandle->ptNetPara->pGpsSet->getcmd) + 1;
                ptHandle->curGpsMode = atoi(temp);
            }
        }
    }

    return 0;
}


/**********************************************************************
* @name      : wireless_check_simcard
* @brief     ����ѯSIM���Ƿ�����
* @param[in] ��void *pDev
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-9-27
* @Update    :
**********************************************************************/
int wireless_check_simcard(uint8 ifnum, void *pDev, WIRELESS_HANDLE_T *ptHandle)
{
    int  at_status = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    char *temp = NULL;
    uint8 ccid[21] = {0};
    
    memset(at_get_buf, 0, sizeof(at_get_buf));
    at_status = at_get_nolog(ifnum, pDev, "AT$MYCCID\r\n", at_get_buf, sizeof(at_get_buf)); /*��ȡSIM��ϵ�к�*/
    if(1 == at_status)
    {
        return 1;
    }
    
    temp = strstr((char *)at_get_buf, "$MYCCID");
    /*��δ�յ�'ERROR'����SIM������*/
    if (NULL != temp) /*���SIM���ɹ���֪ͨӦ�ò㣬������һ��*/
    {
        /* д��CCID */
        memcpy(ccid, temp+strlen("$MYCCID:")+2, 20);
        if(0 != memcmp(ccid, ptHandle->ptNetPara->ICCID, 20))
        {
            memcpy(ptHandle->ptNetPara->ICCID, ccid, 20);
            WIRELESS_FMT_DEBUG("%s%d CCID�仯%s\n", AT_DEV_NAMENUM(ifnum), ccid);
        }
    }
    if (NULL == temp) /*���SIM���ɹ���֪ͨӦ�ò㣬������һ��*/
    {
        WIRELESS_FMT_DEBUG("%s%d ��⵽SIM�����γ�, AT$MYCCID:\n%s\n", AT_DEV_NAMENUM(ifnum), at_get_buf);
        return -1;
    }
    return 0;
}

/**********************************************************************
* @name      : wireless_work_bofore_work
* @brief     �������߳�û��׼���õ������, ֧�ֲ���
* @param[in] ��uint8 ifnum                   ���ڱ��
               WIRELESS_HANDLE_T *ptHandle   ���
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-12-11
* @Update    :
**********************************************************************/
void wireless_work_bofore_work(uint8 ifnum, WIRELESS_HANDLE_T *ptHandle)
{
    void *pUart = ptHandle->pUart; /*���ھ��*/
    uint8 recvBuf[WIRELESS_BUF_LENGTH] = {0};   /*�豸���ͻ�����*/
    int   recvLen = 0;              /*�������ݳ���*/
    MESSAGE_INFO_T *ptMsgInfo = NULL;

    if(NULL != pUart && ptHandle->stepStatus < GSM_SUCCESS && ptHandle->stepStatus >= GSM_CPIN)
    {
        recvLen = read_queue(ptHandle->ptMsgQueue, recvBuf);
        if(ptHandle->curGpsMode != ptHandle->setGpsMode && ptHandle->stepStatus > GSM_CPIN)
        {
            wireless_set_getGPSmode(AT_SMS_TYPE + ifnum, pUart, ptHandle);
        }
    }
    
    if(recvLen > 0)
    {
        ptMsgInfo = (MESSAGE_INFO_T *)recvBuf;
        if(MESSAGE_IID_WIRELESS == ptMsgInfo->IID && WIRELESS_IOP_GET_GPS == ptMsgInfo->IOP)
        {
            if(0 == wireless_get_gpsposition(AT_SMS_TYPE + ifnum, ptHandle->ptNetPara->gpsHalName, pUart, 
                                             ptHandle->ptNetPara->pGps, TRUE))
            {
                ptHandle->bExistGPS = 1;
            }
            else
            {
                ptHandle->bExistGPS = 0;
            }
            wireless_gps_back(ptMsgInfo, ptHandle->ptNetPara->pGps);
        }
        else if(MESSAGE_IID_WIRELESS == ptMsgInfo->IID && WIRELESS_IOP_TRANS_AT == ptMsgInfo->IOP)
        {
            wireless_trans_AT(ptMsgInfo, ifnum, pUart, ptHandle);
        }
    }

    if(NULL != pUart && ptHandle->stepStatus < GSM_SUCCESS && ptHandle->stepStatus > GSM_CPIN)
    {
        struct timespec curtime = {0};
        clock_gettime(CLOCK_MONOTONIC, &curtime);

        if(curtime.tv_sec >= ptHandle->getGpsTime + 10)
        {
            if(0 == wireless_get_gpsposition(AT_SMS_TYPE + ifnum, ptHandle->ptNetPara->gpsHalName, pUart, 
                                             ptHandle->ptNetPara->pGps, FALSE))
            {
                ptHandle->bExistGPS = 1;
            }
            else
            {
                ptHandle->bExistGPS = 0;
            }
            ptHandle->getGpsTime = curtime.tv_sec;
        }
    }

}

/**********************************************************************
* @name      : wireless_GSM_CHECK
* @brief     ������ע����(ATָ�SIM���β塢����ע��)
* @param[in] ��uint8 ifnum                  ���ڱ��
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint8 *ftype                 ʧ������
               uint8 *cnt                   ����ʧ�ܴ���
* @return    ��0-����/> 0�쳣/< 0����
* @Create    : ����
* @Date      ��2021-1-12
* @Update    :
**********************************************************************/
int wireless_GSM_CHECK(uint8 ifnum, uint8 *ftype, uint8 *cnt, WIRELESS_HANDLE_T *ptHandle)
{
    void  *pUart = ptHandle->pUart;
    int    at_status = 0;
    int    signal_value = 0;
    uint8  network = 0;
    static uint8 nettime[2] = {0};
    
    /* ATָ�� */
    if(GSM_FAILED_AT == *ftype)
    {
        at_status = at_check(AT_SMS_TYPE + ifnum, pUart, "AT\r\n", "OK");
    }
    else
    {
        at_status = at_check_nolog(AT_SMS_TYPE + ifnum, pUart, "AT\r\n", "OK");
    }
    if(0 != at_status)
    {
        if(GSM_FAILED_AT != *ftype)
        {
            *cnt = 0;
            *ftype = GSM_FAILED_AT;
        }
        goto error;
    }

    /* SIM�� */
    at_status = wireless_check_simcard(AT_SMS_TYPE + ifnum, pUart, ptHandle);
    if(at_status < 0)
    {
        if(GSM_FAILED_SIM != *ftype)
        {
            *cnt = 0;  //SIM���ε����ټ��
            *ftype = GSM_FAILED_SIM;
        }
        goto error;
    }

    /* �ź�ǿ�� �� ������ʽ */
    signal_value = wireless_get_csqvalue(ifnum, pUart);
    if(ptHandle->ptNetPara->csqvalue != signal_value)
    {
        ptHandle->ptNetPara->csqvalue = (int16)signal_value;
    }
    network = wireless_get_network(AT_SMS_TYPE + ifnum, pUart, NULL, FALSE);
    if(ptHandle->ptNetPara->networkType != network || 
        signal_value > ptHandle->ptNetPara->basecsqvalue + 2 ||
        signal_value + 2 < ptHandle->ptNetPara->basecsqvalue || nettime[ifnum] >= 20)
    {
        /* �����¼� */
        nettime[ifnum] = 0;
        ptHandle->ptNetPara->networkType = network;
        ptHandle->ptNetPara->basecsqvalue = signal_value;
        wireless_send_change(ifnum, network, (uint16)signal_value);
    }
    else
    {
        nettime[ifnum] += 1;
    }

    /* ����ע�� */
    at_status = wireless_check_newwork_reg(AT_SMS_TYPE + ifnum, pUart, 
                                           (GSM_FAILED_REG == *ftype && 1 == *cnt % 10) ? TRUE : FALSE);
    if(0 != at_status)
    {
        if(GSM_FAILED_REG != *ftype)
        {
            *cnt = 0;
            *ftype = GSM_FAILED_REG;
        }
        goto error;
    }
    
    /* �ź�ǿ���ж� */
    if(signal_value < MIN_CSQ_VALUE)
    {
        if(GSM_FAILED_CSQ != *ftype)
        {
            *cnt = 0;
            *ftype = GSM_FAILED_CSQ;
        }
        if(1 == (*cnt % 20))
        {
            WIRELESS_FMT_DEBUG("�豸%d �ź���, ǿ��%d\n", AT_DEV_NUM(ifnum), signal_value);
        }
        goto error;
    }

    *ftype = GSM_FAILED_NULL;
    *cnt = 0;
    return 0;
error:
    *cnt += 1;
    switch(*ftype)
    {
        case GSM_FAILED_AT:  return *cnt > 4 ? -1 : 1;
        case GSM_FAILED_SIM: return *cnt > 30 ? -1 : 1;
        case GSM_FAILED_REG: return *cnt > 60 ? -1 : 1; /* 60sû������ ����ģ�� */
        case GSM_FAILED_CSQ: return *cnt > 120 ? -1 : 1;      /* 120s�����ź��� ���²��� */
        default: break;
    }
    return 0;
}

/**********************************************************************
* @name      : wireless_GSM_INIT
* @brief     ������Э��ջGSM_INIT ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_INIT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    UART_DEVICE_T* pUart = NULL; /*���ھ��*/
    uint8 devname[256] = {0};
    
    if(NULL != ptHandle->pUart)
    {
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d ��������ͷ�\n", AT_DEV_NUM(ifnum));
        wireless_uart_uninit(ptHandle->pUart);
        ptHandle->pUart = NULL;
    }

    pUart = (UART_DEVICE_T*)wireless_uart_init(dev); /*�򿪴���*/
    if (NULL == pUart)
    {
        if(stepCnt > 3)
        {
            WIRELESS_FMT_DEBUG("���Ŷ˿�%d ��δ򿪴���%sʧ�� ����ģ��!\n", AT_DEV_NUM(ifnum), dev);
            /* �����豸 */
            return GSM_RESTART;
        }
        *delay = 5000;
        return GSM_INIT;
    }
    pUart->uart_get_real_dev_name(pUart, devname);
    WIRELESS_FMT_DEBUG("���Ŷ˿�%d �򿪴���%s�ɹ�(�豸%s)\n", AT_DEV_NUM(ifnum), dev, devname);

    ptHandle->pUart = pUart;
    *delay = 1000;
    return GSM_AT;
}

/**********************************************************************
* @name      : wireless_GSM_AT
* @brief     ������Э��ջGSM_AT ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_AT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; /*���ھ��*/

    /*��ʼ��ͨѶģ��*/
    if(0 == at_check_model(AT_SMS_TYPE + ifnum, pUart, 1))
    {
        wireless_power_status(TRUE);
        return GSM_ATE0;
    }
    else
    {
        if(stepCnt >= 0)
        {
            if(ptHandle->sendfaild >= 5)
            {
                if(0xFF == ptHandle->commport)
                {
                    WIRELESS_FMT_DEBUG("���Ŷ˿�%d ģ���ʼ��ʧ��, �л�����!\n", AT_DEV_NUM(ifnum));
                    if(Isusb !=0xAA)
                    {
                        wireless_change_type(WIRELESS_COMMTYPE_UART);
                    }
                }
                else
                {
                    WIRELESS_FMT_DEBUG("���Ŷ˿�%d ģ���ʼ��ʧ��, ����ģ��\n", AT_DEV_NUM(ifnum));
                    /* �����豸 */
                }
                wireless_hw_reboot(10);
                return GSM_RESTART;
            }
            else
            {
                ptHandle->sendfaild += 1;
                *delay = 2000;
                return GSM_INIT;
            }
        }
    }
    *delay = 100;
    return GSM_AT;
}

/**********************************************************************
* @name      : wireless_GSM_ATE0
* @brief     ������Э��ջGSM_ATE0 ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_ATE0(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    int at_status = 0;
    void *pUart = ptHandle->pUart; /*���ھ��*/

    at_status = at_check(AT_SMS_TYPE + ifnum, pUart, "ATE0\r\n", "OK");
    return 0 == at_status ? GSM_CPIN : GSM_RESTART;
}

/**********************************************************************
* @name      : wireless_GSM_CPIN
* @brief     ������Э��ջGSM_CPIN ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2022-5-26
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CPIN(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; /*���ھ��*/
    uint8 newifnum = AT_SMS_TYPE + ifnum;
    int   ret = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    
    /*��ȡSIM��ϵ�к�*/
    if(stepCnt < 1)
    {
        at_get(newifnum, pUart, "AT+CPIN?\r\n", at_get_buf, AT_LENTH); 
        wireless_set_getGPSmode(newifnum, pUart, ptHandle);
    }
    else
    {
        struct timespec curtime = {0};
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        
        /* û��SIM�� 10s����һ��GPS */
        at_get_nolog(newifnum, pUart, "AT+CPIN?\r\n", at_get_buf, AT_LENTH); 
        if(0 == wireless_get_gpsposition(newifnum, ptHandle->ptNetPara->gpsHalName, ptHandle->pUart, 
                                         ptHandle->ptNetPara->pGps, 1 == stepCnt))
        {
            ptHandle->bExistGPS = 1;
        }
        else
        {
            ptHandle->bExistGPS = 0;
        }
        ptHandle->getGpsTime = curtime.tv_sec;

        if(ptHandle->curGpsMode != ptHandle->setGpsMode)
        {
            wireless_set_getGPSmode(newifnum, pUart, ptHandle);
        }
    }

    if(NULL != strstr(at_get_buf, "READY"))
    {
        ret = 0;
    }
    else
    {
        ret = 1;
    }
    
    if (0 == ret)
    {
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d ��⵽SIM������\n");
        return GSM_CIMI;
    }

    /*�����ɹ���ÿ��10s����һ�� �ȴ�SIM������*/
    if(stepCnt < 1)
    {
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d û�м�⵽SIM�� ����\n", AT_DEV_NUM(ifnum));
    }

    *delay = 10000;
    return GSM_CPIN;
}

/**********************************************************************
* @name      : wireless_GSM_CIMI
* @brief     ������Э��ջGSM_CIMI ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CIMI(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; /*���ھ��*/
    char  at_get_buf[WIRELESS_BUF_LENGTH] = {0};   /*�豸���ͻ�����*/
    int   at_status = 0;
    uint8 newifnum = AT_SMS_TYPE + ifnum;

    /*��ȡSIM��ϵ�к�*/
    if(stepCnt < 1)
    {
        at_status = at_get(newifnum, pUart, "AT+CIMI\r\n", at_get_buf, WIRELESS_BUF_LENGTH); 
    }
    else
    {
        at_status = at_get_nolog(newifnum, pUart, "AT+CIMI\r\n", at_get_buf, WIRELESS_BUF_LENGTH); 
    }
    
    if (0 != at_status)
    {   
        return GSM_INIT_END;
    }
    
    /*��δ�յ�'ERROR'����SIM������*/
    if(NULL == strstr(at_get_buf, "ERROR")) /*���SIM���ɹ���֪ͨӦ�ò㣬������һ��*/
    {
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d ��⵽SIM��IMSI: %s\n", AT_DEV_NUM(ifnum), at_get_buf);
        wireless_get_IMSI(at_get_buf, ptHandle->ptNetPara->IMSI, 15);
        return GSM_CSQ;
    }
    /*�����ɹ���ÿ��10s����һ�� �ȴ�SIM������*/
    if(stepCnt < 1)
    {
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d û�м�⵽SIM��IMSI ����\n", AT_DEV_NUM(ifnum));
    }

    *delay = 10000;
    return GSM_CIMI;
}

/**********************************************************************
* @name      : wireless_GSM_CSQ
* @brief     ������Э��ջGSM_CSQ ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CSQ(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_get_buf[WIRELESS_BUF_LENGTH] = {0};   /*�豸���ͻ�����*/
    void *pUart = ptHandle->pUart; /*���ھ��*/
    int at_status = 0;
    char *ptemp = NULL;
    int   signal_value = 0;

    /*����AT+CSQ, �鿴�ź�ֵ*/
    if(stepCnt < 1)
    {
        at_status = at_get(AT_SMS_TYPE + ifnum, pUart, "AT+CSQ\r\n", at_get_buf, WIRELESS_BUF_LENGTH); /*����AT+CSQ*/
        ptemp = strstr(at_get_buf, "+CSQ:");
        signal_value = at_get_csqvalue(ptemp);
        if(signal_value <= 0)
        {
            memset(at_get_buf, 0, sizeof(at_get_buf));
            at_status = at_get(AT_SMS_TYPE + ifnum, pUart, "AT+CESQ\r\n", at_get_buf, sizeof(at_get_buf));
            if (0 == at_status)
            {
                ptemp = strstr((char *)at_get_buf, "+CESQ:");
                signal_value = at_get_cesqvalue(ptemp);
            }
        }
        if (0 != at_status)
        {   
            return GSM_INIT_END;
        }
    }
    else
    {
        signal_value = wireless_get_csqvalue(ifnum, pUart);
    }
    
    ptHandle->ptNetPara->csqvalue = signal_value;
    WIRELESS_FMT_DEBUG("���Ŷ˿�%d �ź�ǿ��%d\n", AT_DEV_NUM(ifnum), signal_value);
    if (signal_value >= MIN_CSQ_VALUE)
    {
        ptHandle->ptNetPara->networkType = wireless_get_network(AT_SMS_TYPE + ifnum, pUart, NULL, FALSE);
        wireless_send_change(ifnum, ptHandle->ptNetPara->networkType, (uint16)ptHandle->ptNetPara->csqvalue);
        return GSM_CREG;
    }
    else if(stepCnt >= GSM_GET_CNT)
    {
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d �ź�ǿ����, ����ģ��\n", AT_DEV_NUM(ifnum));
        return GSM_RESTART;
    }
    *delay = 5000;
    return GSM_CSQ;
}

/**********************************************************************
* @name      : wireless_GSM_CREG
* @brief     ������Э��ջGSM_CREG ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CREG(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; /*���ھ��*/

    /*����AT+CREG?�����SIM�Ƿ���ע�ᵽ���� */
    if(0 == wireless_check_newwork_reg(AT_SMS_TYPE + ifnum, pUart, stepCnt < 1 ? TRUE : FALSE))
    {
        ptHandle->bsimOK = 0;
        return GSM_CMGF;
    }
    else
    {
        if (stepCnt > 30)       //����
        {
            WIRELESS_FMT_DEBUG("���Ŷ˿�%d ���û������ע��, ����\n", AT_DEV_NUM(ifnum));
            /* �����豸 */
            return GSM_RESTART;
        }
        else
        {
            if(stepCnt < 1)
            {
                WIRELESS_FMT_DEBUG("���Ŷ˿�%d ���û������ע��, ����\n", AT_DEV_NUM(ifnum));
            }
            *delay = 10000;
            return GSM_CREG;
        }
    }
}

/**********************************************************************
* @name      : wireless_GSM_CMGF
* @brief     ������Э��ջGSM_CMGF ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CMGF(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_send_buf[WIRELESS_BUF_LENGTH] = {0};   /*�豸���ͻ�����*/
    void *pUart = ptHandle->pUart; /*���ھ��*/
    int at_status = 0;

    /*����AT+CMGF=, ���ö��Ÿ�ʽ*/
    sprintf(at_send_buf, "AT+CMGF=%d\r\n", ptHandle->ptNetPara->smsType);
    at_status = at_check(AT_SMS_TYPE + ifnum, pUart, at_send_buf, "OK");       /*���ڷ���AT����*/
    return (0 == at_status) ? GSM_CPMS : GSM_INIT_END;
}

/**********************************************************************
* @name      : wireless_GSM_CPMS
* @brief     ������Э��ջGSM_CPMS ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CPMS(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_get_buf[WIRELESS_BUF_LENGTH] = {0};   /*�豸���ͻ�����*/
    void *pUart = ptHandle->pUart; /*���ھ��*/
    int at_status = 0;

    /*����AT+CPMS="ME","ME","MT", ���ö��Ŵ洢ģʽ*/
    at_status = at_get(AT_SMS_TYPE + ifnum, pUart, "AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n", at_get_buf, WIRELESS_BUF_LENGTH);
    if (0 == at_status)
    {
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d ��ʼ���ɹ�\n", AT_DEV_NUM(ifnum));
    }
    *delay = 1000;
    return 0 == at_status ? GSM_CSCA : GSM_INIT_END; 
}

/**********************************************************************
* @name      : wireless_GSM_CSCA
* @brief     ������Э��ջGSM_CSCA ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CSCA(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_buf[WIRELESS_BUF_LENGTH] = {0};   /*�豸���ͻ�����*/
    int at_status = 0;
    void *pUart = ptHandle->pUart; /*���ھ��*/

    /* �������ĺ������� */
    if(0 != ptHandle->ptNetPara->centerNnm[0])
    {
        sprintf(at_buf, "AT+CSCA=\"%s\",145\r\n", ptHandle->ptNetPara->centerNnm);
        at_status = at_check(AT_SMS_TYPE + ifnum, pUart, at_buf, "OK");
        if(0 != at_status)
        {
            WIRELESS_FMT_DEBUG("�豸%d ���ö������ĺ���%sʧ��\n", AT_DEV_NUM(ifnum), ptHandle->ptNetPara->centerNnm);
        }
    }

    /* ��ѯ�������ĺ��� ֻ�ǲ�ѯ ��ǰû�õ� */
    memset(at_buf, 0, WIRELESS_BUF_LENGTH);
    at_status = at_get_nolog(AT_SMS_TYPE + ifnum, pUart, "AT+CSCA?\r\n", at_buf, WIRELESS_BUF_LENGTH);
    if(0 == at_status)
    {
        WIRELESS_FMT_DEBUG("�豸%d �������ĺ����ѯ���%s\n", AT_DEV_NUM(ifnum), at_buf);
    }
    return GSM_GPS; 
}

/**********************************************************************
* @name      : wireless_GSM_GPS
* @brief     ������Э��ջGSM_GPS ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_GPS(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    if(0 == wireless_get_gpsposition(AT_SMS_TYPE + ifnum, ptHandle->ptNetPara->gpsHalName, ptHandle->pUart, 
                                     ptHandle->ptNetPara->pGps, TRUE))
    {
        ptHandle->bExistGPS = 1;
        WIRELESS_FMT_DEBUG("�豸%d �������Ƕ�λģ��\n", AT_DEV_NUM(ifnum));
    }
    else
    {
        ptHandle->bExistGPS = 0;
        WIRELESS_FMT_DEBUG("�豸%d ���������Ƕ�λģ��\n", AT_DEV_NUM(ifnum));
    }
    return GSM_SUCCESS;
}

/**********************************************************************
* @name      : wireless_AT_RESTART
* @brief     ������Э��ջGSM_RESTART ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_RESTART(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    if(ptHandle->pUart)
    {
        wireless_uart_uninit(ptHandle->pUart);
        ptHandle->pUart = NULL;
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d ����ͷ�\n", AT_DEV_NUM(ifnum));
    }

    /* �����豸 */
    wireless_restart_dev(ifnum, ptHandle->commport + ((ptHandle->subport << 4) & 0xf0));

    *delay = 20000;
    ptHandle->workAgain = TRUE; //�˳���ǰ״̬��
    return GSM_INIT_END;
}

/**********************************************************************
* @name      : wireless_GSM_SUCCESS
* @brief     ������Э��ջGSM_SUCCESS ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_SUCCESS(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    /* �������ݻ�ȡ */
    wireless_get_phonenum(AT_SMS_TYPE + ifnum, ptHandle->pUart, ptHandle->ptNetPara->SIMNum, 24);

    ptHandle->dialState = PPP_DAIL_STATUS_ONLINE;
    ptHandle->ptNetPara->smsStatusOK = TRUE;
    return GSM_WORK;
}

/**********************************************************************
* @name      : wireless_GSM_END
* @brief     ������Э��ջGSM_INIT_END ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_END(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    if(ptHandle->pUart)
    {
        wireless_uart_uninit(ptHandle->pUart);
        ptHandle->pUart = NULL;
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d ����ͷ�\n", AT_DEV_NUM(ifnum));
    }

    *delay = 20000;
    ptHandle->workAgain = TRUE; //�˳���ǰ״̬��
    return GSM_INIT_END;
}

/**********************************************************************
* @name      : wireless_AT_REBOOT
* @brief     ������Э��ջGSM_AT_REBOOT ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2021-4-8
* @Update    :
**********************************************************************/
uint32 wireless_AT_REBOOT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
#if 0
    void *pUart = ptHandle->pUart; /*���ھ��*/
    int at_status = 0;
    at_status = at_check(AT_SMS_TYPE + ifnum, pUart, "AT+CFUN=1,1\r\n", "OK");
    if(0 == at_status)
    {
        ptHandle->ATReboot = FALSE;
    }
#endif
    return GSM_INIT_END;
}

/**********************************************************************
* @name      : wireless_GSM_WORK
* @brief     ������Э��ջGSM_WORK ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_WORK(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void  *pUart = ptHandle->pUart;
    uint8 state_dev = GMS_SEND_DEVICE;            /*�豸����״̬*/   

    SMS_PDU_RECV_T recv_sms;
    uint8 send_buf[WIRELESS_BUF_LENGTH];   /*�豸���ͻ�����*/
    int    recv_len;              /*�������ݳ���*/
    uint32 send_len;              /*�������ݳ���*/
    WIRELESS_QUEUE_T *ptMsgQueue = ptHandle->ptMsgQueue;  /* �̴߳�����Ϣ���� */
    TIME_STRUCT_T     currentTime = {0};
    TIME_STRUCT_T     lastTime = {0};
    MESSAGE_INFO_T   *ptMsgInfo = NULL;
    uint8             gpstime = WIRELESS_GPS_TIME; //��ȡGPS��ʱ��
    uint8             aterrorCnt = 0;
    int               at_status = 0;
    uint8             failedType = GSM_FAILED_NULL;
    char             *failedname[4] = {"ATָ��", "SIM��״̬", "����ע��", "�ź�ǿ��"};
    while(1)
    {
        if(FALSE == ptHandle->workFlag || TRUE == ptHandle->workAgain)
        {
            *delay = 1000;
            return GSM_INIT_END;
        }

        /* һ��ͳ��һ������, ���ж��Ƿ��쳣 */
        get_curtime_ms(&currentTime);
        currentTime.milliseconds = 0;
        if(0 != memcmp(&currentTime, &lastTime, sizeof(TIME_STRUCT_T)))
        {
            at_status = wireless_GSM_CHECK(ifnum, &failedType, &aterrorCnt, ptHandle);
            if(at_status < 0)
            {
                WIRELESS_FMT_DEBUG("�豸%d �������%s�쳣, ����ģ��\n", AT_DEV_NUM(ifnum), 
                                   failedType <= 4 ? failedname[failedType-1] : "��������");
                return GSM_RESTART;
            }
            else if(0 == at_status)
            {
                if(gpstime >= WIRELESS_GPS_TIME)
                {
                    gpstime = 0;
                    /* ��ȡGPSλ�� */
                    if(1 == ptHandle->bExistGPS)
                    {
                        wireless_get_gpsposition(AT_SMS_TYPE + ifnum, ptHandle->ptNetPara->gpsHalName, pUart, 
                                                 ptHandle->ptNetPara->pGps, FALSE);
                    }
                }
                else
                {
                    gpstime++;
                }
            }
            memcpy(&lastTime, &currentTime,sizeof(TIME_STRUCT_T));
        }

        /* ͨ��ɨ����Ϣ�������ж���Ϣ�շ� */
        send_len = read_queue(ptMsgQueue, send_buf);
        if(send_len > 0 && GMS_CLOSE_DEVICE != state_dev)
        {
            ptMsgInfo = (MESSAGE_INFO_T *)send_buf;
            if(MESSAGE_IID_WIRELESS == ptMsgInfo->IID)
            {
                if(WIRELESS_IOP_SEND_SMS == ptMsgInfo->IOP)
                {
                    /* ���Ͷ��� */
                    MQTTMSG_FMT_DEBUG("�豸%d ���Ͷ���\n", AT_DEV_NUM(ifnum));
                    state_dev = wireless_gsm_send(AT_SMS_TYPE + ifnum, pUart, ptMsgInfo, ptHandle);
                }
                else if(WIRELESS_IOP_REVC_SMS == ptMsgInfo->IOP)
                {
                    /* ���ն��� */
                    MQTTMSG_FMT_DEBUG("�豸%d ���ն���\n", AT_DEV_NUM(ifnum));

                    recv_len = wireless_gsm_recv(AT_SMS_TYPE + ifnum, pUart, &recv_sms, ptHandle);
                    /* д�뻺�������, �ȴ����� */
                    if(recv_len > 0)
                    {
                        if (check_active_frame(recv_sms.user_data, recv_sms.user_data_len) == 1)
                        {
                            state_dev = GMS_CLOSE_DEVICE;
                            WIRELESS_FMT_DEBUG("�豸%d ���ռ������\n", AT_DEV_NUM(ifnum));
                        }
                        else
                        {
                            wireless_recv_smsback(ptMsgInfo, &recv_sms);
                        }
                    }
                    else 
                    {
                        bzero(&recv_sms, sizeof(recv_sms));
                        wireless_recv_smsback(ptMsgInfo, &recv_sms);
                    }
                }
                else if(WIRELESS_IOP_GET_GPS == ptMsgInfo->IOP)
                {
                    if(0 == wireless_get_gpsposition(AT_SMS_TYPE + ifnum, ptHandle->ptNetPara->gpsHalName, pUart, 
                                                     ptHandle->ptNetPara->pGps, TRUE))
                    {
                        ptHandle->bExistGPS = 1;
                        WIRELESS_FMT_TRACE("�豸%d �������Ƕ�λģ��\n", AT_DEV_NUM(ifnum));
                    }
                    else
                    {
                        ptHandle->bExistGPS = 0;
                        WIRELESS_FMT_TRACE("�豸%d ���������Ƕ�λģ��\n", AT_DEV_NUM(ifnum));
                    }
                    gpstime = 0;
                    wireless_gps_back(ptMsgInfo, ptHandle->ptNetPara->pGps);
                }
                else if(WIRELESS_IOP_SET_SMSNO == ptMsgInfo->IOP)
                {
                    wireless_set_centernum(ptMsgInfo, ifnum, pUart, ptHandle);
                }
                if(WIRELESS_IOP_TRANS_AT == ptMsgInfo->IOP)
                {
                    wireless_trans_AT(ptMsgInfo, ifnum, pUart, ptHandle);
                }
            }
            memset(send_buf, 0, WIRELESS_BUF_LENGTH);
        }

        if(ptHandle->curGpsMode != ptHandle->setGpsMode)
        {
            wireless_set_getGPSmode(AT_SMS_TYPE + ifnum, pUart, ptHandle);
        }

        if(state_dev == GMS_CLOSE_DEVICE)
        {
            WIRELESS_FMT_DEBUG("�豸%d �����߳��˳�\n", AT_DEV_NUM(ifnum));
            return GSM_INIT_END;
        }
        usleep(10000);
    }

    return GSM_INIT_END;
}

/**********************************************************************
* @name      : wireless_gsm_working
* @brief     ��gsm����, ע�ᡢ��ѯ�������շ�
* @param[in] ��uint8 commport               �˿ں�
               uint8 ifnum                  ���ڱ��
               WIRELESS_HANDLE_T *ptHandle  �߳����ݾ��
* @param[out]��
* @return    ���˳��ȴ�ʱ��s
* @Create    : ����
* @Date      ��2019-10-23
* @Update    : 2020-11-13
**********************************************************************/
int wireless_gsm_working(uint8 commport, uint8 ifnum, WIRELESS_HANDLE_T *ptHandle)
{
    uint32  i;
    uint32  status = GSM_INIT; /*���Ӳ���*/
    uint32  stepcnt = 0;       /*״̬������*/
    uint32  othercount = 0;       /*������������*/
    uint32  delay = 0;
    uint16  buflen = 0;

    WIRELESS_WORK_FUN_T statusFuns[] = 
    {
        {2, GSM_INIT,    wireless_GSM_INIT,    "GSM_INIT", {0}, 0, 0},
        {2, GSM_AT,      wireless_GSM_AT,      "GSM_AT",   {0}, 0, 0},
        {1, GSM_ATE0,    wireless_GSM_ATE0,    "GSM_ATE0", {0}, 0, 0},
        {2, GSM_CPIN,    wireless_GSM_CPIN,    "GSM_CPIN", {0}, 0, 0},
        {2, GSM_CIMI,    wireless_GSM_CIMI,    "GSM_CIMI", {0}, 0, 0},
        {2, GSM_CSQ,     wireless_GSM_CSQ,     "GSM_CSQ",  {0}, 0, 0},
        {2, GSM_CREG,    wireless_GSM_CREG,    "GSM_CREG", {0}, 0, 0},
        {0, GSM_CMGF,    wireless_GSM_CMGF,    "GSM_CMGF", {0}, 0, 0},
        {0, GSM_CPMS,    wireless_GSM_CPMS,    "GSM_CPMS", {0}, 0, 0},
        {0, GSM_CSCA,    wireless_GSM_CSCA,    "GSM_CSCA", {0}, 0, 0},
        {0, GSM_GPS,     wireless_GSM_GPS,     "GSM_GPS",  {0}, 0, 0},

        {0, GSM_RESTART, wireless_GSM_RESTART, "GSM_RESTART", {0}, 0, 0},
        {0, GSM_SUCCESS, wireless_GSM_SUCCESS, "GSM_SUCCESS", {0}, 0, 0},
        {0, GSM_WORK,    wireless_GSM_WORK,    "GSM_WORK",    {0}, 0, 0},

        {0, GSM_INIT_END, wireless_GSM_END,    "GSM_INIT_END", {0}, 0, 0},
    };

    uint32 statusNum = sizeof(statusFuns)/sizeof(WIRELESS_WORK_FUN_T);

    ptHandle->stepStatus = status;
    ptHandle->bsimOK = 1;

    /* �������Ϣ���� */
    clear_queue(ptHandle->ptMsgQueue);
    while(1)
    {
        /* �쳣״̬��� */
        if(FALSE == ptHandle->workFlag || TRUE == ptHandle->workAgain)
        {
            WIRELESS_FMT_DEBUG("���Ŷ˿�%d ״̬�쳣, ֹͣ����\n", AT_DEV_NUM(ifnum));
            if(ptHandle->pUart)
            {
                wireless_uart_uninit(ptHandle->pUart);
                ptHandle->pUart = NULL;
                WIRELESS_FMT_DEBUG("���Ŷ˿�%d ����ͷ�\n", AT_DEV_NUM(ifnum));
            }
            return 20;
        }

        /* ֧�������в�����ȡ֮ǰ��ȡ */
        wireless_work_bofore_work(ifnum, ptHandle);

        /* ״̬���л���ʱ */
        if(delay >= 100)
        {
            delay -= 100;
            usleep(100000);
            continue;
        }
        else
        {
            delay = 0;
        }

        /* ״̬��ִ�� */
        for(i = 0; i < statusNum; i++)
        {
            if(status == statusFuns[i].status)
            {
                if(NULL == statusFuns[i].func)
                {
                    WIRELESS_FMT_DEBUG("���Ŷ˿�%d ��Ч״̬�� %d, �л���״̬ INIT\n", AT_DEV_NUM(ifnum), status);
                    status = AT_INIT;
                    break;
                }
                if((stepcnt < statusFuns[i].printNum || 0 == statusFuns[i].printNum) && 0 != strlen(statusFuns[i].name))
                {
                     WIRELESS_FMT_DEBUG("���Ŷ˿�%d �л�״̬%s\n", AT_DEV_NUM(ifnum), statusFuns[i].name);
                }
                buflen = strlen(statusFuns[i].info);
                if(0 != buflen && (0 == statusFuns[i].showNum || stepcnt < statusFuns[i].showNum))
                {
                    /* ״̬��ˢ�� */
                    wireless_send_diagStatus(ifnum + 1, statusFuns[i].info, buflen, statusFuns[i].times);
                }
                
                status = statusFuns[i].func(ifnum, ptHandle->halName, stepcnt, &othercount, ptHandle, &delay);
                ptHandle->stepStatus = status;
                /* ״̬��������״̬ */
                stepcnt = status != statusFuns[i].status ? 0 : (stepcnt + 1);
                break;
            }
        }
        if(i >= statusNum)
        {
            WIRELESS_FMT_DEBUG("���Ŷ˿�%d ��Ч״̬�� %d, �л���״̬ INIT\n", AT_DEV_NUM(ifnum), status);
            status = AT_INIT;
        }
    }

    return 10;
}

/**********************************************************************
* @name      : wireless_at0_init
* @brief     ������0��ʼ��
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int wireless_at0_init(void *pPublic, void **ppPrivate)
{
    WIRELESS_DATA_T      *ptWireData = (WIRELESS_DATA_T*)pPublic;
    ptWireData->atdata0.no = 0;
    *ppPrivate = &ptWireData->atdata0;
    return 0;
}

/**********************************************************************
* @name      : wireless_at1_init
* @brief     ������1��ʼ��
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int wireless_at1_init(void *pPublic, void **ppPrivate)
{
    WIRELESS_DATA_T      *ptWireData = (WIRELESS_DATA_T*)pPublic;
    ptWireData->atdata1.no = 1;
    *ppPrivate = &ptWireData->atdata1;
    return 0;
}

/**********************************************************************
* @name      : wireless_at_proc
* @brief     �����Ŵ���
* @param[in] ��void *pArg     �߳̾��
               void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2021-3-22
* @Update    :
**********************************************************************/
int wireless_at_proc(void *pArg, void *pPublic, void *pPrivate)
{
    WIRELESS_HANDLE_T *ptHandle = (WIRELESS_HANDLE_T *)pPrivate;
    int                delay = 0;

    if(FALSE != ptHandle->workFlag && WIRELESS_UNUSED_COMMPORT != ptHandle->commport)
    {
        ptHandle->dialState = PPP_DAIL_STATUS_OFFLINE;
        ptHandle->ptNetPara->smsStatusOK = FALSE;
        ptHandle->workAgain = FALSE;
        ptHandle->stepStatus = 0;
        ptHandle->sendfaild = 0;

        sleep(1);  //�ȴ�1s ����ͬ��
        
        delay = wireless_gsm_working(ptHandle->commport, ptHandle->no, ptHandle);

        /* �˳����� */
        ptHandle->stepStatus = 0;
        ptHandle->dialState = PPP_DAIL_STATUS_OFFLINE;
        ptHandle->ptNetPara->smsStatusOK = FALSE;
        bzero(ptHandle->ptNetPara->IMSI, 16);
        ptHandle->ptNetPara->networkType = OOP_NET_UNKNOW; /* �������� */
        bzero(ptHandle->ptNetPara->SIMNum, 24);  /* SIM ������ */

        ptHandle->ptNetPara->csqvalue = 0;    /* �ź�ǿ�� */
        ptHandle->ptNetPara->basecsqvalue = 0;/* ��׼�ź�ǿ�� */

        ptHandle->ptNetPara->pGps->mode = 4;
        ptHandle->ptNetPara->pGps->ststus = 0;
        ptHandle->bExistGPS = 0;
        wireless_send_change(ptHandle->no, WIRELESS_OPT_NONE, 0);

        sleep(delay);
    }
    
    return 1000000;

}

