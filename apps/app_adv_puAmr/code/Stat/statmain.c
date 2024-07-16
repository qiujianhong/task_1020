/*
*********************************************************************
* @file    AutoTask.c
* @brief： 统计模块实现文件
* @author：贺宁
* @date：   2019.11.11
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#include "common.h"
#include "framecomm.h"
#include "task_msg.h"
#include "db_api.h"
#include "statlib.h"
#include "statmain.h"
#include "task_id.h"
#include "appcomm.h"
#include "storage.h"
#include "oopservice.h"
#include "hal.h"
#include "uart.h"
#include "mapmsg.h"
#include "mapmsg_split.h"
#include "commClass.h"
#include "area.h"


OOP_DATETIME_S_T   realstattime;     /*系统实时时间,全局时间变量*/
OOP_DATETIME_S_T   laststattime;     /*系统上一次统计时间*/
OOP_DATETIME_S_T   g_currtime;     /*当前时间,全局时间变量*/
OOP_DATETIME_S_T   g_lasttime;     /*当前时间,全局时间变量*/

extern uint32 datainitfalg;

/*以下为时间过秒,过分,过时,过日,过月，过年标记*/
uint8   pastminflag;
uint8   pasthourflag;
uint8   pastdayflag;
uint8   pastmonflag;
uint8   pastyearflag;

//分钟电量计算标记，防止总加组内部循环中重复计算
uint8   g_MinuteCalcEn = 1; 

uint32 g_frozen_index[STA_FROZEN_OBJNUMS] = {0};

/* UDP通信句柄，全局唯一 */
DB_CLIENT g_staUdpHandle = 0;

STA_PARAM_UPDATE g_staUpdateData = {{{0}},0xff,0xff,0xff,0xff};

/*冻结参数链表*/
FrzRunParaList_t* g_staFrozenParaList[STA_FROZEN_OBJNUMS]={NULL};

/*区间统计参数集合(分,时,日,月,年5种统计类型)*/
STA_REGIONS_PARA_T g_staRegionsPara[STA_REGION_OBJNUMS] = 
{
    {{.nObjID = STA_REGION_MIN},0,{{0}}},
    {{.nObjID = STA_REGION_HOUR},0,{{0}}},
    {{.nObjID = STA_REGION_DAY},0,{{0}}}, 
    {{.nObjID = STA_REGION_MON},0,{{0}}},    
    {{.nObjID = STA_REGION_YEAR},0,{{0}}},       
}; 

/*累加平均统计参数集合(分,时,日,月,年5种统计类型)*/
STA_TOTALAVGS_PARA_T g_staTotalavgsPara[STA_TOTALAVG_OBJNUMS] =
{
    {{.nObjID = STA_TOTALAVG_MIN},0,{{0}}},
    {{.nObjID = STA_TOTALAVG_HOUR},0,{{0}}},
    {{.nObjID = STA_TOTALAVG_DAY},0,{{0}}}, 
    {{.nObjID = STA_TOTALAVG_MON},0,{{0}}},    
    {{.nObjID = STA_TOTALAVG_YEAR},0,{{0}}},       
};

/*极值统计参数集合(分,时,日,月,年5种统计类型)*/
STA_EXTREMES_PARA_T g_staExtremesPara[STA_EXTREME_OBJNUMS] =
{
    {{.nObjID = STA_EXTREME_MIN},0,{{0}}},
    {{.nObjID = STA_EXTREME_HOUR},0,{{0}}},
    {{.nObjID = STA_EXTREME_DAY},0,{{0}}}, 
    {{.nObjID = STA_EXTREME_MON},0,{{0}}},    
    {{.nObjID = STA_EXTREME_YEAR},0,{{0}}},       
}; 

/*授权状态   -状态变化标记-是否收到安全认证命令-是否本地安全认证通过-授权时长*/
STA_PORT_AUTHORIZE_INFO g_portRS232 = {STA_PORT_AUTH_FORBIDDEN, TRUE, FALSE, FALSE, 0}; //232端口授权
STA_PORT_AUTHORIZE_INFO g_portRS485 = {STA_PORT_AUTH_PERMANENT, TRUE, FALSE, FALSE, 0}; //485维护端口授权(默认永久授权，此处与文档相悖)
STA_PORT_AUTHORIZE_INFO g_portUSB   = {STA_PORT_AUTH_FORBIDDEN, TRUE, FALSE, FALSE, 0}; //USB接入授权
STA_PORT_AUTHORIZE_INFO g_portIR    = {STA_PORT_AUTH_FORBIDDEN, TRUE, FALSE, FALSE, 0}; //红外端口授权
STA_PORT_AUTHORIZE_INFO g_portSSH   = {STA_PORT_AUTH_PERMANENT, TRUE, FALSE, FALSE, 0}; //SSH服务授权(默认永久授权，此处与文档相悖)
STA_PORT_AUTHORIZE_INFO g_portBT    = {STA_PORT_AUTH_FORBIDDEN, TRUE, FALSE, FALSE, 0}; //蓝牙端口授权
STA_PORT_AUTHORIZE_INFO g_portSFTP  = {STA_PORT_AUTH_PERMANENT, TRUE, FALSE, FALSE, 0}; //SFTP服务授权(默认永久授权，此处与文档相悖)

STA_ACDATA_T g_acData = {0};

int g_pt100_slot = 0;
STA_DC_ANALOG_E g_dc_colstatus = STA_DC_GETM;
BOOL g_mod_status_chg = 0;
extern STA_SLIPPOWER_FIFO_T g_slippower_fifo[OOP_MAX_TGSETS];
OOP_SERVICES_T g_serData = {0};
extern STA_TGDEL_T g_tgdel;
extern uint8 gdelay;

#if DESC("统计模块公共函数",1)

/**
*********************************************************************
* @brief：      交采电压数据变更
* @param[in]： clientid 数据中心客户端句柄
* @param[in]： logicId 逻辑设备号
* @param[in]： infoNum 信息点号
* @param[in]： value   变更的DI
* @Create    : 贺宁
* @Date      ：2020-05-09
* @return：     void
*********************************************************************
*/
int stat_acvolt_change(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_WORD3V_T phaseVolt;
    uint32 len = 0;

    nomalOAD.logicId = logicId;
    nomalOAD.infoNum = infoNum;
    nomalOAD.oad.value = value; //实时电压

    /* 读普通ODA获取电压值 */
    ret = db_read_nomal(clientid, &nomalOAD, sizeof(OOP_WORD3V_T), (uint8*)&phaseVolt, &len);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("rtVolt read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_WORD3V_T));
        return ret;
    }    

    g_acData.PhaAVolt = phaseVolt.nValue[0];
    g_acData.PhaBVolt = phaseVolt.nValue[1];
    g_acData.PhaCVolt = phaseVolt.nValue[2];   

    return ret;
}

int stat_change_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int ret = 0;
    NOMAL_OAD_T nomal = {0};
    OOP_PORTS_T data;
    uint32 len = 0;
    int i = 0;
    
    MEMZERO(&data, sizeof(data));

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_staUpdateData.mutex);

    switch(value)
    {
        case 0x50000300:
        {
            g_staUpdateData.frzInitFlag |= (1UL<<0);
        }break;
        case 0x50010300:
        {
            g_staUpdateData.frzInitFlag |= (1UL<<1);
        }break;      
        case 0x50020300:
        {
            g_staUpdateData.frzInitFlag |= (1UL<<2);
        }break;   
        case 0x50030300:
        {
            g_staUpdateData.frzInitFlag |= (1UL<<3);
        }break;  
        case 0x50040300:
        {
            g_staUpdateData.frzInitFlag |= (1UL<<4);
        }break;  
        case 0x50050300:
        {
            g_staUpdateData.frzInitFlag |= (1UL<<5);
        }break;            
        case 0x50060300:
        {
            g_staUpdateData.frzInitFlag |= (1UL<<6);
        }break;  
        case 0x50070300:
        {
            g_staUpdateData.frzInitFlag |= (1UL<<7);
        }break;
        case 0x21000300:
        {
            g_staUpdateData.regionInitFlag |= (1UL<<0);
        }break;
        case 0x21010300:
        {
            g_staUpdateData.regionInitFlag |= (1UL<<1);
        }break;      
        case 0x21020300: 
        {
            g_staUpdateData.regionInitFlag |= (1UL<<2);
        }break;   
        case 0x21030300:
        {
            g_staUpdateData.regionInitFlag |= (1UL<<3);
        }break;  
        case 0x21040300:
        {
            g_staUpdateData.regionInitFlag |= (1UL<<4);
        }break;          
        case 0x21100300:
        {
            g_staUpdateData.avgInitFlag |= (1UL<<0);
        }break;
        case 0x21110300:
        {
            g_staUpdateData.avgInitFlag |= (1UL<<1);
        }break;      
        case 0x21120300:
        {
            g_staUpdateData.avgInitFlag |= (1UL<<2);
        }break;   
        case 0x21130300:
        {
            g_staUpdateData.avgInitFlag |= (1UL<<3);
        }break;  
        case 0x21140300: 
        {
            g_staUpdateData.avgInitFlag |= (1UL<<4);
        }break;          
        case 0x21200300:
        {
            g_staUpdateData.extremeInitFlag |= (1UL<<0);
        }break;
        case 0x21210300:
        {
            g_staUpdateData.extremeInitFlag |= (1UL<<1);
        }break;      
        case 0x21220300:
        {
            g_staUpdateData.extremeInitFlag |= (1UL<<2);
        }break;   
        case 0x21230300:
        {
            g_staUpdateData.extremeInitFlag |= (1UL<<3);
        }break;  
        case 0x21240300:
        {
            g_staUpdateData.extremeInitFlag |= (1UL<<4);
        }break;    
        case 0x43000B00:
        {
            memset(&g_serData, 0, sizeof(g_serData));
            
            //SSH服务授权参数
            nomal.oad.value = value;
            
            ret = db_read_nomal(clientid, &nomal, sizeof(g_serData), (uint8*)&g_serData, &len);
            if(ret == 0 )
            {
                for(i = 0; i < g_serData.nNum; i++)
                {
                    if(g_serData.item[i].type == 1)
                    {
                        sta_tmn_port_authorize_status(g_serData.item[i].times, &g_portSSH);
                        
                        //发送服务授权消息
                        g_portSSH.isAuthChg = TRUE;

                        AUTH_FMT_DEBUG("SSH服务授权: time %d, status %d, chg %d\n", g_serData.item[i].times, g_portSSH.status, g_portSSH.isAuthChg);
                    }
                    else if(g_serData.item[i].type == 2)
                    {
                        sta_tmn_port_authorize_status(g_serData.item[i].times, &g_portSFTP);
                        
                        //发送服务授权消息
                        g_portSFTP.isAuthChg = TRUE;
                        
                        AUTH_FMT_DEBUG("SFTP服务授权: time %d, status %d, chg %d\n", g_serData.item[i].times, g_portSFTP.status, g_portSFTP.isAuthChg);
                    }
                }
            }
            else
            {
                AUTH_FMT_DEBUG("tmn_port_authorize read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                    ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(data));
            }

            
        }break;           
        case 0xF2000500:
        {
            //RS232端口授权参数
            nomal.oad.value = value;
            
            ret = db_read_nomal(clientid, &nomal, sizeof(data), (uint8*)&data, &len);
            if(ret == 0 )
            {
                sta_tmn_port_authorize_status(data.item[0], &g_portRS232);
            }
            else
            {
                AUTH_FMT_DEBUG("tmn_port_authorize read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                    ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(data));
            }

        }break;       
        case 0xF2010500:
        {
            //RS485端口授权参数
            nomal.oad.value = value;
            
            ret = db_read_nomal(clientid, &nomal, sizeof(data), (uint8*)&data, &len);
            if(ret == 0 )
            {
                sta_tmn_port_authorize_status(data.item[0], &g_portRS485);
            }
            else
            {
                AUTH_FMT_DEBUG("tmn_port_authorize read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                    ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(data));
            }

        }break;    
        case 0xF2020500:
        {
            //红外端口授权参数
            nomal.oad.value = value;
            
            ret = db_read_nomal(clientid, &nomal, sizeof(data), (uint8*)&data, &len);
            if(ret == 0 )
            {
                sta_tmn_port_authorize_status(data.item[0], &g_portIR);
            }
            else
            {
                AUTH_FMT_DEBUG("tmn_port_authorize read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                    ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(data));
            }

        }break;    
        case 0xF20E0500:
        {
            //USB端口授权参数
            nomal.oad.value = value;
            
            ret = db_read_nomal(clientid, &nomal, sizeof(data), (uint8*)&data, &len);
            if(ret == 0 )
            {
                //发送服务授权消息
                g_portUSB.isAuthChg = TRUE;
                sta_tmn_port_authorize_status(data.item[0], &g_portUSB);
            }
            else
            {
                AUTH_FMT_DEBUG("tmn_USB port_authorize read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                    ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(data));
            }

        }break;     
        default:break;        
    }

    if(value == 0x20000200)
    {
        stat_acvolt_change(clientid, logicId, infoNum, value);
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_staUpdateData.mutex);    

    return 0;
}

/**
*********************************************************************
* @brief：     统计数据变更初始化
* @param[in]： NA
* @param[out]：NA
* @return：     -1 执行错误
*********************************************************************
*/
void stat_change_init()
{
    pthread_mutex_init(&g_staUpdateData.mutex, NULL);
}

/**
*********************************************************************
* @brief：      过时标记更新
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_spantime_update(void)
{   
    if(realstattime.year != laststattime.year)
    {
        pastyearflag = 1;
        pastmonflag = 1;
        pastdayflag = 1;
        pasthourflag = 1;
        pastminflag = 1;
    }
    else
    {
        pastyearflag = 0;
        if(realstattime.month != laststattime.month)
        {
            pastmonflag = 1;
            pastdayflag = 1;
            pasthourflag = 1;
            pastminflag = 1;
        }
        else
        {
            pastmonflag = 0;
            if(realstattime.day != laststattime.day)
            {
                pastdayflag = 1;
                pasthourflag = 1;
                pastminflag = 1;
				STAT_FMT_DEBUG("过日!\n");
            }
            else
            {
                pastdayflag = 0;
                if(realstattime.hour != laststattime.hour)
                {
                    pasthourflag = 1;
                    pastminflag = 1;
                }
                else
                {
                    pasthourflag = 0;
                    if(realstattime.minute != laststattime.minute)
                        pastminflag = 1;
                    else
                        pastminflag = 0;
                }
            }
        }
    }
}

/**
*********************************************************************
* @brief：      统计间隔分析，统计周期处理函数提供统一的输入
* @param[in]： oad       待分析的OAD
* @param[out]：interval 分析出的间隔
* @return：     void
*********************************************************************
*/
void sta_interval_analyse(OOP_OAD_U oad, OOP_TI_SPAN_E *interval)
{

    if(oad.nObjID == STA_FROZEN_SEC)
    {
        *interval = TI_SEC; 
    }

    if((oad.nObjID == STA_FROZEN_MIN)||(oad.nObjID == STA_REGION_MIN)
        ||(oad.nObjID == STA_TOTALAVG_MIN)||(oad.nObjID == STA_EXTREME_MIN))
    {
        *interval = TI_MIN;    
    }

    if((oad.nObjID == STA_FROZEN_HOUR)||(oad.nObjID == STA_REGION_HOUR)
        ||(oad.nObjID == STA_TOTALAVG_HOUR)||(oad.nObjID == STA_EXTREME_HOUR))
    {
        *interval = TI_HOUR;    
    } 

    if((oad.nObjID == STA_FROZEN_DAY)||(oad.nObjID == STA_REGION_DAY)
        ||(oad.nObjID == STA_TOTALAVG_DAY)||(oad.nObjID == STA_EXTREME_DAY)
        ||(oad.nObjID == STA_FROZEN_SETTLING_DAY))
    {
        *interval =TI_DAY; 
    }

    if((oad.nObjID == STA_FROZEN_MON)||(oad.nObjID == STA_REGION_MON)
        ||(oad.nObjID == STA_TOTALAVG_MON)||(oad.nObjID == STA_EXTREME_MON))
    {
        *interval = TI_MON;    
    }

    if((oad.nObjID == STA_FROZEN_YEAR)||(oad.nObjID == STA_REGION_YEAR)
        ||(oad.nObjID == STA_TOTALAVG_YEAR)||(oad.nObjID == STA_EXTREME_YEAR))
    {
        *interval = TI_YEAR;    
    }      
}

/**
*********************************************************************
* @brief：      下一次统计时间刷新
* @param[in]： pTmpNode 冻结参数
* @param[out]：NA
* @return：     time_t 下一次冻结时间
*********************************************************************
*/
void sta_nexttime_update(OOP_TI_T ti, time_t lasttime, time_t *nextFrzTime)
{
    *nextFrzTime = lasttime;

    DT_TimeAddInv(nextFrzTime, ti);

    return;
}

/**
*********************************************************************
* @brief：      周期数更新
* @param[in]： interval 周期单位
* @param[in]： period   周期
* @param[out]：count     周期计数数
* @return：     void
*********************************************************************
*/
void sta_period_cnt_update(OOP_TI_SPAN_E interval, uint32 period, uint32 *count, uint32 *freqCnt)
{
    switch(interval)
    {
        case TI_SEC: break;
        case TI_MIN: if(pastminflag)  {(*count)++;}break;
        case TI_HOUR:if(pasthourflag) {(*count)++;}break;
        case TI_DAY: if(pastdayflag)  {(*count)++;}break;
        case TI_MON: if(pastmonflag)  {(*count)++;}break;
        case TI_YEAR:if(pastyearflag) {(*count)++;}break;
        default:break;
    }
    
    //达到周期则清零计数
    if(*count == period)
    {
        *count = 0;
        *freqCnt = 0;
    }
}

/**
*********************************************************************
* @brief：      时间间隔比较
* @param[in]： interval 周期单位
* @param[in]： period   周期
* @param[out]：count     周期计数数
* @return：     void
*********************************************************************
*/
BOOL sta_time_interval_equal(OOP_TI_T ti1, OOP_TI_T ti2)
{
    if((ti1.unit == ti2.unit) && (ti1.value == ti2.value))
    {
        return TRUE;
    }

    return FALSE;
}

/**
*********************************************************************
* @brief：      数值类型解析
* @param[in]： buffer  报文缓冲区
* @param[in]： offset  缓冲区中的偏移
* @param[out]: value   解析后的值
* @return：     void
*********************************************************************
*/
void sta_digit_value_parser(uint8 *buffer, uint32 *offset, OOP_DIGIT_T *value)
{
    value->choice = buffer[*offset++];

    switch(value->choice)
    {
        case DT_DOUBLE_LONG: //double long/int32
        {
            value->nVal = buffer[*offset];
            value->nVal = value->nVal<<8|buffer[*offset+1];
            value->nVal = value->nVal<<8|buffer[*offset+2];
            value->nVal = value->nVal<<8|buffer[*offset+3];            
            *offset += 4;              
        }break;
        case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
        {
            value->dwVal = buffer[*offset];
            value->dwVal = value->dwVal<<8|buffer[*offset+1];
            value->dwVal = value->dwVal<<8|buffer[*offset+2];
            value->dwVal = value->dwVal<<8|buffer[*offset+3];  
            *offset += 4;
        }break;     
        case DT_INTEGER: //integer/char
        {
            value->cVal = buffer[*offset++];
        }break;
        case DT_LONG: //long/int16
        {
            value->lVal = (int16)buffer[*offset]<<8 | buffer[*offset+1];
            *offset += 2;
        }break; 
        case DT_UNSIGNED: //unsigned/uint8
        {
            value->bVal = buffer[*offset++];
        }break;   
        case DT_LONG_UNSIGNED: //long unsigned/uint16
        {
            value->wVal = (uint16)buffer[*offset]<<8 | buffer[*offset+1];
            *offset += 2;            
        }break;   
        case DT_LONG64: //long64/int64
        {
            memcpy_rev(&value->llVal, &buffer[*offset], sizeof(int64));
            *offset += 8;            
        }break;   
        case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
        {
            memcpy_rev(&value->ullVal, &buffer[*offset], sizeof(uint64));
            *offset += 8;            
        }break;
        default:break;
    }    
}

/**
*********************************************************************
* @brief：     获取系统实时时间
* @param[in]： rectify 修正秒数
* @param[out]：pTime 系统时间
* @return：     void
*********************************************************************
*/
void sta_datetime_get(uint32 rectify, OOP_DATETIME_S_T *pTime)
{
    struct tm systime;
    struct  timeval tv;
    struct  timezone tz;
    gettimeofday(&tv, &tz);

    tv.tv_sec += rectify;
    FastSecondToDate(&tv, &tz, &systime);

    pTime->second = systime.tm_sec;
    pTime->minute = systime.tm_min;
    pTime->hour = systime.tm_hour;
    pTime->day = systime.tm_mday;            
    pTime->month = systime.tm_mon + 1;    
    pTime->year = systime.tm_year + 1900; 
}

/**
*********************************************************************
* @brief：      将当前时间转变为相对于2000年1月1日 00:00:00经过的秒数
* @param[in]： NA
* @param[out]：pTime 系统时间
* @return：     void
*********************************************************************
*/
//uint32 time_to_sec(time_struct *srcTime)
//{
//    const unsigned int MonTab[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
//    if(srcTime->month==0 || srcTime->day==0)
//        return 0;
//    uint32 time;
//    uint16 i;
//    
//    time = ((srcTime->year) / 4) * FOURYEARS + ((srcTime->year) % 4) * 365;     /*  days in normal years */
//    
//                                                                                /*  If we're past the leap year in the four-year block or if
//                                                                                *  we're past February then account for leap day
//    */
//    if (((srcTime->year) % 4) != 0 || (srcTime->month) > 2)
//        time++;
//    
//    /*(time) is the number of days to the beginning of this year */
//    for (i = 1; i < (srcTime->month); i++)
//        time += MonTab[i - 1];
//    
//    /*  (time) is the number of days to the beginning of this month*/
//    time += (srcTime->day) - 1;
//    
//    return ((time * 24 + (srcTime->hour)) * 60 + (srcTime->minute)) * 60 + (srcTime->second);
//}

/**
*********************************************************************
* @brief：      初始化上次采集时间
* @param[in]： lastTime
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_last_time_init(OOP_TI_T freq, time_t *lastTime)
{
    time_t curTime;
    DT_DateTimeShort2Time(&realstattime, &curTime);
    OOP_DATETIME_S_T begin={2000,1,1,0,0,0,0};//2000-01-01 00:00:00
    time_t beginTime;
    DT_DateTimeShort2Time(&begin,&beginTime);
    time_t tmpLastTime = 0;

    //周期为0，表示立即冻结，冻结时间就是当前时间
    if(freq.value == 0)
    {
        *lastTime = curTime;
    }
    else
    {
        /* 
         算法目的，计算距离下一个过秒，过分，过时，过日，过月，过年剩余的秒数,再减掉一个周期。
        */
        switch(freq.unit)
        {
            case TI_SEC:
                tmpLastTime = (curTime-beginTime)/freq.value;   //TODO 间隔可以为0
                tmpLastTime = beginTime+tmpLastTime*freq.value; //结果小与或等于curTime
                if(tmpLastTime==curTime)//相等时，推后一个间隔，后面计算的下次时间会等于curTime,立即进入统计
                {
                    tmpLastTime = tmpLastTime-freq.value;
                }
                *lastTime  = tmpLastTime;
                break;
            case TI_MIN:
                tmpLastTime = (curTime-beginTime)/(60*freq.value);
                tmpLastTime = beginTime+(tmpLastTime*(60*freq.value));
                if(tmpLastTime==curTime)
                {
                    tmpLastTime = tmpLastTime-60*freq.value;
                }
                *lastTime = tmpLastTime;
                break;
            case TI_HOUR:
                tmpLastTime = (curTime-beginTime)/(3600*freq.value);
                tmpLastTime = beginTime+(tmpLastTime*(3600*freq.value));
                if(tmpLastTime==curTime)
                {
                    tmpLastTime = tmpLastTime-3600*freq.value;
                }
                *lastTime = tmpLastTime;
                break;
            case TI_DAY:
                tmpLastTime = (curTime-beginTime)/(86400*freq.value);
                tmpLastTime = beginTime+(tmpLastTime*(86400*freq.value));
                if(tmpLastTime==curTime)
                {
                    tmpLastTime = tmpLastTime-86400*freq.value;
                }
                *lastTime = tmpLastTime;
                break;
            case TI_MON:
                while(beginTime<curTime)
                {
                    DT_TimeAddInv(&beginTime,freq);
                }
                DT_TimeSubInv(&beginTime,freq);
                *lastTime = beginTime;
                break;
            case TI_YEAR:
                while(beginTime<curTime)
                {
                    DT_TimeAddInv(&beginTime,freq);
                }
                DT_TimeSubInv(&beginTime,freq);
                *lastTime = beginTime;
                break;
            default:
                *lastTime = 0;
                break;
        }   
    }
    
    DT_Time2DateTimeShort(*lastTime, &begin);

//    STAT_FMT_DEBUG("init last time: %04d-%02d-%02d:%02d:%02d:%02d\n", 
//        begin.year,begin.month,begin.day,begin.hour,begin.minute,begin.second);

    return;
}

/**
*********************************************************************
* @brief：      设置时间更新标志
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_time_update_fset(BOOLEAN isUpdate)
{
    pthread_mutex_lock(&g_staUpdateData.mutex);

    g_staUpdateData.timeUpdate = isUpdate;
    
    pthread_mutex_unlock(&g_staUpdateData.mutex);    
}

/**
*********************************************************************
* @brief：      统计时间刷新
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_realtime_update(void)
{
    sta_datetime_get(0, &realstattime);
    
    memcpy(&g_currtime, &realstattime, sizeof(OOP_DATETIME_S_T));
    
    pthread_mutex_lock(&g_staUpdateData.mutex);

    if(g_staUpdateData.timeUpdate == TRUE)
    {
        g_staUpdateData.timeUpdate = FALSE;
            
        //对时后重新读取当前时间到全局变量
        sta_datetime_get(0, &realstattime);
        sta_frozen_lasttime_reinit();
        sta_region_lasttime_reinit();
        sta_totalavg_lasttime_reinit();
        sta_extreme_lasttime_reinit();  
    }
    
    pthread_mutex_unlock(&g_staUpdateData.mutex);    
}

/**
*********************************************************************
* @brief：      存储重启时间
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_restart_time_store(void)
{
    int ret = ERR_OK;
    OOP_DATETIME_S_T curTime = {0};

    sta_datetime_get(10, &curTime);

    ret = write_pdata_xram("statpdata",(char *)&curTime,sizeof(curTime),sizeof(curTime));

    STAT_FMT_DEBUG("system will be reboot, ret(%d), offset(%d), restart time(%4d-%02d-%02d %02d:%02d:%02d)\n", 
        ret, sizeof(curTime), curTime.year, curTime.month, curTime.day, 
        curTime.hour, curTime.minute, curTime.second);
}

#endif

#if DESC("直流模拟量处理",1)


/**********************************************************************
* @name      : wireless_uart_init
* @brief     ：初始化串口接口
* @param[in] ：char *device       设备id
* @param[out]：
* @return    ：串口句柄
* @Create    : 王津剑
* @Date      ：2020-3-13
* @Update    :
**********************************************************************/
void *sta_uart_init(char *device)
{
    UART_DEVICE_T* dev;
    dev = (UART_DEVICE_T*)hal_device_get(device);
    if(NULL == dev)
    {
        return NULL;    
    }
    
    STAT_FMT_DEBUG("sta_uart_init. dev: %s.\n", device);
    
    // 打开设备
    if(0 != dev->uart_param_set(dev, 115200, 8, 1, PARITY_NO))
    {
        STAT_FMT_DEBUG("设置串口%s属性失败\n", device);
        hal_device_release((HW_DEVICE *)dev);
        return NULL; 
    }
    return dev;
}

/**********************************************************************
* @name      : wireless_uart_uninit
* @brief     ：去初始化串口接口
* @param[in] ：void *pDev           串口句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-10
* @Update    :
**********************************************************************/
void sta_uart_uninit(void **pDev)
{
    if((NULL == pDev) || (NULL == *pDev))
    {
        return;
    }

    STAT_FMT_DEBUG("sta_uart_uninit. dev: %s%d_1\n",MOD_DEV_DESC_PRFIX, g_pt100_slot);
    
    hal_device_release((HW_DEVICE *)*pDev);

    *pDev = NULL;
}

/**********************************************************************
* @name      : wireless_uart_send
* @brief     ：串口数据接收接口
* @param[in] ：void *pDev                   串口句柄
               uint8* pBuffer               发送数据缓冲区
               int nBufLen                  发送长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-10
* @Update    :
**********************************************************************/
int sta_uart_send(void *pDev, uint8* pBuffer, int nBufLen)
{
    if(pDev == NULL)
    {
        STAT_FMT_DEBUG("sta_uart_send: pDev is NULL.\n");
        return ERR_PNULL;
    }

    UART_DEVICE_T* dev = (UART_DEVICE_T*)pDev;
    if(0 == dev->uart_data_send(dev, pBuffer, nBufLen))
    {
        return 0;
    }
    return -1;
}

/**********************************************************************
* @name      : wireless_uart_recv
* @brief     ：串口数据接收接口
* @param[in] ：void *pDev                   串口句柄
               int nBufLen                  最大接收长度
               int rsptime                  数据返回时间(20ms)
* @param[out]：
* @return    ：接收到数据的长度
* @Create    : 王津剑
* @Date      ：2020-3-13
* @Update    :
**********************************************************************/
int sta_uart_recv(void *pDev, uint8* pBuffer, int nBufLen)
{
    if(pDev == NULL)
    {
        STAT_FMT_DEBUG("sta_uart_recv: pDev is NULL.\n");
        return 0;
    }
    
    UART_DEVICE_T* dev = (UART_DEVICE_T*)pDev;
    return dev->uart_data_recv(dev, pBuffer, nBufLen);
}

/**********************************************************************
* @name      : proxy_apply_ack_set
* @brief     ：设置通信代理的应用连接状态通知确认
* @param[in] ：ack   确认消息
* @return    ：返回消息长度
* @Create    : 贺宁
* @Date      ：2020-04-20
* @Update    :
**********************************************************************/
void sta_mod_event(TASK_MSG_t *pMsg)
{
    uint16 msglen = pMsg->msglen;
    uint8 *revcbuf = pMsg->msg;
    uint8  offset = 0;
    int slot = 0;
    uint8 status = 0;

    if(msglen != 0)
    {
        memcpy_r(&slot, &revcbuf[offset], sizeof(int));
        offset += sizeof(int);  

        memcpy(&status, &revcbuf[offset], sizeof(uint8));
        offset += sizeof(uint8);  

        if(offset > msglen)
        {
            STAT_FMT_DEBUG("module plug/uplug msg: offset(%d) > msglen(%d).\n", offset, msglen);
            return;
        }

        //模块已经拔出，再检测到插入模块
        if((g_pt100_slot == 0) &&(status == 0))
        {
            STAT_FMT_DEBUG("module plug notify: slot(%d)\n", slot);
            g_mod_status_chg = TRUE;  
        }
        else if((status == 1) && (g_pt100_slot == slot)) //拔出目标模块，则清零槽位和状态，待查询到新信息
        {
            g_pt100_slot = 0;
            g_mod_status_chg = TRUE;
            STAT_FMT_DEBUG("module unplug: slot(%d)\n", slot);
        }
    }
    
    return;
}

/**********************************************************************
* @name      : sta_get_module_slot
* @brief     ：组织获取模组槽位消息
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-06-18
* @Update    :
**********************************************************************/
int sta_get_pt100_slot(int *slots, int mNum)
{
    uint8 revcbuf[128] = {0};
    uint8 num = 0;
    uint16 msglen = 0;
    uint8  offset = 0;
    int i = 0;
    
    msglen =  appmain_get_map_slot(MOD_PT100_TYPE_PRFIX, revcbuf, sizeof(revcbuf));
    if(msglen != 0)
    {
        num = revcbuf[offset++];
        
        if(num != 0)
        {
            for(i = 0; i< mNum && i < num; i++)
            {
                memcpy_r(&slots[i], &revcbuf[offset], sizeof(int));
                offset += sizeof(int);

                STAT_FMT_DEBUG("slot%d for \"%s\".\n", slots[i], MOD_PT100_TYPE_PRFIX);
            }
        }
    }
    
    return (num > mNum ? mNum : num);
}

/**********************************************************************
* @name      : sta_get_module_info
* @brief     ：组织获取模组信息
* @param[in] ：slot 槽位号，从左到右依次为1，2，3，4，5,其中0-表示所有槽位
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-06-18
* @Update    :
**********************************************************************/
int sta_get_pt100_info(void)
{
    int slot = 0;
    int num = 0;
    //uint8 buf[320] = {0};
    //uint16 msglen = 0;
    //MOD_INFO_ACK_T tModeInfo;

    //MEMZERO(&tModeInfo, sizeof(MOD_INFO_ACK_T));

    num = sta_get_pt100_slot(&slot, 1);
    
    if(num > 0)
    {
        /*msglen = appmain_get_map_info(slot, buf, 320);
        if(0 != msglen)
        {
            appmsg_modinfo_split(buf, &tModeInfo);
            
            if(tModeInfo.num < 2)
            {
                STAT_FMT_DEBUG("no channel for pt100 slot%d.\n", slot);
                return ERR_FAILPORT;
            }
        }*/
    }
    else
    {   g_pt100_slot = 0;
        return ERR_NOTEXIST;
    }

    g_pt100_slot = slot;

    return ERR_OK;
}

/**********************************************************************
* @name      : pt100_make_frame
* @brief     ：组装PT100查询报文
* @param[in] ：uint16 dt 
               uint16 buflen   报文最大长度
* @param[out]：uint8 *sendBuf  报文内容
* @return    ：报文长度
* @Create    : 贺宁
* @Date      ：2020-06-18
**********************************************************************/
uint16 pt100_make_frame(uint16 dt, uint8 *sendBuf, uint16 buflen)
{
    uint8          msgbuf[128] = {0};
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    static uint8 index = 0;

    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index++;
    
    Temp[offset++] = MAPMSG_APDU_GetRequest;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);

    pMsgInfo->len = offset;

    return mapmsg_package(pMsgInfo, sendBuf, buflen);
}

/**********************************************************************
* @name      : pt100_data_store
* @brief     ：组装PT100查询报文
* @param[in] ：uint16 recvBuf  接收的报文
               uint16 buflen   报文最大长度
* @return    ：报文长度
* @Create    : 贺宁
* @Date      ：2020-06-18
**********************************************************************/
int pt100_data_store(uint8 *recvBuf, uint16 buflen)
{
    int            ret = 0;
    uint16         offset = 0;
    MAPMSG_INFO_T *pMsgInfo = NULL;
    uint8         *temp = NULL;
    uint16         dt = 0;
    uint8          i = 0;
    int            tempret = 0;
    STA_DCANALOGS_T analog = {0};
    uint16 sOffset = 0;
    OOP_ANALOG_T data = {0};
    OOP_ANALOG_CONFIG_T config = {0};
    NOMAL_OAD_T nomalOAD = {0};
    uint32 len = 0;

    nomalOAD.oad.value = 0xf2040400;
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(config), (uint8*)&config, &len);
    if(ret != 0)
    {
        STAT_FMT_TRACE("dc data read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                    ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(config));
        //return ret;
    }

    //找到帧起始位
    while(offset < buflen)
    {
        if(FRAMESTART == recvBuf[offset])
        {
            tempret = mapmsg_split(recvBuf + offset, buflen - offset, &pMsgInfo);
            if(tempret != 0)
            {
                STAT_FMT_DEBUG("pt100_data_store() mapmsg_split() tempret=%d \n", tempret);
            }
            break;
        }
        offset++;
    }

    if(pMsgInfo == NULL)
    {
        STAT_BUF_DEBUG(temp, sOffset, "check null pointer.\n");
        return ERR_PNULL;
    }

    //pt100功能单一，不考虑多帧
    if(MAPMSG_CTR_MSG == pMsgInfo->cmt.cmd && pMsgInfo->len > 4)
    {
        temp = pMsgInfo->apdu;


        if((MAPMSG_APDU_GetResponse == temp[0])&&(1 == temp[3]))  //data [1]
        {
            sOffset += 2;

            memcpy_r(&dt, &temp[1], sizeof(uint16));
            sOffset += sizeof(uint16);
            
            switch(dt)
            {
                case MAPMSG_DT_ANALOG_GET:
                {
                    if(temp[sOffset++] != 1) //非array
                    {
                        STAT_BUF_DEBUG(temp, sOffset, "check array type failed.\n");
                        return ERR_TYPE;
                    }

                    analog.nNum = temp[sOffset++];

                    if(analog.nNum > OOP_MAX_DCANALOG)
                    {
                        STAT_BUF_DEBUG(temp, sOffset, "analog.nNum(%d) > OOP_MAX_DCANALOG(%d).\n", analog.nNum, OOP_MAX_DCANALOG);
                        return ERR_RANGE;
                    }

                    for(i = 0; i < analog.nNum; i++)
                    {
                        if(temp[sOffset++] != 2) //非struct
                        {
                            STAT_BUF_DEBUG(temp, sOffset, "check struct type failed.\n");
                            return ERR_TYPE;
                        }

                        if(temp[sOffset++] != 2) //成员数
                        {
                            STAT_BUF_DEBUG(temp, sOffset, "check struct num failed.\n");
                            return ERR_TYPE;
                        }

                        sOffset++; //double long类型

                        memcpy_r(&analog.data[i].value, &temp[sOffset], sizeof(int));
                        sOffset += sizeof(int);

                        sOffset++; //scaler_unit类型

                        analog.data[i].scaler.scaler = temp[sOffset++];
                        
                        analog.data[i].scaler.unit = temp[sOffset++];

                    }

                    data.nNum = analog.nNum;
                    for(i = 0; i < analog.nNum; i++)
                    {
                        data.value[i] = analog.data[i].value;
                    }

                    nomalOAD.classtag = CLASS_DATA_NORMAL;
                    nomalOAD.oad.value = 0xf2040200;
                    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&data, sizeof(data));
                    if(ret != 0)
                    {
                        STAT_FMT_DEBUG("pt100_data_store failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                        ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));

                        return ret;
                    }
                }break;
                
                default: ret = ERR_OPTION; break;
            }
        }
    }
    
    FREE_S(pMsgInfo);
    
    return ret;
}

/**
*********************************************************************
* @brief：     直流模拟量采集初始化
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_dc_analog_init(void)
{
#ifdef PRODUCT_ECU
    sta_get_pt100_info();
#endif
}

/**
*********************************************************************
* @brief：     直流模拟量采集初
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_dc_analog_collect(void)
{
    char dev[32] = {0};  /* 模块所用串口描述符 */
    static void *pUart = NULL; /*串口句柄*/
    static uint8 retry = 0;
    uint8 sendbuf[128] = {0};
    uint8 recvbuf[128] = {0};
    int msglen = 0;
    int ret = 0;

    switch(g_dc_colstatus)
    {
        case STA_DC_INIT: 
        {
            STAT_FMT_TRACE("STA_DC_INIT.\n");
            if(g_pt100_slot != 0)
            {
                sta_uart_uninit(&pUart);
                g_dc_colstatus = STA_DC_SEND;
            }
            else
            {
                //收到模块插拔事件
                if(g_mod_status_chg)
                {
                    g_mod_status_chg = FALSE;
                    g_dc_colstatus = STA_DC_GETM;
                }
            }
        }break;

        case STA_DC_SEND:
        {
            STAT_FMT_TRACE("STA_DC_SEND.\n");
            if(g_mod_status_chg)
            {
                g_mod_status_chg = FALSE;
                g_dc_colstatus = STA_DC_GETM;
                break;
            }

            if(pUart == NULL)
            {
                sprintf(dev, "%s%d_1",MOD_DEV_DESC_PRFIX, g_pt100_slot);
                pUart = sta_uart_init(dev); /*打开串口*/
                if(pUart == NULL)
                {
                    STAT_FMT_DEBUG("sta_uart_init failed dev: %s.\n", dev);
                    g_dc_colstatus = STA_DC_GETM;
                    break;
                }
            }
            
            msglen = pt100_make_frame(0x8000, sendbuf, sizeof(sendbuf));
            ret = sta_uart_send(pUart, sendbuf, msglen);
            STAT_BUF_TRACE(sendbuf, msglen, "sta_uart_send.\n");
            if(ret != 0)
            {
                if(++retry > 10)
                {
                    g_pt100_slot = 0;

                    if(sta_get_pt100_info() != ERR_OK)
                    {
                    
                        sta_uart_uninit(&pUart);
                        g_dc_colstatus = STA_DC_INIT;
                        retry = 0;
                    }
                }
            }
            else
            {
                g_dc_colstatus = STA_DC_RECV;
            }

        }break;

        case STA_DC_RECV: 
        {
            STAT_FMT_TRACE("STA_DC_RECV.\n");
            if(g_mod_status_chg)
            {
                g_mod_status_chg = FALSE;
                g_dc_colstatus = STA_DC_GETM;
                break;
            }
                    
            msglen = sta_uart_recv(pUart, recvbuf, sizeof(recvbuf));
            STAT_BUF_TRACE(recvbuf, msglen, "sta_uart_recv.\n");
            
            if(msglen > 0)
            {
                ret = pt100_data_store(recvbuf, msglen);
                if(ret != 0)
                {
                    STAT_BUF_DEBUG(recvbuf, msglen, "pt100_data_store failed,ret(%d).\n", ret);
                }
            }

            g_dc_colstatus = STA_DC_SEND;   

        }break;

        case STA_DC_GETM:
        {
            STAT_FMT_TRACE("STA_DC_GETM.\n");
            //过秒执行，1s查一次，查10次
            if(retry++ < 10 )
            {
                if(sta_get_pt100_info()== ERR_OK)
                {
                    sta_uart_uninit(&pUart);
                    g_dc_colstatus = STA_DC_SEND;
                }
            }
            else
            {
                g_dc_colstatus = STA_DC_INIT;
                retry = 0;
            }
        }break;

        default: g_dc_colstatus = STA_DC_INIT; break;
    }
}

