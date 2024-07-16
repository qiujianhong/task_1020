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
//#include "deskgui_cfg.h"
#include "pageInterface.h"


//const char* g_pczDirectFlag[] = {"����", "����"};
//const char* g_pczOperationFlag[] = {"��", "��"};
//const char* g_pczPoints[] = {"1", "2", "3", "4", "5", "6", "7", "8"};
const char* g_pczRadioDelay[] = {"100", "200", "300", "400", "500", "800", "1000", "2000"};

//extern const char* g_pczMeasureBaud[];
//extern const char* g_pczMeasureData[];
//extern const char* g_pczMeasureStop[];
//extern const char* g_pczMeasureParity[];

//extern void CancelMenu();
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





#include "wireless.h"
extern WIRELESS_HANDLE_T *wireless_get_handle(uint8 id);
extern int wireless_set_stack(uint8 id, uint8 stack);
extern int wireless_set_apn(uint8 id, uint8 operate, char *apn, char *user, char *pwd);
extern int wireless_set_auth(uint8 id, uint8 authType);
extern int wireless_diag_again(uint8 id);
extern char* wireless_get_version(Date_Time_S *ptTime);
extern void wireless_app_stop();
extern void CancelMenu();
extern void wireless_get_appid_gui(uint8 *buf, uint8 len);

#ifdef __cplusplus
extern "C" {
#endif

extern WIRELESS_HANDLE_T *wireless_get_athandle(uint8 id);

#ifdef __cplusplus
}
#endif

//��������
const char* g_stack_type[] =
{
    "�Զ��л�Э��ջ",
    "����Э��ջ",
    "����Э��ջ",
};

//��Ӫ��
const char* g_operat[] =
{
    "�й��ƶ�",
    "�й���ͨ",
    "�й�����",
    "δ֪"
};

//��Ȩģʽ
const char* g_auth_type[] =
{
    "null",
    "chap",
    "pap",
    "pap&chap",
    "����Ӧ",
};

typedef struct _WIRELESSPARA_INFO
{
	uint8	operat;            // ��Ӫ��
    char    apn[64];           // apn
	char    user[64];          // user
    char    pwd[64];           // pwd
	uint8	authType;		   // ��Ȩģʽ
    int     nSel;              // ѡ����
}WIRELESSPARA_INFO;

typedef struct _WIRELESSPARA_SET
{
    uint8 index;
    WIRELESSPARA_INFO wireless_info1;
    WIRELESSPARA_INFO wireless_info2;
}WIRELESSPARA_SET;

WIRELESSPARA_SET g_wireless_set_info;


int g_DialingShowDrawPageIndex=0;//��ҳ����
void DialingStatusDataInit(DWORD dwPluginId)
{
    g_DialingShowDrawPageIndex=0;
}

void StatusDataDraw(HDC hdc, uint8 id)
{
    WIRELESS_HANDLE_T *ptemp = wireless_get_handle(id);
    
    if(ptemp == NULL)
            return;
    
	char czBuf[64]={0};

    int nLeft = 5;
    int nLeft2 = 65;
	int nTop = 10;
    int tmpTop = 20;

    switch(g_DialingShowDrawPageIndex)
	{
	case 0://��һҳ
    {   
    	// SIM��״̬
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "SIM��״̬");
        if(ptemp->bsimOK == 0)
        {
            TextOut(hdc, nLeft2, nTop, "����");
        }
        else
        {
    		TextOut(hdc, nLeft2, nTop, "û��");
        }

        // ����״̬
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "����״̬");
        if(ptemp->dialState == 0)
        {
            TextOut(hdc, nLeft2, nTop, "�ɹ�");
        }
        else
        {
    		TextOut(hdc, nLeft2, nTop, "ʧ��");
        }

        // ICCID
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "ICCID");
    	TextOut(hdc, nLeft2-30, nTop, (const char*)ptemp->ptNetPara->ICCID);

    	// IMSI
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "IMSI");
    	TextOut(hdc, nLeft2-30, nTop, (const char*)ptemp->ptNetPara->IMSI);

        // �ֻ���
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "�ֻ���");
    	TextOut(hdc, nLeft2, nTop, (const char*)ptemp->ptNetPara->SIMNum);
        
        break;
    }
	case 1://��2ҳ
    {
        // �ź�ǿ��
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "�ź�ǿ��");
        snprintf(czBuf, 64, "%d", ptemp->ptNetPara->csqvalue);
        TextOut(hdc, nLeft2, nTop, czBuf);
        
        // ��Ӫ��
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "��Ӫ��");
        if(ptemp->ptNetPara->operat == 0)
        {
            TextOut(hdc, nLeft2, nTop, "�й��ƶ�");
        }
        else if(ptemp->ptNetPara->operat == 1)
        {
    		TextOut(hdc, nLeft2, nTop, "�й���ͨ");
        }
        else if(ptemp->ptNetPara->operat == 2)
        {
    		TextOut(hdc, nLeft2, nTop, "�й�����");
        }
        else
        {
    		TextOut(hdc, nLeft2, nTop, "δ֪");
        }

        // ������ʽ
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "������ʽ");
        if(ptemp->ptNetPara->networkType == 0)
        {
            TextOut(hdc, nLeft2, nTop, "2G");
        }
        else if(ptemp->ptNetPara->networkType == 1)
        {
    		TextOut(hdc, nLeft2, nTop, "3G");
        }
        else if(ptemp->ptNetPara->networkType == 2)
        {
    		TextOut(hdc, nLeft2, nTop, "4G");
        }
        else if(ptemp->ptNetPara->networkType == 3)
        {
    		TextOut(hdc, nLeft2, nTop, "5G");
        }
        else
        {
    		TextOut(hdc, nLeft2, nTop, "δ֪");
        }

        // IP��ַ
    	nTop += tmpTop;
    	TextOut(hdc, nLeft, nTop, "IP��ַ");
    	commfun_IPToStr(htonl(ptemp->ptNetPara->ipAddr), czBuf);
    	TextOut(hdc, nLeft2, nTop, czBuf);

        
        // ��������
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "��������");
    	commfun_IPToStr(htonl(ptemp->ptNetPara->mask), czBuf);
    	TextOut(hdc, nLeft2, nTop, czBuf);

        break;
	}
    case 2://��3ҳ
    {
        // ����
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "����");
    	commfun_IPToStr(htonl(ptemp->ptNetPara->getway), czBuf);
    	TextOut(hdc, nLeft2, nTop, czBuf);

        // APN
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "APN");
        if(ptemp->apn.nNum <= 15)
        {	
            memcpy(czBuf, ptemp->apn.value, ptemp->apn.nNum);
            czBuf[ptemp->apn.nNum] = '\0';
            TextOut(hdc, nLeft2, nTop, czBuf);
		}
        else
        {
            memcpy(czBuf, ptemp->apn.value, 15);
            czBuf[15] = '\0';
            TextOut(hdc, nLeft2, nTop, czBuf);
            memcpy(czBuf, ptemp->apn.value + 15, ptemp->apn.nNum - 15);
            czBuf[ptemp->apn.nNum - 15] = '\0';
            nTop += 12;
            TextOut(hdc, 2, nTop, czBuf);
        }
        
        break;
	}
	}

    AppCancelKeyDraw(hdc, 3, 16, 12, 12);
}
void DialingStatusDataDraw1(HDC hdc)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;
	ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontSmall);

    StatusDataDraw(hdc, 1);

}

