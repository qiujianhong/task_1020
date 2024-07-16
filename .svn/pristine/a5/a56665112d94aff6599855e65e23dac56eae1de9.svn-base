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

#define EnterTurnDisp    0        //进入轮显
#define ExitTurnDisp    1        //退出轮显

#define LOGIN_APP_CNT_MAX    48        //最大app_gui 数量

#define MAX_POINT_LEN		20*160		//最大点阵长度

extern pthread_mutex_t        appGuiInfoLock;
extern BOOL g_abort_Trun;

/**********************************************************************
* @brief     ：APP给桌面发消息的回调函数
* @param[in] ：char *                   发送主题
               MESSAGE_INFO_T*         需要发送的消息
               int                     优先级
* @param[out]：   
* @return    ：
* @Create    : 
* @Date      ：2021-2-4
* @Update    :
**********************************************************************/
typedef int (*GUISendMqtt)(char *, MESSAGE_INFO_T*, int);

typedef struct _APP_REGIST_INFO
{
    char     appname[32];                //app名称
    char     shortname[32];
    char     iconFileName[128];            //位图文件位置
}APP_REGIST_INFO_T;

typedef struct _APP_GUI_INFO
{
    int                    loginAppCnt;                    //注册app实际数量
    uint32                loginID[LOGIN_APP_CNT_MAX];
    uint16                menuID[LOGIN_APP_CNT_MAX];        //菜单ID
    APP_REGIST_INFO_T    applogin[LOGIN_APP_CNT_MAX];
}__attribute__ ((packed))APP_GUI_INFO_T;                //App信息

typedef struct _STATUS_BAR_INFO
{
    uint32            disptime;    //显示时长，时长为0表示一直显示
    time_t            dispEnd;    //显示结束时间
    char            text[24];    //显示内容
}STATUS_BAR_INFO_T;

//弹出信息提示框
typedef struct _POP_MES_BOX
{    
    BOOL            is_pop;
    uint32            disptime;
    time_t            dispEnd;    //显示结束时间
    uint32          delayTime;
    char             text[512];
}POP_MES_BOX_T;

 typedef enum
{
    SIG_STRENGTH,    //信号强度，long_unsigned
    NET_FORMAT,        //网络制式，unsigned
    CON_STATUS,        //联机状态，bool
}REMOTE_COM_STATE_E;

typedef enum
{
    CHANNEL1,        	//通道1，远程通信状态
    CHANNEL2,        	//通道2，远程通信状态            
    ETH_CON_STATUS,    	//以太网联机            bool
    WARN,            	//告警        bool
    INFO_ID,        	//信息点        long-unsigned
    KEY_STATUS,        	//密钥状态        unsigned
    BLUE_STATUS,		//蓝牙状态
    RADIO_STATUS,		//电台状态
}TOOL_BAR_FIELD_E;

typedef struct
{
    uint16     signalStrengh;    //信号强度
    uint8     netFormat;        //网络制式
    bool    conStatus;        //联机状态
}REMOTE_COM_STATE_T;

typedef struct 
{
    REMOTE_COM_STATE_T     	channel1;          //通道1
    REMOTE_COM_STATE_T    	channel2;          //通道2
    bool                	ethConStatus;      //以太网联机
    bool                	warning;          //告警
    uint16                	infoID;           //信息点号
    uint8                	keyStatus;        //密钥状态
    uint8                	blueStatus;       //蓝牙状态
    uint8                	radioStatus;      //电台状态
}TOOL_BAR_INFO_T;

 typedef enum
{
    SCREEN_WHITE,        //显示全白
    SCREEN_BLACK,        //显示全黑
    SCREEN_POINT,        //显示点阵
    SCREEN_EXIT, 		 //屏显取消
}SCREEN_TYPE_E;

typedef struct
{
    SCREEN_TYPE_E     screenType;      //屏显控制类型
    uint8     pointBuf[MAX_POINT_LEN];        //点阵内容
}SCREEN_CTR_T;


typedef int (*PMSGTORAWOBJ)(char*,char*,MESSAGE_INFO_T*);

extern SCREEN_CTR_T	g_sceenCtr;				//屏显控制
void screen_ctr_ini(void);

void* app_gui_entry(void*);
int checkShowName(char* appname,char** showName);
int checkBitmap(char* appname,char** pbitmap);
int getAuthor(void);            //检查app前台权限
void openAuthor(void);            //打开app前台权限
void closeAuthor(void);            //关闭app前台权限
void turnAuthor(void);            //翻转app前台权限
int getLoginguiID(void);            //返回当前注册appID,为0表示未注册成功
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
