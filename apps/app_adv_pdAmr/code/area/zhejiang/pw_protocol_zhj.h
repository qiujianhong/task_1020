/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2020-01-03
*********************************************************************
*/


#ifndef __PW_PROTOCOL_ZHJ_H__
#define __PW_PROTOCOL_ZHJ_H__

#include "pw_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

/*******************浙江地区专用参数与变量**********************/

/****************************************************************
***全局函数声明
****************************************************************/

#define MAX_EQUIP_TYPE_NUM             9           //设备类型最大种类
#define MAX_EQUIP_GROUP_NUM            2           //一类设备最大组数
#define MAX_EQUIP_COUNT_NUM            10           //一组设备最大个数

#define MAX_POINT_CALL_NUM             80           //点召唤单次最大信息体个数
#define MAX_PARA_CALL_NUM              80           //部分参数召唤单次最大信息体个数

//上报任务参数
#define MAX_REPORT_TASK_NUM       43           //上报任务最大数目
#define MAX_INFO_ITEM_NUM         80           //上报任务信息元素最大数目
#define REPORT_TASK_YX             1           //上报任务为遥信数据
#define REPORT_TASK_YC             2           //上报任务为遥测数据

//上报数据类型
#define TASK_DATA_NULL             0           //未知上送类型
#define TASK_DATA_REALTIME         1           //实时数据上送
#define TASK_DATA_FROZEN           2           //冻结数据上送

//信息体地址类型
#define INFO_ADDR_TYPE_NULL           0           //未知信息体类型
#define INFO_ADDR_TYPE_YX             1           //遥信地址
#define INFO_ADDR_TYPE_YC             2           //遥测地址
#define INFO_ADDR_TYPE_PARA           3           //参数定值地址
#define INFO_ADDR_TYPE_FAULT          4           //故障事件

//冻结数据存储深度
#define FROZEN_SAVE_DEPTH_MAX        50           //冻结数据存储深度

//定值区数量
#define MAX_AREA_NUM_ZHJ             2            //定值区数量

//点表各类信号最大数量(一维数组)
#define EQGROUP (MAX_EQUIP_GROUP_NUM) //一类设备最大组数(仅为了缩短名称长度)
#define EQCOUNT (MAX_EQUIP_COUNT_NUM) //一组设备最大个数(仅为了缩短名称长度)

//点表各类信号最大数量(一维数组) Σ(终端+配变+电容器+漏保)(组最大数量*组内最大个数*本类型点表信号最大个数)
#define MAX_YX_NUM_ZHJ     (1*1* 20 + 1*1* 60 + EQGROUP*EQCOUNT*20 + EQGROUP*EQCOUNT*20)  //终端遥信数量
#define MAX_YC_NUM_ZHJ     (1*1* 60 + 1*1*800 + EQGROUP*EQCOUNT*60 + EQGROUP*EQCOUNT*80)  //遥测数据数量
#define FAULT_MAX_NUM_ZHJ  (1*1* 10 + 1*1* 80 + EQGROUP*EQCOUNT*10 + EQGROUP*EQCOUNT*10)  //故障信息最大个数
#define IDEN_MAX_NUM_ZHJ   (1*1*  1 + 1*1*  1 + EQGROUP*EQCOUNT* 1 + EQGROUP*EQCOUNT* 1)  //定值区特征标识最大个数
#define PARA_MAX_NUM_ZHJ   (1*1*200 + 1*1*100 + EQGROUP*EQCOUNT*50 + EQGROUP*EQCOUNT*50)  //参数配置最大个数

//点表上报任务最大数量(一维数组) Σ(只有终端)(组最大数量*组内最大个数*单设备最大上报任务数量)
#define TASK_MAX_NUM_ZHJ   (1*1*MAX_REPORT_TASK_NUM)  //参数配置最大个数

//采集设备档案最大数量(一维数组) Σ(电容器+漏保)(组最大数量*组内最大个数*单设备最大上报任务数量)
#define FILE_MAX_NUM_ZHJ   (EQGROUP*EQCOUNT*1 + EQGROUP*EQCOUNT*1)  //每个设备默认只有一个档案

//各类信号特殊处理方法
//遥信
#define YX_SP_START              (0x10000000)           //遥信特殊处理开始
#define YX_SP_END                (0x1FFFFFFF)           //遥信特殊处理开始

//电量
#define EN_SP_START              (0x20000000)           //电量特殊处理开始
#define EN_SP_END                (0x2FFFFFFF)           //电量特殊处理开始

//遥测
#define YC_SP_START              (0x30000000)           //遥测特殊处理开始

