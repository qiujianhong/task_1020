#ifdef AREA_JIANGXI

#include "CtsAll.h"
#include "area.h"

#if DESC("功率超差事件",1)
/*******************************************************************************
* 函数名称: taskmng_event_power_out_limit_record_read 
* 函数功能: 读取数据库中的96点曲线，判断是否产生功率超差事件
* 输入参数: pipe        端口号
            logicid     电表逻辑id
            pn          电表号
            startTime   启动时标
            endTime     结束时标
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
static int taskmng_event_power_out_limit_record_read(uint8 pipe, uint8 logicid, uint16 pn, time_t startTime, time_t endTime)
{
    AmrThreadArg_T      *pAmrThreadArg;
    READ_RECORD_T       inReadRecord;
    STEP_READ_RECORD_T  stepReadRecord;
    RECORD_UNIT_T       outReadRecord;
    uint32              handle = 0;
    uint32              recNum = 0;
    uint8               tmpAddr[16];
    int                 ret = ERR_OK;
    uint8               isEvent = 1;
    int                 i;
    int                 j;

    pAmrThreadArg = &gAmrThreadArg[pipe];

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&outReadRecord, 0, sizeof(RECORD_UNIT_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));

    inReadRecord.recordOAD.road.oadMain.value = 0x50020200;
    inReadRecord.recordOAD.road.oadCols.nNum = 2;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x20140200; //电表运行状态字
    inReadRecord.recordOAD.road.oadCols.oad[1].value = 0x20040200; //有功功率
    inReadRecord.recordOAD.logicId = logicid;
    inReadRecord.recordOAD.infoNum = pn;
    inReadRecord.MAC.nNum = 6;
    taskmng_mtr_addr_get(pn, logicid, tmpAddr);
    memcpy_r(inReadRecord.MAC.value, tmpAddr, 6);
    inReadRecord.cType = 2; //采集存储时间
    inReadRecord.cStart = startTime;
    inReadRecord.cEnd = endTime + 1;


    ret = db_read_record_start(pAmrThreadArg->DBClientHandle, &inReadRecord, &handle, &recNum);
    if (0 != ret)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "db_read_record_start failed\n");
        return ret;
    }
    if(0 == recNum)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "recNum = 0\n");
        return ERR_NORESOURCE;
    }
    
    memcpy(&stepReadRecord.MAC, &inReadRecord.MAC, sizeof(OOP_OCTETVAR64_T));

    /* 96点曲线recNum=96*/
    TASK_FMT_DEBUG(pipe, REC_LOG, "recNum is %d\n", recNum);
    if (recNum != 96)
    {
        recNum = 0;     /* 不需要下面的for循环了，直接结束读记录返回 */
        isEvent = 0;
    }

    for ( ; recNum > 0; recNum--)
    {
        ret = db_read_record_step(pAmrThreadArg->DBClientHandle, handle, &stepReadRecord, &outReadRecord);
        if (0 != ret)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "db_read_record_step failed\n");
            return ret;
        }

        TASK_FMT_DEBUG(pipe, REC_LOG, "outRecordRecord.colStartTm = %d, outRecordRecord.colEndTm = %d, outRecordRecord.colStoreTm = %d\n", outReadRecord.colStartTm, outReadRecord.colEndTm, outReadRecord.colStoreTm);
        /* outReadRecord.recordnum = 2, 分别为电表运行状态字和有功功率 */
        TASK_FMT_DEBUG(pipe, REC_LOG, "outRecordRecord.recordnum = %d\n", outReadRecord.recordnum);
        if (outReadRecord.recordnum != 2)
        {
            isEvent = 0;
            break;
        }
        
        for (i = 0; i < outReadRecord.recordnum; i++)
        {
            TASK_BUF_DEBUG(pipe, REC_LOG, outReadRecord.record[i].outDataBuf, outReadRecord.record[i].outDatalen);
            for (j = 0; j < outReadRecord.record[i].road.oadCols.nNum; j++)
                TASK_FMT_DEBUG(pipe, REC_LOG, "outRecordRecord.record[%d].road.oadCols.oad[%d].value is %d\n", i, j, outReadRecord.record[i].road.oadCols.oad[j].value);
        }

        OOP_METWORD_T   metWord;
        OOP_INT4V_T     power;

        /* 判断是否逆相序 */
        memcpy(&metWord, outReadRecord.record[0].outDataBuf, sizeof(OOP_METWORD_T));
        if (!(metWord.metWord[6] & 0x03)) //电表运行状态字7        第0位:电压逆相序 第1位:电流逆相序
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "one of 96 point reverse phase normal\n");
            isEvent = 0;
            break;
        }

        /* 判断功率是否超限 */
        memcpy(&power, outReadRecord.record[1].outDataBuf, sizeof(OOP_INT4V_T));
        if ((power.nValue[1] <= 2000) || 
            (power.nValue[2] <= 2000) || 
            (power.nValue[3] <= 2000) ||
            (abs((power.nValue[1] + power.nValue[2] + power.nValue[3]) - power.nValue[0]) * 10 <= power.nValue[0]))
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "one of 96 point power normal\n");
            isEvent = 0;
            break;
        }
    }

    /*结束读记录*/
    ret = db_read_record_end(pAmrThreadArg->DBClientHandle, handle);
    if (0 != ret)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "db_read_record_end failed\n");
        return ret;
    }

    if (!isEvent)
    {
        return -1;
    }

    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_event_power_out_limit_frame_make 
* 函数功能: 模拟抄回的事件报文
* 输入参数: realTime        事件发生时标
* 输出参数: recvBuf         模拟抄回的具体事件报文
* 返 回 值: 
*******************************************************************************/
static uint16 taskmng_event_power_out_limit_frame_make(uint8* recvBuf, time_t realTime)
{
    uint8           index = 0;
    uint32          value;
    static uint32   eventNo = 1;

    /* RCSD len (事件记录序号, 事件发生时间, 事件结束时间) */
    recvBuf[index++] = 3;

    /* 事件记录序号OAD */
    recvBuf[index++] = 0;
    value = 0x20220200;
    memcpy_r(&recvBuf[index], &value, sizeof(value));
    index += sizeof(value);

    /* 事件发生时间OAD */
    recvBuf[index++] = 0;
    value = 0x201e0200;
    memcpy_r(&recvBuf[index], &value, sizeof(value));
    index += sizeof(value);

    /* 事件结束时间OAD */
    recvBuf[index++] = 0;
    value = 0x20200200;
    memcpy_r(&recvBuf[index], &value, sizeof(value));
    index += sizeof(value);

    /* 结果类型 */
    recvBuf[index++] = 1;

    /* 长度 */
    recvBuf[index++] = 1;

    /* 事件记录序号 */
    recvBuf[index++] = 0x06;
    memcpy_r(&recvBuf[index], &eventNo, sizeof(eventNo));
    index += sizeof(eventNo);
    eventNo++;

    /* 事件发生时间 */
    recvBuf[index++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(realTime,(DateTimeHex_t *)&recvBuf[index]);
    index += 7;

    /* 事件结束时间 */
    recvBuf[index++] = 0;

    /* 无跟随上报信息域 */
    recvBuf[index++] = 0;

    /* 无时间标签 */
    recvBuf[index++] = 0;

    return index;
}

/*******************************************************************************
* 函数名称: taskmng_event_power_out_limit 
* 函数功能: 功率超差事件判断处理入口函数
* 输入参数: pipe        端口号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_event_power_out_limit(uint8 pipe)
{
    AmrThreadArg_T  *pAmrThreadArg;
    uint16          mtrIndex = 0;
    time_t          realTime;
    struct tm       tmTime;
    static uint8    eventFlag = 0;
    int             ret;
    uint8           logicid;
    uint16          pn;
    time_t          startTime;
    time_t          endTime;

    pAmrThreadArg = &gAmrThreadArg[pipe];
    realTime = time(NULL);
    LOCALTIME_R(&realTime, &tmTime);

    /* 由于事件上报时间要求：凌晨4-5点，所以在凌晨4点进行判断 */
    if (eventFlag && tmTime.tm_hour == 4 && tmTime.tm_min == 0 && tmTime.tm_sec >= 0)  /* 4点整事件已经判断 */
    {
        return;
    }
    else if (tmTime.tm_hour == 4 && tmTime.tm_min == 0 && tmTime.tm_sec >= 0)
    {
        eventFlag = 1;
        for (; mtrIndex < MAX_AMR_COLLECT_CNT; mtrIndex++)
        {
            if (pAmrThreadArg->pMtrAmrInfo[mtrIndex].reversePhase)
            {
                logicid = pAmrThreadArg->pMtrAmrInfo[mtrIndex].logicid;
                pn = pAmrThreadArg->pMtrAmrInfo[mtrIndex].Pn;
                
                /* 计算上日零点格林威治时间 */
                tmTime.tm_hour = 0;
                tmTime.tm_min = 0;
                tmTime.tm_sec = 0;
                startTime = mktime(&tmTime);
                startTime -= 24 * 60 * 60;

                /* 计算今日零点格林威治时间 */
                endTime = startTime + 24 * 60 * 60;
                
                /* 读上日96点的电表运行状态字0x20140200和有功功率0x20040200
                 * 首先判断96点运行运行状态字是否都有逆向序 
                 * 再判断有功功率是否超差
                 */
                ret = taskmng_event_power_out_limit_record_read(pipe, logicid, pn, startTime, endTime);
                
                /* 形成事件写入上报队列 */
                if (ret == 0)
                {
                    uint8       recvBuf[128] = {0};
                    uint16      bufLen;
                    OOP_OAD_U   mainOad;

                    mainOad.value = 0x304D0200;
                    bufLen = taskmng_event_power_out_limit_frame_make(recvBuf, realTime);
                    Save698EventData(pipe, recvBuf, bufLen, pn, mainOad, realTime);
                }

                /* 恢复逆向序标志 */
                pAmrThreadArg->pMtrAmrInfo[mtrIndex].reversePhase = 0;
            }
        }
    }
    else
    {
        eventFlag = 0;
    }
}

#endif

