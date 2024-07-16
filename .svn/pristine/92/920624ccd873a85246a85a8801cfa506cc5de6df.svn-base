/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
  程序名称: main.h
  功    能：FKGA42显示模块菜单页面库实现文件。完成菜单的绘制以及按键消息的处理
  开 发 人：gaoyf
  开发时间：2007-1
  
  版本历史:
  
    v1.0:
      2006-1-8, gaoyf : New file   
     
  包含页面库：
    1列菜单页面  0x00A00000
    2列菜单页面  0x00A10000
	测量点专用菜单  0x00A20000
  
***************************************************************************
*/

#include "main.h"
#include "commdef.h"
#include "commfun.h"
#include "gui_prtl.h"


MENU_INFO* g_menuMain[1000];
int	g_nMenus = 0;
int g_nParentIndex = -1;
int g_nCurIndex = -1;   // 只用于"取消"返回上一级菜单时判定选择项使用，确定是设置为-1

int g_nCurMenuIndex[1000];
int g_nCurMenuCnt = 0;
int g_nCurSelect = 0;
int g_nMaxMenuChar = 0;

int g_narrMenuIndex[20] = {0};
int g_nMenuNodes = 1;
char g_czLayerCap[30];

int g_nLeft1 = 8;
int g_nLeft2 = 58;
int g_nLeft3 = 108;
int	g_nTop = 32;
int	g_nInterval = 0;

#if MODULARIZATION_TMN_EN == 1
int	g_ncancelkey = 0;

const char* g_icon_cancel[]=
{
	"offcancelkey.bmp",
	"oncancelkey.bmp",
};
//enum ICON_APP_INDEX
//{
//	APP_CALL,
//	APP_HOME,
//	APP_LOCAL,
//	APP_LOOP,
//	APP_MOUDLE,
//	APP_PARK,
//	APP_SYSTEM,
//};
//const char* g_icon_app[] = 
//{
//	"app_call.bmp",
//	"app_home.bmp",
//	"app_local.bmp",
//	"app_loop.bmp",
//	"app_module.bmp",
//	"app_park.bmp",
//	"app_system.bmp",
//};

MENU_ICON g_menu_icon;

/*-------------------------------------------------------------
*功能：左上角显示选中状态返回按钮
*处理：界面如果增加该组件，按确认后会执行取消操作返回上一菜单
*输入：x,y横纵坐标，w,h组件宽和高
*输出：无
*-------------------------------------------------------------
*/
void CancelKeyDraw(HDC hdc, int x, int y, int w, int h)
{
	char czBuf[100];
	if(g_menu_icon.bCancel[1] == 0)
	{
		sprintf(czBuf, "%s%s", ICON_PATH, g_icon_cancel[1]);
		LoadBitmapFromFile(HDC_SCREEN, &g_menu_icon.mapCancel[1], czBuf);
		g_menu_icon.bCancel[1] = 1;
	}
	FillBoxWithBitmap(hdc, x, y, w, h, &g_menu_icon.mapCancel[1]);
	g_guiComm.bOKiscancel = 1;
}
#endif

void CancelKeyDrawsub(HDC hdc, int x, int y, int w, int h, int i)
{
	char czBuf[100];

	if(i==0)
	{
		sprintf(czBuf, "%s%s", ICON_PATH, g_icon_cancel[0]);
		LoadBitmapFromFile(HDC_SCREEN, &g_menu_icon.mapCancel[0], czBuf);
		FillBoxWithBitmap(hdc, x, y, w, h, &g_menu_icon.mapCancel[0]);
		}
	else
	{
		sprintf(czBuf, "%s%s", ICON_PATH, g_icon_cancel[1]);
		LoadBitmapFromFile(HDC_SCREEN, &g_menu_icon.mapCancel[1], czBuf);
		FillBoxWithBitmap(hdc, x, y, w, h, &g_menu_icon.mapCancel[1]);
	}
}



/************************************************************************
 基本菜单页面
 完成功能:160*160页面上完成1竖列菜单的操作
*************************************************************************/
typedef struct _MENU_BASE_INFO
{
	int nMenuShows;// 在该页面上最多显示的菜单项
	int nMenuShowMin;
	int nMenuShowMax;
}MENU_BASE_INFO;
MENU_BASE_INFO g_menuBase = {8, 0, 0};
MENU_BASE_INFO g_menuThree = {24, 0 , 0};

BOOL IsMenuLayer(DWORD dwID)
{
	return (dwID==0x00A00000||dwID==0x00A10000||dwID==0x00A20000||dwID==0x00A80000)?TRUE:FALSE;
}

void MenuDraw(HDC hdc);
BOOL MenuKeyMsg(int nKey);
void InitMenu(DWORD dwPluginId);
PLUGIN_INFO g_menuLayer = {
	0x00A00000/*id*/, 
	InitMenu/*InitLayer*/,
	NULL/*UninitLayer*/,
	MenuDraw/*Draw*/,
	NULL/*Timer*/,
	MenuKeyMsg/*keymsg*/,
	NULL/*ipc*/
};

extern BOOL g_bAllMenu;
extern BOOL ReadConfig(BOOL bAll);

#if HARDWARE_2012_EN==1
static int n;
void SetparaInit(DWORD dwPluginID)
{
	n =0;
}
void SetparaDraw(HDC hdc)
{
//printf("Enter SetparaDraw\n");

	SelectFont(hdc, g_guiComm.fontBig);
	int nLeft1;
	//int nLeft2;
	int y;
	nLeft1 = 40;
	//nLeft2 = 140;
	y = 20;

	ITEM_NOT_SELECT;
	if (n == 0)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "时间设置");
	
	y += 16;
	ITEM_NOT_SELECT;
	if (n == 1)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "行政区码和终端地址的设置");

	y += 16;
	ITEM_NOT_SELECT;
	if (n == 2)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "通讯通道配置");
	
	y += 16;
	ITEM_NOT_SELECT;
	if (n == 3)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "测量点参数配置");
	
	y += 16;
	ITEM_NOT_SELECT;
	if (n == 4)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "密码的设置");

}

