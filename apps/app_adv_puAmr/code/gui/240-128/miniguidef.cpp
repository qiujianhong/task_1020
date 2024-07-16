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

extern PLUGIN_INFO g_layer_control_pc_display1;
extern PLUGIN_INFO g_layer_control_pc_display2;
extern PLUGIN_INFO g_layer_control_pc_display3;
extern PLUGIN_INFO g_layer_control_pc_display4;
extern PLUGIN_INFO g_layer_control_pc_display5;
extern PLUGIN_INFO g_layer_control_pc_display6;
extern PLUGIN_INFO g_layer_control_pc_display7;
extern PLUGIN_INFO g_layer_control_pc_display8;

extern PLUGIN_INFO g_layer_control_ec_display1;
extern PLUGIN_INFO g_layer_control_ec_display2;
extern PLUGIN_INFO g_layer_control_ec_display3;
extern PLUGIN_INFO g_layer_control_ec_display4;
extern PLUGIN_INFO g_layer_control_ec_display5;
extern PLUGIN_INFO g_layer_control_ec_display6;
extern PLUGIN_INFO g_layer_control_ec_display7;
extern PLUGIN_INFO g_layer_control_ec_display8;

extern PLUGIN_INFO g_layer_ctrl_period_display1;
extern PLUGIN_INFO g_layer_ctrl_period_display2;
extern PLUGIN_INFO g_layer_ctrl_period_display3;
extern PLUGIN_INFO g_layer_ctrl_period_display4;
extern PLUGIN_INFO g_layer_ctrl_period_display5;
extern PLUGIN_INFO g_layer_ctrl_period_display6;
extern PLUGIN_INFO g_layer_ctrl_period_display7;
extern PLUGIN_INFO g_layer_ctrl_period_display8;

extern PLUGIN_INFO g_layer_ctrl_rest_display1;
extern PLUGIN_INFO g_layer_ctrl_rest_display2;
extern PLUGIN_INFO g_layer_ctrl_rest_display3;
extern PLUGIN_INFO g_layer_ctrl_rest_display4;
extern PLUGIN_INFO g_layer_ctrl_rest_display5;
extern PLUGIN_INFO g_layer_ctrl_rest_display6;
extern PLUGIN_INFO g_layer_ctrl_rest_display7;
extern PLUGIN_INFO g_layer_ctrl_rest_display8;

extern PLUGIN_INFO g_layer_ctrl_stop_display1;
extern PLUGIN_INFO g_layer_ctrl_stop_display2;
extern PLUGIN_INFO g_layer_ctrl_stop_display3;
extern PLUGIN_INFO g_layer_ctrl_stop_display4;
extern PLUGIN_INFO g_layer_ctrl_stop_display5;
extern PLUGIN_INFO g_layer_ctrl_stop_display6;
extern PLUGIN_INFO g_layer_ctrl_stop_display7;
extern PLUGIN_INFO g_layer_ctrl_stop_display8;

extern PLUGIN_INFO g_layer_ctrl_down_display1;
extern PLUGIN_INFO g_layer_ctrl_down_display2;
extern PLUGIN_INFO g_layer_ctrl_down_display3;
extern PLUGIN_INFO g_layer_ctrl_down_display4;
extern PLUGIN_INFO g_layer_ctrl_down_display5;
extern PLUGIN_INFO g_layer_ctrl_down_display6;
extern PLUGIN_INFO g_layer_ctrl_down_display7;
extern PLUGIN_INFO g_layer_ctrl_down_display8;

extern PLUGIN_INFO g_layer_ctrl_month_display1;
extern PLUGIN_INFO g_layer_ctrl_month_display2;
extern PLUGIN_INFO g_layer_ctrl_month_display3;
extern PLUGIN_INFO g_layer_ctrl_month_display4;
extern PLUGIN_INFO g_layer_ctrl_month_display5;
extern PLUGIN_INFO g_layer_ctrl_month_display6;
extern PLUGIN_INFO g_layer_ctrl_month_display7;
extern PLUGIN_INFO g_layer_ctrl_month_display8;

