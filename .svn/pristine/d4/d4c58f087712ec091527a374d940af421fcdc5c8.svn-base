/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
  程序名称: main.h
  功    能：FKGA42显示模块框架文件实现文件
  开 发 人：gaoyf
  开发时间：2007-1

  版本历史:

    v1.0:
      2006-1-8, gaoyf : New file

***************************************************************************
*/
#include "main.h"
#ifndef _MSC_VER
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/time.h>
#endif
#include <net/if.h> 

#include "scrollview.h"
#include "scrollbar.h"
#include "static.h"

#include "task_id.h"
#include "msg_base.h"
#include "out_msg.h"
#include "msg_proc.h"
#include "iniparser.h"
#include "task_msg.h"
#include "appoutmsg.h"
#include "appreport.h"
#include "gui_prtl.h"
#include "lcd.h"
#include "power.h"
#include "key.h"
#include "signal.h"
#include "framecomm.h"
#include "trace_back.h"
#include "eeprom.h"

//extern uint8 * g_pFrameBuffer;
extern "C"
{
    #include "desk_meter.h"
    int32 LCD_Refresh( LCD_DEVICE_T* dev, uint32 startx, uint32 starty,uint32 endx, uint32 endy);
}



uint32 activeID = 1000;
uint32 lastactiveID = 2000;

extern BOOL ReadConfig(BOOL bAll);
extern BOOL RestoreConfigFromXml(char* appName);
LCD_DEVICE_T* gui_dev;

//本地APP启动时，异常点亮背光灯，特殊处理
extern uint8 blkFlag;

extern PLUGIN_INFO g_layerValidatePwd;
GUI_MAIN g_guiMain = {0, 0, 0, 0, 0, 0, NULL, 0, NULL, NULL, NULL, NULL, NULL, FALSE, TRUE, 0, 10, FALSE, 0, 15, 0, -1, 5};
#if BEIJING_FUN
GUI_COMM g_guiComm = {NULL, NULL, NULL, 160, 160, TRUE, 0, 0, 0, 0, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE};
#else
GUI_COMM g_guiComm = {NULL, NULL, NULL, 160, 160, TRUE, 0, 0, 0, 0, FALSE, FALSE, TRUE, FALSE, TRUE};
#endif

PLUGIN_INFO* g_pluginTrunDisp[MAX_TRUNDISP_CNT];
int g_nTrunDisp[MAX_TRUNDISP_CNT];  // 轮显页面库数组
int g_nTotalTrunDispCnt = 0;
int g_nIpcRet = -1;

int g_bQuickTimer = 0;  // 100ms定时器
int g_bMainTimer = 0;
int g_bTimerDispose = 0;
int g_bKeyboard = 0;
unsigned char g_nKeyCode = 0;

BOOL g_bLogo = FALSE;
int g_nLogoTime = 0; //-logo页面显示时间-
uint32 gethstatus = TRUE;

BOOL g_bpop = FALSE;                //弹出提示框
BOOL g_bOftenDisp = FALSE;          //是否常显
BOOL g_turn_cutover_finish = TRUE;  //轮显状态切换是否完成

BOOL g_bUpdate = FALSE;
#if HUBEI_FUN == 1
char g_poweron_Flag = 0;
char g_poweron_reset_Flag = 0;//彻底断电重启
char g_count = 0;
#endif

extern int g_narrMenuIndex[20];
extern int g_nMenuNodes;

int g_nPasswdTime = 0;      // 密码失效时间
uint8 g_read_sc[20];        //初始化读取记录成功与否

BOOL g_bAllMenu = FALSE;        // 是否显示隐藏菜单
//隐藏菜单显示时间
int g_allMenu_time = 0;

DB_CLIENT g_hUdbCenter;
DB_AGENT  g_dbAgent = 0;
extern BOOL g_getdbstate;
BOOL g_firstread;
BOOL g_circleNo;

#define MINI_GUI_TASK_PATH  "desktopGui"

#define TERMINAL_TYPE   "/mnt/data/app/desktopGui/terminaltype"


/*MessageBox页面库操作*/
typedef struct _MSG_INFO
{
    char*   pczMsg;     // MessageBox消息
    BOOL    bExist;     // 判断是否退出页面库标志
    BOOL    bRet;       // 退出时的返回值
    int     nType;      // MessageBox类型MB_OK、MB_OKCANCEL
}MSG_INFO_t;
MSG_INFO_t g_msgInfo = {NULL, FALSE, FALSE, MB_OK};

uint8        g_OutMsgFlag = 0;
OUTMSG_THREAD_T g_tOutMsg = {&g_hUdbCenter, &g_OutMsgFlag};


char* TBoot(char* pBuf)
{
    while(*pBuf != '\0')
    {
        if(*pBuf == ' ')
        {
            pBuf ++;
        }
        else
        {
            break;
        }
    }
    return pBuf;
}

void Assert(const char* pFileName, int nLine)
{
    GUI_FMT_TRACE("error is %s----%d\n", pFileName, nLine);
}

/**********************************************************************
* @name      : get_power_state
* @brief     ：获取电源状态
* @param[in] ：  
* @param[out]：state: 电源状态，1有电，0 掉电
* @return    ：1 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-07-27
* @Update    :
**********************************************************************/
int get_power_state(int *state)
{
    POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
    if(NULL == dev)
    {
        return -1;
    }
    
    if(dev->power_power_state_get(dev,POWER_MAIN,state) < 0)
    {
        return -1;
    }

    
    hal_device_release((HW_DEVICE *)dev);
    return 1;
}


void PrintHexData(char* pHex, int nHexLen)
{
    //printf("pHex is %p----%d\n", pHex, nHexLen);

    ASSERT(pHex != NULL && nHexLen > 0);
    char czBuf[4096];
    char* pTmp = czBuf;
    if(nHexLen > 1024) nHexLen=1024;
    for(int i = 0; i < nHexLen; i ++)
    {
        sprintf(pTmp, "%02X ", (unsigned char)pHex[i]);
        pTmp += 3;
    }
    czBuf[pTmp-czBuf] = '\n';
    czBuf[pTmp-czBuf+1] = '\0';
    GUI_FMT_TRACE(czBuf);
}

#define IPC_BUFFER  1024

/*********************************************************************
    所属功能单元：显示框架模块
    功能描述：    获取指定的页面库
    输入参数：    DWORD dwPluginId：页面库ID
    输出参数：
    函数返回值：  PLUGIN_INFO* ：页面库指针，NULL表示不存在
    内部处理流程：使用折半查找算法
********************************************************************/
PLUGIN_INFO* GetPlugIn(DWORD dwPluginId)
{
    int nbeg = -1;
    int nend = g_nPluginCnt;
    int nmid;
    while(nbeg+1 != nend)
    {
        nmid = (nbeg+nend)>>1;
        if(g_plugin[nmid]->dwId < dwPluginId)/*比较*/
        {
            nbeg = nmid;
        }
        else
        {
            nend = nmid;
        }
    }
    GUI_FMT_TRACE("GetPlugIn id---0x%08x\n", dwPluginId);
    
    if(nend >= g_nPluginCnt || g_plugin[nend]->dwId != dwPluginId)/*比较*/
    {
        GUI_FMT_TRACE("未找到页面id---0x%08x\n", dwPluginId);
        return NULL;
    }
    return g_plugin[nend];
}

/*********************************************************************
    所属功能单元：显示框架模块
    功能描述：    页面库的排序操作，为了快速检索,使用快排算法.
    输入参数：
    输出参数：
    函数返回值：  BOOL：成功返回TRUE，失败返回FALSE
    内部处理流程：采用快排算法
********************************************************************/
void QuickSortPlugin(int nLeft, int nRight)
{
    int i, j;
    PLUGIN_INFO* pPlugin;
    PLUGIN_INFO* pPluginTmp;

    if(nRight - nLeft <= 5)
    {
        return;
    }

    i = (nLeft+nRight)>>1;
    pPluginTmp = g_plugin[nLeft]; g_plugin[nLeft] = g_plugin[i]; g_plugin[i] = pPluginTmp;

    pPlugin = g_plugin[nLeft];
    i = nLeft;
    j = nRight+1;

    while(1)
    {
        do{i++;}while(i<j && g_plugin[i]->dwId<pPlugin->dwId);
        do{j--;}while(g_plugin[j]->dwId>pPlugin->dwId);
        if(i > j)
        {
            break;
        }
        pPlugin = g_plugin[i]; g_plugin[i] = g_plugin[j]; g_plugin[j] = pPlugin;
    }

    pPlugin = g_plugin[nLeft]; g_plugin[nLeft] = g_plugin[j]; g_plugin[j] = pPlugin;
    QuickSortPlugin(nLeft,j-1);
    QuickSortPlugin(j+1,nRight);
}
void sys_sort_insert(int nLeft, int nRight)
{
    int i, j;
    PLUGIN_INFO*  pTmp;
    for(i = nLeft+1; i <= nRight; i ++)
    {
        pTmp = g_plugin[i];
        for(j  = i; j>0 && pTmp->dwId<g_plugin[j-1]->dwId; j --)   /*比较*/
        {
            g_plugin[j] = g_plugin[j-1];
        }
        g_plugin[j] = pTmp;
    }
}

/*********************************************************************
    所属功能单元：显示框架模块
    功能描述：    弹出一个指定的页面库，用于弹出密码确认，IPC命令弹出
    输入参数：    DWORD dwPluginId：页面库ID
    输出参数：
    函数返回值：  BOOL ：TRUE表示成功执行，FALSE表示失败
    内部处理流程：
********************************************************************/
BOOL PopLayer(DWORD dwPluginId)
{
    ClosePopLayer();
//    StopTrunDisp();


    PLUGIN_INFO* pPlugin = GetPlugIn(dwPluginId);
    ASSERT(pPlugin != NULL);
    if(pPlugin == NULL)
    {
        return FALSE;
    }
    if(g_guiMain.pluginCur != NULL && g_guiMain.pluginCur->dwId == dwPluginId)
    {// 如果要弹出的页面与当前页面一致，就不再弹出
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }

    g_guiComm.nCurPointTmp = g_guiComm.nCurPoint;
    g_guiComm.bCanTrunDispTmp = g_guiComm.bCanTrunDisp;
    g_guiComm.nCurPoint = 0;

    g_guiComm.nIndexPointTmp = g_guiComm.nIndexPoint;
    g_guiComm.bPointOffsetTmp = g_guiComm.bPointOffset;

    g_guiComm.nArrowTmp = g_guiComm.nArrow;
    g_guiComm.nArrow = -1;

    if(pPlugin->pfnInitLayer != NULL)
    {
        pPlugin->pfnInitLayer(pPlugin->dwId);
    }
    g_guiMain.plugPop = pPlugin;


    UpdateWindow(g_guiComm.hMainWnd, TRUE);
    return TRUE;
}

void ClosePopLayer()
{
//    g_pop_mes_Fresh.delayTime = 0;
//    g_pop_mes_Fresh.is_pop = FALSE;
    if(g_guiMain.plugPop != NULL)
    {
        if(g_guiMain.plugPop->pfnUninitLayer != NULL)
        {
            g_guiMain.plugPop->pfnUninitLayer();
        }
        g_guiMain.plugPop = NULL;
        g_guiComm.nCurPoint = g_guiComm.nCurPointTmp;
        g_guiComm.bCanTrunDisp = g_guiComm.bCanTrunDispTmp;
        g_guiComm.nArrow = g_guiComm.nArrowTmp;

        g_guiComm.nIndexPoint = g_guiComm.nIndexPointTmp;
        g_guiComm.bPointOffset = g_guiComm.bPointOffsetTmp;

        UpdateWindow(g_guiComm.hMainWnd, TRUE);
    }
    
}