#if DESC("零火线不平衡事件",1)
/*******************************************************************************
* 函数名称: taskmng_event_zero_wire_current_record_read 
* 函数功能: 读取数据库中最近的电流和零线电流，判断是否产生零火线不平衡事件
* 输入参数: pipe                端口号
            logicid         电表逻辑id
            pn              电表号

* 输出参数: pwireCurrent        电流
            pzeroCurrent    零线电流
* 返 回 值: 
*******************************************************************************/
static int taskmng_event_zero_wire_current_record_read(uint8 pipe, uint8 logicid, uint16 pn, OOP_INT3V_T *pwireCurrent, int32 *pzeroCurrent)
{
    AmrThreadArg_T      *pAmrThreadArg;
    READ_RECORD_T       inReadRecord;
    RECORD_UNIT_T       outReadRecord;
    uint8               tmpAddr[16];
    OOP_METER_BASIC_T   meterBasicInfo;
    int32               zeroCurrent;
    OOP_INT3V_T         wireCurrent;
    int                 ret = ERR_OK;

    pAmrThreadArg = &gAmrThreadArg[pipe];

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&outReadRecord, 0, sizeof(RECORD_UNIT_T));
    memset(&meterBasicInfo, 0, sizeof(OOP_METER_BASIC_T));

    inReadRecord.recordOAD.road.oadMain.value = 0x50020200;
    inReadRecord.recordOAD.road.oadCols.nNum = 2;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x20010200; //电流
    inReadRecord.recordOAD.road.oadCols.oad[1].value = 0x20010400; //零线电流
    inReadRecord.recordOAD.logicId = logicid;
    inReadRecord.recordOAD.infoNum = pn;
    inReadRecord.MAC.nNum = 6;
    taskmng_mtr_addr_get(pn, logicid, tmpAddr);
    memcpy_r(inReadRecord.MAC.value, tmpAddr, 6);
    inReadRecord.cType = 2; //采集存储时间
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = time(NULL)+1;

    ret = db_read_record(pAmrThreadArg->DBClientHandle,&inReadRecord,DATA_LEN_MAX,&outReadRecord);
    if (ret != ERR_OK)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "db_read_record failed\n");
        return -1;
    }

    TASK_FMT_DEBUG(pipe, REC_LOG, "outReadRecord.recordnum = %d\n", outReadRecord.recordnum);
    if (outReadRecord.recordnum != 2)
    {
        return -1;
    }

    TASK_BUF_DEBUG(pipe, REC_LOG, outReadRecord.record[0].outDataBuf, outReadRecord.record[0].outDatalen);
    TASK_BUF_DEBUG(pipe, REC_LOG, outReadRecord.record[1].outDataBuf, outReadRecord.record[1].outDatalen);
    
    memcpy(&wireCurrent, outReadRecord.record[0].outDataBuf, sizeof(wireCurrent));  /* 电流 */
    memcpy(&zeroCurrent, outReadRecord.record[1].outDataBuf, sizeof(zeroCurrent));  /* 零线电流 */

    memcpy(pwireCurrent, outReadRecord.record[0].outDataBuf, sizeof(OOP_INT3V_T));  /* 电流 */
    memcpy(pzeroCurrent, outReadRecord.record[1].outDataBuf, sizeof(int32));        /* 零线电流 */

    taskmng_mtr_basic_info_get(pn, logicid, &meterBasicInfo);

    /* 再次判断零线电流是否大于0.5A */
    if (zeroCurrent <= 500)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "zeroCurrent <= 0.5A\n");
        return -1;
    }
    
    if (meterBasicInfo.pwrType == 1) /* 单相 */
    {
        
        if ((abs(zeroCurrent - wireCurrent.nValue[0]) * 10) > zeroCurrent)
        {
            return 0;
        }
    }
    else if (meterBasicInfo.pwrType == 3) /* 三相四线 */
    {
        if ((abs(zeroCurrent - wireCurrent.nValue[0]) * 10) > zeroCurrent ||
            (abs(zeroCurrent - wireCurrent.nValue[1]) * 10) > zeroCurrent ||
            (abs(zeroCurrent - wireCurrent.nValue[2]) * 10) > zeroCurrent)
        {
            return 0;
        }
    }
    else
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "meterBasicInfo.pwrType != 1 && meterBasicInfo.pwrType != 3\n");
    }
    
    return -1;
}

/*******************************************************************************
* 函数名称: taskmng_event_zero_wire_current_frame_make 
* 函数功能: 模拟抄回的事件报文
* 输入参数: realTime            事件发生时标
            pwireCurrent    电流
            pzeroCurrent    零线电流
            
* 输出参数: recvBuf             模拟抄回的具体事件报文
* 返 回 值: 
*******************************************************************************/
static uint16 taskmng_event_zero_wire_current_frame_make(uint8* recvBuf, time_t realTime, OOP_INT3V_T *pwireCurrent, int32 *pzeroCurrent)
{
    uint8           index = 0;
    uint32          value;
    static uint32   eventNo = 1;

    /* RCSD len (事件记录序号, 事件发生时间, 事件结束时间, 零线电流, 火线电流) */
    recvBuf[index++] = 5;

    /* 事件记录序号OAD */
    recvBuf[index++] = 0;
    value = 0x20220200;
    memcpy_r(&recvBuf[index], &value, sizeof(value));
    index += sizeof(value);

    /* 事件发生时间OAD */
    recvBuf[index++] = 0;
    value = 0x201e0200;
    memcpy_r(&recvBuf[index], &value, sizeof(value));
    index += sizeof(value);

    /* 事件结束时间OAD */
    recvBuf[index++] = 0;
    value = 0x20200200;
    memcpy_r(&recvBuf[index], &value, sizeof(value));
    index += sizeof(value);

    /* 零线电流OAD */
    recvBuf[index++] = 0;
    value = 0x20010400;
    memcpy_r(&recvBuf[index], &value, sizeof(value));
    index += sizeof(value);

    /* 电流OAD */
    recvBuf[index++] = 0;
    value = 0x20010200;
    memcpy_r(&recvBuf[index], &value, sizeof(value));
    index += sizeof(value);

    /* 结果类型 */
    recvBuf[index++] = 1;

    /* 长度 */
    recvBuf[index++] = 1;

    /* 事件记录序号 */
    recvBuf[index++] = 0x06;
    memcpy_r(&recvBuf[index], &eventNo, sizeof(eventNo));
    index += sizeof(eventNo);
    eventNo++;

    /* 事件发生时间 */
    recvBuf[index++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(realTime,(DateTimeHex_t *)&recvBuf[index]);
    index += 7;

    /* 事件结束时间 */
    recvBuf[index++] = 0;

    /* 零线电流 */
    recvBuf[index++] = DT_DOUBLE_LONG;
    memcpy_r(&recvBuf[index], pzeroCurrent, sizeof(int32));
    index += sizeof(int32);

    /* 电流 */
    recvBuf[index++] = DT_ARRAY;
    recvBuf[index++] = 0x03;
    recvBuf[index++] = DT_DOUBLE_LONG;
    memcpy_r(&recvBuf[index], &(pwireCurrent->nValue[0]), sizeof(int32));
    index += sizeof(int32);   
    recvBuf[index++] = DT_DOUBLE_LONG;
    memcpy_r(&recvBuf[index], &(pwireCurrent->nValue[1]), sizeof(int32));
    index += sizeof(int32); 
    recvBuf[index++] = DT_DOUBLE_LONG;
    memcpy_r(&recvBuf[index], &(pwireCurrent->nValue[2]), sizeof(int32));
    index += sizeof(int32);

    /* 无跟随上报信息域 */
    recvBuf[index++] = 0;

    /* 无时间标签 */
    recvBuf[index++] = 0;

    return index;
}

/*******************************************************************************
* 函数名称: taskmng_event_zero_wire_current 
* 函数功能: 零火线不平衡事件判断处理入口函数
* 输入参数: pipe        端口号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_event_zero_wire_current(uint8 pipe, uint16 mtrIndex)
{
    AmrThreadArg_T  *pAmrThreadArg;
    int             zeroCurrentRet;
    uint8           jxLogicid;
    uint16          jxPn;
    int32           zeroCurrent;
    OOP_INT3V_T     wireCurrent;
    uint8           jxRecvBuf[128] = {0};
    uint16          jxBufLen;
    OOP_OAD_U       jxMainOad;
    time_t          jxRealTime;

    pAmrThreadArg = &gAmrThreadArg[pipe];

    if (pAmrThreadArg->pMtrAmrInfo[mtrIndex].zeroCurrentFlag)
    {
        
        

        jxLogicid = pAmrThreadArg->pMtrAmrInfo[mtrIndex].logicid;
        jxPn = pAmrThreadArg->pMtrAmrInfo[mtrIndex].Pn;
        
        zeroCurrentRet = taskmng_event_zero_wire_current_record_read(pipe, jxLogicid, jxPn, &wireCurrent, &zeroCurrent);
        zeroCurrentRet = 0;
        
        if (zeroCurrentRet == 0)
            pAmrThreadArg->pMtrAmrInfo[mtrIndex].zeroCurrentAbnormal++;
        
        /* 合计异常点数大于6个点上报事件 */
        if(pAmrThreadArg->pMtrAmrInfo[mtrIndex].zeroCurrentAbnormal > 6)
        {
            jxRealTime = time(NULL);
            jxMainOad.value = 0x304c0200;
            
            jxBufLen = taskmng_event_zero_wire_current_frame_make(jxRecvBuf, jxRealTime, &wireCurrent, &zeroCurrent);
            Save698EventData(pipe, jxRecvBuf, jxBufLen, jxPn, jxMainOad, jxRealTime);
            
            pAmrThreadArg->pMtrAmrInfo[mtrIndex].zeroCurrentAbnormal = 0;
        }
        pAmrThreadArg->pMtrAmrInfo[mtrIndex].zeroCurrentFlag = 0;
    }
}

#endif

#if DESC("台区停上电",1)
extern int GetMsindexFromAddr(uint8 pipe,uint8 *Addr);

