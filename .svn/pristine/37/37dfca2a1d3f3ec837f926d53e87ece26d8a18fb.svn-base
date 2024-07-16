/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 数据中心入口程序
* @date： 2019-11-9
*********************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <syslog.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <semaphore.h>

#include "dataType.h"

#include "hal.h"
#include "devinfo.h"
#include "constant.h"
#include "dbCenterConfig.h"
#include "globalConfiguration.h"
#include "dbCenterModule.h"
#include "optparse.h"
#include "pidfile.h"
#include "olog.h"

uint8 g_app_id[25] = {0};
//信号量
static sem_t s_signal_sem;

//日志输出
void *     db_ocat;
void *     timeout_ocat;
void *     db_err;
void *     db_recovery;
void *     db_record_status;
void *     db_ini;

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
    if (signum == SIGUSR1) //信号为SIGUSR1时重载日志配置 搭配crontab+logrotate 进行日志管理
    {
        if (olog_reload(g_configObj.dbCfg.logFile))
        {
            syslog(LOG_ERR, "zlog_reload fail...");
            olog_error(db_ocat,"zlog_reload fail...");
        }
        else
        {
            syslog(LOG_INFO, "zlog_reload succ...");
            olog_info(db_ocat,"zlog_reload succ...");
        }
        return;
    }

    syslog(LOG_INFO, "Shut down signal is %d", signum);
    olog_info(db_ocat,"shut down signal is %d, sender PID:%d", signum, sigInfo->si_pid);
    olog_info(db_ocat,"sem_post(&s_signal_sem)");
    sem_post(&s_signal_sem);
}

/* @brief: opt类型参数表  */
struct optparse_long longopts[] =
    {
        {"daemon", 'd', OPTPARSE_NONE},         //程序以守护进程形式运行
        {"config", 'c', OPTPARSE_REQUIRED},     //设置配置文件路径
        {"version", 'v', OPTPARSE_NONE},        //打印当前程序版本信息
        {"log-config", 'l', OPTPARSE_REQUIRED}, //设置ZLOG配置文件路径
        {0},                                    //结尾补充空对象防止越界
};

sint32 get_app_id()
{
    int iRet = 0;
    memset(g_app_id, 0, 25);

    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*)hal_device_get(HW_DEVICE_ID_DEVINFO);
	if(NULL == dev)
    {
        return -1;
    }
//	dev->devinfo_read_appid(SERVICE_BASE_NAME, g_app_id, 25);
	iRet = dev->devinfo_read_appid(dev, SERVICE_BASE_NAME, (char*)g_app_id, 24);

    
	hal_device_release((HW_DEVICE *)dev);

	return iRet;
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
//    sint32 nice_ret = 0;
//    sint32 ioprio_set_ret = 0;
    sint32 daemonFlag = 0;
    sint32 option = 0;
    struct optparse options = {0};
    struct sigaction act = {}; //Linux 信号对象初始化

    /*gdb测试*/
	if(argc >= 2)
    {
        if(0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            printf("30s waiting...\n");
            sleep(30);
        }
    }

    optparse_init(&options, argv); //opt 命令行参数初始化
    while ((option = optparse_long(&options, longopts, NULL)) != -1)
    {
        switch (option)
        {
        case 'd':
            daemonFlag = 1; //daemon 方式运行标志位
            break;
        case 'c':
            strncpy(g_configObj.dbCfg.cfgFile, options.optarg, strlen(g_configObj.dbCfg.cfgFile));
            //命令行指定dbCenter配置文件路径
            break;
        case 'l':
            strncpy(g_configObj.dbCfg.logFile, options.optarg, strlen(g_configObj.dbCfg.logFile));
            //命令行指定dbCenter 日志配置文件路径
            break;
        case 'v':
            fprintf(stdout, "dbCenter Version: %s Time: %s Build: %s %s", APP_VERSION, APP_PUBLISH_TIME, __DATE__, __TIME__); //输出版本信息
            exit(EXIT_SUCCESS);
            break;
        case '?':
            fprintf(stderr, "%s: %s\n", argv[0], options.errmsg); //错误opt选项处理
            exit(EXIT_FAILURE);
            break;
        default:
            fprintf(stderr, "opt parser fail"); //opt库内部异常
            exit(EXIT_FAILURE);
            break;
        }
    }

    if (1 == daemonFlag)
    {
        if (-1 == daemon(0, 0)) //守护进程方式运行
        {
            fprintf(stderr, "dbCenter run as daemon fail\n"); //日志系统未初始化 输出至stderr
        }

//#if 1
//        if (0 != set_pid_file(PID_PATH))  //设置独占PID文件 程序单列运行
//        {
//            fprintf(stderr,
//                    "Set pid fail. PATH:[%s].\n"
//                    "The pid file lock is locked\n",
//                    PID_PATH);
//            exit(EXIT_FAILURE);
//        }
//#endif

    }

	if (0 != set_pid_file(PID_PATH))  //设置独占PID文件 程序单列运行
    {
        fprintf(stderr,
                "Set pid fail. PATH:[%s].\n"
                "The pid file lock is locked\n",
                PID_PATH);
        exit(EXIT_FAILURE);
    }

    act.sa_flags = SA_SIGINFO;         //当设定了该标志位时，表示信号附带的参数可以被传递到信号处理函数中
    act.sa_sigaction = signal_handler; //为信号对象绑定回调函数
    sigaction(SIGTERM, &act, NULL);    //注册预留多组信号的回调 默认终止程序
    sigaction(SIGHUP, &act, NULL);     //默认终止程序
    sigaction(SIGINT, &act, NULL);     //默认终止程序
    sigaction(SIGUSR1, &act, NULL);    //SIGUSR1用于处理日志重载
    sigaction(SIGUSR2, &act, NULL);    //默认终止程序

    openlog("dbCenter:", LOG_PID | LOG_CONS | LOG_NDELAY, LOG_LOCAL1); //初始化系统日志
    syslog(LOG_INFO, "Starting dbCenter service...");

    //检查日志目录
    if (0 != dir_init())
    {
        syslog(LOG_ERR, "dir_init error");
        printf("dbCenter dir_init error\n");
        exit(EXIT_FAILURE);
    }

    //初始化ZLOG日志 优先级1 /etc/dbCenter/dbCenterLog.con 优先级2默认配置
    if (0 != dolog_init(g_configObj.dbCfg.logFile, ZLOG_CAT_NAME))
    {
        fprintf(stderr, "dbCenter load [%s] fail.\n", g_configObj.dbCfg.logFile);
        syslog(LOG_ERR, "dbCenter load [%s] fail.", g_configObj.dbCfg.logFile);
    }

	printf("Starting dbCenter service...\n");
    dolog_info("Starting dbCenter service...");
    dolog_info("Version: %s Time: %s Build: %s %s ", APP_VERSION, APP_PUBLISH_TIME, __DATE__, __TIME__);
    db_ocat = olog_get_category("db");
    timeout_ocat = olog_get_category("to");
    db_err = olog_get_category("dbState");
    db_recovery  = olog_get_category("dbRecovery");
    db_record_status = olog_get_category("dbStatus");
	db_ini = olog_get_category("dbIni");

	olog_info(db_ini, "Starting dbCenter service...");
    olog_info(db_ini, "Version: %s Time: %s Build: %s %s ", APP_VERSION, APP_PUBLISH_TIME, __DATE__, __TIME__);

    //db_ser_ocat = olog_get_category("db_ser");

    hal_init();
    int ret = get_app_id();     // 获取APPID
    dolog_debug("get_app_id ret%d...", ret);
    holog_debug(db_ocat, g_app_id, 24);
