/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ң������ģ��
* @date��    2019-10-29
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "hal.h"
#include "db_api.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "iniparser.h"

#include "calculate.h"
#include "pulse_adv.h"
#include "framecomm.h"

#define     PULSE_DATA  "pulse.date"

DB_NOTIFY_T        g_PulseDB;
PULSE_CFG_T        g_PluseCfg;      //ң����������

PULSE_OAD_T        g_tSwitchPulse;  //ң����������
PULSE_CALCULATE_T  g_tCalculate;    //ң����������м�����

pthread_mutex_t    g_fPulseNormallock;	
uint8              g_PulseDBNormal = FALSE;

pthread_mutex_t    g_fPulseArglock;
uint8              g_PulseDBArg = FALSE;

uint8              g_PulseGetFlag = TRUE;
extern uint8 gdelay;

/**********************************************************************
* @name      : pulse_get_config
* @brief     ���������ļ��л�ȡң������
* @param[in] ��char *filepath�����ļ�
* @param[out]��
* @return    ��ң������
* @Create    : ����
* @Date      ��2020-2-29
* @Update    :
**********************************************************************/
void pulse_get_config(dictionary *ini)
{
    int    intvalue = 0;
    char   *stringvalue = NULL;
    uint8         i;
    uint8         j;
    char    keyPort[32] = {0};
    char    keyCfg[32] = {0};
    char    keySet[32] = {0};
    int            len = 0;
    char   cfgstr[256] = {0};
    uint16  offset = 0;
    
    memset(&g_PluseCfg, 0, sizeof(PULSE_CFG_T));
    
    if(NULL == ini)
    {
        return;
    }

    stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:DATAPATH", "/data/app/puAmr/data");
    if(0 != access(stringvalue, F_OK))
    {
        if(0 != mkdir(stringvalue, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            PULSE_FMT_DEBUG("Create data filepath %s, failed\n", stringvalue);
        }
    }
    sprintf(g_PluseCfg.datafile, "%s/%s", stringvalue, PULSE_DATA);

    for(i = 0; i < PULSE_MODEL_MAX; i++)
    {
        g_PluseCfg.cfg[i].no = i;
        
        sprintf(keyPort, "PULSE_CONF:MODEL%d_PORT", i + 1);
        intvalue = iniparser_getint(ini, keyPort, 4);
        if(0 == i && intvalue > 24)
        {
            intvalue = 24;
        }
        else if(i > 0 && intvalue > 8)
        {
            intvalue = 8;
        }
        g_PluseCfg.cfg[i].portNum  = intvalue;

        sprintf(keyCfg, "PULSE_CONF:MODEL%d_CFG", i + 1);
        stringvalue = iniparser_getstring(ini, keyCfg, "0000");
        len = strlen(stringvalue);
        memset(cfgstr, 0, 256);
        offset = 0;
        for(j = 0; j < len && j < g_PluseCfg.cfg[i].portNum; j++)
        {
            if('0' == stringvalue[j])
            {
                g_PluseCfg.cfg[i].config[j] = 0;
                offset += sprintf(cfgstr + offset, " %s", "ң��");
            }
            else
            {
                g_PluseCfg.cfg[i].config[j] = 1;
                offset += sprintf(cfgstr + offset, " %s", "����");
                g_PluseCfg.bHasPulse = 1;
            }
        }
        g_PluseCfg.cfg[i].portNum  = intvalue;

        
        sprintf(keySet, "PULSE_CONF:MODEL%d_SET", i + 1);
        intvalue = iniparser_getint(ini, keyPort, 1);
        if(intvalue > 1)
        {
            intvalue = 1;
        }
        g_PluseCfg.cfg[i].bSetCfg = intvalue;

        PULSE_FMT_DEBUG("�豸%d(%s)����: �˿�%d, %s֧�ֹ�������, ��ʼ����%s\n", g_PluseCfg.cfg[i].no, 
                        0 == g_PluseCfg.cfg[i].no ? "����" : "ģ��", g_PluseCfg.cfg[i].portNum,
                        0 == g_PluseCfg.cfg[i].bSetCfg ? "��" : "", cfgstr);
    }

    for(i = 0; i < g_PluseCfg.cfg[0].portNum; i++)
    {
        if(0 == g_PluseCfg.cfg[0].config[i])
        {
            g_PluseCfg.mapSTStart = i + 1;
        }
    }
    PULSE_FMT_DEBUG("ģ��ң�ſ�ʼ���%d\n", g_PluseCfg.mapSTStart);
}

/* ��ȡ���� */
uint32 read_oad_notify(DB_NOTIFY_T *pQueue)
{
    uint32 headOad = 0;
    PTHREAD_LOCK_SAFE(&pQueue->f_lock);  /*��������*/

    /*�������������ݣ���ȡ��buf�У��⿪��*/
    if(0 < pQueue->num)
    {
        headOad = pQueue->OAD[0];
        memmove(&(pQueue->OAD[0]), &(pQueue->OAD[1]), (pQueue->num-1)*sizeof(uint32));
        pQueue->OAD[pQueue->num-1] = 0;
        (pQueue->num)--;
    }

    PTHREAD_UNLOCK_SAFE(&pQueue->f_lock);
    return headOad;
}

/* ������� */
int write_oad_notify(DB_NOTIFY_T *pQueue, uint32 tailOad)
{
    if(NULL == pQueue)
    {
        return -1;
    }

    PTHREAD_LOCK_SAFE(&pQueue->f_lock);  /*��������*/
    if(PULSE_DB_QUEUE_SIZE <= pQueue->num || 0 == tailOad)
    {
        return -1;
    }
    pQueue->OAD[pQueue->num] = tailOad;
    (pQueue->num)++;
    
    PTHREAD_UNLOCK_SAFE(&pQueue->f_lock);
    return 0;
}

/**********************************************************************
* @name      : pulse_oad_notify
* @brief     ������oad
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-1-7
* @Update    :
**********************************************************************/
int pulse_oad_notify(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    uint8 id = 0;
    if(0x24010000 < value && value < 0x24080401)
    {   
        id  = (((value & 0x000f0000) >> 16) & 0x0f);
        if(id > 8 || 0 == id)
        {
            return -1;
        }
    }
    return write_oad_notify(&g_PulseDB, value);
}

/**********************************************************************
* @name      : pulse_oad_notify_init
* @brief     ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-1-10
* @Update    :
**********************************************************************/
void pulse_oad_notify_init()
{
    pthread_mutex_init(&g_PulseDB.f_lock, NULL);
}

/**********************************************************************
* @name      : pulse_normal_init
* @brief     ���������ĳ�ʼ��
* @param[out]��uint8 logicid  �߼��豸��
               uint8 flag     ��ʼ�����
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-1-10
* @Update    :
**********************************************************************/
void pulse_normal_init()
{
    pthread_mutex_init(&g_fPulseNormallock, NULL);
}

/**********************************************************************
* @name      : pulse_set_normalflag
* @brief     ���������ĳ�ʼ��
* @param[out]��uint8 logicid  �߼��豸��
               uint8 flag     ��ʼ�����
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-3-6
* @Update    :
**********************************************************************/
void pulse_set_normalflag(uint8 logicid, uint16 classTag, uint8 flag)
{
    if(0 == logicid && (0 == classTag || CLASS_DATA_NORMAL == classTag))
    {
        PTHREAD_LOCK_SAFE(&g_fPulseNormallock);
        g_PulseDBNormal = flag;
        PTHREAD_UNLOCK_SAFE(&g_fPulseNormallock);
    }
}

/**********************************************************************
* @name      : pulse_get_normalflag
* @brief     ���������ĳ�ʼ��
* @param[out]��
* @param[out]��
* @return    ����ʼ�����
* @Create    : ����
* @Date      ��2020-3-6
* @Update    :
**********************************************************************/
uint8 pulse_get_normalflag()
{
    uint8 flag = 0;
    PTHREAD_LOCK_SAFE(&g_fPulseNormallock);
    flag = g_PulseDBNormal;
    PTHREAD_UNLOCK_SAFE(&g_fPulseNormallock);
    return flag;
}

/**********************************************************************
* @name      : pulse_arg_init
* @brief     ���������ĳ�ʼ��
* @param[out]��uint8 logicid  �߼��豸��
               uint8 flag     ��ʼ�����
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-1-10
* @Update    :
**********************************************************************/
void pulse_arg_init()
{
    pthread_mutex_init(&g_fPulseArglock, NULL);
}

/**********************************************************************
* @name      : pulse_set_defaultflag
* @brief     ���������ĳ�ʼ��
* @param[out]��uint8 logicid  �߼��豸��
               uint8 flag     ��ʼ�����
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-1-10
* @Update    :
**********************************************************************/
void pulse_set_argflag(uint8 logicid, uint16 classTag, uint8 flag)
{
    if(0 == logicid && (0 == classTag || CLASS_DATA_INIT == classTag))
    {
        pthread_mutex_lock(&g_fPulseArglock);
        g_PulseDBArg = flag;
        pthread_mutex_unlock(&g_fPulseArglock);
    }
}

/**********************************************************************
* @name      : pulse_get_defaultflag
* @brief     ���������ĳ�ʼ��
* @param[out]��
* @param[out]��
* @return    ����ʼ�����
* @Create    : ����
* @Date      ��2020-3-6
* @Update    :
**********************************************************************/
uint8 pulse_get_argflag()
{
    uint8 flag = 0;
    pthread_mutex_lock(&g_fPulseArglock);
    flag = g_PulseDBArg;
    pthread_mutex_unlock(&g_fPulseArglock);
    return flag;
}

/**********************************************************************
* @name      : pulse_get_ok
* @brief     ������ͳ���Ƿ�OK(���ܼ���ʹ��)
* @param[out]��
* @param[out]��
* @return    ������״̬
* @Create    : ����
* @Date      ��2020-6-8
* @Update    :
**********************************************************************/
uint8 pulse_get_ok()
{
    return g_PulseGetFlag;
}

extern BOOLEAN advance_mqtt_rspok();

/**********************************************************************
* @name      : pulse_task_entry
* @brief     ��ң������ģ���߳����
* @param[in] ��void *pArg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-12-23
* @Update    : 2020-6-1
**********************************************************************/
int pulse_task_entry(uint32 dbip, uint16 dbport)
{
    int             errCode = 0;
    BOOLEAN         bFirst = TRUE;
    uint32          OAD = 0;
    TIME_STRUCT_T   tCurrTime = {0};
    TIME_STRUCT_T   tLastTimes = {0};
    TIME_STRUCT_T   tLastTime = {0};

    uint8       recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;
    int         recvlen = 0;
    uint32      sender = 0;
    int         pulseTask = 0;
    int         pulseTaskSy = 0;
    uint8       bNoRmt = TRUE;
    
    int         ret = 0;
    uint8       bGetPulse = 0;

    /* ����Taskͨ�ž�� */
    pulseTask = task_msg_init(PULSE_MSG);
    if(pulseTask <= 0)
    {
        PULSE_FMT_TRACE("PULSE_MSG task msg initfailed\n");
        return 0;
    }
    task_msg_settime(pulseTask, 0, 100000);  //����������ʱʱ��Ϊ0.1s

    pulseTaskSy = task_msg_init(PULSE_SYNCMSG);
    if(pulseTaskSy <= 0)
    {
        PULSE_FMT_TRACE("PULSE_SYNCMSG task msg initfailed\n");
        return 0;
    }
    task_msg_settime(pulseTaskSy, 0, 100000);  //����������ʱʱ��Ϊ0.1s

    /* ��ȡ�����������ú����� */
    memset(&g_tSwitchPulse, 0, sizeof(PULSE_OAD_T));
    errCode = pulse_dbCenter_init(&g_tSwitchPulse);
    if(-2 == errCode)
    {
        PULSE_FMT_DEBUG("pulse_dbCenter_init! err:%d", errCode);
        ret = -1;
        goto end;
    }

    memset(&g_tCalculate, 0, sizeof(PULSE_CALCULATE_T));
    /* ��ȡң���� */
    g_tCalculate.pCfg = &g_PluseCfg;
    memcpy(g_tCalculate.datafile, g_PluseCfg.datafile, 256);

    /* ��ʼ������ */
    errCode = pulse_calculate_init(&g_tCalculate, &tLastTime);
    if(errCode != 0)
    {
        PULSE_FMT_DEBUG("pulse_calculate_init! err:%d\n", errCode);
        ret = -1;
        goto end;
    }

    /* �ȴ�APPע�� */
    while(FALSE == advance_mqtt_rspok())
    {
        sleep(1);
        continue;
    }

    /* ң��APP������ɺ󣬵�һ��ʱ���ң��APP��ʼ�� */
    sleep(20);

    /* �״����� ����ң������ */
    ret = app_set_configure(pulseTaskSy, g_tCalculate.pCfg);
    if(0 != ret)
    {
        PULSE_FMT_DEBUG("app_set_configure failed\n");
        ret = -2;
        goto end;
    }
    /* ����ң�ŷ���ʱ�� */
    app_set_shaketime(pulseTask, g_tSwitchPulse.rmtTime);

    /* ����ң��״̬Ĭ��ֵ, ��������������ʱ���� */
    app_reproc_rmt(&g_tSwitchPulse, &g_tCalculate, g_tSwitchPulse.bInitSwitch);

    /* �״�������ѯң��״̬ */
    app_get_rmt(pulseTask, g_tCalculate.pCfg);

    g_tSwitchPulse.bFirstGetRmt1 = TRUE;
    g_tSwitchPulse.bFirstGetRmt2 = TRUE;
    
    /* ѭ���������ݡ�ң�š����� */
    while(1)
    {
        /* ������ʼ�� */
        if(TRUE == pulse_get_argflag())
        {
            pulse_set_argflag(0, 0, FALSE);
            pulse_arg_arg_init(&g_tSwitchPulse, &g_tCalculate);
            sleep(20);  //�ȴ�20s �ն˻�����
        }

        /* ����ʼ�� */
        if(TRUE == pulse_get_normalflag())
        {
            pulse_set_normalflag(0, 0, FALSE);
            pulse_write_normal_init(&g_tSwitchPulse, &g_tCalculate);
        }

        /* OAD�仯��ȡ */
        OAD = read_oad_notify(&g_PulseDB);
        while(0 != OAD)
        {
            /* �������K�仯��ǰ���� */
            if(0x24000400 == (OAD & 0xfff0ff00) || 0x24001C00 == (OAD & 0xfff0ff00))
            {
                pulse_change_K(pulseTaskSy, OAD, &g_tSwitchPulse, &g_tCalculate);
            }
            pulse_update_db(OAD, &g_tSwitchPulse);

            /* �������Ա仯, ���»�ȡһ��ң������ */
            if(0xf2030400 == OAD)
            {
                app_get_rmt(pulseTask, g_tCalculate.pCfg);
            }

            OAD = read_oad_notify(&g_PulseDB);
        }

        if(TRUE == g_tSwitchPulse.bChangeTime)
        {
            /* ����ʱ��仯 */
            g_tSwitchPulse.bChangeTime = FALSE;
            app_set_shaketime(pulseTask, g_tSwitchPulse.rmtTime);
        }

        /* ���ճ�ʱʱ����0.1s ����Ҫ��ִ��usleep */
        recvlen = task_recv_msg(pulseTask, recvBuf, TASK_MSG_MAX_LEN, &sender);
        if(recvlen > 0)
        {
            PULSE_BUF_DEBUG(pMsg->msg, pMsg->msglen, "Recv msgid 0x%x, label %d\n", pMsg->msg_id, pMsg->label);
            if(MSG_PULSE_POWER_DOWN == pMsg->msg_id)
            {
                PULSE_FMT_DEBUG("Reveice power down!\n");
                if(TRUE == g_PluseCfg.bHasPulse)
                {
                    app_get_pulse(pulseTaskSy, &g_tCalculate);
                    pulse_proc_calculate(&g_tSwitchPulse, &g_tCalculate, &tCurrTime);
                    pulse_write_pulse(&g_tCalculate);
                }
                sleep(10); //�յ�����ͣ�磬���϶�ȡһ���������� ��ֹ���ݶ�ʧ
                continue;
            }
            if(MSG_PULSE_GET_RMT_ENENT == pMsg->msg_id)
            {
                /* ң�ű�λ�¼������ϱ� */
                bNoRmt = FALSE;
                app_proc_rmt(*pMsg->msg, pMsg->msg + 1, pMsg->msglen - 1, TRUE, &g_tSwitchPulse, &g_tCalculate);
                if(1 == *pMsg->msg || 0xFF == *pMsg->msg)
                {
                    g_tSwitchPulse.bFirstGetRmt1 = FALSE;
                }
                if(2 == *pMsg->msg || 0xFF == *pMsg->msg)
                {
                    g_tSwitchPulse.bFirstGetRmt2 = FALSE;
                }
            }
            if(MSG_PULSE_GET_RMT == pMsg->msg_id)
            {
                /* ң��״̬��ѯ��� */
                if(255 == pMsg->label && 0 != pMsg->msg[0])
                {
                    bNoRmt = FALSE;
                }
                app_proc_rmt((uint8)pMsg->label, pMsg->msg, pMsg->msglen, FALSE, &g_tSwitchPulse, &g_tCalculate);
                if(0 != pMsg->msg[0])
                {
                    g_tSwitchPulse.bFirstGetRmt1 = FALSE;
                    g_tSwitchPulse.bFirstGetRmt2 = FALSE;
                }
            }
            memset(recvBuf, 0, recvlen);
        }

        /* ��������ɼ�������, ��ͳ��ģ��ʹ�� */
        pulse_get_curtime_s(&tCurrTime);
        if(tCurrTime.second >= 58)
        {
            g_PulseGetFlag = FALSE; //���ֿ�ʼǰ2s���ñ��
        }
        else if(tCurrTime.second >= 2)
        {
            g_PulseGetFlag = TRUE; //���ó�ʱʱ��2s
        }

        /* �״��ϵ� ���� �ɼ��������� */
        if((TRUE == bFirst || TRUE == bGetPulse) && TRUE == g_PluseCfg.bHasPulse)
        {
            app_get_pulse(pulseTaskSy, &g_tCalculate);
            bFirst = FALSE;
        }

        /* ���ּ��� */
        if(TRUE == bGetPulse && TRUE == g_PluseCfg.bHasPulse)
        {
            pulse_proc_calculate(&g_tSwitchPulse, &g_tCalculate, &tCurrTime);
            g_PulseGetFlag = TRUE;  //ʵ�ʼ��������ǰ�������λ
            pulse_write_pulse(&g_tCalculate);
        }

        /* �ۺ���ʱ0.2s��ȡ��������,��ֹң������APP���ֵ�����û�вɼ���� */
        if(tCurrTime.minute != tLastTime.minute)
        {
            usleep(100000);
            bGetPulse = TRUE;
            tLastTime = tCurrTime;
        }
        else
        {
            bGetPulse = FALSE;
        }

        /* �Žڵ��ȡ */
        pulse_check_status(&g_tSwitchPulse);

        /* û�л�ȡң�ţ�ÿ2s��һ�� */
        if(TRUE == bNoRmt)
        {
            if((tCurrTime.minute != tLastTimes.minute || tCurrTime.second != tLastTimes.second) &&
                0 == tCurrTime.second % 2)
            {
                tLastTimes = tCurrTime;
                app_get_rmt(pulseTask, g_tCalculate.pCfg);
            }
        }
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

end:
    if(pulseTask > 0)
    {
        close(pulseTask);
    }
    if(pulseTaskSy > 0)
    {
        close(pulseTaskSy);
    }
    if(g_tSwitchPulse.pHalDev)
    {
        hal_device_release((HW_DEVICE *)g_tSwitchPulse.pHalDev);
        g_tSwitchPulse.pHalDev = NULL;
    }
    return ret;
}

/**********************************************************************
* @name      : pulse_get_rmtcfg
* @brief     ����ȡң������״̬
* @param[out]��
* @param[out]��
* @return    ��������״̬
* @Create    : ����
* @Date      ��2020-7-25
* @Update    :
**********************************************************************/
uint8 pulse_get_rmtcfg()
{
    uint8 ststus = 0;
    uint8 i, j;
    uint8 num = 0;

    for(i = 0; i < PULSE_MODEL_MAX; i++)
    {
        for(j = 0; j < g_PluseCfg.cfg[i].portNum; j++)
        {
            if(num > 8)
            {
                break;
            }
            if(0 == g_PluseCfg.cfg[i].realcfg[j])
            {
                ststus |= (0x01 << num);
                num++;
            }
        }
    }
    return ststus;
}

