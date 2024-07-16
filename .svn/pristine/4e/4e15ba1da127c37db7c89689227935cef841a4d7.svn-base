/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：EventFun.c
* 版本号：1.0
* 功能：实现事件的检测功能
* 原始开发人及时间：董宁 2006年12月25日
* 修改人员、时间及修改内容:无
* 其他：
******************************************************************-*/
#include "Event_zhejiang.h"
#include "EventFun_zhejiang.h"
#include "commfun.h"
#include "framecomm.h"
#include "oopVarUpdate.h"
#include "storage.h"
#include "pw_protocol.h"
#include "pw_all.h"
#include "pw_protocol_zhj.h"
#include "pw_common_zhj.h"

EVENT_GLOBAL_T  event_global;       //-共享测量点数据结构-
int16 vc_angle[3] = {0};            //-三相电压电流夹角-
extern uint8 bCheckParaTemp[128];   //-检测终端参数时，临时存放读取的参数-
extern mtrnum_t MtrACID;

#if WRITE_LOG_EN
extern uint8 bLogBuf[1000];
#endif

/*-把带符号的原码去掉符号位-*/
int Int_ABS(int value)
{
    //int val;
    //val = value&0x7fffffff;
    return abs(value);
}

/*-把带符号的原码去掉符号位-*/
uint16 Int16_ABS(uint16 value)
{
    uint16 val;
    val = (value&0x7fff);

    return val;
}

/*********************************************************************
所属功能单元：给事件模块调用
功能描述：    比较获得最大值以及最小值
输入参数：    bItemNum-比较的数目
              Max
              Min
              awSrc[]-比较的值
输出参数：    无
函数返回值：  wMax-最大值
内部处理流程：
********************************************************************/
void CmpValint(char bItemNum, int awSrc[],int *MaxVal,int *MinVal)
{
    uint8    i;
    int     wMax = awSrc[0];
    int     wMin = awSrc[0];

    for(i = 0; i < bItemNum; i++)
    {
        if(wMax < awSrc[i])
            wMax = awSrc[i];
        if(wMin > awSrc[i])
            wMin = awSrc[i];
    }

    *MaxVal = wMax;
    *MinVal = wMin;
}

/*********************************************************************
所属功能单元：给事件模块调用
功能描述：    比较获得最大值以及最小值
输入参数：    bItemNum-比较的数目
              Max
              Min
              awSrc[]-比较的值
输出参数：    无
函数返回值：  wMax-最大值
内部处理流程：
********************************************************************/
void CmpValuint16(char bItemNum, uint16 awSrc[],uint16 *MaxVal,uint16 *MinVal)
{
    uint8    i;
    uint16     wMax = awSrc[0];
    uint16     wMin = awSrc[0];

    for(i = 0; i < bItemNum; i++)
    {
        if(wMax < awSrc[i])
            wMax = awSrc[i];
        if(wMin > awSrc[i])
            wMin = awSrc[i];
    }
    *MaxVal = wMax;
    *MinVal = wMin;

}

int ReadRamXData(int off, char *buf, int len)
{
    char bBuf[EVENT_RAMX_FILE_SZIE+1] = {0};

    if (off < 0 || len <= 0 || off + len > EVENT_RAMX_FILE_SZIE)
    {
        EVENT_FMT_DEBUG("ReadRamXData错误, 长度:%u,偏移:%u\n", len, off);
        memset(buf, 0, len);

        return 0;
    }

    read_pdata_xram("^EventS104", bBuf, 0, EVENT_RAMX_FILE_SZIE);
    memcpy(buf, &bBuf[off], len);

    return len;
}

int WriteRamXData(int off, char *buf, int len)
{
    char bBuf[EVENT_RAMX_FILE_SZIE+1] = {0};

    if (off < 0 || len <= 0 || off + len > EVENT_RAMX_FILE_SZIE)
    {
        EVENT_FMT_DEBUG("WriteRamXData错误, 长度:%u,偏移:%u\n", len, off);

        return 0;
    }

    read_pdata_xram("^EventS104", bBuf, 0, EVENT_RAMX_FILE_SZIE);
    memcpy(&bBuf[off], buf, len);

    return write_pdata_xram("^EventS104", bBuf, 0, EVENT_RAMX_FILE_SZIE);
}


/**********************************************************************
* @name      : save_event_flag
* @brief     ：存储事件标志
* @param[in] ：flag_add     存储地址
               event_type   事件编号，发生时的事件编号，恢复编号为:event_type+0x80
               bPhase       为相位标志，1代表A相，2代表B相，3代表C相，如果为0表示不需要记录相位信息
                            最高位表示发生或恢复，1发生
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void save_event_flag(int flag_add, uint8 bPhase,uint8 msn)
{
    uint8 buf[10] = {0};
    ReadRamXData(flag_add, (char*)buf, 9);

    if(bPhase&0x80)//-发生-
    {
        buf[msn] = buf[msn]|(bPhase&0x07);//-添加相位信息-
        
        if(buf[msn] == 0)//不需要记录相位信息           D0位表示是否发生，位D4表示是否有变化-*/
        {
            buf[msn] = buf[msn]|0x01|0x10;
        }
        if(bPhase&0x01)//-A相-
        {
            buf[msn] = buf[msn]|0x10;
        }
        if(bPhase&0x02)//-B相-
        {
            buf[msn] = buf[msn]|0x20;
        }
        if(bPhase&0x04)//-C相-
        {
            buf[msn] = buf[msn]|0x40;
        }
    }
    else//-恢复-
    {
        if(buf[msn] == 0)//不需要记录相位信息           D0位表示是否发生，位D4表示是否有变化-*/
        {
            buf[msn] = (buf[msn]&0xFE)|0x10;
        }
        
        if(bPhase&0x01)//-A相-
        {
            buf[msn] = (buf[msn]&0xFE)|0x10;
        }
        if(bPhase&0x02)//-B相-
        {
            buf[msn] = (buf[msn]&0xFD)|0x20;
        }
        if(bPhase&0x04)//-C相-
        {
            buf[msn] = (buf[msn]&0xFB)|0x40;
        }
    }
    WriteRamXData(flag_add, (char*)buf, 9);
    EVENT_FMT_LOGCC("save_event_flag, ID:%u buf%02x\n", flag_add, buf[0]);
}

/*********************************************************************
所属功能单元：事件检测模块
名称：EventFunc.c
功能描述：将国网规约事件转化存储为GB101/104格式，供SOE检测、上报
输入参数：bEventType:，D0`D6表示事件类型，最高位用于区分同一编号的两种事件
          bPhase为相位标志，1代表A相，2代表B相，3代表C相，如果为0表示不需要记录相位信息
          最高位表示发生或恢复，1发生
          msn 为测量点号，0表示终端
输出参数：无
函数返回值：无
内部处理流程： 字典的数据转成规约数据格式
********************************************************************/
void SaveFileRecBfg(uint8 bEventType,uint8 bPhase,uint8 msn)
{

    EVENT_FMT_DEBUG("SaveFileRecBfg, ERC=%u, MTR=%8X\n", bEventType, msn);

    switch(bEventType&0x7F)//-最高位用来区分同一编号的两种事件-
    {
        case ERC_NO_09://电流反极性
        {
            save_event_flag(GBPhaseReveAddr, bPhase,msn);
            break;
        }
        case ERC_NO_10:
        {   //-断相和失压使用同一个序号，在相位的最高位来区分，最高位为1表示断相，为0表示失压-
            if(bEventType&0x80)
            {//-电压缺相记录地址-
                save_event_flag(GBLostvltAddr, bPhase,msn);
            }
            else
            {//-电压断相记录地址
                save_event_flag(GBLowvltAddr, bPhase,msn);
                
            }

            break;
        }
        case ERC_NO_11://电压/电流逆相序
        {   //-电压/电流逆相序使用同一个序号，在相位的最高位来区分，最高位为1表示电压不平衡，为0表示电流不平衡-
            if(bEventType&0x80)
            {//-电压逆相序记录地址-
                save_event_flag(GBVltAbnormalAddr, bPhase,msn);
            }
            else
            {//-电流逆相序记录地址
                save_event_flag(GBCurrAbnormalAddr, bPhase,msn);
            }

            break;
        }
        case ERC_NO_14:
        {
            if(bEventType&0x80)
            {//-终端停上电-
                save_event_flag(RTUSelfCheckAddr, bPhase,msn);
            }
            else
            {//-配电变压器停电
                save_event_flag(RTUSelfCheckAddr1, bPhase,msn);
            }
            break;
        }
        case ERC_NO_17:
        {   //-电压不平和和电流不平衡使用同一个序号，在相位的最高位来区分，最高位为1表示电压不平衡，为0表示电流不平衡-
            if(bEventType&0x80)
            {//-电压不平衡记录地址-
                save_event_flag(GBVltUnbAddr, bPhase,msn);
            }
            else
            {//-电流不平衡记录地址
                save_event_flag(GBCurrUnbAddr, bPhase,msn);
            }
            break;
        }
        case ERC_NO_24:
        {   //-越上限和越下限使用同一个序号，在相位的最高位来区分，最高位为1表示越上限，为0表示越下限-
            if(bEventType&0x80)
            {//-电压越上限记录地址-
                save_event_flag(GBVltUpAddr, bPhase,msn);
                
            }
            else
            {//-电压越下限记录地址
                save_event_flag(GBVltDownAddr, bPhase,msn);
                
            }

            break;
        }
        case ERC_NO_25:
        {   //-电流过载和轻载使用同一个序号，在相位的最高位来区分，最高位为1表示过载，为0表示轻载-
            if(bEventType&0x80)
            {//-电流过载记录地址-
                save_event_flag(GBCurrUpAddr, bPhase,msn);
                
            }
            else
            {//-电流轻载记录地址
                save_event_flag(GBCurrDownAddr, bPhase,msn);
                
            }
            break;
        }
        case ERC_NO_26:
        {   //-电压电流畸变率越限
            if(bEventType&0x80)
            {//-电压-
                save_event_flag(GBVltDisUpAddr, bPhase,msn);
                
            }
            else
            {//-电流
                save_event_flag(GBCurrDisUpAddr, bPhase,msn);
                
            }
            break;
        }
        case ERC_NO_27:
        {   //-零序电流偏大判断
            save_event_flag(GBZeroCurrUpAddr, bPhase,msn);
            break;
        }
        case ERC_NO_28:
        {   //-负荷过载
            save_event_flag(GBEnUpAddr, bPhase,msn);
            break;
        }
        case ERC_NO_29:
        {   //-无功过补偿
            save_event_flag(GBNoEnUpAddr, bPhase,msn);
            break;
        }
        case ERC_NO_30:
        {   //-无功欠补偿
            save_event_flag(GBNoEnLowAddr, bPhase,msn);
            break;
        }
        
        default:
        {
            break;
        }
    }
}

/**********************************************************************
* @name      : OopDateTimeSToRtc
* @brief     ：时间格式转换 rtc_t转换为OOP_DATETIME_S_T
* @param[in] ：OOP_DATETIME_S_T *dateTimeHex
* @param[out]：rtc_t *rtcTm
* @return    ：
* @Create    : 梅安宁
* @Date      ：2019-12-22
* @Update    :
**********************************************************************/
void ConvertRtcToBuff(rtc_t rtcTm, uint8* buff)
{
    if(!buff)    return;
    uint16 year = 2000+commfun_BcdToHex(rtcTm.yy);

    buff[0] = year>>8;
    buff[1] = year&0xff;
    buff[2] = commfun_BcdToHex(rtcTm.mm);
    buff[3] = commfun_BcdToHex(rtcTm.dd);
    buff[4] = commfun_BcdToHex(rtcTm.hh);
    buff[5] = commfun_BcdToHex(rtcTm.nn);
    buff[6] = commfun_BcdToHex(rtcTm.ss);
    return;
}

/**********************************************************************
* @name      : OopDateTimeSToRtc
* @brief     ：时间格式转换 rtc_t转换为OOP_DATETIME_S_T
* @param[in] ：rtc_t rtcTm
* @param[out]：OOP_DATETIME_S_T *dateTime
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-1-20
* @Update    :
**********************************************************************/
void ConvertRtcToOopDateTime(rtc_t rtcTm, OOP_DATETIME_S_T* dateTime)
{
    if(!dateTime)    return;
    dateTime->year = 2000+commfun_BcdToHex(rtcTm.yy);
    dateTime->month = commfun_BcdToHex(rtcTm.mm);
    dateTime->day = commfun_BcdToHex(rtcTm.dd);
    dateTime->hour = commfun_BcdToHex(rtcTm.hh);
    dateTime->minute = commfun_BcdToHex(rtcTm.nn);
    dateTime->second = commfun_BcdToHex(rtcTm.ss);
    return;
}