/*******************************************************************************
* 函数名称: taskmng_power_on_off_deal_tmn 
* 函数功能: 收到终端停上电事件的数据更新后，通过读数据库判断是终端停电还是上电
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_power_on_off_deal_tmn(AmrThreadArg_T* pAmrThreadArg, uint8* buf, uint16 len, MTR_POWER_ON_OFF_JX* pMtrPowerOnOff)
{
    READ_RECORD_T       readRecordIn;
    RECORD_UNIT_T       readRecordOut;
    DB_UPDATE_T         dbUpdate;
    int                 ret;
    OOP_POWERLOG_T      powerEventUnit;
    uint8               commPipe;

    memcpy(&dbUpdate, buf, len);
    commPipe = pAmrThreadArg->CommPipe;

    /* 读取终端地址 */
    NOMAL_OAD_T nomalOAD;
    uint32      length = 0;
    OOP_OCTETVAR16_T tLocalAddr;
    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
    nomalOAD.logicId = 0;
    nomalOAD.infoNum = 0;
    nomalOAD.oad.value = 0x40010200;
    ret = db_read_nomal(pAmrThreadArg->DBClientHandle, &nomalOAD, sizeof(tLocalAddr), (uint8*)&tLocalAddr, &length);
    TASK_FMT_DEBUG(commPipe, RELAY_LOG, "db_read_nomal return %d\n", ret);

    /* 读取终端停上电事件 */
    readRecordIn.recordOAD.optional = 0;
    readRecordIn.recordOAD.classtag = CLASS_DATA_EVENT;
    readRecordIn.recordOAD.road.oadMain.value = 0x00000000;
    readRecordIn.recordOAD.road.oadCols.nNum = 1;
    readRecordIn.recordOAD.road.oadCols.oad[0].value= 0x31060200;
    readRecordIn.recordOAD.logicId = dbUpdate.logicid;
    readRecordIn.recordOAD.infoNum = dbUpdate.infonum;
    readRecordIn.cType = COL_TM_STORE;
    readRecordIn.cStart = 0;
    readRecordIn.cEnd = time(NULL)+1;
    readRecordIn.sortType = 1;
    readRecordIn.MAC.nNum = 6;
    memcpy(readRecordIn.MAC.value, tLocalAddr.value, 6);

    ret = db_read_record(pAmrThreadArg->DBClientHandle, &readRecordIn, DATA_LEN_MAX, &readRecordOut);
    TASK_FMT_DEBUG(commPipe, RELAY_LOG, "db_read_record return %d\n", ret);
    TASK_BUF_DEBUG(commPipe, RELAY_LOG, readRecordOut.record[0].outDataBuf, readRecordOut.record[0].outDatalen);
    memcpy(&powerEventUnit, readRecordOut.record[0].outDataBuf, sizeof(OOP_POWERLOG_T));
    if (powerEventUnit.source.choice == DT_ENUM)
    {
        if (powerEventUnit.source.nValue == 0)  //停电
        {
            pMtrPowerOnOff->tmnPowerOnOffFlag       = TMN_POWER_CHANGE_JX;
            pMtrPowerOnOff->tmnPowerOff.flag        = TMN_POWER_OFF_JX;
            pMtrPowerOnOff->tmnPowerOff.happenTime  = time(NULL);
        }
        else if (powerEventUnit.source.nValue == 1)  //上电
        {
            pMtrPowerOnOff->tmnPowerOnOffFlag       = TMN_POWER_CHANGE_JX;
            pMtrPowerOnOff->tmnPowerOn.flag         = TMN_POWER_ON_JX;
            pMtrPowerOnOff->tmnPowerOn.happenTime   = time(NULL);
        }

        memcpy(&(pMtrPowerOnOff->tmStart), &powerEventUnit.tmStart, sizeof(OOP_DATETIME_S_T));
        memcpy(&(pMtrPowerOnOff->tmEnd), &powerEventUnit.tmEnd, sizeof(OOP_DATETIME_S_T));
    }
}

/*******************************************************************************
* 函数名称: MakePowerOnOffFrameAllZone
* 函数功能: 组全台区停上电报文
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
static uint16 MakePowerOnOffFrameAllZone(uint8 *buff, uint16 start, uint32 eventSeq, MTR_POWER_ON_OFF_JX* pMtrPowerOnOff)
{
    uint16 index = start;
    
    buff[index++] = 1;  //结果类型
    buff[index++] = 1;
    buff[index++] = DT_NULL;

    buff[index++] = DT_DATETIME_S;
    memcpy_r(&buff[index], &pMtrPowerOnOff->tmStart.year, sizeof(uint16));
    index += 2;
    memcpy(&buff[index], &pMtrPowerOnOff->tmStart.month, 5);
    index += 5;

    buff[index++] = DT_DATETIME_S;
    memcpy_r(&buff[index], &pMtrPowerOnOff->tmStart.year, sizeof(uint16));
    index += 2;
    memcpy(&buff[index], &pMtrPowerOnOff->tmStart.month, 5);
    index += 5;

    buff[index++] = DT_DATETIME_S;
    memcpy_r(&buff[index], &pMtrPowerOnOff->tmStart.year, sizeof(uint16));
    index += 2;
    memcpy(&buff[index], &pMtrPowerOnOff->tmStart.month, 5);
    index += 5;

    buff[index++] = 1;
    buff[index++] = 3;

    buff[index++] = DT_DATETIME_S;
    memcpy_r(&buff[index], &pMtrPowerOnOff->tmStart.year, sizeof(uint16));
    index += 2;
    memcpy(&buff[index], &pMtrPowerOnOff->tmStart.month, 5);
    index += 5;

    if (pMtrPowerOnOff->tmnPowerOn.flag)    // 上电
    {
        buff[index++] = DT_DATETIME_S;
        memcpy_r(&buff[index], &pMtrPowerOnOff->tmEnd.year, sizeof(uint16));
        index += 2;
        memcpy(&buff[index], &pMtrPowerOnOff->tmEnd.month, 5);
        index += 5;
    }
    else
    {
        buff[index++] = DT_NULL;
    }

    buff[index++] = DT_DOUBLE_LONG_UNSIGNED;
    memcpy_r(&buff[index],(uint8*)&eventSeq,4);
    index += 4;

    return index;
}

/*******************************************************************************
* 函数名称: SaveEventPowerOnOff_Jx
* 函数功能://存储停上电事件 
* 输入参数:EventType 1 停电 0上电
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
static void SaveEventPowerOnOff_Jx(uint8 pipe, uint8* Addr, uint8 EventType, MTR_POWER_ON_OFF_JX* pMtrPowerOnOff)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int msindex = -1;
    time_t powerofftime;
    time_t timenow = time(NULL);
    uint32 Eventseq = 0;
    uint8  index;

    uint8 rptBuff[2000] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00,
        0x01, 0x30, 0x11, 0x02, 0x00, 0x03, 0x20, 0x1E, 0x02, 0x00, 0x20, 0x20, 0x02, 0x00,
        0x20, 0x22, 0x02, 0x00,};
    uint16 rptLen = 43;

    if (pMtrPowerOnOff->tmnPowerOnOffFlag == TMN_POWER_CHANGE_JX)
    {
        rptBuff[rptLen++] = 1;  //结果类型
        rptBuff[rptLen++] = 1;  //
        rptBuff[rptLen++] = DT_TSA;
        rptBuff[rptLen++] = 7;
        rptBuff[rptLen++] = 5;
        memcpy_r(&rptBuff[rptLen],Addr,6);
        rptLen+=6;
        msindex = GetMsindexFromAddr(pipe,Addr);
        if(msindex==-1)
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "该电表不在档案中 不进行存储  \n");
            return;
        }
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;

        rptBuff[rptLen++] = 1;
        rptBuff[rptLen++] = 3;
        if(EventType==1)
        {
            rptBuff[rptLen++] = DT_DATETIME_S;
            DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
            rptLen+=7;
            //将停电时间写入内存
            if(pAmrThreadArg->pMtrAmrInfo[msindex].powerstate == 1)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "该电表重复上报停电事件 直接过滤 \n");
                return;
            }
            pAmrThreadArg->pMtrAmrInfo[msindex].powerofftime = timenow;
            
            pAmrThreadArg->pMtrAmrInfo[msindex].powerstate = 1;
            Eventseq = pAmrThreadArg->pMtrAmrInfo[msindex].powereventseq;
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "停电事件序号 %d \n",Eventseq);
            //停电 事件结束时间为NULL
            rptBuff[rptLen++] = DT_NULL;
        }else
        {
            //终端停电，电表停电的信息也记录不了，在这里不判powerstate
            
            powerofftime = pAmrThreadArg->pMtrAmrInfo[msindex].powerofftime;
            Eventseq = pAmrThreadArg->pMtrAmrInfo[msindex].powereventseq;
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "上电事件序号 %d \n",Eventseq);
            pAmrThreadArg->pMtrAmrInfo[msindex].powerstate = 0;
            pAmrThreadArg->pMtrAmrInfo[msindex].powereventseq++;
            //电表上电事件的“发生时间”应为电表的停电时间，如果终端没有收到该电表的停电事件，则“发生时间”应为空
            if(powerofftime==0)
            {
                rptBuff[rptLen++] = DT_NULL;
            }else
            {

                rptBuff[rptLen++] = DT_DATETIME_S;
                DT_Time2DateTimeHex(powerofftime,(DateTimeHex_t *)&rptBuff[rptLen]);
                rptLen+=7;
            }
            //上电 事件结束时间有效
            rptBuff[rptLen++] = DT_DATETIME_S;
            DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
            rptLen+=7;
        }
        rptBuff[rptLen++] = DT_DOUBLE_LONG_UNSIGNED;
        memcpy_r(&rptBuff[rptLen],(uint8*)&Eventseq,4);
        rptLen+=4;

        if ((pMtrPowerOnOff->tmnPowerOn.flag == TMN_POWER_ON_JX)  && (EventType == 2)) /* 事件类型 1停电 2复电 */
        {
            index = pMtrPowerOnOff->tmnPowerOn.happenCnt;
            pMtrPowerOnOff->tmnPowerOn.eventMsg[index].len = rptLen;
            memcpy(pMtrPowerOnOff->tmnPowerOn.eventMsg[index].buf, rptBuff, rptLen);
            pMtrPowerOnOff->tmnPowerOn.happenCnt++;

            /* 终端上电且在3分钟内接收到5块表的上电事件，报台区上电事件 */
            if (pMtrPowerOnOff->tmnPowerOn.happenCnt >= 5)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "全台区上电\n");
                rptLen = MakePowerOnOffFrameAllZone(rptBuff, 43, Eventseq, pMtrPowerOnOff);
                taskmng_event_report_data_write(pipe,rptBuff,rptLen,PowerOnOffOad);
                pMtrPowerOnOff->tmnPowerOn.flag = TMN_POWER_NORMAL_JX;
                pMtrPowerOnOff->tmnPowerOn.happenCnt = 0;
                //memset(pMtrPowerOnOff, 0x00, sizeof(MTR_POWER_ON_OFF_JX));
            }
        }
        
        if ((pMtrPowerOnOff->tmnPowerOff.flag == TMN_POWER_OFF_JX) && (EventType == 1))
        {
            index = pMtrPowerOnOff->tmnPowerOff.happenCnt;
            pMtrPowerOnOff->tmnPowerOff.eventMsg[index].len = rptLen;
            memcpy(pMtrPowerOnOff->tmnPowerOff.eventMsg[index].buf, rptBuff, rptLen);
            pMtrPowerOnOff->tmnPowerOff.happenCnt++;

             /* 终端停电且在1分钟内接收到5块表的停电事件，报台区停电事件 */
            if (pMtrPowerOnOff->tmnPowerOff.happenCnt >= 5)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "全台区停电\n");
                rptLen = MakePowerOnOffFrameAllZone(rptBuff, 43, Eventseq, pMtrPowerOnOff);
                taskmng_event_report_data_write(pipe,rptBuff,rptLen,PowerOnOffOad);
                pMtrPowerOnOff->tmnPowerOff.flag = TMN_POWER_NORMAL_JX;
                pMtrPowerOnOff->tmnPowerOff.happenCnt = 0;
                //memset(pMtrPowerOnOff, 0x00, sizeof(MTR_POWER_ON_OFF_JX));
            }
        }
    }

    //taskmng_event_report_data_write(pipe,rptBuff,rptLen,PowerOnOffOad);

}

/*******************************************************************************
* 函数名称: taskmng_power_on_off_deal_met_jx
* 函数功能: 收到电表停上电的处理
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_power_on_off_deal_met_jx(uint8 pipe,uint8* buf,uint16 len, MTR_POWER_ON_OFF_JX* pMtrPowerOnOff)
{
    uint16 index=0;
    uint8 Addr[6]={0};
    uint8 EventType;    //事件类型 1停电 2复电

    EventType = buf[index++];
    if(EventType!=1 &&EventType !=2)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "停上电事件类型 %d 不对！  \n",EventType);
        return;
    }
    while(index+6 <= len)
    {
        memcpy(Addr,&buf[index],6);
        index+=6;
        SaveEventPowerOnOff_Jx( pipe, Addr, EventType, pMtrPowerOnOff);
    }
}

/*******************************************************************************
* 函数名称: PowerOnOffEventReport_Jx
* 函数功能: 终端停上电时电表停上电事件的上报
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void PowerOnOffEventReport_Jx(uint8 pipe, MTR_POWER_ON_OFF_JX* pMtrPowerOnOff)
{
    time_t  tm;
    int     index;
    uint8*  rptBuf;
    uint16  rptLen;
    
    if (pMtrPowerOnOff->tmnPowerOnOffFlag == TMN_POWER_CHANGE_JX)
    {
        tm = time(NULL);
        /* 终端上电，5分钟内未收到5块电表的上电事件，把存储的电表事件上报 */
        if (abs(tm - (pMtrPowerOnOff->tmnPowerOn.happenTime)) >= 5 * 60)
        {
            for (index = 0; index < pMtrPowerOnOff->tmnPowerOn.happenCnt; index++)
            {
                rptBuf = pMtrPowerOnOff->tmnPowerOn.eventMsg[index].buf;
                rptLen = pMtrPowerOnOff->tmnPowerOn.eventMsg[index].len;
                taskmng_event_report_data_write(pipe, rptBuf, rptLen,PowerOnOffOad);
            }
            pMtrPowerOnOff->tmnPowerOn.happenCnt = 0;
            pMtrPowerOnOff->tmnPowerOn.flag = TMN_POWER_NORMAL_JX;
        }

        /* 终端停电，5分钟内未收到5块电表的停电事件，把存储的电表事件上报 */
        if (abs(tm - (pMtrPowerOnOff->tmnPowerOff.happenTime)) >= 5 * 60)
        {
            for (index = 0; index < pMtrPowerOnOff->tmnPowerOff.happenCnt; index++)
            {
                rptBuf = pMtrPowerOnOff->tmnPowerOff.eventMsg[index].buf;
                rptLen = pMtrPowerOnOff->tmnPowerOff.eventMsg[index].len;
                taskmng_event_report_data_write(pipe, rptBuf, rptLen,PowerOnOffOad);
            }
            pMtrPowerOnOff->tmnPowerOff.happenCnt = 0;
            pMtrPowerOnOff->tmnPowerOff.flag = TMN_POWER_NORMAL_JX;
        }

        if (pMtrPowerOnOff->tmnPowerOn.flag == TMN_POWER_NORMAL_JX && 
            pMtrPowerOnOff->tmnPowerOff.flag == TMN_POWER_NORMAL_JX)
        {
            memset(pMtrPowerOnOff, 0x00, sizeof(MTR_POWER_ON_OFF_JX));
        }
    }
}

#endif

extern OOP_PERCEPTION_TERMINAL_LIST_T perceptionterminal;
uint16  GetVirtualPipePn(uint8 *pipeaddr,uint8 *termianladdr)
{
    uint16 i ,j;

    TASK_FMT_TRACE( 0, REC_LOG, "perceptionterminal.num is %d\n",perceptionterminal.num);
    for(i=0;i<perceptionterminal.num;i++)
    {
        TASK_BUF_TRACE( 0, REC_LOG, perceptionterminal.terminalinfo[i].terminaladdr,6);
        if(memcmp(termianladdr,perceptionterminal.terminalinfo[i].terminaladdr,6)==0)
        {
            break;
        }
    }
    if(i>=perceptionterminal.num)
    {
        return 0xFFFF;
    }
    TASK_FMT_TRACE( 0, REC_LOG, "perceptionterminal.terminalinfo[i].meterlist.num is %d\n",perceptionterminal.terminalinfo[i].meterlist.num);
    for(j=0;j<perceptionterminal.terminalinfo[i].meterlist.num;j++)
    {
        TASK_FMT_TRACE( 0, REC_LOG, "j is %d\n",j);
        TASK_BUF_TRACE( 0, REC_LOG, perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pipeaddr,6);
        if(memcmp(pipeaddr,perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pipeaddr,6)==0)
        {
           TASK_FMT_TRACE( 0, REC_LOG, "perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pn is %d\n",perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pn);
           return perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pn;
        }
    }
    return 0xFFFF;
}

BOOL  CmpVirtualPipeAddr(uint8 *pipeaddr,uint8 *termianladdr)
{
    uint16 i ,j;

    TASK_FMT_TRACE( 0, REC_LOG, "perceptionterminal.num is %d\n",perceptionterminal.num);
    for(i=0;i<perceptionterminal.num;i++)
    {
        TASK_BUF_TRACE( 0, REC_LOG, perceptionterminal.terminalinfo[i].terminaladdr,6);
        if(memcmp(termianladdr,perceptionterminal.terminalinfo[i].terminaladdr,6)==0)
        {
            break;
        }
    }
    if(i>=perceptionterminal.num)
    {
        return FALSE;
    }
    TASK_FMT_TRACE( 0, REC_LOG, "perceptionterminal.terminalinfo[i].meterlist.num is %d\n",perceptionterminal.terminalinfo[i].meterlist.num);
    for(j=0;j<perceptionterminal.terminalinfo[i].meterlist.num;j++)
    {
        TASK_FMT_TRACE( 0, REC_LOG, "j is %d\n",j);
        TASK_BUF_TRACE( 0, REC_LOG, perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pipeaddr,6);
        if(memcmp(pipeaddr,perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pipeaddr,6)==0)
        {
           TASK_FMT_TRACE( 0, REC_LOG, "perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pn is %d\n",perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pn);
           return   TRUE;
        }
    }
    return FALSE;
}


int  GetVirtualPipeAddr(uint16 pn,uint8 *MtrAddr)
{
    uint16 i ,j;

    for(i=0;i<perceptionterminal.num;i++)
    {    
        for(j=0;j<perceptionterminal.terminalinfo[i].meterlist.num;j++)
        {
            if(pn==perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pn)
            {
               memcpy_r(MtrAddr,perceptionterminal.terminalinfo[i].meterlist.meterinfo[j].pipeaddr,6);
                return 0;
            }
        }
    }
    if(i>=perceptionterminal.num)
        return -1;
    return 0;
}

