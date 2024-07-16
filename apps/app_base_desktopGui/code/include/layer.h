/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
  程序名称: main.h
  功    能：FKGA42显示模块框160*160页面库头文件，用于声明页面库变量
  开 发 人：gaoyf
  开发时间：2007-1
  
  版本历史:
  
    v1.0:
      2006-1-19, gaoyf : New file        
  
***************************************************************************
*/

#ifndef _LAYER_H_
#define _LAYER_H_

extern PLUGIN_INFO g_puamr;
extern PLUGIN_INFO g_stamr;

extern PLUGIN_INFO g_Template1;			// 背景风格库
extern PLUGIN_INFO g_WelcomeLayer;			// 0xFFFFFE
extern PLUGIN_INFO g_layerPopMsg;			// 0xFFFFFD
#if proAdvanceEN2 == 1
extern PLUGIN_INFO g_CommucationParaResetLayer;	// 通讯参数初始化       0x00FFFFFC
#endif
// 菜单页面库
extern PLUGIN_INFO g_menuLayer;         // 1列菜单风格  0x00A00000
extern PLUGIN_INFO g_menu2Layer;        // 2列菜单风格  0x00A10000
extern PLUGIN_INFO g_menu3Layer;        // 3列菜单风格  0x00A50000

extern PLUGIN_INFO g_layerInput;        // 输入法用户界面  0x00a20000
extern PLUGIN_INFO g_layerTimeArea;     // 输入时间范围界面0x00a30000
extern PLUGIN_INFO g_RemoteConStatusDraw;

extern PLUGIN_INFO g_menuMeasureLayer;  // Measure菜单风格  0x00A20000

extern PLUGIN_INFO g_layerInput;        // 输入法用户界面  0x00a30000
extern PLUGIN_INFO g_layerTimeArea;     // 输入时间范围界面0x00a40000
#if HARDWARE_2012_EN ==1
extern PLUGIN_INFO g_SetparaLayer; 
extern PLUGIN_INFO g_Set_ParaResetTime;
extern PLUGIN_INFO g_TerminalAddr;
extern PLUGIN_INFO g_ChangePwd;
#endif
extern PLUGIN_INFO g_layerSetGPRSMode;	//GPRS网络模式 0x21B70088

extern PLUGIN_INFO g_smiOSLogExport;	  //日志导出 0x00A000F1
extern PLUGIN_INFO g_smiOSUUpdate;	    //U盘升级 0x00A000F0

extern PLUGIN_INFO g_SSHSet_Layer;	    //SSH服务设置 0x00A000D0
extern PLUGIN_INFO g_TCPDump_Set_Layer;	//TCPDump服务设置 0x00A000D1
extern PLUGIN_INFO g_passwd_Layer;	    //恢复默认密码 0x00A000F2
extern PLUGIN_INFO g_LogClear_Layer;    //日志清除 0x00A000F3
extern PLUGIN_INFO g_ModuleUpdate;      //模组升级 0x00A000F4
extern PLUGIN_INFO g_SystemCmd;         //System命令 0x00A000F5
extern PLUGIN_INFO g_EchoPrint;         //回显打印 0x10A000F5
extern PLUGIN_INFO g_ACModuleUpdate;    //交采模块升级 0x00A000F6
extern PLUGIN_INFO g_CoUpdate;          //交采模块升级 0x00A000F6
extern PLUGIN_INFO g_layer_bluetooth;   //设置蓝牙开关
extern PLUGIN_INFO g_layer_CPU_temperature;    //CPU当前温度
extern PLUGIN_INFO g_layer_power_status;       //电源供电状态
extern PLUGIN_INFO g_layer_ac_ver;             //交采版本
extern PLUGIN_INFO g_layer_co_ver;             //协处理器版本显示
extern PLUGIN_INFO g_layer_comm_proxy;         //湖南通信代理配置


/************************************************************************/
/* 实时信息                                                           */
/************************************************************************/
extern PLUGIN_INFO g_RemoteConStatusDraw;       // 遥控状态 
extern PLUGIN_INFO g_CurrStatusDraw;            // 控制状态

extern PLUGIN_INFO g_RepstopConStatusDraw1;       // 报停控状态
extern PLUGIN_INFO g_RepstopConStatusDraw2;       // 报停控状态
extern PLUGIN_INFO g_RepstopConStatusDraw3;       // 报停控状态
extern PLUGIN_INFO g_RepstopConStatusDraw4;       // 报停控状态
extern PLUGIN_INFO g_RepstopConStatusDraw5;       // 报停控状态
extern PLUGIN_INFO g_RepstopConStatusDraw6;       // 报停控状态
extern PLUGIN_INFO g_RepstopConStatusDraw7;       // 报停控状态
extern PLUGIN_INFO g_RepstopConStatusDraw8;       // 报停控状态

extern PLUGIN_INFO g_OverflowConStatusDraw1;       // 下浮控状态
extern PLUGIN_INFO g_OverflowConStatusDraw2;       // 下浮控状态
extern PLUGIN_INFO g_OverflowConStatusDraw3;       // 下浮控状态
extern PLUGIN_INFO g_OverflowConStatusDraw4;       // 下浮控状态
extern PLUGIN_INFO g_OverflowConStatusDraw5;       // 下浮控状态
extern PLUGIN_INFO g_OverflowConStatusDraw6;       // 下浮控状态
extern PLUGIN_INFO g_OverflowConStatusDraw7;       // 下浮控状态
extern PLUGIN_INFO g_OverflowConStatusDraw8;       // 下浮控状态

extern PLUGIN_INFO g_RestConStatusDraw1;       // 厂休控状态
extern PLUGIN_INFO g_RestConStatusDraw2;       // 厂休控状态
extern PLUGIN_INFO g_RestConStatusDraw3;       // 厂休控状态
extern PLUGIN_INFO g_RestConStatusDraw4;       // 厂休控状态
extern PLUGIN_INFO g_RestConStatusDraw5;       // 厂休控状态
extern PLUGIN_INFO g_RestConStatusDraw6;       // 厂休控状态
extern PLUGIN_INFO g_RestConStatusDraw7;       // 厂休控状态
extern PLUGIN_INFO g_RestConStatusDraw8;       // 厂休控状态

extern PLUGIN_INFO g_TimesConStatusDraw1;       // 时段控状态
extern PLUGIN_INFO g_TimesConStatusDraw2;       // 时段控状态
extern PLUGIN_INFO g_TimesConStatusDraw3;       // 时段控状态
extern PLUGIN_INFO g_TimesConStatusDraw4;       // 时段控状态
extern PLUGIN_INFO g_TimesConStatusDraw5;       // 时段控状态
extern PLUGIN_INFO g_TimesConStatusDraw6;       // 时段控状态
extern PLUGIN_INFO g_TimesConStatusDraw7;       // 时段控状态
extern PLUGIN_INFO g_TimesConStatusDraw8;       // 时段控状态

extern PLUGIN_INFO g_MonthConStatusDraw1;       // 月电控状态
extern PLUGIN_INFO g_MonthConStatusDraw2;       // 月电控状态
extern PLUGIN_INFO g_MonthConStatusDraw3;       // 月电控状态
extern PLUGIN_INFO g_MonthConStatusDraw4;       // 月电控状态
extern PLUGIN_INFO g_MonthConStatusDraw5;       // 月电控状态
extern PLUGIN_INFO g_MonthConStatusDraw6;       // 月电控状态
extern PLUGIN_INFO g_MonthConStatusDraw7;       // 月电控状态
extern PLUGIN_INFO g_MonthConStatusDraw8;       // 月电控状态

extern PLUGIN_INFO g_BuyConStatusDraw1;       // 购电控状态
extern PLUGIN_INFO g_BuyConStatusDraw2;       // 购电控状态
extern PLUGIN_INFO g_BuyConStatusDraw3;       // 购电控状态
extern PLUGIN_INFO g_BuyConStatusDraw4;       // 购电控状态
extern PLUGIN_INFO g_BuyConStatusDraw5;       // 购电控状态
extern PLUGIN_INFO g_BuyConStatusDraw6;       // 购电控状态
extern PLUGIN_INFO g_BuyConStatusDraw7;       // 购电控状态
extern PLUGIN_INFO g_BuyConStatusDraw8;       // 购电控状态
extern PLUGIN_INFO g_BuyDataDraw1;	// 购电信息	0x00010100
extern PLUGIN_INFO g_BuyDataDraw2;	// 购电信息	0x00010200
extern PLUGIN_INFO g_BuyDataDraw3;	// 购电信息	0x00010300
extern PLUGIN_INFO g_BuyDataDraw4;	// 购电信息	0x00010400
extern PLUGIN_INFO g_BuyDataDraw5;	// 购电信息	0x00010500
extern PLUGIN_INFO g_BuyDataDraw6;	// 购电信息	0x00010600
extern PLUGIN_INFO g_BuyDataDraw7;	// 购电信息	0x00010700
extern PLUGIN_INFO g_BuyDataDraw8;	// 购电信息	0x00010800

extern PLUGIN_INFO g_TermStatusDraw1;       // 终端状态
extern PLUGIN_INFO g_TermStatusDraw2;       // 终端状态
extern PLUGIN_INFO g_TermStatusDraw3;       // 终端状态
extern PLUGIN_INFO g_TermStatusDraw4;       // 终端状态
extern PLUGIN_INFO g_TermStatusDraw5;       // 终端状态
extern PLUGIN_INFO g_TermStatusDraw6;       // 终端状态
extern PLUGIN_INFO g_TermStatusDraw7;       // 终端状态
extern PLUGIN_INFO g_TermStatusDraw8;       // 终端状态

