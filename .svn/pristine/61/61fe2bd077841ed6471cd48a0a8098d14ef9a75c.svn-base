/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#include "ac_sample.h"
#include "ac_sample698.h"
#include "ac_lib.h"
#include "ac_main.h"
#include "ac_commdrv.h"
#include "ac_mqtt.h"
#include "ac_save.h"
#include "ac_update.h"

/*******************************************************************************
* 全局变量
*******************************************************************************/

FACTORY_VER_TMP_T g_tVer =
{
    "XXXX",      /**< 厂商代码           */
    "XXXX",      /**< 软件版本号         */
    "211022",    /**< 软件版本日期       */
    "DM01",      /**< 硬件版本号         */
    "200624",    /**< 硬件版本日期       */
    "XXXX-18",   /**< 厂家扩展信息       */
};

int32                    fd_ac = -1;              // 句柄

COMMON_DATA_TYPE         gCommonData;             // 公共交互变量
PRIVTE_MEASURE_TYPE      gPriMeasurePara;         // 私有计量参数
PRIVATE_DATA_TYPE        gPrivateData;            // 私有数据变量
MEA_REAL_DATA_COM_TYPE   gMeaRealTimData;         // 计量实时数据

// 交采与统计交互的参数的声明
TERM_SHARE_DATA_TYPE    TermSharedData;           // 交采共享内存数据的声明
SHARED_MEM_MARK_BYTE    SharedMemMark;            // 刷新共享数据标记

//历史数据统计变量
TERM_SHARE_DATA_TYPE    MidTermSharedData;          // (中间变量)交采共享内存数据的声明
PRIVTE_MEASURE_TYPE     gMidPriMeasurePara;         // (中间变量)私有计量参数
TERM_SHARE_DATA_TYPE    HisTermSharedData;          // (历史)交采共享内存数据的声明
PRIVTE_MEASURE_TYPE     gHisPriMeasurePara;         // (历史)私有计量参数

//历史数据统计变量
UPDATE_FLAG_T           gUpdateFlag = {0};          //采集数据是否越过变化死区标志

//切换波特率之后，重新召唤版本信息标志
uint8                   gBaudRateChangedFlag = 0;  //切换波特率标志
uint8                   gCallVerAfterBaudRateChanged = 0;  //切换波特率之后，重新召唤版本信息标志

//交采参数初始化标志
uint8 g_nAcParamInitFlag = 0;  //交采参数初始化标志

//交采数据初始化标志
uint8 g_nAcDataInitFlag = 0;  //交采数据初始化标志

//交采需量初始化标志
uint8 g_nAcDemandInitFlag = 0;  //交采需量初始化标志

//交采需量初始化标志
uint8 g_nPowerOffFlag = 0;     //掉电标志

//交采模块对时标志
uint8 g_nAcSetDateFlag = 0;  //交采模块对时标志(日期)
uint8 g_nAcSetTimeFlag = 0;  //交采模块对时标志(时间)
uint8 g_nAcReadDemandPeriodFlag = 1;  //交采模块查询最大需量周期标志
uint8 g_nAcReadSlipTimeFlag = 1;  //交采模块查询滑差时间标志

uint8 g_nAcReadZoneNumFlag = 1;  //交采模块当前套/备用套时区标志 数量
uint8 g_nAcReadDayTableNumFlag = 1;  //交采模块当前套/备用套日时段表标志 数量
uint8 g_nAcReadDayPeriNumFlag = 1;  //交采模块当前套/备用套日时段标志 数量
uint8 g_nAcReadPeriNumFlagArr[OOP_MAX_DAYPERIODS];//当前套/备用套日时段表(设置交采模块)标志 数量
uint8 g_nAcReadCostRateNumFlag = 1;  //交采模块费率数标志 数量
uint8 g_nAcReadHolidayNumFlag = 1;  //交采模块公共假日数标志 数量

uint8 g_nAcReadCurrZoneFlag = 1;  //交采模块当前套时区标志
uint8 g_nAcReadAddiZoneFlag = 1;  //交采模块备用套时区标志
uint8 g_nAcReadCurrDayPeriFlag = 1;  //交采模块当前套日时段标志
uint8 g_nAcReadAddiDayPeriFlag = 1;  //交采模块备用套日时段标志
uint8 g_nAcReadCurrDayFlagArr[OOP_MAX_DAYPERIODS];//当前套日时段表(设置交采模块)标志
uint8 g_nAcReadAddiDayFlagArr[OOP_MAX_DAYPERIODS];//备用套日时段表(设置交采模块)标志

uint8 g_nRefreshCurrDayFlagArr[OOP_MAX_DAYPERIODS] = {0};//当前套日时段表 刷新标志
uint8 g_nRefreshAddiDayFlagArr[OOP_MAX_DAYPERIODS] = {0};//备用套日时段表 刷新标志

uint8 g_nAddiZoneChangeTimeFlag = 1;//备用套时区表切换时间 标志 查询
uint8 g_nAddiPeriChangeTimeFlag = 1;//备用套日时段表切换时间 标志 查询
uint8 g_nActPowCombWordFlag = 1;//有功组合方式特征字 标志 查询
uint8 g_nReaPowComb1WordFlag = 1;//无功组合方式1特征字 标志 查询
uint8 g_nReaPowComb2WordFlag = 1;//无功组合方式2特征字 标志 查询

uint8 g_nReadCurrDayPeriLabelFlag = 1;//交采模块目前运行的日时段表是第几套 查询标志
uint8 g_nRefreshCurrDayPeriLabelFlag = 0;//交采模块目前运行的日时段表是第几套 已刷新标志

uint8 NO_DATA[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
extern uint8 gdelay;
/*******************************************************************************
* 交采模块实时任务
********************************************************************************/
const uint32 AMR_RT_TASK_ID_EX[] =
{
    /* 电能 */
    0x0000ff00,             //(当前)组合有功电能块
    0x0001ff00,             //(当前)正向有功电能总尖峰平谷示值
    0x00150000,             //(A相)正向有功总电能示值
    0x00290000,             //(B相)正向有功总电能示值
    0x003d0000,             //(C相)正向有功总电能示值
    0x0002ff00,             //(当前)反向有功电能总尖峰平谷示值
    0x00160000,             //(A相)反向有功总电能示值
    0x002a0000,             //(B相)反向有功总电能示值
    0x003e0000,             //(C相)反向有功总电能示值
    0x0003ff00,             //(当前)正向无功电能总尖峰平谷示值
    0x00170000,             //(A相)正向无功总电能示值
    0x002b0000,             //(B相)正向无功总电能示值
    0x003f0000,             //(C相)正向无功总电能示值
    0x0004ff00,             //(当前)反向无功电能总尖峰平谷示值
    0x00180000,             //(A相)反向无功总电能示值
    0x002c0000,             //(B相)反向无功总电能示值
    0x00400000,             //(C相)反向无功总电能示值
    0x0005ff00,             //(当前)第一象限无功总尖峰平谷电能示值
    0x00190000,             //(A相)第一象限无功电能
    0x002d0000,             //(B相)第一象限无功电能
    0x00410000,             //(C相)第一象限无功电能
    0x0006ff00,             //(当前)第二象限无功总尖峰平谷电能示值
    0x001a0000,             //(A相)第二象限无功电能
    0x002e0000,             //(B相)第二象限无功电能
    0x00420000,             //(C相)第二象限无功电能
    0x0007ff00,             //(当前)第三象限无功总尖峰平谷电能示值
    0x001b0000,             //(A相)第三象限无功电能
    0x002f0000,             //(B相)第三象限无功电能
    0x00430000,             //(C相)第三象限无功电能
    0x0008ff00,             //(当前)第四象限无功总尖峰平谷电能示值
    0x001c0000,             //(A相)第四象限无功电能
    0x00300000,             //(B相)第四象限无功电能
    0x00440000,             //(C相)第四象限无功电能
    /*
    0x0009ff00,             //(当前)正向视在电能数据块(回复异常 33 32 3C 33 34 34 )
    0x001d0000,             //(A相)正向视在电能(回复异常 33 33 50 33 34 34 )
    0x00310000,             //(B相)正向视在电能(回复异常 33 33 64 33 34 34 )
    0x00450000,             //(C相)正向视在电能(回复异常 33 33 78 33 34 34 )
    
    0x000aff00,             //(当前)反向视在电能数据块(回复异常 33 32 3D 33 35 33 33 )
    0x001e0000,             //(A相)反向视在电能(回复异常 33 33 51 33 34 34 )
    0x00320000,             //(B相)反向视在电能(回复异常 33 33 65 33 34 34 )
    0x00460000,             //(C相)反向视在电能(回复异常 33 33 79 33 34 34 )
    */

    /* 电能(高精度 四位小数) */
    0xf100ff00,             //(当前)组合有功电能块
    
    0xf101ff00,             //(当前)正向有功电能总尖峰平谷示值
    0xf1150000,             //(A相)正向有功总电能示值
    0xf1290000,             //(B相)正向有功总电能示值
    0xf13d0000,             //(C相)正向有功总电能示值
    
    0xf102ff00,             //(当前)反向有功电能总尖峰平谷示值
    0xf1160000,             //(A相)反向有功总电能示值
    0xf12a0000,             //(B相)反向有功总电能示值
    0xf13e0000,             //(C相)反向有功总电能示值
    
    0xf103ff00,             //(当前)正向无功电能总尖峰平谷示值
    0xf1170000,             //(A相)正向无功总电能示值
    0xf12b0000,             //(B相)正向无功总电能示值
    0xf13f0000,             //(C相)正向无功总电能示值
    
    0xf104ff00,             //(当前)反向无功电能总尖峰平谷示值
    0xf1180000,             //(A相)反向无功总电能示值
    0xf12c0000,             //(B相)反向无功总电能示值
    0xf1400000,             //(C相)反向无功总电能示值
    
    0xf105ff00,             //(当前)第一象限无功总尖峰平谷电能示值
    0xf1190000,             //(A相)第一象限无功电能
    0xf12d0000,             //(B相)第一象限无功电能
    0xf1410000,             //(C相)第一象限无功电能
    
    0xf106ff00,             //(当前)第二象限无功总尖峰平谷电能示值
    0xf11a0000,             //(A相)第二象限无功电能
    0xf12e0000,             //(B相)第二象限无功电能
    0xf1420000,             //(C相)第二象限无功电能
    
    0xf107ff00,             //(当前)第三象限无功总尖峰平谷电能示值
    0xf11b0000,             //(A相)第三象限无功电能
    0xf12f0000,             //(B相)第三象限无功电能
    0xf1430000,             //(C相)第三象限无功电能
    
    0xf108ff00,             //(当前)第四象限无功总尖峰平谷电能示值
    0xf11c0000,             //(A相)第四象限无功电能
    0xf1300000,             //(B相)第四象限无功电能
    0xf1440000,             //(C相)第四象限无功电能
    /*
    0xf109ff00,             //(当前)正向视在电能数据块(回复异常 33 32 3C 33 34 34 )
    0xf11d0000,             //(A相)正向视在电能(回复异常 33 33 50 33 34 34 )
    0xf1310000,             //(B相)正向视在电能(回复异常 33 33 64 33 34 34 )
    0xf1450000,             //(C相)正向视在电能(回复异常 33 33 78 33 34 34 )
    
    0xf10aff00,             //(当前)反向视在电能数据块(回复异常 33 32 3D 33 35 33 33 )
    0xf11e0000,             //(A相)反向视在电能(回复异常 33 33 51 33 34 34 )
    0xf1320000,             //(B相)反向视在电能(回复异常 33 33 65 33 34 34 )
    0xf1460000,             //(C相)反向视在电能(回复异常 33 33 79 33 34 34 )
    */
    
    /* 需量 */
    0x0101ff00,             //(当前)正向有功总尖峰平谷最大需量
    0x01150000,             //(A相)正向有功总最大需量
    0x01290000,             //(B相)正向有功总最大需量
    0x013d0000,             //(C相)正向有功总最大需量
    0x0102ff00,             //(当前)反向有功总尖峰平谷最大需量
    0x01160000,             //(A相)反向有功最大需量
    0x012a0000,             //(B相)反向有功最大需量
    0x013e0000,             //(C相)反向有功最大需量
    0x0103ff00,             //(当前)正向无功总尖峰平谷最大需量
    0x01170000,             //(A相)组合无功1最大需量
    0x012b0000,             //(B相)组合无功1最大需量
    0x013f0000,             //(C相)组合无功1最大需量
    0x0104ff00,             //(当前)反向无功总尖峰平谷最大需量
    0x01180000,             //(A相)组合无功2最大需量
    0x012c0000,             //(B相)组合无功2最大需量
    0x01400000,             //(C相)组合无功2最大需量
    0x0105FF00,             //(当前)第一象限最大需量总尖峰平谷
    0x01190000,             //(A相)第一象限最大需量
    0x012d0000,             //(B相)第一象限最大需量
    0x01410000,             //(C相)第一象限最大需量
    0x0106FF00,             //(当前)第二象限最大需量总尖峰平谷
    0x011a0000,             //(A相)第二象限最大需量
    0x012e0000,             //(B相)第二象限最大需量
    0x01420000,             //(C相)第二象限最大需量
    0x0107FF00,             //(当前)第三象限最大需量总尖峰平谷
    0x011b0000,             //(A相)第三象限最大需量
    0x012f0000,             //(B相)第三象限最大需量
    0x01430000,             //(C相)第三象限最大需量
    0x0108FF00,             //(当前)第四象限最大需量总尖峰平谷
    0x011c0000,             //(A相)第四象限最大需量
    0x01300000,             //(B相)第四象限最大需量
    0x01440000,             //(C相)第四象限最大需量
    0x0109ff00,             //(当前)正向视在最大需量总尖峰平谷
    0x011d0000,             //(A相)正向视在最大需量
    0x01310000,             //(B相)正向视在最大需量
    0x01450000,             //(C相)正向视在最大需量
    0x010aff00,             //(当前)反向视在最大需量总尖峰平谷
    0x011e0000,             //(A相)反向视在最大需量
    0x01320000,             //(B相)反向视在最大需量
    0x01460000,             //(C相)反向视在最大需量

    /* 实时数据 */
    0x0201ff00, 			 //(当前)三相电压
    0x0202ff00, 			 //(当前)三相电流
    0x02800001,              //零序电流
    
    0x028800FF,             //三相电压相角与三相电流相角
    0x0207FF00,              //电压电流相角 
    0x0203ff00,              //(当前)瞬时正向有功功率
    0x0204ff00, 		     //(当前)瞬时正向无功功率
    0x0205FF00,              //(当前)瞬时正向视在功率
    0x0206ff00,              //(当前)瞬时总及分相功率因数
    0x02800002,              //电网频率
    0x04000501,             //电表运行状态字1
    0x04000502,             //电表运行状态字2
    0x04000503,             //电表运行状态字3
    0x04000504,             //电表运行状态字4
    0x04000505,             //电表运行状态字5
    0x04000506,             //电表运行状态字6
    0x04000507,             //电表运行状态字7

    /* 谐波 */
    0x020A01FF,           	//A相电压谐波含有率（总及2…n次）
    0x020A02FF,           	//B相电压谐波含有率（总及2…n次）
    0x020A03FF,           	//C相电压谐波含有率（总及2…n次）
    0x020B01FF,           	//A相电流谐波含有率（总及2…n次）
    0x020B02FF,           	//B相电流谐波含有率（总及2…n次）
    0x020B03FF,           	//C相电流谐波含有率（总及2…n次）
    
    0x020C01FF,           	//A相电压谐波含量（总及2…n次）
    0x020C02FF,           	//B相电压谐波含量（总及2…n次）
    0x020C03FF,           	//C相电压谐波含量（总及2…n次）
    0x020D01FF,           	//A相电流谐波含量（总及2…n次）
    0x020D02FF,           	//B相电流谐波含量（总及2…n次）
    0x020D03FF,           	//C相电流谐波含量（总及2…n次）
};

uint16 gAmrRtTaskIdExNum = 0;  //AMR_RT_TASK_ID_EX的元素个数


/*******************************************************************************
* 交采模块实时采集任务(修改后, 添加采集周期)
********************************************************************************/
const uint32 gSampleTaskEnergy[] =
{
    /* 电能 */
    
    0x0000ff00,             //(当前)组合有功电能块
    
    0x0001ff00,             //(当前)正向有功电能总尖峰平谷示值
    0x00150000,             //(A相)正向有功总电能示值
    0x00290000,             //(B相)正向有功总电能示值
    0x003d0000,             //(C相)正向有功总电能示值
    
    0x0002ff00,             //(当前)反向有功电能总尖峰平谷示值
    0x00160000,             //(A相)反向有功总电能示值
    0x002a0000,             //(B相)反向有功总电能示值
    0x003e0000,             //(C相)反向有功总电能示值
    
    0x0003ff00,             //(当前)正向无功电能总尖峰平谷示值
    0x00170000,             //(A相)正向无功总电能示值
    0x002b0000,             //(B相)正向无功总电能示值
    0x003f0000,             //(C相)正向无功总电能示值
    
    0x0004ff00,             //(当前)反向无功电能总尖峰平谷示值
    0x00180000,             //(A相)反向无功总电能示值
    0x002c0000,             //(B相)反向无功总电能示值
    0x00400000,             //(C相)反向无功总电能示值
    
    0x0005ff00,             //(当前)第一象限无功总尖峰平谷电能示值
    0x00190000,             //(A相)第一象限无功电能
    0x002d0000,             //(B相)第一象限无功电能
    0x00410000,             //(C相)第一象限无功电能
    
    0x0006ff00,             //(当前)第二象限无功总尖峰平谷电能示值
    0x001a0000,             //(A相)第二象限无功电能
    0x002e0000,             //(B相)第二象限无功电能
    0x00420000,             //(C相)第二象限无功电能
    
    0x0007ff00,             //(当前)第三象限无功总尖峰平谷电能示值
    0x001b0000,             //(A相)第三象限无功电能
    0x002f0000,             //(B相)第三象限无功电能
    0x00430000,             //(C相)第三象限无功电能
    
    0x0008ff00,             //(当前)第四象限无功总尖峰平谷电能示值
    0x001c0000,             //(A相)第四象限无功电能
    0x00300000,             //(B相)第四象限无功电能
    0x00440000,             //(C相)第四象限无功电能
    
    /*
    0x0009ff00,             //(当前)正向视在电能数据块(回复异常 33 32 3C 33 34 34 )
    0x001d0000,             //(A相)正向视在电能(回复异常 33 33 50 33 34 34 )
    0x00310000,             //(B相)正向视在电能(回复异常 33 33 64 33 34 34 )
    0x00450000,             //(C相)正向视在电能(回复异常 33 33 78 33 34 34 )
    
    0x000aff00,             //(当前)反向视在电能数据块(回复异常 33 32 3D 33 35 33 33 )
    0x001e0000,             //(A相)反向视在电能(回复异常 33 33 51 33 34 34 )
    0x00320000,             //(B相)反向视在电能(回复异常 33 33 65 33 34 34 )
    0x00460000,             //(C相)反向视在电能(回复异常 33 33 79 33 34 34 )
    */

    /* 实时数据 */
    
    0x0201ff00, 			 //(当前)三相电压
    0x0202ff00, 			 //(当前)三相电流
    0x02800001,              //零序电流
    
0x028800FF,             //三相电压相角与三相电流相角
    0x0207FF00,              //电压电流相角 
    0x0203ff00,              //(当前)瞬时正向有功功率
    0x0204ff00, 		     //(当前)瞬时正向无功功率
    0x0205FF00,              //(当前)瞬时正向视在功率
    0x0206ff00,              //(当前)瞬时总及分相功率因数
    0x02800002,              //电网频率
    0x04000501,             //电表运行状态字1
    0x04000502,             //电表运行状态字2
    0x04000503,             //电表运行状态字3
    0x04000504,             //电表运行状态字4
    0x04000505,             //电表运行状态字5
    0x04000506,             //电表运行状态字6
    0x04000507,             //电表运行状态字7
    
#if HD_ENERGY_EN == 1
    /* 电能(高精度 四位小数) */

    0xf100ff00,             //(当前)组合有功电能块
    
    0xf101ff00,             //(当前)正向有功电能总尖峰平谷示值
    0xf1150000,             //(A相)正向有功总电能示值
    0xf1290000,             //(B相)正向有功总电能示值
    0xf13d0000,             //(C相)正向有功总电能示值
    
    0xf102ff00,             //(当前)反向有功电能总尖峰平谷示值
    0xf1160000,             //(A相)反向有功总电能示值
    0xf12a0000,             //(B相)反向有功总电能示值
    0xf13e0000,             //(C相)反向有功总电能示值
    
    0xf103ff00,             //(当前)正向无功电能总尖峰平谷示值
    0xf1170000,             //(A相)正向无功总电能示值
    0xf12b0000,             //(B相)正向无功总电能示值
    0xf13f0000,             //(C相)正向无功总电能示值
    
    0xf104ff00,             //(当前)反向无功电能总尖峰平谷示值
    0xf1180000,             //(A相)反向无功总电能示值
    0xf12c0000,             //(B相)反向无功总电能示值
    0xf1400000,             //(C相)反向无功总电能示值
    
    0xf105ff00,             //(当前)第一象限无功总尖峰平谷电能示值
    0xf1190000,             //(A相)第一象限无功电能
    0xf12d0000,             //(B相)第一象限无功电能
    0xf1410000,             //(C相)第一象限无功电能
    
    0xf106ff00,             //(当前)第二象限无功总尖峰平谷电能示值
    0xf11a0000,             //(A相)第二象限无功电能
    0xf12e0000,             //(B相)第二象限无功电能
    0xf1420000,             //(C相)第二象限无功电能
    
    0xf107ff00,             //(当前)第三象限无功总尖峰平谷电能示值
    0xf11b0000,             //(A相)第三象限无功电能
    0xf12f0000,             //(B相)第三象限无功电能
    0xf1430000,             //(C相)第三象限无功电能
    
    0xf108ff00,             //(当前)第四象限无功总尖峰平谷电能示值
    0xf11c0000,             //(A相)第四象限无功电能
    0xf1300000,             //(B相)第四象限无功电能
    0xf1440000,             //(C相)第四象限无功电能

    /*
    0xf109ff00,             //(当前)正向视在电能数据块(回复异常 33 32 3C 33 34 34 )
    0xf11d0000,             //(A相)正向视在电能(回复异常 33 33 50 33 34 34 )
    0xf1310000,             //(B相)正向视在电能(回复异常 33 33 64 33 34 34 )
    0xf1450000,             //(C相)正向视在电能(回复异常 33 33 78 33 34 34 )
    
    0xf10aff00,             //(当前)反向视在电能数据块(回复异常 33 32 3D 33 35 33 33 )
    0xf11e0000,             //(A相)反向视在电能(回复异常 33 33 51 33 34 34 )
    0xf1320000,             //(B相)反向视在电能(回复异常 33 33 65 33 34 34 )
    0xf1460000,             //(C相)反向视在电能(回复异常 33 33 79 33 34 34 )
    */
#endif

};

uint16 gSampleTaskEnergyNum = 0;  //gSampleTaskEnergy的元素个数

/*******************************************************************************
* 交采模块实时采集任务(修改后, 添加采集周期)
********************************************************************************/
const uint32 gSampleTaskDemand[] =
{
    /* 需量 */
    0x0101ff00,             //(当前)正向有功总尖峰平谷最大需量
    0x01150000,             //(A相)正向有功总最大需量
    0x01290000,             //(B相)正向有功总最大需量
    0x013d0000,             //(C相)正向有功总最大需量
    
    0x0102ff00,             //(当前)反向有功总尖峰平谷最大需量
    0x01160000,             //(A相)反向有功最大需量
    0x012a0000,             //(B相)反向有功最大需量
    0x013e0000,             //(C相)反向有功最大需量
    
    0x0103ff00,             //(当前)正向无功总尖峰平谷最大需量
    0x01170000,             //(A相)组合无功1最大需量
    0x012b0000,             //(B相)组合无功1最大需量
    0x013f0000,             //(C相)组合无功1最大需量
    
    0x0104ff00,             //(当前)反向无功总尖峰平谷最大需量
    0x01180000,             //(A相)组合无功2最大需量
    0x012c0000,             //(B相)组合无功2最大需量
    0x01400000,             //(C相)组合无功2最大需量
    
    0x0105FF00,             //(当前)第一象限最大需量总尖峰平谷
    0x01190000,             //(A相)第一象限最大需量
    0x012d0000,             //(B相)第一象限最大需量
    0x01410000,             //(C相)第一象限最大需量
    
    0x0106FF00,             //(当前)第二象限最大需量总尖峰平谷
    0x011a0000,             //(A相)第二象限最大需量
    0x012e0000,             //(B相)第二象限最大需量
    0x01420000,             //(C相)第二象限最大需量
    
    0x0107FF00,             //(当前)第三象限最大需量总尖峰平谷
    0x011b0000,             //(A相)第三象限最大需量
    0x012f0000,             //(B相)第三象限最大需量
    0x01430000,             //(C相)第三象限最大需量
    
    0x0108FF00,             //(当前)第四象限最大需量总尖峰平谷
    0x011c0000,             //(A相)第四象限最大需量
    0x01300000,             //(B相)第四象限最大需量
    0x01440000,             //(C相)第四象限最大需量
    /*
    0x0109ff00,             //(当前)正向视在最大需量总尖峰平谷
    0x011d0000,             //(A相)正向视在最大需量(回复异常 33 33 50 34 34 34 )
    0x01310000,             //(B相)正向视在最大需量(回复异常 33 33 64 34 34 34 )
    0x01450000,             //(C相)正向视在最大需量(回复异常 33 33 78 34 34 34 )
    
    0x010aff00,             //(当前)反向视在最大需量总尖峰平谷(回复异常 33 32 3D 34 35 33 33 )
    0x011e0000,             //(A相)反向视在最大需量(回复异常 33 33 51 34 34 34 )
    0x01320000,             //(B相)反向视在最大需量(回复异常 33 33 65 34 34 34 )
    0x01460000,             //(C相)反向视在最大需量(回复异常 33 33 79 34 34 34 )
    */

    /* 实时数据 */
    0x0201ff00, 			 //(当前)三相电压
    0x0202ff00, 			 //(当前)三相电流
    0x02800001,              //零序电流
    
0x028800FF,             //三相电压相角与三相电流相角
    0x0207FF00,              //电压电流相角 
    0x0203ff00,              //(当前)瞬时正向有功功率
    0x0204ff00, 		     //(当前)瞬时正向无功功率
    0x0205FF00,              //(当前)瞬时正向视在功率
    0x0206ff00,              //(当前)瞬时总及分相功率因数
    0x02800002,              //电网频率
    0x04000501,             //电表运行状态字1
    0x04000502,             //电表运行状态字2
    0x04000503,             //电表运行状态字3
    0x04000504,             //电表运行状态字4
    0x04000505,             //电表运行状态字5
    0x04000506,             //电表运行状态字6
    0x04000507,             //电表运行状态字7
};

uint16 gSampleTaskDemandNum = 0;  //gSampleTaskDemand的元素个数

/*******************************************************************************
* 交采模块实时采集任务(修改后, 添加采集周期)
********************************************************************************/
const uint32 gSampleTaskRealtime[] =
{
    /* 实时数据 */
    0x0201ff00, 			 //(当前)三相电压
    0x0202ff00, 			 //(当前)三相电流
    0x02800001,              //零序电流
    
    0x028800FF,             //三相电压相角与三相电流相角
    0x0207FF00,              //电压电流相角 
    0x0203ff00,              //(当前)瞬时正向有功功率
    0x0204ff00, 		     //(当前)瞬时正向无功功率
    0x0205FF00,              //(当前)瞬时正向视在功率
    0x0206ff00,              //(当前)瞬时总及分相功率因数
    0x02800002,              //电网频率
    0x04000501,             //电表运行状态字1
    0x04000502,             //电表运行状态字2
    0x04000503,             //电表运行状态字3
    0x04000504,             //电表运行状态字4
    0x04000505,             //电表运行状态字5
    0x04000506,             //电表运行状态字6
    0x04000507,             //电表运行状态字7
};

uint16 gSampleTaskRealtimeNum = 0;  //gSampleTaskRealtime的元素个数

/*******************************************************************************
* 交采模块实时采集任务(修改后, 添加采集周期)
********************************************************************************/
const uint32 gSampleTaskHarmonic[] =
{
    /* 谐波 */
    0x020A01FF,           	//A相电压谐波含有率（总及2…n次）
    0x020A02FF,           	//B相电压谐波含有率（总及2…n次）
    0x020A03FF,           	//C相电压谐波含有率（总及2…n次）
    0x020B01FF,           	//A相电流谐波含有率（总及2…n次）
    0x020B02FF,           	//B相电流谐波含有率（总及2…n次）
    0x020B03FF,           	//C相电流谐波含有率（总及2…n次）
    
    0x020C01FF,           	//A相电压谐波含量（总及2…n次）
    0x020C02FF,           	//B相电压谐波含量（总及2…n次）
    0x020C03FF,           	//C相电压谐波含量（总及2…n次）
    0x020D01FF,           	//A相电流谐波含量（总及2…n次）
    0x020D02FF,           	//B相电流谐波含量（总及2…n次）
    0x020D03FF,           	//C相电流谐波含量（总及2…n次）

    /* 实时数据 */
    0x0201ff00, 			 //(当前)三相电压
    0x0202ff00, 			 //(当前)三相电流
    0x02800001,              //零序电流
    
0x028800FF,             //三相电压相角与三相电流相角
    0x0207FF00,              //电压电流相角 
    0x0203ff00,              //(当前)瞬时正向有功功率
    0x0204ff00, 		     //(当前)瞬时正向无功功率
    0x0205FF00,              //(当前)瞬时正向视在功率
    0x0206ff00,              //(当前)瞬时总及分相功率因数
    0x02800002,              //电网频率
    0x04000501,             //电表运行状态字1
    0x04000502,             //电表运行状态字2
    0x04000503,             //电表运行状态字3
    0x04000504,             //电表运行状态字4
    0x04000505,             //电表运行状态字5
    0x04000506,             //电表运行状态字6
    0x04000507,             //电表运行状态字7
};

uint16 gSampleTaskHarmonicNum = 0;  //gSampleTaskHarmonic的元素个数


/*********************************************************************
功能描述：
输入参数：
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 MakeFrameReadACModule(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    int 	j;
    uint8	DI_Buf[4] = {0};
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x1C;
    PrtlData[9] = sizeof(AMR_RT_TASK_ID_EX)&0xFF;
    PrtlData[10] = (sizeof(AMR_RT_TASK_ID_EX)>>8)&0xFF;

    for(i = 0; i < (sizeof(AMR_RT_TASK_ID_EX)/4); i++)
    {
        memset(DI_Buf, 0, 4);
        memcpy(DI_Buf, (uint8*)&AMR_RT_TASK_ID_EX[i], 4);
        for(j = 0; j < 4; j++)
        {
            PrtlData[11 + i*4 + j] =  DI_Buf[j] + 0x33;
        }
    }

    CurIndex = PrtlData[9] + (PrtlData[10]<<8) + 11;

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述：
输入参数：
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 MakeFrameReadEnergy(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    int 	j;
    uint8	DI_Buf[4] = {0};
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x1C;
    PrtlData[9] = sizeof(gSampleTaskEnergy)&0xFF;
    PrtlData[10] = (sizeof(gSampleTaskEnergy)>>8)&0xFF;

    for(i = 0; i < (sizeof(gSampleTaskEnergy)/4); i++)
    {
        memset(DI_Buf, 0, 4);
        memcpy(DI_Buf, (uint8*)&gSampleTaskEnergy[i], 4);
        for(j = 0; j < 4; j++)
        {
            PrtlData[11 + i*4 + j] =  DI_Buf[j] + 0x33;
        }
    }

    CurIndex = PrtlData[9] + (PrtlData[10]<<8) + 11;

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述：
输入参数：
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 MakeFrameReadDemand(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    int 	j;
    uint8	DI_Buf[4] = {0};
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x1C;
    PrtlData[9] = sizeof(gSampleTaskDemand)&0xFF;
    PrtlData[10] = (sizeof(gSampleTaskDemand)>>8)&0xFF;

    for(i = 0; i < (sizeof(gSampleTaskDemand)/4); i++)
    {
        memset(DI_Buf, 0, 4);
        memcpy(DI_Buf, (uint8*)&gSampleTaskDemand[i], 4);
        for(j = 0; j < 4; j++)
        {
            PrtlData[11 + i*4 + j] =  DI_Buf[j] + 0x33;
        }
    }

    CurIndex = PrtlData[9] + (PrtlData[10]<<8) + 11;

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述：
输入参数：
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 MakeFrameReadRealtime(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    int 	j;
    uint8	DI_Buf[4] = {0};
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x1C;
    PrtlData[9] = sizeof(gSampleTaskRealtime)&0xFF;
    PrtlData[10] = (sizeof(gSampleTaskRealtime)>>8)&0xFF;

    for(i = 0; i < (sizeof(gSampleTaskRealtime)/4); i++)
    {
        memset(DI_Buf, 0, 4);
        memcpy(DI_Buf, (uint8*)&gSampleTaskRealtime[i], 4);
        for(j = 0; j < 4; j++)
        {
            PrtlData[11 + i*4 + j] =  DI_Buf[j] + 0x33;
        }
    }

    CurIndex = PrtlData[9] + (PrtlData[10]<<8) + 11;

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述：
输入参数：
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 MakeFrameReadHarmonic(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    int 	j;
    uint8	DI_Buf[4] = {0};
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x1C;
    PrtlData[9] = sizeof(gSampleTaskHarmonic)&0xFF;
    PrtlData[10] = (sizeof(gSampleTaskHarmonic)>>8)&0xFF;

    for(i = 0; i < (sizeof(gSampleTaskHarmonic)/4); i++)
    {
        memset(DI_Buf, 0, 4);
        memcpy(DI_Buf, (uint8*)&gSampleTaskHarmonic[i], 4);
        for(j = 0; j < 4; j++)
        {
            PrtlData[11 + i*4 + j] =  DI_Buf[j] + 0x33;
        }
    }

    CurIndex = PrtlData[9] + (PrtlData[10]<<8) + 11;

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}


int STDAC_CheckRspFrame(uint8 *pBuf, uint16 bufLen)
{
    uint8	frameCs;
    uint16	frameLen;
    uint16	DataLen = 0;


    if((pBuf[8] & 0xF0) != 0x90)
    {
        return -4;
    }

    DataLen = pBuf[9] + (pBuf[10]<<8);
    frameLen = 0x0d + DataLen;
    if (frameLen > bufLen)
    {
        ACCOMM_FMT_LOGCC("frameLen=%d, bufLen=%d \n", frameLen, bufLen);
        return -1;
    }
    
    frameCs = CalCs(pBuf, frameLen - 2);

    if((pBuf[0] == 0x68)
            && (pBuf[7] == pBuf[0]) /*-北京系统电表地址补0或AA可变-*/
            && (pBuf[frameLen - 2] == frameCs)
            && (pBuf[frameLen - 1] == 0x16))
    {
        if(frameLen >= 0x0E)
        {
            return 1;
        }
    }
    else
    {
        return -2;
    }

    return -3;
}

int STDAC_CheckRspFrame_Single(uint8 *pBuf, uint16 bufLen)
{
    uint8	frameCs;
    uint16	frameLen;
    uint16	DataLen = 0;

    //新增交采模块主动上报功能, 不再判断控制域
    /*
    if((pBuf[8] & 0xF0) != 0x90)
    {
        return 0;
    }
    */

    DataLen = pBuf[9];
    frameLen = 8+2+2 + DataLen;

    if (frameLen > bufLen)
    {
        ACCOMM_FMT_LOGCC("frameLen=%d, bufLen=%d \n", frameLen, bufLen);
        return -1;
    }
    
    frameCs = CalCs(pBuf, frameLen - 2);

    if((pBuf[0] == 0x68)
            && (pBuf[7] == pBuf[0])
            && (pBuf[frameLen - 2] == frameCs)
            && (pBuf[frameLen - 1] == 0x16))
    {
        if(frameLen >= 0x0D)
        {
            return 1;
        }
    }
    else
    {
        return -2;
    }

    return -3;
}

/*******************************************************************************
* 函数名称: ac_curtime_get
* 函数功能: 时间管理初始化
* 输入参数: 无
* 输出参数: 无   
* 返 回 值: 无
*******************************************************************************/
void ac_curtime_get(YYMMDDWWhhnnss_TYPE *curtime)
{
    time_t now;
    struct tm *systime = NULL;
    
    time(&now);
    systime = localtime(&now);          // 结构元素tm_sec,tm_min,tm_hour,tm_mday,tm_mon,tm_year

    curtime->ss = Hex2Bcd((uint8)systime->tm_sec);
    curtime->nn = Hex2Bcd((uint8)systime->tm_min);
    curtime->hh = Hex2Bcd((uint8)systime->tm_hour);
    curtime->DD = Hex2Bcd((uint8)systime->tm_mday);            
    curtime->WW = Hex2Bcd((uint8)systime->tm_wday);         // 星期
    curtime->MM = Hex2Bcd((uint8)systime->tm_mon + 1);      // 月份0代表1月
    curtime->YY = Hex2Bcd((uint8)(systime->tm_year - 100)); // 取年份的后两位

    return;
}

/*********************************************************************
功能描述： make_frame_ac_date
输入参数：组帧 交采模块校时(日期)
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_ac_date(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    uint8   MeterAddr[6];
    YYMMDDWWhhnnss_TYPE curtime;

    ac_curtime_get(&curtime);    
    //SaveTimeManageInit();
    //TimeManageInit();

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x10;

    PrtlData[10] = 0x01;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = curtime.WW;
    PrtlData[23] = curtime.DD;
    PrtlData[24] = curtime.MM;
    PrtlData[25] = curtime.YY;

    CurIndex = 26;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_ac_time
输入参数：组帧 交采模块校时(时间)
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_ac_time(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    uint8   MeterAddr[6];
    YYMMDDWWhhnnss_TYPE curtime;

    ac_curtime_get(&curtime);
    //SaveTimeManageInit();
    //TimeManageInit();

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0F;

    PrtlData[10] = 0x02;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = curtime.ss;
    PrtlData[23] = curtime.nn;
    PrtlData[24] = curtime.hh;

    CurIndex = 25;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_ac_date_time
输入参数：组帧 交采模块校时
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_ac_date_time(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    uint8   MeterAddr[6];
    YYMMDDWWhhnnss_TYPE curtime;

    ac_curtime_get(&curtime); 

//    SaveTimeManageInit();
//    TimeManageInit();

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x13;

    PrtlData[10] = 0x0C;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = curtime.ss;
    PrtlData[23] = curtime.nn;
    PrtlData[24] = curtime.hh;
    PrtlData[25] = curtime.WW;
    PrtlData[26] = curtime.DD;
    PrtlData[27] = curtime.MM;
    PrtlData[28] = curtime.YY;

    CurIndex = 29;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_master_date
输入参数：组帧 交采模块校时(主站主动下发的日期)
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_master_date(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x10;

    PrtlData[10] = 0x01;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(TermSharedData.DateTimeSet.week);
    PrtlData[23] = Hex2Bcd(TermSharedData.DateTimeSet.day);
    PrtlData[24] = Hex2Bcd(TermSharedData.DateTimeSet.month);
    PrtlData[25] = Hex2Bcd((uint8)(TermSharedData.DateTimeSet.year - 2000));

    CurIndex = 26;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}


/*********************************************************************
功能描述： make_frame_master_time
输入参数：组帧 交采模块校时(主站下发校时时间)
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_master_time(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0F;

    PrtlData[10] = 0x02;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(TermSharedData.DateTimeSet.second);
    PrtlData[23] = Hex2Bcd(TermSharedData.DateTimeSet.minute);
    PrtlData[24] = Hex2Bcd(TermSharedData.DateTimeSet.hour);

    CurIndex = 25;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_zone_table_change_time
输入参数：组帧 时区表切换时间
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_zone_table_change_time(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x11;

    PrtlData[10] = 0x06;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(TermSharedData.AddiZoneChangeTimeSet.minute);
    PrtlData[23] = Hex2Bcd(TermSharedData.AddiZoneChangeTimeSet.hour);
    PrtlData[24] = Hex2Bcd(TermSharedData.AddiZoneChangeTimeSet.day);
    PrtlData[25] = Hex2Bcd(TermSharedData.AddiZoneChangeTimeSet.month);
    PrtlData[26] = Hex2Bcd((uint8)(TermSharedData.AddiZoneChangeTimeSet.year - 2000));

    CurIndex = 27;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_peri_table_change_time
输入参数：组帧 日时段表切换时间
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_peri_table_change_time(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x11;

    PrtlData[10] = 0x07;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(TermSharedData.AddiPeriChangeTimeSet.minute);
    PrtlData[23] = Hex2Bcd(TermSharedData.AddiPeriChangeTimeSet.hour);
    PrtlData[24] = Hex2Bcd(TermSharedData.AddiPeriChangeTimeSet.day);
    PrtlData[25] = Hex2Bcd(TermSharedData.AddiPeriChangeTimeSet.month);
    PrtlData[26] = Hex2Bcd((uint8)(TermSharedData.AddiPeriChangeTimeSet.year - 2000));

    CurIndex = 27;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}


/*********************************************************************
功能描述： make_frame_init_demand
输入参数：组帧 初始化交采需量
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_init_demand(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x19;
    PrtlData[9] = 0x08;

    PrtlData[10] = 0x00;
    PrtlData[11] = 0x56;
    PrtlData[12] = 0x34;
    PrtlData[13] = 0x12;
    PrtlData[14] = 0x99;
    PrtlData[15] = 0x66;
    PrtlData[16] = 0x88;
    PrtlData[17] = 0x00;

    CurIndex = 18;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}


/*********************************************************************
功能描述： make_frame_init_data
输入参数：组帧 初始化交采数据
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_init_data(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x1A;
    PrtlData[9] = 0x08;

    PrtlData[10] = 0x00;
    PrtlData[11] = 0x56;
    PrtlData[12] = 0x34;
    PrtlData[13] = 0x12;
    PrtlData[14] = 0x99;
    PrtlData[15] = 0x66;
    PrtlData[16] = 0x88;
    PrtlData[17] = 0x00;

    CurIndex = 18;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}


/*********************************************************************
功能描述： make_frame_init_param
输入参数：组帧 初始化交采参数
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_init_param(uint8 *PrtlData)
{
    uint8	CheckSum = 0;						//校验和
    uint16	CurIndex;							//索引
    int 	i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x18;
    PrtlData[9] = 0x05;

    PrtlData[10] = 0x05;
    PrtlData[11] = 0x70;
    PrtlData[12] = 0x86;
    PrtlData[13] = 0x61;
    PrtlData[14] = 0x47;

    CurIndex = 15;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_set_demand_period
输入参数：组帧 设置最大需量周期
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_set_demand_period(uint8 *PrtlData)
{
    uint8   CheckSum = 0;                       //校验和
    uint16  CurIndex;                           //索引
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0D;

    PrtlData[10] = 0x03;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(TermSharedData.MaxDemandPeriodSet);

    CurIndex = 23;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_set_slip_time
输入参数：组帧 设置滑差时间
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_set_slip_time(uint8 *PrtlData)
{
    uint8   CheckSum = 0;                       //校验和
    uint16  CurIndex;                           //索引
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0D;

    PrtlData[10] = 0x04;
    PrtlData[11] = 0x01;
    PrtlData[12] = 0x00;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(TermSharedData.SlipDiffTimeSet);

    CurIndex = 23;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_set_zone_peri_num
输入参数：组帧 设置当前套时区/时段数
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_set_zone_peri_num(uint8 *PrtlData, uint32 DI, uint8 Num)
{
    uint8   CheckSum = 0;                       //校验和
    uint16  CurIndex;                           //索引
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0D;

    memcpy(&PrtlData[10], &DI, 4);//数据标识

    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(Num);
    
    CurIndex = 23;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_set_holiday_num
输入参数：组帧 设置公共假日数
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_set_holiday_num(uint8 *PrtlData, uint32 DI, uint8 Num)
{
    uint8   CheckSum = 0;                       //校验和
    uint16  CurIndex;                           //索引
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0E;

    memcpy(&PrtlData[10], &DI, 4);//数据标识

    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Hex2Bcd(Num);
    PrtlData[23] = 0x00;
    
    CurIndex = 24;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}



/*********************************************************************
功能描述： make_frame_set_curr_zone
输入参数：组帧 设置当前套时区表
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_set_curr_zone(uint8 *PrtlData)
{
    uint8   CheckSum = 0;                       //校验和
    uint16  CurIndex;                           //索引
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    //PrtlData[9] = 0x36;

    PrtlData[10] = 0x00;
    PrtlData[11] = 0x00;
    PrtlData[12] = 0x01;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    CurIndex = 22;
    for(i=0; i<TermSharedData.CurrZoneTableSet.nNum; i++)
    {
        if(i == OOP_MAX_TIMEZONE)
            break;
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.CurrZoneTableSet.period[i].meterNum);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.CurrZoneTableSet.period[i].day);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.CurrZoneTableSet.period[i].mon);
    }
    PrtlData[9] = CurIndex - 10;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_set_addi_zone
输入参数：组帧 设置备用套时区表
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_set_addi_zone(uint8 *PrtlData)
{
    uint8   CheckSum = 0;                       //校验和
    uint16  CurIndex;                           //索引
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    //PrtlData[9] = 0x36;

    PrtlData[10] = 0x00;
    PrtlData[11] = 0x00;
    PrtlData[12] = 0x02;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    CurIndex = 22;
    for(i=0; i<TermSharedData.AddiZoneTableSet.nNum; i++)
    {
        if(i == OOP_MAX_TIMEZONE)
            break;
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.AddiZoneTableSet.period[i].meterNum);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.AddiZoneTableSet.period[i].day);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.AddiZoneTableSet.period[i].mon);
    }
    PrtlData[9] = CurIndex - 10;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_set_curr_day_period
输入参数：组帧 设置当前日时段表
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_set_curr_day_period(uint8 *PrtlData, uint8 DayIndex)
{
    uint8   CheckSum = 0;                       //校验和
    uint16  CurIndex;                           //索引
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    //PrtlData[9] = 0x36;

    PrtlData[10] = DayIndex + 1;
    PrtlData[11] = 0x00;
    PrtlData[12] = 0x01;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    CurIndex = 22;
    for(i=0; i<TermSharedData.CurrDayPeriTableSet.dPeriod[DayIndex].nNum; i++)
    {
        if(i == OOP_MAX_DAYPERIOD)
            break;
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.CurrDayPeriTableSet.dPeriod[DayIndex].period[i].rateNO);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.CurrDayPeriTableSet.dPeriod[DayIndex].period[i].min);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.CurrDayPeriTableSet.dPeriod[DayIndex].period[i].hour);
    }
    PrtlData[9] = CurIndex - 10;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_set_addi_day_period
输入参数：组帧 设置备套日时段表
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_set_addi_day_period(uint8 *PrtlData, uint8 DayIndex)
{
    uint8   CheckSum = 0;                       //校验和
    uint16  CurIndex;                           //索引
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    //PrtlData[9] = 0x36;

    PrtlData[10] = DayIndex + 1;
    PrtlData[11] = 0x00;
    PrtlData[12] = 0x02;
    PrtlData[13] = 0x04;
    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    CurIndex = 22;
    for(i=0; i<TermSharedData.AddiDayPeriTableSet.dPeriod[DayIndex].nNum; i++)
    {
        if(i == OOP_MAX_DAYPERIOD)
            break;
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.AddiDayPeriTableSet.dPeriod[DayIndex].period[i].rateNO);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.AddiDayPeriTableSet.dPeriod[DayIndex].period[i].min);
        PrtlData[CurIndex++] = Hex2Bcd(TermSharedData.AddiDayPeriTableSet.dPeriod[DayIndex].period[i].hour);
    }
    PrtlData[9] = CurIndex - 10;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_set_p_q_word
输入参数：组帧 设置有功/无功状态字
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_set_p_q_word(uint8 *PrtlData, uint32 DI, uint8 Num)
{
    uint8   CheckSum = 0;                       //校验和
    uint16  CurIndex;                           //索引
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x14;
    PrtlData[9] = 0x0D;

    memcpy(&PrtlData[10], &DI, 4);//数据标识

    PrtlData[14] = 0x00;
    PrtlData[15] = 0x56;
    PrtlData[16] = 0x34;
    PrtlData[17] = 0x12;
    PrtlData[18] = 0x99;
    PrtlData[19] = 0x66;
    PrtlData[20] = 0x88;
    PrtlData[21] = 0x00;
    PrtlData[22] = Num;//状态字不需要转bcd码
    
    CurIndex = 23;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_query_para
输入参数：组帧 查询交采参数
输出参数：
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_query_para(uint8 *PrtlData, uint32 DataIdentify)
{
    uint8   CheckSum = 0;                       //校验和
    uint16  CurIndex;                           //索引
    int     i;
    uint8   MeterAddr[6];

    memset(MeterAddr, 0xAA, 6);

    PrtlData[0] = 0x68;
    memcpy(&PrtlData[1], MeterAddr, 6);
    PrtlData[7] = 0x68;

    PrtlData[8] = 0x11;
    PrtlData[9] = 0x04;

    memcpy(&PrtlData[10], &DataIdentify, 4);//数据标识

    CurIndex = 14;

    for(i=10; i<CurIndex; i++)
    {
        PrtlData[i] += 0x33;
    }

    for(i = 0; i < CurIndex; i++)
    {
        CheckSum += PrtlData[i];
    }
    PrtlData[CurIndex] = CheckSum;
    PrtlData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*********************************************************************
功能描述： make_frame_query_para
输入参数：组帧 查询交采参数
            uint8 nInLen DI之后长度
            uint8 *inData DI之后校验码之前整个报文数据域内容
输出参数：  uint8 *outData  完整的确认报文
函数返回值：send_len
内部处理流程：
********************************************************************/
int32 make_frame_confirm(uint8 *inData, uint8 nInLen, uint32	DataIdentify, uint8 *outData)
{
    uint8   CheckSum = 0;                       //校验和
    uint16  CurIndex;                           //索引
    int     i;
    uint8   MeterAddr[6];
    

    memset(MeterAddr, 0xAA, 6);

    outData[0] = 0x68;
    memcpy(&outData[1], MeterAddr, 6);
    outData[7] = 0x68;

    outData[8] = 0x91;
    outData[9] = 4 + nInLen;

    memcpy(&outData[10], &DataIdentify, 4);//数据标识
    memcpy(&outData[14], inData, nInLen);//报文内容

    CurIndex = 14 + nInLen;

    for(i=10; i<CurIndex; i++)
    {
        outData[i] += 0x33;
    }

    for(i=0; i<CurIndex; i++)
    {
        CheckSum += outData[i];
    }
    outData[CurIndex] = CheckSum;
    outData[CurIndex + 1] = 0x16;

    CurIndex += 2;

    return CurIndex;
}

/*******************************************************************************
* 函数名称: ac_demand_initial
* 函数功能: 交采需量初始化
* 输入参数:
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void ac_demand_initial(MSG_INFO_T *ptMsgInfo)
{
    uint16 nPayloadLen = 0;
    uint8  aPayloadBuff[100] = {0};

    //置交采初始化标志
    g_nAcDemandInitFlag = 1;

    //组装肯定应答
    nPayloadLen = 1;
    aPayloadBuff[0] = 0x01;
    make_resp_info(ptMsgInfo, nPayloadLen, aPayloadBuff);
}

/*******************************************************************************
* 函数名称: ac_system_deal
* 函数功能: 执行系统指令
* 输入参数:
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void ac_system_deal(MSG_INFO_T *ptMsgInfo)
{
    uint16 nPayloadLen = 0;
    uint8  aPayloadBuff[100] = {0};
    uint16 offset = 0;//偏移位
    uint8  nCount = 0;//指令个数
    char  aCharArr[500] = {0};//指令内容
    uint16  nCharLen = 0;//指令内容长度
    int32 result = 0;
    int i = 0;

    //指令个数
    nCount = ptMsgInfo->MsgData[offset];
    offset++;
    ACMQTT_FMT_DEBUG("ac_system_deal() 指令数量nCount=%d, offset=%d \n", nCount, offset);

    //组装应答(seq of)
    aPayloadBuff[nPayloadLen] = nCount;
    nPayloadLen += 1;

    //解析指令内容
    for(i=0; i<nCount; i++)
    {
        nCharLen = 0;
        BuffToChangeLen(&ptMsgInfo->MsgData[offset], &offset, &nCharLen);
        memset(aCharArr, 0x00, sizeof(aCharArr));
        strncpy(aCharArr,(char *)&ptMsgInfo->MsgData[offset], nCharLen > sizeof(aCharArr) ? sizeof(aCharArr) : nCharLen);
        result = system(aCharArr);
        offset += nCharLen;
        ACMQTT_FMT_DEBUG("ac_system_deal() i=%d, nCharLen=%d, aCharArr=%s, offset=%d result=%d\n", 
            i, nCharLen, aCharArr, offset, result);

        //组装应答
        memcpy_r(&aPayloadBuff[nPayloadLen], &result, 4);
        nPayloadLen += 4;	
    }

    //组装应答
    make_resp_info(ptMsgInfo, nPayloadLen, aPayloadBuff);
}


/*******************************************************************************
* 函数名称: ac_data_initial
* 函数功能: 交采数据初始化
* 输入参数:
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void ac_data_initial(MSG_INFO_T *ptMsgInfo)
{
    uint16 nPayloadLen = 0;
    uint8  aPayloadBuff[100] = {0};

    //置交采初始化标志
    g_nAcDataInitFlag = 1;

    //组装肯定应答
    nPayloadLen = 1;
    aPayloadBuff[0] = 0x01;
    make_resp_info(ptMsgInfo, nPayloadLen, aPayloadBuff);
}

/*******************************************************************************
* 函数名称: ac_param_initial
* 函数功能: 交采参数初始化
* 输入参数:
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void ac_param_initial(MSG_INFO_T *ptMsgInfo)
{
    uint16 nPayloadLen = 0;
    uint8  aPayloadBuff[100] = {0};

    //置交采初始化标志
    g_nAcParamInitFlag = 1;

    //组装肯定应答
    nPayloadLen = 1;
    aPayloadBuff[0] = 0x01;
    make_resp_info(ptMsgInfo, nPayloadLen, aPayloadBuff);
}

/*******************************************************************************
* 函数名称: check_data_initial_result
* 函数功能: 检查交采数据初始化结果
* 输入参数:
* 输出参数: 无
* 返 回 值: 1:成功; -1:失败
*******************************************************************************/
int check_data_initial_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* 函数名称: check_param_initial_result
* 函数功能: 检查交采数据初始化结果
* 输入参数:
* 输出参数: 无
* 返 回 值: 1:成功; -1:失败
*******************************************************************************/
int check_param_initial_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* 函数名称: check_demand_initial_result
* 函数功能: 检查需量数据初始化结果
* 输入参数:
* 输出参数: 无
* 返 回 值: 1:成功; -1:失败
*******************************************************************************/
int check_demand_initial_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* 函数名称: check_timing_result
* 函数功能: 检查校时结果
* 输入参数:
* 输出参数: 无
* 返 回 值: 1:成功; -1:失败
*******************************************************************************/
int check_timing_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* 函数名称: check_set_demand_period_result
* 函数功能: 检查设置最大需量周期结果
* 输入参数:
* 输出参数: 无
* 返 回 值: 1:成功; -1:失败
*******************************************************************************/
int check_set_demand_period_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* 函数名称: check_set_slip_time_result
* 函数功能: 检查设置滑差时间结果
* 输入参数:
* 输出参数: 无
* 返 回 值: 1:成功; -1:失败
*******************************************************************************/
int check_set_slip_time_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* 函数名称: check_set_zone_result
* 函数功能: 检查时区结果
* 输入参数:
* 输出参数: 无
* 返 回 值: 1:成功; -1:失败
*******************************************************************************/
int check_set_zone_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* 函数名称: check_set_day_period_result
* 函数功能: 检查设置日时段结果
* 输入参数:
* 输出参数: 无
* 返 回 值: 1:成功; -1:失败
*******************************************************************************/
int check_set_day_period_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}

