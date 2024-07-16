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

#include "task_id.h"
#include "out_msg.h"
#include "iniparser.h"
#include "task_msg.h"
#include "appreport.h"
#include <signal.h>
#include "gui_prtl.h"
#include "lcd.h"
//#include "gdi.h"
#include "key.h"
#include "pageInterface.h"

//extern uint8 * g_pFrameBuffer;
extern "C"
{	int32 LCD_Refresh( LCD_DEVICE_T* dev, uint32 startx, uint32 starty,uint32 endx, uint32 endy);	}

extern BOOL ReadConfig(BOOL bAll);
extern BOOL RestoreConfigFromXml();
LCD_DEVICE_T* app_dev;

int appAuthority = 0;

uint32 guiActiveID = 1000;
uint32 lastGuiActiveID = 1000;
uint32 RegisterID = 0;

extern PLUGIN_INFO g_layerValidatePwd;
GUI_MAIN g_guiMain = {0, 0, 0, 0, 0, 0, NULL, 0, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE, 0, 10, FALSE, 0, 15, 0, -1, 5};
#if BEIJING_FUN
GUI_COMM g_guiComm = {NULL, NULL, NULL, 160, 160, TRUE, 0, 0, 0, 0, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE};
#else
GUI_COMM g_guiComm = {NULL, NULL, NULL, 160, 160, TRUE, 0, 0, 0, 0, FALSE, FALSE, TRUE, FALSE, TRUE};
#endif

PLUGIN_INFO* g_pluginTrunDisp[MAX_TRUNDISP_CNT];
int g_nTrunDisp[MAX_TRUNDISP_CNT];  // 轮显页面库数组
int g_nTotalTrunDispCnt = 0;
int g_nIpcRet = -1;

int g_bQuickTimer = 0;	// 100ms定时器
int g_bMainTimer = 0;
int g_bTimerDispose = 0;
int g_bKeyboard = 0;
unsigned char g_nKeyCode = 0;

BOOL g_bLogo = FALSE;
int g_nLogoTime = 0; //-logo页面显示时间-

BOOL g_bUpdate = FALSE;
#if HUBEI_FUN == 1
char g_poweron_Flag = 0;
char g_poweron_reset_Flag = 0;//彻底断电重启
char g_count = 0;
#endif

extern int g_narrMenuIndex[20];
extern int g_nMenuNodes;

int g_nPasswdTime = 0;		// 密码失效时间
uint8 g_read_sc[20];		//初始化读取记录成功与否

BOOL g_bAllMenu = FALSE;        // 是否显示全部菜单
DB_CLIENT g_hUdbCenter;
DB_AGENT  g_dbAgent = 0;
extern BOOL g_getdbstate;
BOOL g_firstread;
BOOL g_circleNo;

//#define MINI_GUI	"test_gui"

