#include "dataType.h"
#include "common.h"
#include<unistd.h>
#include <signal.h>
#include "iniparser.h"
#include "framecomm.h"
#include "signal.h"
#include "task_msg.h"
#include "trace_back.h"
#include "storage.h"           // 通用存储头文件
#include"extern.h"

/**********************************************************************
* @name      : init_config
* @brief     ：初始化配置文件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int init_config(uint32 *ip, uint16 *port)
{
    dictionary *ini = NULL;

    char  *intername = NULL;
    uint32 ipmask = 0xffffffff;
    uint32 ipaddr = 0;
    char  *stringvalue = NULL;
    int    intvalue = 0;

    char   curpath[256] = {0};
    char   filename[256] = {0};
    int i;

    //获取当前目录绝对路径，即去掉程序名
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("get err path!");
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
    sprintf(filename, "%s%s", curpath, GUI_INI_FILE);
    printf("filename is %s!\n", filename);

    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* ip获取 */
        intername = iniparser_getstring(ini, "MQTT_CONF:INTERNAME", NULL);
        if(NULL != intername)
        {
            //ipaddr = get_ifIP(intername);
            if(0 != ipaddr)
            {
                stringvalue = iniparser_getstring(ini, "MQTT_CONF:IPMASK", (char *)"255.255.255.255");
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
                stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", (char *)"127.0.0.1");
                ipaddr = inet_addr(stringvalue);
                ipaddr = ntohl(ipaddr);
            }
        }
        else
        {
            /* INTERNAME为空用IP */
            stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", (char *)"127.0.0.1");
            ipaddr = inet_addr(stringvalue);
            ipaddr = ntohl(ipaddr);
        }
        *ip = ipaddr;

        /* 端口获取 */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        *port = (uint16)intvalue;

        /* 获取ipc路径 */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:IPCPATH", (char *)"/mnt/ipc_socket");
        task_msg_prepare(stringvalue);

        /* 获取log路径 */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:LOGPATH", (char *)"/mnt/log");

        iniparser_freedict(ini);
    }
    else
    {
        printf("Get no ini file %s!\n", GUI_INI_FILE);

        *ip = 0x7f000001;
        *port = 1883;
        task_msg_prepare((char *)"/mnt/ipc_socket");
    }

    /* 日志模块初始化 */
    diag_init(filename, TRUE);

    return 0;
}

/**********************************************************************
* @name      : init_main
* @brief     ：初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int init_main(int argc, char *argv[])
{

//    int     iret = -1;

    /* 读取配置文件 */
    init_config(&gMqttIp, &gMqttPort);

    //APP重复运行检查
    if(!singleton_check((char *)MINI_GUI_TASK_PATH))
    {
        GUI_FMT_DEBUG("gui already running!\n");
        exit(-1);
        return -1;
    }

    if(argc >= 2)
    {
        if(0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            printf("sleep 30s\n");
            struct timespec curtime = {0};
            struct timespec oldtime = {0};
            clock_gettime(CLOCK_MONOTONIC, &oldtime);
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            while(curtime.tv_sec <= oldtime.tv_sec + 30)
            {
                sleep(1);
                clock_gettime(CLOCK_MONOTONIC, &curtime);
            }
        }
    }

	//注册文件
	reg_pdata_xram(ADV_APP_NAME, GUI_PRIVATE_DATA);

    //进程异常退出消息处理
    //sigaction(SIGINT,&act,NULL);
    init_sig_action((char *)"/data/app/desktopGui/log/Exception.log");

    return 0;
}
