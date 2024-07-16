/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：Event.h
* 版本号：1.0
* 功能：对事件检测过程中用到的数据结构，调度函数进行声明
* 原始开发人及时间：董宁 2006年12月25日
* 修改人员、时间及修改内容:无
* 其他：
******************************************************************-*/
#ifndef _EVENT_ZHI_H
#define _EVENT_ZHI_H

#include "baseCommon.h"
#include "sys_cfg.h"
#include "db_api.h"
//#include "../../include/protocol104.h"

#define MAX_EVENT_NUM  			15
#define MAX_DIFF_NUMBER 		4  				//差动越限事件检测支持的差动组个数
#define POWER_STOP_TIME_LMT 	180 				//上电事件判断上电与否需要的超时时间

/*--计量点类型定义--*/
#define OI_485       0XF201   /*-485表-*/
//#define OI_SIM       2   /*-模拟表-*/
#define OI_PULSE     0XF20A   /*-脉冲-*/
//#define OI_VIR       4   /*-虚拟-*/
#define OI_AC        0XF208   /*-交采-*/
#define OI_PLC       0XF209   /*-载波-*/
//#define OI_PLCMUI    7   /*-载波多功能-*/

#if SECURITY_DEFEND == 1
#include "../../../../../platform/Src/defend/defend.h"
#endif
#define	DEBUG_EN        0   //-调试语句开关0关闭，1打开-
#define	DEBUG_EN_1      0   //-调试语句开关0关闭，1打开-
#define	DEBUG_EN_8      0   //-调试语句开关0关闭，1打开-
#define DEBUG_EN_9      0
#define DEBUG_EN_9_1    0
#define DEBUG_EN_10     0   //电压回路异常检测、记录函数
#define DEBUG_EN_11 	0
#define DEBUG_EN_21 	0
#define DEBUG_EN_22 	0
#define DEBUG_EN_25 	0
#define DEBUG_EN_29 	0
#define DEBUG_EN_34 	0

//记录号
#define	ERC_NO_01	    		1 				//初始化及版本变更记录
#define	ERC_NO_02	    		2 				//终端参数丢失记录
#define	ERC_NO_03	    		3 				//备用
#define	ERC_NO_04	    		4 				//遥信变位记录
#define	ERC_NO_05	    		5 				//遥控跳闸记录
#define	ERC_NO_06	    		6 				//功控跳闸记录
#define	ERC_NO_07	    		7 				//电控跳闸记录
#define	ERC_NO_08	    		8 				//电能表参数变更记录
#define	ERC_NO_09	    		9 				//电流回路异常记录
#define	ERC_NO_10	    		10				//电压回路异常记录
#define	ERC_NO_11	    		11				//相序异常记录
#define	ERC_NO_12	    		12				//表时钟异常记录
#define	ERC_NO_13	    		13				//电能表故障记录
#define	ERC_NO_14	    		14				//终端停/上电事件记录
#define	ERC_NO_15	    		15				//谐波越限记录
#define	ERC_NO_16	    		16				//备用
#define	ERC_NO_17	    		17				//电压电流不平衡
#define	ERC_NO_18	    		18				//备用
#define	ERC_NO_19	    		19				//电压电流不平衡
#define	ERC_NO_20	    		20				//密码错误记录
#define	ERC_NO_21	    		21				//终端故障记录(485故障)
#define	ERC_NO_22	    		22				//有功总电能量差动越限
#define	ERC_NO_23	    		23				//电控告警事件
#define	ERC_NO_24	    		24				//电压越限记录
#define	ERC_NO_25	    		25				//电流越限告警
#define	ERC_NO_26	    		26				//电压电流畸变率越限及恢复告警
#define	ERC_NO_27	    		27				//零序电流偏大判断
#define	ERC_NO_28	    		28				//负荷过载
#define	ERC_NO_29	    		29				//无功过补偿
#define	ERC_NO_30	    		30				//无功欠补偿






#if ANOLOGY_EN == 1		//直流模拟量越限事件
#define DPRINT_16(x...) ({ if (getenv("DEBUG_EVENT_16") != NULL)  printf(x);})
#endif

#define DPRINT(x...) ({ if (getenv("DEBUG_EVENT") != NULL)  printf(x);})
#define DPRINT_28(x...) ({ if (getenv("DEBUG_EVENT_28") != NULL)  printf(x);})
/*-******************************************************************-*/
/*-                             变量定义                             -*/
/*-******************************************************************-*/
#define Event07 0
#define MINS_IN_DAY 86400	//每日的秒数
/*-******************************************************************-*/
/*-                             结构定义                             -*/
/*-******************************************************************-*/
typedef struct
{
    uint8  bfgEmpty;	//-记录为空标志-
    uint8  bRecNo;		//-首次记录的记录号-
    uint8  bTurnNo;		//-轮次，按位表示D0表示第一轮，以此类推-
    uint8  bTime[5];	//-D0开始分别为分、时、日、月、年，数据格式15-
    uint8  bBFPower[2];	//- 跳闸前功率，数据格式02-
    uint8  bFWPower[2];	//- 跳闸后功率，数据格式02-
}T_REMOTE_REC;

