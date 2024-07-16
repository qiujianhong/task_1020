/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级APP用采进程
* @date：    2019-12-23
* @history： 
*********************************************************************
*/
#include "pageInterface.h"
#include "trace_back.h"
#include "oopVarUpdate.h"
#include "hal.h"
#include "baseCommon.h"
#include <net/if.h>	
#include "iniparser.h"
#include "task_msg.h"
#include "framecomm.h"
#include "comm.h"
#include <sys/prctl.h>

extern FACTORY_VER_TMP_T g_tVer;

extern PLUGIN_INFO* g_plugin[];
extern int g_nPluginCnt;

uint32 g_maindbip = 0;
uint16 g_maindbport = 0;
uint8 gdelay = 0;
/* 线程判断 */
typedef int (*TASK_ENTRY)(uint32, uint16);
typedef void *(*EntryFun)(void *);
typedef struct tagMainThread
{
    pthread_t          nThreadId;   /* 线程ID */
    EntryFun           pEntryFunc;  /* 线程入口函数 */
    TASK_ENTRY         pTaskEntry;
    char               taskname[20];
    uint32             restartFlag; 
}MAIN_TRHEAD_T;

extern int msg_task_start(uint32 ip, uint16 port);

extern void pulse_get_config(dictionary *ini);
extern int pulse_task_entry(uint32 dbip, uint16 dbport);
extern int comm_task_entry(uint32 dbip, uint16 dbport);
extern int appmain_task_entry(uint32 dbip, uint16 dbport);
extern int acport_task_entry(uint32 dbip, uint16 dbport);
extern int statis_task_entry(uint32 dbip, uint16 dbport);
extern int hotplug_detect_entry(uint32 dbip, uint16 dbport);
#if AREA_FUJIAN
extern int app_update_entry(uint32 dbip, uint16 dbport);
#endif
extern void* advance_gui_start(void* pArg);

extern void comm_set_ethname(uint8 ifno, char *ifname);
extern void app_set_apdu_default_len(uint16 len);
extern void app_comm_proxy_set(BOOLEAN isUse);
extern void app_secure_defend_set(BOOLEAN isUse);
extern void app_ctrl_init(uint8 port, uint8 turnMax);
extern BOOLEAN advance_start_goon();

void *main_start_task(void *pArg);

MAIN_TRHEAD_T g_tMainThread[] = 
{
    {0, main_start_task, comm_task_entry,   "comm", 1},
    {0, main_start_task, appmain_task_entry,"appamin", 1},
    //{0, main_start_task, acport_task_entry, "acport", 1},
    {0, main_start_task, pulse_task_entry,  "pulse", 1},
    {0, main_start_task, statis_task_entry, "statis", 1},
    {0, main_start_task, hotplug_detect_entry, "hotplug", 1},
    #if AREA_FUJIAN
    {0, main_start_task, app_update_entry, "update", 1},
    #endif
};
uint8 g_MainTaskNum = sizeof(g_tMainThread)/sizeof(MAIN_TRHEAD_T);

/**********************************************************************
* @name      : main_start_task
* @brief     ：启动task任务
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-24
* @Update    :
**********************************************************************/
void *main_start_task(void *pArg)
{
    int ret;
    MAIN_TRHEAD_T *pHandle = (MAIN_TRHEAD_T *)pArg;

    printf("main start task %s\n", pHandle->taskname);
    pHandle->restartFlag = 0;
	prctl(PR_SET_NAME,pHandle->taskname);
    ret = pHandle->pTaskEntry(g_maindbip, g_maindbport);
    printf("\n\nmain task %s stop, code %d!\n\n", pHandle->taskname, ret);
    sleep(20);
    pHandle->restartFlag = 1;
    sleep(20);
    return 0;
}

