#ifdef AREA_SHANDONG

#include "CtsAll.h"
#include "area.h"
#include "iniparser.h"

//智能量测开关运行状态字上报事件对应表
const EventID_To_DI RunStateEventIDToDI[] = {
    //保护动作
    {2,   {0xffffffff},   0x03810202,   1,  //新增次数在协议中没找到
    {0x038e0001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
 
    //高压线路失/复电
    {3,   {0xffffffff},   0x03810203,   1,
    {0x03930001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //保护功能退出/恢复
    {4,  {0xffffffff},    0x03810202,  1,
    {0x038f0001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //闸位状态变化
    {5,  {0xffffffff},    0x03810204,  1,
    {0x03910001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //异常告警发生/恢复
    {6,  {0xffffffff},    0x03810201,  1,
    {0x03920001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //自检事件
    {7,  {0xffffffff},    0x03810205,  1,
    {0x038d0001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    {0xff,  {0xffffffff},  0xffffffff,  0xff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

};

const EventID_To_DI *GetRunStateEventIDToDIByIndex(uint8 IDIndex)
{
    int DICount = (sizeof(RunStateEventIDToDI) / sizeof(RunStateEventIDToDI[0])) - 1;       //RunStateEventIDToDI成员的数量
    int i = 0;

    for(i = 0; i < DICount; i++)
    {
        if(RunStateEventIDToDI[i].WorkStatBit == IDIndex)     //状态字对应的位信息
            return &RunStateEventIDToDI[i];
    }
    return NULL;
}

const EventID_To_DI *GetRunStateEventIDToDIByDI(uint32 DI)
{
    int DICount = (sizeof(RunStateEventIDToDI) / sizeof(RunStateEventIDToDI[0])) - 1;       //RunStateEventIDToDI成员的数量
    int i = 0;

    for(i = 0; i < DICount; i++)
    {
        if(RunStateEventIDToDI[i].DI_EventCount == DI)
            return &RunStateEventIDToDI[i];
    }
    return NULL;
}


/*******************************************************************************
* 函数名称: GetRunState_SD 
* 函数功能: 收到抄读运行状态字2的回复后，判断需要抄读的事件
* 输入参数: pipe 端口号
            buf 收到的报文
            len 报文长度
* 输出参数: 
* 返 回 值: 0 正常 -1 异常
*******************************************************************************/
static int GetRunState_SD(uint8 pipe, uint8* buf, uint16 len)
{
    AmrThreadArg_T      *pAmrThreadArg;
    uint8               mtrIndex;
    uint8               eventIndex;
    MTR_EVENT_REPORT_T  *pMtrEvent = NULL;
    uint16              reallen = 0;
    uint8               data;             //数据域
    uint8               eventStateDI[4] = {0x35, 0x38, 0x33, 0x37}; //运行状态字2的ID  0x04000502
    uint8               count;
    const EventID_To_DI *pEventMap;
    uint16              mask;
    uint8               startIndex;
    
    pAmrThreadArg = &gAmrThreadArg[pipe];
    mtrIndex = pAmrThreadArg->EventReport.CurIndex;
    pMtrEvent = &pAmrThreadArg->EventReport.MtrEvent[mtrIndex];
    eventIndex = pMtrEvent->EventCount;
    
    uint8 *prt =(uint8*)commfun_645_frame_get((char*)buf, 0x68, len, &reallen);
    if(prt == NULL)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "GetRunState_SD不是645报文 \n");
        return -1;
    }
    
    if(memcmp(&prt[10],eventStateDI,4)!=0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "不是04000502 \n");
        return -1;
    }
    
    if(prt[9] != 1 + 4) //运行状态字2就一个字节，DI占4个字节
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "数据域长度 %d  \n", prt[9]);
        return -1;
    }

    data = prt[14] - 0x33;

    /* 运行状态字2：
     * bit0-1:保留
     * bit2:保护动作标识
     * bit3:高压线路失/复电
     * bit4:保护功能退出/恢复
     * bit5:闸位变化
     * bit6:异常告警发生/恢复
     * bit7:自检事件标识
     */
    mask = 0x04;    /* 0b0000 0100 */
    startIndex = 2;
    while (mask < 0x80)
    {
        if (data & mask)
        {
            pEventMap = GetRunStateEventIDToDIByIndex(startIndex);
            if (!pEventMap)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "没有找到事件映射表\n");
                return -1;
            }
            pMtrEvent->EventInfo[eventIndex].AllDI[count++] = pEventMap->DI_EventCount;
        }
        startIndex++;
        mask <<= 1;
    }
    
    pMtrEvent->EventInfo[eventIndex].count = count;

    if (count == 0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "运行状态字2中没有事件\n");
        return -1;
    }

    pMtrEvent->state = EVENT_GET_EVENT;
    pMtrEvent->CurIndex = 0;
    eventIndex++;
    pMtrEvent->EventCount = eventIndex;

    return 0;
}

/*******************************************************************************
* 函数名称: GetRunStateEvent 
* 函数功能: 把运行状态字中需要抄读的事件以及次数添加到事件处理的全局变量中
* 输入参数: pipe 端口号
            buf 收到的报文
            len 报文长度
* 输出参数: 
* 返 回 值: 0 正常 -1 异常
*******************************************************************************/
int GetRunStateEvent(uint8 pipe, uint8* buf, uint16 len)
{
    AmrThreadArg_T      *pAmrThreadArg;
    EVENT_INFO_T        *pEventinfo = NULL;
    MTR_EVENT_REPORT_T  *mtrEvent = NULL;
    uint8               mtrIndex;
    uint8               eventIndex;
    uint32              sendDI;
    uint8               curEventCnt;    /* 当前电表事件个数 */
    uint8               addEventCnt;    /* 新增事件个数 */
    uint8               IDIndex;
    const EventID_To_DI *pEventMap;
    int                 i;
    uint8               subCnt;

    pAmrThreadArg = &gAmrThreadArg[pipe];
    mtrIndex = pAmrThreadArg->EventReport.CurIndex;
    mtrEvent = &pAmrThreadArg->EventReport.MtrEvent[mtrIndex];
    eventIndex = pAmrThreadArg->EventReport.MtrEvent[mtrIndex].CurIndex;
    pEventinfo = &(pAmrThreadArg->EventReport.MtrEvent[mtrIndex].EventInfo[eventIndex]);
    sendDI = pEventinfo->AllDI[pEventinfo->CurDIIndex];
    
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"抄读事件回复 sendDI is %d\n",sendDI);
    TASK_BUF_DEBUG( pipe, RELAY_LOG,buf, len);

    switch (sendDI)
    {
        case 0x04000502:    //运行状态字2
        {
            return GetRunState_SD(pipe, buf, len);
        }
            break;
        //case 0x03810202:  IDIndex = 2;    break;  //保护动作标识 
        case 0x03810203:    IDIndex = 3;    break;  //高压线路失/复电                
        case 0x03810202:    IDIndex = 4;    break;  //保护功能退出/恢复   
        case 0x03810204:    IDIndex = 5;    break;  //闸位状态变化
        case 0x03810201:    IDIndex = 6;    break;  //异常告警发生/恢复
        case 0x03810205:    IDIndex = 7;    break;  //自检事件标识
        default:
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"不是运行状态字2对应DI \n");
            return -1;
            break;
    }

    curEventCnt = mtrEvent->EventCount;
    addEventCnt = buf[14];  /* 68H Addr(6bytes) 68H Ctl(1byte) len(1byte) DI(4bytes) times(1byte) */

    pEventMap = GetRunStateEventIDToDIByIndex(IDIndex);
    if (!pEventMap)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "没有找到事件映射表\n");
        return -1;
    }
    
    for (i = 0; i < addEventCnt; i++)
    {
        mtrEvent->EventInfo[curEventCnt++].AllDI[subCnt++] = pEventMap->DI_Frid[0] + 0x00000001 * i;
    }
    mtrEvent->EventInfo[curEventCnt].count = subCnt;
    mtrEvent->EventCount = curEventCnt;

    return 0;  
}

/* 5分钟和1分钟的曲线任务全部改为采集当前数据 */
void Change5min1minToReal(SCHEME_TASK_RUN_INFO *info, OOP_TASK_CFG *cfg)
{
    if (info->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002 && cfg->RunCycleUnit == TASK_CYCLE_FLAG_MIN && (cfg->RunCycle == 5 || cfg->RunCycle == 1))
    {
        TASK_FMT_TRACE(-1, TASK_LOG, "unit=%u,value=%u改为采集当前数据\n", cfg->RunCycleUnit, cfg->RunCycle);
        info->AcqInfo.Acqtype = ACQTYP_NULL;
    }
}

/*******************************************************************************
* 函数名称: SendAndRecv_0038
* 函数功能: 组从节点监控发送并接收
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
uint32 SendAndRecv_0038(uint8 pipe, uint8 prtl, uint8 *addr, uint8 *sendBuf, uint16 sendLen, uint8 *recvBuf, uint16 *recvLen)
{
    uint8 payload[1024] = {0};
    uint16 index = 0;
    uint16 overtime = 30;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get(pipe);

    memcpy_r(&payload[index], (uint8 *)&pipeoad.value, 4); /* 端口号 */
    index += 4;
    payload[index++] = prtl; /* 协议类型 */
    memcpy_r(&payload[index], addr, 6); /* 载波从节点地址 */
    index += 6;
    index++; /* 通信控制字 */
    memcpy_r(&payload[index], (uint8 *)&overtime, 2); /* 帧超时时间 */
    index += 2;
    memcpy_r(&payload[index], (uint8 *)&overtime, 2); /* 字节超时时间 */
    index += 2;
    /* 报文内容 */
    if (sendLen < 0x80)
    {
        payload[index++] = sendLen;
    }
    else
    {
        payload[index++] = 0x82;
        payload[index++] = (uint8)(sendLen >> 8);
        payload[index++] = (uint8)sendLen;
    }

    memcpy(&payload[index], sendBuf, sendLen);
    index += sendLen;

    if (taskmng_msg_fun_deal(pipe, 0x0003, 0x0038, index, payload, recvLen, recvBuf) < 0)
    {
        TASK_FMT_TRACE(pipe, REC_LOG, "从节点监控返回失败\n");
        return 1;
    }

    return 0;
}