/*********************************************************************
    所属功能单元：显示框架模块
    功能描述：    显示模块初始化操作,主要进行配置文件读取，加载、初始化调用模块
    输入参数：BOOL show_oftendisp 是否设置常显
    输出参数：
    函数返回值：  BOOL：成功返回TRUE，失败返回FALSE
    内部处理流程：
********************************************************************/
BOOL Initialize(char** appName, BOOL show_oftendisp)
{
    // 快排页面库数组
    QuickSortPlugin(0, g_nPluginCnt-1);
    sys_sort_insert(0, g_nPluginCnt-1);
    if(appName == NULL)
    {
        g_guiMain.nLightonTime = 0;
        g_guiMain.bLighton = TRUE;
        // 点亮背光灯
        lighton_lcd();
    }
    

    /*获取公用的信息*/
    // 获取16*16 宋体
    g_guiComm.fontBig = GetSystemFont(SYSLOGFONT_MENU);
    // 获取12*12 宋体，因为在MiniCFG.ini的配置中，使用12*12为缺省字体
    g_guiComm.fontSmall = NULL;

    //初始化数据字典加载
    // 1、加入数据字典的头文件mem.h，data_id.h，para_id.h
    //    因为本文件为cpp文件，使用C++编译器连接，因此mem.h中需要加入extern "C"
    // 2、在makefile文件中加入对libMemManager.so的连接引用
    // 3、初始化数据字典
    #if SpeBOOTSCREEN==0
    //int nRet = InitMem(TRUE, TRUE);
    int nRet = init_mem();
    if(nRet < 0)
    {
        GUI_FMT_TRACE("InitMem Failed! Ret = %d\n", nRet);
    }
    #endif

    reg_pdata_xram(ADV_APP_NAME, GUI_PRIVATE_DATA);
    reg_pdata_xram(ADV_APP_NAME, GUI_CONFIG_DATA);
    reg_pdata_xram(ADV_APP_NAME, EVENT_PRIVATE_FILE);
    reg_pdata_xram(ADV_APP_NAME, GUI_PARA);

    //RamDisk文件操作模块加载

    //背光灯操控模块加载
    // 加载背光灯模块，点亮背光灯

    //初始化IPC加载
    // 1、包含ipc.h文件
    //    因为本文件为cpp文件，使用C++编译器连接，因此ipc.h中需要加入extern "C"
    // 2、在makefile文件中加入对libipc.a的连接引用
    // 3、打开IPC通道
    //g_nIpcRet = OpenIPC(GUI_PORT_ID, IPC_SERVICE);
    //init_ipc_msg();

    /*读取xml操作*/
    /*包含读取动态菜单，滚动界面信息等*/
    // 生成菜单时会分配内存，在Uninitialize中释放
    //if(ReadConfig(g_bAllMenu) == FALSE)
    {
//      ReadConfig(g_bAllMenu);
        RestoreConfigFromXml(appName);
        ReadConfig(g_bAllMenu);

        //GUI_FMT_DEBUG("=====行号: %d, g_guiMain.nMenuRootId = 0x%08x\n",__LINE__,g_guiMain.nMenuRootId);
    }

    GUI_FMT_DEBUG("read config success!\n");
    if(g_nMenus <= 0)
    {
        ASSERT(FALSE);
        return FALSE;
    }

    // 根据主菜单风格ID，确定当前的plug
//    if(g_bAllMenu == TRUE)
//    {
//        g_guiMain.pluginCur = GetPlugIn(0x00A00000);
//
//    }
//    else
//    {
        g_guiMain.pluginCur = GetPlugIn(g_guiMain.nMenuRootId);
//
//    }


    // 初始化当前页面库，在Uninitialize中释放
    ASSERT(g_guiMain.pluginCur != NULL);
    g_guiComm.bCanTrunDisp = TRUE;
    g_guiComm.nArrow = -1;
#if BEIJING_JZQ_EN == 1
    g_guiComm.sCaption[0] = '\0';
    g_guiComm.NodeAddrPage = 0;
    g_guiComm.NodeAddrIndex = 0;
#endif
    if(g_guiMain.pluginCur->pfnInitLayer != NULL)
    {
        g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
    }

    // 初始化背景页面库，在Uninitialize中释放
    if(g_guiMain.plugTemplate != NULL && g_guiMain.plugTemplate->pfnInitLayer != NULL)
    {
        g_guiMain.plugTemplate->pfnInitLayer(g_guiMain.plugTemplate->dwId);
    }
    // 初始化弹出页面库，在Uninitialize中释放
    if(g_guiMain.plugPop != NULL && g_guiMain.plugPop->pfnInitLayer != NULL)
    {
        g_guiMain.plugPop->pfnInitLayer(g_guiMain.plugPop->dwId);
    }

    //常显界面
    if(show_oftendisp == TRUE)
    {
        g_guiMain.bOftenDisp = TRUE;

        FILE *fp = fopen(TERMINAL_TYPE,"r");
        if(fp != NULL)    //专变
        {
            g_guiMain.plugOften = GetPlugIn(0x0000ffff);
            fclose(fp);
            
        }
        else                                //公变
        {
#if defined PRODUCT_CCU && defined AREA_CHONGQING
            g_guiMain.plugOften = GetPlugIn(0x00A00102);
#else
            g_guiMain.plugOften = GetPlugIn(0x00000fff);
#endif
        }
        
        ASSERT(g_guiMain.plugOften != NULL);
    }
        
    
    return TRUE;
}
BOOL Uninitialize()
{
#ifndef _MSC_VER
    /*关闭进程间通讯*/
    //CloseIPC(GUI_PORT_ID);
#endif

    // 释放当前页面库
    ASSERT(g_guiMain.pluginCur != NULL);
    if(g_guiMain.pluginCur->pfnUninitLayer != NULL)
    {
        g_guiMain.pluginCur->pfnUninitLayer();
    }
    // 释放背景页面库
    if(g_guiMain.plugTemplate != NULL && g_guiMain.plugTemplate->pfnUninitLayer != NULL)
    {
        g_guiMain.plugTemplate->pfnUninitLayer();
    }
    // 释放弹出页面库
    if(g_guiMain.plugPop != NULL && g_guiMain.plugPop->pfnUninitLayer != NULL)
    {
        g_guiMain.plugPop->pfnUninitLayer();
    }

    // 释放菜单资源
    MENU_INFO* pMenu;
    for(int i = 0; i < g_nMenus; i ++)
    {
        pMenu = g_menuMain[i];
        ASSERT(pMenu != NULL);
        if(pMenu->bMenuAggregate == FALSE)
        {
            free(pMenu->pPlugStruct);
        }
        free(pMenu);
    }
    g_nMenus = 0;
    return TRUE;
}

/*********************************************************************
    所属功能单元：显示框架模块
    功能描述：    缺省的页面库释放函数，设置测量点为0，可用于缺省的页面库回调
    输入参数：    DWORD dwPluginId：页面库ID
    输出参数：
    函数返回值：  BOOL ：TRUE表示成功执行，FALSE表示失败
    内部处理流程：
********************************************************************/
void UninitDefault()
{
    g_guiComm.nCurPoint = 0;
    set_gui_infonum((uint16)g_guiComm.nCurPoint);
}

//HWND g_scrollwnd;

void delete_app_info(uint32 appid)
{
    
    APP_GUI_INFO_T* pAppLoginInfo = NULL;
    int i,j;
    
    char strbuf[128] = {0};
    
    pthread_mutex_lock(&appGuiInfoLock);
    pAppLoginInfo = &appGuiInfoSaved;
    for(i=0; i<pAppLoginInfo->loginAppCnt;i++)
    {
        if(pAppLoginInfo->loginID[i] == appid)
        {
            g_bpop = TRUE;
            sprintf(strbuf, "     %s已下线！", pAppLoginInfo->applogin[i].shortname);
            PopMsg(strbuf);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            sleep(1);
            g_bpop = FALSE;
            
            for(j=i; j<pAppLoginInfo->loginAppCnt-1;j++)
            {
                pAppLoginInfo->loginID[j] = pAppLoginInfo->loginID[j+1];
//                pAppLoginInfo->menuID[j] = pAppLoginInfo->menuID[j+1];
                pAppLoginInfo->applogin[j] = pAppLoginInfo->applogin[j+1];
            }
            memset(&pAppLoginInfo->applogin[pAppLoginInfo->loginAppCnt-1], 0, sizeof(APP_REGIST_INFO_T));
            pAppLoginInfo->loginAppCnt -= 1;
            save_app_info();
            
            //刷新桌面，加载app
            load_app_icon(FALSE);       //载入其他app桌面信息
            break;
        }
    }
    pthread_mutex_unlock(&appGuiInfoLock);
    return ;
}

/**********************************************************************
* @name      :  OnPaintOper
* @brief     ： 主显示区画面绘制函数，框架本身并不实现绘制，通过调用页面库实现
* @param[in] ： 
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-11
* @Update    :
**********************************************************************/
void OnPaintOper(HDC hdc)
{

    //弹显页面控制，有按键或显示时间超时时，关闭弹显页面
    if((g_pop_mes_Fresh.is_pop == TRUE && g_pop_mes_Fresh.dispEnd != 0 &&  time(NULL) > g_pop_mes_Fresh.dispEnd))
    {
        GUI_FMT_DEBUG("ClosePopLayer\n");
        g_pop_mes_Fresh.is_pop = FALSE;
        ClosePopLayer();
    }

    //当终端有消息框或提示框需要显示时，常显界面关闭
    if(g_guiMain.plugMsg != NULL || g_guiMain.plugPop != NULL)
    {
        g_bOftenDisp = FALSE;
    }

    // 判断是否处于常显状态
    if(g_bOftenDisp == TRUE && g_guiMain.bOftenDisp == TRUE&& g_guiMain.plugOften != NULL)
    {
        ASSERT(g_guiMain.plugOften->pfnDraw != NULL);
        g_guiMain.plugOften->pfnDraw(hdc);
    }
    else
    {
    	// 判断是否处于轮显状态
	    if(g_guiMain.bTrunDisp == TRUE && g_guiMain.nTrunDispCnt > 0)
	    {
	        // 画轮显页面库
	        PLUGIN_INFO* pPlugin;
	        ASSERT(g_guiMain.nTrunDispIndex >= 0);
	        pPlugin = g_pluginTrunDisp[g_guiMain.nTrunDispIndex];
	        ASSERT(pPlugin != NULL && pPlugin->pfnDraw != NULL);
	        pPlugin->pfnDraw(hdc);
	    }
        else if(g_guiMain.plugMsg != NULL)   
        {//判断是否存在消息框页面库 优先处理
            ASSERT(g_guiMain.plugMsg->pfnDraw != NULL);
            g_guiMain.plugMsg->pfnDraw(hdc);
        }
        else if(g_guiMain.plugPop != NULL)
        {// 判断是否存在pop页面库 优先处理弹出页面库
            
            g_guiMain.nTrunDispTime = 0;        //弹显的时候，不轮显
            
            ASSERT(g_guiMain.plugPop->pfnDraw != NULL);
            g_guiMain.plugPop->pfnDraw(hdc);

        }
        else
        {
            // 画功能页面库
            if(g_guiMain.pluginCur != NULL)
            {
                g_guiMain.pluginCur->pfnDraw(hdc);
            }
        }

    }
}


/**********************************************************************
* @name      :  OnPaint
* @brief     ： LED显示画面绘制函数，框架本身并不实现绘制，通过调用页面库实现
* @param[in] ： 
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-11
* @Update    :
**********************************************************************/
void OnPaint()
{
	//屏显控制，
	if(g_sceenCtr.screenType != SCREEN_EXIT)
	{
		gui_dev->lcd_brush_region(gui_dev, 0, 0, g_guiComm.nWidth-1, g_guiComm.nHeight-1, g_sceenCtr.pointBuf, MAX_POINT_LEN);
		return ;
	}
    HDC hdcDst=BeginPaint(g_guiComm.hMainWnd);
    HDC hdc = CreateCompatibleDC(hdcDst);
    //桌面显示控制主显示区时
    if(getAuthor() == 1 || activeID == 0)
    {
        FillBox(hdc, 0, 0, g_guiComm.nWidth, g_guiComm.nHeight);
        OnPaintOper(hdc);
        // 绘制顶层显示状态栏、底层显示状态栏
        if(g_bLogo == FALSE && g_guiMain.plugTemplate != NULL)
        {
            ASSERT(g_guiMain.plugTemplate->pfnDraw != NULL);
            g_guiMain.plugTemplate->pfnDraw(hdc);
        }
        BitBlt(hdc, 0, 0, g_guiComm.nWidth, g_guiComm.nHeight, hdcDst, 0, 0, 0);
        DeleteCompatibleDC(hdc);
        EndPaint(g_guiComm.hMainWnd,hdcDst);
        LCD_Refresh( gui_dev, 0, 0, g_guiComm.nWidth-1, g_guiComm.nHeight-1);

        return;
    }
    
}
#if DESC("地区标识写铁电",1)

/**********************************************************************
* @name      : area_info_save
* @brief     ：地区标识写铁电，供系统识别，江西地区上电时显示“国网 江西省电力有限公司”
				铁电第81个字节，江西地区 写P 字母
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
int area_info_save(void)
{
    uint8 areaInfo = 0;
    /* MAC保存在铁电 */
    EEPROM_DEVICE_T* dev = (EEPROM_DEVICE_T*)hal_device_get(HW_DEVICE_ID_EEPROM);
    int  ret = 0;

    if (NULL == dev)
    {
        GUI_FMT_DEBUG("eeprom_write null\n");
        return -1;
    }

    /* 关闭写保护 */
    dev->eeprom_write_protect_enable(dev, 0);

//江西地区 铁电第81个字节，写P 字母
#ifdef AREA_JIANGXI
	areaInfo = 'P';
#endif

    ret = dev->eeprom_write(dev, &areaInfo, 81, 1);
    if (ret < 0)
    {
        GUI_FMT_DEBUG("eeprom_write err = %d\n", ret);
    }
    /* 打开写保护 */
    dev->eeprom_write_protect_enable(dev, 1);
    hal_device_release((HW_DEVICE *)dev);

    return 0;
}

