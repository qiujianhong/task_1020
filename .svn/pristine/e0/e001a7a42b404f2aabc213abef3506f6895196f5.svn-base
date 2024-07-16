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

PLUGIN_INFO* g_plugin[] =
{
    // 菜单页面库
    &g_menuLayer,           // 1列菜单风格  0x00A00000
    
    &g_layer_HPLC_frequency,              //HPLC频点  
    &g_layer_meter_reading_port,          //抄表端口
    &g_layer_module_address,              //模块地址  
    &g_layer_module_info,                 //模块信息
    &g_layer_meter_reading_status,        //抄表状态
    &g_layer_station_identification,      //台区识别      
    &g_layer_off_grid_awareness,          //离网感知
    &g_layer_reject_node_report,          //拒绝节点上报
    &g_layer_file_info,                   //档案信息
    &g_layer_restart_app,                 //重启应用
    &g_layer_hardware_initialization,     //硬件初始化
    &g_layer_param_initialization,        //参数初始化
    &g_layer_app_version_info,            //应用版本信息
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

