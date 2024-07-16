/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved.
* @brief：   高级APP用采进程
* @date：    2020-3-4
* @history：
*********************************************************************
*/
#include "pageInterface.h"
#include "hal.h"
#include "trace_back.h"

#include "baseCommon.h"
#include <net/if.h>
#include "iniparser.h"
#include "task_msg.h"
#include "framecomm.h"
#include "app104/include/security.h"
#include "app104/include/pw_protocol.h"
#ifdef AREA_ZHEJIANG
#include "./area/zhejiang/pw_protocol_zhj.h"
#include "./area/zhejiang/pw_common_zhj.h"
#endif


#define  ADV_PD_INI_FILE            "pdAmr.ini"
#define  ADV_PD_ARG_FILE            "PdAmrConfig.ini"

extern PLUGIN_INFO* g_plugin[];
extern int g_nPluginCnt;

/* 线程判断 */
typedef int (*TASK_ENTRY)(void *);
typedef void *(*EntryFun)(void *);
typedef struct tagMainThread
{
    pthread_t          nThreadId;   /* 线程ID */
    EntryFun           pEntryFunc;  /* 线程入口函数 */
    TASK_ENTRY         pTaskEntry;
    char               taskname[20];
    uint32             restartFlag;
}MAIN_TRHEAD_T;

extern void comm_task_getarg(char *filename);
extern int msg_task_start(uint32 ip, uint16 port);
extern int comm_task_entry(void *pArg);
extern void advPd_app104task_getarg(char *filename);
extern int advPd_app104task_entry(void *pArg);
extern void advPd_pulse_set_config(uint8 bStart, uint8 rmtsig, uint8 status);
extern int advPd_pulsetask_entry(void *pArg);
extern void* advPd_gui_start(void* pArg);
extern BOOLEAN advPd_start_goon();

void *main_start_task(void *pArg);

MAIN_TRHEAD_T g_tMainThread[] =
{
    {0, main_start_task, comm_task_entry,         "comm",   1},
    {0, main_start_task, advPd_app104task_entry,  "app104", 1},
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
    ret = pHandle->pTaskEntry(pArg);
    printf("\n\nmain task %s stop, code %d!\n\n", pHandle->taskname, ret);
    sleep(20);
    pHandle->restartFlag = 1;
    sleep(10);
    return 0;
}

/**********************************************************************
* @name      : advPd_get_ifIP
* @brief     ：获取网口的IP地址
* @param[in] ：char *ifname 网口名称
* @param[out]：
* @return    ：IP地址
* @Create    : 王津剑
* @Date      ：2020-3-4
* @Update    :
**********************************************************************/
uint32 advPd_get_ifIP(char *ifname)
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

