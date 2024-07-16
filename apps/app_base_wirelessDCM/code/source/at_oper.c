/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ����Զ�̲���ģ��at����
* @date��    2019-10-24
* @history�� 
*********************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
    
#include "dataType.h"
#include "dataCommon.h"
#include "appmsg_struct.h"
#include "message.h"
#include "storage.h"

#include "wireless.h"
#include "at_oper.h"

#define AT_PRINT_TIME(TT)     TT.year+1900, TT.month+1, TT.day, TT.hour, TT.minute, TT.second, TT.milliseconds

#define AT_LOG_1 \
    "%s%d ָ����(%s):\n"\
    "===============================\n"\
    "[%04d-%02d-%02d %02d:%02d:%02d.%03d]����:\n%s\n"\
    "-------------------------------\n"\
    "[%04d-%02d-%02d %02d:%02d:%02d.%03d]����:\n%s\n"\
    "===============================\n"

#define AT_LOG_2 \
    "%s%d ָ�����:\n"\
    "===============================\n"\
    "[%04d-%02d-%02d %02d:%02d:%02d.%03d]����:\n%s\n"\
    "-------------------------------\n"\
    "[%04d-%02d-%02d %02d:%02d:%02d.%03d]����:\n%s\n"\
    "===============================\n"


#define AT_CHECK_LOG(ifnum, CC, T1, B1, T2, B2) \
    WIRELESS_FMT_DEBUG(AT_LOG_1, AT_DEV_NAMENUM(ifnum), CC, AT_PRINT_TIME(T1), B1, AT_PRINT_TIME(T2), B2);

#define AT_GET_LOG(ifnum, T1, B1, T2, B2) \
    WIRELESS_FMT_DEBUG(AT_LOG_2, AT_DEV_NAMENUM(ifnum), AT_PRINT_TIME(T1), B1, AT_PRINT_TIME(T2), B2);

#define AT_CHECK_TRACE_LOG(ifnum, CC, T1, B1, T2, B2) \
        WIRELESS_FMT_TRACE(AT_LOG_1, AT_DEV_NAMENUM(ifnum), CC, AT_PRINT_TIME(T1), B1, AT_PRINT_TIME(T2), B2);
    
#define AT_GET_TRACE_LOG(ifnum, T1, B1, T2, B2) \
        WIRELESS_FMT_TRACE(AT_LOG_2, AT_DEV_NAMENUM(ifnum), AT_PRINT_TIME(T1), B1, AT_PRINT_TIME(T2), B2);

/**********************************************************************
* @name      : at_check
* @brief     ����֤AT�����Ƿ�ɹ� �ܳ�ʱʱ��6s
               ������ֵ��check_buf����ͬ��AT������֤�ɹ�
* @param[in] ��uint8 ifnum     �豸��
               void *pDev      ���ھ��
               char *send_buf  Ҫ���͵�AT����
               char *check_buf ��ȷ��AT�����ֵ
* @param[out]��
* @return    ��0-�ɹ�/������(1-����ʧ��;-1���ʧ��)
* @Create    : ����
* @Date      ��2019-10-24
* @Update    :
**********************************************************************/
int at_check(uint8 ifnum, void *pDev, char *send_buf, char *check_buf)
{
    int8 cnt = 0;        /*����AT����Ĵ���*/
    int  recv_cnt;    /*��at�����ֵ����*/ 
    int  recv_len;    /*AT������ַ�������*/
    char recv_buf[AT_ANSWER_LENGTH] = {0};    /*���AT������ַ����Ļ�����*/    
    uint16 buflen = 0;
    TIME_STRUCT_T starttm;
    TIME_STRUCT_T endtm;
    int ret = 0;

    while (cnt < AT_SEND_CNT)    /*�����ɹ���AT�����ط�AT_CNT��*/
    {
        recv_cnt = 0;
        recv_len = 0;
        buflen = 0;

        /* ��ջ����� */
        wireless_uart_recv(pDev, (uint8 *)recv_buf, AT_ANSWER_LENGTH);
        memset(recv_buf, 0, AT_ANSWER_LENGTH);
        
        get_curtime_ms(&starttm);
        
        ret = wireless_uart_send(pDev, (uint8 *)send_buf, strlen(send_buf));    /*����AT����*/
        if(0 != ret)
        {
            WIRELESS_FMT_DEBUG("%s%d���%s: ����ATָ��%sʧ��\n", AT_DEV_NAMENUM(ifnum), check_buf, send_buf);
            cnt++;
            sleep(1);    /*��AT����أ���ʱ1s�ط�*/
            continue;
        }

        while(recv_cnt < 60)        /*at����ͳ�ȥ��ÿ��100ms��һ�η���ֵ����60��*/
        {
            usleep(100000);
            /*��AT������ַ���*/
            recv_len = wireless_uart_recv(pDev, (uint8 *)recv_buf + buflen, AT_ANSWER_LENGTH - buflen);
            if(recv_len <= 0)
            {
                recv_cnt++;
                continue;
            }
            buflen += recv_len;

            recv_cnt++;
            if (recv_len > 0)
            {
                /*��֤AT����Ӧ���Ƿ���ȷ*/
                if(NULL != strstr(recv_buf, check_buf))
                {   
                    get_curtime_ms(&endtm);
                    AT_CHECK_LOG(ifnum, check_buf, starttm, send_buf, endtm, recv_buf);
                    return 0;
                }
            }
        }
        get_curtime_ms(&endtm);
        AT_CHECK_LOG(ifnum, check_buf, starttm, send_buf, endtm, recv_buf);
        cnt++;
        usleep(10000);    /*��AT����أ���ʱ0.01s�ط�*/
    }
      
    return 0 == ret ? -1 : 1;
}