/**********************************************************************
* @name      : get_ifIP
* @brief     ：获取网口的IP地址
* @param[in] ：char *ifname 网口名称
* @param[out]：
* @return    ：IP地址
* @Create    : 王津剑
* @Date      ：2020-2-26
* @Update    :
**********************************************************************/
uint32 get_ifIP(char *ifname)
{ 
    register int fd, intrface;
    struct ifreq buf[12]; 
    struct ifconf ifc; 
    uint32 ipAddr = 0;

    if(NULL == ifname)
    {
        return 0;
    }
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) 
    { 
        ifc.ifc_len = sizeof(buf); 
        ifc.ifc_buf = (caddr_t) buf; 

        if (!ioctl(fd, SIOCGIFCONF, (char *) &ifc)) 
        {
            /*获取接口数量*/
            intrface = ifc.ifc_len / sizeof (struct ifreq);

            /*查找PPP位置*/                       
            while ( (strcmp(buf[intrface].ifr_name, ifname)) && (intrface >= 0) )
                intrface--;

            if (intrface >= 0)
            {
                /*获取当前网卡的IP地址*/ 
                if (!(ioctl (fd, SIOCGIFADDR, (char *)&buf[intrface]))) 
                {
                    ipAddr = ((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr.s_addr;
                    if (0 != ipAddr)
                    {
                        close(fd);
                        return ntohl(ipAddr);
                    }
                }           
            }
        } 
        else 
            perror ("ioctl error"); 

        close(fd);
    }
    return 0;
}

/**********************************************************************
* @name      : init_config
* @brief     ：初始化配置文件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-2-27
* @Update    : 
**********************************************************************/
int app_yc_init_config(uint32 *ip, uint16 *port)
{
    dictionary *ini = NULL;

    uint32 ipaddr = 0;
    char  *stringvalue = NULL;
    int    intvalue = NULL;

    char   curpath[256] = {0};
    char   filename[256] = {0};
    int i;

    //获取当前目录绝对路径，即去掉程序名
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("wireless get err path!\n");
        YCMSG_FMT_DEBUG("wireless get err path!");
    }
    else
    {
        for (i = cnt; i >=0; --i)
        {
            if (curpath[i] == '/')
            {
                curpath[i+1] = '\0';
                break;
            }
        }
    }
    sprintf(filename, "%s%s", curpath, ADV_INI_NAME);
    printf("filename is %s!\n", filename);

    
    ini = iniparser_load(filename);
    
    /* 日志模块初始化 */
    diag_init(filename, TRUE);
    
    if(NULL != ini)
    {
        /* ip获取 */
        stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "172.17.0.1");
        ipaddr = inet_addr(stringvalue);
        ipaddr = ntohl(ipaddr);
        *ip = ipaddr;

        /* 端口获取 */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        *port = (uint16)intvalue;
        
        /* 获取ipc路径 */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:IPCPATH", "/mnt/ipc_socket");
        task_msg_prepare("/mnt/ipc_socket");

        /* 获取log路径 */
#ifdef ADV_ST_AMR
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:LOGPATH", "/data/app/stAmr/log");
#else
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:LOGPATH", "/data/app/puAmr/log");
#endif
        char   logpath[256] = {0};
        sprintf(logpath, "%s/Exception.log", stringvalue);
        change_sig_file(logpath);

        /* 获取以太网口名称 */
        stringvalue = iniparser_getstring(ini, "ETH_CONF:ETH1_NAME", "eth0");
        if(NULL != stringvalue)
        {
            comm_set_ethname(0, stringvalue);
        }
        stringvalue = iniparser_getstring(ini, "ETH_CONF:ETH2_NAME", "eth1");
        if(NULL != stringvalue)
        {
            comm_set_ethname(1, stringvalue);
        }

        /* 获取运行参数 */
        intvalue = iniparser_getint(ini, "RUN_CONF:APDU_SPLIT", 1400);
        app_set_apdu_default_len((uint16)intvalue);

        intvalue = iniparser_getint(ini, "RUN_CONF:COMM_PROXY", FALSE);
        app_comm_proxy_set((BOOLEAN)intvalue);

        intvalue = iniparser_getint(ini, "RUN_CONF:SECURE_DEFEND", FALSE);
        app_secure_defend_set((BOOLEAN)intvalue);

        stringvalue = iniparser_getstring(ini, "RUN_CONF:UPDATE_VERSION", NULL);
        if (NULL != stringvalue)
        {
            app_update_version_set(stringvalue);
        }

#ifdef ADV_ST_AMR
        /* 获取控制参数 */
        int turnmax = iniparser_getint(ini, "CTRL_CONF:TURNMAX", 8);
        int portturn = iniparser_getint(ini, "CTRL_CONF:MODETURN", 4);
        app_ctrl_init((uint8)portturn, (uint8)turnmax);
#endif

        intvalue = iniparser_getint(ini, "RUN_CONF:BLE_COMM", TRUE);
        set_startup_flag_ble((BOOL)intvalue);

        intvalue = iniparser_getint(ini, "RUN_CONF:RS485_COMM", TRUE);
        set_startup_flag_RS485((BOOL)intvalue);

        intvalue = iniparser_getint(ini, "RUN_CONF:BLE_PIN_EN", FALSE);
        set_pin_flag_ble((BOOL)intvalue);

        pulse_get_config(ini);
        iniparser_freedict(ini);
    }
    else
    {
        printf("Get no ini file %s!\n", ADV_INI_NAME);

        *ip = 0x7f000001;
        *port = 1883;
        task_msg_prepare("/mnt/ipc_socket");
    }

    /* 文件传输升级路径创建 */
    system("mkdir -p /data/app/download");
    system("mkdir -p /update/download");
    system("mkdir -p /data/log");
    return 0;
}

