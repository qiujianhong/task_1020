/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ����ģ�� �������
* @date��    2020-4-11
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "appmsg_split.h"
#include "led.h"

#include "appctrl_comm.h"
#include "appctrl_alarm.h"
#include "appctrl_dev.h"
#include "framecomm.h"
#include "msg_id.h"


ALARM_CTRL_T    g_AlarmInfo;

pthread_mutex_t g_fCtrlKey;
uint8           g_CtrlKey = 0;         //��������
uint8           g_AlarmDelay = 0;	   //-ÿ�θ澯���-
uint8           g_AlarmMaxTurn = 0;
uint32          g_Ctrlshowtime = 120;  //Ĭ�ϵ���ʾ��ʱʱ��
uint8           g_zcu1_alarm = 0;

extern void appmain_audio_set(int indexno);

/**********************************************************************
* @name      : appctrl_alarm_init
* @brief     �����Ƹ澯��ʼ��
* @param[in] ��uint8 maxturn
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-4-29
* @Update    :
**********************************************************************/
void appctrl_alarm_init(uint8 maxturn)
{
    g_AlarmMaxTurn = maxturn;
    pthread_mutex_init(&g_fCtrlKey, NULL);
}

/**********************************************************************
* @name      : appctrl_show_ifvoice
* @brief     �����Ƹ澯��ʾ
* @param[in] ��uint8 index       ���
               uint8 num        ���Ŵ���
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2023-2-24
* @Update    :
**********************************************************************/
void appctrl_show_ifvoice(uint8 index, uint8 num)
{
    uint16 len = 0;
    uint8  buf[2] = {0};

    if(0 == index)
    {
        return;
    }
    buf[len++] = index;
    buf[len++] = num;

    appmain_send_msg(MSG_CTRL_IF_VOICE, 0, buf, len);
}

/**********************************************************************
* @name      : appctrl_alarm_turnnum
* @brief     �������ִ�����
* @param[in] ��uint8 maxturn
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-4-29
* @Update    :
**********************************************************************/
void appctrl_alarm_turnnum(uint8 turnmax)
{
    g_AlarmMaxTurn = turnmax;
}

/**********************************************************************
* @name      : appctrl_get_key
* @brief     ����ȡ��������״̬
* @param[in] ��
* @param[out]��
* @return    ������״̬
* @Create    : ����
* @Date      ��2020-4-29
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
* @brief     ����ȡ��������״̬
* @param[in] ��uint8 status   ����״̬ TRUE-FALSE
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-4-29
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
* @brief     �����Ƹ澯���ã�ר1��
* @param[in] ��uint8 status   ״̬
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2023-1-18
* @Update    :
**********************************************************************/
void appctrl_alarm_zcu1(uint8 status)
{
   g_zcu1_alarm = status;
}

/**********************************************************************
* @name      : appctrl_alarm_zcu1_get
* @brief     �����Ƹ澯���ã�ר1��
* @param[in] ��uint8 status   ״̬
* @param[out]��
* @return    ���澯״̬
* @Create    : ����
* @Date      ��2023-1-18
* @Update    :
**********************************************************************/
uint8 appctrl_alarm_zcu1_get()
{
   return g_zcu1_alarm;
}


/**********************************************************************
* @name      : appctrl_alarm_hint
* @brief     ��������ʾ��ʾ��Ϣ����������ʾ
* @param[in] ��
* @param[out]��
* @return    ��TRUE-����ִ��/FALSE-ִ�н���
* @Create    : ����
* @Date      ��2020-4-30
* @Update    :
**********************************************************************/
uint8 appctrl_alarm_hint()
{
    uint16 len = 0;
    uint8  buf[420] = {0};
    uint8  bufMsg[428] = {0};
    uint16 offset = 0;
    uint8  status = FALSE;
    uint32 showtime = g_Ctrlshowtime; //Ĭ�ϵ���ʾʱ��s
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

        ret = appmain_send_msg(MSG_CTRL_GUI_HINT, g_AlarmInfo.ctrlType, bufMsg, offset);
        if(0 != ret)
        {
            CTRL_BUF_DEBUG(bufMsg, offset, "Send gui type %d hint msg failed, ret %d", g_AlarmInfo.ctrlType, ret);
        }
    }
    return status;
}

/**********************************************************************
* @name      : appctrl_system_alarm
* @brief     �����ݵ�ǰբ״̬�и澯��Ϣ������������������ʾ�澯����
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-4-14
* @Update    : 2020-4-29
**********************************************************************/
void appctrl_system_alarm(uint8 turnmax)
{       
	static uint8 voiceTimes = 0;//-�澯����ʣ�����,��̬����-
    static uint8 voiceDelay = 0;//-�澯������ʱ-

    static uint8 buzzerStatus = TRUE;//-�������Ƿ��-
    static uint8 buzzerStop = TRUE;//-�������Ƿ����-
    static uint8 guiStop = TRUE;//-��ʾ�Ƿ����-

    static uint8 ledStatus = TRUE;//-led�Ƿ��-
    static uint8 ledStop = TRUE;  //-led�Ƿ����-
    static uint16 ledDelay = 0; //-�������-

    g_AlarmMaxTurn = turnmax;

    /* �������� ����澯��������� */
	if(TRUE == appctrl_get_key())
	{
		appctrl_set_key(FALSE);
        if(ALARM_OVER != g_AlarmInfo.alarmStep)
        {
            appctrl_show_guiinfo(0);
            g_AlarmInfo.alarmTimes = 0;         //-�澯��������-
            g_AlarmInfo.alarmStep = ALARM_OVER; //-�澯״̬Ϊ�澯����-
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
            CTRL_FMT_TRACE("�¸澯 ctrlype %d, voice %d\n", g_AlarmInfo.ctrlType, g_AlarmInfo.voiceIndex);  	
            //��ʾ��
            guiStop = FALSE;
            if(FALSE == appctrl_alarm_hint())
            {
                guiStop = TRUE;
                g_AlarmInfo.pFunc = NULL;
                CTRL_FMT_TRACE("�澯����ˢ�½���!\n");
            }

            //�ܼ���
            if(g_AlarmInfo.group < OOP_MAX_TG)
            {
                appctrl_show_guiinfo(g_AlarmInfo.group + 1);
            }
            else
            {
                appctrl_show_guiinfo(0);
            }

            //-��������-
            buzzerStatus = TRUE;
            voiceTimes = g_AlarmInfo.alarmTimes - 1;
            voiceDelay = g_AlarmInfo.alarmInterval;
            appctrl_ctrl_buzzer(0, buzzerStatus);
            buzzerStop = FALSE;

            //led��
            ledStatus = TRUE;
            appctrl_ctrl_led(EMLED_ALARM, ledStatus);
            ledStop = FALSE;
            ledDelay = g_Ctrlshowtime;

            g_AlarmInfo.alarmStep = ALARM_ING;
            break;
        }
        case ALARM_ING:
        {	
            //-������״̬ά��-
            if(0 != voiceTimes)
            {
                if(voiceDelay == g_AlarmInfo.alarmInterval)
                {
                    appmain_audio_set(g_AlarmInfo.voiceIndex);
                    appctrl_alarm_zcu1(TRUE);
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
                    CTRL_FMT_TRACE("�������澯����!\n");
                }
            }
	
            //��ʾ�����ʱ��
            if(FALSE == guiStop)
            {
                if(FALSE == appctrl_alarm_hint())
                {
                    guiStop = TRUE;
                    g_AlarmInfo.pFunc = NULL;
                    CTRL_FMT_TRACE("�澯����ˢ�½���!\n");
                }
            }

            //led����
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
                    CTRL_FMT_TRACE("LED��˸����!\n");
                }
            }

            if(TRUE == guiStop && TRUE == buzzerStop && TRUE == ledStop)
            {
                g_AlarmInfo.alarmStep = ALARM_OVER;
                CTRL_FMT_TRACE("�澯����!\n");
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
* @brief     ����д�澯��Ϣ���ݽṹ����ά���澯��Ϣ����
* @param[in] ��uint8 ctrltype       �������͸澯
               uint8 group          �ܼ���
               uint8 times          �澯����
               uint8 Voindex        ��������
               CTRL_SHOW_HINT pfunc ��ʾ���캯��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-4-14
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
        /* �������ġ�������Ϣ��ˢ�µ��� */
        g_AlarmInfo.ctrlType = ctrltype;
        g_AlarmInfo.group = group;
        g_AlarmInfo.pFunc = pfunc;
    }
}

/**********************************************************************
* @name      : appctrl_alarm_stop
* @brief     ���澯����
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-5-4
* @Update    : 
**********************************************************************/
void appctrl_alarm_stop()
{
    appctrl_show_guiinfo(0);
    g_AlarmInfo.pFunc = NULL;
    g_AlarmInfo.alarmStep = ALARM_OVER;
    g_AlarmInfo.alarmTimes = 0;         //-�澯��������-
    g_AlarmInfo.alarmStep = ALARM_OVER; //-�澯״̬Ϊ�澯����-
    g_AlarmInfo.ctrlType = CTRL_NULL_TYPE;
}

/**********************************************************************
* @name      : appctrl_show_guistatus
* @brief     ���µĸ澯״̬������
* @param[in] ��uint32 delaytime     ����ʱ��
               uint8 *pShow         ��ʾ����
               uint16 len           ����
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-5-4
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

    ret = appmain_send_msg(MSG_CTRL_GUI_STATUS, 0, buf, len);
    if(0 != ret)
    {
        CTRL_BUF_DEBUG(buf, len, "Send gui status msg failed, ret %d", ret);
    }
}

/**********************************************************************
* @name      : appctrl_show_guiinfo
* @brief     ��������Ϣ���
* @param[in] ��uint16 info ��Ϣ���
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-5-4
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

    ret = appmain_send_msg(MSG_CTRL_GUI_TOOL, 0, buf, len);
    if(0 != ret)
    {
        CTRL_BUF_DEBUG(buf, len, "Send gui tool msg failed, ret %d", ret);
    }
}

/**********************************************************************
* @name      : appctrl_show_guihint
* @brief     ���µĸ澯��ʾ
* @param[in] ��uint32 delaytime     ����ʱ��
               uint8 *pShow         ��ʾ����
               uint16 len           ����
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-5-4
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

    ret = appmain_send_msg(MSG_CTRL_GUI_HINT, 0, buf, len);
    if(0 != ret)
    {
        CTRL_BUF_DEBUG(buf, len, "Send gui hint msg failed, ret %d", ret);
    }
}

/**********************************************************************
* @name      : appctrl_show_ifhint
* @brief     �����Ƹ澯��ʾ
* @param[in] ��IN_CTRL_INFO_T *pCtrlInfo      ��ʾ����
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-5-4
* @Update    :
**********************************************************************/
void appctrl_show_ifhint(IN_CTRL_INFO_T *pCtrlInfo)
{
    uint16 len = 0;
    uint8  buf[1028] = {0};
    uint8  bufLen[3] = {0};
    uint8  bytes = 0;
    uint8  i = 0;
    
    buf[len++] = pCtrlInfo->group;
    buf[len++] = pCtrlInfo->type;

    bytes = appmsg_set_len_offset(strlen(pCtrlInfo->head), bufLen);
    memcpy(buf + len, bufLen, bytes);
    len += bytes;
    memcpy(buf + len, pCtrlInfo->head, strlen(pCtrlInfo->head));
    len += strlen(pCtrlInfo->head);

    bytes = appmsg_set_len_offset(strlen(pCtrlInfo->info), bufLen);
    memcpy(buf + len, bufLen, bytes);
    len += bytes;
    memcpy(buf + len, pCtrlInfo->info, strlen(pCtrlInfo->info));
    len += strlen(pCtrlInfo->info);

    buf[len++] = pCtrlInfo->times;

    buf[len++] = pCtrlInfo->num;
    for(i = 0; i < pCtrlInfo->num; i++)
    {
        buf[len++] = pCtrlInfo->turn[i].no;
        buf[len++] = pCtrlInfo->turn[i].curstatus;
        buf[len++] = pCtrlInfo->turn[i].futurestatus;
        buf[len++] = pCtrlInfo->turn[i].balarm;
        buf[len++] = pCtrlInfo->turn[i].hour;
        buf[len++] = pCtrlInfo->turn[i].min;
        buf[len++] = pCtrlInfo->turn[i].sec;
    }

    appmain_send_msg(MSG_CTRL_IF_HINT, 0, buf, len);
}

/**********************************************************************
* @name      : appctrl_db_chg_event
* @brief     �����Ƹ澯��ʾ
* @param[in] ��IN_CTRL_INFO_T *pCtrlInfo      ��ʾ����
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2022-12-15
* @Update    :
**********************************************************************/
void appctrl_db_chg_event(uint8 type, uint8 dbtype, uint32 value)
{
    uint16 len = 0;
    uint8  buf[6] = {0};
    
    buf[len++] = type;
    buf[len++] = dbtype;
    MEMCPY_R(buf + len, &value, 4);
    len += 4;

    appmain_send_msg(MSG_CTRL_DB_CHG, 0, buf, len);
}


