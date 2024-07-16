/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 系统情报管理
* @date： 2019-11-20
*********************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sysinfo.h>

#include "baseType.h"
#include "framecomm.h"
#include "systemInfodef.h"
#include "systemInfo.h"
#include "xdrCustom.h"
/*cpu情报的name数组*/
const uint8 cpu_infoname[][20] = {
    CPUINFO_VENDOR_ID, CPUINFO_CPU_FAMILY, CPUINFO_MODEL,
    CPUINFO_MODEL_NAME, CPUINFO_STEPPING, CPUINFO_CPU_MHZ,
    CPUINFO_CACHE_SIZE, CPUINFO_PHYSICAL_ID, CPUINFO_CPU_CORES,
    CPUINFO_FPU, CPUINFO_FPU_EXCEPTION, CPUINFO_CPUID_LEVEL,
    CPUINFO_WP};
/* cpu情报的name数组的长度 */
#define CPU_INFONAME_SIZE (sizeof(cpu_infoname) / sizeof(cpu_infoname[0]))

/* 内存情报的name数组 */
const uint8 mem_infoname[][20] = {MEMINFO_MEMTOTAL,
                                  MEMINFO_MEMFREE,
                                  MEMINFO_MEMAVAILABLE,
                                  MEMINFO_BUFFERS,
                                  MEMINFO_CACHED,
                                  MEMINFO_SWAPCACHED,
                                  MEMINFO_ACTIVE,
                                  MEMINFO_INACTIVE,
                                  MEMINFO_ACTIVE_ANON,
                                  MEMINFO_INACTIVE_ANON,
                                  MEMINFO_ACTIVE_FILE,
                                  MEMINFO_INACTIVE_FILE,
                                  MEMINFO_UNEVICTABLE,
                                  MEMINFO_MLOCKED,
                                  MEMINFO_SWAPTOTAL,
                                  MEMINFO_SWAPFREE,
                                  MEMINFO_DIRTY,
                                  MEMINFO_WRITEBACK,
                                  MEMINFO_ANONPAGES,
                                  MEMINFO_MAPPED,
                                  MEMINFO_SHMEM,
                                  MEMINFO_KRECLAIMABLE,
                                  MEMINFO_SLAB,
                                  MEMINFO_SRECLAIMABLE,
                                  MEMINFO_SUNRECLAIM,
                                  MEMINFO_KERNELSTACK,
                                  MEMINFO_PAGETABLES,
                                  MEMINFO_NFS_UNSTABLE,
                                  MEMINFO_BOUNCE,
                                  MEMINFO_WRITEBACKTMP,
                                  MEMINFO_COMMITLIMIT,
                                  MEMINFO_COMMITTED_AS,
                                  MEMINFO_VMALLOCTOTAL,
                                  MEMINFO_VMALLOCUSED,
                                  MEMINFO_VMALLOCCHUNK,
                                  MEMINFO_HARDWARECORRUPTE,
                                  MEMINFO_ANONHUGEPAGES,
                                  MEMINFO_SHMEMHUGEPAGES,
                                  MEMINFO_SHMEMPMDMAPPED,
                                  MEMINFO_CMATOTAL,
                                  MEMINFO_CMAFREE,
                                  MEMINFO_HUGEPAGES_TOTAL,
                                  MEMINFO_HUGEPAGES_FREE,
                                  MEMINFO_HUGEPAGES_RSVD,
                                  MEMINFO_HUGEPAGES_SURP,
                                  MEMINFO_HUGEPAGESIZE,
                                  MEMINFO_DIRECTMAP4K,
                                  MEMINFO_DIRECTMAP2M};
/* 内存情报的name数组长度 */
#define MEM_INFONAME_SIZE (sizeof(mem_infoname) / sizeof(mem_infoname[0]))

/*系统情报结构体指针 */
static SYSTEMINFO_T *s_psysinfo = NULL;

