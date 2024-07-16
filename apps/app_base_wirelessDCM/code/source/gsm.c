/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块GPRS和GSM功能
* @date：    2019-10-29
* @history： 
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
* @brief     ：字节转ascii
* @param[in] ：uint8 byte    字节数据
* @param[out]：char *buflow  低内容
               char *bufhigh 高内容
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-26
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
* @brief     ：ascii转字节
* @param[in] ：char ascii    ascii
* @param[out]：uint8 *byte   字节
* @return    ：0-成功/1-失败
* @Create    : 王津剑
* @Date      ：2020-4-26
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
*模块编号：
*名称：将模式串对应的数据转换成BCD(10进制的手机号)
*功能：将模式串对应的数据转换成BCD
*输入参数：目的数据（*dest）、源数据（*src）、源数据长度（len）
*输出参数：转换后的数据。
*处理：将ASCII转换成BCD
*返回：无
***************************************************************************
-*/
void ascii10_to_bcd(uint8 *asc, uint16 len, uint8 *bcd)
{
    uint8	i = 0, j, k;
	uint8   num = 0;
    uint8   temp = 0;

    /* 去掉 +86 86 */
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
    /* 最后一位补f */
	if(1 == k)
    {
        num += 0xf;
        bcd[j] = num;
    }
}

/*********************************************************************
    所属功能单元：GSM动态库
    功能描述：初始化发送短信的PDU结构体
    输入参数：
        void *pDev            句柄
        dest_phone_num        短信目的号码
        send_buf              发送数据
        send_len              发送数据长度
        pdu_sms               发送PDU结构体
        
    输出参数:
        无
    函数返回值:
        无
    内部处理流程: 填写发送PDU结构体pdu_sms，将短信中心号码、短信目的号码等
                                参数写入PDU结构中
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

    /*确定短信中心电话号码长度*/
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
    

    /*短信中心电话号码长度*/
    if(service_center_len > 0)
    {
        /*短信中心号码*/
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
    
    /*协议数据单元类型*/
    pdu_sms->pdu_type = 0x11;
    
    /*SMS-SUBMIT参考数目*/
    pdu_sms->message_reference = 0x00;
    
    /*确定接收方电话号码长度*/
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
    
    /*接收方电话号码长度*/
    pdu_sms->destination_address.len = (uint8)dest_phone_len;    /*+2字节为86*/

    dest_phone_len = dest_phone_len/2 +dest_phone_len%2;
    
    /*接收方号码类型*/
//    pdu_sms->destination_address.type = 0x91;        /*国际短信格式 +86*/
    pdu_sms->destination_address.type = 0xA1;            /*国内短信格式 无需 +86*/    
    
    /*接收方电话号码*/
    memset(pdu_sms->destination_address.addr, 0xFF, SMS_PDU_PHONE_LEN);
//    pdu_sms->destination_address.addr[0] = 0x86;
    memcpy(pdu_sms->destination_address.addr, dest_phone_num, dest_phone_len);
    
    //putout(dest_phone_num, dest_phone_len);
    
    /*SMSC以何种方式处理SM (比如FAX,、Voice等)*/    
    pdu_sms->protocol_identifier = 0x00;

    /*用户数据(UD)编码方案*/
    pdu_sms->data_coding_scheme = 0x04;

    /*消息在SMSC中不再有效的时长*/
    pdu_sms->validity_period[0] = 0xAA;
    
    /*用户数据长度*/
    pdu_sms->user_data_len = (uint8)send_len;
    
    /*用户数据*/    
    memcpy(pdu_sms->user_data, send_buf, send_len);
    
    return;
}

