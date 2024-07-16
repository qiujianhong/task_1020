
#include "gui_prtl.h"
//#include "out_msg.h"
#include "framecomm.h"
//#include "miniguidef.h"
#include <time.h>
#include "appmsg_split.h"
#include "appmsg_struct.h"
#include "XMLite.h"
#include <pthread.h>
#include "mqtt_client.h"

#if (defined(PRODUCT_CCU) && defined(AREA_CHONGQING))
#include "dictionary.h"
#include "iniparser.h"
#endif

extern int pMsgtoRawObj(char* source, char* dest, MESSAGE_INFO_T* pMsgInfo );
static PMSGTORAWOBJ pfuncSend = NULL;

extern MQTT_CLIENT_T *g_GuiMqttClient;

APP_GUI_INFO_T      appGuiInfoSaved;
pthread_mutex_t     appGuiInfoLock;
uint32              liveAppID = 0;      //��ǰ��ԾappId
STATUS_BAR_INFO_T   statusInfoFresh;
POP_MES_BOX_T       g_pop_mes_Fresh;
TOOL_BAR_INFO_T     toolInfoFresh;
SCREEN_CTR_T        g_sceenCtr;             //���Կ���
uint32              loginguiID = 0;         //appGuiע�ᵽ����gui��ID
BOOL                TurnDisp = 0;           //�Ƿ�׼�����Ե�״̬
BOOL                g_abort_Trun = FALSE;   //�˳�����
char                localAppName[APP_NAME_MAX_LEN] = {0};       
const char*         deskGui = "M-desktopGui";
extern              int g_nCurMenuCnt;
static      char    g_AppName[128] ={0};
static int          appAuthority    = 0;        //appȨ��
const char*         appInfoPath = "/tmp/deskgui";
const char*         appInfoFile = "appGuiInfo";
char*               RegistAppName[LOGIN_APP_CNT_MAX] = {0};
uint8               RegistAppCnt = 0;
char                xmlPath[100] = {0};

//λͼ�ļ�ͷ
uint8              g_icon_header[] = { 0x42, 0x4d, 0xbe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x28, 0x00,
                                       0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00 };

LCD_DEVICE_T* pAppDev= NULL;
LCD_DEVICE_T* pGuiDev= NULL;

//����APP����ʱ���쳣��������ƣ����⴦��
uint8 blkFlag = 0;


APP_NAME_SHOW_T appShowName[] = {
    {"mapManager"   ,"ģ��"},
    {"smiOS"        ,"ϵͳ"},
    {"wirelessDCM"  ,"����"},
    {"ccoRouter"    ,"����ͨ��"},
    {"pdAmr"        ,"���"},
    {"stAmr"        ,"ר��"},
    {"puAmr"        ,"����"},
    {"lcMonitor"    ,"��·"},
};

GUISendMqtt g_GuiSendMsgFunc = NULL;

int  checkRegistAppCnt(void)
{
    return RegistAppCnt;
}
int checkShowName(char* appname,char** showName)
{
    int i;
//  uint8 size = sizeof(appShowName)/sizeof(APP_NAME_SHOW_T);
    for(i = 0; i < appGuiInfoSaved.loginAppCnt; i++)
    {
        if(strncmp(appname,appGuiInfoSaved.applogin[i].appname,APP_NAME_MAX_LEN) == 0)
        {
            *showName = appGuiInfoSaved.applogin[i].shortname;
            return 1;
        }
    }
    *showName = appname;
    return 0;
}

int checkBitmap(char* appname,char** pbitmap)
{
    int i;
    int ret = -1;
    pthread_mutex_lock(&appGuiInfoLock);
    for(i = 0; i < appGuiInfoSaved.loginAppCnt; i++)
    {
        if(strncmp(appname,appGuiInfoSaved.applogin[i].appname,APP_NAME_MAX_LEN) == 0)
        {
            *pbitmap = appGuiInfoSaved.applogin[i].iconFileName;
            ret = 1;
            break;
        }
    }
    pthread_mutex_unlock(&appGuiInfoLock);
    return ret;
}

int getAuthor(void)         //���Ȩ��
{
    return appAuthority;
}

int getLoginguiID(void)         //���ص�ǰע��appID,Ϊ0��ʾδע��ɹ�
{
    return loginguiID;
}
void openAuthor(void)           //��Ȩ��
{
    appAuthority = 1;
}

void closeAuthor(void)          //�ر�Ȩ��
{
    appAuthority = 0;
}

void turnAuthor(void)           //��תȨ��
{
    appAuthority = 1 - appAuthority;
}

void RegisterDev(LCD_DEVICE_T* pAppdev,LCD_DEVICE_T* pGuidev)
{
    if(pAppdev != NULL)
    {
        pAppDev = pAppdev;      //appdevָ�봫��
        if(loginguiID != 0)
        {
            pAppDev->app_regist_id = loginguiID;
            GUI_FMT_DEBUG("RegisterDev: pAppDev->app_regist_id = %d \n",pAppDev->app_regist_id);
        }
    }
        
    if(pGuidev != NULL)
        pGuiDev = pGuidev;      //guidevָ�봫��
}

int initlize_desk_func(const char* appname, PMSGTORAWOBJ fp)
{
    char tempApp[20] = {0};
    pthread_mutex_lock(&appGuiInfoLock);
    memset(&appGuiInfoSaved,0,sizeof(APP_GUI_INFO_T));
    pthread_mutex_unlock(&appGuiInfoLock);
    memset(&statusInfoFresh,0,sizeof(STATUS_BAR_INFO_T));
    memset(&g_pop_mes_Fresh,0,sizeof(POP_MES_BOX_T));
    memset(&toolInfoFresh,0,sizeof(TOOL_BAR_INFO_T));
    loginguiID = 0;
    if(fp != NULL)
    {
        pfuncSend = fp;
    }
    else
    {
        pfuncSend = NULL;
    }
    
    if(appname == NULL)
    {
        return -1;
    }
    sprintf(g_AppName,"%s%s","M-",appname);
    sprintf(tempApp,appname,strlen(appname));
    GetAppName(tempApp);
    strncpy(localAppName,tempApp,APP_NAME_MAX_LEN);
    sprintf(xmlPath,"%s%s%s%s%s","/usr/local/extapps/",tempApp,"/bin/cfg/fkgaconfig_",tempApp,".xml");
    GUI_FMT_DEBUG("the xmlPath is: %s\n",xmlPath);
    return 1;
}

/**********************************************************************
* @brief     ��APP��ʾ��ʼ��
* @param[in] ��const char* appname          APP��������puAmr
               GUISendMqtt pSendFunc       APP�����淢��Ϣ�Ļص�����
* @param[out]��   
* @return    ��1 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2021-2-4
* @Update    :
**********************************************************************/
int initlize_desk(const char* appname, GUISendMqtt pSendFunc)           
{
    char tempApp[20] = {0};
    pthread_mutex_lock(&appGuiInfoLock);
    memset(&appGuiInfoSaved,0,sizeof(APP_GUI_INFO_T));
    pthread_mutex_unlock(&appGuiInfoLock);
    memset(&statusInfoFresh,0,sizeof(STATUS_BAR_INFO_T));
    memset(&g_pop_mes_Fresh,0,sizeof(POP_MES_BOX_T));
    memset(&toolInfoFresh,0,sizeof(TOOL_BAR_INFO_T));
    loginguiID = 0;
    if(pSendFunc != NULL)
    {
        g_GuiSendMsgFunc = pSendFunc;
        if(appname != NULL)
        {
            gui_next_index_init();
        }
    }
    
    if(appname == NULL)
    {
        sprintf(g_AppName,"%s",deskGui);
        return -1;
    }
    sprintf(g_AppName,"%s%s","M-",appname);
    sprintf(tempApp,appname,strlen(appname));
    GetAppName(tempApp);
    strncpy(localAppName,tempApp,APP_NAME_MAX_LEN);
    sprintf(xmlPath,"%s%s%s%s%s","/usr/local/extapps/",tempApp,"/bin/cfg/fkgaconfig_",tempApp,".xml");
    GUI_FMT_DEBUG("the xmlPath is: %s\n",xmlPath);
    return 1;
}

/**********************************************************************
* @name      :  screen_ctr_ini
* @brief     �� ���Կ��Ƴ�ʼ����Ĭ�ϲ����Կ���
* @param[in] �� 
* @param[out]��
* @return    ��
* @Create    : ÷����
* @Date      ��2020-12-11
* @Update    :
**********************************************************************/
void screen_ctr_ini(void)
{
    g_sceenCtr.screenType = SCREEN_EXIT;
}

int desktop_screen_ctr(MESSAGE_INFO_T *pInMsg)
{
    int ret = 0;
    uint8 offset = 0;
    uint16 msglen = 0;
    int i;
    
    BOOLEAN result = 1;
    if(pInMsg->IID != MESSAGE_IID_DESKGUI || pInMsg->IOP != DESKGUI_IOP_SCREEN_CTR)
    {
        return -1;
    }

    memset(&g_sceenCtr,0,sizeof(SCREEN_CTR_T));

    g_sceenCtr.screenType = (SCREEN_TYPE_E)pInMsg->playload[0];
    switch (g_sceenCtr.screenType)
    {
        case SCREEN_WHITE:
            for(i = 0; i < MAX_POINT_LEN; i++)
            {
                g_sceenCtr.pointBuf[i] = 0;
            }
            break;
        case SCREEN_BLACK:
            for(i = 0; i < MAX_POINT_LEN; i++)
            {
                g_sceenCtr.pointBuf[i] = 0xff;
            }
            break;
        case SCREEN_POINT:
            msglen = appmsg_get_len_offset(&pInMsg->playload[1], &offset);
            if(msglen > MAX_POINT_LEN)
            {
                msglen = MAX_POINT_LEN;
            
}
            memcpy(g_sceenCtr.pointBuf,&pInMsg->playload[offset+1],msglen);
            break;
        case SCREEN_EXIT:
            break;
    }

    ret = sendback_msg_build(pInMsg,&result,1);
    return ret;
}

int desktop_msg_handle(MESSAGE_INFO_T *pInMsg)
{
    int ret = 0;
    if(pInMsg->IID != MESSAGE_IID_DESKGUI)
    {
        return -1;
    }

    switch(pInMsg->IOP)
    {
#ifdef PRODUCT_CCU
        case DESKGUI_IOP_APP_REG:       //appע��
      #ifndef AREA_CHONGQING
            return desktop_app_login(pInMsg);
      #else
            break;
       #endif
#else
        case DESKGUI_IOP_APP_REG:       //appע��
            return desktop_app_login(pInMsg);
            break;
#endif

        case DESKGUI_IOP_APP_UNREG:     //app�˳�
            return desktop_app_exit(pInMsg);
            break;

        case DESKGUI_IOP_FRESH_STATE:   //ˢ״̬��
            return desktop_fresh_state(pInMsg);
            break;

        case DESKGUI_IOP_FRESH_TOOL:    //ˢ������
            return desktop_fresh_tool(pInMsg);
            break;

        case DESKGUI_IOP_PUSH_HINT:     //������Ϣ��ʾ��
            return desktop_pop_msg(pInMsg);
            break;

        case DESKGUI_IOP_POP_HINT:      //ȡ����Ϣ��ʾ��
            return desktop_cancel_pop(pInMsg);
            break;

        case DESKGUI_IOP_EXIT_DISPLAY:  //app�˳�����
            return desktop_exit_turndisp(pInMsg);
            break;

        case DESKGUI_IOP_LIVE_APP:      //��ǰ��Ծapp
            return desktop_live_app(pInMsg);
            break;

        case DESKGUI_IOP_SCREEN_CTR:        //���Կ���
            return desktop_screen_ctr(pInMsg);
            break;

        default: 
            return 0x3333;
    }
    return ret;
}

/*
ret : > 0   ������
      = 0   ����
      < 0   �Ƿ�
*/
int Is_gui_app_exist(char* appName,uint32* pLoginID)
{
    int ret = 1;
    if(appName == NULL)
        return -1;
    APP_GUI_INFO_T* pAppLoginInfo = NULL;
    int i;
    pthread_mutex_lock(&appGuiInfoLock);
    pAppLoginInfo = &appGuiInfoSaved;
    for(i=0; i<pAppLoginInfo->loginAppCnt;i++)
    {
        if(strncmp(appName,pAppLoginInfo->applogin[i].appname,APP_NAME_MAX_LEN) == 0)
        {
            *pLoginID = pAppLoginInfo->loginID[i];
            ret = 0;
            break;
        }
    }
    pthread_mutex_unlock(&appGuiInfoLock);
    return ret;
}

int read_app_info(void)
{
    FILE*   fd = NULL;
    char    filepath[100] = {0};
    APP_GUI_INFO_T *pAppGuiInfo = &appGuiInfoSaved;
    memset(&appGuiInfoSaved, 0, sizeof(APP_GUI_INFO_T));
    if(access(appInfoPath,F_OK) < 0)
    {
        GUI_FMT_DEBUG("%s not exists!\n",appInfoPath);
        return -1;
    }
    sprintf(filepath,"%s/%s",appInfoPath,appInfoFile);

    fd = fopen(filepath,"rb");
    if(!fd)
    {
        GUI_FMT_DEBUG("%s not exists\n",filepath);
        return -1;
    }
    fread((char*)pAppGuiInfo,1,sizeof(APP_GUI_INFO_T),fd);
    fclose(fd);
    return 0;
}

void load_app_icon(BOOL show_oftendisp)
{
    read_app_info();
    RegistAppCnt = appGuiInfoSaved.loginAppCnt;
    int i;
    
    if(RegistAppCnt == 0)
    {
        return;
    }
    for(i = 0; i < RegistAppCnt; i++)
    {
        RegistAppName[i] = appGuiInfoSaved.applogin[i].appname;
    }
    Initialize(RegistAppName, show_oftendisp);

}

int save_app_info(void)
{
    FILE*   fd = NULL;
    char    filepath[100] = {0};
    APP_GUI_INFO_T *pAppGuiInfo = &appGuiInfoSaved;
    
    if(access(appInfoPath,F_OK) < 0)
    {
        mkdir(appInfoPath,0766);
    }
    sprintf(filepath,"%s/%s",appInfoPath,appInfoFile);

    fd = fopen(filepath,"wb");
    if(!fd)
    {
        return -1;
    }
    fwrite((char*)pAppGuiInfo,1,sizeof(APP_GUI_INFO_T),fd);
    fclose(fd);
    return 0;
}

BOOL get_app_name(uint32 appid, char* app_name, char* appshortname)
{
    BOOL ret = FALSE;
    if(app_name == NULL)
        return ret;
    
    APP_GUI_INFO_T* pAppLoginInfo = NULL;
    int i;
    pthread_mutex_lock(&appGuiInfoLock);
    pAppLoginInfo = &appGuiInfoSaved;
    for(i=0; i<pAppLoginInfo->loginAppCnt;i++)
    {
        if(pAppLoginInfo->loginID[i] == appid)
        {
            memcpy(app_name,pAppLoginInfo->applogin[i].appname,strlen(pAppLoginInfo->applogin[i].appname));
            memcpy(appshortname,pAppLoginInfo->applogin[i].shortname,strlen(pAppLoginInfo->applogin[i].shortname));
            ret = TRUE;
            break;
        }
    }
    pthread_mutex_unlock(&appGuiInfoLock);
    return ret;
}

int app_is_online_bycmd(char* pcmd)
{
    char pstatus[10] = {0};
    
    FILE* pipe = popen(pcmd, "r");  
    if (!pipe)  
    {
        pipe = popen(pcmd, "r");
    }
    if (!pipe)
    {
        GUI_FMT_DEBUG("��ѯAPP�Ƿ�����ʧ��cmd: %s,\n", pcmd);
        return TRUE;
    }
    
    fgets(pstatus, 10, pipe);
    pclose(pipe);
    
    return atoi(pstatus);
        
}


BOOL app_is_online(uint32 appid)
{
    BOOL ret = FALSE;
    char app_name[APP_NAME_MAX_LEN] = {0};
    char appshortname[APP_NAME_MAX_LEN] = {0};
    char pstr_cmd[128] = {0};
    int onLine = 0;
    
    if(get_app_name(appid, app_name, appshortname) == FALSE)
    {
        GUI_FMT_DEBUG("��ȡAPP����ʧ��\n");
        return ret;
    }

    sprintf(pstr_cmd, "pidof %s | wc -l ", app_name);

    onLine = app_is_online_bycmd(pstr_cmd);  
//    if (onLine <= 0)  
//    {
//      sprintf(pstr_cmd, "ps -ef |grep %s |grep -v grep | wc -l ", app_name);
//        onLine = app_is_online_bycmd(pstr_cmd);  
//    }

    if(onLine >= 1)
    {
        ret = TRUE;
    }
    else
    {
        memset(&statusInfoFresh,0,sizeof(STATUS_BAR_INFO_T));
        statusInfoFresh.dispEnd = time(NULL) + 5;

        sprintf(statusInfoFresh.text, "%s�����ߣ�", appshortname);
        GUI_FMT_DEBUG("%s cmd %s\n", statusInfoFresh.text, pstr_cmd);
    }
    
    return ret;
        
}