/*******************************************************************************
* 函数名称: Save698TerminalEventData 
* 函数功能://写0xFF100A00 事件上报数据
* 输入参数:pipe 端口号
        DataBuf 事件内容
        DataLen 长度
        Pn 电表号
        MainOad 事件主oad
        StartTime 启动时标
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void Save698TerminalEventData(uint8 pipe,uint8* DataBuf,uint16 DataLen,uint16 Pn,OOP_OAD_U MainOad,time_t StartTime)
{

    uint8 i,j,k;
    uint16 index=0;
    uint8 CsdNum;
    uint8 RecordNum;
    uint16 rptLen = 0;
    //static uint16 terminalnum=10000;
    uint8 meternum;
    //事件上报时把固定列CSD先填进去
    uint8 rptBuff[2000] = {0x00};
    CsdNum = DataBuf[index++];
    OOP_PERCEPTION_TERMINAL_LIST_T tmpperceptionterminal;
    OOP_PERCEPTION_TERMINAL_INFO_T singalterminalinfo;

    memset((uint8*)&singalterminalinfo,0x00,sizeof(OOP_PERCEPTION_TERMINAL_INFO_T));
    //rptBuff[rptLen++] = 1;  //CSD类型1 记录型对象描述符
    memcpy_r(&rptBuff[rptLen],(uint8*)&MainOad.value,4);
    rptLen+=4;
    rptBuff[rptLen++] = CsdNum; //  

    TASK_FMT_DEBUG( pipe, RELAY_LOG,"Save698TerminalEventData 处理户变关系\n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,DataBuf, DataLen);
    for(i=0;i<CsdNum;i++)
    {
        if(DataBuf[index++]!=0)
            return;
        rptBuff[rptLen++] = 0x00;
        memcpy(&rptBuff[rptLen],&DataBuf[index],4);
        index+=4;
        rptLen+=4;
    }
    if(DataBuf[index++]==0)   //0为错误DAR 1 为SEQUENCE OF A-RecordRow
        return;
    RecordNum = DataBuf[index++];
    if(RecordNum==0)
        return;
        
    rptBuff[rptLen++] = 1;//结果类型
    rptBuff[rptLen++] = RecordNum;//记录条数

    //需要继续解析每条记录 不需要最后的跟随上报域以及时间标签
    //uint8 tmpBuff[30]={0};
    uint16 indexbak = index;
    uint16 tmpindexbak = index;
    uint8 flag =0 ;
    //uint8 tempmetrenum=0;
    if(perceptionterminal.num == 0)
    {
        perceptionterminal.totalpn = 10000;
    }
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"perceptionterminal.num is %d\n,perceptionterminal.totalpn is %d\n",perceptionterminal.num,perceptionterminal.totalpn);
    memcpy((uint8*)&tmpperceptionterminal,(uint8*)&perceptionterminal,sizeof(perceptionterminal));
    for(i=0;i<RecordNum;i++)
    {
        for(j=0;j<CsdNum;j++)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"j is %d,CsdNum is %d\n",j,CsdNum);
            if(j ==CsdNum-2)
            {
                tmpindexbak = index;
                memcpy_r(tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].terminaladdr,&DataBuf[tmpindexbak+3],6);
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"感知终端地址\n");
                TASK_BUF_DEBUG( pipe, RELAY_LOG,tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].terminaladdr, 6);
            }
            if(j ==CsdNum-1)
            {
                tmpindexbak = index;
                tmpindexbak = tmpindexbak+1;
                meternum = DataBuf[tmpindexbak++];
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"meternum is %d\n",meternum);
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.num is %d\n",tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.num);
                for(k=0;k<meternum;k++)
                {
                    flag =0;
                    tmpindexbak = tmpindexbak+5;
                    //tempmetrenum = perceptionterminal.terminalinfo[perceptionterminal.num].meterlist.num;
                    memcpy(tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pipeaddr,&DataBuf[tmpindexbak],6); 
                    tmpindexbak = tmpindexbak + 9;
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"第%d虚拟通道\n",k);
                    TASK_BUF_DEBUG( pipe, RELAY_LOG,tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pipeaddr, 6);
                    memcpy(tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].meteraddr,&DataBuf[tmpindexbak],6); 
                    TASK_BUF_DEBUG( pipe, RELAY_LOG,tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].meteraddr, 6);
                    tmpindexbak = tmpindexbak + 7;
                    tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pn = DataBuf[tmpindexbak];
                    tmpindexbak = tmpindexbak + 1;
                    if(TRUE == CmpVirtualPipeAddr(tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pipeaddr,tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].terminaladdr))
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG,"不是新的感知终端的地址\n");
                        continue;
                    }
                    flag =1;
                    tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pn = tmpperceptionterminal.totalpn;
                    tmpperceptionterminal.totalpn++;
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"tmpperceptionterminal.num is %d\n",tmpperceptionterminal.num);
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pn is %d,k is %d\n",tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.meterinfo[k].pn,k);
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.num is %d\n",tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.num);
                    tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num].meterlist.num++;
                }
                
                if(flag ==1)
                {
                    memcpy((uint8*)&singalterminalinfo,(uint8*)&tmpperceptionterminal.terminalinfo[tmpperceptionterminal.num],sizeof(OOP_PERCEPTION_TERMINAL_INFO_T));
                    tmpperceptionterminal.num++;
                }
            }
            taskmng_oop_next_read_data_analysis(DataBuf,&index,DataLen,DT_NULL,FALSE);
        }
    }
    //TASK_FMT_DEBUG( pipe, RELAY_LOG,"indexbak  %d index %d  DataLen %d \n",indexbak,index,DataLen);
    memcpy(&rptBuff[rptLen],&DataBuf[indexbak],index-indexbak);
    rptLen+=(index-indexbak);
    memcpy((uint8*)&perceptionterminal,(uint8*)&tmpperceptionterminal,sizeof(tmpperceptionterminal));
    if(flag ==1)
    {
        WritePerceptionTerminalData(pipe,(uint8*)&singalterminalinfo,sizeof(singalterminalinfo),PerceptionTerminalOad);
    }
    //将组好的事件内容写进数据中心
    taskmng_event_report_data_write(pipe,rptBuff,rptLen,EventReportOad);
}

/*******************************************************************************
* 函数名称: Save698SepicialEventData 
* 函数功能://写0xFF100A00 事件上报数据
* 输入参数:pipe 端口号
        DataBuf 事件内容
        DataLen 长度
        Pn 电表号
        MainOad 事件主oad
        StartTime 启动时标
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void Save698SepicialEventData(uint8 pipe,uint8* DataBuf,uint16 DataLen,uint16 Pn,OOP_OAD_U MainOad,time_t StartTime)
{

    uint16 rptLen = 0;

    uint8 rptBuff[2000];
  
    memcpy_r(&rptBuff[rptLen],(uint8*)&MainOad.value,4);
    rptLen+=4;
    memcpy(&rptBuff[rptLen],DataBuf,DataLen);
    rptLen = rptLen+DataLen;
    //将组好的事件内容写进数据中心
    taskmng_event_report_data_write(pipe,rptBuff,rptLen,EventReportOad);
}

/*******************************************************************************
* 函数名称: OopPatchTeminalAmrDataSave_Record
* 函数功能:补抄存储面向对象规约表记录型数据解析
* 输入参数: pipe 端口号
            DataBuf              //接收到的电表数据
            DataLen              //接收到的电表长度
            MtrIndex             //电表索引信息
            pTmp_NormalLink      //方案任务节点
            RecvRunTask     //任务运行时参数
            StoreTime            //存储时间
* 输出参数:
* 返 回 值:
*******************************************************************************/
uint16 OopPatchTeminalAmrDataSave_Record(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    
    uint16           Dataoffset = 0;
    uint8           CsdNum = DataBuf[Dataoffset++];    //CSD数目
    //OOP_OAD_U            Oad[128];
    OOP_CSD_T    csd[32];
     uint8           i;
    uint8           j;
    uint8           k;
    uint8           RecordNum = 0;
    
    //uint16          Pn;
    uint8  pipeaddr[6];
    uint8  termianladdr[6];
    uint16 virtualpn;
    uint16          OadLen = 0;

    DateTimeBcd_t   StoreTm;
//    OadInfoMap   pMap;
//    OOP_OAD_U            OadTmp;
    OOP_OAD_U       MainOad;

    //TASK_FMT_TRACE(Pipe, REC_LOG,"698补抄存储记录型数据 SchemeType %d RoadIndex %d CsdNum %d\n",pTmp_NormalLink->SchemeType,CsdNum);
    //变量RCSD对应的OAD信息
    for(i = 0; i < CsdNum; i++)
    {
        if(DataBuf[Dataoffset++] != 0)
        {
            csd[i].choice =1;
            memcpy_r((uint8*)&csd[i].road.oadMain.value,&DataBuf[Dataoffset],4);
            Dataoffset += 4;
            csd[i].road.oadCols.nNum = DataBuf[Dataoffset++];
            for(k = 0;k < csd[i].road.oadCols.nNum;k++)
            {
                memcpy_r((uint8*)&csd[i].road.oadCols.oad[k].value,&DataBuf[Dataoffset],4);
                Dataoffset += 4;
                 TASK_FMT_TRACE(Pipe, REC_LOG,"返回的csd %d  oad %08x \n",i,csd[i].road.oadCols.oad[k].value);
            }           
        }
        else
        {           
            csd[i].choice =0;     
            memcpy_r((uint8*)&csd[i].oad.value,&DataBuf[Dataoffset],4);
            Dataoffset += 4;
            TASK_FMT_TRACE(Pipe, REC_LOG,"返回的csd %d  oad %08x \n",i,csd[i].oad.value);
        }  
       
    }
    //错误信息 [0] DAR
    if(DataBuf[Dataoffset++] == 0x0)
    {
        //printf("错误信息 [0] DAR : %d  %d  %d\n",MtrIndex,pTmp_NormalLink->MsInfo[MtrIndex].CSDIndex,pTmp_NormalLink->MsInfo[MtrIndex].SigFrameNum);
        pAmrThreadArg->PatchMtrInfo[MtrIndex].CSDIndex += pAmrThreadArg->PatchMtrInfo[MtrIndex].SigFrameNum;
        return 0;
    }
    //MainOad = pTmp_NormalLink->CsdTask.RoadTask[0].MainOad;
    RecordNum = DataBuf[Dataoffset++];
    //TASK_FMT_TRACE(Pipe, REC_LOG,"mainoad %08x recorenum %d \n",MainOad.value,RecordNum);
    if(RecordNum == 0x0)
    {
        pAmrThreadArg->PatchMtrInfo[MtrIndex].CSDIndex += pAmrThreadArg->PatchMtrInfo[MtrIndex].SigFrameNum;
        return 0;
    }
    for(i = 0; i < RecordNum; i++)
    {
        for(j = 0; j < CsdNum; j++)
        {
            if(csd[j].choice == 1)
            {
                MainOad = csd[j].road.oadMain;   
                Dataoffset =Dataoffset +2;
                for(k=0;k < csd[j].road.oadCols.nNum;k++)
                {
                    if(pTmp_NormalLink->StoreTimeType == STRTMTYP_FRZTIME)
                    {
                        //抄读record 的时候第一个OI为时标
                        if(csd[j].road.oadCols.oad[k].nObjID == 0x2021)
                        {
                            //抄读record 的时候第一个OI为时标
                            if(csd[j].road.oadCols.oad[k].nObjID == 0x2021)
                            {
                                DateTimeHex_t        TmpDateTimeHex;
                                memset(&TmpDateTimeHex,0x0,sizeof(TmpDateTimeHex));
                                TASK_FMT_TRACE( Pipe, REC_LOG, "冻结时标,Dataoffset is %d\n",Dataoffset);
                                TASK_BUF_TRACE( Pipe, REC_LOG, DataBuf+Dataoffset+1,6);
                                TmpDateTimeHex.year_h = DataBuf[Dataoffset+1];
                                TmpDateTimeHex.year_l = DataBuf[Dataoffset+2];
                                TmpDateTimeHex.month  = DataBuf[Dataoffset+3];
                                TmpDateTimeHex.day    = DataBuf[Dataoffset+4];
                                TmpDateTimeHex.hour   = DataBuf[Dataoffset+5];
                                TmpDateTimeHex.min    = DataBuf[Dataoffset+6];

                                if(DataBuf[Dataoffset+7] != 0xff)//曲线秒回复为0xff(林洋电表)
                                TmpDateTimeHex.sec    = DataBuf[Dataoffset+7];

                                DT_DateTimeHex2BCD(&TmpDateTimeHex,&StoreTm);
                            }

                        }
                    }
                    else
                    {
                        StoreTm = taskmng_patch_scheme_task_store_time_get(Pipe,MtrIndex,pTmp_NormalLink);
                    }
                    //TASK_FMT_TRACE(Pipe, REC_LOG, "第%d条记录第%d个数据项  \n",i,j);
                    TASK_FMT_TRACE(Pipe, REC_LOG,"StoreTm %02x/%02x/%02x %02x:%02x:%02x \n",StoreTm.year_l,StoreTm.month,StoreTm.day,StoreTm.hour,StoreTm.min,StoreTm.sec);

                    OadLen = OopPatchAmrData(Pipe,MtrIndex,MainOad,csd[j].road.oadCols.oad[k],DataBuf+Dataoffset,(DataLen-Dataoffset),pTmp_NormalLink,&StoreTm);
                    Dataoffset += OadLen;
                 }
            }
        else
        {

            if(csd[j].oad.value == 0x202A0200)
            {
                memcpy(pipeaddr,DataBuf+Dataoffset+3,6);
                TASK_FMT_TRACE( Pipe, REC_LOG, "csd %ld  Dataoffset is %d\n",j,Dataoffset);
                TASK_BUF_TRACE( Pipe, REC_LOG, DataBuf+Dataoffset,50);
                TASK_BUF_TRACE( Pipe, REC_LOG, termianladdr,6);
                TASK_BUF_TRACE( Pipe, REC_LOG, pipeaddr,6);
                virtualpn = GetVirtualPipePn(pipeaddr,termianladdr);
                TASK_FMT_TRACE( Pipe, REC_LOG, "virtualpn %ld  \n",virtualpn);
            }

            taskmng_oop_next_read_data_analysis(DataBuf,&Dataoffset,DataLen,DT_NULL,FALSE);
            //OadLen = OopMeterAmrData(Pipe,MtrIndex,MainOad,csd[j].oad,DataBuf+Dataoffset,(DataLen-Dataoffset),pTmp_NormalLink,RecvRunTask,&StoreTm);
            //Dataoffset += OadLen;
        }
        }
    }
    pAmrThreadArg->PatchMtrInfo[MtrIndex].CSDIndex += pAmrThreadArg->PatchMtrInfo[MtrIndex].SigFrameNum;

    return 1;
}