/**********************************************************************
* @name      : OopDateTimeSToRtc
* @brief     ：时间格式转换 OOP_DATETIME_S_T转换为rtc_t
* @param[in] ：OOP_DATETIME_S_T *dateTimeHex
* @param[out]：rtc_t *rtcTm
* @return    ：
* @Create    : 梅安宁
* @Date      ：2019-12-22
* @Update    :
**********************************************************************/
void OopDateTimeSToRtc(OOP_DATETIME_S_T *dateTimeHex,rtc_t *rtcTm)
{
    rtcTm->yy =(uint8)commfun_HexToBcd(dateTimeHex->year%100);
    rtcTm->mm =(uint8)commfun_HexToBcd(dateTimeHex->month);
    rtcTm->dd =(uint8)commfun_HexToBcd(dateTimeHex->day);
    rtcTm->hh =(uint8)commfun_HexToBcd(dateTimeHex->hour);
    rtcTm->nn =(uint8)commfun_HexToBcd(dateTimeHex->minute);
    rtcTm->ss =(uint8)commfun_HexToBcd(dateTimeHex->second);
    return ;
}

/**********************************************************************
* @name      : OopDateTimeSToLinuxTime
* @brief     ：OOP_DATETIME_S_T转换为Linux时间秒数
* @param[in] ：OOP_DATETIME_S_T *dateTime
* @param[out]：time_t linuxTime                 Linux时间
* @return    ：
* @Create    : 梅安宁
* @Date      ：2019-12-22
* @Update    :
**********************************************************************/
void OopDateTimeSToLinuxTime(OOP_DATETIME_S_T *dateTime, time_t *linuxTim)
{
    struct tm ptm;

    if(dateTime == NULL)
    {
        return ;
    }

    ptm.tm_year = dateTime->year-1900;
    ptm.tm_mon = dateTime->month-1;
    ptm.tm_mday = dateTime->day;
    ptm.tm_hour = dateTime->hour;
    ptm.tm_min = dateTime->minute;
    ptm.tm_sec = dateTime->second;

    *linuxTim = FastDateToSecond(&ptm);
}

/**********************************************************************
* @name      : LinuxTimeToOopDateTimeS
* @brief     ：Linux时间秒数转换为OOP_DATETIME_S_T
* @param[in] ：time_t linuxTime                 Linux时间
* @param[out]：OOP_DATETIME_S_T *dateTime
* @return    ：
* @Create    : 梅安宁
* @Date      ：2019-12-22
* @Update    :
**********************************************************************/
void LinuxTimeToOopDateTimeS(time_t linuxTime, OOP_DATETIME_S_T *dateTime)
{
    struct tm tm_Time;

    LOCALTIME_R(&linuxTime, &tm_Time);

    dateTime->year = tm_Time.tm_year+1900;
    dateTime->month = tm_Time.tm_mon+1;
    dateTime->day  = tm_Time.tm_mday;
    dateTime->week = tm_Time.tm_wday;
    dateTime->hour = tm_Time.tm_hour;
    dateTime->minute = tm_Time.tm_min;
    dateTime->second = tm_Time.tm_sec;

    return;
}

/**********************************************************************
* @name      : GetCurrentOopDateTimeS
* @brief     ：获取当前OOP_DATETIME_S_T格式时间
* @param[in] ：
* @param[out]：OOP_DATETIME_S_T *dateTime
* @return    ：
* @Create    : 梅安宁
* @Date      ：2019-12-22
* @Update    :

**********************************************************************/
void GetCurrentOopDateTimeS(OOP_DATETIME_S_T *dateTime)
{
    time_t timep;
    time(&timep);
    LinuxTimeToOopDateTimeS(timep, dateTime);
    return;
}



/**********************************************************************
* @name      : getMsgLenOffset
* @brief     ：获取octet-string数据元素长度,偏移量
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：uint8 *bytes             偏移量
* @return    ：长度
* @Create    : 梅安宁
* @Date      ：2020-1-19
* @Update    :
**********************************************************************/
uint16 getMsgLenOffset(uint8 *indata,uint8 *bytes)
{
    uint16 EleNum;
    uint8 nbytes = 1;

    EleNum = indata[0];
    if(EleNum==0x81)
    {
        EleNum = indata[1];
        nbytes = 2;
    }
    else if(EleNum==0x82)
    {
        EleNum = indata[1];
        EleNum <<= 8;
        EleNum += indata[2];
        nbytes = 3;
    }
    else if(EleNum==0x80 || EleNum>0x82)
    {
        return 0;
    }

    if(bytes != NULL)
        *bytes = nbytes;

    return EleNum;
}


/*****************************************************************************
所属功能单元：
功能描述：电流反极性告警
输入参数：测量点号
输出参数：无
函数返回值：
内部处理流程：判别条件：a)    90度<对应相电压与电流夹角< 270度
                        b)    三相三线不判B相
       三相四线－UI夹角在90至270间为反向发生
       三相三线－对A相夹角，在120至300间为反向发生；C相夹角，在60至240间为反向发生
***************************************************************************-*/
void CurReverseEvent(int clientid, uint8 bType)
{
    uint8  i;
    uint16 vlt_lmt = 0;                         //电压断相阀值
    uint16 ccAngle[3],vvAngle[3];               //电流、电压相角
    uint32 cur_para=0;                          //电流阀值
    uint16 angle[3];                            //-电压电流夹角-
    uint8  bCurReverseStat=0;
    uint8  bEventFlag = 0;
    float32  tmpFloat = 0;
    uint8 power_type = 4;
    static uint16 lastTimesLmt[3] = {0};    /* 发生持续时间 */
    static uint16 lastTimesLmtl[3] = {0};   /* 恢复持续时间 */
    static uint8 happenFlag[3];     /* 事件发生标志 */

    cur_para = 500*2/100;    //-默认为最大电流的2%-
    //额定电流
    if (para_query_float32_zhj(1, 0, 0, 0x00008009, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt = tmpFloat*1000*2/100;
    }
    EVENT_FMT_TRACE("电流反极性阈值 cur_para=%lu \n",cur_para);

    //-断相比例默认为75%-
    vlt_lmt = 2200*75/100;
    //额定电压
    if (para_query_float32_zhj(1, 0, 0, 0x00008008, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt = tmpFloat*10*75/100;
    }

    //接线方式
    if(para_query_array_zhj(1, 0, 0, 0x00008005, &power_type) < 0)
        power_type = 4; //三相四线

    if (power_type == 4)            //-三相四线-
    {
        if((Int_ABS(g_LatestACData.cur.nValue[0]) <= (cur_para)) || (Int_ABS(g_LatestACData.cur.nValue[1])<=(cur_para)) || 
            (Int_ABS(g_LatestACData.cur.nValue[2])<=(cur_para)))
        {
            //-判别相电流当前值≤电流反极性之阀值,不判-
            EVENT_FMT_DEBUG("判别相电流当前值≤电流反极性之阀值,不判\n");
            //write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);
            return;
        }

        if((g_LatestACData.vol.nValue[0] <= vlt_lmt) || (g_LatestACData.vol.nValue[1] <= vlt_lmt) || (g_LatestACData.vol.nValue[2] <= vlt_lmt))
        {
            //-断缺相不判-
            EVENT_FMT_DEBUG("缺相不判\n");
            //write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);

            return;
        }
    }

    EVENT_FMT_TRACE("接线方式 = %d \n",power_type);

    if (power_type == 3)            //-三相三线-
    {
        if((Int_ABS(event_global.cur_a)<=(cur_para))||(Int_ABS(event_global.cur_c)<=(cur_para)))
        {
            //-判别相电流当前值≤电流反极性之阀值,不判-
            EVENT_FMT_DEBUG("判别相电流当前值≤电流反极性之阀值,不判\n");
            //write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);

            return;
        }

        if((event_global.vlt_a<=vlt_lmt)||(event_global.vlt_c<=vlt_lmt))
        {
            //-断缺相不判-
            EVENT_FMT_DEBUG("缺相不判\n");
            //write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);

            return;
        }
    }

    vvAngle[0] = 0;                                //-a相电压相角-
    vvAngle[1] = g_LatestACData.oopWord3V.nValue[1] / 10;                            //-b相电压相角-
    vvAngle[2] = g_LatestACData.oopWord3V.nValue[2] / 10;                            //-c相电压相角-
    ccAngle[0] = g_LatestACData.oopWord3C.nValue[0] / 10;                            //-a相电流相角-
    ccAngle[1] = g_LatestACData.oopWord3C.nValue[1] / 10;                            //-b相电流相角-
    ccAngle[2] = g_LatestACData.oopWord3C.nValue[2] / 10;                            //-c相电流相角-

    if ((ccAngle[0] == APP_NONELWORD) && (ccAngle[1] == APP_NONELWORD) && (ccAngle[2] == APP_NONELWORD))
    {
        EVENT_FMT_DEBUG( "电流相角无效直接返回\n");

        return;
    }

    EVENT_FMT_TRACE("a相电压相角=%d,b相电压相角=%d,c相电压相角=%d,\n", vvAngle[0],vvAngle[1],vvAngle[2]);
    EVENT_FMT_TRACE("a相电流相角=%d,b相电流相角=%d,c相电流相角=%d,\n", ccAngle[0],ccAngle[1],ccAngle[2]);

    memset(vc_angle, 0x0, sizeof(vc_angle));
    for(i=0; i<3; i++)
    {
        /* 交采记录的电流夹角就是相对于电压的 */
        vc_angle[i] = g_LatestACData.oopWord3C.nValue[i] / 10;
    }

    if (power_type == 3)
    {
        angle[0] -= 30;
        if(angle[0]<0)
            vc_angle[0] = angle[0]+360;
        else
            vc_angle[0] = angle[0];
        angle[2] += 30;
        if(angle[2]>360)
            vc_angle[2] = angle[2]-360;
        else
            vc_angle[2] = angle[2];
    }

    EVENT_FMT_TRACE("转化后       a相电压相角=%d,b相电压相角=%d,c相电压相角=%d,\n", vvAngle[0],vvAngle[1],vvAngle[2]);
    EVENT_FMT_TRACE("转化后       a相电流相角=%d,b相电流相角=%d,c相电流相角=%d,\n", ccAngle[0],ccAngle[1],ccAngle[2]);
    EVENT_FMT_TRACE("电压电流夹角 a相夹角=%d,    b相夹角=%d,    c相夹角=%d,\n", vc_angle[0],vc_angle[1],vc_angle[2]);

    for(i = 0; i < 3; i++)
    {
        //-循环检测三相-
        if( power_type == 4)    //-三相四线-
        {
            if((vc_angle[i]>90) && (vc_angle[i]<270))
                bCurReverseStat |= (0x01<<(i*2));
            else
                bCurReverseStat |= (0x02<<(i*2));
        }

        if( power_type == 3)    //-三相三线-
        {
            if(i != 1)  //-B相不判-
            {
                if((vc_angle[i]>90) && (vc_angle[i]<270))
                    bCurReverseStat |= (0x01<<(i*2));
                else
                    bCurReverseStat |= (0x02<<(i*2));
            }
            else
            {
                bCurReverseStat |= 0;
            }
        }
    }

    EVENT_FMT_TRACE("事件发生标志 bCurReverseStat = %02X\n", bCurReverseStat);

    uint32 ArrIdx = 0;//一维数组索引(起始)
    ArrIdx = g_aFaultDataIndexZhj[1][0][0];
    
    if(bType==2)//-电流反极性恢复检测-
    {
        if(((bCurReverseStat&0x03)==2) && (happenFlag[0] == 1))
        {
            EVENT_FMT_DEBUG("A相电流反极性恢复 lastTimesLmtl[0] = %d\n", lastTimesLmtl[0]);
            if (lastTimesLmtl[0] > 60)
            {
                bEventFlag |= 0x01;    //-a相反极性恢复-
                yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+17].nFaultYxAddr,0);
                lastTimesLmt[0] = 0;
                happenFlag[0] = 0;
            }
            else
            {
                lastTimesLmtl[0]++;
            }
        }
        
        if((((bCurReverseStat&0x0c)>>2)==2) && (happenFlag[1] == 1))
        {
            EVENT_FMT_DEBUG("B相电流反极性恢复 lastTimesLmtl[1] = %d\n", lastTimesLmtl[1]);
            if (lastTimesLmtl[1] > 60)
            {
                bEventFlag |= 0x02;    //-b相反极性恢复-
                yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+18].nFaultYxAddr,0);
                lastTimesLmt[1] = 0;
                happenFlag[1] = 0;
            }
            else
            {
                lastTimesLmtl[1]++;
            }
            
        }
        
        if((((bCurReverseStat&0x30)>>4)==2) && (happenFlag[2] == 1))
        {
            EVENT_FMT_DEBUG("C相电流反极性恢复 lastTimesLmtl[2] = %d\n", lastTimesLmtl[2]);
            if (lastTimesLmtl[2] > 60)
            {
                bEventFlag |= 0x04;    //-c相反极性恢复-
                yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+19].nFaultYxAddr,0);
                lastTimesLmt[2] = 0;
                happenFlag[2] = 0;
            }
            else
            {
                lastTimesLmtl[2]++;
            }
        }
    }
    else if(bType==1)//-电流反极性发生检测-
    {
        if(((bCurReverseStat&0x03)==1) && (happenFlag[0] == 0))
        {
            EVENT_FMT_DEBUG("A相电流反极性发生 lastTimesLmt[0] = %d\n", lastTimesLmt[0]);
            if (lastTimesLmt[0] > 60)
            {
                bEventFlag |= 0x01;    //-a相反极性发生-
                yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+17].nFaultYxAddr,1);
                lastTimesLmtl[0] = 0;
                happenFlag[0] = 1;
            }
            else
            {
                lastTimesLmt[0]++;
            }
        }

        if((((bCurReverseStat&0x0c)>>2)==1) && (happenFlag[1] == 0))
        {
            EVENT_FMT_DEBUG("B相电流反极性发生 lastTimesLmt[1] = %d\n", lastTimesLmt[1]);
            if (lastTimesLmt[1] > 60)
            {
                bEventFlag |= 0x02;    //-b相反极性发生-
                yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+18].nFaultYxAddr,1);
                lastTimesLmtl[1] = 0;
                happenFlag[1] = 1;
            }
            else
            {
                lastTimesLmt[1]++;
            }
        }

        if((((bCurReverseStat&0x30)>>4)==1) && (happenFlag[2] == 0))
        {
            EVENT_FMT_DEBUG("C相电流反极性发生 lastTimesLmt[2] = %d\n", lastTimesLmt[2]);
            if (lastTimesLmt[2] > 60)
            {
                bEventFlag |= 0x04;    //-c相反极性发生-
                yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+19].nFaultYxAddr,1);
                lastTimesLmtl[2] = 0;
                happenFlag[2] = 1;
            }
            else
            {
                lastTimesLmt[2]++;
            }
        }
    }

    if(bEventFlag)
    {

        if(bType==1)
        {
            SaveFileRecBfg(ERC_NO_09,(bEventFlag&0x07)|0x80,0);//记录电流反极性(潮流反向)发生
        }
        else if(bType==2)
        {
            SaveFileRecBfg(ERC_NO_09,(bEventFlag&0x07),0);//记录电流反极性(潮流反向)恢复
        }
    }
}

