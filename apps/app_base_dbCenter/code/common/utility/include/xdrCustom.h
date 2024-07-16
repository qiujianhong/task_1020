/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� xdrCustom.h
           ������ݵ�xdr����ת������
* @date�� 2019-11-9
*********************************************************************
*/

#ifndef __XDRCUSTOM_H__
#define __XDRCUSTOM_H__

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "baseConstant.h"
#include "baseType.h"
#include "oopType.h"

#ifdef __cplusplus
extern "C" {
#endif

#define XDR_TRUE (1)   // XDR TRUEֵ
#define XDR_FALSE (0)  // XDR FALSEֵ

#define XDR_OPTIONSL_TRUE (1)   // XDR OPTIONS TRUE
#define XDR_OPTIONSL_FALSE (0)  // XDR OPTIONS FALSE

#define XDR_DATA_TIME_LEN (7)
#define XDR_MAX_COM_NAME_LEN (128)
#define APPID_LEN               (24)
#pragma pack(1)
typedef struct data_time_S
{
    uint16 year;
    uint8  month;
    uint8  day;
    uint8  hour;
    uint8  minute;
    uint8  second;
} DATA_TIME_T;
#pragma pack()

typedef struct _app_sub_raw_info
{
    uint32 subscribeInfoNum;
    uint32 subscribeInfo[APP_MAX_SUB_NUM];
} APP_SUB_INFO_RAW_T;

typedef struct _app_sub_point_info
{
    uint32 subscribePointInfo[APP_MAX_SUB_POUINT_NUM];
} APP_SUB_POINT_INFO_T;

typedef struct mqt_plugin_
{
    uint8              comName[XDR_MAX_COM_NAME_LEN];
    APP_SUB_INFO_RAW_T subr;
    uint8              version[32];
    DATA_TIME_T        dataTime;
    uint32 appidlen;
    uint8 appid[APPID_LEN];
} MQT_PLUGIN_T;

//003A�豸��Ϣ
typedef struct 
{
    uint8 HardSeq1[100]; //Ӳ�����к�1
    uint8 HardSeq2[100]; //Ӳ�����к�2
    uint8 HardSeq3[100]; //Ӳ�����к�3
    uint8 HardSeq4[100]; //Ӳ�����к�4
    uint32 DeviceType;  //�豸����
    uint8 DeviceSupplier[100];//�豸��Ӧ��
    uint8 HardVersion[100];//Ӳ���汾��
    uint8 SoftVersion[100];//����汾��
    uint8 DeviceDate[100];//�豸��������
    uint8 DeviceDescriptor[100];//�豸������
    uint8 DeviceMac[100];//�豸MAC
    uint32 DeviceMem;//�豸�ڴ�����
    uint32 DeviceStorage;//�豸�洢����
    uint32 CPUFrequency;//��������Ƶ

}DEVICE_INFO_T;

typedef struct
{
    uint8 ContainerID;
    uint8 ContainerName[100];
    uint8 ContainerState[100];
    uint8 CpuCores;
    uint8 MemLimit[50];
    uint8 StorageLimit[50];
    uint8 FilePath[100];
    uint8 Interface[100];
    uint8 DownloadFilePath[100];
    uint16 CpuRate;
    uint16 MenRate;
    uint16 StorageRate;
    uint8 ContainerIP[4];
    uint8 ImageName[100];
    OOP_DATETIME_S_T CreateTime;
    OOP_DATETIME_S_T StartUpTime;
    uint32 RunTime;
}EVERY_CONTAINER_T;
//003C������Ϣ
typedef struct
{
    uint8 containercount;       //
    EVERY_CONTAINER_T container[50]; //
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
}APP_INFO_T;

//003D������APP��Ϣ
typedef struct
{
    uint8 appcount;
    APP_INFO_T appinfo[10];
}ALL_APP_INFO_T;

//�����Լ�APPӦ����Ϣ�ṹ��
typedef struct
{
    EVERY_CONTAINER_T container;
    ALL_APP_INFO_T apps;
}CONTAINER_AND_APP_T;

//�����Լ�APPӦ����Ϣ�ṹ��
typedef struct
{
    pthread_mutex_t lock;
    uint8 count;
    CONTAINER_AND_APP_T containers[20];
}ALL_CONTAINER_AND_APP_T;
/**
*********************************************************************
* @brief�� u8���;�xdr��������ת��Ϊ��ͨδ��������
* @param[in]��
               p       ���ݵ�ַ
* @param[out]��
               none
* @return�� u8����δ��������
*********************************************************************
*/
static inline uint8 pop_u8(uint8 **p)
{
    uint8 ret = (*p)[0];
    (*p)++;
    return ret;
}

/**
*********************************************************************
* @brief�� u16���;�xdr��������ת��Ϊ��ͨδ��������
* @param[in]��
               p       ���ݵ�ַ
* @param[out]��
               none
* @return�� u16����δ��������
*********************************************************************
*/
static inline uint16 pop_u16(uint8 **p)
{
    uint16 ret = ((*p)[0] << 8) + ((*p)[1]);
    (*p) += 2;
    return ret;
}

/**
*********************************************************************
* @brief�� u32���;�xdr��������ת��Ϊ��ͨδ��������
* @param[in]��
               p       ���ݵ�ַ
* @param[out]��
               none
* @return�� u32����δ��������
*********************************************************************
*/
static inline uint32 pop_u32(uint8 **p)
{
    uint32 ret = ((*p)[0] << 24) + ((*p)[1] << 16) + ((*p)[2] << 8) + ((*p)[3]);
    (*p) += 4;
    return ret;
}

/**
*********************************************************************
* @brief�� n�ֽھ�xdr��������ת��Ϊ��ͨδ��������
* @param[in]��
               p       ���ݵ�ַ
* @param[out]��
               buf     ת��������ݴ洢��ַ
* @return�� n �����ֽڸ���
*********************************************************************
*/
static inline uint32 pop_nbyte(uint8 **p, uint8 **buf)
{
    uint32 len = 0;
    if ((*p)[0] == 0x82)
    {
        len = ((*p)[1] << 8) + (*p)[2];
        (*p) += 3;
    }
    else if((*p)[0] == 0x81)
    {
        len = (*p)[1];
        (*p) += 2;
    }
    else
    {
        len = (*p)[0];
        (*p)++;
    }

    *buf = *p;
    *p += len;
    return len;
}

/**
*********************************************************************
* @brief�� fix number �ֽھ�xdr��������ת��Ϊ��ͨδ��������
* @param[in]��
               p       ���ݵ�ַ
* @param[out]��
               buf     ת��������ݴ洢��ַ
* @return�� n �����ֽڸ���
*********************************************************************
*/
static inline void pop_fixbyte(uint8 **p, uint8 **buf, uint32 n)
{
    *buf = *p;
    *p += n;
}

/**
*********************************************************************
* @brief�� nU16��xdr��������ת��Ϊ��ͨδ��������
* @param[in]��
               p       ���ݵ�ַ
* @param[out]��
               buf     ת��������ݴ洢��ַ
* @return�� n �����ֽڸ���
*********************************************************************
*/
static inline uint32 pop_nshort(uint8 **p, uint8 **buf)
{
    uint32 len = 0;
    if ((*p)[0] == 0x82)
    {
        len = ((*p)[1] << 8) + (*p)[2];
        (*p) += 3;
    }
    else
    {
        len = (*p)[0];
        (*p)++;
    }

    *buf = *p;
    *p += (len << 1);
    return len;
}

/**
*********************************************************************
* @brief�� nU32��xdr��������ת��Ϊ��ͨδ��������
* @param[in]��
               p       ���ݵ�ַ
* @param[out]��
               buf     ת��������ݴ洢��ַ
* @return�� n �����ֽڸ���
*********************************************************************
*/
static inline uint32 pop_nint(uint8 **p, uint8 **buf)
{
    uint32 len = 0;
    if ((*p)[0] == 0x82)
    {
        len = ((*p)[1] << 8) + (*p)[2];
        (*p) += 3;
    }
    else
    {
        len = (*p)[0];
        (*p)++;
    }

    *buf = *p;
    *p += (len << 2);
    return len;
}

/**
*********************************************************************
* @brief�� u8������ͨδ��������ת��Ϊxdr��������
* @param[in]��
               v     u8����ֵ
* @param[out]��
               p     ת������洢��ַ
* @return�� void
*********************************************************************
*/
static inline void push_u8(uint8 **p, uint8 v)
{
    (*p)[0] = v;
    (*p)++;
}

/**
*********************************************************************
* @brief�� u16������ͨδ��������ת��Ϊxdr��������
* @param[in]��
               v     u16����ֵ
* @param[out]��
               p     ת������洢��ַ
* @return�� void
*********************************************************************
*/
static inline void push_u16(uint8 **p, uint16 v)
{
    (*p)[0] = (v >> 8) & 0xff;
    (*p)[1] = v & 0xff;
    (*p) += 2;
}

/**
*********************************************************************
* @brief�� u32������ͨδ��������ת��Ϊxdr��������
* @param[in]��
               v     u32����ֵ
* @param[out]��
               p     ת������洢��ַ
* @return�� void
*********************************************************************
*/
static inline void push_u32(uint8 **p, uint32 v)
{
    (*p)[0] = (v >> 24) & 0xff;
    (*p)[1] = (v >> 16) & 0xff;
    (*p)[2] = (v >> 8) & 0xff;
    (*p)[3] = v & 0xff;
    (*p) += 4;
}

static inline void push_s32(uint8 **p, sint32 v)
{
    (*p)[0] = (v >> 24) & 0xff;
    (*p)[1] = (v >> 16) & 0xff;
    (*p)[2] = (v >> 8) & 0xff;
    (*p)[3] = v & 0xff;
    (*p) += 4;
}

/**
*********************************************************************
* @brief�� n�ֽ�������ͨδ��������ת��Ϊxdr��������
* @param[in]��
               buf     ������ʼ��ַ
               bufLen  �����ֽڸ���
* @param[out]��
               p     ת������洢��ַ
* @return�� void
*********************************************************************
*/
static inline void push_nbyte(uint8 **p, uint8 *buf, sint32 bufLen)
{
    if (bufLen < 0x80)
    {
        (*p)[0] = bufLen;
        (*p)++;
    }
    else
    {
        (*p)[0] = 0x82;
        (*p)[1] = ((bufLen >> 8) & 0xff);
        (*p)[2] = bufLen & 0xff;
        (*p) += 3;
    }

    memcpy(*p, buf, bufLen);
    *p += bufLen;
}

// /**
// *********************************************************************
// * @brief�� n�ֽ�������ͨδ��������ת��Ϊxdr��������
// * @param[in]��
//                buf     ������ʼ��ַ
//                bufLen  �����ֽڸ���
// * @param[out]��
//                p     ת������洢��ַ
// * @return�� void
// *********************************************************************
// */
// static inline void push_nint(uint8 **p, uint8 *buf, sint32 bufLen)
// {
//     if (bufLen < 0x80)
//     {
//         (*p)[0] = bufLen;
//         (*p)++;
//     }
//     else
//     {
//         (*p)[0] = 0x82;
//         (*p)[1] = ((bufLen >> 8) & 0xff);
//         (*p)[2] = bufLen & 0xff;
//         (*p) += 3;
//     }

//     memcpy(*p, buf, bufLen);
//     *p += bufLen;
// }

/**
*********************************************************************
* @brief�� fix �ֽ�������ͨδ��������ת��Ϊxdr��������
* @param[in]��
               buf     ������ʼ��ַ
               bufLen  �����ֽڸ���
* @param[out]��
               p     ת������洢��ַ
* @return�� void
*********************************************************************
*/
static inline void push_fixbyte(uint8 **p, uint8 *buf, sint32 bufLen)
{
    memcpy(*p, buf, bufLen);
    *p += bufLen;
}

static inline void push_data_time(uint8 **p, DATA_TIME_T *dateTime)
{
    push_u16(p, dateTime->year);
    push_u8(p, dateTime->month);
    push_u8(p, dateTime->day);
    push_u8(p, dateTime->hour);
    push_u8(p, dateTime->minute);
    push_u8(p, dateTime->second);
}

static inline void push_mqt_plugin(uint8 **p, MQT_PLUGIN_T *plugin)
{
    uint32 i = 0;
    push_nbyte(p, plugin->comName, strlen((const sint8 *)plugin->comName));
    // push_u32(p, plugin->subscribeInfo);
    if(plugin->subr.subscribeInfoNum < 0x80)
    {
        push_u8(p, plugin->subr.subscribeInfoNum);
    }
    else
    {
        (*p)[0] = 0x82;
        (*p)[1] = ((plugin->subr.subscribeInfoNum >> 8) & 0xff);
        (*p)[2] = plugin->subr.subscribeInfoNum & 0xff;
        (*p) += 3;
    }
    for (i = 0; i < plugin->subr.subscribeInfoNum; i++)
    {
        push_u32(p, plugin->subr.subscribeInfo[i]);
    }
	push_nbyte(p, plugin->version, strlen((const sint8 *)plugin->version));
    push_data_time(p, &plugin->dataTime);

    if (plugin->appidlen > APPID_LEN)
    {
        plugin->appidlen = APPID_LEN;
    }
    
    if (plugin->appidlen < 0x80)
    {
        push_u8(p, plugin->appidlen);
    }
    else
    {
        (*p)[0] = 0x82;
        (*p)[1] = ((plugin->appidlen >> 8) & 0xff);
        (*p)[2] = plugin->appidlen & 0xff;
        (*p) += 3;
    }
    push_fixbyte(p, plugin->appid, plugin->appidlen);
}

static inline void push_sub_info(uint8 **p, APP_SUB_INFO_RAW_T *subr)
{
    uint32 i = 0;
    if (subr->subscribeInfoNum < 0x80)
    {
        push_u8(p, subr->subscribeInfoNum);
    }
    else
    {
        (*p)[0] = 0x82;
        (*p)[1] = ((subr->subscribeInfoNum >> 8) & 0xff);
        (*p)[2] = subr->subscribeInfoNum & 0xff;
        (*p) += 3;
    }

    for (i = 0; i < subr->subscribeInfoNum; i++)
    {
        push_u32(p, subr->subscribeInfo[i]);
    }
}

static inline void pop_data_time(uint8 **p, DATA_TIME_T *dateTime)
{
    DATA_TIME_T *t_dataTime;
    pop_fixbyte(p, (uint8 **)&t_dataTime, XDR_DATA_TIME_LEN);
    *dateTime = *t_dataTime;
}

static inline void pop_mqt_plugin(uint8 **p, MQT_PLUGIN_T *plugin)
{
    uint32 *sub              = NULL;
    uint32  i                = 0;
    uint8 * componentName    = {0};
    uint32  componentNameLen = 0;
	uint8 * versionName    = {0};
    uint32  versionNameLen = 0;

    componentNameLen = pop_nbyte(p, &componentName);
    UNUSED(componentNameLen);
    strncpy((sint8 *)plugin->comName, (sint8 *)componentName,
            sizeof(plugin->comName));
    plugin->subr.subscribeInfoNum = pop_nint(p, (uint8 **)(&sub));

    plugin->subr.subscribeInfoNum =
        plugin->subr.subscribeInfoNum > APP_MAX_SUB_NUM ?
            APP_MAX_SUB_NUM :
            plugin->subr.subscribeInfoNum;

    if (plugin->subr.subscribeInfoNum > 0)
    {
        for (i = 0; i < plugin->subr.subscribeInfoNum; i++)
        {
            plugin->subr.subscribeInfo[i] = ntohl(sub[i]);  //�ֽ���ת��
        }
    }
	
	versionNameLen = pop_nbyte(p, &versionName);
    UNUSED(versionNameLen);
	strncpy((sint8 *)plugin->version, (sint8 *)versionName,  sizeof(plugin->version));

    pop_data_time(p, &plugin->dataTime);
}
/**
*********************************************************************
* @brief�� ��ż��ϳ���n�ֽ�������ͨδ��������ת��Ϊxdr��������
* @param[in]��
               *p      ������ʼ��ַ
               sLen    �����ֽڸ���
* @param[out]��
               p     ת������洢��ַ
* @return�� void
*********************************************************************
*/
static inline void push_sequnce_length(uint8** p, sint16 sLen)
{
    if (sLen < 0x80)
    {
        (*p)[0] = sLen;
        (*p)++;
    }
    else
    {
        (*p)[0] = 0x82;
        (*p)[1] = ((sLen >> 8) & 0xff);
        (*p)[2] = sLen & 0xff;
        (*p) += 3;
    }
    return;
}

/**
*********************************************************************
* @brief�� ȡ��ż��ϳ���n�ֽ�xdr��������Ϊ������ͨ����
* @param[in]��
               buf     ������ʼ��ַ
               bufLen  �����ֽڸ���
* @param[out]��
               p     ת������洢��ַ
* @return�� void
*********************************************************************
*/
static inline sint16 pop_sequnce_length(uint8** p)
{
    uint32 len = 0;
    if ((*p)[0] == 0x82)
    {
        len = ((*p)[1] << 8) + (*p)[2];
        (*p) += 3;
    }
	else if ((*p)[0] == 0x81)
    {
        len = (*p)[1];
        (*p) += 2;
    }
    else
    {
        len = (*p)[0];
        (*p)++;
    }
    return len;
}

static inline void push_device_info(uint8 **p, DEVICE_INFO_T *info)
{
    push_nbyte(p, info->HardSeq1, strlen((const sint8 *)info->HardSeq1));
    push_nbyte(p, info->HardSeq2, strlen((const sint8 *)info->HardSeq2));
    push_nbyte(p, info->HardSeq3, strlen((const sint8 *)info->HardSeq3));
    push_nbyte(p, info->HardSeq4, strlen((const sint8 *)info->HardSeq4));
    push_u32(p, info->DeviceType);
    push_nbyte(p, info->DeviceSupplier, strlen((const sint8 *)info->DeviceSupplier));
    push_nbyte(p, info->HardVersion, strlen((const sint8 *)info->HardVersion));
    push_nbyte(p, info->SoftVersion, strlen((const sint8 *)info->SoftVersion));
    push_nbyte(p, info->DeviceDate, strlen((const sint8 *)info->DeviceDate));
    push_nbyte(p, info->DeviceDescriptor, strlen((const sint8 *)info->DeviceDescriptor));
    push_nbyte(p, info->DeviceMac, strlen((const sint8 *)info->DeviceMac));
    push_u32(p, info->DeviceMem);
    push_u32(p, info->DeviceStorage);
    push_u32(p, info->CPUFrequency);
}
/**
*********************************************************************
* @brief�� ���ĸ��ֽڵ�ipת��Ϊxdr��������
* @param[in]��
               ip 4�ֽ�ip��ַ
* @param[out]��
               p     ת������洢��ַ
* @return�� void
*********************************************************************
*/
static inline void push_ip(uint8 **p, uint8* ip)
{
    (*p)[0] =4;
    (*p)[1] = ip[0];
    (*p)[2] = ip[1];
    (*p)[3] = ip[2];
    (*p)[4] = ip[3];
    (*p) += 5;
}
static inline void push_data_time_s(uint8 **p, OOP_DATETIME_S_T *dateTime)
{
    push_u16(p, dateTime->year);
    push_u8(p, dateTime->month);
    push_u8(p, dateTime->day);
    push_u8(p, dateTime->hour);
    push_u8(p, dateTime->minute);
    push_u8(p, dateTime->second);
}
static inline void push_container_info(uint8 **p, ALL_CONTAINER_INFO_T *info)
{
    uint8 i;
    push_u8(p,info->containercount);
    for(i=0;i<info->containercount;i++)
    {
        push_u8(p,info->container[i].ContainerID);
        push_nbyte(p, info->container[i].ContainerName, strlen((const sint8 *)info->container[i].ContainerName));
        push_nbyte(p, info->container[i].ContainerState, strlen((const sint8 *)info->container[i].ContainerState));
        push_u8(p,info->container[i].CpuCores);
        push_nbyte(p, info->container[i].MemLimit, strlen((const sint8 *)info->container[i].MemLimit));
        push_nbyte(p, info->container[i].StorageLimit, strlen((const sint8 *)info->container[i].StorageLimit));
        push_nbyte(p, info->container[i].FilePath, strlen((const sint8 *)info->container[i].FilePath));
        push_nbyte(p, info->container[i].Interface, strlen((const sint8 *)info->container[i].Interface));
        push_nbyte(p, info->container[i].DownloadFilePath, strlen((const sint8 *)info->container[i].DownloadFilePath));
        push_u16(p,info->container[i].CpuRate);
        push_u16(p,info->container[i].MenRate);
        push_u16(p,info->container[i].StorageRate);
        push_ip(p,info->container[i].ContainerIP);
        push_nbyte(p, info->container[i].ImageName, strlen((const sint8 *)info->container[i].ImageName));
        push_data_time_s(p,&info->container[i].CreateTime);
        push_data_time_s(p,&info->container[i].StartUpTime);
        push_u32(p,info->container[i].RunTime);
    }
}

static inline void push_app_info(uint8 **p, ALL_APP_INFO_T *info)
{
    uint8 i,j;
    push_u8(p,info->appcount);
    for(i=0;i<info->appcount;i++)
    {
        push_u8(p,info->appinfo[i].appindex);
        push_nbyte(p, info->appinfo[i].appname, strlen((const sint8 *)info->appinfo[i].appname));
        push_nbyte(p, info->appinfo[i].containername, strlen((const sint8 *)info->appinfo[i].containername));
        push_nbyte(p, info->appinfo[i].appversion, strlen((const sint8 *)info->appinfo[i].appversion));
        push_nbyte(p, info->appinfo[i].apphash, strlen((const sint8 *)info->appinfo[i].apphash));
        push_u8(p,info->appinfo[i].servicecount);
        for(j=0;j<info->appinfo[i].servicecount;j++)
        {
            push_u8(p,info->appinfo[i].serviceinfo[j].serviceindex);
            push_nbyte(p, info->appinfo[i].serviceinfo[j].servicename, strlen((const sint8 *)info->appinfo[i].serviceinfo[j].servicename));
            push_u8(p,info->appinfo[i].serviceinfo[j].serviceenable);
            push_u8(p,info->appinfo[i].serviceinfo[j].servicestate);
            push_u16(p,info->appinfo[i].serviceinfo[j].cpulimit);
            push_u16(p,info->appinfo[i].serviceinfo[j].cpurate);
            push_u16(p,info->appinfo[i].serviceinfo[j].memrate);
            push_data_time_s(p,&info->appinfo[i].serviceinfo[j].starttime);
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
