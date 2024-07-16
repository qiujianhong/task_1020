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
#include "pageInterface.h"
//#include "mdlmgr.h"
#include "mdlcommon.h"
#include <time.h>

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
    
//接口类型
const char* interfaceType[] = 
{
    "CDC-ACM",
    "CDC-ECM",
    "HID",
};
//功能配置
const char* pipeFunc[] = 
{
    "管理",
    "远程",
    "PLC/RF",
    "RS-485",
    "遥脉",
    "CAN",
    "M-Bus",
    "控制",
    "模拟量",
    "RS232",
    "未定义",
    "回路",
    "负荷",
};

uint8	gCurMoudleIndex = 0;
int		gModulePageIndex = 0;
void InitmoduleInfo1(DWORD dwPluginId)
{
    gCurMoudleIndex = 0;
    gModulePageIndex = 0;
}

//模块设备型号、模块设备ID、软件版本、软件发布日期、硬件版本、
//硬件发布日期、厂商编码、接口类型、功能配置
void moduleInfoDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;
    
    MODE_ALL_INFO_T* pMoudluInfoAll = moduleInfo_all;
    
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    uint8	validFlag = pMoudluInfoAll[gCurMoudleIndex].modInfoFlag;
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    char  czBuf[300] = {0};
    sprintf(czBuf,"模组%d信息",gCurMoudleIndex+1);
    rc.top = 26;
    rc.bottom = rc.top + 16;
    DATE_T* pDATE = NULL;
    int nTop = rc.top;
    int nLeft1 = 7;
    int nLeft2 = 60;
    //int nLeft3 = 57;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    switch(gModulePageIndex)
    {
    case 0:
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "设备型号:");
        nTop += 16;
        if(validFlag == 0x55)
            TextOut(hdc, nLeft2-30, nTop, pMoudluInfoAll[gCurMoudleIndex].moduleInfo.DevType);
        else
            TextOut(hdc, nLeft2-30, nTop, "--------");
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "设备ID:");
        if(validFlag == 0x55)
            TextOut(hdc, nLeft2, nTop, pMoudluInfoAll[gCurMoudleIndex].moduleInfo.DevID);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
        
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "软件版本:");
        //新老协议兼容
        if (pMoudluInfoAll[gCurMoudleIndex].moduleInfo.type == MDL_PRTL_VER1)
        {
            sprintf(czBuf,"%s", pMoudluInfoAll[gCurMoudleIndex].moduleInfo.softVer);
        }
        else
        {
            sprintf(czBuf,"%08x",pMoudluInfoAll[gCurMoudleIndex].moduleInfo.softVersion);
        }
        if(validFlag == 0x55)
            TextOut(hdc, nLeft2, nTop,czBuf);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
        
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "发布日期:");
        //新老协议兼容
        if (pMoudluInfoAll[gCurMoudleIndex].moduleInfo.type == MDL_PRTL_VER1)
        {
            sprintf(czBuf,"%s", pMoudluInfoAll[gCurMoudleIndex].moduleInfo.softDate);
        }
        else
        {
            pDATE = &pMoudluInfoAll[gCurMoudleIndex].moduleInfo.date_soft;
            sprintf(czBuf,"%d年%d月%d日",pDATE->year,pDATE->month,pDATE->dayofmonth);
        }
        if(validFlag == 0x55)
            TextOut(hdc, nLeft2, nTop,czBuf);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
    break;

    case 1:
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "硬件版本:");
        if (pMoudluInfoAll[gCurMoudleIndex].moduleInfo.type == MDL_PRTL_VER1)
        {
            sprintf(czBuf,"%s",pMoudluInfoAll[gCurMoudleIndex].moduleInfo.hardVer);
        }
        else
        {
            sprintf(czBuf,"%08x",pMoudluInfoAll[gCurMoudleIndex].moduleInfo.hardwVersion);
        }
        if(validFlag == 0x55)
            TextOut(hdc, nLeft2, nTop,czBuf);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "发布日期:");
        if (pMoudluInfoAll[gCurMoudleIndex].moduleInfo.type == MDL_PRTL_VER1)
        {
            sprintf(czBuf,"%s", pMoudluInfoAll[gCurMoudleIndex].moduleInfo.hardDate);
        }
        else
        {
            pDATE = &pMoudluInfoAll[gCurMoudleIndex].moduleInfo.date_hard;
            sprintf(czBuf,"%d年%d月%d日",pDATE->year,pDATE->month,pDATE->dayofmonth);
        }
        if(validFlag == 0x55)
            TextOut(hdc, nLeft2, nTop,czBuf);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
        
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "厂商编码:");
        if(validFlag == 0x55)
            TextOut(hdc, nLeft2, nTop,pMoudluInfoAll[gCurMoudleIndex].moduleInfo.VendorCode);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
    break;

    case 2:
    {
        nTop += 20;
        int pipeCnt = pMoudluInfoAll[gCurMoudleIndex].linkInfo.virPipeCnt;
        int i;
        VIRTURL_PIPE_MODE_T* pVirturlPipe = pMoudluInfoAll[gCurMoudleIndex].linkInfo.virturMode;
        if(validFlag == 0x55)
        {
            for(i = 0; i< pipeCnt; i++)
            {
                sprintf(czBuf,"通道%d:",i);
                TextOut(hdc, nLeft1, nTop, czBuf);
                sprintf(czBuf,"%s %s",interfaceType[pVirturlPipe[i].interf-1],pipeFunc[pVirturlPipe[i].funcConfig]);
                TextOut(hdc, nLeft2, nTop,czBuf);
                nTop += 15;
            }
        }
        else
            TextOut(hdc, nLeft1, nTop, "通道:");

        break;
    }
    default:
    break;
    }
}
BOOL moduleInfoKeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;

    switch(nKey)
    {
        case KEY_UP:
            gModulePageIndex--;
            if(gModulePageIndex < 0)
                gModulePageIndex = 2;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;

        case KEY_DOWN:
            gModulePageIndex++;
            if(gModulePageIndex > 2)
                gModulePageIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;

        default:
            break;	
    }
    return FALSE;
}


