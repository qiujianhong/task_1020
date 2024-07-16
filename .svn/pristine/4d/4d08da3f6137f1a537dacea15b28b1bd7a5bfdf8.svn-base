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

extern PLUGIN_INFO g_layer_control_pc_display1;
extern PLUGIN_INFO g_layer_control_pc_display2;
extern PLUGIN_INFO g_layer_control_pc_display3;
extern PLUGIN_INFO g_layer_control_pc_display4;
extern PLUGIN_INFO g_layer_control_pc_display5;
extern PLUGIN_INFO g_layer_control_pc_display6;
extern PLUGIN_INFO g_layer_control_pc_display7;
extern PLUGIN_INFO g_layer_control_pc_display8;

extern PLUGIN_INFO g_layer_control_ec_display1;
extern PLUGIN_INFO g_layer_control_ec_display2;
extern PLUGIN_INFO g_layer_control_ec_display3;
extern PLUGIN_INFO g_layer_control_ec_display4;
extern PLUGIN_INFO g_layer_control_ec_display5;
extern PLUGIN_INFO g_layer_control_ec_display6;
extern PLUGIN_INFO g_layer_control_ec_display7;
extern PLUGIN_INFO g_layer_control_ec_display8;

extern PLUGIN_INFO g_layer_ctrl_period_display1;
extern PLUGIN_INFO g_layer_ctrl_period_display2;
extern PLUGIN_INFO g_layer_ctrl_period_display3;
extern PLUGIN_INFO g_layer_ctrl_period_display4;
extern PLUGIN_INFO g_layer_ctrl_period_display5;
extern PLUGIN_INFO g_layer_ctrl_period_display6;
extern PLUGIN_INFO g_layer_ctrl_period_display7;
extern PLUGIN_INFO g_layer_ctrl_period_display8;

extern PLUGIN_INFO g_layer_ctrl_rest_display1;
extern PLUGIN_INFO g_layer_ctrl_rest_display2;
extern PLUGIN_INFO g_layer_ctrl_rest_display3;
extern PLUGIN_INFO g_layer_ctrl_rest_display4;
extern PLUGIN_INFO g_layer_ctrl_rest_display5;
extern PLUGIN_INFO g_layer_ctrl_rest_display6;
extern PLUGIN_INFO g_layer_ctrl_rest_display7;
extern PLUGIN_INFO g_layer_ctrl_rest_display8;

extern PLUGIN_INFO g_layer_ctrl_stop_display1;
extern PLUGIN_INFO g_layer_ctrl_stop_display2;
extern PLUGIN_INFO g_layer_ctrl_stop_display3;
extern PLUGIN_INFO g_layer_ctrl_stop_display4;
extern PLUGIN_INFO g_layer_ctrl_stop_display5;
extern PLUGIN_INFO g_layer_ctrl_stop_display6;
extern PLUGIN_INFO g_layer_ctrl_stop_display7;
extern PLUGIN_INFO g_layer_ctrl_stop_display8;

extern PLUGIN_INFO g_layer_ctrl_down_display1;
extern PLUGIN_INFO g_layer_ctrl_down_display2;
extern PLUGIN_INFO g_layer_ctrl_down_display3;
extern PLUGIN_INFO g_layer_ctrl_down_display4;
extern PLUGIN_INFO g_layer_ctrl_down_display5;
extern PLUGIN_INFO g_layer_ctrl_down_display6;
extern PLUGIN_INFO g_layer_ctrl_down_display7;
extern PLUGIN_INFO g_layer_ctrl_down_display8;

extern PLUGIN_INFO g_layer_ctrl_month_display1;
extern PLUGIN_INFO g_layer_ctrl_month_display2;
extern PLUGIN_INFO g_layer_ctrl_month_display3;
extern PLUGIN_INFO g_layer_ctrl_month_display4;
extern PLUGIN_INFO g_layer_ctrl_month_display5;
extern PLUGIN_INFO g_layer_ctrl_month_display6;
extern PLUGIN_INFO g_layer_ctrl_month_display7;
extern PLUGIN_INFO g_layer_ctrl_month_display8;