/**********************************************************************
* @name      : at_check_nolog
* @brief     ����֤AT�����Ƿ�ɹ�
               �����ȷ���AT����ȴ�u_time(us)�󣬶�ȡAT�����ֵ
               ������ֵ��check_buf����ͬ��AT������֤�ɹ�
* @param[in] ��uint8 ifnum     �豸��
               void *pDev       ���ھ��
               char *send_buf   Ҫ���͵�AT����
               char *check_buf  ��ȷ��AT�����ֵ
               int u_time       ����AT����󣬼��u_time(us)�󣬶�ȡAT����ķ���ֵ
* @param[out]��
* @return    ��0-�ɹ�/������(1-����ʧ��;-1���ʧ��)
* @Create    : ����
* @Date      ��2019-10-24
* @Update    :
**********************************************************************/
int at_check_nolog(uint8 ifnum, void *pDev, char *send_buf, char *check_buf)
{
    int8  cnt = 0;        /*����AT����Ĵ���*/
    int32 recv_cnt;    /*��at�����ֵ����*/ 
    int32 recv_len;    /*AT������ַ�������*/
    char  recv_buf[AT_ANSWER_LENGTH] = {0};    /*���AT������ַ����Ļ�����*/    
    uint16 buflen = 0;
    int ret = 0;
    TIME_STRUCT_T starttm;
    TIME_STRUCT_T endtm;

    while (cnt < AT_SEND_CNT)    /*�����ɹ���AT�����ط�AT_CNT��*/
    {
        recv_cnt = 0;
        recv_len = 0;
        buflen = 0;

        /* ��ջ����� */
        wireless_uart_recv(pDev, (uint8 *)recv_buf, AT_ANSWER_LENGTH);
        memset(recv_buf, 0, AT_ANSWER_LENGTH);
        
        get_curtime_ms(&starttm);
        ret = wireless_uart_send(pDev, (uint8 *)send_buf, strlen(send_buf));    /*����AT����*/
        if(0 != ret)
        {
            WIRELESS_FMT_DEBUG("%s%d���%s: ����ATָ��%sʧ��\n", AT_DEV_NAMENUM(ifnum), check_buf, send_buf);
            cnt++;
            sleep(1);    /*��AT����أ���ʱ1s�ط�*/
            continue;
        }

        while(recv_cnt < 60)        /*at����ͳ�ȥ��ÿ��100ms��һ�η���ֵ����60��*/
        {
            usleep(100000);            
            recv_len = wireless_uart_recv(pDev, (uint8 *)recv_buf + buflen, AT_ANSWER_LENGTH - buflen);
            if(recv_len <= 0)
            {
                recv_cnt++;
                continue;
            }
            buflen += recv_len;
            recv_cnt++;
            if (recv_len > 0)
            {
                /*��֤AT����Ӧ���Ƿ���ȷ*/
                if(NULL != strstr(recv_buf, check_buf))
                {
                    get_curtime_ms(&endtm);
                    AT_CHECK_TRACE_LOG(ifnum, check_buf, starttm, send_buf, endtm, recv_buf);
                    return 0;
                }    
            }
        }
        get_curtime_ms(&endtm);
        AT_CHECK_TRACE_LOG(ifnum, check_buf, starttm, send_buf, endtm, recv_buf);
        cnt++;
        usleep(10000);    /*��AT����أ���ʱ0.01s�ط�*/
    }
    return 0 == ret ? -1 : 1;
}


