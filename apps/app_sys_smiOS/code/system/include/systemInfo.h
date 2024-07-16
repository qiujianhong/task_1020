/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 头文件
* @date： 2019-11-20
*********************************************************************
*/
#ifndef __SYSTEM_INFO_H__
#define __SYSTEM_INFO_H__

#include "baseType.h"
#include "systemInfodef.h"
#ifdef __cplusplus
extern "C" {
#endif

/* @brief:内存结构体定义  */
typedef struct
{
    sint8  vendor_id[12];
    sint8  cpu_family;
    sint8  model;
    sint8  model_name[80];
    sint8  stepping;
    sint8  cpu_MHz[50];
    sint32 cache_size;
    sint8  physical_id;
    sint8  cpu_cores;
    sint8  fpu[10];
    sint8  fpu_exception[10];
    sint32 cpuid_level;
    sint8  wp[10];
    sint8  cpurate_1min[5];
    sint8  cpurate_5min[5];
    sint8  cpurate_15min[5];
}CPUINFO_T;

/* @brief:存储情报结构体定义  */
typedef struct
{
    sint32 total;
    sint32 used;
    sint32 free;
}DISKINFO_T;

/* @brief:内存情报结构体定义  */
typedef struct
{
    sint64  MemTotal;
    sint64  MemFree;
    sint64  MemAvailable;
    sint64  Buffers;
    sint64  Cached;
    sint64  SwapCached;
    sint64  Active;
    sint64  Inactive;
    sint64  Active_anon;
    sint64  Inactive_anon;
    sint64  Active_file;
    sint64  Inactive_file;
    sint64  Unevictable;
    sint64  Mlocked;
    sint64  SwapTotal;
    sint64  SwapFree;
    sint64  Dirty;
    sint64  Writeback;
    sint64  AnonPages;
    sint64  Mapped;
    sint64  Shmem;
    sint64  KReclaimable;
    sint64  Slab;
    sint64  SReclaimable;
    sint64  SUnreclaim;
    sint64  KernelStack;
    sint64  PageTables;
    sint64  NFS_Unstable;
    sint64  Bounce;
    sint64  WritebackTmp;
    sint64  CommitLimit;
    sint64  Committed_AS;
    sint64  VmallocTotal;
    sint64  VmallocUsed;
    sint64  VmallocChunk;
    sint64  HardwareCorrupted;
    sint64  AnonHugePages;
    sint64  ShmemHugePages;
    sint64  ShmemPmdMapped;
    sint64  CmaTotal;
    sint64  CmaFree;
    sint64  HugePages_Total;
    sint64  HugePages_Free;
    sint64  HugePages_Rsvd;
    sint64  HugePages_Surp;
    sint64  Hugepagesize;
    sint64  DirectMap4k;
    sint64  DirectMap2M;
}MEMINFO_T;

/* @brief:OS情报结构体定义  */
typedef struct
{
    sint8 type[10];
    sint8 version[30];
}OSINFO_T;

/* @brief:系统情报结构体定义  */
typedef struct
{
    CPUINFO_T cpu;
    MEMINFO_T mem;
    DISKINFO_T disk;
    OSINFO_T   os;
}SYSTEMINFO_T;

/* @brief： 取得OS版本跟OS类型 */
sint8  sys_osversion_get(sint8 *version);

/* @brief： 取得cpu的1分钟负载 */
sint32 sys_cpurate_get(void);

/* @brief：取得ram使用率 */
sint32 sys_ramrate_get(void);

/* @brief：取得存储使用率 */
sint32 sys_storgaerate_get(void);

/* @brief：检测falsh坏块 */
sint32 sys_flashfailure_detect(void);

/* @brief：检测时钟错误 */
sint32 sys_clockfailure_get(void);

/* @brief：检测ram错误 */
sint32 sys_ramfailure_get(void);

/* @brief：检测esam错误 */
sint32 sys_esamfailure_get(void);

/* @brief：执行shell命令 */
void   sys_run_cmd(sint8 *cmd);

/* @brief： 取得内存情报*/
sint32 sys_meminfo_get(MEMINFO_T *);

/* @brief：取得存储信息 */
sint32 sys_storageinfo_get(DISKINFO_T *);

/* @brief：取得cpu情报 */
sint32 sys_cpuinfo_get(CPUINFO_T *);

/* @brief：取得所有的系统情报 */
sint32 sys_systeminfo_get(SYSTEMINFO_T *);

/* @brief：初始化结构体，更新所有情报 */
void   sys_systeminfo_start(void);

/* @brief： 释放结构体 */
void   sys_systeminfo_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_INFO_H__ */