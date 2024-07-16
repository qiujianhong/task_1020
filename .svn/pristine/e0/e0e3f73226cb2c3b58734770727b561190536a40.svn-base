/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块at操作
* @date：    2019-10-24
* @history： 
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
    "%s%d 指令检查(%s):\n"\
    "===============================\n"\
    "[%04d-%02d-%02d %02d:%02d:%02d.%03d]发送:\n%s\n"\
    "-------------------------------\n"\
    "[%04d-%02d-%02d %02d:%02d:%02d.%03d]接收:\n%s\n"\
    "===============================\n"

#define AT_LOG_2 \
    "%s%d 指令操作:\n"\
    "===============================\n"\
    "[%04d-%02d-%02d %02d:%02d:%02d.%03d]发送:\n%s\n"\
    "-------------------------------\n"\
    "[%04d-%02d-%02d %02d:%02d:%02d.%03d]接收:\n%s\n"\
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
* @brief     ：验证AT命令是否成功 总超时时间6s
               若返回值与check_buf中相同，AT命令验证成功
* @param[in] ：uint8 ifnum     设备号
               void *pDev      串口句柄
               char *send_buf  要发送的AT命令
               char *check_buf 正确的AT命令返回值
* @param[out]：
* @return    ：0-成功/错误码(1-发送失败;-1检查失败)
* @Create    : 王津剑
* @Date      ：2019-10-24
* @Update    :
**********************************************************************/
int at_check(uint8 ifnum, void *pDev, char *send_buf, char *check_buf)
{
    int8 cnt = 0;        /*发送AT命令的次数*/
    int  recv_cnt;    /*读at命令返回值次数*/ 
    int  recv_len;    /*AT命令返回字符串长度*/
    char recv_buf[AT_ANSWER_LENGTH] = {0};    /*存放AT命令返回字符串的缓冲区*/    
    uint16 buflen = 0;
    TIME_STRUCT_T starttm;
    TIME_STRUCT_T endtm;
    int ret = 0;

    while (cnt < AT_SEND_CNT)    /*若不成功，AT命令重发AT_CNT次*/
    {
        recv_cnt = 0;
        recv_len = 0;
        buflen = 0;

        /* 清空缓冲区 */
        wireless_uart_recv(pDev, (uint8 *)recv_buf, AT_ANSWER_LENGTH);
        memset(recv_buf, 0, AT_ANSWER_LENGTH);
        
        get_curtime_ms(&starttm);
        
        ret = wireless_uart_send(pDev, (uint8 *)send_buf, strlen(send_buf));    /*发送AT命令*/
        if(0 != ret)
        {
            WIRELESS_FMT_DEBUG("%s%d检查%s: 发送AT指令%s失败\n", AT_DEV_NAMENUM(ifnum), check_buf, send_buf);
            cnt++;
            sleep(1);    /*若AT命令不回，延时1s重发*/
            continue;
        }

        while(recv_cnt < 60)        /*at命令发送出去后，每隔100ms读一次返回值，共60次*/
        {
            usleep(100000);
            /*读AT命令返回字符串*/
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
                /*验证AT命令应答是否正确*/
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
        usleep(10000);    /*若AT命令不回，延时0.01s重发*/
    }
      
    return 0 == ret ? -1 : 1;
}

/**********************************************************************
* @name      : at_check_nolog
* @brief     ：验证AT命令是否成功
               串口先发送AT命令，等待u_time(us)后，读取AT命令返回值
               若返回值与check_buf中相同，AT命令验证成功
* @param[in] ：uint8 ifnum     设备号
               void *pDev       串口句柄
               char *send_buf   要发送的AT命令
               char *check_buf  正确的AT命令返回值
               int u_time       发送AT命令后，间隔u_time(us)后，读取AT命令的返回值
* @param[out]：
* @return    ：0-成功/错误码(1-发送失败;-1检查失败)
* @Create    : 王津剑
* @Date      ：2019-10-24
* @Update    :
**********************************************************************/
int at_check_nolog(uint8 ifnum, void *pDev, char *send_buf, char *check_buf)
{
    int8  cnt = 0;        /*发送AT命令的次数*/
    int32 recv_cnt;    /*读at命令返回值次数*/ 
    int32 recv_len;    /*AT命令返回字符串长度*/
    char  recv_buf[AT_ANSWER_LENGTH] = {0};    /*存放AT命令返回字符串的缓冲区*/    
    uint16 buflen = 0;
    int ret = 0;
    TIME_STRUCT_T starttm;
    TIME_STRUCT_T endtm;

    while (cnt < AT_SEND_CNT)    /*若不成功，AT命令重发AT_CNT次*/
    {
        recv_cnt = 0;
        recv_len = 0;
        buflen = 0;

        /* 清空缓冲区 */
        wireless_uart_recv(pDev, (uint8 *)recv_buf, AT_ANSWER_LENGTH);
        memset(recv_buf, 0, AT_ANSWER_LENGTH);
        
        get_curtime_ms(&starttm);
        ret = wireless_uart_send(pDev, (uint8 *)send_buf, strlen(send_buf));    /*发送AT命令*/
        if(0 != ret)
        {
            WIRELESS_FMT_DEBUG("%s%d检查%s: 发送AT指令%s失败\n", AT_DEV_NAMENUM(ifnum), check_buf, send_buf);
            cnt++;
            sleep(1);    /*若AT命令不回，延时1s重发*/
            continue;
        }

        while(recv_cnt < 60)        /*at命令发送出去后，每隔100ms读一次返回值，共60次*/
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
                /*验证AT命令应答是否正确*/
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
        usleep(10000);    /*若AT命令不回，延时0.01s重发*/
    }
    return 0 == ret ? -1 : 1;
}


/**********************************************************************
* @name      : at_get
* @brief     ：获取AT命令返回值
               串口先发送AT命令，等待100ms后，读取AT命令返回字符串
               将字符串放入get_buf
* @param[in] ：uint8 ifnum     设备号
               void *pDev        串口句柄
               char *send_buf    要发送的AT命令
               int32 get_buf_len 缓冲区长度
* @param[out]：char *get_buf     AT命令应答字符串
* @return    ：0-成功/错误码(1-发送失败;-1检查失败)
* @Create    : 王津剑
* @Date      ：2019-10-25
* @Update    :
**********************************************************************/
int at_get(uint8 ifnum, void *pDev, char *send_buf, char *get_buf, const int32 get_buf_len)
{
    int8 cnt = 0;    /*AT命令发送次数*/
    uint8 recv_buf[AT_ANSWER_LENGTH] = {0};    /*AT命令接收缓冲区*/
    int  recv_len;    /*AT命令接收长度*/
    int8 recv_cnt;        /*读取at命令返回值次数*/
    uint16 buflen = 0;
    TIME_STRUCT_T starttm;
    TIME_STRUCT_T endtm;
    int ret = 0;

    /* 清空缓冲区 */
    wireless_uart_recv(pDev, (uint8 *)recv_buf, AT_ANSWER_LENGTH);
    memset(recv_buf, 0, sizeof(recv_buf));    /*清接收缓冲区*/

    get_curtime_ms(&starttm);
    while (cnt < AT_SEND_CNT)            /*不成功重发AT_CNT次*/
    {
        ret = wireless_uart_send(pDev, (uint8 *)send_buf, strlen(send_buf));    /*发送AT命令*/
        if(0 != ret)
        {
            WIRELESS_FMT_DEBUG("%s%d获取: 发送AT指令%s失败\n", AT_DEV_NAMENUM(ifnum), send_buf);
            cnt++;
            sleep(1);    /*若AT命令不回，延时1s重发*/
            continue;
        }

        recv_len = 0;
        recv_cnt = 0;
        /*发送AT命令,等100ms后读串口返回内容;若无返回值,每隔100ms读一次,读60次*/
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
        usleep(10000);    /*若AT命令不回，延时0.01s重发*/
    }

end:
    get_curtime_ms(&endtm);
    AT_GET_LOG(ifnum, starttm, send_buf, endtm, recv_buf);
    if (buflen > 0)
    {
        /*将应答字符串放入get_buf指向的缓冲区中*/
        memcpy(get_buf, recv_buf, buflen < get_buf_len ? buflen : get_buf_len);
        return 0;
    }

    return 0 == ret ? -1 : 1;
}

/**********************************************************************
* @name      : at_get_nolog
* @brief     ：获取AT命令返回值
               串口先发送AT命令，等待100ms后，读取AT命令返回字符串
               将字符串放入get_buf
* @param[in] ：uint8 ifnum     设备号
               void *pDev        串口句柄
               char *send_buf    要发送的AT命令
               char *get_buf_len 缓冲区长度
* @param[out]：char *get_buf     AT命令应答字符串
* @return    ：0-成功/错误码(1-发送失败;-1检查失败)
* @Create    : 王津剑
* @Date      ：2019-10-25
* @Update    :
**********************************************************************/
int at_get_nolog(uint8 ifnum, void *pDev, char *send_buf, char *get_buf, const int32 get_buf_len)
{
    int8 cnt = 0;    /*AT命令发送次数*/
    uint8 recv_buf[AT_ANSWER_LENGTH] = {0};    /*AT命令接收缓冲区*/
    int  recv_len;    /*AT命令接收长度*/
    int8 recv_cnt;        /*读取at命令返回值次数*/
    uint16 buflen = 0;
    int ret = 0;
    TIME_STRUCT_T starttm;
    TIME_STRUCT_T endtm;

    /* 清空缓冲区 */
    wireless_uart_recv(pDev, (uint8 *)recv_buf, AT_ANSWER_LENGTH);
    memset(recv_buf, 0, sizeof(recv_buf));    /*清接收缓冲区*/

    get_curtime_ms(&starttm);
    while (cnt < AT_SEND_CNT)            /*不成功重发AT_CNT次*/
    {
        ret = wireless_uart_send(pDev, (uint8 *)send_buf, strlen(send_buf));    /*发送AT命令*/
        if(0 != ret)
        {
            WIRELESS_FMT_DEBUG("%s%d获取: 发送AT指令%s失败\n", AT_DEV_NAMENUM(ifnum), send_buf);
            cnt++;
            sleep(1);    /*若AT命令不回，延时1s重发*/
            continue;
        }

        recv_len = 0;
        recv_cnt = 0;
        /*发送AT命令,等100ms后读串口返回内容;若无返回值,每隔100ms读一次,读60次*/
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
        usleep(10000);    /*若AT命令不回，延时0.01s重发*/
    }

end:
    get_curtime_ms(&endtm);
    AT_GET_TRACE_LOG(ifnum, starttm, send_buf, endtm, recv_buf);
    if (buflen > 0)
    {
        /*将应答字符串放入get_buf指向的缓冲区中*/
        memcpy(get_buf, recv_buf, buflen < get_buf_len ? buflen : get_buf_len);
        return 0;
    }

    return 0 == ret ? -1 : 1;
}

/**********************************************************************
* @name      : at_check_model
* @brief     ：测试模块能否应答AT命令
* @param[in] ：uint8 ifnum       设备号
               const void *pDev  串口句柄
               const int at_cnt
* @param[out]：
* @return    ：0/错误码
* @Create    : 王津剑
* @Date      ：2019-10-23
* @Update    :
**********************************************************************/
int at_check_model(uint8 ifnum, void *pDev, const int at_cnt)
{
    /*串口新打开的时候，缓冲区内可能有一些乱数据，所以将该buf开大，可以将缓冲区内一些垃圾数据读出来*/
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
* @brief     ：读取+CSQ: 12,99 中的信号值大小
* @param[in] ：buf
* @param[out]：
* @return    ：信号大小
* @Create    : 王津剑
* @Date      ：2019-10-25
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
* @brief     ：读取+CESQ: +CESQ: 42,0,255,255,2,54 专换成CSQ的值
* @param[in] ：buf
* @param[out]：
* @return    ：信号大小
* @Create    : 王津剑
* @Date      ：2020-9-17
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
    if(csq_signal > 63) /* 最大63 */
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
* @brief     ：获取版本信息
* @param[in] ：void *pDev          串口句柄
* @param[out]：MYGMR_INFO_T *buffer
* @return    ：0/错误码
* @Create    : 王津剑
* @Date      ：2019-10-23
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

    at_status = at_get(ifnum, pDev, "AT$MYGMR\r\n", at_get_buf, sizeof(at_get_buf));    /*获取SIM卡系列号*/
    if (0 != at_status)
    {
        return -1;
    }

    pCont = at_get_buf + 2;
    //厂商代号4字节
    pCont = at_crlf_deal(pCont, buffer->manufacture, sizeof(buffer->manufacture), "buffer->manufacture");
    if(pCont == NULL)
    {
        return -1;
    }

    //模块型号 8 字节
    pCont = at_crlf_deal(pCont, buffer->model, sizeof(buffer->model), "buffer->model");
    if(pCont == NULL)
    {
        return -1;
    }

    //软件版本号4 字节
    pCont = at_crlf_deal(pCont, buffer->firmare_version, sizeof(buffer->firmare_version), "buffer->firmare_version");
    if(pCont == NULL)
    {
        return -1;
    }

    //软件发布日期6 字节
    pCont = at_crlf_deal(pCont, buffer->release_date, sizeof(buffer->release_date), "buffer->release_date");
    if(pCont == NULL)
    {
        return -1;
    }

    //硬件版本号4 字节
    pCont = at_crlf_deal(pCont, buffer->hw_version, sizeof(buffer->hw_version), "buffer->hw_version");
    if(pCont == NULL)
    {
        return -1;
    }

    //硬件发布日期6 字节
    pCont = at_crlf_deal(pCont, buffer->hw_release_date, sizeof(buffer->hw_release_date), "buffer->hw_release_date");
    if(pCont == NULL)
    {
        return -1;
    }
    return 0;
}

/**********************************************************************
* @name      : at_get_module_type
* @brief     ：获取无线模块型号
* @param[in] ：buf       字符串缓冲区
* @param[out]：rettype   字符串
* @return    ：第0位置1: GSM网络支持GPRS
               第1位置1: WCDMA
               第2位置1: TD_SCDMA
               第3位置1: CDMA2000
               第4位置1: CDMAEVDO
               第5位置1: LTE
               第6位置1: PSTN
               第7位置1: 表示扩展1个字节
* @Create    : 
* @Date      ：2019-10-25
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
    if(strstr(buf, "P7"))//SIMCOMM的4G模块网络类型返回的是P7
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
* @brief     ：查询信号强度
* @param[in] ：void *pDev
* @param[out]：
* @return    ：信号强度
* @Create    : 王津剑
* @Date      ：2019-10-28
* @Update    :
**********************************************************************/
int wireless_get_csqvalue(uint8 ifnum, void *pDev)
{
    char               *ptemp;
    int             at_status = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    int              csqvalue = 0;

    at_status = at_get_nolog(ifnum, pDev, "AT+CSQ\r\n", at_get_buf, sizeof(at_get_buf)); /*发送AT+CSQ*/
    if (0 != at_status)
    {
        return -1;
    }
    ptemp = strstr((char *)at_get_buf, "+CSQ:");
    csqvalue = at_get_csqvalue(ptemp);
    if(csqvalue <= 0)
    {
        /* SA网络中需要使用CESQ查询强度，不支持CSQ */
        memset(at_get_buf, 0, sizeof(at_get_buf));
        at_status = at_get_nolog(ifnum, pDev, "AT+CESQ\r\n", at_get_buf, sizeof(at_get_buf)); /*发送AT+CESQ*/
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
AcT 接入技术                     技术
0： GSM                          2G
1： GSM compact                  2G
2： UTRAN                        3G
3： GSM w/EGPRS                  2G
4： UTRAN w/HSDPA                3G
5： UTRAN w/HSUPA                3G
6： UTRAN w/HSDPA and HSUPA      3G
7： E-UTRAN                      4G
8:  EC-GSM_IOT                   5G(物联网)
9： E-UTRAN                      5G(物联网)
10：E-UTRA connected to a 5GCN   5G
11：NR connected to a 5GCN       5G SA
12：NG-RAN                       5G
13：E-UTRA-NR dual connectivity  5G NSA
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
* @brief     ：查询网络类型
* @param[in] ：void *pDev
* @param[out]：
* @return    ：网络类型
* @Create    : 王津剑
* @Date      ：2020-4-1
* @Update    :
**********************************************************************/
uint8 wireless_get_network(uint8 ifnum, void *pDev, uint8 *regOperat, uint8 bLog)
{
    char               *ptemp;
    int             at_status = 0;
    uint8             service = 0;
    int                retNet = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/

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
* @brief     ：查询手机号
* @param[in] ：void *pDev
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-28
* @Update    :
**********************************************************************/
int wireless_get_phonenum(uint8 ifnum, void *pDev, uint8 *pBuf, uint8 Len)
{
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    char   *ptemp;
    uint8 i = 0;
    uint16 start, len = 0;
    memset(at_get_buf, 0, sizeof(at_get_buf));
    memset(pBuf, 0, Len);

    at_get(ifnum, pDev, "AT+CNUM?\r\n", at_get_buf, sizeof(at_get_buf)); /*发送AT+CSQ*/
    ptemp = strstr(at_get_buf, "+CNUM:");
    if(NULL == ptemp)
    {    
        memset(at_get_buf, 0, sizeof(at_get_buf));
        at_get(ifnum, pDev, "AT+CNUM\r\n", at_get_buf, sizeof(at_get_buf)); /*发送AT+CSQ*/
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
* @brief     ：解析IMSI
* @param[in] ：char *pRecv  接收的消息
               uint8 buflen 缓存大小
* @param[out]：uint8 *pBuf  内容
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-3
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
* @brief     ：查询IMEI
* @param[in] ：void *pDev
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-28
* @Update    :
**********************************************************************/
int wireless_get_IMEI(uint8 ifnum, void *pDev, char *pBuf, uint8 Len)
{
    char               *ptemp = NULL;
    int             at_status = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    uint8   i = 0;
    memset(at_get_buf, 0, sizeof(at_get_buf));
    at_status = at_get(ifnum, pDev, "AT+CGSN\r\n", at_get_buf, sizeof(at_get_buf)); /*发送AT+CSQ*/
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
* @brief     ：查询网络是否注册成功
* @param[in] ：uint8 ifnum
               void *pDev
* @param[out]：
* @return    ：0-成功/其他没有注册成功
* @Create    : 王津剑
* @Date      ：2020-10-14
* @Update    :
**********************************************************************/
int wireless_check_newwork_reg(uint8 ifnum, void *pDev, uint8 bLog)
{
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/

    /* CREG、CGREG、CEREG其中之一注册成功则表示网络注册成功 */
    if(bLog)
    {
        at_get(ifnum, pDev, "AT+CREG?\r\n", at_get_buf, sizeof(at_get_buf));
    }
    else
    {
        at_get_nolog(ifnum, pDev, "AT+CREG?\r\n", at_get_buf, sizeof(at_get_buf));
    }
    /*判收到的字符串是否为 +CREG: *,1*/
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