void InitmoduleInfo2(DWORD dwPluginId)
{
    gCurMoudleIndex = 1;
    gModulePageIndex = 0;
}

void InitmoduleInfo3(DWORD dwPluginId)
{
    gCurMoudleIndex = 2;
    gModulePageIndex = 0;
}

void InitmoduleInfo4(DWORD dwPluginId)
{
    gCurMoudleIndex = 3;
    gModulePageIndex = 0;
}

void InitmoduleInfo5(DWORD dwPluginId)
{
    gCurMoudleIndex = 4;
    gModulePageIndex = 0;
}

void InitPlugEvent(DWORD dwPluginId)
{
    
}
void PlugEventDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;
    int i;
    PLUG_STATUS_T* pStatus = plug_status;
    
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    char  czBuf[300] = {0};
    sprintf(czBuf,"模组插拔信息");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    int nTop = rc.top;
    int nLeft1 = 7;
    int nLeft2 = 46;
    //int nLeft3 = 57;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);
    nTop += 20;
    struct tm* ptm = NULL;
    for(i = 0; i< mdlnum; i++)
    {
        sprintf(czBuf,"No.%d:",i);
        TextOut(hdc, nLeft1,nTop,czBuf);
        ptm = localtime(&pStatus[i].time);
        if(&pStatus[i].time == 0)
            sprintf(czBuf,"%s --------",(pStatus[i].plugStatus == ON_STATE)?"ON":"OFF");
        else
            sprintf(czBuf,"%s %d.%d.%d %d:%d:%d",(pStatus[i].plugStatus == ON_STATE)?"ON":"OFF",
                ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
        TextOut(hdc, nLeft2,nTop,czBuf);
        nTop += 20;
    }
}

