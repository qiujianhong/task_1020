#ifndef GUI_PRTL_H
#define GUI_PRTL_H

#include "miniguidef.h"
#include "main.h"

#ifdef __cplusplus
    extern "C" {
#endif

#include "gui_c_prtl.h"

#define     APP_NAME_MAX_LEN    32

extern APP_GUI_INFO_T         appGuiInfoSaved;
extern pthread_mutex_t        appGuiInfoLock;
extern uint32                liveAppID;        //当前活跃appId
extern STATUS_BAR_INFO_T     statusInfoFresh;
extern POP_MES_BOX_T         g_pop_mes_Fresh;
extern TOOL_BAR_INFO_T        toolInfoFresh;
extern char                    localAppName[APP_NAME_MAX_LEN];
extern BOOL                 TurnDisp;

typedef struct
{
    const char* appName;            //app名称
    const char* showName;            //显示名称
}APP_NAME_SHOW_T;

//extern char ICON_PATH[100];
extern int gui_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);
extern int appGUI_send_msg_to_gui(uint16 IID, uint16 IOP, void* dataIn, uint32 InLen);
extern uint16 gui_get_next_index();
extern void gui_next_index_init();
extern int GetAppName(char* appName);        //minigui中获取app名称,用于gdk.cfg路径配置
void set_gui_infonum(uint16 infonum);
extern BOOL app_is_online(uint32 appid);


#ifdef __cplusplus
}
#endif

int AddMenu(int nParentIndex, char* pCaption, BOOL bMenuAggregate, 
            DWORD dwMenuID, int nLeft1, int nLeft2, int nTop, int nInterval, int nMenuMaxCnt,
            BOOL bVisible,
            PLUG_STRUCT* pPluginStruct, int nPluginCnt);
BOOL RestoreConfigFromXml(char** appName);
BOOL ReadConfig(BOOL bAll);
void OnPaint();
BOOL Initialize(char** appName, BOOL show_oftendisp);


    
extern BOOL g_bAllMenu;

#endif