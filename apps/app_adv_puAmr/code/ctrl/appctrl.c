/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ����ģ�� �����߳�
* @date��    2020-3-31
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "hal.h"
#include "uart.h"

#include "appcomm.h"
#include "appctrl_comm.h"
#include "appctrl.h"
#include "appctrl_cfg.h"
#include "appctrl_power.h"
#include "appctrl_elec.h"
#include "appctrl_remote.h"
#include "appctrl_protect.h"
#include "appctrl_dev.h"
#include "appctrl_alarm.h"

#include "framecomm.h"


#define RSP_MAP_FUNC_CTRL     7
DB_CLIENT        g_CtrlDBClient = 0;
CTRL_CONDIG_T    g_CtrlCfg;          //����ģ����������

SWITCH_INTERFACE_T g_curSwitch;  //��ǰ����ģ�鶯��״̬
uint8            g_AdvPowerOff = FALSE; //�ǻ���Դ��Ԫͣ�粻������
extern void appmain_audio_set(int indexno);

/**********************************************************************
* @name      : appctrl_getmsg_port
* @brief     �����ͻ�ȡģ���ն���Ϣ��Ϣ
* @param[in] ��int sockedfd         task���
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-3-31
* @Update    : 
**********************************************************************/
void appctrl_getmsg_port(int sockedfd)
{
#ifdef PRODUCT_ECU
    int   ret = 0;
    ret = task_send_labelmsg(sockedfd, MSG_CTRL_MOD_GET_PORT, MSG_CTRL_FLAG, NULL, 0, MSG_MONITOR);
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Ctrl send get port mode ret %d\n", ret);
    }
#endif

#ifdef PRODUCT_ZCU_3
    /* ������� ͨ��hal�滻 */
    if(FALSE == g_CtrlCfg.bModeOk)
    {
        UART_DEVICE_T* dev;
        int  fd = 0;
        dev = (UART_DEVICE_T*)hal_device_get(UART_1_1_DEVICE_ID_STR);
        if(NULL == dev)
        {
            return ;
        }
        
        // ���豸
        if(0 != dev->uart_param_set(dev, 19200, 8, 1, PARITY_EVEN))
        {
            CTRL_FMT_DEBUG("���ô���%s����ʧ��\n", UART_1_1_DEVICE_ID_STR);
            hal_device_release((HW_DEVICE *)dev);
            return;
        }
        hal_device_release((HW_DEVICE *)dev);

        fd = open("/dev/tty_io", O_RDONLY);
        if(fd <= 0)
        {
           return;
        }
        ioctl(fd, CONTROL_CLEAR_RTS, 0);
        usleep(200000);
        ioctl(fd, CONTROL_SET_RTS, 0);          //���ڴ�,��ģ����Ϊ��������ģʽ
        close(fd);
        sleep(1);

        g_CtrlCfg.nACM1 = 1;
        g_CtrlCfg.nsubACM1 = 1;
        g_CtrlCfg.bModeOk = TRUE;
        g_CtrlCfg.turnNum = 4;
        
        CTRL_FMT_DEBUG("����ģ�鿪��\n");
    }
#endif

#ifdef PRODUCT_ZCU_1
    if(FALSE == g_CtrlCfg.bModeOk)
    {
        appctrl_switch_init_zcu_1();
        g_CtrlCfg.nACM1 = 1;
        g_CtrlCfg.nsubACM1 = 1;
        g_CtrlCfg.bModeOk = TRUE;
        g_CtrlCfg.turnNum = 8;
        
        CTRL_FMT_DEBUG("����ģ�鿪��\n");
    }
#endif

}