#define YEAR_MONTH_DAY      0   /* 类型年月日 */
#define HOUR_MINUTE_SECOND  1   /* 类型时分秒 */
/* 07规约解码电表时间 */
uint32 Unpack07MeterTime(uint8 *buf, uint16 len, OOP_DATETIME_S_T *time, uint32 ymdOrHms)
{
    MeterPrtlFmt_T MeterPrtlData = {0};
    int ret = taskmng_645_meter_data_unpack(len, buf, MeterPrtlData.MtrAddr, &MeterPrtlData.CtrlCode, &MeterPrtlData.DataLen, MeterPrtlData.DataBuff);

    if (ret == CHECK_RESULT_SUCCEED)
    {
        if (ymdOrHms == YEAR_MONTH_DAY) /* 年月日 */
        {
            time->year = commfun_BcdToHex(MeterPrtlData.DataBuff[7]) + 2000;
            time->month = commfun_BcdToHex(MeterPrtlData.DataBuff[6]);
            time->day = commfun_BcdToHex(MeterPrtlData.DataBuff[5]);
        }
        else /* 时分秒 */
        {
            time->hour = commfun_BcdToHex(MeterPrtlData.DataBuff[6]);
            time->minute = commfun_BcdToHex(MeterPrtlData.DataBuff[5]);
            time->second = commfun_BcdToHex(MeterPrtlData.DataBuff[4]);
        }

        return 0;
    }

    return 1;
}