/**********************************************************************
* @name      : at_get
* @brief     ����ȡAT�����ֵ
               �����ȷ���AT����ȴ�100ms�󣬶�ȡAT������ַ���
               ���ַ�������get_buf
* @param[in] ��uint8 ifnum     �豸��
               void *pDev        ���ھ��
               char *send_buf    Ҫ���͵�AT����
               int32 get_buf_len ����������
* @param[out]��char *get_buf     AT����Ӧ���ַ���
* @return    ��0-�ɹ�/������(1-����ʧ��;-1���ʧ��)
* @Create    : ����
* @Date      ��2019-10-25
* @Update    :
**********************************************************************/
int at_get(uint8 ifnum, void *pDev, char *send_buf, char *get_buf, const int32 get_buf_len)
{
    int8 cnt = 0;    /*AT����ʹ���*/
    uint8 recv_buf[AT_ANSWER_LENGTH] = {0};    /*AT������ջ�����*/
    int  recv_len;    /*AT������ճ���*/
    int8 recv_cnt;        /*��ȡat�����ֵ����*/
    uint16 buflen = 0;
    TIME_STRUCT_T starttm;
    TIME_STRUCT_T endtm;
    int ret = 0;

    /* ��ջ����� */
    wireless_uart_recv(pDev, (uint8 *)recv_buf, AT_ANSWER_LENGTH);
    memset(recv_buf, 0, sizeof(recv_buf));    /*����ջ�����*/

    get_curtime_ms(&starttm);
    while (cnt < AT_SEND_CNT)            /*���ɹ��ط�AT_CNT��*/
    {
        ret = wireless_uart_send(pDev, (uint8 *)send_buf, strlen(send_buf));    /*����AT����*/
        if(0 != ret)
        {
            WIRELESS_FMT_DEBUG("%s%d��ȡ: ����ATָ��%sʧ��\n", AT_DEV_NAMENUM(ifnum), send_buf);
            cnt++;
            sleep(1);    /*��AT����أ���ʱ1s�ط�*/
            continue;
        }

        recv_len = 0;
        recv_cnt = 0;
        /*����AT����,��100ms������ڷ�������;���޷���ֵ,ÿ��100ms��һ��,��60��*/
        while(recv_cnt < 60)
        {
            usleep(100000);
            recv_len = wireless_uart_recv(pDev, (uint8 *)recv_buf + buflen, AT_ANSWER_LENGTH - buflen);
            if(recv_len <= 0)
            {
                recv_cnt++;
                continue;
            }
            buflen += recv_len;
            recv_cnt++;

            if (recv_len > 0 && (NULL != strstr((char*)recv_buf, "OK") || NULL != strstr((char*)recv_buf, "ERROR")))
            {
                goto end;
            }
        }
        cnt++;
        usleep(10000);    /*��AT����أ���ʱ0.01s�ط�*/
    }

end:
    get_curtime_ms(&endtm);
    AT_GET_LOG(ifnum, starttm, send_buf, endtm, recv_buf);
    if (buflen > 0)
    {
        /*��Ӧ���ַ�������get_bufָ��Ļ�������*/
        memcpy(get_buf, recv_buf, buflen < get_buf_len ? buflen : get_buf_len);
        return 0;
    }

    return 0 == ret ? -1 : 1;
}

/**********************************************************************
* @name      : at_get_nolog
* @brief     ����ȡAT�����ֵ
               �����ȷ���AT����ȴ�100ms�󣬶�ȡAT������ַ���
               ���ַ�������get_buf
* @param[in] ��uint8 ifnum     �豸��
               void *pDev        ���ھ��
               char *send_buf    Ҫ���͵�AT����
               char *get_buf_len ����������
* @param[out]��char *get_buf     AT����Ӧ���ַ���
* @return    ��0-�ɹ�/������(1-����ʧ��;-1���ʧ��)
* @Create    : ����
* @Date      ��2019-10-25
* @Update    :
**********************************************************************/
int at_get_nolog(uint8 ifnum, void *pDev, char *send_buf, char *get_buf, const int32 get_buf_len)
{
    int8 cnt = 0;    /*AT����ʹ���*/
    uint8 recv_buf[AT_ANSWER_LENGTH] = {0};    /*AT������ջ�����*/
    int  recv_len;    /*AT������ճ���*/
    int8 recv_cnt;        /*��ȡat�����ֵ����*/
    uint16 buflen = 0;
    int ret = 0;
    TIME_STRUCT_T starttm;
    TIME_STRUCT_T endtm;

    /* ��ջ����� */
    wireless_uart_recv(pDev, (uint8 *)recv_buf, AT_ANSWER_LENGTH);
    memset(recv_buf, 0, sizeof(recv_buf));    /*����ջ�����*/

    get_curtime_ms(&starttm);
    while (cnt < AT_SEND_CNT)            /*���ɹ��ط�AT_CNT��*/
    {
        ret = wireless_uart_send(pDev, (uint8 *)send_buf, strlen(send_buf));    /*����AT����*/
        if(0 != ret)
        {
            WIRELESS_FMT_DEBUG("%s%d��ȡ: ����ATָ��%sʧ��\n", AT_DEV_NAMENUM(ifnum), send_buf);
            cnt++;
            sleep(1);    /*��AT����أ���ʱ1s�ط�*/
            continue;
        }

        recv_len = 0;
        recv_cnt = 0;
        /*����AT����,��100ms������ڷ�������;���޷���ֵ,ÿ��100ms��һ��,��60��*/
        while(recv_cnt < 60)
        {
            usleep(100000);
            recv_len = wireless_uart_recv(pDev, (uint8 *)recv_buf + buflen, AT_ANSWER_LENGTH - buflen);
            if(recv_len <= 0)
            {
                recv_cnt++;
                continue;
            }
            buflen += recv_len;
            recv_cnt++;

            if (recv_len > 0 && (NULL != strstr((char*)recv_buf, "OK") || NULL != strstr((char*)recv_buf, "ERROR")))
            {
                goto end;
            }
        }
        cnt++;
        usleep(10000);    /*��AT����أ���ʱ0.01s�ط�*/
    }

end:
    get_curtime_ms(&endtm);
    AT_GET_TRACE_LOG(ifnum, starttm, send_buf, endtm, recv_buf);
    if (buflen > 0)
    {
        /*��Ӧ���ַ�������get_bufָ��Ļ�������*/
        memcpy(get_buf, recv_buf, buflen < get_buf_len ? buflen : get_buf_len);
        return 0;
    }

    return 0 == ret ? -1 : 1;
}