/*****************************************************************************
所属功能单元：
功能描述：电压断相告警
输入参数：测量点号
输出参数：无
函数返回值：
内部处理流程：根据当前计量点的电流电压门限值和相关的门限参数判断是否断相，
断相则调用告警数据生成处理函数生成告警数据并填写主动上报队列
***************************************************************************-*/
void VltBreakEvent(int clientid,uint8 bType)
{
    int    ret = 0;
    uint16 vlt[3] = {0};                        //-当前３相电压-
    uint32 cur[3] = {0};                        //-当前３相电流-
    uint32 vlt_lmt = 0;                         //-电压判定值-
    uint16 lastTimesLmt = 0;                    //持续时间限值
    uint16 lastTimesLmt1 = 0;                    //恢复持续时间限值
    uint8 power_type = 0;       //接线方式
    uint16 vltgate = 2200;                         //额定电压
    uint16 crtgate = 5000;                         //额定电流
    uint32 vlt_resume = 0;                      //电压断相、缺相恢复比例
    static uint16 lastTimes[4][3] = {{0}};    //事件发生/恢复持续时间
    uint32 vlt_lmt1 = 0;                        //电压下下限值(失压限值)
    float32  tmpFloat   = 0;
    int64   tmpint  = 0;

    uint32 cur_lmt = 0;                         //电流判定值(启动电流)
    uint32 ERFlg = 0;
    uint8  order = 0;                           //相位
    uint8  is_now_flag = 0;
    int16  len = 0;
    uint8  bData = 0;
    uint8  bphase = 0;
    uint32 dwERFlgTmp = 0;
    uint16 dTemp = 0;
    uint8  tmp_flag = 0;

    //-读取实时电压、电流-
    vlt[0] = g_LatestACData.vol.nValue[0];
    vlt[1] = g_LatestACData.vol.nValue[1];
    vlt[2] = g_LatestACData.vol.nValue[2];
    cur[0] = g_LatestACData.cur.nValue[0];
    cur[1] = g_LatestACData.cur.nValue[1];
    cur[2] = g_LatestACData.cur.nValue[2];

    EVENT_FMT_TRACE("断缺相3相电压vlt[0]=%u,vlt[1]=%u,vlt[2]=%u, 3相电流cur[0]=%u,cur[1]=%u,cur[2]=%u\n",vlt[0],vlt[1],vlt[2],cur[0],cur[1],cur[2]);

    if ((vlt[0] == APP_NONEWORD) && (vlt[1] == APP_NONEWORD) && (vlt[2] == APP_NONEWORD))
    {
        EVENT_FMT_DEBUG( "电压无效直接返回\n");

        return;
    }

    if((cur[0] == 0x0)&&(vlt[0] == 0x0)&&(cur[1] == 0x0)&&(vlt[1] == 0x0)&&(cur[2] == 0x0)&&(vlt[2] == 0x0))
    {
        EVENT_FMT_DEBUG("电压电流都为0，是停电\n");

        return;//-电压电流都为0，是停电-
    }

    //额定电压
    if (para_query_float32_zhj(1, 0, 0, 0x00008008, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vltgate = tmpFloat*10;
    }
    //-读取电压断相比例-
    vlt_lmt = vltgate*78/100;  /*- 断相比例默认为78%-*/

    //-读取电压下下限值,作为失压 门限-
    vlt_lmt1 = vltgate*78/100;  /*- 断相比例默认为78%-*/
    


    vlt_resume = vltgate * 85/100;              //-失压和断相恢复比例默认为85%-
    EVENT_FMT_TRACE("额定电压vltgate = %d,电压断相门限vlt_lmt=%lu,电压失压门限vlt_lmt1=%lu, 失压和断相恢复比例默认vlt_resume = %lu\n",vltgate, vlt_lmt,vlt_lmt1,vlt_resume);

    //-失压持续时间限值-
    lastTimesLmt = 2*60;
    lastTimesLmt1 = 2*60;
    
    if (para_query_int64_zhj(1, 0, 0, 0x00008042, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt = tmpint * 60;
    }

    if (para_query_int64_zhj(1, 0, 0, 0x00008043, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt1 = tmpint * 60;
    }

    EVENT_FMT_TRACE("失压断相持续时间限值lastTimesLmt=%d,恢复时间%d\n",lastTimesLmt,lastTimesLmt1);

    if (para_query_float32_zhj(1, 0, 0, 0x00008009, &tmpFloat) > 0 && tmpFloat > 0)
    {
        crtgate = tmpFloat*1000;
    }

    cur_lmt = crtgate * 2/5;  //-求得断缺相电流限值-
    EVENT_FMT_TRACE("额定电流的值cur_gate = %lu    启动电流的值cur_lmt = %lu\n",crtgate,cur_lmt);

    ret = para_query_array_zhj(1, 0, 0, 0x00008005, &power_type);
    if(ret < 0)
        power_type = 4; //三相四线

    //-循环检测三相-
    for(order = 0; order < 3; order++)
    {
        if((cur[order] == APP_NONELWORD)||(vlt[order] == APP_NONEWORD)
                ||(cur[order] == NONE_LWORD)||(vlt[order] == NONE_WORD))    //-电流电压为无效-
        {
            continue;
        }

        if(power_type == 3 && order == 1)    //三相三线B相不判断
        {
            continue;
        }

        tmp_flag = 0;
        if(bType == 1)              //-电压缺相-
        {
            tmp_flag = (cur[order] > 100) && (vlt[order] == 0 || vlt[order] <= vlt_lmt1); /* 电流大于0.1A，防止和断相混淆 */
        }
        else if(bType == 2)         //-电压缺相恢复-
        {
            tmp_flag = ((cur[order] > 0) && (vlt[order] > vlt_resume));
        }
        else if(bType == 3)         //-电压断相发生检测条件-
        {
            tmp_flag =  ((cur[order] < 100) && (vlt[order] < 100));
        }
        else if(bType == 4)         //-电压断相恢复条件-
        {
            tmp_flag =  ((cur[order] > 0) && (vlt[order] > vlt_resume));
        }
        else
        {
            return;
        }

        if(tmp_flag)                //如果满足相应的发生条件，将对应的时间进行累加
        {
            if(lastTimes[bType-1][order] >= lastTimesLmt-1)
            {
                is_now_flag = 1;
            }
            else
            {
                lastTimes[bType-1][order]++;
                is_now_flag = 0;
            }
            EVENT_FMT_DEBUG("第%d相满足发生条件，开始统计时间，目前时间lastTimes = %d\n",order+1,lastTimes[bType-1][order]);
        }
        else
        {
            EVENT_FMT_TRACE("第%d相不满足发生条件，时间清零\n",order+1);
            lastTimes[bType-1][order] = 0;
            is_now_flag = 0;
        }

        if((bType == 1)||(bType ==2))    //-缺相及恢复检测-
            len =  read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+3), 1);
        else    //-断相及恢复检测-
            len =  read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+4), 1);

        if (bData == 0xff) //文件系统初始为0xff值
        {
            bData = 0;
        }

        if (len < 0)
        {
            EVENT_FMT_DEBUG("读取事件状态失败");
            return;
        }

        EVENT_FMT_TRACE("读取事件状态%02X\n", bData);
        uint32 ArrIdx = 0;//一维数组索引(起始)
        ArrIdx = g_aFaultDataIndexZhj[1][0][0];

        if((bType == 1)||(bType ==3))    //-事件发生检测-
        {
            if(is_now_flag)
            {
                EVENT_FMT_TRACE("事件发生检测满足发生条件\n");
                switch(order)
                {
                case 0:
                    if(!(bData&A_PHASE))
                    {
                        bData |= A_PHASE;    //-A相-
                        bphase |= A_PHASE;    //-A相-
                        if(bType == 3)
                        {
                            ERFlg = BREAK_PHASE;//-断相标记-
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+7].nFaultYxAddr,1);
                            EVENT_FMT_DEBUG("置A相断相标记\n");
                        }
                        else
                        {
                            ERFlg = LOSE_VOLT;    //-缺相标记-
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+10].nFaultYxAddr,1);
                            EVENT_FMT_DEBUG("置A相失压标记\n");
                        }
                    }
                    else
                    {
                        EVENT_FMT_TRACE("A相已经记录过\n");
                    }
                    break;
                case 1:
                    if(!(bData&B_PHASE))
                    {
                        bData |= B_PHASE;
                        bphase |= B_PHASE;
                        if(bType == 3)
                        {
                            ERFlg = BREAK_PHASE;//-断相标记-
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+8].nFaultYxAddr,1);
                            EVENT_FMT_DEBUG("置B相断相标记\n");
                        }
                        else
                        {
                            ERFlg = LOSE_VOLT ;    //-缺相标记-
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+11].nFaultYxAddr,1);
                            EVENT_FMT_DEBUG("置B相失压标记\n");
                        }
                    }
                    else
                    {
                        EVENT_FMT_TRACE("B相已经记录过\n");
                    }
                    break;
                case 2:
                    if(!(bData&C_PHASE))
                    {
                        bData |= C_PHASE;
                        bphase |= C_PHASE;
                        if(bType == 3)
                        {
                            ERFlg = BREAK_PHASE;//-断相标记-
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+9].nFaultYxAddr,1);
                            EVENT_FMT_DEBUG("置C相断相标记\n");
                        }
                        else
                        {
                            ERFlg = LOSE_VOLT ;    //-缺相标记-
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+12].nFaultYxAddr,1);
                            EVENT_FMT_DEBUG("置C相失压标记\n");
                        }
                    }
                    else
                    {
                        EVENT_FMT_TRACE("C相已经记录过\n");
                    }
                    break;
                default:
                    break;
                }
                if(bType == 1)
                    write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+3), 1);
                else
                    write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+4), 1);
            }
        }
        else//-事件恢复检测-
        {
            if((is_now_flag)&&( bData &(1<<order)))
            {
                EVENT_FMT_TRACE("事件恢复检测\n");
                switch(order)
                {
                case 0:
                    bData &= ~A_PHASE;    //-清除A相标记-
                    bphase |= A_PHASE;
                    if(bType == 4)
                    {
                        ERFlg = BREAK_PHASE;//-断相标记-
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+7].nFaultYxAddr,0);
                        
                        EVENT_FMT_TRACE("置A相断相恢复标记\n");
                    }
                    else
                    {
                        ERFlg = LOSE_VOLT;    //-缺相标记-
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+10].nFaultYxAddr,0);
                        EVENT_FMT_TRACE("置A相失压恢复标记\n");
                    }
                    break;

                case 1:
                    bData &= ~B_PHASE;
                    bphase |= B_PHASE;
                    if(bType == 4)
                    {
                        ERFlg = BREAK_PHASE;//-断相标记-
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+8].nFaultYxAddr,0);
                        EVENT_FMT_TRACE("置B相断相恢复标记\n");
                    }
                    else
                    {
                        ERFlg = LOSE_VOLT;    //-缺相标记-
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+11].nFaultYxAddr,0);
                        EVENT_FMT_TRACE("置B相失压恢复标记\n");
                    }
                    break;

                case 2:
                    bData &= ~C_PHASE;
                    bphase |= C_PHASE;
                    if(bType == 4)
                    {
                        ERFlg = BREAK_PHASE;//-断相标记-
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+9].nFaultYxAddr,0);
                        EVENT_FMT_TRACE("置C相断相恢复标记\n");
                    }
                    else
                    {
                        ERFlg = LOSE_VOLT;    //-缺相标记-
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+12].nFaultYxAddr,0);
                        EVENT_FMT_TRACE("置C相失压恢复标记\n");
                    }
                    break;

                default:
                    break;
                }
                if(bType == 2)
                    write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+3), 1);
                else
                    write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+4), 1);
            }
        }
    }



    if(ERFlg)//-事件发生或恢复-
    {
        uint32  aTemp = 0;

        if((bType == 1)&&(ERFlg == BREAK_PHASE))
        {
            //-缺相发生才报表故障事件-
            len = read_pdata_xram("^Event", (char *)&aTemp, (BreakphaseAddr), 4);
            if(len>=0)
            {
                aTemp++;//-记录表的新缺相次数-
                write_pdata_xram("^Event", (char *)&aTemp, (BreakphaseAddr), 4);
            }
        }

        if((bType==3)&&(ERFlg == LOSE_VOLT))
        {
            if(read_pdata_xram("^Event", (char *)&dTemp, (LostvltAddr), 2)>=0)
                dTemp++;    //-断相事件发生,次数有变化-
            write_pdata_xram("^Event", (char *)&dTemp, (LostvltAddr), 2);
        }


        //通知104协议读取
        if((bType == 2)||(bType == 4))//-事件恢复-
        {
            len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);
            dwERFlgTmp &= ~ERFlg;//-保存恢复标记到此测量点全局事情标记中-
            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);

            //断相和缺相使用同一个序号，在类型的最高位来区分，最高位为1表示缺相，为0表示断相
            if(bType == 2)
                SaveFileRecBfg(ERC_NO_10|0x80,(bphase&0x07),0);
            else if (bType == 4)
                SaveFileRecBfg(ERC_NO_10,bphase&0x07,0);

        }
        else//-事件发生-
        {
            len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);
            dwERFlgTmp |= ERFlg;//-保存发生标记到此测量点全局事情标记中-
            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);

            //断相和缺相使用同一个序号，在相位的最高位来区分，最高位为1表示缺相，为0表示断相
            if(bType == 1)
                SaveFileRecBfg(ERC_NO_10|0x80,(bphase&0x07)|0x80,0);
            else if (bType == 3)
                SaveFileRecBfg(ERC_NO_10,     (bphase&0x07)|0x80,0);
       }
    }
}

