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

extern PLUGIN_INFO g_layer_show_server_master;
extern PLUGIN_INFO g_layer_show_server_channel;
extern PLUGIN_INFO g_layer_show_eth_channel;
extern PLUGIN_INFO g_layer_show_ppp_channel;
extern PLUGIN_INFO g_layer_set_client_channel;
extern PLUGIN_INFO g_layer_set_client_eth;
extern PLUGIN_INFO g_layer_set_client_ppp;

PLUGIN_INFO* g_plugin[] =
{
    // �˵�ҳ���
    &g_menuLayer,           // 1�в˵����  0x00A00000
    &g_socketstart,
	&g_remotestatus,
    &g_layerInput,
    &g_layerValidatePwd,
    &g_layer_ac_forward_power_display,   //���ɵ�ǰ�������ʾ��
    &g_layer_ac_reverse_power_display,   //���ɵ�ǰ�������ʾ��
    &g_layer_ac_quadrant_power_display,  //���ɵ�ǰ�������޹����� 
    &g_layer_ac_vol_cur_display,         //��ѹ��������
    &g_layer_ac_power_display,           //��ǰ����ʾ��
    &g_layer_ac_demand_display,          //�������й�����
    &g_layer_measure_data_display,      //�������ݲ鿴

    &g_layer_ethmaster101_display,      //ͨ��ͨ���鿴
    &g_layer_ethmaster104_display,
    &g_layer_ethmaster101_set,      //ͨ��ͨ������
    &g_layer_ethmaster104_set,
    &g_layer_pass_rate_param_display,   //�ϸ��ʲ���
    &g_layer_pass_rate_param_set,

    &g_layer_restartAPP,      //����Ӧ��
    &g_layer_data_initialization,      //���ݳ�ʼ��
    &g_layer_param_initialization,      //������ʼ��
    &g_layer_app_version_display,      //�汾��Ϣ 

    &g_layer_show_server_master,  //������վ(S)״̬
    &g_layer_show_server_channel, //ͨ��ͨ�����Ͳ鿴
    &g_layer_show_eth_channel,    //��̫�������鿴
    &g_layer_show_ppp_channel,    //���߲����鿴

    &g_layer_set_client_channel,  //���ÿͻ���ͨ��
    &g_layer_set_client_eth,      //������̫������
    &g_layer_set_client_ppp,      //�������߲���
};
int g_nPluginCnt = sizeof(g_plugin) / sizeof(g_plugin[0]);

