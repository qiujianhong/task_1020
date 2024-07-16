/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� ϵͳ��������ڳ���
* @date�� 2019-11-9
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

//smios����·��
static const sint8 *s_configFilePath    = CFG_PATH;

 char APPID[APPID_LEN]={0};
 uint32 APPIDLen=APPID_LEN;

/**
*********************************************************************
* @brief�� Termination handler
* @param[in]��
               signum      UNIX �źű��
               sigInfo     �ź����Խṹ����
               context     �ص�����
* @param[out]��
               none
* @return�� none
*********************************************************************
*/
static void signal_handler(sint32 signum, siginfo_t *sigInfo, void *context)
{
    UNUSED(sigInfo);
    UNUSED(context);
    if (signum == SIGUSR1)  //�ź�ΪSIGUSR1ʱ������־���� ����crontab+logrotate
                            //������־����
    {
        //��־������Ϣ����
        diag_init((char*)s_configFilePath, TRUE);

        return;
    }
                            
    INIT_FMT_DEBUG("shut down signal is %d, sender PID:%d\n", signum,
               sigInfo->si_pid);
    de_init_system();  //�����ͷż�����������ռ���ⲿ��Դ��ˢ�����ݿ�
    INIT_FMT_DEBUG("smiOS is shut down!\n");
    exit(EXIT_SUCCESS);
}

/* @brief: opt���Ͳ�����  */
struct optparse_long longopts[] = {
    {"daemon", 'd', OPTPARSE_NONE},          //�������ػ�������ʽ����
    {"config", 'c', OPTPARSE_REQUIRED},      //���������ļ�·��
    {"version", 'v', OPTPARSE_NONE},         //��ӡ��ǰ����汾��Ϣ
    {"log-config", 'l', OPTPARSE_REQUIRED},  //����OLOG�����ļ�·��
    {0},                                     //��β����ն����ֹԽ��
};


/**
*********************************************************************
* @brief�� ��Ӳʱ��ͬ��
* @param[in]��
                ��

* @param[out]��
               none        ����˵��
* @return�� ����0 �����˳�
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
            INIT_FMT_DEBUG("�ϵ�ͬ��ʱ�Ӵ���: %d-%d-%d %d:%d:%d, �ĳ�120\n", rtc.tm_year, rtc.tm_mon, rtc.tm_mday, 
                                                                        rtc.tm_hour, rtc.tm_min, rtc.tm_sec);
            rtc.tm_year = 120;
        }
        
        iRet = dev->rtc_time_set(dev, &rtc);
    }
    else
    {
        INIT_FMT_DEBUG("��ȡʱ��ʧ��\n");
    }
    
    hal_device_release((HW_DEVICE *)dev);
#endif
    return iRet;
}

/**
*********************************************************************
* @brief�� ����ip����
* @param[in]�� void
* @param[out]��
               none        ����˵��
* @return�� ����0 �����˳�
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
        /* MAC��ַ�Ͷ�λ������ż��, ��32λ��1��ʾ�鲥��ַ, ������0 */
        uint8 i = 0;
        struct  timeval tv;
        struct  timezone tz;
        /* ���һ��MAC��ַ */
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
* @brief�� get_app_id ��ȡϵͳ������appid
* @param[in]��
               argc      ��������
               argv[]    �����б�
* @param[out]��
               none        ����˵��
* @return�� ����appid����
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
* @brief�� main�������
* @param[in]��
               argc      ��������
               argv[]    �����б�
* @param[out]��
               none        ����˵��
* @return�� ����0 �����˳�
*********************************************************************
*/
sint32 main(sint32 argc, sint8 *argv[])
{
    sint32          daemonFlag = 0;
    sint32          option     = 0;
    struct optparse options    = {0};
    struct sigaction act;  // Linux �źŶ����ʼ��
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
    const char* appName = "smiOS";
    const char* iconFile = "/usr/local/extapps/smiOS/bin/cfg/smiOS.bmp";
    APP_REGIST_INFO_T  appRegInfo;      //��¼��������ʾ
    time_t   LastLoginGUI = 0;
    struct timespec curtime = {0};
//#endif
    memset(&act, 0, sizeof(struct sigaction));
    //�ж�smios�Ƿ��Ѿ�����
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

    //�����쳣���
    init_sig_action("/data/app/smiOS/log/Exception.log");
    hal_init();
    diag_init((char*)s_configFilePath, TRUE);
    chk_systime_with_rtc();     // �ϵ�ͨ��RTCУ׼ʱ��
//    APPIDLen = get_app_id((uint8*)APPID,APPID_LEN);

    /* ��־ģ���ʼ�� */
    
    smios_set_starttime();

    UNUSED(argc);
    optparse_init(&options, argv);  // opt �����в�����ʼ��
    while ((option = optparse_long(&options, longopts, NULL)) != -1)
    {
        switch (option)
        {
            case 'd':
                daemonFlag = 1;  // daemon ��ʽ���б�־λ
                break;
            case 'c':
                s_configFilePath =
                    options.optarg;  //������ָ��smiOS�����ļ�·��
                break;
            case 'v':
                INIT_FMT_DEBUG("smiOS Version:\t%d.%d.%d.%d\n",
                        smiOS_VERSION_MAJOR, smiOS_VERSION_MINOR,
                        smiOS_VERSION_PATCH, smiOS_VERSION_SVN);  //����汾��Ϣ
                exit(EXIT_SUCCESS);
                break;
            case '?':
                INIT_FMT_DEBUG("%s: %s\n", argv[0],
                        options.errmsg);  //����optѡ���
                exit(EXIT_FAILURE);
                break;
            default:
                INIT_FMT_DEBUG("opt parser fail\n");  // opt���ڲ��쳣
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (1 == daemonFlag)
    {
        if (-1 == daemon(0, 0))  //�ػ����̷�ʽ����
        {
            INIT_FMT_DEBUG("smiOS run as daemon fail\n");  //��־ϵͳδ��ʼ�� �����stderr
        }
    }
    INIT_FMT_DEBUG("Starting smiOS service...\n");
    act.sa_flags = SA_SIGINFO;  //���趨�˸ñ�־λʱ����ʾ�źŸ����Ĳ������Ա����ݵ��źŴ�������
    act.sa_sigaction = signal_handler;  //Ϊ�źŶ���󶨻ص�����
    sigaction(SIGTERM, &act, NULL);  //ע��Ԥ�������źŵĻص� Ĭ����ֹ����
    sigaction(SIGHUP, &act, NULL);   //Ĭ����ֹ����
    sigaction(SIGINT, &act, NULL);   //Ĭ����ֹ����
    sigaction(SIGUSR1, &act, NULL);  // SIGUSR1���ڴ�����־����
    sigaction(SIGUSR2, &act, NULL);  //Ĭ����ֹ����

    INIT_FMT_DEBUG("Starting smiOS service...\n");

    //ȫ�������ļ���ʼ��
    if (0 != global_config_init(s_configFilePath))
    {
        INIT_FMT_DEBUG("global_config_init fail path:[%s]\n", s_configFilePath);
        exit(EXIT_FAILURE);
    }
    INIT_FMT_DEBUG("Starting smiOS service...\n");

    local_ip_info_parse();      //�ϵ����ñ���ip

#ifdef GLOBAL_CONFIG_DUMP
    global_config_dump();  //���ȫ�������ļ�����
#endif

    if (0 != init_system())  //��ʼ��ϵͳ����ģ��
    {
        INIT_FMT_DEBUG("init system fail\n");
        exit(EXIT_FAILURE);
    }

    if (0 != start_system())  //��ʼ��ϵͳ����ģ��
    {
        INIT_FMT_DEBUG("start system fail\n");
        exit(EXIT_FAILURE);
    }
    
    //���ý������ȼ�
    nice(-20);
    while (1)
    {
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        if(getLoginguiID() == 0 && curtime.tv_sec - LastLoginGUI > 3)
        {
            printf("������GUI����ע������!,getLoginguiID()= %d\n",getLoginguiID());
            memset(&appRegInfo,0x0,sizeof(APP_REGIST_INFO_T));
            memcpy(appRegInfo.appname,appName,strlen(appName));
            memcpy(appRegInfo.shortname, ADV_SHOW_NAME, strlen(ADV_SHOW_NAME));
            memcpy(appRegInfo.iconFileName,iconFile,strlen(iconFile));
            appGUI_send_req_handle(0x0007, 0x0010, &appRegInfo, sizeof(APP_REGIST_INFO_T)); //ע�ᵽ������ʾ
            LastLoginGUI = curtime.tv_sec;
        }
//#endif
        sleep(1000);  //�����̳������д� �����߼��������������ģ��
    }

    exit(EXIT_SUCCESS);
}
