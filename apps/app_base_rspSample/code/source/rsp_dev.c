/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块主线程功能
* @date：    2022-3-20
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "oopType.h"
#include "appmsg_split.h"
#include "framecomm.h"
#include "task_base.h"

#include "rsp_mqtt.h"
#include "rsp_data.h"
#include "rsp_module.h"


#define RSP_IO_NAME          "/dev/ym"
#define RSP_YM_NUM           32
//一个周期内，最多采集的脉冲个数(目前按照3分钟计算，每秒钟10个脉冲), 最多统计10min
#define MAXPLS_ONE_PERIOD    (60*10*3)
#define MAXPLS_TIME_PERIOD   10

typedef struct
{
    uint16 pls_count;                   //脉冲个数
    uint16 pls_fst_20ms;                //起始时标（20ms计数）
    uint8  pls_fst_min;                 //起始时标（分）
    uint16 pls_lst_20ms;                //结束时标（20ms计数）
    uint8  pls_lst_min;                 //结束时标（分）
}__attribute__ ((packed)) PulseInfo;    /*-从接口板召测回来的脉冲信息-*/

/**********************************************************************
* @name      : rsp_get_cur_YX
* @brief     ：获取遥信最新状态
* @param[in] ：int fd            设备
* @param[out]：uint8 *pState     遥信状态
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_get_cur_YX(int fd, uint8 *pState)
{
    uint32 data = 0;

    read(fd, &data, sizeof(data));
    *pState = (data & 0xFF);

    return 0;
}

/**********************************************************************
* @name      : rsp_get_cur_YK
* @brief     ：获取遥信最新状态
* @param[in] ：int fd            设备
* @param[out]：uint8 *pState     遥信状态
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-7-4
* @Update    :
**********************************************************************/
int rsp_get_cur_YK(int fd, uint16 *pSwitch, uint32 switchLocate)
{
    int    ret = 0;
    uint64 data = 0;
    uint8  i = 0;
    uint16 Switch = 0;
    uint8  indexswitch = 0;

    ret = read(fd, &data, sizeof(data));
    if(ret < 0)
    {
        return -1;
    }
    for(i = 0; i < 32; i++)
    {
        if((switchLocate >> i) & 0x01)
        {
            Switch |= (((data >> i) & 0x01) << indexswitch);
            indexswitch++;
        }
    }
    
    *pSwitch = Switch;

    return 0;
}