void DialingStatusDataDraw2(HDC hdc)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;
	ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontSmall);

    StatusDataDraw(hdc, 2);

}

BOOL DialingStatusKeyMsg(int nKey)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return FALSE;
	switch (nKey)
	{
	case KEY_DOWN:
		{
			g_DialingShowDrawPageIndex++;
			if (g_DialingShowDrawPageIndex > 2)
			{
				g_DialingShowDrawPageIndex=0;
			}
			UpdateWindow(pg_guicomm->hMainWnd, TRUE);
			return  TRUE;

		} 
	case KEY_UP:
		{
			g_DialingShowDrawPageIndex--;
			if (g_DialingShowDrawPageIndex < 0 )
			{
				g_DialingShowDrawPageIndex=2;
			}
			UpdateWindow(pg_guicomm->hMainWnd, TRUE);
			return TRUE;
		}
	default:
		return FALSE;
	}

}

//���߲���״̬
PLUGIN_INFO g_layerDialingStatusData1 = {
	0x11110000/*id*/,
	DialingStatusDataInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	DialingStatusDataDraw1/*Draw*/,
	NULL/*Timer*/,
	DialingStatusKeyMsg/*keymsg*/,
	NULL/*ipc*/
};

PLUGIN_INFO g_layerDialingStatusData2 = {
	0x11110001/*id*/,
	DialingStatusDataInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	DialingStatusDataDraw2/*Draw*/,
	NULL/*Timer*/,
	DialingStatusKeyMsg/*keymsg*/,
	NULL/*ipc*/
};

void DialingEventDataInit(DWORD dwPluginId)
{
}

void EventDataDraw(HDC hdc,uint8 id)
{
    WIRELESS_HANDLE_T *ptemp = wireless_get_handle(id);

    if(ptemp == NULL)
    {
        return;
    }
	char czBuf[40]={0};

    int nLeft = 10;
    int nLeft2 = 65;
	int nTop = 20;
    int tmpTop = 20;

    // ����״̬
	nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����״̬");
    if(ptemp->dialState == 0)
    {
        TextOut(hdc, nLeft2, nTop, "�ɹ�");
    }
    else
    {
		TextOut(hdc, nLeft2, nTop, "ʧ��");
    }

    // ����ʱ��
	nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "���һ�β��ųɹ�ʱ��");

    if(ptemp->Connecttime.year > 0)
    {
	    snprintf(czBuf, 40, "%04u-%02u-%02u %02u:%02u:%02u", ptemp->Connecttime.year+1900, ptemp->Connecttime.month+1, ptemp->Connecttime.day, 
                 ptemp->Connecttime.hour, ptemp->Connecttime.minute, ptemp->Connecttime.second);
    }
    else
    {
        snprintf(czBuf, 40, "                     --------");
    }
    nTop += tmpTop;
    TextOut(hdc, 32, nTop, czBuf);

    // �ɹ�����
	nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "���ղ��Ŵ���");
    
	snprintf(czBuf, 40, "%hu", ptemp->ConnectNum);
    TextOut(hdc, nLeft2 + 24, nTop, czBuf);

	// ʧ�ܴ���
	nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����ʧ�ܴ���");
	snprintf(czBuf, 40, "%hu", ptemp->FailedNum);
    TextOut(hdc, nLeft2 + 24, nTop, czBuf);

    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