/* 698规约解码电表时间 */
uint32 UnpackOopMeterTime(uint8 pipe, uint8 *buf, uint16 len, OOP_DATETIME_S_T *time)
{
    AppLayerList_t *pAppLayer = NULL;
    uint8 databuf[100] = {0};
    uint16 datalen;
    uint16 index = 0;
    int ret = LinkDataReceive(pipe, buf, len, NULL);
    TASK_FMT_DEBUG(pipe, REC_LOG, "LinkDataReceive ret %d \n", ret);

    if (ret >= eOopComm_Fail) //解帧失败直接返回
    {
        return 1;
    }
    //获取电表地址
    pAppLayer = FindAppLayer(pipe, NULL);

    if (pAppLayer == NULL)
    {
        return 1;
    }

    datalen = pAppLayer->recvApduList->apduLen - pAppLayer->recvApduList->offSet;
    memcpy(databuf, pAppLayer->recvApduList->apdu + pAppLayer->recvApduList->offSet, datalen);
    TASK_BUF_TRACE(pipe, REC_LOG, databuf, datalen);
    //databuf从oad后面的结果类型开始
    if (databuf[index++] != 1)
    {
        return 1;
    }

    if (databuf[index++] != DT_DATETIME_S)
    {
        return 1;
    }

    memcpy_r((uint8 *)&time->year, &databuf[index], 2);
    index += 2;
    time->month = databuf[index++];
    time->day = databuf[index++];
    time->hour = databuf[index++];
    time->minute = databuf[index++];
    time->second = databuf[index++];

    return 0;
}

