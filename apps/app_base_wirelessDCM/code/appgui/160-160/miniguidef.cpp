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
    // �˵�ҳ���
    &g_menuLayer,           // 1�в˵����  0x00A00000
    &g_layerInput,
    &g_layerValidatePwd,
    &g_layerDialingStatusData1,         //���߲���״̬
    &g_layerDialingStatusData2,
    &g_layerDialingEventData1,          //���߲����¼�
    &g_layerDialingEventData2,
    &g_layerModuleData1,
    &g_layerModuleData2,
    &g_layerGPSData1,                   //λ����Ϣ
    &g_layerGPSData2, 
    &g_layerATShowData1,                //ATָ��
    &g_layerATShowData2,
    &g_layerATShowData3,
    &g_layerATShowData4,
    &g_layerDialingParamData1,          //���Ų�����ʾ
    &g_layerDialingParamData2,            
    &g_layerDialingPARASet1,            //���Ų�������
    &g_layerDialingPARASet2,
    &g_layerDialingRestartAPP,          //����Ӧ��
    &g_layerReDialing1,                 //���²���
    &g_layerReDialing2,
    &g_layerDialVersionInfo,            //�汾��Ϣ
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

