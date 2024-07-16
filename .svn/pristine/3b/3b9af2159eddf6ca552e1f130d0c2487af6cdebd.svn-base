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
#include "Event.h"
#include "EventFun.h"
#include "commfun.h"
#include "framecomm.h"
#include "oopVarUpdate.h"
#include "storage.h"
#include "../app104/include/pw_all.h"


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

#if 0
//获取当前时间
void GetCurnRtcTm(rtc_t *prtc)
{
    time_t timep;
    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep
    Sec_To_Rtc(timep, prtc);

    return;
}
#endif

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
    uint8 buf[10] = {0};

    EVENT_FMT_DEBUG("SaveFileRecBfg, ERC=%u, MTR=%8X\n", bEventType, msn);

    switch(bEventType&0x7F)//-最高位用来区分同一编号的两种事件-
    {
        case ERC_NO_09://电流反极性
        {//-电流反极性记录地址-
            ReadRamXData(GBPhaseReveAddr,(char*)buf, 9);

             /*-  进线开关(1QF)A相电流反极性告警   D0位表示是否电流反极性，位D4表示是否有变化
                             进线开关(1QF)B相电流反极性告警   D1位表示是否电流反极性，位D5表示是否有变化
                             进线开关(1QF)C相电流反极性告警   D2位表示是否电流反极性，位D6表示是否有变化-*/
            if(bPhase&0x80)//-发生-
            {
                buf[msn] = buf[msn]|(bPhase&0x07);//-添加相位信息-
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
            WriteRamXData(GBPhaseReveAddr, (char*)buf, 9);
            EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBPhaseReveAddr);
            break;
        }
        case ERC_NO_10:
        {   //-断相和失压使用同一个序号，在相位的最高位来区分，最高位为1表示断相，为0表示失压-
            if(bEventType&0x80)
            {//-电压断相记录地址-
                ReadRamXData(GBLostvltAddr,(char*)buf, 9);

                 /*-  进线开关(1QF)A相断相告警   D0位表示是否断相，位D4表示是否有变化
                                 进线开关(1QF)B相断相告警   D1位表示是否断相，位D5表示是否有变化
                                 进线开关(1QF)C相断相告警   D2位表示是否断相，位D6表示是否有变化-*/
                if(bPhase&0x80)//-发生-
                {
                    buf[msn] = buf[msn]|(bPhase&0x07);//-添加相位信息-
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
                WriteRamXData(GBLostvltAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBLostvltAddr);
            }
            else
            {//-电压失压记录地址
                ReadRamXData(GBLowvltAddr, (char*)buf, 9);

                /*-进线开关(1QF)A相失压告警   D0位表示是否失压，位D4表示是否有变化
                       进线开关(1QF)B相失压告警   D1位表示是否失压，位D5表示是否有变化
                        进线开关(1QF)C相失压告警   D2位表示是否失压，位D6表示是否有变化-*/
                if(bPhase&0x80)//-发生-
                {
                    buf[msn] = buf[msn]|(bPhase&0x07);//-添加相位信息-
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
                WriteRamXData(GBLowvltAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBLowvltAddr);
            }

            break;
        }
        case ERC_NO_11://电压/电流逆相序
        {   //-电压/电流逆相序使用同一个序号，在相位的最高位来区分，最高位为1表示电压不平衡，为0表示电流不平衡-
            if(bEventType&0x80)
            {//-电压逆相序记录地址-
                ReadRamXData(GBVltAbnormalAddr, (char*)buf, 9);

                //-buf[msn]定义如下:-
               /*-进线开关(1QF)电压逆相序告警   D0位表示是否电压逆相序，位D4表示是否有变化-*/
                if(bPhase&0x80)//-发生-
                {
                    buf[msn] = buf[msn]|0x01|0x10;
                }
                else//-恢复-
                {
                    buf[msn] = (buf[msn]&0xFE)|0x10;
                }
                WriteRamXData(GBVltAbnormalAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBVltAbnormalAddr);
            }
            else
            {//-电流逆相序记录地址
                ReadRamXData(GBCurrAbnormalAddr, (char*)buf, 9);

                //-buf[msn]定义如下:-
               /*-进线开关(1QF)电流逆相序告警   D0位表示是电流逆相序，位D4表示是否有变化-*/
                if(bPhase&0x80)//-发生-
                {
                    buf[msn] = buf[msn]|0x01|0x10;
                }
                else//-恢复-
                {
                    buf[msn] = (buf[msn]&0xFE)|0x10;
                }
                WriteRamXData(GBCurrAbnormalAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBCurrAbnormalAddr);
            }

            break;
        }
        case ERC_NO_14:
        {
            ReadRamXData(RTUSelfCheckAddr, (char*)buf, 9);
            if(bEventType&0x80)//停电记录
            {//-停电记录，需要记录停电或上电-
                if(bPhase&0x80)
                    buf[0] = buf[0]|0x01;
                else
                    buf[0] = buf[0]&0xFE;
                buf[1] = buf[1]|0x01;
            }
            else//自检错误记录
            {//-终端自检只需要记录是否发生，没有恢复的情况-
                buf[0] = buf[0]|0x02;
                buf[1] = buf[1]|0x02;
            }
            WriteRamXData(RTUSelfCheckAddr,(char*)buf, 9);
            EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", RTUSelfCheckAddr);
            break;
        }
        case ERC_NO_17:
        {   //-电压不平和和电流不平衡使用同一个序号，在相位的最高位来区分，最高位为1表示电压不平衡，为0表示电流不平衡-
            if(bEventType&0x80)
            {//-电压不平衡记录地址-
                ReadRamXData(GBVltUnbAddr, (char*)buf, 9);

                //-buf[msn]定义如下:-
               /*-进线开关(1QF)电压不平衡告警   D0位表示是否电压不平衡，位D4表示是否有变化-*/
                if(bPhase&0x80)//-发生-
                {
                    buf[msn] = buf[msn]|0x01|0x10;
                }
                else//-恢复-
                {
                    buf[msn] = (buf[msn]&0xFE)|0x10;
                }
                WriteRamXData(GBVltUnbAddr, (char*)buf, 9);
            }
            else
            {//-电流不平衡记录地址
                ReadRamXData(GBCurrUnbAddr, (char*)buf, 9);

                //-buf[msn]定义如下:-
               /*-进线开关(1QF)电流不平衡告警   D0位表示是电流不平衡，位D4表示是否有变化-*/
                if(bPhase&0x80)//-发生-
                {
                    buf[msn] = buf[msn]|0x01|0x10;
                }
                else//-恢复-
                {
                    buf[msn] = (buf[msn]&0xFE)|0x10;
                }
                WriteRamXData(GBCurrUnbAddr, (char*)buf, 9);
            }
            EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBCurrUnbAddr);
            break;
        }
        case ERC_NO_24:
        {   //-越上限和越下限使用同一个序号，在相位的最高位来区分，最高位为1表示越上限，为0表示越下限-
            if(bEventType&0x80)
            {//-电压越上限记录地址-
                ReadRamXData(GBVltUpAddr, (char*)buf, 9);
                //-buf[msn]定义如下:-
                /*-进线开关(1QF)A相越上限告警   D0位表示是否越上限，位D4表示是否有变化
                                     进线开关(1QF)B相越上限告警   D1位表示是否越上限，位D5表示是否有变化
                                     进线开关(1QF)C相越上限告警   D2位表示是否越上限，位D6表示是否有变化-*/
                if(bPhase&0x80)//-发生-
                {
                    buf[msn] = buf[msn]|(bPhase&0x07);//-添加相位信息-
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
                WriteRamXData(GBVltUpAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBVltUpAddr);
            }
            else
            {//-电压越下限记录地址
                ReadRamXData(GBVltDownAddr, (char*)buf, 9);

                //-buf[msn]定义如下:-
                /*-进线开关(1QF)A相越下限告警   D0位表示是否越下限，位D4表示是否有变化
                                     进线开关(1QF)B相越下限告警   D1位表示是否越下限，位D5表示是否有变化
                                     进线开关(1QF)C相越下限告警   D2位表示是否越下限，位D6表示是否有变化-*/
                if(bPhase&0x80)//-发生-
                {
                    buf[msn] = buf[msn]|(bPhase&0x07);//-添加相位信息-
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
                WriteRamXData(GBVltDownAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBVltDownAddr);
            }

            break;
        }
        case ERC_NO_25:
        {   //-电流过载和轻载使用同一个序号，在相位的最高位来区分，最高位为1表示过载，为0表示轻载-
            if(bEventType&0x80)
            {//-电流过载记录地址-
                ReadRamXData(GBCurrUpAddr, (char*)buf, 9);

                //-buf[msn]定义如下:-
                /*-进线开关(1QF)A相电流过载告警   D0位表示是否电流过载，位D4表示是否有变化
                                     进线开关(1QF)B相电流过载告警   D1位表示是否电流过载，位D5表示是否有变化
                                     进线开关(1QF)C相电流过载告警   D2位表示是否电流过载，位D6表示是否有变化-*/
                if(bPhase&0x80)//-发生-
                {
                    buf[msn] = buf[msn]|(bPhase&0x07);//-添加相位信息-
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
                WriteRamXData(GBCurrUpAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBCurrUpAddr);
            }
            else
            {//-电流轻载记录地址
                ReadRamXData(GBCurrDownAddr, (char*)buf, 9);

                //-buf[msn]定义如下:-
                /*-进线开关(1QF)A相电流轻载告警   D0位表示是否电流轻载，位D4表示是否有变化
                                     进线开关(1QF)B相电流轻载告警   D1位表示是否电流轻载，位D5表示是否有变化
                                     进线开关(1QF)C相电流轻载告警   D2位表示是否电流轻载，位D6表示是否有变化-*/
                if(bPhase&0x80)//-发生-
                {
                    buf[msn] = buf[msn]|(bPhase&0x07);//-添加相位信息-
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
                WriteRamXData(GBCurrDownAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBCurrDownAddr);
            }
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

    *linuxTim = mktime(&ptm);
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

/*根据传入的事件OI,返回要读取的记录事件序号的数据字典ID*/
uint16 GetEventRecNumIDByOI(uint16 EvtOI)
{
    uint16 ID = 0;
/*
    switch(EvtOI)
    {
    case 0x3105:
        ID = LastSucc_3105EventRecNO;            //3105H：时钟超差
        break;
    case 0x310B:
        ID = LastSucc_310BEventRecNO;            //310BH：电能表示度下降
        break;
    case 0x310C:
        ID = LastSucc_310CEventRecNO;            //310CH：电能量超差
        break;
    case 0x310D:
        ID = LastSucc_310DEventRecNO;            //310DH：电能表飞走
        break;
    case 0x310E:
        ID = LastSucc_310EEventRecNO;            //310EH：电能表停走
        break;
    case 0x310F:
        ID = LastSucc_310FEventRecNO;            //310FH：抄表失败
        break;
    default:
        ID=0;
        break;
    }
*/    return ID;
}

/*****************************************************************************
所属功能单元：
功能描述：为事件恢复做准备工作，将当前事件序号记录到字典中
输入参数：计量点号，oad
输出参数：无
函数返回值：无
内部处理流程：
***************************************************************************-*/

int PrepareResumeEvent(uint16 EvtOI,mtrnum_t mtr_id)
{/*
    uint32 curRecNum = 0;//当前事件记录的事件序号
    uint16 uIdIndex = 0U;//EvtOI事件记录序号的字典索引
    uint8 IDbuff[RECORD_DATA_POS + MAX_EVT_RECORD_NUM*SINGLE_RECORD_LEN];
    const char *szRoot;
    char  filename[200] = {0};

    memset(IDbuff,0xFF,RECORD_DATA_POS + MAX_EVT_RECORD_NUM*SINGLE_RECORD_LEN);
    szRoot ="/mnt/data/";
    sprintf(filename, "%sE_DATA/%04X", szRoot, EvtOI);

    //读取事件序号
    if(read_pdata_xram(filename, (char *)&curRecNum, 2, 4) < 0)
    {
        return -1;
    }
    #ifdef EE_DEBUG_EN
    DPRINT("curRecNum =%ld,mtr_id = %d\n",curRecNum,mtr_id);
    #endif
    uIdIndex = GetEventRecNumIDByOI(EvtOI);
    if(WriteMtrXData(mtr_id,RT_DATA,uIdIndex,(char*)&curRecNum)<0)
    {
        return -1;
    }

*/    return 1;
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

/**********************************************************************
* @name      : SetEventNum
* @brief     ：终端事件记录条数记录
* @param[in] ：EVENT_COMM_INFO *eventCommInfo     事件基础信息
* @param[out]：
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-03-03
* @Update    :
**********************************************************************/
int SetEventNum(EVENT_COMM_INFO *eventCommInfo)
{
    int             ret     = -1;
    NOMAL_OAD_T     nomalOAD;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    //事件记录条数
    eventCommInfo->eventNum++;
    nomalOAD.oad.value = Uint16To32(eventCommInfo->eventOAD.nObjID, 0X0400);
    nomalOAD.classtag = CLASS_DATA_EVENT;
    ret = db_write_nomal(g_sDataCenterClient, &nomalOAD, (uint8 *)&eventCommInfo->eventNum, 2);
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_write_nomal oad[%08X]记录事件条数=%d失败返回%d\n", nomalOAD.oad.value, eventCommInfo->eventNum, ret);
    }
    return ret;
}


/**********************************************************************
* @name      : SetEventStat
* @brief     ：终端事件当前值记录
* @param[in] ：mtrnum_t          mtr_id            测量点号(包括终端本体，从0开始)
               EVENT_COMM_INFO *eventCommInfo     事件基础信息
               OOP_EVTSOURCE_T  eventSource       事件发生源
               uint8            bType             事件状态 0(只记录事件，无发生，恢复区分)，1发生，2恢复
* @param[out]：
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-03-03
* @Update    :
**********************************************************************/
int SetEventStat(mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, uint8 bType)
{
    int             ret     = -1;
    uint32          time    = 0;
    NOMAL_OAD_T     nomalOAD;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    eventCommInfo->eventState.source = eventSource;
    eventCommInfo->eventState.dwCount = eventCommInfo->eventNum;
    eventCommInfo->eventState.dbIdx = eventCommInfo->storagenum[mtr_id];

    //恢复事件
    if(bType == 2)
    {
        OopDateTimeSToLinuxTime(&eventCommInfo->endtime[mtr_id], (time_t *)&time);
        eventCommInfo->eventState.dwTimes += time - eventCommInfo->eventState.tmStart;
    }
    else
    {
        OopDateTimeSToLinuxTime(&eventCommInfo->starttime[mtr_id], (time_t *)&time);
        eventCommInfo->eventState.tmStart = time;
    }

    //事件当前值记录
    nomalOAD.oad.value = Uint16To32(eventCommInfo->eventOAD.nObjID, 0X0700);
    nomalOAD.classtag = CLASS_DATA_EVENT;
    ret = db_write_nomal(g_sDataCenterClient, &nomalOAD, (uint8 *)&eventCommInfo->eventState, sizeof(EVTSTATE));
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_write_nomal oad[%08X]记录事件当前值失败 返回%d\n", nomalOAD.oad.value, ret);
    }
    return ret;
}


/**********************************************************************
* @name      : SetEventTime
* @brief     ：终端事件时间状态记录
* @param[in] ：mtrnum_t          mtr_id            测量点号(包括终端本体，从0开始)
               EVENT_COMM_INFO *eventCommInfo     事件基础信息
               OOP_EVTSOURCE_T  eventSource       事件发生源
               OOP_EVTBUF_T     eventData         事件关联对象数据
               uint8            bType             事件状态 0(只记录事件，无发生，恢复区分)，1发生，2恢复
* @param[out]：
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2019-12-22
* @Update    :
**********************************************************************/
int SetEventTime(mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, uint8 bType)
{
    int             ret     = -1;
    //uint32          time    = 0;
    NOMAL_OAD_T     nomalOAD;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    eventCommInfo->eventTime.source = eventSource;
    eventCommInfo->eventTime.dbIdx = eventCommInfo->storagenum[mtr_id];

    //恢复事件
    if(bType == 2)
    {
        eventCommInfo->eventTime.tmEnd = eventCommInfo->endtime[mtr_id];
    }
    else
    {
        eventCommInfo->eventTime.tmStart = eventCommInfo->starttime[mtr_id];
    }

    //事件时间状态记录
    nomalOAD.oad.value = Uint16To32(eventCommInfo->eventOAD.nObjID, 0X0A00);
    nomalOAD.classtag = CLASS_DATA_EVENT;
    ret = db_write_nomal(g_sDataCenterClient, &nomalOAD, (uint8 *)&eventCommInfo->eventTime, sizeof(EVTTIME));
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_write_nomal oad[%08X]记录事件时间状态失败 返回%d\n", nomalOAD.oad.value, ret);
    }
    return ret;
}


/**********************************************************************
* @name      : WriteEventToDB
* @brief     ：终端事件写入数据库
* @param[in] ：mtrnum_t          mtr_id            测量点号(包括终端本体，从0开始)
               EVENT_COMM_INFO *eventCommInfo     事件基础信息
               OOP_EVTSOURCE_T  eventSource       事件发生源
               OOP_EVTBUF_T     eventData         事件关联对象数据
               uint8            bType             事件状态 0(只记录事件，无发生，恢复区分)，1发生，2恢复
* @param[out]：
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2019-12-22
* @Update    :
**********************************************************************/
int WriteEventToDB(mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, OOP_EVTBUF_T eventData, uint8 bType)
{
    time_t             timep        = 0;
    int                ret          = 0;
    //uint8              *bDatabuf    = NULL;
    //uint32             bufLen       = 0;
    OOP_EVENT_T        oopEvent;        //标准事件单元
    OOP_CLOCKFLYLOG_T  clockEvent;      //时钟超差事件单元
    OOP_POWERLOG_T     powerEvent;      //停上电事件单元
    OOP_SMLOG_T        smEvent;         //未知电能表事件单元
    OOP_PROGRAMLOG_T   programEvent;    //编程事件
    UPDATE_RECORD_T    updateRecordData;
    WRITE_RECORD_T     recInData;

    memset(&recInData,0,sizeof(WRITE_RECORD_T));
    memset(&updateRecordData,0,sizeof(UPDATE_RECORD_T));
    memset(&oopEvent,0,sizeof(OOP_EVENT_T));
    memset(&clockEvent,0,sizeof(OOP_CLOCKFLYLOG_T));
    memset(&powerEvent,0,sizeof(OOP_POWERLOG_T));
    memset(&smEvent,0,sizeof(OOP_SMLOG_T));
    memset(&programEvent,0,sizeof(OOP_PROGRAMLOG_T));

    oopEvent.source = eventSource;
    oopEvent.data = eventData;
    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep

    if(bType <= 1)//发生
    {
        //事件记录内容
        GetCurrentOopDateTimeS(eventCommInfo->starttime + mtr_id);
        oopEvent.nIndex = eventCommInfo->recordnum +1;
        oopEvent.tmStart = eventCommInfo->starttime[mtr_id];
        if(bType == 0)                       //bType=1事件发生时，不记录结束时间，等事件恢复时更新
            oopEvent.tmEnd = oopEvent.tmStart;
//            oopEvent.state

        recInData.recordOAD.classtag = CLASS_DATA_EVENT;
        recInData.recordOAD.road.oadCols.nNum = 1;
        recInData.recordOAD.road.oadCols.oad[0] = eventCommInfo->eventOAD;
//        recInData.MAC = MAC;
        recInData.colStartTm = timep;
        recInData.colEndTm = timep;
        recInData.colStoreTm = eventCommInfo->recordnum +1;
        recInData.inDatalen = sizeof(OOP_EVENT_T);
        memcpy(recInData.inDataBuf, &oopEvent, sizeof(OOP_EVENT_T));

        //停上电事件
        if (eventCommInfo->eventOAD.value == 0X31060200)
        {
            memcpy(&powerEvent, &oopEvent, sizeof(OOP_EVENT_T));

            if(event_3106.ishappen == 0xaa)
            {
                powerEvent.tmStart = event_3106.tmnStarttime;
                powerEvent.tmEnd = event_3106.tmnEndtime;
                EVENT_FMT_DEBUG("终端停电时间%u-%u-%u %u:%u:%u \n", event_3106.tmnStarttime.year, event_3106.tmnStarttime.month, event_3106.tmnStarttime.day, event_3106.tmnStarttime.hour, event_3106.tmnStarttime.minute, event_3106.tmnStarttime.second);
                EVENT_FMT_DEBUG("终端上电时间%u-%u-%u %u:%u:%u \n", event_3106.tmnEndtime.year, event_3106.tmnEndtime.month, event_3106.tmnEndtime.day, event_3106.tmnEndtime.hour, event_3106.tmnEndtime.minute, event_3106.tmnEndtime.second);
            }

            powerEvent.flag.nValue = event_3106.eventFlag;

            recInData.inDatalen = sizeof(OOP_POWERLOG_T);
            memcpy(recInData.inDataBuf, &powerEvent, sizeof(OOP_POWERLOG_T));
        }
        else
        {
            recInData.inDatalen = sizeof(OOP_EVENT_T);
            memcpy(recInData.inDataBuf, &oopEvent, sizeof(OOP_EVENT_T));
        }

        EVENT_FMT_DEBUG("WriteEventToDB write, infoNum=%u, OAD=%8X\n", mtr_id, eventCommInfo->eventOAD.value);

        ret = db_write_record(g_sDataCenterClient, &recInData, &eventCommInfo->storagenum[mtr_id]);
        if (ret != ERR_OK)
        {
            EVENT_FMT_DEBUG("failed, infoNum=%u, OAD=%8X\n", mtr_id, eventCommInfo->eventOAD.value);
            return ret;
        }

        //事件记录条数
        SetEventNum(eventCommInfo);
        //事件当前值记录
        SetEventStat(mtr_id, eventCommInfo, eventSource, bType);
        //事件时间状态记录
        SetEventTime(mtr_id, eventCommInfo, eventSource, bType);

        if(bType == 1) //电能表相关
        {
            //记录存储序号和事件开始时间
            write_pdata_xram("^Event", (char *)&eventCommInfo->storagenum[mtr_id], (eventCommInfo->EventStoragNumRecAddr+(mtr_id)*(4)), 4);
            write_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(mtr_id)*(4)), 4);
        }

        eventCommInfo->recordnum++;
    }
    else if(bType == 2)
    {
        //恢复事件记录内容
        read_pdata_xram("^Event", (char *)&eventCommInfo->storagenum[mtr_id], (eventCommInfo->EventStoragNumRecAddr+(mtr_id)*(4)), 4);
        read_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(mtr_id)*(4)), 4);

        LinuxTimeToOopDateTimeS(timep, &eventCommInfo->starttime[mtr_id]);
        GetCurrentOopDateTimeS(&eventCommInfo->endtime[mtr_id]);
        oopEvent.nIndex = eventCommInfo->recordnum - 1;         //事件发生时，已经+1
        oopEvent.tmStart = eventCommInfo->starttime[mtr_id];
        oopEvent.tmEnd = eventCommInfo->endtime[mtr_id];
    //            oopEvent.state

        updateRecordData.recordOAD.road.oadCols.nNum = 1;
        updateRecordData.recordOAD.road.oadCols.oad[0] = eventCommInfo->eventOAD;
//        updateRecordData.MAC = MAC;
        updateRecordData.filter.choice = 2;
        updateRecordData.filter.storeNo = eventCommInfo->storagenum[mtr_id];
        updateRecordData.inDatalen = sizeof(OOP_EVENT_T);
        memcpy(updateRecordData.inDataBuf, &oopEvent, sizeof(OOP_EVENT_T));

        //停上电事件
        if (eventCommInfo->eventOAD.value == 0X31060200)
        {
            memcpy(&powerEvent, &oopEvent, sizeof(OOP_EVENT_T));
            powerEvent.flag.nValue = event_3106.eventParam.cParam.flag;

            updateRecordData.inDatalen = sizeof(OOP_POWERLOG_T);
            memcpy(updateRecordData.inDataBuf, &powerEvent, sizeof(OOP_POWERLOG_T));
        }
        else
        {
            updateRecordData.inDatalen = sizeof(OOP_EVENT_T);
            memcpy(updateRecordData.inDataBuf, &oopEvent, sizeof(OOP_EVENT_T));
        }

        EVENT_FMT_DEBUG("WriteEventToDB update, infoNum=%u, OAD=%8X\n", mtr_id, eventCommInfo->eventOAD.value);

        ret = db_update_record(g_sDataCenterClient, &updateRecordData);
        if (ret != ERR_OK)
        {
            EVENT_FMT_DEBUG("failed, infoNum=%u, OAD=%8X\n", mtr_id, eventCommInfo->eventOAD.value);
            return ret;
        }
    }

    return ERR_OK;
}

/*******************************************************************************
所属功能单元：事件预处理函数
功能描述：调用相对应的功能处理函数预先生成告警所需的数据(读取测量点基本参数F25)
输入参数：
  mtr_id  测量点号
输出参数：无
函数返回值：无
内部处理流程：读取当前计量点的数据及其参数填写到该计量点相对应的全局信息结构体,填
    充电压电流到内存,为之后的告警程序提供数据基。(接线方式,三相电压,电流,当前正向
    有功电能,上一次编程时间电池工作状态,额定电流,电流过负荷的相对额定值比例,额定电压)
********************************************************************************/
void FillEventGlobal(mtrnum_t mtr_id, OOP_METER_T *oopMeter)
{
    uint16         cur_rate = 0;    //-电流过负荷的相对额定值比例-

    //PROG_INFO_AND_EV_T prog_info;//-定义一个结构体，包括需量清零最大次数，编程次数等信息-
    //OOP_ENERGY_T  fw_act_en;     //正向有功电能

    //-当前抄表数据读取-
    event_global.vlt_a = APP_NONEWORD;
    event_global.vlt_b = APP_NONEWORD;
    event_global.vlt_c = APP_NONEWORD;
    event_global.cur_a = APP_NONELWORD;
    event_global.cur_b = APP_NONELWORD;
    event_global.cur_c = APP_NONELWORD;
    //event_global.fw_act_en = APP_NONELWORD;
    //memset(&prog_info,APP_NONECHAR,sizeof(PROG_INFO_AND_EV_T));
    //-4byte，最近一次编程时间-
    //memset(&event_global.prog_time,APP_NONECHAR,4);
    event_global.power_type = oopMeter->basic.pwrType;

#if 0
    if(event_3000.eventCommInfo.checkflag == 1 || event_3001.eventCommInfo.checkflag == 1 ||
       event_3002.eventCommInfo.checkflag == 1 || event_3003.eventCommInfo.checkflag == 1 ||
       event_3005.eventCommInfo.checkflag == 1 || event_301D.eventCommInfo.checkflag == 1 ||
       event_301E.eventCommInfo.checkflag == 1 || event_3119.eventCommInfo.checkflag == 1)
    {
        READ_RECORD_T startReadRecord;
        RECORD_UNIT_T stepRecordOut;
        time_t timep = {0};
        int  ret = 0;
        OOP_DWORD3V_T curTemp;       //电流，
        OOP_DWORD3V_T valTemp;       //电压

        time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep

        //获取 电流
        startReadRecord.recordOAD.road.oadMain.value = 0;
        startReadRecord.recordOAD.road.oadCols.nNum = 1;
        startReadRecord.recordOAD.logicId = 0;
        startReadRecord.recordOAD.infoNum = mtr_id;
        startReadRecord.cType = 0x00;
        startReadRecord.cStart = 0;
        startReadRecord.cEnd = timep;
        startReadRecord.sortType = 1;
        startReadRecord.MAC.nNum = 6;
        memcpy(startReadRecord.MAC.value, oopMeter->basic.tsa.add, 6);

        if (mtr_id != 0)
        {
            startReadRecord.recordOAD.road.oadCols.oad[0].value = 0x20010200;
            ret = db_read_record(g_sDataCenterClient, &startReadRecord, sizeof(OOP_DWORD3V_T), &stepRecordOut);
            if (ret == ERR_OK)
            {
                memcpy(&curTemp, stepRecordOut.record[0].outDataBuf, sizeof(OOP_DWORD3V_T));
                EVENT_FMT_TRACE("curTemp.nNum=[%u] \n", curTemp.nNum);
                if(curTemp.nNum >= 3)
                {
                    event_global.cur_c = curTemp.nValue[2];
                    EVENT_FMT_TRACE("Read Electric OK!event_global.cur_c=[%u] \n", event_global.cur_c);
                }

                if(curTemp.nNum >= 2)
                {
                    event_global.cur_b = curTemp.nValue[1];
                    EVENT_FMT_TRACE("Read Electric OK!event_global.cur_b=[%u] \n", event_global.cur_b);
                }

                if(curTemp.nNum >= 1)
                {
                    event_global.cur_a = curTemp.nValue[0];
                    EVENT_FMT_TRACE("Read Electric OK!event_global.cur_a=[%u] \n", event_global.cur_a);
                }
            }
            else
                EVENT_FMT_DEBUG("Read Electric current error!infonum=%u OAD=[0x20010200]ret=%d\n", mtr_id, ret);

            //获取 电压
            startReadRecord.recordOAD.road.oadCols.oad[0].value = 0x20000200;
            ret = db_read_record(g_sDataCenterClient, &startReadRecord, sizeof(OOP_DWORD3V_T), &stepRecordOut);
            if (ret == ERR_OK)
            {
                memcpy(&valTemp, stepRecordOut.record[0].outDataBuf, sizeof(OOP_DWORD3V_T));
                EVENT_FMT_TRACE("valTemp.nNum=[%u] \n", valTemp.nNum);
                if(valTemp.nNum >= 3)
                {
                    event_global.vlt_c = valTemp.nValue[2];
                    EVENT_FMT_TRACE("Read Voltage OK!event_global.vlt_c=[%u] \n", event_global.vlt_c);
                }

                if(valTemp.nNum >= 2)
                {
                    event_global.vlt_b = valTemp.nValue[1];
                    EVENT_FMT_TRACE("Read Voltage OK!event_global.vlt_b=[%u] \n", event_global.vlt_b);
                }

                if(valTemp.nNum >= 1)
                {
                    event_global.vlt_a = valTemp.nValue[0];
                    EVENT_FMT_TRACE("Read Voltage OK!event_global.vlt_a=[%u] \n", event_global.vlt_a);
                }
            }
            else
                EVENT_FMT_DEBUG("Read Voltage error!infonum=%u OAD=[0x20000200] ret=%d \n", mtr_id, ret);
        }
    }
#else
    if(event_3000.eventCommInfo.checkflag == 1 || event_3001.eventCommInfo.checkflag == 1 ||
       event_3002.eventCommInfo.checkflag == 1 || event_3003.eventCommInfo.checkflag == 1 ||
       event_3005.eventCommInfo.checkflag == 1 || event_300F.eventCommInfo.checkflag == 1 ||
       event_301D.eventCommInfo.checkflag == 1 || event_301E.eventCommInfo.checkflag == 1 ||
       event_3119.eventCommInfo.checkflag == 1)
    {
        if (g_LatestMeterReadData.valTemp[mtr_id].nNum >= 1)
            event_global.vlt_a = g_LatestMeterReadData.valTemp[mtr_id].nValue[0];
        if (g_LatestMeterReadData.valTemp[mtr_id].nNum >= 2)
            event_global.vlt_b = g_LatestMeterReadData.valTemp[mtr_id].nValue[1];
        if (g_LatestMeterReadData.valTemp[mtr_id].nNum >= 3)
            event_global.vlt_c = g_LatestMeterReadData.valTemp[mtr_id].nValue[2];
        if (g_LatestMeterReadData.curTemp[mtr_id].nNum >= 1)
            event_global.cur_a = g_LatestMeterReadData.curTemp[mtr_id].nValue[0];
        if (g_LatestMeterReadData.curTemp[mtr_id].nNum >= 2)
            event_global.cur_b = g_LatestMeterReadData.curTemp[mtr_id].nValue[1];
        if (g_LatestMeterReadData.curTemp[mtr_id].nNum >= 3)
            event_global.cur_c = g_LatestMeterReadData.curTemp[mtr_id].nValue[2];
    }
#endif

#if 0
    //获取 正向有功电能-
    if(event_3119.eventCommInfo.checkflag == 1)
    {
        startReadRecord.recordOAD.road.oadCols.oad[0].value = 0x00100200;
        ret = db_read_record(g_sDataCenterClient, &startReadRecord, sizeof(OOP_ENERGY_T), &stepRecordOut);
        if (ret == ERR_OK)
        {
            memcpy(&fw_act_en, stepRecordOut.record[0].outDataBuf, sizeof(OOP_ENERGY_T));
            event_global.fw_act_en = fw_act_en.nValue[0];
            EVENT_FMT_TRACE("Read Electric energy OK! event_global.fw_act_en=%u \n", event_global.fw_act_en);
        }
        else
            EVENT_FMT_DEBUG("Read Electric energy error!infonum=%u OAD=[0x00100200] ret=%d\n", mtr_id, ret);
    }
#endif

    //额定电压
    event_global.vlt_gate = oopMeter->basic.stVolt;
    EVENT_FMT_TRACE("Read event_global.vlt_gate=%u \n", event_global.vlt_gate);

    //额定电流
    event_global.cur_gate = (uint32)oopMeter->basic.stAmp*100;  //乘100和3位小数的电流比较
    EVENT_FMT_TRACE("Read event_global.cur_gate=%u \n", event_global.cur_gate);
/*
    //电流不平衡率
    startReadRecord.recordOAD.road.oadMain.value = 0x20270200;
    ret = db_read_record(g_sDataCenterClient, &startReadRecord, sizeof(uint16), &stepRecordOut);
    if (ret != ERR_OK)
    {
        //-电流过负荷的相对额定值比例-
        cur_rate = 120;    //-默认为120-
    }
    else
    {
        memcpy(&cur_rate, stepRecordOut.record[0].outDataBuf, sizeof(uint16));
        if(cur_rate == 0||cur_rate == 0xffff)
        {
            cur_rate = 120;
        }
    }
*/
    EVENT_FMT_DEBUG("测量点%u事件检测,接线类型%u,3相电压vlt[0]=%d,vlt[1]=%d,vlt[2]=%d\n",mtr_id,event_global.power_type,event_global.vlt_a,event_global.vlt_b,event_global.vlt_c);
    EVENT_FMT_DEBUG("3相电流cur[0]=%ld,cur[1]=%ld,cur[2]=%ld\n",event_global.cur_a,event_global.cur_b,event_global.cur_c);

    //-电流过负荷的相对额定值比例-
    cur_rate = 120; //-默认为120-
    event_global.cur_lmt = event_global.cur_gate*cur_rate/100;//-电流过负荷门限-
    EVENT_FMT_TRACE("Read event_global.cur_lmt=%u \n", event_global.cur_lmt);

    event_global.cb_interval = CB_INTERVAL;//-这里重庆要改为1分钟-
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
void CurReverseEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType)
{
    uint8  i;
    uint16 vlt_lmt = 0;                         //电压断相阀值
    uint16 ccAngle[3],vvAngle[3];               //电流、电压相角
    uint32 cur_para=0;                          //电流阀值
    uint32 dwERFlgTmp = 0;
    uint16 angle[3];                            //-电压电流夹角-
    int16  bLen;
    uint8  bCurReverseStat=0,bLastCurReverseStat=0;
    uint8  A_polarity_last = 0,B_polarity_last = 0,C_polarity_last = 0;
    uint8  bEventFlag = 0;
    float32  tmpFloat;
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    //int ret = 0;

    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

#if DELIVER_LOG_EN == 1
    static uint8 writeTimes = 0;
#endif

#if HEBEI_FUN == 1//现场默认持续时间为2分钟
    static  uint8 phaselastTimes[3] = {0};    //表示持续时间
    uint16  lastTimesLmt = 0;                 //持续时间限值
    char    paralimit;
    MtrProperty mtrPro;
    //集中器出现A相相序异常误报现象。
    //沟通：根据问题描述，电流由超前电压30°变化为滞后电压50°，首先我们公司上报相序
    //异常是不对的，要报也应该是电流反极性（功率为负），但是现场人员认为这个变化是瞬间的，因此不应该上报任何事件。
    //以上沟通是与先行公司肖学勋确认的内容，软件如何更改最好与衡水市供电公司运维刘班长（13091192670）联系，要与先行研发人员的身份联系。

    //软件修改为连续判断2分钟才算超时
    if (GetXParaInfo(mtr_id, &mtrPro) == 0 &&  mtrPro.category == MTR_AC)
    {
        if(read_pdata_xram(PARA_ERC11_FILE, &paralimit, 0, 1) == 0 && paralimit == 0x55)
            lastTimesLmt = 12;
    }
#endif

    cur_para = event_global.cur_gate*2/100;    //-默认为最大电流的2%-
    EVENT_FMT_TRACE("电流反极性阈值 cur_para=%lu \n",cur_para);

    //-断相比例默认为75%-
    vlt_lmt = event_global.vlt_gate*75/100;
    if (mtr_id == 0 && para_query_float32(0x8028, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt = tmpFloat*10;
    }

    if (event_global.power_type == 2)            //-三相四线-
    {
        if((Int_ABS(event_global.cur_a)<=(cur_para))||(Int_ABS(event_global.cur_b)<=(cur_para))||(Int_ABS(event_global.cur_c)<=(cur_para)))
        {
            //-判别相电流当前值≤电流反极性之阀值,不判-
            EVENT_FMT_DEBUG("判别相电流当前值≤电流反极性之阀值,不判\n");
            //write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);
            return;
        }

        if((event_global.vlt_a<=vlt_lmt)||(event_global.vlt_b<=vlt_lmt)||(event_global.vlt_c<=vlt_lmt))
        {
            //-断缺相不判-
            EVENT_FMT_DEBUG("缺相不判\n");
            //write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);

            return;
        }
    }

    EVENT_FMT_TRACE("接线方式 = %d (2:3/3; 3:3/4)\n",event_global.power_type);

    if (event_global.power_type == 1)            //-三相三线-
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
    vvAngle[1] = event_3119.oopWord4V[mtr_id].nValue[1];                            //-b相电压相角-
    vvAngle[2] = event_3119.oopWord4V[mtr_id].nValue[2];                            //-c相电压相角-
    ccAngle[0] = event_3119.oopWord4C[mtr_id].nValue[0];                            //-a相电流相角-
    ccAngle[1] = event_3119.oopWord4C[mtr_id].nValue[1];                            //-b相电流相角-
    ccAngle[2] = event_3119.oopWord4C[mtr_id].nValue[2];                            //-c相电流相角-

    if ((ccAngle[0] == APP_NONELWORD) && (ccAngle[1] == APP_NONELWORD) && (ccAngle[2] == APP_NONELWORD))
    {
        EVENT_FMT_DEBUG( "电流相角无效直接返回\n");

        return;
    }

    EVENT_FMT_TRACE("a相电压相角=%d,b相电压相角=%d,c相电压相角=%d,\n", vvAngle[0],vvAngle[1],vvAngle[2]);
    EVENT_FMT_TRACE("a相电流相角=%d,b相电流相角=%d,c相电流相角=%d,\n", ccAngle[0],ccAngle[1],ccAngle[2]);

    if (read_pdata_xram("^Event",(char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4)>=0)
    {
        //-读取事件状态标志-
        if(((vvAngle[1]+vvAngle[2])>=60) && (vvAngle[1]>vvAngle[2]) && (event_global.power_type == 2))
        {
            return;
        }
        if(((vvAngle[1]+vvAngle[2])>=60) && (vvAngle[1]>vvAngle[2]) && (event_global.power_type == 1))
        {
            return;
        }
    }

    if (dwERFlgTmp == 0xffffffff) //文件系统初始为0xff值
    {
        dwERFlgTmp = 0;
    }

    EVENT_FMT_DEBUG("读取事件状态1 %08X\n", dwERFlgTmp);

    if((ccAngle[0] == 0) && (ccAngle[1] == 0) && (ccAngle[2] == 0))
    {
        return;
    }

    memset(vc_angle, 0x0, sizeof(vc_angle));
    for(i=0; i<3; i++)
    {
        if(vvAngle[i] <= ccAngle[i])
            angle[i] = ccAngle[i] - vvAngle[i];
        else
            angle[i] =  360 + ccAngle[i] - vvAngle[i];
        vc_angle[i] = angle[i];
    }

    if (event_global.power_type == 1)
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
        if( event_global.power_type == 2)    //-三相四线-
        {
            if((vc_angle[i]>90) && (vc_angle[i]<270))
                bCurReverseStat |= (0x01<<(i*2));
            else
                bCurReverseStat |= (0x02<<(i*2));
        }

        if( event_global.power_type == 1)    //-三相三线-
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

#if 0
    if(bfirst[mtr_id]==0)
    {
        bfirst[mtr_id]=1;
        write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);
#if DEBUG_EN_9 == 1
        DPRINT("首次检测，写入原始值并返回\n");
#endif
        return;
    }
#endif

    bLen =  read_pdata_xram("^Event", (char *)&bLastCurReverseStat, (CurReverseRecAddr+mtr_id), 1);
    if (bLen < 0)
    {
        EVENT_FMT_DEBUG("读取事件状态失败");
        return;
    }

    if(bLastCurReverseStat == 0xff)
    {
        write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);
        EVENT_FMT_DEBUG("事件发生标志为非法，重新写入\n");

        return;
    }

    EVENT_FMT_TRACE("事件发生标志 bLastCurReverseStat = %02X        bCurReverseStat = %02X\n",bLastCurReverseStat,bCurReverseStat);

    A_polarity_last = bLastCurReverseStat&0x03 ;
    B_polarity_last = (bLastCurReverseStat&0x0c)>>2 ;
    C_polarity_last = (bLastCurReverseStat&0x30)>>4 ;

#if HEBEI_FUN == 1
    if(bType==2)//-电流反极性恢复检测-
    {
        if((bCurReverseStat&0x03)==2)
        {
            if(A_polarity_last==1)//上次发生
            {
                EVENT_FMT_DEBUG("A 相恢复 :phaselastTimes[0] = %d  %d\n",phaselastTimes[0],lastTimesLmt);
                if(phaselastTimes[0]-- <= lastTimesLmt)
                {
                    bEventFlag |= 0x01; //-a相反极性恢复-
                    phaselastTimes[0] = 0;
                }
            }
            else
            {
                if(phaselastTimes[0] != 0)
                    phaselastTimes[0] = 0;
            }
        }

        if(((bCurReverseStat&0x0c)>>2)==2)
        {
            if(B_polarity_last==1)//上次发生
            {
                EVENT_FMT_DEBUG("B 相恢复 :phaselastTimes[1] = %d  %d\n",phaselastTimes[1],lastTimesLmt);
                if(phaselastTimes[1]-- <= lastTimesLmt)
                {
                    bEventFlag |= 0x02; //-a相反极性恢复-
                    phaselastTimes[1] = 0;
                }
            }
            else
            {
                if(phaselastTimes[1] != 0)
                    phaselastTimes[1] = 0;
            }
        }

        if(((bCurReverseStat&0x30)>>4)==2)
        {
            if(C_polarity_last==1)//上次发生
            {
                EVENT_FMT_DEBUG("C 相恢复 :phaselastTimes[2] = %d  %d\n",phaselastTimes[2],lastTimesLmt);
                if(phaselastTimes[2]-- <= lastTimesLmt)
                {
                    bEventFlag |= 0x04; //-a相反极性恢复-
                    phaselastTimes[2] = 0;
                }
            }
            else
            {
                if(phaselastTimes[2] != 0)
                    phaselastTimes[2] = 0;
            }
        }

    }
    else if(bType==1)//-电流反极性发生检测-
    {
        if((bCurReverseStat&0x03)==1)//发生
        {
            if(A_polarity_last!=1)//尚未发生
            {
                EVENT_FMT_DEBUG("A 相发生 :phaselastTimes[0] = %d  %d\n",phaselastTimes[0],lastTimesLmt);
                if(phaselastTimes[0]++ >= lastTimesLmt)
                {
                    bEventFlag |= 0x01; //-a相反极性发生-
                    phaselastTimes[0] = lastTimesLmt*2;
                }
            }
            else
            {
                if(phaselastTimes[0] != lastTimesLmt * 2)
                    phaselastTimes[0] = lastTimesLmt*2;
            }
        }

        if(((bCurReverseStat&0x0c)>>2)==1)//发生
        {
            if(B_polarity_last!=1)//尚未发生
            {
                EVENT_FMT_DEBUG("B 相发生 :phaselastTimes[1] = %d  %d\n",phaselastTimes[1],lastTimesLmt);
                if(phaselastTimes[1]++ >= lastTimesLmt)
                {
                    bEventFlag |= 0x02; //-a相反极性发生-
                    phaselastTimes[1] = lastTimesLmt*2;
                }
            }
            else
            {
                if(phaselastTimes[1] != lastTimesLmt * 2)
                    phaselastTimes[1] = lastTimesLmt*2;
            }
        }


        if(((bCurReverseStat&0x30)>>4)==1)//发生
        {
            if(C_polarity_last!=1)//尚未发生
            {
                EVENT_FMT_DEBUG("C 相发生 :phaselastTimes[2] = %d  %d\n",phaselastTimes[2],lastTimesLmt);
                if(phaselastTimes[2]++ >= lastTimesLmt)
                {
                    bEventFlag |= 0x04; //-a相反极性发生-
                    phaselastTimes[2] = lastTimesLmt*2;
                }
            }
            else
            {
                if(phaselastTimes[2] != lastTimesLmt * 2)
                    phaselastTimes[2] = lastTimesLmt*2;
            }
        }

    }

#else
    if(bType==2)//-电流反极性恢复检测-
    {
        if(((bCurReverseStat&0x03)==2)&&(A_polarity_last==1))
            bEventFlag |= 0x01;    //-a相反极性恢复-
        if((((bCurReverseStat&0x0c)>>2)==2)&&(B_polarity_last==1))
            bEventFlag |= 0x02;    //-b相反极性恢复-
        if((((bCurReverseStat&0x30)>>4)==2)&&(C_polarity_last==1))
            bEventFlag |= 0x04;    //-c相反极性恢复-
    }
    else if(bType==1)//-电流反极性发生检测-
    {
        if(((bCurReverseStat&0x03)==1)&&(A_polarity_last!=1))
            bEventFlag |= 0x01;    //-a相反极性发生-
        if((((bCurReverseStat&0x0c)>>2)==1)&&(B_polarity_last!=1))
            bEventFlag |= 0x02;    //-b相反极性发生-
        if((((bCurReverseStat&0x30)>>4)==1)&&(C_polarity_last!=1))
            bEventFlag |= 0x04;    //-c相反极性发生-
    }
#endif

    if(bEventFlag)
    {
#if HEBEI_FUN == 1
        if(bEventFlag & 0x01)
        {
            EVENT_FMT_TRACE("A相CheckERC_3119事件\n");
            bLastCurReverseStat= (bLastCurReverseStat&0xfc)|(bCurReverseStat & 0x03);
        }
        if (bEventFlag & 0x02)
        {
            EVENT_FMT_TRACE("B相CheckERC_3119事件\n");
            bLastCurReverseStat= (bLastCurReverseStat&0xf3)|(bCurReverseStat & 0x0c);
        }
        if(bEventFlag & 0x04)
        {
            EVENT_FMT_TRACE("C相CheckERC_3119事件\n");
            bLastCurReverseStat= (bLastCurReverseStat&0xcf)|(bCurReverseStat & 0x30);
        }
        bCurReverseStat = bLastCurReverseStat;
#endif
        if(bType==1)
        {
            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                SaveFileRecBfg(ERC_NO_09,(bEventFlag&0x07)|0x80,mtr_id);//记录电流反极性(潮流反向)发生
            }
        }
        else if(bType==2)
        {
            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                SaveFileRecBfg(ERC_NO_09,(bEventFlag&0x07),mtr_id);//记录电流反极性(潮流反向)恢复
            }
        }

        write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id),1);

        //TODO 事件写入数据库
        //事件发生源
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter->basic.tsa;

        //事件关联数据
        eventData.oadNum = 2;
        eventData.cols.nNum = 2;
        eventData.cols.oad[0].value = 0X20034200;
        eventData.cols.oad[1].value = 0X00104201;
        eventData.idLen[0] = sizeof(OOP_WORD4V_T);
        eventData.idLen[1] = sizeof(OOP_ENERGY_T);
        eventData.len = eventData.idLen[0] + eventData.idLen[1];
        memcpy(eventData.buf, &event_3119.oopWord4C[mtr_id], eventData.idLen[0]);
        memcpy(eventData.buf+eventData.idLen[0], &event_3119.oopEnergys[mtr_id], eventData.idLen[1]);

        if(bType==1)//发生
        {
            //写数据库
            int ret = WriteEventToDB(mtr_id, &event_3119.eventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("测量点mtr_id=%hu  电流反极性事件记录CheckERC_3119失败返回%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_TRACE("测量点mtr_id=%hu  电流反极性事件记录CheckERC_3119成功 storagenum=%u \n", mtr_id, event_3119.eventCommInfo.storagenum[mtr_id]);

        }
        else if(bType==2)
        {
            //恢复事件记录内容
            int ret = WriteEventToDB(mtr_id, &event_3119.eventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("测量点mtr_id=%hu  电流反极性事件恢复记录CheckERC_3119失败返回%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_TRACE("测量点mtr_id=%hu  电流反极性事件恢复记录CheckERC_3119成功 , storagenum=%u !\n", mtr_id, event_3119.eventCommInfo.storagenum[mtr_id]);
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
void VltBreakEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter,uint8 bType)
{
    int    ret = 0;
    uint16 vlt[3] = {0};                        //-当前３相电压-
    uint32 cur[3] = {0};                        //-当前３相电流-
    uint32 vlt_lmt = 0;                         //-电压判定值-
    uint16 lastTimesLmt = 0;                    //持续时间限值
    uint16 vltgate = 0;                         //额定电压    这里如果没有读到额定电压就直接返回，所以没有使用event_global.vlt_gate
    uint32 vlt_resume = 0;                      //电压断相、缺相恢复比例
    static uint16 lastTimes[4][3][MTR485_MAX_NUM] = {{{0}}};    //事件发生/恢复持续时间
    uint32 vlt_lmt1 = 0;                        //电压下下限值(失压限值)
    float32  tmpFloat   = 0;

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
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据

    //-读取实时电压、电流-
    vlt[0] = event_global.vlt_a;                //-取a相电压-
    vlt[1] = event_global.vlt_b;                //-取b相电压-
    vlt[2] = event_global.vlt_c;                //-取c相电压-
    cur[0] = Int_ABS(event_global.cur_a);       //-取a相电流-
    cur[1] = Int_ABS(event_global.cur_b);       //-取b相电流-
    cur[2] = Int_ABS(event_global.cur_c);       //-取c相电流-

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

    //-读取电压断相比例-
    vlt_lmt = event_global.vlt_gate*78/100;  /*- 断相比例默认为78%-*/
    if (mtr_id == 0 && para_query_float32(0x8028, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt = tmpFloat*10;
    }

    EVENT_FMT_DEBUG("电压断相门限vlt_lmt=%lu\n",vlt_lmt);
    //-读取电压下下限值,作为失压 门限-
    vlt_lmt1 = event_global.vlt_gate*78/100;  /*- 断相比例默认为78%-*/
    EVENT_FMT_DEBUG("电压失压门限vlt_lmt1=%lu\n",vlt_lmt1);

    //-额定电压-
    if(event_global.power_type == 2)            //-三相四线-
        vltgate = 2200;
    else if(event_global.power_type == 1)       //-三相三线-
        vltgate = 1000;
    else
        vltgate = 2200;

    EVENT_FMT_DEBUG("没有读取到当前测量点的额定电压，取默认值    vltgate = %d\n",vltgate);

    vlt_resume = vltgate * 85/100;              //-失压和断相恢复比例默认为85%-
    EVENT_FMT_DEBUG("失压和断相恢复比例默认vlt_resume = %lu\n",vlt_resume);

    //-失压持续时间限值-
    lastTimesLmt = 1;
    if((lastTimesLmt == 0) || (lastTimesLmt == 0xffff))
    {
        lastTimesLmt = 1;
    }

    EVENT_FMT_DEBUG("失压断相持续时间限值lastTimesLmt=%d\n",lastTimesLmt);

    cur_lmt = event_global.cur_gate * 5 /1000;  //-求得断缺相电流限值-
    EVENT_FMT_DEBUG("额定电流的值event_global.cur_gate = %lu    启动电流的值cur_lmt = %lu\n",event_global.cur_gate,cur_lmt);

    //-循环检测三相-
    for(order = 0; order < 3; order++)
    {
        if((cur[order] == APP_NONELWORD)||(vlt[order] == APP_NONEWORD)
                ||(cur[order] == NONE_LWORD)||(vlt[order] == NONE_WORD))    //-电流电压为无效-
        {
            continue;
        }

        if(event_global.power_type == 1 && order == 1)    //三相三线B相不判断
        {
            continue;
        }

        tmp_flag = 0;
        if(bType == 1)              //-电压断相-
        {
            tmp_flag = (cur[order] <= cur_lmt) && (vlt[order] <= vlt_lmt);
        }
        else if(bType == 2)         //-电压断相恢复-
        {
            tmp_flag = !((cur[order] <= cur_lmt) && (vlt[order] <= vlt_lmt));
        }
        else if(bType == 3)         //-失压发生检测条件-
        {
#if 0//PHASE_LOW_EVENT_TYPE0 == 1
            tmp_flag = (cur[order] > cur_lmt) && ((vlt[order] <= vlt_lmt1) && (vlt[order] > vlt_lmt));
#else
            tmp_flag = (cur[order] > cur_lmt) && (vlt[order] <= vlt_lmt1);
#endif
        }
        else if(bType == 4)         //-失压恢复条件-
        {
#if 0//PHASE_LOW_EVENT_TYPE0 == 1
            tmp_flag = !((cur[order] > cur_lmt) && ((vlt[order] <= vlt_lmt1) && (vlt[order] > vlt_lmt)));
#else
            tmp_flag = !((cur[order] > cur_lmt) && (vlt[order] <= vlt_lmt1));
#endif
        }
        else
        {
            return;
        }

        if(tmp_flag)                //如果满足相应的发生条件，将对应的时间进行累加
        {
            if(lastTimes[bType-1][order][mtr_id] >= lastTimesLmt-1)
            {
                is_now_flag = 1;
            }
            else
            {
                lastTimes[bType-1][order][mtr_id]++;
                is_now_flag = 0;
            }
            EVENT_FMT_DEBUG("第%d相满足发生条件，开始统计时间，目前时间lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
        }
        else
        {
            EVENT_FMT_DEBUG("第%d相不满足发生条件，时间清零\n",order+1);
            lastTimes[bType-1][order][mtr_id] = 0;
            is_now_flag = 0;
        }

        if((bType == 1)||(bType ==2))    //-断相及恢复检测-
            len =  read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+3), 1);
        else    //-失压及恢复检测-
            len =  read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+4), 1);

        if (bData == 0xff) //文件系统初始为0xff值
        {
            bData = 0;
        }

        if (len < 0)
        {
            EVENT_FMT_DEBUG("读取事件状态失败");
            return;
        }

        EVENT_FMT_DEBUG("读取事件状态%02X\n", bData);

        if((bType == 1)||(bType ==3))    //-事件发生检测-
        {
            EVENT_FMT_DEBUG("事件发生检测\n");
            if(is_now_flag)
            {
                EVENT_FMT_DEBUG("事件发生检测满足发生条件\n");
                switch(order)
                {
                case 0:
                    if(!(bData&A_PHASE))
                    {
                        bData |= A_PHASE;    //-A相-
                        bphase |= A_PHASE;    //-A相-
                        if(bType == 1)
                        {
                            ERFlg = BREAK_PHASE;//-断相标记-
                            EVENT_FMT_DEBUG("置A相断相标记\n");
                        }
                        else
                        {
                            ERFlg = LOSE_VOLT;    //-失压标记-
                            EVENT_FMT_DEBUG("置A相失压标记\n");
                        }
                    }
                    else
                    {
                        EVENT_FMT_DEBUG("A相已经记录过\n");
                    }
                    break;
                case 1:
                    if(!(bData&B_PHASE))
                    {
                        bData |= B_PHASE;
                        bphase |= B_PHASE;
                        if(bType == 1)
                        {
                            ERFlg = BREAK_PHASE;//-断相标记-
                            EVENT_FMT_DEBUG("置B相断相标记\n");
                        }
                        else
                        {
                            ERFlg = LOSE_VOLT ;    //-失压标记-
                            EVENT_FMT_DEBUG("置B相失压标记\n");
                        }
                    }
                    else
                    {
                        EVENT_FMT_DEBUG("B相已经记录过\n");
                    }
                    break;
                case 2:
                    if(!(bData&C_PHASE))
                    {
                        bData |= C_PHASE;
                        bphase |= C_PHASE;
                        if(bType == 1)
                        {
                            ERFlg = BREAK_PHASE;//-断相标记-
                            EVENT_FMT_DEBUG("置C相断相标记\n");
                        }
                        else
                        {
                            ERFlg = LOSE_VOLT ;    //-失压标记-
                            EVENT_FMT_DEBUG("置C相失压标记\n");
                        }
                    }
                    else
                    {
                        EVENT_FMT_DEBUG("C相已经记录过\n");
                    }
                    break;
                default:
                    break;
                }
                if(bType == 1)
                    write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+3), 1);
                else
                    write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+4), 1);
            }
        }
        else//-事件恢复检测-
        {
            if((is_now_flag)&&( bData &(1<<order)))
            {
                EVENT_FMT_DEBUG("事件恢复检测\n");
                switch(order)
                {
                case 0:
                    bData &= ~A_PHASE;    //-清除A相标记-
                    bphase |= A_PHASE;
                    if(bType == 2)
                    {
                        ERFlg = BREAK_PHASE;//-断相标记-
                        EVENT_FMT_DEBUG("置A相断相恢复标记\n");
                    }
                    else
                    {
                        ERFlg = LOSE_VOLT;    //-失压标记-
                        EVENT_FMT_DEBUG("置A相失压恢复标记\n");
                    }
                    break;

                case 1:
                    bData &= ~B_PHASE;
                    bphase |= B_PHASE;
                    if(bType == 2)
                    {
                        ERFlg = BREAK_PHASE;//-断相标记-
                        EVENT_FMT_DEBUG("置B相断相恢复标记\n");
                    }
                    else
                    {
                        ERFlg = LOSE_VOLT;    //-失压标记-
                        EVENT_FMT_DEBUG("置B相失压恢复标记\n");
                    }
                    break;

                case 2:
                    bData &= ~C_PHASE;
                    bphase |= C_PHASE;
                    if(bType == 2)
                    {
                        ERFlg = BREAK_PHASE;//-断相标记-
                        EVENT_FMT_DEBUG("置C相断相恢复标记\n");
                    }
                    else
                    {
                        ERFlg = LOSE_VOLT;    //-失压标记-
                        EVENT_FMT_DEBUG("置C相失压恢复标记\n");
                    }
                    break;

                default:
                    break;
                }
                if(bType == 2)
                    write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+3), 1);
                else
                    write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+4), 1);
            }
        }
    }

