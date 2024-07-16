/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief��   ͳһ�ն�ƽ̨��Ϣ����
* @date��    2019/12/27
* @history�� 
*********************************************************************
*/

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "dataType.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define  NAME_BROADCAST                "Broadcast"

/*ͨ����Ϣ�ӿ�*/
#define MESSAGE_IID_COMMON             0x0000
#define COMMON_IOP_URGENT_POWERDOWN    0x0001    //����ͣ�磨�¼���
#define COMMON_IOP_SYSTEM_POWERDOWN    0x0002    //ϵͳͣ�磨�¼���
#define COMMON_IOP_SYSTEM_RESTART      0x0003    //ϵͳ��λ���¼���
#define COMMON_IOP_SYSTEM_POWERON      0x0004    //ϵͳ�ϵ磨�¼���
#define COMMON_IOP_SYSTEM_TIMING       0x0005    //ϵͳУʱ���¼���
#define COMMON_IOP_HERTBEAT            0x0010    //�������
#define COMMON_IOP_MAINTAIN            0x0011    //ִ��ά������
#define COMMON_IOP_TRANSPORT           0x0012    //͸������
#define COMMON_IOP_VERSION             0x0013    //�汾��Ϣ
#define COMMON_IOP_ERRMSG              0x0014    //������Ϣ
#define COMMON_IOP_GETAPPID            0x0016    //��ѯAPPID


/*ϵͳ��������Ϣ�ӿ�*/
#define MESSAGE_IID_SYSTEM             0x0001
#define SYSTEM_IOP_EVENT_ETHSTATUS     0x0001    //��̫�����
#define SYSTEM_IOP_EVENT_IPCHANGE      0x0002    //IP��ַ����¼�
#define SYSTEM_IOP_APP_REGISTER        0x0010    //App����ע��
#define SYSTEM_IOP_APP_UNREGISTER      0x0011    //Appȡ��ע��
#define SYSTEM_IOP_GET_REGISTERAPP     0x0012    //��ѯ��ע��App
#define SYSTEM_IOP_GET_APP             0x0013    //��ѯָ��App��Ϣ
#define SYSTEM_IOP_SUB_EVENT           0x0020    //�¼�����
#define SYSTEM_IOP_UNSUB_EVENT         0x0021    //�¼�ȡ������
#define SYSTEM_IOP_SEEK_SUBEVENT       0x0022    //��ѯ���ĵ��¼���Ϣ
#define SYSTEM_IOP_REQUIRE_RESTART     0x0030    //����ϵͳ����
#define SYSTEM_IOP_GET_POWERSTATUS     0x0031    //��ѯ��Դ״̬
#define SYSTEM_IOP_GET_ETHSTATUS       0x0032    //��ѯ��̫�����״̬
//#define SYSTEM_IOP_REG_SERVERHOST      0x0033    //ע�������վ��ַ
//#define SYSTEM_IOP_UNREG_SERVERHOST    0x0034    //ȡ��ע�������վ��ַ
#define SYSTEM_IOP_SET_IPADDR          0x0035    //���ñ���IP��ַ
#define SYSTEM_IOP_ADD_ROUTE           0x0036    //���·�ɱ�
#define SYSTEM_IOP_DEL_ROUTE           0x0037    //ɾ��·�ɱ�
#define SYSTEM_IOP_RCT                 0x0038    //ʱ��ͬ��
#define SYSTEM_IOP_START_SNTP          0x0039    //����SNTP��ʱ
#define SYSTEM_IOP_GET_DEVINFO         0x003A    //��ȡ�豸��Ϣ
#define SYSTEM_IOP_SYSTEM_CMD          0x003B    //ִ��system����
#define SYSTEM_IOP_GET_CONTAINER       0x003C    //��ȡ������Ϣ
#define SYSTEM_IOP_GET_CONTAINER_APP   0x003D    //��ȡ������App��Ϣ
#define SYSTEM_IOP_GET_CUR_DEVINFO     0x003E    //��ȡ�豸��ǰ������Ϣ
#define SYSTEM_IOP_APP_MANAGER         0x0040    //APP����
#define SYSTEM_IOP_SERVICE_START       0x0040    //��������
#define SYSTEM_IOP_SERVICE_STOP        0x0041    //�رշ���

