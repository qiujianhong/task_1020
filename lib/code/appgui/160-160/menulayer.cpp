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
int g_nMenus = 0;
int g_nParentIndex = -1;
int g_nCurIndex = -1;   // 只用于"取消"返回上一级菜单时判定选择项使用，确定是设置为-1

int g_nCurMenuIndex[1000];
int g_nCurMenuCnt = 0;
int g_nCurSelect = 0;
int g_nMaxMenuChar = 0;

int g_narrMenuIndex[20] = { 0 };
int g_nMenuNodes = 1;
char g_czLayerCap[30];

int g_nLeft1 = 10;
int g_nLeft2 = 90;
int g_nTop = 32;
int g_nInterval = 0;

#if MODULARIZATION_TMN_EN == 1
int g_ncancelkey = 0;

const char* g_icon_cancel[] =
{
	"offcancelkey.bmp",
	"oncancelkey.bmp",
};

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
	if (g_menu_icon.bCancel[1] == 0)
	{
		sprintf(czBuf, "%s%s", ICON_PATH, g_icon_cancel[1]);
		LoadBitmapFromFile(HDC_SCREEN, &g_menu_icon.mapCancel[1], czBuf);
		g_menu_icon.bCancel[1] = 1;
	}
	//  FillBoxWithBitmap(hdc, x, y, w, h, &g_menu_icon.mapCancel[1]);
	g_guiComm.bOKiscancel = 1;
}
#endif

