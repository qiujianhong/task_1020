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

extern PLUGIN_INFO g_layerLcStatus;
extern PLUGIN_INFO g_layerHisStatus;
extern PLUGIN_INFO g_layerRestartAPP;
extern PLUGIN_INFO g_layerVersionInfo;
extern PLUGIN_INFO g_layerCoreInfo;
extern PLUGIN_INFO g_layerCTAInfo;
extern PLUGIN_INFO g_layerCTBInfo;
extern PLUGIN_INFO g_layerCTCInfo;

PLUGIN_INFO* g_plugin[] =
{
    // 菜单页面库
    &g_menuLayer,           // 1列菜单风格  0x00A00000
    &g_layerLcStatus,            //当前电流回路状态
    &g_layerHisStatus,           //历史电流回路状态
    &g_layerRestartAPP,          //重启应用
    &g_layerVersionInfo,         //版本信息
    &g_layerCoreInfo,            //核心板信息
    &g_layerCTAInfo,             //A相互感器信息
    &g_layerCTBInfo,             //B相互感器信息
    &g_layerCTCInfo,             //C相互感器信息
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

