/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   远方控制模块头文件
* @date：    2020-4-10
* @history： 
*********************************************************************
*/
#ifndef __APPCTRL_REMOTE_H_
#define __APPCTRL_REMOTE_H_

#ifdef __cplusplus
        extern "C" {
#endif

#define SWITCH_IDLE_STEP        0   /*-初始化进程步-*/
#define SWITCH_START_STEP       1   /*-遥控命令预处理进程步-*/
#define SWITCH_ALARM_STEP       2   /*-告警延时处理进程步-*/
#define SWITCH_OPEN_STEP        3   /*-遥控拉闸限电延时进程步-*/

#define SWITCH_TYPE_OPEN        0   /* 分闸继电器 */
#define SWITCH_TYPE_CLOSE       1   /* 合闸继电器 */

typedef struct
{
    uint8  switchtype[OOP_MAX_TURN * 2];      /*-继电器类型*/
    uint8  turnNO[OOP_MAX_TURN * 2];          /*-轮次序号（0-7）*/
    uint8  bSwitchStep[OOP_MAX_TURN * 2];     /*-状态机标志-*/
    uint8  btripNo[OOP_MAX_TURN * 2];         /*-参数序号-*/
    uint16 wOpenBrakeDelay[OOP_MAX_TURN * 2]; /*-告警秒倒计时-*/    
    uint32 wLimitEcTime[OOP_MAX_TURN * 2];    /*-计划限电时间-*/
    uint8  bLimitForever[OOP_MAX_TURN * 2];   /* 是否永久限电 */
    uint8 bfgFirstRtrl[OOP_MAX_TURN * 2];     /*-第一次补充两分钟后遥控记录标志-*/
    uint8 bTwoMin[OOP_MAX_TURN * 2];          /*-两分钟计时-*/
    uint32 oad[OOP_MAX_TURN * 2];             /* 继电器OAD */
    int64 TwoMinPower[OOP_MAX_TURN * 2][OOP_MAX_TG]; /* 遥控跳闸后2min总加组有功功率 */
    int64 actPower[OOP_MAX_TURN * 2][OOP_MAX_TG];    /* 遥控跳闸发生总加组有功功率 */

}REMOTE_INFO_T;  /*-遥控信息数据结构-*/

void appctrl_remote_init();
void appctrl_remote_ctrl(int cid, uint8 bDayPass, uint8 nmax);
void appctrl_remote_status(uint8 turnmax);
void appctrl_remote_write(uint8 bDay);

#ifdef __cplusplus
}
#endif

#endif