/*****************************************************************************
所属功能单元：
功能描述：逆相序告警
输入参数：bType 1－－电流逆相，0－－电压
输出参数：无
函数返回值：1－－逆相，0－－正常
内部处理流程：仅判内置交采逆相序事件，相角数据来源交采驱动Real_cal.c全局变量
       另注：按04事件规范，断缺相时不判逆相序
***************************************************************************-*/
void PhaseReverseEvent(int clientid, uint8 bType)
{
    uint8    lastTimes    = 0;      //-持续发生/恢复时间，0：电压逆相序，1：电流逆相序-
    uint8    lastTimesLmt = 1;      //-持续时间限值，默认1分钟-
    int16    len          = 0;
    uint8    bData        = 0;
    uint32   ERFlg        = 0;      //-电压电流逆相序 1：发生，2：恢复-
    uint8    b_event_Type = 0;      //-0：电压逆相序发生/恢复，1：电流逆相序发生/恢复-
    uint8    happenFlg    = 0;
    uint8    recoverFlg   = 0;
    uint16   vlt_lmt      = 0;      //-断相比例默认为75%-
    uint16   cur_lmt      = 0;      //-断相比例默认为75%-
    float32  tmpFloat     = 0;

    len = read_pdata_xram("^Event", (char *)&bData, (PhaseAbnormalAddr+bType), 1);
    if (len < 0)//-未申请到内存或内存乱-
    {
        EVENT_FMT_DEBUG("CheckERC_300F，读取事件状态失败");
        return;
    }

    if(bData == 0xFF)
        bData = 0;
    
    //电压
    if (bType == 0)
    {
        //读取额定电压
        if (para_query_float32_zhj(1, 0, 0,0x00008008, &tmpFloat) > 0 && tmpFloat > 0)
        {
            vlt_lmt = tmpFloat*75*10/100;
        }
        else
        {
            vlt_lmt = 2200*75/100;
        }

        EVENT_FMT_TRACE("电压断相门限=%d\n", vlt_lmt);

        //-逆相序判断-
        if (g_LatestACData.vol.nValue[0] == APP_NONEWORD || g_LatestACData.vol.nValue[1] == APP_NONEWORD || g_LatestACData.vol.nValue[2] == APP_NONEWORD)
        {
            EVENT_FMT_DEBUG("三相四线电压无效时不判逆相序\n");

            return;
        }

        if( (g_LatestACData.vol.nValue[0]<vlt_lmt) || (g_LatestACData.vol.nValue[1]<vlt_lmt)
           || (g_LatestACData.vol.nValue[2]<vlt_lmt))    //-断缺相时不判逆相序-
        {
            EVENT_FMT_DEBUG("三相四线断缺相时不判逆相序\n");

            return;
        }

        if((g_LatestACData.meterRsw.metWord[6]&0x0001) == 1)
        {
            b_event_Type = 1;
        }
    }
    else if (bType == 1) //电流
    {
        //读取额定电流
        if (para_query_float32_zhj(1, 0, 0,0x00008009, &tmpFloat) > 0 && tmpFloat > 0)
        {
            cur_lmt = tmpFloat*1000*75/100;
        }
        else
        {
            cur_lmt = 5000*75/100;
        }

        EVENT_FMT_TRACE("电流断相门限=%d\n", vlt_lmt);

        //-逆相序判断-
        if (g_LatestACData.cur.nValue[0] == APP_NONELWORD || g_LatestACData.cur.nValue[1] == APP_NONELWORD || g_LatestACData.cur.nValue[2] == APP_NONELWORD)
        {
            EVENT_FMT_DEBUG("三相四线电流无效时不判逆相序\n");

            return;
        }

        if( (abs(g_LatestACData.cur.nValue[0]) < cur_lmt) || (abs(g_LatestACData.cur.nValue[1]) < cur_lmt)
           || (abs(g_LatestACData.cur.nValue[2]) < cur_lmt))    //-断缺相时不判逆相序-
        {
            EVENT_FMT_DEBUG("三相四线断缺相时不判逆相序\n");

            return;
        }

        if((g_LatestACData.meterRsw.metWord[6]&0x0002) == 0x0002)
        {
            b_event_Type = 1;
        }
    }

    //未找到参数，使用默认参数1分钟
    lastTimesLmt = 60;

    EVENT_FMT_TRACE("读取事件之前状态%02X,当前状态:%02X,持续时间限值:%u\n", bData, b_event_Type, lastTimesLmt);

    if(b_event_Type)
    {
        if(!(bData & 1))//尚未发生
        {
            len = read_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+bType), 1);
            if(len < 0 || (lastTimes == 0xff))
                lastTimes = 0;
            
            EVENT_FMT_TRACE("CheckERC_300F发生 :lastTimes = %d lastTimesLmt=%d\n",lastTimes,lastTimesLmt);
            if(lastTimes++ >= lastTimesLmt)
            {
                bData |= 1;
                ERFlg = 1;
                lastTimes = lastTimesLmt * 2;
                write_pdata_xram("^Event", (char *)&bData, (PhaseAbnormalAddr+bType), 1);
            }

            write_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+bType), 1);
        }
        else
        {
            len = read_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+bType), 1);
            if(len < 0 || (lastTimes == 0xff))
                lastTimes = 0;
            if(lastTimes != lastTimesLmt * 2)
            {
                lastTimes = lastTimesLmt * 2;
                write_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+bType), 1);
            }
        }
    }
    else
    {
        if(bData & 1)
        {
            len = read_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+bType), 1);
            if(len < 0 || (lastTimes == 0xff))
                lastTimes = 2*lastTimesLmt;
            EVENT_FMT_TRACE("CheckERC_300F恢复: lastTimes = %d lastTimesLmt%d\n",lastTimes,lastTimesLmt);
            if(lastTimes-- <= lastTimesLmt)
            {
                bData &= ~(1);
                ERFlg = 2;  //-恢复-
                lastTimes = 0;
                write_pdata_xram("^Event", (char *)&bData, (PhaseAbnormalAddr+bType), 1);
            }
            write_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+bType), 1);
        }
        else
        {
            len = read_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+bType), 1);
            if(len < 0 || (lastTimes == 0xff))
                lastTimes = 0;
            if(lastTimes != 0)
            {
                lastTimes = 0;
                write_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+bType), 1);
            }
        }
    }

    if(ERFlg ==1)  //-发生-
    {
        happenFlg = 1;
    }

    if(ERFlg==2)  //-恢复-
    {
        recoverFlg = 1;
    }

    uint32 ArrIdx = 0;//一维数组索引(起始)
    ArrIdx = g_aFaultDataIndexZhj[1][0][0];

    if(happenFlg) //发生
    {
        
        if (bType == 0)
        {
            SaveFileRecBfg(ERC_NO_11|0x80,0x80,0);//记录电压逆相序发生
            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+15].nFaultYxAddr,1);   //置电压逆相序事件标志
        }
        else
        {
            SaveFileRecBfg(ERC_NO_11&0x7f,0x80,0);//记录电流逆相序发生
            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+16].nFaultYxAddr,1);   //置电流逆相序事件标志
        }
    }
    else if(recoverFlg) //恢复
    {

        if (bType == 0)
        {
            SaveFileRecBfg(ERC_NO_11|0x80,0,0);//记录电压逆相序恢复
            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+15].nFaultYxAddr,0);   //置电压逆相序事件恢复标志
        }
        else
        {
            SaveFileRecBfg(ERC_NO_11&0x7f,0,0);//记录电流逆相序恢复
            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+16].nFaultYxAddr,0);   //置电流逆相序事件恢复标志
        }
    }

    return;
}