int gAppRestartSel = 0;
void InitappRestart(DWORD dwPluginId)
{
    gAppRestartSel = 0;
}
void appRestartDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;
    
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    char  czBuf[300] = {0};
    sprintf(czBuf,"重启应用");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    int nTop = rc.top;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    nTop = 80;
    int nLeft = 26;
    if(gAppRestartSel == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "取消");
    }

    nLeft = 110;
    if(gAppRestartSel == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "确定");
    }
    
    PopListDraw(hdc);

}
BOOL appRestartKeyMsg(int nKey)
{
    char czBuf[300] = {0};
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;

    switch(nKey)
    {
        case KEY_RIGHT:
            gAppRestartSel++;
            if(gAppRestartSel > 1)
                gAppRestartSel = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case KEY_LEFT:
            gAppRestartSel--;
            if(gAppRestartSel < 0)
                gAppRestartSel = 1;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case KEY_OK:
            if(gAppRestartSel == 0)	//取消
            {
                AppCancelMenu();
            }
            else if(gAppRestartSel == 1)	//确认处理
            {
                if(ValidatePwd() == TRUE)
                {
                    if(FALSE == MessageBox("提示\n是否重启模组\n请再确认一遍", MB_OKCANCEL))
                        return TRUE;
                    else
                    {
                        sprintf(czBuf,"sudo pkill mapManager");
                        system(czBuf);
                        //程序杀掉，等系统管理器拉起
                    }
                }
            }
            return TRUE;

            break;
        default:
        break;
    }
    return FALSE;
}

const char* moduleSel[] =
{
    "槽位1",
    "槽位2",
    "槽位3",
    "槽位4",
    "槽位5",
};

