/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
  程序名称: configcontrol.cpp
  功    能：完成终端数据配置页面库
  开 发 人：gaoyf
  开发时间：2007-1

  版本历史:

    v1.0:
      2006-1-20, gaoyf : New file

  包含页面库：
    终端地址（含行政区码）  0x00630000
    终端当前主备通道信息    0x00660000


***************************************************************************
*/

#include <sys/stat.h>
#include "miniguidef.h"
#include "commdef.h"
#include "main.h"
#include "commfun.h"
#include "dataType.h"
#include "appmsg_struct.h"
//#include "deskgui_cfg.h"
#include "pageInterface.h"
#include "gui_c_prtl.h"


const char* g_pczRadioDelay[] = {"100", "200", "300", "400", "500", "800", "1000", "2000"};

int RebootTerminal();
void MenuDraw_add_name(HDC hdc,const char *pname);

void MenuDraw(HDC hdc)
{
    MenuDraw_add_name(hdc, ADV_SHOW_NAME);
}

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


