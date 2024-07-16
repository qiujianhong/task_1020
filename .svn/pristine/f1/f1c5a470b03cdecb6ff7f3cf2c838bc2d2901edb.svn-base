#ifdef AREA_JIANGXI

#include "pdsAll.h"
#include "pdsCommon.h"
#include "area.h"
#include "pdsMqtt.h"
#include <sys/prctl.h>

TOPO_ARG_T g_TopoArg;

extern Meter_File_T MeterFileAll;
extern LockQueue_T gTransQueue;            //存放收到的taskManager透传消息回码报文
extern uint8 LocalAddr[6];
extern int gCalcSock;
extern DateTimeHex_t              gCurBinTime;                             //当前BIN时间
extern DateTimeHex_t              gPreBinTime;                             //上次BIN时间

void TopoInit()
{
    memset(&g_TopoArg, 0x00, sizeof(g_TopoArg));
    pthread_rwlock_init(&g_TopoArg.rwLock, NULL);

    return;
}

/* 广播校时 */
uint32 BroadcastTime(TOPO_ARG_T *arg)
{
    MeterPrtlFmt_T data;
    uint8   frame[1024] = {0};
    uint16  frameLen = 0;
    int     ret;
    MESSAGE_INFO_T *msgRecv = NULL;
    uint16  recvLen = 0;
    uint8   recvBuf[MAX_MSG_LEN] = {0};
    uint8   lenOffset = 0;

    memset(&data, 0x00, sizeof(MeterPrtlFmt_T));
    PDS_FMT_DEBUG("拓扑识别广播校时 \n");
    rtc_t prtc;
    DT_Time2RTC((uint32)time(NULL), &prtc);
    data.CtrlCode = 0x08;
    memset(data.MtrAddr, 0x99, 6);
    data.DataLen = 6;
    data.DataBuff[0] = prtc.ss;
    data.DataBuff[1] = prtc.nn;
    data.DataBuff[2] = prtc.hh;
    data.DataBuff[3] = prtc.dd;
    data.DataBuff[4] = prtc.mm;
    data.DataBuff[5] = prtc.yy;

    frameLen = PackMeterPrtl(frame, data);
    ret = mqtt_sendtrans(frame, frameLen, arg->dev[0].port, arg->dev[0].baud, arg->overtime, &arg->msgIndex); /* 载波广播校时不关心port、baud */
    PDS_BUF_DEBUG(frame, frameLen, "拓扑识别广播校时消息发送 msgIndex %d ret %d \n", arg->msgIndex, ret);

    sleep(10);
    recvLen = TransReadQueue(&gTransQueue, recvBuf);

    if (recvLen > 0)
    {
        msgRecv = (MESSAGE_INFO_T *)recvBuf;

        PDS_BUF_DEBUG(recvBuf, recvLen, "拓扑消息接收TransReadQueue msgRecv->index=%u,arg->msgIndex=%u\n", msgRecv->index, arg->msgIndex);
        PDS_BUF_DEBUG(msgRecv->playload, msgRecv->msglen, "收到回码消息 \n");

        if (msgRecv->index != arg->msgIndex)
        {
            PDS_FMT_DEBUG("收到的消息序号 %d 不等于发出的消息序号 %d  \n", msgRecv->index, arg->msgIndex);
        }
        else
        {
            frameLen = get_len_offset(recvBuf, &lenOffset);

            if (frameLen == 0)
            {
                PDS_FMT_DEBUG("收到广播校时回码  \n");
            }
        }
    }

    sleep(30); /* 广播校时报文发送后，等待30秒 */

    return 0;
}

#define BREAKER_USERTYPE_CHECK(userType) \
((userType) == USERTYPE_FRAME_BREAKER || (userType) == USERTYPE_LOW_BREAKER || (userType) == USERTYPE_METERBOX_BREAKER)

/* 获取断路器数量 */
uint32 GetBreakNum(TOPO_ARG_T *arg)
{
    uint32 i = 0;
    uint8 userType;
    arg->breakNum = 0;

    pthread_rwlock_rdlock(&MeterFileAll.rwLock);

    for (i = 0; i < MeterFileAll.MeterCount; i++)
    {
        userType = MeterFileAll.MeterFile[i].basic.userType;

        if (BREAKER_USERTYPE_CHECK(userType))
        {
            arg->breakNum++;
        }
    }

    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    PDS_FMT_DEBUG("获取断路器数量 breakNum=%u \n", arg->breakNum);
    return arg->breakNum;
}

/* 获取点名名单 */
uint32 GetRollCallList(TOPO_ARG_T *arg)
{
    uint32 i = 0;
    uint16 count = 0;
    uint8 userType;
    uint16 sonID = 0; /* 断路器后代节点信息索引 */
    arg->devNum = 0;
    memset(arg->dev, 0x00, sizeof(arg->dev));
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);

    for (i = 0; i < MeterFileAll.MeterCount; i++)
    {
        userType = MeterFileAll.MeterFile[i].basic.userType;
        arg->dev[count].pn = MeterFileAll.MeterFile[i].nIndex;
        arg->dev[count].userType = userType;
        memcpy_r(arg->dev[count].addr, MeterFileAll.MeterFile[i].basic.tsa.add, 6);
        arg->dev[count].port = MeterFileAll.MeterFile[i].basic.port.value;
        count++;

        if (BREAKER_USERTYPE_CHECK(userType))
        {
            arg->dev[count].sonID = sonID;
            sonID++;
        }
    }

    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    arg->devNum = count;
    PDS_FMT_DEBUG("拓扑识别获取点名名单 devNum=%u \n", arg->devNum);
    return count;
}

#define DEV_TYPE_METER      0 /* 点名报文中的设备类型，0:电能表 */
#define DEV_TYPE_BREAKER    1 /* 点名报文中的设备类型，1:断路器 */
uint16  g_RollCallSeq = 0; /* 点名报文中的序号 */