extern int cmd_execute(const char* cmd,  char* result);
extern char * strim(char *str);
/**
*********************************************************************
* @brief： 更新内存情报信息
* @param[in]： 
               none
* @param[out]：
               none
* @return： 返回值说明 
*********************************************************************
*/
static void sys_meminfo_update(void)
{
    FILE *fp = NULL;
    sint8 buffer[50] = {0};
    sint8 *pbuff = NULL;
    sint8 meminfo_result[MEM_INFONAME_SIZE][20] = {};
    uint8 i = 0;
    sint8 len = 0;
    do
    {
        fp = fopen(FILE_PATH_MEMEINFO, "rb");
        if (NULL == fp)
        {
            break;
        }
        UNUSED(len);
        i = 0;
        while ((fgets(buffer, 50, fp)) != NULL)
        {
            if (i < MEM_INFONAME_SIZE)
            {
                if ((pbuff = strstr(buffer, (const char *)mem_infoname[i])) != NULL)
                {
                    pbuff = strtok(buffer, ":");
                    pbuff = strtok(NULL, ":");
                    pbuff[strlen(pbuff) - 1] = 0;
                    sscanf(pbuff, "%20s", meminfo_result[i]);
                }
                i++;
            }
        }
        fclose(fp);
        i = 0;
        s_psysinfo->mem.MemTotal = atol(meminfo_result[i++]);
        s_psysinfo->mem.MemFree = atol(meminfo_result[i++]);
        s_psysinfo->mem.MemAvailable = atol(meminfo_result[i++]);
        s_psysinfo->mem.Buffers = atol(meminfo_result[i++]);
        s_psysinfo->mem.Cached = atol(meminfo_result[i++]);
        s_psysinfo->mem.SwapCached = atol(meminfo_result[i++]);
        s_psysinfo->mem.Active = atol(meminfo_result[i++]);
        s_psysinfo->mem.Inactive = atol(meminfo_result[i++]);
        s_psysinfo->mem.Active_anon = atol(meminfo_result[i++]);
        s_psysinfo->mem.Inactive_anon = atol(meminfo_result[i++]);
        s_psysinfo->mem.Active_file = atol(meminfo_result[i++]);
        s_psysinfo->mem.Inactive_file = atol(meminfo_result[i++]);
        s_psysinfo->mem.Unevictable = atol(meminfo_result[i++]);
        s_psysinfo->mem.Mlocked = atol(meminfo_result[i++]);
        s_psysinfo->mem.SwapTotal = atol(meminfo_result[i++]);
        s_psysinfo->mem.SwapFree = atol(meminfo_result[i++]);
        s_psysinfo->mem.Dirty = atol(meminfo_result[i++]);
        s_psysinfo->mem.Writeback = atol(meminfo_result[i++]);
        s_psysinfo->mem.AnonPages = atol(meminfo_result[i++]);
        s_psysinfo->mem.Mapped = atol(meminfo_result[i++]);
        s_psysinfo->mem.Shmem = atol(meminfo_result[i++]);
        s_psysinfo->mem.KReclaimable = atol(meminfo_result[i++]);
        s_psysinfo->mem.Slab = atol(meminfo_result[i++]);
        s_psysinfo->mem.SReclaimable = atol(meminfo_result[i++]);
        s_psysinfo->mem.SUnreclaim = atol(meminfo_result[i++]);
        s_psysinfo->mem.KernelStack = atol(meminfo_result[i++]);
        s_psysinfo->mem.PageTables = atol(meminfo_result[i++]);
        s_psysinfo->mem.NFS_Unstable = atol(meminfo_result[i++]);
        s_psysinfo->mem.Bounce = atol(meminfo_result[i++]);
        s_psysinfo->mem.WritebackTmp = atol(meminfo_result[i++]);
        s_psysinfo->mem.CommitLimit = atol(meminfo_result[i++]);
        s_psysinfo->mem.Committed_AS = atol(meminfo_result[i++]);
        s_psysinfo->mem.VmallocTotal = atol(meminfo_result[i++]);
        s_psysinfo->mem.VmallocUsed = atol(meminfo_result[i++]);
        s_psysinfo->mem.VmallocChunk = atol(meminfo_result[i++]);
        s_psysinfo->mem.HardwareCorrupted = atol(meminfo_result[i++]);
        s_psysinfo->mem.AnonHugePages = atol(meminfo_result[i++]);
        s_psysinfo->mem.ShmemHugePages = atol(meminfo_result[i++]);
        s_psysinfo->mem.ShmemPmdMapped = atol(meminfo_result[i++]);
        s_psysinfo->mem.CmaTotal = atol(meminfo_result[i++]);
        s_psysinfo->mem.CmaFree = atol(meminfo_result[i++]);
        s_psysinfo->mem.HugePages_Total = atol(meminfo_result[i++]);
        s_psysinfo->mem.HugePages_Free = atol(meminfo_result[i++]);
        s_psysinfo->mem.HugePages_Rsvd = atol(meminfo_result[i++]);
        s_psysinfo->mem.HugePages_Surp = atol(meminfo_result[i++]);
        s_psysinfo->mem.Hugepagesize = atol(meminfo_result[i++]);
        s_psysinfo->mem.DirectMap4k = atol(meminfo_result[i++]);
        s_psysinfo->mem.DirectMap2M = atol(meminfo_result[i++]);
    }while(0);
    return;
}
/**
*********************************************************************
* @brief： 更新存储情报
* @param[in]： 
               none
* @param[out]：
              none
* @return： 返回值说明 
*********************************************************************
*/
static void sys_storageinfo_update(void)
{
    FILE *fp;
    sint8 nouse[80], buf[512], filesystem[20];
    sint64 temptotal, tempused, tempavailable;
    sint8 *ptmp = NULL;
    do
    {
        fp = popen("df", "r");
        if (NULL == fp)
        {
            break;
        }
        ptmp = fgets(buf, 512, fp);
        UNUSED(ptmp);
        sint64 dev_total = 0, dev_used = 0, dev_available = 0;
        /* 文件系统 1K-块 已用 可用 已用% 挂载点 */
        while (6 == fscanf(fp, "%s %ld %ld %ld %s %s", filesystem, &temptotal, &tempused, &tempavailable, nouse, nouse))
        {
            //排除外接设备,临时文件，光驱
            if (strcmp(filesystem, "udev") != 0 && strcmp(filesystem, "tmpfs") != 0 && strcmp(filesystem, "/dev/sr0") != 0)
            {
                dev_total += temptotal;
                dev_used += tempused;
                dev_available += tempavailable;
            }
        }
        pclose(fp);

        s_psysinfo->disk.free = dev_available;
        s_psysinfo->disk.total = dev_total;
        s_psysinfo->disk.used = dev_used;
    }while(0);

    return;
}
/**
*********************************************************************
* @brief： 更新cpu平均负载
* @param[in]： 

* @param[out]：

* @return： 返回值说明 
*********************************************************************
*/
static void sys_cpuinfo_avage_update(void)
{
    sint32 fd = 0;
    sint8 buff[80] = {0};
    size_t tmpsize = 0;
    
    do{
        fd = open(FILE_PATH_LOADAVG, O_RDONLY);
        if (0 == fd)
        {
            SMIOS_FMT_DEBUG("open error [%s]\n", FILE_PATH_LOADAVG);
            break;
        }
        tmpsize = read(fd, buff, 80);
        UNUSED(tmpsize);
        //拆分成多个字符串放入buff
        sscanf(buff, "%s %s %s", s_psysinfo->cpu.cpurate_1min, s_psysinfo->cpu.cpurate_5min, s_psysinfo->cpu.cpurate_15min);
        close(fd);
    }while(0);
    
    return;
}
/**
*********************************************************************
* @brief： 更新包含平均负载的cpu情报
* @param[in]： 
               none
* @param[out]：
               none
* @return： none
*********************************************************************
*/
static void sys_cpuinfo_update(void)
{
    sint8 nousebuff[10] = {0};
    FILE *fp = NULL;
    sint8 buffer[50] = {0};
    sint8 *pbuff = NULL;
    uint8 cpuinfo_resulte[CPU_INFONAME_SIZE][20] = {};
    uint8 i = 0;
    sint64 bitflag = 0;
    do
    {
        fp = fopen(FILE_PATH_CPUINFO, "rb");
        UNUSED(nousebuff);
        if (NULL == fp)
        {
            SMIOS_FMT_DEBUG("open error[%s]\n", FILE_PATH_CPUINFO);
            break;
        }
        while ((fgets(buffer, 50, fp)) != NULL)
        {
            for (i = 0; i < (sint8)CPU_INFONAME_SIZE; i++)
            {
                //在cpu_infoname已经查找到的，在下次循环中跳过该字符串,避免被重复
                if ((GET_BIT_VALUE(bitflag,i) == 0) && (pbuff = strstr(buffer,(const char *)cpu_infoname[i]))!= NULL)
                {   
                    //替换：
                    pbuff = strtok(buffer, ":");
                    pbuff = strtok(NULL, ":");
                    pbuff[strlen(pbuff) - 1] = 0;
                    sscanf(pbuff, "%s", cpuinfo_resulte[i]);
                    bitflag = SET_BIT_ON(bitflag,i);
                    break;
                }
            }
        }
        i = 0;
        memcpy(s_psysinfo->cpu.vendor_id, cpuinfo_resulte[i], sizeof(cpuinfo_resulte[i]));
        i++;
        s_psysinfo->cpu.cpu_family = (sint8)atol((const char *)cpuinfo_resulte[i++]);
        s_psysinfo->cpu.model = (sint8)atol((const char *)cpuinfo_resulte[i++]);
        memcpy(s_psysinfo->cpu.model_name, cpuinfo_resulte[i], sizeof(cpuinfo_resulte[i]));
        i++;
        s_psysinfo->cpu.stepping = (sint8)atol((const char *)cpuinfo_resulte[i++]);
        memcpy(s_psysinfo->cpu.cpu_MHz, cpuinfo_resulte[i], sizeof(cpuinfo_resulte[i]));
        i++;
        s_psysinfo->cpu.cache_size = (sint32)atol((const char *)cpuinfo_resulte[i++]);
        s_psysinfo->cpu.physical_id = (sint8)atol((const char *)cpuinfo_resulte[i++]);
        s_psysinfo->cpu.cpu_cores = (sint8)atol((const char *)cpuinfo_resulte[i++]);
        memcpy(s_psysinfo->cpu.fpu, cpuinfo_resulte[i], sizeof(cpuinfo_resulte[i]));
        i++;
        memcpy(s_psysinfo->cpu.fpu_exception, cpuinfo_resulte[i], sizeof(cpuinfo_resulte[i]));
        i++;
        s_psysinfo->cpu.cpuid_level = (sint32)atol((const char *)cpuinfo_resulte[i++]);
        memcpy(s_psysinfo->cpu.wp, cpuinfo_resulte[i], sizeof(cpuinfo_resulte[i]));

        fclose(fp);
        sys_cpuinfo_avage_update();
    }while(0);
    
    return;
}
/**
*********************************************************************
* @brief： 更新OS版本跟OS类型情报到结构体中
* @param[in]： 
              none
* @param[out]：
              none
* @return： none 
*********************************************************************
*/
static void sys_osversion_update(void)
{
    sint32 fd = 0;
    sint8 buffer[80] = {0};
    sint8 nouse[50] = {0};
    size_t tempsize = 0;
    do{
        fd = open(FILE_PATH_VERSION, O_RDONLY);
        if (0 == fd)
        {
            printf("open error[%s]\n", FILE_PATH_VERSION);
            memset(&s_psysinfo->os, 0, sizeof(OSINFO_T));
            break;
        }
        tempsize = read(fd, (void *)buffer, 80);
        UNUSED(tempsize);
        //拆分成多个字符串放入buff
        sscanf(buffer, "%s %s %s %s", s_psysinfo->os.type, nouse, s_psysinfo->os.version, nouse);
        close(fd);
    }while(0);
    
    return;
}
/**
*********************************************************************
* @brief： 从系统情报结构体中取得OS版本跟OS类型
* @param[in]： none

* @param[out]：
               pversion os情报（类型 版本)
* @return： 返回值说明 <0 失败
                       =0 成功
*********************************************************************
*/
sint8 sys_osversion_get(sint8 *pversion)
{
    sint8 buffer[80] = {0};
    sint8 nret = -1;

    do
    {
        if ((strlen(s_psysinfo->os.type) == 0 && strlen(s_psysinfo->os.version) == 0) || (NULL == pversion))
        {
            nret = -1;
        }
        strcat(buffer, s_psysinfo->os.type);
        strcat(buffer, " ");
        strcat(buffer, s_psysinfo->os.version);

        memcpy(pversion, buffer, strlen(buffer));
        nret = 0;
    }while(0);
    
    return nret;
}
/**
*********************************************************************
* @brief： 从系统情报结构体中取得cpu的1分钟负载
* @param[in]： 
               none
* @param[out]：
               none
* @return： 返回值说明 cpu 1分钟负载率
*********************************************************************
*/
sint32 sys_cpurate_get(void)
{
    sint32 cpurate = -1;
    sys_cpuinfo_avage_update();
    if (strlen(s_psysinfo->cpu.cpurate_1min) > 0)
    {
        cpurate = (sint32)(atof(s_psysinfo->cpu.cpurate_1min) * 100);
    }
    return cpurate;
}
/**
*********************************************************************
* @brief： 从系统情报结构体中取得ram使用率
* @param[in]： 
             none
* @param[out]：
             none
* @return： none
*********************************************************************
*/
sint32 sys_ramrate_get(void)
{
    sint32 retram = -1;

    sys_meminfo_update();
    if (((s_psysinfo->mem.MemAvailable > s_psysinfo->mem.MemTotal) || (s_psysinfo->mem.MemTotal == 0)))
    {
        retram = -1;
    }
    else
    {
        retram = (100 - (s_psysinfo->mem.MemFree) * 100 / (s_psysinfo->mem.MemTotal));
    }

    return retram;
}
/**
*********************************************************************
* @brief： 从系统情报结构体中取得存储使用率
* @param[in]： 
             none
* @param[out]：
            none
* @return： 存储使用率
*********************************************************************
*/
sint32 sys_storgaerate_get(void)
{
    sint32 usedrate = -1;
    do{
        sys_storageinfo_update();
        if (s_psysinfo->disk.used > s_psysinfo->disk.total)
        {
            usedrate = -1;
            break;
        }
        usedrate = ((s_psysinfo->disk.total-s_psysinfo->disk.free) * 100 / s_psysinfo->disk.total);
    }while(0);
    
    return usedrate;
}
/**
*********************************************************************
* @brief： TODO 检测falsh坏块
* @param[in]： 

* @param[out]：

* @return： 返回值说明 
*********************************************************************
*/
sint32 sys_flashfailure_detect(void)
{
    return 0;
}
/**
*********************************************************************
* @brief： TODO 检测时钟错误
* @param[in]： 

* @param[out]：

* @return： 返回值说明 
*********************************************************************
*/
sint32 sys_clockfailure_get(void)
{
    return 0;
}
/**
*********************************************************************
* @brief： TODO检测ram错误
* @param[in]： 

* @param[out]：

* @return： 返回值说明 
*********************************************************************
*/
sint32 sys_ramfailure_get(void)
{
    return 0;
}
/**
*********************************************************************
* @brief： TODO检测esam错误
* @param[in]： 

* @param[out]：

* @return： 返回值说明 
*********************************************************************
*/
sint32 sys_esamfailure_get(void)
{
    return 0;
}
/**
*********************************************************************
* @brief： 通过popen执行shell命令
* @param[in]： cmd shell命令

* @param[out]：

* @return： none
*********************************************************************
*/
void sys_run_cmd(sint8 *cmd)
{
    UNUSED(cmd);
    FILE *fp;
    // char buffer[80]={0};
    fp=popen(cmd,"r");
    // fgets(buffer,sizeof(buffer),fp);
    if(NULL != fp)
    {
        pclose(fp);
    }
}
/**
*********************************************************************
* @brief： 从系统情报结构体中取得内存情报
* @param[in]： 
            none
* @param[out]：
            pmeminfo 内存情报结构体指针
* @return： <0 不成功
            = 0 成功
*********************************************************************
*/
sint32 sys_meminfo_get(MEMINFO_T *pmeminfo)
{
    sint32 nret = -1;
    do{
        if(NULL == s_psysinfo)
        {
            nret = -1;
            break;
        }
        sys_meminfo_update();
        if ((NULL == pmeminfo))
        {
            nret = -1;
            break;
        }
        memcpy(pmeminfo, &s_psysinfo->mem, sizeof(s_psysinfo->mem));
        nret = 0;
    }while(0);
    return nret;
}
/**
*********************************************************************
* @brief： 从系统情报中取得cpu情报
* @param[in]： 
            none
* @param[out]：
             pcpuinfo cpu情报结构体指针
* @return： <0 不成功
            = 0 成功
*********************************************************************
*/
sint32 sys_cpuinfo_get(CPUINFO_T *pcpuinfo)
{
    sint32 nret = -1;
    do{
        if(NULL == s_psysinfo)
        {
            nret = -1;
            break;
        }
        sys_cpuinfo_update();
        if (NULL == pcpuinfo)
        {
            nret = -1;
            break;
        }
        memcpy(pcpuinfo, &s_psysinfo->cpu, sizeof(CPUINFO_T));
        nret = 0;
    }while(0);

    return nret;
}
/**
*********************************************************************
* @brief： 从系统情报结构体中取得存储信息
* @param[in]： 
               none
* @param[out]：
             pdiskinfo
* @return： <0 不成功
            = 0 成功
*********************************************************************
*/
sint32 sys_storageinfo_get(DISKINFO_T *pdiskinfo)
{
    sint32 nret = -1;
    do{
        if(NULL == s_psysinfo)
        {
            nret = -1;
            break;
        }
        sys_storageinfo_update();
        if (NULL == pdiskinfo)
        {
            nret = -1;
            break;
        }
        memcpy(pdiskinfo, &s_psysinfo->disk, sizeof(DISKINFO_T));
        nret = 0;
    }while(0);

    return nret;
}
/**
*********************************************************************
* @brief： 取得所有的系统情报
* @param[in]： 
             none
* @param[out]：
               psysteminfo 系统情报结构体指针
* @return： <0 不成功
            = 0 成功
*********************************************************************
*/
sint32 sys_systeminfo_get(SYSTEMINFO_T *psysteminfo)
{
    sint32 nret = -1;
    do{
        if(NULL == s_psysinfo)
        {
            nret = -1;
            break;
        }
        sys_cpuinfo_update();
        sys_storageinfo_update();
        sys_meminfo_update();
        sys_osversion_update();
        if (NULL == psysteminfo)
        {
            nret = -1;
            break;
        }
        memcpy(psysteminfo, s_psysinfo, sizeof(SYSTEMINFO_T));
        nret = 0;
    }while(0);

    return nret;
}
/**
*********************************************************************
* @brief： 初始化结构体，更新所有情报
* @param[in]： 
             none
* @param[out]：
            none
* @return： none
*********************************************************************
*/
void sys_systeminfo_start(void)
{
    do{
        if(NULL != s_psysinfo)
        {
            //已经启动了，不需要再次启动。
            break;
        }
        s_psysinfo = malloc(sizeof(SYSTEMINFO_T));
        if(NULL == s_psysinfo)
        {
            break;
        }
        sys_cpuinfo_update();
        sys_storageinfo_update();
        sys_meminfo_update();
        sys_osversion_update();
    }while(0);
}
/**
*********************************************************************
* @brief： 释放结构体
* @param[in]： 
            none
* @param[out]：
            none
* @return： none
*********************************************************************
*/
void sys_systeminfo_stop(void)
{
    if(NULL != s_psysinfo)
    {
        free(s_psysinfo);
        s_psysinfo = NULL;
    }
}

