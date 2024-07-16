/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：EventFun.h
* 版本号：1.0
* 功能：对事件检测过程中用到的数据结构，调度函数进行声明
* 原始开发人及时间：董宁 2006年12月25日
* 修改人员、时间及修改内容:无
* 其他：
******************************************************************-*/
#ifndef _EVENTFUN_H
#define _EVENTFUN_H

typedef struct
{
    uint8   power_type;                         //测量点接线方式
    uint16  cur_a;                              //A相电流
    uint16  cur_b;                              //B相电流
    uint16  cur_c;                              //C相电流
    uint32  cur_gate;                           //额定电流-从数据字典读出后*10,方便和3位小数的电流比较
    uint16  vlt_gate;                           //额定电压
    uint32  cur_lmt;                            //电压断相、失压时电流限值-
    uint16  vlt_a;                              //A相电压
    uint16  vlt_b;                              //B相电压
    uint16  vlt_c;                              //C相电压
    uint32  fw_act_en;                          //当前正向有功电能
    uint8   batt_status;                        //电池状态
    uint8   cb_interval;                        //抄表间隔
}EVENT_GLOBAL_T;

//宏定义
#define BIT0                        0x01
#define BIT1                        0x02
#define BIT2                        0x04
#define BIT3                        0x08
#define BIT4                        0x10
#define BIT5                        0x20
#define BIT6                        0x40
#define BIT7                        0x80

//事件标记定义
#define BREAK_CIRCUIT               0x00001     //电流开路
#define CUR_RVS                     0x00002     //电流反极性
#define BREAK_PHASE                 0x00004     //电压断相
#define LOSE_VOLT                   0x00008     //电压缺相（失压）
#define PHASE_REVER                 0x00010     //逆相序
#define SUPASS_VLT                  0x00020     //电压超上限
#define LOW_VLT                     0x00040     //电压低下限
#define HIGH_CUR                    0x00080     //电流超上限
#define HIGHER_CUR                  0x00100     //电流超上上限
#define HIGH_POW                    0x00200     //视在功率超上限
#define HIGHER_POW                  0x00400     //视在功率超上上限
#define CUR_NBLC                    0x00800     //电流不平衡
#define VLT_NBLC                    0x01000     //电压不平衡
#define METTM_ERROR                 0x02000     //电能表时间超差
#define POWER_DIFF                  0x04000     //差动组超差
#define STOP_POWER                  0x08000     //电能表停电
#define MET_EN_DOWN                 0x10000     //电能表示度下降检测、记录
#define MET_EN_OUT                  0x20000     //电能量超差检测、记录
#define MET_EN_FLY                  0x40000     //电能表飞走检测、记录
#define MET_EN_STOP                 0x80000     //电能表停走、检测记录

#define SHORT_CIRCUIT               0x100000    //电流短路

#define VLT_HR_PEC_UP_LMT           0x200000    //电压谐波含有率越上限
#define CUR_HR_VAL_UP_LMT           0x400000    //电流谐波有效值越上限

#define MET_EN_RM_FAILURE           0x800000     //电能表抄表失败记录  ERC31
#define METTM_ERROR_M               0x1000000     //电能表时间偏差

#define SEG_EVENT                   0x01        //电表时段费率更改
#define PROG_TIME_EVENT             0x02        //电表编程时间更改
#define CB_DAY_EVENT                0x04        //电表抄表日更改
#define CONST_EVENT                 0x08        //电表脉冲常数更改
#define CTPT_EVENT                  0x10        //电表互感器倍率更改
#define DM_ZERO_EVENT               0x20        //电表最大需量时间清零

#define PROG_DM_EVENT               0x01        //编程次数或需量清零次数变化
#define PHASE_BREAK_EVENT           0x02        //电压断相次数变化
#define VLT_LOSE_EVENT              0x04        //电压失压次数变化
#define STOP_POWER_EVENT            0x08        //停电次数变化
#define BAT_LOWER_EVENT             0x10        //电池欠压

#define CB_ERROR_EVENT              0x04        //终端故障只检测所有485表能否抄通,如果全不通，则置此标志
#define RTC_ERROR_EVENT             0x08        //终端故障检测 时钟电池故障，欠压
#define BT_ERROR_EVENT              0x10        //终端故障检测电池故障，则置此标志


