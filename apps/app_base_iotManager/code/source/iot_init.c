/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��·��س�ʼ��
* @date��    2020-6-30
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"
#include "iniparser.h"
#include "cJSON.h"
#include "mqtt_client.h"
#include "iot_init.h"
#include "iot_mqtt.h"
#include "hal.h"
#include "devinfo.h"


extern int iot_mqtt_init(IOT_APPDATA_T *pData);
char Private_Dir[50]="/data/app/iotManager/data/";
MQTT_CLIENT_T   *i_MqttClient;
char      g_AppVer[APPMSG_NAME_LEN] = {0};
uint8      i_AppName[APPMSG_NAME_LEN] = {0};

extern int read_pdata_xram(const char *filename, char *buf, unsigned offset, unsigned len);
extern int write_pdata_xram(const char *filename, const char *buf, unsigned offset, unsigned len);

/**********************************************************************
* @name      : usb_datafile_create
* @brief     �����������ļ�
* @param[in] ��char *filename    �����ļ�����
* @param[out]��
* @return    ��0-�ɹ�/ʧ��
* @Create    : ����
* @Date      ��2021-7-28
* @Update    :
**********************************************************************/
int usb_datafile_create(char *filename)
{
    int     ret = 0;

    return ret;
}

/**********************************************************************
* @name      : usb_paramcfg_proc
* @brief     �����������ļ�����
* @param[in] ��char *curpath   ��ǰ�ļ���·��
               char *cfgpath   �����ļ���·��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-6
* @Update    :
**********************************************************************/
int usb_datafile_deal(char *curpath, char *cfgpath)
{
    return 0;
}

/**********************************************************************
* @name      : usb_datafile_init
* @brief     ��ģ�ͳ�ʼ��, ��Ч�����ļ�
* @param[in] ��char *datapath    �����ļ�·��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-7-28
* @Update    :
**********************************************************************/
void usb_datafile_init(char *curpath, char *cfgpath)
{
    char   datafile[256] = {0};
    int    ret = 0;    
    
    /* �ļ����ж� */
    if(0 != access(curpath, F_OK))
    {
        if(0 != mkdir(curpath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            INIT_FMT_DEBUG("%s����ʧ��\n", curpath);
            return;
        }
    }
    
    sprintf(datafile, "%s/%s", curpath, IOT_DATAFILE);
    
    /* �����ļ����� */
    if(0 != access(datafile, F_OK))
    {
        /* �����������ļ�, ����һ���µ� */
        ret = usb_datafile_create(datafile);
        if(0 != ret)
        {
            INIT_FMT_DEBUG("%s����ʧ��, %d\n", datafile, ret);
        }
    }

    ret = usb_datafile_deal(curpath, cfgpath);
    if(0 != ret)
    {
        INIT_FMT_DEBUG("%s����ʧ��, %d\n", datafile, ret);
    }

    return;
}

/**********************************************************************
* @name      : init_config
* @brief     ����ʼ�������ļ�
* @param[in] ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-2-27
* @Update    : 
**********************************************************************/
int usb_init_config(void)
{
    dictionary *ini = NULL;
    char  *stringvalue = NULL;
    char   curpath[256] = {0};
    char   filename[256] = {0};
    int i;

    //��ȡ��ǰĿ¼����·������ȥ��������
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("usbManager get err path!\n");
    }
    else
    {
        for (i = cnt; i >=0; --i)
        {
            if (curpath[i] == '/')
            {
                curpath[i+1] = '\0';
                break;
            }
        }
    }
    
    sprintf(filename, "%s%s", curpath, IOT_INITFILE);
    
    /* ��־ģ���ʼ�� */
    diag_init(filename, TRUE);
    
    printf("filename is %s!\n", filename);
    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        //��ȡ�����ļ�·��
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:CFGPATH", "/data/app/"IOT_APP_NAME"/config");

        //�����ļ���ȡ
        usb_datafile_init(curpath, stringvalue);
    }
    else
    {
        printf("Get no ini file %s!\n", filename);
    }

    return 0;
}

