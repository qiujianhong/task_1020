/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#ifndef __AC_METER_REDEF_H__
#define __AC_METER_REDEF_H__

#include "ac_all.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

// 高精度电量采集与存储开关
#define  HD_ENERGY_EN   1

// 调试信息开关宏
#define  DEBUG_ACAPP_EN   1
#define  DEBUG_ACCOMM_EN  1

#ifdef  DEBUG_ACAPP_EN
#include <errno.h>
extern int errno;
#define ACPRINT(x...) {if (getenv("DEBUG_AC") != NULL)  printf(x);}
#else
#define ACPRINT(x...)
#endif


#if DEBUG_ACCOMM_EN == 1
#define ACCOMM(x...) {if (getenv("DEBUG_ACCOMM") != NULL)  printf(x);}
#else
#define ACCOMM(x...)
#endif


#define TEST_VER_FLAG    (1)     //是否临时测试版本


/*******************************************************************************
* 宏定义
*******************************************************************************/

#define MAX_ALLOWED_DOWN_NUM    8               //485模块下行最大允许线程数量
#define RUN_DOWN_NUM            1               //485模块下行实际运行线程数量

#define MAX_QUEUE_CNT		                 40                      //最大队列数
#define MAX_QUEUE_BUFF_LEN                 4224                      //上行、下行缓冲区长度

#define MAX_MODULE_NUM     5       //模组的最大数量
#define MAX_CHANNEL_NUM     5      //每个模组最多有5个通道 0默认管理通道
#define MAX_MODULE_DELAY_TIME  6   //查询模组消息最大超时时间
#define AC485_MODULE_DEVNAME  "uart_acm"

#define AC_NORMAL_DATA    1               //交采普通数据类型
#define AC_PARA_DATA      2               //交采普通参数类型
#define AC_ACTION_DATA    3               //交采操作数据类型
#define AC_RECORD_DATA    4               //交采记录型数据类型


/*******************************************************************************
* 枚举和结构体定义
*******************************************************************************/

//队列
typedef struct
{
    int32  QueueLen;					        //队列长度
    uint8   QueueBuf[MAX_QUEUE_BUFF_LEN];	    //队列存储缓冲区
}Queue_T;

//锁共享缓冲区结构
typedef struct
{
    pthread_mutex_t QueueLock;					//接收缓冲区互斥锁
    int16  QueueHead;						    //队列头
    int16  QueueTail;							//队列尾
    Queue_T Queue[MAX_QUEUE_CNT];	            //通讯数据队列
}LockQueue_T;

//模组信息中的通道信息
typedef struct
{
    uint8 ChannelType;//接口类型	enum{CDC-ACM(1), CDC-ECM(2)},
    uint8 ChannelFunc;//功能配置	enum{管理通道(0), 远程通信(1)，PLC/RF通信(2)，RS-485通信(3)，遥信脉冲(4)，CAN通信(5)，M-Bus通信(6)，控制(7)，模拟量采集(8)，RS232通信(9)}
}CHANNEL_INFO_T;  //模组信息中的通道信息

//模组信息
typedef struct
{
    pthread_rwlock_t   ModuleLock;  //读写模组信息所用读写锁
    int32 ModuleID;//模组序号(从1开始)
    uint8 IsValid;//是否有效 0无效 1有效
    uint8 DeviceType[32];//设备型号
    uint8 DeviceID[32];//设备id
    uint32 SoftVersion;//软件版本
    uint8 SoftDate[10];//软件发布日期
    uint32 HardVersion;//硬件版本
    uint8 HardDate[10];//硬件发布日期
    uint8 ManufacturerCode[8];//厂商编码
    uint8 ChannelNum;//通道个数
    CHANNEL_INFO_T ChannelInfo[MAX_CHANNEL_NUM];//通道配置
}MODULE_INFO_T;//模组信息

//遥信模块HAL接口参数
typedef struct
{
    char  deviceName[30];  /*设备名 */
    uint8 isvalid;
    uint8 moduleIndex;  /*模块编号 从1开始 */
    UART_DEVICE_T* dev;
}DEV_HAL_INFO;



/*******************************************************************************
* 常量声明
*******************************************************************************/
#define MAX_TARIFF_NUM            4          // 最大费率数:最多63个 尖、峰、平、谷
#define PhaMaxTariffNum           0          // 分相电能量费率数 0:无分费率，仅总

#define MAX_QUEUE_BUFF_LEN                 4224                      //上行、下行缓冲区长度
#define MAX_MSG_PAYLOAD_LEN                2040                      //消息MSG的payload最大长度

#define MAX_HARMONIC_NUM          21         // 电压谐波含有率（总及2…n次） n大小

#define MAX_DB_R_W_LEN            1000       // 数据中心单次读写最大长度

/*******************************************************************************
* 显示控制相关参数结构声明
*******************************************************************************/
#define AC_PROOF_LOG  "/mnt/log/acproof.log"
#define AC_APP_LOG    "/mnt/log/acapp.log"


/*******************************************************************************
* 数据结构声明
*******************************************************************************/

typedef enum
{
    ACT_COMM_IDLE = 0,                          //空闲
    ACT_COMM_SEND,                              //发送
    ACT_COMM_RECV,                              //接收
    ACT_COMM_RETRY,                             //重试
    ACT_COMM_DELAY,                             //延时
}ACTIVE_COMM_STATE_T;                           //主动通讯状态字

typedef struct{
    OOP_OAD_U  Oad;          //OAD
    int8   DesScaler;        //0AD对应的换算单位
    uint32  DI07;	         //07协议数据标识
    uint8   DataLen;	     //645 每个子元素的长度
    uint8   TotalDataLen;	 //645 总长度
    uint16  DI97;
    DATA_E  CovertSruct; //要转成的结构体类型
    int8   Scaler;           //645小数点位数
}Map645ToOop_NEW;


/* 考虑字节对齐, 没有严格按照报文格式, 收到的消息解析成如下格式 */
typedef struct tagInfoData
{
    uint32 IP;        //目的端ip
    uint16 port;
    uint8  res[2];
    
    uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128];
    char   DestAddr[128];

    uint16 IID_ID;
    uint16 IOP_ID;

    uint32 nPayloadLength; 
    uint8  aPayloadData[MAX_MSG_PAYLOAD_LEN];
}INFO_DATA_T;


/* 采样数据变化死区 */
#define DEAD_ZONE_ENG            (1)          // 电能 -2
#define DEAD_ZONE_ENG_HD         (1)          // 电能(高精度) -4
#define DEAD_ZONE_DMD            (2)          // 需量 0
#define DEAD_ZONE_VOLT           (2)          // 电压 -1
#define DEAD_ZONE_CURR           (2)          // 电流 -3
#define DEAD_ZONE_ANGLE          (2)          // 相角 -1
#define DEAD_ZONE_POWER          (2)          // 功率 -1
#define DEAD_ZONE_PF             (2)          // 功率因数 -3
#define DEAD_ZONE_FR             (2)          // 频率 -2
#define DEAD_ZONE_RATE           (2)          // 不平衡率
#define DEAD_ZONE_HARM           (1)          // 谐波


