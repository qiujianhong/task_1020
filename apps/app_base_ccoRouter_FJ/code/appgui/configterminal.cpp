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

#include "CcoGui.h"

void HPLC_frequency_init(DWORD dwPluginId)
{
}

void HPLC_frequency_draw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    
    uint8 Fr = 0;
    
    char czBuf[64]={0};

    int nLeft = 10;
    int nLeft2 = 65;
    int nTop = 20;
    int tmpTop = 50;
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "��ǰHPLCƵ��");

    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);


    nTop = rc.top + tmpTop;
    TextOut(hdc, nLeft, nTop, "Ƶ�㣺");

    gui_hplc_fr_query(1, &Fr);
    sprintf(czBuf, "%d", Fr);
    TextOut(hdc, nLeft2, nTop, czBuf);

//    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//HPLCƵ��
PLUGIN_INFO g_layer_HPLC_frequency = {
	0x10010000/*id*/,
	HPLC_frequency_init/*InitLayer*/,
	NULL/*UninitLayer*/,
	HPLC_frequency_draw/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

void meter_reading_port_init(DWORD dwPluginId)
{
}

void meter_reading_port_draw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    
    uint32 Port = 0;
    
    char czBuf[64]={0};

    int nLeft = 10;
    int nLeft2 = 65;
    int nTop = 20;
    int tmpTop = 50;
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "��ǰ����˿�");

    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

    nTop = rc.top + tmpTop;
    TextOut(hdc, nLeft, nTop, "�˿ڣ�");
    
    gui_amr_port_query(1, &Port);
    sprintf(czBuf, "%08x", Port);
    TextOut(hdc, nLeft2, nTop, czBuf);

//    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//����˿�
PLUGIN_INFO g_layer_meter_reading_port = {
	0x10010001/*id*/,
	meter_reading_port_init/*InitLayer*/,
	NULL/*UninitLayer*/,
	meter_reading_port_draw/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

void module_address_init(DWORD dwPluginId)
{
}

void module_address_draw(HDC hdc)
{
	SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    
    char czBuf[64]={0};

    int nLeft = 5;
    int nLeft2 = 45;
    int nTop = 20;
    int tmpTop = 50;
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "��ǰģ���ַ");

    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);


    nTop = rc.top + tmpTop;
    TextOut(hdc, nLeft, nTop, "��ַ��");
    uint8 straddr[10]={0};
    
    gui_module_addr_query(1, 6, straddr);
    int i = 0;
    for(i=0; i<6; i++)
    {
        sprintf(czBuf+i*2, "%02X", straddr[i]);
    }
    czBuf[12] = '\0';
    TextOut(hdc, nLeft2, nTop, czBuf);

//    CancelKeyDraw(hdc, 3, 16, 12, 12);

}

//ģ���ַ
PLUGIN_INFO g_layer_module_address = {
	0x10010002/*id*/,
	module_address_init/*InitLayer*/,
	NULL/*UninitLayer*/,
	module_address_draw/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};
    
int g_module_info_drawpage_index;
void module_info_init(DWORD dwPluginId)
{
    g_module_info_drawpage_index = 0;
}

