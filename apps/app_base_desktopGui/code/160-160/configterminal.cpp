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
#include <sys/statfs.h>
#include "miniguidef.h"
#include "commdef.h"
#include "main.h"
#include "commfun.h"
#include "iniparser.h"
#include "appmsg_split.h"
#include "out_msg.h"
#include "fileOp.h"
#include "hal.h"
#include "power.h"
#include "devinfo.h"


extern void CancelMenu();

uint8 safe_status = 0;		//��ȫģʽ�� 0�����رգ�1����������Ĭ�Ϲر�
uint8 mac_status = 1;		//MACУ�飺 0�����رգ�1����������Ĭ�Ͽ���
uint8 hplc_mac_status = 0;      //�ز�MACУ�鿪�� Ĭ�Ϲر�
uint8 g_Relay485Flag = 1;   //�Ƿ�֧��ת��485���ڱ���: 1-enable 0-disable
int safe_index = 0;
uint8 router_state =1; //·��ѧϰ״̬�� 0�����رգ�1����������Ĭ�Ͽ���
uint8 g_minutefunclag = 0;   //�Ƿ�֧��ת��485���ڱ���: 1-enable 0-disable

const char *g_safeModel_list[] = {
	"�ر�",
	"����",
};
const char *g_macModel_list[] = {
	"�ر�",
	"����",
};

const char *g_ac_485_list[] = {
	"�ر�",
	"����",
};
const char *g_routerstudy_list[] = {
	"�ر�",
	"����",
};
const char *g_minutefunc_list[] = {
	"�ر�",
	"����",
};

int set_ini_key_value(const char * ininame, const char * key, uint8 notfound)
{
    char str_value[8] = {0};
    dictionary *ini = NULL;
    ini = iniparser_load(ininame);
    printf("�������ļ�%s \n", ininame);
    if(ini!=NULL)
    {
        sprintf(str_value, "%hu", notfound);
        dictionary_set(ini, key, str_value);
        iniparser_freedict(ini);
    }

    return 0;
}

/**********************************************************************
* @name      : get_line_by_key
* @brief     ���ҵ��ؼ��������е�λ�úͳ���
* @param[in] ��FILE *pf            �ļ����
               const char *key    �ؼ���

* @param[out]��long *poffset �ؼ�����������ʼλ��
* @return    ��>0����, <0����
* @Create    :
* @Date      ��2020-4-21
* @Update    :
**********************************************************************/
int get_line_by_key(FILE *pf/*in*/, const char *key/*in*/, long *poffset/*out*/)
{
    char line[1024] = {0};
    long offset = 0;
    int line_len = 0;

    if(pf == NULL){
        return -1;
    }

    while(fgets(line,1024,pf) != NULL)
    {
        if(strstr(line, key) != NULL)
        {
            offset = ftell(pf); // ��¼�ļ�ָ��λ��, ��Ϊ���滹Ҫ��, �ļ�ָ�������
            line_len = strlen(line);
            *poffset = offset;
            printf("�����ļ����ó���%d %s  \n", line_len, line);
            return line_len;
        }
        
    }
    return -1;
}

int write_cfg_by_key(const char *filename/*in*/, const char *key/*in*/, uint8 value/*in*/)
{
    
    FILE *fp;
    char buf[128]; // ����ÿ�в�����1024�ֽ�, ����������ڴ�С
    long offset;
    
    if (!(fp = fopen(filename, "r+"))) 
    { // �����Զ�д��ʽ���ļ�.
        printf("���ļ������ļ�%s ʧ�� \n", filename );
        return -1;
    }

    fseek(fp,0,SEEK_END);
    long lSize = ftell(fp);
    // �������Ҫ���ڴ�free��
    char *str =(char*)malloc(lSize+1);
    rewind(fp); 
    fread(str,sizeof(char),lSize,fp);
    str[lSize] = '\0';

    fseek(fp,0,SEEK_SET);
    int len = get_line_by_key(fp , key, &offset);
    if (len < 0) 
    {
        free(str);
        fclose(fp);
        printf("�����ļ������ļ�%s ʧ�� \n", key );
        return -1;
    }

    offset -= len;
    fseek(fp, offset, SEEK_SET);
    sprintf(buf, "%s=%hu", key, value);
    fwrite(buf, strlen(buf), 1, fp);
    fwrite(str+offset+len, strlen(str+offset+len), 1, fp);

    free(str);
    fclose(fp);
    return 0;
}

int write_safe_cfg(const char *filename/*in*/)
{
    
    FILE *fp;
    char buf[128]; // ����ÿ�в�����1024�ֽ�, ����������ڴ�С
 
    if (!(fp = fopen(filename, "w"))) 
    { // ������д��ʽ���ļ�.
        printf("���ļ������ļ�%s ʧ�� \n", filename );
        return -1;
    }

    sprintf(buf, "[SAFE_MODE]\n");
    fwrite(buf, strlen(buf), 1, fp);

    
//    sprintf(buf, "SAFE_BUTTON=%hu\n", safe_status);
//    fwrite(buf, strlen(buf), 1, fp);

    sprintf(buf, "MAC_MODE=%hu\n", mac_status);
    fwrite(buf, strlen(buf), 1, fp);
    sprintf(buf, "HPLC_MAC_MODE=%hu\n", hplc_mac_status);
    fwrite(buf, strlen(buf), 1, fp);

    sprintf(buf, "AC485FLAG=%hu\n", g_Relay485Flag);
    fwrite(buf, strlen(buf), 1, fp);
    sprintf(buf, "ROUTER_STATE_STUDY=%hu\n", router_state);
    fwrite(buf, strlen(buf), 1, fp);

    sprintf(buf, "MINUTE_COLLECT_STATE=%hu\n", g_minutefunclag);
    fwrite(buf, strlen(buf), 1, fp);

    fclose(fp);
    return 0;
}

OOP_SECURITYFLAG_E g_safe_mode;

int write_safemode()
{
	int ret;
	NOMAL_OAD_T NomalOad = { 0 };
	NomalOad.logicId = 0;
	NomalOad.oad.value = 0xF1010200;
	NomalOad.classtag = 2;
	NomalOad.infoNum = 0;

    if (safe_status == 0) 
    {
    	g_safe_mode = SECURITY_OFF;
    }
    else 
    {
    	g_safe_mode = SECURITY_ON;
    }
    
	ret = db_write_nomal(g_hUdbCenter, &NomalOad, (uint8*)&g_safe_mode, sizeof(OOP_SECURITYFLAG_E));
	if (ret != 0)
	{
		GUI_FMT_DEBUG("���ð�ȫģʽ�豸 0xF1010200 ret=%d \n", ret);
	}
	return ret;
}


void setSafeONOFFInit(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;

    dictionary *ini = NULL;
    ini = iniparser_load("/data/app/desktopGui/safemode.ini");
    printf("��ȡ�����ļ�/data/app/desktopGui/safemode.ini \n");
    if(ini!=NULL)
    {
//        safe_status= iniparser_getint(ini,"SAFE_MODE:SAFE_BUTTON",0);
        mac_status= iniparser_getint(ini,"SAFE_MODE:MAC_MODE",1);
        hplc_mac_status= iniparser_getint(ini,"SAFE_MODE:HPLC_MAC_MODE",0);
        g_Relay485Flag= iniparser_getint(ini,"SAFE_MODE:AC485FLAG",1);
        router_state= iniparser_getint(ini,"SAFE_MODE:ROUTER_STATE_STUDY",1);
        g_minutefunclag= iniparser_getint(ini,"SAFE_MODE:MINUTE_COLLECT_STATE",0);
        iniparser_freedict(ini);
    }
/*    
    ini = iniparser_load("/data/app/taskManager/taskManager.ini");
    printf("��ȡ�����ļ�/data/app/taskManager/taskManager.ini \n");
    if(ini!=NULL)
    {
        mac_status= iniparser_getint(ini,"SAFE_MODE:SAFE_BUTTON",1);
        iniparser_freedict(ini);
    }

    ini = iniparser_load("/usr/local/extapps/acMeter/bin/acMeter.ini");
    printf("��ȡ�����ļ�/usr/local/extapps/acMeter/bin/acMeter.ini \n");
    if(ini!=NULL)
    {
        g_Relay485Flag= iniparser_getint(ini,"RELAY_CONF:AC485FLAG",0);
        iniparser_freedict(ini);
    }   
*/
    int ret = 0;
	uint32 len = 0;
	NOMAL_OAD_T nomalOAD = { 0 };
	nomalOAD.logicId = 0;
	nomalOAD.infoNum = 0;
	nomalOAD.oad.value = 0xF1010200;
    g_safe_mode = SECURITY_OFF;
	ret = db_read_nomal(g_hUdbCenter, &nomalOAD, sizeof(OOP_SECURITYFLAG_E), (uint8*)&g_safe_mode, &len);
	if (ret != ERR_OK || len != sizeof(OOP_SECURITYFLAG_E))
	{
		GUI_FMT_DEBUG("db_read_nomal 0xF1010200 err ret=%d, len=%u!\n", ret, len);
	}

    if (g_safe_mode == SECURITY_OFF) 
    {
        safe_status = 0;
    }
    else 
    {
    	safe_status = 1;
    }
    printf("ret=%d, len=%u!\n", ret, len);
	safe_index = 0;
}

void setSafeONOFFDraw(HDC hdc)
{
    //int       nTop = 0;
    char      czbuf[32];
    RECT      rc;
    //uint8     i = 0;

	SelectFont(hdc,g_guiComm.fontSmall);

    //������ʾ
    ITEM_NOT_SELECT;
    rc.left  = 15;
    rc.right = g_guiComm.nWidth;
    rc.top   = 20;
    rc.bottom = rc.top+16;
    sprintf(czbuf, "��ȫģʽ:");//�òɼ��ܿ��أ������ܿ���
    DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

    rc.left  = 100;
    rc.right = g_guiComm.nWidth;
    //rc.top   += 20;
    rc.bottom = rc.top+16;

	if(safe_index == 0)
     {
         ITEM_SELECT;
         TextOut(hdc, rc.left, rc.top, g_safeModel_list[safe_status]);
         ITEM_NOT_SELECT;
     }
     else
     {
         TextOut(hdc, rc.left, rc.top, g_safeModel_list[safe_status]);
     }

	ITEM_NOT_SELECT;
	rc.left  = 15;
	rc.right = g_guiComm.nWidth;
	rc.top   += 16;
	rc.bottom = rc.top+16;
	sprintf(czbuf, "MACУ��:");
	DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

	rc.left  = 100;
	rc.right = g_guiComm.nWidth;
	//rc.top   += 20;
	rc.bottom = rc.top+16;

	if(safe_index == 1)
	{
	 ITEM_SELECT;
	 TextOut(hdc, rc.left, rc.top, g_macModel_list[mac_status]);
	 ITEM_NOT_SELECT;
	}
	else
	{
	 TextOut(hdc, rc.left, rc.top, g_macModel_list[mac_status]);
	}
	
	ITEM_NOT_SELECT;
	rc.left  = 15;
	rc.right = g_guiComm.nWidth;
	rc.top   += 16;
	rc.bottom = rc.top+16;
	sprintf(czbuf, "�ز�MACУ��:");
	DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

	rc.left  = 100;
	rc.right = g_guiComm.nWidth;
	//rc.top   += 20;
	rc.bottom = rc.top+16;

	if(safe_index == 2)
	{
    	 ITEM_SELECT;
    	 TextOut(hdc, rc.left, rc.top, g_macModel_list[hplc_mac_status]);
    	 ITEM_NOT_SELECT;
	}
	else
	{
	    TextOut(hdc, rc.left, rc.top, g_macModel_list[hplc_mac_status]);
	}


    ITEM_NOT_SELECT;
	rc.left  = 15;
	rc.right = g_guiComm.nWidth;
	rc.top   += 16;
	rc.bottom = rc.top+16;
	sprintf(czbuf, "����485ͨ��:");
	DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

	rc.left  = 100;
	rc.right = g_guiComm.nWidth;
	//rc.top   += 20;
	rc.bottom = rc.top+16;

	if(safe_index == 3)
	{
    	 ITEM_SELECT;
    	 TextOut(hdc, rc.left, rc.top, g_ac_485_list[g_Relay485Flag]);
    	 ITEM_NOT_SELECT;
	}
	else
	{
	    TextOut(hdc, rc.left, rc.top, g_ac_485_list[g_Relay485Flag]);
	}
    ITEM_NOT_SELECT;
	rc.left  = 15;
	rc.right = g_guiComm.nWidth;
	rc.top   += 16;
	rc.bottom = rc.top+16;
    sprintf(czbuf, "·��ѧϰ:");
	DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

	rc.left  = 100;
	rc.right = g_guiComm.nWidth;
	//rc.top   += 20;
	rc.bottom = rc.top+16;

	if(safe_index == 4)
	{
	 ITEM_SELECT;
	 TextOut(hdc, rc.left, rc.top, g_routerstudy_list[router_state]);
	 ITEM_NOT_SELECT;
	}
	else
	{
	 TextOut(hdc, rc.left, rc.top, g_routerstudy_list[router_state]);
	}
    
    ITEM_NOT_SELECT;
	rc.left  = 15;
	rc.right = g_guiComm.nWidth;
	rc.top   += 16;
	rc.bottom = rc.top+16;
    sprintf(czbuf, "���Ӳɼ�:");
	DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

	rc.left  = 100;
	rc.right = g_guiComm.nWidth;
	//rc.top   += 20;
	rc.bottom = rc.top+16;

	if(safe_index == 5)
	{
	 ITEM_SELECT;
	 TextOut(hdc, rc.left, rc.top, g_minutefunc_list[g_minutefunclag]);
	 ITEM_NOT_SELECT;
	}
	else
	{
	 TextOut(hdc, rc.left, rc.top, g_minutefunc_list[g_minutefunclag]);
	}
     if(safe_index == 6)
     {
         ITEM_SELECT;
         TextOut(hdc, 30, 118, "ȷ��");
         ITEM_NOT_SELECT;
     }
     else
     {
         TextOut(hdc, 30, 118, "ȷ��");
     }

     PopListDraw(hdc);
}

 BOOL setSafeONOFFMsg(int nKey)
 {
 	 //uint8     pRD;
 	 //uint8     pRD1 = 0x55;

     int ret = 0;
     // ����Listѡ����
     if(g_guiComm.bPopList == TRUE)
     {
         int nSel;
         PopListKeyMsg(nKey, &nSel);
         if(nKey == KEY_OK)
         {
             if(safe_index == 0)
             	safe_status = nSel;
             else if(safe_index == 1)
             	mac_status = nSel;
             else if(safe_index == 2)
             	hplc_mac_status = nSel;
             else if(safe_index == 3)
             	g_Relay485Flag = nSel;
             else if(safe_index == 4)
             	router_state = nSel;
             else if(safe_index == 5)
             	g_minutefunclag = nSel;
             UpdateWindow(g_guiComm.hMainWnd, TRUE);
         }
         return TRUE;
     }

     switch(nKey)
     {
     case KEY_UP:
         safe_index--;
         if(safe_index < 0)
             safe_index = 6;
         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     case KEY_DOWN:
         safe_index ++;
         if(safe_index > 6)
             safe_index = 0;
         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     case KEY_OK:
         if(ValidatePwd() == TRUE)
         {// ȷ�ϴ���
            switch(safe_index)
            {
            	case 0:
					PopList(g_safeModel_list, 2, g_safeModel_list[safe_status], 120, 30, FALSE);
					break;
             	case 1:
					PopList(g_macModel_list, 2, g_macModel_list[mac_status], 120, 45, FALSE);
					break;
				case 2:
					PopList(g_macModel_list, 2, g_macModel_list[hplc_mac_status], 120, 60, FALSE);
					break;
                case 3:
					PopList(g_ac_485_list, 2, g_ac_485_list[g_Relay485Flag], 120, 75, FALSE);
					break;
                case 4:
					PopList(g_routerstudy_list, 2, g_routerstudy_list[router_state], 120, 90, FALSE);
					break;
                case 5:
					PopList(g_minutefunc_list, 2, g_minutefunc_list[g_minutefunclag], 120, 105, FALSE);
					break;
             	case 6:
                    write_safe_cfg("/data/app/desktopGui/safemode.ini");
                    ret = write_safemode();
//                    write_cfg_by_key("/data/app/desktopGui/safemode.ini", "SAFE_BUTTON", safe_status);
//                    write_cfg_by_key("/data/app/taskManager/taskManager.ini", "SAFE_BUTTON", mac_status);
//                    write_cfg_by_key("/usr/local/extapps/acMeter/bin/acMeter.ini", "AC485FLAG", g_Relay485Flag);

					UpdateWindow(g_guiComm.hMainWnd, TRUE);
                    g_bpop = TRUE;
                    if(ret == 0)
                    {
                        PopMsg("       ���óɹ�!\n     ��������Ӧ��...");
                        RebootTerminal();
                    }
                    else
                    {
                        MessageBox("��ʾ\n\n���ð�ȫģʽʧ��", MB_OK);
                        g_bpop = FALSE;

//                        PopMsg("       ���ð�ȫģʽʧ��!");
                    }
                    
					break;
             	default:
                 	break;
             }
             return TRUE;
         }
     }

	 return FALSE;
}


