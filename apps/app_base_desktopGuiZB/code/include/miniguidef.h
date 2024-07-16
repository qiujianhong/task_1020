/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
  程序名称: main.h
  功    能：FKGA42显示模块框架实现文件，用于定义和添加页面库操作
  开 发 人：gaoyf
  开发时间：2007-1
  
  版本历史:
  
    v1.0:
      2006-1-8, gaoyf : New file        
  
***************************************************************************
*/

#ifndef _MINIGUI_DEF_H_
#define _MINIGUI_DEF_H_

//#define ARM_LINUX 1
#define MODULE_AC_SAMPLE

#include "minigui/common.h"     /*MiniGUI公用定义*/
#include "minigui/minigui.h"    /*MiniGUI头文件*/
#include "minigui/gdi.h"        /*MiniGUI绘图相关定义*/
#include "minigui/window.h"     /*MiniGUI窗口相关定义*/
#include <stdio.h>              /*标准输入输出定义*/
#include <stdlib.h>             /*标准函数库定义*/
#include <string.h>             /*字符处理函数定义*/
#include <sys/time.h>
#include <unistd.h>

#undef HIWORD
#undef LOWORD

#define _DLL_IMPLICIT_ 1
//#include "mem.h"      /*数字字典定义*/
#include "sys_cfg.h"      /*数字字典定义*/
extern "C" 
{
#include "mettype.h"
}
#include "commdrv.h"
#include "commfun.h"
//#include "hal.h"
//#include "key.h"
#include "db_api.h"
#include "framecomm.h"

#if SHANDONG_FUN == 1 && HARDWARE_TYPE == 3
#include "../../../common/include/ad.h"
#endif

#include "storage.h"           // 通用存储头文件

extern DB_CLIENT g_hUdbCenter;
#define GUI_PRIVATE_DATA    "^gui_paivate_file"
#define GUI_CONFIG_DATA     "^gui_config_file"
#define EVENT_PRIVATE_FILE  "^Event"
#define GUI_PARA            "^gui_para_file"//显示参数

//轮显参数大小和偏移量  GUI_PARA中使用
#define Bluetooth_Status        0                                //蓝牙状态
#define Bluetooth_Status_Len    1
#define Esam_Status             (Bluetooth_Status+Bluetooth_Status_Len)    //秘钥下装状态
#define Esam_Status_Len         1
#define Turn_Num_Time           (Esam_Status+Esam_Status_Len)    //轮显画面数；按键空闲进入轮显时间；轮显间隔时间*/
#define Turn_Num_Time_Len       3
#define Turn_IDs                (Turn_Num_Time+Turn_Num_Time_Len)
#define Turn_IDs_Len            (MAX_TRUNDISP_CNT*4)

#define GUI_DEBUG 	0
#if GUI_DEBUG == 1
  #define GUI_DEBUG_ENV "guidebug"
  #define MEASURE_DEGUG_ENV  "measuredebug"
  #define GUI_LOG_FILE		"/mnt/log/gui.log"
#endif

#undef NULL
#define NULL    0
//#define NULL    ((void *)0)
#define DWORD unsigned int

#undef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0

#define TIMER_MAIN	0
#define TIMER_QUICK	1

// 键盘消息
#define KEY_UP		1
#define KEY_DOWN	2
#define KEY_LEFT	3
#define KEY_RIGHT	4
#define KEY_OK		5
#define KEY_CANCEL	6
#define KEY_ADDTRUN	7
#define KEY_MENU_EXT	8
#define KEY_PROTECT	9
#if MODULARIZATION_TMN_EN == 1
#define MODU_CANCEL KEY_OK
#endif
#if HARDWARE_2012_EN==1
#define KEY_SETPARA  	10
#endif

// 页面库按键消息处理函数返回值
#define RET_DEFAULT 0
#define RET_OK      1
#define RET_CANCEL  -1

// icons存放路径
#ifdef _MSC_VER
#define ICON_PATH	"c:\\icons\\"
#else
#define ICON_PATH	"/usr/local/extapps/desktopGui/bin/cfg/icons/"
#endif

#if GUI_SYSVERSION_EN == 1
#define VERSION_LEN				400
#define OS_VERSION_LEN			17								  //系统版本字节数
#endif