/**********************************************************************
* @name      : at_check_model
* @brief     ������ģ���ܷ�Ӧ��AT����
* @param[in] ��uint8 ifnum       �豸��
               const void *pDev  ���ھ��
               const int at_cnt
* @param[out]��
* @return    ��0/������
* @Create    : ����
* @Date      ��2019-10-23
* @Update    :
**********************************************************************/
int at_check_model(uint8 ifnum, void *pDev, const int at_cnt)
{
    /*�����´򿪵�ʱ�򣬻������ڿ�����һЩ�����ݣ����Խ���buf���󣬿��Խ���������һЩ�������ݶ�����*/
    int  recv_cnt = 0;
    if (at_cnt <= 0)
    {
        return -1;
    }

    while(recv_cnt < at_cnt)
    {
        if(0 == at_check(ifnum, pDev, "AT\r\n", "OK"))
        {
            return 0;
        }
        recv_cnt++;
    }

    return -1;
}

/**********************************************************************
* @name      : at_get_csqvalue
* @brief     ����ȡ+CSQ: 12,99 �е��ź�ֵ��С
* @param[in] ��buf
* @param[out]��
* @return    ���źŴ�С
* @Create    : ����
* @Date      ��2019-10-25
* @Update    :
**********************************************************************/
int at_get_csqvalue(char *buf)
{
    int csq_signal = 0;
    char *ptemp;
    
    csq_signal = 0;
    
    if (buf == NULL)
        return -1;
    
    if ( (ptemp = strstr(buf, "+CSQ:")) != NULL )
    {
        if ( ptemp[5] == ' ')
        {
            if ( (ptemp[7] == ',')&&(ptemp[6] >= '0')&&(ptemp[6] <= '9'))
            {
                csq_signal = ptemp[6] - '0';
            }
            else if((ptemp[8] == ',')&&(ptemp[6] >= '0')&&(ptemp[6] <= '9')&&
                            (ptemp[7] >= '0')&&(ptemp[7] <= '9'))
            {
                csq_signal = 10*(ptemp[6] - '0')+(ptemp[7] - '0');
            }
        }
        else if( (ptemp[5] >= '0')&&(ptemp[5] <= '9') )
        {
            if (ptemp[6] == ',')
            {
                csq_signal = ptemp[5] - '0';
            }
            else if((ptemp[7] == ',')&&(ptemp[6] >= '0')&&(ptemp[6] <= '9'))
            {
                csq_signal = 10*(ptemp[5] - '0')+(ptemp[6] - '0');
            }
        }
    }
    if(csq_signal == 99) /*99: not known or not detected*/
    {
        csq_signal = 0;
    }
    return csq_signal;
}

/**********************************************************************
* @name      : at_get_cesqvalue
* @brief     ����ȡ+CESQ: +CESQ: 42,0,255,255,2,54 ר����CSQ��ֵ
* @param[in] ��buf
* @param[out]��
* @return    ���źŴ�С
* @Create    : ����
* @Date      ��2020-9-17
* @Update    :
**********************************************************************/
int at_get_cesqvalue(char *buf)
{
    int csq_signal = 0;
    char *ptemp;
    
    csq_signal = 0;
    
    if (buf == NULL || NULL == strstr(buf, "+CESQ:"))
        return -1;

    if ( (ptemp = strstr(buf, "+CESQ:")) != NULL )
    {
        if (ptemp[6] == ' ')
        {
            if ((ptemp[8] == ',')&&(ptemp[7] >= '0')&&(ptemp[7] <= '9'))
            {
                csq_signal = ptemp[7] - '0';
            }
            else if((ptemp[9] == ',')&&(ptemp[7] >= '0')&&(ptemp[7] <= '9') && (ptemp[8] >= '0')&&(ptemp[8] <= '9'))
            {
                csq_signal = 10*(ptemp[7] - '0') + (ptemp[8] - '0');
            }
        }
        else if( (ptemp[6] >= '0')&&(ptemp[6] <= '9') )
        {
            if (ptemp[7] == ',')
            {
                csq_signal = ptemp[6] - '0';
            }
            else if((ptemp[8] == ',')&&(ptemp[7] >= '0')&&(ptemp[7] <= '9'))
            {
                csq_signal = 10*(ptemp[6] - '0')+(ptemp[7] - '0');
            }
        }
    }
    if(csq_signal > 63) /* ���63 */
    {
        csq_signal = 0;
    }
    else if(csq_signal >= 2)
    {
        csq_signal = 2 + (csq_signal -2)/2;
    }
    
    return csq_signal;
}


char *at_crlf_deal(char *pCont, char *InfoBuf, int buf_size, char *pMsgBuf)
{
    int buflen = 0;
    char* pStrok = "\r\n";
    char* pCont1 = strstr(pCont, pStrok);

    if(NULL == pCont || NULL == pCont1)
        return NULL;
    buflen = (int)(pCont1 - pCont);
    if(buflen > buf_size)
        return NULL;

    memcpy(InfoBuf, pCont, buflen);
    return pCont1 + 2;
}

/**********************************************************************
* @name      : Get_MYGMR_Information
* @brief     ����ȡ�汾��Ϣ
* @param[in] ��void *pDev          ���ھ��
* @param[out]��MYGMR_INFO_T *buffer
* @return    ��0/������
* @Create    : ����
* @Date      ��2019-10-23
* @Update    :
**********************************************************************/
int at_get_mygmr(uint8 ifnum, void *pDev, MYGMR_INFO_T* buffer)
{
    char  at_get_buf[100];    
    char* pCont = NULL;
    int at_status = 0;

    if(0 == pDev || NULL == buffer)
        return -1;

    memset((void *)buffer, 0x0, sizeof(MYGMR_INFO_T));
    memset(at_get_buf, 0x0, sizeof(at_get_buf));

    at_status = at_get(ifnum, pDev, "AT$MYGMR\r\n", at_get_buf, sizeof(at_get_buf));    /*��ȡSIM��ϵ�к�*/
    if (0 != at_status)
    {
        return -1;
    }

    pCont = at_get_buf + 2;
    //���̴���4�ֽ�
    pCont = at_crlf_deal(pCont, buffer->manufacture, sizeof(buffer->manufacture), "buffer->manufacture");
    if(pCont == NULL)
    {
        return -1;
    }

    //ģ���ͺ� 8 �ֽ�
    pCont = at_crlf_deal(pCont, buffer->model, sizeof(buffer->model), "buffer->model");
    if(pCont == NULL)
    {
        return -1;
    }

    //����汾��4 �ֽ�
    pCont = at_crlf_deal(pCont, buffer->firmare_version, sizeof(buffer->firmare_version), "buffer->firmare_version");
    if(pCont == NULL)
    {
        return -1;
    }

    //�����������6 �ֽ�
    pCont = at_crlf_deal(pCont, buffer->release_date, sizeof(buffer->release_date), "buffer->release_date");
    if(pCont == NULL)
    {
        return -1;
    }

    //Ӳ���汾��4 �ֽ�
    pCont = at_crlf_deal(pCont, buffer->hw_version, sizeof(buffer->hw_version), "buffer->hw_version");
    if(pCont == NULL)
    {
        return -1;
    }

    //Ӳ����������6 �ֽ�
    pCont = at_crlf_deal(pCont, buffer->hw_release_date, sizeof(buffer->hw_release_date), "buffer->hw_release_date");
    if(pCont == NULL)
    {
        return -1;
    }
    return 0;
}