PLUGIN_INFO g_layersetSafeONOFF=
{
    0x000c0003, // 0x000c0003
    setSafeONOFFInit,
    NULL,
    setSafeONOFFDraw,
    NULL,
    setSafeONOFFMsg,
    NULL
};

uint8 sshsetindex;
void InitSSHSet(DWORD dwPluginId)
{
	sshsetindex = 0;
}

void SSHSetDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return;

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top+16;
    char pcz[20];
	strcpy(pcz, "SSH��������");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if(sshsetindex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "����");
    }

    nLeft = 110;
    if(sshsetindex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "�ر�");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "�ر�");
    }
}

//choice 0 ����ssh 1 �ر�ssh
void sshset(uint8 choice)
{
    char cmd[100]={0};
    if(choice ==0)
    {
        sprintf(cmd,"/usr/sbin/sshd -D &");
    }else 
    {
        sprintf(cmd,"pkill sshd");
    }
    printf("sshset %s \n",cmd);
    system(cmd);
}
BOOL SSHSetKey(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
        if(sshsetindex>0)
            sshsetindex --;
        else
            sshsetindex = 1;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
    case KEY_DOWN:
        if(sshsetindex<1)
            sshsetindex++;
        else
            sshsetindex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
		if(ValidatePwd() == TRUE)
        {
        	g_bpop = FALSE;
	        switch(sshsetindex)
	        {
	            case 0:
	                if(FALSE == MessageBox("��ʾ\n�Ƿ���SSH����\n", MB_OKCANCEL))
	                {
	                    return TRUE;
	                }
	                sshset(0);
	                CancelMenu();
	                return TRUE;
	            case 1:

	                if(FALSE == MessageBox("��ʾ\n�Ƿ�ر�SSH����\n", MB_OKCANCEL))
	                {
	                    return TRUE;
	                }
	                sshset(1);
	                CancelMenu();
	                return TRUE;
	        }
//        g_bpop = FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

//SSH��������
PLUGIN_INFO g_SSHSet_Layer = {
	0x00A000D0/*id*/, 
	InitSSHSet/*InitLayer*/,
	NULL/*UninitLayer*/,
	SSHSetDraw/*Draw*/,
	NULL/*Timer*/,
	SSHSetKey/*keymsg*/,
	NULL/*ipc*/
};

extern BOOL CanUseUDisks();
extern void ModuleDeleteDir(const char *dir);

uint8 g_tcpdump_set_index;  /* 0-ץFE0; 1-ץlo; 2-ץdocker0; 3-ȫץ; 4-�ر�; 5-����U��; 6-���ز�ɾ��ץ��Ŀ¼ */
uint8 g_tcpdump_status;     /* 0-δ����; 1-�ѿ���; 2-�ѹر�; 3-���ڿ����ļ�; 4-�����ļ��ɹ�; 5-����Ŀ¼ʧ�� */

#define TCPDUMP_FILE_DIR "/tmp/tcpdump"
#define TCPDUMP_SINGLE_FILE_SIZE 100     /* �����ļ���С����λMB */


pthread_t    g_TCPDumpCaptureThreadId;  /* tcpdumpץ���߳�ID */
pthread_t    g_TCPDumpCopyThreadId;     /* tcpdump�����߳�ID */

void *TCPDumpCaptureThread(void *arg)
{
    char *interface = (char *)arg;
    char cmd[128];

    sprintf(cmd, "tcpdump -i %s -C %u -w %s/tcpdump.cap", interface, TCPDUMP_SINGLE_FILE_SIZE, TCPDUMP_FILE_DIR);
    system(cmd);
    /* �߳��˳�֮ǰ�Լ�ˢ�߳�id */
    g_TCPDumpCaptureThreadId = 0;
    return NULL;
}

void InitTCPDumpSet(DWORD dwPluginId)
{
    g_tcpdump_set_index = 0;
    g_tcpdump_status = 0;
    g_TCPDumpCaptureThreadId = 0;
    g_TCPDumpCopyThreadId = 0;
}

void TCPDumpSetDraw(HDC hdc)
{
    char buf[256]={0};
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top+16;
    char pcz[20];
    strcpy(pcz, "TCPDUMP��������");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);

    char *fileStatus[] = {(char*)"δ����", (char*)"�ѿ���", (char*)"�ѹر�",
                          (char*)"���ڿ����ļ�",       (char*)"�����ļ��ɹ�", (char*)"����Ŀ¼ʧ��"};

    int nTop = 75;
    int nLeft = 16;

    TextOut(hdc, nLeft, nTop, "״̬");
    TextOut(hdc, nLeft + 35, nTop, fileStatus[g_tcpdump_status]);

    if (g_tcpdump_status == 3)
    {
        nTop = 100;
        nLeft = 10;
        TextOut(hdc, nLeft, nTop, "�����ļ���...");
        nTop = 115;
        nLeft = 10;
        sprintf(buf, "ץ���ļ��� %ld MB", GetFilesSizeRecu(TCPDUMP_FILE_DIR) / (1024 * 1024));
        TextOut(hdc, nLeft, nTop, buf);
        nTop = 130;
        nLeft = 10;
        TextOut(hdc, nLeft, nTop, "��������ն˻��߰γ�U��");

        return;
    }

    nTop = 100;
    nLeft = 10;
    if(g_tcpdump_set_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ץFE0");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ץFE0");
    }

    nLeft = 55;
    if(g_tcpdump_set_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ץlo");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ץlo");
    }

    nLeft = 100;
    if(g_tcpdump_set_index == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ץdocker0");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ץdocker0");
    }

    nTop = 115;
    nLeft = 10;
    if(g_tcpdump_set_index == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȫץ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȫץ");
    }

    nLeft = 55;
    if(g_tcpdump_set_index == 4)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "�ر�");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "�ر�");
    }

    nLeft = 100;
    if(g_tcpdump_set_index == 5)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "����U��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "����U��");
    }

    nTop = 130;
    nLeft = 10;
    if(g_tcpdump_set_index == 6)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "���ز�ɾ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "���ز�ɾ��");
    }
}

void TCPDumpStop()
{
    char cmd[100]={0};

    sprintf(cmd,"ps aux | grep tcpdump | awk '{print $2}' | xargs kill -INT");
    system(cmd);

    if (0 != g_TCPDumpCaptureThreadId)
    {
        pthread_cancel(g_TCPDumpCaptureThreadId);
        g_TCPDumpCaptureThreadId = 0;
    }

    g_tcpdump_status = 2;
    printf("g_TCPDumpCaptureThreadId %lu \n", g_TCPDumpCaptureThreadId);
    return;
}

/* ��ȡָ��·�����ô��̿ռ� */
unsigned int GetAvailableDiskMB(const char *dir)
{
    struct statfs diskInfo;
    unsigned int availableDiskMB = 0;

    statfs(dir, &diskInfo);  
    unsigned long long blockSize = diskInfo.f_bsize;    //ÿ��block��������ֽ���  
    unsigned long long availableDisk = diskInfo.f_bavail * blockSize;   //���ÿռ��СByte  
    availableDiskMB = availableDisk >> 20;

    return availableDiskMB;
}

#define AVAILABLE_DISK_SIZE_MB_MIN 100
unsigned int TCPDumpDiskVolumeEnough()
{
    unsigned int availableDiskMB = GetAvailableDiskMB(TCPDUMP_FILE_DIR);

    if (availableDiskMB < AVAILABLE_DISK_SIZE_MB_MIN)
    {
        return 1;
    }

    return 0;  
}

void TCPDumpDeleteOldestFile()
{
    char cmd[256];

    sprintf(cmd, "cd %s && ls -ltar | grep cap | head -1 | awk '{print $9}' | xargs rm -rf", TCPDUMP_FILE_DIR);
    system(cmd);
}

void *TCPDumpCopyThread(void *arg)
{
    char cmd[128]={0};
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return NULL;
    }

    sprintf(cmd, "cp -rfP %s /tmp/udisk/", TCPDUMP_FILE_DIR);
    system(cmd);
    system("sync");

    printf("copythread ������� �˳�\n");
    g_tcpdump_status = 4;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    ModuleDeleteDir(TCPDUMP_FILE_DIR);
    system("umount /tmp/udisk");
    g_TCPDumpCopyThreadId = 0;
    return NULL;
}

void TCPDumpUpdateTimer(int nId)
{   
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    /* 1s��ʱ����� */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* ��TCPDumpץ��ҳ�棬���������� */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* ��TCPDumpץ��ҳ�棬��Ļ���� */

    if (g_tcpdump_status == 1 && TCPDumpDiskVolumeEnough() != 0)
    {
        //TCPDumpStop();
        TCPDumpDeleteOldestFile();
        //MessageBox("���̿ռ䲻��!");
    }

    return;
}

unsigned int TCPDumpCaptureProc(uint8 index)
{
    char *interface[4] = {(char*)"FE0", (char*)"lo", (char*)"docker0", (char*)"any"};

    ModuleDeleteDir(TCPDUMP_FILE_DIR);

    if (0 != access(TCPDUMP_FILE_DIR, F_OK))
    {
        if(0 != mkdir(TCPDUMP_FILE_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            g_tcpdump_status = 5;
            return 1;
        }
    }

    if (index < 4)
    {
        if (pthread_create(&g_TCPDumpCaptureThreadId, NULL, TCPDumpCaptureThread, interface[index]) != 0)
        {
            g_TCPDumpCaptureThreadId = 0;
            MessageBox("ϵͳæ\n�Ժ���ץ��!");
            return 1;
        }

        g_tcpdump_status = 1;
        return 0;
    }

    return 1;
}

BOOL TCPDumpSetKey(int nKey)
{
    uint8 oldStatus = 0;
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    /* �����ļ�ʱ���ܲ����˵� */
    if (g_tcpdump_status == 3)
    {
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_tcpdump_set_index > 0)
            {
                g_tcpdump_set_index--;
            }
            else
            {
                g_tcpdump_set_index = 6;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_tcpdump_set_index < 6)
            {
                g_tcpdump_set_index++;
            }
            else
            {
                g_tcpdump_set_index = 0;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
			if (ValidatePwd() == TRUE)
            {
                g_bpop = FALSE;
	            switch(g_tcpdump_set_index)
	            {
                
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    {
                        if (g_TCPDumpCaptureThreadId != 0)
                        {
                            MessageBox("��ʾ\ntcpdump����ץ��!\n����ֹͣץ��\n");
                            return TRUE;
                        }

                        if (FALSE == MessageBox("��ʾ\n�Ƿ���tcpdumpץ������\n", MB_OKCANCEL))
                        {
                            return TRUE;
                        }

                        TCPDumpCaptureProc(g_tcpdump_set_index);
                        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                        return TRUE;
                    }
                    case 4:
                    {
                        if (FALSE == MessageBox("��ʾ\n�Ƿ�ر�tcpdumpץ������\n", MB_OKCANCEL))
                        {
                            return TRUE;
                        }

                        TCPDumpStop();
                        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                        return TRUE;
                    }
                    case 5:
                    {
                        if (access(TCPDUMP_FILE_DIR, F_OK) != 0)
                        {
                            MessageBox("��ʾ\nδ��⵽ץ���ļ�!\n����ץ��\n");
                            return TRUE;
                        }

                        if (g_TCPDumpCaptureThreadId != 0)
                        {
                            MessageBox("��ʾ\ntcpdump����ץ��!\n����ֹͣץ��\n");
                            return TRUE;
                        }

                        if (FALSE == CanUseUDisks())
                        {
                            MessageBox("U��û�в���\n\n���ܿ����ļ�!");
                            return TRUE;
                        }

                        oldStatus = g_tcpdump_status;
                        g_tcpdump_status = 3;
                        UpdateWindow(pg_guicomm->hMainWnd, TRUE);

                        if (pthread_create(&g_TCPDumpCopyThreadId, NULL, TCPDumpCopyThread, NULL) != 0)
                        {
                            g_TCPDumpCopyThreadId = 0;
                            g_tcpdump_status = oldStatus;
                            MessageBox("ϵͳæ\n�Ժ��ٿ���!");
                            return TRUE;
                        }

                        return TRUE;
                    }
                    case 6:
                    {
                        if (FALSE == MessageBox("��ʾ\n�Ƿ񷵻ز�ɾ��\n�ն��ڵ�ץ���ļ�\n", MB_OKCANCEL))
                        {
                            return TRUE;
                        }

                        ModuleDeleteDir(TCPDUMP_FILE_DIR);
                        CancelMenu();
                        return TRUE;
                    }
                }
            }

            return TRUE;
    }

    return FALSE;
}

//TCPDump��������
PLUGIN_INFO g_TCPDump_Set_Layer = {
    0x00A000D1,         /*id*/
    InitTCPDumpSet,     /*InitLayer*/
    NULL,               /*UninitLayer*/
    TCPDumpSetDraw,     /*Draw*/
    TCPDumpUpdateTimer, /*Timer*/
    TCPDumpSetKey,      /*keymsg*/
    NULL                /*ipc*/
};

uint8 passwdsetindex;
char g_curtmnPasswd[64] = "Zgdky@guest123";

void passwdchg(char* pNewStr, void* pPara)
{
    memset(g_curtmnPasswd, '\0', strlen(g_curtmnPasswd));
    memcpy(g_curtmnPasswd, pNewStr, strlen(pNewStr));
}

void passwdset(void)
{
    char cmd[100]={0};

    sprintf(cmd,"echo sysadm:%s|chpasswd",g_curtmnPasswd);
    system(cmd);
}

void InitPasswdSet(DWORD dwPluginId)
{
    passwdsetindex = 2;
    g_bpop = TRUE;
}

void UninitPasswdSet()
{
    g_bpop = FALSE;
}


void PasswdSetDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
        return;

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 36;
    rc.bottom = rc.top+16;
    const char* pcz = "sysadm����";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);

    rc.top = 66;
    rc.bottom = rc.top+16;

    if(passwdsetindex == 0)
    {
        ITEM_SELECT;
        DrawText(hdc, g_curtmnPasswd, strlen(g_curtmnPasswd), &rc, DT_CENTER|DT_VCENTER);
        ITEM_NOT_SELECT;
    }
    else
    {
        DrawText(hdc, g_curtmnPasswd, strlen(g_curtmnPasswd), &rc, DT_CENTER|DT_VCENTER);
    }

     SelectFont(hdc,pg_guicomm->fontBig);
     int nTop = 98;
     int nLeft = 26;
    if(passwdsetindex == 1)
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
    if(passwdsetindex == 2)
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