#endif

#if DESC("经典统计处理函数",1)

/**
*********************************************************************
* @brief：     安全防护参数初始化
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_secure_defend(void)
{
    int ret  = ERR_OK; 
    int wret  = ERR_OK;  
    NOMAL_OAD_T normal = {0};
    OOP_PORTS_T data;
    OOP_SERVICES_T serData;
    uint32 len = 0;

    MEMZERO(&serData, sizeof(serData));   
    MEMZERO(&data, sizeof(data));    

    //读取服务授权参数
    normal.classtag = CLASS_DATA_UNINIT;
    normal.oad.value = 0x43000B00;

    //主站没下发授权前，SSH默认永久授权
    ret = db_read_nomal(g_staUdpHandle, &normal, sizeof(serData), (uint8*)&serData, &len);
    if(ret != 0)
    {
        serData.nNum = 1;
        serData.item[0].type = 1;
        serData.item[0].times = 255;
        
        wret = db_write_nomal(g_staUdpHandle, &normal, (uint8*)&serData, sizeof(serData));
        if(ret != ERR_OK)
        {
            AUTH_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                wret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(serData));
            return;
        }

        AUTH_FMT_DEBUG("ret %d secure defend default: SSH service is permanent authorized.\n",ret);
    }
    
    g_portSSH.authTime = serData.item[0].times;
    g_portSSH.status = AUTH_STATUS_GET(g_portSSH.authTime);


    //读取232端口授权参数
    normal.oad.value = 0xF2000500;

    //主站没下发授权前，默认都不授权
    ret = db_read_nomal(g_staUdpHandle, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != 0)
    {
        wret = db_write_nomal(g_staUdpHandle, &normal, (uint8*)&data, sizeof(data));
        if(wret != ERR_OK)
        {
            AUTH_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                wret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(data));
            return;
        }

        AUTH_FMT_DEBUG("ret %d secure defend default: RS232 ports are forbidden.\n",ret);
    }

    g_portRS232.authTime = data.item[0];
    g_portRS232.status = AUTH_STATUS_GET(g_portRS232.authTime);
    
    MEMZERO(&data, sizeof(data));    

    //读取485端口授权参数
    normal.oad.value = 0xF2010500;

    //主站没下发授权前，默认都不授权
    ret = db_read_nomal(g_staUdpHandle, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != 0)
    {
        data.nNum = 1;
        data.item[0] = 255;
        
        wret = db_write_nomal(g_staUdpHandle, &normal, (uint8*)&data, sizeof(data));
        if(wret != ERR_OK)
        {
            AUTH_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                wret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(data));
            return;
        } 

        AUTH_FMT_DEBUG("ret %d secure defend default: RS485 ports are permanent authorized.\n", ret);
    }

    g_portRS485.authTime = data.item[0];
    g_portRS485.status = AUTH_STATUS_GET(g_portRS485.authTime); 
    
    MEMZERO(&data, sizeof(data));    

    //读取红外端口授权参数
    normal.oad.value = 0xF2020500;

    //主站没下发授权前，默认都不授权
    ret = db_read_nomal(g_staUdpHandle, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != 0)
    {
        wret = db_write_nomal(g_staUdpHandle, &normal, (uint8*)&data, sizeof(data));
        if(wret != ERR_OK)
        {
            STAT_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                wret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(data));
            return;
        }

        AUTH_FMT_DEBUG("ret %d secure defend default: IR ports are forbidden.\n",ret);
    }    

    g_portIR.authTime = data.item[0];
    g_portIR.status = AUTH_STATUS_GET(g_portIR.authTime); 

    MEMZERO(&data, sizeof(data));    

    //读取USB端口授权参数
    normal.oad.value = 0xF20E0500;

    //主站没下发授权前，默认都不授权
    ret = db_read_nomal(g_staUdpHandle, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != 0)
    {
        wret = db_write_nomal(g_staUdpHandle, &normal, (uint8*)&data, sizeof(data));
        if(wret != ERR_OK)
        {
            STAT_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                wret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(data));
            return;
        } 

        AUTH_FMT_DEBUG("ret %d secure defend default: USB ports are forbidden.\n",ret);
    }    

    g_portUSB.authTime = data.item[0];
    g_portUSB.status = AUTH_STATUS_GET(g_portUSB.authTime); 

    return;
}

/**
*********************************************************************
* @brief：     注册私有数据区，用于存储统计的中间结果
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_private_register(void)
{
    int iret = 0;
    uint16 i;
    char pstring[200] = {"\0"};
    char bakpstring[205] = {"\0"};

    //注册一个与测量点无关的私有文件
    if((iret = reg_pdata_xram(ADV_APP_NAME, "statpdata")) < 0)
    {
        STAT_FMT_DEBUG("register private file failed. ret(%d)",iret);
    } 

    //注册交采测量点的私有文件
    if((iret = reg_pdata_xram(ADV_APP_NAME, "statacsample")) < 0)
    {
        STAT_FMT_DEBUG("register private file for ac sample failed. ret(%d) pluse(%d)",iret);
    } 

    if((iret = reg_pdata_xram(ADV_APP_NAME, "bakstatacsample")) < 0)
    {
        STAT_FMT_DEBUG("register private file for ac sample failed. ret(%d) pluse(%d)",iret);
    }

    //注册存储脉冲计量点的信息0x2401~0x2408为脉冲OI，取低4位1~8来访问私有文件
    for(i = 1; i < MTRPLS_MAX_NUM+1; i++)
    {
        memset(pstring, 0, sizeof(pstring));
        sprintf(pstring,"statpluse%d",i);
        if((iret = reg_pdata_xram(ADV_APP_NAME, pstring)) < 0)
        {
            STAT_FMT_DEBUG("register private file for pluse failed. ret(%d) pluse(%d)",iret, i);
        }

        memset(bakpstring, 0, sizeof(bakpstring));
        sprintf(bakpstring, "bak%s", pstring);
        if((iret = reg_pdata_xram(ADV_APP_NAME, bakpstring)) < 0)
        {
            STAT_FMT_DEBUG("register private file for pluse failed. ret(%d) pluse(%d)",iret, i);
        }
    }

    //485表用配置序号来关联私有文件，配置序号1~64
    for(i = 1; i < MTR485_MAX_NUM+1; i++)
    {
        memset(pstring, 0, sizeof(pstring));
        sprintf(pstring,"statmeter%d",i);
        if((iret = reg_pdata_xram(ADV_APP_NAME, pstring)) < 0)
        {
            STAT_FMT_DEBUG("register private file for meter failed. ret(%d) nIndex(%d)",iret, i);
        }    

        memset(bakpstring, 0, sizeof(bakpstring));
        sprintf(bakpstring, "bak%s", pstring);
        if((iret = reg_pdata_xram(ADV_APP_NAME, bakpstring)) < 0)
        {
            STAT_FMT_DEBUG("register private file for meter failed. ret(%d) nIndex(%d)",iret, i);
        }
    }

    return;
}

/**
*********************************************************************
* @brief：      经典统计的初始化
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_classical_init(void)
{
    if(app_secure_defend_isuse())  
    {
        sta_secure_defend();
    }
    
    /*私有存储空间申请*/
    sta_private_register();
    sta_dc_analog_init();
    sta_datetime_get(0, &g_lasttime);
    frozen_ac_objattr_add(g_staUdpHandle);

    return;
}

