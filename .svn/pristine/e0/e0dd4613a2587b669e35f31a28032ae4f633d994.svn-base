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
#ifndef _EVENT_H
#define _EVENT_H
#define ROUTE_MODULE_CHANGE_FILE     "route_module_change_file"
#define NODE_MODULE_CHANGE_FILE     "node_module_change_file"
#define COMMU_MODULE_CHANGE_FILE     "commu_module_change_file"




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

/***********************************************************************/
/*事件id定义结构体*/
/***********************************************************************/
typedef struct
{
	uint8 starttime[6];	//开始时间
	uint8 endtime[6];	//结束时间
	uint32 cap_start_pos_en;	//开表盖前正向有功电能
	uint32 cap_start_neg_en;	//开表盖前反向有功总电能
	uint32 cap_start_re_en_1;	//开表盖前第一象限无功总电能
	uint32 cap_start_re_en_2;	//开表盖前第二象限无功总电能
	uint32 cap_start_re_en_3;	//开表盖前第三象限无功总电能
	uint32 cap_start_re_en_4;	//开表盖前第四象限无功总电能
	uint32 cap_end_pos_en;		//开表盖后正向有功总电能
	uint32 cap_end_neg_en;		//开表盖后反向有功总电能
	uint32 cap_end_re_en_1;	//开表盖后第一象限无功总电能
	uint32 cap_end_re_en_2;	//开表盖后第二象限无功总电能
	uint32 cap_end_re_en_3;	//开表盖后第三象限无功总电能
	uint32 cap_end_re_en_4;	//开表盖后第四象限无功总电能
}MET_CAP_STRUCT;//开表盖记录结构体

typedef struct
{
	uint8 starttime[6];	//开始时间
	uint8 endtime[6];	//结束时间
	uint32 cover_start_pos_en;	//开表盖前正向有功电能
	uint32 cover_start_neg_en;	//开表盖前反向有功总电能
	uint32 cover_start_re_en_1;	//开表盖前第一象限无功总电能
	uint32 cover_start_re_en_2;	//开表盖前第二象限无功总电能
	uint32 cover_start_re_en_3;	//开表盖前第三象限无功总电能
	uint32 cover_start_re_en_4;	//开表盖前第四象限无功总电能
	uint32 cover_end_pos_en;		//开表盖后正向有功总电能
	uint32 cover_end_neg_en;		//开表盖后反向有功总电能
	uint32 cover_end_re_en_1;	//开表盖后第一象限无功总电能
	uint32 cover_end_re_en_2;	//开表盖后第二象限无功总电能
	uint32 cover_end_re_en_3;	//开表盖后第三象限无功总电能
	uint32 cover_end_re_en_4;	//开表盖后第四象限无功总电能
}MET_COVER_STRUCT;//开端钮盖记录结构体

typedef struct
{
	uint8 happentime[6];	//发生时刻
	uint32 actioncode;	//操作者代码
	uint8 code[40];	//编程的前10个数据标识码(不足补FFFFFFFFH)
}MET_PRG_STRUCT;//电表编程记录结构体

typedef struct
{
	uint8 starttime[6];	//开始时间
	uint8 endtime[6];	//结束时间
	uint32 lostvlt_pos_en;	//失压期间正向有功总电能增量
	uint32 lostvlt_neg_en;		//失压期间反向有功总电能增量
	uint32 lostvlt_re_en_zuhe1;	//失压期间组合无功1总电能增量
	uint32 lostvlt_re_en_zuhe2;	//失压期间组合无功2总电能增量

	uint32 lostvlt_a_pos_en;	//失压期间A相正向有功电能增量
	uint32 lostvlt_a_neg_en;	//失压期间A相反向有功电能增量
	uint32 lostvlt_a_re_en_zuhe1;	//失压期间A相组合无功1电能增量
	uint32 lostvlt_a_re_en_zuhe2;	//失压期间A相组合无功2电能增量
	uint16	lostvlt_a;							//失压时刻A相电压
	uint8 lostcur_a[3];							//失压时刻A相电流
	uint8 lost_a_ae_pow[3];	//失压时刻A相有功功率
	uint8 lost_a_re_pow[3];		//失压时刻A相无功功率
	uint16 lost_a_pow;				//失压时刻A相功率因数

	uint32 lostvlt_b_pos_en;	//失压期间B相正向有功电能增量
	uint32 lostvlt_b_neg_en;	//失压期间B相反向有功电能增量
	uint32 lostvlt_b_re_en_zuhe1;	//失压期间B相组合无功1电能增量
	uint32 lostvlt_b_re_en_zuhe2;	//失压期间B相组合无功2电能增量
	uint16	lostvlt_b;							//失压时刻B相电压
	uint8 lostcur_b[3];							//失压时刻B相电流
	uint8 lost_b_ae_pow[3];	//失压时刻B相有功功率
	uint8 lost_b_re_pow[3];		//失压时刻B相无功功率
	uint16 lost_b_pow;				//失压时刻B相功率因数

	uint32 lostvlt_c_pos_en;	//失压期间C相正向有功电能增量
	uint32 lostvlt_c_neg_en;	//失压期间C相反向有功电能增量
	uint32 lostvlt_c_re_en_zuhe1;	//失压期间C相组合无功1电能增量
	uint32 lostvlt_c_re_en_zuhe2;	//失压期间C相组合无功2电能增量
	uint16	lostvlt_c;							//失压时刻C相电压
	uint8 lostcur_c[3];							//失压时刻C相电流
	uint8 lost_c_ae_pow[3];	//失压时刻C相有功功率
	uint8 lost_c_re_pow[3];		//失压时刻C相无功功率
	uint16 lost_c_pow;				//失压时刻C相功率因数

	uint32 lost_total_ah;				//失压期间总安时数
	uint32 lost_a_ah;					//失压期间A相安时数
	uint32 lost_b_ah;					//失压期间B相安时数
	uint32 lost_c_ah;					//失压期间C相安时数

}LOST_VLT_STRUCT;

typedef struct
{
	uint8 starttime[6];	//开始时间
	uint8 endtime[6];	//结束时间
	uint32 breakvlt_pos_en;	//断相期间正向有功总电能增量
	uint32 breakvlt_neg_en;		//断相期间反向有功总电能增量
	uint32 breakvlt_re_en_zuhe1;	//断相期间组合无功1总电能增量
	uint32 breakvlt_re_en_zuhe2;	//断相期间组合无功2总电能增量

	uint32 breakvlt_a_pos_en;	//断相期间A相正向有功电能增量
	uint32 breakvlt_a_neg_en;	//断相期间A相反向有功电能增量
	uint32 breakvlt_a_re_en_zuhe1;	//断相期间A相组合无功1电能增量
	uint32 breakvlt_a_re_en_zuhe2;	//断相期间A相组合无功2电能增量
	uint16	breakvlt_a;							//断相时刻A相电压
	uint8 breakcur_a[3];							//断相时刻A相电流
	uint8 break_a_ae_pow[3];	//断相时刻A相有功功率
	uint8 break_a_re_pow[3];		//断相时刻A相无功功率
	uint16 break_a_pow;				//断相时刻A相功率因数

	uint32 breakvlt_b_pos_en;	//断相期间B相正向有功电能增量
	uint32 breakvlt_b_neg_en;	//断相期间B相反向有功电能增量
	uint32 breakvlt_b_re_en_zuhe1;	//断相期间B相组合无功1电能增量
	uint32 breakvlt_b_re_en_zuhe2;	//断相期间B相组合无功2电能增量
	uint16	breakvlt_b;							//断相时刻B相电压
	uint8 breakcur_b[3];							//断相时刻B相电流
	uint8 break_b_ae_pow[3];	//断相时刻B相有功功率
	uint8 break_b_re_pow[3];		//断相时刻B相无功功率
	uint16 break_b_pow;				//断相时刻B相功率因数

	uint32 breakvlt_c_pos_en;	//断相期间C相正向有功电能增量
	uint32 breakvlt_c_neg_en;	//断相期间C相反向有功电能增量
	uint32 breakvlt_c_re_en_zuhe1;	//断相期间C相组合无功1电能增量
	uint32 breakvlt_c_re_en_zuhe2;	//断相期间C相组合无功2电能增量
	uint16	breakvlt_c;							//断相时刻C相电压
	uint8 breakcur_c[3];							//断相时刻C相电流
	uint8 break_c_ae_pow[3];	//断相时刻C相有功功率
	uint8 break_c_re_pow[3];		//断相时刻C相无功功率
	uint16 break_c_pow;				//断相时刻C相功率因数

	uint32 break_total_ah;				//断相期间总安时数
	uint32 break_a_ah;					//断相期间A相安时数
	uint32 break_b_ah;					//断相期间B相安时数
	uint32 break_c_ah;					//断相期间C相安时数
}BREAK_VLT_STRUCT;

typedef struct
{
	uint8 starttime[6];	//开始时间
	uint8 endtime[6];	//结束时间
	uint32 overvlt_pos_en;	//过压期间正向有功总电能增量
	uint32 overvlt_neg_en;		//过压期间反向有功总电能增量
	uint32 overvlt_re_en_zuhe1;	//过压期间组合无功1总电能增量
	uint32 overvlt_re_en_zuhe2;	//过压期间组合无功2总电能增量

	uint32 overvlt_a_pos_en;	//过压期间A相正向有功电能增量
	uint32 overvlt_a_neg_en;	//过压期间A相反向有功电能增量
	uint32 overvlt_a_re_en_zuhe1;	//过压期间A相组合无功1电能增量
	uint32 overvlt_a_re_en_zuhe2;	//过压期间A相组合无功2电能增量
	uint16	overvlt_a;							//过压时刻A相电压
	uint8 overcur_a[3];							//过压时刻A相电流
	uint8 over_a_ae_pow[3];	//过压时刻A相有功功率
	uint8 over_a_re_pow[3];		//过压时刻A相无功功率
	uint16 over_a_pow;				//过压时刻A相功率因数

	uint32 overvlt_b_pos_en;	//过压期间B相正向有功电能增量
	uint32 overvlt_b_neg_en;	//过压期间B相反向有功电能增量
	uint32 overvlt_b_re_en_zuhe1;	//过压期间B相组合无功1电能增量
	uint32 overvlt_b_re_en_zuhe2;	//过压期间B相组合无功2电能增量
	uint16	overvlt_b;							//过压时刻B相电压
	uint8 overcur_b[3];							//过压时刻B相电流
	uint8 over_b_ae_pow[3];	//过压时刻B相有功功率
	uint8 over_b_re_pow[3];		//过压时刻B相无功功率
	uint16 over_b_pow;				//过压时刻B相功率因数

	uint32 overvlt_c_pos_en;	//过压期间C相正向有功电能增量
	uint32 overvlt_c_neg_en;	//过压期间C相反向有功电能增量
	uint32 overvlt_c_re_en_zuhe1;	//过压期间C相组合无功1电能增量
	uint32 overvlt_c_re_en_zuhe2;	//过压期间C相组合无功2电能增量
	uint16	overvlt_c;							//过压时刻C相电压
	uint8 overcur_c[3];							//过压时刻C相电流
	uint8 over_c_ae_pow[3];	//过压时刻C相有功功率
	uint8 over_c_re_pow[3];		//过压时刻C相无功功率
	uint16 over_c_pow;				//过压时刻C相功率因数

	uint32 over_total_ah;				//过压期间总安时数
	uint32 over_a_ah;					//过压期间A相安时数
	uint32 over_b_ah;					//过压期间B相安时数
	uint32 over_c_ah;					//过压期间C相安时数
}OVER_VLT_STRUCT;


typedef struct
{
	uint8 starttime[6];	//电压逆相序开始时间
	uint8 endtime[6];	//电压逆相序结束时间
	uint8 other[64];		//其他数据
}REVERSE_VLT_STRUCT;

typedef struct
{
	uint8 starttime[6];	//电流逆相序开始时间
	uint8 endtime[6];	//电流逆相序结束时间
	uint8 other[64];		//其他数据
}REVERSE_CUR_STRUCT;

typedef struct
{
	uint8 starttime[6];		//开始时间
	uint8 endtime[6];		//结束时间
	uint16 maxvltImb;			//最大不平衡率
	uint8 other[64];			//其它数据
}VLT_IMBALANCE_STRUCT;

typedef struct
{
	uint8 starttime[6];		//开始时间
	uint8 endtime[6];		//结束时间
	uint16 maxcurImb;			//最大不平衡率
	uint8 other[64];			//其它数据
}CUR_IMBALANCE_STRUCT;


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
//修改事件标记存储地址，存储事件由64测量点增加到2040测量点，时钟超差事件由64测量点增加到2040测量点，此次地址作废，新的存储地址在下方定义
#define EventStatusRecAddr		MinuteCountRecAddr+MinuteCountRecLen		//-全局变量，保存测量点事件标志记录-
#define EventStoragNumRecAddr3105	EventStatusRecAddr+EventMaxFlagLen	//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr3105	EventStoragNumRecAddr3105+EventMaxFlagLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragIndex3105		EventStoragTmRecAddr3105+EventMaxFlagLen	//-全局变量，保存测量点事件记录序号-
#define EventStoragNumRecAddr310A	EventStoragIndex3105+EventMaxFlagLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr310A	EventStoragNumRecAddr310A+EventStoragNumMntRecLen*20	//-全局变量，保存测量点事件开始时间-
#define EventStoragIndex310A	    EventStoragTmRecAddr310A+EventStoragMntRecLen*20		//-全局变量，保存测量点事件存储序号
#define EventStoragNumRecAddr310B	EventStoragIndex310A+EventStoragMntRecLen*20
#define EventStoragTmRecAddr310B	EventStoragNumRecAddr310B+EventMaxFlagLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragIndex310B		EventStoragTmRecAddr310B+EventMaxFlagLen	//-全局变量，保存测量点事件记录序号-
#define EventStoragNumRecAddr310C	EventStoragIndex310B+EventMaxFlagLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr310C	EventStoragNumRecAddr310C+EventMaxFlagLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragIndex310C		EventStoragTmRecAddr310C+EventMaxFlagLen	//-全局变量，保存测量点事件记录序号-
#define EventStoragNumRecAddr310D	EventStoragIndex310C+EventMaxFlagLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr310D	EventStoragNumRecAddr310D+EventMaxFlagLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragIndex310D		EventStoragTmRecAddr310D+EventMaxFlagLen	//-全局变量，保存测量点事件记录序号-
#define EventStoragNumRecAddr310E	EventStoragIndex310D+EventMaxFlagLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr310E	EventStoragNumRecAddr310E+EventMaxFlagLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragIndex310E		EventStoragTmRecAddr310E+EventMaxFlagLen	//-全局变量，保存测量点事件记录序号-
#define EventStoragNumRecAddr310F	EventStoragIndex310E+EventMaxFlagLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr310F	EventStoragNumRecAddr310F+EventMaxFlagLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragIndex310F		EventStoragTmRecAddr310F+EventMaxFlagLen	//-全局变量，保存测量点事件记录序号-
#define EventStoragNumRecAddr3119	EventStoragIndex310F+EventMaxFlagLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr3119	EventStoragNumRecAddr3119+EventMaxFlagLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragIndex3119		EventStoragTmRecAddr3119+EventMaxFlagLen	//-全局变量，保存测量点事件记录序号-
#define EventStoragNumRecAddr300F	EventStoragIndex3119+EventMaxFlagLen		//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr300F	EventStoragNumRecAddr300F+EventStoragNumMntRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr3110	EventStoragTmRecAddr300F+EventStoragMntRecLen	//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr3110	EventStoragNumRecAddr3110+EventStoragNumMntRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragNumRecAddr3115	EventStoragTmRecAddr3110+EventStoragMntRecLen	//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr3115	EventStoragNumRecAddr3115+EventStoragCtrlRecLen	//-全局变量，保存测量点事件开始时间-
#define EventStoragIndex3115	    EventStoragTmRecAddr3115+EventStoragCtrlRecLen	//-全局变量，保存测量点事件存储序号
#define EventStoragNumRecAddr3200	EventStoragIndex3115+EventStoragCtrlRecLen	//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr3200	EventStoragNumRecAddr3200+EventStoragCtrlRecLen*8	//-全局变量，保存测量点事件开始时间-8个总加组*8个轮次
#define EventStoragIndex3200	    EventStoragTmRecAddr3200+EventStoragCtrlRecLen*8	//-全局变量，保存测量点事件记录序号
#define EventStoragNumRecAddr3600	EventStoragIndex3200+EventStoragCtrlRecLen*8	//-全局变量，保存测量点事件存储序号
#define EventStoragTmRecAddr3600	EventStoragNumRecAddr3600+4*5	
#define EventStoragIndex3600		EventStoragTmRecAddr3600+4*5
#define EventStoragNumRecAddr3601	EventStoragIndex3600+4*5
#define EventStoragTmRecAddr3601	EventStoragNumRecAddr3601+4*5
#define EventStoragIndex3601		EventStoragTmRecAddr3601+4*5
#define EventStoragNumRecAddr3602	EventStoragIndex3601+4*5
#define EventStoragTmRecAddr3602	EventStoragNumRecAddr3602+4*5	
#define EventStoragIndex3602		EventStoragTmRecAddr3602+4*5


#define LostvltAddr				EventStoragIndex3602+4*5	//-失压次数-
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

#ifdef AREA_HUNAN
#define SmokeStateLen               1
#define WaterStateLen               1
#define SmokeStateAddr          (MetBCFailAddr+MetCBFailLen)
#define WaterStateAddr          (SmokeStateAddr+SmokeStateLen)
#define FluxOverTopAddr         (WaterStateAddr+WaterStateLen)
#else
#define FluxOverTopAddr         (MetBCFailAddr + MetCBFailLen)				//通讯流量超标已记录标识
#endif

#if 1
#define ControlLoopStatusAddr   FluxOverTopAddr + FluxOverTopLen          //控制回路状态
#define TimeSettingAddr			ControlLoopStatusAddr + MetCBFailLen
#else
#define MetBMagneticAbNormalAddr	MetBCFailAddr+MetCBFailLen							// 磁场事件异常的记录
#define ControlLoopStatusAddr   	MetBMagneticAbNormalAddr + MetBMagneticAbNormalLen  	// 控制回路状态

//#define ControlLoopStatusAddr   MetBCFailAddr + MetCBFailLen  //控制回路状态
#define TimeSettingAddr			ControlLoopStatusAddr + MetCBFailLen			// 主要保存上次校时总次数
#endif

#ifdef AREA_FUJIAN
#define EventStopRpt			TimeSettingAddr + TimeSettingLen		//终端停电事件上报标记
#define EventStopTm				EventStopRpt + EventStopRptLen			//终端停电后持续工作时间超过3分钟标记
#define EventExamLen			EventStopTm + EventStopRptLen	
#else
#define EventExamLen			TimeSettingAddr + TimeSettingLen		//-全局变量，保存测量点事件标志记录-
#endif



#define OutIRecLen				sizeof(RECODE_SAVE_T)*256		//-重要事件记录-
#define OutNRecLen				sizeof(RECODE_SAVE_T)*256		//-一般事件记录-
#define OutRecCountLen			4	//-申请四个字节，顺序存放重要事件个数，重要事件存储位置，一般事件个数，一般事件存储位置-
#define RemoteRecLen			sizeof(T_REMOTE_REC)*5			//-遥控记录-
#define PowerRecLen				sizeof(T_POWER_REC)*5			//-功控记录-
#define BuyRecLen				sizeof(T_BUYEN_REC)*5			//-购电控记录-
#define DayEnRecLen				sizeof(T_DAYEN_REC)*5			//-日电控记录-
#define MonEnRecLen				sizeof(T_MONEN_REC)*5			//-月电控记录-
#define PowerDownRecLen			sizeof(T_POWERDOWN_REC)*5		//-失电记录-
#define ProgDownRecLen			sizeof(OOP_DATETIME_S_T)		//-停电时间-
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
#define EventStoragNumMntRecLen  	4							//-全局变量，保存终端事件存储序号,
#define EventStoragMntRecLen	    4							//-全局变量，保存终端事件开始时间,
#define EventStoragCtrlRecLen		4*8							//控制事件，8轮

#define EventMaxFlagLen		NO_VIR_MTR_MAX_NUM*4				//-全局变量，所有测量点事件

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
#define TmnPowerOffLen			1								//-终端停电事件发生标志-
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

#define EventStopRptLen			1

//北京地区停上电事件特殊要求
typedef struct
{
    uint8  D0       :1;     //D0置"1"：事件正常，D0置"0"：事件异常，此处是对停电时间有效间隔的判断
    uint8  D1       :1;	    //D1置"1"：事件有效，D1置"0"：事件无效，此处是对停电时间偏差限值的判断
    uint8  D2_D7    :6;     //预留
}__attribute__ ((packed)) TMN_HALT_INFO_STR;


//编程事件
#define     PS_ERR           -1        //编程失败
#define     PS_ON             0        //编程中
#define     PS_OK             1        //编程成功


/*-******************************************************************-*/
/*-                             函数                                 -*/
/*-************************************** ** ************************-*/
int get_evt_dbclient();

uint32 Uint16To32(uint16 front, uint16 rear);
void MntVersionCheck(OOP_VERSION_T tVersion);
int GetEventNumAndIndex(int clientid, uint32 eventOAD, uint16* nEventNum, uint32* nIndex);


int get_meter_data_by_oad(int clientid, uint16 infoNum, uint32 oadvalue, uint8 *poutbuf, uint32 max_len);
int GetEventNomalParam(int clientid, uint8 logicId, uint16 infoNum, uint32 value);

void InitEventPara(void);

void  evt_check_3100(int clientid);
void  evt_check_3101(int clientid);
void CheckERC_3104(DB_CLIENT fd, OOP_SWITCHIN_STATE_T switchTemp);			        //-状态量变位检测、记录函数-
void CheckERC_3119(mtrnum_t mtr_id);	//-电流回路异常记录函数-

void evt_check_3106(void);			        //-终端停/上电事件检测、记录函数-
void CheckERC_310A(mtrnum_t mtr_id);        //-终端故障检测、记录函数
void evt_check_310F(mtrnum_t mtr_id, OOP_METER_T oopMeter);	//-终端485抄表失败事件检测、记录函数-
int evt_check_3105(int clientid,uint8 logicid,uint16 mtr_id, OOP_METER_T oopMeter,uint8* databuf,uint16 datalen);	//-电能表时间超差记录函数-
int evt_check_310X(int clientid,uint8 logicid,uint16 mtr_id, OOP_METER_T oopMeter,uint8* databuf,uint16 datalen); //电能表电量检测事件 301B电能表示度下降 ,310C电能量超差 ,310D电能表飞走 ,310E电能表停走
int evt_check_3110(int clientid,uint8 logicid,uint16 mtr_id, OOP_METER_T oopMeter,uint8* databuf,uint16 datalen);//终端与主站通信流量超门限事件记录
int evt_check_300F(); //终端相序异常

void evt_check_311C(mtrnum_t mtr_id, OOP_METER_T oopMeter);
int update_perceptionterminal_info(int clientid, uint8 logicId, uint16 infoNum, uint32 value);

void RtuExam(void);
void MetExam(mtrnum_t mtr_id, OOP_METER_T oopMeter);

#endif


