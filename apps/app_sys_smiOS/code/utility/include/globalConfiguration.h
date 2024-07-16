/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� �������ķ���ȫ������ͷ�ļ�
* @date�� 2019-11-9
*********************************************************************
*/

#ifndef __GLOBAL_CONFIGURATION_H__
#define __GLOBAL_CONFIGURATION_H__

#include "baseType.h"
#include "constant.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @brief:ͨѶ�������ò��� �ṹ���װ  */
typedef struct _server_config_obj
{
    sint8  mqttUserName[MAX_USER_NAME_LEN];
    sint8  mqttPassword[MAX_PASSWD_LEN];
    sint8  mqttBrokerIP[IP_LEN];  // mqtt �����IP
    uint16 mqttBrokerPort;        // mqtt ����Ķ˿�
    uint16 mqttSubQos;            // mqtt Broker ����qos
    sint8  tcpServerIP[IP_LEN];   // tcp �����IP
    uint16 tcpServerPort;         // tcp ����Ķ˿�
    sint8  udpServerIP[IP_LEN];   // udp �����IP
    uint16 udpServerPort;         // udp ����Ķ˿�
    sint8  debugSqlIP[IP_LEN];    // debug �����IP
    uint16 debugSqlPort;          // debug ����Ķ˿�
    uint32 maxClientAccept;       //���ͻ��˽�����
    uint32 handerThreadNum;       //����߳���
    uint32 handerTaskQueueSize;   //������������
    sint8 mqttServiceName[MAX_APP_NAME_LEN];
    sint8 tcpServiceName[MAX_APP_NAME_LEN];
    sint8 udpServiceName[MAX_APP_NAME_LEN];
    
    sint8 localShareDir[APP_MAX_SHARE_DIR_LEN];
    sint8 localAppInstallDir[APP_MAX_SHARE_DIR_LEN];
    sint8 localAppConfigDir[APP_MAX_SHARE_DIR_LEN];
    sint8 dockerShareDir[APP_MAX_SHARE_DIR_LEN];
    sint8 dockerstartListFile[DOCKER_START_LIST_FILE_NAME_LEN];
    sint8 startListFile[APP_START_LIST_FILE_NAME_LEN];
    sint8 signFilePath[APP_SIGN_FILE_PATH_LEN];
    sint8 registerListFile[APP_REGISTER_LIST_FILE_NAME_LEN];
	sint8 localIpInfoFile[LOCAL_IP_FILE_NAME_LEN];

    uint32 memratewarnlevel;
    uint32 cpuratewarnlevel;
    uint32 storagerateWarnlevel;
    uint32 wdfeedtimerval;
} SERVICE_CONFIG_OBJ_T;

typedef struct _platform_conf
{
    sint8  mqttBrokerIP[IP_LEN];  // ����ƽ̨ MQTT �����IP
    uint16 mqttBrokerPort;        // ����ƽ̨ MQTT ����Ķ˿�
    sint8  mqttUserID[MAX_USER_ID_LEN];
    sint8  mqttUserName[MAX_USER_NAME_LEN];
    sint8  mqttPassword[MAX_PASSWD_LEN];
    // TLS config

} PLATFORM_CONFIG_OBJ_T;

typedef struct _fcgi_conf
{
    sint8  ip[IP_LEN];  // FCGI ��IP
    uint16 port;        // FCGI �󶨶˿�
    // TLS config

} FCGI_CONFIG_OBJ_T;

/* @brief:ȫ�����ýṹ���װ ����UDP ���ݿ�����  */
typedef struct _global_config_obj
{
    SERVICE_CONFIG_OBJ_T  serviceCfg;   //��������
    PLATFORM_CONFIG_OBJ_T platformCfg;  //ƽ̨����
    FCGI_CONFIG_OBJ_T fcgiCfg;          //fcgi��������

} GLOBAL_CONFIG_OBJ_T;

/* @brief:����һ��ȫ�����ñ���  */
extern GLOBAL_CONFIG_OBJ_T g_configObj;

/* @brief:ȫ�����ò�������  */
sint32 global_config_init(const sint8 *path);

/* @brief:ȫ������debug��ӡ����  */
void global_config_dump(void);

#ifdef __cplusplus
}
#endif

#endif