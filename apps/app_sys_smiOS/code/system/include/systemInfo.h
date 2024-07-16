/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ͷ�ļ�
* @date�� 2019-11-20
*********************************************************************
*/
#ifndef __SYSTEM_INFO_H__
#define __SYSTEM_INFO_H__

#include "baseType.h"
#include "systemInfodef.h"
#ifdef __cplusplus
extern "C" {
#endif

/* @brief:�ڴ�ṹ�嶨��  */
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

/* @brief:�洢�鱨�ṹ�嶨��  */
typedef struct
{
    sint32 total;
    sint32 used;
    sint32 free;
}DISKINFO_T;

/* @brief:�ڴ��鱨�ṹ�嶨��  */
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

/* @brief:OS�鱨�ṹ�嶨��  */
typedef struct
{
    sint8 type[10];
    sint8 version[30];
}OSINFO_T;

/* @brief:ϵͳ�鱨�ṹ�嶨��  */
typedef struct
{
    CPUINFO_T cpu;
    MEMINFO_T mem;
    DISKINFO_T disk;
    OSINFO_T   os;
}SYSTEMINFO_T;

/* @brief�� ȡ��OS�汾��OS���� */
sint8  sys_osversion_get(sint8 *version);

/* @brief�� ȡ��cpu��1���Ӹ��� */
sint32 sys_cpurate_get(void);

/* @brief��ȡ��ramʹ���� */
sint32 sys_ramrate_get(void);

/* @brief��ȡ�ô洢ʹ���� */
sint32 sys_storgaerate_get(void);

/* @brief�����falsh���� */
sint32 sys_flashfailure_detect(void);

/* @brief�����ʱ�Ӵ��� */
sint32 sys_clockfailure_get(void);

/* @brief�����ram���� */
sint32 sys_ramfailure_get(void);

/* @brief�����esam���� */
sint32 sys_esamfailure_get(void);

/* @brief��ִ��shell���� */
void   sys_run_cmd(sint8 *cmd);

/* @brief�� ȡ���ڴ��鱨*/
sint32 sys_meminfo_get(MEMINFO_T *);

/* @brief��ȡ�ô洢��Ϣ */
sint32 sys_storageinfo_get(DISKINFO_T *);

/* @brief��ȡ��cpu�鱨 */
sint32 sys_cpuinfo_get(CPUINFO_T *);

/* @brief��ȡ�����е�ϵͳ�鱨 */
sint32 sys_systeminfo_get(SYSTEMINFO_T *);

/* @brief����ʼ���ṹ�壬���������鱨 */
void   sys_systeminfo_start(void);

/* @brief�� �ͷŽṹ�� */
void   sys_systeminfo_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_INFO_H__ */