BOOL PasswdSetKey(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
        if(passwdsetindex>0)
            passwdsetindex --;
        else
            passwdsetindex = 2;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
    case KEY_DOWN:
        if(passwdsetindex<2)
            passwdsetindex++;
        else
            passwdsetindex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch(passwdsetindex)
        {
            case 0:
                InputBox(g_curtmnPasswd, passwdchg, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return FALSE;

                
            case 1:
                CancelMenu();
                break;

            case 2:
                if(ValidatePwd() == TRUE)
                {
                    passwdset();
                    MessageBox("�����޸ĳɹ���");
                    CancelMenu();
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                }

                break;
        }
        return TRUE;
    }
    
    return FALSE;
}

//�����޸�����
PLUGIN_INFO g_passwd_Layer = {
	0x00A000F2/*id*/, 
	InitPasswdSet/*InitLayer*/,
	UninitPasswdSet/*UninitLayer*/,
	PasswdSetDraw/*Draw*/,
	NULL/*Timer*/,
	PasswdSetKey/*keymsg*/,
	NULL/*ipc*/
};

uint8 logClearindex;

void LogClear(void)
{
    char cmd[100] = {0};

    //sprintf(cmd, "find /data/app/ -name log -type d |xargs rm -rfv");
    sprintf(cmd,"find /data/app -path '/data/app/*/log/*' |xargs sudo rm -rf");
    system(cmd);
}

void InitLogClear(DWORD dwPluginId)
{
    logClearindex = 1;
}

void LogClearDraw(HDC hdc)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    SelectFont(hdc, pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 36;
    rc.bottom = rc.top + 16;
    const char *pcz = "��־���";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, pg_guicomm->fontSmall);

    rc.top = 66;
    rc.bottom = rc.top + 16;

    SelectFont(hdc, pg_guicomm->fontBig);
    int nTop = 98;
    int nLeft = 26;
    if (logClearindex == 0)
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
    if (logClearindex == 1)
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

BOOL LogClearKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
            if (logClearindex > 0)
            {
                logClearindex--;
            }
            else
            {
                logClearindex = 1;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case KEY_RIGHT:
        case KEY_DOWN:
            if (logClearindex < 1)
            {
                logClearindex++;
            }
            else
            {
                logClearindex = 0;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        case KEY_OK:
            switch (logClearindex)
            {
                case 0:
                    CancelMenu();
                    return TRUE;

                case 1:
                    LogClear();
                    MessageBox("��־����ɹ���");
                    CancelMenu();
                    return TRUE;
            }
            return TRUE;
    }
    
    return FALSE;
}

//��־���
PLUGIN_INFO g_LogClear_Layer = {
    0x00A000F3,     /* id */
    InitLogClear,   /* InitLayer */
    NULL,           /* UninitLayer */
    LogClearDraw,   /* Draw */
    NULL,           /* Timer */
    LogClearKey,    /* keymsg */
    NULL            /* ipc */
};

static int g_SlotIndex = 0;     //ѡ����: 0-������λ1; 1-������λ2; 2-������λ3; 3-������λ4; 4-������λ5; 5-����
static unsigned int g_ModuleUpdateStatus = 0;    //״̬    : 0-δ��ʼ����;1-������װ�ļ�;2-����Ŀ¼ʧ��;3-����������Ϣ;
                                       //          4-�����ڴ�ʧ��;5-δ�ҵ�U����İ�װĿ¼;6-ȱ�������ļ�;7-�ȴ�ģ�����������Ӧ��Ϣ;8-�յ�ģ�����������Ӧ��Ϣ
unsigned int g_ModuleWaitTimer = 0;


#define MODULE_UPDATE_STATUS_INIT       0
#define MODULE_UPDATE_STATUS_COPY       1
#define MODULE_UPDATE_STATUS_MKDIR_ERR  2
#define MODULE_UPDATE_STATUS_SEND_MSG   3
#define MODULE_UPDATE_STATUS_MALLOC_ERR 4
#define MODULE_UPDATE_STATUS_NO_DIR     5
#define MODULE_UPDATE_STATUS_NO_BIN     6
#define MODULE_UPDATE_STATUS_WAIT_ACK   7
#define MODULE_UPDATE_STATUS_ACK        8


unsigned int ModuleCopyPkg(unsigned int slotId, char *srcFolder, char *dstFolder)
{
    char cmd[256];

    if (0 == access(dstFolder, F_OK))
    {
        sprintf(cmd, "rm -rf %s", dstFolder);
        system(cmd);
    }

    if (0 != access(dstFolder, F_OK))
    {
        if(0 != mkdir(dstFolder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_MKDIR_ERR;
            return 1;
        }
    }

    if (0 != access(srcFolder, F_OK))
    {
        g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_NO_DIR;
        return 1;
    }

    sprintf(cmd, "cp -r %s/* %s/", srcFolder, dstFolder);
    system(cmd);
    return 0;
}

unsigned int ModuleCheckPkg(unsigned int slotId, char *dstFolder, char *pkgPath)
{
    DIR           *dir;
    struct dirent *ent;

    if ((dir = opendir(dstFolder)) == NULL)
    {
        g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_NO_BIN;
        return 1;
    }
    
    while ((ent = readdir(dir)) != NULL)
    {
        if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0))
        {
            continue;
        }
        else if (ent->d_type == 8)    /* 8:�ļ� */
        {
            sprintf(pkgPath, "%s/%s", dstFolder, ent->d_name);
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_SEND_MSG;
            closedir(dir);
            return 0;
        }
    }

    g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_NO_BIN;
    closedir(dir);
    return 1;
}

unsigned int ModuleSendMsg(GUI_COMM *pg_guicomm, unsigned int slotId, char *pkgPath)
{
    unsigned int updateSlotId = slotId + 1; /* ��λ�Ŵ�1��ʼ��� */
    unsigned int len = 0;
    uint8 pkgPathLen = strlen(pkgPath);
    uint32 msgLen = sizeof(unsigned int) + 1 + pkgPathLen; /* 1:pkgPathLen�ֶγ��� */
    unsigned int offset = 0;
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    char topic[MESSAGE_ADDR_LEN+1] = {0};
    const char *desktopGui = "M-desktopGui";
    const char *mapManager = "M-mapManager";

    if (g_ModuleUpdateStatus != MODULE_UPDATE_STATUS_SEND_MSG)
    {
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);

    len = sizeof(MESSAGE_INFO_T) + msgLen;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(len);

    if (NULL == pMsgInfo)
    {
        printf("[0007]%s malloc failed %d\n", __FUNCTION__, len);
        g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_MALLOC_ERR;
        return 1;
    }

    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = gui_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_MODULE;
    pMsgInfo->IOP = MODULE_IOP_FILE_TRANSFER;
    
    pMsgInfo->sourlen = strlen((char *)desktopGui);
    memcpy(pMsgInfo->souraddr, desktopGui, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(mapManager);
    memcpy(pMsgInfo->destaddr, mapManager, pMsgInfo->destlen);

    pMsgInfo->msglen = msgLen;
    appmsg_memcpy_r(pMsgInfo->playload, (char *)&updateSlotId, sizeof(uint32));
    offset += sizeof(uint32);
    pMsgInfo->playload[offset] = pkgPathLen;
    offset += 1;
    memcpy(pMsgInfo->playload + offset, pkgPath, pkgPathLen);

    snprintf(topic, MESSAGE_ADDR_LEN+1,"%s/%s", desktopGui, pMsgInfo->destaddr);
    ret = gui_send_msg(topic, pMsgInfo, 0);

    free(pMsgInfo);
    g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_WAIT_ACK;
    g_ModuleWaitTimer = 0;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    return ret;
}

void ModuleAckProc()
{
    g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_ACK;
}

void InitModuleUpdateDraw(DWORD dwPluginId)
{
    g_SlotIndex = 0;
    g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
    g_ModuleWaitTimer = 0;
}

void ModuleUpdateDraw(HDC hdc)
{
    int nTop = 0;
    int nLeft = 0;
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (g_ModuleUpdateStatus == MODULE_UPDATE_STATUS_COPY)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�����ļ���...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��ʱԼ10����");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��Ҫ�ε�U��");

        return;
    }

    if (g_ModuleUpdateStatus == MODULE_UPDATE_STATUS_SEND_MSG)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "���ڷ���������Ϣ...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��ʱԼ10����");
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "���԰ε�U��");

        return;
    }

    if (g_ModuleUpdateStatus == MODULE_UPDATE_STATUS_WAIT_ACK)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�ȴ���Ӧ��Ϣ��...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��ʱԼ10����");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "���԰ε�U��");

        return;
    }

    nLeft = 40;
    nTop = 20;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (0 == g_SlotIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "������λ1");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "������λ1");
    }

    nTop += 20;

    if (1 == g_SlotIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "������λ2");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "������λ2");
    }

    nTop += 20;

    if (2 == g_SlotIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "������λ3");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "������λ3");
    }

    nTop += 20;

    if (3 == g_SlotIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "������λ4");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "������λ4");
    }

    nTop += 20;

    if (4 == g_SlotIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "������λ5");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "������λ5");
    }

    nTop += 20;

    if (5 == g_SlotIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "����");
    }

    PopListDraw(hdc);
}