#define YC_SP_PRE_DAY_START          (0x30000001)            //------日统计开始------
#define YC_SP_PRE_DAY_MAX            (0x30000002)            //上日的最大值、峰值
#define YC_SP_PRE_DAY_MIN            (0x30000003)            //上日的最小值、谷值
#define YC_SP_PRE_DAY_MT_TIME        (0x30000004)            //监测时间
#define YC_SP_PRE_DAY_QR_TIME        (0x30000005)            //合格时间
#define YC_SP_PRE_DAY_UP_TIME        (0x30000006)            //超上限时间
#define YC_SP_PRE_DAY_DW_TIME        (0x30000007)            //超下限时间
#define YC_SP_PRE_DAY_QT_RATE        (0x30000008)            //合格率
#define YC_SP_PRE_DAY_UP_RATE        (0x30000009)            //超上限率
#define YC_SP_PRE_DAY_DW_RATE        (0x3000000A)            //超下限率
#define YC_SP_PRE_DAY_MT_TOTAL       (0x3000000B)            //累加和
#define YC_SP_PRE_DAY_MT_AVG         (0x3000000C)            //平均值
#define YC_SP_PRE_DAY_PF_CAL         (0x3000000D)            //三段功率因数统计时间
#define YC_SP_PRE_DAY_DEMAND         (0x3000000E)            //需量统计
#define YC_SP_PRE_DAY_END            (0x3000FFFF)            //------日统计结束------

#define YC_SP_PRE_MON_START          (0x30010001)            //------月统计开始------
#define YC_SP_PRE_MON_MAX            (0x30010002)            //上月的最大值、峰值
#define YC_SP_PRE_MON_MIN            (0x30010003)            //上月的最小值、谷值
#define YC_SP_PRE_MON_MT_TIME        (0x30010004)            //监测时间
#define YC_SP_PRE_MON_QR_TIME        (0x30010005)            //合格时间
#define YC_SP_PRE_MON_UP_TIME        (0x30010006)            //超上限时间
#define YC_SP_PRE_MON_DW_TIME        (0x30010007)            //超下限时间
#define YC_SP_PRE_MON_QT_RATE        (0x30010008)            //合格率
#define YC_SP_PRE_MON_UP_RATE        (0x30010009)            //超上限率
#define YC_SP_PRE_MON_DW_RATE        (0x3001000A)            //超下限率
#define YC_SP_PRE_MON_MT_TOTAL       (0x3001000B)            //累加和
#define YC_SP_PRE_MON_MT_AVG         (0x3001000C)            //平均值
#define YC_SP_PRE_MON_PF_CAL         (0x3001000D)            //三段功率因数统计时间
#define YC_SP_PRE_MON_DEMAND         (0x3001000E)            //需量统计
#define YC_SP_PRE_MON_END            (0x3001FFFF)            //------月统计结束------

#define YC_SP_RT_START              (0x30020001)            //------实时统计开始------
#define YC_SP_RT_TMN_WORK_TIME      (0x30020002)            //终端累计运行时间
#define YC_SP_RT_VOLT_SPLIT         (0x30020003)            //电压正序、负序、零序分量
#define YC_SP_RT_CURR_SPLIT         (0x30020004)            //电流正序、负序、零序分量
#define YC_SP_RT_VOLT_DIFF          (0x30020005)            //ABC三项电压偏差
#define YC_SP_RT_FR_DIFF            (0x30020006)            //总及ABC三项频率偏差
#define YC_SP_RT_VOLT_IMBALA        (0x30020007)            //电压不平衡度
#define YC_SP_RT_CURR_IMBALA        (0x30020008)            //电流不平衡度
#define YC_SP_RT_LOAD_RATE          (0x30020009)            //负载率
#define YC_SP_RT_U_HMONIC_ODD       (0x3002000A)            //ABC三项谐波电压奇次谐波含有率
#define YC_SP_RT_U_HMONIC_EVEN      (0x3002000B)            //ABC三项谐波电压偶次谐波含有率
#define YC_SP_RT_I_ANGLE            (0x3002000C)            //ABC三项电流相角
#define YC_SP_RT_END                (0x3002FFFF)            //------实时统计结束------

#define YC_SP_HARMONIC_START        (0x30030001)            //------谐波相关开始------
#define YC_SP_HARMONIC_RT_GET       (0x30030002)            //谐波实时获取
#define YC_SP_HARMONIC_END          (0x3003FFFF)            //------谐波相关结束------

#define YC_SP_DEMAND_START          (0x30040001)            //------需量相关开始------
#define YC_SP_DEMAND_RT_GET         (0x30040002)            //需量实时获取
#define YC_SP_DEMAND_END            (0x3004FFFF)            //------需量相关结束------

#define YC_SP_ON_DAY_START          (0x30050001)            //------上日统计开始------
#define YC_SP_ON_DAY_MAX            (0x30050002)            //上日的最大值、峰值
#define YC_SP_ON_DAY_MIN            (0x30050003)            //上日的最小值、谷值
#define YC_SP_ON_DAY_MT_TIME        (0x30050004)            //监测时间
#define YC_SP_ON_DAY_QR_TIME        (0x30050005)            //合格时间
#define YC_SP_ON_DAY_UP_TIME        (0x30050006)            //超上限时间
#define YC_SP_ON_DAY_DW_TIME        (0x30050007)            //超下限时间
#define YC_SP_ON_DAY_QT_RATE        (0x30050008)            //合格率
#define YC_SP_ON_DAY_UP_RATE        (0x30050009)            //超上限率
#define YC_SP_ON_DAY_DW_RATE        (0x3005000A)            //超下限率
#define YC_SP_ON_DAY_MT_TOTAL       (0x3005000B)            //累加和
#define YC_SP_ON_DAY_MT_AVG         (0x3005000C)            //平均值
#define YC_SP_ON_DAY_PF_CAL         (0x3005000D)            //三段功率因数统计时间
#define YC_SP_ON_DAY_DEMAND         (0x3005000E)            //需量统计
#define YC_SP_ON_DAY_END            (0x3005FFFF)            //------上日统计结束------

