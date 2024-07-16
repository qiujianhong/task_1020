/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
  程序名称: configcontrol.cpp
  功    能：完成公共页面库
  开 发 人：gaoyf
  开发时间：2007-1
  
  版本历史:
  
    v1.0:
      2006-1-20, gaoyf : New file

  包含页面库：
    输入法用户界面  0x00a30000
    时间范围设置界面  0x00a40000

  
***************************************************************************
*/

#include "miniguidef.h"
#include "main.h"
#include "gui_c_prtl.h"

/************************************************************************/
/* 输入法用户界面                                                       */
/************************************************************************/
#define INPUT_STR_MAX_LEN 255
typedef struct _INPUT_INFO
{
    int     nType;      // 0:数字 1:数字 2:大写 3:小写 4:字符
    int     nTypeMask;  // 0x01:数字 0x02:数字 0x04:大写 0x08:小写 00x10:字符
    int     nIndex;     // 当前索引

    int     nTotalCnt;	// 待选择字符串的长度 根据nType变化 如strlen(g_input_1)的长度
    int     nLineCnt;	// 待选择字符串的行数
    const char*   pInputStr;	// 待选择字符串g_input_1、g_input_2等

    int     nMaxLen;	// 输入字符串的最大长度
    int     nCurLen;	// 输入字符串的当前长度
    char    czInfo[INPUT_STR_MAX_LEN + 1];  // 输入字符串

    fnRetureInput  pfnReturn;	// 该页面返回时调用的回调函数，用于传出输入数据
    void*       pPara;	// 回调函数的参数，设置回调函数时设置
}INPUT_INFO;
const char* g_input_1 = "0123456789";
const char* g_input_2 = "0123456789./-+:;";
const char* g_input_3 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ./- _|>*\":"; /* 增加空格键 */
const char* g_input_4 = "abcdefghijklmnopqrstuvwxyz./- _|>*\":"; /* 增加空格键 */
const char* g_input_5 = "`~!@#$%^&*()-+_={}[]<>|\\\"',?";
const char* g_input_6 = "0123456789ABCDEF";
INPUT_INFO g_input_info;
void GetInputInfo(int nType)
{
    switch(nType)
    {
    case 0:g_input_info.pInputStr = g_input_1;break;
    case 1:g_input_info.pInputStr = g_input_2;break;
    case 2:g_input_info.pInputStr = g_input_3;break;
    case 3:g_input_info.pInputStr = g_input_4;break;
	case 4:g_input_info.pInputStr = g_input_5;break;
	case 5:g_input_info.pInputStr = g_input_6;break;
    }
    g_input_info.nMaxLen = INPUT_STR_MAX_LEN;
}
BOOL CanInputType(int nType)
{
    switch(nType)
    {
    case 0:return (g_input_info.nTypeMask&0x01)==0x01;
    case 1:return (g_input_info.nTypeMask&0x02)==0x02;
    case 2:return (g_input_info.nTypeMask&0x04)==0x04;
    case 3:return (g_input_info.nTypeMask&0x08)==0x08;
	case 4:return (g_input_info.nTypeMask&0x10)==0x10;
	case 5:return (g_input_info.nTypeMask&0x20)==0x20;
    }
    return FALSE;
}
const char* GetInputTypeStr(int nType)
{
    switch(nType)
    {
    case 0:return "数字";
    case 1:return "数字";
    case 2:return "大写";
    case 3:return "小写";
	case 4:return "字符";
	case 5:return "HEX";
    }
    ASSERT(FALSE);
    return "";
}
// 当索引nIndex在50上时，选择焦点在输入类型上
// 当索引nIndex在51上时，选择焦点在确定上
// 索引在0到当前待选字符串长度-1时，选择分别在各个字符上
void InputDraw(HDC hdc)
{
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);

	// 根据当前类型，画字符串信息以及待选字符串的矩形框
#ifdef PRODUCT_ZCU_1    
    int nLeft = 35;
#else
    int nLeft = 2;
#endif
    int nTop = 32;
    #if MODULARIZATION_TMN_EN == 1
#ifdef PRODUCT_ZCU_1
        nTop = 18;   
#else
        nTop = 27; 
#endif
    if(g_input_info.nIndex == 49)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop+17, "退格");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop+17, "退格");
    }	
    #endif
   	if(g_input_info.nIndex == 50)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, GetInputTypeStr(g_input_info.nType));
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, GetInputTypeStr(g_input_info.nType));
    }
#ifdef PRODUCT_ZCU_1
    Rectangle(hdc, 60, 18, 155, 48);
#else
	Rectangle(hdc, 27, 26, 132, 56);
#endif
	if(strlen(g_input_info.czInfo) > 16)
	{
		char czBuf[300];
		sprintf(czBuf, g_input_info.czInfo);
		czBuf[16] = '\0';
#ifdef PRODUCT_ZCU_1
        TextOut(hdc, 62, 20, czBuf);
		TextOut(hdc, 62, 34, g_input_info.czInfo + 16);
#else
		TextOut(hdc, 32, 28, czBuf);
		TextOut(hdc, 32, 42, g_input_info.czInfo + 16);
#endif
	}
	else
	{
        #ifdef PRODUCT_ZCU_1
        TextOut(hdc, 62, 20, g_input_info.czInfo);
        #else
		TextOut(hdc, 32, 28, g_input_info.czInfo);
        #endif
	}

	// 画确定选择框
#ifdef PRODUCT_ZCU_1
    nLeft = 157;
#else
    nLeft = 134;
#endif
    if(g_input_info.nIndex == 51)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "确定");
    }
    #if MODULARIZATION_TMN_EN == 1
    if(g_input_info.nIndex == 52)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop+17, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop+17, "返回");
    }
    #endif
    if(g_input_info.nType == 0)
    {// 画十进制字符串g_input_1
#ifdef PRODUCT_ZCU_1
		nTop = 70;
		Rectangle(hdc, 60, nTop, 152, nTop+38);
#else
		nTop = 80;
        Rectangle(hdc, 34, nTop, 126, nTop+38);
#endif
        nTop += 2;
        int nIndex = 0;
        for(int i = 0; i < 2; i ++)
        {
#ifdef PRODUCT_ZCU_1            
            nLeft = 64;
#else
            nLeft = 38;
#endif
            for(int j = 0; j < g_input_info.nLineCnt; j ++)
            {
                if(g_input_info.nIndex == nIndex)
                {
                    ITEM_SELECT;
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                    ITEM_NOT_SELECT;
                }
                else
                {
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                }
                nIndex ++;
                nLeft += 20;
            }
            nTop += 18;
        }
    }
    else if(g_input_info.nType == 4)
    {// 画字符串g_input_5
#ifdef PRODUCT_ZCU_1
        nTop = 55;
        Rectangle(hdc, 43, nTop, 178, nTop+74);
#else
        nTop = 60;
        Rectangle(hdc, 14, nTop, 146, nTop + 74);
#endif
        nTop += 2;
        int nIndex = 0;
        for(int i = 0; i < 4; i ++)
        {
#ifdef PRODUCT_ZCU_1
                nLeft = 45; 
#else
                nLeft = 18; 
#endif
            for(int j = 0; j < g_input_info.nLineCnt; j ++)
            {
                if(g_input_info.nIndex == nIndex)
                {
                    ITEM_SELECT;
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                    ITEM_NOT_SELECT;
                }
                else
                {
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                }
                nIndex++;
                nLeft += 20;
            }
            nTop += 18;
        }
    }
    else if(g_input_info.nType == 1 || g_input_info.nType == 5)
    {// 画16进制字符串g_input_2
#ifdef PRODUCT_ZCU_1
    nTop  = 52;
    Rectangle(hdc, 72, nTop, 144, nTop+74);
#else
    nTop = 60;
    Rectangle(hdc, 44, nTop, 116, nTop+74);
#endif
        nTop += 2;
        int nIndex = 0;
        for(int i = 0; i < 4; i ++)
        {
#ifdef PRODUCT_ZCU_1
            nLeft = 74;
#else
			nLeft = 48;
#endif
            for(int j = 0; j < g_input_info.nLineCnt; j ++)
            {
                if(g_input_info.nIndex == nIndex)
                {
                    ITEM_SELECT;
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                    ITEM_NOT_SELECT;
                }
                else
                {
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                }
                nIndex ++;
                nLeft += 20;
            }
            nTop += 18;
        }
    }
    else
    {// 画大小写字符串信息
#ifdef PRODUCT_ZCU_1
            nTop = 55;
            Rectangle(hdc, 45, nTop, 175, nTop+74);
#else
            nTop = 60;
            Rectangle(hdc, 14, nTop, 146, nTop+74);
#endif    
        nTop += 2;
        int nIndex = 0;
        for(int i = 0; i < 4; i ++)
        {
#ifdef PRODUCT_ZCU_1
            nLeft = 51; 
#else
            nLeft = 21; 
#endif
            for(int j = 0; j < g_input_info.nLineCnt; j ++)
            {
                if(g_input_info.nIndex == nIndex)
                {
                    ITEM_SELECT;
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                    ITEM_NOT_SELECT;
                }
                else
                {
                    TextOutLen(hdc, nLeft, nTop, (const char*)&g_input_info.pInputStr[nIndex], 1);
                }
                nIndex ++;
                nLeft += 14;
            }
            nTop += 18;
        }
    }
}
BOOL InputKeyMsg(int nKey)
{
    switch(nKey)
    {
    case KEY_LEFT:
        if(g_input_info.nIndex < g_input_info.nTotalCnt)
        {
            if(g_input_info.nIndex%g_input_info.nLineCnt == 0)
            {
                g_input_info.nIndex += (g_input_info.nLineCnt-1);
            }
            else
            {
                g_input_info.nIndex --;
            }
        }
        else if(g_input_info.nIndex == 51)
        {
            g_input_info.nIndex --;
        }
		#if MODULARIZATION_TMN_EN == 1
        else if(g_input_info.nIndex == 52)
        {
            g_input_info.nIndex = 49;
        }
		#endif
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        if(g_input_info.nIndex < g_input_info.nTotalCnt)
        {
            if((g_input_info.nIndex+1)%g_input_info.nLineCnt == 0)
            {
                g_input_info.nIndex -= (g_input_info.nLineCnt-1);
            }
            else
            {
                g_input_info.nIndex ++;
            }
        }
        else if(g_input_info.nIndex == 50)
        {
            g_input_info.nIndex ++;
        }
        #if MODULARIZATION_TMN_EN == 1
        else if(g_input_info.nIndex == 49)
        {
            g_input_info.nIndex = 52;
        }
		#endif
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_UP:
        #if MODULARIZATION_TMN_EN == 1
        if(g_input_info.nIndex < g_input_info.nTotalCnt)
        {
            if(g_input_info.nIndex >= g_input_info.nLineCnt)
            {
                g_input_info.nIndex -= g_input_info.nLineCnt;
            }
            else if(g_input_info.nIndex < (g_input_info.nLineCnt/2))
            {
                g_input_info.nIndex = 49;
            }
            else
            {
                g_input_info.nIndex = 52;
            }
        }
        else if(g_input_info.nIndex == 49)
        {
            g_input_info.nIndex = 50;
        }
        else if(g_input_info.nIndex == 52)
        {
            g_input_info.nIndex = 51;
        }
		#else
        if(g_input_info.nIndex < g_input_info.nTotalCnt)
        {
            if(g_input_info.nIndex >= g_input_info.nLineCnt)
            {
                g_input_info.nIndex -= g_input_info.nLineCnt;
            }
            else if(g_input_info.nIndex < (g_input_info.nLineCnt/2))
            {
                g_input_info.nIndex = 50;
            }
            else
            {
                g_input_info.nIndex = 51;
            }
        }
		#endif
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
    case KEY_DOWN:
        if(g_input_info.nIndex < g_input_info.nTotalCnt)
        {
            if(g_input_info.nIndex < g_input_info.nTotalCnt-g_input_info.nLineCnt)
            {
                g_input_info.nIndex += g_input_info.nLineCnt;
            }
        }
		#if MODULARIZATION_TMN_EN == 1
        else if(g_input_info.nIndex == 50)
        {
            g_input_info.nIndex = 49;
        }
		else if(g_input_info.nIndex == 49)
        {
            g_input_info.nIndex = 0;
        }
		else if(g_input_info.nIndex == 51)
        {
            g_input_info.nIndex = 52;
        }
        else if(g_input_info.nIndex == 52)
        {
            g_input_info.nIndex = g_input_info.nLineCnt-1;
        }
		#else
        else if(g_input_info.nIndex == 50)
        {
            g_input_info.nIndex = 0;
        }
        else if(g_input_info.nIndex == 51)
        {
            g_input_info.nIndex = g_input_info.nLineCnt-1;
        }
		#endif
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        {
            if(g_input_info.nIndex == 50)
            {// 转换类型
                int nType = g_input_info.nType;
				// 此处将所有的字符串类型循环一遍
				// 寻找一个合适的类型用于显示
				// 合适的类型根据InputBox输入的类型掩码
                for(int i = 0; i < 6; i ++)
                {
                    nType++;
                    if(nType > 5)nType=0;
                    if(CanInputType(nType))
                    {
						// 根据当前类型设置对应的待选字符串、长度、行数等
                        g_input_info.nType = nType;
                        switch(nType)
                        {
                        case 0:g_input_info.pInputStr = g_input_1;break;
                        case 1:g_input_info.pInputStr = g_input_2;break;
                        case 2:g_input_info.pInputStr = g_input_3;break;
                        case 3:g_input_info.pInputStr = g_input_4;break;
						case 4:g_input_info.pInputStr = g_input_5;break;
						case 5:g_input_info.pInputStr = g_input_6;break;
                        }
                        
                        g_input_info.nTotalCnt = strlen(g_input_info.pInputStr);
                        g_input_info.nLineCnt = g_input_info.nTotalCnt/4;
                        if(nType == 0)
						{
							g_input_info.nLineCnt=g_input_info.nTotalCnt/2;
						}
						/*else if(nType == 4)
						{
							g_input_info.nLineCnt=g_input_info.nTotalCnt/3;
						}*/
                        
                        UpdateWindow(g_guiComm.hMainWnd, TRUE);
                        break;
                    }
                }
            }
            else if(g_input_info.nIndex == 51)
            {// 确定返回
            	if(g_guiMain.bValidatePwd == FALSE)
        		{
        			g_guiMain.plugPop = NULL;
					g_guiComm.nCurPoint = g_guiComm.nCurPointTmp;
        		}
				else
				{
					PopLayer(0x00600000);
					g_guiMain.bValidatePwd = FALSE;
				}
                
                ASSERT(g_input_info.pfnReturn != NULL);
                if(g_input_info.pfnReturn != NULL)
                {// 调用回调函数返回字符串信息
                    g_input_info.pfnReturn(g_input_info.czInfo, g_input_info.pPara);
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            }
            #if MODULARIZATION_TMN_EN == 1
            else if(g_input_info.nIndex == 52)
            {
                ClosePopLayer();
            }
            else if(g_input_info.nIndex == 49)
            {
                if(g_input_info.nCurLen > 0)
                {
                    g_input_info.nCurLen --;
                    g_input_info.czInfo[g_input_info.nCurLen] = '\0';
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                }
            }
            #endif
            else
            {// 将当前选中的字符串添加到输入字符串中
                if(g_input_info.nCurLen < g_input_info.nMaxLen)
                {
                    g_input_info.czInfo[g_input_info.nCurLen] 
                        = g_input_info.pInputStr[g_input_info.nIndex];
                    g_input_info.nCurLen++;
                    g_input_info.czInfo[g_input_info.nCurLen] = '\0';
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                }
            }
        }
        return TRUE;
    case KEY_CANCEL:
        {// 取消退出：如果当前有输入字符串，则依次删除字符
			// 否则返回FALSE，由框架执行缺省处理，退出当前页面
            if(g_input_info.nCurLen > 0)
            {
                g_input_info.nCurLen --;
                g_input_info.czInfo[g_input_info.nCurLen] = '\0';
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            }
        }
        break;
    }
    return FALSE;
}
PLUGIN_INFO g_layerInput = {
	0x00a30000/*id*/, 
	NULL/*InitLayer*/,
	NULL/*UninitLayer*/,
	InputDraw/*Draw*/,
	NULL/*Timer*/,
	InputKeyMsg/*keymsg*/,
	NULL/*ipc*/
};

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    弹出一个输入字符串页面
	输入参数：    char* pDefault：缺省字符串，输入当前的字符串
				  fnRetureInput pfnReturn：返回字符串信息的回调函数
				  void* pPara：回调函数的参数
				  int nType：类型掩码，用以指定可选的字符串类型
	输出参数：
	函数返回值：  BOOL ：TRUE表示成功执行，FALSE表示失败
	内部处理流程：
********************************************************************/
void InputBox(char* pDefault, fnRetureInput pfnReturn, void* pPara, int nType, int nCurType)
{
	ASSERT(pDefault != NULL);
    g_input_info.nIndex = 51;//确定
	// 设置当前的输入类型
    g_input_info.nTypeMask = nType;
	int nTmp = nType&nCurType;
	switch(nTmp)
	{
	case 0x01:g_input_info.nType=0;break;
	case 0x02:g_input_info.nType=1;break;
	case 0x04:g_input_info.nType=2;break;
	case 0x08:g_input_info.nType=3;break;
	case 0x10:g_input_info.nType=4;break;
	case 0x20:g_input_info.nType=5;break;
	default:
		{
			if((nType&0x02) == 0x02){g_input_info.nType=1;}
			else if((nType&0x04) == 0x04){g_input_info.nType=2;}
			else if((nType&0x01) == 0x01){g_input_info.nType=0;}
			else if((nType&0x08) == 0x08){g_input_info.nType=3;}
			else if((nType&0x10) == 0x10){g_input_info.nType=4;}
			else if((nType&0x20) == 0x20){g_input_info.nType=5;}
			else{return;}
		}
		break;
	}
    
	// 根据当前类型，设置参数
    GetInputInfo(g_input_info.nType);

	// 设置缺省字符串信息
    if(pDefault != NULL)
    {
        int nLength = strlen(pDefault);
        if(nLength > 0)
        {
            if(nLength > g_input_info.nMaxLen)nLength=g_input_info.nMaxLen;

            memcpy(g_input_info.czInfo, pDefault, nLength);
            g_input_info.czInfo[nLength] = '\0';
        }
		else
		{
			strcpy(g_input_info.czInfo, "");
		}
		g_input_info.nCurLen = nLength;
    }
	//strcpy(g_input_info.czInfo, "");
	//g_input_info.nCurLen = 0;

    ASSERT(pfnReturn != NULL);
    g_input_info.pfnReturn = pfnReturn;
    g_input_info.pPara = pPara;

    g_input_info.nTotalCnt = strlen(g_input_info.pInputStr);
    g_input_info.nLineCnt = g_input_info.nTotalCnt/4;
    if(g_input_info.nType == 0)
	{
		g_input_info.nLineCnt = g_input_info.nTotalCnt/2;
	}
	/*else if(g_input_info.nType == 4)
	{
		g_input_info.nLineCnt = g_input_info.nTotalCnt/3;
	}*/
	// 弹出页面
    PopLayer(g_layerInput.dwId);
}
/************************************************************************/
/* 输入法用户界面结束                                                   */
/************************************************************************/


/************************************************************************/
/* 输入时间范围界面                                                     */
/************************************************************************/
typedef struct _TIME_AREA
{
    int     nIndex;		// 选中项
    char    czInfo[15];	// 当前的时间字符串，其中包含了':'字符
						// 在处理key消息时需要跳过

	// 下面两个时间保存的初始输入时间，
	// 用以在输入错误以后，恢复输入时间
    int     nHour1;		// 开始时间 
    int     nMin1;
    int     nHour2;		// 结束时间
    int     nMin2;

    fnRetureTime  pfnReturn;
}TIME_AREA;
TIME_AREA g_time_area;
void TimeAreaDraw(HDC hdc)
{
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

	rc.top = 50;
    rc.bottom = rc.top+24;
    const char* pShow = "时间范围设置";
    DrawText(hdc, pShow, strlen(pShow), &rc, DT_CENTER|DT_VCENTER);

	// 画时间信息，需要一个一个字符draw，因为需要判定选择项
    int nTop = rc.top + 20;
    int nLeft = 47;
    int i;
    for(i = 0; i < 11; i++)
    {
        if(i == g_time_area.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_time_area.czInfo[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_time_area.czInfo[i], 1);
        }
        nLeft += 6;
    }

    nTop += 20;
    nLeft -= 12;
    if(g_time_area.nIndex == 12)
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

// 检查输入的时间范围是否正确
BOOL CheckNumber(int nHour1, int nMin1, int nHour2, int nMin2)
{
    int nVal1 = nHour1*60+nMin1;
    int nVal2 = nHour2*60+nMin2;
    if(nHour1 > 23)
    {
        if(TRUE == MessageBox("开始时间输入错误\n\n确认是否重新输入?", MB_OKCANCEL))
        {
            InputTimeArea(g_time_area.nHour1, g_time_area.nMin1, g_time_area.nHour2, g_time_area.nMin2, 
                    g_time_area.pfnReturn);
        }
        return FALSE;
    }
    if(nHour2 > 23)
    {
        if(TRUE == MessageBox("结束时间输入错误\n\n确认是否重新输入?", MB_OKCANCEL))
        {
            InputTimeArea(g_time_area.nHour1, g_time_area.nMin1, g_time_area.nHour2, g_time_area.nMin2, 
                    g_time_area.pfnReturn);
        }
        return FALSE;
    }
    if(nVal1 > nVal2)
    {
        if(TRUE == MessageBox("开始时间大于结束时间\n\n确认是否重新输入?", MB_OKCANCEL))
        {
            InputTimeArea(g_time_area.nHour1, g_time_area.nMin1, g_time_area.nHour2, g_time_area.nMin2, 
                    g_time_area.pfnReturn);
        }
        return FALSE;
    }
    if(nVal1 == nVal2)
    {
        if(TRUE == MessageBox("开始时间等于结束时间\n\n确认是否重新输入?", MB_OKCANCEL))
        {
            InputTimeArea(g_time_area.nHour1, g_time_area.nMin1, g_time_area.nHour2, g_time_area.nMin2, 
                    g_time_area.pfnReturn);
        }
        return FALSE;
    }
    return TRUE;
}
BOOL TimeAreaKeyMsg(int nKey)
{
    switch(nKey)
    {
    case KEY_LEFT:// 向左按键：需要跳过":"等字符串
        g_time_area.nIndex --;
        if(g_time_area.czInfo[g_time_area.nIndex] > '9'
            || g_time_area.czInfo[g_time_area.nIndex] < '0')
        {
            g_time_area.nIndex --;
        }
        if(g_time_area.nIndex < 0)
        {
            g_time_area.nIndex = 12;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:// 向右按键：需要跳过":"等字符串
        g_time_area.nIndex ++;
        if(g_time_area.czInfo[g_time_area.nIndex] > '9'
            || g_time_area.czInfo[g_time_area.nIndex] < '0')
        {
            g_time_area.nIndex ++;
        }
        if(g_time_area.nIndex > 12)
        {
            g_time_area.nIndex = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_UP:// 向上按键：这里需要判断实际的输入范围
        if(g_time_area.nIndex >= 0 && g_time_area.nIndex <= 11)
        {
            g_time_area.czInfo[g_time_area.nIndex] ++;
            char ctmp = '9';
            if(g_time_area.nIndex == 0 || g_time_area.nIndex == 6)
            {// 小时的10位 范围0--2
                ctmp = '2';
            }
            else if(g_time_area.nIndex == 3 || g_time_area.nIndex == 9)
            {// 分钟的10位 范围0--5
                ctmp = '5';
            }
            if(g_time_area.czInfo[g_time_area.nIndex] > ctmp)
            {
                g_time_area.czInfo[g_time_area.nIndex] = '0';
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    case KEY_DOWN:// 向下按键：这里需要判断实际的输入范围
        if(g_time_area.nIndex >= 0 && g_time_area.nIndex <= 11)
        {
            g_time_area.czInfo[g_time_area.nIndex] --;
            char ctmp = '9';
            if(g_time_area.nIndex == 0 || g_time_area.nIndex == 6)
            {// 小时的10位 范围0--2
                ctmp = '2';
            }
            else if(g_time_area.nIndex == 3 || g_time_area.nIndex == 9)
            {// 分钟的10位 范围0--5
                ctmp = '5';
            }
            if(g_time_area.czInfo[g_time_area.nIndex] < '0')
            {
                g_time_area.czInfo[g_time_area.nIndex] = ctmp;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    case KEY_OK:
        {// 对当前输入的时间字符串进行验证
            g_guiMain.plugPop = NULL;
			g_guiComm.nCurPoint = g_guiComm.nCurPointTmp;

            g_time_area.czInfo[2] = '\0';
            g_time_area.czInfo[5] = '\0';
            g_time_area.czInfo[8] = '\0';
            g_time_area.czInfo[11] = '\0';

            int nHour1, nMin1, nHour2, nMin2;
            char* pBuf = &g_time_area.czInfo[0];
            nHour1 = atoi(pBuf);

            pBuf = &g_time_area.czInfo[3];
            nMin1 = atoi(pBuf);

            pBuf = &g_time_area.czInfo[6];
            nHour2 = atoi(pBuf);

            pBuf = &g_time_area.czInfo[9];
            nMin2 = atoi(pBuf);
			// 验证通过的话才调用返回信息回调函数
            if(TRUE == CheckNumber(nHour1, nMin1, nHour2, nMin2))
            {
                g_time_area.pfnReturn(nHour1, nMin1, nHour2, nMin2);
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }
    return FALSE;
}
PLUGIN_INFO g_layerTimeArea = {
	0x00a40000/*id*/, 
	NULL/*InitLayer*/,
	NULL/*UninitLayer*/,
	TimeAreaDraw/*Draw*/,
	NULL/*Timer*/,
	TimeAreaKeyMsg/*keymsg*/,
	NULL/*ipc*/
};

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    弹出输入时间范围页面
	输入参数：    int nHour1, int nMin1, int nHour2, int nMin2：时间范围的开始及结束时间
				  fnRetureTime pfnRet：返回信息的回调函数
	输出参数：
	函数返回值：  
	内部处理流程：
********************************************************************/
void InputTimeArea(int nHour1, int nMin1, int nHour2, int nMin2, fnRetureTime pfnRet)
{
    ASSERT(pfnRet != NULL);
    g_time_area.nIndex = 0;
    sprintf(g_time_area.czInfo, "%02d:%02d-%02d:%02d",
        nHour1, nMin1, nHour2, nMin2);
    g_time_area.pfnReturn = pfnRet;

    g_time_area.nHour1 = nHour1;
    g_time_area.nMin1 = nMin1;
    g_time_area.nHour2 = nHour2;
    g_time_area.nMin2 = nMin2;

    PopLayer(g_layerTimeArea.dwId);
}
/************************************************************************/
/* 输入时间范围界面结束                                                 */
/************************************************************************/

/*-------------------------------------------------------------
*功能：清除统计模块指定计量点的所有私有数据
*处理：当重新配置一个计量点调用该函数,清除该计量点之前已经保存的私有数据
*输入：计量点号
*输出：无
*-------------------------------------------------------------
*/
void ClearStatData(uint8 msn)
{
	char 		pfile[100];
	int 		flag;

	printf("clear stat data! %d\n", msn);
	memset(pfile,0x00,100);
	sprintf(pfile,"rm -f /mnt/data/private/statpdata%u",msn);
	if(access(pfile,F_OK))
	{
		flag = system(pfile);
		if(flag  < 0)
		{
			printf("remove the stat private data failed\n");
		}
	}
	//ClearXData(msn);
}

char* g_pczPopMsg = NULL;
void PopMsgDraw(HDC hdc)
{
    ITEM_NOT_SELECT;
    
    // 判断信息字符串函数
    if(g_pczPopMsg == NULL)
	{
		usleep(100000);
	}
    
    int nLen = strlen(g_pczPopMsg);
    char* pPopMsg =  (char*)malloc(nLen+1);
    
    memset(pPopMsg, 0, nLen+1);
    strcpy(pPopMsg, g_pczPopMsg);
    pPopMsg[nLen] = '\0';

    char * pBuf = pPopMsg;
    
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
	if(nLine > 9)
    {
        nLine = 9;
    }   
    int nTop = (g_guiComm.nHeight-(nLine*13)-32)/2+17;
    SelectFont(hdc, g_guiComm.fontSmall);
    int posintval=12;

    // 绘制字符串信息
    pBuf = pPopMsg;
    char* pTmp = pBuf;
    char tmpbug;
    while(*pTmp != '\0')
    {
        if(*pTmp == '\n')
        {

            if(nTop > 130)
            {
                GUI_FMT_DEBUG("显示行数过多，丢弃，nTop %d\n", nTop );
                break;
            }  
			
            if(pTmp != pBuf)
            {
                tmpbug = *(pTmp-1);
                if(tmpbug == '\r')
                {
                    TextOutLen(hdc, 0, nTop, pBuf, pTmp-pBuf-1);
//                    DrawText(hdc, pBuf, pTmp-pBuf-1, &rc, DT_CENTER|DT_VCENTER);
                }
                else
                {
                    TextOutLen(hdc, 0, nTop, pBuf, pTmp-pBuf);
//                    DrawText(hdc, pBuf, pTmp-pBuf, &rc, DT_CENTER|DT_VCENTER);
                }
            }
            nTop += posintval+1;
            pBuf = pTmp+1;
        }
        pTmp ++;
    }
    if(pTmp != pBuf && nTop <= 130)
    {
        TextOutLen(hdc, 0, nTop, pBuf, pTmp-pBuf);
    }

    if(pPopMsg != NULL)
	{
		free(pPopMsg);
		pPopMsg = NULL;
	}
}
BOOL PopMsgKeyMsg(int nKey)
{
//#if NEW_GW_GUI_APPEND == 1
//    uint8 EndSymbol = 0;
//    if (nKey == KEY_CANCEL)
//    {
//        ClosePopLayer();
//        EndSymbol = 0xBB;
//        write_pdata_xram(GUI_RELAY_PN_FILE, (char *)&EndSymbol,0, sizeof(EndSymbol));
//    }
//    return TRUE;
//#else
	return TRUE;
//#endif
}
PLUGIN_INFO g_layerPopMsg = {
	0x00FFFFFD/*id*/, 
	NULL/*InitLayer*/,
	NULL/*UninitLayer*/,
	PopMsgDraw/*Draw*/,
	NULL/*Timer*/,
	PopMsgKeyMsg/*keymsg*/,
	NULL/*ipc*/
};

extern void OnPaint();
extern POP_MES_BOX_T g_pop_mes_Fresh;

void PopMsg(const char* pczMsg)
{
//	ClosePopLayer();
    
	if(pczMsg == NULL)
	{
	    GUI_FMT_TRACE("pczMsg NULL\n");
		ClosePopLayer();
		return;
	}

    pthread_mutex_lock(&appGuiInfoLock);
	
	if(g_pczPopMsg != NULL)
	{
		free(g_pczPopMsg);
		g_pczPopMsg = NULL;
	}
    
	int nLen = strlen(pczMsg);
    
    ASSERT(g_pczPopMsg == NULL);
    g_pczPopMsg = (char*)malloc(nLen+1);
    
    memset(g_pczPopMsg, 0, nLen+1);
    strcpy(g_pczPopMsg, pczMsg);
    g_pczPopMsg[nLen] = '\0';

    pthread_mutex_unlock(&appGuiInfoLock);

	PopLayer(g_layerPopMsg.dwId);
//    g_pop_mes_Fresh.is_pop = TRUE;
//	OnPaint();
}

void CurPointReturn(char* pNewStr, void* pPara)
{	
    int nPoint = atoi(pNewStr);
    if(nPoint >= VIR_BEGIN)
    {
        char bzbuf[100];
        sprintf(bzbuf, "测量点最大为%d\n确认是否重新输入?", (VIR_BEGIN-1));
        if(TRUE == MessageBox(bzbuf, MB_OKCANCEL))
        {
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
        }
    }
    else
    {
        g_guiComm.nCurPoint = nPoint;
    }
}

double type2todoble(char* pBuf)
{
	ASSERT(pBuf != NULL);
	double dVal = (pBuf[0]&0x0F)+((pBuf[0]>>4)&0x0F)*10+(pBuf[1]&0x0F)*100;
	int nG = (pBuf[1]>>5)&0x7;
	switch(nG)
	{
	case 0:dVal *= 10000;break;
	case 1:dVal *= 1000;break;
	case 2:dVal *= 100;break;
	case 3:dVal *= 10;break;
	case 4:break;
	case 5:dVal *= 0.1;break;
	case 6:dVal *= 0.01;break;
	case 7:dVal *= 0.001;break;
	}
	if((pBuf[1]&0x10) != 0)
	{
		return -dVal;
	}
	return dVal;
}

int type3toint(char* pBuf)
{
	ASSERT(pBuf != NULL);
	int nVal = (pBuf[0]&0x0F)+((pBuf[0]>>4)&0x0F)*10
		+(pBuf[1]&0x0F)*100+((pBuf[1]>>4)&0x0F)*1000
		+(pBuf[2]&0x0F)*10000+((pBuf[2]>>4)&0x0F)*100000
		+(pBuf[3]&0x0F)*1000000;
	if((pBuf[3]&0x10)!=0)
	{
		return -nVal;
	}
	return nVal;
}