void sys_get_start_time(OOP_DATETIME_S_T *starttime)
{
    char *cmd="date -d \"$(awk -F. '{print $1}' /proc/uptime) second ago\" +\"%Y-%m-%d %H:%M:%S\"";
    char result[200]={0};
    int year,mon,day,hour,min,sec;
    
    cmd_execute(cmd,result);
    sscanf(strim(result),"%d-%d-%d %d:%d:%d",&year,&mon,&day,&hour,&min,&sec);
    starttime->year = year;
    starttime->month = mon;
    starttime->day = day;
    starttime->hour = hour;
    starttime->minute = min;
    starttime->second = sec;
}

void sys_dev_run_info_get(DEV_RUN_INTO_T *info)
{
    char cmd[200]={0};
    char result[200]={0};

    sys_osversion_update();

    info->memrate = sys_ramrate_get();
    info->virmem = s_psysinfo->mem.VmallocTotal;
    info->storagerate = sys_storgaerate_get();
    info->cpurate = sys_cpurate_get();
#ifdef PRODUCT_CCU

    sprintf(cmd,"cat /sys/class/version/vkernel |awk -F '.' '{print $1}'");
	memset(result,0x00,sizeof(result));
    cmd_execute(cmd,result);
	strcpy(info->osname,strim(result));

	sprintf(cmd,"cat /sys/class/version/vkernel |awk -F '_' '{print $2}'");
	memset(result,0x00,sizeof(result));
    cmd_execute(cmd,result);
	sprintf(info->oskernel,"%s.%s", info->osname, strim(result));

	sprintf(cmd,"version -d");
	memset(result,0x00,sizeof(result));
	cmd_execute(cmd,result);
	strcpy(info->osversion, strim(result));
#else
	sprintf(cmd,"version -k |awk -F ':' '{print $2}'");
    cmd_execute(cmd,result);
    strcpy(info->oskernel, strim(result));

	strcpy(info->osname,s_psysinfo->os.type);
	
	sprintf(cmd,"version -d | grep software|awk -F ':' '{print $2}'");
	memset(result,0x00,sizeof(result));
	cmd_execute(cmd,result);
	strcpy(info->osversion, strim(result));
#endif

    sys_get_start_time(&info->starttime);
}
