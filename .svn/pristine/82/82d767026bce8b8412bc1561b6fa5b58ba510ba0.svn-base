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

#include "lc_gui.h"

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

typedef struct
{
	OOP_WORKINFO_T         workstatus;      //工况信息 0x40400300
	OOP_LOOPSTAT_T         loopstatus;      //电流回路状态 0x20500200
	OOP_ABNORMALCTLOG_T    abnormallclog;   //电流互感器异常事件 0x31200200
}LC_DATA_INFO;

LC_DATA_INFO g_lc_data_info;

extern OOP_CT_VERTIONS_T g_ctVer;

int read_db_noamal_data(DB_CLIENT fp, uint32 oad_value, uint8 logic_id, uint16 info_num, uint8* pbuf, uint32 buf_len)
{
	int ret = 0;
	uint32 len = 0;
	NOMAL_OAD_T nomalOAD = { 0 };

	nomalOAD.logicId = logic_id;
	nomalOAD.infoNum = info_num;
	nomalOAD.oad.value = oad_value;
	ret = db_read_nomal(fp, &nomalOAD, buf_len, pbuf, &len);
	if (ret != ERR_OK || len != buf_len)
	{
		GUI_FMT_DEBUG("db_read_nomal  err ret=%d, recNum=%d!\n", ret, len);
		return -1;
	}

	return 0;
}

int g_lcstatus_drawpage_index;
void CurLoopStatusInit(DWORD dwPluginId)
{
    int db_client = lc_get_dbClient();

    memset(&g_lc_data_info.workstatus, 0, sizeof(OOP_WORKINFO_T));
    read_db_noamal_data(db_client, 0x40400300, 0, 0, (uint8*)&g_lc_data_info.workstatus, sizeof(OOP_WORKINFO_T));

    g_lcstatus_drawpage_index = 0;
}

