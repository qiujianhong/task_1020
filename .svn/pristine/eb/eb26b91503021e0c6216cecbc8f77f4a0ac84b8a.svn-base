/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 系统管理器入口程序
* @date： 2019-11-9
*********************************************************************
*/

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>

#include "smiOSConfig.h"

#include "baseType.h"
#include "framecomm.h"
#include "constant.h"
#include "globalConfiguration.h"
#include "optparse.h"
#include "pidfile.h"
#include "smiOSModule.h"
#include "hal.h"
#include "rtc.h"
#include "cJSON.h"
#include "trace_back.h"
#include "msgHandler.h"
#include "gui_c_prtl.h"
#include "devinfo.h"
#include "msgService.h"
#include "systemCtrl.h"

//smios配置路径
static const sint8 *s_configFilePath    = CFG_PATH;

 char APPID[APPID_LEN]={0};
 uint32 APPIDLen=APPID_LEN;

/**
*********************************************************************
* @brief： Termination handler
* @param[in]：
               signum      UNIX 信号编号
               sigInfo     信号属性结构对象
               context     回调参数
* @param[out]：
               none
* @return： none
*********************************************************************
*/
static void signal_handler(sint32 signum, siginfo_t *sigInfo, void *context)
{
    UNUSED(sigInfo);
    UNUSED(context);
    if (signum == SIGUSR1)  //信号为SIGUSR1时重载日志配置 搭配crontab+logrotate
                            //进行日志管理
    {
        //日志配置信息加载
        diag_init((char*)s_configFilePath, TRUE);

        return;
    }
                            
    INIT_FMT_DEBUG("shut down signal is %d, sender PID:%d\n", signum,
               sigInfo->si_pid);
    de_init_system();  //反向释放及析构程序所占内外部资源，刷新数据库
    INIT_FMT_DEBUG("smiOS is shut down!\n");
    exit(EXIT_SUCCESS);
}

/* @brief: opt类型参数表  */
struct optparse_long longopts[] = {
    {"daemon", 'd', OPTPARSE_NONE},          //程序以守护进程形式运行
    {"config", 'c', OPTPARSE_REQUIRED},      //设置配置文件路径
    {"version", 'v', OPTPARSE_NONE},         //打印当前程序版本信息
    {"log-config", 'l', OPTPARSE_REQUIRED},  //设置OLOG配置文件路径
    {0},                                     //结尾补充空对象防止越界
};


/**
*********************************************************************
* @brief： 软硬时钟同步
* @param[in]：
                无

* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
*********************************************************************
*/
sint32 chk_systime_with_rtc(void)
{
    int iRet = 0;
    struct tm rtc;

#if (DISABLE_HAL == 0)
    RTC_DEVICE_T* dev = (RTC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_RTC);
    if(NULL == dev)
    {
        return -1;
    }
    
    iRet = dev->rtc_time_get(dev, &rtc);
    if (iRet >= 0)
    {
        if(rtc.tm_year < 70 || rtc.tm_year > 138)
        {
            INIT_FMT_DEBUG("上电同步时钟错误: %d-%d-%d %d:%d:%d, 改成120\n", rtc.tm_year, rtc.tm_mon, rtc.tm_mday, 
                                                                        rtc.tm_hour, rtc.tm_min, rtc.tm_sec);
            rtc.tm_year = 120;
        }
        
        iRet = dev->rtc_time_set(dev, &rtc);
    }
    else
    {
        INIT_FMT_DEBUG("获取时钟失败\n");
    }
    
    hal_device_release((HW_DEVICE *)dev);
#endif
    return iRet;
}

