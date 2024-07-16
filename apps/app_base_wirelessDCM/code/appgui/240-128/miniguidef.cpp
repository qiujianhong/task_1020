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

extern PLUGIN_INFO g_layerGPSData1;
extern PLUGIN_INFO g_layerGPSData2;
extern PLUGIN_INFO g_layerATShowData1;
extern PLUGIN_INFO g_layerATShowData2;
extern PLUGIN_INFO g_layerATShowData3;
extern PLUGIN_INFO g_layerATShowData4;
extern PLUGIN_INFO g_layerModuleData1;
extern PLUGIN_INFO g_layerModuleData2;



PLUGIN_INFO* g_plugin[] =
{
    // 菜单页面库
    &g_menuLayer,           // 1列菜单风格  0x00A00000
    &g_layerInput,
    &g_layerValidatePwd,
    &g_layerDialingStatusData1,         //无线拨号状态
    &g_layerDialingStatusData2,
    &g_layerDialingEventData1,          //无线拨号事件
    &g_layerDialingEventData2,
    &g_layerModuleData1,
    &g_layerModuleData2,
    &g_layerGPSData1,                   //位置信息
    &g_layerGPSData2, 
    &g_layerATShowData1,                //AT指令
    &g_layerATShowData2,
    &g_layerATShowData3,
    &g_layerATShowData4,
    &g_layerDialingParamData1,          //拨号参数显示
    &g_layerDialingParamData2,            
    &g_layerDialingPARASet1,            //拨号参数设置
    &g_layerDialingPARASet2,
    &g_layerDialingRestartAPP,          //重启应用
    &g_layerReDialing1,                 //重新拨号
    &g_layerReDialing2,
    &g_layerDialVersionInfo,            //版本信息
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