/**********************************************************************
* @name      : at_get_module_type
* @brief     ����ȡ����ģ���ͺ�
* @param[in] ��buf       �ַ���������
* @param[out]��rettype   �ַ���
* @return    ����0λ��1: GSM����֧��GPRS
               ��1λ��1: WCDMA
               ��2λ��1: TD_SCDMA
               ��3λ��1: CDMA2000
               ��4λ��1: CDMAEVDO
               ��5λ��1: LTE
               ��6λ��1: PSTN
               ��7λ��1: ��ʾ��չ1���ֽ�
* @Create    : 
* @Date      ��2019-10-25
* @Update    :
**********************************************************************/
uint8 at_get_module_type(char* buf, char *rettype)
{
    uint8 module_type = 0;
    char *cur_pos = 0;
    uint8 i = 0;
    
    if(!strstr(buf, "$MYTYPE:"))
    {
        return 0;
    }
    if(strstr(buf, "P7"))//SIMCOMM��4Gģ���������ͷ��ص���P7
    {
        rettype[0] = '2';
        rettype[1] = '5';
        return 0x25;
    }
    cur_pos = strstr(buf, ",");
    if(cur_pos)
    {
        cur_pos+=1;
        while(*cur_pos != ',' && i < 2)
        {
            rettype[i] = *cur_pos;
            if(*cur_pos >= '0' && *cur_pos <= '9')
            {
                module_type = (module_type<<4)+(*cur_pos - '0');
            }
            else if(*cur_pos >= 'a' && *cur_pos <= 'f')
            {
                module_type = (module_type<<4)+(*cur_pos - 'a') + 10;
            }
            else if(*cur_pos >= 'A' && *cur_pos <= 'F')
            {
                module_type = (module_type<<4)+(*cur_pos - 'A') + 10;
            }
            else
            {
                break;
            }
            cur_pos++;
            i++;
        }
    }
    return module_type;
}

/**********************************************************************
* @name      : wireless_get_csqvalue
* @brief     ����ѯ�ź�ǿ��
* @param[in] ��void *pDev
* @param[out]��
* @return    ���ź�ǿ��
* @Create    : ����
* @Date      ��2019-10-28
* @Update    :
**********************************************************************/
int wireless_get_csqvalue(uint8 ifnum, void *pDev)
{
    char               *ptemp;
    int             at_status = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    int              csqvalue = 0;

    at_status = at_get_nolog(ifnum, pDev, "AT+CSQ\r\n", at_get_buf, sizeof(at_get_buf)); /*����AT+CSQ*/
    if (0 != at_status)
    {
        return -1;
    }
    ptemp = strstr((char *)at_get_buf, "+CSQ:");
    csqvalue = at_get_csqvalue(ptemp);
    if(csqvalue <= 0)
    {
        /* SA��������Ҫʹ��CESQ��ѯǿ�ȣ���֧��CSQ */
        memset(at_get_buf, 0, sizeof(at_get_buf));
        at_status = at_get_nolog(ifnum, pDev, "AT+CESQ\r\n", at_get_buf, sizeof(at_get_buf)); /*����AT+CESQ*/
        if (0 != at_status)
        {
            return -1;
        }
        ptemp = strstr((char *)at_get_buf, "+CESQ:");
        csqvalue = at_get_cesqvalue(ptemp);
    }
    return csqvalue;
}

/*
AcT ���뼼��                     ����
0�� GSM                          2G
1�� GSM compact                  2G
2�� UTRAN                        3G
3�� GSM w/EGPRS                  2G
4�� UTRAN w/HSDPA                3G
5�� UTRAN w/HSUPA                3G
6�� UTRAN w/HSDPA and HSUPA      3G
7�� E-UTRAN                      4G
8:  EC-GSM_IOT                   5G(������)
9�� E-UTRAN                      5G(������)
10��E-UTRA connected to a 5GCN   5G
11��NR connected to a 5GCN       5G SA
12��NG-RAN                       5G
13��E-UTRA-NR dual connectivity  5G NSA
*/
uint8 get_network_type(char *pBuf)
{
    //"+COPS: 0,0,\"CHINA MOBILE\",7\r\nOK\r\n"
    uint16 len;
    uint16 i;
    uint8  count = 0;
    char  *temp = pBuf;
    int    type = 0xff;

    if(NULL == pBuf)
    {
        return OOP_NET_UNKNOW;
    }
    
    len = strlen(pBuf);
    for(i= 0; i < len; i++, temp++)
    {
        if(',' == *temp)
        {
            count++;
        }
        if(3 == count)
        {
            temp++;
            type = *temp - '0';
            temp++;
            if('0' <= *temp && *temp <= '9')
            {
                type = type * 10 + (*temp - '0');
            }
            break;
        }
    }

    if(0 == type || 1 == type || 9 == type || 3 == type)
    {
        return OOP_2G;
    }
    else if(2 == type || 4 == type || 5 == type || 6 == type)
    {
        return OOP_3G;
    }
    else if(7 == type)
    {
        return OOP_4G;
    }
    else if(8 <= type && type <= 13)
    {
        return OOP_5G;
    }
    return OOP_NET_UNKNOW;
}