/**
*********************************************************************
* @brief： 本机ip配置
* @param[in]： void
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
*********************************************************************
*/
sint32 local_ip_info_parse(void)
{
    int ret = 0;

    memset(&setIpInfo, 0, sizeof(SET_IP_INFO_T));
    ret = load_ip_info(&setIpInfo);
    
    if(1 == (setIpInfo.mac[0] & 0x01) || 
      (0 == setIpInfo.mac[0] && 0 == setIpInfo.mac[1] && 0 == setIpInfo.mac[2] &&  
       0 == setIpInfo.mac[3] && 0 == setIpInfo.mac[4] && 0 == setIpInfo.mac[5]))
    {
        /* MAC地址低二位数字是偶数, 第32位是1表示组播地址, 必须是0 */
        uint8 i = 0;
        struct  timeval tv;
        struct  timezone tz;
        /* 随机一个MAC地址 */
        gettimeofday(&tv, &tz);
        srand(tv.tv_usec);
        setIpInfo.mac[i] = (rand() % 128) * 2;
        if(0 == setIpInfo.mac[i] || 255 == setIpInfo.mac[i])
        {
            setIpInfo.mac[i] = 0x8E;
        }
        for(i = 1; i < 6; i++)
        {
            gettimeofday(&tv, &tz);
            srand(tv.tv_usec + i);
            setIpInfo.mac[i] = rand() % 255;
        }
        local_mac_info_save();
    }
    SMIOS_FMT_DEBUG("Init Mac %02X:%02X:%02X:%02X:%02X:%02X\n", setIpInfo.mac[0], setIpInfo.mac[1], setIpInfo.mac[2],
                                                       setIpInfo.mac[3], setIpInfo.mac[4], setIpInfo.mac[5]);
    memcpy(setIpInfo.nodeName, ETH_FIRST_NAME, strlen(ETH_FIRST_NAME));
    act_set_mac();
    sleep(1);
    if(0 == ret)
    {
        act_set_ip(FALSE);
    }
    return 0;
}

