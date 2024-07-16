/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程主线程
* @date：    2019-12-3
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "db_api.h"
#include "task_id.h"
#include "msgSecApp.h"
#include "appcomm.h"
#include "appipc.h"
#include "protocol698.h"
#include "appreport.h"
#include "appoutmsg.h"
#include "appctrl.h"
#include "framecomm.h"
#include "hal.h"
#include "led.h"
#include "audio.h"
#include "appctrl_module.h"
#include "lcmtrans.h"

#define APP698_PROTOCOL_POOL   1  //1-线程池调度方法 0-固定通道线程调度

DB_CLIENT    g_AppSockDb = 0;

uint8        g_IpcRecvFlag = 0;
IPC_THREAD_T g_tIpcRecv = {&g_IpcRecvFlag};
uint8        g_IpcSendFlag = 0;
IPC_THREAD_T g_tIpcSend = {&g_IpcSendFlag};

ProxyLockQueue ProxyQueue;
ProxyLockQueue ProxyrespondeQueue;
ProxyLockQueue TransQueue;
OOP_PERCEPTION_TERMINAL_LIST_T perceptionterminal;


#if APP698_PROTOCOL_POOL
uint8        g_ProtFlag = 0;
PROTOCOL_THREAD_T g_tProtocol = {0, 0, 0, &g_ProtFlag, {0}};
#else
uint8        g_Prot1Flag = 0;
PROTOCOL_THREAD_T g_tProtocol1 = {1, 0, 0, &g_Prot1Flag, {0}};
uint8        g_Prot2Flag = 0;
PROTOCOL_THREAD_T g_tProtocol2 = {2, 0, 0, &g_Prot2Flag, {0}};
uint8        g_Prot3Flag = 0;
PROTOCOL_THREAD_T g_tProtocol3 = {3, 0, 0, &g_Prot3Flag, {0}};
uint8        g_Prot4Flag = 0;
PROTOCOL_THREAD_T g_tProtocol4 = {4, 0, 0, &g_Prot4Flag, {0}};
uint8        g_Prot5Flag = 0;
PROTOCOL_THREAD_T g_tProtocol5 = {5, 0, 0, &g_Prot5Flag, {0}};
#endif

uint8        g_ReportFlag = 0;
REPORT_THREAD_T g_tReport = {&g_ReportFlag};

uint8        g_ConnectFlag = 0;
REPORT_THREAD_T g_tConnect = {&g_ConnectFlag};

uint8        g_EventTimeFlag = 0;
REPORT_THREAD_T g_tEventTime = {&g_EventTimeFlag};

uint8        g_OtherTimeFlag = 0;
REPORT_THREAD_T g_tOtherTime = {&g_OtherTimeFlag};

uint8        g_OutMsgFlag = 0;
OUTMSG_THREAD_T g_tOutMsg = {0, &g_AppSockDb, &g_OutMsgFlag};

uint8         g_CtrlFlag = 0;
CTRL_THREAD_T g_tCtrlHandle = {&g_CtrlFlag};

uint8         g_CtrlModuleFlag = 0;
CTRL_DEV_H_T  g_tCtrlModuleHandle = {&g_CtrlModuleFlag, &g_send, &g_recv, &gState[0], &gConnect};

#if defined (AREA_JIANGSU) && defined (PRODUCT_ZCU_1)
uint8        g_LcmTimeFlag = 0;
LCMTRANS_THREAD_T g_tLcmTime = {.pRunFlag=&g_LcmTimeFlag};

uint8        g_LjmTimeFlag = 0;
LCMTRANS_THREAD_T g_tLjmTime = {.pRunFlag=&g_LjmTimeFlag};

#endif

