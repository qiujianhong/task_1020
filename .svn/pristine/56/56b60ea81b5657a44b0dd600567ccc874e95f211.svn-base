/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#ifndef __AC_METER_SAMPLE_H__
#define __AC_METER_SAMPLE_H__

#include "ac_redef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif


//交采模块编译开关------<测试修改>------
#define AC_DEV_HAL  0   //交采模块通信接口是否HAL

//交采模块升级优化
#define AC_HIGH_UPDATE_MOD  1
#define AC_UPDATE_NUM       4

/*******************************************************************************
* 全局变量
*******************************************************************************/
extern int32                    fd_ac;                   // 句柄
extern uint8                    gCallVerAfterBaudRateChanged;  //切换波特率之后，重新召唤版本信息标志

extern COMMON_DATA_TYPE         gCommonData;             // 公共交互变量
extern PRIVTE_MEASURE_TYPE      gPriMeasurePara;         // 私有计量参数
extern PRIVATE_DATA_TYPE        gPrivateData;            // 私有数据变量
extern MEA_REAL_DATA_COM_TYPE   gMeaRealTimData;         // 计量实时数据


// 交采与统计交互的参数的声明
extern TERM_SHARE_DATA_TYPE    TermSharedData;           // 交采共享内存数据的声明
extern SHARED_MEM_MARK_BYTE    SharedMemMark;            // 刷新共享数据标记

//历史数据统计变量
extern TERM_SHARE_DATA_TYPE    MidTermSharedData;          // (中间变量)交采共享内存数据的声明
extern PRIVTE_MEASURE_TYPE     gMidPriMeasurePara;         // (中间变量)私有计量参数
extern TERM_SHARE_DATA_TYPE    HisTermSharedData;          // (历史)交采共享内存数据的声明
extern PRIVTE_MEASURE_TYPE     gHisPriMeasurePara;         // (历史)私有计量参数

//历史数据统计变量
extern UPDATE_FLAG_T           gUpdateFlag;          //采集数据是否越过变化死区标志

//交采模块对时标志
extern uint8 g_nAcSetDateFlag;  //交采模块对时标志(日期)
extern uint8 g_nAcSetTimeFlag;  //交采模块对时标志(时间)
extern uint8 g_nAcReadDemandPeriodFlag;  //交采模块查询最大需量周期标志
extern uint8 g_nAcReadSlipTimeFlag;  //交采模块查询滑差时间标志

extern uint8 g_nAcReadZoneNumFlag;  //交采模块当前套/备用套时区标志 数量
extern uint8 g_nAcReadDayTableNumFlag;  //交采模块当前套/备用套日时段表标志 数量
extern uint8 g_nAcReadDayPeriNumFlag;  //交采模块当前套/备用套日时段标志 数量
extern uint8 g_nAcReadPeriNumFlagArr[OOP_MAX_DAYPERIODS];//当前套/备用套日时段表(设置交采模块)标志 数量

extern uint8 g_nAcReadCurrZoneFlag;  //交采模块当前套时区标志
extern uint8 g_nAcReadAddiZoneFlag;  //交采模块备用套时区标志
extern uint8 g_nAcReadCurrDayPeriFlag;  //交采模块当前套日时段标志
extern uint8 g_nAcReadAddiDayPeriFlag;  //交采模块备用套日时段标志
extern uint8 g_nAcReadCurrDayFlagArr[OOP_MAX_DAYPERIODS];//当前套日时段表(设置交采模块)标志
extern uint8 g_nAcReadAddiDayFlagArr[OOP_MAX_DAYPERIODS];//备用套日时段表(设置交采模块)标志

extern uint8 g_nRefreshCurrDayFlagArr[OOP_MAX_DAYPERIODS];//当前套日时段表 刷新标志
extern uint8 g_nRefreshAddiDayFlagArr[OOP_MAX_DAYPERIODS];//备用套日时段表 刷新标志

extern uint8 g_nReadCurrDayPeriLabelFlag;   //交采模块目前运行的日时段表是第几套 查询标志
extern uint8 g_nRefreshCurrDayPeriLabelFlag;//交采模块目前运行的日时段表是第几套 已刷新标志
extern uint8 g_nAcParamInitFlag;            //交采参数初始化标志
extern uint8 g_nPowerOffFlag;               //掉电标志

/*******************************************************************************
* 电表类型配置
*******************************************************************************/
#define ACQ_ADDR_INDEX              1           //采集器地址索引位置
#define CTRL_INDEX                  8           //控制码索引位置
#define LEN_INDEX                   9           //数据域长度开始位置
#define MTR_DATA_INDEX              10          //电表格式数据域索引位置