#define YC_SP_ON_MON_START          (0x30060001)            //------上月统计开始------
#define YC_SP_ON_MON_MAX            (0x30060002)            //上月的最大值、峰值
#define YC_SP_ON_MON_MIN            (0x30060003)            //上月的最小值、谷值
#define YC_SP_ON_MON_MT_TIME        (0x30060004)            //监测时间
#define YC_SP_ON_MON_QR_TIME        (0x30060005)            //合格时间
#define YC_SP_ON_MON_UP_TIME        (0x30060006)            //超上限时间
#define YC_SP_ON_MON_DW_TIME        (0x30060007)            //超下限时间
#define YC_SP_ON_MON_QT_RATE        (0x30060008)            //合格率
#define YC_SP_ON_MON_UP_RATE        (0x30060009)            //超上限率
#define YC_SP_ON_MON_DW_RATE        (0x3006000A)            //超下限率
#define YC_SP_ON_MON_MT_TOTAL       (0x3006000B)            //累加和
#define YC_SP_ON_MON_MT_AVG         (0x3006000C)            //平均值
#define YC_SP_ON_MON_PF_CAL         (0x3006000D)            //三段功率因数统计时间
#define YC_SP_ON_MON_DEMAND         (0x3006000E)            //需量统计
#define YC_SP_ON_MON_END            (0x3006FFFF)            //------上月统计结束------


#define YC_SP_END                   (0x3FFFFFFF)           //遥测特殊处理结束
#define YC_SP_INVALID               (0x40000000)           //忽略数据


typedef enum
{   
    E_EQUIP_FIRST = 1,
    E_EQUIP_POWERTERMINAL = 1,       //终端 1
    E_EQUIP_TRANSFORMER,         //配电变压器 2
    E_EQUIP_SVG,                 //SVG 3
    E_EQUIP_CAPACITOR,           //智能电容器 4
    E_EQUIP_COMMUTATIONSWITCH,   //换相开关 5
    E_EQUIP_RESIDENTMETER,       //用户表 6
    E_EQUIP_CHARGINGPILE,        //充电桩 7
    E_EQUIP_PHOTOVOLTAIC,        //光伏 8
    E_EQUIP_RESIDUALCURRENT,     //剩余电流动作保护器 9
    E_EQUIP_INTELLIGENTSWITCH,   //智能开关 10
    E_EQUIP_LOWVOLTSHUNT,        //智能低压分路监测设备 11
    E_EQUIP_LOWVOLTFAULT,        //智能低压故障传感器 12
    E_EQUIP_MEACTRLMETER,        //多功能测控表 13
    E_EQUIP_STATIONMONITOR,      //站所环境监测终端 14
    E_EQUIP_LAST = E_EQUIP_STATIONMONITOR,
}PW_EQUIP_TYPE_E;

//遥信配置信息结构体
typedef struct
{
    uint16 nEqType;  //设备类型 从1开始
    uint16 nEqGroup;  //设备组别 从0开始
    uint16 nEquipNo;  //设备组内序号 从1开始
    uint8  nObjAddrHB;    //信息对象地址（最高字节）
	uint16 nObjAddr;      //信息对象地址（加上起始地址的实际值）
	uint32 nObjAddrU32;   //信息对象地址（完整3个字节）
	char   *szObjName;    //信息对象名称
	uint32 nSignalType;   //类型标识
	uint32 nDbCenterOAD;  //数据中心查询标签（OAD）
	uint32 nOadColsNum;  //关联次OAD数量
	uint32 aOadCols[MAX_OAD_COLS_NUM];  //关联次OAD
	uint8  nItemIndex;    //分项序号（从0开始）
	uint32 nDealType;  //特殊处理类型(非直接从数据中心获取, 人工处理)
}YX_DATA_CONFIG_ZHJ;

//遥测配置信息结构体
typedef struct
{
    uint16 nEqType;  //设备类型 从1开始
    uint16 nEqGroup;  //设备组别 从0开始
    uint16 nEquipNo;  //设备组内序号 从1开始
    uint8  nObjAddrHB;    //信息对象地址（最高字节）
	uint16 nObjAddr;      //信息对象地址（加上起始地址的实际值）
	uint32 nObjAddrU32;   //信息对象地址（完整3个字节）
	char   *szObjName;    //信息对象名称
	uint32 nSignalType;   //类型标识
	int8 nReportRatio;    //当前数据上送主站的倍率
	uint16 nDeadZoneAddr; //变化死区值的对象地址
	float32 fDeadZoneValue; //变化死区值
	uint8 nLogicId; //逻辑地址，交采：1，电表：0
    uint16 nInfoNum; //信息点号  ，交采：0，电表：从1开始，当信息点号为0xFFFF时，表示全部信息点
	uint32 nDbCenterOAD;  //数据中心查询标签（主OAD）
	uint32 nOadColsNum;  //关联次OAD数量
	uint32 aOadCols[MAX_OAD_COLS_NUM];  //关联次OAD
	uint8  nItemIndex;    //分项序号（从0开始）
	float32 fFullValue;  //满码值（用于归一化遥测值）
	uint32 nDealType;  //特殊处理类型(非直接从数据中心获取, 人工处理)
	uint8 nArrLen;       //不需处理的数组参数长度(比特串)
}YC_DATA_CONFIG_ZHJ;