/*******************************************************************************
* 函数名称: OopMeterAmrData
* 函数功能:存储面向对象规约表数据
* 输入参数:pipe 端口号
         MtrIndex 表索引
         MainOad 主oad
         Oad 子oad
         pRcvBuf 收到的数据buf
         RcvLen 收到的数据长度
         pTmp_NormalLink 任务参数
         RecvRunTask 任务运行时参数
         StoreTime 存储时间
         
* 输出参数:
* 返 回 值: 已解析存储的数据长度
*******************************************************************************/
uint16 OopTerminalAmrData(uint8 Pipe,uint16 MtrIndex,OOP_OAD_U MainOad,OOP_OAD_U Oad, uint8* pRcvBuf, uint16 RcvLen, SCHEME_TASK_RUN_INFO* pTmp_NormalLink,CTS_RUN_TASK_T *RecvRunTask , DateTimeBcd_t *StoreTime,uint16 pn)
{
    AmrThreadArg_T *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16 offset =0 ;
    uint16 DataLen = 0;
    uint8 DataBuff[MAX_MSG_LEN]={0};
    uint16 Pn;
    uint8 logicid;
    DateTimeBcd_t  tmpTime;
    OadStoreInfo StoreInfo;
    memset(&StoreInfo,0x00,sizeof(OadStoreInfo));
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    BOOL ret;
    uint8 pwrType;
 
    Pn = pn;
    logicid = pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid;
    if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].IsNeedGetSafeMode == TRUE)
    {
        //保存ESAM序列号
        if(Oad.nObjID==0x4002)
        {
            offset+=2;;
           
            uint8 MtrNo[8]={0};
            uint8 len = pRcvBuf[1];
            if(len>8)
                memcpy(MtrNo,pRcvBuf+2+len-8,8);
            else
                memcpy(MtrNo+8-len,pRcvBuf+2,len);
            TASK_FMT_TRACE(Pipe,REC_LOG,"保存ESAM序列号 %d 0x%02x%02x%02x%02x%02x%02x%02x%02x\n",Pn,MtrNo[0],MtrNo[1],
                MtrNo[2],MtrNo[3],MtrNo[4],MtrNo[5],MtrNo[6],MtrNo[7]);
            WriteOopMeterSecurityEsamSerialNo(Pipe,Pn,MtrNo);
            Set_ReadMtrNum(Pipe,MtrNo);
            offset+=len;
        }
        //存储安全模式
        if(Oad.nObjID==0xF101)
        {
            offset++;
            if(pRcvBuf[1]!=1)
                pRcvBuf[1]=0;
            TASK_FMT_TRACE(Pipe,REC_LOG,"保存存储安全模式 %d 开关 %d \n",Pn,pRcvBuf[1]);
            WriteOopMeterSecuritySecModEn(Pipe,Pn,pRcvBuf[1]);
            Set_MtrSecModEn(Pipe,pRcvBuf[1]);
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].IsNeedGetSafeMode=FALSE;
            offset++;
        }
        return offset;
    }
    
    //根据oad获取对应的结构体信息
    ret = taskmng_oad_info_get( Oad,&pOadInfo);
    //TASK_FMT_TRACE(Pipe,REC_LOG,"OopMeterAmrData OAD %08x ret %d \n",Oad.value,ret);
    if(ret==FALSE)
    {
        return 0;
    }
    if(pRcvBuf[0]==0) //如果为datanull 则无需解析
    {
        TASK_FMT_TRACE(Pipe,REC_LOG,"data为空 不解析\n");
        return 1;
    }
    pwrType=taskmng_mtr_conntype_get(Pn,logicid);
    Convert_Oop_Data_To_Struct(pRcvBuf,&offset,RcvLen,DataBuff,&pOadInfo,&DataLen,pwrType);
    //TASK_FMT_TRACE(Pipe, REC_LOG,"Convert_Oop_Data_To_Struct  DataLen %d 转换后数据 \n",DataLen);

    //TASK_BUF_TRACE(Pipe, REC_LOG,DataBuff, DataLen);
    
    StoreInfo.recordOAD.logicId=logicid;
    StoreInfo.recordOAD.infoNum = Pn;

    StoreInfo.recordOAD.road.oadMain=MainOad;
    StoreInfo.recordOAD.road.oadCols.nNum=1;
    StoreInfo.recordOAD.road.oadCols.oad[0]=Oad;
    StoreInfo.recordOAD.classtag = 5;
    if(Pn>=10000)
    {
        GetVirtualPipeAddr(Pn,(uint8*)StoreInfo.MAC.nValue);
    }
    else
    {
        taskmng_mtr_addr_get( Pn, logicid,(uint8*)StoreInfo.MAC.nValue);
    }
    memcpy_r((uint8*)&tmpTime,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
    DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StoreInfo.colStartTm);     //开始时间

    if(StoreTime==NULL)
    {
        time_t storetm;
        OOP_TI_T delay;
        DT_DateTimeBCD2Time(&tmpTime,&storetm);
        delay.unit = pTmp_NormalLink->DelayTimeUnit;
        delay.value = pTmp_NormalLink->DelayTime;
        DT_TimeSubInv(&storetm, delay);
        tmpTime = taskmng_acq_store_time_get(pTmp_NormalLink->StoreTimeType,storetm);
        DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StoreInfo.colStoreTm);
    }
    else
    {
        DT_DateTimeBCD2Time(StoreTime,(time_t*)&StoreInfo.colStoreTm);
        
        TASK_FMT_TRACE(Pipe, REC_LOG,"StoreTm 20%02x/%02x/%02x %02x:%02x:%02x \n",StoreTime->year_l,StoreTime->month,StoreTime->day,StoreTime->hour,StoreTime->min,StoreTime->sec);
    }
    StoreInfo.colEndTm = time(NULL);
    taskmng_meter_data_save(DataBuff,DataLen ,StoreInfo, Pipe );

    return offset;
}

/*******************************************************************************
* 函数名称: 存储面向对象规约表记录型数据解析
* 函数功能:
* 输入参数: pipe 端口号
            DataBuf              //接收到的电表数据
            DataLen              //接收到的电表长度
            MtrIndex             //电表索引信息
            pTmp_NormalLink      //方案任务节点
            RecvRunTask     //任务运行时参数
            StoreTime            //存储时间
* 输出参数:
* 返 回 值:
*******************************************************************************/
uint16 taskmng_oop_meter_data_tasktable_save(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink,CTS_RUN_TASK_T *RecvRunTask,DateTimeBcd_t *StoreTime)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16           Dataoffset = 0;
    uint8           CsdNum = DataBuf[Dataoffset++];    //CSD数目
    //OOP_OAD_U            Oad[128];
    OOP_CSD_T    csd[32];
    uint8           i;
    uint8           j;
    uint8           k;
    uint8           RecordNum = 0;
    
    //uint16          Pn;
    uint16          OadLen = 0;
    uint8           ExistPosae = 0;
    DateTimeBcd_t   StoreTm;
