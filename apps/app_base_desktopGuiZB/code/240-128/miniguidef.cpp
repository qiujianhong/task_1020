/*
***************************************************************************
Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
All rights reserved.
程序名称: main.h
功    能：FKGA42显示模块框架实现文件，用于定义和添加页面库操作
开 发 人：gaoyf
开发时间：2007-1

  版本历史:

    v1.0:
	2006-1-8, gaoyf : New file

	  ***************************************************************************
*/

#include "miniguidef.h"
#include "layer.h"

extern PLUGIN_INFO g_CoUpdate;
extern PLUGIN_INFO g_layer_co_ver;
extern PLUGIN_INFO g_layer_sys_ver;
extern PLUGIN_INFO g_layer_symKey_ver;
extern PLUGIN_INFO g_layer_clientno_set;

PLUGIN_INFO* g_plugin[] =
{	
	&g_puamr,
    &g_stamr,               //常显页面
    &g_layerPopMsg,			// 0xFFFFFD
    &g_Template1,			// 背景风格库

    // 菜单页面库
    &g_menuLayer,           // 1列菜单风格  0x00A00000
    &g_menu2Layer,          // 2列菜单风格  0x20A10000
    &g_layerInput,          // 输入法用户界面  0x00a30000
    &g_layerTimeArea,       // 输入时间范围界面0x00a40000
    &g_layerValidatePwd,    // 密码验证 0x00600000
    &g_layerFreezeTime, 	  // 验证冻结 0x00600002
    &g_layer_task_status,     //任务状态查看
	&g_CommDispDraw1,		// 常显画面
    &g_CommDispDraw2,		// 常显画面
    &g_CommDispDraw3,		// 常显画面
    &g_CommDispDraw4,		// 常显画面
	&g_CommDispDraw5,		// 常显画面
    &g_CommDispDraw6,		// 常显画面
    &g_CommDispDraw7,		// 常显画面
    &g_CommDispDraw8,		// 常显画面

	&g_layer_magneticEvent_display,
	&g_layer_cur_powerTotal,  			  //当前功率
    &g_layer_gruop_cur_powerTotal,          //当前功率
	&g_layer_cur_EleQ,					  //当前电量
	&g_layer_power_ctr_evt_display,		  //功控记录
	&g_layer_elect_ctr_evt_display,		  //电控记录
	&g_layer_pwct_ctr_evt_display,		  //失电记录
	&g_layer_rct_ctr_evt_display,		  //遥控记录
	&g_layer_switch_state_display,		  //开关状态
	&g_layer_periodCtrl_param_display,	  //时段控参数
	&g_layer_restCtrl_param_display,	  //厂休控参数
	&g_layer_monthCtrl_param_display,	  //月电控参数
	&g_layer_downCtrl_param_display,	  //下浮控参数
	&g_layer_stopCtrl_param_display,      //报停控参数
	&g_layer_kivp_display,				  //kikvkp
    &g_layer_kivp_puls_display,       // 脉冲互感  
	&g_layer_meterParam_display,		  //电表参数
	&g_layer_powerCurve_display,           //负荷曲线

	/************************************************************************/
	/* 控制状态 																*/
	/************************************************************************/
	&g_ControlStatusDraw1,	// 控制状态 0x00010101
	&g_ControlStatusDraw2,	// 控制状态 0x00010201
	&g_ControlStatusDraw3,	// 控制状态 0x00010301
	&g_ControlStatusDraw4,	// 控制状态 0x00010401
	&g_ControlStatusDraw5,	// 控制状态 0x00010501
	&g_ControlStatusDraw6,	// 控制状态 0x00010601
	&g_ControlStatusDraw7,	// 控制状态 0x00010701
	&g_ControlStatusDraw8,	// 控制状态 0x00010801
	
	/************************************************************************/
	/* 控制状态 结束															 */
	/************************************************************************/

	& g_EquipValDraw1,			// 电表示数  0x20300000
	& g_EquipValDraw2,			// 电表示数  0x20300001
	& g_EquipValDraw3,			// 电表示数  0x20300002
	& g_EquipValDraw4,			// 电表示数  0x20300003
	& g_EquipValDraw5,			// 电表示数  0x20300004
	& g_EquipValDraw6,			// 电表示数  0x20300005
	& g_EquipValDraw7,			// 电表示数  0x20300006
	& g_EquipValDraw8,			// 电表示数  0x20300007
	& g_EquipValDraw9,			// 电表示数  0x20300008
	& g_EquipValDraw10,			// 电表示数  0x20300009
	& g_EquipValDraw11,			// 电表示数  0x2030000a
	& g_EquipValDraw12,			// 电表示数  0x2030000b
	& g_EquipValDraw13,			// 电表示数  0x2030000c
	& g_EquipValDraw14,			// 电表示数  0x2030000d
	& g_EquipValDraw15,			// 电表示数  0x2030000e
	& g_EquipValDraw16,			// 电表示数  0x2030000f


	& g_HEquipValDraw1,			// 电表示数  0x20300000
	& g_HEquipValDraw2,			// 电表示数  0x20300001
	& g_HEquipValDraw3,			// 电表示数  0x20300002
	& g_HEquipValDraw4,			// 电表示数  0x20300003
	& g_HEquipValDraw5,			// 电表示数  0x20300004
	& g_HEquipValDraw6,			// 电表示数  0x20300005
	& g_HEquipValDraw7,			// 电表示数  0x20300006
	& g_HEquipValDraw8,			// 电表示数  0x20300007
	& g_HEquipValDraw9,			// 电表示数  0x20300008
	& g_HEquipValDraw10,		// 电表示数  0x20300009
	& g_HEquipValDraw11,		// 电表示数  0x2030000a
	& g_HEquipValDraw12,		// 电表示数  0x2030000b
	& g_HEquipValDraw13,		// 电表示数  0x2030000c
	& g_HEquipValDraw14,		// 电表示数  0x2030000d
	& g_HEquipValDraw15,		// 电表示数  0x2030000e
	& g_HEquipValDraw16,		// 电表示数  0x2030000f
	/************************************************************************/
	/* 中文信息页面库                                                           */
	/************************************************************************/
	& g_ZhMsgInfoDraw1,		// 中文信息1  0x20400000
	& g_ZhMsgInfoDraw2,		// 中文信息2  0x20400001
    &g_ZhMsgInfoDraw3,		// 中文信息3	0x20400002
	&g_ZhMsgInfoDraw4,		// 中文信息4	0x20400003
	&g_ZhMsgInfoDraw5,		// 中文信息5	0x20400004
	&g_ZhMsgInfoDraw6,		// 中文信息6	0x20400005
	&g_ZhMsgInfoDraw7,		// 中文信息7	0x20400006
	&g_ZhMsgInfoDraw8,		// 中文信息8	0x20400007
	&g_ZhMsgInfoDraw9,		// 中文信息9	0x20400008
	&g_ZhMsgInfoDraw10,		// 中文信息10	0x20400009	
	
	/************************************************************************/
	/* 中文信息页面库结束                                                       */
	/************************************************************************/

	/************************************************************************/
	/* 购电信息                                                              */
	/************************************************************************/
    &g_BuyDataDraw1,	// 购电信息	0x00010100
    &g_BuyDataDraw2,	// 购电信息	0x00010200
    &g_BuyDataDraw3,	// 购电信息	0x00010300
    &g_BuyDataDraw4,	// 购电信息	0x00010400
    &g_BuyDataDraw5,	// 购电信息	0x00010500
    &g_BuyDataDraw6,	// 购电信息	0x00010600
    &g_BuyDataDraw7,	// 购电信息	0x00010700
    &g_BuyDataDraw8,	// 购电信息	0x00010800
	/************************************************************************/
	/*  购电信息结束                                                             */
	/************************************************************************/

	&g_layer_address_set,		//配置参数
    &g_layer_address_set1,               //通信地址设置
	&g_TermDataDraw,					// 终端信息  0x20600000
	&g_layer_app_version_display,
	&g_UpdateStatusDraw,
	&g_gprsStatusDraw,					//拨号信息
	&g_ChannelDebugStatusDraw,
	&g_layer_ac_vol_cur_display,		//交采电压电流
    &g_NetmodeDraw,
	&g_ModemversionDraw,
	
//	&g_layerChnlInfo,					//通讯通道配置
	&g_layerEthernet,
	&g_layer_gprs_set,
	&g_layer_networkmode_set,
	&g_layer_meterfile_set,             //测量点配置
	&g_layerPulseMain,	  				// 脉冲测量点配置  0x20740000
	&g_layerGroupMain,                   //总加组配置
	&g_Set_ParaResetTime,				//时间设置
	&g_layerChangePwd,                  //密码设置
	//&g_layerac_FeatureCfg,          //交采电流设置
	&g_layerRadioCfg,					// 电台参数设置 0x20B70000
	&g_layerRadioLongSendCfg,					// 电台长发
	&g_MeterReadFailStat,
	&g_layer_normal_plan_set,

	&g_layer_ip_set,					//IP设置
	&g_layerTrunDisp,					//页面配置
    &g_layer_reboot_terminal,           //重启终端
    &g_layer_data_initialization,		//数据初始化
    &g_layer_param_initialization,		//参数初始化
    &g_layer_ac_param_initialization,	//交采参数初始化
    &g_layer_data_param_initialization,	//交采数据初始化
    &g_smiOSLogExport,					//日志导出
    &g_SSHSet_Layer,       //SSH服务设置
    &g_smiOSUUpdate,					//U盘升级
    &g_LogClear_Layer,					//日志清除
    &g_layersetSafeONOFF,  				//安全模式启用关闭0x000c0003
    &g_layer_wirelessStatus_set,        //无线拨号方式
    &g_ACModuleUpdate,                  //交采模块升级 0x00A000F6
    &g_layer_ac_ver,                    //交采版本
    &g_CoUpdate,            //协处理器升级 0x00A000FE
    &g_layer_co_ver,        //协处理器版本
	&g_layer_sys_ver,       //系统版本
	&g_layer_symKey_ver,    //系统密钥版本
	&g_layer_gprs_numnet,
	&g_layer_gprs2_display,
    &g_layer_clientno_set,                  //客户编号
	&g_layerProductVoltageCfg,
    #ifdef AREA_JIANGSU
    &g_layer_loop_state_display,
    &g_layer_inspector_ver_display,
    &g_layer_GPRS_param_clean,
    #endif	
	&g_disk_status,              /*磁盘信息*/
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

