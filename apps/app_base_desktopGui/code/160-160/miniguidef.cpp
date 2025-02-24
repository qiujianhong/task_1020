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

extern PLUGIN_INFO g_layer_wirelessStatus_set;

PLUGIN_INFO* g_plugin[] =
{
    &g_puamr,
    &g_stamr,               //常显页面
    &g_Template1,			      // 背景风格库
    &g_WelcomeLayer,				// 0xFFFFFE
    &g_layerPopMsg,			    // 0xFFFFFD

    // 菜单页面库
    &g_menuLayer,           // 1列菜单风格  0x00A00000
    &g_menu2Layer,          // 2列菜单风格  0x00A10000

    &g_layerInput,          // 输入法用户界面  0x00a30000
    &g_layerTimeArea,       // 输入时间范围界面0x00a40000
    &g_menu3Layer,			    //3列菜单风格0x00A50000

    &g_layersetSafeONOFF,   //安全模式启用关闭0x000c0003
    &g_smiOSUUpdate,        //U盘升级
    &g_layer_wirelessStatus_set,
    &g_smiOSLogExport,      //日志导出
    &g_SSHSet_Layer,        //SSH服务设置
    &g_TCPDump_Set_Layer,   //TCPDump服务设置
    &g_passwd_Layer,        //密码修改
    &g_LogClear_Layer,      //日志清除 0x00A000F3
    &g_ModuleUpdate,        //模组升级 0x00A000F4
    &g_SystemCmd,           //System命令 0x00A000F5
    &g_EchoPrint,           //回显打印 0x10A000F5
    &g_ACModuleUpdate,      //交采模块升级 0x00A000F6
    &g_CoUpdate,            //协处理器升级 0x00A000FE
    &g_layer_bluetooth,     //设置蓝牙开关
    &g_layer_CPU_temperature,    //CPU当前温度
    &g_layer_power_status,       //电源供电状态
    &g_layer_ac_ver,        //交采版本
    &g_layer_co_ver,        //协处理器版本
    & g_layer_comm_proxy,
    // 配置页面库
    &g_layerValidatePwd,    // 密码验证 0x00600000

    &g_layerFreezeTime, 	  // 验证冻结 0x00600002
    
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

