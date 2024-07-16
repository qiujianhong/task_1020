/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
  程序名称: debuginfo.cpp
  功    能：完成配置页面库
  开 发 人：gaoyf
  开发时间：2007-1

  版本历史:

    v1.0:
      2006-1-20, gaoyf : New file

  包含页面库：
    验证密码    0x00600000
    变更密码    0x00600001

    复位终端命令    0x00610000
    终端时间设置    0x00620000
    软件重新安装     0x00680000
    系统日志转存     0x00680002

    GPRS设置画面    0x00660001

***************************************************************************
*/

#include "main.h"
#include "iniparser.h"
#include "commfun.h"

extern PLUGIN_INFO g_layerValidatePwd;
extern PLUGIN_INFO g_layerResetTime;
extern char* g_pczMeasureFlag[];

//int IPconfig_set_to_smiOS(char* Name);

/************************************************************************/
/* 验证密码                                                             */
/************************************************************************/
typedef struct _VALID_PWD
{
    int     nIndex;     // 当前索引
    char    czPwd[PASSWORD_LEN+1];   // 输入密码
}VALID_PWD;
VALID_PWD g_valid_pwd;
int	g_nErrPwdCnt = 0;
const char* g_IPConfigMode[] = {"DHCP","静态","PPPoE"};
/*********************************************************************
	所属功能单元：配置页面库--密码验证
	功能描述：    密码验证初始化函数
	输入参数：    DWORD dwPluginId：页面库ID
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
void ValidatePwdInit(DWORD dwPluginId)
{
    g_valid_pwd.nIndex = PASSWORD_LEN+1;
    memset(g_valid_pwd.czPwd, '0',PASSWORD_LEN);
    /*#ifdef AREA_FUJIAN
		memcpy(g_valid_pwd.czPwd, "000000", sizeof(g_valid_pwd.czPwd));
    #elif AREA_JIANGXI
        memcpy(g_valid_pwd.czPwd, "000000", sizeof(g_valid_pwd.czPwd));
	#else
		memcpy(g_valid_pwd.czPwd, PASSWORD_STR, sizeof(g_valid_pwd.czPwd));
	#endif*/
}
/*********************************************************************
	所属功能单元：配置页面库--密码验证
	功能描述：    密码验证绘制函数
	输入参数：    HDC hdc：绘制上下文句柄
	输出参数：
	函数返回值：
	内部处理流程：
********************************************************************/
void ValidatePwdDraw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

	TextOut(hdc, 14, 26,"修改配置必须验证密码");

    // 绘制界面
    TextOut(hdc, 20, 50,"请输入密码");
    Rectangle(hdc, 28, 70, 30 + 8 * PASSWORD_LEN + 2, 92);

    // 绘制密码部分
    int nLeft=32;
    int nTop=74;
    uint32 i;
    for(i = 0; i < PASSWORD_LEN; i ++)
    {
        if((uint32)g_valid_pwd.nIndex == i)
        {
            ITEM_SELECT;
            TextOutLen(hdc,nLeft,nTop,(const char*)&g_valid_pwd.czPwd[i],1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc,nLeft,nTop,(const char*)&g_valid_pwd.czPwd[i],1);
        }
        nLeft+=8;
    }

    nLeft += 8;
    if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN)
    {
        ITEM_SELECT;
        TextOut(hdc,nLeft,nTop,"详细");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc,nLeft,nTop,"详细");
    }

    // 绘制OK部分
    nTop += 28;
    if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+1)
    {
        ITEM_SELECT;
        TextOut(hdc,nLeft,nTop,"确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc,nLeft,nTop,"确定");
    }

    #if MODULARIZATION_TMN_EN == 1
    if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+2)
    {
        ITEM_SELECT;
        TextOut(hdc, 38, nTop, "返回");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 38, nTop,"返回");
    }
    #endif
}
int IsValidatePwd()
{
	char czPwd[20];

	// 读取当前的密码
#ifdef _MSC_VER
#if HARDWARE_2012_EN ==1
	memcpy(czPwd, SETPARAPASSWORD, PASSWORD_LEN+1);
#else
	memcpy(czPwd, PASSWORD_STR, PASSWORD_LEN+1);
#endif
#else
	int nRet = read_pdata_xram(GUI_PRIVATE_DATA, czPwd, PRIVATE_PASSWORD, PASSWORD_LEN);
	if(nRet < 0)
	{
		#if HARDWARE_2012_EN ==1
			memcpy(czPwd, SETPARAPASSWORD, PASSWORD_LEN+1);
		#else
			memcpy(czPwd, PASSWORD_STR, PASSWORD_LEN+1);
		#endif
	}
#endif
	//万能密码
	if(memcmp(czPwd, g_valid_pwd.czPwd, PASSWORD_LEN) == 0 || memcmp("334455", g_valid_pwd.czPwd, 6) == 0)
	{
		g_guiComm.bValidatePwd = TRUE;
		g_nErrPwdCnt = 0;
	    MessageBox("密码验证成功!");
		return RET_CANCEL;// 关闭页面
	}
	else
	{// 验证错误
		if(g_guiMain.plugPop != NULL
			&& g_guiMain.plugPop->dwId == g_layerValidatePwd.dwId)
		{
			g_guiMain.plugPop = NULL;
			g_guiComm.nCurPoint = g_guiComm.nCurPointTmp;
		}
		g_nErrPwdCnt ++;
		#if SECURITY_DEFEND == 1
		if(g_nErrPwdCnt == 5)
		#else
		if(g_nErrPwdCnt == 3)
		#endif
		{
			long lTime = time(NULL);
			nRet = write_pdata_xram(GUI_PRIVATE_DATA, (char*)&lTime, PRIVATE_FREEZE_TIME, sizeof(long));
			//printf("WritePDataXram return %d\n", nRet);
			printf("return %d\n", nRet);
			ASSERT(nRet >= 0);
			g_nErrPwdCnt = 0;
			#if SECURITY_DEFEND == 1
			MessageBox("五次密码输入错误!\n1小时冻结修改配置");
			#else
			MessageBox("三次密码输入错误!\n24小时冻结修改配置");
			#endif
		}
		else
		{
			MessageBox("密码输入错误!");
		}
	}
	return TRUE;
}
void ValidatePwdReturn(char* pNewStr, void* pPara)
{
     if(strlen(pNewStr) != PASSWORD_LEN && memcmp("334455", pNewStr, 6) != 0)
    {
		char msg[64];
		sprintf(msg, "密码长度必须为%u字节\n\n确认是否重新输入?", PASSWORD_LEN);
        if(TRUE == MessageBox(msg, MB_OKCANCEL))
        {
            InputBox(g_valid_pwd.czPwd, ValidatePwdReturn);
        }
    }
    else
    {
	    memcpy(g_valid_pwd.czPwd, pNewStr, sizeof(g_valid_pwd.czPwd));
		IsValidatePwd();
    }
}