void DialingEventDataDraw1(HDC hdc)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

	ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontSmall);

    EventDataDraw(hdc,1);

}

void DialingEventDataDraw2(HDC hdc)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

	ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontSmall);

    EventDataDraw(hdc,2);

}

//���߲����¼�
PLUGIN_INFO g_layerDialingEventData1 = {
	0x11210000/*id*/,
	DialingEventDataInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	DialingEventDataDraw1/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};
    
PLUGIN_INFO g_layerDialingEventData2 = {
	0x11210001/*id*/,
	DialingEventDataInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	DialingEventDataDraw2/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

uint8 g_GpsIndex = 0;
void DialingGPSDataInit(DWORD dwPluginId)
{
    g_GpsIndex = (dwPluginId & 0x01) + 1;
}

void DialingGPSDataDraw(HDC hdc)
{
    WIRELESS_HANDLE_T *ptemp = wireless_get_athandle(g_GpsIndex);

    if(ptemp == NULL)
        return;

    GPS_T *ptGps = ptemp->ptNetPara->pGps;

	char czBuf[80]={0};

    int nLeft = 5;
	int nTop = 30;
    int tmpTop = 14;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "���Ƕ�λ ģ��%d", g_GpsIndex);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);
    
    if(0 == ptemp->bExistGPS)
    {
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "û��GPS/����ģ��");
        return;
    }
    
    // λ����Ϣ
	nTop += tmpTop;
    if(0xffff == ptGps->utctime.year || 0xff == ptGps->utctime.month || 0xff == ptGps->utctime.mday)
    {
        sprintf(czBuf, "����: ��Ч");
    }
    else
    {
        sprintf(czBuf, "����: %04d-%02d-%02d", ptGps->utctime.year, ptGps->utctime.month, ptGps->utctime.mday);
    }
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    if(0xff == ptGps->utctime.hour || 0xff == ptGps->utctime.minute || 0xff == ptGps->utctime.second)
    {
        sprintf(czBuf, "ʱ��: ��Ч");
    }
    else
    {
        sprintf(czBuf, "ʱ��: %02d:%02d:%02d.%03d(UTC)", ptGps->utctime.hour, ptGps->utctime.minute, ptGps->utctime.second, 
                                                         ptGps->utctime.msec);
    }
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    char *statusname[] = {(char *)"δ��λ", (char *)"��Ч��λ", (char *)"��Ч��λ", (char *)"���ڹ���"};
    sprintf(czBuf, "״̬: %s", statusname[ptGps->ststus]);
    TextOut(hdc, nLeft, nTop, czBuf);

    char *modename[] = {(char *)"������λ", (char *)"���", (char *)"����"};
    nTop += tmpTop;
    sprintf(czBuf, "ģʽ: %s", ptGps->mode >= 3 ? "������Ч" : modename[ptGps->mode]);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "γ��: %s %d��%f��", 0 == ptGps->position.latitude.position ? "��" : "��",
                                        ptGps->position.latitude.degre, ptGps->position.latitude.branch);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "����: %s %d��%f��", 0 == ptGps->position.longitude.position ? "��" : "��",
                                        ptGps->position.longitude.degre, ptGps->position.longitude.branch);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "�߶�: %d cm", ptGps->position.height);
    TextOut(hdc, nLeft, nTop, czBuf);
}


//GPSλ����ʾ
PLUGIN_INFO g_layerGPSData1 = {
	0x11220000/*id*/,
	DialingGPSDataInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	DialingGPSDataDraw/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};
    