/**
*********************************************************************
* @brief：      经典统计处理
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_classical_stat(void)
{
    STAT_FMT_TRACE("---------classical stat start--------\n");

    //终端累积供电时间
    sta_tmn_power_acctime();
    
    //终端累积复位次数
    sta_tmn_reset_acctimes();
    
    //终端累积通信流量
    sta_tmn_comm_traffic();

    sleep(1);
    
    if ((realstattime.minute)%30 == 0)  //半个小时写一次
    {
        STAT_FMT_DEBUG("stat now is %04d-%02d-%02d %02d:%02d:%02d\n", realstattime.year, realstattime.month, realstattime.day,
            realstattime.hour, realstattime.minute, realstattime.second);
    }
    
    if(app_secure_defend_isuse())  
    {
        //端口授权计时
        sta_tmn_port_authorize_tmcnt();
    }

    //计算日，月电压合格率
    sta_volt_qualifyrate_calc();

    //电表电压合格率统计
    sta_qr_entry(g_staUdpHandle);
    
    //计算电流，电压不平衡率
    //sta_curr_imbalance_calc();
    //sta_volt_imbalance_calc();

    //分钟电量计算，为总加组和脉冲电量提供数据
    sta_mpoint_entry();

    //总加组相关计算
    sta_tgsets_data_calc();

    //脉冲电量计算
    sta_pulse_energy_calc();

    usleep(200000);

    //终端累积供电时间
    //sta_tmn_power_acctime();
    
    //终端累积复位次数
    //sta_tmn_reset_acctimes();
    
    //终端累积通信流量
    //sta_tmn_comm_traffic();
    STAT_FMT_TRACE("---------classical stat end--------\n");
}

#endif

#if DESC("冻结处理函数",1)


//清空冻结运行参数列表
FrzRunParaList_t* sta_frozen_paralist_clear(FrzRunParaList_t* pFrzRunParaListHead)
{
    FrzRunParaList_t *pTmpNode;//临时节点

    while(pFrzRunParaListHead!=NULL)
    {
        pTmpNode = pFrzRunParaListHead->pNext;
        if(pFrzRunParaListHead->frzRunPara.pColOad!=NULL)
            free(pFrzRunParaListHead->frzRunPara.pColOad);
        free(pFrzRunParaListHead);
        pFrzRunParaListHead = pTmpNode;
    }
    return NULL;
}

//增加关联对象
FrzRunParaList_t* sta_frozen_paranode_add(FrzRunParaList_t* pFrzRunParaListHead, OOP_FROZENOBJ_T incOadInfo)
{
    uint16 i;
    FrzRunParaList_t *pTmpNode,*pCurNode;//临时节点
    OOP_OAD_U *pColOad;
    pTmpNode = pFrzRunParaListHead;
    pCurNode = pFrzRunParaListHead;
    while(pTmpNode != NULL)
    {
        pCurNode = pTmpNode;
        if(pTmpNode->frzRunPara.period == incOadInfo.nPeriod
            && pTmpNode->frzRunPara.storeDepth == incOadInfo.nDepth)
        {
            if(pTmpNode->frzRunPara.pColOad == NULL)
                pTmpNode->frzRunPara.oadNum = 0;
            for(i=0;i<pTmpNode->frzRunPara.oadNum;i++)
            {
                if(pTmpNode->frzRunPara.pColOad[i].nObjID == incOadInfo.oad.nObjID
                    &&pTmpNode->frzRunPara.pColOad[i].attID == incOadInfo.oad.attID
                    &&pTmpNode->frzRunPara.pColOad[i].nIndex == incOadInfo.oad.nIndex)
                {
                    break;
                }
            }
            if(i>=pTmpNode->frzRunPara.oadNum)
            {
                if(i==0) 
                {
                    STAT_FMT_DEBUG("frozen_paranode_add：not possible.\n");
                    i=1;
                }
                
                pColOad = (OOP_OAD_U*)malloc((i+1)*sizeof(OOP_OAD_U));
                memcpy((uint8*)pColOad,(uint8*)pTmpNode->frzRunPara.pColOad,(i)*sizeof(OOP_OAD_U));
                pColOad[i] = incOadInfo.oad;
                free(pTmpNode->frzRunPara.pColOad);
                pTmpNode->frzRunPara.pColOad = pColOad; 
                pTmpNode->frzRunPara.oadNum++; 
            }
            break;
        }
        pTmpNode = pTmpNode->pNext;
    }
    //新增结点并添加到链接尾部
    if(pTmpNode==NULL)
    {
        pTmpNode = (FrzRunParaList_t*)malloc(sizeof(FrzRunParaList_t));
        memset((uint8*)pTmpNode,0x00,sizeof(FrzRunParaList_t));
        pTmpNode->frzRunPara.period = incOadInfo.nPeriod;
        pTmpNode->frzRunPara.storeDepth = incOadInfo.nDepth;
        pTmpNode->frzRunPara.oadNum = 1;
        pTmpNode->frzRunPara.pColOad = (OOP_OAD_U*)malloc(sizeof(OOP_OAD_U));
        pTmpNode->frzRunPara.pColOad[0] = incOadInfo.oad;

        if(pCurNode == NULL)
        {
            return pTmpNode;
        }
        else 
        {
            pCurNode->pNext = pTmpNode;
        }        
    }

    return pFrzRunParaListHead;
}

/**
*********************************************************************
* @brief：      从数据中心获取冻结方案
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
int sta_frozen_scheme_get(const OOP_OAD_U mainOad, OOP_FROZENSETS_T *frzPara)
{
    NOMAL_OAD_T nomalOAD;
    uint32 len = 0;

    MEMZERO(&nomalOAD, sizeof(NOMAL_OAD_T));

    //关联对象属性表
    nomalOAD.oad.value = mainOad.value + 0x300;
    
    return db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_FROZENSETS_T), (uint8*)frzPara, &len);
}

//根据冻结对象标识初始化冻结运行参数
FrzRunParaList_t* sta_frozen_paralist_init(OOP_OAD_U FrzOAD,FrzRunParaList_t* pFrzRunParaListHead)
{
    OOP_TI_SPAN_E time_span;
    OOP_TI_T tmpTI;    
    FrzRunParaList_t *pTmpNode;//临时节点
    pFrzRunParaListHead = sta_frozen_paralist_clear(pFrzRunParaListHead);//清空链表
    OOP_FROZENSETS_T frozensets;
    uint16 i = 0;
    memset(&frozensets, 0x00, sizeof(OOP_FROZENSETS_T));

    /*从数据中心读取配置*/
    sta_frozen_scheme_get(FrzOAD,&frozensets);

    STAT_FMT_DEBUG("frzOAD(0x%08x), nObjs(%d)\n", FrzOAD.value, frozensets.nNum);
    
    //初始化关联属性表
    for(i = 0; i < frozensets.nNum; i++)
    {
        STAT_FMT_DEBUG("oad(0x%08x), nPeriod(%d), nDepth(%d)\n",
            frozensets.objs[i].oad.value, frozensets.objs[i].nPeriod,frozensets.objs[i].nDepth);
        pFrzRunParaListHead = sta_frozen_paranode_add(pFrzRunParaListHead,frozensets.objs[i]);
    }
    
    //初始化上一次冻结时间
    //先按照停电不补冻结设计    
    pTmpNode = pFrzRunParaListHead;
    sta_interval_analyse(FrzOAD, &time_span);

    while(pTmpNode!=NULL)
    {
        //需要用这个OAD去写记录表
        pTmpNode->frzOAD.value = FrzOAD.value + 0x200;
        
        tmpTI.unit = time_span;
        tmpTI.value = pTmpNode->frzRunPara.period;
        
        sta_last_time_init(tmpTI,&pTmpNode->frzRunPara.lastFrzTm);
        
        pTmpNode = pTmpNode->pNext;
    }
    
    return pFrzRunParaListHead;
}

/**
*********************************************************************
* @brief：      冻结功能处理
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_frozen_init()
{
    uint8 i;
    OOP_OAD_U frzOad = {0x50000000};
    uint8 frzInitFlg=0;

    pthread_mutex_lock(&g_staUpdateData.mutex);
    
    /*冻结参数改变，获取冻结运行参数*/
    frzInitFlg = g_staUpdateData.frzInitFlag;
    
    /*遍历每一个节点*/
    for(i=0; i<STA_FROZEN_OBJNUMS; i++)
    {
        if(frzInitFlg&(1UL<<i))
        {
            frzOad.nObjID = STA_FROZEN_TRAN + i;

            //TODO 后续改为数组，链表的时候OI动态写，过滤终端不需要的OI
            g_staFrozenParaList[i] = sta_frozen_paralist_init(frzOad, g_staFrozenParaList[i]);

            frzInitFlg &= ~(1UL<<i);

            g_frozen_index[i] = 0; //冻结记录序号，每次冻结参数变更，初始化为0
        }
    }
    
    g_staUpdateData.frzInitFlag = frzInitFlg;

    pthread_mutex_unlock(&g_staUpdateData.mutex);    

    return;
}

BOOL JudgeDbData(void)
{
	int ret = ERR_OK;
	NOMAL_OAD_T nomalOAD = {0};
	uint32 realLen;
	OOP_ENERGY_T enger={0};
	nomalOAD.logicId = 1; //交采
	nomalOAD.oad.value =0x00100200;

    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(enger), (uint8*)&enger, &realLen);
	if((ret !=ERR_OK)||(realLen !=sizeof(enger)))
	{
		return FALSE;
	}
	FREEZE_BUF_DEBUG((uint8*)&enger, realLen,"nomalOAD.oad.value is %x,ret is %d\n",nomalOAD.oad.value,ret);
	return TRUE;
}

void WaitDbData()
{
	static uint32 begintime;

	begintime = time(NULL);
	while(1)
	{
		if((TRUE==JudgeDbData())||((time(NULL) -begintime)>30))
		{
			return;
		}
	}
}
/**
*********************************************************************
* @brief：      读取冻结列对象数据
* @param[in]： ColOAD 冻结关联对象OAD
* @param[out]：outData 输出缓冲区
* @param[in]： outLen  输出缓大小
* @return：     真实数据长度
*********************************************************************
*/
int sta_frozen_objdata_read(OOP_OAD_U ColOAD, uint8 *outData, uint32 outLen, uint32 *realLen,OOP_OAD_U frzOAD)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};   
    CHANNEL_CACHE_T tChannel;
    uint8 tmpPdu[4] = {0};

    //块数据直接读，分量数据走规约接口,如果规约完善了非解析读，可以都从规约读
    if(ColOAD.nIndex == 0)
    {
        nomalOAD.logicId = 0; //终端
        nomalOAD.oad.value = ColOAD.value;
        
        ret = db_read_nomal(g_staUdpHandle, &nomalOAD, outLen, outData, realLen);
		STAT_BUF_TRACE(outData, MIN(outLen, *realLen), "ColOAD.value is %x,ret IS %d\n",ColOAD.value,ret);
        if(ret != 0)
        { 
            nomalOAD.logicId = 1; //交采

            ret = db_read_nomal(g_staUdpHandle, &nomalOAD, outLen, outData, realLen);
			if(frzOAD.nObjID == 0x5004)
			{
				FREEZE_BUF_DEBUG(outData, MIN(outLen, *realLen), "ColOAD.value is %x,ret IS %d\n",ColOAD.value,ret);
			}
            if(ret != 0)
            {
                STAT_FMT_TRACE("frozen data read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                    ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, outLen);
                
                return ret;
            }            
        }
    }
    else
    {
        memcpy_r(tmpPdu, &ColOAD, sizeof(ColOAD));
        
        tChannel.clientid = g_staUdpHandle;
        tChannel.pApduBreak = app_apdubreak_init();
        tChannel.private[0] = 1; //逻辑设备1
        tChannel.private[3] = TRUE; //只返回读到的数据，不解析成报文
        
        *realLen = invoke_get_req(&tChannel, tmpPdu, 4, outData, outLen);
        if(*realLen == 0)
        {
            tChannel.private[0] = 0; //逻辑设备0
            *realLen = invoke_get_req(&tChannel, tmpPdu, 4, outData, outLen);
            if(*realLen == 0)
            {
                STAT_FMT_DEBUG("frozen data read failed.ret(%d), logicId(%d), oad(0x%08x), inMaxLen(%d)\n",
                    ret, tChannel.private[0], ColOAD.value, outLen);
            }
        }

        app_apdubreak_free(&tChannel.pApduBreak);
    }

    return ret;
}

/**
*********************************************************************
* @brief：      写一条冻结记录
* @param[in]： ColOAD 冻结关联对象OAD
* @param[out]：outData 输出缓冲区
* @param[in]： outLen  输出缓大小
* @return：     真实数据长度
*********************************************************************
*/
uint16 sta_frozen_record_write(const OOP_OAD_U mainOad, OOP_FROZENOBJ_T frzObjs, BOOLEAN bStart, time_t storeTime, const uint8 *dataBuf, const uint32 dataLen)
{
    int ret = ERR_OK;
    WRITE_RECORD_T inData;
    uint32 storeNo = 0;
    OOP_DATETIME_S_T tmStore = {0};
    uint8 *pAddr = NULL;

    MEMZERO(&inData, sizeof(WRITE_RECORD_T));
    
    //获取终端地址
    app_get_localaddr(&pAddr);

    // 校验码，通信地址的前6个字节
//    inData.MAC.nNum = 6;
//    memcpy(inData.MAC.value, pAddr, 6);

    // 注：+8个小时(28800)转成格林时间
    inData.colStartTm = time(NULL)+ COMPENSATION_TIME;
	inData.colEndTm = time(NULL)+ COMPENSATION_TIME;
    inData.colStoreTm =  storeTime + COMPENSATION_TIME;
    inData.recordOAD.classtag = CLASS_DATA_NORMAL;
    inData.recordOAD.road.oadMain.value = mainOad.value;
    inData.recordOAD.road.oadCols.nNum = 1;

    //如果是第一个，那么把冻结序号和冻结时间也作为关联数据写入
    if(bStart == TRUE)
    {
        STAT_FMT_TRACE("tmn frozen record write: mainOad = %lu(0x%08x)\n", 
            inData.recordOAD.road.oadMain.value, inData.recordOAD.road.oadMain.value);
        
        //冻结记录序号
        inData.recordOAD.road.oadCols.oad[0].value = 0x20230200;
        inData.inDatalen = 4;
        memcpy(inData.inDataBuf, &g_frozen_index[mainOad.nObjID-STA_FROZEN_TRAN], inData.inDatalen);
        g_frozen_index[mainOad.nObjID-STA_FROZEN_TRAN]++;
        
        ret = db_write_record(g_staUdpHandle, &inData, &storeNo);
        if(ret != ERR_OK)
        {
            return ret;
        }
        
        STAT_FMT_TRACE("tmn frozen record write: oadCols.oad[0] = %lu(0x%08x)\n", 
            inData.recordOAD.road.oadCols.oad[0].value, inData.recordOAD.road.oadCols.oad[0].value);

        //数据冻结时间
        DT_Time2DateTimeShort(storeTime, &tmStore);
        inData.recordOAD.road.oadCols.oad[0].value = 0x20210200;
        inData.inDatalen = sizeof(OOP_DATETIME_S_T);
        memcpy(inData.inDataBuf, &tmStore, inData.inDatalen);
        
        ret = db_write_record(g_staUdpHandle, &inData, &storeNo); 
        if(ret != ERR_OK)
        {
            return ret;
        }
        
        STAT_FMT_TRACE("tmn frozen record write: oadCols.oad[0] = %lu(0x%08x)\n", 
            inData.recordOAD.road.oadCols.oad[0].value, inData.recordOAD.road.oadCols.oad[0].value);
    }
    
    inData.inDatalen = dataLen;
    memcpy(inData.inDataBuf, dataBuf, dataLen);

    inData.recordOAD.road.oadCols.nNum = 1;
    inData.recordOAD.road.oadCols.oad[0].value = frzObjs.oad.value;

    ret = db_write_record(g_staUdpHandle, &inData, &storeNo);    
    if(ret != ERR_OK)
    {
        return ret;
    }

    STAT_FMT_TRACE("tmn frozen record write: oadCols.oad[0] = %lu(0x%08x)\n", 
        inData.recordOAD.road.oadCols.oad[0].value, inData.recordOAD.road.oadCols.oad[0].value);

    return ret;
}

/**
*********************************************************************
* @brief：      执行冻结
* @param[in]： pTmpNode 冻结参数
* @param[out]：NA
* @return：     time_t 下一次冻结时间
*********************************************************************
*/
void sta_frozen_execute(time_t storeTime, FrzRunParaList_t *pTmpNode)
{
    int ret = ERR_OK;
    uint8 FrzDataBuff[300];
    uint32 FrzDataLen;
    uint16 j;
    OOP_FROZENOBJ_T frzObjs;
    BOOLEAN bStart = TRUE;

    STAT_FMT_TRACE("---------frozen execute start--------\n");

    frzObjs.nPeriod = pTmpNode->frzRunPara.period;
    frzObjs.nDepth= pTmpNode->frzRunPara.storeDepth;

    /*遍历每个关联对象属性*/
    for(j = 0; j < pTmpNode->frzRunPara.oadNum; j++)
    {
        frzObjs.oad = pTmpNode->frzRunPara.pColOad[j];

        /*读一个普通OAD db_read_nomal*/
        ret = sta_frozen_objdata_read(frzObjs.oad, FrzDataBuff, sizeof(FrzDataBuff), &FrzDataLen,pTmpNode->frzOAD);
        if(ret != 0)
        {
            STAT_FMT_TRACE("sta_frozen_objdata_read failed. ret(%d) OAD(0x%08x)\n", ret, frzObjs.oad.value);
            continue;   
        }
        
        /* 写一个记录型数据 db_write_record*/
        ret = sta_frozen_record_write(pTmpNode->frzOAD, frzObjs, bStart, storeTime, FrzDataBuff, FrzDataLen);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("sta_frozen_record_write failed. ret(%d), mainOAD(0x%08x), subOAD(0x%08x)\n", ret, pTmpNode->frzOAD.value, frzObjs.oad.value);
            return;   
        }

        bStart = FALSE;
    }

    STAT_FMT_TRACE("---------frozen execute end--------\n");

    return;
}

/**
*********************************************************************
* @brief：      冻结时间刷新
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_frozen_lasttime_reinit(void)
{
    uint8 i = 0;
    FrzRunParaList_t *pstPara = NULL;
    OOP_TI_SPAN_E time_span;
    OOP_TI_T tmpTI;    

    for(i = 0; i < STA_FROZEN_OBJNUMS; i++)
    {
        if(g_staFrozenParaList[i] == NULL)
        {
            continue;
        }
        
        pstPara = g_staFrozenParaList[i];
                
        sta_interval_analyse(pstPara->frzOAD, &time_span);

        STAT_FMT_DEBUG("frozen last time reinit. oda(0x%08x)\n", pstPara->frzOAD.value);

        //初始化上一次时间
        while(pstPara!=NULL)
        {
            tmpTI.unit = time_span;
            tmpTI.value = pstPara->frzRunPara.period;
            
            sta_last_time_init(tmpTI,&pstPara->frzRunPara.lastFrzTm);
            
            pstPara = pstPara->pNext;
        }

    }
    
    return;
}

/**
*********************************************************************
* @brief：      冻结功能处理
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_frozen_proc()
{
    uint8 i;
    FrzRunParaList_t *pTmpNode;//临时节点
    //time_t nextFrzTime;        //时间为秒数，方便转化为协议要求的时间格式
    time_t curTime;
    OOP_TI_SPAN_E time_span;
    OOP_TI_T time_interval;
    DT_DateTimeShort2Time(&realstattime, &curTime);
    OOP_DATETIME_S_T   tmptime;
    /*初始化冻结参数链表*/
    sta_frozen_init();

    tmptime = realstattime;
    /*遍历冻结类型（秒，分，时，日，月，年）没有12种*/
    for(i = 0; i < STA_FROZEN_OBJNUMS; i++)
    {
        if(g_staFrozenParaList[i] == NULL)
        {
            continue;
        }
        
        pTmpNode = g_staFrozenParaList[i];

        if((pTmpNode->frzOAD.nObjID == STA_FROZEN_MIN) 
            || (pTmpNode->frzOAD.nObjID == STA_FROZEN_HOUR))
        {
            continue;
        }
        
        sta_interval_analyse(pTmpNode->frzOAD, &time_span);
        
        while(pTmpNode != NULL)
        {
            time_interval.unit = time_span;
            time_interval.value = pTmpNode->frzRunPara.period;
            
            //sta_nexttime_update(time_interval, pTmpNode->frzRunPara.lastFrzTm, &nextFrzTime);

            /* 满足冻结条件，执行冻结 */
//            if((nextFrzTime!=0) && (nextFrzTime<=curTime))
//            {
//                sta_frozen_execute(nextFrzTime, pTmpNode);
//                pTmpNode->frzRunPara.lastFrzTm = nextFrzTime;
//            }
            if((time_interval.unit ==TI_DAY)&&(pastdayflag))
            {
                tmptime.hour =0x00;
                tmptime.minute =0x00;
                tmptime.second =0x00;
                DT_DateTimeShort2Time(&tmptime, &curTime);
                sta_frozen_execute(curTime, pTmpNode);
                FREEZE_FMT_DEBUG("日冻结数据统计curTime(%d).\n", curTime);
            }
            if((time_interval.unit ==TI_MON)&&(pastmonflag))
            {
                tmptime.day =0x01;
                tmptime.hour =0x00;
                tmptime.minute =0x00;
                tmptime.second =0x00;
                DT_DateTimeShort2Time(&tmptime, &curTime);
                sta_frozen_execute(curTime, pTmpNode);
            }
            pTmpNode = pTmpNode->pNext;
        }
        
    }
    
    return;
}

/**
*********************************************************************
* @brief：      冻结功能处理
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_frozen_curve_proc()
{
    uint8 i;
    FrzRunParaList_t *pTmpNode;//临时节点
    //time_t nextFrzTime;        //时间为秒数，方便转化为协议要求的时间格式
    time_t curTime;
    OOP_TI_SPAN_E time_span;
    OOP_TI_T time_interval;
    DT_DateTimeShort2Time(&realstattime, &curTime);
    uint32 period =0;
    /*初始化冻结参数链表*/
    sta_frozen_init();

    /*遍历冻结类型（秒，分，时，日，月，年）没有12种*/
    for(i = 0; i < STA_FROZEN_OBJNUMS; i++)
    {
        if(g_staFrozenParaList[i] == NULL)
        {
            continue;
        }
        
        pTmpNode = g_staFrozenParaList[i];

        if((pTmpNode->frzOAD.nObjID != STA_FROZEN_MIN) 
            && (pTmpNode->frzOAD.nObjID != STA_FROZEN_HOUR))
        {
            continue;
        }
        
        sta_interval_analyse(pTmpNode->frzOAD, &time_span);
        
        while(pTmpNode != NULL)
        {
            time_interval.unit = time_span;
            time_interval.value = pTmpNode->frzRunPara.period;
            
            //sta_nexttime_update(time_interval, pTmpNode->frzRunPara.lastFrzTm, &nextFrzTime);

            /* 满足冻结条件，执行冻结 */
//            if((nextFrzTime!=0) && (nextFrzTime<=curTime))
//            {
//                sta_frozen_execute(nextFrzTime, pTmpNode);
//                pTmpNode->frzRunPara.lastFrzTm = nextFrzTime;
//            }
            if((time_interval.unit ==TI_MIN)&&(time_interval.value !=0)&&(realstattime.minute%time_interval.value ==0)&&(pastminflag))
            {
                period= time_interval.value*60;
                curTime =(curTime/period)*period;
                sta_frozen_execute(curTime, pTmpNode);
            }
            if((time_interval.unit ==TI_HOUR)&&(pasthourflag))
            {
                period= time_interval.value*3600;
                curTime =(curTime/period)*period;
                sta_frozen_execute(curTime, pTmpNode);
            }
            pTmpNode = pTmpNode->pNext;
        }
        
    }
    
    return;
}

#endif

#if DESC("区间统计处理函数",1)

/**
*********************************************************************
* @brief：      落在前闭后开区间[)
* @param[in]： buffer  报文缓冲区
* @param[in]： offset  缓冲区中的偏移
* @param[out]: value   解析后的值
* @return：     void
* @author    : 贺宁
* @Date      ：2020-04-26     
*********************************************************************
*/
BOOLEAN sta_digit_in_region0(OOP_DIGIT_T data, OOP_DIGIT_T from, OOP_DIGIT_T to)
{
    BOOLEAN result = FALSE;
    
    switch(data.choice) 
    {
        case DT_DOUBLE_LONG: //double long/int32
        {
            if((data.nVal >= from.nVal) && (data.nVal < to.nVal))
            {
                result = TRUE;
            }
        }break;
        case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
        {
            if((data.dwVal >= from.dwVal) && (data.dwVal < to.dwVal))
            {
                result = TRUE;
            }
        }break;     
        case DT_INTEGER: //integer/char
        {
            if((data.cVal >= from.cVal) && (data.cVal < to.cVal))
            {
                result = TRUE;
            }            
        }break;
        case DT_LONG: //long/int16
        {
            if((data.lVal >= from.lVal) && (data.lVal < to.lVal))
            {
                result = TRUE;
            }               
        }break; 
        case DT_UNSIGNED: //unsigned/uint8
        {
            if((data.bVal >= from.bVal) && (data.bVal < to.bVal))
            {
                result = TRUE;
            }             
        }break;   
        case DT_LONG_UNSIGNED: //long unsigned/uint16
        {
            if((data.wVal >= from.wVal) && (data.wVal < to.wVal))
            {
                result = TRUE;
            }              
        }break;   
        case DT_LONG64: //long64/int64
        {   
            if((data.llVal >= from.llVal) && (data.llVal < to.llVal))
            {
                result = TRUE;
            }              
        }break;   
        case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
        {    
            if((data.ullVal >= from.ullVal) && (data.ullVal < to.ullVal))
            {
                result = TRUE;
            }                
        }break;
        default:break;           
    }

    return result;
}

/**
*********************************************************************
* @brief：      落在前开后闭区间(]
* @param[in]： buffer  报文缓冲区
* @param[in]： offset  缓冲区中的偏移
* @param[out]: value   解析后的值
* @return：     void
* @author    : 贺宁
* @Date      ：2020-04-26     
*********************************************************************
*/
BOOLEAN sta_digit_in_region1(OOP_DIGIT_T data, OOP_DIGIT_T from, OOP_DIGIT_T to)
{
    BOOLEAN result = FALSE;
    
    switch(data.choice) 
    {
        case DT_DOUBLE_LONG: //double long/int32
        {
            if((data.nVal > from.nVal) && (data.nVal <= to.nVal))
            {
                result = TRUE;
            }
        }break;
        case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
        {
            if((data.dwVal > from.dwVal) && (data.dwVal <= to.dwVal))
            {
                result = TRUE;
            }
        }break;     
        case DT_INTEGER: //integer/char
        {
            if((data.cVal > from.cVal) && (data.cVal <= to.cVal))
            {
                result = TRUE;
            }            
        }break;
        case DT_LONG: //long/int16
        {
            if((data.lVal > from.lVal) && (data.lVal <= to.lVal))
            {
                result = TRUE;
            }               
        }break; 
        case DT_UNSIGNED: //unsigned/uint8
        {
            if((data.bVal > from.bVal) && (data.bVal <= to.bVal))
            {
                result = TRUE;
            }             
        }break;   
        case DT_LONG_UNSIGNED: //long unsigned/uint16
        {
            if((data.wVal > from.wVal) && (data.wVal <= to.wVal))
            {
                result = TRUE;
            }              
        }break;   
        case DT_LONG64: //long64/int64
        {   
            if((data.llVal > from.llVal) && (data.llVal <= to.llVal))
            {
                result = TRUE;
            }              
        }break;   
        case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
        {    
            if((data.ullVal > from.ullVal) && (data.ullVal <= to.ullVal))
            {
                result = TRUE;
            }                
        }break;
        default:break;           
    }

    return result;
}

/**
*********************************************************************
* @brief：      落在前闭后闭区间[]
* @param[in]： buffer  报文缓冲区
* @param[in]： offset  缓冲区中的偏移
* @param[out]: value   解析后的值
* @return：     void
* @author    : 贺宁
* @Date      ：2020-04-26     
*********************************************************************
*/
BOOLEAN sta_digit_in_region2(OOP_DIGIT_T data, OOP_DIGIT_T from, OOP_DIGIT_T to)
{
    BOOLEAN result = FALSE;
    
    switch(data.choice) 
    {
        case DT_DOUBLE_LONG: //double long/int32
        {
            if((data.nVal >= from.nVal) && (data.nVal <= to.nVal))
            {
                result = TRUE;
            }
        }break;
        case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
        {
            if((data.dwVal >= from.dwVal) && (data.dwVal <= to.dwVal))
            {
                result = TRUE;
            }
        }break;     
        case DT_INTEGER: //integer/char
        {
            if((data.cVal >= from.cVal) && (data.cVal <= to.cVal))
            {
                result = TRUE;
            }            
        }break;
        case DT_LONG: //long/int16
        {
            if((data.lVal >= from.lVal) && (data.lVal <= to.lVal))
            {
                result = TRUE;
            }               
        }break; 
        case DT_UNSIGNED: //unsigned/uint8
        {
            if((data.bVal >= from.bVal) && (data.bVal <= to.bVal))
            {
                result = TRUE;
            }             
        }break;   
        case DT_LONG_UNSIGNED: //long unsigned/uint16
        {
            if((data.wVal >= from.wVal) && (data.wVal <= to.wVal))
            {
                result = TRUE;
            }              
        }break;   
        case DT_LONG64: //long64/int64
        {   
            if((data.llVal >= from.llVal) && (data.llVal <= to.llVal))
            {
                result = TRUE;
            }              
        }break;   
        case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
        {    
            if((data.ullVal >= from.ullVal) && (data.ullVal <= to.ullVal))
            {
                result = TRUE;
            }                
        }break;
        default:break;           
    }

    return result;
}