extern PLUGIN_INFO g_ControlStatusDraw1;	// 控制状态	0x00010101
extern PLUGIN_INFO g_ControlStatusDraw2;	// 控制状态	0x00010201
extern PLUGIN_INFO g_ControlStatusDraw3;	// 控制状态	0x00010301
extern PLUGIN_INFO g_ControlStatusDraw4;	// 控制状态	0x00010401
extern PLUGIN_INFO g_ControlStatusDraw5;	// 控制状态	0x00010501
extern PLUGIN_INFO g_ControlStatusDraw6;	// 控制状态	0x00010601
extern PLUGIN_INFO g_ControlStatusDraw7;	// 控制状态	0x00010701
extern PLUGIN_INFO g_ControlStatusDraw8;	// 控制状态	0x00010801

extern PLUGIN_INFO g_TermTimeDateDraw;       // 终端日期时间
extern PLUGIN_INFO g_RemConSwitchStatusDrawDraw;       // 遥信开关状态
extern PLUGIN_INFO g_TermVersionDraw;       // 终端版本
extern PLUGIN_INFO g_TermVersionDraw3;       // 终端版本
extern PLUGIN_INFO g_UpdateStatusDraw;       // 升级状态 0x000e0000
#if TH208_RTU == 1
extern PLUGIN_INFO g_IUpdateStatusDraw;       // 升级状态 0x000e0001
#endif
#if HARDWARE_TYPE == 4
extern PLUGIN_INFO g_UpdateRouterInfo;
#endif

#if MODULARIZATION_TMN_EN == 1
extern PLUGIN_INFO g_UpdateMdlInfo;
#endif

extern PLUGIN_INFO g_AutoCheckStatusDraw;       // 终端自检状态

extern PLUGIN_INFO g_ChannelDebugStatusDraw2;       // 通道调试信息
extern PLUGIN_INFO g_ChannelDebugStatusDraw3;       // 通道调试信息
extern PLUGIN_INFO g_ChannelDebugStatusDraw4;       // 通道调试信息
extern PLUGIN_INFO g_ChannelDebugStatusDraw5;       // 通道调试信息
extern PLUGIN_INFO g_ChannelDebugStatusDraw6;       // 通道调试信息
#if COMM_DEBUG_PRO == 1
extern PLUGIN_INFO g_ChannelDebugStatusDraw7;       // 通道调试信息
extern PLUGIN_INFO g_ChannelDebugStatusDraw8;       // 通道调试信息
#if ANHUI_FUN == 1
extern PLUGIN_INFO g_ChannelDebugStatusDraw9;       // 通道调试信息
#endif
#endif

extern PLUGIN_INFO g_CbStatusDraw;						// 当前抄表状态
extern PLUGIN_INFO g_CbMtStatusDraw1;					// 测量点抄表状态
extern PLUGIN_INFO g_CbMtStatusDraw2;					// 测量点抄表状态
extern PLUGIN_INFO g_CbMtStatusDraw3;					// 测量点抄表成功率





/************************************************************************/
/* 测量点数据                                                           */
/************************************************************************/
extern PLUGIN_INFO g_CurElecDraw10;         // 0x00500000 当前电量
extern PLUGIN_INFO g_PreMonSupplyDraw1;     // 0x00510000 上月电量
extern PLUGIN_INFO g_CurElecDraw11;	        // 0x00520000 四象限无功电能
extern PLUGIN_INFO g_layerJc1Power;		//0x005AA000  正向有功
extern PLUGIN_INFO g_layerJc2Power;		//0x005AAA00 反相有功
extern PLUGIN_INFO g_CurSupplyDraw1;        // 0x00530000 当前需量
extern PLUGIN_INFO g_MaxSupplyDraw1;	    // 0x00540000 最大需量
extern PLUGIN_INFO g_PreMonMaxSupplyDraw1;	// 0x00550000 上月最大需量
extern PLUGIN_INFO g_layerVoltagEelectric1; // 0x00560000 电压电流
extern PLUGIN_INFO g_layerPower1;	        // 0x00570000 功率及功率因数
extern PLUGIN_INFO g_layerJCPower1;	        // 0x00580001 交采有功功率
extern PLUGIN_INFO g_layerJCWuPower1;	    // 0x00580002 交采无功功率
extern PLUGIN_INFO g_layerJCPowerFactor1;	// 0x00590000 功率因数
extern PLUGIN_INFO g_layerPhaseAngle1;      // 0x005A0000 相位角数据
extern PLUGIN_INFO g_CurElecQCur1;		    // 0x005E0000 当前电能示数
extern PLUGIN_INFO g_CurElecQCur2;		    // 0x005E0000 当前电能示数
extern PLUGIN_INFO g_SyElecQCur1;		    // 0x005D0000 上月电能示数
extern PLUGIN_INFO g_RdjElecQCur;			// 0x005F0000 日冻结电能示数

extern PLUGIN_INFO g_VltHrPec_A;    // A相电压谐波含有率
extern PLUGIN_INFO g_VltHrPec_B;    // B相电压谐波含有率
extern PLUGIN_INFO g_VltHrPec_C;    // C相电压谐波含有率
extern PLUGIN_INFO g_CurHrPec_A;    // A相电流谐波含有率
extern PLUGIN_INFO g_CurHrPec_B;    // B相电流谐波含有率
extern PLUGIN_INFO g_CurHrPec_C;    // C相电流谐波含有率

extern PLUGIN_INFO g_VltHrVal_A;    // A相电压谐波有效值
extern PLUGIN_INFO g_VltHrVal_B;    // B相电压谐波有效值
extern PLUGIN_INFO g_VltHrVal_C;    // C相电压谐波有效值
extern PLUGIN_INFO g_CurHrVal_A;    // A相电流谐波有效值
extern PLUGIN_INFO g_CurHrVal_B;    // B相电流谐波有效值
extern PLUGIN_INFO g_CurHrVal_C;    // C相电流谐波有效值

extern PLUGIN_INFO g_layerActCurve1;		// 有功功率曲线 0x00520106
extern PLUGIN_INFO g_layerActCurve2;		// 有功功率曲线 0x00520206
extern PLUGIN_INFO g_layerActCurve3;		// 有功功率曲线 0x00520306
extern PLUGIN_INFO g_layerActCurve4;		// 有功功率曲线 0x00520406
extern PLUGIN_INFO g_layerActCurve5;		// 有功功率曲线 0x00520506
extern PLUGIN_INFO g_layerActCurve6;		// 有功功率曲线 0x00520606
extern PLUGIN_INFO g_layerActCurve7;		// 有功功率曲线 0x00520706
extern PLUGIN_INFO g_layerActCurve8;		// 有功功率曲线 0x00520806

extern PLUGIN_INFO g_layerReactCurve1;		// 无功功率曲线 0x00520107
extern PLUGIN_INFO g_layerReactCurve2;		// 无功功率曲线 0x00520207
extern PLUGIN_INFO g_layerReactCurve3;		// 无功功率曲线 0x00520307
extern PLUGIN_INFO g_layerReactCurve4;		// 无功功率曲线 0x00520407
extern PLUGIN_INFO g_layerReactCurve5;		// 无功功率曲线 0x00520507
extern PLUGIN_INFO g_layerReactCurve6;		// 无功功率曲线 0x00520607
extern PLUGIN_INFO g_layerReactCurve7;		// 无功功率曲线 0x00520707
extern PLUGIN_INFO g_layerReactCurve8;		// 无功功率曲线 0x00520807


extern PLUGIN_INFO g_layerEquipVal1;	// 电能表示数 0x00520105
extern PLUGIN_INFO g_layerEquipVal2;	// 电能表示数 0x00520205
extern PLUGIN_INFO g_layerEquipVal3;	// 电能表示数 0x00520305
extern PLUGIN_INFO g_layerEquipVal4;	// 电能表示数 0x00520405
extern PLUGIN_INFO g_layerEquipVal5;	// 电能表示数 0x00520505
extern PLUGIN_INFO g_layerEquipVal6;	// 电能表示数 0x00520605
extern PLUGIN_INFO g_layerEquipVal7;	// 电能表示数 0x00520705
extern PLUGIN_INFO g_layerEquipVal8;	// 电能表示数 0x00520805
extern PLUGIN_INFO g_layerEquipVal9;	// 电能表示数 0x00520905
extern PLUGIN_INFO g_layerEquipVal10;	// 电能表示数 0x00520A05
extern PLUGIN_INFO g_layerEquipVal11;	// 电能表示数 0x00520B05
extern PLUGIN_INFO g_layerEquipVal12;	// 电能表示数 0x00520C05
extern PLUGIN_INFO g_layerEquipVal13;	// 电能表示数 0x00520D05
extern PLUGIN_INFO g_layerEquipVal14;	// 电能表示数 0x00520E05
extern PLUGIN_INFO g_layerEquipVal15;	// 电能表示数 0x00520F05
extern PLUGIN_INFO g_layerEquipVal16;	// 电能表示数 0x00521005

extern PLUGIN_INFO g_layerEquipValExt1;	// 电能表示数 0x00520108
extern PLUGIN_INFO g_layerEquipValExt2;	// 电能表示数 0x00520208
extern PLUGIN_INFO g_layerEquipValExt3;	// 电能表示数 0x00520308
extern PLUGIN_INFO g_layerEquipValExt4;	// 电能表示数 0x00520408
extern PLUGIN_INFO g_layerEquipValExt5;	// 电能表示数 0x00520508
extern PLUGIN_INFO g_layerEquipValExt6;	// 电能表示数 0x00520608
extern PLUGIN_INFO g_layerEquipValExt7;	// 电能表示数 0x00520708
extern PLUGIN_INFO g_layerEquipValExt8;	// 电能表示数 0x00520808
extern PLUGIN_INFO g_layerEquipValExt9;	// 电能表示数 0x00520908
extern PLUGIN_INFO g_layerEquipValExt10;	// 电能表示数 0x00520A08
extern PLUGIN_INFO g_layerEquipValExt11;	// 电能表示数 0x00520B08
extern PLUGIN_INFO g_layerEquipValExt12;	// 电能表示数 0x00520C08
extern PLUGIN_INFO g_layerEquipValExt13;	// 电能表示数 0x00520D08
extern PLUGIN_INFO g_layerEquipValExt14;	// 电能表示数 0x00520E08
extern PLUGIN_INFO g_layerEquipValExt15;	// 电能表示数 0x00520F08
extern PLUGIN_INFO g_layerEquipValExt16;	// 电能表示数 0x00521008


extern PLUGIN_INFO g_layerGroupData1;		// 总加组数据 0x00130100
extern PLUGIN_INFO g_layerGroupData2;		// 总加组数据 0x00130200
extern PLUGIN_INFO g_layerGroupData3;		// 总加组数据 0x00130300
extern PLUGIN_INFO g_layerGroupData4;		// 总加组数据 0x00130400
extern PLUGIN_INFO g_layerGroupData5;		// 总加组数据 0x00130500
extern PLUGIN_INFO g_layerGroupData6;		// 总加组数据 0x00130600
extern PLUGIN_INFO g_layerGroupData7;		// 总加组数据 0x00130700
extern PLUGIN_INFO g_layerGroupData8;		// 总加组数据 0x00130800

//////////////////////////////////////新加
extern PLUGIN_INFO g_layerGroupDataNow1;		// 总加组数据 0x00140100
extern PLUGIN_INFO g_layerGroupDataNow2;		// 总加组数据 0x00140200
extern PLUGIN_INFO g_layerGroupDataNow3;		// 总加组数据 0x00140300
extern PLUGIN_INFO g_layerGroupDataNow4;		// 总加组数据 0x00140400
extern PLUGIN_INFO g_layerGroupDataNow5;		// 总加组数据 0x00140500
extern PLUGIN_INFO g_layerGroupDataNow6;		// 总加组数据 0x00140600
extern PLUGIN_INFO g_layerGroupDataNow7;		// 总加组数据 0x00140700
extern PLUGIN_INFO g_layerGroupDataNow8;		// 总加组数据 0x00140800

#if 1//湖北专变
extern PLUGIN_INFO g_layerGroupDataPow1;		// 总加组1当前功率 0x00141100
extern PLUGIN_INFO g_layerGroupDataPow2;		// 总加组2当前功率 0x00141200
extern PLUGIN_INFO g_layerGroupDataPow3;		// 总加组3当前功率 0x00141300
extern PLUGIN_INFO g_layerGroupDataPow4;		// 总加组4当前功率 0x00141400

extern PLUGIN_INFO g_CurElecVir1;		// 总加组当日电量 0x00021100
extern PLUGIN_INFO g_CurElecVir2;		// 总加组当日电量 0x00021200
extern PLUGIN_INFO g_CurElecVir3;		// 总加组当日电量 0x00021300
extern PLUGIN_INFO g_CurElecVir4;		// 总加组当日电量 0x00021400

extern PLUGIN_INFO g_CurElecVirMon1;		// 总加组当月电量 0x00021101
extern PLUGIN_INFO g_CurElecVirMon2;		// 总加组当月电量 0x00021201
extern PLUGIN_INFO g_CurElecVirMon3;		// 总加组当月电量 0x00021301
extern PLUGIN_INFO g_CurElecVirMon4;		// 总加组当月电量 0x00021401
#endif

/************************************************************************/
/* 测量点数据结束                                                       */
/************************************************************************/


/************************************************************************/
/* 终端数据                                                             */
/************************************************************************/
extern PLUGIN_INFO g_layerTerminalAddrData;       // 终端地址 0x00100000
extern PLUGIN_INFO g_layerTerminalInfo;           // 终端信息 0x00100002
#if SHANDONG_FUN == 1 && HARDWARE_TYPE == 3
extern PLUGIN_INFO g_BatValInfo;				  //电池电压信息0x00100003
#endif
#if ANHUI_FUN == 1 
extern PLUGIN_INFO g_POSITIONINFO;                //设备地理位置信息0x000c0005
#endif
extern PLUGIN_INFO g_ESAMINFO;                    // ESAM信息 0x000c0001
#if SHANGHAI_FUN == 1
extern PLUGIN_INFO g_layerTerminalInfo1;            // 终端信息 0x00100010
extern PLUGIN_INFO g_layerTerminalInfo2;            // 终端信息 0x00100020
extern PLUGIN_INFO g_layerTerminalInfo3;            // 终端信息 0x00100030
extern PLUGIN_INFO g_layerTerminalInfo4;            // 终端信息 0x00100040
extern PLUGIN_INFO g_layerTerminalInfo5;            // 终端信息 0x00100050
extern PLUGIN_INFO g_layerTerminalInfo6;            // 终端信息 0x00100060
extern PLUGIN_INFO g_layerCommdisp;
extern PLUGIN_INFO g_layerLossVoltRec;              //失压统计 0x00830001
extern PLUGIN_INFO g_layerLossVoltNRec;             //最近N次失压记录 0x00830002
extern PLUGIN_INFO g_layerLossVoltRec1;	            //测量点失压统计页面1 0x00830003
extern PLUGIN_INFO g_layerLossVoltRec2; 	        //测量点失压统计页面2 0x00830004
extern PLUGIN_INFO g_layerLossVoltRec3;	            //测量点失压统计页面3 0x00830005		
extern PLUGIN_INFO g_layerLossVoltNRec1;	        //测量点最近失压记录1 0x00830006       
extern PLUGIN_INFO g_layerLossVoltNRec2;	        //测量点最近失压记录2 0x00830007
extern PLUGIN_INFO g_layerLossVoltNRec3;	        //测量点最近失压记录3 0x00830008
#endif

extern PLUGIN_INFO g_layerChnlInfoData;			    // 终端当前主备通道 0x00110000
extern PLUGIN_INFO g_layerGroupMainData;          // 总加组配置信息 0x00120000
extern PLUGIN_INFO g_layerTermRelay;		// 终端中继参数 0x00130000
extern PLUGIN_INFO g_layerTermGroupAddr;	// 终端组地址参数 0x00140000
extern PLUGIN_INFO g_layerSysReset;			// 系统复位 0x00610001
extern PLUGIN_INFO g_layerAbout;			// 关于对话框 0x00150000
extern PLUGIN_INFO g_layerEnergyVir;		// 电控累加 0x00160000
extern PLUGIN_INFO g_layerPowerVir;			// 功控累加 0x00160001
extern PLUGIN_INFO g_layerInduct;			// 互感器频率 0x00170000
#if COMMPARA_BAK_EN == 1
extern PLUGIN_INFO g_layerNetVirInfo;           // 虚拟专网信息 0x21B70000
#endif

#if PRTL_STACK_EXCHANGE_EN == 1
extern PLUGIN_INFO g_prtl_stack_exchange;
#endif
extern PLUGIN_INFO g_layerSetStartMechineModelTime;	//修改通信模块开机时序 0x21B70077
extern PLUGIN_INFO g_layersetSafeONOFF;//安全模式启用关闭0x000c0003
extern PLUGIN_INFO g_layersetFTP_TelnetONOFF;//FTP Telnet 功能开启关闭0x000c0004
#if SECURITY_DEFEND == 1
extern PLUGIN_INFO g_layerDefend;//FTP Telnet 功能开启关闭0x000c0004
extern PLUGIN_INFO g_layerAuthTime;
#endif
/************************************************************************/
/* 终端数据结束                                                         */
/************************************************************************/


/************************************************************************/
/* 控制参数                                                             */
/************************************************************************/
// 时段控参数

extern PLUGIN_INFO g_layerTimeCtl1;	   // 时段控参数 0x00200108
extern PLUGIN_INFO g_layerTimeCtl2;	   // 时段控参数 0x00200208
extern PLUGIN_INFO g_layerTimeCtl3;	   // 时段控参数 0x00200308
extern PLUGIN_INFO g_layerTimeCtl4;	   // 时段控参数 0x00200408
extern PLUGIN_INFO g_layerTimeCtl5;	   // 时段控参数 0x00200508
extern PLUGIN_INFO g_layerTimeCtl6;	   // 时段控参数 0x00200608
extern PLUGIN_INFO g_layerTimeCtl7;	   // 时段控参数 0x00200708
extern PLUGIN_INFO g_layerTimeCtl8;	   // 时段控参数 0x00200808

// 控制组1
extern PLUGIN_INFO g_layerTimeCtl11;       // 控制组1	0x00200100
extern PLUGIN_INFO g_layerTimeCtl12;	   // 控制组1	0x00200101
extern PLUGIN_INFO g_layerTimeCtl13;       // 控制组1	0x00200102
extern PLUGIN_INFO g_layerTimeCtl14;	   // 控制组1	0x00200103

// 控制组2
extern PLUGIN_INFO g_layerTimeCtl21;       // 控制组2	0x00200200
extern PLUGIN_INFO g_layerTimeCtl22;	   // 控制组2	0x00200201
extern PLUGIN_INFO g_layerTimeCtl23;       // 控制组2	0x00200202
extern PLUGIN_INFO g_layerTimeCtl24;	   // 控制组2	0x00200203

// 控制组3
extern PLUGIN_INFO g_layerTimeCtl31;       // 控制组3	0x00200300
extern PLUGIN_INFO g_layerTimeCtl32;	   // 控制组3	0x00200301
extern PLUGIN_INFO g_layerTimeCtl33;       // 控制组3	0x00200302
extern PLUGIN_INFO g_layerTimeCtl34;	   // 控制组3	0x00200303

// 控制组4
extern PLUGIN_INFO g_layerTimeCtl41;       // 控制组4	0x00200400
extern PLUGIN_INFO g_layerTimeCtl42;	   // 控制组4	0x00200401
extern PLUGIN_INFO g_layerTimeCtl43;       // 控制组4	0x00200402
extern PLUGIN_INFO g_layerTimeCtl44;	   // 控制组4	0x00200403

// 控制组5
extern PLUGIN_INFO g_layerTimeCtl51;       // 控制组5	0x00200500
extern PLUGIN_INFO g_layerTimeCtl52;	   // 控制组5	0x00200501
extern PLUGIN_INFO g_layerTimeCtl53;       // 控制组5	0x00200502
extern PLUGIN_INFO g_layerTimeCtl54;	   // 控制组5	0x00200503

// 控制组6
extern PLUGIN_INFO g_layerTimeCtl61;       // 控制组6	0x00200600
extern PLUGIN_INFO g_layerTimeCtl62;	   // 控制组6	0x00200601
extern PLUGIN_INFO g_layerTimeCtl63;       // 控制组6	0x00200602
extern PLUGIN_INFO g_layerTimeCtl64;	   // 控制组6	0x00200603

// 控制组7
extern PLUGIN_INFO g_layerTimeCtl71;       // 控制组7	0x00200700
extern PLUGIN_INFO g_layerTimeCtl72;	   // 控制组7	0x00200701
extern PLUGIN_INFO g_layerTimeCtl73;       // 控制组7	0x00200702
extern PLUGIN_INFO g_layerTimeCtl74;	   // 控制组7	0x00200703

// 控制组8
extern PLUGIN_INFO g_layerTimeCtl81;       // 控制组8	0x00200800
extern PLUGIN_INFO g_layerTimeCtl82;	   // 控制组8	0x00200801
extern PLUGIN_INFO g_layerTimeCtl83;       // 控制组8	0x00200802
extern PLUGIN_INFO g_layerTimeCtl84;	   // 控制组8	0x00200803

// 下浮控参数
extern PLUGIN_INFO g_layerLowCtrl1;	// 下浮控参数	0x00210100
extern PLUGIN_INFO g_layerLowCtrl2;	// 下浮控参数	0x00210200
extern PLUGIN_INFO g_layerLowCtrl3;	// 下浮控参数	0x00210300
extern PLUGIN_INFO g_layerLowCtrl4;	// 下浮控参数	0x00210400
extern PLUGIN_INFO g_layerLowCtrl5;	// 下浮控参数	0x00210500
extern PLUGIN_INFO g_layerLowCtrl6;	// 下浮控参数	0x00210600
extern PLUGIN_INFO g_layerLowCtrl7;	// 下浮控参数	0x00210700
extern PLUGIN_INFO g_layerLowCtrl8;	// 下浮控参数	0x00210800
extern PLUGIN_INFO g_layerLowsubCtrl1; // 下浮控参数	0x00230100 
extern PLUGIN_INFO g_layerLowsubCtrl2; // 下浮控参数	0x00230200 
extern PLUGIN_INFO g_layerLowsubCtrl3; // 下浮控参数	0x00230300 
extern PLUGIN_INFO g_layerLowsubCtrl4; // 下浮控参数	0x00230400 
extern PLUGIN_INFO g_layerLowsubCtrl5; // 下浮控参数	0x00230500 
extern PLUGIN_INFO g_layerLowsubCtrl6; // 下浮控参数	0x00230600 
extern PLUGIN_INFO g_layerLowsubCtrl7; // 下浮控参数	0x00230700 
extern PLUGIN_INFO g_layerLowsubCtrl8; // 下浮控参数	0x00230800 

// 厂休控参数
extern PLUGIN_INFO g_layerDayOff1;	// 厂休控参数 0x00220100
extern PLUGIN_INFO g_layerDayOff2;	// 厂休控参数 0x00220200
extern PLUGIN_INFO g_layerDayOff3;	// 厂休控参数 0x00220300
extern PLUGIN_INFO g_layerDayOff4;	// 厂休控参数 0x00220400
extern PLUGIN_INFO g_layerDayOff5;	// 厂休控参数 0x00220500
extern PLUGIN_INFO g_layerDayOff6;	// 厂休控参数 0x00220600
extern PLUGIN_INFO g_layerDayOff7;	// 厂休控参数 0x00220700
extern PLUGIN_INFO g_layerDayOff8;	// 厂休控参数 0x00220800

extern PLUGIN_INFO g_layerMonthCtl1;// 月电控参数 0x00240100
extern PLUGIN_INFO g_layerMonthCtl2;// 月电控参数 0x00240200
extern PLUGIN_INFO g_layerMonthCtl3;// 月电控参数 0x00240300
extern PLUGIN_INFO g_layerMonthCtl4;// 月电控参数 0x00240400

extern PLUGIN_INFO g_layerStopCtl1;	// 报停控参数 0x00290100
extern PLUGIN_INFO g_layerStopCtl2;	// 报停控参数 0x00290200
extern PLUGIN_INFO g_layerStopCtl3;	// 报停控参数 0x00290300
extern PLUGIN_INFO g_layerStopCtl4;	// 报停控参数 0x00290400

extern PLUGIN_INFO g_layerExesRate1;	// 电量费率时段 0x00270000
extern PLUGIN_INFO g_layerExesRate2;	// 电量费率时段 0x00270001
extern PLUGIN_INFO g_layerExesRate3;	// 电量费率时段 0x00270002
extern PLUGIN_INFO g_layerExesRate4;	// 电量费率时段 0x00270003
extern PLUGIN_INFO g_layerExesRate5;	// 电量费率时段 0x00270004
extern PLUGIN_INFO g_layerExesRate6;	// 电量费率时段 0x00270005

extern PLUGIN_INFO g_layerBuyCoulomb1;	// 购电控参数 0x00250100
extern PLUGIN_INFO g_layerBuyCoulomb2;	// 购电控参数 0x00250200
extern PLUGIN_INFO g_layerBuyCoulomb3;	// 购电控参数 0x00250300
extern PLUGIN_INFO g_layerBuyCoulomb4;	// 购电控参数 0x00250400

extern PLUGIN_INFO g_layerCallingExes;	// 催费控参数 0x00260000

/************************************************************************/
/* 控制参数结束                                                         */
/************************************************************************/

/************************************************************************/
/* 测量点参数                                                           */
/************************************************************************/
extern PLUGIN_INFO g_layerMeasureMainPara;	// 测量点参数 0x00440000

extern PLUGIN_INFO g_layerVarietyRate1; // 测量点变比参数 0x00400100
extern PLUGIN_INFO g_layerVarietyRate2; // 测量点变比参数 0x00400200
extern PLUGIN_INFO g_layerVarietyRate3; // 测量点变比参数 0x00400300
extern PLUGIN_INFO g_layerVarietyRate4; // 测量点变比参数 0x00400400
extern PLUGIN_INFO g_layerVarietyRate5; // 测量点变比参数 0x00400500
extern PLUGIN_INFO g_layerVarietyRate6; // 测量点变比参数 0x00400600
extern PLUGIN_INFO g_layerVarietyRate7; // 测量点变比参数 0x00400700
extern PLUGIN_INFO g_layerVarietyRate8; // 测量点变比参数 0x00400800
extern PLUGIN_INFO g_layerVarietyRate9; // 测量点变比参数 0x00400900
extern PLUGIN_INFO g_layerVarietyRate10;// 测量点变比参数 0x00400A00
extern PLUGIN_INFO g_layerVarietyRate11;// 测量点变比参数 0x00400B00
extern PLUGIN_INFO g_layerVarietyRate12;// 测量点变比参数 0x00400C00
extern PLUGIN_INFO g_layerVarietyRate13;// 测量点变比参数 0x00400D00
extern PLUGIN_INFO g_layerVarietyRate14;// 测量点变比参数 0x00400E00
extern PLUGIN_INFO g_layerVarietyRate15;// 测量点变比参数 0x00400F00
extern PLUGIN_INFO g_layerVarietyRate16;// 测量点变比参数 0x00401000
extern PLUGIN_INFO g_layerVartyRate1;// 测量点变比参数(显示测量点1~8的KiKvKp)  0x00400000
extern PLUGIN_INFO g_layerVartyRate2;// 测量点变比参数(显示测量点9~16的KiKvKp) 0x00400001

extern PLUGIN_INFO g_layerEligible1;	// 合格电压上下限 0x00420100
extern PLUGIN_INFO g_layerEligible2;	// 合格电压上下限 0x00420200
extern PLUGIN_INFO g_layerEligible3;	// 合格电压上下限 0x00420300
extern PLUGIN_INFO g_layerEligible4;	// 合格电压上下限 0x00420400
extern PLUGIN_INFO g_layerEligible5;	// 合格电压上下限 0x00420500
extern PLUGIN_INFO g_layerEligible6;	// 合格电压上下限 0x00420600
extern PLUGIN_INFO g_layerEligible7;	// 合格电压上下限 0x00420700
extern PLUGIN_INFO g_layerEligible8;	// 合格电压上下限 0x00420800
extern PLUGIN_INFO g_layerEligible9;	// 合格电压上下限 0x00420900
extern PLUGIN_INFO g_layerEligible10;	// 合格电压上下限 0x00420A00
extern PLUGIN_INFO g_layerEligible11;	// 合格电压上下限 0x00420B00
extern PLUGIN_INFO g_layerEligible12;	// 合格电压上下限 0x00420C00
extern PLUGIN_INFO g_layerEligible13;	// 合格电压上下限 0x00420D00
extern PLUGIN_INFO g_layerEligible14;	// 合格电压上下限 0x00420E00
extern PLUGIN_INFO g_layerEligible15;	// 合格电压上下限 0x00420F00
extern PLUGIN_INFO g_layerEligible16;	// 合格电压上下限 0x00421000
/************************************************************************/
/* 测量点参数结束                                                       */
/************************************************************************/

/************************************************************************/
/* 测量点数据		                                                    */
/************************************************************************/
extern PLUGIN_INFO g_layerMeasureMainData;	//测量点数据0x00440008
extern PLUGIN_INFO g_layerMeasureMainDataExt;	//测量点数据0x00440009
/************************************************************************/
/* 测量点数据结束                                                       */
/************************************************************************/


/************************************************************************/
/* 参数配置                                                             */
/************************************************************************/
// 终端数据
extern PLUGIN_INFO g_layerProtoInfo;      // 通讯规约设置 0x00660009
extern PLUGIN_INFO g_layerChnlInfo;       // 通讯通道设置 0x00660000
extern PLUGIN_INFO g_layerGprsInfo;     //   GPRS设置    0x00660001
//extern PLUGIN_INFO g_layerNoteInfo;     //   短信 0x00660002
#if GPRS_ETH_ALL_OLINE == 1
extern PLUGIN_INFO g_layerEthCommPara;    // 终端以太网通信通道参数 0x00666001
#endif

extern PLUGIN_INFO g_layerMeasureMain;	  // 测量点配置 0x00640000
extern PLUGIN_INFO g_layerGroupMain;	  // 总加组配置 0x00670000
extern PLUGIN_INFO g_layerPulseMain;	  // 脉冲测量点配置 0x00650000

#if TIME_CTRL_ON_LINE_EXT == 1
extern PLUGIN_INFO g_TimeCtlOnLine;
#endif 

// 配置页面库
extern PLUGIN_INFO g_layerValidatePwd;  // 密码验证 0x00600000
extern PLUGIN_INFO g_layerChangePwd;      // 变更密码 0x00600001
extern PLUGIN_INFO g_layerFreezeTime;	// 验证冻结 0x00600002
extern PLUGIN_INFO g_layerResetTime;      // 终端时间设置 0x00620000
extern PLUGIN_INFO g_layerTerminalAddr;   // 终端地址  0x00100000
#if CHONG_QING_FUN == 1
extern PLUGIN_INFO g_layerSetUsernamePwd;   // 终端地址  0x00100000
#endif

extern PLUGIN_INFO g_layerResetTerm;      // 复位终端命令 0x00610000
extern PLUGIN_INFO g_layerStReinstall;      // 软件重新安装 0x00680000
extern PLUGIN_INFO g_layerLogDump;			// 系统日志转存 0x00680002
extern PLUGIN_INFO g_layerTrunDisp;		 // 轮显页面设置 0x00690000


extern PLUGIN_INFO g_layerMeasureAddr;	 // 表计地址设置 0x00690000
extern PLUGIN_INFO g_layerMeasureTime;	 // 表计校时设置 0x00690000
extern PLUGIN_INFO g_layerRemoteCtlMain; // 遥信开关设置 0x006C0000
extern PLUGIN_INFO g_layerRemoteCtlMain2; // 遥信开关设置 0x006C0001
extern PLUGIN_INFO g_layerCfgMonthCtl;	 // 月电控设置 0x006D0000
extern PLUGIN_INFO g_layerCfgTimeCtl;	 // 时段控设置 0x006E0000
extern PLUGIN_INFO g_layerIPConfig_eth0;	 // IPConfig 0x006F0000
extern PLUGIN_INFO g_layerIPConfig_eth1;	 // IPConfig 0x006F0010
extern PLUGIN_INFO g_layerDefaultGW;	 // 设置默认网关 0x006F0010
extern PLUGIN_INFO g_layerInductConfig1;// A相互感器频率 0x00B10000
extern PLUGIN_INFO g_layerInductConfig2;// B相互感器频率 0x00B10001
extern PLUGIN_INFO g_layerInductConfig3;// C相互感器频率 0x00B10002
extern PLUGIN_INFO g_layerTermRetset;	// 重起终端设备 0x00B30000
extern PLUGIN_INFO g_layerClearPara;	// 参数初始化 0x00B40000
extern PLUGIN_INFO g_layerClearACMeterData; // 交采电表清零   0x00B42000
extern PLUGIN_INFO g_layerInitACMeterPara;  // 交采全面初始化 0x00B43000
extern PLUGIN_INFO g_layerClearLog;		// 清除日志 0x00B50000
extern PLUGIN_INFO g_layerBackup;		// 备份近期数据 0x00B60000
#if NEW_GW_GUI_APPEND == 1
extern PLUGIN_INFO g_layerExtendBackup;	// 新国网备份 0x00B50100
#endif
#if PLC_AMR_EN == 1
extern PLUGIN_INFO g_layerPlcVenderCodeAndVersionInfo; // 载波模块厂商代码及版本信息 0x00B5000B
#endif

#if MODULARIZATION_TMN_EN == 1
extern PLUGIN_INFO g_layerMdlVenderCodeAndVersionInfo;
#endif

extern PLUGIN_INFO g_layerRadioCfg;		// 电台参数设置 0x20B70000
extern PLUGIN_INFO g_layerXjyAcCfg;		//巡检仪交采电源设置
/************************************************************************/
/* 参数配置结束                                                         */
/************************************************************************/

// 调试页面库
extern PLUGIN_INFO g_layerCustomDebug;  // 自定义调试信息   0x000d0000

/************************************************************************/
/* 任务配置参数                                                         */
/************************************************************************/
extern PLUGIN_INFO g_layerTaskPara11;	// 任务配置1-1	0x003000100
extern PLUGIN_INFO g_layerTaskPara12;	// 任务配置1-2	0x003000200
extern PLUGIN_INFO g_layerTaskPara13;	// 任务配置1-3	0x003000300
extern PLUGIN_INFO g_layerTaskPara14;	// 任务配置1-4	0x003000400
extern PLUGIN_INFO g_layerTaskPara15;	// 任务配置1-5	0x003000500
extern PLUGIN_INFO g_layerTaskPara16;	// 任务配置1-6	0x003000600
extern PLUGIN_INFO g_layerTaskPara17;	// 任务配置1-7	0x003000700
extern PLUGIN_INFO g_layerTaskPara18;	// 任务配置1-8	0x003000800

extern PLUGIN_INFO g_layerTaskPara19;	// 任务配置1-9	0x003000900
extern PLUGIN_INFO g_layerTaskPara1A;	// 任务配置1-10	0x003000A00
extern PLUGIN_INFO g_layerTaskPara1B;	// 任务配置1-11	0x003000B00
extern PLUGIN_INFO g_layerTaskPara1C;	// 任务配置1-12	0x003000C00
extern PLUGIN_INFO g_layerTaskPara1D;	// 任务配置1-13	0x003000D00
extern PLUGIN_INFO g_layerTaskPara1E;	// 任务配置1-14	0x003000E00
extern PLUGIN_INFO g_layerTaskPara1F;	// 任务配置1-15	0x003000F00
extern PLUGIN_INFO g_layerTaskPara110;	// 任务配置1-16	0x003001000

extern PLUGIN_INFO g_layerTaskPara111;	// 任务配置1-17	0x003001100
extern PLUGIN_INFO g_layerTaskPara112;	// 任务配置1-18	0x003002200
extern PLUGIN_INFO g_layerTaskPara113;	// 任务配置1-19	0x003003300
extern PLUGIN_INFO g_layerTaskPara114;	// 任务配置1-20	0x003004400
extern PLUGIN_INFO g_layerTaskPara115;	// 任务配置1-21	0x003005500
extern PLUGIN_INFO g_layerTaskPara116;	// 任务配置1-22	0x003006600
extern PLUGIN_INFO g_layerTaskPara117;	// 任务配置1-23	0x003007700
extern PLUGIN_INFO g_layerTaskPara118;	// 任务配置1-24	0x003008800

extern PLUGIN_INFO g_layerTaskPara119;	// 任务配置1-25	0x003001900
extern PLUGIN_INFO g_layerTaskPara11A;	// 任务配置1-26	0x003001A00
extern PLUGIN_INFO g_layerTaskPara11B;	// 任务配置1-27	0x003001B00
extern PLUGIN_INFO g_layerTaskPara11C;	// 任务配置1-28	0x003001C00
extern PLUGIN_INFO g_layerTaskPara11D;	// 任务配置1-29	0x003001D00
extern PLUGIN_INFO g_layerTaskPara11E;	// 任务配置1-30	0x003001E00
extern PLUGIN_INFO g_layerTaskPara11F;	// 任务配置1-31	0x003001F00
extern PLUGIN_INFO g_layerTaskPara120;	// 任务配置1-32	0x003002000

extern PLUGIN_INFO g_layerTaskPara121;	// 任务配置1-33	0x003002100
extern PLUGIN_INFO g_layerTaskPara122;	// 任务配置1-34	0x003002200
extern PLUGIN_INFO g_layerTaskPara123;	// 任务配置1-35	0x003002300
extern PLUGIN_INFO g_layerTaskPara124;	// 任务配置1-36	0x003002400
extern PLUGIN_INFO g_layerTaskPara125;	// 任务配置1-37	0x003002500
extern PLUGIN_INFO g_layerTaskPara126;	// 任务配置1-38	0x003002600
extern PLUGIN_INFO g_layerTaskPara127;	// 任务配置1-39	0x003002700
extern PLUGIN_INFO g_layerTaskPara128;	// 任务配置1-40	0x003002800

extern PLUGIN_INFO g_layerTaskPara129;	// 任务配置1-41	0x003002900
extern PLUGIN_INFO g_layerTaskPara12A;	// 任务配置1-42	0x003002A00
extern PLUGIN_INFO g_layerTaskPara12B;	// 任务配置1-43	0x003002B00
extern PLUGIN_INFO g_layerTaskPara12C;	// 任务配置1-44	0x003002C00
extern PLUGIN_INFO g_layerTaskPara12D;	// 任务配置1-45	0x003002D00
extern PLUGIN_INFO g_layerTaskPara12E;	// 任务配置1-46	0x003002E00
extern PLUGIN_INFO g_layerTaskPara12F;	// 任务配置1-47	0x003002F00
extern PLUGIN_INFO g_layerTaskPara130;	// 任务配置1-48	0x003003000

extern PLUGIN_INFO g_layerTaskPara131;	// 任务配置1-49	0x003003100
extern PLUGIN_INFO g_layerTaskPara132;	// 任务配置1-50	0x003003200
extern PLUGIN_INFO g_layerTaskPara133;	// 任务配置1-51	0x003003300
extern PLUGIN_INFO g_layerTaskPara134;	// 任务配置1-52	0x003003400
extern PLUGIN_INFO g_layerTaskPara135;	// 任务配置1-53	0x003003500
extern PLUGIN_INFO g_layerTaskPara136;	// 任务配置1-54	0x003003600
extern PLUGIN_INFO g_layerTaskPara137;	// 任务配置1-55	0x003003700
extern PLUGIN_INFO g_layerTaskPara138;	// 任务配置1-56	0x003003800

extern PLUGIN_INFO g_layerTaskPara139;	// 任务配置1-57	0x003003900
extern PLUGIN_INFO g_layerTaskPara13A;	// 任务配置1-58	0x003003A00
extern PLUGIN_INFO g_layerTaskPara13B;	// 任务配置1-59	0x003003B00
extern PLUGIN_INFO g_layerTaskPara13C;	// 任务配置1-60	0x003003C00
extern PLUGIN_INFO g_layerTaskPara13D;	// 任务配置1-61	0x003003D00
extern PLUGIN_INFO g_layerTaskPara13E;	// 任务配置1-62	0x003003E00
extern PLUGIN_INFO g_layerTaskPara13F;	// 任务配置1-63	0x003003F00
extern PLUGIN_INFO g_layerTaskPara140;	// 任务配置1-64	0x003004000
///----------------------------------------------------------------


extern PLUGIN_INFO g_layerTaskPara21;	// 任务配置2-1	0x003000101
extern PLUGIN_INFO g_layerTaskPara22;	// 任务配置2-2	0x003000201
extern PLUGIN_INFO g_layerTaskPara23;	// 任务配置2-3	0x003000301
extern PLUGIN_INFO g_layerTaskPara24;	// 任务配置2-4	0x003000401
extern PLUGIN_INFO g_layerTaskPara25;	// 任务配置2-5	0x003000501
extern PLUGIN_INFO g_layerTaskPara26;	// 任务配置2-6	0x003000601
extern PLUGIN_INFO g_layerTaskPara27;	// 任务配置2-7	0x003000701
extern PLUGIN_INFO g_layerTaskPara28;	// 任务配置2-8	0x003000801

extern PLUGIN_INFO g_layerTaskPara29;	// 任务配置2-9	0x003000901
extern PLUGIN_INFO g_layerTaskPara2A;	// 任务配置2-10	0x003000A01
extern PLUGIN_INFO g_layerTaskPara2B;	// 任务配置2-11	0x003000B01
extern PLUGIN_INFO g_layerTaskPara2C;	// 任务配置2-12	0x003000C01
extern PLUGIN_INFO g_layerTaskPara2D;	// 任务配置2-13	0x003000D01
extern PLUGIN_INFO g_layerTaskPara2E;	// 任务配置2-14	0x003000E01
extern PLUGIN_INFO g_layerTaskPara2F;	// 任务配置2-15	0x003000F01
extern PLUGIN_INFO g_layerTaskPara210;	// 任务配置2-16	0x003001001

extern PLUGIN_INFO g_layerTaskPara211;	// 任务配置2-17	0x003001101
extern PLUGIN_INFO g_layerTaskPara212;	// 任务配置2-18	0x003002201
extern PLUGIN_INFO g_layerTaskPara213;	// 任务配置2-19	0x003003301
extern PLUGIN_INFO g_layerTaskPara214;	// 任务配置2-20	0x003004401
extern PLUGIN_INFO g_layerTaskPara215;	// 任务配置2-21	0x003005501
extern PLUGIN_INFO g_layerTaskPara216;	// 任务配置2-22	0x003006601
extern PLUGIN_INFO g_layerTaskPara217;	// 任务配置2-23	0x003007701
extern PLUGIN_INFO g_layerTaskPara218;	// 任务配置2-24	0x003008801

extern PLUGIN_INFO g_layerTaskPara219;	// 任务配置2-25	0x003001901
extern PLUGIN_INFO g_layerTaskPara21A;	// 任务配置2-26	0x003001A01
extern PLUGIN_INFO g_layerTaskPara21B;	// 任务配置2-27	0x003001B01
extern PLUGIN_INFO g_layerTaskPara21C;	// 任务配置2-28	0x003001C01
extern PLUGIN_INFO g_layerTaskPara21D;	// 任务配置2-29	0x003001D01
extern PLUGIN_INFO g_layerTaskPara21E;	// 任务配置2-30	0x003001E01
extern PLUGIN_INFO g_layerTaskPara21F;	// 任务配置2-31	0x003001F01
extern PLUGIN_INFO g_layerTaskPara220;	// 任务配置2-32	0x003002001

extern PLUGIN_INFO g_layerTaskPara221;	// 任务配置2-33	0x003002101
extern PLUGIN_INFO g_layerTaskPara222;	// 任务配置2-34	0x003002201
extern PLUGIN_INFO g_layerTaskPara223;	// 任务配置2-35	0x003002301
extern PLUGIN_INFO g_layerTaskPara224;	// 任务配置2-36	0x003002401
extern PLUGIN_INFO g_layerTaskPara225;	// 任务配置2-37	0x003002501
extern PLUGIN_INFO g_layerTaskPara226;	// 任务配置2-38	0x003002601
extern PLUGIN_INFO g_layerTaskPara227;	// 任务配置2-39	0x003002701
extern PLUGIN_INFO g_layerTaskPara228;	// 任务配置2-40	0x003002801

extern PLUGIN_INFO g_layerTaskPara229;	// 任务配置2-41	0x003002901
extern PLUGIN_INFO g_layerTaskPara22A;	// 任务配置2-42	0x003002A01
extern PLUGIN_INFO g_layerTaskPara22B;	// 任务配置2-43	0x003002B01
extern PLUGIN_INFO g_layerTaskPara22C;	// 任务配置2-44	0x003002C01
extern PLUGIN_INFO g_layerTaskPara22D;	// 任务配置2-45	0x003002D01
extern PLUGIN_INFO g_layerTaskPara22E;	// 任务配置2-46	0x003002E01
extern PLUGIN_INFO g_layerTaskPara22F;	// 任务配置2-47	0x003002F01
extern PLUGIN_INFO g_layerTaskPara230;	// 任务配置2-48	0x003003001

extern PLUGIN_INFO g_layerTaskPara231;	// 任务配置2-49	0x003003101
extern PLUGIN_INFO g_layerTaskPara232;	// 任务配置2-50	0x003003201
extern PLUGIN_INFO g_layerTaskPara233;	// 任务配置2-51	0x003003301
extern PLUGIN_INFO g_layerTaskPara234;	// 任务配置2-52	0x003003401
extern PLUGIN_INFO g_layerTaskPara235;	// 任务配置2-53	0x003003501
extern PLUGIN_INFO g_layerTaskPara236;	// 任务配置2-54	0x003003601
extern PLUGIN_INFO g_layerTaskPara237;	// 任务配置2-55	0x003003701
extern PLUGIN_INFO g_layerTaskPara238;	// 任务配置2-56	0x003003801

extern PLUGIN_INFO g_layerTaskPara239;	// 任务配置2-57	0x003003901
extern PLUGIN_INFO g_layerTaskPara23A;	// 任务配置2-58	0x003003A01
extern PLUGIN_INFO g_layerTaskPara23B;	// 任务配置2-59	0x003003B01
extern PLUGIN_INFO g_layerTaskPara23C;	// 任务配置2-60	0x003003C01
extern PLUGIN_INFO g_layerTaskPara23D;	// 任务配置2-61	0x003003D01
extern PLUGIN_INFO g_layerTaskPara23E;	// 任务配置2-62	0x003003E01
extern PLUGIN_INFO g_layerTaskPara23F;	// 任务配置2-63	0x003003F01
extern PLUGIN_INFO g_layerTaskPara240;	// 任务配置2-64	0x003004001

/////////////////
extern PLUGIN_INFO g_layerTaskPara31;	// 任务配置3-1	0x003000102
extern PLUGIN_INFO g_layerTaskPara32;	// 任务配置3-2	0x003000202
extern PLUGIN_INFO g_layerTaskPara33;	// 任务配置3-3	0x003000302
extern PLUGIN_INFO g_layerTaskPara34;	// 任务配置3-4	0x003000402
extern PLUGIN_INFO g_layerTaskPara35;	// 任务配置3-5	0x003000502
extern PLUGIN_INFO g_layerTaskPara36;	// 任务配置3-6	0x003000602
extern PLUGIN_INFO g_layerTaskPara37;	// 任务配置3-7	0x003000702
extern PLUGIN_INFO g_layerTaskPara38;	// 任务配置3-8	0x003000802

extern PLUGIN_INFO g_layerTaskPara39;	// 任务配置3-9	0x003000902
extern PLUGIN_INFO g_layerTaskPara3A;	// 任务配置3-10	0x003000A02
extern PLUGIN_INFO g_layerTaskPara3B;	// 任务配置3-11	0x003000B02
extern PLUGIN_INFO g_layerTaskPara3C;	// 任务配置3-12	0x003000C02
extern PLUGIN_INFO g_layerTaskPara3D;	// 任务配置3-13	0x003000D02
extern PLUGIN_INFO g_layerTaskPara3E;	// 任务配置3-14	0x003000E02
extern PLUGIN_INFO g_layerTaskPara3F;	// 任务配置3-15	0x003000F02
extern PLUGIN_INFO g_layerTaskPara310;	// 任务配置3-16	0x003001002

extern PLUGIN_INFO g_layerTaskPara311;	// 任务配置3-17	0x003001102
extern PLUGIN_INFO g_layerTaskPara312;	// 任务配置3-18	0x003002202
extern PLUGIN_INFO g_layerTaskPara313;	// 任务配置3-19	0x003003302
extern PLUGIN_INFO g_layerTaskPara314;	// 任务配置3-20	0x003004402
extern PLUGIN_INFO g_layerTaskPara315;	// 任务配置3-21	0x003005502
extern PLUGIN_INFO g_layerTaskPara316;	// 任务配置3-22	0x003006602
extern PLUGIN_INFO g_layerTaskPara317;	// 任务配置3-23	0x003007702
extern PLUGIN_INFO g_layerTaskPara318;	// 任务配置3-24	0x003008802

extern PLUGIN_INFO g_layerTaskPara319;	// 任务配置3-25	0x003001902
extern PLUGIN_INFO g_layerTaskPara31A;	// 任务配置3-26	0x003001A02
extern PLUGIN_INFO g_layerTaskPara31B;	// 任务配置3-27	0x003001B02
extern PLUGIN_INFO g_layerTaskPara31C;	// 任务配置3-28	0x003001C02
extern PLUGIN_INFO g_layerTaskPara31D;	// 任务配置3-29	0x003001D02
extern PLUGIN_INFO g_layerTaskPara31E;	// 任务配置3-30	0x003001E02
extern PLUGIN_INFO g_layerTaskPara31F;	// 任务配置3-31	0x003001F02
extern PLUGIN_INFO g_layerTaskPara320;	// 任务配置3-32	0x003002002

extern PLUGIN_INFO g_layerTaskPara321;	// 任务配置3-33	0x003002102
extern PLUGIN_INFO g_layerTaskPara322;	// 任务配置3-34	0x003002202
extern PLUGIN_INFO g_layerTaskPara323;	// 任务配置3-35	0x003002302
extern PLUGIN_INFO g_layerTaskPara324;	// 任务配置3-36	0x003002402
extern PLUGIN_INFO g_layerTaskPara325;	// 任务配置3-37	0x003002502
extern PLUGIN_INFO g_layerTaskPara326;	// 任务配置3-38	0x003002602
extern PLUGIN_INFO g_layerTaskPara327;	// 任务配置3-39	0x003002702
extern PLUGIN_INFO g_layerTaskPara328;	// 任务配置3-40	0x003002802

extern PLUGIN_INFO g_layerTaskPara329;	// 任务配置3-41	0x003002902
extern PLUGIN_INFO g_layerTaskPara32A;	// 任务配置3-42	0x003002A02
extern PLUGIN_INFO g_layerTaskPara32B;	// 任务配置3-43	0x003002B02
extern PLUGIN_INFO g_layerTaskPara32C;	// 任务配置3-44	0x003002C02
extern PLUGIN_INFO g_layerTaskPara32D;	// 任务配置3-45	0x003002D02
extern PLUGIN_INFO g_layerTaskPara32E;	// 任务配置3-46	0x003002E02
extern PLUGIN_INFO g_layerTaskPara32F;	// 任务配置3-47	0x003002F02
extern PLUGIN_INFO g_layerTaskPara330;	// 任务配置3-48	0x003003002

extern PLUGIN_INFO g_layerTaskPara331;	// 任务配置3-49	0x003003102
extern PLUGIN_INFO g_layerTaskPara332;	// 任务配置3-50	0x003003202
extern PLUGIN_INFO g_layerTaskPara333;	// 任务配置3-51	0x003003302
extern PLUGIN_INFO g_layerTaskPara334;	// 任务配置3-52	0x003003402
extern PLUGIN_INFO g_layerTaskPara335;	// 任务配置3-53	0x003003502
extern PLUGIN_INFO g_layerTaskPara336;	// 任务配置3-54	0x003003602
extern PLUGIN_INFO g_layerTaskPara337;	// 任务配置3-55	0x003003702
extern PLUGIN_INFO g_layerTaskPara338;	// 任务配置3-56	0x003003802

extern PLUGIN_INFO g_layerTaskPara339;	// 任务配置3-57	0x003003902
extern PLUGIN_INFO g_layerTaskPara33A;	// 任务配置3-58	0x003003A02
extern PLUGIN_INFO g_layerTaskPara33B;	// 任务配置3-59	0x003003B02
extern PLUGIN_INFO g_layerTaskPara33C;	// 任务配置3-60	0x003003C02
extern PLUGIN_INFO g_layerTaskPara33D;	// 任务配置3-61	0x003003D02
extern PLUGIN_INFO g_layerTaskPara33E;	// 任务配置3-62	0x003003E02
extern PLUGIN_INFO g_layerTaskPara33F;	// 任务配置3-63	0x003003F02
extern PLUGIN_INFO g_layerTaskPara340;	// 任务配置3-64	0x003004002
/************************************************************************/
/* 任务配置参数结束                                                     */
/************************************************************************/

/************************************************************************/
/* 告警页面库                                                           */
/************************************************************************/
extern PLUGIN_INFO g_layerAlarm1;	// 时段控8个轮次的告警状态	0x00900000
extern PLUGIN_INFO g_layerAlarm2;	// 厂休控8个轮次的告警状态	0x00900001
extern PLUGIN_INFO g_layerAlarm3;	// 实时控8个轮次的告警状态	0x00900002
extern PLUGIN_INFO g_layerAlarm4;	// 营业报停控8个轮次的告警状态	0x00900003
extern PLUGIN_INFO g_layerAlarm5;	// 日电控8个轮次的告警状态	0x00900004
extern PLUGIN_INFO g_layerAlarm6;	// 月电控8个轮次的告警状态	0x00900005
extern PLUGIN_INFO g_layerAlarm7;	// 购电控8个轮次的告警状态	0x00900006
extern PLUGIN_INFO g_layerAlarm8;	// 遥控8个轮次的告警状态	0x00900007
extern PLUGIN_INFO g_layerAlarm9;	// 总控制8个轮次的告警状态	0x00900008
extern PLUGIN_INFO g_layerAlarmA;	// 时段控8个轮次的跳闸状态	0x00900009
extern PLUGIN_INFO g_layerAlarmB;	// 厂休控8个轮次的跳闸状态	0x0090000A
extern PLUGIN_INFO g_layerAlarmC;	// 实时控8个轮次的跳闸状态	0x0090000B
extern PLUGIN_INFO g_layerAlarmD;	// 营业报停控8个轮次的跳闸状态	0x0090000C
extern PLUGIN_INFO g_layerAlarmE;	// 日电控8个轮次的跳闸状态	0x0090000D
extern PLUGIN_INFO g_layerAlarmF;	// 月电控8个轮次的跳闸状态	0x0090000E
extern PLUGIN_INFO g_layerAlarm10;	// 购电控8个轮次的跳闸状态	0x0090000F
extern PLUGIN_INFO g_layerAlarm11;	// 遥控8个轮次的跳闸状态	0x00900010
extern PLUGIN_INFO g_layerAlarm12;	// 总控制8个轮次的跳闸状态	0x00900011
/************************************************************************/
/* 告警页面库结束                                                       */
/************************************************************************/

/************************************************************************/
/* 事件记录页面库                                                       */
/************************************************************************/
extern PLUGIN_INFO g_layerEventI1;	// 重要事件 0x00910101
extern PLUGIN_INFO g_layerEventI2;	// 重要事件 0x00910102
extern PLUGIN_INFO g_layerEventI3;	// 重要事件 0x00910103
extern PLUGIN_INFO g_layerEventI4;	// 重要事件 0x00910104
extern PLUGIN_INFO g_layerEventI5;	// 重要事件 0x00910105

extern PLUGIN_INFO g_layerEventN1;	// 一般事件 0x00910201
extern PLUGIN_INFO g_layerEventN2;	// 一般事件 0x00910202
extern PLUGIN_INFO g_layerEventN3;	// 一般事件 0x00910203
extern PLUGIN_INFO g_layerEventN4;	// 一般事件 0x00910204
extern PLUGIN_INFO g_layerEventN5;	// 一般事件 0x00910205
/************************************************************************/
/* 事件记录页面库结束                                                   */
/************************************************************************/


/************************************************************************/
/* 中文信息页面库                                                           */
/************************************************************************/
extern PLUGIN_INFO g_ZhMsgInfoDraw1;		// 中文信息1
extern PLUGIN_INFO g_ZhMsgInfoDraw2;		// 中文信息2
extern PLUGIN_INFO g_ZhMsgInfoDraw3;		// 中文信息3
extern PLUGIN_INFO g_ZhMsgInfoDraw4;		// 中文信息4	
extern PLUGIN_INFO g_ZhMsgInfoDraw5;		// 中文信息5	
extern PLUGIN_INFO g_ZhMsgInfoDraw6;		// 中文信息6	
extern PLUGIN_INFO g_ZhMsgInfoDraw7;		// 中文信息7	
extern PLUGIN_INFO g_ZhMsgInfoDraw8;		// 中文信息8	
extern PLUGIN_INFO g_ZhMsgInfoDraw9;		// 中文信息9	
extern PLUGIN_INFO g_ZhMsgInfoDraw10;	// 中文信息10	

/************************************************************************/
/* 中文信息页面库结束                                                       */
/************************************************************************/

#if ANOLOGY_EN == 1
extern PLUGIN_INFO g_anologystatus;		// 直流模拟量 0x00021000
#endif

extern PLUGIN_INFO g_layerRs232IoInfo;	//本地232通讯参数	0x02222003
extern PLUGIN_INFO g_layerSet485;     //设置485 II口的功能，确认是抄表口还是被抄口

#if TH208_RTU == 1
extern PLUGIN_INFO g_layerPhotoManual;	//手工拍照	0x00B61000
extern PLUGIN_INFO g_layerPhotoDump;	//照片转存	0x00B61001
extern PLUGIN_INFO g_layerClearPhoto;	//清除照片	0x00B61002
#endif

#if GUI_CLEAR_MTRDATA == 1
extern PLUGIN_INFO g_layerClearData;           // 数据初始化0x00B41000
#endif

extern PLUGIN_INFO g_layerCurPower;           // 各路脉冲功率0x00010001
extern PLUGIN_INFO g_layerSwitchStatus;           // 开关状态
extern PLUGIN_INFO g_layerPowerLog1;           // 功控记录0x00040000
extern PLUGIN_INFO g_layerPowerLog2;           // 功控记录0x00040001
extern PLUGIN_INFO g_layerElecLog1;           // 购电控记录0x00050000
extern PLUGIN_INFO g_layerElecLog2;           // 购电控记录0x00050001
extern PLUGIN_INFO g_layermonthLog1;           //月电控记录0x00090000
extern PLUGIN_INFO g_layermonthLog2;           //月电控记录0x00090001


extern PLUGIN_INFO g_layerRemoteLog1;           // 遥控记录0x00060000
extern PLUGIN_INFO g_layerRemoteLog2;           // 遥控记录0x00060001
extern PLUGIN_INFO g_layerRemoteLog3;           // 遥控记录0x00060002
extern PLUGIN_INFO g_layerRemoteLog4;           // 遥控记录0x00060003
extern PLUGIN_INFO g_layerLosteleLog1;           // 失电记录0x00070000
extern PLUGIN_INFO g_layerLosteleLog2;           // 失电记录0x00070001
extern PLUGIN_INFO g_layerLosteleLog3;           // 失电记录0x00070002
extern PLUGIN_INFO g_layerLosteleLog4;           // 失电记录0x00070003
extern PLUGIN_INFO g_layerLosteleLog5;           // 失电记录0x00070004

extern PLUGIN_INFO g_JcVoltagEelectricMsgDraw;  // 交流采样记录0x00990000
extern PLUGIN_INFO g_JcCurMsgDraw;           // 交流采样记录0x00990001
extern PLUGIN_INFO g_JcPAngleDraw;           // 交流采样记录0x00990002
extern PLUGIN_INFO g_JcPcosDraw;             // 交流采样记录0x00990003
extern PLUGIN_INFO g_JcZPowerDraw;             // 交流采样记录0x00990004
extern PLUGIN_INFO g_JcWPowerDraw;			   // 交流采样记录0x00990005
extern PLUGIN_INFO g_JcTPowerDraw;             // 交流采样记录0x00990006

extern PLUGIN_INFO g_CQKeyUserRead;
extern PLUGIN_INFO g_CQKeyUserActRead;
extern PLUGIN_INFO g_CQKeyUserReActRead;
extern PLUGIN_INFO g_CQKeyUserVolRead;
extern PLUGIN_INFO g_CQKeyUserCurRead;
extern PLUGIN_INFO g_MeterReadStat;
extern PLUGIN_INFO g_MeterReadFailStat;
extern PLUGIN_INFO g_GprsNumStat;

extern PLUGIN_INFO g_CQTotalEn;
extern PLUGIN_INFO g_CQFengEn;
extern PLUGIN_INFO g_CQPingEn;
extern PLUGIN_INFO g_CQGuEn;
extern PLUGIN_INFO g_CQJianEn;				// 0x0014000c 测量点有功(电能量)尖示值
extern PLUGIN_INFO g_CQSelectDate;			// 0x0015000c 选择抄表查询日期
extern PLUGIN_INFO g_CQMeasureParaRead;
extern PLUGIN_INFO g_CQCommunicationParaRead;
extern PLUGIN_INFO g_CQMeterParaRead;
extern PLUGIN_INFO g_CQRouterInfoQuery;	// (重庆集抄)中继电表信息查询 0x0023000c
extern PLUGIN_INFO g_CQRouterInfoQuery1;	// (重庆集抄)中继电表信息查询 0x0023000c
extern PLUGIN_INFO g_CQRouterInfoStat;	// (重庆集抄)中继电表信息统计 0x0043000c
#if HARDWARE_TYPE == 4
extern PLUGIN_INFO g_layerPlcRouteModeSet;  //设置路由模式
#endif
#if 211
extern PLUGIN_INFO g_layerPlcAutoCheck;
extern PLUGIN_INFO g_layerPlcInfoShow;
extern PLUGIN_INFO g_layerPlcInfoSet;
#if NEW_GW_GUI_APPEND == 1
extern PLUGIN_INFO g_layerPlcRelayTest;
#endif
#endif


extern PLUGIN_INFO g_layerJCSoftwareConfig;
extern PLUGIN_INFO g_layerJCSoftwareReconfig;
extern PLUGIN_INFO g_layerShowIpInfo;
#if REMOTE_VPN_EN == 1
extern PLUGIN_INFO g_RemoteVPN;         //终端远程VPN功能 0x0001BDB9
#endif

#if JIANGXI_FUN == 1 || (SHANDONG_FUN == 1 && HARDWARE_TYPE == 3)
extern PLUGIN_INFO g_ControlOutputTypeLay;
#endif

#if (SHANDONG_FUN == 1 && HARDWARE_TYPE == 3)
extern PLUGIN_INFO g_layerPulseWidthSet;
#endif

#if HUNAN_FUN == 1
extern PLUGIN_INFO g_layerJCPower_21;
extern PLUGIN_INFO g_layerJCPower_22;
extern PLUGIN_INFO g_layerJCPower_23;
extern PLUGIN_INFO g_layerVoltagEelectric_21;
extern PLUGIN_INFO g_layerVoltagEelectric_22;
extern PLUGIN_INFO g_layerVoltagEelectric_23;
extern PLUGIN_INFO g_layerVoltagEelectric_31;
extern PLUGIN_INFO g_layerVoltagEelectric_32;
extern PLUGIN_INFO g_layerVoltagEelectric_33;
extern PLUGIN_INFO g_layerPositive_Active_A;
extern PLUGIN_INFO g_layerPositive_Active_R;
#if PROTECTEC_EN == 1
//extern PLUGIN_INFO g_layerPrtlChange;
#endif
extern PLUGIN_INFO g_layerEsamInfo;
#endif

#if SHANDONG_FUN == 1
extern PLUGIN_INFO g_layerSpecialFuc;   // 电压压缩使能 0x21B70078
#endif

#if HARDWARE_TYPE == 4
extern PLUGIN_INFO g_ResetPlcModeuleRouter; //0x0020000c
extern PLUGIN_INFO g_WLNetCurSate;          //0x00200006
extern PLUGIN_INFO g_WLLstReadMetTime;      //0x00200005
extern PLUGIN_INFO g_WLNetFaultData;        //0x00200004
extern PLUGIN_INFO g_WLOnlineMetData;       //0x00200003
extern PLUGIN_INFO g_WLDownLoadMetData;     //0x00200002

extern PLUGIN_INFO g_layerResetPlcModule;   //0x4000000C
extern PLUGIN_INFO g_layerClearPlcModule;   //0x4000000d
extern PLUGIN_INFO g_layerNetPlcModule;     //0x4000000e
extern PLUGIN_INFO g_layerCallPlcModule;    //0x4000000f
extern PLUGIN_INFO g_layerPlcNetStatInfo;
#endif
#if SHANGHAI_FUN == 1
/************************************************************************/
/* 失压记录页面库                                                           */
/************************************************************************/
extern PLUGIN_INFO g_layerLossVoltRec;	//失压统计 0x00830001
extern PLUGIN_INFO g_layerLossVoltNRec;	//最近N次失压记录 0x00830002

extern PLUGIN_INFO g_layerLossVoltRec1;	//测量点失压统计页面1 0x00830003
extern PLUGIN_INFO g_layerLossVoltRec2; 	//测量点失压统计页面2 0x00830004
extern PLUGIN_INFO g_layerLossVoltRec3;	//测量点失压统计页面3 0x00830005

extern PLUGIN_INFO g_layerLossVoltNRec1;	//测量点最近失压记录1 0x00830006
extern PLUGIN_INFO g_layerLossVoltNRec2;	//测量点最近失压记录2 0x00830007
extern PLUGIN_INFO g_layerLossVoltNRec3;	//测量点最近失压记录3 0x00830008

/************************************************************************/
/* 失压记录页面库                                                           */
/************************************************************************/
extern PLUGIN_INFO g_layerPhoneNumber;	// 终端手机号码 0x20B80000
extern PLUGIN_INFO g_layerTerminalInfo1;	//终端信息第一屏:电表一信息 0x00100010
extern PLUGIN_INFO g_layerTerminalInfo2;	//终端信息第二屏:电表二信息 0x00100020
extern PLUGIN_INFO g_layerTerminalInfo3;	//终端信息第三屏:主通道信息(gprs/cdma) 0x00100030
extern PLUGIN_INFO g_layerTerminalInfo4;	//终端信息第四屏:软硬件版本号 0x00100040
extern PLUGIN_INFO g_layerTerminalInfo5;	//终端信息第五屏:主通道信息(以太网) 0x00100050
extern PLUGIN_INFO g_layerTerminalInfo6;	//终端信息第六屏:终端编号,ICCID号、SIM卡手机号 0x00100060
extern PLUGIN_INFO g_layerCommdisp;
#endif

#if GANSU_FUN == 1
extern PLUGIN_INFO		g_layerIPConfig;	 // IPConfig 0x006F0003
extern PLUGIN_INFO g_layerPublicNetCommPara;
#endif
#if ANHUI_FUN == 1 
extern PLUGIN_INFO		g_layerIPConfig;	 // IPConfig 0x006F0003
#endif
#if HUBEI_FUN == 1
extern PLUGIN_INFO g_layerTerminalInfo1;            // 终端信息 0x00100010
extern PLUGIN_INFO g_layerTerminalInfo2;            // 终端信息 0x00100020
extern PLUGIN_INFO g_layerAutoPara;
#endif
#if HUABEI_FUN == 1 || CHONG_QING_FUN == 1
#if GPRS_USB_VIRTUAL_NET_EN == 1
extern PLUGIN_INFO g_layersetusbcom;
#endif
#endif
#if GANSU_FUN == 1
extern PLUGIN_INFO g_layerTmnTrafficLmt;    //月流量门限0x00630003
#endif
#if LIAONING_FUN == 1
extern PLUGIN_INFO g_layerMeasureMainDataJC;	//JC测量点数据0x00440010
#endif
#endif//_LAYER_H_