/**********************************************************************
* @name      : wireless_get_network
* @brief     ����ѯ��������
* @param[in] ��void *pDev
* @param[out]��
* @return    ����������
* @Create    : ����
* @Date      ��2020-4-1
* @Update    :
**********************************************************************/
uint8 wireless_get_network(uint8 ifnum, void *pDev, uint8 *regOperat, uint8 bLog)
{
    char               *ptemp;
    int             at_status = 0;
    uint8             service = 0;
    int                retNet = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/

    if(TRUE == bLog)
    {
        at_get(ifnum, pDev, "AT$MYSYSINFO\r\n", at_get_buf, sizeof(at_get_buf));
    }
    else
    {
        at_get_nolog(ifnum, pDev, "AT$MYSYSINFO\r\n", at_get_buf, sizeof(at_get_buf));
    }
    
    if (0 != at_status)
    {
        return OOP_NET_UNKNOW;
    }
    
    ptemp = strstr((char *)at_get_buf, "$MYSYSINFO: ");
    if(NULL == ptemp)
    {
        return OOP_NET_UNKNOW;
    }
    ptemp += strlen("$MYSYSINFO: ");
    service = atoi(ptemp);
    retNet = atoi(ptemp + 2);

    if(regOperat)
    {
        switch(retNet)
        {
            case 0: *regOperat = WIRELESS_OPT_NONE; break;
            case 1: *regOperat = WIRELESS_CMCC;     break;
            case 2: *regOperat = WIRELESS_CUCC;     break;
            case 3: *regOperat = WIRELESS_CTCC;     break;
            default: *regOperat = WIRELESS_ZWCC;    break;
        }
    }
    
    if(2 == service)
    {
         return OOP_2G;
    }
    else if(3 == service)
    {
         return OOP_3G;
    }
    else if(4 == service)
    {
         return OOP_4G;
    }
    else if(5 == service)
    {
         return OOP_5G;
    }
    else
    {
        return OOP_NET_UNKNOW;
    }
}

/**********************************************************************
* @name      : wireless_get_phonenum
* @brief     ����ѯ�ֻ���
* @param[in] ��void *pDev
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2019-10-28
* @Update    :
**********************************************************************/
int wireless_get_phonenum(uint8 ifnum, void *pDev, uint8 *pBuf, uint8 Len)
{
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    char   *ptemp;
    uint8 i = 0;
    uint16 start, len = 0;
    memset(at_get_buf, 0, sizeof(at_get_buf));
    memset(pBuf, 0, Len);

    at_get(ifnum, pDev, "AT+CNUM?\r\n", at_get_buf, sizeof(at_get_buf)); /*����AT+CSQ*/
    ptemp = strstr(at_get_buf, "+CNUM:");
    if(NULL == ptemp)
    {    
        memset(at_get_buf, 0, sizeof(at_get_buf));
        at_get(ifnum, pDev, "AT+CNUM\r\n", at_get_buf, sizeof(at_get_buf)); /*����AT+CSQ*/
        ptemp = strstr(at_get_buf, "+CNUM:");
    }

    if(NULL != strstr(at_get_buf, "ERROR"))
    {
        memset(pBuf, 0, Len);
    }

    if(NULL != ptemp)
    {
        len = strlen(ptemp);
        start = 5;
        while(start < len)
        {
            if(',' == ptemp[start])
            {
                start++;
                break;
            }
            start++;
        }
        
        for(; start < len; start++)
        {
            if(',' == ptemp[start] || i + 1 >= Len)
            {
                break;
            }
            if('+' == ptemp[start] || ('0' <= ptemp[start] && ptemp[start] <= '9'))
            {
                pBuf[i] = ptemp[start];
                i++;
                break;
            }
        }
    }
    else
    {
        memset(pBuf, 0, Len);
    }
    return 0;
}

/**********************************************************************
* @name      : wireless_get_IMSI
* @brief     ������IMSI
* @param[in] ��char *pRecv  ���յ���Ϣ
               uint8 buflen �����С
* @param[out]��uint8 *pBuf  ����
* @return    ��
* @Create    : ����
* @Date      ��2020-5-3
* @Update    :
**********************************************************************/
void wireless_get_IMSI(char *pRecv, uint8 *pBuf, uint8 buflen)
{
    uint8 i = 0;
    uint8 start = 0;
    uint8 len = strlen(pRecv);

    for(start = 0; start < len; start++)
    {
        if('0' <= pRecv[start] && pRecv[start] <= '9')
        {
            for(i = 0; i < buflen && i + start < len; i++)
            {
                if('0' <= pRecv[start + i] && pRecv[start + i] <= '9')
                {
                    pBuf[i] = pRecv[start + i];
                }
                else
                {
                    break;
                }
            }
            break;
        }
    }

}