/**
*********************************************************************
* @brief：      落在前开后开区间()
* @param[in]： buffer  报文缓冲区
* @param[in]： offset  缓冲区中的偏移
* @param[out]: value   解析后的值
* @return：     void
* @author    : 贺宁
* @Date      ：2020-04-26     
*********************************************************************
*/
BOOLEAN sta_digit_in_region3(OOP_DIGIT_T data, OOP_DIGIT_T from, OOP_DIGIT_T to)
{
    BOOLEAN result = FALSE;
    
    switch(data.choice) 
    {
        case DT_DOUBLE_LONG: //double long/int32
        {
            if((data.nVal > from.nVal) && (data.nVal < to.nVal))
            {
                result = TRUE;
            }
        }break;
        case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
        {
            if((data.dwVal > from.dwVal) && (data.dwVal < to.dwVal))
            {
                result = TRUE;
            }
        }break;     
        case DT_INTEGER: //integer/char
        {
            if((data.cVal > from.cVal) && (data.cVal < to.cVal))
            {
                result = TRUE;
            }            
        }break;
        case DT_LONG: //long/int16
        {
            if((data.lVal > from.lVal) && (data.lVal < to.lVal))
            {
                result = TRUE;
            }               
        }break; 
        case DT_UNSIGNED: //unsigned/uint8
        {
            if((data.bVal > from.bVal) && (data.bVal < to.bVal))
            {
                result = TRUE;
            }             
        }break;   
        case DT_LONG_UNSIGNED: //long unsigned/uint16
        {
            if((data.wVal > from.wVal) && (data.wVal < to.wVal))
            {
                result = TRUE;
            }              
        }break;   
        case DT_LONG64: //long64/int64
        {   
            if((data.llVal > from.llVal) && (data.llVal < to.llVal))
            {
                result = TRUE;
            }              
        }break;   
        case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
        {    
            if((data.ullVal > from.ullVal) && (data.ullVal < to.ullVal))
            {
                result = TRUE;
            }                
        }break;
        default:break;           
    }

    return result;
}

/**
*********************************************************************
* @brief：      数值类型解析
* @param[in]： buffer  报文缓冲区
* @param[in]： offset  缓冲区中的偏移
* @param[out]: value   解析后的值
* @return：     void
* @author    : 贺宁
* @Date      ：2020-04-26     
*********************************************************************
*/
BOOLEAN sta_data_in_region(OOP_DIGIT_T data, OOP_DOMAIN_T domain)
{
    if((data.choice != domain.from.choice) || (data.choice != domain.to.choice))
    {
        STAT_FMT_DEBUG("digit type not equal. data(%d), from(%d), to(%d)\n", data.choice, domain.from.choice, domain.to.choice);
        return FALSE;
    }

    switch(domain.type)
    {
        case 0: return sta_digit_in_region0(data, domain.from, domain.to); break;

        case 1: return sta_digit_in_region1(data, domain.from, domain.to); break;

        case 2: return sta_digit_in_region2(data, domain.from, domain.to); break;

        case 3: return sta_digit_in_region3(data, domain.from, domain.to); break;

        default: STAT_FMT_DEBUG("not support for this region type(%d).\n", domain.type); break;
    }

    return FALSE;
}

/**
*********************************************************************
* @brief：      数值类型解析
* @param[in]： buffer  报文缓冲区
* @param[in]： offset  缓冲区中的偏移
* @param[out]: value   解析后的值
* @return：     void
*********************************************************************
*/
void sta_region_digit_construct(OOP_DIGIT_T rtData, time_t intervalTime, STA_REGIONOBJ_T region, OOP_REGION_T *result)
{
    uint8 changedFlag = 0;
    int i = 0;
    
    //从最大上限开始，大于则说明数据落在比它大的区间，记录后直接结束这个数据的判断
    for(i = region.nNum-1; i >= 0; i--)
    {
        switch(rtData.choice) 
        {
            case DT_DOUBLE_LONG: //double long/int32
            {
                if(rtData.nVal >= region.parm[i].value.nVal)
                {
                    changedFlag = 1;
                }
            }break;
            case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
            {
                if(rtData.dwVal >= region.parm[i].value.dwVal)
                {
                    changedFlag = 1;
                }
            }break;     
            case DT_INTEGER: //integer/char
            {
                if(rtData.cVal >= region.parm[i].value.cVal)
                {
                    changedFlag = 1;
                }
            }break;
            case DT_LONG: //long/int16
            {
                if(rtData.lVal >= region.parm[i].value.lVal)
                {
                    changedFlag = 1;
                }
            }break; 
            case DT_UNSIGNED: //unsigned/uint8
            {
                if(rtData.bVal >= region.parm[i].value.bVal)
                {
                    changedFlag = 1;
                }
            }break;   
            case DT_LONG_UNSIGNED: //long unsigned/uint16
            {
                if(rtData.wVal >= region.parm[i].value.wVal)
                {
                    changedFlag = 1;
                }
            }break;   
            case DT_LONG64: //long64/int64
            {
                if(rtData.llVal >= region.parm[i].value.llVal)
                {
                    changedFlag = 1;
                }                   
            }break;   
            case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
            {
                if(rtData.ullVal >= region.parm[i].value.ullVal)
                {
                    changedFlag = 1;
                }           
            }break;
            default:break;           
        }
        
        if(changedFlag)
        {
            result->value[i+1].dwCount++;               //此区间内越限次数累加
            result->value[i+1].dwTimes += intervalTime; //累积时间，秒
            break;
        }
    }
    
    //没有任何越限,记录在最小限[0]
    if(i == -1)
    {
        result->value[i+1].dwCount++;               //此区间内越限次数增加累加
        result->value[i+1].dwTimes += intervalTime; //累积时间，秒
    }
    
    result->nNum = region.nNum+1;
    result->obj.value = region.obj.value;
}

/**
*********************************************************************
* @brief：      数值类型解析
* @param[in]： buffer  报文缓冲区
* @param[in]： offset  缓冲区中的偏移
* @param[out]: value   解析后的值
* @return：     void
*********************************************************************
*/
void sta_region_range_construct(OOP_DIGIT_T rtData, time_t intervalTime, STA_REGIONOBJ_T region, OOP_REGION_T *result)
{
    int i = 0;

    //从第一个区间开始，一个个区间比较
    for(i = 0; i < region.nNum; i++)
    {
        if(sta_data_in_region(rtData, region.parm[i]))
        {
            result->value[i].dwCount++;               //此区间内越限次数增加累加
            result->value[i].dwTimes += intervalTime; //累积时间，秒
        }
    }

    result->nNum = region.nNum;

    return;
}

/**
*********************************************************************
* @brief：      数值类型解析
* @param[in]： buffer  报文缓冲区
* @param[in]： offset  缓冲区中的偏移
* @param[out]: value   解析后的值
* @return：     void
*********************************************************************
*/
void sta_region_construct(OOP_DIGIT_T rtData, time_t intervalTime, STA_REGIONOBJ_T region, OOP_REGION_T *result)
{
    //要么都是数值，要么都是区间，不可能混合
    if(region.parm[0].choice == 0)
    {
        sta_region_digit_construct(rtData, intervalTime, region, result);
    }
    else
    {
        sta_region_range_construct(rtData, intervalTime, region, result);
    }
}

/**
*********************************************************************
* @brief：      区间统计计算(通用计算)
* @param[in]： rtData  参数计算的数据
* @param[out]：outData 计算结果
* @return：     void
*********************************************************************
*/
void sta_region_calc(OOP_DIGIT_T rtData, OOP_OAD_U mainOAD, OOP_OAD_U subOAD, uint32 count, time_t intervalTime, STA_REGIONOBJ_T region)
{
    int ret = ERR_OK;
    OOP_REGION_T result = {{0}};  //一个区间统计结果
    READ_RECORD_T readRecord;
    RECORD_UNIT_T recordBuf = {0};
    WRITE_RECORD_T writeRecord;
    uint32 storeNo;
    uint32 len = sizeof(OOP_REGION_T);    
    UPDATE_RECORD_T updateRecord;
    BOOLEAN isUpdate = FALSE;
    uint8 i = 0;
    uint8 *pAddr = NULL;

    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&writeRecord, sizeof(writeRecord));
    MEMZERO(&updateRecord, sizeof(updateRecord));

    //获取终端地址
    app_get_localaddr(&pAddr);

    // 校验码，通信地址的前6个字节
    readRecord.MAC.nNum = 6;
    memcpy(readRecord.MAC.value, pAddr, 6);
 /*   
    writeRecord.MAC.nNum = 6;
    memcpy(writeRecord.MAC.value, pAddr, 6);

    updateRecord.MAC.nNum = 6;
    memcpy(updateRecord.MAC.value, pAddr, 6);
*/
    result.obj.value = subOAD.value;

    /* 首次达到统计周期，先清历史数据 */    
    if(count == 0)
    {
        writeRecord.recordOAD.road.oadMain.value = mainOAD.value;
        writeRecord.recordOAD.road.oadCols.nNum = 1;
        writeRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;
        writeRecord.inDatalen = len;
        writeRecord.colStoreTm = time(NULL) + COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间

        memcpy(writeRecord.inDataBuf, (uint8*)&result, len);
        
        ret = db_write_record(g_staUdpHandle, &writeRecord, &storeNo);  

        STAT_FMT_DEBUG("region new cycle begin. ret(%d) oadMain(0x%08x), oadCols(0x%08x), len(%d)\n", 
           ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, len);
    }

    //读取历史统计数据
    readRecord.recordOAD.road.oadMain.value = mainOAD.value;
    readRecord.recordOAD.road.oadCols.nNum = 1;
    readRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;     
    readRecord.cType = COL_TM_STORE;
    readRecord.sortType = DOWN_SORT;
    readRecord.cStart = 0;
    readRecord.cEnd = time(NULL) + COMPENSATION_TIME + 1; // 注：+8个小时(28800)转成格林时间
    
    ret = db_read_record(g_staUdpHandle, &readRecord, sizeof(result), &recordBuf);
    if(ret != ERR_OK)
    {
        isUpdate = FALSE;
        STAT_FMT_DEBUG("region old read failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
           ret, readRecord.recordOAD.road.oadMain.value, readRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
    }
    else
    {
        isUpdate = TRUE;
        
        if(len != recordBuf.record[0].outDatalen)
        {
            STAT_FMT_DEBUG("len(%d) != outDatelen(%d)\n", len, recordBuf.record[0].outDatalen);
        }

        memcpy((uint8*)&result, recordBuf.record[0].outDataBuf, len);
    
        for(i = 0; i < result.nNum; i ++)
        {
            STAT_FMT_TRACE("region old[%d]: dwTimes(%d), dwCount(%d)\n", i, result.value[i].dwTimes, result.value[i].dwCount);
        }       
    }

    sta_region_construct(rtData, intervalTime, region, &result);

    //累积完成后回写
    if(isUpdate)
    {
        updateRecord.filter.choice = 1;
        updateRecord.recordOAD.road.oadMain.value = mainOAD.value;
        updateRecord.recordOAD.road.oadCols.nNum = 1;
        updateRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;
        updateRecord.inDatalen = len;
        updateRecord.filter.storeTm = recordBuf.colStoreTm;

        //传出时，按结构体强转
        memcpy(updateRecord.inDataBuf, (uint8*)&result, len);

        ret = db_update_record(g_staUdpHandle, &updateRecord);   
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("region update failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
               ret, updateRecord.recordOAD.road.oadMain.value, updateRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
        }
    }
    else
    {
        writeRecord.recordOAD.road.oadMain.value = mainOAD.value;
        writeRecord.recordOAD.road.oadCols.nNum = 1;
        writeRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;
        writeRecord.inDatalen = len;
        writeRecord.colStoreTm = time(NULL) + COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间

        //传出时，按结构体强转
        memcpy(writeRecord.inDataBuf, (uint8*)&result, len);

        ret = db_write_record(g_staUdpHandle, &writeRecord, &storeNo);   
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("region write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
               ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
        }
    }

    for(i = 0; i < result.nNum; i ++)
    {
        STAT_FMT_TRACE("region new[%d]: dwTimes(%d), dwCount(%d)\n", i, result.value[i].dwTimes, result.value[i].dwCount);
    }

    return; 
}

/**
*********************************************************************
* @brief：      添加区间统计参数节点
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_region_node_add(STA_REGIONS_PARA_T *pstPara)
{
    uint8 i = 0,j=0;
    int ret = ERR_OK;
    OOP_REGIONS_T subObjs;
    uint32 nNum = 0;
    OOP_OAD_U mainObj;
    STA_REGIONS_T *block = NULL;
    NOMAL_OAD_T nomalOAD = {0};
    uint32 len = 0;

    mainObj.value = pstPara->oad.value + 0x0300;
    block = pstPara->para;
    
    /*使用mainObj读取区间统计参数*/
    nomalOAD.logicId = 0;
    nomalOAD.oad.value = mainObj.value;

    /* 获取统计数据源 */
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_REGIONS_T), (uint8*)&subObjs, &len);
    if(ret != 0 )
    {
        STAT_FMT_DEBUG("region param read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_REGIONS_T));
        return;
    } 
    
    //先清空block
    memset(block, 0, sizeof(STA_REGIONS_T)*OOP_REGION_OBJS);
    pstPara->blockNum = 0;

    if(subObjs.nNum > OOP_REGION_OBJS)
    {
        STAT_FMT_DEBUG("region Illegal subObjs.nNum(%u)\n", subObjs.nNum);
        return;
    }

    /*读出的参数按统计周期相同，重新整理分布*/
    for(i = 0; i < subObjs.nNum; i++)
    {
        STAT_FMT_DEBUG(" subObjs.nNum(%d),i %d,subObjs.objs[i].period %d, subObjs.objs[i].uint %d,subObjs.objs[i].freq.value is %d,subObjs.objs[i].obj.value %x\n", subObjs.nNum,i,subObjs.objs[i].period,subObjs.objs[i].freq.unit,subObjs.objs[i].freq.value,subObjs.objs[i].obj.value);
        for(j = 0; j< OOP_REGION_OBJS; j++)
        {
            nNum = block[j].nNum;
            STAT_FMT_DEBUG(" subObjs.nNum(%d),j is %d\n", block[j].nNum,j);

            STAT_FMT_DEBUG(" block[j].freq.unit(%d),j is %d\n", block[j].freq.unit,block[j].freq.value);
            /*组中关联对象为0，或者成员的统计周期与间隔相同，则新关联对象加入这个组*/
            if((nNum == 0) || ((block[j].period == subObjs.objs[i].period)
                && sta_time_interval_equal(block[j].freq,subObjs.objs[i].freq)))
            {
                if(nNum == 0)
                {
                    pstPara->blockNum++;   
                }
                      
                block[j].period = subObjs.objs[i].period;
                block[j].freq = subObjs.objs[i].freq;
                
                block[j].objs[nNum].obj = subObjs.objs[i].obj;
                block[j].objs[nNum].nNum = subObjs.objs[i].nNum;
                memcpy(block[j].objs[nNum].parm, subObjs.objs[i].parm, subObjs.objs[i].nNum*sizeof(OOP_DOMAIN_T));

                block[j].nNum++;
                STAT_FMT_DEBUG(" block[%d].nNum(%d),i is %d\n", j,block[j].nNum,i);
                break;
            }
        }
    }

    //初始化上一次时间
    for(i = 0; i < pstPara->blockNum; i++)
    {
        sta_last_time_init(pstPara->para[i].freq, &pstPara->para[i].lastTime);
    }

    return;
}

/**
*********************************************************************
* @brief：      区间统计关联OAD的校验
* @param[in]： oad         待校验的oad
* @return：     TRUE-校验通过，FALSE-校验不通过
*********************************************************************
*/
BOOL sta_region_slave_check(OOP_OAD_U oad)
{
    //电压，电流，电压不平衡率，电流不平衡率，功率，功率因数，谐波含有率
    if((oad.nObjID == 0x2000) || (oad.nObjID == 0x2001) 
        || (oad.nObjID == 0x2026)|| (oad.nObjID == 0x2027) 
        || (oad.nObjID == 0x2004)|| (oad.nObjID == 0x2005)
        || (oad.nObjID == 0x2006)|| (oad.nObjID == 0x200A))
    {
        return TRUE;
    }

    return FALSE;
}

/**
*********************************************************************
* @brief：      电压区间统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_region_volt_calc(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_REGIONS_T regionPara)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_WORD3V_T rtData;     //当前实时数据
    uint32 len = 0;
    int i = 0;
    STA_REGIONOBJ_T region;
    OOP_DIGIT_T digit = {0};

    region = regionPara.objs[index];
    
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = region.obj.value;

    /* 获取统计数据源 */
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_WORD3V_T), (uint8*)&rtData, &len);
    if(ret < 0)
    {
        STAT_FMT_DEBUG("region volt read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_COGNATES_T));
        return;
    }

    digit.choice = DT_LONG_UNSIGNED;

    //三相
    for(i = 0; i < 3; i++)
    {
        region.obj.nIndex = i+1; //一次取一相

        digit.wVal = rtData.nValue[i];

        sta_region_calc(digit, mainOAD, region.obj, regionPara.freqCnt, (storeTime-regionPara.lastTime), region);
    }
}

/**
*********************************************************************
* @brief：      电流区间统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_region_curr_calc(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_REGIONS_T regionPara)
{
    NOMAL_OAD_T nomalOAD = {0};
    OOP_INT3V_T rtData;     //当前实时数据
    uint32 len = 0;
    int i = 0;
    STA_REGIONOBJ_T region;
    OOP_DIGIT_T digit = {0};

    region = regionPara.objs[index];
    
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = region.obj.value;

    /* 获取统计数据源 */
    if(db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_INT3V_T), (uint8*)&rtData, &len) <0 )
    {
        return;
    }

    digit.choice = DT_DOUBLE_LONG;

    //三相
    for(i = 0; i < 3; i++)
    {
        region.obj.nIndex = i+1; //一次取一相

        digit.nVal = rtData.nValue[i];
        
        sta_region_calc(digit, mainOAD, region.obj, regionPara.freqCnt, (storeTime-regionPara.lastTime), region);
    }
}

/**
*********************************************************************
* @brief：      电流/电压不平衡率的区间统计
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_region_imbalance_calc(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_REGIONS_T regionPara)
{
    NOMAL_OAD_T nomalOAD = {0};
    uint16 rtData;          //实时数据，统计源数据
    uint32 len = 0;
    STA_REGIONOBJ_T region;
    OOP_DIGIT_T digit = {0};
    
    region = regionPara.objs[index];
    
    nomalOAD.logicId = 0;
    nomalOAD.oad.value = region.obj.value;

    /* 获取统计源数据 */
    if(db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(uint16), (uint8*)&rtData, &len) <0 )
    {
        return;
    }

    digit.choice = DT_LONG_UNSIGNED;
    digit.wVal = rtData;

    sta_region_calc(digit, mainOAD, region.obj, regionPara.freqCnt, (storeTime-regionPara.lastTime), region);
}

/**
*********************************************************************
* @brief：      功率区间统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_region_power_calc(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_REGIONS_T regionPara)
{
    NOMAL_OAD_T nomalOAD = {0};
    OOP_INT4V_T rtData;          //实时数据，统计源数据
    uint32 len = 0;
    int i = 0;
    STA_REGIONOBJ_T region;
    OOP_DIGIT_T digit = {0};

    region = regionPara.objs[index];
    
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = region.obj.value;

    /* 获取统计源数据 */
    if(db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_INT4V_T), (uint8*)&rtData, &len) <0 )
    {
        return;
    }

    digit.choice = DT_LONG;

    //总及分相
    for(i = 0; i < 3; i++)
    {
        region.obj.nIndex = i+1; //一次取一相
        
        digit.lVal = rtData.nValue[i];  
        
        sta_region_calc(digit, mainOAD, region.obj, regionPara.freqCnt, (storeTime-regionPara.lastTime), region);
    }

}

/**
*********************************************************************
* @brief：      功率区间统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_region_powerfac_calc(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_REGIONS_T regionPara)
{
    NOMAL_OAD_T nomalOAD = {0};
    OOP_LONG4V_T rtData;          //实时数据，统计源数据
    uint32 len = 0;
    int i = 0;
    STA_REGIONOBJ_T region;
    OOP_DIGIT_T digit = {0};

    region = regionPara.objs[index];
    region.obj.nIndex = 0; //从数据中心读出总的
    
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = region.obj.value;

    /* 获取统计源数据 */
    if(db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_LONG4V_T), (uint8*)&rtData, &len) != 0 )
    {
        return;
    }

    STAT_FMT_TRACE("region(0x%08x) calc: oad(0x%08x), rtData(%d), choice(%d)\n", mainOAD.value, region.obj.value, rtData.nValue[0], region.parm[0].choice);
    
    for(i = 0; i < region.nNum; i++)
    {
        if(region.parm[i].choice == 1)
        {
            switch(region.parm[i].type)
            {
                case 0: STAT_FMT_TRACE("[%d,%d)\n", region.parm[i].from.lVal, region.parm[i].to.lVal); break;
                case 1: STAT_FMT_TRACE("(%d,%d]\n", region.parm[i].from.lVal, region.parm[i].to.lVal); break;
                case 2: STAT_FMT_TRACE("[%d,%d]\n", region.parm[i].from.lVal, region.parm[i].to.lVal); break;
                case 3: STAT_FMT_TRACE("(%d,%d)\n", region.parm[i].from.lVal, region.parm[i].to.lVal); break;
                default: break; 
            }
        }
    }

    digit.choice = DT_LONG;

    //总及分相
    for(i = 0; i < 4; i++)
    {
        region.obj.nIndex = i+1; //一次取一相
        
        digit.nVal = rtData.nValue[i];  
        
        sta_region_calc(digit, mainOAD, region.obj, regionPara.freqCnt, (storeTime-regionPara.lastTime), region);
    }

}

/**
*********************************************************************
* @brief：      执行区间统计
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_region_execute(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, STA_REGIONS_T regionPara)
{
    uint32 i = 0;
    OOP_OAD_U oad;

    mainOAD.value += 0x0200;

    for(i = 0; i < regionPara.nNum; i++)
    {
        oad.value = regionPara.objs[i].obj.value;

        //区间统计目前只支持 电压，电流，电流不平衡率，视在功率
        if(sta_region_slave_check(oad))
        {
            //TODO 块类型要拆分出子项
            switch(oad.value)
            {   
                case 0x20000200: //三相电压(uint16)
                {
                    sta_region_volt_calc(mainOAD, curTime, storeTime, i, regionPara);

                }break;
                
                case 0x20010200: //三相电流(int32)
                {
                    sta_region_curr_calc(mainOAD, curTime, storeTime, i, regionPara);

                }break;   
                case 0x20270200: //电流不平衡率(uint16)
                {
                    sta_region_imbalance_calc(mainOAD, curTime, storeTime, i, regionPara);

                }break;
                case 0x20040200: //有功功率（总及分相（int32））
                {
                    sta_region_power_calc(mainOAD, curTime, storeTime, i, regionPara);

                }break; 
                case 0x20050200: //无功功率（总及分相（int32））
                {
                    sta_region_power_calc(mainOAD, curTime, storeTime, i, regionPara);

                }break;                 
                case 0x20060200: //视在功率（总及分相（int32））
                {
                    sta_region_power_calc(mainOAD, curTime, storeTime, i, regionPara);

                }break; 
                case 0x200A0200: //功率因数（总及分相（int16））
                case 0x200A0201: //功率因数（总（int16））
                {
                    sta_region_powerfac_calc(mainOAD, curTime, storeTime, i, regionPara);

                }break;                 
                default:STAT_FMT_DEBUG("region object func not exit. OAD(0x%08x)\n", oad.value); break;
            }
        }   
        else
        {
            STAT_FMT_DEBUG("region object not support. nObjID(%d)\n", oad.nObjID);
        }
    }
}

/**
*********************************************************************
* @brief：      区间统计初始化
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_region_init(void)
{
    uint8 i = 0;
    uint8 regionInitFlag = 0;

    pthread_mutex_lock(&g_staUpdateData.mutex);

    /*读取区间统计参数变更标记*/
    regionInitFlag = g_staUpdateData.regionInitFlag;

    for(i = 0; i < STA_REGION_OBJNUMS; i++)
    {
        if(regionInitFlag&(1UL<<i))
        {
            sta_region_node_add(&g_staRegionsPara[i]);
            regionInitFlag &= ~(1UL<<i);
        }
    }
    
    /*存储区间统计参数变更标记*/
    g_staUpdateData.regionInitFlag = regionInitFlag;

    pthread_mutex_unlock(&g_staUpdateData.mutex);    

    return;
}

/**
*********************************************************************
* @brief：      统计时间刷新
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_region_lasttime_reinit(void)
{
    uint8 i = 0, j = 0;
    STA_REGIONS_PARA_T *pstPara = NULL;

    for(i = 0; i < STA_REGION_OBJNUMS; i++)
    {
        if(g_staRegionsPara[i].blockNum == 0)
        {
            continue;
        }
            
        pstPara = &g_staRegionsPara[i];

        STAT_FMT_DEBUG("region last time reinit. oda(0x%08x)\n", pstPara->oad.value);
        
        //初始化上一次时间
        for(j = 0; j < pstPara->blockNum; j++)
        {
            sta_last_time_init(pstPara->para[j].freq, &pstPara->para[j].lastTime);
        }
    }
    
    return;
}

/**
*********************************************************************
* @brief：      区间统计处理
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_region_proc()
{
    uint32 i = 0;
    uint32 j = 0;
    STA_REGIONS_PARA_T *tmpNode = NULL;
    time_t nextTime = 0;
    time_t curTime;
    OOP_TI_SPAN_E time_span;

    DT_DateTimeShort2Time(&realstattime, &curTime);

    /*区间统计类型（分，时，日，月，年）5种*/
    for(i = 0; i < STA_REGION_OBJNUMS; i++)
    {
        if(g_staRegionsPara[i].blockNum == 0)
        {
            continue;
        }
        
        tmpNode = &g_staRegionsPara[i];

        sta_interval_analyse(tmpNode->oad, &time_span);

        //每个block中的对象具有相同的freq和period
        for(j = 0; j < tmpNode->blockNum; j++)
        {
            sta_nexttime_update(tmpNode->para[j].freq, tmpNode->para[j].lastTime, &nextTime);

            /* 当前时间初始是比下次时间大的，当下次时间比它小时，说明时间到了，满足统计条件，执行统计*/
            if((nextTime!=0) && (nextTime<=curTime))
            {
                sta_region_execute(tmpNode->oad, curTime, nextTime, tmpNode->para[j]);
                tmpNode->para[j].lastTime = nextTime;
                tmpNode->para[j].freqCnt++;  
            }

            //刷新周期数
            sta_period_cnt_update(time_span, tmpNode->para[j].period, &tmpNode->para[j].count, &tmpNode->para[j].freqCnt);     
        }
        
    }    
    
    return;
}

