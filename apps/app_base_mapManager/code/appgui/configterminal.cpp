/*
***************************************************************************
  Copyright (c) 2006-2007,�Ͼ��������������������ι�˾
             All rights reserved.
  ��������: configcontrol.cpp
  ��    �ܣ�����ն���������ҳ���
  �� �� �ˣ�gaoyf
  ����ʱ�䣺2007-1

  �汾��ʷ:

    v1.0:
      2006-1-20, gaoyf : New file

  ����ҳ��⣺
    �ն˵�ַ�����������룩  0x00630000
    �ն˵�ǰ����ͨ����Ϣ    0x00660000


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
    
//�ӿ�����
const char* interfaceType[] = 
{
    "CDC-ACM",
    "CDC-ECM",
    "HID",
};
//��������
const char* pipeFunc[] = 
{
    "����",
    "Զ��",
    "PLC/RF",
    "RS-485",
    "ң��",
    "CAN",
    "M-Bus",
    "����",
    "ģ����",
    "RS232",
    "δ����",
    "��·",
    "����",
};

uint8	gCurMoudleIndex = 0;
int		gModulePageIndex = 0;
void InitmoduleInfo1(DWORD dwPluginId)
{
    gCurMoudleIndex = 0;
    gModulePageIndex = 0;
}

//ģ���豸�ͺš�ģ���豸ID������汾������������ڡ�Ӳ���汾��
//Ӳ���������ڡ����̱��롢�ӿ����͡���������
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
    sprintf(czBuf,"ģ��%d��Ϣ",gCurMoudleIndex+1);
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
        TextOut(hdc, nLeft1, nTop, "�豸�ͺ�:");
        nTop += 16;
        if(validFlag == 0x55)
            TextOut(hdc, nLeft2-30, nTop, pMoudluInfoAll[gCurMoudleIndex].moduleInfo.DevType);
        else
            TextOut(hdc, nLeft2-30, nTop, "--------");
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "�豸ID:");
        if(validFlag == 0x55)
            TextOut(hdc, nLeft2, nTop, pMoudluInfoAll[gCurMoudleIndex].moduleInfo.DevID);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
        
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "����汾:");
        //����Э�����
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
        TextOut(hdc, nLeft1, nTop, "��������:");
        //����Э�����
        if (pMoudluInfoAll[gCurMoudleIndex].moduleInfo.type == MDL_PRTL_VER1)
        {
            sprintf(czBuf,"%s", pMoudluInfoAll[gCurMoudleIndex].moduleInfo.softDate);
        }
        else
        {
            pDATE = &pMoudluInfoAll[gCurMoudleIndex].moduleInfo.date_soft;
            sprintf(czBuf,"%d��%d��%d��",pDATE->year,pDATE->month,pDATE->dayofmonth);
        }
        if(validFlag == 0x55)
            TextOut(hdc, nLeft2, nTop,czBuf);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
    break;

    case 1:
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "Ӳ���汾:");
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
        TextOut(hdc, nLeft1, nTop, "��������:");
        if (pMoudluInfoAll[gCurMoudleIndex].moduleInfo.type == MDL_PRTL_VER1)
        {
            sprintf(czBuf,"%s", pMoudluInfoAll[gCurMoudleIndex].moduleInfo.hardDate);
        }
        else
        {
            pDATE = &pMoudluInfoAll[gCurMoudleIndex].moduleInfo.date_hard;
            sprintf(czBuf,"%d��%d��%d��",pDATE->year,pDATE->month,pDATE->dayofmonth);
        }
        if(validFlag == 0x55)
            TextOut(hdc, nLeft2, nTop,czBuf);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
        
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "���̱���:");
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
                sprintf(czBuf,"ͨ��%d:",i);
                TextOut(hdc, nLeft1, nTop, czBuf);
                sprintf(czBuf,"%s %s",interfaceType[pVirturlPipe[i].interf-1],pipeFunc[pVirturlPipe[i].funcConfig]);
                TextOut(hdc, nLeft2, nTop,czBuf);
                nTop += 15;
            }
        }
        else
            TextOut(hdc, nLeft1, nTop, "ͨ��:");

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
    sprintf(czBuf,"ģ������Ϣ");
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
    sprintf(czBuf,"����Ӧ��");
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
        TextOut(hdc, nLeft, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȡ��");
    }

    nLeft = 110;
    if(gAppRestartSel == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
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
            if(gAppRestartSel == 0)	//ȡ��
            {
                AppCancelMenu();
            }
            else if(gAppRestartSel == 1)	//ȷ�ϴ���
            {
                if(ValidatePwd() == TRUE)
                {
                    if(FALSE == MessageBox("��ʾ\n�Ƿ�����ģ��\n����ȷ��һ��", MB_OKCANCEL))
                        return TRUE;
                    else
                    {
                        sprintf(czBuf,"sudo pkill mapManager");
                        system(czBuf);
                        //����ɱ������ϵͳ����������
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
    "��λ1",
    "��λ2",
    "��λ3",
    "��λ4",
    "��λ5",
};

int gModuleRestartSel = 0;
int gModuleRestartPageIndex = 0;
void InitmoduleRestart(DWORD dwPluginId)
{
    gModuleRestartSel = 0;				//����ģ��ѡ��
    gModuleRestartPageIndex = 0;		//��ǰҳ������
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
    sprintf(czBuf,"ģ������");
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
    //TextOut(hdc, nLeft1,nTop,"ģ��");

    if(gModuleRestartPageIndex == 0)
    {
        ITEM_SELECT;
        sprintf(czBuf,"��λ%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        sprintf(czBuf,"��λ%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
    }
    nLeft1 = 26;
    nTop += 40;
    if(gModuleRestartPageIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }

    nLeft1 = 110;
    if(gModuleRestartPageIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȷ��");
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
            else if(gModuleRestartPageIndex == 1)	//ȡ��
            {
                AppCancelMenu();
            }
            else if(gModuleRestartPageIndex == 2)	//ȷ�ϴ���
            {
                if(ValidatePwd() == TRUE)
                {
                    if(FALSE == MessageBox("��ʾ\n�Ƿ�����ģ��\n����ȷ��һ��", MB_OKCANCEL))
                        return TRUE;
                    else
                    {
                        if(ModulePowerOff(gModuleRestartSel+1,0) < 0 )
                        {
                            printf("ģ�� %d ����ʧ��! \n",gModuleRestartSel+1);
                            return FALSE;
                        }
                        modPowerCtl[gModuleRestartSel].offpowerflag = 1;
                        modPowerCtl[gModuleRestartSel].offpowertime = 10;
                        sprintf(czBuf,"ģ��%d�ϵ�,%d�������\n",gModuleRestartSel+1,(int)modPowerCtl[gModuleRestartSel].offpowertime);
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
    sprintf(czBuf,"ģ�鸴λ");
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
    //TextOut(hdc, nLeft1,nTop,"ģ��");

    if(gModuleRestartPageIndex == 0)
    {
        ITEM_SELECT;
        sprintf(czBuf,"��λ%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        sprintf(czBuf,"��λ%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
    }
    nLeft1 = 26;
    nTop += 40;
    if(gModuleRestartPageIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }

    nLeft1 = 110;
    if(gModuleRestartPageIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȷ��");
    }
    
    PopListDraw(hdc);
}
BOOL moduleResetKeyMsg(int nKey)
{
    return FALSE;
}

void InitdataInitilize(DWORD dwPluginId)
{
    gModuleRestartSel = 0;				//����ģ��ѡ��
    gModuleRestartPageIndex = 0;		//��ǰҳ������
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
    sprintf(czBuf,"���ݳ�ʼ��");
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
    //TextOut(hdc, nLeft1,nTop,"ģ��");

    if(gModuleRestartPageIndex == 0)
    {
        ITEM_SELECT;
        sprintf(czBuf,"��λ%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        sprintf(czBuf,"��λ%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
    }
    nLeft1 = 26;
    nTop += 40;
    if(gModuleRestartPageIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }

    nLeft1 = 110;
    if(gModuleRestartPageIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȷ��");
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
            else if(gModuleRestartPageIndex == 1)	//ȡ��
            {
                AppCancelMenu();
            }
            else if(gModuleRestartPageIndex == 2)	//ȷ�ϴ���
            {
                if(ValidatePwd() == TRUE)
                {
                    if(FALSE == MessageBox("��ʾ\n�Ƿ����ݳ�ʼ��ģ��\n����ȷ��һ��", MB_OKCANCEL))
                        return TRUE;
                    else
                    {
//						if(ModulePowerOff(gModuleRestartSel+1,0) < 0 )
                        {
//							printf("ģ�� %d ���ݳ�ʼ��ʧ��! \n",gModuleRestartSel+1);
//							return FALSE;
                        }
                        sprintf(czBuf,"ģ��%d ���ݳ�ʼ���ɹ�\n",gModuleRestartSel+1);
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
    gModuleRestartSel = 0;				//����ģ��ѡ��
    gModuleRestartPageIndex = 0;		//��ǰҳ������
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
    sprintf(czBuf,"������ʼ��");
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
    //TextOut(hdc, nLeft1,nTop,"ģ��");

    if(gModuleRestartPageIndex == 0)
    {
        ITEM_SELECT;
        sprintf(czBuf,"��λ%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        sprintf(czBuf,"��λ%d",gModuleRestartSel+1);
        TextOut(hdc, nLeft2,nTop,czBuf);
    }
    nLeft1 = 26;
    nTop += 40;
    if(gModuleRestartPageIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }

    nLeft1 = 110;
    if(gModuleRestartPageIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȷ��");
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
            else if(gModuleRestartPageIndex == 1)	//ȡ��
            {
                AppCancelMenu();
            }
            else if(gModuleRestartPageIndex == 2)	//ȷ�ϴ���
            {
                if(ValidatePwd() == TRUE)
                {
                    if(FALSE == MessageBox("��ʾ\n�Ƿ������ʼ��ģ��\n����ȷ��һ��", MB_OKCANCEL))
                        return TRUE;
                    else
                    {
//						if(ModulePowerOff(gModuleRestartSel+1,0) < 0 )
                        {
//							printf("ģ�� %d ������ʼ��ʧ��! \n",gModuleRestartSel+1);
//							return FALSE;
                        }
                        sprintf(czBuf,"ģ�� %d ������ʼ���ɹ�\n",gModuleRestartSel+1);
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
* @brief     ���ַ���ת����
* @param[in] ��str  �ַ���
* @param[out]��
* @return    ������
**********************************************************************/
int map_stoi(const char *str)
{
    assert(str != NULL);//�пն��ԣ��뺯��ʵ���޹�
    if(str==NULL)
        return 0;

    int tmp=0;
    while(*str !='\0')
    {
        if(*str>'0' && *str<='9')//if(isdigit(*str)) �Ƽ�ʹ�ú���isdigit,����ַ��Ƿ�Ϊ�ַ���0��~~��9��
        {
            tmp=tmp*10+(*str -'0');//������ֲ������ӡ�
        }
        str++;//ָ�����
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
    
    // ����汾��
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����汾��");

    nTop += tmpTop;
    sprintf(czBuf, "%u", map_stoi(APP_VERSION));
    TextOut(hdc, nLeft2, nTop, czBuf);

    
    // ����ʱ��
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����ʱ��");
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

