/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 其他辅助功能代码
* @date： 2019-10-25
*********************************************************************
*/

#include "ac_otherfun.h"
#include "ac_main.h"
#include "ac_redef.h"
#include "rtc.h"
#include "power.h"
//#include "gpio.h"
#include <sys/prctl.h>
#include "ac_save.h"

#if TID_CONTAINER_APP == 1
const APP_CHECK_T appCheck[] = 
{
    {"dbCenter",    "baseContainer",  "/usr/local/extapps/dbCenter.tar"},
    {"ccoRouter",   "baseContainer",  "/usr/local/extapps/ccoRouter.tar"},
    {"puAmr",       "amrContainer",   "/usr/local/extapps/puAmr.tar"},
    {"taskManager", "edgerContainer", "/usr/local/extapps/taskManager.tar"},
    {"lcMonitor",   "edgerContainer", "/usr/local/extapps/lcMonitor.tar"},
    {"pdAmr",       "pdContainer",    "/usr/local/extapps/pdAmr.tar"},
};
const uint32 appCheckNum = sizeof(appCheck)/sizeof(APP_CHECK_T);

/*******************************************************************************
* 函数名称: start_containerAPP_proc
* 函数功能: 重启APP
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void start_containerAPP_proc(char *appName)
{
    FILE*  fp = NULL;
    char   cmd[128] = {0};

    if (appName == NULL)
    {
        SELFCHK_FMT_DEBUG("ptr nill appName[%p]\n", appName);
        return;
    }

    //解压安装包
    sprintf(cmd, "tar -xvf /usr/local/extapps/%s.tar", appName);
    fp = popen(cmd, "r"); 
    if (NULL == fp)   
    {   
        SELFCHK_FMT_DEBUG("%s_scan:popen error\n", appName);  
        return;   
    }
    fclose(fp);
    SELFCHK_FMT_TRACE("tar -xvf /usr/local/extapps/%s.tar\n", appName);

    //执行脚本
    sprintf(cmd, "/usr/local/extapps/%s/bin/startup_app.sh", appName);
    system(cmd);
    SELFCHK_FMT_TRACE("/usr/local/extapps/%s/bin/startup_app.sh\n", appName);

    return;
}

/*******************************************************************************
* 函数名称: get_containerInfo_by_appName
* 函数功能: 根据APP名称获取容器信息
* 输入参数: appName       - APP名称 
* 输出参数: containerName - 容器名称
* 返 回 值: 错误码s
*******************************************************************************/
int get_containerInfo_by_appName(char *appName, char *containerName, char *appTarPath)
{
    uint32 i = 0;
    
    for (i = 0; i < appCheckNum; i++)
    {
        if (strcmp(appName, appCheck[i].appName) == 0)
        {
            strcpy(containerName, appCheck[i].containerName);
            strcpy(appTarPath, appCheck[i].appTarPath);
            break;
        }
    }

    if (i == appCheckNum)
    {
        return -1;
    }

    return 0;
}