PLUGIN_INFO g_layerGPSData2 = {
	0x11220001/*id*/,
	DialingGPSDataInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	DialingGPSDataDraw/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

void *g_ATUart = NULL;
uint8 g_ATNum = 0;
void DialingATShowInit(DWORD dwPluginId)
{
    WIRELESS_HANDLE_T *ptemp = NULL;

    if(0x11220002 == dwPluginId)
    {
        ptemp = wireless_get_athandle(1);
        g_ATNum = 1;
    }
    else if(0x11220003 == dwPluginId)
    {
        ptemp = wireless_get_handle(1);
        g_ATNum = 2;
    }
    else if(0x11220004 == dwPluginId)
    {
        ptemp = wireless_get_athandle(2);
        g_ATNum = 3;
    }
    else if(0x11220005 == dwPluginId)
    {
        ptemp = wireless_get_handle(2);
        g_ATNum = 4;
    }
    g_ATUart = (NULL == ptemp) ? NULL : ptemp->pUart;
    wireless_atshow_set(g_ATUart);
}

void DialingATShowUninit()
{
    g_ATUart = NULL;
    g_ATNum = 0;
    wireless_atshow_set(g_ATUart);
}

void DialingATShowDraw(HDC hdc)
{
    char sendbuf[256] = {0};
    char recvbuf[256] = {0};
    TIME_STRUCT_T sendtime;
    TIME_STRUCT_T recvtime;
    char czBuf[32]={0};
    uint16 start = 0;
    uint16 end = 0;
    uint16 templen = 0;
    uint16 len = 0;
    int nLeft = 1;
    int nTop = 15;
    int tmpTop = 14;

    if(NULL == g_ATUart)
    {
        SelectFont(hdc, g_guiComm.fontBig);
        ITEM_NOT_SELECT;
        TextOut(hdc, 5, 30, "û�д򿪴����豸");
        return;
    }
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    wireless_atshow_get(sendbuf, 255, &sendtime, recvbuf, 255, &recvtime);

    
    sprintf(czBuf, "[Send %02d:%02d:%02d.%03d]", sendtime.hour, sendtime.minute, sendtime.second, sendtime.milliseconds);
    TextOut(hdc, nLeft, nTop, czBuf);
    nTop += tmpTop;

    /* �������� һ�����26��ĸ �����ʾ2�� */
    len = strlen(sendbuf);
    if(len > 2)
    {
        if(sendbuf[len - 2] == '\r' && sendbuf[len - 1] == '\n')
        {
            len -= 2;
        }
        /* ��ʾ��1�� */
        memset(czBuf, 0, 32);
        memcpy(czBuf, sendbuf, MIN(26, len));
        TextOut(hdc, nLeft, nTop, czBuf);
        nTop += tmpTop;
        if(len > 26)
        {
            /* ��ʾ��2�� */
            memset(czBuf, 0, 32);
            memcpy(czBuf, sendbuf + 26, MIN(27, len- 26));
            TextOut(hdc, nLeft, nTop, czBuf);
            nTop += tmpTop;
        }
    }
    else
    {
        nTop += tmpTop;
    }

    sprintf(czBuf, "[Recv %02d:%02d:%02d.%03d]", recvtime.hour, recvtime.minute, recvtime.second, recvtime.milliseconds);
    TextOut(hdc, nLeft, nTop, czBuf);
    nTop += tmpTop;

    /* �������� һ�����26��ĸ �Լ�������ʾ */
    len = strlen(recvbuf);
    /* ����ȥ����ǰ�� \r\n  */
    while(start < len)
    {
        if(recvbuf[start] == '\r' || recvbuf[start] == '\n')
        {
            start++;
            continue;
        }
        /* �ҵ�������ʾ������ */
        end = start;
        while(end < len)
        {
            if(recvbuf[end] == '\r' || recvbuf[end] == '\n')
            {
                break;
            }
            end++;
        }
        if(end > start)
        {
            templen = end - start;
            while(start < end)
            {
                memset(czBuf, 0, 32);
                templen = MIN(26, end - start);
                memcpy(czBuf, recvbuf + start, templen);
                TextOut(hdc, nLeft, nTop, czBuf);
                nTop += tmpTop;
                start += templen;
            }
        }
    }
}

void DialingATShowTimer(int nId)
{
    if(NULL == g_ATUart && nId == TIMER_MAIN)
    {
        WIRELESS_HANDLE_T *ptemp = NULL;
        /* 1sˢ��һ�� */
        switch(g_ATNum)
        {
            case 1: ptemp = wireless_get_athandle(1); break;
            case 2: ptemp = wireless_get_handle(1); break;
            case 3: ptemp = wireless_get_athandle(2); break;
            case 4: ptemp = wireless_get_handle(2); break;
            default:break;
        }
        g_ATUart = (NULL == ptemp) ? NULL : ptemp->pUart;
        if(NULL != g_ATUart)
        {
            wireless_atshow_set(g_ATUart);
        }
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
    }
	else if(NULL != g_ATUart && nId == TIMER_QUICK)
	{
	    /* 100msˢ��һ�� */
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
	}
}

//ATָ��鿴
PLUGIN_INFO g_layerATShowData1 = {
	0x11220002/*id*/,
	DialingATShowInit/*InitLayer*/,
	DialingATShowUninit/*UninitLayer*/,
	DialingATShowDraw/*Draw*/,
	DialingATShowTimer/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

PLUGIN_INFO g_layerATShowData2 = {
	0x11220003/*id*/,
	DialingATShowInit/*InitLayer*/,
	DialingATShowUninit/*UninitLayer*/,
	DialingATShowDraw/*Draw*/,
	DialingATShowTimer/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

PLUGIN_INFO g_layerATShowData3 = {
	0x11220004/*id*/,
	DialingATShowInit/*InitLayer*/,
	DialingATShowUninit/*UninitLayer*/,
	DialingATShowDraw/*Draw*/,
	DialingATShowTimer/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

PLUGIN_INFO g_layerATShowData4 = {
	0x11220005/*id*/,
	DialingATShowInit/*InitLayer*/,
	DialingATShowUninit/*UninitLayer*/,
	DialingATShowDraw/*Draw*/,
	DialingATShowTimer/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};


void DialingParamDataInit(DWORD dwPluginId)
{
}

void ParamDataDraw(HDC hdc,uint8 id)
{
    WIRELESS_HANDLE_T *ptemp = wireless_get_handle(id);

    if(ptemp == NULL)
        return;

    int nLeft = 10;
    int nLeft2 = 65;
	int nTop = 10;
    int tmpTop = 16;
    char czBuf[64] = {0};

    // ��Ӫ��
	nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "��Ӫ��");
    if(ptemp->ptNetPara->operat == 0)
    {
        TextOut(hdc, nLeft2, nTop, "�й��ƶ�");
    }
    else if(ptemp->ptNetPara->operat == 1)
    {
		TextOut(hdc, nLeft2, nTop, "�й���ͨ");
    }
    else if(ptemp->ptNetPara->operat == 2)
    {
		TextOut(hdc, nLeft2, nTop, "�й�����");
    }
    else
    {
		TextOut(hdc, nLeft2, nTop, "δ֪");
    }

    // APN
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "APN");
    if(ptemp->apn.nNum == 0)
    {
        TextOut(hdc, nLeft2, nTop, "----");
    }
    else
    {
		if(ptemp->apn.nNum <= 15)
        {	
            memcpy(czBuf, ptemp->apn.value, ptemp->apn.nNum);
            czBuf[ptemp->apn.nNum] = '\0';
            TextOut(hdc, nLeft2, nTop, czBuf);
		}
        else
        {
            memcpy(czBuf, ptemp->apn.value, 15);
            czBuf[15] = '\0';
            TextOut(hdc, nLeft2, nTop, czBuf);
            memcpy(czBuf, ptemp->apn.value + 15, ptemp->apn.nNum - 15);
            czBuf[ptemp->apn.nNum - 15] = '\0';
            nTop += 12;
            TextOut(hdc, 2, nTop, czBuf);
        }
    }

    // APN
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "�û���");
    if(ptemp->user.nNum == 0)
    {
        TextOut(hdc, nLeft2, nTop, "----");
    }
    else
    {
        if(ptemp->user.nNum <= 15)
        {	
            memcpy(czBuf, ptemp->user.value, ptemp->user.nNum);
            czBuf[ptemp->user.nNum] = '\0';
            TextOut(hdc, nLeft2, nTop, czBuf);
		}
        else
        {
            memcpy(czBuf, ptemp->user.value, 15);
            czBuf[15] = '\0';
            TextOut(hdc, nLeft2, nTop, czBuf);
            memcpy(czBuf, ptemp->user.value + 15, ptemp->user.nNum - 15);
            czBuf[ptemp->user.nNum - 15] = '\0';
            nTop += 12;
            TextOut(hdc, 2, nTop, czBuf);
        }
    }

    // APN
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����");
    if(ptemp->pwd.nNum == 0)
    {
        TextOut(hdc, nLeft2, nTop, "----");
    }
    else
    {
        memcpy(czBuf, ptemp->pwd.value, ptemp->pwd.nNum);
        czBuf[ptemp->pwd.nNum] = '\0';
        TextOut(hdc, nLeft2, nTop, czBuf);
        if(ptemp->pwd.nNum <= 15)
        {	
            memcpy(czBuf, ptemp->pwd.value, ptemp->pwd.nNum);
            czBuf[ptemp->pwd.nNum] = '\0';
            TextOut(hdc, nLeft2, nTop, czBuf);
		}
        else
        {
            memcpy(czBuf, ptemp->pwd.value, 15);
            czBuf[15] = '\0';
            TextOut(hdc, nLeft2, nTop, czBuf);
            memcpy(czBuf, ptemp->pwd.value + 15, ptemp->pwd.nNum - 15);
            czBuf[ptemp->pwd.nNum - 15] = '\0';
            nTop += 12;
            TextOut(hdc, 2, nTop, czBuf);
        }
    }

    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

void DialingParamDataDraw1(HDC hdc)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

	ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontSmall);

    ParamDataDraw(hdc,1);

}

void DialingParamDataDraw2(HDC hdc)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

	ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontSmall);

    ParamDataDraw(hdc,2);

}
//���Ų�����ʾ
PLUGIN_INFO g_layerDialingParamData1 = {
	0x11410000/*id*/,
	DialingParamDataInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	DialingParamDataDraw1/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

PLUGIN_INFO g_layerDialingParamData2 = {
	0x11410001/*id*/,
	DialingParamDataInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	DialingParamDataDraw2/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

void DialingParaGet(uint8 id, WIRELESSPARA_INFO *ptmp)
{
    WIRELESS_HANDLE_T *ptemp = wireless_get_handle(id);
    if(ptemp == NULL)
        return;
    
    ptmp->nSel = 0;
    ptmp->operat = ptemp->ptNetPara->operat;
    if(ptmp->operat > 2)
    {
        ptmp->operat = 3;
    }
    ptmp->authType = ptemp->ptNetPara->authType;
    if(ptmp->authType > 3)
    {
        ptmp->authType = 4;
    }
    if(ptemp->apn.nNum == 0 || ptemp->apn.nNum>64)
    {
        sprintf(ptmp->apn, "-----");
    }
    else
    {
        memcpy(ptmp->apn, ptemp->apn.value, ptemp->apn.nNum);
        ptmp->apn[ptemp->apn.nNum] = '\0';
    }

    if(ptemp->user.nNum == 0 || ptemp->user.nNum>64)
    {
        sprintf(ptmp->user, "-----");
    }
    else
    {
        memcpy(ptmp->user, ptemp->user.value, ptemp->user.nNum);
        ptmp->user[ptemp->user.nNum] = '\0';
    }

    if(ptemp->pwd.nNum == 0 || ptemp->pwd.nNum>64)
    {
        sprintf(ptmp->pwd, "-----");
    }
    else
    {
        memcpy(ptmp->pwd, ptemp->pwd.value, ptemp->pwd.nNum);
        ptmp->pwd[ptemp->pwd.nNum] = '\0';
    }

    return;
}

uint8 g_IfNo = 0;

void g_layerModuleData(DWORD dwPluginId)
{
    g_IfNo = 0x11240000 == dwPluginId ? 1 : 2;
}

void DialingModuleDraw(HDC hdc)
{
    WIRELESS_HANDLE_T *ptemp = wireless_get_handle(g_IfNo);
    uint8              bbInsert = TRUE;
    if(ptemp == NULL)
        return;

    NET_PARA_T *ptNet = ptemp->ptNetPara;

    char czBuf[80]={0};

    int nLeft = 5;
    int nTop = 16;
    int tmpTop = 16;
    ITEM_NOT_SELECT;

    SelectFont(hdc, g_guiComm.fontSmall);
    
    nTop = 20;
    bbInsert = 0 == ptemp->commport ? FALSE : TRUE;
    sprintf(czBuf, "ģ��״̬: %s", TRUE == bbInsert ? "����" : "δ����");
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "  IMEI  : %s", TRUE == bbInsert ? ptNet->IMEI : "--");
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "���̴���: %s", TRUE == bbInsert ? (char*)ptNet->VERINFO.manuNum : "--");
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "ģ���ͺ�: %s", TRUE == bbInsert ? (char*)ptNet->VERINFO.manuExten : "--");
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "ģ�����: %s %s", TRUE == bbInsert ? (char*)ptNet->VERINFO.softversion : "--",
                                      TRUE == bbInsert ? (char*)ptNet->VERINFO.softdata : "--");
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "ģ��Ӳ��: %s %s", TRUE == bbInsert ? (char*)ptNet->VERINFO.hwversion : "--",
                                      TRUE == bbInsert ? (char*)ptNet->VERINFO.hwdata : "--");
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "��������: %s", TRUE == bbInsert ? ptNet->nettype : "--");
    TextOut(hdc, nLeft, nTop, czBuf);
}