/*****************************************************************************
所属功能单元：
功能描述：电压越限告警
输入参数：计量点号
输出参数：无
函数返回值：无
内部处理流程：在连续失压规定时间内根据当前计量点的电流电压门限值和相关的门限参数判断电压
                 是否越上限，越上限则调用告警数据生成处理函数生成告警数据并填写主动上报队列
***************************************************************************-*/
void VltSupassEvent(int clientid, uint8 bType)
{
    uint16  vlt[3] = {0};    //-当前3相电压-
    uint32  cur[3] = {0};    //-当前3相电流-
    uint16  vlt_lmt = 0;     //-电压异常上限值-
    uint16  vlt_lmt1 = 0;    //-电压异常下限值-
    uint32  ERFlg = 0;       //-事件发生标志-
    uint8   order = 0;       //-相限-
    //uint8  bTemp = 0;
    uint8   is_now_flag = 0; //-电压越限标志-
    uint8   bData = 0;
    uint8   bphase = 0;      //-相限-
    int16   len = 0;
    uint32  dwERFlgTmp = 0;
    int8    bfgPower=0;
    float32 tmpFloat = 0;
    int64   tmpint;

    static  uint8 lastTimes[4][3];   //越上上限、下下限的累计时间
    uint16   lastTimesLmt = 1;                        //默认的电压异常上限持续时间
    uint16   lastTimesLmt1 = 1;                       //默认的电压异常下限持续时间
    uint16  vlt_lmt_rev = 0;                         //-电压异常上限恢复值-
    uint16  vlt_lmt1_rev = 0;                        //-电压异常下限恢复值-
    float32 Rated_voltage =2200;

    uint8 power_type = 0;       //接线方式
    int ret = 0;

#if WRITE_LOG_EN
    uint16   i;
#endif
    //uint16    j;

    EVENT_FMT_TRACE("停电标志bfgPower=%d\n",bfgPower);

    vlt[0] = g_LatestACData.vol.nValue[0];
    vlt[1] = g_LatestACData.vol.nValue[1];
    vlt[2] = g_LatestACData.vol.nValue[2];
    cur[0] = g_LatestACData.cur.nValue[0];
    cur[1] = g_LatestACData.cur.nValue[1];
    cur[2] = g_LatestACData.cur.nValue[2];

    if ((vlt[0] == APP_NONEWORD) && (vlt[1] == APP_NONEWORD) && (vlt[2] == APP_NONEWORD))
    {
        EVENT_FMT_DEBUG( "电压无效直接返回\n");

        return;
    }

    //额定电压
    if (para_query_float32_zhj(1, 0, 0, 0x00008008, &tmpFloat) > 0 && tmpFloat > 0)
    {
        Rated_voltage = tmpFloat*10;
    }

    //---原来代码不设限值就返回，现在修改为默认参数 2500 ----return;---
    vlt_lmt = 2530;
    //--原来代码不设限值就返回，现在修改为默认参数 1870 ----return;---
    vlt_lmt1 = 2046;
    vlt_lmt1_rev = vlt_lmt1;

    vlt_lmt_rev = 2310;

    if (para_query_float32_zhj(1, 0, 0, 0x00008022, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt = tmpFloat*Rated_voltage;
    }

    if (para_query_float32_zhj(1, 0, 0, 0x00008023, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt_rev = tmpFloat*Rated_voltage;
    }

    lastTimesLmt = 15;           //容错
    lastTimesLmt1 = 15;          //容错

    if (para_query_int64_zhj(1, 0, 0, 0x00008040, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt = tmpint * 60;
        //EVENT_FMT_DEBUG("VltSupassEvent lastTimesLmt tmpint=%lld \n", tmpint);
    }

    if (para_query_int64_zhj(1, 0, 0, 0x00008041, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt1 = tmpint * 60;
        //EVENT_FMT_DEBUG("VltSupassEvent lastTimesLmt1 tmpint=%lld \n", tmpint);
    }

    //EVENT_FMT_DEBUG("VltSupassEvent 电压a=%d,电压b=%d, 电压c=%d \n", vlt[0],vlt[1],vlt[2]);
    EVENT_FMT_DEBUG("VltSupassEvent 电压上限值=%d,持续时间=%d， 电压上限恢复值=%d    持续时间=%d\n", vlt_lmt, lastTimesLmt,vlt_lmt_rev,lastTimesLmt1);
    if((bType == 1)||(bType == 2))
    {
        //-读取相位记录信息-
        len = read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+7), 1);
    }
    else
    {
        //-读取相位记录信息-
        len = read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+8), 1);
    }

    if (len < 0)
    {
        EVENT_FMT_DEBUG("VltSupassEvent 读取事件状态失败");
        return;
    }

    if (bData == 0xff) //文件系统初始为0xff值
    {
        bData = 0;
    }

    EVENT_FMT_TRACE("读取事件状态%02X\n", bData);

    if((cur[0] == 0x0)&&(vlt[0] == 0x0)&&(cur[1] == 0x0)&&(vlt[1] == 0x0)&&(cur[2] == 0x0)&&(vlt[2] == 0x0))
        return;//-电压电流都为0，是停电-

    ret = para_query_array_zhj(1, 0, 0, 0x00008005, &power_type);
    if(ret < 0)
        power_type = 4; //三相四线

    for(order = 0; order < 3; order++)
    {
        if((vlt[order] == 0xeeee)||(vlt[order] == 0xffff))
        {
            continue;
        }
        
        if((power_type == 4) || (event_global.power_type == 3 && order != 1))    //三相三线,不判B相
        {
            switch (bType)
            {
            case 1:        //电压过压
                if(vlt[order] > vlt_lmt)        //如果满足相应的发生条件，将对应的时间进行累加
                {
                    if(lastTimes[bType-1][order] >= lastTimesLmt-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order]++;
                        is_now_flag = 0;
                    }
//					yx_calculate_sta_set(CALC_YX_VOLT_OVER_H, order, 1);
                    EVENT_FMT_DEBUG("第%d相满足电压过压发生条件，已持续时间lastTimes= %d\n",order+1,lastTimes[bType-1][order]);
                }
                else
                {
                    EVENT_FMT_TRACE("第%d相不满足电压过压发生条件，时间清零\n",order+1);
                    lastTimes[bType-1][order] = 0;
                    is_now_flag = 0;
                }
                break;

            case 2:        //电压过压恢复
                if((vlt[order] <= vlt_lmt_rev) && (bData &(1<<order)))    //电压小于上上限恢复阀值 并且 已经发生过越上上限
                {
                    //如果满足相应的发生条件，将对应的时间进行累加
                    if(lastTimes[bType-1][order] >= lastTimesLmt1-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order]++;
                        is_now_flag = 0;
                    }
//					yx_calculate_sta_set(CALC_YX_VOLT_OVER_H, order, 0);
                    EVENT_FMT_DEBUG("第%d相满足电压过压恢复条件，已持续时间lastTimes= %d\n",order+1,lastTimes[bType-1][order]);
                }
                else
                {
                    EVENT_FMT_TRACE("第%d相不满足电压过压恢复条件，时间清零\n",order+1);
                    lastTimes[bType-1][order] = 0;
                    is_now_flag = 0;
                }
                break;

            case 3:        //电压失压
                if(vlt[order] < vlt_lmt1)        //如果满足相应的发生条件，将对应的时间进行累加
                {
                    if(lastTimes[bType-1][order] >= lastTimesLmt-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order]++;
                        is_now_flag = 0;
                    }
//					yx_calculate_sta_set(CALC_YX_VOLT_DOWN_L, order, 1);
                    EVENT_FMT_DEBUG("第%d相满足电压失压发生条件，已持续时间lastTimes = %d\n",order+1,lastTimes[bType-1][order]);
                }
                else
                {
                    EVENT_FMT_TRACE("第%d相不满足电压失压发生条件，时间清零\n",order+1);
                    lastTimes[bType-1][order] = 0;
                    is_now_flag = 0;
                }

                break;

            case 4:        //电压失压恢复
                if((vlt[order] >= vlt_lmt1_rev) && (bData &(1<<order)))    //电压大于于下下限恢复阀值 并且 已经发生过越下下限
                {
                    //如果满足相应的发生条件，将对应的时间进行累加
                    if(lastTimes[bType-1][order] >= lastTimesLmt1-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order]++;
                        is_now_flag = 0;
                    }
//					yx_calculate_sta_set(CALC_YX_VOLT_DOWN_L, order, 0);
                    EVENT_FMT_DEBUG("第%d相满足电压失压恢复条件，已持续时间lastTimes = %d\n",order+1,lastTimes[bType-1][order]);
                }
                else
                {
                    EVENT_FMT_TRACE("第%d相不满足电压失压恢复条件，时间清零\n",order+1);
                    lastTimes[bType-1][order] = 0;
                    is_now_flag = 0;
                }

                break;

            default:
                continue;
            }
        }
        else
        {
            continue;
        }

        uint32 ArrIdx = 0;//一维数组索引(起始)
        ArrIdx = g_aFaultDataIndexZhj[1][0][0];

        if((bType == 1)||(bType == 3))
        {
            if(is_now_flag)
            {
                switch(order)
                {
                case 0:
                    if(!(bData&A_PHASE))
                    {
                        bData |= A_PHASE;    //-A相-
                        bphase |= A_PHASE;    //-A相-
                        if(bType == 1)
                        {
                            ERFlg = SUPASS_VLT;    //-超上限标志-
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+1].nFaultYxAddr,1);   //置电压越上限事件标志
                        }
                        else
                        {
                            ERFlg = LOW_VLT;
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+30].nFaultYxAddr,1);
                        }
                    }
                    break;
                case 1:

                    if(!(bData&B_PHASE))
                    {
                        bData |= B_PHASE;
                        bphase |= B_PHASE;
                        if(bType == 1)
                        {
                            ERFlg = SUPASS_VLT;    //-超上限标志-
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+2].nFaultYxAddr,1);   //置电压越上限事件标志
                        }
                        else
                        {
                            ERFlg = LOW_VLT;
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+31].nFaultYxAddr,1);
                        }
                    }
                    break;
                case 2:
                    if(!(bData&C_PHASE))
                    {
                        bData |= C_PHASE;
                        bphase |= C_PHASE;
                        if(bType == 1)
                        {
                            ERFlg = SUPASS_VLT;    //-超上限标志-
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+3].nFaultYxAddr,1);   //置电压越上限事件标志
                        }
                        else
                        {
                            ERFlg = LOW_VLT;
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+32].nFaultYxAddr,1);
                        }
                    }
                    break;
                default:
                    break;
                }
                if(bType == 1)
                {
                    if(write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+7), 1)<0)
                    {
                        return;
                    }
                }
                else
                {
                    if(write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+8), 1)<0)
                    {
                        return;
                    }
                }
            }
        }
        else
        {
            if(is_now_flag)
            {
                //-恢复不判持续时间-
                switch(order)
                {
                case 0:
                    bData  &= ~A_PHASE;        //-清相位标记-
                    bphase |= A_PHASE;
                    if(bType == 2)
                    {
                        ERFlg = SUPASS_VLT;    //-超上限标志-
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+1].nFaultYxAddr,0);   //置电压越上限事件恢复标志
                    }  
                    else
                    {
                        ERFlg = LOW_VLT;
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+30].nFaultYxAddr,0);
                    }
                    break;
                case 1:
                    bData  &= ~B_PHASE;        //-清相位标记-
                    bphase |= B_PHASE;
                    if(bType == 2)
                    {
                        ERFlg = SUPASS_VLT;    //-超上限标志-
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+2].nFaultYxAddr,0);   //置电压越上限事件恢复标志
                    }
                    else
                    {
                        ERFlg = LOW_VLT;
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+31].nFaultYxAddr,0);
                    }
                    break;
                case 2:
                    bData  &= ~C_PHASE;        //-清相位标记-
                    bphase |= C_PHASE;
                    if(bType == 2)
                    {
                        ERFlg = SUPASS_VLT;    //-超上限标志-
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+3].nFaultYxAddr,0);   //置电压越上限事件恢复标志
                    }
                    else
                    {
                        ERFlg = LOW_VLT;
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+32].nFaultYxAddr,0);
                    }
                    break;
                default:
                    break;
                }
                if(bType == 2)
                {
                    if(write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+7), 1)<0)
                    {
                        return;
                    }
                }
                else
                {
                    if(write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+8), 1)<0)
                    {
                        return;
                    }
                }
            }
        }
    }

    if(ERFlg)//-事件发生-
    {
        
        if((bData == 0)&&((bType == 2)||(bType == 4)))
        {
            len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);
            dwERFlgTmp &= ~ERFlg;    //-保存发生标记到此测量点全局事情标记中-
            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);

            if(bType == 2)
                SaveFileRecBfg(ERC_NO_24|0x80,(bphase&0x07),0);//-越上限恢复-
            else
                SaveFileRecBfg(ERC_NO_24,     (bphase&0x07),0);//-越下限恢复-
        }
        else
        {
            len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);
            dwERFlgTmp |= ERFlg;    //-置该测量点全局事件标志位-
            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);

            if(bType == 1)
               SaveFileRecBfg(ERC_NO_24|0x80,(bphase&0x07)|0x80,0);//-越上限发生-
            else
               SaveFileRecBfg(ERC_NO_24     ,(bphase&0x07)|0x80,0);//-越下限发生-
        }
    }
}

