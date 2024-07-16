/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2020-01-03
*********************************************************************
*/

#include "pw_protocol.h"
#include "pw_debug.h"
#include "pw_common.h"

#ifdef AREA_ZHEJIANG
#include "../area/zhejiang/pw_protocol_zhj.h"
#include "../area/zhejiang/pw_common_zhj.h"
#endif

/****************************************************************/
//信号点表配置区
/***************************************************************/

//遥信点表实际配置个数
uint16 g_nConfigYxNum = 8;

//遥测点表实际配置个数
uint16 g_nConfigYcNum = 21;

//遥控点表实际配置个数
uint16 g_nConfigYkNum = 8;

//电量点表实际配置个数
uint16 g_nConfigEnNum = 24;

//故障事件点表实际配置个数
uint16 g_nConfigFaultNum = 27;

//参数点表实际配置个数
uint16 g_nConfigParaNum = 29;

//遥信点表配置
YX_DATA_CONFIG g_aYxDataConfig[MAX_YX_NUM];	
YX_DATA_SEND   g_aYxDataSend[MAX_YX_NUM];  //遥信信号存储区

//遥测点表配置
YC_DATA_CONFIG g_aYcDataConfig[MAX_YC_NUM];
YC_DATA_SEND   g_aYcDataSend[MAX_YC_NUM];  //遥测数据存储区

//电量点表配置
EN_DATA_CONFIG g_aEnergyDataConfig[MAX_ENERGY_NUM];
EN_DATA_SEND   g_aEnergyDataSend[MAX_ENERGY_NUM];  //电量数据存储区

//遥控点表配置
YK_DATA_CONFIG g_aYkDataConfig[MAX_YK_NUM];
YK_DATA_SEND   g_aYkDataSend[MAX_YK_NUM];  //遥控数据存储区

//定值区参数配置
FIXED_AREA_IDEN_T g_aFixedValueArea[MAX_FIXED_AREA_NUM] = 
{
{0, {0}, 0xaa, {0}},
{1, {0}, 0xaa, {0}},
};

/*主站下发的tag格式类型，具体如下
名称		类型	标记（TAG）  长度(LENGTH)	 值（VALUE） 
布尔		Boolean 	1			1			1:true,0:false 
小整形		Tiny		43			 1			-128…127 
无符号小整形 UTiny	32	1			 0…255 
短整形		Short		33			2			 -32768…32767 
无符号短整形 UShort 	45			2			0…65535 
整形		Int 		2			4			 -2147483648…2147483648 
无符号整形	Uint		35			4			 0…4294967295 
长整形		Long		36			8			 -2^64…2^64 
无符号长整形 Ulong		37			8			 0…2^128-1 
单精度浮点	Float		38			4			 -2^128…2^128 
双精度浮点	Double	39			8			 -2^1024…2^1024 
八位位串类型 OcterString  4 			可变		OcterString 
字符串类型	String		4 	        可变    一个或者多个ASCII组成,最长64个字节，以‘\0’结尾	
*/

//定值参数配置
PARA_SET_VALUE_T g_aParaSetValue[MAX_FIXED_AREA_NUM][PARA_MAX_NUM];
PARA_SET_VALUE_T g_aParaValueInit[PARA_MAX_NUM];

//故障事件配置（目前终端只支持交采事件，电表事件不支持）
FAULT_REPORT_CONFIG g_aFaultDataConfig[FAULT_MAX_NUM];
FAULT_REPORT_SEND g_aFaultDataSend[FAULT_MAX_NUM];  //故障数据存储区

//面向对象定值参数实时数据映射表
const Map104ToOop g_aMap104ToOopRT[] = 
{
	{{0x49010200}, 0, 0xFFFFFFFF, 1, 18, 0xFFFF, E_OOP_DEAD_ZONE_T, 	 0,	 {0x8020, 0x8021, 0x8022, 0x8023, 0x8024}, {-3, -2, -2, -3, -3}},  //死区参数
	{{0x49020200}, 0, 0xFFFFFFFF, 2, 4,  0xFFFF, E_OOP_PT_T,        	 0,	 {0x8026, 0x8027},						   {-1, -1}},  //PT参数
	{{0x49030200}, 0, 0xFFFFFFFF, 4, 16, 0xFFFF, E_OOP_CT_T, 	    	 0,	 {0x8040, 0x8041, 0x8042, 0x8043}, 		   {-3, -3, -3, -3}},  //CT参数
	{{0x49040200}, 0, 0xFFFFFFFF, 1, 6,  0xFFFF, E_OOP_LOW_U_T, 		 0,	 {0x8028, 0x8029}, 						   {-2, 0}},  //低电压报警
	{{0x49050200}, 0, 0xFFFFFFFF, 1, 6,  0xFFFF, E_OOP_OVER_U_T, 		 0,	 {0x802a, 0x802b}, 						   {-2, 0}},  //过电压报警
	{{0x49060200}, 0, 0xFFFFFFFF, 1, 6,  0xFFFF, E_OOP_HEAVY_LOAD_T, 	 0,	 {0x802c, 0x802d}, 						   {-3, 0}},  //重载报警
	{{0x49070200}, 0, 0xFFFFFFFF, 1, 6,  0xFFFF, E_OOP_OVER_LOAD_T, 	 0,	 {0x802e, 0x802f}, 						   {-3, 0}},  //过载报警
	{{0x49080200}, 0, 0xFFFFFFFF, 4, 4,  0xFFFF, E_OOP_YX_GET_TIME_T, 	 0,  {0x8030}, 								   {-2}},  //开入量采集防抖时间
	{{0x49090200}, 0, 0xFFFFFFFF, 4, 8,  0xFFFF, E_OOP_PULSE_TIME_T, 	 0,	 {0x8031, 0x8032}, 						   {-2, -2}},  //分闸输出脉冲保持时间
	{{0x490A0200}, 0, 0xFFFFFFFF, 4, 8,  0xFFFF, E_OOP_AUTO_ACT_CYCLE_T, 0,  {0x8033, 0x8034}, {0, 0}},  //蓄电池自动活化周期

	{{0x490B0200}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_BATT_LOW_U_T, 0, {0}, {0}},  //电池电压低报警阈值
	{{0x490C0200}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_SHORT_FAULT_T, 0, {0}, {0}},  //短路故障报警
	{{0x490E0200}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_RECORD_DROP_T, 0, {0}, {0}},  //录波电场下降比例
	{{0x490F0200}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_RESET_TPYE_T, 0, {0}, {0}},  //永久故障
	{{0x490F0300}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_RESET_TIME_T, 0, {0}, {0}},  //永久故障
	{{0x49100200}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_LOAD_I_LIMIT_T, 0, {0}, {0}},  //负荷电流超限阈值
	{{0x49110200}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_OVER_I_LIMIT_T, 0, {0}, {0}},  //过电流报警阈值
	{{0x49120200}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_PHASE_LINE_POWER_T, 0, {0}, {0}},  //相线路有电阈值
	{{0x49130200}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_FAULT_TUBE_RESET_T, 0, {0}, {0}},  //故障指示灯自动复归
	{{0x49140200}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_FAULT_YX_KEEP_T, 0, {0}, {0}},  //故障遥信保持时间
	{{0x49160200}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_OVER_I_PARA_T, 0, {0}, {0}},  //过流Ⅰ段
	{{0x49160300}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_OVER_I_PARA_T, 0, {0}, {0}},  //过流Ⅱ段
	{{0x49160400}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_OVER_I_PARA_T, 0, {0}, {0}},  //过流Ⅲ段
	{{0x49160500}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_OVER_I_PARA_T, 0, {0}, {0}},  //零序过流
	{{0xFFFFFFFF}, 0, 0xFFFFFFFF, 1, 1, 0xFFFF, E_OOP_OVER_I_PARA_T, 0, {0}, {0}},  //备用
	
};

const Map645ToOop g_aMap645ToOopRT[] = //实时数据映射表
{
    {{0x00000200},  0,  0x0000FF00,4,20,0xFFFF,E_OOP_ENERGYS_T,   0, },   // (当前)组合有功电能块
    {{0x00000201}, -2,  0x00000000,4,4, 0xFFFF,E_OOP_ENERGYS_T,  -2, },   // 4字节, XXXXXX.XX(当前)组合有功总电能
    {{0x00000202}, -2,  0x00000100,4,4, 0xFFFF,E_OOP_ENERGYS_T,  -2, },	 // 4字节, XXXXXX.XX(当前)费率1组合有功电能
    {{0x00000203}, -2,  0x00000200,4,4, 0xFFFF,E_OOP_ENERGYS_T,  -2, },	 // 4字节, XXXXXX.XX(当前)费率2组合有功电能
    {{0x00000204}, -2,  0x00000300,4,4, 0xFFFF,E_OOP_ENERGYS_T,  -2, },	 // 4字节, XXXXXX.XX(当前)费率3组合有功电能
    {{0x00000205}, -2,  0x00000400,4,4, 0xFFFF,E_OOP_ENERGYS_T,  -2, },	 // 4字节, XXXXXX.XX(当前)费率4组合有功电能

    // (当前)正向有功总电能
    {{0x00100200}, -2,  0x0001FF00,4,20,0x901F,E_OOP_ENERGY_T, -2, },	 // (当前)正向有功电能块
    {{0x00100201}, -2,  0x00010000,4,4, 0x9010,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00100202}, -2,  0x00010100,4,4, 0x9011,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)费率1正向有功电能
    {{0x00100203}, -2,  0x00010200,4,4, 0x9012,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)费率2正向有功电能
    {{0x00100204}, -2,  0x00010300,4,4, 0x9013,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)费率3正向有功电能
    {{0x00100205}, -2,  0x00010400,4,4, 0x9014,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)费率4正向有功电能

    // (当前)A相正向有功总电能		
    {{0x00110200}, -2,  0x00150000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)A相正向有功总电能
    {{0x00110201}, -2,  0x00150000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)A相正向有功总电能
    // (当前)B相正向有功总电能	
    {{0x00120200}, -2,  0x00290000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)B相正向有功总电能
    {{0x00120201}, -2,  0x00290000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)B相正向有功总电能
    // (当前)C相正向有功总电能			
    {{0x00130200}, -2,  0x003D0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)C相正向有功总电能
    {{0x00130201}, -2,  0x003D0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)C相正向有功总电能

    // (当前)A相反向有功总电能			
    {{0x00210200}, -2,  0x00160000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)A相反向有功总电能
    {{0x00210201}, -2,  0x00160000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)A相反向有功总电能
    // (当前)B相反向有功总电能			
    {{0x00220200}, -2,  0x002A0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)B相反向有功总电能
    {{0x00220201}, -2,  0x002A0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)B相反向有功总电能
    // (当前)C相反向有功总电能			
    {{0x00230200}, -2,  0x003E0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)C相反向有功总电能
    {{0x00230201}, -2,  0x003E0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)C相反向有功总电能
    
	{{0x00310200}, -2,  0x00170000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)A 相组合无功 1 电能
    {{0x00310201}, -2,  0x00170000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)A 相组合无功 1 电能

	{{0x00320200}, -2,  0x002B0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)B 相组合无功 1 电能
    {{0x00320201}, -2,  0x002B0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)B 相组合无功 1 电能

	{{0x00330200}, -2,  0x003F0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)C 相组合无功 1 电能
    {{0x00330201}, -2,  0x003F0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)C 相组合无功 1 电能

	{{0x00410200}, -2,  0x00180000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)A 相组合无功 2 电能
    {{0x00410201}, -2,  0x00180000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)A 相组合无功 2 电能

	{{0x00420200}, -2,  0x002C0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)B 相组合无功 2 电能
    {{0x00420201}, -2,  0x002C0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)B 相组合无功 2 电能

	{{0x00430200}, -2,  0x00400000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)C 相组合无功 2 电能
    {{0x00430201}, -2,  0x00400000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)C 相组合无功 2 电能

    //(当前)反向有功总电能
    {{0x00200200}, -2,  0x0002FF00,4,20,0x902F,E_OOP_ENERGY_T, -2, },	 // (当前)反向有功电能块
    {{0x00200201}, -2,  0x00020000,4,4, 0x9020,E_OOP_ENERGY_T, -2, },	 // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00200202}, -2,  0x00020100,4,4, 0x9021,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)费率1反向有功电能
    {{0x00200203}, -2,  0x00020200,4,4, 0x9022,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)费率2反向有功电能
    {{0x00200204}, -2,  0x00020300,4,4, 0x9023,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)费率3反向有功电能
    {{0x00200205}, -2,  0x00020400,4,4, 0x9024,E_OOP_ENERGY_T,  -2, },	 // 4字节, XXXXXX.XX(当前)费率4反向有功电能

	//(当前)正向无功总电能(+RL,+RC)
    {{0x00300200}, -2,  0x0003FF00,4,20,0x911F,E_OOP_ENERGY_T, -2, },    // (当前)正向无功电能块(+RL,+RC)
    {{0x00300201}, -2,  0x00030000,4,4, 0x9110,E_OOP_ENERGY_T,  -2, },    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00300202}, -2,	0x00030100,4,4, 0x9111,E_OOP_ENERGY_T,  -2, },    // 4字节,XXXXXX.XX (当前)费率1正向无功电能
    {{0x00300203}, -2,	0x00030200,4,4, 0x9112,E_OOP_ENERGY_T,  -2, },    // 4字节,XXXXXX.XX (当前)费率2正向无功电能
    {{0x00300204}, -2,	0x00030300,4,4, 0x9113,E_OOP_ENERGY_T,  -2, },    // 4字节,XXXXXX.XX (当前)费率3正向无功电能
    {{0x00300205}, -2,	0x00030400,4,4, 0x9114,E_OOP_ENERGY_T,  -2, },    // 4字节,XXXXXX.XX (当前)费率4正向无功电能

    //(当前)反向无功总电能
    {{0x00400200}, -2,	0x0004FF00,4,20,0x912F,E_OOP_ENERGY_T, -2, },    // (当前)反向无功电能块(-RL,-RC)
    {{0x00400201}, -2,	0x00040000,4,4, 0x9120,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00400202}, -2,	0x00040100,4,4, 0x9121,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率1反向无功电能
    {{0x00400203}, -2,	0x00040200,4,4, 0x9122,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率2反向无功电能
    {{0x00400204}, -2,	0x00040300,4,4, 0x9123,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率3反向无功电能
    {{0x00400205}, -2,	0x00040400,4,4, 0x9124,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率4反向无功电能

    //一象限无功总电能
    {{0x00500200}, -2,	0x0005FF00,4,20,0x913F,E_OOP_ENERGY_T, -2, },    // (当前)一象限无功电能块(+RL)
    {{0x00500201}, -2,	0x00050000,4,4, 0x9130,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)一象限无功总电能(+RL)
    {{0x00500202}, -2,	0x00050100,4,4, 0x9131,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率1一象限无功电能
    {{0x00500203}, -2,	0x00050200,4,4, 0x9132,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率2一象限无功电能
    {{0x00500204}, -2,	0x00050300,4,4, 0x9133,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率3一象限无功电能
    {{0x00500205}, -2,	0x00050400,4,4, 0x9134,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率4一象限无功电能

	//二象限无功总电能
    {{0x00600200}, -2,	0x0006FF00,4,20,0x915F,E_OOP_ENERGY_T, -2, },    // (当前)二象限无功电能块(+Rc)
    {{0x00600201}, -2,	0x00060000,4,4, 0x9150,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)二象限无功总电能(+Rc)
    {{0x00600202}, -2,	0x00060100,4,4, 0x9151,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率1二象限无功电能
    {{0x00600203}, -2,	0x00060200,4,4, 0x9152,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率2二象限无功电能
    {{0x00600204}, -2,	0x00060300,4,4, 0x9153,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率3二象限无功电能
    {{0x00600205}, -2,	0x00060400,4,4, 0x9154,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率4二象限无功电能

    //三象限无功总电能
    {{0x00700200}, -2,	0x0007FF00,4,20,0x916F,E_OOP_ENERGY_T, -2, },    // (当前)三象限无功电能块(-RL)
    {{0x00700201}, -2,	0x00070000,4,4, 0x9160,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)三象限无功总电能(-RL)
    {{0x00700202}, -2,	0x00070100,4,4, 0x9161,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率1三象限无功电能
    {{0x00700203}, -2,	0x00070200,4,4, 0x9162,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率2三象限无功电能
    {{0x00700204}, -2,	0x00070300,4,4, 0x9163,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率3三象限无功电能
    {{0x00700205}, -2,	0x00070400,4,4, 0x9164,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率4三象限无功电能

    //四象限无功总电能
    {{0x00800200}, -2,	0x0008FF00,4,20,0x914F,E_OOP_ENERGY_T, -2, },    // (当前)四象限无功电能块(-Rc)
    {{0x00800201}, -2,	0x00080000,4,4, 0x9140,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)四象限无功总电能(-Rc)
    {{0x00800202}, -2,	0x00080100,4,4, 0x9141,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率1四象限无功电能
    {{0x00800203}, -2,	0x00080200,4,4, 0x9142,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率2四象限无功电能
    {{0x00800204}, -2,	0x00080300,4,4, 0x9143,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率3四象限无功电能
    {{0x00800205}, -2,	0x00080400,4,4, 0x9144,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率4四象限无功电能

    //(当前)正向有功总最大需量
    {{0x10100200},  0,	 0x0101FF00,	8,40,	0xA01F,E_OOP_DEMAND_T,  0,       },  // (当前)正向有功最大需量块
    {{0x10100200},  0,	 0x0101FF00,	8,40,	0xB01F,E_OOP_DEMAND_T,  0,       },  // (当前)正向有功最大需量块
    {{0x10100201},  0,	 0x01010000,	8,8,	0xA010,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)正向有功总最大需量
    {{0x10100201},  0,	 0x01010000,	8,8,	0xB010,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)正向有功总最大需量
    {{0x10100202},  0,	 0x01010100,	8,8,	0xA011,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1正向有功最大需量
    {{0x10100202},  0,	 0x01010100,	8,8,	0xB011,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1正向有功最大需量
    {{0x10100203},  0,	 0x01010200,	8,8,	0xA012,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2正向有功最大需量
    {{0x10100203},  0,	 0x01010200,	8,8,	0xB012,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2正向有功最大需量
    {{0x10100204},  0,	 0x01010300,	8,8,	0xA013,E_OOP_DEMAND_T,  0       },  // 3字节, XX.XXXX,  (当前)费率3正向有功最大需量
    {{0x10100204},  0,	 0x01010300,	8,8,	0xB013,E_OOP_DEMAND_T,  0       },  // 3字节, XX.XXXX,  (当前)费率3正向有功最大需量
    {{0x10100205},  0,	 0x01010400,	8,8,	0xA014,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4正向有功最大需量
    {{0x10100205},  0,	 0x01010400,	8,8,	0xB014,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4正向有功最大需量

    //(当前)反向有功总最大需量
    {{0x10200200},  0,	 0x0102FF00,	8,40,	0xA02F,E_OOP_DEMAND_T,  -0,       },  // (当前)反向有功最大需量块
	{{0x10200200},  0,	 0x0102FF00,	8,40,	0xB02F,E_OOP_DEMAND_T,  -0,       },  // (当前)反向有功最大需量块
    {{0x10200201},  0,	 0x01020000,	8,8,	0xA020,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)反向有功总最大需量
	{{0x10200201},  0,	 0x01020000,	8,8,	0xB020,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)反向有功总最大需量
    {{0x10200202},  0,	 0x01020100,	8,8,	0xA021,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率1反向有功最大需量
	{{0x10200202},  0,	 0x01020100,	8,8,	0xB021,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率1反向有功最大需量
    {{0x10200203},  0,	 0x01020200,	8,8,	0xA022,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率2反向无功向有功最大需量
	{{0x10200203},  0,	 0x01020200,	8,8,	0xB022,E_OOP_DEMAND_T,  -0,		 },  // 3字节, XX.XXXX,  (当前)费率2反向无功向有功最大需量
    {{0x10200204},  0,	 0x01020300,	8,8,	0xA023,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率3反向无功向有功最大需量
	{{0x10200204},  0,	 0x01020300,	8,8,	0xB023,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率3反向无功向有功最大需量
    {{0x10200205},  0,	 0x01020400,	8,8,	0xA024,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率4反向无功向有功最大需量
	{{0x10200205},  0,	 0x01020400,	8,8,	0xB024,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率4反向无功向有功最大需量

    //(当前)正向无功总最大需量
    {{0x10300200}, 0,	 0x0103FF00,	8,40,	0xA11F,E_OOP_DEMAND_T,  -0,       },  //  (当前)正向无功最大需量块
	{{0x10300200}, 0,	 0x0103FF00,	8,40,	0xB11F,E_OOP_DEMAND_T,  -0,       },  //  (当前)正向无功最大需量块
    {{0x10300201}, 0,	 0x01030000,	8,8,	0xA110,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)正向无功总最大需量
	{{0x10300201}, 0,	 0x01030000,	8,8,	0xB110,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)正向无功总最大需量
    {{0x10300202}, 0,	 0x01030100,	8,8,	0xA111,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率1正向无功最大需量
	{{0x10300202}, 0,	 0x01030100,	8,8,	0xB111,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率1正向无功最大需量  
    {{0x10300203}, 0,	 0x01030200,	8,8,	0xA112,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率2正向无功最大需量
	{{0x10300203}, 0,	 0x01030200,	8,8,	0xB112,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率2正向无功最大需量
    {{0x10300204}, 0,	 0x01030300,	8,8,	0xA113,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率3正向无功最大需量
	{{0x10300204}, 0,	 0x01030300,	8,8,	0xB113,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率3正向无功最大需量
    {{0x10300205}, 0,	 0x01030400,	8,8,	0xA114,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率4正向无功最大需量
	{{0x10300205}, 0,	 0x01030400,	8,8,	0xB114,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率4正向无功最大需量

    //反向无功总最大需量
    {{0x10400200}, 0,	 0x0104FF00,	8,40,0xA12F,E_OOP_DEMAND_T,  0,       },  //(当前)反向无功最大需量块
	{{0x10400200}, 0,	 0x0104FF00,	8,40,0xB12F,E_OOP_DEMAND_T,  0,       },  //(当前)反向无功最大需量块
    {{0x10400201}, 0,	 0x01040000,	8,8,0xA120,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前反向无功总最大需量
	{{0x10400201}, 0,	 0x01040000,	8,8,0xB120,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前反向无功总最大需量
    {{0x10400202}, 0,	 0x01040100,	8,8,0xA121,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1反向无功最大需量
	{{0x10400202}, 0,	 0x01040100,	8,8,0xB121,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1反向无功最大需量
    {{0x10400203}, 0,	 0x01040200,	8,8,0xA122,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2反向无功最大需量
	{{0x10400203}, 0,	 0x01040200,	8,8,0xB122,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2反向无功最大需量
    {{0x10400204}, 0,	 0x01040300,	8,8,0xA123,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3反向无功最大需量
	{{0x10400204}, 0,	 0x01040300,	8,8,0xB123,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3反向无功最大需量
    {{0x10400205}, 0,	 0x01040400,	8,8,0xA124,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4反向无功最大需量
	{{0x10400205}, 0,	 0x01040400,	8,8,0xB124,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4反向无功最大需量
    //分相正向有功总最大需量
    {{0x10110201},  0,0x01150000,	8,8,	0xFFFF,E_OOP_DEMAND_T,  -0,       },    // 3字节, XX.XXXX,  (当前)A相正向有功总最大需量

    {{0x10120201},  0,0x01290000,	8,8,	0xFFFF,E_OOP_DEMAND_T,  -0,       },    // 35字节,XX.XXXX,  (当前)B相正向有功总最大需量

    {{0x10130201},  0,0x013D0000,	8,8,	0xFFFF,E_OOP_DEMAND_T,  -0,       },    // 3字节, XX.XXXX,  (当前)C相正向有功总最大需量

    //电压
    {{0x20000200},			   -1,	0x0201FF00,2,6,0xB61F,E_OOP_WORD3V_T,	-1, },   // 电压数据块
    {{0x20000201},			   -1,	0x02010100,2,2,0xB610,E_OOP_WORD3V_T,	-1, },   // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20000202},			   -1,	0x02010200,2,2,0xB611,E_OOP_WORD3V_T,	-1, },   // 2字节, X.XXX B相电压/无/无
    {{0x20000203},			   -1,	0x02010300,2,2,0xB612,E_OOP_WORD3V_T,	-1, },   // 2字节, X.XXX C相电压/CB线电压/无
    //电流
    {{0x20010200},			   -3,	0x0202FF00,3,9,0xB62F,E_OOP_INT3V_T,	-3, },	// 电流数据块
    {{0x20010201},			   -3,	0x02020100,3,3,0xB620,E_OOP_INT3V_T,	-3, },	// 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20010202},			   -3,	0x02020200,3,3,0xB621,E_OOP_INT3V_T,	-3, },	// 3字节, XXX.XXX, B相电流/无/无
    {{0x20010203},			   -3,	0x02020300,3,3,0xB622,E_OOP_INT3V_T,	-3, },	// 3字节, XXX.XXX, C相电流/C相电流/无
	{{0x20010400},			   -3,	0x02800001,3,3,0xFFFF,E_DOUBLE_LONG,	-3, },	// 3字节, XXX.XXX, C相电流/C相电流/无

    //零序电压
	{{0x20000400},			   -1,	0xFFFFFFFF,2,2,0xFFFF,E_LONG_UNSIGNED,	-1, },  // 2字节, XXX.X 零序电压

    //零序电流
    {{0x20010600},			   -3,	0x02800001,3,3,0xFFFF,E_DOUBLE_LONG,	-3, },	// 3字节, XXX.XXX, 零序电流
    
    //电压电流相角
    {{0x20030200},		   -1,	0x0207FF00,2,6,0xFFFF,E_OOP_WORD3V_T,	-1, },	// 电压电流相角数据块
    {{0x20030201},		   -1,	0x02070100,2,2,0xFFFF,E_OOP_WORD3V_T,	-1, },	// 2字节, XXX.X, A相电压电流相角
    {{0x20030202},		   -1,	0x02070200,2,2,0xFFFF,E_OOP_WORD3V_T,	-1, },	// 2字节, XXX.X, B相电压电流相角
    {{0x20030203},		   -1,	0x02070300,2,2,0xFFFF,E_OOP_WORD3V_T,	-1, },	// 2字节, XXX.X, C相电压电流相角

    //电压相角
    {{0x20020200},         -1,  0xEFB10000,2,6,0xFFFF,E_OOP_WORD3V_T,   -1, },  // 电压相角数据块

    //电流相角
    {{0xAAAA0001},         -1,  0xEFB10000,2,6,0xFFFF,E_OOP_WORD3V_T,   -1, },  // 电压相角数据块

    
    //A相电压谐波含有率（总及2…n次）
    {{0x200D0200},         -2,  0x020A01FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -2, },  // 2字节, XX.XX, A相电压谐波含有率（总及2…n次）21次
    {{0x200D0201},         -2,  0x020A01FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -2, },  // 2字节, XX.XX, A相电压谐波含有率（总及2…n次）21次
    //B相电压谐波含有率（总及2…n次）
    {{0x200D0300},         -2,  0x020A02FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -2, },  // 2字节, XX.XX, B相电压谐波含有率（总及2…n次）21次
    {{0x200D0301},         -2,  0x020A02FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -2, },  // 2字节, XX.XX, B相电压谐波含有率（总及2…n次）21次
    //C相电压谐波含有率（总及2…n次）
    {{0x200D0400},         -2,  0x020A03FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -2, },  // 2字节, XX.XX, C相电压谐波含有率（总及2…n次）21次
    {{0x200D0401},         -2,  0x020A03FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -2, },  // 2字节, XX.XX, C相电压谐波含有率（总及2…n次）21次

    //A相电流谐波含有率（总及2…n次）
    {{0x200E0200},         -2,  0x020B01FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -2, },  // 2字节, XX.XX, A相电流谐波含有率（总及2…n次）21次
    {{0x200E0201},         -2,  0x020B01FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -2, },  // 2字节, XX.XX, A相电流谐波含有率（总及2…n次）21次
    //B相电流谐波含有率（总及2…n次）
    {{0x200E0300},         -2,  0x020B02FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -2, },  // 2字节, XX.XX, B相电流谐波含有率（总及2…n次）21次
    {{0x200E0301},         -2,  0x020B02FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -2, },  // 2字节, XX.XX, B相电流谐波含有率（总及2…n次）21次
    //C相电流谐波含有率（总及2…n次）
    {{0x200E0400},         -2,  0x020B03FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -2, },  // 2字节, XX.XX, C相电流谐波含有率（总及2…n次）21次
    {{0x200E0401},         -2,  0x020B03FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -2, },  // 2字节, XX.XX, C相电流谐波含有率（总及2…n次）21次

    //A相电压谐波含量（总及2…n次）
    {{0x20330200},         -1,  0x020C01FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -1, },  // 2字节, XX.XX, A相电压谐波含量（1…n次）21次
    {{0x20330201},         -1,  0x020C01FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -1, },  // 2字节, XX.XX, A相电压谐波含量（1…n次）21次
    //B相电压谐波含量（总及2…n次）
    {{0x20330300},         -1,  0x020C02FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -1, },  // 2字节, XX.XX, B相电压谐波含量（1…n次）21次
    {{0x20330301},         -1,  0x020C02FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -1, },  // 2字节, XX.XX, B相电压谐波含量（1…n次）21次
    //C相电压谐波含量（总及2…n次）
    {{0x20330400},         -1,  0x020C03FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -1, },  // 2字节, XX.XX, C相电压谐波含量（1…n次）21次
    {{0x20330401},         -1,  0x020C03FF,2,42,0xFFFF,E_OOP_HARMONIC_T,    -1, },  // 2字节, XX.XX, C相电压谐波含量（1…n次）21次

    //A相电流谐波含量（总及2…n次）
    {{0x20340200},         -3,  0x020D01FF,3,63,0xFFFF,E_OOP_HARMONIC_T,    -3, },  // 2字节, XX.XX, A相电流谐波含量（1…n次）21次
    {{0x20340201},         -3,  0x020D01FF,3,63,0xFFFF,E_OOP_HARMONIC_T,    -3, },  // 2字节, XX.XX, A相电流谐波含量（1…n次）21次
    //B相电流谐波含量（总及2…n次）
    {{0x20340300},         -3,  0x020D02FF,3,63,0xFFFF,E_OOP_HARMONIC_T,    -3, },  // 2字节, XX.XX, B相电流谐波含量（1…n次）21次
    {{0x20340301},         -3,  0x020D02FF,3,63,0xFFFF,E_OOP_HARMONIC_T,    -3, },  // 2字节, XX.XX, B相电流谐波含量（1…n次）21次
    //C相电流谐波含量（总及2…n次）
    {{0x20340400},         -3,  0x020D03FF,3,63,0xFFFF,E_OOP_HARMONIC_T,    -3, },  // 2字节, XX.XX, C相电流谐波含量（1…n次）21次
    {{0x20340401},         -3,  0x020D03FF,3,63,0xFFFF,E_OOP_HARMONIC_T,    -3, },  // 2字节, XX.XX, C相电流谐波含量（1…n次）21次

    //瞬时正向有功功率
    {{0x20040200},			 -1,	0x0203FF00,3,12,0xB63F,E_OOP_INT4V_T,	-4, },	 // 瞬时正向有功功率块
    {{0x20040201},			   -1,	0x02030000,3,3, 0xB630,E_OOP_INT4V_T,	-4, },	 // 3字节, XX.XXXX, 瞬时正向有功功率,都为正，与RT_NEG_Z_AP_EV_ID 互斥
    {{0x20040202},			   -1,	0x02030100,3,3, 0xB631,E_OOP_INT4V_T,	-4, },	 // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20040203},			   -1,	0x02030200,3,3, 0xB632,E_OOP_INT4V_T,	-4, },	 // 3字节, XX.XXXX, B相有功功率,有正负,格式同上
    {{0x20040204},			   -1,	0x02030300,3,3, 0xB633,E_OOP_INT4V_T,	-4, },	 // 3字节, XX.XXXX, C相有功功率,有正负,格式同上

    //瞬时正向无功功率
    {{0x20050200},			 -1,	0x0204FF00,3,12,0xB64F,E_OOP_INT4V_T,	-4, },	 //
    {{0x20050201},			   -1,	0x02040000,3,3, 0xB640,E_OOP_INT4V_T,	-4, },	 // 瞬时正向无功功率块
    {{0x20050202},			   -1,	0x02040100,3,3, 0xB641,E_OOP_INT4V_T,	-4, },	 // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20050203},			   -1,	0x02040200,3,3, 0xB642,E_OOP_INT4V_T,	-4, },	 // 3字节, XX.XXXX, B相无功功率,格式同上
    {{0x20050204},			   -1,	0x02040300,3,3, 0xB643,E_OOP_INT4V_T,	-4, },	 // 3字节, XX.XXXX, C相无功功率,格式同上

    //瞬时总功率因数
    {{0x200A0200},			 -3,	0x0206FF00,2,8,0xB65F,E_OOP_LONG4V_T,	-3, },	 // 瞬时功率因数块
    {{0x200A0201},			   -3,	0x02060000,2,2,0xB650,E_OOP_LONG4V_T,	-3, },	 // 2字节, XX.XXXX, 瞬时总功率因数
    {{0x200A0202},			   -3,	0x02060100,2,2,0xB651,E_OOP_LONG4V_T,	-3, },	 // 2字节, XX.XXXX, A相功率因数
    {{0x200A0203},			   -3,	0x02060200,2,2,0xB652,E_OOP_LONG4V_T,	-3, },	 // 2字节, XX.XXXX, B相功率因数
    {{0x200A0204},			   -3,	0x02060300,2,2,0xB653,E_OOP_LONG4V_T,	-3, },	 // 2字节, XX.XXXX, C相功率因数
	{{0x200F0200},			   -2,	0x02800002,2,2,0xFFFF,E_LONG_UNSIGNED,	-2, },	 // 2字节, XX.XXXX, 频率
    //瞬时视在功率
    {{0x20060200},			   -1, 0x0205FF00,3,12,0xB66F,E_OOP_INT4V_T,  -4, },	// 瞬时视在功率块
    {{0x20060201},			   -1, 0x02050000,3,3, 0xB660,E_OOP_INT4V_T,  -4, },	// 3字节, XX.XXXX, 瞬时视在功率
    {{0x20060202},			   -1, 0x02050100,3,3, 0xB661,E_OOP_INT4V_T,  -4, },	// 3字节, XX.XXXX, A相视在功率
    {{0x20060203},			   -1, 0x02050200,3,3, 0xB662,E_OOP_INT4V_T,  -4, },	// 3字节, XX.XXXX, B相视在功率
    {{0x20060204},			   -1, 0x02050300,3,3, 0xB663,E_OOP_INT4V_T,  -4, },	// 3字节, XX.XXXX, C相视在功率

    {{0x20140200},		   0, 0x040005FF,2,14,0xFFFF,E_OOP_METWORD_T,   0,},	// 7*2字节,电表运行状态字块
    {{0x20140201},		   0, 0x04000501,2,2,0xC020,E_OOP_METWORD_T,	 0, },	// 2字节,电表运行状态字1
    {{0x20140202},		   0, 0x04000502,2,2,0xC021,E_OOP_METWORD_T,	 0, },	// 2字节,电表运行状态字2
    {{0x20140203},		   0, 0x04000503,2,2,0xFFFF,E_OOP_METWORD_T,	 0, },	// 2字节,电表运行状态字3
    {{0x20140204},		   0, 0x04000504,2,2,0xFFFF,E_OOP_METWORD_T,	 0, },	// 2字节,电表运行状态字4
    {{0x20140205},		   0, 0x04000505,2,2,0xFFFF,E_OOP_METWORD_T,	 0, },	// 2字节,电表运行状态字5
    {{0x20140206},		   0, 0x04000506,2,2,0xFFFF,E_OOP_METWORD_T,	 0, },	// 2字节,电表运行状态字6
    {{0x20140207},		   0, 0x04000507,2,2,0xFFFF,E_OOP_METWORD_T,	 0, },	// 2字节,电表运行状态字7

    {{0x20260200},		   -2, 0xFFFFFFFF,2,2,0xFFFF,E_LONG_UNSIGNED,	 -2, },	// 2字节,电压不平衡率
	{{0x20270200},		   -2, 0xFFFFFFFF,2,2,0xFFFF,E_LONG_UNSIGNED,	 -2, },	// 2字节,电流不平衡率
	
    {{0x40000200},		   0, 0x04000101,4,4,0xC010,E_REAL_DATETIME_S,   0, },// 4字节,
    {{0x40000200},		   0, 0x04000102,3,3,0xC011,E_REAL_DATETIME_S,   0, },	        // 3字节,

    {{0x202C0200},       -2,    0x00900200, 4,4,0xFFFF,    E_OOP_WALLET_T, -2, },// 4字节,钱包文件
    {{0x202C0200},        0,    0x03330201, 2,2,0xFFFF,    E_OOP_WALLET_T,  0, },// 3字节,钱包文件
    {{0x202C0201},       -2,    0x00900200, 4,4,0xFFFF,    E_DOUBLE_LONG_UNSIGNED, -2, },// 4字节,剩余金额
    {{0x202C0202},        0,    0x03330201, 2,2,0xFFFF,    E_DOUBLE_LONG_UNSIGNED,  0, },// 3字节,购电次数
    {{0x202D0200},       -2,    0x00900201, 4,4,0xFFFF,    E_DOUBLE_LONG_UNSIGNED, -2, },// 4字节,透支金额
    {{0x202E0200},       -2,    0x03330601, 4,4,0xFFFF,    E_DOUBLE_LONG_UNSIGNED, -2, },// 4字节,累计购电金额

    //第一象限无功总最大需量及发生时间
    {{0x10500200}, 0,	 0x0105FF00,	8,40,	0xA13F,E_OOP_DEMAND_T,  0,       },  //  (当前)第一象限无功总最大需量块
    {{0x10500200}, 0,	 0x0105FF00,	8,40,	0xB13F,E_OOP_DEMAND_T,  0,       },  //  (当前)第一象限无功总最大需量块
    {{0x10500201}, 0,	 0x01050000,	8,8,	0xA130,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第一象限无功总总最大需量
	{{0x10500201}, 0,	 0x01050000,	8,8,	0xB130,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第一象限无功总总最大需量
    {{0x10500202}, 0,	 0x01050100,	8,8,	0xA131,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第一象限无功最大需量
	{{0x10500202}, 0,	 0x01050100,	8,8,	0xB131,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第一象限无功最大需量
    {{0x10500203}, 0,	 0x01050200,	8,8,	0xA132,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第一象限无功最大需量
	{{0x10500203}, 0,	 0x01050200,	8,8,	0xB132,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第一象限无功最大需量
    {{0x10500204}, 0,	 0x01050300,	8,8,	0xA133,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第一象限无功最大需量
	{{0x10500204}, 0,	 0x01050300,	8,8,	0xB133,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第一象限无功最大需量
    {{0x10500205}, 0,	 0x01050400,	8,8,	0xA134,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第一象限无功最大需量
	{{0x10500205}, 0,	 0x01050400,	8,8,	0xB134,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第一象限无功最大需量

    //第二象限无功总最大需量及发生时间
    {{0x10600200}, 0,	 0x0106FF00,	8,40,	0xA15F,E_OOP_DEMAND_T,  0,       },  //  (当前)第二象限无功总最大需量块
	{{0x10600200}, 0,	 0x0106FF00,	8,40,	0xB15F,E_OOP_DEMAND_T,  0,       },  //  (当前)第二象限无功总最大需量块
    {{0x10600201}, 0,	 0x01060000,	8,8,	0xA150,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第二象限无功总总最大需量
	{{0x10600201}, 0,	 0x01060000,	8,8,	0xB150,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第二象限无功总总最大需量
    {{0x10600202}, 0,	 0x01060100,	8,8,	0xA151,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第二象限无功最大需量
	{{0x10600202}, 0,	 0x01060100,	8,8,	0xB151,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第二象限无功最大需量
    {{0x10600203}, 0,	 0x01060200,	8,8,	0xA152,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第二象限无功最大需量
	{{0x10600203}, 0,	 0x01060200,	8,8,	0xB152,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第二象限无功最大需量
    {{0x10600204}, 0,	 0x01060300,	8,8,	0xA153,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第二象限无功最大需量
	{{0x10600204}, 0,	 0x01060300,	8,8,	0xB153,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第二象限无功最大需量
    {{0x10600205}, 0,	 0x01060400,	8,8,	0xA154,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第二象限无功最大需量	
	{{0x10600205}, 0,    0x01060400,	8,8,	0xB154,E_OOP_DEMAND_T,  0,		},	// 3字节, XX.XXXX,  (当前)费率4第二象限无功最大需量
    //第三象限无功总最大需量及发生时间
    {{0x10700200}, 0,	 0x0107FF00,	8,40,	0xA16F,E_OOP_DEMAND_T,  0,       },  //  (当前)第三象限无功总最大需量块
	{{0x10700200}, 0,	 0x0107FF00,	8,40,	0xB16F,E_OOP_DEMAND_T,  0,       },  //  (当前)第三象限无功总最大需量块
    {{0x10700201}, 0,	 0x01070000,	8,8,	0xA160,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第三象限无功总总最大需量
	{{0x10700201}, 0,	 0x01070000,	8,8,	0xB160,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第三象限无功总总最大需量
    {{0x10700202}, 0,	 0x01070100,	8,8,	0xA161,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第三象限无功最大需量
	{{0x10700202}, 0,	 0x01070100,	8,8,	0xB161,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第三象限无功最大需量
    {{0x10700203}, 0,	 0x01070200,	8,8,	0xA162,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第三象限无功最大需量
	{{0x10700203}, 0,	 0x01070200,	8,8,	0xB162,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第三象限无功最大需量
    {{0x10700204}, 0,	 0x01070300,	8,8,	0xA163,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第三象限无功最大需量
	{{0x10700204}, 0,	 0x01070300,	8,8,	0xB163,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第三象限无功最大需量
    {{0x10700205}, 0,	 0x01070400,	8,8,	0xA164,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第三象限无功最大需量
	{{0x10700205}, 0,	 0x01070400,	8,8,	0xB164,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第三象限无功最大需量
    //第四象限无功总最大需量及发生时间
    {{0x10800200}, 0,	 0x0108FF00,	8,40,	0xA14F,E_OOP_DEMAND_T,  0,       },  //  (当前)第四象限无功总最大需量块
	{{0x10800200}, 0,	 0x0108FF00,	8,40,	0xB14F,E_OOP_DEMAND_T,  0,       },  //  (当前)第四象限无功总最大需量块
    {{0x10800201}, 0,	 0x01080000,	8,8,	0xA140,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第四象限无功总总最大需量
	{{0x10800201}, 0,	 0x01080000,	8,8,	0xB140,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第四象限无功总总最大需量
    {{0x10800202}, 0,	 0x01080100,	8,8,	0xA141,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第四象限无功最大需量
	{{0x10800202}, 0,	 0x01080100,	8,8,	0xB141,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第四象限无功最大需量
    {{0x10800203}, 0,	 0x01080200,	8,8,	0xA142,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第四象限无功最大需量
	{{0x10800203}, 0,	 0x01080200,	8,8,	0xB142,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第四象限无功最大需量
    {{0x10800204}, 0,	 0x01080300,	8,8,	0xA143,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第四象限无功最大需量
	{{0x10800204}, 0,	 0x01080300,	8,8,	0xB143,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第四象限无功最大需量
    {{0x10800205}, 0,	 0x01080400,	8,8,	0xA144,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第四象限无功最大需量
	{{0x10800205}, 0,	 0x01080400,	8,8,	0xB144,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第四象限无功最大需量

    //电容器电流
    {{0x260A0200},			   -3,	0x0202FF00,3,9,0xB62F,E_OOP_INT3V_T,	-3, },	// 电流数据块

    {{0xFFFFFFFF}, 0xFF, 0xFFFFFFFF,    0xFF, 0xFF,    0xFFFF, 0xFF,    0xFF},

};

/****************************************************************/
//需要通信管理线程刷新的全局变量
/***************************************************************/
uint8  g_nNetWorkFlag = PRO_DATA_TRANSMIT;  //链路连接状态标志

/****************************************************************/
//全局变量配置区
/***************************************************************/
uint16 g_nPublicAddr = 1;  //终端公共地址
uint8 g_nGlobalPipe = 1;  //全局通道序号(记录最近一次的收发通道号，主要用于变化遥测、变化遥信和故障事件主动上报)
BOOL g_bGlobalInitFlag = FALSE;  //全局初始化完成标志

BOOL g_bEnergyCycSendFlag = FALSE;  //-突发送电量上送功能-
BOOL g_bYcCycSendFlag = FALSE;  //-循环上送数据功能-
BOOL g_bBackScanSendFlag = FALSE;  //-背景扫描功能-

uint8  g_bNetTestFlag               = 0;   //链路测试使能
uint8  g_bAckRecvFlag               = 0;   //104归约链路接收应答s帧发送标志
uint8  g_bInitEndFlag               = 0;   /* 初始化结束标志 */
uint8  g_bDataSendFlag              = 0;   //全局可以发送数据的标志
int8   g_nIFrameT1Count             = 0;	 /*帧未收到确认回答的最大延时时间*/	
uint8  g_nIFrameT2Count	          = 0;	 //i帧应答确认超时时间
uint8  g_nIFrameT3Count	          = 0;	 //链路测试最大间隔时间
uint8  g_nNoneActIFramTimes	      = 0;   /*发送I帧未收到确认帧的次数*/
uint16 g_nIFramRecvTimes	          = 0;	 /*收到I帧的帧数量*/ 	
uint16 g_nIFramSendTimes	          = 0;	 /*I帧发送帧数量*/	
uint16 g_nRcvIFramSendNum	          = 0;	 /*收到帧的发送帧序号*/	
uint16 g_nRcvFramRcvNum	          = 0;	 /*收到帧的接收帧序号*/	
uint8  g_bAllDataCallFlag           = 0;   //总召唤启动标志	
uint8  g_bPointCallFlag           = 0;   //点召唤启动标志	
uint8  g_bTaskCallFlag           = 0;   //任务召唤启动标志
uint8  g_bAllParaCallFlag           = 0;   //所有参数召唤启动标志
uint8  g_bSomeParaCallFlag          = 0;   //部分参数召唤启动标志
uint8  g_bYCYXChangeSendFlag        = 0;   /*遥测数据变化上报使能标志,在连接成功且上报过总召唤后打开*/
uint8  g_bTimerSynFlag              = 0;   /*已时钟同步的标志*/
uint8  g_nElecDataCallFlag          = 0;   //电能召唤标志
uint8  g_nFrameErrTimes             = 0;   //用于判断104归约帧序号错误的次数，如果大于3，关闭通道，小于3发链路复位
uint16 g_nDataReadFlagOrAddr        = 0;   /*读数据使能标志,如果有读数据，置为数据信息体地址*/
uint8  g_nRtuResetFlag              = 0;   /*终端复位标志，1表示全复位，清soe队列及遥测数据，2表示只清soe事件队列
                                                       				4表示终端停电复位，8表示终端本地复位，最高位如果置位，表示主站下发复位后，未复位终端的状态*/

uint32  g_nParaDataAddr              = 0; //参数定值召唤保存召唤的数据的信息体地址
uint8   g_nParaDataStep              = 0;//参数上送标志 1，启动上送，2，主站收到上送，3，上送结束 ，0无上送

uint8  g_nControlState                 = 0xaa;     //遥控全局变量，0x55预置，0xaa执行
uint8  g_nRemoteSetFlag                = 0;       //遥控预置标志
uint8  g_nRemoteSetDelay               = 0;       //遥控预置延时时间，不允许超过60秒
uint8  g_aCtrlReSendBuff[MAX_YK_BUFF_LEN] = {0};     //存储遥控下发数据，第一个字节为标志，第二个字节开始存放数据帧-

uint8  g_nParaFixedFlag                = 0;       //参数固化标志

uint8   g_nParaGetFlag = 0;       //101、104规约参数读取后续处理使能 ，国网新实施细则 
uint16  g_nParaSetFlag = 0;       //101、104规约参数设置后续处理使能 ，国网新实施细则 
uint8   g_nParaPortChangeFlag = 0;//101、104规约参数区号切换后续处理使能 ，国网新实施细则 
uint8   g_nParaPortReadFlag = 0;  //101、104规约参数区号读取后续处理使能 ，国网新实施细则

BOOL    g_bYxChanged = FALSE;    //遥信变化，需要上送
BOOL    g_bYcChanged = FALSE;    //遥测变化，需要上送
BOOL    g_bEnergyChanged = FALSE;    //电量变化，需要上送
BOOL    g_bFaultChanged = FALSE;    //事件变化，需要上送

uint8   g_nFixedValueArea = 0;    //定值区号
BOOL    g_bFixedValueChanged = FALSE;    //定值参数改变，需要同步至数据中心

TASK_RUN_STEP_T g_sTaskRunStep = {0};  //104任务执行流程全局变量
TASK_REPORT_PIPE_T g_sTaskReportPipe = {0};  //上报任务对应通道号


/********************************************************************
所属功能单元：GB104规约处理模块
功能描述	：S帧打包
输入参数	：无
			
输出参数	：*pOutBuf-发送缓冲区地址
			  
函数返回值	：无
*******************************************************************/
int s_frame_data_pack(uint8 *pOutBuf)
{
	int nOutbufLen = 6;//返回的发送数据长度	

	TOTAL_104_FRAME_T stTotal104Frame;
	memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
	
	stTotal104Frame.nHead= 0x68;
	stTotal104Frame.nLength = 0x04;
	stTotal104Frame.sCrtlType12.sbit.fir = 1;
	stTotal104Frame.sCrtlType12.sbit.framenum = 0;
	stTotal104Frame.sCrtlType34.rbit.fir = 0;
	stTotal104Frame.sCrtlType34.rbit.framenum = g_nIFramRecvTimes;
	g_nIFrameT2Count = 0;

	memcpy(pOutBuf, &stTotal104Frame, nOutbufLen);
	return nOutbufLen;
}


/********************************************************************
所属功能单元：GB104规约处理模块
功能描述	：U帧打包
输入参数	：bTpye：0x08启动发送的确认
                     0x20关闭发送的确认
                     0x80链路测试的确认
                     0x40启动链路的测试
			
输出参数	：*pOutBuf-发送缓冲区地址
函数返回值	：发送数据长度
*******************************************************************/
int u_frame_data_pack(uint8 bTpye, uint8 *pOutBuf)
{
	int nOutbufLen = 6;//返回的发送数据长度	

	TOTAL_104_FRAME_T stTotal104Frame;
	memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));

	stTotal104Frame.nHead= 0x68;
	stTotal104Frame.nLength = 0x04;
	stTotal104Frame.sCrtlType12.crtltype12[0] = 0;
	stTotal104Frame.sCrtlType12.ubit.fir = 3;
	stTotal104Frame.sCrtlType12.ubit.framenum = 0;
	stTotal104Frame.sCrtlType34.crtltype34[0] = 0;
	stTotal104Frame.sCrtlType34.crtltype34[1] = 0;
	switch(bTpye)
	{
		case 0x08://启动发送的确认
			stTotal104Frame.sCrtlType12.ubit.stacon = 1;
			break;
		
		case 0x20://关闭发送的确认
			stTotal104Frame.sCrtlType12.ubit.stocon = 1;
			break;

		case 0x40://启动链路的测试
			stTotal104Frame.sCrtlType12.ubit.testact = 1;
			g_nIFrameT1Count = MAX_FRAMCOM_DELAY_T1;
			break;
			
		case 0x80://链路测试的确认
			stTotal104Frame.sCrtlType12.ubit.testcon = 1;
			break;
		
		case 0x55://主动上报前测试联络，这里不再判超时，以免影响正常通讯
			stTotal104Frame.sCrtlType12.ubit.testact = 1;
			break;
        case 0xaa://链路复位
            stTotal104Frame.sCrtlType12.ubit.staact = 1;
            break;
		case 0xbb://关闭链路
			stTotal104Frame.sCrtlType12.ubit.stoact = 1;
            break;
		default:
			break;
	}

	memcpy(pOutBuf, &stTotal104Frame, nOutbufLen);
	return nOutbufLen;
}


/*********************************************************************
所属功能单元：GB104规约处理模块
功能描述	：I帧打包
输入参数：nTI-数据标识, 
          nVSQ-可变结构限定词, 
          nCOT-传送原因
          *pInObjBuff-信息对象内容，
          nInObjLen-信息对象长度
输出参数	：*pOutBuf-发送缓冲区地址			  
函数返回值	：正数表示返回数据的长度，-1表示需要断连
*******************************************************************/
int i_frame_data_pack(uint8 nTI, uint8 nVSQ, uint16 nCOT, uint8 *pInObjBuff, uint8 nInObjLen, uint8 *pOutBuf)
{
	uint8 bStep = 0;
    union s_num_union sCrtlType12;
    union r_num_union sCrtlType34;
    uint8 len = 0;
    
	g_nNoneActIFramTimes++;

	g_nIFrameT1Count = MAX_FRAMCOM_DELAY_T1;
    pOutBuf[bStep++] = 0x68;

    len = CTRL_UNIT_LABEL_LEN + nInObjLen;
    pOutBuf[bStep++] = len;
    
    sCrtlType12.ibit.fir = 0;
	sCrtlType12.ibit.framenum = g_nIFramSendTimes;
	sCrtlType34.rbit.fir = 0;
	sCrtlType34.rbit.framenum = g_nIFramRecvTimes;
    pOutBuf[bStep++] = sCrtlType12.crtltype12[0];
    pOutBuf[bStep++] = sCrtlType12.crtltype12[1];
    pOutBuf[bStep++] = sCrtlType34.crtltype34[0];
    pOutBuf[bStep++] = sCrtlType34.crtltype34[1];
    pOutBuf[bStep++] = nTI;
    pOutBuf[bStep++] = nVSQ;
    pOutBuf[bStep++] = nCOT&0xff;
    pOutBuf[bStep++] = nCOT>>8;
	pOutBuf[bStep++] = g_nPublicAddr&0xff;
    pOutBuf[bStep++] = g_nPublicAddr>>8;

    memcpy(&pOutBuf[bStep], pInObjBuff, nInObjLen); //数据移到信息体位置

	if(g_nIFramSendTimes <32767)
		g_nIFramSendTimes++;
	else
		g_nIFramSendTimes = 0;
    
	return len+2;
}

/*********************************************************************
所属功能单元：GB101规约处理模块
功能描述	：检测主站下发时钟的有效性
输入参数	：*ptime 输入的时钟数组，顺序为nn,hh,dd,mm,yy
输出参数	：无
函数返回值	：0表示错误，1表示正确
*******************************************************************
*/
uint8 time_check(uint8 *ptime)
{
	uint8 mm,hh,nn;

    nn = *ptime;
	hh = *(ptime+1);
	mm = *(ptime+3);
	if((nn>59)||(hh>59)||(mm>12))
		return 0;
	else
		return 1;
}


/*********************************************************************
所属功能单元：GB104规约处理模块
功能描述	：遥控执行逻辑
输入参数	：
输出参数	：  
函数返回值	：返回FALSE 操作失败
*******************************************************************/
uint8 control_operate(uint8 infolen, uint8 *pdata, uint8 btype)
{
	//判断遥控信息对象地址是否合法
	uint16 objaddr = 0;
	//uint8 bstate = 0;  //用于判断遥控分/合是否合法
	int i = 0;
	int ret = -1;
	
	objaddr = (*(uint16*)pdata);
	//bstate  = pdata[infolen];
	
	for(i = 0; i<g_nConfigYkNum; i++)
	{
		if(g_aYkDataConfig[i].nObjAddr == objaddr)
		{
			ret = 1;
			break;
		}
	}
	if(ret < 0)
	{
		FRAME104_FMT_TRACE("control_operate() 遥控点表未查到地址 objaddr=%04x \n", objaddr);
		return FALSE;
	}

	//判断当前状态是否可以预置/执行/撤销
    uint8 rtn = TRUE;

    switch(btype)
    {
        case CTRL_SELECT://-遥控预置-
        {
            rtn =  TRUE;
            g_nControlState = 0x55;
			g_nRemoteSetFlag = 1;
			g_nRemoteSetDelay = 0;
			FRAME104_FMT_TRACE("control_operate() 遥控预置接受! objaddr=%04x g_nControlState=%02x \n", objaddr, g_nControlState);
            break;
        }
        case CTRL_RUN://-遥控执行-
        {
            if(g_nControlState != 0x55)//-未预置，返回失败-
            {
                rtn =  FALSE;
				FRAME104_FMT_TRACE("control_operate() 遥控执行拒绝!请先预置! objaddr=%04x g_nControlState=%02x \n", objaddr, g_nControlState);
            }
            else
            {
                g_nControlState = 0xaa;
				g_nRemoteSetFlag = 0;
				g_nRemoteSetDelay = 0;
                rtn =  TRUE;
				FRAME104_FMT_TRACE("control_operate() 遥控执行接受! objaddr=%04x g_nControlState=%02x \n", objaddr, g_nControlState);
            }
            break;
        }
        case CTRL_STOP://-遥控撤销-
        {
            if(g_nControlState != 0x55)//-未预置，返回失败-
            {
                rtn =  FALSE;
				FRAME104_FMT_TRACE("control_operate() 遥控撤销拒绝!请先预置! objaddr=%04x g_nControlState=%02x \n", objaddr, g_nControlState);
            }
            else
            {
                g_nControlState = 0xaa;
				g_nRemoteSetFlag = 0;
				g_nRemoteSetDelay = 0;
                rtn = TRUE;
				FRAME104_FMT_TRACE("control_operate() 遥控撤销接受! objaddr=%04x g_nControlState=%02x \n", objaddr, g_nControlState);
            }
            break;
        }
        default:
            rtn = FALSE;
            break;
    }
    return rtn;
}

/*********************************************************************
所属功能单元：GB104规约处理模块
功能描述	：参数设置逻辑
输入参数	：
输出参数	：  
函数返回值	：返回FALSE 操作失败
*******************************************************************/
uint8 para_set_operate(uint8 infonum, uint8 *pdata, uint8 infolen, uint8 btype)
{
	int i = 0;
	uint8  index = 0;
	uint8  nObjNum = 0;  //信息体元素个数 
	nObjNum = infonum;  //预置：对应信息元素个数；固化/撤销：0x00
	uint16 nFixedAreaID = 0;  //定值区号
	uint8 nParaIden = 0;  //参数特征标识

	PARA_SET_VALUE_T  sParaSetValue;   //设置参数结构体
	memset(&sParaSetValue, 0, sizeof(PARA_SET_VALUE_T));
	
	//判断当前状态是否可以预置/执行/撤销
    uint8 rtn = TRUE;

	nFixedAreaID = pdata[index] + (pdata[index+1]<<8);
	index += 2;  //定值区号2字节
	nParaIden = pdata[index];
	index += 1;  //参数特征标识1字节

	if(nFixedAreaID > MAX_FIXED_AREA_NUM)
	{
		FRAME104_FMT_TRACE("para_set_operate() 定值区号错误! nFixedAreaID:%d \n", nFixedAreaID);
		return FALSE;
	}

    switch(btype)
    {
        case PARA_SELECT://-参数预置-
        {
            rtn =  TRUE;
			g_aFixedValueArea[nFixedAreaID].unParaIdentify.nParaIden = nParaIden;
			g_aFixedValueArea[nFixedAreaID].nSetState = 0x55;
			g_aFixedValueArea[nFixedAreaID].aSetPara[0] = 0x55;
			g_aFixedValueArea[nFixedAreaID].aSetPara[1] = nObjNum;
			memcpy(&g_aFixedValueArea[nFixedAreaID].aSetPara[2], &pdata[3], infolen-3);
			FRAME104_FMT_TRACE("para_set_operate() 参数预置接受! 定值区号nFixedAreaID=%d, 参数标识nParaIden=%02x, 当前状态nSetState=%02x 参数个数nObjNum:%d 参数报文: \n", 
				nFixedAreaID, nParaIden, g_aFixedValueArea[nFixedAreaID].nSetState, nObjNum);
			FRAME104_BUF_TRACE(&g_aFixedValueArea[nFixedAreaID].aSetPara[2], infolen-3, "报文: ");
            break;
        }
        case PARA_RUN://-参数固化-
        {
            if(g_aFixedValueArea[nFixedAreaID].nSetState != 0x55)//-未预置，返回失败-
            {
                rtn =  FALSE;
				FRAME104_FMT_TRACE("para_set_operate() 参数固化拒绝!请先预置! 定值区号nFixedAreaID=%d, 参数标识nParaIden=%02x, 当前状态nSetState=%02x \n", 
					nFixedAreaID, nParaIden, g_aFixedValueArea[nFixedAreaID].nSetState);
            }
            else
            {
            	nObjNum = g_aFixedValueArea[nFixedAreaID].aSetPara[1];
				index = 2;
				for(i=0; i<nObjNum; i++)
				{
					memset(&sParaSetValue, 0, sizeof(PARA_SET_VALUE_T));
					sParaSetValue.nObjAddr = g_aFixedValueArea[nFixedAreaID].aSetPara[index] + (g_aFixedValueArea[nFixedAreaID].aSetPara[index+1]<<8);  //信息体地址
					index += 3;  //信息体3字节，最高字节为0，index当前位置是tag
					sParaSetValue.nTagType = g_aFixedValueArea[nFixedAreaID].aSetPara[index];
					index += 1;  //tag 1字节
					sParaSetValue.nDataLen = g_aFixedValueArea[nFixedAreaID].aSetPara[index];
					index += 1;  //长度 1字节
					memcpy(&sParaSetValue.unValue, &g_aFixedValueArea[nFixedAreaID].aSetPara[index], sParaSetValue.nDataLen);
					index += sParaSetValue.nDataLen;  //数据内容 sParaSetValue.nDataLen个字节

					FRAME104_FMT_TRACE("para_set_operate() 参数固化解析 nObjNum:%d, i=%d, nObjAddr:%04x, nTagType=%d, nDataLen=%d, unValue= \n", 
						nObjNum, i, sParaSetValue.nObjAddr, sParaSetValue.nTagType, sParaSetValue.nDataLen);
					FRAME104_BUF_TRACE(&sParaSetValue.unValue.arrayV[0], sParaSetValue.nDataLen, "报文: ");
					
					para_data_write(nFixedAreaID, sParaSetValue);  //修改定值参数
				}
				
            	g_aFixedValueArea[nFixedAreaID].unParaIdentify.nParaIden = nParaIden;
                g_aFixedValueArea[nFixedAreaID].nSetState = 0xaa;
				g_nParaFixedFlag = 1;
                rtn =  TRUE;
				FRAME104_FMT_TRACE("para_set_operate() 参数固化接受! 定值区号nFixedAreaID=%d, 参数标识nParaIden=%02x, 当前状态nSetState=%02x \n", 
					nFixedAreaID, nParaIden, g_aFixedValueArea[nFixedAreaID].nSetState);
            }
            break;
        }
        case PARA_STOP://-参数预置撤销-
        {
            if(g_aFixedValueArea[nFixedAreaID].nSetState != 0x55)//-未预置，返回失败-
            {
                rtn =  FALSE;
				FRAME104_FMT_TRACE("para_set_operate() 参数取消预置拒绝!请先预置! 定值区号nFixedAreaID=%d, 参数标识nParaIden=%02x, 当前状态nSetState=%02x \n", 
					nFixedAreaID, nParaIden, g_aFixedValueArea[nFixedAreaID].nSetState);
            }
            else
            {
            	g_aFixedValueArea[nFixedAreaID].unParaIdentify.nParaIden = nParaIden;
                g_aFixedValueArea[nFixedAreaID].nSetState = 0xaa;
                rtn = TRUE;
				FRAME104_FMT_TRACE("para_set_operate() 参数取消预置接受! 定值区号nFixedAreaID=%d, 参数标识nParaIden=%02x, 当前状态nSetState=%02x \n", 
					nFixedAreaID, nParaIden, g_aFixedValueArea[nFixedAreaID].nSetState);
            }
            break;
        }
        default:
            rtn = FALSE;
            break;
    }
    return rtn;
}


/*           主站下发的tag格式类型，具体如下
名称		类型	标记（TAG）  长度(LENGTH)	 值（VALUE） 
布尔		Boolean 	1			1			1:true,0:false 
小整形		Tiny		43			 1			-128…127 
无符号小整形 UTiny	32	1			 0…255 
短整形		Short		33			2			 -32768…32767 
无符号短整形 UShort 	45			2			0…65535 
整形		Int 		2			4			 -2147483648…2147483648 
无符号整形	Uint		35			4			 0…4294967295 
长整形		Long		36			8			 -2^64…2^64 
无符号长整形 Ulong		37			8			 0…2^128-1 
单精度浮点	Float		38			4			 -2^128…2^128 
双精度浮点	Double	39			8			 -2^1024…2^1024 
八位位串类型 OcterString  4 			可变		OcterString 
字符串类型	String		4       可变        一个或者多个ASCII组成,最长64个字节，以'\0'结尾	
*/

int para_data_write(uint16 nFixArea, PARA_SET_VALUE_T sParaSetValue)
{
	int i = 0;
	if(nFixArea > MAX_FIXED_AREA_NUM)
	{
		FRAME104_FMT_TRACE("para_data_write() if(nFixArea > MAX_FIXED_AREA_NUM) ... \n");
		return -1;
	}

	switch(sParaSetValue.nObjAddr)
	{
		case 0x8001://配变终端型号
		case 0x8002://终端操作系统
		case 0x8003://终端制造商  	
		case 0x8004://终端硬件版本
		case 0x8005://终端软件版本 
		case 0x8006://终端软件版本校验码
		case 0x8007://终端通信规约类型 
		case 0x8008://终端出厂型号 
		case 0x8009://终端ID号
		case 0x800A://终端网卡MAC地址
		{
			//固有参数不可写
			FRAME104_FMT_TRACE("para_data_write() 固有参数不可写 ... \n");
			break;
		}
		case 0x8020://电流死区 
		case 0x8021://交流电压死区
		case 0x8022://直流电压死区
		case 0x8023://功率死区 
		case 0x8024://频率死区 
		case 0x8025://功率因数死区
		case 0x8026://PT一次额定 
		case 0x8027://PT二次额定
		case 0x8028://低电压报警门限值 
		case 0x8029://低电压报警周期 
		case 0x802A://过电压报警门限值 
		case 0x802B://过电压报警周期 
		case 0x802C://重载报警门限值 
		case 0x802D://重载报警周期 
		case 0x802E://过载报警门限值 
		case 0x802F://过载报警周期 
		case 0x8030://开入量采集防抖时间 
		case 0x8031://分闸输出脉冲保持时间 
		case 0x8032://合闸输出脉冲保持时间 
		case 0x8033://蓄电池自动活化周期 
		case 0x8034://蓄电池自动活化时刻
		case 0x8040://L01相CT一次额定 
		case 0x8041://L01相CT二次额定
		case 0x8042://L01零序CT一次额定 
		case 0x8043://L01零序CT二次额定 
		case 0x8050://L02相CT一次额定 
		case 0x8051://L02相CT二次额定 
		case 0x8052://L02零序CT一次额定 
		case 0x8053://L02零序CT二次额定
		case 0x8221://故障指示灯自动复归时间 
		case 0x8222://故障遥信保持时间 
		case 0x8224://X时间定值 
		case 0x8225://Y时间定值
		case 0x8226://C时间定值 
		case 0x8227://S时间定值 
		case 0x8228://单相接地跳闸时间
		case 0x8229://选线跳闸重合时间定值 
		case 0x822D://一次重合时间 
		case 0x822F://大电流闭重定值 
		case 0x8243://L01过流I段定值 
		case 0x8244://L01过流I段时间 
		case 0x8247://L01过流II段定值 
		case 0x8248://L01过流II段时间 
		case 0x824B://L01零序过流定值
		case 0x824C://L01零序过流时间 
		case 0x8263://L02过流I段定值 
		case 0x8264://L02过流I段时间 
		case 0x8267://L02过流II段定值 
		case 0x8268://L02过流II段时间 
		case 0x826B://L02零序过流定值 
		case 0x826C://L02零序过流时间
		{	
			for(i=0; i<g_nConfigParaNum; i++)
			{
				//FRAME104_FMT_TRACE("sParaSetValue.nObjAddr=%04x, %d; g_aParaSetValue[%d][%d].nObjAddr=%04x, %d \n", 
				//	sParaSetValue.nObjAddr, sParaSetValue.nObjAddr, nFixArea, i, g_aParaSetValue[nFixArea][i].nObjAddr, g_aParaSetValue[nFixArea][i].nObjAddr);
				if(sParaSetValue.nObjAddr == g_aParaSetValue[nFixArea][i].nObjAddr)
				{
					g_aParaSetValue[nFixArea][i].nTagType = sParaSetValue.nTagType;
					g_aParaSetValue[nFixArea][i].nDataLen = sParaSetValue.nDataLen;
					memcpy(&g_aParaSetValue[nFixArea][i].unValue, &sParaSetValue.unValue, sParaSetValue.nDataLen);
					g_aParaSetValue[nFixArea][i].bValid = TRUE;

					FRAME104_FMT_TRACE("para_data_write() nFixArea=%d, i=%d, nObjAddr=%04x, nTagType=%d, bValid=%d, nDataLen=%d, unValue= \n", 
						nFixArea, i, sParaSetValue.nObjAddr, sParaSetValue.nTagType, g_aParaSetValue[nFixArea][i].bValid, sParaSetValue.nDataLen);
					FRAME104_BUF_TRACE(&g_aParaSetValue[nFixArea][i].unValue.arrayV[0], sParaSetValue.nDataLen, "报文: ");

					break;
				}
			}
			break;
		}
		case 0x8220://故障指示灯自动复归投入 
		case 0x8223://首端FTU投入 
		case 0x822C://一次重合闸投退 
		case 0x822E://大电流闭锁重投退 
		case 0x8240://L01过流停电跳闸投退 
		case 0x8241://L01过流I段告警投退 
		case 0x8242://L01过流I段出口投退 
		case 0x8245://L01过流II段告警投退 
		case 0x8246://L01过流II段出口投退 
		case 0x8249://L01零序过流告警投退 
		case 0x824A://L01零序过流出口投退 
		case 0x824D://L01小电流接地告警投退 
		case 0x824E://L01小电流接地出口投退 
		case 0x8260://L02过流停电跳闸投退 
		case 0x8261://L02过流I段告警投退 
		case 0x8262://L02过流I段出口投退 
		case 0x8265://L02过流II段告警投退 
		case 0x8266://L02过流II段出口投退 
		case 0x822A://自适应相间短路故障处理投入 
		case 0x822B://自适应单相接地故障处理投入 
		case 0x8269://L02零序过流告警投退 
		case 0x826A://L02零序过流出口投退 
		case 0x826D://L02小电流接地告警投退 
		case 0x826E://L02小电流接地出口投退 
		{
			for(i=0; i<g_nConfigParaNum; i++)
			{
				//FRAME104_FMT_TRACE("sParaSetValue.nObjAddr=%04x, %d; g_aParaSetValue[%d][%d].nObjAddr=%04x, %d \n", 
				//	sParaSetValue.nObjAddr, sParaSetValue.nObjAddr, nFixArea, i, g_aParaSetValue[nFixArea][i].nObjAddr, g_aParaSetValue[nFixArea][i].nObjAddr);
				if(sParaSetValue.nObjAddr == g_aParaSetValue[nFixArea][i].nObjAddr)
				{
					g_aParaSetValue[nFixArea][i].nTagType = sParaSetValue.nTagType;
					g_aParaSetValue[nFixArea][i].nDataLen = sParaSetValue.nDataLen;
					memcpy(&g_aParaSetValue[nFixArea][i].unValue, &sParaSetValue.unValue, sParaSetValue.nDataLen);
					g_aParaSetValue[nFixArea][i].bValid = TRUE;

					FRAME104_FMT_TRACE("para_data_write() nFixArea=%d, i=%d, nObjAddr=%04x, nTagType=%d, bValid=%d, nDataLen=%d, unValue= \n", 
						nFixArea, i, sParaSetValue.nObjAddr, sParaSetValue.nTagType, g_aParaSetValue[nFixArea][i].bValid, sParaSetValue.nDataLen);
					FRAME104_BUF_TRACE(&g_aParaSetValue[nFixArea][i].unValue.arrayV[0], sParaSetValue.nDataLen, "报文: ");

					break;
				}
			}
			break;
		}
		
		default:
		{	
			FRAME104_FMT_TRACE("para_data_write() default ... \n");
			break;
		}
	}

	g_bFixedValueChanged = TRUE;
	return 1;
}

int para_data_read(uint16 nFixArea, PARA_SET_VALUE_T *sParaSetValue)
{
	int i = 0;
	if(nFixArea > MAX_FIXED_AREA_NUM)
	{
		return -1;
	}

	switch(sParaSetValue->nObjAddr)
	{
		case 0x8001://配变终端型号
		case 0x8002://终端操作系统
		case 0x8003://终端制造商  	
		case 0x8004://终端硬件版本
		case 0x8005://终端软件版本 
		case 0x8006://终端软件版本校验码
		case 0x8007://终端通信规约类型 
		case 0x8008://终端出厂型号 
		case 0x8009://终端ID号
		case 0x800A://终端网卡MAC地址
		{
			//固有参数暂不支持读
			break;
		}
		case 0x8020://电流死区 
		case 0x8021://交流电压死区
		case 0x8022://直流电压死区
		case 0x8023://功率死区 
		case 0x8024://频率死区 
		case 0x8025://功率因数死区
		case 0x8026://PT一次额定 
		case 0x8027://PT二次额定
		case 0x8028://低电压报警门限值 
		case 0x8029://低电压报警周期 
		case 0x802A://过电压报警门限值 
		case 0x802B://过电压报警周期 
		case 0x802C://重载报警门限值 
		case 0x802D://重载报警周期 
		case 0x802E://过载报警门限值 
		case 0x802F://过载报警周期 
		case 0x8030://开入量采集防抖时间 
		case 0x8031://分闸输出脉冲保持时间 
		case 0x8032://合闸输出脉冲保持时间 
		case 0x8033://蓄电池自动活化周期 
		case 0x8034://蓄电池自动活化时刻
		case 0x8040://L01相CT一次额定 
		case 0x8041://L01相CT二次额定
		case 0x8042://L01零序CT一次额定 
		case 0x8043://L01零序CT二次额定 
		case 0x8050://L02相CT一次额定 
		case 0x8051://L02相CT二次额定 
		case 0x8052://L02零序CT一次额定 
		case 0x8053://L02零序CT二次额定
		case 0x8221://故障指示灯自动复归时间 
		case 0x8222://故障遥信保持时间 
		case 0x8224://X时间定值 
		case 0x8225://Y时间定值
		case 0x8226://C时间定值 
		case 0x8227://S时间定值 
		case 0x8228://单相接地跳闸时间
		case 0x8229://选线跳闸重合时间定值 
		case 0x822D://一次重合时间 
		case 0x822F://大电流闭重定值 
		case 0x8243://L01过流I段定值 
		case 0x8244://L01过流I段时间 
		case 0x8247://L01过流II段定值 
		case 0x8248://L01过流II段时间 
		case 0x824B://L01零序过流定值
		case 0x824C://L01零序过流时间 
		case 0x8263://L02过流I段定值 
		case 0x8264://L02过流I段时间 
		case 0x8267://L02过流II段定值 
		case 0x8268://L02过流II段时间 
		case 0x826B://L02零序过流定值 
		case 0x826C://L02零序过流时间
		{	
			for(i=0; i<g_nConfigParaNum; i++)
			{
				if(sParaSetValue->nObjAddr == g_aParaSetValue[nFixArea][i].nObjAddr)
				{
					memcpy(sParaSetValue, &g_aParaSetValue[nFixArea][i], sizeof(PARA_SET_VALUE_T));
				}
				break;
			}
			break;
		}
		case 0x8220://故障指示灯自动复归投入 
		case 0x8223://首端FTU投入 
		case 0x822C://一次重合闸投退 
		case 0x822E://大电流闭锁重投退 
		case 0x8240://L01过流停电跳闸投退 
		case 0x8241://L01过流I段告警投退 
		case 0x8242://L01过流I段出口投退 
		case 0x8245://L01过流II段告警投退 
		case 0x8246://L01过流II段出口投退 
		case 0x8249://L01零序过流告警投退 
		case 0x824A://L01零序过流出口投退 
		case 0x824D://L01小电流接地告警投退 
		case 0x824E://L01小电流接地出口投退 
		case 0x8260://L02过流停电跳闸投退 
		case 0x8261://L02过流I段告警投退 
		case 0x8262://L02过流I段出口投退 
		case 0x8265://L02过流II段告警投退 
		case 0x8266://L02过流II段出口投退 
		case 0x822A://自适应相间短路故障处理投入 
		case 0x822B://自适应单相接地故障处理投入 
		case 0x8269://L02零序过流告警投退 
		case 0x826A://L02零序过流出口投退 
		case 0x826D://L02小电流接地告警投退 
		case 0x826E://L02小电流接地出口投退 
		{
			for(i=0; i<g_nConfigParaNum; i++)
			{
				if(sParaSetValue->nObjAddr == g_aParaSetValue[nFixArea][i].nObjAddr)
				{
					memcpy(sParaSetValue, &g_aParaSetValue[nFixArea][i], sizeof(PARA_SET_VALUE_T));
				}
				break;
			}
			break;
		}			
		
		default:
		{	
			break;
		}
	}
	
	return 1;
}

/*******************************************************************************
* 函数名称: i_frame_data_unpack
* 函数功能: 104完整报文详细解帧
* 输入参数: uint16 nFrameLen  主站报文长度
            uint8 *pFrameBuff 主站报文内容
* 输出参数: uint8 *pOutBuf     应答报文内容
            uint8 *pAppType    主站报文类型
* 返 回 值: 应答报文长度
*******************************************************************************/
int i_frame_data_unpack(uint16 nFrameLen, uint8 *pFrameBuff, uint8 *pOutBuf, uint8 *pAppType)
{
	int nOutbufLen = 0;//返回的发送数据长度
	int    result = 0;
	
	//uint8 nSendTI = 0;
	//uint8 nSendVSQ = 0;
	uint16 nSendCOT = 0;
	uint8 nSendInObjLen = 0;
	uint8 pBackInObjBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //用于备份
	uint8 pSendInObjBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //用于组发送报文

	TOTAL_104_FRAME_T stTotal104Frame;
	memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
	memcpy(&stTotal104Frame, pFrameBuff, nFrameLen);

	*pAppType = APP_TYPE_NOMMAL;

	if(stTotal104Frame.nLength < CTRL_UNIT_LABEL_LEN)
	{
		FRAME104_FMT_TRACE("i_frame_data_unpack() if(stTotal104Frame.nLength < CTRL_UNIT_LABEL_LEN) nFrameLen:%d \n", stTotal104Frame.nLength);
		return -1;
	}

	nSendInObjLen = stTotal104Frame.nLength - CTRL_UNIT_LABEL_LEN;  //信息体长度
	memcpy(pBackInObjBuff, &pFrameBuff[APCI_UNIT_LABEL_LEN], nSendInObjLen);  //拷贝信息体内容
	memcpy(pSendInObjBuff, &pFrameBuff[APCI_UNIT_LABEL_LEN], OBJ_ADDR_LEN);  //拷贝信息体地址内容

	switch (stTotal104Frame.nDataType)
	{
		case C_SE_NA_1://设定值命令, 规一化值  48,需要进一步判断信息体地址
		case C_SE_NB_1://设定值命令, 标度化值  49,需要进一步判断信息体地址
		case C_SE_NC_1://设定值命令, 短浮点数  50,需要进一步判断信息体地址
		case C_BO_NA_1://32比特串  51
		case C_SE_TA_1://带CP56Time2a时标的设定值命令, 规一化值,104特有,61
		case C_SE_TB_1://带CP56Time2a时标的设定值命令, 标度化值,104特有,62
		case C_SE_TC_1://带CP56Time2a时标的设定值命令, 短浮点数,104特有,63
		case C_BO_TA_1://带CP56Time2a时标的32比特串, 104特有, 64
		{
			//这些命令目前还不知道设置的数据起什么作用，需要自定义，暂时未使用
			if(stTotal104Frame.nTransCauseL == CAUSE_ACT)
			{
				nSendCOT = CAUSE_ACTCON;
			}
			else if(stTotal104Frame.nTransCauseL == CAUSE_DEACT)
			{
				nSendCOT = CAUSE_DEACTCON;
			}
			else 
			{
				nSendCOT  =  ERROR_CAUSE|0x40;
			}
			nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT, pSendInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}
		case C_IC_NA_1://总召唤 100,激活或停止激活
		{
			if(stTotal104Frame.nTransCauseL == CAUSE_ACT)
			{
				nSendCOT = CAUSE_ACTCON;
				pSendInObjBuff[OBJ_ADDR_LEN] = 0x14;
				nSendInObjLen = OBJ_ADDR_LEN + 1;
			}
			else if(stTotal104Frame.nTransCauseL == CAUSE_DEACT)
			{
				nSendCOT = CAUSE_DEACTCON;
				pSendInObjBuff[OBJ_ADDR_LEN] = 0x14;
				nSendInObjLen = OBJ_ADDR_LEN + 1;
				g_bAllDataCallFlag = 0;//总召唤结束
			}
			else 
			{
				nSendCOT  =  ERROR_CAUSE|0x40;
			}
			nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT, pSendInObjBuff, nSendInObjLen, pOutBuf);
			if(nOutbufLen > 0)
			{
				g_bAllDataCallFlag =  ((nSendCOT&0xff) == CAUSE_ACTCON ? 1 : 0);//总召唤激活
			}
			g_sTaskReportPipe.nAllDataCallPipe = g_nGlobalPipe;
			break;
		}
		case C_CI_NA_1://电能量召唤命令 101
		{
			pSendInObjBuff[0] = 0;
			pSendInObjBuff[1] = 0;
			pSendInObjBuff[2] = CAUSE_INTROGEN;
			nSendInObjLen = 3;
			if(stTotal104Frame.nTransCauseL == CAUSE_ACT)
			{
				nSendCOT = CAUSE_ACTCON;
			}
			else 
			{
				//返回未知的传送原因
				nSendCOT = ERROR_CAUSE|0x0040;
			}
			nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT, pSendInObjBuff, nSendInObjLen, pOutBuf);
			if((nOutbufLen > 0)&&((nSendCOT&0xff) == CAUSE_ACTCON))
			{
				g_nElecDataCallFlag = pBackInObjBuff[OBJ_ADDR_LEN]+1;
			}
			g_sTaskReportPipe.nEnDataCallPipe = g_nGlobalPipe;
			break;
		}
		case C_CS_NA_1://时钟同步 103
		{
			/*
			struct tm
			 {
			   int tm_sec;           //Seconds. [0-60] (1 leap second)
			   int tm_min;           //Minutes. [0-59]
			   int tm_hour;          //Hours.   [0-23]
			   int tm_mday;          //Day.     [1-31]
			   int tm_mon;           //Month.   [0-11]
			   int tm_year;          //Year - 1900.
			   int tm_wday;          //Day of week. [0-6]
			   int tm_yday;          //Days in year.[0-365]
			   int tm_isdst;         //DST.     [-1/0/1]
			}
			*/
			uint8 bfgTimeErr = 0;
			int nSetResult = 0;
			if(stTotal104Frame.nTransCauseL == CAUSE_ACT)
			{
				bfgTimeErr = time_check(&pBackInObjBuff[OBJ_ADDR_LEN + 2]);//时钟有效性判断
				if(bfgTimeErr)//时钟格式正确
				{
					struct tm	tMetertime;
					time_t		MeterSec;
					struct		timeval    tv;
					
					tMetertime.tm_year = pBackInObjBuff[OBJ_ADDR_LEN+6]+100;//年
					tMetertime.tm_mon  = pBackInObjBuff[OBJ_ADDR_LEN+5]-1;//月
					tMetertime.tm_mday = pBackInObjBuff[OBJ_ADDR_LEN+4]&0x1f;//日
					tMetertime.tm_hour = pBackInObjBuff[OBJ_ADDR_LEN+3];//时
					tMetertime.tm_min  = pBackInObjBuff[OBJ_ADDR_LEN+2];//分
					tMetertime.tm_sec  = (pBackInObjBuff[OBJ_ADDR_LEN] + (pBackInObjBuff[OBJ_ADDR_LEN+1]<<8))/1000;//秒
					MeterSec = mktime(&tMetertime);
					tv.tv_sec = MeterSec;
					tv.tv_usec = ((pBackInObjBuff[OBJ_ADDR_LEN] + (pBackInObjBuff[OBJ_ADDR_LEN+1]<<8))%1000)*1000;
					nSetResult = settimeofday(&tv, NULL);
					FRAME104_FMT_TRACE("i_frame_data_unpack() tv.tv_sec=%ld, tv.tv_usec=%ld, 结果nSetResult=%d \n",tv.tv_sec, tv.tv_usec, nSetResult);
					nSendCOT = CAUSE_ACTCON;
					g_bTimerSynFlag = 1; //已时钟同步标志
					memcpy(pSendInObjBuff, pBackInObjBuff, nSendInObjLen);
					nSendInObjLen = OBJ_ADDR_LEN + 7;
				}
				else
				{
					nSendCOT = CAUSE_ACTCON|0x40;
				}
				
			}
			else 
			{
				//返回未知的传送原因
				nSendCOT = ERROR_CAUSE|0x0040;
			}
			nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT,  pSendInObjBuff, nSendInObjLen, pOutBuf);
			g_sTaskReportPipe.nTimeSynPipe = g_nGlobalPipe;
			break;
		}
        case C_SC_NA_1://单点命令
        {            
            if(stTotal104Frame.nTransCauseL == CAUSE_ACT)//遥控预置/执行
			{
			    if(pBackInObjBuff[OBJ_ADDR_LEN]&0x80)//遥控预置
                {
                    result = control_operate(OBJ_ADDR_LEN, pBackInObjBuff, CTRL_SELECT);
					pSendInObjBuff[OBJ_ADDR_LEN] = pBackInObjBuff[OBJ_ADDR_LEN]|0x80;
					*pAppType = APP_TYPE_CTRL_SELECT_ACK;//遥控选择确认
                }      
                else//遥控执行
                {
                    result = control_operate(OBJ_ADDR_LEN, pBackInObjBuff, CTRL_RUN);
					pSendInObjBuff[OBJ_ADDR_LEN] = pBackInObjBuff[OBJ_ADDR_LEN]&0x7f;
					if(TRUE == result)
					{
						g_aCtrlReSendBuff[0] = 0x55;
						if(nFrameLen < MAX_YK_BUFF_LEN)
						{
							memcpy((void*)&g_aCtrlReSendBuff[1],pFrameBuff,nFrameLen);
						}
						else
						{
							memcpy((void*)&g_aCtrlReSendBuff[1],pFrameBuff,MAX_YK_BUFF_LEN-1);//防止数据溢出
						}
					}
                }
                if(TRUE == result)
            	{
				    nSendCOT = CAUSE_ACTCON;
            	}
                else
            	{
                    nSendCOT = CAUSE_ACTCON|0x40;
            	}
			}
			else if(stTotal104Frame.nTransCauseL == CAUSE_DEACT)//遥控撤销
			{
			    result = control_operate(OBJ_ADDR_LEN, pBackInObjBuff, CTRL_STOP);
				pSendInObjBuff[OBJ_ADDR_LEN] = pBackInObjBuff[OBJ_ADDR_LEN];
				if(result == TRUE)
                {
					nSendCOT = CAUSE_DEACTCON;
				}
                else
                {
					nSendCOT = CAUSE_DEACTCON|0x40;
				}
			}
			else 
			{
				nSendCOT  =  ERROR_CAUSE|0x40;
			}
			nSendInObjLen = OBJ_ADDR_LEN + 1;
			nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT,  pSendInObjBuff, nSendInObjLen, pOutBuf);
            g_sTaskReportPipe.nRemoteCtrlAckPipe = g_nGlobalPipe;
			break;
        }
		case C_DC_NA_1:  //双点命令
		{
			nSendCOT  =  ERROR_TYPE|0x40;//未知的数据标识，且是否认回答
			nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT, pSendInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}			
		case C_TS_NA_1://测试命令
		{
			if(stTotal104Frame.nTransCauseL == CAUSE_ACT)
			{
				nSendCOT = CAUSE_ACTCON;
			}
			else 
			{
				//返回未知的传送原因
				nSendCOT = ERROR_CAUSE|0x0040;
			}
			nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT, pBackInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}
		case C_RP_NA_1://复位进程命令105 初始化
		{
			if(stTotal104Frame.nTransCauseL == CAUSE_ACT)
			{
				nSendCOT = CAUSE_ACTCON;
				g_nRtuResetFlag = pBackInObjBuff[OBJ_ADDR_LEN];  //1全复位，清soe队列及遥测数据;2只清soe事件队列;4终端停电复位;8终端本地复位
			}
			else 
			{
				//返回未知的传送原因
				nSendCOT = ERROR_CAUSE|0x0040;
			}
			nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT,  pSendInObjBuff, nSendInObjLen, pOutBuf);
			g_sTaskReportPipe.nProcessResetPipe = g_nGlobalPipe;
			break;
		}
		case C_TS_TA_1://带CP56Time2a时标的测试命令,104特有,107
		{
			if(stTotal104Frame.nTransCauseL == CAUSE_ACT)
			{
				nSendCOT = CAUSE_ACTCON;
			}
			else 
			{
				//返回未知的传送原因
				nSendCOT = ERROR_CAUSE|0x0040;
			}
			nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT,  pSendInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}
		case P_ME_NA_1://测量值参数, 规一化值,需要进一步判断信息体地址
		case P_ME_NC_1://测量值参数, 短浮点数,需要进一步判断信息体地址
		case P_ME_NB_1://测量值参数, 标度化值,需要进一步判断信息体地址
		{			
			nSendCOT  =  ERROR_TYPE|0x40;//未知的数据标识，且是否认回答
			nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT, pSendInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}
		case P_AC_NA_1://参数激活，需要进一步判断激活判定词
		{
			nSendCOT  =  ERROR_TYPE|0x40;//未知的数据标识，且是否认回答
			nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT, pSendInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}
		//国网2019年2月发布最新规范
		case C_SR_NA_1: //200，网最新实施细则参数切换类型标识
		{ 
			nSendCOT  =  ERROR_TYPE|0x40;//未知的数据标识，且是否认回答
		    nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT,  pSendInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}

		case C_RR_NA_1:  //201,最新实施细则参数读定值区号类型标识，pjhtest
		{ 
			nSendCOT  =  ERROR_TYPE|0x40;//未知的数据标识，且是否认回答
		    nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT,  pSendInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}
		case C_RS_NA_1:  //202,最新实施细则读单个或多个定值/参数类型标识，pjhtest
		{
			nSendCOT  =  ERROR_TYPE|0x40;//未知的数据标识，且是否认回答
		    nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT,  pSendInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}
		case C_WS_NA_1:  //203,最新实施细则写单个或多个定值/参数类型标识，pjhtest
		{
			uint8  nObjNum = 0;  //信息体元素个数
			FIXED_AREA_IDEN_T sFixedAreaIden;  //定值区特征标识结构体
			PARA_SET_VALUE_T  sParaSetValue;   //设置参数结构体
			memset(&sFixedAreaIden, 0, sizeof(FIXED_AREA_IDEN_T));
			memset(&sParaSetValue, 0, sizeof(PARA_SET_VALUE_T));

			memcpy(pSendInObjBuff, pBackInObjBuff, nSendInObjLen);  //定值区号2字节，参数标识1字节，后面是信息体内容

			if((stTotal104Frame.nTransCauseL&0xff) == CAUSE_ACT)//参数预置/固化
			{
				if(pSendInObjBuff[2]&0x80)//预置
				{
					nObjNum = stTotal104Frame.nVSQ;  //预置：对应信息元素个数；固化/撤销：0x00
					result = para_set_operate(nObjNum, pBackInObjBuff, nSendInObjLen, PARA_SELECT);
					*pAppType = APP_TYPE_CTRL_SELECT_ACK;//参数预置确认
				}
				else//固化
				{
					nObjNum = stTotal104Frame.nVSQ;  //预置：对应信息元素个数；固化/撤销：0x00
					result = para_set_operate(nObjNum, pBackInObjBuff, nSendInObjLen, PARA_RUN);
				}

				if(TRUE == result)
				{
					nSendCOT = CAUSE_ACTCON;
				}
				else
				{
					nSendCOT = CAUSE_ACTCON|0x40;
				}
			}
			else if((stTotal104Frame.nTransCauseL&0xff) == CAUSE_DEACT)//参数预置撤销
			{
				if(pSendInObjBuff[2]&0x40)//取消预置
				{
					nObjNum = stTotal104Frame.nVSQ;  //预置：对应信息元素个数；固化/撤销：0x00
					result = para_set_operate(nObjNum, pBackInObjBuff, nSendInObjLen, PARA_STOP);
				}

				if(result == TRUE)
                {
					nSendCOT = CAUSE_DEACTCON;
				}
                else
                {
					nSendCOT = CAUSE_DEACTCON|0x40;
				}
			}
			else//未知的COT
			{
				nSendCOT  =  ERROR_CAUSE|0x40;
			}
			
		    nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT,  pSendInObjBuff, nSendInObjLen, pOutBuf);

			break;
		}

		case M_IT_NB_1://206,最新实施细则累计量，短浮点数
		{
			nSendCOT  =  ERROR_TYPE|0x40;//未知的数据标识，且是否认回答
		    nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT,  pSendInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}
		case M_IT_TC_1://207 国网最新实施细则带CP56Time2a时标的累计量，短浮点数
		{
			nSendCOT  =  ERROR_TYPE|0x40;//未知的数据标识，且是否认回答
		    nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT,  pSendInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}

		case F_SC_NA_2://210  ,最新实施细则文件操作类型标识
		{
			nSendCOT  =  ERROR_TYPE|0x40;//未知的数据标识，且是否认回答
		    nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT,  pSendInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}
		case F_SR_NA_1_N://211,  网最新实施细则软件升级启动/结束类型标识
		{
			nSendCOT  =  ERROR_TYPE|0x40;//未知的数据标识，且是否认回答
		    nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT,  pSendInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}
		//end of 国网2019年2月发布最新规范
		
		default:
		{
			nSendCOT  =  ERROR_TYPE|0x40;//未知的数据标识，且是否认回答
			nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT, pSendInObjBuff, nSendInObjLen, pOutBuf);
			break;
		}
	}

	return nOutbufLen;
}

/*******************************************************************************
* 函数名称: frame_data_check
* 函数功能: 104完整报文检查
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
int frame_data_check(uint16 nFrameLen, uint8 *pFrameBuff)
{
	if(nFrameLen < 6 || nFrameLen > 255)
	{
		FRAME104_FMT_TRACE("frame_data_check() if(nFrameLen < 6 || nFrameLen > 255) nFrameLen:%d \n", nFrameLen);
		return -1;
	}

	if(pFrameBuff[0] != 0x68)
	{
		FRAME104_FMT_TRACE("frame_data_check() if(pFrameBuff[0] != 0x68) pFrameBuff[0]:%02x \n", pFrameBuff[0]);
		return -1;
	}

	if(pFrameBuff[1] < 4 || pFrameBuff[1] > 253)
	{
		FRAME104_FMT_TRACE("frame_data_check() if(pFrameBuff[1] < 4 || pFrameBuff[1] > 253) pFrameBuff[1]:%d \n", pFrameBuff[1]);
		return -1;
	}

	return 1;
}


/*******************************************************************************
* 函数名称: frame_data_unpack
* 函数功能: 104完整报文解帧
* 输入参数: uint16 nFrameLen  主站报文长度
            uint8 *pFrameBuff 主站报文内容
* 输出参数: uint8 *pOutBuf     应答报文内容
            uint8 *pAppType    主站报文类型
* 返 回 值: 应答报文长度
*******************************************************************************/
int frame_data_unpack(uint16 nFrameLen, uint8 *pFrameBuff, uint8 *pOutBuf, uint8 *pAppType)
{
	int nOutbufLen = 0;//返回的发送数据长度
	static uint8  bErrRecvSend = 0;
	
	TOTAL_104_FRAME_T stTotal104Frame;
	memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));	

	g_nIFrameT3Count = 0;  //收到任何帧都清链路间隔测试时间

	//报文检查
	if(frame_data_check(nFrameLen, pFrameBuff) < 0)
	{
		return -1;
	}
	
	memcpy(&stTotal104Frame, pFrameBuff, nFrameLen);
	frame_data_print(stTotal104Frame);

	g_nFrameErrTimes = 0;  //帧序号错误次数
	if(stTotal104Frame.sCrtlType12.ibit.fir == 0)//I帧
	{
		g_nRcvIFramSendNum   = stTotal104Frame.sCrtlType12.ibit.framenum;//发送序号
		g_nRcvFramRcvNum     = stTotal104Frame.sCrtlType34.rbit.framenum;//接收序号

		g_nIFrameT1Count = 0;  //t1清零
		
		if((g_nRcvFramRcvNum == g_nIFramSendTimes)&&(g_nRcvIFramSendNum == g_nIFramRecvTimes))
		{
			g_nNoneActIFramTimes = 0;
			if(bErrRecvSend)
			{
				FRAME104_FMT_TRACE("frame_data_unpack() I帧序号错误后得到主站重新确认 RcvIFramSendNum = %d, IFramRcvTimes = %d, RcvFramRcvNum = %d, IFramSendTimes = %d \n", 
						g_nRcvIFramSendNum, g_nIFramRecvTimes, g_nRcvFramRcvNum, g_nIFramSendTimes);
				bErrRecvSend = 0;
			}
		}
		else
		{
			bErrRecvSend = 1;
			if(g_nRcvIFramSendNum > g_nIFramRecvTimes)//接收到的帧序号错误，按主站帧序号继续通讯
			{
				//丢帧情况下，不主动断链，继续通讯
                g_nNoneActIFramTimes = g_nRcvIFramSendNum - g_nIFramRecvTimes;
                FRAME104_FMT_TRACE("I 帧确认, 丢失接收帧, 计算丢失帧计数 RcvIFramSendNum = %d, IFramRcvTimes = %d, NoneActIFramTimes = %d \n",
					g_nRcvIFramSendNum,g_nIFramRecvTimes,g_nNoneActIFramTimes);
			}
			else if(g_nRcvFramRcvNum < g_nIFramSendTimes)
			{
				//丢帧情况下，不主动断链，继续通讯
			    g_nNoneActIFramTimes = g_nIFramSendTimes - g_nRcvFramRcvNum;
				FRAME104_FMT_TRACE("I 帧确认, 丢失发送帧, 计算丢失帧计数 RcvFramRcvNum = %d, IFramSendTimes = %d, NoneActIFramTimes = %d \n",
					g_nRcvFramRcvNum,g_nIFramSendTimes,g_nNoneActIFramTimes);
			}
		}

		if(g_nIFramRecvTimes < 32767)
		{
			g_nIFramRecvTimes++;
		}
		else
		{
			g_nIFramRecvTimes = 0;
		}
			
		g_nIFrameT2Count = MAX_FRAMCOM_DELAY_T2;  //t2计时开始
		
		if((g_nIFramRecvTimes%IFRAMCOM_TIMES_MAX_W) == 0)//每收到IFRAMCOM_TIMES_MAX_W个I帧，返回一个S确认帧
		{
			g_bAckRecvFlag = 1;
		}

		//详细解帧
        #ifdef AREA_ZHEJIANG
        nOutbufLen = i_frame_data_unpack_zhj(nFrameLen, pFrameBuff, pOutBuf, pAppType);
        #else
        nOutbufLen = i_frame_data_unpack(nFrameLen, pFrameBuff, pOutBuf, pAppType);
        #endif
        
	}
	else if(stTotal104Frame.sCrtlType12.sbit.fir == 1)//S帧
	{
		g_nRcvFramRcvNum     = stTotal104Frame.sCrtlType34.rbit.framenum;//接收序号

		g_nIFrameT1Count = 0;  //t1清零
		
		if(g_nRcvFramRcvNum == g_nIFramSendTimes)
		{
			g_nNoneActIFramTimes = 0;
			nOutbufLen =  0;
			
			if(bErrRecvSend)
			{
				FRAME104_FMT_TRACE("frame_data_unpack() S帧序号错误后得到主站重新确认 RcvIFramSendNum = %d, IFramRcvTimes = %d, RcvFramRcvNum = %d, IFramSendTimes = %d \n", 
						g_nRcvFramRcvNum, g_nIFramSendTimes, g_nRcvIFramSendNum, g_nIFramRecvTimes);
				bErrRecvSend = 0;
			}
		}
		else
		{
			bErrRecvSend = 1;

			//丢帧情况下，不主动断链，继续通讯
		    g_nNoneActIFramTimes = g_nIFramSendTimes - g_nRcvFramRcvNum;
			FRAME104_FMT_TRACE("S 帧确认, 丢失发送帧, 计算丢失帧计数 RcvFramRcvNum = %d, IFramSendTimes = %d, NoneActIFramTimes = %d \n",
				g_nRcvFramRcvNum,g_nIFramSendTimes,g_nNoneActIFramTimes);
		}
		
	}
	else if(stTotal104Frame.sCrtlType12.ubit.fir == 3)//U帧
	{
		g_nIFrameT1Count = 0;  //t1清零

		if(stTotal104Frame.sCrtlType12.ubit.testcon == 1)//U帧链路测试命令 确认帧
		{	
			nOutbufLen  = 0;
			FRAME104_FMT_TRACE("收到U帧链路测试命令 确认帧... \n");
		}
		else if((stTotal104Frame.sCrtlType12.ubit.staact == 1)||(stTotal104Frame.sCrtlType12.ubit.stacon == 1))//U帧链路启动命令，启动应用层数据发送，帧序号复位
		{
			g_nIFramSendTimes = 0;
			g_nIFramRecvTimes  = 0;
			
            if(stTotal104Frame.sCrtlType12.ubit.stacon == 1)
            {
            	nOutbufLen  = 0;
				FRAME104_FMT_TRACE("收到U帧链路启动命令 确认帧... \n");
			}
			
            if(stTotal104Frame.sCrtlType12.ubit.staact == 1)
            {
				nOutbufLen = u_frame_data_pack(0x08, pOutBuf);
				FRAME104_FMT_TRACE("收到U帧链路启动命令 生效帧... \n");
                g_bInitEndFlag = 1;
			}
			
			g_bDataSendFlag = 1;
			FRAME104_FMT_TRACE("启动应用层数据发送，帧序号清零复位... \n");
		}
		else if(stTotal104Frame.sCrtlType12.ubit.stoact == 1)//U帧链路终止命令，关闭应用层数据发送
		{
			nOutbufLen = u_frame_data_pack(0x20, pOutBuf);
			g_bDataSendFlag = 0;
			FRAME104_FMT_TRACE("收到U帧链路终止命令 生效帧...主站停止终端发送报文... \n");
		}
		else if(stTotal104Frame.sCrtlType12.ubit.testact == 1)//U帧链路测试帧，测试链路
		{			
			nOutbufLen = u_frame_data_pack(0x80, pOutBuf);
			FRAME104_FMT_TRACE("收到U帧链路测试命令 生效帧... \n");
		}
	}

	return nOutbufLen;
}


/*******************************************************************************
* 函数名称: energy_cyc_send
* 函数功能: 发电量循环上送
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void energy_cyc_send()
{
	
}

/*******************************************************************************
* 函数名称: yc_cyc_send
* 函数功能: 遥测循环上送
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yc_cyc_send()
{

}

/*******************************************************************************
* 函数名称: back_scan_send
* 函数功能: 背景扫描上送
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void back_scan_send()
{

}

/*******************************************************************************
* 函数名称: timing_active_send
* 函数功能: 系统定时任务，104归约处理背景扫描或循环上报数据
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void timing_active_send()
{
	if(g_bDataSendFlag == 2)
	{
		if(g_unPastTimeFlag.BitSect.Second == TRUE)
		{
			//循环上送遥测
			if(g_bYcCycSendFlag)
			{
				yc_cyc_send();
			}

			//循环上送发电量
			if(g_bEnergyCycSendFlag)
			{
				energy_cyc_send();
			}

			//背景扫描
			if(g_bBackScanSendFlag)
			{
				back_scan_send();
			}
		}
	}
	
}

/*******************************************************************************
* 函数名称: yk_state_cyc_check
* 函数功能: 遥控状态循环检测
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yk_state_cyc_check()
{
	//遥控预置标志置位，如果超过60秒未执行遥控执行，取消预置
	if(g_nRemoteSetFlag == 1)
	{
		g_nRemoteSetDelay++;
		if(g_nRemoteSetDelay > 60)
		{
			g_nRemoteSetFlag = 0;
			g_nRemoteSetDelay = 0;
			g_nControlState = 0xaa;

			FRAME104_FMT_TRACE("yk_state_cyc_check() 遥控预置等待超时! g_nControlState=%02x \n", g_nControlState);
		}
	}
}

/*******************************************************************************
* 函数名称: t1_t2_t3_cyc_check
* 函数功能: 三个超时参数循环检测
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void t1_t2_t3_cyc_check()
{
	int nOutbufLen = 0;//返回的发送数据长度
	uint8 pSendInObjBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //用于组发送报文
	uint8 nPipeIndex = g_nGlobalPipe;  //收发通道号
	uint8  nAppType = APP_TYPE_NOMMAL;  //主要用于加密类型
	uint8	SendBufSecure[1400] = {0};  //加密之后的发送缓冲区
	int32	SendLenSecure = 0;			//加密之后的发送数据长度
	
	//接收端t2时间判断
	if(g_nIFrameT2Count > 0)
    {
        g_nIFrameT2Count--;
        if(g_nIFrameT2Count == 0)//收到I帧未到最大数量，但已超过延时时间，回S帧确认回答
        {
            g_bAckRecvFlag = 1;
        }
    }

	//发送端t1时间判断
	if(g_nIFrameT1Count > 0)
    {
    	//超过延时时间未收到确认帧，发生链路测试
        g_nIFrameT1Count--;
        if(g_nIFrameT1Count <= 0)
        {
            FRAME104_FMT_TRACE("未接收到任何确认I帧超时...\n");
			nOutbufLen = u_frame_data_pack(0x40, pSendInObjBuff);
			if(nOutbufLen > 0)
			{
				//记录报文
				FRAME104_FMT_TRACE("T1检查判断 上报 t1_t2_t3_cyc_check() nOutbufLen:%d, nPipeIndex:%d \n", nOutbufLen, nPipeIndex);
				FRAME104_BUF_TRACE(pSendInObjBuff, nOutbufLen, "报文: ");

				//写共享的发送缓冲区
				SendLenSecure = security_packappframe(nPipeIndex, nAppType, pSendInObjBuff, nOutbufLen, SendBufSecure);  //调用加密接口
				if(SendLenSecure > 0)
				{
					advPd_send_104msg(nPipeIndex, PRO_DATA_TRANSMIT, nAppType, SendBufSecure, SendLenSecure);  //调用发送接口
				}

				//清接收缓冲区
				memset(pSendInObjBuff, 0, sizeof(pSendInObjBuff));
				nOutbufLen = 0;
				memset(SendBufSecure, 0, sizeof(SendBufSecure));
				SendLenSecure = 0;
			}
        }
    }

	//超过MAX_FRAMCOM_DELAY_T2延时时间无通讯，发送链路测试帧
    if(g_bNetTestFlag == 1)//控制站未发起链路测试的情况下，主动发送链路测试帧
    {
        g_nIFrameT3Count++;
        if(g_nIFrameT3Count > MAX_FRAMCOM_DELAY_T3)
        {
            g_nIFrameT1Count = MAX_FRAMCOM_DELAY_T1;
            g_nIFrameT3Count = 0; 
            nOutbufLen = u_frame_data_pack(0x40, pSendInObjBuff);
			if(nOutbufLen > 0)
			{
				//记录报文
				FRAME104_FMT_TRACE("T2检查判断 上报 t1_t2_t3_cyc_check() nOutbufLen:%d, nPipeIndex:%d \n", nOutbufLen, nPipeIndex);
				FRAME104_BUF_TRACE(pSendInObjBuff, nOutbufLen, "报文: ");

				//写共享的发送缓冲区
				SendLenSecure = security_packappframe(nPipeIndex, nAppType, pSendInObjBuff, nOutbufLen, SendBufSecure);  //调用加密接口
				if(SendLenSecure > 0)
				{
					advPd_send_104msg(nPipeIndex, PRO_DATA_TRANSMIT, nAppType, SendBufSecure, SendLenSecure);  //调用发送接口
				}
				
				//清接收缓冲区
				memset(pSendInObjBuff, 0, sizeof(pSendInObjBuff));
				nOutbufLen = 0;
				memset(SendBufSecure, 0, sizeof(SendBufSecure));
				SendLenSecure = 0;
			}
            return;
        }
    }
}

/*******************************************************************************
* 函数名称: yc_report_data_clear
* 函数功能: 初始化遥测数据
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yc_report_data_clear()
{
	int i = 0;
	for(i=0; i<g_nConfigYcNum; i++)
	{
		memset((void*)&g_aYcDataSend[i].fNewValue, 0, sizeof(YC_DATA_SEND));
	}
}

/*******************************************************************************
* 函数名称: yx_report_soe_clear
* 函数功能: 遥信变位数据初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void yx_report_soe_clear()
{
	int i = 0;
	for(i=0; i<g_nConfigYxNum; i++)
	{
		memset((void*)&g_aYxDataSend[i].nNewValue, 0, sizeof(YX_DATA_SEND));
	}
}

/*******************************************************************************
* 函数名称: report_task_create
* 函数功能: 创建上报任务
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 1 成功; -1 失败
*******************************************************************************/
int report_task_create()
{
	//FRAME104_FMT_TRACE("report_task_create() 创建上报任务... \n");
	
	if(g_nNetWorkFlag != PRO_DATA_TRANSMIT)
	{
		//通信控制变量初始化
        g_bDataSendFlag        = 0;		//全局可以发送数据的标志
		g_bNetTestFlag 	       = 0;		//链路测试使能
		g_nIFrameT1Count	   = 0;		/*帧未收到确认回答的最大延时时间*/	
		g_nIFrameT2Count	   = 0;		//链路测试最大间隔时间
		g_nNoneActIFramTimes   = 0;		/*发送I帧未收到确认帧的次数*/
		g_nIFramRecvTimes	   = 0;		/*收到I帧的帧数量*/ 	
		g_nIFramSendTimes	   = 0;		/*I帧发送帧数量*/
		g_bAllDataCallFlag	   = 0;		/*清总召唤使能标志*/
		g_nElecDataCallFlag	   = 0;		/*电能召唤标志位*/
		g_bYCYXChangeSendFlag  = 0;	    /*遥测数据变化上报使能标志,在连接成功且上报过总召唤后打开*/
		g_bTimerSynFlag		   = 0;		/*时钟同步标志，在主站发生时钟同步后置位*/
		g_nRtuResetFlag		   = 0;		/*进程初始化标志*/
		g_sTaskRunStep.nTaskStep			  = STEP_CONT_DEFAULT;	/*任务进程控制标志*/

		return -1;
	}

	//过秒检测
	if(g_unPastTimeFlag.BitSect.Second == TRUE)
	{
		//遥控预置标志置位，如果超过60秒未执行遥控执行，取消预置
		yk_state_cyc_check();

		//三个超时参数t1_t2_t3循环检测
		t1_t2_t3_cyc_check();
	}

	//接收到主站允许通讯发送应用层数据后开始检验以下流程
	if(g_bDataSendFlag == 2)
    {
		if(g_sTaskRunStep.nTaskStep == STEP_CONT_DEFAULT)//首先判断当前任务是否空闲
        {
            if(g_sTaskRunStep.bFirstRun == 1 )
            {
                g_sTaskRunStep.bFirstRun = 0;
				g_nRtuResetFlag = 8;
				if(g_nRtuResetFlag&0x03)//进程复位 1全复位，清soe队列及遥测数据;2只清soe事件队列;4终端停电复位;8终端本地复位
                {
                    if(g_nRtuResetFlag == 1)//清遥测数据及soe上报队列
                    {
                        yc_report_data_clear();
                        yx_report_soe_clear();
                    }
                    else if(g_nRtuResetFlag == 2)//只清soe上报队列
                    {
                        yx_report_soe_clear();
                    }
                }
                g_sTaskRunStep.nTaskStep = STEP_PRESS_RESET;
            }
            else if(g_nRtuResetFlag)
            {
            	//system("reboot");
            	g_nRtuResetFlag = 0;
                FRAME104_FMT_TRACE("report_task_create() 终端复位 reboot... \n");
            }
            else if (g_aCtrlReSendBuff[0] == 0x55)//遥控命令应答报文
            {
                g_sTaskRunStep.nTaskStep = STEP_REMOTE_CTRL_ACK;
                g_aCtrlReSendBuff[0] = 0xaa;
            }
            else if (g_bYxChanged && g_bYCYXChangeSendFlag)//是否有soe事件上报
            {
                g_sTaskRunStep.nTaskStep = STEP_SOE_CHANGE_SEND;
            }
			else if (g_bFaultChanged && g_bYCYXChangeSendFlag)//是否有故障信息事件上报
			{
				g_sTaskRunStep.nTaskStep = STEP_FAULT_CHANGE_SEND;
				g_bFaultChanged = 0;
			}
            else if(g_bTimerSynFlag == 1)//时钟同步
            {
                g_bTimerSynFlag = 2;
                g_sTaskRunStep.nTaskStep = STEP_TIMER_SYN;
            }
            else if(g_nDataReadFlagOrAddr)//读数据
            {
                g_sTaskRunStep.nTaskStep = STEP_DATA_READ;
            }
            else if(g_bAllDataCallFlag)//是否有总召唤
            {
            	g_sTaskRunStep.nTaskStep = STEP_ALL_DATA_CALL_ALL;
                g_sTaskRunStep.nDataCallStep = ALLDATA_STEP_SEND_YX;
            }
            else if(g_bYcChanged && g_bYCYXChangeSendFlag)//是否有变化遥测数据上报
            {
                g_sTaskRunStep.nTaskStep = STEP_YC_CHANGE_SEND;
				g_sTaskRunStep.nDataCallStep = YC_CHANGE_STEP_SEND_NVA;
            }
            else if(g_nElecDataCallFlag)//电能召唤标志位
            {
                g_sTaskRunStep.nTaskStep = STEP_ENN_DATA_CALL_ALL;
                g_sTaskRunStep.nDataCallStep = ENDATA_STEP_SEND_AC;
            }
			else if(g_nParaDataStep)//参数上送
			{
				g_sTaskRunStep.nTaskStep = STEP_PARA_RES_ACK0;
				g_nParaDataStep = 0;
			}
			else if (g_nParaGetFlag)//参数读取后续处理
            {
                g_sTaskRunStep.nTaskStep =  STEP_NEW_PARA_CALL_ACK;
            }
			else if(g_bAckRecvFlag == 1)//链路接收应答s帧发送标志
			{
				g_sTaskRunStep.nTaskStep = STEP_IDEL_ACK_SEND;
				g_bAckRecvFlag = 0;
			}	
            else
            {
            	g_bNetTestFlag = 1;
            }
        }
		else
		{
			g_bNetTestFlag = 0;
		}
    }
	else if (g_bDataSendFlag == 1)
	{
		g_bDataSendFlag        = 2 ;	//全局可以发送数据的标志
		g_bNetTestFlag 	       = 0;		//链路测试使能
		g_nIFrameT1Count	   = 0;		/*帧未收到确认回答的最大延时时间*/	
		g_nIFrameT2Count	   = 0;		//链路测试最大间隔时间
		g_nNoneActIFramTimes   = 0;		/*发送I帧未收到确认帧的次数*/
		g_nIFramRecvTimes	   = 0;		/*收到I帧的帧数量*/ 	
		g_nIFramSendTimes	   = 0;		/*I帧发送帧数量*/
		g_bAllDataCallFlag	   = 0;		/*清总召唤使能标志*/
		g_nElecDataCallFlag	   = 0;		/*电能召唤标志位*/
		g_bYCYXChangeSendFlag  = 0;	    /*遥测数据变化上报使能标志,在连接成功且上报过总召唤后打开*/
		g_bTimerSynFlag		   = 0;		/*时钟同步标志，在主站发生时钟同步后置位*/
		g_nRtuResetFlag		   = 0;		/*进程初始化标志*/
		g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;	/*任务进程控制标志*/
	}
    else
    {
        g_sTaskRunStep.nTaskStep   = STEP_CONT_DEFAULT;
    }

	return 1;
}

/*********************************************************************
所属功能单元：yx_total_get
功能描述	：获取总招测时的遥信数据
输入参数	：bStartNum-数据队列的位置，*pGetNum，想要获取的数据个数	
输出参数	：*pdata-返回数据缓冲区，返回数据个数*pGetNum 		  
函数返回值	：返回数据的长度
*******************************************************************/
uint8 yx_total_get(uint8 bAdulen, uint8 *pData ,uint16 bStartNum,uint8 *pGetNum)
{
	int i = 0;
	int j = 0;
	uint16 startnum = 0;
	uint8 bgetnum = 0;
	uint8 len = 0;
	uint8 index = 0;
	
	startnum = bStartNum;
	bgetnum = *pGetNum;
	*pGetNum = 0; 
	if(startnum>= g_nConfigYxNum)
	{
		return 0;
	}

	//将数据填写返回区
	for(i=startnum; i<g_nConfigYxNum; i++)
	{
		//信息体地址
		pData[index++] = g_aYxDataConfig[i].nObjAddr&0xff;
		pData[index++] = (g_aYxDataConfig[i].nObjAddr>>8)&0xff;
		if(bAdulen == 3)
		{
			pData[index++] = 0;
		}
		//单点遥信数值
		pData[index++] = g_aYxDataSend[i].nNewValue;

		//修改发送状态
		g_aYxDataSend[i].nHisValue = g_aYxDataSend[i].nNewValue;
		g_aYxDataSend[i].bChangeFlag = FALSE;
		g_aYxDataSend[i].bSendFlag = TRUE;

		DB104_FMT_TRACE("yx_total_get() g_aYxDataSend[%d]: nNewValue=%d, nHisValue=%d, bChangeFlag=%d, bSendFlag=%d, 全局g_bYxChanged=%d \n", 
			index, g_aYxDataSend[index].nNewValue, g_aYxDataSend[index].nHisValue, g_aYxDataSend[index].bChangeFlag, 
			g_aYxDataSend[index].bSendFlag, g_bYxChanged);
		
        j++;
		*pGetNum = j;
		if(j >= bgetnum)
			break;
	}
	
	len = index;
	return len;
}

/*******************************************************************************
* 函数名称: ratio_convert
* 函数功能: 真实值-OAD值，OAD值-真实值，变换系数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
float32 ratio_convert(int8 label, CONVERT_TYPE_E type)
{
	float32 fRatio = 1;

	if((type == E_OOP_TO_REAL) || (type == E_DISPATCH_TO_REAL))
	{
		switch(label)
		{
			case (3):
				fRatio = 1000;
				break;
			case (2):
				fRatio = 100;
				break;
			case (1):
				fRatio = 10;
				break;
			case (0):
				fRatio = 1;
				break;
			case (-1):
				fRatio = 0.1;
				break;
			case (-2):
				fRatio = 0.01;
				break;
			case (-3):
				fRatio = 0.001;
				break;
            case (-4):
                fRatio = 0.0001;
                break;
			default:
				break;
			
		}
	}
	else if((type == E_REAL_TO_OOP) || (type == E_REAL_TO_DISPATCH))
	{
		switch(label)
		{
			case (3):
				fRatio = 0.001;
				break;
			case (2):
				fRatio = 0.01;
				break;
			case (1):
				fRatio = 0.1;
				break;
			case (0):
				fRatio = 1;
				break;
			case (-1):
				fRatio = 10;
				break;
			case (-2):
				fRatio = 100;
				break;
			case (-3):
				fRatio = 1000;
				break;
            case (-4):
                fRatio = 10000;
			default:
				break;
			
		}
	}

	return fRatio;
}


/*********************************************************************
所属功能单元：yc_total_nva_get
功能描述	：总召获取遥测量数据（归一化值）
输入参数	：bStartNum-数据队列的位置，*pGetNum，想要获取的数据个数	
输出参数	：*pdata-返回数据缓冲区，返回数据个数*pGetNum 		  
函数返回值	：返回数据的长度
*******************************************************************/
uint8 yc_total_nva_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum)
{
	//FRAME104_FMT_TRACE("yc_total_nva_get() 总召获取遥测量数据（归一化值） \n");
	
	int i = 0;
	int j = 0;
	uint16 startnum_bk = 0;//备份起始序号
	uint8 getnum_bk = 0;//备份最大允许上报个数
	uint8 len = 0;
	uint8 index = 0;
	float32 fRatio = 1;  //上报调度倍率
	float32 fConvertValue = 0;  //上报调度的值需要乘以系数
	float32 fNvaValue = 0;  //浮点型归一化值
	int16 nValueNVA = 0;  //转换之后的归一化值
	
	startnum_bk = *pStartNum;
	getnum_bk = *pGetNum;
	*pGetNum = 0; 
	if(startnum_bk >= g_nConfigYcNum)
	{
		return 0;
	}

	//将数据填写返回区
	for(i=startnum_bk; i<g_nConfigYcNum; i++)
	{
		if(g_aYcDataConfig[i].nSignalType == M_ME_NA_1)
		{
			//信息体地址
			pData[index++] = g_aYcDataConfig[i].nObjAddr&0xff;
			pData[index++] = (g_aYcDataConfig[i].nObjAddr>>8)&0xff;
			if(bAdulen == 3)
			{
				pData[index++] = 0;
			}
			//归一化数值
			fRatio = ratio_convert(g_aYcDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
			fConvertValue = g_aYcDataSend[i].fNewValue*fRatio;
			fNvaValue = fConvertValue/g_aYcDataConfig[i].fFullValue;  //实际值除以满码值
			//溢出检查
			if(fNvaValue > (1-pow(2, -15)))
			{
				fNvaValue = 1-pow(2, -15);
			}
			else if(fNvaValue < (-1+pow(2, -15)))
			{
				fNvaValue = -1+pow(2, -15);
			}

			if(fNvaValue >= 0)
			{
				nValueNVA = (int16)(fNvaValue*pow(2, 15));
			}
			else
			{
				nValueNVA = (int16)(fNvaValue*pow(2, 15)*(-1));
				nValueNVA = (int16)(nValueNVA*(-1));
			}

			memcpy((void *)&pData[index], (void *)&nValueNVA, sizeof(int16));
			index += sizeof(int16);
			//品质
			pData[index++] = 0;

			//修改变化状态和发送状态
			g_aYcDataSend[i].fHisValue = g_aYcDataSend[i].fNewValue;
			g_aYcDataSend[i].bChangeFlag = FALSE;
			g_aYcDataSend[i].bSendFlag = TRUE;

			FRAME104_FMT_TRACE("总召唤 遥测(归一化值)yc_total_nva_get() i=%d, bChangeFlag:%d, bSendFlag:%d, fNewValue:%f, fHisValue:%f \n", 
				i, g_aYcDataSend[i].bChangeFlag, g_aYcDataSend[i].bSendFlag, g_aYcDataSend[i].fNewValue, g_aYcDataSend[i].fHisValue);

			//判断已经获取的个数是否达到允许最大个数
			j++;
			*pGetNum = j;
			if(j >= getnum_bk)
				break;
		}
	}

	//计算下一次的起始位置
	*pStartNum = i+1;
	
	len = index;
	return len;
}

/*********************************************************************
所属功能单元：yc_total_sva_get
功能描述	：总召获取遥测量数据（标度化值）
输入参数	：bStartNum-数据队列的位置，*pGetNum，想要获取的数据个数	
输出参数	：*pdata-返回数据缓冲区，返回数据个数*pGetNum 		  
函数返回值	：返回数据的长度
*******************************************************************/
uint8 yc_total_sva_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum)
{
	//FRAME104_FMT_TRACE("yc_total_sva_get() 总召获取遥测量数据（标度化值） \n");

	int i = 0;
	int j = 0;
	uint16 startnum_bk = 0;//备份起始序号
	uint8 getnum_bk = 0;//备份最大允许上报个数
	uint8 len = 0;
	uint8 index = 0;
	float32 fRatio = 1;  //上报调度倍率
	float32 fConvertValue = 0;  //上报调度的值需要乘以系数
	int16 nValueSVA;  //转换之后的标度化值
	
	startnum_bk = *pStartNum;
	getnum_bk = *pGetNum;
	*pGetNum = 0; 
	if(startnum_bk >= g_nConfigYcNum)
	{
		return 0;
	}

	//将数据填写返回区
	for(i=startnum_bk; i<g_nConfigYcNum; i++)
	{
		if(g_aYcDataConfig[i].nSignalType == M_ME_NB_1)
		{
			//信息体地址
			pData[index++] = g_aYcDataConfig[i].nObjAddr&0xff;
			pData[index++] = (g_aYcDataConfig[i].nObjAddr>>8)&0xff;
			if(bAdulen == 3)
			{
				pData[index++] = 0;
			}
			//标度化数值
			fRatio = ratio_convert(g_aYcDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
			fConvertValue = g_aYcDataSend[i].fNewValue*fRatio;
			//溢出检查
			if(fConvertValue > 32767)
			{
				nValueSVA = 32767;
			}
			else if(fConvertValue < (-32768))
			{
				nValueSVA = (-32768);
			}
			else
			{
				nValueSVA = (int16)fConvertValue;
			}
			memcpy((void *)&pData[index], (void *)&nValueSVA, sizeof(int16));
			index += sizeof(int16);
			//品质
			pData[index++] = 0;

			//修改变化状态和发送状态
			g_aYcDataSend[i].fHisValue = g_aYcDataSend[i].fNewValue;
			g_aYcDataSend[i].bChangeFlag = FALSE;
			g_aYcDataSend[i].bSendFlag = TRUE;

			FRAME104_FMT_TRACE("总召唤 遥测(标度化值)yc_total_sva_get() i=%d, bChangeFlag:%d, bSendFlag:%d, fNewValue:%f, fHisValue:%f \n", 
				i, g_aYcDataSend[i].bChangeFlag, g_aYcDataSend[i].bSendFlag, g_aYcDataSend[i].fNewValue, g_aYcDataSend[i].fHisValue);

			//判断已经获取的个数是否达到允许最大个数
			j++;
			*pGetNum = j;
			if(j >= getnum_bk)
				break;
		}
	}

	//计算下一次的起始位置
	*pStartNum = i+1;
	
	len = index;
	return len;
}

/*********************************************************************
所属功能单元：yc_total_r_get
功能描述	：总召获取遥测量数据（短浮点数）
输入参数	：bStartNum-数据队列的位置，*pGetNum，想要获取的数据个数	
输出参数	：*pdata-返回数据缓冲区，返回数据个数*pGetNum 		  
函数返回值	：返回数据的长度
*******************************************************************/
uint8 yc_total_r_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum)
{
	//FRAME104_FMT_TRACE("yc_total_r_get() 总召获取遥测量数据（短浮点数） \n");

	int i = 0;
	int j = 0;
	uint16 startnum_bk = 0;//备份起始序号
	uint8 getnum_bk = 0;//备份最大允许上报个数
	uint8 len = 0;
	uint8 index = 0;
	float32 fRatio = 1;  //上报调度倍率
	float32 fConvertValue = 0;  //上报调度的值需要乘以系数
	
	startnum_bk = *pStartNum;
	getnum_bk = *pGetNum;
	*pGetNum = 0; 
	if(startnum_bk >= g_nConfigYcNum)
	{
		return 0;
	}

	//将数据填写返回区
	for(i=startnum_bk; i<g_nConfigYcNum; i++)
	{
		if(g_aYcDataConfig[i].nSignalType == M_ME_NC_1)
		{
			//信息体地址
			pData[index++] = g_aYcDataConfig[i].nObjAddr&0xff;
			pData[index++] = (g_aYcDataConfig[i].nObjAddr>>8)&0xff;
			if(bAdulen == 3)
			{
				pData[index++] = 0;
			}
			//浮点型数值
			fRatio = ratio_convert(g_aYcDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
			fConvertValue = g_aYcDataSend[i].fNewValue*fRatio;
			//FRAME104_FMT_TRACE("fConvertValue=%f, g_aYcDataSend[%d].fNewValue=%f, fRatio=%f \n", fConvertValue, i, g_aYcDataSend[i].fNewValue, fRatio);
			memcpy((void *)&pData[index], (void *)&fConvertValue, sizeof(float32));
			index += sizeof(float32);
			//品质
			pData[index++] = 0;

			//修改变化状态和发送状态
			g_aYcDataSend[i].fHisValue = g_aYcDataSend[i].fNewValue;
			g_aYcDataSend[i].bChangeFlag = FALSE;
			g_aYcDataSend[i].bSendFlag = TRUE;

			FRAME104_FMT_TRACE("总召唤 遥测(短浮点数)yc_total_r_get() i=%d, bChangeFlag:%d, bSendFlag:%d, fNewValue:%f, fHisValue:%f \n", 
				i, g_aYcDataSend[i].bChangeFlag, g_aYcDataSend[i].bSendFlag, g_aYcDataSend[i].fNewValue, g_aYcDataSend[i].fHisValue);

			//判断已经获取的个数是否达到允许最大个数
			j++;
			*pGetNum = j;
			if(j >= getnum_bk)
				break;
		}
	}

	//计算下一次的起始位置
	*pStartNum = i+1;
	
	len = index;
	return len;
}

/*********************************************************************
所属功能单元：yc_total_get
功能描述	：获取总招测时的遥测数据
输入参数	：
输出参数	：
函数返回值	：
*******************************************************************/
int yc_total_get(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType)
{
	int nOutbufLen = 0;//返回的发送数据长度
	uint8 pOutBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //返回的完整发送数据长度
	//int    result = 0;
	uint8 nPipeIndex = g_nGlobalPipe;  //收发通道号
	
	uint8 nSendTI = 0;
	uint8 nSendVSQ = 0;
	uint16 nSendCOT = 0;
	uint8 nSendInObjLen = 0;
	//uint8 pBackInObjBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //用于备份
	uint8 pSendInObjBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //用于组发送报文中的消息对象集内容

	uint8  bgetnum = 0;
	*pAppType = APP_TYPE_NOMMAL;

	switch (g_sTaskRunStep.nSubDataCallSetp)
	{
		case ALLDATA_STEP_SEND_NVA://上报所有遥测信息（归一化值）
        {
            nSendTI = M_ME_NA_1;
            nSendCOT = CAUSE_INTROGEN;
            uint8 nSendNumMax = 30;  //单次上送信号的最大个数(255-12)/(3+2+1)
            uint16 nStartIndex = g_sTaskRunStep.nStartNum;  //起始序号
            bgetnum = nSendNumMax;  //单次上送信号的最大个数
            nSendInObjLen  = yc_total_nva_get(OBJ_ADDR_LEN, pSendInObjBuff, &nStartIndex, &bgetnum);
            FRAME104_FMT_TRACE("上报所有遥测信息（归一化值），返回数据长度 nSendInObjLen =%d, bgetnum = %d \n",nSendInObjLen,bgetnum);
            if(bgetnum == 0)
            {
            	//-没有更多数据需要上报-
                g_sTaskRunStep.nStartNum = 0;
                g_sTaskRunStep.nSubDataCallSetp = ALLDATA_STEP_SEND_SVA;
                break;
            }
			else if(bgetnum < nSendNumMax)
			{
				//-读取的数量少于要读的数据量，表示本次读取结束-
				g_sTaskRunStep.nSubDataCallSetp = ALLDATA_STEP_SEND_SVA;
				g_sTaskRunStep.nStartNum = 0;
			}
			else
			{
				//-数据未读取完，还要继续读取-
				g_sTaskRunStep.nStartNum = nStartIndex;
			}
            nSendVSQ =  bgetnum;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
			break;
        }
		case ALLDATA_STEP_SEND_SVA://上报所有遥测信息（标度化值）
        {
            nSendTI = M_ME_NB_1;
            nSendCOT = CAUSE_INTROGEN;
            uint8 nSendNumMax = 30;  //单次上送信号的最大个数(255-12)/(3+2+1)
            uint16 nStartIndex = g_sTaskRunStep.nStartNum;  //起始序号
            bgetnum = nSendNumMax;  //单次上送信号的最大个数
            nSendInObjLen  = yc_total_sva_get(OBJ_ADDR_LEN, pSendInObjBuff, &nStartIndex, &bgetnum);
            FRAME104_FMT_TRACE("上报所有遥测信息（标度化值），返回数据长度 nSendInObjLen =%d, bgetnum = %d \n",nSendInObjLen,bgetnum);
            if(bgetnum == 0)
            {
            	//-没有更多数据需要上报-
                g_sTaskRunStep.nStartNum = 0;
                g_sTaskRunStep.nSubDataCallSetp = ALLDATA_STEP_SEND_R;
                break;
            }
			else if(bgetnum < nSendNumMax)
			{
				//-读取的数量少于要读的数据量，表示本次读取结束-
				g_sTaskRunStep.nSubDataCallSetp = ALLDATA_STEP_SEND_R;
				g_sTaskRunStep.nStartNum = 0;
			}
			else
			{
				//-数据未读取完，还要继续读取-
				g_sTaskRunStep.nStartNum = nStartIndex;
			}
            nSendVSQ =  bgetnum;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
			break;
        }
		case ALLDATA_STEP_SEND_R://上报所有遥测信息（短浮点数）
        {
            nSendTI = M_ME_NC_1;
            nSendCOT = CAUSE_INTROGEN;
            uint8 nSendNumMax = 25;  //单次上送信号的最大个数(255-12)/(3+4+1)
            uint16 nStartIndex = g_sTaskRunStep.nStartNum;  //起始序号
            bgetnum = nSendNumMax;  //单次上送信号的最大个数
            nSendInObjLen  = yc_total_r_get(OBJ_ADDR_LEN, pSendInObjBuff, &nStartIndex, &bgetnum);
            FRAME104_FMT_TRACE("上报所有遥测信息（短浮点数），返回数据长度 nSendInObjLen =%d, bgetnum = %d \n",nSendInObjLen,bgetnum);
            if(bgetnum == 0)
            {
            	//-没有更多数据需要上报-
                g_sTaskRunStep.nStartNum = 0;
                g_sTaskRunStep.nDataCallStep = ALLDATA_STEP_SEND_STOP;
                break;
            }
			else if(bgetnum < nSendNumMax)
			{
				//-读取的数量少于要读的数据量，表示本次读取结束-
				g_sTaskRunStep.nDataCallStep = ALLDATA_STEP_SEND_STOP;
				g_sTaskRunStep.nStartNum = 0;
			}
			else
			{
				//-数据未读取完，还要继续读取-
				g_sTaskRunStep.nStartNum = nStartIndex;
			}
            nSendVSQ =  bgetnum;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
			break;
        }
	}

	*pPipeIndex = nPipeIndex;
	memcpy(pReportBuf, pOutBuff, nOutbufLen);
	return nOutbufLen;
}


/*********************************************************************
所属功能单元：yx_change_get
功能描述	：获取遥信SOE数据
输入参数	：bStartNum-数据队列的位置，*pGetNum，想要获取的数据个数	
输出参数	：*pdata-返回数据缓冲区，返回数据个数*pGetNum 		  
函数返回值	：返回数据的长度
*******************************************************************/
uint8 yx_change_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum)
{
	int i = 0;
	int j = 0;
	uint16 startnum_bk = 0;//备份起始序号
	uint8 getnum_bk = 0;//备份最大允许上报个数
	uint8 len = 0;
	uint8 index = 0;
	
	startnum_bk = *pStartNum;
	getnum_bk = *pGetNum;
	*pGetNum = 0; 
	if(startnum_bk >= g_nConfigYxNum)
	{
		return 0;
	}

	//将数据填写返回区
	for(i=startnum_bk; i<g_nConfigYxNum; i++)
	{
		if((g_aYxDataSend[i].bChangeFlag == TRUE)
			&& (g_aYxDataSend[i].bSendFlag == FALSE))
		{
			FRAME104_FMT_TRACE("变化遥信yx_change_get()前 i=%d, bChangeFlag:%d, bSendFlag:%d, nNewValue:%d, nHisValue:%d \n", 
				i, g_aYxDataSend[i].bChangeFlag, g_aYxDataSend[i].bSendFlag, g_aYxDataSend[i].nNewValue, g_aYxDataSend[i].nHisValue);
			
			//信息体地址
			pData[index++] = g_aYxDataConfig[i].nObjAddr&0xff;
			pData[index++] = (g_aYxDataConfig[i].nObjAddr>>8)&0xff;
			if(bAdulen == 3)
			{
				pData[index++] = 0;
			}
			//单点遥信数值
			pData[index++] = g_aYxDataSend[i].nNewValue;
			//带时标
			memcpy(&pData[index], &g_aYxDataSend[i].btime[0], 7);
			index += 7;

			//修改变化状态和发送状态
			g_aYxDataSend[i].nHisValue = g_aYxDataSend[i].nNewValue;
			g_aYxDataSend[i].bChangeFlag = FALSE;
			g_aYxDataSend[i].bSendFlag = TRUE;

			FRAME104_FMT_TRACE("变化遥信yx_change_get()后 i=%d, bChangeFlag:%d, bSendFlag:%d, nNewValue:%d, nHisValue:%d \n", 
				i, g_aYxDataSend[i].bChangeFlag, g_aYxDataSend[i].bSendFlag, g_aYxDataSend[i].nNewValue, g_aYxDataSend[i].nHisValue);

			//判断已经获取的个数是否达到允许最大个数
			j++;
			*pGetNum = j;
			if(j >= getnum_bk)
				break;
		}
	}

	//计算下一次的起始位置
	*pStartNum = i+1;
	
	len = index;
	return len;
}

/*********************************************************************
所属功能单元：yc_change_nva_get
功能描述	：变化遥测上报 遥测量数据（归一化值）
输入参数	：bStartNum-数据队列的位置，*pGetNum，想要获取的数据个数	
输出参数	：*pdata-返回数据缓冲区，返回数据个数*pGetNum 		  
函数返回值	：返回数据的长度
*******************************************************************/
uint8 yc_change_nva_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum)
{
	int i = 0;
	int j = 0;
	uint16 startnum_bk = 0;//备份起始序号
	uint8 getnum_bk = 0;//备份最大允许上报个数
	uint8 len = 0;
	uint8 index = 0;
	float32 fRatio = 1;  //上报调度倍率
	float32 fConvertValue = 0;  //上报调度的值需要乘以系数
	float32 fNvaValue = 0;  //浮点型归一化值
	int16 nValueNVA = 0;  //转换之后的归一化值
	
	startnum_bk = *pStartNum;
	getnum_bk = *pGetNum;
	*pGetNum = 0; 
	if(startnum_bk >= g_nConfigYcNum)
	{
		return 0;
	}

	//将数据填写返回区
	for(i=startnum_bk; i<g_nConfigYcNum; i++)
	{
		if(g_aYcDataConfig[i].nSignalType != M_ME_NA_1)
		{
			continue;
		}
		if((g_aYcDataSend[i].bChangeFlag == TRUE)
			&& (g_aYcDataSend[i].bSendFlag == FALSE))
		{
			FRAME104_FMT_TRACE("变化遥测(归一化值)yc_change_nva_get()前 i=%d, bChangeFlag:%d, bSendFlag:%d, fNewValue:%f, fHisValue:%f \n", 
				i, g_aYcDataSend[i].bChangeFlag, g_aYcDataSend[i].bSendFlag, g_aYcDataSend[i].fNewValue, g_aYcDataSend[i].fHisValue);
			
			//信息体地址
			pData[index++] = g_aYcDataConfig[i].nObjAddr&0xff;
			pData[index++] = (g_aYcDataConfig[i].nObjAddr>>8)&0xff;
			if(bAdulen == 3)
			{
				pData[index++] = 0;
			}
			//归一化数值
			fRatio = ratio_convert(g_aYcDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
			fConvertValue = g_aYcDataSend[i].fNewValue*fRatio;
			fNvaValue = fConvertValue/g_aYcDataConfig[i].fFullValue;  //实际值除以满码值
			//溢出检查
			if(fNvaValue > (1-pow(2, -15)))
			{
				fNvaValue = 1-pow(2, -15);
			}
			else if(fNvaValue < (-1+pow(2, -15)))
			{
				fNvaValue = -1+pow(2, -15);
			}

			if(fNvaValue >= 0)
			{
				nValueNVA = (int16)(fNvaValue*pow(2, 15));
			}
			else
			{
				nValueNVA = (int16)(fNvaValue*pow(2, 15)*(-1));
				nValueNVA = (int16)(nValueNVA*(-1));
			}

			memcpy((void *)&pData[index], (void *)&nValueNVA, sizeof(int16));
			index += sizeof(int16);
			//品质
			pData[index++] = 0;
			//带时标
			memcpy(&pData[index], &g_aYcDataSend[i].btime[0], 7);
			index += 7;

			//修改变化状态和发送状态
			g_aYcDataSend[i].fHisValue = g_aYcDataSend[i].fNewValue;
			g_aYcDataSend[i].bChangeFlag = FALSE;
			g_aYcDataSend[i].bSendFlag = TRUE;

			FRAME104_FMT_TRACE("变化遥测(归一化值)yc_change_nva_get()后 i=%d, bChangeFlag:%d, bSendFlag:%d, fNewValue:%f, fHisValue:%f \n", 
				i, g_aYcDataSend[i].bChangeFlag, g_aYcDataSend[i].bSendFlag, g_aYcDataSend[i].fNewValue, g_aYcDataSend[i].fHisValue);

			//判断已经获取的个数是否达到允许最大个数
			j++;
			*pGetNum = j;
			if(j >= getnum_bk)
				break;
		}
	}

	//计算下一次的起始位置
	*pStartNum = i+1;
	
	len = index;
	return len;
}

/*********************************************************************
所属功能单元：yc_change_sva_get
功能描述	：变化遥测上报 遥测量数据（标度化值）
输入参数	：bStartNum-数据队列的位置，*pGetNum，想要获取的数据个数	
输出参数	：*pdata-返回数据缓冲区，返回数据个数*pGetNum 		  
函数返回值	：返回数据的长度
*******************************************************************/
uint8 yc_change_sva_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum)
{
	int i = 0;
	int j = 0;
	uint16 startnum_bk = 0;//备份起始序号
	uint8 getnum_bk = 0;//备份最大允许上报个数
	uint8 len = 0;
	uint8 index = 0;
	float32 fRatio = 1;  //上报调度倍率
	float32 fConvertValue = 0;  //上报调度的值需要乘以系数
	int16 nValueSVA;  //转换之后的标度化值
	
	startnum_bk = *pStartNum;
	getnum_bk = *pGetNum;
	*pGetNum = 0; 
	if(startnum_bk >= g_nConfigYcNum)
	{
		return 0;
	}

	//将数据填写返回区
	for(i=startnum_bk; i<g_nConfigYcNum; i++)
	{
		if(g_aYcDataConfig[i].nSignalType != M_ME_NB_1)
		{
			continue;
		}
		if((g_aYcDataSend[i].bChangeFlag == TRUE)
			&& (g_aYcDataSend[i].bSendFlag == FALSE))
		{
			FRAME104_FMT_TRACE("变化遥测(标度化值)yc_change_sva_get()前 i=%d, bChangeFlag:%d, bSendFlag:%d, fNewValue:%f, fHisValue:%f \n", 
				i, g_aYcDataSend[i].bChangeFlag, g_aYcDataSend[i].bSendFlag, g_aYcDataSend[i].fNewValue, g_aYcDataSend[i].fHisValue);
			
			//信息体地址
			pData[index++] = g_aYcDataConfig[i].nObjAddr&0xff;
			pData[index++] = (g_aYcDataConfig[i].nObjAddr>>8)&0xff;
			if(bAdulen == 3)
			{
				pData[index++] = 0;
			}
			//标度化数值
			fRatio = ratio_convert(g_aYcDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
			fConvertValue = g_aYcDataSend[i].fNewValue*fRatio;
			//溢出检查
			if(fConvertValue > 32767)
			{
				nValueSVA = 32767;
			}
			else if(fConvertValue < (-32768))
			{
				nValueSVA = (-32768);
			}
			else
			{
				nValueSVA = (int16)fConvertValue;
			}
			memcpy((void *)&pData[index], (void *)&nValueSVA, sizeof(int16));
			index += sizeof(int16);
			//品质
			pData[index++] = 0;
			//带时标
			memcpy(&pData[index], &g_aYcDataSend[i].btime[0], 7);
			index += 7;

			//修改变化状态和发送状态
			g_aYcDataSend[i].fHisValue = g_aYcDataSend[i].fNewValue;
			g_aYcDataSend[i].bChangeFlag = FALSE;
			g_aYcDataSend[i].bSendFlag = TRUE;

			FRAME104_FMT_TRACE("变化遥测(标度化值)yc_change_sva_get()后 i=%d, bChangeFlag:%d, bSendFlag:%d, fNewValue:%f, fHisValue:%f \n", 
				i, g_aYcDataSend[i].bChangeFlag, g_aYcDataSend[i].bSendFlag, g_aYcDataSend[i].fNewValue, g_aYcDataSend[i].fHisValue);

			//判断已经获取的个数是否达到允许最大个数
			j++;
			*pGetNum = j;
			if(j >= getnum_bk)
				break;
		}
	}

	//计算下一次的起始位置
	*pStartNum = i+1;
	
	len = index;
	return len;
}

/*********************************************************************
所属功能单元：yc_change_r_get
功能描述	：变化遥测上报 遥测量数据（短浮点数）
输入参数	：bStartNum-数据队列的位置，*pGetNum，想要获取的数据个数	
输出参数	：*pdata-返回数据缓冲区，返回数据个数*pGetNum 		  
函数返回值	：返回数据的长度
*******************************************************************/
uint8 yc_change_r_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum)
{
	int i = 0;
	int j = 0;
	uint16 startnum_bk = 0;//备份起始序号
	uint8 getnum_bk = 0;//备份最大允许上报个数
	uint8 len = 0;
	uint8 index = 0;
	float32 fRatio = 1;  //上报调度倍率
	float32 fConvertValue = 0;  //上报调度的值需要乘以系数
	
	startnum_bk = *pStartNum;
	getnum_bk = *pGetNum;
	*pGetNum = 0; 
	if(startnum_bk >= g_nConfigYcNum)
	{
		return 0;
	}

	//将数据填写返回区
	for(i=startnum_bk; i<g_nConfigYcNum; i++)
	{
		if(g_aYcDataConfig[i].nSignalType != M_ME_NC_1)
		{
			continue;
		}
		if((g_aYcDataSend[i].bChangeFlag == TRUE)
			&& (g_aYcDataSend[i].bSendFlag == FALSE))	
		{
			FRAME104_FMT_TRACE("变化遥测(短浮点数)yc_change_r_get()前 i=%d, bChangeFlag:%d, bSendFlag:%d, fNewValue:%f, fHisValue:%f \n", 
				i, g_aYcDataSend[i].bChangeFlag, g_aYcDataSend[i].bSendFlag, g_aYcDataSend[i].fNewValue, g_aYcDataSend[i].fHisValue);
			
			//信息体地址
			pData[index++] = g_aYcDataConfig[i].nObjAddr&0xff;
			pData[index++] = (g_aYcDataConfig[i].nObjAddr>>8)&0xff;
			if(bAdulen == 3)
			{
				pData[index++] = 0;
			}
			//浮点型数值
			fRatio = ratio_convert(g_aYcDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
			fConvertValue = g_aYcDataSend[i].fNewValue*fRatio;
			memcpy((void *)&pData[index], (void *)&fConvertValue, sizeof(float32));
			index += sizeof(float32);
			//品质
			pData[index++] = 0;
			//带时标
			memcpy(&pData[index], &g_aYcDataSend[i].btime[0], 7);
			index += 7;

			//修改变化状态和发送状态
			g_aYcDataSend[i].fHisValue = g_aYcDataSend[i].fNewValue;
			g_aYcDataSend[i].bChangeFlag = FALSE;
			g_aYcDataSend[i].bSendFlag = TRUE;

			FRAME104_FMT_TRACE("变化遥测(短浮点数)yc_change_r_get()后 i=%d, bChangeFlag:%d, bSendFlag:%d, fNewValue:%f, fHisValue:%f \n", 
				i, g_aYcDataSend[i].bChangeFlag, g_aYcDataSend[i].bSendFlag, g_aYcDataSend[i].fNewValue, g_aYcDataSend[i].fHisValue);

			//判断已经获取的个数是否达到允许最大个数
			j++;
			*pGetNum = j;
			if(j >= getnum_bk)
				break;
		}
	}

	//计算下一次的起始位置
	*pStartNum = i+1;
	
	len = index;
	return len;
}

/*********************************************************************
所属功能单元：yc_change_get
功能描述	：变化遥测上报
输入参数	：
输出参数	：
函数返回值	：
*******************************************************************/
int yc_change_get(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType)
{
	int nOutbufLen = 0;//返回的发送数据长度
	uint8 pOutBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //返回的完整发送数据长度
	//int    result = 0;
	uint8 nPipeIndex = g_nGlobalPipe;  //收发通道号
	
	uint8 nSendTI = 0;
	uint8 nSendVSQ = 0;
	uint16 nSendCOT = 0;
	uint8 nSendInObjLen = 0;
	//uint8 pBackInObjBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //用于备份
	uint8 pSendInObjBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //用于组发送报文中的消息对象集内容

	uint8  bgetnum = 0;
	*pAppType = APP_TYPE_NOMMAL;

	switch (g_sTaskRunStep.nDataCallStep)
	{
		case YC_CHANGE_STEP_SEND_NVA://上报变化遥测信息（归一化值）
        {
            nSendTI = M_ME_TD_1;
            nSendCOT = CAUSE_SPONT;
            uint8 nSendNumMax = 15;  //单次上送信号的最大个数(255-12)/(3+2+1+7)
            uint16 nStartIndex = g_sTaskRunStep.nStartNum;  //起始序号
            bgetnum = nSendNumMax;  //单次上送信号的最大个数
            nSendInObjLen  = yc_change_nva_get(OBJ_ADDR_LEN, pSendInObjBuff, &nStartIndex, &bgetnum);
            FRAME104_FMT_TRACE("上报变化遥测信息（归一化值），返回数据长度 nSendInObjLen =%d, bgetnum = %d \n",nSendInObjLen,bgetnum);
            if(bgetnum == 0)
            {
            	//-没有更多数据需要上报-
                g_sTaskRunStep.nStartNum = 0;
                g_sTaskRunStep.nDataCallStep = YC_CHANGE_STEP_SEND_SVA;
                break;
            }
			else if(bgetnum < nSendNumMax)
			{
				//-读取的数量少于要读的数据量，表示本次读取结束-
				g_sTaskRunStep.nDataCallStep = YC_CHANGE_STEP_SEND_SVA;
				g_sTaskRunStep.nStartNum = 0;
			}
			else
			{
				//-数据未读取完，还要继续读取-
				g_sTaskRunStep.nStartNum = nStartIndex;
			}
            nSendVSQ =  bgetnum;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
			break;
        }
		case YC_CHANGE_STEP_SEND_SVA://上报变化遥测信息（标度化值）
        {
            nSendTI = M_ME_TE_1;
            nSendCOT = CAUSE_SPONT;
            uint8 nSendNumMax = 15;  //单次上送信号的最大个数(255-12)/(3+2+1+7)
            uint16 nStartIndex = g_sTaskRunStep.nStartNum;  //起始序号
            bgetnum = nSendNumMax;  //单次上送信号的最大个数
            nSendInObjLen  = yc_change_sva_get(OBJ_ADDR_LEN, pSendInObjBuff, &nStartIndex, &bgetnum);
            FRAME104_FMT_TRACE("上报变化遥测信息（标度化值），返回数据长度 nSendInObjLen =%d, bgetnum = %d \n",nSendInObjLen,bgetnum);
            if(bgetnum == 0)
            {
            	//-没有更多数据需要上报-
                g_sTaskRunStep.nStartNum = 0;
                g_sTaskRunStep.nDataCallStep = YC_CHANGE_STEP_SEND_R;
                break;
            }
			else if(bgetnum < nSendNumMax)
			{
				//-读取的数量少于要读的数据量，表示本次读取结束-
				g_sTaskRunStep.nDataCallStep = YC_CHANGE_STEP_SEND_R;
				g_sTaskRunStep.nStartNum = 0;
			}
			else
			{
				//-数据未读取完，还要继续读取-
				g_sTaskRunStep.nStartNum = nStartIndex;
			}
            nSendVSQ =  bgetnum;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
			break;
        }
		case YC_CHANGE_STEP_SEND_R://上报变化遥测信息（短浮点数）
        {
            nSendTI = M_ME_TF_1;
            nSendCOT = CAUSE_SPONT;
            uint8 nSendNumMax = 15;  //单次上送信号的最大个数(255-12)/(3+4+1+7)
            uint16 nStartIndex = g_sTaskRunStep.nStartNum;  //起始序号
            bgetnum = nSendNumMax;  //单次上送信号的最大个数
            nSendInObjLen  = yc_change_r_get(OBJ_ADDR_LEN, pSendInObjBuff, &nStartIndex, &bgetnum);
            FRAME104_FMT_TRACE("上报变化遥测信息（短浮点数），返回数据长度 nSendInObjLen =%d, bgetnum = %d \n",nSendInObjLen,bgetnum);
            if(bgetnum == 0)
            {
            	//-没有更多数据需要上报-
                g_sTaskRunStep.nStartNum = 0;
                g_sTaskRunStep.nDataCallStep = YC_CHANGE_STEP_SEND_DEFAULT;
                break;
            }
			else if(bgetnum < nSendNumMax)
			{
				//-读取的数量少于要读的数据量，表示本次读取结束-
				g_sTaskRunStep.nDataCallStep = YC_CHANGE_STEP_SEND_DEFAULT;
				g_sTaskRunStep.nStartNum = 0;
			}
			else
			{
				//-数据未读取完，还要继续读取-
				g_sTaskRunStep.nStartNum = nStartIndex;
			}
            nSendVSQ =  bgetnum;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
			g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
			break;
        }
		case YC_CHANGE_STEP_SEND_DEFAULT://变化遥测上送结束
        {
			g_bYcChanged = FALSE;
			g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
		}
		default:
        {
            g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
            break;
        }
	}

	*pPipeIndex = nPipeIndex;
	memcpy(pReportBuf, pOutBuff, nOutbufLen);
	return nOutbufLen;
}

/*********************************************************************
所属功能单元：en_ac_data_get
功能描述	：获取电能量数据（不带时标 交采数据）
输入参数	：bStartNum-数据队列的位置，*pGetNum，想要获取的数据个数	
输出参数	：*pdata-返回数据缓冲区，返回数据个数*pGetNum 		  
函数返回值	：返回数据的长度
*******************************************************************/
uint8 en_ac_data_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum)
{
	int i = 0;
	int j = 0;
	uint16 startnum_bk = 0;//备份起始序号
	uint8 getnum_bk = 0;//备份最大允许上报个数
	uint8 len = 0;
	uint8 index = 0;
	float32 fRatio = 1;  //上报调度倍率
	float32 fConvertValue = 0;  //上报调度的值需要乘以系数
	
	startnum_bk = *pStartNum;
	getnum_bk = *pGetNum;
	*pGetNum = 0; 
	if(startnum_bk >= g_nConfigEnNum)
	{
		return 0;
	}

	//将数据填写返回区
	FRAME104_FMT_TRACE("en_ac_data_get() startnum_bk=%d, getnum_bk=%d g_nConfigEnNum=%d \n", startnum_bk, getnum_bk, g_nConfigEnNum);
	for(i=startnum_bk; i<g_nConfigEnNum; i++)
	{
		if(g_aEnergyDataConfig[i].nLogicId == 1)
		{
			//信息体地址
			pData[index++] = g_aEnergyDataConfig[i].nObjAddr&0xff;
			pData[index++] = (g_aEnergyDataConfig[i].nObjAddr>>8)&0xff;
			if(bAdulen == 3)
			{
				pData[index++] = 0;
			}
			//浮点型数值
			fRatio = ratio_convert(g_aEnergyDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
			fConvertValue = g_aEnergyDataSend[i].fNewValue*fRatio;
			memcpy((void *)&pData[index], (void *)&fConvertValue, sizeof(float32));
			index += sizeof(float32);
			//品质
			pData[index++] = 0;

			//修改变化状态和发送状态
			g_aEnergyDataSend[i].fHisValue = g_aEnergyDataSend[i].fNewValue;
			g_aEnergyDataSend[i].bChangeFlag = FALSE;
			g_aEnergyDataSend[i].bSendFlag = TRUE;

			//判断已经获取的个数是否达到允许最大个数
			j++;
			*pGetNum = j;
			if(j >= getnum_bk)
				break;
		}
	}

	//计算下一次的起始位置
	*pStartNum = i+1;
	
	len = index;
	return len;
}


/*********************************************************************
所属功能单元：en_without_time_get
功能描述	：获取电能量数据（不带时标）
输入参数	：bStartNum-数据队列的位置，*pGetNum，想要获取的数据个数	
输出参数	：*pdata-返回数据缓冲区，返回数据个数*pGetNum 		  
函数返回值	：返回数据的长度
*******************************************************************/
uint8 en_without_time_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum)
{
	int i = 0;
	int j = 0;
	uint16 startnum_bk = 0;//备份起始序号
	uint8 getnum_bk = 0;//备份最大允许上报个数
	uint8 len = 0;
	uint8 index = 0;
	float32 fRatio = 1;  //上报调度倍率
	float32 fConvertValue = 0;  //上报调度的值需要乘以系数
	
	startnum_bk = *pStartNum;
	getnum_bk = *pGetNum;
	*pGetNum = 0; 
	if(startnum_bk >= g_nConfigEnNum)
	{
		return 0;
	}

	//将数据填写返回区
	FRAME104_FMT_TRACE("en_without_time_get() startnum_bk=%d, getnum_bk=%d g_nConfigEnNum=%d \n", startnum_bk, getnum_bk, g_nConfigEnNum);
	for(i=startnum_bk; i<g_nConfigEnNum; i++)
	{
		if((g_aEnergyDataConfig[i].nLogicId == 0)
			&& (g_aEnergyDataConfig[i].nDbCenterOAD == 0x00000000))
		{
			//信息体地址
			pData[index++] = g_aEnergyDataConfig[i].nObjAddr&0xff;
			pData[index++] = (g_aEnergyDataConfig[i].nObjAddr>>8)&0xff;
			if(bAdulen == 3)
			{
				pData[index++] = 0;
			}
			//浮点型数值
			fRatio = ratio_convert(g_aEnergyDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
			fConvertValue = g_aEnergyDataSend[i].fNewValue*fRatio;
 			//FRAME104_FMT_TRACE(" fConvertValue=%f, g_aEnergyDataSend[%d].fNewValue=%f, fRatio=%f \n", fConvertValue, i, g_aEnergyDataSend[i].fNewValue, fRatio);
			memcpy((void *)&pData[index], (void *)&fConvertValue, sizeof(float32));
			index += sizeof(float32);
			//品质
			pData[index++] = 0;

			//修改变化状态和发送状态
			g_aEnergyDataSend[i].fHisValue = g_aEnergyDataSend[i].fNewValue;
			g_aEnergyDataSend[i].bChangeFlag = FALSE;
			g_aEnergyDataSend[i].bSendFlag = TRUE;

			//判断已经获取的个数是否达到允许最大个数
			j++;
			*pGetNum = j;
			if(j >= getnum_bk)
				break;
		}
	}

	//计算下一次的起始位置
	*pStartNum = i+1;
	
	len = index;
	return len;
}


/*********************************************************************
所属功能单元：en_with_time_get
功能描述	：获取电能量数据（带时标）
输入参数	：bStartNum-数据队列的位置，*pGetNum，想要获取的数据个数	
输出参数	：*pdata-返回数据缓冲区，返回数据个数*pGetNum 		  
函数返回值	：返回数据的长度
*******************************************************************/
uint8 en_with_time_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum)
{
	int i = 0;
	int j = 0;
	uint16 startnum_bk = 0;//备份起始序号
	uint8 getnum_bk = 0;//备份最大允许上报个数
	uint8 len = 0;
	uint8 index = 0;
	float32 fRatio = 1;  //上报调度倍率
	float32 fConvertValue = 0;  //上报调度的值需要乘以系数
	
	startnum_bk = *pStartNum;
	getnum_bk = *pGetNum;
	*pGetNum = 0; 
	if(startnum_bk >= g_nConfigEnNum)
	{
		return 0;
	}

	//将数据填写返回区
	FRAME104_FMT_TRACE("en_with_time_get() startnum_bk=%d, getnum_bk=%d g_nConfigEnNum=%d \n", startnum_bk, getnum_bk, g_nConfigEnNum);
	for(i=startnum_bk; i<g_nConfigEnNum; i++)
	{
		if((g_aEnergyDataConfig[i].nLogicId == 0)
			&& (g_aEnergyDataConfig[i].nDbCenterOAD != 0x00000000))
		{
			//信息体地址
			pData[index++] = g_aEnergyDataConfig[i].nObjAddr&0xff;
			pData[index++] = (g_aEnergyDataConfig[i].nObjAddr>>8)&0xff;
			if(bAdulen == 3)
			{
				pData[index++] = 0;
			}
			//浮点型数值
			fRatio = ratio_convert(g_aEnergyDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
			fConvertValue = g_aEnergyDataSend[i].fNewValue*fRatio;
			memcpy((void *)&pData[index], (void *)&fConvertValue, sizeof(float32));
			index += sizeof(float32);
			//品质
			pData[index++] = 0;
			//带时标
			memcpy(&pData[index], &g_aEnergyDataSend[i].btime[0], 7);
			index += 7;

			//修改变化状态和发送状态
			g_aEnergyDataSend[i].fHisValue = g_aEnergyDataSend[i].fNewValue;
			g_aEnergyDataSend[i].bChangeFlag = FALSE;
			g_aEnergyDataSend[i].bSendFlag = TRUE;

			//判断已经获取的个数是否达到允许最大个数
			j++;
			*pGetNum = j;
			if(j >= getnum_bk)
				break;
		}
	}

	//计算下一次的起始位置
	*pStartNum = i+1;
	
	len = index;
	return len;
}

/*********************************************************************
所属功能单元：en_total_get
功能描述	：电能量总召应答数据
输入参数	：
输出参数	：
函数返回值	：
*******************************************************************/
int en_total_get(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType)
{
	int nOutbufLen = 0;//返回的发送数据长度
	uint8 pOutBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //返回的完整发送数据长度
	//int    result = 0;
	uint8 nPipeIndex = g_nGlobalPipe;  //收发通道号
	
	uint8 nSendTI = 0;
	uint8 nSendVSQ = 0;
	uint16 nSendCOT = 0;
	uint8 nSendInObjLen = 0;
	//uint8 pBackInObjBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //用于备份
	uint8 pSendInObjBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //用于组发送报文中的消息对象集内容

	uint8  bgetnum = 0;
	*pAppType = APP_TYPE_NOMMAL;

	nPipeIndex = g_sTaskReportPipe.nEnDataCallPipe;
    switch(g_sTaskRunStep.nDataCallStep)
    {
        case ENDATA_STEP_SEND_AC:
        {
            nSendTI        =  M_IT_NB_1;
            nSendCOT       =  CAUSE_REQCOGEN;
            uint8 nSendNumMax = 25;  //单次上送信号的最大个数(255-12)/(3+4+1)
            uint16 nStartIndex = g_sTaskRunStep.nStartNum;  //起始序号
            bgetnum = nSendNumMax;  //单次上送信号的最大个数
            nSendInObjLen  = en_ac_data_get(OBJ_ADDR_LEN, pSendInObjBuff, &nStartIndex, &bgetnum);
            FRAME104_FMT_TRACE("交采电能量数据，返回数据长度 nSendInObjLen=%d, nStartIndex=%d, bgetnum=%d \n", nSendInObjLen, nStartIndex, bgetnum);
            if(bgetnum == 0)
            {
            	//-没有更多数据需要上报-
                g_sTaskRunStep.nStartNum = 0;
                g_sTaskRunStep.nDataCallStep = ENDATA_STEP_SEND_NO_TIME;
                break;
            }
			else if(bgetnum < nSendNumMax)
			{
				//-读取的数量少于要读的数据量，表示本次读取结束-
				g_sTaskRunStep.nDataCallStep = ENDATA_STEP_SEND_NO_TIME;
				g_sTaskRunStep.nStartNum = 0;
			}
			else
			{
				//-数据未读取完，还要继续读取-
				g_sTaskRunStep.nStartNum = nStartIndex;
			}
            nSendVSQ =  bgetnum;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
			break;
        }
		case ENDATA_STEP_SEND_NO_TIME:
        {
        	nSendTI        =  M_IT_NB_1;
            nSendCOT       =  CAUSE_REQCOGEN;
            uint8 nSendNumMax = 25;  //单次上送信号的最大个数(255-12)/(3+4+1)
            uint16 nStartIndex = g_sTaskRunStep.nStartNum;  //起始序号
            bgetnum = nSendNumMax;  //单次上送信号的最大个数
            nSendInObjLen  = en_without_time_get(OBJ_ADDR_LEN, pSendInObjBuff, &nStartIndex, &bgetnum);
            FRAME104_FMT_TRACE("电表电能量数据（不带时标），返回数据长度 nSendInObjLen=%d, nStartIndex=%d, bgetnum=%d \n", nSendInObjLen, nStartIndex, bgetnum);
            if(bgetnum == 0)
            {
            	//-没有更多数据需要上报-
                g_sTaskRunStep.nStartNum = 0;
                g_sTaskRunStep.nDataCallStep = ENDATA_STEP_SEND_TIME;
                break;
            }
			else if(bgetnum < nSendNumMax)
			{
				//-读取的数量少于要读的数据量，表示本次读取结束-
				g_sTaskRunStep.nDataCallStep = ENDATA_STEP_SEND_TIME;
				g_sTaskRunStep.nStartNum = 0;
			}
			else
			{
				//-数据未读取完，还要继续读取-
				g_sTaskRunStep.nStartNum = nStartIndex;
			}
            nSendVSQ =  bgetnum;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
			break;
        }
		case ENDATA_STEP_SEND_TIME:
        {
            nSendTI        =  M_IT_TC_1;
            nSendCOT       =  CAUSE_REQCOGEN;
            int8 nSendNumMax = 15;  //单次上送信号的最大个数(255-12)/(3+4+1+7)
            uint16 nStartIndex = g_sTaskRunStep.nStartNum;  //起始序号
            bgetnum = nSendNumMax;  //单次上送信号的最大个数
            nSendInObjLen  = en_with_time_get(OBJ_ADDR_LEN, pSendInObjBuff, &nStartIndex, &bgetnum);
            FRAME104_FMT_TRACE("电表电能量数据（带时标），返回数据长度 nSendInObjLen=%d, nStartIndex=%d, bgetnum=%d \n", nSendInObjLen, nStartIndex, bgetnum);
            if(bgetnum == 0)
            {
            	//-没有更多数据需要上报-
                g_sTaskRunStep.nStartNum = 0;
                g_sTaskRunStep.nDataCallStep = ENDATA_STEP_SEND_STOP;
                break;
            }
			else if(bgetnum < nSendNumMax)
			{
				//-读取的数量少于要读的数据量，表示本次读取结束-
				g_sTaskRunStep.nDataCallStep = ENDATA_STEP_SEND_STOP;
				g_sTaskRunStep.nStartNum = 0;
			}
			else
			{
				//-数据未读取完，还要继续读取-
				g_sTaskRunStep.nStartNum = nStartIndex;
			}
            nSendVSQ =  bgetnum;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
			break;
        }
		case ENDATA_STEP_SEND_STOP://上报激活终止
        {
            pSendInObjBuff[0]  = 0;
            pSendInObjBuff[1]  = 0;
            pSendInObjBuff[2]  = 0;
            pSendInObjBuff[3]  = g_nElecDataCallFlag-1;
            nSendInObjLen = 4;
            nSendTI = C_CI_NA_1;
            nSendCOT = CAUSE_ACTTERM;
            nSendVSQ = 1;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
            g_nElecDataCallFlag = 0;
			g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
			g_sTaskRunStep.nDataCallStep = ENDATA_STEP_SEND_DEFAULT;
            break;
        }
        default:
        {
            g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
            break;
        }
    }

	*pPipeIndex = nPipeIndex;
	memcpy(pReportBuf, pOutBuff, nOutbufLen);
	return nOutbufLen;
}


/*********************************************************************
所属功能单元：fault_change_get
功能描述	：获取故障事件数据
输入参数	：bStartNum-数据队列的位置，*pGetNum，想要获取的数据个数	
输出参数	：*pdata-返回数据缓冲区，返回数据个数*pGetNum 		  
函数返回值	：返回数据的长度
*******************************************************************/
uint8 fault_change_get(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum)
{
	int i = 0;
	int j = 0;
	int k = 0;
	uint8 bstep = 0;
	uint16 startnum_bk = 0;//备份起始序号
	uint8 getnum_bk = 0;//备份最大允许上报个数
	uint8 len = 0;
	uint8 index = 0;
	
	startnum_bk = *pStartNum;
	getnum_bk = *pGetNum;
	*pGetNum = 0; 
	if(startnum_bk >= g_nConfigFaultNum)
	{
		return 0;
	}
	
	//将数据填写返回区
	for(i=startnum_bk; i<g_nConfigFaultNum; i++)
	{
		if((g_aFaultDataSend[i].bChangeFlag == TRUE)
			&& (g_aFaultDataSend[i].bSendFlag == FALSE))
		{
			//带时标遥信个数(是否需要？)
			pData[index++] = 1;

			//带时标遥信类型(是否需要？)
			pData[index++] = g_aFaultDataSend[i].nYxType;
			
			//信息体地址
			pData[index++] = g_aFaultDataConfig[i].nFaultYxAddr&0xff;
			pData[index++] = (g_aFaultDataConfig[i].nFaultYxAddr>>8)&0xff;
			if(bAdulen == 3)
			{
				pData[index++] = 0;
			}
			//单点遥信数值
			pData[index++] = g_aFaultDataSend[i].nNewValue;
			//带时标
			memcpy(&pData[index], &g_aFaultDataSend[i].btime[0], 7);
			index += 7;

			//遥测个数(是否需要？)
			pData[index++] = g_aFaultDataSend[i].nReportNum;

			//遥测类型(是否需要？)
			pData[index++] = g_aFaultDataSend[i].nReportType;
			
			for(k=0; k<g_aFaultDataSend[i].nReportNum; k++)
			{
				if(g_aFaultDataSend[i].nReportType == M_ME_NC_1 )
				{	
					if(bAdulen == 3)
					{	
						memcpy((void *)&pData[index],(void *)&g_aFaultDataSend[i].aReportData[bstep],7);
						index += 7;
						bstep += 7;
					}
					else
					{
						memcpy((void *)&pData[index],(void *)&g_aFaultDataSend[i].aReportData[bstep],6);
						index += 6;
						bstep += 6;
					}
				}
				else
				{
					if(bAdulen == 3)
					{
						memcpy((void *)&pData[index],(void *)&g_aFaultDataSend[i].aReportData[bstep],5);
						index += 5;
						bstep += 5;
					}
					else	
					{
						memcpy((void *)&pData[index],(void *)&g_aFaultDataSend[i].aReportData[bstep],4);
						index += 4;
						bstep += 4;
					}	
				}
			}

			//修改变化状态和发送状态
			g_aFaultDataSend[i].nHisValue = g_aFaultDataSend[i].nNewValue;
			g_aFaultDataSend[i].bChangeFlag = FALSE;
			g_aFaultDataSend[i].bSendFlag = TRUE;

			//判断已经获取的个数是否达到允许最大个数
			j++;
			*pGetNum = j;
			if(j >= getnum_bk)
				break;
		}
	}

	//计算下一次的起始位置
	*pStartNum = i+1;
	
	len = index;
	return len;
}

#ifndef AREA_ZHEJIANG
/*******************************************************************************
* 函数名称: report_task_run
* 函数功能: 执行上报任务
* 输入参数: 无
* 输出参数: uint8 *pPipeIndex  通道号
            uint8 *pReportBuf  上报数据内容
            uint8 *pAppType     处理类型
* 返 回 值: 上报数据长度
*******************************************************************************/
int report_task_run(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType)
{
	/*
	FRAME104_FMT_TRACE("report_task_run() 执行上报任务 g_nNetWorkFlag=%d, nTaskStep=%d, nDataCallStep=%d, nSubDataCallSetp=%d, nStartNum=%d \n", 
		g_nNetWorkFlag, g_sTaskRunStep.nTaskStep, g_sTaskRunStep.nDataCallStep, g_sTaskRunStep.nSubDataCallSetp, g_sTaskRunStep.nStartNum);
	*/
	
	int nOutbufLen = 0;//返回的发送数据长度
	uint8 pOutBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //返回的完整发送数据长度
	uint8 nPipeIndex = g_nGlobalPipe;  //收发通道号
	
	uint8 nSendTI = 0;
	uint8 nSendVSQ = 0;
	uint16 nSendCOT = 0;
	uint8 nSendInObjLen = 0;
	uint8 pSendInObjBuff[MAX_INFO_OBJ_DATA_LEN] = {0};  //用于组发送报文中的消息对象集内容

	uint8  bgetnum = 0;

	*pAppType = APP_TYPE_NOMMAL;
	
	if(g_nNetWorkFlag != PRO_DATA_TRANSMIT)
	{
		return -1;
	}
	
	//任务闪转处理
	switch(g_sTaskRunStep.nTaskStep)
	{
        case STEP_ALL_DATA_CALL_ALL://总召唤处理
        {
        	nPipeIndex = g_sTaskReportPipe.nAllDataCallPipe;
            switch(g_sTaskRunStep.nDataCallStep)
            {
                case ALLDATA_STEP_SEND_YX://上报所有遥信信息
                {
                    nSendTI        =  M_SP_NA_1;
                    nSendCOT       =  CAUSE_INTROGEN;
					uint8 nSendNumMax = 40;  //单次上送信号的最大个数(255-12)/(3+1)
                    bgetnum = nSendNumMax;  //单次上送信号的最大个数
                    nSendInObjLen  = yx_total_get(OBJ_ADDR_LEN, pSendInObjBuff, g_sTaskRunStep.nStartNum, &bgetnum);
                    FRAME104_FMT_TRACE("总召遥信取数据，返回数据长度 nSendInObjLen =%d, bgetnum = %d \n",nSendInObjLen,bgetnum);
                    if(bgetnum == 0)
                    {
                    	//-没有更多数据需要上报-
                        g_sTaskRunStep.nStartNum = 0;
                        g_sTaskRunStep.nDataCallStep = ALLDATA_STEP_SEND_YC;
						g_sTaskRunStep.nSubDataCallSetp = ALLDATA_STEP_SEND_NVA;
                        break;
                    }
        			else if(bgetnum < nSendNumMax)
        			{
        				//-读取的数量少于要读的数据量，表示本次读取结束-
        				g_sTaskRunStep.nDataCallStep = ALLDATA_STEP_SEND_YC;
						g_sTaskRunStep.nSubDataCallSetp = ALLDATA_STEP_SEND_NVA;
        				g_sTaskRunStep.nStartNum = 0;
        			}
        			else
        			{
        				//-数据未读取完，还要继续读取-
        				g_sTaskRunStep.nStartNum += bgetnum ;
        			}
                    nSendVSQ =  bgetnum;
                    nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
					break;
                }
				case ALLDATA_STEP_SEND_YC://上报所有遥测信息
				{
					//FRAME104_FMT_TRACE("case ALLDATA_STEP_SEND_YC://上报所有遥测信息 \n");
					nOutbufLen = yc_total_get(&nPipeIndex, pOutBuff, pAppType);
					break;
				}
                case ALLDATA_STEP_SEND_STOP://数据上报完毕，总召唤激活终止
                {
                    pSendInObjBuff[0]  = 0;
                    pSendInObjBuff[1]  = 0;
                    pSendInObjBuff[2]  = 0;
                    pSendInObjBuff[3]  = 20;//总召唤限定词
                    nSendInObjLen = 4;
                    nSendTI = C_IC_NA_1;
                    nSendCOT = CAUSE_ACTTERM;
                    nSendVSQ = 1;
                    nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
                    g_bAllDataCallFlag = 0;
                    g_bYCYXChangeSendFlag = 1;//总召过后再打开遥信变化上报允许标志
                    g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
                    g_sTaskRunStep.nDataCallStep = ALLDATA_STEP_SEND_DEFAULT;
                    break;
                }
                default:
                {
                    g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
                    break;
                }
            }
            break;
        }
        case STEP_SOE_CHANGE_SEND://soe事件上报(遥信变位)
        {
            nSendTI  =  M_SP_TB_1;
            nSendCOT =  CAUSE_SPONT;  //突发，遥信上报
            uint8 nSendNumMax = 20;  //单次上送信号的最大个数(255-12)/(3+1+7)
            uint16 nStartIndex = g_sTaskRunStep.nStartNum;  //起始序号
            bgetnum = nSendNumMax;  //单次上送信号的最大个数
            nSendInObjLen  = yx_change_get(OBJ_ADDR_LEN, pSendInObjBuff, &nStartIndex, &bgetnum);
            FRAME104_FMT_TRACE("变化遥信数据，返回数据长度 nSendInObjLen =%d, bgetnum = %d \n",nSendInObjLen,bgetnum);
            if(bgetnum == 0)
            {
            	//-没有更多数据需要上报-
                g_sTaskRunStep.nStartNum = 0;
                g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
				g_bYxChanged = FALSE;
                break;
            }
			else if(bgetnum < nSendNumMax)
			{
				//-读取的数量少于要读的数据量，表示本次读取结束-
				g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
				g_sTaskRunStep.nStartNum = 0;
				g_bYxChanged = FALSE;
			}
			else
			{
				//-数据未读取完，还要继续读取-
				g_sTaskRunStep.nStartNum = nStartIndex;
			}
            nSendVSQ =  bgetnum;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
			break;
        }
        case STEP_YC_CHANGE_SEND://遥测数据变化上报
        {   
            nOutbufLen = yc_change_get(&nPipeIndex, pOutBuff, pAppType);
			break;
        }
        case STEP_ENN_DATA_CALL_ALL://--电量召唤--
        {
        	nOutbufLen = en_total_get(&nPipeIndex, pOutBuff, pAppType);
            break;
        }
        case STEP_PRESS_RESET://进程复位处理
        {
        	nPipeIndex = g_sTaskReportPipe.nProcessResetPipe;
            nSendTI  = M_EI_NA_1;
            nSendCOT = CAUSE_INIT;
            nSendVSQ = 1;
            pSendInObjBuff[0] = 0;
            pSendInObjBuff[1] = 0;
            pSendInObjBuff[2] = 0;
            if(g_nRtuResetFlag&0x03)
            {
				pSendInObjBuff[3] = 0x02;//表示远方复位
			}
            else if(g_nRtuResetFlag&0x04)
            {
				pSendInObjBuff[3] = 0x00;//表示停电复位
			}
            else
            {
				pSendInObjBuff[3] = 0x01;//表示本地复位
			}
            nSendInObjLen = 4;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
			g_nRtuResetFlag = 0;
			g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
            break;
        }
        case STEP_DATA_READ://读数据
        {
            g_nDataReadFlagOrAddr = 0;
            g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
            break;
        }   
        case STEP_TIMER_SYN://时钟同步
        {
        	nPipeIndex = g_sTaskReportPipe.nTimeSynPipe;
        	struct timeval tv;
			time_t tmpTime;
    		struct tm *p;
			uint16  wtemp = 0;

            nSendTI  = C_CS_NA_1;
            nSendCOT = CAUSE_ACTTERM;
            nSendVSQ      = 1;
            pSendInObjBuff[0] = 0;
            pSendInObjBuff[1] = 0;
            pSendInObjBuff[2] = 0;
            time( &tmpTime );/*得到当前时间*/
            gettimeofday(&tv,NULL);
            p = localtime( &tmpTime );
            wtemp = tv.tv_usec/1000 + p->tm_sec*1000;
            pSendInObjBuff[3] = wtemp&0xff;
            pSendInObjBuff[4] = (wtemp&0xff00)>>8;
            pSendInObjBuff[5] = p->tm_min;
            pSendInObjBuff[6] = p->tm_hour;
            pSendInObjBuff[7] = p->tm_mday|(p->tm_wday<<5);
            pSendInObjBuff[8] = p->tm_mon+1;
            pSendInObjBuff[9] = p->tm_year;
            if(pSendInObjBuff[9]>100)
            {
				pSendInObjBuff[9] -= 100;
			}
			FRAME104_FMT_TRACE("report_task_run() tv.tv_sec=%ld, tv.tv_usec=%ld \n", tv.tv_sec, tv.tv_usec);
            nSendInObjLen = 10;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
            g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
            break;
        }
        case STEP_REMOTE_CTRL_ACK://应答遥控结束
        {
        	if(g_aCtrlReSendBuff[0] == 0xaa)//防止遥控报文被解析模块覆盖
        	{
				nPipeIndex = g_sTaskReportPipe.nRemoteCtrlAckPipe;
	        	uint8 sendLen = 0;
	        	TOTAL_104_FRAME_T stTotal104Frame;
				memset(&stTotal104Frame, 0, sizeof(TOTAL_104_FRAME_T));
				memcpy(&stTotal104Frame, &g_aCtrlReSendBuff[1], sizeof(g_aCtrlReSendBuff)-1);
		
	            sendLen = stTotal104Frame.nLength - CTRL_UNIT_LABEL_LEN;

				//-第一个字节为标志-
	        	memcpy((void*)pSendInObjBuff, (void*)&g_aCtrlReSendBuff[1+APCI_UNIT_LABEL_LEN], sendLen); 
	            nSendCOT  =  CAUSE_ACTTERM;
			    nOutbufLen = i_frame_data_pack(stTotal104Frame.nDataType, stTotal104Frame.nVSQ, nSendCOT, pSendInObjBuff, sendLen, pOutBuff);
			}
			else
			{
				FRAME104_FMT_TRACE("report_task_run() 遥控报文被覆盖!取消执行! g_aCtrlReSendBuff[0] != 0xaa \n");
			}
        	g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
			break;
        }
        case STEP_NEW_PARA_CALL_ACK://国网新规范参数读取数据处理
        {
            g_nParaGetFlag = 0;
			g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
            break;
		}	
		case STEP_FAULT_CHANGE_SEND:  //故障事件上报
		{
            nSendTI  =  M_FT_NA_1;
            nSendCOT =  CAUSE_SPONT;  //突发，故障信息上报
            uint8 nSendNumMax = 1;  //单次上送信号的最大个数每次只上报一条事件(如何一次上报多条?)
            uint16 nStartIndex = g_sTaskRunStep.nStartNum;  //起始序号
            bgetnum = nSendNumMax;  //单次上送信号的最大个数
            nSendInObjLen  = event_get_faults(OBJ_ADDR_LEN, pSendInObjBuff, nStartIndex, &bgetnum);
            FRAME104_FMT_TRACE("report_task_run() 故障事件数据，返回数据长度 nSendInObjLen =%d, bgetnum = %d \n",nSendInObjLen,bgetnum);
            if(bgetnum == 0)
            {
            	//-没有更多数据需要上报-
                g_sTaskRunStep.nStartNum = 0;
                g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
                break;
            }
			else if(bgetnum < nSendNumMax)
			{
				//-读取的数量少于要读的数据量，表示本次读取结束-
				g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
				g_sTaskRunStep.nStartNum = 0;
			}
			else
			{
				//-数据未读取完，还要继续读取-
				g_sTaskRunStep.nStartNum += bgetnum;
			}
            nSendVSQ =  bgetnum;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
            /*
            nSendInObjLen  = fault_change_get(OBJ_ADDR_LEN, pSendInObjBuff, &nStartIndex, &bgetnum);
            FRAME104_FMT_TRACE("故障事件数据，返回数据长度 nSendInObjLen =%d, bgetnum = %d \n",nSendInObjLen,bgetnum);
            if(bgetnum == 0)
            {
            	//-没有更多数据需要上报-
                g_sTaskRunStep.nStartNum = 0;
                g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
                break;
            }
			else if(bgetnum < nSendNumMax)
			{
				//-读取的数量少于要读的数据量，表示本次读取结束-
				g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
				g_sTaskRunStep.nStartNum = 0;
			}
			else
			{
				//-数据未读取完，还要继续读取-
				g_sTaskRunStep.nStartNum = nStartIndex;
			}
            nSendVSQ =  bgetnum;
            nOutbufLen = i_frame_data_pack(nSendTI,nSendVSQ,nSendCOT,pSendInObjBuff,nSendInObjLen,pOutBuff);
			*/
			break;
		}
		case STEP_IDEL_ACK_SEND://空闲时，对i帧应答确认s帧
		{
			nOutbufLen = s_frame_data_pack(pOutBuff);
			g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
			break;
		}	
        default://任务空闲
        {
			g_sTaskRunStep.nTaskStep = STEP_CONT_DEFAULT;
            break;
        }
    }

	*pPipeIndex = nPipeIndex;
	memcpy(pReportBuf, pOutBuff, nOutbufLen);
	return nOutbufLen;
}
#endif

//打印完整报文信息
void frame_data_print(TOTAL_104_FRAME_T stTotal104Frame)
{	
	FRAME104_FMT_TRACE("报文的完整内容为: 启动字符:%x, 长度:%d \n", stTotal104Frame.nHead, stTotal104Frame.nLength);
	FRAME104_FMT_TRACE("控制域12: %02x %02x \n", stTotal104Frame.sCrtlType12.crtltype12[0], stTotal104Frame.sCrtlType12.crtltype12[1]);
	FRAME104_FMT_TRACE("控制域34: %02x %02x \n", stTotal104Frame.sCrtlType34.crtltype34[0], stTotal104Frame.sCrtlType34.crtltype34[1]);
	FRAME104_FMT_TRACE("类型标识TI:%d, 可变结构VSQ:%d, 传输原因:%d, 公共地址:%d \n", stTotal104Frame.nDataType, stTotal104Frame.nVSQ, 
		    MAKEWORD(stTotal104Frame.nTransCauseL, stTotal104Frame.nTransCauseH), 
		    MAKEWORD(stTotal104Frame.nPubAddrL, stTotal104Frame.nPubAddrH));
	if(stTotal104Frame.nLength > CTRL_UNIT_LABEL_LEN)
	{
		FRAME104_FMT_TRACE("信息对象元素集: \n");
		FRAME104_BUF_TRACE(stTotal104Frame.aObjData, stTotal104Frame.nLength-CTRL_UNIT_LABEL_LEN, "报文: ");
	}
}

//保存完整报文信息至日志
void log_frame_data_save(TOTAL_104_FRAME_T stTotal104Frame, char *path)
{
	char aCrtlType12[2] = {0};      //位置2，控制域1、2
	char aCrtlType34[2] = {0};      //位置4，控制域3、4
	char aObjData[MAX_INFO_OBJ_DATA_LEN] = {0};       //位置12，信息体

	writelog(path, "报文的完整内容为: 启动字符:%x, 长度:%d \n", stTotal104Frame.nHead, stTotal104Frame.nLength);
	buff_space_to_str((char*)stTotal104Frame.sCrtlType12.crtltype12, 2, 2, aCrtlType12);
	writelog(path, "控制域12:%s", aCrtlType12);
	buff_space_to_str((char*)stTotal104Frame.sCrtlType34.crtltype34, 2, 2, aCrtlType34);
	writelog(path, "控制域34:%s", aCrtlType34);
	writelog(path, "类型标识TI:%d, 可变结构VSQ:%d, 传输原因:%d, 公共地址:%d \n", stTotal104Frame.nDataType, stTotal104Frame.nVSQ, 
		    MAKEWORD(stTotal104Frame.nTransCauseL, stTotal104Frame.nTransCauseH), 
		    MAKEWORD(stTotal104Frame.nPubAddrL, stTotal104Frame.nPubAddrH));
	buff_space_to_str((char*)stTotal104Frame.aObjData, stTotal104Frame.nLength-10, MAX_INFO_OBJ_DATA_LEN, aObjData);
	if(stTotal104Frame.nLength > CTRL_UNIT_LABEL_LEN)
	{
		writelog(path, "信息对象元素集:");
		buffer_print(stTotal104Frame.aObjData, stTotal104Frame.nLength-CTRL_UNIT_LABEL_LEN);
	}
}







