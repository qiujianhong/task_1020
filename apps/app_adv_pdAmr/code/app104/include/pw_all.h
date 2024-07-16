/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2020-01-03
*********************************************************************
*/


#ifndef __PW_ALL_H__
#define __PW_ALL_H__


#include "baseCommon.h"
#include "dataCommon.h"
#include "dataType.h"
#include "errCode.h"
#include "macro.h"

#include "storage.h"
#include "db_api.h"
#include "task_id.h"

#include "app104_ipc.h"
#include "app104_base.h"
#include "app104_xml.h"
#include "comm_port.h"

#include "security.h"
#include "framecomm.h"
#include "task_msg.h"
#include "msg_id.h"


#include "../../msg/include/db_update.h"
#ifdef AREA_ZHEJIANG
#include "EventIO_zhejiang.h"
#else
#include "../../event/EventIO.h"
#endif
#include "monit_stat.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

//信号类型
#define TEST_VER_FLAG (1) //是否测试版本

/*******************************************************************************
* 声明外部函数
*******************************************************************************/
extern uint8 advPd_get_rmt(RST_RMT_T *pRmt);
extern void yx_calculate_sta_set(uint32 subtype, uint8 subindex, uint8 value);
extern int para_query_float32_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, float32 *fOut);
extern int para_query_int64_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, int64 *nOut);
extern int para_query_array_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, uint8 *pOut);
extern int yx_event_signal_update_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nAddr, uint8 nNewState);

/*******************************************************************************
* 需要统计/计算的数据定义
*******************************************************************************/

//信号类型
#define BASE_CLASS_YC        	    0x40010000				//遥测类型
#define BASE_CLASS_YX        	    0x00010000				//遥信类型
#define BASE_CLASS_YK        	    0x60010000				//遥控类型
#define BASE_CLASS_EN        	    0x64010000				//电量类型
#define BASE_CLASS_FAULT        	0x01010000				//故障类型
#define BASE_CLASS_PARA        	    0x80010000				//参数类型

//遥测统计/计算量
#define CALC_YC_A_VOLT_DVALUE        	    0x40010001				//A相电压偏差
#define CALC_YC_B_VOLT_DVALUE        	    0x40010002				//B相电压偏差
#define CALC_YC_C_VOLT_DVALUE        	    0x40010003				//C相电压偏差
#define CALC_YC_FR_DVALUE        	        0x40010004				//频率偏差
#define CALC_YC_PD_LOAD_RATE        	    0x40010005				//配变负载率

//遥信统计/计算量
#define CALC_YX_REAL_SIGNAL        	    0x00010001				//两个遥信脉冲模块共计8路硬遥信
#define CALC_YX_VOLT_DOWN_L        	    0x00010002				//总进线电压越下限
#define CALC_YX_VOLT_OVER_H        	    0x00010003				//总进线电压越上限
#define CALC_YX_PHASE_LOSS        	    0x00010004				//断相
#define CALC_YX_VOLT_LOSS        	    0x00010005				//失压
#define CALC_YX_POWER_FAILURE        	0x00010006				//停电
#define CALC_YX_CURR_IMBALANCE        	0x00010007				//变压器电流三相不平衡
#define CALC_YX_VOLT_IMBALANCE        	0x00010008				//变压器电压三相不平衡
#define CALC_YX_TR_OVERLOAD        	    0x00010009				//变压器过载
#define CALC_YX_TR_HEAVYLOAD        	0x0001000A				//变压器重载
#define CALC_YX_S_OVER_LIMIT        	0x0001000B				//视在功率越限

//电量统计/计算量
#define CALC_EN_15MIN_FREEZE        	0x64010001				//15分钟冻结数据
#define CALC_EN_DAY_FREEZE        	    0x64010002				//日冻结数据

//固有参数处理
#define CALC_PARA_TMN_TYPE        	0x80010001				//终端类型
#define CALC_PARA_TMN_OPSYS        	0x80010002				//终端操作系统
#define CALC_PARA_TMN_MAKER        	0x80010003				//终端制造商
#define CALC_PARA_TMN_HARD        	0x80010004				//终端硬件版本
#define CALC_PARA_TMN_SOFT        	0x80010005				//终端软件版本
#define CALC_PARA_TMN_CHECK        	0x80010006				//终端软件版本校验码
#define CALC_PARA_TMN_PROTOCOL      0x80010007				//终端通信规约类型
#define CALC_PARA_TMN_MODEL        	0x80010008				//终端出厂型号
#define CALC_PARA_TMN_ID        	0x80010009				//终端ID号
#define CALC_PARA_TMN_MAC        	0x8001000A				//终端网卡MAC地址

// 结构体的声明    
typedef enum
{   
    E_OOP_ENERGY_T=0,  
	E_OOP_ENERGYS_T,
	E_OOP_HENERGY_T,
    E_OOP_HENERGYS_T,    
    E_OOP_DEMAND_T,
    E_OOP_DEMANDS_T,
    E_OOP_VOLTQR_T,
    E_OOP_DWORD3V_T,
    E_OOP_WORD3V_T,
    E_OOP_INT3V_T,
    E_OOP_WORD4V_T,
    E_OOP_LONG4V_T,
    E_OOP_INT4V_T,
    E_OOP_METWORD_T,
    E_DATETIME_S,
    E_FRZ_DATETIME_S,
    E_REAL_DATETIME_S,
    E_OOP_WALLET_T,
    E_DOUBLE_LONG_UNSIGNED,
    E_DOUBLE_LONG,
    E_LONG_UNSIGNED,
    E_LONG,
    E_OOP_FROZENSETS_T,
    E_OOP_HARMONIC_T,
    E_ENUM,
    E_OOP_SYNCCLOCK_T,
    E_OOP_OCTETVAR16_T,
    E_OOP_GEOGADD_T,
    E_OOP_GRPADDR_T,
    E_OOP_PERIOD_T,
    E_UNSIGNED,
    E_OOP_PUBHOLIDAY_T,
    E_OOP_VISIBLESTR8_T,
    E_OOP_TIMEZONE_T,
    E_OOP_DAYPERIOD_T,
    E_OOP_RATETARIFF_T,
    E_OOP_STEPPRICE_T,
    E_OOP_LIMITCHARGE_T,
    E_OOP_OTHERLIMIT_T,
    E_OOP_POWERLIMIT_T,
    E_OOP_OTHERPOWERLIMIT_T,
    E_OOP_VISIBLESTR16_T,
    E_DAYPERIOD,
    E_TIMEZONE,
    E_OOP_PAYDAY_T,
    E_BIT_STRING,
    //死区参数0x49010200
	E_OOP_DEAD_ZONE_T,
	//PT参数0x49020200
	E_OOP_PT_T,
	//CT参数0x49030200
	E_OOP_CT_T,
	//低电压报警0x49040200
	E_OOP_LOW_U_T,
	//过电压报警0x49050200
	E_OOP_OVER_U_T,
	//重载报警0x49060200
	E_OOP_HEAVY_LOAD_T,
	//过载报警0x49070200
	E_OOP_OVER_LOAD_T,
	//开入量采集防抖动时间0x49080200
	E_OOP_YX_GET_TIME_T,
	//分闸输出脉冲保持时间0x49090200
	E_OOP_PULSE_TIME_T,
	//蓄电池自动活化周期0x490A0200
	E_OOP_AUTO_ACT_CYCLE_T,
	//电池电压低报警阈值0x490B0200
	E_OOP_BATT_LOW_U_T,
	//短路故障报警0x490C0200
	E_OOP_SHORT_FAULT_T,
	//录波电场下降比例0x490E0200
	E_OOP_RECORD_DROP_T,
	//永久故障0x490F0200
	E_OOP_RESET_TPYE_T,
	//永久故障0x490F0300
	E_OOP_RESET_TIME_T,
	//负荷电流超限阈值0x49100200
	E_OOP_LOAD_I_LIMIT_T,
	//过电流报警阈值0x49110200
	E_OOP_OVER_I_LIMIT_T,
	//相线路有电阈值0x49120200
	E_OOP_PHASE_LINE_POWER_T,
	//故障指示灯自动复归0x49130200
	E_OOP_FAULT_TUBE_RESET_T,
	//故障遥信保持时间0x49140200
	E_OOP_FAULT_YX_KEEP_T,
	//过流参数（过流I段）0x49160200
	//过流参数（过流II段）0x49160300
	//过流参数（过流III段）0x49160400
	//过流参数（零序过流）0x49160500
	E_OOP_OVER_I_PARA_T,
    E_NULL=0xFF,

}DATASTRUCT; 

//645报文 与 面向对象 对应关系
typedef struct{
	OOP_OAD_U  Oad;          //OAD
	int8   DesScaler;        //0AD对应的换算单位
	uint32  DI07;	         //07协议数据标识
	uint8   DataLen;	     //645 每个子元素的长度
	uint8   TotalDataLen;	 //645 总长度
	uint16  DI97;			 //97协议数据标识
    DATASTRUCT  CovertSruct; //要转成的结构体类型
    int8   Scaler;           //645小数点位数
}Map645ToOop;

#ifdef __cplusplus
}
#endif

#endif // __PW_ALL_H__