/* 组装点名请求报文 */
uint32 PackRollCall(TOPO_ARG_T *arg, uint8 *frame, uint16 *frameLen, uint16 devIndex)
{
    MeterPrtlFmt_T data = {0};
    uint8 devType = DEV_TYPE_METER;
    uint8 numHigh = g_RollCallSeq >> 8;    /* 数值高位 */
    uint8 numLow = g_RollCallSeq & 0x00FF; /* 数值低位 */
    //PDS_FMT_DEBUG("拓扑识别状态机：触发拓扑 \n");

    /*if (devIndex >= arg->devNum)
    {
        PDS_FMT_DEBUG("所有设备触发拓扑完毕 开始查询拓扑信息 \n");
        //arg->topoState = TOPO_START_METER;
        //arg->devIndex = 0;
        return 1;
    }*/

    /*if (BREAKER_USERTYPE_CHECK(arg->dev[devIndex].userType))
    {
        devType = DEV_TYPE_BREAKER;
    }*/

    memcpy(data.MtrAddr, arg->dev[devIndex].addr, 6);
    //data.CtrlCode = 0x04;
    //uint8 databuf[] = {0x04, 0x00, 0x01, 0x01, 0x3c, 0x02, 0xaa, 0xe9};
    data.CtrlCode = 0x14;
    uint8 databuf[] = {0x06, 0x01, 0x08, 0x09, devType, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, numLow, numHigh, 0x02, 0xaa, 0xe9, 0x90, 0x01, 0x20, 0x03};
    memcpy(data.DataBuff, databuf, sizeof(databuf));
    data.DataLen = sizeof(databuf);
    arg->dev[devIndex].startTime = time(NULL);
    PDS_FMT_DEBUG("设备 pn %d 启动拓扑请求,seq=%u\n",arg->dev[devIndex].pn, g_RollCallSeq);
    g_RollCallSeq++;
    *frameLen = PackMeterPrtl(frame, data);

    return 0;
}

/* 组装查询时间请求报文 */
uint32 PackQueryTime(TOPO_ARG_T *arg, uint8 *frame, uint16 *frameLen, uint16 devIndex)
{
    MeterPrtlFmt_T data = {0};
    //PDS_FMT_DEBUG("拓扑识别状态机：时间补偿 \n");

    memcpy(data.MtrAddr, arg->dev[devIndex].addr, 6);
    data.CtrlCode = 0x11;
    uint8 databuf[] = {0x02, 0x01, 0x00, 0x04};
    memcpy(data.DataBuff, databuf, sizeof(databuf));
    data.DataLen = sizeof(databuf);       
    arg->sendCheckTime = time(NULL);
    PDS_FMT_DEBUG("断路器 pn %d 时间补偿请求 \n", arg->dev[devIndex].pn);
    *frameLen = PackMeterPrtl(frame, data);

    return 0;
}