/*******************************************************************************
* 函数名称: if_containerAPP_exit
* 函数功能: APP自检测程序
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
BOOL if_containerAPP_exit(char *appName)
{
    FILE*  fp = NULL;
    char   cmd[128] = {0};
    char   buf[512] = {0};
    char   *str = NULL;
    char   containerName[64] = {0};
    char   appTarPath[64] = {0};

    if (appName == NULL)
    {
        SELFCHK_FMT_DEBUG("ptr nll appName[%p]\n", appName);
        return TRUE;
    }

    if (get_containerInfo_by_appName(appName, containerName, appTarPath) != 0)
    {
        SELFCHK_FMT_DEBUG("no find container appName[%p]\n", appName);
        return TRUE;
    }

    if (0 != access(appTarPath, F_OK))
    {
        SELFCHK_FMT_DEBUG("the %s inexistence\n", appTarPath);
        return TRUE;
    }
    else
    {
        SELFCHK_FMT_TRACE("the %s is existence\n", appTarPath);
    }

    sprintf(cmd, "docker exec %s ls /usr/local/extapps/", containerName);
    fp = popen(cmd, "r"); 
    if (NULL == fp)   
    {   
        SELFCHK_FMT_DEBUG("%s_scan:popen error\n", appName);  
        return TRUE;   
    }

    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        SELFCHK_FMT_DEBUG("scan[%s] from[%s:/usr/local/extapps/] the content:%s", appName, containerName, buf);
        if ((str = strstr(buf, appName)) != NULL)
        {
            fclose(fp);
            return TRUE;
        }
    }

    fclose(fp);

    return FALSE;
}

/*******************************************************************************
* 函数名称: proc_container_app_chk
* 函数功能: APP自检测程序
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void proc_container_app_chk(char *appName, struct timespec curtime, struct timespec oldtime, uint32 *pNoCheckCnt)
{
    if ((appName == NULL) || (pNoCheckCnt == NULL))
    {
        SELFCHK_FMT_DEBUG("input err, appName[%p], pNoCheckCnt[%p]\n", appName, pNoCheckCnt);
        return;
    }

    if ((curtime.tv_sec - oldtime.tv_sec > 60) && ((*pNoCheckCnt) < 12))
    {
        //仅上电10分钟后检测一次
        (*pNoCheckCnt) ++;

        if ((*pNoCheckCnt) > 10)
        {
            if (0 != access("/system/etc/init_finsh", F_OK))
            {
                SELFCHK_FMT_TRACE("proc_container_app_chk err. /system/etc/init_finsh inexistence\n");
                return;
            }

            //连续三次判断出没有容器
            if (!if_containerAPP_exit(appName)) 
            {
                sleep(1);
                if (!if_containerAPP_exit(appName))
                {
                    sleep(1);
                    if (!if_containerAPP_exit(appName))
                    {
                        SELFCHK_FMT_DEBUG("start %s pNoCheckCnt[%d]\n", appName, (*pNoCheckCnt));
                        start_containerAPP_proc(appName);
                    }
                }
            }
        }
    }

    return;
}

#endif

#if TID_TIME==1

/*******************************************************************************
* 函数名称: commfun_selectsleep
* 函数功能: 延时函数
* 输入参数: sec - 秒; us - 微秒
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void time_chk_selectsleep(unsigned sec, unsigned us)
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = us;
    select(0, 0, 0, 0, &tv);
}

/*******************************************************************************
* 函数名称: time_sysnc_from_rtc
* 函数功能: 从外部时钟同步到系统
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int time_sysnc_from_rtc(void)
{
    int ret = -1;
    struct tm time_rtc;

    memset(&time_rtc, 0, sizeof(time_rtc));

    RTC_DEVICE_T* dev = (RTC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_RTC);
    if (NULL == dev)
    {
        TMCHK_FMT_DEBUG("dev_rtc is null\n");
        return -1;
    }

    ret = dev->rtc_time_get(dev, &time_rtc);
    if (ret >= 0)
    {
        TMCHK_FMT_TRACE("time_sysnc_from_rtc read rtc time: %d-%d-%d %d:%d:%d\n", time_rtc.tm_year + 1900, time_rtc.tm_mon + 1,
            time_rtc.tm_mday, time_rtc.tm_hour, time_rtc.tm_min, time_rtc.tm_sec);

        //同步系统时钟为外部硬件时钟
        ret = dev->rtc_time_set(dev, &time_rtc);
        if (ret >= 0)
        {
            TMCHK_FMT_TRACE("rtc_time_set ok ret[%d]\n", ret);
        }
        else
        {
            TMCHK_FMT_DEBUG("rtc_time_set err ret[%d]\n", ret);
        }
    }
    else
    {
        TMCHK_FMT_DEBUG("time_sysnc_from_rtc read rtc get time is failed err[%d]\n", ret);
    }

    hal_device_release((HW_DEVICE*)dev);

    return ret;
}

/*******************************************************************************
* 函数名称: read_time_form_rtc
* 函数功能: 从外部时钟获取时间
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int read_time_form_rtc(struct tm* time_rtc)
{
    int       ret = -1;

    RTC_DEVICE_T* dev = (RTC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_RTC);
    if (NULL == dev)
    {
        TMCHK_FMT_DEBUG("dev_rtc is null\n");
        return -1;
    }

    ret = dev->rtc_time_get(dev, time_rtc);
    if (ret >= 0)
    {
        TMCHK_FMT_TRACE("read_time_form_rtc read rtc time: %d-%d-%d %d:%d:%d\n", time_rtc->tm_year + 1900, time_rtc->tm_mon + 1,
            time_rtc->tm_mday, time_rtc->tm_hour, time_rtc->tm_min, time_rtc->tm_sec);
    }
    else
    {
        TMCHK_FMT_DEBUG("read_time_form_rtc read rtc get time is failed err[%d]\n", ret);
    }

    hal_device_release((HW_DEVICE*)dev);

    return ret;
}

/*******************************************************************************
* 函数名称: proc_time_chk
* 函数功能: 对时处理, 每隔10分钟对一次值
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void proc_time_chk(uint32* cntchk)
{
    struct tm* time_sys;
    struct tm time_rtc;
    time_t timep = 0;
    int32  ret = 0;
    int32  tmpSec = 0;
    int32  cnt = 0;

    memset(&time_rtc, 0, sizeof(time_rtc));

    while (1)
    {
        *cntchk = 1;	//每隔一秒就置对时线程标记为存活状态

        time(&timep);
        time_sys = localtime(&timep);
        if (time_sys->tm_sec != 12)
        {
            time_chk_selectsleep(1, 0);
            continue;
        }

        ret = read_time_form_rtc(&time_rtc);
        if (ret < ERR_OK)
        {
            TMCHK_FMT_DEBUG("read_time_form_rtc err[%d]\n", ret);
            time_chk_selectsleep(10, 0);
            continue;
        }

        time(&timep);
        time_sys = localtime(&timep);

        // 比较外部时钟和系统时钟,只有年月日时分都相同,秒相差不到2时才不用对时
        tmpSec = time_rtc.tm_sec - time_sys->tm_sec;
        if ((time_rtc.tm_year != (time_sys->tm_year)) ||
            (time_rtc.tm_mon != time_sys->tm_mon) ||
            (time_rtc.tm_mday != time_sys->tm_mday) ||
            //((time_rtc.tm_wday)!= time_sys->tm_wday) ||
            (time_rtc.tm_hour != time_sys->tm_hour) ||
            (time_rtc.tm_min != time_sys->tm_min) ||
            (tmpSec > 2) || (tmpSec < -2))
        {
            ret = time_sysnc_from_rtc();
            if (ret < ERR_OK)
            {
                TMCHK_FMT_DEBUG("time_sysnc_from_rtc err[%d]\n", ret);
                TMCHK_FMT_DEBUG("sys time: %d-%d-%d %d:%d:%d\n", time_sys->tm_year + 1900, time_sys->tm_mon + 1,
                    time_sys->tm_mday, time_sys->tm_hour, time_sys->tm_min, time_sys->tm_sec);
                TMCHK_FMT_DEBUG("rtc time: %d-%d-%d %d:%d:%d\n", time_rtc.tm_year + 1900, time_rtc.tm_mon + 1,
                    time_rtc.tm_mday, time_rtc.tm_hour, time_rtc.tm_min, time_rtc.tm_sec);
            }
            else
            {
                TMCHK_FMT_DEBUG("time_sysnc_from_rtc ok[%d]\n", ret);
                TMCHK_FMT_DEBUG("sys time: %d-%d-%d %d:%d:%d\n", time_sys->tm_year + 1900, time_sys->tm_mon + 1,
                    time_sys->tm_mday, time_sys->tm_hour, time_sys->tm_min, time_sys->tm_sec);
                TMCHK_FMT_DEBUG("rtc time: %d-%d-%d %d:%d:%d\n", time_rtc.tm_year + 1900, time_rtc.tm_mon + 1,
                    time_rtc.tm_mday, time_rtc.tm_hour, time_rtc.tm_min, time_rtc.tm_sec);
            }
        }

        while (cnt++ < 600)
        {
            *cntchk = 1;	//每隔一秒就置对时线程标记为存活状态
            time_chk_selectsleep(1, 0);
        }
        cnt = 0;
    }

    return;
}

/*********************************************************************
 功能描述：对时线程
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void* pthread_time_chk(void* arg)
{
    INIT_FMT_DEBUG("pthread_time_chk() start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T* tArg = (THREAD_ARG_T*)arg;

    //线程名
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    proc_time_chk(&tArg->cnt);

    pthread_exit(NULL);
}
#endif

#if TID_MAGNETIC == 1
#if defined PRODUCT_ZCU_1

static int ymfd = -1;

/*******************************************************************************
* 函数名称: magnetic_dev_init
* 函数功能: 打开磁场设备
* 输入参数: 无
* 输出参数: 无
* 返 回 值: IO电平
*******************************************************************************/
void magnetic_dev_init(void)
{
    ymfd = open("/dev/ym", O_RDONLY|O_NONBLOCK);
    if (ymfd <= 0)
    {
        SELFCHK_FMT_DEBUG("打开磁场设备/dev/ym失败 ymfd = %d\n", ymfd);
    }
    else
    {
        SELFCHK_FMT_DEBUG("打开磁场设备/dev/ym成功 ymfd = %d\n", ymfd);
    }

    return;
}