#endif
#if DESC("新区间统计处理函数",1)

/**
*********************************************************************
* @brief：      区间统计计算(通用计算)
* @param[in]： rtData  参数计算的数据
* @param[out]：outData 计算结果
* @return：     void
*********************************************************************
*/
void sta_region_calc_new(OOP_DIGIT_T rtData, OOP_OAD_U mainOAD, OOP_OAD_U subOAD, uint32 count, time_t intervalTime, STA_REGIONOBJ_T region,uint16 pn)
{
    int ret = ERR_OK;
    OOP_REGIONSS_T result;  //一个累加平均统计结果
    READ_RECORD_T readRecord = {{0}};
    WRITE_RECORD_T writeRecord = {{0}};
    uint32 storeNo = 0;
    uint32 len = sizeof(OOP_REGIONSS_T);
    OOP_METER_T oopMeter;
    NOMAL_OAD_T ptOAD;
    uint16 i=0;
    OOP_OAD_U temmainoad;
    uint32 outlen=0;
    time_t realtime=time(NULL);
    OOP_DATETIME_S_T dateTime;
    time_t linuxTim;

    MEMZERO(&result,sizeof(result));
    MEMZERO(&writeRecord, sizeof(writeRecord));
    ptOAD.logicId=0;
    ptOAD.oad = mainOAD;
    ptOAD.infoNum=pn;


    //获取地址
    app_get_meter_list_by_id(pn, &oopMeter);

    ret=db_read_nomal(g_staUdpHandle,&ptOAD,sizeof(result),(uint8*)&result,&outlen);

    //STAT_BUF_DEBUG((uint8*)&result,sizeof(result),"读取结果 ret is %d\n",ret);
    if((ret != ERR_OK)||(sizeof(result)!=outlen))
    {
        STAT_FMT_DEBUG("读取累加平均失败 ret is %d,outlen is %d\n",ret,outlen);
        MEMZERO(&result,sizeof(result));
    }


    for(i=0;i<result.nNum;i++)
    {
        if(result.regions[i].obj.value == subOAD.value)
        {
            break;
        }
    }

    if(i>=result.nNum)
    {
        result.nNum++;
        result.regions[i].obj.value = subOAD.value;
    }

    sta_region_construct(rtData, intervalTime, region, &result.regions[i]);

    STAT_FMT_DEBUG("result.totalavg[i].obj.value is %x\n",result.regions[i].obj.value);
    // 校验码，通信地址的前6个字节
    readRecord.MAC.nNum = 6;
    memcpy(readRecord.MAC.value, oopMeter.basic.tsa.add, 6);

    STAT_FMT_DEBUG("count is %d,intervalTime is %d\n",count,intervalTime);
    /* 首次达到统计周期，初始化计算数据*/    
    if(pastdayflag)
    {    
        if(mainOAD.value==0x21020200)
        {
            temmainoad.value =0x50040200;
        }
        else if(mainOAD.value==0x21030200)
        {
            temmainoad.value =0x50060200;
        }

        ret=db_write_nomal(g_staUdpHandle,&ptOAD,(uint8*)&result,sizeof(result));
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("刷新累加累加平均数据失败 ret is %d\n",ret);
        }
        //首次到达清零
        if(subOAD.value ==0x20000203)
        {
            realtime=time(NULL);
            writeRecord.recordOAD.infoNum = pn;
            writeRecord.recordOAD.road.oadMain.value = temmainoad.value;
            writeRecord.recordOAD.road.oadCols.nNum = 1;
            writeRecord.recordOAD.road.oadCols.oad[0].value = mainOAD.value;
            writeRecord.inDatalen = len;
            writeRecord.colStartTm =(uint32)realtime+COMPENSATION_TIME;
            writeRecord.colEndTm =(uint32)realtime+COMPENSATION_TIME;
            DT_Time2DateTimeShort(realtime,&dateTime);
            dateTime.minute=0x00;
            dateTime.second=0x00;
            DT_DateTimeShort2Time(&dateTime,&linuxTim);
            writeRecord.colStoreTm = (uint32)linuxTim+COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间
            STAT_FMT_DEBUG("writeRecord.colStartTm(%d) writeRecord.colEndTm(%d), writeRecord.colStoreTm(%d)\n", 
                   writeRecord.colStartTm, writeRecord.colEndTm, writeRecord.colStoreTm);
            //传出时，按结构体强转
            memcpy(writeRecord.inDataBuf, (uint8*)&result, len);
            
            memset((uint8*)&result,0x00,sizeof(result));
            ret=db_write_nomal(g_staUdpHandle,&ptOAD,(uint8*)&result,sizeof(result));
            if(ret != ERR_OK)
            {
                STAT_FMT_DEBUG("清除累加平均数据失败 ret is %d\n",ret);
            }

        
            STAT_FMT_DEBUG("totalavg first write oadMain(0x%08x), oadCols(0x%08x), len(%d),result.nNum(%d)\n", 
                           writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, len,result.nNum);
            ret= db_write_record(g_staUdpHandle, &writeRecord, &storeNo);   
            if(ret != ERR_OK)
            {
                STAT_FMT_DEBUG("totalavg first write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), len(%d)\n", 
                   ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, len);
            }

            return;
        }
    }
    else
    {
        ret=db_write_nomal(g_staUdpHandle,&ptOAD,(uint8*)&result,sizeof(result));
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("刷新累加累加平均数据失败 ret is %d\n",ret);
        }
    }
      


    return;
}


/**
*********************************************************************
* @brief：      电压区间统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_region_volt_calc_new(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_REGIONS_T regionPara,uint16 pn)
{
    RECORD_OAD_T recordOAD={0};
    OOP_WORD3V_T rtData;     //当前实时数据
    int i = 0;
    OOP_DIGIT_T digit = {0};
    uint32 storeTm;
    int ret;
    
    recordOAD.road.oadMain.value=0x00;
    recordOAD.road.oadCols.nNum=1;
    recordOAD.road.oadCols.oad[0].value=0x20000200;
    recordOAD.classtag =5;
    recordOAD.logicId =0;
    recordOAD.infoNum = pn;

    STAT_FMT_DEBUG("pn is %d\n",pn);
    memset((uint8*)&rtData,0x00,sizeof(rtData));
    //读取最近一次电压数据
    ret=sta_record_read_new(g_staUdpHandle,&recordOAD, &storeTm,(uint8*)&rtData,sizeof(OOP_WORD3V_T));
    if(ret!=0)
    {
        STAT_BUF_DEBUG((uint8*)&rtData,sizeof(OOP_WORD3V_T),"读取结果 ret is %d,pn %d\n",ret,pn);
        return;
    }


    digit.choice = DT_LONG_UNSIGNED;
    STA_REGIONOBJ_T region;
    region = regionPara.objs[index];
    //三相
    for(i = 0; i < 3; i++)
    {
        region.obj.nIndex = i+1; //一次取一相

        digit.wVal = rtData.nValue[i];

        sta_region_calc_new(digit, mainOAD, region.obj, regionPara.freqCnt, (storeTime-regionPara.lastTime), region,pn);
    }
}



/**
*********************************************************************
* @brief：      电压区间统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_region_power_calc_new(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_REGIONS_T regionPara,uint16 pn)
{
    RECORD_OAD_T recordOAD={0};
    OOP_INT4V_T rtData;     //当前实时数据
    int i = 0;
    OOP_DIGIT_T digit = {0};
    uint32 storeTm;
    int ret;
    
    recordOAD.road.oadMain.value=0x00;
    recordOAD.road.oadCols.nNum=1;
    recordOAD.road.oadCols.oad[0].value=0x20040200;
    recordOAD.classtag =5;
    recordOAD.logicId =0;
    recordOAD.infoNum = pn;

    STAT_FMT_DEBUG("pn is %d\n",pn);
    memset((uint8*)&rtData,0x00,sizeof(rtData));
    //读取最近一次电压数据
    ret=sta_record_read_new(g_staUdpHandle,&recordOAD, &storeTm,(uint8*)&rtData,sizeof(OOP_INT4V_T));
    if(ret!=0)
    {
        STAT_BUF_DEBUG((uint8*)&rtData,sizeof(OOP_INT4V_T),"读取结果 ret is %d,pn %d\n",ret,pn);
        return;
    }


    digit.choice = DT_DOUBLE_LONG;
    STA_REGIONOBJ_T region;
    region = regionPara.objs[index];
    //三相
    for(i = 0; i < 4; i++)
    {
        region.obj.nIndex = i+1; //一次取一相

        digit.wVal = rtData.nValue[i];

        sta_region_calc_new(digit, mainOAD, region.obj, regionPara.freqCnt, (storeTime-regionPara.lastTime), region,pn);
    }
}





/**
*********************************************************************
* @brief：      电压区间统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_region_apparent_power_calc_new(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_REGIONS_T regionPara,uint16 pn)
{
    RECORD_OAD_T recordOAD={0};
    OOP_INT4V_T rtData;     //当前实时数据
    int i = 0;
    OOP_DIGIT_T digit = {0};
    uint32 storeTm;
    int ret;
    
    recordOAD.road.oadMain.value=0x00;
    recordOAD.road.oadCols.nNum=1;
    recordOAD.road.oadCols.oad[0].value=0x20060200;
    recordOAD.classtag =5;
    recordOAD.logicId =0;
    recordOAD.infoNum = pn;

    STAT_FMT_DEBUG("pn is %d\n",pn);
    memset((uint8*)&rtData,0x00,sizeof(rtData));
    //读取最近一次电压数据
    ret=sta_record_read_new(g_staUdpHandle,&recordOAD, &storeTm,(uint8*)&rtData,sizeof(OOP_INT4V_T));
    if(ret!=0)
    {
        STAT_BUF_DEBUG((uint8*)&rtData,sizeof(OOP_INT4V_T),"读取结果 ret is %d,pn %d\n",ret,pn);
        return;
    }


    digit.choice = DT_DOUBLE_LONG;
    STA_REGIONOBJ_T region;
    region = regionPara.objs[index];
    //三相
    for(i = 0; i < 4; i++)
    {
        region.obj.nIndex = i+1; //一次取一相

        digit.wVal = rtData.nValue[i];

        sta_region_calc_new(digit, mainOAD, region.obj, regionPara.freqCnt, (storeTime-regionPara.lastTime), region,pn);
    }
}






/**
*********************************************************************
* @brief：      电压区间统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_region_powerfac_calc_new(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_REGIONS_T regionPara,uint16 pn)
{
    RECORD_OAD_T recordOAD={0};
    OOP_LONG4V_T rtData;     //当前实时数据
    int i = 0;
    OOP_DIGIT_T digit = {0};
    uint32 storeTm;
    int ret;
    
    recordOAD.road.oadMain.value=0x00;
    recordOAD.road.oadCols.nNum=1;
    recordOAD.road.oadCols.oad[0].value=0x200A0200;
    recordOAD.classtag =5;
    recordOAD.logicId =0;
    recordOAD.infoNum = pn;

    STAT_FMT_DEBUG("pn is %d\n",pn);
    memset((uint8*)&rtData,0x00,sizeof(rtData));
    //读取最近一次电压数据
    ret=sta_record_read_new(g_staUdpHandle,&recordOAD, &storeTm,(uint8*)&rtData,sizeof(OOP_LONG4V_T));
    if(ret!=0)
    {
        STAT_BUF_DEBUG((uint8*)&rtData,sizeof(OOP_LONG4V_T),"读取结果 ret is %d,pn %d\n",ret,pn);
        return;
    }


    digit.choice = DT_LONG;
    STA_REGIONOBJ_T region;
    region = regionPara.objs[index];
    //三相
    for(i = 0; i < 4; i++)
    {
        region.obj.nIndex = i+1; //一次取一相

        digit.wVal = rtData.nValue[i];

        sta_region_calc_new(digit, mainOAD, region.obj, regionPara.freqCnt, (storeTime-regionPara.lastTime), region,pn);
    }
}

/**
*********************************************************************
* @brief：      电压区间统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_region_curr_calc_new(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_REGIONS_T regionPara,uint16 pn)
{
    RECORD_OAD_T recordOAD={0};
    OOP_INT3V_T rtData;     //当前实时数据
    int i = 0;
    OOP_DIGIT_T digit = {0};
    uint32 storeTm;
    int ret;
    
    recordOAD.road.oadMain.value=0x00;
    recordOAD.road.oadCols.nNum=1;
    recordOAD.road.oadCols.oad[0].value=0x20010200;
    recordOAD.classtag =5;
    recordOAD.logicId =0;
    recordOAD.infoNum = pn;

    STAT_FMT_DEBUG("pn is %d\n",pn);
    memset((uint8*)&rtData,0x00,sizeof(rtData));
    //读取最近一次电压数据
    STAT_BUF_DEBUG((uint8*)&rtData,sizeof(OOP_INT3V_T),"读取结果 ret is %d\n",pn);
    ret=sta_record_read_new(g_staUdpHandle,&recordOAD, &storeTm,(uint8*)&rtData,sizeof(OOP_INT3V_T));
    if(ret!=0)
    {
        STAT_BUF_DEBUG((uint8*)&rtData,sizeof(OOP_INT3V_T),"读取结果 ret is %d,pn %d\n",ret,pn);
        return;
    }


    digit.choice = DT_DOUBLE_LONG;
    STA_REGIONOBJ_T region;
    region = regionPara.objs[index];
    //三相
    for(i = 0; i < 3; i++)
    {
        region.obj.nIndex = i+1; //一次取一相

        digit.wVal = rtData.nValue[i];

        sta_region_calc_new(digit, mainOAD, region.obj, regionPara.freqCnt, (storeTime-regionPara.lastTime), region,pn);
    }
}





/**
*********************************************************************
* @brief：      电压区间统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_region_currzero_calc_new(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_REGIONS_T regionPara,uint16 pn)
{
    RECORD_OAD_T recordOAD={0};
    int32 rtData;     //当前实时数据
    int i = 0;
    OOP_DIGIT_T digit = {0};
    uint32 storeTm;
    int ret;
    
    recordOAD.road.oadMain.value=0x00;
    recordOAD.road.oadCols.nNum=1;
    recordOAD.road.oadCols.oad[0].value=0x20010400;
    recordOAD.classtag =5;
    recordOAD.logicId =0;
    recordOAD.infoNum = pn;

    STAT_FMT_DEBUG("pn is %d\n",pn);
    memset((uint8*)&rtData,0x00,sizeof(rtData));
    //读取最近一次电压数据
    ret=sta_record_read_new(g_staUdpHandle,&recordOAD, &storeTm,(uint8*)&rtData,sizeof(int32));
    if(ret!=0)
    {
        STAT_BUF_DEBUG((uint8*)&rtData,sizeof(int32),"读取结果 ret is %d,pn %d\n",ret,pn);
        return;
    }


    digit.choice = DT_DOUBLE_LONG;
    STA_REGIONOBJ_T region;
    region = regionPara.objs[index];
    //三相
    for(i = 0; i < 1; i++)
    {
        region.obj.value = 0x20010400; //一次取一相

        digit.wVal = rtData;

        sta_region_calc_new(digit, mainOAD, region.obj, regionPara.freqCnt, (storeTime-regionPara.lastTime), region,pn);
    }
}

/**
*********************************************************************
* @brief：      执行区间统计
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_region_execute_new(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, STA_REGIONS_T regionPara)
{
    uint32 i = 0;
    OOP_OAD_U oad;
    OOP_METER_T oopMeter;
    uint32 j=0;
    mainOAD.value += 0x0200;

    for(j = 0; j < oopMeterList.oopMeterNum; j++)
    {
        if(app_get_meter_list_by_id(j, &oopMeter) == 0)
        {
            if((oopMeter.basic.port.nObjID!=0xF201)&&(oopMeter.basic.port.nObjID!=0xF208))
            {
                continue;
            }
        }
        for(i = 0; i < regionPara.nNum; i++)
        {
            oad.value = regionPara.objs[i].obj.value;

            //区间统计目前只支持 电压，电流，电流不平衡率，视在功率
            if(sta_region_slave_check(oad))
            {
                //TODO 块类型要拆分出子项
                switch(oad.value)
                {   
                    case 0x20010200: //三相电流(int32)
                    {
                        sta_region_curr_calc_new(mainOAD, curTime, storeTime, i, regionPara,oopMeter.nIndex);

                    }break;
                    case 0x20040200: //有功功率（总及分相（int32））
                    {
                        sta_region_power_calc_new(mainOAD, curTime, storeTime, i, regionPara,oopMeter.nIndex);

                    }break;
                    case 0x20010400: //三相电流(int32)
                    {
                        sta_region_currzero_calc_new(mainOAD, curTime, storeTime, i, regionPara,oopMeter.nIndex);

                    }break;
                    case 0x20000200: //三相电压(uint16)
                    {
                        sta_region_volt_calc_new(mainOAD, curTime, storeTime, i, regionPara,oopMeter.nIndex);

                    }break;
                    case 0x20270200: //电流不平衡率(uint16)
                    {
                        sta_region_imbalance_calc(mainOAD, curTime, storeTime, i, regionPara);

                    }break;
                    case 0x20050200: //无功功率（总及分相（int32））
                    {
                        sta_region_power_calc(mainOAD, curTime, storeTime, i, regionPara);

                    }break;                 
                    case 0x20060200: //视在功率（总及分相（int32））
                    {
                        sta_region_apparent_power_calc_new(mainOAD, curTime, storeTime, i, regionPara,oopMeter.nIndex);

                    }break; 
                    case 0x200A0200: //功率因数（总及分相（int16））
                    case 0x200A0201: //功率因数（总（int16））
                    {
                        sta_region_powerfac_calc_new(mainOAD, curTime, storeTime, i, regionPara,oopMeter.nIndex);
                        if(oopMeter.basic.port.nObjID==0xF208)
                        {
                            sta_region_powerfac_calc(mainOAD, curTime, storeTime, i, regionPara);
                        }

                    }break;                 
                    default:STAT_FMT_DEBUG("region object func not exit. OAD(0x%08x)\n", oad.value); break;
                }
            }   
            else
            {
                STAT_FMT_DEBUG("region object not support. nObjID(%d)\n", oad.nObjID);
            }
        }
    }
}

/**
*********************************************************************
* @brief：      区间统计处理
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_region_proc_new()
{
    uint32 i = 0;
    uint32 j = 0;
    STA_REGIONS_PARA_T *tmpNode = NULL;
    time_t nextTime = 0;
    time_t curTime;
    OOP_TI_SPAN_E time_span;

    DT_DateTimeShort2Time(&realstattime, &curTime);

    /*区间统计类型（分，时，日，月，年）5种*/
    for(i = 0; i < STA_REGION_OBJNUMS; i++)
    {
        if(g_staRegionsPara[i].blockNum == 0)
        {
            continue;
        }
        
        tmpNode = &g_staRegionsPara[i];

        sta_interval_analyse(tmpNode->oad, &time_span);

        //每个block中的对象具有相同的freq和period
        for(j = 0; j < tmpNode->blockNum; j++)
        {
            sta_nexttime_update(tmpNode->para[j].freq, tmpNode->para[j].lastTime, &nextTime);

            /* 当前时间初始是比下次时间大的，当下次时间比它小时，说明时间到了，满足统计条件，执行统计*/
            if((nextTime!=0) && (nextTime<=curTime))
            {
                sta_region_execute_new(tmpNode->oad, curTime, nextTime, tmpNode->para[j]);
                tmpNode->para[j].lastTime = nextTime;
                tmpNode->para[j].freqCnt++;  
            }

            //刷新周期数
            sta_period_cnt_update(time_span, tmpNode->para[j].period, &tmpNode->para[j].count, &tmpNode->para[j].freqCnt);     
        }
        
    }    
    
    return;
}

#endif

#if DESC("累加平均统计处理函数",1)
/**
*********************************************************************
* @brief：      累加平均类型分析
* @param[in]： buffer  报文缓冲区
* @param[in]： offset  缓冲区中的偏移
* @param[out]: value   解析后的值
* @return：     void
*********************************************************************
*/
void sta_totalavg_construct(OOP_DIGIT_T rtData, uint32 freqCnt, OOP_TOTALAVG_T *result)
{
    switch(rtData.choice)
    {  
        case DT_DOUBLE_LONG: //double long/int32
        {
            result->dbTotal += rtData.nVal;
            
        }break;
        case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
        {
            result->dbTotal += rtData.dwVal;
            
        }break;     
        case DT_INTEGER: //integer/char
        {
            result->dbTotal += rtData.cVal;

        }break;
        case DT_LONG: //long/int16
        {
            result->dbTotal += rtData.lVal;

        }break; 
        case DT_UNSIGNED: //unsigned/uint8
        {
            result->dbTotal += rtData.bVal;
            
        }break;   
        case DT_LONG_UNSIGNED: //long unsigned/uint16
        {
            result->dbTotal += rtData.wVal;
     
        }break;   
        case DT_LONG64: //long64/int64
        {
            result->dbTotal += rtData.llVal;
                    
        }break;   
        case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
        {
            result->dbTotal += rtData.ullVal;
            
        }break;
        default:break;
    }
    
    result->dbAvg = result->dbTotal/(freqCnt + 1);
    
    return;      
}

/**
*********************************************************************
* @brief：      累加平均统计计算(通用计算)
* @param[in]： rtData  参数计算的数据(后续改为 OOP_DIGIT_T 类型，以支持泛型)
* @param[out]：outData 计算结果
* @return：     void
*********************************************************************
*/
void sta_totalavg_calc(OOP_DIGIT_T rtData, OOP_OAD_U mainOAD, OOP_OAD_U subOAD, uint32 count, time_t curTime)
{
    int ret = ERR_OK;
    OOP_TOTALAVG_T result = {{0}};  //一个累加平均统计结果
    READ_RECORD_T readRecord = {{0}};
    RECORD_UNIT_T recordBuf = {0};
    WRITE_RECORD_T writeRecord = {{0}};
    uint32 storeNo = 0;
    uint32 len = sizeof(OOP_TOTALAVG_T);
    UPDATE_RECORD_T updateRecord;
    bool isUpdate = FALSE;
    uint8 *pAddr = NULL;

    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&writeRecord, sizeof(writeRecord));
    MEMZERO(&updateRecord, sizeof(updateRecord));

    result.obj.value = subOAD.value;

    //获取终端地址
    app_get_localaddr(&pAddr);

    // 校验码，通信地址的前6个字节
    readRecord.MAC.nNum = 6;
    memcpy(readRecord.MAC.value, pAddr, 6);
/*    
    writeRecord.MAC.nNum = 6;
    memcpy(writeRecord.MAC.value, pAddr, 6);

    updateRecord.MAC.nNum = 6;
    memcpy(updateRecord.MAC.value, pAddr, 6);
*/
    /* 首次达到统计周期，初始化计算数据*/    
    if(count == 0)
    {    
        writeRecord.recordOAD.road.oadMain.value = mainOAD.value;
        writeRecord.recordOAD.road.oadCols.nNum = 1;
        writeRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;
        writeRecord.inDatalen = len;
        writeRecord.colStoreTm = time(NULL) + COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间
               
        memset(writeRecord.inDataBuf, 0, len);
        
        ret= db_write_record(g_staUdpHandle, &writeRecord, &storeNo);   
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("totalavg first write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), len(%d)\n", 
               ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, len);
        }

        return;
    }
      
    //读取历史统计数据
    readRecord.recordOAD.road.oadMain.value = mainOAD.value;
    readRecord.recordOAD.road.oadCols.nNum = 1;
    readRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;     

    //TODO 实现初始化，失败不返回，因为第一次读的时候会失败
    ret = db_read_record(g_staUdpHandle, &readRecord, sizeof(result), &recordBuf);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("totalavg history read failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
           ret, readRecord.recordOAD.road.oadMain.value, readRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
    }
    else
    {
        isUpdate = TRUE;
        
        if(len != recordBuf.record[0].outDatalen)
        {
            STAT_FMT_DEBUG("len(%d) != outDatelen(%d)\n", len, recordBuf.record[0].outDatalen);
        }

        memcpy((uint8*)&result, recordBuf.record[0].outDataBuf, len);
    }

    sta_totalavg_construct(rtData, count, &result);
    
    //累积完成后回写
    if(isUpdate)
    {
        updateRecord.filter.choice = 1;
        updateRecord.recordOAD.road.oadMain.value = mainOAD.value;
        updateRecord.recordOAD.road.oadCols.nNum = 1;
        updateRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;
        updateRecord.inDatalen = len;
        updateRecord.filter.storeTm = recordBuf.colStoreTm;

        //传出时，按结构体强转
        memcpy(updateRecord.inDataBuf, (uint8*)&result, len);

        ret = db_update_record(g_staUdpHandle, &updateRecord);   
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("region update failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
               ret, updateRecord.recordOAD.road.oadMain.value, updateRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
        }
    }
    else
    {
        writeRecord.recordOAD.road.oadMain.value = mainOAD.value;
        writeRecord.recordOAD.road.oadCols.nNum = 1;
        writeRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;
        writeRecord.inDatalen = len;
        writeRecord.colStoreTm = time(NULL) + COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间

        //传出时，按结构体强转
        memcpy(writeRecord.inDataBuf, (uint8*)&result, len);

        ret = db_write_record(g_staUdpHandle, &writeRecord, &storeNo);   
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("region write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
               ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
        }
    }

    return;
}

/**
*********************************************************************
* @brief：      添加累加平均统计参数节点
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_totalavg_node_add(STA_TOTALAVGS_PARA_T *pstPara)
{
    uint8 i = 0,j=0;
    int ret = ERR_OK;
    OOP_COGNATES_T subObjs;
    uint32 nNum = 0;
    OOP_OAD_U mainObj;
    STA_TOTALAVGS_T *block = NULL;
    NOMAL_OAD_T nomalOAD = {0};
    uint32 len = 0;

    mainObj.value = pstPara->oad.value + 0x0300;
    block = pstPara->para;
    
    /*使用mainObj读取累加平均统计参数*/
    nomalOAD.logicId = 0;
    nomalOAD.oad.value = mainObj.value;

    /* 获取统计数据源 */
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_COGNATES_T), (uint8*)&subObjs, &len);
    if(ret != 0 )
    {
        STAT_FMT_DEBUG("totalavg param read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_COGNATES_T));
        return;
    }

    //先清空block
    memset(block, 0, sizeof(STA_TOTALAVGS_T));
    pstPara->blockNum = 0;

    if(subObjs.nNum > OOP_COGNATE_OBJS)
    {
        STAT_FMT_DEBUG("totalavg Illegal subObjs.nNum(%u)\n", subObjs.nNum);
        return;
    }
    
    /*读出的参数按统计周期相同，重新整理分布*/
    for(i = 0; i < subObjs.nNum; i++)
    {
        for(j = 0; j< OOP_COGNATE_OBJS; j++)
        {
            nNum = block[j].nNum;
            
            /*组中关联对象为0，或者成员的统计周期与间隔相同，则新关联对象加入这个组*/
            if((nNum == 0) || ((block[j].period == subObjs.objs[i].period)
                && sta_time_interval_equal(block[j].freq,subObjs.objs[i].freq)))
            {
                if(nNum == 0)
                {
                    pstPara->blockNum++;   
                }
                
                block[j].period = subObjs.objs[i].period;
                block[j].freq = subObjs.objs[i].freq;
                block[j].objs[nNum] = subObjs.objs[i].obj;
                block[j].nNum++;
                
                break;
            }          
        }
    }

    //初始化上一次时间
    for(i = 0; i < pstPara->blockNum; i++)
    {
        sta_last_time_init(pstPara->para[i].freq, &pstPara->para[i].lastTime);
    }

    return;
}

/**
*********************************************************************
* @brief：      累加平均统计关联OAD的校验
* @param[in]： oad         待校验的oad
* @return：     TRUE-校验通过，FALSE-校验不通过
*********************************************************************
*/
BOOL sta_totalavg_slave_check(OOP_OAD_U oad)
{
    //电压，电流，视在功率
    if((oad.nObjID == 0x2000) || (oad.nObjID == 0x2001) 
        || (oad.nObjID == 0x2006))
    {
        return TRUE;
    }

    return FALSE;
}

/**
*********************************************************************
* @brief：      电压累加平均统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_totalavg_volt_calc(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_TOTALAVGS_T totalavgPara)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_WORD3V_T rtData;     //当前实时数据
    uint32 len = 0;
    int i = 0;
    OOP_DIGIT_T digit = {0};
    
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = totalavgPara.objs[index].value;

    /* 获取统计数据源 */
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_WORD3V_T), (uint8*)&rtData, &len);
    if(ret < 0)
    {
        STAT_FMT_DEBUG("totalavg volt read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_WORD3V_T));
    
        return;
    }

    digit.choice = DT_LONG_UNSIGNED;
    
    //三相
    for(i = 0; i < 3; i++)
    {
        totalavgPara.objs[index].nIndex = i+1; //一次取一相
        
        digit.wVal = rtData.nValue[i];

        sta_totalavg_calc(digit, mainOAD, totalavgPara.objs[index], totalavgPara.freqCnt ,curTime);
    }
    return;   
}

/**
*********************************************************************
* @brief：      执行累加平均统计
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_totalavg_execute(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, STA_TOTALAVGS_T regionPara)
{
    uint32 i = 0;
    OOP_OAD_U oad;

    mainOAD.value += 0x0200;
    
    //遍历同周期和间隔的OAD
    for(i = 0; i < regionPara.nNum; i++)
    {
        oad.value = regionPara.objs[i].value;

        //累加平均统计目前只支持 电压
        if(sta_region_slave_check(oad))
        {
            //TODO 块类型要拆分出子项
            switch(oad.value)
            {   
                case 0x20000200: //三相电压(uint16)
                {
                    sta_totalavg_volt_calc(mainOAD, curTime, storeTime, i, regionPara);

                }break;
                
                default:STAT_FMT_DEBUG("totalavg object func not exit. OAD(0x%08x)\n", oad.value); break;
            }
        }
        else
        {
            STAT_FMT_DEBUG("totalavg object not support. nObjID(%d)\n", oad.nObjID);
        }
    }    
    return;      
}

/**
*********************************************************************
* @brief：     累加平均统计初始化
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_totalavg_init(void)
{
    uint8 i = 0;
    uint8 avgInitFlag = 0;

    pthread_mutex_lock(&g_staUpdateData.mutex);

    /*读取累加平均统计参数变更标记*/
    avgInitFlag = g_staUpdateData.avgInitFlag;

    for(i = 0; i < STA_TOTALAVG_OBJNUMS; i++)
    {
        if(avgInitFlag&(1UL<<i))
        {
            sta_totalavg_node_add(&g_staTotalavgsPara[i]);
            avgInitFlag &= ~(1UL<<i);
        }
    }
    
    /*存储累加平均统计参数变更标记*/
    g_staUpdateData.avgInitFlag = avgInitFlag;  

    pthread_mutex_unlock(&g_staUpdateData.mutex);    

    return;
}

/**
*********************************************************************
* @brief：      统计时间刷新
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_totalavg_lasttime_reinit(void)
{
    uint8 i = 0, j = 0;
    STA_TOTALAVGS_PARA_T *pstPara = NULL;

    for(i = 0; i < STA_TOTALAVG_OBJNUMS; i++)
    {
        if(g_staTotalavgsPara[i].blockNum == 0)
        {
            continue;
        }
            
        pstPara = &g_staTotalavgsPara[i];

        STAT_FMT_DEBUG("totalavg last time reinit. oda(0x%08x)\n", pstPara->oad.value);
        
        //初始化上一次时间
        for(j = 0; j < pstPara->blockNum; j++)
        {
            sta_last_time_init(pstPara->para[j].freq, &pstPara->para[j].lastTime);
        }
    }
    
    return;
}

/**
*********************************************************************
* @brief：      累加平均统计处理
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_totalavg_proc()
{
    uint32 i = 0;
    uint32 j = 0;
    STA_TOTALAVGS_PARA_T *tmpNode = NULL;
    time_t nextTime = 0;
    time_t curTime;  
    OOP_TI_SPAN_E time_span;

    DT_DateTimeShort2Time(&realstattime, &curTime);

    /*区间统计类型（分，时，日，月，年）5种*/
    for(i = 0; i < STA_TOTALAVG_OBJNUMS; i++)
    {
        if(g_staTotalavgsPara[i].blockNum == 0)
        {
            continue;
        }
        tmpNode = &g_staTotalavgsPara[i];

        sta_interval_analyse(tmpNode->oad, &time_span);

        //每个block中的对象具有相同的freq和period
        for(j = 0; j < tmpNode->blockNum; j++)
        {

            sta_nexttime_update(tmpNode->para[j].freq, tmpNode->para[j].lastTime, &nextTime);
            /* 当前时间初始是比下次时间大的，当下次时间比它小时，说明时间到了，满足统计条件，执行统计*/
            if((nextTime!=0) && (nextTime<=curTime))
            {
                sta_totalavg_execute(tmpNode->oad, curTime, nextTime, tmpNode->para[j]);

                tmpNode->para[j].lastTime = nextTime;
                tmpNode->para[j].freqCnt++;                
            }

            //刷新周期数(达到周期数，下次统计时，先清历史数据，如日统计，周期2天，那么count=2时清历史数据)
            sta_period_cnt_update(time_span, tmpNode->para[j].period, &tmpNode->para[j].count, &tmpNode->para[j].freqCnt); 
        }
        
    }    
    return;      
}

#endif

#if DESC("新累加平均统计处理函数",1)

/**
*********************************************************************
* @brief：      累加平均统计计算(通用计算)
* @param[in]： rtData  参数计算的数据(后续改为 OOP_DIGIT_T 类型，以支持泛型)
* @param[out]：outData 计算结果
* @return：     void
*********************************************************************
*/
void sta_totalavg_calc_new(OOP_DIGIT_T rtData, OOP_OAD_U mainOAD, OOP_OAD_U subOAD, uint32 count, time_t savetime,uint16 pn)
{
    int ret = ERR_OK;
    OOP_TOTALAVG_T result = {{0}};  //一个累加平均统计结果
    READ_RECORD_T readRecord = {{0}};
    RECORD_UNIT_T recordBuf = {0};
    WRITE_RECORD_T writeRecord = {{0}};
    uint32 storeNo = 0;
    uint32 len = sizeof(OOP_TOTALAVG_T);
    UPDATE_RECORD_T updateRecord;
    bool isUpdate = FALSE;
    OOP_METER_T oopMeter;

    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&writeRecord, sizeof(writeRecord));
    MEMZERO(&updateRecord, sizeof(updateRecord));

    result.obj.value = subOAD.value;

    //获取终端地址
    app_get_meter_list_by_id(pn, &oopMeter);

    // 校验码，通信地址的前6个字节
    readRecord.MAC.nNum = 6;
    memcpy(readRecord.MAC.value, oopMeter.basic.tsa.add, 6);
/*    
    writeRecord.MAC.nNum = 6;
    memcpy(writeRecord.MAC.value, pAddr, 6);

    updateRecord.MAC.nNum = 6;
    memcpy(updateRecord.MAC.value, pAddr, 6);
*/
    /* 首次达到统计周期，初始化计算数据*/    
    if(count == 0)
    {    
        writeRecord.recordOAD.road.oadMain.value = mainOAD.value;
        writeRecord.recordOAD.road.oadCols.nNum = 1;
        writeRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;
        writeRecord.inDatalen = len;
        writeRecord.colStoreTm = savetime; // 注：+8个小时(28800)转成格林时间
               
        memset(writeRecord.inDataBuf, 0, len);
        
        ret= db_write_record(g_staUdpHandle, &writeRecord, &storeNo);   
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("totalavg first write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), len(%d)\n", 
               ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, len);
        }

        return;
    }
      
    //读取历史统计数据
    readRecord.recordOAD.road.oadMain.value = mainOAD.value;
    readRecord.recordOAD.road.oadCols.nNum = 1;
    readRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;     

    //TODO 实现初始化，失败不返回，因为第一次读的时候会失败
    ret = db_read_record(g_staUdpHandle, &readRecord, sizeof(result), &recordBuf);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("totalavg history read failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
           ret, readRecord.recordOAD.road.oadMain.value, readRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
    }
    else
    {
        isUpdate = TRUE;
        
        if(len != recordBuf.record[0].outDatalen)
        {
            STAT_FMT_DEBUG("len(%d) != outDatelen(%d)\n", len, recordBuf.record[0].outDatalen);
        }

        memcpy((uint8*)&result, recordBuf.record[0].outDataBuf, len);
    }

    sta_totalavg_construct(rtData, count, &result);
    
    //累积完成后回写
    if(isUpdate)
    {
        updateRecord.filter.choice = 1;
        updateRecord.recordOAD.road.oadMain.value = mainOAD.value;
        updateRecord.recordOAD.road.oadCols.nNum = 1;
        updateRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;
        updateRecord.inDatalen = len;
        updateRecord.filter.storeTm = recordBuf.colStoreTm;

        //传出时，按结构体强转
        memcpy(updateRecord.inDataBuf, (uint8*)&result, len);

        ret = db_update_record(g_staUdpHandle, &updateRecord);   
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("region update failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
               ret, updateRecord.recordOAD.road.oadMain.value, updateRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
        }
    }
    else
    {
        writeRecord.recordOAD.road.oadMain.value = mainOAD.value;
        writeRecord.recordOAD.road.oadCols.nNum = 1;
        writeRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;
        writeRecord.inDatalen = len;
        writeRecord.colStoreTm = savetime; // 注：+8个小时(28800)转成格林时间

        //传出时，按结构体强转
        memcpy(writeRecord.inDataBuf, (uint8*)&result, len);

        ret = db_write_record(g_staUdpHandle, &writeRecord, &storeNo);   
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("region write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
               ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
        }
    }

    return;
}


/**
*********************************************************************
* @brief：      累加平均统计计算(通用计算)
* @param[in]： rtData  参数计算的数据(后续改为 OOP_DIGIT_T 类型，以支持泛型)
* @param[out]：outData 计算结果
* @return：     void
*********************************************************************
*/
void sta_totalavg_calc_new1(OOP_DIGIT_T rtData, OOP_OAD_U mainOAD, OOP_OAD_U subOAD, uint32 count, time_t savetime,uint16 pn)
{
    int ret = ERR_OK;
    OOP_TOTALAVGS_T result;  //一个累加平均统计结果
    READ_RECORD_T readRecord = {{0}};
    WRITE_RECORD_T writeRecord = {{0}};
    uint32 storeNo = 0;
    uint32 len = sizeof(OOP_TOTALAVGS_T);
    OOP_METER_T oopMeter;
    NOMAL_OAD_T ptOAD;
    uint16 i=0,j=0;;
    OOP_OAD_U temmainoad;
    uint32 outlen=0;
    time_t realtime=time(NULL);
    OOP_DATETIME_S_T dateTime;
    time_t linuxTim;

    MEMZERO(&result,sizeof(result));
    MEMZERO(&writeRecord, sizeof(writeRecord));
    ptOAD.logicId=0;
    ptOAD.oad = mainOAD;
    ptOAD.infoNum=pn;

    STAT_FMT_DEBUG("pn is %d\n",pn);
    //获取地址
    app_get_meter_list_by_id(pn, &oopMeter);

    ret=db_read_nomal(g_staUdpHandle,&ptOAD,sizeof(result),(uint8*)&result,&outlen);

    //STAT_BUF_DEBUG((uint8*)&result,sizeof(result),"读取结果 ret is %d\n",ret);
    if((ret != ERR_OK)||(sizeof(result)!=outlen))
    {
        STAT_FMT_DEBUG("读取累加平均失败 ret is %d,outlen is %d\n",ret,outlen);
        MEMZERO(&result,sizeof(result));
    }
    STAT_FMT_DEBUG("result.nNum is %d\n",result.nNum);
    for(j=0;j<result.nNum;j++)
    {
        STAT_FMT_DEBUG("result.totalavg[j].obj.value is %x\n",result.totalavg[j].obj.value);
        STAT_FMT_DEBUG("result.totalavg[j].dbTotal is %x\n",result.totalavg[j].dbTotal);
        STAT_FMT_DEBUG("result.totalavg[j].dbAvg is %x\n",result.totalavg[j].dbAvg);
    }

    for(i=0;i<result.nNum;i++)
    {
        if(result.totalavg[i].obj.value == subOAD.value)
        {
            break;
        }
    }
    if(i>=result.nNum)
    {
        result.nNum++;
        result.totalavg[i].obj.value = subOAD.value;
    }

    sta_totalavg_construct(rtData, count, &result.totalavg[i]);

    STAT_FMT_DEBUG("result.totalavg[i].obj.value is %x,累加值  is %d,平均值 is %d\n",result.totalavg[i].obj.value,result.totalavg[i].dbTotal,result.totalavg[i].dbAvg);
    // 校验码，通信地址的前6个字节
    readRecord.MAC.nNum = 6;
    memcpy(readRecord.MAC.value, oopMeter.basic.tsa.add, 6);

    STAT_FMT_DEBUG("count is %d,savetime is %d\n",count,savetime);
    /* 首次达到统计周期，初始化计算数据*/   
    STAT_FMT_DEBUG("result.nNum is %d\n",result.nNum);
    for(j=0;j<result.nNum;j++)
    {
        STAT_FMT_DEBUG("result.totalavg[j].obj.value is %x\n",result.totalavg[j].obj.value);
        STAT_FMT_DEBUG("result.totalavg[j].dbTotal is %x\n",result.totalavg[j].dbTotal);
        STAT_FMT_DEBUG("result.totalavg[j].dbAvg is %x\n",result.totalavg[j].dbAvg);
    }
    if(pastdayflag)
    {    
        if(mainOAD.value==0x21120200)
        {
            temmainoad.value =0x50040200;
        }
        else if(mainOAD.value==0x21130200)
        {
            temmainoad.value =0x50060200;
        }

        //首次到达清零
        ret=db_write_nomal(g_staUdpHandle,&ptOAD,(uint8*)&result,sizeof(result));
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("刷新累加累加平均数据失败 ret is %d\n",ret);
        }
        if(subOAD.value ==0x20000203)
        {
            realtime=time(NULL);
            writeRecord.recordOAD.infoNum = pn;
            writeRecord.recordOAD.road.oadMain.value = temmainoad.value;
            writeRecord.recordOAD.road.oadCols.nNum = 1;
            writeRecord.recordOAD.road.oadCols.oad[0].value = mainOAD.value;
            writeRecord.inDatalen = len;
            writeRecord.colStartTm =(uint32)realtime+COMPENSATION_TIME;
            writeRecord.colEndTm =(uint32)realtime+COMPENSATION_TIME;
            DT_Time2DateTimeShort(realtime,&dateTime);
            dateTime.minute=0x00;
            dateTime.second=0x00;
            DT_DateTimeShort2Time(&dateTime,&linuxTim);
            writeRecord.colStoreTm = (uint32)linuxTim+COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间
            STAT_FMT_DEBUG("writeRecord.colStartTm(%d) writeRecord.colEndTm(%d), writeRecord.colStoreTm(%d)\n", 
                   writeRecord.colStartTm, writeRecord.colEndTm, writeRecord.colStoreTm);
            //传出时，按结构体强转
            memcpy(writeRecord.inDataBuf, (uint8*)&result, len);
            
            STAT_FMT_DEBUG("清除累加平均数\n");
            memset((uint8*)&result,0x00,sizeof(result));
            ret=db_write_nomal(g_staUdpHandle,&ptOAD,(uint8*)&result,sizeof(result));
            if(ret != ERR_OK)
            {
                STAT_FMT_DEBUG("清除累加平均数据失败 ret is %d\n",ret);
            }
       
            ret= db_write_record(g_staUdpHandle, &writeRecord, &storeNo);   
            if(ret != ERR_OK)
            {
                STAT_FMT_DEBUG("totalavg first write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), len(%d)\n", 
                   ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, len);
            }

            return;
        }
    }
    else
    {
        ret=db_write_nomal(g_staUdpHandle,&ptOAD,(uint8*)&result,sizeof(result));
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("刷新累加累加平均数据失败 ret is %d\n",ret);
        }
    }
      


    return;
}

/**
*********************************************************************
* @brief：      电压累加平均统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_totalavg_volt_calc_new(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_TOTALAVGS_T 
totalavgPara,uint16 pn)
{
    RECORD_OAD_T recordOAD={0};
    OOP_WORD3V_T rtData;     //当前实时数据
    int i = 0;
    OOP_DIGIT_T digit = {0};
    uint32 storeTm;
    int ret;
    
    recordOAD.road.oadMain.value=0x00;
    recordOAD.road.oadCols.nNum=1;
    recordOAD.road.oadCols.oad[0].value=0x20000200;
    recordOAD.classtag =5;
    recordOAD.logicId =0;
    recordOAD.infoNum = pn;

    memset((uint8*)&rtData,0x00,sizeof(rtData));
    //读取最近一次电压数据
    ret=sta_record_read_new(g_staUdpHandle,&recordOAD, &storeTm,(uint8*)&rtData,sizeof(OOP_WORD3V_T));
    if(ret !=0)
    {
        STAT_BUF_DEBUG((uint8*)&rtData,sizeof(OOP_WORD3V_T),"读取结果 ret is %d,pn %d\n",ret,pn);
        return;
    }
    digit.choice = DT_LONG_UNSIGNED;
    
    //三相
    for(i = 0; i < 3; i++)
    {
        totalavgPara.objs[index].nIndex = i+1; //一次取一相
        
        digit.wVal = rtData.nValue[i];

        sta_totalavg_calc_new1(digit, mainOAD, totalavgPara.objs[index], totalavgPara.freqCnt ,storeTm,pn);
    }
    return;   
}

/**
*********************************************************************
* @brief：      执行累加平均统计
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_totalavg_execute_new(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, STA_TOTALAVGS_T regionPara)
{
    uint32 i = 0;
    uint32 j=0;
    OOP_OAD_U oad;
    OOP_METER_T oopMeter;

    mainOAD.value += 0x0200;

    for(j = 0; j < oopMeterList.oopMeterNum; j++)
    {
        if(app_get_meter_list_by_id(j, &oopMeter) == 0)
        {
            if((oopMeter.basic.port.nObjID!=0xF201)&&(oopMeter.basic.port.nObjID!=0xF208))
            {
                continue;
            }
        }
        //遍历同周期和间隔的OAD
        for(i = 0; i < regionPara.nNum; i++)
        {
            oad.value = regionPara.objs[i].value;

            //累加平均统计目前只支持 电压
            if(sta_region_slave_check(oad))
            {
                //TODO 块类型要拆分出子项
                switch(oad.value)
                {   
                    case 0x20000200: //三相电压(uint16)
                    {
                        sta_totalavg_volt_calc_new(mainOAD, curTime, storeTime, i, regionPara,oopMeter.nIndex);

                    }break;
                    
                    default:STAT_FMT_DEBUG("totalavg object func not exit. OAD(0x%08x)\n", oad.value); break;
                }
            }
            else
            {
                STAT_FMT_DEBUG("totalavg object not support. nObjID(%d)\n", oad.nObjID);
            }
        }    
    }
    return;      
}

/**
*********************************************************************
* @brief：      累加平均统计处理
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_totalavg_proc_new()
{
    uint32 i = 0;
    uint32 j = 0;
    STA_TOTALAVGS_PARA_T *tmpNode = NULL;
    time_t nextTime = 0;
    time_t curTime;  
    OOP_TI_SPAN_E time_span;
    uint8 flag=FALSE;

    DT_DateTimeShort2Time(&realstattime, &curTime);

    /*区间统计类型（分，时，日，月，年）5种*/
    for(i = 0; i < STA_TOTALAVG_OBJNUMS; i++)
    {
        if(g_staTotalavgsPara[i].blockNum == 0)
        {
            continue;
        }
        tmpNode = &g_staTotalavgsPara[i];

        sta_interval_analyse(tmpNode->oad, &time_span);

        //每个block中的对象具有相同的freq和period
        for(j = 0; j < tmpNode->blockNum; j++)
        {

            sta_nexttime_update(tmpNode->para[j].freq, tmpNode->para[j].lastTime, &nextTime);
            /* 当前时间初始是比下次时间大的，当下次时间比它小时，说明时间到了，满足统计条件，执行统计*/
            if((nextTime!=0) && (nextTime<=curTime))
            {
                if(datainitfalg==TRUE)
                {
                    tmpNode->para[j].freqCnt=0;
                    flag=TRUE;
                    STAT_FMT_DEBUG("数据初始化\n");
                }
                sta_totalavg_execute_new(tmpNode->oad, curTime, nextTime, tmpNode->para[j]);

                tmpNode->para[j].lastTime = nextTime;
                tmpNode->para[j].freqCnt++;    
            }

            //刷新周期数(达到周期数，下次统计时，先清历史数据，如日统计，周期2天，那么count=2时清历史数据)
            sta_period_cnt_update(time_span, tmpNode->para[j].period, &tmpNode->para[j].count, &tmpNode->para[j].freqCnt); 
        }
        if((datainitfalg==TRUE)&&(flag==TRUE))
        {
            datainitfalg=FALSE;
            flag=FALSE;
        }
        
    }    
    return;      
}

#endif


#if DESC("极值统计处理函数",1)

/**
*********************************************************************
* @brief：      数值小值类型解析
* @param[in]： buffer  报文缓冲区
* @param[in]： offset  缓冲区中的偏移
* @param[out]: value   解析后的值
* @return：     void
*********************************************************************
*/
void sta_extreme_min_construct(OOP_DIGIT_T rtData, time_t curTime, OOP_DIGIT_T *value, OOP_DATETIME_S_T *time)
{
    uint8 changedFlag = 0;

    switch(rtData.choice)
    {  
        case DT_DOUBLE_LONG: //double long/int32
        {
            if(rtData.nVal < value->nVal)
            {
                changedFlag = 1;
                value->nVal = rtData.nVal;
            }       
        }break;
        case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
        {
            if(rtData.dwVal < value->dwVal)
            {
                changedFlag = 1;
                value->dwVal = rtData.dwVal;
            }

        }break;     
        case DT_INTEGER: //integer/char
        {
            if(rtData.cVal < value->cVal)
            {
                changedFlag = 1;
                value->cVal = rtData.cVal;
            }

        }break;
        case DT_LONG: //long/int16
        {
            if(rtData.lVal < value->lVal)
            {
                changedFlag = 1;
                value->lVal = rtData.lVal;
            }

        }break; 
        case DT_UNSIGNED: //unsigned/uint8
        {
            if(rtData.bVal < value->bVal)
            {
                changedFlag = 1;
                value->bVal = rtData.bVal;
            }

        }break;   
        case DT_LONG_UNSIGNED: //long unsigned/uint16
        {
            if(rtData.wVal < value->wVal)
            {
                changedFlag = 1;
                value->wVal = rtData.wVal;
            }      
        }break;   
        case DT_LONG64: //long64/int64
        {
            if(rtData.llVal < value->llVal)
            {
                changedFlag = 1;
                value->llVal = rtData.llVal;
            }          
        }break;   
        case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
        {
            if(rtData.ullVal < value->ullVal)
            {
                changedFlag = 1;
                value->ullVal = rtData.ullVal;
            }       
        }break;
        default:break;
    }
    value->choice = rtData.choice;
    if(changedFlag)
    {
        DT_Time2DateTimeShort(curTime, time);
    }
    
    return;      
}

/**
*********************************************************************
* @brief：      数值类型解析
* @param[in]： buffer  报文缓冲区
* @param[in]： offset  缓冲区中的偏移
* @param[out]: value   解析后的值
* @return：     void
*********************************************************************
*/
void sta_extreme_max_construct(OOP_DIGIT_T rtData, time_t curTime, OOP_DIGIT_T *value, OOP_DATETIME_S_T *time)
{
    uint8 changedFlag = 0;

    switch(rtData.choice)
    {  
        case DT_DOUBLE_LONG: //double long/int32
        {
            if(rtData.nVal > value->nVal)
            {
                changedFlag = 1;
                value->nVal = rtData.nVal;
                value->choice = rtData.choice;
            }       
        }break;
        case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
        {
            if(rtData.dwVal > value->dwVal)
            {
                changedFlag = 1;
                value->dwVal = rtData.dwVal;
                value->choice = rtData.choice;
            }

        }break;     
        case DT_INTEGER: //integer/char
        {
            if(rtData.cVal > value->cVal)
            {
                changedFlag = 1;
                value->cVal = rtData.cVal;
                value->choice = rtData.choice;
            }

        }break;
        case DT_LONG: //long/int16
        {
            if(rtData.lVal > value->lVal)
            {
                changedFlag = 1;
                value->lVal = rtData.lVal;
                value->choice = rtData.choice;
            }

        }break; 
        case DT_UNSIGNED: //unsigned/uint8
        {
            if(rtData.bVal > value->bVal)
            {
                changedFlag = 1;
                value->bVal = rtData.bVal;
                value->choice = rtData.choice;
            }

        }break;   
        case DT_LONG_UNSIGNED: //long unsigned/uint16
        {
            if(rtData.wVal > value->wVal)
            {
                changedFlag = 1;
                value->wVal = rtData.wVal;
                value->choice = rtData.choice;
            }      
        }break;   
        case DT_LONG64: //long64/int64
        {
            if(rtData.llVal > value->llVal)
            {
                changedFlag = 1;
                value->llVal = rtData.llVal;
                value->choice = rtData.choice;
            }          
        }break;   
        case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
        {
            if(rtData.ullVal > value->ullVal)
            {
                changedFlag = 1;
                value->ullVal = rtData.ullVal;
                value->choice = rtData.choice;
            }       
        }break;
        default:break;
    }
    value->choice = rtData.choice;
    if(changedFlag)
    {
        DT_Time2DateTimeShort(curTime, time);  
    }
    
    return;      
}