void ModuleUpdateTimer(int nId)
{   
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    /* 1s��ʱ����� */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* ��ģ������ҳ�棬���������� */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* ��ģ������ҳ�棬��Ļ���� */

    switch (g_ModuleUpdateStatus)
    {
        case MODULE_UPDATE_STATUS_INIT: //δ��ʼ��������ʼҳ��
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case MODULE_UPDATE_STATUS_COPY: //�����ļ�����
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case MODULE_UPDATE_STATUS_MKDIR_ERR: //����Ŀ¼ʧ��
        {
            MessageBox("����Ŀ¼ʧ�ܣ�");
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case MODULE_UPDATE_STATUS_SEND_MSG: //����������Ϣ
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case MODULE_UPDATE_STATUS_MALLOC_ERR: //�����ڴ�ʧ��
        {
            MessageBox("�����ڴ�ʧ�ܣ�");
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case MODULE_UPDATE_STATUS_NO_DIR: //δ�ҵ�U����İ�װĿ¼
        {
            MessageBox("δ�ҵ�U����İ�װĿ¼\n����U���Ƿ����");
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case MODULE_UPDATE_STATUS_NO_BIN: //ȱ�������ļ�
        {
            MessageBox("ȱ�ٰ�װ�ű���");
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case MODULE_UPDATE_STATUS_WAIT_ACK: //�ȴ�ģ�����������Ӧ��Ϣ
        {
            if (g_ModuleWaitTimer <= 10)
            {
                g_ModuleWaitTimer++;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            }
            else
            {
                MessageBox("�ȴ���Ӧ��Ϣ��ʱ��\n��������ģ��˵�\n��ѯ�������");
                g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
                g_ModuleWaitTimer = 0;
            }
            break;
        }
        case MODULE_UPDATE_STATUS_ACK: //�յ�ģ�����������Ӧ��Ϣ
        {
            MessageBox("�յ�ģ���������Ӧ��Ϣ\n��������ģ��˵�\n��ѯ�������");
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
            break;
        }
        default: 
        {
            g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_INIT;
            break;
        }
    }

}

/* ����ɵ����߱�֤�Ϸ��� */
unsigned int ModuleInstallProc(GUI_COMM *pg_guicomm, unsigned int slotId)
{
    const char *message = "����\n�����������벻Ҫ�ϵ�\n�Ƿ�����";
    char *slotNameCh[] = {(char *)"��λ1", (char *)"��λ2", (char *)"��λ3", (char *)"��λ4", (char *)"��λ5"};
    char *slotNameEn[] = {(char *)"slot1", (char *)"slot2", (char *)"slot3", (char *)"slot4", (char *)"slot5"};
    char buf[200] = {0};
    char srcFolder[200] = {0};
    char dstFolder[200] = {0};
    char pkgPath[PATH_MAX_LEN + 1] = {0};

    if (ValidatePwd() == TRUE)
    {
        sprintf(buf, "%s%s", message, slotNameCh[slotId]);
        if (FALSE == MessageBox(buf, MB_OKCANCEL))
        {
            return 0;
        }
    }
    else
    {
        return 1;
    }

    g_ModuleUpdateStatus = MODULE_UPDATE_STATUS_COPY;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);

    sprintf(srcFolder, "%s%s", "/tmp/udisk/", slotNameEn[slotId]);
    sprintf(dstFolder, "%s%s", "/tmp/", slotNameEn[slotId]);

    if (ModuleCopyPkg(slotId, srcFolder, dstFolder) != 0)
    {
        return 1;
    }

    if (ModuleCheckPkg(slotId, dstFolder, pkgPath) != 0)
    {
        return 1;
    }

    if (ModuleSendMsg(pg_guicomm, slotId, pkgPath) != 0)
    {
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    return 0;
}

void ModuleDeleteDir(const char *dir)
{
    char cmd[256];

    if (0 == access(dir, F_OK))
    {
        sprintf(cmd, "rm -rf %s", dir);
        system(cmd);
    }
}

BOOL ModuleUpdateKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    /* δ��ʼ����״̬�²��ܲ����˵� */
    if (g_ModuleUpdateStatus != MODULE_UPDATE_STATUS_INIT)
    {
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            g_SlotIndex--;

            if (g_SlotIndex < 0)
            {
                g_SlotIndex = 5;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            g_SlotIndex++;

            if (g_SlotIndex > 5)
            {
                g_SlotIndex = 0;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_SlotIndex)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                {
                    ModuleInstallProc(pg_guicomm, g_SlotIndex);
                    return TRUE;
                }

                case 5: /* �˳� */
                {
                    ModuleDeleteDir("/tmp/slot1");
                    ModuleDeleteDir("/tmp/slot2");
                    ModuleDeleteDir("/tmp/slot3");
                    ModuleDeleteDir("/tmp/slot4");
                    ModuleDeleteDir("/tmp/slot5");
                    CancelMenu();
                    return TRUE;
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}


//ģ������
PLUGIN_INFO g_ModuleUpdate = {
    0x00A000F4,             /* id */
    InitModuleUpdateDraw,   /* InitLayer */
    NULL,                   /* UninitLayer */
    ModuleUpdateDraw,       /* Draw */
    ModuleUpdateTimer,      /* Timer */
    ModuleUpdateKey,        /* keymsg */
    NULL                    /* ipc */
};

#define MAX_RESULT_LEN 4096
#define MAX_COMMAND_LEN 256
#define MAX_PAGE_NUM 20
#define ONE_PAGE_LINE_NUM 9
#define ONE_LINE_BYTE_NUM 25


char g_ResultBuf[MAX_RESULT_LEN + 1] = {0};
char g_EchoPrintBuf[MAX_PAGE_NUM][ONE_PAGE_LINE_NUM][ONE_LINE_BYTE_NUM + 1] = {0};

/**
*********************************************************************
* @brief�� ����ִ�к���
* @param[in]��
               cmd          ��ִ�е�����
* @param[out]��
               result       ����ִ�н��
* @return�� ����ֵ˵�� success  0
                     fail    1
*********************************************************************
*/
uint32 CmdExecute(const char *cmd, char *result)
{
    //��Ч�Լ��
    if (NULL == cmd || NULL == result)
    {
        //dzlog_warn("%s, invalid param", __FUNCTION__);
        return -1;
    }
    char tmpBuf[512 + 1];
    char tmpCmd[MAX_COMMAND_LEN + 5] = {0};
    FILE* fp = NULL;
    sprintf(tmpCmd, "%s 2>&1", cmd);
    //����ִ�У��򿪹ܵ�
    if ((fp = popen(tmpCmd, "r")) != NULL)
    {
        //��ȡ����ִ����־
        while (fgets(tmpBuf, 512, fp) != NULL)
        {
            if (strlen(result) >= MAX_RESULT_LEN)
            {
                break;
            }

            if (strlen(result) + strlen(tmpBuf) >= MAX_RESULT_LEN)
            {
                tmpBuf[MAX_RESULT_LEN - strlen(result)] = '\0';
                strcat(result, tmpBuf);
                break;
            }

            strcat(result, tmpBuf);
        }
        //�رչܵ�
        pclose(fp);
        fp = NULL;
    }
    else
    {
        //dzlog_error("%s, popen %s error", __FUNCTION__, tmpCmd);
        sprintf(result, "popen failed. Cmd:%s", tmpCmd);
        return 1;
    }

    return 0;
}

uint32 g_EchoPrintPageIndex = 0;
uint32 g_EchoPrintPageNum = 0;

uint32 EchoPrintTransBuf(const char *inBuf)
{
    uint32 inBufLen = strlen(inBuf);
    uint32 offset = 0;
    uint32 i = 0;
    uint32 j = 0;
    uint32 k = 0;
    uint32 lineFullFlag = 0; /* 1:��Ļһ��д�����У���һ���ַ��ǻ��з����ٻ��� */

    for (i = 0; i < MAX_PAGE_NUM; i++)
    {
        for (j = 0; j < ONE_PAGE_LINE_NUM; j++)
        {
            for (k = 0; k < ONE_LINE_BYTE_NUM; k++)
            {
                if (offset < inBufLen && offset < MAX_RESULT_LEN)
                {
                    if (*(inBuf + offset) == '\n')
                    {
                        offset++;

                        if (lineFullFlag != 1)
                        {
                            break;
                        }
                    }

                    lineFullFlag = 0;
                    g_EchoPrintBuf[i][j][k] = *(inBuf + offset);
                    offset++;
                }
                else
                {
                    return 0;
                }
            }

            if (k == ONE_LINE_BYTE_NUM)
            {
                lineFullFlag = 1;
            }
        }

        if (!(offset < inBufLen && offset < MAX_RESULT_LEN))
        {
            /* inBuf�Ѿ�ת����ɣ������½�һҳ */
            return 0;
        }

        if (g_EchoPrintPageNum < MAX_PAGE_NUM)
        {
            g_EchoPrintPageNum++;
        }
    }

    return 0;
}

void EchoPrintInit(DWORD dwPluginId)
{
    g_EchoPrintPageIndex = 0;
    g_EchoPrintPageNum = 1;
    memset(g_EchoPrintBuf, 0, sizeof(g_EchoPrintBuf));
    EchoPrintTransBuf(g_ResultBuf);
}

void EchoPrintDraw(HDC hdc)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    SelectFont(hdc, pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 20;
    rc.bottom = rc.top + 13;

    char buf[100];
    sprintf(buf, "��%uҳ/��%uҳ �������ҳ", g_EchoPrintPageIndex + 1, g_EchoPrintPageNum);
    DrawText(hdc, buf, strlen(buf), &rc, DT_CENTER | DT_VCENTER);

    int nLeft = 5;
    int nTop = 35;
    uint32 i = 0;

    for (i = 0; i < 9; i++)
    {
        nTop = 35 + 12 * i;
        TextOut(hdc, nLeft, nTop, g_EchoPrintBuf[g_EchoPrintPageIndex][i]);
    }
}

BOOL EchoPrintKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_EchoPrintPageIndex > 0)
            {
                g_EchoPrintPageIndex--;
            }
            else
            {
                g_EchoPrintPageIndex = g_EchoPrintPageNum - 1;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_EchoPrintPageIndex < g_EchoPrintPageNum - 1)
            {
                g_EchoPrintPageIndex++;
            }
            else
            {
                g_EchoPrintPageIndex = 0;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
    }

    return FALSE;
}

/* ���Դ�ӡ */
PLUGIN_INFO g_EchoPrint = {
    0x10A000F5,         /* id */
    EchoPrintInit,      /* InitLayer */
    NULL,               /* UninitLayer */
    EchoPrintDraw,      /* Draw */
    NULL,               /* Timer */
    EchoPrintKey,       /* keymsg */
    NULL                /* ipc */
};

uint8 g_SystemCmdIndex = 0;
uint8 g_SystemCmdStatus = 0; /* 0-δִ��; 1-����ִ�� */
char g_SystemCmdStr[MAX_COMMAND_LEN] = "route -n";

void SystemCmdChg(char *pNewStr, void *pPara)
{
    memset(g_SystemCmdStr, '\0', strlen(g_SystemCmdStr));
    memcpy(g_SystemCmdStr, pNewStr, strlen(pNewStr));
}

void *SystemCmdProcThread(void *arg)
{
    GUI_COMM *pg_guicomm = &g_guiComm;
    
    if (pg_guicomm == NULL)
    {
        return NULL;
    }

    g_SystemCmdStatus = 1;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    //system(g_SystemCmdStr);
    memset(g_ResultBuf, 0 , sizeof(g_ResultBuf));
    CmdExecute(g_SystemCmdStr, g_ResultBuf);
    g_SystemCmdStatus = 0;
    MessageBox("System������ִ�У�");
    PopLayer(g_EchoPrint.dwId);
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    return NULL;
}

void SystemCmdInit(DWORD dwPluginId)
{
    g_SystemCmdIndex = 0;
    g_SystemCmdStatus = 0;
    g_bpop = TRUE;
}

void SystemCmdUninit()
{
    g_bpop = FALSE;
}

void SystemCmdDraw(HDC hdc)
{
    GUI_COMM *pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
    {
        return;
    }

    SelectFont(hdc, pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 20;
    rc.bottom = rc.top + 16;
    const char *pcz = "System����";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);

    rc.top = 40;
    rc.bottom = rc.top + 56;

    if (g_SystemCmdIndex == 0)
    {
        ITEM_SELECT;
        DrawText(hdc, g_SystemCmdStr, strlen(g_SystemCmdStr), &rc, DT_CENTER | DT_VCENTER);
        ITEM_NOT_SELECT;
    }
    else
    {
        DrawText(hdc, g_SystemCmdStr, strlen(g_SystemCmdStr), &rc, DT_CENTER | DT_VCENTER);
    }

    SelectFont(hdc, pg_guicomm->fontBig);
    int nTop = 100;
    int nLeft = 5;

    if (g_SystemCmdStatus == 1)
    {
        nLeft = 10;
        TextOut(hdc, nLeft, nTop, "ִ��System������...");
        nTop = 120;
        TextOut(hdc, nLeft, nTop, "��������ն�");
        return;
    }

    if (g_SystemCmdIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "����");
    }

    nLeft = 50;
    if (g_SystemCmdIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ִ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ִ��");
    }

    nLeft = 95;
    if (g_SystemCmdIndex == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "�鿴����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "�鿴����");
    }
}

void SystemCmdUpdateTimer(int nId)
{
    /* 1s��ʱ����� */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* ��System����ҳ�棬���������� */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* ��System����ҳ�棬��Ļ���� */
}

BOOL SystemCmdKey(int nKey)
{
    pthread_t threadId;
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    if (g_SystemCmdStatus == 1)
    {
        /* System��������ִ�У����ܲ����˵� */
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_SystemCmdIndex > 0)
            {
                g_SystemCmdIndex--;
            }
            else
            {
                g_SystemCmdIndex = 3;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_SystemCmdIndex < 3)
            {
                g_SystemCmdIndex++;
            }
            else
            {
                g_SystemCmdIndex = 0;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_SystemCmdIndex)
            {
                case 0:
                {
                    InputBox(g_SystemCmdStr, SystemCmdChg, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x08);
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                    return FALSE; /* InputBox()��return FALSE����ʹ�� */
                }

                case 1:
                {
                    CancelMenu();
                    break;
                }

                case 2:
                {
                    if (ValidatePwd() == TRUE)
                    {
                        if (FALSE == MessageBox("�Ƿ�ִ��System����", MB_OKCANCEL))
                        {
                            break;
                        }

                        if (pthread_create(&threadId, NULL, SystemCmdProcThread, NULL) != 0)
                        {
                            MessageBox("ϵͳæ\n�Ժ���ִ��!");
                            break;
                        }

                        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                    }

                    break;
                }

                case 3:
                {
                    PopLayer(g_EchoPrint.dwId);
                    break;
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}

/* System���� */
PLUGIN_INFO g_SystemCmd = {
    0x00A000F5,         /* id */
    SystemCmdInit,      /* InitLayer */
    SystemCmdUninit,    /* UninitLayer */
    SystemCmdDraw,      /* Draw */
    SystemCmdUpdateTimer, /* Timer */
    SystemCmdKey,       /* keymsg */
    NULL                /* ipc */
};

#if DESC("����ģ������", 1)
unsigned int g_ACModuleIndex = 0;         //ѡ����: 0-����; 1-����
unsigned int g_ACModuleUpdateStatus = 0;  //״̬    : 0-δ��ʼ����;1-������װ�ļ�;2-acMeterĿ¼������;
                                          //          3-δ�ҵ�U����İ�װĿ¼;4-ȱ�������ļ�;5-���ڰ�װ
unsigned int g_ACModuleWaitTimer = 0;


#define AC_MODULE_UPDATE_STATUS_INIT       0
#define AC_MODULE_UPDATE_STATUS_COPY       1
#define AC_MODULE_UPDATE_STATUS_NO_AC_DIR  2
#define AC_MODULE_UPDATE_STATUS_NO_UP_DIR  3
#define AC_MODULE_UPDATE_STATUS_NO_BIN     4
#define AC_MODULE_UPDATE_STATUS_INSTALL    5

#define AC_MODULE_UPDATE_SRC_FOLDER "/tmp/udisk/acupdate"
#define AC_MODULE_UPDATE_DST_FOLDER "/data/app/acMeter"
#define AC_MODULE_UPDATE_DST_FILE "/data/app/acMeter/jc_update_file"
#define AC_MODULE_UPDATE_TMP_FILE "/data/app/acMeter/jc_update_file_back"
unsigned int ACModuleCopyPkg(char *pkgPath)
{
    char cmd[256];

    if (0 != access(AC_MODULE_UPDATE_DST_FOLDER, F_OK))
    {
        g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_NO_AC_DIR;
        return 1;
    }

    ModuleDeleteDir(AC_MODULE_UPDATE_DST_FILE);
    ModuleDeleteDir(AC_MODULE_UPDATE_TMP_FILE);

    sprintf(cmd, "cp -r %s %s", pkgPath, AC_MODULE_UPDATE_DST_FILE);
    system(cmd);
    system("sync");
    g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INSTALL;
    g_ACModuleWaitTimer = 0;
    return 0;
}

unsigned int ACModuleGetPkgPath(char *pkgPath)
{
    DIR           *dir;
    struct dirent *ent;

    if ((dir = opendir(AC_MODULE_UPDATE_SRC_FOLDER)) == NULL)
    {
        g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_NO_UP_DIR;
        return 1;
    }
    
    while ((ent = readdir(dir)) != NULL)
    {
        if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0))
        {
            continue;
        }
        else if (ent->d_type == 8)    /* 8:�ļ� */
        {
            sprintf(pkgPath, "%s/%s", AC_MODULE_UPDATE_SRC_FOLDER, ent->d_name);
            closedir(dir);
            return 0;
        }
    }

    g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_NO_BIN;
    closedir(dir);
    return 1;
}

void InitACModuleUpdateDraw(DWORD dwPluginId)
{
    g_ACModuleIndex = 1;
    g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
    g_ACModuleWaitTimer = 0;
}

void ACModuleUpdateDraw(HDC hdc)
{
    int nTop = 0;
    int nLeft = 0;
    char buf[200] = {0};
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (g_ACModuleUpdateStatus == AC_MODULE_UPDATE_STATUS_COPY)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�����ļ���...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��ʱԼ1����");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��Ҫ�ε�U��");

        return;
    }

    if (g_ACModuleUpdateStatus == AC_MODULE_UPDATE_STATUS_INSTALL)
    {
        nTop = 40;
        nLeft = 16;
        sprintf(buf, "���ڰ�װ...(%u��)", g_ACModuleWaitTimer);
        TextOut(hdc, nLeft, nTop, buf);
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��ʱԼ7����");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "���԰ε�U��");

        return;
    }

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 36;
    rc.bottom = rc.top + 16;
    const char *pcz = "����ģ������";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    nTop = 98;
    nLeft = 26;

    if (g_ACModuleIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "����");
    }

    nLeft = 110;
    if (g_ACModuleIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }

    return;
}

