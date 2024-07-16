/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
  程序名称: main.h
  功    能：FKGA42显示模块框架XML解析实现文件
  开 发 人：gaoyf
  开发时间：2007-1
  
  版本历史:
  
    v1.0:
      2006-1-8, gaoyf : New file        
  
***************************************************************************
*/

#include "main.h"
#include "XMLite.h"
#include <pthread.h>
#include "macro.h"
#include "framecomm.h"

#define CONFIG_HEADER_LEN	68
#if HARDWARE_TYPE == 4
#define CONFIG_VERSION	104
#else
#define CONFIG_VERSION	6
#endif

DWORD g_dwTemplateId = 0;
extern int g_narrMenuIndex[20];
extern int g_nMenuNodes;
pthread_mutex_t		menuLock = PTHREAD_MUTEX_INITIALIZER;
/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    增加一个菜单项到菜单数据中.
	输入参数：    LPXNode pnodeMenus：菜单项的父节点
	输出参数：
	函数返回值：  int：返回增加的菜单项在数组中的索引值
	内部处理流程：
********************************************************************/
int AddMenu(int nParentIndex, char* pCaption, BOOL bMenuAggregate, 
			DWORD dwMenuID, int nLeft1, int nLeft2, int nTop, int nInterval, int nMenuMaxCnt,
			BOOL bVisible,
			PLUG_STRUCT* pPluginStruct, int nPluginCnt)
{
	MENU_INFO* pMenu = (MENU_INFO*)malloc(sizeof(MENU_INFO));
	pMenu->nParent = nParentIndex;
	pCaption[MENU_CAPTION_LEN-1] = '\0';
	strcpy(pMenu->czCaption, pCaption);
	
	pMenu->bMenuAggregate = bMenuAggregate;
	pMenu->dwMenuID = dwMenuID;
	pMenu->nLeft1 = nLeft1;
	pMenu->nLeft2 = nLeft2;
	pMenu->nTop = nTop;
	pMenu->nInterval = nInterval;
	pMenu->nMenuMaxCnt = nMenuMaxCnt;
	pMenu->bVisible = bVisible;

	pMenu->pPlugStruct = pPluginStruct;
	pMenu->nPlugCnt = nPluginCnt;
	pthread_mutex_lock(&menuLock);
	g_menuMain[g_nMenus] = pMenu;
	g_nMenus++;
	pthread_mutex_unlock(&menuLock);
	return g_nMenus-1;
}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    增加一个菜单项.
	输入参数：    LPXNode pnodeMenus：菜单项的父节点
	输出参数：
	函数返回值：  BOOL：成功返回TRUE，失败返回FALSE
	内部处理流程：
********************************************************************/
BOOL AddMenus(LPXNode pnodeMenus, int nParentId)
{
	ASSERT(pnodeMenus != NULL);

	int nCnt = pnodeMenus->GetChildCount();
	int nChildCnt;
	LPXNode pnodeMenu;
	LPXNode pnodePlugin;
	LPCTSTR pVal;
	LPCTSTR pName;
	int nIndex;
	PLUG_STRUCT* pPluginStruct;
	for(int i = 0; i < nCnt; i ++)
	{
		pnodeMenu = pnodeMenus->GetChild(i);
		pName = pnodeMenu->GetAttrValue("Name");
		pVal = pnodeMenu->GetAttrValue("MenuId");

		BOOL bVisible = TRUE;
		LPCTSTR pVisible = pnodeMenu->GetAttrValue("Visible");
		if(pVisible != NULL && strlen(pVisible) > 0)
		{
			bVisible = (atoi(pVisible)==1)?TRUE:FALSE;
		}

		if(pVal == NULL)
		{// 没有PluginId,表示为最终菜单项，
			nChildCnt = pnodeMenu->GetChildCount();
			if(nChildCnt <= 0)
			{
				char czBuf[20];
				sprintf(czBuf, "parentid is %d\n", nParentId);
				printf(czBuf);
			}
//			ASSERT(nChildCnt > 0);
			pPluginStruct = (PLUG_STRUCT*)malloc(sizeof(PLUG_STRUCT)*nChildCnt);
			nIndex = 0;
			for(int j = 0; j < nChildCnt; j ++)
			{
				pnodePlugin = pnodeMenu->GetChild(j);
				ASSERT(pnodePlugin != NULL);
				pVal = pnodePlugin->GetAttrValue("Id");
				ASSERT(pVal != NULL);
				if(pVal == NULL || strlen(pVal) <= 0)
				{
					continue;
				}
				pPluginStruct[nIndex].dwPlugin = atoi(pVal);
				pVal = pnodePlugin->GetAttrValue("Name");
				if(pVal != NULL && strlen(pVal) > 0)
				{
					strcpy(pPluginStruct[nIndex].czCaption, pVal);
				}
				else
				{
					strcpy(pPluginStruct[nIndex].czCaption, "");
				}
				nIndex ++;
			}
			printf("%s:添加最终菜单项：%s nParentId%d, bVisible%d, nIndex%d\n",__FUNCTION__,pName, nParentId, bVisible, nIndex);
            AddMenu(nParentId, (char*)pName, FALSE, 0, 0, 0, 0, 0, nChildCnt, bVisible,
				pPluginStruct, nIndex);
		}
		else
		{
			LPCTSTR pTmp = pnodeMenu->GetAttrValue("Left1");
			int nLeft1 = (pTmp != NULL&&strlen(pTmp)>0)?atoi(pTmp):10;
			pTmp = pnodeMenu->GetAttrValue("Left2");
			int nLeft2 = (pTmp != NULL&&strlen(pTmp)>0)?atoi(pTmp):90;
			pTmp = pnodeMenu->GetAttrValue("Top");
			int nTop = (pTmp != NULL&&strlen(pTmp)>0)?atoi(pTmp):32;
			pTmp = pnodeMenu->GetAttrValue("MenuInterval");
			int nInterval = (pTmp != NULL&&strlen(pTmp)>0)?atoi(pTmp):0;
			pTmp = pnodeMenu->GetAttrValue("MenuMaxCnt");
			int nMenuMaxCnt = (pTmp != NULL&&strlen(pTmp)>0)?atoi(pTmp):8;
			int Id = AddMenu(nParentId, (char*)pName, TRUE, atoi(pVal), nLeft1, nLeft2, nTop, nInterval, nMenuMaxCnt, bVisible,
				NULL, 1);
            // 进入递归
            printf("%s:添加非最终菜单项：%s nParentId %d ID %d bVisible%d\n",__FUNCTION__,pName, nParentId, Id, bVisible);
			AddMenus(pnodeMenu, Id);
		}
	}
	return TRUE;
}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    读取XML配置中的沦陷页面信息.
	输入参数：    LPXNode pnodeTrunDisps：轮显页面的父节点
	输出参数：
	函数返回值：  BOOL：成功返回TRUE，失败返回FALSE
	内部处理流程：
********************************************************************/
BOOL ReadTrunDisp(LPXNode pnodeTrunDisps)
{
	// 如果在私有文件中已经存在轮显页面，则从私有文件中读取
	// 否则从xml文件中读取，并写道私有文件中，以便下次从私有文件读取
	g_guiMain.nTrunDispCnt = 0;
	if(pnodeTrunDisps == NULL)
	{
		return TRUE;
	}
	LPCTSTR pVal = pnodeTrunDisps->GetAttrValue("Intervel");
	if(pVal != NULL)
	{
		g_guiMain.nInterval = atoi(pVal);
	}
	int nCnt = pnodeTrunDisps->GetChildCount();
	if(nCnt > MAX_TRUNDISP_CNT)nCnt=MAX_TRUNDISP_CNT;
	g_nTotalTrunDispCnt = nCnt;
	LPXNode pnodeDisp;
	
	PLUGIN_INFO* pPlugin;
	int i;
	for(i = 0; i < nCnt; i ++)
	{
		pnodeDisp = pnodeTrunDisps->GetChild(i);
		ASSERT(pnodeDisp != NULL);
		pVal = pnodeDisp->GetAttrValue("Id");
		ASSERT(pVal != NULL);
		g_nTrunDisp[i] = atoi(pVal);
		pPlugin = GetPlugIn(g_nTrunDisp[i]);
		ASSERT(pPlugin != NULL);
		if(pPlugin != NULL)
		{
			g_pluginTrunDisp[g_guiMain.nTrunDispCnt] = pPlugin;
			g_guiMain.nTrunDispCnt ++;
		}
	}
	return TRUE;
}
extern int  checkRegistAppCnt();

int WriteXML(char** appName,XNode *pnodeRoot)
{
	int appRegistCnt = checkRegistAppCnt();
	int i;
	char* appTemp = NULL;
	if(appName == NULL || pnodeRoot == NULL)
	{
		return -1;
	}
	LPXNode pnodeChild = pnodeRoot->GetChild("Menus");
	if(pnodeChild != NULL)
	{
		for(i= 0; i< appRegistCnt; i++)
		{
			appTemp = appName[i];
			if(appTemp != NULL && strlen(appTemp) != 0)
			{
				printf("WriteXML:appTemp = %s\n",appTemp);
				LPXNode pnodeMenu = pnodeChild->CreateNode("Menu");
		
				LPXNode pnodeChild_menu = pnodeChild->AppendChild(pnodeMenu);
				if(pnodeChild_menu == NULL)
					return -1;
				//pnodeChild_menu->CreateAttr("Name", appName);
				pnodeChild_menu->AppendAttr("Name", appTemp);
				if(pnodeChild->GetChild("Menu"))
				{
					GUI_FMT_TRACE("WriteXML:pnodeChild->GetAttrValue: %s\n",pnodeChild_menu->GetAttrValue("Name"));
				}	
			}
		}

	}
	return TRUE;
}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    读取XML配置信息.
	输入参数：    
	输出参数：
	函数返回值：  BOOL：成功返回TRUE，失败返回FALSE
	内部处理流程：
********************************************************************/
BOOL ReadXML(char** appName)
{
    // 打开文件
	FILE* pFile = fopen("/usr/local/extapps/desktopGui/bin/cfg/fkgaconfig.xml", "r");
	if(pFile == NULL)
	{
        ASSERT(FALSE);
		return FALSE;
	}
    // 分配内存
	fseek(pFile, 0, SEEK_END);
	long lLength = ftell(pFile);
	printf("xml length is %ld\n", lLength);
	char* pXml = (char*)malloc(lLength+10000);

    // 读取xml
	fseek(pFile, 0, SEEK_SET);
	fread(pXml, 1, lLength, pFile);
    // 关闭文件
	fclose(pFile);
	pXml[lLength] = '\0';

    // 解析xml
	XNode nodeRoot;
	nodeRoot.Load(pXml);
	printf("load xml success!\n");
	if(appName != NULL)
	{
		WriteXML(appName,&nodeRoot);
	}
	// 读取总体参数
	LPCTSTR pVal = nodeRoot.GetAttrValue("Width");
	if(pVal != NULL)
	{
		g_guiComm.nWidth = atoi(pVal);
	}
	pVal = nodeRoot.GetAttrValue("Height");
	if(pVal != NULL)
	{
		g_guiComm.nHeight = atoi(pVal);
	}

    pVal = nodeRoot.GetAttrValue("Template");
    if(pVal != NULL)
	{
		g_dwTemplateId = atoi(pVal);
        g_guiMain.plugTemplate = GetPlugIn(g_dwTemplateId);
	}

	pVal = nodeRoot.GetAttrValue("LightonTime");
	if(pVal != NULL)
	{
		g_guiMain.nLightonTimeMax = atoi(pVal);
	}
	
	pVal = nodeRoot.GetAttrValue("TrunDispTime");
	if(pVal != NULL)
	{
		g_guiMain.nTrunDispTimeMax = atoi(pVal);
	}

	// 动态读取菜单项
	LPXNode pnodeChild = nodeRoot.GetChild("Menus");
	pVal = nodeRoot.GetAttrValue("MenuId");
	if(pVal != NULL)
    {
        g_guiMain.nMenuRootId = atoi(pVal);// 顶层菜单
        GUI_FMT_TRACE("-------------------g_guiMain.nMenuRootId--------- = %d \n",g_guiMain.nMenuRootId);
    }
	LPCTSTR pTmp = nodeRoot.GetAttrValue("Left1");
	g_guiMain.nLeft1 = (pTmp != NULL&&strlen(pTmp)>0)?atoi(pTmp):10;
	pTmp = nodeRoot.GetAttrValue("Left2");
	g_guiMain.nLeft2 = (pTmp != NULL&&strlen(pTmp)>0)?atoi(pTmp):90;
	pTmp = nodeRoot.GetAttrValue("Top");
	g_guiMain.nTop = (pTmp != NULL&&strlen(pTmp)>0)?atoi(pTmp):32;
	pTmp = nodeRoot.GetAttrValue("MenuInterval");
	g_guiMain.nMenuInterval = (pTmp != NULL&&strlen(pTmp)>0)?atoi(pTmp):0;
	pTmp = nodeRoot.GetAttrValue("MenuMaxCnt");
	g_guiMain.nMenuMaxCnt = (pTmp != NULL&&strlen(pTmp)>0)?atoi(pTmp):8;
	if(pnodeChild != NULL)
	{
		AddMenus(pnodeChild, -1);
	}
	// 读取轮显画面集合
	pnodeChild = nodeRoot.GetChild("TrunDisp");
	ReadTrunDisp(pnodeChild);
	
	free(pXml);
	return TRUE;
}

extern void WriteTrunDisp();
BOOL RestoreConfigFromXml(char** appName)
{
	// 释放数据
	// 释放菜单资源
    MENU_INFO* pMenu;
	int i;
    for(i = 0; i < g_nMenus; i ++)
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
	g_guiMain.nTrunDispCnt = 0;

	g_narrMenuIndex[0] = 0;
	g_nMenuNodes = 1;

	// 读取xml数据
	printf("Begin to ReadXML.\n");
	if(TRUE != ReadXML(appName))
	{
		return FALSE;
	}

	int nRet  = -1;

	// 保存数据到私有文件和数据字典中
	// 一次写多个菜单数据，避免写私有文件次数太多
	char czBuf[2048];
	int nLen = 0;
	int nCurLen;
	int nTotalLen = 0;
	// 写菜单信息
	char* pBuf = czBuf;
	int nPDataOff = CONFIG_HEADER_LEN;
	for(i = 0; i < g_nMenus; i++)
	{
		pMenu = g_menuMain[i];
		if(pMenu->bMenuAggregate == FALSE)
		{
			nCurLen = sizeof(MENU_INFO)+sizeof(PLUG_STRUCT)*pMenu->nPlugCnt;
		}
		else
		{
			nCurLen = sizeof(MENU_INFO);
		}
		if(nLen+nCurLen > 2048)
		{// 把当前数据写到私有文件中
			nRet = write_pdata_xram(GUI_CONFIG_DATA, czBuf, nPDataOff, nLen);
			if(nRet < 0)
			{
				printf("write error!\n");
				return FALSE;
				}
			nPDataOff += nLen;
			nLen = 0;
			pBuf = czBuf;
		}

		memcpy(pBuf, pMenu, sizeof(MENU_INFO));
		if(pMenu->bMenuAggregate == FALSE)
		{
			memcpy(pBuf+sizeof(MENU_INFO), pMenu->pPlugStruct, sizeof(PLUG_STRUCT)*pMenu->nPlugCnt);
		}
		nLen += nCurLen;
		pBuf += nCurLen;
		nTotalLen += nCurLen;
	}
	if(nLen > 0)
	{
		nRet = write_pdata_xram(GUI_CONFIG_DATA, czBuf, nPDataOff, nLen);
		if(nRet < 0)
		{
			printf("write error!");
			return FALSE;
			}
	}

	// 写全局变量参数
	pBuf = czBuf;
	*pBuf = CONFIG_VERSION;pBuf++;
	int32tochar(g_nMenus, pBuf);pBuf+=4;
	int32tochar(nTotalLen, pBuf);pBuf+=4;
	int32tochar(g_guiComm.nWidth, pBuf);pBuf+=4;
	int32tochar(g_guiComm.nHeight, pBuf);pBuf+=4;
	int32tochar(g_dwTemplateId, pBuf);pBuf+=4;
	int32tochar(g_guiMain.nLightonTimeMax, pBuf);pBuf+=4;
	int32tochar(g_guiMain.nMenuRootId, pBuf);pBuf+=4;

	int32tochar(g_guiMain.nLeft1, pBuf);pBuf+=4;
	int32tochar(g_guiMain.nLeft2, pBuf);pBuf+=4;
	int32tochar(g_guiMain.nTop, pBuf);pBuf+=4;
	int32tochar(g_guiMain.nMenuInterval, pBuf);pBuf+=4;
	int32tochar(g_guiMain.nMenuMaxCnt, pBuf);pBuf+=4;
	nRet = write_pdata_xram(GUI_CONFIG_DATA, czBuf, 0, CONFIG_HEADER_LEN);
	if(nRet < 0)
	{
		printf("write error!");
		return FALSE;
		}

	// 保存轮显信息
	WriteTrunDisp();
	return TRUE;
}

BOOL ReadConfig(BOOL bAll)
{
	// 释放数据
	// 释放菜单资源
    MENU_INFO* pMenu;
	int i;
    for(i = 0; i < g_nMenus; i ++)
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
	g_guiMain.nTrunDispCnt = 0;
	
	g_narrMenuIndex[0] = 0;
	g_nMenuNodes = 1;

	char czBuf[150];
    //int nRet = ReadXPara(TMN_RUN_PARA, Turn_Menu_Num_Time, czBuf, 0);
    int nRet = read_pdata_xram(GUI_PARA, czBuf, Turn_Num_Time, Turn_Num_Time_Len);
    if(nRet < 0 || (unsigned char)czBuf[0] == NONE_CHAR)
    {// 认为如果能从私有文件中读取,即能从数据字典中读取,如果不行,则用缺省
        return FALSE;
    }
    else
    {// 不读了,反正每次进入轮显时,还要重新读取
        g_guiMain.nTrunDispTimeMax = (unsigned char)czBuf[1];
        g_guiMain.nInterval = (unsigned char)czBuf[2];
    }

	nRet = read_pdata_xram(GUI_CONFIG_DATA, czBuf, 0, CONFIG_HEADER_LEN);
	if(nRet < 0 || (unsigned char)czBuf[0] == NONE_CHAR)
	{
		return FALSE;
	}
	
	char* pBuf = czBuf;
	if(*pBuf != CONFIG_VERSION)
	{// 如果版本不对，也从xml中读取
		return FALSE;
	}
	pBuf ++;
	int nMenuCnt = chartoint32(pBuf); pBuf+=4;
	int nTotalCnt = chartoint32(pBuf); pBuf+=4;
	// 长宽
	g_guiComm.nWidth = chartoint32(pBuf); pBuf+=4;
	g_guiComm.nHeight = chartoint32(pBuf); pBuf+=4;
	// 背景库ID
	DWORD dwTemplateId = chartoint32(pBuf); pBuf+=4;
	g_guiMain.plugTemplate = GetPlugIn(dwTemplateId);
#ifdef AREA_HUBEI
    g_guiMain.nLightonTimeMax = 90; pBuf+=4;   //湖北默认设置为90S
#else
	// 背光灯最大时间
	g_guiMain.nLightonTimeMax = chartoint32(pBuf); pBuf+=4;
#endif
	// 主菜单ID
	g_guiMain.nMenuRootId = chartoint32(pBuf); pBuf+=4;

	g_guiMain.nLeft1 = chartoint32(pBuf); pBuf+=4;
	g_guiMain.nLeft2 = chartoint32(pBuf); pBuf+=4;
	g_guiMain.nTop = chartoint32(pBuf); pBuf+=4;
	g_guiMain.nMenuInterval = chartoint32(pBuf); pBuf+=4;
	g_guiMain.nMenuMaxCnt = chartoint32(pBuf); pBuf+=4;

	char* pData = (char*)malloc(nTotalCnt+10);
	nRet = read_pdata_xram(GUI_CONFIG_DATA, pData, CONFIG_HEADER_LEN, nTotalCnt);
	if(nRet < 0)
	{
		return FALSE;
	}

	char* pDataTmp = pData;
	g_nMenus = 0;
	for(i = 0; i < nMenuCnt; i ++)
	{
		pMenu = (MENU_INFO*)malloc(sizeof(MENU_INFO));
		memcpy(pMenu, pDataTmp, sizeof(MENU_INFO));
		pDataTmp += sizeof(MENU_INFO);

//		if(bAll == TRUE && pMenu->bVisible == FALSE)
        if(bAll == TRUE || pMenu->bVisible == TRUE)
		{
			g_menuMain[g_nMenus] = pMenu;
			g_nMenus ++;
			if(pMenu->bMenuAggregate == FALSE)
			{
				pMenu->pPlugStruct = (PLUG_STRUCT*)malloc(sizeof(PLUG_STRUCT)*pMenu->nPlugCnt);
				memcpy(pMenu->pPlugStruct, pDataTmp, sizeof(PLUG_STRUCT)*pMenu->nPlugCnt);
				pDataTmp += sizeof(PLUG_STRUCT)*pMenu->nPlugCnt;
			}
		}
//        else if(bAll == FALSE && pMenu->bVisible == TRUE)
//		{
//			g_menuMain[g_nMenus] = pMenu;
//			g_nMenus ++;
//			if(pMenu->bMenuAggregate == FALSE)
//			{
//				pMenu->pPlugStruct = (PLUG_STRUCT*)malloc(sizeof(PLUG_STRUCT)*pMenu->nPlugCnt);
//				memcpy(pMenu->pPlugStruct, pDataTmp, sizeof(PLUG_STRUCT)*pMenu->nPlugCnt);
//				pDataTmp += sizeof(PLUG_STRUCT)*pMenu->nPlugCnt;
//			}
//		}
		else
		{
			if(pMenu->bMenuAggregate == FALSE)
			{
				pDataTmp += sizeof(PLUG_STRUCT)*pMenu->nPlugCnt;
			}
			free(pMenu);
		}
	}
	free(pData);
    return TRUE;
}
