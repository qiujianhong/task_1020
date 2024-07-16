/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
  程序名称: main.h
  功    能：FKGA42显示模块框架文件头文件
  开 发 人：gaoyf
  开发时间：2007-1

  版本历史:

    v1.0:
      2006-1-8, gaoyf : New file

***************************************************************************
*/

#ifndef _MAIN_H_
#define _MAIN_H_

#include "miniguidef.h"

// 与主台相关命令定义
#define GUI_PORT_ID				4003	/*定义与MINIGUI进程间通信的端口号*/
#define IPC_SERVICE             2

#define GUI_CMD_POPUP			0x01	/*显示应用命令，指出菜单*/
#define GUI_CMD_KEYCODE			0x81	/*显示应用返回给主调度按键码*/
#define GUI_CMD_OTHER			0x83	/*保留待它用*/
#define	GUI_RD_MTR_SETTING		0x84   //显示应用要求发送中继执行任务
#define	GUI_START_IMCC_COMPRESS	0x85	//启动IMCC压缩
#define	GUI_STOP_IMCC_COMPRESS	0x86	//取消IMCC压缩
#define	GUI_CLEAR_PRARA			0X87	// 参数初始化
#define GUI_CLEAR_AC_METER_DATA 0x89    // 界面交采电表清零
#define GUI_INIT_AC_METER_PARA  0x8A    // 界面交采全面初始化
#define GUI_SET_AC_TIME         0X8B    //界面设置终端时间，也给交采设置下时间
#if PROTOCOL_CHANGE
#define GUI_PROTOCOL_CHANG      0x8C    //规约转换
#endif
#define	PHONE_KEY				0xAA	//通话键键值
#define	RD_METER_KEY			0xBB	//抄表键键值
#define	START_UPGRADE_IR_KEY	0xCC	//红外升级启动按键码
#define	STOP_UPGRADE_IR_KEY		0xDD	//红外升级启动按键码
#define	STOP_PHONE_KEY			0xEE	//停止通话键(消音)

#if TD_SCDMA == 1
#define TDSCDMA_GUI_ID				9			/*显示界面中，td-scdma的ID序列*/
#define TDSCDMA_GSM_GUI_ID			10			/*显示界面中，td-scdma短信信道的ID序列*/
#define IS_GUI_GPRS(ID)		(((ID) == 2) || ((ID) == 6) || ((ID) == TDSCDMA_GUI_ID))	//是否为GPRS或者ETH或者TD_SCDMA设置页面
#define NOT_GUI_GPRS(ID)	(((ID) != 2) && ((ID) != 6) && ((ID) != TDSCDMA_GUI_ID))	//非GPRS或者ETH或者TD_SCDMA设置页面
#else
#define IS_GUI_GPRS(ID)		(((ID) == 2) || ((ID) == 6))	//是否为GPRS或者ETH或者TD_SCDMA设置页面
#define NOT_GUI_GPRS(ID)	(((ID) != 2) && ((ID) != 6))	//非GPRS或者ETH或者TD_SCDMA设置页面
#endif

#ifndef HUDP
#define HUDP void*
#endif

//最后需要统一 jackyben
//#define	DBCENTER_IPADDR "192.168.9.188"
//#define	DBCENTER_PORT	8598

#define  GUI_INI_FILE            "gui.ini"

#define	BLUETOOH_ON		0xaa	//蓝牙开启
#define	BLUETOOH_OFF	0x55	//蓝牙关闭

//#define GUI_DB_CLIENT_NUM	250

/**********菜单项数据结构**********/
#define MENU_CAPTION_LEN	20
typedef struct _PLUG_STRUCT
{
	char	czCaption[MENU_CAPTION_LEN];
	DWORD	dwPlugin;
}PLUG_STRUCT;
typedef struct _MENU_INFO
{
	int		nParent;        // 父菜单项在数组中的索引
	char	czCaption[MENU_CAPTION_LEN];	// 菜单呈现
	BOOL	bMenuAggregate; // 是否包含子菜单

	int		nLeft1;
	int		nLeft2;
	int		nTop;
	int		nInterval;
	int		nMenuMaxCnt;
	BOOL	bVisible;

	DWORD	dwMenuID;
	PLUG_STRUCT* pPlugStruct;
	int		nPlugCnt;       // 如果是子菜单项nPlugCnt=1，否则nPlugCnt>=1

}MENU_INFO;

/*主框架结构定义*/
#define MAX_TURNDISP_NUM	400
typedef struct _GUI_MAIN
{
    int nMenuRootId;            // 0 主菜单风格库Id
	int		nLeft1;
	int		nLeft2;
	int		nTop;
	int		nMenuInterval;
	int		nMenuMaxCnt;

    MENU_INFO*   pPrentMenu;    // NULL，父菜单指针，主要用于多个功能页面时获取其它功能页面
    int          nPageLayerIndex;//0   功能页面的当前索引值

    PLUGIN_INFO* pluginCur;     // NULL 当前的功能页面
    PLUGIN_INFO* plugTemplate;  // NULL 模板(背景库)页面
    PLUGIN_INFO* plugPop;       // NULL 弹出页面
    PLUGIN_INFO* plugMsg;       // NULL MessageBox页面

    PLUGIN_INFO* plugOften;     // NULL 常显页面
    BOOL         bOftenDisp;    // FALSE 是否常显

    BOOL bLighton;              // TRUE 是否亮灯
    int nLightonTime;           // 0    亮灯时间(无按键消息以来)

    int nLightonTimeMax;        // 10   无按键消息的最长亮灯时间

    BOOL bTrunDisp;             // FALSE 是否轮显
    int nTrunDispTime;          // 0    功能页面显示时间(无按键消息以来,即多长时间进入轮显)
    int nTrunDispTimeMax;       // 10   进入轮显的最长时间

    int nTrunDispCnt;           // 0    轮显页面库个数
    int nTrunDispIndex;         // -1   当前轮显页面的索引
    int	nInterval;              // 5    轮显页面之间的间隔时间

}GUI_MAIN;

extern GUI_MAIN g_guiMain;
extern PLUGIN_INFO* g_pluginTrunDisp[MAX_TRUNDISP_CNT];  // 轮显页面库数组
extern int g_nTrunDisp[MAX_TRUNDISP_CNT];  // 轮显页面库数组
extern int g_nTotalTrunDispCnt;

extern MENU_INFO* g_menuMain[1000];	// 菜单项数组，最大菜单项为1000
extern int g_nMenus;                // 所有菜单项个数
extern int g_nParentIndex;          // 父菜单在数组中的索引
extern int g_menuIdx;
extern BOOL g_bpop;

extern PLUGIN_INFO* g_plugin[]; // 所有页面库的数组集合
extern int g_nPluginCnt;        // 包含的页面库总的个数
//extern HUDP g_hUdbCenter;
extern BOOL g_bRunPaint;

extern void delete_app_info(uint32 appid);

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    获取指定页面库指针
	输入参数：    DWORD dwPluginId：页面库ID
	输出参数：
	函数返回值：  PLUGIN_INFO* ：页面库指针，NULL表示不存在
	内部处理流程：
********************************************************************/
PLUGIN_INFO* GetPlugIn(DWORD dwPluginId);
void StopTrunDisp();
void OnPaint();

int set_bluetooth_state(uint8 state);
int get_bluetooth_state(int32 *state);

#endif/*_MAIN_H_*/