/*����ģ�鲦�Ź���App��Ϣ�ӿ�*/
#define MESSAGE_IID_WIRELESS           0x0002
#define WIRELESS_IOP_DIAG_EVENT        0x0001    //����״̬�仯�¼�
#define WIRELESS_IOP_GET_MODULEINFO    0x0010    //��ѯģ����Ϣ
#define WIRELESS_IOP_GET_DIAGINFO      0x0011    //��ѯ����״̬
#define WIRELESS_IOP_SEND_SMS          0x0012    //���Ͷ���
#define WIRELESS_IOP_REVC_SMS          0x0013    //���ն���
#define WIRELESS_IOP_SET_DIAGARG       0x0014    //�޸Ĳ��Ų���
#define WIRELESS_IOP_REDIAG            0x0015    //���²���
#define WIRELESS_IOP_GET_GPS           0x0016    //��ȡGPS��λ��Ϣ
#define WIRELESS_IOP_SET_AUTH          0x0017    //���ü�Ȩ��ʽ
#define WIRELESS_IOP_SET_SMSNO         0x0018    //���ö������ĺ���
#define WIRELESS_IOP_GET_NUM           0x0019    //����ͳ��
#define WIRELESS_IOP_TRANS_AT          0x0020    //͸��ATָ��
#define WIRELESS_IOP_GTOPTSCAN_AT      0xFF01    //�����ź�����
#define WIRELESS_IOP_GTOPTSCAN_LAST_AT 0xFF02    //�ṩ���1���ź��������

/*���س���ģ�����App��Ϣ�ӿ�*/
#define MESSAGE_IID_CCOROUTER          0x0003
#define CCOROUTER_IOP_REPORT_EVENT     0x0001    //�ϱ��¼�����
#define CCOROUTER_IOP_OFFLINE_EVENT    0x0002    //�ϱ������¼�
#define CCOROUTER_IOP_PORT_CFG         0x0010    //����˿ڹ�������
#define CCOROUTER_IOP_ADD_METER        0x0020    //��ӱ���
#define CCOROUTER_IOP_DEL_METER        0x0021    //ɾ������
#define CCOROUTER_IOP_CLEAR_METER      0x0022    //�������
#define CCOROUTER_IOP_REPORT_METER     0x0031    //�ϱ���������
//#define CCOROUTER_IOP_REPORT_EVENT     0x0032    //�ϱ��¼�����
#define CCOROUTER_IOP_TRANS_DATA       0x0033    //����͸��
#define CCOROUTER_IOP_START_SEEKMETER  0x0034    //�����ѱ�
#define CCOROUTER_IOP_PAUSE_SEEKMETER  0x0035    //��ͣ�ѱ�
#define CCOROUTER_IOP_REPORT_SEEKDATA  0x0036    //�ϱ��ѱ�����
#define CCOROUTER_IOP_GET_NODE_DELAY   0x0037    //��ȡ�ӽڵ�����ʱ
#define CCOROUTER_IOP_MONITOR_NODE     0x0038    //�ӽڵ���
#define CCOROUTER_IOP_GET_BROAD_DELAY  0x0039    //��ȡ�㲥��ʱ
#define CCOROUTER_IOP_BROADCAST        0x003a    //�㲥
#define CCOROUTER_IOP_SET_ADDR         0x003b    //ģ���ַ����
#define CCOROUTER_IOP_RECORD_NUM       0x003d    //��ǰ���õ�����
#define CCOROUTER_IOP_ACTIVE_READMETER 0x003e    //��������
#define CCOROUTER_IOP_PAUSE_READMETER  0x0040    //��ͣ����
#define CCOROUTER_IOP_RECOV_READMETER  0x0041    //�ָ�����
#define CCOROUTER_IOP_RESTAR_READMETER 0x0042    //��������
#define CCOROUTER_IOP_SEEK_METER       0x0043    //��ѯ����
#define CCOROUTER_IOP_GET_HPLC         0x0050    //��ѯHPLC��Ϣ
#define CCOROUTER_IOP_REPORT_HPLC      0x0051    //�ϱ�HPLC��ѯ��Ϣ
#define CCOROUTER_IOP_SET_HPLC_FRE     0x0052    //HPLCƵ������
#define CCOROUTER_IOP_GET_HPLC_FRE     0x0053    //HPLCƵ���ѯ
#define CCOROUTER_IOP_GET_HPLC_AREA    0x0054    //HPLC̨��ʶ��
#define CCOROUTER_IOP_GET_HPLC_OFFLINE 0x0055    //HPLC������֪
#define CCOROUTER_IOP_HPLC_REFUSE      0x0056    //HPLC�ܾ��ڵ��ϱ�
#define CCOROUTER_IOP_GET_SEEKMETER_ST 0x0060    //��ѯ����״̬
#define CCOROUTER_IOP_GET_SEEKMODULE   0x0061    //��ѯ����ģ����Ϣ

