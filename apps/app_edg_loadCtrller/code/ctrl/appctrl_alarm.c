/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   控制模块 保电控制
* @date：    2020-4-11
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "appmsg_split.h"
#include "hal.h"
#include "led.h"
#include "audio.h"

#include "appctrl_comm.h"
#include "appctrl_alarm.h"
#include "appctrl_dev.h"
#include "framecomm.h"
#include "msg_id.h"

LED_DEVICE_T*   g_LEDdev = NULL;
AUDIO_DEVICE_T* g_Buzzdev = NULL;

ALARM_CTRL_T    g_AlarmInfo;

pthread_mutex_t g_fCtrlKey;
uint8           g_CtrlKey = 0;         //按键按下
uint8           g_AlarmDelay = 0;      //-每次告警间隔-
uint8           g_AlarmMaxTurn = 0;
uint32          g_Ctrlshowtime = 120;  //默认的提示延时时间
uint8           g_zcu1_alarm = 0;

extern void appmain_audio_set(int indexno);

/**********************************************************************
* @name      : appctrl_alarm_init
* @brief     ：控制告警初始化
* @param[in] ：uint8 maxturn
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-29
* @Update    :
**********************************************************************/
void appctrl_alarm_init(uint8 maxturn)
{
    g_AlarmMaxTurn = maxturn;
    pthread_mutex_init(&g_fCtrlKey, NULL);

    if(NULL == g_Buzzdev)
    {
        g_Buzzdev = (AUDIO_DEVICE_T*)hal_device_get(HW_DEVICE_ID_AUDIO);
    }
    if(NULL != g_Buzzdev)
    {
        /* 关闭蜂鸣器 */
        g_Buzzdev->audio_set_buzzer_out(g_Buzzdev, 0, 100, 1);
    }

    if(NULL == g_LEDdev)
    {
        g_LEDdev = (LED_DEVICE_T*)hal_device_get(HW_DEVICE_ID_LED);
    }
    if(NULL != g_LEDdev)
    {
        /* 关闭告警灯 */
        g_LEDdev->led_state_set(g_LEDdev, EMLED_ALARM, LED_OFF, COM_MODE, NULL, 0);
    }
}

/**********************************************************************
* @name      : appctrl_ctrl_buzzer
* @brief     ：控制蜂鸣器
* @param[in] ：int dbClient  数据中心id
               uint8 bOpen   是否打开
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-14
* @Update    :
**********************************************************************/
void appctrl_ctrl_buzzer(int dbClient, uint8 bOpen)
{
    uint32           warnid = 0;
    OOP_WARNINGOUT_T tWarnGroup;

    memset(&tWarnGroup, 0, sizeof(OOP_WARNINGOUT_T));
    /* 关闭不限时时间 */
    if(FALSE == bOpen)
    {
        g_Buzzdev->audio_set_buzzer_out(g_Buzzdev, 0, 100, 1);
    }
    else
    {
        warnid = appctrl_get_warning_id();
        if(warnid > 0)
        {
             g_Buzzdev->audio_set_buzzer_out(g_Buzzdev, 1, 100, 1);
             tWarnGroup.state[warnid - 1] = 1;
        }
    }

    appmain_write_warn_status(&tWarnGroup);
}

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

/**********************************************************************
* @name      : appctrl_alarm_turnnum
* @brief     ：更新轮次数量
* @param[in] ：uint8 maxturn
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-29
* @Update    :
**********************************************************************/
void appctrl_alarm_turnnum(uint8 turnmax)
{
    g_AlarmMaxTurn = turnmax;
}

/**********************************************************************
* @name      : appctrl_get_key
* @brief     ：获取按键加锁状态
* @param[in] ：
* @param[out]：
* @return    ：按键状态
* @Create    : 王津剑
* @Date      ：2020-4-29
* @Update    :
**********************************************************************/
uint8 appctrl_get_key()
{
    uint8 status = 0;

    PTHREAD_LOCK_SAFE(&g_fCtrlKey);
    status = g_CtrlKey;
    PTHREAD_UNLOCK_SAFE(&g_fCtrlKey);
    return status;
}

/**********************************************************************
* @name      : appctrl_get_key
* @brief     ：获取按键加锁状态
* @param[in] ：uint8 status   按键状态 TRUE-FALSE
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-29
* @Update    :
**********************************************************************/
void appctrl_set_key(uint8 status)
{
    PTHREAD_LOCK_SAFE(&g_fCtrlKey);
    g_CtrlKey = status;
    PTHREAD_UNLOCK_SAFE(&g_fCtrlKey);
}