void ACModuleUpdateTimer(int nId)
{   
    char buf[200] = {0};
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    /* 1s��ʱ����� */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* �ڽ���ģ������ҳ�棬���������� */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* �ڽ���ģ������ҳ�棬��Ļ���� */

    switch (g_ACModuleUpdateStatus)
    {
        case AC_MODULE_UPDATE_STATUS_INIT: //δ��ʼ��������ʼҳ��
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case AC_MODULE_UPDATE_STATUS_COPY: //�����ļ�����
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case AC_MODULE_UPDATE_STATUS_NO_AC_DIR: //acMeterĿ¼������
        {
            MessageBox("acMeterĿ¼�����ڣ�");
            g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case AC_MODULE_UPDATE_STATUS_NO_UP_DIR: //δ�ҵ�U����İ�װĿ¼
        {
            sprintf(buf, "δ�ҵ�U���������Ŀ¼\n%s\n����U���Ƿ����", AC_MODULE_UPDATE_SRC_FOLDER);
            MessageBox(buf);
            g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case AC_MODULE_UPDATE_STATUS_NO_BIN: //ȱ�������ļ�
        {
            MessageBox("ȱ�������ļ���");
            g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
            break;
        }
        case AC_MODULE_UPDATE_STATUS_INSTALL: //���ڰ�װ
        {
            if ((0 != access(AC_MODULE_UPDATE_DST_FILE, F_OK)) && (0 != access(AC_MODULE_UPDATE_TMP_FILE, F_OK)))
            {
                MessageBox("��װ��ɣ�\n����ycjcup.log��\n�鿴�������");
                g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
                break;
            }

            if (g_ACModuleWaitTimer <= 1000)
            {
                g_ACModuleWaitTimer++;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            }
            else
            {
                MessageBox("��װ��ʱ��\n����ycjcup.log��\n�鿴�������");
                g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
                g_ACModuleWaitTimer = 0;
            }
            break;
        }
        default: 
        {
            g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_INIT;
            break;
        }
    }

}

/* ����ɵ����߱�֤�Ϸ��� */
unsigned int ACModuleUpdateProc(GUI_COMM *pg_guicomm)
{
    char pkgPath[PATH_MAX_LEN + 1] = {0};

    if (ValidatePwd() == TRUE)
    {
        if (FALSE == MessageBox("����\n�����������벻Ҫ�ϵ�\n�Ƿ�����", MB_OKCANCEL))
        {
            return 0;
        }
    }
    else
    {
        return 1;
    }

    g_ACModuleUpdateStatus = AC_MODULE_UPDATE_STATUS_COPY;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);

    if (ACModuleGetPkgPath(pkgPath) != 0)
    {
        return 1;
    }

    if (ACModuleCopyPkg(pkgPath) != 0)
    {
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    return 0;
}

BOOL ACModuleUpdateKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    /* δ��ʼ����״̬�²��ܲ����˵� */
    if (g_ACModuleUpdateStatus != AC_MODULE_UPDATE_STATUS_INIT)
    {
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            if (g_ACModuleIndex > 0)
            {
                g_ACModuleIndex--;
            }
            else
            {
                g_ACModuleIndex = 1;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            if (g_ACModuleIndex < 1)
            {
                g_ACModuleIndex++;
            }
            else
            {
                g_ACModuleIndex = 0;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_ACModuleIndex)
            {
                case 0: /* ���� */
                {
                    CancelMenu();
                    return TRUE;
                }
                case 1: 
                {
                    ACModuleUpdateProc(pg_guicomm);
                    return TRUE;
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}


//����ģ������
PLUGIN_INFO g_ACModuleUpdate = {
    0x00A000F6,             /* id */
    InitACModuleUpdateDraw, /* InitLayer */
    NULL,                   /* UninitLayer */
    ACModuleUpdateDraw,     /* Draw */
    ACModuleUpdateTimer,    /* Timer */
    ACModuleUpdateKey,      /* keymsg */
    NULL                    /* ipc */
};
#endif

//���´���ΪЭ��������������
#if DESC("Э����������", 1)
unsigned int g_CoUpdateIndex = 0;         //ѡ����: 0-����; 1-����
unsigned int g_CoUpdateStatus = 0;  //״̬    : 0-δ��ʼ����;1-������װ�ļ�;2-Ŀ¼������;
                                    //          3-δ�ҵ�U����İ�װĿ¼;4-ȱ�������ļ�;5-���ڰ�װ
unsigned int g_CoWaitTimer = 0;


#define CO_UPDATE_STATUS_INIT       0
#define CO_UPDATE_STATUS_COPY       1
#define CO_UPDATE_STATUS_NO_AC_DIR  2
#define CO_UPDATE_STATUS_NO_UP_DIR  3
#define CO_UPDATE_STATUS_NO_BIN     4
#define CO_UPDATE_STATUS_INSTALL    5

#define CO_UPDATE_SRC_FOLDER "/tmp/udisk/coupdate"
#define CO_UPDATE_DST_FOLDER "/data/app/rspSample"
#define CO_UPDATE_DST_FILE   "/data/app/rspSample/update_file.bin"
#define CO_UPDATE_START_FILE   "/data/app/rspSample/data/startupdate"
#define CO_UPDATE_END_FILE   "/data/app/rspSample/data/endupdate"
unsigned int COModuleCopyPkg(char *pkgPath)
{
    char cmd[256];

    if (0 != access(CO_UPDATE_DST_FOLDER, F_OK))
    {
        g_CoUpdateStatus = CO_UPDATE_STATUS_NO_AC_DIR;
        return 1;
    }

    ModuleDeleteDir(CO_UPDATE_DST_FILE);
    ModuleDeleteDir(CO_UPDATE_END_FILE);

    sprintf(cmd, "cp -r %s %s", pkgPath, CO_UPDATE_DST_FILE);
    system(cmd);
    system("sync");
    g_CoUpdateStatus = CO_UPDATE_STATUS_INSTALL;
    g_CoWaitTimer = 0;
    return 0;
}

unsigned int COModuleGetPkgPath(char *pkgPath)
{
    DIR           *dir;
    struct dirent *ent;

    if ((dir = opendir(CO_UPDATE_SRC_FOLDER)) == NULL)
    {
        g_CoUpdateStatus = CO_UPDATE_STATUS_NO_UP_DIR;
        return 1;
    }
    
    while ((ent = readdir(dir)) != NULL)
    {
        if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0))
        {
            continue;
        }
        else if (ent->d_type == 8)    /* 8:�ļ� */
        {
            sprintf(pkgPath, "%s/%s", CO_UPDATE_SRC_FOLDER, ent->d_name);
            closedir(dir);
            return 0;
        }
    }

    g_CoUpdateStatus = CO_UPDATE_STATUS_NO_BIN;
    closedir(dir);
    return 1;
}

void InitCoModuleUpdateDraw(DWORD dwPluginId)
{
    g_CoUpdateIndex = 1;
    g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
    g_CoWaitTimer = 0;
}

void COModuleUpdateDraw(HDC hdc)
{
    int nTop = 0;
    int nLeft = 0;
    char buf[200] = {0};
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (g_CoUpdateStatus == CO_UPDATE_STATUS_COPY)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�����ļ���...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��ʱԼ1����");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��Ҫ�ε�U��");

        return;
    }

    if (g_CoUpdateStatus == CO_UPDATE_STATUS_INSTALL)
    {
        nTop = 40;
        nLeft = 16;
        sprintf(buf, "���ڰ�װ...(%u��)", g_CoWaitTimer);
        TextOut(hdc, nLeft, nTop, buf);
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��ʱԼ1����");      
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "���԰ε�U��");

        return;
    }

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 36;
    rc.bottom = rc.top + 16;
    const char *pcz = "Э����������";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    nTop = 98;
    nLeft = 26;

    if (g_CoUpdateIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "����");
    }

    nLeft = 110;
    if (g_CoUpdateIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }

    return;
}

void COModuleUpdateTimer(int nId)
{   
    char buf[200] = {0};
    char ver[128] = {0};
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    /* 1s��ʱ����� */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* ��Э����������ҳ�棬���������� */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* ��Э������ҳ�棬��Ļ���� */

    switch (g_CoUpdateStatus)
    {
        case CO_UPDATE_STATUS_INIT: //δ��ʼ��������ʼҳ��
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case CO_UPDATE_STATUS_COPY: //�����ļ�����
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case CO_UPDATE_STATUS_NO_AC_DIR: //Ŀ¼������
        {
            MessageBox("rspSampleĿ¼�����ڣ�");
            g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
            break;
        }
        case CO_UPDATE_STATUS_NO_UP_DIR: //δ�ҵ�U����İ�װĿ¼
        {
            sprintf(buf, "δ�ҵ�U���������Ŀ¼\n%s\n����U���Ƿ����", CO_UPDATE_SRC_FOLDER);
            MessageBox(buf);
            g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
            break;
        }
        case CO_UPDATE_STATUS_NO_BIN: //ȱ�������ļ�
        {
            MessageBox("ȱ�������ļ���");
            g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
            break;
        }
        case CO_UPDATE_STATUS_INSTALL: //���ڰ�װ
        {
            if ((0 == access(CO_UPDATE_END_FILE, F_OK)) && (0 != access(CO_UPDATE_START_FILE, F_OK)))
            {
                sleep(1);             //��ʱ�ȴ�һ���
                if (get_co_ver(ver) == 0)
                {
                    sprintf(buf, "��װ��ɣ�\n�鿴�������\n%s", ver);
                    MessageBox(buf);
                }
                else
                {
                    MessageBox("��װ��ɣ�\n�������ز˵���\n�鿴�������");
                }
                g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
                break;
            }

            if (g_CoWaitTimer <= 1000)
            {
                g_CoWaitTimer++;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            }
            else
            {
                MessageBox("��װ��ʱ��\n����update.log��\n�鿴�������");
                g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
                g_CoWaitTimer = 0;
            }
            break;
        }
        default: 
        {
            g_CoUpdateStatus = CO_UPDATE_STATUS_INIT;
            break;
        }
    }

}

/* ����ɵ����߱�֤�Ϸ��� */
unsigned int COModuleUpdateProc(GUI_COMM *pg_guicomm)
{
    char pkgPath[PATH_MAX_LEN + 1] = {0};
    char ver[128] = {0};
    char msg[128] = {0};

    if (ValidatePwd() == TRUE)
    {
        if (get_co_ver(ver) == 0)
        {
            sprintf(msg, "����\n�����������벻Ҫ�ϵ�\n��ǰ%s\n�Ƿ�����", ver);
            if (FALSE == MessageBox(msg, MB_OKCANCEL))
            {
                return 0;
            }
        }
        else
        {
            if (FALSE == MessageBox("����\n�����������벻Ҫ�ϵ�\n�Ƿ�����", MB_OKCANCEL))
            {
                return 0;
            }
        }
    }
    else
    {
        return 1;
    }

    g_CoUpdateStatus = CO_UPDATE_STATUS_COPY;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);

    if (COModuleGetPkgPath(pkgPath) != 0)
    {
        return 1;
    }

    if (COModuleCopyPkg(pkgPath) != 0)
    {
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    return 0;
}

BOOL COModuleUpdateKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    /* δ��ʼ����״̬�²��ܲ����˵� */
    if (g_CoUpdateStatus != CO_UPDATE_STATUS_INIT)
    {
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            if (g_CoUpdateIndex > 0)
            {
                g_CoUpdateIndex--;
            }
            else
            {
                g_CoUpdateIndex = 1;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            if (g_CoUpdateIndex < 1)
            {
                g_CoUpdateIndex++;
            }
            else
            {
                g_CoUpdateIndex = 0;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_CoUpdateIndex)
            {
                case 0: /* ���� */
                {
                    CancelMenu();
                    return TRUE;
                }
                case 1: 
                {
                    COModuleUpdateProc(pg_guicomm);
                    return TRUE;
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}

//Э����������
PLUGIN_INFO g_CoUpdate = {
    0x00A000FE,             /* id */
    InitCoModuleUpdateDraw, /* InitLayer */
    NULL,                   /* UninitLayer */
    COModuleUpdateDraw,     /* Draw */
    COModuleUpdateTimer,    /* Timer */
    COModuleUpdateKey,      /* keymsg */
    NULL                    /* ipc */
};
#endif

static int g_UpdateIndex = 0;     //ѡ����: 0-��װ����; 1-��װר��; 2-����
static unsigned int g_InstallType = 0;      //������Ʒ: 0-��; 1-����;2-ר��
static unsigned int g_UpdateStatus = 0;    //״̬    : 0-δ��ʼ����;1-������װ�ļ�;2-����Ŀ¼ʧ��;3-���ڰ�װ;
                                       //          4-������װ�߳�ʧ��;5-δ�ҵ�U����İ�װĿ¼;6-ȱ�ٰ�װ�ű�


#define UPDATE_STATUS_INIT          0
#define UPDATE_STATUS_COPY          1
#define UPDATE_STATUS_MKDIR_ERR      2
#define UPDATE_STATUS_INSTALL       3
#define UPDATE_STATUS_CREATE_ERR    4
#define UPDATE_STATUS_NO_PACKAGE    5
#define UPDATE_STATUS_NO_SCRIPT     6

unsigned int UdiskCopyPkg()
{
    if (0 == access("/tmp/update", F_OK))
    {
        system("rm -rf /tmp/update");
    }

    if (0 != access("/tmp/update", F_OK))
    {
        if(0 != mkdir("/tmp/update", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            g_UpdateStatus = UPDATE_STATUS_MKDIR_ERR;
            return 1;
        }
    }

    if (0 != access("/tmp/udisk/package", F_OK))
    {
        g_UpdateStatus = UPDATE_STATUS_NO_PACKAGE;
        return 1;
    }

    system("cp -r /tmp/udisk/package/* /tmp/update/");
    return 0;
}

unsigned int UdiskCheckPkg()
{
    uint8 product = 0; /* ��Ʒ����: bit0-����; bit1-ר�� */

    if (0 == access("/tmp/update/setup.sh", F_OK))
    {
        product |= 0x01;
        sleep(2);  //��ʱ�ȴ�һ���
    }

    g_UpdateStatus = (0 == product ? UPDATE_STATUS_NO_SCRIPT : UPDATE_STATUS_INSTALL);

    return (0 == product ? 1 : 0);
}

#define INSTALL_TYPE_GB 1
#define INSTALL_TYPE_ZB 2
#define INSTALL_TYPE_QT 3    //Ԥ��

unsigned int UdiskInstallPkg()
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return 1;
    }

    if (g_UpdateStatus != UPDATE_STATUS_INSTALL)
    {
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);

    if (g_InstallType == INSTALL_TYPE_GB)
    {
        system("chmod 755 -R /tmp/update");
        chmod("/tmp/update/setup.sh", 0777);
        system("/tmp/update/setup.sh > /data/app/desktopGui/log/udisk.log -d");
    }
    else
    {
        return 1;
    }

    return 0;
}

void *UdiskInstallThread(void *arg)
{
    if (UdiskCopyPkg() != 0)
    {
        return NULL;
    }

    if (UdiskCheckPkg() != 0)
    {
        return NULL;
    }

    if (UdiskInstallPkg() != 0)
    {
        return NULL;
    }

    return NULL;
}

void InitUdiskUpdateDraw(DWORD dwPluginId)
{
    g_UpdateIndex = 0;
    g_InstallType = 0;
    g_UpdateStatus = UPDATE_STATUS_INIT;
}

void UdiskUpdateDraw(HDC hdc)
{
    int nTop = 0;
    int nLeft = 0;
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return;
    }

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (g_UpdateStatus == UPDATE_STATUS_COPY)
    {
        nTop = 40;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�����ļ���...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��ʱԼ30s");
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��Ҫ�ε�U��");

        return;
    }

    if (g_UpdateStatus == UPDATE_STATUS_INSTALL)
    {
        nTop = 40;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "���ڰ�װ...");
        nTop = 60;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "��ʱԼ2����");
        nTop = 80;
        nLeft = 24;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        nLeft = 32;
        TextOut(hdc, nLeft, nTop, "���԰ε�U��");

        return;
    }

    nLeft = 40;
    nTop = 40;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (0 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "��װ����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "��װ����");
    }

    nTop += 30;

    if (1 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ִ�нű�");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ִ�нű�");
    }

    nTop += 30;

    if (2 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "����");
    }

    PopListDraw(hdc);
}

void UdiskUpdateTimer(int nId)
{   
    GUI_COMM *pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
    {
        return;
    }
    
    /* 1s��ʱ����� */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* ��U������ҳ�棬���������� */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
    lighton_lcd(); /* ��U������ҳ�棬��Ļ���� */

    switch (g_UpdateStatus)
    {
        case UPDATE_STATUS_INIT: //δ��ʼ��������ʼҳ��
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case UPDATE_STATUS_COPY: //�����ļ�����
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case UPDATE_STATUS_MKDIR_ERR: //����Ŀ¼ʧ��
        {
            MessageBox("����Ŀ¼ʧ�ܣ�");
            g_UpdateStatus = UPDATE_STATUS_INIT;
            break;
        }
        case UPDATE_STATUS_INSTALL: //���ڰ�װ
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            break;
        }
        case UPDATE_STATUS_CREATE_ERR: //������װ�߳�ʧ��
        {
            MessageBox("������װ�߳�ʧ�ܣ�");
            g_UpdateStatus = UPDATE_STATUS_INIT;
            break;
        }
        case UPDATE_STATUS_NO_PACKAGE: //δ�ҵ�U����İ�װĿ¼
        {
            MessageBox("δ�ҵ�U����İ�װĿ¼\n����U���Ƿ����");
            g_UpdateStatus = UPDATE_STATUS_INIT;
            break;
        }
        case UPDATE_STATUS_NO_SCRIPT: //ȱ�ٰ�װ�ű�
        {
            MessageBox("ȱ�ٰ�װ�ű���");
            g_UpdateStatus = UPDATE_STATUS_INIT;
            break;
        }
        default: 
        {
            g_UpdateStatus = UPDATE_STATUS_INIT;
            break;
        }
    }

}

/* ����ɵ����߱�֤�Ϸ��� */
unsigned int UdiskInstallProc(GUI_COMM *pg_guicomm, unsigned int installType)
{
    int errCode = 0;
    pthread_t threadId;
    const char *message = "����\n��װ�������벻Ҫ�ϵ�\n�Ƿ�װ";
#ifdef PRODUCT_CCU
    char productName[24] = {"������"};
#else
    char productName[24] = {"��Դ������"};
#endif
    char buf[200] = {0};

    if (ValidatePwd() == TRUE)
    {
        sprintf(buf, "%s%s", message, productName);
        if (FALSE == MessageBox(buf, MB_OKCANCEL))
        {
            return 0;
        }
    }
    else
    {
        return 1;
    }

    g_UpdateStatus = UPDATE_STATUS_COPY;
    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    g_InstallType = installType;
    errCode = pthread_create(&threadId, NULL, UdiskInstallThread, NULL);

    if (errCode != 0)
    {
        threadId = 0;
        g_UpdateStatus = UPDATE_STATUS_CREATE_ERR;
        return 1;
    }

    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    return 0;
}

