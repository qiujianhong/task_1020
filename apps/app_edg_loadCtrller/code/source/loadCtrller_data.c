/*
*********************************************************************
* Copyright(C) 2024 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ���ɿ���APP���ݳ�ʼ��
* @date��    2024-06-05
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "hal.h"
#include "devinfo.h"
#include "iniparser.h"
#include "appmsg_split.h"
#include "framecomm.h"
#include "storage.h"

#include "loadCtrller_data.h"
#include "loadCtrller_mqtt.h"

#define LCTRL_INI_FILE      "loadCtrller.ini"
#define CTRL_FILE_NAME      "ctrl_status"

LCTRL_DATA_T g_lcData;


uint32 lctrl_read_oad_notify(LCTRL_DBCHG_T *pDbChg, uint16 *infonum)
{
    uint32 headOad = 0;
    if(NULL == infonum)
    {
        return headOad;
    }

    PTHREAD_LOCK_SAFE(&(pDbChg->flock));  /*��������*/
    /*�������������ݣ���ȡ��buf�У��⿪��*/
    if(0 < pDbChg->num)
    {
        headOad = pDbChg->OAD[0];
        memmove(&(pDbChg->OAD[0]), &(pDbChg->OAD[1]), (pDbChg->num - 1)*sizeof(uint32));
        pDbChg->OAD[pDbChg->num-1] = 0;

        *infonum = pDbChg->infonum[0];
        memmove(&(pDbChg->infonum[0]), &(pDbChg->infonum[1]), (pDbChg->num - 1)*sizeof(uint16));
        pDbChg->infonum[pDbChg->num-1] = 0;

        pDbChg->num--;
    }
    PTHREAD_UNLOCK_SAFE(&(pDbChg->flock));
    return headOad;
}

/* ������� */
int lctrl_write_oad_notify(LCTRL_DBCHG_T *pDbChg, uint32 Oad, uint16 infoNum)
{
    if(0 == Oad || LCTRL_DB_QUEUE_SIZE <= pDbChg->num)
    {
        return -1;
    }

    PTHREAD_LOCK_SAFE(&(pDbChg->flock));  /*��������*/

    pDbChg->OAD[pDbChg->num] = Oad;
    pDbChg->infonum[pDbChg->num] = infoNum;

    pDbChg->num++;
    
    PTHREAD_UNLOCK_SAFE(&(pDbChg->flock));
    return 0;
}

/**********************************************************************
* @name      : lctrl_set_dataflag
* @brief     ���������ĳ�ʼ��
* @param[out]��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2024-6-8
* @Update    :
**********************************************************************/
void lctrl_set_dataflag(LCTRL_DBCHG_T *pDbChg)
{
    PTHREAD_LOCK_SAFE(&(pDbChg->flock));
    pDbChg->bInitData = TRUE;
    PTHREAD_UNLOCK_SAFE(&(pDbChg->flock));
}

/**********************************************************************
* @name      : lctrl_get_dataflag
* @brief     ���������ĳ�ʼ��
* @param[out]��
* @param[out]��
* @return    ����ʼ�����
* @Create    : ����
* @Date      ��2020-5-11
* @Update    :
**********************************************************************/
uint8 lctrl_get_dataflag(LCTRL_DBCHG_T *pDbChg)
{
    uint8 flag = 0;
    PTHREAD_LOCK_SAFE(&(pDbChg->flock));
    flag = pDbChg->bInitData;
    pDbChg->bInitData = FALSE;
    PTHREAD_UNLOCK_SAFE(&(pDbChg->flock));
    return flag;
}

/**********************************************************************
* @name      : lctrl_set_argflag
* @brief     ���������ĳ�ʼ��
* @param[out]��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2024-6-8
* @Update    :
**********************************************************************/
void lctrl_set_argflag(LCTRL_DBCHG_T *pDbChg)
{
    PTHREAD_LOCK_SAFE(&(pDbChg->flock));
    pDbChg->bInitArg = TRUE;
    PTHREAD_UNLOCK_SAFE(&(pDbChg->flock));
}

/**********************************************************************
* @name      : lctrl_get_argflag
* @brief     ���������ĳ�ʼ��
* @param[out]��
* @param[out]��
* @return    ����ʼ�����
* @Create    : ����
* @Date      ��2024-6-8
* @Update    :
**********************************************************************/
uint8 lctrl_get_argflag(LCTRL_DBCHG_T *pDbChg)
{
    uint8 flag = 0;
    PTHREAD_LOCK_SAFE(&(pDbChg->flock));
    flag = pDbChg->bInitArg;
    pDbChg->bInitArg = FALSE;
    PTHREAD_UNLOCK_SAFE(&(pDbChg->flock));
    return flag;
}

/**********************************************************************
* @name      : lctrl_write_ctrl
* @brief     �������������
* @param[in] ��uint8 type      ���� (CTRL_REMOTE_TYPE, CTRL_POWER_TYPE, CTRL_ELEC_TYPE, CTRL_STATUS_TYPE)
               uint8 *data     ����
               uint32 datalen  ���ݳ���(�������4K)
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-6-27
* @Update    :
**********************************************************************/
int lctrl_write_ctrl(uint8 type, uint8 *data, uint32 datalen)
{
    /* ǰ4�ֽ�У�飬��4�ֽ��ǳ��ȣ���������� */
    char   filepath[256] = {0};
    uint32 csNO = 0;  //У��ֵ
    uint32 i = 0;
    int    ret = 0;

    if(NULL == data || datalen > 8192 || 0 == datalen)
    {
        return -2;
    }

    for(i = 0; i < datalen; i++)
    {
        csNO += data[i];
    }
    
    sprintf(filepath, "/data/app/loadCtrller/data/%s_%d", CTRL_FILE_NAME, type);

    ret = write_pdata_xram(filepath, (char *)&csNO, 0, sizeof(uint32));
    ret |= write_pdata_xram(filepath, (char *)&datalen, 4, sizeof(uint32));
    ret |= write_pdata_xram(filepath, (char *)data, 8, datalen);

    return ret;
}

/**********************************************************************
* @name      : lctrl_read_ctrl
* @brief     ����ȡ��������
* @param[in] ��uint8 type      ���� (CTRL_REMOTE_TYPE, CTRL_POWER_TYPE, CTRL_ELEC_TYPE, CTRL_STATUS_TYPE, CTRL_CLOSE_TYPE)
               uint32 buflen   ���ݻ��泤��
* @param[out]��uint8 *databuf  ����
               uint32 *reallen ���ݳ���
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-5-7
* @Update    :
**********************************************************************/
int lctrl_read_ctrl(uint8 type, uint8 *databuf, uint32 buflen, uint32 *readlen)
{
    char   filepath[256] = {0};
    uint8  data[8192] = {0};
    uint32 csNO = 0;      //У��ֵ
    uint32 csNOTemp = 0;  //У��ֵ(����)
    uint32 datalen = 0;
    uint32 i = 0;
    int    ret = 0;

    if(NULL == databuf || 0 == buflen || NULL == readlen)
    {
        return -2;
    }
    
    sprintf(filepath, "/data/app/loadCtrller/data/%s_%d", CTRL_FILE_NAME, type);
    ret = read_pdata_xram(filepath, (char *)&csNO, 0, sizeof(uint32));
    ret |= read_pdata_xram(filepath, (char *)&datalen, 4, sizeof(uint32));
    datalen = MIN(datalen, 8192);
    ret |= read_pdata_xram(filepath, (char *)data, 8, datalen);

    /* ����У��ֵ */
    for(i = 0; i < datalen; i++)
    {
        csNOTemp += data[i];
    }

    if(csNOTemp != csNO)
    {
        return -3;
    }

    memcpy(databuf, data, MIN(buflen, datalen));
    *readlen = datalen;
    return 0;
}

/**********************************************************************
* @name      : lctrl_check_master_connect
* @brief     : �����վ����״̬
* @param[in] : LCTRL_DATA_T *ptLaData   ȫ������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2024-7-16
* @Update    :
**********************************************************************/
uint8 lctrl_check_master_connect()
{
    //return g_lcData.hasMaster;
    return TRUE;
}

/**********************************************************************
* @name      : lctrl_data_init
* @brief     : ���ɿ���ȫ�����ݳ�ʼ��
* @param[in] : LCTRL_DATA_T *ptLaData   ȫ������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2024-6-27
* @Update    :
**********************************************************************/
void lctrl_data_init(LCTRL_DATA_T *ptLaData)
{
    /* ��ʼ�� */
    memset(ptLaData, 0, sizeof(LCTRL_DATA_T));

    pthread_mutex_init(&(ptLaData->ctrlDbChg.flock), NULL);
    pthread_mutex_init(&(ptLaData->statDbChg.flock), NULL);

}

/**********************************************************************
* @name      : lctrl_init_config
* @brief     ����ʼ�������ļ�
* @param[in] LCTRL_DATA_T *ptLaData   ȫ������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-6-27
* @Update    :
**********************************************************************/
int lctrl_init_config(LCTRL_DATA_T *ptLaData)
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

    sprintf(filename, "%s%s", curpath, LCTRL_INI_FILE);
    /* ��־ģ���ʼ�� */
    diag_init(filename, FALSE);
    
    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* ip��ȡ */
        stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "172.17.0.1");
        ipaddr = inet_addr(stringvalue);
        ptLaData->mqttip = ntohl(ipaddr);

        /* �˿ڻ�ȡ */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        ptLaData->mqttport = (uint16)intvalue;
        
        /* ��ȡdata·�� */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:DATAPATH", "/data/app/loadCtrller/data");
        if(0 != access(stringvalue, F_OK))
        {
            /* �����ȴ����ļ��� */
            if(0 != mkdir(stringvalue, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
            {
                printf("Create data filepath %s, failed\n", stringvalue);
            }
        }

        intvalue = iniparser_getint(ini, "CTRL_CONF:TURNMAX", 4);
        ptLaData->turnMax = (uint8)intvalue;
        intvalue = iniparser_getint(ini, "CTRL_CONF:MODETURN", 4);
        ptLaData->modeturn = (uint8)intvalue;

        iniparser_freedict(ini);
    }
    else
    {
        printf("loadCtrller get no %s!\n", LCTRL_INI_FILE);
        if(0 != access("/data/app/loadCtrller/data", F_OK))
        {
            if(0 != mkdir("/data/app/loadCtrller/data", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
            {
                printf("Create data filepath /data/app/loadCtrller/data, failed\n");
            }
        }
        ptLaData->turnMax = 4;
        ptLaData->modeturn = 4;
    }

    return 0;
}


/**********************************************************************
* @name      : loadctrl_app_init
* @brief     �����ɹ����ʼ��
* @param[in] ��
* @param[out]��void **ppPublicData
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-6-27
* @Update    :
**********************************************************************/
int loadctrl_app_init(void **ppPublicData)
{
    int           ret = 0;
    /* ������ֻ������һ������ */
    if(!singleton_check("loadCtrller"))
    {
        printf("%s is started!\n", "loadCtrller");
        return -1;
    }

    /* ����hal */
    if(0 != hal_init())
    {
        printf("hal_init failed!\n");
        return -2;
    }

    /* ���ݳ�ʼ�� */
    lctrl_data_init(&g_lcData);

    /* �����ļ� */
    ret = lctrl_init_config(&g_lcData);
    if(0 != ret)
    {
        printf("wireless_init_config failed!\n");
        return -3;
    }

    /* mqtt��ʼ�� */
    ret = lctrl_mqtt_init(g_lcData.mqttip, g_lcData.mqttport);
    if(0 != ret)
    {
        printf("wireless_mqtt_init failed!\n");
        return -4;
    }

    /* ��ȡAPPID */
    g_lcData.appidlen = 0;


    /* ������ʼ�� */
    remote_set_lock_init();

    
    *ppPublicData = &g_lcData;
    return 0;
}

/**********************************************************************
* @name      : loadctrl_version_print
* @brief     ���汾��ӡ
* @param[in] ��uint32 buflen    ���泤��
* @param[out]��char *buf         �����־
* @return    ��ʵ�ʳ���
* @Create    : ����
* @Date      ��2024-6-5
* @Update    :
**********************************************************************/
int loadctrl_version_print(char *buf, uint32 buflen)
{
    return snprintf(buf, buflen, "Version: %s, Time: %s", APP_VERSION, APP_PUBLISH_TIME);
}


