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

PLUGIN_INFO* g_plugin[] =
{
    // �˵�ҳ���
    &g_menuLayer,           // 1�в˵����  0x00A00000
	&g_layerValidatePwd,
	
    &g_moduleInfo1,         //ģ��1��Ϣ
    &g_moduleInfo2,			//ģ��2��Ϣ
    &g_moduleInfo3,         //ģ��3��Ϣ
    &g_moduleInfo4,			//ģ��4��Ϣ
    &g_moduleInfo5,         //ģ��5��Ϣ
    &g_modulePlugEvent,     //ģ�����¼�
    &g_appRestart,            //����Ӧ��
    &g_moduleRestart,         //����ĳ��ģ��
    &g_moduleReset,           //��λĳ��ģ��
    &g_dataInitilize,			//���ݳ�ʼ��
    &g_paraInitilize,         //������ʼ��
    &g_appVersionInfo,        //Ӧ�ð汾��Ϣ
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