/**********************************************************************
* @name      : init_config
* @brief     ����ʼ�������ļ�
* @param[in] ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-2-27
* @Update    : 
**********************************************************************/
int iot_init_config(IOT_APPDATA_T *pData)
{
    dictionary *ini = NULL;

  //  char  *intername = NULL;
   // uint32 ipmask = 0xffffffff;
    uint32 ipaddr = 0;
    char  *stringvalue = NULL;
    int    intvalue = 0;

    char   curpath[256] = {0};
    char   filename[256] = {0};
    int i;

    //��ȡ��ǰĿ¼����·������ȥ��������
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("iot get err path!\n");
    }
    else
    {
        for (i = cnt; i >=0; --i)
        {
            if (curpath[i] == '/')
            {
                curpath[i+1] = '\0';
                break;
            }
        }
    }
    
    sprintf(filename, "%s%s", curpath, IOT_INITFILE);
    
    /* ��־ģ���ʼ�� */
    diag_init(filename, TRUE);
    
    printf("filename is %s!\n", filename);
    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* ip��ȡ */
        stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "172.17.0.1");
        ipaddr = inet_addr(stringvalue);
       pData->mqttip = ntohl(ipaddr);

        /* �˿ڻ�ȡ */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        pData->mqttport = (uint16)intvalue;

         /* ���ݸ������� */
        intvalue = iniparser_getint(ini, "IOTRUN_CONF:UPDATETIME", 60);
        pData->app_t.updateTime = (uint16)intvalue;

        /* ���ݼ������ */
        intvalue = iniparser_getint(ini, "IOTRUN_CONF:MONITORTIME", 60);
        pData->app_t.monitorTime = (uint16)intvalue;


         /* �ն�cpu��ֵ % -2 */
        intvalue = iniparser_getint(ini, "IOTRUN_CONF:DEVCPULIMIT", 8000);
        pData->app_t.devCpuLimit = (uint16)intvalue;

         /* �ն�mem��ֵ % -2 */
        intvalue = iniparser_getint(ini, "IOTRUN_CONF:DEVMEMLIMIT", 8000);
        pData->app_t.devMemLimit = (uint16)intvalue;
        /* �ն�disk��ֵ % -2 */
        intvalue = iniparser_getint(ini, "IOTRUN_CONF:DEVDISKLIMIT", 8000);
        pData->app_t.devDiskLimit = (uint16)intvalue;
        /*cpu�¶���ֵ*/
        intvalue = iniparser_getint(ini, "IOTRUN_CONF:DEVCPUTEMPUP", 90000);
        pData->app_t.devCpuTempup = (uint16)intvalue;
        intvalue = iniparser_getint(ini, "IOTRUN_CONF:DEVCPUTEMPDOWN", 50000);
        pData->app_t.devCpuTempdown = (uint16)intvalue;
    }
    else
    {
        printf("Get no ini file %s!\n", filename);

        pData->mqttip = 0x7f000001;
        pData->mqttport = 1883;
        pData->app_t.updateTime   = 60;
        pData->app_t.monitorTime  = 60;
        pData->app_t.devCpuLimit  = 8000;
        pData->app_t.devMemLimit  = 8000;
        pData->app_t.devDiskLimit = 8000;
        return 0;
    }

    return 0;
}


/**********************************************************************
* @name      : wireless_get_appid
* @brief     ����ȡAPPID
* @param[in] ��uint8 len     ���泤��
* @param[out]��uint8 *appid  APPID
* @return    ������
* @Create    : ����
* @Date      ��2021-3-22
* @Update    : 
**********************************************************************/
uint8 iot_get_appid(uint8 *appid, uint8 len)
{
    int ret = 0;
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        memset(appid, 0x00, len);
        INIT_FMT_DEBUG("devinfo hal_device_get error \n");
        return MIN(24, len);
    }
    else
    {
        ret = dev->devinfo_read_appid(dev, "wirelessDCM", (char*)appid, len);
        if(ERR_OK != ret)
        {
            INIT_FMT_DEBUG("devinfo devinfo_read_appid error ret %d\n", ret);
            memset(appid, 0x00, len);
        }
        else
        {
            INIT_BUF_DEBUG(appid, MIN(24, len), "APPID\n");
        }
        hal_device_release((HW_DEVICE *)dev);
        return MIN(24, len);
    }
}

int iot_mqtt_init(IOT_APPDATA_T *pData)
{
    int ret = 0;
    uint8 status = 0;
    char   mqttaddr[16] = {0};
    SUB_TOPIC_T subtopic[2];
    char  mqttstatus[2][16] = {"connectlost", "success"};
    uint32 ip = pData->mqttip;

    /* �½��ͻ��� */
    sprintf((char *)i_AppName, "%s", IOT_APP_NAME);
    sprintf((char *)g_AppVer, "%s", APP_VERSION);
    i_MqttClient = mqtt_client_create((char *)IOT_APP_NAME);
    if (NULL == i_MqttClient)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_create fail\n");
        
        return -1;
    }
    
    /* �������� */
    memset(&subtopic, 0, sizeof(subtopic));
    subtopic[0].qos = 0;
    memcpy(subtopic[0].topic, IOT_SUBTOPIC1, strlen(IOT_SUBTOPIC1));
    subtopic[1].qos = 0;
    memcpy(subtopic[1].topic, IOT_SUBTOPIC2, strlen(IOT_SUBTOPIC2));
    ret = mqtt_client_set_sub_topic(i_MqttClient, subtopic, 2);
    if(0 != ret)
    {
        INIT_FMT_DEBUG("mqtt_client_set_sub_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* ����Ĭ������ */
    ret = mqtt_client_set_default_send_topic(i_MqttClient, IOT_DEFAULT_TOPIC, 0);
    if(0 != ret)
    {
        INIT_FMT_DEBUG("mqtt_client_set_default_send_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* ���ӷ���� */
    sprintf(mqttaddr, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
        INIT_FMT_DEBUG("mqtt connect %s:%d\n", mqttaddr, pData->mqttport);
    ret = mqtt_client_connect(i_MqttClient, mqttaddr, pData->mqttport, NULL);
    if(0 != ret)
    {
        INIT_FMT_DEBUG("mqtt_client_connect fail, ret %d", ret);
        goto errorend;
    }

    /* �ȴ����ӳɹ� */
    while(1)
    {
        status = mqtt_client_get_status(i_MqttClient);
        INIT_FMT_DEBUG("Get mqtt connect status %s\n", mqttstatus[status]);
        if(MQTT_CONNECT_SUCC == status)
        {
            break;
        }
        sleep(1);
    }
    INIT_FMT_DEBUG("mqtt connect %s:%d success\n", mqttaddr, pData->mqttport);

    return 0;

errorend:
    mqtt_client_destroy(i_MqttClient);
    return ret;

    return ret;
}



/**********************************************************************
* @name      : iot_app_init
* @brief     �����������ʼ��
* @param[in] ��
* @param[out]��void **ppPublicData
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int iot_app_init(void **ppPublicData)
{

    int  ret = 0;
    char filePath[255] = {0}; 
    IOT_APPDATA_T *pData = NULL;
    /* ������ֻ������һ������ */
    if(!singleton_check(IOT_APP_NAME))
    {
        printf("%s is started!\n", IOT_APP_NAME);
        return -1;
    }

    /* ����hal */
    if(0 != hal_init())
    {
        printf("hal_init failed!\n");
        return -2;
    }
    
    /*mqttע��*/
    pData = malloc(sizeof(IOT_APPDATA_T));
    if(NULL == pData)
    {
        printf("malloc pbulic data failed!\n");
        return -3;
    }
    memset(pData, 0, sizeof(IOT_APPDATA_T));
    pData->appidlen = IOT_APPID_MAX;

    /* �����ļ� */
    ret = iot_init_config(pData);
    if(0 != ret)
    {
        printf("iot_init_config failed!\n");
        free(pData);
        return -4;
    }
    /*�����û��/data/app/iotManager/data/ �ļ���û�оʹ���*/
    if(access(Private_Dir,F_OK)!=0)
    {
        system("mkdir /data/app/iotManager/data/");
    }
    /*�����û��ϵͳapp����(���� �ֱ���ΪSYS_CONF DOC_CONF APP_CONF)������˽���ļ�������еĻ���ȡ��û�о��������ļ���*/
    SYS_CONF sysconf;
    memset(&sysconf,0,sizeof(sysconf));
    sprintf(filePath,"%s%s",Private_Dir,"SYSCONF");
    ret = read_pdata_xram(filePath,(char *)&sysconf,0,sizeof(SYS_CONF));
    if(ret == 0 )
    {
      pData->app_t.devCpuLimit      =   sysconf.cpulimit;
      pData->app_t.devMemLimit      =   sysconf.memLimit;
      pData->app_t.devDiskLimit     =   sysconf.diskLimit;
      pData->app_t.devCpuTempup     =   sysconf.cputemp_up;
      pData->app_t.devCpuTempdown   =   sysconf.cputemp_down;
      pData->app_t.monitorTime      =   sysconf.sys_monitortime;
    }
    INIT_FMT_DEBUG("Get sys monitor info devcpulimit %d devMemLimit %d devDiskLimit %d devCpuTempdown %d devCpuTempup %d monitorTime %d\n", pData->app_t.devCpuLimit,pData->app_t.devMemLimit,pData->app_t.devDiskLimit,pData->app_t.devCpuTempdown,pData->app_t.devCpuTempup,pData->app_t.monitorTime);
    ret = iot_mqtt_init(pData);
    if(0 != ret)
    {
        printf("iot_mqtt_init failed!\n");
        free(pData);
        return -5;
    }
//#endif

    pData->appidlen  = iot_get_appid(pData->appid,24);
    
#if 0
    /* �����ļ� */
    ret = usb_init_config();
    if(0 != ret)
    {
        printf("usb_init_config failed!\n");
        return -2;
    }
#endif
    *ppPublicData = pData;

    return 0;
}

/**********************************************************************
* @name      : usb_version_print
* @brief     ���汾��ӡ
* @param[in] ��uint32 buflen     ���泤��
* @param[out]��char *buf         �����־
* @return    ��ʵ�ʳ���
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int iot_version_print(char *buf, uint32 buflen)
{
    return snprintf(buf, buflen, "Version: %s, Time: %s", APP_VERSION, APP_PUBLISH_TIME);
}

