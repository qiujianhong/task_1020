/*
***************************************************************************
  Copyright (c) 2006-2007,�Ͼ��������������������ι�˾
             All rights reserved.
  ��������: main.h
  ��    �ܣ�FKGA42��ʾģ���160*160ҳ���ͷ�ļ�����������ҳ������
  �� �� �ˣ�gaoyf
  ����ʱ�䣺2007-1
  
  �汾��ʷ:
  
    v1.0:
      2006-1-19, gaoyf : New file        
  
***************************************************************************
*/

#ifndef _LAYER_H_
#define _LAYER_H_
/***************************ר��1��****************************
****************************************************************/
extern PLUGIN_INFO g_CommDispDraw1;		// ���Ի���  0x20ffff01
extern PLUGIN_INFO g_CommDispDraw2;		// ���Ի���  0x20ffff02
extern PLUGIN_INFO g_CommDispDraw3;		// ���Ի���  0x20ffff03
extern PLUGIN_INFO g_CommDispDraw4;		// ���Ի���  0x20ffff04
extern PLUGIN_INFO g_CommDispDraw5;		// ���Ի���  0x20ffff05
extern PLUGIN_INFO g_CommDispDraw6;		// ���Ի���  0x20ffff06
extern PLUGIN_INFO g_CommDispDraw7;		// ���Ի���  0x20ffff07
extern PLUGIN_INFO g_CommDispDraw8;		// ���Ի���  0x20ffff08

extern PLUGIN_INFO g_layer_cur_powerTotal;   			//��ǰ����
extern PLUGIN_INFO g_layer_gruop_cur_powerTotal;   			//��ǰ����
extern PLUGIN_INFO g_layer_active_powerTotal;          //�й�����
extern PLUGIN_INFO g_layer_reactive_powerTotal;       //�޹�����
extern PLUGIN_INFO g_layer_power_display;            //�޹�����
extern PLUGIN_INFO g_layer_ac_high_active_power_display; //��ǰ�����й�������ʾֵ
extern PLUGIN_INFO g_layer_ac_high_reactive_power_display;//��ǰ�����޹�������ʾֵ
extern PLUGIN_INFO g_layer_high_reverse_power_display;   //��ǰ�����޹�������ʾֵ
extern PLUGIN_INFO g_layer_high_reactive_power_display; //��ǰ�����޹�������ʾֵ
extern PLUGIN_INFO g_layer_cur_EleQ;					//��ǰ����
extern PLUGIN_INFO g_layer_power_ctr_evt_display;		//���ؼ�¼
extern PLUGIN_INFO g_layer_elect_ctr_evt_display;		//��ؼ�¼

extern PLUGIN_INFO g_layer_pwct_ctr_evt_display;		//ʧ���¼

extern PLUGIN_INFO g_layer_rct_ctr_evt_display;			//ң�ؼ�¼
extern PLUGIN_INFO g_layer_switch_state_display;		//����״̬
extern PLUGIN_INFO g_layer_periodCtrl_param_display;	//ʱ�οز���
extern PLUGIN_INFO g_layer_restCtrl_param_display;		//���ݿز���
extern PLUGIN_INFO g_layer_monthCtrl_param_display;		//�µ�ز���
extern PLUGIN_INFO g_layer_downCtrl_param_display;		//�¸��ز���

extern PLUGIN_INFO g_layer_stopCtrl_param_display; 		//��ͣ�ز���
extern PLUGIN_INFO g_layer_kivp_display;          		//kikvkp
extern PLUGIN_INFO g_layer_kivp_puls_display;         //���廥��
extern PLUGIN_INFO g_layer_meterParam_display;    		//������
extern PLUGIN_INFO g_layer_powerCurve_display;    		//��������

/************************************************************************/
/* ����״̬                                                                 */
/************************************************************************/
extern PLUGIN_INFO g_ControlStatusDraw1;	// ����״̬	0x00010101
extern PLUGIN_INFO g_ControlStatusDraw2;	// ����״̬	0x00010201
extern PLUGIN_INFO g_ControlStatusDraw3;	// ����״̬	0x00010301
extern PLUGIN_INFO g_ControlStatusDraw4;	// ����״̬	0x00010401
extern PLUGIN_INFO g_ControlStatusDraw5;	// ����״̬	0x00010501
extern PLUGIN_INFO g_ControlStatusDraw6;	// ����״̬	0x00010601
extern PLUGIN_INFO g_ControlStatusDraw7;	// ����״̬	0x00010701
extern PLUGIN_INFO g_ControlStatusDraw8;	// ����״̬	0x00010801
/************************************************************************/
/* ����״̬ ����                                                           */
/************************************************************************/

/************************************************************************/
/* ����������                                                           */
/************************************************************************/
extern PLUGIN_INFO g_EquipValDraw1;			// ���ʾ��  0x20300000
extern PLUGIN_INFO g_EquipValDraw2;			// ���ʾ��  0x20300001
extern PLUGIN_INFO g_EquipValDraw3;			// ���ʾ��  0x20300002
extern PLUGIN_INFO g_EquipValDraw4;			// ���ʾ��  0x20300003
extern PLUGIN_INFO g_EquipValDraw5;			// ���ʾ��  0x20300004
extern PLUGIN_INFO g_EquipValDraw6;			// ���ʾ��  0x20300005
extern PLUGIN_INFO g_EquipValDraw7;			// ���ʾ��  0x20300006
extern PLUGIN_INFO g_EquipValDraw8;			// ���ʾ��  0x20300007
extern PLUGIN_INFO g_EquipValDraw9;			// ���ʾ��  0x20300008
extern PLUGIN_INFO g_EquipValDraw10;			// ���ʾ��  0x20300009
extern PLUGIN_INFO g_EquipValDraw11;			// ���ʾ��  0x2030000a
extern PLUGIN_INFO g_EquipValDraw12;			// ���ʾ��  0x2030000b
extern PLUGIN_INFO g_EquipValDraw13;			// ���ʾ��  0x2030000c
extern PLUGIN_INFO g_EquipValDraw14;			// ���ʾ��  0x2030000d
extern PLUGIN_INFO g_EquipValDraw15;			// ���ʾ��  0x2030000e
extern PLUGIN_INFO g_EquipValDraw16;			// ���ʾ��  0x2030000f

extern PLUGIN_INFO g_HEquipValDraw1;			// ���ʾ��  0x20300000
extern PLUGIN_INFO g_HEquipValDraw2;			// ���ʾ��  0x20300001
extern PLUGIN_INFO g_HEquipValDraw3;			// ���ʾ��  0x20300002
extern PLUGIN_INFO g_HEquipValDraw4;			// ���ʾ��  0x20300003
extern PLUGIN_INFO g_HEquipValDraw5;			// ���ʾ��  0x20300004
extern PLUGIN_INFO g_HEquipValDraw6;			// ���ʾ��  0x20300005
extern PLUGIN_INFO g_HEquipValDraw7;			// ���ʾ��  0x20300006
extern PLUGIN_INFO g_HEquipValDraw8;			// ���ʾ��  0x20300007
extern PLUGIN_INFO g_HEquipValDraw9;			// ���ʾ��  0x20300008
extern PLUGIN_INFO g_HEquipValDraw10;			// ���ʾ��  0x20300009
extern PLUGIN_INFO g_HEquipValDraw11;			// ���ʾ��  0x2030000a
extern PLUGIN_INFO g_HEquipValDraw12;			// ���ʾ��  0x2030000b
extern PLUGIN_INFO g_HEquipValDraw13;			// ���ʾ��  0x2030000c
extern PLUGIN_INFO g_HEquipValDraw14;			// ���ʾ��  0x2030000d
extern PLUGIN_INFO g_HEquipValDraw15;			// ���ʾ��  0x2030000e
extern PLUGIN_INFO g_HEquipValDraw16;			// ���ʾ��  0x2030000f
/************************************************************************/
/* ���������ݽ���                                                       */
/************************************************************************/


/************************************************************************/
/* ������Ϣҳ���                                                           */
/************************************************************************/
extern PLUGIN_INFO g_ZhMsgInfoDraw1;		// ������Ϣ1  0x20400000
extern PLUGIN_INFO g_ZhMsgInfoDraw2;		// ������Ϣ2  0x20400001	
extern PLUGIN_INFO g_ZhMsgInfoDraw3;		// ������Ϣ3	0x20400002
extern PLUGIN_INFO g_ZhMsgInfoDraw4;		// ������Ϣ4	0x20400003
extern PLUGIN_INFO g_ZhMsgInfoDraw5;		// ������Ϣ5	0x20400004
extern PLUGIN_INFO g_ZhMsgInfoDraw6;		// ������Ϣ6	0x20400005
extern PLUGIN_INFO g_ZhMsgInfoDraw7;		// ������Ϣ7	0x20400006
extern PLUGIN_INFO g_ZhMsgInfoDraw8;		// ������Ϣ8	0x20400007
extern PLUGIN_INFO g_ZhMsgInfoDraw9;		// ������Ϣ9	0x20400008
extern PLUGIN_INFO g_ZhMsgInfoDraw10;		// ������Ϣ10   0x20400009

/************************************************************************/
/* ������Ϣҳ������                                                       */

/************************************************************************/
/* ������Ϣ                                                              */
/************************************************************************/
extern PLUGIN_INFO g_BuyDataDraw1;	// ������Ϣ	0x00010100
extern PLUGIN_INFO g_BuyDataDraw2;	// ������Ϣ	0x00010200
extern PLUGIN_INFO g_BuyDataDraw3;	// ������Ϣ	0x00010300
extern PLUGIN_INFO g_BuyDataDraw4;	// ������Ϣ	0x00010400
extern PLUGIN_INFO g_BuyDataDraw5;	// ������Ϣ	0x00010500
extern PLUGIN_INFO g_BuyDataDraw6;	// ������Ϣ	0x00010600
extern PLUGIN_INFO g_BuyDataDraw7;	// ������Ϣ	0x00010700
extern PLUGIN_INFO g_BuyDataDraw8;	// ������Ϣ	0x00010800
/************************************************************************/
/*  ������Ϣ����                                                             */
/************************************************************************/


//extern PLUGIN_INFO g_layerChnlInfo ;  //ͨѶͨ������
extern PLUGIN_INFO g_layerEthernet ;  //ͨѶͨ������
extern PLUGIN_INFO g_layer_gprs_set ;  //ͨѶͨ������
extern PLUGIN_INFO g_layer_networkmode_set ;  //ͨѶͨ������


extern PLUGIN_INFO g_layerGroupMain;	  // �ܼ�������  0x20760000
extern PLUGIN_INFO g_layerPulseMain;	  // �������������  0x20740000

extern PLUGIN_INFO g_TermDataDraw;		  // �ն���Ϣ  0x20600000

extern PLUGIN_INFO g_NetmodeDraw;

extern PLUGIN_INFO g_ModemversionDraw;

extern PLUGIN_INFO g_layerRadioCfg;		// ��̨�������� 0x20B70000


extern PLUGIN_INFO g_layer_aesam;		// ESAM�·�״̬


/***************************ר�� ����****************************
****************************************************************/
//�߾���
extern PLUGIN_INFO g_layer_ac_high_forward_power_display ;  //���ɵ�ǰ�������ʾ��
extern PLUGIN_INFO g_layer_ac_high_reverse_power_display ;  //���ɵ�ǰ�������ʾ��
extern PLUGIN_INFO g_layer_ac_high_quadrant_power_display ; //���ɵ�ǰ�������޹�����

extern PLUGIN_INFO g_layer_ac_forward_power_display ;  //���ɵ�ǰ�������ʾ��
extern PLUGIN_INFO g_layer_ac_reverse_power_display ;  //���ɵ�ǰ�������ʾ��
extern PLUGIN_INFO g_layer_ac_quadrant_power_display ; //���ɵ�ǰ�������޹�����
extern PLUGIN_INFO g_layer_ac_vol_cur_display ;        //��ѹ��������
extern PLUGIN_INFO g_layer_ac_vol_xj_display ;        //��ѹ���
extern PLUGIN_INFO g_layer_ac_power_display;           //��ǰ����ʾ��
extern PLUGIN_INFO g_layer_ac_demand_display;          //�������й�����
extern PLUGIN_INFO g_layer_ac_remainPower_display1;     //��ʣ�������һ��
extern PLUGIN_INFO g_layer_ac_remainPower_display2;     //��ʣ������ڶ���
extern PLUGIN_INFO g_layer_switch_status_display;      //ң��״̬
extern PLUGIN_INFO g_layer_pwct_ctr_evt_display;      //ͣ�ϵ��¼�
extern PLUGIN_INFO g_layer_rct_ctr_evt_display;      
extern PLUGIN_INFO g_layer_elect_ctr_evt_display;
extern PLUGIN_INFO g_layer_buy_elect_evt_display;
extern PLUGIN_INFO g_layer_elect_warn_evt_display;
extern PLUGIN_INFO g_layer_measure_data_display;      //����ʵʱ���ݲ鿴
extern PLUGIN_INFO g_layer_measure_day_frozen_display; //�����ն������ݲ鿴
extern PLUGIN_INFO g_layer_harmonic_display;          //г��������
extern PLUGIN_INFO g_layer_harmonic_value_display;    //г������
extern PLUGIN_INFO g_layer_dc_data_display;           //ֱ��ģ����

extern PLUGIN_INFO g_layer_address_display;               //ͨ�ŵ�ַ�鿴
extern PLUGIN_INFO g_layer_ip_display;                   //IP��ַ�鿴
extern PLUGIN_INFO g_layer_eth_display;               //��̫��ͨ���鿴
extern PLUGIN_INFO g_layer_ethmaster_display;         //��̫����վͨ�Ų����鿴
extern PLUGIN_INFO g_layer_gprs_display;               //���߹���ͨ���鿴
extern PLUGIN_INFO g_layer_gprs2_display;               //���߹���ͨ���鿴
extern PLUGIN_INFO g_layer_gprs_numnet;                 //���߹���1����������
extern PLUGIN_INFO g_layer_gprs2_numnet;                 //���߹���2����������
#ifdef AREA_SHAANXI
extern PLUGIN_INFO g_layer_gprs1chanenl_param_set;
extern PLUGIN_INFO g_layer_gprs2chanenl_param_set;
#endif
extern PLUGIN_INFO g_layer_gprsmaster_display;            //GPRS��վ����
extern PLUGIN_INFO g_layer_gprs2master_display;            //GPRS��վ����
extern PLUGIN_INFO g_layer_meterfile_display;             //������鿴
extern PLUGIN_INFO g_layer_normal_plan_display;         //��ͨ�����鿴
extern PLUGIN_INFO g_layer_event_plan_display;          //�¼������鿴
extern PLUGIN_INFO g_layer_report_plan_display;         //�ϱ������鿴
extern PLUGIN_INFO g_layer_task_display;                 //�ɼ�����鿴
extern PLUGIN_INFO g_layer_HPLC_display;                //HPLC�����鿴

extern PLUGIN_INFO g_Set_ParaResetTime;		//ʱ������
extern PLUGIN_INFO g_layerChangePwd;      //��������
extern PLUGIN_INFO g_layerac_FeatureCfg;  //���ɵ�������
extern PLUGIN_INFO g_layer_restartAPP;      //����Ӧ��
extern PLUGIN_INFO g_layer_reboot_terminal;      //�����ն�
extern PLUGIN_INFO g_layer_data_initialization;      //���ݳ�ʼ��
extern PLUGIN_INFO g_layer_param_initialization;      //������ʼ��
extern PLUGIN_INFO g_layer_ac_param_initialization;      //���ɲ�����ʼ��
extern PLUGIN_INFO g_layer_data_param_initialization;    //�������ݳ�ʼ��
extern PLUGIN_INFO g_layer_task_status;      			 //����״̬�鿴
extern PLUGIN_INFO g_layer_task_fail;                    //����ʧ�ܵ��

extern PLUGIN_INFO g_layer_manual_amr;       //�ֶ��������
extern PLUGIN_INFO g_layer_line_net;           //�ز��������
#ifdef AREA_HUNAN
extern PLUGIN_INFO g_layer_sensor_param;          //�����������鿴
extern PLUGIN_INFO g_layer_sensor_set;            //��������������
#endif

extern PLUGIN_INFO g_layer_app_version_display;     //�汾��Ϣ
extern PLUGIN_INFO g_UpdateStatusDraw;              //����״̬
extern PLUGIN_INFO g_gprsStatusDraw;                //������Ϣ
extern PLUGIN_INFO g_ChannelDebugStatusDraw;        //ͨѶ��Ϣ
extern PLUGIN_INFO g_layer_tmn_version_display;     //�ն˰汾��Ϣ
extern PLUGIN_INFO g_layer_comm_proxy_set;          //ͨ�Ŵ�������
extern PLUGIN_INFO g_layer_address_set;              //ͨ�ŵ�ַ����
extern PLUGIN_INFO g_layer_address_set1;             //ͨ�ŵ�ַ����

extern PLUGIN_INFO g_layer_channel_set;                 //ͨ��ͨ������
extern PLUGIN_INFO g_layer_ethmaster101_set;
extern PLUGIN_INFO g_layer_ethmaster104_set;
extern PLUGIN_INFO g_layer_meterfile_set;                 //���������
extern PLUGIN_INFO g_layer_normal_plan_set;           //��ͨ�ɼ���������
extern PLUGIN_INFO g_layer_task_set;                  //�ɼ���������
extern PLUGIN_INFO g_layer_event_plan_set;           //�¼��ɼ���������

extern PLUGIN_INFO g_layer_HPLC_set;                 //HPLC��������
extern PLUGIN_INFO g_layer_Hchanenl_set;             //�ز�Ƶ������
extern PLUGIN_INFO g_layer_ip_set;                      //ip��ַ����
extern PLUGIN_INFO g_layer_eth_set;                     //��̫��ͨ����������
extern PLUGIN_INFO g_layer_ethmaster_set;                   //��̫����վ��������
extern PLUGIN_INFO g_layer_gprs_set;                     //GPRSͨ����������
//extern PLUGIN_INFO g_layer_networkmode_set;                     //GPRSͨ����������

extern PLUGIN_INFO g_layer_gprsmaster_set;                     //GPRS��վ����
extern PLUGIN_INFO g_layer_gprs2_set;                     //GPRSͨ����������
extern PLUGIN_INFO g_layer_gprs2master_set;                     //GPRS��վ����

extern PLUGIN_INFO g_CbStatusDraw;                        // ��ǰ����״̬
extern PLUGIN_INFO g_CbMtStatusDraw1;                    // �����㳭��״̬
extern PLUGIN_INFO g_CbMtStatusDraw2;                    // �����㳭��״̬
extern PLUGIN_INFO g_CbMtStatusDraw3;                    // �����㳭��ɹ���
extern PLUGIN_INFO g_layer_GatherFJ_set;
extern PLUGIN_INFO g_layer_rs485Status_set;

extern PLUGIN_INFO g_ZhMsgInfoDraw1;   //������Ϣ
extern PLUGIN_INFO g_ZhMsgInfoDraw2;
extern PLUGIN_INFO g_ZhMsgInfoDraw3;
extern PLUGIN_INFO g_ZhMsgInfoDraw4;
extern PLUGIN_INFO g_ZhMsgInfoDraw5;
extern PLUGIN_INFO g_ZhMsgInfoDraw6;
extern PLUGIN_INFO g_ZhMsgInfoDraw7;
extern PLUGIN_INFO g_ZhMsgInfoDraw8;
extern PLUGIN_INFO g_ZhMsgInfoDraw9;
extern PLUGIN_INFO g_ZhMsgInfoDraw10;


extern PLUGIN_INFO g_puamr;
extern PLUGIN_INFO g_stamr;

extern PLUGIN_INFO g_Template1;			// ��������
extern PLUGIN_INFO g_WelcomeLayer;			// 0xFFFFFE
extern PLUGIN_INFO g_layerPopMsg;			// 0xFFFFFD
#if proAdvanceEN2 == 1
extern PLUGIN_INFO g_CommucationParaResetLayer;	// ͨѶ������ʼ��       0x00FFFFFC
#endif
// �˵�ҳ���
extern PLUGIN_INFO g_menuLayer;         // 1�в˵����  0x00A00000
extern PLUGIN_INFO g_menu2Layer;        // 2�в˵����  0x00A10000
extern PLUGIN_INFO g_menu3Layer;        // 3�в˵����  0x00A50000

extern PLUGIN_INFO g_layerInput;        // ���뷨�û�����  0x00a20000
extern PLUGIN_INFO g_layerTimeArea;     // ����ʱ�䷶Χ����0x00a30000
extern PLUGIN_INFO g_RemoteConStatusDraw;

extern PLUGIN_INFO g_menuMeasureLayer;  // Measure�˵����  0x00A20000

extern PLUGIN_INFO g_layerInput;        // ���뷨�û�����  0x00a30000
extern PLUGIN_INFO g_layerTimeArea;     // ����ʱ�䷶Χ����0x00a40000
#if HARDWARE_2012_EN ==1
extern PLUGIN_INFO g_SetparaLayer; 
extern PLUGIN_INFO g_Set_ParaResetTime;
extern PLUGIN_INFO g_TerminalAddr;
extern PLUGIN_INFO g_ChangePwd;
#endif
extern PLUGIN_INFO g_layerSetGPRSMode;	//GPRS����ģʽ 0x21B70088

extern PLUGIN_INFO g_smiOSLogExport;	//��־���� 0x00A000F1
extern PLUGIN_INFO g_smiOSUUpdate;	    //U������ 0x00A000F0

extern PLUGIN_INFO g_SSHSet_Layer;	    //SSH�������� 0x00A000D0
extern PLUGIN_INFO g_TCPDump_Set_Layer;	//TCPDump�������� 0x00A000D1
extern PLUGIN_INFO g_passwd_Layer;	    //�ָ�Ĭ������ 0x00A000F2
extern PLUGIN_INFO g_LogClear_Layer;    //��־��� 0x00A000F3
extern PLUGIN_INFO g_ModuleUpdate;      //ģ������ 0x00A000F4
extern PLUGIN_INFO g_SystemCmd;         //System���� 0x00A000F5
extern PLUGIN_INFO g_EchoPrint;         //���Դ�ӡ 0x10A000F5
extern PLUGIN_INFO g_ACModuleUpdate;    //����ģ������ 0x00A000F6
extern PLUGIN_INFO g_layer_bluetooth;   //������������
extern PLUGIN_INFO g_layer_CPU_temperature;    //CPU��ǰ�¶�
extern PLUGIN_INFO g_layer_power_status;       //��Դ����״̬
extern PLUGIN_INFO g_layer_ac_ver;       //���ɰ汾
extern PLUGIN_INFO g_layer_comm_proxy;      //����ͨ�Ŵ�������
extern PLUGIN_INFO g_layer_wirelessStatus_set;
extern PLUGIN_INFO g_disk_status;        //����״̬



/************************************************************************/
/* ʵʱ��Ϣ                                                           */
/************************************************************************/
extern PLUGIN_INFO g_RemoteConStatusDraw;       // ң��״̬ 
extern PLUGIN_INFO g_CurrStatusDraw;            // ����״̬

extern PLUGIN_INFO g_RepstopConStatusDraw1;       // ��ͣ��״̬
extern PLUGIN_INFO g_RepstopConStatusDraw2;       // ��ͣ��״̬
extern PLUGIN_INFO g_RepstopConStatusDraw3;       // ��ͣ��״̬
extern PLUGIN_INFO g_RepstopConStatusDraw4;       // ��ͣ��״̬
extern PLUGIN_INFO g_RepstopConStatusDraw5;       // ��ͣ��״̬
extern PLUGIN_INFO g_RepstopConStatusDraw6;       // ��ͣ��״̬
extern PLUGIN_INFO g_RepstopConStatusDraw7;       // ��ͣ��״̬
extern PLUGIN_INFO g_RepstopConStatusDraw8;       // ��ͣ��״̬

extern PLUGIN_INFO g_OverflowConStatusDraw1;       // �¸���״̬
extern PLUGIN_INFO g_OverflowConStatusDraw2;       // �¸���״̬
extern PLUGIN_INFO g_OverflowConStatusDraw3;       // �¸���״̬
extern PLUGIN_INFO g_OverflowConStatusDraw4;       // �¸���״̬
extern PLUGIN_INFO g_OverflowConStatusDraw5;       // �¸���״̬
extern PLUGIN_INFO g_OverflowConStatusDraw6;       // �¸���״̬
extern PLUGIN_INFO g_OverflowConStatusDraw7;       // �¸���״̬
extern PLUGIN_INFO g_OverflowConStatusDraw8;       // �¸���״̬

extern PLUGIN_INFO g_RestConStatusDraw1;       // ���ݿ�״̬
extern PLUGIN_INFO g_RestConStatusDraw2;       // ���ݿ�״̬
extern PLUGIN_INFO g_RestConStatusDraw3;       // ���ݿ�״̬
extern PLUGIN_INFO g_RestConStatusDraw4;       // ���ݿ�״̬
extern PLUGIN_INFO g_RestConStatusDraw5;       // ���ݿ�״̬
extern PLUGIN_INFO g_RestConStatusDraw6;       // ���ݿ�״̬
extern PLUGIN_INFO g_RestConStatusDraw7;       // ���ݿ�״̬
extern PLUGIN_INFO g_RestConStatusDraw8;       // ���ݿ�״̬

extern PLUGIN_INFO g_TimesConStatusDraw1;       // ʱ�ο�״̬
extern PLUGIN_INFO g_TimesConStatusDraw2;       // ʱ�ο�״̬
extern PLUGIN_INFO g_TimesConStatusDraw3;       // ʱ�ο�״̬
extern PLUGIN_INFO g_TimesConStatusDraw4;       // ʱ�ο�״̬
extern PLUGIN_INFO g_TimesConStatusDraw5;       // ʱ�ο�״̬
extern PLUGIN_INFO g_TimesConStatusDraw6;       // ʱ�ο�״̬
extern PLUGIN_INFO g_TimesConStatusDraw7;       // ʱ�ο�״̬
extern PLUGIN_INFO g_TimesConStatusDraw8;       // ʱ�ο�״̬

extern PLUGIN_INFO g_MonthConStatusDraw1;       // �µ��״̬
extern PLUGIN_INFO g_MonthConStatusDraw2;       // �µ��״̬
extern PLUGIN_INFO g_MonthConStatusDraw3;       // �µ��״̬
extern PLUGIN_INFO g_MonthConStatusDraw4;       // �µ��״̬
extern PLUGIN_INFO g_MonthConStatusDraw5;       // �µ��״̬
extern PLUGIN_INFO g_MonthConStatusDraw6;       // �µ��״̬
extern PLUGIN_INFO g_MonthConStatusDraw7;       // �µ��״̬
extern PLUGIN_INFO g_MonthConStatusDraw8;       // �µ��״̬

extern PLUGIN_INFO g_BuyConStatusDraw1;       // �����״̬
extern PLUGIN_INFO g_BuyConStatusDraw2;       // �����״̬
extern PLUGIN_INFO g_BuyConStatusDraw3;       // �����״̬
extern PLUGIN_INFO g_BuyConStatusDraw4;       // �����״̬
extern PLUGIN_INFO g_BuyConStatusDraw5;       // �����״̬
extern PLUGIN_INFO g_BuyConStatusDraw6;       // �����״̬
extern PLUGIN_INFO g_BuyConStatusDraw7;       // �����״̬
extern PLUGIN_INFO g_BuyConStatusDraw8;       // �����״̬
extern PLUGIN_INFO g_BuyDatadisplay1;	// ������Ϣ1 0x11030040
extern PLUGIN_INFO g_BuyDatadisplay2;	
extern PLUGIN_INFO g_BuyDatadisplay3;	
extern PLUGIN_INFO g_BuyDatadisplay4;	
extern PLUGIN_INFO g_BuyDatadisplay5;	
extern PLUGIN_INFO g_BuyDatadisplay6;	
extern PLUGIN_INFO g_BuyDatadisplay7;	
extern PLUGIN_INFO g_BuyDatadisplay8;

extern PLUGIN_INFO g_TermStatusDraw1;       // �ն�״̬
extern PLUGIN_INFO g_TermStatusDraw2;       // �ն�״̬
extern PLUGIN_INFO g_TermStatusDraw3;       // �ն�״̬
extern PLUGIN_INFO g_TermStatusDraw4;       // �ն�״̬
extern PLUGIN_INFO g_TermStatusDraw5;       // �ն�״̬
extern PLUGIN_INFO g_TermStatusDraw6;       // �ն�״̬
extern PLUGIN_INFO g_TermStatusDraw7;       // �ն�״̬
extern PLUGIN_INFO g_TermStatusDraw8;       // �ն�״̬

extern PLUGIN_INFO g_ControlStatusDraw1;	// ����״̬	0x00010101
extern PLUGIN_INFO g_ControlStatusDraw2;	// ����״̬	0x00010201
extern PLUGIN_INFO g_ControlStatusDraw3;	// ����״̬	0x00010301
extern PLUGIN_INFO g_ControlStatusDraw4;	// ����״̬	0x00010401
extern PLUGIN_INFO g_ControlStatusDraw5;	// ����״̬	0x00010501
extern PLUGIN_INFO g_ControlStatusDraw6;	// ����״̬	0x00010601
extern PLUGIN_INFO g_ControlStatusDraw7;	// ����״̬	0x00010701
extern PLUGIN_INFO g_ControlStatusDraw8;	// ����״̬	0x00010801

extern PLUGIN_INFO g_TermTimeDateDraw;       // �ն�����ʱ��
extern PLUGIN_INFO g_RemConSwitchStatusDrawDraw;       // ң�ſ���״̬
extern PLUGIN_INFO g_TermVersionDraw;       // �ն˰汾
extern PLUGIN_INFO g_TermVersionDraw3;       // �ն˰汾
extern PLUGIN_INFO g_UpdateStatusDraw;       // ����״̬ 0x000e0000
#if TH208_RTU == 1
extern PLUGIN_INFO g_IUpdateStatusDraw;       // ����״̬ 0x000e0001
#endif
#if HARDWARE_TYPE == 4
extern PLUGIN_INFO g_UpdateRouterInfo;
#endif

#if MODULARIZATION_TMN_EN == 1
extern PLUGIN_INFO g_UpdateMdlInfo;
#endif

extern PLUGIN_INFO g_AutoCheckStatusDraw;       // �ն��Լ�״̬

extern PLUGIN_INFO g_ChannelDebugStatusDraw2;       // ͨ��������Ϣ
extern PLUGIN_INFO g_ChannelDebugStatusDraw3;       // ͨ��������Ϣ
extern PLUGIN_INFO g_ChannelDebugStatusDraw4;       // ͨ��������Ϣ
extern PLUGIN_INFO g_ChannelDebugStatusDraw5;       // ͨ��������Ϣ
extern PLUGIN_INFO g_ChannelDebugStatusDraw6;       // ͨ��������Ϣ
#if COMM_DEBUG_PRO == 1
extern PLUGIN_INFO g_ChannelDebugStatusDraw7;       // ͨ��������Ϣ
extern PLUGIN_INFO g_ChannelDebugStatusDraw8;       // ͨ��������Ϣ
#if ANHUI_FUN == 1
extern PLUGIN_INFO g_ChannelDebugStatusDraw9;       // ͨ��������Ϣ
#endif
#endif

#if MOD_CCOROUTER_DCW ==1
extern PLUGIN_INFO g_layer_HPLC_frequency;                //HPLCƵ��  
extern PLUGIN_INFO g_layer_meter_reading_port;            //����˿�  
extern PLUGIN_INFO g_layer_module_address;                //ģ���ַ  
extern PLUGIN_INFO g_layer_module_info;                   //ģ����Ϣ
extern PLUGIN_INFO g_layer_meter_reading_status;          //����״̬
extern PLUGIN_INFO g_layer_station_identification;        //̨��ʶ��    
extern PLUGIN_INFO g_layer_off_grid_awareness;            //������֪
extern PLUGIN_INFO g_layer_reject_node_report;            //�ܾ��ڵ��ϱ�
extern PLUGIN_INFO g_layer_file_info;                     //������Ϣ
extern PLUGIN_INFO g_layer_restart_app;                   //����Ӧ��
extern PLUGIN_INFO g_layer_hardware_initialization;       //Ӳ����ʼ��
extern PLUGIN_INFO g_layer_param_initialization;          //������ʼ��         
extern PLUGIN_INFO g_layer_app_version_info;              //Ӧ�ð汾��Ϣ
#endif

#if MOD_WIRELESSDCM_DCW ==1
extern PLUGIN_INFO g_layerDialingStatusData1;          //���߲���״̬
extern PLUGIN_INFO g_layerDialingStatusData2;
extern PLUGIN_INFO g_layerDialingEventData1;          //���߲����¼�
extern PLUGIN_INFO g_layerDialingEventData2;
extern PLUGIN_INFO g_layerDialingTypeData;             //����״̬��ʾ
extern PLUGIN_INFO g_layerDialingParamData1;            //���Ų�����ʾ
extern PLUGIN_INFO g_layerDialingParamData2;            
extern PLUGIN_INFO g_layerDialingTypSet;               //����״̬����
extern PLUGIN_INFO g_layerDialingPARASet1;             //���Ų�������
extern PLUGIN_INFO g_layerDialingPARASet2;
extern PLUGIN_INFO g_layerDialingRestartAPP;            //����Ӧ��
extern PLUGIN_INFO g_layerReDialing1;                   //���²���
extern PLUGIN_INFO g_layerReDialing2;                   
extern PLUGIN_INFO g_layerDialVersionInfo;              //�汾��Ϣ
#endif


extern PLUGIN_INFO g_CbStatusDraw;						// ��ǰ����״̬
extern PLUGIN_INFO g_CbMtStatusDraw1;					// �����㳭��״̬
extern PLUGIN_INFO g_CbMtStatusDraw2;					// �����㳭��״̬
extern PLUGIN_INFO g_CbMtStatusDraw3;					// �����㳭��ɹ���





/************************************************************************/
/* ����������                                                           */
/************************************************************************/
extern PLUGIN_INFO g_CurElecDraw10;         // 0x00500000 ��ǰ����
extern PLUGIN_INFO g_PreMonSupplyDraw1;     // 0x00510000 ���µ���
extern PLUGIN_INFO g_CurElecDraw11;	        // 0x00520000 �������޹�����
extern PLUGIN_INFO g_layerJc1Power;		//0x005AA000  �����й�
extern PLUGIN_INFO g_layerJc2Power;		//0x005AAA00 �����й�
extern PLUGIN_INFO g_CurSupplyDraw1;        // 0x00530000 ��ǰ����
extern PLUGIN_INFO g_MaxSupplyDraw1;	    // 0x00540000 �������
extern PLUGIN_INFO g_PreMonMaxSupplyDraw1;	// 0x00550000 �����������
extern PLUGIN_INFO g_layerVoltagEelectric1; // 0x00560000 ��ѹ����
extern PLUGIN_INFO g_layerPower1;	        // 0x00570000 ���ʼ���������
extern PLUGIN_INFO g_layerJCPower1;	        // 0x00580001 �����й�����
extern PLUGIN_INFO g_layerJCWuPower1;	    // 0x00580002 �����޹�����
extern PLUGIN_INFO g_layerJCPowerFactor1;	// 0x00590000 ��������
extern PLUGIN_INFO g_layerPhaseAngle1;      // 0x005A0000 ��λ������
extern PLUGIN_INFO g_CurElecQCur1;		    // 0x005E0000 ��ǰ����ʾ��
extern PLUGIN_INFO g_CurElecQCur2;		    // 0x005E0000 ��ǰ����ʾ��
extern PLUGIN_INFO g_SyElecQCur1;		    // 0x005D0000 ���µ���ʾ��
extern PLUGIN_INFO g_RdjElecQCur;			// 0x005F0000 �ն������ʾ��

extern PLUGIN_INFO g_VltHrPec_A;    // A���ѹг��������
extern PLUGIN_INFO g_VltHrPec_B;    // B���ѹг��������
extern PLUGIN_INFO g_VltHrPec_C;    // C���ѹг��������
extern PLUGIN_INFO g_CurHrPec_A;    // A�����г��������
extern PLUGIN_INFO g_CurHrPec_B;    // B�����г��������
extern PLUGIN_INFO g_CurHrPec_C;    // C�����г��������

extern PLUGIN_INFO g_VltHrVal_A;    // A���ѹг����Чֵ
extern PLUGIN_INFO g_VltHrVal_B;    // B���ѹг����Чֵ
extern PLUGIN_INFO g_VltHrVal_C;    // C���ѹг����Чֵ
extern PLUGIN_INFO g_CurHrVal_A;    // A�����г����Чֵ
extern PLUGIN_INFO g_CurHrVal_B;    // B�����г����Чֵ
extern PLUGIN_INFO g_CurHrVal_C;    // C�����г����Чֵ

extern PLUGIN_INFO g_layerActCurve1;		// �й��������� 0x00520106
extern PLUGIN_INFO g_layerActCurve2;		// �й��������� 0x00520206
extern PLUGIN_INFO g_layerActCurve3;		// �й��������� 0x00520306
extern PLUGIN_INFO g_layerActCurve4;		// �й��������� 0x00520406
extern PLUGIN_INFO g_layerActCurve5;		// �й��������� 0x00520506
extern PLUGIN_INFO g_layerActCurve6;		// �й��������� 0x00520606
extern PLUGIN_INFO g_layerActCurve7;		// �й��������� 0x00520706
extern PLUGIN_INFO g_layerActCurve8;		// �й��������� 0x00520806

extern PLUGIN_INFO g_layerReactCurve1;		// �޹��������� 0x00520107
extern PLUGIN_INFO g_layerReactCurve2;		// �޹��������� 0x00520207
extern PLUGIN_INFO g_layerReactCurve3;		// �޹��������� 0x00520307
extern PLUGIN_INFO g_layerReactCurve4;		// �޹��������� 0x00520407
extern PLUGIN_INFO g_layerReactCurve5;		// �޹��������� 0x00520507
extern PLUGIN_INFO g_layerReactCurve6;		// �޹��������� 0x00520607
extern PLUGIN_INFO g_layerReactCurve7;		// �޹��������� 0x00520707
extern PLUGIN_INFO g_layerReactCurve8;		// �޹��������� 0x00520807


extern PLUGIN_INFO g_layerEquipVal1;	// ���ܱ�ʾ�� 0x00520105
extern PLUGIN_INFO g_layerEquipVal2;	// ���ܱ�ʾ�� 0x00520205
extern PLUGIN_INFO g_layerEquipVal3;	// ���ܱ�ʾ�� 0x00520305
extern PLUGIN_INFO g_layerEquipVal4;	// ���ܱ�ʾ�� 0x00520405
extern PLUGIN_INFO g_layerEquipVal5;	// ���ܱ�ʾ�� 0x00520505
extern PLUGIN_INFO g_layerEquipVal6;	// ���ܱ�ʾ�� 0x00520605
extern PLUGIN_INFO g_layerEquipVal7;	// ���ܱ�ʾ�� 0x00520705
extern PLUGIN_INFO g_layerEquipVal8;	// ���ܱ�ʾ�� 0x00520805
extern PLUGIN_INFO g_layerEquipVal9;	// ���ܱ�ʾ�� 0x00520905
extern PLUGIN_INFO g_layerEquipVal10;	// ���ܱ�ʾ�� 0x00520A05
extern PLUGIN_INFO g_layerEquipVal11;	// ���ܱ�ʾ�� 0x00520B05
extern PLUGIN_INFO g_layerEquipVal12;	// ���ܱ�ʾ�� 0x00520C05
extern PLUGIN_INFO g_layerEquipVal13;	// ���ܱ�ʾ�� 0x00520D05
extern PLUGIN_INFO g_layerEquipVal14;	// ���ܱ�ʾ�� 0x00520E05
extern PLUGIN_INFO g_layerEquipVal15;	// ���ܱ�ʾ�� 0x00520F05
extern PLUGIN_INFO g_layerEquipVal16;	// ���ܱ�ʾ�� 0x00521005