#ifdef AREA_ZHEJIANG
/**********************************************************************
* @name      : advPd_get_equip
* @brief     ：获取配电设备信息
* @param[in] ：dictionary * dic 配置文件信息
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
void advPd_get_equip(dictionary * dic)
{
    int z = 0;//组别
        
    //终端数量
    g_aGroupCountZhj[E_EQUIP_POWERTERMINAL-1] = iniparser_getint(dic, "PW_EQUIP_CONF:POWERTERMINAL_GRP", 0);
    if(g_aGroupCountZhj[E_EQUIP_POWERTERMINAL-1] > 1)
    {
        g_aGroupCountZhj[E_EQUIP_POWERTERMINAL-1] = 1;//默认只有一组终端设备
    }
    for(z=0; z<g_aGroupCountZhj[E_EQUIP_POWERTERMINAL-1]; z++)
    {
        g_aEquipCountZhj[E_EQUIP_POWERTERMINAL-1][z] = iniparser_getint(dic, "PW_EQUIP_CONF:POWERTERMINAL_NUM", 0);
        if(g_aEquipCountZhj[E_EQUIP_POWERTERMINAL-1][z] > 1)
        {
            g_aEquipCountZhj[E_EQUIP_POWERTERMINAL-1][z] = 1;//默认只有一个终端设备
        }
    }
	
    //配电变压器数量
    g_aGroupCountZhj[E_EQUIP_TRANSFORMER-1] = iniparser_getint(dic, "PW_EQUIP_CONF:TRANSFORMER_GRP", 0);
    if(g_aGroupCountZhj[E_EQUIP_TRANSFORMER-1] > 1)
    {
        g_aGroupCountZhj[E_EQUIP_TRANSFORMER-1] = 1;//默认只有一组配电变压器
    }
    for(z=0; z<g_aGroupCountZhj[E_EQUIP_TRANSFORMER-1]; z++)
    {
        g_aEquipCountZhj[E_EQUIP_TRANSFORMER-1][z] = iniparser_getint(dic, "PW_EQUIP_CONF:TRANSFORMER_NUM", 0);
        if(g_aEquipCountZhj[E_EQUIP_TRANSFORMER-1][z] > 1)
        {
            g_aEquipCountZhj[E_EQUIP_TRANSFORMER-1][z] = 1;//默认只有一个配电变压器
        }
    }
    
	//智能电容器数量
	g_aGroupCountZhj[E_EQUIP_CAPACITOR-1] = iniparser_getint(dic, "PW_EQUIP_CONF:CAPACITOR_GRP", 0);
    if(g_aGroupCountZhj[E_EQUIP_CAPACITOR-1] > MAX_EQUIP_GROUP_NUM)
    {
        g_aGroupCountZhj[E_EQUIP_CAPACITOR-1] = MAX_EQUIP_GROUP_NUM;//默认只有一组智能电容器
    }
    for(z=0; z<g_aGroupCountZhj[E_EQUIP_CAPACITOR-1]; z++)
    {
        g_aEquipCountZhj[E_EQUIP_CAPACITOR-1][z] = iniparser_getint(dic, "PW_EQUIP_CONF:CAPACITOR_NUM", 0);
        if(g_aEquipCountZhj[E_EQUIP_CAPACITOR-1][z] > MAX_EQUIP_COUNT_NUM)
        {
            g_aEquipCountZhj[E_EQUIP_CAPACITOR-1][z] = MAX_EQUIP_COUNT_NUM;//默认只有一个智能电容器
        }
    }
    
	//剩余电流保护数量
	g_aGroupCountZhj[E_EQUIP_RESIDUALCURRENT-1] = iniparser_getint(dic, "PW_EQUIP_CONF:RESIDUALCURRENT_GRP", 0);
    if(g_aGroupCountZhj[E_EQUIP_RESIDUALCURRENT-1] > MAX_EQUIP_GROUP_NUM)
    {
        g_aGroupCountZhj[E_EQUIP_RESIDUALCURRENT-1] = MAX_EQUIP_GROUP_NUM;//默认只有一组剩余电流保护数量
    }
    for(z=0; z<g_aGroupCountZhj[E_EQUIP_RESIDUALCURRENT-1]; z++)
    {
        g_aEquipCountZhj[E_EQUIP_RESIDUALCURRENT-1][z] = iniparser_getint(dic, "PW_EQUIP_CONF:RESIDUALCURRENT_NUM", 0);
        if(g_aEquipCountZhj[E_EQUIP_RESIDUALCURRENT-1][z] > MAX_EQUIP_COUNT_NUM)
        {
            g_aEquipCountZhj[E_EQUIP_RESIDUALCURRENT-1][z] = MAX_EQUIP_COUNT_NUM;//默认只有一个剩余电流保护数量
        }
    }
}
#endif

/**********************************************************************
* @name      : advPd_init_config
* @brief     ：初始化配置文件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-4
* @Update    :
**********************************************************************/
int advPd_init_config(uint32 *ip, uint16 *port)
{
    dictionary *ini = NULL;

    char  *intername = NULL;
    uint32 ipmask = 0xffffffff;
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
    sprintf(filename, "%s%s", curpath, ADV_PD_INI_FILE);

    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* ip获取 */
        intername = iniparser_getstring(ini, "MQTT_CONF:INTERNAME", NULL);
        if(NULL != intername)
        {
            ipaddr = advPd_get_ifIP(intername);
            if(0 != ipaddr)
            {
                stringvalue = iniparser_getstring(ini, "MQTT_CONF:IPMASK", "255.255.255.255");
                ipmask = inet_addr(stringvalue);
                ipmask = ntohl(ipmask);
                if(0xffffffff != ipmask)
                {
                    //通过掩码获取服务端地址
                    ipaddr = (ipaddr & ipmask) + 0x01;
                }
            }
            else
            {
                /* 无接口时用IP */
                stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "127.0.0.1");
                ipaddr = inet_addr(stringvalue);
                ipaddr = ntohl(ipaddr);
            }
        }
        else
        {
            /* INTERNAME为空用IP */
            stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "127.0.0.1");
            ipaddr = inet_addr(stringvalue);
            ipaddr = ntohl(ipaddr);
        }
        *ip = ipaddr;

        /* 端口获取 */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        *port = (uint16)intvalue;

        /* 获取ipc路径 */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:IPCPATH", "/mnt/ipc_socket");
        task_msg_prepare("/mnt/ipc_socket");

        /* 获取log路径 */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:LOGPATH", "/mnt/log");
        char   exceptpath[256] = {0};
        sprintf(exceptpath, "%s/Exception.log", stringvalue);
        change_sig_file(exceptpath);

        /* 获取本地安全协议使能标志 */
        stringvalue = iniparser_getstring(ini, "SECURITY_CONF:LOCAL_ENABLE", NULL);
        if(NULL != stringvalue)
        {
            security_enable(atoi(stringvalue));
        }

        #ifdef AREA_ZHEJIANG
        advPd_get_equip(ini);
        #endif

        iniparser_freedict(ini);
    }
    else
    {
        printf("Get no ini file %s!\n", ADV_PD_INI_FILE);

        *ip = 0x7f000001;
        *port = 1883;
        task_msg_prepare("/mnt/ipc_socket");
    }

	/* 日志模块初始化 */
    diag_init(filename, TRUE);

    memset(filename, 0, 256);
    sprintf(filename, "%s%s", curpath, ADV_PD_ARG_FILE);

    /* 通信模块读取通信配置 */
    comm_task_getarg(filename);

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
void* appPd_main_task(void *arg)
{
    uint8 i;
    int errCode;

	while(FALSE == advPd_start_goon())
    {
    	sleep(1);
    }

    /* 启动task */
    while(1)
    {
        for(i = 0; i < g_MainTaskNum; i++)
        {
            MAIN_TRHEAD_T *pthread = &(g_tMainThread[i]);
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
    int       errCode;
    pthread_t thidGui = 0;
    pthread_t thidMana = 0;
    uint32 mqttip = 0;
    uint16 mqttport = 0;

    if(argc >= 2)
    {
        if(0 == memcmp(argv[1], "version", strlen("version")))
        {
            printf("Version: 0x%x\nTime: %s\n", APP_VERSION, APP_PUBLISH_TIME);
            return 0;
        }
    }

    /* 检查程序只启动了一个副本 */
    if(!singleton_check("pdAmr"))
    {
        return -1;
    }

    if(argc >= 2)
    {
        if(0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            printf("30s waiting...\n");
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
    advPd_init_config(&mqttip, &mqttport);

    /* 对外通信线程启动 */
    if(0 != msg_task_start(mqttip, mqttport))
    {
        printf("msg task start failed\n");
        exit(0);
        return -1;
    }

    /* 启动显示 */
    AppPluginReg(g_plugin, g_nPluginCnt);
    errCode = pthread_create(&thidGui, NULL, advPd_gui_start, NULL);
    if (errCode != 0)
    {
        printf("Thread advance_gui_start create error,err: %d\n", errCode);
        exit(0);
        return -1;
    }
    

    /* 启动task */
    errCode = pthread_create(&thidMana, NULL, appPd_main_task, NULL);
    if (errCode != 0)
    {
        printf("Thread app_main_task create error,err: %d\n", errCode);
        exit(0);
        return -1;
    }

    /* 启动task */
    while(1)
    {
        sleep(2);
    }
    return 0;
}


