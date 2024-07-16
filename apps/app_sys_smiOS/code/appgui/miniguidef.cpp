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

extern PLUGIN_INFO g_smiOSNetPara;
extern PLUGIN_INFO g_smiOSLCDPara;
extern PLUGIN_INFO g_smiOSBlueCfg;
extern PLUGIN_INFO g_smiOSNetParaSet;
extern PLUGIN_INFO g_smiOSLCDParaSet;

extern PLUGIN_INFO g_smiOSDispCodeSet;
extern PLUGIN_INFO g_smiOSSysTimeSet;
extern PLUGIN_INFO g_smiOSContainStat;
extern PLUGIN_INFO g_smiOSOSInfo;
extern PLUGIN_INFO g_smiOSAppReg;
extern PLUGIN_INFO g_smiOSHardwareInfo;
extern PLUGIN_INFO g_smiOSRestartTmn;
extern PLUGIN_INFO g_smiOSFactoryReset;
extern PLUGIN_INFO g_smiOS_SSHSet;
extern PLUGIN_INFO g_smiOSAppVersion;
extern PLUGIN_INFO g_smiOSUUpdate;
extern PLUGIN_INFO g_smiOSLogExport;

PLUGIN_INFO* g_plugin[] =
{
    // 菜单页面库
    &g_menuLayer,           				// 1列菜单风格  0x00A00000
    &g_layerValidatePwd,
    &g_layerInput,
    &g_layerPopMsg,
    &g_smiOSNetPara,						//网络参数查看
    &g_smiOSLCDPara,						//液晶参数查看
    &g_smiOSBlueCfg,						//蓝牙配置查看
    &g_smiOSNetParaSet,						//网络参数设置 0x00A00040
    &g_smiOSLCDParaSet,
    &g_smiOSDispCodeSet,					//界面密码设置	0x00A00050
    &g_smiOSSysTimeSet, 					//系统时间设置	0x00A00060
    &g_smiOSContainStat,					//容器状态查看 0x00A00070
    &g_smiOSOSInfo,							//操作系统信息	0x00A00080
    &g_smiOSAppReg,							//应用注册信息 0x00A00090
    &g_smiOSHardwareInfo,					//设备信息查看 0x00A000A0
    &g_smiOSRestartTmn,						//重启终端 0x00A000B0
    &g_smiOSFactoryReset,					//恢复出厂设置 0x00A000C0
    &g_smiOS_SSHSet,						//SSH服务设置 0x00A000D0
    &g_smiOSAppVersion,						//应用版本信息 0x00A000E0
    &g_smiOSUUpdate,                        //U盘升级 0x00A000F0
    &g_smiOSLogExport,						//日志导出 0x00A000F1
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