/**********************************************************************
* @name      : wireless_get_IMEI
* @brief     ����ѯIMEI
* @param[in] ��void *pDev
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2019-10-28
* @Update    :
**********************************************************************/
int wireless_get_IMEI(uint8 ifnum, void *pDev, char *pBuf, uint8 Len)
{
    char               *ptemp = NULL;
    int             at_status = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    uint8   i = 0;
    memset(at_get_buf, 0, sizeof(at_get_buf));
    at_status = at_get(ifnum, pDev, "AT+CGSN\r\n", at_get_buf, sizeof(at_get_buf)); /*����AT+CSQ*/
    if (0 != at_status)
    {
        return -1;
    }

    ptemp = at_get_buf;
    while(*ptemp && i < Len)
    {
        if('0' <= *ptemp && *ptemp <= '9')
        {
            pBuf[i] = *ptemp;
            i++;
        }
        ptemp++;
    }
    
    return 0;
}

/**********************************************************************
* @name      : wireless_check_newwork_reg
* @brief     ����ѯ�����Ƿ�ע��ɹ�
* @param[in] ��uint8 ifnum
               void *pDev
* @param[out]��
* @return    ��0-�ɹ�/����û��ע��ɹ�
* @Create    : ����
* @Date      ��2020-10-14
* @Update    :
**********************************************************************/
int wireless_check_newwork_reg(uint8 ifnum, void *pDev, uint8 bLog)
{
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/

    /* CREG��CGREG��CEREG����֮һע��ɹ����ʾ����ע��ɹ� */
    if(bLog)
    {
        at_get(ifnum, pDev, "AT+CREG?\r\n", at_get_buf, sizeof(at_get_buf));
    }
    else
    {
        at_get_nolog(ifnum, pDev, "AT+CREG?\r\n", at_get_buf, sizeof(at_get_buf));
    }
    /*���յ����ַ����Ƿ�Ϊ +CREG: *,1*/
    if ((is_str_exist((char *)at_get_buf, "0,1") == 0)||(is_str_exist((char *)at_get_buf, "0,5") == 0) ||
        (is_str_exist((char *)at_get_buf, "1,1") == 0)||(is_str_exist((char *)at_get_buf, "1,5") == 0) ||
        (is_str_exist((char *)at_get_buf, "2,1") == 0)||(is_str_exist((char *)at_get_buf, "2,5") == 0))
    {
        return 0;
    }

    usleep(10000);
    memset(at_get_buf, 0, AT_LENTH);
    if(bLog)
    {
        at_get(ifnum, pDev, "AT+CGREG?\r\n", at_get_buf, sizeof(at_get_buf));
    }
    else
    {
        at_get_nolog(ifnum, pDev, "AT+CGREG?\r\n", at_get_buf, sizeof(at_get_buf));
    }
    if ((is_str_exist((char *)at_get_buf, "0,1") == 0)||(is_str_exist((char *)at_get_buf, "0,5") == 0) ||
        (is_str_exist((char *)at_get_buf, "1,1") == 0)||(is_str_exist((char *)at_get_buf, "1,5") == 0) ||
        (is_str_exist((char *)at_get_buf, "2,1") == 0)||(is_str_exist((char *)at_get_buf, "2,5") == 0))
    {
        return 0;
    }
    
    usleep(10000);
    memset(at_get_buf, 0, AT_LENTH);
    if(bLog)
    {
        at_get(ifnum, pDev, "AT+CEREG?\r\n", at_get_buf, sizeof(at_get_buf));
    }
    else
    {
        at_get_nolog(ifnum, pDev, "AT+CEREG?\r\n", at_get_buf, sizeof(at_get_buf));
    }
    if ((is_str_exist((char *)at_get_buf, "0,1") == 0)||(is_str_exist((char *)at_get_buf, "0,5") == 0) ||
        (is_str_exist((char *)at_get_buf, "1,1") == 0)||(is_str_exist((char *)at_get_buf, "1,5") == 0) ||
        (is_str_exist((char *)at_get_buf, "2,1") == 0)||(is_str_exist((char *)at_get_buf, "2,5") == 0))
    {
        return 0;
    }

    usleep(10000);
    memset(at_get_buf, 0, AT_LENTH);
    if(bLog)
    {
        at_get(ifnum, pDev, "AT+C5GREG?\r\n", at_get_buf, sizeof(at_get_buf));
    }
    else
    {
        at_get_nolog(ifnum, pDev, "AT+C5GREG?\r\n", at_get_buf, sizeof(at_get_buf));
    }
    if ((is_str_exist((char *)at_get_buf, "0,1") == 0)||(is_str_exist((char *)at_get_buf, "0,5") == 0) ||
        (is_str_exist((char *)at_get_buf, "1,1") == 0)||(is_str_exist((char *)at_get_buf, "1,5") == 0) ||
        (is_str_exist((char *)at_get_buf, "2,1") == 0)||(is_str_exist((char *)at_get_buf, "2,5") == 0))
    {
        return 0;
    }

    return 1;  
}