/*******************************************************************************
* 采样任务类型
*******************************************************************************/
#define SAMPLE_TYPE_NULL      0  //无，枚举类型与698保持一致
#define SAMPLE_TYPE_ENERGY    2  //电量
#define SAMPLE_TYPE_DEMAND    4  //需量
#define SAMPLE_TYPE_REALTIME  5  //实时数据
#define SAMPLE_TYPE_HARMONIC  6  //谐波

/*******************************************************************************
* 升级宏定义
*******************************************************************************/
#define CHECK_RESULT_SUCCEED			0
#define CHECK_RESULT_FAILUER			-1
#define CHECK_RESULT_CONFIRM			1
#define CHECK_RESULT_DENY			    2

#define JC_UPDATE_PACKAGE_LEN           256
#define MAX_UPDATE_DELAY_TIME           6
#define MAX_JC_COMM_DELAY_TIME          6

#define JC_UPDATE_BUF_LOG               0    //交采升级日志

/*******************************************************************************
* 结构体与枚举
*******************************************************************************/

typedef enum
{
    INIT_AC_STATE,
    READ_AC_SEND_STATE,
    READ_AC_RECV_STATE,
    CLOSE_AC_STATE
} READ_AC_STATE;

typedef enum
{
    BAUDRATE9600,
	BAUDRATE115200
} AC_BAUDRATE;

