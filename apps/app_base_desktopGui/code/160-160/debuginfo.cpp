/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
  程序名称: debuginfo.cpp
  功    能：完成调试页面库
  开 发 人：gaoyf
  开发时间：2007-1
  
  版本历史:
  
    v1.0:
      2006-1-20, gaoyf : New file    

  包含页面库：
    自定义调试页面  0x000d0000
  
***************************************************************************
*/

#include "miniguidef.h"

void CustomDebugInit(DWORD dwPluginId);
void CustomDebugDraw(HDC hdc);
BOOL CustomDebugKeyMsg(int nKey);
PLUGIN_INFO g_layerCustomDebug = {
	0x000d0000/*id*/, 
	CustomDebugInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	CustomDebugDraw/*Draw*/,
	NULL/*Timer*/,
	CustomDebugKeyMsg/*keymsg*/,
	NULL/*ipc*/
};
#define MAX_CUSTOM_DEBUG_MSG    300
typedef struct _CUSTOM_DEBUG
{
    char* pMsg[MAX_CUSTOM_DEBUG_MSG];   // 自定义消息数组
	int   nTotalLine;       // 消息对应的总数行
	int   nCurLine;         // 当前显示的起始项
    int   nSelLine;         // 当前选择项
    int   nShowLine;        // 页面中能够显示的行数
}CUSTOM_DEBUG;
CUSTOM_DEBUG g_custom_debug = {NULL, 0, 0, 0, 0};
/*********************************************************************
	所属功能单元：调试页面库
	功能描述：    用于弹出自定义调试信息
	输入参数：    char* pMsg：自定义调试信息
	输出参数：
	函数返回值：
	内部处理流程：判断如果溢出，删除最后一项
                  然后总体往后偏移一项
                  增加新项到第一项位置
********************************************************************/
void AddNewMsg(char* pMsg[], int nLine)
{
	int i;
	if(g_custom_debug.nTotalLine + nLine > MAX_CUSTOM_DEBUG_MSG)
	{
		for(i = MAX_CUSTOM_DEBUG_MSG-nLine; i < g_custom_debug.nTotalLine; i ++)
		{
			free(g_custom_debug.pMsg[i]);
			g_custom_debug.nTotalLine --;
		}
	}

	char* pNew[MAX_CUSTOM_DEBUG_MSG];
	memcpy(pNew, g_custom_debug.pMsg, sizeof(char*)*MAX_CUSTOM_DEBUG_MSG);
	memcpy(&g_custom_debug.pMsg[nLine], pNew, sizeof(char*)*(MAX_CUSTOM_DEBUG_MSG-nLine));
	memcpy(g_custom_debug.pMsg, pMsg, sizeof(char*)*nLine);
	g_custom_debug.nTotalLine += nLine;
	/*char* pBuf;
	for(i = 0; i < g_custom_debug.nTotalLine; i ++)
	{
		pBuf = g_custom_debug.pMsg[i];
		printf(pBuf);
		printf("\n");
	}*/
}
void PopCustomDebug(char* pMsg)
{
	int nHeight = g_guiComm.nHeight-34;// 画面库中间区域高度
    g_custom_debug.nShowLine = nHeight/13;
    int nLineChar = (g_guiComm.nWidth-6)/6;

	char* pNew[MAX_CUSTOM_DEBUG_MSG];
	int nNewLine = 0;
	char* pBuf;

	int nLen = strlen(pMsg);
	int nCurIndex = 0;
	int i;
	int nDouble;
	int nLine;
	while(nCurIndex+1 < nLen)
	{
		if(nLen-nCurIndex > nLineChar)
		{
			nLine = nLineChar;
			nDouble = 0;
			for(i = nCurIndex+nLineChar-1; i >= nCurIndex; i --)
			{
				if((pMsg[i]&0x80) == 0)
				{
					break;
				}
				nDouble ++;
			}
			if((nDouble&0x01) == 0x01)
			{
				nLine --;
			}
		}
		else
		{
			nLine = nLen-nCurIndex;
		}
		pBuf = (char*)malloc(nLine+1);
		pNew[nNewLine] = pBuf;
		memcpy(pBuf, &pMsg[nCurIndex], nLine);
		pBuf[nLine] = '\0';
		pBuf = pNew[nNewLine];
		printf(pBuf);
		nNewLine ++;

		nCurIndex += nLine;
	}

	AddNewMsg(pNew, nNewLine);
    // 弹出该页面库
    PopLayer(g_layerCustomDebug.dwId);
}
/*********************************************************************
	所属功能单元：调试页面库--自定义调试
	功能描述：    调试页面库初始化函数
	输入参数：    DWORD dwPluginId：页面库ID
	输出参数：
	函数返回值：
	内部处理流程：主要完成自定义字符串的总体行数判定
********************************************************************/
void CustomDebugInit(DWORD dwPluginId)
{
    g_custom_debug.nSelLine = 0;
    g_custom_debug.nCurLine = 0;
}
/*********************************************************************
	所属功能单元：调试页面库--自定义调试
	功能描述：    调试页面库绘制函数
	输入参数：    HDC hdc：绘制上下文句柄
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
void CustomDebugDraw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT

    int nTop = 16;
	
	char* pMsg;
	int nMax = ((g_custom_debug.nCurLine+g_custom_debug.nShowLine)>g_custom_debug.nTotalLine)
		?g_custom_debug.nTotalLine:(g_custom_debug.nCurLine+g_custom_debug.nShowLine);
    for(int i = g_custom_debug.nCurLine; i < nMax; i ++)
    {
		pMsg = g_custom_debug.pMsg[i];
		if(i == g_custom_debug.nSelLine)
        {
            ITEM_SELECT;
			TextOut(hdc, 1, nTop, pMsg);
			ITEM_NOT_SELECT;
        }
		else
		{
			TextOut(hdc, 1, nTop, pMsg);
		}
		nTop += 13;
	}
}
/*********************************************************************
	所属功能单元：调试页面库--自定义调试
	功能描述：    调试页面库按键处理消息
	输入参数：    int nKey：按键消息
	输出参数：
	函数返回值：  BOOL：TRUE表示页面库已完成处理，FALSE表示执行缺省处理
	内部处理流程：
********************************************************************/
BOOL CustomDebugKeyMsg(int nKey)
{
    switch(nKey)
    {
    case KEY_UP:
        {
			g_custom_debug.nSelLine --;
			if(g_custom_debug.nSelLine < 0)
			{
				g_custom_debug.nSelLine = 0;
			}
			if(g_custom_debug.nSelLine < g_custom_debug.nCurLine)
			{
				g_custom_debug.nCurLine = g_custom_debug.nSelLine;
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
    case KEY_DOWN:
        {
			g_custom_debug.nSelLine ++;
			if(g_custom_debug.nSelLine > g_custom_debug.nTotalLine-1)
			{
				g_custom_debug.nSelLine = g_custom_debug.nTotalLine-1;
			}
			if(g_custom_debug.nSelLine >= g_custom_debug.nCurLine+g_custom_debug.nShowLine)
			{
				g_custom_debug.nCurLine = g_custom_debug.nSelLine-g_custom_debug.nShowLine+1;
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
	}
	return FALSE;
}