int app_login_info_deal(uint8 *pBuf,uint32 bufLen, uint32* pdataOut )
{
    APP_REGIST_INFO_T   appLoginInfo;
    uint8   nameLen = 0;
    uint8   shortLen = 0;
    int     iconFileLen = 0;
    uint8*  pIconfile = NULL;
    uint8   iconLenCnt = 0;
    char    iconName[128] = {0};
    uint32  offset = 0;
    uint8   curIndex = 0;
    int     checkRet = -1;
    uint32  RegisteredID = 0;
    int     ret          = 0;
    
    if(pBuf == NULL || pdataOut == NULL)
    {
        return -1;
    }
    memset(&appLoginInfo,0,sizeof(APP_REGIST_INFO_T));
    nameLen = pBuf[offset++];
    if(nameLen == 0)
    {
        GUI_FMT_DEBUG("app���Ʋ���Ϊ��,!\n");
        return -1;
    }
    if(nameLen > 32)
    {
        nameLen = 32;
    }
    memcpy(appLoginInfo.appname,&pBuf[offset],nameLen);
    offset += nameLen;
    shortLen = pBuf[offset++];
    if(shortLen == 0)
    {
        GUI_FMT_DEBUG("app��Ʋ���Ϊ��,!\n");
        return -1;
    }
    if(shortLen > 32)
    {
        shortLen = 32;
    }
    memcpy(appLoginInfo.shortname,&pBuf[offset],shortLen);
    offset += shortLen;
    if((pBuf[offset] & 0x80) == 0)
    {
        iconFileLen = pBuf[offset++];
    }
    else
    {
        iconLenCnt = (pBuf[offset] & 0x0f);
        offset++;
        appmsg_memcpy_r((char*)&iconFileLen,(char*)&pBuf[offset],iconLenCnt);
        offset += iconLenCnt;
    }
    GUI_FMT_TRACE("[0007]app login to host_gui: iconlen = %d\n",iconFileLen);

    sprintf(iconName,"%sapp_%s.bmp",ICON_PATH,appLoginInfo.appname);
    GUI_FMT_DEBUG("ע���appname[%s],shortname[%s],iconfilename[%s]\n",
            appLoginInfo.appname, appLoginInfo.shortname, iconName);

    //����APP����ʱ���쳣��������ƣ����⴦��
    if(0 == strcmp(appLoginInfo.shortname, "����ͨ��") && g_guiMain.bLighton == FALSE)
    {
        blkFlag = 1;
        GUI_FMT_TRACE("����ͨ�� ע��\n");
    }
    
    checkRet = Is_gui_app_exist(appLoginInfo.appname,&RegisteredID);
    if( checkRet == 0)
    {
        GUI_FMT_DEBUG(" %s app��ע��!\n",appLoginInfo.appname);
        if(RegisteredID == liveAppID)
        {
            openAuthor();               //���app�ҵ���������ע�ᣬ�����ջ�ǰ̨Ȩ��
            if(pGuiDev != NULL)
            {
                ret = pGuiDev->lcd_active_id_set(pGuiDev,0);
                if(ret != 0)
                {
                    GUI_FMT_DEBUG("lcd_active_id_set:activeID = 0 ret %d\n", ret);
                    ret = pGuiDev->lcd_active_id_set(pGuiDev,0);
                }
                GUI_FMT_DEBUG("����gui�ջ�Ȩ�ޣ�set activeID=0 ret %d\n", ret);
            }
                
            liveAppID = 0;
        }
        *pdataOut = RegisteredID;
        return 0;
    }
    else if(checkRet < 0)
    {
        GUI_FMT_DEBUG(" %s app ���ƷǷ�!\n",appLoginInfo.appname);
        return -1;
    }
    
    FILE *pBmp = fopen(iconName,"w");
    if(!pBmp)
    {
        GUI_FMT_DEBUG("[0007]bmp file fopen failed\n");
        return -1;
    }
    
    int tmpCnt = (int)fwrite(g_icon_header,1,0x3e,pBmp);        //λͼ�ļ�ͷ
    
    pIconfile = &pBuf[offset];
    tmpCnt += (int)fwrite(pIconfile,1,iconFileLen,pBmp);        //����λͼ�ļ�
    fclose(pBmp);
    if(tmpCnt != iconFileLen+0x3e)
    {
        GUI_FMT_DEBUG("[0007]iconfile fwrite failed tmpCnt = %d\n",tmpCnt);
        return -1;
    }
    //����ע����Ϣ
    pthread_mutex_lock(&appGuiInfoLock);
    curIndex = appGuiInfoSaved.loginAppCnt;
    memcpy(appGuiInfoSaved.applogin[curIndex].appname,appLoginInfo.appname,nameLen);
    memcpy(appGuiInfoSaved.applogin[curIndex].shortname,appLoginInfo.shortname,shortLen);
    memcpy(appGuiInfoSaved.applogin[curIndex].iconFileName,iconName,strlen(iconName));
    appGuiInfoSaved.loginID[curIndex] = appGuiInfoSaved.loginAppCnt+1;
    *pdataOut = appGuiInfoSaved.loginID[curIndex];
    appGuiInfoSaved.menuID[curIndex] = appGuiInfoSaved.loginID[curIndex] -1;
    printf("[0007]:appGuiInfoSaved.menuID[%d] = %d,g_nCurMenuCnt=%d \n",
                        curIndex,appGuiInfoSaved.menuID[curIndex],g_nCurMenuCnt);
    appGuiInfoSaved.loginAppCnt++;
    g_nCurMenuCnt = appGuiInfoSaved.loginAppCnt;
    RegistAppCnt = appGuiInfoSaved.loginAppCnt;
    int i;
    for(i = 0; i < RegistAppCnt; i++)
    {
        RegistAppName[i] = appGuiInfoSaved.applogin[i].appname;
        GUI_FMT_TRACE("app_login_info_deal: RegistAppName[%d] = %s\n",i,RegistAppName[i]);
    }
    
    GUI_FMT_TRACE("[0007]app_gui register info: appname[%s],shortname[%s],iconfilename[%s],loginID[%d],loginAppCnt[%d]\n",
                    appGuiInfoSaved.applogin[curIndex].appname,
                    appGuiInfoSaved.applogin[curIndex].shortname,
                    appGuiInfoSaved.applogin[curIndex].iconFileName,
                    appGuiInfoSaved.loginID[curIndex],appGuiInfoSaved.loginAppCnt);
    save_app_info();
    pthread_mutex_unlock(&appGuiInfoLock);
    //ˢ�����棬����app
    //AddMenu(-1, appLoginInfo.appname, FALSE, 0, 0, 0, 0, 0, 0, 1,NULL, 1);
    //WriteXML(appLoginInfo.appname);
    Initialize(RegistAppName, TRUE);
    

    return 1;
}

int sendback_msg_build(MESSAGE_INFO_T *pInMsg, void * pLoadBuf, uint16 Loadlen)
{
    int len = 0;
    int ret = 0;
    MESSAGE_INFO_T * pMsgInfo = NULL;
    char topic[256] = {0};
    
    len = sizeof(MESSAGE_INFO_T) + Loadlen;
    pMsgInfo = (MESSAGE_INFO_T*)malloc(len);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_DEBUG("[0007]%s malloc failed %d\n",__FUNCTION__,len);
        return -1;
    }
    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 0;
    pMsgInfo->index = pInMsg->index;
    pMsgInfo->priority = pInMsg->priority;
    pMsgInfo->label = pInMsg->label;
    pMsgInfo->IID = pInMsg->IID;
    pMsgInfo->IOP = pInMsg->IOP;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr , g_AppName, pMsgInfo->sourlen);
    pMsgInfo->destlen = pInMsg->sourlen;
    memcpy(pMsgInfo->destaddr,pInMsg->souraddr,pMsgInfo->destlen);
    pMsgInfo->msglen = Loadlen;
    memcpy(pMsgInfo->playload, (char*)pLoadBuf, Loadlen);
    sprintf(topic,"%s/%s",g_AppName,pMsgInfo->destaddr);
    if(memcmp(g_AppName,"M-smiOS",sizeof("M-smiOS")) == 0 && pfuncSend != NULL)
    {
        ret = pfuncSend(g_AppName, pMsgInfo->destaddr, pMsgInfo );
    }
    else
    {
        ret = g_GuiSendMsgFunc(topic, pMsgInfo, 0);
    }
    free(pMsgInfo);
    return ret;
}

//appע�����
int desktop_app_login(MESSAGE_INFO_T *pInMsg)
{
    int ret = 0;
    uint32  appID = 0;
    uint8   dataBuf[4] = {0};

    GUI_FMT_DEBUG("[0007]desktop_app_login!\n");
    if(pInMsg->IID != MESSAGE_IID_DESKGUI || pInMsg->IOP != DESKGUI_IOP_APP_REG)
    {
        return -1;
    }


    if(app_login_info_deal(pInMsg->playload,pInMsg->msglen,&appID)<0)
    {
        GUI_FMT_DEBUG("[0007]app login info handle failed!\n");
        return -1;
    }
     


    appmsg_memcpy_r(dataBuf,(char*)&appID,4);
    
    ret = sendback_msg_build(pInMsg,dataBuf,4);
    
    return ret;

}

//app�˳��������
int desktop_app_exit(MESSAGE_INFO_T *pInMsg)
{
    int     ret = 0;
    BOOLEAN result = 0;

    uint32  appID = 0;
    if(pInMsg->IID != MESSAGE_IID_DESKGUI || pInMsg->IOP != DESKGUI_IOP_APP_UNREG)
    {
        return -1;
    }

    if(pInMsg->msglen != sizeof(uint32))
    {
        GUI_FMT_DEBUG("[0007]app�˳����泤�� %d ����!\n",pInMsg->msglen);
        return -1;
    }

    appmsg_memcpy_r((char*)&appID,pInMsg->playload,sizeof(uint32));

    if(appID == liveAppID)
    {
        result = 1;
        liveAppID = 0;
        openAuthor();
        if(pGuiDev != NULL)
        {
            ret = pGuiDev->lcd_active_id_set(pGuiDev,0);
            if(ret != 0)
            {
                GUI_FMT_DEBUG("lcd_active_id_set:activeID = 0 ret %d\n", ret);
                ret = pGuiDev->lcd_active_id_set(pGuiDev,0);
            }
            GUI_FMT_DEBUG("desktop_app_exit:activeID = 0 ret %d\n", ret);
        }

        //�г���APP�˳����Է��˳�APP�����������
        g_abort_Trun = TRUE;
        
//      g_guiMain.nLightonTime = 0;
//        g_guiMain.bLighton = TRUE;
//      lighton_lcd();  
        GUI_FMT_DEBUG("[0007]--------desktop_app_exit:appID[%d] deskgui get author������\n\n" ,appID);
    }
    else
    {
        GUI_FMT_DEBUG("[0007]app exit failed,appID[%d] is not the live app[%d]\n",appID,liveAppID);
        result = 0;
    }

    ret = sendback_msg_build(pInMsg,&result,1);
    
    return ret;
    
}

#if (defined(PRODUCT_CCU) && defined(AREA_CHONGQING))

GPRS_DATA_INFO rSignalInfo;
static int signalFlag = 0;
static uint16 maxTime = 0;
static uint16 minTime = 0;

int write_signal_cfg(const char *filename/*in*/)
{
    
    FILE *fp;
    char buf[128]; // ����ÿ�в�����1024�ֽ�, ����������ڴ�С
 
    if (!(fp = fopen(filename, "w"))) 
    { // ������д��ʽ���ļ�.
        printf("���ļ������ļ�%s ʧ�� \n", filename );
        return -1;
    }

    sprintf(buf, "[SIGNAL_INFO]\n");
    fwrite(buf, strlen(buf), 1, fp);

    sprintf(buf, "MAX_SIG=%hu\n", rSignalInfo.maxSig);
    fwrite(buf, strlen(buf), 1, fp);
    sprintf(buf, "MIN_SIG=%hu\n", rSignalInfo.minSig );
    fwrite(buf, strlen(buf), 1, fp);

    sprintf(buf, "MAX_TIME=%hu\n", maxTime);
    fwrite(buf, strlen(buf), 1, fp);
    sprintf(buf, "MIN_TIME=%hu\n", minTime);
    fwrite(buf, strlen(buf), 1, fp);

    fclose(fp);
    return 0;
}

/**********************************************************************
* @name      : appmsg_update_signalinfo_split
* @brief     �������ź�ǿ����Ϣ
* @param[in] ��uint16 signalStrengh �ź�ǿ��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2023-1-29
* @Update    :
**********************************************************************/
int appmsg_update_signalinfo_split(uint16 signalStrengh)
{
    time_t  curtime;
    OOP_DATETIME_S_T curTime = {0};
    curtime = time(NULL);
    DT_Time2DateTimeShort(curtime, &curTime);
    if ((curTime.minute == 0) || (signalFlag == 0)){
        int update = 0;
        memset(&rSignalInfo, 0xff, sizeof(GPRS_DATA_INFO));
        dictionary *ini = NULL;
        ini = iniparser_load("/data/app/desktopGui/data/SignalInfo.ini");
        if(ini!=NULL)
        {
            rSignalInfo.maxSig = iniparser_getint(ini,"SIGNAL_INFO:MAX_SIG",0xFFFF);
            rSignalInfo.minSig = iniparser_getint(ini,"SIGNAL_INFO:MIN_SIG",0xFFFF);
            maxTime = iniparser_getint(ini,"SIGNAL_INFO:MAX_TIME",0xFFFF);
            minTime = iniparser_getint(ini,"SIGNAL_INFO:MIN_TIME",0xFFFF);
            iniparser_freedict(ini);
        }
        rSignalInfo.maxTime.hour = maxTime/60;
        rSignalInfo.maxTime.minute = maxTime%60;
        rSignalInfo.minTime.hour = minTime/60;
        rSignalInfo.minTime.minute = minTime%60;
      
        if (rSignalInfo.maxSig == 0xFFFF)
        {
            rSignalInfo.maxTime = curTime;
            rSignalInfo.minTime = curTime;
            maxTime = rSignalInfo.maxTime.hour*60 + rSignalInfo.maxTime.minute;
            minTime = rSignalInfo.minTime.hour*60 + rSignalInfo.minTime.minute;
            rSignalInfo.maxSig = signalStrengh;
            rSignalInfo.minSig = signalStrengh;
            write_signal_cfg("/data/app/desktopGui/data/SignalInfo.ini");
        }
        else if((curTime.minute == 0) || (rSignalInfo.maxSig == 0 && signalStrengh != 0))//ÿСʱˢ��һ��-
        {
            signalFlag = 1;
            if(signalStrengh > rSignalInfo.maxSig)
            {
                rSignalInfo.maxSig = signalStrengh;
                rSignalInfo.maxTime = curTime;
                maxTime = rSignalInfo.maxTime.hour*60 + rSignalInfo.maxTime.minute;
                update = 1;
            }
            if(signalStrengh < rSignalInfo.minSig)
            {
                rSignalInfo.minSig = signalStrengh;
                rSignalInfo.minTime = curTime;
                minTime = rSignalInfo.minTime.hour*60 + rSignalInfo.minTime.minute;
                update = 1;
            }
            if (update == 1)
            {
                write_signal_cfg("/data/app/desktopGui/data/SignalInfo.ini");
            }
        }
    }
    return 0;
}
#endif
int desktop_fresh_state(MESSAGE_INFO_T *pInMsg)
{
    int ret = 0;
    int offset = 0;
    BOOLEAN result = 0;
    uint32 disptime = 0;        //��ʾʱ��
    char dispText[24] = {0};    //��ʾ����
    uint16 textLen = 0; 
    if(pInMsg->IID != MESSAGE_IID_DESKGUI || pInMsg->IOP != DESKGUI_IOP_FRESH_STATE)
    {
        return -1;
    }
    appmsg_memcpy_r((uint8*)&disptime,&pInMsg->playload[offset],sizeof(uint32));
    offset += sizeof(uint32);
    textLen = pInMsg->playload[offset++];
    
    if(textLen>24)
    {
        textLen = 24;
    }
        
    strncpy(dispText,(char*)&pInMsg->playload[offset],textLen);
    offset += textLen;
    GUI_FMT_TRACE("[0007] status bar info :text :%s , display time :%d\n", dispText,disptime);
    //״̬������ʾ��������resultֵ
    memset(&statusInfoFresh,0,sizeof(STATUS_BAR_INFO_T));
    if(disptime > 0)
    {
        statusInfoFresh.dispEnd = time(NULL) + disptime;
    }
    strncpy(statusInfoFresh.text,dispText,sizeof(dispText));
    result = 1;
    ret = sendback_msg_build(pInMsg,&result,1);
    return ret;
}

int desktop_fresh_tool(MESSAGE_INFO_T *pInMsg)
{
    int ret = 0;
    int offset = 0;
    uint8 choice1 = -1;
    uint8 choice2 = -1;
    uint8 fieldCnt = 0;
    BOOLEAN result = 0;
    int i;
    TOOL_BAR_INFO_T *pToolInfo = &toolInfoFresh;
    if(pInMsg->IID != MESSAGE_IID_DESKGUI || pInMsg->IOP != DESKGUI_IOP_FRESH_TOOL)
    {
        return -1;
    }
    //ˢ�¹����������ؽ��ֵ
    //memset(pToolInfo,0,sizeof(TOOL_BAR_INFO_T));
    fieldCnt = pInMsg->playload[offset++];
    for(i = 0; i < fieldCnt; i++)
    {
        choice1 = pInMsg->playload[offset++];
        if(choice1 == CHANNEL1 || choice1 == CHANNEL2)
        {
            choice2 = pInMsg->playload[offset++];
            if(choice1 == CHANNEL1)
            {
                switch (choice2)
                {
                    case SIG_STRENGTH:
                        appmsg_memcpy_r(&(pToolInfo->channel1.signalStrengh), &pInMsg->playload[offset], sizeof(pToolInfo->channel1.signalStrengh));
                        offset += sizeof(pToolInfo->channel1.signalStrengh);
                        #if (defined(PRODUCT_CCU) && defined(AREA_CHONGQING))
                            appmsg_update_signalinfo_split(pToolInfo->channel1.signalStrengh);
                        #endif
                        printf("[������]pToolInfo->channel1.signalStrengh = %d\n",pToolInfo->channel1.signalStrengh);
                    break;
                    
                    case NET_FORMAT:
                        appmsg_memcpy_r(&(pToolInfo->channel1.netFormat), &pInMsg->playload[offset], sizeof(pToolInfo->channel1.netFormat));
                        offset += sizeof(pToolInfo->channel1.netFormat);
                        printf("[������]pToolInfo->channel1.netFormat = %d\n",pToolInfo->channel1.netFormat);
                    break;

                    case CON_STATUS:
                        appmsg_memcpy_r(&(pToolInfo->channel1.conStatus), &pInMsg->playload[offset], sizeof(pToolInfo->channel1.conStatus));
                        offset += sizeof(pToolInfo->channel1.conStatus) ;
                        printf("[������]pToolInfo->channel1.conStatus = %d\n",pToolInfo->channel1.conStatus);
                    break;

                    default:
                        break;
                }
            }
            else    //choice1 == 1
            {
                switch (choice2)
                {
                    case SIG_STRENGTH:
                        appmsg_memcpy_r(&(pToolInfo->channel2.signalStrengh), &pInMsg->playload[offset], sizeof(pToolInfo->channel2.signalStrengh));
                        offset += sizeof(pToolInfo->channel2.signalStrengh);
                        #if (defined(PRODUCT_CCU) && defined(AREA_CHONGQING))
                            appmsg_update_signalinfo_split(pToolInfo->channel2.signalStrengh);
                        #endif
                        printf("[������]pToolInfo->channel2.signalStrengh = %d\n",pToolInfo->channel2.signalStrengh);
                    break;
                
                    case NET_FORMAT:
                        appmsg_memcpy_r(&(pToolInfo->channel2.netFormat), &pInMsg->playload[offset], sizeof(pToolInfo->channel2.netFormat));
                        offset += sizeof(pToolInfo->channel2.netFormat);
                        printf("[������]pToolInfo->channel2.netFormat = %d\n",pToolInfo->channel2.netFormat);
                    break;
            
                    case CON_STATUS:
                        appmsg_memcpy_r(&(pToolInfo->channel2.conStatus), &pInMsg->playload[offset], sizeof(pToolInfo->channel2.conStatus));
                        offset += sizeof(pToolInfo->channel2.conStatus) ;
                        printf("[������]pToolInfo->channel2.conStatus = %d\n",pToolInfo->channel2.conStatus);
                    break;
            
                    default:
                        break;
                }

            }

        }
        else
        {
            switch(choice1)
            {
                case ETH_CON_STATUS:
                    pToolInfo->ethConStatus = pInMsg->playload[offset++];
                    printf("[������]pToolInfo->ethConStatus = %d\n",pToolInfo->ethConStatus);
                break;

                case WARN:
                    pToolInfo->warning = pInMsg->playload[offset++];
                    printf("[������]pToolInfo->warning = %d\n",pToolInfo->warning);
                break;

                case INFO_ID:
                    appmsg_memcpy_r(&(pToolInfo->infoID), &pInMsg->playload[offset], sizeof(pToolInfo->infoID));
                    offset += sizeof(pToolInfo->infoID);
                    printf("[������]pToolInfo->infoID = %d\n",pToolInfo->infoID);
                break;

                case KEY_STATUS:
                    pToolInfo->keyStatus = pInMsg->playload[offset++];
                    printf("[������]pToolInfo->keyStatus = %d\n",pToolInfo->keyStatus);
                    write_pdata_xram(GUI_PARA, (const char *)&pToolInfo->keyStatus, Esam_Status, Esam_Status_Len);
                break;
                
                case BLUE_STATUS:
                    pToolInfo->blueStatus = pInMsg->playload[offset++];
                    printf("[������]pToolInfo->blueStatus = %d\n",pToolInfo->blueStatus);
                break;
                case RADIO_STATUS:
                    pToolInfo->radioStatus = pInMsg->playload[offset++];
                    printf("[������]pToolInfo->radioStatus = %d\n",pToolInfo->blueStatus);
                break;
            }
        }
    }
    result = 1;
    ret = sendback_msg_build(pInMsg,&result,1);
    return ret;

}

//not done
int desktop_pop_msg(MESSAGE_INFO_T *pInMsg)
{
    int ret = 0;
    int offset = 0;
    uint8 msglen = 0;
    BOOLEAN result = 0;
    uint32 disptime = 0;        //��ʾʱ��
    char dispText[512] = {0};   //��ʾ����
    uint16 textLen = 0; 
    if(pInMsg->IID != MESSAGE_IID_DESKGUI || pInMsg->IOP != DESKGUI_IOP_PUSH_HINT)
    {
        return -1;
    }
    appmsg_memcpy_r((uint8*)&disptime,&pInMsg->playload[offset],sizeof(uint32));
    offset += sizeof(uint32);
    textLen = appmsg_get_len_offset(&pInMsg->playload[offset], &msglen);
    offset += msglen;
    if(textLen > 511)
    {
        textLen = 511;
    }
    memcpy(dispText,&pInMsg->playload[offset],textLen);
    offset += textLen;

    GUI_FMT_DEBUG("[0007] desktop_pop_msg :text :%s , display time :%d\n", dispText,disptime);
    if(g_pop_mes_Fresh.delayTime >=3 )          //����3���ڲ�����
    {
//        memset(&g_pop_mes_Fresh,0,sizeof(POP_MES_BOX_T));
        g_pop_mes_Fresh.dispEnd = 0;

        g_pop_mes_Fresh.delayTime = 3;
        
        if(disptime > 0)
        {
            g_pop_mes_Fresh.dispEnd = time(NULL) + disptime;
        }
        strncpy(g_pop_mes_Fresh.text,dispText,textLen);
        g_pop_mes_Fresh.text[textLen] = '\0';
        openAuthor();
        
        if(pGuiDev != NULL)
        {
            ret = pGuiDev->lcd_active_id_set(pGuiDev,0);
            if(ret != 0)
            {
                GUI_FMT_DEBUG("lcd_active_id_set:activeID = 0 ret %d\n", ret);
                ret = pGuiDev->lcd_active_id_set(pGuiDev,0);
            }
            GUI_FMT_DEBUG("desktop_pop_msg ��set activeID=0 ret %d\n", ret);
            
        }
        g_guiMain.nLightonTime = 0;
        g_guiMain.bLighton = TRUE;
        lighton_lcd();
        g_pop_mes_Fresh.is_pop = TRUE;
        PopMsg(g_pop_mes_Fresh.text);
        result = 1;
        ret = sendback_msg_build(pInMsg,&result,1);
    }
    else
    {
        result = 0;
        ret = sendback_msg_build(pInMsg,&result,1);
    }
    
    return ret;
}

//not done
int desktop_cancel_pop(MESSAGE_INFO_T *pInMsg)
{
    int ret = 0;
    
    BOOLEAN result = 0;
    if(pInMsg->IID != MESSAGE_IID_DESKGUI || pInMsg->IOP != DESKGUI_IOP_POP_HINT)
    {
        return -1;
    }

    //ȡ����Ϣ��ʾ�����
    memset(&g_pop_mes_Fresh,0,sizeof(POP_MES_BOX_T));
    ClosePopLayer();
    ret = sendback_msg_build(pInMsg,&result,1);
    return ret;
}

//֪ͨ��ǰapp�˳�����״̬, not done,��ȷ��
int desktop_exit_turndisp(MESSAGE_INFO_T *pInMsg)
{
    int ret = 0;
    
    BOOLEAN result = 1;
    if(pInMsg->IID != MESSAGE_IID_DESKGUI || pInMsg->IOP != DESKGUI_IOP_EXIT_DISPLAY)
    {
        return -1;
    }

    if(pGuiDev != NULL)
    {
        ret = pGuiDev->lcd_active_id_set(pGuiDev,0);
        if(ret != 0)
        {
            GUI_FMT_DEBUG("lcd_active_id_set:activeID = 0 ret %d\n", ret);
            ret = pGuiDev->lcd_active_id_set(pGuiDev,0);
        }
        GUI_FMT_DEBUG("desktop_exit_turndisp��set activeID=0 ret %d\n", ret);
    }
    liveAppID = 0;
    openAuthor();
    g_abort_Trun = TRUE;
    //
    ret = sendback_msg_build(pInMsg,&result,1);
    return ret;
}
//app��ѯ��ǰǰ̨app
int desktop_live_app(MESSAGE_INFO_T *pInMsg)
{
    int     ret = 0;
    uint32  appID = 0;
    uint8   dataBuf[4] = {0};


    if(pInMsg->IID != MESSAGE_IID_DESKGUI || pInMsg->IOP != DESKGUI_IOP_LIVE_APP)
    {
        return -1;
    }

    appID = liveAppID;
    GUI_FMT_TRACE("[0007]message req for alive app ID: now aliveAppID is %d\n", appID);
    
    appmsg_memcpy_r(dataBuf,(char*)&appID,4);
    
    ret = sendback_msg_build(pInMsg,dataBuf,4);
    
    return ret;
    
}

int turn_disp_inform_events(uint32 AppID, uint8 InOrOut)
{
    int len = 0;
    uint32 msglen = 0;
    int ret = 0;
    MESSAGE_INFO_T * pMsgInfo = NULL;
    char topic[256] = {0};
    int i;
    const char *destaddr = "Broadcast";
    
    if(InOrOut == 0)    //��������
    {
        pthread_mutex_lock(&appGuiInfoLock);
        for(i = 0; i< appGuiInfoSaved.loginAppCnt; i++)
        {
            if(AppID == appGuiInfoSaved.loginID[i])
            {
                break;
            }
        }
        if(i >= appGuiInfoSaved.loginAppCnt)
        {
            GUI_FMT_DEBUG("����not find the regist info of app[%u],turn_disp_inform_events failed!\n", AppID);
            pthread_mutex_unlock(&appGuiInfoLock);
            return -1;
        }
        pthread_mutex_unlock(&appGuiInfoLock);
        
        msglen = sizeof(uint8) + sizeof(uint32);
    }
    else                //�˳�����
    {
        InOrOut = 1;    //��ֹ����쳣����
        msglen = sizeof(uint8);
    }
    
    len = sizeof(MESSAGE_INFO_T) + msglen;
    pMsgInfo = (MESSAGE_INFO_T*)malloc(len);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_DEBUG("[0007]%s malloc failed %d\n",__FUNCTION__,len);
        return -1;
    }
    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = gui_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_DESKGUI;
    pMsgInfo->IOP = DESKGUI_IOP_DISPLAY_ENENT;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr , g_AppName, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(destaddr);
    memcpy(pMsgInfo->destaddr, destaddr, pMsgInfo->destlen);
    
    pMsgInfo->msglen = msglen;

    pMsgInfo->playload[0] = InOrOut;
    if(InOrOut == 0)    //��������
    {
        appmsg_memcpy_r(pMsgInfo->playload+1, (char*)&AppID, sizeof(uint32));
    }
    sprintf(topic,"%s/%s/%04X/%04X",g_AppName,pMsgInfo->destaddr, MESSAGE_IID_DESKGUI, DESKGUI_IOP_DISPLAY_ENENT);
    ret = g_GuiSendMsgFunc(topic, pMsgInfo, 0);
    free(pMsgInfo);

    if(InOrOut == 0)    //��������
    {
        liveAppID = AppID;
        closeAuthor();

        if(pGuiDev != NULL)
        {
            pGuiDev->lcd_active_id_set(pGuiDev, AppID);
            
            ret = pGuiDev->lcd_active_id_set(pGuiDev, AppID);
            if(ret != 0)
            {
                GUI_FMT_DEBUG("lcd_active_id_set:activeID = 0 ret %d\n", ret);
                ret = pGuiDev->lcd_active_id_set(pGuiDev, AppID);
            }
            GUI_FMT_DEBUG("����app:%s, activeID: %d ret %d \n",appGuiInfoSaved.applogin[i].appname,AppID, ret);

        }
    }
    else                //�˳�����
    {
        openAuthor();
        liveAppID = 0;
        if(pGuiDev != NULL)
        {
            ret = pGuiDev->lcd_active_id_set(pGuiDev,0);
            if(ret != 0)
            {
                GUI_FMT_DEBUG("lcd_active_id_set:activeID = 0 ret %d\n", ret);
                ret = pGuiDev->lcd_active_id_set(pGuiDev,0);
            }
            GUI_FMT_DEBUG("�˳�����app:%d ret %d \n",AppID, ret);
        }
    }
    return ret;
}

int desktop_key_pop_cancel(void)
{
    int len = 0;
    uint32 msglen = 5;
    int ret = 0;
    MESSAGE_INFO_T * pMsgInfo = NULL;
    char topic[256] = {0};
    
    const char *destaddr = "M-stAmr";
    
    len = sizeof(MESSAGE_INFO_T) + msglen;
    pMsgInfo = (MESSAGE_INFO_T*)malloc(len);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_DEBUG("[0007]%s malloc failed %d\n",__FUNCTION__,len);
        return -1;
    }
    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = gui_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_DESKGUI;
    pMsgInfo->IOP = DESKGUI_IOP_PUSH_HINT;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr , g_AppName, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(destaddr);
    memcpy(pMsgInfo->destaddr, destaddr, pMsgInfo->destlen);
    
    pMsgInfo->msglen = msglen;

    
    sprintf(topic,"%s/%s",g_AppName,pMsgInfo->destaddr);
    ret = g_GuiSendMsgFunc(topic, pMsgInfo, 0);
    free(pMsgInfo);

    return ret;
}


//app�����¼���0x0002
int app_rise_inform_event(char* pAppName)
{
    int len = 0;
    int ret = 0;
    MESSAGE_INFO_T * pMsgInfo = NULL;
    char topic[256] = {0};
    char tempName[64] = {0};
    int i;
    const char *destaddr = "Broadcast";
    if(pAppName == NULL)
    {
        return -1;
    }
    
    if(getAuthor() != 1 || liveAppID != 0)      //��ǰ������app��Ծ��
    {
        GUI_FMT_DEBUG("[0007]app_rise_inform_event failed,appAuthority = %d,liveAppID = %d\n", getAuthor(),liveAppID);
        return -1;
    }
    pthread_mutex_lock(&appGuiInfoLock);
    for(i = 0; i< appGuiInfoSaved.loginAppCnt; i++)
    {
        memset(tempName,0,sizeof(tempName));
        strncpy(tempName,appGuiInfoSaved.applogin[i].appname,strlen(appGuiInfoSaved.applogin[i].appname));
        if(strstr(tempName,pAppName) != NULL)
        {
            liveAppID = appGuiInfoSaved.loginID[i];
            closeAuthor();
            break;
        }
    }
    if(i >= appGuiInfoSaved.loginAppCnt)
    {
        pthread_mutex_unlock(&appGuiInfoLock);
        GUI_FMT_DEBUG("[0007]not find the regist info of app[%s],app_rise_inform_event failed!\n", pAppName);
        return -1;
    }
    pthread_mutex_unlock(&appGuiInfoLock);
    len = sizeof(MESSAGE_INFO_T) + sizeof(uint32);
    pMsgInfo = (MESSAGE_INFO_T*)malloc(len);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_DEBUG("[0007]%s malloc failed %d\n",__FUNCTION__,len);
        return -1;
    }
    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = gui_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_DESKGUI;
    pMsgInfo->IOP = DESKGUI_IOP_APP_ACTIVATE;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr , g_AppName, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(destaddr);
    memcpy(pMsgInfo->destaddr,destaddr,pMsgInfo->destlen);
    
    pMsgInfo->msglen = sizeof(uint32);
    appmsg_memcpy_r(pMsgInfo->playload, (char*)&liveAppID, pMsgInfo->msglen);
    sprintf(topic,"%s/%s/%04X/%04X",g_AppName,pMsgInfo->destaddr, MESSAGE_IID_DESKGUI, DESKGUI_IOP_APP_ACTIVATE);
    ret = g_GuiSendMsgFunc(topic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @brief     ��APP��������APP��Ϣ����ӿ�
* @param[in] ��MESSAGE_INFO_T *pInMsg          ��Ϣ�ṹ��
* @param[out]��   
* @return    ��0 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2021-2-4
* @Update    :
**********************************************************************/
int appGUI_recv_back_handle(MESSAGE_INFO_T *pInMsg)
{
    int ret = 0;
    if(pInMsg == NULL)
        return -1;
    uint32 tempAppID = 0;
    
    uint16  IID = pInMsg->IID;
    uint16  IOP = pInMsg->IOP;
//  printf("�յ�����gui����Ϣ!,IID = %04X,IOP = %04X \n",pInMsg->IID,pInMsg->IOP);
    if(IID != MESSAGE_IID_DESKGUI)
        return -1;

    switch(IOP)
    {
        case DESKGUI_IOP_DISPLAY_ENENT:         //����֪ͨ�¼�
        {
            if(loginguiID == 0)
            {
                GUI_FMT_DEBUG("�յ�������Ϣ%hu, ��APP��δע����������ʾ���˳��������̡�\n", IOP);
                return -1;
            }
            
            if(pInMsg->playload[0] == EnterTurnDisp)
            {
                appmsg_memcpy_r(&tempAppID, &pInMsg->playload[1], sizeof(uint32));
                GUI_FMT_TRACE("�յ�����gui����Ϣ!,tempAppID = %u,loginguiID = %u \n", tempAppID, loginguiID);
                if(tempAppID == loginguiID)
                {
                    if(strncmp(localAppName,"puAmr",sizeof("puAmr")) == 0 || strncmp(localAppName,"stAmr",sizeof("stAmr")) == 0 )   //Ŀǰֻ��puAmr������
                    {
                        openAuthor();
                        liveAppID = loginguiID;
                        StrartTrunDisp();
                        TurnDisp = TRUE;
                        
                    }
                    else
                    {
                        appGUI_send_msg_to_gui(MESSAGE_IID_DESKGUI, DESKGUI_IOP_EXIT_DISPLAY, NULL, 0);
                        closeAuthor();
                        liveAppID = 0;
                    }
                }
            }
            else if(pInMsg->playload[0] == ExitTurnDisp)
            {
                if(g_guiMain.bTrunDisp == TRUE)
                {
                    StopTrunDisp();
                    closeAuthor();
                    liveAppID = 0;
                    
                }
                TurnDisp = FALSE;
            }
            GUI_FMT_DEBUG("�յ�����֪ͨ�¼�: %s����\n",(TurnDisp == TRUE)? "����":"�˳�");
            
        }
        break;
        case DESKGUI_IOP_APP_ACTIVATE:
        {
            if(loginguiID == 0)
            {
                GUI_FMT_DEBUG("�յ�������Ϣ%hu, ��APP��δע����������ʾ���˳��������̡�\n", IOP);
                return -1;
            }
            appmsg_memcpy_r(&tempAppID, &pInMsg->playload[0], sizeof(uint32));
            GUI_FMT_DEBUG("loginguiID = %d,tempAppID = %d\n",loginguiID,tempAppID);
            if(tempAppID == loginguiID)
            {
                g_guiMain.nLightonTime = 0;
                g_guiMain.bLighton = TRUE;
                if(g_guiMain.bTrunDisp == TRUE)
                {
                    StopTrunDisp();
                }
//                Initialize(NULL, FALSE);
                // ҳ����ʼ��
    			if(g_guiMain.pluginCur != NULL)
    			{
    				g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
    			}
                lighton_lcd();
                openAuthor();
            }
        }
        break;

        case DESKGUI_IOP_APP_REG:               //appע������
        {
            appmsg_memcpy_r((uint8*)&loginguiID, pInMsg->playload, sizeof(uint32));
            GUI_FMT_DEBUG("ע��ɹ�,ע��ID = %d\n", loginguiID);
            if(pAppDev != NULL)
            {
                pAppDev->app_regist_id = loginguiID;
                GUI_FMT_TRACE("pAppDev->app_regist_id = %d ",pAppDev->app_regist_id);
            }
                
        }
        break;

        case DESKGUI_IOP_APP_UNREG:             //app�˳�������
        {
            GUI_FMT_TRACE("�յ�����Gui����,app�˳�%s\n", (pInMsg->playload[0] == 1)? "�ɹ�":"ʧ��");
        }
        break;

        case DESKGUI_IOP_FRESH_STATE:           //ˢ��״̬��
        {
            //printf("�յ�����Gui����,ˢ��״̬��%s\n", (pInMsg->playload[0] == 1)? "�ɹ�":"ʧ��");
        }
        break;

        case DESKGUI_IOP_FRESH_TOOL:            //ˢ�¹�����
        {
            //printf("�յ�����Gui����,ˢ�¹�����%s\n", (pInMsg->playload[0] == 1)? "�ɹ�":"ʧ��");
        }

        break;

        case DESKGUI_IOP_PUSH_HINT:             //������Ϣ��ʾ��
        break;

        case DESKGUI_IOP_POP_HINT:              //ȡ����Ϣ��ʾ��
        break;

        case DESKGUI_IOP_LIVE_APP:              //��ǰ��Ծapp
        {
            
            appmsg_memcpy_r((uint8*)&liveAppID, pInMsg->playload, sizeof(uint32));
            GUI_FMT_DEBUG("��ǰ��ԾappID = %d\n", liveAppID);
        }
        break;

        case DESKGUI_IOP_EXIT_DISPLAY:              //�˳����Ի�ִ
        break;

        default:
        GUI_FMT_DEBUG("wrong data IOP = %d\n", IOP);
        return -1;
        break;
    }
    return ret;

}


int appGUI_send_req_handle(uint16 IID, uint16 IOP, void* dataIn, uint32 InLen)
{
    int ret = 0;
    if(IID != MESSAGE_IID_DESKGUI)
        return -1;
    GUI_FMT_TRACE("��desktopGui��������,IOP = 0x%04x\n",IOP);
    switch(IOP)
    {
//      case DESKGUI_IOP_DISPLAY_ENENT:         //����֪ͨ�¼�
//      return turn_disp_inform_event();
//      break;

        case DESKGUI_IOP_APP_REG:               //appע������
        return appGUI_req_reg(dataIn, InLen);
        break;

        case DESKGUI_IOP_APP_UNREG:             //app�˳�������
        return appGUI_req_exit();
        break;

        case DESKGUI_IOP_FRESH_STATE:           //ˢ��״̬��
        case DESKGUI_IOP_FRESH_TOOL:            //ˢ�¹�����
        case DESKGUI_IOP_PUSH_HINT:             //������Ϣ��ʾ��
        case DESKGUI_IOP_POP_HINT:              //ȡ����Ϣ��ʾ��
        case DESKGUI_IOP_LIVE_APP:              //��ǰ��Ծapp
        default:
            GUI_FMT_DEBUG("wrong data IOP = %d\n", IOP);
            return -1;
        break;
    }
    return ret;
}

int appGUI_send_msg_to_gui(uint16 IID, uint16 IOP, void* dataIn, uint32 InLen)
{
    MESSAGE_INFO_T* pMsgInfo = NULL;
    int ret = 0;
    char    ComAppName[32] = {0};
    char    topic[256] = {0};
    
    uint32 len = sizeof(MESSAGE_INFO_T) + InLen;
    pMsgInfo = (MESSAGE_INFO_T*)malloc(len);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_DEBUG("[0007]%s malloc failed %u\n",__FUNCTION__,len);
        return -1;
    }
    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = gui_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = IID;
    pMsgInfo->IOP = IOP;

    sprintf(ComAppName,"M-%s",localAppName);
    pMsgInfo->sourlen = strlen((char *)ComAppName);
    memcpy(pMsgInfo->souraddr , ComAppName, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(deskGui);
    memcpy(pMsgInfo->destaddr,deskGui,pMsgInfo->destlen);
    pMsgInfo->msglen = InLen;
    if(pMsgInfo->msglen)
    {
        memcpy(pMsgInfo->playload, (char*)dataIn, pMsgInfo->msglen);  //MSG's-Payload
    }
    sprintf(topic,"%s/%s",ComAppName,pMsgInfo->destaddr);
    if(memcmp(g_AppName,"M-smiOS",sizeof("M-smiOS")) == 0 && pfuncSend != NULL)
    {
        ret = pfuncSend(g_AppName, pMsgInfo->destaddr, pMsgInfo );
    }
    else
    {
        ret = g_GuiSendMsgFunc(topic, pMsgInfo, 0);
    }
    free(pMsgInfo);
    pMsgInfo = NULL;

    return ret;
}

int appGUI_req_reg(void* dataIn, uint32 InLen)
{
    int ret = 0;
    uint8* psenddata = NULL;
    uint8  appnamelen = 0;
    uint8  shortlen = 0;
    uint32 senddatalen = 0;
    uint16 iconfilelen = 0;
    MESSAGE_INFO_T* pMsgInfo = NULL;
    uint32 offset = 0;
    struct stat st;
    char    ComAppName[32] = {0};
    char    topic[256] = {0};

    if(dataIn == NULL || InLen != sizeof(APP_REGIST_INFO_T))
        return -1;

    APP_REGIST_INFO_T* pRegInfo = (APP_REGIST_INFO_T*)dataIn;
    appnamelen = strlen(pRegInfo->appname);
    shortlen = strlen(pRegInfo->shortname);
    int fd = open(pRegInfo->iconFileName,O_RDONLY);
    if(fd == -1)
    {
        GUI_FMT_DEBUG("λͼ�ļ� %s ��ȡʧ��!\n",pRegInfo->iconFileName);
        return -1;
    }
    fstat(fd,&st);
    iconfilelen = st.st_size;
    senddatalen = 1 + strlen(pRegInfo->appname) + 1 + strlen(pRegInfo->shortname) + 3 + iconfilelen;
    psenddata = (uint8*)malloc(senddatalen);
    if(!psenddata)
    {
        GUI_FMT_DEBUG("λͼ�ļ�mallocʧ��\n");
        return -1;
    }
    psenddata[offset++] = appnamelen;
    memcpy(&psenddata[offset],pRegInfo->appname,appnamelen);
    offset += appnamelen;
    psenddata[offset++] = shortlen;
    memcpy(&psenddata[offset],pRegInfo->shortname,shortlen);
    offset += shortlen;
    psenddata[offset++] = 0x82;
    uint16 tmplen = iconfilelen-0x3e;
    appmsg_memcpy_r(&psenddata[offset], (uint8*)&tmplen, 2);
    offset += 2;
    int readlen = read(fd,&psenddata[offset],iconfilelen);
    if(readlen != iconfilelen)
    {
        GUI_FMT_DEBUG("λͼ�ļ���ȡʧ��,�ļ�����%d,ʵ�ʶ�ȡ%d\n",iconfilelen,readlen);
        return -1;
    }

    memmove(&psenddata[offset], &psenddata[offset+0x3e], iconfilelen-0x3e);
    senddatalen -= 0x3e;
    //memcpy(dataOut,psenddata,senddatalen);
    //OutLen = senddatalen;

    int len = sizeof(MESSAGE_INFO_T) + senddatalen;
    pMsgInfo = (MESSAGE_INFO_T*)malloc(len);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_DEBUG("[0007]%s malloc failed %d\n",__FUNCTION__,len);
        return -1;
    }
    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = gui_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_DESKGUI;
    pMsgInfo->IOP = DESKGUI_IOP_APP_REG;

    sprintf(ComAppName,"M-%s",localAppName);
    pMsgInfo->sourlen = strlen((char *)ComAppName);
    memcpy(pMsgInfo->souraddr , ComAppName, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(deskGui);
    memcpy(pMsgInfo->destaddr,deskGui,pMsgInfo->destlen);
    pMsgInfo->msglen = senddatalen;
    memcpy(pMsgInfo->playload, (char*)psenddata, pMsgInfo->msglen);
    sprintf(topic,"%s/%s",ComAppName,pMsgInfo->destaddr);

    if(memcmp(g_AppName,"M-smiOS",sizeof("M-smiOS")) == 0 && pfuncSend != NULL)
    {
        ret = pfuncSend(g_AppName, pMsgInfo->destaddr, pMsgInfo );
    }
    else
    {
        ret = g_GuiSendMsgFunc(topic, pMsgInfo, 0);
    }
    free(pMsgInfo);
    close(fd);
    free(psenddata);
    psenddata = NULL;
    pMsgInfo = NULL;

    return ret;
}


int appGUI_req_exit(void)
{
    int ret = 0;
    MESSAGE_INFO_T* pMsgInfo = NULL;
    char    ComAppName[32] = {0};
    char    topic[256] = {0};

    int len = sizeof(MESSAGE_INFO_T) + sizeof(uint32);
    pMsgInfo = (MESSAGE_INFO_T*)malloc(len);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_DEBUG("[0007]%s malloc failed %d\n",__FUNCTION__,len);
        return -1;
    }
    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = gui_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_DESKGUI;
    pMsgInfo->IOP = DESKGUI_IOP_APP_UNREG;

    sprintf(ComAppName,"M-%s",localAppName);
    pMsgInfo->sourlen = strlen((char *)ComAppName);
    memcpy(pMsgInfo->souraddr , ComAppName, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(deskGui);
    memcpy(pMsgInfo->destaddr,deskGui,pMsgInfo->destlen);
    pMsgInfo->msglen = sizeof(uint32);
    appmsg_memcpy_r(pMsgInfo->playload, (uint8*)&loginguiID, sizeof(uint32));
    sprintf(topic, "%s/%s",ComAppName,pMsgInfo->destaddr);
    if(memcmp(g_AppName,"M-smiOS",sizeof("M-smiOS")) == 0 && pfuncSend != NULL)
    {
        ret = pfuncSend(g_AppName, pMsgInfo->destaddr, pMsgInfo );
    }
    else
    {
        ret = g_GuiSendMsgFunc(topic, pMsgInfo, 0);
    }
    free(pMsgInfo);
    if(ret == 0)
    {
        closeAuthor();
    }
    return ret;
}

/**********************************************************************
* @name      : set_gui_infonum
* @brief     ��������Ϣ���
* @param[in] ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2020-7-9
* @Update    : 
**********************************************************************/
void set_gui_infonum(uint16 infonum)
{
    toolInfoFresh.infoID = infonum;
}

//��ʼ������
int PasswordInitialized()
{
    int ret = write_pdata_xram(GUI_PRIVATE_DATA, PASSWORD_STR, PRIVATE_PASSWORD, PASSWORD_LEN);
    ret |= write_pdata_xram("/data/app/desktopGui/data/^gui_paivate_file", PASSWORD_STR, PRIVATE_PASSWORD, PASSWORD_LEN);

    return ret;
}

//��������
int SetPassword(char password[])
{
    int ret = write_pdata_xram(GUI_PRIVATE_DATA, password, PRIVATE_PASSWORD, PASSWORD_LEN);
    ret |= write_pdata_xram("/data/app/desktopGui/data/^gui_paivate_file", password, PRIVATE_PASSWORD, PASSWORD_LEN);

    return ret;
}
//int appGUI_req_fresh_state(void* dataIn, uint32 InLen)
//{
//  return 0;
//}

