/*
***************************************************************************
  Copyright (c) 2006-2007,�Ͼ��������������������ι�˾
             All rights reserved.
  ��������: main.h
  ��    �ܣ�FKGA42��ʾģ�����ļ�ͷ�ļ�
  �� �� �ˣ�gaoyf
  ����ʱ�䣺2007-1

  �汾��ʷ:

    v1.0:
      2006-1-8, gaoyf : New file

***************************************************************************
*/

#ifndef _MAIN_H_
#define _MAIN_H_

#include "miniguidef.h"

// ����̨��������
#define GUI_PORT_ID				4003	/*������MINIGUI���̼�ͨ�ŵĶ˿ں�*/
#define IPC_SERVICE             2

#define GUI_CMD_POPUP			0x01	/*��ʾӦ�����ָ���˵�*/
#define GUI_CMD_KEYCODE			0x81	/*��ʾӦ�÷��ظ������Ȱ�����*/
#define GUI_CMD_OTHER			0x83	/*����������*/
#define	GUI_RD_MTR_SETTING		0x84   //��ʾӦ��Ҫ�����м�ִ������
#define	GUI_START_IMCC_COMPRESS	0x85	//����IMCCѹ��
#define	GUI_STOP_IMCC_COMPRESS	0x86	//ȡ��IMCCѹ��
#define	GUI_CLEAR_PRARA			0X87	// ������ʼ��
#define GUI_CLEAR_AC_METER_DATA 0x89    // ���潻�ɵ������
#define GUI_INIT_AC_METER_PARA  0x8A    // ���潻��ȫ���ʼ��
#define GUI_SET_AC_TIME         0X8B    //���������ն�ʱ�䣬Ҳ������������ʱ��
#if PROTOCOL_CHANGE
#define GUI_PROTOCOL_CHANG      0x8C    //��Լת��
#endif
#define	PHONE_KEY				0xAA	//ͨ������ֵ
#define	RD_METER_KEY			0xBB	//�������ֵ
#define	START_UPGRADE_IR_KEY	0xCC	//������������������
#define	STOP_UPGRADE_IR_KEY		0xDD	//������������������
#define	STOP_PHONE_KEY			0xEE	//ֹͣͨ����(����)

#if TD_SCDMA == 1
#define TDSCDMA_GUI_ID				9			/*��ʾ�����У�td-scdma��ID����*/
#define TDSCDMA_GSM_GUI_ID			10			/*��ʾ�����У�td-scdma�����ŵ���ID����*/
#define IS_GUI_GPRS(ID)		(((ID) == 2) || ((ID) == 6) || ((ID) == TDSCDMA_GUI_ID))	//�Ƿ�ΪGPRS����ETH����TD_SCDMA����ҳ��
#define NOT_GUI_GPRS(ID)	(((ID) != 2) && ((ID) != 6) && ((ID) != TDSCDMA_GUI_ID))	//��GPRS����ETH����TD_SCDMA����ҳ��
#else
#define IS_GUI_GPRS(ID)		(((ID) == 2) || ((ID) == 6))	//�Ƿ�ΪGPRS����ETH����TD_SCDMA����ҳ��
#define NOT_GUI_GPRS(ID)	(((ID) != 2) && ((ID) != 6))	//��GPRS����ETH����TD_SCDMA����ҳ��
#endif

#ifndef HUDP
#define HUDP void*
#endif

//�����Ҫͳһ jackyben
//#define	DBCENTER_IPADDR "192.168.9.188"
//#define	DBCENTER_PORT	8598

#define  GUI_INI_FILE            "gui.ini"

#define	BLUETOOH_ON		0xaa	//��������
#define	BLUETOOH_OFF	0x55	//�����ر�

//#define GUI_DB_CLIENT_NUM	250