#define A_PHASE                     0x01        //a相
#define B_PHASE                     0x02        //b相
#define C_PHASE                     0x04        //c相

#define DELAY       40  //上电启动延时，检查停上电时，上电时间需要去掉启动延时，防止台体测试不过
#define NEW_TNM     1  //新终端

#define SEC_MONITOR_NEW_22  1  //22版本终端新安全在线检测

extern uint8 a_ptrl[5],b_ptrl[5],c_ptrl[5];//-规约数据格式-


typedef enum
{
    RELAY_IDEL,                  //空闲
    RELAY_SEND,                  //发送
    RELAY_WATT,                  //等待接收
    RELAY_RECVD,                 //接收完成
    RELAY_FAIL,                  //接收失败
}RELAY_STATE_T; //中继转发相关的状态信息

typedef enum
{
    HALT_IDLE,         //电表停电抄读空闲
    HALT_INIT,         //电表停电抄读初始化
    HALT_RELAYD,       //电表停电抄读中继
    HALT_RETRY,        //电表停电抄读中继重试
    HALT_DEAL,         //电表停电抄读处理
}METER_HALT_STATE_T; //中继转发相关的状态信息

typedef struct
{
    mtrnum_t                Pn;                 //测量点号
    OOP_TSA_T               Addr;               //电表地址
    OOP_OAD_U               commnum;            //通讯端口号
    uint8                   protocol;           //通信规约类型
    uint8                   baud;               //通信模式字
    uint8                   ctrtype;            //控制位
    uint8                   RelayResult;        //中继穿透结果
}METER_HALT_INFO_STRUCT;

typedef struct
{
    METER_HALT_STATE_T      MeterHalt_Stat;     //
    RELAY_STATE_T           Relaydstat;         //中继状态
    uint8                   CheckMeterHalt;     //0:定时检测   1:上电检测
    uint8                   MeterHaltNum;       //电表停上电抄读的数目
    uint8                   MeterHaltIndex;     //电表停上电抄读的索引
    uint8                   MeterIdNum;         //抄读的ID个数
    time_t                  HaltTaskStartTime;  //任务起始时间
    METER_HALT_INFO_STRUCT  MeterHaltInfo[6];   //电表停上电抄读的信息
}METER_HALT_STAT_STRUCT;


#define TASK_MAX_NUM    255                 //最大任务数

//事件主动上报
#define EVENT_RECORD_FILE     "event_record_file"            //事件主动上报的存储文件
#define EVENT_RECORD_MAX_NUM    200         //事件上报最大记录数量
#define EVENT_RECORD_MAX_LEN    1024        //事件上报数据最大长度
#define EVENT_REPORT_FILE_HEAD_LEN    4     //事件上报文件头长度
#define EVENT_RECORD_CNT_OFFSET    0        //记录总数的偏移位置
#define EVENT_REPORT_CNT_OFFSET    2        //已上报总数的偏移位置


//事件上报标志
#define EVENT_NO_REPORT         0     //不上报（0），
#define EVENT_HAPPENED_REPORT   1     //事件发生上报（1），
#define EVENT_RESTORE_REPORT    2     //事件恢复上报（2），
#define EVENT_ALL_REPORT        3     //事件发生恢复均上报（3）


//事件上报
typedef struct
{
    uint32              dataLen;                        //数据长度
    uint8               cAddr;                          //主站地址 1-上报至在线安全监测系统 0-上报至用采主站
//    uint8               evtType;                        //事件类型，1 事件发生，2事件恢复
//    OOP_OAD_U           eventOAD;                       //事件oad
//    OOP_RCSD_T          eventRCSD;                      //事件上报RCSD
//    uint32              storagenum;                     //存储序号
    uint8               pdataBuf[EVENT_RECORD_MAX_LEN]; //上报数据
}EVENT_REPORT_INFO;