/*********************************************************************
    所属功能单元：GSM动态库
    功能描述：显示SMS_PDU_Send_str结构
    输入参数：
        sms    发送的PDU结构
    输出参数：
        无
    函数返回值：
        无
    内部处理流程：输出SMS_PDU_Send_str结构的各个成员
********************************************************************/
void print_sms_send(SMS_PDU_SEND_T *sms)
{
    char strtemp[2048] = {0};
    uint16 len = 0;
    uint16 i = 0;
    len = sprintf(strtemp, "发送短信:\nmessage center len: %02x\nmessage center type: %02x\nmessage center num: ", 
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
    所属功能单元：GSM动态库
    功能描述：显示SMS_PDU_Send_str结构
    输入参数：
        sms    发送的PDU结构
    输出参数：
        无
    函数返回值：
        无
    内部处理流程：输出SMS_PDU_Send_str结构的各个成员
********************************************************************/
void print_sms_recv(SMS_PDU_RECV_T *sms)
{
    char strtemp[2048] = {0};
    uint16 len = 0;
    uint16 i = 0;
    len = sprintf(strtemp, "接收短信:\nmessage center len: %02x\nmessage center type: %02x\nmessage center num: ", 
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
所属功能单元：GSM动态库
功能描述：将16进制数转换成两个Ascii码
输入参数：
bin        16进制数
high    Ascii码高位
low        Ascii码地位
输出参数:
无
函数返回值:
无
内部处理流程:将16进制数拆成两个Ascii码
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
所属功能单元：GSM动态库
功能描述：将两个Ascii码转换成一个16进制数
输入参数：
high    转换为16进制高位
low        转换为16进制地位
输出参数：
无
函数返回值：
转换后的16进制数
内部处理流程: 将high转换成进制高位, low转换成16进制低位
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
    所属功能单元：GSM动态库
    功能描述：组pdu短信数据包
    输入参数：
        sms_send    pdu结构
        pdu_sms        存放组好的pdu数据包
        pcgms_len    pdu数据包数据体长度
        
    输出参数:
        无
    函数返回值:
        pdu数据包长度
        
    内部处理流程: 按pdu数据结构内容组成pdu数据包
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
    
    /*短信中心地址*/
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
    /*协议数据单元类型*/
    wireless_binto_Ascii(sms_send.pdu_type, &pdu_sms[0], &pdu_sms[1]);
    pdu_sms_len += 2;
    cgms_len += 1;
    pdu_sms += 2;    
    
    /*所有成功的SMS-SUBMIT参考数目(0..255)*/    
    wireless_binto_Ascii(sms_send.message_reference, &pdu_sms[0], &pdu_sms[1]);
    pdu_sms_len += 2;
    cgms_len += 1;
    pdu_sms += 2;
    
    /*接收方SME的地址*/
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
    
    /*参数显示SMSC以何种方式处理SM (比如FAX,、Voice等)*/
    wireless_binto_Ascii(sms_send.protocol_identifier, &pdu_sms[0], &pdu_sms[1]);
    cgms_len += 1;
    pdu_sms_len += 2;
    pdu_sms += 2;                

    /*参数表示用户数据(UD)编码方案*/
    wireless_binto_Ascii(sms_send.data_coding_scheme, &pdu_sms[0], &pdu_sms[1]);
    cgms_len += 1;
    pdu_sms_len += 2;
    pdu_sms += 2;
    
    /*消息在SMSC中不再有效的时长*/
    if ( ( (sms_send.pdu_type)&0x18 ) == 0x18 )    /* PDU_Type = 11 用绝对值表示有效时间,PDU_Type = 10, */
                                                /* 用相对值表示有效时间 */
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
    /*用户数据长度*/
    wireless_binto_Ascii(sms_send.user_data_len, &pdu_sms[0], &pdu_sms[1]);
    cgms_len += 1;
    pdu_sms_len += 2;
    pdu_sms += 2;    
    
    /*用户数据*/
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
所属功能单元：GSM动态库
功能描述：发送PDU短信数据包
输入参数：
    fd                串口操作符
    pdu_data    pdu数据包
    pdu_len        pdu数据包长度
    len_cgms    pdu数据包数据体长度
    
输出参数:
    无
函数返回值:
    0    发送成功
    -1    发送失败
    
内部处理流程: 将打包好的pdu数据包用短信发送出去
              短信发送流程：AT+CMGS=len 0x0D 数据包 0x1A    
********************************************************************/
int send_pduSms(uint8 ifnum, void *pDev, uint8 *pdu_data, int pdu_len, int len_cgms)
{
    uint8 *pcheck = NULL;
    int i = 0;
    int recv_len = 0;
    char send_buf[300] = {0};
    char recv_buf[300] = {0};
    int ret = 0;
    
    /* 先检查是否短信pdu模式 */
    ret = at_get_nolog(ifnum, pDev, "AT+CMGF?\r\n", recv_buf, 300);
    MQTTMSG_FMT_DEBUG("设备%d 检查短信模式%s, 发送AT+CMGF?\n接收: %s\n", AT_DEV_NUM(ifnum), 0 == ret ? "" : "失败", 
                      recv_buf);
    if(0 != ret || NULL == strstr(recv_buf, "+CMGF: 0"))
    {
        memset(recv_buf, 0, 300);
        ret = at_get_nolog(ifnum, pDev, "AT+CMGF=0\r\n", recv_buf, 300);
        MQTTMSG_FMT_DEBUG("设备%d 设置短信模式%s, 发送AT+CMGF=0\n接收: %s\n", AT_DEV_NUM(ifnum), 0 == ret ? "" : "失败", 
                          recv_buf);
        if(0 != ret || NULL == strstr(recv_buf, "OK"))
        {
            MQTTMSG_FMT_DEBUG("%s%d设置短信PDU模式失败\n", AT_DEV_NAMENUM(ifnum));
            return -1;
        }
    }

    /* 设置TE字符集 */
    memset(recv_buf, 0, 300);
    ret = at_get_nolog(ifnum, pDev, "AT+CSCS=\"UCS2\"\r\n", recv_buf, 300);
    MQTTMSG_FMT_DEBUG("设备%d 设置TE字符集%s, 发送AT+CSCS=\"UCS2\"\n接收: %s\n", AT_DEV_NUM(ifnum), 0 == ret ? "" : "失败", 
                      recv_buf);
    if(0 != ret || NULL == strstr(recv_buf, "OK"))
    {
        MQTTMSG_FMT_DEBUG("%s%d: 设置UCS2字符集失败\n", AT_DEV_NAMENUM(ifnum));
        return -1;
    }
    memset(recv_buf, 0, 300);

    /*发送AT+CMGS=XX + 0D(&H)*/
    sprintf((char *)send_buf, "AT+CMGS=%d\x0d", len_cgms);
    wireless_uart_send(pDev, (uint8 *)send_buf, strlen(send_buf));

    /*等模块应答'>'后,开始发送数据包*/
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
        MQTTMSG_FMT_DEBUG("%s%d: 发送短信失败, 长度%d\n", AT_DEV_NAMENUM(ifnum), len_cgms);
        return -1;
    }
    else
    {
        pcheck = NULL;
    }

    MQTTMSG_FMT_DEBUG("%s%d\n===============================\n"
                      "发送: %s\n接收: %s\n===============================\n",
                      AT_DEV_NAMENUM(ifnum), send_buf, recv_buf);

    /*发送pdu数据包*/
    wireless_uart_send(pDev, (uint8 *)pdu_data, pdu_len);
    
    /*发送1A结束*/
    send_buf[0] = 0x1A;
    wireless_uart_send(pDev, (uint8 *)send_buf, 1);    
    
    i = 0;
    pcheck = NULL;
    memset(recv_buf, 0, sizeof(recv_buf));
    recv_len = 0;
    
    /*检测短信是否发送成功*/
    while( (pcheck == NULL) && (i < 30) )
    {
        usleep(400000);
        recv_len = wireless_uart_recv(pDev, (uint8 *)recv_buf+recv_len, sizeof(recv_buf));
        if ( recv_len > 0 )
            pcheck = (uint8 *)strstr((char *)recv_buf, "OK");
        i++;    
    }

    MQTTMSG_FMT_DEBUG("%s%d短信\n===============================\n"
                      "发送: %s\n接收: %s\n===============================\n", 
                      AT_DEV_NAMENUM(ifnum), pdu_data, recv_buf);

    if (pcheck == NULL)
    {
        MQTTMSG_FMT_DEBUG("%s%d发送短信失败\n", AT_DEV_NAMENUM(ifnum));
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
* @brief     ：发送text格式短信
* @param[in] ：void *pDev    设备句柄
               uint8 *data   数据
               uint16 len    长度
               uint8 *dest   目标地址
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-22
* @Update    :
**********************************************************************/
int send_textSms(uint8 ifnum, void *pDev, uint8 *data, uint16 len, uint8 *dest)
{
    int ret = 0;
    uint8 *pcheck = NULL;
    int i;
    int newlen = 0;
    int recv_len;
    char at_get_buf[GSM_LENGTH] = {0};/*AT命令返回内容*/
    char send_ascii[GSM_LENGTH + 1] = {0}; /*完整的短信*/

    /* TEXT模式发送的是ASCII字符串 */
    /* 由于发送短信输入的是字节串, text不支持，需要转换成字节流 */
    newlen = 0;
    for(i = 0; i < len; i++)
    {
        send_ascii[newlen] = data[i];
        newlen++;
        if(newlen > GSM_LENGTH)
        {
            MQTTMSG_FMT_DEBUG("%s%d发送短信超过长度\n", AT_DEV_NAMENUM(ifnum));
            return -1;
        }
    }

    /* 先检查是否短信text模式 */
    ret = at_get_nolog(ifnum, pDev, "AT+CMGF?\r\n", at_get_buf, GSM_LENGTH);
    MQTTMSG_FMT_DEBUG("设备%d 检查短信模式%s, 发送AT+CMGF?\n接收: %s\n", AT_DEV_NUM(ifnum), 0 == ret ? "" : "失败", 
                      at_get_buf);
    if(0 != ret || NULL == strstr(at_get_buf, "+CMGF: 1"))
    {
        memset(at_get_buf, 0, GSM_LENGTH);
        ret = at_get_nolog(ifnum, pDev, "AT+CMGF=1\r\n", at_get_buf, GSM_LENGTH);
        MQTTMSG_FMT_DEBUG("设备%d 设置短信模式%s, 发送AT+CMGF=1\n接收: %s\n", AT_DEV_NUM(ifnum), 0 == ret ? "" : "失败", 
                          at_get_buf);
        if(0 != ret || NULL == strstr(at_get_buf, "OK"))
        {
            MQTTMSG_FMT_DEBUG("%s%d设置短信text模式失败\n", AT_DEV_NAMENUM(ifnum));
            return -2;
        }
    }

    /* 设置TE字符集 */
    memset(at_get_buf, 0, GSM_LENGTH);
    ret = at_get_nolog(ifnum, pDev, "AT+CSCS=\"GSM\"\r\n", at_get_buf, GSM_LENGTH);
    MQTTMSG_FMT_DEBUG("设备%d 设置TE字符集%s, 发送AT+CSCS=\"GSM\"\n接收: %s\n", AT_DEV_NUM(ifnum), 0 == ret ? "" : "失败", 
                      at_get_buf);
    if(0 != ret || NULL == strstr(at_get_buf, "OK"))
    {
        MQTTMSG_FMT_DEBUG("%s%d: 设置GSM字符集失败\n", AT_DEV_NAMENUM(ifnum));
        return -3;
    }

    memset(at_get_buf, 0, GSM_LENGTH);
    sprintf(at_get_buf, "AT+CMGS=\"%s\"\r", dest);
    wireless_uart_send(pDev, (uint8*)at_get_buf, strlen((char *)at_get_buf));
    /*等模块应答'>'后,开始发送数据包*/
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
                      "发送: AT+CMGS=\"%s\"\n接收: %s\n===============================\n", 
                      AT_DEV_NAMENUM(ifnum), dest, at_get_buf);
    if (pcheck == NULL)
    {
        MQTTMSG_FMT_DEBUG("%s%d发送短信给%s失败\n", AT_DEV_NAMENUM(ifnum), dest);
        return -4;
    }

    /*发送text数据包*/
    if(0 != wireless_uart_send(pDev, (uint8 *)send_ascii, newlen))
    {
        MQTTMSG_FMT_DEBUG("%s%d发送短信给%s失败\n", AT_DEV_NAMENUM(ifnum), dest);
        return -5;
    }
    
    /*发送1A结束*/
    at_get_buf[0] = 0x1A;
    if(0 != wireless_uart_send(pDev, (uint8*)at_get_buf, 1))
    {
        MQTTMSG_FMT_DEBUG("%s%d发送Ctrl+z失败\n", AT_DEV_NAMENUM(ifnum));
        return -6;
    }

    i = 0;
    pcheck = NULL;
    memset(at_get_buf, 0, sizeof(at_get_buf));
    recv_len = 0;
    
    /*检测短信是否发送成功*/
    while( (pcheck == NULL) && (i < 30) )
    {
        usleep(400000);
        recv_len = wireless_uart_recv(pDev, (uint8*)at_get_buf+recv_len, sizeof(at_get_buf)-recv_len);
        if ( recv_len > 0 )
            pcheck = (uint8 *)strstr((char *)at_get_buf, "OK");
        i++;    
    }
    MQTTMSG_FMT_DEBUG("%s%d\n===============================\n"
                      "发送: %s\n接收: %s\n===============================\n", 
                      AT_DEV_NAMENUM(ifnum), send_ascii, at_get_buf);

    if (pcheck == NULL)
    {
        MQTTMSG_FMT_DEBUG("%s%d发送短信失败\n", AT_DEV_NAMENUM(ifnum));
        return -7;
    }
    else
    {
        pcheck = NULL;
        return 0;
    }
}

/*********************************************************************
所属功能单元：GSM动态库
功能描述：读模块中短信的条数
输入参数：
-       void *pDev     串口句柄

输出参数:
-       无

函数返回值:
-       >= 0    短信条数
-       -1      读出错

内部处理流程: 
-       发送命令AT+CPMS="ME","ME","ME(MC35为MT)",检测短信条数
-       西门子应答：+CPMS: 0,25,0,25,0,85
-       明基M23A应答：+CPMS: 0,0,0,0,1,60
********************************************************************/
int get_message_num(uint8 ifnum, void *pDev, int *maxnum)
{
    int at_status;
    int message_num = 0;
    int message_max = 0;
    uint8 *pcpms;
    char   at_get_buf[GSM_LENGTH] = {0};
    uint8 *pring = NULL;

    /*发送AT+CPMS="SM","SM","SM", 读取模块中短信的存储状况*/
    at_status = at_get_nolog(ifnum, pDev, "AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n", at_get_buf, sizeof(at_get_buf));
    MQTTMSG_FMT_DEBUG("设备%d 获取短信数量%s, 发送AT+CPMS=\"SM\",\"SM\",\"SM\"\n接收: %s\n", 
                      AT_DEV_NUM(ifnum), 0 == at_status ? "" : "失败", at_get_buf);
    /*模块异常,没有应答AT命令*/
    if (0 == at_status)
    {        
        /*在混合模式下, ring 激活gprs*/
        pring = (uint8 *)strstr((char *)at_get_buf, "RING");
        if (pring != NULL)
        {
            at_check(ifnum, pDev, "ATH\r\n", "OK");
            return -2;
        }

        pcpms = (uint8 *)strstr((char *)at_get_buf, "+CPMS:");    /*在AT命令应答中查找+CPMS:*/

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
所属功能单元：GSM动态库
功能描述：解析接收到的pdu数据包
输入参数：
    message    接收到pdu短信数据包
    message_len    短信长度
    sms            短信数据结构指针
    
输出参数:
    无
函数返回值:
    0  成功
    -1 失败
    
内部处理流程:将短信按pdu格式进行分析，提取短信中心号码、发送者电话号码等参数
********************************************************************/
int parse_sms(uint8 *message, int message_len, SMS_PDU_RECV_T *sms)
{
    int temp;
    uint8 tempchar = 0;
    int i;
    uint8 j;
    uint8 phone[SMS_PDU_PHONE_LEN] = {0};

    //printf("in ParseSms\n");    
    sms->service_center_address.len = wireless_Asciit_tobin(message[0], message[1]);    /*取短信中心号码长度*/
    //printf("Tel_len: %d\n",(int)sms->service_center_address.len);

    message += 2;
    message_len -= 2;

    /*短信中心号码长度若为负数或大于20，则按出错处理*/    
    if (sms->service_center_address.len <= 20 && sms->service_center_address.len > 0)
    {
        sms->service_center_address.type = wireless_Asciit_tobin(message[0], message[1]);
        message += 2;
        message_len -= 2;
        if (sms->service_center_address.type == 0x91)
        {
            /* 跳过前面的86 */
            message += 2;
            message_len -= 2;                    
            /*提取短信中心号码*/
            for (i=0; i<(sms->service_center_address.len) - 2; i++)
            {
                sms->service_center_address.addr[i] = wireless_Asciit_tobin(message[1], message[0]);
                message += 2;
                message_len -=2;
            }
            /*号码不足20位，后面的补'F'*/
            while(i < SMS_PDU_PHONE_LEN)
            {
                sms->service_center_address.addr[i]    = 0xFF;
                i++;
            }
        }
        else
        {
            /*提取短信中心号码*/
            for (i=0; i<(sms->service_center_address.len) - 1; i++)
            {
                sms->service_center_address.addr[i] = wireless_Asciit_tobin(message[1], message[0]);
                message += 2;
                message_len -=2;
            }
            /*号码不足20位，后面的补'F'*/
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
    /*len = 0 没有短信中心号码认为正确*/

    //printf("PDU_Addr len = %02x, type = %02x\n", sms->service_center_address.len, sms->service_center_address.type);    
    //printf("message center number:\n");
    //putout(sms->service_center_address.addr, SMS_PDU_PHONE_LEN);
    //printf("message:\n");
    //putout(message, message_len);


    /*协议数据单元类型 PDU_type*/
    sms->pdu_type = wireless_Asciit_tobin(message[0], message[1]);
    message += 2;
    message_len -= 2;

    /*发送方电话号码*/
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

    /*参数显示SMSC以何种方式处理SM (比如FAX,、Voice等)*/
    sms->protocol_identifier = wireless_Asciit_tobin(message[0], message[1]);
    message += 2;
    message_len -= 2;

    /*参数表示用户数据(UD)编码方案*/
    sms->data_coding_scheme = wireless_Asciit_tobin(message[0], message[1]);
    message += 2;
    message_len -= 2;

    /*收到消息的时间*/
    for(i = 0; i < 7; i++)
    {
        sms->service_time_stamp[i] = wireless_Asciit_tobin(message[1], message[0]);
        message += 2;
        message_len -= 2;            
    }

    /*用户数据长度*/
    sms->user_data_len = wireless_Asciit_tobin(message[0], message[1]);
    message += 2;
    message_len -= 2;  

    /*用户数据*/
    for(i = 0; i < sms->user_data_len; i++)
    {
        sms->user_data[i] = wireless_Asciit_tobin(message[0], message[1]);
        message += 2;
        message_len -= 2;    
    }

    /* 将号码数据转换成ASCII */
    if(sms->service_center_address.len > 0)
    {
        j = 0;
        for(i = 0; i < SMS_PDU_PHONE_LEN/2; i++)
        {
            if(0xff == sms->service_center_address.addr[i])
            {
                break;
            }
            /* 高位在前 低位在后 */
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
            /* 高位在前 低位在后 */
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
    所属功能单元：GSM动态库
    功能描述：删除第index条短信
    输入参数：
        pDev    串口操作符
        index    短信编号
    输出参数：
        无
    函数返回值：
        无
    内部处理流程：串口发送AT+CMGD=index,删除第index条短信
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
    ret = wireless_uart_send(pDev, (uint8 *)at_send, strlen(at_send));    /*发送AT命令*/
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
    MQTTMSG_FMT_DEBUG("删除第%d条短信%s, 返回内容\n%s\n", index, 0 == ret ? "成功" : "失败", at_recv);
}

/*********************************************************************
所属功能单元：GSM动态库
功能描述：判短信发送方电话号码是否与主台电话号码一致
输入参数：
    pdu_addr    发送方pdu地址    
    phone_num    主台电话号码
    phone_len    主台电话号码长度
    
输出参数:
    无
函数返回值:
    0   号码相同
    -1    号码不相同
    
内部处理流程: 
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
* @brief     ：外置协议栈AT_ST 状态机处理
* @param[in] ：char *meaasge  短信消息
* @param[out]：SMS_PDU_RECV_T *pRecvSms 
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-23
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

    /* 获取手机号 */
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
    
    /* 获取短信内容开始 */
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
    /* 实际ascii内容需要转换成字节流格式 */
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
* @brief     ：读短信对短信的发送者身份、有效时间进行验证;提取符合条件的短信的
               内容，处理完后删除短信
* @param[in] ：uint8 ifnum 设备编号
               void *pDev  设备句柄
               int maxnum  短信数量
               WIRELESS_HANDLE_T *ptHandle 线程句柄
* @param[out]：SMS_PDU_RECV_T *pRecvSms    接收短信
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2020-4-24
* @Update    :
**********************************************************************/
int read_message(uint8 ifnum, void *pDev, int maxnum, SMS_PDU_RECV_T *pRecvSms, WIRELESS_HANDLE_T *ptHandle)
{
    int len;
    int i;
    int message_index = 0;    /*短信编号*/
    int message_frame_len = 0;/*短信帧长度*/
    int at_status;
    SMS_PDU_RECV_T recv_sms;
    char  at_get_buf[GSM_LENGTH] = {0};
    char  at_send_buf[50] = {0};
    uint8 *pcms = NULL;
    uint8 *pcmgr = NULL;
    uint8 *ppick = NULL;
    char  *pcpms = NULL;

    memset(&recv_sms, 0, sizeof(SMS_PDU_RECV_T));
    
/*从第一条开始读短信，直到读到一条短信为止
发送AT命令: AT+CMGR=n (n = 1,2,3...)
应答格式:
AT+CMGR=n
+CMGR: 1,,34
0891683108200505F0040D91683106154617F50000701090712450230F61F1985C369FD169F59ADD76BF01

OK
*/
    while(message_index <= maxnum)  /*MAX_MESSAGE_INDEX 模块中可以存储的最大短信条数*/
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
                del_message(ifnum, pDev, message_index); /*删除短信*/
                message_index++;
                continue;
            }
            
            MQTTMSG_FMT_DEBUG("%s%d短信序号%d, 内容:\n%s\n", AT_DEV_NAMENUM(ifnum), message_index, at_get_buf);
            MQTTMSG_BUF_DEBUG(at_get_buf, strlen(at_get_buf), "十六进制内容\n");

            pcpms = strstr(at_get_buf, "+CMGR: \"REC");
            /* 文本格式 TEXT */
            if(NULL != pcpms)
            {
                MQTTMSG_FMT_DEBUG("Text SMS\n");
                if(0 == get_text_message(pcpms, &recv_sms))
                {
                    /*提取短信*/
                    memcpy(pRecvSms, &recv_sms, sizeof(SMS_PDU_RECV_T));
                    if(recv_sms.user_data_len > 0)
                    {
                        del_message(ifnum, pDev, message_index); /*删除短信*/
                        return (int)recv_sms.user_data_len;
                    }
                }
                del_message(ifnum, pDev, message_index); /*删除短信*/
                message_index++;
                continue;
            }
            
            /*检查该位置是否有短信存放*/
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
                    /*取短信长度*/
                    MQTTMSG_FMT_TRACE("Read The message!\n");

                    i = 0;
                    len = strlen((char *)pcmgr);
                    ppick = pcmgr;

                    while(i + 1 < len && ppick[i] != 0x0d && ppick[i+1] != 0x0a)
                        i++;
                        
                    if (i < len)
                    {
                        ppick += i+2;        /*得到完整的短信帧，包括短信头*/
                        message_frame_len = strlen((char *)ppick);
                        
                        if (0 != parse_sms(ppick, message_frame_len, &recv_sms))
                        {
                            del_message(ifnum, pDev, message_index); /*删除短信*/
                            message_index++;
                            continue;
                        }
                        print_sms_recv(&recv_sms);
                        
                        /*提取短信*/
                        memcpy(pRecvSms, &recv_sms, sizeof(SMS_PDU_RECV_T));
                        if(recv_sms.user_data_len > 0)
                        {
                            del_message(ifnum, pDev, message_index); /*删除短信*/
                            return (int)recv_sms.user_data_len;
                        }
                    }
                }
            }
            del_message(ifnum, pDev, message_index); /*删除短信*/
        }
        else
        {
            return -1;
        }
        message_index++;
    }
    
    /*若出现有短信但读不到情况, 删除所有短信*/
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

/*04激活帧 ff 68 39 00 39 00 68 4A 11 34 01 00 02 05 71 00 00 20 04 98 0A CE 16*/
/*浙江版激活帧 68 99 01 01 00 5E 13 68-0F 09 00 00 21 01 00 00 00 00 00 00 16 16*/
/*判功能码05 信息类DT 20 04*/
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
* @brief     ：gsm发送短信
* @param[in] ：void *pDev
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-10-28
* @Update    :
**********************************************************************/
uint8 wireless_gsm_send(uint8 ifnum, void *pDev, MESSAGE_INFO_T *ptMsgInfo, WIRELESS_HANDLE_T *ptHandle)
{
    uint16 send_len = 0;
    uint8  offset = 0;
    uint8 *pBuf = ptMsgInfo->playload + 1;    //去掉设备号
    
    SMS_PDU_SEND_T sms_send;
    uint8 send_pdu[GSM_LENGTH] = {0};        /*完整的短信*/
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

    /*按接收到短信的号码回信息*/
    memset(dest_phone_num, 0, sizeof(dest_phone_num));
    memcpy(dest_phone_num, pBuf, MIN(SMS_PDU_PHONE_LEN, phonelen));  //拷贝手机号
    pBuf += phonelen;
    
    send_len = appmsg_get_len_offset(pBuf, &offset);
    pBuf += offset;

    if(phonelen > SMS_PDU_PHONE_LEN || send_len > 0xff)
    {
        MQTTMSG_FMT_DEBUG("发送短信号码长度 %d, 内容长度 %d, 超过支持范围\n", phonelen, send_len);
        send_len = 0;
    }

    if (send_len > 0)
    {
        MQTTMSG_BUF_DEBUG(pBuf, send_len, "短信端口%d 发送短信到%s, 类型%s\n", 
                          AT_DEV_NUM(ifnum), dest_phone_num, 0 == ptHandle->ptNetPara->smsType ? "PDU" : "TEXT");

        if(0 == ptHandle->ptNetPara->smsType) //PDU
        {
            memset(&dest_phone_bcd, 0xff, sizeof(SMS_PDU_PHONE_LEN));
            ascii10_to_bcd(dest_phone_num, SMS_PDU_PHONE_LEN, dest_phone_bcd);

            /* 初始化短信PDU结构 短信中心号码通过命令获取 */
            init_send_pdustr(ptHandle->ptNetPara->centerNnm, dest_phone_bcd, pBuf, send_len, &sms_send);
            
            /*打印该结构*/
            print_sms_send(&sms_send);

            /*拼短信PDU包*/
            send_pdu_len = ping_send_pdu(sms_send, send_pdu, &len_cmgs);
            
            send_flags = send_pduSms(ifnum, pDev, send_pdu, send_pdu_len, len_cmgs);
        }
        else //TEXT 发送
        {
            send_flags = send_textSms(ifnum, pDev, pBuf, send_len, dest_phone_num);
        }
        if (0 == send_flags)
        {
            MQTTMSG_FMT_DEBUG("短信端口%d 发送短信成功\n", AT_DEV_NUM(ifnum));
            retbool = 1;
        }
        else
        {
            MQTTMSG_FMT_DEBUG("短信端口%d 发送短信失败%d\n", AT_DEV_NUM(ifnum), send_flags);
            retbool = 0;
        }
    }

    /* 构造返回消息 */
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
* @brief     ：gsm接收短信
* @param[in] ：void *pDev
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-10-28
* @Update    :
**********************************************************************/
int wireless_gsm_recv(uint8 ifnum, void *pDev, SMS_PDU_RECV_T *pRecvSms, WIRELESS_HANDLE_T *ptHandle)
{
    int message_numble = 0;    /*短信条数*/
    int message_max = 0;       /*短信最大*/
    int message_len = 0;
    
    message_numble = get_message_num(ifnum, pDev, &message_max); /*读取短信条数*/
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
* @brief     ：gsm收到的将短信返回给发送方
* @param[in] ：MESSAGE_INFO_T *pMsg      收到的消息
               SMS_PDU_RECV_T *pRecvSms  查询到的短信
* @param[out]：   
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-30
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
        /* 计算短信长度占位 */
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
* @brief     ：查询GPS位置
* @param[in] ：uint8 ifnum
               void *pDev
               uint8 bLog
* @param[out]：GPS_T *pGps
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/
int wireless_get_gps(uint8 ifnum, void *pDev, GPS_T *pGps, uint8 bLog)
{
    char *ptemp = NULL;
    int   at_status = 0;
    char  at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
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
$GNGGA 语句的基本格式如下（其中 M 指单位 M，hh 指校验和，CR 和 LF 代表回车换行，下同）：
$GNGGA,(1),(2),(3),(4),(5),(6),(7),(8),(9),M,(10),M,(11),(12)*hh(CR)(LF)
$MYGPSPOS: $GPGGA,021812.000,3156.35964,N,11849.39925,E,1,06,3.2,18.2,M,0.8,M,,*5D
$MYGPSPOS: $GNGGA,120634.000,3156.36386,N,11849.44219,E,1,15,1.1,31.6,M,0.0,M,,*46

1.UTC 时间，格式为 hhmmss.sss；
2.纬度，格式为 ddmm.mmmmm（度分格式）；
3.纬度半球，N 或 S（北纬或南纬）；
4.经度，格式为 dddmm.mmmmm（度分格式）；
5.经度半球，E 或 W（东经或西经）；
6.GPS 状态，定位质量指示，0=定位无效，1=定位有效
7.正在使用的用于定位的卫星数量（00~12）
8.HDOP 水平精确度因子（0.5~99.9）
9.海拔高度（-9999.9 到 9999.9 米）
10.大地水准面高度（-9999.9 到 9999.9 米）
11.差分时间（从最近一次接收到差分信号开始的秒数，非差分定位，此项为空）
12.差分参考基站标号（0000 到 1023，首位 0 也将传送，非差分定位，此项为空)
*/
    /* 获取海拔高度 */
    memset(at_get_buf, 0, sizeof(at_get_buf));
    at_status = at_get_nolog(ifnum, pDev, "AT$MYGPSPOS=0\r\n", at_get_buf, sizeof(at_get_buf));
    if(FALSE == bLog)
    {
        MQTTMSG_FMT_TRACE("设备%d 获取高度信息:\n===============================\n"
                          "发送:AT$MYGPSPOS=0\n接收:%s\n===============================\n", 
                          AT_DEV_NUM(ifnum), at_get_buf);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("设备%d 获取高度信息:\n===============================\n"
                          "发送:AT$MYGPSPOS=0\n接收:%s\n===============================\n"
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
    
    /* 9.海拔高度 */
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
$GPRMC 推荐最小数据量的GPS信息（Recommended Minimum Specific GPS/TRANSIT Data）
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*<13><CR><LF>
$MYGPSPOS: $GPRMC,025944.000,A,3156.36431,N,11849.43602,E,0.00,0.00,200720,,,A,V*19
$MYGPSPOS: $GNRMC,121649.000,A,3156.36225,N,11849.44399,E,0.00,0.00,060820,,,A*72

<1> UTC（Coordinated Universal Time）时间，hhmmss（时分秒）格式
<2> 定位状态，A=有效定位，V=无效定位
<3> Latitude，纬度ddmm.mmmm（度分）格式（前导位数不足则补0）
<4> 纬度半球N（北半球）或S（南半球）
<5> Longitude，经度dddmm.mmmm（度分）格式（前导位数不足则补0）
<6> 经度半球E（东经）或W（西经）
<7> 地面速率（000.0~999.9节，Knot，前导位数不足则补0）
<8> 地面航向（000.0~359.9度，以真北为参考基准，前导位数不足则补0）
<9> UTC日期，ddmmyy（日月年）格式
<10> Magnetic Variation，磁偏角（000.0~180.0度，前导位数不足则补0）
<11> Declination，磁偏角方向，E（东）或W（西）
<12> Mode Indicator，模式指示（仅NMEA0183 3.00版本输出，A=自主定位，D=差分，E=估算，N=数据无效）
<13> 校验和。
*/
    /* 获取位置信息 UTC日期 */
    at_status = at_get_nolog(ifnum, pDev, "AT$MYGPSPOS=3\r\n", at_get_buf, sizeof(at_get_buf));
    if(FALSE == bLog)
    {
        MQTTMSG_FMT_TRACE("设备%d 获取基础信息:\n==============================================\n"
                          "发送:AT$MYGPSPOS=3\n接收:%s\n==============================================\n", 
                          AT_DEV_NUM(ifnum), at_get_buf);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("设备%d 获取基础信息:\n==============================================\n"
                          "发送:AT$MYGPSPOS=3\n接收:%s\n==============================================\n", 
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

    /* 定位方式 */
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

    /* 1. UTC 时间 */
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

    /* 2.定位状态 */
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

    /* 3.纬度 */
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
    /* 4. 纬度半球 */
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

    /* 5.经度 */
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
    /* 6.经度半球 */
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

    /* 7. 地面速率 */
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
    /* 8. 地面航向 */
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
    /* 9. UTC日期 */
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
        /* 星期计算 */
        uint32 d = pGps->utctime.mday, m = pGps->utctime.month, y = pGps->utctime.year;
        pGps->utctime.wday = ((d + 2*m + 3*(m+1)/5 + y + y/4 -y/100 + y/400) + 1) % 7;
        memset(sTemp, '0', i);
    }

    /* 10.磁偏角 */
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

    /* 11. 磁偏角方向 */
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

    /* 12.模式指示 A=自主定位，D=差分，E=估算，N=数据无效 */
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
* @brief     ：查询GPS位置
* @param[in] ：uint8 ifnum
               char *halname
               void *pDev
               uint8 bLog
* @param[out]：GPS_T *pGps
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-5-22
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
        pGPSDEV = wireless_uart_init(halname); /*打开串口*/
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
        MQTTMSG_FMT_TRACE("设备%d 获取GPS信息:\n[%04d-%02d-%02d %02d:%02d:%02d.%03d] status: %d, mode: %d\n"
                          "纬度(%s): %d°%f′\n经度(%s): %d°%f′\n高度: %d cm\n", 
                          AT_DEV_NUM(ifnum), pGps->utctime.year, pGps->utctime.month, pGps->utctime.mday, 
                          pGps->utctime.hour, pGps->utctime.minute, pGps->utctime.second, pGps->utctime.msec,
                          pGps->ststus, pGps->mode, 0 == pGps->position.latitude.position ? "南" : "北",
                          pGps->position.latitude.degre, pGps->position.latitude.branch,
                          0 == pGps->position.longitude.position ? "东" : "西",
                          pGps->position.longitude.degre, pGps->position.longitude.branch, pGps->position.height);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("设备%d 获取GPS信息:\n[%04d-%02d-%02d %02d:%02d:%02d.%03d] status: %d, mode: %d\n"
                          "纬度(%s): %d°%f′\n经度(%s): %d°%f′\n高度: %d cm\n", 
                          AT_DEV_NUM(ifnum), pGps->utctime.year, pGps->utctime.month, pGps->utctime.mday, 
                          pGps->utctime.hour, pGps->utctime.minute, pGps->utctime.second, pGps->utctime.msec,
                          pGps->ststus, pGps->mode, 0 == pGps->position.latitude.position ? "南" : "北",
                          pGps->position.latitude.degre, pGps->position.latitude.branch,
                          0 == pGps->position.longitude.position ? "东" : "西",
                          pGps->position.longitude.degre, pGps->position.longitude.branch, pGps->position.height);
    }

    return ret;
}

/**********************************************************************
* @name      : wireless_gps_back
* @brief     ：GPS返回给发送方
* @param[in] ：MESSAGE_INFO_T *pMsg   收到的消息
               GPS_T *pGps            
* @param[out]：   
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-5-26
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
        
    /* 构造返回消息 */
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
* @brief     ：设置短信中心号码
* @param[in] ：MESSAGE_INFO_T *pMsg        收到的消息
               uint8 ifnum                 设备编号
               void *pDev                  设备句柄
               WIRELESS_HANDLE_T *ptHandle 线程数据句柄 
* @param[out]：   
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-15
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
    char  at_get_buf[GSM_LENGTH];/*AT命令返回内容*/

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
            WIRELESS_FMT_DEBUG("设备%d 设置短信中心号码%s失败\n", AT_DEV_NUM(ifnum), number);
        }
    }
    memset(at_get_buf, 0, sizeof(at_get_buf));
    sprintf(at_get_buf, "AT+CMGF=%d\r\n", type);
    ret = at_check(AT_SMS_TYPE + ifnum, pDev, at_get_buf, "OK");
    if(0 != ret)
    {
        retbool = 0;
        WIRELESS_FMT_DEBUG("设备%d 设置短信模式%s失败\n", AT_DEV_NUM(ifnum), 0 == type ? "PDU" : "TEXT");
    }
    
    if(0 != memcmp(number, ptHandle->ptNetPara->centerNnm, SMS_PDU_PHONE_LEN) || type != ptHandle->ptNetPara->smsType)
    {
        memcpy(ptHandle->ptNetPara->centerNnm, number, SMS_PDU_PHONE_LEN);
        ptHandle->ptNetPara->smsType = type;
        wireless_write_centernum(ptHandle->userfile, ifnum + 1, number, SMS_PDU_PHONE_LEN, type);
    }

    /* 构造返回消息 */
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
* @brief     ：透传AT指令
* @param[in] ：MESSAGE_INFO_T *pMsg        收到的消息
               uint8 ifnum                 设备编号
               void *pDev                  设备句柄
               WIRELESS_HANDLE_T *ptHandle 线程数据句柄
* @param[out]：   
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-1-25
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
    char            at_get_buf[GSM_LENGTH] = {0};/*AT命令返回内容*/
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

    MQTTMSG_FMT_DEBUG("设备%d AT指令透传 发送:\n%s\n", ifnum + 1, at_send_buf);
    ret = at_get_nolog(AT_SMS_TYPE + ifnum, pDev, at_send_buf, at_get_buf, GSM_LENGTH);
    MQTTMSG_FMT_DEBUG("设备%d AT指令透传 接收:\n%s\n", ifnum + 1, at_get_buf);
    free(at_send_buf);
    if(0 == ret)
    {
        getlen = strlen(at_get_buf);
    }
end:
    /* 构造返回消息 */
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
* @brief     ：设置查询GPS模块
* @param[in] ：MESSAGE_INFO_T *pMsg        收到的消息
               uint8 ifnum                 设备编号
               void *pDev                  设备句柄
               WIRELESS_HANDLE_T *ptHandle 线程数据句柄
* @param[out]：   
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-9-26
* @Update    : 
**********************************************************************/
int wireless_set_getGPSmode(uint8 ifnum, void *pDev, WIRELESS_HANDLE_T *ptHandle)
{
    /* GPS定位模式设置 */
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
* @brief     ：查询SIM卡是否在线
* @param[in] ：void *pDev
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-9-27
* @Update    :
**********************************************************************/
int wireless_check_simcard(uint8 ifnum, void *pDev, WIRELESS_HANDLE_T *ptHandle)
{
    int  at_status = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    char *temp = NULL;
    uint8 ccid[21] = {0};
    
    memset(at_get_buf, 0, sizeof(at_get_buf));
    at_status = at_get_nolog(ifnum, pDev, "AT$MYCCID\r\n", at_get_buf, sizeof(at_get_buf)); /*获取SIM卡系列号*/
    if(1 == at_status)
    {
        return 1;
    }
    
    temp = strstr((char *)at_get_buf, "$MYCCID");
    /*若未收到'ERROR'，则SIM卡正常*/
    if (NULL != temp) /*检测SIM卡成功，通知应用层，继续下一步*/
    {
        /* 写入CCID */
        memcpy(ccid, temp+strlen("$MYCCID:")+2, 20);
        if(0 != memcmp(ccid, ptHandle->ptNetPara->ICCID, 20))
        {
            memcpy(ptHandle->ptNetPara->ICCID, ccid, 20);
            WIRELESS_FMT_DEBUG("%s%d CCID变化%s\n", AT_DEV_NAMENUM(ifnum), ccid);
        }
    }
    if (NULL == temp) /*检测SIM卡成功，通知应用层，继续下一步*/
    {
        WIRELESS_FMT_DEBUG("%s%d 检测到SIM卡被拔出, AT$MYCCID:\n%s\n", AT_DEV_NAMENUM(ifnum), at_get_buf);
        return -1;
    }
    return 0;
}

/**********************************************************************
* @name      : wireless_work_bofore_work
* @brief     ：短信线程没有准备好的情况下, 支持操作
* @param[in] ：uint8 ifnum                   网口编号
               WIRELESS_HANDLE_T *ptHandle   句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-12-11
* @Update    :
**********************************************************************/
void wireless_work_bofore_work(uint8 ifnum, WIRELESS_HANDLE_T *ptHandle)
{
    void *pUart = ptHandle->pUart; /*串口句柄*/
    uint8 recvBuf[WIRELESS_BUF_LENGTH] = {0};   /*设备发送缓冲区*/
    int   recvLen = 0;              /*接收数据长度*/
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
* @brief     ：网络注册检查(AT指令、SIM卡拔插、网络注册)
* @param[in] ：uint8 ifnum                  网口编号
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint8 *ftype                 失败类型
               uint8 *cnt                   连续失败次数
* @return    ：0-正常/> 0异常/< 0重启
* @Create    : 王津剑
* @Date      ：2021-1-12
* @Update    :
**********************************************************************/
int wireless_GSM_CHECK(uint8 ifnum, uint8 *ftype, uint8 *cnt, WIRELESS_HANDLE_T *ptHandle)
{
    void  *pUart = ptHandle->pUart;
    int    at_status = 0;
    int    signal_value = 0;
    uint8  network = 0;
    static uint8 nettime[2] = {0};
    
    /* AT指令 */
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

    /* SIM卡 */
    at_status = wireless_check_simcard(AT_SMS_TYPE + ifnum, pUart, ptHandle);
    if(at_status < 0)
    {
        if(GSM_FAILED_SIM != *ftype)
        {
            *cnt = 0;  //SIM卡拔掉快速检测
            *ftype = GSM_FAILED_SIM;
        }
        goto error;
    }

    /* 信号强度 和 网络制式 */
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
        /* 触发事件 */
        nettime[ifnum] = 0;
        ptHandle->ptNetPara->networkType = network;
        ptHandle->ptNetPara->basecsqvalue = signal_value;
        wireless_send_change(ifnum, network, (uint16)signal_value);
    }
    else
    {
        nettime[ifnum] += 1;
    }

    /* 网络注册 */
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
    
    /* 信号强度判断 */
    if(signal_value < MIN_CSQ_VALUE)
    {
        if(GSM_FAILED_CSQ != *ftype)
        {
            *cnt = 0;
            *ftype = GSM_FAILED_CSQ;
        }
        if(1 == (*cnt % 20))
        {
            WIRELESS_FMT_DEBUG("设备%d 信号弱, 强度%d\n", AT_DEV_NUM(ifnum), signal_value);
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
        case GSM_FAILED_REG: return *cnt > 60 ? -1 : 1; /* 60s没有网络 重启模块 */
        case GSM_FAILED_CSQ: return *cnt > 120 ? -1 : 1;      /* 120s网络信号弱 重新拨号 */
        default: break;
    }
    return 0;
}

/**********************************************************************
* @name      : wireless_GSM_INIT
* @brief     ：外置协议栈GSM_INIT 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_INIT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    UART_DEVICE_T* pUart = NULL; /*串口句柄*/
    uint8 devname[256] = {0};
    
    if(NULL != ptHandle->pUart)
    {
        WIRELESS_FMT_DEBUG("短信端口%d 残留句柄释放\n", AT_DEV_NUM(ifnum));
        wireless_uart_uninit(ptHandle->pUart);
        ptHandle->pUart = NULL;
    }

    pUart = (UART_DEVICE_T*)wireless_uart_init(dev); /*打开串口*/
    if (NULL == pUart)
    {
        if(stepCnt > 3)
        {
            WIRELESS_FMT_DEBUG("短信端口%d 多次打开串口%s失败 重启模块!\n", AT_DEV_NUM(ifnum), dev);
            /* 重启设备 */
            return GSM_RESTART;
        }
        *delay = 5000;
        return GSM_INIT;
    }
    pUart->uart_get_real_dev_name(pUart, devname);
    WIRELESS_FMT_DEBUG("短信端口%d 打开串口%s成功(设备%s)\n", AT_DEV_NUM(ifnum), dev, devname);

    ptHandle->pUart = pUart;
    *delay = 1000;
    return GSM_AT;
}

/**********************************************************************
* @name      : wireless_GSM_AT
* @brief     ：外置协议栈GSM_AT 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_AT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; /*串口句柄*/

    /*初始化通讯模块*/
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
                    WIRELESS_FMT_DEBUG("短信端口%d 模块初始化失败, 切换串口!\n", AT_DEV_NUM(ifnum));
                    if(Isusb !=0xAA)
                    {
                        wireless_change_type(WIRELESS_COMMTYPE_UART);
                    }
                }
                else
                {
                    WIRELESS_FMT_DEBUG("短信端口%d 模块初始化失败, 重启模块\n", AT_DEV_NUM(ifnum));
                    /* 重启设备 */
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
* @brief     ：外置协议栈GSM_ATE0 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_ATE0(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    int at_status = 0;
    void *pUart = ptHandle->pUart; /*串口句柄*/

    at_status = at_check(AT_SMS_TYPE + ifnum, pUart, "ATE0\r\n", "OK");
    return 0 == at_status ? GSM_CPIN : GSM_RESTART;
}

/**********************************************************************
* @name      : wireless_GSM_CPIN
* @brief     ：外置协议栈GSM_CPIN 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2022-5-26
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CPIN(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; /*串口句柄*/
    uint8 newifnum = AT_SMS_TYPE + ifnum;
    int   ret = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    
    /*获取SIM卡系列号*/
    if(stepCnt < 1)
    {
        at_get(newifnum, pUart, "AT+CPIN?\r\n", at_get_buf, AT_LENTH); 
        wireless_set_getGPSmode(newifnum, pUart, ptHandle);
    }
    else
    {
        struct timespec curtime = {0};
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        
        /* 没有SIM卡 10s读有一次GPS */
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
        WIRELESS_FMT_DEBUG("短信端口%d 检测到SIM卡插入\n");
        return GSM_CIMI;
    }

    /*若不成功，每隔10s发送一次 等待SIM卡正常*/
    if(stepCnt < 1)
    {
        WIRELESS_FMT_DEBUG("短信端口%d 没有检测到SIM卡 重试\n", AT_DEV_NUM(ifnum));
    }

    *delay = 10000;
    return GSM_CPIN;
}

/**********************************************************************
* @name      : wireless_GSM_CIMI
* @brief     ：外置协议栈GSM_CIMI 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CIMI(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; /*串口句柄*/
    char  at_get_buf[WIRELESS_BUF_LENGTH] = {0};   /*设备发送缓冲区*/
    int   at_status = 0;
    uint8 newifnum = AT_SMS_TYPE + ifnum;

    /*获取SIM卡系列号*/
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
    
    /*若未收到'ERROR'，则SIM卡正常*/
    if(NULL == strstr(at_get_buf, "ERROR")) /*检测SIM卡成功，通知应用层，继续下一步*/
    {
        WIRELESS_FMT_DEBUG("短信端口%d 检测到SIM卡IMSI: %s\n", AT_DEV_NUM(ifnum), at_get_buf);
        wireless_get_IMSI(at_get_buf, ptHandle->ptNetPara->IMSI, 15);
        return GSM_CSQ;
    }
    /*若不成功，每隔10s发送一次 等待SIM卡正常*/
    if(stepCnt < 1)
    {
        WIRELESS_FMT_DEBUG("短信端口%d 没有检测到SIM卡IMSI 重试\n", AT_DEV_NUM(ifnum));
    }

    *delay = 10000;
    return GSM_CIMI;
}

/**********************************************************************
* @name      : wireless_GSM_CSQ
* @brief     ：外置协议栈GSM_CSQ 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CSQ(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_get_buf[WIRELESS_BUF_LENGTH] = {0};   /*设备发送缓冲区*/
    void *pUart = ptHandle->pUart; /*串口句柄*/
    int at_status = 0;
    char *ptemp = NULL;
    int   signal_value = 0;

    /*发送AT+CSQ, 查看信号值*/
    if(stepCnt < 1)
    {
        at_status = at_get(AT_SMS_TYPE + ifnum, pUart, "AT+CSQ\r\n", at_get_buf, WIRELESS_BUF_LENGTH); /*发送AT+CSQ*/
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
    WIRELESS_FMT_DEBUG("短信端口%d 信号强度%d\n", AT_DEV_NUM(ifnum), signal_value);
    if (signal_value >= MIN_CSQ_VALUE)
    {
        ptHandle->ptNetPara->networkType = wireless_get_network(AT_SMS_TYPE + ifnum, pUart, NULL, FALSE);
        wireless_send_change(ifnum, ptHandle->ptNetPara->networkType, (uint16)ptHandle->ptNetPara->csqvalue);
        return GSM_CREG;
    }
    else if(stepCnt >= GSM_GET_CNT)
    {
        WIRELESS_FMT_DEBUG("短信端口%d 信号强度弱, 重启模块\n", AT_DEV_NUM(ifnum));
        return GSM_RESTART;
    }
    *delay = 5000;
    return GSM_CSQ;
}

/**********************************************************************
* @name      : wireless_GSM_CREG
* @brief     ：外置协议栈GSM_CREG 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CREG(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; /*串口句柄*/

    /*发送AT+CREG?，检测SIM是否已注册到网络 */
    if(0 == wireless_check_newwork_reg(AT_SMS_TYPE + ifnum, pUart, stepCnt < 1 ? TRUE : FALSE))
    {
        ptHandle->bsimOK = 0;
        return GSM_CMGF;
    }
    else
    {
        if (stepCnt > 30)       //重启
        {
            WIRELESS_FMT_DEBUG("短信端口%d 检查没有网络注册, 重启\n", AT_DEV_NUM(ifnum));
            /* 重启设备 */
            return GSM_RESTART;
        }
        else
        {
            if(stepCnt < 1)
            {
                WIRELESS_FMT_DEBUG("短信端口%d 检查没有网络注册, 重试\n", AT_DEV_NUM(ifnum));
            }
            *delay = 10000;
            return GSM_CREG;
        }
    }
}

/**********************************************************************
* @name      : wireless_GSM_CMGF
* @brief     ：外置协议栈GSM_CMGF 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CMGF(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_send_buf[WIRELESS_BUF_LENGTH] = {0};   /*设备发送缓冲区*/
    void *pUart = ptHandle->pUart; /*串口句柄*/
    int at_status = 0;

    /*发送AT+CMGF=, 设置短信格式*/
    sprintf(at_send_buf, "AT+CMGF=%d\r\n", ptHandle->ptNetPara->smsType);
    at_status = at_check(AT_SMS_TYPE + ifnum, pUart, at_send_buf, "OK");       /*串口发送AT命令*/
    return (0 == at_status) ? GSM_CPMS : GSM_INIT_END;
}

/**********************************************************************
* @name      : wireless_GSM_CPMS
* @brief     ：外置协议栈GSM_CPMS 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CPMS(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_get_buf[WIRELESS_BUF_LENGTH] = {0};   /*设备发送缓冲区*/
    void *pUart = ptHandle->pUart; /*串口句柄*/
    int at_status = 0;

    /*发送AT+CPMS="ME","ME","MT", 设置短信存储模式*/
    at_status = at_get(AT_SMS_TYPE + ifnum, pUart, "AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n", at_get_buf, WIRELESS_BUF_LENGTH);
    if (0 == at_status)
    {
        WIRELESS_FMT_DEBUG("短信端口%d 初始化成功\n", AT_DEV_NUM(ifnum));
    }
    *delay = 1000;
    return 0 == at_status ? GSM_CSCA : GSM_INIT_END; 
}

/**********************************************************************
* @name      : wireless_GSM_CSCA
* @brief     ：外置协议栈GSM_CSCA 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_CSCA(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_buf[WIRELESS_BUF_LENGTH] = {0};   /*设备发送缓冲区*/
    int at_status = 0;
    void *pUart = ptHandle->pUart; /*串口句柄*/

    /* 短信中心号码设置 */
    if(0 != ptHandle->ptNetPara->centerNnm[0])
    {
        sprintf(at_buf, "AT+CSCA=\"%s\",145\r\n", ptHandle->ptNetPara->centerNnm);
        at_status = at_check(AT_SMS_TYPE + ifnum, pUart, at_buf, "OK");
        if(0 != at_status)
        {
            WIRELESS_FMT_DEBUG("设备%d 设置短信中心号码%s失败\n", AT_DEV_NUM(ifnum), ptHandle->ptNetPara->centerNnm);
        }
    }

    /* 查询短信中心号码 只是查询 当前没用到 */
    memset(at_buf, 0, WIRELESS_BUF_LENGTH);
    at_status = at_get_nolog(AT_SMS_TYPE + ifnum, pUart, "AT+CSCA?\r\n", at_buf, WIRELESS_BUF_LENGTH);
    if(0 == at_status)
    {
        WIRELESS_FMT_DEBUG("设备%d 短信中心号码查询结果%s\n", AT_DEV_NUM(ifnum), at_buf);
    }
    return GSM_GPS; 
}

/**********************************************************************
* @name      : wireless_GSM_GPS
* @brief     ：外置协议栈GSM_GPS 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_GPS(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    if(0 == wireless_get_gpsposition(AT_SMS_TYPE + ifnum, ptHandle->ptNetPara->gpsHalName, ptHandle->pUart, 
                                     ptHandle->ptNetPara->pGps, TRUE))
    {
        ptHandle->bExistGPS = 1;
        WIRELESS_FMT_DEBUG("设备%d 存在卫星定位模块\n", AT_DEV_NUM(ifnum));
    }
    else
    {
        ptHandle->bExistGPS = 0;
        WIRELESS_FMT_DEBUG("设备%d 不存在卫星定位模块\n", AT_DEV_NUM(ifnum));
    }
    return GSM_SUCCESS;
}

/**********************************************************************
* @name      : wireless_AT_RESTART
* @brief     ：外置协议栈GSM_RESTART 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_RESTART(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    if(ptHandle->pUart)
    {
        wireless_uart_uninit(ptHandle->pUart);
        ptHandle->pUart = NULL;
        WIRELESS_FMT_DEBUG("短信端口%d 句柄释放\n", AT_DEV_NUM(ifnum));
    }

    /* 重启设备 */
    wireless_restart_dev(ifnum, ptHandle->commport + ((ptHandle->subport << 4) & 0xf0));

    *delay = 20000;
    ptHandle->workAgain = TRUE; //退出当前状态机
    return GSM_INIT_END;
}

/**********************************************************************
* @name      : wireless_GSM_SUCCESS
* @brief     ：外置协议栈GSM_SUCCESS 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_SUCCESS(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    /* 不变数据获取 */
    wireless_get_phonenum(AT_SMS_TYPE + ifnum, ptHandle->pUart, ptHandle->ptNetPara->SIMNum, 24);

    ptHandle->dialState = PPP_DAIL_STATUS_ONLINE;
    ptHandle->ptNetPara->smsStatusOK = TRUE;
    return GSM_WORK;
}

