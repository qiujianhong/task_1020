/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   控制模块头文件
* @date：    2020-3-31
* @history： 
*********************************************************************
*/
#ifndef __APPCTRL_COMM_H_
#define __APPCTRL_COMM_H_

#include "oopType.h"
#include "appmsg_struct.h"
#include "led.h"
#include "loadctrller_data.h"

#ifdef __cplusplus
        extern "C" {
#endif


/* 送检需要关闭自保电功能 */
#define CTRL_PTOTECT_OPEN        1

#define CTRL_MOD_ID_MIN          1
#define CTRL_MOD_ID_MAX          5

/* 硬件设备参数 */
typedef struct tagCtrlCfg
{
    OOP_MODULE_INFO_NEW_T     modelInfo;                 //模组信息
    uint8                     turnMAX;                   //轮次最大总数     来自配置文件
    uint8                     portNum;                   //一个模块轮次数量 来自配置文件
    uint8                     turnNum;                   //实际轮次总数 = portNum * 模块数量
    uint8                     nACM1;
    uint8                     nACM2;
    uint8                     nsubACM1;
    uint8                     nsubACM2;
    uint8                     bModeOk;
    uint8                     turnFlag;
    
    uint8                     nACMAlarm;
    uint8                     nsubAlarm;;
    uint8                     res[1];
    
}CTRL_CONDIG_T;


/* 规则调整最大轮次数量少于等于2个最大轮次是2, 否则是实际配置的最大值 */
#define CTRL_MAX_TURN(aa, bb) ((aa) <= 4 ? 4 : (bb))

#define CTRL_UNEFFECT    0   /*-未投入，不允许实际的拉闸-*/
#define CTRL_EFFECT      1   /*-投入，允许实际的拉闸-*/

typedef enum CTRL_STORE_TYPE
{
    CTRL_ALARM_STORE        = 0, /* 告警 */
    CTRL_STATUS_STORE       = 1, /* 闸动作 */
    CTRL_NULL_STORE         = 0xff,
}CTRL_STORE_TYPE_E;

//-无动作状态-
#define TURN_OFF            1
//-动作状态-
#define TURN_ON             0

/*-合闸命令来源-*/
#define COMMAND_CLOSE        0x11    /*-主站命令-*/
#define CTRLOFF_CLOSE        0x22    /*-控制解除-*/
#define LOWSETVAL_CLOSE      0x33    /*-低于定值-*/
#define OUTOFTIME_CLOSE      0x44    /*-非控制时段-*/

#define EN_SHARP             1        /*电量尖段*/
#define EN_TOP               2        /*电量峰段*/
#define EN_LEVEL             3        /*电量平段*/
#define EN_BOTTOM            4        /*电量谷段*/

/* 远程控制参数 */
typedef struct tagRemoteArg
{
    /* OAD */
    OOP_TELECON_PARAM_T  param;       //0x80000200 配置参数
    //OOP_BITSTR8_T        relayStatus; //0x80000300 继电器输出状态
#ifdef PRODUCT_ZCU_1
    OOP_BITSTR16_T       alarmstatus; //0x80000400 告警状态
    OOP_BITSTR16_T       orderstatus; //0x80000500 命令状态
#else
    OOP_BITSTR8_T        alarmstatus; //0x80000400 告警状态
    OOP_BITSTR8_T        orderstatus; //0x80000500 命令状态
#endif
    /* OMD */
    uint8                bAlarm;      //方法 127：触发告警-TRUE;方法 128：解除报警-FALSE
    OOP_TELECON_TRIP_T   trip;        //方法 129：跳闸（参数）
    OOP_TELECON_CLOSE_T  close;       //方法 130：合闸（参数）

    /* 私有数据 */
    uint8                bstart[OOP_MAX_TRIP];      //首次执行标记 按照继电器顺序排列
    uint8                startType[OOP_MAX_TRIP];   //生效类型 按照继电器顺序排列 0-无效;1-跳闸;2-合闸;
}CTRL_REMOTE_ARG_T;

/* 保电参数 */
typedef struct tagProtectArg
{
    /* OAD */
    uint8                status;      //0x80010200 保电状态,解除（0），保电（1），自动保电（2）
    uint16               NMTimer;     //0x80010300 允许与主站最大无通信时长（分钟），0表示不自动保电
    uint16               FMTimer;     //0x80010400 上电自动保电时长（分钟），0表示上电不自动保电
    OOP_PRTTIME_T        autoprotect; //0x80010500 自动保电时段
    /* OMD */
    //uint8                bEffect;     //方法 127：投入保电-TRUE ;方法 128：解除保电-FALSE
    //uint8                bCloseAuto;  //方法 129：解除自动保电-TRUE
}CTRL_PROTECT_ARG_T;

/* 催费告警配置 */
typedef struct tagCallFeeArg
{
    /* OAD */
    uint8                status;      //0x80020200 催费告警状态，只读
    OOP_CALLFEE_T        param;       //0x80020300 催费告警参数，只读
    /* OMD */
    uint8                bEffect;     //方法 127：催费告警投入-TRUE（催费告警参数同属性3）; 方法 128：取消催费告警-FALSE
}CTRL_CALLFEE_ARG_T;

/* 时段功控 */
typedef struct tagCtrlPeriodArg
{
    /* OAD */
    OOP_8103_CONFIG_T    param;       //0x81030200 控制方案集
    OOP_TGSTATES_T       effectStatus;//0x81030300 控制投入状态
    OOP_TURNSTATES_T     outputStatus;//0x81030400 控制输出状态
    OOP_TGSTATES_T       alarmStatus; //0x81030500 越限告警状态

    /* OMD */
    uint8                bEffect[OOP_MAX_TG]; //方法 6：投入-TRUE; 方法 7：解除-FALSE
    OOP_POWCON_PERIOD_T  project[OOP_MAX_TG]; //方法 127：时段功控方案切换
}CTRL_PERIOD_ARG_T;

/* 厂休控 */
typedef struct tagCtrlRestArg
{
    /* OAD */
    OOP_8104_CONFIG_T    param;       //0x81040200
    OOP_TGSTATES_T       effectStatus;//0x81040300 控制投入状态
    OOP_TURNSTATES_T     outputStatus;//0x81040400 控制输出状态
    OOP_TGSTATES_T       alarmStatus; //0x81040500 越限告警状态

    /* OMD */
    uint8                bEffect[OOP_MAX_TG];//方法 6：投入-TRUE; 方法 7：解除-FALSE
}CTRL_REST_ARG_T;

/* 营业报停控 */
typedef struct tagCtrlStopArg
{
    /* OAD */
    OOP_8105_CONFIG_T    param;       //0x81050200
    OOP_TGSTATES_T       effectStatus;//0x81050300 控制投入状态
    OOP_TURNSTATES_T     outputStatus;//0x81050400 控制输出状态
    OOP_TGSTATES_T       alarmStatus; //0x81050500 越限告警状态

    /* OMD */
    uint8                bEffect[OOP_MAX_TG];//方法 6：投入-TRUE; 方法 7：解除-FALSE
}CTRL_STOP_ARG_T;

/* 当前功率下浮控 */
typedef struct tagCtrlDownArg
{
    /* OAD */
    OOP_TGSTATES_T       effectStatus;//0x81060300 控制投入状态
    OOP_TURNSTATES_T     outputStatus;//0x81060400 控制输出状态
    OOP_TGSTATES_T       alarmStatus; //0x81060500 越限告警状态

    /* OMD */
    uint8                bEffect[OOP_MAX_TG];//方法 6：投入-TRUE; 方法 7：解除-FALSE
    OOP_POWCON_LOW_T     project[OOP_MAX_TG];//方法 127: 投入（参数） bEffect-TRUE

    /* 中间处理数据 获取到更新的数据(或上电首次读取)中设置 备份到文件/铁电中 todo */
    uint8                bFirst[OOP_MAX_TG];      //下发首次未执行标记
    time_t               starttime[OOP_MAX_TG];   //下发时间
    uint8                freezeTime[OOP_MAX_TG];  //剩余冻结时间(分钟)
}CTRL_DOWN_ARG_T;

/* 购电控 */
typedef struct tagCtrlBuyArg
{
    /* OAD */
    OOP_8107_CONFIG_T    param;       //0x81070200 购电控 
    OOP_TGSTATES_T       effectStatus;//0x81070300 控制投入状态
    OOP_TURNSTATES_T     outputStatus;//0x81070400 控制输出状态
    OOP_TGSTATES_T       alarmStatus; //0x81070500 越限告警状态

    /* OMD */
    uint8                bEffect[OOP_MAX_TG];//方法 6：投入-TRUE; 方法 7：解除-FALSE
}CTRL_BUY_ARG_T;

/* 月电控 */
typedef struct tagCtrlMonthArg
{
    /* OAD */
    OOP_8108_CONFIG_T    param;       //0x81080200 月电控 
    OOP_TGSTATES_T       effectStatus;//0x81080300 控制投入状态
    OOP_TURNSTATES_T     outputStatus;//0x81080400 控制输出状态
    OOP_TGSTATES_T       alarmStatus; //0x81080500 越限告警状态

    /* OMD */
    uint8                bEffect[OOP_MAX_TG];//方法 6：投入-TRUE; 方法 7：解除-FALSE
}CTRL_MONTH_ARG_T;


/* 控制所有参数 */
typedef struct tagCtrlArgAll
{
    CTRL_REMOTE_ARG_T     tRemote;     //远程控制参数
    CTRL_PROTECT_ARG_T    tProtect;    //保电参数

    CTRL_CALLFEE_ARG_T    tCallFee;    //催费告警
    int64                 safevalue;   //0x81000200 终端保安定值 
    OOP_POWCON_TIME_T     tPowconTime; //0x81010200 终端功控时段
    OOP_POWCON_WARNING_T  tPowconWarn; //0x81020200 功控告警时间

    CTRL_PERIOD_ARG_T     tPeriod;     //时段功控
    CTRL_REST_ARG_T       tRest;       //厂休控
    CTRL_STOP_ARG_T       tStop;       //营业报停控
    CTRL_DOWN_ARG_T       tDown;       //当前功率下浮控

    CTRL_BUY_ARG_T        tBuy;        //购电控
    CTRL_MONTH_ARG_T      tMonth;      //月电控
}CTRL_ARG_ALL_T;

#define SUBGROUP_SLIP_MAX   60
/* 总加组 */
typedef struct tagCtrlSubGroup
{
    OOP_TGSETS_T          argList;     //0x230x0200 总加组配置表
    int64                 actPower;    //0x230x0300 总加组有功功率
    int64                 ractPower;   //0x230x0400 总加组无功功率
    int64                 aveActPower; //0x230x0500 总加组滑差时间内平均有功功率
    int64                 aveRactPower;//0x230x0600 总加组滑差时间内平均无功功率
    OOP_HENERGYS_T        dayActEn;    //0x230x0700 总加组日有功电量
    OOP_HENERGYS_T        dayRactEn;   //0x230x0800 总加组日无功电量
    OOP_HENERGYS_T        monActEn;    //0x230x0900 总加组月有功电量
    OOP_HENERGYS_T        monRactEn;   //0x230x0a00 总加组月无功电量
    int64                 leftEn;      //0x230x0b00 总加剩余电量(费)    控制写入
    int64                 downActPower;//0x230x0c00 当前功率下浮控控后总加组有功功率冻结值
    uint8                 pTime;       //0x230x0d00 总加组滑差时间周期
    OOP_BITSTR8_T         ctrlPwTurn;  //0x230x0e00 总加组功控轮次配置  控制读取
    OOP_BITSTR8_T         ctrlEcTurn;  //0x230x0f00 总加组电控轮次配置  控制读取
    OOP_TGSETTING_T       ctrlStatus;  //0x230x1000 总加组控制设置状态  控制写入
    OOP_TGCURSTATE_T      curCtrlStatus;//0x230x1100总加组当前控制状态  控制写入

    /* 中间数据数据 */
    uint8                 slipnum;                         //滑差次数
    int64                 actPowerList[SUBGROUP_SLIP_MAX]; //1min有功功率记录
    int64                 slipPower;                       //功率定值
    int64                 last1minEn;                      //前1min电量
    uint32                storeNo;                         //前1min电量编号
}CTRL_SUBGROUP_T;

/* 功控跳闸记录 */
typedef struct tagCtrlPWEvent
{
    OOP_PCTRLLOG_T        record;      //0x32000200 功控事件记录单元 这是数组 需要修改
    OOP_OADS_T            oadsets;     //0x32000300 关联对象属性表
    uint16                curCount;    //0x32000400 当前记录数
    uint16                maxCount;    //0x32000500 最大记录数
    void                 *pParam;      //0x32000600 配置参数 通过其他OAD获取
    OOP_EVTSTAT_T         curValue;    //0x32000700 当前值记录表
    uint8                 reportFlag;  //0x32000800 上报标识
    uint8                 bEffect;     //0x32000900 有效标识
    OOP_EVTTIME_T         timeStatus;  //0x32000a00 时间状态记录表
}CTRL_PW_EVENT_T;

/* 电控跳闸记录 */
typedef struct tagCtrlECEvent
{
    OOP_ECTRLLOG_T        record;      //0x32010200 电控事件记录单元 这是数组 需要修改
    OOP_OADS_T            oadsets;     //0x32010300 关联对象属性表
    uint16                curCount;    //0x32010400 当前记录数
    uint16                maxCount;    //0x32010500 最大记录数
    void                 *pParam;      //0x32010600 配置参数 通过其他OAD获取
    OOP_EVTSTAT_T         curValue;    //0x32010700 当前值记录表
    uint8                 reportFlag;  //0x32010800 上报标识
    uint8                 bEffect;     //0x32010900 有效标识
    OOP_EVTTIME_T         timeStatus;  //0x32010a00 时间状态记录表
}CTRL_EC_EVENT_T;

/* 电控告警事件记录 */
typedef struct tagCtrlECAlarmEvent
{
    OOP_EWARNLOG_T        record;      //0x32030200 电控告警事件单元 这是数组 需要修改
    OOP_OADS_T            oadsets;     //0x32030300 关联对象属性表
    uint16                curCount;    //0x32030400 当前记录数
    uint16                maxCount;    //0x32030500 最大记录数
    void                 *pParam;      //0x32030600 配置参数 通过其他OAD获取
    OOP_EVTSTAT_T         curValue;    //0x32030700 当前值记录表
    uint8                 reportFlag;  //0x32030800 上报标识
    uint8                 bEffect;     //0x32030900 有效标识
    OOP_EVTTIME_T         timeStatus;  //0x32030a00 时间状态记录表
}CTRL_ECALARM_EVENT_T;

/* 遥控跳闸记录 */
typedef struct tagCtrlRemoteEvent
{
    OOP_RCTRLLOG_T        record;      //0x31150200 遥控事件记录单元 这是数组 需要修改
    OOP_OADS_T            oadsets;     //0x31150300 关联对象属性表
    uint16                curCount;    //0x31150400 当前记录数
    uint16                maxCount;    //0x31150500 最大记录数
    void                 *pParam;      //0x31150600 配置参数 通过其他OAD获取
    OOP_EVTSTAT_T         curValue;    //0x31150700 当前值记录表
    uint8                 reportFlag;  //0x31150800 上报标识
    uint8                 bEffect;     //0x31150900 有效标识
    OOP_EVTTIME_T         timeStatus;  //0x31150a00 时间状态记录表
}CTRL_REMOTE_EVENT_T;

/* 事件 */
typedef struct tagCtrlEvent
{
    CTRL_PW_EVENT_T      tPwEvent;     //功控跳闸记录
    CTRL_EC_EVENT_T      tEcEvent;     //电控跳闸记录
    CTRL_ECALARM_EVENT_T tEcAlarm;     //电控告警事件记录
    CTRL_REMOTE_EVENT_T  tRemote;      //遥控跳闸记录
}CTRL_EVENT_T;

/* 控制统计信息 */
typedef struct tagCtrlSTATIC
{
    uint8   dayRemoteCount;   //日遥控次数
    uint8   dayPwTripCount;   //日功控跳闸次数
    uint8   dayRestTripCount; //日厂休控跳闸次数
    uint8   dayBuyTripCount;  //日购电控跳闸次数
    uint8   datMonTripCount;  //日月电控跳闸次数

    uint8   monRemoteCount;   //月遥控次数
    uint8   monPwTripCount;   //月功控跳闸次数
    uint8   monRestTripCount; //月厂休控跳闸次数
    uint8   monBuyTripCount;  //月购电控跳闸次数
    uint8   monMonTripCount;  //月月电控跳闸次数
}CTRL_STATIC_T;

typedef struct tagSwitchInterface
{
    uint8  TURN_CTRL[OOP_MAX_TURN];     //轮次控制输出 0-动作,1-取消动作
    uint8  TURN_REDLIGHT[OOP_MAX_TURN]; //轮次红灯 0-熄灭,1-点亮
    uint8  TURN_BLUELIGHT[OOP_MAX_TURN];//轮次绿灯 0-熄灭,1-点亮
    uint8  POWER_CTRL_LIGHT ;           //功控灯
    uint8  ELE_CTRL_LIGHT;              //电控灯
    uint8  ALARM_OUTPUT;                //告警继电器 0-动作,1-取消动作
    uint8  PROTECT_LIGHT;               //保电灯
    uint8  INPUT_STATUS[OOP_MAX_TURN];  //接线状态
    uint8  Attri_OPEN[OOP_MAX_TURN];    //跳闸属性
    uint8  Ctrl_STATUS[OOP_MAX_TURN];   //投入状态 0-解除,1-投入

    uint8  portMax;                     //端口数量
    uint8  CLOSE_CTRL[OOP_MAX_TURN];    //合闸继电器0-动作,1-取消动作
    uint8  CLOSE_INPUT[OOP_MAX_TURN];   //合闸继电器接线状态
    uint8  Attri_CLOSE[OOP_MAX_TURN];   //合闸属性
}SWITCH_INTERFACE_T;

typedef struct
{    
    uint8 bSwitchState[OOP_MAX_TG];     //-闸当前状态-
    uint8 bRemoteAState;                //-遥控拉闸延时告警状态-
    uint8 bRemoteSState;                //-遥控跳闸输出状态-
    uint8 bRemoteCAState;               //-遥控合闸延时告警状态-
    uint8 bRemoteCState;                //-遥控合闸输出状态-
    uint8 bPowerAState[OOP_MAX_TG];     //-功控越限告警状态-
    uint8 bPowerSState[OOP_MAX_TG];     //-功控跳闸输出状态-
    uint8 bDownAState[OOP_MAX_TG];      //-下浮控越限告警状态-
    uint8 bDownSState[OOP_MAX_TG];      //-下浮控跳闸输出状态-
    uint8 bStopAState[OOP_MAX_TG];      //-营业报停控越限告警状态-
    uint8 bStopSState[OOP_MAX_TG];      //-营业报停控跳闸输出状态-
    uint8 bRestAState[OOP_MAX_TG];      //-厂休控越限告警状态-
    uint8 bRestSState[OOP_MAX_TG];      //-厂休控跳闸输出状态-
    uint8 bSegAState[OOP_MAX_TG];       //-时段控越限告警状态-
    uint8 bSegSState[OOP_MAX_TG];       //-时段控跳闸输出状态-
    uint8 bMonthENAState[OOP_MAX_TG];   //-月电控越限告警状态-
    uint8 bMonthENSState[OOP_MAX_TG];   //-月电控跳闸输出状态-
    uint8 bBuyENAState[OOP_MAX_TG];     //-购电控越限告警状态-
    uint8 bBuyENSState[OOP_MAX_TG];     //-购电控跳闸输出状态-
}UN_BRAKE_DATA_T;    //-控制状态数据结构-

typedef struct tagSwitchCtrlTIme
{
    uint8  no;
    uint8  curstatus;
    uint8  futurestatus;
    uint8  balarm;
    uint8  hour;
    uint8  min;
    uint8  sec;
}SWITCH_CTRL_TIME_T;


typedef uint8 (*CTRL_SHOW_HINT)(uint8, uint8, uint8, uint8*, uint16, uint16*);

void appctrl_get_argcopy(CTRL_ARG_ALL_T **ppCtrlArg, CTRL_SUBGROUP_T **ppSubGroup, CTRL_STATIC_T **ppCtrlStatic);
void appctrl_pcstop_alarm(uint8 pckind, uint8 grp,uint8 turn);
void appctrl_pcclose_break(uint8 pctype, uint8 grp, uint8 turn);
uint8 appctrl_get_switch_status(uint8 bigType, uint8 type, uint8 turn, uint8 grp);
void appctrl_set_switch_status(uint8 bigType, uint8 type, uint8 turn, uint8 grp, uint8 status);
uint32 appctrl_get_enprice();
void appctri_write_freezepower(uint8 group, int64 value);
int appctrl_write_leften(uint8 group, int64 leftEn);
int appctrl_get_leften(uint8 group);
void appctrl_write_subgroupstatus(uint8 group, OOP_TGCURSTATE_T *pStatus);
void appctrl_write_outstatus(uint8 ctrltype, OOP_TURNSTATES_T *pStatus);
void appctrl_write_alarmstatus(uint8 ctrltype, OOP_TGSTATES_T *pStatus);
uint16 appctrl_get_object(uint8 ctrltype);
void appctrl_set_protect_status(uint8 status);
void appctrl_set_remote_status(uint16 alarm, uint16 trip);
uint8 appctrl_get_neweffect(uint8 *ctrltype, uint16 *infonum, int *index);
void appctrl_no_neweffect();
uint8 appctrl_cal_period(OOP_POWCON_TIME_T *ptPowconTime);
void appctrl_set_oopswitch(uint8 acm1, uint8 acm2, uint8 trip, uint8 close);
void appctrl_set_oopswitch_input(uint8 acm1, uint8 acm2, uint8 *input, uint8 *selfinput);
uint8 appctrl_get_switch_type(uint8 no);
void appctrl_set_callfee_status(uint8 state);
void appctrl_set_down_close(uint8 group);
void appctrl_set_callfee_data(uint8 state, OOP_CALLFEE_T *pFee);


void appctrl_ctrl_buzzer(int dbClient, uint8 bOpen);
void appctrl_ctrl_led(LED_INDEX_E index, uint8 bOpen);
void appctrl_ctrl_ZCUS_turn_led(uint8 index, uint8 bOpen);

uint8 appctrl_protect_status();
uint32 appctrl_get_warning_id();
void appctrl_alarm_data(uint8 ctrltype, uint8 group, uint8 times, uint8 Voindex, CTRL_SHOW_HINT pfunc);
void appctrl_alarm_stop();
void appctrl_show_guistatus(uint32 delaytime, uint8 *pShow, uint16 showlen);
void appctrl_show_guiinfo(uint16 info);
void appctrl_show_guihint(uint32 delaytime, uint8 *pShow, uint16 showlen);
void appctrl_alarm_zcu1(uint8 status);
uint8 appctrl_alarm_zcu1_get();


uint8 appctrl_power_guishow(uint8 ctrltype, uint8 group, uint8 turnmax, uint8* pBuf, uint16 buflen, uint16* msglen);
uint8 appctrl_elec_guishow(uint8 ctrltype, uint8 group, uint8 turnmax, uint8* pBuf, uint16 buflen, uint16* msglen);
uint8 appctrl_remote_guishow(uint8 ctrltype, uint8 group, uint8 turnmax, uint8* pBuf, uint16 buflen, uint16* msglen);
uint8 appctrl_dev_input(uint8 turnno);

uint8 appctrl_check_power_status();
uint8 appctrl_check_elec_status();
void appctrl_db_chg_event(uint8 type, uint8 dbtype, uint32 value);
void appctrl_cal_power_clear(uint8 grp, CTRL_SUBGROUP_T *ptSubGroup);
uint8 appctrl_get_power_type(uint8 grp);
void appmain_write_warn_status(OOP_WARNINGOUT_T *groupstatus);

extern int sta_tg_min_acte_read(OI tgOI, int64 *acteTotal, uint32 *storeNo);
extern int appmain_send_msg(uint32 msgid, uint16 label, uint8 *pMsg, uint16 msglen);
extern void remain_energy_lock(void);
extern void remain_energy_unlock(void);
extern int remain_energy_write(int clientid, uint16 vir_no, int64 energy);
extern int remain_energy_read(int clientid, uint16 vir_no, int64* energy);

#ifdef __cplusplus
}
#endif

#endif


