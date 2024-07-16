/*
***************************************************************************
  Copyright (c) 2006-2007,�Ͼ��������������������ι�˾
             All rights reserved.
  ��������: main.h
  ��    �ܣ�FKGA42��ʾģ��˵�ҳ���ʵ���ļ�����ɲ˵��Ļ����Լ�������Ϣ�Ĵ���
  �� �� �ˣ�gaoyf
  ����ʱ�䣺2007-1
  
  �汾��ʷ:
  
    v1.0:
      2006-1-8, gaoyf : New file   
     
  ����ҳ��⣺
    1�в˵�ҳ��  0x00A00000
    2�в˵�ҳ��  0x00A10000
	������ר�ò˵�  0x00A20000
  
***************************************************************************
*/

#include "main.h"
#include "commdef.h"
#include "commfun.h"
#include "gui_prtl.h"


MENU_INFO* g_menuMain[1000];
int	g_nMenus = 0;
int g_nParentIndex = -1;
int g_nCurIndex = -1;   // ֻ����"ȡ��"������һ���˵�ʱ�ж�ѡ����ʹ�ã�ȷ��������Ϊ-1

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
*���ܣ����Ͻ���ʾѡ��״̬���ذ�ť
*��������������Ӹ��������ȷ�Ϻ��ִ��ȡ������������һ�˵�
*���룺x,y�������꣬w,h�����͸�
*�������
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
 �����˵�ҳ��
 ��ɹ���:160*160ҳ�������1���в˵��Ĳ���
*************************************************************************/
typedef struct _MENU_BASE_INFO
{
	int nMenuShows;// �ڸ�ҳ���������ʾ�Ĳ˵���
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
	TextOut(hdc, nLeft1, y, "ʱ������");
	
	y += 16;
	ITEM_NOT_SELECT;
	if (n == 1)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "����������ն˵�ַ������");

	y += 16;
	ITEM_NOT_SELECT;
	if (n == 2)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "ͨѶͨ������");
	
	y += 16;
	ITEM_NOT_SELECT;
	if (n == 3)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "�������������");
	
	y += 16;
	ITEM_NOT_SELECT;
	if (n == 4)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "���������");

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
		if(TRUE != MessageBox("��ʾ\n\n�Ƿ��������ģʽ��", MB_OKCANCEL))
		{
			return;
		}
	}
	else
	{
		if(TRUE != MessageBox("��ʾ\n\n�Ƿ��������ģʽ��", MB_OKCANCEL))
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
* @brief     ��������ʾģʽ�л�
* @param[in] ��g_bAllMenu TRUE ���ز˵���ʾ��FALSE��������ʾ
* @param[out]��
* @return    ��
* @Create    : ÷����
* @Date      ��2020-12-10
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

	// �������˵����ID��ȷ����ǰ��plug
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
	
	// ��ʼ����ǰҳ��⣬��Uninitialize���ͷ�
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
		if(TRUE != MessageBox("��ʾ\n\n�Ƿ�ر���չ�˵��", MB_OKCANCEL))
		{
			return;
		}
	}
	else
	{
		if(TRUE != MessageBox("��ʾ\n\n�Ƿ���ʾ��չ�˵��", MB_OKCANCEL))
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
	{// ��ʾ���¼�ͷ
		g_guiComm.nArrow = 0;
	}
	else if(g_menuThree.nMenuShowMin > 0)
	{// ��ʾ�ϼ�ͷ
		g_guiComm.nArrow = 1;
	}
	else if(g_menuThree.nMenuShowMax < g_nCurMenuCnt-1)
	{// ��ʾ�¼�ͷ
		g_guiComm.nArrow = 2;
	}
	else
	{
		g_guiComm.nArrow = -1;
	}
}
/*********************************************************************
	�������ܵ�Ԫ����ʾ���ģ��
	����������    ��ʼ���˵�����̬������ǰ�˵�����
	���������    DWORD dwPluginId���˵�ҳ���ID
	���������
	��������ֵ��
	�ڲ��������̣�
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
	// ���ݸ��˵�id�����Ӳ˵���
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

	// ���õ�ǰѡ����
	g_nCurSelect = 0;
	if (g_nCurIndex != -1)
	{
		// ==-1��ʾ��"ȷ��"�������룬���Ե�ǰѡ����Ϊ��һ��
		// !=-1��ʾ��"ȡ��"�������룬��Ҫ�ж��Ӳ˵��ĸ��˵���Ϊ��ǰѡ����
		for (i = 0; i < g_nCurMenuCnt; i++)
		{
			if (g_nCurMenuIndex[i] == g_nCurIndex)
			{
				g_nCurSelect = i;
				break;
			}
		}
	}

	// ������ʾ������Ϊ���ܲ˵�������Ƚ϶࣬����ȫ����ʾ
	// �˴���Ҫ��֤ѡ�в˵�������ʾ������
	if (g_nCurSelect > g_menuBase.nMenuShows - 1)
	{// �����ǰѡ�����Ѿ������ܹ���ʾ�˵�������ֵ
		// ��ô�����ʾ��Ϊ��ǰѡ��������ж�����Сѡ����
		g_menuBase.nMenuShowMax = g_nCurSelect;
		g_menuBase.nMenuShowMin = g_menuBase.nMenuShowMax - g_menuBase.nMenuShows + 1;
	}
	else
	{// ������С��ʾ��Ϊ0
		g_menuBase.nMenuShowMin = 0;
		g_menuBase.nMenuShowMax = (g_nCurMenuCnt > g_menuBase.nMenuShows) ? g_menuBase.nMenuShows - 1 : g_nCurMenuCnt - 1;
	}
	SetArrow();
}

/*********************************************************************
	�������ܵ�Ԫ����ʾ���ģ��
	����������    ���ƶ�̬�˵�
	���������    HDC hdc�������豸�����ľ��
	���������
	��������ֵ��
	�ڲ��������̣�
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
	//����˵���ʾ��������
	int Lenths[100] = { 0 };
	//���ؼ�ͷ����ʾ
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

	//�ҳ���������Сֵ


	
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

#if HARDWARE_TYPE == 4 || (HARDWARE_TYPE == 3 && HUNAN_FUN == 1)        // 375.2�淶Ҫ�����������˵��������
		//        sprintf(czBuf, "%s", pMenu->czCaption);
#endif

#if MODULARIZATION_TMN_EN
		if (g_nCurSelect == i && g_ncancelkey == 0)
#else
		if (g_nCurSelect == i)
#endif
		{
			// ���÷��Բ���
			ITEM_SELECT;
		}
		else
		{
		
			// �ָ����ԣ��������Բ���Ҫ����������ɫ��
			ITEM_NOT_SELECT;
		}
		if (HARDWARE_TYPE == 1 || HARDWARE_TYPE == 2 || HARDWARE_TYPE == 3)
		{
			TextOut(hdc, length, rc.top, czBuf);
		}
		else if (HARDWARE_TYPE == 4) // 375.2�淶Ҫ�����������˵��������
		{

			//DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER);
			TextOut(hdc, length, rc.top, czBuf);
		}
		if (g_nCurSelect == i)
		{
			// �ָ����ԣ��������Բ���Ҫ����������ɫ��
			ITEM_NOT_SELECT;
		}
		Top += (16 + g_nInterval);
	}


}

/*********************************************************************
	�������ܵ�Ԫ����ʾ���ģ��
	����������    ���ƶ�̬�˵�
	���������    HDC hdc�������豸�����ľ��
	���������
	��������ֵ��
	�ڲ��������̣�
********************************************************************/
void MenuDraw(HDC hdc)
{
	MenuDraw_add_name(hdc, ADV_SHOW_NAME);
}