//故障信息上报参数数据结构，上报按带时标的单点遥信上报
typedef struct 
{
    uint16 nEqType;  //设备类型 从1开始
    uint16 nEqGroup;  //设备组别 从0开始
    uint16 nEquipNo;  //设备组内序号 从1开始
    uint8  nObjAddrHB;    //信息对象地址（最高字节）
	uint16 nObjAddr;      //信息对象地址（加上起始地址的实际值）
	uint32 nObjAddrU32;   //信息对象地址（完整3个字节）
	char   *szObjName;    //信息对象名称
	uint8  nEuipType;     //设备类型（用以确定遥信的所属）
	uint32 nSignalType;   //类型标识（故障信息）
	uint32 nFaultYxAddr;  //故障遥信地址(复用事件的判断逻辑)
	uint8  nRptDataSrc;    //上报数据来源（来自哪个设备）
	uint8  nReportNum;      //上报数据个数(总个数)
	uint8  nReportYxNum;      //上报数据个数(遥信个数)
	uint8  nReportYcNum;      //上报数据个数(不带时标 遥测个数)
	uint8  nReportYcTiNum;      //上报数据个数(带时标 遥测个数)
	uint8  nReportEnNum;      //上报数据个数(电能量个数)
	uint8  nReportStrNum;      //上报数据个数(比特串个数)
	uint8  nReportParaNum;      //上报数据个数(参数个数 仅用于参数变更事件)
	uint16 aReportAddr[MAX_FAULT_SIGNAL_NUM]; //上报信息体地址(取点表信息体地址的存放位置)
	uint32 aReportAddrU32[MAX_FAULT_SIGNAL_NUM]; //上报信息体地址(取点表信息体地址的存放位置)
	uint32 nDbCenterOAD;  //数据中心查询标签（OAD）
	uint8  nItemIndex;    //分项序号（从0开始）
}FAULT_REPORT_CONFIG_ZHJ;

//定值区特征标识结构体
typedef struct
{
    uint16 nEqType;  //设备类型 从1开始
    uint16 nEqGroup;  //设备组别 从0开始
    uint16 nEquipNo;  //设备组内序号 从1开始
    uint16 nFixedAreaID;  //定值区号SN
	PARA_IDENTIFY_UN unParaIdentify;  //参数特征标识
	uint8  nSetState;  //0x55已预置, 0xaa已固化/撤销
	uint8  aSetPara[255];  //参数设置报文(首字节为标志0x55/0xaa, 次字节为个数, 后面地址+tag+长度+值)
}__attribute__ ((packed)) FIXED_AREA_IDEN_ZHJ;

//定值参数设定结构体（主要用于报文）
typedef struct
{
    uint16 nEqType;  //设备类型 从1开始
    uint16 nEqGroup;  //设备组别 从0开始
    uint16 nEquipNo;  //设备组内序号 从1开始
    BOOL bModifiable;     //可修改的
    uint8  nObjAddrHB;    //信息对象地址（最高字节）
	uint16 nObjAddr;      //信息对象地址
	uint32 nObjAddrU32;   //信息对象地址（完整3个字节）
	uint8  res;           //信息对象地址（高字节保留）
	uint8  nTagType;      //Tag类型
	uint8  nDataLen;      //数据长度
	VALUE_TPYE_UN unValue;//数据value
	BOOL   bValid;        //数据value是否有效
	float  fDefaultValue; //默认值
	int64  nDefaultValueI64; //默认值(整型)
	float  fMinValue;     //最小值
	float  fMaxValue;     //最大值
	uint32 nDealType;     //特殊处理类型(非直接从数据中心获取, 人工处理)
	uint8  nUpdateFlag;   //是否更新已有的私有文件内容
	uint16 nUpdateArea;   //更新的定值区号对应参数
	uint16 nArrLen;       //不需处理的数组参数长度
	uint8  aArrValue[MAX_ARR_VALUE_LEN];     //不需处理的数组参数
}PARA_SET_VALUE_ZHJ;

