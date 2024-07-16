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

extern PLUGIN_INFO g_layer_show_server_master;
extern PLUGIN_INFO g_layer_show_server_channel;
extern PLUGIN_INFO g_layer_show_eth_channel;
extern PLUGIN_INFO g_layer_show_ppp_channel;
extern PLUGIN_INFO g_layer_set_client_channel;
extern PLUGIN_INFO g_layer_set_client_eth;
extern PLUGIN_INFO g_layer_set_client_ppp;

PLUGIN_INFO* g_plugin[] =
{
    // 菜单页面库
    &g_menuLayer,           // 1列菜单风格  0x00A00000
    &g_socketstart,
	&g_remotestatus,
    &g_layerInput,
    &g_layerValidatePwd,
    &g_layer_ac_forward_power_display,   //交采当前正向电能示数
    &g_layer_ac_reverse_power_display,   //交采当前反向电能示数
    &g_layer_ac_quadrant_power_display,  //交采当前四象限无功电能 
    &g_layer_ac_vol_cur_display,         //电压电流数据
    &g_layer_ac_power_display,           //当前功率示数
    &g_layer_ac_demand_display,          //正反向有功需量
    &g_layer_measure_data_display,      //抄表数据查看

    &g_layer_ethmaster101_display,      //通信通道查看
    &g_layer_ethmaster104_display,
    &g_layer_ethmaster101_set,      //通信通道设置
    &g_layer_ethmaster104_set,
    &g_layer_pass_rate_param_display,   //合格率参数
    &g_layer_pass_rate_param_set,

    &g_layer_restartAPP,      //重启应用
    &g_layer_data_initialization,      //数据初始化
    &g_layer_param_initialization,      //参数初始化
    &g_layer_app_version_display,      //版本信息 

    &g_layer_show_server_master,  //连接主站(S)状态
    &g_layer_show_server_channel, //通信通道类型查看
    &g_layer_show_eth_channel,    //以太网参数查看
    &g_layer_show_ppp_channel,    //无线参数查看

    &g_layer_set_client_channel,  //设置客户机通道
    &g_layer_set_client_eth,      //设置以太网参数
    &g_layer_set_client_ppp,      //设置无线参数
};
int g_nPluginCnt = sizeof(g_plugin) / sizeof(g_plugin[0]);