extern PLUGIN_INFO g_layerEquipValExt1;	// ���ܱ�ʾ�� 0x00520108
extern PLUGIN_INFO g_layerEquipValExt2;	// ���ܱ�ʾ�� 0x00520208
extern PLUGIN_INFO g_layerEquipValExt3;	// ���ܱ�ʾ�� 0x00520308
extern PLUGIN_INFO g_layerEquipValExt4;	// ���ܱ�ʾ�� 0x00520408
extern PLUGIN_INFO g_layerEquipValExt5;	// ���ܱ�ʾ�� 0x00520508
extern PLUGIN_INFO g_layerEquipValExt6;	// ���ܱ�ʾ�� 0x00520608
extern PLUGIN_INFO g_layerEquipValExt7;	// ���ܱ�ʾ�� 0x00520708
extern PLUGIN_INFO g_layerEquipValExt8;	// ���ܱ�ʾ�� 0x00520808
extern PLUGIN_INFO g_layerEquipValExt9;	// ���ܱ�ʾ�� 0x00520908
extern PLUGIN_INFO g_layerEquipValExt10;	// ���ܱ�ʾ�� 0x00520A08
extern PLUGIN_INFO g_layerEquipValExt11;	// ���ܱ�ʾ�� 0x00520B08
extern PLUGIN_INFO g_layerEquipValExt12;	// ���ܱ�ʾ�� 0x00520C08
extern PLUGIN_INFO g_layerEquipValExt13;	// ���ܱ�ʾ�� 0x00520D08
extern PLUGIN_INFO g_layerEquipValExt14;	// ���ܱ�ʾ�� 0x00520E08
extern PLUGIN_INFO g_layerEquipValExt15;	// ���ܱ�ʾ�� 0x00520F08
extern PLUGIN_INFO g_layerEquipValExt16;	// ���ܱ�ʾ�� 0x00521008


extern PLUGIN_INFO g_layerGroupData1;		// �ܼ������� 0x00130100
extern PLUGIN_INFO g_layerGroupData2;		// �ܼ������� 0x00130200
extern PLUGIN_INFO g_layerGroupData3;		// �ܼ������� 0x00130300
extern PLUGIN_INFO g_layerGroupData4;		// �ܼ������� 0x00130400
extern PLUGIN_INFO g_layerGroupData5;		// �ܼ������� 0x00130500
extern PLUGIN_INFO g_layerGroupData6;		// �ܼ������� 0x00130600
extern PLUGIN_INFO g_layerGroupData7;		// �ܼ������� 0x00130700
extern PLUGIN_INFO g_layerGroupData8;		// �ܼ������� 0x00130800

//////////////////////////////////////�¼�
extern PLUGIN_INFO g_layerGroupDataNow1;		// �ܼ������� 0x00140100
extern PLUGIN_INFO g_layerGroupDataNow2;		// �ܼ������� 0x00140200
extern PLUGIN_INFO g_layerGroupDataNow3;		// �ܼ������� 0x00140300
extern PLUGIN_INFO g_layerGroupDataNow4;		// �ܼ������� 0x00140400
extern PLUGIN_INFO g_layerGroupDataNow5;		// �ܼ������� 0x00140500
extern PLUGIN_INFO g_layerGroupDataNow6;		// �ܼ������� 0x00140600
extern PLUGIN_INFO g_layerGroupDataNow7;		// �ܼ������� 0x00140700
extern PLUGIN_INFO g_layerGroupDataNow8;		// �ܼ������� 0x00140800

#if 1//����ר��
extern PLUGIN_INFO g_layerGroupDataPow1;		// �ܼ���1��ǰ���� 0x00141100
extern PLUGIN_INFO g_layerGroupDataPow2;		// �ܼ���2��ǰ���� 0x00141200
extern PLUGIN_INFO g_layerGroupDataPow3;		// �ܼ���3��ǰ���� 0x00141300
extern PLUGIN_INFO g_layerGroupDataPow4;		// �ܼ���4��ǰ���� 0x00141400

extern PLUGIN_INFO g_CurElecVir1;		// �ܼ��鵱�յ��� 0x00021100
extern PLUGIN_INFO g_CurElecVir2;		// �ܼ��鵱�յ��� 0x00021200
extern PLUGIN_INFO g_CurElecVir3;		// �ܼ��鵱�յ��� 0x00021300
extern PLUGIN_INFO g_CurElecVir4;		// �ܼ��鵱�յ��� 0x00021400

extern PLUGIN_INFO g_CurElecVirMon1;		// �ܼ��鵱�µ��� 0x00021101
extern PLUGIN_INFO g_CurElecVirMon2;		// �ܼ��鵱�µ��� 0x00021201
extern PLUGIN_INFO g_CurElecVirMon3;		// �ܼ��鵱�µ��� 0x00021301
extern PLUGIN_INFO g_CurElecVirMon4;		// �ܼ��鵱�µ��� 0x00021401
#endif

/************************************************************************/
/* ���������ݽ���                                                       */
/************************************************************************/


/************************************************************************/
/* �ն�����                                                             */
/************************************************************************/
extern PLUGIN_INFO g_layerTerminalAddrData;       // �ն˵�ַ 0x00100000
extern PLUGIN_INFO g_layerTerminalInfo;           // �ն���Ϣ 0x00100002
#if SHANDONG_FUN == 1 && HARDWARE_TYPE == 3
extern PLUGIN_INFO g_BatValInfo;				  //��ص�ѹ��Ϣ0x00100003
#endif
#if ANHUI_FUN == 1 
extern PLUGIN_INFO g_POSITIONINFO;                //�豸����λ����Ϣ0x000c0005
#endif
extern PLUGIN_INFO g_ESAMINFO;                    // ESAM��Ϣ 0x000c0001
#if SHANGHAI_FUN == 1
extern PLUGIN_INFO g_layerTerminalInfo1;            // �ն���Ϣ 0x00100010
extern PLUGIN_INFO g_layerTerminalInfo2;            // �ն���Ϣ 0x00100020
extern PLUGIN_INFO g_layerTerminalInfo3;            // �ն���Ϣ 0x00100030
extern PLUGIN_INFO g_layerTerminalInfo4;            // �ն���Ϣ 0x00100040
extern PLUGIN_INFO g_layerTerminalInfo5;            // �ն���Ϣ 0x00100050
extern PLUGIN_INFO g_layerTerminalInfo6;            // �ն���Ϣ 0x00100060
extern PLUGIN_INFO g_layerCommdisp;
extern PLUGIN_INFO g_layerLossVoltRec;              //ʧѹͳ�� 0x00830001
extern PLUGIN_INFO g_layerLossVoltNRec;             //���N��ʧѹ��¼ 0x00830002
extern PLUGIN_INFO g_layerLossVoltRec1;	            //������ʧѹͳ��ҳ��1 0x00830003
extern PLUGIN_INFO g_layerLossVoltRec2; 	        //������ʧѹͳ��ҳ��2 0x00830004
extern PLUGIN_INFO g_layerLossVoltRec3;	            //������ʧѹͳ��ҳ��3 0x00830005		
extern PLUGIN_INFO g_layerLossVoltNRec1;	        //���������ʧѹ��¼1 0x00830006       
extern PLUGIN_INFO g_layerLossVoltNRec2;	        //���������ʧѹ��¼2 0x00830007
extern PLUGIN_INFO g_layerLossVoltNRec3;	        //���������ʧѹ��¼3 0x00830008
#endif

extern PLUGIN_INFO g_layerChnlInfoData;			    // �ն˵�ǰ����ͨ�� 0x00110000
extern PLUGIN_INFO g_layerGroupMainData;          // �ܼ���������Ϣ 0x00120000
extern PLUGIN_INFO g_layerTermRelay;		// �ն��м̲��� 0x00130000
extern PLUGIN_INFO g_layerTermGroupAddr;	// �ն����ַ���� 0x00140000
extern PLUGIN_INFO g_layerSysReset;			// ϵͳ��λ 0x00610001
extern PLUGIN_INFO g_layerAbout;			// ���ڶԻ��� 0x00150000
extern PLUGIN_INFO g_layerEnergyVir;		// ����ۼ� 0x00160000
extern PLUGIN_INFO g_layerPowerVir;			// �����ۼ� 0x00160001
extern PLUGIN_INFO g_layerInduct;			// ������Ƶ�� 0x00170000
#if COMMPARA_BAK_EN == 1
extern PLUGIN_INFO g_layerNetVirInfo;           // ����ר����Ϣ 0x21B70000
#endif

#if PRTL_STACK_EXCHANGE_EN == 1
extern PLUGIN_INFO g_prtl_stack_exchange;
#endif
extern PLUGIN_INFO g_layerSetStartMechineModelTime;	//�޸�ͨ��ģ�鿪��ʱ�� 0x21B70077
extern PLUGIN_INFO g_layersetSafeONOFF;//��ȫģʽ���ùر�0x000c0003
extern PLUGIN_INFO g_layersetFTP_TelnetONOFF;//FTP Telnet ���ܿ����ر�0x000c0004
#if SECURITY_DEFEND == 1
extern PLUGIN_INFO g_layerDefend;//FTP Telnet ���ܿ����ر�0x000c0004
extern PLUGIN_INFO g_layerAuthTime;
#endif
/************************************************************************/
/* �ն����ݽ���                                                         */
/************************************************************************/


/************************************************************************/
/* ���Ʋ���                                                             */
/************************************************************************/
// ʱ�οز���

extern PLUGIN_INFO g_layer_ctrl_period_display1;	   // ʱ�οز��� 0x00200108
extern PLUGIN_INFO g_layer_ctrl_period_display2;	   // ʱ�οز��� 0x00200208
extern PLUGIN_INFO g_layer_ctrl_period_display3;	   // ʱ�οز��� 0x00200308
extern PLUGIN_INFO g_layer_ctrl_period_display4;	   // ʱ�οز��� 0x00200408
extern PLUGIN_INFO g_layer_ctrl_period_display5;	   // ʱ�οز��� 0x00200508
extern PLUGIN_INFO g_layer_ctrl_period_display6;	   // ʱ�οز��� 0x00200608
extern PLUGIN_INFO g_layer_ctrl_period_display7;	   // ʱ�οز��� 0x00200708
extern PLUGIN_INFO g_layer_ctrl_period_display8;	   // ʱ�οز��� 0x00200808

// ������1
extern PLUGIN_INFO g_layerTimeCtl11;       // ������1	0x00200100
extern PLUGIN_INFO g_layerTimeCtl12;	   // ������1	0x00200101
extern PLUGIN_INFO g_layerTimeCtl13;       // ������1	0x00200102
extern PLUGIN_INFO g_layerTimeCtl14;	   // ������1	0x00200103

// ������2
extern PLUGIN_INFO g_layerTimeCtl21;       // ������2	0x00200200
extern PLUGIN_INFO g_layerTimeCtl22;	   // ������2	0x00200201
extern PLUGIN_INFO g_layerTimeCtl23;       // ������2	0x00200202
extern PLUGIN_INFO g_layerTimeCtl24;	   // ������2	0x00200203

// ������3
extern PLUGIN_INFO g_layerTimeCtl31;       // ������3	0x00200300
extern PLUGIN_INFO g_layerTimeCtl32;	   // ������3	0x00200301
extern PLUGIN_INFO g_layerTimeCtl33;       // ������3	0x00200302
extern PLUGIN_INFO g_layerTimeCtl34;	   // ������3	0x00200303

// ������4
extern PLUGIN_INFO g_layerTimeCtl41;       // ������4	0x00200400
extern PLUGIN_INFO g_layerTimeCtl42;	   // ������4	0x00200401
extern PLUGIN_INFO g_layerTimeCtl43;       // ������4	0x00200402
extern PLUGIN_INFO g_layerTimeCtl44;	   // ������4	0x00200403

// ������5
extern PLUGIN_INFO g_layerTimeCtl51;       // ������5	0x00200500
extern PLUGIN_INFO g_layerTimeCtl52;	   // ������5	0x00200501
extern PLUGIN_INFO g_layerTimeCtl53;       // ������5	0x00200502
extern PLUGIN_INFO g_layerTimeCtl54;	   // ������5	0x00200503

// ������6
extern PLUGIN_INFO g_layerTimeCtl61;       // ������6	0x00200600
extern PLUGIN_INFO g_layerTimeCtl62;	   // ������6	0x00200601
extern PLUGIN_INFO g_layerTimeCtl63;       // ������6	0x00200602
extern PLUGIN_INFO g_layerTimeCtl64;	   // ������6	0x00200603

// ������7
extern PLUGIN_INFO g_layerTimeCtl71;       // ������7	0x00200700
extern PLUGIN_INFO g_layerTimeCtl72;	   // ������7	0x00200701
extern PLUGIN_INFO g_layerTimeCtl73;       // ������7	0x00200702
extern PLUGIN_INFO g_layerTimeCtl74;	   // ������7	0x00200703

// ������8
extern PLUGIN_INFO g_layerTimeCtl81;       // ������8	0x00200800
extern PLUGIN_INFO g_layerTimeCtl82;	   // ������8	0x00200801
extern PLUGIN_INFO g_layerTimeCtl83;       // ������8	0x00200802
extern PLUGIN_INFO g_layerTimeCtl84;	   // ������8	0x00200803

// �¸��ز���
extern PLUGIN_INFO g_layer_ctrl_down_display1;	// �¸��ز���	0x00210100
extern PLUGIN_INFO g_layer_ctrl_down_display2;	// �¸��ز���	0x00210200
extern PLUGIN_INFO g_layer_ctrl_down_display3;	// �¸��ز���	0x00210300
extern PLUGIN_INFO g_layer_ctrl_down_display4;	// �¸��ز���	0x00210400
extern PLUGIN_INFO g_layer_ctrl_down_display5;	// �¸��ز���	0x00210500
extern PLUGIN_INFO g_layer_ctrl_down_display6;	// �¸��ز���	0x00210600
extern PLUGIN_INFO g_layer_ctrl_down_display7;	// �¸��ز���	0x00210700
extern PLUGIN_INFO g_layer_ctrl_down_display8;	// �¸��ز���	0x00210800
// ���ݿز���
extern PLUGIN_INFO g_layer_ctrl_rest_display1;	// ���ݿز��� 0x00220100
extern PLUGIN_INFO g_layer_ctrl_rest_display2;	// ���ݿز��� 0x00220200
extern PLUGIN_INFO g_layer_ctrl_rest_display3;	// ���ݿز��� 0x00220300
extern PLUGIN_INFO g_layer_ctrl_rest_display4;	// ���ݿز��� 0x00220400
extern PLUGIN_INFO g_layer_ctrl_rest_display5;	// ���ݿز��� 0x00220500
extern PLUGIN_INFO g_layer_ctrl_rest_display6;	// ���ݿز��� 0x00220600
extern PLUGIN_INFO g_layer_ctrl_rest_display7;	// ���ݿز��� 0x00220700
extern PLUGIN_INFO g_layer_ctrl_rest_display8;	// ���ݿز��� 0x00220800

// ���ݿز���
extern PLUGIN_INFO g_layer_ctrl_month_display1;// �µ�ز��� 0x00240100
extern PLUGIN_INFO g_layer_ctrl_month_display2;// �µ�ز��� 0x00240200
extern PLUGIN_INFO g_layer_ctrl_month_display3;// �µ�ز��� 0x00240300
extern PLUGIN_INFO g_layer_ctrl_month_display4;// �µ�ز��� 0x00240400
extern PLUGIN_INFO g_layer_ctrl_month_display5;// �µ�ز��� 0x00240100
extern PLUGIN_INFO g_layer_ctrl_month_display6;// �µ�ز��� 0x00240200
extern PLUGIN_INFO g_layer_ctrl_month_display7;// �µ�ز��� 0x00240300
extern PLUGIN_INFO g_layer_ctrl_month_display8;// �µ�ز��� 0x00240400

extern PLUGIN_INFO g_layer_ctrl_stop_display1;	// ��ͣ�ز��� 0x00290100
extern PLUGIN_INFO g_layer_ctrl_stop_display2;	// ��ͣ�ز��� 0x00290200
extern PLUGIN_INFO g_layer_ctrl_stop_display3;	// ��ͣ�ز��� 0x00290300
extern PLUGIN_INFO g_layer_ctrl_stop_display4;	// ��ͣ�ز��� 0x00290400
extern PLUGIN_INFO g_layer_ctrl_stop_display5;	// ��ͣ�ز��� 0x00290100
extern PLUGIN_INFO g_layer_ctrl_stop_display6;	// ��ͣ�ز��� 0x00290200
extern PLUGIN_INFO g_layer_ctrl_stop_display7;	// ��ͣ�ز��� 0x00290300
extern PLUGIN_INFO g_layer_ctrl_stop_display8;	// ��ͣ�ز��� 0x00290400


extern PLUGIN_INFO g_layerExesRate1;	// ��������ʱ�� 0x00270000
extern PLUGIN_INFO g_layerExesRate2;	// ��������ʱ�� 0x00270001
extern PLUGIN_INFO g_layerExesRate3;	// ��������ʱ�� 0x00270002
extern PLUGIN_INFO g_layerExesRate4;	// ��������ʱ�� 0x00270003
extern PLUGIN_INFO g_layerExesRate5;	// ��������ʱ�� 0x00270004
extern PLUGIN_INFO g_layerExesRate6;	// ��������ʱ�� 0x00270005

extern PLUGIN_INFO g_layerBuyCoulomb1;	// ����ز��� 0x00250100
extern PLUGIN_INFO g_layerBuyCoulomb2;	// ����ز��� 0x00250200
extern PLUGIN_INFO g_layerBuyCoulomb3;	// ����ز��� 0x00250300
extern PLUGIN_INFO g_layerBuyCoulomb4;	// ����ز��� 0x00250400

extern PLUGIN_INFO g_layerCallingExes;	// �߷ѿز��� 0x00260000

/************************************************************************/
/* ���Ʋ�������                                                         */
/************************************************************************/

/************************************************************************/
/* ���������                                                           */
/************************************************************************/
extern PLUGIN_INFO g_layerMeasureMainPara;	// ��������� 0x00440000

extern PLUGIN_INFO g_layerVarietyRate1; // �������Ȳ��� 0x00400100
extern PLUGIN_INFO g_layerVarietyRate2; // �������Ȳ��� 0x00400200
extern PLUGIN_INFO g_layerVarietyRate3; // �������Ȳ��� 0x00400300
extern PLUGIN_INFO g_layerVarietyRate4; // �������Ȳ��� 0x00400400
extern PLUGIN_INFO g_layerVarietyRate5; // �������Ȳ��� 0x00400500
extern PLUGIN_INFO g_layerVarietyRate6; // �������Ȳ��� 0x00400600
extern PLUGIN_INFO g_layerVarietyRate7; // �������Ȳ��� 0x00400700
extern PLUGIN_INFO g_layerVarietyRate8; // �������Ȳ��� 0x00400800
extern PLUGIN_INFO g_layerVarietyRate9; // �������Ȳ��� 0x00400900
extern PLUGIN_INFO g_layerVarietyRate10;// �������Ȳ��� 0x00400A00
extern PLUGIN_INFO g_layerVarietyRate11;// �������Ȳ��� 0x00400B00
extern PLUGIN_INFO g_layerVarietyRate12;// �������Ȳ��� 0x00400C00
extern PLUGIN_INFO g_layerVarietyRate13;// �������Ȳ��� 0x00400D00
extern PLUGIN_INFO g_layerVarietyRate14;// �������Ȳ��� 0x00400E00
extern PLUGIN_INFO g_layerVarietyRate15;// �������Ȳ��� 0x00400F00
extern PLUGIN_INFO g_layerVarietyRate16;// �������Ȳ��� 0x00401000
extern PLUGIN_INFO g_layerVartyRate1;// �������Ȳ���(��ʾ������1~8��KiKvKp)  0x00400000
extern PLUGIN_INFO g_layerVartyRate2;// �������Ȳ���(��ʾ������9~16��KiKvKp) 0x00400001
extern PLUGIN_INFO g_layer_kivp_display;  //  3kֵ


extern PLUGIN_INFO g_layerEligible1;	// �ϸ��ѹ������ 0x00420100
extern PLUGIN_INFO g_layerEligible2;	// �ϸ��ѹ������ 0x00420200
extern PLUGIN_INFO g_layerEligible3;	// �ϸ��ѹ������ 0x00420300
extern PLUGIN_INFO g_layerEligible4;	// �ϸ��ѹ������ 0x00420400
extern PLUGIN_INFO g_layerEligible5;	// �ϸ��ѹ������ 0x00420500
extern PLUGIN_INFO g_layerEligible6;	// �ϸ��ѹ������ 0x00420600
extern PLUGIN_INFO g_layerEligible7;	// �ϸ��ѹ������ 0x00420700
extern PLUGIN_INFO g_layerEligible8;	// �ϸ��ѹ������ 0x00420800
extern PLUGIN_INFO g_layerEligible9;	// �ϸ��ѹ������ 0x00420900
extern PLUGIN_INFO g_layerEligible10;	// �ϸ��ѹ������ 0x00420A00
extern PLUGIN_INFO g_layerEligible11;	// �ϸ��ѹ������ 0x00420B00
extern PLUGIN_INFO g_layerEligible12;	// �ϸ��ѹ������ 0x00420C00
extern PLUGIN_INFO g_layerEligible13;	// �ϸ��ѹ������ 0x00420D00
extern PLUGIN_INFO g_layerEligible14;	// �ϸ��ѹ������ 0x00420E00
extern PLUGIN_INFO g_layerEligible15;	// �ϸ��ѹ������ 0x00420F00
extern PLUGIN_INFO g_layerEligible16;	// �ϸ��ѹ������ 0x00421000
/************************************************************************/
/* �������������                                                       */
/************************************************************************/

/****************************XTL***********************************/
extern PLUGIN_INFO g_layer_config_para;   // ���ò���  
/****************************XTL***********************************/
/************************************************************************/
/* ����������		                                                    */
/************************************************************************/
extern PLUGIN_INFO g_layerMeasureMainData;	//����������0x00440008
extern PLUGIN_INFO g_layerMeasureMainDataExt;	//����������0x00440009
/************************************************************************/
/* ���������ݽ���                                                       */
/************************************************************************/


/************************************************************************/
/* ��������                                                             */
/************************************************************************/
// �ն�����
extern PLUGIN_INFO g_layerProtoInfo;      // ͨѶ��Լ���� 0x00660009
extern PLUGIN_INFO g_layerChnlInfo;       // ͨѶͨ������ 0x00660000
extern PLUGIN_INFO g_layerGprsInfo;     //   GPRS����    0x00660001
//extern PLUGIN_INFO g_layerNoteInfo;     //   ���� 0x00660002
#if GPRS_ETH_ALL_OLINE == 1
extern PLUGIN_INFO g_layerEthCommPara;    // �ն���̫��ͨ��ͨ������ 0x00666001
#endif

extern PLUGIN_INFO g_layerMeasureMain;	  // ���������� 0x00640000
extern PLUGIN_INFO g_layerGroupMain;	  // �ܼ������� 0x00670000
extern PLUGIN_INFO g_layerPulseMain;	  // ������������� 0x00650000

#if TIME_CTRL_ON_LINE_EXT == 1
extern PLUGIN_INFO g_TimeCtlOnLine;
#endif 

// ����ҳ���
extern PLUGIN_INFO g_layerValidatePwd;  // ������֤ 0x00600000
extern PLUGIN_INFO g_layerChangePwd;      // ������� 0x00600001
extern PLUGIN_INFO g_layerFreezeTime;	// ��֤���� 0x00600002
extern PLUGIN_INFO g_layerResetTime;      // �ն�ʱ������ 0x00620000
extern PLUGIN_INFO g_layerTerminalAddr;   // �ն˵�ַ  0x00100000
#if CHONG_QING_FUN == 1
extern PLUGIN_INFO g_layerSetUsernamePwd;   // �ն˵�ַ  0x00100000
#endif

extern PLUGIN_INFO g_layerResetTerm;      // ��λ�ն����� 0x00610000
extern PLUGIN_INFO g_layerStReinstall;      // ������°�װ 0x00680000
extern PLUGIN_INFO g_layerLogDump;			// ϵͳ��־ת�� 0x00680002
extern PLUGIN_INFO g_layerTrunDisp;		 // ����ҳ������ 0x00690000


extern PLUGIN_INFO g_layerMeasureAddr;	 // ��Ƶ�ַ���� 0x00690000
extern PLUGIN_INFO g_layerMeasureTime;	 // ���Уʱ���� 0x00690000
extern PLUGIN_INFO g_layerRemoteCtlMain; // ң�ſ������� 0x006C0000
extern PLUGIN_INFO g_layerRemoteCtlMain2; // ң�ſ������� 0x006C0001
extern PLUGIN_INFO g_layerCfgMonthCtl;	 // �µ������ 0x006D0000
extern PLUGIN_INFO g_layerCfgTimeCtl;	 // ʱ�ο����� 0x006E0000
extern PLUGIN_INFO g_layerIPConfig_eth0;	 // IPConfig 0x006F0000
extern PLUGIN_INFO g_layerIPConfig_eth1;	 // IPConfig 0x006F0010
extern PLUGIN_INFO g_layerDefaultGW;	 // ����Ĭ������ 0x006F0010
extern PLUGIN_INFO g_layerInductConfig1;// A�໥����Ƶ�� 0x00B10000
extern PLUGIN_INFO g_layerInductConfig2;// B�໥����Ƶ�� 0x00B10001
extern PLUGIN_INFO g_layerInductConfig3;// C�໥����Ƶ�� 0x00B10002
extern PLUGIN_INFO g_layerTermRetset;	// �����ն��豸 0x00B30000
extern PLUGIN_INFO g_layerClearPara;	// ������ʼ�� 0x00B40000
extern PLUGIN_INFO g_layerClearACMeterData; // ���ɵ������   0x00B42000
extern PLUGIN_INFO g_layerInitACMeterPara;  // ����ȫ���ʼ�� 0x00B43000
extern PLUGIN_INFO g_layerClearLog;		// �����־ 0x00B50000
extern PLUGIN_INFO g_layerBackup;		// ���ݽ������� 0x00B60000
#if NEW_GW_GUI_APPEND == 1
extern PLUGIN_INFO g_layerExtendBackup;	// �¹������� 0x00B50100
#endif
#if PLC_AMR_EN == 1
extern PLUGIN_INFO g_layerPlcVenderCodeAndVersionInfo; // �ز�ģ�鳧�̴��뼰�汾��Ϣ 0x00B5000B
#endif

#if MODULARIZATION_TMN_EN == 1
extern PLUGIN_INFO g_layerMdlVenderCodeAndVersionInfo;
#endif

extern PLUGIN_INFO g_layerRadioCfg;		// ��̨�������� 0x20B70000
extern PLUGIN_INFO g_layerRadioLongSendCfg;		// ��̨����
extern PLUGIN_INFO g_MeterReadFailStat;
extern PLUGIN_INFO g_layer_normal_plan_set;
extern PLUGIN_INFO g_layerXjyAcCfg;		//Ѳ���ǽ��ɵ�Դ����
extern PLUGIN_INFO g_layerProductVoltageCfg;
#ifdef AREA_JIANGSU
extern PLUGIN_INFO g_layer_loop_state_display;
extern PLUGIN_INFO g_layer_inspector_ver_display;
extern PLUGIN_INFO g_layer_GPRS_param_clean;
#endif

/************************************************************************/
/* �������ý���                                                         */
/************************************************************************/

// ����ҳ���
extern PLUGIN_INFO g_layerCustomDebug;  // �Զ��������Ϣ   0x000d0000

/************************************************************************/
/* �������ò���                                                         */
/************************************************************************/
extern PLUGIN_INFO g_layerTaskPara11;	// ��������1-1	0x003000100
extern PLUGIN_INFO g_layerTaskPara12;	// ��������1-2	0x003000200
extern PLUGIN_INFO g_layerTaskPara13;	// ��������1-3	0x003000300
extern PLUGIN_INFO g_layerTaskPara14;	// ��������1-4	0x003000400
extern PLUGIN_INFO g_layerTaskPara15;	// ��������1-5	0x003000500
extern PLUGIN_INFO g_layerTaskPara16;	// ��������1-6	0x003000600
extern PLUGIN_INFO g_layerTaskPara17;	// ��������1-7	0x003000700
extern PLUGIN_INFO g_layerTaskPara18;	// ��������1-8	0x003000800

extern PLUGIN_INFO g_layerTaskPara19;	// ��������1-9	0x003000900
extern PLUGIN_INFO g_layerTaskPara1A;	// ��������1-10	0x003000A00
extern PLUGIN_INFO g_layerTaskPara1B;	// ��������1-11	0x003000B00
extern PLUGIN_INFO g_layerTaskPara1C;	// ��������1-12	0x003000C00
extern PLUGIN_INFO g_layerTaskPara1D;	// ��������1-13	0x003000D00
extern PLUGIN_INFO g_layerTaskPara1E;	// ��������1-14	0x003000E00
extern PLUGIN_INFO g_layerTaskPara1F;	// ��������1-15	0x003000F00
extern PLUGIN_INFO g_layerTaskPara110;	// ��������1-16	0x003001000

extern PLUGIN_INFO g_layerTaskPara111;	// ��������1-17	0x003001100
extern PLUGIN_INFO g_layerTaskPara112;	// ��������1-18	0x003002200
extern PLUGIN_INFO g_layerTaskPara113;	// ��������1-19	0x003003300
extern PLUGIN_INFO g_layerTaskPara114;	// ��������1-20	0x003004400
extern PLUGIN_INFO g_layerTaskPara115;	// ��������1-21	0x003005500
extern PLUGIN_INFO g_layerTaskPara116;	// ��������1-22	0x003006600
extern PLUGIN_INFO g_layerTaskPara117;	// ��������1-23	0x003007700
extern PLUGIN_INFO g_layerTaskPara118;	// ��������1-24	0x003008800

extern PLUGIN_INFO g_layerTaskPara119;	// ��������1-25	0x003001900
extern PLUGIN_INFO g_layerTaskPara11A;	// ��������1-26	0x003001A00
extern PLUGIN_INFO g_layerTaskPara11B;	// ��������1-27	0x003001B00
extern PLUGIN_INFO g_layerTaskPara11C;	// ��������1-28	0x003001C00
extern PLUGIN_INFO g_layerTaskPara11D;	// ��������1-29	0x003001D00
extern PLUGIN_INFO g_layerTaskPara11E;	// ��������1-30	0x003001E00
extern PLUGIN_INFO g_layerTaskPara11F;	// ��������1-31	0x003001F00
extern PLUGIN_INFO g_layerTaskPara120;	// ��������1-32	0x003002000

extern PLUGIN_INFO g_layerTaskPara121;	// ��������1-33	0x003002100
extern PLUGIN_INFO g_layerTaskPara122;	// ��������1-34	0x003002200
extern PLUGIN_INFO g_layerTaskPara123;	// ��������1-35	0x003002300
extern PLUGIN_INFO g_layerTaskPara124;	// ��������1-36	0x003002400
extern PLUGIN_INFO g_layerTaskPara125;	// ��������1-37	0x003002500
extern PLUGIN_INFO g_layerTaskPara126;	// ��������1-38	0x003002600
extern PLUGIN_INFO g_layerTaskPara127;	// ��������1-39	0x003002700
extern PLUGIN_INFO g_layerTaskPara128;	// ��������1-40	0x003002800

extern PLUGIN_INFO g_layerTaskPara129;	// ��������1-41	0x003002900
extern PLUGIN_INFO g_layerTaskPara12A;	// ��������1-42	0x003002A00
extern PLUGIN_INFO g_layerTaskPara12B;	// ��������1-43	0x003002B00
extern PLUGIN_INFO g_layerTaskPara12C;	// ��������1-44	0x003002C00
extern PLUGIN_INFO g_layerTaskPara12D;	// ��������1-45	0x003002D00
extern PLUGIN_INFO g_layerTaskPara12E;	// ��������1-46	0x003002E00
extern PLUGIN_INFO g_layerTaskPara12F;	// ��������1-47	0x003002F00
extern PLUGIN_INFO g_layerTaskPara130;	// ��������1-48	0x003003000

extern PLUGIN_INFO g_layerTaskPara131;	// ��������1-49	0x003003100
extern PLUGIN_INFO g_layerTaskPara132;	// ��������1-50	0x003003200
extern PLUGIN_INFO g_layerTaskPara133;	// ��������1-51	0x003003300
extern PLUGIN_INFO g_layerTaskPara134;	// ��������1-52	0x003003400
extern PLUGIN_INFO g_layerTaskPara135;	// ��������1-53	0x003003500
extern PLUGIN_INFO g_layerTaskPara136;	// ��������1-54	0x003003600
extern PLUGIN_INFO g_layerTaskPara137;	// ��������1-55	0x003003700
extern PLUGIN_INFO g_layerTaskPara138;	// ��������1-56	0x003003800

extern PLUGIN_INFO g_layerTaskPara139;	// ��������1-57	0x003003900
extern PLUGIN_INFO g_layerTaskPara13A;	// ��������1-58	0x003003A00
extern PLUGIN_INFO g_layerTaskPara13B;	// ��������1-59	0x003003B00
extern PLUGIN_INFO g_layerTaskPara13C;	// ��������1-60	0x003003C00
extern PLUGIN_INFO g_layerTaskPara13D;	// ��������1-61	0x003003D00
extern PLUGIN_INFO g_layerTaskPara13E;	// ��������1-62	0x003003E00
extern PLUGIN_INFO g_layerTaskPara13F;	// ��������1-63	0x003003F00
extern PLUGIN_INFO g_layerTaskPara140;	// ��������1-64	0x003004000
///----------------------------------------------------------------


extern PLUGIN_INFO g_layerTaskPara21;	// ��������2-1	0x003000101
extern PLUGIN_INFO g_layerTaskPara22;	// ��������2-2	0x003000201
extern PLUGIN_INFO g_layerTaskPara23;	// ��������2-3	0x003000301
extern PLUGIN_INFO g_layerTaskPara24;	// ��������2-4	0x003000401
extern PLUGIN_INFO g_layerTaskPara25;	// ��������2-5	0x003000501
extern PLUGIN_INFO g_layerTaskPara26;	// ��������2-6	0x003000601
extern PLUGIN_INFO g_layerTaskPara27;	// ��������2-7	0x003000701
extern PLUGIN_INFO g_layerTaskPara28;	// ��������2-8	0x003000801

extern PLUGIN_INFO g_layerTaskPara29;	// ��������2-9	0x003000901
extern PLUGIN_INFO g_layerTaskPara2A;	// ��������2-10	0x003000A01
extern PLUGIN_INFO g_layerTaskPara2B;	// ��������2-11	0x003000B01
extern PLUGIN_INFO g_layerTaskPara2C;	// ��������2-12	0x003000C01
extern PLUGIN_INFO g_layerTaskPara2D;	// ��������2-13	0x003000D01
extern PLUGIN_INFO g_layerTaskPara2E;	// ��������2-14	0x003000E01
extern PLUGIN_INFO g_layerTaskPara2F;	// ��������2-15	0x003000F01
extern PLUGIN_INFO g_layerTaskPara210;	// ��������2-16	0x003001001

extern PLUGIN_INFO g_layerTaskPara211;	// ��������2-17	0x003001101
extern PLUGIN_INFO g_layerTaskPara212;	// ��������2-18	0x003002201
extern PLUGIN_INFO g_layerTaskPara213;	// ��������2-19	0x003003301
extern PLUGIN_INFO g_layerTaskPara214;	// ��������2-20	0x003004401
extern PLUGIN_INFO g_layerTaskPara215;	// ��������2-21	0x003005501
extern PLUGIN_INFO g_layerTaskPara216;	// ��������2-22	0x003006601
extern PLUGIN_INFO g_layerTaskPara217;	// ��������2-23	0x003007701
extern PLUGIN_INFO g_layerTaskPara218;	// ��������2-24	0x003008801

extern PLUGIN_INFO g_layerTaskPara219;	// ��������2-25	0x003001901
extern PLUGIN_INFO g_layerTaskPara21A;	// ��������2-26	0x003001A01
extern PLUGIN_INFO g_layerTaskPara21B;	// ��������2-27	0x003001B01
extern PLUGIN_INFO g_layerTaskPara21C;	// ��������2-28	0x003001C01
extern PLUGIN_INFO g_layerTaskPara21D;	// ��������2-29	0x003001D01
extern PLUGIN_INFO g_layerTaskPara21E;	// ��������2-30	0x003001E01
extern PLUGIN_INFO g_layerTaskPara21F;	// ��������2-31	0x003001F01
extern PLUGIN_INFO g_layerTaskPara220;	// ��������2-32	0x003002001

extern PLUGIN_INFO g_layerTaskPara221;	// ��������2-33	0x003002101
extern PLUGIN_INFO g_layerTaskPara222;	// ��������2-34	0x003002201
extern PLUGIN_INFO g_layerTaskPara223;	// ��������2-35	0x003002301
extern PLUGIN_INFO g_layerTaskPara224;	// ��������2-36	0x003002401
extern PLUGIN_INFO g_layerTaskPara225;	// ��������2-37	0x003002501
extern PLUGIN_INFO g_layerTaskPara226;	// ��������2-38	0x003002601
extern PLUGIN_INFO g_layerTaskPara227;	// ��������2-39	0x003002701
extern PLUGIN_INFO g_layerTaskPara228;	// ��������2-40	0x003002801

extern PLUGIN_INFO g_layerTaskPara229;	// ��������2-41	0x003002901
extern PLUGIN_INFO g_layerTaskPara22A;	// ��������2-42	0x003002A01
extern PLUGIN_INFO g_layerTaskPara22B;	// ��������2-43	0x003002B01
extern PLUGIN_INFO g_layerTaskPara22C;	// ��������2-44	0x003002C01
extern PLUGIN_INFO g_layerTaskPara22D;	// ��������2-45	0x003002D01
extern PLUGIN_INFO g_layerTaskPara22E;	// ��������2-46	0x003002E01
extern PLUGIN_INFO g_layerTaskPara22F;	// ��������2-47	0x003002F01
extern PLUGIN_INFO g_layerTaskPara230;	// ��������2-48	0x003003001

extern PLUGIN_INFO g_layerTaskPara231;	// ��������2-49	0x003003101
extern PLUGIN_INFO g_layerTaskPara232;	// ��������2-50	0x003003201
extern PLUGIN_INFO g_layerTaskPara233;	// ��������2-51	0x003003301
extern PLUGIN_INFO g_layerTaskPara234;	// ��������2-52	0x003003401
extern PLUGIN_INFO g_layerTaskPara235;	// ��������2-53	0x003003501
extern PLUGIN_INFO g_layerTaskPara236;	// ��������2-54	0x003003601
extern PLUGIN_INFO g_layerTaskPara237;	// ��������2-55	0x003003701
extern PLUGIN_INFO g_layerTaskPara238;	// ��������2-56	0x003003801

extern PLUGIN_INFO g_layerTaskPara239;	// ��������2-57	0x003003901
extern PLUGIN_INFO g_layerTaskPara23A;	// ��������2-58	0x003003A01
extern PLUGIN_INFO g_layerTaskPara23B;	// ��������2-59	0x003003B01
extern PLUGIN_INFO g_layerTaskPara23C;	// ��������2-60	0x003003C01
extern PLUGIN_INFO g_layerTaskPara23D;	// ��������2-61	0x003003D01
extern PLUGIN_INFO g_layerTaskPara23E;	// ��������2-62	0x003003E01
extern PLUGIN_INFO g_layerTaskPara23F;	// ��������2-63	0x003003F01
extern PLUGIN_INFO g_layerTaskPara240;	// ��������2-64	0x003004001

/////////////////
extern PLUGIN_INFO g_layerTaskPara31;	// ��������3-1	0x003000102
extern PLUGIN_INFO g_layerTaskPara32;	// ��������3-2	0x003000202
extern PLUGIN_INFO g_layerTaskPara33;	// ��������3-3	0x003000302
extern PLUGIN_INFO g_layerTaskPara34;	// ��������3-4	0x003000402
extern PLUGIN_INFO g_layerTaskPara35;	// ��������3-5	0x003000502
extern PLUGIN_INFO g_layerTaskPara36;	// ��������3-6	0x003000602
extern PLUGIN_INFO g_layerTaskPara37;	// ��������3-7	0x003000702
extern PLUGIN_INFO g_layerTaskPara38;	// ��������3-8	0x003000802

extern PLUGIN_INFO g_layerTaskPara39;	// ��������3-9	0x003000902
extern PLUGIN_INFO g_layerTaskPara3A;	// ��������3-10	0x003000A02
extern PLUGIN_INFO g_layerTaskPara3B;	// ��������3-11	0x003000B02
extern PLUGIN_INFO g_layerTaskPara3C;	// ��������3-12	0x003000C02
extern PLUGIN_INFO g_layerTaskPara3D;	// ��������3-13	0x003000D02
extern PLUGIN_INFO g_layerTaskPara3E;	// ��������3-14	0x003000E02
extern PLUGIN_INFO g_layerTaskPara3F;	// ��������3-15	0x003000F02
extern PLUGIN_INFO g_layerTaskPara310;	// ��������3-16	0x003001002

extern PLUGIN_INFO g_layerTaskPara311;	// ��������3-17	0x003001102
extern PLUGIN_INFO g_layerTaskPara312;	// ��������3-18	0x003002202
extern PLUGIN_INFO g_layerTaskPara313;	// ��������3-19	0x003003302
extern PLUGIN_INFO g_layerTaskPara314;	// ��������3-20	0x003004402
extern PLUGIN_INFO g_layerTaskPara315;	// ��������3-21	0x003005502
extern PLUGIN_INFO g_layerTaskPara316;	// ��������3-22	0x003006602
extern PLUGIN_INFO g_layerTaskPara317;	// ��������3-23	0x003007702
extern PLUGIN_INFO g_layerTaskPara318;	// ��������3-24	0x003008802

extern PLUGIN_INFO g_layerTaskPara319;	// ��������3-25	0x003001902
extern PLUGIN_INFO g_layerTaskPara31A;	// ��������3-26	0x003001A02
extern PLUGIN_INFO g_layerTaskPara31B;	// ��������3-27	0x003001B02
extern PLUGIN_INFO g_layerTaskPara31C;	// ��������3-28	0x003001C02
extern PLUGIN_INFO g_layerTaskPara31D;	// ��������3-29	0x003001D02
extern PLUGIN_INFO g_layerTaskPara31E;	// ��������3-30	0x003001E02
extern PLUGIN_INFO g_layerTaskPara31F;	// ��������3-31	0x003001F02
extern PLUGIN_INFO g_layerTaskPara320;	// ��������3-32	0x003002002

extern PLUGIN_INFO g_layerTaskPara321;	// ��������3-33	0x003002102
extern PLUGIN_INFO g_layerTaskPara322;	// ��������3-34	0x003002202
extern PLUGIN_INFO g_layerTaskPara323;	// ��������3-35	0x003002302
extern PLUGIN_INFO g_layerTaskPara324;	// ��������3-36	0x003002402
extern PLUGIN_INFO g_layerTaskPara325;	// ��������3-37	0x003002502
extern PLUGIN_INFO g_layerTaskPara326;	// ��������3-38	0x003002602
extern PLUGIN_INFO g_layerTaskPara327;	// ��������3-39	0x003002702
extern PLUGIN_INFO g_layerTaskPara328;	// ��������3-40	0x003002802

extern PLUGIN_INFO g_layerTaskPara329;	// ��������3-41	0x003002902
extern PLUGIN_INFO g_layerTaskPara32A;	// ��������3-42	0x003002A02
extern PLUGIN_INFO g_layerTaskPara32B;	// ��������3-43	0x003002B02
extern PLUGIN_INFO g_layerTaskPara32C;	// ��������3-44	0x003002C02
extern PLUGIN_INFO g_layerTaskPara32D;	// ��������3-45	0x003002D02
extern PLUGIN_INFO g_layerTaskPara32E;	// ��������3-46	0x003002E02
extern PLUGIN_INFO g_layerTaskPara32F;	// ��������3-47	0x003002F02
extern PLUGIN_INFO g_layerTaskPara330;	// ��������3-48	0x003003002

extern PLUGIN_INFO g_layerTaskPara331;	// ��������3-49	0x003003102
extern PLUGIN_INFO g_layerTaskPara332;	// ��������3-50	0x003003202
extern PLUGIN_INFO g_layerTaskPara333;	// ��������3-51	0x003003302
extern PLUGIN_INFO g_layerTaskPara334;	// ��������3-52	0x003003402
extern PLUGIN_INFO g_layerTaskPara335;	// ��������3-53	0x003003502
extern PLUGIN_INFO g_layerTaskPara336;	// ��������3-54	0x003003602
extern PLUGIN_INFO g_layerTaskPara337;	// ��������3-55	0x003003702
extern PLUGIN_INFO g_layerTaskPara338;	// ��������3-56	0x003003802

extern PLUGIN_INFO g_layerTaskPara339;	// ��������3-57	0x003003902
extern PLUGIN_INFO g_layerTaskPara33A;	// ��������3-58	0x003003A02
extern PLUGIN_INFO g_layerTaskPara33B;	// ��������3-59	0x003003B02
extern PLUGIN_INFO g_layerTaskPara33C;	// ��������3-60	0x003003C02
extern PLUGIN_INFO g_layerTaskPara33D;	// ��������3-61	0x003003D02
extern PLUGIN_INFO g_layerTaskPara33E;	// ��������3-62	0x003003E02
extern PLUGIN_INFO g_layerTaskPara33F;	// ��������3-63	0x003003F02
extern PLUGIN_INFO g_layerTaskPara340;	// ��������3-64	0x003004002
/************************************************************************/
/* �������ò�������                                                     */
/************************************************************************/

/************************************************************************/
/* �澯ҳ���                                                           */
/************************************************************************/
extern PLUGIN_INFO g_layerAlarm1;	// ʱ�ο�8���ִεĸ澯״̬	0x00900000
extern PLUGIN_INFO g_layerAlarm2;	// ���ݿ�8���ִεĸ澯״̬	0x00900001
extern PLUGIN_INFO g_layerAlarm3;	// ʵʱ��8���ִεĸ澯״̬	0x00900002
extern PLUGIN_INFO g_layerAlarm4;	// Ӫҵ��ͣ��8���ִεĸ澯״̬	0x00900003
extern PLUGIN_INFO g_layerAlarm5;	// �յ��8���ִεĸ澯״̬	0x00900004
extern PLUGIN_INFO g_layerAlarm6;	// �µ��8���ִεĸ澯״̬	0x00900005
extern PLUGIN_INFO g_layerAlarm7;	// �����8���ִεĸ澯״̬	0x00900006
extern PLUGIN_INFO g_layerAlarm8;	// ң��8���ִεĸ澯״̬	0x00900007
extern PLUGIN_INFO g_layerAlarm9;	// �ܿ���8���ִεĸ澯״̬	0x00900008
extern PLUGIN_INFO g_layerAlarmA;	// ʱ�ο�8���ִε���բ״̬	0x00900009
extern PLUGIN_INFO g_layerAlarmB;	// ���ݿ�8���ִε���բ״̬	0x0090000A
extern PLUGIN_INFO g_layerAlarmC;	// ʵʱ��8���ִε���բ״̬	0x0090000B
extern PLUGIN_INFO g_layerAlarmD;	// Ӫҵ��ͣ��8���ִε���բ״̬	0x0090000C
extern PLUGIN_INFO g_layerAlarmE;	// �յ��8���ִε���բ״̬	0x0090000D
extern PLUGIN_INFO g_layerAlarmF;	// �µ��8���ִε���բ״̬	0x0090000E
extern PLUGIN_INFO g_layerAlarm10;	// �����8���ִε���բ״̬	0x0090000F
extern PLUGIN_INFO g_layerAlarm11;	// ң��8���ִε���բ״̬	0x00900010
extern PLUGIN_INFO g_layerAlarm12;	// �ܿ���8���ִε���բ״̬	0x00900011
/************************************************************************/
/* �澯ҳ������                                                       */
/************************************************************************/

/************************************************************************/
/* �¼���¼ҳ���                                                       */
/************************************************************************/
extern PLUGIN_INFO g_layerEventI1;	// ��Ҫ�¼� 0x00910101
extern PLUGIN_INFO g_layerEventI2;	// ��Ҫ�¼� 0x00910102
extern PLUGIN_INFO g_layerEventI3;	// ��Ҫ�¼� 0x00910103
extern PLUGIN_INFO g_layerEventI4;	// ��Ҫ�¼� 0x00910104
extern PLUGIN_INFO g_layerEventI5;	// ��Ҫ�¼� 0x00910105

extern PLUGIN_INFO g_layerEventN1;	// һ���¼� 0x00910201
extern PLUGIN_INFO g_layerEventN2;	// һ���¼� 0x00910202
extern PLUGIN_INFO g_layerEventN3;	// һ���¼� 0x00910203
extern PLUGIN_INFO g_layerEventN4;	// һ���¼� 0x00910204
extern PLUGIN_INFO g_layerEventN5;	// һ���¼� 0x00910205
/************************************************************************/
/* �¼���¼ҳ������                                                   */
/************************************************************************/


/************************************************************************/
/* ������Ϣҳ���                                                           */
/************************************************************************/
extern PLUGIN_INFO g_ZhMsgInfoDraw1;		// ������Ϣ1
extern PLUGIN_INFO g_ZhMsgInfoDraw2;		// ������Ϣ2
extern PLUGIN_INFO g_ZhMsgInfoDraw3;		// ������Ϣ3
extern PLUGIN_INFO g_ZhMsgInfoDraw4;		// ������Ϣ4	
extern PLUGIN_INFO g_ZhMsgInfoDraw5;		// ������Ϣ5	
extern PLUGIN_INFO g_ZhMsgInfoDraw6;		// ������Ϣ6	
extern PLUGIN_INFO g_ZhMsgInfoDraw7;		// ������Ϣ7	
extern PLUGIN_INFO g_ZhMsgInfoDraw8;		// ������Ϣ8	
extern PLUGIN_INFO g_ZhMsgInfoDraw9;		// ������Ϣ9	
extern PLUGIN_INFO g_ZhMsgInfoDraw10;	// ������Ϣ10	

/************************************************************************/
/* ������Ϣҳ������                                                       */
/************************************************************************/

#if ANOLOGY_EN == 1
extern PLUGIN_INFO g_anologystatus;		// ֱ��ģ���� 0x00021000
#endif

extern PLUGIN_INFO g_layerRs232IoInfo;	//����232ͨѶ����	0x02222003
extern PLUGIN_INFO g_layerSet485;     //����485 II�ڵĹ��ܣ�ȷ���ǳ���ڻ��Ǳ�����

#if TH208_RTU == 1
extern PLUGIN_INFO g_layerPhotoManual;	//�ֹ�����	0x00B61000
extern PLUGIN_INFO g_layerPhotoDump;	//��Ƭת��	0x00B61001
extern PLUGIN_INFO g_layerClearPhoto;	//�����Ƭ	0x00B61002
#endif

#if GUI_CLEAR_MTRDATA == 1
extern PLUGIN_INFO g_layerClearData;           // ���ݳ�ʼ��0x00B41000
#endif

extern PLUGIN_INFO g_layerCurPower;           // ��·���幦��0x00010001
extern PLUGIN_INFO g_layerSwitchStatus;           // ����״̬
extern PLUGIN_INFO g_layerPowerLog1;           // ���ؼ�¼0x00040000
extern PLUGIN_INFO g_layerPowerLog2;           // ���ؼ�¼0x00040001
extern PLUGIN_INFO g_layerElecLog1;           // ����ؼ�¼0x00050000
extern PLUGIN_INFO g_layerElecLog2;           // ����ؼ�¼0x00050001
extern PLUGIN_INFO g_layermonthLog1;           //�µ�ؼ�¼0x00090000
extern PLUGIN_INFO g_layermonthLog2;           //�µ�ؼ�¼0x00090001


extern PLUGIN_INFO g_layerRemoteLog1;           // ң�ؼ�¼0x00060000
extern PLUGIN_INFO g_layerRemoteLog2;           // ң�ؼ�¼0x00060001
extern PLUGIN_INFO g_layerRemoteLog3;           // ң�ؼ�¼0x00060002
extern PLUGIN_INFO g_layerRemoteLog4;           // ң�ؼ�¼0x00060003
extern PLUGIN_INFO g_layerLosteleLog1;           // ʧ���¼0x00070000
extern PLUGIN_INFO g_layerLosteleLog2;           // ʧ���¼0x00070001
extern PLUGIN_INFO g_layerLosteleLog3;           // ʧ���¼0x00070002
extern PLUGIN_INFO g_layerLosteleLog4;           // ʧ���¼0x00070003
extern PLUGIN_INFO g_layerLosteleLog5;           // ʧ���¼0x00070004

extern PLUGIN_INFO g_JcVoltagEelectricMsgDraw;  // ����������¼0x00990000
extern PLUGIN_INFO g_JcCurMsgDraw;           // ����������¼0x00990001
extern PLUGIN_INFO g_JcPAngleDraw;           // ����������¼0x00990002
extern PLUGIN_INFO g_JcPcosDraw;             // ����������¼0x00990003
extern PLUGIN_INFO g_JcZPowerDraw;             // ����������¼0x00990004
extern PLUGIN_INFO g_JcWPowerDraw;			   // ����������¼0x00990005
extern PLUGIN_INFO g_JcTPowerDraw;             // ����������¼0x00990006

extern PLUGIN_INFO g_CQKeyUserRead;
extern PLUGIN_INFO g_CQKeyUserActRead;
extern PLUGIN_INFO g_CQKeyUserReActRead;
extern PLUGIN_INFO g_CQKeyUserVolRead;
extern PLUGIN_INFO g_CQKeyUserCurRead;
extern PLUGIN_INFO g_MeterReadStat;
extern PLUGIN_INFO g_MeterReadFailStat;
extern PLUGIN_INFO g_GprsNumStat;

extern PLUGIN_INFO g_CQTotalEn;
extern PLUGIN_INFO g_CQFengEn;
extern PLUGIN_INFO g_CQPingEn;
extern PLUGIN_INFO g_CQGuEn;
extern PLUGIN_INFO g_CQJianEn;				// 0x0014000c �������й�(������)��ʾֵ
extern PLUGIN_INFO g_CQSelectDate;			// 0x0015000c ѡ�񳭱��ѯ����
extern PLUGIN_INFO g_CQMeasureParaRead;
extern PLUGIN_INFO g_CQCommunicationParaRead;
extern PLUGIN_INFO g_CQMeterParaRead;
extern PLUGIN_INFO g_CQRouterInfoQuery;	// (���켯��)�м̵����Ϣ��ѯ 0x0023000c
extern PLUGIN_INFO g_CQRouterInfoQuery1;	// (���켯��)�м̵����Ϣ��ѯ 0x0023000c
extern PLUGIN_INFO g_CQRouterInfoStat;	// (���켯��)�м̵����Ϣͳ�� 0x0043000c
#if HARDWARE_TYPE == 4
extern PLUGIN_INFO g_layerPlcRouteModeSet;  //����·��ģʽ
#endif
#if 211
extern PLUGIN_INFO g_layerPlcAutoCheck;
extern PLUGIN_INFO g_layerPlcInfoShow;
extern PLUGIN_INFO g_layerPlcInfoSet;
#if NEW_GW_GUI_APPEND == 1
extern PLUGIN_INFO g_layerPlcRelayTest;
#endif
#endif


extern PLUGIN_INFO g_layerJCSoftwareConfig;
extern PLUGIN_INFO g_layerJCSoftwareReconfig;
extern PLUGIN_INFO g_layerShowIpInfo;
#if REMOTE_VPN_EN == 1
extern PLUGIN_INFO g_RemoteVPN;         //�ն�Զ��VPN���� 0x0001BDB9
#endif

#if JIANGXI_FUN == 1 || (SHANDONG_FUN == 1 && HARDWARE_TYPE == 3)
extern PLUGIN_INFO g_ControlOutputTypeLay;
#endif

#if (SHANDONG_FUN == 1 && HARDWARE_TYPE == 3)
extern PLUGIN_INFO g_layerPulseWidthSet;
#endif

#if HUNAN_FUN == 1
extern PLUGIN_INFO g_layerJCPower_21;
extern PLUGIN_INFO g_layerJCPower_22;
extern PLUGIN_INFO g_layerJCPower_23;
extern PLUGIN_INFO g_layerVoltagEelectric_21;
extern PLUGIN_INFO g_layerVoltagEelectric_22;
extern PLUGIN_INFO g_layerVoltagEelectric_23;
extern PLUGIN_INFO g_layerVoltagEelectric_31;
extern PLUGIN_INFO g_layerVoltagEelectric_32;
extern PLUGIN_INFO g_layerVoltagEelectric_33;
extern PLUGIN_INFO g_layerPositive_Active_A;
extern PLUGIN_INFO g_layerPositive_Active_R;
#if PROTECTEC_EN == 1
//extern PLUGIN_INFO g_layerPrtlChange;
#endif
extern PLUGIN_INFO g_layerEsamInfo;
#endif

#if SHANDONG_FUN == 1
extern PLUGIN_INFO g_layerSpecialFuc;   // ��ѹѹ��ʹ�� 0x21B70078
#endif

#if HARDWARE_TYPE == 4
extern PLUGIN_INFO g_ResetPlcModeuleRouter; //0x0020000c
extern PLUGIN_INFO g_WLNetCurSate;          //0x00200006
extern PLUGIN_INFO g_WLLstReadMetTime;      //0x00200005
extern PLUGIN_INFO g_WLNetFaultData;        //0x00200004
extern PLUGIN_INFO g_WLOnlineMetData;       //0x00200003
extern PLUGIN_INFO g_WLDownLoadMetData;     //0x00200002

extern PLUGIN_INFO g_layerResetPlcModule;   //0x4000000C
extern PLUGIN_INFO g_layerClearPlcModule;   //0x4000000d
extern PLUGIN_INFO g_layerNetPlcModule;     //0x4000000e
extern PLUGIN_INFO g_layerCallPlcModule;    //0x4000000f
extern PLUGIN_INFO g_layerPlcNetStatInfo;
#endif
#if SHANGHAI_FUN == 1
/************************************************************************/
/* ʧѹ��¼ҳ���                                                           */
/************************************************************************/
extern PLUGIN_INFO g_layerLossVoltRec;	//ʧѹͳ�� 0x00830001
extern PLUGIN_INFO g_layerLossVoltNRec;	//���N��ʧѹ��¼ 0x00830002

extern PLUGIN_INFO g_layerLossVoltRec1;	//������ʧѹͳ��ҳ��1 0x00830003
extern PLUGIN_INFO g_layerLossVoltRec2; 	//������ʧѹͳ��ҳ��2 0x00830004
extern PLUGIN_INFO g_layerLossVoltRec3;	//������ʧѹͳ��ҳ��3 0x00830005

extern PLUGIN_INFO g_layerLossVoltNRec1;	//���������ʧѹ��¼1 0x00830006
extern PLUGIN_INFO g_layerLossVoltNRec2;	//���������ʧѹ��¼2 0x00830007
extern PLUGIN_INFO g_layerLossVoltNRec3;	//���������ʧѹ��¼3 0x00830008

/************************************************************************/
/* ʧѹ��¼ҳ���                                                           */
/************************************************************************/
extern PLUGIN_INFO g_layerPhoneNumber;	// �ն��ֻ����� 0x20B80000
extern PLUGIN_INFO g_layerTerminalInfo1;	//�ն���Ϣ��һ��:���һ��Ϣ 0x00100010
extern PLUGIN_INFO g_layerTerminalInfo2;	//�ն���Ϣ�ڶ���:������Ϣ 0x00100020
extern PLUGIN_INFO g_layerTerminalInfo3;	//�ն���Ϣ������:��ͨ����Ϣ(gprs/cdma) 0x00100030
extern PLUGIN_INFO g_layerTerminalInfo4;	//�ն���Ϣ������:��Ӳ���汾�� 0x00100040
extern PLUGIN_INFO g_layerTerminalInfo5;	//�ն���Ϣ������:��ͨ����Ϣ(��̫��) 0x00100050
extern PLUGIN_INFO g_layerTerminalInfo6;	//�ն���Ϣ������:�ն˱��,ICCID�š�SIM���ֻ��� 0x00100060
extern PLUGIN_INFO g_layerCommdisp;
#endif

#if GANSU_FUN == 1
extern PLUGIN_INFO		g_layerIPConfig;	 // IPConfig 0x006F0003
extern PLUGIN_INFO g_layerPublicNetCommPara;
#endif
#if ANHUI_FUN == 1 
extern PLUGIN_INFO		g_layerIPConfig;	 // IPConfig 0x006F0003
#endif
#if HUBEI_FUN == 1
extern PLUGIN_INFO g_layerTerminalInfo1;            // �ն���Ϣ 0x00100010
extern PLUGIN_INFO g_layerTerminalInfo2;            // �ն���Ϣ 0x00100020
extern PLUGIN_INFO g_layerAutoPara;
#endif
#if HUABEI_FUN == 1 || CHONG_QING_FUN == 1
#if GPRS_USB_VIRTUAL_NET_EN == 1
extern PLUGIN_INFO g_layersetusbcom;
#endif
#endif
#if GANSU_FUN == 1
extern PLUGIN_INFO g_layerTmnTrafficLmt;    //����������0x00630003
#endif
#if LIAONING_FUN == 1
extern PLUGIN_INFO g_layerMeasureMainDataJC;	//JC����������0x00440010
#endif
extern PLUGIN_INFO g_layer_cur_EleQ;//���յ��µ���
extern PLUGIN_INFO g_layer_power_ctr_evt_display_GK;
extern PLUGIN_INFO g_layer_plus_data_display;
extern PLUGIN_INFO g_layer_powerCurve_display;   //��������
extern PLUGIN_INFO g_layer_cur_powerTotal;       //��ǰ����
extern PLUGIN_INFO g_layer_gruop_cur_powerTotal;   			//��ǰ����
extern PLUGIN_INFO g_layer_magneticEvent_display;

extern PLUGIN_INFO g_port_set_485;
extern PLUGIN_INFO g_layer_gprs_numnet;
extern PLUGIN_INFO g_layer_gprs2_display;
#ifdef AREA_FUJIAN
extern PLUGIN_INFO g_4852_fun;
#endif
//�˴�����������еĽ���
#ifdef AREA_CHONGQING
extern PLUGIN_INFO g_CQSelectDate;
extern PLUGIN_INFO g_MeterReadStat;
extern PLUGIN_INFO g_CQModuleStat;
extern PLUGIN_INFO g_MeterReadFailStat;
extern PLUGIN_INFO g_GprsNumStat;
extern PLUGIN_INFO g_CQRouterInfoStat;
extern PLUGIN_INFO g_QueryEnergyTotal;
extern PLUGIN_INFO g_QueryEnergyTip;
extern PLUGIN_INFO g_QueryEnergyPeak;
extern PLUGIN_INFO g_QueryEnergyFlat;
extern PLUGIN_INFO g_QueryEnergyValley;

extern PLUGIN_INFO g_real_time_meter_reading;
extern PLUGIN_INFO g_real_time_meter_reading1;
extern PLUGIN_INFO g_real_time_reading_all;
extern PLUGIN_INFO g_real_time_reading_all_data;
extern PLUGIN_INFO g_meter_search_start;
extern PLUGIN_INFO g_meter_search_new;
extern PLUGIN_INFO g_meter_search_new_read;
extern PLUGIN_INFO g_meter_not_found_addr;
extern PLUGIN_INFO g_auto_report;

extern PLUGIN_INFO g_layer_meterfile;
extern PLUGIN_INFO g_ipDisplay;
extern PLUGIN_INFO g_moduleInfo;
extern PLUGIN_INFO g_deviceEvent;
extern PLUGIN_INFO g_relayRouteDisplay;
extern PLUGIN_INFO g_trmlVerinfo;
extern PLUGIN_INFO g_trmSNinfo;



extern PLUGIN_INFO g_wirelessComm_set; // ��������
extern PLUGIN_INFO g_changePwd;
extern PLUGIN_INFO g_changeAddr;
extern PLUGIN_INFO g_changeVPNPwd;
extern PLUGIN_INFO g_ApnSetDisplay;
extern PLUGIN_INFO g_layerGprsCfg;
extern PLUGIN_INFO g_layersetusbcom;
extern PLUGIN_INFO g_Set_CCUResetTime;
#endif

#endif//_LAYER_H_