//ģ����Ϣ
PLUGIN_INFO g_layerModuleData1 = {
    0x11240000/*id*/,
    g_layerModuleData/*InitLayer*/,
    NULL/*UninitLayer*/,
    DialingModuleDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//ģ����Ϣ
PLUGIN_INFO g_layerModuleData2 = {
    0x11240001/*id*/,
    g_layerModuleData/*InitLayer*/,
    NULL/*UninitLayer*/,
    DialingModuleDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

void DialingTypSetInit(DWORD dwPluginId)
{
    g_wireless_set_info.index = 0;
    DialingParaGet(1, &g_wireless_set_info.wireless_info1);
    DialingParaGet(2, &g_wireless_set_info.wireless_info2);
    return;
}

void DialingPARASetDraw(HDC hdc, uint8 id)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

    ITEM_NOT_SELECT;
    SelectFont(hdc,pg_guicomm->fontSmall);

    WIRELESSPARA_INFO *ptmp;
    if(id == 1)
    {
        ptmp = &g_wireless_set_info.wireless_info1;
    }
    else
    {
        ptmp = &g_wireless_set_info.wireless_info2;
    }


    char czBuf[300];
    sprintf(czBuf, "���Ų���");
    
    int tmpTop = 16;
    
    // ��Ӫ��
    int nTop = 26;
    int nLeft1 = 10;
    int nLeft2 = 65;
    TextOut(hdc, nLeft1, nTop, "��Ӫ��");
    if(g_wireless_set_info.index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_operat[ptmp->operat]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_operat[ptmp->operat]);
    }
    
    //APN
    nTop += tmpTop;
    TextOut(hdc, nLeft1, nTop, "APN");
    if(g_wireless_set_info.index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop,  ptmp->apn);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, ptmp->apn);
    }
    nTop += tmpTop;
    TextOut(hdc, nLeft1, nTop, "�û���");
    if(g_wireless_set_info.index == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, ptmp->user);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, ptmp->user);
    }

    nTop += tmpTop;
    TextOut(hdc, nLeft1, nTop, "����");
    if(g_wireless_set_info.index == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, ptmp->pwd);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, ptmp->pwd);
    }

    nTop += tmpTop;
    TextOut(hdc, nLeft1, nTop, "��Ȩģʽ");
    if(g_wireless_set_info.index == 4)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_auth_type[ptmp->authType]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_auth_type[ptmp->authType]);
    }
    
   
    nTop += 20;
    nLeft1 = 1;
    nLeft2 = 82;

    if(g_wireless_set_info.index == 5)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "����");
    }

    if(g_wireless_set_info.index == 6)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "��������");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "��������");
    }

    PopListDraw(hdc);
}

void DialingPARASetDraw1(HDC hdc)
{
    DialingPARASetDraw(hdc, 1);
    return;
}