/* 07规约获取电表时间，ymdOrHms控制获取年月日还是时分秒 */
uint32 Get07MeterTime(uint8 pipe, uint8 *addr, OOP_DATETIME_S_T *time, uint32 ymdOrHms)
{
    uint16 sendLen = 0;
    uint8 sendBuf[1000] = {0};
    uint8 recvBuf[1000] = {0};
    uint16 recvLen = 0;
    uint16 frameLen = 0;
    uint8 lenOffset = 0;
    uint32 DI = 0;
    MeterPrtlFmt_T  meter;

    memset(&meter, 0x00, sizeof(MeterPrtlFmt_T));
    memcpy(meter.MtrAddr, addr, 6);
    meter.CtrlCode = 0x11;
    meter.DataLen = 4;

    if (ymdOrHms == YEAR_MONTH_DAY)
    {
        DI = 0x04000101;
    }
    else
    {
        DI = 0x04000102;
    }

    memcpy(meter.DataBuff, &DI, 4);
    sendLen = taskmng_meter_prtl_645_pack(sendBuf, meter);

    TASK_FMT_TRACE(pipe, REC_LOG, "组帧查询电表时间 \n");
    TASK_BUF_TRACE(pipe, REC_LOG, sendBuf, sendLen);

    if (sendLen == 0)
    {
        return 1;
    }

    if (SendAndRecv_0038(pipe, PRTL_07, addr, sendBuf, sendLen, recvBuf, &recvLen) != 0)
    {
        return 1;
    }

    TASK_FMT_TRACE(pipe, REC_LOG, "查询电表时间回码 \n");
    TASK_BUF_TRACE(pipe, REC_LOG, recvBuf, recvLen);

    frameLen = get_len_offset(recvBuf, &lenOffset);

    if (frameLen == 0)
    {
        return 1;
    }

    if (Unpack07MeterTime(recvBuf + lenOffset, frameLen, time, ymdOrHms) != 0)
    {
        return 1;
    }

    return 0;
}

/* 07规约获取电表时间 */
uint32 TaskMgrGet07MeterTime(uint8 pipe, uint8 *addr, time_t *linuxTime)
{
    OOP_DATETIME_S_T oopTime = {0};

    if (Get07MeterTime(pipe, addr, &oopTime, YEAR_MONTH_DAY) != 0)
    {
        return 1;
    }

    if (Get07MeterTime(pipe, addr, &oopTime, HOUR_MINUTE_SECOND) != 0)
    {
        return 1;
    }

    DT_DateTimeShort2Time(&oopTime, linuxTime);
    TASK_FMT_TRACE(pipe, REC_LOG, "电表时间linuxTime=%ld\n", *linuxTime);

    return 0;
}