#endif

#if DESC("轮显功能",1)
/**********************************************************************
* @name      : ReadTrunDisp
* @brief     ：读取轮显APP信息
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
void ReadTrunDisp()
{
    g_guiMain.nTrunDispCnt = 0;

    // 读取轮显参数
    char czBuf[MAX_TRUNDISP_CNT*4+2];
    //int nRet = ReadXPara(TMN_RUN_PARA, Turn_Menu_Num_Time, czBuf, 0);
    int nRet = read_pdata_xram(GUI_PARA, czBuf, Turn_Num_Time, Turn_Num_Time_Len);
    if(nRet < 0 || (unsigned char)czBuf[0] == NONE_CHAR)
    {
        return;
    }
    g_guiMain.nTrunDispTimeMax = (unsigned char)czBuf[1];
    g_guiMain.nInterval = (unsigned char)czBuf[2];
    int nCnt = (unsigned char)czBuf[0];
    if(nCnt > MAX_TRUNDISP_CNT)nCnt = MAX_TRUNDISP_CNT;
    g_nTotalTrunDispCnt = nCnt;

    // 读取轮显页面库ID
    //nRet = ReadXPara(TMN_RUN_PARA, Turn_Menu_IDs, czBuf, 0);
    nRet = read_pdata_xram(GUI_PARA, czBuf, Turn_IDs, Turn_IDs_Len);
    if(nRet < 0)
    {
        return;
    }
    char* pBuf = czBuf;
    PLUGIN_INFO* pPlugin;
    for(int i = 0; i < nCnt; i ++)
    {
        g_nTrunDisp[i] = chartoint32(pBuf);
        //printf("Disp:%x", g_nTrunDisp[i]);
        pPlugin = GetPlugIn(g_nTrunDisp[i]);
        ASSERT(pPlugin != NULL);
        if(pPlugin != NULL)
        {
            g_pluginTrunDisp[g_guiMain.nTrunDispCnt] = pPlugin;
            g_guiMain.nTrunDispCnt ++;
        }
        pBuf += 4;
    }
}

/**********************************************************************
* @name      : WriteTrunDisp
* @brief     ：参加轮显APP信息写入私有文件
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
void WriteTrunDisp()
{
    char czBuf[MAX_TRUNDISP_CNT*4+2];
    czBuf[0] = (unsigned char)g_nTotalTrunDispCnt;
    czBuf[1] = (unsigned char)g_guiMain.nTrunDispTimeMax;
    czBuf[2] = (unsigned char)g_guiMain.nInterval;
    int nRet = write_pdata_xram(GUI_PARA, czBuf, Turn_Num_Time, Turn_Num_Time_Len);
    ASSERT(nRet >= 0);

    char* pBuf = czBuf;
    for(int i = 0; i < g_nTotalTrunDispCnt; i ++)
    {
        int32tochar(g_nTrunDisp[i], pBuf);
        pBuf += 4;
    }
    nRet = write_pdata_xram(GUI_PARA, czBuf, Turn_IDs, Turn_IDs_Len);
    ASSERT(nRet >= 0);
}

void AddTrunDisp()
{
    if(g_guiComm.bCanTrunDisp == TRUE)
    {// 允许加入到轮显页面库中
        if(g_nTotalTrunDispCnt >= MAX_TRUNDISP_CNT)
        {
            char czBuf[100];
            sprintf(czBuf, "轮显页面已经达到%d个页面\n不能再增加轮显页面", MAX_TRUNDISP_CNT);
            MessageBox(czBuf);
            return;
        }
        if(ValidatePwd() == TRUE)
        {
            DWORD dwCurId = g_guiMain.pluginCur->dwId;
            for(int i =0; i < g_nTotalTrunDispCnt; i ++)
            {
                if((DWORD)g_nTrunDisp[i] == dwCurId)
                {
                    MessageBox("当前页面已经存在\n于轮显页面库中");
                    return;
                }
            }
            g_nTrunDisp[g_nTotalTrunDispCnt] = dwCurId;
            g_nTotalTrunDispCnt ++;
            PLUGIN_INFO* plugin = GetPlugIn(dwCurId);
            if(plugin != NULL)
            {
                g_pluginTrunDisp[g_guiMain.nTrunDispCnt] = plugin;
                g_guiMain.nTrunDispCnt ++;
            }
            WriteTrunDisp();
            MessageBox("当前页面已经加入\n到轮显页面库中");
            return;
        }
    }
    else
    {// 不允许加入到轮显页面库中
        MessageBox("该页面不允许加入到\n轮显页面中");
    }
}

/**********************************************************************
* @name      : StrartTrunDisp
* @brief     ：桌面APP启动轮显功能
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
void StrartTrunDisp()
{
    ReadTrunDisp();

    g_guiMain.nTrunDispIndex = 0;
	if(g_guiMain.nTrunDispCnt <= 0)
	{
		g_guiMain.bTrunDisp = TRUE;
//		g_guiMain.nLightonTime = 0;
        StopTrunDisp();
		return;
	}

    g_bOftenDisp = FALSE;
    g_guiMain.bOftenDisp = FALSE;

    // 如果有弹出页面库
    if(g_guiMain.plugPop != NULL)
    {
        // 弹出页面库时,不能有MessageBox
        ASSERT(g_guiMain.plugMsg == NULL);
        ClosePopLayer();
    }

    g_guiComm.nCurPointTmp = g_guiComm.nCurPoint;
    g_guiComm.bCanTrunDispTmp = g_guiComm.bCanTrunDisp;
    g_guiComm.nCurPoint = 0;
    g_guiComm.nIndexPointTmp = g_guiComm.nIndexPoint;
    g_guiComm.bPointOffsetTmp = g_guiComm.bPointOffset;
    g_guiComm.nArrowTmp = g_guiComm.nArrow;
    g_guiComm.nArrow = -1;

    

    // 初始化新的轮显页面库
    PLUGIN_INFO* pPlugin;
    pPlugin = g_pluginTrunDisp[g_guiMain.nTrunDispIndex];
    ASSERT(pPlugin != NULL);
    if(pPlugin->pfnInitLayer != NULL)
    {
		if(pPlugin != g_guiMain.pluginCur)
		{
			pPlugin->pfnInitLayer(pPlugin->dwId);
		}
		else
		{
			g_guiComm.nCurPoint = g_guiComm.nCurPointTmp;
			g_guiComm.bCanTrunDisp = g_guiComm.bCanTrunDispTmp;
			g_guiComm.nIndexPoint = g_guiComm.nIndexPointTmp;
			g_guiComm.bPointOffset = g_guiComm.bPointOffsetTmp;
			g_guiComm.nArrow = g_guiComm.nArrowTmp;
		}
	}
    
    // nTrunDispTime作为两个用途，如果没有进入轮显，则作为判定进入轮显最大时间值
    // 否则作为两个轮显页面的间隔值判定
	g_guiMain.bTrunDisp = TRUE;
	//g_guiMain.nTrunDispIndex = 0;
	g_guiMain.nTrunDispTime = 0;
    
    UpdateWindow(g_guiComm.hMainWnd, TRUE);
}

/**********************************************************************
* @name      : StopTrunDisp
* @brief     ：桌面APP停止轮显功能
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
void StopTrunDisp()
{
	if(g_guiMain.bTrunDisp == TRUE)
    {
        g_guiMain.bTrunDisp = FALSE;
        g_guiMain.nTrunDispTime = 0;
        if(g_guiMain.nTrunDispCnt > 0 && g_guiMain.nTrunDispIndex >= 0)
        {// 取消轮显
            PLUGIN_INFO* pPlugin = g_pluginTrunDisp[g_guiMain.nTrunDispIndex];
            ASSERT(pPlugin != NULL);
            if(pPlugin->pfnUninitLayer != NULL && pPlugin != g_guiMain.pluginCur)
            {
                pPlugin->pfnUninitLayer();
            }
            g_guiMain.nTrunDispIndex = -1;
            g_guiComm.nCurPoint = g_guiComm.nCurPointTmp;
            g_guiComm.bCanTrunDisp = g_guiComm.bCanTrunDispTmp;
            g_guiComm.nIndexPoint = g_guiComm.nIndexPointTmp;
            g_guiComm.bPointOffset = g_guiComm.bPointOffsetTmp;
            g_guiComm.nArrow = g_guiComm.nArrowTmp;
			
			if(g_guiMain.pluginCur->pfnInitLayer != NULL)
			{
				g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
			}
        }

        GUI_FMT_TRACE("退出轮显\n");
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
    }
    
}

/**********************************************************************
* @name      : KeepNextTrunDisp
* @brief     ：桌面APP切换轮显APP
* @param[in] ：BOOL bPre    切换方式，TRUE 倒序切换
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
void KeepNextTrunDisp(BOOL bPre)
{
    if(g_guiMain.bTrunDisp == FALSE)
    {
        return;
    }
	PLUGIN_INFO* pPlugin;
	ASSERT(g_guiMain.nTrunDispIndex >= 0);
	// 释放旧的轮显页面库
	pPlugin = g_pluginTrunDisp[g_guiMain.nTrunDispIndex];
	ASSERT(pPlugin != NULL);
	if(pPlugin->pfnUninitLayer != NULL)
	{
		if(pPlugin != g_guiMain.pluginCur)
		{
			pPlugin->pfnUninitLayer();
		}
	}

	    // 设置新的轮显页面
    if(bPre == TRUE)
    {
        g_guiMain.nTrunDispIndex --;
        if(g_guiMain.nTrunDispIndex < 0)
        {
            g_guiMain.nTrunDispIndex = g_guiMain.nTrunDispCnt-1;
        }
    }
    else
    {
        g_guiMain.nTrunDispIndex ++;
        if(g_guiMain.nTrunDispIndex >= g_guiMain.nTrunDispCnt)
        {
            g_guiMain.nTrunDispIndex = 0;
        }
    }

	// 初始化新的轮显页面库
	if(g_guiMain.nTrunDispCnt > 0)
	{
		pPlugin = g_pluginTrunDisp[g_guiMain.nTrunDispIndex];
		ASSERT(pPlugin != NULL);
		g_guiComm.nCurPoint = 0;
		if(pPlugin->pfnInitLayer != NULL)
		{
			if(pPlugin != g_guiMain.pluginCur)
			{
				pPlugin->pfnInitLayer(pPlugin->dwId);
			}
			else
			{
				g_guiComm.nCurPoint = g_guiComm.nCurPointTmp;
				g_guiComm.bCanTrunDisp = g_guiComm.bCanTrunDispTmp;
				g_guiComm.nIndexPoint = g_guiComm.nIndexPointTmp;
				g_guiComm.bPointOffset = g_guiComm.bPointOffsetTmp;
				g_guiComm.nArrow = g_guiComm.nArrowTmp;
			}
		}
	}
	g_guiMain.nTrunDispTime = 0;

}

#endif

void SetSubLayerArrow()
{
    g_guiComm.nArrow = -1;
    if(g_guiMain.pPrentMenu != NULL && g_guiMain.pPrentMenu->bMenuAggregate == FALSE)
    {
        if(g_guiMain.pPrentMenu->nPlugCnt <= 1
            || g_guiMain.pluginCur == NULL)
        {
            return;
        }

        BOOL bNext = FALSE;
        BOOL bPre = FALSE;
        int i;
        DWORD dwPluginId;
        PLUGIN_INFO* plugin;
        PLUG_STRUCT* pPlugStruct;
        for(i = g_guiMain.nPageLayerIndex-1; i >= 0; i --)
        {
            //dwPluginId = g_guiMain.pPrentMenu->pdwPlugin[i];
            pPlugStruct = &g_guiMain.pPrentMenu->pPlugStruct[i];
            dwPluginId = pPlugStruct->dwPlugin;
            plugin = GetPlugIn(dwPluginId);
            ASSERT(plugin != NULL);
            if(plugin == NULL)
            {
                continue;
            }

            g_guiComm.bCanTrunDisp = TRUE;
            g_guiComm.bSkipLayer = FALSE;
            g_guiComm.bPointOffset = FALSE;
            g_guiComm.nArrow = -1;
            g_guiComm.bSkipLayer = FALSE;
            if(plugin->pfnInitLayer != NULL)
            {
                plugin->pfnInitLayer(plugin->dwId);
            }
            // 判断是否需要跳过该页面
            if(g_guiComm.bSkipLayer == TRUE)
            {
                continue;
            }
            bPre = TRUE;
            break;
        }
        for(i = g_guiMain.nPageLayerIndex+1; i < g_guiMain.pPrentMenu->nPlugCnt; i ++)
        {
            //dwPluginId = g_guiMain.pPrentMenu->pdwPlugin[i];
            pPlugStruct = &g_guiMain.pPrentMenu->pPlugStruct[i];
            dwPluginId = pPlugStruct->dwPlugin;
            plugin = GetPlugIn(dwPluginId);
            ASSERT(plugin != NULL);
            if(plugin == NULL)
            {
                continue;
            }

            g_guiComm.bCanTrunDisp = TRUE;
            g_guiComm.bSkipLayer = FALSE;
            g_guiComm.bPointOffset = FALSE;
            g_guiComm.nArrow = -1;
            g_guiComm.nCurPoint = 0;
            if(plugin->pfnInitLayer != NULL)
            {
                plugin->pfnInitLayer(plugin->dwId);
            }
            // 判断是否需要跳过该页面
            if(g_guiComm.bSkipLayer == TRUE)
            {
                continue;
            }
            bNext = TRUE;
            break;
        }
        g_guiComm.bCanTrunDisp = TRUE;
        g_guiComm.bSkipLayer = FALSE;
        g_guiComm.bPointOffset = FALSE;
        g_guiComm.nArrow = -1;
        g_guiComm.nCurPoint = 0;
        if(g_guiMain.pluginCur->pfnInitLayer != NULL)
        {
            g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
        }

        if(bPre == TRUE && bNext == TRUE)
        {
            g_guiComm.nArrow = 0;
        }
        else if(bPre == TRUE)
        {
            g_guiComm.nArrow = 1;
        }
        else if(bNext == TRUE)
        {
            g_guiComm.nArrow = 2;
        }
        //printf("g_guiComm.nArrow is %d\n", g_guiComm.nArrow);
        /*if(g_guiMain.nPageLayerIndex > 0 && g_guiMain.nPageLayerIndex < g_guiMain.pPrentMenu->nPlugCnt-1)
        {
            g_guiComm.nArrow = 0;
        }
        else if(g_guiMain.nPageLayerIndex > 0)
        {
            g_guiComm.nArrow = 1;
        }
        else if(g_guiMain.nPageLayerIndex < g_guiMain.pPrentMenu->nPlugCnt-1)
        {
            g_guiComm.nArrow = 2;
        }
        else
        {
            g_guiComm.nArrow = -1;
        }*/
    }

}
void CancelMenu()
{
    PLUGIN_INFO* pPlugin;
    if(g_nParentIndex == -1)
    {
        pPlugin = GetPlugIn(g_guiMain.nMenuRootId);
//        return;
    }
    else
    {
        //pPlugin = GetPlugIn(g_menuMain[g_nParentIndex]->pdwPlugin[0]);
        pPlugin = GetPlugIn(g_menuMain[g_nParentIndex]->dwMenuID);
    }
    ASSERT(pPlugin != NULL);
    if(pPlugin == NULL)
    {
        ASSERT(FALSE);
        return;
    }

    if(g_guiMain.pluginCur != NULL && g_guiMain.pluginCur->pfnUninitLayer != NULL)
    {
        g_guiMain.pluginCur->pfnUninitLayer();
    }
    g_guiMain.pluginCur = pPlugin;

    g_guiComm.bCanTrunDisp = TRUE;
    g_guiComm.nArrow = -1;
    g_guiComm.nCurPoint = FALSE;
    if(g_guiMain.pluginCur->pfnInitLayer != NULL)
    {
        g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
    }

    g_nMenuNodes --;
    if (g_nMenuNodes < 1)
    {
        g_nMenuNodes = 1;
    }
    UpdateWindow(g_guiComm.hMainWnd, TRUE);
}
void KeepNextLayer()
{
    DWORD dwPluginId;
    PLUG_STRUCT* pPlugStruct;
    g_guiComm.bKeyDown = TRUE;
    if(g_guiMain.pPrentMenu != NULL && g_guiMain.pPrentMenu->bMenuAggregate == FALSE)
    {
        if(g_guiMain.pPrentMenu->nPlugCnt <= 1)
        {
            return;
        }
        if(g_guiMain.pluginCur->pfnUninitLayer != NULL)
        {
            g_guiMain.pluginCur->pfnUninitLayer();
        }
        g_guiMain.pluginCur = NULL;

        int nCircle = 0;
        while(g_guiMain.pluginCur == NULL)
        {
            g_guiMain.nPageLayerIndex ++;
            if(g_guiMain.nPageLayerIndex >= g_guiMain.pPrentMenu->nPlugCnt)
            {
                g_guiMain.nPageLayerIndex = 0;
                nCircle ++;
                if(nCircle == 2)
                {
                    CancelMenu();
                    break;
                }
            }
            //dwPluginId = g_guiMain.pPrentMenu->pdwPlugin[g_guiMain.nPageLayerIndex];
            pPlugStruct = &g_guiMain.pPrentMenu->pPlugStruct[g_guiMain.nPageLayerIndex];
            dwPluginId = pPlugStruct->dwPlugin;

            g_guiMain.pluginCur = GetPlugIn(dwPluginId);
            ASSERT(g_guiMain.pluginCur != NULL);
            if(g_guiMain.pluginCur == NULL)
            {
                continue;
            }

            g_guiComm.bCanTrunDisp = TRUE;
            g_guiComm.bSkipLayer = FALSE;
            g_guiComm.bPointOffset = FALSE;
            g_guiComm.nArrow = -1;
            g_guiComm.nCurPoint = 0;
            if(g_guiMain.pluginCur->pfnInitLayer != NULL)
            {
                g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
            }
            // 判断是否需要跳过该页面
            if(g_guiComm.bSkipLayer == TRUE)
            {
                if(g_guiMain.pluginCur->pfnUninitLayer != NULL)
                {
                    g_guiMain.pluginCur->pfnUninitLayer();
                }
                g_guiMain.pluginCur = NULL;
            }
        }
        g_narrMenuIndex[g_nMenuNodes-1] = g_guiMain.nPageLayerIndex;
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
    }
}