void DialingPARASetDraw2(HDC hdc)
{
    DialingPARASetDraw(hdc, 2);
    return;
}
void DialingParaReturn(char* pNewStr, void* pPara)
{

    WIRELESSPARA_INFO *ptmp;
    if(*(uint8*)pPara == 1 )
    {
        ptmp = &g_wireless_set_info.wireless_info1;
    }
    else
    {
        ptmp = &g_wireless_set_info.wireless_info2;
    }

    switch(g_wireless_set_info.index)
    {
    case 1:       
        {// ����apn
            if(pNewStr == NULL || strlen(pNewStr) == 0)
            {
                if(TRUE == MessageBox("APN�Ƿ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
            
                    InputBox(ptmp->apn, DialingParaReturn, pPara, 0x01|0x02|0x04|0x08|0x10, 0x04);
                }
            }
            else
            {
                strncpy(ptmp->apn, pNewStr, 64);
            }
        }
        break;

    case 2:
        {// �����û���
            if(pNewStr == NULL || strlen(pNewStr) == 0)
            {
                if(TRUE == MessageBox("�û����Ƿ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
            
                    InputBox(ptmp->apn, DialingParaReturn, pPara, 0x01|0x02|0x04|0x08|0x10, 0x04);
                }
            }
            else
            {
                strncpy(ptmp->user, pNewStr, 64);
            }
        }
        break;

    case 3:
        {// ��������
			if(pNewStr == NULL || strlen(pNewStr) == 0)
            {
                if(TRUE == MessageBox("����Ƿ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
            
                    InputBox(ptmp->apn, DialingParaReturn, pPara, 0x01|0x02|0x04|0x08|0x10, 0x04);
                }
            }
            else
            {
                strncpy(ptmp->pwd, pNewStr, 64);
            }
        }
        break;
	default:
		break;
    }
	return;
}

BOOL DialingParaSetKeyMsg(int nKey, uint8 id)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return FALSE;

    WIRELESSPARA_INFO *ptmp;
    if(id == 1 )
    {
        ptmp = &g_wireless_set_info.wireless_info1;
    }
    else
    {
        ptmp = &g_wireless_set_info.wireless_info2;
    }
    
    // ����Listѡ����
	if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (g_wireless_set_info.index)
            {
            case 0:// ��Ӫ��
                ptmp->operat = nSel;
                break;

            case 4:// ��Ȩģʽ
                ptmp->authType = nSel;
                break;

            default:
                ASSERT(FALSE);
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
	
	int nMax = 6;

	switch (nKey)
    {
    case KEY_UP:
        g_wireless_set_info.index --;

        if (g_wireless_set_info.index < 0)
        {
            g_wireless_set_info.index = nMax;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:
        g_wireless_set_info.index ++;
        if (g_wireless_set_info.index > nMax)
        {
            g_wireless_set_info.index = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        if(5 == g_wireless_set_info.index)
        {
            CancelMenu(); //����
        }
        if (ValidatePwd() == TRUE)
        {
            // ȷ�ϴ���
            switch(g_wireless_set_info.index)
            {
            case 0:// ��Ӫ��
                PopList(g_operat, 3, g_operat[ptmp->operat], 82, 42, FALSE);
                break;
            case 1:// apn
            {
                InputBox(ptmp->apn, DialingParaReturn, &id, 0x01|0x02|0x04|0x08|0x10, 0x04);
            }
            break;

            case 2:// �û����û���
                
                InputBox(ptmp->user, DialingParaReturn, &id, 0x01|0x02|0x04|0x08|0x10, 0x04);
                break;

            case 3:// ����
            {
                InputBox(ptmp->pwd, DialingParaReturn, &id, 0x01|0x02|0x04|0x08|0x10, 0x04);
            }
            break;

			case 4:	// ��Ȩģʽ
	        {
	            PopList(g_auth_type, 5, g_auth_type[ptmp->authType], 82, 42, FALSE);
                break;
	        }
	        break;
       
            case 6:	// ����
            {
                if (ValidatePwd() != TRUE)
    		    {
                    return FALSE;
                }
                if(0 == wireless_set_apn(id, ptmp->operat, ptmp->apn, ptmp->user, ptmp->pwd))
                {
                    if(ptmp->authType < 4)
                    {
    				    wireless_set_auth(id, ptmp->authType);
                    }
                    else
                    {
                        wireless_set_auth(id, 255);
                    }
                    MessageBox("���óɹ���");
                    CancelMenu();
                }
                else
                {
                    MessageBox("��ǰ���ú�ʵ�����Ͳ�ƥ��");
                }
            }
            break;

            default:

                break;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    return FALSE;
}

BOOL DialingParaSetKeyMsg1(int nKey)
{   
    return DialingParaSetKeyMsg(nKey, 1);
}

BOOL DialingParaSetKeyMsg2(int nKey)
{   
    return DialingParaSetKeyMsg(nKey, 2);
}


//���Ų�������
PLUGIN_INFO g_layerDialingPARASet1 =
{
    0x11610000/*id*/,
    DialingTypSetInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    DialingPARASetDraw1/*Draw*/,
    NULL/*Timer*/,
    DialingParaSetKeyMsg1/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layerDialingPARASet2 =
{
    0x11610001/*id*/,
    DialingTypSetInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    DialingPARASetDraw2/*Draw*/,
    NULL/*Timer*/,
    DialingParaSetKeyMsg2/*keymsg*/,
    NULL/*ipc*/
};

int g_nTDialingRestartAPPIndex = 0;
void DialingRestartAPPInit(DWORD dwPluginId)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

    g_nTDialingRestartAPPIndex = 0;
    pg_guicomm->bCanTrunDisp = FALSE;
}
void DialingRestartAPPDraw(HDC hdc)
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
    const char* pcz = "����Ӧ��";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if(g_nTDialingRestartAPPIndex == 0)
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
    if(g_nTDialingRestartAPPIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }

}

BOOL DialingRestartAPPKeyMsg(int nKey)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
        g_nTDialingRestartAPPIndex --;
        if(g_nTDialingRestartAPPIndex < 0)
        {
            g_nTDialingRestartAPPIndex = 1;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_nTDialingRestartAPPIndex ++;
        if(g_nTDialingRestartAPPIndex > 1)
        {
            g_nTDialingRestartAPPIndex = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch(g_nTDialingRestartAPPIndex)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
 //           if(ValidatePwd() == TRUE)
            {
                if(FALSE == MessageBox("��ʾ\n�Ƿ�����Ӧ��\n����ȷ��һ��", MB_OKCANCEL))
                {
                    return TRUE;
                }
                g_nTDialingRestartAPPIndex = 1;
                PopMsg("��������Ӧ��...");

                /* ���� */
                wireless_app_stop();
            }
            break;
        }
        return TRUE;
    }
    return FALSE;
}
//����Ӧ��
PLUGIN_INFO g_layerDialingRestartAPP =
{
    0x12110000/*id*/,
    DialingRestartAPPInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    DialingRestartAPPDraw/*Draw*/,
    NULL/*Timer*/,
    DialingRestartAPPKeyMsg/*keymsg*/,
    NULL/*ipc*/
};


int g_nReDialingIndex = 0;
void ReDialingInit(DWORD dwPluginId)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

    g_nReDialingIndex = 0;
    pg_guicomm->bCanTrunDisp = FALSE;
}
void ReDialingDraw(HDC hdc)
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
    const char* pcz = "���²���";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if(g_nReDialingIndex == 0)
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
    if(g_nReDialingIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }

    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL ReDialingKeyMsg(int nKey, uint8 id)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
        g_nReDialingIndex --;
        if(g_nReDialingIndex < 0)
        {
            g_nReDialingIndex = 1;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_nReDialingIndex ++;
        if(g_nReDialingIndex > 1)
        {
            g_nReDialingIndex = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch(g_nReDialingIndex)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
            if(wireless_diag_again(id)<0 )
            {
                AppMessageBox("���²���ʧ��!");
            }
            else
            {
                AppMessageBox("ͨ�����²��ųɹ�!");
                AppCancelMenu();
            }
            return TRUE;

        }
        return TRUE;
    }
    return FALSE;
}

BOOL ReDialingKeyMsg1(int nKey)
{

    return ReDialingKeyMsg(nKey, 1);
}

BOOL ReDialingKeyMsg2(int nKey)
{

    return ReDialingKeyMsg(nKey, 2);
}

//���²���
PLUGIN_INFO g_layerReDialing1 =
{
    0x13110000/*id*/,
    ReDialingInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    ReDialingDraw/*Draw*/,
    NULL/*Timer*/,
    ReDialingKeyMsg1/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layerReDialing2 =
{
    0x13110001/*id*/,
    ReDialingInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    ReDialingDraw/*Draw*/,
    NULL/*Timer*/,
    ReDialingKeyMsg2/*keymsg*/,
    NULL/*ipc*/
};


uint8 g_WireAppID[32];
void DialVersionInfoInit(DWORD dwPluginId)
{
    wireless_get_appid_gui(g_WireAppID, 32);
}

void DialVersionInfoDraw(HDC hdc)
{
	GUI_COMM* pg_guicomm = AppChkGuiComm();
	if(pg_guicomm == NULL)
		return;

    SelectFont(hdc,pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;
    
    Date_Time_S tmptime ={0};
    char *version =  wireless_get_version(&tmptime);

	char czBuf[64]={0};

    int nLeft = 5;
    int nLeft2 = 15;
	int nTop = 22;
    int tmpTop = 18;
    int tmpTop1 = 13;
    uint16 offset = 0;
    uint8 i;
    
	// ����汾��
    TextOut(hdc, nLeft, nTop, "����汾��");
    nTop += tmpTop1;
    sprintf(czBuf, "%s", version);
    TextOut(hdc, nLeft2, nTop, czBuf);

    
    // ����ʱ��
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����ʱ��");
    nTop += tmpTop1;
    uint32 year = ((0x000000ff&tmptime.year_h)<<8) + tmptime.year_l;
	sprintf(czBuf, "%04u-%02u-%02u %02u:%02u:%02u", year, tmptime.month, tmptime.day, tmptime.hour, tmptime.min, 
                                                    tmptime.sec);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "APPID");

    for(i = 0; i < 24; i ++)
    {
        offset += sprintf(czBuf + offset, "%02x ", g_WireAppID[i]);
        if(0 == (i + 1) % 8)
        {
            nTop += tmpTop1 - 1;
            TextOut(hdc, nLeft2, nTop, czBuf);
            offset = 0;
        }
    }
    
    CancelKeyDraw(hdc, 3, 16, 12, 12);

}
//�汾��Ϣ
PLUGIN_INFO g_layerDialVersionInfo =
{
    0x14110000/*id*/,
    DialVersionInfoInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    DialVersionInfoDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

