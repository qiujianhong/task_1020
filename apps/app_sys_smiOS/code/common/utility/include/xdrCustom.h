/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： xdrCustom.h
           完成数据的xdr编码转换功能
* @date： 2019-11-9
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

#define XDR_TRUE (1)   // XDR TRUE值
#define XDR_FALSE (0)  // XDR FALSE值

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
    char             version[200];
    DATA_TIME_T        dataTime;
    uint32 appidlen;
    uint8 appid[APPID_LEN];
} MQT_PLUGIN_T;

//003A设备信息
typedef struct 
{
    uint8 HardSeq1[100]; //硬件序列号1
    uint8 HardSeq2[100]; //硬件序列号2
    uint8 HardSeq3[100]; //硬件序列号3
    uint8 HardSeq4[100]; //硬件序列号4
    uint32 DeviceType;  //设备类型
    uint8 DeviceSupplier[100];//设备供应商
    uint8 HardVersion[100];//硬件版本号
    uint8 SoftVersion[100];//软件版本号
    uint8 DeviceDate[100];//设备生产日期
    uint8 DeviceDescriptor[100];//设备描述符
    uint8 DeviceMac[100];//设备MAC
    uint32 DeviceMem;//设备内存容量
    uint32 DeviceStorage;//设备存储容量
    uint32 CPUFrequency;//处理器主频
    uint8  CPUCores;    //CPU核数
    uint16 CPUCaches;   //cpu缓存
    uint8  CPUFrame[100];    //cpu架构
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
//003C容器信息
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

//003D容器内APP信息
typedef struct
{
    uint8 appcount;
    APP_INFO_T appinfo[10];
}ALL_APP_INFO_T;

//容器以及APP应用信息结构体
typedef struct
{
    EVERY_CONTAINER_T container;
    ALL_APP_INFO_T apps;
}CONTAINER_AND_APP_T;

//容器以及APP应用信息结构体
typedef struct
{
    pthread_mutex_t lock;
    uint8 count;
    CONTAINER_AND_APP_T containers[20];
}ALL_CONTAINER_AND_APP_T;

//003E设备当前运行信息
typedef struct
{
    uint32 virmem;  //虚拟内存
    uint16 memrate; //  内存使用率
    uint16 storagerate; //磁盘使用率
    uint16 cpurate;     //cpu使用率
    char osname[100];   //操作系统名称
    char osversion[100];//操作系统版本
    char oskernel[100]; //操作系统内核
    OOP_DATETIME_S_T starttime; //系统启动时间
}DEV_RUN_INTO_T;

//路由信息
typedef struct
{
    uint8 dest[4];
    uint8 mask[4];
    uint8 gateway[4];
}ROUTE_INFO_T;

typedef struct
{
    uint8 num;
    ROUTE_INFO_T infolist[20];
}ROUTE_INFO_LIST_T;
/**
*********************************************************************
* @brief： u8类型经xdr编码数据转换为普通未编码数据
* @param[in]：
               p       数据地址
* @param[out]：
               none
* @return： u8类型未编码数据
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
* @brief： u16类型经xdr编码数据转换为普通未编码数据
* @param[in]：
               p       数据地址
* @param[out]：
               none
* @return： u16类型未编码数据
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
* @brief： u32类型经xdr编码数据转换为普通未编码数据
* @param[in]：
               p       数据地址
* @param[out]：
               none
* @return： u32类型未编码数据
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
* @brief： n字节经xdr编码数据转换为普通未编码数据
* @param[in]：
               p       数据地址
* @param[out]：
               buf     转换结果数据存储地址
* @return： n 数据字节个数
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
* @brief： fix number 字节经xdr编码数据转换为普通未编码数据
* @param[in]：
               p       数据地址
* @param[out]：
               buf     转换结果数据存储地址
* @return： n 数据字节个数
*********************************************************************
*/
static inline void pop_fixbyte(uint8 **p, uint8 **buf, uint32 n)
{
    *buf = *p;
    *p += n;
}

/**
*********************************************************************
* @brief： nU16经xdr编码数据转换为普通未编码数据
* @param[in]：
               p       数据地址
* @param[out]：
               buf     转换结果数据存储地址
* @return： n 数据字节个数
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
* @brief： nU32经xdr编码数据转换为普通未编码数据
* @param[in]：
               p       数据地址
* @param[out]：
               buf     转换结果数据存储地址
* @return： n 数据字节个数
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
* @brief： u8类型普通未编码数据转换为xdr编码数据
* @param[in]：
               v     u8数据值
* @param[out]：
               p     转换结果存储地址
* @return： void
*********************************************************************
*/
static inline void push_u8(uint8 **p, uint8 v)
{
    (*p)[0] = v;
    (*p)++;
}

/**
*********************************************************************
* @brief： u16类型普通未编码数据转换为xdr编码数据
* @param[in]：
               v     u16数据值
* @param[out]：
               p     转换结果存储地址
* @return： void
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
* @brief： u32类型普通未编码数据转换为xdr编码数据
* @param[in]：
               v     u32数据值
* @param[out]：
               p     转换结果存储地址
* @return： void
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
* @brief： n字节类型普通未编码数据转换为xdr编码数据
* @param[in]：
               buf     数据起始地址
               bufLen  数据字节个数
* @param[out]：
               p     转换结果存储地址
* @return： void
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
// * @brief： n字节类型普通未编码数据转换为xdr编码数据
// * @param[in]：
//                buf     数据起始地址
//                bufLen  数据字节个数
// * @param[out]：
//                p     转换结果存储地址
// * @return： void
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
* @brief： fix 字节类型普通未编码数据转换为xdr编码数据
* @param[in]：
               buf     数据起始地址
               bufLen  数据字节个数
* @param[out]：
               p     转换结果存储地址
* @return： void
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
    //push_u32(p, plugin->version);
//#ifdef PRODUCT_ECU
//    int a,b,c,d;
//    sscanf(plugin->version,"%d.%d.%d.%d",&a,&b,&c,&d);
//    push_u8(p,(uint8)a);
//    push_u8(p,(uint8)b);
//    push_u8(p,(uint8)c);
//    push_u8(p,(uint8)d);
//#else
    push_nbyte(p,(uint8*)plugin->version,strlen(plugin->version));
//#endif
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

//static inline void pop_mqt_plugin(uint8 **p, MQT_PLUGIN_T *plugin)
//{
//    uint32 *sub              = NULL;
//    uint32  i                = 0;
//    uint8 * componentName    = {0};
//    uint32  componentNameLen = 0;
//
//    componentNameLen = pop_nbyte(p, &componentName);
//    UNUSED(componentNameLen);
//    strncpy((sint8 *)plugin->comName, (sint8 *)componentName,
//            sizeof(plugin->comName));
//    plugin->subr.subscribeInfoNum = pop_nint(p, (uint8 **)(&sub));
//
//    plugin->subr.subscribeInfoNum =
//        plugin->subr.subscribeInfoNum > APP_MAX_SUB_NUM ?
//            APP_MAX_SUB_NUM :
//            plugin->subr.subscribeInfoNum;
//
//    if (plugin->subr.subscribeInfoNum > 0)
//    {
//        for (i = 0; i < plugin->subr.subscribeInfoNum; i++)
//        {
//            plugin->subr.subscribeInfo[i] = ntohl(sub[i]);  //字节序转换
//        }
//    }
//
//    plugin->version = pop_u32(p);
//    pop_data_time(p, &plugin->dataTime);
//}
/**
*********************************************************************
* @brief： 存放集合长度n字节类型普通未编码数据转换为xdr编码数据
* @param[in]：
               *p      数据起始地址
               sLen    数据字节个数
* @param[out]：
               p     转换结果存储地址
* @return： void
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
* @brief： 取存放集合长度n字节xdr编码数据为类型普通数据
* @param[in]：
               buf     数据起始地址
               bufLen  数据字节个数
* @param[out]：
               p     转换结果存储地址
* @return： void
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
    push_u8(p,info->CPUCores);
    push_u16(p,info->CPUCaches);
    push_nbyte(p, info->CPUFrame, strlen((const sint8 *)info->CPUFrame));
}
/**
*********************************************************************
* @brief： 将四个字节的ip转换为xdr编码数据
* @param[in]：
               ip 4字节ip地址
* @param[out]：
               p     转换结果存储地址
* @return： void
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

static inline void push_dev_run_info(uint8 **p, DEV_RUN_INTO_T *info)
{
    push_u32(p,info->virmem);
    push_u16(p,info->memrate);
    push_u16(p,info->storagerate);
    push_u16(p,info->cpurate);
    push_nbyte(p,(uint8*)info->osname,strlen(info->osname));
    push_nbyte(p,(uint8*)info->osversion,strlen(info->osversion));
    push_nbyte(p,(uint8*)info->oskernel,strlen(info->oskernel));
    push_data_time_s(p,&info->starttime);
}

static inline void push_route_info(uint8 **p, ROUTE_INFO_LIST_T *info)
{
    int i;
    push_u8(p,info->num);
    for(i=0;i<info->num;i++)
    {
        push_ip(p,info->infolist[i].dest);
        push_ip(p,info->infolist[i].mask);
        push_ip(p,info->infolist[i].gateway);
    }
}


#ifdef __cplusplus
}
#endif

#endif