//事件公共信息
typedef struct
{
    OOP_OAD_U           eventOAD;                       //事件标识
    uint8               checkflag;                      //有效标识
    uint8               reportflag;                     //上报标识
    uint32              recordnum;                      //记录序号
    uint16              eventNum;                       //事件记录数
    uint32              EventStoragNumRecAddr;          //存储序号 起始地址
    uint32              EventStoragTmRecAddr;           //发生时间 起始地址
    uint32              EventStoragIndexAddr;           //记录序号 起始地址
    uint32              EventFlagAddr;                  //事件发生标志 起始地址
    uint16              EventFlagLen;                   //事件发生标志 长度
    OOP_RCSD_T          eventRCSD;                      //事件上报RCSD
    OOP_DATETIME_S_T    starttime;                      //发生时间
    OOP_DATETIME_S_T    endtime;                        //结束时间
    uint32              storagenum;                     //存储序号
    EVTSTATE            eventState;                     //事件当前值
    EVTTIME             eventTime;                      //事件时间状态记录表
    uint8               bType;                          //事件类型，终端 0 涉及到电能表1
}EVENT_COMM_INFO;

typedef struct
{
    EVENT_COMM_INFO eventCommInfo;
    OOP_VERSION_T   lastversion;   //上个版本号
    OOP_VERSION_T   version;       //版本号
    uint8           ishappen;      //状态标记，标记事件是否发生
    uint8           initflag;      //参数初始化标记，标识参数是否初始化
}EVENT_3101_INFO;

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    uint8             bRecType;         //复位标志
    uint8             ishappen;         //状态标记，标记事件是否发生
    uint8             initflag;         //参数初始化标记，标识参数是否初始化
}EVENT_3100_INFO;

typedef struct
{
    EVENT_COMM_INFO         eventCommInfo;
    OOP_SWITCHIN_STATE_T    switchState;
    OOP_SWITCHIN_FLAG_T     switchFlag;
    uint32                  Stateaftaqutitydce[4];     //状态量变位发生后状态
    uint8                   ishappen;                  //状态标记，标记事件是否发生
    uint8                   initflag;                  //参数初始化标记，标识参数是否初始化
}EVENT_3104_INFO;

typedef struct
{
    EVENT_COMM_INFO      eventCommInfo;
    OOP_EVTCFG_3105_T    checkPara;    //配置参数
    OOP_TI_T             oopTI;        //时间间隔
    OOP_DATETIME_S_T     mettime;      //测量点当前时间
    OOP_DATETIME_S_T     tmntime;      //终端当前时间
    uint8                initflag;                     //参数初始化标记，标识参数是否初始化
}EVENT_3105_INFO;

typedef struct
{   
    EVENT_COMM_INFO     eventCommInfo;
    OOP_POWERONCUT_T    eventParam;    //事件配置参数
    OOP_WORD3V_T       tmnVoltage;    //终端电压
    OOP_DATETIME_S_T    tmnStarttime;      //终端停电发生时间
    OOP_DATETIME_S_T    tmnEndtime;        //终端停电结束时间
    OOP_DATETIME_S_T    metStarttime[OOP_MAX_POWERCUT_MET];      //电表停电发生时间
    OOP_DATETIME_S_T    metEndtime[OOP_MAX_POWERCUT_MET];        //电表停电结束时间
    int8                bfgPower;      //-电池状态标志-
    uint8               eventFlag;     //事件属性标志 bit0 置“1”：事件正常， bit0 置“0”：事件异常；bit1 置“1”：事件有效， bit1 置“0”：事件无效；
    uint8               ishappen;      //状态标记，0x55-停电已经发生 0xAA-停电未发生
    uint8               initflag;      //参数初始化标记，标识参数是否初始化
#ifdef AREA_FUJIAN
	OOP_BITSTR16_T		 powerStatus;		//当前电源状态字
	OOP_BATTERY_STATUS_T batteryStatus;		//停电工作电池状态
	uint8				 rptFlag;			//停电上报标记 <      0x3c 等待上报， 0x55 立即上报，0xaa 已经上报
	uint8				 tmFlag;			//停电后持续工作时间超过3分钟标记， 0x55 超过3分钟，0xaa 未超过
#endif

}EVENT_3106_INFO;

typedef struct
{
    EVENT_COMM_INFO     eventCommInfo;
    OOP_EVTCFG_3110_T   eventParam;         //事件配置参数 通信流量门限
    OOP_DWORD2_T        recvdataFlowrate;   //通信流量
    uint8               ishappen;           //状态标记，标记事件是否发生
    uint8               initflag;           //参数初始化标记，标识参数是否初始化
}EVENT_3110_INFO;

