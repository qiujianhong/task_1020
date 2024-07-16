/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��·���ҵ��
* @date��    2020-6-30
* @history�� 
* @User deatil:

*********************************************************************
*/
#ifndef __IOT_UPDATE_H
#define __IOT_UPDATE_H
    
#ifdef __cplusplus
        extern "C" {
#endif
#define DOCKERMAXLEN 20
#define datalenMax 1024
#define MAX_QUEUE_CNT  100       //�����и���
#define APP_IN_DOCK_MAX 10       //ÿ�������������10��app

//�����Լ�APPӦ����Ϣ�ṹ��
//update�߳����нṹ��
typedef struct
{
    time_t update_time;
    uint8  type;
}IOT_UPDATE_RUN;

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
    uint8 Interface[500]; // Ӳ���ӿ�
    uint8 DownloadFilePath[100];     //Ӧ�ô洢·��
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
//0017�ṹ��
typedef struct
{
    uint8 appcount;
    IOT_APP_INFO_T appinfo[APP_IN_DOCK_MAX*DOCKERMAXLEN];
}ALL_APP_T;


//003D������APP��Ϣ
typedef struct
{
    uint8 appcount;
    IOT_APP_INFO_T appinfo[APP_IN_DOCK_MAX];
}ALL_APP_INFO_T;


typedef struct
{
    uint16 cpu_usage;
    uint16 mem_usage;
    uint16 disk_usage;
    uint16 cputemp;
}dev_run_info;

typedef struct
{
    uint8 containercount;       //
    EVERY_CONTAINER_T container[DOCKERMAXLEN]; //
    
}ALL_CONTAINER_INFO_T;
//�����Լ�APPӦ����Ϣ�ṹ��
typedef struct
{
    EVERY_CONTAINER_T container;
    ALL_APP_INFO_T apps;
}CONTAINER_AND_APP_T;

typedef struct
{
    pthread_mutex_t lock;
    uint8 count;
    dev_run_info dev_run;
    CONTAINER_AND_APP_T containers[DOCKERMAXLEN];
}ALL_CONTAINER_AND_APP_T;




#ifdef __cplusplus
}
#endif

#endif