#define CCOROUTER_IOP_MTREVENT_RPT     0x0003    //������չ���¼��ϱ�֪ͨ

/*ģ���������Ϣ�ӿ�*/
#define MESSAGE_IID_MODULE             0x0004
#define MODULE_IOP_STATUS_EVENT        0x0001    //ģ�����¼�
#define MODULE_IOP_GET_INFO            0x0010    //��ѯģ����Ϣ
#define MODULE_IOP_GET_PORTINFO        0x0011    //��ѯģ���λ��Ϣ
#define MODULE_IOP_GET_STATUS          0x0012    //��ѯģ����״̬
#define MODULE_IOP_RESTORE             0x0013    //ģ�鸴λ
#define MODULE_IOP_RESTART             0x0014    //ģ������
#define MODULE_IOP_FILE_TRANSFER       0x0015    //�ļ�����
#define MODULE_IOP_GET_ALL             0x0016    //��ѯ����ģ����Ϣ

/*����������Ϣ�ӿ�*/
#define MESSAGE_IID_DB                 0x0005
#define DB_IOP_UPDATE_EVENT            0x0001    //���ݸ����¼�
#define DB_IOP_INIT_EVENT              0x0002    //��ʼ���¼�
#define DB_IOP_READ_NORMAL_DATA        0x0010    //����ͨ����
#define DB_IOP_WRITE_NORMAL_DATA       0x0011    //д��ͨ����
#define DB_IOP_READ_RECORD_START       0x0012    //��ʼ����¼������
#define DB_IOP_READ_RECORD_STEP        0x0013    //������ȡ��¼
#define DB_IOP_READ_RECORD_END         0x0014    //������¼�����ݶ�ȡ
#define DB_IOP_WRITE_RECORD            0x0015    //д��¼������
#define DB_IOP_UPDATE_RECORD           0x0016    //���¼�¼����
#define DB_IOP_GET_RECORD_NUM          0x0020    //��ȡ�洢��¼����
#define DB_IOP_GET_RECORD_DEPTH        0x0021    //��ȡ�洢��¼���
#define DB_IOP_INIT_DATA               0x0030    //���ݳ�ʼ��
#define DB_IOP_CLEAR_ONE_DATA          0x0031    //���ָ������
#define DB_IOP_RECOVERY_DEFAULT        0x0032    //�ָ�����
#define DB_IOP_READ_RECORD_START_FJ    0xF012    //��ʼ����¼������(��������)
#define DB_IOP_CLEAR_ONE_DATA_FJ       0xF031    //���ָ������(��������)

/*��ȫ���߼����Ϣ�ӿ�*/
#define MESSAGE_IID_SCS                0x0006
#define SCS_IOP_ETH_PORT_OPEN          0x0001    //��̫��Զ�̶˿ڿ��ż��
#define SCS_IOP_ETH_PORT_CONN          0x0002    //��̫��������Ϣ���
#define SCS_IOP_KEY_PATH_CHGE          0x0003    //�ؼ�Ŀ¼�ļ�������
#define SCS_IOP_ETH_LOGIN_CONSOLE      0x0004    //��̫����¼����̨���
#define SCS_IOP_DANGER_OPREATE         0x0005    //Σ������������
#define SCS_IOP_LOCAL_MAINTAIN         0x0006    //SSH�����쳣���
#define SCS_IOP_PASSWARD_CHANGED       0x0007    //������Ϣ����쳣���
#define SCS_IOP_UART_PORT_LOGIN        0x0008    //���ڵ�¼����̨���
#define SCS_IOP_SESSION_ABNORMAL       0x0009    //�����Ự�쳣���
#define SCS_IOP_BANDWIDTH_ABNORMAL     0x000A    //���������쳣���
#define SCS_IOP_PROCESS_ABNORMAL       0x000B    //�����쳣���
#define SCS_IOP_LOGIN_ABNORMAL         0x000C    //�û���¼�쳣���
#define SCS_IOP_SERVICE_OPEN           0x0017    //��������
#define SCS_IOP_SERVICE_CLOSE          0x0018    //�رշ���
#define SCS_IOP_FLOW_PARAM_SET         0x0010    //�����������޲���
#define SCS_IOP_FLOW_PARAM_GET         0x0011    //��ѯ�������޲���
#define SCS_IOP_TIME_OUT_SET           0x0012    //���ó�ʱ��δ�رշ����жϲ���

/*ͨ�Ŵ�����Ϣ�ӿ�*/
#define MESSAGE_IID_PROXY              0x1776
#define PROXY_IOP_PROTOCOL_DATA_SEND   0x0011    //Э�����ݷ���
#define PROXY_IOP_PHYSIC_CONNECT       0x0012    //proxy����վ��������״̬֪ͨ
#define PROXY_IOP_APPLY_CONNECT        0x0013    //�ò�����վӦ������״̬֪ͨ

/*������Ϣ�ӿ�*/
#define MESSAGE_IID_DESKGUI            0x0007
#define DESKGUI_IOP_DISPLAY_ENENT      0x0001    //����֪ͨ
#define DESKGUI_IOP_APP_ACTIVATE       0x0002    //app�����¼�
#define DESKGUI_IOP_APP_REG            0x0010    //appע��
#define DESKGUI_IOP_APP_UNREG          0x0011    //app�˳�����������
#define DESKGUI_IOP_FRESH_STATE        0x0012    //ˢ��״̬��
#define DESKGUI_IOP_FRESH_TOOL         0x0013    //ˢ�¹�����
#define DESKGUI_IOP_PUSH_HINT          0x0014    //������Ϣ��ʾ��
#define DESKGUI_IOP_POP_HINT           0x0015    //ȡ����Ϣ��ʾ��
#define DESKGUI_IOP_EXIT_DISPLAY       0x0016    //app�˳�����
#define DESKGUI_IOP_LIVE_APP           0x0017    //��ǰ��Ծapp
#define    DESKGUI_IOP_SCREEN_CTR      0x0018    //���Կ���

/*ң���������*/
#define MESSAGE_IID_RSPSAMPLE          0x0008
#define RSPSAMPLE_IOP_RMT_EVENT        0x0001    //ң�ű�λ�¼�
#define RSPSAMPLE_IOP_RMT_STATE        0x0011    //ң��״̬
#define RSPSAMPLE_IOP_PULSE_STATIC     0x0012    //���������ͳ��ʱ��
#define RSPSAMPLE_IOP_CONFIGURE        0x0013    //ң�����幦������
#define RSPSAMPLE_IOP_SHAKE_TIME       0x0014    //ң�ŷ���ʱ��
//����ר���������Ϣ
#define RSPSAMPLE_IOP_WORKMODE_GET     0x0020    //��ѯ485-2�ڵ�ǰ����ģʽ
#define RSPSAMPLE_IOP_WORKMODE_SET     0x0021    //����485-2�ڵ�ǰ����ģʽ

/*���ڷ������*/
#define MESSAGE_IID_UART               0x0009
#define UART_IOP_NOTIFY_MODE           0x0001    //ģʽ���֪ͨ
#define UART_IOP_NOTIFY_RECV           0x0002    //��������֪ͨ
#define UART_IOP_GET_INFO              0x0010    //������Ϣ��ѯ
#define UART_IOP_SET_PARAM             0x0011    //���ڲ�������
#define UART_IOP_SET_LISTEN            0x0012    //��ģʽAPPע��
#define UART_IOP_SEND_DATA             0x0013    //���ݷ���
#define UART_IOP_SET_ABNORMAL          0x0014    //ͨ���쳣֪ͨ
#define UART_IOP_UNSET_LISTEN          0x0015    //ȡ����ģʽAPPע��
#define UART_IOP_ONLYMODE_SET          0x0016    //����ģʽ����

/*�����������*/
#define MESSAGE_IID_BLE                0x000A
#define BLE_IOP_RECV_EVENT             0x0001    //����֪ͨ
#define BLE_IOP_PORT_EVENT             0x0002    //�����˿�֪ͨ
#define BLE_IOP_SEND_DATA              0x0010    //��������
#define BLE_IOP_TRAN_DATA              0x0011    //����ת��
#define BLE_IOP_SET_CONFIG             0x0012    //������������
#define BLE_IOP_GET_CONFIG             0x0013    //����������ѯ
#define BLE_IOP_SET_PORT               0x0014    //�����˿�����
#define BLE_IOP_LINK_INFO              0x0015    //����������Ϣ
#define BLE_IOP_CHECK_METER            0x0016    //�����������

//�Զ����ȡ����sn
#define BLE_IOP_BT_SN                  0x0090    //����������Ϣ
#define BLE_IOP_SET_BT_SN              0x0091    //����������Ϣ

/*��̨�������*/
#define MESSAGE_IID_RADIO              0x200A
#define RADIO_IOP_RECV_EVENT           0x0001    //����֪ͨ
#define RADIO_IOP_SEND_DATA            0x0010    //��������
#define RADIO_IOP_SET_CONFIG           0x0011    //��̨��������
#define RADIO_IOP_SET_ADDR             0x0012    //��̨��������
#define RADIO_IOP_GET_CONFIG           0x0013    //��̨������ѯ
#define RADIO_IOP_GET_STATUS           0x0014    //��̨״̬��ѯ
#define RADIO_IOP_SET_YANG             0x0015    //��̨״̬��ѯ
#define RADIO_IOP_LONG_SENG            0x0016    //��̨����
#define RADIO_IOP_GET_VERSION          0x0017    //��̨�汾��ѯ


/*��������*/
#define MESSAGE_IID_IOT                0x000B
#define IOT_IOP_DOCKER_ACTION          0x0001
#define IOT_IOP_DOCKER_WARRING         0x0002
#define IOT_IOP_APP_ACTION             0x0003
#define IOT_IOP_APP_WARRING            0x0004
#define IOT_IOP_SYS_MOVE_UP            0x0006
#define IOT_IOP_SYS_WARRING            0x0005
#define IOT_IOP_DOCKER_INSTALL         0x0010
#define IOT_IOP_DOCKER_ACTION1         0x0011
#define IOT_IOP_DOCKER_SET             0x0012
#define IOT_IOP_DOCKER_FIND            0x0013
#define IOT_IOP_APP_INSTALL            0x0014
#define IOT_IOP_APP_ACTION1            0x0015
#define IOT_IOP_APP_SET                0x0016
#define IOT_IOP_APP_FIND               0x0017
#define IOT_IOP_APP_LICENSE_SET        0x0018
#define IOT_IOP_APP_LICENSE_FIND       0x0019
#define IOT_IOP_SYS_MOVE_UP1           0x001A
#define IOT_IOP_SYS_VERSION_FIND       0x001B
#define IOT_IOP_LOG_INFO_FIND          0x001C
#define IOT_IOP_LOG_PACK               0x001D
#define IOT_IOP_LOG_DELECT             0x001E
#define IOT_IOP_SYS_INSPECT            0x001F


/*�ɼ�������ȹ���APP��Ϣ�ӿ�*/
#define MESSAGE_IID_CTS                0x1003
#define CTS_IOP_READ_LIST_REQ          0x0010    //͸��ת��
#define CTS_IOP_READ_ONE_REQ           0x0011    //����
#define CTS_IOP_SET_LIST_REQ           0x0021    //��������������ɸ������������ɸ���������
#define CTS_IOP_SET_READ_LIST_REQ      0x0013    //���ճ���״̬
#define CTS_IOP_ACT_LIST_REQ           0x0014    //�������������ɸ������������ɸ����󷽷�
#define CTS_IOP_ACT_READ_LIST_REQ      0x0015    //�������������ȡ���ɸ������������ɸ����󷽷�������
#define CTS_IOP_TRANSPORT_REQ          0x0016    //����������͸��ת������
#define CTS_IOP_CARRIRT_TRANS          0x0020    //�ز�/΢�������߽ӿ�͸��ת��
#define CTS_IOP_SEEK_METER             0x0012    //�ѱ����

#define CTS_IOP_TRANSTASK_RPT          0x0001    //������չ��͸�������ϱ�֪ͨ
#define CTS_IOP_TRANSTASK_STATE        0x0030    //������չ��͸������ɼ�״̬����
#define CTS_IOP_POWERUP_EVT       	   0x0031    //������չ���ն��ϵ��¼���Ϣ
#define CTS_IOP_DEL_PN_DATE            0x1022     //ɾ������������


/*ר�为�ɿ���APP��Ϣ�ӿ�*/
#define MESSAGE_IID_LOADCTRL           0x1004
#define LOADCTRL_IOP_ALARM             0x0010    /* �澯���� */
#define LOADCTRL_IOP_TRIP              0x0011    /* Զ����բ */
#define LOADCTRL_IOP_CLOSE             0x0012    /* Զ�̺�բ */
#define LOADCTRL_IOP_SELECT            0x0013    /* ң������ */
#define LOADCTRL_IOP_PROTECT           0x0014    /* ������� */
#define LOADCTRL_IOP_FEE_INPUT         0x0015    /* �߷Ѹ澯Ͷ�� */
#define LOADCTRL_IOP_FEE_REMOVE        0x0016    /* �߷Ѹ澯��� */
#define LOADCTRL_IOP_PERIOD_CHANGE     0x0017    /* ʱ�ι��ط����л� */
#define LOADCTRL_IOP_INPUT             0x0018    /* ����Ͷ�� */
#define LOADCTRL_IOP_REMOVE            0x0019    /* ���ƽ�� */
#define LOADCTRL_IOP_RELAY_MODE        0x001A    /* �̵���ģʽ */


/*�ɼ�������ȹ���APP��Ϣ�ӿ�*/
#define MESSAGE_IID_SELF               0x1155
#define SELF_IOP_GET_VERSION           0x9090    //��ѯ���̰汾��Ϣ

/*�����Զ�����Ϣ�ӿ�*/
#define MESSAGE_IID_ACMETER            0xFFF0
#define ACMETER_IOP_AC_PARM_INIT       0x0001    //���ɲ�����ʼ��
#define ACMETER_IOP_AC_DATA_INIT       0x0002    //�������ݳ�ʼ��
#define ACMETER_IOP_AC_DEMAND_INIT     0x0003    //����������ʼ��
#define ACMETER_IOP_AC_SYSTEM_CMD      0x0004    //ִ��system����
#define ACMETER_IOP_AC_RATE_SET        0x0005    //���ɷ�������

/*̨��״̬����APP��Ϣ�ӿ�*/
#define MESSAGE_IID_PDS                0x1004
#define PDS_IOP_TOPOLOGY_START         0x0010    //��������

/*�������APP��Ϣ�ӿ�*/
#define MESSAGE_IID_ENVMONITOR    0x1005
#define ENV_IOP_EVENT             0x0001    //�ϱ��¼�

/*JSON�ӿ�*/
#define MESSAGE_IID_INTERFACE          0xFFF1
#define INTERFACE_IOP_POP_CTRL         0x0001    //ȡ��������Ϣ��ʾ��
#define INTERFACE_IOP_FRESH_STATE      0x0012    //ˢ��״̬��
#define INTERFACE_IOP_FRESH_TOOL       0x0013    //ˢ�¹�����
#define INTERFACE_IOP_PUSH_CTRL        0x0014    //����������Ϣ��ʾ��
#define INTERFACE_IOP_VOICE            0x0020    //������ʾ
#define INTERFACE_IOP_UPDATE_PROGRESS  0x0021    //��������(ģ��)
#define INTERFACE_IOP_UPDATE_DB        0x0022    //�������

#define MESSAGE_RPM_RECV        0
#define MESSAGE_RPM_START       1
#define MESSAGE_ADDR_LEN        64
/* �����ֽڶ���, û���ϸ��ձ��ĸ�ʽ, �յ�����Ϣ���������¸�ʽ */
typedef struct tagMessageInfo
{
    uint8  rpm;         /* ������־λ, 1-����, 0-������ */
    uint8  priority;    /* ���ȼ� */

    uint8  sourlen;
    uint8  destlen;
    char   souraddr[MESSAGE_ADDR_LEN];
    char   destaddr[MESSAGE_ADDR_LEN];
    
    uint16 index;       /* ��Ϣ��� */
    uint16 label;       /* ��Ϣ��ǩ */
    
    uint16 IID;
    uint16 IOP;

    uint32 msglen; 
    uint8  playload[0];
}MESSAGE_INFO_T;

#define MQTT_TOPIC_A_XDR           "A-XDR"
#define MQTT_TOPIC_JSON            "JSON"

/* TTU-MQTT������� */
#define MQTT_TOPIC_OPER_GET        "get"
#define MQTT_TOPIC_OPER_SET        "set"
#define MQTT_TOPIC_OPER_ACT        "action"
#define MQTT_TOPIC_OPER_REPORT     "report"
#define MQTT_TOPIC_OPER_TRANS      "transparant"
#define MQTT_TOPIC_INFO_REQ        "request"
#define MQTT_TOPIC_INFO_RES        "response"
#define MQTT_TOPIC_INFO_NOTIFY     "notification"
#define MQTT_TOPIC_INFO_NOTIFYOS   "notify"

/* ϵͳ���������� */
#define SYS_APP_NAME               "OS-system"
#define SYS_TOPIC_REGISTER         "register"
#define SYS_TOPIC_UNREGISTER       "unRegister"
#define SYS_TOPIC_KEEPALIVE        "keepAlive"
#define SYS_TOPIC_GETREGLIST       "getRegisterList"
#define SYS_TOPIC_GETREGAPP        "getRegisterApp"
#define SYS_TOPIC_DEVINFO          "devInfo"
#define SYS_TOPIC_DEVSATAUS        "devStatus"
#define SYS_TOPIC_POWERSATAUS      "powerStatus"
#define SYS_TOPIC_GETETH           "getEthCfg"
#define SYS_TOPIC_SETETH           "setEthCfg"
#define SYS_TOPIC_ETHSTATUS        "ethStatus"
#define SYS_TOPIC_GETAPPINFO       "getAppInfo"
#define SYS_TOPIC_RESTARTAPP       "restartApp"
#define SYS_TOPIC_GETCONTLIST      "getContainerInfoList"
#define SYS_TOPIC_GETAPPINFOLIST   "getAppInfoList"
#define SYS_TOPIC_GETAPPID         "getAppId"
#define SYS_TOPIC_GETAPPLIC        "getAppLic"
#define SYS_TOPIC_UPDAPPLIC        "updateAppLic"
#define SYS_TOPIC_SETTIME          "setTime"
#define SYS_TOPIC_ADDROUTE         "addRoute"
#define SYS_TOPIC_DELROUTE         "delRoute"
#define SYS_TOPIC_GETSNTP          "getSntpCfg"
#define SYS_TOPIC_SETSNTP          "setSntpCfg"
#define SYS_TOPIC_REBOOT           "reboot"
#define SYS_TOPIC_REBOOTEVENT      "toRebootEvent"
#define SYS_TOPIC_ETHSTATUSEVENT   "ethStatusEvent"
#define SYS_TOPIC_ETHCFGEVENT      "ethCfgEvent"
#define SYS_TOPIC_POWERSTOPEVENT   "powerStopEvent"
#define SYS_TOPIC_POWERONEVENT     "powerOnEvent"
#define SYS_TOPIC_TIMEEVENT        "setTimeEvent"
#define SYS_TOPIC_INSTLL_CON       "installContainer"
#define SYS_TOPIC_INSTLL_APP       "installAPP"
#define SYS_TOPIC_UPDATE_APP       "updateAPP"
#define SYS_TOPIC_OPERATE_APP      "OperatingAPP"
#define SYS_TOPIC_SET_APPCFG       "setAppConfig"


/* ���ڷ���APP */
#define UART_APP_NAME              "uartManager"
#define UART_TOPIC_MODECHG         "modeChange"
#define UART_TOPIC_UARTINFO        "uartInfo"  
#define UART_TOPIC_ONLYPARAM       "commParam"
#define UART_TOPIC_ONLYREG         "onlyReg"
#define UART_TOPIC_MONITORREG      "appRegister"
#define UART_TOPIC_DATA            "data"
#define UART_TOPIC_WORKMODE        "workMode"
#define UART_TOPIC_ABNORMAL        "abnormal"

/* ����APP */
#define BLE_APP_NAME               "btManager"
#define BLE_TOPOC_SETPARAM         "paramSet"
#define BLE_TOPOC_GETPARAM         "paramGet"
#define BLE_TOPOC_SETPORT          "portSet"
#define BLE_TOPOC_PORTNOTIFY       "portNotice"
#define BLE_TOPOC_SETDETECT        "detectSet"
#define BLE_TOPOC_CONNINFO         "connectInfo"
#define BLE_TOPOC_SENDDATA         "sendData"
#define BLE_TOPOC_FORWARD          "forward"
#define BLE_TOPOC_RECVNOTIFY       "frameNotice"

#define MESSAGE_APPNAME_LEN        48
#define MESSAGE_OPERINFO_LEN       16
#define MESSAGE_INFOPATH_LEN       96
#define MESSAGE_TIME_LEN           36

/*��������*/
#define IOT_APP_NAME              "iotManager"
#define TRDATA_APP_NAME            "MQTTTrData"
#define IOT_TOPIC_STATUS           "terminalStatus"
#define IOT_TOPIC_DATA             "terminalData"
#define IOT_TOPIC_CMD              "terminalCmd"

#define MESSAGE_APPNAME_LEN        48
#define MESSAGE_OPERINFO_LEN       16
#define MESSAGE_INFOPATH_LEN       96
#define MESSAGE_TIME_LEN           36

/* �յ�����Ϣ(��������)���������¸�ʽ */
typedef struct tagMessageJson
{
    char   sourapp[MESSAGE_APPNAME_LEN];
    char   destapp[MESSAGE_APPNAME_LEN];

    char   operat[MESSAGE_OPERINFO_LEN];   //��������
    char   infotype[MESSAGE_OPERINFO_LEN]; //��������

    char   infopath[MESSAGE_INFOPATH_LEN]; //�ӿ�����

    uint32 token;
    char   timestamp[MESSAGE_TIME_LEN];

    void  *pObject; /* JSON������� */
}MESSAGE_JSON_T;


#define MESSAGE_TYPE_A_XDR         0
#define MESSAGE_TYPE_JSON          1

/* MQTT��Ϣ��ʽ����Ӧ���ڴ��ͷŽӿ�appmsg_mqtt_free() */
typedef struct tagMessageMqtt
{
    uint8  msgtype;
    uint8  res[3];

    /* ����msgtype���� */
    union{
           MESSAGE_INFO_T tAXDR; //MESSAGE_TYPE_A_XDR
           MESSAGE_JSON_T tJson; //MESSAGE_TYPE_JSON
    };
}MESSAGE_MQTT_T;

/* ��̫�� */
#define NETWORK_TYPE_ETH            0
/* ���߹��� */
#define NETWORK_TYPE_WIRE_PUBLIC    1
/* ����ר�� */
#define NETWORK_TYPE_WIRE_PRIVATE   2

#define NETWORK_MODE_NULL           0
#define NETWORK_MODE_2G             1
#define NETWORK_MODE_3G             2
#define NETWORK_MODE_4G             3
#define NETWORK_MODE_5G             4

#ifdef __cplusplus
}
#endif

#endif /* __MESSAGE_H__ */