int gModuleRestartSel = 0;
int gModuleRestartPageIndex = 0;
void InitmoduleRestart(DWORD dwPluginId)
{
    gModuleRestartSel = 0;				//弹出模组选中
    gModuleRestartPageIndex = 0;		//当前页面索引
}
void moduleRestartDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;	
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    char  czBuf[300] = {0};
    sprintf(czBuf,"模组重启");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    int nTop = rc.top;
    int nLeft1 = 7;
    int nLeft2 = 60;
    //int nLeft3 = 57;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);
    //int nIndex = 0;
    nTop += 40;
    //TextOut(hdc, nLeft1,nTop,"模组");

    if(gModuleRestartPageIndex == 0)
    {
        ITEM_SELECT;
        sprintf(czBuf,"槽位%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        sprintf(czBuf,"槽位%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
    }
    nLeft1 = 26;
    nTop += 40;
    if(gModuleRestartPageIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "取消");
    }

    nLeft1 = 110;
    if(gModuleRestartPageIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "确定");
    }
    
    PopListDraw(hdc);
}
BOOL moduleRestartKeyMsg(int nKey)
{
    char czBuf[300] = {0};
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;
    
    //int ret = 0;
    //int ret2 = 0;
    
    if(pg_guicomm->bPopList == TRUE)
    {
        int nSel;
        //char czBuf[50];
        //int nRet;
        PopListKeyMsg(nKey, &nSel);
        if(nKey == KEY_OK)
        {
           if(gModuleRestartPageIndex == 0)
            {
                gModuleRestartSel = nSel;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch(nKey)
    {
        case KEY_UP:
            gModuleRestartPageIndex--;
            if(gModuleRestartPageIndex < 0)
                gModuleRestartPageIndex = 2;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;

        case KEY_DOWN:
            gModuleRestartPageIndex++;
            if(gModuleRestartPageIndex > 2)
                gModuleRestartPageIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;

        case KEY_RIGHT:
            gModuleRestartPageIndex++;
            if(gModuleRestartPageIndex > 2)
                gModuleRestartPageIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;

        case KEY_LEFT:
            gModuleRestartPageIndex--;
            if(gModuleRestartPageIndex < 0)
                gModuleRestartPageIndex = 2;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case KEY_OK:
            if(gModuleRestartPageIndex == 0)
                PopList(moduleSel, 5, moduleSel[gModuleRestartSel], 82, 42, FALSE);
            else if(gModuleRestartPageIndex == 1)	//取消
            {
                AppCancelMenu();
            }
            else if(gModuleRestartPageIndex == 2)	//确认处理
            {
                if(ValidatePwd() == TRUE)
                {
                    if(FALSE == MessageBox("提示\n是否重启模组\n请再确认一遍", MB_OKCANCEL))
                        return TRUE;
                    else
                    {
                        if(ModulePowerOff(gModuleRestartSel+1,0) < 0 )
                        {
                            printf("模组 %d 重启失败! \n",gModuleRestartSel+1);
                            return FALSE;
                        }
                        modPowerCtl[gModuleRestartSel].offpowerflag = 1;
                        modPowerCtl[gModuleRestartSel].offpowertime = 10;
                        sprintf(czBuf,"模组%d断电,%d秒后重启\n",gModuleRestartSel+1,(int)modPowerCtl[gModuleRestartSel].offpowertime);
                        MessageBox(czBuf);
                    }
                }
            }
            return TRUE;
        default:
        break;
    }
    return FALSE;
}

void InitmoduleReset(DWORD dwPluginId)
{
    
}
void moduleResetDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;	
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    char  czBuf[300] = {0};
    sprintf(czBuf,"模组复位");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    int nTop = rc.top;
    int nLeft1 = 7;
    int nLeft2 = 60;
    //int nLeft3 = 57;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);
    //int nIndex = 0;
    nTop += 40;
    //TextOut(hdc, nLeft1,nTop,"模组");

    if(gModuleRestartPageIndex == 0)
    {
        ITEM_SELECT;
        sprintf(czBuf,"槽位%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        sprintf(czBuf,"槽位%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
    }
    nLeft1 = 26;
    nTop += 40;
    if(gModuleRestartPageIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "取消");
    }

    nLeft1 = 110;
    if(gModuleRestartPageIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "确定");
    }
    
    PopListDraw(hdc);
}
BOOL moduleResetKeyMsg(int nKey)
{
    return FALSE;
}

void InitdataInitilize(DWORD dwPluginId)
{
    gModuleRestartSel = 0;				//弹出模组选中
    gModuleRestartPageIndex = 0;		//当前页面索引
}
void dataInitilizeDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;	
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    char  czBuf[300] = {0};
    sprintf(czBuf,"数据初始化");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    int nTop = rc.top;
    int nLeft1 = 7;
    int nLeft2 = 60;
    //int nLeft3 = 57;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);
    //int nIndex = 0;
    nTop += 40;
    //TextOut(hdc, nLeft1,nTop,"模组");

    if(gModuleRestartPageIndex == 0)
    {
        ITEM_SELECT;
        sprintf(czBuf,"槽位%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        sprintf(czBuf,"槽位%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
    }
    nLeft1 = 26;
    nTop += 40;
    if(gModuleRestartPageIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "取消");
    }

    nLeft1 = 110;
    if(gModuleRestartPageIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "确定");
    }
    
    PopListDraw(hdc);
}
BOOL dataInitilizeKeyMsg(int nKey)
{
    char czBuf[300] = {0};
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;
    
    //int ret = 0;
    //int ret2 = 0;
    
    if(pg_guicomm->bPopList == TRUE)
    {
        int nSel;
        //char czBuf[50];
        //int nRet;
        PopListKeyMsg(nKey, &nSel);
        if(nKey == KEY_OK)
        {
           if(gModuleRestartPageIndex == 0)
            {
                gModuleRestartSel = nSel;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch(nKey)
    {
        case KEY_UP:
            gModuleRestartPageIndex--;
            if(gModuleRestartPageIndex < 0)
                gModuleRestartPageIndex = 2;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;

        case KEY_DOWN:
            gModuleRestartPageIndex++;
            if(gModuleRestartPageIndex > 2)
                gModuleRestartPageIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;

        case KEY_RIGHT:
            gModuleRestartPageIndex++;
            if(gModuleRestartPageIndex > 2)
                gModuleRestartPageIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;

        case KEY_LEFT:
            gModuleRestartPageIndex--;
            if(gModuleRestartPageIndex < 0)
                gModuleRestartPageIndex = 2;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case KEY_OK:
            if(gModuleRestartPageIndex == 0)
                PopList(moduleSel, 5, moduleSel[gModuleRestartSel], 82, 42, FALSE);
            else if(gModuleRestartPageIndex == 1)	//取消
            {
                AppCancelMenu();
            }
            else if(gModuleRestartPageIndex == 2)	//确认处理
            {
                if(ValidatePwd() == TRUE)
                {
                    if(FALSE == MessageBox("提示\n是否数据初始化模组\n请再确认一遍", MB_OKCANCEL))
                        return TRUE;
                    else
                    {
//						if(ModulePowerOff(gModuleRestartSel+1,0) < 0 )
                        {
//							printf("模组 %d 数据初始化失败! \n",gModuleRestartSel+1);
//							return FALSE;
                        }
                        sprintf(czBuf,"模组%d 数据初始化成功\n",gModuleRestartSel+1);
                        MessageBox(czBuf);
                    }
                }
            }
            return TRUE;
        default:
        break;
    }
    return FALSE;
}

void InitparaInitilize(DWORD dwPluginId)
{
    gModuleRestartSel = 0;				//弹出模组选中
    gModuleRestartPageIndex = 0;		//当前页面索引
}
void paraInitilizeDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;	
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    char  czBuf[300] = {0};
    sprintf(czBuf,"参数初始化");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    int nTop = rc.top;
    int nLeft1 = 7;
    int nLeft2 = 60;
    //int nLeft3 = 57;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);
    //int nIndex = 0;
    nTop += 40;
    //TextOut(hdc, nLeft1,nTop,"模组");

    if(gModuleRestartPageIndex == 0)
    {
        ITEM_SELECT;
        sprintf(czBuf,"槽位%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        sprintf(czBuf,"槽位%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
    }
    nLeft1 = 26;
    nTop += 40;
    if(gModuleRestartPageIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "取消");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "取消");
    }

    nLeft1 = 110;
    if(gModuleRestartPageIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "确定");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "确定");
    }
    
    PopListDraw(hdc);
}
BOOL paraInitilizeKeyMsg(int nKey)
{
    char czBuf[300] = {0};
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return FALSE;
    
    //int ret = 0;
    //int ret2 = 0;
    
    if(pg_guicomm->bPopList == TRUE)
    {
        int nSel;
        //char czBuf[50];
        //int nRet;
        PopListKeyMsg(nKey, &nSel);
        if(nKey == KEY_OK)
        {
           if(gModuleRestartPageIndex == 0)
            {
                gModuleRestartSel = nSel;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch(nKey)
    {
        case KEY_UP:
            gModuleRestartPageIndex--;
            if(gModuleRestartPageIndex < 0)
                gModuleRestartPageIndex = 2;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;

        case KEY_DOWN:
            gModuleRestartPageIndex++;
            if(gModuleRestartPageIndex > 2)
                gModuleRestartPageIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;

        case KEY_RIGHT:
            gModuleRestartPageIndex++;
            if(gModuleRestartPageIndex > 2)
                gModuleRestartPageIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;

        case KEY_LEFT:
            gModuleRestartPageIndex--;
            if(gModuleRestartPageIndex < 0)
                gModuleRestartPageIndex = 2;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case KEY_OK:
            if(gModuleRestartPageIndex == 0)
                PopList(moduleSel, 5, moduleSel[gModuleRestartSel], 82, 42, FALSE);
            else if(gModuleRestartPageIndex == 1)	//取消
            {
                AppCancelMenu();
            }
            else if(gModuleRestartPageIndex == 2)	//确认处理
            {
                if(ValidatePwd() == TRUE)
                {
                    if(FALSE == MessageBox("提示\n是否参数初始化模组\n请再确认一遍", MB_OKCANCEL))
                        return TRUE;
                    else
                    {
//						if(ModulePowerOff(gModuleRestartSel+1,0) < 0 )
                        {
//							printf("模组 %d 参数初始化失败! \n",gModuleRestartSel+1);
//							return FALSE;
                        }
                        sprintf(czBuf,"模组 %d 参数初始化成功\n",gModuleRestartSel+1);
                        MessageBox(czBuf);
                    }
                }
            }
            return TRUE;
        default:
        break;
    }
    return FALSE;
}

void InitappVersion(DWORD dwPluginId)
{
    
}

/**********************************************************************
* @name      : ble_stoi
* @brief     ：字符串转整形
* @param[in] ：str  字符串
* @param[out]：
* @return    ：整形
**********************************************************************/
int map_stoi(const char *str)
{
    assert(str != NULL);//判空断言，与函数实现无关
    if(str==NULL)
        return 0;

    int tmp=0;
    while(*str !='\0')
    {
        if(*str>'0' && *str<='9')//if(isdigit(*str)) 推荐使用函数isdigit,检查字符是否为字符‘0’~~‘9’
        {
            tmp=tmp*10+(*str -'0');//获得数字并“叠加”
        }
        str++;//指针后移
    }

    return tmp;
}

void appVersionDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if(pg_guicomm == NULL)
        return;

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;
    

    char czBuf[64]={0};

    int nLeft = 5;
    int nLeft2 = 15;
    int nTop = 20;
    int tmpTop = 20;
    
    // 软件版本号
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "软件版本号");

    nTop += tmpTop;
    sprintf(czBuf, "%u", map_stoi(APP_VERSION));
    TextOut(hdc, nLeft2, nTop, czBuf);

    
    // 发布时间
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "发布时间");
    nTop += tmpTop;
    snprintf(czBuf, 64, "%s", APP_PUBLISH_TIME);
    TextOut(hdc, nLeft2, nTop, czBuf);
}
BOOL appVersionKeyMsg(int nKey)
{
    return FALSE;
}

