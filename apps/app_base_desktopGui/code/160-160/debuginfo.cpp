/*
***************************************************************************
  Copyright (c) 2006-2007,�Ͼ��������������������ι�˾
             All rights reserved.
  ��������: debuginfo.cpp
  ��    �ܣ���ɵ���ҳ���
  �� �� �ˣ�gaoyf
  ����ʱ�䣺2007-1
  
  �汾��ʷ:
  
    v1.0:
      2006-1-20, gaoyf : New file    

  ����ҳ��⣺
    �Զ������ҳ��  0x000d0000
  
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
    char* pMsg[MAX_CUSTOM_DEBUG_MSG];   // �Զ�����Ϣ����
	int   nTotalLine;       // ��Ϣ��Ӧ��������
	int   nCurLine;         // ��ǰ��ʾ����ʼ��
    int   nSelLine;         // ��ǰѡ����
    int   nShowLine;        // ҳ�����ܹ���ʾ������
}CUSTOM_DEBUG;
CUSTOM_DEBUG g_custom_debug = {NULL, 0, 0, 0, 0};
/*********************************************************************
	�������ܵ�Ԫ������ҳ���
	����������    ���ڵ����Զ��������Ϣ
	���������    char* pMsg���Զ��������Ϣ
	���������
	��������ֵ��
	�ڲ��������̣��ж���������ɾ�����һ��
                  Ȼ����������ƫ��һ��
                  ���������һ��λ��
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
	int nHeight = g_guiComm.nHeight-34;// ������м�����߶�
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
    // ������ҳ���
    PopLayer(g_layerCustomDebug.dwId);
}
/*********************************************************************
	�������ܵ�Ԫ������ҳ���--�Զ������
	����������    ����ҳ����ʼ������
	���������    DWORD dwPluginId��ҳ���ID
	���������
	��������ֵ��
	�ڲ��������̣���Ҫ����Զ����ַ��������������ж�
********************************************************************/
void CustomDebugInit(DWORD dwPluginId)
{
    g_custom_debug.nSelLine = 0;
    g_custom_debug.nCurLine = 0;
}
/*********************************************************************
	�������ܵ�Ԫ������ҳ���--�Զ������
	����������    ����ҳ�����ƺ���
	���������    HDC hdc�����������ľ��
	���������
	��������ֵ��
	�ڲ��������̣�
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
	�������ܵ�Ԫ������ҳ���--�Զ������
	����������    ����ҳ��ⰴ��������Ϣ
	���������    int nKey��������Ϣ
	���������
	��������ֵ��  BOOL��TRUE��ʾҳ�������ɴ���FALSE��ʾִ��ȱʡ����
	�ڲ��������̣�
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


