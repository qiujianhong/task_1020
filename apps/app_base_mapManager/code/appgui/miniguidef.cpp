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
	&g_layerValidatePwd,
	
    &g_moduleInfo1,         //模组1信息
    &g_moduleInfo2,			//模组2信息
    &g_moduleInfo3,         //模组3信息
    &g_moduleInfo4,			//模组4信息
    &g_moduleInfo5,         //模组5信息
    &g_modulePlugEvent,     //模组插拔事件
    &g_appRestart,            //重启应用
    &g_moduleRestart,         //重启某个模组
    &g_moduleReset,           //复位某个模组
    &g_dataInitilize,			//数据初始化
    &g_paraInitilize,         //参数初始化
    &g_appVersionInfo,        //应用版本信息
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