typedef struct
{
    EVENT_COMM_INFO     eventCommInfo;
    uint32              MetNum;                 //搜表上报个数
    uint8               SerchStat;              //搜表状态 0空闲，1搜表中
    OOP_SEARCHMET_RES_T Res[OOP_MAX_SMREPORT];  //搜表结果
    uint8               ishappen;               //状态标记，标记事件是否发生
    uint8               initflag;               //参数初始化标记，标识参数是否初始化
}EVENT_3111_INFO;

enum RTCSource{

      erminalmotherfa  =0,
      Clockfailure           =  1,
      Mothebcounicatifailure=2,
      meterreadingfailure=3,
      Displaypanelfailure=4,
      CarrierChannelAnomaly=5,
      Memoryinitializationerror=6,
      ESAMerror=7,
      Clockbatteryfailure=8,
      Backupbattefailure=9
};

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    uint8             ishappen;      //状态标记，标记事件是否发生
    uint8             initflag;      //参数初始化标记，标识参数是否初始化
}EVENT_31XX_INFO;

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    uint8             ishappen;      //状态标记，标记事件是否发生
    uint8             initflag;      //参数初始化标记，标识参数是否初始化
    OOP_DATETIME_S_T  preTime;      //校时前时钟
    char              offset;       //时钟误差
}EVENT_311B_INFO;

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    uint8             ishappen;      //状态标记，标记事件是否发生
    uint8             initflag;      //参数初始化标记，标识参数是否初始化
    OOP_REFUSESLAVE_T refuseslave ;      //拒绝从节点列表

}EVENT_311F_INFO;

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    uint8             ishappen;      //状态标记，标记事件是否发生
    uint8             initflag;      //参数初始化标记，标识参数是否初始化
    OOP_ACROSSAREA_RES_T acrossarea ;      //垮台区电表信息

}EVENT_3112_INFO;


typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    uint8             ishappen;      //状态标记，标记事件是否发生
    uint8             initflag;      //参数初始化标记，标识参数是否初始化
    OOP_CHILDNODE_T   prenode ;      //变更前子节点信息
    OOP_CHILDNODE_T   nownode;      //变更后子节点信息

}EVENT_3030_INFO;

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    uint8             ishappen;      //状态标记，标记事件是否发生
    uint8             initflag;      //参数初始化标记，标识参数是否初始化
    Oop_Identify_Result IdentifyInformations;
}EVENT_3E03_INFO;


typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    uint8             nTID;         //关联任务序号
    OOP_EVTCFG_310X_T eventParam;   //事件配置参数
    OOP_EVTCFG_310E_T event310eParam;   //事件配置参数
    OOP_TI_T          oopTI;        //时间间隔
    uint32            lastEnergy;  //上次正向有功总电能
    uint32            nowEnergy;   //当前正向有功总电能
    uint8             initflag;                       //参数初始化标记，标识参数是否初始化
}EVENT_310X_INFO;

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo; 
    OOP_EVTCFG_310F_T eventParam;   //事件配置参数
    OOP_TI_T          oopTI;        //时间间隔
    RELAY_STATE_T     relayState[NO_VIR_MTR_MAX_NUM];     //当日抄读状态 消息状态
    uint8             errNum[NO_VIR_MTR_MAX_NUM];         //当日抄读状态错误次数
    uint8             recodFlag[NO_VIR_MTR_MAX_NUM];      //-抄表成功标志55为成功，aa表示不成功            
    OOP_DATETIME_S_T  recodTime;      //抄表时间               
    OOP_ENERGY_T      oopEnergy;      //正向有功总电能
    OOP_ENERGYS_T     oopEnergyS;     //正向无功总电能
    uint8             initflag;                       //参数初始化标记，标识参数是否初始化
}EVENT_310F_INFO;

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    uint8             ishappen;         //状态标记，标记事件是否发生
    uint8             initflag;         //参数初始化标记，标识参数是否初始化
    uint32            nOADNum;          //
    OOP_OAD_U         oad[OOP_MAX_OADS];//
}EVENT_3118_INFO;


typedef struct
{
    EVENT_COMM_INFO   eventCommInfo; 
    OOP_WORD4V_T      oopWord4V[MTR485_MAX_NUM];    //事件发生时电压相角
    OOP_WORD4V_T      oopWord4C[MTR485_MAX_NUM];    //事件发生时电压电流相角
    OOP_ENERGY_T      oopEnergys[MTR485_MAX_NUM];   //事件发生时正向有功总电能
    uint8             initflag;                     //参数初始化标记，标识参数是否初始化
}EVENT_3119_INFO;



typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    uint8             ishappen;         //状态标记，标记事件是否发生
    uint8             nDelay;           //判断延时
    uint8             initflag;         //参数初始化标记，标识参数是否初始化
    OOP_WORD3V_T      valTemp;          //电压
    OOP_METWORD_T     meterRsw;         //电表运行状态字7
}EVENT_300F_INFO;

typedef struct
{
    EVENT_COMM_INFO     eventCommInfo;
    OOP_CSD_T           di;
    uint8               preLen;
    uint8               pre[64];
    uint8               aftLen;
    uint8               aft[64];
    uint8               ishappen;         //状态标记，标记事件是否发生
    uint8               nTID;             //关联采集任务序号
    uint8               initflag;         //参数初始化标记，标识参数是否初始化
}EVENT_311C_INFO;

typedef struct
{
    EVENT_COMM_INFO         eventCommInfo;
    OOP_BITSTR16_T          flag;
    uint8                   nEthPort;
    uint16                  ethPort[OOP_MAX_SECURITYCHGS];
    uint8                   nEthConnects;
    OOP_CONNECTINFO_T       ethConnects[OOP_MAX_SECURITYCHGS];
    uint8                   nLtePort;
    uint16                  ltePort[OOP_MAX_SECURITYCHGS];
    uint8                   nLteConnects;
    OOP_CONNECTINFO_T       lteConnects[OOP_MAX_SECURITYCHGS];
    OOP_BITSTR8_T           usbInfo;
    OOP_BITSTR8_T           rs232Info;
    uint8                   nChgPath;
    OOP_VISIBLEVAR_T        chgPath[OOP_MAX_SECURITYCHGS];
    OOP_BITSTR8_T           ethFlag;
    OOP_BITSTR8_T           rs232Flag;
    OOP_BITSTR8_T           errFlag;

    #if SEC_MONITOR_NEW_22 == 1
    uint8                   errSSH;
    uint8                   flgSession;
    OOP_SECURITY_SESSION_T  errSession;
    uint8                   flgStream;
    OOP_SECURITY_STREAM_T   errStream;
    uint8                   flgProg;
    OOP_SECURITY_PROG_T     errProg;
    uint8                   flgLogin;
    OOP_SECURITY_LOGIN_T    errLogin;
    #endif
}EVENT_3140_INFO;

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
	uint8 			  rCtrlType;		 //遥控命令类型 0跳闸，1合闸，2直接合闸
    int64             prePower[8];       //控后2分钟总加组功率
    int64             llPower[8];        //控时2分钟总加组功率
    uint8             turn;              //控制轮次
    uint8             initflag;          //参数初始化标记，标识参数是否初始化
}EVENT_3115_INFO;

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    OI                objCtrl;
    OOP_BITSTR8_T     turnState;
    int64             power2Min;
    int64             llSetting;
    int64             power;
    uint8             turn;              //控制轮次
    uint8             initflag;         //参数初始化标记，标识参数是否初始化
}EVENT_3200_INFO;

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    OI                objCtrl;
    OOP_BITSTR8_T     turnState;
    int64             llSetting;
    int64             energy;
    uint8             ishappen;         //状态标记，标记事件是否发生
    uint8             initflag;         //参数初始化标记，标识参数是否初始化
}EVENT_3201_INFO;

typedef struct
{
    EVENT_COMM_INFO    eventCommInfo;
    OOP_CFGUNIT_810C_T cfgunit;          //购电控配置单元
    uint8              ishappen;         //状态标记，标记事件是否发生
    uint8              initflag;         //参数初始化标记，标识参数是否初始化
}EVENT_3202_INFO;


typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    OI                objCtrl;
    int64             llSetting;
    uint8             ishappen;         //状态标记，标记事件是否发生
    uint8             initflag;         //参数初始化标记，标识参数是否初始化
}EVENT_3203_INFO;

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
	uint8 			  warnType;			/**< 告警类别		*/
	OOP_VISIBLEVAR_T  containerName;	/**< 容器名称		*/
    OOP_VISIBLEVAR_T  appName;			/**< 应用名称		*/
	uint8          	  index;			//私有地址偏移
    uint8             ishappen;      //状态标记，标记事件是否发生
    uint8             initflag;      //参数初始化标记，标识参数是否初始化
}EVENT_360X_INFO;