/* 组装清除拓扑记录请求报文 */
uint32 PackClearRecord(TOPO_ARG_T *arg, uint8 *frame, uint16 *frameLen, uint16 devIndex)
{
    MeterPrtlFmt_T data = {0};
    //PDS_FMT_DEBUG("拓扑识别状态机：时间补偿 \n");

    memcpy(data.MtrAddr, arg->dev[devIndex].addr, 6);
    data.CtrlCode = 0x14;
    uint8 databuf[] = {0x04, 0x03, 0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    memcpy(data.DataBuff, databuf, sizeof(databuf));
    data.DataLen = sizeof(databuf);       
    arg->sendCheckTime = time(NULL);
    PDS_FMT_DEBUG("断路器 pn %d 清除拓扑记录请求 \n", arg->dev[devIndex].pn);
    *frameLen = PackMeterPrtl(frame, data);

    return 0;
}

#define READ_RECORD_NUM 20  /* 一次读取识别记录的数量 */
/* 组装读取识别记录请求报文 */
uint32 PackReadRecord(TOPO_ARG_T *arg, uint8 *frame, uint16 *frameLen, uint16 devIndex)
{
    MeterPrtlFmt_T data = {0};
    uint8 numHigh = (arg->dev[devIndex].recordNum) >> 8;    /* 数值高位 */
    uint8 numLow = (arg->dev[devIndex].recordNum) & 0x00FF; /* 数值低位 */
    
    //PDS_FMT_DEBUG("拓扑识别状态机：时间补偿 \n");

    memcpy(data.MtrAddr, arg->dev[devIndex].addr, 6);
    data.CtrlCode = 0x11;
    uint8 databuf[] = {0x06, 0x03, 0x08, 0x09, READ_RECORD_NUM, numLow, numHigh};
    memcpy(data.DataBuff, databuf, sizeof(databuf));
    data.DataLen = sizeof(databuf);
    arg->sendCheckTime = time(NULL);
    PDS_FMT_DEBUG("断路器 pn %d 读取识别记录请求 \n", arg->dev[devIndex].pn);
    *frameLen = PackMeterPrtl(frame, data);

    return 0;
}

/* 拓扑消息发送函数 */
uint32 TopoSend(TOPO_ARG_T *arg, uint16 devIndex, PackFunc pack)
{
    uint8   frame[1024] = {0};
    uint16  frameLen = 0;
    int     ret;

    if (pack(arg, frame, &frameLen, devIndex) != 0)
    {
        return 1;
    }

    ret = mqtt_sendtrans(frame, frameLen, arg->dev[devIndex].port, arg->dev[devIndex].baud, arg->overtime, &arg->msgIndex);
    PDS_BUF_DEBUG(frame, frameLen, "消息发送 msgindex %d ret %d \n", arg->msgIndex, ret);
    clock_gettime(CLOCK_MONOTONIC, &arg->sendTime);

    return 0;
}

/* 解码点名响应报文 */
uint32 UnpackRollCall(TOPO_ARG_T *arg, MeterPrtlFmt_T *data, uint16 devIndex)
{
    if ((data->CtrlCode == 0x94) && !memcmp(data->MtrAddr, arg->dev[devIndex].addr, 6))
    {
        PDS_FMT_DEBUG("设备 pn %d 启动拓扑成功 \n", arg->dev[devIndex].pn);

        //arg->delay = TOPO_DELAY_TIME;
        //arg->devIndex++;
    }
    else
    {
        PDS_FMT_DEBUG("断路器 pn %d 启动拓扑异常 \n", arg->dev[devIndex].pn);
        return 1;
    }

    return 0;
}

/*******************************************************************************
* 函数名称: CalcTimeOffset
* 函数功能: 解析获取的时间报文，计算补偿时间
* 输入参数: buf 获取的时间信息数据域
         len 数据域长度
* 输出参数: 
* 返 回 值:断路器索引
*******************************************************************************/
void CalcTimeOffset(TOPO_ARG_T *arg, uint8 *buf, uint16 len, uint16 devIndex)
{
    //Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    //uint16 breakindex = gTopologyArg->breakIndex;
    uint32 di = 0;
    uint16 index = 4;
    DateTimeBcd_t breakTmBCD;
    time_t breakTime = 0;
    uint32 commTime = (time(NULL) - arg->sendCheckTime) / 2; /* 单向通讯延时 */
    int timeOffset = 0;
    
    DT_Time2DateTimeBCD(arg->sendCheckTime + commTime, &breakTmBCD); 
    
    if(len<7)
    {
        PDS_FMT_DEBUG(" UnpackTimeInfo error len %d \n",len);
        return;
    }
    
    memcpy(&di, buf, 4);
    if(di != 0X04000102)
    {
        PDS_FMT_DEBUG(" UnpackTimeInfo error di 0X%08x !=0X04000102 \n", di);
        return;
    }

    breakTmBCD.sec = buf[index++];  
    breakTmBCD.min = buf[index++]; 
    breakTmBCD.hour = buf[index++];
    
    DT_DateTimeBCD2Time(&breakTmBCD, &breakTime); 

    timeOffset = arg->sendCheckTime + commTime - breakTime;

    if(abs(timeOffset) > TOPO_INTERVAL)
    {
        arg->dev[devIndex].timeOffset = 0;
    }
    else
    {
        arg->dev[devIndex].timeOffset = timeOffset;
    }
    
    PDS_FMT_DEBUG("理论补偿时间 %ds 实际补偿时间 %ds\n", timeOffset, arg->dev[devIndex].timeOffset);  
}

/* 解码查询时间响应报文，计算补偿时间 */
uint32 UnpackQueryTime(TOPO_ARG_T *arg, MeterPrtlFmt_T *data, uint16 devIndex)
{
    if ((data->CtrlCode == 0x91) && !memcmp(data->MtrAddr, arg->dev[devIndex].addr, 6))
    {
        PDS_FMT_DEBUG("断路器 pn %d 获取时间成功 \n", arg->dev[devIndex].pn);
        CalcTimeOffset(arg, data->DataBuff, data->DataLen, devIndex);
    }else
    {
        PDS_FMT_DEBUG("断路器 pn %d 获取时间异常 \n", arg->dev[devIndex].pn);
        return 1;
    }

    return 0;
}

/* 解码清除拓扑记录响应报文 */
uint32 UnpackClearRecord(TOPO_ARG_T *arg, MeterPrtlFmt_T *data, uint16 devIndex)
{
    if ((data->CtrlCode == 0x94) && !memcmp(data->MtrAddr, arg->dev[devIndex].addr, 6))
    {
        PDS_FMT_DEBUG("设备 pn %d 清除拓扑记录成功 \n", arg->dev[devIndex].pn);
    }
    else
    {
        PDS_FMT_DEBUG("断路器 pn %d 清除拓扑记录异常 \n", arg->dev[devIndex].pn);
        return 1;
    }

    return 0;
}

/* 信号所属相位，0：未知；1：A相；  2：B相；  3：C相；4：三相 */
uint8 GetTopoPhase(uint8 in)
{
    uint8 phase = 0;

    if (in == 1)
    {
        phase = 1;
    }
    else if (in == 2)
    {
        phase = 2;
    }
    else if (in == 3)
    {
        phase = 3;
    }
    else if (in == 4)
    {
        phase = 0xFF;
    }

    return phase;
}

/*******************************************************************************
* 函数名称: SaveTopoRelation
* 函数功能: 解析获取的拓扑信息中的记录，找到断路器父亲节点
* 输入参数: recordtime 记录发生时间
         phase 相位
* 输出参数: 
* 返 回 值:断路器索引
*******************************************************************************/
void SaveTopoRelation(TOPO_ARG_T *arg, uint32 recordTime, uint8 phase, uint16 devIndex)
{
    //TOPO_ARG_T *arg = &g_TopoArg;
    uint32 startTime;
    uint8 i;
    uint8 parentCnt = 0;
    uint8 sonCnt = 0;
    int interval = 0;
    uint16 sonID = 0; /* 断路器后代节点信息索引 */

    //遍历所有断路器，找到发送时间与之匹配的断路器，即收到的为该断路器发出的信号，当前收到信息的断路器为亲代
    for (i = 0; i < arg->devNum; i++)
    {
        //获取除自己外的拓扑信息发送时间，并与收到的拓扑信息时间相比
        if (i == devIndex)
        {
            continue;
        }
        
        startTime = arg->dev[i].startTime;
        interval = recordTime - startTime;
        
        PDS_FMT_DEBUG("%012llx recordtime:%d - starttime:%d = %d, Limit(0,%d) \n", *(uint64 *)arg->dev[i].addr, recordTime, startTime, interval, TOPO_INTERVAL);

        //时间差小于阈值，且不是自己，则放入亲子列表
        if (abs(interval) < TOPO_INTERVAL)
        {
            //亲代角度存储，方便拓扑网络计算
            parentCnt = arg->dev[i].parentNum;
            arg->dev[i].parentNode[parentCnt].devID = devIndex;
            arg->dev[i].parentNode[parentCnt].phase = phase;
            memcpy(arg->dev[i].parentNode[parentCnt].addr, arg->dev[devIndex].addr, 6);
            arg->dev[i].parentNum++;
//            PDS_BUF_DEBUG(gTopologyArg->breakInfo[i].ParentInfo[parentCnt].addr,6,"识别到亲代 来自相位 %d   \n",phase);
//            PDS_BUF_DEBUG(gTopologyArg->breakInfo[i].Addr,6,"子代地址\n");

            //子代角度存储，方便阻抗和线损计算，这地方有问题，这个相位未必就是这个子代的相位
            sonID = arg->dev[devIndex].sonID;
            sonCnt = arg->dev[devIndex].sonNum;
            //arg->dev[devIndex].sonNode[sonCnt].phase = phase;
            arg->sonNode[sonID][sonCnt].phase = phase;
            //memcpy(arg->dev[devIndex].sonNode[sonCnt].addr, arg->dev[i].addr, 6);
            memcpy(arg->sonNode[sonID][sonCnt].addr, arg->dev[i].addr, 6);
            arg->dev[devIndex].sonNum++;

//            PDS_BUF_DEBUG(gTopologyArg->breakInfo[i].Addr,6,"识别到子代 相位 %d   \n",phase);  
//            PDS_BUF_DEBUG(gTopologyArg->breakInfo[breakindex].Addr,6,"亲代地址\n");

            PDS_FMT_DEBUG("识别到后代:%012llx-->%012llx, phase %d \n", *(uint64 *)arg->dev[devIndex].addr, *(uint64 *)arg->dev[i].addr, phase);

            break;
        }
    }

}

/*******************************************************************************
* 函数名称: GetTopoRecord
* 函数功能: 提取识别记录
* 输入参数: buf 获取的拓扑信息报文数据域
         len 数据域长度
* 输出参数: 
* 返 回 值:记录条数
*******************************************************************************/
uint8 GetTopoRecord(TOPO_ARG_T *arg, uint8 *buf, uint16 len, uint16 devIndex)
{
    uint16 offset = 4;   /* 4:设备识别到的信号总数量的字段偏移量 */
    uint16 totalNum;    /* 设备识别到的信号总数量 */
    uint16 recordNum = 0;   /* 本次回复的识别信号个数 */
    uint16 startIndex = 0;  /* 起始序号 */
    uint32 i = 0;
    uint32 di =0;
    OOP_DATETIME_S_T time;
    time_t recordTime;
    uint8 phase;
    //Topology_Arg_T *gTopologyArg = &gCalcArg.topologyArg;
    //uint16 breakindex = gTopologyArg->breakIndex;
    PDS_FMT_DEBUG("GetTopoRecord in\n");
    
    if (len < 5)
    {
        PDS_FMT_DEBUG(" GetTopoRecord error len %u \n", len);
        return 0;
    }

    memcpy(&di, buf, 4);

    if (di != 0x09080306)
    {
        PDS_FMT_DEBUG(" GetTopoRecord error di 0x%08x != 0x09080306 \n", di);
        return 0;
    }
    
    totalNum = *(uint16 *)&buf[offset];
    offset += 2;
    recordNum = buf[offset++];
    startIndex = *(uint16 *)&buf[offset];
    offset += 2; /* 偏移掉起始序号2个字节 */
    PDS_FMT_DEBUG(" GetTopoRecord len=%u,totalNum=%u,recordNum=%u,startIndex=%u \n", len, totalNum, recordNum, startIndex);

    //len = 4+2+1+2+(6+1+2+2)*recordNum
    if (len != 9 + 11 * recordNum)
    {
        PDS_FMT_DEBUG(" GetTopoRecord error len %u recordNum %d \n", len, recordNum);
        return 0;
    }

    PDS_FMT_DEBUG("%012llx 本次拓扑记录数 %d \n", *(uint64*)arg->dev[devIndex].addr, recordNum);

    arg->dev[devIndex].recordNum += recordNum;
    
    for (i = 0; i < recordNum; i++)
    {
        PDS_FMT_DEBUG("%012llx 第%u条拓扑记录\n", *(uint64*)arg->dev[devIndex].addr, i + startIndex);
        time.second = buf[offset++];
        time.minute = buf[offset++];
        time.hour = buf[offset++];
        time.day = buf[offset++];
        time.month = buf[offset++];
        time.year = 2000 + buf[offset++];

        DT_DateTimeShort2Time(&time, &recordTime);
        phase = GetTopoPhase(buf[offset++]);
        offset += 4; /* 偏移掉信号强度S1、信号强度S2字段共4个字节 */
        PDS_FMT_DEBUG("recordTime=%u,sec=%u,min=%u,hour=%u,day=%u,month=%u,year=%u,phase=%u\n", 
                      recordTime, time.second, time.minute, time.hour, time.day, time.month, time.year, phase);
        SaveTopoRelation(arg, recordTime + arg->dev[devIndex].timeOffset, phase, devIndex);
        //GetBreakSon(gTopologyArg, recordTime + gTopologyArg->breakInfo[breakindex].timeoffset, phase);
    }

    if (startIndex + recordNum >= totalNum)
    {
        arg->dev[devIndex].readRecordFinish = 1;
        PDS_FMT_DEBUG("%012llx 读取记录完成\n", *(uint64*)arg->dev[devIndex].addr);
    }

    return recordNum;
}

/* 解码读取识别记录响应报文 */
uint32 UnpackReadRecord(TOPO_ARG_T *arg, MeterPrtlFmt_T *data, uint16 devIndex)
{
    PDS_FMT_DEBUG("UnpackReadRecord in\n");
    if ((data->CtrlCode == 0x91) && !memcmp(data->MtrAddr, arg->dev[devIndex].addr, 6))
    {
        if (GetTopoRecord(arg, data->DataBuff, data->DataLen, devIndex) < READ_RECORD_NUM)
        {
            PDS_FMT_DEBUG("%012llx 拓扑记录总数 %d \n", *(uint64 *)arg->dev[devIndex].addr, arg->dev[devIndex].recordNum);
        }
        //PDS_FMT_DEBUG("设备 pn %d 清除拓扑记录成功 \n", arg->dev[devIndex].pn);
    }
    else
    {
        PDS_FMT_DEBUG("断路器 pn %d 读取识别记录异常 \n", arg->dev[devIndex].pn);
        return 1;
    }

    PDS_FMT_DEBUG("UnpackReadRecord out\n");
    return 0;
}

#define TOPO_RECV_OVER_TIME 60  /* 拓扑响应超时时间60秒 */
#define TOPO_SEND_RETRY_CNT 2   /* 拓扑发送重试次数 */
/* 拓扑消息接收函数 */
uint32 TopoRecv(TOPO_ARG_T *arg, uint16 devIndex, UnpackFunc unpack, PackFunc pack)
{
    int ret = 0;
    MeterPrtlFmt_T data;
    memset(&data, 0x00, sizeof(MeterPrtlFmt_T));
    //uint16 index = gTopologyArg->breakIndex;
    uint8   lenOffset = 0;
    MESSAGE_INFO_T *msgRecv = NULL;
    uint16  recvLen = 0;
    uint8   recvBuf[MAX_MSG_LEN] = {0};
    uint16  frameLen = 0;
    struct timespec curTime;
    uint32  sendRetryCnt = TOPO_SEND_RETRY_CNT;

    while (1)
    {
        usleep(10000); /* 睡10ms */
        recvLen = TransReadQueue(&gTransQueue, recvBuf);

        if (recvLen > 0)
        {
            msgRecv = (MESSAGE_INFO_T *)recvBuf;

            PDS_BUF_DEBUG(recvBuf, recvLen, "拓扑消息接收TransReadQueue msgRecv->index=%u,arg->msgIndex=%u\n", msgRecv->index, arg->msgIndex);
            PDS_BUF_DEBUG(msgRecv->playload, msgRecv->msglen, "收到回码消息 \n");

            if (msgRecv->index != arg->msgIndex)
            {
                PDS_FMT_DEBUG("收到的消息序号 %d 不等于发出的消息序号 %d  \n", msgRecv->index, arg->msgIndex);
                continue;
            }

            frameLen = get_len_offset(msgRecv->playload, &lenOffset);
            PDS_FMT_DEBUG("get_len_offset frameLen=%u,lenOffset=%u\n", frameLen, lenOffset);

            if (frameLen == 0)
            {
                PDS_FMT_DEBUG("回码报文为空，发送重试 \n");
            }
            else
            {
                PDS_FMT_DEBUG("UnpackProtocol_645 in\n");
                ret = UnpackProtocol_645(&msgRecv->playload[lenOffset], frameLen, &data);
                PDS_FMT_DEBUG("UnpackProtocol_645 out\n");

                if (ret != 0)
                {
                    PDS_FMT_DEBUG("645报文解析失败，发送重试 ret %d \n", ret);
                }
                else
                {
                    PDS_FMT_DEBUG("645报文解析成功\n");
                    if (unpack(arg, &data, devIndex) == 0)
                    {
                        return 0;
                    }

                    PDS_FMT_DEBUG("解码响应报文失败，发送重试 \n");
                }
            }
        }
        else /* 没收到报文，进入超时检查 */
        {
            clock_gettime(CLOCK_MONOTONIC, &curTime);

            if (curTime.tv_sec - arg->sendTime.tv_sec > TOPO_RECV_OVER_TIME)
            {
                PDS_FMT_DEBUG("响应报文超时，发送重试 \n");
            }
            else /* 没有超时，继续等待 */
            {
                continue;
            }
        }

        if (sendRetryCnt == 0)
        {
            PDS_FMT_DEBUG("没有发送重试次数\n");
            break;
        }

        sleep(10);
        //PDS_FMT_DEBUG("开始发送重试\n");
        TopoSend(arg, devIndex, pack);
        sendRetryCnt--;
    }

    return 1;
}

/* 清除断路器中的拓扑记录 */
uint32 ClearRecordProc(TOPO_ARG_T *arg)
{
    uint32 i = 0;

    PDS_FMT_DEBUG("拓扑识别清除断路器中的拓扑记录 \n");

    for (i = 0; i < arg->devNum; i++)
    {
        /* 只对断路器清除记录 */
        if (BREAKER_USERTYPE_CHECK(arg->dev[i].userType))
        {
            TopoSend(arg, i, PackClearRecord);
            //sleep(10);
            TopoRecv(arg, i, UnpackClearRecord, PackClearRecord);
        }
    }

    return 0;
}

/* 计算时间补偿 */
uint32 CalcTimeOffsetProc(TOPO_ARG_T *arg)
{
    uint32 i = 0;

    PDS_FMT_DEBUG("拓扑识别计算时间补偿 \n");

    for (i = 0; i < arg->devNum; i++)
    {
        /* 只对断路器计算时间补偿 */
        if (BREAKER_USERTYPE_CHECK(arg->dev[i].userType))
        {
            TopoSend(arg, i, PackQueryTime);
            //sleep(10);
            TopoRecv(arg, i, UnpackQueryTime, PackQueryTime);
        }
    }

    return 0;
}

/* 点名处理函数 */
uint32 RollCallProc(TOPO_ARG_T *arg)
{
    uint32 i = 0;

    PDS_FMT_DEBUG("拓扑识别点名处理 \n");
    pthread_rwlock_wrlock(&arg->rwLock);
    arg->readTopoDataFlag = 1; /* 点名时，允许读拓扑电流识别数据 */
    pthread_rwlock_unlock(&arg->rwLock);

    for (i = 0; i < arg->devNum; i++)
    {
        TopoSend(arg, i, PackRollCall);
        //sleep(10);
        TopoRecv(arg, i, UnpackRollCall, PackRollCall);

        /* 一个设备点完名后，等待一会再点下一个设备 */
        sleep(TOPO_DELAY_TIME);
        //sleep(30);
    }

    return 0;
}

/* 读取断路器中的拓扑记录 */
uint32 ReadRecordProc(TOPO_ARG_T *arg)
{
    uint32 i = 0;

    PDS_FMT_DEBUG("拓扑识别读取断路器中的拓扑记录 \n");

    for (i = 0; i < arg->devNum; i++)
    {
        /* 只对断路器计算时间补偿 */
        if (BREAKER_USERTYPE_CHECK(arg->dev[i].userType))
        {
            while (arg->dev[i].readRecordFinish != 1)
            {
                TopoSend(arg, i, PackReadRecord);
                //sleep(10);
                if (TopoRecv(arg, i, UnpackReadRecord, PackReadRecord) != 0)
                {
                    break;
                }
            }
        }
    }

    return 0;
}

/* 根据信息点号pn查询设备索引devIndex */
uint32 GetDevIndexByPn(uint16 *devIndex, uint16 pn)
{
    TOPO_ARG_T *arg = &g_TopoArg;
    uint32  i = 0;

    for (i = 0; i < arg->devNum; i++)
    {
        if (arg->dev[i].pn == pn)
        {
            *devIndex = i;
            return 0;
        }
    }

    return 1;
}

/* 从数据中心读取拓扑电流识别数据 */
int ReadTopoData(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int     ret = 0;
    uint32  readlen = 0;
    NOMAL_OAD_T normalOad = {0};
    Oop_Identify_Result result = {0};
    uint32  i = 0;
    TOPO_ARG_T *arg = &g_TopoArg;
    time_t curTime = 0;
    uint16  devIndex = 0;
    OOP_DATETIME_S_T *tmp = NULL;

    if (0x4E070200 != value)
    {
        return -1;
    }

    pthread_rwlock_wrlock(&arg->rwLock);

    if (arg->readTopoDataFlag == 0)
    {
        pthread_rwlock_unlock(&arg->rwLock);
        return 0;
    }

    normalOad.logicId   = logicId;
    normalOad.infoNum   = infoNum;
    normalOad.oad.value = value;
    PDSDB_FMT_DEBUG("收到拓扑电流识别的数据变更事件 Logicid %d infonum %d ", logicId, infoNum);

    ret = db_read_nomal(clientid, &normalOad, sizeof(result), (uint8 *)&result, &readlen);

    if (0 != ret || readlen != sizeof(result))
    {
       PDS_FMT_DEBUG("读取拓扑电流识别数据失败\n");
       pthread_rwlock_unlock(&arg->rwLock);
       return 0;
    }

    if (result.Identifynum == 0 || result.Identifynum > 4)
    {
        PDS_FMT_DEBUG("拓扑电流识别数量异常 num=%u\n", result.Identifynum);
        pthread_rwlock_unlock(&arg->rwLock);
        return 0;
    }

    for (i = 0; i < result.Identifynum; i++)
    {
        if (GetDevIndexByPn(&devIndex, infoNum) != 0)
        {
            PDS_FMT_DEBUG("没有找到相应的设备索引 infoNum=%u\n", infoNum);
            continue;
        }

        tmp = &result.IdentifyInformation[i].Suctime;
        PDS_FMT_DEBUG("ReadTopoData year=%u,month=%u,day=%u,hour=%u,min=%u,sec=%u,week=%u\n", tmp->year, tmp->month, tmp->day, tmp->hour, tmp->minute, tmp->second, tmp->week);
        ret = DT_DateTimeShort2Time(&result.IdentifyInformation[i].Suctime, &curTime);
        PDS_FMT_DEBUG("ReadTopoData ret=%d,curTime=%u\n", ret, curTime);
        SaveTopoRelation(arg, curTime + arg->dev[devIndex].timeOffset, result.IdentifyInformation[i].signalphase, devIndex);
    }

    pthread_rwlock_unlock(&arg->rwLock);
    return 0;
}

void TopoSwapFunc(TOPO_ARG_T *arg, NODE_INFO_T *node, uint32 id)
{
    NODE_INFO_T tmpNode;  
    uint16 devID1 = (node + id)->devID;
    uint16 devID2 = (node + id + 1)->devID;
    TOPO_DEV_T *dev1 = &arg->dev[devID1];
    TOPO_DEV_T *dev2 = &arg->dev[devID2];

    //升序
    if (dev1->sonNum > dev2->sonNum)
    {
        memcpy(&tmpNode, node + id, sizeof(NODE_INFO_T));
        memcpy(node + id, node + id + 1, sizeof(NODE_INFO_T));
        memcpy(node + id + 1, &tmpNode, sizeof(NODE_INFO_T));
    }

    return;
}

/*******************************************************************************
* 函数名称: NodeBubbleSort
* 函数功能: 冒泡排序
* 输入参数: 
* 输出参数: 
* 返 回 值:
*******************************************************************************/
void NodeBubbleSort(TOPO_ARG_T *arg, NODE_INFO_T *node, uint16 num)
{
    uint32 i = 0;
    uint32 j = 0;

    /* 数量大于1个才需要排序 */
    if (num <= 1)
    {
        return;
    }

    for (i = 0; i < num - 1; i++)
    {
        for (j = 0; j < num - 1 - i; j++)
        {
            TopoSwapFunc(arg, node, j);
        }
    }

    return;
}

/* 查找父节点函数 */
uint32 FindParentNodeProc(TOPO_ARG_T *arg)
{
    uint32 i = 0;

    sleep(60); /* 因为拓扑电流信号识别需要一定时间，等待60秒 */
    PDS_FMT_DEBUG("拓扑识别查找父节点 \n");
    pthread_rwlock_wrlock(&arg->rwLock);
    arg->readTopoDataFlag = 0;  /* 找父节点时，禁止读拓扑电流识别数据，避免节点数据错乱 */
    pthread_rwlock_unlock(&arg->rwLock);

    for (i = 0; i < arg->devNum; i++)
    {
        NodeBubbleSort(arg, arg->dev[i].parentNode, arg->dev[i].parentNum);
    }

    return 0;
}

/*******************************************************************************
* 函数名称: SaveTopoResultDB
* 函数功能: 将拓扑识别结果存入数据中心
* 输入参数: 
* 输出参数: 
* 返 回 值:0 写入成功 其他失败
*******************************************************************************/
void SaveTopoResultDB(TOPO_ARG_T *arg)
{
    int ret;
    TOPO_DEV_T *dev = NULL;
    OOP_NodeTopologyInfo_T TopoInfo;
    uint32  i = 0;
    OOP_OAD_U oad ={0x60F10200};

    PDS_FMT_DEBUG("拓扑识别结果存入数据中心 \n");

    for (i = 0; i < arg->devNum; i++)
    {
        dev = &arg->dev[i];
        //BreakInfo = &gTopologyArg->breakInfo[i];
        PDS_FMT_DEBUG("拓扑设备 Pn %u %012llx, 亲代个数：%u, 后代个数：%u\n", dev->pn, *(uint64*)dev->addr, dev->parentNum, dev->sonNum);

        if (dev->parentNum == 0)
        {
        
            //BreakInfo->nodeType = NODE_ROOT;
            memset(&TopoInfo, 0x00, sizeof(OOP_NodeTopologyInfo_T));
            TopoInfo.NodeDevAddr.len = 5;
            memcpy_r(TopoInfo.NodeDevAddr.add, dev->addr, 6);
            TopoInfo.NodePhaseInfo = 0;
            TopoInfo.ParentNodeDevAddr.len = 5;
            memcpy_r(TopoInfo.ParentNodeDevAddr.add, LocalAddr, 6);
            TopoInfo.ParentNodeLineAddr.len = 5;
            memcpy_r(TopoInfo.ParentNodeLineAddr.add, LocalAddr, 6);
            ret = WriteNormalData(arg->clientId, (uint8 *)&TopoInfo, sizeof(OOP_NodeTopologyInfo_T), oad, dev->pn);

            if (dev->sonNum == 0)
            {
                PDS_FMT_DEBUG("孤节点 Pn %d %012llx<--%012llx, phase %d ret %d  \n", dev->pn, *(uint64 *)dev->addr, *(uint64 *)LocalAddr, TopoInfo.NodePhaseInfo, ret);
            }
            else
            {
                PDS_FMT_DEBUG("根节点 Pn %d %012llx<--%012llx, phase %d ret %d  \n", dev->pn, *(uint64 *)dev->addr, *(uint64 *)LocalAddr, TopoInfo.NodePhaseInfo, ret);
            }
        }
        else
        {
            //parentBreakInfo = &gTopologyArg->breakInfo[BreakInfo->ParentInfo[0].breakID];
            //BreakInfo->nodeType = NODE_LEAF;
            memset(&TopoInfo,0x00,sizeof(OOP_NodeTopologyInfo_T));
            TopoInfo.NodeDevAddr.len = 5;
            memcpy_r(TopoInfo.NodeDevAddr.add, dev->addr, 6);
            TopoInfo.NodePhaseInfo = dev->parentNode[0].phase; //相位来自父亲
            TopoInfo.ParentNodeDevAddr.len = 5;
            memcpy_r(TopoInfo.ParentNodeDevAddr.add, dev->parentNode[0].addr, 6);
            TopoInfo.ParentNodeLineAddr.len = 5;
            memcpy_r(TopoInfo.ParentNodeLineAddr.add, dev->parentNode[0].addr, 6);
            ret = WriteNormalData(arg->clientId, (uint8 *)&TopoInfo, sizeof(OOP_NodeTopologyInfo_T), oad, dev->pn);

            if (dev->sonNum == 0)
            {
                PDS_FMT_DEBUG("叶子节点 Pn %u %012llx<--%012llx, phase %u ret %u\n", dev->pn, *(uint64 *)dev->addr, *(uint64 *)dev->parentNode[0].addr, TopoInfo.NodePhaseInfo, ret);
            }
            else
            {
                PDS_FMT_DEBUG("枝干节点 Pn %u %012llx<--%012llx, phase %u ret %u\n", dev->pn, *(uint64 *)dev->addr, *(uint64 *)dev->parentNode[0].addr, TopoInfo.NodePhaseInfo, ret);
            }
            
        }
    }

    memset(&TopoInfo, 0x00, sizeof(OOP_NodeTopologyInfo_T));
    TopoInfo.NodeDevAddr.len = 5;
    memcpy_r(TopoInfo.NodeDevAddr.add, LocalAddr, 6);
    TopoInfo.NodePhaseInfo = 0;
    TopoInfo.ParentNodeDevAddr.len = 5;
    memcpy_r(TopoInfo.ParentNodeDevAddr.add, LocalAddr, 6);
    TopoInfo.ParentNodeLineAddr.len = 5;
    memcpy_r(TopoInfo.ParentNodeLineAddr.add, LocalAddr, 6);
    ret = WriteNormalData(arg->clientId, (uint8 *)&TopoInfo, sizeof(OOP_NodeTopologyInfo_T), oad, 0);
    PDS_FMT_DEBUG("辅节点 Pn 0 %012llx-->%012llx, phase %d ret %d  \n", *(uint64 *)LocalAddr, *(uint64 *)LocalAddr, TopoInfo.NodePhaseInfo, ret);

    //数据中心存储完成后刷新一遍全局变量，便于后面的计算
    //topoInfoUpdate();
    
    return;
}

/* 清空数据中心所有拓扑识别信息 */
int ClearTopoData(DB_CLIENT clientId)
{
    CLEAR_DATA_T clearData; 
    MEMZERO(&clearData, sizeof(clearData));

    /*清空所有拓扑信息*/
    clearData.recordOAD.logicId = 0x00;
    clearData.recordOAD.optional = 0;
    clearData.recordOAD.road.oadMain.value = 0x00;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = 0x60F10200;
    clearData.bClassTag = FALSE;
	clearData.recordOAD.classtag = CLASS_DATA_NORMAL;
    clearData.recordOAD.infoNum = 0xffff;
    return db_clear_oad_data(clientId, &clearData);
}

#define TRANS_OVER_TIME 60 /* 透传超时时间60秒 */
/* 拓扑识别处理函数 */
uint32 TopoProc(TOPO_ARG_T *arg)
{
    if (arg->isInTopo == 0)
    {
        return 1;
    }

    PDS_FMT_DEBUG("拓扑识别处理函数 \n");
    arg->overtime = TRANS_OVER_TIME;

    if (GetBreakNum(arg) == 0)
    {
        PDS_FMT_DEBUG("当前档案中未识别到断路器，无法进行拓扑识别功能！ \n");
        arg->isInTopo = 0;
        arg->isFinish = 0;
        return 1;
    }

    if (GetRollCallList(arg) == 0)
    {
        PDS_FMT_DEBUG("当前档案中未识别到点名设备，无法进行拓扑识别功能！ \n");
        arg->isInTopo = 0;
        return 1;
    }

    if (ClearTopoData(arg->clientId) == 0)
    {
        PDS_FMT_DEBUG("删除数据中心全部拓扑信息成功！ \n");
    }

    if (BroadcastTime(arg) != 0)
    {
        arg->isInTopo = 0;
        return 1;
    }

    if (ClearRecordProc(arg) == 0)
    {
        PDS_FMT_DEBUG("清除全部断路器中的拓扑记录成功！ \n");
    }

    if (CalcTimeOffsetProc(arg) != 0)
    {
        arg->isInTopo = 0;
        return 1;
    }

    if (RollCallProc(arg) != 0)
    {
        arg->isInTopo = 0;
        return 1;
    }

    if (ReadRecordProc(arg) != 0)
    {
        arg->isInTopo = 0;
        return 1;
    }

    if (FindParentNodeProc(arg) != 0)
    {
        arg->isInTopo = 0;
        return 1;
    }

    SaveTopoResultDB(arg);
    arg->isInTopo = 0;
    arg->isFinish = 1;

    return 0;
}

void TopoThreadInit(TOPO_ARG_T *arg)
{
    memset(arg, 0x00, sizeof(TOPO_ARG_T));
    pthread_rwlock_init(&arg->rwLock, NULL);
    gCalcSock = task_msg_init(PDS_CALC_IPC);

    if (gCalcSock < 0)
    {
        PDS_FMT_DEBUG("创建PDS_CALC_IPC失败 ！！ \n");
        return ;
    }

    task_msg_settime(gCalcSock, 0, 100000);
    DT_Time2DateTimeHex(time(NULL), &gCurBinTime);
    memcpy(&gPreBinTime, &gCurBinTime, sizeof(DateTimeHex_t));
    arg->clientId = db_client_create(DB_PDS_AGENT, DB_PDS_CLIENT_COMMON);
    g_RollCallSeq = time(NULL) & 0xFFFF;

    if (arg->clientId < 0)
    {
        PDS_FMT_DEBUG("DB_PDS_CLIENT_COMMON db_client_create失败 ！！ \n");
        return;
    }
    //LoadTopoInfoFile(&gCalcArg.topologyArg);
}

/**********************************************************************
* @name      : CheckTopoStatus
* @brief     ：检查拓扑状态
* @param[in] ：
* @param[out]：
* @return    ：

**********************************************************************/
void CheckTopoStatus(TOPO_ARG_T *arg, uint8 *buf, uint16 len)
{
    if (len != sizeof(uint16))
    {
        PDS_FMT_DEBUG("checkTopology len=%d error \n", len);
        return;
    }

    uint16 topoTime;
    memcpy(&topoTime, buf, len);

    if (arg->isInTopo == 1)
    {
        PDS_FMT_DEBUG("当前正在拓扑识别过程中，不响应拓扑启动命令 \n");
        return;
    }

    arg->isInTopo = 1;
}

/*******************************************************************************
* 函数名称: TopoThread
* 函数功能: 拓扑识别线程
* 输入参数: 
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
void *TopoThread(void *arg)
{
    int recvLen;
    uint8 recvBuf[TASK_MSG_MAX_LEN] = {0};
    uint32 sender;
    TOPO_ARG_T *topoArg = &g_TopoArg;
    char threadName[20];

    sprintf(threadName, "TopoThread");
    prctl(PR_SET_NAME, threadName);
    TopoThreadInit(topoArg);

    for(;;)
    {
        recvLen = task_recv_msg(gCalcSock, recvBuf, TASK_MSG_MAX_LEN, &sender);

        if (recvLen > 0)
        {
            TASK_MSG_t *msg = (TASK_MSG_t *)recvBuf;

            switch (msg->msg_id)
            {
                case MSG_PDS_TOPOSTART:
                {
                    PDS_FMT_DEBUG("收到启动拓扑消息 \n");
                    CheckTopoStatus(topoArg, msg->msg, msg->msglen);
                }
                    break;
                default:
                    break;
            }

            memset(recvBuf, 0x00, sizeof(recvBuf));
        }

        TopoProc(topoArg);
    }
}

#endif