//上报任务配置结构体
typedef struct
{
    uint16 nEqType;  //设备类型 从1开始
    uint16 nEqGroup;  //设备组别 从0开始
    uint16 nEquipNo;  //设备组内序号 从1开始
    uint8 nTaskNo;  //任务号 从1开始
    uint8 bIsValid;  //任务有效标志 0无效 1有效
    uint8 bIsCorrect;  //任务所有参数配置正确 0存在异常参数 1所有参数正确
    uint8 nReportDataType;  //上报数据类型 1实时 2冻结
    uint8 nReportTaskType;  //上报任务类型 1遥信 2遥测
	uint8 aFrozenBaseTime[7];  //冻结基准时间，CP56Time2a
	time_t tFrozenBaseTime;  //冻结基准时间，秒
	OOP_DATETIME_S_T stFrozenBaseTime;
	//uint32 nFrozenPeriod;  //冻结周期，单位分钟
	uint8 nFrozenPeriodValue;  //冻结周期，数值
	uint8 nFrozenPeriodUnit;  //冻结周期单位，0x02 分; 0x03 小时; 0x04 日; 0x05 月; 
	uint8 aReportBaseTime[7];  //上送基准时间，CP56Time2a
	time_t tReportBaseTime;  //上送基准时间，秒
	OOP_DATETIME_S_T stReportBaseTime;
	//uint32 nReportPeriod;  //上送周期，单位分钟
	uint8 nReportPeriodValue;  //上送周期，数值
	uint8 nReportPeriodUnit;  //上送周期单位，0x02 分; 0x03 小时; 0x04 日; 0x05 月; 
	uint8 nSelectRatio;  //数据抽取倍率
	uint8 nInfoItemNum;  // 信息元素数量（NN 组别可能为7或设备序号可能为255）
	uint8 nRealInfoItemNum;  // 实际信息元素数量（公共地址为具体的某个设备）
	uint16 aPublicAddr[MAX_INFO_ITEM_NUM];  //公共地址
	uint16 aEquipType[MAX_INFO_ITEM_NUM];  //设备类型
	uint16 aEquipGroup[MAX_INFO_ITEM_NUM];  //设备组别
	uint16 aEquipNo[MAX_INFO_ITEM_NUM];  //设备序号
	uint32 aInfoItemAddr[MAX_INFO_ITEM_NUM];  //信息体地址
	uint16 aSignalType[MAX_INFO_ITEM_NUM];  //信息数据标识
}REPORT_TASK_CONFIG_ZHJ;

//上报任务数据结构体
typedef struct
{
    BOOL  bValidFlag[MAX_INFO_ITEM_NUM];  //数据有效性标志
	uint8 aYxFrozenValue[MAX_INFO_ITEM_NUM];  //遥信冻结数据
	float32 aYcFrozenValue[MAX_INFO_ITEM_NUM];  //遥测冻结数据
	uint8 nFrozenArrLen[MAX_INFO_ITEM_NUM];  //冻结比特串长度
    uint8 aFrozenArrValue[MAX_INFO_ITEM_NUM][MAX_YC_STRING_LEN];  //冻结比特串数据
	uint8 aFrozenTime[7];  //冻结时间，CP56Time2a
	time_t tFrozenTime;  //冻结时间，秒
	OOP_DATETIME_S_T stFrozenTime;
    uint8 aReportTime[7];  //上报时间，CP56Time2a
	time_t tReportTime;  //上报时间，秒
	OOP_DATETIME_S_T stReportTime;
}REPORT_TASK_DATA_ZHJ;

//任务召测结构体
typedef struct
{
    uint8 nTaskNo;  //任务编号
    uint8 aStartTime[7];  //数据起始时间，CP56Time2a
	time_t tStartTime;  //数据起始时间，秒
	OOP_DATETIME_S_T stStartTime;
    uint8 aEndTime[7];  //数据结束时间，CP56Time2a
	time_t tEndTime;  //数据结束时间，秒
	OOP_DATETIME_S_T stEndTime;
}TASK_CALL_CONFIG_ZHJ;

//采集设备档案
typedef struct
{
    uint16 nEqType;  //设备类型 从1开始
    uint16 nEqGroup;  //设备组别 从0开始
    uint16 nEquipNo;  //设备组内序号 从1开始
    OOP_METER_T stMeterFile;  //采集设备档案信息
}OOP_METER_FILE_ZHJ;


//变量保存私有文件名
extern char *g_pFixedAreaPathZhj;  //定值区特性私有文件
extern char *g_pParamValuePathZhj;  //定值参数私有文件
extern char *g_pFrozenValuePathZhj;  //历史冻结数据私有文件

extern uint16 g_aGroupCountZhj[MAX_EQUIP_TYPE_NUM];  //表示不同类型设备实际组数
extern uint16 g_aEquipCountZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM];  //表示不同类型设备实际数量

//遥信点表实际配置个数
extern uint16 g_nConfigYxNumZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];
extern  uint32 g_nTotalYxNumZhj;//所有遥信点表数量之和

//遥测点表实际配置个数
extern uint16 g_nConfigYcNumZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];
extern uint32 g_nTotalYcNumZhj;//所有遥测点表数量之和

//参数点表实际配置个数
extern uint16 g_nConfigParaNumZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];
extern uint32 g_nTotalParaNumZhj;//所有参数点表数量之和

//事件点表实际配置个数
extern uint16 g_nConfigFaultNumZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];
extern uint32 g_nTotalFaultNumZhj;//所有故障点表数量之和