// 页面库相关函数定义
typedef void (*fnInitLayer)(DWORD dwPluginID);
typedef void (*fnUninitLayer)();
typedef void (*fnOnTimer)(int nId);
typedef void (*fnDraw)(HDC hdc);
/*页面内处理返回TRUE，缺省处理返回FALSE,返回RET_CANCEL则关闭该页(用于弹出页面)*/
typedef BOOL (*fnKeyMsg)(int nKey);
typedef BOOL (*fnDisposeIPC)(char* pBuf, int nLen);
typedef struct _PLUGIN_INFO
{
	DWORD			dwId;
	fnInitLayer		pfnInitLayer;	/*切换到页面时首先调用(可以为NULL)*/
	fnUninitLayer	pfnUninitLayer;	/*切换到其它页面时调用(可以为NULL)*/
	fnDraw			pfnDraw;	/*画页面*/
	fnOnTimer		pfnOnTimer;	/*定时器触发*/
	fnKeyMsg		pfnKeyMsg;	/*按键消息(可以为NULL)*/
	fnDisposeIPC	pfnDisposeIPC;/*(可以为NULL)*/
}PLUGIN_INFO;

#if MODULARIZATION_TMN_EN == 1
#define Cancel_ICON_COUNT	2
typedef struct _MENU_ICON
{
	BITMAP		mapCancel[Cancel_ICON_COUNT];	// 主界面返回按钮
	int			bCancel[Cancel_ICON_COUNT];
}MENU_ICON;

void CancelKeyDraw(HDC hdc, int x, int y, int w, int h);
void CancelKeyDrawsub(HDC hdc, int x, int y, int w, int h,int i);
#endif



// 公用变量定义
typedef struct _GUI_COMM
{
    HWND hMainWnd;      // 主窗口
    PLOGFONT fontBig;   // 大字体16*16
    PLOGFONT fontSmall; // 小字体12*12

    int nWidth;         // 主窗口宽度
    int nHeight;        // 主窗口高度

    BOOL bTrundispTally;    // 是否进行轮显计数，TRUE:表示进行轮显计数，达到轮显时间，则进入轮显，
    // FALSE不计数，不会进入轮显

    int nCurPoint;      // 当前测量点
	int nCurPointTmp;	// 临时测量点

    int nIndexPoint;    // 屏号
	int nIndexPointTmp; // 屏号
	
    BOOL bPopList;		// 是否处于弹出菜单
	BOOL bValidatePwd;	// 是否需要进行验证密码 FALSE：需要 TRUE：不需要
	// 在确认密码以后一段时间无按键，会设为FALSE

	BOOL bKeyDown;		// 是否为向下按键(用于在上下翻页时确认当前选择项)

	BOOL bCanTrunDisp;	// 当前页面是否可以加入到轮显页面中
	BOOL bCanTrunDispTmp;	// 因为不能区分页面库的区别，当进入轮显或弹出页面时，会修改bCanTrunDisp
						// 所以需要临时保存值，回到当前页面时，用于恢复
#if BEIJING_FUN
	BOOL bKeyInputFlag;	// 是否处于弹出菜单
#endif
#if MODULARIZATION_TMN_EN == 1
	BOOL bOKiscancel;	// 按确定键实现返回功能，采用按键处理取消键的缺省操作
#endif

	int	 nArrow;
	int	 nArrowTmp;


    int16  NodeAddrPage;   //总页数
    int16  NodeAddrIndex;  //页面索引


    char    sCaption[30];

	BOOL bPointOffset;	// 用于浙江版偏移到0测量点时
	BOOL bPointOffsetTmp;	// 用于浙江版偏移到0测量点时

	BOOL bSkipLayer;

	int	 nCurKey;		// 当前按键
	int	 nPwdKey;	// 出现密码页面输入按键

}GUI_COMM;
extern GUI_COMM g_guiComm;

#define	ITEM_SELECT	SetBkColor(hdc,PIXEL_black); \
			        SetTextColor(hdc,PIXEL_lightwhite);
#define	ITEM_NOT_SELECT	SetBkColor(hdc,PIXEL_lightwhite); \
			        SetTextColor(hdc,PIXEL_black);

//--用来解决类型强制转换的问题--
/*#define chartoint16(p) 	((*(unsigned char*)(p + 1) << 8) | *(unsigned char*)(p))
#define chartoint32(p) 	((*(unsigned char*)(p + 3) << 24) | (*(unsigned char*)(p + 2) << 16) | (*(unsigned char*)(p + 1) << 8) | *(unsigned char*)(p))
#define chartoint64(p) 	(((int64)(*(unsigned char*)(p + 7)) << 56) | ((int64)(*(unsigned char*)(p + 6)) << 48) \
							| ((int64)(*(unsigned char*)(p + 5)) << 40) | ((int64)(*(unsigned char*)(p + 4)) << 32) \
							| (*(unsigned char*)(p + 3) << 24) | (*(unsigned char*)(p + 2) << 16) \
							| (*(unsigned char*)(p + 1) << 8) | *(unsigned char*)(p))*/
short chartoint16(char* pBuf);
int chartoint32(char* pBuf);
int64 chartoint64(char* pBuf);
void int16tochar(short nVal, char* pchar);
void int32tochar(int nVal, char* pchar);
void int64tochar(int64 nVal, char* pchar);

#ifdef ARM_LINUX
extern BOOL g_bRunPaint;
void OnPaint();
#define UpdateWindow(wnd, val)	g_bRunPaint = TRUE;
#endif

#if HARDWARE_TYPE == 2 || HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4
void GetStatusInfo();
void GetNewStatusInfo();
void GetStatusInfo_Desk();

#endif

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    缺省的页面库释放函数，设置测量点为0，可用于缺省的页面库回调
	输入参数：    DWORD dwPluginId：页面库ID
	输出参数：
	函数返回值：  BOOL ：TRUE表示成功执行，FALSE表示失败
	内部处理流程：
********************************************************************/
void UninitDefault();

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    弹出一个指定的页面库，用于弹出密码确认，IPC命令弹出
	输入参数：    DWORD dwPluginId：页面库ID
	输出参数：
	函数返回值：  BOOL ：TRUE表示成功执行，FALSE表示失败
	内部处理流程：
********************************************************************/
BOOL PopLayer(DWORD dwPluginId);
void ClosePopLayer();
void PopCustomDebug(char* pMsg);
typedef void (*fnInitLayer)(DWORD dwPluginID);

void PopMsg(const char* pczMsg);

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    弹出一个输入字符串页面
	输入参数：    char* pDefault：缺省字符串，输入当前的字符串
				  fnRetureInput pfnReturn：返回字符串信息的回调函数
				  void* pPara：回调函数的参数
				  int nType：类型掩码，用以指定可选的字符串类型
	输出参数：
	函数返回值：  
	内部处理流程：
********************************************************************/
typedef void (*fnRetureInput)(char* pNewStr, void* pPara);
void InputBox(char* pDefault = NULL,
              fnRetureInput pfnReturn = NULL, void* pPara = NULL,
              int nType = 0x01|0x02|0x04|0x08|0x10,
			  int nCurType = 0x01);

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    弹出输入时间范围页面
	输入参数：    int nHour1, int nMin1, int nHour2, int nMin2：时间范围的开始及结束时间
				  fnRetureTime pfnRet：返回信息的回调函数
	输出参数：
	函数返回值：  
	内部处理流程：
********************************************************************/
typedef void (*fnRetureTime)(int nHour1, int nMin1, int nHour2, int nMin2);
void InputTimeArea(int nHour1, int nMin1, int nHour2, int nMin2, fnRetureTime pfnRet);

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    弹出菜单
	输入参数：    char* pList[]：菜单列表字符串
				  int nLine：菜单项数
				  char* pczSel：选中的菜单字符串
				  int nLeft, int nTop：菜单的左上角位置
				  BOOL bBigFont：是否大字体
	输出参数：
	函数返回值：  
	内部处理流程：
********************************************************************/
void PopList(const char * const pList[], int nLine, const char *pczSel, int nLeft, int nTop, BOOL bBigFont);
/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    画弹出菜单，在页面库的Draw中调用该函数
	输入参数：    HDC hdc：画面设备上下文
	输出参数：
	函数返回值：  
	内部处理流程：
********************************************************************/
void PopListDraw(HDC hdc);
/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    处理菜单的按键，需先判断是否弹出菜单
	输入参数：    int nKey：按键消息
				  int* nSel：返回的选中项索引
	输出参数：	  char*：选中项字符串
	函数返回值：  
	内部处理流程：
********************************************************************/
char* PopListKeyMsg(int nKey, int* nSel);

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    弹出测量点选择页面库
	输入参数：    int nCurSel：支持16个测量点(0-15)
	输出参数：	  
	函数返回值：  
	内部处理流程：
********************************************************************/
void PopMeasure(int nCurSel/*0-15*/);
void PopMeasureDraw(HDC hdc);
BOOL PopMeasureKeyMsg(int nKey, int* nSel);

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    密码验证，在一个时间内没有经过密码验证，会弹出验证框
	输入参数：    
	输出参数：	  
	函数返回值：  
	内部处理流程：
********************************************************************/
BOOL ValidatePwd();

void KeepNextTrunDisp(BOOL bPre = FALSE);

// 参数设置时进行的数据类型转换函数
BOOL CheckNumber(char* pNum, int nMax=-1);
BOOL CheckFloat(char* pNum, int nMax=-1);
BOOL CheckAPN(char* pNum);
// bcd码转换为int8类型 2位bcd转换为1字节bin
uint8 bcd2int8(char val);
// int8类型转换为bcd码 1字节bin转换为2位bcd
char int82bcd(uint8 val);
// bcd码转换为short类型 4位bcd转换为2字节
uint16 bcd2short(char* val);
// short类型转换为bcd码 2字节bin转换为4位bcd
void short2bcd(uint16 val, char* pbcd);

// 16进制ascii码转换为short
short hexasc2short(char* val);
// short转换为16进制ascii
void short2hexasc(char* val, char* pasc);

// 16进制ascii码转换为int
int hexasc2int(char* val);
int asc2int(char* val);
// int转换为16进制ascii
void int2hexasc(int val, char* pasc);

#undef MB_OK
#undef MB_OKCANCEL
#define MB_OK       1
#define MB_OKCANCEL 2
BOOL MessageBox(const char* pczMsg, int nType = MB_OK);

BOOL SendSetFromIpc(char* pBuf, int nLen);

void Assert(const char* pFileName, int nLine);
#define ASSERT(x)	TRACE_LOG(x, __FILE__, __LINE__)
#define TRACE_LOG(x, filename, line) if((x)!=TRUE){Assert(filename, line);}
void PrintHexData(char* pHex, int nHexLen);
#define PRINT_HEX(data, len)	PrintHexData(data, len);

#undef NONE_CHAR
#undef NONE_WORD
#undef NONE_LWORD
#undef NONE_LLWORD
/*#if GB_PRTL == 1
#define NONE_CHAR		0xee
#define NONE_WORD		0xeeee
#define NONE_LWORD	0xeeeeeeeeL
#define NONE_LLWORD 0xeeeeeeeeeeeeeeeeLL
#endif
#if ZJ_PRTL == 1*/
#define NONE_CHAR		0xff
#define NONE_WORD		0xffff
#define NONE_LWORD	0xffffffffL
#define NONE_LLWORD 0xffffffffffffffffLL
//#endif


//私有文件偏移量
#define PRIVATE_FREEZE_TIME 0	// 存放输错密码后的冻结延时(长度4字节)
#define PRIVATE_PASSWORD	PRIVATE_FREEZE_TIME+4	// 存放密码偏移地址

//密码定义
#if SHANGHAI_FUN == 1
#define SETPARAPASSWORD     "0000"  // 设置按键默认初始密码
#define PASSWORD_STR        "0000"  // 默认初始密码
#define PASSWORD_LEN        4