/**********************************************************************
* @name      : appctrl_modport_ack
* @brief     ����ѯģ��˿� ������Ϣ����
* @param[in] ��int cid          ��������cid
               uint16 label     ��Ϣlabel
               uint8* pMsg      ��Ϣ
               uint16 msglen    ��Ϣ����
* @param[out]��uint32* nextStep ��һ������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2022-6-20
* @Update    : 
**********************************************************************/
int appctrl_modport_ack(int cid, uint16 label, uint8* pMsg, uint16 msglen, uint32* nextStep)
{
    uint8           *temp = pMsg;
    uint16           offset = 0;
    uint32           i = 0;
    uint32           j = 0;
    uint8            type = 0;
    
    OOP_MODULE_INFO_NEW_T   tmodelInfo;
    uint8                  no = 0;
    uint8             port[2] = {0};
    uint8          subport[2] = {0};
    uint8               num = 0;
    uint8         alarmport = 0;
    uint8          alarmsub = 0;

    if(NULL == pMsg || NULL == nextStep || 0 == msglen)
    {
        return -1;
    }

    memset(&tmodelInfo, 0, sizeof(OOP_MODULE_INFO_NEW_T));

    tmodelInfo.nNum = temp[offset++];
    if(tmodelInfo.nNum > OOP_MAX_MODULE)
    {
        tmodelInfo.nNum = OOP_MAX_MODULE;
    }
    for(i = 0; i < tmodelInfo.nNum; i++)
    {
        tmodelInfo.item[i].nSlot = i + 1;
        type = temp[offset++];
        if(0 != type)
        {
            continue;
        }
        tmodelInfo.item[i].modType.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].modType.value, temp + offset, tmodelInfo.item[i].modType.nNum);
        offset += tmodelInfo.item[i].modType.nNum;
        
        tmodelInfo.item[i].modId.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].modId.value, temp + offset, tmodelInfo.item[i].modId.nNum);
        offset += tmodelInfo.item[i].modId.nNum;
        
        tmodelInfo.item[i].nSoftVer.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].nSoftVer.value, temp + offset, tmodelInfo.item[i].nSoftVer.nNum);
        offset += tmodelInfo.item[i].nSoftVer.nNum;

        tmodelInfo.item[i].softDate.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].softDate.value, temp + offset, tmodelInfo.item[i].softDate.nNum);
        offset += tmodelInfo.item[i].softDate.nNum;

        tmodelInfo.item[i].nHardVer.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].nHardVer.value, temp + offset, tmodelInfo.item[i].nHardVer.nNum);
        offset += tmodelInfo.item[i].nHardVer.nNum;

        tmodelInfo.item[i].hardDate.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].hardDate.value, temp + offset, tmodelInfo.item[i].hardDate.nNum);
        offset += tmodelInfo.item[i].hardDate.nNum;

        tmodelInfo.item[i].manuf.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].manuf.value, temp + offset, tmodelInfo.item[i].manuf.nNum);
        offset += tmodelInfo.item[i].manuf.nNum;

        tmodelInfo.item[i].nChNum = temp[offset++];
        for(j = 0; j < tmodelInfo.item[i].nChNum; j++)
        {
            tmodelInfo.item[i].chInfo[j].nType = temp[offset++];
            tmodelInfo.item[i].chInfo[j].nFunc = temp[offset++];
            tmodelInfo.item[i].chInfo[j].nDriveName.nNum = temp[offset++];
            memcpy(tmodelInfo.item[i].chInfo[j].nDriveName.value, temp + offset, tmodelInfo.item[i].chInfo[j].nDriveName.nNum);
            offset += tmodelInfo.item[i].chInfo[j].nDriveName.nNum;

            if(0 == memcmp(tmodelInfo.item[i].modType.value, "GK", 2) &&
               RSP_MAP_FUNC_CTRL == tmodelInfo.item[i].chInfo[j].nFunc && num <= 2)
            {
                port[num] = tmodelInfo.item[i].nSlot;
                subport[num] = j;
                num++;
            }
            if(0 == memcmp(tmodelInfo.item[i].modType.value, "GX", 2) && 
               RSP_MAP_FUNC_CTRL == tmodelInfo.item[i].chInfo[j].nFunc)
            {
                alarmport = tmodelInfo.item[i].nSlot;
                alarmsub = j;
            }
        }
        
    }

    memcpy(&g_CtrlCfg.modelInfo, &tmodelInfo, sizeof(OOP_MODULE_INFO_NEW_T));
    if(0 != num)
    {
        CTRL_FMT_DEBUG("û�п���ģ��\n");
        g_CtrlCfg.turnNum = 0;
        g_CtrlCfg.nACM1 = 0;
        g_CtrlCfg.nACM2 = 0;
        g_CtrlCfg.nsubACM1 = 0;
        g_CtrlCfg.nsubACM2 = 0;
    }

    for(i = 0; i < 2 && i < num; i++)
    {
        no = port[i] - 1;
        CTRL_FMT_DEBUG("����%dģ�����%d, �ͺ�: %s, ID: %s, ����: %s. ͨ��: %d\n", i + 1, tmodelInfo.item[no].nSlot,
                           tmodelInfo.item[no].modType.value, tmodelInfo.item[no].modId.value, 
                           tmodelInfo.item[no].manuf.value, subport[i]);
    }

    g_CtrlCfg.nACMAlarm = alarmport;
    g_CtrlCfg.nsubAlarm = alarmsub;
    if(g_CtrlCfg.nACMAlarm > 0)
    {
        CTRL_FMT_DEBUG("�澯ģ�����%d, �ͺ�: %s, ID: %s, ����: %s. ͨ��: %d\n", tmodelInfo.item[alarmport - 1].nSlot,
                           tmodelInfo.item[alarmport - 1].modType.value, tmodelInfo.item[alarmport - 1].modId.value, 
                           tmodelInfo.item[alarmport - 1].manuf.value, alarmsub);
    }

    g_CtrlCfg.bModeOk = TRUE;
    if(1 == num)
    {
        if(g_CtrlCfg.nACM2 == port[0])
        {
            g_CtrlCfg.nACM1 = 0;
            g_CtrlCfg.nsubACM1 = 0;
            g_CtrlCfg.nACM2 = port[0];
            g_CtrlCfg.nsubACM2 = subport[0];
        }
        else
        {
            g_CtrlCfg.nACM1 = port[0];
            g_CtrlCfg.nsubACM1 = subport[0];
            g_CtrlCfg.nACM2 = 0;
            g_CtrlCfg.nsubACM2 = 0;
        }
        g_CtrlCfg.turnNum = g_CtrlCfg.portNum;
    }
    else
    {
        g_CtrlCfg.nACM1 = port[0];
        g_CtrlCfg.nsubACM1 = subport[0];
        g_CtrlCfg.nACM2 = port[1];
        g_CtrlCfg.nsubACM2 = subport[1];
        g_CtrlCfg.turnNum = 2 * g_CtrlCfg.portNum;
    }

    *nextStep = CTRL_LABEL_WORK;
    return 0;
}

/**********************************************************************
* @name      : appctrl_mod_event
* @brief     ����ѯ���ƶ˿ں� ������Ϣ����
* @param[in] ��int cid          ��������cid
               uint16 label     ��Ϣlabel
               uint8* pMsg      ��Ϣ
               uint16 msglen    ��Ϣ����
* @param[out]��uint32* nextStep ��һ������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-3-31
* @Update    : 
**********************************************************************/
int appctrl_mod_event(int cid, uint16 label, uint8* pMsg, uint16 msglen, uint32* nextStep)
{
    uint32 port = 0;
    uint8  status = 0;
    uint8 *temp = pMsg;
    
    if(NULL == pMsg || NULL == nextStep || 0 == msglen)
    {
        return -1;
    }

    appmsg_memcpy_r(&port, temp, sizeof(uint32));
    temp += sizeof(uint32);
    status = *temp;

    if(port < CTRL_MOD_ID_MIN || port > CTRL_MOD_ID_MAX)
    {
        return -1;
    }

    CTRL_FMT_DEBUG("ģ��%d %s\n", port, 0 == status ? "����" : "�γ�");
    if(0 == port && port > OOP_MAX_MODULE)
    {
        return -2;
    }
    
    if(1 == status)
    {
        /* �γ� */
        memset(&(g_CtrlCfg.modelInfo.item[port-1]), 0, sizeof(OOP_MODULE_UINT_NEW_T));
        g_CtrlCfg.modelInfo.item[port-1].nSlot = port;
        
        if(port == g_CtrlCfg.nACM1)
        {
            g_CtrlCfg.nACM1 = 0;
            g_CtrlCfg.nsubACM1 = 0;
            CTRL_FMT_DEBUG("�����豸1�γ�\n");
        }
        if(port == g_CtrlCfg.nACM2)
        {
            g_CtrlCfg.nACM2 = 0;
            g_CtrlCfg.nsubACM2 = 0;
            CTRL_FMT_DEBUG("�����豸2�γ�\n");
        }
    }
    else
    {
        /* ���� */
        *nextStep = CTRL_LABEL_MODPORT;
    }
    return 0;
}

