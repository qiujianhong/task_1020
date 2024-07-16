/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 头文件
* @date： 2019-11-20
*********************************************************************
*/
#ifndef __SYSTEM_INFO_DEF_H__
#define __SYSTEM_INFO_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MEMINFO_MEMTOTAL "MemTotal"
#define MEMINFO_MEMFREE "MemFree"
#define MEMINFO_MEMAVAILABLE "MemAvailable"
#define MEMINFO_BUFFERS "Buffers"
#define MEMINFO_CACHED "Cached"
#define MEMINFO_SWAPCACHED "SwapCached"
#define MEMINFO_ACTIVE "Active"
#define MEMINFO_INACTIVE "Inactive"
#define MEMINFO_ACTIVE_ANON "Active(anon)"
#define MEMINFO_INACTIVE_ANON "Inactive(anon)"
#define MEMINFO_ACTIVE_FILE "Active(file)"
#define MEMINFO_INACTIVE_FILE "Inactive(file)"
#define MEMINFO_UNEVICTABLE "Unevictable"
#define MEMINFO_MLOCKED "Mlocked"
#define MEMINFO_SWAPTOTAL "SwapTotal"
#define MEMINFO_SWAPFREE "SwapFree"
#define MEMINFO_DIRTY "Dirty"
#define MEMINFO_WRITEBACK "Writeback"
#define MEMINFO_ANONPAGES "AnonPages"
#define MEMINFO_MAPPED "Mapped"
#define MEMINFO_SHMEM "Shmem"
#define MEMINFO_KRECLAIMABLE "KReclaimable"
#define MEMINFO_SLAB "Slab"
#define MEMINFO_SRECLAIMABLE "SReclaimable"
#define MEMINFO_SUNRECLAIM "SUnreclaim"
#define MEMINFO_KERNELSTACK "KernelStack"
#define MEMINFO_PAGETABLES "PageTables"
#define MEMINFO_NFS_UNSTABLE "NFS_Unstable"
#define MEMINFO_BOUNCE "Bounce"
#define MEMINFO_WRITEBACKTMP "WritebackTmp"
#define MEMINFO_COMMITLIMIT "CommitLimit"
#define MEMINFO_COMMITTED_AS "Committed_AS"
#define MEMINFO_VMALLOCTOTAL "VmallocTotal"
#define MEMINFO_VMALLOCUSED "VmallocUsed"
#define MEMINFO_VMALLOCCHUNK "VmallocChunk"
#define MEMINFO_HARDWARECORRUPTE "HardwareCorrupte"
#define MEMINFO_ANONHUGEPAGES "AnonHugePages"
#define MEMINFO_SHMEMHUGEPAGES "ShmemHugePages"
#define MEMINFO_SHMEMPMDMAPPED "ShmemPmdMapped"
#define MEMINFO_CMATOTAL "CmaTotal"
#define MEMINFO_CMAFREE "CmaFree"
#define MEMINFO_HUGEPAGES_TOTAL "HugePages_Total"
#define MEMINFO_HUGEPAGES_FREE "HugePages_Free"
#define MEMINFO_HUGEPAGES_RSVD "HugePages_Rsvd"
#define MEMINFO_HUGEPAGES_SURP "HugePages_Surp"
#define MEMINFO_HUGEPAGESIZE "Hugepagesize"
#define MEMINFO_DIRECTMAP4K "DirectMap4k"
#define MEMINFO_DIRECTMAP2M "DirectMap2M"

#define CPUINFO_VENDOR_ID "vendor_id"
#define CPUINFO_CPU_FAMILY "cpu family"
#define CPUINFO_MODEL "model"
#define CPUINFO_MODEL_NAME "model name"
#define CPUINFO_STEPPING "stepping"
#define CPUINFO_CPU_MHZ "cpu MHz"
#define CPUINFO_CACHE_SIZE "cache size"
#define CPUINFO_PHYSICAL_ID "physical id"
#define CPUINFO_CPU_CORES "cpu cores"
#define CPUINFO_FPU "fpu"
#define CPUINFO_FPU_EXCEPTION "fpu_exception"
#define CPUINFO_CPUID_LEVEL "cpuid level"
#define CPUINFO_WP "wp"

#define FILE_PATH_MEMEINFO "/proc/meminfo"
#define FILE_PATH_LOADAVG "/proc/loadavg"
#define FILE_PATH_VERSION "/proc/version"
#define FILE_PATH_CPUINFO  "/proc/cpuinfo"

#define BITMASK(X) ((sint64)(0x01)<<X)
#define SET_BIT_ON(val,position) (val | BITMASK(position))
#define SET_BIT_OFF(val,position) (val & ~BITMASK(position))
#define GET_BIT_VALUE(val,position) ((val & BITMASK(position))>>position)

#define GET_MSG_IID_FROM_TAG(tag) ((sint16)(tag >> 16))
#define GET_MSG_IOP_FROM_TAG(tag) ((sint16)(tag & 0x0000FFFF))
#define MSG_COMMON_IID_0000 (0x0000)
#define MSG_COMMON_IID_0001 (0x0001)

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_INFO_DEF_H__*/
