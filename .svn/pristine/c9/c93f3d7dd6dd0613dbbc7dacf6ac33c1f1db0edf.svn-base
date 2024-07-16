/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   电量控制模块头文件
* @date：    2020-4-9
* @history： 
*********************************************************************
*/
#ifndef __APPCTRL_ELEC_H_
#define __APPCTRL_ELEC_H_

#ifdef __cplusplus
        extern "C" {
#endif

#define HAVE_PASS_SETVAL    1

#define SET_ALARM_TYPE        1    //-超定值告警-
#define BREAK_ALARM_TYPE    2    //-拉闸告警-

typedef struct
{
    uint8 EcTurnAllSet[OOP_MAX_TG];           //电控轮次总投入
    uint8 EcTurnSet[OOP_MAX_TG][OOP_MAX_TURN];//-电控轮次(按位1 投入, 0解除)
    uint8 HavePassMonSet[OOP_MAX_TG];    /*-月电控超定值标志-*/
    uint8 HavePassBuySet[OOP_MAX_TG];    /*-购电控超定值标志-*/
    uint8 bEcMonPassWarn[OOP_MAX_TG];    /*-月电控越限告警标志-*/
    uint8 bEcBuyPassWarn[OOP_MAX_TG];    /*-购电控越限告警标志-*/
    uint8 TelBuyAlarm[OOP_MAX_TG];       /*远程模式特殊告警*/
    int64 MonUsedValue[OOP_MAX_TG];
    int64 MondefValue[OOP_MAX_TG];
    uint8 MonlimitCoe[OOP_MAX_TG];
    int8  MonfloatCoe[OOP_MAX_TG];
    uint8 BuyType[OOP_MAX_TG];          //购电类型 电量（0），电费（1）
    int64 BuyLeftValue[OOP_MAX_TG]; 
    int64 BuyAlarmValue[OOP_MAX_TG];    //报警门限值
    int64 BuySetValue[OOP_MAX_TG];      //跳闸门限值

    uint16 MonShowLeftTimes[OOP_MAX_TG];
    uint16 BuyShowLeftTimes[OOP_MAX_TG];
}EC_INFO_T;    /*-电控信息-*/

void appctrl_elec_init();
void appctrl_elec_ctrl(int cid, uint8 bPassMon, uint8 turnmax);
void appctrl_elec_status(uint8 turnmax);
void appctrl_callfee_ctrl();
void appctrl_elec_write(uint8 bDay);

#ifdef __cplusplus
}
#endif

#endif