/**
*********************************************************************
* @brief：      数值类型解析
* @param[in]： buffer  报文缓冲区
* @param[in]： offset  缓冲区中的偏移
* @param[out]: value   解析后的值
* @return：     void
*********************************************************************
*/
void sta_extreme_construct(OOP_DIGIT_T rtData, time_t curTime, OOP_EXTREMERES_T *result)
{
    //构造极大值结构
    sta_extreme_max_construct(rtData, curTime, &result->dbMaxValue, &result->tmMax);

    //构造极小值结构
    sta_extreme_min_construct(rtData, curTime, &result->dbMinValue, &result->tmMin);

    return;
}
/**
*********************************************************************
* @brief：      极值统计计算(通用计算)
* @param[in]： rtData  参数计算的数据(后续改为 OOP_DIGIT_T 类型，以支持泛型)
* @param[out]：outData 计算结果
* @return：     void
*********************************************************************
*/
void sta_extreme_calc(OOP_DIGIT_T rtData, OOP_OAD_U mainOAD, OOP_OAD_U subOAD, uint32 count, time_t curTime)
{
    int ret = ERR_OK;
    OOP_EXTREMERES_T result = {{0}};
    READ_RECORD_T readRecord = {{0}};
    RECORD_UNIT_T recordBuf = {0};
    WRITE_RECORD_T writeRecord;
    uint32 storeNo;
    uint32 len = sizeof(OOP_EXTREMERES_T);
    UPDATE_RECORD_T updateRecord;
    bool isUpdate = FALSE;
    uint8 *pAddr = NULL;

    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&writeRecord, sizeof(writeRecord));
    MEMZERO(&updateRecord, sizeof(updateRecord));

    result.obj.value = subOAD.value;

    //获取终端地址
    app_get_localaddr(&pAddr);

    // 校验码，通信地址的前6个字节
    readRecord.MAC.nNum = 6;
    memcpy(readRecord.MAC.value, pAddr, 6);
/*    
    writeRecord.MAC.nNum = 6;
    memcpy(writeRecord.MAC.value, pAddr, 6);

    updateRecord.MAC.nNum = 6;
    memcpy(updateRecord.MAC.value, pAddr, 6);
*/
    /* 首次达到统计周期，先清历史数据 */    
    if(count == 0)
    {
        writeRecord.recordOAD.road.oadMain.value = mainOAD.value;
        writeRecord.recordOAD.road.oadCols.nNum = 1;
        writeRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;
        writeRecord.inDatalen = sizeof(result);
        writeRecord.colStoreTm = time(NULL) + COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间

        //初始，最大最小值都设为当前值
        memcpy(&result.dbMaxValue, &rtData, sizeof(OOP_DIGIT_T));
        DT_Time2DateTimeShort(curTime, &result.tmMax);
        memcpy(&result.dbMinValue, &rtData, sizeof(OOP_DIGIT_T));
        DT_Time2DateTimeShort(curTime, &result.tmMin);  

        memcpy(writeRecord.inDataBuf, (uint8*)&result, sizeof(result));
        
        ret = db_write_record(g_staUdpHandle, &writeRecord, &storeNo);   
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("totalavg first write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), len(%d)\n", 
               ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
        }
        
        return;
    }
    
    //读取历史数据
    readRecord.recordOAD.road.oadMain.value = mainOAD.value;
    readRecord.recordOAD.road.oadCols.nNum = 1;
    readRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;
    //TODO 实现初始化，或者这里失败不返回，因为第一次读的时候会失败
    ret = db_read_record(g_staUdpHandle, &readRecord, sizeof(result), &recordBuf);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("extreme history read failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
           ret, readRecord.recordOAD.road.oadMain.value, readRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));

        //初始，最大最小值都设为当前值
        memcpy(&result.dbMaxValue, &rtData, sizeof(OOP_DIGIT_T));
        DT_Time2DateTimeShort(curTime, &result.tmMax);
        memcpy(&result.dbMinValue, &rtData, sizeof(OOP_DIGIT_T));
        DT_Time2DateTimeShort(curTime, &result.tmMin);
    }
    else
    {
        isUpdate = TRUE;
        
        if(len != recordBuf.record[0].outDatalen)
        {
            STAT_FMT_DEBUG("len(%d) != outDatelen(%d)\n", len, recordBuf.record[0].outDatalen);
        }

        memcpy((uint8*)&result, recordBuf.record[0].outDataBuf, len);
    }

    //构造极值结构
    sta_extreme_construct(rtData, curTime, &result);

    STAT_FMT_TRACE("[%d]: oad(0x%08x) dbMaxValue(%d),tmMax(%d), dbMinValue(%d), tmMin(%d)\n", count, subOAD.value, result.dbMaxValue.wVal,result.tmMax.minute,result.dbMinValue.wVal,result.tmMin.minute);

    //累积完成后回写
    if(isUpdate)
    {
        updateRecord.filter.choice = 1;
        updateRecord.recordOAD.road.oadMain.value = mainOAD.value;
        updateRecord.recordOAD.road.oadCols.nNum = 1;
        updateRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;
        updateRecord.inDatalen = len;
        updateRecord.filter.storeTm = recordBuf.colStoreTm;

        //传出时，按结构体强转
        memcpy(updateRecord.inDataBuf, (uint8*)&result, len);

        ret = db_update_record(g_staUdpHandle, &updateRecord);   
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("region update failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
               ret, updateRecord.recordOAD.road.oadMain.value, updateRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
        }
    }
    else
    {
        writeRecord.recordOAD.road.oadMain.value = mainOAD.value;
        writeRecord.recordOAD.road.oadCols.nNum = 1;
        writeRecord.recordOAD.road.oadCols.oad[0].value = subOAD.value;
        writeRecord.inDatalen = len;
        writeRecord.colStoreTm = time(NULL) + COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间

        //传出时，按结构体强转
        memcpy(writeRecord.inDataBuf, (uint8*)&result, len);

        ret = db_write_record(g_staUdpHandle, &writeRecord, &storeNo);   
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("region write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
               ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
        }
    }

    return;     
}

/**
*********************************************************************
* @brief：      添加极值统计参数节点
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_extreme_node_add(STA_EXTREMES_PARA_T *pstPara)
{
    uint8 i = 0,j=0;
    int ret = ERR_OK; 
    OOP_COGNATES_T subObjs;  //极值与累加统计参数结构一样
    uint32 nNum = 0;
    OOP_OAD_U mainObj;
    STA_EXTREMES_T *block = NULL;
    NOMAL_OAD_T nomalOAD = {0};
    uint32 len = 0;

    mainObj.value = pstPara->oad.value + 0x0300;
    block = pstPara->para;
    
    /*使用mainObj读取累加平均统计参数*/
    nomalOAD.logicId = 0;
    nomalOAD.oad.value = mainObj.value;

    /* 获取统计数据源 */
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_COGNATES_T), (uint8*)&subObjs, &len);
    if(ret != 0 )
    {
        STAT_FMT_DEBUG("extreme param read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_COGNATES_T));
        return;
    }

    //先清空数组
    memset(block, 0, sizeof(STA_EXTREMES_T));
    pstPara->blockNum = 0;

    if(subObjs.nNum > OOP_COGNATE_OBJS)
    {
        STAT_FMT_DEBUG("extreme Illegal subObjs.nNum(%u)\n", subObjs.nNum);
        return;
    }

    /*读出的参数按统计周期相同，重新整理分布*/
    for(i = 0; i < subObjs.nNum; i++)
    {
        for(j = 0; j< OOP_COGNATE_OBJS; j++)
        {
            nNum = block[j].nNum;
            
            /*组中关联对象为0，或者成员的统计周期与间隔相同，则新关联对象加入这个组*/
            if((nNum == 0) || ((block[j].period == subObjs.objs[i].period)
                && sta_time_interval_equal(block[j].freq,subObjs.objs[i].freq)))
            {
                if(nNum == 0)
                {
                    pstPara->blockNum++;   
                }
                
                block[j].period = subObjs.objs[i].period;
                block[j].freq = subObjs.objs[i].freq;
                block[j].objs[nNum] = subObjs.objs[i].obj;
                block[j].nNum++;

                break;
            }
        }
    }

    //初始化上一次时间
    for(i = 0; i < pstPara->blockNum; i++)
    {
        sta_last_time_init(pstPara->para[i].freq, &pstPara->para[i].lastTime);
    }
    
    return;      
}

/**
*********************************************************************
* @brief：      极值统计关联OAD的校验
* @param[in]： oad         待校验的oad
* @return：     TRUE-校验通过，FALSE-校验不通过
*********************************************************************
*/
BOOL sta_extreme_slave_check(OOP_OAD_U oad)
{
    //电压，电流，电压不平衡率，电流不平衡率，功率，功率因数，谐波含有率，总加有功功率
    if((oad.nObjID == 0x2000) || (oad.nObjID == 0x2001) 
        || (oad.nObjID == 0x2026)|| (oad.nObjID == 0x2027) 
        || (oad.nObjID == 0x2004)|| (oad.nObjID == 0x2005)
        || (oad.nObjID == 0x2006)|| (oad.nObjID == 0x200A)
        || (oad.nObjID == 0x200D)|| (oad.nObjID == 0x2301))

    {
        return TRUE;
    }

    return FALSE;
}

/**
*********************************************************************
* @brief：      电压极值统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_extreme_volt_calc(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_EXTREMES_T extremPara)
{
    NOMAL_OAD_T nomalOAD = {0};
    OOP_WORD3V_T rtData;     //当前实时数据
    uint32 len = 0;
    int i = 0;
    OOP_DIGIT_T digit = {0};
    
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = extremPara.objs[index].value;

    /* 获取统计数据源 */
    if(db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_WORD3V_T), (uint8*)&rtData, &len) <0 )
    {
        return;
    }

    digit.choice = DT_LONG_UNSIGNED;

    //三相
    for(i = 0; i < 3; i++)
    {

        extremPara.objs[index].nIndex = i+1; //一次取一相

        digit.wVal = rtData.nValue[i];
        
        sta_extreme_calc(digit, mainOAD, extremPara.objs[index], extremPara.freqCnt, curTime);

    }
    return;      
}

/**
*********************************************************************
* @brief：      电压极值统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_extreme_curr_calc(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_EXTREMES_T extremPara)
{
    NOMAL_OAD_T nomalOAD = {0};
    OOP_INT3V_T rtData;     //当前实时数据
    uint32 len = 0;
    int i = 0;
    OOP_DIGIT_T digit = {0};
    
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = extremPara.objs[index].value;

    /* 获取统计数据源 */
    if(db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_INT3V_T), (uint8*)&rtData, &len) <0 )
    {
        return;
    }

    digit.choice = DT_DOUBLE_LONG;

    //三相
    for(i = 0; i < 3; i++)
    {

        extremPara.objs[index].nIndex = i+1; //一次取一相

        digit.nVal = rtData.nValue[i];
        
        sta_extreme_calc(digit, mainOAD, extremPara.objs[index], extremPara.freqCnt, curTime);

    }
    return;      
}

/**
*********************************************************************
* @brief：      电流/电压不平衡率的极值统计
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_extreme_imbalance_calc(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_EXTREMES_T extremPara)
{
    NOMAL_OAD_T nomalOAD = {0};
    uint16 rtData;          //实时数据，统计源数据
    uint32 len = 0;
    OOP_DIGIT_T digit = {0};

    nomalOAD.logicId = 0;
    nomalOAD.oad.value = extremPara.objs[index].value;

    /* 获取统计源数据 */
    if(db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(uint16), (uint8*)&rtData, &len) <0 )
    {
        return;
    }

    digit.choice = DT_LONG_UNSIGNED;
    digit.wVal = rtData;
    
    sta_extreme_calc(digit, mainOAD, extremPara.objs[index], extremPara.freqCnt ,curTime);
    return;
}

/**
*********************************************************************
* @brief：      正向有功率极值统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_extreme_power_calc(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_EXTREMES_T extremPara)
{
    NOMAL_OAD_T nomalOAD = {0};
    OOP_INT4V_T rtData;          //实时数据，统计源数据
    uint32 len = 0;
    uint32 i = 0;
    OOP_DIGIT_T digit = {0};
    
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = extremPara.objs[index].value;

    /* 获取统计源数据 */
    if(db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_INT4V_T), (uint8*)&rtData, &len) <0 )
    {
        return;
    }

    digit.choice = DT_DOUBLE_LONG;

    //总及分相
    for(i = 0; i < 4; i++)
    {
        extremPara.objs[index].nIndex = i+1; //一次取一相

        digit.nVal = rtData.nValue[i];
        
        sta_extreme_calc(digit, mainOAD, extremPara.objs[index], extremPara.freqCnt ,curTime);
    }

    return;
}

/**
*********************************************************************
* @brief：     功率因数极值统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_extreme_powerfac_calc(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_EXTREMES_T extremPara)
{
    NOMAL_OAD_T nomalOAD = {0};
    OOP_LONG4V_T rtData;          //实时数据，统计源数据
    uint32 len = 0;
    uint32 i = 0;
    OOP_DIGIT_T digit = {0};
    
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = extremPara.objs[index].value;

    /* 获取统计源数据 */
    if(db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_LONG4V_T), (uint8*)&rtData, &len) <0 )
    {
        return;
    }

    digit.choice = DT_LONG;

    //总及分相
    for(i = 0; i < 4; i++)
    {
        extremPara.objs[index].nIndex = i+1; //一次取一相

        digit.nVal = rtData.nValue[i];
        
        sta_extreme_calc(digit, mainOAD, extremPara.objs[index], extremPara.freqCnt ,curTime);
    }

    return;
}

/**
*********************************************************************
* @brief：      谐波含有率极值统计
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_extreme_harmonic_calc(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_EXTREMES_T extremPara)
{
    NOMAL_OAD_T nomalOAD = {0};
    OOP_HARMONIC_T rtData;     //当前实时数据
    uint32 len = 0;
    int i = 0;
    OOP_DIGIT_T digit = {0};
    
    nomalOAD.logicId = 0;
    nomalOAD.oad.value = extremPara.objs[index].value;

    /* 获取统计数据源 */
    if(db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_HARMONIC_T), (uint8*)&rtData, &len) <0 )
    {
        return;
    }

    digit.choice = DT_LONG;

    //2~19次谐波含有率
    for(i = 0; i < 18; i++)
    {
        extremPara.objs[index].nIndex = i+1; //一次取一个

        digit.lVal = rtData.nValue[i];
        
        sta_extreme_calc(digit, mainOAD, extremPara.objs[index], extremPara.freqCnt ,curTime);
    }
    return;
}

/**
*********************************************************************
* @brief：      总加功率的极值统计
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_extreme_tgpower_calc(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_EXTREMES_T extremPara)
{
    NOMAL_OAD_T nomalOAD = {0};
    int64 rtData;          //实时数据，统计源数据
    uint32 len = 0;
    OOP_DIGIT_T digit = {0};
    
    nomalOAD.logicId = 0;
    nomalOAD.oad.value = extremPara.objs[index].value;

    /* 获取统计源数据 */
    if(db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(uint16), (uint8*)&rtData, &len) <0 )
    {
        return;
    }

    digit.choice = DT_LONG64;
    digit.llVal = rtData;

    sta_extreme_calc(digit, mainOAD, extremPara.objs[index], extremPara.freqCnt ,curTime);
    return;
}

/**
*********************************************************************
* @brief：      执行极值统计
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_extreme_execute(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, STA_EXTREMES_T regionPara)
{
    uint32 i = 0;
    OOP_OAD_U oad;

    mainOAD.value += 0x0200;

    for(i = 0; i < regionPara.nNum; i++)
    {
        oad.value = regionPara.objs[i].value;

        //区间统计目前只支持 电压，电流，电流不平衡率，视在功率
        if(sta_extreme_slave_check(oad))
        {
            //TODO 块类型要拆分出子项
            switch(oad.value)
            {   
                case 0x20000200: //三相电压(uint16)
                {
                    sta_extreme_volt_calc(mainOAD, curTime, storeTime, i, regionPara);

                }break;
                case 0x20010200: //三相电流(int32)
                {
                    sta_extreme_curr_calc(mainOAD, curTime, storeTime, i, regionPara);

                }break;   
                case 0x20270200: //电流不平衡率(uint16)
                {
                    sta_extreme_imbalance_calc(mainOAD, curTime, storeTime, i, regionPara);
                }break;
                case 0x20040200: //有功功率（总及分相（int32））
                {
                    sta_extreme_power_calc(mainOAD, curTime, storeTime, i, regionPara);
                }break; 
                case 0x20050200: //无功功率（总及分相（int32））
                {
                    sta_extreme_power_calc(mainOAD, curTime, storeTime, i, regionPara);
                }break;
                case 0x20060200: //视在功率（总及分相（int32））
                {
                    sta_extreme_power_calc(mainOAD, curTime, storeTime, i, regionPara);
                }break;
                case 0x200A0200: //功率因数（总及分相（int16））
                {
                    sta_extreme_powerfac_calc(mainOAD, curTime, storeTime, i, regionPara);

                }break;
                case 0x200D0200: //A相电压谐波含有率（2~19次（int16））
                {
                    sta_extreme_harmonic_calc(mainOAD, curTime, storeTime, i, regionPara);

                }break; 
                case 0x23010300: //总加有功功率
                {
                    sta_extreme_tgpower_calc(mainOAD, curTime, storeTime, i, regionPara);

                }break;                 
                default:STAT_FMT_DEBUG("extreme object func not exit. OAD(0x%08x)\n", oad.value); break;
            }
        }    
        else
        {
            STAT_FMT_DEBUG("extreme object not support. nObjID(%d)\n", oad.nObjID);
        }        
    }
    return;  
}

/**
*********************************************************************
* @brief：      极值统计初始化
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_extreme_init(void)
{
    uint8 i = 0;
    uint8 extremeInitFlag = 0;

    pthread_mutex_lock(&g_staUpdateData.mutex);

    /*读取累加平均统计参数变更标记*/
    extremeInitFlag = g_staUpdateData.extremeInitFlag;

    for(i = 0; i < STA_EXTREME_OBJNUMS; i++)
    {
        if(extremeInitFlag&(1UL<<i))
        {
            sta_extreme_node_add(&g_staExtremesPara[i]);
            extremeInitFlag &= ~(1UL<<i);
        }
    }
    
    /*存储累加平均统计参数变更标记*/
    g_staUpdateData.extremeInitFlag = extremeInitFlag;  

    pthread_mutex_unlock(&g_staUpdateData.mutex);    

    return;
}

/**
*********************************************************************
* @brief：      统计时间刷新
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_extreme_lasttime_reinit(void)
{
    uint8 i = 0, j = 0;
    STA_EXTREMES_PARA_T *pstPara = NULL;

    for(i = 0; i < STA_EXTREME_OBJNUMS; i++)
    {
        if(g_staExtremesPara[i].blockNum == 0)
        {
            continue;
        }
            
        pstPara = &g_staExtremesPara[i];
        
        STAT_FMT_DEBUG("extreme last time reinit. oda(0x%08x)\n", pstPara->oad.value);

        //初始化上一次时间
        for(j = 0; j < pstPara->blockNum; j++)
        {
            sta_last_time_init(pstPara->para[j].freq, &pstPara->para[j].lastTime);
        }
    }
    
    return;
}

/**
*********************************************************************
* @brief：      极值统计处理
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_extreme_proc()
{
    uint32 i = 0;
    uint32 j = 0;
    STA_EXTREMES_PARA_T *tmpNode = NULL;
    time_t nextTime = 0;
    time_t curTime;  
    OOP_TI_SPAN_E time_span;

    DT_DateTimeShort2Time(&realstattime, &curTime);

    /*区间统计类型（分，时，日，月，年）5种*/
    for(i = 0; i < STA_EXTREME_OBJNUMS; i++)
    {
        if(g_staExtremesPara[i].blockNum == 0)
        {
            continue;
        }
        
        tmpNode = &g_staExtremesPara[i];

        sta_interval_analyse(tmpNode->oad, &time_span);

        //每个block中的对象具有相同的freq和period
        for(j = 0; j < tmpNode->blockNum; j++)
        {
            sta_nexttime_update(tmpNode->para[j].freq, tmpNode->para[j].lastTime, &nextTime);

            /* 当前时间初始是比下次时间大的，当下次时间比它小时，说明时间到了，满足统计条件，执行统计*/
            if((nextTime!=0) && (nextTime<=curTime))
            {
                sta_extreme_execute(tmpNode->oad, curTime, nextTime, tmpNode->para[j]);
                tmpNode->para[j].lastTime = nextTime;
                tmpNode->para[j].freqCnt++;                
            }

            //刷新周期数
            sta_period_cnt_update(time_span, tmpNode->para[j].period, &tmpNode->para[j].count, &tmpNode->para[j].freqCnt);           
        }
        
    }    
    return;      
}

#endif
#if DESC("新极值统计处理函数",1)
/**
*********************************************************************
* @brief：      极值统计计算(通用计算)
* @param[in]： rtData  参数计算的数据(后续改为 OOP_DIGIT_T 类型，以支持泛型)
* @param[out]：outData 计算结果
* @return：     void
*********************************************************************
*/
void sta_extreme_calc_new(OOP_DIGIT_T rtData, OOP_OAD_U mainOAD, OOP_OAD_U subOAD, uint32 count, time_t curTime,uint16 
pn)
{
    int ret = ERR_OK;
    OOP_EXTREMERESS_T result;
    READ_RECORD_T readRecord = {{0}};
    WRITE_RECORD_T writeRecord;
    uint32 storeNo;
    OOP_METER_T oopMeter;
    NOMAL_OAD_T ptOAD;
    OOP_OAD_U temmainoad;
    uint32 outlen,i;
    time_t realtime=time(NULL);
    OOP_DATETIME_S_T dateTime;
    time_t linuxTim;

    MEMZERO(&result, sizeof(result));
    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&writeRecord, sizeof(writeRecord));

    
    ptOAD.logicId=0;
    ptOAD.oad = mainOAD;
    ptOAD.infoNum=pn;


    //获取地址
    app_get_meter_list_by_id(pn, &oopMeter);

    ret=db_read_nomal(g_staUdpHandle,&ptOAD,sizeof(result),(uint8*)&result,&outlen);

//    STAT_BUF_DEBUG((uint8*)&result,sizeof(result),"极值统计读取结果 ret is %d\n",ret);
    if((ret != ERR_OK)||(sizeof(result)!=outlen))
    {
        STAT_FMT_DEBUG("读取累加平均失败 ret is %d,outlen is %d\n",ret,outlen);
        MEMZERO(&result,sizeof(result));
    }


    for(i=0;i<result.nNum;i++)
    {
        if(result.extremvalue[i].obj.value == subOAD.value)
        {
            break;
        }
    }
    if(i>=result.nNum)
    {
        result.nNum++;
        result.extremvalue[i].obj.value = subOAD.value;
    }
    if(result.extremvalue[i].dbMinValue.wVal==0x00)
    {
        result.extremvalue[i].dbMinValue.wVal =0xFFFF;
    }
    //构造极值结构
       sta_extreme_construct(rtData, curTime, &result.extremvalue[i]);
    
       STAT_FMT_DEBUG("极值统计[%d]: oad(0x%08x) dbMaxValue(%d),tmMax(%d), dbMinValue(%d), tmMin(%d)\n", count, subOAD.value, result.extremvalue[i].dbMaxValue.wVal,result.extremvalue[i].tmMax.minute,result.extremvalue[i].dbMinValue.wVal,result.extremvalue[i].tmMin.minute);
    
    // 校验码，通信地址的前6个字节
    readRecord.MAC.nNum = 6;
    memcpy(readRecord.MAC.value, oopMeter.basic.tsa.add, 6);

    /* 首次达到统计周期，先清历史数据 */    
    if((pastdayflag)||(pastmonflag))
    {
        if(mainOAD.value==0x21220200)
        {
            temmainoad.value =0x50040200;
        }
        else if(mainOAD.value==0x21230200)
        {
            temmainoad.value =0x50060200;
        }
        ret=db_write_nomal(g_staUdpHandle,&ptOAD,(uint8*)&result,sizeof(result));
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("刷新累加累加平均数据失败 ret is %d\n",ret);
        }
        realtime=time(NULL);
        writeRecord.recordOAD.infoNum = pn;
        writeRecord.recordOAD.road.oadMain.value = temmainoad.value;
        writeRecord.recordOAD.road.oadCols.nNum = 1;
        writeRecord.recordOAD.road.oadCols.oad[0].value = mainOAD.value;
        writeRecord.inDatalen = sizeof(result);
        writeRecord.colStartTm =(uint32)realtime+COMPENSATION_TIME;
        writeRecord.colEndTm =(uint32)realtime+COMPENSATION_TIME;
        DT_Time2DateTimeShort(realtime,&dateTime);
        dateTime.minute=0x00;
        dateTime.second=0x00;
        DT_DateTimeShort2Time(&dateTime,&linuxTim);
        writeRecord.colStoreTm = (uint32)linuxTim+COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间
        STAT_FMT_DEBUG("极值temmainoad.value is %x\n",temmainoad.value);

        //初始，最大最小值都设为当前值
        memcpy(& result.extremvalue[i].dbMaxValue, &rtData, sizeof(OOP_DIGIT_T));
        DT_Time2DateTimeShort(curTime, & result.extremvalue[i].tmMax);
        memcpy(& result.extremvalue[i].dbMinValue, &rtData, sizeof(OOP_DIGIT_T));
        DT_Time2DateTimeShort(curTime, & result.extremvalue[i].tmMin);  

        memcpy(writeRecord.inDataBuf, (uint8*)&result, sizeof(result));
        if(pastmonflag)
        {
            STAT_FMT_DEBUG("过月\n");

            if((subOAD.value ==0x20040204)&&(mainOAD.value==0x21230200))
            {
                STAT_FMT_DEBUG("月极值到了清除数据\n");
                memset((uint8*)&result,0x00,sizeof(result));
                ret=db_write_nomal(g_staUdpHandle,&ptOAD,(uint8*)&result,sizeof(result));
                if(ret != ERR_OK)
                {
                    STAT_FMT_DEBUG("清除极值平均数据失败 ret is %d\n",ret);
                }
            ret = db_write_record(g_staUdpHandle, &writeRecord, &storeNo);   
            if(ret != ERR_OK)
            {
                STAT_FMT_DEBUG("totalavg first write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), len(%d)\n", 
                   ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
            }
            return;
         }

        }
        //首次到达清零
        if(subOAD.value ==0x20000203)
        {
            memset((uint8*)&result,0x00,sizeof(result));
            ret=db_write_nomal(g_staUdpHandle,&ptOAD,(uint8*)&result,sizeof(result));
            if(ret != ERR_OK)
            {
                STAT_FMT_DEBUG("清除极值平均数据失败 ret is %d\n",ret);
            }
            if(mainOAD.value==0x21220200)
            {
                ret = db_write_record(g_staUdpHandle, &writeRecord, &storeNo);   
                if(ret != ERR_OK)
                {
                    STAT_FMT_DEBUG("totalavg first write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), len(%d)\n", 
                       ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
                }
            }
            return;
        }

    }
    else
    {
        ret=db_write_nomal(g_staUdpHandle,&ptOAD,(uint8*)&result,sizeof(result));
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("刷新累加累加平均数据失败 ret is %d\n",ret);
        }
    }


    return;     
}