BOOL UdiskUpdateKey(int nKey)
{
    GUI_COMM *pg_guicomm = &g_guiComm;

    if (pg_guicomm == NULL)
    {
        return FALSE;
    }

    /* δ��ʼ����״̬�²��ܲ����˵� */
    if (g_UpdateStatus != UPDATE_STATUS_INIT)
    {
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            g_UpdateIndex--;

            if (g_UpdateIndex < 0)
            {
                g_UpdateIndex = 2;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            g_UpdateIndex++;

            if (g_UpdateIndex > 2)
            {
                g_UpdateIndex = 0;
            }

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_UpdateIndex)
            {
                case 0: /* ��װ���� */
                {
                    UdiskInstallProc(pg_guicomm, INSTALL_TYPE_GB);
                    return TRUE;
                }
                case 1: /* ��װ���� */
                {
                    UdiskInstallProc(pg_guicomm, INSTALL_TYPE_QT);
                    return TRUE;
                }
                case 2: /* �˳� */
                {
                    CancelMenu();
                    return TRUE;
                }
            }

            return TRUE;
        }
    }

    return FALSE;
}
uint8 logexportindex;
int logflag;
time_t logstarttime;
pthread_t copythreadid;
void LogExportInit(DWORD dwPluginId)
{
	logexportindex = 0;
    logflag = -1;
    g_bpop = TRUE;
}
void LogExportUnInit()
{
	logexportindex = 0;
    g_bpop = FALSE;
}

void LogExportDraw(HDC hdc)
{
	 GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return;
	int index = 0;
	char czbuf[300]={0};
	int nleft = 40;
	int ntop = 30;
	SelectFont(hdc,pg_guicomm->fontBig);
	sprintf(czbuf,"����������־");
	if(index == logexportindex)
	{
        ITEM_SELECT;
        TextOut(hdc, nleft, ntop, czbuf);
        ITEM_NOT_SELECT;
	}else
	{
        TextOut(hdc, nleft, ntop, czbuf);
	}

	index++;
	ntop+=28;
	sprintf(czbuf,"�������г���");
	if(index == logexportindex)
	{
        ITEM_SELECT;
        TextOut(hdc, nleft, ntop, czbuf);
        ITEM_NOT_SELECT;
	}else
	{
        TextOut(hdc, nleft, ntop, czbuf);
	}

	index++;
	ntop+=28;
	sprintf(czbuf,"���������ļ�");
	if(index == logexportindex)
	{
        ITEM_SELECT;
        TextOut(hdc, nleft, ntop, czbuf);
        ITEM_NOT_SELECT;
	}else
	{
        TextOut(hdc, nleft, ntop, czbuf);
	}

    index++;
	ntop+=28;
	sprintf(czbuf,"�������ݿ��ļ�");
	if(index == logexportindex)
	{
        ITEM_SELECT;
        TextOut(hdc, nleft, ntop, czbuf);
        ITEM_NOT_SELECT;
	}else
	{
        TextOut(hdc, nleft, ntop, czbuf);
	}
}

void LogExportTimer(int nId)
{   
    GUI_COMM *pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
    {
        return;
    }

    time_t t;
    
    /* 1s��ʱ����� */
    if (nId != TIMER_MAIN)
    {
        return;
    }

    g_guiMain.nTrunDispTime = 0; /* ��U������ҳ�棬���������� */
    g_guiMain.nLightonTime = 0;
    g_guiMain.bLighton = TRUE;
//    lighton_lcd(); /* ��U������ҳ�棬��Ļ���� */

    switch(logflag)
    {
        case 0:
        {
            time(&t);
            if(t - logstarttime>90000)
            {
                GUI_FMT_TRACE("copy timeout!!\n");
                pthread_cancel(copythreadid);
                pthread_join(copythreadid, NULL);
                ClosePopLayer();
                system("ps -ef|grep /tmp/udisk |grep -v grep|awk '{print $2}' |xargs kill");
                system("sync");
                MessageBox("����ʱ�䳬��15����\n�Զ�������");
                CancelMenu();
            }
        }
        break;
        case 1:
        {
            pthread_join(copythreadid, NULL);
            GUI_FMT_TRACE("�������\n");
            ClosePopLayer();
            MessageBox("������ɣ�");
            CancelMenu();
        }
        break;
    }
}

//����Ƿ����U��
BOOL CanUseUDisks()
{
    char cmd[128] = {0};
    char pstatus[128] = {0};

    sprintf(cmd, "ls /sys/class/scsi_device");
    FILE* pf = popen(cmd, "r");  
      
    if(pf == NULL)
    {
        return FALSE;// U��û�в���
    }
    
    if(fgets(pstatus, 128, pf) == NULL)
    {
        pclose(pf);
        return FALSE;// U��û�в���
    }
    pclose(pf);

    if(pstatus[strlen(pstatus)-1] == '\n')
        pstatus[strlen(pstatus)-1] = '\0';

    memset(cmd,0 , 128);
    sprintf(cmd, "ls /sys/class/scsi_device/%s/device/block", pstatus);
    pf = popen(cmd, "r");  
      
    if(pf == NULL)
    {
        return FALSE;// 
    }

    memset(pstatus,0 , 128);
    if(fgets(pstatus, 128, pf) == NULL)
    {
        pclose(pf);
        return FALSE;// 
    }
    pclose(pf);

    if(pstatus[strlen(pstatus)-1] == '\n')
        pstatus[strlen(pstatus)-1] = '\0';

    memset(cmd,0 , 128);
    sprintf(cmd, "cat /proc/mounts | grep %s", pstatus);
    pf = popen(cmd, "r");  
      
    if(pf == NULL)
    {
        return FALSE;// 
    }

    memset(pstatus,0 , 128);
    if(fgets(pstatus, 128, pf) == NULL)
    {
        pclose(pf);
        return FALSE;// U��û�й���
    }
    pclose(pf);
        
    return TRUE;
}

void *CopyThreads(void *arg)
{
    char buf[300]={0};
    char cmd[1000]={0};

    //�����ļ���������ip+ʱ��
    char pstatus[128] = "127.0.0.1";
    time_t t;
    time(&t);
    logstarttime = t;
    tm *p = localtime(&t);
    tm ptime;
    memcpy(&ptime,p,sizeof(tm));
    sprintf(cmd, "ifconfig FE0 |awk  'NR==2 {print $2}' |awk -F  \":\"  '{print $2}'");
    FILE* pf = popen(cmd, "r");  
    if(pf != NULL)
    {
        fgets(pstatus, 128, pf);
    }
    pclose(pf);
    if(pstatus[strlen(pstatus)-1] == '\n')
        pstatus[strlen(pstatus)-1] = '\0';

    sprintf(buf,"          ���ڱ���\n\n        ����ʱ��ϳ�\n\n  ��������ն˻��߰γ�U��");
//    g_bpop = TRUE;
//    PopMsg(buf);

    system("cp -rf /tmp/data/app /data/app");
    system("sync");


    switch(logexportindex)
    {
    case 0:         //������־�ļ�
    {
        sprintf(cmd,"mkdir -p /tmp/udisk/log_%s_%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min);
        system(cmd);
        system("tar -zcvf /tmp/datalog.tar.gz --exclude=/data/app/dbCenter/data --exclude=/data/app/dbCenter/bad /data/app/");
        system("tar -zcvf /tmp/varlog.tar.gz /var/log");
        sprintf(cmd,"mv /tmp/*log.tar.gz /tmp/udisk/log_%s_%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min);
    }
        break;
    case 1:         //����Ӧ�ó���
    {
        sprintf(cmd,"mkdir /tmp/udisk/host; mkdir /tmp/udisk/baseContainer;mkdir /tmp/udisk/pdContainer;mkdir /tmp/udisk/amrContainer; mkdir /tmp/udisk/edgerContainer;");
        system(cmd);
        sprintf(cmd,"cp -rfP /usr/local/extapps/* /tmp/udisk/host/;docker cp baseContainer:/usr/local/extapps/dbCenter /tmp/udisk/baseContainer/; "
                                "docker cp baseContainer:/usr/local/extapps/ccoRouter /tmp/udisk/baseContainer/;docker cp edgerContainer:/usr/local/extapps/taskManager /tmp/udisk/edgerContainer/; "
                                "docker cp amrContainer:/usr/local/extapps/puAmr /tmp/udisk/amrContainer/;docker cp pdContainer:/usr/local/extapps/pdAmr /tmp/udisk/pdContainer/;");
        
    }
        break;
    case 2:         //���������ļ�
    {
        sprintf(cmd,"mkdir -p /tmp/udisk/data/app; mkdir -p /tmp/udisk/var/log; mkdir /tmp/udisk/host; mkdir /tmp/udisk/baseContainer;mkdir /tmp/udisk/pdContainer;mkdir /tmp/udisk/amrContainer; mkdir /tmp/udisk/edgerContainer;");
        system(cmd);
        sprintf(cmd,"cp -rfP /usr/local/extapps/* /tmp/udisk/host/;docker cp baseContainer:/usr/local/extapps/dbCenter /tmp/udisk/baseContainer/; "
                                "docker cp baseContainer:/usr/local/extapps/ccoRouter /tmp/udisk/baseContainer/;docker cp edgerContainer:/usr/local/extapps/taskManager /tmp/udisk/edgerContainer/; "
                                "docker cp amrContainer:/usr/local/extapps/puAmr /tmp/udisk/amrContainer/;docker cp pdContainer:/usr/local/extapps/pdAmr /tmp/udisk/pdContainer/;"
                                "cp  -rfP /data/app/* /tmp/udisk/data/app/; cp -rfP /var/log/* /tmp/udisk/var/log;");
       
    }
        break;
    case 3:         //�������ݿ��ļ�
    {
        sprintf(cmd,"mkdir -p /tmp/udisk/dbCenterData_%s_%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min);
        system(cmd);
        system("tar -zcvf /tmp/dbCenterData.tar.gz /data/app/dbCenter/data");
        sprintf(cmd,"mv /tmp/dbCenterData.tar.gz /tmp/udisk/dbCenterData_%s_%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min);
    }
        break;
    }
    
    system(cmd);
    system("sync");

//    g_bpop = FALSE;
    GUI_FMT_TRACE("copythread ������� �˳�\n");
    
    *(int *)arg = 1;
    
    pthread_exit(NULL);
}
BOOL LogExportKeyMsg(int nKey)
{
	GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return FALSE;
	int maxindex = 3;
	char buf[300]={0};
	int err = 0;
	switch(nKey)
	{
    case KEY_UP:
    case KEY_LEFT:
    {
        if(logexportindex>0)
            logexportindex--;
        else
            logexportindex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if(logexportindex<maxindex)
            logexportindex++;
        else
            logexportindex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if(ValidatePwd() == TRUE)
        {
//            g_bpop = FALSE;
            switch(logexportindex)
            {
                case 0:
                    sprintf(buf,"����������־�ļ���U��\n�Ƿ�ȷ�ϣ�");
                break;
                case 1:
                    sprintf(buf,"��������Ӧ�ó���U��\n�Ƿ�ȷ�ϣ�");
                break;
                case 2:
                    sprintf(buf,"���������ļ���U��\n�Ƿ�ȷ�ϣ�");
                break;
                case 3:
                    sprintf(buf,"�����������ݿ��ļ���U��\n�Ƿ�ȷ�ϣ�");
                break;
                default:
                    return FALSE;
            }
            if(TRUE == MessageBox(buf, MB_OKCANCEL))
            {
                if(FALSE == CanUseUDisks())
                {
                    MessageBox("U��û�в���\n\n���ܱ����ļ�!");
                    return TRUE;
                }
                /*����һ���߳�,���߳�ִ�п����Ȳ���,���߳�ι�ػ�����*/
                logflag = 0;
                err = pthread_create(&copythreadid,NULL, CopyThreads, &logflag);
                if (err != 0)
                {
                    logflag = -1;
                    MessageBox("ϵͳæ\n�Ժ��ٿ���!");
                    break;
                }
                PopMsg("          ���ڱ���\n\n        ����ʱ��ϳ�\n\n  ��������ն˻��߰γ�U��");
                return FALSE;
            }
        }
    }
	}
	return FALSE;
}


//��־����
PLUGIN_INFO g_smiOSLogExport = {
	0x00A000F1/*id*/, 
	LogExportInit/*InitLayer*/,
	LogExportUnInit/*UninitLayer*/,
	LogExportDraw/*Draw*/,
	LogExportTimer/*Timer*/,
	LogExportKeyMsg/*keymsg*/,
	NULL/*ipc*/
};

//U������
PLUGIN_INFO g_smiOSUUpdate = {
	0x00A000F0/*id*/, 
	InitUdiskUpdateDraw/*InitLayer*/,
	NULL/*UninitLayer*/,
	UdiskUpdateDraw/*Draw*/,
	UdiskUpdateTimer/*Timer*/,
	UdiskUpdateKey/*keymsg*/,
	NULL/*ipc*/
};


#if DESC("���߲��ŷ�ʽ����", 1)
uint8 g_Wireless_status = 0;
uint8 g_Wireless_statusOld = 0;
uint8 g_WirelessIndex = 0;

//��Ӫ��
const char* g_WirelessStatusName[] =
{
    "USB",
    "����"
};

void wirelessStatus_set_init(HDC hdc)
{
    if(0 == access("/data/app/wirelessDCM/data/bForceUart", F_OK))
    {
        g_Wireless_status = 1;
    }
    else
    {
        g_Wireless_status = 0;
    }
    g_WirelessIndex = 0;
    g_Wireless_statusOld = g_Wireless_status;
}

void wirelessStatus_set_draw(HDC hdc)
{
    int nTop = 24;
    int nLeft1 = 16;
    int nLeft2 = 60;
    int nLeft3 = 88;

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    TextOut(hdc, 24, nTop, "���߲��ŷ�ʽ");
    nTop += 20;
    TextOut(hdc, 48, nTop, "��������");

    SelectFont(hdc, pg_guicomm->fontSmall);
    nTop += 32;
    TextOut(hdc, nLeft1, nTop, "����: ");
    if(0 == g_WirelessIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_WirelessStatusName[g_Wireless_status]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_WirelessStatusName[g_Wireless_status]);
    }

    SelectFont(hdc, pg_guicomm->fontBig);
    nTop += 32;
    if(1 == g_WirelessIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, 30, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 30, nTop, "����");
    }

    if(2 == g_WirelessIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "ȷ��");
    }
    
    PopListDraw(hdc);
}

BOOL wirelessStatus_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
        
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            g_Wireless_status = nSel;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        return TRUE;
    }
    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_WirelessIndex > 0)
            {
                g_WirelessIndex--;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_WirelessIndex < 2)
            {
                g_WirelessIndex++;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if(1 == g_WirelessIndex)
            {
                CancelMenu();
                return TRUE;
            }
            else if(TRUE == ValidatePwd())
            {
                if(0 == g_WirelessIndex)
                {
                    PopList(g_WirelessStatusName, 2, g_WirelessStatusName[g_Wireless_status], 80, 80, FALSE);
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                    return TRUE;
                }
                else if(2 == g_WirelessIndex)
                {
                    if(g_Wireless_status != g_Wireless_statusOld)
                    {
                        if(0 == g_Wireless_status)
                        {
                            system("rm /data/app/wirelessDCM/data/bForceUart");
                        }
                        else
                        {
                            system("touch /data/app/wirelessDCM/data/bForceUart");
                        }
                        MessageBox("���óɹ�\n�β�ģ��������ն���Ч��");
                    }
                    CancelMenu();
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

PLUGIN_INFO g_layer_wirelessStatus_set = {
    0x00A000FC/*id*/,
    wirelessStatus_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    wirelessStatus_set_draw/*Draw*/,
    NULL/*Timer*/,
    wirelessStatus_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

uint8 bluetooth_status;
int bluetooth_index;
const char *g_bluetooth_list[] = {
	"�ر�",
	"����",
};
void bluetooth_Init(DWORD dwPluginId)
{
//    get_bluetooth_state(&bluetooth_status);
    bluetooth_index = 0;
    g_bpop = TRUE;
}
void bluetooth_Uninit()
{
    g_bpop = FALSE;
}

void bluetooth_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return;

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top+16;
    char pcz[20];
	strcpy(pcz, "������������");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if(bluetooth_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "����");
    }

    nLeft = 110;
    if(bluetooth_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "�ر�");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "�ر�");
    }
}