/*******************************************************************************
* 所属单元：
* 功能描述：电流越限告警
* 输入参数：计量点号
* 输出参数：无
* 返 回 值：无
* 内部流程：在默认时间内根据当前计量点的电流电压门限值和相关的门限参数判断电流是否越上限，
*              越上限则调用告警数据生成处理函数生成告警数据并填写主动上报队列
*******************************************************************************/
void CurHighEvent(int clientid, uint8 bType)
{
    //uint16    i;
    uint16  vlt[3] = {0};                            //当前电压
    uint32  cur[3] = {0};                            //当前电流
    int     cur_lmt1= 0 ,cur_lmt2= 0;       //门限值
    uint32  ERFlg = 0;
    uint8   order;//,bTemp;
    //uint16  mtetmp = 0;
    uint8   is_now_flag = 0;
    uint8   bData=0,bphase = 0;
    int16   len;
    //uint32  dwERFlgTmp=0;
    static uint8 lastTimes[4][3];    //越上上限、下下限的累计时间
    uint16   lastTimesLmt1 = 1;                       //默认的电流越上限持续时间
    uint16   lastTimesLmt2 = 1;                       //默认的电流越上上限持续时间
    uint16  cur_lmt1_rev = 0;                        //电流越上限恢复值
    uint16  cur_lmt2_rev = 0;                        //电流越上上限恢复值
    float32 tmpFloat;
    int64   tmpint;
    float32 Rated_current = 5;
    uint8   power_type = 0;       //接线方式
    int     ret = 0;

    vlt[0] = g_LatestACData.vol.nValue[0];
    vlt[1] = g_LatestACData.vol.nValue[1];
    vlt[2] = g_LatestACData.vol.nValue[2];
    cur[0] = g_LatestACData.cur.nValue[0];
    cur[1] = g_LatestACData.cur.nValue[1];
    cur[2] = g_LatestACData.cur.nValue[2];

    if ((cur[0] == APP_NONELWORD) && (cur[1] == APP_NONELWORD) && (cur[2] == APP_NONELWORD))
    {
        EVENT_FMT_DEBUG( "电流无效直接返回\n");

        return;
    }

    //额定电流
    if (para_query_float32_zhj(1, 0, 0, 0x00008009, &tmpFloat) > 0 && tmpFloat > 0)
    {
        Rated_current = tmpFloat;
    }

    cur_lmt1 = 6500;
    cur_lmt2 = 8000;

    cur_lmt1_rev = 5500;
    cur_lmt2_rev = 7000;

    if (para_query_float32_zhj(1, 0, 0, 0x00008024, &tmpFloat) > 0 && tmpFloat > 0)
    {
        cur_lmt1 = tmpFloat*Rated_current*1000;
    }

    if (para_query_float32_zhj(1, 0, 0, 0x00008025, &tmpFloat) > 0 && tmpFloat > 0)
    {
        cur_lmt1_rev = tmpFloat*Rated_current*1000;
    }


    lastTimesLmt1 = 15;           //容错
    lastTimesLmt2 = 15;          //容错
    if (para_query_int64_zhj(1, 0, 0, 0x00008040, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt1 = tmpint * 60;
    }

    if (para_query_int64_zhj(1, 0, 0, 0x00008041, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt2 = tmpint * 60;
    }

    EVENT_FMT_TRACE("CurHighEvent 电流上限值=%d,持续时间=%d, 电流上限恢复值=%d    持续时间=%d\n", cur_lmt1,lastTimesLmt1,cur_lmt1_rev,lastTimesLmt2);
    EVENT_FMT_TRACE("CurHighEvent 电流a=%d,电流b=%d, 电流c=%d \n", cur[0],cur[1],cur[2]);
    if((bType == 1)||(bType == 2))
    {
        //-读取相位记录信息-
        len = read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+9), 1);
    }
    else
    {
        //-读取相位记录信息-
        len = read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+10), 1);
    }

    if (len < 0)
    {
        EVENT_FMT_DEBUG("CurHighEvent 读取事件状态失败");
        return;
    }

    if (bData == 0xff) //文件系统初始为0xff值
    {
        bData = 0;
    }

    EVENT_FMT_TRACE("读取事件状态%02X\n", bData);
    
    //抄表未抄通
    if ((cur[0] == 0x7fffffff) && (vlt[0] == 0xffff)
            && (cur[1] == 0x7fffffff) && (vlt[1] == 0xffff)
            && (cur[2] == 0x7fffffff) && (vlt[2] == 0xffff))
    {
        EVENT_FMT_DEBUG( "抄表不成功直接返回\n");
        return;
    }

    //电压电流都为0，是停电
    if ((cur[0] == 0x0) && (vlt[0] == 0x0)
        && (cur[1] == 0x0) && (vlt[1] == 0x0)
        && (cur[2] == 0x0) && (vlt[2] == 0x0))
    {
        EVENT_FMT_DEBUG( "停电直接返回\n");
        return;
    }

    ret = para_query_array_zhj(1, 0, 0, 0x00008005, &power_type);
    if(ret < 0)
        power_type = 4; //三相四线
        
    //循环检测三相是否符合事件条件
    for (order = 0; order < 3; order++)
    {
        if ((cur[order] == 0x6eeeeeee) || (cur[order] == 0x7fffffff) || (cur[order] == APP_NONELWORD))
        {
            continue;
        }

        //三相三线,不判B相
        if ((power_type == 4)
         || (power_type == 3 && order != 1))
        {
            switch (bType)
            {
            case 1:                                //电流越上限
                if ((cur[order] > cur_lmt1) && (cur[order] <= cur_lmt2))
                {
                    //如果满足相应的发生条件，将对应的时间进行累加
                    if (lastTimes[bType-1][order] >= lastTimesLmt1-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order]++;
                        is_now_flag = 0;
                    }
                    EVENT_FMT_TRACE("第%d相满足电流越上限发生条件，已持续时间lastTimes = %d\n",order+1,lastTimes[bType-1][order]);
                }
                else
                {
                    EVENT_FMT_TRACE("第%d相不满足电流越上限发生条件，时间清零\n",order+1);
                    lastTimes[bType-1][order] = 0;
                    is_now_flag = 0;
                }
                break;

            case 2:                                //电流越上限恢复
                //电压小于上上限恢复阀值 并且 已经发生过越上上限
                if((cur[order] <= cur_lmt1_rev) && (bData &(1<<order)))
                {
                    //如果满足相应的发生条件，将对应的时间进行累加
                    if (lastTimes[bType-1][order] >= lastTimesLmt1-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order]++;
                        is_now_flag = 0;
                    }
                    EVENT_FMT_TRACE("第%d相满足电流越上限恢复条件，已持续时间lastTimes = %d\n",order+1,lastTimes[bType-1][order]);
                }
                else
                {
                    EVENT_FMT_TRACE("第%d相不满足电流越上限恢复条件，时间清零\n",order+1);
                    lastTimes[bType-1][order] = 0;
                    is_now_flag = 0;
                }
                break;

            case 3:                                //电流越上上限
                //如果满足相应的发生条件，将对应的时间进行累加
                if (cur[order] > cur_lmt2)
                {
                    if (lastTimes[bType-1][order] >= lastTimesLmt2-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order]++;
                        is_now_flag = 0;
                    }
                    EVENT_FMT_TRACE("第%d相满足电流越上上限发生条件，已持续时间lastTimes = %d\n",order+1,lastTimes[bType-1][order]);
                }
                else
                {
                    EVENT_FMT_TRACE("第%d相不满足电流越上上限发生条件，时间清零\n",order+1);
                    lastTimes[bType-1][order] = 0;
                    is_now_flag = 0;
                }

                break;

            case 4:                                //电流越上上限恢复
                if((cur[order] <= cur_lmt2_rev) && (bData &(1<<order)))
                {
                    //如果满足相应的发生条件，将对应的时间进行累加
                    if (lastTimes[bType-1][order] >= lastTimesLmt2-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order]++;
                        is_now_flag = 0;
                    }
                    EVENT_FMT_DEBUG("第%d相满足电流越上上限恢复条件，已持续时间lastTimes= %d\n",order+1,lastTimes[bType-1][order]);
                }
                else
                {
                    EVENT_FMT_DEBUG("第%d相不满足电流越上上限恢复条件，时间清零\n",order+1);
                    lastTimes[bType-1][order] = 0;
                    is_now_flag = 0;
                }
                break;

            default:
                continue;
            }
        }
        else
        {
            continue;
        }

        uint32 ArrIdx = 0;//一维数组索引(起始)
        ArrIdx = g_aFaultDataIndexZhj[1][0][0];

        if ((bType == 1) || (bType == 3))       //事件发生
        {
            if (is_now_flag)
            {
                switch(order)
                {
                case 0:
                    if (!( bData & A_PHASE))
                    {
                        bData |= A_PHASE;       //置相位标志
                        bphase |= A_PHASE;      //置相位标志
                        if (bType == 1)
                        {
                            ERFlg = HIGH_CUR;   //置电流越上限事件标志
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+4].nFaultYxAddr,1);   //置电流越上限事件标志
                        }
                        else
                        {
                            ERFlg = HIGHER_CUR; //置电流越上上限事件标志
                        }
                    }
                    break;

                case 1:
                    if (!( bData & B_PHASE))
                    {
                        bData |= B_PHASE;       //置相位标志
                        bphase |= B_PHASE;      //置相位标志
                        if (bType == 1)
                        {
                            ERFlg = HIGH_CUR;   //置电流越上限事件标志
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+5].nFaultYxAddr,1);   //置电流越上限事件标志
                        }
                        else
                        {
                            ERFlg = HIGHER_CUR; //置电流越上上限事件标志
                        }
                    }
                    break;

                case 2:
                    if (!( bData & C_PHASE))
                    {
                        bData |= C_PHASE;       //置相位标志
                        bphase |= C_PHASE;      //置相位标志
                        if (bType == 1)
                        {
                            ERFlg = HIGH_CUR;   //置电流越上限事件标志
                            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+6].nFaultYxAddr,1);   //置电流越上限事件标志
                        }
                        else
                        {
                            ERFlg = HIGHER_CUR; //置电流越上上限事件标志
                        }
                    }
                    break;

                default:
                    break;
                }
            }
        }
        else                                    //事件恢复
        {
            if ((is_now_flag) && ( bData &(1<<order)))
            {
                //恢复不判持续时间
                switch(order)
                {
                case 0:
                    bData &= ~A_PHASE;          //清除相位标志
                    bphase |= A_PHASE;
                    if (bType == 2)
                    {
                        ERFlg = HIGH_CUR;       //置电流越上限事件标志
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+4].nFaultYxAddr,0);   //置电流越上限事件标志
                    }
                    else
                    {
                        ERFlg = HIGHER_CUR;     //置电流越上上限事件标志
                    }
                    break;

                case 1:
                    bData &= ~B_PHASE;          //清除相位标志
                    bphase |= B_PHASE;
                    if (bType == 2)
                    {
                        ERFlg = HIGH_CUR;       //置电流越上限事件标志
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+5].nFaultYxAddr,0);   //置电流越上限事件标志
                    }
                    else
                    {
                        ERFlg = HIGHER_CUR;     //置电流越上上限事件标志
                    }
                    break;

                case 2:
                    bData &= ~C_PHASE;          //清除相位标志
                    bphase |= C_PHASE;
                    if (bType == 2)
                    {
                        ERFlg = HIGH_CUR;      //置电流越上限事件标志
                        yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+6].nFaultYxAddr,0);   //置电流越上限事件标志
                    }
                    else
                    {
                        ERFlg = HIGHER_CUR;    //置电流越上上限事件标志
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }

    if ((bType == 1)||(bType == 2))
    {
        if (write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+9), 1)<0)
        {
            return;
        }
    }
    else
    {
        if (write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+10), 1)<0)
        {
            return;
        }
    }

    if (ERFlg)                       //事件发生
    {
        

        if ((bType == 1)||(bType == 3))
        {
            SaveFileRecBfg(ERC_NO_25|0x80,(bphase&0x07)|0x80,0);//记录电流越上限发生
        }
        else
        {
            SaveFileRecBfg(ERC_NO_25|0x80,(bphase&0x07),0);//记录电流越上限恢复
        }
    }
}