/* 698规约获取电表时间 */
uint32 TaskMgrGetOopMeterTime(uint8 pipe, uint8 *addr, time_t *linuxTime)
{
    uint16 sendLen = 0;
    uint8 sendBuf[1000] = {0};
    uint8 recvBuf[1000] = {0};
    uint16 recvLen = 0;
    uint16 frameLen = 0;
    uint8 lenOffset = 0;
    OOP_OAD_U Oad = {0x40000200};
    LinkLayer_t *pLinkLayer = NULL;
    uint8 buf[100] = {0};
    uint16 index = 0;
    OOP_TSA_T tsa;
    OOP_DATETIME_S_T oopTime = {0};

    memset(&tsa, 0x00, sizeof(OOP_TSA_T));
    tsa.len = 5;
    memcpy_r(tsa.add, addr, 6);
    memcpy(buf, (uint8 *)&Oad.value, 4);
    index += 4;
    OopGet.requst(pipe, &tsa, 1, buf, index);
    pLinkLayer = FindLinkLayer(pipe, NULL);

    if (pLinkLayer != NULL)
    {
        sendLen = pLinkLayer->sendLen;
        memcpy(sendBuf, pLinkLayer->sendBuff, sendLen);
    }

    TASK_FMT_TRACE(pipe, REC_LOG, "组帧查询电表时间 \n");
    TASK_BUF_TRACE(pipe, REC_LOG, sendBuf, sendLen);

    if (sendLen == 0)
    {
        return 1;
    }

    if (SendAndRecv_0038(pipe, PRTL_OOP, addr, sendBuf, sendLen, recvBuf, &recvLen) != 0)
    {
        return 1;
    }

    TASK_FMT_TRACE(pipe, REC_LOG, "查询电表时间回码 \n");
    TASK_BUF_TRACE(pipe, REC_LOG, recvBuf, recvLen);

    frameLen = get_len_offset(recvBuf, &lenOffset);

    if (frameLen == 0)
    {
        return 1;
    }

    if (UnpackOopMeterTime(pipe, recvBuf + lenOffset, frameLen, &oopTime) != 0)
    {
        return 1;
    }

    DT_DateTimeShort2Time(&oopTime, linuxTime);
    TASK_FMT_TRACE(pipe, REC_LOG, "电表时间linuxTime=%ld\n", *linuxTime);

    return 0;
}

/* 获取电表时间，格式是Linux系统时间，1970年以来的秒数 */
uint32 TaskMgrGetMeterTime(uint8 pipe, uint8 *addr, uint8 prtl, time_t *linuxTime)
{
    if (prtl == PRTL_07)
    {
        if (TaskMgrGet07MeterTime(pipe, addr, linuxTime) != 0)
        {
            return 1;
        }
    }
    else if (prtl == PRTL_OOP)
    {
        if (TaskMgrGetOopMeterTime(pipe, addr, linuxTime) != 0)
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }

    return 0;
}

#define TIME_TOO_SLOW_RATE  80  /* 时钟偏差百分比阈值 */
/* 判断电表时钟是否比终端时钟慢，且偏差大于等于数据采集周期80% */
uint32 JudgeMeterTimeIsTooSlow(uint8 pipe, time_t meterTime, time_t terminalTime, uint32 taskPeriod, time_t *timeOffset)
{
    long timeDiff = 0; /* 电表和终端时钟差值 */

    if (terminalTime > meterTime)
    {
        timeDiff = terminalTime - meterTime;

        TASK_FMT_TRACE(pipe, REC_LOG, "meterTime=%ld,terminalTime=%ld,timeDiff=%ld,taskPeriod=%u\n", meterTime, terminalTime, timeDiff, taskPeriod);

        if ((timeDiff * 100) / taskPeriod >= TIME_TOO_SLOW_RATE)
        {
            TASK_FMT_TRACE(pipe, REC_LOG, "电表时钟比终端时钟慢，且偏差大于等于数据采集周期80%\n");
            *timeOffset = timeDiff;
            return 0;
        }
    }
    
    return 1;
}

/* 获取电表时钟偏移量 */
uint32 TaskMgrGetTimeOffset(uint8 pipe, uint32 taskPeriod, MtrAmrInfo_T *info)
{
    time_t  meterTime = 0;
    time_t  terminalTime = 0;
    uint8   addr[6] = {0};
    uint8   prtl = 0;

    taskmng_mtr_addr_prtl_get(info->Pn, info->logicid, addr, &prtl);

    if (info->calcTimeOffsetCnt == 0 && TaskMgrGetMeterTime(pipe, addr, prtl, &meterTime) == 0)
    {
        terminalTime = time(NULL);

        if (JudgeMeterTimeIsTooSlow(pipe, meterTime, terminalTime, taskPeriod, &info->timeOffset) == 0)
        {
            info->calcTimeOffsetCnt++;
            return 0;
        }
    }

    return 1;
}