/**
*********************************************************************
* @brief：      电压极值统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_extreme_volt_calc_new(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_EXTREMES_T 
extremPara,uint16 pn)
{
    RECORD_OAD_T recordOAD={0};
    OOP_WORD3V_T rtData;     //当前实时数据
    int i = 0;
    int ret;
    uint32 storeTm;
    OOP_DIGIT_T digit = {0};
    
    recordOAD.road.oadMain.value=0x00;
    recordOAD.road.oadCols.nNum=1;
    recordOAD.road.oadCols.oad[0].value=0x20000200;
    recordOAD.classtag =5;
    recordOAD.logicId =0;
    recordOAD.infoNum = pn;

    memset((uint8*)&rtData,0x00,sizeof(rtData));
    //读取最近一次电压数据
    STAT_FMT_DEBUG("pn is %d\n",pn);
    ret=sta_record_read_new(g_staUdpHandle,&recordOAD, &storeTm,(uint8*)&rtData,sizeof(OOP_WORD3V_T));
    if(ret !=0)
    {

        STAT_BUF_DEBUG((uint8*)&rtData,sizeof(OOP_WORD3V_T),"极值统计读取数据结果 ret is %d,pn %d\n",ret,pn);
        return;
    }
    digit.choice = DT_LONG_UNSIGNED;

    //三相
    for(i = 0; i < 3; i++)
    {

        extremPara.objs[index].nIndex = i+1; //一次取一相

        digit.wVal = rtData.nValue[i];
        
        sta_extreme_calc_new(digit, mainOAD, extremPara.objs[index], extremPara.freqCnt, curTime,pn);

    }
    return;      
}


/**
*********************************************************************
* @brief：      电压极值统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_extreme_power_calc_new(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_EXTREMES_T 
extremPara,uint16 pn)
{
    RECORD_OAD_T recordOAD={0};
    OOP_INT4V_T rtData;     //当前实时数据
    int i = 0;
    int ret;
    uint32 storeTm;
    OOP_DIGIT_T digit = {0};
    
    recordOAD.road.oadMain.value=0x00;
    recordOAD.road.oadCols.nNum=1;
    recordOAD.road.oadCols.oad[0].value=0x20040200;
    recordOAD.classtag =5;
    recordOAD.logicId =0;
    recordOAD.infoNum = pn;

    memset((uint8*)&rtData,0x00,sizeof(rtData));
    //读取最近一次电压数据
    STAT_FMT_DEBUG("pn is %d\n",pn);
    ret=sta_record_read_new(g_staUdpHandle,&recordOAD, &storeTm,(uint8*)&rtData,sizeof(OOP_INT4V_T));
    if(ret !=0)
    {
        STAT_BUF_DEBUG((uint8*)&rtData,sizeof(OOP_INT4V_T),"极值统计读取数据结果 ret is %d,pn %d\n",ret,pn);
        return;
    }
    digit.choice = DT_DOUBLE_LONG;

    //三相
    for(i = 0; i < 4; i++)
    {

        extremPara.objs[index].nIndex = i+1; //一次取一相

        digit.wVal = rtData.nValue[i];
        
        sta_extreme_calc_new(digit, mainOAD, extremPara.objs[index], extremPara.freqCnt, curTime,pn);

    }
    return;      
}



/**
*********************************************************************
* @brief：      电压极值统计计算
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_extreme_curr_calc_new(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, uint32 index, STA_EXTREMES_T 
extremPara,uint16 pn)
{
    RECORD_OAD_T recordOAD={0};
    OOP_INT3V_T rtData;     //当前实时数据
    int i = 0;
    int ret;
    uint32 storeTm;
    OOP_DIGIT_T digit = {0};
    
    recordOAD.road.oadMain.value=0x00;
    recordOAD.road.oadCols.nNum=1;
    recordOAD.road.oadCols.oad[0].value=0x20010200;
    recordOAD.classtag =5;
    recordOAD.logicId =0;
    recordOAD.infoNum = pn;

    memset((uint8*)&rtData,0x00,sizeof(rtData));
    //读取最近一次电流数据
    STAT_FMT_DEBUG("pn is %d\n",pn);
    ret=sta_record_read_new(g_staUdpHandle,&recordOAD, &storeTm,(uint8*)&rtData,sizeof(OOP_INT3V_T));
    if(ret !=0)
    {
        STAT_BUF_DEBUG((uint8*)&rtData,sizeof(OOP_INT4V_T),"极值统计读取数据结果 ret is %d,pn %d\n",ret,pn);
        return;
    }
    digit.choice = DT_DOUBLE_LONG;

    //三相
    for(i = 0; i < 3; i++)
    {

        extremPara.objs[index].nIndex = i+1; //一次取一相

        digit.wVal = rtData.nValue[i];
        
        sta_extreme_calc_new(digit, mainOAD, extremPara.objs[index], extremPara.freqCnt, curTime,pn);

    }
    return;      
}

/**
*********************************************************************
* @brief：      执行极值统计
* @param[in]： storeTime 统计时间
* @param[in]： regionPara 统计的对象集合
* @return：     void
*********************************************************************
*/
void sta_extreme_execute_new(OOP_OAD_U mainOAD, time_t curTime, time_t storeTime, STA_EXTREMES_T regionPara)
{
    uint32 i = 0;
    OOP_OAD_U oad;
    uint16 j=0;
    OOP_METER_T oopMeter;

    mainOAD.value += 0x0200;
    for(j = 0; j < oopMeterList.oopMeterNum; j++)
    {
        if(app_get_meter_list_by_id(j, &oopMeter) == 0)
        {
            if((oopMeter.basic.port.nObjID!=0xF201)&&(oopMeter.basic.port.nObjID!=0xF208))
            {
                continue;
            }
        }
        for(i = 0; i < regionPara.nNum; i++)
        {
            oad.value = regionPara.objs[i].value;

            //区间统计目前只支持 电压，电流，电流不平衡率，视在功率
            if(sta_extreme_slave_check(oad))
            {
                //TODO 块类型要拆分出子项
                switch(oad.value)
                {   
                    case 0x20040200: //有功功率（总及分相（int32））
                    {
                        sta_extreme_power_calc_new(mainOAD, curTime, storeTime, i, regionPara,oopMeter.nIndex);
                    }break;
                    case 0x20010200: //三相电流(int32)
                    {
                        sta_extreme_curr_calc_new(mainOAD, curTime, storeTime, i, regionPara,oopMeter.nIndex);

                    }break;  
                    case 0x20000200: //三相电压(uint16)
                    {
                        sta_extreme_volt_calc_new(mainOAD, curTime, storeTime, i, regionPara,oopMeter.nIndex);

                    }break;
                    case 0x20270200: //电流不平衡率(uint16)
                    {
                        sta_extreme_imbalance_calc(mainOAD, curTime, storeTime, i, regionPara);
                    }break;
 
                    case 0x20050200: //无功功率（总及分相（int32））
                    {
                        sta_extreme_power_calc(mainOAD, curTime, storeTime, i, regionPara);
                    }break;
                    case 0x20060200: //视在功率（总及分相（int32））
                    {
                        sta_extreme_power_calc(mainOAD, curTime, storeTime, i, regionPara);
                    }break;
                    case 0x200A0200: //功率因数（总及分相（int16））
                    {
                        sta_extreme_powerfac_calc(mainOAD, curTime, storeTime, i, regionPara);

                    }break;
                    case 0x200D0200: //A相电压谐波含有率（2~19次（int16））
                    {
                        sta_extreme_harmonic_calc(mainOAD, curTime, storeTime, i, regionPara);

                    }break; 
                    case 0x23010300: //总加有功功率
                    {
                        sta_extreme_tgpower_calc(mainOAD, curTime, storeTime, i, regionPara);

                    }break;                 
                    default:STAT_FMT_DEBUG("extreme object func not exit. OAD(0x%08x)\n", oad.value); break;
                }
            }    
            else
            {
                STAT_FMT_DEBUG("extreme object not support. nObjID(%d)\n", oad.nObjID);
            }        
        }
    }
    return;  
}

/**
*********************************************************************
* @brief：      极值统计处理
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_extreme_proc_new()
{
    uint32 i = 0;
    uint32 j = 0;
    STA_EXTREMES_PARA_T *tmpNode = NULL;
    time_t nextTime = 0;
    time_t curTime;  
    OOP_TI_SPAN_E time_span;

    DT_DateTimeShort2Time(&realstattime, &curTime);

    /*区间统计类型（分，时，日，月，年）5种*/
    for(i = 0; i < STA_EXTREME_OBJNUMS; i++)
    {
        if(g_staExtremesPara[i].blockNum == 0)
        {
            continue;
        }
        
        tmpNode = &g_staExtremesPara[i];

        sta_interval_analyse(tmpNode->oad, &time_span);

        //每个block中的对象具有相同的freq和period
        for(j = 0; j < tmpNode->blockNum; j++)
        {
            sta_nexttime_update(tmpNode->para[j].freq, tmpNode->para[j].lastTime, &nextTime);

            /* 当前时间初始是比下次时间大的，当下次时间比它小时，说明时间到了，满足统计条件，执行统计*/
            if((nextTime!=0) && (nextTime<=curTime))
            {
                STAT_FMT_DEBUG("tmpNode->oad %x\n",tmpNode->oad);
                sta_extreme_execute_new(tmpNode->oad, curTime, nextTime, tmpNode->para[j]);
                tmpNode->para[j].lastTime = nextTime;
                tmpNode->para[j].freqCnt++;                
            }

            //刷新周期数
            sta_period_cnt_update(time_span, tmpNode->para[j].period, &tmpNode->para[j].count, &tmpNode->para[j].freqCnt);           
        }
        
    }    
    return;      
}

#endif

#if DESC("协议统计处理函数",1)

/**
*********************************************************************
* @brief：      诊断打印
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_protocol_diag(void)
{
    int i = 0, j = 0, k = 0, m = 0;

    //取OAD数据，计算统计值，存储结果
    for(i = 0; i < STA_REGION_OBJNUMS; i++)
    {
        STAT_FMT_TRACE("g_staRegionsPara[%d].oad(0x%08x), g_staRegionsPara[%d].blocknum(%d)\n",
            i,g_staRegionsPara[i].oad.value, i, g_staRegionsPara[i].blockNum);

        for(j = 0; j < g_staRegionsPara[i].blockNum; j++)
        {
            STAT_FMT_TRACE("para[%d].lastTime(%ld),para[%d].count(%d),para[%d].freq.unit(%d),para[%d].freq.value(%d),para[%d].period(%d),para[%d].nNum(%d)\n", 
                j, g_staRegionsPara[i].para[j].lastTime, 
                j, g_staRegionsPara[i].para[j].count,
                j, g_staRegionsPara[i].para[j].freq.unit,
                j, g_staRegionsPara[i].para[j].freq.value,
                j, g_staRegionsPara[i].para[j].period,
                j, g_staRegionsPara[i].para[j].nNum);

            for(k = 0; k < g_staRegionsPara[i].para[j].nNum; k++)
            {
                STAT_FMT_TRACE("objs[%d].obj(0x%08x), objs[%d].nNum(%d)\n", k, g_staRegionsPara[i].para[j].objs[k].obj.value, 
                    k, g_staRegionsPara[i].para[j].objs[k].nNum);

                for(m = 0; m < g_staRegionsPara[i].para[j].objs[k].nNum; m++)
                {
//                    STAT_FMT_TRACE("parm[%d].choice(%d), parm[%d].bVal(%d),parm[%d].cVal(%d),parm[%d].lVal(%d),parm[%d].wVal(%d),parm[%d].nVal(%d),parm[%d].dwVal(%d),parm[%d].llVal(%lld),parm[%d].ullVal(%llu),\n", 
//                        m, g_staRegionsPara[i].para[j].objs[k].parm[m].choice,
//                        m, g_staRegionsPara[i].para[j].objs[k].parm[m].bVal,
//                        m, g_staRegionsPara[i].para[j].objs[k].parm[m].cVal,
//                        m, g_staRegionsPara[i].para[j].objs[k].parm[m].lVal,
//                        m, g_staRegionsPara[i].para[j].objs[k].parm[m].wVal,
//                        m, g_staRegionsPara[i].para[j].objs[k].parm[m].nVal,
//                        m, g_staRegionsPara[i].para[j].objs[k].parm[m].dwVal,
//                        m, g_staRegionsPara[i].para[j].objs[k].parm[m].llVal,
//                        m, g_staRegionsPara[i].para[j].objs[k].parm[m].ullVal);
                }
            }
        }
    }

    for(i = 0; i < STA_TOTALAVG_OBJNUMS; i++)
    {
        STAT_FMT_TRACE("g_staTotalavgsPara[%d].oad(0x%08x), g_staTotalavgsPara[%d].blocknum(%d)\n",
            i,g_staTotalavgsPara[i].oad.value, i, g_staTotalavgsPara[i].blockNum);

        for(j = 0; j < g_staTotalavgsPara[i].blockNum; j++)
        {
            STAT_FMT_TRACE("para[%d].lastTime(%ld),para[%d].count(%d),para[%d].freq.unit(%d),para[%d].freq.value(%d),para[%d].period(%d),para[%d].nNum(%d)\n", 
                j, g_staTotalavgsPara[i].para[j].lastTime, 
                j, g_staTotalavgsPara[i].para[j].count,
                j, g_staTotalavgsPara[i].para[j].freq.unit,
                j, g_staTotalavgsPara[i].para[j].freq.value,
                j, g_staTotalavgsPara[i].para[j].period,
                j, g_staTotalavgsPara[i].para[j].nNum);

            for(k = 0; k < g_staTotalavgsPara[i].para[j].nNum; k++)
            {
                STAT_FMT_TRACE("objs[%d].obj(0x%08x)\n", k, g_staTotalavgsPara[i].para[j].objs[k].value);
            }
        }
    }

    for(i = 0; i < STA_EXTREME_OBJNUMS; i++)
    {
        STAT_FMT_TRACE("g_staExtremesPara[%d].oad(0x%08x), g_staExtremesPara[%d].blocknum(%d)\n",
            i,g_staExtremesPara[i].oad.value, i, g_staExtremesPara[i].blockNum);

        for(j = 0; j < g_staExtremesPara[i].blockNum; j++)
        {
            STAT_FMT_TRACE("para[%d].lastTime(%ld),para[%d].count(%d),para[%d].freq.unit(%d),para[%d].freq.value(%d),para[%d].period(%d),para[%d].nNum(%d)\n", 
                j, g_staExtremesPara[i].para[j].lastTime, 
                j, g_staExtremesPara[i].para[j].count,
                j, g_staExtremesPara[i].para[j].freq.unit,
                j, g_staExtremesPara[i].para[j].freq.value,
                j, g_staExtremesPara[i].para[j].period,
                j, g_staExtremesPara[i].para[j].nNum);

            for(k = 0; k < g_staExtremesPara[i].para[j].nNum; k++)
            {
                STAT_FMT_TRACE("objs[%d].obj(0x%08x)\n", k, g_staExtremesPara[i].para[j].objs[k].value);
            }
        }
    }

    return;
}

/**
*********************************************************************
* @brief：      协议统计处理
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sta_protocol_stat(void)
{ 

    sta_region_init();

    sta_totalavg_init(); 

    sta_extreme_init();

    //sta_protocol_diag();

    sta_region_proc();

    sta_totalavg_proc();

    sta_extreme_proc();
    return;      
}

#endif

pthread_mutex_t g_fStatisStatus;      //统计工作状态
uint8           g_sClearFlag = FALSE; 

/**
*********************************************************************
* @name      ：sta_data_init
* @brief     ：统计模块数据初始化
* @return    ：
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
void sta_data_init(uint8 type, DB_CLIENT fd)
{
    int ret = ERR_OK;
    int i = 0;  
    NOMAL_OAD_T nomalOAD = {0}; 
    uint16  resetAcc[2] = {0};
    char datapath[256] = {0};
    
    nomalOAD.oad.value = 0x22040200;
    nomalOAD.classtag = CLASS_DATA_NORMAL;

    if(type == 0)
    {
        resetAcc[0] = 1;
        resetAcc[1] = 1;
    }
    else
    {
        resetAcc[0] = 0;
        resetAcc[1] = 0;
    }
    
    //初始化后，终端复位次数应该仍有1次
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)resetAcc, sizeof(resetAcc));
    PRTL_FMT_DEBUG("init terminal reset counts. ret(%d)\n", ret);                

    //初始化数据，交采统计数据私有文件
    memset(datapath, 0, sizeof(datapath));
    sprintf(datapath, "/data/app/%s/data/statacsample", ADV_APP_NAME);

    ret = remove(datapath);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", datapath, ret, strerror(errno));
    }
    else
    {
        PRTL_FMT_DEBUG("remove \"%s\" success.\n", datapath);
    }

    memset(datapath, 0, sizeof(datapath));
    sprintf(datapath, "/data/app/%s/data/bakstatacsample", ADV_APP_NAME);

    ret = remove(datapath);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", datapath, ret, strerror(errno));
    }
    else
    {
        PRTL_FMT_DEBUG("remove \"%s\" success.\n", datapath);
    }

    //脉冲计量点的信息0x2401~0x2408为脉冲OI，取低4位1~8来访问私有文件
    for(i = 1; i < MTRPLS_MAX_NUM+1; i++)
    {
        memset(datapath, 0, sizeof(datapath));
        sprintf(datapath,"/data/app/%s/data/statpluse%d",ADV_APP_NAME, i);

        ret = remove(datapath);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", datapath, ret, strerror(errno));
        }
        else
        {
            PRTL_FMT_DEBUG("remove \"%s\" success.\n", datapath);
        }

        memset(datapath, 0, sizeof(datapath));
        sprintf(datapath,"/data/app/%s/data/bakstatpluse%d",ADV_APP_NAME, i);

        ret = remove(datapath);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", datapath, ret, strerror(errno));
        }
        else
        {
            PRTL_FMT_DEBUG("remove \"%s\" success.\n", datapath);
        }
    }

    //485表用配置序号来关联私有文件
    memset(datapath, 0, sizeof(datapath));
    sprintf(datapath,"/data/app/%s/data/statmeter*",ADV_APP_NAME);
    
    ret = wildcard_remove(datapath);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", datapath, ret, strerror(errno));
    }
    else
    {
        PRTL_FMT_DEBUG("remove \"%s\" success.\n", datapath);
    }

    memset(datapath, 0, sizeof(datapath));
    sprintf(datapath,"/data/app/%s/data/bakstatmeter*",ADV_APP_NAME);
    
    ret = wildcard_remove(datapath);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", datapath, ret, strerror(errno));
    }
    else
    {
        PRTL_FMT_DEBUG("remove \"%s\" success.\n", datapath);
    }
    
    MEMZERO(g_slippower_fifo, sizeof(g_slippower_fifo));
    PRTL_FMT_DEBUG("clear slip power fifo.\n");
}

/**
*********************************************************************
* @brief：      设置统计工作状态
* @param[in]：  uint8 status  工作状态 FALSE-停止工作；TRUE-开始工作
* @param[out]： NA
* @return：     void
*********************************************************************
*/
void statis_work_set(uint8 status)
{
    if(FALSE == status)
    {
        pthread_mutex_unlock(&g_fStatisStatus);
    }
    else
    {
        pthread_mutex_lock(&g_fStatisStatus);
    }
}

/**
*********************************************************************
* @brief：      结合统计业务数据清除
* @param[in]：  DB_CLIENT fd
* @param[out]： CLEAR_DATA_T *ptData
* @return：     int 错误码
*********************************************************************
*/
int statis_clear_data(DB_CLIENT fd, CLEAR_DATA_T *ptData)
{
    int ret = 0;
    
    PTHREAD_LOCK_SAFE(&g_fStatisStatus);
    ret = db_clear_data(fd, ptData);
    if(0 == ret)
    {
        g_sClearFlag = TRUE;
    }
    PTHREAD_UNLOCK_SAFE(&g_fStatisStatus);
    return ret;
}

/**
*********************************************************************
* @brief：      结合统计业务恢复出厂设置
* @param[in]：  DB_CLIENT fd
* @param[out]： uint8 logicId
* @return：     int 错误码
*********************************************************************
*/
int statis_recover_default(DB_CLIENT fd, uint8 logicId)
{
    int ret = 0;
    
    PTHREAD_LOCK_SAFE(&g_fStatisStatus);
    ret = db_recover_default(fd, logicId);
    if(0 == ret)
    {
        g_sClearFlag = TRUE;
    }
    PTHREAD_UNLOCK_SAFE(&g_fStatisStatus);
    return ret;
}

int statis_task_entry(uint32 dbip, uint16 dbport)
{ 
    FREEZE_FMT_DEBUG("====================stat thread start====================\n");
    
    g_staUdpHandle = db_client_create(DB_YC_AGENT, DB_CLIENT_STATIS);
    if(g_staUdpHandle <= 0)
    {
        STAT_FMT_DEBUG("sta db_handle_create failed!\n");
        return 0;
    }
    pthread_mutex_init(&g_fStatisStatus, NULL);
    pthread_mutex_init(&g_tgdel.flock, NULL);
    sta_tg_min_elec_init(); //分钟电量

    /* 统计数据的初始化 */  
    sta_classical_init();
	/* 为了防止防止重启后数据中心还没有写入真实的电量 */
	WaitDbData();
	
    while(1)
    {
        statis_work_set(TRUE);
        if(FALSE != g_sClearFlag)
        {
            /* 初始化延时10s */
            STAT_FMT_DEBUG("clear data or recover default, wait 10s!\n");
            sleep(10);
        }
        g_sClearFlag = FALSE;
        
        //时间刷新，含对时处理
        sta_realtime_update();

        //从私有文件中读取上次时间，主要怕停电后的lasttime丢失 NAND_SPECIAL
        if((read_pdata_xram("statpdata",(char *)&laststattime,0,sizeof(laststattime)) < 0)
            || laststattime.year == 0xff || laststattime.month == 0xff || laststattime.day == 0xff
            || laststattime.hour == 0xff || laststattime.minute == 0xff || laststattime.second == 0xff)
        {
            memcpy((char *)&laststattime,(char *)&realstattime,sizeof(OOP_DATETIME_S_T));
            write_pdata_xram("statpdata",(char *)&realstattime,0,sizeof(realstattime));
			STAT_FMT_DEBUG("上次时间非法,刷新为当前时间!\n");
        }

        if(app_secure_defend_isuse())  
        {
            //端口授权操作
            sta_tmn_port_authorize_operate();
        }

        sta_spantime_update();

        //过秒执行
        if(g_currtime.second != g_lasttime.second)
        {
#ifdef PRODUCT_ECU
            sta_dc_analog_collect();
#endif
        }

        /*每分钟执行一次,对源数据进行加工存储，为协议统计提供输入*/
        if (pastminflag)
        {
            sta_classical_stat();
//            if((access(DELAY_FILE_PATH,F_OK)==0))
//            {    
//                if(gdelay == 0)
//                {
//                    gdelay = 1;
//                    STAT_FMT_DEBUG("检测到开始减压\n");
//                }
//            }
//            else
//            {
//                if(gdelay == 1)
//                {
//                    gdelay = 0;
//                }
//            }
            if((access(DELAY_FILE_PATH,F_OK)==0))
            {    
                if(gdelay == 0)
                {
                    gdelay = 1;
                    gdelaytime = DELAYTIME;
                    STAT_FMT_DEBUG("检测到开始减压\n");
                }
            }else if((access(DELAY_FILE_PATH2,F_OK)==0))
            {
                if(gdelay != 2)
                {
                    gdelay = 2;
                    gdelaytime = DELAYTIME_MORE;
                    STAT_FMT_DEBUG("进一步减压\n");

                }
            }
            else
            {
                if(gdelay != 0)
                {
                    gdelay = 0;
                    gdelaytime = 0;
                }
            }
        }

        sta_protocol_stat();

        //冻结处理在所有统计之后，可以冻结统计计算后的结果
        sta_frozen_proc();

		//冻结远程通信信道信息
#ifdef AREA_SHAANXI
        sta_frozen_wireless_proc();
#endif

        //冻结之后，清日，月数据
        sta_past_data_clear();

        //曲线在清零日月数据之后冻结
        sta_frozen_curve_proc();

        if (pastminflag)
        {
            write_pdata_xram("statpdata", (char *)&realstattime, 0, sizeof(realstattime));
        }

        statis_work_set(FALSE);
        memcpy((char *)&laststattime,(char *)&realstattime,sizeof(OOP_DATETIME_S_T));
        memcpy((char *)&g_lasttime,(char *)&g_currtime,sizeof(OOP_DATETIME_S_T));

        usleep(200000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }

    }

    FREEZE_FMT_DEBUG("====================stat thread end====================\n");

    return ERR_OK;
}

