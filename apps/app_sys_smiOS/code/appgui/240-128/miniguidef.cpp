/*
***************************************************************************
Copyright (c) 2006-2007,�Ͼ��������������������ι�˾
All rights reserved.
��������: main.h
��    �ܣ�FKGA42��ʾģ����ʵ���ļ������ڶ�������ҳ������
�� �� �ˣ�gaoyf
����ʱ�䣺2007-1

  �汾��ʷ:

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
    // �˵�ҳ���
    &g_menuLayer,           				// 1�в˵����  0x00A00000
    &g_layerValidatePwd,
    &g_layerInput,
    &g_layerPopMsg,
    &g_smiOSNetPara,						//��������鿴
    &g_smiOSLCDPara,						//Һ�������鿴
    &g_smiOSBlueCfg,						//�������ò鿴
    &g_smiOSNetParaSet,						//����������� 0x00A00040
    &g_smiOSLCDParaSet,
    &g_smiOSDispCodeSet,					//������������	0x00A00050
    &g_smiOSSysTimeSet, 					//ϵͳʱ������	0x00A00060
    &g_smiOSContainStat,					//����״̬�鿴 0x00A00070
    &g_smiOSOSInfo,							//����ϵͳ��Ϣ	0x00A00080
    &g_smiOSAppReg,							//Ӧ��ע����Ϣ 0x00A00090
    &g_smiOSHardwareInfo,					//�豸��Ϣ�鿴 0x00A000A0
    &g_smiOSRestartTmn,						//�����ն� 0x00A000B0
    &g_smiOSFactoryReset,					//�ָ��������� 0x00A000C0
    &g_smiOS_SSHSet,						//SSH�������� 0x00A000D0
    &g_smiOSAppVersion,						//Ӧ�ð汾��Ϣ 0x00A000E0
    &g_smiOSUUpdate,                        //U������ 0x00A000F0
    &g_smiOSLogExport,						//��־���� 0x00A000F1
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

