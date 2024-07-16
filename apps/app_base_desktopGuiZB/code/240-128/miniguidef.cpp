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

extern PLUGIN_INFO g_CoUpdate;
extern PLUGIN_INFO g_layer_co_ver;
extern PLUGIN_INFO g_layer_sys_ver;
extern PLUGIN_INFO g_layer_symKey_ver;
extern PLUGIN_INFO g_layer_clientno_set;

PLUGIN_INFO* g_plugin[] =
{	
	&g_puamr,
    &g_stamr,               //����ҳ��
    &g_layerPopMsg,			// 0xFFFFFD
    &g_Template1,			// ��������

    // �˵�ҳ���
    &g_menuLayer,           // 1�в˵����  0x00A00000
    &g_menu2Layer,          // 2�в˵����  0x20A10000
    &g_layerInput,          // ���뷨�û�����  0x00a30000
    &g_layerTimeArea,       // ����ʱ�䷶Χ����0x00a40000
    &g_layerValidatePwd,    // ������֤ 0x00600000
    &g_layerFreezeTime, 	  // ��֤���� 0x00600002
    &g_layer_task_status,     //����״̬�鿴
	&g_CommDispDraw1,		// ���Ի���
    &g_CommDispDraw2,		// ���Ի���
    &g_CommDispDraw3,		// ���Ի���
    &g_CommDispDraw4,		// ���Ի���
	&g_CommDispDraw5,		// ���Ի���
    &g_CommDispDraw6,		// ���Ի���
    &g_CommDispDraw7,		// ���Ի���
    &g_CommDispDraw8,		// ���Ի���

	&g_layer_magneticEvent_display,
	&g_layer_cur_powerTotal,  			  //��ǰ����
    &g_layer_gruop_cur_powerTotal,          //��ǰ����
	&g_layer_cur_EleQ,					  //��ǰ����
	&g_layer_power_ctr_evt_display,		  //���ؼ�¼
	&g_layer_elect_ctr_evt_display,		  //��ؼ�¼
	&g_layer_pwct_ctr_evt_display,		  //ʧ���¼
	&g_layer_rct_ctr_evt_display,		  //ң�ؼ�¼
	&g_layer_switch_state_display,		  //����״̬
	&g_layer_periodCtrl_param_display,	  //ʱ�οز���
	&g_layer_restCtrl_param_display,	  //���ݿز���
	&g_layer_monthCtrl_param_display,	  //�µ�ز���
	&g_layer_downCtrl_param_display,	  //�¸��ز���
	&g_layer_stopCtrl_param_display,      //��ͣ�ز���
	&g_layer_kivp_display,				  //kikvkp
    &g_layer_kivp_puls_display,       // ���廥��  
	&g_layer_meterParam_display,		  //������
	&g_layer_powerCurve_display,           //��������

	/************************************************************************/
	/* ����״̬ 																*/
	/************************************************************************/
	&g_ControlStatusDraw1,	// ����״̬ 0x00010101
	&g_ControlStatusDraw2,	// ����״̬ 0x00010201
	&g_ControlStatusDraw3,	// ����״̬ 0x00010301
	&g_ControlStatusDraw4,	// ����״̬ 0x00010401
	&g_ControlStatusDraw5,	// ����״̬ 0x00010501
	&g_ControlStatusDraw6,	// ����״̬ 0x00010601
	&g_ControlStatusDraw7,	// ����״̬ 0x00010701
	&g_ControlStatusDraw8,	// ����״̬ 0x00010801
	
	/************************************************************************/
	/* ����״̬ ����															 */
	/************************************************************************/

	& g_EquipValDraw1,			// ���ʾ��  0x20300000
	& g_EquipValDraw2,			// ���ʾ��  0x20300001
	& g_EquipValDraw3,			// ���ʾ��  0x20300002
	& g_EquipValDraw4,			// ���ʾ��  0x20300003
	& g_EquipValDraw5,			// ���ʾ��  0x20300004
	& g_EquipValDraw6,			// ���ʾ��  0x20300005
	& g_EquipValDraw7,			// ���ʾ��  0x20300006
	& g_EquipValDraw8,			// ���ʾ��  0x20300007
	& g_EquipValDraw9,			// ���ʾ��  0x20300008
	& g_EquipValDraw10,			// ���ʾ��  0x20300009
	& g_EquipValDraw11,			// ���ʾ��  0x2030000a
	& g_EquipValDraw12,			// ���ʾ��  0x2030000b
	& g_EquipValDraw13,			// ���ʾ��  0x2030000c
	& g_EquipValDraw14,			// ���ʾ��  0x2030000d
	& g_EquipValDraw15,			// ���ʾ��  0x2030000e
	& g_EquipValDraw16,			// ���ʾ��  0x2030000f


	& g_HEquipValDraw1,			// ���ʾ��  0x20300000
	& g_HEquipValDraw2,			// ���ʾ��  0x20300001
	& g_HEquipValDraw3,			// ���ʾ��  0x20300002
	& g_HEquipValDraw4,			// ���ʾ��  0x20300003
	& g_HEquipValDraw5,			// ���ʾ��  0x20300004
	& g_HEquipValDraw6,			// ���ʾ��  0x20300005
	& g_HEquipValDraw7,			// ���ʾ��  0x20300006
	& g_HEquipValDraw8,			// ���ʾ��  0x20300007
	& g_HEquipValDraw9,			// ���ʾ��  0x20300008
	& g_HEquipValDraw10,		// ���ʾ��  0x20300009
	& g_HEquipValDraw11,		// ���ʾ��  0x2030000a
	& g_HEquipValDraw12,		// ���ʾ��  0x2030000b
	& g_HEquipValDraw13,		// ���ʾ��  0x2030000c
	& g_HEquipValDraw14,		// ���ʾ��  0x2030000d
	& g_HEquipValDraw15,		// ���ʾ��  0x2030000e
	& g_HEquipValDraw16,		// ���ʾ��  0x2030000f
	/************************************************************************/
	/* ������Ϣҳ���                                                           */
	/************************************************************************/
	& g_ZhMsgInfoDraw1,		// ������Ϣ1  0x20400000
	& g_ZhMsgInfoDraw2,		// ������Ϣ2  0x20400001
    &g_ZhMsgInfoDraw3,		// ������Ϣ3	0x20400002
	&g_ZhMsgInfoDraw4,		// ������Ϣ4	0x20400003
	&g_ZhMsgInfoDraw5,		// ������Ϣ5	0x20400004
	&g_ZhMsgInfoDraw6,		// ������Ϣ6	0x20400005
	&g_ZhMsgInfoDraw7,		// ������Ϣ7	0x20400006
	&g_ZhMsgInfoDraw8,		// ������Ϣ8	0x20400007
	&g_ZhMsgInfoDraw9,		// ������Ϣ9	0x20400008
	&g_ZhMsgInfoDraw10,		// ������Ϣ10	0x20400009	
	
	/************************************************************************/
	/* ������Ϣҳ������                                                       */
	/************************************************************************/

	/************************************************************************/
	/* ������Ϣ                                                              */
	/************************************************************************/
    &g_BuyDataDraw1,	// ������Ϣ	0x00010100
    &g_BuyDataDraw2,	// ������Ϣ	0x00010200
    &g_BuyDataDraw3,	// ������Ϣ	0x00010300
    &g_BuyDataDraw4,	// ������Ϣ	0x00010400
    &g_BuyDataDraw5,	// ������Ϣ	0x00010500
    &g_BuyDataDraw6,	// ������Ϣ	0x00010600
    &g_BuyDataDraw7,	// ������Ϣ	0x00010700
    &g_BuyDataDraw8,	// ������Ϣ	0x00010800
	/************************************************************************/
	/*  ������Ϣ����                                                             */
	/************************************************************************/

	&g_layer_address_set,		//���ò���
    &g_layer_address_set1,               //ͨ�ŵ�ַ����
	&g_TermDataDraw,					// �ն���Ϣ  0x20600000
	&g_layer_app_version_display,
	&g_UpdateStatusDraw,
	&g_gprsStatusDraw,					//������Ϣ
	&g_ChannelDebugStatusDraw,
	&g_layer_ac_vol_cur_display,		//���ɵ�ѹ����
    &g_NetmodeDraw,
	&g_ModemversionDraw,
	
//	&g_layerChnlInfo,					//ͨѶͨ������
	&g_layerEthernet,
	&g_layer_gprs_set,
	&g_layer_networkmode_set,
	&g_layer_meterfile_set,             //����������
	&g_layerPulseMain,	  				// �������������  0x20740000
	&g_layerGroupMain,                   //�ܼ�������
	&g_Set_ParaResetTime,				//ʱ������
	&g_layerChangePwd,                  //��������
	//&g_layerac_FeatureCfg,          //���ɵ�������
	&g_layerRadioCfg,					// ��̨�������� 0x20B70000
	&g_layerRadioLongSendCfg,					// ��̨����
	&g_MeterReadFailStat,
	&g_layer_normal_plan_set,

	&g_layer_ip_set,					//IP����
	&g_layerTrunDisp,					//ҳ������
    &g_layer_reboot_terminal,           //�����ն�
    &g_layer_data_initialization,		//���ݳ�ʼ��
    &g_layer_param_initialization,		//������ʼ��
    &g_layer_ac_param_initialization,	//���ɲ�����ʼ��
    &g_layer_data_param_initialization,	//�������ݳ�ʼ��
    &g_smiOSLogExport,					//��־����
    &g_SSHSet_Layer,       //SSH��������
    &g_smiOSUUpdate,					//U������
    &g_LogClear_Layer,					//��־���
    &g_layersetSafeONOFF,  				//��ȫģʽ���ùر�0x000c0003
    &g_layer_wirelessStatus_set,        //���߲��ŷ�ʽ
    &g_ACModuleUpdate,                  //����ģ������ 0x00A000F6
    &g_layer_ac_ver,                    //���ɰ汾
    &g_CoUpdate,            //Э���������� 0x00A000FE
    &g_layer_co_ver,        //Э�������汾
	&g_layer_sys_ver,       //ϵͳ�汾
	&g_layer_symKey_ver,    //ϵͳ��Կ�汾
	&g_layer_gprs_numnet,
	&g_layer_gprs2_display,
    &g_layer_clientno_set,                  //�ͻ����
	&g_layerProductVoltageCfg,
    #ifdef AREA_JIANGSU
    &g_layer_loop_state_display,
    &g_layer_inspector_ver_display,
    &g_layer_GPRS_param_clean,
    #endif	
	&g_disk_status,              /*������Ϣ*/
};
int g_nPluginCnt = sizeof(g_plugin)/sizeof(g_plugin[0]);

