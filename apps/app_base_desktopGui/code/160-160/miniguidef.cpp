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

extern PLUGIN_INFO g_layer_wirelessStatus_set;

PLUGIN_INFO* g_plugin[] =
{
    &g_puamr,
    &g_stamr,               //����ҳ��
    &g_Template1,			      // ��������
    &g_WelcomeLayer,				// 0xFFFFFE
    &g_layerPopMsg,			    // 0xFFFFFD

    // �˵�ҳ���
    &g_menuLayer,           // 1�в˵����  0x00A00000
    &g_menu2Layer,          // 2�в˵����  0x00A10000

    &g_layerInput,          // ���뷨�û�����  0x00a30000
    &g_layerTimeArea,       // ����ʱ�䷶Χ����0x00a40000
    &g_menu3Layer,			    //3�в˵����0x00A50000

    &g_layersetSafeONOFF,   //��ȫģʽ���ùر�0x000c0003
    &g_smiOSUUpdate,        //U������
    &g_layer_wirelessStatus_set,
    &g_smiOSLogExport,      //��־����
    &g_SSHSet_Layer,        //SSH��������
    &g_TCPDump_Set_Layer,   //TCPDump��������
    &g_passwd_Layer,        //�����޸�
    &g_LogClear_Layer,      //��־��� 0x00A000F3
    &g_ModuleUpdate,        //ģ������ 0x00A000F4
    &g_SystemCmd,           //System���� 0x00A000F5
    &g_EchoPrint,           //���Դ�ӡ 0x10A000F5
    &g_ACModuleUpdate,      //����ģ������ 0x00A000F6
    &g_CoUpdate,            //Э���������� 0x00A000FE
    &g_layer_bluetooth,     //������������
    &g_layer_CPU_temperature,    //CPU��ǰ�¶�
    &g_layer_power_status,       //��Դ����״̬
    &g_layer_ac_ver,        //���ɰ汾
    &g_layer_co_ver,        //Э�������汾
    & g_layer_comm_proxy,
    // ����ҳ���
    &g_layerValidatePwd,    // ������֤ 0x00600000

    &g_layerFreezeTime, 	  // ��֤���� 0x00600002
    
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

