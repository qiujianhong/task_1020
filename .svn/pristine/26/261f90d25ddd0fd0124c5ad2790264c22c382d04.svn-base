/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   功率控制模块头文件
* @date：    2020-4-6
* @history： 
*********************************************************************
*/
#ifndef __APPCTRL_POWER_H_
#define __APPCTRL_POWER_H_

#ifdef __cplusplus
        extern "C" {
#endif

#define NOT_PASS_SETVAL     0x55       /*-未首次超功率定值-*/
#define HAVE_PASSED_SETVAL  0xaa       /*-首次超功率定值-*/

typedef struct
{        
    uint16    home;      
    uint16    end;     
    uint8    index;
}PCSEG_INFO_T;      /*-功控时段信息-*/

typedef struct tagCtrlPowerInfo
{    
    uint8 bFirstPassPower[OOP_MAX_TG][OOP_MAX_TURN];     /*-功率首次超定值标志-*/
    uint8 bFirstDownPower[OOP_MAX_TG];     /*-功率首次低于定值标志-*/
    uint8 bDownTenMin[OOP_MAX_TG];  /*-十分钟计数-*/
    uint8 bCurrentSeg[OOP_MAX_TG];     /*-当前时段号-*/
    uint8 bFirstDownCtrl[OOP_MAX_TG]; /*-下浮控首次投入标志-*/
    uint8 bPcKind[OOP_MAX_TG];  /*-当前功控种类-*/
    uint8 bPcTurnSet[OOP_MAX_TG][OOP_MAX_TURN];  /*-当前功控轮次投入 1投入, 0未投入-*/
    uint8 wPowerAlarmDelay[OOP_MAX_TG][OOP_MAX_TURN]; /*-功控轮次间告警延时,以分钟计-*/
    uint16 wPcAlarmDelayss[OOP_MAX_TG][OOP_MAX_TURN];/*-功控轮次间告警延时,以秒计-*/
    uint8 bFirstInit[OOP_MAX_TG][OOP_MAX_TURN]; /*-告警延时计数器首次赋初值标志-*/
    uint8 bfgFirstPrtl[OOP_MAX_TG][OOP_MAX_TURN]; /*-第一次补充两分钟后功控记录标志-*/
    int64 wPowerSetValue[OOP_MAX_TG]; /*-功率定值-*/
    int64 wPowerONValue[OOP_MAX_TG][OOP_MAX_TURN]; /*-功控触发前有功功率-*/    
    uint8 bLastPCtrl[OOP_MAX_TG];    /*-上次功控类型-*/
    int8  bDownCtrlCoe[OOP_MAX_TG];  //-当前功率下浮控浮动系数-
    uint8 bFirstWriteFreeze[OOP_MAX_TG]; /*-首次写下浮控冻结值标志-*/
    uint8 bTwoMin[OOP_MAX_TG][OOP_MAX_TURN]; /*-两分钟计时-*/
    uint16 wSegAlarmCnt[OOP_MAX_TG][OOP_MAX_TURN];       /*-时段控告警延时计数器，四个轮次-*/
    uint8 bDownFreezeTime[OOP_MAX_TG];      /*-下浮控功率冻结时间-*/
    time_t tDownStart[OOP_MAX_TG];          /* 下浮控投入时间 */
}POWER_INFO_T;      /*-功控信息-*/

void appctrl_power_init();
void appctrl_down_init();
void appctrl_pcturn_change(uint8 turnMax);
void appctrl_power_ctrl(int cid, uint8 bDayPass, uint8 turnmax);
void appctrl_power_status(uint8 turnmax);
void appctrl_power_show_decress(uint8 turnmax);
void appctrl_power_write(uint8 bDay);

#ifdef __cplusplus
}
#endif

#endif


