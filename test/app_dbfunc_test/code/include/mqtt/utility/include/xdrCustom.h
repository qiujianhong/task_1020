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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "baseType.h"

#define XDR_TRUE (1)   // XDR TRUE值
#define XDR_FALSE (0)  // XDR FALSE值

#define XDR_OPTIONSL_TRUE (1)   // XDR OPTIONS TRUE
#define XDR_OPTIONSL_FALSE (0)  // XDR OPTIONS FALSE

#define XDR_DATA_TIME_LEN (7)
#define XDR_MAX_COM_NAME_LEN (128)

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

typedef struct mqt_plugin_
{
    uint8       comName[XDR_MAX_COM_NAME_LEN];
    uint32      subscribeInfo;
    uint32      version;
    DATA_TIME_T dataTime;
} MQT_PLUGIN_T;

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
static inline uint8 pop_u8(uint8** p)
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
static inline uint16 pop_u16(uint8** p)
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
static inline uint32 pop_u32(uint8** p)
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
static inline uint32 pop_nbyte(uint8** p, uint8** buf)
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
static inline void pop_fixbyte(uint8** p, uint8** buf, uint32 n)
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
static inline uint32 pop_nshort(uint8** p, uint8** buf)
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
static inline uint32 pop_nint(uint8** p, uint8** buf)
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
static inline void push_u8(uint8** p, uint8 v)
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
static inline void push_u16(uint8** p, uint16 v)
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
static inline void push_u32(uint8** p, uint32 v)
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
static inline void push_nbyte(uint8** p, uint8* buf, sint32 bufLen)
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
static inline void push_fixbyte(uint8** p, uint8* buf, sint32 bufLen)
{
    memcpy(*p, buf, bufLen);
    *p += bufLen;
}

static inline void push_data_time(uint8** p, DATA_TIME_T* dateTime)
{
    push_u16(p, dateTime->year);
    push_u8(p, dateTime->month);
    push_u8(p, dateTime->day);
    push_u8(p, dateTime->hour);
    push_u8(p, dateTime->minute);
    push_u8(p, dateTime->second);
}

static inline void push_mqt_plugin(uint8** p, MQT_PLUGIN_T* plugin)
{
    push_nbyte(p, plugin->comName, strlen((const sint8*)plugin->comName));
    push_u32(p, plugin->subscribeInfo);
    push_u32(p, plugin->version);
    push_data_time(p, &plugin->dataTime);
}

static inline void pop_data_time(uint8** p, DATA_TIME_T* dateTime)
{
    DATA_TIME_T* t_dataTime;
    pop_fixbyte(p, (uint8**)&t_dataTime, XDR_DATA_TIME_LEN);
    *dateTime = *t_dataTime;
}

static inline void pop_mqt_plugin(uint8** p, MQT_PLUGIN_T* plugin)
{
    uint8* componentName    = {0};
    uint32 componentNameLen = 0;

    componentNameLen = pop_nbyte(p, &componentName);
    UNUSED(componentNameLen);
    strncpy((sint8 *)plugin->comName, (sint8 *)componentName, sizeof(plugin->comName));
    plugin->subscribeInfo = pop_u32(p);
    plugin->version       = pop_u32(p);
    pop_data_time(p,&plugin->dataTime);
}

#endif