//#if DEBUG_EN_10 == 1
    if((bType == 1)&&(is_now_flag)&&(ERFlg == BREAK_PHASE))
    {
    	yx_calculate_sta_set(CALC_YX_PHASE_LOSS, 0, 1);
        //EVENT_FMT_DEBUG("电压断相发生\n");
    }
    if((bType == 3)&&(is_now_flag)&&(ERFlg == LOSE_VOLT))
    {
    	yx_calculate_sta_set(CALC_YX_VOLT_LOSS, 0, 1);
        //EVENT_FMT_DEBUG("电压失压发生\n");
    }
    if((bType == 2)&&(is_now_flag)&&(ERFlg == BREAK_PHASE))
    {
    	yx_calculate_sta_set(CALC_YX_PHASE_LOSS, 0, 0);
        //EVENT_FMT_DEBUG("电压断相恢复\n");
    }
    if((bType == 4)&&(is_now_flag)&&(ERFlg == LOSE_VOLT))
    {
    	yx_calculate_sta_set(CALC_YX_VOLT_LOSS, 0, 0);
        //EVENT_FMT_DEBUG("电压失压恢复\n");
    }
//#endif

    if(ERFlg)//-事件发生或恢复-
    {
        uint32  aTemp = 0;
        EVENT_COMM_INFO* pCommInfo = NULL;

        if((bType == 1)&&(ERFlg == BREAK_PHASE))
        {
            //-断相发生才报表故障事件-
            len = read_pdata_xram("^Event", (char *)&aTemp, (BreakphaseAddr+(mtr_id)*4), 4);
            if(len>=0)
            {
                aTemp++;//-记录表的新断相次数-
                write_pdata_xram("^Event", (char *)&aTemp, (BreakphaseAddr+(mtr_id)*4), 4);
            }
        }

        if((bType==3)&&(ERFlg == LOSE_VOLT))
        {
            if(read_pdata_xram("^Event", (char *)&dTemp, (LostvltAddr+(mtr_id)*2), 2)>=0)
                dTemp++;    //-失压事件发生,次数有变化-
            write_pdata_xram("^Event", (char *)&dTemp, (LostvltAddr+(mtr_id)*2), 2);
        }

        //事件发生源
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter->basic.tsa;

        //事件关联数据
        eventData.oadNum = 2;
        eventData.cols.nNum = 2;
        eventData.cols.oad[0].value = 0X20002200;
        eventData.cols.oad[1].value = 0X20012200;
        eventData.idLen[0] = sizeof(OOP_WORD3V_T);
        eventData.idLen[1] = sizeof(OOP_WORD3V_T);
        eventData.len = eventData.idLen[0] + eventData.idLen[1];

        if (bType < 3)
        {
            memcpy(eventData.buf, &event_3003.oopWord3V[mtr_id], eventData.idLen[0]);
            memcpy(eventData.buf+eventData.idLen[0], &event_3003.oopWord3C[mtr_id], eventData.idLen[1]);
            pCommInfo = &event_3003.eventCommInfo;
        }
        else {
            memcpy(eventData.buf, &event_3000.oopWord3V[mtr_id], eventData.idLen[0]);
            memcpy(eventData.buf+eventData.idLen[0], &event_3000.oopWord3C[mtr_id], eventData.idLen[1]);
            pCommInfo = &event_3000.eventCommInfo;
        }

        //通知104协议读取
        if((bType == 2)||(bType == 4))//-事件恢复-
        {
            len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);
            dwERFlgTmp &= ~ERFlg;//-保存恢复标记到此测量点全局事情标记中-
            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);

            //断相和失压使用同一个序号，在类型的最高位来区分，最高位为1表示断相，为0表示失压
            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                if(bType == 2)
                    SaveFileRecBfg(ERC_NO_10|0x80,(bphase&0x07),mtr_id);
                else if (bType == 4)
                    SaveFileRecBfg(ERC_NO_10,bphase&0x07,mtr_id);
            }

            //恢复事件记录内容
            ret = WriteEventToDB(mtr_id, pCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("测量点mtr_id=%hu  断相事件恢复记录CheckERC_3003失败返回%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_TRACE("测量点mtr_id=%hu  断相事件恢复记录CheckERC_3003成功 , storagenum=%u !\n", mtr_id, pCommInfo->storagenum[mtr_id]);
        }
        else//-事件发生-
        {
            len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);
            dwERFlgTmp |= ERFlg;//-保存发生标记到此测量点全局事情标记中-
            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);

            //断相和失压使用同一个序号，在相位的最高位来区分，最高位为1表示断相，为0表示失压
            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                if(bType == 1)
                    SaveFileRecBfg(ERC_NO_10|0x80,(bphase&0x07)|0x80,mtr_id);
                else if (bType == 3)
                    SaveFileRecBfg(ERC_NO_10,     (bphase&0x07)|0x80,mtr_id);
            }

            //写数据库
            ret = WriteEventToDB(mtr_id, pCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("测量点mtr_id=%hu  断相事件记录CheckERC_3003失败返回%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("测量点mtr_id=%hu  断相事件记录CheckERC_3003成功 storagenum=%u \n", mtr_id, pCommInfo->storagenum[mtr_id]);
        }
    }
}