/**********************************************************************
* @name      : rsp_time_space
* @brief     ：时间间隔计算
* @param[in] ：struct timespec newtime   新时间
               struct timespec oldtime   旧时间
* @param[out]：
* @return    ：时间差 单位us
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_time_space(struct timespec newtime, struct timespec oldtime)
{
    int space_us = 0;
    int space_m = newtime.tv_sec - oldtime.tv_sec;

    if(space_m > 60 || space_m < -60)
    {
        return 60000000;
    }

    space_us = (newtime.tv_nsec - oldtime.tv_nsec)/1000;
    space_us = space_m * 1000000 + space_us;
    return space_us;
}

/**********************************************************************
* @name      : rsp_get_cur_pulse
* @brief     ：脉冲统计(至少1min执行一次)
* @param[in] ：int fd                   设备
* @param[out]：RSP_DEV_T *pDevData      数据
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-6-1
* @Update    :
**********************************************************************/
void rsp_get_cur_pulse(int fd, RSP_DEV_T *pDevData, struct  timespec curTime)
{
    int ret = -1;
    int i = 0;
    int k = 0;
    PulseInfo pulseInfo[RSP_YM_NUM];
    int timepassms = 0;
    int statpassms = 0;
    uint32 timepass = 0;

    memset(pulseInfo, 0x00, sizeof(pulseInfo));
    ret = read(fd, pulseInfo, sizeof(pulseInfo));
    if (ret != sizeof(pulseInfo))
    {
        RSP_FMT_DEBUG("读取设备%s错误\n", RSP_IO_NAME);
        return;
    }

    for(i = 0; i < RSP_YM_NUM && i < RSP_PORT_MAX; i++)
    {
        if(pulseInfo[i].pls_count > MAXPLS_ONE_PERIOD && pulseInfo[i].pls_lst_min > MAXPLS_TIME_PERIOD)
        {
            pulseInfo[i].pls_count = 0;
        }
        if(FALSE == pDevData->bShare || 1 == pDevData->config[i])
        {
            timepassms = 0;
           /* 操作系统统计的是上升延的时间，时间差是脉冲之间的时间，少了一个脉冲的时间 */
            statpassms = (int)pulseInfo[i].pls_lst_min * 60000 + (int)pulseInfo[i].pls_lst_20ms * 20 -
                         (int)pulseInfo[i].pls_fst_min * 60000 - (int)pulseInfo[i].pls_fst_20ms * 20;
            if(1 == pulseInfo[i].pls_count) // 只有一个脉冲, 统计时间是60s
            {
                timepassms = 60000;
            }
            else if(pulseInfo[i].pls_count > 1)
            {
                /* 时间修正 */
                timepassms = (double)statpassms * pulseInfo[i].pls_count / (pulseInfo[i].pls_count - 1);
            }
            RSP_FMT_DEBUG("脉冲%d: 个数%d, 开始时间(%dmin, %d * 20ms), 结束时间(%dmin, %d * 20ms), "
                          "统计时差%dms, 修正时差%dms\n", i +1, 
                          pulseInfo[i].pls_count, pulseInfo[i].pls_fst_min, pulseInfo[i].pls_fst_20ms,
                          pulseInfo[i].pls_lst_min, pulseInfo[i].pls_lst_20ms, statpassms, timepassms);

            if(pulseInfo[i].pls_count > 0 && timepassms > 0)
            {
                pDevData->tPulseAll.nCount[k] += pulseInfo[i].pls_count;
                timepass = timepassms + pDevData->tPulseAll.nTimeL[k];
                pDevData->tPulseAll.nTimeH[k] += timepass / 1000;
                pDevData->tPulseAll.nTimeL[k] = timepass % 1000;
            }
            k++;
        }
    }

    return;
}

/**********************************************************************
* @name      : rsp_static_pulse_IO
* @brief     ：通过IO口统计脉冲
* @param[in] ：struct timespec *pCurtime  当前时间
               uint8 stateYX              状态
               RSP_DATA_T *pRspData       APP进程数据
               RSP_DEV_T *pDevData        线程私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-7-12
* @Update    :
**********************************************************************/
void rsp_static_pulse_IO(uint32 curS, uint16 curMS, uint8 stateYX, RSP_DATA_T *pRspData, RSP_DEV_T *pDevData)
{
    static uint8  s_status_p = TRUE;
    static uint32 s_lasttime_s = 0;
    static uint16 s_lasttime_ms = 0;
    uint64        mspass = 0;
    long          pulsepass = 0;
    long          pulseHpass = 0;
    uint8  i = 0;
    static uint8  s_pcount = 0;
    uint8  index = 0;
    uint8  temp = 0;
    uint8  addflag[RSP_PORT_MAX] = {0};
    uint8  addpulse = FALSE;

    if(TRUE == s_status_p)
    {
        s_lasttime_s = curS;
        s_lasttime_ms = curMS;
        s_status_p = FALSE;
        
        for(i = 0; i < RSP_PORT_MAX; i++)
        {
            temp = (stateYX >> i) & 0x01;
            memset(pDevData->pulseStat[i].point, temp, 3);
        }
        return;
    }

    /* 统计整ms */
    mspass = (curS - s_lasttime_s) * 1000LL + curMS - s_lasttime_ms;
    if(0 == mspass)
    {
        return;
    }

    s_pcount++;
    /* 统计判断 */
    for(i = 0; i < RSP_PORT_MAX; i++)
    {
        if(1 == pDevData->config[i])
        {
            temp = (stateYX >> i) & 0x01;
            pDevData->pulseStat[i].point[0] = pDevData->pulseStat[i].point[1];
            pDevData->pulseStat[i].point[1] = pDevData->pulseStat[i].point[2];
            pDevData->pulseStat[i].point[2] = temp;

            /* 下降产生 */
            if(1 == pDevData->pulseStat[i].point[0] && 0 == pDevData->pulseStat[i].point[1] &&
               0 == pDevData->pulseStat[i].point[2])
            {
                pDevData->pulseStat[i].L_time_s = s_lasttime_s;
                pDevData->pulseStat[i].L_time_ms = s_lasttime_ms;
                pDevData->pulseStat[i].bL = TRUE;
                //RSP_FMT_TRACE("port %d down %d.%03ds\n", i + 1, s_lasttime_s, s_lasttime_ms);
            }
            
            /* 上升沿产生 */
            if(0 == pDevData->pulseStat[i].point[0] && 1 == pDevData->pulseStat[i].point[1] &&
               1 == pDevData->pulseStat[i].point[2])
            {
                pDevData->pulseStat[i].H_time_s = s_lasttime_s;
                pDevData->pulseStat[i].H_time_ms = s_lasttime_ms;
                pDevData->pulseStat[i].bH = TRUE;
                //RSP_FMT_TRACE("port %d up %d.%03ds\n", i + 1, s_lasttime_s, s_lasttime_ms);
            }

            /* 上升沿过10ms判断是否有脉冲 */
            if(TRUE == pDevData->pulseStat[i].bH)
            {
                pulseHpass = (curS - pDevData->pulseStat[i].H_time_s) * 1000 + curMS - pDevData->pulseStat[i].H_time_ms;
            }

            if(TRUE == pDevData->pulseStat[i].bH && TRUE == pDevData->pulseStat[i].bL && pulseHpass >= 10)
            {
                pulsepass = (pDevData->pulseStat[i].H_time_s - pDevData->pulseStat[i].L_time_s) * 1000 + 
                             pDevData->pulseStat[i].H_time_ms - pDevData->pulseStat[i].L_time_ms;
                
                if(pulsepass < 0)
                {
                    pDevData->pulseStat[i].bH = FALSE;
                }
                else
                {
                    /* 考虑现场，脉宽大于25ms即可 */
                    if(pulsepass >= RSP_PULSE_TIME_MIN - 35/* && pulsepass <= RSP_PULSE_TIME_MAX + 10*/)
                    {
                        addflag[i] = TRUE;
                        addpulse = TRUE;
                        if(FALSE == pDevData->pulseStat[i].bStart)
                        {
                            pDevData->pulseStat[i].start_s = pDevData->pulseStat[i].L_time_s;
                            pDevData->pulseStat[i].start_ms = pDevData->pulseStat[i].L_time_ms;
                            pDevData->pulseStat[i].bStart = TRUE;
                        }
                    }
                    pDevData->pulseStat[i].bH = FALSE;
                    pDevData->pulseStat[i].bL = FALSE;
                }
                if(FALSE == addflag[i])
                {
                    RSP_FMT_DEBUG("port %d pass %ld, add %d\n", i + 1, pulsepass, addflag[i]);
                }
                else
                {
                    //RSP_FMT_TRACE("port %d pass %ld, add %d\n", i + 1, pulsepass, addflag[i]);
                }
            }
        }
    }

    /* 存在计数或者统计超过100次(ms)需要更新 */
    if(TRUE == addpulse || s_pcount > 100)
    {
        PTHREAD_LOCK_SAFE(&pDevData->flock);
        for(i = 0; i < RSP_PORT_MAX; i++)
        {
            if(1 == pDevData->config[i])
            {
                if(TRUE == addflag[i])
                {
                    pDevData->tPulseAll.nCount[index] += 1;
                    //RSP_FMT_TRACE("port %d add 1, %u\n", i + 1, pDevData->tPulseAll.nCount[index]);
                }
                if(TRUE == pDevData->pulseStat[i].bStart)
                {
                    pulsepass = (curS - pDevData->pulseStat[i].start_s) * 1000 + curMS - pDevData->pulseStat[i].start_ms;
                    pDevData->tPulseAll.nTimeH[index] = pulsepass / 1000;
                    pDevData->tPulseAll.nTimeL[index] = pulsepass % 1000;
                }
                index++;
            }
        }
        PTHREAD_UNLOCK_SAFE(&pDevData->flock);
        s_pcount = 0;
    }

    s_lasttime_s = curS;
    s_lasttime_ms = curMS;
}

/**********************************************************************
* @name      : rsp_dev_init
* @brief     ：遥信设备初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_dev_init(void *pPublic, void **ppPrivate)
{
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;
    OOP_DATETIME_T   tOopTime = {0};
    uint32           i = 0;
    
    if(FALSE == pRspData->bIIC)
    {
        while(1)
        {
            sleep(1);
        }
    }

    RSP_FMT_DEBUG("IIC设备采集终端本地\n");
    while(1)
    {
        pRspData->ymfd = open(RSP_IO_NAME, O_RDONLY|O_NONBLOCK);
        if(pRspData->ymfd < 0)
        {
            sleep(1);
            continue;
        }
        break;
    }
    RSP_FMT_DEBUG("打开遥信设备\n");
    pRspData->tRspDev.bFirstRun = TRUE;
    *ppPrivate = &(pRspData->tRspDev);

    rsp_get_curtime(&tOopTime);

    pRspData->tRspDev.tPulseAll.num = pRspData->tRspDev.pulsesum;
    for(i = 0; i < pRspData->tRspDev.pulsesum; i++)
    {
        pRspData->tRspDev.tPulseAll.validFlag[i] = 1;
        memcpy(&(pRspData->tRspDev.tPulseAll.startTime[i]), &tOopTime, sizeof(OOP_DATETIME_T));
    }
    return 0;
}

/**********************************************************************
* @name      : rsp_dev_proc
* @brief     ：遥信设备检查
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_dev_proc(void *pArg, void *pPublic, void *pPrivate)
{
    struct  timespec curtime = {0};
    uint8   stateYX = 0;
    uint16  stateYK = 0;
    static uint16  stateYKs[5] = {0};
    static struct  timespec oldtime = {0};
    uint8   newstate[RSP_PORT_MAX] = {0};
    int     ret = 0;
    uint8   i = 0;
    RSP_DATA_T *pRspData = (RSP_DATA_T*)pPublic;
    RSP_DEV_T *pDevData = (RSP_DEV_T*)pPrivate;
    OOP_DATETIME_T tOopTime = {0};
    uint8   bReport = FALSE;
    RSP_REPORT_DATA_T tNode;

    /* 系统统计脉冲 */
    if(pDevData->pulsesum > 0 && RSP_PULSE_SYS == pRspData->pulseType)
    {
        /* 获取当前时间 */
        rsp_get_curtime(&tOopTime);
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        if(tOopTime.year != pDevData->gettime.year || tOopTime.month != pDevData->gettime.month ||
           tOopTime.mday != pDevData->gettime.mday || tOopTime.hour != pDevData->gettime.hour ||
           tOopTime.minute != pDevData->gettime.minute)
        {
            rsp_get_cur_pulse(pRspData->ymfd, pDevData, curtime);
            pDevData->gettime = tOopTime;
        }
    }
    
    /* 获取遥信状态 */
    ret = rsp_get_cur_YX(pRspData->ymfd, &stateYX);
    if(0 != ret)
    {
        RSP_FMT_DEBUG("获取遥信状态失败\n");
        return 1000000;
    }
    
    /* 获取当前时间 */
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    rsp_get_curtime(&tOopTime);

    /* APP统计脉冲 */
    if(pDevData->pulsesum > 0 && RSP_PULSE_IO == pRspData->pulseType)
    {
        rsp_static_pulse_IO(curtime.tv_sec, curtime.tv_nsec/1000000, stateYX, pRspData, pDevData);
    }

    for(i = 0; i < pDevData->sum && i < RSP_PORT_MAX; i++)
    {
        if((stateYX >> i) & 0x01)
        {
            newstate[i] = 1;
        }
    }

    if(TRUE == pDevData->bFirstRun)
    {
        char showbuf[1024] = {0};
        uint16 offset = 0;
        
        pDevData->bFirstRun = FALSE;
        
        for(i = 0; i < pRspData->tRspDev.pulsesum; i++)
        {
            pRspData->tRspDev.tPulseAll.startTime[i] = tOopTime;
        }
        PTHREAD_LOCK_SAFE(&pDevData->flock);
        for(i = 0; i < pDevData->sum && i < RSP_PORT_MAX; i++)
        {
            pDevData->storeState[i] = newstate[i];
            pDevData->lastState[i] = newstate[i];
            pDevData->firstChgTime[i] = curtime;
            
            if(0 == pDevData->config[i])
            {
                pDevData->chgTime[i] = tOopTime;
            }
            offset += snprintf(showbuf + offset, 1024 - offset, " 遥信%d状态 %u\n", i + 1, newstate[i]);
        }
        PTHREAD_UNLOCK_SAFE(&pDevData->flock);
        if(showbuf[0])
        {
            RSP_FMT_DEBUG("首次获取遥信状态\n%s", showbuf);

            /* 首次获取更新总状态 */
            tNode.id = 0;
            tNode.sum = pDevData->sum;
            tNode.bReport = FALSE;
            memcpy(tNode.config, pDevData->config, RSP_PORT_MAX);
            memcpy(tNode.ST, pDevData->storeState, RSP_PORT_MAX);
            memcpy(tNode.CT, pDevData->chgState, RSP_PORT_MAX);
            memcpy(tNode.chgTime, pDevData->chgTime, RSP_PORT_MAX * sizeof(OOP_DATETIME_T));

            memcpy(&pDevData->oldNode, &pDevData->lastNode, sizeof(RSP_REPORT_DATA_T));
            memcpy(&pDevData->lastNode, &pDevData->curNode, sizeof(RSP_REPORT_DATA_T));
            memcpy(&pDevData->curNode, &tNode, sizeof(RSP_REPORT_DATA_T));
            rsp_write_queue(&pRspData->reportList, &tNode);
        }
        return 200;
    }

    if(pDevData->sum > 0 && 0 != memcmp(pDevData->lastState, newstate, pDevData->sum))
    {
        /* 调试日志打印 */
        char showbuf[1024] = {0};
        uint16 offset = 0;
        for(i = 0; i < pDevData->sum && i < RSP_PORT_MAX; i++)
        {
            if(0 != pDevData->config[i])
            {
                continue;
            }
            offset += snprintf(showbuf + offset, 1024 - offset,
                              "  遥信%d[chg: %lus-%03ldms]: 存储状态 %u, 上次状态 %u, 当前状态 %u\n", 
                              i + 1, pDevData->firstChgTime[i].tv_sec, pDevData->firstChgTime[i].tv_nsec/1000000,
                              pDevData->storeState[i], pDevData->lastState[i], newstate[i]);
        }
        if(showbuf[0])
        {
            RSP_FMT_DEBUG("瞬时遥信变化[%lus-%03ldms]:\n%s", curtime.tv_sec, curtime.tv_nsec/1000000,     showbuf);
        }
    }

    /* 遥控接入状态读取 */
    if(pDevData->yksum != 0 && rsp_time_space(curtime, oldtime) >= 167000)
    {
        ret = rsp_get_cur_YK(pRspData->ymfd, &stateYK, pDevData->ykLocate);
        if(0 == ret)
        {
            oldtime = curtime;
            for(i = 1; i <= 4; i++)
            {
                stateYKs[5 - i] = stateYKs[4 - i];
            }   
            stateYKs[0] = stateYK;
            if(stateYKs[0] == stateYKs[1] && stateYKs[0] == stateYKs[2] && stateYKs[0] == stateYKs[3] && 
               stateYKs[0] == stateYKs[4])
            {
                if(pRspData->tRspDev.switchStatus != stateYK)
                {
                    RSP_FMT_DEBUG("继电器接入变化: %X\n", stateYK);
                    rsp_write_ctrlstatus(pDevData->yksum, stateYK);
                    pRspData->tRspDev.switchStatus = stateYK;
                }
            }
        }
    }
    
    /* 遥信状态检查 */
    PTHREAD_LOCK_SAFE(&pDevData->flock);
    for(i = 0; i < pDevData->sum && i < RSP_PORT_MAX; i++)
    {
        if(0 == pDevData->config[i])
        {
            if(0 == pDevData->chgTime[i].year)
            {
                /* 时间为0,表示修改配置后首次执行 */
                pDevData->storeState[i] = newstate[i];
                pDevData->lastState[i] = newstate[i];
                pDevData->firstChgTime[i] = curtime;
                pDevData->chgState[i] = 0;
            }
            else
            {
                /* 状态检查 */
                if(pDevData->storeState[i] != newstate[i])
                {
                    if(0 == pDevData->timeSpace[i])
                    {
                        /* 首次变位,刷新首次变化时间, 随便设置成非0数据保证下次跳过 */
                        pDevData->timeSpace[i] = 1;
                        pDevData->firstChgTime[i] = curtime;
                    }
                    else
                    {
                        pDevData->timeSpace[i] = rsp_time_space(curtime, pDevData->firstChgTime[i]);
                        if(pDevData->timeSpace[i]/1000 >= pDevData->shakeTime)
                        {
                            bReport = TRUE;
                            pDevData->storeState[i] = newstate[i];
                            pDevData->chgTime[i] = tOopTime;
                            pDevData->chgState[i] = 1;
                            pDevData->timeSpace[i] = 0;
                        }
                    }
                }
                else
                {
                    /* 状态相同, 检查上一个周期, 连续两个周期表示没有变位 */
                    if(pDevData->lastState[i] == newstate[i])
                    {
                        pDevData->storeState[i] = newstate[i];
                        pDevData->timeSpace[i] = 0;
                    }
                }
                pDevData->lastState[i] = newstate[i];
            }
        }
        else
        {
            memset(&(pDevData->chgTime[i]), 0, sizeof(OOP_DATETIME_T));
            pDevData->chgState[i] = 0;
        }
    }
    if(TRUE == bReport)
    {
        /* 有上报 */
        tNode.id = 0;
        tNode.sum = pDevData->sum;
        tNode.bReport = TRUE;
        memcpy(tNode.config, pDevData->config, RSP_PORT_MAX);
        memcpy(tNode.ST, pDevData->storeState, RSP_PORT_MAX);
        memcpy(tNode.CT, pDevData->chgState, RSP_PORT_MAX);
        memcpy(tNode.chgTime, pDevData->chgTime, RSP_PORT_MAX * sizeof(OOP_DATETIME_T));

        memcpy(&pDevData->oldNode, &pDevData->lastNode, sizeof(RSP_REPORT_DATA_T));
        memcpy(&pDevData->lastNode, &pDevData->curNode, sizeof(RSP_REPORT_DATA_T));
        memcpy(&pDevData->curNode, &tNode, sizeof(RSP_REPORT_DATA_T));

        memset(pDevData->chgState, 0, RSP_PORT_MAX);
    }
    PTHREAD_UNLOCK_SAFE(&pDevData->flock);

    if(TRUE == bReport)
    {
        char showbuf[1024] = {0};
        uint16 offset = 0;
        
        /* 写入上报队列 */
        rsp_write_queue(&pRspData->reportList, &tNode);

        for(i = 0; i < tNode.sum && i < RSP_PORT_MAX; i++)
        {
            offset += snprintf(showbuf + offset, 1024 - offset,
                              "遥信%d: 状态 %u, 变位 %u, 时间%04d-%02d-%02d %02d:%02d:%02d.%03d\n", 
                              i + 1, tNode.ST[i], tNode.CT[i], tNode.chgTime[i].year, tNode.chgTime[i].month,
                              tNode.chgTime[i].mday, tNode.chgTime[i].hour, tNode.chgTime[i].minute,
                              tNode.chgTime[i].second, tNode.chgTime[i].msec);
        }
        if(showbuf[0])
        {
            RSP_FMT_DEBUG("识别遥信变位: \n%s", showbuf);
        }
    }

    return 200;
}