/**********************************************************************
* @name      : app_main_task
* @brief     ：线程维护
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-15
* @Update    : 
**********************************************************************/
void* app_main_task(void* pArg)
{
    uint8 i;
    int errCode;
    MAIN_TRHEAD_T *pthread = NULL;

    /* 等待数据中心启动 */
    while(TRUE != advance_start_goon())
    {
        sleep(1);
    }

    /* 启动task */
    while(1)
    {
        for(i = 0; i < g_MainTaskNum; i++)
        {
            pthread = &(g_tMainThread[i]);
            if(1 == pthread->restartFlag)
            {
                if(0 != pthread->nThreadId)
                {
                    pthread_cancel(pthread->nThreadId);
                    pthread_join(pthread->nThreadId, NULL);
                    pthread->nThreadId = 0;
                }
                errCode = pthread_create(&pthread->nThreadId, NULL, pthread->pEntryFunc, pthread);
                if (errCode != 0)
                {
                    printf("Task %s Create error! err:%d\n", pthread->taskname, errCode);
                    pthread->nThreadId = 0;
                }
            }
        }
        sleep(2);
    }
    return 0;
}


/*-----------------------------Main-------------------------------*/
int main(int argc, char** argv)
{
    int       errCode = 0;
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
    pthread_t thidGui = 0;
//#endif
    pthread_t thidMana = 0;
    uint32    mqttip = 0;
    uint16    mqttport = 0;

    if(argc >= 2)
    {
        if(0 == memcmp(argv[1], "version", strlen("version")))
        {
            printf("Version: %s\nTime: %s\nPlace: %s\n", APP_VERSION, APP_PUBLISH_TIME, AREANAME_PLACE);
            return 0;
        }
    }

    /* 检查程序只启动了一个副本 */
    if(!singleton_check(ADV_APP_NAME))
    {
        return -1;
    }

    if(argc >= 2)
    {
        if(0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            sleep(30);
        }
    }
    /* 信号处理 */
    init_sig_action(NULL);

    /* hal初始化 */
    if(0 != hal_init())
    {
        printf("hal_init failed!\n");
        exit(0);
        return -1;
    }
    
    /* 读取配置文件 */
    app_yc_init_config(&mqttip, &mqttport);
    INIT_FMT_DEBUG("Version: %s  Time: %s, area %s, Build: %s %s\n", APP_VERSION, APP_PUBLISH_TIME, AREANAME_PLACE, 
                                                                      __DATE__, __TIME__);
	INIT_FMT_DEBUG("终端版本: %s Time: %s\n", g_tVer.extend, g_tVer.softDate);

    /* 根据配置文件读交采透抄是否开启 */
    load_safemod_init();
    
    /* 消息总线模块 */
    if(msg_task_start(mqttip, mqttport))
    {
        printf("msg task start failed\n");
        exit(0);
        return -1;
    }

//#if defined PRODUCT_CCU || defined PRODUCT_ECU
    /* 启动显示模块 */
    AppPluginReg(g_plugin, g_nPluginCnt);
    errCode = pthread_create(&thidGui, NULL, advance_gui_start, NULL);
    if (errCode != 0)
    {
        printf("Thread advance_gui_start create error,err: %d\n", errCode);
        exit(0);
        return -1;
    }
//#endif
    /* 启动其他模块 */
    errCode = pthread_create(&thidMana, NULL, app_main_task, NULL);
    if (errCode != 0)
    {
        printf("Thread app_main_task create error,err: %d\n", errCode);
        exit(0);
        return -1;
    }

    while(1)
    {
        sleep(2);
    }
    return 0;
}