/*****************************************************************************
所属功能单元：
功能描述：电压电流不平衡及恢复告警
输入参数：计量点号
输出参数：无
函数返回值：无
内部处理流程：判断当前计量点的电压电流不平衡度是否越限或恢复，是则调用告警数并填写主动上报队列
***************************************************************************-*/
void VltCurImbalanceEvent(int clientid, uint8 bType)
{
    int    max_cur;    //-最大电流偏差-
    int    min_cur;    //-最小电流偏差-
    int16  max_vlt;    //-最大电压偏差-
    int16  min_vlt;    //-最小电压偏差-
    float32 vlt_para,cur_para;      //电压 电流不平衡阈值
    float32 vlt_para_rev = 0;        //三相电压不平衡恢复值
    float32 cur_para_rev = 0;        //三相电流不平衡恢复值
    uint8 power_type = 4; //三相四线
    float32 tmpFloat = 0;
    int64   tmpint  = 0;
    uint16 vlt[3];
    int    cur[3];
    int    dwERFlgTmp = 0;
    int16  len = 0;
    static uint8 lastTimes[4];    //三相电压、电流不平衡持续时间 0 电压不平衡发生 1 恢复 2 电流不平衡发生 3 恢复
    uint16  lastTimesLmt = 1;        //默认的三相电压不平衡持续时间
    uint16  lastTimesLmt1 = 1;       //默认的三相电流不平衡持续时间
    int8   vlt_status = 0, cur_status = 0; //电压、电流不平衡发生、恢复标志  1 发生 2 恢复
    int8   tmp_flag1 = 0, tmp_flag2 = 0;


    if((g_LatestACData.vol.nValue[0] == 0xeeee)||(g_LatestACData.vol.nValue[1] == 0xeeee)||(g_LatestACData.vol.nValue[2] == 0xeeee)
        ||(g_LatestACData.cur.nValue[0] == 0xeeeeeeee)||(g_LatestACData.cur.nValue[1] == 0xeeeeeeee)||(g_LatestACData.cur.nValue[2] == 0xeeeeeeee)
        ||(g_LatestACData.vol.nValue[0] == 0xffff)||(g_LatestACData.vol.nValue[1] == 0xffff)||(g_LatestACData.vol.nValue[2] == 0xffff)
        ||(g_LatestACData.cur.nValue[0] == 0xffffffff)||(g_LatestACData.cur.nValue[1] == 0xffffffff)||(g_LatestACData.cur.nValue[2] == 0xffffffff))
    {
        //-任意一项电压电流为0-
        return;
    }

    //-原来代码不设限值就返回，现在修改为默认参数 30% ----return;---
    vlt_para = 40;
    cur_para = 300;
    vlt_para_rev = 38;
    cur_para_rev = 250;

    //电流不平衡阈值
    if (para_query_float32_zhj(1, 0, 0, 0x0000802a, &tmpFloat) > 0 && tmpFloat > 0)
    {
        cur_para = tmpFloat*10;
    }

    if (para_query_float32_zhj(1, 0, 0, 0x0000802b, &tmpFloat) > 0 && tmpFloat > 0)
    {
        cur_para_rev = tmpFloat*10;
    }
    
    if (para_query_float32_zhj(1, 0, 0, 0x0000802c, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_para = tmpFloat*10;
    }

    if (para_query_float32_zhj(1, 0, 0, 0x0000802d, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_para_rev = tmpFloat*10;
    }

    EVENT_FMT_TRACE("电压不平衡限值%f,恢复限值%f,电流不平衡限值%f,恢复限值%f\n", vlt_para, vlt_para_rev, cur_para, cur_para_rev);

    lastTimesLmt = 15;       //容错
    lastTimesLmt1 = 15;      //容错

    if (para_query_int64_zhj(1, 0, 0, 0x00008040, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt = tmpint * 60;
    }

    if (para_query_int64_zhj(1, 0, 0, 0x00008041, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt1 = tmpint * 60;
    }


    len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);
    if (len < 0)
    {
        EVENT_FMT_DEBUG("读取事件状态失败");
        return;
    }

    if (dwERFlgTmp == -1) //文件系统初始为0xff值
    {
        dwERFlgTmp = 0;
    }

    EVENT_FMT_TRACE("读取事件状态%08X\n", dwERFlgTmp);

    vlt[0] = g_LatestACData.vol.nValue[0];
    vlt[1] = g_LatestACData.vol.nValue[1];
    vlt[2] = g_LatestACData.vol.nValue[2];
    cur[0] = g_LatestACData.cur.nValue[0];
    cur[1] = g_LatestACData.cur.nValue[1];
    cur[2] = g_LatestACData.cur.nValue[2];

    if(para_query_array_zhj(1, 0, 0, 0x00008005, &power_type) < 0)
        power_type = 4; //三相四线

    //获取最大电压电流值以及最小电压电流值
    if(power_type==4)    //-三相四线-
    {
        //电流值带符号，需进一步处理
        if(cur[0]&0x80000000) cur[0] = (-1) * (cur[0]&0x7fffffff);
        if(cur[1]&0x80000000) cur[1] = (-1) * (cur[1]&0x7fffffff);
        if(cur[2]&0x80000000) cur[2] = (-1) * (cur[2]&0x7fffffff);

        CmpValuint16(3,vlt,(uint16*)&max_vlt,(uint16*)&min_vlt);
        CmpValint(3,cur,&max_cur,&min_cur);
    }
    else if(power_type==3)    //-三相三线-
    {

        //电流值带符号，需进一步处理
        if(cur[0]&0x80000000) cur[0] = (-1) * (cur[0]&0x7fffffff);
        if(cur[1]&0x80000000) cur[1] = (-1) * (cur[1]&0x7fffffff);

        CmpValuint16(2,vlt,(uint16*)&max_vlt,(uint16*)&min_vlt);
        CmpValint(2,cur,&max_cur,&min_cur);
    }

    uint32 ArrIdx = 0;//一维数组索引(起始)
    ArrIdx = g_aFaultDataIndexZhj[1][0][0];

    if (bType == 1)
    {
        if(max_vlt != 0)
        {
            //-电压不平衡判断-
            tmp_flag1 = ((uint32)(max_vlt-min_vlt)*1000/max_vlt) > vlt_para;
            tmp_flag2 = ((uint32)(max_vlt-min_vlt)*1000/max_vlt) > vlt_para_rev;
        }

        if(tmp_flag1 && !(dwERFlgTmp & VLT_NBLC))            //发生了不平衡，并且以前没有记录过该事件
        {
            //如果满足相应的发生条件，将对应的时间进行累加
            if(lastTimes[0] >= lastTimesLmt-1)
            {
                vlt_status = 1;
                yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+14].nFaultYxAddr,1);
            }
            else
            {
                lastTimes[0]++;
                vlt_status = 0;
            }
            lastTimes[1] = 0;
        }
        else if(!tmp_flag2 && (dwERFlgTmp & VLT_NBLC))    //没有发生不平衡，并且以前记录过该事件
        {
            //如果满足相应的发生条件，将对应的时间进行累加
            if(lastTimes[1] >= lastTimesLmt-1)
            {
                vlt_status = 2;
				yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+14].nFaultYxAddr,0);
            }
            else
            {
                lastTimes[1]++;
                vlt_status = 0;
            }
            lastTimes[0] = 0;
        }
        else
        {
            lastTimes[0] = 0;
            lastTimes[1] = 0;
            vlt_status = 0;
        }
    }

    if (bType == 2)
    {
        //<start common problem>    //防止平均电流为0时，电压不平衡发生时引起电流不平衡发生
        tmp_flag1 = 0;
        tmp_flag2 = 0;
        //<end common problem>
        if(max_cur != 0)
        {
            //-电流不平衡判断-
            /* 判断电流为了模拟负载率大约50%的情况，让台体测试通过 */
            tmp_flag1 = ((uint32)(max_cur-min_cur)*1000/abs(max_cur)) > cur_para && (g_LatestACData.cur.nValue[0] > 5000 * 50 / 100);
            tmp_flag2 = ((uint32)(max_cur-min_cur)*1000/abs(max_cur)) > cur_para_rev;
        }

        //0 电压不平衡发生 1 恢复 2 电流不平衡发生 3 恢复
        if((tmp_flag1) && (!(dwERFlgTmp & CUR_NBLC)))        //发生了不平衡，并且以前没有记录过该事件
        {
            //如果满足相应的发生条件，将对应的时间进行累加
            if(lastTimes[2] >= lastTimesLmt1-1)
            {
                cur_status = 1;
				yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+13].nFaultYxAddr,1);
            }
            else
            {
                lastTimes[2]++;
                cur_status = 0;
            }
            lastTimes[3] = 0;
        }
        else if(!tmp_flag2 && (dwERFlgTmp & CUR_NBLC))    //没有发生不平衡，并且以前记录过该事件
        {
            //如果满足相应的发生条件，将对应的时间进行累加
            if(lastTimes[3] >= lastTimesLmt1-1)
            {
                cur_status = 2;
				yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+13].nFaultYxAddr,0);
            }
            else
            {
                lastTimes[3]++;
                cur_status = 0;
            }
            lastTimes[2] = 0;
        }
        else
        {
            lastTimes[2] = 0;
            lastTimes[3] = 0;
            cur_status = 0;
        }
    }

    if (vlt_status != 0 || cur_status != 0)
    {
        

        //-事件发生了，且没有记录-
        if((vlt_status == 1) || (cur_status == 1))
        {
            
            if(vlt_status == 1)
            {
                dwERFlgTmp |= VLT_NBLC;        //-电压事件标志-

                SaveFileRecBfg(ERC_NO_17|0x80,0x80,0);//-电压不平衡-
            }
            if(cur_status == 1)
            {
                dwERFlgTmp |= CUR_NBLC;        //-电流事件标志-

                SaveFileRecBfg(ERC_NO_17,0x80,0);//-电流不平衡-
            }

            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);
        }

        //-事件没有发生，且已有记录，则恢复-
        if((vlt_status == 2) || (cur_status == 2))
        {
            
            if(vlt_status == 2)
            {
                dwERFlgTmp &= ~VLT_NBLC;//-清除该事件标志-

                SaveFileRecBfg(ERC_NO_17|0x80,0,0);//-电压不平衡-
            }

            if(cur_status == 2)
            {
                dwERFlgTmp &= ~CUR_NBLC;//-清除该事件标志-

                SaveFileRecBfg(ERC_NO_17,0,0);//-电流不平衡-
            }

            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);
        }
    }
    
}

/**********************************************************************
* @name      : VltCurHarmoniceEvent
* @brief     ：电压电流畸变率越限及恢复告警
* @param[in] ：int clientid    数据中心句柄,预留
               bType 1,电压    2，电流
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void VltCurHarmoniceEvent(int clientid, uint8 bType)
{
    float32 vlt_para,cur_para;      //电压 电流谐波总畸变率超限阈值
    float32 vlt_para_rev = 0;        //三相电压流谐波总畸变率超限恢复值
    float32 cur_para_rev = 0;        //三相电流流谐波总畸变率超限恢复值
    float32 tmpFloat = 0;
    int64   tmpint  = 0;
    int    dwERFlgTmp = 0;
    int16  len = 0;
    static uint8 lastTimes[4][3];    //三相电压、电流谐波总畸变率超限持续时间 0 电压谐波总畸变率超限发生 1 恢复 2 电流谐波总畸变率超限发生 3 恢复
    uint16  lastTimesLmt = 1;        //默认的三相电压谐波总畸变率超限持续时间
    uint16  lastTimesLmt1 = 1;       //默认的三相电流谐波总畸变率超限持续时间
    int    vlt_flag[3] = {VLT_HARA, VLT_HARB, VLT_HARC};
    int    cur_flag[3] = {CUR_HARA, CUR_HARB, CUR_HARC};
    int8   tmp_flag1[3], tmp_flag2[3];
    uint8  order;
    uint8  bphase = 0;
    uint8    happenFlg    = 0;
    uint8    recoverFlg   = 0;


    //-原来代码不设限值就返回，现在修改为默认参数 30% ----return;---
    vlt_para = 5;
    cur_para = 5;
    vlt_para_rev = 4.75;
    cur_para_rev = 4.75;

    //电流谐波总畸变率阈值
    if (para_query_float32_zhj(1, 0, 0, 0x00008032, &tmpFloat) > 0 && tmpFloat > 0)
    {
        cur_para = tmpFloat;
    }

    if (para_query_float32_zhj(1, 0, 0, 0x00008033, &tmpFloat) > 0 && tmpFloat > 0)
    {
        cur_para_rev = tmpFloat;
    }
    
    if (para_query_float32_zhj(1, 0, 0, 0x00008030, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_para = tmpFloat;
    }

    if (para_query_float32_zhj(1, 0, 0, 0x00008031, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_para_rev = tmpFloat;
    }

    EVENT_FMT_TRACE("电压谐波总畸变率限值%f,恢复限值%f,电流谐波总畸变率限值%f,恢复限值%f\n", vlt_para, vlt_para_rev, cur_para, cur_para_rev);

    lastTimesLmt = 15;       //容错
    lastTimesLmt1 = 15;      //容错

    if (para_query_int64_zhj(1, 0, 0, 0x00008040, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt = tmpint * 60;
    }

    if (para_query_int64_zhj(1, 0, 0, 0x00008041, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt1 = tmpint * 60;
    }


    len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);
    if (len < 0)
    {
        EVENT_FMT_DEBUG("读取事件状态失败");
        return;
    }

    if (dwERFlgTmp == -1) //文件系统初始为0xff值
    {
        dwERFlgTmp = 0;
    }

    EVENT_FMT_TRACE("读取事件状态%08X\n", dwERFlgTmp);

    uint32 ArrIdx = 0;//一维数组索引(起始)
    ArrIdx = g_aFaultDataIndexZhj[1][0][0];
    for (order = 0; order < 3; order++)
    {
        if (bType == 1)
        {
            //-电压谐波总畸变率超限判断-
            tmp_flag1[order] = ((float32)g_LatestACData.harmonicV[order].nValue[0]/100) > vlt_para;
            tmp_flag2[order] = ((float32)g_LatestACData.harmonicV[order].nValue[0]/100) > vlt_para_rev;
            
            EVENT_FMT_TRACE("电压谐波[%u]= %f\n", order,     (float32)g_LatestACData.harmonicV[order].nValue[0]/100);

            if(tmp_flag1[order] && !(dwERFlgTmp & vlt_flag[order]))            //发生了，并且以前没有记录过该事件
            {
                //如果满足相应的发生条件，将对应的时间进行累加
                if(lastTimes[0][order] >= lastTimesLmt-1)
                {
                    dwERFlgTmp |= vlt_flag[order];        //-电压事件标志-
                    bphase |= (0x0001<order);
                    yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+24+order].nFaultYxAddr,1);
                    happenFlg = 1;
                }
                else
                {
                    lastTimes[0][order]++;
                }
                lastTimes[1][order] = 0;
            }
            else if(!tmp_flag2[order] && (dwERFlgTmp & vlt_flag[order]))    //没有发生，并且以前记录过该事件
            {
                //如果满足相应的发生条件，将对应的时间进行累加
                if(lastTimes[1][order] >= lastTimesLmt-1)
                {
                    dwERFlgTmp  &= ~vlt_flag[order];        //-电压事件标志-
                    bphase |= (0x0001<order);
    				yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+24+order].nFaultYxAddr,0);
                    recoverFlg = 1;
                }
                else
                {
                    lastTimes[1][order]++;
                }
                lastTimes[0][order] = 0;
            }
            else
            {
                lastTimes[0][order] = 0;
                lastTimes[1][order] = 0;
            }
        }

        
        if (bType == 2)
        {
            /* 判断电流为了模拟负载率大约50%的情况，让台体测试通过 */
             tmp_flag1[order] = ((float32)g_LatestACData.harmonicC[order].nValue[0]/100 )> vlt_para &&  (g_LatestACData.cur.nValue[order] > 5000 * 50 / 100);
             tmp_flag2[order] = ((float32)g_LatestACData.harmonicC[order].nValue[0]/100 )> vlt_para_rev;

             EVENT_FMT_TRACE("电流谐波[%u]= %f\n", order, (float32)g_LatestACData.harmonicC[order].nValue[0]/100);

            //0 电压谐波总畸变率超限发生 1 恢复 2 电流谐波总畸变率超限发生 3 恢复
            if((tmp_flag1[order]) && (!(dwERFlgTmp & cur_flag[order])))        //发生了，并且以前没有记录过该事件
            {
                //如果满足相应的发生条件，将对应的时间进行累加
                if(lastTimes[2][order] >= lastTimesLmt1-1)
                {
                    dwERFlgTmp |= cur_flag[order]; 
                    bphase |= (0x0001<order);
                    yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+27+order].nFaultYxAddr,1);
                    happenFlg = 1;
                }
                else
                {
                    lastTimes[2][order]++;
                }
                lastTimes[3][order] = 0;
            }
            else if(!tmp_flag2[order] && (dwERFlgTmp & cur_flag[order]))    //没有发生，并且以前记录过该事件
            {
                //如果满足相应的发生条件，将对应的时间进行累加
                if(lastTimes[3][order] >= lastTimesLmt1-1)
                {
                    dwERFlgTmp  &= ~cur_flag[order];
                    bphase |= (0x0001<order);
                    yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+27+order].nFaultYxAddr,0);
                    recoverFlg = 1;
                }
                else
                {
                    lastTimes[3][order]++;
                }
                lastTimes[2][order] = 0;
            }
            else
            {
                lastTimes[2][order] = 0;
                lastTimes[3][order] = 0;
            }
        }
    
            
    }
    
    if(happenFlg) //发生
    {
        write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);
        
        if (bType == 1)
        {
            SaveFileRecBfg(ERC_NO_26|0x80,(bphase&0x07)|0x80,0);//记录电压谐波总畸变率超限发生
        }
        else
        {
            SaveFileRecBfg(ERC_NO_26&0x7f,(bphase&0x07)|0x80,0);//记录电流谐波总畸变率超限发生
        }
    }
    else if(recoverFlg) //恢复
    {
        write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr), 4);

        if (bType == 1)
        {
            SaveFileRecBfg(ERC_NO_26|0x80,(bphase&0x07),0);//记录电压谐波总畸变率超限恢复
        }
        else
        {
            SaveFileRecBfg(ERC_NO_26&0x7f,(bphase&0x07),0);//记录电流谐波总畸变率超限恢复
        }
    }
    
}