#elif HUBEI_FUN == 1
#define SETPARAPASSWORD     "111111"  // 设置按键默认初始密码
#define PASSWORD_STR        "000000"  // 默认初始密码
#define PASSWORD_LEN        6
#else
	#ifdef AREA_FUJIAN
		#define SETPARAPASSWORD     "095598"  // 设置按键默认初始密码
		#define PASSWORD_STR        "095598"  // 默认初始密码
		#define PASSWORD_LEN        6
	#elif AREA_JIANGXI
		#define SETPARAPASSWORD     "659539"  // 设置按键默认初始密码
		#define PASSWORD_STR        "659539"  // 默认初始密码
		#define PASSWORD_LEN        6
	#elif AREA_SHANXI
		#define SETPARAPASSWORD     "sxdl@123"  // 设置按键默认初始密码
		#define PASSWORD_STR        "sxdl@123"  // 默认初始密码
		#define PASSWORD_LEN        8
	#elif AREA_HEBEI
		#define SETPARAPASSWORD     "DLJL@123"  // 设置按键默认初始密码
		#define PASSWORD_STR        "DLJL@123"  // 默认初始密码
		#define PASSWORD_LEN        8
	#else
		#define SETPARAPASSWORD     "000000"  // 设置按键默认初始密码
		#define PASSWORD_STR        "000000"  // 默认初始密码
		#define PASSWORD_LEN        6

	#endif
	

#endif



// 清除统计模块指定计量点的所有私有数据
void ClearStatData(uint8 msn);

char* GetMettypeStr(uint8 nMettypeID);
int GetMettypeIndex(uint8 nMettypeID);
int GetMeterPoint(int nPoint);
BOOL GetPointMeterSn(int  *MetSn,uint8 rmtctrl);
uint16  GetMetSnIndex(char *MetAddr);
int IsBuffAll00_EE_FF(uint8 *Buff, int Len);
int IsBuffAllPointValue(uint8 *Buff, uint8 Value, int n);

BOOL HardwareSafetyKeyIsPressed(void);
inline BOOL IsHardWareSafety(void)  {return (HardwareSafetyKeyIsPressed());}

typedef struct
{		
    uint16	home;      
    uint16	end;     
    uint8	index;
}PCSEG_INFO;  	/*-功控时段信息-*/

void SpeakCtrl(BOOL bOpen);

// 数据字典返回值检查
// 函数名: BufIsInitialized(buf, n)
BOOLEAN BufIsInitialized(void *buf, unsigned int n);
//
// 输入参数:
// void *buf      缓冲区起始地址
// unsigned int n 要检查的字节数,注:n>=0
//
// 返回值:
// 当且仅当buf中n字节内容全0xFF(表示没有被初始化过)时, 函数返回false
// 特殊情况:当输入参数n值等于0时函数必须返回false
// 当buf中任意一字节不等于0xFF时, 函数将认为数据是有效的, 直接返回true
//
// 函数用法范例:
//	int nRet;
//	char buf[100];
//	nRet = ReadXPara(MTR_PARA_TYPE, AMR_USER_METER_CLASS_ID, buf, g_guiComm.nCurPoint);
//	if (nRet<=0 || !BufIsInitialized(buf, (unsigned int)nRet))
//	{
//		printf("ERROR:读取参数失败!");
//	}
//	//注:nRet可以是负数,而函数BufIsInitialized()的输入参数n被定义为unsigned int型仍是合理的


// 函数功能:从数据字典读取保安定值
// 函数名: GetKeepBaseConstant()
double GetKeepBaseConstant(void);
// 返回值: 保安定值(double)
// 输入参数: 无
// 函数用法范例:
// double dKeepBase;
// dKeepBase = GetKeepBaseConstant();

typedef struct _IP_CONFIG
{
	int		nIndex;
	int		nIP;
	int		bEthIndex;
	uint32	bWriteID;		//写入数据字典的ID
	uint8	bEnable;
	char	EthName[10];

	int 	nIpConfigMode;
	int		nNetMask;		//子网掩码
	int		nNetGw;			//网关
	char 	PPPoEUsr[20];	//PPPOE用户名
	char 	PPPoEPwd[20];	//PPPOE密码
}IP_CONFIG;
/*获得系统版本号*/
BOOL commfun_GetSysVersion(char *pBuf);
//void commfun_HexIPToInt(uint8 *pczIp, int *pnIp);

#endif/*_MINIGUI_DEF_H_*/