/*MessageBox页面库操作*/
typedef struct _MSG_INFO
{
    char*   pczMsg;     // MessageBox消息
    BOOL    bExist;     // 判断是否退出页面库标志
    BOOL    bRet;       // 退出时的返回值
    int     nType;      // MessageBox类型MB_OK、MB_OKCANCEL
}MSG_INFO_t;
MSG_INFO_t g_msgInfo = {NULL, FALSE, FALSE, MB_OK};

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
	printf("error is %s----%d\n", pFileName, nLine);
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
	printf(czBuf);
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
    int nend = pluginCnt;
    int nmid;
    while(nbeg+1 != nend)
    {
        nmid = (nbeg+nend)>>1;
        if(pG_plugin[nmid]->dwId < dwPluginId)/*比较*/
        {
            nbeg = nmid;
        }
        else
        {
            nend = nmid;
        }
    }
	printf("id---0x%08x\n", dwPluginId);
    if(nend >= pluginCnt || pG_plugin[nend]->dwId != dwPluginId)/*比较*/
    {
		printf("未找到页面id---0x%08x\n", dwPluginId);
        return NULL;
    }
    return pG_plugin[nend];
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
    pPluginTmp = pG_plugin[nLeft]; pG_plugin[nLeft] = pG_plugin[i]; pG_plugin[i] = pPluginTmp;

    pPlugin = pG_plugin[nLeft];
    i = nLeft;
    j = nRight+1;

    while(1)
    {
        do{i++;}while(i<j && pG_plugin[i]->dwId<pPlugin->dwId);
        do{j--;}while(pG_plugin[j]->dwId>pPlugin->dwId);
        if(i > j)
        {
            break;
        }
        pPlugin = pG_plugin[i]; pG_plugin[i] = pG_plugin[j]; pG_plugin[j] = pPlugin;
    }

    pPlugin = pG_plugin[nLeft]; pG_plugin[nLeft] = pG_plugin[j]; pG_plugin[j] = pPlugin;
    QuickSortPlugin(nLeft,j-1);
    QuickSortPlugin(j+1,nRight);
}
void sys_sort_insert(int nLeft, int nRight)
{
    int i, j;
    PLUGIN_INFO*  pTmp;
    for(i = nLeft+1; i <= nRight; i ++)
    {
        pTmp = pG_plugin[i];
        for(j  = i; j>0 && pTmp->dwId<pG_plugin[j-1]->dwId; j --)   /*比较*/
        {
            pG_plugin[j] = pG_plugin[j-1];
        }
        pG_plugin[j] = pTmp;
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
	StopTrunDisp();

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
	输入参数：
	输出参数：
	函数返回值：  BOOL：成功返回TRUE，失败返回FALSE
	内部处理流程：
********************************************************************/
BOOL Initialize(char** appName, BOOL show_oftendisp)
{
    // 快排页面库数组
    QuickSortPlugin(0, pluginCnt-1);
	sys_sort_insert(0, pluginCnt-1);


    /*获取公用的信息*/
	// 获取16*16 宋体
	g_guiComm.fontBig = GetSystemFont(SYSLOGFONT_MENU);
    // 获取12*12 宋体，因为在MiniCFG.ini的配置中，使用12*12为缺省字体
    g_guiComm.fontSmall = NULL;

	if ((localAppName[0] == 0) &&
	    (localAppName[1] == 0) &&
		(localAppName[2] == 0) &&
		(localAppName[3] == 0))
	{
		reg_pdata_xram("desktopGui", GUI_PRIVATE_DATA);
		reg_pdata_xram("desktopGui", GUI_CONFIG_DATA);
		reg_pdata_xram("desktopGui", EVENT_PRIVATE_FILE);
		reg_pdata_xram("desktopGui", GUI_PARA);
	}
	else
	{
		reg_pdata_xram(localAppName, GUI_PRIVATE_DATA);
		reg_pdata_xram(localAppName, GUI_CONFIG_DATA);
		reg_pdata_xram(localAppName, EVENT_PRIVATE_FILE);
		reg_pdata_xram(localAppName, GUI_PARA);
	}


	/*读取xml操作*/
	/*包含读取动态菜单，滚动界面信息等*/
    // 生成菜单时会分配内存，在Uninitialize中释放
	//if(ReadConfig(g_bAllMenu) == FALSE)
	{
		RestoreConfigFromXml();
		ReadConfig(g_bAllMenu);
	}

	printf("read config success!\n");
	if(g_nMenus <= 0)
	{
        ASSERT(FALSE);
		return FALSE;
	}

    // 根据主菜单风格ID，确定当前的plug
	g_guiMain.pluginCur = GetPlugIn(g_guiMain.nMenuRootId);
    ASSERT(g_guiMain.pluginCur != NULL);

    // 初始化当前页面库，在Uninitialize中释放
	ASSERT(g_guiMain.pluginCur != NULL);
	g_guiComm.bCanTrunDisp = TRUE;
	g_guiComm.nArrow = -1;
#if BEIJING_JZQ_EN == 1
    g_guiComm.sCaption[0] = '\0';
    g_guiComm.NodeAddrPage = 0;
    g_guiComm.NodeAddrIndex = 0;
#endif
	if(g_guiMain.pluginCur != NULL)
	{
		if(g_guiMain.pluginCur->pfnInitLayer != NULL)
		{
			g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
		}
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
}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    显示模块绘制函数，框架本身并不实现绘制，通过调用页面库实现
	输入参数：
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
void OnPaint()
{
    int ret = 0;
    
	GUI_FMT_TRACE("%s:消息权限:%d,guiActiveID = %d,RegisterID = %d,app_dev = %lu\n\n",__FUNCTION__,getAuthor(),guiActiveID,RegisterID,app_dev);
	if(getAuthor() == 1 || (guiActiveID != 0 && guiActiveID == RegisterID))
	{
	    // 判断轮显状态时，APP无轮显时，不显示，直接退出
        if(g_guiMain.bTrunDisp == TRUE && g_guiMain.nTrunDispCnt == 0)
        {
            GUI_FMT_DEBUG("OnPaint:bTrunDisp:%d,nTrunDispCnt = %d\n\n",g_guiMain.bTrunDisp,g_guiMain.nTrunDispCnt);
            return;
        }

    	//printf("appGui刷正屏：getAuthor()=%d,activeID = %d, registerID = %d\n",getAuthor(),guiActiveID,RegisterID);
    	HDC hdcDst=BeginPaint(g_guiComm.hMainWnd);
    	HDC hdc = CreateCompatibleDC(hdcDst);
        if(g_guiComm.nWidth == 160 && g_guiComm.nHeight == 160)
        {
            FillBox(hdc, 0, 16, g_guiComm.nWidth, 128);
        }
        else        //专一显示
        {
            FillBox(hdc, 0, 18, g_guiComm.nWidth, g_guiComm.nHeight-18);
        }    

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
        else
        {
            if(g_guiMain.plugMsg != NULL)
            {
                ASSERT(g_guiMain.plugMsg->pfnDraw != NULL);
                g_guiMain.plugMsg->pfnDraw(hdc);
            }
            else if(g_guiMain.plugPop != NULL)
            {// 判断是否存在pop画面库
                // 优先处理弹出页面库
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

        if(g_guiComm.nWidth == 160 && g_guiComm.nHeight == 160)
        {
            BitBlt(hdc, 0, 16, g_guiComm.nWidth, 128, hdcDst, 0, 16, 0);
        }
        else        //专一显示
        {
            BitBlt(hdc, 0, 18, g_guiComm.nWidth, g_guiComm.nHeight-18, hdcDst, 0, 18, 0);
        }   
    	
//        BitBlt(hdc, 0, 0, g_guiComm.nWidth, g_guiComm.nHeight, hdcDst, 0, 0, 0);

    	if(g_guiComm.nWidth == 160 && g_guiComm.nHeight == 160)
        {
            ret = LCD_Refresh( app_dev, 0, 16, 159, 143);
        }
        else        //专一显示
        {
            ret = LCD_Refresh( app_dev, 0, 18, g_guiComm.nWidth-1, g_guiComm.nHeight-1);
        } 

        if(ret != 0)
        {
            GUI_FMT_DEBUG("LCD_Refresh:ret:%d,x[0,159]y[16,143],app_dev = %lu\n\n",ret,app_dev);
        }

        DeleteCompatibleDC(hdc);
    	EndPaint(g_guiComm.hMainWnd,hdcDst);
	}
}

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
	//printf("Read trundisp cnt is %d max trundisp time %d interval time %d\n ",
	//	g_guiMain.nTrunDispCnt, g_guiMain.nTrunDispTimeMax, g_guiMain.nInterval);
}
void WriteTrunDisp()
{
	char czBuf[MAX_TRUNDISP_CNT*4+2];
	czBuf[0] = (unsigned char)g_nTotalTrunDispCnt;
	czBuf[1] = (unsigned char)g_guiMain.nTrunDispTimeMax;
	czBuf[2] = (unsigned char)g_guiMain.nInterval;
	//int nRet = WriteXPara(TMN_RUN_PARA, Turn_Menu_Num_Time, czBuf, 0);
    int nRet = write_pdata_xram(GUI_PARA, czBuf, Turn_Num_Time, Turn_Num_Time_Len);
	ASSERT(nRet > 0);

	char* pBuf = czBuf;
	//PLUGIN_INFO* pPlugin;
	for(int i = 0; i < g_nTotalTrunDispCnt; i ++)
	{
		int32tochar(g_nTrunDisp[i], pBuf);
		pBuf += 4;
	}
	//printf("Write trundisp cnt is %d max trundisp time %d interval time %d\n ",
	//	g_guiMain.nTrunDispCnt, g_guiMain.nTrunDispTimeMax, g_guiMain.nInterval);
	//nRet = WriteXPara(TMN_RUN_PARA, Turn_Menu_IDs, czBuf, 0);
    nRet = write_pdata_xram(GUI_PARA, czBuf, Turn_IDs, Turn_IDs_Len);
	ASSERT(nRet > 0);
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
void StrartTrunDisp()
{
	ReadTrunDisp();

	if(g_guiMain.nTrunDispCnt == 0)
	{
		g_guiMain.bTrunDisp = TRUE;
		g_guiMain.nTrunDispIndex = 0;
//		g_guiMain.nLightonTime = 0;
        StopTrunDisp();
		return;
	}

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

    g_guiMain.bTrunDisp = TRUE;
	g_guiMain.nTrunDispIndex = 0;
	g_guiMain.nTrunDispTime = 0;
	// 重新读取轮显页面库
	//ReadTrunDisp();

    // 初始化新的轮显页面库
    if(g_guiMain.nTrunDispCnt > 0)
    {
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
    }
	else
	{
		g_guiComm.nCurPoint = g_guiComm.nCurPointTmp;
		g_guiComm.bCanTrunDisp = g_guiComm.bCanTrunDispTmp;
		g_guiComm.nIndexPoint = g_guiComm.nIndexPointTmp;
		g_guiComm.bPointOffset = g_guiComm.bPointOffsetTmp;
		g_guiComm.nArrow = g_guiComm.nArrowTmp;
	}

    // nLightonTime作为两个用途，如果没有进入轮显，则作为判定进入轮显最大时间值
    // 否则作为两个轮显页面的间隔值判定
//    g_guiMain.nLightonTime = 0;
	UpdateWindow(g_guiComm.hMainWnd, TRUE);
}
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
        }

		appGUI_send_msg_to_gui(MESSAGE_IID_DESKGUI, DESKGUI_IOP_EXIT_DISPLAY, NULL, 0);
        if(pAppDev != NULL)
        {
//            pAppDev->lcd_active_id_set(pAppDev,0);
        }
        
        closeAuthor();
        guiActiveID = 0;
        TurnDisp = FALSE;
        
        GUI_FMT_TRACE("%s退出轮显\n", localAppName);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
    }
}
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

    if(g_guiMain.nTrunDispIndex >= g_guiMain.nTrunDispCnt-1)
	{
		StopTrunDisp();
        return ;
	}
    
	if(bPre == TRUE)
	{
		g_guiMain.nTrunDispIndex --;

	}
	else
	{
		g_guiMain.nTrunDispIndex ++;

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

extern void init_layer();
extern void ShowOrHideMenu();
extern void SetParaMenu();
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

	ASSERT(g_guiMain.pluginCur != NULL);
	switch(nKey)
	{
	case KEY_MENU_EXT:
		ShowOrHideMenu();
		return;
	case KEY_PROTECT:
		{
			/*char czBuf[100];
			int nRet = ReadXPara(TMN_FUNC_PARA, Protect_Exclude_Para_ID, czBuf, TMN_ID);
			if(nRet > 0)
			{
				czBuf[0] &= 0xFB;
				WriteXPara(TMN_FUNC_PARA, Protect_Exclude_Para_ID, czBuf, TMN_ID);
			}

			nRet = ReadXPara(TMN_FUNC_PARA, Protect_Exclude_Para_ID, czBuf, TMN_ID);*/
		}
		return;
		#if HARDWARE_2012_EN==1 && (HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4)
		case KEY_SETPARA:
		{
			SetParaMenu();
		}
		return;
		#endif
	}

	g_guiMain.nTrunDispTime = 0;
	if(g_guiMain.bTrunDisp == TRUE)
	{// 取消轮显
		if(g_guiMain.nTrunDispCnt > 0)
		{
			switch(nKey)
			{
			#if SHANGHAI_FUN == 0
			case KEY_UP:
			case KEY_LEFT://任意键停止轮显
				//KeepNextTrunDisp(TRUE);
				//UpdateWindow(g_guiComm.hMainWnd, TRUE);
				//break;
			case KEY_DOWN:
			case KEY_RIGHT:
				//KeepNextTrunDisp();
				//UpdateWindow(g_guiComm.hMainWnd, TRUE);
				//break;
			default:
				StopTrunDisp();
			#else
			case KEY_OK:
			default:
				StopTrunDisp();
			#endif
			}
			return;
		}
		else
		{
			g_guiMain.bTrunDisp = FALSE;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
	}

    if(g_guiMain.pluginCur != NULL)
    {
        printf("pluginCur->->dwId---0x%08x\n",g_guiMain.pluginCur->dwId);
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
    else if(g_guiMain.pluginCur->pfnKeyMsg != NULL
		&& TRUE == g_guiMain.pluginCur->pfnKeyMsg(nKey))
	{
		if(KEY_ADDTRUN == nKey)
		{
			AddTrunDisp();
		}
		return;
	}
	// 使用缺省操作
	printf("#####使用缺省操作方案\n");
	switch(nKey)
	{
	#if MODULARIZATION_TMN_EN == 1
	case MODU_CANCEL:
		if(g_guiComm.bOKiscancel == 1)
		{
        	CancelMenu();
			g_guiComm.bOKiscancel = 0;
		}
		break;
	#endif
	case KEY_CANCEL:
        CancelMenu();
		break;
    case KEY_UP:
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
        // 当前指向页面库集合时，指向下一个页面库
		KeepNextLayer();
		SetSubLayerArrow();
        break;
	case KEY_ADDTRUN:
		// 加入当前页面到轮显页面库中
		AddTrunDisp();
		break;
	default:break;
	}
}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    处理ipc消息，
                    该函数由窗口过程函数调用，完成分发和缺省处理
	输入参数：    char* pczBuf：IPC数据缓冲区
                  int nLen：IPC数据缓冲区长度
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
void DisposeIPC()
{
/*
#ifndef _MSC_VER
    uint8 czBuf[IPC_BUFFER];
    int nLen = 0;

	if(g_nIpcRet <= 0)
	{
		g_nIpcRet = OpenIPC( GUI_PORT_ID, IPC_SERVICE);
	}
	if(g_nIpcRet > 0)
	{
		memset(czBuf, IPC_BUFFER, 0);
		nLen = RcvIPC(GUI_PORT_ID, czBuf, IPC_BUFFER);
		if(nLen <= -1)
		{
			CloseIPC(GUI_PORT_ID);
			g_nIpcRet = -1;
		}
		else if(nLen > 0)
		{
			ASSERT(nLen > 0);
			switch(czBuf[0])
			{
			case GUI_CMD_POPUP:
				{
					char* pData = (char*)&czBuf[1];
					if(nLen >= 5)
					{
						DWORD dwPluginId = chartoint32(pData);
						// 弹出GPRS参数菜单时电量背景灯
						if (dwPluginId == 0x00660001)
						{
							PopLayer(0x00660000);
							g_guiMain.nLightonTime = 0;
							//if (g_guiMain.bLighton == FALSE)
							{// 亮灯
								g_guiMain.bLighton = TRUE;
						        // 点亮背光灯
								#ifdef ARM_LINUX
								lighton_lcd();
								printf("ARM_LINUX pop lighton\n");
								#endif
								printf("pop lighton\n");
							}
						}

						PopLayer(dwPluginId);
						printf("ipc pop layer %04X\n", dwPluginId);
					}
				}
				break;
			default:
				break;
			}
		}
	}

    // 系统IPC 消息队列处理
    char* pMsg;
    while(recv_ipc_msg(&pMsg) > 0)
    {
        PopCustomDebug(pMsg);// 弹出调试页面
    }
#endif*/
}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    显示模块定时器消息，
                    完成页面库的定时器分发。背光灯、轮显方面的操作
	输入参数：    int nId：定时器ID
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
int g_nSpeak_cnt = 0;
void OnTimer(int nId)
{
	char czBuf[50];
    int ret = 0;
	// 分发消息
	// 如果有弹出页面库，则发送定时器消息到弹出页面库
	// 否则发送消息到当前页面库
	if(g_guiMain.plugPop != NULL && g_guiMain.plugPop->pfnOnTimer != NULL)
	{
		g_guiMain.plugPop->pfnOnTimer(nId);
	}
	else if(g_guiMain.bTrunDisp == TRUE && g_guiMain.nTrunDispCnt > 0)
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

	if(nId == TIMER_MAIN)
	{// 如果为主定时器时，需要执行背光灯、轮显页面的判定操作
		//int nRet;
		//nRet = ReadRamXData(Radio_Speak_State, czBuf);
		//if(nRet > 0 && czBuf[0] == 0)
		//jackyben 2020-1-8
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


		// 背光灯定时器操作判定
		if(g_guiMain.bLighton == TRUE)
		{
			g_guiMain.nLightonTime++;
			if(g_guiMain.nLightonTime > g_guiMain.nLightonTimeMax)
			{// 灯灭
				g_guiMain.bLighton = FALSE;
				// 熄灭背光灯
//				#ifdef ARM_LINUX
//				lightoff_lcd();
//				#endif
			}
		}

		//KeepMeAlive();

		g_nPasswdTime ++;
		if(g_nPasswdTime > 180)
		{
			g_guiComm.bValidatePwd = FALSE;
		}

//        GUI_FMT_DEBUG("ActiveID:%u , myID%u, %d秒.\n",guiActiveID, RegisterID, g_guiMain.nTrunDispTime);
//        GUI_FMT_TRACE("guiActiveID %u, RegisterID %u, bTrunDisp %d ,nTrunDispTime %d, TurnDisp %u\n", guiActiveID, RegisterID, g_guiMain.bTrunDisp, g_guiMain.nTrunDispTime, TurnDisp);
		// 轮显页面库操作判定
		if((getAuthor() == 1 || (guiActiveID != 0 && guiActiveID == RegisterID)) && g_guiMain.bTrunDisp == FALSE)
		{// 判定是否进入轮显操作模式
			if(TurnDisp == TRUE)
			{
//				StrartTrunDisp();
			}
			else
			{// 执行轮显计数,超时未操作，权限退回给桌面gui
				g_guiMain.nTrunDispTime ++;
				if(g_guiMain.nTrunDispTime > g_guiMain.nTrunDispTimeMax)
				{
				    appGUI_send_req_handle(MESSAGE_IID_DESKGUI, DESKGUI_IOP_APP_UNREG, NULL, 0);
					if(pAppDev != NULL)
            		{
            			ret = pAppDev->lcd_active_id_set(pAppDev,0);
                        if(ret != 0)
                        {
                            GUI_FMT_DEBUG("lcd_active_id_set:activeID = 0 ret %d\n", ret);
                            ret = pAppDev->lcd_active_id_set(pAppDev,0);
                        }
						g_guiComm.bPopList = FALSE;
            			GUI_FMT_DEBUG("显示app:%s 超过%d秒未操作，权限退回给桌面 ret %d\n",localAppName,g_guiMain.nTrunDispTimeMax, ret);
            		}
                    closeAuthor();
                    
                	init_layer();
                    g_guiMain.nTrunDispTime = 0;
				}
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

		// 定时刷新页面库，因为页面需要实时刷新，比如信号值、时间值等，所以放在该处统一刷新。
		UpdateWindow(g_guiComm.hMainWnd, TRUE);

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
        {
			#ifndef ARM_LINUX
            KillTimer(g_guiComm.hMainWnd, TIMER_MAIN);
			KillTimer(g_guiComm.hMainWnd, TIMER_QUICK);
			#endif
            DestroyMainWindow(g_guiComm.hMainWnd);
            PostQuitMessage(g_guiComm.hMainWnd);
        }
        break;

        /*模拟按键消息*/
		case MSG_CHAR:
		{
			switch(wParam)
			{
			case 'w':
			case 'W':OnKeyMsg(KEY_UP);break;
			case 's':
			case 'S':OnKeyMsg(KEY_DOWN);break;
			case 'a':
			case 'A':OnKeyMsg(KEY_LEFT);break;
			case 'd':
			case 'D':OnKeyMsg(KEY_RIGHT);break;
			case 'e':
			case 'E':OnKeyMsg(KEY_OK);break;
			case 'q':
			case 'Q':OnKeyMsg(KEY_CANCEL);break;

			// phone按键，抄表按键
			case 'P':
			case 'p':OnKeyMsg(PHONE_KEY);break;
			case 'M':
			case 'm':OnKeyMsg(RD_METER_KEY);break;
			case 'T':
			case 't':OnKeyMsg(KEY_ADDTRUN);break;

			case 'B':
			case 'b':OnKeyMsg(START_UPGRADE_IR_KEY);break;
			case 'N':
			case 'n':OnKeyMsg(STOP_UPGRADE_IR_KEY);break;
			}
		}
        break;
        /*模拟按键消息结束*/
		case MSG_KEYUP:
		{
			switch(wParam)
			{
			case 103:OnKeyMsg(KEY_UP);break;
			case 108:OnKeyMsg(KEY_DOWN);break;
			case 105:OnKeyMsg(KEY_LEFT);break;
			case 106:OnKeyMsg(KEY_RIGHT);break;
			case 28:OnKeyMsg(KEY_OK);break;
			case 1:OnKeyMsg(KEY_CANCEL);break;
			case 10:OnKeyMsg(PHONE_KEY);break;
			case 11:OnKeyMsg(RD_METER_KEY);break;
			case 12:OnKeyMsg(KEY_ADDTRUN);break;
			case 13:OnKeyMsg(START_UPGRADE_IR_KEY);break;
			case 14:OnKeyMsg(STOP_UPGRADE_IR_KEY);break;
			}
		}
		break;
        case MSG_PAINT:OnPaint();break;
    	case MSG_TIMER:OnTimer(wParam);break;
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
{
	MAINWINCREATE window_info;
    window_info.dwStyle = WS_VISIBLE;
    window_info.dwExStyle = WS_EX_NONE;
    window_info.spCaption = "";
    window_info.hMenu = 0;
    //window_info.hCursor = GetSystemCursor(0);
    window_info.hCursor = 0;
    window_info.hIcon = 0;
    window_info.MainWindowProc = WinProc;
    window_info.lx = 0;
    window_info.ty = 0;
    window_info.rx = g_guiComm.nWidth;
    window_info.by = g_guiComm.nHeight;
	GUI_FMT_DEBUG("width %d--height %d\n", g_guiComm.nWidth, g_guiComm.nHeight);
    window_info.iBkColor = COLOR_lightwhite;
    window_info.dwAddData = 0;
    window_info.hHosting = HWND_DESKTOP;
    g_guiComm.hMainWnd = CreateMainWindow(&window_info);
    if(g_guiComm.hMainWnd == HWND_INVALID)
		return FALSE;
    return TRUE;
}

pthread_t	timer_thread_id;
pthread_t   gOutMsgFlag;

time_t g_nMainTimer = -1;
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
		commfun_selectsleep(0, 100000);			/*定时100ms*/
/*
		g_circleNo++;
		if(g_circleNo == 10)
		{
			if(g_getdbstate == TRUE && g_firstread == TRUE)
			{
				//g_read_sc长度，根据读取的数据个数来确定
				if(app_report_getdb(g_read_sc, g_hUdbCenter) == TRUE)
				{
					//GUI_BUF_DEBUG(g_read_sc, 11, "read_sc:");
					g_firstread = FALSE;
					}
				}
			g_circleNo = 0;
		}
		*/
	}
	pthread_exit(&timer_thread_id);
}

pthread_t	timer_dispose_id;
int g_nUpdate_Flag = 0;
#if HARDWARE_TYPE == 4
int g_routerupdata_flag = 0;
int UsbFirstInsert = 1;
#endif


#if USB_CLEAN_ACDATA== 1
int g_nClearAC_Flag = 0;	//清除交采标志  0 未检测到文件  1 已经检测到文件  2 等待用户处理处理
#endif
/*

void* timer_dispose_thread(void *arg)
{
    char czBuf[20];
	#if USB_CLEAN_ACDATA== 1
	static time_t t_old = 0;		//距离1970年1月1日0时0分0秒的秒数
	time_t t_new = 0;
	long t_tmp = 0;
	#endif
	while(1)
	{
		if(g_bTimerDispose == 1)
		{
			g_bTimerDispose = 0;
			#if HARDWARE_TYPE == 2 || HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4
			//GetStatusInfo();
			GetNewStatusInfo();
			#endif

			// 查询是否请求升级
			if(g_nUpdate_Flag == 0)
			{
				//int nRet = ReadRamXData(Update_Flag, czBuf);
				int nRet = -1;
				if(nRet > 0 &&
					((unsigned char)czBuf[0] == 0x01 || (unsigned char)czBuf[0] == 0x04))
				{
					g_nUpdate_Flag = (unsigned char)czBuf[0];// 确认请求升级
				}
			}

            #if HARDWARE_TYPE == 4
            //检测U盘的路由升级包
            if((time(NULL)%5) == 0)
            {
                if(access("/tmp/udisk/router_update_file", F_OK) >= 0)//如果检测到U盘的升级包
                {
                    if(UsbFirstInsert)
                    {
                        UsbFirstInsert = 0;
                        system("cp -f /tmp/udisk/router_update_file  "ROUTER_UPDATE_FILE"");
                    }
                }
				else
				    UsbFirstInsert = 1;
            }

			int nRet;
			//nRet = ReadRamXData(ROUTER_UPDATA_FLAG,czBuf);
			nRet = -1;
			if(nRet > 0)
            {
                if(czBuf[0] == 0x55)
                {
		            printf("已经检测到合法的路由升级文件\n");
                    g_routerupdata_flag = 1;// 确认请求升级
                }
            }
			#endif

			#if MODULARIZATION_TMN_EN == 1
			//nRet = ReadRamXData(MDL_UPDATA_FLAG,czBuf);
			nRet = -1;
			if(nRet > 0)
            {
                if(czBuf[0] == 0x55)
                {
		            printf("已经检测到合法的模组升级文件\n");
                    g_routerupdata_flag = 2;// 确认请求升级
                }
            }
			#endif

			#if USB_CLEAN_ACDATA== 1
		  	t_new = time(NULL);
			t_tmp = t_new - t_old;
			if(t_tmp > 2 || t_tmp < 0)		//5秒钟检测一次文件
			{
				//printf("检测文件	g_nClearAC_Flag = %d\n",g_nClearAC_Flag);
				t_old = time(NULL);	//记录上次检测时间
				if(access("/tmp/udisk/clearjcdata.tar.gz", F_OK) < 0)
					g_nClearAC_Flag = 0;
				else
					if(g_nClearAC_Flag == 0)
						g_nClearAC_Flag = 1;
			}
			#endif



		}
		else
		{
			usleep(20000);
		}
	}
	pthread_exit(&timer_dispose_id);

}
*/

#define KBD_DEV		"/dev/s3c2410_kbd"

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
pthread_t	keyboard_thread_id;

BOOL g_bUpPress = FALSE;
BOOL g_bDownPress = FALSE;
BOOL g_bLeftPress = FALSE;
BOOL g_bRightPress = FALSE;
BOOL g_bOKPress = FALSE;
BOOL g_bCancelPress = FALSE;

BOOL g_bPress = FALSE;
void* keyboard_thread(void *arg)
{

	uint8 keyboardtype = 0;
	KEY_VALUE_T 	key_val;
	/*打开按键设备文件*/

	KEY_DEVICE_T* dev = (KEY_DEVICE_T*)hal_device_get(HW_DEVICE_ID_KEY);
	if(NULL == dev)
	{
		printf("dev is null\n");
		//return -1;
		exit(1);
	}

	unsigned char data;

//	int UPPRESS = UPPRESS_FIR;
//	int DWPRESS = DWPRESS_FIR;
//	int LTPRESS = LTPRESS_FIR;
	int RTPRESS = RTPRESS_FIR;
	int OKPRESS = OKPRESS_FIR;
	//if(access(SEC_FILE, F_OK) >= 0)
	{
		keyboardtype = 1;
//		UPPRESS = UPPRESS_SEC;
//		DWPRESS = DWPRESS_SEC;
//		LTPRESS = LTPRESS_SEC;
		RTPRESS = RTPRESS_SEC;
		OKPRESS = OKPRESS_SEC;
	}
	//KEY_VALUE_T akey[10];

	while(1)
	{
		if(((getAuthor() != 1 && (guiActiveID == 0 || guiActiveID != RegisterID))) || TurnDisp == TRUE)
		{
			//printf("键盘休眠!\n");
			usleep(200000);
			continue;
		}
		
		if(dev->key_value_get(dev, &key_val, sizeof(KEY_VALUE_T)) >= 0)
		{
            g_guiMain.nTrunDispTime = 0;
            
			data = key_val.key;
//					printf("press key %02X\n", data);
			//g_bPress = ((data&0x80)==0)?TRUE:FALSE;
			g_bPress = key_val.status;
			data = data&0x7F;
			GUI_FMT_TRACE("getAuthor %u guiActiveID %u press key %02X, g_bPress = %02x,data = %d\n",getAuthor() , guiActiveID, key_val.key, g_bPress, data);
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
					//case 1:g_bCancelPress = g_bPress?TRUE:FALSE;break;
				}
			}
		

			if(g_bPress == TRUE && g_bLeftPress == TRUE)
			{
				if(data==OKPRESS)// 左   , 效果等同取消
				{
					g_nKeyCode=20;
					g_bKeyboard=1;
					g_bLeftPress = FALSE;
				}
			}
			else if(g_bPress == TRUE && g_bOKPress == TRUE)
			{
				
				if(data==RTPRESS)// 右
				{// 显示隐藏菜单按键
					g_nKeyCode=15;
					g_bKeyboard=1;
					g_bOKPress = FALSE;
				}
				
			}

			if(g_bPress == TRUE && data == CLEPRESS_SEC)		//取消键
			{
				g_nKeyCode=20;
				g_bKeyboard=1;
			}
			else if(g_bKeyboard != 1 && g_bPress == TRUE)
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
		{
			g_nKeyCode = 0;
			//printf("键值获取失败!\n");
		}
			

		usleep(20000);
	}
	hal_device_release((HW_DEVICE *)dev);
	pthread_exit(&keyboard_thread_id);

}

#define YC_DEV      "/dev/talk"
#define BM_DEV		"/dev/baomi"
pthread_t	keyboard_ex_thread_id;
BOOL g_bHardWareSafety = FALSE;
void* keyboard_ex_thread(void *arg)
{
	//int yc_fd,bm_fd;
	//unsigned int data = 127;
	//unsigned int val = 127;

	//unsigned int nBaoPress = 0x37;

	//unsigned int nCaoPress = 0x37;

	BOOL bOpenYc = FALSE;
	BOOL bOpenBm = FALSE;

	while(1)
	{
#if DRIVER_CAOBIAO == 1
		int nCaobiaoCnt = 0;
		if(bOpenYc != TRUE)
		{
			/*打开预抄设备文件*/
			if((yc_fd = open(YC_DEV, O_RDONLY)) < 0)
			{
				printf("Error opening %s device\n", YC_DEV);
			}
			else
			{
				bOpenYc = TRUE;
			}
		}
		else
		{
			if(read(yc_fd, &data, sizeof data) == sizeof(data))
			{
				//printf("yc_fd data is %d\n", data);
				if(nCaoPress != data)
				{
					nCaoPress = data;
					if(data == 0)
					{
						nCaobiaoCnt = 0;
					}
					else
					{
						nCaobiaoCnt = 51;
					}
				}

				if(nCaobiaoCnt == 50)
				{
					g_nKeyCode=11;
					g_bKeyboard=1;
					nCaobiaoCnt = 51;
				}
				else if(nCaobiaoCnt < 50)
				{
					nCaobiaoCnt ++;
				}
			}
		}
#endif

#if DRIVER_BAOMI == 2
		if(bOpenBm != TRUE)
		{
			/*打开保密设备文件*/
			if((bm_fd = open(BM_DEV, O_RDONLY)) < 0)
			{
				printf("Error opening %s device\n", BM_DEV);
			}
			else
			{
				bOpenBm = TRUE;
			}
		}
		else
		{
			if (read(bm_fd, &val, sizeof val) == sizeof(val))
			{
				if(nBaoPress != val)
				{
					nBaoPress = val;
					g_bHardWareSafety = (val == 0)?TRUE:FALSE;// == 0时防护键按下
				}
			}
		}
#endif
        if(bOpenBm != TRUE && bOpenYc != TRUE)
        {
            usleep(1000000);
        }
		usleep(20000);
	}
	pthread_exit(&keyboard_ex_thread_id);
}

BOOL g_bRunPaint = FALSE;

void DisposeMsg(BOOL bMsgBox)//-显示主处理函数-
{
	//char czBuf[20];
    uint8 keyboardtype = 0;
	static BOOL lastLightState;
	//if(access(SEC_FILE, F_OK) >= 0)
	{
	    keyboardtype = 1;
	}
	static char lastAuthor = 0;
	for(;;)
	{
		if(app_dev != NULL)
		{
			RegisterID = app_dev->app_regist_id;
			app_dev->lcd_active_id_get(app_dev,&guiActiveID);
		}

        liveAppID = guiActiveID;
		//printf("%s:消息权限:%d,guiActiveID = %d,RegisterID = %d,app_dev = %d\n\n",__FUNCTION__,getAuthor(),guiActiveID,RegisterID,(int)app_dev);
		if(lastAuthor != getAuthor() && lastGuiActiveID != guiActiveID)
		{	
			//printf("权限变化了!\n");
			GUI_FMT_DEBUG("lastAuthor %d getAuthor %d guiActiveID = %d,RegisterID = %d\n", lastAuthor, getAuthor(), guiActiveID,RegisterID);
			if(lastAuthor == 0 || (guiActiveID != 0 && guiActiveID == RegisterID))	//画面切换到当前
			{
//				g_guiMain.nTrunDispTime = 0;
			}
            
            if(lastAuthor == 1 || guiActiveID != RegisterID)	//画面切换其他APP
            {
                closeAuthor();
                g_bAllMenu = FALSE;
            }
			lastGuiActiveID = guiActiveID;
			lastAuthor = getAuthor();
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
                switch(g_nKeyCode)
                {
                case UPPRESS_SEC:OnKeyMsg(KEY_UP);break;
    			case DWPRESS_SEC:OnKeyMsg(KEY_DOWN);break;
    			case LTPRESS_SEC:OnKeyMsg(KEY_LEFT);break;
    			case RTPRESS_SEC:OnKeyMsg(KEY_RIGHT);break;
    			case OKPRESS_SEC:OnKeyMsg(KEY_OK);break;
    			#if MODULARIZATION_TMN_EN == 1
    			case 20:OnKeyMsg(KEY_CANCEL);break;
    		    #endif
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
    			#if MODULARIZATION_TMN_EN == 1
    			case 20:OnKeyMsg(KEY_CANCEL);break;
    		    #endif
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

		if((lastLightState != g_guiMain.bLighton) && (getAuthor() || (guiActiveID != 0 && guiActiveID == RegisterID)))
		{
			if(g_guiMain.bLighton == TRUE)
			{
				g_guiMain.nLightonTime = 0;
				lighton_lcd();
			}
			else 
				lightoff_lcd();
			lastLightState = g_guiMain.bLighton;
		}
		usleep(20000);
	}
}
//#endif
#if BOOTSCREEN == 1
int TheBootScreen()
{
	BITMAP	mapbs;	// 是否上线
	char czBuf[100];
	#if HARDWARE_TYPE == 1			//一型终端
	sprintf(czBuf, "%s%s", ICON_PATH, "bootscreen1.bmp");
	g_guiComm.nWidth = 240;
	g_guiComm.nHeight= 128;
	#elif HARDWARE_TYPE == 2		//二型终端
	sprintf(czBuf, "%s%s", ICON_PATH, "bootscreen2.bmp");
	#elif HARDWARE_TYPE == 3		//三型终端
	#if LIAONING_FUN == 1
	sprintf(czBuf, "%s%s", ICON_PATH, "bootscreen3_ln.bmp");
        #elif HUABEI_FUN == 1
        sprintf(czBuf, "%s%s", ICON_PATH, "huabei_3.bmp");
	#else
	sprintf(czBuf, "%s%s", ICON_PATH, "bootscreen3.bmp");
	#endif
	#elif HARDWARE_TYPE == 4		//集中器终端
        #if HUABEI_FUN == 1
        sprintf(czBuf, "%s%s", ICON_PATH, "huabei_jzq.bmp");
        #else
	sprintf(czBuf, "%s%s", ICON_PATH, "bootscreen4.bmp");
        #endif
	#else
	sprintf(czBuf, "%s%s", ICON_PATH, "bootscreen3.bmp");
	#endif
	//sprintf(czBuf, "%s%s", ICON_PATH, "bootscreen3.bmp");

	LoadBitmapFromFile(HDC_SCREEN, &mapbs, czBuf);
    InitMainWindow();   // 定义以及创建窗口

	HDC hdcDst=BeginPaint(g_guiComm.hMainWnd);
	HDC hdc = CreateCompatibleDC(hdcDst);
	FillBox(hdc, 0, 0, g_guiComm.nWidth, g_guiComm.nHeight);

	FillBoxWithBitmap(hdc, 0, 0, g_guiComm.nWidth, g_guiComm.nHeight, &mapbs);
	BitBlt(hdc, 0, 0, g_guiComm.nWidth, g_guiComm.nHeight, hdcDst, 0, 0, 0);

	DeleteCompatibleDC(hdc);
	EndPaint(g_guiComm.hMainWnd, hdcDst);
	UnloadBitmap(&mapbs);

	return TRUE;
}
#endif

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
        GUI_FMT_TRACE("get err path!");
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
#ifdef PRODUCT_ZCU_S
        task_msg_prepare("/tmp/ipc_socket");
#else
        task_msg_prepare(stringvalue);
#endif

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
    diag_init(filename, TRUE);

    return 0;
}

static void signal_handler(int signum,siginfo_t* sigInfo,void* context)
{
	if(signum == SIGINT)	//暂定SIGINT，方便手动外部切换
	{
		turnAuthor();
		//appAuthority = 1- appAuthority;
		printf("收到信号，切换控制权, appAuthority = %d \n",getAuthor());
	}
	else
	{
		printf("未处理的信号：%d\n",signum);
	}
}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    作为程序的入口，主要进行一些初始化操作
	输入参数：    argc：int类型，进程入口参数个数
                  argv：char类型指针，输入的入口参数信息数组。
	输出参数：
	函数返回值：  int：进程退出时的返回值。0表示正常返回，否则表示含有错误
	内部处理流程：
********************************************************************/

int GUIMain(int argc,const char **argv)
{		
	struct sigaction	act;
	memset(&act,0,sizeof(struct sigaction));
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = signal_handler;
	//sigaction(SIGINT,&act,NULL);

	//JoinLayer("app_layer", "test", 0,16,160,144);
	
	app_dev = (LCD_DEVICE_T*)hal_device_get(HW_DEVICE_ID_LCD);
	if(app_dev != NULL)
		RegisterDev(app_dev,NULL);
	else
	{
		GUI_FMT_DEBUG("hal_device_get failed,app_dev is NULL!\n");
		return -1;
    }
	g_getdbstate = FALSE;
	g_firstread = TRUE;
	memset(g_read_sc, 0, sizeof(g_read_sc));
	int pTime=0;
	int speGuiFlag = 0;

	#if SpeBOOTSCREEN==1
	int Ret =0;
	char czBuf[5];
	memset(czBuf,0,sizeof(czBuf));

	//int nRet = InitMem(TRUE, TRUE);
	#endif

	#if BOOTSCREEN == 1
	#if SpeBOOTSCREEN==1
	Ret = read_from_eeprom(czBuf, FM_GUI_MEAIC_TYPE, 2);
	if(Ret>0 && (czBuf[0] == 'P' ||czBuf[0] == 'p') && (czBuf[1] == '3' ||czBuf[1] == '4'))
	{
		speGuiFlag = 1;
	}
	else
	#endif
	//TheBootScreen();
	#endif
	InitMainWindow();   // 定义以及创建窗口
	
	/*gdb测试*/
	if(argc >= 2)
    {
        if(0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            printf("30s waiting...\n");
            sleep(30);
        }
    }

	Initialize(NULL, FALSE);       // 初始化信息
	printf("run in GB version!\n");
	printf("run in arm_linux\n");
    read_pdata_xram(GUI_PARA, (char *)&toolInfoFresh.keyStatus, Esam_Status, Esam_Status_Len);
    GUI_FMT_DEBUG("[工具栏]toolInfoFresh.keyStatus = %d\n",toolInfoFresh.keyStatus);
	if(pthread_create(&timer_thread_id, NULL, timer_thread, NULL) != 0)
	{
		printf("error create timer thread\n");
		exit(EXIT_FAILURE);
	}
/*
    if(pthread_create(&timer_dispose_id, NULL, timer_dispose_thread, NULL) != 0)
	{
		printf("error create timer thread\n");
		exit(EXIT_FAILURE);
	}
*/
	if(pthread_create(&keyboard_thread_id, NULL, keyboard_thread, NULL) != 0)
	{
		printf("error create keyboard thread\n");
		exit(EXIT_FAILURE);
	}


#if (DRIVER_CAOBIAO == 1 || DRIVER_BAOMI == 2)
    // 其他按键检测，不想放在按键线程中，以免影响效率
    if(pthread_create(&keyboard_ex_thread_id, NULL, keyboard_ex_thread, NULL) != 0)
    {
        printf("error create keyboard thread\n");
        exit(EXIT_FAILURE);
    }
#endif

#ifdef _LITE_VERSION
    SetDesktopTect(0, 0, g_guiComm.nWidth, g_guiComm.nHeight);
#endif

	#if BOOTSCREEN == 0
	printf("InitMainWindow!\n");
    InitMainWindow();   // 定义以及创建窗口
    if(GUI_LOGON_LAYER != 0xFFFFFFFF && GetPlugIn(GUI_LOGON_LAYER) != NULL)
    {
        PopLayer(GUI_LOGON_LAYER);
		g_nLogoTime = 0;
		g_bLogo = TRUE;
    }
	#else
	#if SpeBOOTSCREEN==1
	if (speGuiFlag == 1)
	{
		pTime = 5;
	}
	else
	#endif
    {
        #if SICHUAN_FUN == 1
		pTime = 20;
		#elif HUABEI_FUN == 1
		pTime = 50;
        #else
		pTime = 5;
        #endif
    }

	//app_report_getdb(g_hUdbCenter);

	uint8 lasttime = 0;
	time_t starttime= time(NULL);
	while(1)
	{
		if(starttime != time(NULL))
		{
			starttime = time(NULL);
			lasttime ++;
			//KeepMeAlive();
		}

		if((lasttime >= pTime)||(g_nKeyCode==19))
			break;
	}
	#endif

	#if SpeBOOTSCREEN==1
	if (speGuiFlag == 1)
	{
		InitMainWindow();   // 定义以及创建窗口
		if(GUI_LOGON_LAYER != 0xFFFFFFFF && GetPlugIn(GUI_LOGON_LAYER) != NULL)
		{
			PopLayer(GUI_LOGON_LAYER);
			g_nLogoTime = 0;
			g_bLogo = TRUE;
		}
	}
	#endif

	//g_ethInfo.ethstate[0].state = 1;
	//GetAdvEthState();
	//GetPPPState();

	OnPaint();

	DisposeMsg(FALSE);
	DestroyMainWindow(g_guiComm.hMainWnd);

    // 释放操作
    MainWindowThreadCleanup(g_guiComm.hMainWnd);
	Uninitialize();
	hal_device_release((HW_DEVICE *)app_dev);
    return 0;
}

/**********************************************************************
* @brief     ：APP启动显示线程
* @param[in] ：
* @param[out]：   
* @return    ：
* @Create    : 梅安宁
* @Date      ：
* @Update    :
**********************************************************************/
void* app_gui_entry(void*)
{
	int iRet = 0;
	if(InitGUI(0,NULL) != 0)
	{
		//return 1;
	}
	iRet = GUIMain(0, NULL);
	TerminateGUI(iRet);
	return (void*)NULL;
}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    绘制MessageBox页面
	输入参数：    HDC hdc：绘制设备上下文
	输出参数：
	函数返回值：  void
	内部处理流程：
********************************************************************/
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

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    处理按键消息
	输入参数：    int nKey：按键消息
	输出参数：
	函数返回值：  BOOL：成功返回TRUE，失败返回FALSE
	内部处理流程：
********************************************************************/
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

/************************************************************************/
/* 弹出菜单界面                                                         */
/************************************************************************/
typedef struct _POP_LIST
{
    const char *pczList[96];// 最大30个选项
    int     nLine;      // 行
    int     nSel;       // 当前选择项
	int		nIndex;		// 显示的首行
    BOOL    bBigFont;

    int     nLeft;      // position
    int     nTop;       // position
    int     nRight;     // 弹出的最大宽度
    int     nBottom;    // 弹出的最大高度
}POP_LIST;
POP_LIST g_pop_list;
#define MAX_POP_LINE	6
/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    弹出菜单
	输入参数：    char* pList[]：菜单列表字符串
				  int nLine：菜单项数
				  char* pczSel：选中的菜单字符串
				  int nLeft, int nTop：菜单的左上角位置
				  BOOL bBigFont：是否大字体
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
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
/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    画弹出菜单，在页面库的Draw中调用该函数
	输入参数：    HDC hdc：画面设备上下文
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
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
/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    处理菜单的按键，需先判断是否弹出菜单
	输入参数：    int nKey：按键消息
				  int* nSel：返回的选中项索引
	输出参数：	  char*：选中项字符串
	函数返回值：
	内部处理流程：
********************************************************************/
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
/************************************************************************/
/* 弹出菜单结束                                                         */
/************************************************************************/

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    密码验证，在一个时间内没有经过密码验证，会弹出验证框
	输入参数：
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
extern PLUGIN_INFO g_layerFreezeTime;
extern long GetIceOutTime();
BOOL ValidatePwd()
{
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

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif
