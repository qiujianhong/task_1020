/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   遥脉计算模块
* @date：    2019-10-29
* @history： 
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
* @brief     ：写入队列
* @param[in] ：PULSE_CHG_LIST_T *pList          上报队列
               OOP_SWITCHIN_STATE_T *pNode     写入内容
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2023-1-3
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
* @brief     ：读出队列
* @param[in] ：PULSE_CHG_LIST_T *pList         上报队列
* @param[out]：OOP_SWITCHIN_STATE_T *pNode     上报内容
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2023-1-3
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
* @brief     ：初始化数据中心客户端, 并读取数据中心数据
* @param[in] ：
* @param[out]：PULSE_OAD_T *ptCfg    数据中心
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-18
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
    
    /* 1 读取遥脉配置 */
    tOAD.oad.value = 0xf2030300; //信号量设备对象数量
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
    
    tOAD.oad.value = 0xf2030200; //信号量列表参数
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
    
    tOAD.oad.value = 0xf2030400; //信号量接入属性
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

    tOAD.oad.value = 0xf2030600; //信号
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
    
    /* f20a 先不读取 */
#if 0
    /* 2 读取脉冲输入设备 */
    tOAD.oad.value = 0xf20a0300; //脉冲输入设备对象数量
    readLen = 0;
    retDb = nomal_data_read(g_pPulseDb, tOAD, sizeof(uint8), (uint8 *)&(ptCfg->nPulseNum), &readLen);
    if(0 == retDb && readLen == sizeof(uint8) && 0 != ptCfg->nPulseNum)
    {
        tOAD.oad.value = 0xf20a0200; //脉冲输入端口描述
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

    /* 3 脉冲计量 */
    for(i = 1; i <= OOP_MAX_PULSE; i++)
    {
        ptPM = &(ptCfg->tPulse[i-1]);
        
        tOAD.oad.value = 0x24000300 + i * 0x10000; //互感器倍率
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_PTCT_T), (uint8 *)&(ptPM->tPTCT), &readLen);
        if(0 != ret || readLen != sizeof(OOP_PTCT_T))
        {
            PULSE_FMT_DEBUG("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
        }

        tOAD.oad.value = 0x24000400 + i * 0x10000; //脉冲配置
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_PULSESETS_T), (uint8 *)&(ptPM->tPulseCfg), &readLen);
        if(0 != ret || readLen != sizeof(OOP_PULSESETS_T))
        {
            PULSE_FMT_DEBUG("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(ptPM, 0, sizeof(PULSE_Measure_T));
            continue;
        }

        tOAD.oad.value = 0x24001C00 + i * 0x10000; //脉冲配置
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_PULSESETS_B_T), (uint8 *)&(ptPM->tBigCfg), &readLen);
        if(0 != ret || readLen != sizeof(OOP_PULSESETS_B_T))
        {
            PULSE_FMT_DEBUG("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
        }
        /* 不需要判重复 */
#if 0
        /* 对脉冲配置检查 是否和遥信冲突 */
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
        tOAD.oad.value = 0x24000500 + i * 0x10000; //有功功率
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(int32), (uint8 *)&(ptPM->nActivePower), &readLen);
        if(readLen != sizeof(int32))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nActivePower = 0;
        }

        tOAD.oad.value = 0x24000600 + i * 0x10000; //无功功率
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(int32), (uint8 *)&(ptPM->nReactivePower), &readLen);
        if(0 != ret || readLen != sizeof(int32))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nReactivePower = 0;
        }
        
        tOAD.oad.value = 0x24000f00 + i * 0x10000; //正向有功电能示值
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tShowForwActEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nReactivePower = 0;
        }

        tOAD.oad.value = 0x24001000 + i * 0x10000; //正向无功电能示值
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tShowForwRactEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nReactivePower = 0;
        }
        
        tOAD.oad.value = 0x24001100 + i * 0x10000; //反向有功电能示值
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tShowBackActEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nReactivePower = 0;
        }

        tOAD.oad.value = 0x24001200 + i * 0x10000; //反向无功电能示值
        readLen = 0;
        ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tShowBackRactEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nReactivePower = 0;
        }
    }

    /* 当前套日时段表 */
    tOAD.oad.value = 0x40160200; //当前套日时段表
    readLen = 0;
    ret = db_read_nomal(sockfd, &tOAD, sizeof(OOP_DAYPERIOD_T), (uint8 *)&(ptCfg->tDayPeriod), &readLen);
    if(0 != ret  || readLen != sizeof(OOP_DAYPERIOD_T))
    {
        PULSE_FMT_DEBUG("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
        //return -1;
    }

    /* 遥信防抖时间 */
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
* @brief     ：获取系统时间, 精确到s 提前5s
* @param[in] ：
* @return    ：TIME_STRUCT_T *pTime
* @Create    : 王津剑
* @Date      ：2019-11-19
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
    pTime->month = (uint8)systime.tm_mon;         /*月份0代表1月*/
    pTime->year = (uint8)systime.tm_year;         /*年份0代表1900年*/
}

/**********************************************************************
* @name      : pulse_calculate_init
* @brief     ：eeprom初始化并获取遗留信息, 时间不存在就取当前时间
* @param[in] ：PULSE_CALCULATE_T *ptCfg
* @param[out]：TIME_STRUCT_T *pTime
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-19
* @Update    :
**********************************************************************/
int pulse_calculate_init(PULSE_CALCULATE_T *ptCalu, TIME_STRUCT_T *pTime)
{
    char check[4] = {0xea, 0xeb, 0xec, 0x03}; /* 最后一位是消息版本号, 每修改一次增加1 */
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
                    /* 设备返回数据压缩 */
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
* @brief     ：设置遥信脉冲功能配置
* @param[in] ：int taskid               通信句柄
               PULSE_CFG_T *pPluseCfg  配置
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-1
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
        /* 设置配置 */
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
        PULSE_FMT_DEBUG("设置设备%d端口配置%s\n", pPluseCfg->cfg[i].no, 0 == retbool ? "失败" : "成功");
    }

    task_msg_settime(taskid, 0, 100000);  //设置阻塞超时时间为0.1s
    return 0;
}

/**********************************************************************
* @name      : app_set_shaketime
* @brief     ：设置遥信脉冲防抖时间
* @param[in] ：int taskid       线程句柄
               uint32 timelong  防抖时间
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-8-3
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
* @brief     ：主动回去遥信状态
* @param[in] ：int taskid       线程句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-1
* @Update    :
**********************************************************************/
void app_get_rmt(int taskid, PULSE_CFG_T *pPluseCfg)
{
    uint8 pulseNum = 0xFF;
    int   ret = 0;

    ret = task_send_labelmsg(taskid, MSG_PULSE_GET_RMT, pulseNum, &pulseNum, 1, MSG_MONITOR);
    PULSE_FMT_DEBUG("获取所有遥信状态 %s\n", 0 == ret ? "success" : "failed");
}

/**********************************************************************
* @name      : app_proc_rmt
* @brief     ：遥信处理
* @param[in] ：uint8 id                  设备号 为0重新写入一次数据库
               uint8 *pMsg               消息内容
               uint16 msglen             消息长度
               PULSE_OAD_T *ptCfg        配置
               PULSE_CALCULATE_T *ptCalu 计算
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-1
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
    uint8 bEvent3104 = FALSE; //是否产生遥信变位事件
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

    PULSE_FMT_DEBUG("设备%d: 遥信数量%d, 状态(%s); 接收前状态(%s), 汇总后状态(%s), %s\n", id, 
                    STNum, 0 == showtempDev[0] ? "NULL" : showtempDev, showtempOld, showtempNew,
                    TRUE == bEvent3104 ? "变位" : "不变");

    /* 状态存私有变量 */
    pulse_write_OAD(0xf20302FF, ptCalu->curST, OOP_MAX_SWITCHIN);

    memset(ptCalu->realST, 0, OOP_MAX_SWITCHIN);
    /* 模组返回0-合,698规约1-合,需要转换 适配接入属性 */
    for(i = 0; i < OOP_MAX_SWITCHIN; i++)
    {
        if(ptCfg->tSwitchFlag.inputFlag.bitValue & (0x01 << i))
        {
            if(ptCfg->tSwitchFlag.attrFlag.bitValue & (0x01 << i))
            {
                /* 常开需要遥信状态取反 */
                ptCalu->realST[i] = 1 - ptCalu->curST[i];
            }
            else
            {
                /* 常闭取原值 */
                ptCalu->realST[i] = ptCalu->curST[i];
            }
            offReal += sprintf(showtempReal + offReal, " %s", 0 == ptCalu->realST[i] ? "分" : "合");
        }
        else
        {
            offReal += sprintf(showtempReal + offReal, " --");
        }
    }

    /*  */

    /* 先获取数据中心新的变位状态 */
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
        /* 拷贝变位 */
        offST += sprintf(showtempST + offST, " %s", 0 == ptCfg->tSwitchState.state[i].st ? "分" : "合");
        
        ptCfg->tSwitchState.state[i].cd = tOADState.state[i].cd;  //拷贝数据中心的新数据
        /* 判断是否变位 */
        if(ptCfg->tSwitchState.state[i].st != ptCalu->realST[i])
        {
            ptCfg->tSwitchState.state[i].cd = 1;
        }
        ptCfg->tSwitchState.state[i].st = ptCalu->realST[i];
        offCT += sprintf(showtempCT + offCT, " %s", 1 == ptCfg->tSwitchState.state[i].cd ? "变" : "同");
        
    }
    PULSE_FMT_DEBUG("遥信路数: \n          一 二 三 四 五 六 七 八 九 十 一 二 三 四 五 六\n"
                    "历史状态:%s\n"
                    "实际状态:%s\n"
                    "变位状态:%s\n", showtempST, showtempReal, showtempCT);
                    
    /* 状态没有变化, 不需要写数据库 */
    if(0 == memcmp(&tSwitchState, &ptCfg->tSwitchState, sizeof(OOP_SWITCHIN_STATE_T)))
    {
        return;
    }

    /* 没有接入设备不产生变位事件 */
    if(TRUE == bEvent3104 && 0 != ptCfg->tSwitchFlag.inputFlag.nValue)
    {
        PULSE_FMT_DEBUG("Dev %d, 产生遥信变位事件\n", id);
#if defined PRODUCT_CCU || defined PRODUCT_ZCU_3
        /* 集中器第8路作为门节点 */
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
    //湖南用遥信1接总开关 监控开关分合状态 enum{分（1），合（2）}
    uint8 state =ptCfg->tSwitchState.state[0].st+1;  //状态ST——0：“分”状态；1：“合”状态
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
* @brief     ：遥信处理
* @param[in] ：PULSE_OAD_T *ptCfg        配置
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-1
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
            PULSE_FMT_DEBUG("停电, 丢弃事件\n");
        }
        else
        {
            PULSE_FMT_DEBUG("延时产生\n");
            DT_Time2DateTimeShort(tNode.happentmes, &dateTime);
            CheckERC_3104_seu(g_pulsedb, dateTime, tNode.sc);
        }
    }
}

/**********************************************************************
* @name      : app_reproc_rmt
* @brief     ：遥信变位预处理
* @param[in] ：PULSE_OAD_T *ptCfg        配置
               PULSE_CALCULATE_T *ptCalu 计算
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-16
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

        /* 初始化状态 */
        ret = pulse_write_OAD(0xf2030200, (uint8 *)&(ptCfg->tSwitchState), sizeof(OOP_SWITCHIN_STATE_T));
        if(0 != ret)
        {
            PULSE_FMT_DEBUG("Write switch state 0xf2030200 failed, code %d\n", ret);
        }
    }
}

/**********************************************************************
* @name      : app_get_pulse
* @brief     ：脉冲采集
* @param[in] ：int taskid                线程句柄
* @param[out]：PULSE_CALCULATE_T *ptCalu 统计结果
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-1
* @Update    :
**********************************************************************/
void app_get_pulse(int taskid, PULSE_CALCULATE_T *ptCalu)
{
    uint8 i;
    uint8 j;
    uint8 no = 0xFF;
    uint8 index = 0; //当前序号
    int   ret = 0;
    uint8 pulseNum = 0;
    uint8  recvBuf[200] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;
    RST_PULSE_STATIC_T tPulse;
    char  strtemp[600] = {0};
    uint16 len = 0;
    uint64 lastms = 0;
    uint64 endms = 0;

    PULSE_FMT_DEBUG("获取所有脉冲\n");

    ret = task_syncmsg_safe(taskid, MSG_PULSE_GET_PULSE, &no, sizeof(uint8), MSG_MONITOR, recvBuf, 200, 3);
    if(0 == ret)
    {
        memset(&tPulse, 0, sizeof(RST_PULSE_STATIC_T));
        PULSE_BUF_DEBUG(pMsg->msg, pMsg->msglen, "获取消息");
        appmsg_rsppulse_split(pMsg->msg, &tPulse);
        for(j = 0; j < tPulse.num; j++)
        {
            if(0 == tPulse.item[j].flag)
            {
                continue;
            }
            /* 判断是否重启 携带启动时间的接口 */
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
                    /* 反转 */
                    ptCalu->tPulseTemp[index].count += (0xFFFFFFFF - ptCalu->tPulseTemp[index].totalcou) + tPulse.item[j].count;
                }
                
                ptCalu->tPulseTemp[index].timems += (uint32)(endms - lastms);
            }
            
            ptCalu->tPulseTemp[index].totalcou = tPulse.item[j].count;
            ptCalu->tPulseTemp[index].totals = tPulse.item[j].timesec;
            ptCalu->tPulseTemp[index].totalms = tPulse.item[j].timems;

            len += sprintf(strtemp + len, "端口%d: 总数%u, 时间 %u.%03us, 增加%u, 时间%ums\n",
                           j + 1, tPulse.item[j].count, tPulse.item[j].timesec, tPulse.item[j].timems, 
                           ptCalu->tPulseTemp[index].count, ptCalu->tPulseTemp[index].timems);
            index++;
        }
        PULSE_FMT_DEBUG("脉冲\n%s\n", strtemp);
    }

    /* 设备返回数据压缩 */
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
* @brief     ：电表常数发生变化, 计算后取新的K值
* @param[in] ：int taskid   任务句柄
               uint8 dev    设备编号   0表示1号对象, 1表示2号对象
               PULSE_OAD_T *ptCfg
               PULSE_CALCULATE_T *ptCalu
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-22
* @Update    :
**********************************************************************/
void pulse_change_K(int taskid, uint32 OAD, PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu)
{
    uint8  j;
    uint8  pulseLocate = 0;
    uint8  powerid = 0;
    uint32 metK = 5000; //电表常数 默认是5000
    uint32 newK = 0;    //电表常数
    PULSE_Measure_T *ptMeasure = NULL;
    uint32 power;
    //uint32 powerDM;
    uint32 en;
    TIME_STRUCT_T tCurrTime = {0};
    uint8 i = 0, k = 0;
    int    showEner = 0;
    uint32 showEnerTotal = 0;
    uint8 dev = 0;
    OOP_PULSESETS_T      tnewPulseCfg;     //脉冲配置
    OOP_PULSESETS_B_T    tnewBigCfg;
    int retS = 1;
    int retB = 1;
    char  *posname[NEG_RE_Tag + 1] = {"正向有功", "正向无功", "反向有功", "反向无功"};
    
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

    if(0x24000400 == (OAD & 0xfff0ff00)) //脉冲配置
    {
         retS = pulse_get_OAD(OAD, (uint8 *)&tnewPulseCfg, sizeof(OOP_PULSESETS_T));
    }
    else if(0x24001C00 == (OAD & 0xfff0ff00)) //脉冲配置
    {
        retB = pulse_get_OAD(OAD, (uint8 *)&tnewBigCfg, sizeof(OOP_PULSESETS_B_T));
    }

    /* 获取最新的脉冲 */
    app_get_pulse(taskid, ptCalu);
    
    /* 取历史费率 */
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
            /* 江苏地区使用大脉冲 */
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

        pulseLocate -= 1;   //下表从0开始
        
        /*脉冲计量点功率和电量计算*/
        ptMeasure->nActivePower = 0;
        ptMeasure->nReactivePower = 0;
        power = pulse_Acal_pow_en(ptCalu->Pulse[pulseLocate].val, ptCalu->Pulse[pulseLocate].interval, metK,
                                  &(ptCalu->Pulse[pulseLocate].caluleft), &en);
        PULSE_FMT_DEBUG("端口%d: 功率%u, 电量%u, K %u, 剩余%u\n", 
                        pulseLocate + 1, power, en, metK, ptCalu->Pulse[pulseLocate].caluleft);
        
        /* 需量计算 虽然没有OAD存放 */
//        powerDM = cal_pls_dm(power, &(ptCalu->Pulse[pulseLocate].powerNum), ptCalu->Pulse[pulseLocate].powerList);
//        PULSE_FMT_TRACE("DM power is %u\n", powerDM);

        /* K值变化, 剩余计算变化 */
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
        
        PULSE_FMT_DEBUG("端口%d更新剩余%u\n", pulseLocate + 1, ptCalu->Pulse[pulseLocate].caluleft);
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
        PULSE_FMT_DEBUG("脉冲%d 端口%d(%d%s): 功率%u, 电量%u, K %u, 剩余%u; 总电量%u, 费率%d电量%u\n", 
                        i + 1, pulseLocate + 1, powerid, powerid <= NEG_RE_Tag ? posname[powerid] : "未知", 
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
* @brief     ：脉冲计算
* @param[in] ：PULSE_OAD_T *ptCfg   OAD数据
               PULSE_CALCULATE_T *ptCalu  计算数据
               TIME_STRUCT_T *pTime  当前时间
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-19
* @Update    :
**********************************************************************/
void pulse_proc_calculate(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu, TIME_STRUCT_T *pTime)
{
    uint8  i = 0, j = 0;
    uint8  rateNO = 0;
    uint8  pulseLocate = 0;
    uint8  powerid = 0;
    uint32 metK = 5000; //电表常数 默认是5000
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
    char  *posname[NEG_RE_Tag + 1] = {"正向有功", "正向无功", "反向有功", "反向无功"};

    if(0 == ptCalu->pulseNum || ptCalu->pulseNum > OOP_MAX_PULSE)
    {
        return;
    }
    
    /* 获取当前费率 */
    rateNO = pulse_get_rateNO(&(ptCfg->tDayPeriod), pTime->hour, pTime->minute);
    PULSE_FMT_DEBUG("%02d:%02d:%02d 费率%d\n", pTime->hour, pTime->minute, pTime->second, rateNO);
    if(0 == rateNO || rateNO > OOP_MAX_RATES)
    {
        goto end;
    }

    if(0 == g_LastRate)
    {
        g_LastRate = rateNO;
    }
    
    /* 获取上一分钟的费率 防止对时导致电量统计到上一个时段内 */
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
            /* 没有配置功率写0 */
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
                /* 江苏地区使用大脉冲 */
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

            pulseLocate -= 1;   //下标从0开始
            
            /*脉冲计量点功率和电量计算*/
            power = pulse_Acal_pow_en(ptCalu->Pulse[pulseLocate].val, ptCalu->Pulse[pulseLocate].interval, metK, 
                                      &(ptCalu->Pulse[pulseLocate].caluleft), &en);
            
            /* 需量计算 虽然没有OAD存放 */
            if(g_LastRate != rateNO)
            {
                /*不在同一个费率时段,则清空需量滑差区*/
                memset(ptCalu->Pulse[pulseLocate].powerList, 0, DM_POWER_MAX * sizeof(uint32));
                ptCalu->Pulse[pulseLocate].powerNum = 0;
            }
            //powerDM = cal_pls_dm(power, &(ptCalu->Pulse[pulseLocate].powerNum), ptCalu->Pulse[pulseLocate].powerList);
            //PULSE_FMT_TRACE("%d dev index %d DM power is %u\n", i, pulseLocate + 1, powerDM);
            
            switch(powerid)
            {
                case POS_AE_Tag://正向有功
                {
                    ptMeasure->nActivePower += (int)power;
                    ptMeasure->tShowForwActEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tShowForwActEn.nValue[0] += en;
                    ptMeasure->tShowForwActEn.nValue[g_LastRate] += en;
                    showEnerTotal = ptMeasure->tShowForwActEn.nValue[0];
                    showEner = ptMeasure->tShowForwActEn.nValue[g_LastRate];
                    break;
                }
                case POS_RE_Tag://正向无功
                {
                    ptMeasure->nReactivePower += (int)power;
                    ptMeasure->tShowForwRactEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tShowForwRactEn.nValue[0] += en;
                    ptMeasure->tShowForwRactEn.nValue[g_LastRate] += en;
                    showEnerTotal = ptMeasure->tShowForwRactEn.nValue[0];
                    showEner = ptMeasure->tShowForwRactEn.nValue[g_LastRate];
                    break;
                }
                case NEG_AE_Tag://反向有功
                {
                    ptMeasure->nActivePower -= ((int)power);
                    ptMeasure->tShowBackActEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tShowBackActEn.nValue[0] += en;
                    ptMeasure->tShowBackActEn.nValue[g_LastRate] += en;
                    showEnerTotal = ptMeasure->tShowBackActEn.nValue[0];
                    showEner = ptMeasure->tShowBackActEn.nValue[g_LastRate];
                    break;
                }
                case NEG_RE_Tag://反向无功
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

            PULSE_FMT_DEBUG("脉冲%d 端口%d(%d%s): 功率%u, 电量%u, K %u, 剩余%u; 总电量%u, 费率%d电量%u\n", 
                            i + 1, pulseLocate + 1, powerid, powerid <= NEG_RE_Tag ? posname[powerid] : "未知", 
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
* @brief     ：统计节点信息
* @param[in] ：(PULSE_OAD_T *ptCfg
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-21
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
    
    /* 门节点 */
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

            PULSE_FMT_DEBUG("门节点变位%s\n", 0 == (statuscur & 0x01) ? "分" : "合");
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
* @brief     ：写入OAD
* @param[in] ：uint32 uOAD
               uint8 *pData
               uint16 dataLen
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-21
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
* @brief     ：写入OAD
* @param[in] ：uint32 uOAD
               uint16 classtag
               void  *pData
               uint16 dataLen
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-7-2
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
* @brief     ：读取OAD
* @param[in] ：uint32 uOAD
               uint8 *pBuf
               uint32 bufLen
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-22
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
* @brief     ：数据中心数据更新
* @param[in] ：uint32 uOAD           OAD
* @param[out]：PULSE_OAD_T *ptCfg    数据中心
               PULSE_CALCULATE_T *ptCalu 计算过程
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-13
* @Update    : 2020-1-13
**********************************************************************/
int pulse_update_db(uint32 uOAD, PULSE_OAD_T *ptCfg)
{
    int ret = -3;
    uint8 dev = 0;
    
    if(0xf2030300 == uOAD) //信号量设备对象数量
    {
        ret = pulse_get_OAD(0xf2030300, (uint8 *)&(ptCfg->nSwitchNUM), sizeof(uint8));
    }
    else if(0xf2030400 == uOAD) //信号量接入属性
    {
        ret = pulse_get_OAD(0xf2030400, (uint8 *)&(ptCfg->tSwitchFlag), sizeof(OOP_SWITCHIN_FLAG_T));
    }
    else if(0x24000300 == (uOAD & 0xfff0ff00)) //互感器倍率
    {
        dev = (uint8)((uOAD & 0x000f0000) >> 16);
        if(dev <= OOP_MAX_PULSE && dev > 0)
        {
            ret = pulse_get_OAD(uOAD, (uint8 *)&(ptCfg->tPulse[dev-1].tPTCT), sizeof(OOP_PTCT_T));
        }
    }
    else if(0x24000400 == (uOAD & 0xfff0ff00)) //脉冲配置
    {
        dev = (uint8)((uOAD & 0x000f0000) >> 16);
        if(dev <= OOP_MAX_PULSE && dev > 0)
        {
            ret = pulse_get_OAD(uOAD, (uint8 *)&(ptCfg->tPulse[dev-1].tPulseCfg), sizeof(OOP_PULSESETS_T));
        }
    }
    else if(0x24001C00 == (uOAD & 0xfff0ff00)) //脉冲配置
    {
        dev = (uint8)((uOAD & 0x000f0000) >> 16);
        if(dev <= OOP_MAX_PULSE && dev > 0)
        {
            ret = pulse_get_OAD(uOAD, (uint8 *)&(ptCfg->tPulse[dev-1].tBigCfg), sizeof(OOP_PULSESETS_B_T));
        }
    }
    else if(0x40160200 == uOAD) //当前套日时段表
    {
        ret = pulse_get_OAD(0x40160200, (uint8 *)&(ptCfg->tDayPeriod), sizeof(OOP_DAYPERIOD_T));
    }
    else if(0x49080200 == uOAD) //遥信防抖时间
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
* @brief     ：数据初始化后重写
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-6
* @Update    :
**********************************************************************/
void pulse_write_normal_init(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu)
{
    uint8 i;
    int   ret = 0;
    PULSE_Measure_T *ptMeasure = NULL;

    PULSE_FMT_DEBUG("Init write switch state\n");

    /* 遥信数据初始化 */
    for(i = 0; i < OOP_MAX_SWITCHIN; i++)
    {
        ptCfg->tSwitchState.state[i].cd = 0;
    }
    
    ret = pulse_write_OAD(0xf2030200, (uint8 *)&(ptCfg->tSwitchState), sizeof(OOP_SWITCHIN_STATE_T));
    if(0 != ret)
    {
        PULSE_FMT_TRACE("Init write switch state failed %d\n", ret);
    }

    /* 脉冲数据初始化 */
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
* @brief     ：数据恢复出厂设置
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-6
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
        
        //互感器倍率
        memset(&(ptMeasure->tPTCT), 0, sizeof(OOP_PTCT_T));
        pulse_get_OAD(0x24000300 + i * 0x10000, (uint8 *)&(ptMeasure->tPTCT), sizeof(OOP_PTCT_T));

        //脉冲配置
        memset(&(ptMeasure->tPulseCfg), 0, sizeof(OOP_PULSESETS_T));
        pulse_get_OAD(0x24000400 + i * 0x10000, (uint8 *)&(ptMeasure->tPulseCfg), sizeof(OOP_PULSESETS_T));

        memset(&(ptMeasure->tBigCfg), 0, sizeof(OOP_PULSESETS_B_T));
        pulse_get_OAD(0x24001C00 + i * 0x10000, (uint8 *)&(ptMeasure->tBigCfg), sizeof(OOP_PULSESETS_B_T));
    }

    /* 当前套日时段表 */
    memset(&(ptCfg->tDayPeriod), 0, sizeof(OOP_DAYPERIOD_T));
    pulse_get_OAD(0x40160200, (uint8 *)&(ptCfg->tDayPeriod), sizeof(OOP_DAYPERIOD_T));

    /* 遥信防抖时间 */
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
* @brief     ：脉冲写入存储文件中
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-4
* @Update    :
**********************************************************************/
void pulse_write_pulse(PULSE_CALCULATE_T *ptCalu)
{
    write_pdata_xram(ptCalu->datafile, (char *)&(ptCalu->tPulseTemp[0]), 4, OOP_MAX_PULSE * sizeof(PULSE_TEMP_T));
}