extern PLUGIN_INFO g_layer_ctrl_turn_display1;
extern PLUGIN_INFO g_layer_ctrl_turn_display2;
extern PLUGIN_INFO g_layer_ctrl_turn_display3;
extern PLUGIN_INFO g_layer_ctrl_turn_display4;
extern PLUGIN_INFO g_layer_ctrl_turn_display5;
extern PLUGIN_INFO g_layer_ctrl_turn_display6;
extern PLUGIN_INFO g_layer_ctrl_turn_display7;
extern PLUGIN_INFO g_layer_ctrl_turn_display8;

extern PLUGIN_INFO g_layer_magneticEvent_display;
extern PLUGIN_INFO g_layer_ac_param_initialization;

extern PLUGIN_INFO g_layer_ZJ_left_display;

extern PLUGIN_INFO g_layer_esam_display;
extern PLUGIN_INFO g_layer_battery_display;

#ifdef AREA_FUJIAN
extern PLUGIN_INFO g_layer_GatherFJ_set;              //数据采集模式
extern PLUGIN_INFO g_layer_rs485Status_set;            //RS485端口数据共享
#endif


PLUGIN_INFO* g_plugin[] =
{
    // 菜单页面库
    &g_menuLayer,           // 1列菜单风格  0x00A00000
    &g_layerInput,
    &g_layerValidatePwd,
    &g_layerPopMsg,
    &g_layer_ac_forward_power_display,   //交采当前正向电能示数
    &g_layer_ac_reverse_power_display,   //交采当前反向电能示数
    &g_layer_ac_quadrant_power_display,  //交采当前四象限无功电能 
    &g_layer_ac_vol_cur_display,         //电压电流数据
    &g_layer_ac_power_display,           //当前功率示数
    &g_layer_ac_demand_display,          //正反向有功需量
    &g_layer_switch_status_display,      //遥信状态
    &g_turnstart,                        //轮次状态查看
    &g_layer_power_off_on_display,       //停上电事件
    &g_layer_magneticEvent_display,      //恒定磁场干扰事件
    &g_layer_power_ctr_evt_display,      //功控记录
    &g_layer_elect_ctr_evt_display, 
    &g_layer_buy_elect_evt_display,
    &g_layer_elect_warn_evt_display,
    &g_layer_measure_data_display,      //抄表实时数据查看
    &g_layer_measure_day_frozen_display,//抄表日冻结数据查看
    &g_layer_harmonic_display,          //谐波含有率
    &g_layer_harmonic_value_display,    //谐波含量

    //高精度
    &g_layer_ac_high_forward_power_display,   //交采当前正向电能示数
    &g_layer_ac_high_reverse_power_display,   //交采当前反向电能示数
    &g_layer_ac_high_quadrant_power_display,  //交采当前四象限无功电能

//  &g_layer_dc_data_display,           //直流模拟量

    &g_layerChangePwd,                  //密码设置
    &g_layer_restartAPP,                //重启应用
//  &g_layer_reboot_terminal,           //重启终端
    &g_layer_data_initialization,       //数据初始化
    &g_layer_param_initialization,      //参数初始化
    &g_layer_ac_param_initialization,   //交采参数初始化
    &g_layer_task_status,               //任务状态查看
    &g_layer_manual_amr,                //手动抄表测试
    &g_layer_line_net,                  //载波组网情况
#ifdef AREA_HUNAN
    &g_layer_sensor_param,               //传感器参数查看
    &g_layer_sensor_set,                //传感器参数设置
#endif    
    &g_layer_app_version_display,      //版本信息 
    &g_layer_tmn_version_display,      //版本信息
    &g_layer_comm_proxy_set,                      //通信代理设置
    
    &g_layer_address_display,               //通信地址查看
    &g_layer_ip_display,                //IP地址查看
    &g_layer_ip_display1,                //IP地址查看
    &g_layer_eth_display,               //以太网通道查看
    &g_layer_ethmaster_display,             //以太网主站配置查看
    &g_layer_gprs_display,               //以太网通道查看
    &g_layer_gprs2_display,               //以太网通道查看
    &g_layer_gprs_numnet,                  //无线公网1多网络配置
    &g_layer_gprs2_numnet,                  //无线公网2多网络配置
#ifdef AREA_SHAANXI
    &g_layer_gprs1chanenl_param_set,
    &g_layer_gprs2chanenl_param_set,
#endif
    &g_layer_gprsmaster_display,            //GPRS主站配置
    &g_layer_gprs2master_display,
    &g_layer_meterfile_display,             //电表档案查看
    &g_layer_normal_plan_display,           //普通方案查看
    &g_layer_event_plan_display,            //事件方案查看
    &g_layer_report_plan_display,           //上报方案查看
    &g_layer_task_display,                  //采集任务查看
    &g_layer_HPLC_display,                   //HPLC参数查看

    &g_layer_address_set,                   //通信地址设置
    &g_layer_ip_set,                    //IP地址设置
    &g_layer_ip_set1,                    //IP地址设置
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
    &g_layer_HPLCSearch,                    //载波搜表设置
    &g_layer_Hchanenl_set,                  //载波频点设置
#ifdef AREA_FUJIAN
    &g_layer_GatherFJ_set,               //数据采集模式
    &g_layer_rs485Status_set,            //RS485端口数据共享
#endif      
    &g_layer_plus_data_display,       //脉冲数据
    &g_layer_gruop_data_display1,       // 总加组数据 0x11011000
    &g_layer_gruop_data_display2,
    &g_layer_gruop_data_display3,
    &g_layer_gruop_data_display4,
    &g_layer_gruop_data_display5,
    &g_layer_gruop_data_display6,
    &g_layer_gruop_data_display7,
    &g_layer_gruop_data_display8,

    &g_layer_control_pc_display1,             //功控状态1 0x11012100
    &g_layer_control_pc_display2,
    &g_layer_control_pc_display3,
    &g_layer_control_pc_display4,
    &g_layer_control_pc_display5,
    &g_layer_control_pc_display6,
    &g_layer_control_pc_display7,
    &g_layer_control_pc_display8,

    &g_layer_control_ec_display1,             //功控状态1 0x11012200
    &g_layer_control_ec_display2,
    &g_layer_control_ec_display3,
    &g_layer_control_ec_display4,
    &g_layer_control_ec_display5,
    &g_layer_control_ec_display6,
    &g_layer_control_ec_display7,
    &g_layer_control_ec_display8,
    
    &g_layer_control_group_display1,        // 总加组控制状态1 0x11012000
    &g_layer_control_group_display2,
    &g_layer_control_group_display3,
    &g_layer_control_group_display4,
    &g_layer_control_group_display5,
    &g_layer_control_group_display6,
    &g_layer_control_group_display7,
    &g_layer_control_group_display8,

    &g_layer_plus_param1,                  //脉冲配置
    &g_layer_plus_param2,
    &g_layer_plus_param3,
    &g_layer_plus_param4,
    &g_layer_plus_param5,
    &g_layer_plus_param6,
    &g_layer_plus_param7,
    &g_layer_plus_param8,

    &g_layer_gruop_param1,                //总加组配置
    &g_layer_gruop_param2,
    &g_layer_gruop_param3,
    &g_layer_gruop_param4,
    &g_layer_gruop_param5,
    &g_layer_gruop_param6,
    &g_layer_gruop_param7,
    &g_layer_gruop_param8,

    &g_BuyDatadisplay1,     // 购电信息1 0x11030040
    &g_BuyDatadisplay2,
    &g_BuyDatadisplay3,
    &g_BuyDatadisplay4,
    &g_BuyDatadisplay5,
    &g_BuyDatadisplay6,
    &g_BuyDatadisplay7,
    &g_BuyDatadisplay8,

    &g_layer_chinese_info_display1,    //中文信息
    &g_layer_chinese_info_display2,

    &g_layer_reset_time,          //终端时间设置

    &g_module_info_display1,                //模块信息  0x11050011
    &g_module_info_display2,
    &g_module_info_display3,
    &g_module_info_display4,
    &g_module_info_display5,

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
    
    &g_layer_ctrl_turn_display1,   //轮次参数 0x11031060
    &g_layer_ctrl_turn_display2,
    &g_layer_ctrl_turn_display3,
    &g_layer_ctrl_turn_display4,
    &g_layer_ctrl_turn_display5,
    &g_layer_ctrl_turn_display6,
    &g_layer_ctrl_turn_display7,
    &g_layer_ctrl_turn_display8,
    &g_safemode,//安全模式
    &g_layer_address_set_hunan,                   //通信地址设置

    &g_layer_ZJ_left_display,    /* 剩余电量 */

    &g_layer_esam_display,
    &g_layer_battery_display,
};
int g_nPluginCnt = sizeof(g_plugin) / sizeof(g_plugin[0]);