//电流回路状态
void CurLoopStatusdraw(HDC hdc)
{
//	SelectFont(hdc, g_guiComm.fontBig);
	ITEM_NOT_SELECT;

	char czBuf[64] = { 0 };

	int nLeft = 0;
	int nTop = 0;
	int tmpTop = 18;
    int db_client = lc_get_dbClient();

	RECT rc;
	rc.left = 0;
	rc.right = g_guiComm.nWidth;
	rc.top = 18;
	rc.bottom = rc.top + 16;
//	sprintf(czBuf, "电流回路状态");
//	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
	SelectFont(hdc, g_guiComm.fontSmall);

    memset(&g_lc_data_info.workstatus, 0, sizeof(OOP_WORKINFO_T));
    read_db_noamal_data(db_client, 0x40400300, 0, 0, (uint8*)&g_lc_data_info.workstatus, sizeof(OOP_WORKINFO_T));

    nTop = 0;

	// 回路状态
	for (int i = 0; i < 3; i++)
	{
        if(g_lcstatus_drawpage_index != i)
        {
            continue;
        }
        
        nTop += tmpTop;
        nLeft = 0;
        
		TextOut(hdc, nLeft, nTop, "<-");
		switch (i) {
		case 0:TextOut(hdc, nLeft+18, nTop, "A"); break;
		case 1:TextOut(hdc, nLeft+18, nTop, "B"); break;
		case 2:TextOut(hdc, nLeft+18, nTop, "C"); break;
		default:ASSERT(FALSE);
		}
		TextOut(hdc, nLeft+26, nTop, "相");

		if (g_lc_data_info.workstatus.nNum == 0)
		{
			sprintf(czBuf, "------------ ");
		}
		else
		{
            switch(g_lc_data_info.workstatus.item[i].curloopstat)
            {
                case 0: sprintf(czBuf, "正常"); break;
                case 1: sprintf(czBuf, "短路"); break;
                case 2: sprintf(czBuf, "开路"); break;
                case 3: sprintf(czBuf, "二次串入整流设备"); break;
                default:sprintf(czBuf, "error stat %d", g_lc_data_info.workstatus.item[i].curloopstat);
            }
		}

//        DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
        TextOut(hdc, nLeft+42, nTop, czBuf);
		TextOut(hdc, nLeft+148, nTop, "->");

        nLeft = 0;
        nTop += tmpTop;

        MoveTo(hdc, 0,nTop-4);
        LineTo(hdc, 160,nTop-4);

        MoveTo(hdc, nLeft,nTop-4);        
        LineTo(hdc, nLeft,nTop+tmpTop-4);

        sprintf(czBuf, "电流 %.3fA", g_lc_data_info.workstatus.item[i].  curvalid*1.0/1000);
        TextOut(hdc, nLeft+2, nTop, czBuf);

        MoveTo(hdc, nLeft+78,nTop-4);        
        LineTo(hdc, nLeft+78,nTop+tmpTop-4);

        sprintf(czBuf, "温度 %.1f℃", g_lc_data_info.workstatus.item[i].  temp*1.0/10);
        TextOut(hdc, nLeft+80, nTop, czBuf);

        MoveTo(hdc, nLeft+158,nTop-4);        
        LineTo(hdc, nLeft+158,nTop+tmpTop-4);
        
        nLeft = 0;
        nTop += tmpTop;

        MoveTo(hdc, 0,nTop-4);
        LineTo(hdc, 160,nTop-4);
        
        MoveTo(hdc, nLeft,nTop-4);        
        LineTo(hdc, nLeft,nTop+tmpTop-4);
        
        sprintf(czBuf, "频率 %d~%d Hz", g_lc_data_info.workstatus.item[i].minfreq, g_lc_data_info.workstatus.item[i].maxfreq);
        TextOut(hdc, nLeft+2, nTop, czBuf);

        MoveTo(hdc, nLeft+158,nTop-4);        
        LineTo(hdc, nLeft+158,nTop+tmpTop-4);

        nLeft = 0;
        nTop += tmpTop;

        MoveTo(hdc, 0,nTop-4);
        LineTo(hdc, 160,nTop-4);

        //sprintf(czBuf, "阻抗   频率   幅值   相角");
        //TextOut(hdc, nLeft, nTop, "|");
        
        MoveTo(hdc, nLeft,nTop-4);        
        LineTo(hdc, nLeft,nTop+tmpTop-4);
        
        TextOut(hdc, nLeft+2, nTop, "阻抗");
        MoveTo(hdc, nLeft+28,nTop-4);        
        LineTo(hdc, nLeft+28,nTop+tmpTop-4);
        TextOut(hdc, nLeft+30, nTop, "频率kHz");
        MoveTo(hdc, nLeft+70,nTop-4);        
        LineTo(hdc, nLeft+70,nTop+tmpTop-4);
        TextOut(hdc, nLeft+72, nTop, "幅值Ω");
        MoveTo(hdc, nLeft+123,nTop-4);        
        LineTo(hdc, nLeft+123,nTop+tmpTop-4);
        TextOut(hdc, nLeft+125, nTop, "相角°");
        
        MoveTo(hdc, nLeft+158,nTop-4);        
        LineTo(hdc, nLeft+158,nTop+tmpTop-4);

        nLeft = 0;
        nTop += tmpTop;

        MoveTo(hdc, 0,nTop-4);
        LineTo(hdc, 160,nTop-4);
        
        MoveTo(hdc, nLeft,nTop-4);        
        LineTo(hdc, nLeft,nTop+tmpTop-4);

        TextOut(hdc, nLeft+4, nTop, "1");
        MoveTo(hdc, nLeft+28,nTop-4);        
        LineTo(hdc, nLeft+28,nTop+tmpTop-4);
        sprintf(czBuf, "%2d", g_lc_data_info.workstatus.item[i].impfrq.impefreq[0]/1000);
        TextOut(hdc, nLeft+30, nTop, czBuf);
        MoveTo(hdc, nLeft+70,nTop-4);        
        LineTo(hdc, nLeft+70,nTop+tmpTop-4);
        sprintf(czBuf, "%0.2f", g_lc_data_info.workstatus.item[i].impval.impeval[0]*1.0/100);
        TextOut(hdc, nLeft+72, nTop, czBuf);
        MoveTo(hdc, nLeft+123,nTop-4);        
        LineTo(hdc, nLeft+123,nTop+tmpTop-4);
        sprintf(czBuf, "%0.1f", g_lc_data_info.workstatus.item[i].impang.impeang[0]*1.0/10);
        TextOut(hdc, nLeft+125, nTop, czBuf);
        MoveTo(hdc, nLeft+158,nTop-4);        
        LineTo(hdc, nLeft+158,nTop+tmpTop-4);

        nLeft = 0;
        nTop += tmpTop;

        MoveTo(hdc, 0,nTop-4);
        LineTo(hdc, 160,nTop-4);

        MoveTo(hdc, nLeft,nTop-4);        
        LineTo(hdc, nLeft,nTop+tmpTop-4);

        TextOut(hdc, nLeft+4, nTop, "2");
        MoveTo(hdc, nLeft+28,nTop-4);        
        LineTo(hdc, nLeft+28,nTop+tmpTop-4);
        sprintf(czBuf, "%2d", g_lc_data_info.workstatus.item[i].impfrq.impefreq[1]/1000);
        TextOut(hdc, nLeft+30, nTop, czBuf);
        MoveTo(hdc, nLeft+70,nTop-4);        
        LineTo(hdc, nLeft+70,nTop+tmpTop-4);
        sprintf(czBuf, "%0.2f", g_lc_data_info.workstatus.item[i].impval.impeval[1]*1.0/100);
        TextOut(hdc, nLeft+72, nTop, czBuf);
        MoveTo(hdc, nLeft+123,nTop-4);        
        LineTo(hdc, nLeft+123,nTop+tmpTop-4);
        sprintf(czBuf, "%0.1f", g_lc_data_info.workstatus.item[i].impang.impeang[1]*1.0/10);
        TextOut(hdc, nLeft+125, nTop, czBuf);
        MoveTo(hdc, nLeft+158,nTop-4);        
        LineTo(hdc, nLeft+158,nTop+tmpTop-4);

        nLeft = 0;
        nTop += tmpTop;

        MoveTo(hdc, 0,nTop-4);
        LineTo(hdc, 160,nTop-4);

        MoveTo(hdc, nLeft,nTop-4);        
        LineTo(hdc, nLeft,nTop+tmpTop-4);

        TextOut(hdc, nLeft+4, nTop, "3");
        MoveTo(hdc, nLeft+28,nTop-4);        
        LineTo(hdc, nLeft+28,nTop+tmpTop-4);
        sprintf(czBuf, "%2d", g_lc_data_info.workstatus.item[i].impfrq.impefreq[2]/1000);
        TextOut(hdc, nLeft+30, nTop, czBuf);
        MoveTo(hdc, nLeft+70,nTop-4);        
        LineTo(hdc, nLeft+70,nTop+tmpTop-4);
        sprintf(czBuf, "%0.2f", g_lc_data_info.workstatus.item[i].impval.impeval[2]*1.0/100);
        TextOut(hdc, nLeft+72, nTop, czBuf);
        MoveTo(hdc, nLeft+123,nTop-4);        
        LineTo(hdc, nLeft+123,nTop+tmpTop-4);
        sprintf(czBuf, "%0.1f", g_lc_data_info.workstatus.item[i].impang.impeang[2]*1.0/10);
        TextOut(hdc, nLeft+125, nTop, czBuf);
        MoveTo(hdc, nLeft+158,nTop-4);        
        LineTo(hdc, nLeft+158,nTop+tmpTop-4);

        MoveTo(hdc, 0,nTop+tmpTop-4);
        LineTo(hdc, 160,nTop+tmpTop-4);
//
//        sprintf(czBuf, "阻抗1 %2dkHz %0.2fΩ %0.1f°", g_lc_data_info.workstatus.item[i].impfrq.impefreq[0]/1000, g_lc_data_info.workstatus.item[i].impval.impeval[0]*1.0/100, g_lc_data_info.workstatus.item[i].impang.impeang[0]*1.0/10);
//        TextOut(hdc, nLeft, nTop, czBuf);
//
//        nLeft = 0;
//        nTop += tmpTop;
//        
//        sprintf(czBuf, "阻抗2 %2dkHz %0.2fΩ %0.1f°", g_lc_data_info.workstatus.item[i].impfrq.impefreq[1]/1000, g_lc_data_info.workstatus.item[i].impval.impeval[1]*1.0/100, g_lc_data_info.workstatus.item[i].impang.impeang[1]*1.0/10);
//        TextOut(hdc, nLeft, nTop, czBuf);
//
//        nLeft = 0;
//        nTop += tmpTop;
//        
//        sprintf(czBuf, "阻抗3 %2dkHz %0.2fΩ %0.1f°", g_lc_data_info.workstatus.item[i].impfrq.impefreq[2]/1000, g_lc_data_info.workstatus.item[i].impval.impeval[2]*1.0/100, g_lc_data_info.workstatus.item[i].impang.impeang[2]*1.0/10);
//        TextOut(hdc, nLeft, nTop, czBuf);
	}
}