/*******************************************************************************
* 函数名称: get_mapnetic_status
* 函数功能: 获取磁场状态
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 0 表示检查到磁场 1 表示没检查磁场 -1 表示接口异常
*******************************************************************************/
int get_mapnetic_status(void)
{
    int iRet = 1;

    uint32 data = 0xffff;

    if (ymfd <= 0)
    {
        ymfd = open("/dev/ym", O_RDONLY|O_NONBLOCK);
        if (ymfd <= 0)
        {
            return -1;
        }
    }
    
    if (read(ymfd, &data, sizeof(data)) != 4) 
    {
        SELFCHK_FMT_DEBUG("读取磁场状态失败\n");
        close(ymfd);
        ymfd = -1;
        return -2;
    }

    SELFCHK_FMT_TRACE("读取磁场状态[0X%04X]\n", data);
    iRet = ((data >> 24) & 0x01);

    //测试代码
    #if 0
    static int cnt = 0;
    static int value = 1;
    cnt ++;
    if (cnt > 10)
    {
        cnt = 0;
        if (value == 1)
        {
            value = 0;
        }
        else
        {
            value = 1;
        }
    }
    iRet = value;
    #endif

    return iRet;
}

#else
/*******************************************************************************
* 函数名称: get_gpio_value
* 函数功能: 读取IO接口
* 输入参数: 无
* 输出参数: 无
* 返 回 值: IO电平
*******************************************************************************/
int get_gpio_value(unsigned int pin)
{
    int fd;
    int ret_value;
    int count = 100;
    int value = GPIO_IN_PULLUP;

    while (((fd = open(GPIO_DEV_NAME, O_RDONLY)) < 0) && ((count--) > 0))
        usleep(2000);
    if (fd < 0)
    {
        SELFCHK_FMT_DEBUG("can not open gpio device.\n");
        return -1;
    }

    /* (0 low | 1 high | <0 error) */
    ret_value = ioctl(fd, value, pin);

    SELFCHK_FMT_TRACE("get_gpio_value ret_val %d\n", ret_value);

    //关闭
    close(fd);

    return ret_value;
}