/**********************************************************************
* @name      : appctrl_alarm_zcu1
* @brief     ：控制告警设置（专1）
* @param[in] ：uint8 status   状态
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2023-1-18
* @Update    :
**********************************************************************/
void appctrl_alarm_zcu1(uint8 status)
{
   g_zcu1_alarm = status;
}

/**********************************************************************
* @name      : appctrl_alarm_zcu1_get
* @brief     ：控制告警设置（专1）
* @param[in] ：uint8 status   状态
* @param[out]：
* @return    ：告警状态
* @Create    : 王津剑
* @Date      ：2023-1-18
* @Update    :
**********************************************************************/
uint8 appctrl_alarm_zcu1_get()
{
   return g_zcu1_alarm;
}


/**********************************************************************
* @name      : appctrl_alarm_hint
* @brief     ：构造显示提示信息发送桌面显示
* @param[in] ：
* @param[out]：
* @return    ：TRUE-继续执行/FALSE-执行结束
* @Create    : 王津剑
* @Date      ：2020-4-30
* @Update    :
**********************************************************************/
uint8 appctrl_alarm_hint()
{
    uint16 len = 0;
    uint8  buf[420] = {0};
    uint8  bufMsg[428] = {0};
    uint16 offset = 0;
    uint8  status = FALSE;
    uint32 showtime = g_Ctrlshowtime; //默认的显示时间s
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;

    CTRL_SHOW_HINT pFunc = (CTRL_SHOW_HINT)g_AlarmInfo.pFunc;
    int ret = 0;

    if(NULL == g_AlarmInfo.pFunc)
    {
        return status;
    }
    
    status = pFunc(g_AlarmInfo.ctrlType, g_AlarmInfo.group, g_AlarmMaxTurn, buf, 420, &len);
    if(len > 0)
    {
        appmsg_memcpy_r(bufMsg, &showtime, sizeof(uint32));
        offset += sizeof(uint32);

        len = strlen((char *)buf);
        bytes = appmsg_set_len_offset(len, bufLen);
        memcpy(bufMsg + offset, bufLen, bytes);
        offset += bytes;

        memcpy(bufMsg + offset, buf, len);
        offset += len;

        ret = lctrl_send_deskmsg(g_AlarmInfo.ctrlType, DESKGUI_IOP_PUSH_HINT, bufMsg, offset);
        if(0 != ret)
        {
            CTRL_BUF_DEBUG(bufMsg, offset, "Send gui type %d hint msg failed, ret %d", g_AlarmInfo.ctrlType, ret);
        }
    }
    return status;
}

/**********************************************************************
* @name      : appctrl_system_alarm
* @brief     ：根据当前闸状态中告警信息，发出语音报警及显示告警画面
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-14
* @Update    : 2020-4-29
**********************************************************************/
void appctrl_system_alarm(uint8 turnmax)
{       
    static uint8 voiceTimes = 0;//-告警声音剩余次数,静态变量-
    static uint8 voiceDelay = 0;//-告警声音延时-

    static uint8 buzzerStatus = TRUE;//-蜂鸣器是否打开-
    static uint8 buzzerStop = TRUE;//-蜂鸣器是否结束-
    static uint8 guiStop = TRUE;//-显示是否结束-

    static uint8 ledStatus = TRUE;//-led是否打开-
    static uint8 ledStop = TRUE;  //-led是否结束-
    static uint16 ledDelay = 0; //-结束标记-

    g_AlarmMaxTurn = turnmax;

    /* 按键按下 清除告警画面和声音 */
    if(TRUE == appctrl_get_key())
    {
        appctrl_set_key(FALSE);
        if(ALARM_OVER != g_AlarmInfo.alarmStep)
        {
            appctrl_show_guiinfo(0);
            g_AlarmInfo.alarmTimes = 0;         //-告警次数清零-
            g_AlarmInfo.alarmStep = ALARM_OVER; //-告警状态为告警结束-
            g_AlarmInfo.ctrlType = CTRL_NULL_TYPE;
        }
        appmain_audio_set(0);
    }

    switch(g_AlarmInfo.alarmStep)
    {
        case ALARM_OVER:
        {
            if(TRUE == buzzerStatus)
            {
                buzzerStatus = FALSE;
                appctrl_ctrl_buzzer(0, buzzerStatus);
            }
            
            if(TRUE == ledStatus)
            {
                ledStatus = FALSE;
                appctrl_ctrl_led(EMLED_ALARM, ledStatus);
            }
            guiStop = TRUE;
            buzzerStop = TRUE;
            ledStop = TRUE;
            return;
        }
        case NEW_ALARM:
        {
            CTRL_FMT_TRACE("新告警 ctrlype %d, voice %d\n", g_AlarmInfo.ctrlType, g_AlarmInfo.voiceIndex);

            //提示框
            guiStop = FALSE;
            if(FALSE == appctrl_alarm_hint())
            {
                guiStop = TRUE;
                g_AlarmInfo.pFunc = NULL;
                CTRL_FMT_TRACE("告警画面刷新结束!\n");
            }

            //总加组
            if(g_AlarmInfo.group < OOP_MAX_TG)
            {
                appctrl_show_guiinfo(g_AlarmInfo.group + 1);
            }
            else
            {
                appctrl_show_guiinfo(0);
            }

            //-蜂鸣器打开-
            buzzerStatus = TRUE;
            voiceTimes = g_AlarmInfo.alarmTimes - 1;
            voiceDelay = g_AlarmInfo.alarmInterval;
            appctrl_ctrl_buzzer(0, buzzerStatus);
            buzzerStop = FALSE;

            //led打开
            ledStatus = TRUE;
            appctrl_ctrl_led(EMLED_ALARM, ledStatus);
            ledStop = FALSE;
            ledDelay = g_Ctrlshowtime;

            g_AlarmInfo.alarmStep = ALARM_ING;
            break;
        }
        case ALARM_ING:
        {    
            //-蜂鸣器状态维护-
            if(0 != voiceTimes)
            {
                if(voiceDelay == g_AlarmInfo.alarmInterval)
                {
                    appmain_audio_set(g_AlarmInfo.voiceIndex);
                    appctrl_alarm_zcu1(TRUE);
#ifdef PRODUCT_SEU
                    appctrl_alarm_out();
#endif
                }
                if(0 == voiceDelay)
                {
                    buzzerStatus = TRUE - buzzerStatus;
                    appctrl_ctrl_buzzer(0, buzzerStatus);
                    if(FALSE == buzzerStatus)
                    {
                        voiceTimes--;
                    }
                    voiceDelay = g_AlarmInfo.alarmInterval;
                }
                else
                {
                    voiceDelay--;
                }
            }
            else
            {
                if(buzzerStop == FALSE)
                {
                    buzzerStatus = FALSE;
                    buzzerStop = TRUE;
                    appctrl_ctrl_buzzer(0, buzzerStatus);
                    CTRL_FMT_TRACE("蜂鸣器告警结束!\n");
                }
            }
    
            //提示框更新时间
            if(FALSE == guiStop)
            {
                if(FALSE == appctrl_alarm_hint())
                {
                    guiStop = TRUE;
                    g_AlarmInfo.pFunc = NULL;
                    CTRL_FMT_TRACE("告警画面刷新结束!\n");
                }
            }

            //led更新
            if(ledDelay > 0)
            {
                ledStatus = TRUE - ledStatus;
                appctrl_ctrl_led(EMLED_ALARM, ledStatus);
                ledDelay--;

            }
            else
            {
                if(FALSE == ledStop)
                {
                    ledStatus = FALSE;
                    appctrl_ctrl_led(EMLED_ALARM, ledStatus);
                    ledStop = TRUE;
                    CTRL_FMT_TRACE("LED闪烁结束!\n");
                }
            }

            if(TRUE == guiStop && TRUE == buzzerStop && TRUE == ledStop)
            {
                g_AlarmInfo.alarmStep = ALARM_OVER;
                CTRL_FMT_TRACE("告警结束!\n");
                appctrl_show_guiinfo(0);
            }
            break;
        }
        default:
        {
            g_AlarmInfo.alarmStep = ALARM_OVER;
            break;
        }
    }
}