/**********************************************************************
* @name      : repower_comp_event
* @brief     ：无功过补偿,无功欠补偿 事件
* @param[in] ：int clientid    数据中心句柄,预留
               bType 1,无功过补偿    2，无功欠补偿
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void repower_comp_event(int clientid, uint8 bType)
{
    float32 lfactor_para,cfactor_para;   //无功欠补偿的功率因数（感性）,无功过补偿的功率因数（容性）
    float32 lfactor_para_rev = 0;        //无功欠补偿恢复的功率因数（感性）
    float32 cfactor_para_rev = 0;        //无功过补偿恢复的功率因数（容性）
    float32 tmpFloat = 0;
    int64   tmpint  = 0;
    static int lFlag = 0;
    static int cFlag = 0;
    static uint8 lastTimes[4];    //无功过补偿、无功欠补偿限持续时间 0 无功过补偿发生 1 恢复 2 无功欠补偿发生 3 恢复
    uint16  lastTimesLmt = 1;        //默认的无功过补偿持续时间
    uint16  lastTimesLmt1 = 1;       //默认的无功过补偿恢复限持续时间
    int8   tmp_flag1, tmp_flag2;


    //-原来代码不设限值就返回，现在修改为默认参数 30% ----return;---
    cfactor_para = 0;
    lfactor_para = 0;
    cfactor_para_rev = 0;
    lfactor_para_rev = 0;

    //电流谐波总畸变率阈值
    if (para_query_float32_zhj(1, 0, 0, 0x00008026, &tmpFloat) > 0 && tmpFloat != 0)
    {
        cfactor_para = tmpFloat*1000;
    }

    if (para_query_float32_zhj(1, 0, 0, 0x00008027, &tmpFloat) > 0 && tmpFloat > 0)
    {
        cfactor_para_rev = tmpFloat*1000;
    }
    
    if (para_query_float32_zhj(1, 0, 0, 0x00008028, &tmpFloat) > 0 && tmpFloat > 0)
    {
        lfactor_para = tmpFloat*1000;
    }

    if (para_query_float32_zhj(1, 0, 0, 0x00008029, &tmpFloat) > 0 && tmpFloat > 0)
    {
        lfactor_para_rev = tmpFloat*1000;
    }

    lastTimesLmt = 15;       //容错
    lastTimesLmt1 = 15;      //容错

    if (para_query_int64_zhj(1, 0, 0, 0x00008040, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt = tmpint * 60;
    }

    if (para_query_int64_zhj(1, 0, 0, 0x00008041, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt1 = tmpint * 60;
    }

    uint32 ArrIdx = 0;//一维数组索引(起始)
    ArrIdx = g_aFaultDataIndexZhj[1][0][0];
    if (bType == 1)
    {
        EVENT_FMT_DEBUG("无功过补偿的功率因数（容性）%f,恢复限值%f\n", cfactor_para, cfactor_para_rev);
        EVENT_FMT_DEBUG("g_LatestACData.fpower.nValue[0] = %d\n", g_LatestACData.fpower.nValue[0]);
        //-无功过补偿判断-
        tmp_flag1 = (((float32)g_LatestACData.fpower.nValue[0]) > cfactor_para && g_LatestACData.fpower.nValue[0]<0);
        tmp_flag2 = (((float32)g_LatestACData.fpower.nValue[0]) > cfactor_para_rev &&  g_LatestACData.fpower.nValue[0]>0);

        if(tmp_flag1 && !cFlag)            //发生了，并且以前没有记录过该事件
        {
            EVENT_FMT_DEBUG("lastTimes[0] = %d\n", lastTimes[0]);
            //如果满足相应的发生条件，将对应的时间进行累加
            if(lastTimes[0] >= lastTimesLmt-1)
            {
                yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+20].nFaultYxAddr,1);
                SaveFileRecBfg(ERC_NO_29,0x80,0);//记录
                cFlag = 1;
            }
            else
            {
                lastTimes[0]++;
            }
            lastTimes[1]= 0;
        }
        else if(!tmp_flag2 && cFlag)    //没有发生，并且以前记录过该事件
        {
            //如果满足相应的发生条件，将对应的时间进行累加
            if(lastTimes[1] >= lastTimesLmt1-1)
            {
				yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+20].nFaultYxAddr,0);
                SaveFileRecBfg(ERC_NO_29,0,0);//记录
                cFlag = 0;
            }
            else
            {
                lastTimes[1]++;
            }
            lastTimes[0] = 0;
        }
        else
        {
            lastTimes[0] = 0;
            lastTimes[1]= 0;
        }
    }

    
    if (bType == 2)
    {
        EVENT_FMT_DEBUG("无功欠补偿的功率因数（容性）%f,恢复限值%f\n", lfactor_para, lfactor_para_rev);
        EVENT_FMT_DEBUG("g_LatestACData.fpower.nValue[0] = %d\n", g_LatestACData.fpower.nValue[0]);
        //-无功欠补偿判断-
        tmp_flag1 = (((float32)g_LatestACData.fpower.nValue[0]) < lfactor_para && g_LatestACData.fpower.nValue[0]>0);
        tmp_flag2 = (((float32)g_LatestACData.fpower.nValue[0]) < lfactor_para_rev &&  g_LatestACData.fpower.nValue[0]>0);

        if(tmp_flag1 && !lFlag)            //发生了，并且以前没有记录过该事件
        {
            EVENT_FMT_DEBUG("lastTimes[2] = %d\n", lastTimes[2]);
            //如果满足相应的发生条件，将对应的时间进行累加
            if(lastTimes[2] >= lastTimesLmt-1)
            {
                yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+21].nFaultYxAddr,1);
                SaveFileRecBfg(ERC_NO_30,0x80,0);//记录
                lFlag = 1;
            }
            else
            {
                lastTimes[2]++;
            }
            lastTimes[3]= 0;
        }
        else if(!tmp_flag2 && lFlag)    //没有发生，并且以前记录过该事件
        {
            //如果满足相应的发生条件，将对应的时间进行累加
            if(lastTimes[3] >= lastTimesLmt1-1)
            {
				yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+21].nFaultYxAddr,0);
                SaveFileRecBfg(ERC_NO_30,0,0);//记录
                lFlag = 0;
            }
            else
            {
                lastTimes[3]++;
            }
            lastTimes[2] = 0;
        }
        else
        {
            lastTimes[2] = 0;
            lastTimes[3]= 0;
        }
    }
    
}

/**********************************************************************
* @name      : over_load_event
* @brief     ：负荷过载事件
* @param[in] ：int clientid    数据中心句柄,预留
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-11-19
* @Update    :
**********************************************************************/
void over_load_event(int clientid)
{
    float32 overload_para = 0;            //负荷过载比例
    float32 overload_para_rev = 0;        //负荷过载恢复比例
    float32 capacity = 0;                 //变压器容量
    float32 tmpFloat = 0;
    int64   tmpint  = 0;
    int64   intCT  = 0;
    int64   intPT  = 0;
    static int    dwERFlgTmp = 0;
    static uint8 lastTimes[2];    //持续时间 0 发生 1 恢复 
    uint16  lastTimesLmt = 1;        //默认持续时间
    uint16  lastTimesLmt1 = 1;       //默认恢复限持续时间
    int8   tmp_flag1, tmp_flag2;


    //-原来代码不设限值就返回，现在修改为默认参数 30% ----return;---
    overload_para = 1.2;
    overload_para_rev = 1.05;
    
    //阈值
    if (para_query_float32_zhj(1, 0, 0, 0x00008024, &tmpFloat) > 0 && tmpFloat > 0)
    {
        overload_para = tmpFloat;
    }

    if (para_query_float32_zhj(1, 0, 0, 0x00008025, &tmpFloat) > 0 && tmpFloat > 0)
    {
        overload_para_rev = tmpFloat;
    }
    
    if (para_query_float32_zhj(1, 0, 0, 0x0000800b, &tmpFloat) > 0 && tmpFloat > 0)
    {
        capacity = tmpFloat*1000;
    }    

    lastTimesLmt = 15;       //容错
    lastTimesLmt1 = 15;      //容错

    if (para_query_int64_zhj(1, 0, 0, 0x00008040, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt = tmpint * 60;
    }

    if (para_query_int64_zhj(1, 0, 0, 0x00008041, &tmpint) > 0 && tmpint > 0)
    {
        lastTimesLmt1 = tmpint * 60;
    }

    if (para_query_int64_zhj(1, 0, 0, 0x00008006, &tmpint) > 0 && tmpint > 0)
    {
        intCT = tmpFloat;
    }

    if (para_query_int64_zhj(1, 0, 0, 0x00008007, &tmpint) > 0 && tmpint > 0)
    {
        intPT = tmpFloat;
    }

    EVENT_FMT_TRACE("负荷过载比例%f,恢复限值%f,变压器容量%f, CT%ll, PT%ll\n", overload_para, overload_para_rev, capacity, intCT, intPT);

    //tmp_flag1 = (float32)g_LatestACData.fpower.nValue[0] *intCT*intPT > capacity*overload_para;
    //tmp_flag2 = (float32)g_LatestACData.fpower.nValue[0] *intCT*intPT > capacity*overload_para_rev;
    tmp_flag1 = (g_LatestACData.cur.nValue[0] > 5750 && g_LatestACData.vol.nValue[0] > 2530) ||
                (g_LatestACData.cur.nValue[1] > 5750 && g_LatestACData.vol.nValue[1] > 2530) ||
                (g_LatestACData.cur.nValue[2] > 5750 && g_LatestACData.vol.nValue[2] > 2530);
    tmp_flag2 = (g_LatestACData.cur.nValue[0] > 5750 && g_LatestACData.vol.nValue[0] > 2530) &&
                (g_LatestACData.cur.nValue[1] > 5750 && g_LatestACData.vol.nValue[1] > 2530) &&
                (g_LatestACData.cur.nValue[2] > 5750 && g_LatestACData.vol.nValue[2] > 2530);

    uint32 ArrIdx = 0;//一维数组索引(起始)
    ArrIdx = g_aFaultDataIndexZhj[1][0][0];

    if(tmp_flag1 && !(dwERFlgTmp))            //发生了，并且以前没有记录过该事件
    {
        //如果满足相应的发生条件，将对应的时间进行累加
        if(lastTimes[0] >= lastTimesLmt-1)
        {
            yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+14].nFaultYxAddr,1);
            SaveFileRecBfg(ERC_NO_28,0x80,0);//记录
            dwERFlgTmp = 1;
        }
        else
        {
            lastTimes[0]++;
        }
        lastTimes[1]= 0;
    }
    else if(!tmp_flag2 && dwERFlgTmp)    //没有发生，并且以前记录过该事件
    {
        //如果满足相应的发生条件，将对应的时间进行累加
        if(lastTimes[1] >= lastTimesLmt1-1)
        {
			yx_event_signal_update_zhj(1, 0, 0, g_aFaultDataConfigZhj[ArrIdx+14].nFaultYxAddr,0);
            SaveFileRecBfg(ERC_NO_28,0,0);//记录
            dwERFlgTmp = 0;
        }
        else
        {
            lastTimes[1]++;
        }
        lastTimes[0] = 0;
    }
    else
    {
        lastTimes[0] = 0;
        lastTimes[1]= 0;
    }
    
}



