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
#include "message.h"
#include "appmsg_struct.h"
#include <net/if.h>	
#include "iniparser.h"
#include "storage.h"

#include "wireless.h"


pthread_mutex_t g_fWIreATUart;

void wireless_lock_init()
{
    pthread_mutex_init(&g_fWIreATUart, NULL);
}

int is_str_exist(char *source_str, char *res_str)
{
    int i;
    if ((source_str == NULL)||(res_str == NULL))
    {
        return -1;
    }

    for(i=0; i<strlen(source_str); i++)
    {
        if (source_str[i] == ' ')
        {
            memmove(source_str+i, source_str+i+1, strlen(source_str)-i-1);
        }
    }

    if (strstr(source_str, res_str) == NULL)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

/* ��ȡϵͳʱ��, ��ȷ��ms */
void get_curtime_ms(TIME_STRUCT_T *pTime)
{
    struct  tm systime;
    struct  timeval tv;
    struct  timezone tz;
    gettimeofday(&tv, &tz);

    FastSecondToDate(&tv, &tz, &systime);

    pTime->milliseconds = tv.tv_usec/1000;
    pTime->second = (uint8)systime.tm_sec;
    pTime->minute = (uint8)systime.tm_min;
    pTime->hour = (uint8)systime.tm_hour;
    pTime->day = (uint8)systime.tm_mday;
    pTime->month = (uint8)systime.tm_mon;         /*�·�0����1��*/
    pTime->year = (uint8)systime.tm_year;         /*���0����1900��*/
}

void set_curtime_ms(uint32 linuxTime, TIME_STRUCT_T *pTime)
{
    struct  tm systime;
    struct  timeval tv;
    struct  timezone tz;
    
    gettimeofday(&tv, &tz);
    tv.tv_sec = linuxTime;
    FastSecondToDate(&tv, &tz, &systime);

    pTime->milliseconds = tv.tv_usec/1000;
    pTime->second = (uint8)systime.tm_sec;
    pTime->minute = (uint8)systime.tm_min;
    pTime->hour = (uint8)systime.tm_hour;
    pTime->day = (uint8)systime.tm_mday;
    pTime->month = (uint8)systime.tm_mon;         /*�·�0����1��*/
    pTime->year = (uint8)systime.tm_year;         /*���0����1900��*/
}


/* ��ȡ���� */
uint32 read_queue(WIRELESS_QUEUE_T *plockqueue, uint8 *buf)
{
    uint32 len;
    uint8 *pdata_buf;
    pthread_mutex_lock(&plockqueue->f_lock);  /*��������*/
    len = 0;

    /*�������������ݣ���ȡ��buf�У��⿪��*/
    if ((plockqueue->head != plockqueue->tail)||((plockqueue->head == plockqueue->tail)&&(plockqueue->queue[plockqueue->head].len != 0)))
    {
        pdata_buf = plockqueue->queue[plockqueue->head].buf_queue;
        len = plockqueue->queue[plockqueue->head].len;
        memcpy(buf, pdata_buf, len);  /*ȡ�����е�����*/
        memset(pdata_buf, 0, len);    /*������*/
        plockqueue->queue[plockqueue->head].len = 0;
        plockqueue->head = (plockqueue->head+1)%WIRELESS_QUEUE_SIZE; /*����ͷ����ƶ�һλ*/
    }

    pthread_mutex_unlock(&plockqueue->f_lock);
    return len;
}

/* д���� */
int write_queue(WIRELESS_QUEUE_T *plockqueue, uint8 *buf, uint32 buf_len)
{
    int flags;
    flags = -1;
    pthread_mutex_lock(&plockqueue->f_lock);  /*��������*/

   /*�ж������Ƿ��пյ�λ��*/
    if (plockqueue->queue[plockqueue->tail].len == 0)
    {
        plockqueue->queue[plockqueue->tail].len = buf_len;
        memcpy(plockqueue->queue[plockqueue->tail].buf_queue, buf, buf_len);
        flags = 0;
        plockqueue->tail = (plockqueue->tail+1)%WIRELESS_QUEUE_SIZE;
    }
    pthread_mutex_unlock(&plockqueue->f_lock);  /*�⻺����*/
    return flags;
}

/* ��ն��� */
void clear_queue(WIRELESS_QUEUE_T *plockqueue)
{
    pthread_mutex_lock(&plockqueue->f_lock);  /*��������*/
    plockqueue->head = 0;
    plockqueue->tail= 0;
    memset(plockqueue->queue, 0, sizeof(WIRELESS_QUEUEMSG_T) * WIRELESS_QUEUE_SIZE);
    pthread_mutex_unlock(&plockqueue->f_lock);  /*�⻺����*/
}

/**********************************************************************
* @name      : wireless_get_apn
* @brief     ������ָ����Ӫ�̵�apn, �û����õĲ����ھ���Ĭ�����õ�
* @param[in] ��WIRELESS_HANDLE_T *ptHandle
* @param[out]��
* @return    ��0-�ɹ�/ʧ��
* @Create    : ����
* @Date      ��2020-4-22
* @Update    :
**********************************************************************/
int wireless_get_apn(WIRELESS_HANDLE_T *ptHandle)
{
    uint8 i = ptHandle->apnIndex;
    
    /* ��ʶ��ľʹ��û�������ѡ��һ�� */
    if(WIRELESS_OPT_NONE == ptHandle->ptNetPara->operat)
    {
        if(0 == ptHandle->ptNetPara->userAPN.diagnum)
        {
            ptHandle->bUserApn = FALSE;
            memcpy(&(ptHandle->apn), &(ptHandle->ptNetPara->defaultAPN.tDiagApn[i].apn), sizeof(OOP_VISIBLEVAR_T));
            memcpy(&(ptHandle->user), &(ptHandle->ptNetPara->defaultAPN.tDiagApn[i].username), sizeof(OOP_VISIBLEVAR_T));
            memcpy(&(ptHandle->pwd), &(ptHandle->ptNetPara->defaultAPN.tDiagApn[i].pwd), sizeof(OOP_VISIBLEVAR_T));
        }
        else
        {
            ptHandle->bUserApn = TRUE;
            memcpy(&(ptHandle->apn), &(ptHandle->ptNetPara->userAPN.tDiagApn[i].apn), sizeof(OOP_VISIBLEVAR_T));
            memcpy(&(ptHandle->user), &(ptHandle->ptNetPara->userAPN.tDiagApn[i].username), sizeof(OOP_VISIBLEVAR_T));
            memcpy(&(ptHandle->pwd), &(ptHandle->ptNetPara->userAPN.tDiagApn[i].pwd), sizeof(OOP_VISIBLEVAR_T));
        }
        return 0;
    }

    /* ���û����� */
    if(0 != ptHandle->ptNetPara->userAPN.diagnum)
    {
        ptHandle->bUserApn = TRUE;
        for(i = ptHandle->apnIndex; i < ptHandle->ptNetPara->userAPN.diagnum; i++)
        {
            if(ptHandle->ptNetPara->operat == ptHandle->ptNetPara->userAPN.tDiagApn[i].operat ||
               WIRELESS_NOCC == ptHandle->ptNetPara->userAPN.tDiagApn[i].operat ||
               WIRELESS_OPT_NONE == ptHandle->ptNetPara->userAPN.tDiagApn[i].operat)
            {
                ptHandle->apnIndex = i;
                memcpy(&(ptHandle->apn), &(ptHandle->ptNetPara->userAPN.tDiagApn[i].apn), sizeof(OOP_VISIBLEVAR_T));
                memcpy(&(ptHandle->user), &(ptHandle->ptNetPara->userAPN.tDiagApn[i].username), sizeof(OOP_VISIBLEVAR_T));
                memcpy(&(ptHandle->pwd), &(ptHandle->ptNetPara->userAPN.tDiagApn[i].pwd), sizeof(OOP_VISIBLEVAR_T));
                return 0;
            }
        }
        /* �����ѯ */
        for(i = 0; i < ptHandle->apnIndex; i++)
        {
            if(ptHandle->ptNetPara->operat == ptHandle->ptNetPara->userAPN.tDiagApn[i].operat ||
               WIRELESS_NOCC == ptHandle->ptNetPara->userAPN.tDiagApn[i].operat ||
               WIRELESS_OPT_NONE == ptHandle->ptNetPara->userAPN.tDiagApn[i].operat)
            {
                ptHandle->apnIndex = i;
                memcpy(&(ptHandle->apn), &(ptHandle->ptNetPara->userAPN.tDiagApn[i].apn), sizeof(OOP_VISIBLEVAR_T));
                memcpy(&(ptHandle->user), &(ptHandle->ptNetPara->userAPN.tDiagApn[i].username), sizeof(OOP_VISIBLEVAR_T));
                memcpy(&(ptHandle->pwd), &(ptHandle->ptNetPara->userAPN.tDiagApn[i].pwd), sizeof(OOP_VISIBLEVAR_T));
                return 0;
            }
        }
    } 

    /* �û�û�в�ѯ�� ��Ĭ�������л�ȡ */
    ptHandle->bUserApn = FALSE;
    for(i = ptHandle->apnIndex; i < ptHandle->ptNetPara->defaultAPN.diagnum; i++)
    {
        if(ptHandle->ptNetPara->operat == ptHandle->ptNetPara->defaultAPN.tDiagApn[i].operat)
        {
            ptHandle->apnIndex = i;
            memcpy(&(ptHandle->apn), &(ptHandle->ptNetPara->defaultAPN.tDiagApn[i].apn), sizeof(OOP_VISIBLEVAR_T));
            memcpy(&(ptHandle->user), &(ptHandle->ptNetPara->defaultAPN.tDiagApn[i].username), sizeof(OOP_VISIBLEVAR_T));
            memcpy(&(ptHandle->pwd), &(ptHandle->ptNetPara->defaultAPN.tDiagApn[i].pwd), sizeof(OOP_VISIBLEVAR_T));
            return 0;
        }
    }
    /* �����ѯ */
    for(i = 0; i < ptHandle->apnIndex; i++)
    {
        if(ptHandle->ptNetPara->operat == ptHandle->ptNetPara->defaultAPN.tDiagApn[i].operat)
        {
            ptHandle->apnIndex = i;
            memcpy(&(ptHandle->apn), &(ptHandle->ptNetPara->defaultAPN.tDiagApn[i].apn), sizeof(OOP_VISIBLEVAR_T));
            memcpy(&(ptHandle->user), &(ptHandle->ptNetPara->defaultAPN.tDiagApn[i].username), sizeof(OOP_VISIBLEVAR_T));
            memcpy(&(ptHandle->pwd), &(ptHandle->ptNetPara->defaultAPN.tDiagApn[i].pwd), sizeof(OOP_VISIBLEVAR_T));
            return 0;
        }
    }

    WIRELESS_FMT_DEBUG("Can not fine apn for id %d!\n", ptHandle->ptNetPara->defaultAPN.id);
    return -1;
}


/**********************************************************************
* @name      : wireless_set_nextapn
* @brief     ��������һ����ȡAPN
* @param[in] ��WIRELESS_HANDLE_T *ptHandle
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-11-6
* @Update    :
**********************************************************************/
void wireless_set_nextapn(WIRELESS_HANDLE_T *ptHandle)
{
    if(TRUE == ptHandle->bUserApn)
    {
        if(ptHandle->apnIndex < ptHandle->ptNetPara->userAPN.diagnum - 1)
        {
            ptHandle->apnIndex++;
        }
        else
        {
            ptHandle->apnIndex = 0;
        }
    }
    else
    {
        if(ptHandle->apnIndex < ptHandle->ptNetPara->defaultAPN.diagnum - 1)
        {
            ptHandle->apnIndex++;
        }
        else
        {
            ptHandle->apnIndex = 0;
        }
    }
    wireless_get_apn(ptHandle);
}

void *g_WireATUart = NULL;
TIME_STRUCT_T g_ATSHowSendTime;
char  g_ATSHowSend[256] = {0};
uint16 g_ATSHowSendlen = 0;
TIME_STRUCT_T g_ATSHowRecvTime;
char  g_ATSHowRecv[256] = {0};
uint16 g_ATSHowRecvlen = 0;

/**********************************************************************
* @name      : wireless_atshow_set
* @brief     ��atָ����ʾ����
* @param[in] ��void *pUart   ���ھ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-4-8
* @Update    :
**********************************************************************/
void wireless_atshow_set(void *pUart)
{
    pthread_mutex_lock(&g_fWIreATUart);
    g_WireATUart = pUart;
    memset(g_ATSHowSend, 0,256);
    memset(g_ATSHowRecv, 0,256);
    g_ATSHowSendlen = 0;
    g_ATSHowRecvlen = 0;
    memset(&g_ATSHowSendTime, 0, sizeof(TIME_STRUCT_T));
    memset(&g_ATSHowRecvTime, 0, sizeof(TIME_STRUCT_T));
    pthread_mutex_unlock(&g_fWIreATUart);
}

/**********************************************************************
* @name      : wireless_atshow_send
* @brief     ��atָ����ʾ���÷�������
* @param[in] ��void *pUart     ���ھ��
               char *senddata  ��������
               uint16 sendlen  ���ͳ���
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-4-8
* @Update    :
**********************************************************************/
void wireless_atshow_send(void *pUart, char *senddata, uint16 sendlen)
{
    if(NULL == g_WireATUart || g_WireATUart != pUart || NULL == senddata || 0 == sendlen)
    {
        return;
    }
    
    pthread_mutex_lock(&g_fWIreATUart);
    memset(g_ATSHowSend, 0,256);
    g_ATSHowSendlen = MIN(255, sendlen);
    memcpy(g_ATSHowSend, senddata, g_ATSHowSendlen);
    get_curtime_ms(&g_ATSHowSendTime);
    
    memset(g_ATSHowRecv, 0,256);
    g_ATSHowRecvlen = 0;
    memset(&g_ATSHowRecvTime, 0, sizeof(TIME_STRUCT_T));
    pthread_mutex_unlock(&g_fWIreATUart);
}

/**********************************************************************
* @name      : wireless_atshow_recv
* @brief     ��atָ����ʾ���ý�������
* @param[in] ��void *pUart     ���ھ��
               char *recvdata  ��������
               uint16 recvlen  ���ճ���
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-4-8
* @Update    :
**********************************************************************/
void wireless_atshow_recv(void *pUart, char *recvdata, uint16 recvlen)
{
    uint16 len = 0;
    if(NULL == g_WireATUart || g_WireATUart != pUart || NULL == recvdata || 0 == recvlen)
    {
        return;
    }
    
    pthread_mutex_lock(&g_fWIreATUart);
    get_curtime_ms(&g_ATSHowRecvTime);
    len = MIN(255 - g_ATSHowRecvlen, recvlen);
    if(len > 0)
    {
        memcpy(g_ATSHowRecv + g_ATSHowRecvlen, recvdata, len);
    }
    g_ATSHowRecvlen += len;
    pthread_mutex_unlock(&g_fWIreATUart);
}

/**********************************************************************
* @name      : wireless_atshow_get
* @brief     ��atָ����ʾ���ݻ�ȡ
* @param[in] ��uint16 sendbuflen         ���ͻ��泤��
               uint16 recvbuflen         ���ջ��泤��
* @param[out]��char *sendbuf             ��������
               TIME_STRUCT_T *pSendTime  ����ʱ��
               char *recvbuf             ��������
               TIME_STRUCT_T *pRecvTime  ����ʱ��
* @return    ��
* @Create    : ����
* @Date      ��2021-4-8
* @Update    :
**********************************************************************/
void wireless_atshow_get(char *sendbuf, uint16 sendbuflen, TIME_STRUCT_T *pSendTime,
                         char *recvbuf, uint16 recvbuflen, TIME_STRUCT_T *pRecvTime)
{
    pthread_mutex_lock(&g_fWIreATUart);
    memcpy(sendbuf, g_ATSHowSend, MIN(sendbuflen, g_ATSHowSendlen));
    memcpy(pSendTime, &g_ATSHowSendTime, sizeof(TIME_STRUCT_T));
    memcpy(recvbuf, g_ATSHowRecv, MIN(recvbuflen, g_ATSHowRecvlen));
    memcpy(pRecvTime, &g_ATSHowRecvTime, sizeof(TIME_STRUCT_T));
    pthread_mutex_unlock(&g_fWIreATUart);
}

/**********************************************************************
* @name      : wireless_uart_init
* @brief     ����ʼ�����ڽӿ�
* @param[in] ��char *device       �豸id
* @param[out]��
* @return    �����ھ��
* @Create    : ����
* @Date      ��2020-3-13
* @Update    :
**********************************************************************/
void *wireless_uart_init(char *device)
{
    UART_DEVICE_T* dev;
    dev = (UART_DEVICE_T*)hal_device_get(device);
    if(NULL == dev)
    {
        return NULL;
    }
    
    // ���豸
    if(0 != dev->uart_param_set(dev, 115200, 8, 1, PARITY_NO))
    {
        WIRELESS_FMT_DEBUG("���ô���%s����ʧ��\n", device);
        hal_device_release((HW_DEVICE *)dev);
        return NULL;
    }

    return dev;
}

/**********************************************************************
* @name      : wireless_uart_uninit
* @brief     ��ȥ��ʼ�����ڽӿ�
* @param[in] ��void *pDev           ���ھ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-3-10
* @Update    :
**********************************************************************/
void wireless_uart_uninit(void *pDev)
{
    if(NULL == pDev)
    {
        return;
    }
    
    hal_device_release((HW_DEVICE *)pDev);
}

/**********************************************************************
* @name      : wireless_uart_send
* @brief     ���������ݽ��սӿ�
* @param[in] ��void *pDev               ���ھ��
               uint8* data              �������ݻ�����
               int len                  ���ͳ���
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-3-10
* @Update    :
**********************************************************************/
int wireless_uart_send(void *pDev, uint8* data, int len)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T*)pDev;
    if(0 == dev->uart_data_send(dev, data, len))
    {
        wireless_atshow_send(dev, (char *)data, len);
        return 0;
    }
    return -1;
}