/*******************************************************************************
* 函数名称: check_set_para_result
* 函数功能: 检查设置参数结果
* 输入参数:
* 输出参数: 无
* 返 回 值: 1:成功; -1:失败
*******************************************************************************/
int check_set_para_result(uint8 *buff, int32 length)
{
    int ret = -1;
    int i = 0;
    int startIndex = 0;

    for(i=0; i<length; i++)
    {
        if(buff[i] == 0x68)
        {
            startIndex = i;
            break;
        }
    }

    if(startIndex+9 < length)
    {
        if(0x00 == buff[startIndex+9])
        {
            ret = 1;
        }
    }

    return ret;
}


/*******************************************************************************
* 函数名称: SaveDemand07
* 函数功能: 存储最大需量及发生时间数据
* 输入参数:
*           DataLen             数据长度
*           DataBuff            数据内容
*           EnergySort          能量类别
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveDemand07(uint8 DataLen, uint8 *DataBuff, ENERGY_SORT_T EnergySort)
{
    uint8	SrcData[10] = {0};
    uint8	TempBuff[10] = {0};
    uint8	i = 0, index = 0;
    uint8   X;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    switch (EnergySort)
    {
    case ES_POS_ACT:						//正向有功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_ACT:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.CurrDemand[0][i].DemNum, 0xff, 3);
                memset(TermSharedData.CurrDemandTime[0][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.CurrDemand[0][i].DemNum, SrcData, 3);
                TermSharedData.CurrDemand[0][i].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.CurrDemandTime[0][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.CurrDemandTime[0][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemand[0][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemand[0][i].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemandTime[0][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemandTime[0][i].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_NEG_ACT:						//反向有功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_ACT:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.CurrDemand[1][i].DemNum, 0xff, 3);
                memset(TermSharedData.CurrDemandTime[1][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.CurrDemand[1][i].DemNum, SrcData, 3);
                TermSharedData.CurrDemand[1][i].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.CurrDemandTime[1][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.CurrDemandTime[1][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemand[1][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemand[1][i].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemandTime[1][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemandTime[1][i].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_POS_REA:						//正向无功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_REA:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.CurrDemand[2][i].DemNum, 0xff, 3);
                memset(TermSharedData.CurrDemandTime[2][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.CurrDemand[2][i].DemNum, SrcData, 3);
                TermSharedData.CurrDemand[2][i].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.CurrDemandTime[2][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.CurrDemandTime[2][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemand[2][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemand[2][i].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemandTime[2][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemandTime[2][i].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_NEG_REA:						//反向无功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_REA:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.CurrDemand[3][i].DemNum, 0xff, 3);
                memset(TermSharedData.CurrDemandTime[3][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.CurrDemand[3][i].DemNum, SrcData, 3);
                TermSharedData.CurrDemand[3][i].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.CurrDemandTime[3][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.CurrDemandTime[3][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemand[3][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemand[3][i].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.CurrDemandTime[3][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.CurrDemandTime[3][i].DemTime, 6, "内容:");

            index += X;
        }

        break;




    case ES_1_REA:						//1象限无功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_1_REA:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.QuaDemand[0][i].DemNum, 0xff, 3);
                memset(TermSharedData.QuaDemandTime[0][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.QuaDemand[0][i].DemNum, SrcData, 3);
                TermSharedData.QuaDemand[0][i].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.QuaDemandTime[0][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.QuaDemandTime[0][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemand[0][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemand[0][i].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemandTime[0][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemandTime[0][i].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_2_REA:						//2象限无功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_2_REA:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.QuaDemand[1][i].DemNum, 0xff, 3);
                memset(TermSharedData.QuaDemandTime[1][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.QuaDemand[1][i].DemNum, SrcData, 3);
                TermSharedData.QuaDemand[1][i].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.QuaDemandTime[1][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.QuaDemandTime[1][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemand[1][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemand[1][i].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemandTime[1][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemandTime[1][i].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_3_REA:						//3象限无功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_3_REA:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.QuaDemand[2][i].DemNum, 0xff, 3);
                memset(TermSharedData.QuaDemandTime[2][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.QuaDemand[2][i].DemNum, SrcData, 3);
                TermSharedData.QuaDemand[2][i].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.QuaDemandTime[2][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.QuaDemandTime[2][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemand[2][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemand[2][i].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemandTime[2][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemandTime[2][i].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_4_REA:						//4象限无功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_4_REA:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.QuaDemand[3][i].DemNum, 0xff, 3);
                memset(TermSharedData.QuaDemandTime[3][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.QuaDemand[3][i].DemNum, SrcData, 3);
                TermSharedData.QuaDemand[3][i].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.QuaDemandTime[3][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.QuaDemandTime[3][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemand[3][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemand[3][i].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemandTime[3][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemandTime[3][i].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_POS_APP:						//正向视在电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_APP:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.AppaDemand[0][i].DemNum, 0xff, 3);
                memset(TermSharedData.QuaDemandTime[0][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.AppaDemand[0][i].DemNum, SrcData, 3);
                TermSharedData.AppaDemand[0][i].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.QuaDemandTime[0][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.QuaDemandTime[0][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.AppaDemand[0][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.AppaDemand[0][i].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemandTime[0][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemandTime[0][i].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_NEG_APP:						//反向视在电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_APP:...... \n");
        X = DataLen/5;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.AppaDemand[1][i].DemNum, 0xff, 3);
                memset(TermSharedData.QuaDemandTime[1][i].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.AppaDemand[1][i].DemNum, SrcData, 3);
                TermSharedData.AppaDemand[1][i].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.QuaDemandTime[1][i].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.QuaDemandTime[1][i].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.AppaDemand[1][%d].DemNum:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.AppaDemand[1][i].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.QuaDemandTime[1][%d].DemTime:  \n", i);
            ACCOMM_BUF_LOGCC(TermSharedData.QuaDemandTime[1][i].DemTime, 6, "内容:");

            index += X;
        }

        break;



    case ES_POS_ACT_A:						//A相正向有功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_ACT_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.PosActDemand[0].DemNum, 0xff, 3);
                memset(TermSharedData.PosActDemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.PosActDemand[0].DemNum, SrcData, 3);
                TermSharedData.PosActDemand[0].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.PosActDemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.PosActDemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosActDemand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosActDemand[0].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.PosActDemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosActDemandTime[0].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_POS_ACT_B:						//B相正向有功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_ACT_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.PosActDemand[1].DemNum, 0xff, 3);
                memset(TermSharedData.PosActDemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.PosActDemand[1].DemNum, SrcData, 3);
                TermSharedData.PosActDemand[1].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.PosActDemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.PosActDemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosActDemand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosActDemand[1].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.PosActDemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosActDemandTime[1].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_POS_ACT_C:						//C相正向有功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_ACT_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.PosActDemand[2].DemNum, 0xff, 3);
                memset(TermSharedData.PosActDemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.PosActDemand[2].DemNum, SrcData, 3);
                TermSharedData.PosActDemand[2].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.PosActDemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.PosActDemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosActDemand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosActDemand[2].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.PosActDemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosActDemandTime[2].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_NEG_ACT_A:						//A相反向有功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_ACT_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegActDemand[0].DemNum, 0xff, 3);
                memset(TermSharedData.NegActDemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegActDemand[0].DemNum, SrcData, 3);
                TermSharedData.NegActDemand[0].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.NegActDemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegActDemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegActDemand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegActDemand[0].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegActDemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegActDemandTime[0].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_NEG_ACT_B:						//B相反向有功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_ACT_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegActDemand[1].DemNum, 0xff, 3);
                memset(TermSharedData.NegActDemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegActDemand[1].DemNum, SrcData, 3);
                TermSharedData.NegActDemand[1].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.NegActDemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegActDemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegActDemand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegActDemand[1].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegActDemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegActDemandTime[1].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_NEG_ACT_C:						//C相反向有功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_ACT_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegActDemand[2].DemNum, 0xff, 3);
                memset(TermSharedData.NegActDemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegActDemand[2].DemNum, SrcData, 3);
                TermSharedData.NegActDemand[2].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.NegActDemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegActDemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegActDemand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegActDemand[2].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegActDemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegActDemandTime[2].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_POS_REA_A:						//A相正向无功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_REA_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.PosReaDemand[0].DemNum, 0xff, 3);
                memset(TermSharedData.PosReaDemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.PosReaDemand[0].DemNum, SrcData, 3);
                TermSharedData.PosReaDemand[0].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.PosReaDemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.PosReaDemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosReaDemand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosReaDemand[0].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.PosReaDemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosReaDemandTime[0].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_POS_REA_B:						//B相正向无功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_REA_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.PosReaDemand[1].DemNum, 0xff, 3);
                memset(TermSharedData.PosReaDemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.PosReaDemand[1].DemNum, SrcData, 3);
                TermSharedData.PosReaDemand[1].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.PosReaDemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.PosReaDemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosReaDemand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosReaDemand[1].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.PosReaDemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosReaDemandTime[1].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_POS_REA_C:						//C相正向无功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_REA_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.PosReaDemand[2].DemNum, 0xff, 3);
                memset(TermSharedData.PosReaDemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.PosReaDemand[2].DemNum, SrcData, 3);
                TermSharedData.PosReaDemand[2].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.PosReaDemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.PosReaDemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosReaDemand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosReaDemand[2].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.PosReaDemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.PosReaDemandTime[2].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_NEG_REA_A:						//A相反向无功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_REA_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegReaDemand[0].DemNum, 0xff, 3);
                memset(TermSharedData.NegReaDemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegReaDemand[0].DemNum, SrcData, 3);
                TermSharedData.NegReaDemand[0].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.NegReaDemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegReaDemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegReaDemand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegReaDemand[0].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegReaDemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegReaDemandTime[0].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_NEG_REA_B:						//B相反向无功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_REA_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegReaDemand[1].DemNum, 0xff, 3);
                memset(TermSharedData.NegReaDemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegReaDemand[1].DemNum, SrcData, 3);
                TermSharedData.NegReaDemand[1].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.NegReaDemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegReaDemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegReaDemand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegReaDemand[1].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegReaDemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegReaDemandTime[1].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_NEG_REA_C:						//C相反向无功
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_REA_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegReaDemand[2].DemNum, 0xff, 3);
                memset(TermSharedData.NegReaDemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegReaDemand[2].DemNum, SrcData, 3);
                TermSharedData.NegReaDemand[2].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.NegReaDemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegReaDemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegReaDemand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegReaDemand[2].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegReaDemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegReaDemandTime[2].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_1_REA_A:						//A相第一象限无功电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_1_REA_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua1Demand[0].DemNum, 0xff, 3);
                memset(TermSharedData.Qua1DemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua1Demand[0].DemNum, SrcData, 3);
                TermSharedData.Qua1Demand[0].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.Qua1DemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua1DemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua1Demand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua1Demand[0].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua1DemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua1DemandTime[0].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_1_REA_B:						//B相第一象限无功电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_1_REA_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua1Demand[1].DemNum, 0xff, 3);
                memset(TermSharedData.Qua1DemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua1Demand[1].DemNum, SrcData, 3);
                TermSharedData.Qua1Demand[1].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.Qua1DemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua1DemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua1Demand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua1Demand[1].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua1DemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua1DemandTime[1].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_1_REA_C:						//C相第一象限无功电能
        ACCOMM_FMT_LOGCC("SaveDemand07()ES_1_REA_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua1Demand[2].DemNum, 0xff, 3);
                memset(TermSharedData.Qua1DemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua1Demand[2].DemNum, SrcData, 3);
                TermSharedData.Qua1Demand[2].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.Qua1DemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua1DemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua1Demand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua1Demand[2].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua1DemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua1DemandTime[2].DemTime, 6, "内容:");

            index += X;
        }

        break;

        case ES_2_REA_A:						//A相第二象限无功电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_2_REA_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua2Demand[0].DemNum, 0xff, 3);
                memset(TermSharedData.Qua2DemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua2Demand[0].DemNum, SrcData, 3);
                TermSharedData.Qua2Demand[0].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.Qua2DemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua2DemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua2Demand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua2Demand[0].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua2DemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua2DemandTime[0].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_2_REA_B:						//B相第二象限无功电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_2_REA_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua2Demand[1].DemNum, 0xff, 3);
                memset(TermSharedData.Qua2DemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua2Demand[1].DemNum, SrcData, 3);
                TermSharedData.Qua2Demand[1].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.Qua2DemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua2DemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua2Demand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua2Demand[1].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua2DemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua2DemandTime[1].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_2_REA_C:						//C相第二象限无功电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_2_REA_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua2Demand[2].DemNum, 0xff, 3);
                memset(TermSharedData.Qua2DemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua2Demand[2].DemNum, SrcData, 3);
                TermSharedData.Qua2Demand[2].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.Qua2DemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua2DemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua2Demand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua2Demand[2].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua2DemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua2DemandTime[2].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_3_REA_A:						//A相第三象限无功电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_3_REA_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua3Demand[0].DemNum, 0xff, 3);
                memset(TermSharedData.Qua3DemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua3Demand[0].DemNum, SrcData, 3);
                TermSharedData.Qua3Demand[0].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.Qua3DemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua3DemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua3Demand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua3Demand[0].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua3DemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua3DemandTime[0].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_3_REA_B:						//B相第三象限无功电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_3_REA_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua3Demand[1].DemNum, 0xff, 3);
                memset(TermSharedData.Qua3DemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua3Demand[1].DemNum, SrcData, 3);
                TermSharedData.Qua3Demand[1].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.Qua3DemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua3DemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua3Demand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua3Demand[1].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua3DemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua3DemandTime[1].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_3_REA_C:						//C相第三象限无功电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_3_REA_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua3Demand[2].DemNum, 0xff, 3);
                memset(TermSharedData.Qua3DemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua3Demand[2].DemNum, SrcData, 3);
                TermSharedData.Qua3Demand[2].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.Qua3DemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua3DemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua3Demand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua3Demand[2].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua3DemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua3DemandTime[2].DemTime, 6, "内容:");

            index += X;
        }

        break;

        case ES_4_REA_A:						//A相第四象限无功电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_4_REA_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua4Demand[0].DemNum, 0xff, 3);
                memset(TermSharedData.Qua4DemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua4Demand[0].DemNum, SrcData, 3);
                TermSharedData.Qua4Demand[0].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.Qua4DemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua4DemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua4Demand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua4Demand[0].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua4DemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua4DemandTime[0].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_4_REA_B:						//B相第四象限无功电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_4_REA_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua4Demand[1].DemNum, 0xff, 3);
                memset(TermSharedData.Qua4DemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua4Demand[1].DemNum, SrcData, 3);
                TermSharedData.Qua4Demand[1].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.Qua4DemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua4DemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua4Demand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua4Demand[1].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua4DemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua4DemandTime[1].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_4_REA_C:						//C相第四象限无功电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_4_REA_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.Qua4Demand[2].DemNum, 0xff, 3);
                memset(TermSharedData.Qua4DemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.Qua4Demand[2].DemNum, SrcData, 3);
                TermSharedData.Qua4Demand[2].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.Qua4DemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.Qua4DemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.Qua4Demand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua4Demand[2].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.Qua4DemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.Qua4DemandTime[2].DemTime, 6, "内容:");

            index += X;
        }

        break;

        case ES_POS_APP_A:						//A相正向视在电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_APP_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.ActAppDemand[0].DemNum, 0xff, 3);
                memset(TermSharedData.ActAppDemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.ActAppDemand[0].DemNum, SrcData, 3);
                TermSharedData.ActAppDemand[0].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.ActAppDemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.ActAppDemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.ActAppDemand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.ActAppDemand[0].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.ActAppDemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.ActAppDemandTime[0].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_POS_APP_B:						//B相正向视在电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_APP_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.ActAppDemand[1].DemNum, 0xff, 3);
                memset(TermSharedData.ActAppDemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.ActAppDemand[1].DemNum, SrcData, 3);
                TermSharedData.ActAppDemand[1].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.ActAppDemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.ActAppDemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.ActAppDemand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.ActAppDemand[1].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.ActAppDemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.ActAppDemandTime[1].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_POS_APP_C:						//C相正向视在电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_POS_APP_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.ActAppDemand[2].DemNum, 0xff, 3);
                memset(TermSharedData.ActAppDemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.ActAppDemand[2].DemNum, SrcData, 3);
                TermSharedData.ActAppDemand[2].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.ActAppDemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.ActAppDemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.ActAppDemand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.ActAppDemand[2].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.ActAppDemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.ActAppDemandTime[2].DemTime, 6, "内容:");

            index += X;
        }

        break;

        case ES_NEG_APP_A:						//A相反向视在电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_APP_A:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegAppDemand[0].DemNum, 0xff, 3);
                memset(TermSharedData.NegAppDemandTime[0].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegAppDemand[0].DemNum, SrcData, 3);
                TermSharedData.NegAppDemand[0].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.NegAppDemandTime[0].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegAppDemandTime[0].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegAppDemand[0].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegAppDemand[0].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegAppDemandTime[0].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegAppDemandTime[0].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_NEG_APP_B:						//B相反向视在电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_APP_B:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegAppDemand[1].DemNum, 0xff, 3);
                memset(TermSharedData.NegAppDemandTime[1].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegAppDemand[1].DemNum, SrcData, 3);
                TermSharedData.NegAppDemand[1].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.NegAppDemandTime[1].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegAppDemandTime[1].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegAppDemand[1].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegAppDemand[1].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegAppDemandTime[1].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegAppDemandTime[1].DemTime, 6, "内容:");

            index += X;
        }

        break;

    case ES_NEG_APP_C:						//C相反向视在电能
        ACCOMM_FMT_LOGCC("SaveDemand07() ES_NEG_APP_C:...... \n");
        X = DataLen;
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, 3) == 0)
            {
                memset(TermSharedData.NegAppDemand[2].DemNum, 0xff, 3);
                memset(TermSharedData.NegAppDemandTime[2].DemTime, 0xff, 6);
            }
            else
            {
                memcpy(TermSharedData.NegAppDemand[2].DemNum, SrcData, 3);
                TermSharedData.NegAppDemand[2].DemNum[2] &= 0x7f;                // 去掉组合无功的符号位
                memcpy(&TermSharedData.NegAppDemandTime[2].DemTime[1], &SrcData[3+X-8], 5);
                TermSharedData.NegAppDemandTime[2].DemTime[0] = 0;
            }

            ACCOMM_FMT_LOGCC("TermSharedData.NegAppDemand[2].DemNum:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegAppDemand[2].DemNum, 3, "内容:");
            ACCOMM_FMT_LOGCC("TermSharedData.NegAppDemandTime[2].DemTime:  \n");
            ACCOMM_BUF_LOGCC(TermSharedData.NegAppDemandTime[2].DemTime, 6, "内容:");

            index += X;
        }

        break;
        
    default:
        return;
    }
}


/*******************************************************************************
* 函数名称: SaveER07
* 函数功能: 存储电能示数
* 输入参数: Pn                  测量点号
*           DataLen             数据长度
*           DataBuff            数据内容
*           EnergySort          能量类别
*           EnergyType          能量数据类型 0:非高精度 两位小数; 1 高精度 四位小数
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveER07(uint8 DataLen, uint8 *DataBuff, ENERGY_SORT_T EnergySort, uint8 EnergyType)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //符号标志(0:正 / 1:负)
    uint8   X, Y;                               //BCD码字节数、BIN码字节数
    uint8   Len = 0;
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    if(1 == EnergyType)
    {
        X = 5;
        Y = 8;
    }
    else
    {
        X = 4;
        Y = 4;
    }

    ACCOMM_FMT_LOGCC("SaveER07() BCD码字节数X=%d, BIN码字节数Y=%d, DataLen=%u...... \n", X, Y, DataLen);
    
    //单个费率数据长度5/4
    switch (EnergySort)
    {
    case ES_POS_ACT:                        //正向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.PosActEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PosActEng[%d]:%d \n", i, TermSharedData.PosActEng[i].ULinteger);

            index += Len;
        }

        break;

    case ES_NEG_ACT:                        //反向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.NegActEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.NegActEng[%d]:%d \n", i, TermSharedData.NegActEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_POS_REA:                        //正向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                if((SrcData[X - 1] & 0x80) != 0)   //负
                {
                    SrcData[X - 1] &= 0x7F;
                    SignFlag = 1;
                }
                else
                {
                    SignFlag = 0;
                }
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.PosReaEnergyType[i] = 1;
            }
            else
            {
                gPriMeasurePara.PosReaEnergyType[i] = 0;
            }
            
            memcpy((uint8*)&TermSharedData.PosReaEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PosReaEng[%d]:%d \n", i, TermSharedData.PosReaEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_NEG_REA:                        //反向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                if((SrcData[X - 1] & 0x80) != 0)   //负
                {
                    SrcData[X - 1] &= 0x7F;
                    SignFlag = 1;
                }
                else
                {
                    SignFlag = 0;
                }
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.NegReaEnergyType[i] = 1;
            }
            else
            {
                gPriMeasurePara.NegReaEnergyType[i] = 0;
            }
            
            memcpy((uint8*)&TermSharedData.NegReaEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.NegReaEng[%d]:%d \n", i, TermSharedData.NegReaEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_1_REA:                          //1象限无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad1ReaEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad1ReaEng[%d]:%d \n", i, TermSharedData.Quad1ReaEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_2_REA:                          //2象限无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad2ReaEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad2ReaEng[%d]:%d \n", i, TermSharedData.Quad2ReaEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_3_REA:                          //3象限无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad3ReaEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad3ReaEng[%d]:%d \n", i, TermSharedData.Quad3ReaEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_4_REA:                          //4象限无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad4ReaEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad4ReaEng[%d]:%d \n", i, TermSharedData.Quad4ReaEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_POS_ACT_A:                      //A相正向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaAPosActEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAPosActEng:%d \n", TermSharedData.PhaAPosActEng[0].ULinteger);
        break;

    case ES_NEG_ACT_A:                      //A相反向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaANegActEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaANegActEng:%d \n", TermSharedData.PhaANegActEng[0].ULinteger);
        break;

    case ES_POS_REA_A:                      //A相正向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //负
        {
            gPriMeasurePara.PhaAPosReaEngType[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaAPosReaEngType[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaAPosReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAPosReaEng:%d \n", TermSharedData.PhaAPosReaEng[0].ULinteger);
        break;

    case ES_NEG_REA_A:                      //A相反向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //负
        {
            gPriMeasurePara.PhaANegReaEngType[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaANegReaEngType[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaANegReaEng, TempBuff, Y);
        
        ACCOMM_FMT_LOGCC("TermSharedData.PhaANegReaEng:%d \n", TermSharedData.PhaANegReaEng[0].ULinteger);
        break;

    case ES_POS_ACT_B:                      //B相正向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBPosActEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBPosActEng:%d \n", TermSharedData.PhaBPosActEng[0].ULinteger);
        break;

    case ES_NEG_ACT_B:                      //B相反向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBNegActEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBNegActEng:%d \n", TermSharedData.PhaBNegActEng[0].ULinteger);
        break;

    case ES_POS_REA_B:                      //B相正向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //负
        {
            gPriMeasurePara.PhaBPosReaEngType[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaBPosReaEngType[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaBPosReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBPosReaEng:%d \n", TermSharedData.PhaBPosReaEng[0].ULinteger);
        break;

    case ES_NEG_REA_B:                      //B相反向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //负
        {
            gPriMeasurePara.PhaBNegReaEngType[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaBNegReaEngType[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaBNegReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBNegReaEng:%d \n", TermSharedData.PhaBNegReaEng[0].ULinteger);
        break;

    case ES_POS_ACT_C:                      //C相正向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCPosActEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCPosActEng:%d \n", TermSharedData.PhaCPosActEng[0].ULinteger);
        break;

    case ES_NEG_ACT_C:                      //C相反向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCNegActEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCNegActEng:%d \n", TermSharedData.PhaCNegActEng[0].ULinteger);
        break;

    case ES_POS_REA_C:                      //C相正向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //负
        {
            gPriMeasurePara.PhaCPosReaEngType[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaCPosReaEngType[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaCPosReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCPosReaEng:%d \n", TermSharedData.PhaCPosReaEng[0].ULinteger);
        break;

    case ES_NEG_REA_C:                      //C相反向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //负
        {
            gPriMeasurePara.PhaCNegReaEngType[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaCNegReaEngType[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaCNegReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCNegReaEng:%d \n", TermSharedData.PhaCNegReaEng[0].ULinteger);
        break;

    case ES_CONBIN_ACT:						//(当前)组合有功电能块
        ACCOMM_FMT_LOGCC("SaveER07() ES_CONBIN_ACT:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                if((SrcData[X - 1] & 0x80) != 0)   //负
                {
                    SrcData[X - 1] &= 0x7F;
                    SignFlag = 1;
                }
                else
                {
                    SignFlag = 0;
                }
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.CmbEnergyType[i] = 1;
            }
            else
            {
                gPriMeasurePara.CmbEnergyType[i] = 0;
            }
            
            memcpy((uint8*)&TermSharedData.CmbActEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.CmbActEng[%d]:%d \n", i, TermSharedData.CmbActEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_POS_APP:						//(当前)正向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.PosAppEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PosAppEng[%d]:%d \n", i, TermSharedData.PosAppEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_NEG_APP:						//(当前反向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.NegAppEng[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.NegAppEng[%d]:%d \n", i, TermSharedData.NegAppEng[i].ULinteger);

            index += Len;
        }
        break;

    case ES_1_REA_A:                      //A相第一象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA1ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA1ReaEng:%d \n", TermSharedData.PhaA1ReaEng[0].ULinteger);
        break;

    case ES_1_REA_B:                      //B相第一象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB1ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB1ReaEng:%d \n", TermSharedData.PhaB1ReaEng[0].ULinteger);
        break;

    case ES_1_REA_C:                      //C相第一象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC1ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC1ReaEng:%d \n", TermSharedData.PhaC1ReaEng[0].ULinteger);
        break;

    case ES_2_REA_A:                      //A相第二象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA2ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA2ReaEng:%d \n", TermSharedData.PhaA2ReaEng[0].ULinteger);
        break;

    case ES_2_REA_B:                      //B相第二象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB2ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB2ReaEng:%d \n", TermSharedData.PhaB2ReaEng[0].ULinteger);
        break;

    case ES_2_REA_C:                      //C相第二象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC2ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC2ReaEng:%d \n", TermSharedData.PhaC2ReaEng[0].ULinteger);
        break;

    case ES_3_REA_A:                      //A相第三象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA3ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA3ReaEng:%d \n", TermSharedData.PhaA3ReaEng[0].ULinteger);
        break;

    case ES_3_REA_B:                      //B相第三象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB3ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB3ReaEng:%d \n", TermSharedData.PhaB3ReaEng[0].ULinteger);
        break;

    case ES_3_REA_C:                      //C相第三象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC3ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC3ReaEng:%d \n", TermSharedData.PhaC3ReaEng[0].ULinteger);
        break;

    case ES_4_REA_A:                      //A相第四象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA4ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA4ReaEng:%d \n", TermSharedData.PhaA4ReaEng[0].ULinteger);
        break;

    case ES_4_REA_B:                      //B相第四象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB4ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB4ReaEng:%d \n", TermSharedData.PhaB4ReaEng[0].ULinteger);
        break;

    case ES_4_REA_C:                      //C相第四象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC4ReaEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC4ReaEng:%d \n", TermSharedData.PhaC4ReaEng[0].ULinteger);
        break;

    case ES_POS_APP_A:                      //A相正向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaAActAppEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAActAppEng:%d \n", TermSharedData.PhaAActAppEng[0].ULinteger);
        break;

    case ES_POS_APP_B:                      //B相正向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBActAppEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBActAppEng:%d \n", TermSharedData.PhaBActAppEng[0].ULinteger);
        break;

    case ES_POS_APP_C:                      //C相正向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCActAppEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCActAppEng:%d \n", TermSharedData.PhaCActAppEng[0].ULinteger);
        break;

    case ES_NEG_APP_A:                      //A相反向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_A:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaANegAppEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaANegAppEng:%d \n", TermSharedData.PhaANegAppEng[0].ULinteger);
        break;

    case ES_NEG_APP_B:                      //B相反向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_B:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBNegAppEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBNegAppEng:%d \n", TermSharedData.PhaBNegAppEng[0].ULinteger);
        break;

    case ES_NEG_APP_C:                      //C相反向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_C:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCNegAppEng, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCNegAppEng:%d \n", TermSharedData.PhaCNegAppEng[0].ULinteger);
        break;

    /***高精度 电能量 begin***/
    
    case ES_POS_ACT_HD:                        //正向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.PosActEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PosActEngHD[%d]:%d \n", i, TermSharedData.PosActEngHD[i].ULinteger);

            index += Len;
        }

        break;

    case ES_NEG_ACT_HD:                        //反向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.NegActEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.NegActEngHD[%d]:%d \n", i, TermSharedData.NegActEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_POS_REA_HD:                        //正向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                if((SrcData[X - 1] & 0x80) != 0)   //负
                {
                    SrcData[X - 1] &= 0x7F;
                    SignFlag = 1;
                }
                else
                {
                    SignFlag = 0;
                }
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.PosReaEnergyTypeHD[i] = 1;
            }
            else
            {
                gPriMeasurePara.PosReaEnergyTypeHD[i] = 0;
            }
            
            memcpy((uint8*)&TermSharedData.PosReaEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PosReaEngHD[%d]:%d \n", i, TermSharedData.PosReaEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_NEG_REA_HD:                        //反向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                if((SrcData[X - 1] & 0x80) != 0)   //负
                {
                    SrcData[X - 1] &= 0x7F;
                    SignFlag = 1;
                }
                else
                {
                    SignFlag = 0;
                }
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.NegReaEnergyTypeHD[i] = 1;
            }
            else
            {
                gPriMeasurePara.NegReaEnergyTypeHD[i] = 0;
            }
            
            memcpy((uint8*)&TermSharedData.NegReaEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.NegReaEngHD[%d]:%d \n", i, TermSharedData.NegReaEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_1_REA_HD:                          //1象限无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad1ReaEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad1ReaEngHD[%d]:%d \n", i, TermSharedData.Quad1ReaEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_2_REA_HD:                          //2象限无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad2ReaEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad2ReaEngHD[%d]:%d \n", i, TermSharedData.Quad2ReaEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_3_REA_HD:                          //3象限无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad3ReaEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad3ReaEngHD[%d]:%d \n", i, TermSharedData.Quad3ReaEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_4_REA_HD:                          //4象限无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.Quad4ReaEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.Quad4ReaEngHD[%d]:%d \n", i, TermSharedData.Quad4ReaEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_POS_ACT_A_HD:                      //A相正向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaAPosActEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAPosActEngHD:%d \n", TermSharedData.PhaAPosActEngHD[0].ULinteger);
        break;

    case ES_NEG_ACT_A_HD:                      //A相反向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaANegActEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaANegActEngHD:%d \n", TermSharedData.PhaANegActEngHD[0].ULinteger);
        break;

    case ES_POS_REA_A_HD:                      //A相正向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //负
        {
            gPriMeasurePara.PhaAPosReaEngTypeHD[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaAPosReaEngTypeHD[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaAPosReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAPosReaEngHD:%d \n", TermSharedData.PhaAPosReaEngHD[0].ULinteger);
        break;

    case ES_NEG_REA_A_HD:                      //A相反向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //负
        {
            gPriMeasurePara.PhaANegReaEngTypeHD[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaANegReaEngTypeHD[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaANegReaEngHD, TempBuff, Y);
        
        ACCOMM_FMT_LOGCC("TermSharedData.PhaANegReaEngHD:%d \n", TermSharedData.PhaANegReaEngHD[0].ULinteger);
        break;

    case ES_POS_ACT_B_HD:                      //B相正向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBPosActEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBPosActEngHD:%d \n", TermSharedData.PhaBPosActEngHD[0].ULinteger);
        break;

    case ES_NEG_ACT_B_HD:                      //B相反向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBNegActEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBNegActEngHD:%d \n", TermSharedData.PhaBNegActEngHD[0].ULinteger);
        break;

    case ES_POS_REA_B_HD:                      //B相正向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //负
        {
            gPriMeasurePara.PhaBPosReaEngTypeHD[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaBPosReaEngTypeHD[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaBPosReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBPosReaEngHD:%d \n", TermSharedData.PhaBPosReaEngHD[0].ULinteger);
        break;

    case ES_NEG_REA_B_HD:                      //B相反向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //负
        {
            gPriMeasurePara.PhaBNegReaEngTypeHD[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaBNegReaEngTypeHD[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaBNegReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBNegReaEngHD:%d \n", TermSharedData.PhaBNegReaEngHD[0].ULinteger);
        break;

    case ES_POS_ACT_C_HD:                      //C相正向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_ACT_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCPosActEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCPosActEngHD:%d \n", TermSharedData.PhaCPosActEngHD[0].ULinteger);
        break;

    case ES_NEG_ACT_C_HD:                      //C相反向有功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_ACT_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCNegActEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCNegActEngHD:%d \n", TermSharedData.PhaCNegActEngHD[0].ULinteger);
        break;

    case ES_POS_REA_C_HD:                      //C相正向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_REA_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //负
        {
            gPriMeasurePara.PhaCPosReaEngTypeHD[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaCPosReaEngTypeHD[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaCPosReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCPosReaEngHD:%d \n", TermSharedData.PhaCPosReaEngHD[0].ULinteger);
        break;

    case ES_NEG_REA_C_HD:                      //C相反向无功
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_REA_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        i = 0;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        if(SignFlag == 1)                   //负
        {
            gPriMeasurePara.PhaCNegReaEngTypeHD[i] = 1;
        }
        else
        {
            gPriMeasurePara.PhaCNegReaEngTypeHD[i] = 0;
        }
        
        memcpy((uint8*)&TermSharedData.PhaCNegReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCNegReaEngHD:%d \n", TermSharedData.PhaCNegReaEngHD[0].ULinteger);
        break;

    case ES_CONBIN_ACT_HD:						//(当前)组合有功电能块
        ACCOMM_FMT_LOGCC("SaveER07() ES_CONBIN_ACT_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                if((SrcData[X - 1] & 0x80) != 0)   //负
                {
                    SrcData[X - 1] &= 0x7F;
                    SignFlag = 1;
                }
                else
                {
                    SignFlag = 0;
                }
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.CmbEnergyTypeHD[i] = 1;
            }
            else
            {
                gPriMeasurePara.CmbEnergyTypeHD[i] = 0;
            }
            
            memcpy((uint8*)&TermSharedData.CmbActEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.CmbActEngHD[%d]:%d \n", i, TermSharedData.CmbActEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_POS_APP_HD:						//(当前)正向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.PosAppEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PosAppEngHD[%d]:%d \n", i, TermSharedData.PosAppEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_NEG_APP_HD:						//(当前反向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_HD:...... \n");
        Len = DataLen / 5;
        if(Len < X)
            break;
        for(i = 0; i < 5; i++)
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy((uint8*)&TermSharedData.NegAppEngHD[i], TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.NegAppEngHD[%d]:%d \n", i, TermSharedData.NegAppEngHD[i].ULinteger);

            index += Len;
        }
        break;

    case ES_1_REA_A_HD:                      //A相第一象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA1ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA1ReaEngHD:%d \n", TermSharedData.PhaA1ReaEngHD[0].ULinteger);
        break;

    case ES_1_REA_B_HD:                      //B相第一象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB1ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB1ReaEngHD:%d \n", TermSharedData.PhaB1ReaEngHD[0].ULinteger);
        break;

    case ES_1_REA_C_HD:                      //C相第一象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_1_REA_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC1ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC1ReaEngHD:%d \n", TermSharedData.PhaC1ReaEngHD[0].ULinteger);
        break;

    case ES_2_REA_A_HD:                      //A相第二象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA2ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA2ReaEngHD:%d \n", TermSharedData.PhaA2ReaEngHD[0].ULinteger);
        break;

    case ES_2_REA_B_HD:                      //B相第二象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB2ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB2ReaEngHD:%d \n", TermSharedData.PhaB2ReaEngHD[0].ULinteger);
        break;

    case ES_2_REA_C_HD:                      //C相第二象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_2_REA_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC2ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC2ReaEngHD:%d \n", TermSharedData.PhaC2ReaEngHD[0].ULinteger);
        break;

    case ES_3_REA_A_HD:                      //A相第三象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA3ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA3ReaEngHD:%d \n", TermSharedData.PhaA3ReaEngHD[0].ULinteger);
        break;

    case ES_3_REA_B_HD:                      //B相第三象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB3ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB3ReaEngHD:%d \n", TermSharedData.PhaB3ReaEngHD[0].ULinteger);
        break;

    case ES_3_REA_C_HD:                      //C相第三象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_3_REA_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC3ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC3ReaEngHD:%d \n", TermSharedData.PhaC3ReaEngHD[0].ULinteger);
        break;

    case ES_4_REA_A_HD:                      //A相第四象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaA4ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaA4ReaEngHD:%d \n", TermSharedData.PhaA4ReaEngHD[0].ULinteger);
        break;

    case ES_4_REA_B_HD:                      //B相第四象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaB4ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaB4ReaEngHD:%d \n", TermSharedData.PhaB4ReaEngHD[0].ULinteger);
        break;

    case ES_4_REA_C_HD:                      //C相第四象限无功电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_4_REA_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaC4ReaEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaC4ReaEngHD:%d \n", TermSharedData.PhaC4ReaEngHD[0].ULinteger);
        break;

    case ES_POS_APP_A_HD:                      //A相正向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaAActAppEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAActAppEngHD:%d \n", TermSharedData.PhaAActAppEngHD[0].ULinteger);
        break;

    case ES_POS_APP_B_HD:                      //B相正向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBActAppEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBActAppEngHD:%d \n", TermSharedData.PhaBActAppEngHD[0].ULinteger);
        break;

    case ES_POS_APP_C_HD:                      //C相正向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_POS_APP_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCActAppEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCActAppEngHD:%d \n", TermSharedData.PhaCActAppEngHD[0].ULinteger);
        break;

    case ES_NEG_APP_A_HD:                      //A相反向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_A_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaANegAppEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaANegAppEngHD:%d \n", TermSharedData.PhaANegAppEngHD[0].ULinteger);
        break;

    case ES_NEG_APP_B_HD:                      //B相反向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_B_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaBNegAppEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaBNegAppEngHD:%d \n", TermSharedData.PhaBNegAppEngHD[0].ULinteger);
        break;

    case ES_NEG_APP_C_HD:                      //C相反向视在电能
        ACCOMM_FMT_LOGCC("SaveER07() ES_NEG_APP_C_HD:...... \n");
        Len = DataLen;
        if(Len < X)
            break;
        memcpy(SrcData, DataBuff, X);
        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint8*)&TermSharedData.PhaCNegAppEngHD, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaCNegAppEngHD:%d \n", TermSharedData.PhaCNegAppEngHD[0].ULinteger);
        break;

    /***高精度 电能量 end***/

    default:
        return;
    }
}

/*******************************************************************************
* 函数名称: SaveU07
* 函数功能: 存储电压
* 输入参数:
*           DataLen             数据长度
*           DataBuff            数据内容
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveU07(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   X, Y;                               //BCD码字节数、BIN码字节数
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 2;
    Y = 2;

    ACCOMM_FMT_LOGCC("SaveU07():...... \n");

    for(i = 0; i < 3; i++)
    {
        memcpy(SrcData, DataBuff + index, X);

        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint16*)&TermSharedData.PhaAVolt+i, TempBuff, Y);

        ACCOMM_FMT_LOGCC("TermSharedData.PhaAVolt+%d: %d \n", i, *((uint16*)&TermSharedData.PhaAVolt+i));

        index += X;
    }
}

/*******************************************************************************
* 函数名称: Savefreq07
* 函数功能: 存储电网频率
* 输入参数:
*           DataLen             数据长度
*           DataBuff            数据内容
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void Savefreq07(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   X, Y;                               //BCD码字节数、BIN码字节数

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 2;
    Y = 2;

    ACCOMM_FMT_LOGCC("Savefreq07():...... \n");

    memcpy(SrcData, DataBuff, X);

    if(memcmp(SrcData, NO_DATA, X) == 0)
    {
        memset(TempBuff, 0xff, Y);
    }
    else
    {
        BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
    }
    memcpy((uint8*)&TermSharedData.LineFrz, TempBuff, Y);

    ACCOMM_FMT_LOGCC("TermSharedData.LineFrz: %d \n", TermSharedData.LineFrz);
}

/*******************************************************************************
* 函数名称: SaveP07
* 函数功能: 存储功率
* 输入参数:
*           DataLen             数据长度
*           DataBuff            数据内容
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveP07(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //符号标志(0:正 / 1:负)
    uint8   X, Y;                               //BCD码字节数、BIN码字节数
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 3;
    Y = 4;

    ACCOMM_FMT_LOGCC("SaveP07():...... \n");

    for(i = 0; i < 4; i++)
    {
        memcpy(SrcData, DataBuff + index, X);

        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        index += X;
        if(i == 0)                              //总
        {
            if(SignFlag == 1)
            {
                gPriMeasurePara.CurPowDir.BitSect.ActPowFlag = 1;
                memcpy((uint8*)&TermSharedData.NegActPower, TempBuff, Y);
                memset(TempBuff, 0x00, 4);
                memcpy((uint8*)&TermSharedData.PosActPower, TempBuff, Y);
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.ActPowFlag = 0;
                memcpy((uint8*)&TermSharedData.PosActPower, TempBuff, Y);
                memset(TempBuff, 0x00, 4);
                memcpy((uint8*)&TermSharedData.NegActPower, TempBuff, Y);
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosActPower: %d, SignFlag=%d \n", TermSharedData.PosActPower, SignFlag);
            ACCOMM_FMT_LOGCC("TermSharedData.NegActPower: %d, SignFlag=%d \n", TermSharedData.NegActPower, SignFlag);
        }
        else if(i == 1) /* A相有功功率*/
        {
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaAActPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaAActPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaAActPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaAActPower: %d, SignFlag=%d \n", TermSharedData.PhaAActPower, SignFlag);
        }
        else if(i == 2) /* B相有功功率*/
        {
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaBActPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaBActPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaBActPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaBActPower: %d, SignFlag=%d \n", TermSharedData.PhaBActPower, SignFlag);
        }
        else if(i == 3) /* C相有功功率*/
        {
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaCActPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaCActPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaCActPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaCActPower: %d, SignFlag=%d \n", TermSharedData.PhaCActPower, SignFlag);
        }
    }
}

/*******************************************************************************
* 函数名称: SaveQ07
* 函数功能: 存储功率
* 输入参数:
*           DataLen             数据长度
*           DataBuff            数据内容
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveQ07(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //符号标志(0:正 / 1:负)
    uint8   X, Y;                               //BCD码字节数、BIN码字节数
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 3;
    Y = 4;

    ACCOMM_FMT_LOGCC("SaveQ07():...... \n");

    for(i = 0; i < 4; i++)
    {
        memcpy(SrcData, DataBuff + index, X);

        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        index += X;
        if(i == 0)                              //总
        {
            if(SignFlag == 1)
            {
                gPriMeasurePara.CurPowDir.BitSect.ReaPowFlag = 1;
                memcpy((uint8*)&TermSharedData.NegReaPower, TempBuff, Y);
                memset(TempBuff, 0x00, 4);
                memcpy((uint8*)&TermSharedData.PosReaPower, TempBuff, Y);
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.ReaPowFlag = 0;
                memcpy((uint8*)&TermSharedData.PosReaPower, TempBuff, Y);
                memset(TempBuff, 0x00, 4);
                memcpy((uint8*)&TermSharedData.NegReaPower, TempBuff, Y);
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosReaPower: %d, SignFlag=%d \n", TermSharedData.PosReaPower, SignFlag);
            ACCOMM_FMT_LOGCC("TermSharedData.NegReaPower: %d, SignFlag=%d \n", TermSharedData.NegReaPower, SignFlag);
        }
        else if(i == 1) /* A相无功功率*/
        {
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaAReaPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaAReaPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaAReaPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaAReaPower: %d, SignFlag=%d \n", TermSharedData.PhaAReaPower, SignFlag);
        }
        else if(i == 2) /* B相无功功率*/
        {
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaBReaPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaBReaPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaBReaPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaBReaPower: %d, SignFlag=%d \n", TermSharedData.PhaBReaPower, SignFlag);
        }
        else if(i == 3) /* C相无功功率*/
        {
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaCReaPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaCReaPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaCReaPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaCReaPower: %d, SignFlag=%d \n", TermSharedData.PhaCReaPower, SignFlag);
        }
    }
}

/*******************************************************************************
* 函数名称: SaveS07
* 函数功能: 存储功率
* 输入参数:
*           DataLen             数据长度
*           DataBuff            数据内容
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveS07(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //符号标志(0:正 / 1:负)
    uint8   X, Y;                               //BCD码字节数、BIN码字节数
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 3;
    Y = 4;

    ACCOMM_FMT_LOGCC("SaveS07():...... \n");

    for(i = 0; i < 4; i++)
    {
        memcpy(SrcData, DataBuff + index, X);

        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        index += X;
        if(i == 0)                              //总
        {
            if(SignFlag == 1)
            {
                gPriMeasurePara.CurPowDir.BitSect.AppPowFlag = 1;
                memcpy((uint8*)&TermSharedData.NegApparentPower, TempBuff, Y);
                memset(TempBuff, 0x00, 4);
                memcpy((uint8*)&TermSharedData.PosApparentPower, TempBuff, Y);
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.AppPowFlag = 0;
                memcpy((uint8*)&TermSharedData.PosApparentPower, TempBuff, Y);
                memset(TempBuff, 0x00, 4);
                memcpy((uint8*)&TermSharedData.NegApparentPower, TempBuff, Y);
            }

            ACCOMM_FMT_LOGCC("TermSharedData.PosApparentPower: %d, SignFlag=%d \n", TermSharedData.PosApparentPower, SignFlag);
            ACCOMM_FMT_LOGCC("TermSharedData.NegApparentPower: %d, SignFlag=%d \n", TermSharedData.NegApparentPower, SignFlag);
        }
        else if(i == 1) /* A相视在功率*/
        {
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaAAppPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaAAppPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaAAppPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaAAppPower: %d, SignFlag=%d \n", TermSharedData.PhaAAppPower, SignFlag);
        }
        else if(i == 2) /* B相视在功率*/
        {
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaBAppPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaBAppPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaBAppPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaBAppPower: %d, SignFlag=%d \n", TermSharedData.PhaBAppPower, SignFlag);
        }
        else if(i == 3) /* C相视在功率*/
        {
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaCAppPowFlag = 1;
            }
            else
            {
                gPriMeasurePara.CurPowDir.BitSect.PhaCAppPowFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaCAppPower, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaCAppPower: %d, SignFlag=%d \n", TermSharedData.PhaCAppPower, SignFlag);
        }
    }
}


/*******************************************************************************
* 函数名称: SaveI07
* 函数功能: 存储电流
* 输入参数: Pn                  测量点号
*           DataLen             数据长度
*           DataBuff            数据内容
*           DataItemLen         数据项长度
*           OffsetId            数据字典偏移
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveI07(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //符号标志(0:正 / 1:负)
    uint8   X, Y;                               //BCD码字节数、BIN码字节数
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 3;
    Y = 4;

    ACCOMM_FMT_LOGCC("SaveI07():...... \n");

    for(i = 0; i < 3; i++)
    {
        memcpy(SrcData, DataBuff + index, X);

        if(memcmp(SrcData, NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_SBinY(X, Y, TempBuff, SrcData, 1);
        }
        memcpy((uint32*)&TermSharedData.PhaACurr+i, TempBuff, Y);

        if(i == 0)
        {
            gPriMeasurePara.CurrentSignType.PhaAAppCurrentFlag = SignFlag;
        }
        else if(i == 1)
        {
            gPriMeasurePara.CurrentSignType.PhaBAppCurrentFlag = SignFlag;
        }
        else if(i == 2)
        {
            gPriMeasurePara.CurrentSignType.PhaCAppCurrentFlag = SignFlag;
        }

        ACCOMM_FMT_LOGCC("TermSharedData.PhaACurr+%d: %d, SignFlag=%d \n", i, *((uint32*)&TermSharedData.PhaACurr+i), SignFlag);

        index += X;
    }
}

/*******************************************************************************
* 函数名称: SaveZeroI
* 函数功能: 存储零序电流
* 输入参数: Pn                  测量点号
*           DataLen             数据长度
*           DataBuff            数据内容
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveZeroI(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   X, Y;                               //BCD码字节数、BIN码字节数
    uint8   SignFlag = 0;

    memcpy(SrcData, &DataBuff[0], 3);

    X = 3;
    Y = 4;
    
    ACCOMM_FMT_LOGCC("SaveZeroI():...... \n");
    
    if(memcmp(SrcData , NO_DATA, X) == 0)
    {
        memset(TempBuff, 0xff, Y);
    }
    else
    {
        if((SrcData[X - 1] & 0x80) != 0)          //负
        {
            SrcData[X - 1] &= 0x7F;
            SignFlag = 1;
        }
        else
        {
            SignFlag = 0;
        }

        //3字节BCD转换为4字节BIN
        BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
    }
    if(SignFlag ==  1)                      //负
    {
        *(uint32*)&TempBuff |= 0x80000000;
    }

    memcpy((uint8*)&TermSharedData.PhaMCurr, TempBuff, Y);
    gPriMeasurePara.CurrentSignType.ZeroCurrentFlag = SignFlag;

    ACCOMM_FMT_LOGCC("TermSharedData.PhaMCurr: %d \n", TermSharedData.PhaMCurr);

}

/*******************************************************************************
* 函数名称: SavePf
* 函数功能: 存储功率因数
* 输入参数: Pn                  测量点号
*           DataLen             数据长度
*           DataBuff            数据内容
*           OffsetId            数据字典偏移
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SavePf(uint8 DataLen, uint8 *DataBuff)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //符号标志(0:正 / 1:负)
    uint8   CurIndex = 0;                       //当前偏移
    uint8   X, Y;                               //BCD码字节数、BIN码字节数
    int i;

    X = 2;
    Y = 2;

    ACCOMM_FMT_LOGCC("SavePf():...... \n");

    for(i = 0; i < 4; i++)
    {
        memcpy(SrcData, &DataBuff[CurIndex], X);

        if(memcmp(SrcData , NO_DATA, X) == 0)
        {
            memset(TempBuff, 0xff, Y);
        }
        else
        {
            if((SrcData[X - 1] & 0x80) != 0)   //负
            {
                SrcData[X - 1] &= 0x7F;
                SignFlag = 1;
            }
            else
            {
                SignFlag = 0;
            }
            BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
        }
        CurIndex += X;
        if(i == 0)                              //总功率因数
        {
            if(SignFlag == 1)
            {
                gPriMeasurePara.PowerFactorType.AppPfFlag = 1;
            }
            else
            {
                gPriMeasurePara.PowerFactorType.AppPfFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PowerFactor, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PowerFactor: %d, SignFlag=%d \n", TermSharedData.PowerFactor, SignFlag);
        }
        else if(i == 1) /* A相功率因数*/
        {
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.PowerFactorType.PhaAAppPfFlag = 1;
            }
            else
            {
                gPriMeasurePara.PowerFactorType.PhaAAppPfFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaAPowerFactor, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaAPowerFactor: %d, SignFlag=%d \n", TermSharedData.PhaAPowerFactor, SignFlag);
        }
        else if(i == 2) /* B相功率因数*/
        {
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.PowerFactorType.PhaBAppPfFlag = 1;
            }
            else
            {
                gPriMeasurePara.PowerFactorType.PhaBAppPfFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaBPowerFactor, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaBPowerFactor: %d, SignFlag=%d \n", TermSharedData.PhaBPowerFactor, SignFlag);
        }
        else if(i == 3) /* C相功率因数*/
        {
            if(SignFlag == 1)                   //负
            {
                gPriMeasurePara.PowerFactorType.PhaCAppPfFlag = 1;
            }
            else
            {
                gPriMeasurePara.PowerFactorType.PhaCAppPfFlag = 0;
            }
            memcpy((uint8*)&TermSharedData.PhaCPowerFactor, TempBuff, Y);

            ACCOMM_FMT_LOGCC("TermSharedData.PhaCPowerFactor: %d, SignFlag=%d \n", TermSharedData.PhaCPowerFactor, SignFlag);
        }
    }
}

/*******************************************************************************
* 函数名称: SaveAngle
* 函数功能: 存储相角
* 输入参数:
*           DataLen             数据长度
*           DataBuff            数据内容
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveAngle(uint8 DataLen, uint8 *DataBuff, uint32	DI07)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //符号标志(0:正 / 1:负)
    uint8   X, Y;                               //BCD码字节数、BIN码字节数
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 2;
    Y = 2;

    ACCOMM_FMT_LOGCC("SaveAngle():...... \n");

    switch (DI07)
    {
        case 0x0207FF00:  //电压电流相角
        {
            for(i = 0; i < 3; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //负
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                index += X;

                if(i == 0)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaAVolCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaAVolCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaAAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() 电压电流相角 TermSharedData.PhaAAngle: %d \n", TermSharedData.PhaAAngle);		        
                }
                else if(i == 1)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBVolCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBVolCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaBAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() 电压电流相角 TermSharedData.PhaBAngle: %d \n", TermSharedData.PhaBAngle);		        
                }
                else if(i == 2)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCVolCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCVolCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaCAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() 电压电流相角 TermSharedData.PhaCAngle: %d \n", TermSharedData.PhaCAngle);		        
                }
            }
        }
            break;

        case 0xEFB10000:  //电压相角
        {
            for(i = 0; i < 3; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //负
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                index += X;

                if(i == 0)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaAVolAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaAVolAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaAVoltAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() 电压相角 TermSharedData.PhaAVoltAngle: %d \n", TermSharedData.PhaAVoltAngle);		        
                }
                else if(i == 1)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBVolAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBVolAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaBVoltAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() 电压相角 TermSharedData.PhaBVoltAngle: %d \n", TermSharedData.PhaBVoltAngle);		        
                }
                else if(i == 2)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCVolAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCVolAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaCVoltAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() 电压相角 TermSharedData.PhaCVoltAngle: %d \n", TermSharedData.PhaCVoltAngle);		        
                }
            }
        }
            break;

        case 0xEFB20000:  //电流相角
        {
            for(i = 0; i < 3; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //负
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                index += X;

                if(i == 0)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaACurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaACurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaACurrAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() 电流相角 TermSharedData.PhaACurrAngle: %d \n", TermSharedData.PhaACurrAngle);		        
                }
                else if(i == 1)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaBCurrAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() 电流相角 TermSharedData.PhaBCurrAngle: %d \n", TermSharedData.PhaBCurrAngle);		        
                }
                else if(i == 2)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaCCurrAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() 电流相角 TermSharedData.PhaCCurrAngle: %d \n", TermSharedData.PhaCCurrAngle);		        
                }
            }
        }
            break;

        case 0x028800FF:  //电压相角 与 电流相角
        {
            for(i = 0; i < 6; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //负
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                index += X;

                if(i == 0)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaAVolAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaAVolAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaAVoltAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() case 0x028800FF: 电压相角 TermSharedData.PhaAVoltAngle: %d \n", TermSharedData.PhaAVoltAngle);		        
                }
                else if(i == 1)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBVolAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBVolAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaBVoltAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() case 0x028800FF: 电压相角 TermSharedData.PhaBVoltAngle: %d \n", TermSharedData.PhaBVoltAngle);		        
                }
                else if(i == 2)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCVolAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCVolAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaCVoltAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() case 0x028800FF: 电压相角 TermSharedData.PhaCVoltAngle: %d \n", TermSharedData.PhaCVoltAngle);		        
                }
                else if(i == 3)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaACurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaACurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaACurrAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() case 0x028800FF: 电流相角 TermSharedData.PhaACurrAngle: %d \n", TermSharedData.PhaACurrAngle);		        
                }
                else if(i == 4)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaBCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaBCurrAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() case 0x028800FF: 电流相角 TermSharedData.PhaBCurrAngle: %d \n", TermSharedData.PhaBCurrAngle);		        
                }
                else if(i == 5)
                {
                    if(SignFlag == 1)                   //负
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCCurrAngFlag = 1;
                    }
                    else
                    {
                        gPriMeasurePara.AngleDir.BitSect.PhaCCurrAngFlag = 0;
                    }
                    memcpy(&TermSharedData.PhaCCurrAngle, TempBuff, Y);
                    ACCOMM_FMT_LOGCC("SaveAngle() case 0x028800FF: 电流相角 TermSharedData.PhaCCurrAngle: %d \n", TermSharedData.PhaCCurrAngle);		        
                }
            }
        }
            break;

        default:
            return;
    }

}

/*******************************************************************************
* 函数名称: SaveHarmonic
* 函数功能: 存储谐波
* 输入参数:
*           DataLen             数据长度
*           DataBuff            数据内容
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveHarmonic(uint8 DataLen, uint8 *DataBuff, uint32	DI07)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   SignFlag = 0;                       //符号标志(0:正 / 1:负)
    uint8   X, Y;                               //BCD码字节数、BIN码字节数
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 2;
    Y = 2;

    ACCOMM_FMT_LOGCC("SaveHarmonic():...... \n");

    switch (DI07)
    {
        //谐波含有率
        case 0x020A01FF:  //A相电压谐波含有率（总及2…n次）
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaAVoltHarmonic[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() 谐波信息 TermSharedData.PhaAVoltHarmonic[%d]: %d \n", i, TermSharedData.PhaAVoltHarmonic[i]);

                index += X;
            }
        }
            break;

        case 0x020A02FF:  //B相电压谐波含有率（总及2…n次）
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaBVoltHarmonic[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() 谐波信息 TermSharedData.PhaBVoltHarmonic[%d]: %d \n", i, TermSharedData.PhaBVoltHarmonic[i]);

                index += X;
            }
        }
            break;

        case 0x020A03FF:  //C相电压谐波含有率（总及2…n次）
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaCVoltHarmonic[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() 谐波信息 TermSharedData.PhaCVoltHarmonic[%d]: %d \n", i, TermSharedData.PhaCVoltHarmonic[i]);

                index += X;
            }
        }
            break;

        case 0x020B01FF:  //A相电流谐波含有率（总及2…n次）
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaACurrHarmonic[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() 谐波信息 TermSharedData.PhaACurrHarmonic[%d]: %d \n", i, TermSharedData.PhaACurrHarmonic[i]);

                index += X;
            }
        }
            break;

        case 0x020B02FF:  //B相电流谐波含有率（总及2…n次）
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaBCurrHarmonic[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() 谐波信息 TermSharedData.PhaBCurrHarmonic[%d]: %d \n", i, TermSharedData.PhaBCurrHarmonic[i]);

                index += X;
            }
        }
            break;

        case 0x020B03FF:  //C相电流谐波含有率（总及2…n次）	    
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaCCurrHarmonic[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() 谐波信息 TermSharedData.PhaCCurrHarmonic[%d]: %d \n", i, TermSharedData.PhaCCurrHarmonic[i]);

                index += X;
            }
        }
            break;


        //谐波含量
        case 0x020C01FF:  //A相电压谐波含量（总及2…n次）
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaAVoltHarmonicValue[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() 谐波信息 TermSharedData.PhaAVoltHarmonicValue[%d]: %d \n", i, TermSharedData.PhaAVoltHarmonicValue[i]);

                index += X;
            }
        }
            break;

        case 0x020C02FF:  //B相电压谐波含量（总及2…n次）
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaBVoltHarmonicValue[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() 谐波信息 TermSharedData.PhaBVoltHarmonicValue[%d]: %d \n", i, TermSharedData.PhaBVoltHarmonicValue[i]);

                index += X;
            }
        }
            break;

        case 0x020C03FF:  //C相电压谐波含量（总及2…n次）
        {
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy((uint16*)&TermSharedData.PhaCVoltHarmonicValue[i], TempBuff, Y);

                ACCOMM_FMT_LOGCC("SaveHarmonic() 谐波信息 TermSharedData.PhaCVoltHarmonicValue[%d]: %d \n", i, TermSharedData.PhaCVoltHarmonicValue[i]);

                index += X;
            }
        }
            break;

        case 0x020D01FF:  //A相电流谐波含量（总及2…n次）
        {
            X = 3;
            Y = 4;
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //负
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy(&TermSharedData.PhaACurrHarmonicValue[i], TempBuff, Y);
                gPriMeasurePara.HarmonicRateValueType.PhaACurrHarmonicValueFlag[i] = SignFlag;
                TermSharedData.PhaACurrHarmonicValue[i] = 10*TermSharedData.PhaACurrHarmonicValue[i];

                ACCOMM_FMT_LOGCC("SaveHarmonic() 谐波信息 TermSharedData.PhaACurrHarmonicValue[%d]: %d \n", i, TermSharedData.PhaACurrHarmonicValue[i]);

                index += X;
            }
        }
            break;

        case 0x020D02FF:  //B相电流谐波含量（总及2…n次）
        {
            X = 3;
            Y = 4;
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //负
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy(&TermSharedData.PhaBCurrHarmonicValue[i], TempBuff, Y);
                gPriMeasurePara.HarmonicRateValueType.PhaBCurrHarmonicValueFlag[i] = SignFlag;
                TermSharedData.PhaBCurrHarmonicValue[i] = 10*TermSharedData.PhaBCurrHarmonicValue[i];

                ACCOMM_FMT_LOGCC("SaveHarmonic() 谐波信息 TermSharedData.PhaBCurrHarmonicValue[%d]: %d \n", i, TermSharedData.PhaBCurrHarmonicValue[i]);

                index += X;
            }
        }
            break;

        case 0x020D03FF:  //C相电流谐波含量（总及2…n次）		
        {
            X = 3;
            Y = 4;
            for(i = 0; i < MAX_HARMONIC_NUM; i++)
            {
                memcpy(SrcData, DataBuff + index, X);

                if(memcmp(SrcData, NO_DATA, X) == 0)
                {
                    memset(TempBuff, 0xff, Y);
                }
                else
                {
                    if((SrcData[X - 1] & 0x80) != 0)   //负
                    {
                        SrcData[X - 1] &= 0x7F;
                        SignFlag = 1;
                    }
                    else
                    {
                        SignFlag = 0;
                    }
                    BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                }
                memcpy(&TermSharedData.PhaCCurrHarmonicValue[i], TempBuff, Y);
                gPriMeasurePara.HarmonicRateValueType.PhaCCurrHarmonicValueFlag[i] = SignFlag;
                TermSharedData.PhaCCurrHarmonicValue[i] = 10*TermSharedData.PhaCCurrHarmonicValue[i];

                ACCOMM_FMT_LOGCC("SaveHarmonic() 谐波信息 TermSharedData.PhaCCurrHarmonicValue[%d]: %d \n", i, TermSharedData.PhaCCurrHarmonicValue[i]);

                index += X;
            }
        }
            break;
            

        default:
            return;
    }
}

/*******************************************************************************
* 函数名称: SaveTopo
* 函数功能: 存储拓扑识别结果
* 输入参数: Pn                  测量点号
*           DataLen             数据长度
*           DataBuff            数据内容
*           DataItemLen         数据项长度
*           OffsetId            数据字典偏移
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SaveTopo(uint8 DataLen, uint8 *DataBuff, uint32	DI07)
{
    float32 fSrcValue = 0;
    uint32  nDstValue = 0;
    uint8   SignFlag = 0;                       //符号标志(0:正 / 1:负)
    uint8   X, Y;                               //BCD码字节数、BIN码字节数
    uint8   i = 0, index = 0;
    uint8   ConfirmBuf[MAX_QUEUE_BUFF_LEN] = {0};     //确认应答帧
    int     ConfirmBufLen = 0;
    int     ret = 0;

    ACCOMM_FMT_DEBUG("SaveTopo():...... \n");

    uint8 MinLen = 6+1+4+4+4+1+2;
    if(DataLen < MinLen)
    {
        ACCOMM_FMT_DEBUG("SaveTopo() if(DataLen < MinLen) MinLen=%d \n", MinLen);
        return;
    }

    //识别成功时间(直接采用终端时间)
    time_t tStartTime;//发生时间
    time(&tStartTime); /*获取time_t类型的当前时间*/
    systm_to_ooptm(tStartTime, &TermSharedData.TopoIdenResult.stIdenTime);  //事件发生时间
    index += 6;//(跳过6字节报文)

    //信号所属相位 1字节, 0:未知, 1:A相, 2:B相, 3:C相, 4:三相
    memcpy(&TermSharedData.TopoIdenResult.nPhase, DataBuff + index, 1);
    index += 1;

    //信号强度S1 4字节 报文以浮点型上报
    //信号强度S2 4字节 报文以浮点型上报
    //背景噪声 4字节 报文以浮点型上报
    X = 4;
    Y = 4;
    for(i = 0; i < 3; i++)
    {
        memcpy((uint8 *)&fSrcValue, DataBuff + index, X);
        if(fSrcValue < 0)
        {
            SignFlag = 1;
            nDstValue = (uint32)(0 - fSrcValue);
        }
        else
        {
            SignFlag = 0;
            nDstValue = (uint32)(fSrcValue);
        }

        if(i == 0)
        {
            memcpy(&TermSharedData.TopoIdenResult.nSignalS1, &nDstValue, Y);
            gPriMeasurePara.SignalIdenType.SignalS1Flag = SignFlag;
        }
        else if(i == 1)
        {
            memcpy(&TermSharedData.TopoIdenResult.nSignalS2, &nDstValue, Y);
            gPriMeasurePara.SignalIdenType.SignalS2Flag = SignFlag;
        }
        else if(i == 2)
        {
            memcpy(&TermSharedData.TopoIdenResult.nNoise, &nDstValue, Y);
            gPriMeasurePara.SignalIdenType.NoiseFlag = SignFlag;
        }

        index += X;
    }

    //识别信号长度 1字节
    memcpy(&TermSharedData.TopoIdenResult.nInfoLen, DataBuff + index, 1);
    index += 1;

    //识别信号内容 变长
    memcpy(TermSharedData.TopoIdenResult.aInfo, DataBuff + index, TermSharedData.TopoIdenResult.nInfoLen);
    index += TermSharedData.TopoIdenResult.nInfoLen;

    ACCOMM_FMT_DEBUG("nFlag=%d, nPhase=%d, nSignalS1=%u, SignalS2Flag=%d, nSignalS2=%u, SignalS2Flag=%d, nNoise=%u, NoiseFlag=%d, nInfoLen=%d \n", 
        TermSharedData.TopoIdenResult.nFlag, TermSharedData.TopoIdenResult.nPhase, TermSharedData.TopoIdenResult.nSignalS1, 
        gPriMeasurePara.SignalIdenType.SignalS1Flag, TermSharedData.TopoIdenResult.nSignalS2, gPriMeasurePara.SignalIdenType.SignalS2Flag, 
        TermSharedData.TopoIdenResult.nNoise, gPriMeasurePara.SignalIdenType.NoiseFlag, TermSharedData.TopoIdenResult.nInfoLen);
    ACCOMM_BUF_TRACE(TermSharedData.TopoIdenResult.aInfo, TermSharedData.TopoIdenResult.nInfoLen, "info:");

    //生成事件并上报
    ACDB_FMT_DEBUG("SaveTopo()检测: 发生拓扑识别事件! \n");
    InitEventPara();
    //SaveTopoEventOccur();
    SaveTopoResultRecord();

    //组装确认应答帧
    ConfirmBufLen = make_frame_confirm(DataBuff, DataLen, DI07, ConfirmBuf);
    if(ConfirmBufLen > 0)
    {
        ret = PlcWriteQueue(pConfirmDown, ConfirmBuf, ConfirmBufLen);
        ACCOMM_FMT_DEBUG("缓存确认应答报文, ConfirmBufLen=%d, ret=%d \n", ConfirmBufLen, ret);
    }
}

/*******************************************************************************
* 函数名称: SavePara
* 函数功能: 存储参数
* 输入参数:
*           DataLen             数据长度
*           DataBuff            数据内容
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void SavePara(uint8 DataLen, uint8 *DataBuff, uint32	DI07)
{
    uint8   SrcData[10] = {0};
    uint8   TempBuff[10] = {0};
    uint8   X, Y;                               //BCD码字节数、BIN码字节数
    uint8   i = 0, index = 0;

    memset(SrcData, 0x00, sizeof(SrcData));
    memset(TempBuff, 0x00, sizeof(TempBuff));

    X = 1;
    Y = 1;

    ACCOMM_FMT_TRACE("SavePara():...... \n");

    switch (DI07)
    {
        case 0x04000103:             //最大需量周期
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy(&TermSharedData.MaxDemandPeriodRD, TempBuff, Y);

            ACCOMM_FMT_TRACE("SavePara() 参数信息 MaxDemandPeriodRD: %u \n", TermSharedData.MaxDemandPeriodRD);
        }
            break;

        case 0x04000104:             //滑差时间
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy(&TermSharedData.SlipDiffTimeRD, TempBuff, Y);

            ACCOMM_FMT_TRACE("SavePara() 参数信息 SlipDiffTimeRD: %u \n", TermSharedData.SlipDiffTimeRD);
        }
            break;

        case 0x04000106:			   //备用套时区表切换时间
        {
            X = 5;
            Y = 5;
            uint8 nYear = 0;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(1, 1, &TempBuff[0], &SrcData[0], 1);
                BcdX_To_BinY(1, 1, &TempBuff[1], &SrcData[1], 1);
                BcdX_To_BinY(1, 1, &TempBuff[2], &SrcData[2], 1);
                BcdX_To_BinY(1, 1, &TempBuff[3], &SrcData[3], 1);
                BcdX_To_BinY(1, 1, &TempBuff[4], &SrcData[4], 1);
            }
            memcpy(&TermSharedData.AddiZoneChangeTimeRD.minute, &TempBuff[0], 1);
            memcpy(&TermSharedData.AddiZoneChangeTimeRD.hour, &TempBuff[1], 1);
            memcpy(&TermSharedData.AddiZoneChangeTimeRD.day, &TempBuff[2], 1);
            memcpy(&TermSharedData.AddiZoneChangeTimeRD.month, &TempBuff[3], 1);
            memcpy(&nYear, &TempBuff[4], 1);
            TermSharedData.AddiZoneChangeTimeRD.year = 2000 + nYear;

            ACCOMM_FMT_TRACE("SavePara() 备用套时区表切换时间 minute=%u, hour=%u, day=%u, month=%u, year=%u \n", 
                TermSharedData.AddiZoneChangeTimeRD.minute, TermSharedData.AddiZoneChangeTimeRD.hour, 
                TermSharedData.AddiZoneChangeTimeRD.day, TermSharedData.AddiZoneChangeTimeRD.month, 
                TermSharedData.AddiZoneChangeTimeRD.year);
        }
            break;

        case 0x04000107:			   //备用套日时段切换时间
        {
            X = 5;
            Y = 5;
            uint8 nYear = 0;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(1, 1, &TempBuff[0], &SrcData[0], 1);
                BcdX_To_BinY(1, 1, &TempBuff[1], &SrcData[1], 1);
                BcdX_To_BinY(1, 1, &TempBuff[2], &SrcData[2], 1);
                BcdX_To_BinY(1, 1, &TempBuff[3], &SrcData[3], 1);
                BcdX_To_BinY(1, 1, &TempBuff[4], &SrcData[4], 1);
            }
            memcpy(&TermSharedData.AddiPeriChangeTimeRD.minute, &TempBuff[0], 1);
            memcpy(&TermSharedData.AddiPeriChangeTimeRD.hour, &TempBuff[1], 1);
            memcpy(&TermSharedData.AddiPeriChangeTimeRD.day, &TempBuff[2], 1);
            memcpy(&TermSharedData.AddiPeriChangeTimeRD.month, &TempBuff[3], 1);
            memcpy(&nYear, &TempBuff[4], 1);
            TermSharedData.AddiPeriChangeTimeRD.year = 2000 + nYear;

            ACCOMM_FMT_TRACE("SavePara() 备用套日时段切换时间 minute=%u, hour=%u, day=%u, month=%u, year=%u \n", 
                TermSharedData.AddiPeriChangeTimeRD.minute, TermSharedData.AddiPeriChangeTimeRD.hour, 
                TermSharedData.AddiPeriChangeTimeRD.day, TermSharedData.AddiPeriChangeTimeRD.month, 
                TermSharedData.AddiPeriChangeTimeRD.year);
        }
            break;

        case 0x04000201:             //年时区数 ≤14
        {
            uint8 nNum = 0;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy(&nNum, TempBuff, Y);

            TermSharedData.ZonePeriNumRD.yPeriod = nNum;
            TermSharedData.CurrZoneTableRD.nNum = nNum;
            TermSharedData.AddiZoneTableRD.nNum = nNum;
            
            ACCOMM_FMT_TRACE("SavePara() 参数信息 年时区数(≤14): %u \n", nNum);
        }
            break;
        case 0x04000202:             //日时段表数 ≤8
        {
            uint8 nNum = 0;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy(&nNum, TempBuff, Y);

            TermSharedData.ZonePeriNumRD.dPeriodMet = nNum;
            TermSharedData.CurrDayPeriTableRD.nNum = nNum;
            TermSharedData.AddiDayPeriTableRD.nNum = nNum;
            
            ACCOMM_FMT_TRACE("SavePara() 参数信息 日时段表数(≤8): %u \n", nNum);
        }
            break;
        case 0x04000203:             //日时段数 ≤14
        {
            int i = 0;
            uint8 nNum = 0;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy(&nNum, TempBuff, Y);

            TermSharedData.ZonePeriNumRD.dPeriod = nNum;
            for(i=0; i<TermSharedData.CurrDayPeriTableRD.nNum; i++)
            {
                TermSharedData.CurrDayPeriTableRD.dPeriod[i].nNum = nNum;
            }
            for(i=0; i<TermSharedData.AddiDayPeriTableRD.nNum; i++)
            {
                TermSharedData.AddiDayPeriTableRD.dPeriod[i].nNum = nNum;
            }
            
            ACCOMM_FMT_TRACE("SavePara() 参数信息 日时段数(≤14): %u \n", nNum);
        }
            break;
        case 0x04000204:             //费率数 ≤63
        {
            uint8 nNum = 0;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
            }
            memcpy(&nNum, TempBuff, Y);

            TermSharedData.ZonePeriNumRD.rate = nNum;
            
            ACCOMM_FMT_TRACE("SavePara() 参数信息 费率数(≤63): %u \n", nNum);
        }
            break;
        case 0x04000205:             //公共假日数 ≤254
        {
            uint8 nNumH = 0;
            uint8 nNumL = 0;
            X = 2;
            Y = 2;
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                BcdX_To_BinY(1, 1, &TempBuff[0], &SrcData[0], 1);
                BcdX_To_BinY(1, 1, &TempBuff[1], &SrcData[1], 1);
            }
            memcpy(&nNumL, &TempBuff[0], 1);
            memcpy(&nNumH, &TempBuff[1], 1);

            TermSharedData.ZonePeriNumRD.pubHoliday = nNumL;
            
            ACCOMM_FMT_TRACE("SavePara() 参数信息 公共假日数(≤254): %u \n", nNumL);
        }
            break;

        case 0x04000601:			   //有功组合方式特征字
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                //特征字不需要转码, 直接取值
                //BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                memcpy(TempBuff, SrcData, Y);
            }
            memcpy(&TermSharedData.ActPowCombWordRD.nValue, TempBuff, Y);

            ACCOMM_FMT_TRACE("SavePara() 参数信息 ActPowCombWordRD: %u \n", TermSharedData.ActPowCombWordRD.nValue);
        }
            break;

        case 0x04000602:			   //无功组合方式1特征字
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                //特征字不需要转码, 直接取值
                //BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                memcpy(TempBuff, SrcData, Y);
            }
            memcpy(&TermSharedData.ReaPowComb1WordRD.nValue, TempBuff, Y);

            ACCOMM_FMT_TRACE("SavePara() 参数信息 ReaPowComb1WordRD: %u \n", TermSharedData.ReaPowComb1WordRD.nValue);
        }
            break;

        case 0x04000603:			   //无功组合方式2特征字
        {
            memcpy(SrcData, DataBuff + index, X);

            if(memcmp(SrcData, NO_DATA, X) == 0)
            {
                memset(TempBuff, 0xff, Y);
            }
            else
            {
                //特征字不需要转码, 直接取值
                //BcdX_To_BinY(X, Y, TempBuff, SrcData, 1);
                memcpy(TempBuff, SrcData, Y);
            }
            memcpy(&TermSharedData.ReaPowComb2WordRD.nValue, TempBuff, Y);

            ACCOMM_FMT_TRACE("SavePara() 参数信息 ReaPowComb2WordRD: %u \n", TermSharedData.ReaPowComb2WordRD.nValue);
        }
            break;

        case 0x04010000:             //当前套时区表(最多14个时区)
        {
            uint8 nZoneCount = DataLen/3;//每个时区均有时段表号、日、月3个字节
            TermSharedData.CurrZoneTableRD.nNum = nZoneCount;

            for(i=0; i<nZoneCount; i++)
            {
                memcpy(SrcData, DataBuff + index, 3);//3个字节依次为表号、日、月

                if(memcmp(SrcData, NO_DATA, 3) == 0)
                {
                    memset(TempBuff, 0xff, 3);
                }
                else
                {
                    //表号
                    BcdX_To_BinY(X, Y, &TempBuff[0], &SrcData[0], 1);
                    
                    //日
                    BcdX_To_BinY(X, Y, &TempBuff[1], &SrcData[1], 1);

                    //月
                    BcdX_To_BinY(X, Y, &TempBuff[2], &SrcData[2], 1);
                }
                memcpy(&TermSharedData.CurrZoneTableRD.period[i].meterNum, &TempBuff[0], Y);
                memcpy(&TermSharedData.CurrZoneTableRD.period[i].day, &TempBuff[1], Y);
                memcpy(&TermSharedData.CurrZoneTableRD.period[i].mon, &TempBuff[2], Y);

                ACCOMM_FMT_TRACE("SavePara() 参数信息 CurrZoneTableRD i=%d, nNum=%d, meterNum=%d, day=%d, mon=%d \n", 
                    i, TermSharedData.CurrZoneTableRD.nNum, TermSharedData.CurrZoneTableRD.period[i].meterNum, 
                    TermSharedData.CurrZoneTableRD.period[i].day, TermSharedData.CurrZoneTableRD.period[i].mon);

                index += 3;
            }
            
        }
            break;

        case 0x04020000:             //备用套时区表(最多14个时区)
        {
            uint8 nZoneCount = DataLen/3;//每个时区均有时段表号、日、月3个字节
            TermSharedData.AddiZoneTableRD.nNum = nZoneCount;

            for(i=0; i<nZoneCount; i++)
            {
                memcpy(SrcData, DataBuff + index, 3);//3个字节依次为表号、日、月

                if(memcmp(SrcData, NO_DATA, 3) == 0)
                {
                    memset(TempBuff, 0xff, 3);
                }
                else
                {
                    //表号
                    BcdX_To_BinY(X, Y, &TempBuff[0], &SrcData[0], 1);
                    
                    //日
                    BcdX_To_BinY(X, Y, &TempBuff[1], &SrcData[1], 1);

                    //月
                    BcdX_To_BinY(X, Y, &TempBuff[2], &SrcData[2], 1);
                }
                memcpy(&TermSharedData.AddiZoneTableRD.period[i].meterNum, &TempBuff[0], Y);
                memcpy(&TermSharedData.AddiZoneTableRD.period[i].day, &TempBuff[1], Y);
                memcpy(&TermSharedData.AddiZoneTableRD.period[i].mon, &TempBuff[2], Y);

                ACCOMM_FMT_TRACE("SavePara() 参数信息 AddiZoneTableRD i=%d, nNum=%d, meterNum=%d, day=%d, mon=%d \n", 
                    i, TermSharedData.AddiZoneTableRD.nNum, TermSharedData.AddiZoneTableRD.period[i].meterNum, 
                    TermSharedData.AddiZoneTableRD.period[i].day, TermSharedData.AddiZoneTableRD.period[i].mon);

                index += 3;
            }
            
        }
            break;

        case 0x04010001:             //当前套日时段表(最多8日, 第1日时段时段表数据, 每日最多14时段)
        case 0x04010002:             //当前套日时段表(最多8日, 第2日时段时段表数据, 每日最多14时段)
        case 0x04010003:             //当前套日时段表(最多8日, 第3日时段时段表数据, 每日最多14时段)
        case 0x04010004:             //当前套日时段表(最多8日, 第4日时段时段表数据, 每日最多14时段)
        case 0x04010005:             //当前套日时段表(最多8日, 第5日时段时段表数据, 每日最多14时段)
        case 0x04010006:             //当前套日时段表(最多8日, 第6日时段时段表数据, 每日最多14时段)
        case 0x04010007:             //当前套日时段表(最多8日, 第7日时段时段表数据, 每日最多14时段)
        case 0x04010008:             //当前套日时段表(最多8日, 第8日时段时段表数据, 每日最多14时段)
        {
            uint8 nTableIndex = (uint32)(DI07 & 0x000000ff) - 1;
            uint8 nPeriodCount = DataLen/3;//每个时段均有费率号、分、时3个字节
            TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].nNum = nPeriodCount;
            
            for(i = 0; i < nPeriodCount; i++)
            {
                memcpy(SrcData, DataBuff + index, 3);//3个字节依次为表号、日、月

                if(memcmp(SrcData, NO_DATA, 3) == 0)
                {
                    memset(TempBuff, 0xff, 3);
                }
                else
                {
                    //费率号
                    BcdX_To_BinY(X, Y, &TempBuff[0], &SrcData[0], 1);
                    
                    //分钟
                    BcdX_To_BinY(X, Y, &TempBuff[1], &SrcData[1], 1);

                    //时
                    BcdX_To_BinY(X, Y, &TempBuff[2], &SrcData[2], 1);
                }
                memcpy(&TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].period[i].rateNO, &TempBuff[0], Y);
                memcpy(&TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].period[i].min, &TempBuff[1], Y);
                memcpy(&TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].period[i].hour, &TempBuff[2], Y);

                ACCOMM_FMT_TRACE("SavePara() 参数信息 CurrDayPeriTableRD nTableIndex=%d, nNum=%d, i=%d, periodNum=%d, rateNO=%d, min=%d, hour=%d \n", 
                    nTableIndex, TermSharedData.CurrDayPeriTableRD.nNum, i, TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].nNum, 
                    TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].period[i].rateNO, TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].period[i].min, 
                    TermSharedData.CurrDayPeriTableRD.dPeriod[nTableIndex].period[i].hour);

                index += 3;
            }

            g_nRefreshCurrDayFlagArr[nTableIndex] = 1;//已刷新
        }
            break;

        case 0x04020001:             //备用套日时段表(最多8日, 第1日时段时段表数据, 每日最多14时段)
        case 0x04020002:             //备用套日时段表(最多8日, 第2日时段时段表数据, 每日最多14时段)
        case 0x04020003:             //备用套日时段表(最多8日, 第3日时段时段表数据, 每日最多14时段)
        case 0x04020004:             //备用套日时段表(最多8日, 第4日时段时段表数据, 每日最多14时段)
        case 0x04020005:             //备用套日时段表(最多8日, 第5日时段时段表数据, 每日最多14时段)
        case 0x04020006:             //备用套日时段表(最多8日, 第6日时段时段表数据, 每日最多14时段)
        case 0x04020007:             //备用套日时段表(最多8日, 第7日时段时段表数据, 每日最多14时段)
        case 0x04020008:             //备用套日时段表(最多8日, 第8日时段时段表数据, 每日最多14时段)
        {
            uint8 nTableIndex = (uint32)(DI07 & 0x000000ff) - 1;
            uint8 nPeriodCount = DataLen/3;//每个时段均有费率号、分、时3个字节
            TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].nNum = nPeriodCount;
            
            for(i = 0; i < nPeriodCount; i++)
            {
                memcpy(SrcData, DataBuff + index, 3);//3个字节依次为表号、日、月

                if(memcmp(SrcData, NO_DATA, 3) == 0)
                {
                    memset(TempBuff, 0xff, 3);
                }
                else
                {
                    //费率号
                    BcdX_To_BinY(X, Y, &TempBuff[0], &SrcData[0], 1);
                    
                    //分钟
                    BcdX_To_BinY(X, Y, &TempBuff[1], &SrcData[1], 1);

                    //时
                    BcdX_To_BinY(X, Y, &TempBuff[2], &SrcData[2], 1);
                }
                memcpy(&TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].period[i].rateNO, &TempBuff[0], Y);
                memcpy(&TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].period[i].min, &TempBuff[1], Y);
                memcpy(&TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].period[i].hour, &TempBuff[2], Y);

                ACCOMM_FMT_TRACE("SavePara() 参数信息 CurrDayPeriTableRD nTableIndex=%d, nNum=%d, i=%d, periodNum=%d, rateNO=%d, min=%d, hour=%d \n", 
                    nTableIndex, TermSharedData.AddiDayPeriTableRD.nNum, i, TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].nNum, 
                    TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].period[i].rateNO, TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].period[i].min, 
                    TermSharedData.AddiDayPeriTableRD.dPeriod[nTableIndex].period[i].hour);

                index += 3;
            }

            g_nRefreshAddiDayFlagArr[nTableIndex] = 1;//已刷新
        }
            break;

        case 0x04000503:             //当前交采模块的日时段表运行在哪一套
        {
            memcpy(&TermSharedData.g_nCurrDayPeriLabel, DataBuff + index, 1);
            TermSharedData.g_nCurrDayPeriLabel &= 0x01;
            ACCOMM_FMT_TRACE("SavePara() 参数信息 g_nCurrDayPeriLabel: %u(0:第一套; 1:第二套) \n", TermSharedData.g_nCurrDayPeriLabel);
            g_nRefreshCurrDayPeriLabelFlag = 1;//已刷新
        }
            break;

        default:
            return;
    }
}

/*******************************************************************************
* 函数名称:	AmrRtDataSave
* 函数功能:	抄表实时数据存储
* 输入参数:	PrtlID              规约类型
*           DataLen             数据长度
*           DataBuff            数据内容
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
void AmrRtDataSave(uint16 DataLen, uint8 *DataBuff)
{
    uint32  DI07;                                //07数据标识
    uint16  Lenoffset = 0;
    uint8   nLen = 0;

    Lenoffset = 0;
    while(Lenoffset < DataLen)
    {
        DI07 = *(uint32*)(&DataBuff[Lenoffset]);
        nLen = DataBuff[Lenoffset+4];
        switch(DI07)
        {
        case 0x0001ff00:                        //(当前)正向有功电能总尖峰平谷示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0002ff00:                        //(当前)反向有功电能总尖峰平谷示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0003ff00:                        //(当前)正向无功电能总尖峰平谷示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0004ff00:                        //(当前)反向无功电能总尖峰平谷示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0005ff00:                        //(当前)第一象限无功总尖峰平谷电能示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0006ff00:                        //(当前)第二象限无功总尖峰平谷电能示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0007ff00:                        //(当前)第三象限无功总尖峰平谷电能示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0008ff00:                        //(当前)第四象限无功总尖峰平谷电能示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00150000:                        //(A相)正向有功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00160000:                        //(A相)反向有功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00170000:                        //(A相)正向无功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00180000:                        //(A相)反向无功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00290000:                        //(B相)正向有功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x002a0000:                        //(B相)反向有功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x002b0000:                        //(B相)正向无功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x002c0000:                        //(B相)反向无功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x003d0000:                        //(C相)正向有功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x003e0000:                        //(C相)反向有功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x003f0000:                        //(C相)正向无功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00400000:                        //(C相)反向无功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_C, 0);
            Lenoffset += (nLen+5);
            break;


        /* 电能 新增 begin */
        case 0x0000ff00:                        //(当前)组合有功电能块
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_CONBIN_ACT, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x0009ff00:                        //(当前)正向视在电能
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x000aff00:                        //(当前)反向视在电能
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00190000:                        //A相第一象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x002d0000:                        //B相第一象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00410000:                        //C相第一象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x001a0000:                        //A相第二象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x002e0000:                        //B相第二象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00420000:                        //C相第二象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x001b0000:                        //A相第三象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x002f0000:                        //B相第三象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00430000:                        //C相第三象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x001c0000:                        //A相第四象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00300000:                        //B相第四象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00440000:                        //C相第四象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x001d0000:                        //A相正向视在电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00310000:                        //B相正向视在电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00450000:                        //C相正向视在电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_C, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x001e0000:                        //A 相反向视在电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_A, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00320000:                        //B相反向视在电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_B, 0);
            Lenoffset += (nLen+5);
            break;

        case 0x00460000:                        //C相反向视在电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_C, 0);
            Lenoffset += (nLen+5);
            break;

        /* 电能 新增 end */

        /***高精度 电能量 begin***/
        case 0xf101ff00:                        //(当前)正向有功电能总尖峰平谷示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf102ff00:                        //(当前)反向有功电能总尖峰平谷示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf103ff00:                        //(当前)正向无功电能总尖峰平谷示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf104ff00:                        //(当前)反向无功电能总尖峰平谷示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf105ff00:                        //(当前)第一象限无功总尖峰平谷电能示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf106ff00:                        //(当前)第二象限无功总尖峰平谷电能示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf107ff00:                        //(当前)第三象限无功总尖峰平谷电能示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf108ff00:                        //(当前)第四象限无功总尖峰平谷电能示值
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1150000:                        //(A相)正向有功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1160000:                        //(A相)反向有功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1170000:                        //(A相)正向无功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1180000:                        //(A相)反向无功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1290000:                        //(B相)正向有功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf12a0000:                        //(B相)反向有功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf12b0000:                        //(B相)正向无功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf12c0000:                        //(B相)反向无功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf13d0000:                        //(C相)正向有功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf13e0000:                        //(C相)反向有功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf13f0000:                        //(C相)正向无功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1400000:                        //(C相)反向无功总电能示值
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_C_HD, 1);
            Lenoffset += (nLen+5);
            break;


        /* 电能 新增 begin */
        case 0xf100ff00:                        //(当前)组合有功电能块
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_CONBIN_ACT_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf109ff00:                        //(当前)正向视在电能
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf10aff00:                        //(当前)反向视在电能
            if(nLen > 25)
            {
                nLen = 25;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1190000:                        //A相第一象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf12d0000:                        //B相第一象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1410000:                        //C相第一象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf11a0000:                        //A相第二象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf12e0000:                        //B相第二象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1420000:                        //C相第二象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf11b0000:                        //A相第三象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf12f0000:                        //B相第三象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1430000:                        //C相第三象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf11c0000:                        //A相第四象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1300000:                        //B相第四象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1440000:                        //C相第四象限无功电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf11d0000:                        //A相正向视在电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1310000:                        //B相正向视在电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1450000:                        //C相正向视在电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf11e0000:                        //A 相反向视在电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_A_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1320000:                        //B相反向视在电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_B_HD, 1);
            Lenoffset += (nLen+5);
            break;

        case 0xf1460000:                        //C相反向视在电能
            if(nLen > 5)
            {
                nLen = 5;
            }
            SaveER07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_C_HD, 1);
            Lenoffset += (nLen+5);
            break;

        /* 电能 新增 end */

        /***高精度 电能量 end***/

        case 0x0101ff00:                //(当前)正向有功总尖峰平谷最大需量
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT);
            Lenoffset += (nLen+5);
            break;

        case 0x0102ff00:                //(当前)反向有功总尖峰平谷最大需量
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT);
            Lenoffset += (nLen+5);
            break;

        case 0x0103ff00:                //(当前)正向无功总尖峰平谷最大需量
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA);
            Lenoffset += (nLen+5);
            break;

        case 0x0104ff00:                //(当前)反向无功总尖峰平谷最大需量
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA);
            Lenoffset += (nLen+5);
            break;
            
        /* 需量 新增 begin */
        case 0x0105FF00:                //(当前)第一象限最大需量
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_1_REA);
            Lenoffset += (nLen+5);
            break;

        case 0x0106FF00:                //(当前)第二象限最大需量
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_2_REA);
            Lenoffset += (nLen+5);
            break;

        case 0x0107FF00:                //(当前)第三象限最大需量
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_3_REA);
            Lenoffset += (nLen+5);
            break;

        case 0x0108FF00:                //(当前)第四象限最大需量
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_4_REA);
            Lenoffset += (nLen+5);
            break;

        case 0x0109ff00:                //(当前)正向视在最大需量
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP);
            Lenoffset += (nLen+5);
            break;

        case 0x010aff00:                //(当前)反向视在最大需量
            if(nLen > 45)
            {
                nLen = 45;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP);
            Lenoffset += (nLen+5);
            break;          

        case 0x01150000:                //(当前)A相 正向有功
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_A);
            Lenoffset += (nLen+5);
            break;

        case 0x01290000:                //(当前)B相 正向有功
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_B);
            Lenoffset += (nLen+5);
            break;

        case 0x013d0000:                //(当前)C相 正向有功
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_ACT_C);
            Lenoffset += (nLen+5);
            break;

        case 0x01160000:                //(当前)A相 反向有功
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_A);
            Lenoffset += (nLen+5);
            break;

        case 0x012a0000:                //(当前)B相 反向有功
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_B);
            Lenoffset += (nLen+5);
            break;

        case 0x013e0000:                //(当前)C相 反向有功
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_ACT_C);
            Lenoffset += (nLen+5);
            break;

        case 0x01170000:                //(当前)A相 组合无功1
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_A);
            Lenoffset += (nLen+5);
            break;

        case 0x012b0000:                //(当前)B相 组合无功1
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_B);
            Lenoffset += (nLen+5);
            break;

        case 0x013f0000:                //(当前)C相 组合无功1
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_REA_C);
            Lenoffset += (nLen+5);
            break;

        case 0x01180000:                //(当前)A相 组合无功2
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_A);
            Lenoffset += (nLen+5);
            break;

        case 0x012c0000:                //(当前)B相 组合无功2
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01400000:                //(当前)C相 组合无功2
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_REA_C);
            Lenoffset += (nLen+5);
            break;

        case 0x01190000:                //(当前)A相 第一象限最大需量
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_A);
            Lenoffset += (nLen+5);
            break;

        case 0x012d0000:                //(当前)B相 第一象限最大需量
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01410000:                //(当前)C相 第一象限最大需量
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_1_REA_C);
            Lenoffset += (nLen+5);
            break;

        case 0x011a0000:                //(当前)A相 第二象限无功电能
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_A);
            Lenoffset += (nLen+5);
            break;

        case 0x012e0000:                //(当前)B相 第二象限无功电能
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01420000:                //(当前)C相 第二象限无功电能
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_2_REA_C);
            Lenoffset += (nLen+5);
            break;

        case 0x011b0000:                //(当前)A相 第三象限最大需量
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_A);
            Lenoffset += (nLen+5);
            break;

        case 0x012f0000:                //(当前)B相 第三象限最大需量
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01430000:                //(当前)C相 第三象限最大需量
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_3_REA_C);
            Lenoffset += (nLen+5);
            break;

        case 0x011c0000:                //(当前)A相 第四象限无功电能
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_A);
            Lenoffset += (nLen+5);
            break;

        case 0x01300000:                //(当前)B相 第四象限无功电能
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01440000:                //(当前)C相 第四象限无功电能
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_4_REA_C);
            Lenoffset += (nLen+5);
            break;

        case 0x011d0000:                //(当前)A相 正向视在最大需量
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_A);
            Lenoffset += (nLen+5);
            break;

        case 0x01310000:                //(当前)B相 正向视在最大需量
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01450000:                //(当前)C相 正向视在最大需量
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_POS_APP_C);
            Lenoffset += (nLen+5);
            break;

        case 0x011e0000:                //(当前)A相 反向视在电能
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_A);
            Lenoffset += (nLen+5);
            break;

        case 0x01320000:                //(当前)B相 反向视在电能
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_B);
            Lenoffset += (nLen+5);
            break;

        case 0x01460000:                //(当前)C相 反向视在电能
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveDemand07(nLen, &DataBuff[Lenoffset+5], ES_NEG_APP_C);
            Lenoffset += (nLen+5);
            break;

        /* 需量 新增 end */
            

        case 0x0203ff00:                        //(当前)瞬时有功功率
            if(nLen > 12)
            {
                nLen = 12;
            }

            SaveP07(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        case 0x0204ff00:               //(当前)瞬时无功功率
            if(nLen > 12)
            {
                nLen = 12;
            }
            SaveQ07(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        /* 视在功率 新增 begin */
        case 0x0205FF00:               //(当前)瞬时视在功率
            if(nLen > 12)
            {
                nLen = 12;
            }
            SaveS07(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        /* 视在功率 新增 end*/

        case 0x0201ff00:               //(当前)三相电压
            if(nLen > 6)
            {
                nLen = 6;
            }
            SaveU07(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        case 0x0202ff00:                        //(当前)三相电流
            if(nLen > 9)
            {
                nLen = 9;
            }
            SaveI07(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        case 0x02800001:                        //(当前)零序电流
            if(nLen > 3)
            {
                nLen = 3;
            }
            SaveZeroI(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        case 0x02800002:                        //(当前)电网频率
            if(nLen > 2)
            {
                nLen = 2;
            }
            Savefreq07(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        case 0x0206ff00:                        //(当前)瞬时总及分相功率因数
            if(nLen > 8)
            {
                nLen = 8;
            }
            SavePf(nLen, &DataBuff[Lenoffset+5]);
            Lenoffset += (nLen+5);
            break;

        case 0x04000507:  //状态字7
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw7, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() 状态字 0x04000507: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw7, 2, "状态字");
            break;

        case 0x04000506:  //状态字6
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw6, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() 状态字 0x04000506: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw6, 2, "状态字");
            break;

        case 0x04000505:  //状态字5
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw5, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() 状态字 0x04000505: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw5, 2, "状态字");
            break;

        case 0x04000504:  //状态字4
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw4, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() 状态字 0x04000504: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw4, 2, "状态字");
            break;

        case 0x04000503:  //状态字3
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw3, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() 状态字 0x04000503: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw3, 2, "状态字");
            break;

        case 0x04000502:  //状态字2
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw2, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() 状态字 0x04000502: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw2, 2, "状态字");
            break;

        case 0x04000501:  //状态字1
            if(nLen > 2)
            {
                nLen = 2;
            }
            memcpy((uint16*)&TermSharedData.MetStusPara.Rsw1, &DataBuff[Lenoffset+5], 2);
            Lenoffset += (nLen+5);
            ACCOMM_FMT_LOGCC("AmrRtDataSave() 状态字 0x04000501: \n");
            ACCOMM_BUF_LOGCC((uint8*)&TermSharedData.MetStusPara.Rsw1, 2, "状态字");
            break;

        /* 相角 新增 begin */
        case 0x0207FF00:  //电压电流相角
        case 0xEFB10000:  //电压相角
        case 0xEFB20000:  //电流相角
            if(nLen > 6)
            {
                nLen = 6;
            }
            SaveAngle(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;

        case 0x028800FF:  //电压相角 与 电流相角
            if(nLen > 12)
            {
                nLen = 12;
            }
            SaveAngle(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;

        /* 相角 新增 end */

        /* 谐波 新增 begin */
        case 0x020A01FF:  //A相电压谐波含有率（总及2…n次）
        case 0x020A02FF:  //B相电压谐波含有率（总及2…n次）
        case 0x020A03FF:  //C相电压谐波含有率（总及2…n次）
        case 0x020B01FF:  //A相电流谐波含有率（总及2…n次）
        case 0x020B02FF:  //B相电流谐波含有率（总及2…n次）
        case 0x020B03FF:  //C相电流谐波含有率（总及2…n次）
            if(nLen > (2*31))
            {
                nLen = 2*31;//交采模块上送的报文是31次
            }
            SaveHarmonic(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;

        /* 谐波 新增 end */

        /* 谐波含量 新增 begin */
        case 0x020C01FF:  //A相电压谐波含量（总及2…n次）
        case 0x020C02FF:  //B相电压谐波含量（总及2…n次）
        case 0x020C03FF:  //C相电压谐波含量（总及2…n次）
        if(nLen > (2*31))
            {
                nLen = 2*31;//交采模块上送的报文是31次
            }
            SaveHarmonic(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;
        case 0x020D01FF:  //A相电流谐波含量（总及2…n次）
        case 0x020D02FF:  //B相电流谐波含量（总及2…n次）
        case 0x020D03FF:  //C相电流谐波含量（总及2…n次）
            if(nLen > (3*31))
            {
                nLen = 3*31;//交采模块上送的报文是31次
            }
            SaveHarmonic(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;

        /* 谐波 新增 end */

        /* 交采参数 新增 begin */
        case 0x04000103:             //最大需量周期
            if(nLen > 1)
            {
                nLen = 1;
            }
            SavePara(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;
        case 0x04000104:             //滑差时间
            if(nLen > 1)
            {
                nLen = 1;
            }
            SavePara(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;
        case 0x04010000:             //当前套时区表(最多14个时区)
        case 0x04020000:             //备用套时区表(最多14个时区)
            if(nLen > 3*14)
            {
                nLen = 3*14;
            }
            SavePara(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;
        case 0x04010001:             //当前套日时段表(最多8日, 第1日时段时段表数据, 每日最多14时段)
        case 0x04010002:             //当前套日时段表(最多8日, 第2日时段时段表数据, 每日最多14时段)
        case 0x04010003:             //当前套日时段表(最多8日, 第3日时段时段表数据, 每日最多14时段)
        case 0x04010004:             //当前套日时段表(最多8日, 第4日时段时段表数据, 每日最多14时段)
        case 0x04010005:             //当前套日时段表(最多8日, 第5日时段时段表数据, 每日最多14时段)
        case 0x04010006:             //当前套日时段表(最多8日, 第6日时段时段表数据, 每日最多14时段)
        case 0x04010007:             //当前套日时段表(最多8日, 第7日时段时段表数据, 每日最多14时段)
        case 0x04010008:             //当前套日时段表(最多8日, 第8日时段时段表数据, 每日最多14时段)
        case 0x04020001:             //备用套日时段表(最多8日, 第1日时段时段表数据, 每日最多14时段)
        case 0x04020002:             //备用套日时段表(最多8日, 第2日时段时段表数据, 每日最多14时段)
        case 0x04020003:             //备用套日时段表(最多8日, 第3日时段时段表数据, 每日最多14时段)
        case 0x04020004:             //备用套日时段表(最多8日, 第4日时段时段表数据, 每日最多14时段)
        case 0x04020005:             //备用套日时段表(最多8日, 第5日时段时段表数据, 每日最多14时段)
        case 0x04020006:             //备用套日时段表(最多8日, 第6日时段时段表数据, 每日最多14时段)
        case 0x04020007:             //备用套日时段表(最多8日, 第7日时段时段表数据, 每日最多14时段)
        case 0x04020008:             //备用套日时段表(最多8日, 第8日时段时段表数据, 每日最多14时段)
            if(nLen > 3*14)
            {
                nLen = 3*14;
            }
            SavePara(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;

        /* 交采参数 新增 end */

        /* 拓扑识别 新增 begin */
        case 0xf2000001:             //(自定义扩展)拓扑识别结果(浙江)
            SaveTopo(nLen, &DataBuff[Lenoffset+5], DI07);
            Lenoffset += (nLen+5);
            break;
        /* 拓扑识别 新增 end */

        default:
            Lenoffset = DataLen;
            break;
        }
    }

    //打印输出 交采与统计模块共享内存结构体
}


/*******************************************************************************
* 函数名称:	AmrRtDataSave
* 函数功能:	抄表实时数据存储
* 输入参数:	PrtlID              规约类型
*           DataLen             数据长度
*           DataBuff            数据内容
* 输出参数:	无
* 返 回 值: 无
*******************************************************************************/
void AmrRtDataSaveSingle(uint16 DataLen, uint8 *DataBuff)
{
    uint32	DI07;								 //07数据标识
    uint16	Lenoffset = 0;
    uint8	nLen = 0;

    Lenoffset = 0;

    if(DataLen < 5)//至少包含4字节数据标识+1字节内容
    {
        ACCOMM_FMT_DEBUG("AmrRtDataSaveSingle() DataLen=%d错误! 至少包含4字节数据标识+1字节内容\n", DataLen);
        return;
    }

    DI07 = *(uint32*)(&DataBuff[Lenoffset]);
    nLen = DataLen - 4;//减去4字节数据标识长度, 剩下内容为具体数据域
    switch(DI07)
    {
    /* 交采参数 新增 begin */
    case 0x04000103:             //最大需量周期
    case 0x04000104:             //滑差时间
        if(nLen > 1)
        {
            nLen = 1;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    case 0x04000106:			   //备用套时区表切换时间 分时日月年
    case 0x04000107:			   //备用套日时段切换时间 分时日月年
        if(nLen > 5)
        {
            nLen = 5;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    case 0x04000201:             //年时区数 ≤14
    case 0x04000202:             //日时段表数 ≤8
    case 0x04000203:             //日时段数 ≤14
    case 0x04000204:             //费率数 ≤63
    //case 0x04000205:             //公共假日数 ≤254 2字节
    case 0x04000601:			   //有功组合方式特征字
    case 0x04000602:			   //无功组合方式1特征字
    case 0x04000603:			   //无功组合方式2特征字
        if(nLen > 1)
        {
            nLen = 1;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    case 0x04000205:             //公共假日数 ≤254
        if(nLen > 2)
        {
            nLen = 2;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    case 0x04010000:             //当前套时区表(最多14个时区)
    case 0x04020000:             //备用套时区表(最多14个时区)
        if(nLen > 3*14)
        {
            nLen = 3*14;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    case 0x04010001:             //当前套日时段表(最多8日, 第1日时段时段表数据, 每日最多14时段)
    case 0x04010002:             //当前套日时段表(最多8日, 第2日时段时段表数据, 每日最多14时段)
    case 0x04010003:             //当前套日时段表(最多8日, 第3日时段时段表数据, 每日最多14时段)
    case 0x04010004:             //当前套日时段表(最多8日, 第4日时段时段表数据, 每日最多14时段)
    case 0x04010005:             //当前套日时段表(最多8日, 第5日时段时段表数据, 每日最多14时段)
    case 0x04010006:             //当前套日时段表(最多8日, 第6日时段时段表数据, 每日最多14时段)
    case 0x04010007:             //当前套日时段表(最多8日, 第7日时段时段表数据, 每日最多14时段)
    case 0x04010008:             //当前套日时段表(最多8日, 第8日时段时段表数据, 每日最多14时段)
    case 0x04020001:             //备用套日时段表(最多8日, 第1日时段时段表数据, 每日最多14时段)
    case 0x04020002:             //备用套日时段表(最多8日, 第2日时段时段表数据, 每日最多14时段)
    case 0x04020003:             //备用套日时段表(最多8日, 第3日时段时段表数据, 每日最多14时段)
    case 0x04020004:             //备用套日时段表(最多8日, 第4日时段时段表数据, 每日最多14时段)
    case 0x04020005:             //备用套日时段表(最多8日, 第5日时段时段表数据, 每日最多14时段)
    case 0x04020006:             //备用套日时段表(最多8日, 第6日时段时段表数据, 每日最多14时段)
    case 0x04020007:             //备用套日时段表(最多8日, 第7日时段时段表数据, 每日最多14时段)
    case 0x04020008:             //备用套日时段表(最多8日, 第8日时段时段表数据, 每日最多14时段)
        if(nLen > 3*14)
        {
            nLen = 3*14;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    case 0x04000503:             //当前交采模块的日时段表运行在哪一套
        if(nLen > 2)
        {
            nLen = 2;
        }
        SavePara(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;

    /* 交采参数 新增 end */

    /* 拓扑识别 新增 begin */
    case 0x04401004:             //(自定义扩展)拓扑识别结果(浙江)
        SaveTopo(nLen, &DataBuff[Lenoffset+4], DI07);
        Lenoffset += (nLen+4);
        break;
    /* 拓扑识别 新增 end */

    default:
        Lenoffset = DataLen;
        break;
    }
}


void Pro_AC_Amr_Response(uint8 *pRecvData)
{
    uint16	i;
    uint8	*pDataBuff = NULL;
    uint16	DataLen = 0;

    pDataBuff 	= 	&pRecvData[MTR_DATA_INDEX+1];
    DataLen	= pRecvData[LEN_INDEX] + (pRecvData[LEN_INDEX+1]<<8);

    //数据域减33处理

    for(i = 0; i < DataLen; i++)
    {
        pRecvData[MTR_DATA_INDEX + i + 1] -= 0x33;
    }
    AmrRtDataSave(DataLen, pDataBuff);
}

void Pro_AC_Amr_Response_Single(uint8 *pRecvData)
{
    uint16	i;
    uint8	*pDataBuff = NULL;
    uint16	DataLen = 0;

    pDataBuff 	= 	&pRecvData[MTR_DATA_INDEX];
    DataLen	= pRecvData[LEN_INDEX];

    //数据域减33处理

    for(i = 0; i < DataLen; i++)
    {
        pRecvData[MTR_DATA_INDEX + i] -= 0x33;
    }
    AmrRtDataSaveSingle(DataLen, pDataBuff);
}


/*******************************************************************************
* 函数名称: UpdateStatistics
* 函数功能: 更新统计数据
* 输入参数:
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void UpdateStatistics(uint8 type)
{
    ACCOMM_FMT_LOGCC("UpdateStatistics():...... \n");
    float pi = acos(-1.0);//弧度

#if TEST_VER_FLAG == 1
    {
        if(type == SAMPLE_TYPE_REALTIME)
        {
            if (gSaveFlag.bFirstSampRealtime == FALSE)
            {
                gSaveFlag.bFirstSampRealtime = TRUE;
                gSaveFlag.bFirstSampSpecial = TRUE;
            }
            
            if(1)//电压相关统计
            {
                //以A相电压为基准
                int32 UZeroX = 0;
                int32 UZeroY = 0;
                int32  PhaAVoltAngle = 0.1 * TermSharedData.PhaAVoltAngle;
                int32  PhaBVoltAngle = 0.1 * TermSharedData.PhaBVoltAngle;
                int32  PhaCVoltAngle = 0.1 * TermSharedData.PhaCVoltAngle;
                if(gPriMeasurePara.AngleDir.BitSect.PhaAVolAngFlag == 1)
                {
                    PhaAVoltAngle = 0 - PhaAVoltAngle;
                }
                
                if(gPriMeasurePara.AngleDir.BitSect.PhaBVolAngFlag == 1)
                {
                    PhaBVoltAngle = 0 - PhaBVoltAngle;
                }
                
                if(gPriMeasurePara.AngleDir.BitSect.PhaCVolAngFlag == 1)
                {
                    PhaCVoltAngle = 0 - PhaCVoltAngle;
                }

                //零序电压计算
                UZeroX = TermSharedData.PhaAVolt + TermSharedData.PhaBVolt*cos((PhaBVoltAngle-PhaAVoltAngle)*pi/180) + TermSharedData.PhaCVolt*cos((PhaCVoltAngle-PhaAVoltAngle)*pi/180);
                UZeroY = 0 + TermSharedData.PhaBVolt*sin((PhaBVoltAngle-PhaAVoltAngle)*pi/180) + TermSharedData.PhaCVolt*sin((PhaCVoltAngle-PhaAVoltAngle)*pi/180);
                TermSharedData.PhaZeroVolt = sqrt(UZeroX*UZeroX + UZeroY*UZeroY);
                /*
                ACCOMM_FMT_LOGCC("PhaAVolt=%d, PhaBVolt=%d, PhaCVolt=%d, PhaAVoltAngle=%d, PhaBVoltAngle=%d, PhaCVoltAngle=%d, UZeroX=%d, UZeroY=%d \n", 
                        TermSharedData.PhaAVolt, TermSharedData.PhaBVolt, TermSharedData.PhaCVolt, 
                        PhaAVoltAngle, PhaBVoltAngle, PhaCVoltAngle, UZeroX, UZeroY);
                */
                ACCOMM_FMT_LOGCC("零序电压TermSharedData.PhaZeroVolt: %d \n", TermSharedData.PhaZeroVolt);

                //正序电压
                UZeroX = TermSharedData.PhaAVolt + TermSharedData.PhaBVolt*cos((PhaBVoltAngle-PhaAVoltAngle-120)*pi/180) + TermSharedData.PhaCVolt*cos((PhaCVoltAngle-PhaAVoltAngle+120)*pi/180);
                UZeroY = 0 + TermSharedData.PhaBVolt*sin((PhaBVoltAngle-PhaAVoltAngle-120)*pi/180) + TermSharedData.PhaCVolt*sin((PhaCVoltAngle-PhaAVoltAngle+120)*pi/180);
                TermSharedData.PhaPosVolt = sqrt(UZeroX*UZeroX + UZeroY*UZeroY);
                /*
                ACCOMM_FMT_LOGCC("PhaAVolt=%d, PhaBVolt=%d, PhaCVolt=%d, PhaAVoltAngle=%d, PhaBVoltAngle=%d, PhaCVoltAngle=%d, UZeroX=%d, UZeroY=%d \n", 
                        TermSharedData.PhaAVolt, TermSharedData.PhaBVolt, TermSharedData.PhaCVolt, 
                        PhaAVoltAngle, PhaBVoltAngle, PhaCVoltAngle, UZeroX, UZeroY);
                */
                ACCOMM_FMT_LOGCC("正序电压TermSharedData.PhaPosVolt: %d \n", TermSharedData.PhaPosVolt);

                //负序电压
                UZeroX = TermSharedData.PhaAVolt + TermSharedData.PhaBVolt*cos((PhaBVoltAngle-PhaAVoltAngle+120)*pi/180) + TermSharedData.PhaCVolt*cos((PhaCVoltAngle-PhaAVoltAngle-120)*pi/180);
                UZeroY = 0 + TermSharedData.PhaBVolt*sin((PhaBVoltAngle-PhaAVoltAngle+120)*pi/180) + TermSharedData.PhaCVolt*sin((PhaCVoltAngle-PhaAVoltAngle-120)*pi/180);
                TermSharedData.PhaNegVolt = sqrt(UZeroX*UZeroX + UZeroY*UZeroY);
                /*
                ACCOMM_FMT_LOGCC("PhaAVolt=%d, PhaBVolt=%d, PhaCVolt=%d, PhaAVoltAngle=%d, PhaBVoltAngle=%d, PhaCVoltAngle=%d, UZeroX=%d, UZeroY=%d \n", 
                        TermSharedData.PhaAVolt, TermSharedData.PhaBVolt, TermSharedData.PhaCVolt, 
                        PhaAVoltAngle, PhaBVoltAngle, PhaCVoltAngle, UZeroX, UZeroY);
                */
                ACCOMM_FMT_LOGCC("负序电压TermSharedData.PhaNegVolt: %d \n", TermSharedData.PhaNegVolt);
                
                //电压不平衡率
                if(TermSharedData.PhaPosVolt != 0)
                {
                    TermSharedData.VoltUnbalanceRate = 100 * 100 * TermSharedData.PhaNegVolt/(TermSharedData.PhaPosVolt + 0.0);
                }
                ACCOMM_FMT_LOGCC("电压不平衡率TermSharedData.VoltUnbalanceRate: %d \n", TermSharedData.VoltUnbalanceRate);
                
            }
            
            if(1)//电流相关统计
            {
                //以A相电流为基准
                int32 IZeroX = 0;
                int32 IZeroY = 0;
                int32  PhaACurrAngle = 0.1 * TermSharedData.PhaACurrAngle;
                int32  PhaBCurrAngle = 0.1 * TermSharedData.PhaBCurrAngle;
                int32  PhaCCurrAngle = 0.1 * TermSharedData.PhaCCurrAngle;
                if(gPriMeasurePara.AngleDir.BitSect.PhaACurrAngFlag == 1)
                {
                    PhaACurrAngle = 0 - PhaACurrAngle;
                }
                
                if(gPriMeasurePara.AngleDir.BitSect.PhaBCurrAngFlag == 1)
                {
                    PhaBCurrAngle = 0 - PhaBCurrAngle;
                }
                
                if(gPriMeasurePara.AngleDir.BitSect.PhaCCurrAngFlag == 1)
                {
                    PhaCCurrAngle = 0 - PhaCCurrAngle;
                }

                //零序电流计算
                /*
                //零序电流已从交采直接获取, 不计算
                IZeroX = TermSharedData.PhaACurr + TermSharedData.PhaBCurr*cos((PhaBCurrAngle-PhaACurrAngle)*pi/180) + TermSharedData.PhaCCurr*cos((PhaCCurrAngle-PhaACurrAngle)*pi/180);
                IZeroY = 0 + TermSharedData.PhaBCurr*sin((PhaBCurrAngle-PhaACurrAngle)*pi/180) + TermSharedData.PhaCCurr*sin((PhaCCurrAngle-PhaACurrAngle)*pi/180);
                TermSharedData.PhaMCurr = sqrt(IZeroX*IZeroX + IZeroY*IZeroY);
                ACCOMM_FMT_LOGCC("零序电流TermSharedData.PhaZeroCurr: %d \n", TermSharedData.PhaMCurr);
                */
                
                //正序电流
                IZeroX = TermSharedData.PhaACurr + TermSharedData.PhaBCurr*cos((PhaBCurrAngle-PhaACurrAngle-120)*pi/180) + TermSharedData.PhaCCurr*cos((PhaCCurrAngle-PhaACurrAngle+120)*pi/180);
                IZeroY = 0 + TermSharedData.PhaBCurr*sin((PhaBCurrAngle-PhaACurrAngle-120)*pi/180) + TermSharedData.PhaCCurr*sin((PhaCCurrAngle-PhaACurrAngle+120)*pi/180);
                TermSharedData.PhaPosCurr = sqrt(IZeroX*IZeroX + IZeroY*IZeroY);
                ACCOMM_FMT_LOGCC("正序电流TermSharedData.PhaPosCurr: %d \n", TermSharedData.PhaPosCurr);

                //负序电流
                IZeroX = TermSharedData.PhaACurr + TermSharedData.PhaBCurr*cos((PhaBCurrAngle-PhaACurrAngle+120)*pi/180) + TermSharedData.PhaCCurr*cos((PhaCCurrAngle-PhaACurrAngle-120)*pi/180);
                IZeroY = 0 + TermSharedData.PhaBCurr*sin((PhaBCurrAngle-PhaACurrAngle+120)*pi/180) + TermSharedData.PhaCCurr*sin((PhaCCurrAngle-PhaACurrAngle-120)*pi/180);
                TermSharedData.PhaNegCurr = sqrt(IZeroX*IZeroX + IZeroY*IZeroY);
                ACCOMM_FMT_LOGCC("负序电流TermSharedData.PhaNegCurr: %d \n", TermSharedData.PhaNegCurr);
                
                //电流不平衡率
                if(TermSharedData.PhaPosCurr != 0)
                {
                    TermSharedData.CurrUnbalanceRate = 100 * 100 * TermSharedData.PhaNegCurr/(TermSharedData.PhaPosCurr + 0.0);
                }
                ACCOMM_FMT_LOGCC("电流不平衡率TermSharedData.CurrUnbalanceRate: %d \n", TermSharedData.CurrUnbalanceRate);
                
            }
            
        }
        else if(type == SAMPLE_TYPE_DEMAND)
        {
            if (gSaveFlag.bFirstSampDemand == FALSE)
            {
                gSaveFlag.bFirstSampDemand = TRUE;
            }
        }
        else if(type == SAMPLE_TYPE_ENERGY)
        {
            if (gSaveFlag.bFirstSampEnergy == FALSE)
            {
                gSaveFlag.bFirstSampEnergy = TRUE;
            }
        }
        else if(type == SAMPLE_TYPE_HARMONIC)
        {
            if (gSaveFlag.bFirstSampHarmonic == FALSE)
            {
                gSaveFlag.bFirstSampHarmonic = TRUE;
            }
        }
    }
#else
    {
        if(type == SAMPLE_TYPE_REALTIME)
        {
            //零序电压计算
            uint16 UZeroX = 0;
            uint16 UZeroY = 0;
            int32  PhaBVoltAngle = 0.1 * TermSharedData.PhaBVoltAngle;
            int32  PhaCVoltAngle = 0.1 * TermSharedData.PhaCVoltAngle;
            if(gPriMeasurePara.AngleDir.BitSect.PhaBVolAngFlag == 1)
            {
                PhaBVoltAngle = 0 - PhaBVoltAngle;
            }
            
            if(gPriMeasurePara.AngleDir.BitSect.PhaCVolAngFlag == 1)
            {
                PhaCVoltAngle = 0 - PhaCVoltAngle;
            }
            
            UZeroX = TermSharedData.PhaAVolt + TermSharedData.PhaBVolt*cos(PhaBVoltAngle*pi/180) + TermSharedData.PhaCVolt*cos(PhaCVoltAngle*pi/180);
            UZeroY = 0 + TermSharedData.PhaBVolt*sin(PhaBVoltAngle*pi/180) + TermSharedData.PhaCVolt*sin(PhaCVoltAngle*pi/180);
            TermSharedData.PhaZeroVolt = sqrt(UZeroX*UZeroX + UZeroY*UZeroY);
            ACCOMM_FMT_LOGCC("TermSharedData.PhaZeroVolt: %d \n", TermSharedData.PhaZeroVolt);
            
            //电压不平衡率
            uint16 MinVolt = TermSharedData.PhaAVolt;
            uint16 MaxVolt = TermSharedData.PhaAVolt;
            MinVolt = (TermSharedData.PhaBVolt < MinVolt) ? TermSharedData.PhaBVolt : MinVolt;
            MinVolt = (TermSharedData.PhaCVolt < MinVolt) ? TermSharedData.PhaCVolt : MinVolt;
            MaxVolt = (TermSharedData.PhaBVolt > MaxVolt) ? TermSharedData.PhaBVolt : MaxVolt;
            MaxVolt = (TermSharedData.PhaCVolt > MaxVolt) ? TermSharedData.PhaCVolt : MaxVolt;
            if(MaxVolt > 0)
            {
                TermSharedData.VoltUnbalanceRate = 100 * MinVolt/(MaxVolt + 0.0);
                ACCOMM_FMT_LOGCC("TermSharedData.VoltUnbalanceRate: %d \n", TermSharedData.VoltUnbalanceRate);
            }
            

            //电流不平衡率
            uint16 MinCurr = TermSharedData.PhaACurr;
            uint16 MaxCurr = TermSharedData.PhaACurr;
            MinCurr = (TermSharedData.PhaBCurr < MinCurr) ? TermSharedData.PhaBCurr : MinCurr;
            MinCurr = (TermSharedData.PhaCCurr < MinCurr) ? TermSharedData.PhaCCurr : MinCurr;
            MaxCurr = (TermSharedData.PhaBCurr > MaxCurr) ? TermSharedData.PhaBCurr : MaxCurr;
            MaxCurr = (TermSharedData.PhaCCurr > MaxCurr) ? TermSharedData.PhaCCurr : MaxCurr;
            if(MaxCurr > 0)
            {
                TermSharedData.CurrUnbalanceRate = 100 * MinCurr/(MaxCurr + 0.0);
                ACCOMM_FMT_LOGCC("TermSharedData.CurrUnbalanceRate: %d \n", TermSharedData.CurrUnbalanceRate);
            }
            
        }
        else if(type == SAMPLE_TYPE_DEMAND)
        {
            
        }
        else if(type == SAMPLE_TYPE_ENERGY)
        {
            
        }
        else if(type == SAMPLE_TYPE_HARMONIC)
        {
            
        }
    }
#endif
}

/*********************************************************************
    所属功能单元：
    功能描述：
    输入参数：isUpdateAC 是否需要升级 isGetVer是否需要招版本号
    输出参数：
        无
    函数返回值：
                   > 0: 解析数据的长度		, < 0: 错误信息
    内部处理流程：
********************************************************************/
void ReadACSample(BOOL isUpdateAC, BOOL isGetVer, uint8 inType)
{
    RS232_COMM_PARA commPara;
    int32 recvLen = 0;
    int32 sendLen = 0;
    int32 bRet = 0;
    uint8  recvBuf[2000];
    uint8  sendBuf[1000];
    uint8  recvBufTemp[2000];
    static READ_AC_STATE state;
    uint8 *ptr = NULL;  //应答数据帧起始指针(0x68)
    uint8 *pstart = NULL;  //应答数据帧起始指针(首地址,可能并非0x68,用于多帧解帧)
    int32  validlen = 0;  //一帧完整报文长度
    int32  leftlen = 0;  //报文剩余长度
    static AC_BAUDRATE eBaudRate = BAUDRATE115200;  //默认波特率
    uint8  nRecvNullMaxCnt = 10;  //最大连续接收为空的次数
    static uint8  nRecvNullCnt = 0;  //连续接收为空的次数
    BOOL   bAcDateAndTimeSet = FALSE;
    BOOL   bAcDateSet = FALSE;
    BOOL   bAcTimeSet = FALSE;
    BOOL   bDataInit = FALSE;
    BOOL   bParamInit = FALSE;
    BOOL   bDemandInit = FALSE;
    BOOL   bRelay485 = FALSE;
    BOOL   bRelayUDP = FALSE;
    BOOL   bDemandPeriodSet = FALSE;//最大需量周期设置
    BOOL   bSlipTimeSet = FALSE;//滑差时间设置
    BOOL   bZoneNumSet = FALSE;//时区数设置
    BOOL   bPeriodNumSet = FALSE;//日时段数设置
    BOOL   bZoneTableSet = FALSE;//时区表设置
    BOOL   bDayPeriodSet = FALSE;//日时段表设置
    BOOL   bParaSet = FALSE;//参数设置
    BOOL   bParaQuery = FALSE;//参数查询
    uint8  pipe = 0;  //485串口通道
    int    initRet = 0;
    int    i = 0;
    int    CheckRetSum = 0;

    //防止非周期采集数据时，UpdateStatistics将缓存数据刷新成全0
    uint8  type = SAMPLE_TYPE_NULL;

    if (isUpdateAC)
    {
        bRet = JcUpdateDeal(fd_ac, FALSE);
        if (bRet == -2)
        {
            state = INIT_AC_STATE;
        }
        else
        {
            return;
        }
    }

    if (isGetVer)
    {
        bRet = JcUpdateDeal(fd_ac, TRUE);
        if (bRet == -2)
        {
            state = INIT_AC_STATE;
        }
        else if(bRet == -3)
        {
            //已经达到重试次数上限, 不再召唤版本
        }
        else
        {
            return;
        }
    }

    memset(recvBuf, 0x00, sizeof(recvBuf));
    memset(sendBuf, 0x00, sizeof(sendBuf));

    switch (state)
    {
    case INIT_AC_STATE:
        memset((void *)&commPara, 0, sizeof(RS232_COMM_PARA));

        //9600波特率
        if(eBaudRate == BAUDRATE9600)
        {
            commPara.dwBaudRate = 9600;
            commPara.byByteSize = 8;
            commPara.byParity = 'e';
            commPara.byStopBits = 1;
        }
        else if(eBaudRate == BAUDRATE115200)
        {
            //115200波特率
            commPara.dwBaudRate = 115200;
            commPara.byByteSize = 8;
            commPara.byParity = 'e';
            commPara.byStopBits = 1;
        }
        else
        {
            ACCOMM_FMT_TRACE("ReadACSample() INIT_AC_STATE...波特率类型错误! \n");
            sleep(2);
            break;
        }

        sprintf((char *)commPara.sDeviceName, gUartConf.dev);

        if (fd_ac < 0)
        {
            ACCOMM_FMT_TRACE("if (fd_ac < 0)...111...JCcommPara.sDeviceName=%s, dwBaudRate=%d, byByteSize:%d, byParity:%c, byStopBits:%d \n", commPara.sDeviceName, 
                commPara.dwBaudRate, commPara.byByteSize, commPara.byParity, commPara.byStopBits);
            fd_ac = commdrv_Init_232(&commPara);   /*打开串口*/
        }
        if (fd_ac < 0)
        {
            ACCOMM_FMT_TRACE("if (fd_ac < 0)...222...JCcommPara.sDeviceName=%s, dwBaudRate=%d, byByteSize:%d, byParity:%c, byStopBits:%d \n", commPara.sDeviceName, 
                commPara.dwBaudRate, commPara.byByteSize, commPara.byParity, commPara.byStopBits);
            state = INIT_AC_STATE;
            sleep(2);
            break;
        }
        ACCOMM_FMT_TRACE("JCcommPara.sDeviceName = %s, fd_ac = %d\n", commPara.sDeviceName, fd_ac);

        //波特率切换之后，召唤一下版本信息，防止前面召唤失败
        if(gBaudRateChangedFlag)
        {
            gBaudRateChangedFlag = 0;
            gCallVerAfterBaudRateChanged = 1;
            ACCOMM_FMT_TRACE("ReadACSample() INIT_AC_STATE...波特率切换之后，召唤一下版本信息，防止前面召唤失败 \n");
        }
        
        state = READ_AC_SEND_STATE;
        break;

        // 发送抄读交采的帧
    case READ_AC_SEND_STATE:
        //ACCOMM_FMT_TRACE("ReadACSample() READ_AC_SEND_STATE...type=%d\n", type);
        //先清缓存
        sendLen = 0;
        if ((1 == g_nAcSetDateFlag) && 
            (1 == g_nAcSetTimeFlag) &&
            (!TermSharedData.DateSetFlag) &&
            (!TermSharedData.TimeSetFlag))
        {
            //同时对交采模块日期和时间
            g_nAcSetDateFlag = 0;
            g_nAcSetTimeFlag = 0;
            bAcDateAndTimeSet = TRUE;

            sendLen = make_frame_ac_date_time(sendBuf);

            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置日期和时间");
        }
        else if (1 == g_nAcSetDateFlag)//判断交采模块是否需要校时
        {
            //交采模块校时(日期)
            g_nAcSetDateFlag = 0;
            bAcDateSet = TRUE;
            if(TRUE == TermSharedData.DateSetFlag)
            {
                sendLen = make_frame_master_date(sendBuf);
                TermSharedData.DateSetFlag = FALSE;
            }
            else
            {
                sendLen = make_frame_ac_date(sendBuf);
            }
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置日期");
        }
        else if (1 == g_nAcSetTimeFlag)//判断交采模块是否需要校时
        {
            //交采模块校时(时间)
            g_nAcSetTimeFlag = 0;
            bAcTimeSet = TRUE;
            if(TRUE == TermSharedData.TimeSetFlag)
            {
                sendLen = make_frame_master_time(sendBuf);
                TermSharedData.TimeSetFlag = FALSE;
            }
            else
            {
                sendLen = make_frame_ac_time(sendBuf);
            }
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置时间");
        }
        else if(1 == g_nAcDataInitFlag)//判断是否需要参数初始化或数据初始化
        {
            //数据初始化
            g_nAcDataInitFlag = 0;
            bDataInit = TRUE;
            sendLen = make_frame_init_data(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "数据初始化");
        }
        else if(1 == g_nAcParamInitFlag)
        {
            //参数初始化
            g_nAcParamInitFlag = 0;
            bParamInit = TRUE;
            sendLen = make_frame_init_param(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "参数初始化");
        }
        else if(1 == g_nAcDemandInitFlag)
        {
            //需量初始化
            g_nAcDemandInitFlag = 0;
            bDemandInit = TRUE;
            sendLen = make_frame_init_demand(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "需量初始化");
        }
        else if(TermSharedData.MaxDemandPeriodSetFlag == TRUE)
        {
            //设置最大需量周期
            TermSharedData.MaxDemandPeriodSetFlag = FALSE;
            bDemandPeriodSet = TRUE;
            sendLen = make_frame_set_demand_period(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置最大需量周期");
            g_nAcReadDemandPeriodFlag = 1;//设置之后需要查询一次
        }
        else if(1 == g_nAcReadDemandPeriodFlag)
        {
            //查询最大需量周期
            g_nAcReadDemandPeriodFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000103);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询最大需量周期");
        }
        else if(TermSharedData.SlipDiffTimeSetFlag == TRUE)
        {
            //设置滑差时间
            TermSharedData.SlipDiffTimeSetFlag = FALSE;
            bSlipTimeSet = TRUE;
            sendLen = make_frame_set_slip_time(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置滑差时间");
            g_nAcReadSlipTimeFlag = 1;//设置之后需要查询一次
        }
        else if(1 == g_nAcReadSlipTimeFlag)
        {
            //查询滑差时间
            g_nAcReadSlipTimeFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000104);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询滑差时间");
        }
        else if(TermSharedData.AddiZoneChangeTimeSetFlag == TRUE)
        {
            //设置备用套时区表切换时间
            TermSharedData.AddiZoneChangeTimeSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_zone_table_change_time(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置备用套时区表切换时间");
            g_nAddiZoneChangeTimeFlag = 1;//设置之后需要查询一次
        }
        else if(1 == g_nAddiZoneChangeTimeFlag)
        {
            //查询备用套时区表切换时间
            g_nAddiZoneChangeTimeFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000106);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询备用套时区表切换时间");
        }
        else if(TermSharedData.AddiPeriChangeTimeSetFlag == TRUE)
        {
            //设置备用套日时段表切换时间
            TermSharedData.AddiPeriChangeTimeSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_peri_table_change_time(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置备用套日时段表切换时间");
            g_nAddiPeriChangeTimeFlag = 1;//设置之后需要查询一次
        }
        else if(1 == g_nAddiPeriChangeTimeFlag)
        {
            //查询备用套日时段表切换时间
            g_nAddiPeriChangeTimeFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000107);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询备用套日时段表切换时间");
        }
        else if(TermSharedData.ActPowCombWordSetFlag == TRUE)
        {
            //设置有功组合方式特征字
            TermSharedData.ActPowCombWordSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_set_p_q_word(sendBuf, 0x04000601, TermSharedData.ActPowCombWordSet.nValue);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置有功组合方式特征字=%d", TermSharedData.ActPowCombWordSet.nValue);
            g_nActPowCombWordFlag = 1;//设置之后需要查询一次
        }
        else if(1 == g_nActPowCombWordFlag)
        {
            //查询有功组合方式特征字
            g_nActPowCombWordFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000601);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询有功组合方式特征字");
        }
        else if(TermSharedData.ReaPowComb1WordSetFlag == TRUE)
        {
            //设置无功组合方式1特征字
            TermSharedData.ReaPowComb1WordSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_set_p_q_word(sendBuf, 0x04000602, TermSharedData.ReaPowComb1WordSet.nValue);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置无功组合方式1特征字=%d", TermSharedData.ReaPowComb1WordSet.nValue);
            g_nReaPowComb1WordFlag = 1;//设置之后需要查询一次
        }
        else if(1 == g_nReaPowComb1WordFlag)
        {
            //查询无功组合方式1特征字
            g_nReaPowComb1WordFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000602);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询无功组合方式1特征字");
        }
        else if(TermSharedData.ReaPowComb2WordSetFlag == TRUE)
        {
            //设置无功组合方式2特征字
            TermSharedData.ReaPowComb2WordSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_set_p_q_word(sendBuf, 0x04000603, TermSharedData.ReaPowComb2WordSet.nValue);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置无功组合方式2特征字=%d", TermSharedData.ReaPowComb2WordSet.nValue);
            g_nReaPowComb2WordFlag = 1;//设置之后需要查询一次
        }
        else if(1 == g_nReaPowComb2WordFlag)
        {
            //查询无功组合方式2特征字
            g_nReaPowComb2WordFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000603);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询无功组合方式2特征字");
        }
        else if(TermSharedData.ZoneTableNumSetFlag == TRUE)
        {
            //设置时区数量
            TermSharedData.ZoneTableNumSetFlag = FALSE;
            bZoneNumSet = TRUE;
            sendLen = make_frame_set_zone_peri_num(sendBuf, 0x04000201, TermSharedData.ZonePeriNumSet.yPeriod);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置时区数量=%d", TermSharedData.ZonePeriNumSet.yPeriod);
            g_nAcReadZoneNumFlag = 1;//设置之后需要查询一次
        }
        else if(1 == g_nAcReadZoneNumFlag)
        {
            //查询时区数量
            g_nAcReadZoneNumFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000201);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询时区数量");
        }
        else if(TermSharedData.DayPeriTableNumSetFlag == TRUE)
        {
            //设置日时段表数量
            TermSharedData.DayPeriTableNumSetFlag = FALSE;
            bPeriodNumSet = TRUE;
            sendLen = make_frame_set_zone_peri_num(sendBuf, 0x04000202, TermSharedData.ZonePeriNumSet.dPeriodMet);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置日时段表数量=%d", TermSharedData.ZonePeriNumSet.dPeriodMet);
            g_nAcReadDayTableNumFlag = 1;//设置之后需要查询一次
        }
        else if(1 == g_nAcReadDayTableNumFlag)
        {
            //查询日时段表数量
            g_nAcReadDayTableNumFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000202);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询日时段表数量");
        }
        else if(TermSharedData.DayPeriNumSetFlag == TRUE)
        {
            //设置时段数量
            TermSharedData.DayPeriNumSetFlag = FALSE;
            bPeriodNumSet = TRUE;
            sendLen = make_frame_set_zone_peri_num(sendBuf, 0x04000203, TermSharedData.ZonePeriNumSet.dPeriod);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置日时段数量=%d", TermSharedData.ZonePeriNumSet.dPeriod);
            g_nAcReadDayPeriNumFlag = 1;//设置之后需要查询一次
        }
        else if(1 == g_nAcReadDayPeriNumFlag)
        {
            //查询时段数量
            g_nAcReadDayPeriNumFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000203);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询日时段数量");
        }
        else if(TermSharedData.CostRateNumSetFlag == TRUE)
        {
            //设置费率数量
            TermSharedData.CostRateNumSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_set_zone_peri_num(sendBuf, 0x04000204, TermSharedData.ZonePeriNumSet.rate);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置费率数量=%d", TermSharedData.ZonePeriNumSet.rate);
            g_nAcReadCostRateNumFlag = 1;//设置之后需要查询一次
        }
        else if(1 == g_nAcReadCostRateNumFlag)
        {
            //查询费率数量
            g_nAcReadCostRateNumFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000204);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询费率数量");
        }
        else if(TermSharedData.HolidayNumSetFlag == TRUE)
        {
            //设置公共假日数量
            TermSharedData.HolidayNumSetFlag = FALSE;
            bParaSet = TRUE;
            sendLen = make_frame_set_holiday_num(sendBuf, 0x04000205, TermSharedData.ZonePeriNumSet.pubHoliday);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置公共假日数量=%d", TermSharedData.ZonePeriNumSet.pubHoliday);
            g_nAcReadHolidayNumFlag = 1;//设置之后需要查询一次
        }
        else if(1 == g_nAcReadHolidayNumFlag)
        {
            //查询公共假日数量
            g_nAcReadHolidayNumFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000205);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询公共假日数量");
        }
        else if(TermSharedData.CurrZoneTableSetFlag == TRUE)
        {
            //设置当前套时区表
            TermSharedData.CurrZoneTableSetFlag = FALSE;
            bZoneTableSet = TRUE;
            sendLen = make_frame_set_curr_zone(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置当前套时区表");
            g_nAcReadCurrZoneFlag = 1;//设置之后需要查询一次
        }
        else if(TermSharedData.AddiZoneTableSetFlag == TRUE)
        {
            //设置备用套时区表
            TermSharedData.AddiZoneTableSetFlag = FALSE;
            bZoneTableSet = TRUE;
            sendLen = make_frame_set_addi_zone(sendBuf);
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置备用套时区表");
            g_nAcReadAddiZoneFlag = 1;//设置之后需要查询一次
        }
        else if(1 == g_nAcReadCurrZoneFlag)
        {
            //查询当前套时区表
            g_nAcReadCurrZoneFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04010000);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询当前套时区表");
        }
        else if(1 == g_nAcReadAddiZoneFlag)
        {
            //查询备用套时区表
            g_nAcReadAddiZoneFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04020000);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询备用套时区表");
        }
        else if(1 == g_nReadCurrDayPeriLabelFlag)
        {
            //查询交采模块当前的日时段表是哪一套
            g_nReadCurrDayPeriLabelFlag = 0;
            sendLen = make_frame_query_para(sendBuf, 0x04000503);
            bParaQuery = TRUE;
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询交采模块当前的日时段表是哪一套");
        }
        else if(TermSharedData.CurrDayPeriTableSetFlag == TRUE)
        {
            BOOL bNeedSet = FALSE;
            uint8 nNeedSetIndex = 0;
            for(i=0; i<TermSharedData.CurrDayPeriTableSet.nNum; i++)
            {
                if(i == OOP_MAX_DAYPERIODS)
                    break;
                if(1 == TermSharedData.CurrDayPeriSetFlagArr[i])
                {
                    TermSharedData.CurrDayPeriSetFlagArr[i] = 0;
                    nNeedSetIndex = i;
                    bNeedSet = TRUE;
                    break;
                }
            }
            if(bNeedSet)
            {
                //设置交采模块的第一套日时段表
                bZoneTableSet = TRUE;
                sendLen = make_frame_set_curr_day_period(sendBuf, nNeedSetIndex);
                ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置当前套日时段表nNeedSetIndex=%d", nNeedSetIndex);
                g_nAcReadCurrDayPeriFlag = 1;//设置之后需要查询一次
                g_nAcReadCurrDayFlagArr[nNeedSetIndex] = 1;
            }
            else
            {
                //不需要设置当前套日时段表
                TermSharedData.CurrDayPeriTableSetFlag = FALSE;
                state = READ_AC_SEND_STATE;        // 返回发送
                ACCOMM_FMT_TRACE("ReadACSample() READ_AC_SEND_STATE...CurrZoneTableSetFlag = FALSE\n");
                break;
            }
            
        }
        else if(TermSharedData.AddiDayPeriTableSetFlag == TRUE)
        {
            BOOL bNeedSet = FALSE;
            uint8 nNeedSetIndex = 0;
            for(i=0; i<TermSharedData.AddiDayPeriTableSet.nNum; i++)
            {
                if(i == OOP_MAX_DAYPERIODS)
                    break;
                if(1 == TermSharedData.AddiDayPeriSetFlagArr[i])
                {
                    TermSharedData.AddiDayPeriSetFlagArr[i] = 0;
                    nNeedSetIndex = i;
                    bNeedSet = TRUE;
                    break;
                }
            }
            if(bNeedSet)
            {
                //设置交采模块的第二套日时段表
                bZoneTableSet = TRUE;
                sendLen = make_frame_set_addi_day_period(sendBuf, nNeedSetIndex);
                ACCOMM_BUF_DEBUG(sendBuf, sendLen, "设置备用套日时段表nNeedSetIndex=%d", nNeedSetIndex);
                g_nAcReadAddiDayPeriFlag = 1;//设置之后需要查询一次
                g_nAcReadAddiDayFlagArr[nNeedSetIndex] = 1;
            }
            else
            {
                //不需要设置备用套日时段表
                TermSharedData.AddiDayPeriTableSetFlag = FALSE;
                state = READ_AC_SEND_STATE;        // 返回发送
                ACCOMM_FMT_TRACE("ReadACSample() READ_AC_SEND_STATE...AddiZoneTableSetFlag = FALSE\n");
                break;
            }
            
        }
        else if(1 == g_nAcReadCurrDayPeriFlag)
        {
            //查询交采模块的第一套日时段表
            BOOL bNeedQuery = FALSE;
            for(i=0; i<TermSharedData.CurrDayPeriTableRD.nNum; i++)
            {
                if(1 == g_nAcReadCurrDayFlagArr[i])
                {
                    g_nAcReadCurrDayFlagArr[i] = 0;
                    sendLen = make_frame_query_para(sendBuf, 0x04010001 + i);
                    bNeedQuery = TRUE;
                    bParaQuery = TRUE;
                    ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询当前套日时段表 i=%d", i);
                    break;
                }
            }
            if(!bNeedQuery)
            {
                g_nAcReadCurrDayPeriFlag = 0;
                state = READ_AC_SEND_STATE;        // 返回发送
                ACCOMM_FMT_TRACE("ReadACSample() READ_AC_SEND_STATE...g_nAcReadCurrDayPeriFlag = FALSE\n");
                break;
            }
            
        }
        else if(1 == g_nAcReadAddiDayPeriFlag)
        {
            //查询交采模块的第二套日时段表
            BOOL bNeedQuery = FALSE;
            for(i=0; i<TermSharedData.AddiDayPeriTableRD.nNum; i++)
            {
                if(1 == g_nAcReadAddiDayFlagArr[i])
                {
                    g_nAcReadAddiDayFlagArr[i] = 0;
                    sendLen = make_frame_query_para(sendBuf, 0x04020001 + i);
                    bNeedQuery = TRUE;
                    bParaQuery = TRUE;
                    ACCOMM_BUF_DEBUG(sendBuf, sendLen, "查询备用套日时段表 i=%d", i);
                    break;
                }
            }
            if(!bNeedQuery)
            {
                g_nAcReadAddiDayPeriFlag = 0;
                state = READ_AC_SEND_STATE;        // 返回发送
                ACCOMM_FMT_TRACE("ReadACSample() READ_AC_SEND_STATE...g_nAcReadAddiDayPeriFlag = FALSE\n");
                break;
            }
            
        }
        else
        {
            //判断是否有确认帧需要下发
            sendLen = PlcReadQueue(pConfirmDown, sendBuf);
            if(sendLen > 0)
            {
                ACCOMM_BUF_DEBUG(sendBuf, sendLen, "优先处理确认应答帧");
            }
            else
            {
                //优先判断UDP是否有内容
                sendLen = PlcReadQueue(pUdpRecvDown, sendBuf);
                if(sendLen > 0)
                {
                    bRelayUDP = TRUE;//优先处理UDP报文
                    ACCOMM_BUF_DEBUG(sendBuf, sendLen, "处理UDP转发报文");
                }
                else
                {
                    sendLen = PlcReadQueue(pDealRecvDown[pipe], sendBuf);
                    if (sendLen > 0)
                    {
                        bRelay485 = TRUE;//优先处理485串口报文
                        ACCOMM_BUF_DEBUG(sendBuf, sendLen, "优先处理485转发报文");
                    }
                    else
                    {
                        //防止非周期采集数据时，UpdateStatistics将缓存数据刷新成全0
                        type = inType;
                        
                        if(type == SAMPLE_TYPE_ENERGY)
                        {
                            sendLen = MakeFrameReadEnergy(sendBuf);
                            ACCOMM_FMT_LOGCC("ReadACSample() READ_AC_SEND_STATE...采集电量数据\n");
                        }
                        else if(type == SAMPLE_TYPE_DEMAND)
                        {
                            sendLen = MakeFrameReadDemand(sendBuf);
                            ACCOMM_FMT_LOGCC("ReadACSample() READ_AC_SEND_STATE...采集需量数据\n");
                        }
                        else if(type == SAMPLE_TYPE_REALTIME)
                        {
                            sendLen = MakeFrameReadRealtime(sendBuf);
                            ACCOMM_FMT_LOGCC("ReadACSample() READ_AC_SEND_STATE...采集实时数据\n");
                        }
                        else if(type == SAMPLE_TYPE_HARMONIC)
                        {
                            sendLen = MakeFrameReadHarmonic(sendBuf);
                            ACCOMM_FMT_LOGCC("ReadACSample() READ_AC_SEND_STATE...采集谐波数据\n");
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
        }
        
        if (sendBuf > 0)
        {
            commdrv_Send_232(fd_ac, sendBuf, sendLen);
        }
        state = READ_AC_RECV_STATE;      // 状态不break往下顺序执行

        ACCOMM_BUF_LOGCC(sendBuf, sendLen, " 发送报文: ");//默认不打印

        if(bDataInit == TRUE)//数据初始化
        {
            sleep(1);
        }
        else if(bParamInit == TRUE)//参数初始化
        {
            sleep(1);
        }
        else if(bDemandInit == TRUE)//需量初始化
        {
            sleep(1);
        }
        else if(bRelay485 == TRUE)//透传485报文
        {
            usleep(500000);
        }
        else if(bParaQuery == TRUE)//参数查询
        {
            bParaQuery = FALSE;
            usleep(20000);
        }
        else if(bAcDateSet || bAcTimeSet || bDemandPeriodSet || bSlipTimeSet || 
            bZoneNumSet || bPeriodNumSet || bZoneTableSet || bDayPeriodSet || 
            bParaSet)//参数设置
        {
            usleep(10000);
        }
        else//实时数据查询
        {
            usleep(200000);
        }
        
        // 处理抄读交采回码帧
    case READ_AC_RECV_STATE:
        recvLen = commdrv_Recv_232(fd_ac, recvBuf, sizeof(recvBuf), 200, 5, 0xFF);
        if (recvLen <= 0)
        {
            ACCOMM_BUF_DEBUG(sendBuf, sendLen, "接收数据为空 recvLen[%d] 发送的报文:", recvLen);
            nRecvNullCnt ++;
            if(nRecvNullCnt >= nRecvNullMaxCnt)
            {
                ACCOMM_FMT_TRACE("ReadACSample() READ_AC_RECV_STATE...连续接收为空次数nRecvNullCnt=%d, 最大允许次数nRecvNullMaxCnt=%d, 当前波特率类别eBaudRate=%d (%d:BAUDRATE9600, %d:BAUDRATE115200) \n", 
                    nRecvNullCnt, nRecvNullMaxCnt, eBaudRate, BAUDRATE9600, BAUDRATE115200);

                nRecvNullCnt = 0;
                if(eBaudRate == BAUDRATE9600)
                {
                    eBaudRate = BAUDRATE115200;
                }
                else if(eBaudRate == BAUDRATE115200)
                {
                    eBaudRate = BAUDRATE9600;
                }

                ACCOMM_FMT_TRACE("ReadACSample() READ_AC_RECV_STATE...修改波特率重试, 切换之后波特率类别eBaudRate=%d (%d:BAUDRATE9600, %d:BAUDRATE115200) \n", 
                    eBaudRate, BAUDRATE9600, BAUDRATE115200);
                
                state = CLOSE_AC_STATE;        // 切换波特率重试
                gBaudRateChangedFlag = 1;      // 波特率切换标志
                break;
            }
        }
        else
        {
            nRecvNullCnt = 0;
        }

        //优先处理UDP透传
        if (bRelayUDP)//收到UDP相关应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "UDP转发接收报文");
            
            PlcWriteQueue(pUdpSendDown, recvBuf, recvLen);
            bRelayUDP = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bAcDateAndTimeSet == TRUE)//收到设置日期应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            
            initRet = check_timing_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "设置日期和时间接收报文[%d](1成功; -1失败)", initRet);
            bAcDateAndTimeSet = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bAcDateSet == TRUE)//收到设置日期应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            initRet = check_timing_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "设置日期接收报文[%d](1成功; -1失败)", initRet);
            bAcDateSet = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bAcTimeSet == TRUE)//收到设置时间应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            initRet = check_timing_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "设置时间接收报文[%d](1成功; -1失败)", initRet);
            bAcTimeSet = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bDataInit == TRUE)//收到数据初始化应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            initRet = check_data_initial_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "数据初始化接收报文[%d](1成功; -1失败)", initRet);
            bDataInit = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bParamInit == TRUE)//收到参数初始化应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            initRet = check_param_initial_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "参数初始化接收报文[%d](1成功; -1失败)", initRet);
            bParamInit = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bDemandInit == TRUE)//收到需量初始化应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            initRet = check_demand_initial_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "需量初始化接收报文[%d](1成功; -1失败)", initRet);
            bDemandInit = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bDemandPeriodSet == TRUE)//收到设置最大需量周期应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            initRet = check_set_demand_period_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "设置最大需量周期接收报文[%d](1成功; -1失败)", initRet);
            bDemandPeriodSet = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bSlipTimeSet == TRUE)//收到设置滑差时间应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            initRet = check_set_slip_time_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "设置滑差时间接收报文[%d](1成功; -1失败)", initRet);
            bSlipTimeSet = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bZoneNumSet == TRUE)//收到设置时区数量应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            initRet = check_set_zone_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "设置时区数量接收报文[%d](1成功; -1失败)", initRet);
            bZoneNumSet = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bPeriodNumSet == TRUE)//收到设置日时段表/时段数量应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            initRet = check_set_day_period_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "设置日时段表/时段数量接收报文[%d](1成功; -1失败)", initRet);
            bPeriodNumSet = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bZoneTableSet == TRUE)//收到设置时区表应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            initRet = check_set_zone_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "设置时区表接收报文[%d](1成功; -1失败)", initRet);
            bZoneTableSet = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bDayPeriodSet == TRUE)//收到设置日时段表应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            initRet = check_set_day_period_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "设置日时段表接收报文[%d](1成功; -1失败)", initRet);
            bDayPeriodSet = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bParaSet == TRUE)//收到设置参数应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            initRet = check_set_para_result(recvBuf, recvLen);
            ACCOMM_BUF_DEBUG(recvBuf, recvLen, "设置参数接收报文[%d](1成功; -1失败)", initRet);
            bParaSet = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }

        if (bRelay485 == TRUE)//收到485相关应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            ACCOMM_BUF_TRACE(recvBuf, recvLen, "485转发接收报文");
            
            PlcWriteQueue(pDealSendDown[pipe], recvBuf, recvLen);
            bRelay485 = FALSE;
            state = READ_AC_SEND_STATE;        // 返回发送
            break;
        }
        
        if (recvLen > 9)//非485相关应答报文
        {
            if(recvLen > sizeof(recvBuf))
            {
                recvLen = sizeof(recvBuf);//长度容错
            }
            ACCOMM_BUF_LOGCC(recvBuf, recvLen, "Recv: ");

            pstart = (uint8 *)recvBuf;
            leftlen = recvLen;
            while(leftlen >= 0x0d)//循环检测1字节长度和2字节长度, 连续两次失败退出
            {
                while(leftlen >= 0x0d)//报文长度占2字节
                {
                    memset(recvBufTemp, 0x00, sizeof(recvBufTemp));
                    ptr = (uint8*)commfun_memchr_gb((char *)pstart, 0x68, leftlen);
                    if(ptr != NULL)
                    {
                        leftlen -= (ptr - pstart);					
                        memmove(recvBufTemp, ptr, leftlen);
                        
                        bRet = STDAC_CheckRspFrame(recvBufTemp, leftlen);
                        if(bRet == 1)
                        {
                            CheckRetSum = 0;
                            validlen = (recvBufTemp[9] + (recvBufTemp[10]<<8) + 0x0d);  //报文有效长度
                            ACCOMM_BUF_LOGCC(recvBufTemp, validlen, "L2有效分帧报文: ");
                            Pro_AC_Amr_Response(recvBufTemp);
                            
                            pstart = ptr + validlen;
                            leftlen -= validlen;
                        }
                        else
                        {
                            ACCOMM_FMT_LOGCC("bRet=%d \n", bRet);
                            CheckRetSum++;
                            break;
                        }
                    }
                    else
                    {
                        ACCOMM_FMT_LOGCC("if(ptr == NULL) \n");
                        CheckRetSum++;
                        break;
                    }
                }

                while(leftlen >= 0x0d)//报文长度占1字节
                {
                    memset(recvBufTemp, 0x00, sizeof(recvBufTemp));
                    ptr = (uint8*)commfun_memchr_gb((char *)pstart, 0x68, leftlen);
                    if(ptr != NULL)
                    {
                        leftlen -= (ptr - pstart);					
                        memmove(recvBufTemp, ptr, leftlen);
                        bRet = STDAC_CheckRspFrame_Single(recvBufTemp, leftlen);
                        if(bRet == 1)
                        {
                            CheckRetSum = 0;
                            validlen = (recvBufTemp[9] + 0x0c);  //报文有效长度
                            ACCOMM_BUF_LOGCC(recvBufTemp, validlen, "L1有效分帧报文: ");
                            Pro_AC_Amr_Response_Single(recvBufTemp);
                            
                            pstart = ptr + validlen;
                            leftlen -= validlen;
                        }
                        else
                        {
                            ACCOMM_FMT_LOGCC("bRet=%d \n", bRet);
                            CheckRetSum++;
                            break;
                        }
                    }
                    else
                    {
                        ACCOMM_FMT_LOGCC("if(ptr == NULL) \n");
                        CheckRetSum++;
                        break;
                    }
                }

                if(CheckRetSum >= 2)
                {
                    ACCOMM_FMT_LOGCC("if(CheckRetSum >= 2) \n");
                    break;
                }
            }

            //统计量计算(非直接采集)
            UpdateStatistics(type);
        }

        state = READ_AC_SEND_STATE;        // 状态不break往下顺序执行
        break;

    case CLOSE_AC_STATE:
        ACCOMM_FMT_TRACE("ReadACSample() CLOSE_AC_STATE...\n");
        if (fd_ac >= 0)
        {
            close(fd_ac);
        }

        fd_ac = -1;

        state = INIT_AC_STATE;
        sleep(2);
        break;

    default:

        break;
    }
}

/*********************************************************************
    所属功能单元：HAL接口对应的教材模块收发交互函数
    功能描述：
    输入参数：
    输出参数：
        无
    函数返回值：
    内部处理流程：
********************************************************************/
void ReadACSampleHAL(BOOL isUpdateAC, BOOL isGetVer)
{
    int32 recvLen = 0;
    int32 sendLen = 0;
    int32 bRet = 0;
    uint8  recvBuf[2000];
    uint8  sendBuf[600];
    static READ_AC_STATE state = INIT_AC_STATE;
    static UART_DEVICE_T* dev = NULL;
    uint8 *ptr=NULL;  //应答数据帧起始指针
    int ret = 0;

    memset(recvBuf, 0x00, sizeof(recvBuf));
    memset(sendBuf, 0x00, sizeof(sendBuf));

    switch (state)
    {
    case INIT_AC_STATE:
        ACPRINT(" ReadACSampleHAL() INIT_AC_STATE...\n");
        if(NULL == dev)
        {
            //dev = (UART_DEVICE_T*)hal_device_get(UART43_DEVICE_ID_STR);
            ACPRINT(" dev:%p \n", dev);
            sleep(1);
        }
        else
        {
            // 打开设备
            ACPRINT(" dev:%p \n", dev);
            ret = dev->uart_param_set(dev, 9600, 8, 1, PARITY_EVEN);  //TRUE:1, FALSE:0
            //ret = dev->uart_param_set(dev, 115200, 8, 1, PARITY_NO);
            ACPRINT(" dev->uart_param_set() ret=%d \n", ret);
            if(ret > 0)
            {
                state = READ_AC_SEND_STATE;      // 状态不break往下顺序执行
            }
        }

        break;

    case READ_AC_SEND_STATE:  // 发送抄读交采的帧
        ACPRINT(" ReadACSampleHAL() READ_AC_SEND_STATE...\n");
    
        sendLen = MakeFrameReadACModule(sendBuf);
        ret = dev->uart_data_send(dev, sendBuf, sizeof(sendLen));
        ACPRINT(" dev->uart_data_send() ret=%d \n", ret);
        if(ret > 0)
        {
            state = READ_AC_RECV_STATE;      // 状态不break往下顺序执行
        }
        printfBuff(sendBuf, sendLen, 0);
        
    case READ_AC_RECV_STATE:  // 处理抄读交采回码帧
        ACPRINT(" ReadACSampleHAL() READ_AC_RECV_STATE...\n");
        recvLen = dev->uart_data_recv(dev, recvBuf, sizeof(recvBuf));
        ACPRINT(" recvLen = %d\n", recvLen);
        
        if (recvLen > 0)
        {
            printfBuff(recvBuf, recvLen, 1);
            ptr = (uint8*)commfun_memchr_gb((char *)recvBuf, 0x68, recvLen);
            if(ptr != NULL)
            {
                recvLen -= (ptr - recvBuf);
                memmove(recvBuf, ptr, recvLen);
                bRet = STDAC_CheckRspFrame(recvBuf, recvLen);
                if(bRet == 1)
                {
                    Pro_AC_Amr_Response(recvBuf);
                }
            }
        }

        state = READ_AC_SEND_STATE;        // 状态不break往下顺序执行
        break;

    case CLOSE_AC_STATE:
        ACPRINT(" ReadACSampleHAL() CLOSE_AC_STATE...\n");
        if(NULL != dev)
        {
            // 关闭设备
            hal_device_release((HW_DEVICE *)dev);
        }

        dev = NULL;
        state = INIT_AC_STATE;
        break;

    default:

        break;
    }
}


/*******************************************************************************
* 函数名称: ac_calc_deal
* 函数功能: 交采处理线程主控函数
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void ac_calc_deal(uint32* cntchk)
{
    uint16 nEnergySecCounter = 3;         //电量采集秒计时器
    uint16 nDemandSecCounter = 4;         //需量采集秒计时器
    uint16 nHarmonicSecCounter = 4;       //谐波采集秒计时器
    uint16 nRealtimeSecCounter = 0;       //实时数据采集秒计时器
    uint8  nTaskType = SAMPLE_TYPE_NULL;  //任务类型

    uint16 nEnergySecCycle = 3;           //电量采集周期
    uint16 nDemandSecCycle = 5;           //需量采集周期
    uint16 nHarmonicSecCycle = 5;         //谐波采集周期
    uint16 nRealtimeSecCycle = 1;         //实时数据采集周期

    BOOL  isUpdateAC = FALSE;
    BOOL  isGetVer = FALSE;

    TimeManageInit();                      // 时间管理
    SharedMemMark.Bitsect.FreshMark = 1;   // 重新刷新交采共享数据

    while(1)
    {
        *cntchk = 0;
        TimeManageStart();                 // 一轮时间管理开始
        nTaskType = SAMPLE_TYPE_NULL;      //任务类型

        // 过秒处理
        if(gTimeFlag.BitSect.Second)
        {
            nEnergySecCounter ++;
            nDemandSecCounter ++;
            nHarmonicSecCounter ++;
            nRealtimeSecCounter ++;
            
            isUpdateAC = isJCUpdateRunning();//检测是否需要升级交采
            isGetVer = isJCNeedGetVer();     //是否需要读取版本信息
        }

        //检测任务类型
        if(nDemandSecCounter >= nDemandSecCycle)
        {
            nDemandSecCounter = 0;
            nTaskType = SAMPLE_TYPE_DEMAND;  //任务类型 需量
            ACCOMM_FMT_TRACE("TASK:DEMAND, Cycle=%d \n", nDemandSecCycle);
        }
        else if(nHarmonicSecCounter >= nHarmonicSecCycle)
        {
            nHarmonicSecCounter = 0;
            nTaskType = SAMPLE_TYPE_HARMONIC;  //任务类型 谐波
            ACCOMM_FMT_TRACE("TASK:HARMONIC, Cycle=%d \n", nHarmonicSecCycle);
        }
        else if(nEnergySecCounter >= nEnergySecCycle)
        {
            nEnergySecCounter = 0;
            nTaskType = SAMPLE_TYPE_ENERGY;  //任务类型 电量
            ACCOMM_FMT_TRACE("TASK:ENERGY, Cycle=%d \n", nEnergySecCycle);
        }
        else if(nRealtimeSecCounter >= nRealtimeSecCycle)
        {
            nRealtimeSecCounter = 0;
            nTaskType = SAMPLE_TYPE_REALTIME;  //任务类型 实时数据
            ACCOMM_FMT_TRACE("TASK:REALTIME, Cycle=%d \n", nRealtimeSecCycle);
        }
        else
        {
            nTaskType = SAMPLE_TYPE_NULL;  //任务类型 空
        }

#if AC_DEV_HAL == 1
        ReadACSampleHAL(FALSE, FALSE);
#else
        if (isUpdateAC == TRUE)
        {
            ReadACSample(TRUE, FALSE, nTaskType);
        }
        else if (isGetVer == TRUE)
        {
            ReadACSample(FALSE, TRUE, nTaskType);
        }
        else
        {
            ReadACSample(FALSE, FALSE, nTaskType);  //这里不需要判断过秒, 因为前面生成任务的时候已经判断过
        }
#endif

        usleep(10000);                  // 等待NEC层采样完成,本为500ms,加上采样等太长
        TimeManageEnd();                 // 一轮时间管理结束
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}