typedef struct
{
    uint8               enabled;        //配置标记
    OOP_METER_T         oopMeter;       //采集档案
}OOP_METER_INFO;

typedef int (*MeterEventFunc)(DB_CLIENT,uint8 ,uint16 , OOP_METER_T ,uint8* ,uint16);

typedef struct 
{
    uint32  oadValue;
    uint32  dateLen;
    uint8   *pBuf;
    uint8   *pvaild;
    MeterEventFunc func;
}EventUpdateDataInfo;

typedef struct 
{   
    pthread_mutex_t      mutex;                             //锁 多线程访问
    OOP_DATETIME_S_T     meterReadTime[NO_VIR_MTR_MAX_NUM];     //抄表时间
    uint8                time_vaild[NO_VIR_MTR_MAX_NUM];        //有效标志
    OOP_INT3V_T          curTemp[NO_VIR_MTR_MAX_NUM];           //电流，
    uint8                cur_vaild[NO_VIR_MTR_MAX_NUM];         //有效标志
    OOP_WORD3V_T         valTemp[NO_VIR_MTR_MAX_NUM];           //电压
    uint8                val_vaild[NO_VIR_MTR_MAX_NUM];         //有效标志
    OOP_ENERGY_T         forwardActive[NO_VIR_MTR_MAX_NUM];     //正向有功电能
    uint8                Active_vaild[NO_VIR_MTR_MAX_NUM];      //有效标志
    OOP_ENERGYS_T        EnergyS[NO_VIR_MTR_MAX_NUM];           //组合无功1
    uint8                EnergyS_vaild[NO_VIR_MTR_MAX_NUM];     //有效标志
}LatestMeterReadData;

//事件处理结构体
typedef struct
{
    EVENT_COMM_INFO   *peventCommInfo;
    int               (*initFp)(void);   //事件的初始化回调函数
    void              (*cleanFp)(EVENT_COMM_INFO *peventCommInfo);   //事件的清理回调函数
} EVENT_MODULE_T;


extern LatestMeterReadData g_LatestMeterReadData;

extern EVENT_3100_INFO event_3100;   //终端初始化事件
extern EVENT_3101_INFO event_3101;   //终端版本号变更事件
extern EVENT_3104_INFO event_3104;   //终端状态量变位事件
extern EVENT_3105_INFO event_3105;   //电能表时间超差事件
extern EVENT_3106_INFO event_3106;   //终端停上电事件
extern EVENT_31XX_INFO event_3109;   //终端消息认证错误事件
extern EVENT_31XX_INFO event_310A;    //设备故障事件
extern EVENT_310X_INFO event_310B;   //电能表示度下降事件
extern EVENT_310X_INFO event_310C;   //电能表超差事件
extern EVENT_310X_INFO event_310D;   //电能表飞走事件
extern EVENT_310X_INFO event_310E;   //电能表停走事件
extern EVENT_310F_INFO event_310F;   //抄表失败事件
extern EVENT_3110_INFO event_3110;   //终端流量超限事件
extern EVENT_3111_INFO event_3111;   //发现未知电表事件
extern EVENT_31XX_INFO event_3114;   //终端对时事件
extern EVENT_3118_INFO event_3118;   //终端编程事件
extern EVENT_3119_INFO event_3119;   //电流回路异常事件
extern EVENT_300F_INFO event_300F;   //终端相序异常事件
extern EVENT_311C_INFO event_311C;   //电能表数据变更监控记录
extern EVENT_3140_INFO event_3140;   //安全事件变更记录
extern EVENT_3115_INFO event_3115;   //遥控跳闸事件记录
extern EVENT_3200_INFO event_3200;   //功控跳闸事件记录
extern EVENT_3201_INFO event_3201;   //电控跳闸事件记录
extern EVENT_3202_INFO event_3202;   //购电参数设置事件记录
extern EVENT_3203_INFO event_3203;   //电控告警事件记录
extern EVENT_311B_INFO event_311B; 
extern EVENT_311F_INFO event_311F;    //拒绝从节点入网记录
extern EVENT_3112_INFO event_3112; 
extern EVENT_3030_INFO event_3030; 
extern EVENT_3E03_INFO event_3E03;    //拓扑识别
extern EVENT_360X_INFO event_3600;
extern EVENT_360X_INFO event_3601;
extern EVENT_360X_INFO event_3602;