void TaskmngAdjustReadOad(SCHEME_TASK_RUN_INFO *pCbTask)
{
    OOP_OAD_U     TmpOadListInfo1[MAX_OAD_NUM];
    OOP_OAD_U     TmpOadListInfo2[MAX_OAD_NUM];
    uint16 k=0;
    uint8 n=0;
    uint8 m=0;
    memset(TmpOadListInfo1,0x00,sizeof(TmpOadListInfo1));
    memset(TmpOadListInfo2,0x00,sizeof(TmpOadListInfo2));
    if(((pCbTask->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)||(pCbTask->CsdTask.RoadTask[0].MainOad.nObjID == 0x5003))&&(pCbTask->AcqInfo.Acqtype != ACQTYP_NULL))
    {
//        for(k=0;k<pCbTask->CsdTask.RoadTask[0].SubOadNum;k++)
//        {
//            if(pCbTask->CsdTask.RoadTask[0].OadListInfo[k].value == 0x20210200)
//            {
//                TmpOadListInfo1[n++].value = 0x20210200;
//                pCbTask->IsFreezeTime =TRUE;
//            }
//        }
        for(k=0;k<pCbTask->CsdTask.RoadTask[0].SubOadNum;k++)
        {
            if(TRUE==taskmng_energy_oad_check(pCbTask->CsdTask.RoadTask[0].OadListInfo[k]))
            {
                TmpOadListInfo1[n++].value = pCbTask->CsdTask.RoadTask[0].OadListInfo[k].value;
                if(((pCbTask->CsdTask.RoadTask[0].OadListInfo[k].nObjID>>12)&0x0F)==0x00)
				{
                	pCbTask->Isenergyoad =TRUE;
				}
            }
            else
            {
                if(pCbTask->CsdTask.RoadTask[0].OadListInfo[k].value != 0x20210200)
                {
                    TmpOadListInfo2[m++].value = pCbTask->CsdTask.RoadTask[0].OadListInfo[k].value;
                }
            }
        }
        memcpy((uint8*)&pCbTask->CsdTask.RoadTask[0].OadListInfo[0],(uint8*)&TmpOadListInfo1[0],n*sizeof(OOP_OAD_U));
        
        memcpy((uint8*)&pCbTask->CsdTask.RoadTask[0].OadListInfo[n],(uint8*)&TmpOadListInfo2[0],m*sizeof(OOP_OAD_U));
        if(pCbTask->Isenergyoad ==TRUE)
        {
            pCbTask->energynum = n;
        }
    }
}
/*******************************************************************************
* 函数名称: taskmng_energy_oad_check
* 函数功能: 判断0ad是否属于电能量类
* 输入参数: oad 要抄读的数据项
* 输出参数: 
* 返 回 值: true 属于
        FALSE 不属于
*******************************************************************************/

BOOL taskmng_energy_oad_check(OOP_OAD_U oad) 
{
    uint8 OIA1;
    //冻结时标也要用冻结来抄
    if(oad.value == 0x20210200)
        return TRUE;
    if(oad.value == 0x48000400)
        return TRUE;
    OIA1=(oad.nObjID>>12)&0x0F;
    if(OIA1 == 0)
        return TRUE;
    return FALSE;
}

/*******************************************************************************
* 函数名称: taskmng_taskinfo_4800_check
* 函数功能: 检查任务是否要抄48000300或者48000400 抄谐波电压电流的时候也需要查一下 2046 2047
* 输入参数: TaskInfo 任务参数
* 输出参数: 
* 返 回 值: true 有4800
        FALSE 没有
*******************************************************************************/

BOOL taskmng_taskinfo_4800_check(TASK_INFO *TaskInfo)
{
    BOOL ret = FALSE;
    uint8 i;
    for(i = 0;i<TaskInfo->RoadTask[0].SubOadNum;i++)
    {
        if(TaskInfo->RoadTask[0].OadListInfo[i].value == 0x48000300 || TaskInfo->RoadTask[0].OadListInfo[i].value == 0x48000400
                ||TaskInfo->RoadTask[0].OadListInfo[i].nObjID == 0x2046 ||TaskInfo->RoadTask[0].OadListInfo[i].nObjID == 0x2047)
        {
            ret = TRUE;
            break;
        }
    }
    return ret;
}

/*******************************************************************************
* 函数名称: taskmng_meter_NILM_module_check
* 函数功能: 检查电表是否有非介入式负荷辨识模组 其中模组描述符：“type=NILM”，逻辑地址：3，模组类别：000AH。
            顺便检查下逻辑地址5 抄谐波电压电流用
* 输入参数: buf 电表回的数据
            buflen 长度
* 输出参数: 
* 返 回 值: 1 有模组
            2 没有模组
*******************************************************************************/