//choice 1 ����ssh 0 �ر�ssh
void bluetooth_set(uint8 choice)
{
    char cmd[100]={0};
    int ret = 0;
    ret = set_bluetooth_state(choice);
    
    if(ret == 0)
    {
        sprintf(cmd,"%s�������ܳɹ�!", g_bluetooth_list[choice]);
        if(choice == 1)
        {
            bluetooth_status = BLUETOOH_ON;
        }
        else
        {
            bluetooth_status = BLUETOOH_OFF;
        }
        write_pdata_xram(GUI_PARA, (char *)&bluetooth_status, Bluetooth_Status, Bluetooth_Status_Len);
    }
    else 
    {
        sprintf(cmd,"%s��������ʧ��!", g_bluetooth_list[choice]);
    }

    MessageBox(cmd);
}
BOOL bluetooth_KeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return FALSE;

    switch(nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
        if(bluetooth_index>0)
            bluetooth_index --;
        else
            bluetooth_index = 1;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
    case KEY_DOWN:
        if(bluetooth_index<1)
            bluetooth_index++;
        else
            bluetooth_index = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
		if(ValidatePwd() == TRUE)
        {
	        switch(bluetooth_index)
	        {
	            case 0:
	                if(FALSE == MessageBox("��ʾ\n�Ƿ�����������\n", MB_OKCANCEL))
	                {
	                    return TRUE;
	                }
	                bluetooth_set(1);
	                CancelMenu();
	                return TRUE;
	            case 1:

	                if(FALSE == MessageBox("��ʾ\n�Ƿ�ر���������\n", MB_OKCANCEL))
	                {
	                    return TRUE;
	                }
	                bluetooth_set(0);
	                CancelMenu();
	                return TRUE;
	        }
//        g_bpop = FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

PLUGIN_INFO g_layer_bluetooth=
{
    0x00A000F7, // 0x00A000F7
    bluetooth_Init,
    bluetooth_Uninit,
    bluetooth_Draw,
    NULL,
    bluetooth_KeyMsg,
    NULL
};

void CPU_temperature_Init(DWORD dwPluginId)
{

}

int get_cpu_cur_temper(int *temp)
{
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        return -1;
    }
    else
    {
        *temp = dev->devinfo_get_cpu_temperature(dev);
        hal_device_release((HW_DEVICE *)dev);
    }
    return 0;
}

void CPU_temperature_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return;

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top+16;
    char pcz[40];
    int temp = 0;
	strcpy(pcz, "CPU��ǰ�¶�");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);
    
    int nTop = 80;
    int nLeft = 26;
    
    TextOut(hdc, nLeft, nTop, "��ǰ�¶ȣ�");

    nLeft = 90;
    if(get_cpu_cur_temper(&temp) == 0)
    {
        float tempC = temp;
        sprintf(pcz, "%.1f��C", tempC/10.0);
        TextOut(hdc, nLeft, nTop, pcz);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----��C");
    }
}


PLUGIN_INFO g_layer_CPU_temperature=
{
    0x00A000F8, // 0x00A000F8
    CPU_temperature_Init,
    NULL,
    CPU_temperature_Draw,
    NULL,
    NULL,
    NULL
};

//FTP TELNET ���ܿ����ر�����

uint8 FTP_Telnet_status = 0;		//ftp telnetʹ�ܣ� 0�����رգ�1����������Ĭ�Ϲر�
const char *g_FTP_TelnetModel_list[] = {
	"�ر�",
	"����",
};
uint8 key_index = 0;

void setFTP_TelnetONOFFInit(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;

	//if ((ReadXPara(TMN_PARA_EXTEND,FTP_TELNET_EN, (char *)&FTP_Telnet_status, 0) < 0) || (FTP_Telnet_status != 1))
	{
		FTP_Telnet_status = 0;
	}

	key_index = 0;
}

void setFTP_TelnetONOFFDraw(HDC hdc)
{
    //int       nTop = 0;
    char      czbuf[32];
    RECT      rc;
    //uint8     i = 0;

	SelectFont(hdc,g_guiComm.fontBig);

    //������ʾ
    ITEM_NOT_SELECT;
    rc.left  = 0;
    rc.right = g_guiComm.nWidth;
    rc.top   = 40;
    rc.bottom = rc.top+16;
    sprintf(czbuf, "ftp/Telnetʹ������:");
    DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

    rc.left  = 30;
    rc.right = g_guiComm.nWidth;
    rc.top   = 70;
    rc.bottom = rc.top+16;

     if(key_index == 0)
     {
         ITEM_SELECT;
         TextOut(hdc, rc.left, rc.top, g_FTP_TelnetModel_list[FTP_Telnet_status]);
         ITEM_NOT_SELECT;
     }
     else
     {
         TextOut(hdc, rc.left, rc.top, g_FTP_TelnetModel_list[FTP_Telnet_status]);
     }

     if(key_index == 1)
     {
         ITEM_SELECT;
         TextOut(hdc, 120, 110, "ȷ��");
         ITEM_NOT_SELECT;
     }
     else
     {
         TextOut(hdc, 120, 110, "ȷ��");
     }

     PopListDraw(hdc);
}

 BOOL setFTP_TelnetONOFFMsg(int nKey)
 {
 	 //uint8     pRD;
 	 //uint8     pRD1 = 0x55;
     // ����Listѡ����
     if(g_guiComm.bPopList == TRUE)
     {
         int nSel;
         PopListKeyMsg(nKey, &nSel);
         if(nKey == KEY_OK)
         {
             FTP_Telnet_status = nSel;
             UpdateWindow(g_guiComm.hMainWnd, TRUE);
         }
         return TRUE;
     }

     switch(nKey)
     {
     case KEY_UP:
         key_index--;
         if(key_index < 0)
             key_index = 1;
         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     case KEY_DOWN:
         key_index ++;
         if(key_index > 1)
             key_index = 0;
         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     case KEY_OK:
         if(ValidatePwd() == TRUE)
         {// ȷ�ϴ���
             switch(key_index)
             {
	            case 0:
					PopList(g_FTP_TelnetModel_list, 2, g_FTP_TelnetModel_list[FTP_Telnet_status], 30, 80, FALSE);
					break;
    	        case 1:
					//WriteXPara(TMN_PARA_EXTEND,FTP_TELNET_EN, (char *)&FTP_Telnet_status, 0);
					UpdateWindow(g_guiComm.hMainWnd, TRUE);
					MessageBox("���óɹ�");
		            if(FTP_Telnet_status ==1 )//����
		            {
		                system("inetd &");
		            }
		            else if(FTP_Telnet_status==0)//�ر�
		            {
		                system("killall inetd");
		            }
					break;
    	        default:
	                break;
             }
             return TRUE;
         }
     }

	return TRUE;
}

PLUGIN_INFO g_layersetFTP_TelnetONOFF=
{
	0x000c0004, // 0x000c0004
	setFTP_TelnetONOFFInit,
	NULL,
	setFTP_TelnetONOFFDraw,
	NULL,
	setFTP_TelnetONOFFMsg,
	NULL
};


#if DESC("��Դ����״̬",1)

void power_status_draw(HDC hdc)
{
    //int       nTop = 0;
    char      czbuf[32];
    RECT      rc;
    uint8     flag = 0;
    int32     state = 0;

    POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
	if(NULL == dev)
	{
        GUI_FMT_DEBUG("dev is null!\n");
        flag = 1;
	}
	
	if(dev->power_power_state_get(dev, POWER_MAIN, &state) < 0)
	{
		GUI_FMT_DEBUG("get power state failed!\n");
        flag = 1;
	}

    
	hal_device_release((HW_DEVICE *)dev);

	SelectFont(hdc,g_guiComm.fontBig);

    //������ʾ
    ITEM_NOT_SELECT;
    rc.left  = 0;
    rc.right = g_guiComm.nWidth;
    rc.top   = 40;
    rc.bottom = rc.top+16;
    sprintf(czbuf, "��Դ����״̬");
    DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);

    rc.left  = 30;
    rc.right = g_guiComm.nWidth;
    rc.top   = 70;
    rc.bottom = rc.top+16;

     if(flag == 1)
     {
         sprintf(czbuf, "��ȡ��Դ״̬ʧ�ܣ�");
     }
     else
     {
         if(state == 1)
         {
             sprintf(czbuf, "��������");
         }
         else
         {
             sprintf(czbuf, "����");
         }
     }

     DrawText(hdc, czbuf, strlen(czbuf), &rc, DT_VCENTER);
}

PLUGIN_INFO g_layer_power_status=
{
	0x00A000F9, // 0x00A000F9
	NULL,
	NULL,
	power_status_draw,
	NULL,
	NULL,
	NULL
};
#endif

#if DESC("���ɰ汾��ʾ",1)
void ac_ver_Init(DWORD dwPluginId)
{

}

int get_ac_minver(char * temp)
{
    char buf[1024] = {0};

    if(temp == NULL)
        return -1;
    
    FILE* pipe = fopen("/tmp/data/app/acMeter/log/update.log", "r");  
    if (!pipe)  
    {
        pipe = fopen("/data/app/acMeter/log/update.log", "r");
    }
    if (!pipe)
    {
        GUI_FMT_DEBUG("��ѯ����ģ��汾ʧ��,/data/app/acMeter/log/update.log��\n");
        return -1;
    }

    fseek(pipe, -1024L, SEEK_END);
    while (fgets(buf, 1024, pipe) != NULL)
    {
        GUI_FMT_TRACE("%s", buf);
        char *pfile = NULL;
        if ((pfile = strstr(buf,"��ģ��С�汾��VERS = ")) != NULL)
        {
            memcpy(temp, pfile + strlen("��ģ��С�汾��VERS = "), 8);
        }
        memset(buf, 0x0, sizeof(buf));
    }

    if (temp[0] == '\0')
    {
        fclose(pipe);
        return -1; 
    }

    if(temp[strlen(temp)-1] == '\n')
        temp[strlen(temp)-1] = '\0';
    
    fclose(pipe);
    return 0;
}

int get_ac_data_ver(char * temp)
{
    char buf[1024] = {0};

    if(temp == NULL)
        return -1;
    
    FILE* pipe = fopen("/tmp/data/app/acMeter/log/update.log", "r");  
    if (!pipe)  
    {
        pipe = fopen("/data/app/acMeter/log/update.log", "r");
    }
    if (!pipe)
    {
        GUI_FMT_DEBUG("��ѯ����ģ��汾ʧ��,/data/app/acMeter/log/update.log��\n");
        return -1;
    }

    fseek(pipe, -1024L, SEEK_END);
    while (fgets(buf, 1024, pipe) != NULL)
    {
        GUI_FMT_TRACE("%s", buf);
        char *pfile = NULL;
        if ((pfile = strstr(buf,"��ģ��汾����DATE = ")) != NULL)
        {
            memcpy(temp, pfile + strlen("��ģ��汾����DATE = "), 6);
        }
        memset(buf, 0x0, sizeof(buf));
    }

    if (temp[0] == '\0')
    {
        fclose(pipe);
        return -1; 
    }

    if(temp[strlen(temp)-1] == '\n')
        temp[strlen(temp)-1] = '\0';
    
    fclose(pipe);
    return 0;
}

int get_ac_ver(char * temp)
{
    char buf[1024] = {0};

    if(temp == NULL)
        return -1;
    
    FILE* pipe = fopen("/tmp/data/app/acMeter/log/update.log", "r");  
    if (!pipe)  
    {
        pipe = fopen("/data/app/acMeter/log/update.log", "r");
    }
    if (!pipe)
    {
        GUI_FMT_DEBUG("��ѯ����ģ��汾ʧ��,/data/app/acMeter/log/update.log��\n");
        return -1;
    }

    fseek(pipe, -1024L, SEEK_END);
    while (fgets(buf, 1024, pipe) != NULL)
    {
        GUI_FMT_TRACE("%s", buf);
        char *pfile = NULL;
        if ((pfile = strstr(buf,"����ģ��汾��VER = ")) != NULL)
        {
            memcpy(temp, pfile + strlen("����ģ��汾��VER = "), 5);
        }
        memset(buf, 0x0, sizeof(buf));
    }

    if (temp[0] == '\0')
    {
        fclose(pipe);
        return -1; 
    }

    if(temp[strlen(temp)-1] == '\n')
        temp[strlen(temp)-1] = '\0';
    
    fclose(pipe);
    return 0;
}

int get_ac_meterIC_ver(char * temp)
{
    char buf[1024] = {0};

    if(temp == NULL)
        return -1;
    
    FILE* pipe = fopen("/tmp/data/app/acMeter/log/update.log", "r");  
    if (!pipe)  
    {
        pipe = fopen("/data/app/acMeter/log/update.log", "r");
    }
    if (!pipe)
    {
        GUI_FMT_DEBUG("��ѯ���ɼ����汾ʧ��,/data/app/acMeter/log/update.log��\n");
        return -1;
    }

    fseek(pipe, -1024L, SEEK_END);
    while (fgets(buf, 1024, pipe) != NULL)
    {
        GUI_FMT_TRACE("%s", buf);
        char *pfile = NULL;
        if ((pfile = strstr(buf,"���ɼ����汾��VER = ")) != NULL)
        {
            memcpy(temp, pfile + strlen("���ɼ����汾��VER = "), 2);
        }
        memset(buf, 0x0, sizeof(buf));
    }

    if (temp[0] == '\0')
    {
        fclose(pipe);
        return -1; 
    }

    if(temp[strlen(temp)-1] == '\n')
        temp[strlen(temp)-1] = '\0';
    
    fclose(pipe);
    return 0;
}

void ac_ver_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return;

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 30;
    rc.bottom = rc.top+16;
    char pcz[40];
	strcpy(pcz, "����ģ�鵱ǰ�汾");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);
    
    int nTop = 60;
    int nLeft = 26;
    
    TextOut(hdc, nLeft, nTop, "ģ��汾��");

    nLeft = 90;
    memset(pcz, 0, 40);
    if (get_ac_ver(pcz) == 0)
    {
        TextOut(hdc, nLeft, nTop, pcz);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }

    nTop = 80;
    nLeft = 26;
    
    TextOut(hdc, nLeft, nTop, "�ڲ��汾��");

    nLeft = 90;
    memset(pcz, 0, 40);
    if (get_ac_minver(pcz) == 0)
    {
        TextOut(hdc, nLeft, nTop, pcz);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }

    nTop = 100;
    nLeft = 26;
    
    TextOut(hdc, nLeft, nTop, "�汾���ڣ�");

    nLeft = 90;
    memset(pcz, 0, 40);
    if (get_ac_data_ver(pcz) == 0)
    {
        TextOut(hdc, nLeft, nTop, pcz);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }

    #ifdef AREA_SHANDONG
    nTop = 120;
    nLeft = 26;
    
    TextOut(hdc, nLeft, nTop, "�����汾��");

    nLeft = 90;
    memset(pcz, 0, 40);
    if (get_ac_meterIC_ver(pcz) == 0)
    {
        TextOut(hdc, nLeft, nTop, pcz);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }
    #endif
}