extern void *app_ctrl_entry(void *pArg);
extern void *app_module_ctrl_entry(void *pArg);
APP698_TRHEAD_T g_tThread[] = 
{/*  线程ID, 入口函数,           入参,          线程运行标记 */
    {0,      app_ipc_recv,       &g_tIpcRecv,   &g_IpcRecvFlag},
    {0,      app_ipc_send,       &g_tIpcSend,   &g_IpcSendFlag},
#if APP698_PROTOCOL_POOL
    {0,      protocol_698_pool,  &g_tProtocol,  &g_ProtFlag},
#else
    {0,      protocol_698_entry, &g_tProtocol1, &g_Prot1Flag},
    {0,      protocol_698_entry, &g_tProtocol2, &g_Prot2Flag},
    {0,      protocol_698_entry, &g_tProtocol3, &g_Prot3Flag},
    {0,      protocol_698_entry, &g_tProtocol4, &g_Prot4Flag},
    {0,      protocol_698_entry, &g_tProtocol5, &g_Prot5Flag},
#endif
    {0,      app_auto_report,    &g_tReport,    &g_ReportFlag},
    {0,      app_other_time_work,&g_tOtherTime, &g_OtherTimeFlag},
    {0,      app_event_time_work,&g_tEventTime, &g_EventTimeFlag},
    {0,      app_preconnect_proc,&g_tConnect,   &g_ConnectFlag},
    {0,      app_msg_entry,      &g_tOutMsg,    &g_OutMsgFlag},
#ifdef ADV_ST_AMR
    {0,      app_ctrl_entry,     &g_tCtrlHandle,&g_CtrlFlag},
    //{0,   app_module_ctrl_entry, &g_tCtrlModuleHandle,&g_CtrlModuleFlag},
#endif

#if defined (AREA_JIANGSU) && defined (PRODUCT_ZCU_1)
    {0,      app_lcm_trans_work,&g_tLcmTime, &g_LcmTimeFlag},
    {0,      app_ljm_trans_work,&g_tLjmTime, &g_LjmTimeFlag},
#endif

};
uint8 g_TrheadNum = sizeof(g_tThread)/sizeof(APP698_TRHEAD_T);

LED_DEVICE_T*   g_LEDdev = NULL;
AUDIO_DEVICE_T* g_Buzzdev = NULL;
OOP_WARNINGTIME_T g_AppWarnTime;
OOP_WARNINGOUT_T  g_AppWarnGroup;

/**********************************************************************
* @name      : app_leddev_get
* @brief     ：控制LED句柄获取
* @param[in] ：
* @param[out]：
* @return    ：void * 
* @Create    : 王津剑
* @Date      ：2020-7-17
* @Update    : 
**********************************************************************/
void *app_leddev_get()
{
    return g_LEDdev;
}

/**********************************************************************
* @name      : appmain_led_status
* @brief     ：控制LED出事状态
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-24
* @Update    : 
**********************************************************************/
void appmain_led_status()
{
    if(NULL == g_LEDdev)
    {
        g_LEDdev = (LED_DEVICE_T*)hal_device_get(HW_DEVICE_ID_LED);
    }
    
    if(NULL != g_LEDdev)
    {
        /* 关闭告警灯 */
        g_LEDdev->led_state_set(g_LEDdev, EMLED_ALARM, LED_OFF, COM_MODE, NULL, 0);
        /* 打开运行灯 */
        g_LEDdev->led_state_set(g_LEDdev, EMLED_RUN, LED_ON, COM_MODE, NULL, 0);
    }
}

/**********************************************************************
* @name      : app_buzzer_init
* @brief     ：蜂鸣器初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-30
* @Update    :
**********************************************************************/
void app_buzzer_init()
{
    if(NULL == g_Buzzdev)
    {
        g_Buzzdev = (AUDIO_DEVICE_T*)hal_device_get(HW_DEVICE_ID_AUDIO);
    }
    if(NULL != g_Buzzdev)
    {
        /* 关闭蜂鸣器 */
        g_Buzzdev->audio_set_buzzer_out(g_Buzzdev, 0, 100, 1);
    }
    memset(&g_AppWarnGroup, 0, sizeof(OOP_WARNINGOUT_T));
    memset(&g_AppWarnTime, 0, sizeof(OOP_WARNINGOUT_T));
}