/**********************************************************************
* @name      : appctrl_alarm_data
* @brief     ：填写告警信息数据结构，并维护告警信息队列
* @param[in] ：uint8 ctrltype       控制类型告警
               uint8 group          总加组
               uint8 times          告警次数
               uint8 Voindex        语音索引
               CTRL_SHOW_HINT pfunc 显示构造函数
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-14
* @Update    : 2020-4-29
**********************************************************************/
void appctrl_alarm_data(uint8 ctrltype, uint8 group, uint8 times, uint8 Voindex, CTRL_SHOW_HINT pfunc)
{
    g_AlarmInfo.voiceIndex = Voindex;
    g_AlarmInfo.alarmTimes = times;
    g_AlarmInfo.alarmInterval = ALARM_INTERVAL - 1; 
    g_AlarmInfo.alarmStep = NEW_ALARM;
    
    if(1 != Voindex && 2 != Voindex)
    {
        /* 参数更改、中文信息不刷新弹显 */
        g_AlarmInfo.ctrlType = ctrltype;
        g_AlarmInfo.group = group;
        g_AlarmInfo.pFunc = pfunc;
    }
}

/**********************************************************************
* @name      : appctrl_alarm_stop
* @brief     ：告警消除
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-4
* @Update    : 
**********************************************************************/
void appctrl_alarm_stop()
{
    appctrl_show_guiinfo(0);
    g_AlarmInfo.pFunc = NULL;
    g_AlarmInfo.alarmStep = ALARM_OVER;
    g_AlarmInfo.alarmTimes = 0;         //-告警次数清零-
    g_AlarmInfo.alarmStep = ALARM_OVER; //-告警状态为告警结束-
    g_AlarmInfo.ctrlType = CTRL_NULL_TYPE;
}

/**********************************************************************
* @name      : appctrl_show_guistatus
* @brief     ：新的告警状态栏内容
* @param[in] ：uint32 delaytime     持续时间
               uint8 *pShow         显示内容
               uint16 len           长度
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-4
* @Update    :
**********************************************************************/
void appctrl_show_guistatus(uint32 delaytime, uint8 *pShow, uint16 showlen)
{
    uint16 len = 0;
    uint8  buf[128] = {0};
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;
    int   ret = 0;
    
    appmsg_memcpy_r(buf, &delaytime, sizeof(uint32));
    len += 4;

    bytes = appmsg_set_len_offset(showlen, bufLen);
    memcpy(buf + len, bufLen, bytes);
    len += bytes;

    memcpy(buf + len, pShow, showlen);
    len += showlen;

    ret = lctrl_send_deskmsg(0, DESKGUI_IOP_FRESH_STATE, buf, len);
    if(0 != ret)
    {
        CTRL_BUF_DEBUG(buf, len, "Send gui status msg failed, ret %d", ret);
    }
}

/**********************************************************************
* @name      : appctrl_show_guiinfo
* @brief     ：设置信息点号
* @param[in] ：uint16 info 信息点号
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-4
* @Update    :
**********************************************************************/
void appctrl_show_guiinfo(uint16 info)
{
    uint16 len = 0;
    uint8  buf[6] = {0};
    int   ret = 0;
    
    buf[len++] = 1;
    buf[len++] = 4;
    appmsg_memcpy_r(buf + len, &info, sizeof(uint16));
    len += 2;

    ret = lctrl_send_deskmsg(0, DESKGUI_IOP_FRESH_TOOL, buf, len);
    if(0 != ret)
    {
        CTRL_BUF_DEBUG(buf, len, "Send gui tool msg failed, ret %d", ret);
    }
}

/**********************************************************************
* @name      : appctrl_show_guihint
* @brief     ：新的告警提示
* @param[in] ：uint32 delaytime     持续时间
               uint8 *pShow         显示内容
               uint16 len           长度
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-4
* @Update    :
**********************************************************************/
void appctrl_show_guihint(uint32 delaytime, uint8 *pShow, uint16 showlen)
{
    uint16 len = 0;
    uint8  buf[400] = {0};
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;
    int   ret = 0;
    
    appmsg_memcpy_r(buf, &delaytime, sizeof(uint32));
    len += 4;

    bytes = appmsg_set_len_offset(showlen, bufLen);
    memcpy(buf + len, bufLen, bytes);
    len += bytes;

    memcpy(buf + len, pShow, showlen);
    len += showlen;

    ret = lctrl_send_deskmsg(0,  DESKGUI_IOP_PUSH_HINT, buf, len);
    if(0 != ret)
    {
        CTRL_BUF_DEBUG(buf, len, "Send gui hint msg failed, ret %d", ret);
    }
}