/*********************************************************************
	所属功能单元：配置页面库--密码验证
	功能描述：    密码验证按键处理消息
	输入参数：    int nKey：按键消息
	输出参数：
	函数返回值：  BOOL：TRUE表示页面库已完成处理，FALSE表示执行缺省处理
	内部处理流程：
********************************************************************/
BOOL ValidatePwdKeyMsg(int nKey)
{
    switch(nKey)
    {
    case KEY_LEFT:
        #if MODULARIZATION_TMN_EN == 1
        if(g_valid_pwd.nIndex == PASSWORD_LEN+1)
        {
            g_valid_pwd.nIndex = PASSWORD_LEN+2;
        }
        else if(g_valid_pwd.nIndex == PASSWORD_LEN+2)
        {
            g_valid_pwd.nIndex = PASSWORD_LEN+1;
        }
        else
        {
            g_valid_pwd.nIndex --;
            if(g_valid_pwd.nIndex < 0)
            {
                g_valid_pwd.nIndex = PASSWORD_LEN;
            }
        }
        #else
        g_valid_pwd.nIndex --;
        if(g_valid_pwd.nIndex < 0)
        {
            g_valid_pwd.nIndex = PASSWORD_LEN+1;
        }
        #endif
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        #if MODULARIZATION_TMN_EN == 1
        if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+1)
        {
            g_valid_pwd.nIndex = PASSWORD_LEN+2;
        }
        else if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+2)
        {
            g_valid_pwd.nIndex = PASSWORD_LEN+1;
        }
        else
        {
            g_valid_pwd.nIndex ++;
            if((uint32)g_valid_pwd.nIndex > PASSWORD_LEN)
            {
                g_valid_pwd.nIndex = 0;
            }
        }
        #else
        g_valid_pwd.nIndex ++;
        if((uint32)g_valid_pwd.nIndex > PASSWORD_LEN+1)
        {
            g_valid_pwd.nIndex = 0;
        }
        #endif
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_UP:
        if((uint32)g_valid_pwd.nIndex >= 0 && (uint32)g_valid_pwd.nIndex < PASSWORD_LEN)
        {
            g_valid_pwd.czPwd[g_valid_pwd.nIndex] ++;
            if(g_valid_pwd.czPwd[g_valid_pwd.nIndex] > '9')
            {
                g_valid_pwd.czPwd[g_valid_pwd.nIndex] = '0';
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        else if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+1)
        {
            g_valid_pwd.nIndex --;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        #if MODULARIZATION_TMN_EN == 1
        else if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+2)
        {
            g_valid_pwd.nIndex  = 0;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        #endif
        break;
    case KEY_DOWN:
        if((uint32)g_valid_pwd.nIndex >= 0 && (uint32)g_valid_pwd.nIndex < PASSWORD_LEN)
        {
            g_valid_pwd.czPwd[g_valid_pwd.nIndex] --;
            if(g_valid_pwd.czPwd[g_valid_pwd.nIndex] < '0')
            {
                g_valid_pwd.czPwd[g_valid_pwd.nIndex] = '9';
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        else if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN)
        {
            g_valid_pwd.nIndex ++;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        break;
    case KEY_OK:
        {// 确认处理
            if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN)
            {// 处理详细选择按键，
                InputBox(g_valid_pwd.czPwd, ValidatePwdReturn);
            }
            else if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+1)
            {// 处理确定按键
                return IsValidatePwd();
            }
            #if MODULARIZATION_TMN_EN == 1
            else if((uint32)g_valid_pwd.nIndex == PASSWORD_LEN+2)
            {// 处理取消键
                ClosePopLayer();
            }
            #endif
        }
        break;
    }
    return FALSE;
}
PLUGIN_INFO g_layerValidatePwd = {
	0x00600000/*id*/,
	ValidatePwdInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	ValidatePwdDraw/*Draw*/,
	NULL/*Timer*/,
	ValidatePwdKeyMsg/*keymsg*/,
	NULL/*ipc*/
};
/************************************************************************/
/* 验证密码结束                                                         */
/************************************************************************/
//大于0表示处于冻结,小于0处于解冻
long GetIceOutTime()
{
	char czBuf[10];
	int nRet = read_pdata_xram(GUI_PRIVATE_DATA, czBuf, PRIVATE_FREEZE_TIME, sizeof(long));
	if(nRet < 0 ||
		((unsigned char)czBuf[0] == NONE_CHAR&&(unsigned char)czBuf[1] == NONE_CHAR
		&&(unsigned char)czBuf[2] == NONE_CHAR&&(unsigned char)czBuf[3] == NONE_CHAR))
	{// 数据无效
		return -1;// 处于解冻状态
	}
	#if SECURITY_DEFEND == 1
	long lTime = (long)chartoint32(czBuf)+3600; // 延后一小时
	#else
	long lTime = (long)chartoint32(czBuf)+86400; // 延后一天
	#endif
	long lNow = time(NULL);
	//long lNow = lTime;
	if(lTime-lNow <= 0)
	{ // 已然解冻了，就设为无效，因为可能会修改时间，而正好在冻结期内
		czBuf[0] = (char)NONE_CHAR;
		czBuf[1] = (char)NONE_CHAR;
		czBuf[2] = (char)NONE_CHAR;
		czBuf[3] = (char)NONE_CHAR;
		int nRet = write_pdata_xram(GUI_PRIVATE_DATA, czBuf, PRIVATE_FREEZE_TIME, sizeof(long));
		ASSERT(nRet >= 0);
	}
	return lTime-lNow;//大于0表示处于冻结
}
int g_nIceOut[] = {KEY_UP, KEY_DOWN, KEY_UP, KEY_UP, KEY_RIGHT, KEY_LEFT};
int g_nIceOutLen = sizeof(g_nIceOut)/sizeof(g_nIceOut[0]);
int g_nIceOutIndex = 0;
void FreezeTimeInit(DWORD dwPluginId)
{
	g_nIceOutIndex = 0;
}
void FreezeTimeDraw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

	char czBuf[300];
	RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 40;
    rc.bottom = rc.top+16;

	long lTime = GetIceOutTime();
	if(lTime > 0)
	{
		sprintf(czBuf, "当前处于设置冻结状态");
		DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

		rc.top += 26;
		rc.bottom = rc.top+16;
		sprintf(czBuf, "距解冻时间还有");
		DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

		int nHour = lTime / 3600;
		lTime -= (nHour*3600);
		int nMin = lTime / 60;
		int nSec = lTime % 60;
		rc.top += 18;
		rc.bottom = rc.top+16;
		sprintf(czBuf, "%02d小时%02d分%02d秒", nHour, nMin, nSec);
		DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
	}
	else
	{
		sprintf(czBuf, "当前已处于设置解冻状态");
		DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
	}
}
BOOL FreezeTimeKeyMsg(int nKey)
{
	if(nKey == KEY_OK || nKey == KEY_CANCEL)
	{
		return FALSE;
	}
	if(g_nIceOut[g_nIceOutIndex] != nKey)
	{
		g_nIceOutIndex = 0;
		return TRUE;
	}
	g_nIceOutIndex ++;
    if(g_nIceOutIndex == g_nIceOutLen)
	{// 全对了，解冻操作
		g_guiComm.bValidatePwd = FALSE;
		// 已然解冻了，就设为无效，因为可能会修改时间，而正好在冻结期内
		char czBuf[10];
		czBuf[0] = (char)NONE_CHAR;
		czBuf[1] = (char)NONE_CHAR;
		czBuf[2] = (char)NONE_CHAR;
		czBuf[3] = (char)NONE_CHAR;
		int nRet = write_pdata_xram(GUI_PRIVATE_DATA, czBuf, PRIVATE_FREEZE_TIME, sizeof(long));
		ASSERT(nRet >= 0);

		return TRUE;
	}

	return TRUE;
}
PLUGIN_INFO g_layerFreezeTime = {
	0x00600002/*id*/,
	FreezeTimeInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	FreezeTimeDraw/*Draw*/,
	NULL/*Timer*/,
	FreezeTimeKeyMsg/*keymsg*/,
	NULL/*ipc*/
};


