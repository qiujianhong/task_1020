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
    // �˵�ҳ���
    &g_menuLayer,           // 1�в˵����  0x00A00000
    &g_layerLcStatus,            //��ǰ������·״̬
    &g_layerHisStatus,           //��ʷ������·״̬
    &g_layerRestartAPP,          //����Ӧ��
    &g_layerVersionInfo,         //�汾��Ϣ
    &g_layerCoreInfo,            //���İ���Ϣ
    &g_layerCTAInfo,             //A�໥������Ϣ
    &g_layerCTBInfo,             //B�໥������Ϣ
    &g_layerCTCInfo,             //C�໥������Ϣ
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

