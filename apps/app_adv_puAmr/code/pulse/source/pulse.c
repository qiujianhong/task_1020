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
#include "eeprom.h"
#include "status_signal.h"
#include "macro.h"
#include "task_msg.h"
#include "db_api.h"
#include "task_id.h"
#include "msg_id.h"
#include "iniparser.h"

#include "calculate.h"
#include "pulse_adv.h"
#include "framecomm.h"
#include "storage.h"

DB_CLIENT   g_pulsedb = 0;
uint8       g_LastRate = 0;

extern void CheckERC_3104(DB_CLIENT fd, OOP_SWITCHIN_STATE_T switchTemp);
extern void CheckERC_3104_seu(DB_CLIENT fd, OOP_DATETIME_S_T evtStart, OOP_SWITCHIN_STATE_T switchTemp);

/**********************************************************************
* @name      : pulse_write_queue
* @brief     ��д�����
* @param[in] ��PULSE_CHG_LIST_T *pList          �ϱ�����
               OOP_SWITCHIN_STATE_T *pNode     д������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2023-1-3
* @Update    :
**********************************************************************/
void pulse_write_queue(PULSE_CHG_LIST_T *pList, OOP_SWITCHIN_STATE_T *pNode)
{
    struct timespec  curtime = {0};
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    memcpy(&pList->list[pList->tail].sc, pNode, sizeof(OOP_SWITCHIN_STATE_T));
    pList->list[pList->tail].happentmes = time(NULL);
    pList->list[pList->tail].systemtime = curtime.tv_sec;
    pList->tail = (pList->tail + 1) % PULSE_CHG_MAX;
}

/**********************************************************************
* @name      : pulse_read_queue
* @brief     ����������
* @param[in] ��PULSE_CHG_LIST_T *pList         �ϱ�����
* @param[out]��OOP_SWITCHIN_STATE_T *pNode     �ϱ�����
* @return    ��0-�ɹ�/ʧ��
* @Create    : ����
* @Date      ��2023-1-3
* @Update    :
**********************************************************************/
int pulse_read_queue(PULSE_CHG_LIST_T *pList, PULSE_CHG_NODE_T *pNode, uint32 curtimes)
{
    if(pList->head == pList->tail)
    {
        return -1;
    }

    if(pList->list[pList->head].systemtime + 4 <= curtimes)
    {
        memcpy(pNode, &pList->list[pList->head], sizeof(PULSE_CHG_NODE_T));
        memset(&pList->list[pList->head], 0, sizeof(PULSE_CHG_NODE_T));
        pList->head = (pList->head + 1) % PULSE_CHG_MAX;

        return 0;
    }
    return -1;
}

/**********************************************************************
* @name      : pulse_dbCenter_init
* @brief     ����ʼ���������Ŀͻ���, ����ȡ������������
* @param[in] ��
* @param[out]��PULSE_OAD_T *ptCfg    ��������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2019-11-18
* @Update    : 2020-1-13
**********************************************************************/
int pulse_dbCenter_init(PULSE_OAD_T *ptCfg)
{
    int             ret = 0;
    NOMAL_OAD_T     tOAD;
    uint32          readLen = 0;
    uint8           i;
    PULSE_Measure_T *ptPM = NULL;
    int             sockfd = 0;

    ptCfg->pHalDev = (STATUS_SIGNAL_DEVICE_T *)hal_device_get(STATUS_SIGNAL_DEVICE_ID_STR);
    if(NULL == ptCfg->pHalDev)
    {
        PULSE_FMT_DEBUG("hal_device_get STATUS_SIGNAL_DEVICE_ID_STR failed\n");
        return -1;
    }
    
    ptCfg->dbclient = db_client_create(DB_YC_AGENT, DB_CLIENT_PULSE);
    if(ptCfg->dbclient <= 0)
    {
        PULSE_FMT_DEBUG("db_handle_create failed\n");
        return -2;
    }

    sockfd = ptCfg->dbclient;
    g_pulsedb = sockfd;

    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    
    /* 1 ��ȡң������ */
    tOAD.oad.value = 0xf2030300; //�ź����豸��������
    readLen = 0;
    ret = db_read_nomal(sockfd, &tOAD, sizeof(uint8), (uint8 *)&(ptCfg->nSwitchNUM), &readLen);
    if(0 != ret || readLen != sizeof(uint8))
    {
        PULSE_FMT_DEBUG("Get 0xf2030300 failed %d, write default value\n", ret);
#if defined PRODUCT_CCU
        ptCfg->nSwitchNUM = 2;
#elif defined PRODUCT_ZCU_1
    #ifdef AREA_JIANGSU
        ptCfg->nSwitchNUM = 16;
    #else
        ptCfg->nSwitchNUM = 8;
    #endif
#else
        ptCfg->nSwitchNUM = 4;
#endif
        pulse_write_INITOAD(0xf2030300, CLASS_DATA_INIT, &(ptCfg->nSwitchNUM), sizeof(uint8));
    }
    PULSE_FMT_DEBUG("Get switch num is %d\n", ptCfg->nSwitchNUM);
    
    tOAD.oad.value = 0xf2030200; //�ź����б����
    readLen = 0;
    ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_SWITCHIN_STATE_T), (uint8 *)&(ptCfg->tSwitchState), &readLen);
    if(0 != ret || readLen != sizeof(OOP_SWITCHIN_STATE_T))
    {
        PULSE_FMT_DEBUG("Get 0xf2030200 failed, ret = %d, len(%d) = %d\n", ret, sizeof(OOP_SWITCHIN_STATE_T), readLen);
        memset(&(ptCfg->tSwitchState), 0, sizeof(OOP_SWITCHIN_STATE_T));
        ptCfg->bInitSwitch = FALSE;
    }
    else
    {
        ptCfg->bInitSwitch = TRUE;
        PULSE_FMT_DEBUG("Init state(num %d): %d %d %d %d %d %d %d %d\n", ptCfg->tSwitchState.nNum,
                        ptCfg->tSwitchState.state[0].st, ptCfg->tSwitchState.state[1].st, 
                        ptCfg->tSwitchState.state[2].st, ptCfg->tSwitchState.state[3].st,
                        ptCfg->tSwitchState.state[4].st, ptCfg->tSwitchState.state[5].st, 
                        ptCfg->tSwitchState.state[6].st, ptCfg->tSwitchState.state[7].st);
    }
    
    tOAD.oad.value = 0xf2030400; //�ź�����������
    readLen = 0;
    ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_SWITCHIN_FLAG_T), (uint8 *)&(ptCfg->tSwitchFlag), &readLen);
    if(0 != ret || readLen != sizeof(OOP_SWITCHIN_FLAG_T))
    {
        memset(&(ptCfg->tSwitchFlag), 0, sizeof(OOP_SWITCHIN_FLAG_T));
        pulse_write_INITOAD(0xf2030400, CLASS_DATA_INIT, &(ptCfg->tSwitchFlag), sizeof(OOP_SWITCHIN_FLAG_T));
    }
    else
    {
        PULSE_FMT_DEBUG("Get 0xf2030400 , input %04X, attr %04X\n", ptCfg->tSwitchFlag.inputFlag.bitValue, 
                                                                    ptCfg->tSwitchFlag.attrFlag.bitValue);
    }

    tOAD.oad.value = 0xf2030600; //�ź�
    readLen = 0;
    ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_STATUS_SIGNAL_T), (uint8 *)&(ptCfg->tStatusSignal), &readLen);
    if(0 != ret || readLen != sizeof(OOP_SWITCHIN_FLAG_T))
    {
        PULSE_FMT_DEBUG("Get 0xf2030600 failed, write default value\n");
        memset(&ptCfg->tStatusSignal, 0, sizeof(OOP_STATUS_SIGNAL_T));
#ifdef PRODUCT_CCU
        ptCfg->tStatusSignal.nNum = 1;
        ptCfg->tStatusSignal.ST.nValue = 0x80;
#else
        ptCfg->tStatusSignal.nNum = 3;
        ptCfg->tStatusSignal.ST.nValue = 0xE0;
#endif
        pulse_write_INITOAD(0xf2030600, CLASS_DATA_NORMAL, &(ptCfg->tStatusSignal), sizeof(OOP_STATUS_SIGNAL_T));
    }
    else
    {
        PULSE_FMT_DEBUG("Get 0xf2030600 , num %d, ST %02X, attr %02X\n", ptCfg->tStatusSignal.nNum, ptCfg->tStatusSignal.ST, 
                                                                    ptCfg->tStatusSignal.CT);
    }
    
    /* f20a �Ȳ���ȡ */
#if 0
    /* 2 ��ȡ���������豸 */
    tOAD.oad.value = 0xf20a0300; //���������豸��������
    readLen = 0;
    retDb = nomal_data_read(g_pPulseDb, tOAD, sizeof(uint8), (uint8 *)&(ptCfg->nPulseNum), &readLen);
    if(0 == retDb && readLen == sizeof(uint8) && 0 != ptCfg->nPulseNum)
    {
        tOAD.oad.value = 0xf20a0200; //��������˿�����
        readLen = 0;
        retDb = nomal_data_read(g_pPulseDb, tOAD, sizeof(OOP_PULSE_T), (uint8 *)&(ptCfg->tPulseInfo), 
                                &readLen);
        if(0 != retDb || readLen != sizeof(OOP_PULSE_T))
        {
            DPRINT("Get 0xf20a0200 failed\n");
            memset(&(ptCfg->tPulseInfo), 0, sizeof(OOP_PULSE_T));
            ptCfg->nPulseNum = 0;
        }
    }
    else
    {
        DPRINT("Get 0xf20a0300 failed %d, or num is 0(%d-%d)\n", retDb, readLen, ptCfg->nPulseNum);
        memset(&(ptCfg->tPulseInfo), 0, sizeof(OOP_PULSE_T));
        ptCfg->nPulseNum = 0;
    }
#endif

    /* 3 ������� */
    for(i = 1; i <= OOP_MAX_PULSE; i++)
    {
        ptPM = &(ptCfg->tPulse[i-1]);
        
        tOAD.oad.value = 0x24000300 + i * 0x10000; //����������
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_PTCT_T), (uint8 *)&(ptPM->tPTCT), &readLen);
        if(0 != ret || readLen != sizeof(OOP_PTCT_T))
        {
            PULSE_FMT_DEBUG("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
        }

        tOAD.oad.value = 0x24000400 + i * 0x10000; //��������
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_PULSESETS_T), (uint8 *)&(ptPM->tPulseCfg), &readLen);
        if(0 != ret || readLen != sizeof(OOP_PULSESETS_T))
        {
            PULSE_FMT_DEBUG("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(ptPM, 0, sizeof(PULSE_Measure_T));
            continue;
        }

        tOAD.oad.value = 0x24001C00 + i * 0x10000; //��������
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_PULSESETS_B_T), (uint8 *)&(ptPM->tBigCfg), &readLen);
        if(0 != ret || readLen != sizeof(OOP_PULSESETS_B_T))
        {
            PULSE_FMT_DEBUG("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
        }
        /* ����Ҫ���ظ� */
#if 0
        /* ���������ü�� �Ƿ��ң�ų�ͻ */
        for(j = 0; j < ptPM->tPulseCfg.nNum; j++)
        {
            port = ptPM->tPulseCfg.pulse[j].port.value & 0xff;
            if(1 <= port && port <= OOP_MAX_PULSE && ptCfg->nSwitchNUM > 0)
            {
                if((ptCfg->tSwitchFlag.inputFlag >> (port-1)) & 0x01)
                {
                    DPRINT("0x%x port %d has switch and pulse\n", tOAD.oad.value, port);
                    return -1;
                }
            }
        }
#endif
        tOAD.oad.value = 0x24000500 + i * 0x10000; //�й�����
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(int32), (uint8 *)&(ptPM->nActivePower), &readLen);
        if(readLen != sizeof(int32))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nActivePower = 0;
        }

        tOAD.oad.value = 0x24000600 + i * 0x10000; //�޹�����
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(int32), (uint8 *)&(ptPM->nReactivePower), &readLen);
        if(0 != ret || readLen != sizeof(int32))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nReactivePower = 0;
        }
        
        tOAD.oad.value = 0x24000f00 + i * 0x10000; //�����й�����ʾֵ
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tShowForwActEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nReactivePower = 0;
        }

        tOAD.oad.value = 0x24001000 + i * 0x10000; //�����޹�����ʾֵ
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tShowForwRactEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nReactivePower = 0;
        }
        
        tOAD.oad.value = 0x24001100 + i * 0x10000; //�����й�����ʾֵ
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tShowBackActEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nReactivePower = 0;
        }

        tOAD.oad.value = 0x24001200 + i * 0x10000; //�����޹�����ʾֵ
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tShowBackRactEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nReactivePower = 0;
        }
    }

    /* ��ǰ����ʱ�α� */
    tOAD.oad.value = 0x40160200; //��ǰ����ʱ�α�
    readLen = 0;
    ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_DAYPERIOD_T), (uint8 *)&(ptCfg->tDayPeriod), &readLen);
    if(0 != ret  || readLen != sizeof(OOP_DAYPERIOD_T))
    {
        PULSE_FMT_DEBUG("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
        //return -1;
    }

    /* ң�ŷ���ʱ�� */
    tOAD.oad.value = 0x49080200;
    readLen = 0;
    ret = db_read_nomal(sockfd, &tOAD, sizeof(uint32), (uint8 *)&(ptCfg->rmtTime), &readLen);
    if(0 != ret  || readLen != sizeof(uint32))
    {
        PULSE_FMT_DEBUG("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
        ptCfg->rmtTime = 0;
    }

    return 0;
}

/**********************************************************************
* @name      : pulse_get_curtime_s
* @brief     ����ȡϵͳʱ��, ��ȷ��s ��ǰ5s
* @param[in] ��
* @return    ��TIME_STRUCT_T *pTime
* @Create    : ����
* @Date      ��2019-11-19
* @Update    :
**********************************************************************/
void pulse_get_curtime_s(TIME_STRUCT_T *pTime)
{
    struct tm systime;
    struct  timeval tv;
    struct  timezone tz;
    gettimeofday(&tv, &tz);

    FastSecondToDate(&tv, &tz, &systime);

    pTime->milliseconds = 0;
    pTime->second = (uint8)systime.tm_sec;
    pTime->minute = (uint8)systime.tm_min;
    pTime->hour = (uint8)systime.tm_hour;
    pTime->day = (uint8)systime.tm_mday;
    pTime->month = (uint8)systime.tm_mon;         /*�·�0����1��*/
    pTime->year = (uint8)systime.tm_year;         /*���0����1900��*/
}

/**********************************************************************
* @name      : pulse_calculate_init
* @brief     ��eeprom��ʼ������ȡ������Ϣ, ʱ�䲻���ھ�ȡ��ǰʱ��
* @param[in] ��PULSE_CALCULATE_T *ptCfg
* @param[out]��TIME_STRUCT_T *pTime
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2019-11-19
* @Update    :
**********************************************************************/
int pulse_calculate_init(PULSE_CALCULATE_T *ptCalu, TIME_STRUCT_T *pTime)
{
    char check[4] = {0xea, 0xeb, 0xec, 0x03}; /* ���һλ����Ϣ�汾��, ÿ�޸�һ������1 */
    char buf[4] = {0};
    PULSE_TEMP_T  tPulseTemp[OOP_MAX_PULSE];
    uint8 i = 0;
    uint8 pulseNum = 0;

    pulse_get_curtime_s(pTime);
    memset(&(tPulseTemp[0]), 0, OOP_MAX_PULSE * sizeof(PULSE_TEMP_T));

    if(0 == access(ptCalu->datafile, F_OK))
    {
        if(0 == read_pdata_xram(ptCalu->datafile, buf, 0, 4))
        {
            PULSE_FMT_DEBUG("File version %02x %02x %02x %02x, check %02x %02x %02x %02x\n", 
                             buf[0], buf[1], buf[2], buf[3], check[0], check[1], check[2], check[3]);
            if(0 == memcmp(buf, check, 4))
            {
                if(0 == read_pdata_xram(ptCalu->datafile, (char *)&(tPulseTemp[0]), 4, OOP_MAX_PULSE * sizeof(PULSE_TEMP_T)))
                {
                    memcpy(&(ptCalu->tPulseTemp[0]), &(tPulseTemp[0]), OOP_MAX_PULSE * sizeof(PULSE_TEMP_T));
                    char  strtemp[1024] = {0};
                    uint16 offset = 0;
                    /* �豸��������ѹ�� */
                    for(i = 0; i < OOP_MAX_PULSE; i++)
                    {
                        ptCalu->Pulse[pulseNum].val += ptCalu->tPulseTemp[i].count;
                        ptCalu->Pulse[pulseNum].interval += ptCalu->tPulseTemp[i].timems;
                        pulseNum++;
                        offset += sprintf(strtemp + offset, "pulse %d: total, %u-%u.%03u(s), single %u-%u(ms)\n", i + 1, 
                                          tPulseTemp[i].totalcou, tPulseTemp[i].totals, tPulseTemp[i].totalms, 
                                          tPulseTemp[i].count, tPulseTemp[i].timems);
                    }
                    PULSE_FMT_DEBUG("Init get pulse:\n%s\n", strtemp);
                    return 0;
                }
            }
        }
        
    }
    PULSE_FMT_DEBUG("Create new pulse file %s!\n", ptCalu->datafile);

    write_pdata_xram(ptCalu->datafile, check, 0, 4);
    write_pdata_xram(ptCalu->datafile, (char *)&(tPulseTemp[0]), 4, OOP_MAX_PULSE * sizeof(PULSE_TEMP_T));
    
    return 0;
}


/**********************************************************************
* @name      : app_set_configure
* @brief     ������ң�����幦������
* @param[in] ��int taskid               ͨ�ž��
               PULSE_CFG_T *pPluseCfg  ����
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-1
* @Update    : 2022-6-18
**********************************************************************/
int app_set_configure(int taskid, PULSE_CFG_T *pPluseCfg)
{
    uint8  i = 0;
    uint8  j = 0;
    uint8  sendbuf[3];
    int    ret = 0;
    uint8  recvBuf[100] = {0};
    uint8  retbool = 0;
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;
    uint8 status = 0;

    for(i = 0; i < PULSE_MODEL_MAX; i++)
    {
        if(0 == pPluseCfg->cfg[i].bSetCfg || 0 == pPluseCfg->cfg[i].portNum)
        {
            continue;
        }
        sendbuf[0] = pPluseCfg->cfg[i].no;
        sendbuf[1] = MIN(pPluseCfg->cfg[i].portNum, 8);
        /* �������� */
        status = 0;
        for(j = 0; j < pPluseCfg->cfg[i].portNum && j < 8; j++)
        {
            if(1 == pPluseCfg->cfg[i].config[j])
            {
                status |= (0x01 << (7 - j));
            }
        }
        sendbuf[2] = status;
        ret = task_syncmsg_safe(taskid, MSG_PULSE_SET_RSP, sendbuf, 3, MSG_MONITOR, recvBuf, 100, 3);
        if(0 == ret && 1 == pMsg->msglen)
        {
            retbool = *pMsg->msg;
        }
        PULSE_FMT_DEBUG("�����豸%d�˿�����%s\n", pPluseCfg->cfg[i].no, 0 == retbool ? "ʧ��" : "�ɹ�");
    }

    task_msg_settime(taskid, 0, 100000);  //����������ʱʱ��Ϊ0.1s
    return 0;
}

/**********************************************************************
* @name      : app_set_shaketime
* @brief     ������ң���������ʱ��
* @param[in] ��int taskid       �߳̾��
               uint32 timelong  ����ʱ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-8-3
* @Update    :
**********************************************************************/
void app_set_shaketime(int taskid, uint32 timelong)
{
    uint8  buf[4] = {0};
    int    ret = 0;
    uint16 timems = 0;
 
    if(0 == timelong)
    {
        return;
    }

    timems = (uint16)timelong * 10;
    appmsg_memcpy_r(buf + 1, &timems, 2);

    buf[0] = 1;
    ret = task_send_labelmsg(taskid, MSG_PULSE_SHAKE_TIME, 1, buf, 3, MSG_MONITOR);
    PULSE_FMT_DEBUG("Set port 1 shaketime %dms, ret %s\n", timems, 0 == ret ? "success" : "failed");

    buf[0] = 2;
    ret = task_send_labelmsg(taskid, MSG_PULSE_SHAKE_TIME, 2, buf, 3, MSG_MONITOR);
    PULSE_FMT_DEBUG("Set port 2 shaketime %dms, ret %s\n", timems, 0 == ret ? "success" : "failed");

    buf[0] = 3;
    ret = task_send_labelmsg(taskid, MSG_PULSE_SHAKE_TIME, 2, buf, 3, MSG_MONITOR);
    PULSE_FMT_DEBUG("Set port 2 shaketime %dms, ret %s\n", timems, 0 == ret ? "success" : "failed");
}

/**********************************************************************
* @name      : app_get_rmt
* @brief     ��������ȥң��״̬
* @param[in] ��int taskid       �߳̾��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-1
* @Update    :
**********************************************************************/
void app_get_rmt(int taskid, PULSE_CFG_T *pPluseCfg)
{
    uint8 pulseNum = 0xFF;
    int   ret = 0;

    ret = task_send_labelmsg(taskid, MSG_PULSE_GET_RMT, pulseNum, &pulseNum, 1, MSG_MONITOR);
    PULSE_FMT_DEBUG("��ȡ����ң��״̬ %s\n", 0 == ret ? "success" : "failed");
}

/**********************************************************************
* @name      : app_proc_rmt
* @brief     ��ң�Ŵ���
* @param[in] ��uint8 id                  �豸�� Ϊ0����д��һ�����ݿ�
               uint8 *pMsg               ��Ϣ����
               uint16 msglen             ��Ϣ����
               PULSE_OAD_T *ptCfg        ����
               PULSE_CALCULATE_T *ptCalu ����
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-1
* @Update    :
**********************************************************************/
void app_proc_rmt(uint8 id, uint8 *pMsg, uint16 msglen, uint8 bReport, PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu)
{
    uint8 i;
    uint8 start;
    int   ret = 0;
    uint8 ModelST[OOP_MAX_SWITCHIN] = {0};
    uint8 oldST[OOP_MAX_SWITCHIN] = {0};
    uint8 STNum = 0;
    uint8 bEvent3104 = FALSE; //�Ƿ����ң�ű�λ�¼�
    RST_RMT_T tRmt;

    char showtempOld[256] = {0};
    uint16 offOld = 0;
    char showtempDev[256] = {0};
    uint16 offDev = 0;
    char showtempNew[256] = {0};
    uint16 offNew = 0;
    char showtempReal[256] = {0};
    uint16 offReal = 0;
    char showtempST[256] = {0};
    uint16 offST = 0;
    char showtempCT[256] = {0};
    uint16 offCT = 0;
    
    if(0 != id && 1 != id && 2 != id && 0xFF != id)
    {
        return;
    }

    STNum = appmsg_rsp_currmt_split(bReport, pMsg, ModelST, &tRmt, OOP_MAX_SWITCHIN);
    for(i = 0; i < STNum; i++)
    {
        offDev += sprintf(showtempDev + offDev, " %d", ModelST[i]);
    }

    memcpy(oldST, ptCalu->curST, OOP_MAX_SWITCHIN);
    for(i = 0; i < OOP_MAX_SWITCHIN ; i++)
    {
        offOld += sprintf(showtempOld + offOld, " %d", oldST[i]);
    }

    if(0 == id)
    {
        for(i = 0; i < STNum && i < ptCalu->pCfg->mapSTStart; i++)
        {
            ptCalu->curST[i] = ModelST[i];
            ptCalu->pCfg->cfg[id].realcfg[i] = tRmt.flag[i];
        }
    }
    else if(1 == id || 2 == id)
    {
        start = ptCalu->pCfg->mapSTStart;
        if(2 == id)
        {
            start += ptCalu->pCfg->cfg[1].portNum;
        }
        
        for(i = 0; i < STNum && i + start < OOP_MAX_SWITCHIN; i++)
        {
            ptCalu->curST[i + start] = ModelST[i];
            ptCalu->pCfg->cfg[id].realcfg[i] = 1 - tRmt.flag[i];
        }
    }
    else
    {
        for(i = 0; i < STNum; i++)
        {
            ptCalu->curST[i] = ModelST[i];
            if(i < ptCalu->pCfg->mapSTStart)
            {
                ptCalu->pCfg->cfg[0].realcfg[i] = 1 - tRmt.flag[i];
            }
            else if(i < ptCalu->pCfg->mapSTStart + ptCalu->pCfg->cfg[1].portNum)
            {
                ptCalu->pCfg->cfg[1].realcfg[i - ptCalu->pCfg->mapSTStart] = 1 - tRmt.flag[i];
            }
            else if(i < ptCalu->pCfg->mapSTStart + ptCalu->pCfg->cfg[1].portNum + ptCalu->pCfg->cfg[2].portNum)
            {
                ptCalu->pCfg->cfg[2].realcfg[i - ptCalu->pCfg->mapSTStart - ptCalu->pCfg->cfg[1].portNum] = 1 - tRmt.flag[i];
            }
        }
    }

    for(i = 0; i < OOP_MAX_SWITCHIN ; i++)
    {
        offNew += sprintf(showtempNew + offNew, " %d", ptCalu->curST[i]);
    }

    if(0 != memcmp(ptCalu->curST, oldST, OOP_MAX_SWITCHIN) && msglen > 0)
    {
        bEvent3104 = TRUE;
    }

    PULSE_FMT_DEBUG("�豸%d: ң������%d, ״̬(%s); ����ǰ״̬(%s), ���ܺ�״̬(%s), %s\n", id, 
                    STNum, 0 == showtempDev[0] ? "NULL" : showtempDev, showtempOld, showtempNew,
                    TRUE == bEvent3104 ? "��λ" : "����");

    /* ״̬��˽�б��� */
    pulse_write_OAD(0xf20302FF, ptCalu->curST, OOP_MAX_SWITCHIN);

    memset(ptCalu->realST, 0, OOP_MAX_SWITCHIN);
    /* ģ�鷵��0-��,698��Լ1-��,��Ҫת�� ����������� */
    for(i = 0; i < OOP_MAX_SWITCHIN; i++)
    {
        if(ptCfg->tSwitchFlag.inputFlag.bitValue & (0x01 << i))
        {
            if(ptCfg->tSwitchFlag.attrFlag.bitValue & (0x01 << i))
            {
                /* ������Ҫң��״̬ȡ�� */
                ptCalu->realST[i] = 1 - ptCalu->curST[i];
            }
            else
            {
                /* ����ȡԭֵ */
                ptCalu->realST[i] = ptCalu->curST[i];
            }
            offReal += sprintf(showtempReal + offReal, " %s", 0 == ptCalu->realST[i] ? "��" : "��");
        }
        else
        {
            offReal += sprintf(showtempReal + offReal, " --");
        }
    }

    /*  */

    /* �Ȼ�ȡ���������µı�λ״̬ */
    OOP_SWITCHIN_STATE_T tSwitchState;
    OOP_SWITCHIN_STATE_T tOADState;

    memset(&tOADState, 0, sizeof(OOP_SWITCHIN_STATE_T));
    ret = pulse_get_OAD(0xf2030200, (uint8 *)&tOADState, sizeof(OOP_SWITCHIN_STATE_T));
    if(0 != ret)
    {
        PULSE_FMT_DEBUG("Get switch state 0xf2030200 failed, code %d\n", ret);
    }

    memcpy(&tSwitchState, &ptCfg->tSwitchState, sizeof(OOP_SWITCHIN_STATE_T));
    
    ptCfg->tSwitchState.nNum = OOP_MAX_SWITCHIN;
    for(i = 0; i < OOP_MAX_SWITCHIN; i++)
    {
        /* ������λ */
        offST += sprintf(showtempST + offST, " %s", 0 == ptCfg->tSwitchState.state[i].st ? "��" : "��");
        
        ptCfg->tSwitchState.state[i].cd = tOADState.state[i].cd;  //�����������ĵ�������
        /* �ж��Ƿ��λ */
        if(ptCfg->tSwitchState.state[i].st != ptCalu->realST[i])
        {
            ptCfg->tSwitchState.state[i].cd = 1;
        }
        ptCfg->tSwitchState.state[i].st = ptCalu->realST[i];
        offCT += sprintf(showtempCT + offCT, " %s", 1 == ptCfg->tSwitchState.state[i].cd ? "��" : "ͬ");
        
    }
    PULSE_FMT_DEBUG("ң��·��: \n          һ �� �� �� �� �� �� �� �� ʮ һ �� �� �� �� ��\n"
                    "��ʷ״̬:%s\n"
                    "ʵ��״̬:%s\n"
                    "��λ״̬:%s\n", showtempST, showtempReal, showtempCT);
                    
    /* ״̬û�б仯, ����Ҫд���ݿ� */
    if(0 == memcmp(&tSwitchState, &ptCfg->tSwitchState, sizeof(OOP_SWITCHIN_STATE_T)))
    {
        return;
    }

    /* û�н����豸��������λ�¼� */
    if(TRUE == bEvent3104 && 0 != ptCfg->tSwitchFlag.inputFlag.nValue)
    {
        PULSE_FMT_DEBUG("Dev %d, ����ң�ű�λ�¼�\n", id);
#if defined PRODUCT_CCU || defined PRODUCT_ZCU_3
        /* ��������8·��Ϊ�Žڵ� */
        memcpy(&tSwitchState, &ptCfg->tSwitchState, sizeof(OOP_SWITCHIN_STATE_T));
        tSwitchState.state[7].st = 0;
        tSwitchState.state[7].cd = 0;
        CheckERC_3104(g_pulsedb, tSwitchState);
#else
        CheckERC_3104(g_pulsedb, ptCfg->tSwitchState);
#endif
    }

    ret = pulse_write_OAD(0xf2030200, (uint8 *)&(ptCfg->tSwitchState), sizeof(OOP_SWITCHIN_STATE_T));
    if(0 != ret)
    {
        PULSE_FMT_DEBUG("Write switch state 0xf2030200 failed, code %d\n", ret);
    }
#ifdef AREA_HUNAN
    //������ң��1���ܿ��� ��ؿ��طֺ�״̬ enum{�֣�1�����ϣ�2��}
    uint8 state =ptCfg->tSwitchState.state[0].st+1;  //״̬ST����0�����֡�״̬��1�����ϡ�״̬
    PULSE_FMT_DEBUG("Write 0x28000200, state %d\n", state);
    ret = pulse_write_OAD(0x28000200, &state, sizeof(uint8));
    if(0 != ret)
    {
        PULSE_FMT_DEBUG("Write 0x28000200 failed, code %d\n", ret);
    }
#endif  
}

/**********************************************************************
* @name      : app_proc_rmt
* @brief     ��ң�Ŵ���
* @param[in] ��PULSE_OAD_T *ptCfg        ����
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-1
* @Update    :
**********************************************************************/
void app_proc_rmt_later(PULSE_OAD_T *ptCfg)
{
    PULSE_CHG_NODE_T tNode;
    struct timespec  curtime = {0};
    OOP_DATETIME_S_T dateTime = {0};
    clock_gettime(CLOCK_MONOTONIC, &curtime);

    while(0 == pulse_read_queue(&ptCfg->tList, &tNode, curtime.tv_sec))
    {
        if(TRUE == ptCfg->bPowerClose)
        {
            PULSE_FMT_DEBUG("ͣ��, �����¼�\n");
        }
        else
        {
            PULSE_FMT_DEBUG("��ʱ����\n");
            DT_Time2DateTimeShort(tNode.happentmes, &dateTime);
            CheckERC_3104_seu(g_pulsedb, dateTime, tNode.sc);
        }
    }
}

/**********************************************************************
* @name      : app_reproc_rmt
* @brief     ��ң�ű�λԤ����
* @param[in] ��PULSE_OAD_T *ptCfg        ����
               PULSE_CALCULATE_T *ptCalu ����
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-16
* @Update    :
**********************************************************************/
void app_reproc_rmt(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu, uint8 bExit)
{
    uint8 i;
    int   ret = 0;

    ret = pulse_get_OAD(0xf20302FF, ptCalu->curST, OOP_MAX_SWITCHIN);

    if(FALSE == bExit)
    {
        memset(&(ptCfg->tSwitchState), 0, sizeof(OOP_SWITCHIN_STATE_T));
        ptCfg->tSwitchState.nNum = OOP_MAX_SWITCHIN;
        for(i = 0; i < OOP_MAX_SWITCHIN; i++)
        {
            if(0 == ((ptCfg->tSwitchFlag.attrFlag.bitValue >> i) & 0x01))
            {
                ptCfg->tSwitchState.state[i].st = 1;
            }
        }

        PULSE_FMT_DEBUG("Set default switch: %d %d %d %d %d %d %d %d\n", 
                                ptCfg->tSwitchState.state[0].st, ptCfg->tSwitchState.state[1].st, 
                                ptCfg->tSwitchState.state[2].st, ptCfg->tSwitchState.state[3].st,
                                ptCfg->tSwitchState.state[4].st, ptCfg->tSwitchState.state[5].st, 
                                ptCfg->tSwitchState.state[6].st, ptCfg->tSwitchState.state[7].st);

        /* ��ʼ��״̬ */
        ret = pulse_write_OAD(0xf2030200, (uint8 *)&(ptCfg->tSwitchState), sizeof(OOP_SWITCHIN_STATE_T));
        if(0 != ret)
        {
            PULSE_FMT_DEBUG("Write switch state 0xf2030200 failed, code %d\n", ret);
        }
    }
}

/**********************************************************************
* @name      : app_get_pulse
* @brief     ������ɼ�
* @param[in] ��int taskid                �߳̾��
* @param[out]��PULSE_CALCULATE_T *ptCalu ͳ�ƽ��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-1
* @Update    :
**********************************************************************/
void app_get_pulse(int taskid, PULSE_CALCULATE_T *ptCalu)
{
    uint8 i;
    uint8 j;
    uint8 no = 0xFF;
    uint8 index = 0; //��ǰ���
    int   ret = 0;
    uint8 pulseNum = 0;
    uint8  recvBuf[200] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;
    RST_PULSE_STATIC_T tPulse;
    char  strtemp[600] = {0};
    uint16 len = 0;
    uint64 lastms = 0;
    uint64 endms = 0;

    PULSE_FMT_DEBUG("��ȡ��������\n");

    ret = task_syncmsg_safe(taskid, MSG_PULSE_GET_PULSE, &no, sizeof(uint8), MSG_MONITOR, recvBuf, 200, 3);
    if(0 == ret)
    {
        memset(&tPulse, 0, sizeof(RST_PULSE_STATIC_T));
        PULSE_BUF_DEBUG(pMsg->msg, pMsg->msglen, "��ȡ��Ϣ");
        appmsg_rsppulse_split(pMsg->msg, &tPulse);
        for(j = 0; j < tPulse.num; j++)
        {
            if(0 == tPulse.item[j].flag)
            {
                continue;
            }
            /* �ж��Ƿ����� Я������ʱ��Ľӿ� */
            lastms = ptCalu->tPulseTemp[index].totals * 1000L + ptCalu->tPulseTemp[index].totalms;
            endms = tPulse.item[j].timesec * 1000L + tPulse.item[j].timems;

            if(lastms > endms ||
               0 != memcmp(&(ptCalu->tPulseTemp[index].start), &(tPulse.item[j].start), sizeof(Date_Time)))
            {
                ptCalu->tPulseTemp[index].count += tPulse.item[j].count;
                ptCalu->tPulseTemp[index].timems += (uint32)endms;

                ptCalu->tPulseTemp[index].start = tPulse.item[j].start;
            }
            else
            {
                if(tPulse.item[j].count >= ptCalu->tPulseTemp[index].totalcou)
                {
                    ptCalu->tPulseTemp[index].count += tPulse.item[j].count - ptCalu->tPulseTemp[index].totalcou;
                }
                else
                {
                    /* ��ת */
                    ptCalu->tPulseTemp[index].count += (0xFFFFFFFF - ptCalu->tPulseTemp[index].totalcou) + tPulse.item[j].count;
                }
                
                ptCalu->tPulseTemp[index].timems += (uint32)(endms - lastms);
            }
            
            ptCalu->tPulseTemp[index].totalcou = tPulse.item[j].count;
            ptCalu->tPulseTemp[index].totals = tPulse.item[j].timesec;
            ptCalu->tPulseTemp[index].totalms = tPulse.item[j].timems;

            len += sprintf(strtemp + len, "�˿�%d: ����%u, ʱ�� %u.%03us, ����%u, ʱ��%ums\n",
                           j + 1, tPulse.item[j].count, tPulse.item[j].timesec, tPulse.item[j].timems, 
                           ptCalu->tPulseTemp[index].count, ptCalu->tPulseTemp[index].timems);
            index++;
        }
        PULSE_FMT_DEBUG("����\n%s\n", strtemp);
    }

    /* �豸��������ѹ�� */
    for(i = 0; i < OOP_MAX_PULSE && i < index; i++)
    {
        ptCalu->Pulse[pulseNum].val = ptCalu->tPulseTemp[i].count;
        ptCalu->Pulse[pulseNum].interval = ptCalu->tPulseTemp[i].timems;
        pulseNum++;
    }
    
    ptCalu->pulseNum = pulseNum;
    if(ptCalu->pulseNum < OOP_MAX_PULSE)
    {
        memset(&(ptCalu->Pulse[pulseNum]), 0, (OOP_MAX_PULSE - pulseNum) * sizeof(PULSE_COUNT_T));
    }
}

/**********************************************************************
* @name      : pulse_change_K
* @brief     ������������仯, �����ȡ�µ�Kֵ
* @param[in] ��int taskid   ������
               uint8 dev    �豸���   0��ʾ1�Ŷ���, 1��ʾ2�Ŷ���
               PULSE_OAD_T *ptCfg
               PULSE_CALCULATE_T *ptCalu
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-11-22
* @Update    :
**********************************************************************/
void pulse_change_K(int taskid, uint32 OAD, PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu)
{
    uint8  j;
    uint8  pulseLocate = 0;
    uint8  powerid = 0;
    uint32 metK = 5000; //����� Ĭ����5000
    uint32 newK = 0;    //�����
    PULSE_Measure_T *ptMeasure = NULL;
    uint32 power;
    //uint32 powerDM;
    uint32 en;
    TIME_STRUCT_T tCurrTime = {0};
    uint8 i = 0, k = 0;
    int    showEner = 0;
    uint32 showEnerTotal = 0;
    uint8 dev = 0;
    OOP_PULSESETS_T      tnewPulseCfg;     //��������
    OOP_PULSESETS_B_T    tnewBigCfg;
    int retS = 1;
    int retB = 1;
    char  *posname[NEG_RE_Tag + 1] = {"�����й�", "�����޹�", "�����й�", "�����޹�"};
    
    j = (uint8)((OAD & 0x000f0000) >> 16);
    dev = j - 1;

    if(dev >= OOP_MAX_PULSE)
    {
        return;
    }
    
    if(0 == ptCalu->pulseNum || ptCalu->pulseNum > OOP_MAX_PULSE)
    {
        goto end;
    }

    if(0x24000400 == (OAD & 0xfff0ff00)) //��������
    {
         retS = pulse_get_OAD(OAD, (uint8 *)&tnewPulseCfg, sizeof(OOP_PULSESETS_T));
    }
    else if(0x24001C00 == (OAD & 0xfff0ff00)) //��������
    {
        retB = pulse_get_OAD(OAD, (uint8 *)&tnewBigCfg, sizeof(OOP_PULSESETS_B_T));
    }

    /* ��ȡ���µ����� */
    app_get_pulse(taskid, ptCalu);
    
    /* ȡ��ʷ���� */
    if(0 == g_LastRate || g_LastRate > OOP_MAX_RATES)
    {
        pulse_get_curtime_s(&tCurrTime);
        g_LastRate = pulse_get_rateNO(&(ptCfg->tDayPeriod), tCurrTime.hour, tCurrTime.minute);
        PULSE_FMT_TRACE("hour %d, min %d, sec %d, rate %d\n", tCurrTime.hour, tCurrTime.minute, tCurrTime.second, g_LastRate);
        if(0 == g_LastRate || g_LastRate > OOP_MAX_RATES)
        {
            goto end;
        }
    }

    ptMeasure = &(ptCfg->tPulse[dev]);
    if(0 == ptMeasure->tPulseCfg.nNum)
    {
        goto end;
    }

    for(j = 0; j < ptMeasure->tPulseCfg.nNum; j++)
    {
        pulseLocate = ptMeasure->tPulseCfg.pulse[j].port.value & 0x000000ff;
        powerid = ptMeasure->tPulseCfg.pulse[j].property;
        metK = ptMeasure->tPulseCfg.pulse[j].K;
        if(0 != ptMeasure->tPulseCfg.rsv && j < ptMeasure->tBigCfg.nNum)
        {
            /* ���յ���ʹ�ô����� */
            metK = ptMeasure->tBigCfg.pulse[j].K;
        }
        if(metK==0)
        {
            metK = 5000;
        }
        if(pulseLocate > ptCalu->pulseNum || 0 == pulseLocate || pulseLocate > OOP_MAX_PULSE)
        {
            continue;
        }

        pulseLocate -= 1;   //�±��0��ʼ
        
        /*��������㹦�ʺ͵�������*/
        ptMeasure->nActivePower = 0;
        ptMeasure->nReactivePower = 0;
        power = pulse_Acal_pow_en(ptCalu->Pulse[pulseLocate].val, ptCalu->Pulse[pulseLocate].interval, metK,
                                  &(ptCalu->Pulse[pulseLocate].caluleft), &en);
        PULSE_FMT_DEBUG("�˿�%d: ����%u, ����%u, K %u, ʣ��%u\n", 
                        pulseLocate + 1, power, en, metK, ptCalu->Pulse[pulseLocate].caluleft);
        
        /* �������� ��Ȼû��OAD��� */
//        powerDM = cal_pls_dm(power, &(ptCalu->Pulse[pulseLocate].powerNum), ptCalu->Pulse[pulseLocate].powerList);
//        PULSE_FMT_TRACE("DM power is %u\n", powerDM);

        /* Kֵ�仯, ʣ�����仯 */
        if(0x24000400 == (OAD & 0xfff0ff00))
        {
            if(0 != retS)
            {
                ptCalu->Pulse[pulseLocate].caluleft = 0;
            }
            else if(0 == tnewPulseCfg.rsv)
            {
                for(k = 0; k < tnewPulseCfg.nNum; k++)
                {
                    if(pulseLocate + 1 == (tnewPulseCfg.pulse[k].port.value & 0x000000ff))
                    {
                        newK = tnewPulseCfg.pulse[k].K;
                        break;
                    }
                }
                if(newK > 0)
                {
                    ptCalu->Pulse[pulseLocate].caluleft = (ptCalu->Pulse[pulseLocate].caluleft * 1L * newK)/metK;
                    
                }
            }
        }
        if(0x24001C00 == (OAD & 0xfff0ff00))
        {
            if(0 != retB)
            {
                ptCalu->Pulse[pulseLocate].caluleft = 0;
            }
            else
            {
                for(k = 0; k < tnewBigCfg.nNum; k++)
                {
                    if(pulseLocate + 1 == (tnewBigCfg.pulse[k].port.value & 0x000000ff))
                    {
                        newK = tnewBigCfg.pulse[k].K;
                        break;
                    }
                }
                if(newK > 0)
                {
                    ptCalu->Pulse[pulseLocate].caluleft = (ptCalu->Pulse[pulseLocate].caluleft * 1L * newK)/metK;
                    
                }
            }
        }
        
        PULSE_FMT_DEBUG("�˿�%d����ʣ��%u\n", pulseLocate + 1, ptCalu->Pulse[pulseLocate].caluleft);
        switch(powerid)
        {
            case POS_AE_Tag:
            {
                ptMeasure->nActivePower = (int)power;
                ptMeasure->tShowForwActEn.nNum = 5;
                ptMeasure->tShowForwActEn.nValue[0] += en;
                ptMeasure->tShowForwActEn.nValue[g_LastRate] += en;
                showEnerTotal = ptMeasure->tShowForwActEn.nValue[0];
                showEner = ptMeasure->tShowForwActEn.nValue[g_LastRate];
                break;
            }
            case POS_RE_Tag:
            {
                ptMeasure->nReactivePower = (int)power;
                ptMeasure->tShowForwRactEn.nNum = 5;
                ptMeasure->tShowForwRactEn.nValue[0] += en;
                ptMeasure->tShowForwRactEn.nValue[g_LastRate] += en;
                showEnerTotal = ptMeasure->tShowForwRactEn.nValue[0];
                showEner = ptMeasure->tShowForwRactEn.nValue[g_LastRate];
                break;
            }
            case NEG_AE_Tag:
            {
                ptMeasure->nActivePower = -((int)power);
                ptMeasure->tShowBackActEn.nNum = 5;
                ptMeasure->tShowBackActEn.nValue[0] += en;
                ptMeasure->tShowBackActEn.nValue[g_LastRate] += en;
                showEnerTotal = ptMeasure->tShowBackActEn.nValue[0];
                showEner = ptMeasure->tShowBackActEn.nValue[g_LastRate];
                break;
            }
            case NEG_RE_Tag:
            {
                ptMeasure->nReactivePower = -((int)power);
                ptMeasure->tShowBackRactEn.nNum = 5;
                ptMeasure->tShowBackRactEn.nValue[0] += en;
                ptMeasure->tShowBackRactEn.nValue[g_LastRate] += en;
                showEnerTotal = ptMeasure->tShowBackRactEn.nValue[0];
                showEner = ptMeasure->tShowBackRactEn.nValue[g_LastRate];
                break;
            }
            default: break;
        }
        PULSE_FMT_DEBUG("����%d �˿�%d(%d%s): ����%u, ����%u, K %u, ʣ��%u; �ܵ���%u, ����%d����%u\n", 
                        i + 1, pulseLocate + 1, powerid, powerid <= NEG_RE_Tag ? posname[powerid] : "δ֪", 
                        power, en, metK, ptCalu->Pulse[pulseLocate].caluleft,
                        showEnerTotal, g_LastRate, showEner);

        ptCalu->Pulse[pulseLocate].val = 0;
        ptCalu->Pulse[pulseLocate].interval = 0;
        
        for(i = 0; i < OOP_MAX_PULSE; i++)
        {
            if(k == pulseLocate)
            {
                ptCalu->tPulseTemp[i].count = 0;
                ptCalu->tPulseTemp[i].timems = 0;
                break;
            }
            k++;
        }
    }
    pulse_write_OAD(0x24010500+i*0x10000, (uint8 *)&(ptMeasure->nActivePower), sizeof(int32));
    pulse_write_OAD(0x24010600+i*0x10000, (uint8 *)&(ptMeasure->nReactivePower), sizeof(int32));
    pulse_write_OAD(0x24010f00+i*0x10000, (uint8 *)&(ptMeasure->tShowForwActEn), sizeof(OOP_ENERGY_T));
    pulse_write_OAD(0x24011000+i*0x10000, (uint8 *)&(ptMeasure->tShowForwRactEn), sizeof(OOP_ENERGY_T));
    pulse_write_OAD(0x24011100+i*0x10000, (uint8 *)&(ptMeasure->tShowBackActEn), sizeof(OOP_ENERGY_T));
    pulse_write_OAD(0x24011200+i*0x10000, (uint8 *)&(ptMeasure->tShowBackRactEn), sizeof(OOP_ENERGY_T));

end:
    return;
}

/**********************************************************************
* @name      : pulse_proc_calculate
* @brief     ���������
* @param[in] ��PULSE_OAD_T *ptCfg   OAD����
               PULSE_CALCULATE_T *ptCalu  ��������
               TIME_STRUCT_T *pTime  ��ǰʱ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-11-19
* @Update    :
**********************************************************************/
void pulse_proc_calculate(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu, TIME_STRUCT_T *pTime)
{
    uint8  i = 0, j = 0;
    uint8  rateNO = 0;
    uint8  pulseLocate = 0;
    uint8  powerid = 0;
    uint32 metK = 5000; //����� Ĭ����5000
    PULSE_Measure_T *ptMeasure = NULL;
    uint32 power;
    //uint32 powerDM;
    uint32 en;
    int    showEner = 0;
    uint32 showEnerTotal = 0;
    struct tm systime;
    struct timeval tv;
    struct timezone tz;
    uint8  rateNOLast = 0;
    char  *posname[NEG_RE_Tag + 1] = {"�����й�", "�����޹�", "�����й�", "�����޹�"};

    if(0 == ptCalu->pulseNum || ptCalu->pulseNum > OOP_MAX_PULSE)
    {
        return;
    }
    
    /* ��ȡ��ǰ���� */
    rateNO = pulse_get_rateNO(&(ptCfg->tDayPeriod), pTime->hour, pTime->minute);
    PULSE_FMT_DEBUG("%02d:%02d:%02d ����%d\n", pTime->hour, pTime->minute, pTime->second, rateNO);
    if(0 == rateNO || rateNO > OOP_MAX_RATES)
    {
        goto end;
    }

    if(0 == g_LastRate)
    {
        g_LastRate = rateNO;
    }
    
    /* ��ȡ��һ���ӵķ��� ��ֹ��ʱ���µ���ͳ�Ƶ���һ��ʱ���� */
    gettimeofday(&tv, &tz);
    tv.tv_sec -= 60;
    FastSecondToDate(&tv, &tz, &systime);
    rateNOLast = pulse_get_rateNO(&(ptCfg->tDayPeriod), systime.tm_hour, systime.tm_min);
    if(rateNOLast == rateNO)
    {
        g_LastRate = rateNO;
    }

    for(i = 0; i < OOP_MAX_PULSESETS; i++)
    {
        ptMeasure = &(ptCfg->tPulse[i]);
        if(0 == ptMeasure->tPulseCfg.nNum)
        {
            /* û�����ù���д0 */
            ptMeasure->nActivePower = 0;
            ptMeasure->nReactivePower = 0;
            pulse_write_OAD(0x24010500+i*0x10000, (uint8 *)&(ptMeasure->nActivePower), sizeof(int32));
            pulse_write_OAD(0x24010600+i*0x10000, (uint8 *)&(ptMeasure->nReactivePower), sizeof(int32));
            continue;
        }

        ptMeasure->nActivePower = 0;
        ptMeasure->nReactivePower = 0;
        for(j = 0; j < ptMeasure->tPulseCfg.nNum; j++)
        {
            /* port.value = 0xf20a02XX */
            pulseLocate = ptMeasure->tPulseCfg.pulse[j].port.nIndex;
            powerid = ptMeasure->tPulseCfg.pulse[j].property;
            metK = ptMeasure->tPulseCfg.pulse[j].K;
            if(0 != ptMeasure->tPulseCfg.rsv && j < ptMeasure->tBigCfg.nNum)
            {
                /* ���յ���ʹ�ô����� */
                metK = ptMeasure->tBigCfg.pulse[j].K;
            }
            if(metK==0)
            {
                metK = 5000;
            }
            if(pulseLocate > ptCalu->pulseNum || 0 == pulseLocate)
            {
                continue;
            }

            pulseLocate -= 1;   //�±��0��ʼ
            
            /*��������㹦�ʺ͵�������*/
            power = pulse_Acal_pow_en(ptCalu->Pulse[pulseLocate].val, ptCalu->Pulse[pulseLocate].interval, metK, 
                                      &(ptCalu->Pulse[pulseLocate].caluleft), &en);
            
            /* �������� ��Ȼû��OAD��� */
            if(g_LastRate != rateNO)
            {
                /*����ͬһ������ʱ��,���������������*/
                memset(ptCalu->Pulse[pulseLocate].powerList, 0, DM_POWER_MAX * sizeof(uint32));
                ptCalu->Pulse[pulseLocate].powerNum = 0;
            }
            //powerDM = cal_pls_dm(power, &(ptCalu->Pulse[pulseLocate].powerNum), ptCalu->Pulse[pulseLocate].powerList);
            //PULSE_FMT_TRACE("%d dev index %d DM power is %u\n", i, pulseLocate + 1, powerDM);
            
            switch(powerid)
            {
                case POS_AE_Tag://�����й�
                {
                    ptMeasure->nActivePower += (int)power;
                    ptMeasure->tShowForwActEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tShowForwActEn.nValue[0] += en;
                    ptMeasure->tShowForwActEn.nValue[g_LastRate] += en;
                    showEnerTotal = ptMeasure->tShowForwActEn.nValue[0];
                    showEner = ptMeasure->tShowForwActEn.nValue[g_LastRate];
                    break;
                }
                case POS_RE_Tag://�����޹�
                {
                    ptMeasure->nReactivePower += (int)power;
                    ptMeasure->tShowForwRactEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tShowForwRactEn.nValue[0] += en;
                    ptMeasure->tShowForwRactEn.nValue[g_LastRate] += en;
                    showEnerTotal = ptMeasure->tShowForwRactEn.nValue[0];
                    showEner = ptMeasure->tShowForwRactEn.nValue[g_LastRate];
                    break;
                }
                case NEG_AE_Tag://�����й�
                {
                    ptMeasure->nActivePower -= ((int)power);
                    ptMeasure->tShowBackActEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tShowBackActEn.nValue[0] += en;
                    ptMeasure->tShowBackActEn.nValue[g_LastRate] += en;
                    showEnerTotal = ptMeasure->tShowBackActEn.nValue[0];
                    showEner = ptMeasure->tShowBackActEn.nValue[g_LastRate];
                    break;
                }
                case NEG_RE_Tag://�����޹�
                {
                    ptMeasure->nReactivePower -= ((int)power);
                    ptMeasure->tShowBackRactEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tShowBackRactEn.nValue[0] += en;
                    ptMeasure->tShowBackRactEn.nValue[g_LastRate] += en;
                    showEnerTotal = ptMeasure->tShowBackRactEn.nValue[0];
                    showEner = ptMeasure->tShowBackRactEn.nValue[g_LastRate];
                    break;
                }
                default: break;
            }

            PULSE_FMT_DEBUG("����%d �˿�%d(%d%s): ����%u, ����%u, K %u, ʣ��%u; �ܵ���%u, ����%d����%u\n", 
                            i + 1, pulseLocate + 1, powerid, powerid <= NEG_RE_Tag ? posname[powerid] : "δ֪", 
                            power, en, metK, ptCalu->Pulse[pulseLocate].caluleft, 
                            showEnerTotal, g_LastRate, showEner);
        }
        pulse_write_OAD(0x24010500+i*0x10000, (uint8 *)&(ptMeasure->nActivePower), sizeof(int32));
        pulse_write_OAD(0x24010600+i*0x10000, (uint8 *)&(ptMeasure->nReactivePower), sizeof(int32));
        pulse_write_OAD(0x24010f00+i*0x10000, (uint8 *)&(ptMeasure->tShowForwActEn), sizeof(OOP_ENERGY_T));
        pulse_write_OAD(0x24011000+i*0x10000, (uint8 *)&(ptMeasure->tShowForwRactEn), sizeof(OOP_ENERGY_T));
        pulse_write_OAD(0x24011100+i*0x10000, (uint8 *)&(ptMeasure->tShowBackActEn), sizeof(OOP_ENERGY_T));
        pulse_write_OAD(0x24011200+i*0x10000, (uint8 *)&(ptMeasure->tShowBackRactEn), sizeof(OOP_ENERGY_T));
    }
    
    g_LastRate = rateNO;
end:
    for(i = 0; i < OOP_MAX_PULSE; i++)
    {
        ptCalu->Pulse[i].val = 0;
        ptCalu->Pulse[i].interval = 0;
        ptCalu->tPulseTemp[i].count = 0;
        ptCalu->tPulseTemp[i].timems = 0;
    }
}

/**********************************************************************
* @name      : pulse_check_status
* @brief     ��ͳ�ƽڵ���Ϣ
* @param[in] ��(PULSE_OAD_T *ptCfg
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2022-6-21
* @Update    :
**********************************************************************/
void pulse_check_status(PULSE_OAD_T *ptCfg)
{
    uint32 statuscur;
    static uint32 status1 = 0;
    static uint32 status2 = 0;
    static uint32 status3 = 0;
    int ret = 0;
    OOP_STATUS_SIGNAL_T  tStatusSignal;
    
    STATUS_SIGNAL_DEVICE_T *pDev = (STATUS_SIGNAL_DEVICE_T *)ptCfg->pHalDev;
    if (NULL == pDev)
    {
        return;
    }
    
    ret = pDev->status_singal_get(pDev, &statuscur);
    if(0 != ret)
    {
        return;
    }

    memcpy(&tStatusSignal, &ptCfg->tStatusSignal, sizeof(OOP_STATUS_SIGNAL_T));

    if(ptCfg->tStatusSignal.nNum < 1)
    {
        ptCfg->tStatusSignal.nNum = 1;
    }
    
    /* �Žڵ� */
    if((statuscur & 0x01) == (status1 & 0x01) && (statuscur & 0x01) == (status2 & 0x01) &&
       (statuscur & 0x01) == (status3 & 0x01))
    {
        if((statuscur & 0x01) != (ptCfg->tStatusSignal.ST.nValue & 0x01))
        {
            if(statuscur & 0x01)
            {
                ptCfg->tStatusSignal.ST.nValue = 0x01;
            }
            else
            {
                ptCfg->tStatusSignal.ST.nValue = 0;
            }
            ptCfg->tStatusSignal.CT.nValue = (ptCfg->tStatusSignal.CT.nValue | (0x01 << 0));

            PULSE_FMT_DEBUG("�Žڵ��λ%s\n", 0 == (statuscur & 0x01) ? "��" : "��");
#if defined PRODUCT_CCU || defined PRODUCT_ZCU_3
            //OOP_SWITCHIN_STATE_T tSwitchState;
            //memcpy(&tSwitchState, &ptCfg->tSwitchState, sizeof(OOP_SWITCHIN_STATE_T));
            //tSwitchState.state[7].st = (ptCfg->tStatusSignal.ST.nValue & 0x01);
            //tSwitchState.state[7].cd = 1;
            //CheckERC_3104(g_pulsedb, tSwitchState);
#endif
        }
    }

    status3 = status2;
    status2 = status1;
    status1 = statuscur;
    if(0 != memcmp(&tStatusSignal, &ptCfg->tStatusSignal, sizeof(OOP_STATUS_SIGNAL_T)))
    {
        pulse_write_OAD(0xF2030600, (uint8*)&ptCfg->tStatusSignal, sizeof(OOP_STATUS_SIGNAL_T));
    }
}


/**********************************************************************
* @name      : pulse_write_OAD
* @brief     ��д��OAD
* @param[in] ��uint32 uOAD
               uint8 *pData
               uint16 dataLen
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2019-11-21
* @Update    :
**********************************************************************/
int pulse_write_OAD(uint32 uOAD, uint8 *pData, uint32 dataLen)
{
    NOMAL_OAD_T tOAD;
    int         ret = 0;
    if(g_pulsedb <= 0 || NULL == pData || 0 == dataLen)
    {
        return -1;
    }
    
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.oad.value = uOAD;
    tOAD.classtag = CLASS_DATA_NORMAL;
    ret = db_write_nomal(g_pulsedb, &tOAD, pData, dataLen);
    if(0 != ret)
    {
        ret = db_write_nomal(g_pulsedb, &tOAD, pData, dataLen);
        PULSE_FMT_DEBUG("pulse_write_OAD 0x%x failed, try again new ret %d\n", uOAD, ret);
        return ret;
    }
    return 0;
}

/**********************************************************************
* @name      : pulse_write_type_OAD
* @brief     ��д��OAD
* @param[in] ��uint32 uOAD
               uint16 classtag
               void  *pData
               uint16 dataLen
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-7-2
* @Update    :
**********************************************************************/
int pulse_write_INITOAD(uint32 uOAD, uint16 classtag, void *pData, uint32 dataLen)
{
    NOMAL_OAD_T tOAD;
    int         ret = 0;
    if(g_pulsedb <= 0 || NULL == pData || 0 == dataLen)
    {
        return -1;
    }
    
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.oad.value = uOAD;
    tOAD.classtag = classtag;
    ret = db_write_nomal(g_pulsedb, &tOAD, (uint8 *)pData, dataLen);
    if(0 != ret)
    {
        ret = db_write_nomal(g_pulsedb, &tOAD, (uint8 *)pData, dataLen);
        PULSE_FMT_DEBUG("pulse_write_INITOAD 0x%x failed, try again new ret %d\n", uOAD, ret);
        return ret;
    }
    return 0;
}

/**********************************************************************
* @name      : pulse_get_OAD
* @brief     ����ȡOAD
* @param[in] ��uint32 uOAD
               uint8 *pBuf
               uint32 bufLen
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2019-11-22
* @Update    :
**********************************************************************/
int pulse_get_OAD(uint32 uOAD, uint8 *pBuf, uint32 bufLen)
{
    NOMAL_OAD_T tOAD;
    int         ret = 0;
    uint32      readLen = 0;
    
    if(g_pulsedb <= 0 || NULL == pBuf || 0 == bufLen)
    {
        return -1;
    }
    
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.oad.value = uOAD;
    ret = db_read_nomal(g_pulsedb, &tOAD, bufLen, pBuf, &readLen);
    if(0 != ret || readLen != bufLen)
    {
        return -1;
    }
    return 0;
}

/**********************************************************************
* @name      : pulse_update_db
* @brief     �������������ݸ���
* @param[in] ��uint32 uOAD           OAD
* @param[out]��PULSE_OAD_T *ptCfg    ��������
               PULSE_CALCULATE_T *ptCalu �������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-1-13
* @Update    : 2020-1-13
**********************************************************************/
int pulse_update_db(uint32 uOAD, PULSE_OAD_T *ptCfg)
{
    int ret = -3;
    uint8 dev = 0;
    
    if(0xf2030300 == uOAD) //�ź����豸��������
    {
        ret = pulse_get_OAD(0xf2030300, (uint8 *)&(ptCfg->nSwitchNUM), sizeof(uint8));
    }
    else if(0xf2030400 == uOAD) //�ź�����������
    {
        ret = pulse_get_OAD(0xf2030400, (uint8 *)&(ptCfg->tSwitchFlag), sizeof(OOP_SWITCHIN_FLAG_T));
    }
    else if(0x24000300 == (uOAD & 0xfff0ff00)) //����������
    {
        dev = (uint8)((uOAD & 0x000f0000) >> 16);
        if(dev <= OOP_MAX_PULSE && dev > 0)
        {
            ret = pulse_get_OAD(uOAD, (uint8 *)&(ptCfg->tPulse[dev-1].tPTCT), sizeof(OOP_PTCT_T));
        }
    }
    else if(0x24000400 == (uOAD & 0xfff0ff00)) //��������
    {
        dev = (uint8)((uOAD & 0x000f0000) >> 16);
        if(dev <= OOP_MAX_PULSE && dev > 0)
        {
            ret = pulse_get_OAD(uOAD, (uint8 *)&(ptCfg->tPulse[dev-1].tPulseCfg), sizeof(OOP_PULSESETS_T));
        }
    }
    else if(0x24001C00 == (uOAD & 0xfff0ff00)) //��������
    {
        dev = (uint8)((uOAD & 0x000f0000) >> 16);
        if(dev <= OOP_MAX_PULSE && dev > 0)
        {
            ret = pulse_get_OAD(uOAD, (uint8 *)&(ptCfg->tPulse[dev-1].tBigCfg), sizeof(OOP_PULSESETS_B_T));
        }
    }
    else if(0x40160200 == uOAD) //��ǰ����ʱ�α�
    {
        ret = pulse_get_OAD(0x40160200, (uint8 *)&(ptCfg->tDayPeriod), sizeof(OOP_DAYPERIOD_T));
    }
    else if(0x49080200 == uOAD) //ң�ŷ���ʱ��
    {
        uint32 rmtTime = 0;

        ret = pulse_get_OAD(0x49080200, (uint8 *)&(rmtTime), sizeof(uint32));
        if(0 == ret)
        {
            ptCfg->bChangeTime = TRUE;
            ptCfg->rmtTime = rmtTime;
            PULSE_FMT_DEBUG("Pulse update 0x49080200, time = %d\n", rmtTime);
        }
    }
    PULSE_FMT_DEBUG("Pulse update 0x%x, ret = %d\n", uOAD, ret);

    return 0;
}

/**********************************************************************
* @name      : pulse_write_normal_init
* @brief     �����ݳ�ʼ������д
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-3-6
* @Update    :
**********************************************************************/
void pulse_write_normal_init(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu)
{
    uint8 i;
    int   ret = 0;
    PULSE_Measure_T *ptMeasure = NULL;

    PULSE_FMT_DEBUG("Init write switch state\n");

    /* ң�����ݳ�ʼ�� */
    for(i = 0; i < OOP_MAX_SWITCHIN; i++)
    {
        ptCfg->tSwitchState.state[i].cd = 0;
    }
    
    ret = pulse_write_OAD(0xf2030200, (uint8 *)&(ptCfg->tSwitchState), sizeof(OOP_SWITCHIN_STATE_T));
    if(0 != ret)
    {
        PULSE_FMT_TRACE("Init write switch state failed %d\n", ret);
    }

    /* �������ݳ�ʼ�� */
    for(i = 0; i < OOP_MAX_PULSE; i++)
    {
        ptCalu->Pulse[i].caluleft = 0;
        ptMeasure = &(ptCfg->tPulse[i]);
        if(0 == ptMeasure->tPulseCfg.nNum)
        {
            continue;
        }
        ptMeasure->nActivePower = 0;
        pulse_write_OAD(0x24010500 + i*0x10000, (uint8 *)&(ptMeasure->nActivePower), sizeof(int32));

        ptMeasure->nReactivePower = 0;
        pulse_write_OAD(0x24010600 + i*0x10000, (uint8 *)&(ptMeasure->nReactivePower), sizeof(int32));

        memset(&(ptMeasure->tShowForwActEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tShowForwActEn.nNum = OOP_FULL_RATES;
        pulse_write_OAD(0x24010f00 + i*0x10000, (uint8 *)&(ptMeasure->tShowForwActEn), sizeof(OOP_ENERGY_T));
        
        memset(&(ptMeasure->tShowBackActEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tShowBackActEn.nNum = OOP_FULL_RATES;
        pulse_write_OAD(0x24011000 + i*0x10000, (uint8 *)&(ptMeasure->tShowBackActEn), sizeof(OOP_ENERGY_T));

        memset(&(ptMeasure->tShowForwRactEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tShowForwRactEn.nNum = OOP_FULL_RATES;
        pulse_write_OAD(0x24011100 + i*0x10000, (uint8 *)&(ptMeasure->tShowForwRactEn), sizeof(OOP_ENERGY_T));

        memset(&(ptMeasure->tShowBackRactEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tShowBackRactEn.nNum = OOP_FULL_RATES;
        pulse_write_OAD(0x24011200 + i*0x10000, (uint8 *)&(ptMeasure->tShowBackRactEn), sizeof(OOP_ENERGY_T));
    }
}

/**********************************************************************
* @name      : pulse_arg_arg_init
* @brief     �����ݻָ���������
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-3-6
* @Update    :
**********************************************************************/
void pulse_arg_arg_init(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu)
{
    uint8 i;
    PULSE_Measure_T *ptMeasure = NULL;
    int   ret = 0;

    PULSE_FMT_DEBUG("Init default\n");

    memset(&(ptCfg->tSwitchFlag), 0, sizeof(OOP_SWITCHIN_FLAG_T));

    for(i = 1; i <= OOP_MAX_PULSE; i++)
    {
        ptCalu->Pulse[i-1].caluleft = 0;
        ptMeasure = &(ptCfg->tPulse[i-1]);
        
        //����������
        memset(&(ptMeasure->tPTCT), 0, sizeof(OOP_PTCT_T));
        pulse_get_OAD(0x24000300 + i * 0x10000, (uint8 *)&(ptMeasure->tPTCT), sizeof(OOP_PTCT_T));

        //��������
        memset(&(ptMeasure->tPulseCfg), 0, sizeof(OOP_PULSESETS_T));
        pulse_get_OAD(0x24000400 + i * 0x10000, (uint8 *)&(ptMeasure->tPulseCfg), sizeof(OOP_PULSESETS_T));

        memset(&(ptMeasure->tBigCfg), 0, sizeof(OOP_PULSESETS_B_T));
        pulse_get_OAD(0x24001C00 + i * 0x10000, (uint8 *)&(ptMeasure->tBigCfg), sizeof(OOP_PULSESETS_B_T));
    }

    /* ��ǰ����ʱ�α� */
    memset(&(ptCfg->tDayPeriod), 0, sizeof(OOP_DAYPERIOD_T));
    pulse_get_OAD(0x40160200, (uint8 *)&(ptCfg->tDayPeriod), sizeof(OOP_DAYPERIOD_T));

    /* ң�ŷ���ʱ�� */
    ptCfg->rmtTime = 0;
    pulse_get_OAD(0x49080200, (uint8 *)&(ptCfg->rmtTime), sizeof(uint32));

    ret = pulse_get_OAD(0xf2030300, (uint8 *)&(ptCfg->nSwitchNUM), sizeof(uint8));
    if(0 != ret)
    {
        PULSE_FMT_DEBUG("Get 0xf2030300 failed %d\n", ret);
        ptCfg->nSwitchNUM = 0;
    }
    
    ret = pulse_get_OAD(0xf2030400, (uint8 *)&(ptCfg->tSwitchFlag), sizeof(OOP_SWITCHIN_FLAG_T));
    if(0 != ret)
    {
        PULSE_FMT_DEBUG("Get 0xf2030400 failed\n");
        ptCfg->tSwitchFlag.inputFlag.bitValue = 0;
        ptCfg->tSwitchFlag.attrFlag.bitValue = 0xff;
    }
}

/**********************************************************************
* @name      : pulse_write_pulse
* @brief     ������д��洢�ļ���
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-4
* @Update    :
**********************************************************************/
void pulse_write_pulse(PULSE_CALCULATE_T *ptCalu)
{
    write_pdata_xram(ptCalu->datafile, (char *)&(ptCalu->tPulseTemp[0]), 4, OOP_MAX_PULSE * sizeof(PULSE_TEMP_T));
}