extern PLUGIN_INFO g_layer_ctrl_turn_display1;
extern PLUGIN_INFO g_layer_ctrl_turn_display2;
extern PLUGIN_INFO g_layer_ctrl_turn_display3;
extern PLUGIN_INFO g_layer_ctrl_turn_display4;
extern PLUGIN_INFO g_layer_ctrl_turn_display5;
extern PLUGIN_INFO g_layer_ctrl_turn_display6;
extern PLUGIN_INFO g_layer_ctrl_turn_display7;
extern PLUGIN_INFO g_layer_ctrl_turn_display8;

extern PLUGIN_INFO g_layer_magneticEvent_display;
extern PLUGIN_INFO g_layer_ac_param_initialization;

extern PLUGIN_INFO g_layer_ZJ_left_display;

extern PLUGIN_INFO g_layer_esam_display;
extern PLUGIN_INFO g_layer_battery_display;

#ifdef AREA_FUJIAN
extern PLUGIN_INFO g_layer_GatherFJ_set;              //���ݲɼ�ģʽ
extern PLUGIN_INFO g_layer_rs485Status_set;            //RS485�˿����ݹ���
#endif


PLUGIN_INFO* g_plugin[] =
{
    // �˵�ҳ���
    &g_menuLayer,           // 1�в˵����  0x00A00000
    &g_layerInput,
    &g_layerValidatePwd,
    &g_layerPopMsg,
    &g_layer_ac_forward_power_display,   //���ɵ�ǰ�������ʾ��
    &g_layer_ac_reverse_power_display,   //���ɵ�ǰ�������ʾ��
    &g_layer_ac_quadrant_power_display,  //���ɵ�ǰ�������޹����� 
    &g_layer_ac_vol_cur_display,         //��ѹ��������
    &g_layer_ac_power_display,           //��ǰ����ʾ��
    &g_layer_ac_demand_display,          //�������й�����
    &g_layer_switch_status_display,      //ң��״̬
    &g_turnstart,                        //�ִ�״̬�鿴
    &g_layer_power_off_on_display,       //ͣ�ϵ��¼�
    &g_layer_magneticEvent_display,      //�㶨�ų������¼�
    &g_layer_power_ctr_evt_display,      //���ؼ�¼
    &g_layer_elect_ctr_evt_display, 
    &g_layer_buy_elect_evt_display,
    &g_layer_elect_warn_evt_display,
    &g_layer_measure_data_display,      //����ʵʱ���ݲ鿴
    &g_layer_measure_day_frozen_display,//�����ն������ݲ鿴
    &g_layer_harmonic_display,          //г��������
    &g_layer_harmonic_value_display,    //г������

    //�߾���
    &g_layer_ac_high_forward_power_display,   //���ɵ�ǰ�������ʾ��
    &g_layer_ac_high_reverse_power_display,   //���ɵ�ǰ�������ʾ��
    &g_layer_ac_high_quadrant_power_display,  //���ɵ�ǰ�������޹�����

//  &g_layer_dc_data_display,           //ֱ��ģ����

    &g_layerChangePwd,                  //��������
    &g_layer_restartAPP,                //����Ӧ��
//  &g_layer_reboot_terminal,           //�����ն�
    &g_layer_data_initialization,       //���ݳ�ʼ��
    &g_layer_param_initialization,      //������ʼ��
    &g_layer_ac_param_initialization,   //���ɲ�����ʼ��
    &g_layer_task_status,               //����״̬�鿴
    &g_layer_manual_amr,                //�ֶ��������
    &g_layer_line_net,                  //�ز��������
#ifdef AREA_HUNAN
    &g_layer_sensor_param,               //�����������鿴
    &g_layer_sensor_set,                //��������������
#endif    
    &g_layer_app_version_display,      //�汾��Ϣ 
    &g_layer_tmn_version_display,      //�汾��Ϣ
    &g_layer_comm_proxy_set,                      //ͨ�Ŵ�������
    
    &g_layer_address_display,               //ͨ�ŵ�ַ�鿴
    &g_layer_ip_display,                //IP��ַ�鿴
    &g_layer_ip_display1,                //IP��ַ�鿴
    &g_layer_eth_display,               //��̫��ͨ���鿴
    &g_layer_ethmaster_display,             //��̫����վ���ò鿴
    &g_layer_gprs_display,               //��̫��ͨ���鿴
    &g_layer_gprs2_display,               //��̫��ͨ���鿴
    &g_layer_gprs_numnet,                  //���߹���1����������
    &g_layer_gprs2_numnet,                  //���߹���2����������
#ifdef AREA_SHAANXI
    &g_layer_gprs1chanenl_param_set,
    &g_layer_gprs2chanenl_param_set,
#endif
    &g_layer_gprsmaster_display,            //GPRS��վ����
    &g_layer_gprs2master_display,
    &g_layer_meterfile_display,             //������鿴
    &g_layer_normal_plan_display,           //��ͨ�����鿴
    &g_layer_event_plan_display,            //�¼������鿴
    &g_layer_report_plan_display,           //�ϱ������鿴
    &g_layer_task_display,                  //�ɼ�����鿴
    &g_layer_HPLC_display,                   //HPLC�����鿴

    &g_layer_address_set,                   //ͨ�ŵ�ַ����
    &g_layer_ip_set,                    //IP��ַ����
    &g_layer_ip_set1,                    //IP��ַ����
    &g_layer_eth_set,                           //��̫��ͨ����������
    &g_layer_ethmaster_set,                    //��̫����վ��������
    &g_layer_gprs_set,                      //GPRSͨ����������
    &g_layer_gprs2_set,                      //GPRSͨ����������   
    &g_layer_gprsmaster_set,                  //GPRS��վ��������
    &g_layer_gprs2master_set,                  //GPRS��վ��������
    &g_layer_meterfile_set,                 //���������
    &g_layer_normal_plan_set,               //��ͨ�ɼ���������
    &g_layer_task_set,                      //�ɼ���������
    &g_layer_HPLC_set,                      //HPLC��������
    &g_layer_HPLCSearch,                    //�ز��ѱ�����
    &g_layer_Hchanenl_set,                  //�ز�Ƶ������
#ifdef AREA_FUJIAN
    &g_layer_GatherFJ_set,               //���ݲɼ�ģʽ
    &g_layer_rs485Status_set,            //RS485�˿����ݹ���
#endif      
    &g_layer_plus_data_display,       //��������
    &g_layer_gruop_data_display1,       // �ܼ������� 0x11011000
    &g_layer_gruop_data_display2,
    &g_layer_gruop_data_display3,
    &g_layer_gruop_data_display4,
    &g_layer_gruop_data_display5,
    &g_layer_gruop_data_display6,
    &g_layer_gruop_data_display7,
    &g_layer_gruop_data_display8,

    &g_layer_control_pc_display1,             //����״̬1 0x11012100
    &g_layer_control_pc_display2,
    &g_layer_control_pc_display3,
    &g_layer_control_pc_display4,
    &g_layer_control_pc_display5,
    &g_layer_control_pc_display6,
    &g_layer_control_pc_display7,
    &g_layer_control_pc_display8,

    &g_layer_control_ec_display1,             //����״̬1 0x11012200
    &g_layer_control_ec_display2,
    &g_layer_control_ec_display3,
    &g_layer_control_ec_display4,
    &g_layer_control_ec_display5,
    &g_layer_control_ec_display6,
    &g_layer_control_ec_display7,
    &g_layer_control_ec_display8,
    
    &g_layer_control_group_display1,        // �ܼ������״̬1 0x11012000
    &g_layer_control_group_display2,
    &g_layer_control_group_display3,
    &g_layer_control_group_display4,
    &g_layer_control_group_display5,
    &g_layer_control_group_display6,
    &g_layer_control_group_display7,
    &g_layer_control_group_display8,

    &g_layer_plus_param1,                  //��������
    &g_layer_plus_param2,
    &g_layer_plus_param3,
    &g_layer_plus_param4,
    &g_layer_plus_param5,
    &g_layer_plus_param6,
    &g_layer_plus_param7,
    &g_layer_plus_param8,

    &g_layer_gruop_param1,                //�ܼ�������
    &g_layer_gruop_param2,
    &g_layer_gruop_param3,
    &g_layer_gruop_param4,
    &g_layer_gruop_param5,
    &g_layer_gruop_param6,
    &g_layer_gruop_param7,
    &g_layer_gruop_param8,

    &g_BuyDatadisplay1,     // ������Ϣ1 0x11030040
    &g_BuyDatadisplay2,
    &g_BuyDatadisplay3,
    &g_BuyDatadisplay4,
    &g_BuyDatadisplay5,
    &g_BuyDatadisplay6,
    &g_BuyDatadisplay7,
    &g_BuyDatadisplay8,

    &g_layer_chinese_info_display1,    //������Ϣ
    &g_layer_chinese_info_display2,

    &g_layer_reset_time,          //�ն�ʱ������

    &g_module_info_display1,                //ģ����Ϣ  0x11050011
    &g_module_info_display2,
    &g_module_info_display3,
    &g_module_info_display4,
    &g_module_info_display5,

    &g_layer_ctrl_period_display1, //ʱ�οز��� 0x11031010
    &g_layer_ctrl_period_display2,
    &g_layer_ctrl_period_display3,
    &g_layer_ctrl_period_display4,
    &g_layer_ctrl_period_display5,
    &g_layer_ctrl_period_display6,
    &g_layer_ctrl_period_display7,
    &g_layer_ctrl_period_display8,

    &g_layer_ctrl_rest_display1,    //���ݿز��� 0x11031020
    &g_layer_ctrl_rest_display2,
    &g_layer_ctrl_rest_display3,
    &g_layer_ctrl_rest_display4,
    &g_layer_ctrl_rest_display5,
    &g_layer_ctrl_rest_display6,
    &g_layer_ctrl_rest_display7,
    &g_layer_ctrl_rest_display8,

    &g_layer_ctrl_stop_display1,   //��ͣ�ز��� 0x11031030
    &g_layer_ctrl_stop_display2,
    &g_layer_ctrl_stop_display3,
    &g_layer_ctrl_stop_display4,
    &g_layer_ctrl_stop_display5,
    &g_layer_ctrl_stop_display6,
    &g_layer_ctrl_stop_display7,
    &g_layer_ctrl_stop_display8,

    &g_layer_ctrl_down_display1,   //�¸��ز��� 0x11031040
    &g_layer_ctrl_down_display2,
    &g_layer_ctrl_down_display3,
    &g_layer_ctrl_down_display4,
    &g_layer_ctrl_down_display5,
    &g_layer_ctrl_down_display6,
    &g_layer_ctrl_down_display7,
    &g_layer_ctrl_down_display8,

    &g_layer_ctrl_month_display1,   //�¸��ز��� 0x11031050
    &g_layer_ctrl_month_display2,
    &g_layer_ctrl_month_display3,
    &g_layer_ctrl_month_display4,
    &g_layer_ctrl_month_display5,
    &g_layer_ctrl_month_display6,
    &g_layer_ctrl_month_display7,
    &g_layer_ctrl_month_display8,
    
    &g_layer_ctrl_turn_display1,   //�ִβ��� 0x11031060
    &g_layer_ctrl_turn_display2,
    &g_layer_ctrl_turn_display3,
    &g_layer_ctrl_turn_display4,
    &g_layer_ctrl_turn_display5,
    &g_layer_ctrl_turn_display6,
    &g_layer_ctrl_turn_display7,
    &g_layer_ctrl_turn_display8,
    &g_safemode,//��ȫģʽ
    &g_layer_address_set_hunan,                   //ͨ�ŵ�ַ����

    &g_layer_ZJ_left_display,    /* ʣ����� */

    &g_layer_esam_display,
    &g_layer_battery_display,
};
int g_nPluginCnt = sizeof(g_plugin) / sizeof(g_plugin[0]);