//    OadInfoMap   pMap;
//    OOP_OAD_U            OadTmp;
    OOP_OAD_U       MainOad;
    uint8           RoadIndex=0;
    uint8  pipeaddr[6];
    uint8  termianladdr[6];
    uint16 virtualpn;
    uint32 EventSeq;
    uint32 EventSeqReport;
    uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;//测量点号

    taskmng_mtr_addr_get(Pn,0,termianladdr);
    virtualpn = Pn;
    if(pTmp_NormalLink->SchemeType == EVENTPLAN)
    {
        RoadIndex = pAmrThreadArg->pMtrAmrInfo[MtrIndex].ROADIndex;
        
    }
    TASK_FMT_TRACE(Pipe, REC_LOG,"698存储记录型数据 SchemeType %d RoadIndex %d CsdNum %d\n",pTmp_NormalLink->SchemeType,RoadIndex,CsdNum);
    TASK_BUF_TRACE( Pipe, REC_LOG, DataBuf,50);
    //变量RCSD对应的OAD信息
    for(i = 0; i < CsdNum; i++)
    {
        if(DataBuf[Dataoffset++] != 0)
        {
            csd[i].choice =1;
            memcpy_r((uint8*)&csd[i].road.oadMain.value,&DataBuf[Dataoffset],4);
            Dataoffset += 4;
            csd[i].road.oadCols.nNum = DataBuf[Dataoffset++];
            for(k = 0;k < csd[i].road.oadCols.nNum;k++)
            {
                memcpy_r((uint8*)&csd[i].road.oadCols.oad[k].value,&DataBuf[Dataoffset],4);
                Dataoffset += 4;
                if(pTmp_NormalLink->CsdTask.RoadTask[RoadIndex].MainOad.nObjID==0x5004 && csd[i].road.oadCols.oad[k].nObjID==0x0010)
                ExistPosae=1;
                 TASK_FMT_TRACE(Pipe, REC_LOG,"返回的csd %d  oad %08x \n",i,csd[i].road.oadCols.oad[k].value);
            }           
        }
        else
        {           
            csd[i].choice =0;     
            memcpy_r((uint8*)&csd[i].oad.value,&DataBuf[Dataoffset],4);
            Dataoffset += 4;
            TASK_FMT_TRACE(Pipe, REC_LOG,"返回的csd %d  oad %08x \n",i,csd[i].oad.value);
        }  
       
    }

    //错误信息 [0] DAR
    if(DataBuf[Dataoffset++] == 0x0)
    {
        //printf("错误信息 [0] DAR : %d  %d  %d\n",MtrIndex,pTmp_NormalLink->MsInfo[MtrIndex].CSDIndex,pTmp_NormalLink->MsInfo[MtrIndex].SigFrameNum);
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
        return 0;
    }
    //MainOad = pTmp_NormalLink->CsdTask.RoadTask[RoadIndex].MainOad;
    RecordNum = DataBuf[Dataoffset++];
    TASK_FMT_TRACE(Pipe, REC_LOG,"mainoad %08x recordnum %d \n",MainOad.value,RecordNum);
    
    if(RecordNum == 0x0)
    {
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
        if(ExistPosae == 1)
        {
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].CurNum=1;
            pAmrThreadArg->pMtrAmrInfo[MtrIndex].EndNum=0;
        }
        return 0;
    }
    //日冻结不为空，置标志不再补抄
    if(ExistPosae == 1)
    {
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].EndNum=62;
    }

    for(i = 0; i < RecordNum; i++)//记录数目
    {
        //根据每条记录解析对应的OAD内容
        for(j = 0; j < CsdNum; j++)
        {
            if(csd[j].choice == 1)
            {
                MainOad = csd[j].road.oadMain;   
                Dataoffset =Dataoffset +2;
                for(k=0;k < csd[j].road.oadCols.nNum;k++)
                {
    //                OadTmp = csd[j].road.oadCols.oad[k];
    //                OadTmp.attPro = 0;
    //                TASK_FMT_TRACE( Pipe, REC_LOG, "csd[j].choice is %d\n",csd[j].choice);
    //                if(taskmng_oad_info_get(OadTmp,&pMap)==FALSE)
    //                {
    //                    pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
    //                    return 0;
    //                }
                    if(pTmp_NormalLink->SchemeType ==EVENTPLAN && csd[j].road.oadCols.oad[k].value== 0x20220200)
                    {
                        if(DataBuf[Dataoffset]==DT_DOUBLE_LONG_UNSIGNED)
                            memcpy_r((uint8*)&EventSeq,&DataBuf[Dataoffset+1],4);
                        TASK_FMT_TRACE( Pipe, REC_LOG, "事件序号 %ld \n",EventSeq);
                        EventSeqReport = GetTaskReportSeq( Pipe, RecvRunTask->TaskId, RecvRunTask->logicid,MainOad, Pn);
                        TASK_FMT_TRACE( Pipe, REC_LOG, "该事件上次抄读的记录序号为%ld  \n",EventSeqReport );
                        if(EventSeq==EventSeqReport)
                        {
                            pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
                            return 0;
                        }
                        else
                        {
                            SetTaskReportSeq( Pipe, RecvRunTask->TaskId,RecvRunTask->logicid, MainOad, Pn,EventSeq);
                        }
                    }
                    TASK_FMT_TRACE( Pipe, REC_LOG, "pTmp_NormalLink->StoreTimeType is %d\n",pTmp_NormalLink->StoreTimeType);
                    if(pTmp_NormalLink->StoreTimeType == STRTMTYP_FRZTIME)
                    {
                        //抄读record 的时候第一个OI为时标
                        if(csd[j].road.oadCols.oad[k].nObjID == 0x2021)
                        {
                            DateTimeHex_t        TmpDateTimeHex;
                            memset(&TmpDateTimeHex,0x0,sizeof(TmpDateTimeHex));
                            TASK_FMT_TRACE( Pipe, REC_LOG, "冻结时标,Dataoffset is %d\n",Dataoffset);
                            TASK_BUF_TRACE( Pipe, REC_LOG, DataBuf+Dataoffset+1,6);
                            TmpDateTimeHex.year_h = DataBuf[Dataoffset+1];
                            TmpDateTimeHex.year_l = DataBuf[Dataoffset+2];
                            TmpDateTimeHex.month  = DataBuf[Dataoffset+3];
                            TmpDateTimeHex.day    = DataBuf[Dataoffset+4];
                            TmpDateTimeHex.hour   = DataBuf[Dataoffset+5];
                            TmpDateTimeHex.min    = DataBuf[Dataoffset+6];

                            if(DataBuf[Dataoffset+7] != 0xff)//曲线秒回复为0xff(林洋电表)
                                TmpDateTimeHex.sec    = DataBuf[Dataoffset+7];

                            DT_DateTimeHex2BCD(&TmpDateTimeHex,&StoreTm);

                        }
                    }
                    else
                    {
                        StoreTm = taskmng_scheme_task_store_time_get(Pipe,pTmp_NormalLink);
                        //存储时间时，若一次抄回多条事件，以秒数区分各事件
                        if(pTmp_NormalLink->SchemeType == EVENTPLAN)
                            StoreTm.sec = i;
                    }
                    TASK_FMT_TRACE(Pipe, REC_LOG, "第%d条记录第%d个数据项 存储时间 \n",i,j);
                    WriteTimeLog(Pipe, (char *) &StoreTm);
                    if(DataLen<=Dataoffset)
                    {
                        TASK_FMT_TRACE(Pipe, REC_LOG, "DataLen is %d Dataoffset is %d\n",DataLen,Dataoffset);
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
                        return 1;
                    }
                    OadLen = OopTerminalAmrData(Pipe,MtrIndex,MainOad,csd[j].road.oadCols.oad[k],DataBuf+Dataoffset,(DataLen-Dataoffset),pTmp_NormalLink,RecvRunTask,&StoreTm,virtualpn);
                    Dataoffset += OadLen;
                    }
                }
                else
                {
                    //MainOad.value = 0;
                    if(csd[j].oad.value == 0x202A0200)
                    {
                        memcpy(pipeaddr,DataBuf+Dataoffset+3,6);
                        TASK_FMT_TRACE( Pipe, REC_LOG, "csd %ld  Dataoffset is %d\n",j,Dataoffset);
                        TASK_BUF_TRACE( Pipe, REC_LOG, DataBuf+Dataoffset,50);
                        TASK_BUF_TRACE( Pipe, REC_LOG, termianladdr,6);
                        TASK_BUF_TRACE( Pipe, REC_LOG, pipeaddr,6);
                        virtualpn = GetVirtualPipePn(pipeaddr,termianladdr);
                        if(virtualpn == 0xFFFF)
                        {
                            virtualpn = Pn;
                        }
                        TASK_FMT_TRACE( Pipe, REC_LOG, "virtualpn %ld  \n",virtualpn);
                    }

                    taskmng_oop_next_read_data_analysis(DataBuf,&Dataoffset,DataLen,DT_NULL,FALSE);
                    //OadLen = OopMeterAmrData(Pipe,MtrIndex,MainOad,csd[j].oad,DataBuf+Dataoffset,(DataLen-Dataoffset),pTmp_NormalLink,RecvRunTask,&StoreTm);
                    //Dataoffset += OadLen;
                }
        }
    }
        //事件采集方案 如果需要上报 则组成6012格式写入数据中心事件上报表中
    if(pTmp_NormalLink->SchemeType ==EVENTPLAN && pTmp_NormalLink->rptFlag == TRUE )
    {
        TASK_FMT_TRACE( Pipe, REC_LOG, "该事件方案需要上报！\n");
        time_t StartTime;
        DateTimeBcd_t tmpTime;
        memcpy_r((uint8*)&tmpTime,(uint8*)RecvRunTask->ThisTurnBeginTime,7);
        DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StartTime);

        Save698EventData(Pipe, DataBuf, DataLen, Pn, MainOad, StartTime);

    }
    if(DataBuf[Dataoffset++] ==1) //有跟随上报信息域
    {
        OOPFollowReport(Pipe,Pn,&DataBuf[Dataoffset],DataLen-Dataoffset);
    }
    pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex += pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;

    return 1;

}