BOOL SetparaKeyMsg(int nKey)
{
	//printf("SetparaKeyMsg\n");
	switch (nKey)
	{
	case KEY_UP:
		if (n > 0)
		{
			n--;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		return (TRUE);
		break;
	case KEY_DOWN:
		if (n < 4)
		{
			n++;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		return (TRUE);
		break;
		case KEY_OK:
			switch (n)
			{
				case 0:
					PopLayer(0x00620001);
					break;	
				case 1:
					PopLayer(0x00630001);
					break;	
				case 2:
					PopLayer(0x00660000);
				break;	
				case 3:
					PopLayer(0x00640000);
					break;			
				case 4:
					PopLayer(0x00600011);
					break;
				default:
					break;
		}
		return (FALSE);
		break;
	default:
		return (FALSE);
		break;
	}
	
}

PLUGIN_INFO g_SetparaLayer = {
	0x11100000/*id*/, 
	SetparaInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	SetparaDraw/*Draw*/,
	NULL/*Timer*/,
	SetparaKeyMsg/*keymsg*/,
	NULL/*ipc*/
};
void SetParaMenu()
{
	ClosePopLayer();
	StopTrunDisp();

	if(ValidatePwd() != TRUE)
	{
		return;
	}
	if(g_bAllMenu == TRUE)
	{
		if(TRUE != MessageBox("提示\n\n是否进入设置模式？", MB_OKCANCEL))
		{
			return;
		}
	}
	else
	{
		if(TRUE != MessageBox("提示\n\n是否进入设置模式？", MB_OKCANCEL))
		{
			return;
		}
	}
	PopLayer(0x11100000);

	
	//UpdateWindow(g_guiComm.hMainWnd, TRUE);

}
#endif

/**********************************************************************
* @name      : change_display_model
* @brief     ：桌面显示模式切换
* @param[in] ：g_bAllMenu TRUE 隐藏菜单显示；FALSE，正常显示
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-10
* @Update    :
**********************************************************************/
void change_display_model()
{
	ClosePopLayer();
	StopTrunDisp();

	g_bAllMenu = !g_bAllMenu;
	
	ASSERT(g_guiMain.pluginCur != NULL);
	if(g_guiMain.pluginCur->pfnUninitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnUninitLayer();
	}

	// 根据主菜单风格ID，确定当前的plug
	if(g_bAllMenu == TRUE)
	{
//	    RestoreConfigFromXml(NULL);
        ReadConfig(g_bAllMenu);
		g_guiMain.nMenuRootId = 0x00A00000;
	    g_guiMain.pluginCur = GetPlugIn(0x00A00000);

	}
    else
	{
	    load_app_icon(g_bAllMenu);

	}
        
	ASSERT(g_guiMain.pluginCur != NULL);
	
	g_nParentIndex = -1;
	g_nCurIndex = -1;
	g_nCurMenuCnt = 0;
	g_nCurSelect = 0;
	g_nMaxMenuChar = 0;
	
	// 初始化当前页面库，在Uninitialize中释放
	ASSERT(g_guiMain.pluginCur != NULL);
	g_guiComm.bCanTrunDisp = TRUE;
	if(g_guiMain.pluginCur->pfnInitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
	}
	UpdateWindow(g_guiComm.hMainWnd, TRUE);
}

void ShowOrHideMenu()
{
	if(ValidatePwd() != TRUE)
	{
		return;
	}
    g_bpop = FALSE;
    
	if(g_bAllMenu == TRUE)
	{
		if(TRUE != MessageBox("提示\n\n是否关闭扩展菜单项？", MB_OKCANCEL))
		{
			return;
		}
	}
	else
	{
		if(TRUE != MessageBox("提示\n\n是否显示扩展菜单项？", MB_OKCANCEL))
		{
			return;
		}
	}

    change_display_model();
}

void SetArrow()
{
	if(g_menuThree.nMenuShowMin > 0
		&& g_menuThree.nMenuShowMax < g_nCurMenuCnt-1)
	{// 显示上下箭头
		g_guiComm.nArrow = 0;
	}
	else if(g_menuThree.nMenuShowMin > 0)
	{// 显示上箭头
		g_guiComm.nArrow = 1;
	}
	else if(g_menuThree.nMenuShowMax < g_nCurMenuCnt-1)
	{// 显示下箭头
		g_guiComm.nArrow = 2;
	}
	else
	{
		g_guiComm.nArrow = -1;
	}
}
/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    初始化菜单，动态创建当前菜单数组
	输入参数：    DWORD dwPluginId：菜单页面库ID
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
void InitMenu(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;
	g_guiComm.nArrow = -1;
	g_guiComm.nCurPoint = 0;
	int i;
	MENU_INFO* pMenu;
	g_nCurMenuCnt = 0;

	if (g_nParentIndex != -1)
	{
		pMenu = g_menuMain[g_nParentIndex];
		g_nLeft1 = pMenu->nLeft1;
		g_nLeft2 = pMenu->nLeft2;
		g_nTop = pMenu->nTop;
		g_nInterval = pMenu->nInterval;
		g_menuBase.nMenuShows = pMenu->nMenuMaxCnt;
	}
	else
	{
		g_nLeft1 = g_guiMain.nLeft1;
		g_nLeft2 = g_guiMain.nLeft2;
		g_nTop = g_guiMain.nTop;
		g_nInterval = g_guiMain.nMenuInterval;
		g_menuBase.nMenuShows = g_guiMain.nMenuMaxCnt;
	}

//    printf("g_nParentIndex %d \n", g_nParentIndex);
	// 根据父菜单id生成子菜单项
	g_nMaxMenuChar = 0;
	int nTmpChar;
	for (i = 0; i < g_nMenus; i++)
	{
		pMenu = g_menuMain[i];
		ASSERT(pMenu != NULL);
		if (pMenu->nParent == g_nParentIndex)
		{
			g_nCurMenuIndex[g_nCurMenuCnt] = i;
			g_nCurMenuCnt++;

			nTmpChar = strlen(pMenu->czCaption);
			if (nTmpChar > g_nMaxMenuChar)
			{
				g_nMaxMenuChar = nTmpChar;
			}
		}
	}

	// 设置当前选择项
	g_nCurSelect = 0;
	if (g_nCurIndex != -1)
	{
		// ==-1表示从"确定"按键进入，所以当前选择项为第一项
		// !=-1表示从"取消"按键进入，需要判定子菜单的父菜单项为当前选择项
		for (i = 0; i < g_nCurMenuCnt; i++)
		{
			if (g_nCurMenuIndex[i] == g_nCurIndex)
			{
				g_nCurSelect = i;
				break;
			}
		}
	}

	// 设置显示区域，因为可能菜单项个数比较多，不能全屏显示
	// 此处需要保证选中菜单处于显示区域中
	if (g_nCurSelect > g_menuBase.nMenuShows - 1)
	{// 如果当前选择项已经大于能够显示菜单项的最大值
		// 那么最大显示项为当前选择项，依此判定出最小选择项
		g_menuBase.nMenuShowMax = g_nCurSelect;
		g_menuBase.nMenuShowMin = g_menuBase.nMenuShowMax - g_menuBase.nMenuShows + 1;
	}
	else
	{// 否则最小显示项为0
		g_menuBase.nMenuShowMin = 0;
		g_menuBase.nMenuShowMax = (g_nCurMenuCnt > g_menuBase.nMenuShows) ? g_menuBase.nMenuShows - 1 : g_nCurMenuCnt - 1;
	}
	SetArrow();
}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    绘制动态菜单
	输入参数：    HDC hdc：绘制设备上下文句柄
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
void MenuDraw_add_name(HDC hdc, const char* pname)
{
	int i = 0;
	int nIndex;
	MENU_INFO* pMenu;

	char czBuf[300];

	SelectFont(hdc, g_guiComm.fontBig);


	int Top = g_nTop - 10;

	ASSERT(g_nCurMenuCnt > 0);
	ITEM_NOT_SELECT
		RECT rc;
	rc.left = 0;
	rc.right = g_guiComm.nWidth;

	if (g_nParentIndex == -1)
	{
		sprintf(czBuf, "%s", pname);
		rc.top = 20;
		rc.bottom = 36;
		DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER);
		Top += 18;
	}
	//定义菜单显示长度数组
	int Lenths[100] = { 0 };
	//返回箭头不显示
/*
	#if MODULARIZATION_TMN_EN
	if(g_ncancelkey == 0)
	{
		if(g_menu_icon.bCancel[0] == 0)
		{
			sprintf(czBuf, "%s%s", ICON_PATH, g_icon_cancel[0]);
			sprintf(czBuf, "%s%s", ICON_PATH, "ok.bmp");
			LoadBitmapFromFile(HDC_SCREEN, &g_menu_icon.mapCancel[0], czBuf);
			g_menu_icon.bCancel[0] = 1;
		}
		FillBoxWithBitmap(hdc, 3, 16, 20, 20, &g_menu_icon.mapCancel[0]);
	}
	else
	{
		if(g_menu_icon.bCancel[1] == 0)
		{
			sprintf(czBuf, "%s%s", ICON_PATH, g_icon_cancel[1]);
			LoadBitmapFromFile(HDC_SCREEN, &g_menu_icon.mapCancel[1], czBuf);
			g_menu_icon.bCancel[1] = 1;
		}
		FillBoxWithBitmap(hdc, 3, 16, 20, 20, &g_menu_icon.mapCancel[1]);
	}
	#endif
*/
	int length = 0;
	for (i = g_menuBase.nMenuShowMin; i <= g_menuBase.nMenuShowMax; i++)
	{
		nIndex = g_nCurMenuIndex[i];
		ASSERT(nIndex >= 0 && nIndex < g_nMenus);
		pMenu = g_menuMain[nIndex];
		ASSERT(pMenu != NULL);

		sprintf(czBuf, "%d.%s", i + 1, pMenu->czCaption);
            
		int Size = strlen(czBuf);
		length = (160 - ((Size *8))) / 2;

		Lenths[i] = length;
	}

	//找出数组中最小值


	
	length=Lenths[g_menuBase.nMenuShowMin];
	for (i = g_menuBase.nMenuShowMin; i <= g_menuBase.nMenuShowMax; i++)
	{
		if (length >= Lenths[i])
		{
			length = Lenths[i];
		}
	}

	for (i = g_menuBase.nMenuShowMin; i <= g_menuBase.nMenuShowMax; i++)
	{
		nIndex = g_nCurMenuIndex[i];
		ASSERT(nIndex >= 0 && nIndex < g_nMenus);
		pMenu = g_menuMain[nIndex];
		ASSERT(pMenu != NULL);

		rc.top = Top;
		rc.bottom = Top + 16;
		sprintf(czBuf, "%d.%s", i + 1, pMenu->czCaption);

#if HARDWARE_TYPE == 4 || (HARDWARE_TYPE == 3 && HUNAN_FUN == 1)        // 375.2规范要求集中器的主菜单不带序号
		//        sprintf(czBuf, "%s", pMenu->czCaption);
#endif

#if MODULARIZATION_TMN_EN
		if (g_nCurSelect == i && g_ncancelkey == 0)
#else
		if (g_nCurSelect == i)
#endif
		{
			// 设置反显操作
			ITEM_SELECT;
		}
		else
		{
		
			// 恢复反显，这样可以不需要总是设置颜色了
			ITEM_NOT_SELECT;
		}
		if (HARDWARE_TYPE == 1 || HARDWARE_TYPE == 2 || HARDWARE_TYPE == 3)
		{
			TextOut(hdc, length, rc.top, czBuf);
		}
		else if (HARDWARE_TYPE == 4) // 375.2规范要求集中器的主菜单不带序号
		{

			//DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER);
			TextOut(hdc, length, rc.top, czBuf);
		}
		if (g_nCurSelect == i)
		{
			// 恢复反显，这样可以不需要总是设置颜色了
			ITEM_NOT_SELECT;
		}
		Top += (16 + g_nInterval);
	}


}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    绘制动态菜单
	输入参数：    HDC hdc：绘制设备上下文句柄
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
void MenuDraw(HDC hdc)
{
	MenuDraw_add_name(hdc, ADV_SHOW_NAME);
}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    处理菜单按键消息
	输入参数：    int nKey：按键消息
	输出参数：
	函数返回值：  BOOL：如果处理返回TRUE,使用缺省处理返回FALSE
	内部处理流程：
********************************************************************/
extern void KeepNextLayer();
extern void SetSubLayerArrow();
void MenuOkKeyMsg()
{
	// 按确定键，表示当前项肯定为菜单
	// 进入下一个页面：可能为子菜单，也可能为功能页面
	int nIndex;
	MENU_INFO* pMenu;
	PLUGIN_INFO* pPlugin;
	
	// 获取当前选择项在总体菜单项中的索引值
	ASSERT(g_nCurSelect >= 0 && g_nCurSelect < g_nCurMenuCnt);
    
	nIndex = g_nCurMenuIndex[g_nCurSelect];
//    printf("g_nCurSelect %d nIndex %d\n", g_nCurSelect, nIndex);
    
	ASSERT(nIndex >= 0 && nIndex < g_nMenus);
	// 根据索引值获取对应的菜单项指针
	pMenu = g_menuMain[nIndex];
	ASSERT(pMenu != NULL&& pMenu->nPlugCnt > 0);
	
	// 获取菜单项对应的页面：子菜单或者第一个功能页面
	if(pMenu->bMenuAggregate == TRUE)
	{
		pPlugin = GetPlugIn(pMenu->dwMenuID);
	}
	else
	{
		pPlugin = GetPlugIn(pMenu->pPlugStruct[0].dwPlugin);
	}
	if(pPlugin == NULL)
	{
		ASSERT(FALSE);
		return;
	}
	
	// 对旧的当前页面进行释放操作
	ASSERT(g_guiMain.pluginCur != NULL);
	if(g_guiMain.pluginCur->pfnUninitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnUninitLayer();
	}
	
	g_guiMain.pluginCur = pPlugin;
	// 设置父菜单项信息，这是为了保证"取消"回退时，能够找到父菜单项
	// 这只对包含子菜单时起作用
	if(pMenu->bMenuAggregate == TRUE)
	{
		g_nParentIndex = nIndex;
	}
	
	g_narrMenuIndex[g_nMenuNodes] = 0;
	g_nMenuNodes ++;
	
	g_guiMain.pPrentMenu = pMenu;
	g_nCurIndex = nIndex;
	g_guiMain.nPageLayerIndex = 0;
	
	// 对新当前页面初始化操作
	g_guiComm.bKeyDown = TRUE;// 进入第一页时认为向下按键
	g_guiComm.bCanTrunDisp = TRUE;
	g_guiComm.bSkipLayer = FALSE;
	g_guiComm.bPointOffset = FALSE;
	g_guiComm.nArrow = -1;
	if(g_guiMain.pluginCur->pfnInitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
	}
	if(g_guiComm.bSkipLayer == TRUE)
	{
		KeepNextLayer();
	}
	SetSubLayerArrow();
	if(g_guiMain.pPrentMenu != NULL && g_guiMain.pPrentMenu->bMenuAggregate == TRUE)
	{
		SetArrow();
	}
	UpdateWindow(g_guiComm.hMainWnd, TRUE);
}

void MenuOkKeyMsg_Desk()
{
	int i;
    int ret = 0;
	for(i = 0; i < appGuiInfoSaved.loginAppCnt; i++)
	{
		printf("MenuOkKeyMsg_Desk: i=%d,g_nCurSelect=%d,appGuiInfoSaved.menuID[%d] = %d\n",
				i,g_nCurSelect,i,appGuiInfoSaved.menuID[i]);
		
		if(g_nCurSelect == appGuiInfoSaved.menuID[i])
		{
		    if(app_is_online(appGuiInfoSaved.loginID[i]))
            {      
    			app_rise_inform_event(appGuiInfoSaved.applogin[i].appname);
    			if(pGuiDev != NULL)
    			{
    				ret = pGuiDev->lcd_active_id_set(pGuiDev,appGuiInfoSaved.loginID[i]);
                    if(ret != 0)
                    {
                        GUI_FMT_DEBUG("lcd_active_id_set:activeID = %d ret %d\n", appGuiInfoSaved.loginID[i], ret);
                        ret =  pGuiDev->lcd_active_id_set(pGuiDev,appGuiInfoSaved.loginID[i]);
                    }
    				GUI_FMT_DEBUG("激活app:%s, activeID: %d ret %d\n",appGuiInfoSaved.applogin[i].appname,appGuiInfoSaved.loginID[i], ret);
    			}
            }
            else
            {  
//    			delete_app_info(appGuiInfoSaved.loginID[i]);
                
//                UpdateWindow(g_guiComm.hMainWnd, TRUE);
            }
			break;
		}
	}
	printf("%s:  i = %d, loginAppCnt = %d \n",__FUNCTION__,i ,appGuiInfoSaved.loginAppCnt );
	GUI_FMT_TRACE("%s:  i = %d, loginAppCnt = %d \n",__FUNCTION__,i ,appGuiInfoSaved.loginAppCnt );
	if(i >= appGuiInfoSaved.loginAppCnt)
	{
		GUI_FMT_DEBUG("桌面gui发送激活事件失败,g_nCurSelect = %d, appGuiInfoSaved.loginAppCnt = %d\n",
						g_nCurSelect,appGuiInfoSaved.loginAppCnt);
	}
}

extern void StrartTrunDisp();
void MenuCancelKeyMsg()
{
	MENU_INFO* pMenu;
	PLUGIN_INFO* pPlugin;
	if(g_nParentIndex == -1)
	{// 已经是主菜单了，不能在向上翻页了
//		StrartTrunDisp();
		return;
	}
	
	ASSERT(g_nParentIndex >= 0 && g_nParentIndex < g_nMenus);
	// 获取父菜单指针
	pMenu = g_menuMain[g_nParentIndex];
	ASSERT(pMenu != NULL && pMenu->nPlugCnt > 0);
	// 获取父菜单对应的菜单页面库指针
	if(pMenu->nParent == -1)
	{
        if(g_bAllMenu == TRUE)
    	{
    	    pPlugin = GetPlugIn(0x00A00000);

    	}
        else
    	{
    	    pPlugin = GetPlugIn(g_guiMain.nMenuRootId);
    	}
	
	}
	else
	{
		pPlugin = GetPlugIn(g_menuMain[pMenu->nParent]->dwMenuID);
	}
	//pPlugin = GetPlugIn(pMenu->pdwPlugin[0]);
	if(pPlugin == NULL)
	{
		ASSERT(FALSE);
		return;
	}
	g_nMenuNodes --;
	ASSERT(g_nMenuNodes > 0);
	
	// 对旧的当前页面进行释放操作
	ASSERT(g_guiMain.pluginCur != NULL);
	if(g_guiMain.pluginCur->pfnUninitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnUninitLayer();
	}
	
	// 设置为当前页面库
	g_guiMain.pluginCur = pPlugin;
	// 将当前的父菜单项ID设为调到上一个页面的当前选择项索引
	g_nCurIndex = g_nParentIndex;
	// 设置该菜单项的父菜单作为上一页面的父菜单项
	g_nParentIndex = pMenu->nParent;
	
	g_guiMain.pPrentMenu = pMenu;
	g_guiMain.nPageLayerIndex = 0;
	
	// 对新当前页面初始化操作
	g_guiComm.bCanTrunDisp = TRUE;
	g_guiComm.nArrow = -1;
	if(g_guiMain.pluginCur->pfnInitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
	}
	UpdateWindow(g_guiComm.hMainWnd, TRUE);
}
BOOL MenuKeyMsg(int nKey)
{
	switch (nKey)
	{
	case KEY_LEFT:
	{
		//返回箭头不显示
	/*
		#if MODULARIZATION_TMN_EN
		if(g_ncancelkey == 0)
		{
			g_ncancelkey = 1;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		break;
		#endif
	*/
	// 菜单选择项上移
		if (g_nCurSelect % g_menuBase.nMenuShows == 0)
		{
			g_nCurSelect -= g_menuBase.nMenuShows;
		}
		else
		{
			g_nCurSelect = g_nCurSelect / g_menuBase.nMenuShows * g_menuBase.nMenuShows;
		}
		if (g_nCurSelect < 0)
		{
			if (g_nCurMenuCnt % g_menuBase.nMenuShows == 0)
			{
				g_nCurSelect = g_nCurMenuCnt - g_menuBase.nMenuShows;
			}
			else
			{
				g_nCurSelect = g_nCurMenuCnt - (g_nCurMenuCnt % g_menuBase.nMenuShows);
			}
		}
		// 调整菜单显示区域，如果菜单项不能全显的情况下，
		// 选择项已经小于最小显示菜单项，将显示区域上移
		//if(g_nCurMenuCnt > g_menuBase.nMenuShows && g_menuBase.nMenuShowMin > g_nCurSelect)
		{
			g_menuBase.nMenuShowMin = g_nCurSelect;
			g_menuBase.nMenuShowMax = g_menuBase.nMenuShowMin + g_menuBase.nMenuShows - 1;
			if (g_menuBase.nMenuShowMax >= g_nCurMenuCnt)
			{
				g_menuBase.nMenuShowMax = g_nCurMenuCnt - 1;
			}
		}
		SetArrow();
		g_narrMenuIndex[g_nMenuNodes - 1] = g_nCurSelect;
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
	}
	break;
	case KEY_RIGHT:
	{
		/*
			#if MODULARIZATION_TMN_EN
			if(g_ncancelkey == 1)
			{
				g_ncancelkey = 0;
				UpdateWindow(g_guiComm.hMainWnd, TRUE);
			}
			break;
			#endif
			*/
			// 菜单选择项下移
		if (g_nCurSelect % g_menuBase.nMenuShows == 0)
		{
			g_nCurSelect += g_menuBase.nMenuShows;
		}
		else
		{
			g_nCurSelect = g_nCurSelect / g_menuBase.nMenuShows * g_menuBase.nMenuShows + g_menuBase.nMenuShows;
		}
		if (g_nCurSelect + 1 >= g_nCurMenuCnt)
		{
			g_nCurSelect = 0;
		}
		// 调整菜单显示区域，如果菜单项不能全显的情况下，
		// 选择项已经大于最大显示菜单项，将显示区域下移
		//if(g_nCurMenuCnt > g_menuBase.nMenuShows && g_nCurSelect+ > g_menuBase.nMenuShowMax)
		{
			g_menuBase.nMenuShowMin = g_nCurSelect;
			g_menuBase.nMenuShowMax = g_menuBase.nMenuShowMin + g_menuBase.nMenuShows - 1;
			if (g_menuBase.nMenuShowMax >= g_nCurMenuCnt)
			{
				g_menuBase.nMenuShowMax = g_nCurMenuCnt - 1;
			}
		}
		SetArrow();
		g_narrMenuIndex[g_nMenuNodes - 1] = g_nCurSelect;
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
	}
	break;
	case KEY_UP:
	{
		/*
			#if MODULARIZATION_TMN_EN
			if(g_ncancelkey == 1)
			{
				break;
			}
			#endif
			*/
			// 菜单选择项上移
		if (g_nCurSelect > 0)
		{
			g_nCurSelect--;
			// 调整菜单显示区域，如果菜单项不能全显的情况下，
			// 选择项已经小于最小显示菜单项，将显示区域上移
			if (g_nCurMenuCnt > g_menuBase.nMenuShows && g_menuBase.nMenuShowMin > g_nCurSelect)
			{
				g_menuBase.nMenuShowMin--;
				g_menuBase.nMenuShowMax = g_menuBase.nMenuShowMin + g_menuBase.nMenuShows - 1;
				if (g_menuBase.nMenuShowMax >= g_nCurMenuCnt)
				{
					g_menuBase.nMenuShowMax = g_nCurMenuCnt - 1;
				}
			}
		}
		else
		{
			g_nCurSelect = g_nCurMenuCnt - 1;
			if (g_nCurSelect > g_menuBase.nMenuShows - 1)
			{// 如果当前选择项已经大于能够显示菜单项的最大值
				// 那么最大显示项为当前选择项，依此判定出最小选择项
				g_menuBase.nMenuShowMax = g_nCurSelect;
				g_menuBase.nMenuShowMin = g_menuBase.nMenuShowMax - g_menuBase.nMenuShows + 1;
			}
			else
			{// 否则最小显示项为0
				g_menuBase.nMenuShowMin = 0;
				g_menuBase.nMenuShowMax = (g_nCurMenuCnt > g_menuBase.nMenuShows) ? g_menuBase.nMenuShows - 1 : g_nCurMenuCnt - 1;
			}
		}
		g_narrMenuIndex[g_nMenuNodes - 1] = g_nCurSelect;
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
	}
	break;
	case KEY_DOWN:
	{
		/*
			#if MODULARIZATION_TMN_EN
			if(g_ncancelkey == 1)
			{
				break;
			}
			#endif
			*/
			// 菜单选择项下移
		if (g_nCurSelect + 1 < g_nCurMenuCnt)
		{
			g_nCurSelect++;
			// 调整菜单显示区域，如果菜单项不能全显的情况下，
			// 选择项已经大于最大显示菜单项，将显示区域下移
			if (g_nCurMenuCnt > g_menuBase.nMenuShows && g_nCurSelect > g_menuBase.nMenuShowMax)
			{
				g_menuBase.nMenuShowMin++;
				g_menuBase.nMenuShowMax++;
			}
		}
		else
		{
			g_nCurSelect = 0;
			g_menuBase.nMenuShowMin = 0;
			g_menuBase.nMenuShowMax = (g_nCurMenuCnt > g_menuBase.nMenuShows) ? g_menuBase.nMenuShows - 1 : g_nCurMenuCnt - 1;
		}
		g_narrMenuIndex[g_nMenuNodes - 1] = g_nCurSelect;
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
	}
	break;
	case KEY_OK:
	{
		/*
			#if MODULARIZATION_TMN_EN
			if(g_ncancelkey == 1)
			{
				MenuCancelKeyMsg();

				g_ncancelkey = 0;
				break;
			}
			#endif
			*/
		MenuOkKeyMsg();
	}
	break;
	case KEY_CANCEL:
	{
		MenuCancelKeyMsg();
	}
	break;
	}
	return TRUE;
}
/************************************************************************
*************************************************************************/
void ThreeMenuDraw(HDC hdc);
BOOL ThreeMenuKeyMsg(int nKey);
void ThreeInitMenu(DWORD dwPluginId);
PLUGIN_INFO g_menu3Layer =
{
	0x00A50000,
	ThreeInitMenu,
	NULL,
	ThreeMenuDraw,
	NULL,
	ThreeMenuKeyMsg,
	NULL
};

void ThreeInitMenu(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;
	g_guiComm.nArrow = -1;
	g_guiComm.nCurPoint = 0;
	int i;
	MENU_INFO* pMenu;
	g_nCurMenuCnt = 0;

	if(g_nParentIndex != -1)
	{
		pMenu = g_menuMain[g_nParentIndex];
		g_nLeft1 = pMenu->nLeft1;
		g_nLeft2 = g_nLeft1+50;
		g_nLeft3 = g_nLeft2 + g_nLeft2 - g_nLeft1;
		g_nTop = pMenu->nTop;
		g_nInterval = pMenu->nInterval;
		g_menuThree.nMenuShows = pMenu->nMenuMaxCnt/3*3;
	}
	else
	{
		g_nLeft1 = g_guiMain.nLeft1;
		g_nLeft2 = g_nLeft1+50;
		g_nLeft3 = g_nLeft2 + g_nLeft2 - g_nLeft1;
		g_nTop = g_guiMain.nTop;
		g_nInterval = g_guiMain.nMenuInterval;
		g_menuThree.nMenuShows = g_guiMain.nMenuMaxCnt/3*3;
	}

    // 根据父菜单id生成子菜单项
	for(i = 0; i < g_nMenus; i ++)
	{
		pMenu = g_menuMain[i];
		ASSERT(pMenu != NULL);
		if(pMenu->nParent == g_nParentIndex)
		{
			g_nCurMenuIndex[g_nCurMenuCnt] = i;
			g_nCurMenuCnt ++;
		}
	}

    // 设置当前选择项
    g_nCurSelect = 0;
    if(g_nCurIndex != -1)
    {
        // ==-1表示从"确定"按键进入，所以当前选择项为第一项
        // !=-1表示从"取消"按键进入，需要判定子菜单的父菜单项为当前选择项
        for(i = 0; i < g_nCurMenuCnt; i ++)
        {
            if(g_nCurMenuIndex[i] == g_nCurIndex)
            {
                g_nCurSelect = i;
                break;
            }
        }
    }

    // 设置显示区域，因为可能菜单项个数比较多，不能全屏显示
    // 此处需要保证选中菜单处于显示区域中
	if(g_nCurSelect > g_menuThree.nMenuShows-1)
	{// 如果当前选择项已经大于能够显示菜单项的最大值
        // 那么最大显示项为当前选择项，依此判定出最小选择项
		g_menuThree.nMenuShowMax = g_nCurSelect;
        if(g_nCurSelect%3 == 0)
        {
            g_menuThree.nMenuShowMax ++;
        }
		g_menuThree.nMenuShowMin = g_menuThree.nMenuShowMax-g_menuThree.nMenuShows+1;
        if(g_menuThree.nMenuShowMax >= g_nCurMenuCnt)
        {
            g_menuThree.nMenuShowMax = g_nCurMenuCnt-1;
        }
	}
	else
	{// 否则最小显示项为0
		g_menuThree.nMenuShowMin = 0;
		g_menuThree.nMenuShowMax = (g_nCurMenuCnt>g_menuThree.nMenuShows)?g_menuThree.nMenuShows-1:g_nCurMenuCnt-1;
	}

	SetArrow();
}

void ThreeMenuDraw(HDC hdc)
{
	int i;
	int nIndex;
	MENU_INFO* pMenu;
	char czBuf[300];

	int nTop = g_nTop;
	int nLeft1 = g_nLeft1;
	int nLeft2 = g_nLeft2;
	int nLeft3 = g_nLeft3;
	int nLeft;
	
	BITMAP tempMap;
	//char bitmap[100] = {0};
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	char* pBitmap = NULL;
	char* showName = NULL;
	//char  finalShowName[20] = {0};
	RECT rc;
	
	SelectFont(hdc, g_guiComm.fontSmall);

	ASSERT(g_nCurMenuCnt > 0);
    ITEM_NOT_SELECT
    int bLeft = 0;
	//printf("nMenuShowMin = %d,nMenuShowMax = %d\n",g_menuThree.nMenuShowMin,g_menuThree.nMenuShowMax);
	for(i = g_menuThree.nMenuShowMin; i <= g_menuThree.nMenuShowMax; i ++)
	{
		nIndex = g_nCurMenuIndex[i];
		//printf("i= %d, nIndex = %d,nMenuShowMin = %d,nMenuShowMax= %d \n",i,nIndex,g_menuThree.nMenuShowMin,g_menuThree.nMenuShowMax);
		ASSERT(nIndex >= 0 && nIndex < g_nMenus);
		pMenu = g_menuMain[nIndex];
        ASSERT(pMenu != NULL);

        switch(bLeft)
		{
			case 0:
				nLeft = nLeft1;
				break;
			case 1:
				nLeft = nLeft2;
				break;
			case 2:
				nLeft = nLeft3;
				break;
			default:
				break;
		}
		++bLeft;
        bLeft = bLeft % 3;

		sprintf(czBuf, "%s", pMenu->czCaption);
		
		checkShowName(czBuf, &showName);
//		printf("[0007] %s---%s \n",czBuf,showName);

		x = 16 + ((i-g_menuThree.nMenuShowMin) % 3)*50;
		y = 25 + ((i-g_menuThree.nMenuShowMin) / 3)*54;
		width = 32;
		height = 32;
		//printf("x %d, y %d,width %d ,height %d \n",x,y,width,height);
		if(checkBitmap(czBuf, &pBitmap)>0)
		{
			LoadBitmapFromFile(hdc, &tempMap, pBitmap);
			FillBoxWithBitmap(hdc, x, y, width, height, &tempMap);
		}
		
		rc.left = nLeft;
		rc.right = rc.left + 48;
		rc.top = nTop;
		rc.bottom = rc.top + 12;
		
		if(g_nCurSelect == i)
		{
            // 设置反显操作
			ITEM_SELECT;
			//TextOut(hdc, nLeft, nTop, finalShowName);
			DrawText(hdc, showName, strlen(showName), &rc, DT_CENTER|DT_VCENTER);
            // 恢复反显，这样可以不需要总是设置颜色了
            ITEM_NOT_SELECT;
		}
		else
		{
			DrawText(hdc, showName, strlen(showName), &rc, DT_CENTER|DT_VCENTER);
		}
        if(bLeft == 0)
        {
		    nTop += (16+g_nInterval);
        }

        UnloadBitmap (&tempMap);
	}
}

BOOL ThreeMenuKeyMsg(int nKey)
{
	switch(nKey)
	{
	case KEY_UP:
		{
            // 菜单选择项上移
			if(g_nCurSelect > 2)
			{	
				g_nCurSelect-=3;
                // 调整菜单显示区域，如果菜单项不能全显的情况下，
                // 选择项已经小于最小显示菜单项，将显示区域上移
				if(g_nCurMenuCnt > g_menuThree.nMenuShows && g_menuThree.nMenuShowMin > g_nCurSelect)
				{
					
					g_menuThree.nMenuShowMin -= 3;
					g_menuThree.nMenuShowMax = g_menuThree.nMenuShowMin + g_menuThree.nMenuShows -1;
//					printf("line = %d: g_nCurSelect = %d,nMenuShowMin = %d,nMenuShowMax = %d,g_nCurMenuCnt = %d\n",__LINE__,
//						g_nCurSelect,g_menuThree.nMenuShowMin,g_menuThree.nMenuShowMax,g_nCurMenuCnt);
					if(g_menuThree.nMenuShowMax%3 == 0 && g_menuThree.nMenuShowMax < g_nCurMenuCnt-1)
					{
						g_menuThree.nMenuShowMax ++;
					}
				}
			}
			else
			{
				if(g_nCurSelect == 0)
				{
					g_nCurSelect = (g_nCurMenuCnt-1)/3*3;
				}
				else if(g_nCurSelect == 1)
				{
					g_nCurSelect = (g_nCurMenuCnt-1)/3*3+1;
					if(g_nCurSelect > g_nCurMenuCnt-1)
					{
						g_nCurSelect -= 3;
					}
				}
				else
				{
					g_nCurSelect = (g_nCurMenuCnt-1)/3*3+2;
					if(g_nCurSelect > g_nCurMenuCnt-1)
					{
						g_nCurSelect -= 3;
					}
				}
				if(g_nCurMenuCnt>g_menuThree.nMenuShows)
				{// 如果总菜单项已经大于能够显示菜单项的最大值
					// 那么最大显示项为当前选择项，依此判定出最小选择项
                    if(g_nCurMenuCnt%g_menuThree.nMenuShows == 0)
					{
						g_menuThree.nMenuShowMin = g_nCurMenuCnt - g_menuThree.nMenuShows;
					}
                    else if(g_nCurMenuCnt%g_menuThree.nMenuShows < 3)
                    {
						g_menuThree.nMenuShowMin = g_nCurMenuCnt - g_nCurMenuCnt%g_menuThree.nMenuShows - 3;
					}
                    else
                    {
						g_menuThree.nMenuShowMin = g_nCurMenuCnt - g_nCurMenuCnt%g_menuThree.nMenuShows;
					}
                    
					
				}
				else
				{// 否则最小显示项为0
					g_menuThree.nMenuShowMin = 0;
				}

                g_menuThree.nMenuShowMax = g_nCurMenuCnt-1;
			}
			g_narrMenuIndex[g_nMenuNodes-1] = g_nCurSelect;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		break;
	case KEY_DOWN:
		{
            // 菜单选择项下移
			if(g_nCurSelect+3 < g_nCurMenuCnt)
			{
				g_nCurSelect += 3;
                // 调整菜单显示区域，如果菜单项不能全显的情况下，
                // 选择项已经大于最大显示菜单项，将显示区域下移
				if(g_nCurMenuCnt > g_menuThree.nMenuShows && g_nCurSelect > g_menuThree.nMenuShowMax)
				{
					g_menuThree.nMenuShowMin += 3;
					g_menuThree.nMenuShowMax += 3;
                    if(g_menuThree.nMenuShowMax >= g_nCurMenuCnt)
                    {
                        g_menuThree.nMenuShowMax = g_nCurMenuCnt-1;
                    }
				}
			}
			else
			{
			    if(g_nCurSelect < 3)
				{
					g_menuThree.nMenuShowMin = 0;
    				g_menuThree.nMenuShowMax = g_nCurMenuCnt-1;
				}
				else if (g_menuThree.nMenuShowMax == g_nCurMenuCnt-1)
				{
					g_nCurSelect = g_nCurSelect%3;

                    if(g_nCurMenuCnt>g_menuThree.nMenuShows)
    				{
    				    g_menuThree.nMenuShowMin -= 3;
                        g_menuThree.nMenuShowMax = g_menuThree.nMenuShowMin + g_menuThree.nMenuShows -1;
    				}
                    
				}
                else
                {
                    g_menuThree.nMenuShowMin += 3;
                    g_menuThree.nMenuShowMax = g_nCurMenuCnt-1;
                }
                
			}
			g_narrMenuIndex[g_nMenuNodes-1] = g_nCurSelect;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		break;
    case KEY_LEFT:
		{
            // 菜单选择项上移
			if(g_nCurSelect > 0)
            {
                g_nCurSelect --;
				if(g_nCurSelect % 3 == 2)
				{
					if(g_nCurMenuCnt > g_menuThree.nMenuShows && g_menuThree.nMenuShowMin > g_nCurSelect)
					{
						g_menuThree.nMenuShowMin -= 3;
						g_menuThree.nMenuShowMax = g_menuThree.nMenuShowMin + g_menuThree.nMenuShows -1 ;
					}
            	}
			}
			else
			{
				g_nCurSelect = g_nCurMenuCnt-1;
				if(g_nCurSelect > g_menuThree.nMenuShows-1)
				{// 如果当前选择项已经大于能够显示菜单项的最大值
					// 那么最大显示项为当前选择项，依此判定出最小选择项
					g_menuThree.nMenuShowMax = g_nCurSelect;
					if(g_nCurSelect%3 == 0)
					{
						g_menuThree.nMenuShowMax +=2;
					}
					if(g_nCurSelect%3 == 1)
					{
						g_menuThree.nMenuShowMax ++;
					}
					g_menuThree.nMenuShowMin = g_menuThree.nMenuShowMax-g_menuThree.nMenuShows+1;
					if(g_menuThree.nMenuShowMax >= g_nCurMenuCnt)
					{
						g_menuThree.nMenuShowMax = g_nCurMenuCnt-1;
					}
				}
				else
				{// 否则最小显示项为0
					g_menuThree.nMenuShowMin = 0;
					g_menuThree.nMenuShowMax = (g_nCurMenuCnt>g_menuThree.nMenuShows)?g_menuThree.nMenuShows-1:g_nCurMenuCnt-1;
				}
			}
			g_narrMenuIndex[g_nMenuNodes-1] = g_nCurSelect;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		break;
    case KEY_RIGHT:
		{
            // 菜单选择项上移
            if(g_nCurSelect+1 < g_nCurMenuCnt)
            {
                g_nCurSelect ++;
				if(g_nCurSelect % 3 == 0)
				{
					if(g_nCurMenuCnt > g_menuThree.nMenuShows && g_nCurSelect > g_menuThree.nMenuShowMax)
					{
						g_menuThree.nMenuShowMin += 3;
						g_menuThree.nMenuShowMax += 3;
						if(g_menuThree.nMenuShowMax >= g_nCurMenuCnt)
						{
							g_menuThree.nMenuShowMax = g_nCurMenuCnt-1;
						}
					}
				}
            }
			else
			{
				g_nCurSelect = 0;
				g_menuThree.nMenuShowMin = 0;
				g_menuThree.nMenuShowMax = (g_nCurMenuCnt>g_menuThree.nMenuShows)?g_menuThree.nMenuShows-1:g_nCurMenuCnt-1;
			}
			g_narrMenuIndex[g_nMenuNodes-1] = g_nCurSelect;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		break;
	case KEY_OK:
		{
            //MenuOkKeyMsg();
            printf("now the ok key pressed!\n");
            MenuOkKeyMsg_Desk();
		}
		break;
	case KEY_CANCEL:
		{
			MenuCancelKeyMsg();
		}
		break;
	}
	return TRUE;
}

/************************************************************************
*************************************************************************/
void TwoMenuDraw(HDC hdc);
BOOL TwoMenuKeyMsg(int nKey);
void TwoInitMenu(DWORD dwPluginId);
PLUGIN_INFO g_menu2Layer = {
	0x00A10000/*id*/, 
	TwoInitMenu/*InitLayer*/,
	NULL/*UninitLayer*/,
	TwoMenuDraw/*Draw*/,
	NULL/*Timer*/,
	TwoMenuKeyMsg/*keymsg*/,
	NULL/*ipc*/
};
MENU_BASE_INFO g_menuTwo = {16, 0, 0};
/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    初始化菜单，动态创建当前菜单数组
	输入参数：    DWORD dwPluginId：菜单页面库ID
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
void TwoInitMenu(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;
	g_guiComm.nArrow = -1;
	g_guiComm.nCurPoint = 0;
	int i;
	MENU_INFO* pMenu;
	g_nCurMenuCnt = 0;

	if(g_nParentIndex != -1)
	{
		pMenu = g_menuMain[g_nParentIndex];
		g_nLeft1 = pMenu->nLeft1;
		g_nLeft2 = pMenu->nLeft2;
		g_nTop = pMenu->nTop;
		g_nInterval = pMenu->nInterval;
		g_menuTwo.nMenuShows = pMenu->nMenuMaxCnt/2*2;
	}
	else
	{
		g_nLeft1 = g_guiMain.nLeft1;
		g_nLeft2 = g_guiMain.nLeft2;
		g_nTop = g_guiMain.nTop;
		g_nInterval = g_guiMain.nMenuInterval;
		g_menuTwo.nMenuShows = g_guiMain.nMenuMaxCnt/2*2;
	}

    // 根据父菜单id生成子菜单项
	for(i = 0; i < g_nMenus; i ++)
	{
		pMenu = g_menuMain[i];
		ASSERT(pMenu != NULL);
		if(pMenu->nParent == g_nParentIndex)
		{
			g_nCurMenuIndex[g_nCurMenuCnt] = i;
			g_nCurMenuCnt ++;
		}
	}

    // 设置当前选择项
    g_nCurSelect = 0;
    if(g_nCurIndex != -1)
    {
        // ==-1表示从"确定"按键进入，所以当前选择项为第一项
        // !=-1表示从"取消"按键进入，需要判定子菜单的父菜单项为当前选择项
        for(i = 0; i < g_nCurMenuCnt; i ++)
        {
            if(g_nCurMenuIndex[i] == g_nCurIndex)
            {
                g_nCurSelect = i;
                break;
            }
        }
    }

    // 设置显示区域，因为可能菜单项个数比较多，不能全屏显示
    // 此处需要保证选中菜单处于显示区域中
	if(g_nCurSelect > g_menuTwo.nMenuShows-1)
	{// 如果当前选择项已经大于能够显示菜单项的最大值
        // 那么最大显示项为当前选择项，依此判定出最小选择项
		g_menuTwo.nMenuShowMax = g_nCurSelect;
        if(g_nCurSelect%2 == 0)
        {
            g_menuTwo.nMenuShowMax ++;
        }
		g_menuTwo.nMenuShowMin = g_menuTwo.nMenuShowMax-g_menuTwo.nMenuShows+1;
        if(g_menuTwo.nMenuShowMax >= g_nCurMenuCnt)
        {
            g_menuTwo.nMenuShowMax = g_nCurMenuCnt-1;
        }
	}
	else
	{// 否则最小显示项为0
		g_menuTwo.nMenuShowMin = 0;
		g_menuTwo.nMenuShowMax = (g_nCurMenuCnt>g_menuTwo.nMenuShows)?g_menuTwo.nMenuShows-1:g_nCurMenuCnt-1;
	}

	SetArrow();
}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    绘制动态菜单
	输入参数：    HDC hdc：绘制设备上下文句柄
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
void TwoMenuDraw(HDC hdc)
{
	int i;
	int nIndex;
	MENU_INFO* pMenu;
	char czBuf[300];

	int nTop = g_nTop;
	int nLeft1 = g_nLeft1;
	int nLeft2 = g_nLeft2;
	int nLeft;
	SelectFont(hdc, g_guiComm.fontBig);

	ASSERT(g_nCurMenuCnt > 0);
    ITEM_NOT_SELECT
    BOOL bLeft = TRUE;

	for(i = g_menuTwo.nMenuShowMin; i <= g_menuTwo.nMenuShowMax; i ++)
	{
		nIndex = g_nCurMenuIndex[i];
		ASSERT(nIndex >= 0 && nIndex < g_nMenus);
		pMenu = g_menuMain[nIndex];
        ASSERT(pMenu != NULL);

        nLeft = (bLeft==TRUE)?nLeft1:nLeft2;
        bLeft = !bLeft;
		sprintf(czBuf, "%d.%s", i+1, pMenu->czCaption);
		if(g_nCurSelect == i)
		{
            // 设置反显操作
			ITEM_SELECT;
			TextOut(hdc, nLeft, nTop, czBuf);
            // 恢复反显，这样可以不需要总是设置颜色了
            ITEM_NOT_SELECT;
		}
		else
		{
			TextOut(hdc, nLeft, nTop, czBuf);
		}
        if(bLeft == TRUE)
        {
		    nTop += (16+g_nInterval);
        }
	}
}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    处理菜单按键消息
	输入参数：    int nKey：按键消息
	输出参数：
	函数返回值：  BOOL：如果处理返回TRUE,使用缺省处理返回FALSE
	内部处理流程：
********************************************************************/
BOOL TwoMenuKeyMsg(int nKey)
{
	switch(nKey)
	{
	case KEY_UP:
		{
            // 菜单选择项上移
			if(g_nCurSelect > 1)
			{
				g_nCurSelect-=2;
                // 调整菜单显示区域，如果菜单项不能全显的情况下，
                // 选择项已经小于最小显示菜单项，将显示区域上移
				if(g_nCurMenuCnt > g_menuTwo.nMenuShows && g_menuTwo.nMenuShowMin > g_nCurSelect)
				{
					g_menuTwo.nMenuShowMin -= 2;
					g_menuTwo.nMenuShowMax -= 2;
					if(g_menuTwo.nMenuShowMax%2 == 0 && g_menuTwo.nMenuShowMax < g_nCurMenuCnt-1)
					{
						g_menuTwo.nMenuShowMax ++;
					}
				}
			}
			else
			{
				if(g_nCurSelect == 0)
				{
					g_nCurSelect = (g_nCurMenuCnt-1)/2*2;
				}
				else
				{
					g_nCurSelect = (g_nCurMenuCnt-1)/2*2+1;
					if(g_nCurSelect > g_nCurMenuCnt-1)
					{
						g_nCurSelect -= 2;
					}
				}
				if(g_nCurSelect > g_menuTwo.nMenuShows-1)
				{// 如果当前选择项已经大于能够显示菜单项的最大值
					// 那么最大显示项为当前选择项，依此判定出最小选择项
					g_menuTwo.nMenuShowMax = g_nCurSelect;
					if(g_nCurSelect%2 == 0)
					{
						g_menuTwo.nMenuShowMax ++;
					}
					g_menuTwo.nMenuShowMin = g_menuTwo.nMenuShowMax-g_menuTwo.nMenuShows+1;
					if(g_menuTwo.nMenuShowMax >= g_nCurMenuCnt)
					{
						g_menuTwo.nMenuShowMax = g_nCurMenuCnt-1;
					}
				}
				else
				{// 否则最小显示项为0
					g_menuTwo.nMenuShowMin = 0;
					g_menuTwo.nMenuShowMax = (g_nCurMenuCnt>g_menuTwo.nMenuShows)?g_menuTwo.nMenuShows-1:g_nCurMenuCnt-1;
				}
			}
			g_narrMenuIndex[g_nMenuNodes-1] = g_nCurSelect;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		break;
	case KEY_DOWN:
		{
            // 菜单选择项下移
			if(g_nCurSelect+2 < g_nCurMenuCnt)
			{
				g_nCurSelect += 2;
                // 调整菜单显示区域，如果菜单项不能全显的情况下，
                // 选择项已经大于最大显示菜单项，将显示区域下移
				if(g_nCurMenuCnt > g_menuTwo.nMenuShows && g_nCurSelect > g_menuTwo.nMenuShowMax)
				{
					g_menuTwo.nMenuShowMin += 2;
					g_menuTwo.nMenuShowMax += 2;
                    if(g_menuTwo.nMenuShowMax >= g_nCurMenuCnt)
                    {
                        g_menuTwo.nMenuShowMax = g_nCurMenuCnt-1;
                    }
				}
			}
			else
			{
				if((g_nCurSelect%2) == 0)
				{
					g_nCurSelect = 0;
				}
				else
				{
					g_nCurSelect = 1;
				}
				g_menuTwo.nMenuShowMin = 0;
				g_menuTwo.nMenuShowMax = (g_nCurMenuCnt>g_menuTwo.nMenuShows)?g_menuTwo.nMenuShows-1:g_nCurMenuCnt-1;
			}
			g_narrMenuIndex[g_nMenuNodes-1] = g_nCurSelect;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		break;
    case KEY_LEFT:
		{
            // 菜单选择项上移
			if(g_nCurSelect > 0)
            {
                g_nCurSelect --;
				if(g_nCurSelect % 2 == 1)
				{
					if(g_nCurMenuCnt > g_menuTwo.nMenuShows && g_menuTwo.nMenuShowMin > g_nCurSelect)
					{
						g_menuTwo.nMenuShowMin -= 2;
						g_menuTwo.nMenuShowMax -= 2;
					}
            	}
			}
			else
			{
				g_nCurSelect = g_nCurMenuCnt-1;
				if(g_nCurSelect > g_menuTwo.nMenuShows-1)
				{// 如果当前选择项已经大于能够显示菜单项的最大值
					// 那么最大显示项为当前选择项，依此判定出最小选择项
					g_menuTwo.nMenuShowMax = g_nCurSelect;
					if(g_nCurSelect%2 == 0)
					{
						g_menuTwo.nMenuShowMax ++;
					}
					g_menuTwo.nMenuShowMin = g_menuTwo.nMenuShowMax-g_menuTwo.nMenuShows+1;
					if(g_menuTwo.nMenuShowMax >= g_nCurMenuCnt)
					{
						g_menuTwo.nMenuShowMax = g_nCurMenuCnt-1;
					}
				}
				else
				{// 否则最小显示项为0
					g_menuTwo.nMenuShowMin = 0;
					g_menuTwo.nMenuShowMax = (g_nCurMenuCnt>g_menuTwo.nMenuShows)?g_menuTwo.nMenuShows-1:g_nCurMenuCnt-1;
				}
			}
			g_narrMenuIndex[g_nMenuNodes-1] = g_nCurSelect;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		break;
    case KEY_RIGHT:
		{
            // 菜单选择项上移
            if(g_nCurSelect+1 < g_nCurMenuCnt)
            {
                g_nCurSelect ++;
				if(g_nCurSelect % 2 == 0)
				{
					if(g_nCurMenuCnt > g_menuTwo.nMenuShows && g_nCurSelect > g_menuTwo.nMenuShowMax)
					{
						g_menuTwo.nMenuShowMin += 2;
						g_menuTwo.nMenuShowMax += 2;
						if(g_menuTwo.nMenuShowMax >= g_nCurMenuCnt)
						{
							g_menuTwo.nMenuShowMax = g_nCurMenuCnt-1;
						}
					}
				}
            }
			else
			{
				g_nCurSelect = 0;
				g_menuTwo.nMenuShowMin = 0;
				g_menuTwo.nMenuShowMax = (g_nCurMenuCnt>g_menuTwo.nMenuShows)?g_menuTwo.nMenuShows-1:g_nCurMenuCnt-1;
			}
			g_narrMenuIndex[g_nMenuNodes-1] = g_nCurSelect;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		break;
	case KEY_OK:
		{
            MenuOkKeyMsg();
		}
		break;
	case KEY_CANCEL:
		{
			MenuCancelKeyMsg();
		}
		break;
	}
	return TRUE;
}




