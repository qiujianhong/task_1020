/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#include "ac_main.h"
#include "ac_commdrv.h"
#include "ac.h"
#include "ac_save.h"
#include "ac_sample.h"
#include "ac_sample698.h"
#include "ac_report.h"
#include "ac_mqtt.h"
#include "ac_lib.h"
#include "ac_module.h"
#include "trace_back.h"
#include <sys/prctl.h>
#include <net/if.h> /*网络ifi_xxx结构定义*/
#include "hal.h"
#include "rtc.h"
#include "uart.h"
#include "power.h"
#include "watchdog.h"
#include "adc.h"
#include "ac_485comm.h"
#include "db_update.h"
#include "ac_698msg.h"
#include "ac_otherfun.h"
#include "ac_update698.h"

uint8 gdelay =0 ;

/*******************************************************************************
* 全局变量
*******************************************************************************/
//周期采集采用的规约，0-DLT645 1-698.45
uint8 gSampPrtl = 0;

typedef enum tag_MSG_TYPE
{
    MSGAPDU = 0,           // APDU
    MSG698 = 1,            // 698报文
    MSG645 = 2,            // 645报文
}MSG_TYPE_E;

//调试信息配置文件
#define    AC_METER_INI_FILE  "acMeter.ini"

//485通道配置文件(全路径)
#define    AC_485_INI_FILE    "/data/app/desktopGui/safemode.ini"

#define    DB_CLIENT_MAX       20     //客户端最大个数
DB_AGENT   gDBAgent = 0;              //和数据中心交互的agent
DB_CLIENT  gDataCenterClient = 0;     //和数据中心交互的句柄(保存交采数据至数据中心)
DB_CLIENT  gDbUdpClient = 0;          //和数据中心交互的句柄(UDP专用)

//主动上报消息INDEX
uint16  ActiveInfoIndex = 0;                 //主动上报消息INDEX
pthread_mutex_t    ActiveInfoIndexLock;      //主动上报消息INDEX锁

//unix udp 绑定的文件名称(全路径)
#define    UDP_MSG_PATH    "/tmp/dev/"
char *gUdpFileName = "/tmp/dev/acmeter";
char *gUdpFileNameTest = "/tmp/dev/testfile";

//mqtt ip地址与端口号
char mqttIP[100] = {0};
int mqttport = 0;

//是否关注的app均已注册
uint8 gMapManagerAppLoginFlag = 1;

uint8 gDBCenterAppLoginFlag = 0;

//本app是否已经注册成功
uint8 bAppSelfLoginState = 0;

//是否支持转发485串口报文
int gSupportRelay485Flag = 1;

//appname: app英文名称(用于查询appid)
char *AppIdName = "acMeter";

//appid: 对应app的ID信息
char  AppId[APP_ID_LEN] = {0};

OOP_OCTETVAR16_T    tmnAddr;        			 //终端通信地址
OOP_OCTETVAR64_T    MAC;                  	     //校验码
DB_CLIENT  gRptClient = 0;            //事件上报存储用句柄

#if TID_TIME == 1
THREAD_ARG_T g_ThreadTimeArg =
{
    "pthread_time_chk",  //线程名字
    NULL,                //其他参数
    0,                   //维护计数
};
#endif

#if TID_SLEF_CHECK == 1
THREAD_ARG_T g_ThreadSelfArg =
{
    "pthread_self_chk",  //线程名字
    NULL,                //其他参数
    0,                   //维护计数
};
#endif

THREAD_ARG_T g_ThreadMqttArg =
{
    "pthread_mqtt",       //线程名字
    NULL,                 //其他参数
    0,                    //维护计数
};

THREAD_ARG_T g_ThreadSampArg =
{
    "pthread_sample",      //线程名字
    NULL,                  //其他参数
    0,                     //维护计数
};

THREAD_ARG_T g_ThreadSaveArg =
{
    "pthread_save",        //线程名字
    NULL,                  //其他参数
    0,                     //维护计数
};

THREAD_ARG_T g_ThreadReportArg =
{
    "pthread_report",      //线程名字
    NULL,                  //其他参数
    0,                     //维护计数
};

THREAD_ARG_T g_ThreadUpdateArg =
{
    "db_update_entry",      //线程名字
    NULL,                   //其他参数
    0,                      //维护计数
};

THREAD_ARG_T g_ThreadTestArg =
{
    "pthread_test",         //线程名字
    NULL,                   //其他参数
    0,                      //维护计数
};

THREAD_ARG_T g_Thread485ModuleArg =
{
    "pthread_485module",     //线程名字
    NULL,                    //其他参数
    0,                       //维护计数
};

THREAD_ARG_T g_ThreadPowerArg =
{
    "pthread_power",         //线程名字
    NULL,                   //其他参数
    0,                      //维护计数
};


uint8 g_Arg_485comm[RUN_DOWN_NUM] = {PORT_485};
THREAD_ARG_T g_Thread485CommArg =
{
    "pthread_485comm",       //线程名字
    &g_Arg_485comm[0],       //其他参数
    0,                       //维护计数
};

MAIN_TRHEAD_T g_tMainThread[] =
{
    /*是否启动 线程ID  入参                    线程入口函数           是否维护*/
    #if TID_TIME == 1
    {TRUE,    0,     &g_ThreadTimeArg,         pthread_time_chk,     TRUE},
    #endif
    #if TID_SLEF_CHECK == 1
    {TRUE,    0,     &g_ThreadSelfArg,         pthread_self_chk,     TRUE},
    #endif
    {TRUE,    0,     &g_ThreadMqttArg,         pthread_mqtt,         TRUE},
    {TRUE,    0,     &g_ThreadSampArg,         pthread_sample,       TRUE},
    {TRUE,    0,     &g_ThreadSaveArg,         pthread_save,         TRUE},
    {TRUE,    0,     &g_ThreadReportArg,       pthread_report,       TRUE},
    {TRUE,    0,     &g_ThreadUpdateArg,       db_update_entry,      FALSE},
    {FALSE,   0,     &g_ThreadTestArg,         pthread_test,         TRUE},
    //#ifdef PRODUCT_ECU
    #if 0
    {TRUE,    0,     &g_Thread485ModuleArg,    pthread_485module,    TRUE},
    {TRUE,    0,     &g_Thread485CommArg,      pthread_485comm,      TRUE},
    #else
    {TRUE,    0,     &g_Thread485CommArg,      pthread_uartManager,  TRUE},
    #endif

    {TRUE,    0,     &g_ThreadPowerArg,        pthread_check_poweroff,  FALSE},
};
uint8 g_MainTaskNum = sizeof(g_tMainThread)/sizeof(MAIN_TRHEAD_T);

extern void sta_time_update_fset(BOOLEAN isUpdate);

/*********************************************************************
 功能描述：其他初始化
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void ac_other_init(void)
{
    /* 判断是否需要文件拷贝 */
    if (0 != access(UDP_MSG_PATH, F_OK))
    {
        INIT_FMT_DEBUG("%s non-existent!", UDP_MSG_PATH);
        if (0 != mkdir(UDP_MSG_PATH, 0777))
        {
            INIT_FMT_DEBUG("%s create failed!", UDP_MSG_PATH);
        }
        else
        {
            INIT_FMT_DEBUG("%s create ok", UDP_MSG_PATH);
        }
    }

    return;
}

/*********************************************************************
 功能描述：数据中心交采数据初始化事件处理
 输入参数：buf[0]:0-数据初始化，1-参数初始化；buf[1]:逻辑地址
 输出参数：无
 函数返回值：无
********************************************************************/
void db_ac_init_deal(uint8* buf, uint16 len)
{
    ACMQTT_FMT_DEBUG("db_ac_init_deal. 收到数据中心交采数据初始化事件choice[%d] logicAddr[%d] len[%d]\n",
        buf[0], buf[1], len);

    if (len >= 2)
    {
        //choice 0 数据初始化, 逻辑设备号 1
        if ((0 == buf[0]) && (1 == buf[1]))
        {
            //不需要与历史数据对比, 直接将当前数据往数据中心存一份
            gSaveFlag.bFirstSaveEnergy = TRUE;    //程序启动, 第一次存储标志, 电量
            gSaveFlag.bFirstSaveDemand = TRUE;    //程序启动, 第一次存储标志, 需量
            gSaveFlag.bFirstSaveRealtime = TRUE;  //程序启动, 第一次存储标志, 实时数据
            gSaveFlag.bFirstSaveHarmonic = TRUE;  //程序启动, 第一次存储标志, 谐波
            gSaveFlag.bFirstSaveSpecial = TRUE;   //程序启动, 第一次存储标志, 特殊数据
        }
    }
}

/*********************************************************************
 功能描述：交采数据采样与统计线程
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void *pthread_sample(void *arg)
{
    INIT_FMT_DEBUG("pthread_sample start...PID[%d]\n", (int)getpid());

    THREAD_ARG_T *tArg = (THREAD_ARG_T *)arg;
    //线程名
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    //nice(-5);        // 提高优先级
    if (gSampPrtl == 1)
    {
        ac_calc_deal_698(&tArg->cnt);
    }
    else
    {
        ac_calc_deal(&tArg->cnt);
    }

    pthread_exit(NULL);
}

/*******************************************************************************
* 函数名称: term_addres_update
* 函数功能: 更新终端地址
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 0-成功； 其他-失败
*******************************************************************************/
int term_addres_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int         ret = -1;
    uint32      len = 0;
    NOMAL_OAD_T nomalOAD;

    memset(&nomalOAD, 0, sizeof(NOMAL_OAD_T));
    memset(&tmnAddr, 0, sizeof(OOP_OCTETVAR16_T));
    memset(&MAC, 0, sizeof(OOP_OCTETVAR64_T));

    //读取终端地址
    nomalOAD.oad.value = 0x40010200;
    ret = db_read_nomal(clientid, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&tmnAddr, &len);
    if (0 == ret && len == sizeof(OOP_OCTETVAR16_T))
    {
        MAC.nNum = 6;
        memcpy(MAC.value, tmnAddr.value, MAC.nNum);
        ACDB_FMT_DEBUG("更新终端地址 校验MAC=%02X %02X %02X %02X %02X %02X \n", MAC.value[0],
            MAC.value[1], MAC.value[2], MAC.value[3], MAC.value[4], MAC.value[5]);
    }
    else
    {
        ACDB_FMT_DEBUG("读终端地址失败! OAD=%08X, ret=%d, len=%d\n", nomalOAD.oad.value, ret, len);
    }

    return ret;
}

/*********************************************************************
 功能描述：数据中心读写线程
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void *pthread_save(void *arg)
{
    INIT_FMT_DEBUG("pthread_save start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T *tArg = (THREAD_ARG_T *)arg;

    //线程名
    prctl(PR_SET_NAME, tArg->threadName);

    uint16 nEnergySecCounter = 0;  //电量采集秒计时器
    uint16 nDemandSecCounter = 0;  //需量采集秒计时器
    uint16 nHarmonicSecCounter = 0;  //谐波采集秒计时器
    uint16 nRealtimeSecCounter = 0;  //实时数据采集秒计时器
    uint16 nSpecialSecCounter = 0;  //特殊数据采集秒计时器
    uint8  nTaskType = SAVE_TYPE_NULL;  //任务类型

    uint16 nEnergySecCycle = 3;  //电量存储周期
    uint16 nDemandSecCycle = 5;  //需量存储周期
    uint16 nHarmonicSecCycle = 5;  //谐波存储周期
    uint16 nRealtimeSecCycle = 1;  //实时数据存储周期
    uint16 nSpecialSecCycle = 3;  //特殊数据存储周期

    uint8  bTimeCheckflag = FALSE;

    #if TID_MAGNETIC == 1
    //记录上次磁场状态
    int magneticStatusLast = 1;
    //用于防抖
    int magneticStatusTmp = 1;
    #endif

    tArg->cnt = 0;

    //时间管理初始化
    SaveTimeManageInit();

    //等待数据中心启动
    while (gDBCenterAppLoginFlag != TRUE)
    {
        tArg->cnt = 0;
        sleep(2);
    }

    //初始化数据中心句柄
    gDataCenterClient  = db_client_create(DB_AC_AGENT, DB_AC_CLIENT_SAVE);

    if (gDataCenterClient <= 0)
    {
        ACDB_FMT_TRACE("pthread_save 数据中心句柄gDatyaCenterHandle创建失败, 返回!\n");
        return NULL;
    }

    //更新交采接线方式
    ac_power_type_update();

    #if TID_MAGNETIC == 1
    //获取终端地址
    term_addres_update(gDataCenterClient, 0, 0, 0x40010200);

    //初始化数据磁场状态
    magneticStatusTmp = get_mapnetic_status();
    if (magneticStatusTmp < 0)
    {
        magneticStatusTmp = 1;
    }

    #if defined PRODUCT_ZCU_1
    magnetic_dev_init();
    #endif
    #endif

    while(1)
    {
        tArg->cnt = 0;                   // 用于检测线程运行是否正常
        SaveTimeManageStart();           // 一轮时间管理开始
        nTaskType = SAVE_TYPE_NULL;      // 任务类型

        if ((Bcd2Hex(gSaveCommonData.CurTime.hh) % 8 == 0) &&
            (Bcd2Hex(gSaveCommonData.CurTime.nn) == 0) &&
            (Bcd2Hex(gSaveCommonData.CurTime.ss) >= 19) &&
            (bTimeCheckflag == TRUE))

        {
            bTimeCheckflag = FALSE;
            //送检分支代码关闭8小时对时一次的逻辑
            #if 0
            g_nAcSetDateFlag = 1;
            g_nAcSetTimeFlag = 1;
            #endif
        }

        if (Bcd2Hex(gSaveCommonData.CurTime.ss) < 19)
        {
            bTimeCheckflag = TRUE;
        }

        // 过秒处理
        if (gSaveTimeFlag.BitSect.Second)
        {
            nEnergySecCounter ++;
            nDemandSecCounter ++;
            nHarmonicSecCounter ++;
            nRealtimeSecCounter ++;
            nSpecialSecCounter ++;

            //nOccurSecCouter ++;
            #if TID_MAGNETIC == 1
            proc_magnetic_chk(&magneticStatusLast, &magneticStatusTmp);
            #endif
        }

        //过分处理
        if (gSaveTimeFlag.BitSect.Minute)
        {
            sta_volt_qualifyrate_calc();
            if((access(DELAY_FILE_PATH,F_OK)==0))
            {
                if(gdelay == 0)
                {
                    gdelay = 1;
                    gdelaytime = DELAYTIME;
                    UPDATE_FMT_DEBUG("交采检测到开始减压\n");
                }
            }else if((access(DELAY_FILE_PATH2,F_OK)==0))
            {
                if(gdelay != 2)
                {
                    gdelay = 2;
                    gdelaytime = DELAYTIME_MORE;
                    UPDATE_FMT_DEBUG("进一步减压\n");

                }
            }
            else
            {
                if(gdelay != 0)
                {
                    gdelay = 0;
                    gdelaytime = 0;
                }
            }
        }

        //检测任务类型
        if (nDemandSecCounter >= nDemandSecCycle)
        {
            nDemandSecCounter = 0;
            nTaskType = SAVE_TYPE_DEMAND;  //任务类型 需量
            ACDB_FMT_TRACE("pthread_save 生成任务 存储需量数据 存储周期 nDemandSecCycle[%d]\n", nDemandSecCycle);
        }
        else if (nHarmonicSecCounter >= nHarmonicSecCycle)
        {
            nHarmonicSecCounter = 0;
            nTaskType = SAVE_TYPE_HARMONIC;  //任务类型 谐波
            ACDB_FMT_TRACE("pthread_save 生成任务 存储谐波数据 存储周期 nHarmonicSecCycle[%d]\n", nHarmonicSecCycle);
        }
        else if (nEnergySecCounter >= nEnergySecCycle)
        {
            nEnergySecCounter = 0;
            nTaskType = SAVE_TYPE_ENERGY;  //任务类型 电量
            ACDB_FMT_TRACE("pthread_save 生成任务 存储电量数据 存储周期 nEnergySecCycle[%d]\n", nEnergySecCycle);
        }
        else if (nSpecialSecCounter >= nSpecialSecCycle)
        {
            nSpecialSecCounter = 0;
            nTaskType = SAVE_TYPE_SPECIAL;  //任务类型 临时特殊数据
            ACDB_FMT_TRACE("pthread_save 生成任务 存储特殊数据 存储周期 nSpecialSecCycle[%d]\n", nSpecialSecCycle);
        }
        else if (nRealtimeSecCounter >= nRealtimeSecCycle)
        {
            nRealtimeSecCounter = 0;
            nTaskType = SAVE_TYPE_REALTIME;  //任务类型 实时数据
            ACDB_FMT_TRACE("pthread_save 生成任务 存储实时数据 存储周期 nRealtimeSecCycle[%d]\n", nRealtimeSecCycle);
        }

        //备份实时数据至中间变量
        memcpy(&MidTermSharedData, &TermSharedData, sizeof(TERM_SHARE_DATA_TYPE));
        memcpy(&gMidPriMeasurePara, &gPriMeasurePara, sizeof(PRIVTE_MEASURE_TYPE));

        //测试磁场干扰事件 begin
        /*
        if(nOccurSecCouter > 35)
        {
            MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7 = 1;
            ACDB_FMT_TRACE("pthread_save()模拟生成磁场干扰事件 > 35 MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7=%d, nOccurSecCouter=%d \n",
                MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7, nOccurSecCouter);
            nOccurSecCouter = 0;
        }
        else if(nOccurSecCouter > 30)
        {
            MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7 = 1;
            ACDB_FMT_TRACE("pthread_save()模拟生成磁场干扰事件 > 30 MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7=%d, nOccurSecCouter=%d \n",
                MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7, nOccurSecCouter);
        }
        else
        {
            MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7 = 0;
            ACDB_FMT_TRACE("pthread_save()模拟生成磁场干扰事件 else MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7=%d, nOccurSecCouter=%d \n",
                MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7, nOccurSecCouter);
        }
        */
        //测试磁场干扰事件 end

        //周期性将交采数据写入数据中心
        Save07AcIntoDataCenter(nTaskType);

        //冻结处理，在数据写入之后
        sta_frozen_proc();

        usleep(500000);
        SaveTimeManageEnd();                 // 一轮时间管理结束
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

    pthread_exit(NULL);

}

/**********************************************************************
* @name：      get_head_from_buf
* @brief：     找合法的报文68开头，主要指回码报文
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint8* get_645ptr_from_buf(uint8* buf, uint32 maxLen, uint32* offset, uint32* outLen)
{
    int i;
    uint8* head;
    uint8* pBuf = NULL;
    uint32 off = 0;
    uint8  frameCs;
    uint16 frameLen;
    uint16 DataLen = 0;

    if ((buf == NULL) || (maxLen <= 9))
        return NULL;

    head = buf;
    for (i = 0; i < maxLen - 9; i++)
    {
        if ((*head == 0x68) && (*(head + 7) == 0x68))
        {
            pBuf = head;

            //先用2个字节长度校验
            DataLen = pBuf[9] + (pBuf[10] << 8);
            frameLen = 0x0d + DataLen;
            if (frameLen > maxLen)
            {
                //用一个字节长度校验
                DataLen = pBuf[9];
                frameLen = 0x0c + DataLen;
                if (frameLen > maxLen)
                {
                    ACUDP_FMT_DEBUG("frameLen=%d, bufLen=%d \n", frameLen, maxLen);
                    continue;
                }

                frameCs = CalCs(pBuf, frameLen - 2);

                if ((pBuf[0] == 0x68)
                    && (pBuf[7] == pBuf[0]) /*-北京系统电表地址补0或AA可变-*/
                    && (pBuf[frameLen - 2] == frameCs)
                    && (pBuf[frameLen - 1] == 0x16))
                {
                    *offset = off;
                    *outLen = frameLen;
                    ACUDP_FMT_TRACE("get_645ptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                    return head;
                }
                else
                {
                    continue;
                }
            }

            frameCs = CalCs(pBuf, frameLen - 2);

            if ((pBuf[0] == 0x68)
                && (pBuf[7] == pBuf[0]) /*-北京系统电表地址补0或AA可变-*/
                && (pBuf[frameLen - 2] == frameCs)
                && (pBuf[frameLen - 1] == 0x16))
            {
                *offset = off;
                *outLen = frameLen;
                ACUDP_FMT_TRACE("get_645ptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                return head;
            }
            else
            {
                //用一个字节长度校验
                DataLen = pBuf[9];
                frameLen = 0x0c + DataLen;
                if (frameLen > maxLen)
                {
                    ACUDP_FMT_DEBUG("frameLen=%d, bufLen=%d \n", frameLen, maxLen);
                    continue;
                }

                frameCs = CalCs(pBuf, frameLen - 2);

                if ((pBuf[0] == 0x68)
                    && (pBuf[7] == pBuf[0]) /*-北京系统电表地址补0或AA可变-*/
                    && (pBuf[frameLen - 2] == frameCs)
                    && (pBuf[frameLen - 1] == 0x16))
                {
                    *offset = off;
                    *outLen = frameLen;
                    ACUDP_FMT_TRACE("get_645ptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                    return head;
                }
                else
                {
                    continue;
                }
            }
        }
        off++;
        head++;
    }
    return NULL;
}

/**********************************************************************
* @name：      get_head_from_buf
* @brief：     找合法的报文68开头，主要指回码报文
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2019-12-6
**********************************************************************/
uint8* get_698ptr_from_buf(uint8* buf, uint32 maxLen, uint32* offset, uint32* outLen)
{
    int i;
    uint8* head;
    uint32 len = 0;
    uint32 off = 0;

    if ((buf == NULL) || (maxLen <= 9))
        return NULL;

    head = buf;
    for (i = 0; i < maxLen - 9; i++)
    {
        if (*head == 0x68)
        {
            memcpy(&len, head + 1, 2);
            //防止数据溢出
            if ((i + len + 2) <= maxLen)
            {
                if (app_check_message((uint8*)head, len + 2) == 0)
                {
                    *offset = off;
                    *outLen = len + 2;

                    ACUDP_FMT_TRACE("get_698ptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                    return head;
                }
            }
        }
        off++;
        head++;
    }
    return NULL;
}

/*
*********************************************************************
* @name      : ac_get_msg_type
* @brief     ：获取报文类型，目前认为不存在混合报文
* @param[in] ：buf - 报文
*              len - 报文长度
* @param[out]：
* @return    ：类型
*********************************************************************
*/
MSG_TYPE_E ac_get_msg_type(uint8 *buf, uint32 len)
{
    uint8* ptr = NULL;
    uint32 outLen = 0;
    uint32 offset = 0;

    ptr = get_698ptr_from_buf(buf, len, &offset, &outLen);
    if (ptr != NULL)
    {
        return MSG698;
    }

    ptr = get_645ptr_from_buf(buf, len, &offset, &outLen);
    if (ptr != NULL)
    {
        return MSG645;
    }

    return MSGAPDU;
}

/*
*********************************************************************
* @name      : pthread_report_698jc
* @brief     ：交采数据UDP上报服务线程(透传到698交采)
* @param[in] ：
* @param[out]：
* @return    ：无
*********************************************************************
*/
void *pthread_report_698jc(void *arg)
{
    INIT_FMT_DEBUG("pthread_report_698jc start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T *tArg = (THREAD_ARG_T *)arg;

    //线程名
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    int8    SocketState = ACIPC_INIT;        //环回socket运行状态
    uint8   RecvBuf[MAX_UDP_BUFF_LEN] = {0}; //socket接收缓冲区
    uint8   SendBuf[MAX_UDP_BUFF_LEN] = {0}; //socekt发送缓冲区
    uint8   outMsg[APP_RECV_MSG_MAXLEN] = {0}; //发送IPC缓存
    int32   RecvLen = 0;                        //接收数据长度
    int32   SendLen = 0;                        //发送数据长度
    int     ret = -1;

    MSG_TYPE_E msgType = MSGAPDU;
    uint8   *ptr = NULL;
    uint32  outLen = 0;
    uint32  offset = 0;

    int     UdpServiceFd = -1;       //unix udp扩展消息句柄
    char    SenderName[100] = {0};  //发送者
    int     delay = 0;

    //698帧头结构体
    FRAME_HEAD_T tFrame = {0};
    uint8        Addr[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

    //初始化数据中心句柄
    gDbUdpClient  = db_client_create(DB_AC_AGENT, DB_AC_CLIENT_UDP);

    if (gDbUdpClient <= 0)
    {
        ACDB_FMT_TRACE("pthread_report_698jc 数据中心句柄gDbUdpClient创建失败, 返回! \n");
    }

    while (1)
    {
        tArg->cnt = 0;                    // 用于检测线程运行是否正常

        switch (SocketState)
        {
            case ACIPC_INIT:
            {
                //udp句柄初始化
                UdpServiceFd = task_pfmsg_init(gUdpFileName);

                ACUDP_FMT_TRACE("pthread_report_698jc ACIPC_INIT... UdpServiceFd=%d \n", UdpServiceFd);
                if(UdpServiceFd < 0)
                {
                    ACUDP_FMT_DEBUG("pthread_report_698jc ACIPC_INIT err UdpServiceFd[%d]\n", UdpServiceFd);
                    sleep(1);
                }
                else
                {
                    //设置超时时间
                    task_msg_settime(UdpServiceFd, 0, 10000);

                    //转到接收
                    SocketState = ACIPC_RECV;
                }

                break;
            }
            case ACIPC_RECV:
            {
                //环回进行数据接收
                RecvLen = 0;
                memset(RecvBuf,0x00,sizeof(RecvBuf));
                SendLen = 0;
                memset(SendBuf,0x00,sizeof(SendBuf));

                //清空队列
                while ((SendLen = PlcReadQueue(pUdpSendDown, SendBuf) > 0));

                RecvLen = task_pfmsg_recv(&UdpServiceFd, RecvBuf, MAX_UDP_BUFF_LEN, SenderName, sizeof(SenderName));
                if (RecvLen > 0)
                {
                    ACUDP_BUF_DEBUG(RecvBuf, RecvLen, "recv from %s:", SenderName);

                    msgType = ac_get_msg_type(RecvBuf, RecvLen);
                    if (msgType == MSG698)
                    {
                        //目前认为不存在混合报文，多条报文一起，也任务是同种报文，不然不好处理
                        while ((ptr = get_698ptr_from_buf(RecvBuf, RecvLen, &offset, &outLen)) != NULL)
                        {
                            SendLen = outLen;
                            memcpy(outMsg, ptr, outLen);

                            RecvLen -= (offset + outLen);
                            memmove(RecvBuf, (ptr + outLen), RecvLen);

                            ret = PlcWriteQueue(pUdpRecvDown, outMsg, SendLen);
                            if (ret == 0)
                            {
                                ACUDP_BUF_DEBUG(outMsg, SendLen, "msg[%d] send queue full. outMsg:\n", msgType);
                            }
                            else
                            {
                                ACUDP_BUF_DEBUG(outMsg, SendLen, "msg[%d] send jc module:", msgType);
                            }
                        }
                    }
                    else if (msgType == MSG645)
                    {
                        //目前认为不存在混合报文，多条报文一起，也任务是同种报文，不然不好处理
                        while ((ptr = get_645ptr_from_buf(RecvBuf, RecvLen, &offset, &outLen)) != NULL)
                        {
                            SendLen = outLen;
                            memcpy(outMsg, ptr, outLen);

                            RecvLen -= (offset + outLen);
                            memmove(RecvBuf, (ptr + outLen), RecvLen);

                            ret = PlcWriteQueue(pUdpRecvDown, outMsg, SendLen);
                            if (ret == 0)
                            {
                                ACUDP_BUF_DEBUG(outMsg, SendLen, "msg[%d] send queue full. outMsg:\n", msgType);
                            }
                            else
                            {
                                ACUDP_BUF_DEBUG(outMsg, SendLen, "msg[%d] send jc module:", msgType);
                            }
                        }
                    }
                    else
                    {
                        //上面RecvBuf返回的是完整的APDU，下面拼封装链路层头
                        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
                        tFrame.dir = 0;
                        tFrame.prm = 1;
                        tFrame.funCode = 3;
                        tFrame.apduLen = RecvLen;
                        tFrame.pApdu = RecvBuf;

                        //填写通配地址
                        tFrame.sAddr.len = 6;
                        memcpy(tFrame.sAddr.addr, Addr, 6);
                        tFrame.sAddr.type = 1;
                        tFrame.sAddr.logicAddr = 0;

                        tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
                        tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

                        SendLen = app_frame_to_package(&tFrame, outMsg, APP_RECV_MSG_MAXLEN - 2, TRUE);

                        ret = PlcWriteQueue(pUdpRecvDown, outMsg, SendLen);
                        if (ret == 0)
                        {
                            ACUDP_BUF_DEBUG(outMsg, SendLen, "send queue full. outMsg:\n");
                        }
                        else
                        {
                            ACUDP_BUF_DEBUG(outMsg, SendLen, "send jc module:");
                        }
                    }

                    //转到发送
                    SocketState = ACIPC_SEND;
                    delay = 0;
                    break;
                }
                else
                {
                    //对时处理，为兼容07规约，2个标记共存
                    if ((g_nAcSetDateFlag == 1) || (g_nAcSetTimeFlag == 1))
                    {
                        g_nAcSetDateFlag = 0;
                        g_nAcSetTimeFlag = 0;
                        if (app698msg_time_set() == DATA_SUCCESS)
                        {
                            ACUDP_FMT_DEBUG("set time ok\n");
                        }
                        else
                        {
                            ACUDP_FMT_DEBUG("set time err\n");
                        }
                    }
                }

                //转到发送
                SocketState = ACIPC_RECV;

                break;
            }

            case ACIPC_SEND:
            {
                SendLen = PlcReadQueue(pUdpSendDown, SendBuf);
                if(SendLen > 0)
                {
                    ACUDP_BUF_DEBUG(SendBuf, SendLen, "recv jc module:");

                    if (msgType != MSGAPDU)
                    {
                        ACUDP_BUF_DEBUG(SendBuf, SendLen, "msg[%d] send %s:", msgType, SenderName);
                        task_pfmsg_send(UdpServiceFd, SendBuf, SendLen, SenderName);
                    }
                    else
                    {
                        /* 合法性检查 */
                        ret = app_check_message(SendBuf, SendLen);
                        if (0 != ret)
                        {
                            ACUDP_BUF_DEBUG(SendBuf, SendLen, "app_check_message error[%d] udp recv:\n", ret);
                            break;
                        }

                        /* 解析帧头 */
                        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
                        ret = app_get_frame(SendBuf, SendLen, &tFrame, TRUE);
                        if (0 != ret)
                        {
                            ACUDP_BUF_DEBUG(SendBuf, SendLen, "app_get_frame error[%d] udp recv:\n", ret);
                            break;
                        }

                        ACUDP_BUF_DEBUG(tFrame.pApdu, tFrame.apduLen, "udp send %s:", SenderName);
                        task_pfmsg_send(UdpServiceFd, tFrame.pApdu, tFrame.apduLen, SenderName);
                    }
                }
                else
                {
                    delay ++;
                    if (delay < 180)
                    {
                        break;
                    }
                }

                //转入接收处理
                SocketState = ACIPC_RECV;
                delay = 0;

                break;
            }

            default:
            {
                SocketState = ACIPC_INIT;
                break;
            }

        }

        usleep(50000);                         //出让CPU资源
    }

    pthread_exit(NULL);
}

/*
*********************************************************************
* @name      : pthread_report_07jc
* @brief     ：交采数据UDP上报服务线程(07交采)
* @param[in] ：
* @param[out]：
* @return    ：无
*********************************************************************
*/
void *pthread_report_07jc(void *arg)
{
    INIT_FMT_DEBUG("pthread_report_07jc start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T *tArg = (THREAD_ARG_T *)arg;

    //线程名
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    int8    SocketState = ACIPC_INIT;           //环回socket运行状态
    uint8   RecvBuf[MAX_UDP_BUFF_LEN] = {0};    //socket接收缓冲区
    uint8   SendBuf[MAX_UDP_BUFF_LEN] = {0};    //socekt发送缓冲区
    uint8   outMsg[APP_RECV_MSG_MAXLEN] = {0};  //发送IPC缓存
    int32   RecvLen = 0;                        //接收数据长度
    int32   SendLen = 0;                        //发送数据长度
    int     ret = -1;

    MSG_TYPE_E msgType = MSGAPDU;
    uint8*  ptr = NULL;
    uint32  outLen = 0;
    uint32  offset = 0;

    int     UdpServiceFd = 0;  //unix udp扩展消息句柄
    char    SenderName[100] = {0};  //发送者
    int     delay = 0;

    memset(RecvBuf, 0x00, sizeof(RecvBuf));
    memset(SendBuf, 0x00, sizeof(SendBuf));

    //初始化数据中心句柄
    gDbUdpClient  = db_client_create(DB_AC_AGENT, DB_AC_CLIENT_UDP);

    if(gDbUdpClient <= 0)
    {
        ACDB_FMT_TRACE("pthread_report_07jc 数据中心句柄gDbUdpClient创建失败, 返回! \n");
    }

    while(1)
    {
        tArg->cnt = 0;                    // 用于检测线程运行是否正常

        switch (SocketState)
        {
            case ACIPC_INIT:
            {
                //udp句柄初始化
                UdpServiceFd = task_pfmsg_init(gUdpFileName);

                ACUDP_FMT_TRACE("pthread_report_07jc ACIPC_INIT... UdpServiceFd=%d \n", UdpServiceFd);
                if(UdpServiceFd < 0)
                {
                    ACUDP_FMT_TRACE("pthread_report_07jc ACIPC_INIT err UdpServiceFd[%d]\n", UdpServiceFd);
                    sleep(1);
                }
                else
                {
                    //设置超时时间
                    task_msg_settime(UdpServiceFd, 0, 10000);

                    //转到接收
                    SocketState = ACIPC_RECV;
                }

                break;
            }
            case ACIPC_RECV:
            {
                //环回进行数据接收
                msgType = MSGAPDU;
                RecvLen = 0;
                memset(RecvBuf,0x00,sizeof(RecvBuf));
                SendLen = 0;
                memset(SendBuf,0x00,sizeof(SendBuf));
                memset(SenderName,0x00,sizeof(SenderName));

                RecvLen = task_pfmsg_recv(&UdpServiceFd, RecvBuf, MAX_UDP_BUFF_LEN, SenderName, sizeof(SenderName));
                if (RecvLen > 0)
                {
                    ACUDP_BUF_DEBUG(RecvBuf, RecvLen, "udp recv %s:", SenderName);

                    msgType = ac_get_msg_type(RecvBuf, RecvLen);
                    if (msgType == MSG645)
                    {
                        //目前认为不存在混合报文，多条报文一起，也任务是同种报文，不然不好处理
                        while ((ptr = get_645ptr_from_buf(RecvBuf, RecvLen, &offset, &outLen)) != NULL)
                        {
                            SendLen = outLen;
                            memcpy(outMsg, ptr, outLen);

                            RecvLen -= (offset + outLen);
                            memmove(RecvBuf, (ptr + outLen), RecvLen);

                            ret = PlcWriteQueue(pUdpRecvDown, outMsg, SendLen);
                            if (ret == 0)
                            {
                                ACUDP_BUF_DEBUG(outMsg, SendLen, "msg[%d] send queue full. outMsg:\n", msgType);
                            }
                            else
                            {
                                ACUDP_BUF_DEBUG(outMsg, SendLen, "msg[%d] send jc module:", msgType);
                            }
                        }
                    }
                    else
                    {
                        //解析apdu报文
                        SendLen = udp_apdu_analyse(RecvBuf, RecvLen, SendBuf, MAX_UDP_BUFF_LEN);
                    }
                }

                if(SendLen > 0)
                {
                    //转到发送
                    SocketState = ACIPC_SEND;
                    delay = 0;
                }

                break;
            }

            case ACIPC_SEND:
            {
                if (msgType == MSG645)
                {
                    SendLen = 0;
                    SendLen = PlcReadQueue(pUdpSendDown, SendBuf);
                    if (SendLen > 0)
                    {
                        ACUDP_BUF_DEBUG(SendBuf, SendLen, "msg[%d] recv jc module:", msgType);
                    }
                    else
                    {
                        delay++;
                        if (delay < 180)
                        {
                            break;
                        }
                    }
                }

                if (SendLen > 0)
                {
                    ACUDP_BUF_DEBUG(SendBuf, SendLen, "udp send %s:", SenderName);
                    task_pfmsg_send(UdpServiceFd, SendBuf, SendLen, SenderName);
                }

                //转入接收处理
                SocketState = ACIPC_RECV;
                delay = 0;
                break;
            }

            default:
            {
                SocketState = ACIPC_INIT;
                break;
            }

        }

        usleep(50000);                         //出让CPU资源
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

    pthread_exit(NULL);
}

/*
*********************************************************************
* @name      : pthread_report
* @brief     ：自适应698和07交采
* @param[in] ：
* @param[out]：
* @return    ：无
*********************************************************************
*/
void *pthread_report(void *arg)
{
    THREAD_ARG_T* tArg = (THREAD_ARG_T*)arg;

    //如果没有招读到交采版本号，则等待
    while (isJCNeedGetVer())
    {
        tArg->cnt = 0;
        usleep(100000);
    }

    if (gSampPrtl == 1)
    {
        pthread_report_698jc(arg);
    }
    else
    {
        pthread_report_07jc(arg);
    }

    return NULL;
}

/*********************************************************************
 功能描述：交采数据MQTT通信线程
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void *pthread_mqtt(void *arg)
{
    INIT_FMT_DEBUG("pthread_mqtt() start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T* tArg = (THREAD_ARG_T*)arg;

    //线程名
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    struct timespec LastLoginTime = {0};
    struct timespec CurrentTime = {0};
    static uint32 LastLoginTimeSec = 0;
    uint32 CurrentTimeSec = 0;
    int8    SocketState = ACIPC_INIT;        //环回socket运行状态
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socket接收缓冲区
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socekt发送缓冲区
    uint8   SendInfoBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socekt发送缓冲区
    int32   RecvLen = 0;                        //接收数据长度
    uint32  sleepus = 10000;
    int     ret;

    MSG_INFO_T *ptMsgInfo = NULL;

    memset(RecvBuf, 0x00, sizeof(RecvBuf));
    memset(SendBuf, 0x00, sizeof(SendBuf));
    memset(SendInfoBuf, 0x00, sizeof(SendInfoBuf));

    INFO_DATA_T stInfoData;  //消息
    memset(&stInfoData, 0x00, sizeof(stInfoData));

    uint16 nMapManagerSendIndex = 0;//发送消息index
    time_t MapManagerLastCheckTime;//查询时间
    MapManagerLastCheckTime = abs(time(NULL) - 5);//查询时间

    uint16 nDBCenterSendIndex = 0;//发送消息index
    time_t DBCenterLastCheckTime;//查询时间
    DBCenterLastCheckTime = abs(time(NULL) - 5);//查询时间

    while(1)
    {
        tArg->cnt = 0;                    // 用于检测线程运行是否正常
        sleepus = 10000;

        switch (SocketState)
        {
            case ACIPC_INIT:
            {
                //printx(" void *pthread_mqtt(void *arg)...case IPC_SOCK_INIT: \n");

                if(0 == gMapManagerAppLoginFlag)//查看模组管理器是否注册成功
                {
                    if( abs(time(NULL) - MapManagerLastCheckTime) > 2 )
                    {
                        ACMQTT_FMT_TRACE("查询模组管理器注册状态 \n");
                        MapManagerLastCheckTime = time(NULL);
                        check_app_login_state(mapManagerName, &nMapManagerSendIndex);
                    }
                    SocketState = ACIPC_RECV;

                }
                else if(0 == gDBCenterAppLoginFlag)//查看数据中心是否注册成功
                {
                    if( abs(time(NULL) - DBCenterLastCheckTime) > 2 )
                    {
                        ACMQTT_FMT_TRACE("查询数据中心注册状态 \n");
                        DBCenterLastCheckTime = time(NULL);
                        check_app_login_state(dbName, &nDBCenterSendIndex);
                    }
                    SocketState = ACIPC_RECV;

                }
                else
                {
                    //转到注册
                    SocketState = ACIPC_LOGIN;
                }

                break;
            }
            case ACIPC_LOGIN:
            {
                //printx(" void *pthread_mqtt(void *arg)...case IPC_SOCK_LOGIN: \n");

                login_to_smiOS();
                clock_gettime(CLOCK_MONOTONIC, &LastLoginTime);
                LastLoginTimeSec = (uint32)LastLoginTime.tv_sec;
                SocketState = ACIPC_LOGIN_WAIT;

                break;
            }
            case ACIPC_LOGIN_WAIT:
            {
                clock_gettime(CLOCK_MONOTONIC, &CurrentTime);
                CurrentTimeSec = (uint32)CurrentTime.tv_sec;
                if(CurrentTimeSec - LastLoginTimeSec > 5)
                    SocketState = ACIPC_LOGIN;
                else
                    SocketState = ACIPC_RECV;

                break;
            }
            case ACIPC_RECV:
            {
                //环回进行数据接收
                memset(RecvBuf,0x00,sizeof(RecvBuf));
                RecvLen = mqtt_read(RecvBuf);

                if(RecvLen > 0)
                {
                    sleepus = 0;
                    //将消息解析成MSG_INFO_T
                    ret = msg_mqtt_split(RecvBuf,RecvLen, 0, 0, &ptMsgInfo);
                    if(ret < 0)
                    {
                        ACMQTT_BUF_DEBUG(RecvBuf, RecvLen, "消息解析失败\n");
                        break;
                    }

                    //打印完整消息内容
                    print_info_msg(ptMsgInfo);

                    if(ptMsgInfo->IID == 0x0000)
                    {
                        switch(ptMsgInfo->IOP)
                        {
                            case 0x0005:  //系统校时事件
                            {
                                ACMQTT_FMT_DEBUG("通过Mqtt收到系统校时事件\n");
                                sta_time_update_fset(TRUE);
                                g_nAcSetDateFlag = 1;
                                g_nAcSetTimeFlag = 1;
                            }
                                break;
                            case 0x0010:  //心跳
                            {
                                 heart_beat(ptMsgInfo->MsgIndex);
                            }
                                break;
                            case 0x0013://版本信息
                            {
                                Appversion_Response(ptMsgInfo);
                            }
                                break;
                            case 0x0016://appid信息
                            {
                                appid_response(ptMsgInfo);
                            }
                                break;
                            default:
                                break;
                        }
                    }
                    else if(ptMsgInfo->IID == 0x0001)
                    {
                        switch (ptMsgInfo->IOP)
                        {
                            case 0x0010://注册响应
                            {
                                bAppSelfLoginState = 1;
                            }
                                break;
                            case 0x0013://查询指定App信息
                            {
                                if(0 == gMapManagerAppLoginFlag)//查看模组管理器是否注册成功
                                {
                                    gMapManagerAppLoginFlag = unpack_login_info(ptMsgInfo, mapManagerName, nMapManagerSendIndex);
                                }
                                else if(0 == gDBCenterAppLoginFlag)//查看数据中心是否注册成功
                                {
                                    gDBCenterAppLoginFlag = unpack_login_info(ptMsgInfo, dbName, nDBCenterSendIndex);
                                }
                            }
                                break;
                            default:
                                break;
                        }
                    }
                    else if(ptMsgInfo->IID == 0x0005)//数据中心
                    {
                        MESSAGE_INFO_T *ptMsg = NULL;
                        appmsg_split(RecvBuf,RecvLen, &ptMsg);
                        if(NULL != ptMsg)
                        {
                            switch (ptMsgInfo->IOP)
                            {
                                case 0x0002://数据初始化事件
                                {
                                    db_ac_init_deal(ptMsg->playload,ptMsg->msglen);
                                }
                                    break;
                                case 0x0010:
                                case 0x0011:
                                case 0x0012:
                                case 0x0013:
                                case 0x0014:
                                case 0x0015:
                                case 0x0016:
                                case 0x0020:
                                case 0x0021:
                                case 0x0030:
                                case 0x0031:
                                {
                                    db_agent_msg_entry(gDBAgent, ptMsg);
                                }
                                    break;

                                case 0x0001:
                                {
                                    db_update_msg_proc(ptMsg);
                                }break;

                                default:
                                    break;
                            }

                            free(ptMsg);
                            ptMsg = NULL;
                        }
                    }
                    else if(ptMsgInfo->IID == 0x1155)
                    {
                        switch(ptMsgInfo->IOP)
                        {
                        case 0x9090:  //查询厂商版本信息
                        {
                             Factoryversion_Response(ptMsgInfo);
                        }
                            break;
                        default:
                            break;
                        }
                    }
                    else if(ptMsgInfo->IID == 0x0004)//模组管理器
                    {
                        switch (ptMsgInfo->IOP)
                        {
                            case 0x0001://模块插拔事件
                            {
                                ModuleEventFun(ptMsgInfo->MsgData, ptMsgInfo->MsgLen);
                            }
                                break;
                            case 0x0010://查询模组信息响应
                            {
                                PlcWriteQueue(pModuleRecvUp, (uint8*)ptMsgInfo, sizeof(MSG_INFO_T)+ptMsgInfo->MsgLen);
                            }
                                break;
                            default:
                                break;
                        }
                    }
                    else if(ptMsgInfo->IID == 0xFFF0)//厂家自定义消息接口
                    {
                        switch (ptMsgInfo->IOP)
                        {
                            case 0x0001://交采参数初始化
                            {
                                ACMQTT_FMT_DEBUG("pthread_mqtt() 交采参数初始化 \n");
                                ac_param_initial(ptMsgInfo);
                            }
                                break;
                            case 0x0002://交采数据初始化
                            {
                                ACMQTT_FMT_DEBUG("pthread_mqtt() 交采数据初始化 \n");
                                ac_data_initial(ptMsgInfo);
                            }
                                break;
                            case 0x0003://交采需量初始化
                            {
                                ACMQTT_FMT_DEBUG("pthread_mqtt() 交采需量初始化 \n");
                                ac_demand_initial(ptMsgInfo);
                            }
                                break;
                            case 0x0004://升级执行系统指令
                            {
                                ACMQTT_FMT_DEBUG("pthread_mqtt() 升级执行系统指令 \n");
                                ac_system_deal(ptMsgInfo);
                            }
                                break;
                            default:
                                break;
                        }
                    }
                    else if ((ptMsgInfo->IID == MESSAGE_IID_UART) && (gSupportRelay485Flag > 0))//串口服务器操作
                    {
                        //串口服务代码从SCU移植过，存在入参不一致的情况，需要转换下接口
                        MESSAGE_INFO_T *ptMessageInfo = NULL;
                        uint32          messageLen = sizeof(MESSAGE_INFO_T) + ptMsgInfo->MsgLen;

                        ptMessageInfo = (MESSAGE_INFO_T *)malloc(messageLen);
                        memset(ptMessageInfo, 0, messageLen);

                        ac_485comm_data_convert(ptMsgInfo, ptMessageInfo);
                        ac_485comm_msg_proc(ptMessageInfo);

                        free(ptMessageInfo);
                        ptMessageInfo = NULL;
                    }

                    free(ptMsgInfo);
                    ptMsgInfo = NULL;
                }

                if( (0 == gMapManagerAppLoginFlag) || (0 == gDBCenterAppLoginFlag) )
                {
                    SocketState = ACIPC_INIT;
                }
                else
                {
                    //判断是否登录成功
                    if(bAppSelfLoginState == 1)
                    {
                        SocketState = ACIPC_SEND;
                    }
                    else
                    {
                        SocketState = ACIPC_LOGIN_WAIT;
                    }
                }

                break;
            }

            case ACIPC_SEND:
            {
                //转入进程间数据接收处理
                SocketState = ACIPC_RECV;

                break;
            }

            default:
            {
                SocketState = ACIPC_INIT;
                break;
            }

        }

        if (sleepus > 0)
        {
            usleep(sleepus);                         //出让CPU资源
        }
//        if(gdelay != 0)
//        {
//            usleep(gdelaytime);
//        }
    }

    pthread_exit(NULL);
}

/*********************************************************************
 功能描述：485模块信息维护线程
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void *pthread_485module(void *arg)
{
    INIT_FMT_DEBUG("pthread_485module() start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T* tArg = (THREAD_ARG_T*)arg;

    //线程名
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    while(1)
    {
        tArg->cnt = 0;

        if( (1 == gMapManagerAppLoginFlag) && (1 == gDBCenterAppLoginFlag) && (1 == bAppSelfLoginState) )
        {
            //查询模组信息
            if(gQueryAllModuleFlag == TRUE)
            {
                ACMQTT_FMT_DEBUG("pthread_485module() if(gQueryAllModuleFlag == TRUE)...\n");
                //查询所有模组信息
                if(GetAllModuleInfo() > 0)//判断是否有模块插入
                {
                    UpdateModuleInfo();
                    gQueryAllModuleFlag = FALSE;
                }
            }
            else if(gReloadModuleFlag[0] == 1)
            {
                ACMQTT_FMT_DEBUG("pthread_485module() else if(gReloadModuleFlag[0] == 1)...\n");
                //查询单个模组信息
                if(ModuleEventDeal() > 0)
                {
                    UpdateModuleInfo();
                }
            }
        }

        sleep(1);
    }

    pthread_exit(NULL);
}

/*********************************************************************
 功能描述：485模块串口通信线程
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void *pthread_485comm(void *arg)
{
    THREAD_ARG_T* tArg = (THREAD_ARG_T*)arg;

    uint8 pipe = *((uint8*)tArg->arg);
    INIT_FMT_DEBUG("pthread_485comm start. PID = %d. pipe = %d\n", (int)getpid(), pipe);

    //线程名
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    int8   SocketState = ACIPC_INIT;        //线程运行状态
    uint8  RecvBuf[MAX_QUEUE_BUFF_LEN];        //接收缓冲区
    uint8  SendBuf[MAX_QUEUE_BUFF_LEN];        //发送缓冲区
    int32  RecvLen = 0, SendLen = 0;           //接收、发送数据长度
    int    ret = -1;

    while(1)
    {
        tArg->cnt = 0;                      //用于检测线程运行是否正常

        switch (SocketState)
        {
        case ACIPC_INIT:                    //初始化串口
            if(gSupportRelay485Flag <= 0)
            {
                SocketState = ACIPC_INIT;
                sleep(5);  //不支持转发485串口报文, 线程空转
                break;
            }

            if(gDevHalInfo[pipe].isvalid == 0)
            {
                SocketState = ACIPC_INIT;
                sleep(1);  //模块信息无效, 线程空转
                break;
            }

            if((NULL == gDevHalInfo[pipe].dev) || (1 == gReloadDevFlag[pipe]))
            {
                //获取设备
                gDevHalInfo[pipe].dev = (UART_DEVICE_T*)hal_device_get(gDevHalInfo[pipe].deviceName);
                AC485COM_FMT_TRACE(pipe, "串口[%d] deviceName:%s, dev:%p, gReloadDevFlag=%d \n",
                    pipe, gDevHalInfo[pipe].deviceName, gDevHalInfo[pipe].dev, gReloadDevFlag[pipe]);

                // 打开设备
                ret = gDevHalInfo[pipe].dev->uart_param_set(gDevHalInfo[pipe].dev, 9600, 8, 1, PARITY_EVEN);  //TRUE:1, FALSE:0
                AC485COM_FMT_TRACE(pipe, "串口[%d] dev->uart_param_set() ret=%d \n", pipe, ret);
                if(ret == 0)
                {
                    SocketState = ACIPC_RECV;
                    AC485COM_FMT_TRACE(pipe, "串口[%d]打开成功DownTtlThread Success \n", pipe);
                    gReloadDevFlag[pipe] = 0;
                }
                else
                {
                    AC485COM_FMT_TRACE(pipe, "串口[%d]打开失败DownTtlThread Error \n", pipe);
                    sleep(2);
                    break;
                }
                sleep(1);
            }
            else
            {
                SocketState = ACIPC_RECV;
            }
            break;

        case ACIPC_RECV:
            if((gDevHalInfo[pipe].isvalid == 0) || (1 == gReloadDevFlag[pipe]))
            {
                SocketState = ACIPC_INIT;
                break;
            }

            memset(RecvBuf, 0x00, sizeof(RecvBuf));

            if(gDevHalInfo[pipe].dev == NULL)
            {
                SocketState = ACIPC_INIT;
                break;
            }
            RecvLen = gDevHalInfo[pipe].dev->uart_data_recv(gDevHalInfo[pipe].dev, RecvBuf, sizeof(RecvBuf));
            if (RecvLen > 0)
            {
                AC485COM_FMT_TRACE(pipe, "串口[%d]接收, deviceName=%s, RecvLen = %d \n", pipe, gDevHalInfo[pipe].deviceName, RecvLen);
                AC485COM_BUF_TRACE(pipe, RecvBuf, RecvLen, "报文");

                ret = PlcWriteQueue(pDealRecvDown[pipe], RecvBuf, RecvLen);
                AC485COM_FMT_TRACE(pipe, "PlcWriteQueue() ret=%d \n", ret);
            }

            SocketState = ACIPC_SEND;
            break;

        case ACIPC_SEND:
            if((gDevHalInfo[pipe].isvalid == 0) || (1 == gReloadDevFlag[pipe]))
            {
                SocketState = ACIPC_INIT;
                break;
            }

            SendLen = PlcReadQueue(pDealSendDown[pipe], SendBuf);
            if (SendLen > 0)
            {
                AC485COM_FMT_TRACE(pipe, "串口[%d]发送, deviceName=%s, SendLen = %d \n", pipe, gDevHalInfo[pipe].deviceName, SendLen);
                AC485COM_BUF_TRACE(pipe, SendBuf, SendLen, "报文");

                if(gDevHalInfo[pipe].dev == NULL)
                {
                    SocketState = ACIPC_INIT;
                    break;
                }
                ret = gDevHalInfo[pipe].dev->uart_data_send(gDevHalInfo[pipe].dev, SendBuf, SendLen);
                AC485COM_FMT_TRACE(pipe, "发送结果: pipe=%d, deviceName=%s, ret=%d \n", pipe, gDevHalInfo[pipe].deviceName, ret);

                //写通信日志
                memset(SendBuf, 0, sizeof(SendBuf));
            }
            SocketState = ACIPC_INIT;
            break;

        case ACIPC_CLOSE:
            ret = hal_device_release((HW_DEVICE *)gDevHalInfo[pipe].dev);
            AC485COM_FMT_TRACE(pipe, "hal_device_release() ret=%d \n", ret);
            SocketState = ACIPC_INIT;
            break;

        default:
            ret = hal_device_release((HW_DEVICE *)gDevHalInfo[pipe].dev);
            AC485COM_FMT_TRACE(pipe, "hal_device_release() ret=%d \n", ret);
            SocketState = ACIPC_INIT;
            break;
        }

        usleep(100000);                         //出让CPU资源

    }

    pthread_exit(NULL);
}

/*********************************************************************
 功能描述：485模块串口通信线程
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void *pthread_uartManager(void *arg)
{
    THREAD_ARG_T *tArg = (THREAD_ARG_T *)arg;

    uint8 pipe = *((uint8 *)tArg->arg);
    INIT_FMT_DEBUG("pthread_485comm start. PID = %d. pipe = %d\n", (int)getpid(), pipe);

    //线程名
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    int8   SocketState = ACIPC_INIT;              //线程运行状态
    // uint8  RecvBuf[MAX_QUEUE_BUFF_LEN];        //接收缓冲区
    uint8  SendBuf[MAX_QUEUE_BUFF_LEN] = {0};     //发送缓冲区
    uint8  SendBufTmp[MAX_QUEUE_BUFF_LEN] = {0};
    // int32  RecvLen = 0;
    int32  SendLen = 0;
    int    ret = -1;
    uint32 outLen = 0;
    uint32 offset = 0;
    uint8* ptr = NULL;

    while(1)
    {
        tArg->cnt = 0;                 //用于检测线程运行是否正常

        switch (SocketState)
        {
        case ACIPC_INIT:                       //初始化串口
            if (gSupportRelay485Flag <= 0)
            {
                //不需要转发485串口报文, 线程空转
                SocketState = ACIPC_INIT;
                sleep(5);
                break;
            }

            if (g_Comm485Info.isGetInfo == 0)
            {
                ret = ac_485comm_getinfo(pipe);
                if (ret == 0)
                {
                    AC485COM_FMT_DEBUG(pipe, "查询串口信息成功 ret[%d]\n", ret);
                    g_Comm485Info.isGetInfo = 1;
                }
                else
                {
                    AC485COM_FMT_DEBUG(pipe, "查询串口信息失败 ret[%d]\n", ret);
                    sleep(5);
                    break;
                }
                SocketState = ACIPC_INIT;
                break;
            }
            else if (g_Comm485Info.isSubcribe == 0)
            {
                ret = ac_485comm_subscribe(pipe);
                if (ret == 0)
                {
                    AC485COM_FMT_DEBUG(pipe, "订阅串口服务成功 ret[%d]\n", ret);
                    g_Comm485Info.isSubcribe = 1;
                }
                else
                {
                    AC485COM_FMT_DEBUG(pipe, "订阅串口服务失败 ret[%d]\n", ret);
                    SocketState = ACIPC_INIT;
                    sleep(5);
                    break;
                }
            }

            if ((UART_MODE_MASTER == g_Comm485Info.tModeInfo.mode) ||
                (UART_MODE_ONLY == g_Comm485Info.tModeInfo.mode && 0 != strcmp(g_Comm485Info.tModeInfo.onlyApp, AC_APP_NAME)))
            {
                //不需要转发485串口报文, 线程空转
                AC485COM_FMT_TRACE(pipe, "模式错误，线程空转\n");
                SocketState = ACIPC_INIT;
                sleep(5);
                break;
            }

            SocketState = ACIPC_RECV;
            break;

        case ACIPC_RECV:

            //串口收消息改到mqtt线程，等待串口服务发送数据通知
            SocketState = ACIPC_SEND;
            break;

        case ACIPC_SEND:
            SendLen = PlcReadQueue(pDealSendDown[0], SendBuf);
            if (SendLen > 0)
            {
                while ((ptr = get_07ptr_from_buf_n(SendBuf, SendLen, &offset, &outLen)) != NULL)
                {
                    memcpy(SendBufTmp, ptr, outLen);

                    SendLen -= (offset + outLen);
                    memmove(SendBuf, (ptr + outLen), SendLen);

                    ret = ac_485comm_send(pipe, SendBufTmp, outLen);
                    if (ret != 0)
                    {
                        AC485COM_BUF_DEBUG(pipe, SendBufTmp, outLen, "串口[%d]发送报文失败. ret[%d]", pipe, ret);
                    }
                    else
                    {
                        AC485COM_BUF_DEBUG(pipe, SendBufTmp, outLen, "  串口[%d]发送报文", pipe);
                    }
                }

                memset(SendBuf, 0, sizeof(SendBuf));
            }
            SocketState = ACIPC_INIT;
            break;

        case ACIPC_CLOSE:
            SocketState = ACIPC_INIT;
            break;

        default:
            SocketState = ACIPC_INIT;
            break;
        }

        usleep(20000);                         //出让CPU资源
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }

    }

    pthread_exit(NULL);
}

/*********************************************************************
 功能描述：测试线程
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void *pthread_test(void *arg)
{
    INIT_FMT_DEBUG("【测试】pthread_test() AC main start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T *tArg = (THREAD_ARG_T *)arg;

    //线程名
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    int8    SocketState = ACIPC_INIT;        //环回socket运行状态
    uint8   RecvBuf[MAX_UDP_BUFF_LEN] = {0}; //socket接收缓冲区
    uint8   SendBuf[MAX_UDP_BUFF_LEN] = {0}; //socekt发送缓冲区
    int32   RecvLen = 0;                        //接收数据长度
    int32   SendLen = 0;                        //发送数据长度

    int     UdpServiceFd = 0;  //unix udp扩展消息句柄
    char    SenderName[100] = {0};  //发送者

    BOOL    bOnlyTestGetNormal = FALSE;
    BOOL    bTestGetNormal = FALSE;
    BOOL    bTestGetNormalList = FALSE;
    BOOL    bTestGetNormalOther = FALSE;
    BOOL    bTestApduBuff = TRUE;

    uint16 i = 0;
    uint16 j = 0;
    uint16 k = 0;
    uint16 index = 0;

    memset(RecvBuf, 0x00, sizeof(RecvBuf));
    memset(SendBuf, 0x00, sizeof(SendBuf));

    while(1)
    {
        tArg->cnt = 0;                    // 用于检测线程运行是否正常


        switch (SocketState)
        {
            case ACIPC_INIT:
            {
                //udp句柄初始化
                UdpServiceFd = task_pfmsg_init(gUdpFileNameTest);
                ACUDP_FMT_TRACE("【测试】pthread_test() ACIPC_INIT... UdpServiceFd=%d \n", UdpServiceFd);
                if(UdpServiceFd < 0)
                {
                    ACUDP_FMT_TRACE("【测试】pthread_test() ACIPC_INIT... if(UdpServiceFd < 0) \n");
                    sleep(1);
                }
                else
                {
                    //转到接收
                    SocketState = ACIPC_SEND;
                }

                break;
            }

            case ACIPC_SEND:
            {
                sleep(1);  //报文发送间隔

                //测试方案1: GetRequestNormal测试
                if(bTestGetNormal)
                {
                    if(i >= gSaveTaskAllNum)
                    {
                        sleep(20);
                        i = 0;
                        if(!bOnlyTestGetNormal)
                        {
                            bTestGetNormal = FALSE;
                            bTestGetNormalList = TRUE;
                            break;
                        }
                    }

                    index = 0;
                    SendLen = 0;
                    memset(SendBuf,0x00,sizeof(SendBuf));

                    //组装APDU测试报文
                    SendBuf[index] = 0x05;  //get-request
                    index += 1;
                    SendBuf[index] = 0x01;  //GetRequestNormal
                    index += 1;
                    SendBuf[index] = 0x01;  //PIID
                    index += 1;
                    memcpy_r(&SendBuf[index], &gSaveTaskAll[i], 4);  //OAD
                    index += 4;
                    SendBuf[index] = 0x01;  //没有时间标签
                    index += 1;

                    if(index > 0)
                    {
                        SendLen = index;
                        ACUDP_FMT_TRACE("【测试】【GetNormal】pthread_test() ACIPC_SEND... SendLen=%d, DestName=%s \n", SendLen, gUdpFileName);
                        ACUDP_BUF_TRACE(SendBuf, SendLen, "【测试】【GetNormal】发送报文:");
                        task_pfmsg_send(UdpServiceFd, SendBuf, SendLen, gUdpFileName);
                    }

                    //跳到下一个OAD
                    i ++;
                }

                //测试方案2: GetRequestNormalList测试
                if(bTestGetNormalList)
                {
                    if(j >= 5)
                    {
                        sleep(20);
                        j = 0;
                        bTestGetNormal = TRUE;
                        bTestGetNormalList = FALSE;
                        break;
                    }

                    index = 0;
                    SendLen = 0;
                    memset(SendBuf,0x00,sizeof(SendBuf));

                    //组装APDU测试报文
                    SendBuf[index] = 0x05;  //get-request
                    index += 1;
                    SendBuf[index] = 0x02;  //GetRequestNormalList
                    index += 1;
                    SendBuf[index] = 0x01;  //PIID
                    index += 1;
                    if(j == 0)
                    {
                        ChangeLenToBuff(gSaveTaskEnergyNum, &index, &SendBuf[index]);  //seq of
                        for(k=0; k<gSaveTaskEnergyNum; k++)
                        {
                            memcpy_r(&SendBuf[index], &gSaveTaskEnergy[k], 4);  //OAD
                            index += 4;
                        }
                    }
                    else if(j == 1)
                    {
                        ChangeLenToBuff(gSaveTaskDemandNum, &index, &SendBuf[index]);  //seq of
                        for(k=0; k<gSaveTaskDemandNum; k++)
                        {
                            memcpy_r(&SendBuf[index], &gSaveTaskDemand[k], 4);  //OAD
                            index += 4;
                        }
                    }
                    else if(j == 2)
                    {
                        ChangeLenToBuff(gSaveTaskRealtimeNum, &index, &SendBuf[index]);  //seq of
                        for(k=0; k<gSaveTaskRealtimeNum; k++)
                        {
                            memcpy_r(&SendBuf[index], &gSaveTaskRealtime[k], 4);  //OAD
                            index += 4;
                        }
                    }
                    else if(j == 3)
                    {
                        ChangeLenToBuff(gSaveTaskHarmonicNum, &index, &SendBuf[index]);  //seq of
                        for(k=0; k<gSaveTaskHarmonicNum; k++)
                        {
                            memcpy_r(&SendBuf[index], &gSaveTaskHarmonic[k], 4);  //OAD
                            index += 4;
                        }
                    }
                    else if(j == 4)
                    {
                        ChangeLenToBuff(gSaveTaskContainerNum, &index, &SendBuf[index]);  //seq of
                        for(k=0; k<gSaveTaskContainerNum; k++)
                        {
                            memcpy_r(&SendBuf[index], &gSaveTaskContainer[k], 4);  //OAD
                            index += 4;
                        }
                    }

                    SendBuf[index] = 0x01;  //没有时间标签
                    index += 1;

                    if(index > 0)
                    {
                        SendLen = index;
                        ACUDP_FMT_TRACE("【测试】【GetNormalList】pthread_test() ACIPC_SEND... SendLen=%d, DestName=%s \n", SendLen, gUdpFileName);
                        ACUDP_BUF_TRACE(SendBuf, SendLen, "【测试】【GetNormalList】发送报文:");
                        task_pfmsg_send(UdpServiceFd, SendBuf, SendLen, gUdpFileName);
                    }

                    //跳到下一个OAD
                    j ++;
                }

                //测试方案3: GetRequestNormal测试
                if(bTestGetNormalOther)
                {
                    if(i >= gSubOadDataNum)
                    {
                        sleep(20);
                        i = 0;
                    }

                    index = 0;
                    SendLen = 0;
                    memset(SendBuf,0x00,sizeof(SendBuf));

                    //组装APDU测试报文
                    SendBuf[index] = 0x05;  //get-request
                    index += 1;
                    SendBuf[index] = 0x01;  //GetRequestNormal
                    index += 1;
                    SendBuf[index] = 0x01;  //PIID
                    index += 1;
                    memcpy_r(&SendBuf[index], &gSubOadData[i], 4);  //OAD
                    index += 4;
                    SendBuf[index] = 0x01;  //没有时间标签
                    index += 1;

                    if(index > 0)
                    {
                        SendLen = index;
                        ACUDP_FMT_TRACE("【测试】【bTestGetNormalOther】pthread_test() ACIPC_SEND... SendLen=%d, DestName=%s \n", SendLen, gUdpFileName);
                        ACUDP_BUF_TRACE(SendBuf, SendLen, "【测试】【bTestGetNormalOther】发送报文:");
                        task_pfmsg_send(UdpServiceFd, SendBuf, SendLen, gUdpFileName);
                    }

                    //跳到下一个OAD
                    i ++;
                }

                //测试方案4: 固定apdu报文测试
                if(bTestApduBuff)
                {
                    index = 0;
                    SendLen = 0;
                    memset(SendBuf,0x00,sizeof(SendBuf));

                    if(i > 4)
                    {
                        sleep(20);
                        i = 0;
                    }

                    //组装APDU测试报文
                    if(0 == i)
                    {
                        //设置交采电压合格率参数
                        uint8 tmpbuf[] = {0x06,0x01,0x00,0x40,0x30,0x02,0x00,0x02,0x04,0x12,0x03,0xde,0x12,0x03,0xde,0x12,0x03,0xde,0x12,0x03,0xde,0x00};
                        uint8 tmpbufLen = sizeof(tmpbuf)/sizeof(uint8);
                        index = tmpbufLen;
                        memcpy(SendBuf, tmpbuf, tmpbufLen);
                    }
                    else if(1 == i)
                    {
                        //设置交采电压逆向序参数
                        uint8 tmpbuf[] = {0x06,0x01,0x01,0x30,0x0f,0x06,0x00,0x02,0x01,0x11,0x0a,0x00};
                        uint8 tmpbufLen = sizeof(tmpbuf)/sizeof(uint8);
                        index = tmpbufLen;
                        memcpy(SendBuf, tmpbuf, tmpbufLen);
                    }
                    else if(2 == i)
                    {
                        //配置交采冻结关联列
                        uint8 tmpbuf[] = {0x07,0x01,0x02,0x50,0x04,0x07,0x00,0x01,0x01,0x02,0x03,0x12,0x00,0x0f,0x51,0x20,0x00,0x02,0x00,0x12,0x00,0x0f,0x00};
                        uint8 tmpbufLen = sizeof(tmpbuf)/sizeof(uint8);
                        index = tmpbufLen;
                        memcpy(SendBuf, tmpbuf, tmpbufLen);
                    }
                    else if(3 == i)
                    {
                        //删除交采冻结关联列，698测试报文
                        uint8 tmpbuf[] = {0x68, 0x1c, 0x00, 0x43, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbe, 0xa4, 0x07, 0x01, 0x02, 0x50, 0x04,
                            0x05, 0x00, 0x51, 0x20, 0x00, 0x02, 0x00, 0x00, 0x77, 0xb0, 0x16};
                        uint8 tmpbufLen = sizeof(tmpbuf)/sizeof(uint8);
                        index = tmpbufLen;
                        memcpy(SendBuf, tmpbuf, tmpbufLen);
                    }
                    else if (4 == i)
                    {
                        //645测试报文
                        uint8 tmpbuf[] = {0x68, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x68, 0x14, 0x12, 0x34, 0x37, 0x33, 0x37, 0x33, 0x33, 0x33, 0x33, 0x33,
                            0x33, 0x33, 0x33, 0x34, 0x33, 0x33, 0x33, 0x33, 0x33, 0x92, 0x16};
                        uint8 tmpbufLen = sizeof(tmpbuf) / sizeof(uint8);
                        index = tmpbufLen;
                        memcpy(SendBuf, tmpbuf, tmpbufLen);
                    }

                    if(index > 0)
                    {
                        SendLen = index;
                        ACUDP_FMT_TRACE("【测试】【bTestGetNormalOther】pthread_test() ACIPC_SEND... SendLen=%d, DestName=%s \n", SendLen, gUdpFileName);
                        ACUDP_BUF_TRACE(SendBuf, SendLen, "【测试】【bTestGetNormalOther】发送报文:");
                        task_pfmsg_send(UdpServiceFd, SendBuf, SendLen, gUdpFileName);
                    }

                    //跳到下一个OAD
                    i ++;
                }

                //转入接收处理
                SocketState = ACIPC_RECV;

                break;
            }

            case ACIPC_RECV:
            {
                //环回进行数据接收
                RecvLen = 0;
                memset(RecvBuf,0x00,sizeof(RecvBuf));
                memset(SenderName,0x00,sizeof(SenderName));

                RecvLen = task_pfmsg_recv(&UdpServiceFd, RecvBuf, MAX_UDP_BUFF_LEN, SenderName, sizeof(SenderName));

                if(RecvLen > 0)
                {
                    ACUDP_FMT_TRACE("【测试】pthread_test() ACIPC_RECV... RecvLen=%d, SenderName=%s \n", RecvLen, SenderName);
                    ACUDP_BUF_TRACE(RecvBuf, RecvLen, "【测试】接收报文:");
                }

                //转到发送
                SocketState = ACIPC_SEND;

                break;
            }

            default:
            {
                SocketState = ACIPC_INIT;
                break;
            }

        }

        usleep(200000);                         //出让CPU资源
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

    pthread_exit(NULL);

}



/*********************************************************************
 功能描述：全局变量初始化
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void init_global()
{
    int ret = -1;

    //主动上送消息INDEX锁初始化
    if (pthread_mutex_init(&ActiveInfoIndexLock, NULL) != 0)
    {
        ACMQTT_FMT_DEBUG("init_global() init pthread ActiveInfoIndexLock lock error!\n");
        exit(1);
    }

    //全局变量初始化
    memset(&TermSharedData, 0, sizeof(TERM_SHARE_DATA_TYPE));
    memset(&gPriMeasurePara, 0, sizeof(PRIVTE_MEASURE_TYPE));
    memset(&MidTermSharedData, 0, sizeof(TERM_SHARE_DATA_TYPE));
    memset(&gMidPriMeasurePara, 0, sizeof(PRIVTE_MEASURE_TYPE));
    memset(&HisTermSharedData, 0, sizeof(TERM_SHARE_DATA_TYPE));
    memset(&gHisPriMeasurePara, 0, sizeof(PRIVTE_MEASURE_TYPE));

    memset(g_nAcReadPeriNumFlagArr, 0x01, OOP_MAX_DAYPERIODS);
    memset(g_nAcReadCurrDayFlagArr, 0x01, OOP_MAX_DAYPERIODS);
    memset(g_nAcReadAddiDayFlagArr, 0x01, OOP_MAX_DAYPERIODS);

    //电能表恒定磁场干扰事件
    memset(&gEventCommInfo, 0, sizeof(EVENT_COMM_INFO));

    //加载电能表恒定磁场干扰事件参数
    ret = read_pdata_xram(gEventParaPath, (char *)&gEventCommInfo, 0, sizeof(EVENT_COMM_INFO));
    ACDB_FMT_DEBUG("init_global() 加载磁场干扰事件参数 ret=%d \n", ret);
    if (ret == 0)
    {
        HisTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7 = gEventCommInfo.EffectState;
        ACDB_FMT_DEBUG("init_global() 加载磁场干扰事件参数成功, 初始状态 gEventCommInfo.EffectState=%d \n", gEventCommInfo.EffectState);
    }

    //所有事件状态汇总
    memset(&gTotalEventInfo, 0, sizeof(gTotalEventInfo));

    //交采电压逆向序事件初始化
    memset(&event_300F, 0, sizeof(event_300F));
    event_300F.eventCommInfo.eventOAD.value = 0x300F0200;

    //公共存储标记初始化
    memset(&gSaveFlag, 0, sizeof(SAVE_FLAG_T));
    gSaveFlag.bFirstSaveEnergy = TRUE;
    gSaveFlag.bFirstSaveDemand= TRUE;
    gSaveFlag.bFirstSaveRealtime = TRUE;
    gSaveFlag.bFirstSaveHarmonic = TRUE;
    gSaveFlag.bFirstSaveSpecial = TRUE;

    return;
}

/*********************************************************************
 功能描述：配置文件解析
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
int init_config()
{
    dictionary *ini = NULL;
    char* tmp;
    char* stringvalue = NULL;
    char   curpath[256] = {0};
    char   filename[256] = {0};
    int i;

    //获取当前目录绝对路径，即去掉程序名
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("acMeter get err path!\n");
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
    sprintf(filename, "%s%s", curpath, AC_METER_INI_FILE);
    printf("配置文件filename: %s!\n", filename);
    ini = iniparser_load(filename);

    /* 日志模块初始化 */
    diag_init(filename, TRUE);
    sleep(1);

    if(NULL != ini)
    {
        /* 配置文件内容获取 */
        tmp = iniparser_getstring(ini, "mqtt:MQTTIP", "172.17.0.1");
        printf("读取mqttip = %s \n", tmp);
        strcpy(mqttIP, tmp);
        mqttport = iniparser_getint(ini, "mqtt:MQTTPORT", 1883);
        printf("读取mqttport = %d \n", mqttport);

        tmp = iniparser_getstring(ini, "FILEPATH_CONF:LOGPATH", "/data/app/acMeter/log");
        char   exceptpath[256] = {0};
        sprintf(exceptpath, "%s/Exception.log", tmp);
        change_sig_file(exceptpath);

        /*
        gSupportRelay485Flag = iniparser_getint(ini, "RELAY_CONF:AC485FLAG", 0);
        printf("读取gSupportRelay485Flag = %d \n", gSupportRelay485Flag);
        */

        /* 周期采集采用的规约，0-DLT645 1-698.45 */
        gSampPrtl = iniparser_getint(ini, "SAMP_CONF:PRTL", 2);
        INIT_FMT_DEBUG("周期采集采用的规约[%d], 0-DLT645 1-698.45\n", gSampPrtl);

        /* 获取串口配置 */
        gUartConf.enable = iniparser_getint(ini, "UART_CONF:ENABLE", 0);
        INIT_FMT_DEBUG("读取串口使能标志[%d]\n", gUartConf.enable);

#ifdef BOARD_801
        //801核心板串口通过HAL获取
        UART_DEVICE_T *dev = (UART_DEVICE_T*)hal_device_get(UART_DEVICE_ACPORT_STR);
        if(dev)
        {
            dev->uart_get_real_dev_name(dev, (uint8*)gUartConf.dev);
            hal_device_release((HW_DEVICE *)dev);
        }
        else
        {
            sprintf(gUartConf.dev, "%s", "/tty/S8");
        }

#else
        stringvalue = iniparser_getstring(ini, "UART_CONF:DEV", "/dev/ttyS4");
        strcpy(gUartConf.dev, stringvalue);
        INIT_FMT_DEBUG("读取串口设备名称[%s]\n", gUartConf.dev);
#endif

        gUartConf.BaudRate = iniparser_getint(ini, "UART_CONF:BAUDRATE", 115200);
        INIT_FMT_DEBUG("读取串口的波特率[%d]\n", gUartConf.BaudRate);

        gUartConf.ByteSize = iniparser_getint(ini, "UART_CONF:BYTESIZE", 8);
        INIT_FMT_DEBUG("读取串口字节长度[%d]\n", gUartConf.ByteSize);

        stringvalue = iniparser_getstring(ini, "UART_CONF:PARITY", "e");
        gUartConf.Parity = stringvalue[0];
        INIT_FMT_DEBUG("读取串口校验模式[%c]\n", gUartConf.Parity);

        gUartConf.StopBits = iniparser_getint(ini, "UART_CONF:STOPBITS", 1);
        INIT_FMT_DEBUG("读取串口的停止位[%d]\n", gUartConf.StopBits);

        iniparser_freedict(ini);
    }
    else
    {
        printf("Get no ini file: %s!\n", AC_METER_INI_FILE);
    }

    return 0;
}

/*********************************************************************
 功能描述：485-2调试口配置文件解析
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
int init_485_config()
{
    dictionary *ini = NULL;
    char   filename[256] = {0};
    char*  stringvalue = NULL;

    sprintf(filename, "%s", AC_485_INI_FILE);
    INIT_FMT_TRACE("485-2调试口配置文件[%s]\n", filename);
    ini = iniparser_load(filename);

    if (NULL != ini)
    {
        /* 配置文件内容获取 */
        gSupportRelay485Flag = iniparser_getint(ini, "SAFE_MODE:AC485FLAG", 1);
        INIT_FMT_DEBUG("读取gSupportRelay485Flag = %d \n", gSupportRelay485Flag);

        iniparser_freedict(ini);
    }
    else
    {
        INIT_FMT_DEBUG("485-2调试口配置文件[%s]获取失败\n", filename);
    }

    sprintf(filename, "%s", AC_METER_INI_FILE);
    ini = iniparser_load(filename);

    if (NULL != ini)
    {
        /* 获取ipc路径 */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:IPCPATH", (char*)"/mnt/ipc_socket");
        task_msg_prepare(stringvalue);

        iniparser_freedict(ini);
    }
    else
    {
        sprintf(filename, "/usr/local/extapps/acMeter/bin/%s", AC_METER_INI_FILE);
        ini = iniparser_load(filename);
        if (NULL != ini)
        {
            /* 获取ipc路径 */
            stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:IPCPATH", (char*)"/mnt/ipc_socket");
            task_msg_prepare(stringvalue);

            iniparser_freedict(ini);
        }
        else
        {
            INIT_FMT_DEBUG("配置文件[%s]获取失败!\n", filename);
        }
    }

    return 0;
}

uint32 shandong_get_ppp1_ip(char *ucPPPName)
{
    int fd;
    struct ifreq ifr;
    uint32 ipAddr = 0;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return 0;
    }

    strcpy(ifr.ifr_name, ucPPPName);
    if(0 == ioctl(fd, SIOCGIFFLAGS, &ifr))
    {
        if(0 == ioctl(fd, SIOCGIFADDR, &ifr))
        {
            ipAddr = ((struct sockaddr_in*)(&(ifr.ifr_addr)))->sin_addr.s_addr;
            ipAddr = ntohl(ipAddr);
        }
    }

    close(fd);
    return ipAddr;
}

/**********************************************************************
* @name：      shandong_ppp1_route
* @brief：    山东地区增加路由设置, 真是供货需要去掉
* @param[in] ：
* @param[out]：
* @return    ：
* @Date      ：2022-4-22
**********************************************************************/
void shandong_ppp1_route()
{
    uint32 pppIP = 0;
    /* 检查路由 */
    FILE *fp;
    char ucRoute[256] = {0};
    char ucSystem[256] = {0};

    pppIP = shandong_get_ppp1_ip("ppp1");
    if(20 == ((pppIP >> 24) & 0xff) && 0 == ((pppIP >> 16) & 0xff) && (0 == ((pppIP >> 8) & 0xff)))
    {
        sprintf(ucSystem, "route -n|grep ppp1|grep '20.0.0.0'|grep -v UH|awk '{print $1}'");
        fp = popen(ucSystem, "r");
        if(fp)
        {
            fscanf(fp, "%s", ucRoute);
            pclose(fp);
            ucRoute[24] = 0;
            if(0 != strcmp(ucRoute, "20.0.0.0"))
            {
                system("route add -net 20.0.0.0/24 dev ppp1");
            }
        }
    }
    memset(ucSystem, 0, 256);
    memset(ucRoute, 0, 256);
    sprintf(ucSystem, "ifconfig usb1|grep 'inet addr:'|awk '{print $2}'");
    fp = popen(ucSystem, "r");
    if(fp)
    {
        fscanf(fp, "%s", ucRoute);
        pclose(fp);
        if(0 != strlen(ucRoute))
        {
            system("ifconfig usb1 down");
        }
    }
}

/**********************************************************************
* @name：      ac_Version_parser
* @brief：     版本信息处理
* @param[in] ：oad    -  oad
               pInMsg - 输入内容(不包含OAD)
               inLen  - 输出内容
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
void get_samp_prtl(uint8 *pSampPrtl)
{
    uint8 sampPrtl = *pSampPrtl;

    uint32 cnt = 0;

    //配置文件中配置了采集方式，0 - 07规约; 1 - 698规约
    if ((sampPrtl == 0) || (sampPrtl == 1))
    {
        return;
    }

    //先初始化下设备
    while ((fd_ac = ac_sample_dev_open()) < 0)
    {
        cnt ++;
        if (cnt > 10)
        {
            INIT_FMT_DEBUG("ac_sample_dev_open err fd[%d]\n", fd_ac);
            exit(1);
        }
        fd_ac = -1;
        sleep(1);
    }

    //验证是否支持698规约
    for (cnt = 0; cnt < 3; cnt++)
    {
        if (ERR_OK == ac_get_698ver_bak(&fd_ac))
        {
            break;
        }
    }

    if (cnt >= 3)
    {
        sampPrtl = 0;
        INIT_FMT_DEBUG("验证失败，底板是07交采 sampPrtl[%d]\n", sampPrtl);
    }
    else
    {
        sampPrtl = 1;
        INIT_FMT_DEBUG("验证成功，底板是698交采 sampPrtl[%d]\n", sampPrtl);
    }

    *pSampPrtl = sampPrtl;

    ac_sample_dev_close(&fd_ac);

    return;
}

#if DESC("获取交采接线状态", 1)
/*******************************************************************************
* 函数名称: ac_get_power_type
* 函数功能: 获取接线方式规约处理
* 输入参数: fd_spi 操作句柄
* 输出参数: 无
* 返 回 值: >0    成功
*           <0    失败
*******************************************************************************/
int ac_get_power_type(int32 *pfd)
{
    int   ret = -1;
    int32 recvLen = 0;
    int32 sendLen = 0;
    uint8 recvBuf[1024] = { 0 };
    uint8 sendBuf[1024] = { 0 };
    OOP_OAD_U oad = {0x40100200};

    int32 fd_spi = *pfd;

    if (fd_spi < 0)
    {
        fd_spi = ac_sample_dev_open();
        if (fd_spi < 0)
        {
            return ERR_NO_HANDLE;
        }
        else
        {
            *pfd = fd_spi;
            UPDATE_FMT_DEBUG("msg: the spi open ok.\n");
        }
    }

    sendLen = ac_get_msg_package(oad, sendBuf, 1024, 0);
    if (sendLen < 0)    //发送完成
    {
        return ERR_NORMAL;
    }

    if (gUartConf.enable)
    {
        ret = commdrv_Send_232(fd_spi, sendBuf, sendLen);
    }
    else
    {
        ret = ac_spi_send_data(fd_spi, sendBuf, sendLen, recvBuf, 500000);
    }

    if (ret < 0)
    {
        INIT_FMT_DEBUG("fd_spi = %d send ret = %d\n", fd_spi, ret);
        ac_sample_dev_close(pfd);
        return ERR_IFACE;
    }
    else
    {
        INIT_BUF_DEBUG(sendBuf, sendLen, "获取接线方式，发送");
    }

    if (gUartConf.enable)
    {
        recvLen = commdrv_Recv_232(fd_spi, recvBuf, sizeof(recvBuf), 300, 8, 0xFF);
    }
    else
    {
        recvLen = ac_spi_recv_data_timeout(fd_spi, sendBuf, recvBuf, sizeof(recvBuf), 50000, 65);
    }

    if (recvLen > 0)
    {
        INIT_BUF_DEBUG(recvBuf, recvLen, "获取接线方式，接受");
        ret = ac_read_sample_parser(SAMPLE_OTHER, recvBuf, recvLen);
        if (ret != ERR_OK)
        {
            INIT_FMT_DEBUG("解帧结果异常[%d]\n", ret);
            return ret;
        }
    }
    else
    {
        return ERR_TIMEOUT;
    }

    return ERR_OK;
}

/**********************************************************************
* @name：       get_phase_wire_type
* @brief：      获取接线状态
* @param[in] ：
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
void get_samp_power_type(void)
{
    char *type[] = {"未知", "单相表", "三相三线", "三相四线"};

    uint32 cnt = 0;

    //先初始化下设备
    while ((fd_ac = ac_sample_dev_open()) < 0)
    {
        cnt ++;
        if (cnt > 10)
        {
            INIT_FMT_DEBUG("ac_sample_dev_open err fd[%d]\n", fd_ac);
            exit(1);
        }
        fd_ac = -1;
        sleep(1);
    }

    //获取接线状态
    for (cnt = 0; cnt < 3; cnt++)
    {
        if (ERR_OK == ac_get_power_type(&fd_ac))
        {
            break;
        }
    }

    if (cnt >= 3)
    {
        INIT_FMT_TRACE("获取接线方式失败 类型[%s]\n", type[TermSharedData.powerType]);
    }
    else
    {
        INIT_FMT_TRACE("获取接线方式成功 类型[%s]\n", type[TermSharedData.powerType]);
    }

    ac_sample_dev_close(&fd_ac);

    return;
}

/**********************************************************************
* @name：       ac_power_type_update
* @brief：      获取接线状态更新到数据中心
* @param[in] ：
* @param[out]：
* @return    ：错误码
* @Date      ：2019-12-6
**********************************************************************/
void ac_power_type_update(void)
{
    int  ret = -1;
    char *type[] = {"未知", "单相表", "三相三线", "三相四线"};

    NOMAL_OAD_T nomalOad = {0};

    nomalOad.logicId = 1;  //逻辑地址
    nomalOad.infoNum = 0;  //信息点

    if (gDataCenterClient <= 0)
    {
        return;
    }

    if (TermSharedData.powerType > 0)
    {
        nomalOad.oad.value = 0x40100200;
        nomalOad.classtag = CLASS_DATA_INIT;
        ret = db_write_nomal(gDataCenterClient, &nomalOad, (uint8*)&TermSharedData.powerType, sizeof(TermSharedData.powerType));
        if (ret != 0)
        {
            INIT_FMT_DEBUG("接线方式写入数据中心失败 错误码[%d] 接线状态[%s]\n", ret, type[TermSharedData.powerType]);
        }
        else
        {
            INIT_FMT_DEBUG("接线方式写入数据中心成功 接线状态[%s]\n", type[TermSharedData.powerType]);
        }
    }
}

#endif

#if DESC("监测持续断电5min复位终端", 1)

#define POWER_CHARGE_CONTROL  0    /* 是否上电30分钟后才开启电池充电 */

int system_get_main_cell_power(int *value)
{
    int  ret = 0;
	ADC_DEVICE_T* dev = (ADC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_ADC);
	if(NULL == dev)
	{
        return -1;
	}
	ret = dev->adc_volt_read(dev, EMADC_BATVOL_CHANNEL, value);
	hal_device_release((HW_DEVICE *)dev);
	return ret;
}

void *pthread_check_poweroff(void *arg)
{
    int32 lastpowerstate = -1;
    int32 lastpowerstate1 = 1;
    int32 lastpowerstate2 = 1;
    int32 lastpowerstate3 = 1;
    int32 powerstate;
    int ret = 0;
    int value = 0;
    int celllefttime = -1;  //电池连续供电剩余
    POWER_DEVICE_T* dev = NULL;
    prctl(PR_SET_NAME, "pthread_check_poweroff");

    while(1)
    {
        if(NULL == dev)
        {
            dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
            if(dev)
            {
                dev->power_battery_charge_set(dev, 0, 1);
                dev->power_battery_charge_set(dev, 1, 1);
            }
        }
        sleep(1);

        if(NULL == dev)
        {
            continue;
        }

        if(dev->power_power_state_get(dev, POWER_MAIN, &powerstate) < 0)
        {
            continue;
        }

        if(powerstate == lastpowerstate1 && powerstate == lastpowerstate2 && powerstate == lastpowerstate3)
        {
            if(0 == lastpowerstate && powerstate == 1)
            {
                INIT_FMT_DEBUG("检测到电源供电\n");
                celllefttime = -1;
            }
            else if(1 == lastpowerstate && powerstate == 0)
            {
                INIT_FMT_DEBUG("检测到电源断电, 停止电池、电容充电\n");
                celllefttime = 300;

                g_nPowerOffFlag = 1;
            }
            else if(-1 == lastpowerstate && powerstate == 0)
            {
                //终端通过按键拉起，没有电源供电，直接是电池，5分钟后断电
                INIT_FMT_DEBUG("无电源启动(电池供电)\n");
                celllefttime = 300;
            }
            lastpowerstate = powerstate;
        }
        lastpowerstate1 = lastpowerstate2;
        lastpowerstate2 = lastpowerstate3;
        lastpowerstate3 = powerstate;

        if(0 == celllefttime && 0 == lastpowerstate)
        {
            /* 倒计时结束 复位 */
            WATCHDOG_DEVICE_T* dogdev = (WATCHDOG_DEVICE_T*)hal_device_get(HW_DEVICE_ID_WATCHDOG);
            INIT_FMT_DEBUG("无电源工作超过300s, 终端复位!!!\n\n");
            celllefttime = -1;
            reboot_to_smiOS();
            sleep(10);
            if (dogdev)
            {
                ret = dogdev->watchdog_period_set(dogdev, 1);
                if (ret != ERR_OK)
                {
                    INIT_FMT_DEBUG("set watchdog period 1s err\n");
                }
                hal_device_release((HW_DEVICE *)dogdev);
            }
            continue;
        }
        else if(1 == lastpowerstate)
        {
            celllefttime = -1;
            continue;
        }

        ret = system_get_main_cell_power(&value);
        if(celllefttime >= 0 && 0 == ret && value > 0)
        {
            celllefttime--;
        }
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

    return 0;
}

#endif

int ac_check_cmd_exist(char *cmd)
{
    BOOL ret = FALSE;
    char pstatus[10] = {0};
    FILE* pipe = NULL;

    if (cmd == NULL)
    {
        return ret;
    }

    pipe = popen(cmd, "r");

    if (!pipe)
    {
        pipe = popen(cmd, "r");
    }

    if (!pipe)
    {
        return ret;
    }

    fgets(pstatus, 10, pipe);
    pclose(pipe);

    if (pstatus[0] >= '1')
    {
        ret = TRUE;
    }

    return ret;
}

void ppp0_test()
{
    const char* cmd = "route add -net 20.0.0.0 netmask 255.0.0.0  dev ppp0";
    if(FALSE == ac_check_cmd_exist((char*)"ifconfig |grep ppp0") ||
        TRUE == ac_check_cmd_exist((char*)"route -n |grep 20.0.0.0"))
    {
        return;
    }
    INIT_FMT_DEBUG("cmd %s\n",cmd);

    system(cmd);
}

/*********************************************************************
 功能描述：交采app主函数
 输入参数：argc
           argv
 输出参数：无
 函数返回值：无
********************************************************************/
int main(int argc, char *argv[])
{
    int           err  = 0;               // 打开线程时，返回的错误号
    int           ret  = 0;
    int           i    = 0;
    MAIN_TRHEAD_T *pthread = NULL;
    int           count = 0;
    //收发共享队列初始化
    LockQueue_T LockDealRecvDown[RUN_DOWN_NUM]; 					 //抄表线程下行接收共享内存指针
    LockQueue_T LockDealSendDown[RUN_DOWN_NUM]; 					 //抄表线程下行发送共享内存指针
    LockQueue_T LockUdpRecvDown; 					                 //UDP透传下行接收共享内存指针
    LockQueue_T LockUdpSendDown; 					                 //UDP透传下行发送共享内存指针
    LockQueue_T LockTimeRecvDown; 					                 //对时下行接收共享内存指针
    LockQueue_T LockTimeSendDown; 					                 //对时下行发送共享内存指针
    LockQueue_T LockModuleRecvUp;	                                 //模组信息上行接收共享内存指针
    LockQueue_T LockConfirmDown;	                                 //回复交采模块确认共享内存指针

    uint16  CheckSeconds = 120;  //线程异常检测周期

    /* 检查程序只启动了一个副本 */
    if (!singleton_check("acMeter"))
    {
        return -1;
    }

    if (argc >= 2)
    {
        if (0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            printf("30s waiting...\n");
            sleep(30);
        }
    }

    //检查CPU字节序，在一个终端必然存在的进程中添加
    check_endian();

    /* hal初始化 */
    ret = hal_init();
    while(0 != ret)
    {
        INIT_FMT_DEBUG("hal初始化失败!\n");
        ret = hal_init();
        sleep(2);
    }

    /* 信号处理 */
    init_sig_action(NULL);

    /* 读取配置文件 */
    init_config();
    INIT_FMT_DEBUG("Version: %s Time: %s, area: %s, Build: %s %s\n", APP_VERSION, APP_PUBLISH_TIME, AREANAME_PLACE,
        __DATE__, __TIME__);
    INIT_FMT_DEBUG("读配置文件. mqttIP = %s, mqttport = %d\n", mqttIP, mqttport);

    init_485_config();
    INIT_FMT_DEBUG("是否支持转发485串口报文[%d]\n", gSupportRelay485Flag);

    /* 串口服务器同步消息初始化 */
    ac_485comm_msg_init();

    /* 全局变量初始化 */
    init_global();

    /* 其他初始化 */
    ac_other_init();

    /* mqtt初始化 */
    INIT_FMT_DEBUG("准备进行mqtt初始化... \n");
    ret = mqtt_init();
    if (ret < 0)
    {
        INIT_FMT_DEBUG("MQTT初始化失败! ret=%d \n", ret);
        exit(1);
    }

    /* 数据更新初始化 */
    db_updata_init();

    /* 如果配置文件中，周期采集规约为自适应，则自动获取底板采集方式 */
    get_samp_prtl(&gSampPrtl);

    /* 获取接线方式 */
    get_samp_power_type();

    /* hal获取appid */
    //appid_query(AppIdName, AppId, APP_ID_LEN);

    /* 创建数据中心agent */
    gDBAgent = db_agent_create(AGENT_TYPE_THREAD, DB_AC_AGENT, info_to_dbCenter, DB_CLIENT_MAX);
    while (gDBAgent < 0)
    {
        INIT_FMT_DEBUG("创建dbagent失败! gDBAgent = %d\n", gDBAgent);
        gDBAgent = db_agent_create(AGENT_TYPE_THREAD, DB_AC_AGENT, info_to_dbCenter, DB_CLIENT_MAX);
        sleep(1);
    }

    //抄表线程下行接收共享内存指针
    for (i = 0; i < RUN_DOWN_NUM; i++)
    {
        memset(&LockDealRecvDown[i], 0, sizeof(LockDealRecvDown[i]));
        if (pthread_mutex_init(&LockDealRecvDown[i].QueueLock, NULL) != 0)
        {
            INIT_FMT_DEBUG("init pthread LockDealRecvDown lock error! i=%d \n", i);
            exit(1);
        }
        pDealRecvDown[i] = &LockDealRecvDown[i];
    }

    //抄表线程下行发送共享内存指针
    for (i = 0; i < RUN_DOWN_NUM; i++)
    {
        memset(&LockDealSendDown[i], 0, sizeof(LockDealSendDown[i]));
        if (pthread_mutex_init(&LockDealSendDown[i].QueueLock, NULL) != 0)
        {
            INIT_FMT_DEBUG("init pthread LockDealSendDown lock error! i=%d \n", i);
            exit(1);
        }
        pDealSendDown[i] = &LockDealSendDown[i];
    }

    //UDP透传下行接收共享内存指针
    memset(&LockUdpRecvDown, 0, sizeof(LockUdpRecvDown));
    if (pthread_mutex_init(&LockUdpRecvDown.QueueLock, NULL) != 0)
    {
        INIT_FMT_DEBUG("init pthread LockDealRecvDown lock error!\n");
        exit(1);
    }
    pUdpRecvDown = &LockUdpRecvDown;

    //UDP透传下行发送共享内存指针
    memset(&LockUdpSendDown, 0, sizeof(LockUdpSendDown));
    if (pthread_mutex_init(&LockUdpSendDown.QueueLock, NULL) != 0)
    {
        INIT_FMT_DEBUG("init pthread LockUdpSendDown lock error!\n");
        exit(1);
    }
    pUdpSendDown = &LockUdpSendDown;

    //对时下行接收共享内存指针
    memset(&LockTimeRecvDown, 0, sizeof(LockTimeRecvDown));
    if (pthread_mutex_init(&LockTimeRecvDown.QueueLock, NULL) != 0)
    {
        INIT_FMT_DEBUG("init pthread LockTimeRecvDown lock error!\n");
        exit(1);
    }
    pTimeRecvDown = &LockTimeRecvDown;

    //对时下行发送共享内存指针
    memset(&LockTimeSendDown, 0, sizeof(LockTimeSendDown));
    if (pthread_mutex_init(&LockTimeSendDown.QueueLock, NULL) != 0)
    {
        INIT_FMT_DEBUG("init pthread LockTimeSendDown lock error!\n");
        exit(1);
    }
    pTimeSendDown = &LockTimeSendDown;

    //模组信息上行接收共享内存指针
    memset(&LockModuleRecvUp, 0, sizeof(LockModuleRecvUp));
    if (pthread_mutex_init(&LockModuleRecvUp.QueueLock, NULL) != 0)
    {
        INIT_FMT_DEBUG("init pthread LockModuleRecvUp lock error! \n");
        exit(1);
    }
    pModuleRecvUp = &LockModuleRecvUp;

    //回复交采模块确认共享内存指针
    memset(&LockConfirmDown, 0, sizeof(LockConfirmDown));
    if (pthread_mutex_init(&LockConfirmDown.QueueLock, NULL) != 0)
    {
        INIT_FMT_DEBUG("init pthread LockConfirmDown lock error! \n");
        exit(1);
    }
    pConfirmDown = &LockConfirmDown;


    for (i = 0; i < g_MainTaskNum; i++)
    {
        pthread = &g_tMainThread[i];
        THREAD_ARG_T *tArg = (THREAD_ARG_T *)pthread->arg;

        if (pthread->isStart)
        {
            err = pthread_create(&pthread->nThreadId, NULL, pthread->pEntryFunc, pthread->arg);
            if (err != 0)
            {
                INIT_FMT_DEBUG("%s create error! err:%d \n", tArg->threadName, err);
                exit(1);
            }
        }
    }

    while (1)
    {
        for (i = 0; i < g_MainTaskNum; i++)
        {
            pthread = &g_tMainThread[i];
            THREAD_ARG_T *tArg = (THREAD_ARG_T *)pthread->arg;

            if ((pthread->isDefend) && (pthread->isStart))
            {
                if (tArg->cnt >= CheckSeconds)           // 若2分钟检测都不正常，则关闭此线程，然后重新打开此线程
                {
                    err = pthread_cancel(pthread->nThreadId);
                    if (err != 0)
                    {
                        INIT_FMT_DEBUG("%s cancel error! err[%d]\n", tArg->threadName, err);
                    }
                    else
                    {
                        pthread_join(pthread->nThreadId, NULL);
                        err = pthread_create(&pthread->nThreadId, NULL, pthread->pEntryFunc, tArg); // 打开一个交采数据采样与统计线程
                        if (err != 0)
                        {
                            INIT_FMT_DEBUG("%s create error! err:%d \n", tArg->threadName, err);
                        }
                        else
                        {
                            tArg->cnt = 0;
                            INIT_FMT_DEBUG("%s create OK!\n", tArg->threadName);
                        }
                    }
                }
                else
                {
                    tArg->cnt ++;
                }
            }
        }

        sleep(1);
        if(count++ > 10)
        {
#ifdef AREA_SHANDONG
//山东给无线2增加调试功能
            shandong_ppp1_route();
#endif
            ppp0_test();

            count = 0;
        }
    }

    return 0;
}