PLUGIN_INFO g_moduleInfo1 = {
    0x00A00010/*id*/, 
    InitmoduleInfo1/*InitLayer*/,
    NULL/*UninitLayer*/,
    moduleInfoDraw/*Draw*/,
    NULL/*Timer*/,
    moduleInfoKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_moduleInfo2 = {
    0x00A00020/*id*/, 
    InitmoduleInfo2/*InitLayer*/,
    NULL/*UninitLayer*/,
    moduleInfoDraw/*Draw*/,
    NULL/*Timer*/,
    moduleInfoKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_moduleInfo3 = {
    0x00A00030/*id*/, 
    InitmoduleInfo3/*InitLayer*/,
    NULL/*UninitLayer*/,
    moduleInfoDraw/*Draw*/,
    NULL/*Timer*/,
    moduleInfoKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_moduleInfo4 = {
    0x00A00040/*id*/, 
    InitmoduleInfo4/*InitLayer*/,
    NULL/*UninitLayer*/,
    moduleInfoDraw/*Draw*/,
    NULL/*Timer*/,
    moduleInfoKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_moduleInfo5 = {
    0x00A00050/*id*/, 
    InitmoduleInfo5/*InitLayer*/,
    NULL/*UninitLayer*/,
    moduleInfoDraw/*Draw*/,
    NULL/*Timer*/,
    moduleInfoKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_modulePlugEvent = {
    0x00A00060/*id*/, 
    InitPlugEvent/*InitLayer*/,
    NULL/*UninitLayer*/,
    PlugEventDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_appRestart = {
    0x00A00070/*id*/, 
    InitappRestart/*InitLayer*/,
    NULL/*UninitLayer*/,
    appRestartDraw/*Draw*/,
    NULL/*Timer*/,
    appRestartKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_moduleRestart = {
    0x00A00080/*id*/, 
    InitmoduleRestart/*InitLayer*/,
    NULL/*UninitLayer*/,
    moduleRestartDraw/*Draw*/,
    NULL/*Timer*/,
    moduleRestartKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_moduleReset = {
    0x00A00090/*id*/, 
    InitmoduleRestart/*InitLayer*/,
    NULL/*UninitLayer*/,
    moduleResetDraw/*Draw*/,
    NULL/*Timer*/,
    moduleResetKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_dataInitilize = {
    0x00A000A0/*id*/, 
    InitdataInitilize/*InitLayer*/,
    NULL/*UninitLayer*/,
    dataInitilizeDraw/*Draw*/,
    NULL/*Timer*/,
    dataInitilizeKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_paraInitilize = {
    0x00A000B0/*id*/, 
    InitparaInitilize/*InitLayer*/,
    NULL/*UninitLayer*/,
    paraInitilizeDraw/*Draw*/,
    NULL/*Timer*/,
    paraInitilizeKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_appVersionInfo = {
    0x00A000C0/*id*/, 
    InitappVersion/*InitLayer*/,
    NULL/*UninitLayer*/,
    appVersionDraw/*Draw*/,
    NULL/*Timer*/,
    appVersionKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