/*******************************************************************************
* 函数名称: taskmng_oop_perception_term_get_request
* 函数功能: 面向对象表组帧
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_oop_perception_term_get_request(uint8 Pipe,uint8 seqindex, time_t StartTime,int MsIndex,OOP_OAD_U MainOad,uint8 SubOadNum,OOP_OAD_U *SubOad,uint16 *FrameLen, uint8 *FrameBuff)
{
    OOP_TSA_T                    TsaAddr;
    LinkLayer_t*            pLinkLayer = NULL;
    SCHEME_TASK_RUN_INFO    pTmp_NormalLink;
    uint16                  LastTaskId;
    uint8                   rsd[30];
    uint8                   rsdLen=0;
    OOP_OAD_U                    RsdOad;
    //uint16                  OadId;
    GetRecord_t             GetRecord;
    uint8                    InDataBuf[255];
    uint16                  offset = 0;
    uint16                  i = 0;
    //OadInfoMap              OadInfo;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    LastTaskId     = pAmrThreadArg->TmnAmrInfo.CurrentTaskId;//发送时的方案号
    AcqType_t  Acqtype =ACQTYP_NULL;
    taskmng_task_info_get(LastTaskId,pAmrThreadArg->TmnAmrInfo.CurrentLogicid, &pTmp_NormalLink);

    OOP_MSP_T    ms;
    OOP_OAD_U tempMainOad;
    uint16 tmpoffset;
    OOP_OAD_U                    tempRsdOad;
    OadInfoMap OadInfo;
    BOOL frezon =FALSE;
    #if (defined AREA_JIANGXI) 
    OOP_METER_BASIC_T MtrAddr;
    taskmng_mtr_basic_info_get(pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid,&MtrAddr);
    #endif
    tempMainOad.value = 0x60120300;
    ms.choice = 1;
    //读取面向对象电表地址
    TsaAddr = taskmng_tsa_get(pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid);
    //TASK_FMT_TRACE(Pipe,REC_LOG,"mainoad %08x    \n",MainOad.value);
     Acqtype = pTmp_NormalLink.AcqInfo.Acqtype;
    //根据主OAD对应的类号获取RSD方法对象属性描述符OAD

    //载波组645帧时按照并发数分配通道号 以防有分帧时报文被覆盖
    if(ACQTYP_NULL == Acqtype || (TRUE == pAmrThreadArg->pMtrAmrInfo[MsIndex].IsNeedGetSafeMode))//实时数据采用GetRequestNormalList
    {
        offset = 0;
        for(i = 0; i < SubOadNum; i++)
        {
            InDataBuf[offset++] = (uint8)(SubOad[i].nObjID >> 8);
            InDataBuf[offset++] = (uint8)(SubOad[i].nObjID);
            InDataBuf[offset++] = SubOad[i].attID;
            InDataBuf[offset++] = SubOad[i].nIndex;
        }
        OopGet.requst(oop_pipe_get(Pipe,seqindex),&TsaAddr,2,InDataBuf,offset);
    }
    else//其他读取采用GetRequestRecord
    {
        if(ACQTYP_LASTN == Acqtype)
        {
            rsdLen = MakeRsd(Pipe,10,rsd,&pTmp_NormalLink,RsdOad,MsIndex,StartTime,&ms);
        }
        else
        {
            rsdLen = MakeRsd(Pipe,7,rsd,&pTmp_NormalLink,RsdOad,MsIndex,StartTime,&ms);
        }
        GetRecord.rsd = rsd;
        GetRecord.rsdLen = rsdLen;

        GetRecord.rcsd = InDataBuf;

        offset = 1;
        
        GetRecord.rcsd[offset++] = 0;
        GetRecord.rcsd[offset++] = 0x60;
        GetRecord.rcsd[offset++] = 0x40;
        GetRecord.rcsd[offset++] = 0x02;
        GetRecord.rcsd[offset++] = 0x00;

        GetRecord.rcsd[offset++] = 0;
        GetRecord.rcsd[offset++] = 0x60;
        GetRecord.rcsd[offset++] = 0x41;
        GetRecord.rcsd[offset++] = 0x02;
        GetRecord.rcsd[offset++] = 0x00;

        GetRecord.rcsd[offset++] = 0;
        GetRecord.rcsd[offset++] = 0x60;
        GetRecord.rcsd[offset++] = 0x42;
        GetRecord.rcsd[offset++] = 0x02;
        GetRecord.rcsd[offset++] = 0x00;

        GetRecord.rcsd[offset++] = 0;
        GetRecord.rcsd[offset++] = 0x20;
        GetRecord.rcsd[offset++] = 0x2A;
        GetRecord.rcsd[offset++] = 0x02;
        GetRecord.rcsd[offset++] = 0x00;
        
        if(MainOad.nObjID !=0x0000)
        {
            GetRecord.rcsd[offset++] = 1;
            GetRecord.rcsd[offset++] = (uint8)(MainOad.nObjID >> 8);
            GetRecord.rcsd[offset++] = (uint8)(MainOad.nObjID);
            GetRecord.rcsd[offset++] = MainOad.attID;
            GetRecord.rcsd[offset++] = MainOad.nIndex;
           if(taskmng_oad_info_get(MainOad,&OadInfo)==FALSE)
                return 0;
            if(OadInfo.class ==9)
            {
                tempRsdOad.value = 0x20210200;

            }
            else 
            {
                tempRsdOad.value = 0x20220200;
            }
        }

        tmpoffset = offset;
        GetRecord.rcsd[offset++] = SubOadNum;
        for(i = 0; i < SubOadNum; i++)
        {
            if(SubOad[i].nObjID == tempRsdOad.nObjID)
            {
                frezon =TRUE;
            }
        }
        if(frezon ==TRUE)
        {
            for(i = 0; i < SubOadNum; i++)
            {

                GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID >> 8);
                GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID);
                GetRecord.rcsd[offset++] = SubOad[i].attID;
                GetRecord.rcsd[offset++] = SubOad[i].nIndex;

            }
        }
        else
        {
            GetRecord.rcsd[tmpoffset] = SubOadNum+1;
            
            GetRecord.rcsd[offset++] = (uint8)(tempRsdOad.nObjID >> 8);
            GetRecord.rcsd[offset++] = (uint8)(tempRsdOad.nObjID);
            GetRecord.rcsd[offset++] = tempRsdOad.attID;
            GetRecord.rcsd[offset++] = tempRsdOad.nIndex;
            for(i = 0; i < SubOadNum; i++)
            {

                GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID >> 8);
                GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID);
                GetRecord.rcsd[offset++] = SubOad[i].attID;
                GetRecord.rcsd[offset++] = SubOad[i].nIndex;

            }
        }

        GetRecord.rcsd[0] = (tmpoffset-1)/5;
        GetRecord.rcsdLen = offset;
        GetRecord.oad = tempMainOad;

       
        OopGet.requst(oop_pipe_get(Pipe,seqindex),&TsaAddr,3,(uint8*)&GetRecord,sizeof(GetRecord_t));
    }
    
    pLinkLayer = FindLinkLayer(oop_pipe_get(Pipe,seqindex), NULL);

    if(pLinkLayer->sendLen>MAX_MSG_LEN)
        return 0;
    *FrameLen = pLinkLayer->sendLen;
    memcpy(FrameBuff,pLinkLayer->sendBuff,*FrameLen);

    return     *FrameLen;
}


/*******************************************************************************
* 函数名称: OopPatchGetRequst 
* 函数功能: //组698协议补抄报文
* 输入参数: 

* 输出参数:  
* 返 回 值:   
*******************************************************************************/
int OopTermianlPatchGetRequst(uint8 Pipe,uint8 seqindex, time_t StartTime,int MsIndex,OOP_OAD_U MainOad,uint8 SubOadNum,OOP_OAD_U *SubOad,uint16 *FrameLen, uint8 *FrameBuff,SCHEME_TASK_RUN_INFO *pTmp_NormalLink)
{
    OOP_TSA_T                    TsaAddr;
    LinkLayer_t*            pLinkLayer = NULL;
    uint8                   rsd[30];
    uint8                   rsdLen=0;
    OOP_OAD_U                    RsdOad;
    GetRecord_t             GetRecord;
    uint8                    InDataBuf[255];
    uint16                  offset = 0;
    uint16                  i = 0;
    OOP_OAD_U tempMainOad;
    uint16 tmpoffset;

    tempMainOad.value = 0x60120300;

    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];

    TsaAddr = taskmng_tsa_get(pAmrThreadArg->PatchMtrInfo[MsIndex].Pn,pAmrThreadArg->PatchMtrInfo[MsIndex].logicid);
   
    rsdLen = MakeTerminalPatchRsdFrame(Pipe,rsd,pTmp_NormalLink,RsdOad,MsIndex,StartTime);
    GetRecord.rsd = rsd;
    GetRecord.rsdLen = rsdLen;

    GetRecord.rcsd = InDataBuf;

    offset = 1;
    
    GetRecord.rcsd[offset++] = 0;
    GetRecord.rcsd[offset++] = 0x60;
    GetRecord.rcsd[offset++] = 0x40;
    GetRecord.rcsd[offset++] = 0x02;
    GetRecord.rcsd[offset++] = 0x00;
    
    GetRecord.rcsd[offset++] = 0;
    GetRecord.rcsd[offset++] = 0x60;
    GetRecord.rcsd[offset++] = 0x41;
    GetRecord.rcsd[offset++] = 0x02;
    GetRecord.rcsd[offset++] = 0x00;
    
    GetRecord.rcsd[offset++] = 0;
    GetRecord.rcsd[offset++] = 0x60;
    GetRecord.rcsd[offset++] = 0x42;
    GetRecord.rcsd[offset++] = 0x02;
    GetRecord.rcsd[offset++] = 0x00;
    
    GetRecord.rcsd[offset++] = 0;
    GetRecord.rcsd[offset++] = 0x20;
    GetRecord.rcsd[offset++] = 0x2A;
    GetRecord.rcsd[offset++] = 0x02;
    GetRecord.rcsd[offset++] = 0x00;
    
    if(MainOad.nObjID !=0x0000)
    {
        GetRecord.rcsd[offset++] = 1;
        GetRecord.rcsd[offset++] = (uint8)(MainOad.nObjID >> 8);
        GetRecord.rcsd[offset++] = (uint8)(MainOad.nObjID);
        GetRecord.rcsd[offset++] = MainOad.attID;
        GetRecord.rcsd[offset++] = MainOad.nIndex;
    }
    
    tmpoffset = offset;
    GetRecord.rcsd[offset++] = SubOadNum;
    for(i = 0; i < SubOadNum; i++)
    {
    
        GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID >> 8);
        GetRecord.rcsd[offset++] = (uint8)(SubOad[i].nObjID);
        GetRecord.rcsd[offset++] = SubOad[i].attID;
        GetRecord.rcsd[offset++] = SubOad[i].nIndex;
    
    }
    
    GetRecord.rcsd[0] = (tmpoffset-1)/5;
    GetRecord.rcsdLen = offset;
    GetRecord.oad = tempMainOad;
    
    
    OopGet.requst(oop_pipe_get(Pipe,seqindex),&TsaAddr,3,(uint8*)&GetRecord,sizeof(GetRecord_t));

    pLinkLayer = FindLinkLayer(oop_pipe_get(Pipe, seqindex), NULL);
    *FrameLen = pLinkLayer->sendLen;
    memcpy(FrameBuff,pLinkLayer->sendBuff,*FrameLen);

    return     *FrameLen;
}


/*******************************************************************************
* 函数名称: OopPatchGetRequst 
* 函数功能: //组698协议补抄报文
* 输入参数: 

* 输出参数:  
* 返 回 值:   
*******************************************************************************/
int MakeTerminalPatchRsdFrame(uint8 Pipe,uint8 *RsdBuf,SCHEME_TASK_RUN_INFO *pTmp_NormalLink,OOP_OAD_U RsdOad,int MsIndex,time_t StartTime)
{
    uint8                   RsdLen = 0;
    //DateTimeBcd_t           TempTimeBcd;
    time_t                  TempTimeSec;
    OOP_TI_T                    DelayTi;

    RsdBuf[RsdLen++] = 7;    //选择方法2




    TempTimeSec = StartTime;
    if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID < 0x5000 || pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID > 0x5003)
        TempTimeSec = TempTimeSec+60;
    else
        TempTimeSec = StartTime;
    DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
    RsdLen += 7;

    if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
        TempTimeSec = TempTimeSec+taskmng_task_period_get(pTmp_NormalLink->TaskId,pTmp_NormalLink->logicid);
    else
    {
        TempTimeSec = StartTime;
        DelayTi.value = pTmp_NormalLink->AcqInfo.TI.value;
        DelayTi.unit = pTmp_NormalLink->AcqInfo.TI.unit;
        DT_TimeAddInv(&TempTimeSec, DelayTi);//加上一个时标间隔，即只抄1个点
        TempTimeSec = TempTimeSec-1;  //部分表筛选方法是前闭后闭，-1秒强制为前闭后开
    }
    DT_Time2DateTimeHex(TempTimeSec, (DateTimeHex_t*)(RsdBuf+RsdLen));
    RsdLen += 7;

    if(pTmp_NormalLink->AcqInfo.Acqtype == ACQTYP_FRZTM)
    {
        if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
            pTmp_NormalLink->AcqInfo.TI.unit = TASK_CYCLE_FLAG_DAY;
        else if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5006)
            pTmp_NormalLink->AcqInfo.TI.unit = TASK_CYCLE_FLAG_MON;
        else if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID== 0x5003)
            pTmp_NormalLink->AcqInfo.TI.unit = TASK_CYCLE_FLAG_HOUR;
        else if(pTmp_NormalLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)
            pTmp_NormalLink->AcqInfo.TI.unit = TASK_CYCLE_FLAG_MIN;
        pTmp_NormalLink->AcqInfo.TI.value = 0;
    }
    RsdBuf[RsdLen++] = pTmp_NormalLink->AcqInfo.TI.unit;
    RsdBuf[RsdLen++] = (uint8)(pTmp_NormalLink->AcqInfo.TI.value >> 8);
    RsdBuf[RsdLen++] = (uint8)pTmp_NormalLink->AcqInfo.TI.value;
    RsdBuf[RsdLen++] = 0x01;//MS集合默认全部电表地址;
    return RsdLen;
}


/*******************************************************************************
* 函数名称: taskmng_perception_tmn_info_clear 
* 函数功能: //清空内存中感知终端信息
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_perception_tmn_info_clear()
{
    PerceptionTerminalInfoNum = 1;
    //pthread_mutex_lock(&perceptionterminal.lock);
    perceptionterminal.num = 0;
    memset((uint8*)&perceptionterminal,0x00,sizeof(OOP_PERCEPTION_TERMINAL_LIST_T));
    //pthread_mutex_unlock(&perceptionterminal.lock);
}


#endif


