/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控业务
* @date：    2020-6-30
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef __USB_MAP_H
#define __USB_MAP_H
    
#ifdef __cplusplus
        extern "C" {
#endif

pthread_mutex_t  g_IotSendLock;
pthread_mutex_t  g_MqttIndexlock;

#define MAX_MSG_LEN 8192

/* 串口服务APP公共数据 */
#define MSG_INIT           0               //初始化连接
#define MSG_RECV           1               //数据接收
#define MSG_SEND           2               //s数据发送
#define MSG_CLOSE          3               //关闭连接
#define MSG_LOGIN           4           //登录到系统管理器
#define MSG_LOGIN_WAIT      5       //登录等待回复 
#define IS_LITTLE_ENDIAN    1

typedef struct tagMsgInfo
{
    uint32 IP;        //目的端ip
    uint16 port;
    uint8  res[2];
    
    uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128];
    char   DestAddr[128];

    uint16 IID;
    uint16 IOP;

    uint32 MsgLen; 
    uint8  MsgData[0];
}MSG_INFO_T;

typedef struct
{
    uint8 len;
    uint8  buff[255];
}SEQ_STR;

typedef struct
{
    char               logPath[255];
    uint32             logSize;
    char               logMd5[100];
    uint16             md5len;
    OOP_DATETIME_S_T   logCreat;
    uint8              logType;
    char               conName[100];
    char               appName[100];
}log_info;

typedef struct 
{
    log_info log[20];
    uint8 lognum;
}log_info_T;
typedef struct
{
    char    signname[100];
    char    signpath[100];
    uint32  signsize;
    uint8    signmd5[255];
}signinfo;
typedef struct
{
    char    filename[100];
    char    filetype[100];
    char    filepath[100];
    uint32  filesize; //bytes
    uint8    filemd5[255];
    signinfo  signinfo_t;    
}file_info;
typedef struct
{
    uint8 cpucore;
    uint16 cpuLimit;    //%
    uint16 mem_limit;  //MByte
    uint16 memLimit;   //%
}app_config;

typedef struct
{
    char updatever[100];
    uint8 updatetype;
    file_info file_info_t;
}sys_update_info;

typedef struct
{
    char app_version[100];
    file_info file_info_t;
    app_config app_config_t;
}app_info;
typedef struct
{
    SEQ_STR docker;
    SEQ_STR app;
    app_config app_config_t;
}iot_mqtt_0016;

typedef struct
{
    SEQ_STR docker;
    SEQ_STR app;
    app_info app_config_t;
}iot_mqtt_0014;


typedef struct
{
    uint8 status;
    uint8 smiregtype;
}iot_mqtt_type;

typedef struct
{
    uint8 ImageNamelen;
    uint8 ImageName[100];
    uint8 ImageSavelen;
    uint8 ImageSave[100];
}container_image;
typedef struct
{
    uint8  cpu_core;
    uint16 cpu_limit;
    uint16 mem_limitval;
    uint16 mem_limit;
    uint16 disk_limitval;
    uint16 disk_limit;
    uint16 strogefilelen;
    uint8  stroge_file[255];
    uint16 dev_infolen;
    SEQ_STR dev_info[10];
    uint16 host_folder;
    SEQ_STR host_info[10];
    uint16 port_para;
    uint8 port_val[255];
}iot_docker_config;
typedef struct
{
    uint8 dockernamelen;
    uint8 dockername[255];
    iot_docker_config iot_docker_config_T;
}docker_config_T;

typedef struct 
{
   char constatus[100];
   uint8 constalen;
   uint16 cpu_usage;
   uint16 mem_usage;
   uint16 disk_usage;
   char IpAddr[100];
   uint8 IpLen;
   char conImage[100];
   uint8 ImageLen;
   OOP_DATETIME_S_T creat_time;
   OOP_DATETIME_S_T start_time;
   uint32 runTime;
}container_status;
typedef struct
{
    uint8 ContainerID; //容器id
    uint8 ConNameLen;
    uint8 ContainerName[100];
    iot_docker_config iot_dockerconfig;
    container_status  iot_constatus;    
}iot_container_info;

typedef struct
{
    char appnum[100];
    uint8 num;
    char appname[100];
    uint8 namelen;
    char applicen[100];
    uint8 licenlen;
}app_license;
typedef struct
{
    uint8 licennum;
    app_license licen[100];
}app_license_T;
#ifdef __cplusplus
}
#endif

#endif /* __USB_MAP_H */