/* 采样数据是否越过变化死区标志 */
typedef struct tagUpdateFlag
{
    /* 电能 */
    BOOL bUpdateCmbActEng;             //(当前)组合有功电能块
    
    BOOL bUpdatePosActEng;             //(当前)正向有功电能总尖峰平谷示值
    BOOL bUpdatePhaAPosActEng;             //(A相)正向有功总电能示值
    BOOL bUpdatePhaBPosActEng;             //(B相)正向有功总电能示值
    BOOL bUpdatePhaCPosActEng;             //(C相)正向有功总电能示值
    
    BOOL bUpdateNegActEng;             //(当前)反向有功电能总尖峰平谷示值
    BOOL bUpdatePhaANegActEng;             //(A相)反向有功总电能示值
    BOOL bUpdatePhaBNegActEng;             //(B相)反向有功总电能示值
    BOOL bUpdatePhaCNegActEng;             //(C相)反向有功总电能示值
    
    BOOL bUpdatePosReaEng;             //(当前)正向无功电能总尖峰平谷示值
    BOOL bUpdatePhaAPosReaEng;             //(A相)正向无功总电能示值
    BOOL bUpdatePhaBPosReaEng;             //(B相)正向无功总电能示值
    BOOL bUpdatePhaCPosReaEng;             //(C相)正向无功总电能示值
    
    BOOL bUpdateNegReaEng;             //(当前)反向无功电能总尖峰平谷示值
    BOOL bUpdatePhaANegReaEng;             //(A相)反向无功总电能示值
    BOOL bUpdatePhaBNegReaEng;             //(B相)反向无功总电能示值
    BOOL bUpdatePhaCNegReaEng;             //(C相)反向无功总电能示值
    
    BOOL bUpdateQuad1ReaEng;             //(当前)第一象限无功总尖峰平谷电能示值
    BOOL bUpdatePhaA1ReaEng;             //(A相)第一象限无功电能
    BOOL bUpdatePhaB1ReaEng;             //(B相)第一象限无功电能
    BOOL bUpdatePhaC1ReaEng;             //(C相)第一象限无功电能
    
    BOOL bUpdateQuad2ReaEng;             //(当前)第二象限无功总尖峰平谷电能示值
    BOOL bUpdatePhaA2ReaEng;             //(A相)第二象限无功电能
    BOOL bUpdatePhaB2ReaEng;             //(B相)第二象限无功电能
    BOOL bUpdatePhaC2ReaEng;             //(C相)第二象限无功电能
    
    BOOL bUpdateQuad3ReaEng;             //(当前)第三象限无功总尖峰平谷电能示值
    BOOL bUpdatePhaA3ReaEng;             //(A相)第三象限无功电能
    BOOL bUpdatePhaB3ReaEng;             //(B相)第三象限无功电能
    BOOL bUpdatePhaC3ReaEng;             //(C相)第三象限无功电能
    
    BOOL bUpdateQuad4ReaEng;             //(当前)第四象限无功总尖峰平谷电能示值
    BOOL bUpdatePhaA4ReaEng;             //(A相)第四象限无功电能
    BOOL bUpdatePhaB4ReaEng;             //(B相)第四象限无功电能
    BOOL bUpdatePhaC4ReaEng;             //(C相)第四象限无功电能
    
    BOOL bUpdatePosAppEng;             //(当前)正向视在电能数据块
    BOOL bUpdatePhaAActAppEng;             //(A相)正向视在电能
    BOOL bUpdatePhaBActAppEng;             //(B相)正向视在电能
    BOOL bUpdatePhaCActAppEng;             //(C相)正向视在电能
    
    BOOL bUpdateNegAppEng;             //(当前)反向视在电能数据块
    BOOL bUpdatePhaANegAppEng;             //(A相)反向视在电能
    BOOL bUpdatePhaBNegAppEng;             //(B相)反向视在电能
    BOOL bUpdatePhaCNegAppEng;             //(C相)反向视在电能

    /* 高精度 电能 */
    BOOL bUpdateCmbActEngHD;             //(当前)组合有功电能块
    
    BOOL bUpdatePosActEngHD;             //(当前)正向有功电能总尖峰平谷示值
    BOOL bUpdatePhaAPosActEngHD;             //(A相)正向有功总电能示值
    BOOL bUpdatePhaBPosActEngHD;             //(B相)正向有功总电能示值
    BOOL bUpdatePhaCPosActEngHD;             //(C相)正向有功总电能示值
    
    BOOL bUpdateNegActEngHD;             //(当前)反向有功电能总尖峰平谷示值
    BOOL bUpdatePhaANegActEngHD;             //(A相)反向有功总电能示值
    BOOL bUpdatePhaBNegActEngHD;             //(B相)反向有功总电能示值
    BOOL bUpdatePhaCNegActEngHD;             //(C相)反向有功总电能示值
    
    BOOL bUpdatePosReaEngHD;             //(当前)正向无功电能总尖峰平谷示值
    BOOL bUpdatePhaAPosReaEngHD;             //(A相)正向无功总电能示值
    BOOL bUpdatePhaBPosReaEngHD;             //(B相)正向无功总电能示值
    BOOL bUpdatePhaCPosReaEngHD;             //(C相)正向无功总电能示值
    
    BOOL bUpdateNegReaEngHD;             //(当前)反向无功电能总尖峰平谷示值
    BOOL bUpdatePhaANegReaEngHD;             //(A相)反向无功总电能示值
    BOOL bUpdatePhaBNegReaEngHD;             //(B相)反向无功总电能示值
    BOOL bUpdatePhaCNegReaEngHD;             //(C相)反向无功总电能示值
    
    BOOL bUpdateQuad1ReaEngHD;             //(当前)第一象限无功总尖峰平谷电能示值
    BOOL bUpdatePhaA1ReaEngHD;             //(A相)第一象限无功电能
    BOOL bUpdatePhaB1ReaEngHD;             //(B相)第一象限无功电能
    BOOL bUpdatePhaC1ReaEngHD;             //(C相)第一象限无功电能
    
    BOOL bUpdateQuad2ReaEngHD;             //(当前)第二象限无功总尖峰平谷电能示值
    BOOL bUpdatePhaA2ReaEngHD;             //(A相)第二象限无功电能
    BOOL bUpdatePhaB2ReaEngHD;             //(B相)第二象限无功电能
    BOOL bUpdatePhaC2ReaEngHD;             //(C相)第二象限无功电能
    
    BOOL bUpdateQuad3ReaEngHD;             //(当前)第三象限无功总尖峰平谷电能示值
    BOOL bUpdatePhaA3ReaEngHD;             //(A相)第三象限无功电能
    BOOL bUpdatePhaB3ReaEngHD;             //(B相)第三象限无功电能
    BOOL bUpdatePhaC3ReaEngHD;             //(C相)第三象限无功电能
    
    BOOL bUpdateQuad4ReaEngHD;             //(当前)第四象限无功总尖峰平谷电能示值
    BOOL bUpdatePhaA4ReaEngHD;             //(A相)第四象限无功电能
    BOOL bUpdatePhaB4ReaEngHD;             //(B相)第四象限无功电能
    BOOL bUpdatePhaC4ReaEngHD;             //(C相)第四象限无功电能
    
    BOOL bUpdatePosAppEngHD;             //(当前)正向视在电能数据块
    BOOL bUpdatePhaAActAppEngHD;             //(A相)正向视在电能
    BOOL bUpdatePhaBActAppEngHD;             //(B相)正向视在电能
    BOOL bUpdatePhaCActAppEngHD;             //(C相)正向视在电能
    
    BOOL bUpdateNegAppEngHD;             //(当前)反向视在电能数据块
    BOOL bUpdatePhaANegAppEngHD;             //(A相)反向视在电能
    BOOL bUpdatePhaBNegAppEngHD;             //(B相)反向视在电能
    BOOL bUpdatePhaCNegAppEngHD;             //(C相)反向视在电能

    /* 需量 */
    BOOL bUpdatePosActDmd;             //(当前)正向有功最大需量总尖峰平谷示值
    BOOL bUpdatePhaAPosActDmd;             //(A相)正向有功总最大需量示值
    BOOL bUpdatePhaBPosActDmd;             //(B相)正向有功总最大需量示值
    BOOL bUpdatePhaCPosActDmd;             //(C相)正向有功总最大需量示值
    
    BOOL bUpdateNegActDmd;             //(当前)反向有功最大需量总尖峰平谷示值
    BOOL bUpdatePhaANegActDmd;             //(A相)反向有功总最大需量示值
    BOOL bUpdatePhaBNegActDmd;             //(B相)反向有功总最大需量示值
    BOOL bUpdatePhaCNegActDmd;             //(C相)反向有功总最大需量示值
    
    BOOL bUpdatePosReaDmd;             //(当前)正向无功最大需量总尖峰平谷示值
    BOOL bUpdatePhaAPosReaDmd;             //(A相)正向无功总最大需量示值
    BOOL bUpdatePhaBPosReaDmd;             //(B相)正向无功总最大需量示值
    BOOL bUpdatePhaCPosReaDmd;             //(C相)正向无功总最大需量示值
    
    BOOL bUpdateNegReaDmd;             //(当前)反向无功最大需量总尖峰平谷示值
    BOOL bUpdatePhaANegReaDmd;             //(A相)反向无功总最大需量示值
    BOOL bUpdatePhaBNegReaDmd;             //(B相)反向无功总最大需量示值
    BOOL bUpdatePhaCNegReaDmd;             //(C相)反向无功总最大需量示值
    
    BOOL bUpdateQuad1ReaDmd;             //(当前)第一象限无功总尖峰平谷最大需量示值
    BOOL bUpdatePhaA1ReaDmd;             //(A相)第一象限无功最大需量
    BOOL bUpdatePhaB1ReaDmd;             //(B相)第一象限无功最大需量
    BOOL bUpdatePhaC1ReaDmd;             //(C相)第一象限无功最大需量
    
    BOOL bUpdateQuad2ReaDmd;             //(当前)第二象限无功总尖峰平谷最大需量示值
    BOOL bUpdatePhaA2ReaDmd;             //(A相)第二象限无功最大需量
    BOOL bUpdatePhaB2ReaDmd;             //(B相)第二象限无功最大需量
    BOOL bUpdatePhaC2ReaDmd;             //(C相)第二象限无功最大需量
    
    BOOL bUpdateQuad3ReaDmd;             //(当前)第三象限无功总尖峰平谷最大需量示值
    BOOL bUpdatePhaA3ReaDmd;             //(A相)第三象限无功最大需量
    BOOL bUpdatePhaB3ReaDmd;             //(B相)第三象限无功最大需量
    BOOL bUpdatePhaC3ReaDmd;             //(C相)第三象限无功最大需量
    
    BOOL bUpdateQuad4ReaDmd;             //(当前)第四象限无功总尖峰平谷最大需量示值
    BOOL bUpdatePhaA4ReaDmd;             //(A相)第四象限无功最大需量
    BOOL bUpdatePhaB4ReaDmd;             //(B相)第四象限无功最大需量
    BOOL bUpdatePhaC4ReaDmd;             //(C相)第四象限无功最大需量
    
    BOOL bUpdatePosAppDmd;             //(当前)正向视在最大需量数据块
    BOOL bUpdatePhaAActAppDmd;             //(A相)正向视在最大需量
    BOOL bUpdatePhaBActAppDmd;             //(B相)正向视在最大需量
    BOOL bUpdatePhaCActAppDmd;             //(C相)正向视在最大需量
    
    BOOL bUpdateNegAppDmd;             //(当前)反向视在最大需量数据块
    BOOL bUpdatePhaANegAppDmd;             //(A相)反向视在最大需量
    BOOL bUpdatePhaBNegAppDmd;             //(B相)反向视在最大需量
    BOOL bUpdatePhaCNegAppDmd;             //(C相)反向视在最大需量

    /* 实时数据 */
    BOOL bUpdateVolt; 			 //(当前)三相电压
    BOOL bUpdateVoltHD; 			 //(当前)高精度三相电压
    BOOL bUpdateZeroVolt; 			 //(当前)零序电压
    BOOL bUpdateCurr; 			 //(当前)三相电流
    BOOL bUpdateZeroCurr;              //零序电流
    BOOL bUpdateVoltAngle;              //电压相角
    BOOL bUpdateCurrAngle;              //电压相角
    BOOL bUpdateVoltCurrAngle;              //电压电流相角 
    BOOL bUpdateActPower;              //(当前)瞬时正向有功功率
    BOOL bUpdateReaPower; 		     //(当前)瞬时正向无功功率
    BOOL bUpdateAppPower;              //(当前)瞬时正向视在功率
    BOOL bUpdatePowerFactor;              //(当前)瞬时总及分相功率因数
    BOOL bUpdateLineFrz;              //电网频率
    BOOL bUpdateMetStusPara;             //电表运行状态字1-7
    BOOL bUpdateVoltUnRate;              //电压不平衡率
    BOOL bUpdateCurrUnRate;              //电流不平衡率

    /* 谐波 */
    BOOL bUpdateAVoltHarmonic;           	//A相电压谐波含有率（总及2…n次）
    BOOL bUpdateBVoltHarmonic;           	//B相电压谐波含有率（总及2…n次）
    BOOL bUpdateCVoltHarmonic;           	//C相电压谐波含有率（总及2…n次）
    BOOL bUpdateACurrHarmonic;           	//A相电流谐波含有率（总及2…n次）
    BOOL bUpdateBCurrHarmonic;           	//B相电流谐波含有率（总及2…n次）
    BOOL bUpdateCCurrHarmonic;           	//C相电流谐波含有率（总及2…n次）

    BOOL bUpdateAVoltHarmonicValue;           	//A相电压谐波含量（总及2…n次）
    BOOL bUpdateBVoltHarmonicValue;           	//B相电压谐波含量（总及2…n次）
    BOOL bUpdateCVoltHarmonicValue;           	//C相电压谐波含量（总及2…n次）
    BOOL bUpdateACurrHarmonicValue;           	//A相电流谐波含量（总及2…n次）
    BOOL bUpdateBCurrHarmonicValue;           	//B相电流谐波含量（总及2…n次）
    BOOL bUpdateCCurrHarmonicValue;           	//C相电流谐波含量（总及2…n次）
}UPDATE_FLAG_T;






typedef union 
{   // 小端模式下的双字节联合体
    uint8   Byte2[2];
    int16  Sinteger;
    uint16  Uinteger;
}__attribute__ ((packed)) BYTE2_INT16_TYPE;

typedef union 
{   // 小端模式下的4字节联合体
    uint8  Byte4[4];
    int    SLinteger;
    uint32 ULinteger; 
}__attribute__ ((packed)) BYTE4_INT32_TYPE;

typedef union 
{   // 小端模式下的8字节联合体
    uint8  Byte4[8];
    int64  SLinteger;
    uint64 ULinteger;
}__attribute__ ((packed)) BYTE8_INT64_TYPE;

typedef struct         
{
    uint8 ss;   // 秒
    uint8 nn;   // 分
    uint8 hh;   // 时
    uint8 ww;   // 星期
    uint8 dd;   // 日
    uint8 mm;   // 月
    uint8 yy;   // 年
} __attribute__ ((packed)) REAL_TIME_TYPE;

/*******************************************************************************
* 时间相关结构体
*******************************************************************************/
typedef struct
{    
    uint8    ss;
    uint8    nn;
    uint8    hh;
    uint8    WW;
    uint8    DD;
    uint8    MM;
    uint8    YY;
}__attribute__ ((packed))YYMMDDWWhhnnss_TYPE;     // YYMMDDWWhhnnss结构体声明

typedef struct
{    
    uint8    ss;
    uint8    nn;
    uint8    hh;
    uint8    DD;
    uint8    MM;
    uint8    YY;
}__attribute__ ((packed))YYMMDDhhnnss_TYPE;       // YYMMDDhhnnss结构体声明

typedef struct
{    
    uint8    nn;
    uint8    hh;
    uint8    DD;
    uint8    MM;
    uint8    YY;
}__attribute__ ((packed))YYMMDDhhnn_TYPE;         // YYMMDDhhnn结构体声明

typedef struct
{    
    uint8    DD;
    uint8    MM;
    uint8    YY;
}__attribute__ ((packed))YYMMDD_TYPE;             // YYMMDD结构体声明

typedef struct
{    
    uint8    nn;
    uint8    hh;
    uint8    DD;
    uint8    MM;
}__attribute__ ((packed))MMDDhhnn_TYPE;           // MMDDhhnn结构体声明

typedef struct
{    
    uint8    DD;
    uint8    MM;
}__attribute__ ((packed))MMDD_TYPE;               // MMDD结构体声明

typedef struct
{    
    uint8    nn;
    uint8    hh;
    uint8    DD;
}__attribute__ ((packed))DDhhnn_TYPE;             // DDhhnn结构体声明

typedef struct
{
    uint8    hh;
    uint8    DD;
}__attribute__ ((packed))DDhh_TYPE;               // DDhh类型

typedef struct
{    
    uint8    ss;
    uint8    nn;
    uint8    hh;
}__attribute__ ((packed))hhnnss_TYPE;             // hhnnss结构体声明

typedef struct
{    
    uint8    nn;
    uint8    hh;
}__attribute__ ((packed))hhnn_TYPE;               // hhnn结构体声明

/*******************************************************************************
* 私有数据结构体
*******************************************************************************/
typedef struct
{
    YYMMDDWWhhnnss_TYPE          LastCompTime;    // 上次比较时间

}PRIVATE_DATA_TYPE;


/*******************************************************************************
* 实时数据结构体
*******************************************************************************/

typedef struct
{
    YYMMDDWWhhnnss_TYPE          CurTime;         // 当前时间
    YYMMDDhhnnss_TYPE            BuildTime;       // 生成时间(不带WW)

}COMMON_DATA_TYPE;



/*******************************************************************************
* 状态字、特征字、模式字、错误信息字
*******************************************************************************/
typedef struct
{    
    uint16 ReservedBit1          :1;              // 保留
    uint16 DemandModeFlag        :1;              // 需量积算方式(0滑差，1区间)
    uint16 IntBatVoltFlag        :1;              // 时钟电池(0正常，1欠压)
    uint16 ExtBatVoltFlag        :1;              // 停电抄表电池(0正常，1欠压)
    uint16 ActPowNegFlag         :1;              // 有功功率方向(0正向、1反向)
    uint16 ReaPowNegFlag         :1;              // 无功功率方向(0正向、1反向)
    uint16 ReservedBit7          :1;              // 磁场干扰异常(扩展)
    uint16 ReservedBit8          :1;              // 保留
}__attribute__ ((packed)) METER_STATE1_BIT_TYPE;  // 电表运行状态字1

typedef union
{
    METER_STATE1_BIT_TYPE    BitSect;
    uint16                    Word;
}__attribute__ ((packed)) METER_STATE1_TYPE;

typedef struct
{    
    uint16 PhaAActPowNegFlag    :1;               // A相有功功率方向    
    uint16 PhaBActPowNegFlag    :1;               // B相有功功率方向    
    uint16 PhaCActPowNegFlag    :1;               // C相有功功率方向
    uint16 ReservedBit4         :1;               // 保留    
    uint16 PhaAReaPowNegFlag    :1;               // A相无功功率方向    
    uint16 PhaBReaPowNegFlag    :1;               // B相无功功率方向    
    uint16 PhaCReaPowNegFlag    :1;               // C相无功功率方向    
    uint16 ReservedBit8         :1;               // 保留    
}__attribute__ ((packed)) METER_STATE2_BIT_TYPE;  // 电表运行状态字2 注：0代表正向，1代表反向

typedef union
{
    METER_STATE2_BIT_TYPE    BitSect;
    uint16                   Word;
}__attribute__ ((packed)) METER_STATE2_TYPE;      // 电表运行状态字2 

typedef struct
{    
    uint16 CurPeriodFlag          :1;             // 当前运行时段(0第一套，1第二套)
    uint16 SupEletModeFlag        :2;             // 供电方式(00主电源，01辅助电源，10电池供电)
    uint16 ProgAllowFlag          :1;             // 编程允许(0禁止，1许可)
    uint16 RelayStatusFlag        :1;             // 继电器状态(0通，1断)
    uint16 CurTimZoneFlag         :1;             // 当前运行时区(0第一套，1第二套)
    uint16 RelayOrderFlag         :1;             // 继电器命令状态(0通，1断)
    uint16 PreJumpAlarm           :1;             // 预跳闸报警状态(0无，1有)
    uint16 MetModeFlag            :2;             // 电能表类型(00非预付费表，01电量型预付费表，10电费型预付费表
    uint16 CurTarifMark           :1;             // 当前运行分时费率 0:第一套 1:第二套
    uint16 CurStepMark            :1;             // 当前阶梯 0第一套，1第二套
}__attribute__ ((packed)) METER_STATE3_BIT_TYPE;  // 电表运行状态字3（操作类）注：编程允许一般指编程按键状态

typedef union
{
    METER_STATE3_BIT_TYPE    BitSect;
    uint16                   Word;
}__attribute__ ((packed)) METER_STATE3_TYPE;      // 电表运行状态字3（操作类）

typedef struct
{    
    uint16 LossVoltFlag         :1;               // 失压
    uint16 LackVoltFlag         :1;               // 欠压
    uint16 OverVoltFlag         :1;               // 过压
    uint16 LossCurrFlag         :1;               // 失流
    uint16 OverCurrFlag         :1;               // 过流
    uint16 OverLoadFlag         :1;               // 过载    
    uint16 CurrRevsFlag         :1;               // 反向    
    uint16 BreakVoltFlag        :1;               // 断相
}__attribute__ ((packed)) METER_STATE456_BIT_TYPE;// 电表运行状态字456（ABC相故障状态）注：0代表无此类故障，1代表当前发生此类故障

typedef union
{
    METER_STATE456_BIT_TYPE   BitSect;
    uint16                    Word;    
}__attribute__ ((packed)) METER_STATE456_TYPE;    // 电表运行状态字456（ABC相故障状态）
    
typedef struct
{    
    uint16 VoltConvFlag         :1;               // 电压逆相序
    uint16 CurrConvFlag         :1;               // 电流逆相序
    uint16 VoltUnblFlag         :1;               // 电压不平衡
    uint16 CurrUnblFlag         :1;               // 电流不平衡
    uint16 AsPowerLossFlag      :1;               // 辅助电源丢失
    uint16 PowerLossFlag        :1;               // 电源丢失
    uint16 OverDemandFlag       :1;               // 需量超限
    uint16 LackPfFlag           :1;               // 总功率因数超下限
    uint16 CurrUnblHighFlag     :1;               // 电流严重不平衡
}__attribute__ ((packed)) METER_STATE7_BIT_TYPE;  // 电表运行状态字7（合相故障状态）注：0代表无此类故障，1代表当前发生此类故障。

typedef union
{
    METER_STATE7_BIT_TYPE    BitSect;
    uint16                   Word;
}__attribute__ ((packed)) METER_STATE7_TYPE;      // 电表运行状态字7（合相故障状态）

typedef struct
{
    METER_STATE1_TYPE    Rsw1;                    // 电表运行状态字1
    METER_STATE2_TYPE    Rsw2;                    // 电表运行状态字2
    METER_STATE3_TYPE    Rsw3;                    // 电表运行状态字3
    METER_STATE456_TYPE  Rsw4;                    // 电表运行状态字4
    METER_STATE456_TYPE  Rsw5;                    // 电表运行状态字5
    METER_STATE456_TYPE  Rsw6;                    // 电表运行状态字6
    METER_STATE7_TYPE    Rsw7;                    // 电表运行状态字7
}__attribute__ ((packed)) METER_STATE_WORD_TYPE;  // 电表运行状态字集合类型

/*******************************************************************************
* 分相电能量结构体
*******************************************************************************/
typedef struct
{    
    BYTE4_INT32_TYPE     Energy;
}__attribute__ ((packed)) SNGL_ENERGY_TYPE;       // 分相有功能量数据结构体声明;

typedef struct
{
    SNGL_ENERGY_TYPE     Postive;
    SNGL_ENERGY_TYPE     Negative;
}__attribute__ ((packed)) PHASE_ACT_ENERGY_TYPE;  // 分相有功能量数据结构体声明

typedef struct
{    
    SNGL_ENERGY_TYPE     Postive;                
    SNGL_ENERGY_TYPE     Negative;
    SNGL_ENERGY_TYPE     Quadrant1;
    SNGL_ENERGY_TYPE     Quadrant2;
    SNGL_ENERGY_TYPE     Quadrant3;
    SNGL_ENERGY_TYPE     Quadrant4;
}__attribute__ ((packed)) PHASE_REA_ENERGY_TYPE;  // 分相无功能量数据结构体声明

typedef struct
{    
    SNGL_ENERGY_TYPE    Postive;
    SNGL_ENERGY_TYPE     Negative;
}__attribute__ ((packed)) PHASE_APP_ENERGY_TYPE;  // 分相视在能量数据结构体声明

typedef struct
{    
    PHASE_ACT_ENERGY_TYPE Active;                // 有功
    PHASE_REA_ENERGY_TYPE Reactive;              // 无功
    PHASE_APP_ENERGY_TYPE Apparent;              // 视在
}__attribute__ ((packed)) PHASE_ACT_REA_APP_ENERGY_TYPE;    // 分相有无功能量数据结构体







typedef struct
{    
    // 需量值 xx.xxxx
    uint8 DemNum[3];
}__attribute__ ((packed)) DEM_NUM_TYPE;

typedef struct 
{    // 需量时间声明
    uint8 DemTime[6];
}__attribute__ ((packed))TERM_DEM_TIME_TYPE;

typedef struct
{    
    // 事件累计次数结构体
    uint8 Num[2];    
} __attribute__ ((packed)) NUMBER_TYPE;

typedef struct
{   // 事件累计时间结构体
    uint8 Time[3];    
} __attribute__ ((packed)) TIME_TYPE;

typedef struct
{    
    // 事件起止时间结构体
    uint8 Time[4];    // 月、日、时、分    
} __attribute__ ((packed)) COM_TIME_TYPE;

#define  SIGNAL_INFO_LEN  (16)  //交采普通参数类型

typedef struct{
    uint8 nFlag;                 //1发生, 0未发生
    OOP_DATETIME_S_T stIdenTime; //识别成功时间
    uint8 nPhase;                //信号所属相位, 0:未知, 1:A相, 2:B相, 3:C相, 4:三相
    int32 nSignalS1;             //信号强度S1
    int32 nSignalS2;             //信号强度S2
    int32 nNoise;                //背景噪声
    uint8 nInfoLen;              //识别信号长度
    uint8 aInfo[SIGNAL_INFO_LEN];//识别信号内容
}TOPO_IDEN_RESULT;//(自定义扩展)拓扑识别结果(浙江)

/*******************************************************************************
* 交采模块共享内存
********************************************************************************/
typedef struct 
{   // 交采与统计模块共享内存结构体声明
    BYTE4_INT32_TYPE PosActEng[MAX_TARIFF_NUM+1];         // 正向有功-总,尖,峰,平,谷
    BYTE4_INT32_TYPE NegActEng[MAX_TARIFF_NUM+1];         // 反向有功-总,尖,峰,平,谷
    BYTE4_INT32_TYPE PosReaEng[MAX_TARIFF_NUM+1];         // 正向无功-总,尖,峰,平,谷
    BYTE4_INT32_TYPE NegReaEng[MAX_TARIFF_NUM+1];         // 反向无功-总,尖,峰,平,谷
    BYTE4_INT32_TYPE Quad1ReaEng[MAX_TARIFF_NUM+1];       // 总,尖,峰,平,谷 象限1
    BYTE4_INT32_TYPE Quad4ReaEng[MAX_TARIFF_NUM+1];       // 总,尖,峰,平,谷 象限4
    BYTE4_INT32_TYPE Quad2ReaEng[MAX_TARIFF_NUM+1];       // 总,尖,峰,平,谷 象限2
    BYTE4_INT32_TYPE Quad3ReaEng[MAX_TARIFF_NUM+1];       // 总,尖,峰,平,谷 象限3
    BYTE4_INT32_TYPE CmbActEng[MAX_TARIFF_NUM+1];         // 组合有功-总,尖,峰,平,谷
    BYTE4_INT32_TYPE PosAppEng[MAX_TARIFF_NUM+1];         // 正向视在电能-总,尖,峰,平,谷
    BYTE4_INT32_TYPE NegAppEng[MAX_TARIFF_NUM+1];         // 反向视在电能-总,尖,峰,平,谷

    BYTE4_INT32_TYPE PhaAPosActEng[PhaMaxTariffNum+1];    // A相 正有 
    BYTE4_INT32_TYPE PhaANegActEng[PhaMaxTariffNum+1];    // A相 反有
    BYTE4_INT32_TYPE PhaAPosReaEng[PhaMaxTariffNum+1];    // A相 正无
    BYTE4_INT32_TYPE PhaANegReaEng[PhaMaxTariffNum+1];    // A相 反无
    
    BYTE4_INT32_TYPE PhaA1ReaEng[PhaMaxTariffNum+1];    // A相 象限1
    BYTE4_INT32_TYPE PhaA2ReaEng[PhaMaxTariffNum+1];    // A相 象限2
    BYTE4_INT32_TYPE PhaA3ReaEng[PhaMaxTariffNum+1];    // A相 象限3
    BYTE4_INT32_TYPE PhaA4ReaEng[PhaMaxTariffNum+1];    // A相 象限4
    BYTE4_INT32_TYPE PhaACmbActEng[PhaMaxTariffNum+1];    // A相 组合有功
    BYTE4_INT32_TYPE PhaAActAppEng[PhaMaxTariffNum+1];    // A相 正向视在电能
    BYTE4_INT32_TYPE PhaANegAppEng[PhaMaxTariffNum+1];    // A相 反向视在电能
    
    BYTE4_INT32_TYPE PhaBPosActEng[PhaMaxTariffNum+1];    // B相 正有 
    BYTE4_INT32_TYPE PhaBNegActEng[PhaMaxTariffNum+1];    // B相 反有
    BYTE4_INT32_TYPE PhaBPosReaEng[PhaMaxTariffNum+1];    // B相 正无
    BYTE4_INT32_TYPE PhaBNegReaEng[PhaMaxTariffNum+1];    // B相 反无

    BYTE4_INT32_TYPE PhaB1ReaEng[PhaMaxTariffNum+1];    // B相 象限1
    BYTE4_INT32_TYPE PhaB2ReaEng[PhaMaxTariffNum+1];    // B相 象限2
    BYTE4_INT32_TYPE PhaB3ReaEng[PhaMaxTariffNum+1];    // B相 象限3
    BYTE4_INT32_TYPE PhaB4ReaEng[PhaMaxTariffNum+1];    // B相 象限4
    BYTE4_INT32_TYPE PhaBCmbActEng[PhaMaxTariffNum+1];    // B相 组合有功
    BYTE4_INT32_TYPE PhaBActAppEng[PhaMaxTariffNum+1];    // B相 正向视在电能
    BYTE4_INT32_TYPE PhaBNegAppEng[PhaMaxTariffNum+1];    // B相 反向视在电能
    
    BYTE4_INT32_TYPE PhaCPosActEng[PhaMaxTariffNum+1];    // C相 正有 
    BYTE4_INT32_TYPE PhaCNegActEng[PhaMaxTariffNum+1];    // C相 反有
    BYTE4_INT32_TYPE PhaCPosReaEng[PhaMaxTariffNum+1];    // C相 正无
    BYTE4_INT32_TYPE PhaCNegReaEng[PhaMaxTariffNum+1];    // C相 反无

    BYTE4_INT32_TYPE PhaC1ReaEng[PhaMaxTariffNum+1];    // C相 象限1
    BYTE4_INT32_TYPE PhaC2ReaEng[PhaMaxTariffNum+1];    // C相 象限2
    BYTE4_INT32_TYPE PhaC3ReaEng[PhaMaxTariffNum+1];    // C相 象限3
    BYTE4_INT32_TYPE PhaC4ReaEng[PhaMaxTariffNum+1];    // C相 象限4
    BYTE4_INT32_TYPE PhaCCmbActEng[PhaMaxTariffNum+1];    // C相 组合有功
    BYTE4_INT32_TYPE PhaCActAppEng[PhaMaxTariffNum+1];    // C相 正向视在电能
    BYTE4_INT32_TYPE PhaCNegAppEng[PhaMaxTariffNum+1];    // C相 反向视在电能

    /***高精度电能量数据 begin***/
    
    BYTE8_INT64_TYPE PosActEngHD[MAX_TARIFF_NUM+1];         // 正向有功-总,尖,峰,平,谷
    BYTE8_INT64_TYPE NegActEngHD[MAX_TARIFF_NUM+1];         // 反向有功-总,尖,峰,平,谷
    BYTE8_INT64_TYPE PosReaEngHD[MAX_TARIFF_NUM+1];         // 正向无功-总,尖,峰,平,谷
    BYTE8_INT64_TYPE NegReaEngHD[MAX_TARIFF_NUM+1];         // 反向无功-总,尖,峰,平,谷
    BYTE8_INT64_TYPE Quad1ReaEngHD[MAX_TARIFF_NUM+1];       // 总,尖,峰,平,谷 象限1
    BYTE8_INT64_TYPE Quad4ReaEngHD[MAX_TARIFF_NUM+1];       // 总,尖,峰,平,谷 象限4
    BYTE8_INT64_TYPE Quad2ReaEngHD[MAX_TARIFF_NUM+1];       // 总,尖,峰,平,谷 象限2
    BYTE8_INT64_TYPE Quad3ReaEngHD[MAX_TARIFF_NUM+1];       // 总,尖,峰,平,谷 象限3
    BYTE8_INT64_TYPE CmbActEngHD[MAX_TARIFF_NUM+1];         // 组合有功-总,尖,峰,平,谷
    BYTE8_INT64_TYPE PosAppEngHD[MAX_TARIFF_NUM+1];         // 正向视在电能-总,尖,峰,平,谷
    BYTE8_INT64_TYPE NegAppEngHD[MAX_TARIFF_NUM+1];         // 反向视在电能-总,尖,峰,平,谷

    BYTE8_INT64_TYPE PhaAPosActEngHD[PhaMaxTariffNum+1];    // A相 正有 
    BYTE8_INT64_TYPE PhaANegActEngHD[PhaMaxTariffNum+1];    // A相 反有
    BYTE8_INT64_TYPE PhaAPosReaEngHD[PhaMaxTariffNum+1];    // A相 正无
    BYTE8_INT64_TYPE PhaANegReaEngHD[PhaMaxTariffNum+1];    // A相 反无
    
    BYTE8_INT64_TYPE PhaA1ReaEngHD[PhaMaxTariffNum+1];    // A相 象限1
    BYTE8_INT64_TYPE PhaA2ReaEngHD[PhaMaxTariffNum+1];    // A相 象限2
    BYTE8_INT64_TYPE PhaA3ReaEngHD[PhaMaxTariffNum+1];    // A相 象限3
    BYTE8_INT64_TYPE PhaA4ReaEngHD[PhaMaxTariffNum+1];    // A相 象限4
    BYTE8_INT64_TYPE PhaACmbActEngHD[PhaMaxTariffNum+1];    // A相 组合有功
    BYTE8_INT64_TYPE PhaAActAppEngHD[PhaMaxTariffNum+1];    // A相 正向视在电能
    BYTE8_INT64_TYPE PhaANegAppEngHD[PhaMaxTariffNum+1];    // A相 反向视在电能
    
    BYTE8_INT64_TYPE PhaBPosActEngHD[PhaMaxTariffNum+1];    // B相 正有 
    BYTE8_INT64_TYPE PhaBNegActEngHD[PhaMaxTariffNum+1];    // B相 反有
    BYTE8_INT64_TYPE PhaBPosReaEngHD[PhaMaxTariffNum+1];    // B相 正无
    BYTE8_INT64_TYPE PhaBNegReaEngHD[PhaMaxTariffNum+1];    // B相 反无

    BYTE8_INT64_TYPE PhaB1ReaEngHD[PhaMaxTariffNum+1];    // B相 象限1
    BYTE8_INT64_TYPE PhaB2ReaEngHD[PhaMaxTariffNum+1];    // B相 象限2
    BYTE8_INT64_TYPE PhaB3ReaEngHD[PhaMaxTariffNum+1];    // B相 象限3
    BYTE8_INT64_TYPE PhaB4ReaEngHD[PhaMaxTariffNum+1];    // B相 象限4
    BYTE8_INT64_TYPE PhaBCmbActEngHD[PhaMaxTariffNum+1];    // B相 组合有功
    BYTE8_INT64_TYPE PhaBActAppEngHD[PhaMaxTariffNum+1];    // B相 正向视在电能
    BYTE8_INT64_TYPE PhaBNegAppEngHD[PhaMaxTariffNum+1];    // B相 反向视在电能
    
    BYTE8_INT64_TYPE PhaCPosActEngHD[PhaMaxTariffNum+1];    // C相 正有 
    BYTE8_INT64_TYPE PhaCNegActEngHD[PhaMaxTariffNum+1];    // C相 反有
    BYTE8_INT64_TYPE PhaCPosReaEngHD[PhaMaxTariffNum+1];    // C相 正无
    BYTE8_INT64_TYPE PhaCNegReaEngHD[PhaMaxTariffNum+1];    // C相 反无

    BYTE8_INT64_TYPE PhaC1ReaEngHD[PhaMaxTariffNum+1];    // C相 象限1
    BYTE8_INT64_TYPE PhaC2ReaEngHD[PhaMaxTariffNum+1];    // C相 象限2
    BYTE8_INT64_TYPE PhaC3ReaEngHD[PhaMaxTariffNum+1];    // C相 象限3
    BYTE8_INT64_TYPE PhaC4ReaEngHD[PhaMaxTariffNum+1];    // C相 象限4
    BYTE8_INT64_TYPE PhaCCmbActEngHD[PhaMaxTariffNum+1];    // C相 组合有功
    BYTE8_INT64_TYPE PhaCActAppEngHD[PhaMaxTariffNum+1];    // C相 正向视在电能
    BYTE8_INT64_TYPE PhaCNegAppEngHD[PhaMaxTariffNum+1];    // C相 反向视在电能
    
    /***高精度电能量数据 end***/

    DEM_NUM_TYPE CurrDemand[4][MAX_TARIFF_NUM+1];         // 正有、反有、正无、反无需量
    TERM_DEM_TIME_TYPE CurrDemandTime[4][MAX_TARIFF_NUM+1];// 正有、反有、正无、反无需量时间

    DEM_NUM_TYPE QuaDemand[4][MAX_TARIFF_NUM+1];         // 一、二、三、四象限需量
    TERM_DEM_TIME_TYPE QuaDemandTime[4][MAX_TARIFF_NUM+1];// 一、二、三、四象限需量时间

    DEM_NUM_TYPE AppaDemand[2][MAX_TARIFF_NUM+1];         // 正向视在、反向视在需量
    TERM_DEM_TIME_TYPE AppaDemandTime[2][MAX_TARIFF_NUM+1];// 正向视在、反向视在需量时间

    
    DEM_NUM_TYPE PosActDemand[3];          // A、B、C相正向有功需量
    TERM_DEM_TIME_TYPE PosActDemandTime[3];// A、B、C相正向有功需量发生时间

    DEM_NUM_TYPE NegActDemand[3];          // A、B、C相反有需量
    TERM_DEM_TIME_TYPE NegActDemandTime[3];// A、B、C相反有需量发生时间

    DEM_NUM_TYPE PosReaDemand[3];          // A、B、C相正无需量
    TERM_DEM_TIME_TYPE PosReaDemandTime[3];// A、B、C相正无需量发生时间

    DEM_NUM_TYPE NegReaDemand[3];          // A、B、C相反无需量
    TERM_DEM_TIME_TYPE NegReaDemandTime[3];// A、B、C相反无需量发生时间

    DEM_NUM_TYPE Qua1Demand[3];          // A、B、C相1象限需量
    TERM_DEM_TIME_TYPE Qua1DemandTime[3];// A、B、C相1象限需量发生时间

    DEM_NUM_TYPE Qua2Demand[3];          // A、B、C相2象限需量
    TERM_DEM_TIME_TYPE Qua2DemandTime[3];// A、B、C相2象限需量发生时间

    DEM_NUM_TYPE Qua3Demand[3];          // A、B、C相3象限需量
    TERM_DEM_TIME_TYPE Qua3DemandTime[3];// A、B、C相3象限需量发生时间

    DEM_NUM_TYPE Qua4Demand[3];          // A、B、C相4象限需量
    TERM_DEM_TIME_TYPE Qua4DemandTime[3];// A、B、C相4象限需量发生时间

    DEM_NUM_TYPE ActAppDemand[3];          // A、B、C相正向视在需量
    TERM_DEM_TIME_TYPE ActAppDemandTime[3];// A、B、C相正向视在需量发生时间

    DEM_NUM_TYPE NegAppDemand[3];          // A、B、C相反向视在需量
    TERM_DEM_TIME_TYPE NegAppDemandTime[3];// A、B、C相反向视在需量发生时间

    int PosActPower;           // 正总有功功率
    int NegActPower;           // 反总有功功率
    int PhaAActPower;          // A相有功功率
    int PhaBActPower;          // B相有功功率
    int PhaCActPower;          // C相有功功率

    int PosReaPower;           // 正总无功功率
    int NegReaPower;           // 反总无功功率
    int PhaAReaPower;          // A相无功功率
    int PhaBReaPower;          // B相无功功率
    int PhaCReaPower;          // C相无功功率

    int PosApparentPower;         // 瞬时视在功率
    int NegApparentPower;         // 瞬时视在功率
    int PhaAAppPower;          // A相视在功率
    int PhaBAppPower;          // B相视在功率
    int PhaCAppPower;          // C相视在功率

    int16 PowerFactor;        // 总功率因数
    int16 PhaAPowerFactor;    // A相功率因数
    int16 PhaBPowerFactor;    // B相功率因数
    int16 PhaCPowerFactor;    // C相功率因数

    uint16 PhaAVolt;           // A相电压 
    uint16 PhaBVolt;           // B相电压
    uint16 PhaCVolt;           // C相电压
    uint16 PhaZeroVolt;        // 零序电压
    uint16 PhaPosVolt;         // 正序电压
    uint16 PhaNegVolt;         // 负序电压

    uint32 LineABVolt;         //AB线电压
    uint32 LineBCVolt;         //BC线电压
    uint32 LineCAVolt;         //CA线电压

    uint32 PhaACurr;           // A相电流 XX.XXXX
    uint32 PhaBCurr;           // B相电流 
    uint32 PhaCCurr;           // C相电流
    uint32 PhaMCurr;           // 零序电流
    uint16 PhaPosCurr;         // 正序电流
    uint16 PhaNegCurr;         // 负序电流
    uint32 PhaGCurr;           // 中性线电流
    uint32 ResidCurr;          // 剩余电流
    
    uint32 LineFrz;			   //电网频率
    uint32 LineAFrz;		   //A相频率
    uint32 LineBFrz;		   //B相频率
    uint32 LineCFrz;		   //C相频率
    int16 MtrTemp;
    uint16 ClkBatVol;
    uint16 CBBatVol;
    uint32 ActDemand;
    uint32 ReDemand;
    uint32 AppDemand;
    uint16 VoltUnbalanceRate;			   //电压不平衡率
    uint16 CurrUnbalanceRate;			   //电流不平衡率
    
    uint16 PhaAAngle;        // A相电压电流相角
    uint16 PhaBAngle;        // B相电压电流相角
    uint16 PhaCAngle;        // C相电压电流相角
    uint16 PhaAVoltAngle;    // A相电压相角
    uint16 PhaBVoltAngle;    // B相电压相角
    uint16 PhaCVoltAngle;    // C相电压相角
    uint16 PhaACurrAngle;    // A相电流相角
    uint16 PhaBCurrAngle;    // B相电流相角
    uint16 PhaCCurrAngle;    // C相电流相角

    uint16 PhaAVoltHarmonic[MAX_HARMONIC_NUM];    // A相电压谐波含有率（总及2…n次）
    uint16 PhaBVoltHarmonic[MAX_HARMONIC_NUM];    // B相电压谐波含有率（总及2…n次）
    uint16 PhaCVoltHarmonic[MAX_HARMONIC_NUM];    // C相电压谐波含有率（总及2…n次）
    uint16 PhaACurrHarmonic[MAX_HARMONIC_NUM];    // A相电流谐波含有率（总及2…n次）
    uint16 PhaBCurrHarmonic[MAX_HARMONIC_NUM];    // B相电流谐波含有率（总及2…n次）
    uint16 PhaCCurrHarmonic[MAX_HARMONIC_NUM];    // C相电流谐波含有率（总及2…n次）

    uint16 PhaAVoltHarmonicValue[MAX_HARMONIC_NUM];    // A相电压谐波含量（总及2…n次）
    uint16 PhaBVoltHarmonicValue[MAX_HARMONIC_NUM];    // B相电压谐波含量（总及2…n次）
    uint16 PhaCVoltHarmonicValue[MAX_HARMONIC_NUM];    // C相电压谐波含量（总及2…n次）
    uint32 PhaACurrHarmonicValue[MAX_HARMONIC_NUM];    // A相电流谐波含量（总及2…n次）
    uint32 PhaBCurrHarmonicValue[MAX_HARMONIC_NUM];    // B相电流谐波含量（总及2…n次）
    uint32 PhaCCurrHarmonicValue[MAX_HARMONIC_NUM];    // C相电流谐波含量（总及2…n次）

    OOP_TIMEZONE_T CurrZoneTableRD;//当前套时区表(从交采模块读取)
    OOP_TIMEZONE_T AddiZoneTableRD;//备用套时区表(从交采模块读取)
    OOP_DAYPERIOD_T CurrDayPeriTableRD;//当前套日时段表(从交采模块读取)
    OOP_DAYPERIOD_T AddiDayPeriTableRD;//当前套日时段表(从交采模块读取)
    uint8 MaxDemandPeriodRD;//最大需量周期(从交采模块读取)
    uint8 SlipDiffTimeRD;//滑差时间(从交采模块读取)

    OOP_TIMEZONE_T CurrZoneTableSet;//当前套时区表(设置交采模块)
    OOP_TIMEZONE_T AddiZoneTableSet;//备用套时区表(设置交采模块)
    OOP_DAYPERIOD_T CurrDayPeriTableSet;//当前套日时段表(设置交采模块)
    OOP_DAYPERIOD_T AddiDayPeriTableSet;//备用套日时段表(设置交采模块)
    uint8 MaxDemandPeriodSet;//最大需量周期(设置交采模块)
    uint8 SlipDiffTimeSet;//滑差时间(设置交采模块)

    BOOL CurrZoneNumSetFlag;//当前套时区表(设置交采模块)标志 时区数量
    BOOL AddiZoneNumSetFlag;//备用套时区表(设置交采模块)标志 时区数量
    BOOL CurrDayTableNumSetFlag;//当前套日时段表(设置交采模块)标志 表数量
    BOOL AddiDayTalbeNumSetFlag;//备用套日时段表(设置交采模块)标志 表数量
    BOOL CurrDayPeriNumSetFlag;//当前套日时段表(设置交采模块)标志 时段数量
    BOOL AddiDayPeriNumSetFlag;//备用套日时段表(设置交采模块)标志 时段数量
    uint8 CurrDayPeriNumSetFlagArr[OOP_MAX_DAYPERIODS];//当前套日时段表(设置交采模块)标志 时段数量
    uint8 AddiDayPeriNumSetFlagArr[OOP_MAX_DAYPERIODS];//备用套日时段表(设置交采模块)标志 时段数量

    BOOL CurrZoneTableSetFlag;//当前套时区表(设置交采模块)标志
    BOOL AddiZoneTableSetFlag;//备用套时区表(设置交采模块)标志
    BOOL CurrDayPeriTableSetFlag;//当前套日时段表(设置交采模块)标志
    BOOL AddiDayPeriTableSetFlag;//备用套日时段表(设置交采模块)标志
    uint8 CurrDayPeriSetFlagArr[OOP_MAX_DAYPERIODS];//当前套日时段表(设置交采模块)标志
    uint8 AddiDayPeriSetFlagArr[OOP_MAX_DAYPERIODS];//备用套日时段表(设置交采模块)标志
    BOOL MaxDemandPeriodSetFlag;//最大需量周期(设置交采模块)标志
    BOOL SlipDiffTimeSetFlag;//滑差时间(设置交采模块)标志

    OOP_DATETIME_S_T DateTimeSet;//终端的设置时间
    BOOL DateSetFlag;//终端的设置日期标志
    BOOL TimeSetFlag;//终端的设置时间标志

    OOP_DATETIME_S_T AddiZoneChangeTimeRD;//备用套时区表切换时间 读取
    OOP_DATETIME_S_T AddiPeriChangeTimeRD;//备用套日时段表切换时间 读取
    OOP_PERIOD_T ZonePeriNumRD;//时区时段数 读取

    OOP_BITSTR8_T ActPowCombWordRD;//有功组合方式特征字 读取
    OOP_BITSTR8_T ReaPowComb1WordRD;//无功组合方式1特征字 读取
    OOP_BITSTR8_T ReaPowComb2WordRD;//无功组合方式2特征字 读取

    OOP_DATETIME_S_T AddiZoneChangeTimeSet;//备用套时区表切换时间 设置
    OOP_DATETIME_S_T AddiPeriChangeTimeSet;//备用套日时段表切换时间 设置
    OOP_PERIOD_T ZonePeriNumSet;//时区时段数 设置

    OOP_BITSTR8_T ActPowCombWordSet;//有功组合方式特征字 设置
    OOP_BITSTR8_T ReaPowComb1WordSet;//无功组合方式1特征字 设置
    OOP_BITSTR8_T ReaPowComb2WordSet;//无功组合方式2特征字 设置

    BOOL ZoneTableNumSetFlag;//时区表(设置交采模块)标志
    BOOL DayPeriTableNumSetFlag;//日时段表(设置交采模块)标志
    BOOL DayPeriNumSetFlag;//日时段(设置交采模块)标志
    BOOL CostRateNumSetFlag;//费率数(设置交采模块)标志
    BOOL HolidayNumSetFlag;//假日数(设置交采模块)标志

    BOOL AddiZoneChangeTimeSetFlag;//备用套时区表切换时间 标志
    BOOL AddiPeriChangeTimeSetFlag;//备用套日时段表切换时间 标志
    BOOL ActPowCombWordSetFlag;//有功组合方式特征字 标志
    BOOL ReaPowComb1WordSetFlag;//无功组合方式1特征字 标志
    BOOL ReaPowComb2WordSetFlag;//无功组合方式2特征字 标志

    uint8 g_nCurrDayPeriLabel;//交采模块目前运行的日时段表是第几套, 0：当前运行第一套, 1：当前运行第二套

    NUMBER_TYPE BreakPhaNum[4];    // 总、A、B、C断相次数
    TIME_TYPE BreakPhaTime[4];     // 总、A、B、C断相时间
    COM_TIME_TYPE BreakPhaStart[4];// 总、A、B、C断相起始时刻
    COM_TIME_TYPE BreakPhaEnd[4];  // 总、A、B、C断相结束时刻    
    uint8     ActConst[3];         // 电表常数（有功）
    uint8     ReactConst[3];       // 电表常数（无功）    
    uint8     RealTime[6];         // 当前实时时间 6B 年月日时分秒
    BYTE4_INT32_TYPE LastEnergy[8][MAX_TARIFF_NUM+1];  // 上月正有、反有、正无、反无、1－4象限电量
    BYTE4_INT32_TYPE LastPhaEng[12][PhaMaxTariffNum+1];// 上月分相 正有、反有、正无、反无(A、B、C)
    BYTE4_INT32_TYPE LastCombActEng[MAX_TARIFF_NUM+1]; // 上一结算日组合有功电能量
    DEM_NUM_TYPE LastDemand[4][MAX_TARIFF_NUM+1];      // 上月正有、反有、正无、反无需量
    TERM_DEM_TIME_TYPE LastDemandTime[4][MAX_TARIFF_NUM+1]; // 上月正有、反有、正无、反无需量时间   

    uint16 MinPhaAVolt;                // A相分钟电压 
    uint16 MinPhaBVolt;                // B相分钟电压
    uint16 MinPhaCVolt;                // C相分钟电压    

    uint8    BatWorkTime[4];            // 电池工作时间
    uint8    ProgramNum[3];             // 编程总次数
    uint8    ProgramTime[6];            // 最近一次编程发生时刻
    uint8    ClrMetNum[3];              // 电表清零总次数
    uint8    ClrMetTime[6];             // 最近一次清零发生时刻
    uint8    ClrMaxDemNum[3];           // 需量清零总次数
    uint8    ClrMaxDemTime[6];          // 最近一次清零发生时刻
    uint8    ClrEventNum[3];            // 事件清零总次数
    uint8    ClrEventTime[6];           // 最近一次清零发生时刻
    uint8    AdjClockNum[3];            // 校时总次数
    uint8    AdjClockBeforeTime[6];     // 校时前时间
    uint8    AdjClockAfterTime[6];      // 校时后时间
    uint8    AdjClockTime[6];           // 最近一次校时发生时刻
    uint8    AdjClockTableNum[3];       // 时段表编程总次数
    uint8    AdjClockTableLastTime[6];  // 最近一次时段表编程时间
        
    METER_STATE_WORD_TYPE MetStusPara;  // 电表运行状态字

    TOPO_IDEN_RESULT TopoIdenResult;    // (自定义扩展)拓扑识别结果(浙江)

    BYTE4_INT32_TYPE CuPosActEng;       // 当前铜损有功总电能示值
    BYTE4_INT32_TYPE FePosActEng;       // 当前铁损有功总电能示值
    BYTE4_INT32_TYPE LastCuPosActEng;   // 上月铜损有功总电能示值
    BYTE4_INT32_TYPE LastFePosActEng;   // 上月铁损有功总电能示值

    BYTE4_INT32_TYPE TIME_ZERO_Eng[2][MAX_TARIFF_NUM+1];  // (第一、二时区冻结)正向有功-总,尖,峰,平,谷    

    uint8 powerType;                    // 接线方式
    char  HaltCnt[3];
    char  HaltInfo[120];
    
    DEM_NUM_TYPE CurrPhaDemand[3];
    TERM_DEM_TIME_TYPE CurrPhaDemandTime[3];
    
    int16  TemperatureValue;

    int PhaAReaPowerBasic;          // A相基波无功功率
    int PhaBReaPowerBasic;          // B相基波无功功率
    int PhaCReaPowerBasic;          // C相基波无功功率

    uint32 PhaAVoltHD;              // 高精度A相电压 
    uint32 PhaBVoltHD;              // 高精度B相电压
    uint32 PhaCVoltHD;              // 高精度C相电压
}__attribute__ ((packed)) TERM_SHARE_DATA_TYPE;

typedef struct     
{    
    uint8  VoltCurBit    :1; // 电压、电流
    uint8  PowBit        :1; // 功率
    uint8  AngFreqBit    :1; // 相角、频率
    uint8  EngBit        :1; // 能量
    uint8  StatBit       :1; // 状态
    uint8  CheckBit      :1; // 配置寄存器
    uint8  DemBit       :1; // 需量     
}__attribute__ ((packed)) MEA_INST_MARK_BIT_TYPE;

typedef union     
{
    uint8 Word;
    MEA_INST_MARK_BIT_TYPE BitSect; 
}__attribute__ ((packed)) MEA_INST_MARK_TYPE;

typedef struct 
{
    uint16 PhaAActPowFlag    : 1;    
    uint16 PhaBActPowFlag    : 1;
    uint16 PhaCActPowFlag    : 1;
    uint16 ActPowFlag        : 1;
    
    uint16 PhaAReaPowFlag    : 1;    
    uint16 PhaBReaPowFlag    : 1;    
    uint16 PhaCReaPowFlag    : 1;
    uint16 ReaPowFlag        : 1;
    
    uint16 PhaAAppPowFlag    : 1;    
    uint16 PhaBAppPowFlag    : 1;    
    uint16 PhaCAppPowFlag    : 1;
    uint16 AppPowFlag        : 1;
}__attribute__ ((packed)) POW_BIT_TYPE;           // 有无功功率反向标记位段 
typedef union   
{ 
    POW_BIT_TYPE  BitSect;
    uint16        Word;
}__attribute__ ((packed)) POW_DIR_TYPE;           // 有无功功率反向标记

typedef struct 
{
    uint16 PhaAVolCurrAngFlag    : 1;    
    uint16 PhaBVolCurrAngFlag    : 1;
    uint16 PhaCVolCurrAngFlag    : 1;
    
    uint16 PhaAVolAngFlag    : 1;    
    uint16 PhaBVolAngFlag    : 1;    
    uint16 PhaCVolAngFlag    : 1;
    
    uint16 PhaACurrAngFlag    : 1;    
    uint16 PhaBCurrAngFlag    : 1;    
    uint16 PhaCCurrAngFlag    : 1;
}__attribute__ ((packed)) ANGLE_BIT_TYPE;           // 电压/电流相角反向标记位段 
typedef union   
{ 
    ANGLE_BIT_TYPE  BitSect;
    uint16        Word;
}__attribute__ ((packed)) ANGLE_DIR_TYPE;           // 电压/电流相角反向标记


typedef struct 
{
    uint8     TotActEngFlag    : 1;
    uint8     PosActEngFlag    : 1;
    uint8     NegActEngFlag    : 1;
    uint8     PosReaEngFlag    : 1;
    uint8     NegReaEngFlag    : 1;
    uint8     PosAppEngFlag    : 1;
    uint8     NegAppEngFlag    : 1;
    uint8     ReserveBit        : 1;
}__attribute__ ((packed)) TOT_ENG_BIT_TYPE;       // 有无功能量反向标记位段 

typedef union   
{
    TOT_ENG_BIT_TYPE  BitSet;
    uint8             Word;
}__attribute__ ((packed)) TOT_ENG_DIR_TYPE;       // 有无功功率反向标记

typedef  struct 
{
    uint8   Data[4];                              // 数据标识
} DI_ITEM_TYPE;

typedef struct 
{
    int8  EngFlag;                               // 能量标记(符号标识)-1:带符号 
    int8  AddFlag;                               // 累加标志
    uint16 CurPlu;                                // 新采集脉冲
    uint16 PluConst;                              // 脉冲个数每0.01度
    DI_ITEM_TYPE      DIMark;                     // 数据标志
    uint16            *pResPlu;                   // 剩余脉冲首地址  
    BYTE4_INT32_TYPE  *pEnergy;                   // 待处理能量RAM首地址
} ENG_CAL_SAVE_INFOR_TYPE;// 能量的计算和保存信息结构体

typedef struct
{
    uint8     VoltConvFlag    : 1;                // 电压逆相序标志
    uint8     CurrConvFlag    : 1;                // 电流逆相序标志
    uint8     SIGFlag         : 1;                // SIG标志
    uint8     ReserveBit1     : 1;               
    uint8     PhaALatentFlag  : 1;                // A相潜动标志
    uint8     PhaBLatentFlag  : 1;                // B相潜动标志
    uint8     PhaCLatentFlag  : 1;                // C相潜动标志
    uint8     ReserveBit2     : 1;
}__attribute__ ((packed))MEA_STATUS_BIT_TYPE;     // 逆相序、潜动标记等

typedef union
{
    MEA_STATUS_BIT_TYPE BitSect;
    uint8 Word;
}__attribute__ ((packed))MEA_STATUS_TYPE;         // 逆相序、潜动标记等 

typedef struct
{
    uint8     PhaAVolt        : 1;
    uint8     PhaBVolt        : 1;
    uint8     PhaCVolt        : 1;
    uint8     PhaNVolt        : 1;
    uint8     PhaACurr        : 1;
    uint8     PhaBCurr        : 1;
    uint8     PhaCCurr        : 1;
    uint8     PhaNCurr        : 1;
}__attribute__ ((packed)) VOLT_CURR_BIT_TYPE;     // 电压、电流标记位段

typedef struct
{
    VOLT_CURR_BIT_TYPE  Bitsect;
    uint8               Word;
}__attribute__ ((packed)) VOLT_CURR_MARK_TYPE;

typedef struct
{   // E214: 12个REG  COM:36Byte        
    BYTE4_INT32_TYPE    AWATTHR;    // 4    24    32ZP    (0x0031) A相有功全波电能量
    BYTE4_INT32_TYPE    BWATTHR;    // 4    24    32ZP    (0x0032) B相有功全波电能量
    BYTE4_INT32_TYPE    CWATTHR;    // 4    24    32ZP    (0x0033) C相有功全波电能量
    BYTE4_INT32_TYPE    ZWATTHR;    // 4    24    32ZP    (0x0034) 合相有功全波电能量

    BYTE4_INT32_TYPE    AVARHR;     // 4    24    32ZP    (0x0035) A相无功全波电能量
    BYTE4_INT32_TYPE    BVARHR;     // 4    24    32ZP    (0x0036) B相无功全波电能量
    BYTE4_INT32_TYPE    CVARHR;     // 4    24    32ZP    (0x0037) C相无功全波电能量
    BYTE4_INT32_TYPE    ZVARHR;     // 4    24    32ZP    (0x0038) 合相无功全波电能量

    BYTE4_INT32_TYPE    AVAHR;      // 4    24    32ZP    (0x70) A相视在全波电能量
    BYTE4_INT32_TYPE    BVAHR;      // 4    24    32ZP    (0x71) B相视在全波电能量
    BYTE4_INT32_TYPE    CVAHR;      // 4    24    32ZP    (0x72) C相视在全波电能量
    BYTE4_INT32_TYPE    ZVAHR;      // 4    24    32ZP    (0x77) 合相视在全波电能量
}ENERG_PLUSE_TYPE;

// 计量芯片动态库与应用层交互结构体定义
typedef struct 
{   
    MEA_INST_MARK_TYPE    MeaInstMark;            // 交互标志，0x1F--数据采集计算完成
    
    // 实时数据对应标志
    POW_DIR_TYPE          CurPowDir;         // 有无功、视在功率方向标志 [MSB->LSB:无功总CBA有功总CBA]
    TOT_ENG_DIR_TYPE      TotEngDir;         // 有无功、视在能量方向标志
    MEA_STATUS_TYPE       MeaStatusMark;     // 逆相序、潜动标记等
    VOLT_CURR_MARK_TYPE   VoltCurrMark;      // 电压电流非零标志

    // 测试模式标志，功率校表小电流时，脉冲常数放大8倍
    uint8 TestModeFlag;
    
    // 电压格式:  XXXX.XXXX V   4位小数    无符号
    uint32 PhaAVolt;                              // A相电压
    uint32 PhaBVolt;                              // B相电压
    uint32 PhaCVolt;                              // C相电压
    uint32 PhaMVolt;                              // 电压矢量和
    
    // 电流格式:  XXXX.XXXX A   4位小数 
    int PhaACurr;                            // A相电流  有符号(有功功率方向)
    int PhaBCurr;                            // B相电流  有符号(有功功率方向)
    int PhaCCurr;                            // C相电流  有符号(有功功率方向)
    uint32 PhaMCurr;                         // 零序电流      无符号
    uint32 PhaGCurr;                         // 中性线电流    无符号
    int ResidCurr;                           // 剩余电流      有符号(有功功率方向)
    int AverageCurr;                         // 三相平均电流  有符号(有功功率方向)

    // 功率格式:  XX.XXXXXX kW  6位小数  有符号(有功功率方向)；先总后分，与07电表规约对应
    int ActPower;                            // 总有功功率
    int PhaAActPower;                        // A相有功功率
    int PhaBActPower;                        // B相有功功率
    int PhaCActPower;                        // C相有功功率
    int ReaPower;                            // 总无功功率
    int PhaAReaPower;                        // A相无功功率
    int PhaBReaPower;                        // B相无功功率
    int PhaCReaPower;                        // C相无功功率
    int AppPower;                            // 瞬时视在功率
    int PhaAAppPower;                        // A相视在功率
    int PhaBAppPower;                        // B相视在功率
    int PhaCAppPower;                        // C相视在功率    

    // 功率因数格式:  XXX.XXXXX   5位小数    有符号(有功功率方向)
    int PowFactor;                           // 总功率因数
    int PhaAPowFactor;                       // A相功率因数
    int PhaBPowFactor;                       // B相功率因数
    int PhaCPowFactor;                       // C相功率因数
    

    // 电网频率格式:  XXXX.XXXX Hz  4位小数  无符号
    uint32 ElectNetFrequ;                    // 电网频率XX.XXXX

    // 温度格式:  XXX.X Hz 摄氏度  1位小数  有符号(最高位符号)
    int16 MeaICTempNum;                     // 计量芯片自测温度

    // 三相电压、三相电流相角，数据格式(XXX.X)
    uint16 PhaAVoltAngle;                    // A相电压相角(XXX.X)
    uint16 PhaBVoltAngle;                    // B相电压相角
    uint16 PhaCVoltAngle;                    // C相电压相角
    uint16 PhaACurrAngle;                    // A相电流相角
    uint16 PhaBCurrAngle;                    // B相电流相角
    uint16 PhaCCurrAngle;                    // C相电流相角

    // 三相电压与电流夹角，数据格式(XXX.X) -- 07电表规约 02 07 01/02/03 00
    uint16 PhaACurrVoltAngle;                // A相压流夹角
    uint16 PhaBCurrVoltAngle;                // B相压流夹角
    uint16 PhaCCurrVoltAngle;                // C相压流夹角

    // A,B,C,合相无功象限号
    uint8 PhaACurrQuadNum;                   // A相当前象限号
    uint8 PhaBCurrQuadNum;                   // B相当前象限号
    uint8 PhaCCurrQuadNum;                   // C相当前象限号
    uint8 CurrQuadNum;                       // 当前象限号 1：1象限 2：2象限 3：3象限 4：4象限

    ENERG_PLUSE_TYPE      EnergPluse;        // 能量，原始数据，能量在应用层计算
}MEA_REAL_DATA_COM_TYPE;

typedef struct     
{
    uint8  FreshMark            :1; // 刷新所有
    uint8  ActReaEng            :1; // 能量
    uint8  PhaActReaEng         :1;
    uint8  ActReaDem            :1; // 需量
    uint8  BreakPha             :1;
    uint8  ProgRecd             :1; 
    uint8  MonFreze             :1; 
    uint8  TimeZoneSwitch       :1;  
}__attribute__ ((packed)) SHARED_MEM_MARK_BIT_TYPE;

typedef union     
{ 
    uint8   Word;
    SHARED_MEM_MARK_BIT_TYPE Bitsect; 
}__attribute__ ((packed)) SHARED_MEM_MARK_BYTE;


typedef struct 
{    
    uint8 PhaAAppCurrentFlag;
    uint8 PhaBAppCurrentFlag;
    uint8 PhaCAppCurrentFlag;
    uint8 ZeroLineCurrentFlag;  //零线电流
    uint8 ZeroCurrentFlag;      //零序电流
}CURRENT_SIGN_TYPE;//电流正负符号1负0正

typedef struct 
{
    uint8 AppPfFlag;
    uint8 PhaAAppPfFlag;
    uint8 PhaBAppPfFlag;
    uint8 PhaCAppPfFlag;
}POWER_FACTOR_TYPE;//功率因数正负符号1负0正

typedef struct 
{
    uint8 PhaAVoltHarmonicFlag[MAX_HARMONIC_NUM];    // A相电压谐波含有率（总及2…n次）
    uint8 PhaBVoltHarmonicFlag[MAX_HARMONIC_NUM];    // B相电压谐波含有率（总及2…n次）
    uint8 PhaCVoltHarmonicFlag[MAX_HARMONIC_NUM];    // C相电压谐波含有率（总及2…n次）
    uint8 PhaACurrHarmonicFlag[MAX_HARMONIC_NUM];    // A相电流谐波含有率（总及2…n次）
    uint8 PhaBCurrHarmonicFlag[MAX_HARMONIC_NUM];    // B相电流谐波含有率（总及2…n次）
    uint8 PhaCCurrHarmonicFlag[MAX_HARMONIC_NUM];    // C相电流谐波含有率（总及2…n次）
    
    uint8 PhaAVoltHarmonicValueFlag[MAX_HARMONIC_NUM];    // A相电压谐波含量（总及2…n次）
    uint8 PhaBVoltHarmonicValueFlag[MAX_HARMONIC_NUM];    // B相电压谐波含量（总及2…n次）
    uint8 PhaCVoltHarmonicValueFlag[MAX_HARMONIC_NUM];    // C相电压谐波含量（总及2…n次）
    uint8 PhaACurrHarmonicValueFlag[MAX_HARMONIC_NUM];    // A相电流谐波含量（总及2…n次）
    uint8 PhaBCurrHarmonicValueFlag[MAX_HARMONIC_NUM];    // B相电流谐波含量（总及2…n次）
    uint8 PhaCCurrHarmonicValueFlag[MAX_HARMONIC_NUM];    // C相电流谐波含量（总及2…n次）
}HARMONIC_RATE_VALUE_TYPE;//谐波含量正负符号1负0正

typedef struct 
{    
    uint8 SignalS1Flag;//信号强度S1 正负符号1负0正
    uint8 SignalS2Flag;//信号强度S2 正负符号1负0正
    uint8 NoiseFlag;//背景噪声 正负符号1负0正
}SIGNAL_IDEN_TYPE;//(自定义扩展)浙江拓扑识别结果

typedef struct 
{    
    uint16 PluNumPer01Kwh;                        // 每0.01度脉冲数
    uint8  TestModeFlag;                          // 测试模式标记
    POW_DIR_TYPE CurPowDir;                       // 有无功、视在功率方向标志 [MSB->LSB:无功总CBA有功总CBA]
    TOT_ENG_DIR_TYPE TotEngDir;                   // 有无功、视在能量方向标志
    MEA_STATUS_TYPE    MeaStatusMark;             // 逆相序、潜动标记等
    VOLT_CURR_MARK_TYPE    VoltCurrMark;          // 电压电流非零标志

    uint8 PhaACurrQuadNum;                        // A相当前象限号
    uint8 PhaBCurrQuadNum;                        // B相当前象限号
    uint8 PhaCCurrQuadNum;                        // C相当前象限号
    uint8 CurrQuadNum;                            // 当前象限号 1：1象限 2：2象限 3：3象限 4：4象限 

    uint8    TotActMark;
    uint8    IncTotActMark;                       // 计量总有功标识 0x55:计量
    uint16   IncTotActEng;                        // 该次电量增量

    uint8 FirstLowVoltMark;                       // 低电压下电流采样标识
    PHASE_ACT_REA_APP_ENERGY_TYPE PhaEng[3];      // 分相能量BCD码

    ANGLE_DIR_TYPE AngleDir;                       // 相角方向
    CURRENT_SIGN_TYPE CurrentSignType;  //电流方向1负0正
    POWER_FACTOR_TYPE PowerFactorType;  //功率因数方向1负0正
    uint8 CmbEnergyType[MAX_TARIFF_NUM+1];  //组合电能方向1负0正
    uint8 CmbEnergyTypeHD[MAX_TARIFF_NUM+1];  //组合电能(高精度)方向1负0正
    
    uint8 PosReaEnergyType[MAX_TARIFF_NUM+1];  //正向无功电能方向1负0正
    uint8 NegReaEnergyType[MAX_TARIFF_NUM+1];  //正向无功电能方向1负0正
    uint8 PhaAPosReaEngType[PhaMaxTariffNum+1];    // A相 正无 方向1负0正
    uint8 PhaANegReaEngType[PhaMaxTariffNum+1];    // A相 反无 方向1负0正
    uint8 PhaBPosReaEngType[PhaMaxTariffNum+1];    // B相 正无 方向1负0正
    uint8 PhaBNegReaEngType[PhaMaxTariffNum+1];    // B相 反无 方向1负0正
    uint8 PhaCPosReaEngType[PhaMaxTariffNum+1];    // C相 正无 方向1负0正
    uint8 PhaCNegReaEngType[PhaMaxTariffNum+1];    // C相 反无 方向1负0正

    uint8 PosReaEnergyTypeHD[MAX_TARIFF_NUM+1];  //正向无功电能(高精度)方向1负0正
    uint8 NegReaEnergyTypeHD[MAX_TARIFF_NUM+1];  //正向无功电能(高精度)方向1负0正
    uint8 PhaAPosReaEngTypeHD[PhaMaxTariffNum+1];    // A相(高精度) 正无 方向1负0正
    uint8 PhaANegReaEngTypeHD[PhaMaxTariffNum+1];    // A相(高精度) 反无 方向1负0正
    uint8 PhaBPosReaEngTypeHD[PhaMaxTariffNum+1];    // B相(高精度) 正无 方向1负0正
    uint8 PhaBNegReaEngTypeHD[PhaMaxTariffNum+1];    // B相(高精度) 反无 方向1负0正
    uint8 PhaCPosReaEngTypeHD[PhaMaxTariffNum+1];    // C相(高精度) 正无 方向1负0正
    uint8 PhaCNegReaEngTypeHD[PhaMaxTariffNum+1];    // C相(高精度) 反无 方向1负0正
    
    HARMONIC_RATE_VALUE_TYPE HarmonicRateValueType;  //谐波含有率/含量方向1负0正

    SIGNAL_IDEN_TYPE SignalIdenType;  //(自定义扩展)浙江拓扑识别结果 1负0正
}PRIVTE_MEASURE_TYPE; 






/*******************************************************************************
* 结构体
*******************************************************************************/
typedef struct
{
    uint8 ms100          :1;            // 过100ms标志 
    uint8 ms500          :1;            // 过500ms标志 
    uint8 Second         :1;            // 过秒标志
    uint8 Minute         :1;            // 过分标志
    uint8 Hour           :1;            // 过时标志
    uint8 Day            :1;            // 过日标志
    uint8 Month          :1;            // 过月标志
    uint8 ReserveBit8    :1;            // 保留
}PAST_TIME_FLAG_BIT_TYPE;

typedef union   
{   
    PAST_TIME_FLAG_BIT_TYPE   BitSect;
    uint8                     Word;
}PAST_TIME_FLAG_TYPE;                   // 过时间标志

typedef struct
{
    YYMMDDWWhhnnss_TYPE       InfoCurTime;        // 当前时间
    YYMMDDWWhhnnss_TYPE       InfoLastTime;       // 上次比较时间
    PAST_TIME_FLAG_TYPE       InfoTimeFlag;       // 过分、过秒标志
}TIME_INFO;  // 时间信息，包括当前时间、过分过秒标志等等

//事件公共信息
typedef struct
{
    OOP_OAD_U           eventOAD;                       //事件标识
    uint8               checkflag;                      //有效标识
    uint8               reportflag;                     //上报标识
    uint32              recordnum;                      //记录序号(某种事件存储的数量)
    uint16              eventNum;                       //事件记录数
    uint32              EventStoragNumRecAddr;          //存储序号 起始地址
    uint32              EventStoragTmRecAddr;           //发生时间 起始地址
    uint32              EventFlagAddr;                  //事件发生标志 起始地址
    uint16              EventFlagLen;                   //事件发生标志 长度
    OOP_RCSD_T          eventRCSD;                      //事件上报RCSD
    OOP_DATETIME_S_T    starttime;      //发生时间
    OOP_DATETIME_S_T    endtime;        //结束时间
    uint32              storagenum;     //存储序号(写记录型数据的出参, 代表数据库中存储的行号)
    uint32              ERFlg;          //状态标记，标记事件是否发生
    EVTSTATE            eventState;                     //事件当前值
    EVTTIME             eventTime;                      //事件时间状态记录表
    uint8               bType;                          //事件类型，终端 0 涉及到电能表1
    time_t              tStartTime;                     //发生时间
    uint8               EffectState;                    //磁场干扰状态
}EVENT_COMM_INFO;

typedef struct
{
    uint8 nState;//0已恢复 1已发生
    time_t tOccurTime;//发生时间
    time_t tRecovTime;//恢复时间
}SINGLE_EVENT_INFO;  //单个事件状态


typedef struct
{
    SINGLE_EVENT_INFO stEvent300F;//交采电压逆向序事件
}TOTAL_EVENT_INFO;  //所有事件状态

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    uint8             ishappen;         //状态标记，标记事件是否发生
    uint8             nDelay;           //判断延时
    uint8             initflag;         //参数初始化标记，标识参数是否初始化
    OOP_WORD3V_T      valTemp;          //电压
    OOP_METWORD_T     meterRsw;         //电表运行状态字7
}EVENT_300F_INFO;
















#ifdef __cplusplus
}
#endif

#endif // __AC_METER_REDEF_H__