/**********************************************************************
* @name      : wireless_uart_recv
* @brief     ���������ݽ��սӿ�
* @param[in] ��void *pDev                  ���ھ��
               int bufLen                  �����ճ���
* @param[out]��uint8* buf                  ��������
* @return    �����յ����ݵĳ���
* @Create    : ����
* @Date      ��2020-3-13
* @Update    :
**********************************************************************/
int wireless_uart_recv(void *pDev, uint8* buf, int bufLen)
{
    int ret;
    UART_DEVICE_T* dev = (UART_DEVICE_T*)pDev;
    ret = dev->uart_data_recv(dev, buf, bufLen);
    if(ret > 0)
    {
        wireless_atshow_recv(dev, (char *)buf, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : wireless_uart_init
* @brief     ����ʼ�����ڽӿ�
* @param[in] ��char *device       �豸id
* @param[out]��
* @return    �����Ƴ���
* @Create    : ����
* @Date      ��2022-5-26
* @Update    :
**********************************************************************/
int wireless_uart_getname(void *pDev, uint8* pName)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T*)pDev;
    if(NULL == dev)
    {
        return 0;
    }

    return dev->uart_get_real_dev_name(dev, pName);
}

/**********************************************************************
* @name      : wireless_get_default_apn
* @brief     ������Ĭ�ϵĲ��Ų���
* @param[in] ��void *pIni       �ļ����
* @param[out]��void *ptApn      ���Ų���
               uint16 *pTime   ����ʱ��
               uint8 *bIpv6    �Ƿ�֧��ipv6
               uint8 *lognum   ��־����
               char *plogpath  ��־
* @return    ��
* @Create    : ����
* @Date      ��2020-4-21
* @Update    :
**********************************************************************/
void wireless_get_default_conf(char *filepath, void *ptApn, uint16 *pTime, uint8 *bIpv6, uint8 *lognum,
                                            char *plogpath)
{
    dictionary *ini = NULL;
    char  *stringvalue = NULL;
    int    intvalue = 0;
    WIRE_DIAG_APN_T *pAPN = (WIRE_DIAG_APN_T *)ptApn;
    uint8  i, j = 0, n = 0;
    char   keywords[32] = {0};
    int    oper;
    char   apn[64] = {0};
    char   user[64] = {0};
    char   pwd[64] = {0};
    char   cmd[256] = {0};

    if(NULL == ptApn || NULL == pTime)
    {
        return;
    }
    
    if(NULL != filepath)
    {
        ini = iniparser_load(filepath);
        if(NULL == ini)
        {
            WIRELESS_FMT_DEBUG("�����ļ�%s��Ч\n", filepath);
        }
    }
    
    /* ��Ч */
    if(NULL == filepath || NULL == ini)
    {
        *pTime = 60;
        goto no_para;
    }
    
    intvalue = iniparser_getint(ini, "TIME_CONF:NOSIM_REBOOT", 60);
    *pTime = intvalue <= 0 ? 0: MIN(intvalue, 1440);
    
    intvalue = iniparser_getint(ini, "DEFAULT_APN_CONF:PARAM_NUM", 0);
    pAPN->diagnum = MIN((uint8)intvalue, WIRELESS_DIAGAPN_MAX);
    for(i = 0; i < pAPN->diagnum; i++)
    {
        sprintf(keywords, "DEFAULT_APN_CONF:PARAM%d", i + 1);
        stringvalue = iniparser_getstring(ini, keywords, NULL);
        if(NULL == stringvalue)
        {
            continue;
        }
        n = sscanf(stringvalue, "%d %s %s %s", &oper, apn, user, pwd);
        if(n < 2)
        {
            continue;
        }
        pAPN->tDiagApn[j].operat = (uint8)oper;
        pAPN->tDiagApn[j].apn.nNum = strlen(apn);
        memcpy(pAPN->tDiagApn[j].apn.value, apn, pAPN->tDiagApn[j].apn.nNum);
        if(n >= 3)
        {
            pAPN->tDiagApn[j].username.nNum = strlen(user);
            memcpy(pAPN->tDiagApn[j].username.value, user, pAPN->tDiagApn[j].username.nNum);
        }
        if(n >= 4)
        {
            pAPN->tDiagApn[j].pwd.nNum = strlen(pwd);
            memcpy(pAPN->tDiagApn[j].pwd.value, pwd, pAPN->tDiagApn[j].pwd.nNum);
        }
        j++;
    }
    pAPN->diagnum = j;

    intvalue = iniparser_getint(ini, "PPP_CONF:IPV6", 0);
    *bIpv6 = intvalue;
    intvalue = iniparser_getint(ini, "PPP_CONF:LOGNUM", 0);
    *lognum = intvalue;
    stringvalue = iniparser_getstring(ini, "PPP_CONF:LOGPATH", "/tmp/pppd_log");
    memcpy(plogpath, stringvalue, MIN(128, strlen(stringvalue)));
    sprintf(cmd, "mkdir -p %s", plogpath);
    system(cmd);
    
    iniparser_freedict(ini);
    if(0 == pAPN->diagnum)
    {
        goto no_para; //û�л�ȡ�ɹ�, дĬ��ֵ
    }
    return;
    
no_para:
    /* ��ԭʼ��Ĭ��ֵ */
    pAPN->diagnum = 3;
    
    pAPN->tDiagApn[0].operat = 0;
    pAPN->tDiagApn[0].apn.nNum = strlen("cmnet");
    memcpy(pAPN->tDiagApn[0].apn.value, "cmnet", pAPN->tDiagApn[0].apn.nNum);
    pAPN->tDiagApn[0].username.nNum = strlen("gsm");
    memcpy(pAPN->tDiagApn[0].username.value, "gsm", pAPN->tDiagApn[0].username.nNum);
    pAPN->tDiagApn[0].pwd.nNum = strlen("123");
    memcpy(pAPN->tDiagApn[0].pwd.value, "123", pAPN->tDiagApn[0].pwd.nNum);
    
    pAPN->tDiagApn[1].operat = 1;
    pAPN->tDiagApn[1].apn.nNum = strlen("3gnet");
    memcpy(pAPN->tDiagApn[1].apn.value, "3gnet", pAPN->tDiagApn[1].apn.nNum);
    pAPN->tDiagApn[1].username.nNum = 0;
    memset(pAPN->tDiagApn[1].username.value, 0, 64);
    pAPN->tDiagApn[1].pwd.nNum = 0;
    memset(pAPN->tDiagApn[1].pwd.value, 0, 64);
    
    pAPN->tDiagApn[2].operat = 2;
    pAPN->tDiagApn[2].apn.nNum = strlen("ctnet");
    memcpy(pAPN->tDiagApn[2].apn.value, "ctnet", pAPN->tDiagApn[2].apn.nNum);
    pAPN->tDiagApn[2].username.nNum = 0;
    memset(pAPN->tDiagApn[2].username.value, 0, 64);
    pAPN->tDiagApn[2].pwd.nNum = 0;
    memset(pAPN->tDiagApn[2].pwd.value, 0, 64);

    *bIpv6 = 0;
    *lognum = 0;
    memcpy(plogpath, "/tmp/log/app/wirelessDCM", strlen("/tmp/log/app/wirelessDCM"));
}

/**********************************************************************
* @name      : wireless_check_userfile
* @brief     ������û������ļ�, ���û�оʹ���
               �ļ���С 2048 * 3; ǰ4���ֽ��д��У��
* @param[in] ��char *filename �ļ���
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-4-22
* @Update    :
**********************************************************************/
void wireless_check_userfile(char *filename)
{
    char check[4] = {0xea, 0xeb, 0xec, 0x07}; /* ���һλ����Ϣ�汾��, ÿ�޸�һ������1 */
    char buf[4] = {0};
    WIRE_DIAG_APN_T userAPN;

    if(0 == access(filename, F_OK))
    {
        if(0 == read_pdata_xram(filename, buf, 0, 4))
        {
            WIRELESS_FMT_TRACE("File version %02x %02x %02x %02x, check %02x %02x %02x %02x\n", 
                   buf[0], buf[1], buf[2], buf[3], check[0], check[1], check[2], check[3]);
            if(0 == memcmp(buf, check, 4))
            {
                return;
            }
        }
        WIRELESS_FMT_DEBUG("Create new userfile\n");
    }

    char writebuf[2048 * 3] = {0};
    write_pdata_xram(filename, writebuf, 0, 2048 * 3);
    write_pdata_xram(filename, check, 0, 4);

    memset(&userAPN, 0, sizeof(WIRE_DIAG_APN_T));
    userAPN.id = 1;
    wireless_write_user_apn(filename, 1, &userAPN);
    userAPN.id = 2;
    wireless_write_user_apn(filename, 2, &userAPN);

    wireless_write_authtype(filename, 1, 255);
    wireless_write_authtype(filename, 2, 255);
}

/**********************************************************************
* @name      : wireless_get_default_apn
* @brief     ����ȡ�û����õ�APN, ���λ�� id * 2048
* @param[in] ��char *filename �ļ���
               uint8 id       �豸���(1-2)
* @param[out]��void *ptApn    ���Ų���
* @return    ��
* @Create    : ����
* @Date      ��2020-4-21
* @Update    :
**********************************************************************/
void wireless_get_user_apn(char *filename, uint8 id, void *ptApn)
{
    if(NULL == filename || id > 2 || 0 == id || NULL == ptApn)
    {
        return;
    }

    read_pdata_xram(filename, ptApn, id * 2048, sizeof(WIRE_DIAG_APN_T));
}

/**********************************************************************
* @name      : wireless_write_user_apn
* @brief     ���洢�û����õ�APN, ���λ�� id * 2048
* @param[in] ��char *file    �ļ���
               uint8 id      �豸���(1-2)
               void *ptApn   ���Ų���
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-4-21
* @Update    :
**********************************************************************/
int wireless_write_user_apn(char *filename, uint8 id, void *ptApn)
{
    if(NULL == filename || id > 2 || 0 == id || NULL == ptApn)
    {
        return 0;
    }

    return write_pdata_xram(filename, ptApn, id * 2048, sizeof(WIRE_DIAG_APN_T));
}

/**********************************************************************
* @name      : wireless_get_authtype
* @brief     ����ȡ�û����õļ�Ȩ��ʽ, ���λ�� id * 10
* @param[in] ��char *file    �ļ���
               uint8 id      �豸���(1-2)
* @param[out]��
* @return    ����Ȩ��ʽ
* @Create    : ����
* @Date      ��2020-4-22
* @Update    :
**********************************************************************/
uint8 wireless_get_authtype(char *filename, uint8 id)
{
    uint8 type = 255;

    if(NULL == filename || id > 2 || 0 == id)
    {
        return 0;
    }

    read_pdata_xram(filename, (char *)&type, id * 10, 1);

    return type;
}

/**********************************************************************
* @name      : wireless_write_authtype
* @brief     ���洢�û����õļ�Ȩ��ʽ, ���λ�� id * 10
* @param[in] ��char *file    �ļ���
               uint8 id      �豸���(1-2)
               uint8 type    ��Ȩ��ʽ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-4-22
* @Update    :
**********************************************************************/
int wireless_write_authtype(char *filename, uint8 id, uint8 type)
{
    if(NULL == filename || id > 2 || 0 == id)
    {
        return 0;
    }
    return write_pdata_xram(filename, (char *)&type, id * 10, 1);
}

/**********************************************************************
* @name      : wireless_get_centernum
* @brief     ����ȡ�û����õĶ������ĺ���Ͷ��Ÿ�ʽ, ���λ�� 30 + (id-1) * 35
* @param[in] ��char *file    �ļ���
               uint8 id      �豸���(1-2)
               uint8 len     ���泤��
* @param[out]��uint8 *pNum   ���ĺ���
               uint8 *type   ���Ÿ�ʽ
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-4-22
* @Update    :
**********************************************************************/
int wireless_get_centernum(char *filename, uint8 id, uint8 *pNum, uint8 len, uint8 *type)
{
    if(NULL == filename || id > 2 || 0 == id || NULL == pNum)
    {
        return -1;
    }

    read_pdata_xram(filename, (char *)type, 30 + (id-1) * 35, 1);
    read_pdata_xram(filename, (char *)pNum, 30 + (id-1) * 35 + 1, MIN(32, len));
    return 0;
}

/**********************************************************************
* @name      : wireless_write_authtype
* @brief     ���洢�û����õĶ������ĺ���Ͷ��Ÿ�ʽ, ���λ�� ���λ�� 30 + (id-1) * 35
* @param[in] ��char *file    �ļ���
               uint8 id      �豸���(1-2)
               uint8 *pNum   ���ĺ���
               uint8 len     ���볤��
               uint8 type    ���Ÿ�ʽ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-4-22
* @Update    :
**********************************************************************/
int wireless_write_centernum(char *filename, uint8 id, uint8 *pNum, uint8 len, uint8 type)
{
    if(NULL == filename || id > 2 || 0 == id || NULL == pNum)
    {
        return -1;
    }
    char writebuf[35] = {0};

    write_pdata_xram(filename, writebuf, 30 + (id-1) * 35, 35);
    write_pdata_xram(filename, (char *)&type, 30 + (id-1) * 35, 1);
    return write_pdata_xram(filename, (char *)pNum, 30 + (id-1) * 35 + 1, MIN(32, len));
}

/**********************************************************************
* @name      : wireless_get_flowset
* @brief     ����ȡ�û����õ���������, ���λ�� 100 + (id-1) * 10
* @param[in] ��char *file    �ļ���
               uint8 id      �豸���(1-2)
* @param[out]��
* @return    ������
* @Create    : ����
* @Date      ��2020-4-22
* @Update    :
**********************************************************************/
uint32 wireless_get_flowset(char *filename, uint8 id)
{
    uint32 value = 0;

    if(NULL == filename || id > 2 || 0 == id)
    {
        return 0;
    }
    read_pdata_xram(filename, (char *)&value, 100 + (id-1) * 10, 4);

    return value;
}

/**********************************************************************
* @name      : wireless_write_authtype
* @brief     ���洢�û����õ���������, ���λ�� 100 + (id-1) * 10
* @param[in] ��char *file    �ļ���
               uint8 id      �豸���(1-2)
               uint8 *pNum   ���ĺ���
               uint8 len     ���볤��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-4-22
* @Update    :
**********************************************************************/
int wireless_write_flowset(char *filename, uint8 id, uint32 value)
{
    if(NULL == filename || id > 2 || 0 == id)
    {
        return -1;
    }
    return write_pdata_xram(filename, (char *)&value, 100 + (id-1) * 10, 4);
}

/**********************************************************************
* @name      : wireless_get_flowset
* @brief     ����ȡ�û����õ���������, �����ʼλ�� 120 + (id-1) * 20
               ǰ10�ֽڴ洢 ����, ����10�ֽڴ洢 ʱ��
* @param[in] ��char *file    �ļ���
               uint8 id      �豸���(1-2)
* @param[out]��
* @return    ������
* @Create    : ����
* @Date      ��2020-4-22
* @Update    :
**********************************************************************/
uint64 wireless_get_userflow(char *filename, uint8 id, TIME_STRUCT_T *pTime)
{
    uint64 value = 0;

    if(NULL == filename || id > 2 || 0 == id || NULL == pTime)
    {
        return 0;
    }

    read_pdata_xram(filename, (char *)&value, 120 + (id-1) * 20, sizeof(uint64));
    read_pdata_xram(filename, (char *)pTime, 120 + (id-1) * 20 + 10, sizeof(TIME_STRUCT_T));
    return value;
}

/**********************************************************************
* @name      : wireless_write_authtype
* @brief     ���洢�û����õ���������, �����ʼλ�� 120 + (id-1) * 20
               ǰ10�ֽڴ洢 ����, ����10�ֽڴ洢 ʱ��
* @param[in] ��char *file    �ļ���
               uint8 id      �豸���(1-2)
               uint8 *pNum   ���ĺ���
               uint8 len     ���볤��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-4-22
* @Update    :
**********************************************************************/
int wireless_write_userflow(char *filename, uint8 id, uint64 value)
{
    TIME_STRUCT_T curtime;

    if(NULL == filename || id > 2 || 0 == id)
    {
        return -1;
    }

    get_curtime_ms(&curtime);
    write_pdata_xram(filename, (char *)&value, 120 + (id-1) * 20, sizeof(uint64));
    return write_pdata_xram(filename, (char *)&curtime, 120 + (id-1) * 20 + 10, sizeof(TIME_STRUCT_T));
}

/**********************************************************************
* @name      : wireless_get_num
* @brief     ���洢���Ŵ�����ȡ, �����ʼλ�� 160 + (id-1) * 20
               ǰ8�ֽڴ洢 ʱ��, ����12�ֽڴ洢����
* @param[in] ��char *file               �ļ���
               uint8 id                 �豸���(1-2)
* @param[out]��TIME_STRUCT_T *pdaytime  ʱ��
               uint32 *cnum             ���Ӵ���
               uint32 *fnum             ʧ�ܴ���
               uint32 *stime            �ɹ�ʱ��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-5-31
* @Update    :
**********************************************************************/
int wireless_get_num(char *filename, uint8 id, TIME_STRUCT_T *pdaytime, uint32 *cnum, uint32 *fnum, uint32 *stime)
{
    int ret = 0;

    if(NULL == filename || id > 2 || 0 == id || NULL == pdaytime)
    {
        return 0;
    }

    ret = read_pdata_xram(filename, (char *)pdaytime, 160 + (id-1) * 20, sizeof(TIME_STRUCT_T));
    ret |= read_pdata_xram(filename, (char *)cnum, 160 + (id-1) * 20 + 8, sizeof(uint32));
    ret |= read_pdata_xram(filename, (char *)fnum, 160 + (id-1) * 20 + 12, sizeof(uint32));
    ret |= read_pdata_xram(filename, (char *)stime, 160 + (id-1) * 20 + 16, sizeof(uint32));

    if(pdaytime->year == 0xff)
    {
        memset(pdaytime, 0, sizeof(TIME_STRUCT_T));
    }
    
    return ret;
}

/**********************************************************************
* @name      : wireless_write_num
* @brief     ���洢���Ŵ���, �����ʼλ�� 160 + (id-1) * 20
               ǰ8�ֽڴ洢 ʱ��, ����12�ֽڴ洢����
* @param[in] ��char *file               �ļ���
               uint8 id                 �豸���(1-2)
               TIME_STRUCT_T *pdaytime  ʱ��
               uint8 type               ����(0-���Ӵ���, 1-ʧ�ܴ���, 2-�ɹ�ʱ��)
               uint32 timeornum         ������ʱ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-5-31
* @Update    :
**********************************************************************/
int wireless_write_num(char *filename, uint8 id, TIME_STRUCT_T *pdaytime, uint8 type, uint32 timeornum)
{
    if(NULL == filename || id > 2 || 0 == id || type > 2 || NULL == pdaytime)
    {
        return -1;
    }
    pdaytime->hour = 0;
    pdaytime->minute = 0;
    pdaytime->second = 0;
    pdaytime->milliseconds = 0;

    write_pdata_xram(filename, (char *)pdaytime, 160 + (id-1) * 20, sizeof(TIME_STRUCT_T));
    return write_pdata_xram(filename, (char *)&timeornum, 160 + (id-1) * 20 + 8 + type * 4, sizeof(uint32));
}

/**********************************************************************
* @name      : wireless_get_logno
* @brief     ����ȡ������־���, �����ʼλ�� 200 + (id-1) * 4
* @param[in] ��char *file               �ļ���
               uint8 id                �豸���(1-2)
* @param[out]��uint8 *no                ��־���
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2022-6-28
* @Update    :
**********************************************************************/
int wireless_get_logno(char *filename, uint8 id, uint8 *no)
{
    int ret = 0;

    if(NULL == filename || id > 2 || 0 == id || NULL == no)
    {
        return 0;
    }

    *no = 0;
    ret = read_pdata_xram(filename, (char *)no, 200 + (id-1) * 4, sizeof(uint8));

    return ret;
}

/**********************************************************************
* @name      : wireless_write_logno
* @brief     ���洢������־���, �����ʼλ�� 200 + (id-1) * 4
* @param[in] ��char *file               �ļ���
               uint8 id                �豸���(1-2)
               uint8 no                ��־���
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2022-6-28
* @Update    :
**********************************************************************/
int wireless_write_logno(char *filename, uint8 id, uint8 no)
{
    if(NULL == filename || id > 2 || 0 == id)
    {
        return -1;
    }

    return write_pdata_xram(filename, (char *)&no, 200 + (id-1) * 4, sizeof(uint8));
}

/**********************************************************************
* @name      : wireless_get_gpsmode
* @brief     ����ȡ����GPSģʽ, �����ʼλ�� 210 + (id-1) * 2
* @param[in] ��char *file               �ļ���
               uint8 id                �豸���(1-2)
* @param[out]��uint8 *mode              ģʽ
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2022-9-26
* @Update    :
**********************************************************************/
int wireless_get_gpsmode(char *filename, uint8 id, uint8 *mode)
{
    int ret = 0;
    uint8 buf[2] = {0};
    
    if(NULL == filename || id > 2 || 0 == id || NULL == mode)
    {
        return 0;
    }

    ret = read_pdata_xram(filename, (char *)buf, 210 + (id-1) * 4, 2);
    if(0 != ret || 0 == buf[0])
    {
        return 1;
    }
    *mode = buf[1];
    return 0;
}

/**********************************************************************
* @name      : wireless_write_gpsmode
* @brief     ���洢������GPSģʽ, �����ʼλ�� 210 + (id-1) * 4
* @param[in] ��char *file               �ļ���
               uint8 id                �豸���(1-2)
               uint8 mode              ģʽ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2022-9-26
* @Update    :
**********************************************************************/
int wireless_write_gpsmode(char *filename, uint8 id, uint8 mode)
{
    uint8 buf[2] = {1, 0};
    if(NULL == filename || id > 2 || 0 == id)
    {
        return -1;
    }
    buf[1] = mode;

    return write_pdata_xram(filename, (char *)buf, 210 + (id-1) * 4, 2);
}