/**********************************************************************
* @name      : wireless_GSM_END
* @brief     ：外置协议栈GSM_INIT_END 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_END(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    if(ptHandle->pUart)
    {
        wireless_uart_uninit(ptHandle->pUart);
        ptHandle->pUart = NULL;
        WIRELESS_FMT_DEBUG("短信端口%d 句柄释放\n", AT_DEV_NUM(ifnum));
    }

    *delay = 20000;
    ptHandle->workAgain = TRUE; //退出当前状态机
    return GSM_INIT_END;
}

/**********************************************************************
* @name      : wireless_AT_REBOOT
* @brief     ：外置协议栈GSM_AT_REBOOT 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2021-4-8
* @Update    :
**********************************************************************/
uint32 wireless_AT_REBOOT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
#if 0
    void *pUart = ptHandle->pUart; /*串口句柄*/
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
* @brief     ：外置协议栈GSM_WORK 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_GSM_WORK(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void  *pUart = ptHandle->pUart;
    uint8 state_dev = GMS_SEND_DEVICE;            /*设备运行状态*/   

    SMS_PDU_RECV_T recv_sms;
    uint8 send_buf[WIRELESS_BUF_LENGTH];   /*设备发送缓冲区*/
    int    recv_len;              /*接收数据长度*/
    uint32 send_len;              /*发送数据长度*/
    WIRELESS_QUEUE_T *ptMsgQueue = ptHandle->ptMsgQueue;  /* 线程处理消息队列 */
    TIME_STRUCT_T     currentTime = {0};
    TIME_STRUCT_T     lastTime = {0};
    MESSAGE_INFO_T   *ptMsgInfo = NULL;
    uint8             gpstime = WIRELESS_GPS_TIME; //获取GPS定时器
    uint8             aterrorCnt = 0;
    int               at_status = 0;
    uint8             failedType = GSM_FAILED_NULL;
    char             *failedname[4] = {"AT指令", "SIM卡状态", "网络注册", "信号强度"};
    while(1)
    {
        if(FALSE == ptHandle->workFlag || TRUE == ptHandle->workAgain)
        {
            *delay = 1000;
            return GSM_INIT_END;
        }

        /* 一秒统计一次数据, 并判断是否异常 */
        get_curtime_ms(&currentTime);
        currentTime.milliseconds = 0;
        if(0 != memcmp(&currentTime, &lastTime, sizeof(TIME_STRUCT_T)))
        {
            at_status = wireless_GSM_CHECK(ifnum, &failedType, &aterrorCnt, ptHandle);
            if(at_status < 0)
            {
                WIRELESS_FMT_DEBUG("设备%d 连续检测%s异常, 重启模块\n", AT_DEV_NUM(ifnum), 
                                   failedType <= 4 ? failedname[failedType-1] : "其他功能");
                return GSM_RESTART;
            }
            else if(0 == at_status)
            {
                if(gpstime >= WIRELESS_GPS_TIME)
                {
                    gpstime = 0;
                    /* 获取GPS位置 */
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

        /* 通过扫描消息队列来判断消息收发 */
        send_len = read_queue(ptMsgQueue, send_buf);
        if(send_len > 0 && GMS_CLOSE_DEVICE != state_dev)
        {
            ptMsgInfo = (MESSAGE_INFO_T *)send_buf;
            if(MESSAGE_IID_WIRELESS == ptMsgInfo->IID)
            {
                if(WIRELESS_IOP_SEND_SMS == ptMsgInfo->IOP)
                {
                    /* 发送短信 */
                    MQTTMSG_FMT_DEBUG("设备%d 发送短信\n", AT_DEV_NUM(ifnum));
                    state_dev = wireless_gsm_send(AT_SMS_TYPE + ifnum, pUart, ptMsgInfo, ptHandle);
                }
                else if(WIRELESS_IOP_REVC_SMS == ptMsgInfo->IOP)
                {
                    /* 接收短信 */
                    MQTTMSG_FMT_DEBUG("设备%d 接收短信\n", AT_DEV_NUM(ifnum));

                    recv_len = wireless_gsm_recv(AT_SMS_TYPE + ifnum, pUart, &recv_sms, ptHandle);
                    /* 写入缓存队列中, 等待发送 */
                    if(recv_len > 0)
                    {
                        if (check_active_frame(recv_sms.user_data, recv_sms.user_data_len) == 1)
                        {
                            state_dev = GMS_CLOSE_DEVICE;
                            WIRELESS_FMT_DEBUG("设备%d 接收激活短信\n", AT_DEV_NUM(ifnum));
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
                        WIRELESS_FMT_TRACE("设备%d 存在卫星定位模块\n", AT_DEV_NUM(ifnum));
                    }
                    else
                    {
                        ptHandle->bExistGPS = 0;
                        WIRELESS_FMT_TRACE("设备%d 不存在卫星定位模块\n", AT_DEV_NUM(ifnum));
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
            WIRELESS_FMT_DEBUG("设备%d 短信线程退出\n", AT_DEV_NUM(ifnum));
            return GSM_INIT_END;
        }
        usleep(10000);
    }

    return GSM_INIT_END;
}

/**********************************************************************
* @name      : wireless_gsm_working
* @brief     ：gsm工作, 注册、查询、短信收发
* @param[in] ：uint8 commport               端口号
               uint8 ifnum                  网口编号
               WIRELESS_HANDLE_T *ptHandle  线程数据句柄
* @param[out]：
* @return    ：退出等待时间s
* @Create    : 王津剑
* @Date      ：2019-10-23
* @Update    : 2020-11-13
**********************************************************************/
int wireless_gsm_working(uint8 commport, uint8 ifnum, WIRELESS_HANDLE_T *ptHandle)
{
    uint32  i;
    uint32  status = GSM_INIT; /*连接步骤*/
    uint32  stepcnt = 0;       /*状态机次数*/
    uint32  othercount = 0;       /*其他计数次数*/
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

    /* 先清空消息队列 */
    clear_queue(ptHandle->ptMsgQueue);
    while(1)
    {
        /* 异常状态监测 */
        if(FALSE == ptHandle->workFlag || TRUE == ptHandle->workAgain)
        {
            WIRELESS_FMT_DEBUG("短信端口%d 状态异常, 停止工作\n", AT_DEV_NUM(ifnum));
            if(ptHandle->pUart)
            {
                wireless_uart_uninit(ptHandle->pUart);
                ptHandle->pUart = NULL;
                WIRELESS_FMT_DEBUG("短信端口%d 句柄释放\n", AT_DEV_NUM(ifnum));
            }
            return 20;
        }

        /* 支持在所有参数获取之前获取 */
        wireless_work_bofore_work(ifnum, ptHandle);

        /* 状态机切换延时 */
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

        /* 状态机执行 */
        for(i = 0; i < statusNum; i++)
        {
            if(status == statusFuns[i].status)
            {
                if(NULL == statusFuns[i].func)
                {
                    WIRELESS_FMT_DEBUG("短信端口%d 无效状态机 %d, 切换新状态 INIT\n", AT_DEV_NUM(ifnum), status);
                    status = AT_INIT;
                    break;
                }
                if((stepcnt < statusFuns[i].printNum || 0 == statusFuns[i].printNum) && 0 != strlen(statusFuns[i].name))
                {
                     WIRELESS_FMT_DEBUG("短信端口%d 切换状态%s\n", AT_DEV_NUM(ifnum), statusFuns[i].name);
                }
                buflen = strlen(statusFuns[i].info);
                if(0 != buflen && (0 == statusFuns[i].showNum || stepcnt < statusFuns[i].showNum))
                {
                    /* 状态栏刷新 */
                    wireless_send_diagStatus(ifnum + 1, statusFuns[i].info, buflen, statusFuns[i].times);
                }
                
                status = statusFuns[i].func(ifnum, ptHandle->halName, stepcnt, &othercount, ptHandle, &delay);
                ptHandle->stepStatus = status;
                /* 状态连续运行状态 */
                stepcnt = status != statusFuns[i].status ? 0 : (stepcnt + 1);
                break;
            }
        }
        if(i >= statusNum)
        {
            WIRELESS_FMT_DEBUG("短信端口%d 无效状态机 %d, 切换新状态 INIT\n", AT_DEV_NUM(ifnum), status);
            status = AT_INIT;
        }
    }

    return 10;
}

/**********************************************************************
* @name      : wireless_at0_init
* @brief     ：短信0初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
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
* @brief     ：短信1初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
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
* @brief     ：短信处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2021-3-22
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

        sleep(1);  //等待1s 数据同步
        
        delay = wireless_gsm_working(ptHandle->commport, ptHandle->no, ptHandle);

        /* 退出清理 */
        ptHandle->stepStatus = 0;
        ptHandle->dialState = PPP_DAIL_STATUS_OFFLINE;
        ptHandle->ptNetPara->smsStatusOK = FALSE;
        bzero(ptHandle->ptNetPara->IMSI, 16);
        ptHandle->ptNetPara->networkType = OOP_NET_UNKNOW; /* 网络类型 */
        bzero(ptHandle->ptNetPara->SIMNum, 24);  /* SIM 卡号码 */

        ptHandle->ptNetPara->csqvalue = 0;    /* 信号强度 */
        ptHandle->ptNetPara->basecsqvalue = 0;/* 基准信号强度 */

        ptHandle->ptNetPara->pGps->mode = 4;
        ptHandle->ptNetPara->pGps->ststus = 0;
        ptHandle->bExistGPS = 0;
        wireless_send_change(ptHandle->no, WIRELESS_OPT_NONE, 0);

        sleep(delay);
    }
    
    return 1000000;

}