/**
*********************************************************************
* @brief： get_app_id 获取系统管理器appid
* @param[in]：
               argc      参数个数
               argv[]    参数列表
* @param[out]：
               none        出参说明
* @return： 返回appid长度
*********************************************************************
*/
int get_app_id(uint8* appid,int maxlen)
{
    int appidlen;
    const char* appname = "smiOS";
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        INIT_FMT_DEBUG("dev is null!\n");
        appidlen = 24;
        memset(appid,0x00,appidlen);
        
    }else
    {
        INIT_FMT_DEBUG("get_app_id! \n");
        if(dev->devinfo_read_appid(dev,appname,(char*)appid,maxlen) < 0)
        {
            INIT_FMT_DEBUG("get_app_id failed!\n");
            appidlen = 24;
            memset(appid,0x00,appidlen);
        }else
        {
            appidlen = 24;
        }

        hal_device_release((HW_DEVICE *)dev);
    }
    
    return appidlen;
}
/**
*********************************************************************
* @brief： main函数入口
* @param[in]：
               argc      参数个数
               argv[]    参数列表
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
*********************************************************************
*/
sint32 main(sint32 argc, sint8 *argv[])
{
    sint32          daemonFlag = 0;
    sint32          option     = 0;
    struct optparse options    = {0};
    struct sigaction act;  // Linux 信号对象初始化
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
    const char* appName = "smiOS";
    const char* iconFile = "/usr/local/extapps/smiOS/bin/cfg/smiOS.bmp";
    APP_REGIST_INFO_T  appRegInfo;      //登录到桌面显示
    time_t   LastLoginGUI = 0;
    struct timespec curtime = {0};
//#endif
    memset(&act, 0, sizeof(struct sigaction));
    //判断smios是否已经启动
    if(!singleton_check((char*)"smiOS"))
    {
        return -1;
    }

    if(argc >= 2)
    {
        if(0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            printf("wait 30s\n");
            sleep(30);
        }
    }

    //程序异常监控
    init_sig_action("/data/app/smiOS/log/Exception.log");
    hal_init();
    diag_init((char*)s_configFilePath, TRUE);
    chk_systime_with_rtc();     // 上电通过RTC校准时钟
//    APPIDLen = get_app_id((uint8*)APPID,APPID_LEN);

    /* 日志模块初始化 */
    
    smios_set_starttime();

    UNUSED(argc);
    optparse_init(&options, argv);  // opt 命令行参数初始化
    while ((option = optparse_long(&options, longopts, NULL)) != -1)
    {
        switch (option)
        {
            case 'd':
                daemonFlag = 1;  // daemon 方式运行标志位
                break;
            case 'c':
                s_configFilePath =
                    options.optarg;  //命令行指定smiOS配置文件路径
                break;
            case 'v':
                INIT_FMT_DEBUG("smiOS Version:\t%d.%d.%d.%d\n",
                        smiOS_VERSION_MAJOR, smiOS_VERSION_MINOR,
                        smiOS_VERSION_PATCH, smiOS_VERSION_SVN);  //输出版本信息
                exit(EXIT_SUCCESS);
                break;
            case '?':
                INIT_FMT_DEBUG("%s: %s\n", argv[0],
                        options.errmsg);  //错误opt选项处理
                exit(EXIT_FAILURE);
                break;
            default:
                INIT_FMT_DEBUG("opt parser fail\n");  // opt库内部异常
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (1 == daemonFlag)
    {
        if (-1 == daemon(0, 0))  //守护进程方式运行
        {
            INIT_FMT_DEBUG("smiOS run as daemon fail\n");  //日志系统未初始化 输出至stderr
        }
    }
    INIT_FMT_DEBUG("Starting smiOS service...\n");
    act.sa_flags = SA_SIGINFO;  //当设定了该标志位时，表示信号附带的参数可以被传递到信号处理函数中
    act.sa_sigaction = signal_handler;  //为信号对象绑定回调函数
    sigaction(SIGTERM, &act, NULL);  //注册预留多组信号的回调 默认终止程序
    sigaction(SIGHUP, &act, NULL);   //默认终止程序
    sigaction(SIGINT, &act, NULL);   //默认终止程序
    sigaction(SIGUSR1, &act, NULL);  // SIGUSR1用于处理日志重载
    sigaction(SIGUSR2, &act, NULL);  //默认终止程序

    INIT_FMT_DEBUG("Starting smiOS service...\n");

    //全局配置文件初始化
    if (0 != global_config_init(s_configFilePath))
    {
        INIT_FMT_DEBUG("global_config_init fail path:[%s]\n", s_configFilePath);
        exit(EXIT_FAILURE);
    }
    INIT_FMT_DEBUG("Starting smiOS service...\n");

    local_ip_info_parse();      //上电配置本地ip

#ifdef GLOBAL_CONFIG_DUMP
    global_config_dump();  //输出全局配置文件参数
#endif

    if (0 != init_system())  //初始化系统各子模块
    {
        INIT_FMT_DEBUG("init system fail\n");
        exit(EXIT_FAILURE);
    }

    if (0 != start_system())  //初始化系统各子模块
    {
        INIT_FMT_DEBUG("start system fail\n");
        exit(EXIT_FAILURE);
    }
    
    //设置进程优先级
    nice(-20);
    while (1)
    {
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        if(getLoginguiID() == 0 && curtime.tv_sec - LastLoginGUI > 3)
        {
            printf("向桌面GUI发送注册请求!,getLoginguiID()= %d\n",getLoginguiID());
            memset(&appRegInfo,0x0,sizeof(APP_REGIST_INFO_T));
            memcpy(appRegInfo.appname,appName,strlen(appName));
            memcpy(appRegInfo.shortname, ADV_SHOW_NAME, strlen(ADV_SHOW_NAME));
            memcpy(appRegInfo.iconFileName,iconFile,strlen(iconFile));
            appGUI_send_req_handle(0x0007, 0x0010, &appRegInfo, sizeof(APP_REGIST_INFO_T)); //注册到桌面显示
            LastLoginGUI = curtime.tv_sec;
        }
//#endif
        sleep(1000);  //主进程持续运行此 程序逻辑代码运行与各子模块
    }

    exit(EXIT_SUCCESS);
}