#include <algorithm>
#include <vector>

using namespace std;

class KeySequenceRecorder
{
    public:
    KeySequenceRecorder(const vector<int> &special_key_sequence)
    {
        this->special = special_key_sequence;
    }

    public:
    KeySequenceRecorder(const int special_key_sequence[], unsigned int n)
    {
        this->special = vector<int>(special_key_sequence, special_key_sequence + n);
    }

    public:
    void Record(int nKey)
    {
        current.push_back(nKey);
        if (current.size() > special.size())
        {
            current.erase(current.begin());
        }
    }

    public:
    void ClearKeyRecords(void)
    {
        current.clear();
    }

    public:
    BOOLEAN SpecialKeySequenceIsRecognized(void) // 特殊按键序列被识别出来时返回true
    {
        vector<int>::iterator pos;
        pos = search(current.begin(), current.end(), special.begin(), special.end());
        if (pos != current.end())
        {
            fpdebug = stderr;
            if (fpdebug != NULL)
            {
                uint32 i;
                fprintf(fpdebug, "%s:%d:%s:已捕获到特殊的按键序列:\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
                for (i=0; i!=special.size(); i++)
                {
                    const char *keyname;
                    switch (special[i])
                    {
                        case KEY_UP:     keyname="上"; break;
                        case KEY_DOWN:   keyname="下"; break;
                        case KEY_LEFT:   keyname="左"; break;
                        case KEY_RIGHT:  keyname="右"; break;
                        case KEY_OK:     keyname="确认"; break;
                        case KEY_CANCEL: keyname="取消"; break;
                        default:         keyname="组合键"; break;
                    }
                    fprintf(fpdebug, " %s", keyname);
                }
                fprintf(fpdebug, "\n");
            }
            return (true);
        }
        return (false);
    }

    private:
    vector<int> current; // 当前已记录的按键序列
    vector<int> special;
    FILE *fpdebug;

};

extern PLUGIN_INFO g_layerHiddenLayer;
extern PLUGIN_INFO g_layerProductFeatureCfg;

void ShowLayerDirectly(PLUGIN_INFO *p)
{
    if(g_guiMain.pluginCur == p)
    {// 如果要弹出的页面与当前页面一致，就不再弹出
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return;
    }
    else
    {
        if (g_guiMain.bTrunDisp)
        {
            StopTrunDisp();
        }
        g_guiComm.bCanTrunDispTmp = g_guiComm.bCanTrunDisp;

        // 关闭旧页面
        if (g_guiMain.pluginCur->pfnUninitLayer != NULL)
            g_guiMain.pluginCur->pfnUninitLayer();
        // 进入新页面
        g_guiMain.pluginCur = p;
        g_guiComm.bCanTrunDisp = FALSE;
        g_guiComm.nArrow = -1;
        g_guiComm.nCurPoint = 0;
        if(g_guiMain.pluginCur->pfnInitLayer != NULL)
        {
            g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return;
    }
}

extern void ShowOrHideMenu();
extern void change_display_model();
extern void SetParaMenu();
/**********************************************************************
* @name      :  keyboard_thread
* @brief     ： 按键消息处理函数
* @param[in] ： nKey   键值
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-11
* @Update    :
**********************************************************************/
void OnKeyMsg(int nKey)
{
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;

    g_guiComm.nCurKey = nKey;
    if(g_bLogo == TRUE)
    {
        ClosePopLayer();
        g_bLogo = FALSE;
    }

    g_nPasswdTime = 0;
    g_guiMain.nTrunDispTime = 0;

    if(g_bAllMenu == TRUE)
    {
        g_allMenu_time = 0;
    }

    switch(nKey)
    {
    case KEY_MENU_EXT:        
        ShowOrHideMenu();
        return;
    }
    
    if(g_guiMain.pluginCur != NULL)
    {
        GUI_FMT_TRACE("pluginCur->->dwId---0x%08x\n",g_guiMain.pluginCur->dwId);
    }
    
    if(g_guiMain.plugMsg != NULL)
    {
        g_guiMain.plugMsg->pfnKeyMsg(nKey);
        return;
    }
    else if(g_guiMain.plugPop != NULL)
    {
        int nRet = RET_DEFAULT;
        if(g_guiMain.plugPop->pfnKeyMsg != NULL)
        {
            nRet = g_guiMain.plugPop->pfnKeyMsg(nKey);
            if(nRet == RET_OK)
            {
                return;
            }
        }
        if(nKey == KEY_CANCEL || nRet == RET_CANCEL)
        {// 退出POP页面库
            // 将pop库释放后，重新刷新界面，会自动进入当前页面库
            if(nRet == RET_CANCEL && g_guiMain.plugPop->dwId == g_layerValidatePwd.dwId)
            {
                ClosePopLayer();
                if(g_guiMain.pluginCur->pfnKeyMsg != NULL)
                {
                    g_guiMain.pluginCur->pfnKeyMsg(g_guiComm.nPwdKey);
                }
                if(KEY_ADDTRUN == g_guiComm.nPwdKey)
                {
                    AddTrunDisp();
                }
                if(KEY_MENU_EXT == g_guiComm.nPwdKey)
                {
                    ShowOrHideMenu();
                    return;
                }
            }
            else
            {
                ClosePopLayer();
            }
        }
        return;
    }
    else if(g_guiMain.pluginCur != NULL && g_guiMain.pluginCur->pfnKeyMsg != NULL)
    {
		if(g_guiMain.pluginCur->pfnKeyMsg(nKey) == TRUE)
		{
			return;
		}
		
    }
    // 使用缺省操作
    GUI_FMT_TRACE("#####使用缺省操作方案\n");
    switch(nKey)
    {
    case KEY_CANCEL:
        CancelMenu();
        break;
    case KEY_UP:
	case KEY_LEFT:
        {// 当前指向页面库集合时，指向上一个页面库
            DWORD dwPluginId;
            PLUG_STRUCT* pPlugStruct;
            g_guiComm.bKeyDown = FALSE;

            if(g_guiMain.pPrentMenu != NULL && g_guiMain.pPrentMenu->bMenuAggregate == FALSE)
            {
                if(g_guiMain.pPrentMenu->nPlugCnt <= 1)
                {
                    break;
                }
                if(g_guiMain.pluginCur->pfnUninitLayer != NULL)
                {
                    g_guiMain.pluginCur->pfnUninitLayer();
                }
                g_guiMain.pluginCur = NULL;

                int nCircle = 0;
                while(g_guiMain.pluginCur == NULL)
                {
                    g_guiMain.nPageLayerIndex --;
                    if(g_guiMain.nPageLayerIndex < 0)
                    {
                        g_guiMain.nPageLayerIndex = g_guiMain.pPrentMenu->nPlugCnt-1;
                        nCircle ++;
                        if(nCircle == 2)
                        {
                            CancelMenu();
                            break;
                        }
                    }
                    //dwPluginId = g_guiMain.pPrentMenu->pdwPlugin[g_guiMain.nPageLayerIndex];
                    pPlugStruct = &g_guiMain.pPrentMenu->pPlugStruct[g_guiMain.nPageLayerIndex];
                    dwPluginId = pPlugStruct->dwPlugin;

                    g_guiMain.pluginCur = GetPlugIn(dwPluginId);
                    ASSERT(g_guiMain.pluginCur != NULL);
                    if(g_guiMain.pluginCur == NULL)
                    {
                        continue;
                    }

                    g_guiComm.bCanTrunDisp = TRUE;
                    g_guiComm.bSkipLayer = FALSE;
                    g_guiComm.bPointOffset = FALSE;
                    g_guiComm.nArrow = -1;
                    g_guiComm.nCurPoint = 0;
                    if(g_guiMain.pluginCur->pfnInitLayer != NULL)
                    {
                        g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
                    }
                    // 判断是否需要跳过该页面
                    if(g_guiComm.bSkipLayer == TRUE)
                    {
                        if(g_guiMain.pluginCur->pfnUninitLayer != NULL)
                        {
                            g_guiMain.pluginCur->pfnUninitLayer();
                        }
                        g_guiMain.pluginCur = NULL;
                    }
                }
                SetSubLayerArrow();
                g_narrMenuIndex[g_nMenuNodes-1] = g_guiMain.nPageLayerIndex;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
            }
        }
        break;
    case KEY_DOWN:
	case KEY_RIGHT:
        // 当前指向页面库集合时，指向下一个页面库
        KeepNextLayer();
        SetSubLayerArrow();
        break;
    default:break;
    }
}

extern BOOL CanUseUDisks();

/**********************************************************************
* @name      :  updateAPP
* @brief     ： 终端APP升级，U盘插入触发，检查升级
* @param[in] ： 
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2022-7-26
* @Update    :
**********************************************************************/
void *UdiskUpdateThread(void *arg)
{
    if (0 == access("/tmp/update", F_OK))
    {
        system("rm -rf /tmp/update");
    }

    if (0 != access("/tmp/update", F_OK))
    {
        if(0 != mkdir("/tmp/update", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            MessageBox("创建升级目录失败\n稍后再拷贝!");
            return NULL;
        }
    }

    if (0 != access("/tmp/udisk/package", F_OK))
    {
        MessageBox("U盘升级文件不存在\n请确认后再试!");
        return NULL;
    }
#ifdef PRODUCT_ZCU_1
            PopMsg("                 准备升级\n\n             开始拷贝升级文件\n\n             请勿操作终端或者拔出U盘");
#else
            PopMsg("          准备升级\n\n      开始拷贝升级文件\n\n  请勿操作终端或者拔出U盘");
#endif

    system("cp -r /tmp/udisk/package/* /tmp/update/");

    GUI_FMT_DEBUG("U盘升级文件拷贝成功，可以拔出U盘\n");

    if (0 != access("/tmp/update/setup.sh", F_OK))
    {
        MessageBox("升级脚本不存在\n请确认后再试!");
        return NULL;
    }
#ifdef PRODUCT_ZCU_1
    PopMsg("               开始升级\n\n             可以拔出U盘\n\n             升级时间较长\n\n             请勿操作终端");
#else
    PopMsg("          开始升级\n\n        可以拔出U盘\n\n        升级时间较长\n\n        请勿操作终端");
#endif
    system("chmod 755 -R /tmp/update");
    chmod("/tmp/update/setup.sh", 0777);
    system("/tmp/update/setup.sh > /data/app/desktopGui/log/udisk.log");
    GUI_FMT_DEBUG("升级成功,重启终端\n");
    return NULL;
}

/**********************************************************************
* @name      :  updateAPP
* @brief     ： 终端APP升级，U盘插入触发，检查升级
* @param[in] ： 
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2022-7-26
* @Update    :
**********************************************************************/
void updateAPP()
{
    static uint8 udiskFlag = 0;
    static uint8 lastUdiskFlag = 0;
    int errCode = 0;
    pthread_t threadId;

    if(CanUseUDisks() == TRUE)
    {
        udiskFlag = 1;
    }
    else
    {
        udiskFlag = 0;
    }

    if(udiskFlag == 1 && lastUdiskFlag == 0)	//U盘插入
    {
        GUI_FMT_DEBUG("检测到有U盘插入，判断是否需要升级\n");
        lastUdiskFlag = udiskFlag;
        
        if (0 != access("/tmp/udisk/package", F_OK) || 0 != access("/tmp/udisk/package/setup.sh", F_OK))
        {
            GUI_FMT_DEBUG("U盘没有升级文件，不需要升级\n");
            return ;
        }
        if(g_pop_mes_Fresh.is_pop == TRUE)
        {
            ClosePopLayer();
        }
        g_guiMain.bLighton = TRUE;
        g_guiMain.nLightonTime = 0;
        lighton_lcd();
        g_bpop = TRUE;
        if(g_guiMain.bTrunDisp == TRUE)
        {
            StopTrunDisp();
        }
    
        if(TRUE == MessageBox("即将进行U盘升级\n是否确认？", MB_OKCANCEL))
        {
            /*创建一个线程,子线程执行拷贝等操作,主线程喂守护进程*/
            errCode = pthread_create(&threadId,NULL, UdiskUpdateThread, NULL);
            if (errCode != 0)
            {
                MessageBox("系统忙\n稍后再拷贝!");
                return ;
            }
            return ;
        }
    }

    lastUdiskFlag = udiskFlag;
}


/**********************************************************************
* @name      :  OnTimer
* @brief     ： 定时器控制函数，完成页面库的定时器分发。背光灯、轮显方面的操作
* @param[in] ： int nId：定时器ID
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-11
* @Update    :
**********************************************************************/
int g_nSpeak_cnt = 0;
void OnTimer(int nId)
{
    char czBuf[50];

    
    // 分发消息
    // 如果有弹出页面库，则发送定时器消息到弹出页面库
    // 否则发送消息到当前页面库
    if(g_guiMain.plugPop != NULL && g_guiMain.plugPop->pfnOnTimer != NULL)
    {
        g_guiMain.plugPop->pfnOnTimer(nId);
    }
    else if(g_guiMain.bTrunDisp == TRUE && appGuiInfoSaved.loginAppCnt > 0)
    {
        PLUGIN_INFO* pPlugin;
        ASSERT(g_guiMain.nTrunDispIndex >= 0);
        pPlugin = g_pluginTrunDisp[g_guiMain.nTrunDispIndex];
        if(pPlugin != NULL && pPlugin->pfnOnTimer != NULL)
        {
            pPlugin->pfnOnTimer(nId);
        }
    }
    else if(g_guiMain.pluginCur != NULL && g_guiMain.pluginCur->pfnOnTimer != NULL)
    {
        g_guiMain.pluginCur->pfnOnTimer(nId);
    }
    if(g_guiMain.plugTemplate != NULL && g_guiMain.plugTemplate->pfnOnTimer != NULL)
    {
        g_guiMain.plugTemplate->pfnOnTimer(nId);
    }

    if(g_pop_mes_Fresh.is_pop == TRUE && g_guiMain.bTrunDisp == TRUE)
    {
        StopTrunDisp();     //轮显时，收到弹显，先停止轮显
    }

    if(nId == TIMER_MAIN)
    {// 如果为主定时器时，需要执行背光灯、轮显页面的判定操作
        czBuf[0] = 1;
        if(czBuf[0] == 0)
        {
            g_nSpeak_cnt ++;
            if(g_nSpeak_cnt > 600)
            {
                g_nSpeak_cnt = 0;
                SpeakCtrl(FALSE);
            }
        }
        else
        {
            g_nSpeak_cnt = 0;
        }

        //本地APP启动时，异常点亮背光灯，特殊处理
        if(blkFlag>0 && g_guiMain.bLighton == FALSE)
        {
            blkFlag += 1;
        }
        if(blkFlag == 10 && g_guiMain.bLighton == FALSE)
        {
            lightoff_lcd();
        }

        // 背光灯定时器操作判定
        if(g_guiMain.bLighton == TRUE)
        {
            g_guiMain.nLightonTime++;
			#ifdef AREA_JIANGXI
            if(g_guiMain.nLightonTime > 30)
			#else
			if(g_guiMain.nLightonTime > g_guiMain.nLightonTimeMax)
			#endif
            {// 灯灭
                g_guiMain.bLighton = FALSE;
                // 熄灭背光灯

            }
        }

        //密码有效期，输入密码3分钟有效
        g_nPasswdTime ++;
        if(g_nPasswdTime > 180)
        {
            g_guiComm.bValidatePwd = FALSE;
        }

        //隐藏菜单模式超时切换回APP图标显示
        if(g_bAllMenu == TRUE)
        {
            g_allMenu_time ++;
            if(g_allMenu_time > 600)
            {
                change_display_model();
                g_allMenu_time = 0;
            }
        }
        else
        {
            g_allMenu_time = 0;
        }

        //按键3秒内不弹显
        if(g_pop_mes_Fresh.delayTime < 3)
            g_pop_mes_Fresh.delayTime++;

       // 轮显页面库操作判定
        if(g_guiMain.bTrunDisp == FALSE)
        {
            g_guiMain.nTrunDispTime ++;
            if(g_guiMain.nTrunDispTime > g_guiMain.nTrunDispTimeMax)
            {
                StrartTrunDisp();
            }
        }

		if(g_guiMain.bTrunDisp == TRUE)
		{// 在轮显模式下，判定当前轮显页面的时间，确定是否需要翻页
			g_guiMain.nTrunDispTime++;
			if(g_guiMain.nTrunDispCnt > 0 && g_guiMain.nTrunDispTime > g_guiMain.nInterval)
			{
				KeepNextTrunDisp();
			}
		} 

//        GUI_FMT_TRACE("recycle_permission_time = %d \n", recycle_permission_time);
  
        // 定时刷新页面库，因为页面需要实时刷新，比如信号值、时间值等，所以放在该处统一刷新。
        UpdateWindow(g_guiComm.hMainWnd, TRUE);

        updateAPP();

        if(g_bUpdate == TRUE)
        {
            //SendKeyFromIpc((uint8)START_UPGRADE_IR_KEY);
        }
    }
    else if(nId == TIMER_QUICK)
    {
        //DisposeIPC();
    }
}

/*********************************************************************
    所属功能单元：显示框架模块
    功能描述：    主要完成窗口的消息处理.
    输入参数：    HWND hWnd：窗口句柄，当前只有窗口，即hMainWnd
                  int message：消息ID,如MSG_PAINT
                  WPARAM wParam：消息对应的参数1
                  LPARAM lParam：消息对应的参数2
    输出参数：
    函数返回值：  int：成功返回0
    内部处理流程：
********************************************************************/
static int WinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {

        case MSG_CLOSE:
        case MSG_CHAR:
        case MSG_KEYUP:
        case MSG_PAINT:
        case MSG_TIMER:break;
        default:return DefaultMainWinProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


/*********************************************************************
    所属功能单元：显示框架模块
    功能描述：    定义以及创建窗口
    输入参数：
    输出参数：
    函数返回值：  BOOL：成功返回TRUE，否则返回FALSE
    内部处理流程：
********************************************************************/
BOOL InitMainWindow()
{//WS_VSCROLL |
    MAINWINCREATE window_info;
    window_info.dwStyle = WS_VISIBLE;
    window_info.dwExStyle = WS_EX_NONE;
    window_info.spCaption = "";
    window_info.hMenu = 0;
    window_info.hCursor = GetSystemCursor(0);
    window_info.hIcon = 0;
    window_info.MainWindowProc = WinProc;
    window_info.lx = 0;
    window_info.ty = 0;
    window_info.rx = g_guiComm.nWidth;
    window_info.by = g_guiComm.nHeight;
    GUI_FMT_TRACE("width %d--height %d\n", g_guiComm.nWidth, g_guiComm.nHeight);
    window_info.iBkColor = COLOR_lightwhite;
    window_info.dwAddData = 0;
    window_info.hHosting = HWND_DESKTOP;
    g_guiComm.hMainWnd = CreateMainWindow(&window_info);
    if(g_guiComm.hMainWnd == HWND_INVALID)
        return FALSE;
    return TRUE;
}

pthread_t   timer_thread_id;
pthread_t   gOutMsgFlag;

time_t g_nMainTimer = -1;
/**********************************************************************
* @name      :  timer_thread
* @brief     ： 定时器控制线程
* @param[in] ： 
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-11
* @Update    :
**********************************************************************/
void* timer_thread(void *arg)
{
    time_t t;
    while(1)
    {
        g_bQuickTimer = 1;
        time(&t);
        if(g_nMainTimer == -1)
        {
            g_nMainTimer = t;
        }
        else if(g_nMainTimer != t)
        {
            g_nMainTimer = t;
            g_bMainTimer = 1;
            g_bTimerDispose = 1;

            if(g_bLogo == TRUE)
            {
                g_nLogoTime ++;
                if(g_nLogoTime == 5)
                {// 关闭logo
                    g_bLogo = FALSE;
                    ClosePopLayer();
                }
            }
        }
        commfun_selectsleep(0, 100000);         /*定时100ms*/

        g_circleNo++;
        if(g_circleNo == 10)
        {
//          if(g_getdbstate == TRUE && g_firstread == TRUE)
//          {
//              //g_read_sc长度，根据读取的数据个数来确定
//              if(app_report_getdb(g_read_sc, g_hUdbCenter) == TRUE)
//              {
//                  //GUI_BUF_DEBUG(g_read_sc, 11, "read_sc:");
//                  g_firstread = FALSE;
//                  }
//              }
            g_circleNo = 0;
        }
    }
    pthread_exit(&timer_thread_id);
}


#if DESC("按键消息处理线程",1)

#define KBD_DEV     "/dev/s3c2410_kbd"

#define  UPPRESS_FIR    108
#define  DWPRESS_FIR    106
#define  LTPRESS_FIR    103
#define  RTPRESS_FIR    1
#define  OKPRESS_FIR    105

#define  UPPRESS_SEC    103
#define  DWPRESS_SEC    108
#define  LTPRESS_SEC    105
#define  RTPRESS_SEC    106
#define  OKPRESS_SEC    28
#define  CLEPRESS_SEC   1


//#define  SEC_FILE       "/usr/local/extapps/gui/cfg/fkgaconfig.xml"  //有该文件使用第二套按键键值
#define  SEC_FILE       "/usr/bin/force_setup"  //有该文件使用第二套按键键值
/*
#define  UPPRESS    108
#define  DWPRESS    106
#define  LTPRESS    103
#define  RTPRESS    1
#define  OKPRESS    105
*/
pthread_t   keyboard_thread_id;

BOOL g_bUpPress = FALSE;
BOOL g_bDownPress = FALSE;
BOOL g_bLeftPress = FALSE;
BOOL g_bRightPress = FALSE;
BOOL g_bOKPress = FALSE;
BOOL g_bCancelPress = FALSE;

BOOL g_bPress = FALSE;

/**********************************************************************
* @name      :  keyboard_thread
* @brief     ： 按键消息读取线程
* @param[in] ： 
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-11
* @Update    :
**********************************************************************/
void* keyboard_thread(void *arg)
{
    uint8 keyboardtype = 0;
    KEY_VALUE_T     key_val;
    /*打开按键设备文件*/ 

    KEY_DEVICE_T* dev = (KEY_DEVICE_T*)hal_device_get(HW_DEVICE_ID_KEY);
    if(NULL == dev)
    {
        GUI_FMT_DEBUG("dev is null\n");
        exit(1);
    }
#ifdef AREA_HUBEI
    static int bfgPower=1;
    static int afgPower=0;
#endif
    //隐藏菜单  按住上，按一下右，按一下中
    static uint8 rihgt_key_flag = 0;
    static uint8 okKeyFlag = 0;


    unsigned char data;

//  int UPPRESS = UPPRESS_FIR;
//  int DWPRESS = DWPRESS_FIR;
//  int LTPRESS = LTPRESS_FIR;
    int RTPRESS = RTPRESS_FIR;
    int OKPRESS = OKPRESS_FIR;
    //if(access(SEC_FILE, F_OK) >= 0)
    {
        keyboardtype = 1;
//      UPPRESS = UPPRESS_SEC;
//      DWPRESS = DWPRESS_SEC;
//      LTPRESS = LTPRESS_SEC;
        RTPRESS = RTPRESS_SEC;
        OKPRESS = OKPRESS_SEC;
    }

    while(1)
    {//只有显示控制权限在桌面APP或者轮显时，桌面APP才会读取设备的键值
#ifdef AREA_HUBEI
         if(bfgPower==-1&&(get_power_state(&afgPower) && afgPower == 1))   //外部供电切换背光灯亮90s
         {
             //停止轮显开启背光
             StopTrunDisp();
            // g_guiMain.bTrunDisp = FALSE;
            g_guiMain.nLightonTime = 0;
            g_guiMain.bLighton = TRUE;
            bfgPower=1;
            afgPower=0;
         }
         else
         {
            if (get_power_state(&bfgPower) && bfgPower == 0)
            {
            bfgPower = -1;
            }
            else
            {
            bfgPower = 1;
            }
         }
#endif         
        if(getAuthor() != 1 && activeID != 0 && g_guiMain.bTrunDisp == FALSE)
        {
            usleep(200000);
            continue;
        }
    
        if(dev->key_value_get(dev, &key_val, sizeof(KEY_VALUE_T)) >= 0)
        {   
        	//常显界面时，按键无效
		    if(g_guiMain.bOftenDisp == TRUE)
		    {
		        g_bOftenDisp = FALSE;
            	g_guiMain.bOftenDisp = FALSE;
				continue;
		    }
            //按键时，结束显示 常显页面，弹显页面，轮显页面
//            g_bOftenDisp = FALSE;
//            g_guiMain.bOftenDisp = FALSE;
            
            if(g_pop_mes_Fresh.is_pop == TRUE)
            {
                g_pop_mes_Fresh.delayTime = 0;
                g_pop_mes_Fresh.is_pop = FALSE;

				ClosePopLayer();
                desktop_key_pop_cancel();
            }


            //点亮背光灯
            g_guiMain.nLightonTime = 0;
            g_guiMain.bLighton = TRUE;

            //读取键值
            data = key_val.key;
            g_bPress = key_val.status;
            data = data&0x7F;
            GUI_FMT_DEBUG("getAuthor %u guiActiveID %u press key %02X, g_bPress = %02x,data = %d\n",getAuthor() , activeID, key_val.key, g_bPress, data);

			if(g_guiMain.bTrunDisp == TRUE && g_bPress == TRUE)
            {
                StopTrunDisp();
				continue;
            }
			
            if(keyboardtype)
            {
                switch(data)
                {
                    case UPPRESS_SEC:g_bUpPress = g_bPress?TRUE:FALSE;break;
                    case DWPRESS_SEC:g_bDownPress = g_bPress?TRUE:FALSE;break;
                    case LTPRESS_SEC:g_bLeftPress = g_bPress?TRUE:FALSE;break;
                    case RTPRESS_SEC:g_bRightPress = g_bPress?TRUE:FALSE;break;
                    case OKPRESS_SEC:g_bOKPress = g_bPress?TRUE:FALSE;break;
                    case CLEPRESS_SEC:g_bCancelPress = g_bPress?TRUE:FALSE;break;
                }
            }
            else
            {
                switch(data)
                {

                    case UPPRESS_FIR:g_bUpPress = g_bPress?TRUE:FALSE;break;
                    case DWPRESS_FIR:g_bDownPress = g_bPress?TRUE:FALSE;break;
                    case LTPRESS_FIR:g_bLeftPress = g_bPress?TRUE:FALSE;break;
                    case RTPRESS_FIR:g_bRightPress = g_bPress?TRUE:FALSE;break;
                    case OKPRESS_FIR:g_bOKPress = g_bPress?TRUE:FALSE;break;
//                    case CLEPRESS_SEC:g_bCancelPress = g_bPress?TRUE:FALSE;break;
                }
            }

            //组合按键 隐藏菜单两种方式：
            //老产品方式 按住确定，按一下右；
            //新方式：按住上，按一下右，按一下确定

            // 隐藏菜单  按住确，按一下右；
            if(g_bPress == TRUE && g_bOKPress == TRUE )
            {
                if(data==RTPRESS_SEC)
                {
                    g_nKeyCode=15;
                    g_bKeyboard=1;
                    g_bOKPress = FALSE;
                    okKeyFlag = 1;
                }
            }
            //隐藏菜单  按住上，按一下右，按一下中
            if(g_bPress == TRUE && g_bUpPress == TRUE )
            {
                if(data==RTPRESS)// 下  
                {
                    rihgt_key_flag = 1;
                }
                
                if(rihgt_key_flag == 1 && data==OKPRESS)
                {// 添加隐藏页面按键
                    g_nKeyCode=15;
                    g_bKeyboard=1;
                    g_bUpPress = FALSE;
                    rihgt_key_flag = 0;
                    okKeyFlag = 1;
                }                    
            }

            if(g_bPress == TRUE && data == CLEPRESS_SEC)		//取消键
			{
				g_nKeyCode=20;
				g_bKeyboard=1;
			}
            else if(g_bKeyboard != 1  && data == OKPRESS_SEC && g_bPress == FALSE)      //确认键，以弹出按键为准
            {
                if(okKeyFlag == 1 )
                {
                    okKeyFlag = 0;
                } 
                else
                {
                    g_nKeyCode = data;
                    g_bKeyboard = 1;
                } 
            }
			else if(g_bKeyboard != 1 && g_bPress == TRUE && data != OKPRESS_SEC)
            {// 普通按键
                g_nKeyCode = data;
                g_bKeyboard = 1;
            }
            else if(g_bPress == FALSE)
            {
                g_bOKPress = FALSE;
                g_bCancelPress = FALSE;
            }

        }
        else
            g_nKeyCode = 0;

        usleep(20000);
    }
    hal_device_release((HW_DEVICE *)dev);
    pthread_exit(&keyboard_thread_id);
}

#endif

BOOL g_bRunPaint = FALSE;

/**********************************************************************
* @name      :  DisposeMsg
* @brief     ： 桌面显示主线程函数
* @param[in] ： BOOL bMsgBox       //处理消息框页面标志 主线程中为FALSE
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-11
* @Update    :
**********************************************************************/
void DisposeMsg(BOOL bMsgBox)
{
    //LCD_DEVICE_T* gui_dev = (LCD_DEVICE_T*)hal_device_get(HW_DEVICE_ID_LCD);
    //char czBuf[20];
    uint8 keyboardtype = 0;
    int ret = 0;

    static BOOL lastLightState = TRUE;
    //if(access(SEC_FILE, F_OK) >= 0)
    {
        keyboardtype = 1;
    }
    static char lastAuthor = 0;
    for(;;)
    {
        if(gui_dev != NULL)
        {
            gui_dev->lcd_active_id_get(gui_dev,&activeID);
        }

        liveAppID = activeID;
        
        if(lastAuthor != getAuthor() || activeID != lastactiveID)
        {
            //printf("权限变化了!\n");
            if(lastAuthor == 0 || activeID == 0)        //画面切换到当前
            {
                //printf("权限切入\n");
                OnPaint();
                openAuthor();
            }
            lastAuthor = getAuthor();
            lastactiveID = activeID;
        }
        if(g_bMainTimer == 1)
        {
            g_bMainTimer = 0;
            OnTimer(TIMER_MAIN);
        }

        // 处理ipc
        //DisposeIPC();

        // 处理按键
        if(g_bKeyboard == 1)
        {
            g_bKeyboard = 0;
            if(keyboardtype)
            {   
//                GUI_FMT_TRACE("g_nKeyCode %02X\n", g_nKeyCode);
                switch(g_nKeyCode)
                {
                case UPPRESS_SEC:OnKeyMsg(KEY_UP);break;
                case DWPRESS_SEC:OnKeyMsg(KEY_DOWN);break;
                case LTPRESS_SEC:OnKeyMsg(KEY_LEFT);break;
                case RTPRESS_SEC:OnKeyMsg(KEY_RIGHT);break;
                case OKPRESS_SEC:OnKeyMsg(KEY_OK);break;
                case 20:OnKeyMsg(KEY_CANCEL);break;
                //case 1:OnKeyMsg(KEY_CANCEL);break;
                case 10:OnKeyMsg(PHONE_KEY);break;
                case 11:OnKeyMsg(RD_METER_KEY);break;
                case 12:OnKeyMsg(KEY_ADDTRUN);break;
                case 13:OnKeyMsg(START_UPGRADE_IR_KEY);break;
                case 14:OnKeyMsg(STOP_UPGRADE_IR_KEY);break;
                case 15:OnKeyMsg(KEY_MENU_EXT);break;
                case 16:OnKeyMsg(KEY_PROTECT);break;
                case 17:
                    PopLayer(0xFFFFFC);
                #if HARDWARE_2012_EN==1
                case 18:OnKeyMsg(KEY_SETPARA);break;
                #endif
                    break;
                }
            }
            else
            {
                switch(g_nKeyCode)
                {
                case UPPRESS_FIR:OnKeyMsg(KEY_UP);break;
                case DWPRESS_FIR:OnKeyMsg(KEY_DOWN);break;
                case LTPRESS_FIR:OnKeyMsg(KEY_LEFT);break;
                case RTPRESS_FIR:OnKeyMsg(KEY_RIGHT);break;
                case OKPRESS_FIR:OnKeyMsg(KEY_OK);break;
                case 20:OnKeyMsg(KEY_CANCEL);break;
                //case 1:OnKeyMsg(KEY_CANCEL);break;
                case 10:OnKeyMsg(PHONE_KEY);break;
                case 11:OnKeyMsg(RD_METER_KEY);break;
                case 12:OnKeyMsg(KEY_ADDTRUN);break;
                case 13:OnKeyMsg(START_UPGRADE_IR_KEY);break;
                case 14:OnKeyMsg(STOP_UPGRADE_IR_KEY);break;
                case 15:OnKeyMsg(KEY_MENU_EXT);break;
                case 16:OnKeyMsg(KEY_PROTECT);break;
                case 17:
                    PopLayer(0xFFFFFC);
                #if HARDWARE_2012_EN==1
                case 18:OnKeyMsg(KEY_SETPARA);break;
                #endif
                    break;
                }
            }
        }

        if(g_bRunPaint == TRUE)
        {
            OnPaint();
            g_bRunPaint = FALSE;
            continue;
        }

        if(bMsgBox == TRUE && g_msgInfo.bExist == TRUE)
        {
            break;
        }

        if(g_bQuickTimer == 1)
        {
            g_bQuickTimer = 0;
            OnTimer(TIMER_QUICK);
        }
        
        if((lastLightState != g_guiMain.bLighton))
        {
            if(g_guiMain.bLighton == TRUE)
            {
                g_guiMain.nLightonTime = 0;
                ret = lighton_lcd();
            }
            else
            {
                ret = lightoff_lcd();
            }
            
            if(ret >= 0) //背光操作成功
            {
                lastLightState = g_guiMain.bLighton;
            }
//			lastLightState = g_guiMain.bLighton;
            
        }
        usleep(20000);
    }
    //hal_device_release((HW_DEVICE *)gui_dev);
}
//#endif

/**********************************************************************
* @name      : get_ifIP
* @brief     ：获取网口的IP地址
* @param[in] ：char *ifname 网口名称
* @param[out]：
* @return    ：IP地址
* @Create    :
* @Date      ：2020-2-26
* @Update    :
**********************************************************************/

uint32 get_ifIP(char *ifname)
{ 
    register int fd, intrface;
    struct ifreq buf[12]; 
    struct ifconf ifc; 
    uint32 ipAddr = 0;

    if(NULL == ifname)
    {
        return 0;
    }
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) 
    { 
        ifc.ifc_len = sizeof(buf); 
        ifc.ifc_buf = (caddr_t) buf; 

        if (!ioctl(fd, SIOCGIFCONF, (char *) &ifc)) 
        {
            /*获取接口数量*/
            intrface = ifc.ifc_len / sizeof (struct ifreq);

            /*查找PPP位置*/
            while ( (strcmp(buf[intrface].ifr_name, ifname)) && (intrface >= 0) )
                intrface--;

            if (intrface >= 0)
            {
                /*获取当前网卡的IP地址*/ 
                if (!(ioctl (fd, SIOCGIFADDR, (char *)&buf[intrface]))) 
                {
                    ipAddr = ((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr.s_addr;
                    if (0 != ipAddr)
                    {
                        close(fd);
                        return ntohl(ipAddr);
                    }
                }           
            }
        } 
        else 
            perror ("ioctl error"); 

        close(fd);
    }
    return 0;
}



/**********************************************************************
* @name      : init_config
* @brief     ：初始化配置文件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-2-27
* @Update    : 
**********************************************************************/
int init_config(uint32 *ip, uint16 *port)
{
    dictionary *ini = NULL;

    char  *intername = NULL;
    uint32 ipmask = 0xffffffff;
    uint32 ipaddr = 0;
    char  *stringvalue = NULL;
    int    intvalue = 0;

    char   curpath[256] = {0};
    char   filename[256] = {0};
    int i;

    //获取当前目录绝对路径，即去掉程序名
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("get err path!");
    }
    else
    {
        for (i = cnt; i >=0; --i)
        {
            if (curpath[i] == '/')
            {
                curpath[i+1] = '\0';
                break;
            }
        }
    }
    sprintf(filename, "%s%s", curpath, GUI_INI_FILE);
    printf("filename is %s!\n", filename);

    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* ip获取 */
        intername = iniparser_getstring(ini, "MQTT_CONF:INTERNAME", NULL);
        if(NULL != intername)
        {
            //ipaddr = get_ifIP(intername);
            if(0 != ipaddr)
            {
                stringvalue = iniparser_getstring(ini, "MQTT_CONF:IPMASK", (char *)"255.255.255.255");
                ipmask = inet_addr(stringvalue);
                ipmask = ntohl(ipmask);
                if(0xffffffff != ipmask)
                {
                    //通过掩码获取服务端地址
                    ipaddr = (ipaddr & ipmask) + 0x01;
                }
            }
            else
            {
                /* 无接口时用IP */
                stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", (char *)"127.0.0.1");
                ipaddr = inet_addr(stringvalue);
                ipaddr = ntohl(ipaddr);
            }
        }
        else
        {
            /* INTERNAME为空用IP */
            stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", (char *)"127.0.0.1");
            ipaddr = inet_addr(stringvalue);
            ipaddr = ntohl(ipaddr);
        }
        *ip = ipaddr;

        /* 端口获取 */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        *port = (uint16)intvalue;
        
        /* 获取ipc路径 */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:IPCPATH", (char *)"/mnt/ipc_socket");
        task_msg_prepare(stringvalue);

        /* 获取log路径 */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:LOGPATH", (char *)"/mnt/log");
        
        iniparser_freedict(ini);
    }
    else
    {
        printf("Get no ini file %s!\n", GUI_INI_FILE);

        *ip = 0x7f000001;
        *port = 1883;
        task_msg_prepare((char *)"/mnt/ipc_socket");
    }

    /* 日志模块初始化 */
    diag_init(filename, FALSE);

    return 0;
}

static void signal_handler(int signum,siginfo_t* sigInfo,void* context)
{
    if(signum == SIGINT)    //暂定SIGINT，方便手动外部切换
    {
        turnAuthor();
        GUI_FMT_TRACE("收到信号，切换控制权, appAuthority = %d \n",getAuthor());
    }
    else
    {
        GUI_FMT_TRACE("未处理的信号：%d\n",signum);
    }
}
extern int gui_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);

int set_bluetooth_state(uint8 state)
{
    int ret = 0;
//    POWER_DEVICE_T* dev = (POWER_DEVICE_T*)hal_device_get(HW_DEVICE_ID_POWER);
//    if(NULL == dev)
//    {
//        GUI_FMT_DEBUG("set_bluetooth_close dev is null\n");
//        return -1;
//    }
//
//    ret = dev->power_ply_vcc_out_set(dev, MODULE_BT, state);
//    hal_device_release((HW_DEVICE *)dev);
    return ret;
}

int get_bluetooth_state(int32 *state)
{
    int ret = 0;
//    POWER_DEVICE_T* dev = (POWER_DEVICE_T*)hal_device_get(HW_DEVICE_ID_POWER);
//    if(NULL == dev)
//    {
//        GUI_FMT_DEBUG("set_bluetooth_close dev is null\n");
//        return -1;
//    }
//
//    ret = dev->power_ply_vcc_state_get(dev, MODULE_BT, state);
//    hal_device_release((HW_DEVICE *)dev);
    return ret;
}



/**********************************************************************
* @name      :  MiniGUIMain
* @brief     ： 桌面显示主函数
* @param[in] ： 
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-11
* @Update    :
**********************************************************************/
int MiniGUIMain(int argc,const char **argv)
{
    uint32 mqttip = 0;
    uint16 mqttport = 0;
    int     iret = -1;
	pthread_t meterThreadId;
    //APP重复运行检查
    if(!singleton_check((char *)MINI_GUI_TASK_PATH))
    {
        printf("gui already running!\n");
        exit(-1);
        return 0;
    }

    if(argc >= 2)
    {
        if(0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            struct timespec curtime = {0};
            struct timespec oldtime = {0};

            clock_gettime(CLOCK_MONOTONIC, &oldtime);
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            GUI_FMT_TRACE("sleep 30\n");
            while(curtime.tv_sec - oldtime.tv_sec <= 30)
            {
                sleep(30);
                clock_gettime(CLOCK_MONOTONIC, &curtime);
            }
        }
    }

	/* 读取配置文件 */
    init_config(&mqttip, &mqttport);
	
    GUI_FMT_DEBUG("====start run gui\n\n");

    //进程异常退出消息处理
    struct sigaction    act;
    memset(&act,0,sizeof(struct sigaction));
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = signal_handler;
    //sigaction(SIGINT,&act,NULL);
    init_sig_action((char *)"/mnt/data/app/desktopGui/log/Exception");
    
    if(0 != hal_init())
    {
        GUI_FMT_DEBUG("hal_init failed!\n");
        return FALSE;
    }
    gui_dev = (LCD_DEVICE_T*)hal_device_get(HW_DEVICE_ID_LCD);
    if(gui_dev != NULL)
    {
        RegisterDev(NULL,gui_dev);
        while (iret < 0)
        {
            iret = gui_dev->lcd_active_id_set(gui_dev,0);
            GUI_FMT_DEBUG("gui_dev->lcd_active_id_set(gui_dev,0);,iret = %d\n",iret);
            usleep(20000);
        }
    }
    else
    {
        GUI_FMT_DEBUG("hal_device_get failed,gui_dev is NULL!\n");
    }
    
    initlize_desk(NULL, gui_send_msg);
    openAuthor();
	
	screen_ctr_ini();	//屏显控制
    
    g_getdbstate = FALSE;
    g_firstread = TRUE;
    memset(g_read_sc, 0, sizeof(g_read_sc));

    pthread_mutex_init(&appGuiInfoLock, NULL);
    
    Initialize(NULL, TRUE);       // 初始化信息

	INIT_FMT_DEBUG("Version: %s Time: %s Build: %s %s 地区 %s \n", APP_VERSION, APP_PUBLISH_TIME, __DATE__, __TIME__,AREANAME_PLACE);
    
    g_bOftenDisp = TRUE;
//    load_app_icon(TRUE);        //载入其他app桌面信息

	//地区标识写铁电，供系统识别，江西地区上电时显示 “国网 江西省电力有限公司”
	//铁电第81个字节，江西地区 写P 字母
    area_info_save();

    read_pdata_xram(GUI_PARA, (char *)&toolInfoFresh.keyStatus, Esam_Status, Esam_Status_Len);

//    //蓝牙控制
//    uint8 bluetooth_status = 0;
//    read_pdata_xram(GUI_PARA, (char *)&bluetooth_status, Bluetooth_Status, Bluetooth_Status_Len);
//    if(bluetooth_status == BLUETOOH_ON)
//    {
//        set_bluetooth_state(1);
//    }
//    else
//    {
//        set_bluetooth_state(0);
//    }

     /* 对外通信线程启动 */
    if(msg_task_start(mqttip, mqttport))
    {
        GUI_FMT_DEBUG("msg task start failed\n");
        exit(0);
        return -1;
    }
    
    g_hUdbCenter = db_client_create(DB_GUI_AGENT, DB_CLIENT_GUI);
    if(g_hUdbCenter <= 0)
    {
        GUI_FMT_DEBUG("connect dbCenter failed!\n");
        exit(-1);

        return 0;
    }

    if(pthread_create(&timer_thread_id, NULL, timer_thread, NULL) != 0)
    {
        GUI_FMT_DEBUG("error create timer thread\n");
        exit(EXIT_FAILURE);
    }
/*
    if(pthread_create(&timer_dispose_id, NULL, timer_dispose_thread, NULL) != 0)
    {
        GUI_FMT_DEBUG("error create timer thread\n");
        exit(EXIT_FAILURE);
    }
*/
    if(pthread_create(&keyboard_thread_id, NULL, keyboard_thread, NULL) != 0)
    {
        GUI_FMT_DEBUG("error create keyboard thread\n");
        exit(EXIT_FAILURE);
    }
    //档案处理线程
    meter_thread_param_init();
    if(pthread_create(&meterThreadId, NULL, meter_thread, NULL) != 0)
    {
        GUI_FMT_DEBUG("error create meter thread\n");
        exit(EXIT_FAILURE);
    }


    GUI_FMT_DEBUG("InitMainWindow!\n");
    InitMainWindow();   // 定义以及创建窗口
//    if(GUI_LOGON_LAYER != 0xFFFFFFFF && GetPlugIn(GUI_LOGON_LAYER) != NULL)
//    {
//        PopLayer(GUI_LOGON_LAYER);
//        g_nLogoTime = 0;
//        g_bLogo = TRUE;
//    }
    
    OnPaint();

    DisposeMsg(FALSE);
    
    DestroyMainWindow(g_guiComm.hMainWnd);
    
    // 释放操作
    MainWindowThreadCleanup(g_guiComm.hMainWnd);
    Uninitialize();
    hal_device_release((HW_DEVICE *)gui_dev);
    return 0;
}

#if DESC("弹出消息框",1)

/**********************************************************************
* @name      :  MsgBoxDraw
* @brief     ： 绘制MessageBox页面
* @param[in] ： HDC hdc：绘制设备上下文
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
void MsgBoxDraw(HDC hdc)
{
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    // 判断信息字符串函数
    char* pBuf = g_msgInfo.pczMsg;
    int nLine = 0;
    while(*pBuf != '\0')
    {
        if(*pBuf == '\n')
        {
            nLine ++;
        }
        pBuf ++;
    }
    nLine ++;// 认为\n后面还有一行

    int nTop = (g_guiComm.nHeight-(nLine*13)-25-30)/2+17;
#if MID_LCD_STYLE == 1 //240x128
    SelectFont(hdc, g_guiComm.fontSmall);
    int posintval=12;
#else
    int posintval=16;
#endif
    // 绘制字符串信息
    pBuf = g_msgInfo.pczMsg;
    char* pTmp = pBuf;
    while(*pTmp != '\0')
    {
        if(*pTmp == '\n')
        {
            rc.top = nTop;
            rc.bottom = nTop+posintval;

            if(pTmp != pBuf)
            DrawText(hdc, pBuf, pTmp-pBuf, &rc, DT_CENTER|DT_VCENTER);
            nTop += posintval+1;
            pBuf = pTmp+1;
        }
        pTmp ++;
    }
    if(pTmp != pBuf)
    {
        rc.top = nTop;
        rc.bottom = nTop+posintval;
        DrawText(hdc, pBuf, pTmp-pBuf, &rc, DT_CENTER|DT_VCENTER);
    }
    nTop += posintval+1;
    nTop += posintval+2;// 信息与OK行之间的间隔

    // 绘制确定、取消button
    //SelectFont(hdc, g_guiComm.fontBig);
    switch(g_msgInfo.nType)
    {
    case MB_OKCANCEL:
        if(g_msgInfo.bRet == TRUE)
        {
            int nLeft = g_guiComm.nWidth/2+18;
            ITEM_SELECT
            TextOut(hdc, nLeft, nTop, "确定");
            ITEM_NOT_SELECT
            nLeft = g_guiComm.nWidth/2-50;
            TextOut(hdc, nLeft, nTop, "取消");
        }
        else
        {
            int nLeft = g_guiComm.nWidth/2+18;
            TextOut(hdc, nLeft, nTop, "确定");
            ITEM_SELECT
            nLeft = g_guiComm.nWidth/2-50;
            TextOut(hdc, nLeft, nTop, "取消");
            ITEM_NOT_SELECT
        }
        break;
    case MB_OK:
        {
            g_msgInfo.bRet = TRUE;
            ITEM_SELECT
            rc.top = nTop;
            rc.bottom = (posintval == 12)?(nTop + 16):(nTop + 20);
            DrawText(hdc, "确定", 4, &rc, DT_CENTER|DT_VCENTER);
            ITEM_NOT_SELECT
        }
        break;
    }
}

/**********************************************************************
* @name      :  MsgBoxKeyMsg
* @brief     ： 处理MessageBox按键消息
* @param[in] ： nKey：键值
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
BOOL MsgBoxKeyMsg(int nKey)
{
    switch(nKey)
    {
    case KEY_LEFT:
        if(g_msgInfo.nType == MB_OKCANCEL && g_msgInfo.bRet == TRUE)
        {
            g_msgInfo.bRet = FALSE;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        break;
    case KEY_RIGHT:
        if(g_msgInfo.nType == MB_OKCANCEL && g_msgInfo.bRet == FALSE)
        {
            g_msgInfo.bRet = TRUE;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        break;
    case KEY_OK:
        g_msgInfo.bExist = TRUE;
        break;
    case KEY_CANCEL:
        g_msgInfo.bRet = FALSE;
        g_msgInfo.bExist = TRUE;
        break;
    }
    return TRUE;
}
PLUGIN_INFO g_MessageBox = {
    0/*id*/,
    NULL/*InitLayer*/,
    NULL/*UninitLayer*/,
    MsgBoxDraw/*Draw*/,
    NULL/*Timer*/,
    MsgBoxKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
/*********************************************************************
    所属功能单元：显示框架模块
    功能描述：    MessageBox接口函数
    输入参数：    char* pczMsg：MessageBox提示信息
                  int nType：MessageBox类型，MB_OK、MB_OKCANCEL
    输出参数：
    函数返回值：  BOOL：确定返回TRUE,取消返回FALSE
    内部处理流程：
********************************************************************/
/**********************************************************************
* @name      :  MessageBox
* @brief     ： MessageBox接口函数
* @param[in] ： char* pczMsg：MessageBox提示信息
                int nType：MessageBox类型，MB_OK、MB_OKCANCEL
* @param[out]：
* @return    ：BOOL：确定返回TRUE,取消返回FALSE
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
BOOL MessageBox(const char* pczMsg, int nType)
{
    // 如果处于弹出页面,不允许弹出MessageBox
    if(g_guiMain.plugPop != NULL)
    {
        return FALSE;
    }
    ASSERT(pczMsg != NULL);
    int nLen = strlen(pczMsg);
    ASSERT(g_msgInfo.pczMsg == NULL);
    g_msgInfo.pczMsg = (char*)malloc(nLen+1);
    strcpy(g_msgInfo.pczMsg, pczMsg);
    g_msgInfo.bExist = FALSE;
    g_msgInfo.bRet = TRUE;
    g_msgInfo.nType = nType;

    g_guiMain.plugMsg = &g_MessageBox;
    UpdateWindow(g_guiComm.hMainWnd, TRUE);

    // 进入消息循环
#ifdef ARM_LINUX
    DisposeMsg(TRUE);
#else
    MSG msg;
    while(GetMessage(&msg, g_guiComm.hMainWnd))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if(g_msgInfo.bExist == TRUE)
        {
            break;
        }
    }
#endif

    // 退出释放
    g_guiMain.plugMsg = NULL;
    if(g_msgInfo.pczMsg != NULL)
    {
        free(g_msgInfo.pczMsg);
        g_msgInfo.pczMsg = NULL;
    }

    // 重新刷新窗口，进入主窗口的消息循环
    UpdateWindow(g_guiComm.hMainWnd, TRUE);
    return g_msgInfo.bRet;
}
#endif

#if DESC("弹出列表框",1)
typedef struct _POP_LIST
{
    const char *pczList[96];// 最大30个选项
    int     nLine;      // 行
    int     nSel;       // 当前选择项
    int     nIndex;     // 显示的首行
    BOOL    bBigFont;

    int     nLeft;      // position
    int     nTop;       // position
    int     nRight;     // 弹出的最大宽度
    int     nBottom;    // 弹出的最大高度
}POP_LIST;
POP_LIST g_pop_list;
#define MAX_POP_LINE    6
/**********************************************************************
* @name      : PopList
* @brief     ： 弹出列表菜单初始化
* @param[in] ：    char* pList[]：菜单列表字符串
                  int nLine：菜单项数
                  char* pczSel：选中的菜单字符串
                  int nLeft, int nTop：菜单的左上角位置
                  BOOL bBigFont：是否大字体
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
void PopList(const   char * const pList[], int nLine, const char* pczSel, int nLeft, int nTop, BOOL bBigFont)
{
    ASSERT(nLine <= 96 && pczSel != NULL);
    if(pczSel == NULL)return ;
    if(nLine > 96)nLine=96;

    g_pop_list.nLine = nLine;
    g_pop_list.nSel = 0;
    g_pop_list.nIndex = 0;

    const char* pBuf;
    int nMaxWidth = 0;
    int nWidth;
    int nIndex = 0;
    for(int i = 0; i < nLine; i ++)
    {
        pBuf = pList[i];
        if(pBuf == NULL)
        {
            g_pop_list.nLine --;
            continue;
        }
        g_pop_list.pczList[nIndex] = pBuf;

        nWidth = strlen(pBuf);
        if(nWidth > nMaxWidth)nMaxWidth=nWidth;

        if(strcmp(pczSel, pBuf) == 0)
        {
            g_pop_list.nSel = nIndex;
        }

        nIndex ++;
    }
    g_pop_list.nIndex = g_pop_list.nSel-MAX_POP_LINE+1;
    if(g_pop_list.nIndex < 0)g_pop_list.nIndex=0;

    g_pop_list.nLeft = nLeft;
    g_pop_list.nTop = nTop;
    g_pop_list.bBigFont = bBigFont;

    int nMaxLine = (g_pop_list.nLine>MAX_POP_LINE)?MAX_POP_LINE:g_pop_list.nLine;
    if(bBigFont == TRUE)
    {
        g_pop_list.nRight = nLeft+nMaxWidth*8+4;
        g_pop_list.nBottom = nTop+nMaxLine*17+3;
    }
    else
    {
        g_pop_list.nRight = nLeft+nMaxWidth*6+4;
        g_pop_list.nBottom = nTop+nMaxLine*13+3;
    }
    g_guiComm.bPopList = TRUE;
    UpdateWindow(g_guiComm.hMainWnd, TRUE);
}

/**********************************************************************
* @name      :  PopListDraw
* @brief     ： 弹出列表菜单界面显示
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
void PopListDraw(HDC hdc)
{
    if(g_guiComm.bPopList == FALSE)
    {
        return;
    }
    PLOGFONT fontOld = SelectFont(hdc, (g_pop_list.bBigFont==TRUE)?g_guiComm.fontBig:g_guiComm.fontSmall);

    ITEM_NOT_SELECT;
    Rectangle(hdc, g_pop_list.nLeft, g_pop_list.nTop, g_pop_list.nRight, g_pop_list.nBottom);
    FillBox(hdc, g_pop_list.nLeft+1, g_pop_list.nTop+1,
        g_pop_list.nRight-g_pop_list.nLeft-1, g_pop_list.nBottom-g_pop_list.nTop-1);

    const char* pBuf;
    int nLeft = g_pop_list.nLeft+2;
    int nTop = g_pop_list.nTop+2;
    int nLineHeight = (g_pop_list.bBigFont==TRUE)?17:13;
    int nMax = (g_pop_list.nIndex+MAX_POP_LINE>g_pop_list.nLine)?
        g_pop_list.nLine:g_pop_list.nIndex+MAX_POP_LINE;
    for(int i = g_pop_list.nIndex; i < nMax; i ++)
    {
        pBuf = g_pop_list.pczList[i];
        if(g_pop_list.nSel == i)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft, nTop, pBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft, nTop, pBuf);
        }
        nTop += nLineHeight;
    }
    SelectFont(hdc, fontOld);
}

/**********************************************************************
* @name      :  PopListKeyMsg
* @brief     ： 按键消息处理
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
char* PopListKeyMsg(int nKey, int* nSel)
{
    *nSel = -1;
    if(g_guiComm.bPopList == FALSE)
    {
        return NULL;
    }
    switch(nKey)
    {
    case KEY_UP:
        g_pop_list.nSel --;
        if(g_pop_list.nSel < 0)
        {
            g_pop_list.nSel = g_pop_list.nLine-1;
        }
        if(g_pop_list.nSel < g_pop_list.nIndex)
        {
            g_pop_list.nIndex = g_pop_list.nSel;
        }
        if(g_pop_list.nSel >= g_pop_list.nIndex+MAX_POP_LINE)
        {
            g_pop_list.nIndex = g_pop_list.nSel-MAX_POP_LINE+1;
        }
        break;
    case KEY_DOWN:
        g_pop_list.nSel ++;
        if(g_pop_list.nSel >= g_pop_list.nLine)
        {
            g_pop_list.nSel = 0;
        }
        if(g_pop_list.nSel < g_pop_list.nIndex)
        {
            g_pop_list.nIndex = g_pop_list.nSel;
        }
        if(g_pop_list.nSel >= g_pop_list.nIndex+MAX_POP_LINE)
        {
            g_pop_list.nIndex = g_pop_list.nSel-MAX_POP_LINE+1;
        }
        break;
    case KEY_CANCEL:
        ASSERT(g_guiComm.bPopList == TRUE);
        g_guiComm.bPopList = FALSE;
        break;
    case KEY_OK:
        {
            g_guiComm.bPopList = FALSE;
            *nSel = g_pop_list.nSel;
            static char retbuf[256];
            memcpy(retbuf, g_pop_list.pczList[*nSel], sizeof(retbuf-1));
            retbuf[sizeof(retbuf)-1] = '\0';
            return (retbuf);
        }
    }
    UpdateWindow(g_guiComm.hMainWnd, TRUE);
    return NULL;
}

#endif

#if DESC("密码验证",1)

extern PLUGIN_INFO g_layerFreezeTime;
extern long GetIceOutTime();

/**********************************************************************
* @name      : PopList
* @brief     ：密码验证，在一个时间内没有经过密码验证，会弹出验证框
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
BOOL ValidatePwd()
{
	g_bpop = TRUE;
    
    if(g_guiComm.bValidatePwd == TRUE)
    {
        return TRUE;
    }

    if(GetIceOutTime() < 0)
    {
        g_guiComm.nPwdKey = g_guiComm.nCurKey;
        PopLayer(g_layerValidatePwd.dwId);
    }
    else
    {
        PopLayer(g_layerFreezeTime.dwId);
    }
    
    return FALSE;
}

#endif

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif
