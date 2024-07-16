/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块PPP操作相关定义申明
* @date：    2019-10-24
* @history： 
*********************************************************************
*/
#ifndef __WUGUAN_MSG_H__
#define __WUGUAN_MSG_H__
        
#ifdef __cplusplus
        extern "C" {
#endif

#define CON_STOP_VALUE "STOP CONTAINER SUCCESS!!"
#define CON_UNINDTATALL_VALUE "UNINSTALL CONTAINER SUCCESS!!"
#define CON_INDTALL_VALUE "INSTALL SUCCESS"
#define CON_INDTALLBLANK_VALUE "INSTALL BLANK CONTAINER SUCCESS"
#define CON_CONFIG_VALUE "Container Config Success!"
#define CON_START_VALUE "START CONTAINER SUCCESS!!"
#define SYS_SETCPU_VALUE "set cpu threshold success !"
#define SYS_SETMEM_VALUE "set memory threshold success !"
#define SYS_SETSTO_VALUE "set storage threshold success !"
#define SYS_SETTEMP_VALUE "set temperature threshold success !"
#define LOG_DELETE_VALUE "Delete log success!"




#define APP_IN_DOCK_MAX 10
#define DOCKERMAXLEN 20


typedef struct
{
    uint8 len;
    uint8  buff[255];
}SEQ_STR;


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

typedef struct
{
    uint8 ContainerID[50];
    uint8 ContainerName[100];
    uint8 ContainerState[100];
    uint8 CpuCores;
    uint16 cpuLimit;
    uint16 memLimit;
    uint16 stroLimit;
    uint8 MemLimit[100];
    uint8 StorageLimit[50];
    uint8 FilePath[500];
    uint8 Interface[500]; // 硬件接口
    uint8 DownloadFilePath[100];     //应用存储路径
    uint8 PortConfig[100];
    uint16 CpuRate;
    uint16 MenRate;
    uint16 StorageRate;
    uint8 ContainerIP[4];
    uint8 ImageName[100];
    OOP_DATETIME_S_T CreateTime;
    OOP_DATETIME_S_T StartUpTime;
    uint32 RunTime;
}EVERY_CONTAINER_T;
typedef struct
{
    uint8 containercount;       //
    EVERY_CONTAINER_T container[DOCKERMAXLEN]; //
    
}ALL_CONTAINER_INFO_T;

typedef struct
{
    uint8 serviceindex;
    uint8 servicename[100];
    uint8 serviceenable;
    uint8 servicestate;
    uint16 cpulimit;
    uint16 cpurate;
    uint16 memrate;
    OOP_DATETIME_S_T starttime;
}APP_SERVICE_INFO_T;

typedef struct
{
    uint8 appindex;
    uint8 appname[100];
    uint8 containername[100];
    uint8 appversion[100];
    uint8 apphash[100];
    uint8 servicecount;
    APP_SERVICE_INFO_T serviceinfo[10];
    uint8 cpucore;
    uint16 cpuLimit;
    uint16 memLval;
    uint16 memLimit;
}IOT_APP_INFO_T;
//0017结构体
typedef struct
{
    uint8 appcount;
    IOT_APP_INFO_T appinfo[APP_IN_DOCK_MAX*DOCKERMAXLEN];
}ALL_APP_INFO_T;

typedef struct
{
    SEQ_STR docker;
    SEQ_STR app;
    app_config app_config_t;
}iot_mqtt_0016;

typedef struct
{
    uint16 cpulimit;
    uint16 memLimit;
    uint16 diskLimit;
    uint16 cputemp_up;
    uint16 cputemp_down;
    uint16 sys_monitortime;
}SYS_CONF;

typedef struct
{
    SEQ_STR docker;
    SEQ_STR app;
    app_info app_config_t;
}iot_mqtt_0014;

typedef struct
{
 uint8 num;
 SEQ_STR conname;
 SEQ_STR app[10];
}iot_appnum;


typedef struct
{
 uint8 num;
 iot_appnum con[10];
}iot_connum;

#ifdef __cplusplus
}
#endif

#endif //__PPP_OPER_H__