void module_info_draw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontSmall);
    ITEM_NOT_SELECT;
    
    AMR_QUERY_MODULE_INFO_T moduleinfo;
    
	char czBuf[64]={0};

    int nLeft = 5;
    int nLeft2 = 95;
	int nTop = 10;
    int tmpTop = 18;

    
    gui_module_info_query(1, &moduleinfo);

    switch(g_module_info_drawpage_index)
	{
	case 0://��һҳ
    {   
    	// ͨ��ģʽ��
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "ͨ��ģʽ��");        
        memcpy(czBuf, moduleinfo.aLocalCommTypeModel, 6);
        czBuf[6] = '\0';
        TextOut(hdc, nLeft2, nTop, czBuf);

        // �ӽڵ������ʱ
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "�ӽڵ㳬ʱʱ��");
        sprintf(czBuf, "%hu", moduleinfo.nSlaveMonitorMaxOvertime);
        TextOut(hdc, nLeft2, nTop, czBuf);

        // �㲥���ʱ
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "�㲥��ʱʱ��");
        sprintf(czBuf, "%hu", moduleinfo.nBroadcastMaxOvertime);
        TextOut(hdc, nLeft2, nTop, czBuf);

        // ����ĳ���
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "����ĳ���");
        sprintf(czBuf, "%hu", moduleinfo.nMaxAllowedFrameLen);
        TextOut(hdc, nLeft2, nTop, czBuf);

        // ������������
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "������������");
        sprintf(czBuf, "%hu", moduleinfo.nMaxUpgradeSingleFrameLen);
        TextOut(hdc, nLeft2, nTop, czBuf);

        // �����ȴ�ʱ��
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "�����ȴ�ʱ��");
        sprintf(czBuf, "%hu", moduleinfo.nUpgradeWaitingPeriod);
        TextOut(hdc, nLeft2, nTop, czBuf);
        break;
    }
	case 1://��2ҳ
    {
        // �㲥��ʱʱ��
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "�㲥��ʱʱ��");
        sprintf(czBuf, "%hu", moduleinfo.nBroadcastDelayPeriod);
        TextOut(hdc, nLeft2, nTop, czBuf);

        // ģ����
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "ģ����");
        memcpy(czBuf, moduleinfo.aModuleID, 6);
        czBuf[6] = '\0';
        TextOut(hdc, nLeft2, nTop, czBuf);
        
        // ���ӽڵ���
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "���ӽڵ���");
        sprintf(czBuf, "%hu", moduleinfo.nMaxSupportSlaveNum);
        TextOut(hdc, nLeft2, nTop, czBuf);
        
        // ��ǰ�ӽڵ���
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "��ǰ�ӽڵ���");
        sprintf(czBuf, "%hu", moduleinfo.nCurrentSlaveNum);
        TextOut(hdc, nLeft2, nTop, czBuf);

        // ģ��汾
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "ģ��汾");
        sprintf(czBuf, "%hu", moduleinfo.nModuleVer);
        TextOut(hdc, nLeft2, nTop, czBuf);
        
        break;
	}

	}

//    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL module_info_keymsg(int nKey)
{
	switch (nKey)
	{
	case KEY_DOWN:
		{
			g_module_info_drawpage_index++;
			if (g_module_info_drawpage_index > 1)
			{
				g_module_info_drawpage_index=0;
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return  TRUE;

		} 
	case KEY_UP:
		{
			g_module_info_drawpage_index--;
			if (g_module_info_drawpage_index < 0 )
			{
				g_module_info_drawpage_index=1;
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
	default:
		return FALSE;
	}

}

//ģ����Ϣ
PLUGIN_INFO g_layer_module_info = {
	0x10010003/*id*/,
	module_info_init/*InitLayer*/,
	NULL/*UninitLayer*/,
	module_info_draw/*Draw*/,
	NULL/*Timer*/,
	module_info_keymsg/*keymsg*/,
	NULL/*ipc*/
};

void meter_reading_status_init(DWORD dwPluginId)
{
}

void meter_reading_status_draw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    
    char czBuf[64]={0};
    AMR_QUERY_READING_STATE_T AmrState;

    int nLeft = 10;
    int nLeft2 = 65;
    int nTop = 20;
    int tmpTop = 50;
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "��ǰ����״̬");

    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);


    nTop = rc.top + tmpTop;
    TextOut(hdc, nLeft, nTop, "״̬��");

    gui_amr_state_query(1, &AmrState);
    if(AmrState.nModuleAmrState == 0)
    {
        TextOut(hdc, nLeft2, nTop, "δ����");
    }
    else
    {
		TextOut(hdc, nLeft2, nTop, "������");
    }


//    CancelKeyDraw(hdc, 3, 16, 12, 12);

    
}

//����״̬
PLUGIN_INFO g_layer_meter_reading_status = {
	0x10010004/*id*/,
	meter_reading_status_init/*InitLayer*/,
	NULL/*UninitLayer*/,
	meter_reading_status_draw/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

void station_identification_init(DWORD dwPluginId)
{
}

void station_identification_draw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    
    char czBuf[64]={0};

    int nLeft = 5;
    int nLeft2 = 85;
    int nTop = 20;
    int tmpTop = 50;
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "̨��ʶ����״̬");

    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);


    nTop = rc.top + tmpTop;
    TextOut(hdc, nLeft, nTop, "����״̬��");

    int ret = gui_area_iden_query(1);
    if(ret == 0)
    {
        TextOut(hdc, nLeft2, nTop, "δ����");
    }
    else
    {
		TextOut(hdc, nLeft2, nTop, "����");
    }


//    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//̨��ʶ��
PLUGIN_INFO g_layer_station_identification = {
	0x10010005/*id*/,
	station_identification_init/*InitLayer*/,
	NULL/*UninitLayer*/,
	station_identification_draw/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};
    
void off_grid_awareness_init(DWORD dwPluginId)
{
}

void off_grid_awareness_draw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    
    char czBuf[64]={0};

    int nLeft = 5;
    int nLeft2 = 85;
    int nTop = 20;
    int tmpTop = 50;
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "������֪����״̬");

    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);


    nTop = rc.top + tmpTop;
    TextOut(hdc, nLeft, nTop, "����״̬��");

    int ret = gui_offline_feel_query(1);
    if(ret == 0)
    {
        TextOut(hdc, nLeft2, nTop, "δ����");
    }
    else
    {
		TextOut(hdc, nLeft2, nTop, "����");
    }

//    CancelKeyDraw(hdc, 3, 16, 12, 12);

}

//������֪
PLUGIN_INFO g_layer_off_grid_awareness = {
	0x10010006/*id*/,
	off_grid_awareness_init/*InitLayer*/,
	NULL/*UninitLayer*/,
	off_grid_awareness_draw/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

void reject_node_report_init(DWORD dwPluginId)
{
}

void reject_node_report_draw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    
    char czBuf[64]={0};

    int nLeft = 5;
    int nLeft2 = 85;
    int nTop = 20;
    int tmpTop = 50;
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "�ܾ��ڵ��ϱ�����״̬");

    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);


    nTop = rc.top + tmpTop;
    TextOut(hdc, nLeft, nTop, "����״̬��");

    int ret = gui_refuse_report_query(1);
    if(ret == 0)
    {
        TextOut(hdc, nLeft2, nTop, "δ����");
    }
    else
    {
		TextOut(hdc, nLeft2, nTop, "����");
    }

//    CancelKeyDraw(hdc, 3, 16, 12, 12);
    
}

//�ܾ��ڵ��ϱ�
PLUGIN_INFO g_layer_reject_node_report = {
	0x10010007/*id*/,
	reject_node_report_init/*InitLayer*/,
	NULL/*UninitLayer*/,
	reject_node_report_draw/*Draw*/,
	NULL/*Timer*/,
	NULL/*keymsg*/,
	NULL/*ipc*/
};

//Ӧ��ͳ����Ϣ
int g_file_info_num;
int g_file_info_point;
int g_file_info_index;
AMR_METER_FILES_INFO_T g_filesinfo;

void file_info_init(DWORD dwPluginId)
{
    g_file_info_point = 1;
    g_file_info_num = 0;
    g_file_info_index = 0;
    gui_single_files_query(1, g_file_info_point-1, &g_filesinfo);
}

void file_info_draw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontSmall);
    ITEM_NOT_SELECT;
    
    AMR_QUERY_MODULE_INFO_T moduleinfo;
    
	char czBuf[64]={0};

    int nLeft = 5;
    int nLeft2 = 85;
	int nTop = 10;
    int tmpTop = 16;
    
    gui_module_info_query(1, &moduleinfo);
    g_file_info_num = gui_files_count_query(1);

    if(g_file_info_num < 0)
        g_file_info_num = 0;
/*
    switch(g_module_info_drawpage_index)
	{
	case 0://��һҳ
    {   
*/    	// ��������
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "��������");        
        sprintf(czBuf, "%d", g_file_info_num);
        TextOut(hdc, nLeft2, nTop, czBuf);
     if(g_file_info_num > 0)
    {
        // �������
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "�������");
        sprintf(czBuf, "%hu", g_file_info_point);
        if(g_file_info_index == 1)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        // �˿ں�
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "�˿ں�");
        sprintf(czBuf, "%08x", g_filesinfo.nPortID);
        TextOut(hdc, nLeft2, nTop, czBuf);
/*
        // �������
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "�������");
        sprintf(czBuf, "%hu", g_filesinfo.stAmrMeterFiles.nMeterFilesID);
        TextOut(hdc, nLeft2, nTop, czBuf);
*/
        // ������ַ
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "������ַ");
        int i = 0;
        for(i=0; i<6; i++)
        {
            sprintf(czBuf+i*2, "%02X", g_filesinfo.stAmrMeterFiles.aMeterFilesAddr[5-i]);
        }
        czBuf[12] = '\0';
        TextOut(hdc, nLeft2, nTop, czBuf);
	
        // ����MAC
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "����MAC");
        for(i=0; i<6; i++)
        {
            sprintf(czBuf+i*2, "%02X", g_filesinfo.stAmrMeterFiles.aMeterFilesMac[i]);
        }
        czBuf[12] = '\0';
        TextOut(hdc, nLeft2, nTop, czBuf);

        // Э������
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "Э������");
        sprintf(czBuf, "%hu", g_filesinfo.stAmrMeterFiles.nProtocolType);
        TextOut(hdc, nLeft2, nTop, czBuf);
#if 0        
        // ͨ�Ų���
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "ͨ�Ų���");
        sprintf(czBuf, "%hu", g_filesinfo.stAmrMeterFiles.nCommPara);
        TextOut(hdc, nLeft2, nTop, czBuf);
#endif
    }
 /*       
        break;
    }
	case 1://��2ҳ
    {
        // Э������
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "Э������");
        sprintf(czBuf, "%hu", g_filesinfo.stAmrMeterFiles.nProtocolType);
        TextOut(hdc, nLeft2, nTop, czBuf);
        
        // ͨ�Ų���
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "ͨ�Ų���");
        sprintf(czBuf, "%hu", g_filesinfo.stAmrMeterFiles.nCommPara);
        TextOut(hdc, nLeft2, nTop, czBuf);
        
        // ��ǰ�ӽڵ���
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "��ǰ�ӽڵ���");
        sprintf(czBuf, "%hu", moduleinfo.nCurrentSlaveNum);
        TextOut(hdc, nLeft2, nTop, czBuf);

        // ģ��汾
    	nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "ģ��汾");
        sprintf(czBuf, "%hu", moduleinfo.nModuleVer);
        TextOut(hdc, nLeft2, nTop, czBuf);
        
        break;
	}

	}
*/
//    CancelKeyDraw(hdc, 3, 16, 12, 12);
    
}

void CurPointReturn(char* pNewStr, void* pPara)
{	
    int nPoint = atoi(pNewStr);
    if(nPoint >= MAX_FILES_SUMMARY_NUM)
    {
        char bzbuf[100];
        sprintf(bzbuf, "�����������Ϊ%d\nȷ���Ƿ���������?", (MAX_FILES_SUMMARY_NUM));
        if(TRUE == MessageBox(bzbuf, MB_OKCANCEL))
        {
            sprintf(bzbuf, "%d", g_file_info_point);
            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
        }
    }
    else
    {
        g_file_info_point = nPoint;
    }
}

BOOL file_info_keymsg(int nKey)
{
	switch (nKey)
	{
	case KEY_DOWN:
		{
			g_file_info_index ++;
			if (g_file_info_index > 6)
			{
				g_file_info_index=0;
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return  TRUE;

		} 
	case KEY_UP:
		{
			g_file_info_index--;
			if (g_file_info_index < 0 )
			{
				g_file_info_index=6;
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
	case KEY_RIGHT:
		{
			g_file_info_point++;
			if (g_file_info_point > g_file_info_num)
			{
				g_file_info_point=1;
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
            gui_single_files_query(1, g_file_info_point-1, &g_filesinfo);
			return  TRUE;

		} 
	case KEY_LEFT:
		{
			g_file_info_point--;
			if (g_file_info_point < 1 )
			{
				g_file_info_point=g_file_info_num;
			}
            gui_single_files_query(1, g_file_info_point-1, &g_filesinfo);
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
    case KEY_OK:
        {
			switch(g_file_info_index)
			{
			case 1:// ����������ѡ��List
//				PopMeasure(g_guiComm.nCurPoint-1);
				char bzbuf[50];
				sprintf(bzbuf, "%d", g_file_info_point);
				InputBox(bzbuf, CurPointReturn, NULL, 0x01);
                gui_single_files_query(1, g_file_info_point-1, &g_filesinfo);
				break;
             }

            return TRUE;
        }
	default:
		return FALSE;
	}

}


//������Ϣ
PLUGIN_INFO g_layer_file_info = {
	0x10020000/*id*/,
	file_info_init/*InitLayer*/,
	NULL/*UninitLayer*/,
	file_info_draw/*Draw*/,
	NULL/*Timer*/,
	file_info_keymsg/*keymsg*/,
	NULL/*ipc*/
};


int g_restart_app_index = 0;
void restart_app_init(DWORD dwPluginId)
{
    g_restart_app_index = 0;
    g_guiComm.bCanTrunDisp = FALSE;
}
void restart_app_draw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top+16;
    const char* pcz = "����Ӧ��";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if(g_restart_app_index == 0)
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
    if(g_restart_app_index == 1)
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

BOOL restart_app_keymsg(int nKey)
{
    switch(nKey)
    {
    case KEY_LEFT:
        g_restart_app_index --;
        if(g_restart_app_index < 0)
        {
            g_restart_app_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_restart_app_index ++;
        if(g_restart_app_index > 1)
        {
            g_restart_app_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch(g_restart_app_index)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
//            if(ValidatePwd() == TRUE)
            {
                if(FALSE == MessageBox("��ʾ\n�Ƿ�����Ӧ��\n����ȷ��һ��", MB_OKCANCEL))
                {
                    return TRUE;
                }
                g_restart_app_index = 0;
                PopMsg("����Ӧ����δʵ��");
                exit(0);
            }
            break;
        }
        return TRUE;
    }
    return FALSE;
}
//����Ӧ��
PLUGIN_INFO g_layer_restart_app =
{
    0x10030000/*id*/,
    restart_app_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    restart_app_draw/*Draw*/,
    NULL/*Timer*/,
    restart_app_keymsg/*keymsg*/,
    NULL/*ipc*/
};


uint8 g_hardware_initialization_index;
void hardware_initialization_init(DWORD dwPluginId)
{
    g_hardware_initialization_index = 0;
    g_guiComm.bCanTrunDisp = FALSE;
}
void hardware_initialization_draw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top+16;
    const char* pcz = "Ӳ����ʼ��";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if(g_hardware_initialization_index == 0)
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
    if(g_hardware_initialization_index == 1)
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

BOOL hardware_initialization_keymsg(int nKey)
{
    switch(nKey)
    {
    case KEY_LEFT:
        g_hardware_initialization_index--;
        if(g_hardware_initialization_index < 0)
        {
            g_hardware_initialization_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_hardware_initialization_index++;
        if(g_hardware_initialization_index > 1)
        {
            g_hardware_initialization_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch(g_hardware_initialization_index)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
            if(gui_hardware_init(1)<0 )
            {
                MessageBox("Ӳ����ʼ��ʧ��!");
            }
            else
            {
                MessageBox("Ӳ����ʼ���ɹ�!");
            }
            return TRUE;

        }
        return TRUE;
    }
    return FALSE;
}

//Ӳ����ʼ��
PLUGIN_INFO g_layer_hardware_initialization =
{
    0x10030001/*id*/,
    hardware_initialization_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    hardware_initialization_draw/*Draw*/,
    NULL/*Timer*/,
    hardware_initialization_keymsg/*keymsg*/,
    NULL/*ipc*/
};

uint8 g_param_initialization_index;
void param_initialization_init(DWORD dwPluginId)
{
    g_param_initialization_index = 0;
    g_guiComm.bCanTrunDisp = FALSE;
}
void param_initialization_draw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top+16;
    const char* pcz = "������ʼ��";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if(g_param_initialization_index == 0)
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
    if(g_param_initialization_index == 1)
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

BOOL param_initialization_keymsg(int nKey)
{
    switch(nKey)
    {
    case KEY_LEFT:
        g_param_initialization_index--;
        if(g_param_initialization_index < 0)
        {
            g_param_initialization_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_param_initialization_index++;
        if(g_param_initialization_index > 1)
        {
            g_param_initialization_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch(g_param_initialization_index)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
            if(gui_param_init(1)<0 )
            {
                MessageBox("������ʼ��ʧ��!");
            }
            else
            {
                MessageBox("������ʼ���ɹ�!");
            }
            return TRUE;

        }
        return TRUE;
    }
    return FALSE;
}

//������ʼ��
PLUGIN_INFO g_layer_param_initialization =
{
    0x10030002/*id*/,
    param_initialization_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    param_initialization_draw/*Draw*/,
    NULL/*Timer*/,
    param_initialization_keymsg/*keymsg*/,
    NULL/*ipc*/
};

void app_version_info_init(DWORD dwPluginId)
{
}

void app_version_info_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontBig);
    ITEM_NOT_SELECT;


    char czBuf[64] = { 0 };

    int nLeft = 5;
    int nLeft2 = 15;
    int nTop = 20;
    int tmpTop = 20;

    // ����汾��
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����汾��");

    nTop += tmpTop;
    sprintf(czBuf, "%s", APP_VERSION);
    TextOut(hdc, nLeft2, nTop, czBuf);


    // ����ʱ��
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����ʱ��");
    nTop += tmpTop;
    snprintf(czBuf, 64, "%s", APP_PUBLISH_TIME);
    TextOut(hdc, nLeft2, nTop, czBuf);

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);

}
//Ӧ�ð汾��Ϣ
PLUGIN_INFO g_layer_app_version_info =
{
    0x10030003/*id*/,
    app_version_info_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    app_version_info_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};