PLUGIN_INFO g_layer_ac_ver=
{
    0x00A000FA, // 0x00A000FA
    ac_ver_Init,
    NULL,
    ac_ver_Draw,
    NULL,
    NULL,
    NULL
};

#endif

#if DESC("Э�������汾��ʾ",1)
void co_ver_Init(DWORD dwPluginId)
{

}

int get_co_ver(char * temp)
{
    char buf[1024] = {0};

    if(temp == NULL)
        return -1;
    
    FILE* pipe = fopen("/data/app/rspSample/data/version", "r");
    if (!pipe)
    {
        pipe = fopen("/data/app/rspSample/data/version", "r");
    }
    if (!pipe)
    {
        GUI_FMT_DEBUG("��ѯЭ�������汾ʧ�� ������[-2]\n");
        return -2;
    }

    if (fgets(buf, 1024, pipe) != NULL)
    {
        strcpy(temp, buf);
    }
    else
    {
        GUI_FMT_DEBUG("��ѯЭ�������汾ʧ�� ������[-3]\n");
        return -3;
    }
    
    fclose(pipe);
    return 0;
}

void co_ver_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return;

    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top+16;
    char pcz[40];
	strcpy(pcz, "Э��������ǰ�汾");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);
    
    int nTop = 80;
    int nLeft = 26;

    memset(pcz, 0, 40);
    if (get_co_ver(pcz) == 0)
    {
        TextOut(hdc, nLeft, nTop, pcz);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }

    return;
}


PLUGIN_INFO g_layer_co_ver=
{
    0x00A000FD, // 0x00A000FD
    co_ver_Init,
    NULL,
    co_ver_Draw,
    NULL,
    NULL,
    NULL
};

#endif

uint8 proxyip[4]={0};
uint16 proxyport = 0;
uint8 proxyindex;
const char* proxy_filepath = "/etc/comm_proxy/sysConfig.cfg";
const char* ipkey = "StationPrepositionIp";
const char* portkey = "StationPrepositionPort";

char * strim(char *str)
{
	char *strend,*sp,*ep;
	int len;
	sp = str;
	strend = str + strlen(str) - 1;
	ep = strend;
 
	while(sp<=strend && isspace(*sp))// *sp == ' 'Ҳ����
		sp++;
	while(ep>=sp && isspace(*ep))
		ep--;
	len = (ep < sp) ? 0:(ep-sp)+1;//(ep < sp)�ж��Ƿ����ж��ǿո�
	sp[len] = '\0';
	return sp;

}

//���ַ�����ʽ��ipת��Ϊ���� ʧ�ܷ���false
BOOL commfun_IP_unpack(char* ipin, uint8* ipout)
{
    if (ipin == NULL || strlen(ipin) == 0)
        return FALSE;
    char* p1[4];
    char czBuf[20];
    strcpy(czBuf, ipin);

    char* pTmp = p1[0] = czBuf;
    int nIndex = 1;
    while (*pTmp != '\0')
    {
        if (*pTmp == '.')
        {
            *pTmp = '\0';
            pTmp++;
            if (*pTmp == '\0')
            {
                break;
            }
            else
            {
                if (*pTmp > '9' || *pTmp < '0')
                    return FALSE;
                if (nIndex > 4)
                    return FALSE;
                p1[nIndex] = pTmp;
                nIndex++;
            }
        }
        else if (*pTmp > '9' || *pTmp < '0')
        {
            return FALSE;
        }
        pTmp++;
    }
    if (nIndex != 4)
    {
        return FALSE;
    }
    ipout[0] = atoi(p1[0]);
    ipout[1] = atoi(p1[1]);
    ipout[2] = atoi(p1[2]);
    ipout[3] = atoi(p1[3]);
    return TRUE;
}

//�������ļ��л�ȡip�Ͷ˿�
int get_proxy_ip_port()
{
    FILE *fp;
    char buf[512] = {0};
    uint8 count = 0;
    char* tmp = NULL;
    fp = fopen(proxy_filepath,"r+");
    if(NULL == fp)
    {
        GUI_FMT_DEBUG("��ͨ�Ŵ��������ļ�ʧ�� %s ��\n",proxy_filepath);
        return -1;
    }
    while(fgets(buf,sizeof(buf),fp)!=NULL)
    {
        tmp = strstr(buf,ipkey);
        if(tmp!=NULL)
        {
            tmp+=strlen(ipkey);
            tmp++;//ƫ��һ���Ⱥ�
            //����Ҫȥ�����Ļ��з�
            if(TRUE == commfun_IP_unpack(strim(tmp),proxyip))
                GUI_FMT_DEBUG("��ȡip�ɹ� %d.%d.%d.%d ��\n",proxyip[0],proxyip[1],proxyip[2],proxyip[3]);
            count++;
            continue;
        }
        tmp = strstr(buf,portkey);
        if(tmp!=NULL)
        {
            tmp+=strlen(portkey);
            tmp++;
            proxyport = atoi(tmp);
            GUI_FMT_DEBUG("��ȡport�ɹ� %d��\n",proxyport);
            count++;
        }
        if(count==2)
            break;
    }
    fclose(fp);
    return 0;
}

void set_proxy_ip_port()
{
    FILE *fp;
    char buf[512] = {0};
    char str[100][512] = {0};
    uint16 i=0;
    uint16 linenum;

    fp = fopen(proxy_filepath,"r+");
    if(NULL == fp)
    {
        GUI_FMT_DEBUG("��ͨ�Ŵ��������ļ�ʧ�� %s ��\n",proxy_filepath);
        return ;
    }
    while(fgets(buf,sizeof(buf),fp)!=NULL)
    {
        if(strstr(buf,ipkey)!=NULL)
        {
            sprintf(str[i],"%s=%d.%d.%d.%d\n",ipkey,proxyip[0],proxyip[1],proxyip[2],proxyip[3]);
        }else if(strstr(buf,portkey)!=NULL)
        {
            sprintf(str[i],"%s=%d\n",portkey,proxyport);
        }else
        {
            strcpy(str[i],buf);
        }
        i++;
        GUI_FMT_DEBUG("i %d %s ��\n",i,buf);
        if(i>=100)
            break;
    }
    linenum = i;
    fseek(fp,0,SEEK_SET);
    for(i=0;i<linenum;i++)
    {
        fputs(str[i],fp);
    }
    fclose(fp);
}

void comm_proxy_Init(DWORD dwPluginId)
{
#ifndef AREA_HUNAN
    return;
#endif
    get_proxy_ip_port();
    g_bpop = TRUE;
}

void comm_proxy_Draw(HDC hdc)
{

#ifndef AREA_HUNAN
    MessageBox("�Ǻ��ϰ汾\n��֧���޸�ͨ�Ŵ������ã�");
    CancelMenu();
    return;
#endif
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    uint8 index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    char czBuf[300];
    int nTop = 40;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "��վIP");
    sprintf(czBuf,"%d.%d.%d.%d",proxyip[0],proxyip[1],proxyip[2],proxyip[3]);
    if(proxyindex == index)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT
    }else
        TextOut(hdc, nLeft2, nTop, czBuf);

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "��վ�˿�");
    sprintf(czBuf,"%d",proxyport);
    
    if(proxyindex == index)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT
    }else
        TextOut(hdc, nLeft2, nTop, czBuf);

    index++;
    nTop+=40;
    if(proxyindex == index)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT
    }else
        TextOut(hdc, nLeft1, nTop,  "ȡ��");

    index++;
    nLeft2 = 80;
    if(proxyindex == index)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft2, nTop, "����");
        ITEM_NOT_SELECT
    }else
        TextOut(hdc, nLeft2, nTop,  "����");

}
void proxyConfigReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };
    switch(proxyindex)
    {
    case 0:
    {
        if(FALSE == commfun_IP_unpack(pNewStr,tmp))
        {
            if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                sprintf(czBuf,"%d.%d.%d.%d",proxyip[0],proxyip[1],proxyip[2],proxyip[3]);
                InputBox(czBuf, proxyConfigReturn, NULL, 0x02);
                
            }
        }else
        {
            memcpy(proxyip,tmp,4);
        }
    }
        break;
    case 1:
    {
        int port = atoi(pNewStr);
        if(port<=65535)
        {
            proxyport = port;
        }else
        {
            if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                sprintf(czBuf, "%d", proxyport);
                InputBox(czBuf, proxyConfigReturn, NULL, 0x01);
            }
        }
    }
        break;
    }

}
 BOOL comm_proxy_Msg(int nKey)
 {
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int maxindex = 3;
    char czBuf[20];
    switch(nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if(proxyindex>0)
            proxyindex--;
         else
            proxyindex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (proxyindex < maxindex)
            proxyindex++;
        else
            proxyindex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if(ValidatePwd() == TRUE)
        {
            switch(proxyindex)
            {
            case 0://IP
            {
                sprintf(czBuf,"%d.%d.%d.%d",proxyip[0],proxyip[1],proxyip[2],proxyip[3]);
                InputBox(czBuf, proxyConfigReturn, NULL, 0x02 );
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            case 1://port
            {
                sprintf(czBuf, "%d", proxyport);
                InputBox(czBuf, proxyConfigReturn, NULL, 0x01);
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            case 2://ȡ��
            {
                CancelMenu();
                proxyindex = 0;
                return TRUE;
            }
            case 3: //����
            {
                set_proxy_ip_port();
                proxyindex =0;
                PopMsg("       ���óɹ�!\n     ��������...");
                GUI_FMT_DEBUG("�޸���� �����ն�\n");
                RebootTerminal();
                return TRUE;
            }
            }
        }
    }

    }

        
    return FALSE;
 }
PLUGIN_INFO g_layer_comm_proxy=
{
    0x00A000FB, // 0x00A000FB
    comm_proxy_Init,
    NULL,
    comm_proxy_Draw,
    NULL,
    comm_proxy_Msg,
    NULL
};

#if DESC("�ն˵�ַ����", 1)
uint8 LocalAddr[16] = { 0 };
int get_addr(uint8* Addr)
{
    int   ret;
    uint32 len = 0;
    NOMAL_OAD_T NormalOad;
	
	if(Addr == NULL)
	{
		return -1;
	}
	
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x40010200;
    NormalOad.infoNum = 0;
    OOP_OCTETVAR16_T LocalAddr;
    memset(&LocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
    ret = db_read_nomal(g_hUdbCenter, &NormalOad, sizeof(OOP_OCTETVAR16_T), (uint8*)&LocalAddr, &len);
    if (ret != 0 || len != sizeof(OOP_OCTETVAR16_T))
        return -1;
    memcpy(Addr, LocalAddr.value, LocalAddr.nNum);
    printf("�ն˵�ַ 0x%02x%02x%02x%02x%02x%02x \n", Addr[5], Addr[4], Addr[3], Addr[2], Addr[1], Addr[0]);
    return LocalAddr.nNum;
}

int write_normal_data(uint8* DataBuf, uint16 DataLen, OOP_OAD_U Oad, uint16 infonum, uint8 logicid, uint8 classtag)
{
    int ret;
    NOMAL_OAD_T NomalOad;
    NomalOad.logicId = logicid;
    NomalOad.oad = Oad;
    NomalOad.classtag = classtag;
    NomalOad.infoNum = infonum;

    ret = db_write_nomal(g_hUdbCenter, &NomalOad, DataBuf, DataLen);
    if (ret != 0)
    {
        GUI_FMT_DEBUG("��ͨ����д�����ݿ�ʧ��oad %08x  \n", Oad.value);
    }
    return ret;
}

void address_set_init(HDC hdc)
{
    get_addr(LocalAddr);
}

void address_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "�ն�ͨ�ŵ�ַ");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, pg_guicomm->fontSmall);

    memset(czBuf, 0x00, sizeof(czBuf));
#ifdef AREA_ZHEJIANG
    sprintf(czBuf, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
#else
    sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
#endif
    rc.top += 30;
    rc.bottom = rc.top + 15;
    ITEM_SELECT;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    ITEM_NOT_SELECT;
}

void AddrReturn(char* pNewStr, void* pPara)
{
    int nLen = strlen(pNewStr);
    int ret;

#ifdef AREA_ZHEJIANG
    if (nLen > 8 && TRUE == MessageBox("OOP�ն˵�ַ���ȴ���4�ֽ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        char tmp[20] = { 0 };
        sprintf(tmp, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
        InputBox(tmp, AddrReturn, NULL, 0x01);
    }
    else
    {
        char tmp[20] = { 0 };
        strcpy(tmp, "00000000");
        strcpy(tmp + 8 - nLen, pNewStr);
        printf("����ĵ�ַΪ %s \n", tmp);
        commfun_ASC2BCD((char*)LocalAddr, tmp, 8, 0);
        printf("ת��Ϊbcd %02x%02x%02x%02x \n", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
        OOP_OCTETVAR16_T TmpLocalAddr;
        memset(&TmpLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
        TmpLocalAddr.nNum = 4;
        memcpy(TmpLocalAddr.value, LocalAddr, 4);
        OOP_OAD_U oad = { 0x40010200 };
        ret = write_normal_data((uint8*)&TmpLocalAddr, sizeof(OOP_OCTETVAR16_T), oad, 0, 0, 1);
        if (ret == 0)
        {
            MessageBox("�ն˵�ַ���óɹ�");
        }
        else
        {
            MessageBox("�ն˵�ַ����ʧ��");
        }
    }
#else
    if (nLen > 12 && TRUE == MessageBox("OOP�ն˵�ַ���ȴ���6�ֽ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        char tmp[20] = { 0 };
        sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
        InputBox(tmp, AddrReturn, NULL, 0x01);
    }
    else
    {
        char tmp[20] = { 0 };
        strcpy(tmp, "000000000000");
        strcpy(tmp + 12 - nLen, pNewStr);
        printf("����ĵ�ַΪ %s \n", tmp);
        commfun_ASC2BCD((char*)LocalAddr, tmp, 12, 0);
        printf("ת��Ϊbcd %02x%02x%02x%02x%02x%02x \n", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
        OOP_OCTETVAR16_T TmpLocalAddr;
        memset(&TmpLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
        TmpLocalAddr.nNum = 6;
        memcpy(TmpLocalAddr.value, LocalAddr, 6);
        OOP_OAD_U oad = { 0x40010200 };
        ret = write_normal_data((uint8*)&TmpLocalAddr, sizeof(OOP_OCTETVAR16_T), oad, 0, 0, 1);
        if (ret == 0)
        {
            MessageBox("�ն˵�ַ���óɹ�");
        }
        else
        {
            MessageBox("�ն˵�ַ����ʧ��");
        }
    }
#endif
}

BOOL address_set_keymsg(int nKey)
{

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
        case KEY_OK:
        {
            if (ValidatePwd() == TRUE)
            {
                char tmp[20] = {0};
#ifdef AREA_ZHEJIANG
                sprintf(tmp, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
#else
                sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
#endif
                InputBox(tmp, AddrReturn, NULL, 0x01);
                return TRUE;
            }
            break;
        }
        default: break;
    }
    return FALSE;
}

//ͨ�ŵ�ַ����
PLUGIN_INFO g_layer_address_set = {
    0x11030011/*id*/,
    address_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    address_set_draw/*Draw*/,
    NULL/*Timer*/,
    address_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif





