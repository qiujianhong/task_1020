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
extern PLUGIN_INFO g_HplcUpdate;
extern PLUGIN_INFO g_QueryProviderSignal;
extern PLUGIN_INFO g_SearchProviderSignal;

PLUGIN_INFO* g_plugin[] =
{
    &g_ControlStatusDraw1,	// 控制状态 0x00010101
	&g_ControlStatusDraw2,	// 控制状态 0x00010201
	&g_ControlStatusDraw3,	// 控制状态 0x00010301
	&g_ControlStatusDraw4,	// 控制状态 0x00010401
	&g_ControlStatusDraw5,	// 控制状态 0x00010501
	&g_ControlStatusDraw6,	// 控制状态 0x00010601
	&g_ControlStatusDraw7,	// 控制状态 0x00010701
	&g_ControlStatusDraw8,	// 控制状态 0x00010801

	&g_ZhMsgInfoDraw1,    //中文信息
    &g_ZhMsgInfoDraw2,
    &g_ZhMsgInfoDraw3,
    &g_ZhMsgInfoDraw4,
    &g_ZhMsgInfoDraw5,
    &g_ZhMsgInfoDraw6,
    &g_ZhMsgInfoDraw7,
    &g_ZhMsgInfoDraw8,
    &g_ZhMsgInfoDraw9,
    &g_ZhMsgInfoDraw10,
    
	&g_BuyDatadisplay1,     // 购电信息1 0x11030040
    &g_BuyDatadisplay2,
    &g_BuyDatadisplay3,
    &g_BuyDatadisplay4,
    &g_BuyDatadisplay5,
    &g_BuyDatadisplay6,
    &g_BuyDatadisplay7,
    &g_BuyDatadisplay8,
    
    &g_puamr,
    &g_stamr,               //常显页面
    &g_Template1,			// 背景风格库
    &g_WelcomeLayer,				// 0xFFFFFE
    &g_layerPopMsg,			// 0xFFFFFD

    // 菜单页面库
    &g_menuLayer,           // 1列菜单风格  0x00A00000
    &g_menu2Layer,          // 2列菜单风格  0x00A10000

    &g_layerInput,          // 输入法用户界面  0x00a30000
    &g_layerTimeArea,       // 输入时间范围界面0x00a40000
	&g_menu3Layer,			//3列菜单风格0x00A50000

    &g_layersetSafeONOFF,  //安全模式启用关闭0x000c0003
    &g_smiOSUUpdate,       //U盘升级
    &g_smiOSLogExport,     //日志导出
    &g_SSHSet_Layer,       //SSH服务设置
    &g_TCPDump_Set_Layer,  //TCPDump服务设置
    &g_passwd_Layer,       //密码修改
    &g_LogClear_Layer,     //日志清除 0x00A000F3
    &g_ModuleUpdate,       //模组升级 0x00A000F4
    &g_SystemCmd,          //System命令 0x00A000F5
    &g_EchoPrint,          //回显打印 0x10A000F5
    &g_ACModuleUpdate,     //交采模块升级 0x00A000F6
    &g_layer_bluetooth,    //设置蓝牙开关
    &g_layer_CPU_temperature,    //CPU当前温度
    &g_layer_power_status,       //电源供电状态
    &g_layer_wirelessStatus_set, //无线拨号方式
    &g_layer_ac_ver,       //交采版本
    &g_layer_comm_proxy,
    &g_CoUpdate,            //协处理器升级 0x00A000FE
    &g_layer_co_ver,        //协处理器版本
    &g_layer_sys_ver,       //系统版本
    &g_layer_symKey_ver,    //系统密钥版本
    &g_HplcUpdate,          //HPLC升级
    &g_disk_status,         //磁盘状态
    // 配置页面库
    &g_layerValidatePwd,    // 密码验证 0x00600000

    &g_layerFreezeTime, 	// 验证冻结 0x00600002

	
   &g_layer_ac_forward_power_display,	 //交采当前正向电能示数
   &g_layer_ac_reverse_power_display,	//交采当前反向电能示数
   &g_layer_ac_quadrant_power_display,	//交采当前四象限无功电能 
   &g_layer_ac_vol_cur_display, 		//电压电流数据
   &g_layer_ac_power_display,			//当前功率示数
   &g_layer_ac_demand_display,			//正反向有功需量
   &g_layer_ac_remainPower_display1,     //总剩余电量第一屏
   &g_layer_ac_remainPower_display2,     //总剩余电量第二屏
   &g_layer_measure_data_display,	    //抄表实时数据查看
   &g_layer_measure_day_frozen_display,//抄表日冻结数据查看
   &g_layer_harmonic_display,		   //谐波含有率
   &g_layer_harmonic_value_display,    //谐波含量

   //高精度
   &g_layer_ac_high_forward_power_display,	 //交采当前正向电能示数
   &g_layer_ac_high_reverse_power_display,	 //交采当前反向电能示数
   &g_layer_ac_high_quadrant_power_display,  //交采当前四象限无功电能

	&g_Set_ParaResetTime,				//时间设置
	&g_layerChangePwd,                  //密码设置
    &g_layer_restartAPP,                //重启应用
    &g_layer_reboot_terminal,           //重启终端
    &g_layer_data_initialization,       //数据初始化
    &g_layer_param_initialization,      //参数初始化
    &g_layer_ac_param_initialization,   //交采参数初始化
    &g_layer_data_param_initialization,	//交采数据初始化
    &g_layer_task_status,               //任务状态查看
    &g_layer_task_fail,                 //任务失败电表
    &g_layerTrunDisp,		 			// 轮显页面设置  0x20690000
    &g_layer_manual_amr,                //手动抄表测试
//    &g_layer_line_net,                  //载波组网情况
	&g_layer_switch_status_display,      //遥信状态
 
    &g_layer_app_version_display,      //版本信息 
    &g_layer_tmn_version_display,      //版本信息
//    &g_layer_comm_proxy_set,                      //通信代理设置
    
    &g_layer_address_display,               //通信地址查看
//    &g_layer_ip_display,                //IP地址查看
    &g_layer_eth_display,               //以太网通道查看
//    &g_layer_ethmaster_display,             //以太网主站配置查看
    &g_layer_gprs_display,               //以太网通道查看
    &g_layer_gprs2_display,               //以太网通道查看
    &g_layer_gprs_numnet,                  //无线公网1多网络配置
    &g_layer_gprs2_numnet,                  //无线公网2多网络配置

    &g_layer_gprsmaster_display,            //GPRS主站配置
    &g_layer_gprs2master_display,
    &g_layer_meterfile_display,             //电表档案查看
    &g_layer_normal_plan_display,           //普通方案查看
    &g_layer_event_plan_display,            //事件方案查看
    &g_layer_report_plan_display,           //上报方案查看
    &g_layer_task_display,                  //采集任务查看
    &g_layer_HPLC_display,                   //HPLC参数查看

    &g_layer_address_set,                   //通信地址设置
    &g_layer_address_set1,                   //通信地址设置
    &g_layer_ip_set,                    //IP地址设置
    &g_layer_eth_set,                           //以太网通信配置设置
    &g_layer_ethmaster_set,                    //以太网主站配置设置
    &g_layer_gprs_set,                      //GPRS通信配置设置
    &g_layer_gprs2_set,                      //GPRS通信配置设置   
    &g_layer_gprsmaster_set,                  //GPRS主站配置设置
    &g_layer_gprs2master_set,                  //GPRS主站配置设置
    &g_layer_meterfile_set,                 //电表档案设置
    &g_layer_normal_plan_set,               //普通采集方案设置
    &g_layer_task_set,                      //采集任务设置
    &g_layer_HPLC_set,                      //HPLC参数设置
    &g_layer_Hchanenl_set,                  //载波频点设置
    &g_layer_GatherFJ_set,                  //外部设备数据采集模式
    &g_layer_rs485Status_set,               //RS485-2端口数据共享功能
  //      ---------xtl-------------
      /************************************************************************/
    /* 控制参数                                                             */
    /************************************************************************/

    /************************************************************************/
    /* 控制参数结束                                                         */
    /************************************************************************/
	//  &g_layerMeasureMain,	// 测量点配置 0x00640000 
     /************************************************************************/
    /* 测量点参数                                                           */
    /************************************************************************/
     // 测量点变比参数
   
    &g_layer_kivp_display,       // 3k值   
    &g_layer_kivp_puls_display,       // 脉冲互感  
    &g_layer_config_para,   //配置参数       0x20720000

	&g_layer_ctrl_period_display1, //时段控参数 0x11031010
    &g_layer_ctrl_period_display2,
    &g_layer_ctrl_period_display3,
    &g_layer_ctrl_period_display4,
    &g_layer_ctrl_period_display5,
    &g_layer_ctrl_period_display6,
    &g_layer_ctrl_period_display7,
    &g_layer_ctrl_period_display8,

    &g_layer_ctrl_rest_display1,    //厂休控参数 0x11031020
    &g_layer_ctrl_rest_display2,
    &g_layer_ctrl_rest_display3,
    &g_layer_ctrl_rest_display4,
    &g_layer_ctrl_rest_display5,
    &g_layer_ctrl_rest_display6,
    &g_layer_ctrl_rest_display7,
    &g_layer_ctrl_rest_display8,

    &g_layer_ctrl_stop_display1,   //报停控参数 0x11031030
    &g_layer_ctrl_stop_display2,
    &g_layer_ctrl_stop_display3,
    &g_layer_ctrl_stop_display4,
    &g_layer_ctrl_stop_display5,
    &g_layer_ctrl_stop_display6,
    &g_layer_ctrl_stop_display7,
    &g_layer_ctrl_stop_display8,

    &g_layer_ctrl_down_display1,   //下浮控参数 0x11031040
    &g_layer_ctrl_down_display2,
    &g_layer_ctrl_down_display3,
    &g_layer_ctrl_down_display4,
    &g_layer_ctrl_down_display5,
    &g_layer_ctrl_down_display6,
    &g_layer_ctrl_down_display7,
    &g_layer_ctrl_down_display8,

    &g_layer_ctrl_month_display1,   //下浮控参数 0x11031050
    &g_layer_ctrl_month_display2,
    &g_layer_ctrl_month_display3,
    &g_layer_ctrl_month_display4,
    &g_layer_ctrl_month_display5,
    &g_layer_ctrl_month_display6,
    &g_layer_ctrl_month_display7,
    &g_layer_ctrl_month_display8,



//////////////////////////////////////////////////////////////
    &g_layer_powerCurve_display,            //负荷曲线
	&g_layer_cur_EleQ,						//当前电量
	&g_layer_power_ctr_evt_display_GK,		//功控记录
	&g_layer_cur_powerTotal,                //当前功率
	&g_layer_gruop_cur_powerTotal,          //当前功率
	&g_layer_active_powerTotal,             //有功功率
	&g_layer_reactive_powerTotal,           //无功功率
	&g_layer_power_display,                  //功率因数
	&g_layer_ac_high_active_power_display, //当前正向有功电能量示值
	&g_layer_ac_high_reactive_power_display,//当前正向无功电能量示值
	&g_layer_high_reverse_power_display,   //当前反向无功电能量示值
	&g_layer_high_reactive_power_display, //当前反向无功电能量示值
	/////////////////////////////////////////////////////
	&g_layer_elect_ctr_evt_display,         //电控记录
	&g_layer_pwct_ctr_evt_display,          //失电记录
	&g_layer_magneticEvent_display,
	&g_layer_ac_vol_xj_display,             //相角信息
	&g_layer_rct_ctr_evt_display,
	
    &g_layer_clientno_set,                  //客户编号
    &g_MeterReadFailStat,
	&g_layer_aesam,                  //ESAM下发状态
    &g_port_set_485,
	#ifdef AREA_FUJIAN
    &g_4852_fun,
    &g_QueryProviderSignal,
    &g_SearchProviderSignal,
    #endif
    //此处添加重庆特有的界面
    #ifdef AREA_CHONGQING
    &g_real_time_meter_reading,
    &g_real_time_meter_reading1,
    &g_real_time_reading_all,
    &g_real_time_reading_all_data,
    &g_meter_search_start,
    &g_meter_search_new,
    &g_meter_search_new_read,
    &g_meter_not_found_addr,
    &g_auto_report,
    &g_CQSelectDate,
    &g_MeterReadStat,
    &g_CQModuleStat,
    &g_GprsNumStat,
    &g_CQRouterInfoStat,
    &g_QueryEnergyTotal,
    &g_QueryEnergyTip,
    &g_QueryEnergyPeak,
    &g_QueryEnergyFlat,
    &g_QueryEnergyValley,    &g_layer_meterfile,
    &g_ipDisplay,
    &g_moduleInfo,
    &g_deviceEvent,
    &g_relayRouteDisplay,


    &g_layer_meterfile,
    &g_ipDisplay,
    &g_moduleInfo,
    &g_deviceEvent,
    &g_relayRouteDisplay,
    &g_trmlVerinfo,
    &g_trmSNinfo,
    &g_wirelessComm_set, // 参数设置
    &g_changePwd,
    &g_changeAddr,
    &g_changeVPNPwd,
    &g_layerGprsCfg,
    &g_layersetusbcom,
    &g_ApnSetDisplay,
    &g_Set_CCUResetTime,
    #endif
    //  ------------------------------------       ---------xtl-------------
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

