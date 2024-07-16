#ifndef PAGEINTERFACE_H
#define PAGEINTERFACE_H

#include "miniguidef.h"
//#include "layer.h"

#ifdef __cplusplus
extern "C" {
#endif

extern     PLUGIN_INFO**    pG_plugin;
extern     uint16            pluginCnt;


extern     int AppPluginReg(PLUGIN_INFO** pg_plugin, uint16 cnt);
extern    GUI_COMM * AppChkGuiComm();
extern     void AppCancelKeyDraw(HDC hdc, int x, int y, int w, int h);
extern     BOOL AppMessageBox(const char* pczMsg);
extern     void AppCancelMenu();
extern    void AppMenuDraw(HDC hdc);
extern    BOOL AppMenuKeyMsg(int nKey);



#ifdef __cplusplus
}

#endif

#endif