void CancelKeyDrawsub(HDC hdc, int x, int y, int w, int h, int i)
{
	char czBuf[100];

	if (i == 0)
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
MENU_BASE_INFO g_menuBase = { 8, 0, 0 };

BOOL IsMenuLayer(DWORD dwID)
{
	return (dwID == 0x00A00000 || dwID == 0x00A10000 || dwID == 0x00A20000 || dwID == 0x00A80000) ? TRUE : FALSE;
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
	n = 0;
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
	{
		ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "时间设置");

	y += 16;
	ITEM_NOT_SELECT;
	if (n == 1)
	{
		ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "行政区码和终端地址的设置");

	y += 16;
	ITEM_NOT_SELECT;
	if (n == 2)
	{
		ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "通讯通道配置");

	y += 16;
	ITEM_NOT_SELECT;
	if (n == 3)
	{
		ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "测量点参数配置");

	y += 16;
	ITEM_NOT_SELECT;
	if (n == 4)
	{
		ITEM_SELECT;
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

	if (ValidatePwd() != TRUE)
	{
		return;
	}
	if (g_bAllMenu == TRUE)
	{
		if (TRUE != MessageBox("提示\n\n是否进入设置模式？", MB_OKCANCEL))
		{
			return;
		}
	}
	else
	{
		if (TRUE != MessageBox("提示\n\n是否进入设置模式？", MB_OKCANCEL))
		{
			return;
		}
	}
	PopLayer(0x11100000);


	//UpdateWindow(g_guiComm.hMainWnd, TRUE);

}
#endif

//app退出时，初始化页面
void init_layer()
{
    ReadConfig(FALSE);
    if (g_guiMain.pluginCur->pfnUninitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnUninitLayer();
	}
	// 根据主菜单风格ID，确定当前的plug
	g_guiMain.pluginCur = GetPlugIn(g_guiMain.nMenuRootId);
	ASSERT(g_guiMain.pluginCur != NULL);

	g_nParentIndex = -1;
	g_nCurIndex = -1;
	g_nCurMenuCnt = 0;
	g_nCurSelect = 0;
	g_nMaxMenuChar = 0;
    if (g_guiMain.pluginCur->pfnInitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
	}
	UpdateWindow(g_guiComm.hMainWnd, TRUE);
}

void ShowOrHideMenu()
{
	ClosePopLayer();
	StopTrunDisp();

	if (ValidatePwd() != TRUE)
	{
		return;
	}
	if (g_bAllMenu == TRUE)
	{
		if (TRUE != MessageBox("提示\n\n是否关闭扩展菜单项？", MB_OKCANCEL))
		{
			return;
		}
	}
	else
	{
		if (TRUE != MessageBox("提示\n\n是否显示扩展菜单项？", MB_OKCANCEL))
		{
			return;
		}
	}
	g_bAllMenu = !g_bAllMenu;
	ReadConfig(g_bAllMenu);

	ASSERT(g_guiMain.pluginCur != NULL);
	if (g_guiMain.pluginCur->pfnUninitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnUninitLayer();
	}
	// 根据主菜单风格ID，确定当前的plug
	g_guiMain.pluginCur = GetPlugIn(g_guiMain.nMenuRootId);
	ASSERT(g_guiMain.pluginCur != NULL);

	g_nParentIndex = -1;
	g_nCurIndex = -1;
	g_nCurMenuCnt = 0;
	g_nCurSelect = 0;
	g_nMaxMenuChar = 0;

	// 初始化当前页面库，在Uninitialize中释放
	ASSERT(g_guiMain.pluginCur != NULL);
	g_guiComm.bCanTrunDisp = TRUE;
	if (g_guiMain.pluginCur->pfnInitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
	}
	UpdateWindow(g_guiComm.hMainWnd, TRUE);
}

void SetArrow()
{
	if (g_menuBase.nMenuShowMin > 0
		&& g_menuBase.nMenuShowMax < g_nCurMenuCnt - 1)
	{// 显示上下箭头
		g_guiComm.nArrow = 0;
	}
	else if (g_menuBase.nMenuShowMin > 0)
	{// 显示上箭头
		g_guiComm.nArrow = 1;
	}
	else if (g_menuBase.nMenuShowMax < g_nCurMenuCnt - 1)
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
	int i;
	int nIndex;
	MENU_INFO* pMenu;

	char czBuf[300];

	SelectFont(hdc, g_guiComm.fontBig);

	//int nLeft = (g_guiComm.nWidth-g_nMaxMenuChar*g_menuBase.nMenuShows)/2;
	int nLeft = g_nLeft1;
	int Top = g_nTop - 10;

	ASSERT(g_nCurMenuCnt > 0);
	ITEM_NOT_SELECT
		RECT rc;
	rc.left = 0;
	rc.right = g_guiComm.nWidth;
#if SHANGHAI_FUN == 1 && HARDWARE_TYPE == 3
	if (g_nParentIndex == -1)
	{
		sprintf(czBuf, "%s", "主菜单");
		rc.top = Top;
		rc.bottom = Top + 16;
		DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER);
		Top += 16;
	}
#endif
	//返回箭头不显示

	for (i = g_menuBase.nMenuShowMin; i <= g_menuBase.nMenuShowMax; i++)
	{
		nIndex = g_nCurMenuIndex[i];
		ASSERT(nIndex >= 0 && nIndex < g_nMenus);
		pMenu = g_menuMain[nIndex];
		ASSERT(pMenu != NULL);

		rc.top = Top;
		rc.bottom = Top + 16;
		sprintf(czBuf, "%d.%s", i + 1, pMenu->czCaption);
		if(strlen(pMenu->czCaption) > 18)
		{
			SelectFont(hdc, g_guiComm.fontSmall);
		}
		else
		{
			SelectFont(hdc, g_guiComm.fontBig);
		}

#if HARDWARE_TYPE == 4 || (HARDWARE_TYPE == 3 && HUNAN_FUN == 1)        // 375.2规范要求集中器的主菜单不带序号
		sprintf(czBuf, "%s", pMenu->czCaption);
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
			TextOut(hdc, nLeft, rc.top, czBuf);
		}
		else if (HARDWARE_TYPE == 4) // 375.2规范要求集中器的主菜单不带序号
		{
			DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER);
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
	ASSERT(pMenu != NULL && pMenu->nPlugCnt > 0);

	// 获取菜单项对应的页面：子菜单或者第一个功能页面
	if (pMenu->bMenuAggregate == TRUE)
	{
		pPlugin = GetPlugIn(pMenu->dwMenuID);
	}
	else
	{
		pPlugin = GetPlugIn(pMenu->pPlugStruct[0].dwPlugin);
	}
	if (pPlugin == NULL)
	{
		ASSERT(FALSE);
		return;
	}

	// 对旧的当前页面进行释放操作
	ASSERT(g_guiMain.pluginCur != NULL);
	if (g_guiMain.pluginCur->pfnUninitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnUninitLayer();
	}

	g_guiMain.pluginCur = pPlugin;
	// 设置父菜单项信息，这是为了保证"取消"回退时，能够找到父菜单项
	// 这只对包含子菜单时起作用
	if (pMenu->bMenuAggregate == TRUE)
	{
		g_nParentIndex = nIndex;
	}

	g_narrMenuIndex[g_nMenuNodes] = 0;
	g_nMenuNodes++;

	g_guiMain.pPrentMenu = pMenu;
	g_nCurIndex = nIndex;
	g_guiMain.nPageLayerIndex = 0;

	// 对新当前页面初始化操作
	g_guiComm.bKeyDown = TRUE;// 进入第一页时认为向下按键
	g_guiComm.bCanTrunDisp = TRUE;
	g_guiComm.bSkipLayer = FALSE;
	g_guiComm.bPointOffset = FALSE;
	g_guiComm.nArrow = -1;
	if (g_guiMain.pluginCur->pfnInitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
	}
	if (g_guiComm.bSkipLayer == TRUE)
	{
		KeepNextLayer();
	}
	SetSubLayerArrow();
	if (g_guiMain.pPrentMenu != NULL && g_guiMain.pPrentMenu->bMenuAggregate == TRUE)
	{
		SetArrow();
	}
	UpdateWindow(g_guiComm.hMainWnd, TRUE);
}
extern void StrartTrunDisp();
void MenuCancelKeyMsg()
{
	MENU_INFO* pMenu;
	PLUGIN_INFO* pPlugin;
	if (g_nParentIndex == -1)
	{// 已经是主菜单了，不能在向上翻页了
		//StrartTrunDisp();
		appGUI_req_exit();
		return;
	}

	ASSERT(g_nParentIndex >= 0 && g_nParentIndex < g_nMenus);
	// 获取父菜单指针
	pMenu = g_menuMain[g_nParentIndex];
	ASSERT(pMenu != NULL && pMenu->nPlugCnt > 0);
	// 获取父菜单对应的菜单页面库指针
	if (pMenu->nParent == -1)
	{
		pPlugin = GetPlugIn(g_guiMain.nMenuRootId);
	}
	else
	{
		pPlugin = GetPlugIn(g_menuMain[pMenu->nParent]->dwMenuID);
	}
	//pPlugin = GetPlugIn(pMenu->pdwPlugin[0]);
	if (pPlugin == NULL)
	{
		ASSERT(FALSE);
		return;
	}
	g_nMenuNodes--;
	ASSERT(g_nMenuNodes > 0);

	// 对旧的当前页面进行释放操作
	ASSERT(g_guiMain.pluginCur != NULL);
	if (g_guiMain.pluginCur->pfnUninitLayer != NULL)
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
	if (g_guiMain.pluginCur->pfnInitLayer != NULL)
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
MENU_BASE_INFO g_menuTwo = { 16, 0, 0 };
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

	if (g_nParentIndex != -1)
	{
		pMenu = g_menuMain[g_nParentIndex];
		g_nLeft1 = pMenu->nLeft1;
		g_nLeft2 = pMenu->nLeft2;
		g_nTop = pMenu->nTop;
		g_nInterval = pMenu->nInterval;
		g_menuTwo.nMenuShows = pMenu->nMenuMaxCnt / 2 * 2;
	}
	else
	{
		g_nLeft1 = g_guiMain.nLeft1;
		g_nLeft2 = g_guiMain.nLeft2;
		g_nTop = g_guiMain.nTop;
		g_nInterval = g_guiMain.nMenuInterval;
		g_menuTwo.nMenuShows = g_guiMain.nMenuMaxCnt / 2 * 2;
	}

	// 根据父菜单id生成子菜单项
	for (i = 0; i < g_nMenus; i++)
	{
		pMenu = g_menuMain[i];
		ASSERT(pMenu != NULL);
		if (pMenu->nParent == g_nParentIndex)
		{
			g_nCurMenuIndex[g_nCurMenuCnt] = i;
			g_nCurMenuCnt++;
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
	if (g_nCurSelect > g_menuTwo.nMenuShows - 1)
	{// 如果当前选择项已经大于能够显示菜单项的最大值
		// 那么最大显示项为当前选择项，依此判定出最小选择项
		g_menuTwo.nMenuShowMax = g_nCurSelect;
		if (g_nCurSelect % 2 == 0)
		{
			g_menuTwo.nMenuShowMax++;
		}
		g_menuTwo.nMenuShowMin = g_menuTwo.nMenuShowMax - g_menuTwo.nMenuShows + 1;
		if (g_menuTwo.nMenuShowMax >= g_nCurMenuCnt)
		{
			g_menuTwo.nMenuShowMax = g_nCurMenuCnt - 1;
		}
	}
	else
	{// 否则最小显示项为0
		g_menuTwo.nMenuShowMin = 0;
		g_menuTwo.nMenuShowMax = (g_nCurMenuCnt > g_menuTwo.nMenuShows) ? g_menuTwo.nMenuShows - 1 : g_nCurMenuCnt - 1;
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

	for (i = g_menuTwo.nMenuShowMin; i <= g_menuTwo.nMenuShowMax; i++)
	{
		nIndex = g_nCurMenuIndex[i];
		ASSERT(nIndex >= 0 && nIndex < g_nMenus);
		pMenu = g_menuMain[nIndex];
		ASSERT(pMenu != NULL);

		nLeft = (bLeft == TRUE) ? nLeft1 : nLeft2;
		bLeft = !bLeft;
		sprintf(czBuf, "%d.%s", i + 1, pMenu->czCaption);
		if (g_nCurSelect == i)
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
		if (bLeft == TRUE)
		{
			nTop += (16 + g_nInterval);
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
	switch (nKey)
	{
	case KEY_UP:
	{
		// 菜单选择项上移
		if (g_nCurSelect > 1)
		{
			g_nCurSelect -= 2;
			// 调整菜单显示区域，如果菜单项不能全显的情况下，
			// 选择项已经小于最小显示菜单项，将显示区域上移
			if (g_nCurMenuCnt > g_menuTwo.nMenuShows && g_menuTwo.nMenuShowMin > g_nCurSelect)
			{
				g_menuTwo.nMenuShowMin -= 2;
				g_menuTwo.nMenuShowMax -= 2;
				if (g_menuTwo.nMenuShowMax % 2 == 0 && g_menuTwo.nMenuShowMax < g_nCurMenuCnt - 1)
				{
					g_menuTwo.nMenuShowMax++;
				}
			}
		}
		else
		{
			if (g_nCurSelect == 0)
			{
				g_nCurSelect = (g_nCurMenuCnt - 1) / 2 * 2;
			}
			else
			{
				g_nCurSelect = (g_nCurMenuCnt - 1) / 2 * 2 + 1;
				if (g_nCurSelect > g_nCurMenuCnt - 1)
				{
					g_nCurSelect -= 2;
				}
			}
			if (g_nCurSelect > g_menuTwo.nMenuShows - 1)
			{// 如果当前选择项已经大于能够显示菜单项的最大值
				// 那么最大显示项为当前选择项，依此判定出最小选择项
				g_menuTwo.nMenuShowMax = g_nCurSelect;
				if (g_nCurSelect % 2 == 0)
				{
					g_menuTwo.nMenuShowMax++;
				}
				g_menuTwo.nMenuShowMin = g_menuTwo.nMenuShowMax - g_menuTwo.nMenuShows + 1;
				if (g_menuTwo.nMenuShowMax >= g_nCurMenuCnt)
				{
					g_menuTwo.nMenuShowMax = g_nCurMenuCnt - 1;
				}
			}
			else
			{// 否则最小显示项为0
				g_menuTwo.nMenuShowMin = 0;
				g_menuTwo.nMenuShowMax = (g_nCurMenuCnt > g_menuTwo.nMenuShows) ? g_menuTwo.nMenuShows - 1 : g_nCurMenuCnt - 1;
			}
		}
		g_narrMenuIndex[g_nMenuNodes - 1] = g_nCurSelect;
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
	}
	break;
	case KEY_DOWN:
	{
		// 菜单选择项下移
		if (g_nCurSelect + 2 < g_nCurMenuCnt)
		{
			g_nCurSelect += 2;
			// 调整菜单显示区域，如果菜单项不能全显的情况下，
			// 选择项已经大于最大显示菜单项，将显示区域下移
			if (g_nCurMenuCnt > g_menuTwo.nMenuShows && g_nCurSelect > g_menuTwo.nMenuShowMax)
			{
				g_menuTwo.nMenuShowMin += 2;
				g_menuTwo.nMenuShowMax += 2;
				if (g_menuTwo.nMenuShowMax >= g_nCurMenuCnt)
				{
					g_menuTwo.nMenuShowMax = g_nCurMenuCnt - 1;
				}
			}
		}
		else
		{
			if ((g_nCurSelect % 2) == 0)
			{
				g_nCurSelect = 0;
			}
			else
			{
				g_nCurSelect = 1;
			}
			g_menuTwo.nMenuShowMin = 0;
			g_menuTwo.nMenuShowMax = (g_nCurMenuCnt > g_menuTwo.nMenuShows) ? g_menuTwo.nMenuShows - 1 : g_nCurMenuCnt - 1;
		}
		g_narrMenuIndex[g_nMenuNodes - 1] = g_nCurSelect;
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
	}
	break;
	case KEY_LEFT:
	{
		// 菜单选择项上移
		if (g_nCurSelect > 0)
		{
			g_nCurSelect--;
			if (g_nCurSelect % 2 == 1)
			{
				if (g_nCurMenuCnt > g_menuTwo.nMenuShows && g_menuTwo.nMenuShowMin > g_nCurSelect)
				{
					g_menuTwo.nMenuShowMin -= 2;
					g_menuTwo.nMenuShowMax -= 2;
				}
			}
		}
		else
		{
			g_nCurSelect = g_nCurMenuCnt - 1;
			if (g_nCurSelect > g_menuTwo.nMenuShows - 1)
			{// 如果当前选择项已经大于能够显示菜单项的最大值
				// 那么最大显示项为当前选择项，依此判定出最小选择项
				g_menuTwo.nMenuShowMax = g_nCurSelect;
				if (g_nCurSelect % 2 == 0)
				{
					g_menuTwo.nMenuShowMax++;
				}
				g_menuTwo.nMenuShowMin = g_menuTwo.nMenuShowMax - g_menuTwo.nMenuShows + 1;
				if (g_menuTwo.nMenuShowMax >= g_nCurMenuCnt)
				{
					g_menuTwo.nMenuShowMax = g_nCurMenuCnt - 1;
				}
			}
			else
			{// 否则最小显示项为0
				g_menuTwo.nMenuShowMin = 0;
				g_menuTwo.nMenuShowMax = (g_nCurMenuCnt > g_menuTwo.nMenuShows) ? g_menuTwo.nMenuShows - 1 : g_nCurMenuCnt - 1;
			}
		}
		g_narrMenuIndex[g_nMenuNodes - 1] = g_nCurSelect;
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
	}
	break;
	case KEY_RIGHT:
	{
		// 菜单选择项上移
		if (g_nCurSelect + 1 < g_nCurMenuCnt)
		{
			g_nCurSelect++;
			if (g_nCurSelect % 2 == 0)
			{
				if (g_nCurMenuCnt > g_menuTwo.nMenuShows && g_nCurSelect > g_menuTwo.nMenuShowMax)
				{
					g_menuTwo.nMenuShowMin += 2;
					g_menuTwo.nMenuShowMax += 2;
					if (g_menuTwo.nMenuShowMax >= g_nCurMenuCnt)
					{
						g_menuTwo.nMenuShowMax = g_nCurMenuCnt - 1;
					}
				}
			}
		}
		else
		{
			g_nCurSelect = 0;
			g_menuTwo.nMenuShowMin = 0;
			g_menuTwo.nMenuShowMax = (g_nCurMenuCnt > g_menuTwo.nMenuShows) ? g_menuTwo.nMenuShows - 1 : g_nCurMenuCnt - 1;
		}
		g_narrMenuIndex[g_nMenuNodes - 1] = g_nCurSelect;
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


#if HARDWARE_2012_EN==1
/************************************************************************/
/* 终端时间设置                                                         */
/************************************************************************/
typedef struct _RESET_TIME
{
	int     nIndex;     // 当前索引
	char    czInfo[16]; // 当前时间字符串，包含:

	struct tm   tmNow;

}SETRESET_TIME;
SETRESET_TIME g_set_time;

void Set_ParaResetTimeInit(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;
	g_set_time.nIndex = 0;

	// 读取当前时间
//#ifndef ARM_LINUX
	long lTime = time(NULL);
	tm* tnow = localtime(&lTime);
	memcpy(&g_set_time.tmNow, tnow, sizeof(tm));

	/*#else
		rtc rtctime;
		read_rtc_inter(&rtctime);
		g_reset_time.tmNow.tm_year = rtctime.yy-1900;
		g_reset_time.tmNow.tm_mon = rtctime.mm-1;
		g_reset_time.tmNow.tm_mday = rtctime.dd;
		g_reset_time.tmNow.tm_hour = rtctime.hh;
		g_reset_time.tmNow.tm_min = rtctime.nn;
		g_reset_time.tmNow.tm_sec = rtctime.ss;
	#endif*/
	sprintf(g_set_time.czInfo, "%04d%02d%02d%02d:%02d:%02d",
		g_set_time.tmNow.tm_year + 1900,
		g_set_time.tmNow.tm_mon + 1,
		g_set_time.tmNow.tm_mday,
		g_set_time.tmNow.tm_hour,
		g_set_time.tmNow.tm_min,
		g_set_time.tmNow.tm_sec);
}
void Set_ParaResetTimeDraw(HDC hdc)
{
	SelectFont(hdc, g_guiComm.fontBig);
	ITEM_NOT_SELECT;

	RECT rc;
	rc.left = 0;
	rc.right = g_guiComm.nWidth;

	rc.top = 30;
	rc.bottom = rc.top + 16;
	char pcz[20];
	strcpy(pcz, "终端时间设置");
	DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

	// 绘制日期
	int nLeft = 24;
	int nTop = rc.top + 30;
	int i;
	for (i = 0; i < 8; i++)
	{
		switch (i)
		{
		case 4:TextOut(hdc, nLeft, nTop, "年"); nLeft += 16; break;
		case 6:TextOut(hdc, nLeft, nTop, "月"); nLeft += 16; break;
		}

		if (g_set_time.nIndex == i)
		{
			ITEM_SELECT;
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_set_time.czInfo[i], 1);
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_set_time.czInfo[i], 1);
		}
		nLeft += 8;
	}
	TextOut(hdc, nLeft, nTop, "日");

	// 绘制时间
	nLeft = 48;
	nTop += 20;
	for (; i < 16; i++)
	{
		if (g_set_time.nIndex == i)
		{
			ITEM_SELECT;
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_set_time.czInfo[i], 1);
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_set_time.czInfo[i], 1);
		}
		nLeft += 8;
	}

	// 确定
	nTop += 20;
	nLeft = 68;
	if (g_set_time.nIndex == i)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft, nTop, "确定");
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, "确定");
	}
}
BOOL Set_ParaResetTimeKeyMsg(int nKey)
{
	int iRet = 0;

	switch (nKey)
	{
	case KEY_LEFT:// 修改输入焦点
		g_set_time.nIndex--;
		if (g_set_time.czInfo[g_set_time.nIndex] < '0'
			|| g_set_time.czInfo[g_set_time.nIndex] > '9')
		{
			g_set_time.nIndex--;
		}
		if (g_set_time.nIndex < 0)
		{
			g_set_time.nIndex = 16;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	case KEY_RIGHT:// 修改输入焦点
		g_set_time.nIndex++;
		if ((g_set_time.czInfo[g_set_time.nIndex] < '0'
			|| g_set_time.czInfo[g_set_time.nIndex] > '9')
			&& g_set_time.nIndex != 16)
		{
			g_set_time.nIndex++;
		}
		if (g_set_time.nIndex > 16)
		{
			g_set_time.nIndex = 0;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	case KEY_UP:// 修改时间参数，其中对有效位做了一些判断
		if (g_set_time.nIndex >= 0 && g_set_time.nIndex <= 15)
		{
			char czMax = '9';
			switch (g_set_time.nIndex)
			{
			case 0:czMax = '2'; break;   // 年千位
			case 4:czMax = '1'; break;   // 月十位
			case 6:czMax = '3'; break;   // 日十位
			case 8:czMax = '2'; break;   // 小时十位
			case 11:czMax = '5'; break;  // 分十位
			case 14:czMax = '5'; break;  // 秒十位
			}
			g_set_time.czInfo[g_set_time.nIndex] ++;
			if (g_set_time.czInfo[g_set_time.nIndex] > czMax)
			{
				if (g_set_time.nIndex == 0)
					g_set_time.czInfo[g_set_time.nIndex] = '1';
				else
					g_set_time.czInfo[g_set_time.nIndex] = '0';
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
		break;
	case KEY_DOWN:// 修改时间参数，其中对有效位做了一些判断
		if (g_set_time.nIndex >= 0 && g_set_time.nIndex <= 15)
		{
			char czMax = '9';
			switch (g_set_time.nIndex)
			{
			case 0:czMax = '2'; break;   // 年千位
			case 4:czMax = '1'; break;   // 月十位
			case 6:czMax = '3'; break;   // 日十位
			case 8:czMax = '2'; break;   // 小时十位
			case 11:czMax = '5'; break;  // 分十位
			case 14:czMax = '5'; break;  // 秒十位
			}
			g_set_time.czInfo[g_set_time.nIndex] --;
			if (g_set_time.czInfo[g_set_time.nIndex] < '0')
			{
				g_set_time.czInfo[g_set_time.nIndex] = czMax;
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
		break;
	case KEY_OK:
		printf("ENTER KEY_OK\n");
		if (g_set_time.nIndex == 16 && ValidatePwd() == TRUE)
		{
			char czBuf[20];
			int nVal;
			// 年
			memcpy(czBuf, g_set_time.czInfo, 4);
			czBuf[4] = '\0';
			nVal = atoi(czBuf);
			if (nVal > 2099)
			{
				g_set_time.nIndex = 3;
				MessageBox("年份值不能超出2099年\n\n请重新输入！", MB_OK);
				return TRUE;
			}
			if (nVal < 2000)
			{
				g_set_time.nIndex = 3;
				MessageBox("年份值不能低于2000年\n\n请重新输入！", MB_OK);
				return TRUE;
			}
			g_set_time.tmNow.tm_year = nVal;

			// 月
			memcpy(czBuf, g_set_time.czInfo + 4, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			if (nVal == 0)
			{
				g_set_time.nIndex = 5;
				MessageBox("月份值不能为零月份\n\n请重新输入！", MB_OK);
				return TRUE;
			}
			if (nVal > 12)
			{
				g_set_time.nIndex = 5;
				MessageBox("月份值不能大于12月份\n\n请重新输入！", MB_OK);
				return TRUE;
			}
			g_set_time.tmNow.tm_mon = nVal;

			// 日
			memcpy(czBuf, g_set_time.czInfo + 6, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			if (nVal == 0)
			{
				g_set_time.nIndex = 7;
				MessageBox("日期值不能为零\n\n请重新输入！", MB_OK);
				return TRUE;
			}
			if (nVal > 31)
			{
				g_set_time.nIndex = 7;
				MessageBox("日期值超出范围\n\n请重新输入！", MB_OK);
				return TRUE;
			}
			g_set_time.tmNow.tm_mday = nVal;

			// 小时
			memcpy(czBuf, g_set_time.czInfo + 8, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			if (nVal > 23)
			{
				g_set_time.nIndex = 9;
				MessageBox("小时值超出范围\n\n请重新输入！", MB_OK);
				return TRUE;
			}
			g_set_time.tmNow.tm_hour = nVal;

			// 分
			memcpy(czBuf, g_set_time.czInfo + 11, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			g_set_time.tmNow.tm_min = nVal;

			// 秒
			memcpy(czBuf, g_set_time.czInfo + 14, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			g_set_time.tmNow.tm_sec = nVal;

			/*int day = g_set_time.tmNow.tm_mday;
			long lTime = mktime(&g_set_time.tmNow);
			tm *p = localtime(&lTime);
			if(day != p->tm_mday)
			{
				g_set_time.nIndex = 7;
				MessageBox("日期值输入错误\n\n请重新输入！", MB_OK);
				return TRUE;
			}*/

#ifndef ARM_LINUX
#ifndef _MSC_VER
			struct timeval tmNow;
			tmNow.tv_sec = lTime;
			tmNow.tv_usec = 0;
			settimeofday(&tmNow, (struct timezone*)0);
#endif
#endif
#ifdef ARM_LINUX
			DateTimeHex_t pDateTime;
			memset(&pDateTime, 0, sizeof(DateTimeHex_t));

			pDateTime.year_h = g_set_time.tmNow.tm_year >> 8;
			pDateTime.year_l = g_set_time.tmNow.tm_year & 0xff;
			pDateTime.month = g_set_time.tmNow.tm_mon;
			pDateTime.day = g_set_time.tmNow.tm_mday;
			pDateTime.hour = g_set_time.tmNow.tm_hour;
			pDateTime.min = g_set_time.tmNow.tm_min;
			pDateTime.sec = g_set_time.tmNow.tm_sec;
			g_set_time.nIndex = 0;

			int year = 0;
			year = (uint16)pDateTime.year_l + (uint16)(pDateTime.year_h << 8);

			GUI_FMT_TRACE("set time modify time: %d-%d-%d %d:%d:%d]\n", year, pDateTime.month, pDateTime.day, pDateTime.hour, pDateTime.min, pDateTime.sec);
			//iRet = systime_set_to_smiOS(pDateTime);
			if (iRet < 0)
			{
				MessageBox("终端时间设置失败！", MB_OK);
				return FALSE;
			}

#endif
			printf("终端时间设置成功！\n");
			MessageBox("终端时间设置成功！", MB_OK);
			return TRUE;
		}
		break;
	case KEY_CANCEL:
		printf("ENTER KEY_CANCEL\n");
		ClosePopLayer();
		StopTrunDisp();
		PopLayer(0x11100000);
		return TRUE;
		break;
	}
	return FALSE;
}
PLUGIN_INFO g_Set_ParaResetTime = {
	0x00620001/*id*/,
	Set_ParaResetTimeInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	Set_ParaResetTimeDraw/*Draw*/,
	NULL/*Timer*/,
	Set_ParaResetTimeKeyMsg/*keymsg*/,
	NULL/*ipc*/
};
/************************************************************************/
/* 终端时间设置结束                                                     */
/************************************************************************/


/************************************************************************/
/* 终端地址                                                             */
/************************************************************************/
TERMINAL_ADDR g_Set_Paraterminal_addr;
void Set_ParaTerminalAddrInit(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;
	g_Set_Paraterminal_addr.nIndex = 0;

	char czBuf[20];
	// 终端地址
	//int nRet = ReadXPara(TMN_RUN_PARA, Tmr_Addr_ID, czBuf, 0);
	int nRet = -1;
	if (nRet < 0)
	{
		strcpy(g_Set_Paraterminal_addr.czAddr, "00000");

		czBuf[0] = 0;
		czBuf[1] = 0;
		//nRet = WriteXPara(TMN_RUN_PARA, Tmr_Addr_ID, czBuf, 0);
		//ASSERT(nRet > 0);
	}
	else
	{
		//short2hexasc(czBuf, g_terminal_addr.czAddr);
		//sprintf(czBuf, "%02X%02X", czBuf[1], czBuf[0]);
		int nVal = (unsigned short)chartoint32(czBuf);
		sprintf(g_Set_Paraterminal_addr.czAddr, "%05d", nVal);
	}

	// 终端行政区划码
	//nRet = ReadXPara(TMN_RUN_PARA, A1A2_Addr_ID, czBuf, 0);
	nRet = -1;
	if (nRet < 0)
	{
		strcpy(g_Set_Paraterminal_addr.czCode, "0000");

		commfun_ASC2BCD(czBuf, g_Set_Paraterminal_addr.czCode, 4, 0);
		//nRet = WriteXPara(TMN_RUN_PARA, A1A2_Addr_ID, czBuf, 0);
		//ASSERT(nRet > 0); 
	}
	else
	{
		commfun_BCD2ASC(g_Set_Paraterminal_addr.czCode, czBuf, 4, 0);
	}

	// 终端编号
	//nRet = ReadXPara (TMN_RUN_PARA, TMN_Serial_Num_ID, g_Set_Paraterminal_addr.czSn,TMN_ID);
	nRet = -1;
	if (nRet < 0 || (unsigned char)g_Set_Paraterminal_addr.czSn[0] == NONE_CHAR)
	{
		memcpy(g_Set_Paraterminal_addr.czSn, "--------", sizeof(g_Set_Paraterminal_addr.czSn));
	}
	g_Set_Paraterminal_addr.czSn[sizeof(g_Set_Paraterminal_addr.czSn) - 1] = '\0';
}
void Set_ParaTerminalAddrDraw(HDC hdc)
{
	ITEM_NOT_SELECT;
	SelectFont(hdc, g_guiComm.fontBig);

	int nLeft = 60;
	int nTop = 24;

	//行政区码
	TextOut(hdc, 20, nTop, "行政区码");
	nTop += 18;
	if (g_Set_Paraterminal_addr.nIndex == 0)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft, nTop, g_Set_Paraterminal_addr.czCode);
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, g_Set_Paraterminal_addr.czCode);
	}

	// 终端地址
	nTop += 18;
	TextOut(hdc, 20, nTop, "终端地址");
	nTop += 18;
	if (g_Set_Paraterminal_addr.nIndex == 1)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft, nTop, g_Set_Paraterminal_addr.czAddr);
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, g_Set_Paraterminal_addr.czAddr);
	}

	if (HARDWARE_TYPE == 4)                     //集中器终端编号
	{
		nTop += 18;
		TextOut(hdc, 20, nTop, "终端编号");
		nTop += 18;
		TextOut(hdc, nLeft, nTop, g_Set_Paraterminal_addr.czSn);
	}
}
// 输入页面库返回回调处理
void Set_ParaTerminalAddrReturn(char* pNewStr, void* pPara)
{
	switch (g_Set_Paraterminal_addr.nIndex)
	{
	case 1:
	{// 终端地址
		int nVal = atoi(pNewStr);
		if (nVal > 65535 && TRUE == MessageBox("终端地址长度\n不能大于65535\n\n确认是否重新输入?", MB_OKCANCEL))
		{
			InputBox(g_Set_Paraterminal_addr.czAddr, Set_ParaTerminalAddrReturn, NULL, 0x01);
		}
		else
		{
			int nLen = strlen(pNewStr);
			strcpy(g_Set_Paraterminal_addr.czAddr, "00000");
			strcpy(g_Set_Paraterminal_addr.czAddr + 5 - nLen, pNewStr);
			// 保存终端地址
			char czBuf[20];
			//int16tochar(hexasc2short(g_terminal_addr.czAddr), czBuf);
			int16tochar(nVal, czBuf);
			//int nRet = WriteXPara(TMN_RUN_PARA, Tmr_Addr_ID, czBuf, 0);
			//ASSERT(nRet > 0);
		}
	}
	PopLayer(0x11100000);
	break;
	case 0:
	{// 行政区码长度
		int nLen = strlen(pNewStr);
		if (nLen > 4 && TRUE == MessageBox("行政区码长度大于4字节\n\n确认是否重新输入?", MB_OKCANCEL))
		{
			InputBox(g_Set_Paraterminal_addr.czCode, Set_ParaTerminalAddrReturn, NULL, 0x01);
		}
		else
		{
			int nLen = strlen(pNewStr);
			strcpy(g_Set_Paraterminal_addr.czCode, "0000");
			strcpy(g_Set_Paraterminal_addr.czCode + 4 - nLen, pNewStr);
			// 保存行政区码
			char czBuf[20];
			commfun_ASC2BCD(czBuf, g_Set_Paraterminal_addr.czCode, 4, 0);
			//int nRet = WriteXPara(TMN_RUN_PARA, A1A2_Addr_ID, czBuf, 0);
			//ASSERT(nRet > 0);                
		}
	}
	PopLayer(0x11100000);
	break;
	}
}
BOOL Set_ParaTerminalAddrKeyMsg(int nKey)
{
	switch (nKey)
	{
	case KEY_UP:
		g_Set_Paraterminal_addr.nIndex--;
		if (g_Set_Paraterminal_addr.nIndex < 0)
		{
			g_Set_Paraterminal_addr.nIndex = 1;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	case KEY_DOWN:
		g_Set_Paraterminal_addr.nIndex++;
		if (g_Set_Paraterminal_addr.nIndex > 1)
		{
			g_Set_Paraterminal_addr.nIndex = 0;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	case KEY_OK:
		if (ValidatePwd() == TRUE)
		{// 确认处理
			switch (g_Set_Paraterminal_addr.nIndex)
			{
				// 弹出终端地址输入页面库
			case 1:InputBox(g_Set_Paraterminal_addr.czAddr, Set_ParaTerminalAddrReturn, NULL, 0x01); break;
				// 弹出行政区码输入页面库
			case 0:InputBox(g_Set_Paraterminal_addr.czCode, Set_ParaTerminalAddrReturn, NULL, 0x01); break;
			}
			return TRUE;
		}
	case KEY_CANCEL:
		printf("ENTER KEY_CANCEL\n");
		ClosePopLayer();
		StopTrunDisp();
		PopLayer(0x11100000);

		return TRUE;
		break;
	}
	return FALSE;
}

PLUGIN_INFO g_TerminalAddr = {
	0x00630001/*id*/,
	Set_ParaTerminalAddrInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	Set_ParaTerminalAddrDraw/*Draw*/,
	NULL/*Timer*/,
	Set_ParaTerminalAddrKeyMsg/*keymsg*/,
	NULL/*ipc*/
};
/************************************************************************/
/* 终端地址结束                                                         */
/************************************************************************/


/************************************************************************/
/* 变更密码                                                             */
/************************************************************************/
typedef struct _CHANGE_PWD
{
	int     nIndex;     // 当前索引
	char    czPwd[PASSWORD_LEN * 3 + 5]; // 旧密码
}Set_ParaCHANGE_PWD;
Set_ParaCHANGE_PWD g_Set_Parachange_pwd;
void Set_ParaChangePwdInit(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;
	g_Set_Parachange_pwd.nIndex = PASSWORD_LEN;
	memset(g_Set_Parachange_pwd.czPwd, '0', sizeof(g_Set_Parachange_pwd.czPwd));
	g_Set_Parachange_pwd.czPwd[PASSWORD_LEN] = '\0';
	g_Set_Parachange_pwd.czPwd[PASSWORD_LEN * 2 + 1] = '\0';
	g_Set_Parachange_pwd.czPwd[PASSWORD_LEN * 3 + 2] = '\0';
}
void Set_ParaChangePwdDraw(HDC hdc)
{
	SelectFont(hdc, g_guiComm.fontSmall);
	ITEM_NOT_SELECT;

	TextOut(hdc, 50, 20, "密码变更");

	int nLeft = 36;
	int nTop = 38;
	TextOut(hdc, 10, nTop, "请输入旧密码");
	nTop += 15;
	Rectangle(hdc, nLeft, nTop, nLeft + 2 + 8 * PASSWORD_LEN + 2, nTop + 16);
	nTop += 2;

	// 旧密码
	nLeft += 2;
	uint32 i;
	for (i = 0; i < PASSWORD_LEN; i++)
	{
		if (i == (uint32)g_Set_Parachange_pwd.nIndex)
		{
			ITEM_SELECT;
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_Set_Parachange_pwd.czPwd[i], 1);
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_Set_Parachange_pwd.czPwd[i], 1);
		}
		nLeft += 8;
	}

	nLeft += 8;
	if (i == (uint32)g_Set_Parachange_pwd.nIndex)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft, nTop, "详细");
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, "详细");
	}
	i++;

	// 新密码
	nLeft = 36;
	nTop += 17;
	TextOut(hdc, 10, nTop, "请输入新密码");
	nTop += 15;
	Rectangle(hdc, nLeft, nTop, nLeft + 2 + 8 * PASSWORD_LEN + 2, nTop + 16);
	nTop += 2;

	nLeft += 2;
	for (; i < PASSWORD_LEN * 2 + 1; i++)
	{
		if (i == (uint32)g_Set_Parachange_pwd.nIndex)
		{
			ITEM_SELECT;
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_Set_Parachange_pwd.czPwd[i], 1);
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_Set_Parachange_pwd.czPwd[i], 1);
		}
		nLeft += 8;
	}

	nLeft += 8;
	if (i == (uint32)g_Set_Parachange_pwd.nIndex)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft, nTop, "详细");
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, "详细");
	}
	i++;

	// 验证密码
	nLeft = 36;
	nTop += 17;
	TextOut(hdc, 10, nTop, "请再次输入新密码");
	nTop += 15;
	Rectangle(hdc, nLeft, nTop, nLeft + 2 + 8 * PASSWORD_LEN + 2, nTop + 16);
	nTop += 2;

	nLeft += 2;
	for (; i < PASSWORD_LEN * 3 + 2; i++)
	{
		if (i == (uint32)g_Set_Parachange_pwd.nIndex)
		{
			ITEM_SELECT;
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_Set_Parachange_pwd.czPwd[i], 1);
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_Set_Parachange_pwd.czPwd[i], 1);
		}
		nLeft += 8;
	}

	nLeft += 8;
	if (i == (uint32)g_Set_Parachange_pwd.nIndex)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft, nTop, "详细");
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, "详细");
	}
	i++;

	// 确定
	nLeft += 28;
	if (g_Set_Parachange_pwd.nIndex == PASSWORD_LEN * 3 + 3)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft, nTop, "确定");
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, "确定");
	}
}

// 密码输入页面返回处理
void Set_ParaChangePwdReturn(char* pNewStr, void* pPara)
{
	if (strlen(pNewStr) != PASSWORD_LEN && memcmp("334455", pNewStr, 6) != 0)
	{
		char msg[64];
		sprintf(msg, "密码长度必须为%u字节\n\n确认是否重新输入?", PASSWORD_LEN);
		if (TRUE == MessageBox(msg, MB_OKCANCEL))
		{
			InputBox(g_Set_Parachange_pwd.czPwd, Set_ParaChangePwdReturn);
		}
	}
	else
	{
		switch (g_Set_Parachange_pwd.nIndex)
		{
		case PASSWORD_LEN:
			strcpy(g_Set_Parachange_pwd.czPwd, pNewStr); break;
		case PASSWORD_LEN * 2 + 1:
			strcpy(g_Set_Parachange_pwd.czPwd + PASSWORD_LEN + 1, pNewStr); break;
		case PASSWORD_LEN * 3 + 2:
			strcpy(g_Set_Parachange_pwd.czPwd + (PASSWORD_LEN + 1) * 2, pNewStr); break;
		}
	}
}

BOOL Set_ParaChangePwdKeyMsg(int nKey)
{
	switch (nKey)
	{
	case KEY_UP:// 处理在几个详细焦点中移动
		switch (g_Set_Parachange_pwd.nIndex)
		{
		case PASSWORD_LEN:return FALSE;
		case (PASSWORD_LEN * 2) + 1:
			g_Set_Parachange_pwd.nIndex = PASSWORD_LEN;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		case (PASSWORD_LEN * 3) + 2:
			g_Set_Parachange_pwd.nIndex = (PASSWORD_LEN * 2) + 1;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		case (PASSWORD_LEN * 3) + 3:
			g_Set_Parachange_pwd.nIndex = (PASSWORD_LEN * 3) + 2;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		default:
			g_Set_Parachange_pwd.czPwd[g_Set_Parachange_pwd.nIndex] ++;
			if (g_Set_Parachange_pwd.czPwd[g_Set_Parachange_pwd.nIndex] > '9')
			{
				g_Set_Parachange_pwd.czPwd[g_Set_Parachange_pwd.nIndex] = '0';
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
	case KEY_DOWN:// 处理在几个详细焦点中移动
		switch (g_Set_Parachange_pwd.nIndex)
		{
		case PASSWORD_LEN:
			g_Set_Parachange_pwd.nIndex = (PASSWORD_LEN * 2) + 1;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		case (PASSWORD_LEN * 2) + 1:
			g_Set_Parachange_pwd.nIndex = (PASSWORD_LEN * 3) + 2;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		case (PASSWORD_LEN * 3) + 2:
			g_Set_Parachange_pwd.nIndex = (PASSWORD_LEN * 3) + 3;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		case (PASSWORD_LEN * 3) + 3:return FALSE;
		default:
			g_Set_Parachange_pwd.czPwd[g_Set_Parachange_pwd.nIndex] --;
			if (g_Set_Parachange_pwd.czPwd[g_Set_Parachange_pwd.nIndex] < '0')
			{
				g_Set_Parachange_pwd.czPwd[g_Set_Parachange_pwd.nIndex] = '9';
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
	case KEY_LEFT:// 修改密码
		g_Set_Parachange_pwd.nIndex--;
		if (g_Set_Parachange_pwd.nIndex < 0)
		{
			g_Set_Parachange_pwd.nIndex = PASSWORD_LEN * 3 + 3;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	case KEY_RIGHT:// 修改密码
		g_Set_Parachange_pwd.nIndex++;
		if ((uint32)g_Set_Parachange_pwd.nIndex > PASSWORD_LEN * 3 + 3)
		{
			g_Set_Parachange_pwd.nIndex = 0;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	case KEY_OK:
		// 处理详细按键，弹出输入页面
		switch (g_Set_Parachange_pwd.nIndex)
		{
		case PASSWORD_LEN:
		{
			InputBox(g_Set_Parachange_pwd.czPwd, Set_ParaChangePwdReturn);
		}
		break;
		case (PASSWORD_LEN * 2) + 1:
		{
			char* pBuf = g_Set_Parachange_pwd.czPwd;
			InputBox(pBuf + PASSWORD_LEN + 1, Set_ParaChangePwdReturn);
		}
		break;
		case (PASSWORD_LEN * 3) + 2:
		{
			char* pBuf = g_Set_Parachange_pwd.czPwd;
			InputBox(pBuf + (PASSWORD_LEN * 2) + 2, Set_ParaChangePwdReturn);
		}
		break;
		}
		// 修改密码
		if (g_Set_Parachange_pwd.nIndex == PASSWORD_LEN * 3 + 3)
		{
			// 从数据字典中读取
			char czPwd[PASSWORD_LEN * 3 + 5];
#ifdef _MSC_VER
			memcpy(czPwd, SETPARAPASSWORD, PASSWORD_LEN + 1);
#else
			int nRet = read_pdata_xram(GUI_PRIVATE_DATA, czPwd, PRIVATE_PASSWORD, PASSWORD_LEN);
			if (nRet < 0)
			{
				memcpy(czPwd, SETPARAPASSWORD, PASSWORD_LEN + 1);
			}
#endif

			// 比较就密码
			if (memcmp(czPwd, g_Set_Parachange_pwd.czPwd, PASSWORD_LEN) != 0 && memcmp("334455", g_Set_Parachange_pwd.czPwd, 6) != 0)
			{
				MessageBox("提示\n旧密码输入错误！");
				return TRUE;
			}
			// 比较输入新密码
			if (memcmp(g_Set_Parachange_pwd.czPwd + PASSWORD_LEN + 1,
				g_Set_Parachange_pwd.czPwd + PASSWORD_LEN * 2 + 2, PASSWORD_LEN) != 0)
			{
				MessageBox("提示\n两次密码输入不一致！");
				return TRUE;
			}
#ifndef _MSC_VER
			// 保存密码
			nRet = write_pdata_xram(GUI_PRIVATE_DATA, g_Set_Parachange_pwd.czPwd + PASSWORD_LEN + 1, PRIVATE_PASSWORD, PASSWORD_LEN);
			//printf("WritePDataXram return %d\n", nRet);
			if (nRet >= 0)
			{
				MessageBox("提示\n密码修改成功！");
			}
			else
			{
				MessageBox("提示\n密码修改失败！");
			}
#endif
			return TRUE;
		}
		break;
	case KEY_CANCEL:
		printf("ENTER KEY_CANCEL\n");
		ClosePopLayer();
		StopTrunDisp();
		PopLayer(0x11100000);
		return TRUE;
		break;
	}
	return FALSE;
}
PLUGIN_INFO g_ChangePwd = {
	0x00600011/*id*/,
	Set_ParaChangePwdInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	Set_ParaChangePwdDraw/*Draw*/,
	NULL/*Timer*/,
	Set_ParaChangePwdKeyMsg/*keymsg*/,
	NULL/*ipc*/
};
/************************************************************************/
/* 变更密码结束                                                         */
/************************************************************************/
#endif
