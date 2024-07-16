/*
*********************************************************************
* Copyright(C) 2021 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief��   ң������������
* @date��    2021-7-24
* @history��
*********************************************************************
*/
#include "hal.h"
#include "baseCommon.h"
#include "appmsg_split.h"
#include "iniparser.h"
#include "framecomm.h"
#include "storage.h"

#include "rsp_data.h"
#include "rsp_mqtt.h"

#define RSP_INI_FILE   "rspSample.ini"
#define RSP_USER_FILE  "userparam.dat"

RSP_DATA_T g_tRspData;
char       g_RspDataFile[256] = {0};

/**********************************************************************
* @name      : rsp_write_queue
* @brief     ��д�����
* @param[in] ��RSP_REPORT_LIST_T *pList     �ϱ�����
               RSP_REPORT_DATA_T *pNode     д������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2022-3-21
* @Update    :
**********************************************************************/
void rsp_write_queue(RSP_REPORT_LIST_T *pList, RSP_REPORT_DATA_T *pNode)
{
    memcpy(&pList->list[pList->tail], pNode, sizeof(RSP_REPORT_DATA_T));
    pList->tail = (pList->tail + 1) % RSP_REPORT_CACHE;
}

/**********************************************************************
* @name      : rsp_read_queue
* @brief     ����������
* @param[in] ��RSP_REPORT_LIST_T *pList    �ϱ�����
* @param[out]��RSP_REPORT_DATA_T *pNode     �ϱ�����
* @return    ��0-�ɹ�/ʧ��
* @Create    : ����
* @Date      ��2022-3-21
* @Update    :
**********************************************************************/
int rsp_read_queue(RSP_REPORT_LIST_T *pList, RSP_REPORT_DATA_T *pNode)
{
    if(pList->head == pList->tail)
    {
        return -1;
    }

    memcpy(pNode, &pList->list[pList->head], sizeof(RSP_REPORT_DATA_T));
    memset( &pList->list[pList->head], 0, sizeof(RSP_REPORT_DATA_T));
    pList->head = (pList->head + 1) % RSP_REPORT_CACHE;

    return 0;
}

/**********************************************************************
* @name      : rsp_write_msg_queue
* @brief     ��д�����
* @param[in] ��RSP_MSG_LIST_T *pList     �ϱ�����
               uint8 *pMsg               ��Ϣ
               uint16 msglen             ����
* @param[out]��
* @return    ��0-�ɹ�/ʧ��
* @Create    : ����
* @Date      ��2022-6-16
* @Update    :
**********************************************************************/
int rsp_write_msg_queue(RSP_MSG_LIST_T *pList, uint8 *pMsg, uint16 msglen)
{
    if(msglen > RSP_MSG_MAXLEN)
    {
        return -1;
    }
    pList->list[pList->tail].len = msglen;
    memcpy(pList->list[pList->tail].msg, pMsg, msglen);
    pList->tail = (pList->tail + 1) % RSP_MSG_CACHE;

    return 0;
}

/**********************************************************************
* @name      : rsp_read_msg_queue
* @brief     ����������
* @param[in] ��RSP_MSG_LIST_T *pList    �ϱ�����
               uint16 buflen            �����С
* @param[out]��uint8 *pMsg               ��Ϣ����
* @return    ��0-��Ϣ����
* @Create    : ����
* @Date      ��2022-6-16
* @Update    :
**********************************************************************/
int rsp_read_msg_queue(RSP_MSG_LIST_T *pList, uint8 *pMsg, uint16 buflen)
{
    int len = 0;
    if(pList->head == pList->tail)
    {
        return 0;
    }

    len = pList->list[pList->head].len;
    memcpy(pMsg, pList->list[pList->head].msg, MIN(buflen, len));

    pList->list[pList->head].len = 0;
    memset(pList->list[pList->head].msg, 0, RSP_MSG_MAXLEN);
    pList->head = (pList->head + 1) % RSP_MSG_CACHE;

    return 0;
}


/**********************************************************************
* @name      : rsp_check_userfile
* @brief     ������û������ļ�, ���û�оʹ���
               �ļ���С 4 + 1024 * 2; ǰ4���ֽ��д��У��
* @param[in] ��char *filename �ļ���
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-7-25
* @Update    :
**********************************************************************/
void rsp_check_userfile(char *filename)
{
    char check[4] = {0xea, 0xeb, 0xec, 0x02}; /* ���һλ����Ϣ�汾��, ÿ�޸�һ������1 */
    char buf[4] = {0};

    if(0 == access(filename, F_OK))
    {
        if(0 == read_pdata_xram(filename, buf, 0, 4))
        {
            RSP_FMT_TRACE("File version %02x %02x %02x %02x, check %02x %02x %02x %02x\n",
                   buf[0], buf[1], buf[2], buf[3], check[0], check[1], check[2], check[3]);
            if(0 == memcmp(buf, check, 4))
            {
                return;
            }
        }
        RSP_FMT_DEBUG("Create new userfile\n");
    }

    char writebuf[1024 * 2] = {0};

    write_pdata_xram(filename, check, 0, 4);
    write_pdata_xram(filename, writebuf, 4, 1024 * 2);
}

/**********************************************************************
* @name      : rsp_get_config
* @brief     ����ȡң���������� ���λ�� 4 + id * 1024
* @param[in] ��uint8 id       �豸���(1-3)
               uint8 num      ��������
* @param[out]��uint8 *config  ң����������
* @return    ��
* @Create    : ����
* @Date      ��2021-7-25
* @Update    :
**********************************************************************/
int rsp_get_config(uint8 id, uint8 *config, uint8 num)
{
    if(id > 2 || NULL == config)
    {
        return -1;
    }

    return read_pdata_xram(g_RspDataFile, (char *)config, 4 + id * 1024, num);
}

/**********************************************************************
* @name      : rsp_write_config
* @brief     ������ң���������� ���λ�� 4 + id * 1024
* @param[in] ��uint8 id       �豸���(0-2)
               uint8 *config  ң����������
               uint8 num      ��������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-7-25
* @Update    :
**********************************************************************/
int rsp_write_config(uint8 id, uint8 *config, uint8 num)
{
    if(id > 2 || NULL == config)
    {
        return -1;
    }

    return write_pdata_xram(g_RspDataFile, (char *)config, 4 + id * 1024, num);
}

/**********************************************************************
* @name      : rsp_get_shaketime
* @brief     ����ȡң�ŷ���ʱ�� ���λ�� 4 + id * 1024 + 16
* @param[in] ��uint8 id           �豸���(0-2)
* @param[out]��uint16 *shaketime  ����ʱ��
* @return    ��
* @Create    : ����
* @Date      ��2021-7-25
* @Update    :
**********************************************************************/
int rsp_get_shaketime(uint8 id, uint16 *shaketime)
{
    if(id > 2 || NULL == shaketime)
    {
        return -1;
    }

    return read_pdata_xram(g_RspDataFile, (char *)shaketime, 4 + id * 1024 + 16, sizeof(uint16));
}

/**********************************************************************
* @name      : rsp_write_shaketime
* @brief     ������ң�ŷ���ʱ�� ���λ�� 4 + id * 1024 + 16
* @param[in] ��uint8 id           �豸���(0-2)
               uint16 shaketime   ����ʱ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-7-25
* @Update    :
**********************************************************************/
int rsp_write_shaketime(uint8 id, uint16 shaketime)
{
    if(id > 2)
    {
        return -1;
    }

    return write_pdata_xram(g_RspDataFile, (char *)&shaketime, 4 + id * 1024 + 16, sizeof(uint16));
}

/**********************************************************************
* @name      : rsp_get_curtime
* @brief     ����ǰʱ��ת���ɱ�׼��ʽ
* @param[in] ��
* @param[out]��OOP_DATETIME_T *pcurTime ��׼ʱ��
* @return    ��
* @Create    : ����
* @Date      ��2022-3-20
* @Update    :
**********************************************************************/
void rsp_get_curtime(OOP_DATETIME_T *pCurTime)
{
    struct  tm systime;
    struct  timeval tv;
    struct  timezone tz;
    gettimeofday(&tv, &tz);

    FastSecondToDate(&tv, &tz, &systime);

    pCurTime->msec = tv.tv_usec/1000;
    pCurTime->second = (uint8)systime.tm_sec;
    pCurTime->minute = (uint8)systime.tm_min;
    pCurTime->hour = (uint8)systime.tm_hour;
    pCurTime->wday = (uint8)systime.tm_wday;
    pCurTime->mday = (uint8)systime.tm_mday;
    pCurTime->month = (uint8)systime.tm_mon + 1;         /*�·�0����1��*/
    pCurTime->year = (uint8)systime.tm_year + 1900;         /*���0����1900��*/
}

/**********************************************************************
* @name      : rsp_init_config
* @brief     ����ʼ�������ļ�
* @param[in] ��
* @param[out]��uint32 *ip    ����˵�ַ
               uint16 *port  ����˶˿�
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2022-3-20
* @Update    :
**********************************************************************/
int rsp_init_config(RSP_DATA_T *pRspData)
{
    dictionary *ini = NULL;
    uint32 ipaddr = 0;
    char  *stringvalue = NULL;
    int    intvalue = 0;
    char   curpath[256] = {0};
    char   filename[256] = {0};
    int cnt = readlink("/proc/self/exe", curpath, 256);

    if(cnt < 0 || cnt >= 256)
    {
        printf("wireless get err path!\n");
    }
    //��ȡ��ǰĿ¼����·������ȥ��������
    int i;
    for (i = cnt; i >=0; --i)
    {
        if (curpath[i] == '/')
        {
            curpath[i+1] = '\0';
            break;
        }
    }

    sprintf(filename, "%s%s", curpath, RSP_INI_FILE);
    /* ��־ģ���ʼ�� */
    diag_init(filename, TRUE);

    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* ip��ȡ */
        stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "172.17.0.1");
        ipaddr = inet_addr(stringvalue);
        pRspData->mqttip = ntohl(ipaddr);

        /* �˿ڻ�ȡ */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        pRspData->mqttport = (uint16)intvalue;

        /* ��ȡdata·�� */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:DATAPATH", "/data/app/rspSample/data");
        if(0 != access(stringvalue, F_OK))
        {
            /* �����ȴ����ļ��� */
            if(0 != mkdir(stringvalue, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
            {
                printf("Create data filepath %s, failed\n", stringvalue);
            }
        }
        sprintf(g_RspDataFile, "%s/%s", stringvalue, RSP_USER_FILE);

        /* ���� */
        stringvalue = iniparser_getstring(ini, "YM_CONF:TYPE", "IIC");
        pRspData->bIIC = 0 == strcmp(stringvalue, "IIC") ? TRUE : FALSE;

        stringvalue = iniparser_getstring(ini, "YM_CONF:PULSE_TYPE", "SYS");
        pRspData->pulseType = 0 == strcmp(stringvalue, "IO") ? RSP_PULSE_IO : RSP_PULSE_SYS;
        pRspData->tRspDev.pulseLocate = iniparser_getint(ini, "YM_CONF:PULSE_LOCATE", 0);

        intvalue = iniparser_getint(ini, "YM_CONF:YX_SUM", 8);
        pRspData->tRspDev.sum = (uint8)intvalue;
        if(pRspData->tRspDev.sum > 16)
        {
            pRspData->tRspDev.sum = 16;
        }

        intvalue = iniparser_getint(ini, "YM_CONF:PULSE_SUM", 8);
        pRspData->tRspDev.pulsesum = (uint8)intvalue;
        if(pRspData->tRspDev.pulsesum > 8)
        {
            pRspData->tRspDev.pulsesum = 8;
        }

        intvalue = iniparser_getint(ini, "YM_CONF:YM_SHARE", 0);
        pRspData->tRspDev.bShare = (uint8)intvalue;
        if(pRspData->tRspDev.bShare > 1)
        {
            pRspData->tRspDev.bShare = 1;
        }

        /* ģ�� */
        intvalue = iniparser_getint(ini, "MAP_CONF:SUPPORT", 1);
        pRspData->bMAP = (uint8)intvalue;
        if(pRspData->bMAP > 1)
        {
            pRspData->bMAP = 1;
        }

        intvalue = iniparser_getint(ini, "MAP_CONF:PROT_SUM", 4);
        if(intvalue > 4 || intvalue < 0)
        {
            intvalue = 4;
        }
        pRspData->tRspModel[0].sum = intvalue;
        pRspData->tRspModel[1].sum = intvalue;

        intvalue = iniparser_getint(ini, "YM_CONF:CTRL_CHECK", 0);
        pRspData->bGetCtrl = 1 == intvalue ? 1 : 0;

        intvalue = iniparser_getint(ini, "YM_CONF:SYS_YK_NUM", 0);
        pRspData->tRspDev.yksum = intvalue > 16 ? 0 : intvalue;
        pRspData->tRspDev.ykLocate = iniparser_getint(ini, "YM_CONF:SYS_YK_LOCATE", 0);

        iniparser_freedict(ini);
    }
    else
    {
        printf("rspSample get no %s!\n", RSP_INI_FILE);
        sprintf(g_RspDataFile, "%s/%s", "/data/app/rspSample/data", RSP_USER_FILE);
        pRspData->tRspDev.sum = 2;
    }

    rsp_check_userfile(g_RspDataFile);
    return 0;
}

/**********************************************************************
* @name      : rsp_app_init
* @brief     ��ң������APP��ʼ��
* @param[in] ��
* @param[out]��void **ppPublicData
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-7-24
* @Update    :
**********************************************************************/
int rsp_app_init(void **ppPublicData)
{
    int           ret = 0;
    uint8         i = 0;
    char          showtemp[256] = {0};
    uint16        offset = 0;

    /* ������ֻ������һ������ */
    if(!singleton_check("rspSample"))
    {
        printf("%s is started!\n", "rspSample");
        return -1;
    }

    /* ����hal */
    if(0 != hal_init())
    {
        printf("hal_init failed!\n");
        return -2;
    }

    /* ���ݳ�ʼ�� */
    memset(&g_tRspData, 0, sizeof(RSP_DATA_T));
    pthread_mutex_init(&g_tRspData.tRspDev.flock, NULL);
    pthread_mutex_init(&g_tRspData.tRspModel[0].flock, NULL);
    pthread_mutex_init(&g_tRspData.tRspModel[1].flock, NULL);
    pthread_mutex_init(&g_tRspData.flock, NULL);

    /* �����ļ� */
    ret = rsp_init_config(&g_tRspData);
    if(0 != ret)
    {
        printf("rsp_init_config failed!\n");
        return -3;
    }

    /* mqtt��ʼ�� */
    ret = rsp_mqtt_init(g_tRspData.mqttip, g_tRspData.mqttport);
    if(0 != ret)
    {
        printf("wireless_mqtt_init failed!\n");
        return -4;
    }

    RSP_FMT_DEBUG("ң������rspSample����, �汾��: %s, ʱ��: %s\n", APP_VERSION, APP_PUBLISH_TIME);

    /* ���������ȡ */
    rsp_get_config(0, g_tRspData.tRspDev.config, RSP_PORT_MAX);
    rsp_get_shaketime(0, &(g_tRspData.tRspDev.shakeTime));
    if(0 == g_tRspData.tRspDev.shakeTime)
    {
        g_tRspData.tRspDev.shakeTime = 100;
    }
#ifdef PRODUCT_ECU
#ifdef BOARD_3358
    g_tRspData.tRspDev.config[0] = 1;
    g_tRspData.tRspDev.config[1] = 1;
#endif
#ifdef BOARD_801
    g_tRspData.tRspDev.config[6] = 1;
    g_tRspData.tRspDev.config[7] = 1;
#endif
    g_tRspData.tRspDev.sum = 0;
#endif
#ifdef PRODUCT_ZCU_3
    for(i = 0; i < RSP_PORT_MAX; i++)
    {
        if((g_tRspData.tRspDev.pulseLocate >> i) & 0x01)
        {
            g_tRspData.tRspDev.config[i] = 1;
        }
    }
#endif

    /* ������ӡ��� */
    RSP_FMT_DEBUG("��������(ң������: %d, �������� %d, %s���ö˿�):\n  ����ʱ��: %ums, ����: %s %s %s %s %s %s %s %s\n",
                  g_tRspData.tRspDev.sum, g_tRspData.tRspDev.pulsesum, 0 == g_tRspData.tRspDev.bShare ? "��" : "",
                  g_tRspData.tRspDev.shakeTime,
                  0 == g_tRspData.tRspDev.config[0] ? "ң��" : "����",
                  0 == g_tRspData.tRspDev.config[1] ? "ң��" : "����",
                  0 == g_tRspData.tRspDev.config[2] ? "ң��" : "����",
                  0 == g_tRspData.tRspDev.config[3] ? "ң��" : "����",
                  0 == g_tRspData.tRspDev.config[4] ? "ң��" : "����",
                  0 == g_tRspData.tRspDev.config[5] ? "ң��" : "����",
                  0 == g_tRspData.tRspDev.config[6] ? "ң��" : "����",
                  0 == g_tRspData.tRspDev.config[7] ? "ң��" : "����");
    if(g_tRspData.bIIC)
    {
        RSP_FMT_DEBUG("����ɼ�IIC, ����ɼ���ʽ%s\n", 0 == g_tRspData.pulseType ? "IO" : "SYS");
    }
    else
    {
        RSP_FMT_DEBUG("����ɼ�UART%s\n", 0 == g_tRspData.bGetCtrl ? "" : ", �ɼ����ƻ�·����״̬���");
    }
    /* ģ�� */
    g_tRspData.tRspModel[0].no = 1;
    rsp_get_config(1, g_tRspData.tRspModel[0].config, RSP_PORT_MAX);
    rsp_get_shaketime(1, &(g_tRspData.tRspModel[0].shakeTime));
    if(0 == g_tRspData.tRspModel[0].shakeTime)
    {
        g_tRspData.tRspModel[0].shakeTime = 100;
    }
#ifdef PRODUCT_ECU
    g_tRspData.tRspModel[0].config[0] = 0;
    g_tRspData.tRspModel[0].config[1] = 0;
    g_tRspData.tRspModel[0].config[2] = 0;
    g_tRspData.tRspModel[0].config[3] = 0;
#endif

    g_tRspData.tRspModel[1].no = 2;
    rsp_get_config(2, g_tRspData.tRspModel[1].config, RSP_PORT_MAX);
    rsp_get_shaketime(2, &(g_tRspData.tRspModel[1].shakeTime));
    if(0 == g_tRspData.tRspModel[1].shakeTime)
    {
        g_tRspData.tRspModel[1].shakeTime = 100;
    }
#ifdef PRODUCT_ECU
    g_tRspData.tRspModel[1].config[0] = 0;
    g_tRspData.tRspModel[1].config[1] = 0;
    g_tRspData.tRspModel[1].config[2] = 0;
    g_tRspData.tRspModel[1].config[3] = 0;
#endif

    if(1 == g_tRspData.bMAP)
    {
        for(i = 0; i < g_tRspData.tRspModel[0].sum; i++)
        {
            offset += sprintf(showtemp + offset, " %s", 0 == g_tRspData.tRspModel[0].config[i] ? "ң��" : "����");
        }
        RSP_FMT_DEBUG("ģ��1����(�˿�����: %d):\n  ����ʱ��: %ums, ����:%s\n",
                      g_tRspData.tRspModel[0].sum, g_tRspData.tRspModel[0].shakeTime, showtemp);

        memset(showtemp, 0, 256);
        offset = 0;
        for(i = 0; i < g_tRspData.tRspModel[1].sum; i++)
        {
            offset += sprintf(showtemp + offset, " %s", 0 == g_tRspData.tRspModel[1].config[i] ? "ң��" : "����");
        }
        RSP_FMT_DEBUG("ģ��2����(�˿�����: %d):\n  ����ʱ��: %ums, ����:%s\n",
                      g_tRspData.tRspModel[1].sum, g_tRspData.tRspModel[1].shakeTime, showtemp);
    }

    *ppPublicData = &g_tRspData;
    return 0;
}

/**********************************************************************
* @name      : rsp_version_print
* @brief     ���汾��ӡ
* @param[in] ��uint32 buflen     ���泤��
* @param[out]��char *buf         �����־
* @return    ��ʵ�ʳ���
* @Create    : ����
* @Date      ��2021-10-28
* @Update    :
**********************************************************************/
int rsp_version_print(char *buf, uint32 buflen)
{
    return snprintf(buf, buflen, "Version: %s, Time: %s", APP_VERSION, APP_PUBLISH_TIME);
}