//遥信点表配置
extern YX_DATA_CONFIG_ZHJ g_aYxDataConfigZhj[MAX_YX_NUM_ZHJ];    //遥信信号配置区
extern YX_DATA_SEND   g_aYxDataSendZhj[MAX_YX_NUM_ZHJ];  //遥信信号存储区
extern uint32         g_aYxDataIndexZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];  //信号索引

//遥测点表配置
extern YC_DATA_CONFIG_ZHJ g_aYcDataConfigZhj[MAX_YC_NUM_ZHJ];  //遥测数据配置区
extern YC_DATA_SEND   g_aYcDataSendZhj[MAX_YC_NUM_ZHJ];  //遥测数据存储区
extern uint32         g_aYcDataIndexZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];  //信号索引
extern uint32         g_nYcStatVoltDay;  //日电压监测统计起始点
extern uint32         g_nYcStatHarmonyDay;  //日电压谐波统计起始点
extern uint32         g_nYcStatVoltUnbalanceDay;  //日电压不平衡度统计起始点
extern uint32         g_nYcVoltUnbalance;  //电压不平衡度实时值起始点
extern uint32         g_nYcStatCurrUnbalanceDay;  //日电流不平衡度统计起始点
extern uint32         g_nYcStatFrequencyDay;  //日频率统计起始点
extern uint32         g_nYcStatExtremeDay; //极值统计起始点
extern uint32         g_nYcStatPwrFactDay; //功率因数统计起始点


//事件点表配置
extern FAULT_REPORT_CONFIG_ZHJ g_aFaultDataConfigZhj[FAULT_MAX_NUM_ZHJ];  //故障信息配置区
extern uint32         g_aFaultDataIndexZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];  //信号索引

//定值区号与变化标志
extern uint16 g_nFixedAreaIdZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];    //定值区号
extern uint8 g_bFixedChangeFlagZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_FIXED_AREA_NUM];    //定值参数改变标志，需要同步至数据中心

//定值区特征标识结构体参数配置
extern uint16 g_aFixedAreaNumZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//定值区数量
extern FIXED_AREA_IDEN_ZHJ g_aFixedValueAreaZhj[MAX_AREA_NUM_ZHJ][IDEN_MAX_NUM_ZHJ];//定值区特征标识
extern uint32 g_aFixedAreaIndexZhj[MAX_AREA_NUM_ZHJ][MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//信号索引

//定值参数配置
extern PARA_SET_VALUE_ZHJ g_aParaValueInitZhj[PARA_MAX_NUM_ZHJ];//点表参数
extern uint32             g_aParaValueIndexZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//信号索引
extern PARA_SET_VALUE_ZHJ g_aParaSetValueZhj[MAX_AREA_NUM_ZHJ][PARA_MAX_NUM_ZHJ];//私有文件保存参数

//写定值参数时解析的公共地址
extern uint16 g_nFixedValuePubAddrZhj;   //写定值参数的公共地址	
extern uint8 g_aFixedValueStateZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];

//参数固化标志
extern uint8  g_nParaFixedFlagZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_FIXED_AREA_NUM];//参数固化标志

