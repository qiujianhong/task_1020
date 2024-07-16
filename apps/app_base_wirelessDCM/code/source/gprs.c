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

extern uint8 Isusb;

/* ��ȡ����ע����Ϣ */
uint8 get_network_state(char *pBuf)
{
    char *temp = pBuf;

    while(*temp)
    {
        if(',' == *temp)
        {
          break;
        }
        temp++;
    }
    if(*temp)
    {
        temp++;
        return *temp - '0';
    }
    return 0;
}

/* ��ȡ��Ӫ�̴��� ��OPERATOR_E��Ӧ */
uint8 get_operator_code(char *pBuf)
{
    /* �й��ƶ�CMCC: 46000 46002 46004 46007 */
    /* �й�����CTCC: 46003 46005 46011 */
    /* �й���ͨCUCC: 46001 46006 46009 */
    uint8 uNetopt = WIRELESS_OPT_NONE;
    
    /*��֤AT����Ӧ���Ƿ���ȷ*/
    if(NULL != strstr(pBuf, "CMCC") || NULL != strstr(pBuf, "MOBILE")
       || NULL != strstr(pBuf, "46000") || NULL != strstr(pBuf, "46002")
       || NULL != strstr(pBuf, "46004") || NULL != strstr(pBuf, "46007"))
    {   
        uNetopt = WIRELESS_CMCC;
    }
    else if(NULL != strstr(pBuf, "CTCC") || NULL != strstr(pBuf, "TELECOM") || NULL != strstr(pBuf, "CHN-CT")
            || NULL != strstr(pBuf, "46003") || NULL != strstr(pBuf, "46005") 
            || NULL != strstr(pBuf, "46011"))
    {
        uNetopt = WIRELESS_CTCC;
    }
    else if(NULL != strstr(pBuf, "CUCC") || NULL != strstr(pBuf, "UNICOM")
            || NULL != strstr(pBuf, "46001") || NULL != strstr(pBuf, "46006") 
            || NULL != strstr(pBuf, "46009"))
    {
        uNetopt = WIRELESS_CUCC;
    }

    return uNetopt;
}

/* ͨ��IMSI�ж���Ӫ�� */
uint8 get_operator_IMSI(uint8 *pBuf)
{
    /* �й��ƶ�CMCC: 46000 46002 46004 46007 */
    /* �й�����CTCC: 46003 46005 46011 */
    /* �й���ͨCUCC: 46001 46006 46009 */
    uint8 uNetopt = WIRELESS_OPT_NONE;
    
    /*��֤AT����Ӧ���Ƿ���ȷ*/
    if(0 == memcmp(pBuf, "46000", 5) || 0 == memcmp(pBuf, "46002", 5)
       || 0 == memcmp(pBuf, "46004", 5) || 0 == memcmp(pBuf, "46007", 5))
    {   
        uNetopt = WIRELESS_CMCC;
    }
    else if(0 == memcmp(pBuf, "46003", 5) || 0 == memcmp(pBuf, "46005", 5) 
            || 0 == memcmp(pBuf, "46011", 5))
    {
        uNetopt = WIRELESS_CTCC;
    }
    else if(0 == memcmp(pBuf, "46001", 5) || 0 == memcmp(pBuf, "46006", 5) 
            || 0 == memcmp(pBuf, "46009", 5))
    {
        uNetopt = WIRELESS_CUCC;
    }

    return uNetopt;
}

/* ͨ��CCID�ж���Ӫ�� */
uint8 get_operator_CCID(uint8 *pBuf)
{
    /* �й��ƶ�CMCC: 898600 898602 898604 898607 */
    /* �й�����CTCC: 898603 898611 */
    /* �й���ͨCUCC: 898601 898606 898609 */
    uint8 uNetopt = WIRELESS_OPT_NONE;
    
    /*��֤AT����Ӧ���Ƿ���ȷ*/
    if(0 == memcmp(pBuf, "898600", 6) || 0 == memcmp(pBuf, "898602", 6)
       || 0 == memcmp(pBuf, "898604", 6) || 0 == memcmp(pBuf, "898607", 6))
    {   
        uNetopt = WIRELESS_CMCC;
    }
    else if(0 == memcmp(pBuf, "898603", 6) || 0 == memcmp(pBuf, "898611", 6))
    {
        uNetopt = WIRELESS_CTCC;
    }
    else if(0 == memcmp(pBuf, "898601", 6) || 0 == memcmp(pBuf, "898606", 6) 
            || 0 == memcmp(pBuf, "898609", 6))
    {
        uNetopt = WIRELESS_CUCC;
    }

    return uNetopt;
}

/**********************************************************************
* @name      : wireless_AT_INIT
* @brief     ������Э��ջAT_INIT ״̬������
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
uint32 wireless_AT_INIT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    UART_DEVICE_T* pUart = NULL; /*���ھ��*/
    uint8 devname[256] = {0};
    
    if(NULL != ptHandle->pUart)
    {
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d ��������ͷ�\n", AT_DEV_NUM(ifnum));
        wireless_uart_uninit(ptHandle->pUart);
        ptHandle->pUart = NULL;
    }

    pUart = (UART_DEVICE_T *)wireless_uart_init(dev); /*�򿪴���*/
    if (NULL == pUart)
    {
        if(stepCnt > 3)
        {
            WIRELESS_FMT_DEBUG("���Ŷ˿�%d ��δ򿪴���%sʧ�� ����ģ��!\n", AT_DEV_NUM(ifnum), dev);
            /* �����豸 */
            return AT_RESTART;
        }
        *delay = 5000;
        return AT_INIT;
    }
    pUart->uart_get_real_dev_name(pUart, devname);
    WIRELESS_FMT_DEBUG("���Ŷ˿�%d �򿪴���%s�ɹ�(�豸%s)\n", AT_DEV_NUM(ifnum), dev, devname);
    ptHandle->pUart = pUart;
    *delay = 1000;
    return AT_ST;
}

/**********************************************************************
* @name      : wireless_AT_ST
* @brief     ������Э��ջAT_ST ״̬������
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
uint32 wireless_AT_ST(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; 
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    
    wireless_uart_recv(pUart, (uint8 *)at_get_buf, sizeof(at_get_buf));
    WIRELESS_FMT_DEBUG("ģ�鿪����: %s\n", at_get_buf);

    return AT_AT;
}

/**********************************************************************
* @name      : wireless_AT_AT
* @brief     ������Э��ջAT_AT ״̬������
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
uint32 wireless_AT_AT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; 
    int  at_status = 0;

    if(0 == at_check_model(ifnum, pUart, 1))
    {
        at_status = at_check(ifnum, pUart, "ATE0\r\n", "OK");     /*���ڷ���AT����*/
        if(0 == at_status)
        {
            MYGMR_INFO_T info;
            if(0 == at_get_mygmr(ifnum, pUart, &info))
            {
                /* �����汾��Ϣ */
                memcpy(ptHandle->ptNetPara->VERINFO.manuNum, info.manufacture, 4);
                memcpy(ptHandle->ptNetPara->VERINFO.softversion, info.firmare_version, 4);
                memcpy(ptHandle->ptNetPara->VERINFO.softdata, info.release_date, 6);
                memcpy(ptHandle->ptNetPara->VERINFO.hwversion, info.hw_version, 4);
                memcpy(ptHandle->ptNetPara->VERINFO.hwdata, info.hw_release_date, 6);
                memcpy(ptHandle->ptNetPara->VERINFO.manuExten, info.model, 8);
            }
        }
        wireless_power_status(TRUE);
        return 0 == at_status ? AT_GMM : AT_RESTART;
    }
    else if(stepCnt >= 0)
    {
        if(ptHandle->sendfaild >= 5)
        {
            if(0xFF == ptHandle->commport)
            {
                WIRELESS_FMT_DEBUG("���Ŷ˿�%d ģ���ʼ��ʧ��, �л����ŷ�ʽ!\n", AT_DEV_NUM(ifnum));
                if(Isusb !=0xAA)
                {
                    wireless_change_type(ptHandle->commtype == WIRELESS_COMMTYPE_UART ? WIRELESS_COMMTYPE_USB : WIRELESS_COMMTYPE_UART);
                }
            }
            else
            {
                WIRELESS_FMT_DEBUG("���Ŷ˿�%d ģ���ʼ��ʧ��, ����ģ��\n", AT_DEV_NUM(ifnum));
                /* �����豸 */
            }
            wireless_hw_reboot(10);
            return AT_RESTART;
        }
        else
        {
            ptHandle->sendfaild += 1;
            *delay = 2000;
            return AT_INIT;
        }
    }
    *delay = 1000;
    return AT_AT;
}

/**********************************************************************
* @name      : wireless_AT_GMM
* @brief     ������Э��ջAT_GMM ״̬������
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
uint32 wireless_AT_GMM(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    int  at_status = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    void *pDev = ptHandle->pUart;

    memset(at_get_buf, 0, sizeof(at_get_buf));
    at_status = at_get(ifnum, pDev, "AT$MYTYPE?\r\n", at_get_buf, sizeof(at_get_buf));
    if (0 != at_status)
    {
        return AT_RESTART;
    }
    
    ptHandle->ptNetPara->moduleType = at_get_module_type(at_get_buf, ptHandle->ptNetPara->nettype);
    wireless_get_IMEI(ifnum, ptHandle->pUart, ptHandle->ptNetPara->IMEI, 17);
    return AT_CPIN;
}

/**********************************************************************
* @name      : wireless_AT_CPIN
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
uint32 wireless_AT_CPIN(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    void *pUart = ptHandle->pUart; /*���ھ��*/
    int   ret = 0;
    uint16 CheckTime = ptHandle->ptNetPara->SimCheckTime;
    
    if(stepCnt < 1)
    {
        at_get(ifnum, pUart, "AT+CPIN?\r\n", at_get_buf, AT_LENTH); 
    }
    else
    {
        /* û��SIM�� 10s����һ��GPS */
        at_get_nolog(ifnum, pUart, "AT+CPIN?\r\n", at_get_buf, AT_LENTH); 
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
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d ��⵽SIM������\n", ifnum);
        return AT_CCID;
    }

    if(CheckTime > 0 && stepCnt > CheckTime * 6)
    {
        /* �ȶ��Բ��Թ����п��ܵ���ģ��ʶ��SIM������, ��Ҫ��λ���лָ�. ֻ�ڲ���������ʵ�ּ��� */
        WIRELESS_FMT_DEBUG("%s%d ģ��%d_%dû�м�⵽SIM��, ����!\n", AT_DEV_NAMENUM(ifnum), ptHandle->commport, 
                                                                     ptHandle->subport);
        return AT_RESTART;
    }

    if(stepCnt >= 12 && 0 == stepCnt % 6)
    {
        memset(at_get_buf, 0, AT_LENTH);
        sprintf(at_get_buf, "�豸%dû�м�⵽SIM��", ifnum + 1);
        wireless_send_diagStatus(ifnum, at_get_buf, strlen(at_get_buf), 10);
    }
    
    if(stepCnt < 1)
    {
        WIRELESS_FMT_DEBUG("%s%d ģ��%d_%dû�м�⵽SIM��, �������!\n", AT_DEV_NAMENUM(ifnum), ptHandle->commport, 
                                                                         ptHandle->subport);
    }
    *delay = 10000;
    return AT_CPIN;
}

/**********************************************************************
* @name      : wireless_AT_CCID
* @brief     ������Э��ջAT_CCID ״̬������
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
uint32 wireless_AT_CCID(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    char *temp = NULL;
    void *pDev = ptHandle->pUart; 
    
    if(stepCnt < 1)
    {
        at_get(ifnum, pDev, "AT$MYCCID\r\n", at_get_buf, sizeof(at_get_buf)); /*��ȡSIM��ϵ�к�*/
    }
    else
    {
        at_get_nolog(ifnum, pDev, "AT$MYCCID\r\n", at_get_buf, sizeof(at_get_buf)); /*��ȡSIM��ϵ�к�*/
    }
	
    temp = strstr((char *)at_get_buf, "$MYCCID");
    /*��δ�յ�'ERROR'����SIM������*/
    if (NULL != temp) /*���SIM���ɹ���֪ͨӦ�ò㣬������һ��*/
    {
        /* д��CCID */
        ptHandle->bsimOK = 0;
        memcpy(ptHandle->ptNetPara->ICCID, temp+strlen("$MYCCID:")+2, 20);
        *delay = 1000;
        return WIRELESS_COMMTYPE_USB == ptHandle->commtype ? AT_CSQ : AT_CIMI;;
    }

    if(stepCnt >= 12)
    {
        WIRELESS_FMT_DEBUG("%s%d ģ��%d_%dû�ж���SIM����, ����!\n", AT_DEV_NAMENUM(ifnum), ptHandle->commport, 
                                                                     ptHandle->subport);
        return AT_RESTART;
    }
    
    if(stepCnt < 1)
    {
        WIRELESS_FMT_DEBUG("%s%d ģ��%d_%dû�ж���SIM����, �������!\n", AT_DEV_NAMENUM(ifnum), ptHandle->commport, 
                                                                         ptHandle->subport);
    }
    *delay = 10000;
    return AT_CCID;
}

/**********************************************************************
* @name      : wireless_AT_CIMI
* @brief     ������Э��ջAT_CIMI ״̬������
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
uint32 wireless_AT_CIMI(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; /*���ھ��*/
    char  at_get_buf[AT_LENTH] = {0};   /*�豸���ͻ�����*/
    int   at_status = 0;

    /*��ȡSIM��ϵ�к�*/
    memset(at_get_buf, 0, AT_LENTH);
    at_status = at_get(ifnum, pUart, "AT+CIMI\r\n", at_get_buf, AT_LENTH); 
    if (0 != at_status)
    {   
        return AT_RESTART;
    }
    
    /*��δ�յ�'ERROR'����SIM������*/
    if(NULL == strstr(at_get_buf, "ERROR")) /*���SIM���ɹ���֪ͨӦ�ò㣬������һ��*/
    {
        WIRELESS_FMT_DEBUG("%s%d ��⵽SIM��, IMSI: %s\n", AT_DEV_NAMENUM(ifnum), at_get_buf);
        wireless_get_IMSI(at_get_buf, ptHandle->ptNetPara->IMSI, 15);
    }
    else
    {
        WIRELESS_FMT_DEBUG("%s%d û�л�ȡ��IMSI\n", AT_DEV_NUM(ifnum));
    }

    *delay = 100;
    return AT_CSQ;
}


/**********************************************************************
* @name      : wireless_AT_CSQ
* @brief     ������Э��ջAT_CSQ ״̬������
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
uint32 wireless_AT_CSQ(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char               *ptemp;
    int          signal_value;
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    void *pDev = ptHandle->pUart;
    int  at_status = 0;

    if(stepCnt < 1)
    {
        at_status = at_get(ifnum, pDev, "AT+CSQ\r\n", at_get_buf, sizeof(at_get_buf)); /*����AT+CSQ*/
        ptemp = strstr((char *)at_get_buf, "+CSQ:");
        signal_value = at_get_csqvalue(ptemp);
        if(signal_value <= 0)
        {
            memset(at_get_buf, 0, sizeof(at_get_buf));
            at_status = at_get(ifnum, pDev, "AT+CESQ\r\n", at_get_buf, sizeof(at_get_buf));
            if (0 == at_status)
            {
                ptemp = strstr((char *)at_get_buf, "+CESQ:");
                signal_value = at_get_cesqvalue(ptemp);
            }
        }
        if(0 != at_status)
        {
            return AT_RESTART;
        }

    }
    else
    {
        signal_value = wireless_get_csqvalue(ifnum, pDev);
    }

    if(WIRELESS_COMMTYPE_USB != ptHandle->commtype)
    {
        ptHandle->ptNetPara->networkType = WIRELESS_OPT_NONE;
        ptHandle->ptNetPara->csqvalue = signal_value;
        wireless_send_change(ifnum, ptHandle->ptNetPara->networkType, (uint16)ptHandle->ptNetPara->csqvalue);
    }
    
    /*�źŵĴ�С�����ڴ���6�������ź���,�����ʽ��+CSQ: 9,99 */
    if (signal_value >= MIN_CSQ_VALUE)
    {
        return AT_SYSINFO;
    }
    else if(stepCnt >= AT_GET_CNT)
    {
        WIRELESS_FMT_DEBUG("%s%d�ź�ǿ�ȹ���, �����л�APN!\n", AT_DEV_NAMENUM(ifnum));
        wireless_set_nextapn(ptHandle);
        /* �����豸 */
        return AT_RESTART;
    }
    if(stepCnt < 1)
    {
        WIRELESS_FMT_DEBUG("%s%d�ź�ǿ�ȹ���, ���¼��!\n", AT_DEV_NAMENUM(ifnum));
    }
    *delay = 5000;
    return AT_CSQ;
}

/**********************************************************************
* @name      : wireless_AT_SYSINFO
* @brief     ������Э��ջAT_SYSINFO ״̬������
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
uint32 wireless_AT_SYSINFO_default(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    char opera[4][20] = {"�й��ƶ�", "�й���ͨ", "�й�����", "ר��"};
    char network[4][5] = {"2G", "3G", "4G", "5G"};
    void *pDev = ptHandle->pUart; 

    uint8 operat = WIRELESS_OPT_NONE;
    uint8 networkType = 0xff;

    if(stepCnt < 1)
    {
        /* ���������������� */
        sprintf(at_get_buf,"AT$MYSYSINFO=%d\r\n", 
        ptHandle->ptNetPara->networkSet < 4 ? ptHandle->ptNetPara->networkSet + 1 : 1);
        at_check(ifnum, pDev, at_get_buf, "OK");
        memset(at_get_buf, 0, AT_LENTH);
        //��ѯ��������(��Ӫ��2G,3G,4G,5G)
        at_get(ifnum, pDev, "AT+COPS?\r\n", at_get_buf, sizeof(at_get_buf));     /*����AT+CREG?*/
    }
    else
    {
        //��ѯ��������(��Ӫ��2G,3G,4G,5G)
        at_get_nolog(ifnum, pDev, "AT+COPS?\r\n", at_get_buf, sizeof(at_get_buf));     /*����AT+CREG?*/
    }
    /* ��ȡ��Ӫ������ */
    operat = get_operator_code(at_get_buf);
    if(WIRELESS_OPT_NONE == operat)
    {
        /* ע����Ϣ��ѯ����������ͨ��IMSI CCID�ж���Ӫ�� */
        operat = get_operator_IMSI(ptHandle->ptNetPara->IMSI);
        if(WIRELESS_OPT_NONE == operat)
        {
            operat = get_operator_CCID(ptHandle->ptNetPara->ICCID);
        }
    }
    if(WIRELESS_OPT_NONE == operat)
    {
        if (stepCnt > 70)
        {
            WIRELESS_FMT_DEBUG("%s%d��ȡ��Ӫ��ʧ��, ����!\n", AT_DEV_NAMENUM(ifnum));
            /* �����豸 */
            return AT_RESTART;
        }
        if(stepCnt < 1)
        {
            WIRELESS_FMT_DEBUG("%s%d��ȡ��Ӫ��: δ֪, ����!\n", AT_DEV_NAMENUM(ifnum));
        }
        *delay = 2000;
        return AT_SYSINFO;
    }

    networkType = wireless_get_network(ifnum, pDev, NULL, TRUE);
    ptHandle->ptNetPara->operat = operat;
    ptHandle->ptNetPara->networkType = networkType;

    WIRELESS_FMT_DEBUG("%s%d��ȡ��Ӫ��: %s, ��������: %s\n", AT_DEV_NAMENUM(ifnum), 
                       operat <= WIRELESS_ZWCC ? opera[operat] : "δ֪", 
                       networkType <= OOP_5G ? network[networkType] : "������");

    if(WIRELESS_COMMTYPE_USB != ptHandle->commtype)
    {
        wireless_send_change(ifnum, ptHandle->ptNetPara->networkType, (uint16)ptHandle->ptNetPara->csqvalue);
    }

    /* ��ȡָ����APN */
    if(0 != wireless_get_apn(ptHandle))
    {
        WIRELESS_FMT_DEBUG("%s%dƥ�䲦�Ų���ʧ��: ����!\n", AT_DEV_NAMENUM(ifnum));
        *delay = 5000;
        return AT_SYSINFO;
    }

    //����APN
    memset(at_get_buf, 0, sizeof(at_get_buf));
    sprintf(at_get_buf,"AT$MYNETCON=%d,\"APN\",\"%s\"\r\n",0, ptHandle->apn.value);
    at_check(ifnum, pDev, at_get_buf, "OK");

    //�����û�������
    memset(at_get_buf, 0, sizeof(at_get_buf));
    if(0 == ptHandle->user.nNum && 0 == ptHandle->pwd.nNum)
    {
        WIRELESS_FMT_DEBUG("����%dû���û���������!\n", AT_DEV_NUM(ifnum));
    }
    else
    {
        sprintf(at_get_buf, "AT$MYNETCON=%d,\"USERPWD\",\"%s,%s\"\r\n", 0, ptHandle->user.value, ptHandle->pwd.value);
        at_check(ifnum, pDev, at_get_buf, "OK");
    }
    //���ü�Ȩģʽ
    if(ptHandle->ptNetPara->authType < 4)
    {
        memset(at_get_buf, 0, sizeof(at_get_buf));
        if(1 == ptHandle->ptNetPara->authType)
        {
            sprintf(at_get_buf, "AT$MYNETCON=%d,\"AUTH\",%d\r\n", 0, 2);
        }
        else if(2 == ptHandle->ptNetPara->authType)
        {
            sprintf(at_get_buf, "AT$MYNETCON=%d,\"AUTH\",%d\r\n", 0, 1);
        }
        else
        {
            sprintf(at_get_buf, "AT$MYNETCON=%d,\"AUTH\",%d\r\n", 0, ptHandle->ptNetPara->authType);
        }
        at_check(ifnum, pDev, at_get_buf, "OK");
    }

    return AT_CREG;
}

/**********************************************************************
* @name      : wireless_AT_CREG_JS
* @brief     ������Э��ջAT_CREG ״̬������(����)
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
uint32 wireless_AT_SYSINFO_JS(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    char opera[4][20] = {"�й��ƶ�", "�й���ͨ", "�й�����", "ר��"};
    char network[4][5] = {"2G", "3G", "4G", "5G"};
    void *pDev = ptHandle->pUart; 
    char *temp = NULL;
    uint8 ccid[21] = {0};
    
    uint8 operat = WIRELESS_OPT_NONE;
    uint8 networkType = 0xff;

    if(stepCnt < 1)
    {
        /* ���������������� */
        sprintf(at_get_buf,"AT$MYSYSINFO=%d\r\n", 
        ptHandle->ptNetPara->networkSet < 4 ? ptHandle->ptNetPara->networkSet + 1 : 1);
        at_check(ifnum, pDev, at_get_buf, "OK");
    }
    
    networkType = wireless_get_network(ifnum, pDev, &operat, TRUE);
    if(WIRELESS_OPT_NONE == operat)
    {
        if (stepCnt > 24)
        {
            WIRELESS_FMT_DEBUG("%s%d����ʧ��, ����!\n", AT_DEV_NAMENUM(ifnum));
            /* �����豸 */
            return AT_RESTART;
        }
        if(stepCnt < 1)
        {
            WIRELESS_FMT_DEBUG("%s%d�ȴ�����, ����!\n", AT_DEV_NAMENUM(ifnum));
        }
        *delay = 5000;
        return AT_SYSINFO;
    }

    ptHandle->ptNetPara->operat = operat;
    ptHandle->ptNetPara->networkType = networkType;

    WIRELESS_FMT_DEBUG("%s%d��ȡ��Ӫ��: %s, ��������: %s\n", AT_DEV_NAMENUM(ifnum), 
                       operat <= WIRELESS_ZWCC ? opera[operat] : "δ֪", 
                       networkType <= OOP_5G ? network[networkType] : "������");

    if(WIRELESS_COMMTYPE_USB != ptHandle->commtype)
    {
        wireless_send_change(ifnum, ptHandle->ptNetPara->networkType, (uint16)ptHandle->ptNetPara->csqvalue);
    }

    /* ����CCID */
    memset(at_get_buf, 0, sizeof(at_get_buf));
    at_get_nolog(ifnum, pDev, "AT$MYCCID\r\n", at_get_buf, sizeof(at_get_buf)); /*��ȡSIM��ϵ�к�*/
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

    /* ��ȡָ����APN */
    if(0 != wireless_get_apn(ptHandle))
    {
        WIRELESS_FMT_DEBUG("%s%dƥ�䲦�Ų���ʧ��: ����!\n", AT_DEV_NAMENUM(ifnum));
        *delay = 5000;
        return AT_SYSINFO;
    }

    //����APN
    memset(at_get_buf, 0, sizeof(at_get_buf));
    sprintf(at_get_buf,"AT$MYNETCON=%d,\"APN\",\"%s\"\r\n",0, ptHandle->apn.value);
    at_check(ifnum, pDev, at_get_buf, "OK");

    //�����û�������
    memset(at_get_buf, 0, sizeof(at_get_buf));
    if(0 == ptHandle->user.nNum && 0 == ptHandle->pwd.nNum)
    {
        WIRELESS_FMT_DEBUG("����%dû���û���������!\n", AT_DEV_NUM(ifnum));
    }
    else
    {
        sprintf(at_get_buf, "AT$MYNETCON=%d,\"USERPWD\",\"%s,%s\"\r\n", 0, ptHandle->user.value, ptHandle->pwd.value);
        at_check(ifnum, pDev, at_get_buf, "OK");
    }
    //���ü�Ȩģʽ
    if(ptHandle->ptNetPara->authType < 4)
    {
        memset(at_get_buf, 0, sizeof(at_get_buf));
        if(1 == ptHandle->ptNetPara->authType)
        {
            sprintf(at_get_buf, "AT$MYNETCON=%d,\"AUTH\",%d\r\n", 0, 2);
        }
        else if(2 == ptHandle->ptNetPara->authType)
        {
            sprintf(at_get_buf, "AT$MYNETCON=%d,\"AUTH\",%d\r\n", 0, 1);
        }
        else
        {
            sprintf(at_get_buf, "AT$MYNETCON=%d,\"AUTH\",%d\r\n", 0, ptHandle->ptNetPara->authType);
        }
        at_check(ifnum, pDev, at_get_buf, "OK");
    }

    return AT_CREG;
}

/**********************************************************************
* @name      : wireless_AT_SYSINFO
* @brief     ������Э��ջAT_SYSINFO ״̬������
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
uint32 wireless_AT_SYSINFO(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    if(0 == ptHandle->bJS1_8G)
    {
        return wireless_AT_SYSINFO_default(ifnum, dev, stepCnt, cnt, ptHandle, delay);
    }
    else
    {
        return wireless_AT_SYSINFO_JS(ifnum, dev, stepCnt, cnt, ptHandle, delay);
    }
}

/**********************************************************************
* @name      : wireless_AT_CREG
* @brief     ������Э��ջAT_CREG ״̬������
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
uint32 wireless_AT_CREG(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    if(0 == wireless_check_newwork_reg(ifnum, ptHandle->pUart, TRUE))
    {
        return AT_CGDCONT;
    }
    else
    {
        if (stepCnt > 30)
        {
            WIRELESS_FMT_DEBUG("����%d�������û��ע��, �����л�APN!\n", AT_DEV_NUM(ifnum));
            wireless_set_nextapn(ptHandle);
            /* �����豸 */
            return AT_RESTART;
        }
        else
        {
            *delay = 10000;
            return AT_CREG;
        }
    } 
}

/**********************************************************************
* @name      : wireless_AT_CGDCONT
* @brief     ������Э��ջAT_CGDCONT ״̬������
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
uint32 wireless_AT_CGDCONT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    int              at_status = 0;
    char  at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    void *pDev = ptHandle->pUart;

    /* ����PDP���ͣ�IP-TCP/IP, PPP-��Ե�Э�� */
    sprintf((char *)at_get_buf, "AT+CGDCONT=1,\"IP\",\"%s\"\r\n", ptHandle->apn.value);
    at_status = at_check(ifnum, pDev, at_get_buf, "OK");
    if (0 != at_status)
    {
        return AT_RESTART;
    }
    else
    {
        *delay = 100;
        return AT_CGATT;
    }
}

/**********************************************************************
* @name      : wireless_AT_CGATT
* @brief     ������Э��ջAT_CGATT ״̬������
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
uint32 wireless_AT_CGATT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    int             at_status = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT���������*/
    void *pDev = ptHandle->pUart;

    //�ȷ���һ��CGATT����ʹģ������
    at_status = at_check(ifnum, pDev, "AT+CGATT=1\r\n", "OK");
    at_status = at_get(ifnum, pDev, "AT+CGATT?\r\n", at_get_buf, sizeof(at_get_buf));/*����AT+CREG?*/
    if (0 != at_status)
    {
        return AT_RESTART;
    }
    
    /*���յ����ַ����Ƿ�Ϊ +CGATT: 1*/
    if (0 == is_str_exist((char *)at_get_buf, "1"))
    {
        *delay = 100;
        return WIRELESS_COMMTYPE_USB == ptHandle->commtype ? AT_ATDT : AT_GETINFO;
    }
    else if(stepCnt > 10)
    {
        wireless_set_nextapn(ptHandle);
        WIRELESS_FMT_DEBUG("%s%dģ��û������, �л�APN\n", AT_DEV_NAMENUM(ifnum));
        ptHandle->ConnectNum++;
        wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 0, ptHandle->ConnectNum);
        ptHandle->FailedNum++;
        wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
        return AT_RESTART;
    }
    if(stepCnt < 1)
    {
        WIRELESS_FMT_DEBUG("%s%dģ��û������, ����\n", AT_DEV_NAMENUM(ifnum));
    }
    *delay = 2000;
    return AT_CGATT;
}

/**********************************************************************
* @name      : wireless_AT_CGATT
* @brief     ������Э��ջAT_GETINFO ״̬������
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
uint32 wireless_AT_GETINFO(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pDev = ptHandle->pUart;

    /* �ź�ǿ�� �� ������ʽ */
    ptHandle->ptNetPara->csqvalue = wireless_get_csqvalue(ifnum, pDev);
    ptHandle->ptNetPara->networkType = wireless_get_network(ifnum, pDev, NULL, FALSE);
    wireless_send_change(ifnum, ptHandle->ptNetPara->networkType, ptHandle->ptNetPara->csqvalue);

    wireless_get_phonenum(ifnum, ptHandle->pUart, ptHandle->ptNetPara->SIMNum, 24);

    *delay = 100;
    return AT_ATDT;
}

/**********************************************************************
* @name      : wireless_AT_ATDT
* @brief     ������Э��ջAT_ATDT ״̬������
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
uint32 wireless_AT_ATDT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    int   at_status = 0;
    uint8 ch_data_at = 0;    /* pppģʽ����ģʽ�л������� */
    void *pDev = ptHandle->pUart;

    ptHandle->ConnectNum++;
    wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 0, ptHandle->ConnectNum);

    if(*cnt != 0)//ppp����ʧ�ܽ������´򿪴��ڽ�ģ�������̬�л���at̬
    {
        wireless_uart_uninit(pDev);
        sleep(2);
        pDev = wireless_uart_init(dev); /*�򿪴���*/
        if(NULL == pDev)
        {
            ptHandle->pUart = NULL;
            WIRELESS_FMT_DEBUG("���´򿪴���%sʧ��\n", dev);
            return AT_RESTART;
        }
        WIRELESS_FMT_DEBUG("���´򿪴���%s, �л���atģʽ\n", dev);
        ptHandle->pUart = pDev;
        for(ch_data_at = 0; ch_data_at<3; ch_data_at++)
        {
            wireless_uart_send(ptHandle->pUart, (uint8 *)"+", 1);//д+++ 
            usleep(10000);
        }
        at_check(ifnum, pDev, "AT\r\n", "OK");       /*���ڷ���AT����*/
    }

    if(1 == ptHandle->ptNetPara->moduleType) /*��⵽GPRSģ���ģ���ͺ�δ֪*/
    {
        /* CDMA2000 */
        at_status = at_check(ifnum, pDev, "ATDT#777\r\n", "CONNECT");
    }
    else
    {
        //����Ŀǰ�кܶ�ģ�鷢��ATDT����������ո�Ҳ�в���ģ�鲻��Ҫ���ո�
        //Ϊ�˶�����ģ�������ݣ����������Թ��ܣ������������һ��
        //ATDT��1, ��Ӧ����ͨ��0
        at_status = at_check(ifnum, pDev, "ATDT *99***1#\r\n", "CONNECT");
        if(0 != at_status)
        {
            at_status = at_check(ifnum, pDev, "ATDT*99***1#\r\n", "CONNECT");
        }
    }
    if (0 != at_status)
    {
        wireless_set_nextapn(ptHandle);
        WIRELESS_FMT_DEBUG("%s%d�л�APN\n", AT_DEV_NAMENUM(ifnum));
        *delay = 5000;
        return AT_RESTART;
    }
    else
    {
        return AT_PPP;
    }
}

/**********************************************************************
* @name      : wireless_AT_PPP
* @brief     ������Э��ջAT_PPP ״̬������
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
uint32 wireless_AT_PPP(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char      ppp_command[600] = {0};  /* ppp���� */
    char      devname[128] = {0};
    char      filename[128] = {0};
    char      cmd[128] = {0};
    uint8     index = 0;

    if(0 == ptHandle->devName[0])
    {
        if(0 == ptHandle->subport)
        {
            sprintf(devname, "/tmp/dev/usb_exm%d_2", ptHandle->commport);
        }
        else
        {
            sprintf(devname, "/tmp/dev/usb_exm%d_%d_2", ptHandle->commport, ptHandle->subport);
        }
    }
    else
    {
        sprintf(devname, "%s", ptHandle->devName);
    }

    if(0 == ptHandle->pppdlogMax)
    {
        sprintf(ppp_command, "pppd debug noauth unit %d nocrtscts modem %s 115200 asyncmap 0000000 "
                             "escape FF kdebug 0 0.0.0.0:0.0.0.0 noipdefault netmask 255.255.255.0 defaultroute "
                             "user \"%s\" password \"%s\" connect \"\" lcp-echo-interval 60 lcp-echo-failure 4%s",
                             ifnum, devname, ptHandle->user.value, ptHandle->pwd.value, 
                             0 == ptHandle->bIPV6 ? " noipv6" : "");
    }
    else
    {
        ptHandle->logNo++;
        if(ptHandle->logNo >= ptHandle->pppdlogMax)
        {
            ptHandle->logNo = 0;
        }
        index = ptHandle->logNo;
        sprintf(filename, "%s/ppp%d.%d.log", ptHandle->logpath, ifnum, index);
        if(0 == access(filename, F_OK))
        {
            sprintf(cmd, "rm %s/ppp%d.%d.log", ptHandle->logpath, ifnum, index);
            system(cmd);
            usleep(100000);
        }

        sprintf(ppp_command, "pppd debug noauth unit %d nocrtscts modem %s 115200 asyncmap 0000000 "
                             "escape FF kdebug 0 0.0.0.0:0.0.0.0 noipdefault netmask 255.255.255.0 defaultroute "
                             "user \"%s\" password \"%s\" connect \"\" lcp-echo-interval 60 lcp-echo-failure 4%s "
                             "logfile %s/ppp%d.%d.log",
                             ifnum, devname, ptHandle->user.value, ptHandle->pwd.value, 
                             0 == ptHandle->bIPV6 ? " noipv6" : "", ptHandle->logpath, ifnum, index);
    }
    if (TRUE == wireless_check_ppp_porc(ifnum))
    {
        WIRELESS_FMT_TRACE("pppd was running\n");
    }
    else
    {
        WIRELESS_FMT_DEBUG("\n%s\n", ppp_command);
        system(ppp_command);
    }
    if(ptHandle->pppdlogMax > 0)
    {
        wireless_write_logno(ptHandle->userfile, ifnum + 1, ptHandle->logNo);
    }
    *delay = 1000;
    return AT_LCP;
}

/**********************************************************************
* @name      : wireless_AT_LCP
* @brief     ������Э��ջAT_PPP1 ״̬������
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
uint32 wireless_AT_LCP(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    /* ��������Ƿ���� */
    if (TRUE == wireless_check_pppif(ifnum))
    {
        WIRELESS_FMT_DEBUG("%s%d pppd����ִ�гɹ�, ��������ppp%d!\n", AT_DEV_NAMENUM(ifnum), ifnum);
        return AT_IPCP;
    }
    else if(stepCnt > 30)
    {
        if(*cnt > 2)
        {
            *cnt = 0;
            ptHandle->FailedNum++;
            wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
            wireless_set_nextapn(ptHandle);
            WIRELESS_FMT_DEBUG("%s%d PPPʧ��, û��ӳ������, �л�APN\n", AT_DEV_NAMENUM(ifnum));
            *delay = 5000;
            return AT_RESTART;
        }
        else
        {
            *cnt += 1;
            ptHandle->FailedNum++;
            wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
            WIRELESS_FMT_DEBUG("%s%d PPPʧ��, û��ӳ������, ����\n", AT_DEV_NAMENUM(ifnum));
            return AT_ATDT;
        }
    }

    *delay = 1000;
    return AT_LCP;
}

/**********************************************************************
* @name      : wireless_AT_IPCP
* @brief     ������Э��ջAT_PPP2 ״̬������
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
uint32 wireless_AT_IPCP(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    uint32    ipAddr = 0;
    
    /* ��ȡip��ַ */
    ipAddr = wireless_get_pppIP(ifnum);
    if (0 != ipAddr)
    {
        WIRELESS_FMT_DEBUG("%s%d PPP�ɹ�, ���ڵ�ַ  %d.%d.%d.%d!\n", AT_DEV_NAMENUM(ifnum), 
                           (ipAddr >> 24) & 0xff, (ipAddr >> 16) & 0xff, (ipAddr >> 8) & 0xff, ipAddr & 0xff);

        ptHandle->ptNetPara->ipAddr = ipAddr;
        sleep(1);
        ptHandle->ptNetPara->getway = wireless_get_pppgetway(ifnum); /* �������� */
        WIRELESS_FMT_DEBUG("%s%d ���� %d.%d.%d.%d!\n", AT_DEV_NAMENUM(ifnum), 
                           (ptHandle->ptNetPara->getway >> 24) & 0xff, (ptHandle->ptNetPara->getway >> 16) & 0xff, 
                           (ptHandle->ptNetPara->getway >> 8) & 0xff, ptHandle->ptNetPara->getway & 0xff);
        *cnt = 0;
        return AT_WAIT;
    }
    else if(stepCnt > 120)
    {
        /* ��Щ�ź�����ģ��, ʱ�䳤 */
        if (*cnt > 2)      /*���2�ν���PPP���ɹ�������Ϊʧ��*/
        {
            *cnt = 0;
            ptHandle->FailedNum++;
            wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
            wireless_set_nextapn(ptHandle);
            wireless_ppp_off(ifnum);//û�л�ȡ����ip��ַ������Ϸ����������
            WIRELESS_FMT_DEBUG("%s%d PPPʧ��, û�з��䵽IP, �л�APN\n", AT_DEV_NAMENUM(ifnum));
            *delay = 5000;
            return AT_RESTART;
        }
        else    /*����3��PPP����*/
        {
            WIRELESS_FMT_DEBUG("%s%d PPPʧ��, û�з��䵽IP, ��������\n", AT_DEV_NAMENUM(ifnum));
            wireless_ppp_off(ifnum);//û�л�ȡ����ip��ַ������Ϸ����������
            *cnt += 1;
            ptHandle->FailedNum++;
            wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
            return AT_ATDT;
        }
    }
    
    *delay = 1000;
    return AT_IPCP;
}

/**********************************************************************
* @name      : wireless_AT_RESTART
* @brief     ������Э��ջAT_RESTART ״̬������
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
uint32 wireless_AT_RESTART(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    WIRELESS_FMT_DEBUG("���Ŷ˿�%d ����ʧ��\n", AT_DEV_NUM(ifnum));
    if(ptHandle->pUart && 0 == ptHandle->bJS1_8G)
    {
        wireless_uart_uninit(ptHandle->pUart);
        ptHandle->pUart = NULL;
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d ����ͷ�\n", AT_DEV_NUM(ifnum));
    }

    /* �����豸 */
    wireless_restart_dev(ifnum, ptHandle->commport + ((ptHandle->subport << 4) & 0xf0));

    *delay = 20000;
    //ptHandle->workAgain = TRUE; //�˳���ǰ״̬��
    return AT_RESTART;
}

/**********************************************************************
* @name      : wireless_AT_WAIT
* @brief     ������Э��ջAT_WAIT ״̬������
* @param[in] ��uint8 ifnum                  ���ڱ��
               char *dev                    �豸����
               uint32 stepCnt               ״̬����ϵִ�д���
               uint32 *cnt                  �ڲ�����
               WIRELESS_HANDLE_T *ptHandle  �߳̾��
* @param[out]��uint32 *delaytimems          ״̬���л���ʱʱ��, ��λms, ��Сʶ��10ms
* @return    ����һ��״̬��
* @Create    : ����
* @Date      ��2020-11-26
* @Update    :
**********************************************************************/
uint32 wireless_AT_WAIT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    if(TRUE == ptHandle->ptNetPara->smsStatusOK)
    {
        return PPP_SUCCESS;
    }
    *delay = 1000;
    return AT_WAIT;
}

/**********************************************************************
* @name      : wireless_PPP_SUCCESS
* @brief     ������Э��ջPPP_SUCCESS ״̬������
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
uint32 wireless_PPP_SUCCESS(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    time_t stime = 0;
    WIRELESS_FMT_DEBUG("���Ŷ˿�%d ���ųɹ�\n", AT_DEV_NUM(ifnum));
    
    /* ���һ�γɹ�ʱ�� */
    get_curtime_ms(&ptHandle->Connecttime);
    stime = time(NULL);
    ptHandle->SuccessTimes = stime;
    wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 2, ptHandle->SuccessTimes);
    if(ptHandle->pUart)
    {
        wireless_uart_uninit(ptHandle->pUart);
        ptHandle->pUart = NULL;
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d ����ͷ�\n", AT_DEV_NUM(ifnum));
    }
    
    ptHandle->ptNetPara->mask = wireless_get_pppMASK(ifnum); 
    ptHandle->dialState = PPP_DAIL_STATUS_ONLINE;
    ptHandle->ptNetPara->getway = wireless_get_pppgetway(ifnum); /* ��ȡ���� */

    wireless_set_pppstatus(ifnum, 0);
    return PPP_CHECK;
}

/**********************************************************************
* @name      : wireless_PPP_CHECK
* @brief     ������Э��ջPPP_CHECK ״̬������
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
uint32 wireless_PPP_CHECK(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    uint32        ipAddr = 0;

    ipAddr = wireless_get_pppIP(ifnum);
    ptHandle->ptNetPara->ipAddr = ipAddr;
    if(0 == ipAddr)
    {
        *cnt += 1;

        /* ���ڲ�ѯ */
        if(FALSE == wireless_check_pppif(ifnum))
        {
            WIRELESS_FMT_DEBUG("���Ŷ˿�%d ��鲻��������\n", AT_DEV_NUM(ifnum));
            *delay = 2000;
            ptHandle->workAgain = TRUE; //�˳���ǰ״̬��
            ptHandle->FailedNum++;
            wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
            return AT_RESTART;
        }
        
        /* ���ڲ��ܻ�ȡIP����, �Ǳ������� */
        WIRELESS_FMT_DEBUG("���Ŷ˿�%d �������ip��ַʧ��\n", AT_DEV_NUM(ifnum));
        if(*cnt > 30)
        {
            WIRELESS_FMT_DEBUG("���Ŷ˿�%d �����������ip������ ���²���\n", AT_DEV_NUM(ifnum));
            *delay = 2000;
            ptHandle->workAgain = TRUE; //�˳���ǰ״̬��
            ptHandle->FailedNum++;
            wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
            return AT_RESTART;
        }
        *delay = 1000;
        return PPP_CHECK;
    }

#if 0
    /* ����ͳ�� Ŀǰû���õ� */
    lastTime = ptNetPara->usertime;
    get_curtime_ms(&curTime);
    curFlow = wireless_get_flow(ifnum);
    if(curTime.year != lastTime.year || curTime.month != lastTime.month)
    {
        /* ����������0 */
        ptNetPara->usedflow = 0;
        wireless_write_userflow(ptHandle->userfile, ifnum + 1, ptNetPara->usedflow);
        ptHandle->lastFlow = curFlow;
        ptNetPara->usertime = curTime;
        bFisrtMoreValue = TRUE;
    }
    else if(curFlow < ptHandle->lastFlow)  //�쳣����� С����ʷֵ
    {
        ptHandle->lastFlow = curFlow;
    }
    else if(curFlow - ptHandle->lastFlow > 1024 || curTime.minute != lastTime.minute)
    {   //����1KB, ���ߴ���1min��¼
        ptNetPara->usedflow += (curFlow - ptHandle->lastFlow);
        wireless_write_userflow(ptHandle->userfile, ifnum + 1, ptNetPara->usedflow);

        ptHandle->lastFlow = curFlow;
        ptNetPara->usertime = curTime;
    }
    
    /* �������������ȼ�¼���� */
    if(0 < ptNetPara->flowSetValue && TRUE == bFisrtMoreValue)
    {
        if(ptNetPara->flowSetValue < ptNetPara->usedflow/1024)
        {
            bFisrtMoreValue = FALSE;
            WIRELESS_FMT_DEBUG("���Ŷ˿�%d ���������޶�ֵ%uKB\n", AT_DEV_NUM(ifnum), ptNetPara->flowSetValue);
        }
    }
#endif

    *delay = 1000;
    return PPP_CHECK;
}

/**********************************************************************
* @name      : wireless_grps_working
* @brief     ������Э��ջgrps����
* @param[in] ��uint8 commport               �˿ں�
               uint8 ifnum                  ���ڱ��
               WIRELESS_HANDLE_T *ptHandle  �߳����ݾ��
* @param[out]��
* @return    ���˳��ȴ�ʱ��s
* @Create    : ����
* @Date      ��2019-10-23
* @Update    : 2020-11-13
**********************************************************************/
int wireless_grps_working(uint8 commport, uint8 ifnum, WIRELESS_HANDLE_T *ptHandle)
{
    uint32  i;
    uint32  status = AT_INIT; /*���Ӳ���*/
    uint32  stepcnt = 0;       /*״̬������*/
    uint32  othercount = 0;       /*������������*/
    uint32  delay = 0;
    uint16  buflen = 0;
    
    WIRELESS_WORK_FUN_T statusFuns[] = 
    {
        {2, AT_INIT,    wireless_AT_INIT,    "AT_INIT",   "�򿪴���", 0, 60},
        {1, AT_ST,      wireless_AT_ST,      "AT_ST",     {0}, 0, 60},
        {1, AT_AT,      wireless_AT_AT,      "AT_AT",     "���ͨ��ģ��", 0, 60},
        {1, AT_GMM,     wireless_AT_GMM,     "AT_GMM",    "��ȡͨ��ģ���ͺ�", 0, 60},
        {2, AT_CPIN,    wireless_AT_CPIN,    "AT_CPIN",   "���SIM��", 1, 60},
        {2, AT_CCID,    wireless_AT_CCID,    "AT_CCID",   "��ȡSIM����", 1, 60},
        {1, AT_CIMI,    wireless_AT_CIMI,    "AT_CIMI",   {0}, 0, 60},
        {2, AT_CSQ,     wireless_AT_CSQ,     "AT_CSQ",    "��ȡ�ź�", 0, 60},
        {2, AT_SYSINFO, wireless_AT_SYSINFO, "AT_SYSINFO","��ȡ��Ӫ��", 0, 60},
        {2, AT_CREG,    wireless_AT_CREG,    "AT_CREG",   "����ע��", 0, 60},
        {0, AT_CGDCONT, wireless_AT_CGDCONT, "AT_CGDCONT","����APN", 0, 60},
        {0, AT_CGATT,   wireless_AT_CGATT,   "AT_CGATT",  "���gprs����", 0, 60},
        {0, AT_GETINFO, wireless_AT_GETINFO, "AT_GETINFO",{0}, 0, 60},
        {0, AT_ATDT,    wireless_AT_ATDT,    "AT_ATDT",   "��ʼ����", 0, 60},
        {1, AT_PPP,     wireless_AT_PPP,     "AT_PPP",    "PPP��֤", 0, 60},
        {1, AT_LCP,     wireless_AT_LCP,     "AT_LCP",    "LCP��·Э��", 0, 60},
        {1, AT_IPCP,    wireless_AT_IPCP,    "AT_IPCP",   "IPCPЭ��", 0, 60},
        {1, AT_RESTART, wireless_AT_RESTART, "AT_RESTART","����ʧ��", 0, 60},

        {1, AT_WAIT,     wireless_AT_WAIT,      "AT_WAIT",     "�ȴ����ų�ʼ��", 0, 60},
        {0, PPP_SUCCESS, wireless_PPP_SUCCESS,  "PPP_SUCCESS", "���ųɹ�", 0, 60},
        {0, PPP_CHECK,   wireless_PPP_CHECK,  {0},             {0}, 0, 60},
    };

    uint32 statusNum = sizeof(statusFuns)/sizeof(WIRELESS_WORK_FUN_T);

    ptHandle->bsimOK = 1;
    ptHandle->stepStatus = status;

    /* ���PPP�˿� */
    wireless_ppp_off(ifnum);
    
    while(1)
    {   
        /* �쳣״̬��� */
        if(FALSE == ptHandle->workFlag || TRUE == ptHandle->workAgain)
        {
            wireless_ppp_off(ifnum);
            WIRELESS_FMT_DEBUG("���Ŷ˿�%d ״̬�쳣, ֹͣ����\n", AT_DEV_NUM(ifnum));
            if(ptHandle->pUart)
            {
                wireless_uart_uninit(ptHandle->pUart);
                ptHandle->pUart = NULL;
                WIRELESS_FMT_DEBUG("���Ŷ˿�%d ����ͷ�\n", AT_DEV_NUM(ifnum));
            }
            return 20;
        }

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
* @name      : wireless_grpsnew_working
* @brief     ������Э��ջgrps����, ��ʵ��
* @param[in] ��uint8 commport               �˿ں�
               uint8 ifnum                  ���ڱ��
               WIRELESS_HANDLE_T *ptHandle  �߳����ݾ��
* @param[out]��
* @return    ���˳��ȴ�ʱ��s
* @Create    : ����
* @Date      ��2019-10-26
* @Update    : 2020-4-21
**********************************************************************/
int wireless_grpsnew_working(uint8 commport, uint8 ifnum, WIRELESS_HANDLE_T *ptHandle)
{
    WIRELESS_FMT_DEBUG("���Ŷ˿�%d ��֧������Э��ջ, �л�������Э��ջ\n", AT_DEV_NUM(ifnum));

    return wireless_grps_working(commport, ifnum, ptHandle);
}


/**********************************************************************
* @name      : wireless_ppp0_init
* @brief     ������0��ʼ��
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int wireless_ppp0_init(void *pPublic, void **ppPrivate)
{
    WIRELESS_DATA_T      *ptWireData = (WIRELESS_DATA_T*)pPublic;
    ptWireData->pppdata0.no = 0;
    *ppPrivate = &ptWireData->pppdata0;
    return 0;
}

/**********************************************************************
* @name      : wireless_ppp1_init
* @brief     ������1��ʼ��
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int wireless_ppp1_init(void *pPublic, void **ppPrivate)
{
    WIRELESS_DATA_T      *ptWireData = (WIRELESS_DATA_T*)pPublic;
    ptWireData->pppdata1.no = 1;
    *ppPrivate = &ptWireData->pppdata1;
    return 0;
}

/**********************************************************************
* @name      : wireless_ppp_proc
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
int wireless_ppp_proc(void *pArg, void *pPublic, void *pPrivate)
{
    WIRELESS_HANDLE_T *ptHandle = (WIRELESS_HANDLE_T *)pPrivate;
    int                delay = 0;

    if(FALSE != ptHandle->workFlag && WIRELESS_UNUSED_COMMPORT != ptHandle->commport)
    {
        ptHandle->dialState = PPP_DAIL_STATUS_OFFLINE;
        ptHandle->workAgain = FALSE;
        ptHandle->stepStatus = 0;
        ptHandle->sendfaild = 0;

        sleep(1);  //�ȴ�1s ����ͬ��

        /* Э��ջѡ�� */
        if(0 != ptHandle->ptNetPara->userAPN.diagnum)
        {
            ptHandle->bUserApn = TRUE;
        }
        
        /* ����Э��ջ �Զ� */
        delay = wireless_grps_working(ptHandle->commport, ptHandle->no, ptHandle);

        /* �������� */
        ptHandle->stepStatus = 0;
        ptHandle->dialState = PPP_DAIL_STATUS_OFFLINE;
        wireless_set_pppstatus(ptHandle->no, 1);

        bzero(ptHandle->ptNetPara->ICCID, 21);/* SIM �� ICCID */
        ptHandle->ptNetPara->ipAddr = 0;      /* ����IP��ַ */
        ptHandle->ptNetPara->mask = 0;        /* �������� */   
        ptHandle->ptNetPara->getway = 0;      /* ���� */  
        ptHandle->ptNetPara->operat = WIRELESS_OPT_NONE;

        sleep(1 + delay);
    }
    
    return 1000000;
}
