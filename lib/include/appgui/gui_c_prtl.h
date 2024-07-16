#ifndef GUI_C_PRTL_H
#define GUI_C_PRTL_H


#include "message.h"
#include <stdio.h>
//#include "dataType.h"
#include "baseCommon.h"
#include "lcd.h"
#include "message.h"

extern LCD_DEVICE_T* pGuiDev;
extern LCD_DEVICE_T* pAppDev;

#define EnterTurnDisp    0        //��������
#define ExitTurnDisp    1        //�˳�����

#define LOGIN_APP_CNT_MAX    48        //���app_gui ����

#define MAX_POINT_LEN		20*160		//�����󳤶�

extern pthread_mutex_t        appGuiInfoLock;
extern BOOL g_abort_Trun;

/**********************************************************************
* @brief     ��APP�����淢��Ϣ�Ļص�����
* @param[in] ��char *                   ��������
               MESSAGE_INFO_T*         ��Ҫ���͵���Ϣ
               int                     ���ȼ�
* @param[out]��   
* @return    ��
* @Create    : 
* @Date      ��2021-2-4
* @Update    :
**********************************************************************/
typedef int (*GUISendMqtt)(char *, MESSAGE_INFO_T*, int);

typedef struct _APP_REGIST_INFO
{
    char     appname[32];                //app����
    char     shortname[32];
    char     iconFileName[128];            //λͼ�ļ�λ��
}APP_REGIST_INFO_T;

typedef struct _APP_GUI_INFO
{
    int                    loginAppCnt;                    //ע��appʵ������
    uint32                loginID[LOGIN_APP_CNT_MAX];
    uint16                menuID[LOGIN_APP_CNT_MAX];        //�˵�ID
    APP_REGIST_INFO_T    applogin[LOGIN_APP_CNT_MAX];
}__attribute__ ((packed))APP_GUI_INFO_T;                //App��Ϣ

typedef struct _STATUS_BAR_INFO
{
    uint32            disptime;    //��ʾʱ����ʱ��Ϊ0��ʾһֱ��ʾ
    time_t            dispEnd;    //��ʾ����ʱ��
    char            text[24];    //��ʾ����
}STATUS_BAR_INFO_T;

//������Ϣ��ʾ��
typedef struct _POP_MES_BOX
{    
    BOOL            is_pop;
    uint32            disptime;
    time_t            dispEnd;    //��ʾ����ʱ��
    uint32          delayTime;
    char             text[512];
}POP_MES_BOX_T;

 typedef enum
{
    SIG_STRENGTH,    //�ź�ǿ�ȣ�long_unsigned
    NET_FORMAT,        //������ʽ��unsigned
    CON_STATUS,        //����״̬��bool
}REMOTE_COM_STATE_E;

typedef enum
{
    CHANNEL1,        	//ͨ��1��Զ��ͨ��״̬
    CHANNEL2,        	//ͨ��2��Զ��ͨ��״̬            
    ETH_CON_STATUS,    	//��̫������            bool
    WARN,            	//�澯        bool
    INFO_ID,        	//��Ϣ��        long-unsigned
    KEY_STATUS,        	//��Կ״̬        unsigned
    BLUE_STATUS,		//����״̬
    RADIO_STATUS,		//��̨״̬
}TOOL_BAR_FIELD_E;

typedef struct
{
    uint16     signalStrengh;    //�ź�ǿ��
    uint8     netFormat;        //������ʽ
    bool    conStatus;        //����״̬
}REMOTE_COM_STATE_T;

typedef struct 
{
    REMOTE_COM_STATE_T     	channel1;          //ͨ��1
    REMOTE_COM_STATE_T    	channel2;          //ͨ��2
    bool                	ethConStatus;      //��̫������
    bool                	warning;          //�澯
    uint16                	infoID;           //��Ϣ���
    uint8                	keyStatus;        //��Կ״̬
    uint8                	blueStatus;       //����״̬
    uint8                	radioStatus;      //��̨״̬
}TOOL_BAR_INFO_T;

 typedef enum
{
    SCREEN_WHITE,        //��ʾȫ��
    SCREEN_BLACK,        //��ʾȫ��
    SCREEN_POINT,        //��ʾ����
    SCREEN_EXIT, 		 //����ȡ��
}SCREEN_TYPE_E;

typedef struct
{
    SCREEN_TYPE_E     screenType;      //���Կ�������
    uint8     pointBuf[MAX_POINT_LEN];        //��������
}SCREEN_CTR_T;


typedef int (*PMSGTORAWOBJ)(char*,char*,MESSAGE_INFO_T*);

extern SCREEN_CTR_T	g_sceenCtr;				//���Կ���
void screen_ctr_ini(void);

void* app_gui_entry(void*);
int checkShowName(char* appname,char** showName);
int checkBitmap(char* appname,char** pbitmap);
int getAuthor(void);            //���appǰ̨Ȩ��
void openAuthor(void);            //��appǰ̨Ȩ��
void closeAuthor(void);            //�ر�appǰ̨Ȩ��
void turnAuthor(void);            //��תappǰ̨Ȩ��
int getLoginguiID(void);            //���ص�ǰע��appID,Ϊ0��ʾδע��ɹ�
int initlize_desk(const char* appname, GUISendMqtt pSendFunc);
int initlize_desk_func(const char* appname, PMSGTORAWOBJ fp);
int desktop_msg_handle(MESSAGE_INFO_T *pInMsg);
int Is_gui_app_exist(char* appName,uint32* pLoginID);
int app_login_info_deal(uint8 *pBuf,uint32 bufLen, uint32* pdataOut );
int sendback_msg_build(MESSAGE_INFO_T *pInMsg, void * pLoadBuf, uint16 Loadlen);
int desktop_app_login(MESSAGE_INFO_T *pInMsg);
int desktop_app_exit(MESSAGE_INFO_T *pInMsg);
int desktop_fresh_state(MESSAGE_INFO_T *pInMsg);
int desktop_fresh_tool(MESSAGE_INFO_T *pInMsg);
int desktop_pop_msg(MESSAGE_INFO_T *pInMsg);
int desktop_cancel_pop(MESSAGE_INFO_T *pInMsg);
int desktop_exit_turndisp(MESSAGE_INFO_T *pInMsg);
int desktop_live_app(MESSAGE_INFO_T *pInMsg);
int turn_disp_inform_event(char* pAppName,uint8 InOrOut);
int turn_disp_inform_events(uint32 AppID,uint8 InOrOut);
int app_rise_inform_event(char* pAppName);
int appGUI_recv_back_handle(MESSAGE_INFO_T *pInMsg);
int appGUI_send_req_handle(uint16 IID, uint16 IOP, void* dataIn, uint32 InLen);
int appGUI_req_reg(void* dataIn, uint32 InLen);
int appGUI_req_exit(void);
int desktop_key_pop_cancel(void);

int read_app_info(void);
int save_app_info(void);
void load_app_icon(BOOL show_oftendisp);
void RegisterDev(LCD_DEVICE_T* pAppdev,LCD_DEVICE_T* pGuidev);

#endif