typedef enum
{
    ES_CONBIN_ACT = 0,                          //组合有功
    ES_POS_ACT,                                 //正向有功
    ES_NEG_ACT,                                 //反向有功
    ES_POS_REA,                                 //正向无功
    ES_NEG_REA,                                 //反向无功
    ES_1_REA,                                   //1象限无功
    ES_2_REA,                                   //2象限无功
    ES_3_REA,                                   //3象限无功
    ES_4_REA,                                   //4象限无功
    ES_POS_APP,                               //正向视在电能
    ES_NEG_APP,                               //反向视在电能
    SY_CMB_AE_Z_ER_ACT,                         //(抄表日)组合有功
    SY_ES_POS_ACT,                                 //上月正向有功
    SY_ES_NEG_ACT,                                 //上月反向有功
    SY_ES_POS_REA,                                 //上月正向无功
    SY_ES_NEG_REA,                                 //上月反向无功
    SY_ES_1_REA,                                   //上月1象限无功
    SY_ES_2_REA,                                   //上月2象限无功
    SY_ES_3_REA,                                   //上月3象限无功
    SY_ES_4_REA,                                   //上月4象限无功
    ES_POS_ACT_A,                               //A相正向有功
    ES_NEG_ACT_A,                               //A相反向有功
    ES_POS_REA_A,                               //A相正向无功
    ES_NEG_REA_A,                               //A相反向无功
    ES_POS_ACT_B,                               //B相正向有功
    ES_NEG_ACT_B,                               //B相反向有功
    ES_POS_REA_B,                               //B相正向无功
    ES_NEG_REA_B,                               //B相反向无功
    ES_POS_ACT_C,                               //C相正向有功
    ES_NEG_ACT_C,                               //C相反向有功
    ES_POS_REA_C,                               //C相正向无功
    ES_NEG_REA_C,                               //C相反向无功
    ES_TZ_FREEZE1,                              //时区切换冻结1
    ES_TZ_FREEZE2,                              //时区切换冻结2
    
    ES_1_REA_A,                               //A相第一象限无功电能
    ES_1_REA_B,                               //B相第一象限无功电能
    ES_1_REA_C,                               //C相第一象限无功电能
    ES_2_REA_A,                               //A相第二象限无功电能
    ES_2_REA_B,                               //B相第二象限无功电能
    ES_2_REA_C,                               //C相第二象限无功电能
    ES_3_REA_A,                               //A相第三象限无功电能
    ES_3_REA_B,                               //B相第三象限无功电能
    ES_3_REA_C,                               //C相第三象限无功电能
    ES_4_REA_A,                               //A相第四象限无功电能
    ES_4_REA_B,                               //B相第四象限无功电能
    ES_4_REA_C,                               //C相第四象限无功电能
    
    ES_POS_APP_A,                             //A相正向视在电能
    ES_POS_APP_B,                             //B相正向视在电能
    ES_POS_APP_C,                             //C相正向视在电能
	
	ES_NEG_APP_A,                             //A相反向视在电能
	ES_NEG_APP_B,                             //B相反向视在电能
	ES_NEG_APP_C,                             //C相反向视在电能

    /***高精度电能量 begin***/
    ES_CONBIN_ACT_HD,                              //组合有功
    ES_POS_ACT_HD,                                 //正向有功
    ES_NEG_ACT_HD,                                 //反向有功
    ES_POS_REA_HD,                                 //正向无功
    ES_NEG_REA_HD,                                 //反向无功
    ES_1_REA_HD,                                   //1象限无功
    ES_2_REA_HD,                                   //2象限无功
    ES_3_REA_HD,                                   //3象限无功
    ES_4_REA_HD,                                   //4象限无功
    ES_POS_APP_HD,                               //正向视在电能
    ES_NEG_APP_HD,                               //反向视在电能
    SY_CMB_AE_Z_ER_ACT_HD,                         //(抄表日)组合有功
    SY_ES_POS_ACT_HD,                                 //上月正向有功
    SY_ES_NEG_ACT_HD,                                 //上月反向有功
    SY_ES_POS_REA_HD,                                 //上月正向无功
    SY_ES_NEG_REA_HD,                                 //上月反向无功
    SY_ES_1_REA_HD,                                   //上月1象限无功
    SY_ES_2_REA_HD,                                   //上月2象限无功
    SY_ES_3_REA_HD,                                   //上月3象限无功
    SY_ES_4_REA_HD,                                   //上月4象限无功
    ES_POS_ACT_A_HD,                               //A相正向有功
    ES_NEG_ACT_A_HD,                               //A相反向有功
    ES_POS_REA_A_HD,                               //A相正向无功
    ES_NEG_REA_A_HD,                               //A相反向无功
    ES_POS_ACT_B_HD,                               //B相正向有功
    ES_NEG_ACT_B_HD,                               //B相反向有功
    ES_POS_REA_B_HD,                               //B相正向无功
    ES_NEG_REA_B_HD,                               //B相反向无功
    ES_POS_ACT_C_HD,                               //C相正向有功
    ES_NEG_ACT_C_HD,                               //C相反向有功
    ES_POS_REA_C_HD,                               //C相正向无功
    ES_NEG_REA_C_HD,                               //C相反向无功
    ES_TZ_FREEZE1_HD,                              //时区切换冻结1
    ES_TZ_FREEZE2_HD,                              //时区切换冻结2
    
    ES_1_REA_A_HD,                               //A相第一象限无功电能
    ES_1_REA_B_HD,                               //B相第一象限无功电能
    ES_1_REA_C_HD,                               //C相第一象限无功电能
    ES_2_REA_A_HD,                               //A相第二象限无功电能
    ES_2_REA_B_HD,                               //B相第二象限无功电能
    ES_2_REA_C_HD,                               //C相第二象限无功电能
    ES_3_REA_A_HD,                               //A相第三象限无功电能
    ES_3_REA_B_HD,                               //B相第三象限无功电能
    ES_3_REA_C_HD,                               //C相第三象限无功电能
    ES_4_REA_A_HD,                               //A相第四象限无功电能
    ES_4_REA_B_HD,                               //B相第四象限无功电能
    ES_4_REA_C_HD,                               //C相第四象限无功电能
    
    ES_POS_APP_A_HD,                             //A相正向视在电能
    ES_POS_APP_B_HD,                             //B相正向视在电能
    ES_POS_APP_C_HD,                             //C相正向视在电能
	
	ES_NEG_APP_A_HD,                             //A相反向视在电能
	ES_NEG_APP_B_HD,                             //B相反向视在电能
	ES_NEG_APP_C_HD,                             //C相反向视在电能

    /***高精度电能量 end***/

    /***交采参数 begin***/
    
    PARA_CURR_ZONE,//当前套时区表
    PARA_ADDI_ZONE,//备用套时区表
    PARA_CURR_DAY_PERIOD,//当前套日时段表
    PARA_ADDI_DAY_PERIOD,//备用套日时段表
    PARA_DEMAND_PERIOD,//最大需量周期
    PARA_SLIP_TIME,//滑差时间
    
    /***交采参数 end***/
	
}ENERGY_SORT_T;                                 //能量类别

typedef struct _JC_UPDATE_CFG_T
{
    uint8   iUpdateState;                  //升级状态: 0x00 没有升级任务 0x55 暂停升级 0xAA 正在升级
    uint16  CurJcIndex;                    //当前升级交采索引
    uint16  JcNum;                         //升级交采总数
    uint8   CollecterNum;                  //本次地址文件的采集器总数
    uint32  UpdateSeq;                     //当前升级的帧序列
    uint8   AcqAddr[6];                    //当前升级采集器地址
    uint16  iUpdateSuccCnt;                //升级成功个数
    int     iUpdateCnt;                    //升级轮次计数
    uint8   UpdateFlag[30];                //升级失败标志   0:未配置；1:成功；2:失败
    uint8   ClearFlag;                     //首帧清除标志   0:未发送清除    1:已发送清除
}__attribute__ ((packed)) JC_UPDATE_CFG_T; //采集器升级配置结构体

typedef enum
{
    METER_NULL = 0,
    METER_PLC,                             //PLC电表
    METER_485,                             //485电表
}MTR_TYPE_E;                               //电表类型


typedef struct
{
#if HARDWARE_2012_EN
    uint8      DelayFlag;                   //组帧时的通信延时标志
    uint16     DelayCnt;                    //通信延时时常
#endif
    MTR_TYPE_E MtrType;                     //电表类型(1:plc/2:485)
    uint8      PrtlMode;                    //规约类型
    uint8      AcqUnitAddr[6];              //采集器地址
    uint8      MtrAddr[6];                  //电表地址
    uint8      CtrlCode;                    //控制码
    uint8      DataLen;                     //数据域长度
    uint8      DataBuff[256];               //数据域内容
}METER_PRTL_FMT_T; 

typedef enum
{
    PRTL_NULL = 0,
    PRTL_97,                                    //DLT/645-1997规约类型
    PRTL_07,                                    //DLT/645-2007规约类型
#if 0//WHG_FUN== 1
    PRTL_WHG,                                   //水热气表规约类型
#endif
#if SHANGHAI_FUN == 1
    PRTL_SH97,                                  //上海97规约电表
#endif
#if UPDATE_698_FUNC2 == 1
    PRTL_OOP,									//面向对象规约电表类型
#endif
}PRTL_MODE_T;                                   //规约类型

typedef struct _JC_FRAME_TYPE_T
{
    uint8     type;                          //文件标识
    uint8     kind;                          //文件属性
    uint8     cmd;                           //文件指令
    uint16    seg;                           //总段数
    uint16    offset;                        //第 i 段标识或偏移（ i=0～n）
    uint8     len;                           //第 i 段数据长度
    char      buf[256];	                     //文件数据
} __attribute__ ((packed))JC_FRAME_TYPE_T;

typedef enum
{
    CMD_IDLE = 0,                            //开始初始化
    CMD_SEND,                                //升级发送
    CMD_RECV,                                //升级接收
    CMD_RETRY,                               //发送尝试
    CMD_DELAY,                               //升级后延时
    //CMD_NEWVER,	                         //获得更新后的采集器版本号
    CMD_FAIL,	                             //升级失败
    CMD_SUC,	                             //升级成功
    CMD_VER_SEND,                            //发版本号
    CMD_VER_RECV,                            //收版本号
    CMD_END,                                 //升级结束
}MODULE_JC_UPDATE_DEAL_T;                    //通道板模块升级状态字

typedef enum _JC_UPDATE_STEP_E
{
    JC_UPDATE_IDLE = -1,
    JC_UPDATE_START = 0,
    JC_UPDATE_RUN,
    JC_UPDATE_END,
}JC_UPDATE_STEP_E;

extern FACTORY_VER_TMP_T g_tVer;
extern uint8 gVerBuf[8];

//函数声明
extern void ac_calc_deal(uint32* cntchk);
extern void ac_demand_initial(MSG_INFO_T *ptMsgInfo);
extern void ac_data_initial(MSG_INFO_T *ptMsgInfo);
extern void ac_param_initial(MSG_INFO_T *ptMsgInfo);
extern void ac_system_deal(MSG_INFO_T *ptMsgInfo);
extern BOOL isJCNeedGetVer(void);
extern void ReadACSample(BOOL isUpdateAC, BOOL isGetVer, uint8 type);
extern void UpdateStatistics(uint8 type);
extern void ac_clean_ver(void);
extern int8 UnPackMeterData(uint16 FrameLen, uint8* FrameBuff, uint8* MtrAddr, uint8* CtrlCode, uint8* DataLen, uint8* DataBuff);
extern void Pro_AC_Amr_Response_Single(uint8* pRecvData);
extern int STDAC_CheckRspFrame_Single(uint8* pBuf, uint16 bufLen);
#ifdef __cplusplus
}
#endif

#endif // __AC_METER_SAMPLE_H__