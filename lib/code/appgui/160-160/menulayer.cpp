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
int g_nMenus = 0;
int g_nParentIndex = -1;
int g_nCurIndex = -1;   // ֻ����"ȡ��"������һ���˵�ʱ�ж�ѡ����ʹ�ã�ȷ��������Ϊ-1

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
*���ܣ����Ͻ���ʾѡ��״̬���ذ�ť
*��������������Ӹ��������ȷ�Ϻ��ִ��ȡ������������һ�˵�
*���룺x,y�������꣬w,h�����͸�
*�������
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
 �����˵�ҳ��
 ��ɹ���:160*160ҳ�������1���в˵��Ĳ���
*************************************************************************/
typedef struct _MENU_BASE_INFO
{
	int nMenuShows;// �ڸ�ҳ���������ʾ�Ĳ˵���
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
	TextOut(hdc, nLeft1, y, "ʱ������");

	y += 16;
	ITEM_NOT_SELECT;
	if (n == 1)
	{
		ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "����������ն˵�ַ������");

	y += 16;
	ITEM_NOT_SELECT;
	if (n == 2)
	{
		ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "ͨѶͨ������");

	y += 16;
	ITEM_NOT_SELECT;
	if (n == 3)
	{
		ITEM_SELECT;
	}
	TextOut(hdc, nLeft1, y, "�������������");

	y += 16;
	ITEM_NOT_SELECT;
	if (n == 4)
	{
		ITEM_SELECT;
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

	if (ValidatePwd() != TRUE)
	{
		return;
	}
	if (g_bAllMenu == TRUE)
	{
		if (TRUE != MessageBox("��ʾ\n\n�Ƿ��������ģʽ��", MB_OKCANCEL))
		{
			return;
		}
	}
	else
	{
		if (TRUE != MessageBox("��ʾ\n\n�Ƿ��������ģʽ��", MB_OKCANCEL))
		{
			return;
		}
	}
	PopLayer(0x11100000);


	//UpdateWindow(g_guiComm.hMainWnd, TRUE);

}
#endif

//app�˳�ʱ����ʼ��ҳ��
void init_layer()
{
    ReadConfig(FALSE);
    if (g_guiMain.pluginCur->pfnUninitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnUninitLayer();
	}
	// �������˵����ID��ȷ����ǰ��plug
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
		if (TRUE != MessageBox("��ʾ\n\n�Ƿ�ر���չ�˵��", MB_OKCANCEL))
		{
			return;
		}
	}
	else
	{
		if (TRUE != MessageBox("��ʾ\n\n�Ƿ���ʾ��չ�˵��", MB_OKCANCEL))
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
	// �������˵����ID��ȷ����ǰ��plug
	g_guiMain.pluginCur = GetPlugIn(g_guiMain.nMenuRootId);
	ASSERT(g_guiMain.pluginCur != NULL);

	g_nParentIndex = -1;
	g_nCurIndex = -1;
	g_nCurMenuCnt = 0;
	g_nCurSelect = 0;
	g_nMaxMenuChar = 0;

	// ��ʼ����ǰҳ��⣬��Uninitialize���ͷ�
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
	{// ��ʾ���¼�ͷ
		g_guiComm.nArrow = 0;
	}
	else if (g_menuBase.nMenuShowMin > 0)
	{// ��ʾ�ϼ�ͷ
		g_guiComm.nArrow = 1;
	}
	else if (g_menuBase.nMenuShowMax < g_nCurMenuCnt - 1)
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
		sprintf(czBuf, "%s", "���˵�");
		rc.top = Top;
		rc.bottom = Top + 16;
		DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER);
		Top += 16;
	}
#endif
	//���ؼ�ͷ����ʾ

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

#if HARDWARE_TYPE == 4 || (HARDWARE_TYPE == 3 && HUNAN_FUN == 1)        // 375.2�淶Ҫ�����������˵��������
		sprintf(czBuf, "%s", pMenu->czCaption);
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
			TextOut(hdc, nLeft, rc.top, czBuf);
		}
		else if (HARDWARE_TYPE == 4) // 375.2�淶Ҫ�����������˵��������
		{
			DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER);
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
	ASSERT(pMenu != NULL && pMenu->nPlugCnt > 0);

	// ��ȡ�˵����Ӧ��ҳ�棺�Ӳ˵����ߵ�һ������ҳ��
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

	// �Ծɵĵ�ǰҳ������ͷŲ���
	ASSERT(g_guiMain.pluginCur != NULL);
	if (g_guiMain.pluginCur->pfnUninitLayer != NULL)
	{
		g_guiMain.pluginCur->pfnUninitLayer();
	}

	g_guiMain.pluginCur = pPlugin;
	// ���ø��˵�����Ϣ������Ϊ�˱�֤"ȡ��"����ʱ���ܹ��ҵ����˵���
	// ��ֻ�԰����Ӳ˵�ʱ������
	if (pMenu->bMenuAggregate == TRUE)
	{
		g_nParentIndex = nIndex;
	}

	g_narrMenuIndex[g_nMenuNodes] = 0;
	g_nMenuNodes++;

	g_guiMain.pPrentMenu = pMenu;
	g_nCurIndex = nIndex;
	g_guiMain.nPageLayerIndex = 0;

	// ���µ�ǰҳ���ʼ������
	g_guiComm.bKeyDown = TRUE;// �����һҳʱ��Ϊ���°���
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
	{// �Ѿ������˵��ˣ����������Ϸ�ҳ��
		//StrartTrunDisp();
		appGUI_req_exit();
		return;
	}

	ASSERT(g_nParentIndex >= 0 && g_nParentIndex < g_nMenus);
	// ��ȡ���˵�ָ��
	pMenu = g_menuMain[g_nParentIndex];
	ASSERT(pMenu != NULL && pMenu->nPlugCnt > 0);
	// ��ȡ���˵���Ӧ�Ĳ˵�ҳ���ָ��
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

	// �Ծɵĵ�ǰҳ������ͷŲ���
	ASSERT(g_guiMain.pluginCur != NULL);
	if (g_guiMain.pluginCur->pfnUninitLayer != NULL)
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

	// ���ݸ��˵�id�����Ӳ˵���
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
	if (g_nCurSelect > g_menuTwo.nMenuShows - 1)
	{// �����ǰѡ�����Ѿ������ܹ���ʾ�˵�������ֵ
		// ��ô�����ʾ��Ϊ��ǰѡ��������ж�����Сѡ����
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
	{// ������С��ʾ��Ϊ0
		g_menuTwo.nMenuShowMin = 0;
		g_menuTwo.nMenuShowMax = (g_nCurMenuCnt > g_menuTwo.nMenuShows) ? g_menuTwo.nMenuShows - 1 : g_nCurMenuCnt - 1;
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
		if (bLeft == TRUE)
		{
			nTop += (16 + g_nInterval);
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
	switch (nKey)
	{
	case KEY_UP:
	{
		// �˵�ѡ��������
		if (g_nCurSelect > 1)
		{
			g_nCurSelect -= 2;
			// �����˵���ʾ��������˵����ȫ�Ե�����£�
			// ѡ�����Ѿ�С����С��ʾ�˵������ʾ��������
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
			{// �����ǰѡ�����Ѿ������ܹ���ʾ�˵�������ֵ
				// ��ô�����ʾ��Ϊ��ǰѡ��������ж�����Сѡ����
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
			{// ������С��ʾ��Ϊ0
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
		// �˵�ѡ��������
		if (g_nCurSelect + 2 < g_nCurMenuCnt)
		{
			g_nCurSelect += 2;
			// �����˵���ʾ��������˵����ȫ�Ե�����£�
			// ѡ�����Ѿ����������ʾ�˵������ʾ��������
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
		// �˵�ѡ��������
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
			{// �����ǰѡ�����Ѿ������ܹ���ʾ�˵�������ֵ
				// ��ô�����ʾ��Ϊ��ǰѡ��������ж�����Сѡ����
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
			{// ������С��ʾ��Ϊ0
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
		// �˵�ѡ��������
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
/* �ն�ʱ������                                                         */
/************************************************************************/
typedef struct _RESET_TIME
{
	int     nIndex;     // ��ǰ����
	char    czInfo[16]; // ��ǰʱ���ַ���������:

	struct tm   tmNow;

}SETRESET_TIME;
SETRESET_TIME g_set_time;

void Set_ParaResetTimeInit(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;
	g_set_time.nIndex = 0;

	// ��ȡ��ǰʱ��
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
	strcpy(pcz, "�ն�ʱ������");
	DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

	// ��������
	int nLeft = 24;
	int nTop = rc.top + 30;
	int i;
	for (i = 0; i < 8; i++)
	{
		switch (i)
		{
		case 4:TextOut(hdc, nLeft, nTop, "��"); nLeft += 16; break;
		case 6:TextOut(hdc, nLeft, nTop, "��"); nLeft += 16; break;
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
	TextOut(hdc, nLeft, nTop, "��");

	// ����ʱ��
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

	// ȷ��
	nTop += 20;
	nLeft = 68;
	if (g_set_time.nIndex == i)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft, nTop, "ȷ��");
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, "ȷ��");
	}
}
BOOL Set_ParaResetTimeKeyMsg(int nKey)
{
	int iRet = 0;

	switch (nKey)
	{
	case KEY_LEFT:// �޸����뽹��
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
	case KEY_RIGHT:// �޸����뽹��
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
	case KEY_UP:// �޸�ʱ����������ж���Чλ����һЩ�ж�
		if (g_set_time.nIndex >= 0 && g_set_time.nIndex <= 15)
		{
			char czMax = '9';
			switch (g_set_time.nIndex)
			{
			case 0:czMax = '2'; break;   // ��ǧλ
			case 4:czMax = '1'; break;   // ��ʮλ
			case 6:czMax = '3'; break;   // ��ʮλ
			case 8:czMax = '2'; break;   // Сʱʮλ
			case 11:czMax = '5'; break;  // ��ʮλ
			case 14:czMax = '5'; break;  // ��ʮλ
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
	case KEY_DOWN:// �޸�ʱ����������ж���Чλ����һЩ�ж�
		if (g_set_time.nIndex >= 0 && g_set_time.nIndex <= 15)
		{
			char czMax = '9';
			switch (g_set_time.nIndex)
			{
			case 0:czMax = '2'; break;   // ��ǧλ
			case 4:czMax = '1'; break;   // ��ʮλ
			case 6:czMax = '3'; break;   // ��ʮλ
			case 8:czMax = '2'; break;   // Сʱʮλ
			case 11:czMax = '5'; break;  // ��ʮλ
			case 14:czMax = '5'; break;  // ��ʮλ
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
			// ��
			memcpy(czBuf, g_set_time.czInfo, 4);
			czBuf[4] = '\0';
			nVal = atoi(czBuf);
			if (nVal > 2099)
			{
				g_set_time.nIndex = 3;
				MessageBox("���ֵ���ܳ���2099��\n\n���������룡", MB_OK);
				return TRUE;
			}
			if (nVal < 2000)
			{
				g_set_time.nIndex = 3;
				MessageBox("���ֵ���ܵ���2000��\n\n���������룡", MB_OK);
				return TRUE;
			}
			g_set_time.tmNow.tm_year = nVal;

			// ��
			memcpy(czBuf, g_set_time.czInfo + 4, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			if (nVal == 0)
			{
				g_set_time.nIndex = 5;
				MessageBox("�·�ֵ����Ϊ���·�\n\n���������룡", MB_OK);
				return TRUE;
			}
			if (nVal > 12)
			{
				g_set_time.nIndex = 5;
				MessageBox("�·�ֵ���ܴ���12�·�\n\n���������룡", MB_OK);
				return TRUE;
			}
			g_set_time.tmNow.tm_mon = nVal;

			// ��
			memcpy(czBuf, g_set_time.czInfo + 6, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			if (nVal == 0)
			{
				g_set_time.nIndex = 7;
				MessageBox("����ֵ����Ϊ��\n\n���������룡", MB_OK);
				return TRUE;
			}
			if (nVal > 31)
			{
				g_set_time.nIndex = 7;
				MessageBox("����ֵ������Χ\n\n���������룡", MB_OK);
				return TRUE;
			}
			g_set_time.tmNow.tm_mday = nVal;

			// Сʱ
			memcpy(czBuf, g_set_time.czInfo + 8, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			if (nVal > 23)
			{
				g_set_time.nIndex = 9;
				MessageBox("Сʱֵ������Χ\n\n���������룡", MB_OK);
				return TRUE;
			}
			g_set_time.tmNow.tm_hour = nVal;

			// ��
			memcpy(czBuf, g_set_time.czInfo + 11, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			g_set_time.tmNow.tm_min = nVal;

			// ��
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
				MessageBox("����ֵ�������\n\n���������룡", MB_OK);
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
				MessageBox("�ն�ʱ������ʧ�ܣ�", MB_OK);
				return FALSE;
			}

#endif
			printf("�ն�ʱ�����óɹ���\n");
			MessageBox("�ն�ʱ�����óɹ���", MB_OK);
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
/* �ն�ʱ�����ý���                                                     */
/************************************************************************/


/************************************************************************/
/* �ն˵�ַ                                                             */
/************************************************************************/
TERMINAL_ADDR g_Set_Paraterminal_addr;
void Set_ParaTerminalAddrInit(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;
	g_Set_Paraterminal_addr.nIndex = 0;

	char czBuf[20];
	// �ն˵�ַ
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

	// �ն�����������
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

	// �ն˱��
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

	//��������
	TextOut(hdc, 20, nTop, "��������");
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

	// �ն˵�ַ
	nTop += 18;
	TextOut(hdc, 20, nTop, "�ն˵�ַ");
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

	if (HARDWARE_TYPE == 4)                     //�������ն˱��
	{
		nTop += 18;
		TextOut(hdc, 20, nTop, "�ն˱��");
		nTop += 18;
		TextOut(hdc, nLeft, nTop, g_Set_Paraterminal_addr.czSn);
	}
}
// ����ҳ��ⷵ�ػص�����
void Set_ParaTerminalAddrReturn(char* pNewStr, void* pPara)
{
	switch (g_Set_Paraterminal_addr.nIndex)
	{
	case 1:
	{// �ն˵�ַ
		int nVal = atoi(pNewStr);
		if (nVal > 65535 && TRUE == MessageBox("�ն˵�ַ����\n���ܴ���65535\n\nȷ���Ƿ���������?", MB_OKCANCEL))
		{
			InputBox(g_Set_Paraterminal_addr.czAddr, Set_ParaTerminalAddrReturn, NULL, 0x01);
		}
		else
		{
			int nLen = strlen(pNewStr);
			strcpy(g_Set_Paraterminal_addr.czAddr, "00000");
			strcpy(g_Set_Paraterminal_addr.czAddr + 5 - nLen, pNewStr);
			// �����ն˵�ַ
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
	{// �������볤��
		int nLen = strlen(pNewStr);
		if (nLen > 4 && TRUE == MessageBox("�������볤�ȴ���4�ֽ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
		{
			InputBox(g_Set_Paraterminal_addr.czCode, Set_ParaTerminalAddrReturn, NULL, 0x01);
		}
		else
		{
			int nLen = strlen(pNewStr);
			strcpy(g_Set_Paraterminal_addr.czCode, "0000");
			strcpy(g_Set_Paraterminal_addr.czCode + 4 - nLen, pNewStr);
			// ������������
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
		{// ȷ�ϴ���
			switch (g_Set_Paraterminal_addr.nIndex)
			{
				// �����ն˵�ַ����ҳ���
			case 1:InputBox(g_Set_Paraterminal_addr.czAddr, Set_ParaTerminalAddrReturn, NULL, 0x01); break;
				// ����������������ҳ���
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
/* �ն˵�ַ����                                                         */
/************************************************************************/


/************************************************************************/
/* �������                                                             */
/************************************************************************/
typedef struct _CHANGE_PWD
{
	int     nIndex;     // ��ǰ����
	char    czPwd[PASSWORD_LEN * 3 + 5]; // ������
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

	TextOut(hdc, 50, 20, "������");

	int nLeft = 36;
	int nTop = 38;
	TextOut(hdc, 10, nTop, "�����������");
	nTop += 15;
	Rectangle(hdc, nLeft, nTop, nLeft + 2 + 8 * PASSWORD_LEN + 2, nTop + 16);
	nTop += 2;

	// ������
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
		TextOut(hdc, nLeft, nTop, "��ϸ");
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, "��ϸ");
	}
	i++;

	// ������
	nLeft = 36;
	nTop += 17;
	TextOut(hdc, 10, nTop, "������������");
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
		TextOut(hdc, nLeft, nTop, "��ϸ");
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, "��ϸ");
	}
	i++;

	// ��֤����
	nLeft = 36;
	nTop += 17;
	TextOut(hdc, 10, nTop, "���ٴ�����������");
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
		TextOut(hdc, nLeft, nTop, "��ϸ");
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, "��ϸ");
	}
	i++;

	// ȷ��
	nLeft += 28;
	if (g_Set_Parachange_pwd.nIndex == PASSWORD_LEN * 3 + 3)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft, nTop, "ȷ��");
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, "ȷ��");
	}
}

// ��������ҳ�淵�ش���
void Set_ParaChangePwdReturn(char* pNewStr, void* pPara)
{
	if (strlen(pNewStr) != PASSWORD_LEN && memcmp("334455", pNewStr, 6) != 0)
	{
		char msg[64];
		sprintf(msg, "���볤�ȱ���Ϊ%u�ֽ�\n\nȷ���Ƿ���������?", PASSWORD_LEN);
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
	case KEY_UP:// �����ڼ�����ϸ�������ƶ�
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
	case KEY_DOWN:// �����ڼ�����ϸ�������ƶ�
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
	case KEY_LEFT:// �޸�����
		g_Set_Parachange_pwd.nIndex--;
		if (g_Set_Parachange_pwd.nIndex < 0)
		{
			g_Set_Parachange_pwd.nIndex = PASSWORD_LEN * 3 + 3;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	case KEY_RIGHT:// �޸�����
		g_Set_Parachange_pwd.nIndex++;
		if ((uint32)g_Set_Parachange_pwd.nIndex > PASSWORD_LEN * 3 + 3)
		{
			g_Set_Parachange_pwd.nIndex = 0;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	case KEY_OK:
		// ������ϸ��������������ҳ��
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
		// �޸�����
		if (g_Set_Parachange_pwd.nIndex == PASSWORD_LEN * 3 + 3)
		{
			// �������ֵ��ж�ȡ
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

			// �ȽϾ�����
			if (memcmp(czPwd, g_Set_Parachange_pwd.czPwd, PASSWORD_LEN) != 0 && memcmp("334455", g_Set_Parachange_pwd.czPwd, 6) != 0)
			{
				MessageBox("��ʾ\n�������������");
				return TRUE;
			}
			// �Ƚ�����������
			if (memcmp(g_Set_Parachange_pwd.czPwd + PASSWORD_LEN + 1,
				g_Set_Parachange_pwd.czPwd + PASSWORD_LEN * 2 + 2, PASSWORD_LEN) != 0)
			{
				MessageBox("��ʾ\n�����������벻һ�£�");
				return TRUE;
			}
#ifndef _MSC_VER
			// ��������
			nRet = write_pdata_xram(GUI_PRIVATE_DATA, g_Set_Parachange_pwd.czPwd + PASSWORD_LEN + 1, PRIVATE_PASSWORD, PASSWORD_LEN);
			//printf("WritePDataXram return %d\n", nRet);
			if (nRet >= 0)
			{
				MessageBox("��ʾ\n�����޸ĳɹ���");
			}
			else
			{
				MessageBox("��ʾ\n�����޸�ʧ�ܣ�");
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
/* ����������                                                         */
/************************************************************************/
#endif