typedef struct
{
	uint8  bfgEmpty;	//-记录为空标志-
    uint8  bRecNo;		//-首次记录的记录号-
    uint8  bTurnNo;		//-轮次，按位表示D0表示第一轮，以此类推-
	uint8  bTime[5];	//-D0开始分别为分、时、日、月、年，数据格式15-
    uint8  bBFPower[2];	//- 跳闸前功率，数据格式02-
    uint8  bFWPower[2];	//- 跳闸后功率，数据格式02-
    uint8  bFVPower[2];	//- 当时定值，数据格式02-
}T_POWER_REC;

typedef struct
{
    uint8  bfgEmpty;	//-记录为空标志-
    uint8  bTurnNo; 	//-轮次-
	uint8  bTime[5];	//-D0开始分别为分、时、日、月、年，数据格式15-
    uint8  bLeftEN[5];	//-跳闸时剩余电量，数据格式03-
}T_BUYEN_REC;

typedef struct
{
    uint8  bfgEmpty;	//-记录为空标志-
    uint8  bTurnNo; 	//-轮次-
	uint8  bTime[5];	//-D0开始分别为分、时、日、月、年，数据格式15-
	uint8  bPowerBuf[4];//- 跳闸时用电负荷-
	uint8  bSetVal[5];	//- 跳闸时电量定值，数据格式03-
}T_DAYEN_REC;

typedef struct
{
    uint8  bfgEmpty;	//-记录为空标志-
    uint8  bTurnNo; 	//-轮次-
	uint8  bTime[5];	//-D0开始分别为分、时、日、月、年，数据格式15-
    uint8  bPowerBuf[4];//-月电控记录写入私有文件"^Event"的数据以国网376.1规约ERC7(电控跳闸记录)格式为准,bPowerBuf[]中存放的数据实际应为“跳闸时电能量（总加电能量）”数据格式03-
	uint8  bSetVal[5];	//- 跳闸时电量定值，数据格式03-
}T_MONEN_REC;

typedef struct
{
    uint8  bfgEmpty;	//-记录为空标志-
	#if TMNHALT_METHOD == 1
	uint8 bValidFlag;		// 有效性标志
	#endif
	uint8  bSTTime[5];	//-D0开始分别为分、时、日、月、年，数据格式15-
    uint8  bOVTime[5];	//-D0开始分别为分、时、日、月、年，数据格式15-
}T_POWERDOWN_REC;

typedef struct									//-记录存放数据结构-
{
	uint8 bERC;	     							//-记录号-
	uint8 bLen;		 							//-记录长度-
	uint8 bData[128];							//-记录数据内容-
}RECODE_SAVE_T;

/***********************************************************************/
/*事件id定义结构体*/
/***********************************************************************/

/*-******************************************************************-*/
/*-                       私有数据地址和长度定义                     -*/
/*-******************************************************************-*/

#define OutIRecAddr				50											//-重要事件记录-
#define OutNRecAddr				OutIRecAddr+OutIRecLen						//-一般事件记录-
#define OutRecCountAddr			OutNRecAddr+OutNRecLen	//-申请四个字节，顺序存放重要事件个数，重要事件存储位置，一般事件个数，一般事件存储位置-
#define RemoteAddr				OutRecCountAddr+OutRecCountLen				//-遥控记录-
#define PowerAddr				RemoteAddr+RemoteRecLen						//-功控记录-
#define BuyAddr					PowerAddr+PowerRecLen						//-购电控记录-
#define DayEnAddr				BuyAddr+BuyRecLen							//-日电控记录-
#define MonEnAddr				DayEnAddr+DayEnRecLen						//-月电控记录-
#define PowerDownAddr			MonEnAddr+MonEnRecLen						//-失电记录-
#define ProgUpAddr			    PowerDownAddr+PowerDownRecLen				//-上电时间-
#define ProgDownAddr			ProgUpAddr+ProgDownRecLen				    //-停电时间-
#define SwitchStatAddr			ProgDownAddr+ProgDownRecLen			        //-开关状态-
#define SwitchFastStatAddr		SwitchStatAddr+SwitchStatLen				//-立刻检测变化的开关状态-
#define SwitchChgStatAddr		SwitchFastStatAddr+SwitchFastStatLen		//-开关变位状态-
#define BattStatusLastRecAddr	SwitchChgStatAddr+SwitchChgStatLen			//-上次电池欠压状态记录-
#if HENAN_FUN == 1 || HUNAN_FUN == 1 || LIAONING_FUN == 1
#define CBBattStatusLastRecAddr	BattStatusLastRecAddr+BattStatusLastRecLen			//-上次抄表电池欠压状态记录-
#define EventRtcFlagRecAddr		CBBattStatusLastRecAddr+BattStatusLastRecLen		//-终端时钟电池欠压记录-
#define EventPhaseRecAddr		EventRtcFlagRecAddr+EventFlagRecLen				//-相位信息记录-
#else
#define EventPhaseRecAddr		BattStatusLastRecAddr+BattStatusLastRecLen	//-相位信息记录-
#endif
#define ProgNumLastRecAddr		EventPhaseRecAddr+EventPhaseRecLen			//-编程次数记录-
#define DmZeroNumLastRecAddr	ProgNumLastRecAddr+ProgNumLastRecLen		//-清零次数记录-
#define MetEnLastRecAddr		DmZeroNumLastRecAddr+DmZeroNumLastRecLen	//-电表上次电量-
#define MetLastCTPTAddr			MetEnLastRecAddr+MetEnLastRecLen			//-电表上次互感倍率-
#define MetLastConstAddr		MetLastCTPTAddr+MetLastCTPTLen				//-电表上次有功电表常数-
#define MetLastProgTimeAddr		MetLastConstAddr+MetLastConstLen			//-电表上次编程时间-
#define MetLastSegInfoAddr		MetLastProgTimeAddr+MetLastProgTimeLen		//-电表上次电表时段信息-
#define MetLastCbDayAddr		MetLastSegInfoAddr+MetLastSegInfoLen		//-电表上次抄表日-
#define MetLastDmZeroAddr		MetLastCbDayAddr+MetLastCbDayLen			//-电表上次电表清零次数-