/*****************************************************************************
所属功能单元：
功能描述：逆相序告警
输入参数：计量点号
输出参数：无
函数返回值：1－－逆相，0－－正常
内部处理流程：仅判内置交采逆相序事件，相角数据来源交采驱动Real_cal.c全局变量
       另注：按04事件规范，断缺相时不判逆相序
***************************************************************************-*/
void PhaseReverseEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter)
{
    uint8    lastTimes    = 0;      //-持续发生/恢复时间，0：电压逆相序，1：电流逆相序-
    uint8    lastTimesLmt = 1;      //-持续时间限值，默认1分钟-
    int16    len          = 0;
    uint8    bData        = 0;
    uint32   ERFlg        = 0;      //-电压电流逆相序 1：发生，2：恢复-
    uint8    bType        = 0;      //-0：电压逆相序发生/恢复，1：电流逆相序发生/恢复-
    uint8    happenFlg    = 0;
    uint8    recoverFlg   = 0;
    uint32   dwERFlgTmp   = 0;
    uint16   vlt_lmt      = 0;
    float32  tmpFloat     = 0;

    //-断相比例默认为75%-
    vlt_lmt = event_global.vlt_gate*75/100;
    if (mtr_id == 0 && para_query_float32(0x8028, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt = tmpFloat*10;
    }

    EVENT_FMT_DEBUG("电压断相门限=%d\n", vlt_lmt);

    //-逆相序判断-
    if (event_global.vlt_a == APP_NONEWORD || event_global.vlt_b == APP_NONEWORD || event_global.vlt_c == APP_NONEWORD)
    {
        EVENT_FMT_DEBUG("三相四线电压无效时不判逆相序\n");

        return;
    }

    if( (event_global.vlt_a<vlt_lmt) || (event_global.vlt_b<vlt_lmt)
       || (event_global.vlt_c<vlt_lmt))    //-断缺相时不判逆相序-
    {
        EVENT_FMT_DEBUG("三相四线断缺相时不判逆相序\n");

        return;
    }

    len = read_pdata_xram("^Event", (char *)&bData, (PhaseAbnormalAddr+(mtr_id)*4+3), 1);
    if (len < 0)//-未申请到内存或内存乱-
    {
        EVENT_FMT_DEBUG("读取事件状态失败");
        return;
    }

    if(bData == 0xFF)
        bData = 0;

    if((event_300F.meterRsw.metWord[6]&0x0001) == 1)
    {
        bType = 1;
    }
    else
    {
        bType = 0;
    }

    if (event_300F.nDelay > 0)
        lastTimesLmt = event_300F.nDelay;

    EVENT_FMT_DEBUG("读取事件之前状态%02X,当前状态:%02X,持续时间限值:%u\n", bData, bType, lastTimesLmt);

    if(bType)
    {
        if(!(bData & 1))//尚未发生
        {
            len = read_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
            if(len < 0 || (lastTimes == 0xff))
                lastTimes = 0;
            printf("发生 :lastTimes = %d %d\n",lastTimes,lastTimesLmt);
            if(lastTimes++ >= lastTimesLmt)
            {
                bData |= 1;
                ERFlg = 1;
                lastTimes = lastTimesLmt * 2;
                write_pdata_xram("^Event", (char *)&bData, (PhaseAbnormalAddr+(mtr_id)*4+3), 1);
            }

            write_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
        }
        else
        {
            len = read_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
            if(len < 0 || (lastTimes == 0xff))
                lastTimes = 0;
            if(lastTimes != lastTimesLmt * 2)
            {
                lastTimes = lastTimesLmt * 2;
                write_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
            }
        }
    }
    else
    {
        if(bData & 1)
        {
            len = read_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
            if(len < 0 || (lastTimes == 0xff))
                lastTimes = 2*lastTimesLmt;
            printf("恢复: lastTimes = %d %d\n",lastTimes,lastTimesLmt);
            if(lastTimes-- <= lastTimesLmt)
            {
                bData &= ~(1);
                ERFlg = 2;  //-恢复-
                lastTimes = 0;
                write_pdata_xram("^Event", (char *)&bData, (PhaseAbnormalAddr+(mtr_id)*4+3), 1);
            }
            write_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*3), 1);
        }
        else
        {
            len = read_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
            if(len < 0 || (lastTimes == 0xff))
                lastTimes = 0;
            if(lastTimes != 0)
            {
                lastTimes = 0;
                write_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
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

    if(happenFlg) //发生
    {
        len = read_pdata_xram("^Event",(char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*(4)), 4);
        dwERFlgTmp |= PHASE_REVER;    //-保存发生标记到此测量点全局事情标记中-
        write_pdata_xram("^Event",(char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);

        if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
        {
            SaveFileRecBfg(ERC_NO_11|0x80,0x80,mtr_id);//记录电压逆相序发生
        }
    }
    else if(recoverFlg) //恢复
    {
        len = read_pdata_xram("^Event",(char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);
        dwERFlgTmp &= ~PHASE_REVER;    //-保存发生标记到此测量点全局事情标记中-
        write_pdata_xram("^Event",(char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);

        if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
        {
            SaveFileRecBfg(ERC_NO_11|0x80,0,mtr_id);//记录电压逆相序恢复
        }
    }
    else
        return;

#if 0
    int ret = 0;
    OOP_EVTSOURCE_T eventSource;    //事件发生源
    OOP_EVTBUF_T    eventData;      //事件关联数据
    //事件发生源NULL
    eventSource.choice = DT_NULL;

    //事件关联数据
    eventData.oadNum = 32;
    eventData.cols.nNum = 32;
//    eventData.oadNum = 16;
//    eventData.cols.nNum = 16;
    eventData.cols.oad[0].value = 0X00102201;
    eventData.cols.oad[1].value = 0X00202201;
    eventData.cols.oad[2].value = 0X00302201;
    eventData.cols.oad[3].value = 0X00402201;
    eventData.idLen[0] = sizeof(uint32);
    eventData.idLen[1] = sizeof(uint32);
    eventData.idLen[2] = sizeof(int);
    eventData.idLen[3] = sizeof(int);
    eventData.cols.oad[4].value = 0X00112201;
    eventData.cols.oad[5].value = 0X00212201;
    eventData.cols.oad[6].value = 0X00312201;
    eventData.cols.oad[7].value = 0X00412201;
    eventData.idLen[4] = sizeof(uint32);
    eventData.idLen[5] = sizeof(uint32);
    eventData.idLen[6] = sizeof(int);
    eventData.idLen[7] = sizeof(int);
    eventData.cols.oad[8].value = 0X00122201;
    eventData.cols.oad[9].value = 0X00222201;
    eventData.cols.oad[10].value = 0X00322201;
    eventData.cols.oad[11].value = 0X00422201;
    eventData.idLen[8] = sizeof(uint32);
    eventData.idLen[9] = sizeof(uint32);
    eventData.idLen[10] = sizeof(int);
    eventData.idLen[11] = sizeof(int);
    eventData.cols.oad[12].value = 0X00132201;
    eventData.cols.oad[13].value = 0X00232201;
    eventData.cols.oad[14].value = 0X00332201;
    eventData.cols.oad[15].value = 0X00432201;
    eventData.idLen[12] = sizeof(uint32);
    eventData.idLen[13] = sizeof(uint32);
    eventData.idLen[14] = sizeof(int);
    eventData.idLen[15] = sizeof(int);
    eventData.cols.oad[16].value = 0X00108201;
    eventData.cols.oad[17].value = 0X00208201;
    eventData.cols.oad[18].value = 0X00308201;
    eventData.cols.oad[19].value = 0X00408201;
    eventData.idLen[16] = sizeof(uint32);
    eventData.idLen[17] = sizeof(uint32);
    eventData.idLen[18] = sizeof(int);
    eventData.idLen[19] = sizeof(int);
    eventData.cols.oad[20].value = 0X00118201;
    eventData.cols.oad[21].value = 0X00218201;
    eventData.cols.oad[22].value = 0X00318201;
    eventData.cols.oad[23].value = 0X00418201;
    eventData.idLen[20] = sizeof(uint32);
    eventData.idLen[21] = sizeof(uint32);
    eventData.idLen[22] = sizeof(int);
    eventData.idLen[23] = sizeof(int);
    eventData.cols.oad[24].value = 0X00128201;
    eventData.cols.oad[25].value = 0X00228201;
    eventData.cols.oad[26].value = 0X00328201;
    eventData.cols.oad[27].value = 0X00428201;
    eventData.idLen[24] = sizeof(uint32);
    eventData.idLen[25] = sizeof(uint32);
    eventData.idLen[26] = sizeof(int);
    eventData.idLen[27] = sizeof(int);
    eventData.cols.oad[28].value = 0X00138201;
    eventData.cols.oad[29].value = 0X00238201;
    eventData.cols.oad[30].value = 0X00338201;
    eventData.cols.oad[31].value = 0X00438201;
    eventData.idLen[28] = sizeof(uint32);
    eventData.idLen[29] = sizeof(uint32);
    eventData.idLen[30] = sizeof(int);
    eventData.idLen[31] = sizeof(int);
    eventData.len = 4*32;

//    eventData.len = 4*16;
    if(happenFlg)
    {
        //写数据库
        ret = WriteEventToDB(0, &event_300F.eventCommInfo, eventSource, eventData, 1);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_300F 终端相序异常事件记录失败返回%d\n", ret);
            return;
        }

        EVENT_FMT_TRACE("CheckERC_300F 终端相序异常事件 记录成功, storagenum=%u !\n", event_300F.eventCommInfo.storagenum[0]);
    }
    else
    {
        //写数据库
        ret = WriteEventToDB(0, &event_300F.eventCommInfo, eventSource, eventData, 2);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_300F 终端相序异常恢复事件 记录失败返回%d\n", ret);
            return;
        }

        EVENT_FMT_TRACE("CheckERC_300F 终端相序异常恢复事件 记录成功, storagenum=%u !\n", event_300F.eventCommInfo.storagenum[0]);
    }
#endif
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
void VltSupassEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType)
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

    static  uint8 lastTimes[4][3][MTR485_MAX_NUM];   //越上上限、下下限的累计时间
    uint8   lastTimesLmt = 1;                        //默认的电压异常上限持续时间
    uint8   lastTimesLmt1 = 1;                       //默认的电压异常下限持续时间
    uint16  vlt_lmt_rev = 0;                         //-电压异常上限恢复值-
    uint16  vlt_lmt1_rev = 0;                        //-电压异常下限恢复值-
    char    tmpBuf[10];
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据

#if WRITE_LOG_EN
    uint16   i;
#endif
    //uint16    j;

    EVENT_FMT_DEBUG("停电标志bfgPower=%d\n",bfgPower);

    vlt[0] = event_global.vlt_a;
    vlt[1] = event_global.vlt_b;
    vlt[2] = event_global.vlt_c;
    cur[0] = Int_ABS(event_global.cur_a);
    cur[1] = Int_ABS(event_global.cur_b);
    cur[2] = Int_ABS(event_global.cur_c);

    if ((vlt[0] == APP_NONEWORD) && (vlt[1] == APP_NONEWORD) && (vlt[2] == APP_NONEWORD))
    {
        EVENT_FMT_DEBUG( "电压无效直接返回\n");

        return;
    }

    //---原来代码不设限值就返回，现在修改为默认参数 2500 ----return;---
    vlt_lmt = 2500;
    //--原来代码不设限值就返回，现在修改为默认参数 1870 ----return;---
    vlt_lmt1 = 1870;

    if (mtr_id == 0 && para_query_float32(0x802a, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt = tmpFloat*10;
    }

    if (mtr_id == 0 && para_query_float32(0x8028, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt1 = tmpFloat*10;
    }

    EVENT_FMT_DEBUG("测量点%d电压上限值=%d, 下限值=%d\n",mtr_id,vlt_lmt,vlt_lmt1);

    memset(tmpBuf, 0x0, sizeof(tmpBuf));

    //-原来代码不设限值就返回，现在修改为默认参数 10%---return;---
    tmpBuf[0] = 100;
    tmpBuf[1] = 0;
    vlt_lmt_rev = vlt_lmt*(1000-Int16_ABS((*(uint16 *)tmpBuf)))/1000;    //返回系数在数据字典中%扩大了10倍

    //-原来代码不设限值就返回，现在修改为默认参数 10%---return;---
    tmpBuf[0] = 100;
    tmpBuf[1] = 0;
    vlt_lmt1_rev = vlt_lmt1*(1000+Int16_ABS((*(uint16 *)tmpBuf)))/1000;//返回系数在数据字典中%扩大了10倍,下限+系数

    lastTimesLmt = 1;           //容错
    lastTimesLmt1 = 1;          //容错

    EVENT_FMT_DEBUG("CheckERC24 测量点%d电压上限恢复值=%d    持续时间=%d\n",mtr_id,vlt_lmt_rev,lastTimesLmt);
    EVENT_FMT_DEBUG("CheckERC24 测量点%d电压下限恢复值=%d    持续时间=%d\n",mtr_id,vlt_lmt1_rev,lastTimesLmt1);

    if((bType == 1)||(bType == 2))
    {
        //-读取相位记录信息-
        len = read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+7), 1);
    }
    else
    {
        //-读取相位记录信息-
        len = read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+8), 1);
    }

    if (len < 0)
    {
        EVENT_FMT_DEBUG("读取事件状态失败");
        return;
    }

    if (bData == 0xff) //文件系统初始为0xff值
    {
        bData = 0;
    }

    EVENT_FMT_DEBUG("读取事件状态%02X\n", bData);

    if((cur[0] == 0x0)&&(vlt[0] == 0x0)&&(cur[1] == 0x0)&&(vlt[1] == 0x0)&&(cur[2] == 0x0)&&(vlt[2] == 0x0))
        return;//-电压电流都为0，是停电-

    for(order = 0; order < 3; order++)
    {
        if((vlt[order] == 0xeeee)||(vlt[order] == 0xffff))
        {
            continue;
        }
        if((event_global.power_type == 2) || (event_global.power_type == 1 && order != 1))    //三相三线,不判B相
        {
            switch (bType)
            {
            case 1:        //电压过压
                if(vlt[order] > vlt_lmt)        //如果满足相应的发生条件，将对应的时间进行累加
                {
                    if(lastTimes[bType-1][order][mtr_id] >= lastTimesLmt-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
					yx_calculate_sta_set(CALC_YX_VOLT_OVER_H, order, 1);
                    EVENT_FMT_DEBUG("第%d相满足电压过压发生条件，已持续时间lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("第%d相不满足电压过压发生条件，时间清零\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }
                break;

            case 2:        //电压过压恢复
                if((vlt[order] <= vlt_lmt_rev) && (bData &(1<<order)))    //电压小于上上限恢复阀值 并且 已经发生过越上上限
                {
                    //如果满足相应的发生条件，将对应的时间进行累加
                    if(lastTimes[bType-1][order][mtr_id] >= lastTimesLmt-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
					yx_calculate_sta_set(CALC_YX_VOLT_OVER_H, order, 0);
                    EVENT_FMT_DEBUG("第%d相满足电压过压恢复条件，已持续时间lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("第%d相不满足电压过压恢复条件，时间清零\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }
                break;

            case 3:        //电压失压
                if(vlt[order] < vlt_lmt1)        //如果满足相应的发生条件，将对应的时间进行累加
                {
                    if(lastTimes[bType-1][order][mtr_id] >= lastTimesLmt1-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
					yx_calculate_sta_set(CALC_YX_VOLT_DOWN_L, order, 1);
                    EVENT_FMT_DEBUG("第%d相满足电压失压发生条件，已持续时间lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("第%d相不满足电压失压发生条件，时间清零\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }

                break;

            case 4:        //电压失压恢复
                if((vlt[order] >= vlt_lmt1_rev) && (bData &(1<<order)))    //电压大于于下下限恢复阀值 并且 已经发生过越下下限
                {
                    //如果满足相应的发生条件，将对应的时间进行累加
                    if(lastTimes[bType-1][order][mtr_id] >= lastTimesLmt1-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
					yx_calculate_sta_set(CALC_YX_VOLT_DOWN_L, order, 0);
                    EVENT_FMT_DEBUG("第%d相满足电压失压恢复条件，已持续时间lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("第%d相不满足电压失压恢复条件，时间清零\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
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
                        }
                        else
                        {
                            ERFlg = LOW_VLT;
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
                        }
                        else
                        {
                            ERFlg = LOW_VLT;
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
                        }
                        else
                        {
                            ERFlg = LOW_VLT;
                        }
                    }
                    break;
                default:
                    break;
                }
                if(bType == 1)
                {
                    if(write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+7), 1)<0)
                    {
                        return;
                    }
                }
                else
                {
                    if(write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+8), 1)<0)
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
                        ERFlg = SUPASS_VLT;    //-超上限标志-
                    else
                        ERFlg = LOW_VLT;
                    break;
                case 1:
                    bData  &= ~B_PHASE;        //-清相位标记-
                    bphase |= B_PHASE;
                    if(bType == 2)
                        ERFlg = SUPASS_VLT;    //-超上限标志-
                    else
                        ERFlg = LOW_VLT;
                    break;
                case 2:
                    bData  &= ~C_PHASE;        //-清相位标记-
                    bphase |= C_PHASE;
                    if(bType == 2)
                        ERFlg = SUPASS_VLT;    //-超上限标志-
                    else
                        ERFlg = LOW_VLT;
                    break;
                default:
                    break;
                }
                if(bType == 2)
                {
                    if(write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+7), 1)<0)
                    {
                        return;
                    }
                }
                else
                {
                    if(write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+8), 1)<0)
                    {
                        return;
                    }
                }
            }
        }
    }

    if(ERFlg)//-事件发生-
    {
        EVENT_COMM_INFO* pEventCommInfo;

        //事件发生源
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter->basic.tsa;

        //事件关联数据
        eventData.oadNum = 2;
        eventData.cols.nNum = 2;
        eventData.cols.oad[0].value = 0X20002200;
        eventData.cols.oad[1].value = 0X20012200;
        eventData.idLen[0] = sizeof(OOP_WORD3V_T);
        eventData.idLen[1] = sizeof(OOP_WORD3V_T);
        eventData.len = eventData.idLen[0]+eventData.idLen[1];

        if (bType == 3 || bType == 4)
        {
            pEventCommInfo = &event_3001.eventCommInfo;
            memcpy(eventData.buf, &event_3001.oopWord3V[mtr_id], eventData.idLen[0]);
            memcpy(eventData.buf+eventData.idLen[0], &event_3001.oopWord3C[mtr_id], eventData.idLen[1]);
        }
        else {
            pEventCommInfo = &event_3002.eventCommInfo;
            memcpy(eventData.buf, &event_3002.oopWord3V[mtr_id], eventData.idLen[0]);
            memcpy(eventData.buf+eventData.idLen[0], &event_3002.oopWord3C[mtr_id], eventData.idLen[1]);
        }

        if(bType==1 || bType==3)//发生
        {
            //写数据库
            int ret = WriteEventToDB(mtr_id, pEventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("测量点mtr_id=%hu  断相事件记录CheckERC_3001失败返回%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("测量点mtr_id=%hu  断相事件记录CheckERC_3001成功 storagenum=%u \n", mtr_id, pEventCommInfo->storagenum[mtr_id]);
        }
        else
        {
            //恢复事件记录内容
            int ret = WriteEventToDB(mtr_id, pEventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("测量点mtr_id=%hu  电流反极性事件恢复记录CheckERC_3001失败返回%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("测量点mtr_id=%hu  电流反极性事件恢复记录CheckERC_3001成功 , storagenum=%u !\n", mtr_id, pEventCommInfo->storagenum[mtr_id]);
        }

        if((bData == 0)&&((bType == 2)||(bType == 4)))
        {
            len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);
            dwERFlgTmp &= ~ERFlg;    //-保存发生标记到此测量点全局事情标记中-
            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);

            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                if(bType == 2)
                    SaveFileRecBfg(ERC_NO_24|0x80,(bphase&0x07),mtr_id);//-越上限恢复-
                else
                    SaveFileRecBfg(ERC_NO_24,     (bphase&0x07),mtr_id);//-越下限恢复-
            }
        }
        else
        {
            len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);
            dwERFlgTmp |= ERFlg;    //-置该测量点全局事件标志位-
            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);

            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                if(bType == 1)
                   SaveFileRecBfg(ERC_NO_24|0x80,(bphase&0x07)|0x80,mtr_id);//-越上限发生-
                else
                   SaveFileRecBfg(ERC_NO_24     ,(bphase&0x07)|0x80,mtr_id);//-越下限发生-
            }
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
void CurHighEvent(mtrnum_t  mtr_id, OOP_METER_T *oopMeter, uint8 bType)
{
    //uint16    i;
    uint16  vlt[3] = {0};                            //当前电压
    uint32  cur[3] = {0};                            //当前电流
    int     cur_lmt1[3]= {0},cur_lmt2[3]= {0};       //门限值
    uint32  ERFlg = 0;
    uint8   order;//,bTemp;
    //uint16  mtetmp = 0;
    uint8   is_now_flag = 0;
    uint8   bData=0,bphase = 0;
    int16   len;
    //uint32  dwERFlgTmp=0;
    static uint8 lastTimes[4][3][MTR485_MAX_NUM];    //越上上限、下下限的累计时间
    uint8   lastTimesLmt1 = 1;                       //默认的电流越上限持续时间
    uint8   lastTimesLmt2 = 1;                       //默认的电流越上上限持续时间
    uint16  cur_lmt1_rev = 0;                        //电流越上限恢复值
    uint16  cur_lmt2_rev = 0;                        //电流越上上限恢复值
    float32 tmpFloat;
    char    tmpBuf[10];

    vlt[0] = event_global.vlt_a;
    vlt[1] = event_global.vlt_b;
    vlt[2] = event_global.vlt_c;
    cur[0] = Int_ABS(event_global.cur_a);
    cur[1] = Int_ABS(event_global.cur_b);
    cur[2] = Int_ABS(event_global.cur_c);

    if ((event_global.cur_a == APP_NONELWORD) && (event_global.cur_b == APP_NONELWORD) && (event_global.cur_c == APP_NONELWORD))
    {
        EVENT_FMT_DEBUG( "电流无效直接返回\n");

        return;
    }

    //读电流上限值
    cur_lmt1[0] = (uint16)event_global.cur_gate * 120 / 100;
    cur_lmt1[0] &= 0x7fffffff;                        //去除符号位
    if (mtr_id == 0 && para_query_float32(0x802c, &tmpFloat) > 0 && tmpFloat > 0)
    {
        cur_lmt1[0] = tmpFloat*1000;
    }

    //读电流上上限值
    cur_lmt2[0] = (uint16)event_global.cur_gate * 160 / 100;
    cur_lmt2[0] &= 0x7fffffff;                        //去除符号位
    if (mtr_id == 0 && para_query_float32(0x802e, &tmpFloat) > 0 && tmpFloat > 0)
    {
        cur_lmt2[0] = tmpFloat*1000;
    }

    EVENT_FMT_DEBUG("测量点%d电流上上限值=%ld 上限值=%ld \n", mtr_id, cur_lmt2[0], cur_lmt1[0]);

    //读取相电流上限（额定电流门限）的返回系数
    tmpBuf[0] = 0;
    tmpBuf[1] = 0;

    //返回系数在数据字典中%扩大了10倍
    if(Int16_ABS((*(uint16 *)tmpBuf))>1000)
    {
        return;
    }
    else
    {
        cur_lmt1_rev = cur_lmt1[0]*(1000-Int16_ABS((*(uint16 *)tmpBuf)))/1000;
    }

    //读取相电流上上限（过流门限）的返回系数
    tmpBuf[0] = 0;
    tmpBuf[1] = 0;

    //返回系数在数据字典中%扩大了10倍
    if(Int16_ABS((*(uint16 *)tmpBuf))>1000)
    {
        return;
    }
    else
    {
        cur_lmt2_rev = cur_lmt2[0]*(1000-Int16_ABS((*(uint16 *)tmpBuf)))/1000;
    }

    //电流上限的持续时间
    lastTimesLmt1 = 1;                        //容错

    EVENT_FMT_DEBUG("测量点%d电流上限（额定电流门限）恢复值=%d 持续时间=%d\n",mtr_id,cur_lmt1_rev,lastTimesLmt1);

    //电流上上限的持续时间
    lastTimesLmt2 = 1;                        //容错

    EVENT_FMT_DEBUG("测量点%d电流上上限（过流门限）恢复值=%d 持续时间=%d\n",mtr_id,cur_lmt2_rev,lastTimesLmt2);

    //读取相位信息
    if((bType == 1) || (bType == 2))
    {
        len =  read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+9), 1);
    }
    else
    {
        len =  read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+10), 1);
    }

    if (len < 0)
    {
        EVENT_FMT_DEBUG("读取事件状态失败");
        return;
    }

    if (bData == 0xff) //文件系统初始为0xff值
    {
        bData = 0;
    }

    EVENT_FMT_DEBUG("读取事件状态%02X\n", bData);

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

    //循环检测三相是否符合事件条件
    for (order = 0; order < 3; order++)
    {
        if ((cur[order] == 0x6eeeeeee) || (cur[order] == 0x7fffffff) || (cur[order] == APP_NONELWORD))
        {
            continue;
        }

        //三相三线,不判B相
        if ((event_global.power_type == 2)
         || (event_global.power_type == 1 && order != 1))
        {
            switch (bType)
            {
            case 1:                                //电流越上限
                EVENT_FMT_DEBUG("测量点%d电流越<上限发生>检测 \n", mtr_id);
                EVENT_FMT_DEBUG(">cur_lmt1[0] = %ld    <cur_lmt2[0] = %ld\n", cur_lmt1[0], cur_lmt2[0]);
                if ((cur[order] > cur_lmt1[0]) && (cur[order] <= cur_lmt2[0]))
                {
                    //如果满足相应的发生条件，将对应的时间进行累加
                    if (lastTimes[bType-1][order][mtr_id] >= lastTimesLmt1-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
                    EVENT_FMT_DEBUG("第%d相满足电流越上限发生条件，已持续时间lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("第%d相不满足电流越上限发生条件，时间清零\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }
                break;

            case 2:                                //电流越上限恢复
                EVENT_FMT_DEBUG("测量点%d电流越<上限恢复>检测 \n", mtr_id);
                EVENT_FMT_DEBUG("cur_lmt1_rev = %d\n", cur_lmt1_rev);
                //电压小于上上限恢复阀值 并且 已经发生过越上上限
                if((cur[order] <= cur_lmt1_rev) && (bData &(1<<order)))
                {
                    //如果满足相应的发生条件，将对应的时间进行累加
                    if (lastTimes[bType-1][order][mtr_id] >= lastTimesLmt1-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
                    EVENT_FMT_DEBUG("第%d相满足电流越上限恢复条件，已持续时间lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("第%d相不满足电流越上限恢复条件，时间清零\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }
                break;

            case 3:                                //电流越上上限
                EVENT_FMT_DEBUG("测量点%d电流越<上上限发生>检测 \n", mtr_id);
                EVENT_FMT_DEBUG("cur_lmt2[0] = %ld\n", cur_lmt2[0]);
                //如果满足相应的发生条件，将对应的时间进行累加
                if (cur[order] > cur_lmt2[0])
                {
                    if (lastTimes[bType-1][order][mtr_id] >= lastTimesLmt2-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
                    EVENT_FMT_DEBUG("第%d相满足电流越上上限发生条件，已持续时间lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("第%d相不满足电流越上上限发生条件，时间清零\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }

                break;

            case 4:                                //电流越上上限恢复
                EVENT_FMT_DEBUG("测量点%d电流越<上上限恢复>检测 \n", mtr_id);
                EVENT_FMT_DEBUG("cur_lmt2_rev = %d\n", cur_lmt2_rev);
                //电压大于于下下限恢复阀值 并且 已经发生过越下下限
                if((cur[order] <= cur_lmt2_rev) && (bData &(1<<order)))
                {
                    //如果满足相应的发生条件，将对应的时间进行累加
                    if (lastTimes[bType-1][order][mtr_id] >= lastTimesLmt2-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
                    EVENT_FMT_DEBUG("第%d相满足电流越上上限恢复条件，已持续时间lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("第%d相不满足电流越上上限恢复条件，时间清零\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
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
			if (bType == 1)
            {
                yx_calculate_sta_set(CALC_YX_TR_OVERLOAD, 0, 1);   //置电流越上限事件标志
            }
            else
            {
                yx_calculate_sta_set(CALC_YX_TR_HEAVYLOAD, 0, 1);   //置电流越上上限事件标志
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
			if (bType == 2)
            {
                yx_calculate_sta_set(CALC_YX_TR_OVERLOAD, 0, 0);   //置电流越上限事件标志
            }
            else
            {
                yx_calculate_sta_set(CALC_YX_TR_HEAVYLOAD, 0, 0);   //置电流越上上限事件标志
            }
        }
    }

    if ((bType == 1)||(bType == 2))
    {
        if (write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+9), 1)<0)
        {
            return;
        }
    }
    else
    {
        if (write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+10), 1)<0)
        {
            return;
        }
    }

    if (ERFlg)                       //事件发生
    {
        OOP_EVTSOURCE_T eventSource; //事件发生源
        OOP_EVTBUF_T    eventData;   //事件关联数据
        EVENT_COMM_INFO* pEventCommInfo;

        //TODO 写入数据库
        //事件发生源
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter->basic.tsa;

        //事件关联数据
        eventData.oadNum = 1;
        eventData.cols.nNum = 1;
        eventData.cols.oad[0].value = 0X20012200;
        eventData.idLen[0] = sizeof(OOP_WORD3V_T);
        eventData.len = eventData.idLen[0];

        pEventCommInfo = &event_3005.eventCommInfo;
        memcpy(eventData.buf, &event_3005.oopWord3C[mtr_id], eventData.idLen[0]);

        if(bType==1 || bType==3)//发生
        {
            //写数据库
            int ret = WriteEventToDB(mtr_id, pEventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("测量点mtr_id=%hu  断相事件记录CheckERC_3005失败返回%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("测量点mtr_id=%hu  断相事件记录CheckERC_3005成功 storagenum=%u \n", mtr_id, pEventCommInfo->storagenum[mtr_id]);
        }
        else
        {
            //恢复事件记录内容
            int ret = WriteEventToDB(mtr_id, pEventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("测量点mtr_id=%hu  电流反极性事件恢复记录CheckERC_3005失败返回%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("测量点mtr_id=%hu  电流反极性事件恢复记录CheckERC_3005成功 , storagenum=%u !\n", mtr_id, pEventCommInfo->storagenum[mtr_id]);
        }

        if ((bType == 1)||(bType == 3))
        {
            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                SaveFileRecBfg(ERC_NO_25|0x80,(bphase&0x07)|0x80,mtr_id);//记录电流越上限发生
            }
        }
        else
        {
            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                SaveFileRecBfg(ERC_NO_25|0x80,(bphase&0x07),mtr_id);//记录电流越上限恢复
            }
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
void VltCurImbalanceEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType)
{
    int    max_cur;    //-最大电流偏差-
    int    min_cur;    //-最小电流偏差-
    int16  max_vlt;    //-最大电压偏差-
    int16  min_vlt;    //-最小电压偏差-
    uint16 vlt_para,cur_para;
    //uint32 ulTemp = 0;
    //int8  sign1 = 0,sign2 = 0;

    //int8   bTemp = 0;
    uint16 vlt[3];
    int    cur[3];
    int    dwERFlgTmp = 0;
    int16  len = 0;
    //uint16    j;

    static uint8 lastTimes[4][MTR485_MAX_NUM];    //三相电压、电流不平衡持续时间 0 电压不平衡发生 1 恢复 2 电流不平衡发生 3 恢复
    uint8  lastTimesLmt = 1;        //默认的三相电压不平衡持续时间
    uint8  lastTimesLmt1 = 1;       //默认的三相电流不平衡持续时间
    uint16 vlt_para_rev = 0;        //三相电压不平衡恢复值
    uint16 cur_para_rev = 0;        //三相电流不平衡恢复值
    int8   vlt_status = 0, cur_status = 0; //电压、电流不平衡发生、恢复标志  1 发生 2 恢复
    int8   tmp_flag1 = 0, tmp_flag2 = 0;
    char   tmpBuf[10];

    //-原来代码不设限值就返回，现在修改为默认参数 30% ----return;---
    vlt_para = 300;
    cur_para = 300;

    memset(tmpBuf, 0x0, sizeof(tmpBuf));
    //读取持续时间和返回系数
    //-原来代码不设限值就返回，现在修改为默认参数 10% ----return;---
    tmpBuf[0] = 10;
    tmpBuf[1] = 0;
    vlt_para_rev = vlt_para*(1000-Int16_ABS(*(uint16 *)tmpBuf))/1000;    //返回系数在数据字典中%扩大了10倍

    //-原来代码不设限值就返回，现在修改为默认参数 10% ----return;---
    tmpBuf[0] = 10;
    tmpBuf[1] = 0;
    cur_para_rev = cur_para*(1000-Int16_ABS(*(uint16 *)tmpBuf))/1000;    //返回系数在数据字典中%扩大了10倍

    EVENT_FMT_DEBUG("测量点%u电压不平衡限值%d,恢复限值%d,电流不平衡限值%d,恢复限值%d\n", mtr_id, vlt_para, vlt_para_rev, cur_para, cur_para_rev);

    lastTimesLmt = 1;       //容错
    lastTimesLmt1 = 1;      //容错

    if((event_global.vlt_a == 0xeeee)||(event_global.vlt_b == 0xeeee)||(event_global.vlt_c == 0xeeee)
        ||(event_global.cur_a == 0xeeeeeeee)||(event_global.cur_b == 0xeeeeeeee)||(event_global.cur_c == 0xeeeeeeee)
        ||(event_global.vlt_a == 0xffff)||(event_global.vlt_b == 0xffff)||(event_global.vlt_c == 0xffff)
        ||(event_global.cur_a == 0xffffffff)||(event_global.cur_b == 0xffffffff)||(event_global.cur_c == 0xffffffff))
    {
        //-任意一项电压电流为0-
        return;
    }

    len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*(4)), 4);
    if (len < 0)
    {
        EVENT_FMT_DEBUG("读取事件状态失败");
        return;
    }

    if (dwERFlgTmp == -1) //文件系统初始为0xff值
    {
        dwERFlgTmp = 0;
    }

    EVENT_FMT_DEBUG("读取事件状态%08X\n", dwERFlgTmp);

    //获取最大电压电流值以及最小电压电流值
    if(event_global.power_type==2)    //-三相四线-
    {
        vlt[0] = event_global.vlt_a;
        vlt[1] = event_global.vlt_b;
        vlt[2] = event_global.vlt_c;
        cur[0] = event_global.cur_a;
        cur[1] = event_global.cur_b;
        cur[2] = event_global.cur_c;

        //电流值带符号，需进一步处理
        if(cur[0]&0x80000000) cur[0] = (-1) * (cur[0]&0x7fffffff);
        if(cur[1]&0x80000000) cur[1] = (-1) * (cur[1]&0x7fffffff);
        if(cur[2]&0x80000000) cur[2] = (-1) * (cur[2]&0x7fffffff);

        CmpValuint16(3,vlt,(uint16*)&max_vlt,(uint16*)&min_vlt);
        CmpValint(3,cur,&max_cur,&min_cur);
    }
    else if(event_global.power_type==1)    //-三相三线-
    {
        vlt[0] = event_global.vlt_a;
        vlt[1] = event_global.vlt_c;
        cur[0] = event_global.cur_a;
        cur[1] = event_global.cur_c;

        //电流值带符号，需进一步处理
        if(cur[0]&0x80000000) cur[0] = (-1) * (cur[0]&0x7fffffff);
        if(cur[1]&0x80000000) cur[1] = (-1) * (cur[1]&0x7fffffff);

        CmpValuint16(2,vlt,(uint16*)&max_vlt,(uint16*)&min_vlt);
        CmpValint(2,cur,&max_cur,&min_cur);
    }

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
            if(lastTimes[0][mtr_id] >= lastTimesLmt-1)
            {
                vlt_status = 1;
				yx_calculate_sta_set(CALC_YX_VOLT_IMBALANCE, 0, 1);
            }
            else
            {
                lastTimes[0][mtr_id]++;
                vlt_status = 0;
            }
        }
        else if(!tmp_flag2 && (dwERFlgTmp & VLT_NBLC))    //没有发生不平衡，并且以前记录过该事件
        {
            //如果满足相应的发生条件，将对应的时间进行累加
            if(lastTimes[1][mtr_id] >= lastTimesLmt-1)
            {
                vlt_status = 2;
				yx_calculate_sta_set(CALC_YX_VOLT_IMBALANCE, 0, 0);
            }
            else
            {
                lastTimes[1][mtr_id]++;
                vlt_status = 0;
            }
        }
        else
        {
            lastTimes[0][mtr_id] = 0;
            lastTimes[1][mtr_id] = 0;
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
            tmp_flag1 = ((uint32)(max_cur-min_cur)*1000/abs(max_cur)) > cur_para;
            tmp_flag2 = ((uint32)(max_cur-min_cur)*1000/abs(max_cur)) > cur_para_rev;
        }

        //0 电压不平衡发生 1 恢复 2 电流不平衡发生 3 恢复
        if((tmp_flag1) && (!(dwERFlgTmp & CUR_NBLC)))        //发生了不平衡，并且以前没有记录过该事件
        {
            //如果满足相应的发生条件，将对应的时间进行累加
            if(lastTimes[2][mtr_id] >= lastTimesLmt1-1)
            {
                cur_status = 1;
				yx_calculate_sta_set(CALC_YX_CURR_IMBALANCE, 0, 1);
            }
            else
            {
                lastTimes[2][mtr_id]++;
                cur_status = 0;
            }
        }
        else if(!tmp_flag2 && (dwERFlgTmp & CUR_NBLC))    //没有发生不平衡，并且以前记录过该事件
        {
            //如果满足相应的发生条件，将对应的时间进行累加
            if(lastTimes[3][mtr_id] >= lastTimesLmt1-1)
            {
                cur_status = 2;
				yx_calculate_sta_set(CALC_YX_CURR_IMBALANCE, 0, 0);
            }
            else
            {
                lastTimes[3][mtr_id]++;
                cur_status = 0;
            }
        }
        else
        {
            lastTimes[2][mtr_id] = 0;
            lastTimes[3][mtr_id] = 0;
            cur_status = 0;
        }
    }

    if (vlt_status != 0 || cur_status != 0)
    {
        //TODO 事件写入数据库
        OOP_EVTSOURCE_T eventSource; //事件发生源
        OOP_EVTBUF_T    eventData;   //事件关联数据
        EVENT_COMM_INFO* pEventCommInfo;

        //事件发生源
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter->basic.tsa;

        //事件关联数据
        eventData.oadNum = 1;
        eventData.cols.nNum = 1;
        eventData.idLen[0] = sizeof(OOP_WORD3V_T);
        eventData.len = eventData.idLen[0];

        if(bType==1) {
            eventData.cols.oad[0].value = 0X20002200;
            pEventCommInfo = &event_301D.eventCommInfo;
            memcpy(eventData.buf, &event_301D.oopWord3V[mtr_id], eventData.idLen[0]);
        }
        else {
            eventData.cols.oad[0].value = 0X20012200;
            pEventCommInfo = &event_301E.eventCommInfo;
            memcpy(eventData.buf, &event_301E.oopWord3C[mtr_id], eventData.idLen[0]);
        }

        //-事件发生了，且没有记录-
        if((vlt_status == 1) || (cur_status == 1))
        {
            //写数据库
            int ret = WriteEventToDB(mtr_id, pEventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("测量点mtr_id=%hu  断相事件记录CheckERC_301D失败返回%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("测量点mtr_id=%hu  断相事件记录CheckERC_301D成功 storagenum=%u \n", mtr_id, pEventCommInfo->storagenum[mtr_id]);

            if(vlt_status == 1)
            {
                dwERFlgTmp |= VLT_NBLC;        //-电压事件标志-

                if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
                {
                    SaveFileRecBfg(ERC_NO_17|0x80,0x80,mtr_id);//-电压不平衡-
                }
            }
            if(cur_status == 1)
            {
                dwERFlgTmp |= CUR_NBLC;        //-电流事件标志-

                if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
                {
                    SaveFileRecBfg(ERC_NO_17,0x80,mtr_id);//-电流不平衡-
                }
            }

            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*(4)), 4);
        }

        //-事件没有发生，且已有记录，则恢复-
        if((vlt_status == 2) || (cur_status == 2))
        {
            //恢复事件记录内容
            int ret = WriteEventToDB(mtr_id, pEventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("测量点mtr_id=%hu  电流反极性事件恢复记录CheckERC_301D失败返回%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("测量点mtr_id=%hu  电流反极性事件恢复记录CheckERC_301D成功 , storagenum=%u !\n", mtr_id, pEventCommInfo->storagenum[mtr_id]);

            if(vlt_status == 2)
            {
                dwERFlgTmp &= ~VLT_NBLC;//-清除该事件标志-

                if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
                {
                    SaveFileRecBfg(ERC_NO_17|0x80,0,mtr_id);//-电压不平衡-
                }
            }

            if(cur_status == 2)
            {
                dwERFlgTmp &= ~CUR_NBLC;//-清除该事件标志-

                if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
                {
                    SaveFileRecBfg(ERC_NO_17,0,mtr_id);//-电流不平衡-
                }
            }

            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*(4)), 4);
        }
    }
}