BOOL CurLoopStatusKeyMsg(int nKey)
{
	switch (nKey)
	{
	case KEY_RIGHT:
	{
		g_lcstatus_drawpage_index++;
		if (g_lcstatus_drawpage_index > 2)
		{
			g_lcstatus_drawpage_index = 0;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return  TRUE;

	}
	case KEY_LEFT:
	{
		g_lcstatus_drawpage_index--;
		if (g_lcstatus_drawpage_index < 0)
		{
			g_lcstatus_drawpage_index = 2;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	}
	default:
		return FALSE;
	}

}


//电流回路状态
PLUGIN_INFO g_layerLcStatus =
{
    0x11110000/*id*/,
    CurLoopStatusInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    CurLoopStatusdraw/*Draw*/,
    NULL/*Timer*/,
    CurLoopStatusKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

extern LC_EVT_FIFO_T g_ctEvt;
int g_hisstatus_drawpage_index;
void HisLoopStatusInit(DWORD dwPluginId)
{
//    int i = 0;
//
//    g_ctEvt.nCount = 10;
//
//    for(i = 0; i < g_ctEvt.nCount; i++)
//    {
//        g_ctEvt.fifo[i].tmStart.year = 2020+i;
//        g_ctEvt.fifo[i].tmStart.month = 8;
//        g_ctEvt.fifo[i].tmStart.day = 8;
//        g_ctEvt.fifo[i].tmStart.hour = 20;
//        g_ctEvt.fifo[i].tmStart.minute =20;
//        g_ctEvt.fifo[i].tmStart.second = 30;
//
//        g_ctEvt.fifo[i].abnmlflg.nValue = 0x27;
//    }
    
    g_hisstatus_drawpage_index = 0;
}

//电流回路状态
void HisLoopStatusdraw(HDC hdc)
{
	SelectFont(hdc, g_guiComm.fontSmall);
	ITEM_NOT_SELECT;

	char czBuf[64] = { 0 };

	int nLeft = 0;
	int nTop = 26;
	int tmpTop = 18;
    uint8 tmp = 0;

	RECT rc;
	rc.left = 0;
	rc.right = g_guiComm.nWidth;
	rc.top = 18;
	rc.bottom = rc.top + 16;
	sprintf(czBuf, "历史电流回路状态");
	DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

	// 回路状态
	for (int i = 0; i < g_ctEvt.nCount; i++)
	{
        if(g_hisstatus_drawpage_index + i != g_ctEvt.nCount-1)
        {
            continue;
        }
        
        nLeft = 0;
		nTop += tmpTop;

        sprintf(czBuf, "事件 %d", i);
        TextOut(hdc, nLeft, nTop, czBuf);

        nTop += tmpTop;

        sprintf(czBuf, "时间 %4d-%02d-%02d %02d:%02d:%02d", 
            g_ctEvt.fifo[i].tmStart.year, g_ctEvt.fifo[i].tmStart.month,g_ctEvt.fifo[i].tmStart.day,
            g_ctEvt.fifo[i].tmStart.hour, g_ctEvt.fifo[i].tmStart.minute,g_ctEvt.fifo[i].tmStart.second);

		TextOut(hdc, nLeft, nTop, czBuf);


        tmp = g_ctEvt.fifo[i].abnmlflg.nValue;

        for(int j = 0; j < 3; j++)
        {
            if(tmp & 0x01)
            {
                nTop += tmpTop;
                
                switch(j)
                {
                    case 0: TextOut(hdc, nLeft, nTop, "A相");break;
                    case 1: TextOut(hdc, nLeft, nTop, "B相");break;
                    case 2: TextOut(hdc, nLeft, nTop, "C相");break;
                    default:ASSERT(FALSE);
                }

                switch((g_ctEvt.fifo[i].abnmlflg.nValue & 0xe0) >>5)
                {
                    case 0: sprintf(czBuf, "正常"); break;
                    case 4: sprintf(czBuf, "短路"); break;
                    case 6: sprintf(czBuf, "开路"); break;
                    case 1: sprintf(czBuf, "二次串入整流设备"); break;
                    default:sprintf(czBuf, "error stat %d", (g_ctEvt.fifo[i].abnmlflg.nValue & 0xe0) >>5);
                }

                TextOut(hdc, nLeft+40, nTop, czBuf);
            }

            tmp >>= 1;
        }
	}
}

BOOL HisLoopStatusKeyMsg(int nKey)
{
	switch (nKey)
	{
	case KEY_DOWN:
	{
		g_hisstatus_drawpage_index++;
		if (g_hisstatus_drawpage_index > 9)
		{
			g_hisstatus_drawpage_index = 0;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return  TRUE;

	}
	case KEY_UP:
	{
		g_hisstatus_drawpage_index--;
		if (g_hisstatus_drawpage_index < 0)
		{
			g_hisstatus_drawpage_index = 9;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	}
	default:
		return FALSE;
	}

}

//历史电流回路状态
PLUGIN_INFO g_layerHisStatus =
{
    0x18110000/*id*/,
    HisLoopStatusInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    HisLoopStatusdraw/*Draw*/,
    NULL/*Timer*/,
    HisLoopStatusKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

int g_nTRestartAPPIndex = 0;
void RestartAPPInit(DWORD dwPluginId)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

    g_nTRestartAPPIndex = 0;
    pg_guicomm->bCanTrunDisp = FALSE;
}
void RestartAPPDraw(HDC hdc)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top+16;
    const char* pcz = "重启应用";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if(g_nTRestartAPPIndex == 0)
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
    if(g_nTRestartAPPIndex == 1)
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

BOOL RestartAPPKeyMsg(int nKey)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
        g_nTRestartAPPIndex --;
        if(g_nTRestartAPPIndex < 0)
        {
            g_nTRestartAPPIndex = 1;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_nTRestartAPPIndex ++;
        if(g_nTRestartAPPIndex > 1)
        {
            g_nTRestartAPPIndex = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch(g_nTRestartAPPIndex)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
 //           if(ValidatePwd() == TRUE)
            {
                if(FALSE == MessageBox("提示\n是否重起应用\n请再确认一遍", MB_OKCANCEL))
                {
                    return TRUE;
                }
                g_nTRestartAPPIndex = 0;
                PopMsg("正在重起应用...");

                lc_app_stop();
            }
            break;
        }
        return TRUE;
    }
    return FALSE;
}

//重启应用
PLUGIN_INFO g_layerRestartAPP =
{
    0x12110000/*id*/,
    RestartAPPInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    RestartAPPDraw/*Draw*/,
    NULL/*Timer*/,
    RestartAPPKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

void VersionInfoInit(DWORD dwPluginId)
{
}

void VersionInfoDraw(HDC hdc)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

    SelectFont(hdc,pg_guicomm->fontSmall);
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
    sprintf(czBuf, "%s", APP_VERSION);
    TextOut(hdc, nLeft2, nTop, czBuf);

    
    // 发布时间
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "发布时间");
    nTop += tmpTop;
	snprintf(czBuf, 64, "%s", APP_PUBLISH_TIME);
    TextOut(hdc, nLeft2, nTop, czBuf);

    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//版本信息
PLUGIN_INFO g_layerVersionInfo =
{
    0x14110000/*id*/,
    VersionInfoInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    VersionInfoDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

void CoreInfoInit(DWORD dwPluginId)
{
}

void CoreInfoDraw(HDC hdc)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

    SelectFont(hdc,pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

	char czBuf[128]={0};

    //int nLeft = 5;
    int nLeft2 = 15;
    int nTop = 10;
    int tmpTop = 20;

    nTop += tmpTop;
    sprintf(czBuf, "核心板型号:%s", g_ctVer.ver[0].szType.value);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
	sprintf(czBuf, "核心板ID: %02x%02x%02x%02x%02x%02x", 
                                                        g_ctVer.ver[0].szId.value[0],  g_ctVer.ver[0].szId.value[1],
                                                        g_ctVer.ver[0].szId.value[2],  g_ctVer.ver[0].szId.value[3],
                                                        g_ctVer.ver[0].szId.value[4],  g_ctVer.ver[0].szId.value[5]);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop += tmpTop;
	sprintf(czBuf, "    %02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                                                        g_ctVer.ver[0].szId.value[6],  g_ctVer.ver[0].szId.value[7],
                                                        g_ctVer.ver[0].szId.value[8],  g_ctVer.ver[0].szId.value[9],
                                                        g_ctVer.ver[0].szId.value[10], g_ctVer.ver[0].szId.value[11],
                                                        g_ctVer.ver[0].szId.value[12], g_ctVer.ver[0].szId.value[13],
                                                        g_ctVer.ver[0].szId.value[14]);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop += tmpTop;
	sprintf(czBuf, "    %02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                                                        g_ctVer.ver[0].szId.value[15],
                                                        g_ctVer.ver[0].szId.value[16], g_ctVer.ver[0].szId.value[17],
                                                        g_ctVer.ver[0].szId.value[18], g_ctVer.ver[0].szId.value[19],
                                                        g_ctVer.ver[0].szId.value[20], g_ctVer.ver[0].szId.value[21],
                                                        g_ctVer.ver[0].szId.value[22], g_ctVer.ver[0].szId.value[23]);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "软件版本: %s", g_ctVer.ver[0].szSoftVer.value);
    TextOut(hdc, nLeft2, nTop, czBuf);


    nTop += tmpTop;
    sprintf(czBuf, "版本日期: %s %s", g_ctVer.ver[0].szSoftDate.value, g_ctVer.ver[0].szFactory.value);
    TextOut(hdc, nLeft2, nTop, czBuf);

    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//核心板信息
PLUGIN_INFO g_layerCoreInfo =
{
    0x13110000/*id*/,
    CoreInfoInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    CoreInfoDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

void CTAInfoInit(DWORD dwPluginId)
{
}

void CTAInfoDraw(HDC hdc)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

    SelectFont(hdc,pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

	char czBuf[128]={0};

    //int nLeft = 5;
    int nLeft2 = 15;
    int nTop = 10;
    int tmpTop = 20;

    nTop += tmpTop;
    sprintf(czBuf, "互感器型号:%s", g_ctVer.ver[1].szType.value);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
	sprintf(czBuf, "互感器ID: %02x%02x%02x%02x%02x%02x", 
                                                        g_ctVer.ver[1].szId.value[0],  g_ctVer.ver[1].szId.value[1],
                                                        g_ctVer.ver[1].szId.value[2],  g_ctVer.ver[1].szId.value[3],
                                                        g_ctVer.ver[1].szId.value[4],  g_ctVer.ver[1].szId.value[5]);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop += tmpTop;
	sprintf(czBuf, "    %02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                                                        g_ctVer.ver[1].szId.value[6],  g_ctVer.ver[1].szId.value[7],
                                                        g_ctVer.ver[1].szId.value[8],  g_ctVer.ver[1].szId.value[9],
                                                        g_ctVer.ver[1].szId.value[10], g_ctVer.ver[1].szId.value[11],
                                                        g_ctVer.ver[1].szId.value[12], g_ctVer.ver[1].szId.value[13],
                                                        g_ctVer.ver[1].szId.value[14]);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop += tmpTop;
	sprintf(czBuf, "    %02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                                                        g_ctVer.ver[1].szId.value[15],
                                                        g_ctVer.ver[1].szId.value[16], g_ctVer.ver[1].szId.value[17],
                                                        g_ctVer.ver[1].szId.value[18], g_ctVer.ver[1].szId.value[19],
                                                        g_ctVer.ver[1].szId.value[20], g_ctVer.ver[1].szId.value[21],
                                                        g_ctVer.ver[1].szId.value[22], g_ctVer.ver[1].szId.value[23]);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "软件版本: %s", g_ctVer.ver[1].szSoftVer.value);
    TextOut(hdc, nLeft2, nTop, czBuf);


    nTop += tmpTop;
    sprintf(czBuf, "版本日期: %s %s", g_ctVer.ver[1].szSoftDate.value, g_ctVer.ver[1].szFactory.value);
    TextOut(hdc, nLeft2, nTop, czBuf);

    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//A相互感器信息
PLUGIN_INFO g_layerCTAInfo =
{
    0x15110000/*id*/,
    CTAInfoInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    CTAInfoDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};


void CTBInfoInit(DWORD dwPluginId)
{
}

void CTBInfoDraw(HDC hdc)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

    SelectFont(hdc,pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

	char czBuf[128]={0};

    //int nLeft = 5;
    int nLeft2 = 15;
    int nTop = 10;
    int tmpTop = 20;

    nTop += tmpTop;
    sprintf(czBuf, "互感器型号:%s", g_ctVer.ver[2].szType.value);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
	sprintf(czBuf, "互感器ID: %02x%02x%02x%02x%02x%02x", 
                                                        g_ctVer.ver[2].szId.value[0],  g_ctVer.ver[2].szId.value[1],
                                                        g_ctVer.ver[2].szId.value[2],  g_ctVer.ver[2].szId.value[3],
                                                        g_ctVer.ver[2].szId.value[4],  g_ctVer.ver[2].szId.value[5]);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop += tmpTop;
	sprintf(czBuf, "    %02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                                                        g_ctVer.ver[2].szId.value[6],  g_ctVer.ver[2].szId.value[7],
                                                        g_ctVer.ver[2].szId.value[8],  g_ctVer.ver[2].szId.value[9],
                                                        g_ctVer.ver[2].szId.value[10], g_ctVer.ver[2].szId.value[11],
                                                        g_ctVer.ver[2].szId.value[12], g_ctVer.ver[2].szId.value[13],
                                                        g_ctVer.ver[2].szId.value[14]);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop += tmpTop;
	sprintf(czBuf, "    %02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                                                        g_ctVer.ver[2].szId.value[15],
                                                        g_ctVer.ver[2].szId.value[16], g_ctVer.ver[2].szId.value[17],
                                                        g_ctVer.ver[2].szId.value[18], g_ctVer.ver[2].szId.value[19],
                                                        g_ctVer.ver[2].szId.value[20], g_ctVer.ver[2].szId.value[21],
                                                        g_ctVer.ver[2].szId.value[22], g_ctVer.ver[2].szId.value[23]);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "软件版本: %s", g_ctVer.ver[2].szSoftVer.value);
    TextOut(hdc, nLeft2, nTop, czBuf);


    nTop += tmpTop;
    sprintf(czBuf, "版本日期: %s %s", g_ctVer.ver[2].szSoftDate.value, g_ctVer.ver[2].szFactory.value);
    TextOut(hdc, nLeft2, nTop, czBuf);

    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//B相互感器信息
PLUGIN_INFO g_layerCTBInfo =
{
    0x16110000/*id*/,
    CTBInfoInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    CTBInfoDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

void CTCInfoInit(DWORD dwPluginId)
{
}

void CTCInfoDraw(HDC hdc)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

    SelectFont(hdc,pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

	char czBuf[128]={0};

    //int nLeft = 5;
    int nLeft2 = 15;
    int nTop = 10;
    int tmpTop = 20;

    nTop += tmpTop;
    sprintf(czBuf, "互感器型号:%s", g_ctVer.ver[3].szType.value);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
	sprintf(czBuf, "互感器ID: %02x%02x%02x%02x%02x%02x", 
                                                        g_ctVer.ver[3].szId.value[0],  g_ctVer.ver[3].szId.value[1],
                                                        g_ctVer.ver[3].szId.value[2],  g_ctVer.ver[3].szId.value[3],
                                                        g_ctVer.ver[3].szId.value[4],  g_ctVer.ver[3].szId.value[5]);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop += tmpTop;
	sprintf(czBuf, "    %02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                                                        g_ctVer.ver[3].szId.value[6],  g_ctVer.ver[3].szId.value[7],
                                                        g_ctVer.ver[3].szId.value[8],  g_ctVer.ver[3].szId.value[9],
                                                        g_ctVer.ver[3].szId.value[10], g_ctVer.ver[3].szId.value[11],
                                                        g_ctVer.ver[3].szId.value[12], g_ctVer.ver[3].szId.value[13],
                                                        g_ctVer.ver[3].szId.value[14]);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop += tmpTop;
	sprintf(czBuf, "    %02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                                                        g_ctVer.ver[3].szId.value[15],
                                                        g_ctVer.ver[3].szId.value[16], g_ctVer.ver[3].szId.value[17],
                                                        g_ctVer.ver[3].szId.value[18], g_ctVer.ver[3].szId.value[19],
                                                        g_ctVer.ver[3].szId.value[20], g_ctVer.ver[3].szId.value[21],
                                                        g_ctVer.ver[3].szId.value[22], g_ctVer.ver[3].szId.value[23]);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "软件版本: %s", g_ctVer.ver[3].szSoftVer.value);
    TextOut(hdc, nLeft2, nTop, czBuf);


    nTop += tmpTop;
    sprintf(czBuf, "版本日期: %s %s", g_ctVer.ver[3].szSoftDate.value, g_ctVer.ver[3].szFactory.value);
    TextOut(hdc, nLeft2, nTop, czBuf);

    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//C相互感器信息
PLUGIN_INFO g_layerCTCInfo =
{
    0x17110000/*id*/,
    CTCInfoInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    CTCInfoDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