#define ASideStatRecAddr		MetLastDmZeroAddr+MetLastDmZeroLen			//-A相一次二次侧短路开路标志记录-
#define BSideStatRecAddr		ASideStatRecAddr+ASideStatRecLen			//-B相一次二次侧短路开路标志记录-
#define CSideStatRecAddr		BSideStatRecAddr+BSideStatRecLen			//-C相一次二次侧短路开路标志记录-
#define CurReverseRecAddr		CSideStatRecAddr+CSideStatRecLen			//-反极性标志记录-
#define MinuteCountRecAddr		CurReverseRecAddr+CurReverseRecLen			//-分钟计数值记录（表事件用）-
#define EventStatusRecAddr		MinuteCountRecAddr+MinuteCountRecLen		//-全局变量，保存测量点事件标志记录-
#define EventStoragNumRecAddr3105	EventStatusRecAddr+EventStatusRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr3105	EventStoragNumRecAddr3105+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr310B	EventStoragTmRecAddr3105+EventStoragTmRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr310B	EventStoragNumRecAddr310B+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr310C	EventStoragTmRecAddr310B+EventStoragTmRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr310C	EventStoragNumRecAddr310C+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr310D	EventStoragTmRecAddr310C+EventStoragTmRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr310D	EventStoragNumRecAddr310D+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr310E	EventStoragTmRecAddr310D+EventStoragTmRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr310E	EventStoragNumRecAddr310E+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr310F	EventStoragTmRecAddr310E+EventStoragTmRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr310F	EventStoragNumRecAddr310F+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr3119	EventStoragTmRecAddr310F+EventStoragTmRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr3119	EventStoragNumRecAddr3119+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr300F	EventStoragTmRecAddr3119+EventStoragTmRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr300F	EventStoragNumRecAddr300F+EventStoragNumMntRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr3110	EventStoragTmRecAddr300F+EventStoragMntRecLen	//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr3110	EventStoragNumRecAddr3110+EventStoragNumMntRecLen	//-全局变量，保存测量点事件开始时间-

#define LostvltAddr				EventStoragTmRecAddr3110+EventStoragMntRecLen	//-失压次数-
#define BreakphaseAddr			LostvltAddr+LostvltLen						//-断相次数-
#define StopPowerAddr			BreakphaseAddr+BreakphaseLen			    //-掉电次数-
#define PhaseAbnormalAddr		StopPowerAddr+StopPowerLen                  //-相序异常-
#define PhaseRvsLastTimesAddr	PhaseAbnormalAddr+PhaseAbnormalLen			//-相序异常持续时间-

#define EnDiffRecAddr			PhaseRvsLastTimesAddr+PhaseRvsLastTimesLen
#define EventFlagPhaseAbRecAddr	EnDiffRecAddr+EnDiffRecLen			        //--终端相序异常记录标志
#define EventFlagRecAddr		EventFlagPhaseAbRecAddr+EventFlagRecLen		//--485故障记录标志
#define Min15CounterAddr		EventFlagRecAddr+EventFlagRecLen			//-全局变量，保存15分钟计数-
#define Min15_485_CounterAddr		Min15CounterAddr+Min15CounterLen			//-全局变量，保存15分钟计数-
#define OutSeqAddr				Min15_485_CounterAddr+Min15_485_CounterLen	//-传出的帧序号-
#define InSeqAddr				OutSeqAddr+OutSeqLen						//-传入的帧序号-
#define AutoSendSeqAddr			InSeqAddr+InSeqLen							//-主动上报的帧序号-
#define FirstProgAddr			AutoSendSeqAddr+AutoSendSeqLen				//-首次上电标志-
#define TmnPowerOffAddr			FirstProgAddr+FirstProgLen					//-终端停电发生标志记录-
#define LastVersionAddr			TmnPowerOffAddr+TmnPowerOffLen				//-历史软件版本-
#define InDataRPTRecAddr		LastVersionAddr+LastVersionLen				//-重要事件上报标志记录-

#define HRUpLmtEvenAddr			InDataRPTRecAddr+InDataRPTLen				//谐波越限事件发生记录，按位表示总、2、3....次-
#define DCOverFlowAddr			HRUpLmtEvenAddr+HRUpLmtEvenLen				//直流模拟量越限事件发生标志

#define TelOverFlowAddr			DCOverFlowAddr+DCOverFlowLen				//通信流量越线
#define MetStateChangeAddr		TelOverFlowAddr+TelOverFlowLen				//电表状态字
#define MeterOpenNum			MetStateChangeAddr+MetStateChangeLen		//电表开盖记录
#define MeterCapOpenNum		    MeterOpenNum+MeterOpenNumLen
#define MetBCFailAddr			MeterCapOpenNum+MeterCapOpenNumLen			//补抄失败记录
#define FluxOverTopAddr         MetBCFailAddr + MetCBFailLen				//通讯流量超标已记录标识
#if 1
#define ControlLoopStatusAddr   FluxOverTopAddr + FluxOverTopLen          //控制回路状态
#define TimeSettingAddr			ControlLoopStatusAddr + MetCBFailLen
#else
#define MetBMagneticAbNormalAddr	MetBCFailAddr+MetCBFailLen							// 磁场事件异常的记录
#define ControlLoopStatusAddr   	MetBMagneticAbNormalAddr + MetBMagneticAbNormalLen  	// 控制回路状态

//#define ControlLoopStatusAddr   MetBCFailAddr + MetCBFailLen  //控制回路状态
#define TimeSettingAddr			ControlLoopStatusAddr + MetCBFailLen			// 主要保存上次校时总次数
#endif
#if SHANDONG_FUN == 1
#define MetLastSegInfoAddrExt		TimeSettingAddr+TimeSettingAddr		//-电表上次电表时段信息-
#define EventStoragNumRecAddr3000	MetLastSegInfoAddrExt+MetLastSegInfoLen		//-全局变量，保存测量点事件存储序号
#else
#define EventStoragNumRecAddr3000   TimeSettingAddr+TimeSettingAddr
#endif
#define EventStoragTmRecAddr3000	EventStoragNumRecAddr3000+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr3001	EventStoragTmRecAddr3000+EventStoragTmRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr3001	EventStoragNumRecAddr3001+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr3002	EventStoragTmRecAddr3001+EventStoragTmRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr3002	EventStoragNumRecAddr3002+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr3003	EventStoragTmRecAddr3002+EventStoragTmRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr3003	EventStoragNumRecAddr3003+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr3005	EventStoragTmRecAddr3003+EventStoragTmRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr3005	EventStoragNumRecAddr3005+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr301D	EventStoragTmRecAddr3005+EventStoragTmRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr301D	EventStoragNumRecAddr301D+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr301E	EventStoragTmRecAddr301D+EventStoragTmRecLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr301E	EventStoragNumRecAddr301E+EventStoragNumRecLen	//-全局变量，保存测量点事件开始时间-

#define EventExamLen			EventStoragTmRecAddr301E+EventStoragTmRecLen		//-事件检测私有数据总长度-

#define OutIRecLen				sizeof(RECODE_SAVE_T)*256		//-重要事件记录-
#define OutNRecLen				sizeof(RECODE_SAVE_T)*256		//-一般事件记录-
#define OutRecCountLen			4	//-申请四个字节，顺序存放重要事件个数，重要事件存储位置，一般事件个数，一般事件存储位置-
#define RemoteRecLen			sizeof(T_REMOTE_REC)*5			//-遥控记录-
#define PowerRecLen				sizeof(T_POWER_REC)*5			//-功控记录-
#define BuyRecLen				sizeof(T_BUYEN_REC)*5			//-购电控记录-
#define DayEnRecLen				sizeof(T_DAYEN_REC)*5			//-日电控记录-
#define MonEnRecLen				sizeof(T_MONEN_REC)*5			//-月电控记录-
#define PowerDownRecLen			sizeof(T_POWERDOWN_REC)*5		//-失电记录-
#define ProgDownRecLen			sizeof(OOP_DATETIME_S_T)*2		//-停电时间-
#define SwitchStatLen			4								//-开关状态记录-
#define SwitchFastStatLen		4								//-立刻检测变化的开关状态-
#define SwitchChgStatLen		4								//-开关变位状态记录-
#define BattStatusLastRecLen	MTR485_MAX_NUM					//-上次电池状态记录-
#define EventPhaseRecLen		(MTR485_MAX_NUM)*(MAX_EVENT_NUM)//-相位信息记录-

#define ProgNumLastRecLen		(MTR485_MAX_NUM)*4				//-编程次数记录-
#define DmZeroNumLastRecLen		(MTR485_MAX_NUM)*4				//-清零次数记录-
#define MetEnLastRecLen			(MTR485_MAX_NUM)*4*8			//-上次能量值记录-
#define MetLastCTPTLen			(MTR485_MAX_NUM)*4				//-电表上次互感倍率-
#define MetLastConstLen			(MTR485_MAX_NUM)*2				//-电表上次有功电表常数-
#define MetLastProgTimeLen		(MTR485_MAX_NUM)*4				//-电表上次编程时间-
#define MetLastSegInfoLen		(MTR485_MAX_NUM)*24				//-电表上次电表时段信息-
#define MetLastCbDayLen			(MTR485_MAX_NUM)*2				//-电表上次抄表日-
#define MetLastDmZeroLen		(MTR485_MAX_NUM)*4				//-电表上次电表清零次数-

#define ASideStatRecLen			MTR485_MAX_NUM					//-A相一次二次侧短路开路标志记录-
#define BSideStatRecLen			MTR485_MAX_NUM					//-B相一次二次侧短路开路标志记录-
#define CSideStatRecLen			MTR485_MAX_NUM					//-C相一次二次侧短路开路标志记录-
#define CurReverseRecLen		MTR485_MAX_NUM					//-反极性标志记录-
#define MinuteCountRecLen		(MTR485_MAX_NUM)*2				//-分钟计数值记录（表事件用）-
#define EventStatusRecLen		MTR485_MAX_NUM*4				//-全局变量，保存测量点事件标志记录-
#define EventStoragNumRecLen	MTR485_MAX_NUM*4				//-全局变量，保存测量点事件存储序号,
#define EventStoragTmRecLen	    MTR485_MAX_NUM*4				//-全局变量，保存测量点事件开始时间,
#define EventStoragNumMntRecLen  	4				//-全局变量，保存终端事件存储序号,
#define EventStoragMntRecLen	    4				//-全局变量，保存终端事件开始时间,


#define LostvltLen				(MTR485_MAX_NUM)*4				//-失压次数-
#define BreakphaseLen			(MTR485_MAX_NUM)*4				//-断相次数-
#define StopPowerLen			(MTR485_MAX_NUM)*4				//-掉电次数-
#define PhaseAbnormalLen		(MTR485_MAX_NUM)*4				//-4个字节分别存放电流短路/开路/反向/逆相序标志-
#define PhaseRvsLastTimesLen	(MTR485_MAX_NUM)*2				//-相序异常持续时间-

#define EnDiffRecLen			MAX_DIFF_NUMBER*4				//-总加组差动事件-
#define EventFlagRecLen			1								//-事件发生标志记录-
#define Min15CounterLen			4								//-全局变量，保存15分钟计数-
#define Min15_485_CounterLen		128							//-全局变量，保存15分钟计数-
#define OutSeqLen				1								//-传出帧序号-
#define InSeqLen				1								//-传入帧序号-
#define AutoSendSeqLen			1								//-主动上报序号-
#define FirstProgLen			1								//-首次上电标志-
#define TmnPowerOffLen			2								//-终端停电事件发生标志-
#define LastVersionLen			8								//-软件版本长度-
#define InDataRPTLen			8								//-重要事件上报标志长度-
#define HRUpLmtEvenLen			18								//谐波越限事件发生记录，每三个一组:分别表示A电压、B电压、C电压、A电流、B电流、C电流
																//每组按位表示总、2、3....次-
#define DCOverFlowLen			2								//直流模拟量越限事件发生标志，低字节按位表示越上限，高字节按位表示越下限
#define TelOverFlowLen			1								//通信流量越限长度
#define MetStateChangeLen  		(MTR485_MAX_NUM)*14				//电表状态变化的长度
#define MeterOpenNumLen 		(3*NO_VIR_MTR_MAX_NUM)					//电能表开表盖次数的长度
#define MeterCapOpenNumLen		(3*NO_VIR_MTR_MAX_NUM)					//电能表开表端次数的长度
#define MetCBFailLen				NO_VIR_MTR_MAX_NUM						//补抄失败标志长度
#define FluxOverTopLen		    1						                //通讯流量超标已上报标志
#if 1
#define ControlLoopStatusLen    2								//控制回路状态
#define TimeSettingLen			4 * MTR485_MAX_NUM
#else
#define MetBMagneticAbNormalLen	MTR485_MAX_NUM*(6+6)		// 磁场事件异常的记录的上次发生和结束时间
#define ControlLoopStatusLen    2								//控制回路状态
#define TimeSettingLen			4 * MTR485_MAX_NUM				// 存上次校时总次数
#endif

#define DA_MTR_EVENT_MAX_NUM    8 //故障上报支持的电表最大数量
#define BREAKPHASE 		1
#define LOST_VOLT 		2




/*
mtr_id: 测量点号
timesId: 发生次数id
eventDataId:事件数据ID
phase:相别
*/


typedef union
{
	struct
	{
		uint16	mtrid:12;
		uint16	reserve:4;
	}__attribute__ ((packed)) bit;
	uint16	all;
}__attribute__ ((packed)) ERC3738_U;	//-帧序号字节-


typedef struct
{
	uint8 ercNo;
	uint8 ercRecLen;
	uint8 eventTime[5];	//BCD
	ERC3738_U erc3738_u;
	char  openCapCount[3];
	uint8 value[60];	//3761格式和645_07格式一致，不再区分
}__attribute__ ((packed)) ERC3738_RECORD_STRUCT;


typedef struct
{
	uint32 eventTimeId;
	uint32 eventRecordId;
	uint32 cap_id;	//表盖ID  :端钮盖，表盖
	uint32 ercNo;
	uint32 ercRecLen;
}__attribute__ ((packed)) CHECK_CAP_STRUCT;


typedef union
{
	struct
	{
		uint16	mtrid:12;
		uint16	reserve:3;
		uint16	startStopFlag:1;
	}__attribute__ ((packed)) bit;
	uint16	all;
}__attribute__ ((packed)) ERC40_U;	//-帧序号字节-

typedef struct
{
	uint8 ercNo;
	uint8 ercRecLen;
	uint8 eventTime[5];	//BCD
	ERC40_U erc40_u;
	char deviceType;
	char addr[6];
	char magneticType;
}__attribute__ ((packed)) ERC40_RECORD_STRUCT;


//北京地区停上电事件特殊要求
typedef struct
{
    uint8  D0       :1;     //D0置"1"：事件正常，D0置"0"：事件异常，此处是对停电时间有效间隔的判断
    uint8  D1       :1;	    //D1置"1"：事件有效，D1置"0"：事件无效，此处是对停电时间偏差限值的判断
    uint8  D2_D7    :6;     //预留
}__attribute__ ((packed)) TMN_HALT_INFO_STR;


typedef struct
{
    uint8                     ErcNo;              //ERC14
    uint8                     ErcRecLen;          //ERC长度
    TMN_HALT_INFO_STR         ErcMetSn;
    char                      TmnPowerOffTime[5]; //终端停电时间
    char                      TmnPowerOnTime[5];  //终端上电时间
}__attribute__ ((packed)) ERC14_RECORD_STRUCT;

typedef enum
{
    RELAY_IDEL,                  //空闲
    RELAY_SEND,					 //发送
    RELAY_WATT,					 //等待接收
    RELAY_RECVD,				 //接收完成
    RELAY_FAIL,				     //接收失败
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
	mtrnum_t 				Pn;			        //测量点号
    OOP_TSA_T               Addr;               //电表地址
	OOP_OAD_U				commnum;		    //通讯端口号
	uint8                   protocol;           //通信规约类型
	uint8                   mode;               //通信模式字
	uint8					RelayResult;        //中继穿透结果
}METER_HALT_INFO_STRUCT;

typedef struct
{
	METER_HALT_STATE_T   	MeterHalt_Stat;	    //
	RELAY_STATE_T           Relaydstat;         //中继状态
	uint8                   CheckMeterHalt;     //0:定时检测   1:上电检测
	uint8                   MeterHaltNum;       //电表停上电抄读的数目
	uint8                   MeterHaltIndex;     //电表停上电抄读的索引
	uint8                   MeterIdNum;         //抄读的ID个数
	time_t                  HaltTaskStartTime;  //任务起始时间
	METER_HALT_INFO_STRUCT  MeterHaltInfo[6];	//电表停上电抄读的信息
}METER_HALT_STAT_STRUCT;

// 该文件用于判断需不需要设置ERC14和自动上报,第一次上电设置一次以后都不再设置
#define SETPARA_ERC14_AND_AUTOREPORT		"^set_para_erc14_and_auto_report"

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

//编程事件
#define     PS_ERR           -1        //编程失败
#define     PS_ON             0        //编程中
#define     PS_OK             1        //编程成功

//事件上报
typedef struct
{
    uint32              dataLen;                            //数据长度
    uint8               pdataBuf[EVENT_RECORD_MAX_LEN];     //上报数据
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
    OOP_RCSD_T          eventRCSD;                      //事件上报RCSD
    OOP_DATETIME_S_T    starttime[MTR485_MAX_NUM];      //发生时间
    OOP_DATETIME_S_T    endtime[MTR485_MAX_NUM];        //结束时间
    uint32              storagenum[MTR485_MAX_NUM];     //存储序号
    uint32              ERFlg[MTR485_MAX_NUM];          //状态标记，标记事件是否发生
    EVTSTATE            eventState;                     //事件当前值
    EVTTIME             eventTime;                      //事件时间状态记录表
    uint8               bType;                          //事件类型，终端 0 涉及到电能表1
}EVENT_COMM_INFO;

typedef struct
{
    EVENT_COMM_INFO eventCommInfo;
    OOP_WORD3V_T   oopWord3V[MTR485_MAX_NUM];    //事件发生时电压
    OOP_INT3V_T   oopWord3C[MTR485_MAX_NUM];    //事件发生时电流
    uint8           initflag;      //参数初始化标记，标识参数是否初始化
}EVENT_3000_INFO;

typedef struct
{
    EVENT_COMM_INFO eventCommInfo;
    OOP_WORD3V_T   oopWord3V[MTR485_MAX_NUM];    //事件发生时电压
    OOP_INT3V_T   oopWord3C[MTR485_MAX_NUM];    //事件发生时电流
    //uint8           ishappen;      //状态标记，标记事件是否发生
    uint8           initflag;      //参数初始化标记，标识参数是否初始化
}EVENT_3001_INFO;

typedef struct
{
    EVENT_COMM_INFO eventCommInfo;
    OOP_WORD3V_T   oopWord3V[MTR485_MAX_NUM];    //事件发生时电压
    OOP_INT3V_T   oopWord3C[MTR485_MAX_NUM];    //事件发生时电流
    uint8           initflag;      //参数初始化标记，标识参数是否初始化
}EVENT_3002_INFO;

typedef struct
{
    EVENT_COMM_INFO eventCommInfo;
    OOP_WORD3V_T   oopWord3V[MTR485_MAX_NUM];    //事件发生时电压
    OOP_INT3V_T   oopWord3C[MTR485_MAX_NUM];    //事件发生时电流
    //uint8           ishappen;      //状态标记，标记事件是否发生
    uint8           initflag;      //参数初始化标记，标识参数是否初始化
}EVENT_3003_INFO;

typedef struct
{
    EVENT_COMM_INFO 	 eventCommInfo;
    OOP_INT3V_T        oopWord3C[MTR485_MAX_NUM];    //事件发生时电流
    //uint8                ishappen; //状态标记，标记事件是否发生
    uint8             	 initflag; //参数初始化标记，标识参数是否初始化
}EVENT_3005_INFO;

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    uint8             initflag;         //参数初始化标记，标识参数是否初始化
    uint8             nDelay;           //判断延时
    OOP_METWORD_T     meterRsw;         //电表运行状态字7
}EVENT_300F_INFO;

typedef struct
{
    EVENT_COMM_INFO 	 eventCommInfo;
    OOP_WORD3V_T        oopWord3V[MTR485_MAX_NUM];    //事件发生时电压
    //uint8                ishappen; //状态标记，标记事件是否发生
    uint8             	 initflag; //参数初始化标记，标识参数是否初始化
}EVENT_301D_INFO;

typedef struct
{
    EVENT_COMM_INFO 	 eventCommInfo;
    OOP_INT3V_T        oopWord3C[MTR485_MAX_NUM];    //事件发生时电压
    uint8             	 initflag; //参数初始化标记，标识参数是否初始化
}EVENT_301E_INFO;

typedef struct
{
    EVENT_COMM_INFO     eventCommInfo;
    OOP_POWERONCUT_T    eventParam;    //事件配置参数
    OOP_WORD3V_T       tmnVoltage;    //终端电压
    OOP_DATETIME_S_T    tmnStarttime;  //终端停电发生时间
    OOP_DATETIME_S_T    tmnEndtime;    //终端停电结束时间
    int8                bfgPower;	   //-电池状态标志-
    uint8               eventFlag;     //事件属性标志 bit0 置“1”：事件正常， bit0 置“0”：事件异常；bit1 置“1”：事件有效， bit1 置“0”：事件无效；
    uint8               ishappen;      //状态标记，0x55-停电已经发生 0xAA-停电未发生
    uint8               initflag;      //参数初始化标记，标识参数是否初始化
}EVENT_3106_INFO;

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
    OOP_WORD4V_T      oopWord4V[MTR485_MAX_NUM];    //事件发生时电压相角
    OOP_WORD4V_T      oopWord4C[MTR485_MAX_NUM];    //事件发生时电压电流相角
    OOP_ENERGY_T      oopEnergys[MTR485_MAX_NUM];   //事件发生时正向有功总电能
    uint8             initflag;                     //参数初始化标记，标识参数是否初始化
}EVENT_3119_INFO;

typedef struct
{
    uint8               enabled;        //配置标记
    OOP_METER_T         oopMeter;       //采集档案
}OOP_METER_INFO;

typedef struct
{
	uint32  oadValue;
    uint32  dateLen;
    uint8   *pBuf;
}EventUpdateDataInfo;

typedef struct
{
    pthread_mutex_t      mutex;                             //锁 多线程访问
    RECORD_UNIT_T        meterReadTime[MTR485_MAX_NUM];     //抄表时间
	OOP_INT3V_T          curTemp[MTR485_MAX_NUM];           //电流，
    OOP_WORD3V_T        valTemp[MTR485_MAX_NUM];           //电压
    OOP_ENERGY_T         forwardActive[MTR485_MAX_NUM];     //正向有功电能
    OOP_ENERGYS_T        forwardUnActive[MTR485_MAX_NUM];   //正向无功电能
    OOP_WORD4V_T         oopWord4V[MTR485_MAX_NUM];         //事件发生时电压相角
    OOP_WORD4V_T         oopWord4C[MTR485_MAX_NUM];         //事件发生时电压电流相角
}LatestMeterReadData;


typedef struct
{
    pthread_mutex_t      mutex;             //锁 多线程访问
	OOP_ENERGY_T        penergy;         //正向有功 0x00100200
	OOP_ENERGYS_T       penergys;        //正向无功 0x00300200
	OOP_ENERGY_T        nenergy;         //反向有功 0x00200200
	OOP_ENERGYS_T       nenergys;        //反向无功 0x00400200
	OOP_ENERGY_T        quadrant1;       //第一象限 0x00500200
	OOP_ENERGY_T        quadrant2;       //第二象限 0x00600200
	OOP_ENERGY_T        quadrant3;       //第三象限 0x00700200
	OOP_ENERGY_T        quadrant4;       //第四象限 0x00800200
	OOP_WORD3V_T        vol;             //电压       0x20000200
	OOP_INT3V_T         cur;             //电流       0x20010200
	int32               z_cur;           //零序电流     0x20010600
	OOP_WORD3V_T        oopWord3V;       //电压相角 0x20020200
    OOP_WORD3V_T        oopWord3C;       //电流相角 0x20030200
	OOP_INT4V_T         ppower;          //有功功率     0x20040200
	OOP_INT4V_T         npower;          //无功功率     0x20050200
	OOP_LONG4V_T        fpower;          //功率因数     0x200A0200
	OOP_DEMAND_T        pdemand;         //正向有功需量 0x10100200
	OOP_DEMAND_T        ndemand;         //反向有功需量 0x10200200
	OOP_METWORD_T       meterRsw;        //运行状态字 0x20140200
	OOP_HARMONIC_T      harmonicV[3];    //三相电压谐波含有率0x200D0200  0 x200D0400
    OOP_HARMONIC_T      harmonicC[3];    //三相电流谐波含有率0x200E0200  0 x200E0400
}LatestACData;
extern LatestACData g_LatestACData;

#define MAX_TEL_NUM 12
#define MAX_ENE_NUM 14

typedef struct //故障信息上报,关联数据结构体
{
	uint32 		objAddr;		//信息对象地址
	uint32      objdata;        //事件发生时刻数据
	uint8 		QDS;			//品质描述符
}REPORT_RETAL;

typedef struct //故障信息上报参数数据结构，上报按带时标的单点遥信上报
{
	uint8        bfgchange;         //变位标志
	uint16 		 objAddr;		    //事件编码
	uint8        btime[7];          //事件发生时间
	uint8 		 tel_num;		    //遥测个数
	uint8 		 tel_ti_num;		//遥测个数 带时标
	REPORT_RETAL tel_data[MAX_TEL_NUM];       //变位发生时间
	uint8        energy_num;                  //电能量数据个数
	REPORT_RETAL energy_data[MAX_ENE_NUM];    //电能量数据
}EVENT_REPORT_SEND;

typedef struct //故障信息上报,关联数据结构体
{
	uint32 		report_addr;		//信息对象地址
	uint32      *preport_data;      //指针事件发生时刻数据
}REPORT_DATA;


#if GB101_104_EN
extern uint8 bfgCommType[MTR485_MAX_NUM];
#endif

extern OOP_OCTETVAR16_T    tmnAddr;                             //终端通信地址
extern OOP_OCTETVAR64_T    MAC;                                 //校验码
extern uint16              MTR485Num;                           //485测量点的个数


uint32 Uint16To32(uint16 front, uint16 rear);
uint32 OopTIToMIn(OOP_TI_T oopTI);

/*-******************************************************************-*/
/*-                             函数                                 -*/
/*-************************************** ** ************************-*/
void CheckERC_3000(int clientid); //断相
void CheckERC_3001(int clientid); //欠压
void CheckERC_3002(int clientid);	//过压
void CheckERC_3003(int clientid); //缺相
void CheckERC_3005(int clientid); //过流
void CheckERC_300F(int clientid); //逆相
void CheckERC_301D(int clientid); //电压不平衡
void CheckERC_301E(int clientid); //电流不平衡
void CheckERC_3106_zhj(int clientid); //终端停上电
void CheckERC_3119(int clientid); //电流反极性
void CheckERC_3010(int clientid); 
void CheckERC_3031(int clientid); 
void CheckERC_3032(int clientid); 
void CheckERC_3033(int clientid); 
void CheckERC_3034(int clientid);
void CheckERC_3035(int clientid);
void CheckERC_3036(int clientid);
void CheckSelfERC(int clientid);       //自检

void  RtuExam(int clientid);
void  MetExam(int clientid);
uint8 GetEventCount(uint8 type);//-获取事件个数-
void  ClearAppXData(void);		//-清除外存记录-
void  ClearAppYData(void);		//-清除外存记录-
//<Start Common Problem>
void  ClearAppZData(void);


extern METER_HALT_STAT_STRUCT gMeterHalt_Info;

typedef struct //故障信息上报参数数据结构，上报按带时标的单点遥信上报
{
	uint32 		data_code;		//数据字典
	uint8		data_bit;	 	//当前数据存放的bit位
	uint8		state_bit;	 	//遥信是否变位存放bit位
} FAULT_REPORT_RAMINFO;

#endif