/**********************************************************************
* @name      : appmain_buzzer_status
* @brief     ：蜂鸣器状态控制
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-30
* @Update    : 
**********************************************************************/
int appmain_buzzer_warn_data(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    OOP_WARNINGTIME_T tWarn;

    if(0xF2060400 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.oad.value = value;
    memset(&tWarn, 0, sizeof(OOP_WARNINGTIME_T));
    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_WARNINGTIME_T), (uint8 *)&tWarn, &readlen);
    if(0 != ret || readlen != sizeof(OOP_WARNINGTIME_T))
    {
        APP698_FMT_DEBUG("Appmain get no warn time!\n");
        memset(&tWarn, 0, sizeof(OOP_WARNINGTIME_T));
    }
    memcpy(&g_AppWarnTime, &tWarn, sizeof(OOP_WARNINGTIME_T));
    return 0;
}

/**********************************************************************
* @name      : appmain_get_warn_id
* @brief     ：获取当前告警时段序号
* @param[in] ：
* @param[out]：
* @return    ：0-无线; 其他有效
* @Create    : 王津剑
* @Date      ：2020-7-30
* @Update    :
**********************************************************************/
uint8 appmain_get_warn_id()
{
    time_t           now;
    OOP_DATETIME_S_T curTime = {0};
    uint32           curSecond = 0;
    uint32           startSecond = 0;
    uint32           endSecond = 0;
    uint32           i;

    if(0 == g_AppWarnTime.nNum)
    {
        return 0;
    }

    now = time(NULL);
    DT_Time2DateTimeShort(now, &curTime);

    curSecond = curTime.hour * 3600 + curTime.minute * 60 + curTime.second;

    for(i = 0; i < g_AppWarnTime.nNum; i++)
    {
        startSecond = g_AppWarnTime.period[i].startTime.hour * 3600 + g_AppWarnTime.period[i].startTime.minute * 60
                    + g_AppWarnTime.period[i].startTime.second;
        endSecond = g_AppWarnTime.period[i].endTime.hour * 3600 + g_AppWarnTime.period[i].endTime.minute * 60
                    + g_AppWarnTime.period[i].endTime.second;
        if(startSecond > endSecond)
        {
            /* 过0点 */
            endSecond += 24 *3600;
        }
        if(startSecond <= curSecond && curSecond < endSecond)
        {
            return i + 1;
        }
    }
    return 0;
}

/**********************************************************************
* @name      : appmain_buzzer_status
* @brief     ：蜂鸣器状态控制
* @param[in] ：int dbClient  数据中心句柄
               uint8 status  状态
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-30
* @Update    : 
**********************************************************************/
void appmain_buzzer_status(int dbClient, uint8 status)
{
    uint8            warnid = 0;
    OOP_WARNINGOUT_T tWarnGroup;
    NOMAL_OAD_T      tOAD;
    int ret = 0;

    memset(&tWarnGroup, 0, sizeof(OOP_WARNINGOUT_T));
    tWarnGroup.nNum = g_AppWarnTime.nNum;

    if(NULL == g_Buzzdev)
    {
        goto end;
    }

    /* 关闭不限时时间 */
    if(FALSE == status)
    {
        g_Buzzdev->audio_set_buzzer_out(g_Buzzdev, 0, 100, 1);
    }
    else
    {
        warnid = appmain_get_warn_id();
        if(warnid > 0)
        {
            g_Buzzdev->audio_set_buzzer_out(g_Buzzdev, 1, 100, 1);
            tWarnGroup.state[warnid - 1] = 1;
        }
    }

end:
    if(0 == memcmp(&g_AppWarnGroup, &tWarnGroup, sizeof(OOP_WARNINGOUT_T)))
    {
        return;
    }

    if(dbClient <= 0)
    {
        dbClient = g_AppSockDb;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.oad.value = 0xF2060200;
    tOAD.classtag = CLASS_DATA_NORMAL;
    memcpy(&g_AppWarnGroup, &tWarnGroup, sizeof(OOP_WARNINGOUT_T));
    ret = db_write_nomal(dbClient, &tOAD, (uint8*)&g_AppWarnGroup, sizeof(OOP_WARNINGOUT_T));
    if(0 != ret)
    {
        APP698_FMT_DEBUG("Appmain write f2060200 failed, ret %d!\n", ret);
    }
}

/**********************************************************************
* @name      : appmain_audio_set
* @brief     ：语音播放
* @param[in] ：int indexno     序号
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-7-31
* @Update    : 
**********************************************************************/
void appmain_audio_set(int indexno)
{
    if(NULL != g_Buzzdev)
    {
        g_Buzzdev->audio_play(g_Buzzdev, indexno);
    }
}

#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)|| (defined AREA_JIANGSU)
void taskmng_perception_tmn_info_load(DB_CLIENT clientid)
{
    int   ret;
    uint32 Len;
    uint16 i;

    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0xFF110E00;
    NormalOad.infoNum = 1;
    OOP_PERCEPTION_TERMINAL_INFO_T terminalinfo;
    uint8 num=0;
    uint8 PerceptionTerminalInfoNum=0;

    memset((uint8*)&perceptionterminal,0x00,sizeof(OOP_PERCEPTION_TERMINAL_LIST_T));

    for(i=0;i<96;i++)
    {
        NormalOad.infoNum = i+1;
        ret = db_read_nomal(clientid,&NormalOad,sizeof(terminalinfo),(uint8*)&terminalinfo,&Len);
        if(ret!=0 || Len != sizeof(OOP_PERCEPTION_TERMINAL_INFO_T))
        {
            PRTL_FMT_DEBUG("ret %d, Len %d\n",ret,Len);
            continue;
        }
        memcpy((uint8*)&perceptionterminal.terminalinfo[i],(uint8*)&terminalinfo,sizeof(terminalinfo));
        PerceptionTerminalInfoNum++;
    }
    PRTL_FMT_DEBUG("PerceptionTerminalInfoNum %d\n",PerceptionTerminalInfoNum);
    if(PerceptionTerminalInfoNum<1)
    {
        return;
    }
    perceptionterminal.num = PerceptionTerminalInfoNum;
    num = perceptionterminal.terminalinfo[PerceptionTerminalInfoNum-1].meterlist.num;
    if(num>=1)
    {
        perceptionterminal.totalpn = perceptionterminal.terminalinfo[PerceptionTerminalInfoNum-1].meterlist.meterinfo[num-1].pn;
        perceptionterminal.totalpn++;
    }
     PRTL_FMT_DEBUG("perceptionterminal.num %d, perceptionterminal.totalpn %d\n",
                            perceptionterminal.num, perceptionterminal.totalpn);
    return;

}
#endif
extern int ssal_get_dbstatus(int clientid, uint8 logicId, uint16 infoNum, uint32 value);

/**********************************************************************
* @name      : app698_get_db
* @brief     ：进程启动 首次获取数据库书库
* @param[in] ：int sockdbfd    句柄
* @param[out]：
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2019-12-6
* @Update    :
**********************************************************************/
int app698_get_db(int sockdbfd)
{
    appmain_buzzer_warn_data(sockdbfd, 0, 0, 0xF2060400);
    app_report_getdb(sockdbfd);
    #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)|| (defined AREA_JIANGSU)
    taskmng_perception_tmn_info_load(sockdbfd);
    #endif
    ssal_get_dbstatus(sockdbfd, 0, 0, 0xF1010400);
    return 0;
}

/*------------------入口函数---------------------------*/
/**********************************************************************
* @name      : comm_task_entry
* @brief     ：主应用调度模块线程入口
* @param[in] ：uint32 dbip
               uint16 dbport
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-23
* @Update    :
**********************************************************************/
int appmain_task_entry(uint32 dbip, uint16 dbport)
{
    uint8   i;
    int     errCode = 0;
    APP698_TRHEAD_T *pThread = NULL;
    

    g_AppSockDb = db_client_create(DB_YC_AGENT, DB_CLIENT_APPMAIN);
    if(g_AppSockDb <= 0)
    {
        APP698_FMT_DEBUG("db_handle_create failed!\n");
        return 0;
    }

    /* 硬件相关操作 */
    appmain_led_status();
    app_buzzer_init();

    /* 初始化 */
    app_init_channel(dbip, dbport);//初始化通道
    
#if APP698_PROTOCOL_POOL
    /* 线程池初始化 */
    if(0 != protocol_698_poolinit())
    {
        APP698_FMT_DEBUG("protocol_698_poolinit failed!\n");
        return 0;
    }
    APP698_FMT_DEBUG("protocol analyse mathod: pool!\n");
#else
    /* 绑定通道 固定通道初始化 */
    PROTOCOL_THREAD_BIND_CHANNELS(g_tProtocol1, CHL_GPRS_1, CHL_GPRS_1 + 0x30 - 1);
    PROTOCOL_THREAD_BIND_CHANNELS(g_tProtocol2, CHL_GPRS_2, CHL_GPRS_2 + 0x30 - 1);
    PROTOCOL_THREAD_BIND_CHANNELS(g_tProtocol3, CHL_ETH_1, CHL_ETH_1 + 0x30 - 1);
    PROTOCOL_THREAD_BIND_CHANNELS(g_tProtocol4, CHL_ETH_2, CHL_ETH_2 + 0x30 - 1);
    g_tProtocol5.channel[0] = LOCATE_232;
    g_tProtocol5.channel[1] = JOIN485_1;
    g_tProtocol5.channel[2] = COMM_LISTEN;
    g_tProtocol5.num = 3;
    APP698_FMT_DEBUG("protocol analyse mathod: fix-channel!\n");
#endif

    app_msg_sync_init();
    app_report_init();

    if(0 != app698_get_db(g_AppSockDb))
    {
        APP698_FMT_DEBUG("app698_get_db failed!\n");
        return 0;
    }

    /* 安全加密相关的初始化 */
    if (!SaveTESAMInfo())
    {
        APP698_FMT_DEBUG("SaveTESAMInfo failed!\n");
    }

    APP698_FMT_DEBUG("SaveTESAMInfo finish!\n");

    /* 安全管理相关的初始化 */
    if (!sec_smiSsal_fd_init(ADV_APP_NAME))
    {
        APP698_FMT_DEBUG("sec_smiSsal_fd_init failed!\n");
    }

    APP698_FMT_DEBUG("sec_smiSsal_fd_init finish!\n");

    /* 与交采APP通信的UDP相关 */
    errCode = puAmr_acMeter_fd_init();
    if (0 != errCode)
    {
        APP698_FMT_DEBUG("puAmr_acMeter_fd_init failed!\n");
    }

    if(0 != app_ipc_init())
    {
        APP698_FMT_DEBUG("app_ipc_init failed!\n");
        return 0;
    }

    while(1)
    {
        for(i = 0; i < g_TrheadNum; i++)
        {
            pThread = &(g_tThread[i]);
            if(pThread->nThreadId <= 0)
            {
                /* 启动线程 */
                errCode = pthread_create(&pThread->nThreadId, NULL, pThread->pEntryFunc, pThread->pArg);
                if (errCode != 0)
                {
                    APP698_FMT_DEBUG("Thread %d Create error! err:%d\n", i, errCode);
                    pThread->nThreadId = 0;
                    continue;
                }
            }
            (*pThread->pRunFlag)++;
            if(*pThread->pRunFlag < APP_TRHEAD_FLAG_MUX)
            {
                continue;
            }
            /* 关闭线程 */
            APP698_FMT_DEBUG("Cancel appmain thread %d!\n", i);

            pthread_cancel(pThread->nThreadId);
            errCode = pthread_join(pThread->nThreadId, NULL);
            pThread->nThreadId = 0;
        }
        sleep(1);
    }
}