/**********************************************************************
* @name      : appctrl_get_devturn
* @brief     ����ȡʵ�ʽ����ִ�
* @param[in] ��
* @param[out]��uint8 *pTurnmax  ����ִ�
* @return    ��ʵ���ִ�
* @Create    : ����
* @Date      ��2020-7-17
* @Update    : 
**********************************************************************/
uint8 appctrl_get_devturn(uint8 *pTurnmax)
{
    *pTurnmax = g_CtrlCfg.turnMAX;
    return g_CtrlCfg.turnNum;
}

/**********************************************************************
* @name      : appctrl_gui_msg
* @brief     ��������Ϣ
* @param[in] ��int cid          ��������cid
               uint16 label     ��Ϣlabel
               uint8* pMsg      ��Ϣ
               uint16 msglen    ��Ϣ����
* @param[out]��uint32* nextStep ��һ������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-4-29
* @Update    : 
**********************************************************************/
int appctrl_gui_msg(int cid, uint16 label, uint8* pMsg, uint16 msglen, uint32* nextStep)
{
    if(NULL == pMsg || NULL == nextStep || 0 == msglen)
    {
        return -1;
    }
    
    /* ������Ϣ��FALSE��ʾ�������� */
    if(0 == *pMsg)
    {
        CTRL_FMT_DEBUG("���Ա�����ȡ��\n");
        appctrl_set_key(TRUE);
    }
    return 0;
}

/**********************************************************************
* @name      : appctrl_if_msg
* @brief     ��������Ϣ
* @param[in] ��int cid          ��������cid
               uint16 label     ��Ϣlabel
               uint8* pMsg      ��Ϣ
               uint16 msglen    ��Ϣ����
* @param[out]��uint32* nextStep ��һ������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-4-29
* @Update    : 
**********************************************************************/
int appctrl_if_msg(int cid, uint16 label, uint8* pMsg, uint16 msglen, uint32* nextStep)
{
    if(NULL == pMsg || NULL == nextStep || 0 == msglen)
    {
        return -1;
    }
    
    CTRL_FMT_DEBUG("���Ա�����ȡ��\n");
    appctrl_set_key(TRUE);
    return 0;
}

/**********************************************************************
* @name      : appctrl_dev_input
* @brief     ������ģ������ж�
* @param[in] ��uint8 turnno  �ִ����1-8
* @param[out]��
* @return    ��TRUE/FALSE
* @Create    : ����
* @Date      ��2020-11-6
* @Update    :
**********************************************************************/
uint8 appctrl_dev_input(uint8 turnno)
{
    /* Ĭ����4�� */
    if(turnno > 0 && turnno <= g_CtrlCfg.portNum)
    {
        return TRUE;
    }

    if(0 != g_CtrlCfg.nACM2)
    {
        if(turnno > g_CtrlCfg.portNum && turnno <= 4 * g_CtrlCfg.portNum)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**********************************************************************
* @name      : app_ctrl_init
* @brief     ������ģ���ʼ��
* @param[in] ��uint8 port    һ��ģ���п����ִ�
               uint8 turnMax �������ִ�
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-3-31
* @Update    :
**********************************************************************/
void app_ctrl_init(uint8 port, uint8 turnMax)
{
    uint8 i;

    memset(&g_CtrlCfg, 0, sizeof(CTRL_CONDIG_T));
    g_CtrlCfg.turnMAX = turnMax;
    g_CtrlCfg.portNum = port;

    memset(&g_curSwitch, 0, sizeof(SWITCH_INTERFACE_T));
    for(i = 0; i < OOP_MAX_TURN; i++)
    {
        g_curSwitch.TURN_CTRL[i] = 1;
    }
    g_curSwitch.ALARM_OUTPUT = 1;
    g_curSwitch.portMax = port;

    appctrl_switch_init(port, turnMax);
    appctrl_power_init();
    appctrl_elec_init();
    appctrl_remote_init();
    appctrl_alarm_init(turnMax);
}

/**********************************************************************
* @name      : appctrl_effect_proc
* @brief     ������Ͷ��/�������
* @param[in] ��
* @param[out]��uint8 *ctrlType ��������
* @return    ��TRUE-����Ͷ��;FALSE-���ƽ��
* @Create    : ����
* @Date      ��2020-5-4
* @Update    :
**********************************************************************/
uint8 appctrl_effect_proc(uint8 *ctrlType)
{
    uint8    effectType = 0;
    uint8    effectStatus = 0;
    uint16   group = 0;
    char     buf[64] = {0};
    char     alarm[400] = {0};
    uint16   buflen = 0;
    char    *ctrlName[] = {"�¸���", "Ӫҵ��ͣ��", "���ݿ�", "ʱ�ο�", "�����", "�µ��", "ң��", "����"};
    uint8    i = 0;
    char    *turnName[] = {"һ", "��", "��", "��", "��", "��", "��", "��"};
    char     bufPc[40] = {0};
    uint8    pclen = 0;
    char     bufEc[40] = {0};
    uint8    eclen = 0;
    int      indexno = 0;
    
    /* �����ڴ�ָ�� */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //����ģ���������
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //�ܼ������ü�������
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//����ͳ��
    uint8            PwTurn = 0;
    uint8            EcTurn = 0;

    /* ��ȡOAD���� */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    effectStatus = appctrl_get_neweffect(&effectType, &group, &indexno);
    if(0 == effectStatus || effectType > CTRL_PROTCET_TYPE || 0 == group || group > OOP_MAX_TG)
    {
        *ctrlType = CTRL_NULL_TYPE;
        return FALSE;
    }
    appctrl_no_neweffect();
    appctrl_ctrl_buzzer(0, TRUE);
    appctrl_alarm_stop();                    //����ϸ澯
#if defined PRODUCT_ZCU_1 || defined PRODUCT_SEU
    appctrl_alarm_data(effectType, 0, 3, indexno, NULL);
#endif

    buflen = sprintf(buf, "%s%s", ctrlName[effectType], 1 == effectStatus ? "Ͷ��" : "���");
    appctrl_show_guistatus(120, (uint8 *)buf, buflen);  //״̬��

    PwTurn = ptSubGroups[group-1].ctrlPwTurn.nValue;
    EcTurn = ptSubGroups[group-1].ctrlEcTurn.nValue;

    /* ��ʾ�µĸ澯���� */
#ifdef PRODUCT_ZCU_1
    buflen = sprintf(alarm, "\n              �ܼ��� %d ����\n", group);
    buflen += sprintf(alarm + buflen, "         ��");
#else
    buflen = sprintf(alarm, "      �ܼ��� %d ����\n\n", group);
    buflen += sprintf(alarm + buflen, "         ��");
#endif
    for(i = 0; i < g_CtrlCfg.turnMAX; i++)
    {
        buflen += sprintf(alarm + buflen, " %s", turnName[i]);
        if(i < g_CtrlCfg.turnNum)
        {
            pclen += sprintf(bufPc + pclen, " %s", 0 == ((PwTurn >> i) & 1) ? "��" : "Ͷ");
            eclen += sprintf(bufEc + eclen, " %s", 0 == ((EcTurn >> i) & 1) ? "��" : "Ͷ");
        }
        else
        {
            pclen += sprintf(bufPc + pclen, " %s", "--");
            eclen += sprintf(bufEc + eclen, " %s", "--");
        }
    }
    buflen += sprintf(alarm + buflen, "\n");

    buflen += sprintf(alarm + buflen, " �¸���  %s%s\n", TRUE == ptCtrlArg->tDown.bEffect[group-1] ? "Ͷ" : "��", bufPc);
    buflen += sprintf(alarm + buflen, " ��ͣ��  %s%s\n", TRUE == ptCtrlArg->tStop.bEffect[group-1] ? "Ͷ" : "��", bufPc);
    buflen += sprintf(alarm + buflen, " ���ݿ�  %s%s\n", TRUE == ptCtrlArg->tRest.bEffect[group-1] ? "Ͷ" : "��", bufPc);
    buflen += sprintf(alarm + buflen, " ʱ�ο�  %s%s\n", TRUE == ptCtrlArg->tPeriod.bEffect[group-1] ? "Ͷ" : "��", bufPc);
    buflen += sprintf(alarm + buflen, " �µ��  %s%s\n", TRUE == ptCtrlArg->tMonth.bEffect[group-1] ? "Ͷ" : "��", bufEc);
    buflen += sprintf(alarm + buflen, " �����  %s%s\n", TRUE == ptCtrlArg->tBuy.bEffect[group-1] ? "Ͷ" : "��", bufEc);
    
    appctrl_show_guihint(120, (uint8*)alarm, buflen);
    CTRL_BUF_TRACE(alarm, buflen, "Send hint:");
    usleep(100000);
    appctrl_ctrl_buzzer(0, FALSE);
    
    *ctrlType = effectType;
    return 1 == effectStatus ? TRUE : FALSE;
}

/**********************************************************************
* @name      : app_ctrl_entry
* @brief     ��������ں���
* @param[in] ��void *arg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-3-31
* @Update    :
**********************************************************************/
void *app_ctrl_entry(void *pArg)
{
    CTRL_THREAD_T   *pHandle = (CTRL_THREAD_T *)pArg;
    time_t           now;
    OOP_DATETIME_S_T curTime = {0};
    OOP_DATETIME_S_T lastTime = {0};
    uint8            bPwCtrlflag = FALSE;
    uint8            bEcCtrlflag = FALSE;
    static uint8     appCtrlFirst = TRUE;
    uint8            bDayPass = FALSE;
    uint8            bPwDayPass = FALSE;
    uint8            bMonthPass = FALSE;
    uint32           Oad = 0;
    uint16           infoNum = 0;
    static uint8     monthPassTime = 0;
    uint8            ctrlType = CTRL_NULL_TYPE;
    uint8            effectType = 0;
    uint8            pwctrldeal = FALSE;
    uint8            ecctrldeal = FALSE;
    uint8            bCfgChg = FALSE;

    CTRL_FMT_DEBUG("����ģ������!\n");
    g_CtrlDBClient = db_client_create(DB_YC_AGENT, DB_CLIENT_CTRL);
    if(g_CtrlDBClient <= 0)
    {
        CTRL_FMT_DEBUG("db_handle_create failed!\n");
        return 0;
    }

    /* �������ĸ������ */
    Oad = appctrl_read_oad_notify(&infoNum);
    while(0 != Oad)
    {
        Oad = appctrl_read_oad_notify(&infoNum);
    }

    /* ���Ʋ����״ζ�ȡ */
    appctrl_get_arg(g_CtrlDBClient);

    /* �ϵ籣�紦�� */
    if(TRUE == appCtrlFirst)
    {
        appctrl_protect_first();
        appCtrlFirst = FALSE;
    }

    appctrl_ctrl_led(EMLED_ALARM, FALSE);
    appctrl_ctrl_buzzer(g_CtrlDBClient, FALSE);

    appctrl_get_last_time(&lastTime);
    appctrl_down_init();

    CTRL_FMT_DEBUG("����ģ�鿪ʼ����!\n");
    while(1)
    {
        *pHandle->pRunFlag = 0;
        
        time(&now);
        DT_Time2DateTimeShort(now, &curTime);

        bCfgChg = FALSE;
        /* 0. ��ʼ�� */
        if(TRUE == ctrl_get_dataflag())
        {
            bCfgChg = TRUE;
            ctrl_set_dataflag(0, 0, FALSE);
            appctrl_db_chg_event(0, CTRL_DATA_TYPE, 0);
            appctrl_init_data();
            *pHandle->pRunFlag = 0;
        }
        if(TRUE == ctrl_get_argflag())
        {
            ctrl_set_argflag(0, 0, FALSE);
            appctrl_db_chg_event(0, CTRL_PARAM_TYPE, 0);
            appctrl_init_arg();
            *pHandle->pRunFlag = 0;
        }

        /* 1. �������ĸ��� */
        Oad = appctrl_read_oad_notify(&infoNum);
        while(0 != Oad)
        {
            bCfgChg = TRUE;
            appctrl_update_db(Oad, infoNum);
            *pHandle->pRunFlag = 0;
            Oad = appctrl_read_oad_notify(&infoNum);
        }

        if(TRUE == bCfgChg)
        {
#ifdef PRODUCT_SEU
            appctrl_assign_status(g_CtrlDBClient);  //����״̬����(Ͷ�롢�ִ�)
#endif
        }

        /* 2. ���� */
        bDayPass = FALSE;
        if(curTime.day != lastTime.day || curTime.month != lastTime.month || curTime.year != lastTime.year)
        {
            /* �������� */
            appctrl_clear_daystatic();
            bDayPass = TRUE;
            bPwDayPass = TRUE;
        }
        if(curTime.month != lastTime.month || curTime.year != lastTime.year)
        {
            CTRL_FMT_DEBUG("���´���!\n");
            appctrl_clear_monstatic();
            bMonthPass = TRUE;
            monthPassTime = 0;
        }

        /* 3. ����ʱ��ά�� */
        if(curTime.minute != lastTime.minute)
        {
            appctrl_protect_ctrl_min();
        }

        /* 4. ��Լ�������� */
        if(curTime.second != lastTime.second)
        {
            appctrl_assign_status(g_CtrlDBClient);  //����״̬����(Ͷ�롢�ִ�)

            /* ���� */
            appctrl_protect_ctrl();

            /* Ͷ�����澯 */
            pwctrldeal = FALSE;
            ecctrldeal = FALSE;
            effectType = appctrl_effect_proc(&ctrlType);
            if(FALSE == effectType && ctrlType <= CTRL_PERIOD_TYPE)
            {
                pwctrldeal = TRUE;  //���ؽ����Ҫ����ִ��
            }
            else if(FALSE == effectType && (CTRL_BUY_TYPE == ctrlType || CTRL_MONTH_TYPE == ctrlType))
            {
                ecctrldeal = TRUE;  //��ؽ����Ҫ����ִ��
            }

            /* ������ʾ��ʱ */
            appctrl_power_show_decress(CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));

            /* �����ִ�Ͷ���� */
            appctrl_pcturn_change(CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));
        }

        /* 5. ���ʿ��� ÿ����5s��ִ�е������� */
        if(TRUE == pwctrldeal || (curTime.second >= 5 && FALSE == bPwCtrlflag))
        {
            appctrl_power_ctrl(g_CtrlDBClient, bPwDayPass, CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));
            bPwCtrlflag = TRUE;
            bPwDayPass = FALSE;
            pwctrldeal = FALSE;
        }
        if(curTime.second < 5)
        {
            bPwCtrlflag = FALSE;
        }

        /* 6. ��� ���ֺ�5s���߹��º�20sִ��(����ͳ��ˢ����) */
        if(TRUE == ecctrldeal || (FALSE == bMonthPass && curTime.second >= 5 && FALSE == bEcCtrlflag) || (TRUE == bMonthPass && monthPassTime > 20))
        {
            appctrl_elec_ctrl(g_CtrlDBClient, bMonthPass, CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));
            bMonthPass = FALSE;
            bEcCtrlflag = TRUE;
            ecctrldeal = FALSE;
        }
        if(curTime.second < 5)
        {
            bEcCtrlflag = FALSE;
        }

        if(curTime.second != lastTime.second)
        {
            monthPassTime++; //���¼�ʱ

            /* 7. ң�� ң�ؼ��Ϻ�բ��Ҫ����1�� */
            appctrl_remote_ctrl(g_CtrlDBClient, bDayPass, CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX) * 2);

            /* 8. �߷Ѹ澯 */
            appctrl_callfee_ctrl();

            /* 9. �����м�״̬�㼯 ����澯����¼�����־  */
            appctrl_power_status(CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));
            appctrl_elec_status(CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));
            appctrl_remote_status(CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));
            appctrl_control_state(g_CtrlCfg.nACM1, g_CtrlCfg.nACM2, &g_curSwitch); //-��ȡ�ܿ���״̬,����д�澯��Ϣ-

            /* 10. д�м����� */
            appctrl_power_write(curTime.day != lastTime.day);
            appctrl_elec_write(curTime.day != lastTime.day);
            appctrl_remote_write(curTime.day != lastTime.day);
            appctrl_status_write(curTime.day != lastTime.day);
            appctrl_write_last_time(&curTime);

            /* 11. �ơ�����������ʾ���� */
            appctrl_system_alarm(CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));

            /* 12. ����ʵʩ */
#if defined PRODUCT_ECU || defined PRODUCT_ZCU_3 || defined PRODUCT_ECU_G
            appctrl_set_switch_close(&g_curSwitch);
#endif
            if(TRUE == g_CtrlCfg.bModeOk)
            {
#ifdef PRODUCT_ZCU_1
                appctrl_set_switch_zcu1(&g_curSwitch);
#endif
#ifdef PRODUCT_ZCU_3
                appctrl_set_switch_zcu3(g_CtrlCfg.nACM1, g_CtrlCfg.nsubACM1, &g_curSwitch);
#endif
#if defined PRODUCT_ECU || defined PRODUCT_ECU_G || defined PRODUCT_SEU
                appctrl_set_switch(g_CtrlCfg.nACM1, g_CtrlCfg.nsubACM1, g_CtrlCfg.nACM2, g_CtrlCfg.nsubACM1, &g_curSwitch);
#endif
#ifdef PRODUCT_SEU
                appctrl_init_alarm_switch(g_CtrlCfg.nACMAlarm, g_CtrlCfg.nsubAlarm);
#endif
            }

            /* 13. ���ƻ�·���߼�� */
#ifdef PRODUCT_ZCU_1
            appctrl_get_switch_input_zcu1(g_CtrlCfg.nACM1, g_CtrlCfg.nACM2, &g_curSwitch);
#else
            if(FALSE == g_AdvPowerOff)
            {
                appctrl_get_switch_input(g_CtrlCfg.nACM1, g_CtrlCfg.nsubACM1, g_CtrlCfg.nACM2, g_CtrlCfg.nsubACM1, &g_curSwitch);
            }
#endif
        } 

        lastTime = curTime;
        usleep(10000);
    }
}