//终端上报任务配置
extern REPORT_TASK_CONFIG_ZHJ g_aReportTaskConfig[TASK_MAX_NUM_ZHJ];//配置
extern REPORT_TASK_DATA_ZHJ   g_aReportTaskData[TASK_MAX_NUM_ZHJ];//单周期或单实时冻结数据
extern uint32                 g_aReportTaskIndex[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//信号索引

//终端上报任务的公共地址(统计共有几种不同的公共地址)
extern uint16 g_aReportTaskPubAddrNum[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];
extern uint16 g_aReportTaskPubAddr[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM][MAX_INFO_ITEM_NUM];
extern uint8 g_aReportTaskPubAddrState[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM][MAX_INFO_ITEM_NUM];
    
//冻结数据存储(历史数据)
extern uint16 g_aHisFrozenDataNum[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//数量

//设备档案集合
extern OOP_METER_FILE_ZHJ g_aAllEquipFiles[FILE_MAX_NUM_ZHJ];
extern uint32             g_aEquipFilesIndex[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//信号索引

//普通采集方案记录列选择汇总
extern uint16 g_aSchemeColOADNum[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM];//关联OAD的具体个数
extern uint32 g_aSchemeColOAD[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_YC_NUM];//最多情况为每个遥测项对应一个oad

//总召唤附带的公共地址(设备类型+设备序号)	
extern uint16 g_nAllCallPubAddrZhj;   //总召唤附带的公共地址(设备类型+设备序号)	

//总召唤设备类型与设备组号、设备编号
extern uint8 g_aZeroNull[MAX_EQUIP_COUNT_NUM];
extern uint8 g_aAllCallEquipState[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];
extern uint8 g_aAllCallEquipStateYX[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥信
extern uint8 g_aAllCallEquipStateYC_R[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥测(浮点型)
extern uint8 g_aAllCallEquipStateYC_R_T[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥测(浮点型)
extern uint8 g_aAllCallEquipStateYC_STR[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥测(字符串)

//电量召唤附带的公共地址(设备类型+设备组号+设备序号)	
extern uint16 g_nEnergyCallPubAddrZhj;   //总召唤附带的公共地址(设备类型+设备组号+设备序号)	
extern uint8 g_aEnergyCallEquipState[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];


//点召唤附带的公共地址(设备类型+设备组号+设备序号)	
extern uint16 g_nPointCallPubAddrZhj;   //点召唤附带的公共地址(设备类型+设备组号+设备序号)
extern uint8 g_nPointCallInfoAddrNum;//点召唤地址数量
extern uint32 g_aPointCallInfoAddr[MAX_POINT_CALL_NUM];//点召唤地址备份
extern uint8 g_aPointCallEquipState[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];
extern uint8 g_aPointCallEquipStateYX[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥信
extern uint8 g_aPointCallEquipStateYC_R[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥测(浮点型)
extern uint8 g_aPointCallEquipStateYC_R_T[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥测(浮点型)
extern uint8 g_aPointCallEquipStateYC_R_EN[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥测(浮点型)
extern uint8 g_aPointCallEquipStateYC_STR[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥测(字符串)

//任务召唤设备类型与设备编号
extern uint16 g_nTaskCallPubAddrZhj;   //任务召唤附带的公共地址(设备类型+设备组号+设备序号)
extern TASK_CALL_CONFIG_ZHJ g_stTaskCallConfig;//任务召唤的起止时间
extern uint8 g_aTaskCallEquipState[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];
extern uint8 g_aTaskCallEquipStateYX[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//未使用
extern uint8 g_aTaskCallEquipStateYC_R[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//未使用
extern uint8 g_aTaskCallEquipStateYC_STR[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//未使用
extern uint8 g_aTaskCallPubAddrState[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM][MAX_INFO_ITEM_NUM];

//数据变化主动上报设备类型与设备编号
extern uint8 g_aChangedEquipStateYX[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥信
extern uint8 g_aChangedEquipStateYC_R[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥测(浮点型)
extern uint8 g_aChangedEquipStateYC_R_T[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥测(浮点型)
extern uint8 g_aChangedEquipStateYC_R_EN[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥测(浮点型)
extern uint8 g_aChangedEquipStateYC_STR[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//遥测(字符串)

//任务上报设备类型与设备编号
extern uint8 g_aReportTaskState[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];
extern uint8 g_aReportTaskStateYX[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//未使用
extern uint8 g_aReportTaskStateYC_R[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//未使用
extern uint8 g_aReportTaskStateYC_STR[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//未使用

//所有参数召唤附带的公共地址(设备类型+设备组号+设备序号)	
extern uint16 g_nAllParaCallPubAddrZhj;   //所有参数召唤附带的公共地址(设备类型+设备组号+设备序号)	
extern uint16 g_nAllParaCallAreaIdZhj;   //所有参数召唤定值区号
extern uint8 g_aAllParaEquipState[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];

//部分参数召唤附带的公共地址(设备类型+设备组号+设备序号)	
extern uint16 g_nSomeParaCallPubAddrZhj;   //部分参数召唤附带的公共地址(设备类型+设备组号+设备序号)
extern uint16 g_nSomeParaCallAreaIdZhj;   //部分参数召唤定值区号
extern uint8 g_nSomeParaCallInfoAddrNum;//部分参数召唤地址数量
extern uint32 g_aSomeParaCallInfoAddr[MAX_PARA_CALL_NUM];//部分参数召唤地址备份
extern uint8 g_aSomeParaCallEquipState[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];

//时钟同步公共地址(设备类型+设备组号+设备序号)	
extern uint16 g_nTimeSynPubAddrZhj;   //时钟同步附带的公共地址(设备类型+设备组号+设备序号)

//104任务执行流程全局变量(周期冻结与上报专用)
extern TASK_RUN_STEP_T g_sTimingTaskRunStep;

//函数
extern int get_next_equip_attr_zhj(uint16 curTyIndex, uint16 curGrIndex, uint16 curSeqIndex, uint16 *nexTyIndex, uint16 *nexGrIndex, uint16 *nexSeqIndex);
extern int scheme_coload_init_zhj();
extern int query_yx_type_config_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint16 eqnoIndex, uint32 InfoAddr);
extern int query_yx_index_config_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint16 eqnoIndex, uint32 InfoAddr);
extern int query_yc_type_config_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint16 eqnoIndex, uint32 InfoAddr);
extern int query_yc_index_config_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint16 eqnoIndex, uint32 InfoAddr);
extern int query_para_index_config_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint16 eqnoIndex, uint16 areaIndex, uint32 InfoAddr);
extern int timing_task_create_zhj();
extern int scheme_coload_statistics_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint32 oad);
extern int scheme_coload_save_zhj();
extern int equip_files_create_zhj(uint16 equipType, uint16 equipGroup, uint16 equipNo, OOP_METER_T *newOopMeter);
extern int meter_data_clear_zhj(DB_CLIENT fd, uint16 infoNum);
extern int equip_files_save_zhj();
extern int clear_all_scheme_zhj(DB_CLIENT fd);;
extern int equip_scheme_save_zhj();
extern int clear_all_task_zhj(DB_CLIENT fd);
extern int equip_task_save_zhj();
extern int i_frame_data_update_zhj(TOTAL_104_FRAME_T stTotal104Frame, uint8 *pOutBuf);
extern int i_frame_data_init_end_zhj(uint8 *pOutBuf);
extern int i_frame_data_pack_zhj(uint8 nTI, uint8 nVSQ, uint16 nCOT, uint16 nPubAddr, uint8 *pInObjBuff, uint8 nInObjLen, uint8 *pOutBuf);
extern int check_call_all_addr_zhj(uint16 PubAddr);
extern int check_call_point_addr_zhj(uint16 PubAddr);
extern int check_call_point_info_zhj(uint8* pInfo, uint8 nInfoLen);
extern int check_call_task_addr_zhj(uint16 PubAddr, uint8* pInfo, uint8 nInfoLen);
extern int move_to_next_call_task_zhj(uint8 nType);
extern int check_para_oper_addr_zhj(uint16 PubAddr);
extern int para_data_update_zhj(uint16 nEqTypeIndex, uint16 nEqGroupIndex, uint16 nEqIdIndex, uint16 nFixArea, PARA_SET_VALUE_T sParaSetValue);
extern uint8 para_set_operate_zhj(uint8 infonum, uint8 *pdata, uint8 infolen, uint8 btype);
extern int i_frame_data_unpack_zhj(uint16 nFrameLen, uint8 *pFrameBuff, uint8 *pOutBuf, uint8 *pAppType);
extern int report_task_create_zhj();
extern int get_next_equip_yx_zhj();
extern int get_next_equip_en_zhj();
extern int get_next_equip_all_para_zhj();
extern int get_next_equip_some_para_zhj();
extern int get_next_equip_yc_zhj(uint8 yctype);
extern int get_next_point_equip_yx_zhj();
extern int get_next_point_equip_yc_zhj(uint8 yctype);
extern uint8 yx_total_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 bStartNum,uint8 *pGetNum);
extern uint8 yc_total_nva_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_total_sva_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_total_r_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_total_r_t_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_total_str_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int yc_total_get_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern uint8 en_total_r_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int en_total_get_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern uint8 yx_point_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 bStartNum,uint8 *pGetNum);
extern uint8 yc_point_nva_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_point_sva_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_point_r_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_point_r_t_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_point_r_en_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_point_str_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int yc_point_get_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern int changed_yx_left_zhj();
extern int changed_yc_left_zhj(uint8 yctype);
extern int next_changed_yx_eq_zhj();
extern int next_changed_yc_eq_zhj(uint8 yctype);
extern uint8 yx_change_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_nva_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_sva_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_r_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_r_t_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_r_en_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_str_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int move_next_task_call_pub_zhj();
extern int yc_change_get_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern int yx_frozen_call_get_zhj(uint8 bAdulen, uint8 *pData, uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_r_call_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_r_t_call_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_en_call_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_str_call_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_call_get_zhj(uint8 bAdulen, uint8 *pData, uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_refresh_frozen_call_zhj(uint8 *pData, uint8 *pCurrLen);
extern int next_call_all_equip_zhj(uint8 SigType);
extern int next_call_point_equip_zhj(uint8 SigType);
extern int report_task_run_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern int query_yx_by_detail_zhj(uint16 nPubAddr, uint32 nInfoAddr, uint8 *nInfoValue, BOOL *bValidFlag);
extern int query_yc_r_by_detail_zhj(uint16 nPubAddr, uint32 nInfoAddr, float32 *fInfoValue, BOOL *bValidFlag);
extern int query_yc_str_by_detail_zhj(uint16 nPubAddr, uint32 nInfoAddr, uint8 *nArrLen, uint8 *aArr, BOOL *bValidFlag);
extern int check_info_addr_type_zhj(uint32 nInfoAddr);
extern int query_yc_ratio_config_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint16 eqnoIndex, uint32 InfoAddr, int *ratio);
extern int check_frozen_time_zhj(uint16 nGroupIndex, uint16 nNoIndex, uint16 nTaskIndex, OOP_DATETIME_S_T stCurrentTime, time_t tCurrentTime);
extern int check_report_time_zhj(uint16 nGroupIndex, uint16 nNoIndex, uint16 nTaskIndex, OOP_DATETIME_S_T stCurrentTime, time_t tCurrentTime);
extern int save_frozen_data_zhj(uint16 nGroupIndex, uint16 nNoIndex, uint16 nTaskIndex, REPORT_TASK_DATA_ZHJ stFrozenData);
extern void timing_active_send_zhj();
extern int move_to_next_task_zhj(uint8 nType);
extern uint8 yx_timing_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_timing_r_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_timing_r_t_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_timing_en_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_timing_str_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int yc_timing_get_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern int yx_frozen_get_zhj(uint8 bAdulen, uint8 *pData, uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_r_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_r_t_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_en_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_str_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_get_zhj(uint8 bAdulen, uint8 *pData, uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_refresh_frozen_zhj(uint8 *pData, uint8 *pCurrLen);
extern int timing_active_run_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);



#ifdef __cplusplus
}
#endif

#endif // __PW_PROTOCOL_ZHJ_H__