/*
    nice_ret = nice(-20);
    printf("nice_ret:%d\n", nice_ret);
    if (nice_ret != -20)
    {
        printf("dbCenter Nice Number set fail\n");
        olog_error(db_ocat, "dbCenter Nice Number set fail");
    }
    else
    {
        printf("dbCenter Nice Number set :%d\n", nice_ret);
        olog_info(db_ocat, "dbCenter Nice Number set :%d", nice_ret);
    }

    ioprio_set_ret = syscall(SYS_ioprio_set, 1, 0, IOPRIO_PRIO_VALUE(IOPRIO_CLASS_RT, 0));
    if (ioprio_set_ret)
    {
        printf("dbCenter IONice  ioprio_set_ret fail :%d\n", ioprio_set_ret);
        olog_info(db_ocat, "dbCenter IONice  ioprio_set_ret fail :%d\n", ioprio_set_ret);
    }
    else
    {
        printf("dbCenter IONice  ioprio_set_ret :%d\n", ioprio_set_ret);
        olog_info(db_ocat, "dbCenter IONice  ioprio_set_ret :%d\n", ioprio_set_ret);
    }
*/
    //全局配置文件初始化
    if (0 != global_config_init(g_configObj.dbCfg.cfgFile))
    {
        olog_error(db_ocat,"global_config_init fail path:[%s]", g_configObj.dbCfg.cfgFile);
        exit(EXIT_FAILURE);
    }

    if (-1 == sem_init(&s_signal_sem, 0, 0))
    {
        olog_error(db_ocat, "db_ocat sem_init fail [%s]", strerror(errno));
        exit(EXIT_FAILURE);
    }


#ifdef GLOBAL_CONFIG_DUMP
    global_config_dump(); //输出全局配置文件参数
#endif

    if (0 != init_system()) //初始化系统各子模块
    {
        syslog(LOG_ERR, "init system fail");
        exit(EXIT_FAILURE);
    }

    if (0 != start_system()) //初始化系统各子模块
    {
        syslog(LOG_ERR, "start system fail");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        olog_info(db_ocat, "dbCenter main thread brocking in s_signel_sem");
        int sem_ret = sem_wait(&s_signal_sem);
        olog_info(db_ocat, "dbCenter main thread brocking out s_signel_sem sem_ret=%d",sem_ret);
        if(sem_ret == 0)
        {
            break;
        }
    }
    
    syslog(LOG_INFO, "Shutting down dbCenter service...");
    olog_info(db_ocat,"Shutting down dbCenter service...");
    de_init_system(); //反向释放及析构程序所占内外部资源，刷新数据库
    olog_info(db_ocat,"start sync...");
    system("sync");
    olog_info(db_ocat,"Sync done");
    syslog(LOG_INFO, "Shut down dbCenter service successfully");
    olog_info(db_ocat,"dbCenter is shut down!");
    olog_fini();
    closelog(); //关闭系统日志及ZLOG日志
    exit(EXIT_SUCCESS);
}
