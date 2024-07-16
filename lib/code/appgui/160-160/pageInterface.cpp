
#include "pageInterface.h"
#include "miniguidef2.h"
//#include "main.h"

PLUGIN_INFO** 	pG_plugin = NULL;
uint16			pluginCnt = 0;
extern IP_CONFIG g_ip_config;
extern void CancelKeyDraw(HDC hdc, int x, int y, int w, int h);
extern void IpConfigReturn(char* pNewStr, void* pPara);
extern void SetIPConfigInfo();
extern void MenuDraw(HDC hdc);
extern BOOL MenuKeyMsg(int nKey);
extern void InitMenu(DWORD dwPluginId);
extern void CancelMenu();


/**********************************************************************
* @brief     ：APP页面库入口注册
* @param[in] ：PLUGIN_INFO** pg_plugin       页面库指针
               uint16 cnt                   页面库数量
* @param[out]：   
* @return    ：1 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2021-2-4
* @Update    :
**********************************************************************/
int AppPluginReg(PLUGIN_INFO** pg_plugin, uint16 cnt)
{
	pG_plugin = pg_plugin;
	pluginCnt = cnt;
	return 1;
}

GUI_COMM * AppChkGuiComm()
{
	return &g_guiComm;
}

void AppCancelKeyDraw(HDC hdc, int x, int y, int w, int h)
{
	CancelKeyDraw(hdc, x, y, w, h);
}

BOOL AppMessageBox(const char* pczMsg)
{
	int nType = MB_OK;
	return MessageBox(pczMsg, nType);
}

void AppCancelMenu()
{
	CancelMenu();
}

void AppMenuDraw(HDC hdc)
{
	MenuDraw(hdc);
}

BOOL AppMenuKeyMsg(int nKey)
{
	return MenuKeyMsg(nKey);
}