/**********�˵������ݽṹ**********/
#define MENU_CAPTION_LEN	20
typedef struct _PLUG_STRUCT
{
	char	czCaption[MENU_CAPTION_LEN];
	DWORD	dwPlugin;
}PLUG_STRUCT;
typedef struct _MENU_INFO
{
	int		nParent;        // ���˵����������е�����
	char	czCaption[MENU_CAPTION_LEN];	// �˵�����
	BOOL	bMenuAggregate; // �Ƿ�����Ӳ˵�

	int		nLeft1;
	int		nLeft2;
	int		nTop;
	int		nInterval;
	int		nMenuMaxCnt;
	BOOL	bVisible;

	DWORD	dwMenuID;
	PLUG_STRUCT* pPlugStruct;
	int		nPlugCnt;       // ������Ӳ˵���nPlugCnt=1������nPlugCnt>=1

}MENU_INFO;

/*����ܽṹ����*/
#define MAX_TURNDISP_NUM	400
typedef struct _GUI_MAIN
{
    int nMenuRootId;            // 0 ���˵�����Id
	int		nLeft1;
	int		nLeft2;
	int		nTop;
	int		nMenuInterval;
	int		nMenuMaxCnt;

    MENU_INFO*   pPrentMenu;    // NULL�����˵�ָ�룬��Ҫ���ڶ������ҳ��ʱ��ȡ��������ҳ��
    int          nPageLayerIndex;//0   ����ҳ��ĵ�ǰ����ֵ

    PLUGIN_INFO* pluginCur;     // NULL ��ǰ�Ĺ���ҳ��
    PLUGIN_INFO* plugTemplate;  // NULL ģ��(������)ҳ��
    PLUGIN_INFO* plugPop;       // NULL ����ҳ��
    PLUGIN_INFO* plugMsg;       // NULL MessageBoxҳ��

    PLUGIN_INFO* plugOften;     // NULL ����ҳ��
    BOOL         bOftenDisp;    // FALSE �Ƿ���

    BOOL bLighton;              // TRUE �Ƿ�����
    int nLightonTime;           // 0    ����ʱ��(�ް�����Ϣ����)

    int nLightonTimeMax;        // 10   �ް�����Ϣ�������ʱ��

    BOOL bTrunDisp;             // FALSE �Ƿ�����
    int nTrunDispTime;          // 0    ����ҳ����ʾʱ��(�ް�����Ϣ����,���೤ʱ���������)
    int nTrunDispTimeMax;       // 10   �������Ե��ʱ��

    int nTrunDispCnt;           // 0    ����ҳ������
    int nTrunDispIndex;         // -1   ��ǰ����ҳ�������
    int	nInterval;              // 5    ����ҳ��֮��ļ��ʱ��

}GUI_MAIN;

extern GUI_MAIN g_guiMain;
extern PLUGIN_INFO* g_pluginTrunDisp[MAX_TRUNDISP_CNT];  // ����ҳ�������
extern int g_nTrunDisp[MAX_TRUNDISP_CNT];  // ����ҳ�������
extern int g_nTotalTrunDispCnt;

extern MENU_INFO* g_menuMain[1000];	// �˵������飬���˵���Ϊ1000
extern int g_nMenus;                // ���в˵������
extern int g_nParentIndex;          // ���˵��������е�����
extern int g_menuIdx;
extern BOOL g_bpop;

extern PLUGIN_INFO* g_plugin[]; // ����ҳ�������鼯��
extern int g_nPluginCnt;        // ������ҳ����ܵĸ���
//extern HUDP g_hUdbCenter;
extern BOOL g_bRunPaint;

extern void delete_app_info(uint32 appid);

/*********************************************************************
	�������ܵ�Ԫ����ʾ���ģ��
	����������    ��ȡָ��ҳ���ָ��
	���������    DWORD dwPluginId��ҳ���ID
	���������
	��������ֵ��  PLUGIN_INFO* ��ҳ���ָ�룬NULL��ʾ������
	�ڲ��������̣�
********************************************************************/
PLUGIN_INFO* GetPlugIn(DWORD dwPluginId);
void StopTrunDisp();
void OnPaint();

int set_bluetooth_state(uint8 state);
int get_bluetooth_state(int32 *state);

#endif/*_MAIN_H_*/