uint8 taskmng_meter_NILM_module_check(uint8 pipe,uint8 *buf,uint16 buflen,uint8* NILMflag,uint8* logic5flag)
{
    uint16 index = 0;
    uint8 num,i;
    char desc[100];
    uint8 logicaddr;
    uint8 moduletype[10];
    uint8 moduleseq[100];
    uint16 len;
	uint8 item;
    *NILMflag = 2;
    *logic5flag = 2;
    if(buf[index]!=DT_ARRAY)
    {
        return 2;
    }
    index++;
    num = buf[index++];
    TASK_FMT_TRACE(pipe,REC_LOG,"电表有%d 个模组\n",num);
    for(i=0;i<num;i++)
    {
        if(buf[index++]!=DT_STRUCTURE)
            return 2;
		item = buf[index++];
        if(item<4)
            return 2;
        if(buf[index++]!=DT_VISIBLE_STRING)
            return 2;
        len = buf[index++];
        memset(desc,0x00,sizeof(desc));
        memcpy(desc,&buf[index],len);
        index+=len;
        if(buf[index++]!=DT_UNSIGNED)
            return 2;
        logicaddr = buf[index++];
        if(buf[index++]!=DT_BIT_STRING)
            return 2;
        len = buf[index++];
        if(len%8==0)
            len /=8;
        else
            len = (len/8)+1;
        memset(moduletype,0x00,sizeof(moduletype));
        memcpy(moduletype,&buf[index],len);
        index+=len;
        if(buf[index++]!=DT_OCTET_STRING)
            return 2;
        len = buf[index++];
        memset(moduleseq,0x00,sizeof(moduleseq));
        memcpy(moduleseq,&buf[index],len);
        index+=len;
        TASK_FMT_TRACE(pipe,REC_LOG,"%d 模组信息 描述符 %s 逻辑地址 %d 模组类别 0x%02x%02x\n",i,desc,logicaddr,moduletype[0],moduletype[1]);
        //if(0==strcmp(desc,"type=NILM") && logicaddr == 3 &&moduletype[0]==0&&moduletype[1]==0x0A)
        if(logicaddr == 3)
        {
            *NILMflag = 1;
            //return 1;
        }
        if(logicaddr == 5)
        {
            *logic5flag = 1;
        }
		//老版本模组协议适配
		if(item == 5)
        {
            index+=5; //模组通信端口，oad
        }
		else if(item == 6)
        {
            index+=5; //模组通信端口，oad
            index+=5; //esam计数器，double long unsigned
        }
    }
    return 2;
}

/*******************************************************************************
* 函数名称: taskmng_pv_control_unpack
* 函数功能: 判断是否是光伏逆变器的控制命令  FE FE FE FE 01 06 0B EB 13 88 F6 8C  
透传的modbus报文：规律：8字节长度，FE是无效字符，需去掉
第一个字节是规约转换器的地址 01 将报文内容按照376.2格式的报文透传给规约转换器，地址ab ab ab ab ab 01
* 输入参数: buf 报文 
            buflen 长度
* 输出参数: 
* 返 回 值: 1 有模组
            2 没有模组
*******************************************************************************/

BOOL taskmng_pv_control_unpack(uint8 pipe,uint8* buf,uint16 buflen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 tmpbuf[100]={0};
    uint8 addr[6]={0};
    memset(addr,0xab,6);
    if(buflen!= 12)
        return FALSE;
    if(buf[0]!=0xfe || buf[1]!=0xfe || buf[2]!=0xfe || buf[3]!=0xfe)
    {
        return FALSE;
    }
    memcpy(tmpbuf,&buf[4],8);
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "山东收到光伏逆变器的控制命令\n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG, tmpbuf,8);
    pAmrThreadArg->RelayTrans.METER_LIST[0].setlen = 8;
    memcpy(pAmrThreadArg->RelayTrans.METER_LIST[0].setaction,tmpbuf,8);
    pAmrThreadArg->RelayTrans.frametype = PRTL_NULL;    //用透明传输
    addr[0] = tmpbuf[0];
    memcpy(pAmrThreadArg->RelayTrans.MtrAddr,addr,6);
    return TRUE;
}
#endif