/*******************************************************************************
* 函数名称: get_mapnetic_status
* 函数功能: 获取磁场状态
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 0 表示检查到磁场 1 表示没检查磁场 -1 表示接口异常
*******************************************************************************/
int get_mapnetic_status(void)
{
    int iRet = 1;
    int io1 = 1;
    int io2 = 1;

    io1 = get_gpio_value(CC1_PIN);
    if (io1 < 0)
    {
        SELFCHK_FMT_DEBUG("open gpio CC1 error\n");
        return -1;
    }

    io2 = get_gpio_value(CC2_PIN);
    if (io2 < 0)
    {
        SELFCHK_FMT_DEBUG("open gpio CC2 error\n");
        return -1;
    }

    //2个IO任何一个低电平表示有磁场
    if (io1 == 0 || io2 == 0)
    {
        iRet = 0;
    }
    else
    {
        iRet = 1;
    }

    return iRet;
}
#endif
/*******************************************************************************
* 函数名称: proc_magnetic_chk
* 函数功能: 磁场检测程序
* 输入参数: pMagneticStatusLast 上次磁场状态 pMagneticStatusTmp 用于防抖
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void proc_magnetic_chk(int* pMagneticStatusLast, int* pMagneticStatusTmp)
{
    int magneticStatus = 1;
    static int cnt = 0;

    //做3次防抖
    magneticStatus = get_mapnetic_status();
    if (*pMagneticStatusTmp == magneticStatus)
    {
        cnt++;
        if (cnt < 3)
        {
            return;
        }
    }
    else
    {
        *pMagneticStatusTmp = magneticStatus;
        cnt = 0;
        return;
    }

    //防抖计数清0
    cnt = 0;

    if (*pMagneticStatusLast != magneticStatus)
    {
        SELFCHK_FMT_DEBUG("发生磁场干扰事件! statusLast[%d] status[%d]\n", *pMagneticStatusLast, magneticStatus);
        gEventCommInfo.EffectState = magneticStatus;
        InitEventPara();
        SaveMagneticEventOccur();

        //重新置上次状态
        *pMagneticStatusLast = magneticStatus;
    }

    return;
}
#endif

#if TID_SLEF_CHECK == 1
#if TID_HUB == 1
/*******************************************************************************
* 函数名称: reboot_hub_proc
* 函数功能: 重启hub
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void reboot_hub_proc(void)
{
    

    #ifndef BOARD_3358
    if (0 != access("/usr/local/extapps/drivers/usb2517.ko", F_OK))
    {
        SELFCHK_FMT_DEBUG("/usr/local/extapps/drivers/usb2517.ko inexistence\n");
        return;
    }
    #endif

    SELFCHK_FMT_TRACE("reset usb power. \n");
    POWER_DEVICE_T* dev = (POWER_DEVICE_T*)hal_device_get(HW_DEVICE_ID_POWER);
    if (NULL == dev)
    {
        SELFCHK_FMT_DEBUG("hub dev null\n");
        return;
    }
#ifdef PRODUCT_ECU
    int32 ret = ERR_NORMAL;
    ret = dev->power_ply_vcc_out_set(dev, MODULE_USB_HUB, 0);
    if (ret != ERR_OK)
    {
        hal_device_release((HW_DEVICE*)dev);
        SELFCHK_FMT_DEBUG("power_ply_vcc_out_set poweroff err ret[%d]\n", ret);
        return;
    }

    sleep(2);

    ret = dev->power_ply_vcc_out_set(dev, MODULE_USB_HUB, 1);
    if (ret != ERR_OK)
    {
        hal_device_release((HW_DEVICE*)dev);
        SELFCHK_FMT_DEBUG("power_ply_vcc_out_set poweron err ret[%d]\n", ret);
        return;
    }
#endif
    #ifndef BOARD_3358
    //重新加载驱动
    system("rmmod /usr/local/extapps/drivers/usb2517.ko");
    sleep(1);
    system("insmod /usr/local/extapps/drivers/usb2517.ko");
    SELFCHK_FMT_DEBUG("reset usb driver ok. \n");
    #endif

    hal_device_release((HW_DEVICE*)dev);

    return;
}

/*******************************************************************************
* 函数名称: if_exist_ttyacm_dev
* 函数功能: 检测HUB上是否有设备
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
BOOL if_exist_ttyacm_dev(void)
{
    uint32 i = 0;
    FILE* fp = NULL;
    char   cmd[128] = { 0 };
    int32  lastUsbHubNo = -1;
    int32  usbHubNo = -1;
    char   buf[512] = { 0 };
    char* str = NULL;

    sprintf(cmd, "find /sys/devices/ -name ttyACM*");

    //智芯核心板
    /*sysadm@SCT230A:~$ find /sys/devices/ -name ttyACM*
    /sys/devices/soc/1c19000.ohci1-controller/usb2/2-1/2-1.5/2-1.5:1.0/tty/ttyACM0
    /sys/devices/soc/1c19000.ohci1-controller/usb2/2-1/2-1.5/2-1.5:1.2/tty/ttyACM1
    /sys/devices/soc/1c19000.ohci1-controller/usb2/2-1/2-1.5/2-1.5:1.4/tty/ttyACM2*/

    //瑞芯微核心板
    /*[root@px30_32:~]# find /sys/devices/ -name ttyACM*
    /sys/devices/platform/ff340000.usb/usb2/2-1/2-1.5/2-1.5:1.0/tty/ttyACM0
    /sys/devices/platform/ff340000.usb/usb2/2-1/2-1.5/2-1.5:1.4/tty/ttyACM2
    /sys/devices/platform/ff340000.usb/usb2/2-1/2-1.5/2-1.5:1.2/tty/ttyACM1*/

    //如果没有扫描到，就尝试多次
    for (i = 0; i < 10; i++)
    {
        fp = popen(cmd, "r");
        if (NULL == fp)
        {
            SELFCHK_FMT_DEBUG("mdl_channel_scan:popen error\n");
            usbHubNo = -1;
            return FALSE;
        }

        while (fgets(buf, sizeof(buf), fp) != NULL)
        {
            SELFCHK_FMT_TRACE("scan channal buf:%s\n", buf);
            if ((str = strstr(buf, "/usb")) != NULL)
            {
                usbHubNo = str[4] - 0x30;
                if ((usbHubNo > 9) || (usbHubNo < 0))
                {
                    continue;
                }
                else
                {
                    if (lastUsbHubNo != usbHubNo)
                    {
                        SELFCHK_FMT_TRACE("mdl_channel_scan:lastUsbHubNo[%d] != usbHubNo[%d]\n", lastUsbHubNo, usbHubNo);
                    }
                    lastUsbHubNo = usbHubNo;
                }
            }
        }

        fclose(fp);

        if ((usbHubNo >= 0) && (usbHubNo <= 9))
        {
            break;
        }
        else
        {
            usbHubNo = -1;
            lastUsbHubNo = -1;
        }
    }

    SELFCHK_FMT_TRACE("mdl_channel_scan:find usbHubNo[%d]\n", usbHubNo);

    if (usbHubNo != 2)
    {
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
* 函数名称: proc_hub_chk
* 函数功能: hub自检测程序
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void proc_hub_chk(struct timespec curtime, struct timespec oldtime, uint32* pNoDevCnt)
{
    if (curtime.tv_sec - oldtime.tv_sec > 60)
    {
        //连续三次判断出没有设备，则*pNoDevCnt加1
        if (!if_exist_ttyacm_dev())
        {
            sleep(1);
            if (!if_exist_ttyacm_dev())
            {
                sleep(1);
                if (!if_exist_ttyacm_dev())
                {
                    (*pNoDevCnt)++;
                    SELFCHK_FMT_DEBUG("detected no dev  noDevCnt[%d]\n", (*pNoDevCnt));
                }
                else
                {
                    (*pNoDevCnt) = 0;
                }
            }
            else
            {
                (*pNoDevCnt) = 0;
            }
        }
        else
        {
            (*pNoDevCnt) = 0;
        }

        //连续3分钟测出出没有hub设备，则重启hub
        if (*pNoDevCnt > 2)
        {
            (*pNoDevCnt) = 0;
            SELFCHK_FMT_DEBUG("reboot hub  noDevCnt[%d]\n", (*pNoDevCnt));
            reboot_hub_proc();
        }
    }
}
#endif

#if TID_CONTAINER == 1
/*******************************************************************************
* 函数名称: start_container_proc
* 函数功能: 重启容器
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void start_container_proc(char* strContainerName)
{
    char   cmd[128] = { 0 };

    if (strContainerName == NULL)
    {
        SELFCHK_FMT_DEBUG("if_xxcontainer_exit strContainerName[%p]\n", strContainerName);
        return;
    }

    sprintf(cmd, "container start %s", strContainerName);
    system(cmd);

    return;
}
/*******************************************************************************
* 函数名称: if_container_exit
* 函数功能: hub自检测程序
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
BOOL if_container_exit(char* strContainerName)
{
    FILE* fp = NULL;
    char   cmd[128] = { 0 };
    char   buf[512] = { 0 };
    char* str = NULL;

    if (strContainerName == NULL)
    {
        SELFCHK_FMT_DEBUG("if_container_exit strContainerName[%p]\n", strContainerName);
        return TRUE;
    }

    sprintf(cmd, "docker ps | grep %s", strContainerName);
    fp = popen(cmd, "r");
    if (NULL == fp)
    {
        SELFCHK_FMT_DEBUG("%s_scan:popen error\n", strContainerName);
        return TRUE;
    }

    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        SELFCHK_FMT_TRACE("scan %s buf:%s\n", strContainerName, buf);
        if ((str = strstr(buf, strContainerName)) != NULL)
        {
            fclose(fp);
            return TRUE;
        }
    }

    fclose(fp);

    sprintf(cmd, "cat /home/sysadm/container/container_cfg.yaml");
    fp = popen(cmd, "r");
    if (NULL == fp)
    {
        SELFCHK_FMT_DEBUG("%s_scan:popen error\n", strContainerName);
        return TRUE;
    }

    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        SELFCHK_FMT_TRACE("scan %s_cfg buf:%s\n", strContainerName, buf);
        if ((str = strstr(buf, strContainerName)) != NULL)
        {
            fclose(fp);
            return TRUE;
        }
    }

    fclose(fp);

    return FALSE;
}

/*******************************************************************************
* 函数名称: proc_container_chk
* 函数功能: hub自检测程序
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void proc_container_chk(char* strContainerName, struct timespec curtime, struct timespec oldtime, uint32* pNoContainerCnt)
{
    if ((strContainerName == NULL) || (pNoContainerCnt == NULL))
    {
        SELFCHK_FMT_DEBUG("input err, strContainerName[%p], pNoContainerCnt[%p]\n", strContainerName, pNoContainerCnt);
        return;
    }

    if ((curtime.tv_sec - oldtime.tv_sec > 60) && ((*pNoContainerCnt) < 10))
    {
        //仅上电10分钟内检测检
        (*pNoContainerCnt)++;

        if (0 != access("/system/etc/init_finsh", F_OK))
        {
            SELFCHK_FMT_TRACE("/system/etc/init_finsh inexistence\n");
            return;
        }

        //连续三次判断出没有容器
        if (!if_container_exit(strContainerName))
        {
            sleep(1);
            if (!if_container_exit(strContainerName))
            {
                sleep(1);
                if (!if_container_exit(strContainerName))
                {
                    SELFCHK_FMT_DEBUG("start %s noContainerCnt[%d]\n", strContainerName, (*pNoContainerCnt));
                    start_container_proc(strContainerName);
                }
            }
        }
    }

    return;
}

#endif

/*******************************************************************************
* 函数名称: proc_slef_chk
* 函数功能: 自检测
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void proc_slef_chk(uint32* cntchk)
{
    struct timespec curtime = { 0 };
    struct timespec oldtime = { 0 };
    uint32 noDevCnt = 0;

#if TID_CONTAINER == 1
    uint8  ifStamr = 0;
    uint32 noBaseContainerCnt = 0;
    uint32 noAmrContainerCnt = 0;
    uint32 noEdgerContainerCnt = 0;
    uint32 noPdContainerCnt = 0;
#endif

#if TID_CONTAINER_APP == 1
    uint32 appCheckCnt[CONTAINER_APP_NUM_MAX] = { 0 };
    uint32 i = 0;
#endif

#if TID_CONTAINER == 1
    if (access("/data/app/desktopGui/terminaltype", F_OK) == 0)
    {
        ifStamr = 1;
    }
    else
    {
        ifStamr = 0;
    }
#endif

    clock_gettime(CLOCK_MONOTONIC, &oldtime);

    while (1)
    {
        *cntchk = 1;	//每隔一秒就置线程标记为存活状态

        clock_gettime(CLOCK_MONOTONIC, &curtime);

#if TID_HUB == 1
        proc_hub_chk(curtime, oldtime, &noDevCnt);
#endif

#if TID_CONTAINER == 1
        proc_container_chk("baseContainer", curtime, oldtime, &noBaseContainerCnt);
        proc_container_chk("amrContainer", curtime, oldtime, &noAmrContainerCnt);
        proc_container_chk("edgerContainer", curtime, oldtime, &noEdgerContainerCnt);
        if (ifStamr == 0)
        {
            proc_container_chk("pdContainer", curtime, oldtime, &noPdContainerCnt);
        }
#endif

#if TID_CONTAINER_APP == 1
        for (i = 0; i < appCheckNum; i++)
        {
            if (i > CONTAINER_APP_NUM_MAX)
            {
                SELFCHK_FMT_DEBUG("CONTAINER_APP_NUM_MAX [%d]. appCheckNum[%d]\n", CONTAINER_APP_NUM_MAX, appCheckNum);
                break;
            }

            //专变不需要检测配电APP
            if ((ifStamr == 1) && (strcmp(appCheck[i].appName, "pdAmr") == 0))
            {
                continue;
            }
            proc_container_app_chk(appCheck[i].appName, curtime, oldtime, &appCheckCnt[i]);
        }
#endif

        if (curtime.tv_sec - oldtime.tv_sec > 60)
        {
            oldtime = curtime;
        }

        sleep(1);
    }

    return;
}

/*********************************************************************
 功能描述：对时线程
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void* pthread_self_chk(void* arg)
{
    INIT_FMT_DEBUG("pthread_self_chk start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T* tArg = (THREAD_ARG_T*)arg;

    //线程名
    prctl(PR_SET_NAME, (char*)&tArg->threadName[0]);

    tArg->cnt = 0;

    proc_slef_chk(&tArg->cnt);

    pthread_exit(NULL);
}
#endif