#if GB101_104_EN 
extern uint8 bfgCommType[MTR485_MAX_NUM];
#endif

extern METER_HALT_STAT_STRUCT gMeterHalt_Info;

extern OOP_OCTETVAR16_T    tmnAddr;                             //终端通信地址
extern OOP_WORD3V_T       tmnVoltage;                          //终端电压
extern OOP_OCTETVAR64_T    MAC;                                 //校验码
extern uint16              MTR485Num;                           //485测量点的个数
extern OOP_TASK_T          oopTask[TASK_MAX_NUM];               //任务配置表
extern OOP_METER_INFO      oopMeterInfo[NO_VIR_MTR_MAX_NUM];    //采集档案配置
extern DB_CLIENT           gDbCenterClient;                     //和数据中心交互的句柄(新)
extern BOOLEAN                g_tnmEventReport;                    //允许终端主动上报标志,1允许,0不允许
extern OOP_CHANNEL_T       g_tnmPort;
extern int8                g_PowerOff;


#define METER_STATUS_LABEL          0x13
extern int appmain_send_msg(uint32 msgid, uint16 label, uint8 *pMsg, uint16 msglen);

//函数声明
int GetCSDFromPlanByTaskID(uint8 taskID, OOP_RECORD_COL_T *pRecordCol);
int GetReadMeterStatus(OOP_TSA_T          tsa, mtrnum_t mtr_id);
int WriteEventToDB_fj(DB_CLIENT fd,mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, OOP_EVTBUF_T eventData, uint8 bType);

int WriteEventToDB(DB_CLIENT fd,mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, OOP_EVTBUF_T eventData, uint8 bType);
uint16 getMsgLenOffset(uint8 *indata,uint8 *bytes);
uint32 OopTIToSec(OOP_TI_T oopTI);
int GetTaskOopTI(uint8 taskID, OOP_TI_T *oopTI);
int evt_read_nomal_data(int clientid, uint8 logicid,uint16 infonum, uint32 value_oad, uint32 data_len, uint8 *pout_buf);
int evt_write_nomal_data(int clientid, uint8 logicid,uint16 infonum, uint32 value_oad, uint8 *pbuf, uint32 data_len);



int CheckTmn_Halt(void); //终端的停上电检测
int CheckMeter_Halt(void);//电表的停上电检测
int MergePowerOffTime(int checkFlag);//终端与电表停电记录对比
int dealMsgBuf(uint8 *msgBuf, uint16 msgLen); //处理接收的透传报文

int get_power_state(int *state);

int evt_check_csd_data(mtrnum_t mtr_id, OOP_OAD_U   oad, uint8* last2Data, uint8 *lastData); //检查CSD对应的数据是否发生变更
BOOLEAN check_search_meter_res(OOP_SEARCHMET_RES_T *res);

int get_evt_data_300F(int clientid, uint8 evt_flag, OOP_EVTBUF_T* pevt_data);   //获取事件关联数据
int WriteEventReportData(const char *filename, EVENT_COMM_INFO *eventCommInfo, uint8 *bDatabuf, uint32 bufLen);


void  FillEventGlobal(mtrnum_t mtr_id, OOP_METER_T oopMeter);                       //测量点数据全局结构初始化

void MetReadFailureEvent(mtrnum_t mtr_id, OOP_METER_T oopMeter);                  //电能表抄读失败记录

void  MetclockAbnormalEvent(int clientid, mtrnum_t mtr_id, OOP_METER_T oopMeter, uint8 bType,uint8* databuf,uint16 datalen);    //电能表时间超差检测、记录
void  EnDownEvent(int clientid, mtrnum_t mtr_id, OOP_METER_T oopMeter, uint32 en_now, uint32 en_last);                          //电能表示度下降检测、记录
void  MetJumpEvent(int clientid, mtrnum_t mtr_id, OOP_METER_T oopMeter, uint32 en_now, uint32 en_last);                     //电能量超差检测、记录
void  MetFlyEvent(int clientid, mtrnum_t mtr_id, OOP_METER_T oopMeter, uint32 en_now, uint32 en_last);                          //电能表飞走检测、记录
void  MetStopEvent(int clientid, mtrnum_t mtr_id, OOP_METER_T oopMeter, uint32 en_now, uint32 en_last);                     //电能表停走、检测记录


#endif
