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
    
    &g_layer_HPLC_frequency,              //HPLCƵ��  
    &g_layer_meter_reading_port,          //����˿�
    &g_layer_module_address,              //ģ���ַ  
    &g_layer_module_info,                 //ģ����Ϣ
    &g_layer_meter_reading_status,        //����״̬
    &g_layer_station_identification,      //̨��ʶ��      
    &g_layer_off_grid_awareness,          //������֪
    &g_layer_reject_node_report,          //�ܾ��ڵ��ϱ�
    &g_layer_file_info,                   //������Ϣ
    &g_layer_restart_app,                 //����Ӧ��
    &g_layer_hardware_initialization,     //Ӳ����ʼ��
    &g_layer_param_initialization,        //������ʼ��
    &g_layer_app_version_info,            //Ӧ�ð汾��Ϣ
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