/*********************************************************************
	�������ܵ�Ԫ����ʾ���ģ��
	����������    ����˵�������Ϣ
	���������    int nKey��������Ϣ
	���������
	��������ֵ��  BOOL�����������TRUE,ʹ��ȱʡ������FALSE
	�ڲ��������̣�
********************************************************************/
extern void KeepNextLayer();
extern void SetSubLayerArrow();
void MenuOkKeyMsg()
{
	// ��ȷ��������ʾ��ǰ��϶�Ϊ�˵�
	// ������һ��ҳ�棺����Ϊ�Ӳ˵���Ҳ����Ϊ����ҳ��
	int nIndex;
	MENU_INFO* pMenu;
	PLUGIN_INFO* pPlugin;
	
	// ��ȡ��ǰѡ����������˵����е�����ֵ
	ASSERT(g_nCurSelect >= 0 && g_nCurSelect < g_nCurMenuCnt);
    
	nIndex = g_nCurMenuIndex[g_nCurSelect];
//    printf("g_nCurSelect %d nIndex %d\n", g_nCurSelect, nIndex);
    
	ASSERT(nIndex >= 0 && nIndex < g_nMenus);
	// ��������ֵ��ȡ��Ӧ�Ĳ˵���ָ��
	pMenu = g_menuMain[nIndex];
	ASSERT(pMenu != NULL&& pMenu->nPlugCnt > 0);
	
	// ��ȡ�˵����Ӧ��ҳ�棺�Ӳ˵����ߵ�һ������ҳ��
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
	
	// �Ծɵĵ�ǰҳ������ͷŲ���
	ASSERT(g_guiMain.pluginCur != NULL);
	if(g_guiMain.pluginCur->pfnUninitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnUninitLayer();
	}
	
	g_guiMain.pluginCur = pPlugin;
	// ���ø��˵�����Ϣ������Ϊ�˱�֤"ȡ��"����ʱ���ܹ��ҵ����˵���
	// ��ֻ�԰����Ӳ˵�ʱ������
	if(pMenu->bMenuAggregate == TRUE)
	{
		g_nParentIndex = nIndex;
	}
	
	g_narrMenuIndex[g_nMenuNodes] = 0;
	g_nMenuNodes ++;
	
	g_guiMain.pPrentMenu = pMenu;
	g_nCurIndex = nIndex;
	g_guiMain.nPageLayerIndex = 0;
	
	// ���µ�ǰҳ���ʼ������
	g_guiComm.bKeyDown = TRUE;// �����һҳʱ��Ϊ���°���
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
    				GUI_FMT_DEBUG("����app:%s, activeID: %d ret %d\n",appGuiInfoSaved.applogin[i].appname,appGuiInfoSaved.loginID[i], ret);
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
		GUI_FMT_DEBUG("����gui���ͼ����¼�ʧ��,g_nCurSelect = %d, appGuiInfoSaved.loginAppCnt = %d\n",
						g_nCurSelect,appGuiInfoSaved.loginAppCnt);
	}
}

extern void StrartTrunDisp();
void MenuCancelKeyMsg()
{
	MENU_INFO* pMenu;
	PLUGIN_INFO* pPlugin;
	if(g_nParentIndex == -1)
	{// �Ѿ������˵��ˣ����������Ϸ�ҳ��
//		StrartTrunDisp();
		return;
	}
	
	ASSERT(g_nParentIndex >= 0 && g_nParentIndex < g_nMenus);
	// ��ȡ���˵�ָ��
	pMenu = g_menuMain[g_nParentIndex];
	ASSERT(pMenu != NULL && pMenu->nPlugCnt > 0);
	// ��ȡ���˵���Ӧ�Ĳ˵�ҳ���ָ��
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
	
	// �Ծɵĵ�ǰҳ������ͷŲ���
	ASSERT(g_guiMain.pluginCur != NULL);
	if(g_guiMain.pluginCur->pfnUninitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnUninitLayer();
	}
	
	// ����Ϊ��ǰҳ���
	g_guiMain.pluginCur = pPlugin;
	// ����ǰ�ĸ��˵���ID��Ϊ������һ��ҳ��ĵ�ǰѡ��������
	g_nCurIndex = g_nParentIndex;
	// ���øò˵���ĸ��˵���Ϊ��һҳ��ĸ��˵���
	g_nParentIndex = pMenu->nParent;
	
	g_guiMain.pPrentMenu = pMenu;
	g_guiMain.nPageLayerIndex = 0;
	
	// ���µ�ǰҳ���ʼ������
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
		//���ؼ�ͷ����ʾ
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
	// �˵�ѡ��������
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
		// �����˵���ʾ��������˵����ȫ�Ե�����£�
		// ѡ�����Ѿ�С����С��ʾ�˵������ʾ��������
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
			// �˵�ѡ��������
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
		// �����˵���ʾ��������˵����ȫ�Ե�����£�
		// ѡ�����Ѿ����������ʾ�˵������ʾ��������
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
			// �˵�ѡ��������
		if (g_nCurSelect > 0)
		{
			g_nCurSelect--;
			// �����˵���ʾ��������˵����ȫ�Ե�����£�
			// ѡ�����Ѿ�С����С��ʾ�˵������ʾ��������
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
			{// �����ǰѡ�����Ѿ������ܹ���ʾ�˵�������ֵ
				// ��ô�����ʾ��Ϊ��ǰѡ��������ж�����Сѡ����
				g_menuBase.nMenuShowMax = g_nCurSelect;
				g_menuBase.nMenuShowMin = g_menuBase.nMenuShowMax - g_menuBase.nMenuShows + 1;
			}
			else
			{// ������С��ʾ��Ϊ0
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
			// �˵�ѡ��������
		if (g_nCurSelect + 1 < g_nCurMenuCnt)
		{
			g_nCurSelect++;
			// �����˵���ʾ��������˵����ȫ�Ե�����£�
			// ѡ�����Ѿ����������ʾ�˵������ʾ��������
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

    // ���ݸ��˵�id�����Ӳ˵���
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

    // ���õ�ǰѡ����
    g_nCurSelect = 0;
    if(g_nCurIndex != -1)
    {
        // ==-1��ʾ��"ȷ��"�������룬���Ե�ǰѡ����Ϊ��һ��
        // !=-1��ʾ��"ȡ��"�������룬��Ҫ�ж��Ӳ˵��ĸ��˵���Ϊ��ǰѡ����
        for(i = 0; i < g_nCurMenuCnt; i ++)
        {
            if(g_nCurMenuIndex[i] == g_nCurIndex)
            {
                g_nCurSelect = i;
                break;
            }
        }
    }

    // ������ʾ������Ϊ���ܲ˵�������Ƚ϶࣬����ȫ����ʾ
    // �˴���Ҫ��֤ѡ�в˵�������ʾ������
	if(g_nCurSelect > g_menuThree.nMenuShows-1)
	{// �����ǰѡ�����Ѿ������ܹ���ʾ�˵�������ֵ
        // ��ô�����ʾ��Ϊ��ǰѡ��������ж�����Сѡ����
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
	{// ������С��ʾ��Ϊ0
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
            // ���÷��Բ���
			ITEM_SELECT;
			//TextOut(hdc, nLeft, nTop, finalShowName);
			DrawText(hdc, showName, strlen(showName), &rc, DT_CENTER|DT_VCENTER);
            // �ָ����ԣ��������Բ���Ҫ����������ɫ��
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
            // �˵�ѡ��������
			if(g_nCurSelect > 2)
			{	
				g_nCurSelect-=3;
                // �����˵���ʾ��������˵����ȫ�Ե�����£�
                // ѡ�����Ѿ�С����С��ʾ�˵������ʾ��������
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
				{// ����ܲ˵����Ѿ������ܹ���ʾ�˵�������ֵ
					// ��ô�����ʾ��Ϊ��ǰѡ��������ж�����Сѡ����
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
				{// ������С��ʾ��Ϊ0
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
            // �˵�ѡ��������
			if(g_nCurSelect+3 < g_nCurMenuCnt)
			{
				g_nCurSelect += 3;
                // �����˵���ʾ��������˵����ȫ�Ե�����£�
                // ѡ�����Ѿ����������ʾ�˵������ʾ��������
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
            // �˵�ѡ��������
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
				{// �����ǰѡ�����Ѿ������ܹ���ʾ�˵�������ֵ
					// ��ô�����ʾ��Ϊ��ǰѡ��������ж�����Сѡ����
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
				{// ������С��ʾ��Ϊ0
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
            // �˵�ѡ��������
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
	�������ܵ�Ԫ����ʾ���ģ��
	����������    ��ʼ���˵�����̬������ǰ�˵�����
	���������    DWORD dwPluginId���˵�ҳ���ID
	���������
	��������ֵ��
	�ڲ��������̣�
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

    // ���ݸ��˵�id�����Ӳ˵���
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

    // ���õ�ǰѡ����
    g_nCurSelect = 0;
    if(g_nCurIndex != -1)
    {
        // ==-1��ʾ��"ȷ��"�������룬���Ե�ǰѡ����Ϊ��һ��
        // !=-1��ʾ��"ȡ��"�������룬��Ҫ�ж��Ӳ˵��ĸ��˵���Ϊ��ǰѡ����
        for(i = 0; i < g_nCurMenuCnt; i ++)
        {
            if(g_nCurMenuIndex[i] == g_nCurIndex)
            {
                g_nCurSelect = i;
                break;
            }
        }
    }

    // ������ʾ������Ϊ���ܲ˵�������Ƚ϶࣬����ȫ����ʾ
    // �˴���Ҫ��֤ѡ�в˵�������ʾ������
	if(g_nCurSelect > g_menuTwo.nMenuShows-1)
	{// �����ǰѡ�����Ѿ������ܹ���ʾ�˵�������ֵ
        // ��ô�����ʾ��Ϊ��ǰѡ��������ж�����Сѡ����
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
	{// ������С��ʾ��Ϊ0
		g_menuTwo.nMenuShowMin = 0;
		g_menuTwo.nMenuShowMax = (g_nCurMenuCnt>g_menuTwo.nMenuShows)?g_menuTwo.nMenuShows-1:g_nCurMenuCnt-1;
	}

	SetArrow();
}

/*********************************************************************
	�������ܵ�Ԫ����ʾ���ģ��
	����������    ���ƶ�̬�˵�
	���������    HDC hdc�������豸�����ľ��
	���������
	��������ֵ��
	�ڲ��������̣�
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
            // ���÷��Բ���
			ITEM_SELECT;
			TextOut(hdc, nLeft, nTop, czBuf);
            // �ָ����ԣ��������Բ���Ҫ����������ɫ��
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
	�������ܵ�Ԫ����ʾ���ģ��
	����������    ����˵�������Ϣ
	���������    int nKey��������Ϣ
	���������
	��������ֵ��  BOOL�����������TRUE,ʹ��ȱʡ������FALSE
	�ڲ��������̣�
********************************************************************/
BOOL TwoMenuKeyMsg(int nKey)
{
	switch(nKey)
	{
	case KEY_UP:
		{
            // �˵�ѡ��������
			if(g_nCurSelect > 1)
			{
				g_nCurSelect-=2;
                // �����˵���ʾ��������˵����ȫ�Ե�����£�
                // ѡ�����Ѿ�С����С��ʾ�˵������ʾ��������
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
				{// �����ǰѡ�����Ѿ������ܹ���ʾ�˵�������ֵ
					// ��ô�����ʾ��Ϊ��ǰѡ��������ж�����Сѡ����
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
				{// ������С��ʾ��Ϊ0
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
            // �˵�ѡ��������
			if(g_nCurSelect+2 < g_nCurMenuCnt)
			{
				g_nCurSelect += 2;
                // �����˵���ʾ��������˵����ȫ�Ե�����£�
                // ѡ�����Ѿ����������ʾ�˵������ʾ��������
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
            // �˵�ѡ��������
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
				{// �����ǰѡ�����Ѿ������ܹ���ʾ�˵�������ֵ
					// ��ô�����ʾ��Ϊ��ǰѡ��������ж�����Сѡ����
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
				{// ������С��ʾ��Ϊ0
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
            // �˵�ѡ��������
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




