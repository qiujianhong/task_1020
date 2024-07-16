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
#include "adc.h"
#include "gui_prtl.h"
#include "task_msg.h"
#include "msg_base.h"
#include "OopAll.h"
#include "desk_meter.h"


extern void CancelMenu();
int64 unlawPower = 0xeeeeeeeeeeeeeeee;
uint8 safe_status = 0;		//��ȫģʽ�� 0�����رգ�1����������Ĭ�Ϲر�
uint8 mac_status = 1;		//MACУ�飺 0�����رգ�1����������Ĭ�Ͽ���
uint8 hplc_mac_status = 0;      //�ز�MACУ�鿪�� Ĭ�Ϲر�
uint8 g_Relay485Flag = 0;   //�Ƿ�֧��ת��485���ڱ���: 1-enable 0-disable
uint8 g_MeterreadFlag = 0;  //�Ƿ�֧�ֳ����� 0-�رգ�1-����
extern OOP_GEOGADD_A_T GpsPosition;
extern uint8 getmodemversionflag;
extern char modemversion[4];
int nCurPoint;
DESK_METER_LIST_T meterList;
int safe_index = 0;
uint8 router_state =1; //·��ѧϰ״̬�� 0�����رգ�1����������Ĭ�Ͽ���
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
const char *g_meterread_list[] = {
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
    sprintf(buf, "RS485PATCH=%hu\n", g_MeterreadFlag);
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
#if defined AREA_HUNAN || defined AREA_HEBEI || defined AREA_JIANGXI
    NomalOad.classtag = 1;
#else
	NomalOad.classtag = 2;
#endif
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

/* �ӵ�ѹ������ȡĬ�ϲ��� */
void gui_get_default_gprs(OOP_GPRSCONFIG_T *ptCprs)
{
    read_pdata_xram("/mnt/data/app/puAmr/cfg/gprs_default", (char *)ptCprs, 0, sizeof(OOP_GPRSCONFIG_T));
}

uint8 sshsetindex;
uint8 g_sshopen;
void InitSSHSet(DWORD dwPluginId)
{
    char resultBuf[160] = {0};
    FILE* fp = NULL;
    g_sshopen = FALSE;

    if ((fp = popen("ps |grep \"/usr/sbin/sshd\"|grep -v grep|awk '{print $1}'", "r")) != NULL)
    {
        usleep(100000);
        fgets(resultBuf, sizeof(resultBuf), fp);
        if(0 != resultBuf[0])
        {
            g_sshopen = TRUE;
        }
        pclose(fp);
        fp = NULL;
    }
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

/**
*********************************************************************
* @brief��      SSH������Ȩ����
* @param[in]�� STA_PORT_AUTHORIZE_E status ������Ȩ״̬
* @return��     void
*********************************************************************
*/
int sta_ssh_service_authorize_operate()
{
    int    ret = 0;
    uint8  sendbuf[100] = {0};
    uint16 offset = 0;

    sendbuf[offset++] = 0x01;
    sendbuf[offset++] = 0x01;
    sendbuf[offset++] = 0xff;
    sendbuf[offset++] = 0xff;
    sendbuf[offset++] = 0xff;
    sendbuf[offset++] = 0xff;
    ret = gui_send_smios(0, SYSTEM_IOP_SERVICE_START, sendbuf, offset);
    if ((ret != ERR_OK) )
    {
       GUI_FMT_DEBUG("Reboot_set_to_smiOS err! ret = %d\n", ret);
    }
    else
    {
        GUI_FMT_DEBUG("Reboot_set_to_smiOS success! ret = %d\n", ret);
    }

    return ret;
}


//choice 0 ����ssh 1 �ر�ssh
void sshset(uint8 choice)
{
    char cmd[100]={0};
    if(choice ==0 && FALSE == g_sshopen)
    {
        sprintf(cmd,"/usr/sbin/sshd -D &");
        sta_ssh_service_authorize_operate();
    }
    else if(choice ==1 && TRUE == g_sshopen)
    {
        sprintf(cmd,"killall sshd");
    }
    if(0 != cmd[0])
    {
        printf("sshset %s \n",cmd);
        system(cmd);
    }
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
    char topic[256] = {0};
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

    snprintf(topic, 256,"%s/%s", desktopGui, pMsgInfo->destaddr);
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

unsigned int g_ACModuleIndex = 0;     //ѡ����: 0-����; 1-����
unsigned int g_ACModuleUpdateStatus = 0;    //״̬    : 0-δ��ʼ����;1-������װ�ļ�;2-acMeterĿ¼������;
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
    nLeft = 46;

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

    nLeft = 156;
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

int get_cpu_cur_temper(char * temp, int buf_len)
{
    if(temp == NULL)
        return -1;
    int cpu0 = -100;
    int cpu1 = -100;
    char buf[32] = {0};
    int value = 0;
    FILE* pipe0 = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    FILE* pipe1 = fopen("/sys/class/thermal/thermal_zone1/temp", "r");
    if(pipe0)
    {
        if(NULL != fgets(buf, 32, pipe0))
        {
            cpu0 = atoi(buf);
        }
        fclose(pipe0);
    }
    if(pipe1)
    {
        memset(buf, 0, 32);
        if(NULL != fgets(buf, 32, pipe1))
        {
            cpu1 = atoi(buf);
        }
        fclose(pipe1);
    }
    if(cpu0 <= -100 && cpu1 <= -100)
    {
        GUI_FMT_DEBUG("��ѯcpu��ǰ�¶�ʧ��!\n");
        return -1;
    }
    value = cpu0 > cpu1 ? cpu0 : cpu1;
    sprintf(temp, "%d.%0d��", value / 1000, value % 1000);
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
	strcpy(pcz, "CPU��ǰ�¶�");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);
    
    int nTop = 80;
    int nLeft = 26;
    
    TextOut(hdc, nLeft, nTop, "��ǰ�¶ȣ�");

    nLeft = 90;
    if(get_cpu_cur_temper(pcz, 40) == 0)
    {
        TextOut(hdc, nLeft, nTop, pcz);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----��");
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

/**********************************************************************
* @name      : gui_check_cell_insert
* @brief     ����ң���л�ȡ����Ƿ����
* @param[in] ��
* @param[out]��
* @return    ��TRUE-����/FALSE-û�в���
* @Create    : ����
* @Date      ��2022-4-13
* @Update    :
**********************************************************************/
uint8 gui_check_cell_insert()
{
    uint8  type = 1;
    uint32 data1 = 1;
    uint32 data2 = 2;
    uint32 data3 = 3;
    uint32 data4 = 4;
    uint32 data5 = 5;
    int bReadCount = 0;
    
    int fd = open("/dev/ym", O_RDONLY|O_NONBLOCK);
    if (fd < 0)
    {
        return 0;
    }
    
    while(bReadCount++ < 10)
    {
        read(fd, &data1, sizeof(data1));
        usleep(1000);
        
        read(fd, &data2, sizeof(data2));
        usleep(1000);
        
        read(fd, &data3, sizeof(data3));
        usleep(1000);
        
        read(fd, &data4, sizeof(data4));
        usleep(1000);
        
        read(fd, &data5, sizeof(data5));
        //����5�ζ�����ͬ��ֵ����ֹ����
        if(data1 == data2 && data1 == data3 && data1 == data4 && data1 == data5)
        {
            type = ((data1 >> 8) & 0x0001);
            break;
        }
    }
    close(fd);

    return 1 == type ? FALSE : TRUE;
}

void power_status_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return;
    //int       nTop = 0;
    char      czbuf[32];
    RECT      rc;
    uint8     flag = 0;
    uint8     flag1 = 0;
    uint8     flag2 = 0;
    //uint8     flag3 = 0;
    int32     state = 0;
    int       datav1 = 0;
    int       datav2 = 0;
    //int       datav3 = 0;
    int nTop = 16;
    int nLeft = 10;
    int nLeft2 = 80;

    POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
	if(NULL != dev)
	{
        dev->power_power_state_get(dev, POWER_MAIN, &state);
        flag = 1;
    	hal_device_release((HW_DEVICE *)dev);
	}
	
	SelectFont(hdc,g_guiComm.fontBig);

    //������ʾ
    ITEM_NOT_SELECT;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    rc.top = 24;
    rc.bottom = rc.top+16;
    char pcz[40] = {0};
    strcpy(pcz, "��Դ����״̬");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);
    nTop = rc.top + 24;
    TextOut(hdc, nLeft, nTop, "���緽ʽ��");
    if(0 == flag)
    {
        sprintf(czbuf, "----");
    }
    else
    {
        if(state == 1)
        {
            sprintf(czbuf, "��������");
        }
        else
        {
            sprintf(czbuf, "��ع���");
        }
    }
    TextOut(hdc, nLeft2, nTop, czbuf);

    ADC_DEVICE_T* adcdev = (ADC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_ADC);
    if(NULL != adcdev)
    {
        if(0 == adcdev->adc_volt_read(adcdev, EMADC_RTCVOL_CHANNEL, &datav1))
        {
            flag1 = 1;
        }
        if(0 == adcdev->adc_volt_read(adcdev, EMADC_BATVOL_CHANNEL, &datav2))
        {
            flag2 = 1;
        }
        //if(0 == adcdev->adc_volt_read(adcdev, EMADC_CAPVOL_CHANNEL, &datav3))
        //{
            //flag3 = 1;
        //}
        hal_device_release((HW_DEVICE *)adcdev);
    }

    nTop += 20;
    TextOut(hdc, nLeft, nTop, "ʱ�ӵ�أ�");
    memset(czbuf, 0, 32);
    if(0 == flag1)
    {
        sprintf(czbuf, "----");
    }
    else
    {
        sprintf(czbuf, "%d.%03d V", datav1 / 1000, datav1 % 1000);
    }
    TextOut(hdc, nLeft2, nTop, czbuf);

    nTop += 20;
    TextOut(hdc, nLeft, nTop, "���õ�أ�");
    memset(czbuf, 0, 32);
    if(0 == flag2 || FALSE == gui_check_cell_insert())
    {
        sprintf(czbuf, "----");
    }
    else
    {
        sprintf(czbuf, "%d.%03d V", datav2 / 1000, datav2 % 1000);
    }
    TextOut(hdc, nLeft2, nTop, czbuf);

    /*nTop += 20;
    TextOut(hdc, nLeft, nTop, "�������ݣ�");
    memset(czbuf, 0, 32);
    if(0 == flag3)
    {
        sprintf(czbuf, "----");
    }
    else
    {
        sprintf(czbuf, "%d.%03d V", datav3 / 1000, datav3 % 1000);
    }
    TextOut(hdc, nLeft2, nTop, czbuf);*/
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
extern char ac_minver[100];
extern char ac_data_ver[100];
extern char ac_ver[100];
extern char ac_meterIC_ver[100];

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
    int nLeft = 62;
    
    TextOut(hdc, nLeft, nTop, "ģ��汾��");

    nLeft = 120;
    memset(pcz, 0, 40);
    if (strlen(ac_ver) != 0)
    {
        TextOut(hdc, nLeft, nTop, ac_ver);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }

    nTop = 80;
    nLeft = 62;
    
    TextOut(hdc, nLeft, nTop, "�ڲ��汾��");

    nLeft = 120;
    if (strlen(ac_minver) != 0)
    {
        TextOut(hdc, nLeft, nTop, ac_minver);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }

    nTop = 100;
    nLeft = 62;
    
    TextOut(hdc, nLeft, nTop, "�汾���ڣ�");

    nLeft = 120;
    if (strlen(ac_data_ver) != 0)
    {
        TextOut(hdc, nLeft, nTop, ac_data_ver);
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
    if (strlen(ac_meterIC_ver) != 0)
    {
        TextOut(hdc, nLeft, nTop, ac_meterIC_ver);
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

#if DESC("���߲��ŷ�ʽ����", 1)
uint8 g_Wireless_status = 0;
uint8 g_Wireless_statusOld = 0;
uint8 g_WirelessIndex = 0;

//��Ӫ��
const char* g_WirelessStatusName[] =
{
    "USB",
    "����",
    "����Ӧ"
};

void wirelessStatus_set_init(HDC hdc)
{
    if(0 == access("/data/app/wirelessDCM/data/bForceUart", F_OK))
    {
        g_Wireless_status = 1;
    }
    else if(0 == access("/data/app/wirelessDCM/data/bForceUsb", F_OK))
    {
        g_Wireless_status = 0;
    }
    else
    {
        g_Wireless_status = 2;
    }
    g_WirelessIndex = 0;
    g_Wireless_statusOld = g_Wireless_status;
}

void wirelessStatus_set_draw(HDC hdc)
{
    int nTop = 24;
    int nLeft1 = 85;
    int nLeft2 = 115;
    int nLeft3 = 130;

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    TextOut(hdc, 70, nTop, "���߲��ŷ�ʽ");
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "��������");

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
        TextOut(hdc, 46, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 46, nTop, "����");
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
    char cmd[128]={0};
    
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
                    PopList(g_WirelessStatusName, 3, g_WirelessStatusName[g_Wireless_status], 120, 80, FALSE);
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                    return TRUE;
                }
                else if(2 == g_WirelessIndex)
                {
                    if(g_Wireless_status != g_Wireless_statusOld)
                    {
                        if(0 == g_Wireless_status)//ʹ��USB
                        {
                            sprintf(cmd,"rm /data/app/wirelessDCM/data/bForceUart");
                            popen(cmd,"r");
                            sprintf(cmd,"touch /data/app/wirelessDCM/data/bForceUsb");
                            popen(cmd,"r");
                        }
                        else if(1 == g_Wireless_status)//ʹ�ô���
                        {
                            sprintf(cmd,"rm /data/app/wirelessDCM/data/bForceUsb");
                            popen(cmd,"r");
                            sprintf(cmd,"touch /data/app/wirelessDCM/data/bForceUart");
                            popen(cmd,"r");
                        }
                        else
                        {
                            sprintf(cmd,"rm /data/app/wirelessDCM/data/bForceUsb");
                            popen(cmd,"r");
                            sprintf(cmd,"rm /data/app/wirelessDCM/data/bForceUart");
                            popen(cmd,"r");
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
    for (int i =0;i<4;i++){
        if(atoi(p1[i]) > 255)
            return FALSE;
        ipout[i] = atoi(p1[i]);
    }
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

#define MAX_METER_INFONUM   3600   //�����������Ϣ���
#define MAX_TASK_INFONUM 1000       //���������Ϣ���
#define MAX_PLAN_INFONUM 1000       //���������Ϣ���
#define MAX_SHAANXI_SELECT 9        //����Ĭ��9������ͨѶ����

const OOP_OAD_U MeterFileOad = { 0x60000200 };
const OOP_OAD_U NormalPlanOad = { 0x60140200 };
const OOP_OAD_U TaskOad = { 0x60120200 };
const OOP_OAD_U EthIPOad = { 0x45100400 };
const OOP_OAD_U EthConfigOad = { 0x45100200 };
const OOP_OAD_U EthMasterOad = { 0x45100300 };
//const OOP_OAD_U GPRSConfigOad = { 0x45000200 };
//const OOP_OAD_U GPRSMasterOad = { 0x45000300 };
const OOP_OAD_U SecuConfigOad = { 0xF1010200 };
const char* g_pczRadioDelay[] = { "100", "200", "300", "400", "500", "800", "1000", "2000" };
//�������
const char* g_baud[] = { "300bps","600bps","1200bps","2400bps","4800bps","7200bps","9600bps","19200bps","38400bps","57600bps","115200bps","����Ӧ" };

//��Լ����
const char* g_prtl[] = { "δ֪","DL/T 645-1997","DL/T 645-2007","DL/T 698.45","CJ/T 188-2004", "ModBus" };

//��Լ����
const char* g_pwType[] = { "δ֪","����","��������","��������" };


//TI��ʽʱ����
const char* g_ti[] = { "��","��","ʱ","��","��","��" };

//��������
const char* g_plantype[] = { "��ͨ�ɼ�����","�¼��ɼ�����","͸������","�ϱ�����","�ű�����" };

//��ͨ�ɼ�����
const char* g_normalPlanType[] = {"�ɼ���ǰ����", "�ɼ��ϵ�N��", "������ʱ��ɼ�", "��ʱ�����ɼ�", "����"};

//MS����
const char* g_ms[] = {"�ޱ��", "ȫ���û���ַ", "һ���û�����", "һ���û���ַ", "һ���������", "һ���û���������", "һ���û���ַ����", "һ�������������"};

//�洢ʱ��
const char* g_tmFlag[] = {"δ����", "����ʼʱ��", "��Ե���0:0", "�������23:59", "�������0:0", "��Ե���1��0:0", "���ݶ���ʱ��", "���������ĩ23:59"};

//��������
const char* g_regionType[] = {"ǰ�պ�", "ǰ�����", "ǰ�պ��", "ǰ����"};

//�¼��ɼ�����
const char* g_eventPlanType[] = {"���ڲɼ�", "����֪ͨ�ɼ�����", "����֪ͨ�ɼ�ָ��", "����֪ͨ�洢"};

//�ϱ���ʶ
const char* g_eventReportFlag[] = {"���ϱ�", "�����ϱ�"};

//�ϱ���������
const char* g_reportPlanType[] = {"��������", "��¼�Ͷ�������"};

//�ϱ�����RSD
const char* g_reportPlanRSD[] = {"��ѡ��", "ѡ�񷽷�1",  "ѡ�񷽷�2",  "ѡ�񷽷�3",  "ѡ�񷽷�4",  "ѡ�񷽷�5",  "ѡ�񷽷�6",
                                 "ѡ�񷽷�7",  "ѡ�񷽷�8",  "ѡ�񷽷�9",  "ѡ�񷽷�10"};

//�˿�
const char* g_pipe[] = {"485-1","485-2","����-1","����-2","����-3","����-4","����-5","����-6","����-7","����-8","����", "�ز�-1", "�ز�-2" };
//����
const char* g_pulse[] = {"����-1","����-2","����-3","����-4","����-5","����-6","����-7","����-8" };
//����
const char* g_on_off[] = { "�ر�","����" };

//ip���÷�ʽ
const char* g_ipconfig[] = { "DHCP","��̬","PPPoE" };

//ͨ�����ù���ģʽ
const char* g_work_mode[] = { "���ģʽ","�ͻ���ģʽ","������ģʽ" };

//ͨ�����ù���ģʽ
const char* g_network_mode[] = { "����ר��ģʽ","�̶�����ģʽ","�̶�ר��ģʽ","���ȹ���ģʽ","�����л�ģʽ" };
uint8 networkmode;

//��Ӫ�̴���
const char* g_operator_seclct[] = { "�й��ƶ�","�й�����","�й���ͨ","ר��","δ֪" };//��Ӫ�� �ƶ�CMCC��0��������CTCC��1������ͨCUCC��2����δ֪��255��

//��������
const char* g_net_mode[] = { "2G","3G","4G","5G","����Ӧ"};

//����
const char* longitude[] = { "E","W"};

//γ��
const char* latitude[] = { "S","N"};


//��Ȩ��ʽ
const char* g_auth_mode[] = { "null","chap","pap","pap&chap","����Ӧ"};

//���ӷ�ʽ
const char* g_conn_type[] = { "TCP","UDP" };

//����Ӧ�÷�ʽ
const char* g_conn_app_type[] = { "����ģʽ","������ģʽ" };

//APN
const char* g_apn_list[] = { "CMIOTDLCJXT.CQ","CQGWDL1.GZM2MAPN","WLWCQDL.VPDN.CQ" };

//APN���÷�ʽ
const char* g_apnSetType_list[] = { "�˵�ѡ��", "�ֶ�����"};

//���߷�ʽ
const char* g_line_type[] = { "��������","��������" };

//�ز�Ƶ��ѡ�����û��߲�ѯ
const char* g_Hplc_type[] = { "�����ز�Ƶ��","��ѯ�ز�Ƶ��" };

const char*  g_sensor_name_value[]={"��ʪ�ȴ�����","ˮ��������","��������","׮ͷ�¶ȴ�����"};

typedef struct
{
	OOP_ENERGY_T        penergy;         //�����й� 0x00100200
	OOP_ENERGYS_T       penergys;        //�����޹� 0x00300200
	OOP_ENERGY_T        nenergy;         //�����й� 0x00200200
	OOP_ENERGYS_T       nenergys;        //�����޹� 0x00400200
	OOP_ENERGY_T        quadrant1;       //��һ���� 0x00500200
	OOP_ENERGY_T        quadrant2;       //�ڶ����� 0x00600200
	OOP_ENERGY_T        quadrant3;       //�������� 0x00700200
	OOP_ENERGY_T        quadrant4;       //�������� 0x00800200
	OOP_WORD3V_T        vol;             //��ѹ       0x20000200
	OOP_INT3V_T         cur;             //����       0x20010200
	OOP_INT4V_T         ppower;          //�й�����     0x20040200
	OOP_INT4V_T         npower;          //�޹�����     0x20050200
	OOP_LONG4V_T        fpower;          //��������     0x200A0200
	OOP_DEMAND_T        pdemand;         //�����й����� 0x10100200
	OOP_DEMAND_T        ndemand;         //�����й����� 0x10200200
	OOP_HARMONIC_T      harmonicV[3];    //�����ѹг��������0x200D0200  0 x200D0400
    OOP_HARMONIC_T      harmonicC[3];    //�������г��������0x200E0200  0 x200E0400
    OOP_HARMONIC_U_VALUE_T      harmonicV_value[3];    //�����ѹг������0x20330200  0 x20330400
    OOP_HARMONIC_I_VALUE_T      harmonicC_value[3];    //�������г������0x20340200  0 x20340400
	OOP_CHINESEINFOS_T         chinese_info;
	OOP_SWITCHOUT_T     switchout;
	OOP_SECURITYFLAG_E  safemode;
	OOP_DATETIME_S_T	memtTime;
}DISPLAY_DATA_INFO;

typedef struct
{
	OOP_ENERGY_T        penergy;         //�����й� 0x00100200
	OOP_ENERGYS_T       penergys;        //�����޹� 0x00300200
	OOP_ENERGY_T        nenergy;         //�����й� 0x00200200
	OOP_ENERGYS_T       nenergys;        //�����޹� 0x00400200
	OOP_ENERGY_T        quadrant1;       //��һ���� 0x00500200
	OOP_ENERGY_T        quadrant2;       //�ڶ����� 0x00600200
	OOP_ENERGY_T        quadrant3;       //�������� 0x00700200
	OOP_ENERGY_T        quadrant4;       //�������� 0x00800200
}DISPLAY_DAY_FROZEN_INFO;

typedef struct
{
	OOP_HENERGY_T        penergy;         //�����й� 0x00100400
	OOP_HENERGYS_T       penergys;        //�����޹� 0x00300400
	OOP_HENERGY_T        nenergy;         //�����й� 0x00200400
	OOP_HENERGYS_T       nenergys;        //�����޹� 0x00400400
	OOP_HENERGY_T        quadrant1;       //��һ���� 0x00500400
	OOP_HENERGY_T        quadrant2;       //�ڶ����� 0x00600400
	OOP_HENERGY_T        quadrant3;       //�������� 0x00700400
	OOP_HENERGY_T        quadrant4;       //�������� 0x00800400
	OOP_DEMAND_T         pdemand;         //�����й����� 0x10100200
	OOP_DATETIME_S_T     memtTime;
}HIGH_PRECISION_DATA_INFO;

const char* g_showTitleVoltag34[] = { "Ua", "Ub", "Uc" };
const char* g_showTitleEelectric[] = { "Ia", "Ib", "Ic" };
const char* g_showTitleJCPower_Y[] = { "�����й�����", "A���й�����", "B���й�����","C���й�����" };
const char* g_showTitleJCPower_W[] = { "�����޹�����", "A���޹�����", "B���޹�����","C���޹�����" };
const char* g_showTitleJCPowerFactor[] = { "�ܹ������� ", "A�๦������", "B�๦������", "C�๦������" };
const char* g_showSwitchState[] = { "��", "��" };
const char* g_showSwitchFlag[] = { "δ����", "����" };
const char* g_showCtrlState[] = { "��", "��" };
const char* g_showtaskStatus[] = { "δִ��", "ִ����", "��ִ��" };
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
};


const char* g_pczDirect[] = {"��", "��" };
const char* g_pczOperator[] = {"��", "��" };

typedef struct
{
    int32            plus_active_power;         //�����й����� 0x24010500
    int32            plus_reactive_power;       //�����޹����� 0x24010600
    int64            gruop_active_power;        //�ܼ����й����� 0x23010300
    int64            gruop_reactive_power;      //�ܼ����޹����� 0x23010400
    OOP_HENERGYS_T   gruop_day_energy;        //�ܼ������й����� 0x23010700
    OOP_HENERGYS_T   gruop_day_ractenergy;    //�ܼ������޹����� 0x23010800
    OOP_HENERGYS_T   gruop_month_energy;      //�ܼ������й����� 0x23010900
    OOP_HENERGYS_T   gruop_month_ractenergy;  //�ܼ������޹����� 0x23010a00
    OOP_BITSTR8_T    ctrlPwTurn;              //0x230x0e00 �ܼ��鹦���ִ�����  ���ƶ�ȡ
    OOP_BITSTR8_T    ctrlEcTurn;              //0x230x0f00 �ܼ������ִ�����  ���ƶ�ȡ
    OOP_TGSETTING_T  ctrl_seting;             //��������״̬         0x23011000
    OOP_TGCURSTATE_T ctrl_status;             //���Ƶ�ǰ״̬         0x23011100
    OOP_PULSESETS_T  plus_param;              //��������       0x24010400
    OOP_PULSESETS_B_T plus_b_param;           //��������       0x24011C00
    OOP_OCTETVAR16_T pulse_addr;              //�����ַ       0x24010200
    OOP_TGSETS_T     gruop_param;             //��������       0x23010200
}CTRL_DATA_INFO;

CTRL_DATA_INFO g_ctrl_data[8];
CTRL_DATA_INFO g_group_data;
uint8            eletype;                 //����
OOP_8107_CONFIG_T tBuyenergyconfig;
uint8            g_bGuiBuyLastData = 0;
uint8            g_bGuiBuyLastType = 0;
int64            g_GuiBuyLast = 0;
uint8            g_bGuiBuyEndData = 0;
int64            g_GuiBuyEnd = 0;
int64      		 g_wallet[8];
int64     		 g_power[8];
uint8 LocalAddr[16] = { 0 };
uint8 ClientAddr[16] = { 0 };
extern  uint8 netmode;
extern  uint8 getnetmodeflag;
extern  uint8 getpositionflag;

DISPLAY_DATA_INFO display_data;
DISPLAY_DATA_INFO display_datas;
DISPLAY_DAY_FROZEN_INFO display_day_frozen;

HIGH_PRECISION_DATA_INFO g_high_precision_data;
OOP_OAD_U GPRSConfigOad;
OOP_OAD_U GPRSMasterOad;
MOD_INFO_ACK_T g_module_info[5];


#if DESC("�ն˵�ַ",1)

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
#endif


int get_clientno(uint8* Addr)
{

    int   ret;
    uint32 len = 0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x40030200;
    NormalOad.infoNum = 0;
    OOP_OCTETVAR16_T LocalAddr;
    memset(&LocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
    ret = db_read_nomal(g_hUdbCenter, &NormalOad, sizeof(OOP_OCTETVAR16_T), (uint8*)&LocalAddr, &len);
    if (ret != 0 || len != sizeof(OOP_OCTETVAR16_T))
    {
        return -1;
    }
    memcpy(Addr, LocalAddr.value, 6);
    printf("�ͻ���� 0x%02x%02x%02x%02x%02x%02x \n", Addr[5], Addr[4], Addr[3], Addr[2], Addr[1], Addr[0]);
    return LocalAddr.nNum;
}

/**
*********************************************************************
* @brief��      ������������
* @param[in]�� OOP_GPRSCONFIG_T *pGprsConfig   ����
* @param[out]��OOP_GPRSCONFIG_T *pGprsConfig   ������
* @return��     TRUE-�û����������/FALSE-δ����
               <0            - ������
*********************************************************************
*/
uint8 grps_fujian_apn(OOP_GPRSCONFIG_T *pGprsConfig)
{
    if(0 == strcmp("fjep.fj", pGprsConfig->apn.apn.value) || 0 == strcmp("FJEP.FJ", pGprsConfig->apn.apn.value))
    {
        int   addrlen = 0;
        uint8 pAddr[16] = {0};
        int   i = 0;
        OOP_VISIBLEVAR_T username;
        OOP_VISIBLEVAR_T pwd;

        /* ����ȡ�ն˵�ַ */
        memset(&pwd, 0, sizeof(OOP_VISIBLEVAR_T));
        addrlen = get_clientno(pAddr);
        if(addrlen <= 0)
        {
            memset(pwd.value, '0', 12);
            pwd.nNum = 12;
        }
        else
        {
            for(i = 0; i < addrlen; i++)
            {
                pwd.value[2 * (addrlen - i) - 2] = (pAddr[i] & 0x0f) + '0';
                pwd.value[2 * (addrlen - i) - 1] = ((pAddr[i] >> 4) & 0x0f) + '0';
                pwd.nNum += 2;
            }
        }
        
        /* �û��� */
        memset(&username, 0, sizeof(OOP_VISIBLEVAR_T));
        MEMCPY_R(username.value, pwd.value, pwd.nNum);
        username.nNum = pwd.nNum;

        if(0 != memcmp(&username, &(pGprsConfig->apn.username), sizeof(OOP_VISIBLEVAR_T)) || 
           0 != memcmp(&pwd, &(pGprsConfig->apn.pwd), sizeof(OOP_VISIBLEVAR_T)))
        {
            memcpy(&(pGprsConfig->apn.username), &username, sizeof(OOP_VISIBLEVAR_T));
            memcpy(&(pGprsConfig->apn.pwd), &pwd, sizeof(OOP_VISIBLEVAR_T));
            return TRUE;
        }
        
    }
    return FALSE;
}


//״̬�� ��Ϣ��Ŵ���
void Uninit()
{
    set_gui_infonum(0);
}
    
#if DESC("�������Ķ�д",1)
/**********************************************************************
* @name      : write_normal_data
* @brief     ����ͨ����д��������
* @param[in] ��DataBuf			��������
			   DataLen			���ݳ���
			   Oad				����OAD
			   infonum			��Ϣ���
			   logicid			�߼��豸��
			   classtag			��������
* @param[out]��
* @return    : 0/������
* @Create    : 
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
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

/**********************************************************************
* @name      : read_db_noamal_data
* @brief     �����������Ķ���ͨ����
* @param[in] ��
			   fp				��������ͨ�ž��
			   oad_value		����OAD
			   logicid			�߼��豸��
			   infonum			��Ϣ���
* @param[out]��pbuf				��������
			   buf_len			���ݳ���
* @return    : 0/������
* @Create    : 
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
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
        GUI_FMT_DEBUG("db_read_nomal 0x%08x err ret=%d, recNum=%d, buf_len=%u!\n", oad_value, ret, len, buf_len);
        return -1;
    }

    return 0;
}

/**********************************************************************
* @name      : app_get_meter_list
* @brief     ����ȡ��������
* @param[in] ��i        - �������
* @param[out]��oopMeter - ����
* @return    : ������
* @Create    : 
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
int app_get_meter_list(uint32 infoNum, OOP_METER_T *oopMeter)
{
	return read_db_noamal_data(g_hUdbCenter, 0x60000200, 0, (uint16)infoNum, (uint8*)oopMeter, sizeof(OOP_METER_T));

}

/**********************************************************************
* @name      : read_db_record_data
* @brief     �����������Ķ���¼������
* @param[in] ��
			   fp				��������ͨ�ž��
			   oad_value		����OAD
			   logicid			�߼��豸��
			   infonum			��Ϣ���
* @param[out]��pbuf				��������
			   buf_len			���ݳ���
* @return    : 0/������
* @Create    : 
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
int read_db_record_data(DB_CLIENT fp, uint32 oad_value, uint8 logic_id, uint16 info_num, uint8* pbuf, uint32 buf_len)
{
    int                ret = ERR_NORMAL;
    uint32             len     = 0;
    time_t             timep = 0;

    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_METER_T        oopMeter;
    NOMAL_OAD_T        nomalOAD;
    OOP_OCTETVAR16_T   m_tmnAddr;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep

    inReadRecord.recordOAD.logicId = logic_id;
    inReadRecord.recordOAD.infoNum = info_num;
    inReadRecord.cType = COL_TM_END;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = timep + COMPENSATION_TIME;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad_value;

    inReadRecord.MAC.nNum = 6;

    if(info_num == 0)
    {
        memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
        nomalOAD.logicId = logic_id;
        nomalOAD.oad.value = 0x40010200;
        ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&m_tmnAddr, &len);
        if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
        {
            GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
            return -1;
        }
         memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);
    }
    else
    {
        if (app_get_meter_list(info_num, &oopMeter) != ERR_OK)
        {
            GUI_FMT_DEBUG(" app_get_meter_list %d !\n", info_num);
            return -1;
        }
        memcpy(inReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
    }

    ret = db_read_record(fp, &inReadRecord, buf_len, &stepRecordOut);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG(" db_read_record %08x ret=%d !\n", oad_value, ret);
        return -1;
    }
    memcpy(pbuf, stepRecordOut.record[0].outDataBuf, buf_len);

    return 0;
}

/**********************************************************************
* @name      : read_db_record_data_time
* @brief     �����������Ķ���¼�����ݺͼ�¼ʱ��
* @param[in] ��
			   fp				��������ͨ�ž��
			   oad_value		����OAD
			   logicid			�߼��豸��
			   infonum			��Ϣ���
* @param[out]��pbuf				��������
			   buf_len			���ݳ���
* @return    : 0/������
* @Create    : 
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
int read_db_record_data_time(DB_CLIENT fp, uint32 oad_value, uint8 logic_id, uint16 info_num, uint8* pbuf, uint32 buf_len, OOP_DATETIME_S_T *dateTime)
{
    int                ret = ERR_NORMAL;
    uint32             len     = 0;
    time_t             timep = 0;

    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_METER_T        oopMeter;
    NOMAL_OAD_T        nomalOAD;
    OOP_OCTETVAR16_T   m_tmnAddr;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep

    inReadRecord.recordOAD.logicId = logic_id;
    inReadRecord.recordOAD.infoNum = info_num;
    inReadRecord.cType = COL_TM_END;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = timep + COMPENSATION_TIME;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad_value;

    inReadRecord.MAC.nNum = 6;

    if(info_num == 0)
    {
        memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
        nomalOAD.logicId = logic_id;
        nomalOAD.oad.value = 0x40010200;
        ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&m_tmnAddr, &len);
        if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
        {
            GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
            return -1;
        }
         memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);
    }
    else
    {
        if (app_get_meter_list(info_num, &oopMeter) != ERR_OK)
        {
            GUI_FMT_DEBUG(" app_get_meter_list %d !\n", info_num);
            return -1;
        }
        memcpy(inReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
    }

    ret = db_read_record(fp, &inReadRecord, buf_len, &stepRecordOut);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG(" db_read_record %08x ret=%d !\n", oad_value, ret);
        return -1;
    }
    memcpy(pbuf, stepRecordOut.record[0].outDataBuf, buf_len);

    DT_Time2DateTimeShort(stepRecordOut.colEndTm - COMPENSATION_TIME , dateTime);
    return 0;
}


/**********************************************************************
* @name      : read_db_record_data_main
* @brief     �����������Ķ���¼������
* @param[in] ��
			   fp				��������ͨ�ž��
			   oad_main			������oad
			   oad_value		������OAD
			   logicid			�߼��豸��
			   infonum			��Ϣ���
* @param[out]��pbuf				��������
			   buf_len			���ݳ���
* @return    : 0/������
* @Create    : 
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
int read_db_record_data_main(DB_CLIENT fp, uint32 oad_main, uint32 oad_value, uint8 logic_id, uint16 info_num, uint8* pbuf, uint32 buf_len)
{
    int                ret = ERR_NORMAL;
    uint32             len     = 0;
    time_t             timep = 0;

    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_METER_T        oopMeter;
    NOMAL_OAD_T        nomalOAD;
    OOP_OCTETVAR16_T   m_tmnAddr;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep

    inReadRecord.recordOAD.logicId = logic_id;
    inReadRecord.recordOAD.infoNum = info_num;
    inReadRecord.cType = COL_TM_END;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = timep + COMPENSATION_TIME;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.recordOAD.optional = 1;
	inReadRecord.recordOAD.road.oadMain.value = oad_main;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad_value;

    inReadRecord.MAC.nNum = 6;

    if(info_num == 0)
    {
        memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
        nomalOAD.logicId = logic_id;
        nomalOAD.oad.value = 0x40010200;
        ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&m_tmnAddr, &len);
        if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
        {
            GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
            return -1;
        }
         memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);
    }
    else
    {
        if (app_get_meter_list(info_num, &oopMeter) != ERR_OK)
        {
            GUI_FMT_DEBUG(" app_get_meter_list %d !\n", info_num);
            return -1;
        }
        memcpy(inReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
    }

    ret = db_read_record(fp, &inReadRecord, buf_len, &stepRecordOut);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG(" db_read_record %08x ret=%d !\n", oad_value, ret);
        return -1;
    }
    memcpy(pbuf, stepRecordOut.record[0].outDataBuf, buf_len);

    return 0;
}

int read_db_record_6e20(DB_CLIENT fp, uint16 id, uint8* pbuf, uint32 buf_len)
{
    int                ret = ERR_NORMAL;
    time_t             timep = 0;

    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;


    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = id;
    inReadRecord.cType = COL_TM_START;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = timep;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6e200200;

    ret = db_read_record(fp, &inReadRecord, buf_len, &stepRecordOut);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG(" db_read_record 0x6e200200 ret=%d !\n", ret);
        return -1;
    }
    memcpy(pbuf, stepRecordOut.record[0].outDataBuf, buf_len);

    return 0;
}


#endif
    


MEASURE_MAIN g_measure_main_data;
OOP_METER_T  MeterFile;
const char*  g_measure_point_state[] = {"��Ч", "��Ч"};
const char*  g_measure_point_type[] = {"-", "485��", "�ز���", "���ɱ�", "�����"};

NETWORKMODE_MAIN g_networkmode_main_data;

//���˿�OADת��Ϊ����������
int get_infonum_type(OOP_OAD_U port)
{
    switch (port.nObjID)
    {
    case 0xF209:
    {
        if (port.attID == 2)
            return 2;
    }
    	break;
    case 0xF201:
    {
        if (port.attID == 2)
            return 1;
    }
    	break;
	case 0xF208:
    {
        if (port.attID == 2)
            return 3;
    }
		break;
	case 0xF20A:
    {
        if (port.attID == 2)
            return 4;
    }
		break;
    default:
        break;
    }
    return 0;

}


//������������ת��Ϊ�˿�OAD
int set_port_type(int type)
{
    switch (type)
    {
    case 2:
    {
        if (MeterFile.basic.port.attID == 2)
            MeterFile.basic.port.nObjID = 0xF209;
    }
    	break;
    case 1:
    {   
        if (MeterFile.basic.port.attID == 2)
            MeterFile.basic.port.nObjID = 0xF201;
    }
    	break;
	case 3:
    {
        if (MeterFile.basic.port.attID == 2)
        MeterFile.basic.port.nObjID = 0xF208;
    }
		break;
	case 4:
    {
        if (MeterFile.basic.port.attID == 2)
        MeterFile.basic.port.nObjID = 0xF20A;
    }
		break;
    default:
        break;
    }
    return 0;

}

#if DESC("����״̬",1)

const char *gCtrStatusStr[] = {"----", "���", "Ͷ��"};
const char *gCtrlProtectStatusStr[] = {"----", "���", "����", "�Զ�����"};

uint16 g_CtrlInfoNum = 0;

int8  g_CtrlPeriodEffect = 0;
int8  g_CtrlRestEffect = 0;
int8  g_CtrlStopEffect = 0;
int8  g_CtrlDownEffect = 0;

int8  g_CtrlBuyEffect = 0;
int8  g_CtrlMonthEffect = 0;

int8  g_CtrlProtectStatus = 0;

void ControlStatusInit(DWORD dwPluginID)
{
	g_guiComm.nCurPoint = (dwPluginID>>8)&0xff;
	set_gui_infonum((uint16)g_guiComm.nCurPoint);
	
	g_CtrlInfoNum = g_guiComm.nCurPoint;
	g_CtrlPeriodEffect = -1;
    g_CtrlRestEffect = -1;
    g_CtrlStopEffect = -1;
    g_CtrlDownEffect = -1;
    read_db_noamal_data(g_hUdbCenter, 0x810306ff, 0, g_CtrlInfoNum, (uint8*)&g_CtrlPeriodEffect, sizeof(uint8));
    read_db_noamal_data(g_hUdbCenter, 0x810406ff, 0, g_CtrlInfoNum, (uint8*)&g_CtrlRestEffect, sizeof(uint8));
    read_db_noamal_data(g_hUdbCenter, 0x810506ff, 0, g_CtrlInfoNum, (uint8*)&g_CtrlStopEffect, sizeof(uint8));
    read_db_noamal_data(g_hUdbCenter, 0x810606ff, 0, g_CtrlInfoNum, (uint8*)&g_CtrlDownEffect, sizeof(uint8));
	g_CtrlBuyEffect = -1;
    g_CtrlMonthEffect = -1;
    read_db_noamal_data(g_hUdbCenter, 0x810706ff, 0, g_CtrlInfoNum, (uint8*)&g_CtrlBuyEffect, sizeof(uint8));
    read_db_noamal_data(g_hUdbCenter, 0x810806ff, 0, g_CtrlInfoNum, (uint8*)&g_CtrlMonthEffect, sizeof(uint8));

    g_CtrlProtectStatus = -1;
    read_db_noamal_data(g_hUdbCenter, 0x80010200, 0, 0, (uint8*)&g_CtrlProtectStatus, sizeof(uint8));
}

/*********************************************************************
�������ܵ�Ԫ����̬ˢ��ҳ����ƺ���
���������������ն˸�������״̬��Ͷ�����
�����������
�����������
��������ֵ����
�ڲ��������̣� 
�����ɳ�ʼ���溯����ÿ������guiComm.nCurPoint,����4��
����������
********************************************************************/
void ControlStatusDraw(HDC hdc)
{
	SelectFont(hdc,g_guiComm.fontBig);


	int nLeft1 = 30;
	int nLeft2 = 130;
	int nTop = 15;

	TextOut(hdc, nLeft1, nTop, "�¸���");
	TextOut(hdc, nLeft1, nTop+16*1, "��ͣ��");
	TextOut(hdc, nLeft1, nTop+16*2, "���ݿ�");
	TextOut(hdc, nLeft1, nTop+16*3, "ʱ�ο�");
	TextOut(hdc, nLeft1, nTop+16*4, "�µ��");
	TextOut(hdc, nLeft1, nTop+16*5, "�����");
	TextOut(hdc, nLeft1, nTop+16*6, "��  ��");

	TextOut(hdc, nLeft2, nTop, gCtrStatusStr[g_CtrlDownEffect+1]); // ��
	TextOut(hdc, nLeft2, nTop+16*1, gCtrStatusStr[g_CtrlStopEffect+1]);  // ��
	TextOut(hdc, nLeft2, nTop+16*2, gCtrStatusStr[g_CtrlRestEffect+1]);  // ��
	TextOut(hdc, nLeft2, nTop+16*3, gCtrStatusStr[g_CtrlPeriodEffect+1]); // ʱ	
	TextOut(hdc, nLeft2, nTop+16*4, gCtrStatusStr[g_CtrlMonthEffect+1]);	// ��
	TextOut(hdc, nLeft2, nTop+16*5, gCtrStatusStr[g_CtrlBuyEffect+1]);// ��
	TextOut(hdc, nLeft2, nTop+16*6, gCtrlProtectStatusStr[g_CtrlProtectStatus+1]);	// ����

}

void ControlStatusCleanup()
{
	UninitDefault();
}

PLUGIN_INFO g_ControlStatusDraw1={
	0x00010101,
	ControlStatusInit,
	ControlStatusCleanup,
	ControlStatusDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_ControlStatusDraw2={
	0x00010201,
	ControlStatusInit,
	ControlStatusCleanup,
	ControlStatusDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_ControlStatusDraw3={
	0x00010301,
	ControlStatusInit,
	ControlStatusCleanup,
	ControlStatusDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_ControlStatusDraw4={
	0x00010401,
	ControlStatusInit,
	ControlStatusCleanup,
	ControlStatusDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_ControlStatusDraw5={
	0x00010501,
	ControlStatusInit,
	ControlStatusCleanup,
	ControlStatusDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_ControlStatusDraw6={
	0x00010601,
	ControlStatusInit,
	ControlStatusCleanup,
	ControlStatusDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_ControlStatusDraw7={
	0x00010701,
	ControlStatusInit,
	ControlStatusCleanup,
	ControlStatusDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_ControlStatusDraw8={
	0x00010801,
	ControlStatusInit,
	ControlStatusCleanup,
	ControlStatusDraw,
	NULL,
	NULL,
	NULL
};
#endif
static void MakeFormatedStringOfEnergy(int64 energy, char* str, size_t n = 30);


#if DESC("����ҳ��",1)

OOP_HENERGYS_T monActEn;
/*********************************************************************
�������ܵ�Ԫ����̬ˢ��ҳ���ʼ��ҳ�溯��
����������ҳ���ʼ�����������Ի�ȡ������Ż����ǲ������
�����������
�����������
��������ֵ����
�ڲ��������̣� 

********************************************************************/
void CommDispInit(DWORD dwPluginID)
{
	uint32 oadvalue;

	g_guiComm.nCurPoint = dwPluginID&0xff;
	g_guiComm.nCurPoint -= 1;

	oadvalue = 0x23010300 + (0x00010000) * g_guiComm.nCurPoint;
	memset(g_power + g_guiComm.nCurPoint, 0xee, sizeof(int64));
	read_db_noamal_data(g_hUdbCenter, oadvalue, 0, 0, (uint8*)&g_power[g_guiComm.nCurPoint], sizeof(int64));

    memset(g_wallet + g_guiComm.nCurPoint, 0xee, sizeof(int64));
    oadvalue = 0x23010b00 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_hUdbCenter, oadvalue, 0, 0, (uint8*)&g_wallet[g_guiComm.nCurPoint], sizeof(int64));

	memset(&monActEn, 0, sizeof(OOP_HENERGYS_T));
	oadvalue = 0x23010900 + (0x00010000) * g_guiComm.nCurPoint;
	read_db_noamal_data(g_hUdbCenter, oadvalue, 0, 0, (uint8*)&monActEn, sizeof(OOP_HENERGYS_T));
  
	get_addr(LocalAddr);

	memset(&tBuyenergyconfig, 0, sizeof(tBuyenergyconfig));
	read_db_noamal_data(g_hUdbCenter, 0x81070200, 0, 0, (uint8*)&tBuyenergyconfig, sizeof(OOP_8107_CONFIG_T));
}

/*********************************************************************
�������ܵ�Ԫ����̬ˢ��ҳ����ƺ���
���������������ն˳��Ի���
�����������
�����������
��������ֵ����
�ڲ��������̣� 

********************************************************************/
void CommDispDraw(HDC hdc)
{	
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontBig);

	char czBuf[200];
	int nLeft1 = 15;
	int nLeft2 = 100;

	int nTop = 15;
	int nTopOff = 20;	

	uint16 oiobject = 0x2301 + g_guiComm.nCurPoint;

    BOOL NoData = FALSE;
    if (tBuyenergyconfig.nNum == 0)
    {
        NoData = TRUE;
    }

    int index = -1;
    for (uint32 i = 0; i < tBuyenergyconfig.nNum; i++)
    {
        if (tBuyenergyconfig.item[i].object == oiobject)
        {
            index = i;
            break;
        }
    }

    int isfee = 0;
    if (NoData == FALSE || index >= 0)
    {
        if (tBuyenergyconfig.item[index].type == 0)
            isfee = 0;
        else
            isfee = 1;
    }
    else
        isfee = 0;

	char   oopAddr[20];	// OOP�ն˵�ַ

    sprintf(czBuf,"�ܼ��� %d", g_guiComm.nCurPoint + 1);
    TextOut(hdc,90,nTop,czBuf);

    nTop += nTopOff;
	TextOut(hdc,nLeft1,nTop,"���й�����");
	if((uint64)g_power[g_guiComm.nCurPoint] == 0xeeeeeeeeeeeeeeee)
	{
		sprintf(czBuf, "------------ kW");
	}
	else
	{
        if (fabs(g_power[g_guiComm.nCurPoint] * 0.0001) < 10000000.0)
        {
            snprintf(czBuf, 29, "%12.4f kW", g_power[g_guiComm.nCurPoint] * 0.0001);
        }
        else if (10000000.0 < fabs(g_power[g_guiComm.nCurPoint] * 0.0001) && fabs(g_power[g_guiComm.nCurPoint] * 0.0001) < 10000000000.0)
        {
            snprintf(czBuf, 29, "%12.2f MW", g_power[g_guiComm.nCurPoint] * 0.0001 / 1000.0);
        }
        else
        {
            snprintf(czBuf, 29, "%12.2f GW", g_power[g_guiComm.nCurPoint] * 0.0001 / 1000000.0);
        }
    }
	TextOut(hdc,nLeft2,nTop,czBuf);
	
	nTop += nTopOff;
    TextOut(hdc,nLeft1,nTop,"���µ���");
    //if(display_datas.penergy.nNum <= 0 || get_bit_value((uint8 *)&display_datas.penergy.rsv,32,0) == 0)
    if(monActEn.nNum <= 0)
    {
        sprintf(czBuf, "------------ kWh");
    }
    else
    {
        sprintf(czBuf, "%12.2f kWh", (double)monActEn.nValue[0] / 10000);
    }
    TextOut(hdc,nLeft2,nTop,czBuf);
    
    nTop += nTopOff;
    if (isfee == 1)
        TextOut(hdc, nLeft1, nTop, "ʣ����");
    else
        TextOut(hdc, nLeft1, nTop, "ʣ�����");
    if (NoData == TRUE || index == -1 || (uint64)g_wallet[g_guiComm.nCurPoint] == 0xeeeeeeeeeeeeeeee)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "Ԫ" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f Ԫ", (double)g_wallet[g_guiComm.nCurPoint] * 0.0001);
        }
        else
        {
            MakeFormatedStringOfEnergy((double)g_wallet[g_guiComm.nCurPoint], czBuf);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }


	nTop += nTopOff+10;
	TextOut(hdc,nLeft1,nTop,"�ն˵�ַ");
	
	memset(oopAddr, 0x00, sizeof(oopAddr));
    sprintf(oopAddr, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
	TextOut(hdc, nLeft2, nTop, oopAddr);
}


PLUGIN_INFO g_CommDispDraw1={
	0x20ffff01,/*����id-*/
	CommDispInit,/*-��ҳ����г�ʼ��-*/
	UninitDefault,
	CommDispDraw,/*-ҳ����ƺ���-*/
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_CommDispDraw2={
    0x20ffff02,/*����id-*/
    CommDispInit,/*-��ҳ����г�ʼ��-*/
    UninitDefault,
    CommDispDraw,/*-ҳ����ƺ���-*/
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_CommDispDraw3={
    0x20ffff03,/*����id-*/
    CommDispInit,/*-��ҳ����г�ʼ��-*/
    UninitDefault,
    CommDispDraw,/*-ҳ����ƺ���-*/
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_CommDispDraw4={
    0x20ffff04,/*����id-*/
		CommDispInit,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		CommDispDraw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_CommDispDraw5={
    0x20ffff05,/*����id-*/
		CommDispInit,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		CommDispDraw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_CommDispDraw6={
    0x20ffff06,/*����id-*/
		CommDispInit,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		CommDispDraw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_CommDispDraw7={
    0x20ffff07,/*����id-*/
		CommDispInit,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		CommDispDraw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_CommDispDraw8={
    0x20ffff08,/*����id-*/
		CommDispInit,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		CommDispDraw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};
#endif


#if DESC("���ܱ�ʾ��",1)
void valueDisplayProcess_strUnit(int64 energy, char * str, size_t n, uint32 weight, const char * unit);

//������Ϣ��Ż�ȡ������Ϣ
int get_meterfile(uint16 infonum, OOP_METER_T* MeterFile)
{
    int   ret;
    uint32 len = 0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad = MeterFileOad;
    NormalOad.infoNum = infonum;
    OOP_METER_T Meter;
    memset(&Meter, 0, sizeof(OOP_METER_T));
    ret = db_read_nomal(g_hUdbCenter, &NormalOad, sizeof(OOP_METER_T), (uint8*)&Meter, &len);
    if (ret != 0 || len != sizeof(OOP_METER_T))
        return -1;
    memcpy(MeterFile, &Meter, sizeof(OOP_METER_T));
    return 0;
}

void EquipVal_Init(DWORD dwPluginID)
{
    memset(&meterList,0x00,sizeof(DESK_METER_LIST_T));
    meter_info_read_all(&meterList);
	g_guiComm.nCurPoint = (dwPluginID&0xff)+1;	
	memset(&display_datas, 0, sizeof(DISPLAY_DATA_INFO));
	//read_db_record_data(g_hUdbCenter, 0x40000200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.memtTime, sizeof(OOP_DATETIME_S_T));
	read_db_record_data_time(g_hUdbCenter, 0x00100200, 0, meterList.meterInfo[g_guiComm.nCurPoint-1].pn, (uint8*)&display_datas.penergy, sizeof(OOP_ENERGY_T), &display_datas.memtTime);
    read_db_record_data(g_hUdbCenter, 0x00300200, 0, meterList.meterInfo[g_guiComm.nCurPoint-1].pn, (uint8*)&display_datas.penergys, sizeof(OOP_ENERGYS_T));
    read_db_record_data(g_hUdbCenter, 0x00400200, 0, meterList.meterInfo[g_guiComm.nCurPoint-1].pn, (uint8*)&display_datas.nenergys, sizeof(OOP_ENERGYS_T));
	read_db_record_data_main(g_hUdbCenter, 0x50060200, 0x10100200, 0, meterList.meterInfo[g_guiComm.nCurPoint-1].pn, (uint8*)&display_datas.pdemand, sizeof(OOP_DEMAND_T));

	
}

void HEquipVal_Init(DWORD dwPluginID)
{
    memset(&meterList,0x00,sizeof(DESK_METER_LIST_T));
    meter_info_read_all(&meterList);
    g_guiComm.nCurPoint = (dwPluginID&0xff)+1;
    memset(&g_high_precision_data, 0, sizeof(HIGH_PRECISION_DATA_INFO));
    read_db_record_data_time(g_hUdbCenter, 0x00100400, 0, meterList.meterInfo[g_guiComm.nCurPoint-1].pn, (uint8*)&g_high_precision_data.penergy, sizeof(OOP_HENERGY_T), &g_high_precision_data.memtTime);
    read_db_record_data(g_hUdbCenter, 0x00300400, 0, meterList.meterInfo[g_guiComm.nCurPoint-1].pn, (uint8*)&g_high_precision_data.penergys, sizeof(OOP_HENERGYS_T));
    read_db_record_data(g_hUdbCenter, 0x00400400, 0, meterList.meterInfo[g_guiComm.nCurPoint-1].pn, (uint8*)&g_high_precision_data.nenergys, sizeof(OOP_HENERGYS_T));
    read_db_record_data_main(g_hUdbCenter, 0x50060200, 0x10100200, 0, meterList.meterInfo[g_guiComm.nCurPoint-1].pn, (uint8*)&g_high_precision_data.pdemand, sizeof(OOP_DEMAND_T));

}

void EquipVal_Draw(HDC hdc)
{	
	// 2013 COMMON BUG
	SelectFont(hdc,g_guiComm.fontSmall);
	char czBuf[100];
	char czData[100];
	int mtr_id = meterList.meterInfo[g_guiComm.nCurPoint-1].pn;
	int left1 = 15;
	int left2 = 145;
		
	TextOut(hdc,left1,47,"��");
	TextOut(hdc,left1,47+16,"��");
	TextOut(hdc,left1,47+32,"��");
	TextOut(hdc,left1,47+48,"ƽ");
	TextOut(hdc,left1,47+64,"��");
	
	/*-�ֱ��-*/
	TextOut(hdc,5,15,"�ֱ��");
	if (mtr_id == -1 || meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.len <= 0 )
	{
	 	 TextOut(hdc, 46, 15,"------------");
	}
	else
	{	
		sprintf(czData, "%02x%02x%02x%02x%02x%02x", meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.add[0], meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.add[1], meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.add[2], 
		meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.add[3], meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.add[4], meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.add[5]);
		czData[12] = '\0';
    	TextOut(hdc, 46, 15, czData);
    }

	memset(czData, 0x00, sizeof(czData));
	
	/*-����ʱ��-*/
	if(mtr_id==-1 || display_datas.memtTime.year == 0 ||display_datas.memtTime.month == 0) 
	{
		TextOut(hdc,left2,15,"--/-- --:--");
	}
	else
	{
		sprintf(czBuf,"%02d/%02d %02d:%02d",display_datas.memtTime.month,display_datas.memtTime.day,display_datas.memtTime.hour,display_datas.memtTime.minute);
		TextOut(hdc,left2,15,czBuf);
	}

	/*����*/	
	TextOut(hdc,left1,31,"�����й�ʾ��");
	if(mtr_id==-1 || display_datas.penergy.nNum == 0 || get_bit_value((uint8 *)&display_datas.penergy.rsv,32,0) == 0)
	{
		sprintf(czBuf, "----------kWh");
	}
	else
	{
		//sprintf(czBuf, "%10.2fkWh", (double)display_datas.penergy.nValue[0] / 100);
        valueDisplayProcess_strUnit(display_datas.penergy.nValue[0], czBuf, 100, 2, "Wh");
	}
	TextOut(hdc,left1+14,47,czBuf);

	/*����*/
	TextOut(hdc,left2,31,"�����޹�ʾ��:");
	if(mtr_id==-1 || display_datas.penergys.nNum == 0 || get_bit_value((uint8 *)&display_datas.penergys.rsv,32,0) == 0)
	{
		sprintf(czBuf, "----------kvarh");
	}
	else
	{
		//sprintf(czBuf, "%10.2fkvarh", (double)display_datas.penergys.nValue[0] / 100);
        valueDisplayProcess_strUnit(display_datas.penergys.nValue[0], czBuf, 100, 2, "varh");
	}
	TextOut(hdc,left2-3,46,czBuf);

	/*(��)���ƽ��*/
	for(int i=0;i<4;i++)
	{
		if(mtr_id==-1 || display_datas.penergy.nNum == 0 || get_bit_value((uint8 *)&display_datas.penergy.rsv,32,i+1) == 0)
		{
			sprintf(czBuf, "----------kWh");
		}
		else
		{
			//sprintf(czBuf, "%10.2fkWh", (double)display_datas.penergy.nValue[i+1] / 100);
            valueDisplayProcess_strUnit(display_datas.penergy.nValue[i+1], czBuf, 100, 2, "Wh");
		}
		TextOut(hdc,left1+14,31+15+(i+1)*16,czBuf);
	}

	/*�����޹���*/
	if(mtr_id==-1 || display_datas.nenergys.nNum == 0 || get_bit_value((uint8 *)&display_datas.nenergys.rsv,32,0) == 0)
	{
		sprintf(czBuf, "----------kvarh");
	}
	else
	{
		//sprintf(czBuf, "%10.2fkvarh", (double)display_datas.nenergys.nValue[0] / 100);
        valueDisplayProcess_strUnit(display_datas.nenergys.nValue[0], czBuf, 100, 2, "varh");
	}
	TextOut(hdc,left2-3,62,czBuf);
	
	/*-�����������ʱ��ʾ��-*/
	TextOut(hdc,left2,78,"���������");
	if(mtr_id==-1 || get_bit_value((uint8 *)&display_datas.pdemand.rsv,32,0) == 0)
	{
		TextOut(hdc,left2,94,"----------kW");
		TextOut(hdc,left2,110,"--/--:--");
	}
	else
	{
		//sprintf(czBuf, "%10.4f kW", (double)display_datas.pdemand.demand[0].nValue / 10000);
        valueDisplayProcess_strUnit(display_datas.pdemand.demand[0].nValue, czBuf, 100, 4, "W");
		TextOut(hdc,left2,94,czBuf);
		sprintf(czBuf, "%02d/%02d %02d:%02d",
                display_datas.pdemand.demand[0].time.month,
                display_datas.pdemand.demand[0].time.day,
                display_datas.pdemand.demand[0].time.hour,
                display_datas.pdemand.demand[0].time.minute);
		TextOut(hdc,left2,110,czBuf);
	}
	
}

void HEquipVal_Draw(HDC hdc)
{	
	// 2013 COMMON BUG
	SelectFont(hdc,g_guiComm.fontSmall);
	char czBuf[100];
	char czData[100];
	int mtr_id = meterList.meterInfo[g_guiComm.nCurPoint-1].pn;
	int left1 = 15;
	int left2 = 145;
		
	TextOut(hdc,left1,47,"��");
	TextOut(hdc,left1,47+16,"��");
	TextOut(hdc,left1,47+32,"��");
	TextOut(hdc,left1,47+48,"ƽ");
	TextOut(hdc,left1,47+64,"��");
	
	/*-�ֱ��-*/
	TextOut(hdc,5,15,"�ֱ��");
	if (mtr_id == -1 || meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.len <= 0 )
	{
	 	 TextOut(hdc, 46, 15,"------------");
	}
	else
	{	
		sprintf(czData, "%02x%02x%02x%02x%02x%02x", meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.add[0], meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.add[1], meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.add[2], 
		meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.add[3], meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.add[4], meterList.meterInfo[g_guiComm.nCurPoint-1].tsa.add[5]);
		czData[12] = '\0';
    	TextOut(hdc, 46, 15, czData);
    }

	memset(czData, 0x00, sizeof(czData));
	
	/*-����ʱ��-*/
	if(mtr_id==-1 || g_high_precision_data.memtTime.year == 0 ||g_high_precision_data.memtTime.month == 0) 
	{
		TextOut(hdc,left2,15,"--/-- --:--");
	}
	else
	{
		sprintf(czBuf,"%02d/%02d %02d:%02d",g_high_precision_data.memtTime.month,g_high_precision_data.memtTime.day,g_high_precision_data.memtTime.hour,g_high_precision_data.memtTime.minute);
		TextOut(hdc,left2,15,czBuf);
	}

	/*����*/	
	TextOut(hdc,left1,31,"�����й�ʾ��");
	if(mtr_id==-1 || g_high_precision_data.penergy.nNum == 0 || get_bit_value((uint8 *)&g_high_precision_data.penergy.rsv,32,0) == 0)
	{
		sprintf(czBuf, "----------kWh");
	}
	else
	{
		//sprintf(czBuf, "%10.4fkWh", (double)g_high_precision_data.penergy.nValue[0] / 10000);
        valueDisplayProcess_strUnit(g_high_precision_data.penergy.nValue[0], czBuf, 100, 4, "Wh");
	}
	TextOut(hdc,left1+14,47,czBuf);

	/*����*/
	TextOut(hdc,left2,31,"�����޹�ʾ��:");
	if(mtr_id==-1 || g_high_precision_data.penergys.nNum == 0 || get_bit_value((uint8 *)&g_high_precision_data.penergys.rsv,32,0) == 0)
	{
		sprintf(czBuf, "----------kvarh");
	}
	else
	{
		//sprintf(czBuf, "%10.4fkvarh", (double)g_high_precision_data.penergys.nValue[0] / 10000);
        valueDisplayProcess_strUnit(g_high_precision_data.penergys.nValue[0], czBuf, 100, 4, "varh");
	}
	TextOut(hdc,left2-3,46,czBuf);

	/*(��)���ƽ��*/
	for(int i=0;i<4;i++)
	{
		if(mtr_id==-1 || g_high_precision_data.penergy.nNum == 0 || get_bit_value((uint8 *)&g_high_precision_data.penergy.rsv,32,i+1) == 0)
		{
			sprintf(czBuf, "----------kWh");
		}
		else
		{
			//sprintf(czBuf, "%10.4fkWh", (double)g_high_precision_data.penergy.nValue[i+1] / 10000);
            valueDisplayProcess_strUnit(g_high_precision_data.penergy.nValue[i+1] , czBuf, 100, 4, "Wh");
		}
		TextOut(hdc,left1+14,31+15+(i+1)*16,czBuf);
	}

	/*�����޹���*/
	if(mtr_id==-1 || g_high_precision_data.nenergys.nNum == 0 || get_bit_value((uint8 *)&g_high_precision_data.nenergys.rsv,32,0) == 0)
	{
		sprintf(czBuf, "----------kvarh");
	}
	else
	{
		//sprintf(czBuf, "%10.4fkvarh", (double)g_high_precision_data.nenergys.nValue[0] / 10000);
        valueDisplayProcess_strUnit(g_high_precision_data.nenergys.nValue[0], czBuf, 100, 4, "varh");
	}
	TextOut(hdc,left2-3,62,czBuf);
	
	/*-�����������ʱ��ʾ��-*/
	TextOut(hdc,left2,78,"���������");
	if(mtr_id==-1 || get_bit_value((uint8 *)&g_high_precision_data.pdemand.rsv,32,0) == 0)
	{
		TextOut(hdc,left2,94,"----------kW");
		TextOut(hdc,left2,110,"--/--:--");
	}
	else
	{
		//sprintf(czBuf, "%10.4f kW", (double)g_high_precision_data.pdemand.demand[0].nValue / 10000);
        valueDisplayProcess_strUnit(g_high_precision_data.pdemand.demand[0].nValue, czBuf, 100, 4, "W");
		TextOut(hdc,left2,94,czBuf);
		sprintf(czBuf, "%02d/%02d %02d:%02d",
                g_high_precision_data.pdemand.demand[0].time.month,
                g_high_precision_data.pdemand.demand[0].time.day,            
                g_high_precision_data.pdemand.demand[0].time.hour,
                g_high_precision_data.pdemand.demand[0].time.minute);
		TextOut(hdc,left2,110,czBuf);
	}
	
}
PLUGIN_INFO g_EquipValDraw1={
	0x20300000,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw2={
	0x20300001,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw3={
	0x20300002,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw4={
	0x20300003,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw5={
	0x20300004,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw6={
	0x20300005,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw7={
	0x20300006,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw8={
	0x20300007,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw9={
	0x20300008,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw10={
	0x20300009,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw11={
	0x2030000a,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw12={
	0x2030000b,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw13={
	0x2030000c,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw14={
	0x2030000d,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw15={
	0x2030000e,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_EquipValDraw16={
	0x2030000f,/*����id-*/
		EquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		EquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw1={
	0x20500000,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw2={
	0x20500001,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw3={
	0x20500002,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw4={
	0x20500003,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw5={
	0x20500004,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw6={
	0x20500005,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw7={
	0x20500006,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw8={
	0x20500007,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw9={
	0x20500008,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw10={
	0x20500009,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw11={
	0x2050000a,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw12={
	0x2050000b,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw13={
	0x2050000c,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw14={
	0x2050000d,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw15={
	0x2050000e,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

PLUGIN_INFO g_HEquipValDraw16={
	0x2050000f,/*����id-*/
		HEquipVal_Init,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		HEquipVal_Draw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

#endif


#if DESC("������Ϣ",1)

uint32 g_display_info_index;

typedef struct tag_chinaInfoExtend
{
    CHINESEINFO chinaInfo;
    uint64 addTimeValue;
}chinaInfoExtend;

void exchangeInfo(chinaInfoExtend info[],uint32 a,uint32 b)
{
    chinaInfoExtend tmpInfo;
    tmpInfo = info[a];
    info[a] = info[b];
    info[b] = tmpInfo;
}

void chineseInfoSort(chinaInfoExtend info[],uint32 infoSize)
{
    for(uint32 i=0;i<infoSize;i++)
    {
        uint32 max=i;
        for(uint32 j=i;j<infoSize;j++)
        {
            if(info[j].addTimeValue > info[max].addTimeValue)
                max=j;            
        }
        exchangeInfo(info,max,i);
    }
}

void ZhMsgInfoInit (DWORD dwPluginId)
{
	g_guiComm.nCurPoint = ((dwPluginId&0xff)/5)+1;
	g_guiComm.nIndexPoint = (dwPluginId&0xff)%5;				      // ���ڷ���
	memset(&display_datas, 0, sizeof(display_datas));
    uint32 oadvalue;
    oadvalue = 0x80030200 + (0x00010000) * (g_guiComm.nCurPoint-1);
    read_db_noamal_data(g_hUdbCenter, oadvalue, 0, 0, (uint8*)&display_datas.chinese_info, sizeof(OOP_CHINESEINFOS_T));
}
void ZhMsgInfoCleanup (void)
{
	UninitDefault();
}

/*********************************************************************
�������ܵ�Ԫ����̬ˢ��ҳ����ƺ���
�������������������Ϣ
�����������
�����������
��������ֵ����
�ڲ��������̣� 
********************************************************************/ 
void ZhMsgInfoDraw (HDC hdc) 
{
	char czBuf[300];
	char temp[30];
    g_display_info_index =g_guiComm.nIndexPoint;

    //���������ڰ���������Ϣ��ʱ��˳�����չʾ�������ǰ������˳��
    chinaInfoExtend chinainfoExt[OOP_MAX_CHINESEINFO];
    //CHINESEINFO chinInfo[OOP_MAX_CHINESEINFO];
    for(uint32 i =0;i<display_datas.chinese_info.nNum;i++)
    {
        //chinInfo[i]=display_data.chinese_info.item[i];
        chinainfoExt[i].chinaInfo = display_datas.chinese_info.item[i];
        chinainfoExt[i].addTimeValue = display_datas.chinese_info.item[i].time.year  * 10000000000
                                     + display_datas.chinese_info.item[i].time.month * 100000000
                                     + display_datas.chinese_info.item[i].time.day   * 1000000
                                     + display_datas.chinese_info.item[i].time.hour  * 10000
                                     + display_datas.chinese_info.item[i].time.minute* 100
                                     + display_datas.chinese_info.item[i].time.second* 1;
    }

    //Ȼ���chinInfo�����index��������
    chineseInfoSort(chinainfoExt,display_datas.chinese_info.nNum);
    //CHINESEINFO *pInfo = &(chinInfo[g_display_info_index]);
    CHINESEINFO *pInfo = &(chinainfoExt[g_display_info_index].chinaInfo);


	if(display_datas.chinese_info.nNum <=0 || display_datas.chinese_info.nNum < (uint32)g_guiComm.nIndexPoint+1)
	{	
		sprintf(temp,"--��--��--��--:--:--");
		TextOut(hdc, 70, 111, temp);

		SelectFont(hdc,g_guiComm.fontSmall);
		sprintf(temp, "%s", (g_guiComm.nCurPoint==1)?"һ����Ϣ":"��Ҫ��Ϣ"/*, g_guiComm.nIndexPoint+1*/);
		TextOut(hdc, 90, 20, temp);
		SelectFont(hdc,g_guiComm.fontBig);
		return;
	}
	else
	{	
//		sprintf(temp,"%02d��%02d��%02d��%02d:%02d:%02d"
//			,display_datas.chinese_info.item[g_guiComm.nIndexPoint].time.year-2000
//			,display_datas.chinese_info.item[g_guiComm.nIndexPoint].time.month
//			,display_datas.chinese_info.item[g_guiComm.nIndexPoint].time.day
//			,display_datas.chinese_info.item[g_guiComm.nIndexPoint].time.hour
//			,display_datas.chinese_info.item[g_guiComm.nIndexPoint].time.minute
//			,display_datas.chinese_info.item[g_guiComm.nIndexPoint].time.second);
        sprintf(temp,"%02d��%02d��%02d��%02d:%02d:%02d"
                  ,pInfo->time.year-2000
                  ,pInfo->time.month
                  ,pInfo->time.day
                  ,pInfo->time.hour
                  ,pInfo->time.minute
                  ,pInfo->time.second);

		TextOut(hdc, 70, 111, temp);
		
		SelectFont(hdc,g_guiComm.fontSmall);
		sprintf(temp, "%s%d", (g_guiComm.nCurPoint==1)?"һ����Ϣ":"��Ҫ��Ϣ", g_guiComm.nIndexPoint+1);
		TextOut(hdc, 90, 20, temp);
		SelectFont(hdc,g_guiComm.fontBig);
	}
	

	RECT rc;
	rc.left = 5;
	rc.top = 35;
	rc.right = 240;
	rc.bottom = 110;

	//������80������(160�ַ�)ʱ�����ĸ�ʽ
	czBuf[256] = '\0';
	
//	int nLen = display_datas.chinese_info.item[g_guiComm.nIndexPoint].info.nNum;
//	memcpy(czBuf, display_datas.chinese_info.item[g_guiComm.nIndexPoint].info.value, nLen);
    int nLen = pInfo->info.nNum;
    memcpy(czBuf, pInfo->info.value, nLen);

	czBuf[nLen] = '\0';
	if(nLen <= 112)
	{	// �����м��
		SelectFont(hdc,g_guiComm.fontBig);
	}
	else
	{	
		// �����м��
		SelectFont(hdc,g_guiComm.fontSmall);
	}
	DrawText(hdc, czBuf, -1, &rc, DT_LEFT);
    memset(czBuf,0,256);
}

extern void KeepNextLayer();
BOOL ZhMsgInfoKeyMsg(int nKey)
{
	switch(nKey)
    {
    case KEY_LEFT:
	case KEY_RIGHT:
		KeepNextLayer();
		return TRUE;
	case KEY_UP:
		g_guiComm.nIndexPoint --;
		if(g_guiComm.nIndexPoint < 0)
		{
			g_guiComm.nIndexPoint = (int)display_datas.chinese_info.nNum-1 /*4*/;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	case KEY_DOWN:
		g_guiComm.nIndexPoint ++;
		if(g_guiComm.nIndexPoint > (int)display_datas.chinese_info.nNum-1 /*4*/)
		{
			g_guiComm.nIndexPoint = 0;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	}
	return FALSE;
}

PLUGIN_INFO g_ZhMsgInfoDraw1 =
{
	0x20400000, 
	ZhMsgInfoInit, 
	ZhMsgInfoCleanup, 
	ZhMsgInfoDraw, 
	NULL, 
	ZhMsgInfoKeyMsg, 
	NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw2 =
{
	0x20400001, 
	ZhMsgInfoInit, 
	ZhMsgInfoCleanup, 
	ZhMsgInfoDraw, 
	NULL, 
	ZhMsgInfoKeyMsg, 
	NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw3 =
{
	0x20400002, 
	ZhMsgInfoInit, 
	ZhMsgInfoCleanup, 
	ZhMsgInfoDraw, 
	NULL, 
	ZhMsgInfoKeyMsg, 
	NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw4 =
{
	0x20400003, 
	ZhMsgInfoInit, 
	ZhMsgInfoCleanup, 
	ZhMsgInfoDraw, 
	NULL, 
	ZhMsgInfoKeyMsg, 
	NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw5 =
{
	0x20400004, 
	ZhMsgInfoInit, 
	ZhMsgInfoCleanup, 
	ZhMsgInfoDraw, 
	NULL, 
	ZhMsgInfoKeyMsg, 
	NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw6 =
{
	0x20400005, 
	ZhMsgInfoInit, 
	ZhMsgInfoCleanup, 
	ZhMsgInfoDraw, 
	NULL, 
	ZhMsgInfoKeyMsg, 
	NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw7 =
{
	0x20400006, 
	ZhMsgInfoInit, 
	ZhMsgInfoCleanup, 
	ZhMsgInfoDraw, 
	NULL, 
	ZhMsgInfoKeyMsg, 
	NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw8 =
{
	0x20400007, 
	ZhMsgInfoInit, 
	ZhMsgInfoCleanup, 
	ZhMsgInfoDraw, 
	NULL, 
	ZhMsgInfoKeyMsg, 
	NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw9 =
{
	0x20400008, 
	ZhMsgInfoInit, 
	ZhMsgInfoCleanup, 
	ZhMsgInfoDraw, 
	NULL, 
	ZhMsgInfoKeyMsg, 
	NULL 
};
PLUGIN_INFO g_ZhMsgInfoDraw10 =
{
	0x20400009, 
	ZhMsgInfoInit, 
	ZhMsgInfoCleanup, 
	ZhMsgInfoDraw, 
	NULL, 
	ZhMsgInfoKeyMsg, 
	NULL 
};
#endif


#if DESC("������Ϣ",1)
#include <math.h>
//�Ӻ���: MakeFormatedStringOfEnergy()
static
void MakeFormatedStringOfEnergy(int64 energy, char* str, size_t n)
{
    double val_kWh;
    double val_MWh;

    if (str == NULL || n <= 0)
    {
        return;
    }

    val_kWh = energy * 0.0001;//�����ֵ���Լ���ڲ�������10000���洢������
    if (fabs(val_kWh) < 10000000.0)
    {
        snprintf(str, n - 1, "%12.4f kWh", val_kWh);
    }
    else
    {
        val_MWh = val_kWh / 1000.0;
        snprintf(str, n - 1, "%12.2f MWh", val_MWh);
    }
    str[n - 1] = '\0';
}

void BuyDataInit(DWORD dwPluginID)
{
	g_guiComm.nCurPoint = (dwPluginID>>8)&0xff;
	g_guiComm.nCurPoint -= 1;

    memset(&tBuyenergyconfig, 0, sizeof(tBuyenergyconfig));
    read_db_noamal_data(g_hUdbCenter, 0x81070200, 0, 0, (uint8*)&tBuyenergyconfig, sizeof(OOP_8107_CONFIG_T));
    uint32 oadvalue;

    memset(g_wallet + g_guiComm.nCurPoint, 0xff, sizeof(int64));
    oadvalue = 0x23010b00 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_hUdbCenter, oadvalue, 0, 0, (uint8*)&g_wallet[g_guiComm.nCurPoint], sizeof(int64));

    g_bGuiBuyLastData = FALSE;
    if (0 == read_db_noamal_data(g_hUdbCenter, 0x810703ff, 0, g_guiComm.nCurPoint + 1, (uint8*)&g_GuiBuyLast, sizeof(int64)))
    {
        g_bGuiBuyLastData = TRUE;
    }
    g_bGuiBuyEndData = FALSE;
    if (0 == read_db_noamal_data(g_hUdbCenter, 0x810704ff, 0, g_guiComm.nCurPoint + 1, (uint8*)&g_GuiBuyEnd, sizeof(int64)))
    {
        g_bGuiBuyEndData = TRUE;
    }
    g_bGuiBuyLastType = 0;
    if (0 == read_db_noamal_data(g_hUdbCenter, 0x810705ff, 0, g_guiComm.nCurPoint + 1, &g_bGuiBuyLastType, sizeof(uint8)))
    {
        g_bGuiBuyLastData = TRUE;
    }
}
void BuyDataCleanup (void)
{
	UninitDefault();
}
/*******************************************************************
�������ܵ�Ԫ����̬ˢ��ҳ����ƺ���
�������������ƹ�����Ϣ
�����������
�����������
��������ֵ����
�ڲ��������̣� 
�����ɳ�ʼ���溯����ÿ������g_guiComm.nCurPoint,����4��
����������
********************************************************************/
void BuyDataDraw(HDC hdc)
{
	SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[100];
    int nLeft1 = 15;
    int nLeft2 = 96;
    int nTop = 18;
    int nTempTop = 17;
    uint16 oiobject = 0x2301 + g_guiComm.nCurPoint;

    BOOL NoData = FALSE;
    if (tBuyenergyconfig.nNum == 0)
    {
        NoData = TRUE;
    }
    int index = -1;
    for (uint32 i = 0; i < tBuyenergyconfig.nNum; i++)
    {
        if (tBuyenergyconfig.item[i].object == oiobject)
        {
            index = i;
            break;
        }
    }

    int isfee = 0;
    if (NoData == FALSE || index >= 0)
    {
        if (tBuyenergyconfig.item[index].type == 0)
            isfee = 0;
        else
            isfee = 1;
    }
    else
        isfee = 0;

    TextOut(hdc, nLeft1, nTop, "���絥��");

    if (NoData == TRUE || index == -1)
    {
        TextOut(hdc, nLeft2, nTop, "--------");
    }
    else
    {
        sprintf(czBuf, "%08d", tBuyenergyconfig.item[index].number);
        TextOut(hdc, nLeft2+50, nTop, czBuf);
    }


    nTop += nTempTop;

    TextOut(hdc, nLeft1, nTop, g_bGuiBuyLastType == 1 ? "��ǰ���" : "��ǰ����");
	if(isfee==1)
	{
		TextOut(hdc, nLeft1, nTop+nTempTop, "������");
	}
	else
	{
		TextOut(hdc, nLeft1, nTop+nTempTop, "�������");
	}
    if (NoData == TRUE || index == -1 || FALSE == g_bGuiBuyLastData)
    {
        sprintf(czBuf, "------------ %s", (g_bGuiBuyLastType == 1) ? "Ԫ" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (g_bGuiBuyLastType == 1)
        {
            sprintf(czBuf, "%12.4f Ԫ", g_GuiBuyLast * 0.0001);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            MakeFormatedStringOfEnergy(g_GuiBuyLast, czBuf);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
    }

    nTop += nTempTop;
    if (NoData == TRUE || index == -1 || FALSE == g_bGuiBuyEndData)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "Ԫ" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f Ԫ", g_GuiBuyEnd * 0.0001);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            MakeFormatedStringOfEnergy(g_GuiBuyEnd, czBuf);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
    }

    nTop += nTempTop;
    TextOut(hdc, nLeft1, nTop, "��������");

    if (NoData == TRUE || index == -1)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "Ԫ" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f Ԫ", tBuyenergyconfig.item[index].alarmLimit * 0.0001);
        }
        else
        {
            MakeFormatedStringOfEnergy(tBuyenergyconfig.item[index].alarmLimit, czBuf);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += nTempTop;
    TextOut(hdc, nLeft1, nTop, "��բ����");

    if (NoData == TRUE || index == -1)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "Ԫ" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f Ԫ", tBuyenergyconfig.item[index].tripLimit * 0.0001);
        }
        else
        {
            MakeFormatedStringOfEnergy(tBuyenergyconfig.item[index].tripLimit, czBuf);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }


    nTop += nTempTop;
    if (isfee == 1)
        TextOut(hdc, nLeft1, nTop, "ʣ����");
    else
        TextOut(hdc, nLeft1, nTop, "ʣ�����");
    if (NoData == TRUE || index == -1 || g_wallet[g_guiComm.nCurPoint] == 0xffffffff)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "Ԫ" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f Ԫ", (double)g_wallet[g_guiComm.nCurPoint] * 0.0001);
        }
        else
        {
            MakeFormatedStringOfEnergy((double)g_wallet[g_guiComm.nCurPoint], czBuf);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
}

PLUGIN_INFO g_BuyDataDraw1={
	0x00010100,
	BuyDataInit,
	BuyDataCleanup,
	BuyDataDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_BuyDataDraw2={
	0x00010200,
	BuyDataInit,
	BuyDataCleanup,
	BuyDataDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_BuyDataDraw3={
	0x00010300,
	BuyDataInit,
	BuyDataCleanup,
	BuyDataDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_BuyDataDraw4={
	0x00010400,
	BuyDataInit,
	BuyDataCleanup,
	BuyDataDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_BuyDataDraw5={
	0x00010500,
	BuyDataInit,
	BuyDataCleanup,
	BuyDataDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_BuyDataDraw6={
	0x00010600,
	BuyDataInit,
	BuyDataCleanup,
	BuyDataDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_BuyDataDraw7={
	0x00010700,
	BuyDataInit,
	BuyDataCleanup,
	BuyDataDraw,
	NULL,
	NULL,
	NULL
};
PLUGIN_INFO g_BuyDataDraw8={
	0x00010800,
	BuyDataInit,
	BuyDataCleanup,
	BuyDataDraw,
	NULL,
	NULL,
	NULL
};

#endif


#if DESC("����Ӧ��",1)

int g_ac_data_display_drawpage_index;

void restartAPPInit(DWORD dwPluginId)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    g_ac_data_display_drawpage_index = 0;
    pg_guicomm->bCanTrunDisp = FALSE;
	g_bpop = TRUE;
}
void restartAPPDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "����Ӧ��";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if (g_ac_data_display_drawpage_index == 0)
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
    if (g_ac_data_display_drawpage_index == 1)
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

BOOL restartAPPKeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 0:
            CancelMenu();
            return TRUE;
        case 1:
        if(ValidatePwd() == TRUE)
        {
            if (FALSE == MessageBox("��ʾ\n�Ƿ�����Ӧ��\n����ȷ��һ��", MB_OKCANCEL))
            {
                return TRUE;
            }
            g_ac_data_display_drawpage_index = 0;
            PopMsg("    ��������Ӧ��...");
            //                appGUI_send_req_handle(MESSAGE_IID_DESKGUI, DESKGUI_IOP_APP_UNREG, NULL, 0);
            exit(0);
        }
        break;
        }
        return TRUE;
    }
    return FALSE;
}
//����Ӧ��
PLUGIN_INFO g_layer_restartAPP =
{
    0x11040002/*id*/,
    restartAPPInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    restartAPPDraw/*Draw*/,
    NULL/*Timer*/,
    restartAPPKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("�ֶ��������",1)
//CRC����
const uint16 fcstab[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
#define PPPINITFCS16 0xffff /* Initial FCS value */
#define PPPGOODFCS16 0xf0b8 /* Good final FCS value */

uint16 app_crc16_getex(uint16 CRC16_data,const uint8* buf, uint32 len)
{
    const unsigned char *cp=buf;
    while (len--)
        CRC16_data = (CRC16_data >> 8) ^ fcstab[(CRC16_data ^ *cp++) & 0xff];
    return (CRC16_data);
}
uint16 app_crc16_get(const uint8* buf, uint32 len)
{
    uint16 trialfcs;
    /* add on output */
    trialfcs = app_crc16_getex( PPPINITFCS16, buf, len );
    trialfcs ^= 0xffff; /* complement */
    return trialfcs;
}

struct manual_amr_result_struc
{
    uint8   protocol;   //��Լ����
    uint8   piid;       //piid
    char    state[16];  //���ؽ��״̬
    char    time[16];   //���ؽ��ʱ��
    char    data[16];   //���ؽ������(�����й�)
};
int8    g_manual_amr_index;
uint16  g_manual_amr_timeout;
uint8   g_manual_amr_piid = 1;
struct manual_amr_result_struc g_manual_amr_rst;


void manualAmrTimeOutReturn(char* pNewStr, void* pPara)
{
    int value = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (value > 65535 && TRUE == MessageBox("��ʱʱ�����65535\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {           
        sprintf(tmp, "%d", g_manual_amr_timeout);
        InputBox(tmp, manualAmrTimeOutReturn, NULL, 0x01);
    }
    else
    {
        g_manual_amr_timeout = value; 
    }
}

void manualAmrMakeFrame(uint8 *apdu, uint8 *len, OOP_METER_T oopMeter)
{
    uint8   index = 0;
    uint16  crc;
    
    apdu[index++] = 0x09;   //����
    apdu[index++] = 0x07;   //͸��ת��
    apdu[index++] = g_manual_amr_piid++;   //PIID
    //�˿�OAD
    apdu[index++] = (oopMeter.basic.port.value >> 24) & 0xFF;
    apdu[index++] = (oopMeter.basic.port.value >> 16) & 0xFF;
    apdu[index++] = (oopMeter.basic.port.value >> 8) & 0xFF;
    apdu[index++] = oopMeter.basic.port.value & 0xFF;

    apdu[index++] = oopMeter.basic.baud;    //������
    apdu[index++] = 0x02;   //żУ��
    apdu[index++] = 0x08;   //����λ
    apdu[index++] = 0x01;   //ֹͣλ
    apdu[index++] = 0x00;   //����
    //���ĳ�ʱʱ��
    apdu[index++] = g_manual_amr_timeout >> 8;
    apdu[index++] = g_manual_amr_timeout & 0xFF;
    //�ֽڳ�ʱʱ��(10ms)
    apdu[index++] = 0x00;
    apdu[index++] = 0x0A;
    if (oopMeter.basic.protocol == 3)   //698Э��
    {
        apdu[index++] =  0x19; //͸�����ĳ��� 
        apdu[index++] = 0x68;   //��ʼ��
        //����
        apdu[index++] = 0x17;
        apdu[index++] = 0x00;
        
        apdu[index++] = 0x43;   //������
        apdu[index++] = 0x05;   //SA��־
        //SA��ַ
        apdu[index++] = oopMeter.basic.tsa.add[5];
        apdu[index++] = oopMeter.basic.tsa.add[4];
        apdu[index++] = oopMeter.basic.tsa.add[3];
        apdu[index++] = oopMeter.basic.tsa.add[2];
        apdu[index++] = oopMeter.basic.tsa.add[1];
        apdu[index++] = oopMeter.basic.tsa.add[0];

        apdu[index++] = 0x00;   //CA��ַ

        crc = app_crc16_get(&apdu[index - 11], 11);
        apdu[index++] = crc & 0xFF;
        apdu[index++] = crc >> 8;

        apdu[index++] = 0x05;   //��ȡ����
        apdu[index++] = 0x01;   //��ȡһ����������
        apdu[index++] = 0x05;   //PIID
        //OAD
        apdu[index++] = 0x00;
        apdu[index++] = 0x10;
        apdu[index++] = 0x02;
        apdu[index++] = 0x00;

        apdu[index++] = 0x00;   //timeTag

        crc = app_crc16_get(&apdu[index - 21], 21);
        apdu[index++] = crc & 0xFF;
        apdu[index++] = crc >> 8;

        apdu[index++] = 0x16;   //�����ַ�

    }   
    else
    {
        uint8   checkSum = 0; //У���
        int     i;
        
        apdu[index++] = 0x13; //͸�����ĳ���
        apdu[index++] = 0xFE;
        apdu[index++] = 0xFE;
        apdu[index++] = 0xFE;
        apdu[index++] = 0x68; //��ʼ��
        //SA��ַ
        apdu[index++] = oopMeter.basic.tsa.add[5];
        apdu[index++] = oopMeter.basic.tsa.add[4];
        apdu[index++] = oopMeter.basic.tsa.add[3];
        apdu[index++] = oopMeter.basic.tsa.add[2];
        apdu[index++] = oopMeter.basic.tsa.add[1];
        apdu[index++] = oopMeter.basic.tsa.add[0];
        apdu[index++] = 0x68; //��ʼ��
        apdu[index++] = 0x11; //������
        apdu[index++] = 0x04; //����
        apdu[index++] = 0x00 + 0x33;
        apdu[index++] = 0x00 + 0x33;
        apdu[index++] = 0x01 + 0x33;
        apdu[index++] = 0x00 + 0x33;

        for (i = 0; i < index; i++)
        {
            checkSum += apdu[index - 14 + i];
        }
        apdu[index++] = checkSum;
        apdu[index++] = 0x16;   //�����ַ�
    }
    apdu[index++] = 0x00;   //timeTag

    *len = index;
}

void manual_amr_init(DWORD dwPluginId)
{
    g_manual_amr_index = 0;
    g_guiComm.nCurPoint = 1;
    g_manual_amr_timeout = 50;
    memset(&g_manual_amr_rst, 0, sizeof(g_manual_amr_rst));
}

void manual_amr_draw(HDC hdc)
{
    //int flag = 0;  //��ʶ�����Ƿ����
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 85;
    int nLeft3 = 50;
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    //����Ƴ������ȴ�����
    //g_guiMain.nLightonTime = 0;
    //g_guiMain.bLighton = TRUE;

    set_gui_infonum((uint16)g_guiComm.nCurPoint);

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);

    TextOut(hdc, nLeft1, nTop, "�������");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    if (g_manual_amr_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�����ַ");
    if (MeterFile.nIndex != g_guiComm.nCurPoint)
	{
		app_get_meter_list(g_guiComm.nCurPoint, &MeterFile);
	}
	
    if (MeterFile.basic.tsa.len != 0)
    {
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", MeterFile.basic.tsa.add[0], MeterFile.basic.tsa.add[1],
                MeterFile.basic.tsa.add[2], MeterFile.basic.tsa.add[3], MeterFile.basic.tsa.add[4], MeterFile.basic.tsa.add[5]);
    }
    else
    {
        sprintf(czBuf, "--");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��ʱʱ��(s)");
    sprintf(czBuf, "%d", g_manual_amr_timeout);
    if (g_manual_amr_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
  
    
    nTop += 20;
    if (g_manual_amr_index == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "ȷ������");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "ȷ������");
    }

    nTop += 12;
    TextOut(hdc, 0, nTop, "---------------------------------------------");
    
    nTop += 12;
    TextOut(hdc, nLeft1, nTop, "״̬");
    if (g_manual_amr_index == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2 - 10, nTop, g_manual_amr_rst.state);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 - 10, nTop, g_manual_amr_rst.state);
    }
    
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����ʱ��");
    TextOut(hdc, nLeft2 - 10, nTop, g_manual_amr_rst.time);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�������й�");
    TextOut(hdc, nLeft2 - 10, nTop, g_manual_amr_rst.data);
    
}

BOOL manual_amr_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    { 
        case KEY_UP:
        {
            g_manual_amr_index--;
            if (g_manual_amr_index < 0)
                g_manual_amr_index = 2;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        {
            g_manual_amr_index++;
            if (g_manual_amr_index > 2)
                g_manual_amr_index = 0;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_LEFT:
        {
            if (g_manual_amr_index == 0)
            {
                g_guiComm.nCurPoint--;
                if (g_guiComm.nCurPoint < 0)
                {
                    g_guiComm.nCurPoint = MAX_SEL_POINT;
                }
                memset(&g_manual_amr_rst,0,sizeof(manual_amr_result_struc));
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            }
        }
        break;
        case KEY_RIGHT:
        {
            if (g_manual_amr_index == 0)
            {
                g_guiComm.nCurPoint++;
                if (g_guiComm.nCurPoint > MAX_SEL_POINT)
                {
                    g_guiComm.nCurPoint = 0;
                }
                memset(&g_manual_amr_rst,0,sizeof(manual_amr_result_struc));
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            }
        }
        break;
        case KEY_OK:
        {
            if (g_manual_amr_index == 0)
            {
                char bzbuf[50];
                sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
                InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            }

            if (g_manual_amr_index == 1)
            {
                char bzbuf[50];
                sprintf(bzbuf, "%d", g_manual_amr_timeout);
                InputBox(bzbuf, manualAmrTimeOutReturn, NULL, 0x01);
            }
            
            if (g_manual_amr_index == 2)
            {
//                OOP_METER_T        oopMeter;
                uint8   apdu[100] = {0};
                uint8   len = 0;
                
//                memset(&oopMeter, 0, sizeof(OOP_METER_T));
				if (MeterFile.nIndex != g_guiComm.nCurPoint)
				{
					app_get_meter_list(g_guiComm.nCurPoint, &MeterFile);
				}
				
			    if (MeterFile.basic.tsa.len == 0)
                {
                    GUI_FMT_DEBUG(" app_get_meter_list %d !\n", g_guiComm.nCurPoint);
                    MessageBox("��������ȷ�Ĳ������!", MB_OK);
                    return FALSE;
                }

                g_manual_amr_index++;
                memset(&g_manual_amr_rst, 0, sizeof(g_manual_amr_rst));
                sprintf(g_manual_amr_rst.state, "������...");
                g_manual_amr_rst.piid = g_manual_amr_piid;
                g_manual_amr_rst.protocol = MeterFile.basic.protocol;
                manualAmrMakeFrame(apdu, &len, MeterFile);
                gui_mannual_amr_send(apdu, len);
            }

            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    return FALSE;
}


//�ֶ��������
PLUGIN_INFO g_layer_manual_amr =
{
    0x11040006/*id*/,
    manual_amr_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    manual_amr_draw/*Draw*/,
    NULL/*Timer*/,
    manual_amr_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("RS485�˿����ݹ���", 1)
uint8 g_Rs485_status = 0;
uint8 g_Rs485_statusOld = 0;
uint8 g_Rs485Index = 0;

//��Ӫ��
const char* g_Rs485StatusName[] =
{
    "ͣ��",
    "����"
};

void rs485Status_set_init(HDC hdc)
{
    g_Rs485_status = 0;
    read_db_noamal_data(g_hUdbCenter, 0x6E030300, 0, 0, &g_Rs485_status, sizeof(uint8));
    if(g_Rs485_status > 1)
    {
        g_Rs485_status = 0;
    }
    g_Rs485Index = 0;
    g_Rs485_statusOld = g_Rs485_status;
}

void rs485Status_set_draw(HDC hdc)
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
    TextOut(hdc, 4, nTop, "RS485-2�˿����ݹ���");
    nTop += 20;
    TextOut(hdc, 48, nTop, "��������");

    SelectFont(hdc, pg_guicomm->fontSmall);
    nTop += 32;
    TextOut(hdc, nLeft1, nTop, "����: ");
    if(0 == g_Rs485Index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_Rs485StatusName[g_Rs485_status]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_Rs485StatusName[g_Rs485_status]);
    }

    SelectFont(hdc, pg_guicomm->fontBig);
    nTop += 32;
    if(1 == g_Rs485Index)
    {
        ITEM_SELECT;
        TextOut(hdc, 30, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 30, nTop, "����");
    }

    if(2 == g_Rs485Index)
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

BOOL rs485Status_set_keymsg(int nKey)
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
            g_Rs485_status = nSel;
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
            if (g_Rs485Index > 0)
            {
                g_Rs485Index--;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_Rs485Index < 2)
            {
                g_Rs485Index++;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if(1 == g_Rs485Index)
            {
                CancelMenu();
                return TRUE;
            }
            else if(TRUE == ValidatePwd())
            {
                if(0 == g_Rs485Index)
                {
                    PopList(g_Rs485StatusName, 2, g_Rs485StatusName[g_Rs485_status], 80, 80, FALSE);
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                    return TRUE;
                }
                else if(2 == g_Rs485Index)
                {
                    if(g_Rs485_status != g_Rs485_statusOld)
                    {
                        int ret = 0;
                        OOP_OAD_U statusOad;
                        statusOad.value = 0x6E030300;
                        ret = write_normal_data(&g_Rs485_status, 1, statusOad, 0, 0, CLASS_DATA_INIT);
                        if (ret != 0)
                        {
                            MessageBox("485���ݹ���д��ʧ�ܣ�");
                        }
                        else
                        {
                            MessageBox("���óɹ���");
                        }
                    }
                    CancelMenu();
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

PLUGIN_INFO g_layer_rs485Status_set = {
    0x11030085/*id*/,
    rs485Status_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    rs485Status_set_draw/*Draw*/,
    NULL/*Timer*/,
    rs485Status_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("�ն���Ϣ",1)

typedef struct{
    uint8   factory[4];  /**< ���̴���           */
    uint8   softVer[4];  /**< ����汾��         */
    uint8   softDate[6]; /**< ����汾����       */
    uint8   hardVer[4];  /**< Ӳ���汾��         */
    uint8   hardDate[6]; /**< Ӳ���汾����       */
    uint8   extend[8];   /**< ������չ��Ϣ       */
}__attribute__ ((packed)) Factory_Version_t;

Factory_Version_t g_tGuiVer;
char gOSVer[64] = { 0 };
char gSysTime[64] = { 0 };
int termIndex;
int positionFlag;

int cmd_execute(const char* cmd,  char* result)
{
    char resultBuf[500];

    FILE* fp = NULL;

    if ((fp = popen(cmd, "r")) != NULL)
    {
        while( fgets(resultBuf, sizeof(resultBuf), fp) != NULL) 
        {
            strcat(result, resultBuf);
            if( strlen(result) > sizeof(resultBuf))
            {
                break;
            }
        }
        pclose(fp);
        fp = NULL;
    }
    else
    {
        return -1;
    }
    return 0;
}


void TermDataInit(DWORD dwPluginID)
{
    //uint32      len = 0;
    //NOMAL_OAD_T normal;
    
	get_addr(LocalAddr);
	char result[200]={0};
	char cmd[200]={0};
    uint8 sendbuf1[10]={0};
    int ret=0;
    

	memset(&g_tGuiVer, 0, sizeof(Factory_Version_t));
    read_pdata_xram("/data/app/stAmr/cfg/version", (char*)&g_tGuiVer, 0, sizeof(Factory_Version_t));

	sprintf(cmd,"cat /etc/version/version");
    cmd_execute(cmd, result);
    strncpy(gOSVer,strim(result), MIN(strlen(result)-3, 24));

	sprintf(cmd,"uname -v |awk -F 'SMP' '{print $2}' |awk '{print $1,$2,$3,$4,$6}'");
    memset(result,0x00,sizeof(result));
    cmd_execute(cmd,result);
    strcpy(gSysTime,strim(result));
    
    memset(ClientAddr, 0, sizeof(ClientAddr));
    get_clientno(ClientAddr);
    termIndex = 0;
    //����λ����Ϣ
    memset(&GpsPosition, 0, sizeof(OOP_GEOGADD_A_T));
    //positionFlag = read_db_noamal_data(g_hUdbCenter, 0x40040200, 0, 0, (uint8*)&GpsPosition, sizeof(GpsPosition));
    sendbuf1[0]=1;
    getpositionflag= -1;
    ret = gui_send_wirelessmsg(0, WIRELESS_IOP_GET_GPS, sendbuf1, 1);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG("��ѯ����λ����Ϣ!\n");
    }
}

void TermDataDraw1(HDC hdc)
{
	ITEM_NOT_SELECT;
	//SelectFont(hdc,g_guiComm.fontSmall);
	char czBuf[150];
	char czCode[150];
	int nLeft1 = 15;
	int nLeft2 = 100;
	int nTop = 20;
	int nAdd = 18;
	char    oopAddr[20];	// OOP�ն˵�ַ
	// �ն˵�ַ
    TextOut(hdc, nLeft1 + 40, nTop, "�ն˻�����Ϣ");
	nTop += nAdd;
	TextOut(hdc, nLeft1, nTop, "OOP�ն˵�ַ");
#ifdef AREA_ZHEJIANG
	sprintf(oopAddr, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
#else
	sprintf(oopAddr, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
#endif
	TextOut(hdc, nLeft2, nTop, oopAddr);

	// �ն˱��
	nTop += nAdd;
	memset(czBuf, 0x0, sizeof(czBuf));
	TextOut(hdc, nLeft1, nTop, "�ն˱��");
    sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", ClientAddr[0], ClientAddr[1], ClientAddr[2], ClientAddr[3], ClientAddr[4], 
                                               ClientAddr[5]);
	TextOut(hdc,nLeft2, nTop, czBuf);

	// ����汾
	nTop += nAdd;
	TextOut(hdc, nLeft1, nTop, "����汾");
	memset(czCode, 0, 150);
    memcpy(czCode, g_tGuiVer.softVer, 4);
	czCode[4] = ' ';
    memcpy(czCode + 5, g_tGuiVer.softDate, 6);
	TextOut(hdc,nLeft2, nTop, czCode);

    // Ӳ���汾�ż�����ʱ��
    nTop += nAdd;
    TextOut(hdc, nLeft1, nTop, "Ӳ���汾");
    memset(czCode, 0, 150);
    memcpy(czCode, g_tGuiVer.hardVer, 4);
    czCode[4] = ' ';
    memcpy(czCode + 5, g_tGuiVer.hardDate, 6);
    TextOut(hdc,nLeft2, nTop, czCode);

    nTop += nAdd;
    TextOut(hdc, nLeft1, nTop, "������չ��Ϣ");
    memset(czCode, 0, 150);
    memcpy(czCode, g_tGuiVer.extend, 8);
    TextOut(hdc,nLeft2, nTop, czCode);
}

void TermDataDraw2(HDC hdc)
{
	ITEM_NOT_SELECT;
	//SelectFont(hdc,g_guiComm.fontSmall);
	int nLeft1 = 15;
	int nLeft2 = 80;
	int nLeft3 = 140;
	int nTop = 20;
	int nAdd = 20;
    TextOut(hdc, nLeft1 + 40, nTop, "�ն˻�����Ϣ");
	// OS�汾
	nTop += nAdd;
	TextOut(hdc, nLeft1, nTop, "OS�汾");
	TextOut(hdc, nLeft2, nTop, gOSVer);
	
	// ����ϵͳ��������
	nTop += nAdd;
	TextOut(hdc, nLeft1, nTop, "��������");
	TextOut(hdc, nLeft2, nTop, gSysTime);

	// ͨ������
	nTop += nAdd;
	TextOut(hdc, nLeft1, nTop, "ͨ������");
    //240703 msk��̨�������ʸ�Ϊ4800
	TextOut(hdc,nLeft2, nTop, "4800");	
	TextOut(hdc,nLeft3, nTop, "bps");
	
	// ������ʱ
	nTop += nAdd;
	TextOut(hdc, nLeft1, nTop, "������ʱ");
	TextOut(hdc,nLeft2, nTop, "100");	
	TextOut(hdc,nLeft3, nTop, "ms");

}

void TermDataDraw3(HDC hdc)
{
    ITEM_NOT_SELECT;
    //SelectFont(hdc,g_guiComm.fontSmall);
    int nLeft1 = 15;
    int nTop = 20;
    int nAdd = 20;
    char czBuf[150];
    //�ն˵���λ����Ϣ
    TextOut(hdc, nLeft1 + 40, nTop, "�ն˵���λ����Ϣ");
    if ( getpositionflag < 0){
        //����
        nTop += nAdd;
        TextOut(hdc, nLeft1, nTop, "����: - ---��--��--.--��");
     
        //γ��
        nTop += nAdd;
        TextOut(hdc, nLeft1, nTop, "γ��: - ---��--��--.--��");
     
         //����
        nTop += nAdd;
        TextOut(hdc, nLeft1, nTop, "����:       ----.-��");
    }
    else
    {
        if (GpsPosition.type == 0){
            //����
            nTop += nAdd;
            memset(czBuf, 0x0, sizeof(czBuf));
            sprintf(czBuf, "%s %s %d��%d��%d.%d��", "����:", longitude[GpsPosition.oldaddr.longitude.position], GpsPosition.oldaddr.longitude.degre,
                                                 GpsPosition.oldaddr.longitude.branch,  GpsPosition.oldaddr.longitude.second, 0);
            TextOut(hdc, nLeft1, nTop, czBuf);
          
            //γ��
            nTop += nAdd;
            memset(czBuf, 0x0, sizeof(czBuf));
            sprintf(czBuf, "%s %s %d��%d��%d.%d��", "γ��:", latitude[GpsPosition.oldaddr.latitude.position], GpsPosition.oldaddr.latitude.degre, 
                                                 GpsPosition.oldaddr.latitude.branch, GpsPosition.oldaddr.latitude.second, 0);
            TextOut(hdc, nLeft1, nTop, czBuf);
          
             //����
            nTop += nAdd; 
            memset(czBuf, 0x0, sizeof(czBuf));
            sprintf(czBuf, "%s %d.%d��", "����:", GpsPosition.oldaddr.height/100, GpsPosition.oldaddr.height%100);
            TextOut(hdc, nLeft1, nTop, czBuf);
        }else{
            //����
            nTop += nAdd;
            memset(czBuf, 0x0, sizeof(czBuf));
            int second = 0;
            int branch =(int)GpsPosition.newaddr.longitude.branch;
            float temp = GpsPosition.newaddr.longitude.branch - branch;
            second = temp * 6000;   //����60 ��תΪ�룬����100������
            sprintf(czBuf, "%s %s %d��%d��%d.%d��", "����:", longitude[GpsPosition.newaddr.longitude.position], GpsPosition.newaddr.longitude.degre, 
                                                 branch, second, 0);
            TextOut(hdc, nLeft1, nTop, czBuf);
          
            //γ��
            branch =(int)GpsPosition.newaddr.latitude.branch;
            temp = GpsPosition.newaddr.latitude.branch - branch;
            second = temp * 6000;   //����60 ��תΪ�룬����100������
            nTop += nAdd;
            memset(czBuf, 0x0, sizeof(czBuf));
            sprintf(czBuf, "%s %s %d��%d��%d.%d��", "γ��:", latitude[GpsPosition.newaddr.latitude.position], GpsPosition.newaddr.latitude.degre,
                                                 branch, second, 0);
            TextOut(hdc, nLeft1, nTop, czBuf);
          
             //����
            nTop += nAdd; 
            memset(czBuf, 0x0, sizeof(czBuf));
            sprintf(czBuf, "%s %d.%d��", "����:", GpsPosition.newaddr.height/100, GpsPosition.newaddr.height%100);
            TextOut(hdc, nLeft1, nTop, czBuf);
        }
    }
}

void TermDataDraw(HDC hdc)
{
    if(termIndex == 0)//��һҳ
    {
        return TermDataDraw1(hdc);
    }
    else if(termIndex == 1)    //�ڶ�ҳ
    {
        return TermDataDraw2(hdc);
    }
    else
    {
        return TermDataDraw3(hdc);
    }
}

BOOL TermDatakeymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
        termIndex--;
        if (termIndex < 0)
        {
            termIndex = 2;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
    case KEY_DOWN:
        termIndex++;
        if (termIndex >= 3)
        {
            termIndex = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    return FALSE;
}

PLUGIN_INFO g_TermDataDraw={
	0x20600000,/*����id-*/
		TermDataInit,/*-��ҳ����г�ʼ��-*/
		UninitDefault,
		TermDataDraw,/*-ҳ����ƺ���-*/
		NULL,
        TermDatakeymsg/*keymsg*/,
		NULL
};

void ModemVersionInit(DWORD dwPluginID)
{
    //read_db_noamal_data(g_hUdbCenter, 0x45001D00, 0, 0, (uint8*)&netmode, sizeof(netmode));
    getmodemversionflag=0;
    modem_version_get_for_radio();
	
}

void ModemVersionDraw(HDC hdc)
{
	ITEM_NOT_SELECT;
	SelectFont(hdc,g_guiComm.fontSmall);
	char czBuf[150];
	int nLeft1 = 15;
	int nLeft2 = 120;
	int nTop = 25;
	int nAdd = 15;

    sleep(5);
	nTop += nAdd;
	memset(czBuf, 0x0, sizeof(czBuf));
	if(getmodemversionflag)
	{
    	sprintf(czBuf, "%s", modemversion);
	}
	else
	{
		sprintf(czBuf, "%s", "--------");
	}
    
	TextOut(hdc, nLeft1, nTop, "��̨è�汾:");
	TextOut(hdc,nLeft2, nTop, czBuf);

}

PLUGIN_INFO g_ModemversionDraw={
	0x20611111,/*����id-*/
		ModemVersionInit,/*-��ҳ����г�ʼ��-*/
		NULL,
		ModemVersionDraw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

void NetmodeInit(DWORD dwPluginID)
{
    //read_db_noamal_data(g_hUdbCenter, 0x45001D00, 0, 0, (uint8*)&netmode, sizeof(netmode));
    getnetmodeflag=0;
    look_up_at_simsel();
	
}

void NetmodeDraw(HDC hdc)
{
	ITEM_NOT_SELECT;
	SelectFont(hdc,g_guiComm.fontSmall);
	char czBuf[150];
	int nLeft1 = 15;
	int nLeft2 = 120;
	int nTop = 25;
	int nAdd = 15;

    sleep(5);
	nTop += nAdd;
	memset(czBuf, 0x0, sizeof(czBuf));
	if(getnetmodeflag)
	{
    	sprintf(czBuf, "%s", g_network_mode[netmode]);
	}
	else
	{
		sprintf(czBuf, "%s", "--------");
	}
    
	TextOut(hdc, nLeft1, nTop, "��רģʽ��ѯ:");
	TextOut(hdc,nLeft2, nTop, czBuf);

}

PLUGIN_INFO g_NetmodeDraw={
	0x20600100,/*����id-*/
		NetmodeInit,/*-��ҳ����г�ʼ��-*/
		NULL,
		NetmodeDraw,/*-ҳ����ƺ���-*/
		NULL,
		NULL,
		NULL
};

#if DESC("����汾��",1)

extern APP_VERSION_INFO_T g_AppVersion;
int g_VersionIndex;
void app_version_Init(DWORD dwPluginId)
{
	g_VersionIndex = 0;
	gui_seek_app();
}

void app_version_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;

    char czBuf[64] = {0};
	RECT rc;
	rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 18;
    rc.bottom = rc.top + 16;
    
    int nLeft = 35;
    int nLeft2 = 65;
    int nTop = 20;
    int tmpTop = 20;

	if(g_AppVersion.loginAppCnt == 0)
	{
		SelectFont(hdc, pg_guicomm->fontBig);
		
	    sprintf(czBuf, "%s", ADV_APP_NAME);
	    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

//		SelectFont(hdc, pg_guicomm->fontSmall);
		// ����汾��
	    nTop = 20+tmpTop;
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
	}
	else
	{
		SelectFont(hdc, pg_guicomm->fontBig);
		
		sprintf(czBuf, "%s", g_AppVersion.pluginInfo[g_VersionIndex].comName);
	    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

//		SelectFont(hdc, pg_guicomm->fontSmall);
		// ����汾��
	    nTop = 20+tmpTop;
	    TextOut(hdc, nLeft, nTop, "����汾��");

	    nTop += tmpTop;
	    sprintf(czBuf, "%s", g_AppVersion.pluginInfo[g_VersionIndex].version);
	    TextOut(hdc, nLeft2, nTop, czBuf);


	    // ����ʱ��
	    nTop += tmpTop;
	    TextOut(hdc, nLeft, nTop, "����ʱ��");
	    nTop += tmpTop;
	    snprintf(czBuf, 64, "%04u-%02u-%02u %02u:%02u:%02u", g_AppVersion.pluginInfo[g_VersionIndex].dataTime.year_h*256+g_AppVersion.pluginInfo[g_VersionIndex].dataTime.year_l
			, g_AppVersion.pluginInfo[g_VersionIndex].dataTime.month, g_AppVersion.pluginInfo[g_VersionIndex].dataTime.day
			, g_AppVersion.pluginInfo[g_VersionIndex].dataTime.hour, g_AppVersion.pluginInfo[g_VersionIndex].dataTime.min
			, g_AppVersion.pluginInfo[g_VersionIndex].dataTime.sec);
	    TextOut(hdc, nLeft2, nTop, czBuf);
	}	
	 
}

BOOL app_version_KeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
    case KEY_LEFT:
	case KEY_UP:
        g_VersionIndex--;
        if (g_VersionIndex < 0)
        {
            g_VersionIndex = g_AppVersion.loginAppCnt-1;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
	case KEY_DOWN:
        g_VersionIndex++;
        if (g_VersionIndex >= g_AppVersion.loginAppCnt)
        {
            g_VersionIndex = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    return FALSE;
}

//�汾��Ϣ
PLUGIN_INFO g_layer_app_version_display =
{
    0x20600001/*id*/,
    app_version_Init/*InitLayer*/,
    NULL/*UninitLayer*/,
    app_version_Draw/*Draw*/,
    NULL/*Timer*/,
    app_version_KeyMsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#endif

#if DESC("���ɵ�ѹ����",1)
void ac_vol_cur_display_init(DWORD dwPluginId)
{
    memset(&display_data.vol, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.cur, 0, sizeof(OOP_ENERGYS_T));
    //    if(taskid == 0)
    {
        read_db_noamal_data(g_hUdbCenter, 0x20000200, 1, 0, (uint8*)&display_data.vol, sizeof(OOP_WORD3V_T));
        read_db_noamal_data(g_hUdbCenter, 0x20010200, 1, 0, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
    }
    //   else
    {
        //       read_db_record_data(g_hUdbCenter, 0x20000200, 0, taskid, (uint8*)&display_data.vol, sizeof(OOP_DWORD3V_T));
        //        read_db_record_data(g_hUdbCenter, 0x20010200, 0, taskid, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
    }

}

void vol_cur_display_draw(HDC hdc, DISPLAY_DATA_INFO bdisplay_data)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    float tmp;
    int i = 0;

    char czBuf[64] = { 0 };

    int nLeft2 = 120;
    int nTop = 42;
    int tmpTop = 14;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "��ѹ����");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 30, nTop, "��ѹ");
    SelectFont(hdc, g_guiComm.fontSmall);

    // ��ѹ
    for (i = 0; i < 3; i++)
    {
        if (bdisplay_data.vol.nNum == 0 || get_bit_value((uint8 *)&bdisplay_data.vol.rsv,32,i) == 0)
        {
            sprintf(czBuf, "%s --- V", g_showTitleVoltag34[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            tmp = (float)bdisplay_data.vol.nValue[i] / 10;
            sprintf(czBuf, "%s %2.1f V", g_showTitleVoltag34[i], tmp);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;
    }

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 30, nTop, "����");
    SelectFont(hdc, g_guiComm.fontSmall);
    // ����
    for (i = 0; i < 3; i++)
    {
        if (bdisplay_data.cur.nNum == 0 || get_bit_value((uint8 *)&bdisplay_data.cur.rsv,32,i) == 0)
        {
            sprintf(czBuf, "%s --- A", g_showTitleEelectric[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            if (bdisplay_data.cur.nValue[i] < 0)
            {
                tmp = ~bdisplay_data.cur.nValue[i] + 1;
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s -%3.3f A", g_showTitleEelectric[i], tmp);
            }
            else
            {
                tmp = bdisplay_data.cur.nValue[i];
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s %3.3f A", g_showTitleEelectric[i], tmp);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;

    }
}

void vol_cur_display_draws(HDC hdc)
{
	read_db_noamal_data(g_hUdbCenter, 0x20000200, 1, 0, (uint8*)&display_data.vol, sizeof(OOP_WORD3V_T));
    read_db_noamal_data(g_hUdbCenter, 0x20010200, 1, 0, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
    vol_cur_display_draw(hdc, display_data);
}


//��ѹ��������
PLUGIN_INFO g_layer_ac_vol_cur_display = {
    0x20900000/*id*/,
    ac_vol_cur_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    vol_cur_display_draws/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("ͨѶͨ������",1)

const char* g_pczChnlName[] =
{
    "��ͨ��",
    "����ͨ��",
};

const char* g_pczChnlType[] =
{
    "���߹���",
    "Radio",
    "��̫��",
};

const char* g_czCommType[] =
{
    "GPRS",
    "CDMA",
};
const char* g_czCommProtocol[] =
{
    "UDP",
    "TCP",
};

const char* g_czWorkMode[] =
{
    "���ģʽ",
    "�ͻ���ģʽ",
    "������ģʽ",
};

CHNL_INFO g_chnl_info;


void save_chnl_info(int index);
#if DESC("Radio",1)
const char* g_pczMeasureBaud[] = {"300", "600", "1200", "2400", "4800", "7200", "9600", "115200", "19200"};
const char* g_pczMeasureData[] = {"5λ", "6λ", "7λ", "8λ"};
const char* g_pczMeasureStop[] = {"1λ", "1.5λ", "2λ"};
const char* g_pczMeasureParity[] = {"��У��", "żУ��", "��У��"};

// ���ߵ��RS232����
WIRELESS_INFO g_wireless_info;
void WirelessInfoInit(DWORD dwPluginId)
{
	memset(&g_wireless_info, 0, sizeof(WIRELESS_INFO));

}
void WirelessInfoDraw(HDC hdc)
{
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

	char czBuf[300];
	sprintf(czBuf, "���ߵ� ��ϸ����");

    rc.top = 26;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

	// ������
	int nLeft1 = 24;
    int nLeft2 = 130;
    int nTop = rc.top + 20;

	TextOut(hdc, nLeft1, nTop, "������");
	if(g_wireless_info.nIndex == 0)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft2, nTop, (g_wireless_info.nBaudRate==-1)?"-":g_pczMeasureBaud[g_wireless_info.nBaudRate]);
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft2, nTop, (g_wireless_info.nBaudRate==-1)?"-":g_pczMeasureBaud[g_wireless_info.nBaudRate]);
	}
	
	// ����λ
	nTop += 16;
	TextOut(hdc, nLeft1, nTop, "����λ");
	if(g_wireless_info.nIndex == 1)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft2, nTop, (g_wireless_info.nData<0||g_wireless_info.nData>=4)?"-":g_pczMeasureData[g_wireless_info.nData]);
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft2, nTop, (g_wireless_info.nData<0||g_wireless_info.nData>=4)?"-":g_pczMeasureData[g_wireless_info.nData]);
	}
	
	// ֹͣλ
	nTop += 16;
	TextOut(hdc, nLeft1, nTop, "ֹͣλ");
	if(g_wireless_info.nIndex == 2)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft2, nTop, (g_wireless_info.nStop<0||g_wireless_info.nStop>=3)?"-":g_pczMeasureStop[g_wireless_info.nStop]);
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft2, nTop, (g_wireless_info.nStop<0||g_wireless_info.nStop>=3)?"-":g_pczMeasureStop[g_wireless_info.nStop]);
	}

	// ��żλ
	nTop += 16;
	TextOut(hdc, nLeft1, nTop, "��żλ");
	if(g_wireless_info.nIndex == 3)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft2, nTop, (g_wireless_info.nParity<0||g_wireless_info.nParity>=3)?"-":g_pczMeasureParity[g_wireless_info.nParity]);
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft2, nTop, (g_wireless_info.nParity<0||g_wireless_info.nParity>=3)?"-":g_pczMeasureParity[g_wireless_info.nParity]);
	}

	// ������ʱ
	nTop += 16;
	TextOut(hdc, nLeft1, nTop, "������ʱ");
	if(g_wireless_info.nDelay == -1)
	{
		sprintf(czBuf, "-ms");
	}
	else
	{
		sprintf(czBuf, "%dms", g_wireless_info.nDelay);
	}
	if(g_wireless_info.nIndex == 4)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft2, nTop, czBuf);
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft2, nTop, czBuf);
	}

    PopListDraw(hdc);
}

BOOL WirelessInfoKeyMsg(int nKey)
{
	// ������List�б���
	if(g_guiComm.bPopList == TRUE)
    {
        int nSel;
        char * pRet = PopListKeyMsg(nKey, &nSel);
        if(nKey == KEY_OK)
        {
            switch(g_wireless_info.nIndex)
            {
			case 0:
				g_wireless_info.nBaudRate = nSel;
				break;
			case 1:
				// ��������λ
				g_wireless_info.nData = nSel;
				break;
			case 2:
				// ����ֹͣλ
				g_wireless_info.nStop = nSel;
				break;
            case 3:
				// ������żλ
				g_wireless_info.nParity = nSel;
				break;
			case 4:
				// �����ӳٴ���
				g_wireless_info.nDelay = atoi(pRet);
				break;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch(nKey)
    {
    case KEY_UP:
        g_wireless_info.nIndex --;
        if(g_wireless_info.nIndex < 0)
        {
            g_wireless_info.nIndex = 0;
			return FALSE;
        }
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
    case KEY_DOWN:
		g_wireless_info.nIndex ++;
        if(g_wireless_info.nIndex > 4)
        {
            g_wireless_info.nIndex = 4;
			return FALSE;
        }

		UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
		if(ValidatePwd() == TRUE)
        {
			// ȷ�ϴ���
            int nLeft = 130;
            int nIndex = g_wireless_info.nIndex;
            int nTop = nIndex*16+40;

			switch(g_wireless_info.nIndex)
			{
			case 0:// ����������ѡ��List
				PopList(g_pczMeasureBaud, 8, 
					(g_wireless_info.nBaudRate==-1)?g_pczMeasureBaud[0]:g_pczMeasureBaud[g_wireless_info.nBaudRate],
					nLeft, nTop-18, FALSE);
				break;
			case 1:// ��������λѡ��List
				PopList(g_pczMeasureData, 4, 
					(g_wireless_info.nData<0||g_wireless_info.nData>3)?g_pczMeasureData[0]:g_pczMeasureData[g_wireless_info.nData],
					nLeft, nTop, FALSE);
				break;
			case 2:// ����ֹͣλѡ��List
				PopList(g_pczMeasureStop, 3, 
					(g_wireless_info.nStop<0||g_wireless_info.nStop>2)?g_pczMeasureStop[0]:g_pczMeasureStop[g_wireless_info.nStop],
					nLeft, nTop, FALSE);
				break;// ������żλѡ��List
			case 3:PopList(g_pczMeasureParity, 3, 
					(g_wireless_info.nStop<0||g_wireless_info.nParity>2)?g_pczMeasureParity[0]:g_pczMeasureParity[g_wireless_info.nParity],
					nLeft, nTop-14, FALSE);
				break;
			case 4:// �����ӳٴ���ѡ��List
				char czBuf[20];
				sprintf(czBuf, "%d", g_wireless_info.nDelay);
				PopList(g_pczRadioDelay, 8, czBuf, nLeft, nTop-5*16, FALSE);
				break;
			}

			return TRUE;
        }
    }
    return FALSE;
}

PLUGIN_INFO g_layerWirelessInfo = {
	0x00660003/*id*/, 
	WirelessInfoInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	WirelessInfoDraw/*Draw*/,
	NULL/*Timer*/,
	WirelessInfoKeyMsg/*keymsg*/,
	NULL/*ipc*/
};

PLUGIN_INFO* g_pluginWireless[] =
{
    &g_layerWirelessInfo,
};


#endif


#if DESC("���߹���",1)

uint8 GprsConfigIndex;
OOP_MASTERPARAMS_T GPRSMaster;
OOP_GPRSCONFIG_T GprsConfig;
int GprsConfigIsSet;
uint8 gNetTypeSet;  //enum{2G��0���� 3G��1���� 4G��2���� 5G��3����δ֪�� 255�� }
uint8 gAuthTypeSet; //enum{null��0���� chap��1���� pap��2���� pap&chap��3��������Ӧ��255�� }
char* g_netName[] = {(char*)"����2G", (char*)"����3G", (char*)"����4G", (char*)"����5G", (char*)"������"};
char* g_authName[] = {(char*)"null", (char*)"chap", (char*)"pip", (char*)"pap&chap", (char*)"����Ӧ"};
#ifdef AREA_CHONGQING
uint8 ApnIndex;
uint8 ApnSetType;
#endif
//uint8 netTypeFlag;
void gprs_set_init(HDC hdc)
{
    GprsConfigIndex = 0;
    gNetTypeSet = 0xFF;
    memset(&GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T));
    memset(&GPRSMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));

    read_db_noamal_data(g_hUdbCenter, 0x45000300, 0, 0, (uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T));
    GprsConfigIsSet = read_db_noamal_data(g_hUdbCenter, 0x45000200, 0, 0, (uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T));
//    netTypeFlag = 0;
#ifdef AREA_CHONGQING
    for(int i = 0; i < 3 ;i++){
        if (strcmp(GprsConfig.apn.apn.value, g_apn_list[i]) == 0){
            ApnIndex = i;
            break;
        }
    }
    ApnSetType = 0;
#endif

    if(0 != GprsConfigIsSet)
    {
        gui_get_default_gprs(&GprsConfig);
        GprsConfigIsSet = 0;
    }


    read_db_noamal_data(g_hUdbCenter, 0x4500F100, 0, 0, &gNetTypeSet, sizeof(uint8));
    if(gNetTypeSet > 3)
    {
        gNetTypeSet = 255;
    }

    read_db_noamal_data(g_hUdbCenter, 0x4500F200, 0, 0, &gAuthTypeSet, sizeof(uint8));
    if(gAuthTypeSet > 3)
    {
        gAuthTypeSet = 255;
    }

}

#ifdef AREA_CHONGQING
void gprs_set_draw1(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

    char czBuf[300];
    int nTop = 17;
    int nLeft1 = 25;
    int nLeft2 = 120;

    TextOut(hdc, nLeft1, nTop, "����ģʽ");
    if (GprsConfig.workmode>2)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_work_mode[GprsConfig.workmode]);
    }
    if (GprsConfigIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "���߷�ʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", (GprsConfigIsSet < 0 || GprsConfig.linetype> 1) ? "-": g_line_type[GprsConfig.linetype]);
    if(GprsConfigIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "���ӷ�ʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", GprsConfig.contype > 1 ? "-" : g_conn_type[GprsConfig.contype]);
    if (GprsConfigIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "��վIP");
    if (GPRSMaster.master[0].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], 
                                      GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
    }
    if (GprsConfigIndex == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "��վ�˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (GPRSMaster.master[0].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GPRSMaster.master[0].port);
    }
    if (GprsConfigIndex == 4)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "APN���÷�ʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", g_apnSetType_list[ApnSetType]);
    if (GprsConfigIndex == 5)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "APN");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", GprsConfig.apn.apn.nNum == 0 ? "-" : GprsConfig.apn.apn.value);
    if (GprsConfigIndex == 6)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    nLeft2 += 20;
    if (GprsConfigIndex == 7)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "��һҳ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "��һҳ");
    }
    PopListDraw(hdc);
}

void gprs_set_draw2(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

    char czBuf[300];
    int nTop = 17;
    int nLeft1 = 25;
    int nLeft2 = 120;


    TextOut(hdc, nLeft1, nTop, "��������");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d ��", GprsConfig.heart);
    if (GprsConfigIndex == 8)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "����ģʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", GprsConfig.conmode > 1 ? "-" : g_conn_app_type[GprsConfig.conmode]);
    if (GprsConfigIndex == 9)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "�û���");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", GprsConfig.apn.username.nNum == 0 ? "-" : GprsConfig.apn.username.value);
    if (GprsConfigIndex == 10)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2 , nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "����");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", GprsConfig.apn.pwd.nNum == 0 ? "-" : GprsConfig.apn.pwd.value);
    if (GprsConfigIndex == 11)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "����IP");
    if (GprsConfig.apn.proxyip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
    }
    if (GprsConfigIndex == 12)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "����˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (GprsConfig.apn.proxyport == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
    }
    if (GprsConfigIndex == 13)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "��ʱʱ��");
    memset(czBuf, 0x00, sizeof(czBuf));
    if ( GprsConfig.dail.mask.dailtime == 0x3f)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d ��", GprsConfig.dail.mask.dailtime);
    }
    if (GprsConfigIndex == 14)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    nLeft1 += 40;
    if (GprsConfigIndex == 15)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "��һҳ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "��һҳ");
    }

    nLeft2 += 40;
    if (GprsConfigIndex == 16)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "��һҳ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "��һҳ");
    }
    
    PopListDraw(hdc);
}

void gprs_set_draw3(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

    char czBuf[300];
    int nTop = 17;
    int nLeft1 = 25;
    int nLeft2 = 120;

    TextOut(hdc, nLeft1, nTop, "�ط�����");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
    if (GprsConfigIndex == 17)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }


    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "��������");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", gNetTypeSet <= 3 ? g_netName[gNetTypeSet] : "������");
    if (GprsConfigIndex == 18)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "��Ȩ��ʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", gAuthTypeSet <= 3 ? g_authName[gAuthTypeSet] : "����Ӧ");
    if (GprsConfigIndex == 19)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "������վIP");
    if (GPRSMaster.master[1].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], 
                                      GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
    }
    if (GprsConfigIndex == 20)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "������վ�˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (GPRSMaster.master[1].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GPRSMaster.master[1].port);
    }
    if (GprsConfigIndex == 21)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "�����˿��б�");

    nTop += 14;
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", GprsConfig.listen[0]);
    if (GprsConfigIndex == 22)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, czBuf);
    }

    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", GprsConfig.listen[1]);
    if (GprsConfigIndex == 23)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+40, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+40, nTop, czBuf);
    }

    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", GprsConfig.listen[2]);
    if (GprsConfigIndex == 24)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+80, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+80, nTop, czBuf);
    }

    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", GprsConfig.listen[3]);
    if (GprsConfigIndex == 25)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+120, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+120, nTop, czBuf);
    }

    nTop += 14;
    nLeft1 += 40;
    if (GprsConfigIndex == 26)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, 114, "��һҳ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, 114, "��һҳ");
    }
    PopListDraw(hdc);
}

void gprs_set_draw(HDC hdc)
{
    if(GprsConfigIndex < 8)//��һҳ
    {
        return gprs_set_draw1(hdc);
    }
    else if(GprsConfigIndex < 17)    //�ڶ�ҳ
    {
        return gprs_set_draw2(hdc);
    }
    else
    {
        return gprs_set_draw3(hdc);
    }
}

void GprsConfigReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    switch (GprsConfigIndex)
    {
        case 6: //APN
        {
            GprsConfig.apn.apn.nNum = strlen(pNewStr);
            memset(GprsConfig.apn.apn.value, 0, sizeof(GprsConfig.apn.apn.value));
            memcpy(GprsConfig.apn.apn.value, pNewStr, GprsConfig.apn.apn.nNum);
            GprsConfigIsSet = 1;
        }
        break;
        case 10: //�û���
        {
            GprsConfig.apn.username.nNum = strlen(pNewStr);
            memset(GprsConfig.apn.username.value, 0, sizeof(GprsConfig.apn.username.value));
            memcpy(GprsConfig.apn.username.value, pNewStr, GprsConfig.apn.username.nNum);
            GprsConfigIsSet = 1;
        }
        break;
        case 11: //����
        {
            GprsConfig.apn.pwd.nNum = strlen(pNewStr);
            memset(GprsConfig.apn.pwd.value, 0, sizeof(GprsConfig.apn.pwd.value));
            memcpy(GprsConfig.apn.pwd.value, pNewStr, GprsConfig.apn.pwd.nNum);
            GprsConfigIsSet = 1;
        }
        break;

        case 12: //�����������ַ
        {
            if (FALSE == commfun_IP_unpack(pNewStr, tmp))
            {
                if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                }
            }
            else
            {
                memcpy(GprsConfig.apn.proxyip, tmp, 4);
                GprsConfigIsSet = 1;
            }
        }
        break;
        case 13: //�˿�
        {
            int port = atoi(pNewStr);
            if (port <= 65535)
            {
                GprsConfig.apn.proxyport = port;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyport == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 14: //��ʱʱ��
        {
            int time = atoi(pNewStr);
            if (time < 0x3f)
            {
                GprsConfig.dail.mask.dailtime = time;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("��ʱʱ������\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0 || GprsConfig.dail.mask.dailtime == 0x3f)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.dail.mask.dailtime);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 17: //�ط�����
        {
            int cnt = atoi(pNewStr);
            if (cnt < 4)
            {
                GprsConfig.dail.mask.dailcnt = cnt;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("�ط���������\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 8: //��������
        {
            int heart = atoi(pNewStr);
            if (heart < 65535)
            {
                GprsConfig.heart = heart;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("������������\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.heart);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;

        case 3: //�����������ַ
        {
            if (FALSE == commfun_IP_unpack(pNewStr, tmp))
            {
                if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GPRSMaster.master[0].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                }
            }
            else
            {
                memcpy(GPRSMaster.master[0].ip, tmp, 4);
                GPRSMaster.nNum = 2;
                GprsConfigIsSet = 1;
            }
        }
        break;
        case 4: //�˿�
        {
            int port = atoi(pNewStr);
            if (port <= 65535)
            {
                GPRSMaster.master[0].port = port;
                GPRSMaster.nNum = 2;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GPRSMaster.master[0].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GPRSMaster.master[0].port);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 20: //�����������ַ
        {
            if (FALSE == commfun_IP_unpack(pNewStr, tmp))
            {
                if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GPRSMaster.master[1].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], 
                                                      GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                }
            }
            else
            {
                memcpy(GPRSMaster.master[1].ip, tmp, 4);
                GPRSMaster.nNum = 2;
                GprsConfigIsSet = 1;
            }
        }
        break;
        case 21: //�˿�
        {
            int port = atoi(pNewStr);
            if (port <= 65535)
            {
                GPRSMaster.master[1].port = port;
                GPRSMaster.nNum = 2;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GPRSMaster.master[1].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GPRSMaster.master[1].port);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 22: //�˿�
        {
            int port = atoi(pNewStr);
            if(port <= 65535)
            {
                if((GprsConfig.listen[1]!=0)&&(port == 0))
                {
                    if(TRUE == MessageBox("��2������0����������0��", MB_OKCANCEL))
                    {
                        sprintf(czBuf, "%d", GprsConfig.listen[0]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01); 
                    }
                    else
                    {
                        break;
                    }
                }else
                {
                    GprsConfig.listen[0] = port;
                }
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfig.listen[0] == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", GprsConfig.listen[0]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 23: //�˿�
        {
            int port = atoi(pNewStr);
            if(port <= 65535)
            {
                if((GprsConfig.listen[2]!=0)&&(port == 0))
                {
                    if(TRUE == MessageBox("��3������0����������0��", MB_OKCANCEL))
                    {
                        sprintf(czBuf, "%d", GprsConfig.listen[1]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01); 
                    }
                    else
                    {
                        break;
                    }
                }else
                {
                    GprsConfig.listen[1] = port;
                }
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfig.listen[1] == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.listen[1]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 24: //�˿�
        {
            int port = atoi(pNewStr);
            if(port <= 65535)
            {
                if((GprsConfig.listen[3]!=0)&&(port == 0))
                {
                    if(TRUE == MessageBox("��4������0����������0����", MB_OKCANCEL))
                    {
                        sprintf(czBuf, "%d", GprsConfig.listen[2]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01); 
                    }
                    else
                    {
                        break;
                    }
                }else
                {
                    GprsConfig.listen[2] = port;
                }
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfig.listen[2] == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.listen[2]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 25: //�˿�
        {
            int port = atoi(pNewStr);
            if(port <= 65535)
            {
                GprsConfig.listen[3] = port;
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfig.listen[3] == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.listen[3]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
    }

}

BOOL gprs_set_keymsg(int nKey)
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
            switch (GprsConfigIndex)
            {
                case 0://����ģʽ
                {
                    GprsConfig.workmode = nSel;
                    GprsConfigIsSet = 1;
                }
                break;
                case 1://���߷�ʽ
                {
                    GprsConfig.linetype = nSel;
                    GprsConfigIsSet = 1;
                }
                break;
                case 2://���ӷ�ʽ
                {
                    GprsConfig.contype = nSel;
                    GprsConfigIsSet = 1;
                }
                break;
                case 9://����ģʽ
                {
                    GprsConfig.conmode = nSel;
                    GprsConfigIsSet = 1;
                }
                break;
                case 5://APN���÷�ʽ
                {
                    ApnSetType = nSel;
                }
                break;
                case 6://APN
                {
                    ApnIndex = nSel;
                    bzero(GprsConfig.apn.apn.value, sizeof(GprsConfig.apn.apn.value));
                    memcpy(&GprsConfig.apn.apn.value,g_apn_list[ApnIndex], strlen(g_apn_list[ApnIndex]));
                    GprsConfig.apn.apn.nNum = strlen(g_apn_list[ApnIndex]);
                    GprsConfigIsSet = 1;
                }
                break;
                case 18://��������
                {
                    gNetTypeSet = nSel;
                }
                break;
                case 19://��Ȩ��ʽ
                {
                    gAuthTypeSet = nSel;
                }
                break;
                default:
                    return FALSE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            if (GprsConfigIndex > 0)
            {
                GprsConfigIndex--;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            if (GprsConfigIndex < 26)
            {
                GprsConfigIndex++;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if (GprsConfigIndex == 7)
            {
                GprsConfigIndex = 8;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            else if(GprsConfigIndex == 15)
            {
                GprsConfigIndex = 7;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            else if(GprsConfigIndex == 16)
            {
                GprsConfigIndex = 17;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            else if(GprsConfigIndex == 26)
            {
                GprsConfigIndex = 16;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            
            if (ValidatePwd() == TRUE)
            {
                switch (GprsConfigIndex)
                {
                    case 0: //����ģʽ
                    {
                        PopList(g_work_mode, 3, g_work_mode[0], 120, 20, FALSE);
                        return TRUE;
                    }
                    case 1: //����ģʽ
                    {
                        PopList(g_line_type, 2, g_line_type[0], 120, 35, FALSE);
                        return TRUE;
                    }
                    case 2://���ӷ�ʽ
                    {
                        PopList(g_conn_type, 2, g_conn_type[0], 120, 50, FALSE);
                        return TRUE;
                    }
                    case 9://����Ӧ�÷�ʽ
                    {
                        PopList(g_conn_app_type, 2, g_conn_app_type[0], 120, 65, FALSE);
                        return TRUE;
                    }
                    case 5://APN���÷�ʽ
                    {
                        PopList(g_apnSetType_list, 2, g_apnSetType_list[0], 120, 80, FALSE);
                        return TRUE;
                    }
                    case 6://APN
                    {
                        if (ApnSetType == 0){
                            PopList(g_apn_list, 3, g_apn_list[ApnIndex], 120, 80, FALSE);
                        }
                        else {
                            char czBuf[20];
                            if (GprsConfigIsSet < 0 || GprsConfig.apn.apn.nNum == 0)
                            {
                                strcpy(czBuf, "");
                            }
                            else
                            {
                                sprintf(czBuf, "%s", GprsConfig.apn.apn.value);
                            }
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                        }
                        return TRUE;
                    }
                    case 10://�û���
                    {
                        char czBuf[20];
                        if (GprsConfig.apn.username.nNum == 0)
                        {
                            strcpy(czBuf, "");
                        }
                        else
                        {
                            sprintf(czBuf, "%s", GprsConfig.apn.username.value);
                        }
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                        return TRUE;
                    }
                    case 11://����
                    {
                        char czBuf[20];
                        if (GprsConfig.apn.pwd.nNum == 0)
                        {
                            strcpy(czBuf, "");
                        }
                        else
                        {
                            sprintf(czBuf, "%s", GprsConfig.apn.pwd.value);
                        }
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                        return TRUE;
                    }
                    case 12://�����ַ
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], 
                                        GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                        return TRUE;
                    }
                    case 13://����˿�
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 14://��ʱʱ��
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GprsConfig.dail.mask.dailtime);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 17://���Դ���
                    {
                        char czBuf[20];
                        if (GprsConfigIsSet < 0)
                        sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 8://����
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GprsConfig.heart);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 18://��������
                    {
                        PopList(g_netName, 5, g_netName[0], 120, 20, FALSE);
                        return TRUE;
                    }
                    case 19://��Ȩ��ʽ
                    {
                        PopList(g_authName, 5, g_authName[0], 120, 35, FALSE);
                        return TRUE;
                    }
                    case 3://IP��ַ
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], 
                                                      GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                        return TRUE;
                    }
                    case 4://�˿�
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GPRSMaster.master[0].port);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                   case 20://IP��ַ
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], 
                                                      GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                        return TRUE;
                    }
                    case 21://�˿�
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GPRSMaster.master[1].port);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 22://�˿�
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GprsConfig.listen[0]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 23://�˿�
                    {
                        char czBuf[60];
                        if(GprsConfig.listen[0]==0)
                        {
                            sprintf(czBuf, "��%d��û������,����˳�����ã�", GprsConfigIndex - 21);
                            MessageBox(czBuf);
                        }else{
                            sprintf(czBuf, "%d",GprsConfig.listen[1]);
                            InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        }
                        return TRUE;
                    }
                    case 24://�˿�
                    {
                        char czBuf[60];
                        if(GprsConfig.listen[1]==0)
                        {
                            sprintf(czBuf, "��%d��û������,����˳�����ã�", GprsConfigIndex - 21);
                            MessageBox(czBuf);
                        }else{
                            sprintf(czBuf, "%d",GprsConfig.listen[2]);
                            InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        }
                        return TRUE;
                    }
                    case 25://�˿�
                    {
                        char czBuf[60];
                        if(GprsConfig.listen[2]==0)
                        {
                            sprintf(czBuf, "��%d��û������,����˳�����ã�", GprsConfigIndex - 21);
                            MessageBox(czBuf);
                        }else{
                            sprintf(czBuf, "%d",GprsConfig.listen[3]);
                            InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        }
                        return TRUE;
                    }
                }
            }
        }
        /* ����Ҫbreak */
        case KEY_CANCEL:
        if(TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
        {
            GprsConfig.listenNum = 0;
            for (int i =0;i< OOP_MAX_LISTENS;i++){
                if (GprsConfig.listen[i] >0)
                    GprsConfig.listenNum++;
            }
             save_chnl_info(0);;
        }
        return FALSE;
    }
    return FALSE;
}
#else
void gprs_set_draw1(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

    char czBuf[300];
    int nTop = 17;
    int nLeft1 = 25;
    int nLeft2 = 120;

    TextOut(hdc, nLeft1, nTop, "����ģʽ");
    if (GprsConfig.workmode>2)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_work_mode[GprsConfig.workmode]);
    }
    if (GprsConfigIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "���߷�ʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", (GprsConfigIsSet < 0 || GprsConfig.linetype> 1) ? "-": g_line_type[GprsConfig.linetype]);
    if(GprsConfigIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "���ӷ�ʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", GprsConfig.contype > 1 ? "-" : g_conn_type[GprsConfig.contype]);
    if (GprsConfigIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "����ģʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", GprsConfig.conmode > 1 ? "-" : g_conn_app_type[GprsConfig.conmode]);
    if (GprsConfigIndex == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "APN");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", GprsConfig.apn.apn.nNum == 0 ? "-" : GprsConfig.apn.apn.value);
    if (GprsConfigIndex == 4)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "�û���");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", GprsConfig.apn.username.nNum == 0 ? "-" : GprsConfig.apn.username.value);
    if (GprsConfigIndex == 5)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "����");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", GprsConfig.apn.pwd.nNum == 0 ? "-" : GprsConfig.apn.pwd.value);
    if (GprsConfigIndex == 6)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    nLeft2 += 20;
    if (GprsConfigIndex == 7)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "��һҳ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "��һҳ");
    }
    PopListDraw(hdc);
}

void gprs_set_draw2(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontSmall);
	ITEM_NOT_SELECT;


    char czBuf[300];
    int nTop = 17;
    int nLeft1 = 25;
    int nLeft2 = 120;

    TextOut(hdc, nLeft1, nTop, "����IP");
    if (GprsConfig.apn.proxyip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
    }
    if (GprsConfigIndex == 8)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "����˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (GprsConfig.apn.proxyport == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
    }
    if (GprsConfigIndex == 9)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "��ʱʱ��");
    memset(czBuf, 0x00, sizeof(czBuf));
    if ( GprsConfig.dail.mask.dailtime == 0x3f)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d ��", GprsConfig.dail.mask.dailtime);
    }
    if (GprsConfigIndex == 10)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "�ط�����");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
    if (GprsConfigIndex == 11)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "��������");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d ��", GprsConfig.heart);
    if (GprsConfigIndex == 12)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "��������");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", gNetTypeSet <= 3 ? g_netName[gNetTypeSet] : "������");
    if (GprsConfigIndex == 13)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "��Ȩ��ʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%s", gAuthTypeSet <= 3 ? g_authName[gAuthTypeSet] : "����Ӧ");
    if (GprsConfigIndex == 14)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    nLeft1 += 20;
    if (GprsConfigIndex == 15)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "��һҳ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "��һҳ");
    }

    nLeft2 += 20;
    if (GprsConfigIndex == 16)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "��һҳ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "��һҳ");
    }
    
    PopListDraw(hdc);
}

void gprs_set_draw3(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontSmall);
	ITEM_NOT_SELECT;

    char czBuf[300];
    int nTop = 17;
    int nLeft1 = 25;
    int nLeft2 = 120;

    TextOut(hdc, nLeft1, nTop, "��վIP");
    if (GPRSMaster.master[0].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], 
                                      GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
    }
    if (GprsConfigIndex == 17)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "��վ�˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (GPRSMaster.master[0].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GPRSMaster.master[0].port);
    }
    if (GprsConfigIndex == 18)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "������վIP");
    if (GPRSMaster.master[1].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], 
                                      GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
    }
    if (GprsConfigIndex == 19)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "������վ�˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (GPRSMaster.master[1].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GPRSMaster.master[1].port);
    }
    if (GprsConfigIndex == 20)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "�����˿��б�");

    nTop += 14;
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", GprsConfig.listen[0]);
    if (GprsConfigIndex == 21)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, czBuf);
    }

    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", GprsConfig.listen[1]);
    if (GprsConfigIndex == 22)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+40, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+40, nTop, czBuf);
    }

    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", GprsConfig.listen[2]);
    if (GprsConfigIndex == 23)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+80, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+80, nTop, czBuf);
    }

    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", GprsConfig.listen[3]);
    if (GprsConfigIndex == 24)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+120, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+120, nTop, czBuf);
    }

    nTop += 14;
    nLeft1 += 20;
    if (GprsConfigIndex == 25)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, 114, "��һҳ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, 114, "��һҳ");
    }
}

void gprs_set_draw(HDC hdc)
{
    if(GprsConfigIndex < 8)//��һҳ
    {
        return gprs_set_draw1(hdc);
    }
    else if(GprsConfigIndex < 17)    //�ڶ�ҳ
    {
        return gprs_set_draw2(hdc);
    }
    else
    {
        return gprs_set_draw3(hdc);
    }
}

void GprsConfigReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    switch (GprsConfigIndex)
    {
        case 4: //APN
        {
            GprsConfig.apn.apn.nNum = strlen(pNewStr);
            memset(GprsConfig.apn.apn.value, 0, sizeof(GprsConfig.apn.apn.value));
            memcpy(GprsConfig.apn.apn.value, pNewStr, GprsConfig.apn.apn.nNum);
            GprsConfigIsSet = 1;
        }
        break;
        case 5: //�û���
        {
            GprsConfig.apn.username.nNum = strlen(pNewStr);
            memset(GprsConfig.apn.username.value, 0, sizeof(GprsConfig.apn.username.value));
            memcpy(GprsConfig.apn.username.value, pNewStr, GprsConfig.apn.username.nNum);
            GprsConfigIsSet = 1;
        }
        break;
        case 6: //����
        {
            GprsConfig.apn.pwd.nNum = strlen(pNewStr);
            memset(GprsConfig.apn.pwd.value, 0, sizeof(GprsConfig.apn.pwd.value));
            memcpy(GprsConfig.apn.pwd.value, pNewStr, GprsConfig.apn.pwd.nNum);
            GprsConfigIsSet = 1;
        }
        break;

        case 8: //�����������ַ
        {
            if (FALSE == commfun_IP_unpack(pNewStr, tmp))
            {
                if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                }
            }
            else
            {
                memcpy(GprsConfig.apn.proxyip, tmp, 4);
                GprsConfigIsSet = 1;
            }
        }
        break;
        case 9: //�˿�
        {
            int port = atoi(pNewStr);
            if (port <= 65535)
            {
                GprsConfig.apn.proxyport = port;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyport == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 10: //��ʱʱ��
        {
            int time = atoi(pNewStr);
            if (time < 0x3f)
            {
                GprsConfig.dail.mask.dailtime = time;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("��ʱʱ������\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0 || GprsConfig.dail.mask.dailtime == 0x3f)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.dail.mask.dailtime);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 11: //�ط�����
        {
            int cnt = atoi(pNewStr);
            if (cnt < 4)
            {
                GprsConfig.dail.mask.dailcnt = cnt;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("�ط���������\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 12: //��������
        {
            int heart = atoi(pNewStr);
            if (heart < 65535)
            {
                GprsConfig.heart = heart;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("������������\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.heart);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;

        case 17: //�����������ַ
        {
            if (FALSE == commfun_IP_unpack(pNewStr, tmp))
            {
                if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GPRSMaster.master[0].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                }
            }
            else
            {
                memcpy(GPRSMaster.master[0].ip, tmp, 4);
				GPRSMaster.nNum = 2;
                GprsConfigIsSet = 1;
            }
        }
        break;
        case 18: //�˿�
        {
            int port = atoi(pNewStr);
            if (port <= 65535)
            {
                GPRSMaster.master[0].port = port;
				GPRSMaster.nNum = 2;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GPRSMaster.master[0].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GPRSMaster.master[0].port);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 19: //�����������ַ
        {
            if (FALSE == commfun_IP_unpack(pNewStr, tmp))
            {
                if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GPRSMaster.master[1].ip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], 
                                                      GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                }
            }
            else
            {
                memcpy(GPRSMaster.master[1].ip, tmp, 4);
				GPRSMaster.nNum = 2;
                GprsConfigIsSet = 1;
            }
        }
        break;
        case 20: //�˿�
        {
            int port = atoi(pNewStr);
            if (port <= 65535)
            {
                GPRSMaster.master[1].port = port;
				GPRSMaster.nNum = 2;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GPRSMaster.master[1].port == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GPRSMaster.master[1].port);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 21: //�˿�
        {
            int port = atoi(pNewStr);
            if(port <= 65535)
            {
                if((GprsConfig.listen[1]!=0)&&(port == 0))
                {
                    if(TRUE == MessageBox("��2������0����������0��", MB_OKCANCEL))
                    {
                        sprintf(czBuf, "%d", GprsConfig.listen[0]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01); 
                    }
                    else
                    {
                        break;
                    }
                }else
                {
                    GprsConfig.listen[0] = port;
                }
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfig.listen[0] == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d", GprsConfig.listen[0]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 22: //�˿�
        {
            int port = atoi(pNewStr);
            if(port <= 65535)
            {
                if((GprsConfig.listen[2]!=0)&&(port == 0))
                {
                    if(TRUE == MessageBox("��3������0����������0��", MB_OKCANCEL))
                    {
                        sprintf(czBuf, "%d", GprsConfig.listen[1]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01); 
                    }
                    else
                    {
                        break;
                    }
                }else
                {
                    GprsConfig.listen[1] = port;
                }
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfig.listen[1] == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.listen[1]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 23: //�˿�
        {
            int port = atoi(pNewStr);
            if(port <= 65535)
            {
                if((GprsConfig.listen[3]!=0)&&(port == 0))
                {
                    if(TRUE == MessageBox("��4������0����������0����", MB_OKCANCEL))
                    {
                        sprintf(czBuf, "%d", GprsConfig.listen[2]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01); 
                    }
                    else
                    {
                        break;
                    }
                }else
                {
                    GprsConfig.listen[2] = port;
                }
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfig.listen[2] == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.listen[2]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 24: //�˿�
        {
            int port = atoi(pNewStr);
            if(port <= 65535)
            {
                GprsConfig.listen[3] = port;
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfig.listen[3] == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.listen[3]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
    }

}

BOOL gprs_set_keymsg(int nKey)
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
            switch (GprsConfigIndex)
            {
                case 0://����ģʽ
                {
                    GprsConfig.workmode = nSel;
                    GprsConfigIsSet = 1;
                }
                break;
                case 1://���߷�ʽ
                {
                    GprsConfig.linetype = nSel;
                    GprsConfigIsSet = 1;
                }
                break;
                case 2://���ӷ�ʽ
                {
                    GprsConfig.contype = nSel;
                    GprsConfigIsSet = 1;
                }
                break;
                case 3://����ģʽ
                {
                    GprsConfig.conmode = nSel;
                    GprsConfigIsSet = 1;
                }
                break;
                case 13://��������
                {
                    gNetTypeSet = nSel;
                }
                break;
                case 14://��Ȩ��ʽ
                {
                    gAuthTypeSet = nSel;
                }
                break;
                default:
                    return FALSE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            if (GprsConfigIndex > 0)
            {
                GprsConfigIndex--;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            if (GprsConfigIndex < 25)
            {
                GprsConfigIndex++;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if (GprsConfigIndex == 7)
            {
                GprsConfigIndex = 8;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            else if(GprsConfigIndex == 15)
            {
                GprsConfigIndex = 7;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            else if(GprsConfigIndex == 16)
            {
                GprsConfigIndex = 17;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            else if(GprsConfigIndex == 25)
            {
                GprsConfigIndex = 16;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            
            if (ValidatePwd() == TRUE)
            {
                switch (GprsConfigIndex)
                {
                    case 0: //����ģʽ
                    {
                        PopList(g_work_mode, 3, g_work_mode[0], 120, 20, FALSE);
                        return TRUE;
                    }
                    case 1: //����ģʽ
                    {
                        PopList(g_line_type, 2, g_line_type[0], 120, 35, FALSE);
                        return TRUE;
                    }
                    case 2://���ӷ�ʽ
                    {
                        PopList(g_conn_type, 2, g_conn_type[0], 120, 50, FALSE);
                        return TRUE;
                    }
                    case 3://����Ӧ�÷�ʽ
                    {
                        PopList(g_conn_app_type, 2, g_conn_app_type[0], 120, 65, FALSE);
                        return TRUE;
                    }
                    case 4://APN
                    {
                        char czBuf[20];
                        if (GprsConfig.apn.apn.nNum == 0)
                        {
                            strcpy(czBuf, "");
                        }
                        else
                        {
                            sprintf(czBuf, "%s", GprsConfig.apn.apn.value);
                        }
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                        return TRUE;
                    }
                    case 5://�û���
                    {
                        char czBuf[20];
                        if (GprsConfig.apn.username.nNum == 0)
                        {
                            strcpy(czBuf, "");
                        }
                        else
                        {
                            sprintf(czBuf, "%s", GprsConfig.apn.username.value);
                        }
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                        return TRUE;
                    }
                    case 6://����
                    {
                        char czBuf[20];
                        if (GprsConfig.apn.pwd.nNum == 0)
                        {
                            strcpy(czBuf, "");
                        }
                        else
                        {
                            sprintf(czBuf, "%s", GprsConfig.apn.pwd.value);
                        }
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                        return TRUE;
                    }
                    case 8://�����ַ
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], 
                                        GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                        return TRUE;
                    }
                    case 9://����˿�
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 10://��ʱʱ��
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GprsConfig.dail.mask.dailtime);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 11://���Դ���
                    {
                        char czBuf[20];
                        if (GprsConfigIsSet < 0)
                        sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 12://����
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GprsConfig.heart);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 13://����Ӧ�÷�ʽ
                    {
                        PopList(g_netName, 5, g_netName[0], 120, 55, FALSE);
                        return TRUE;
                    }
                    case 14://����Ӧ�÷�ʽ
                    {
                        PopList(g_authName, 5, g_authName[0], 120, 55, FALSE);
                        return TRUE;
                    }
                    case 17://IP��ַ
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], 
                                                      GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                        return TRUE;
                    }
                    case 18://�˿�
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GPRSMaster.master[0].port);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                   case 19://IP��ַ
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], 
                                                      GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                        return TRUE;
                    }
                    case 20://�˿�
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GPRSMaster.master[1].port);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 21://�˿�
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GprsConfig.listen[0]);
                        InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 22://�˿�
                    {
                        char czBuf[60];
                        if(GprsConfig.listen[0]==0)
                        {
                            sprintf(czBuf, "��%d��û������,����˳�����ã�", GprsConfigIndex - 21);
                            MessageBox(czBuf);
                        }else{
                            sprintf(czBuf, "%d",GprsConfig.listen[1]);
                            InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        }
                        return TRUE;
                    }
                    case 23://�˿�
                    {
                        char czBuf[60];
                        if(GprsConfig.listen[1]==0)
                        {
                            sprintf(czBuf, "��%d��û������,����˳�����ã�", GprsConfigIndex - 21);
                            MessageBox(czBuf);
                        }else{
                            sprintf(czBuf, "%d",GprsConfig.listen[2]);
                            InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        }
                        return TRUE;
                    }
                    case 24://�˿�
                    {
                        char czBuf[60];
                        if(GprsConfig.listen[2]==0)
                        {
                            sprintf(czBuf, "��%d��û������,����˳�����ã�", GprsConfigIndex - 21);
                            MessageBox(czBuf);
                        }else{
                            sprintf(czBuf, "%d",GprsConfig.listen[3]);
                            InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                        }
                        return TRUE;
                    }
                }
            }
        }
        /* ����Ҫbreak */
    	case KEY_CANCEL:
        if(TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
    	{
			GprsConfig.listenNum = 0;
			for (int i =0;i< OOP_MAX_LISTENS;i++){
				if (GprsConfig.listen[i] >0)
					GprsConfig.listenNum++;
			}
    	    save_chnl_info(0);;
    	}
        return FALSE;
    }
    return FALSE;
}
#endif
//���߹���ͨ����������
PLUGIN_INFO g_layer_gprs_set = {
    0x20750001/*id*/,
    gprs_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs_set_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
void networkmode_set_init(HDC hdc)
{

    read_db_noamal_data(g_hUdbCenter, 0x45001D00, 0, 0, (uint8*)&networkmode, sizeof(networkmode));
}

void networkmode_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;

    char czBuf[300];
    int nTop = 40;
    int nLeft1 = 25;
    int nLeft2 = 120;

    TextOut(hdc, nLeft1, nTop, "��ר�л�ģʽ");

    
    sprintf(czBuf, "%s", g_network_mode[networkmode]);
   
    ITEM_SELECT;
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;
    PopListDraw(hdc);
}


/**********************************************************************
* @name      : 
* @brief     ��ͨѶͨ����������
* @param[in] ��
* @param[out]��
* @return    : 
* @Create    : 
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
void save_networkmode_info()
{
	int ret = 0;
	int rtn = 0;

    OOP_OAD_U networkmodeOad = { 0x45001D00 };
		
    ret = write_normal_data((uint8*)&networkmode, sizeof(networkmode), networkmodeOad, 0, 0, 1);
    if (ret != 0)
    {
        MessageBox("��ר�л�ģʽ����д��ʧ�ܣ�");
		rtn = 1;
    }

	if (rtn == 0)
    {
        MessageBox("��ר�л�ģʽ�������óɹ���");
    }

	return ;
}


//void save_networkmode_info()
//{
//	uint8    dataBuf[1024] = {0};             /* ������ */
//	char sendbuf[100];
//    uint16 revlen;
//
//    sprintf(sendbuf,"AT^SIMSEL=%d\r\n",networkmode);
//	revlen=appmain_trans_AT_with_outtime(1,sendbuf,dataBuf,1024,5);
//    if((dataBuf[0]==0x01)&&(dataBuf[1]==0x06)&&(dataBuf[4]==0x4F)&&(dataBuf[5]==0x4B))
//    {
//        MessageBox("��ר�л�ģʽ�������óɹ���");
//
//    }
//    else
//    {
//        MessageBox("��ר�л�ģʽ�������óɹ���");
//    }
//
//	return ;
//}
BOOL networkmode_set_keymsg(int nKey)
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
            networkmode = nSel;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch (nKey)
    {
        case KEY_UP:
        case KEY_LEFT:
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
	        if (ValidatePwd() == TRUE)
	        {
	            PopList(g_network_mode, 5, g_network_mode[0], 120, 56, FALSE); 
	        }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;   
        }

        /* ����Ҫbreak */
    	case KEY_CANCEL:
        if(TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
    	{
    	    save_networkmode_info();;
    	}
        return FALSE;
    }
    return FALSE;
}

PLUGIN_INFO g_layer_networkmode_set = {
    0x20750002/*id*/,
    networkmode_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    networkmode_set_draw/*Draw*/,
    NULL/*Timer*/,
    networkmode_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO* g_pluginGprs[] =
{
    &g_layer_gprs_set,
};

#endif


#if DESC("���߹����������",1)

void GPRS_param_clean_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;

	RECT rc;
	rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 38;
    rc.bottom = rc.top + 16;
    
    int nLeft = 104;
	SelectFont(hdc, pg_guicomm->fontBig);
	
    DrawText(hdc, "����ͨ�Ų������", strlen("����ͨ�Ų������"), &rc, DT_CENTER | DT_VCENTER);

	ITEM_SELECT;
    TextOut(hdc, nLeft, 70, "���");
	ITEM_NOT_SELECT;

}


/**********************************************************************
* @name      : 
* @brief     ��ͨѶͨ����������
* @param[in] ��
* @param[out]��
* @return    : 
* @Create    : 
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
void GPRS_param_clean()
{
	int ret = 0;
	int rtn = 0;
	CLEAR_DATA_T tData;
	
	memset(&tData, 0, sizeof(CLEAR_DATA_T));

    tData.bClassTag = FALSE;

    tData.recordOAD.classtag = CLASS_DATA_UNINIT;
	tData.recordOAD.road.oadCols.nNum = 1;
	tData.recordOAD.road.oadCols.oad[0].value = 0x45000200;
	
	ret = db_clear_oad_data(g_hUdbCenter, &tData);
    if (ret != 0)
    {
    	GUI_FMT_DEBUG("ɾ������oad %08x  ʧ�� ret %d\n", tData.recordOAD.road.oadCols.oad[0].value,       ret);
        MessageBox("����ͨ�����ò������ʧ�ܣ�");
		rtn = 1;
    }

	tData.recordOAD.road.oadCols.oad[0].value = 0x45000300;
	
	ret = db_clear_oad_data(g_hUdbCenter, &tData);
    if (ret != 0)
    {
    	GUI_FMT_DEBUG("ɾ������oad %08x  ʧ�� ret %d\n", tData.recordOAD.road.oadCols.oad[0].value,       ret);
        MessageBox("������վ���ò������ʧ�ܣ�");
		rtn = 1;
    }

    tData.recordOAD.road.oadCols.oad[0].value = 0x45000E00;
	
	ret = db_clear_oad_data(g_hUdbCenter, &tData);
    if (ret != 0)
    {
    	GUI_FMT_DEBUG("ɾ������oad %08x  ʧ�� ret %d\n", tData.recordOAD.road.oadCols.oad[0].value,       ret);
        MessageBox("�����������ʧ�ܣ�");
		rtn = 1;
    }
    
	if (rtn == 0)
    {
        MessageBox("����ͨ�Ų�������ɹ���");
    }

	return ;
}

BOOL GPRS_param_clean_keymsg(int nKey)
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
	            if(TRUE == MessageBox("�Ƿ�ȷ��������ã�", MB_OKCANCEL))
		    	{
		    	    GPRS_param_clean();
		    	}
	        }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;   
        }

    }
    return FALSE;
}

PLUGIN_INFO g_layer_GPRS_param_clean = {
    0x20750003/*id*/,
    NULL/*InitLayer*/,
    NULL/*UninitLayer*/,
    GPRS_param_clean_draw/*Draw*/,
    NULL/*Timer*/,
    GPRS_param_clean_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif


#if DESC("��̫��",1)

OOP_MASTERPARAMS_T EthMaster;
OOP_ETHCONFIG_T EthConfig;
int gEthernetIndex;
void EthernetInit(HDC hdc)
{
	memset(&EthConfig, 0x00, sizeof(OOP_ETHCONFIG_T));
	memset(&EthMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));
	if(g_chnl_info.nChannel == 0)	//��ͨ��
	{
		read_db_noamal_data(g_hUdbCenter, 0x45100300, 0, 0, (uint8*)&EthMaster, sizeof(OOP_MASTERPARAMS_T));
    	read_db_noamal_data(g_hUdbCenter, 0x45100200, 0, 0, (uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T));
	}
	else
	{
		read_db_noamal_data(g_hUdbCenter, 0x45110300, 0, 0, (uint8*)&EthMaster, sizeof(OOP_MASTERPARAMS_T));
    	read_db_noamal_data(g_hUdbCenter, 0x45110200, 0, 0, (uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T));
	}	
	gEthernetIndex = 0;
}

void ethmaster_set_draw(HDC hdc)
{
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
	int index = 0;

    char czBuf[100];
    sprintf(czBuf, "��̫����ϸ����");

    rc.top = 15;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

    // ��ַ
    int nTop = rc.top+16;
    int nLeft2 = 120;
    int nLeft1 = 25;
    TextOut(hdc, nLeft1, nTop, "��վIP");
    if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
    }
    if(gEthernetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    // �˿�
    index++;
    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "�˿�");
    if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthMaster.master[0].port);
    }
    if(gEthernetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

	index++;
    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "���õ�ַ");
    if (EthMaster.nNum < 2 || EthMaster.master[1].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[1].ip[0], EthMaster.master[1].ip[1], EthMaster.master[1].ip[2], EthMaster.master[1].ip[3]);
    }
    if (gEthernetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "���ö˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthMaster.nNum < 2 || EthMaster.master[1].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthMaster.master[1].port);
    }
    if (gEthernetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

	index++;
    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "����ģʽ");
    if (EthConfig.workmode>2)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_work_mode[EthConfig.workmode]);
    }
    if (gEthernetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "���ӷ�ʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfig.contype>1)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%s", g_conn_type[EthConfig.contype]);
    }
    if (gEthernetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    PopListDraw(hdc);
}

void ethcfg_set_draw(HDC hdc)
{
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
	int index = 6;

    char czBuf[100];
    sprintf(czBuf, "��̫����ϸ����");

    rc.top = 15;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

	// ��ַ
    int nTop = rc.top+16;
    int nLeft2 = 120;
    int nLeft1 = 25;
    TextOut(hdc, nLeft1, nTop, "����ģʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfig.conmode>1)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_conn_app_type[EthConfig.conmode]);
    }
    if (gEthernetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
	
    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����IP");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfig.proxyip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%d.%d.%d.%d", EthConfig.proxyip[0], EthConfig.proxyip[1], EthConfig.proxyip[2], EthConfig.proxyip[3]);
    }
    if (gEthernetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "����˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", EthConfig.proxyport);
    if (gEthernetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "��ʱʱ��");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d ��", EthConfig.dail.mask.dailtime);
    if (gEthernetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "�ط�����");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
    if (gEthernetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 14;
    TextOut(hdc, nLeft1, nTop, "��������");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d ��", EthConfig.heart);
    if (gEthernetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
	PopListDraw(hdc);
}

void EthernetDraw(HDC hdc)
{
   if (gEthernetIndex < 6)//��һҳ
    {
        return ethmaster_set_draw(hdc);
    }
    else    //�ڶ�ҳ
    {
        return ethcfg_set_draw(hdc);
    }
}
void EthMasterReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    switch (gEthernetIndex)
    {
	    case 0: //IP��ַ
	    {
	        if (FALSE == commfun_IP_unpack(pNewStr, tmp))
	        {
	            if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
	            {
	                if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
	                {
	                    strcpy(czBuf, "");
	                }
	                else
	                {
	                    sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
	                }
	                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
	            }
	        }
	        else
	        {
	            memcpy(EthMaster.master[0].ip, tmp, 4);
	            if (EthMaster.nNum < 1)
	                EthMaster.nNum = 1;
	        }
	    }
	    break;
	    case 1:     //�˿�
	    {
	        int port = atoi(pNewStr);
	        if (port <= 65535)
	        {
	            EthMaster.master[0].port = port;
	            if (EthMaster.nNum < 1)
	                EthMaster.nNum = 1;
	        }
	        else
	        {
	            if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
	            {
	                if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
	                {
	                    strcpy(czBuf, "");
	                }
	                else
	                {

	                    sprintf(czBuf, "%d", EthMaster.master[0].port);
	                }
	                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
	            }
	        }
	    }
	    break;
		case 2: //IP��ַ
	    {
	        if (FALSE == commfun_IP_unpack(pNewStr, tmp))
	        {
	            if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
	            {
	                if (EthMaster.nNum < 2 || EthMaster.master[1].ip[0] == 0)
	                {
	                    strcpy(czBuf, "");
	                }
	                else
	                {
	                    sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[1].ip[0], EthMaster.master[1].ip[1], EthMaster.master[1].ip[2], EthMaster.master[1].ip[3]);
	                }
	                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
	            }
	        }
	        else
	        {
	            memcpy(EthMaster.master[1].ip, tmp, 4);
	            if (EthMaster.nNum < 2)
	                EthMaster.nNum = 2;
	        }
	    }
	    break;
	    case 3:     //�˿�
	    {
	        int port = atoi(pNewStr);
	        if (port <= 65535)
	        {
	            EthMaster.master[1].port = port;
	            if (EthMaster.nNum < 2)
	                EthMaster.nNum = 2;
	        }
	        else
	        {
	            if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
	            {
	                if (EthMaster.nNum < 2 || EthMaster.master[1].port == 65535)
	                {
	                    strcpy(czBuf, "");
	                }
	                else
	                {

	                    sprintf(czBuf, "%d", EthMaster.master[1].port);
	                }
	                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
	            }
	        }
	    }
	    break;
    }
}


void EthConfigReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    switch (gEthernetIndex)
    {
    case 7: //�����������ַ
    {
        if (FALSE == commfun_IP_unpack(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthConfig.proxyip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthConfig.proxyip[0], EthConfig.proxyip[1], EthConfig.proxyip[2], EthConfig.proxyip[3]);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(EthConfig.proxyip, tmp, 4);

        }
    }
    break;
    case 8:     //����˿�
    {
        int port = atoi(pNewStr);
        if (port <= 65535)
        {
            EthConfig.proxyport = port;

        }
        else
        {
            if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthConfig.proxyport == 65535)
                sprintf(czBuf, "%d", EthConfig.proxyport);
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
            }
        }
    }
    break;
    case 9:     //��ʱʱ��
    {
        int time = atoi(pNewStr);
        if (time <= 0x3f)      //��ʱʱ��ռ6λ ���0x3f
        {
            EthConfig.dail.mask.dailtime = time;

        }
        else
        {
            if (TRUE == MessageBox("��ʱʱ�����63 \n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                sprintf(czBuf, "%d", EthConfig.dail.mask.dailtime);
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
            }
        }
    }
    break;
    case 10:     //�ط�����
    {
        int dailcnt = atoi(pNewStr);
        if (dailcnt <= 3)      //�ط�����ռ2λ ���0x3
        {
            EthConfig.dail.mask.dailcnt = dailcnt;

        }
        else
        {
            if (TRUE == MessageBox("�ط��������3 \n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
            }
        }
    }
    break;
    case 11:     //��������
    {
        int heart = atoi(pNewStr);
        if (heart <= 65535)      //
        {
            EthConfig.heart = heart;
  
        }
        else
        {
            if (TRUE == MessageBox("������������\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                sprintf(czBuf, "%d", EthConfig.heart);
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
            }
        }
    }
    break;
    }
}

BOOL EthernetKeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int maxindex = 11;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (gEthernetIndex)
            {
            case 4://����ģʽ
            {
                EthConfig.workmode = nSel;

            }
            break;
            case 5://���ӷ�ʽ
            {
                EthConfig.contype = nSel;

            }
            break;
            case 6://����Ӧ�÷�ʽ
            {
                EthConfig.conmode = nSel;

            }
            break;
            default:
                return FALSE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if (gEthernetIndex > 0)
            gEthernetIndex--;
        else
            gEthernetIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (gEthernetIndex < maxindex)
            gEthernetIndex++;
        else
            gEthernetIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (ValidatePwd() == TRUE)
        {
            switch (gEthernetIndex)
            {
            case 4: //����ģʽ
            {
                PopList(g_work_mode, 3, g_work_mode[0], 120, 88, FALSE);
                return TRUE;
            }
            case 5://���ӷ�ʽ
            {
                PopList(g_conn_type, 2, g_conn_type[0], 120, 100, FALSE);
                return TRUE;
            }
            case 6://����Ӧ�÷�ʽ
            {
                PopList(g_conn_app_type, 2, g_conn_app_type[0], 120, 30, FALSE);
                return TRUE;
            }
            case 9://��ʱʱ��
            {
                char czBuf[20];
                sprintf(czBuf, "%d", EthConfig.dail.mask.dailtime);
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 10://�ط�����
            {
                char czBuf[20];
                sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 11://��������
            {
                char czBuf[20];
                sprintf(czBuf, "%d", EthConfig.heart);
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 0://IP��ַ
            {
                char czBuf[20];
                if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
                return TRUE;
            }
            case 1://�˿�
            {
                char czBuf[20];
                if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthMaster.master[0].port);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
                return TRUE;
            }
			case 2://IP��ַ
            {
                char czBuf[20];
                if (EthMaster.nNum < 2 || EthMaster.master[1].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[1].ip[0], EthMaster.master[1].ip[1], EthMaster.master[1].ip[2], EthMaster.master[1].ip[3]);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
                return TRUE;
            }
            case 3://�˿�
            {
                char czBuf[20];
                if (EthMaster.nNum < 2 || EthMaster.master[1].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthMaster.master[1].port);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
                return TRUE;
            }
			case 7://����IP��ַ
            {
                char czBuf[20];
                if (EthConfig.proxyip[0] == 0)
			    {
			        sprintf(czBuf, "-.-.-.-");
			    }
			    else
			    {

			        sprintf(czBuf, "%d.%d.%d.%d", EthConfig.proxyip[0], EthConfig.proxyip[1], EthConfig.proxyip[2], EthConfig.proxyip[3]);
			    }
                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
                return TRUE;
            }
            case 8://�˿�
            {
                char czBuf[20];
                if (EthConfig.proxyport == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.proxyport);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
                return TRUE;
            }
            }
        }
    }
    case KEY_CANCEL:
    if(TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
	{
	    save_chnl_info(1);
	}
    return FALSE;
    }
    return FALSE;
}

PLUGIN_INFO g_layerEthernet =
{
    0x20750000/*id*/,
    EthernetInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    EthernetDraw/*Draw*/,
    NULL/*Timer*/,
    EthernetKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO* g_pluginEthernet[] =
{
    &g_layerEthernet,
};
#endif

/**********************************************************************
* @name      : 
* @brief     ��ͨѶͨ����������
* @param[in] ��
* @param[out]��
* @return    : 
* @Create    : 
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
void save_chnl_info(int index)
{
	int ret = 0;
	int rtn = 0;
	if(index == 0)
    {
    	OOP_OAD_U configOad = { 0x45000200 };
		OOP_OAD_U masterOad = { 0x45000300 };
		OOP_OAD_U netOad = { 0x4500F100 };
		OOP_OAD_U authOad = { 0x4500F200 };
		
        ret = write_normal_data((uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T), masterOad, 0, 0, 1);
	    if (ret != 0)
	    {
	        MessageBox("���߹�����վ����д��ʧ�ܣ�");
			rtn = 1;
	    }
		ret = write_normal_data((uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T), configOad, 0, 0, 1);
	    if (ret != 0)
		{
	        MessageBox("���߹���ͨ�����ò���д��ʧ�ܣ�");
			rtn = 1;
	    }
	    if(gNetTypeSet > 3)
	    {
	        gNetTypeSet = 255;
	    }
	    ret = write_normal_data(&gNetTypeSet, sizeof(uint8), netOad, 0, 0, 1);
	    if (ret != 0)
		{
	        MessageBox("��������д��ʧ�ܣ�");
			rtn = 1;
	    }
	    if(gAuthTypeSet > 3)
	    {
	        gAuthTypeSet = 255;
	    }
	    ret = write_normal_data(&gAuthTypeSet, sizeof(uint8), authOad, 0, 0, 1);
	    if (ret != 0)
		{
	        MessageBox("��֤����д��ʧ�ܣ�");
			rtn = 1;
	    }

		if (rtn == 0)
	    {
	        MessageBox("���߹����������óɹ���");
	    }
    }
	else if(index == 1)
    {
    	OOP_OAD_U ethConfigOad = { 0x45100200 };
		OOP_OAD_U ethMasterOad = { 0x45100300 };
    	
        ret = write_normal_data((uint8*)&EthMaster, sizeof(OOP_MASTERPARAMS_T), ethMasterOad, 0, 0, 1);
	    if (ret != 0)
	    {
	        MessageBox("��̫����վ����д��ʧ�ܣ�");
			rtn = 1;
	    }
		ret = write_normal_data((uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T), ethConfigOad, 0, 0, 1);
	    if (ret != 0)
		{
	        MessageBox("��̫��ͨ�����ò���д��ʧ�ܣ�");
			rtn = 1;
	    }

		if (rtn == 0)
	    {
	        MessageBox("��̫���������óɹ���");
	    }
    }

	return ;
	
}


///**********************************************************************
//* @name      : 
//* @brief     ��ͨѶͨ����������
//* @param[in] ��
//* @param[out]��
//* @return    : 
//* @Create    : 
//* @Date      ��2019-12-16
//* @Update    :
//**********************************************************************/
//void save_chnl_info(void)
//{
//	int ret = 0;
//	int rtn = 0;
//	if(g_chnl_info.nType == 0)
//    {
//    	OOP_OAD_U configOad = { 0x45000200 };
//		OOP_OAD_U masterOad = { 0x45000300 };
//    	if(g_chnl_info.nChannel == 1)	//����ͨ��
//		{
//			configOad.value = 0x45010200;
//			masterOad.value = 0x45010300;
//		}	
//        ret = write_normal_data((uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T), masterOad, 0, 0, 1);
//	    if (ret != 0)
//	    {
//	        MessageBox("���߹�����վ����д��ʧ�ܣ�");
//			rtn = 1;
//	    }
//		ret = write_normal_data((uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T), configOad, 0, 0, 1);
//	    if (ret != 0)
//		{
//	        MessageBox("���߹���ͨ�����ò���д��ʧ�ܣ�");
//			rtn = 1;
//	    }
//
//		if (rtn == 0)
//	    {
//	        MessageBox("���߹����������óɹ���");
//	    }
//    }
//	else if(g_chnl_info.nType == 2)
//    {
//    	OOP_OAD_U ethConfigOad = { 0x45100200 };
//		OOP_OAD_U ethMasterOad = { 0x45100300 };
//    	if(g_chnl_info.nChannel == 1)	//����ͨ��
//		{
//			ethConfigOad.value = 0x45110200;
//			ethMasterOad.value = 0x45110300;
//		}	
//        ret = write_normal_data((uint8*)&EthMaster, sizeof(OOP_MASTERPARAMS_T), ethMasterOad, 0, 0, 1);
//	    if (ret != 0)
//	    {
//	        MessageBox("��̫����վ����д��ʧ�ܣ�");
//			rtn = 1;
//	    }
//		ret = write_normal_data((uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T), ethConfigOad, 0, 0, 1);
//	    if (ret != 0)
//		{
//	        MessageBox("��̫��ͨ�����ò���д��ʧ�ܣ�");
//			rtn = 1;
//	    }
//
//		if (rtn == 0)
//	    {
//	        MessageBox("��̫���������óɹ���");
//	    }
//    }
//
//	return ;
//}

//void ChnlInfoInit(HDC hdc)
//{
//    g_guiComm.bCanTrunDisp = FALSE;
//    g_chnl_info.nIndex = 0;
//    g_chnl_info.nChannel = 0;	// ֻ��Gprsʱ����
//
////	memset(&EthIP, 0x00, sizeof(OOP_ETHIP_T));
////    read_db_noamal_data(g_hUdbCenter, EthIPOad.value, 0, 0, (uint8*)&EthIP, sizeof(OOP_ETHIP_T));
//
////    memset(&GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T));
////    read_db_noamal_data(g_hUdbCenter, GPRSConfigOad.value, 0, 0, (uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T));
//    g_chnl_info.nType = 0;
//    g_chnl_info.nMode = 1;
//}
//
//void ChnlInfoDraw(HDC hdc)
//{
//    // ��ϸ���û���
//    if(g_chnl_info.pCurPlugin != NULL)
//    {
//        ASSERT(g_chnl_info.pCurPlugin->pfnDraw != NULL);
//        g_chnl_info.pCurPlugin->pfnDraw(hdc);
//        return;
//    }
//
//    ITEM_NOT_SELECT;
//    SelectFont(hdc,g_guiComm.fontBig);
//
//    RECT rc;
//    rc.left = 0;
//    rc.right = g_guiComm.nWidth;
//
//    char czBuf[100];
//    sprintf(czBuf, "�ն˵�ǰ����ͨ����Ϣ");
//
//    rc.top = 20;
//    rc.bottom = rc.top+16;
//    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
//
//    // �ŵ�����
//    int nTop = rc.top + 20;
//    int nLeft1 = 24;
//    int nLeft2 = 130;
//    TextOut(hdc, nLeft1, nTop, "�ŵ�����");
//    if(g_chnl_info.nIndex == 0)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, g_pczChnlType[g_chnl_info.nType]);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, g_pczChnlType[g_chnl_info.nType]);
//    }
//
//    nTop += 16;
//    int nIndex = 1;
//
//    TextOut(hdc, nLeft1, nTop, "ͨ��ģʽ");
//    if(g_chnl_info.nMode == -1)
//    {
//        sprintf(czBuf, "-");
//    }
//    else
//    {
//        sprintf(czBuf, "%s", g_czWorkMode[g_chnl_info.nMode]);
//    }
//    if(g_chnl_info.nIndex == nIndex)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, czBuf);
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, czBuf);
//    }
//    nIndex ++;
//
//    nTop += 16;
//    if(g_chnl_info.nType == 0 || g_chnl_info.nType == 2)
//    {
//        TextOut(hdc, nLeft1, nTop, "��ѡ��ͨ��");
//        if(g_chnl_info.nIndex == nIndex)
//        {
//            ITEM_SELECT;
//            TextOut(hdc, nLeft2, nTop, g_pczChnlName[g_chnl_info.nChannel]);
//            ITEM_NOT_SELECT;
//        }
//        else
//        {
//            TextOut(hdc, nLeft2, nTop, g_pczChnlName[g_chnl_info.nChannel]);
//        }
//        nIndex ++;
//    }
//    else
//    {
//        g_chnl_info.nChannel = 0;
//    }
//
//    nTop += 18;
//    // ��ϸ����
//    if(g_chnl_info.nIndex == nIndex)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, "��ϸ����");
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, "��ϸ����");
//    }
//    nIndex ++;
//
//    nTop += 18;
//    if(g_chnl_info.nIndex == nIndex)
//    {
//        ITEM_SELECT;
//        TextOut(hdc, nLeft2, nTop, "��������");
//        ITEM_NOT_SELECT;
//    }
//    else
//    {
//        TextOut(hdc, nLeft2, nTop, "��������");
//    }
//    PopListDraw(hdc);
//}
//
//BOOL ChnlInfoKeyMsg(int nKey)
//{
//	int index = 0;
//	if(g_chnl_info.nType == 0 || g_chnl_info.nType == 2)
//	{
//		index = 3;
//	}
//	else
//	{
//		index = 2;
//	}
//    // ��ϸ����ҳ��ⰴ����Ϣ����
//    if(g_chnl_info.pCurPlugin != NULL)
//    {
//        BOOL bRet = FALSE;
//        if(g_chnl_info.pCurPlugin->pfnKeyMsg != NULL)
//        {
//            bRet = g_chnl_info.pCurPlugin->pfnKeyMsg(nKey);
//        }
//        // ȱʡ����
//        if((bRet == FALSE && nKey==KEY_CANCEL) || bRet == RET_CANCEL)
//        {
//            // �˳�ҳ���
//            if(g_chnl_info.pCurPlugin->pfnUninitLayer != NULL)
//            {
//                g_chnl_info.pCurPlugin->pfnUninitLayer();
//            }
//            g_chnl_info.pCurPlugin = NULL;
//            g_guiComm.nArrow = -1;
//            UpdateWindow(g_guiComm.hMainWnd, TRUE);
//        }
//        if(bRet == FALSE)
//        {
//            if(nKey==KEY_UP)
//            {
//                if(g_chnl_info.nPluginCnt > 1)
//                {
//                    g_guiComm.bKeyDown = FALSE;
//                    if(g_chnl_info.pCurPlugin->pfnUninitLayer != NULL)
//                    {
//                        g_chnl_info.pCurPlugin->pfnUninitLayer();
//                    }
//                    g_chnl_info.pCurPlugin = NULL;
//
//                    g_chnl_info.nCurPlugin--;
//                    if(g_chnl_info.nCurPlugin < 0)
//                    {
//                        g_chnl_info.nCurPlugin = g_chnl_info.nPluginCnt-1;
//                    }
//                    g_chnl_info.pCurPlugin = g_chnl_info.ppDetail[g_chnl_info.nCurPlugin];
//                    ASSERT(g_chnl_info.pCurPlugin != NULL);
//
//                    if(g_chnl_info.pCurPlugin->pfnInitLayer != NULL)
//                    {
//                        g_chnl_info.pCurPlugin->pfnInitLayer(g_chnl_info.pCurPlugin->dwId);
//                    }
//
//                    if(g_chnl_info.nCurPlugin > 0 && g_chnl_info.nCurPlugin < g_chnl_info.nPluginCnt-1)
//                    {
//                        g_guiComm.nArrow = 0;
//                    }
//                    else if(g_chnl_info.nCurPlugin > 0)
//                    {
//                        g_guiComm.nArrow = 1;
//                    }
//                    else if(g_chnl_info.nCurPlugin < g_chnl_info.nPluginCnt-1)
//                    {
//                        g_guiComm.nArrow = 2;
//                    }
//                    else
//                    {
//                        g_guiComm.nArrow = -1;
//                    }
//                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
//                }
//            }
//            else if(nKey==KEY_DOWN)
//            {
//                if(g_chnl_info.nPluginCnt > 1)
//                {
//                    g_guiComm.bKeyDown = TRUE;
//                    if(g_chnl_info.pCurPlugin->pfnUninitLayer != NULL)
//                    {
//                        g_chnl_info.pCurPlugin->pfnUninitLayer();
//                    }
//                    g_chnl_info.pCurPlugin = NULL;
//
//                    g_chnl_info.nCurPlugin++;
//                    if(g_chnl_info.nCurPlugin >= g_chnl_info.nPluginCnt)
//                    {
//                        g_chnl_info.nCurPlugin = 0;
//                    }
//                    g_chnl_info.pCurPlugin = g_chnl_info.ppDetail[g_chnl_info.nCurPlugin];
//                    ASSERT(g_chnl_info.pCurPlugin != NULL);
//
//                    if(g_chnl_info.pCurPlugin->pfnInitLayer != NULL)
//                    {
//                        g_chnl_info.pCurPlugin->pfnInitLayer(g_chnl_info.pCurPlugin->dwId);
//                    }
//
//                    if(g_chnl_info.nCurPlugin > 0 && g_chnl_info.nCurPlugin < g_chnl_info.nPluginCnt-1)
//                    {
//                        g_guiComm.nArrow = 0;
//                    }
//                    else if(g_chnl_info.nCurPlugin > 0)
//                    {
//                        g_guiComm.nArrow = 1;
//                    }
//                    else if(g_chnl_info.nCurPlugin < g_chnl_info.nPluginCnt-1)
//                    {
//                        g_guiComm.nArrow = 2;
//                    }
//                    else
//                    {
//                        g_guiComm.nArrow = -1;
//                    }
//                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
//                }
//            }
//        }
//        return TRUE;
//    }
//
//    // ����Listѡ����
//    if(g_guiComm.bPopList == TRUE)
//    {
//        int nSel;
////        char czBuf[50];
////        int nRet;
//        PopListKeyMsg(nKey, &nSel);
//        if(nKey == KEY_OK)
//        {
//            switch(g_chnl_info.nIndex)
//            {
//            case 0:// ͨ������
//                g_chnl_info.nType = nSel;
//                break;
//            case 1:
//                g_chnl_info.nMode = nSel;
//                break;
//            case 2:// ͨ��ѡ��
//                g_chnl_info.nChannel = nSel;
//                break;
//            }
//            UpdateWindow(g_guiComm.hMainWnd, TRUE);
//        }
//        return TRUE;
//    }
//    // ��ǰҳ��ⰴ������
//    switch(nKey)
//    {
//    case KEY_UP:
//        g_chnl_info.nIndex --;
//        if(g_chnl_info.nIndex < 0)
//        {
//            g_chnl_info.nIndex = 0;
//            return FALSE;
//        }
//        UpdateWindow(g_guiComm.hMainWnd, TRUE);
//        return TRUE;
//
//    case KEY_DOWN:
//        {
//            int nMax = (g_chnl_info.nType!=1)?4:3;
//            g_chnl_info.nIndex ++;
//            if(g_chnl_info.nIndex > nMax)
//            {
//                g_chnl_info.nIndex = nMax;
//                return FALSE;
//            }
//        }
//        UpdateWindow(g_guiComm.hMainWnd, TRUE);
//        return TRUE;
//
//    case KEY_CANCEL:
//        if(TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
//		{
//		    save_chnl_info();;
//		}
//        return FALSE;
//
//    case KEY_OK:
//        if(ValidatePwd() == TRUE)
//        {
//            
//            if(g_chnl_info.nIndex == 0)
//            {
//                // ����ͨ������ѡ��List
//                PopList(g_pczChnlType, sizeof(g_pczChnlType) / sizeof(g_pczChnlType[0]), g_pczChnlType[g_chnl_info.nType], 130, 16, TRUE);
//            }
//			else if(g_chnl_info.nIndex == 1)
//			{
//                PopList(g_czWorkMode, 3, g_czWorkMode[g_chnl_info.nMode], 130, 56, TRUE);
//			}
//			else if((g_chnl_info.nType == 0 || g_chnl_info.nType == 2) && g_chnl_info.nIndex == 2)
//			{
//                // ����ͨ����ѡ��ֻ���gprs����̫��
//                PopList(g_pczChnlName, 2, g_pczChnlName[g_chnl_info.nChannel], 130, 72, TRUE);
//			}
//            else if(g_chnl_info.nIndex == index)
//            {
//				// ������ϸ����ҳ��
//                // ����ͨ�����ͣ�ѡ���Ӧ��ҳ���
//                switch(g_chnl_info.nType)
//                {
//                case 0://GPRS
//                    g_chnl_info.ppDetail = g_pluginGprs;
//                    g_chnl_info.nPluginCnt = sizeof(g_pluginGprs)/sizeof(g_pluginGprs[0]);
//                    break;
//                case 1://Radio
//                    g_wireless_info.nType = 0;
//                    g_chnl_info.ppDetail = g_pluginWireless;
//                    g_chnl_info.nPluginCnt = sizeof(g_pluginWireless)/sizeof(g_pluginWireless[0]);
//                    break;
//                case 2://Ethernet
//                    g_chnl_info.ppDetail = g_pluginEthernet;
//                    g_chnl_info.nPluginCnt = sizeof(g_pluginEthernet)/sizeof(g_pluginEthernet[0]);
//                    break;
//                default:
//                    ASSERT(FALSE);
//                }
//                // ��ʼ����ϸ����ҳ���
//                g_chnl_info.nCurPlugin = 0;
//                g_chnl_info.pCurPlugin = g_chnl_info.ppDetail[g_chnl_info.nCurPlugin];
//                ASSERT(g_chnl_info.pCurPlugin != NULL);
//                if(g_chnl_info.nPluginCnt > 1)
//                {
//                    g_guiComm.nArrow = 2;
//                }
//
//                g_guiComm.bKeyDown = TRUE;
//                if(g_chnl_info.pCurPlugin->pfnInitLayer != NULL)
//                {
//                    g_chnl_info.pCurPlugin->pfnInitLayer(g_chnl_info.pCurPlugin->dwId);
//                }
//                UpdateWindow(g_guiComm.hMainWnd, TRUE);
//            }
//            else if(g_chnl_info.nIndex == index+1)
//            {
//				save_chnl_info();
//                g_chnl_info.nIndex = 0;
//            }
//            return TRUE;
//        }
//    }
//    return FALSE;
//}
//
//
//PLUGIN_INFO g_layerChnlInfo =
//{
//    0x20750000/*id*/,
//    ChnlInfoInit/*InitLayer*/,
//    NULL/*UninitLayer*/,
//    ChnlInfoDraw/*Draw*/,
//    NULL/*Timer*/,
//    ChnlInfoKeyMsg/*keymsg*/,
//    NULL/*ipc*/
//};
#endif


#if DESC("�ն˵�ַ���ã�IP����", 1)
int gAddressSetIndex;
int gAddressSetState;
TERMINAL_ADDR g_terminal_addr;
OOP_ETHIP_T EthIP;      //�ն�ip
int EthIPisSet;



//���ַ�����ʽ��ipת��Ϊ���� ʧ�ܷ���false
BOOL commfun_IP(char* ipin, uint8* ipout)
{
    if (ipin == NULL || strlen(ipin) == 0)
	{
    	return FALSE;
	}
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
    for (int i =0;i<4;i++){
        if(atoi(p1[i]) > 255)
            return FALSE;
        ipout[i] = atoi(p1[i]);
    }
    return TRUE;
}

//void IpConfigReturn(char* pNewStr, void* pPara)
//{
//    char czBuf[20] = { 0 };
//    uint8 tmp[4] = { 0 };
//
//    switch (EthIPIndex)
//    {
//    case 1: //ip��ַ
//    {
//        if (FALSE == commfun_IP(pNewStr, tmp))
//        {
//            if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
//            {
//                if (EthIPisSet < 0 || EthIP.ip[0] == 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.ip[0], EthIP.ip[1], EthIP.ip[2], EthIP.ip[3]);
//                }
//                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
//            }
//        }
//        else
//        {
//            memcpy(EthIP.ip, tmp, 4);
//            EthIPisSet = 1;
//        }
//    }
//    break;
//    case 2://��������
//    {
//        if (FALSE == commfun_IP(pNewStr, tmp))
//        {
//            if (TRUE == MessageBox("���������ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
//            {
//                if (EthIPisSet < 0 || EthIP.mask[0] == 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.mask[0], EthIP.mask[1], EthIP.mask[2], EthIP.mask[3]);
//                }
//                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
//            }
//        }
//        else
//        {
//            memcpy(EthIP.mask, tmp, 4);
//            EthIPisSet = 1;
//        }
//    }
//    break;
//    case 3://���ص�ַ
//    {
//        if (FALSE == commfun_IP(pNewStr, tmp))
//        {
//            if (TRUE == MessageBox("���ص�ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
//            {
//                if (EthIPisSet < 0 || EthIP.ipGateway[0] == 0)
//                {
//                    strcpy(czBuf, "");
//                }
//                else
//                {
//                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.ipGateway[0], EthIP.ipGateway[1], EthIP.ipGateway[2], EthIP.ipGateway[3]);
//                }
//                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
//            }
//        }
//        else
//        {
//            memcpy(EthIP.ipGateway, tmp, 4);
//            EthIPisSet = 1;
//        }
//    }
//    break;
//    case 4:
//    {
//        EthIP.pppoeUser.nNum = strlen(pNewStr);
//        strcpy(EthIP.pppoeUser.value, pNewStr);
//        EthIPisSet = 1;
//    }
//    break;
//    case 5:
//    {
//        EthIP.pppoePwd.nNum = strlen(pNewStr);
//        strcpy(EthIP.pppoePwd.value, pNewStr);
//        EthIPisSet = 1;
//    }
//    break;
//    }
//
//}
//
uint32 g_GWIP[4] = {0};

KEYPRESS_FIR keyState;
void address_set_init(HDC hdc)
{
	memset(&g_terminal_addr, 0x00, sizeof(TERMINAL_ADDR));
    memset(&keyState, 0x00, sizeof(KEYPRESS_FIR));
    get_addr(LocalAddr);
	memset(&EthIP, 0x00, sizeof(OOP_ETHIP_T));
	EthIP.mask[0] = 255;
	EthIP.mask[1] = 255;
	EthIP.mask[2] = 255;
    EthIPisSet = read_db_noamal_data(g_hUdbCenter, EthIPOad.value, 0, 0, (uint8*)&EthIP, sizeof(OOP_ETHIP_T));
	g_terminal_addr.nIndex = 0;
	g_terminal_addr.nState = 0;

	read_pdata_xram("/data/app/desktopGui/data/radio", (char*)&g_terminal_addr.czCode, 0, 2);
	read_pdata_xram("/data/app/desktopGui/data/radio", (char*)&g_terminal_addr.czAddr, 2, 3);

    char cmd[200];
    char result[100];
    memset(cmd,0x00,sizeof(cmd));
    memset(result,0x00,sizeof(result));
    memset(g_GWIP,0x00,sizeof(g_GWIP));
    sprintf(cmd, "ifconfig %s | grep 'inet addr' | awk '{ print $2}' | awk -F: '{print $2}' ", "ppp0");
    cmd_execute(cmd,result);
    sscanf(result,"%u.%u.%u.%u",(uint32*)&g_GWIP[0],(uint32*)&g_GWIP[1],(uint32*)&g_GWIP[2],(uint32*)&g_GWIP[3]);
}

void address_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
	SelectFont(hdc,g_guiComm.fontSmall);

    // ��������
    char czBuf[100];
    int nLeft = 100;
    int nTop = 20;
	sprintf(czBuf, "%s", (g_terminal_addr.nState == 0)?"��ʾģʽ":"����ģʽ");
    if(g_terminal_addr.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, 60, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 60, nTop, czBuf);
    }
	
    // �ն˵�ַ
    nTop += 20;
    /**
    TextOut(hdc, 20, nTop, "��������");
	sprintf(czBuf, "%02x%02x", g_terminal_addr.czCode[0], g_terminal_addr.czCode[1]);
    if(g_terminal_addr.nState == 1 && g_terminal_addr.nIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, czBuf);
    }

    // �ն˵�ַ
    nTop += 15;
    TextOut(hdc, 20, nTop, "�ն˵�ַ");
	sprintf(czBuf, "%02x%02x%02x", g_terminal_addr.czAddr[0], g_terminal_addr.czAddr[1], g_terminal_addr.czAddr[2]);
    if(g_terminal_addr.nState == 1 && g_terminal_addr.nIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, czBuf);
    }
    **/
    
	nTop += 15;
    TextOut(hdc, 20, nTop, "OOP�ն˵�ַ");
#ifdef AREA_ZHEJIANG
	sprintf(g_terminal_addr.oopAddr, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
#else
	sprintf(g_terminal_addr.oopAddr, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
#endif
	nTop += 15;
    if (g_terminal_addr.nState == 1 && g_terminal_addr.nIndex == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, g_terminal_addr.oopAddr);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, g_terminal_addr.oopAddr);
    }

	nTop += 15;

    // �ն˵�ַ
    nTop += 15;
    TextOut(hdc, 20, nTop, "����IP");
	if (g_GWIP[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%d.%d.%d.%d", g_GWIP[0], g_GWIP[1], g_GWIP[2], g_GWIP[3]);
    }

    TextOut(hdc, nLeft, nTop, czBuf);
}

void AddrReturn(char* pNewStr, void* pPara)
{
    int nLen = strlen(pNewStr);
    int ret;


	switch(g_terminal_addr.nIndex)
    {
        case 1:
        	{
			if (nLen > 4 && TRUE == MessageBox("�������볤�ȴ���2�ֽ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
		    {
		        char tmp[20] = { 0 };
		        sprintf(tmp, "%02x%02x", g_terminal_addr.czCode[0], g_terminal_addr.czCode[1]);
		        InputBox(tmp, AddrReturn, NULL, 0x01);
		    }
		    else
		    {
		        char tmp[20] = { 0 };
		        strcpy(tmp, "0000");
		        strcpy(tmp + 4 - nLen, pNewStr);
		        printf("����ĵ�ַΪ %s \n", tmp);
		        commfun_ASC2BCD((char*)g_terminal_addr.czCode, tmp, 4, 0);
		        printf("ת��Ϊbcd %02x%02x", g_terminal_addr.czCode[0], g_terminal_addr.czCode[1]);
		    }
        	break;
        	}
		case 2:
			{
			if (nLen > 6 && TRUE == MessageBox("�ն˳��ȴ���3�ֽ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
		    {
		        char tmp[20] = { 0 };
		        sprintf(tmp, "%02x%02x%02x", g_terminal_addr.czAddr[0], g_terminal_addr.czAddr[1], g_terminal_addr.czAddr[2]);
		        InputBox(tmp, AddrReturn, NULL, 0x01);
		    }
		    else
		    {
		        char tmp[20] = { 0 };
		        strcpy(tmp, "000000");
		        strcpy(tmp + 6 - nLen, pNewStr);
		        printf("����ĵ�ַΪ %s \n", tmp);
		        commfun_ASC2BCD((char*)g_terminal_addr.czAddr, tmp, 6, 0);
		        printf("ת��Ϊbcd %02x%02x%02x", g_terminal_addr.czAddr[0], g_terminal_addr.czAddr[1], g_terminal_addr.czAddr[2]);
		    }
        	break;
			}
		case 3:
        	{
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
        	break;
        	}
	}
}

void IpReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    if (FALSE == commfun_IP(pNewStr, tmp))
    {
        if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
        {
            if (EthIPisSet < 0 || EthIP.ip[0] == 0)
            {
                strcpy(czBuf, "");
            }
            else
            {
                sprintf(czBuf, "%d.%d.%d.%d", EthIP.ip[0], EthIP.ip[1], EthIP.ip[2], EthIP.ip[3]);
            }
            InputBox(czBuf, IpReturn, NULL, 0x02);
        }
    }
}
#ifdef AREA_CHONGQING
BOOL address_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;


    if(g_terminal_addr.nState == 1)
    {
        switch (nKey)
        {
            case KEY_UP:
                g_terminal_addr.nIndex --;
                if(g_terminal_addr.nIndex < 0)
                {
                    g_terminal_addr.nIndex = 3;
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            case KEY_DOWN:
                g_terminal_addr.nIndex ++;
                if(g_terminal_addr.nIndex > 3)
                {
                    g_terminal_addr.nIndex = 0;
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            case KEY_OK:
            {
                if (ValidatePwd() == TRUE)
                {
                    // ȷ�ϴ���
                    switch(g_terminal_addr.nIndex)
                    {
                        case 1:
                            {
                            char tmp[20] = {0};
                            sprintf(tmp, "%02x%02x", g_terminal_addr.czCode[0], g_terminal_addr.czCode[1]);
                            InputBox(tmp, AddrReturn, NULL, 0x01);
                            break;
                            }
                        case 2:
                            {
                            char tmp[20] = {0};
                            sprintf(tmp, "%02x%02x%02x", g_terminal_addr.czAddr[0], g_terminal_addr.czAddr[1], g_terminal_addr.czAddr[2]);
                        
                            InputBox(tmp, AddrReturn, NULL, 0x01);
                            break;
                            }
                        case 3:
                            {
                            char tmp[20] = {0};
                            sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
                            InputBox(tmp, AddrReturn, NULL, 0x01);
                            break;
                            }
                        default: break;
                    }
                    return TRUE;
                }
                break;
            }
             case KEY_CANCEL:
                 if(TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
                {
                    addr_send_to_radio((uint8*)g_terminal_addr.czCode, (uint8*)g_terminal_addr.czAddr, LocalAddr);
                    write_pdata_xram("/data/app/desktopGui/data/radio", (char*)&g_terminal_addr.czCode, 0, 2);
                    write_pdata_xram("/data/app/desktopGui/data/radio", (char*)&g_terminal_addr.czAddr, 2, 3);
                    MessageBox("�ն˵�ַ���óɹ�");
                }
                return FALSE;
     
            default: break;
        }
    }
    else
    {
        g_terminal_addr.nIndex = 0;
        switch (nKey)
        {
            case KEY_UP:
                if (keyState.upValue < 2)
                    keyState.upValue++;
                else
                    keyState.upValue = 0;
                keyState.dwValue = 0;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            case KEY_DOWN:
                if (keyState.dwValue < 2)
                    keyState.dwValue++;
                else
                    keyState.dwValue = 0;
                if(keyState.dwValue == 2 && keyState.upValue == 2)
                    g_terminal_addr.nState = 1;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            default: break;
        }
    }
    return FALSE;
}
#else
BOOL address_set_keymsg(int nKey)
{

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
        case KEY_UP:
	        if(g_terminal_addr.nState == 1)
	        {
		        if(g_terminal_addr.nIndex == 0)
		        {
		            g_terminal_addr.nIndex = 3;
		        }
                else
                {
                    g_terminal_addr.nIndex = 0;
                }
	        }
	        UpdateWindow(g_guiComm.hMainWnd, TRUE);
	        return TRUE;
		case KEY_DOWN:
		    if(g_terminal_addr.nState == 0)
		    {
		        g_terminal_addr.nIndex = 0;
		    }
		    else
		    {
		        if(g_terminal_addr.nIndex == 0)
		        {
		            g_terminal_addr.nIndex = 3;
		        }
                else
                {
                    g_terminal_addr.nIndex = 0;
                }
		    }
		    UpdateWindow(g_guiComm.hMainWnd, TRUE);
		    return TRUE;
    	case KEY_OK:
        {
            if (ValidatePwd() == TRUE)
            {
            	// ȷ�ϴ���
	            switch(g_terminal_addr.nIndex)
	            {
		            case 0:
		                if(g_terminal_addr.nState == 0)
		                {
		                    g_terminal_addr.nState = 1;
		                }
		                else
		                {
		                    g_terminal_addr.nState = 0;
		                }
		                UpdateWindow(g_guiComm.hMainWnd, TRUE);
		                break;
		                // �����ն˵�ַ����ҳ���
		            case 1:
		            	{
		                char tmp[20] = {0};
						sprintf(tmp, "%02x%02x", g_terminal_addr.czCode[0], g_terminal_addr.czCode[1]);
		                InputBox(tmp, AddrReturn, NULL, 0x01);
	                	break;
		            	}
					case 2:
						{
						char tmp[20] = {0};
						sprintf(tmp, "%02x%02x%02x", g_terminal_addr.czAddr[0], g_terminal_addr.czAddr[1], g_terminal_addr.czAddr[2]);
	                
	                	InputBox(tmp, AddrReturn, NULL, 0x01);
						break;
						}
					case 3:
		            	{
		                char tmp[20] = {0};
#ifdef AREA_ZHEJIANG
		                sprintf(tmp, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
#else
		                sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
#endif
		                InputBox(tmp, AddrReturn, NULL, 0x01);
	                	break;
		            	}
					default: break;
				}
            	return TRUE;
            }
            break;
        }
		 case KEY_CANCEL:
		 	if(TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
			{
			    addr_send_to_radio((uint8*)g_terminal_addr.czCode, (uint8*)g_terminal_addr.czAddr, LocalAddr);
				write_pdata_xram("/data/app/desktopGui/data/radio", (char*)&g_terminal_addr.czCode, 0, 2);
				write_pdata_xram("/data/app/desktopGui/data/radio", (char*)&g_terminal_addr.czAddr, 2, 3);
				MessageBox("�ն˵�ַ���óɹ�");
			}
	        return FALSE;

        default: break;
    }
    return FALSE;
}
#endif
//ͨ�ŵ�ַ����
PLUGIN_INFO g_layer_address_set = {
    0x20720000/*id*/,
    address_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    address_set_draw/*Draw*/,
    NULL/*Timer*/,
    address_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

void address_set_draw1(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    SelectFont(hdc,g_guiComm.fontSmall);

    // ��������
    char czBuf[100];
    int nLeft = 100;
    int nTop = 20;
    ITEM_NOT_SELECT;

    // �ն˵�ַ
    nTop += 20;
    TextOut(hdc, 20, nTop, "��������");
    sprintf(czBuf, "%02x%02x", g_terminal_addr.czCode[0], g_terminal_addr.czCode[1]);
    TextOut(hdc, nLeft, nTop, czBuf);


    // �ն˵�ַ
    nTop += 15;
    TextOut(hdc, 20, nTop, "�ն˵�ַ");
    sprintf(czBuf, "%02x%02x%02x", g_terminal_addr.czAddr[0], g_terminal_addr.czAddr[1], g_terminal_addr.czAddr[2]);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, 20, nTop, "OOP�ն˵�ַ");
    sprintf(g_terminal_addr.oopAddr, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
    TextOut(hdc, nLeft, nTop, g_terminal_addr.oopAddr);

    nTop += 15;

    // �ն˵�ַ
    nTop += 15;
    TextOut(hdc, 20, nTop, "����IP");
    if (g_GWIP[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", g_GWIP[0], g_GWIP[1], g_GWIP[2], g_GWIP[3]);
    }

    TextOut(hdc, nLeft, nTop, czBuf);
}

//ͨ�ŵ�ַ����
PLUGIN_INFO g_layer_address_set1 = {
    0x20720001/*id*/,
    address_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    address_set_draw1/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("���������", 1)

uint8 measure_set_index;


//���˿�OADת��Ϊ����
void get_portname(OOP_OAD_U port, char* name)
{
    switch (port.nObjID)
    {
    case 0xF209:
    {
        if (port.attID == 2)
        {
            sprintf(name, "�ز�-%d", port.nIndex);
            return;
        }
    }
    break;
    case 0xF201:
    {
        if (port.attID == 2)
        {
            sprintf(name, "485-%d", port.nIndex);
            return;
        }
    }
    break;
    case 0xF208:
    {
        sprintf(name, "���ɶ˿�");
        return;
    }
    case 0xF20A:
    {
        if (port.attID == 2)
        {
            sprintf(name, "����-%d", port.nIndex);
            return;
        }
    }
    default:
        break;
    }
    sprintf(name, "-");

}

//���õ������ַ
void MeterAddrReturn(char* pNewStr, void* pPara)
{
    int nLen = strlen(pNewStr);
    uint8 Addr[20] = { 0 };
    char tmp[20] = { 0 };
	float tmpe = 0.0;
	
	switch(measure_set_index)
	{
	case 4:
	    if (nLen > 12 && TRUE == MessageBox("���õ���ַ���ȴ���6�ֽ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
	    {

	        memcpy(Addr, MeterFile.basic.tsa.add, 6);
	        sprintf(tmp, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
	        InputBox(tmp, MeterAddrReturn, NULL, 0x01);
	    }
	    else
	    {

	        strcpy(tmp, "000000000000");
	        strcpy(tmp + 12 - nLen, pNewStr);
	        printf("����ĵ�ַΪ %s \n", tmp);
	        commfun_ASC2BCD((char*)Addr, tmp, 12, 0);
	        printf("ת��Ϊbcd %02x%02x%02x%02x%02x%02x \n", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
	        MeterFile.basic.tsa.len = 5;
	        memcpy(MeterFile.basic.tsa.add, Addr, 6);
	//      memcpy_rev(MeterFile.basic.tsa.add, Addr, 6);
	        MeterFile.nIndex = g_guiComm.nCurPoint;
	    }
		break;

	case 6:
		if(FALSE == CheckFloat(pNewStr, 10000))
        {
            if(MessageBox("���ѹ���벻��ȷ\n\nȷ���Ƿ���������?", MB_OKCANCEL) == FALSE)
            {
                return;
            }
			
            tmpe = (float)MeterFile.basic.stVolt/10;
        	sprintf(tmp, "%2.1f", tmpe);
            InputBox(tmp, MeterAddrReturn, NULL, 0x02);
        }
        else
        {
            tmpe = (float)atof(pNewStr);
			MeterFile.basic.stVolt = (uint16)(tmpe*10);
        }
        break;
	case 7:
		if(FALSE == CheckFloat(pNewStr, 10000))
        {
            if(MessageBox("��������벻��ȷ\n\nȷ���Ƿ���������?", MB_OKCANCEL) == FALSE)
            {
                return;
            }
			
            tmpe = (float)MeterFile.basic.stAmp/10;
        	sprintf(tmp, "%2.1f", tmpe);
            InputBox(tmp, MeterAddrReturn, NULL, 0x02);
        }
        else
        {
            tmpe = (float)atof(pNewStr);
			MeterFile.basic.stAmp = (uint16)(tmpe*10);
        }
        break;
	}
	
}


OOP_OAD_U get_set_pipe(int index)
{
    OOP_OAD_U oad;
    memset(&oad, 0x00, sizeof(OOP_OAD_U));
    switch (index)
    {
    case 0:
        oad.value = 0xF2010201;
        break;
    case 1:
        oad.value = 0xF2010202;
        break;
    case 2:
        oad.value = 0xF20A0201;
        break;
    case 3:
        oad.value = 0xF20A0202;
        break;
    case 4:
        oad.value = 0xF20A0203;
        break;
    case 5:
        oad.value = 0xF20A0204;
        break;
    case 6:
        oad.value = 0xF20A0205;
        break;
    case 7:
        oad.value = 0xF20A0206;
        break;
    case 8:
        oad.value = 0xF20A0207;
        break;
    case 9:
        oad.value = 0xF20A0208;
        break;
    case 10:
        oad.value = 0xF2080201;
        break;
	case 11:
        oad.value = 0xF2090201;
        break;
	case 12:
        oad.value = 0xF2090202;
        break;
    default:
        break;
    }
    return oad;
}

OOP_OAD_U get_set_pulse(int index)
{
    OOP_OAD_U oad;
    memset(&oad, 0x00, sizeof(OOP_OAD_U));
    switch (index)
    {
    case 0:
        oad.value = 0xF20A0201;
        break;
    case 1:
        oad.value = 0xF20A0202;
        break;
    case 2:
        oad.value = 0xF20A0203;
        break;
    case 3:
        oad.value = 0xF20A0204;
        break;
    case 4:
        oad.value = 0xF20A0205;
        break;
    case 5:
        oad.value = 0xF20A0206;
        break;
    case 6:
        oad.value = 0xF20A0207;
        break;
    case 7:
        oad.value = 0xF20A0208;
        break;
    default:
        break;
    }
    return oad;
}

void measure_set_init(HDC hdc)
{
    measure_set_index = 1;
}
int flagMeter;  //��ʶ�����Ƿ����
void measure_set_draw(HDC hdc)
{
    char czBuf[300] = { 0 };
    int index = 0;
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop   = 20;
    int nLeft1 = 20;
    int nLeft2 = 130;
	int iSetp  = 13;
	
    TextOut(hdc, nLeft1, nTop, "�����");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    if (measure_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

	index++;
    nTop += iSetp;
    TextOut(hdc, nLeft1, nTop, "Э�飺");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flagMeter < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 prtl = MeterFile.basic.protocol;
        if (prtl > 5)
            prtl = 0;
        sprintf(czBuf, "%s", g_prtl[prtl]);
    }
    if (measure_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

	index++;
    nTop += iSetp;
    TextOut(hdc, nLeft1, nTop, "�˿ڣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flagMeter < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        get_portname(MeterFile.basic.port, czBuf);
    }
    if (measure_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

	index++;
    nTop += iSetp;
    TextOut(hdc, nLeft1, nTop, "�����ʣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flagMeter < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 index = MeterFile.basic.baud;
        if (index > 11)
            index = 11;
        sprintf(czBuf, "%s", g_baud[index]);
    }
    if (measure_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

	index++;
    nTop += iSetp;
    TextOut(hdc, nLeft1, nTop, "��ַ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flagMeter < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 Addr[6] = { 0 };
        memcpy(Addr, MeterFile.basic.tsa.add, 6);
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
    }
    if (measure_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

	index++;
    nTop += iSetp;
    TextOut(hdc, nLeft1, nTop, "���߷�ʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flagMeter < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 index = MeterFile.basic.pwrType;
        if (index > 3)
            index = 3;
        sprintf(czBuf, "%s", g_pwType[index]);
    }
    if (measure_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    
	index++;
    nTop += iSetp;
    TextOut(hdc, nLeft1, nTop, "���ѹ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flagMeter < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        float tmpe = (float)MeterFile.basic.stVolt/10;
        sprintf(czBuf, "%2.1f", tmpe);
    }
    if (measure_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += iSetp;
    TextOut(hdc, nLeft1, nTop, "�����");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flagMeter < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        float tmpe = (float)MeterFile.basic.stAmp/10;
        sprintf(czBuf, "%2.1f", tmpe);
    }
    if (measure_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    PopListDraw(hdc);
}

BOOL measure_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    int maxindex = 7;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (measure_set_index)
            {
            case 1://��Լ����
            {
                MeterFile.basic.protocol = nSel;
            }
            break;
            case 2://�˿�
            {
                MeterFile.basic.port = get_set_pipe(nSel);
            }
            break;
			case 3://������
            {
                MeterFile.basic.baud = nSel;
            }
            break;
			case 5://���߷�ʽ
            {
                MeterFile.basic.pwrType = nSel;
            }
            break;
            default:
                return FALSE;
            }
            MeterFile.nIndex = g_guiComm.nCurPoint;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }

	float tmpe = 0.0;
	
    switch (nKey)
    {
	    case KEY_UP:
	    {
	        if (measure_set_index > 1)
	            measure_set_index--;
	        else
	            measure_set_index = maxindex;
	        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
	        return TRUE;
	    }
	    case KEY_DOWN:
	    {
	        if (measure_set_index < maxindex)
	            measure_set_index++;
	        else
	            measure_set_index = 1;
	        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
	        return TRUE;
	    }
	    case KEY_OK:
	    {
	    	if (ValidatePwd() == TRUE)
	        {
		        if (measure_set_index == 0)
		        {
		            char bzbuf[50];
		            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
		            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
					MeterFile.nIndex = g_guiComm.nCurPoint;
		            return TRUE;
		        }

				//��Լ����
		        if (measure_set_index == 1)
		        {
		            printf("���ù�Լ����\n");
		            PopList(g_prtl, 6, g_prtl[0], 130, measure_set_index*13+20, FALSE);
		            return TRUE;
		        }

				//�˿�
		        if (measure_set_index == 2)
		        {
		            printf("���ö˿�\n");
		            PopList(g_pipe, 11, g_pipe[0], 130, measure_set_index*13+20, FALSE);
		            return TRUE;
		        }

				//���ò�����
		        if (measure_set_index == 3)
		        {
		            printf("���ò�����\n");
		            PopList(g_baud, 12, g_baud[0], 130, measure_set_index*13+2, FALSE);
		            return TRUE;
		        }
		        
		        //���õ���ַ
		        if (measure_set_index == 4)
		        {
		            uint8 Addr[6] = { 0 };
		            char tmp[20] = { 0 };
		            memcpy(Addr, MeterFile.basic.tsa.add, 6);
		            sprintf(tmp, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
		            if (strcmp(tmp, "000000000000") == 0)
		                strcpy(tmp, "");
		            InputBox(tmp, MeterAddrReturn, NULL, 0x01);
		            return TRUE;
		        }
		        
		        //���ý��߷�ʽ
		        if (measure_set_index == 5)
		        {
		            printf("���ò�����\n");
		            PopList(g_pwType, 4, g_pwType[0], 130, measure_set_index*13, FALSE);
		            return TRUE;
		        }
		        
		        //���ö��ѹ
		        if (measure_set_index == 6)
		        {
		            char tmp[20] = { 0 };
					tmpe = (float)MeterFile.basic.stVolt/10;
        			sprintf(tmp, "%2.1f", tmpe);
		            InputBox(tmp, MeterAddrReturn, NULL, 0x02);
		            return TRUE;
		        }

				//���ö����
		        if (measure_set_index == 7)
		        {
		            char tmp[20] = { 0 };
					tmpe = (float)MeterFile.basic.stAmp/10;
        			sprintf(tmp, "%2.1f", tmpe);
		            InputBox(tmp, MeterAddrReturn, NULL, 0x02);
		            return TRUE;
		        }
	    	}
	    }
    }
    return FALSE;
}

PLUGIN_INFO g_layer_measure_set_display = {
    0x01/*id*/,
    measure_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    measure_set_draw/*Draw*/,
    NULL/*Timer*/,
    measure_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//��ϸ��Ϣҳ���
PLUGIN_INFO* g_measure_set_plugin[] =
{
    &g_layer_measure_set_display,
};

uint8 meterfile_set_index;
int metermaxindex;
// �����������Ĵ���
void MeasureMainReturn(char* pNewStr, void* pPara)
{
    char czBuf[300];
    switch(meterfile_set_index)
    {
    case 2:
		// ����CT��Ϣ����
        if(FALSE == CheckNumber(pNewStr, 10000))
        {
            if(MessageBox("CT������벻��ȷ\n\nȷ���Ƿ���������?", MB_OKCANCEL) == FALSE)
            {
                return;
            }
            
			sprintf(czBuf, "%d", MeterFile.extend.CT);
            InputBox(czBuf, MeasureMainReturn, NULL, 0x01);
        }
        else
        {
            MeterFile.extend.CT = atoi(pNewStr);
        }
        break;
    case 3:
		// ����PT��Ϣ����
        if(FALSE == CheckNumber(pNewStr, 10000))
        {
            if(MessageBox("PT������벻��ȷ\n\nȷ���Ƿ���������?", MB_OKCANCEL) == FALSE)
            {
                return;
            }
			sprintf(czBuf, "%d", MeterFile.extend.PT);
            InputBox(czBuf, MeasureMainReturn, NULL, 0x01);
        }
        else
        {
            MeterFile.extend.PT = atoi(pNewStr);
        }
        break;
    }
}

void meterfile_set_init(HDC hdc)
{
    g_guiComm.nCurPoint = 0;
    meterfile_set_index = 0;
    nCurPoint = -1;
    flagMeter = -1;
}

void meterfile_set_draw(HDC hdc)
{
	// ��������ϸ��Ϣҳ������
    if (g_measure_main_data.pCurPlugin != NULL)
    {
        ASSERT(g_measure_main_data.pCurPlugin->pfnDraw != NULL);
        g_measure_main_data.pCurPlugin->pfnDraw(hdc);
        return;
    }
	
    char czBuf[300] = { 0 };
    int index = 0;
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    if (nCurPoint != g_guiComm.nCurPoint)
    {
        memset(&MeterFile, 0, sizeof(OOP_METER_T));
        flagMeter = get_meterfile(g_guiComm.nCurPoint, &MeterFile);
        if (flagMeter < 0)
        {
            g_measure_main_data.nState = 0;
        }
        else
        {
            g_measure_main_data.nState = 1;
        }
        set_gui_infonum(g_guiComm.nCurPoint);
        nCurPoint = g_guiComm.nCurPoint;
        metermaxindex = 4;
    }
    g_measure_main_data.nType = get_infonum_type(MeterFile.basic.port);
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    int nTop = 20;
    int nLeft1 = 30;
    int nLeft2 = 150;
    TextOut(hdc, nLeft1, nTop, "��ѡ�������");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 17;
    TextOut(hdc, nLeft1, nTop, "������״̬");
    TextOut(hdc, nLeft2, nTop, g_measure_point_state[g_measure_main_data.nState]);

	index++;
    nTop += 17;
    TextOut(hdc, nLeft1, nTop, "����������");
    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_measure_point_type[g_measure_main_data.nType]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_measure_point_type[g_measure_main_data.nType]);
    }
    index++;
    nTop += 17;
    TextOut(hdc, nLeft1, nTop, "CT��Ƶ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flagMeter < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", MeterFile.extend.CT);
    }
    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 17;
    TextOut(hdc, nLeft1, nTop, "PT��Ƶ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flagMeter < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", MeterFile.extend.PT);
    }
    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
	
    // ֻ��485�������ϸ����
	index++;
	nTop += 17;
#ifdef AREA_JIANGSU
    if(g_measure_main_data.nType == 1 || g_measure_main_data.nType == 2 || g_measure_main_data.nType == 3 ||  g_measure_main_data.nType == 4)
#else
    if(g_measure_main_data.nType == 1 || g_measure_main_data.nType == 2 )
#endif
	{// 485��
        metermaxindex = 5;
		if(meterfile_set_index == index)
		{
			ITEM_SELECT;
			TextOut(hdc, 36, nTop, "��ϸ����");
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOut(hdc, 36, nTop, "��ϸ����");
		}
		index++;
	}

	if(meterfile_set_index == index)
	{
		ITEM_SELECT;
		TextOut(hdc, 140, nTop, "��������");
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, 140, nTop, "��������");
	}

    PopListDraw(hdc);
}

//ɾ�������
void delete_meterfile(uint16 infonum)
{
    RECORD_OAD_T RecordDel;
    RecordDel.logicId = 0;
    RecordDel.infoNum = infonum;
    RecordDel.road.oadMain.value = 0;
    RecordDel.road.oadCols.nNum = 1;
    RecordDel.road.oadCols.oad[0] = MeterFileOad;
    CLEAR_DATA_T clear;
    clear.recordOAD = RecordDel;
    clear.bClassTag = FALSE;
    int ret;
    ret = db_clear_oad_data(g_hUdbCenter, &clear);
    if (ret == 0)
    {
        GUI_FMT_DEBUG("����ɾ���ɹ� infonum %d \n", infonum);
    }
    else
    {
        GUI_FMT_DEBUG("����ɾ��ʧ�� infonum %d \n", infonum);
    }

}

BOOL meterfile_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
	// ������ҳ��İ�����Ϣ����
    // ��ǰ���ǻ��ж������ҳ�棬���·�ҳ������
    // ���Ǻ���ֻ��һ��ҳ���ˣ����ǽṹ�����棬�����Ժ����չ
    int nPoint = g_guiComm.nCurPoint;
    if (g_measure_main_data.pCurPlugin != NULL)
    {
        if (g_measure_main_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_measure_main_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }
        // ʹ��ȱʡ����
        switch (nKey)
        {
        case KEY_CANCEL:
            if (g_measure_main_data.pCurPlugin->pfnUninitLayer != NULL)
            {
                g_measure_main_data.pCurPlugin->pfnUninitLayer();
                g_guiComm.nCurPoint = nPoint;
            }
            g_measure_main_data.pCurPlugin = NULL;
//            if (app_get_meter_list(g_guiComm.nCurPoint, &MeterFile) != ERR_OK) //�����ȡ����ʧ��
//                g_measure_main_data.nIndex = 0;
            g_guiComm.nArrow = -1;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case KEY_UP:// ���Ϸ�ҳ�棨���ж��ҳ�������£�
            if (g_measure_main_data.nPluginCnt > 1)
            {
                if (g_measure_main_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnUninitLayer();
                    g_guiComm.nCurPoint = nPoint;
                }
                g_measure_main_data.pCurPlugin = NULL;

                g_measure_main_data.nCurPlugin--;
                if (g_measure_main_data.nCurPlugin < 0)
                {
                    g_measure_main_data.nCurPlugin = g_measure_main_data.nPluginCnt - 1;
                }
                g_measure_main_data.pCurPlugin = g_measure_main_data.ppDetail[g_measure_main_data.nCurPlugin];
                ASSERT(g_measure_main_data.pCurPlugin != NULL);

                if (g_measure_main_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnInitLayer(g_measure_main_data.pCurPlugin->dwId);
                }

                if (g_measure_main_data.nCurPlugin > 0 && g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 0;
                }
                else if (g_measure_main_data.nCurPlugin > 0)
                {
                    g_guiComm.nArrow = 1;
                }
                else if (g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 2;
                }
                else
                {
                    g_guiComm.nArrow = -1;
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
            }
            break;
        case KEY_DOWN:// ���·�ҳ�棨���ж��ҳ�������£�
            if (g_measure_main_data.nPluginCnt > 1)
            {
                if (g_measure_main_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnUninitLayer();
                    g_guiComm.nCurPoint = nPoint;
                }
                g_measure_main_data.pCurPlugin = NULL;

                g_measure_main_data.nCurPlugin++;
                if (g_measure_main_data.nCurPlugin >= g_measure_main_data.nPluginCnt)
                {
                    g_measure_main_data.nCurPlugin = 0;
                }
                g_measure_main_data.pCurPlugin = g_measure_main_data.ppDetail[g_measure_main_data.nCurPlugin];
                ASSERT(g_measure_main_data.pCurPlugin != NULL);

                if (g_measure_main_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnInitLayer(g_measure_main_data.pCurPlugin->dwId);
                }

                if (g_measure_main_data.nCurPlugin > 0 && g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 0;
                }
                else if (g_measure_main_data.nCurPlugin > 0)
                {
                    g_guiComm.nArrow = 1;
                }
                else if (g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 2;
                }
                else
                {
                    g_guiComm.nArrow = -1;
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
            }
            break;
        }
        return TRUE;
    }

    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (meterfile_set_index)
            {
            case 1://����������
            {
                MeterFile.basic.port = get_set_pipe(nSel);
            }
            break;

            default:
                return FALSE;
            }
            MeterFile.nIndex = g_guiComm.nCurPoint;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }


    switch (nKey)
    {
	    case KEY_LEFT:
	    {
	        if (g_guiComm.nCurPoint == 0)
	            g_guiComm.nCurPoint = MAX_METER_INFONUM;
	        else
	            g_guiComm.nCurPoint--;
	        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
	        return TRUE;
	    }
	    case KEY_RIGHT:
	    {
	        if (g_guiComm.nCurPoint < MAX_METER_INFONUM)
	        {
	            g_guiComm.nCurPoint++;
	        }
	        else
	        {
	            g_guiComm.nCurPoint = 0;
	        }
	        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
	        return TRUE;
	    }
	    case KEY_UP:
	    {
	        if (meterfile_set_index > 0)
	            meterfile_set_index--;
	        else
	            meterfile_set_index = metermaxindex;
	        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
	        return TRUE;
	    }
	    case KEY_DOWN:
	    {
	        if (meterfile_set_index < metermaxindex)
	            meterfile_set_index++;
	        else
	            meterfile_set_index = 0;
	        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
	        return TRUE;
	    }
	    case KEY_OK:
	    {
	    	int nLeft = 150;
	    	if (ValidatePwd() == TRUE)
	        {
	        	//��������
#ifdef AREA_JIANGSU
                if (meterfile_set_index == 5 
					|| (meterfile_set_index == 4 && (g_measure_main_data.nType != 1 && g_measure_main_data.nType != 2&&g_measure_main_data.nType != 3 && g_measure_main_data.nType != 4)))
#else
                if (meterfile_set_index == 5 
					    || (meterfile_set_index == 4 && (g_measure_main_data.nType != 1 && g_measure_main_data.nType != 2)))
#endif
		               {
                    set_port_type(g_measure_main_data.nType);
		            ret = write_normal_data((uint8*)&MeterFile, sizeof(OOP_METER_T), MeterFileOad, g_guiComm.nCurPoint, 0, 2);
		            if (ret == 0)
		            {
		                MessageBox("��ǰ�������ñ���ɹ���");
		                meterfile_set_index = 0;
		                return TRUE;
		            }
		            else
		            {
		                MessageBox("��ǰ�������ñ���ʧ�ܣ�");
		                return FALSE;
		            }

		        }
					
		        if (meterfile_set_index == 0)
		        {
		            char bzbuf[50];
		            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
		            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
		//          UpdateWindow(g_guiComm.hMainWnd, TRUE);
		            return TRUE;
		        }

				//����������
		        if (meterfile_set_index == 1)
		        {
		            printf("����������\n");
		            PopList(g_pipe, 13, g_pipe[0], nLeft, 20, FALSE);
		            return TRUE;
		        }
		        //����CT
		        if (meterfile_set_index == 2)
		        {
		            char bzbuf[50];
		            sprintf(bzbuf, "%d", MeterFile.extend.CT);
		            InputBox(bzbuf, MeasureMainReturn, NULL, 0x01);
		//          UpdateWindow(g_guiComm.hMainWnd, TRUE);
		            return TRUE;
		        }
		        //����PT
		        if (meterfile_set_index == 3)
		        {
		            char bzbuf[50];
		            sprintf(bzbuf, "%d", MeterFile.extend.PT);
		            InputBox(bzbuf, MeasureMainReturn, NULL, 0x01);
		            return TRUE;
		        }
		        
		        //��ϸ��Ϣ
		        if (meterfile_set_index == 4)
		        {
		            g_measure_main_data.ppDetail = g_measure_set_plugin;
		            g_measure_main_data.nPluginCnt = sizeof(g_measure_set_plugin) / sizeof(g_measure_set_plugin[0]);
		            g_measure_main_data.nCurPlugin = 0;
		            g_measure_main_data.pCurPlugin = g_measure_main_data.ppDetail[g_measure_main_data.nCurPlugin];
		            ASSERT(g_measure_main_data.pCurPlugin != NULL);
		            if (g_measure_main_data.nPluginCnt > 1)
		            {
		                g_guiComm.nArrow = 2;
		            }
		            if (g_measure_main_data.pCurPlugin->pfnInitLayer != NULL)
		            {
		                g_measure_main_data.pCurPlugin->pfnInitLayer(g_measure_main_data.pCurPlugin->dwId);
		            }
		            UpdateWindow(g_guiComm.hMainWnd, TRUE);
		            return TRUE;
		        }
		        
	    	}
			return TRUE;
	    }

		case KEY_CANCEL://ȡ��
        {
            if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
            {
                set_port_type(g_measure_main_data.nType);
                ret = write_normal_data((uint8*)&MeterFile, sizeof(OOP_METER_T), MeterFileOad, g_guiComm.nCurPoint, 0, 2);
                if (ret == 0)
	            {
	                MessageBox("��ǰ�������ñ���ɹ���");
	                meterfile_set_index = 0;
	                return FALSE;;
	            }
	            else
	            {
	                MessageBox("��ǰ�������ñ���ʧ�ܣ�");
	                return FALSE;
	            }
            }
            return FALSE;
        }
    }
    return FALSE;
}

//���������
PLUGIN_INFO g_layer_meterfile_set = {
    0x20730000/*id*/,
    meterfile_set_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    meterfile_set_draw/*Draw*/,
    NULL/*Timer*/,
    meterfile_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("��������",1)

const char* g_pczPulseGroup[] = {"��1·", "��2·", "��3·", "��4·",
							"��5·", "��6·", "��7·", "��8·",
							"��9·", "��10·", "��11·", "��12·",
							"��13·", "��14·", "��15·", "��16·"};
//const char* g_pczPulseAttr[] = {"�����й�", "�����й�", "�����޹�", "�����޹�",
//		"��������޹�", "���������޹�", "���������޹�", "��������޹�"};
const char* g_pczPulseAttr[] = {"�����й�", "�����޹�", "�����й�", "�����޹�", "��Ч"};
//char* g_pczPulseType[] = {"ң��", "����"};
const char* g_pczPulseStatus[] = {"��Ч", "��Ч"};


OOP_PULSESETS_T  g_plus_param[8];              //��������       0x24010400
OOP_PULSESETS_B_T g_b_plus_param[8];           //��������      0x24011C00

/************************************************************************/
/* �������������                                                           */
/************************************************************************/
typedef struct _PULSE_MAIN
{
    int     nIndex;     // ��ǰ����
	
	int		nGroup;		// ����
	int		nState;		// ״̬
}PULSE_MAIN;
PULSE_MAIN g_pulse_main;
PLUGIN_INFO* g_pulseDetail = NULL;
#if DESC("������ϸ����",1)
/************************************************************************/
/* ��������������                                                     */
/************************************************************************/
int     gPluseIndex;       // ѡ����
uint32 K;//���峣��
int     gPlusePort;       // ѡ����˿�
void PulseDetailInit(DWORD dwPluginId)
{   
    OOP_PULSECFG_T tmp[8];
    memset(tmp, 0, sizeof(OOP_PULSECFG_T)*8);
    OOP_PULSECFG_B_T tBmp[8];
    memset(tBmp, 0, sizeof(OOP_PULSECFG_B_T)*8);
    for (uint32 i = 0;i < g_plus_param[g_pulse_main.nGroup-1].nNum; i++){
        if (g_plus_param[g_pulse_main.nGroup-1].pulse[i].port.value != 0)
        {
            tmp[g_plus_param[g_pulse_main.nGroup-1].pulse[i].port.nIndex - 1] = g_plus_param[g_pulse_main.nGroup-1].pulse[i];
            tBmp[g_plus_param[g_pulse_main.nGroup-1].pulse[i].port.nIndex - 1] = g_b_plus_param[g_pulse_main.nGroup-1].pulse[i];
        }
    }
    memset(g_plus_param[g_pulse_main.nGroup-1].pulse, 0, sizeof(OOP_PULSECFG_T)*8);
    memset(g_b_plus_param[g_pulse_main.nGroup-1].pulse, 0, sizeof(OOP_PULSECFG_T)*8);
    for (int i = 0;i < 8; i++){
        g_plus_param[g_pulse_main.nGroup-1].pulse[i] = tmp[i];
        g_b_plus_param [g_pulse_main.nGroup-1].pulse[i] = tBmp[i];
    }

    gPlusePort = 0;
    gPluseIndex = 0;
}
void PulseDetailDraw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

	char czBuf[100];
	sprintf(czBuf, "%s��������", g_pczPulseGroup[g_pulse_main.nGroup-1]);

    rc.top = 20;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

	// �����������
	int nLeft = 130;
    int nTop = rc.top+20;
    int pluse_property = 4;
    TextOut(hdc, 5, nTop, "����˿�");

    if(g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].port.value == 0)
    {
        g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].port = get_set_pulse(0);
    }
    get_portname(g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].port, czBuf);
    if(gPluseIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, czBuf);
    }

	// ��������
    nTop += 18;
    TextOut(hdc, 5, nTop, "��������");

    //��������698��Լֻ���֣���ԼĿǰû�зŴ��¸�ֵ��������4�����ϣ����4������ʾ��Ч
    if(g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].property < 4)
    {
        pluse_property = g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].property; 
    }
    
	if(gPluseIndex == 1)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft, nTop, g_pczPulseAttr[pluse_property]);
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, g_pczPulseAttr[pluse_property]);
	}

	// ���峣��
    nTop += 18;
    TextOut(hdc, 5, nTop, "���峣��");
   
    if(g_plus_param[g_pulse_main.nGroup-1].rsv == 1)
        sprintf(czBuf, "%d", g_b_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].K);
    else
        sprintf(czBuf, "%d", g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].K);
    
	if(gPluseIndex == 2)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft, nTop, czBuf);
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft, nTop, czBuf);
	}
    PopListDraw(hdc);
}

void set_rsv_value(){
    int  flag = 0;
    for (int i = 0;i < 8; i++){
        if (g_b_plus_param[g_pulse_main.nGroup-1].pulse[i].K >= 0xFFFF)
            flag = 1;
    }
    if (flag == 0)
    {
        g_b_plus_param[g_pulse_main.nGroup-1].rsv = 0;
        g_plus_param[g_pulse_main.nGroup-1].rsv = 0;
    }
    else
    {
        g_b_plus_param[g_pulse_main.nGroup-1].rsv = 1;
        g_plus_param[g_pulse_main.nGroup-1].rsv = 1;
    }
}
void set_nNum_value()
{
    g_plus_param[g_pulse_main.nGroup-1].nNum = 0;
    for (int i = 0;i < 8; i++){
        if (g_plus_param[g_pulse_main.nGroup-1].pulse[i].K == 0)
            g_plus_param[g_pulse_main.nGroup-1].pulse[i].port.value = 0;
        if (g_plus_param[g_pulse_main.nGroup-1].pulse[i].port.value != 0)
            g_plus_param[g_pulse_main.nGroup-1].nNum++;
    }
}

void set_plus_value()
{
    OOP_PULSECFG_T tmp[8];
    int nNum = 0;
    memset(tmp, 0, sizeof(OOP_PULSECFG_T)*8);
    for (int i = 0;i < 8; i++){
        if (g_plus_param[g_pulse_main.nGroup-1].pulse[i].port.value != 0)
            tmp[nNum++] = g_plus_param[g_pulse_main.nGroup-1].pulse[i];
    }
    memset(g_plus_param[g_pulse_main.nGroup-1].pulse, 0, sizeof(OOP_PULSECFG_T)*8);
    for (int i = 0;i < 8; i++){
        g_plus_param[g_pulse_main.nGroup-1].pulse[i] = tmp[i];
    }
}

void set_Bplus_value()
{
    OOP_PULSECFG_B_T tmp[8];
    int nNum = 0;
    memset(tmp, 0, sizeof(OOP_PULSECFG_B_T)*8);
    for (int i = 0;i < 8; i++){
        if (g_b_plus_param[g_pulse_main.nGroup-1].pulse[i].K != 0)
            tmp[nNum++] = g_b_plus_param[g_pulse_main.nGroup-1].pulse[i];
    }
    memset(g_b_plus_param[g_pulse_main.nGroup-1].pulse, 0, sizeof(OOP_PULSECFG_T)*8);
    for (int i = 0;i < 8; i++){
        g_b_plus_param [g_pulse_main.nGroup-1].pulse[i] = tmp[i];
    }
}
void PulseDetailReturn(char* pNewStr, void* pPara)
{// ��ǰֻ�����峣��
	// ��֤������ȷ��
    if(CheckNumber(pNewStr, 999999) == FALSE)
    {
        char czBuf[100];
        if(MessageBox("���峣�����벻��ȷ\n\nȷ���Ƿ���������?", MB_OKCANCEL) == FALSE)
        {
            return;
        }
        if(g_plus_param[g_pulse_main.nGroup-1].rsv == 1)
            sprintf(czBuf, "%d", g_b_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].K);
        else
            sprintf(czBuf, "%d", g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].K);
        InputBox(czBuf, PulseDetailReturn, NULL, 0x01);
    }
    else
    {
		// �������峣��
        K = (uint32)atoi(pNewStr);
        if((K >= 0xFFFF)||(g_plus_param[g_pulse_main.nGroup-1].rsv == 1))
        {
            g_b_plus_param[g_pulse_main.nGroup-1].rsv = 1;
            g_plus_param[g_pulse_main.nGroup-1].rsv = 1;
            g_b_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].K = K;
            if (K >= 0xFFFF)
                g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].K = 0xFFFF;
            else
                g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].K = K;
        }
        else
        {
            g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].K = K;
        }
    }

}
BOOL PulseDetailKeyMsg(int nKey)
{
	int nSe2;
	// ����List��������
    if(g_guiComm.bPopList == TRUE)
    {
        PopListKeyMsg(nKey, &nSe2);
        if(nKey == KEY_OK)
        {
            switch(gPluseIndex)
            {
             case 0:
                gPlusePort = nSe2;
                if(g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].port.value == 0)
                    g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].port = get_set_pulse(nSe2);
                break;
            case 1:
                g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].property = nSe2;
                break;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch(nKey)
    {
    case KEY_UP:
        gPluseIndex --;
        if(gPluseIndex < 0)
        {
            gPluseIndex = 0;
            return FALSE;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_LEFT:
        {
            if(gPlusePort == 0)
            {
                gPlusePort = 7;
            }else{
                gPlusePort--;
            }
            if(g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].port.value == 0)
                g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].port = get_set_pulse(gPlusePort);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    case KEY_RIGHT:
        {
            if(gPlusePort == 7)
            {
                gPlusePort = 0;
            }
            else{
                gPlusePort ++;
            }
            if(g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].port.value == 0)
                g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].port = get_set_pulse(gPlusePort);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
            }
        return TRUE;
    case KEY_DOWN:
		gPluseIndex ++;
		if(gPluseIndex > 2)
		{
			gPluseIndex = 2;
			return FALSE;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
		if(ValidatePwd() == TRUE)
        {// ȷ�ϴ���
            int nLeft = 130;
            int nTop = gPluseIndex*16+26;
            switch(gPluseIndex)
            {
            case 0:// ����������ѡ��ҳ��
				PopList(g_pulse, 8, g_pulse[0], nLeft, 15, TRUE);
                break;
            case 1:// ������������List
                PopList(g_pczPulseAttr, 4, 
                    g_pczPulseAttr[g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].property], nLeft, nTop-26, TRUE);
                break;
            case 2:
                {// �������峣������ҳ���
                    char czBuf[50];
                    if(g_plus_param[g_pulse_main.nGroup-1].rsv == 1)
                        sprintf(czBuf, "%d", g_b_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].K);
                    else
                        sprintf(czBuf, "%d", g_plus_param[g_pulse_main.nGroup-1].pulse[gPlusePort].K);
                    InputBox(czBuf, PulseDetailReturn, NULL, 0x01);
                }
                break;    
            }
        }
		return TRUE;
	    case KEY_CANCEL:
		if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
        {
            OOP_OAD_U oad;
			oad.value = 0x24010400 + (0x00010000) * (g_pulse_main.nGroup -1);
			set_rsv_value();
			set_nNum_value();
			for (uint32 i = 0;i<8;i++){
			    g_b_plus_param[g_pulse_main.nGroup-1].pulse[i].port = g_plus_param[g_pulse_main.nGroup-1].pulse[i].port;
			    g_b_plus_param[g_pulse_main.nGroup-1].pulse[i].property = g_plus_param[g_pulse_main.nGroup-1].pulse[i].property;
			    if(g_b_plus_param[g_pulse_main.nGroup-1].pulse[i].K == 0){
					g_b_plus_param[g_pulse_main.nGroup-1].pulse[i].K = g_plus_param[g_pulse_main.nGroup-1].pulse[i].K;
				}
		    }
            set_plus_value();
            set_Bplus_value();
            int ret = write_normal_data((uint8*)&g_plus_param[g_pulse_main.nGroup-1], sizeof(OOP_PULSESETS_T), oad, 0, 0, 2);
            oad.value = 0x24011C00 + (0x00010000) * (g_pulse_main.nGroup -1);
			g_b_plus_param[g_pulse_main.nGroup-1].nNum = g_plus_param[g_pulse_main.nGroup-1].nNum;
			ret = write_normal_data((uint8*)&g_b_plus_param[g_pulse_main.nGroup-1], sizeof(OOP_PULSESETS_B_T), oad, 0, 0, 2);
            if (ret == 0)
            {
                MessageBox("��ǰ�������ñ���ɹ���");
            }
            else
            {
                MessageBox("��ǰ�������ñ���ʧ�ܣ�");
            }
			
        }
		return FALSE;
        
    }
    return FALSE;
}
PLUGIN_INFO g_layerPulseDetail = {
	0x20740001/*id*/, 
	PulseDetailInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	PulseDetailDraw/*Draw*/,
	NULL/*Timer*/,
	PulseDetailKeyMsg/*keymsg*/,
	NULL/*ipc*/
};
/************************************************************************/
/* �������������ý���                                                 */
/************************************************************************/


#endif

void PulseMainInit(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;
	// ���õ�ǰѡ����
	g_pulse_main.nIndex = (g_guiComm.bKeyDown == TRUE)?0:2;
	g_pulse_main.nGroup = 1;

	memset(g_plus_param, 0, sizeof(OOP_PULSESETS_T)*8);
	memset(g_b_plus_param, 0, sizeof(OOP_PULSESETS_B_T)*8);
    uint32 oadvalue;
    //g_guiComm.nCurPoint = (dwPluginId) & 0x0F; 
    for (int i =0;i<8;i++){
        oadvalue = 0x24010400 + (0x00010000) * i;
        read_db_noamal_data(g_hUdbCenter, oadvalue, 0, 0, (uint8*)&g_plus_param[i], sizeof(OOP_PULSESETS_T));
        oadvalue = 0x24011C00 + (0x00010000) * i;
        read_db_noamal_data(g_hUdbCenter, oadvalue, 0, 0, (uint8*)&g_b_plus_param[i], sizeof(OOP_PULSESETS_B_T));
    }
	// ״̬��־
	if(g_plus_param[g_pulse_main.nGroup-1].nNum == 0)
    {
        g_pulse_main.nState = 0;
    }
    else
    {
        g_pulse_main.nState = 1;
    }
//    oadvalue = 0x24011C00 + (0x00010000) * g_guiComm.nCurPoint;
//    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].plus_b_param, sizeof(OOP_PULSESETS_B_T));

}
void PulseMainDraw(HDC hdc)
{
	// ������ϸ���û���
	if(g_pulseDetail != NULL)
    {
        g_pulseDetail->pfnDraw(hdc);
        return;
    }
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontBig);

	RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

	char czBuf[100];
	sprintf(czBuf, "��������");

    rc.top = 20;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

	// ����·
    int nLeft1 = 20;
	int nLeft2 = 130;
	int nTop = 20;
	nTop += 26;
	TextOut(hdc, nLeft1, nTop, "ѡ������·");
	sprintf(czBuf, "%d", g_pulse_main.nGroup);
    if(g_pulse_main.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_pczPulseGroup[g_pulse_main.nGroup-1]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_pczPulseGroup[g_pulse_main.nGroup-1]);
    }

	nTop += 18;
	TextOut(hdc, nLeft1, nTop, "״̬��־");
    if(g_pulse_main.nIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_pczPulseStatus[g_pulse_main.nState]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_pczPulseStatus[g_pulse_main.nState]);
    }

	// ��ϸ����
	nTop += 23;
	if(g_pulse_main.nState == 1)
	{
		if(g_pulse_main.nIndex == 2)
		{
			ITEM_SELECT;
			TextOut(hdc, nLeft2, nTop, "��ϸ����");
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOut(hdc, nLeft2, nTop, "��ϸ����");
		}
	}
		// ��ϸ����
	nTop += 23;
	if(g_pulse_main.nState == 1)
	{
		if(g_pulse_main.nIndex == 3)
		{
			ITEM_SELECT;
			TextOut(hdc, nLeft2, nTop, "��������");
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOut(hdc, nLeft2, nTop, "��������");
		}
	}
	else
	{
		if(g_pulse_main.nIndex == 2)
		{
			ITEM_SELECT;
			TextOut(hdc, nLeft2, nTop, "��������");
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOut(hdc, nLeft2, nTop, "��������");
		}
	}
	PopListDraw(hdc);
}

BOOL PulseMainKeyMsg(int nKey)
{
	// ������ϸ���ð�����Ϣ
	if(g_pulseDetail != NULL)
    {
        BOOL bRet = FALSE;
        if(g_pulseDetail->pfnKeyMsg != NULL)
        {
            bRet = g_pulseDetail->pfnKeyMsg(nKey);
        }
		// ȱʡ����
        if((bRet == FALSE && nKey==KEY_CANCEL) || bRet == RET_CANCEL)
        {// �˳�ҳ���
            if(g_pulseDetail->pfnUninitLayer != NULL)
            {
                g_pulseDetail->pfnUninitLayer();
            }
            g_pulseDetail = NULL;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }
	// ����List��������
	if(g_guiComm.bPopList == TRUE)
    {
        int nSel;
		//char czBuf[20];
		//int nRet;
        PopListKeyMsg(nKey, &nSel);
        if(nKey == KEY_OK)
        {			
            switch(g_pulse_main.nIndex)
            {
			case 0:// ����·���ı䣬���¶�ȡ������Ϣ
				g_pulse_main.nGroup = nSel+1;
				// ״̬��־
	            if(g_plus_param[g_pulse_main.nGroup-1].nNum == 0)
                {
                    g_pulse_main.nState = 0;
                }
                else
                {
                    g_pulse_main.nState = 1;
                }
				break;
			case 1:
				g_pulse_main.nState = nSel;
				break;
            }
			
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch(nKey)
    {
    case KEY_UP:
        g_pulse_main.nIndex --;
        if(g_pulse_main.nIndex < 0)
        {
            g_pulse_main.nIndex = 0;
            return FALSE;
        }
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
    case KEY_DOWN:
		{
			g_pulse_main.nIndex ++;
		    int nMax;
		    if (g_pulse_main.nState==1)
			    nMax = 3;
			else
			    nMax = 2;
			if(g_pulse_main.nIndex > nMax)
			{
				g_pulse_main.nIndex = nMax;
				return FALSE;
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
	case KEY_LEFT:
		{
			if(g_pulse_main.nIndex == 0)
			{
				g_pulse_main.nGroup --;
				if(g_pulse_main.nGroup <= 0)
				{
					g_pulse_main.nGroup = 1;
				}
				// ״̬��־
	            if(g_plus_param[g_pulse_main.nGroup-1].nNum == 0)
                {
                    g_pulse_main.nState = 0;
                }
                else
                {
                    g_pulse_main.nState = 1;
                }
				UpdateWindow(g_guiComm.hMainWnd, TRUE);
				return TRUE;
			}
		}
		break;
	case KEY_RIGHT:
		{
			if(g_pulse_main.nIndex == 0)
			{
				g_pulse_main.nGroup ++;
				if(g_pulse_main.nGroup > 8)
				{
				    g_pulse_main.nGroup = 1;
				}
				// ״̬��־
	            if(g_plus_param[g_pulse_main.nGroup-1].nNum == 0)
                {
                    g_pulse_main.nState = 0;
                }
                else
                {
                    g_pulse_main.nState = 1;
                }
				UpdateWindow(g_guiComm.hMainWnd, TRUE);
				return TRUE;
			}
		}
		break;
    case KEY_OK:
        {// ȷ�ϰ�������
			int nLeft = 130;
            int nIndex = g_pulse_main.nIndex;
            int nTop = nIndex*16+46;
			
			switch(g_pulse_main.nIndex)
			{
			case 0:// ��������ѡ��List
				PopList(g_pczPulseGroup, 8, g_pczPulseGroup[g_pulse_main.nGroup-1], nLeft, nTop-30, TRUE);
				break;
			case 1:
				if(ValidatePwd() == TRUE)
				{// ��������״̬ѡ��List
					PopList(g_pczPulseStatus, 2, g_pczPulseStatus[g_pulse_main.nState], nLeft, nTop, TRUE);
				}
				break;
			case 2:
				{
					if(g_pulse_main.nState == 1)
					{// ������ϸ����ҳ���
					g_pulseDetail = &g_layerPulseDetail;
					if(g_pulseDetail->pfnInitLayer != NULL)
					{
						g_pulseDetail->pfnInitLayer(g_pulseDetail->dwId);
					}
					UpdateWindow(g_guiComm.hMainWnd, TRUE);
					}
					else{// �������ý���
						OOP_OAD_U oad;
			            oad.value = 0x24010400 + (0x00010000) * (g_pulse_main.nGroup -1);
						memset(&g_plus_param[g_pulse_main.nGroup-1], 0, sizeof(OOP_PULSESETS_T));
	                    memset(&g_b_plus_param[g_pulse_main.nGroup-1], 0, sizeof(OOP_PULSESETS_B_T));
			            int ret = write_normal_data((uint8*)&g_plus_param[g_pulse_main.nGroup-1], sizeof(OOP_PULSESETS_T), oad, 0, 0, 2);
			            oad.value = 0x24011C00 + (0x00010000) * (g_pulse_main.nGroup -1);
                        ret = write_normal_data((uint8*)&g_b_plus_param[g_pulse_main.nGroup-1], sizeof(OOP_PULSESETS_B_T), oad, 0, 0, 2);
                        if (ret == 0)
                        {
                            MessageBox("�����ǰ�������óɹ���");
                        }
                        else
                        {
                            MessageBox("�����ǰ��������ʧ�ܣ�");
                        }
					}
				}
				break;
			case 3:
				{// �������ý���
				    OOP_OAD_U oad;
			        oad.value = 0x24010400 + (0x00010000) * (g_pulse_main.nGroup -1);
					set_rsv_value();
					set_nNum_value();
                    for (uint32 i = 0;i<8;i++){
                        g_b_plus_param[g_pulse_main.nGroup-1].pulse[i].port = g_plus_param[g_pulse_main.nGroup-1].pulse[i].port;
                        g_b_plus_param[g_pulse_main.nGroup-1].pulse[i].property = g_plus_param[g_pulse_main.nGroup-1].pulse[i].property;
                        if(g_b_plus_param[g_pulse_main.nGroup-1].pulse[i].K == 0){
                            g_b_plus_param[g_pulse_main.nGroup-1].pulse[i].K = g_plus_param[g_pulse_main.nGroup-1].pulse[i].K;
                        }
                    }
					set_plus_value();
			        set_Bplus_value();
			        int ret = write_normal_data((uint8*)&g_plus_param[g_pulse_main.nGroup-1], sizeof(OOP_PULSESETS_T), oad, 0, 0, 2);
			        oad.value = 0x24011C00 + (0x00010000) * (g_pulse_main.nGroup -1);
					g_b_plus_param[g_pulse_main.nGroup-1].nNum = g_plus_param[g_pulse_main.nGroup-1].nNum;
                    ret = write_normal_data((uint8*)&g_b_plus_param[g_pulse_main.nGroup-1], sizeof(OOP_PULSESETS_B_T), oad, 0, 0, 2);
                    if (ret == 0)
                    {
                         MessageBox("���浱ǰ�������óɹ���");
                    }
                    else
                    {
                         MessageBox("���浱ǰ��������ʧ�ܣ�");
                    }
				}
				break;
			}
            return TRUE;
        }
    }
    return FALSE;
}
PLUGIN_INFO g_layerPulseMain = {
	0x20740000/*id*/, 
	PulseMainInit/*InitLayer*/,
	UninitDefault/*UninitLayer*/,
	PulseMainDraw/*Draw*/,
	NULL/*Timer*/,
	PulseMainKeyMsg/*keymsg*/,
	NULL/*ipc*/
};
/************************************************************************/
/* �������������                                                       */
/************************************************************************/

#endif


#if DESC("�ܼ�������",1)
const char* g_pczGroup[] = {"1", "2", "3", "4","5", "6", "7", "8","9", "10", "11", "12","13", "14", "15", "16"};

OOP_TGSETS_T     g_gruop_param[8];             //��������       0x23010200
GROUP_DETAIL     g_group_detail;
TOTAL_MEASURE_INFO g_TotalMeasureInfo;
int gCfgIndex;

OOP_METER g_gruop_oopMeter;
char gruopBuf[20];
int groupindex;
//uint32 tmpRsv;
#if DESC("�ܼ���������ϸ����",1)
void GroupConfigDraw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

	char czBuf[300];

	int nTop = 20;
    TextOut(hdc, 56, nTop, "��ϸ����");

	int nLeft1 = 10;
	int nLeft2 = 106;
	nTop += 20;
	TextOut(hdc, nLeft1, nTop, "��·ͨ�ŵ�ַ");
	//if(get_bit_value((uint8 *)&g_gruop_param[g_pulse_main.nGroup-1].rsv,32,0) > 0)
	if(g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len > 0)
	{
	    uint16 offset = 0;
        memset(gruopBuf, 0, offset);
        for (uint8 j = 0; j <= g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len; j++)
        {
            offset += sprintf(gruopBuf + offset, "%02X", g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.add[j]);
        }
		sprintf(czBuf, "%s", gruopBuf);
	}
	else
	{
		sprintf(czBuf, "-");
	}
	if(gCfgIndex == 0)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft2, nTop, czBuf);
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft2, nTop, czBuf);
	}

	nTop += 16;
	TextOut(hdc, nLeft1, nTop, "����");
    if(g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len > 0)
    {
        sprintf(czBuf, "%s", g_pczDirect[g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].tgFlag]);
	}
	else
	{
		sprintf(czBuf, "-");
	}
	
	if(gCfgIndex == 1)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft2, nTop, czBuf);
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft2, nTop, czBuf);
	}

	nTop += 16;
	TextOut(hdc, nLeft1, nTop, "�ۼӱ�־");
    if(g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len > 0)
    {
        sprintf(czBuf, "%s", g_pczOperator[g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].opFlag]);
	}
	else
	{
		sprintf(czBuf, "-");
	}
	if(gCfgIndex == 2)
	{
		ITEM_SELECT;
		TextOut(hdc, nLeft2, nTop, czBuf);
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, nLeft2, nTop, czBuf);
	}
	

	nTop += 26;
	if(gCfgIndex == 3)
	{
		ITEM_SELECT;
		TextOut(hdc, 100, nTop, "ȷ��");
		ITEM_NOT_SELECT;
	}
	else
	{
		TextOut(hdc, 100, nTop, "ȷ��");
	}
    PopListDraw(hdc);
}

void groupPointReturn(char* pNewStr, void* pPara)
{
    uint32 nPoint = atoi(pNewStr);
    bool flag = false;
    if(nPoint >= VIR_BEGIN)
    {
        char bzbuf[100];
        sprintf(bzbuf, "���������Ϊ%d\nȷ���Ƿ���������?", (VIR_BEGIN-1));
        if(TRUE == MessageBox(bzbuf, MB_OKCANCEL))
        {
            sprintf(bzbuf, "%d", g_group_detail.nMeasureInfo[g_group_detail.nSel][0]);
            InputBox(bzbuf, groupPointReturn, NULL, 0x01);
        }
    }else{
        for (uint32 i = 0; i < g_gruop_oopMeter.nNum; i++){
            if(g_gruop_oopMeter.nValue[i] == nPoint){
                flag = true;
            }
        }
        if (flag == true){
            g_group_detail.nMeasureInfo[g_group_detail.nSel][0] = nPoint;
        }else{
            char bzbuf[100];
            sprintf(bzbuf, "%d���ڵ�����\nȷ���Ƿ���������?", nPoint);
            if(TRUE == MessageBox(bzbuf, MB_OKCANCEL))
            {
                sprintf(bzbuf, "%d", g_group_detail.nMeasureInfo[g_group_detail.nSel][0]);
                InputBox(bzbuf, groupPointReturn, NULL, 0x01);
            }
       }
    }
}

//���õ������ַ
void GroupAddrReturn(char* pNewStr, void* pPara)
{
    int nLen = strlen(pNewStr);
    uint8 Addr[20] = { 0 };
    char tmp[20] = { 0 };
    if (nLen > 12 && TRUE == MessageBox("���õ���ַ���ȴ���6�ֽ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        if (g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len <= 0)
        {
            sprintf(tmp, "000000000000");
        }
        else
        {
            uint16 offset = 0;
            memset(tmp, 0, offset);
            for (uint8 j = 0; j <= g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len; j++)
            {
                offset += sprintf(tmp + offset, "%02X", g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.add[j]);
            }
        }
        InputBox(tmp, GroupAddrReturn, NULL, 0x01);
    }
    else
    {
        strcpy(tmp, "000000000000");
        strcpy(tmp + 12 - nLen, pNewStr);
        commfun_ASC2BCD((char*)Addr, tmp, 12, 0);
        g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len = 5;
        memcpy(g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.add, Addr, 6);
        uint16 offset = 0;
        memset(gruopBuf, 0, offset);
        for (uint8 j = 0; j <= g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len; j++)
        {
            offset += sprintf(gruopBuf + offset, "%02X", g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.add[j]);
        }
    }
}

BOOL GroupConfigKeyMsg(int nKey)
{
    // ������List�ķ�����Ϣ
	if(g_guiComm.bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if(nKey == KEY_OK)
        {
			switch(gCfgIndex)
			{
			case 1:// �޸ĵ�ǰ�����־
				g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].tgFlag = nSel;
				break;
			case 2:// �޸ĵ�ǰ�������־
				g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].opFlag = nSel;
				break;
			}
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch(nKey)
    {
    case KEY_UP:
        gCfgIndex --;
        if(gCfgIndex < 0)
        {
            gCfgIndex = 3;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:	
        gCfgIndex ++;	
        if(gCfgIndex > 3)
        {
            gCfgIndex = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
		if(ValidatePwd() == TRUE)
        {
			int nLeft = 106;
			int nTop = gCfgIndex*16 + 40;
			switch(gCfgIndex)
			{
			case 0:// ����������ҳ��
			    char bzbuf[50];
                if (g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len <= 0)
                {
                    sprintf(bzbuf, "000000000000");
                }
                else
               {
                    uint16 offset = 0;
                    memset(bzbuf, 0, offset);
                    for (uint8 j = 0; j <= g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len; j++)
                    {
                        offset += sprintf(bzbuf + offset, "%02X", g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.add[j]);
                    }
                }
		        InputBox(bzbuf, GroupAddrReturn, NULL, 0x01);
				break;
			case 1:// ��������ѡ��List
				PopList(g_pczDirect, 2, g_pczDirect[0], nLeft, nTop, FALSE);
				break;
			case 2:// ���������ѡ��List
				PopList(g_pczOperator, 2, g_pczOperator[0], nLeft, nTop, FALSE);
				break;
			case 3:// �ж�һ������Ĳ��������ã���ǰֻ֧��8·
 				//int nIndex = g_group_detail.nSel;
                //g_gruop_param[g_pulse_main.nGroup-1].rsv = tmpRsv;
                if(g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len <= 0)
				{
                    MessageBox("��ǰ���ò�����");
		            return TRUE;
				}
 				/*g_TotalMeasureInfo.nMeasureInfo[nIndex][0] = g_group_detail.nMeasureInfo[nIndex][0];
 				g_TotalMeasureInfo.nMeasureInfo[nIndex][1] = g_group_detail.nMeasureInfo[nIndex][1];
 				g_TotalMeasureInfo.nMeasureInfo[nIndex][2] = g_group_detail.nMeasureInfo[nIndex][2];
                g_gruop_param[g_pulse_main.nGroup-1].tg[nIndex].tgFlag = g_TotalMeasureInfo.nMeasureInfo[nIndex][1] - 1;
                g_gruop_param[g_pulse_main.nGroup-1].tg[nIndex].opFlag = g_TotalMeasureInfo.nMeasureInfo[nIndex][2] - 1;
                for (uint32 i = 0; i < g_gruop_oopMeter.nNum; i++) {
                    if (g_gruop_oopMeter.nValue[i] == g_group_detail.nMeasureInfo[nIndex][0])
					{
                        g_gruop_param[g_pulse_main.nGroup-1].tg[nIndex].add = g_gruop_oopMeter.oopMeter[i].basic.tsa;
						if (g_gruop_param[g_pulse_main.nGroup-1].nNum < OOP_MAX_TGSETS)
					    {
						    g_gruop_param[g_pulse_main.nGroup-1].nNum +=1;
					    }
						set_bit_value((uint8 *)&g_gruop_param[g_pulse_main.nGroup-1].rsv,32,g_group_detail.nSel);
					}
			    }
				OOP_OAD_U gruopOad;
                //tmpRsv = g_gruop_param[g_pulse_main.nGroup-1].rsv;
                g_gruop_param[g_pulse_main.nGroup-1].nNum = groupindex + 1;
                gruopOad.value = 0x23010200 + (0x00010000) * (g_pulse_main.nGroup-1);
                int ret = write_normal_data((uint8*)&g_gruop_param[g_pulse_main.nGroup-1], sizeof(OOP_TGSETS_T), gruopOad, 0, 0, 2);
		        if(ret == 0){
                    MessageBox("�������óɹ���");
		            return FALSE;
		        }else{
		            MessageBox("��������ʧ�ܣ�");
		            return FALSE;
		        }*/
				g_group_detail.pConfig = NULL;
				UpdateWindow(g_guiComm.hMainWnd, TRUE);
				return FALSE;
			}
        }
        return TRUE;
    case KEY_CANCEL:
        /*if(TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
        {
            if((g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].tgFlag == 0) || (g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].opFlag == 0)
               || (g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len <= 0))
            {
                MessageBox("��ǰ���ò�����");
                return TRUE;
            }
            OOP_OAD_U gruopOad;
            g_gruop_param[g_pulse_main.nGroup-1].nNum = groupindex + 1;
            gruopOad.value = 0x23010200 + (0x00010000) * (g_pulse_main.nGroup-1);
            int ret = write_normal_data((uint8*)&g_gruop_param[g_pulse_main.nGroup-1], sizeof(OOP_TGSETS_T), gruopOad, 0, 0, 2);
            if(ret == 0){
                MessageBox("�������óɹ���");
                return FALSE;
            }else{
                MessageBox("��������ʧ�ܣ�");
                return FALSE;
            }
        }*/
       // memset(&g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex], 0, sizeof(OOP_TG_T));
        return FALSE;
    }
    return FALSE;
}
PLUGIN_INFO g_layerGroupConfig = {
	0x00630101/*id*/, 
	NULL/*InitLayer*/,
	NULL/*UninitLayer*/,
	GroupConfigDraw/*Draw*/,
	NULL/*Timer*/,
	GroupConfigKeyMsg/*keymsg*/,
	NULL/*ipc*/
};


#endif

void GetTotalMeasureInfo()
{
    memset(&g_TotalMeasureInfo, 0, sizeof(TOTAL_MEASURE_INFO));
    for (uint32 j=0;j<16;j++)
    {
        for(uint32 i = 0; i < g_gruop_oopMeter.nNum; i++){
            bool flag = true;
            for(uint8 m =0;m<OOP_TSA_SIZE-1;m++){
                if(g_gruop_param[g_pulse_main.nGroup-1].tg[j].add.add[m] != g_gruop_oopMeter.oopMeter[i].basic.tsa.add[m])
                {
                    flag = false;
                    break;
                }
            }
            if (flag == true){
                g_TotalMeasureInfo.nMeasureInfo[j][0] = g_gruop_oopMeter.nValue[i];
                g_TotalMeasureInfo.nMeasureInfo[j][1] = g_gruop_param[g_pulse_main.nGroup-1].tg[j].tgFlag+1;
                g_TotalMeasureInfo.nMeasureInfo[j][2] = g_gruop_param[g_pulse_main.nGroup-1].tg[j].opFlag+1;
            }
        }
    }
}

void appdb_meter_list_record_init(int clientid)
{
    int                ret = ERR_NORMAL;
    uint32             i = 0;
    uint32             oopMeterNum = 0;
    OOP_METER_T        oopMeter = {0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    memset(&g_gruop_oopMeter, 0, sizeof(OOP_METER));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = NO_VIR_MTR_MAX_NUM + 1;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60000200;

    //����Ҫ����У��
    stepReadRecord.MAC.nNum = 0;
    ret = db_read_record_start(clientid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        GUI_FMT_DEBUG("appdb_meter_list_record_init db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
            ret, inReadRecord.recordOAD.infoNum, recNum);
    }
    else
    {
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                GUI_FMT_DEBUG("appdb_meter_list_record_init db_read_record_step err = %d!\n", ret);
                continue;
            }

            //���������Ч��
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60000200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_METER_T)))
            {
                GUI_FMT_DEBUG("appdb_meter_list_record_init data err !\n");
                continue;
            }

            memcpy(&oopMeter, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);

            g_gruop_oopMeter.nValue[oopMeterNum] = oopMeter.nIndex;
            //������Ŵ�1��ʼ
            g_gruop_oopMeter.oopMeter[oopMeterNum++] = oopMeter;
            
        }
    }
    db_read_record_end(clientid, handle);

    /* ��¼�������� */
    g_gruop_oopMeter.nNum = oopMeterNum;

    return;
}
int flag;

#if DESC("�ܼ���������ϸ���",1)
void GroupDetailInit(DWORD dwPluginId)
{
    //appdb_meter_list_record_init(g_hUdbCenter);
    GetTotalMeasureInfo();
    memset(g_group_detail.nMeasureInfo, 0, sizeof(char)*16*3);
    memset(g_group_detail.sAddr, 0, 32*16);
    g_group_detail.nSel = 0;
    g_group_detail.nPage = 0;
	g_group_detail.pConfig = NULL;
	for(int i = 0; i < 16; i ++)
	{
        g_group_detail.nMeasureInfo[i][0] = g_TotalMeasureInfo.nMeasureInfo[i][0];
        g_group_detail.nMeasureInfo[i][1] = g_TotalMeasureInfo.nMeasureInfo[i][1];
        g_group_detail.nMeasureInfo[i][2] = g_TotalMeasureInfo.nMeasureInfo[i][2];
	}
    flag = 0;
    g_group_detail.nSel = 0;
}
void GroupDetailDraw(HDC hdc)
{
    if(g_group_detail.pConfig != NULL)
    {
        g_group_detail.pConfig->pfnDraw(hdc);
        return;
    }
    SelectFont(hdc,g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    char czBuf[300];
    sprintf(czBuf, "  ��·ͨ�ŵ�ַ  ����  �ۼӱ�־  ");
    rc.top = 15;
    rc.bottom = rc.top+15;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);
    bzero(czBuf,sizeof(czBuf));
    uint16 offset = 0;
	// ���1-8����������  
    int nTop = rc.top+15;
	int nLeft1 = 6;
	for( int i = 0; i < 8; i ++)
	{   
        if (flag ==1){
            groupindex = i + 8;
        }else{
            groupindex = i;
        }
        bzero(czBuf,sizeof(czBuf));
        if (g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len <= 0)
        {
            sprintf(czBuf, "%02d: ------------     -      -", groupindex+1);
        }
        else
        {
            offset = 0;
            memset(gruopBuf, 0, offset);
            for (uint8 j = 0; j <= g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.len; j++)
            {
                offset += sprintf(gruopBuf + offset, "%02X", g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].add.add[j]);
            }
            sprintf(czBuf, "%02d: %s    %s     %s", groupindex+1, gruopBuf, g_pczDirect[g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].tgFlag],
                                                                          g_pczOperator[g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex].opFlag]);
        }
		if(i == g_group_detail.nSel)
		{
			ITEM_SELECT;
			TextOut(hdc, nLeft1, nTop, czBuf);
			ITEM_NOT_SELECT;
		}
        else
        {
           TextOut(hdc, nLeft1, nTop, czBuf);
        }
        nTop+=12;
	}
    if (flag == 0)
        sprintf(czBuf, "��һҳ");
    else
        sprintf(czBuf, "��һҳ");
    if(8 == g_group_detail.nSel)
    {
        ITEM_SELECT;
        TextOut(hdc, 200, 110, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 200, 110, czBuf);
    }

    /*
	// ����9-16�Ĳ�����
	nTop = rc.top+15;
	nLeft1 = 123;
	nLeft2 = nLeft1 + 35;
	nLeft3 = nLeft2 + 35;
	for(; i < 16; i ++)
	{
		if(i == g_group_detail.nSel)
		{
			sprintf(czBuf, "            ");
			ITEM_SELECT;
			TextOut(hdc, nLeft1, nTop, czBuf);
			ITEM_NOT_SELECT;
		}
		if(g_group_detail.nMeasureInfo[i][0] > 0)
		{
			sprintf(czBuf, "#%02d", g_group_detail.nMeasureInfo[i][0]);
		}
		else
		{
			sprintf(czBuf, "# -");
		}
		if(i == g_group_detail.nSel)
		{
			ITEM_SELECT;
			TextOut(hdc, nLeft1, nTop, czBuf);
			TextOut(hdc, nLeft2, nTop, g_pczDirect[g_group_detail.nMeasureInfo[i][1]]);
			TextOut(hdc, nLeft3, nTop, g_pczOperator[g_group_detail.nMeasureInfo[i][2]]);
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOut(hdc, nLeft1, nTop, czBuf);
			TextOut(hdc, nLeft2, nTop, g_pczDirect[g_group_detail.nMeasureInfo[i][1]]);
			TextOut(hdc, nLeft3, nTop, g_pczOperator[g_group_detail.nMeasureInfo[i][2]]);
		}		
		nTop += 12;
	}
    */
    PopListDraw(hdc);
}
BOOL GroupDetailKeyMsg(int nKey)
{
    // �ȴ������������ҳ����Ϣ
	if(g_group_detail.pConfig != NULL)
    {
        BOOL bRet = FALSE;
        if(g_group_detail.pConfig->pfnKeyMsg != NULL)
        {
            bRet = g_group_detail.pConfig->pfnKeyMsg(nKey);
        }
		// ȱʡ����
        if((bRet == FALSE && nKey==KEY_CANCEL) || bRet == RET_CANCEL)
        {
            if(g_group_detail.pConfig->pfnUninitLayer != NULL)
            {
                g_group_detail.pConfig->pfnUninitLayer();
            }
            g_group_detail.pConfig = NULL;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch(nKey)
    {
    case KEY_UP:
        g_group_detail.nSel --;
        if(g_group_detail.nSel < 0 )
        {
            g_group_detail.nSel = 8;
            return FALSE;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:
        g_group_detail.nSel ++;
        if(g_group_detail.nSel > 8)
        {
            g_group_detail.nSel = 0;
            return FALSE;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
	/*case KEY_LEFT:
		if(g_group_detail.nSel > 7)
		{
			g_group_detail.nSel -= 8;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
	case KEY_RIGHT:
		if(g_group_detail.nSel < 8)
		{
			g_group_detail.nSel += 8;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}*/
    case KEY_OK:
        if (g_group_detail.nSel == 8)
        {
            if (flag == 0){
                flag = 1;
                g_group_detail.nSel = 0;
            }
            else{
                flag = 0;
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }else{
            if(ValidatePwd() == TRUE)
            {// �����޸Ĳ�������Ϣҳ��
                gCfgIndex = 0;
                if (flag == 0){
                    groupindex = g_group_detail.nSel;
                }
                else{
                    groupindex = g_group_detail.nSel + 8;
                }
                if((groupindex - 1 >= 0)&&(g_gruop_param[g_pulse_main.nGroup-1].tg[groupindex - 1].add.len <=0)){
                    char czBuf[100];
                    sprintf(czBuf, "��·%dû������,����˳�����ã�",groupindex);
                    MessageBox(czBuf);
                }
                else{
                    g_group_detail.pConfig = &g_layerGroupConfig;
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                }
            }
        return TRUE;
        }
    }
    return FALSE;
}
PLUGIN_INFO g_layerGroupDetail = {
	0x20760001/*id*/, 
	NULL/*InitLayer*/,
	NULL/*UninitLayer*/,
	GroupDetailDraw/*Draw*/,
	NULL/*Timer*/,
	GroupDetailKeyMsg/*keymsg*/,
	NULL/*ipc*/
};


#endif
int nGroupNum;
void GroupMainInit(DWORD dwPluginId)
{
    //tmpRsv = 0;
    for(int i =0;i<8;i++){
        memset(&g_gruop_param[i], 0, sizeof(OOP_TGSETS_T));
	    g_pulse_main.nIndex = (g_guiComm.bKeyDown == TRUE)?0:2;
	
        uint32 oadvalue;
	    g_pulse_main.nGroup = i + 1;
        oadvalue = 0x23010200 + (0x00010000) * (g_pulse_main.nGroup-1);
        read_db_noamal_data(g_hUdbCenter, oadvalue, 0, 0, (uint8*)&g_gruop_param[i], sizeof(OOP_TGSETS_T));
    }
    g_pulse_main.nGroup = 1;
	// ״̬��־
	if(g_gruop_param[g_pulse_main.nGroup-1].nNum == 0)
    {
        g_pulse_main.nState = 0;
    }
    else
    {
        g_pulse_main.nState = 1;
    }
    nGroupNum = g_pulse_main.nGroup;
    flag = 0;
}

void GroupMainDraw(HDC hdc)
{
    // ������ϸ���û���
	if(g_pulseDetail != NULL)
    {
        g_pulseDetail->pfnDraw(hdc);
        return;
    }
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontBig);
	
	RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

	char czBuf[100];
	sprintf(czBuf, "�ܼ���������Ϣ");

    rc.top = 20;
    rc.bottom = rc.top+16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

	// ����·
    int nLeft1 = 20;
	int nLeft2 = 130;
	int nTop = 20;
	nTop += 26;
	TextOut(hdc, nLeft1, nTop, "�ܼ������");
	sprintf(czBuf, "%d", g_pulse_main.nGroup);
    if(g_pulse_main.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_pczGroup[g_pulse_main.nGroup-1]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_pczGroup[g_pulse_main.nGroup-1]);
    }

    if(nGroupNum != g_pulse_main.nGroup){
        if(g_gruop_param[g_pulse_main.nGroup-1].nNum == 0)
        {
            g_pulse_main.nState = 0;
        }
        else
        {
            g_pulse_main.nState = 1;
        }
        nGroupNum = g_pulse_main.nGroup;
    }
	// ״̬��־
	nTop += 18;
	TextOut(hdc, nLeft1, nTop, "״̬��־");
    if(g_pulse_main.nIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_pczPulseStatus[g_pulse_main.nState]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_pczPulseStatus[g_pulse_main.nState]);
    }

	// ��ϸ����
	nTop += 23;
	if(g_pulse_main.nState == 1)
	{
		if(g_pulse_main.nIndex == 2)
		{
			ITEM_SELECT;
			TextOut(hdc, nLeft2, nTop, "��ϸ����");
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOut(hdc, nLeft2, nTop, "��ϸ����");
		}
	}
	// ��������
	if(g_pulse_main.nState == 1)
	{
		if(g_pulse_main.nIndex == 3)
		{
			ITEM_SELECT;
			TextOut(hdc, nLeft2, nTop+20, "��������");
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOut(hdc, nLeft2, nTop+20, "��������");
		}
	}else{
	    if(g_pulse_main.nIndex == 2)
		{
			ITEM_SELECT;
			TextOut(hdc, nLeft2, nTop+20, "��������");
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOut(hdc, nLeft2, nTop+20, "��������");
		}
	}
	PopListDraw(hdc);
}

BOOL GroupMainKeyMsg(int nKey)
{
    // ������ϸ���ð�����Ϣ
	if(g_pulseDetail != NULL)
    {
        BOOL bRet = FALSE;
        if(g_pulseDetail->pfnKeyMsg != NULL)
        {
            bRet = g_pulseDetail->pfnKeyMsg(nKey);
        }
		// ȱʡ����
        if((bRet == FALSE && nKey==KEY_CANCEL) || bRet == RET_CANCEL)
        {// �˳�ҳ���
            if(g_pulseDetail->pfnUninitLayer != NULL)
            {
                g_pulseDetail->pfnUninitLayer();
            }
            g_pulseDetail = NULL;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }
	// ����List��������
	if(g_guiComm.bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if(nKey == KEY_OK)
        {			
            switch(g_pulse_main.nIndex)
            {
			case 0:// ����·���ı䣬���¶�ȡ������Ϣ
				g_pulse_main.nGroup = nSel+1;
				break;
			case 1:
				g_pulse_main.nState = nSel;
				break;
            }
			
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch(nKey)
    {
    case KEY_UP:
        g_pulse_main.nIndex --;
        if(g_pulse_main.nIndex < 0)
        {
            g_pulse_main.nIndex = 0;
            return FALSE;
        }
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
    case KEY_DOWN:
		{
			g_pulse_main.nIndex ++;
		    int nMax;
		    if (g_pulse_main.nState==1)
			    nMax = 3;
			else
			    nMax = 2;
			if(g_pulse_main.nIndex > nMax)
			{
				g_pulse_main.nIndex = nMax;
				return FALSE;
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
	case KEY_LEFT:
		{
			if(g_pulse_main.nIndex == 0)
			{
				g_pulse_main.nGroup --;
				if(g_pulse_main.nGroup <= 0)
				{
					g_pulse_main.nGroup = 8;
				}
				UpdateWindow(g_guiComm.hMainWnd, TRUE);
				return TRUE;
			}
		}
		break;
	case KEY_RIGHT:
		{
			if(g_pulse_main.nIndex == 0)
			{
				g_pulse_main.nGroup ++;
				if(g_pulse_main.nGroup > 8)
				{
					g_pulse_main.nGroup = 1;
				}
				UpdateWindow(g_guiComm.hMainWnd, TRUE);
				return TRUE;
			}
		}
		break;
    case KEY_OK:
        {// ȷ�ϰ�������
			int nLeft = 130;
            int nIndex = g_pulse_main.nIndex;
            int nTop = nIndex*16+46;
			
			switch(g_pulse_main.nIndex)
			{
			case 0:// ��������ѡ��List
				PopList(g_pczGroup, 8, g_pczGroup[g_pulse_main.nGroup-1], nLeft, nTop-30, TRUE);
				break;
			case 1:
				if(ValidatePwd() == TRUE)
				{// ��������״̬ѡ��List
					PopList(g_pczPulseStatus, 2, g_pczPulseStatus[g_pulse_main.nState], nLeft, nTop, TRUE);
				}
				break;
			case 2:
				if(g_pulse_main.nState==1){// ������ϸ����ҳ���
					g_pulseDetail = &g_layerGroupDetail;
					if(g_pulseDetail->pfnInitLayer != NULL)
					{
						g_pulseDetail->pfnInitLayer(g_pulseDetail->dwId);
					}
					UpdateWindow(g_guiComm.hMainWnd, TRUE);
				}else{// ��������
			        OOP_OAD_U gruopOad;
                    memset(&g_gruop_param[g_pulse_main.nGroup-1], 0, sizeof(OOP_TGSETS_T));
                    memset(&g_TotalMeasureInfo, 0, sizeof(GROUP_DETAIL));
                    memset(&g_group_detail, 0, sizeof(TOTAL_MEASURE_INFO));
					gruopOad.value = 0x23010200 + (0x00010000) * (g_pulse_main.nGroup-1);
					int ret = write_normal_data((uint8*)&g_gruop_param[g_pulse_main.nGroup-1], sizeof(OOP_TGSETS_T), gruopOad, 0, 0, 2);
		            if(ret == 0){
		                MessageBox("������óɹ���");
		                return TRUE;
		            }else{
		                MessageBox("�������ʧ�ܣ�");
		                return FALSE;
		            }
				}
				break;
			case 3:// ��������
			    OOP_OAD_U gruopOad;
				g_gruop_param[g_pulse_main.nGroup-1].nNum = 0;
				for(int i = 0; i < 16; i++)
				{
			        if (g_gruop_param[g_pulse_main.nGroup-1].tg[i].add.len <= 0)
					{
						break;
					}
			        else
					{
						g_gruop_param[g_pulse_main.nGroup-1].nNum++;
					}
				}
			    gruopOad.value = 0x23010200 + (0x00010000) * (g_pulse_main.nGroup-1);
				int ret = write_normal_data((uint8*)&g_gruop_param[g_pulse_main.nGroup-1], sizeof(OOP_TGSETS_T), gruopOad, 0, 0, 2);
		        if (ret == 0){
					MessageBox("�������óɹ���");
		            return TRUE;
		        }else{
		            MessageBox("��������ʧ�ܣ�");
		            return FALSE;
		        }
			    break;
			}
            return TRUE;
        }
    }
    return FALSE;
}

PLUGIN_INFO g_layerGroupMain = {
	0x20760000/*id*/, 
	GroupMainInit/*InitLayer*/,
	UninitDefault/*UninitLayer*/,
	GroupMainDraw/*Draw*/,
	NULL/*Timer*/,
	GroupMainKeyMsg/*keymsg*/,
	NULL/*ipc*/
};

#endif

#if DESC("ʱ������",1)
/************************************************************************/
/* �ն�ʱ������                                                         */
/************************************************************************/
typedef struct _RESET_TIME
{
	int     nIndex;     // ��ǰ����
	char    czInfo[20]; // ��ǰʱ���ַ���������:

	struct tm   tmNow;

}SETRESET_TIME;
SETRESET_TIME g_set_time;

void Set_ParaResetTimeInit(DWORD dwPluginId)
{
	        GUI_FMT_DEBUG("Set_ParaResetTimeInit 111111 \n");
	g_guiComm.bCanTrunDisp = FALSE;
	g_set_time.nIndex = 0;

	// ��ȡ��ǰʱ��
	long lTime = time(NULL);
	tm* tnow = localtime(&lTime);
	memcpy(&g_set_time.tmNow, tnow, sizeof(tm));

	sprintf(g_set_time.czInfo, "%04d%02d%02d%02d:%02d:%02d",
		g_set_time.tmNow.tm_year + 1900,
		g_set_time.tmNow.tm_mon + 1,
		g_set_time.tmNow.tm_mday,
		g_set_time.tmNow.tm_hour,
		g_set_time.tmNow.tm_min,
		g_set_time.tmNow.tm_sec);
}
void Set_ParaResetTimeDraw(HDC hdc)
{
	SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 25;
    rc.bottom = rc.top+16;
    char pcz[20];

	strcpy(pcz, "�ն�ʱ������");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

	// ��������
    int nLeft = 64;
    int nTop = rc.top+30;
       int i;
    for(i = 0; i < 8; i ++)
    {
        switch(i)
        {
        case 4:TextOut(hdc, nLeft, nTop, "��");nLeft+=16;break;
        case 6:TextOut(hdc, nLeft, nTop, "��");nLeft+=16;break;
        }

		if (g_set_time.nIndex == i)
		{
			ITEM_SELECT;
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_set_time.czInfo[i], 1);
			ITEM_NOT_SELECT;
		}
		else
		{
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_set_time.czInfo[i], 1);
		}
		nLeft += 8;
	}
	TextOut(hdc, nLeft, nTop, "��");

	// ����ʱ��
    nLeft = 88;
    nTop += 20;
    for(; i < 16; i ++)
    {
		if (g_set_time.nIndex == i)
        {
            ITEM_SELECT;
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_set_time.czInfo[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
			TextOutLen(hdc, nLeft, nTop, (const char*)&g_set_time.czInfo[i], 1);
        }
        nLeft += 8;
    }

	// ȷ��
	nTop += 20;
	nLeft = 168;
	if (g_set_time.nIndex == i)
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
BOOL Set_ParaResetTimeKeyMsg(int nKey)
{
	int iRet = 0;

	switch (nKey)
	{
	case KEY_LEFT:// �޸����뽹��
		g_set_time.nIndex--;
		if (g_set_time.czInfo[g_set_time.nIndex] < '0'
			|| g_set_time.czInfo[g_set_time.nIndex] > '9')
		{
			g_set_time.nIndex--;
		}
		if (g_set_time.nIndex < 0)
		{
			g_set_time.nIndex = 16;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	case KEY_RIGHT:// �޸����뽹��
		g_set_time.nIndex++;
		if ((g_set_time.czInfo[g_set_time.nIndex] < '0'
			|| g_set_time.czInfo[g_set_time.nIndex] > '9')
			&& g_set_time.nIndex != 16)
		{
			g_set_time.nIndex++;
		}
		if (g_set_time.nIndex > 16)
		{
			g_set_time.nIndex = 0;
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
	case KEY_UP:// �޸�ʱ����������ж���Чλ����һЩ�ж�
		if (g_set_time.nIndex >= 0 && g_set_time.nIndex <= 15)
		{
			char czMax = '9';
			switch (g_set_time.nIndex)
			{
			case 0:czMax = '2'; break;   // ��ǧλ
			case 4:czMax = '1'; break;   // ��ʮλ
			case 6:czMax = '3'; break;   // ��ʮλ
			case 8:czMax = '2'; break;   // Сʱʮλ
			case 11:czMax = '5'; break;  // ��ʮλ
			case 14:czMax = '5'; break;  // ��ʮλ
			}
			g_set_time.czInfo[g_set_time.nIndex] ++;
			if (g_set_time.czInfo[g_set_time.nIndex] > czMax)
			{
				if (g_set_time.nIndex == 0)
					g_set_time.czInfo[g_set_time.nIndex] = '1';
				else
					g_set_time.czInfo[g_set_time.nIndex] = '0';
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
		break;
	case KEY_DOWN:// �޸�ʱ����������ж���Чλ����һЩ�ж�
		if (g_set_time.nIndex >= 0 && g_set_time.nIndex <= 15)
		{
			char czMax = '9';
			switch (g_set_time.nIndex)
			{
			case 0:czMax = '2'; break;   // ��ǧλ
			case 4:czMax = '1'; break;   // ��ʮλ
			case 6:czMax = '3'; break;   // ��ʮλ
			case 8:czMax = '2'; break;   // Сʱʮλ
			case 11:czMax = '5'; break;  // ��ʮλ
			case 14:czMax = '5'; break;  // ��ʮλ
			}
			g_set_time.czInfo[g_set_time.nIndex] --;
			if (g_set_time.czInfo[g_set_time.nIndex] < '0')
			{
				g_set_time.czInfo[g_set_time.nIndex] = czMax;
			}
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return TRUE;
		}
		break;
	case KEY_OK:
		printf("ENTER KEY_OK\n");
		if (g_set_time.nIndex == 16 && ValidatePwd() == TRUE)
		{
			char czBuf[20];
			int nVal;
			// ��
			memcpy(czBuf, g_set_time.czInfo, 4);
			czBuf[4] = '\0';
			nVal = atoi(czBuf);
			if (nVal > 2099)
			{
				g_set_time.nIndex = 3;
				MessageBox("���ֵ���ܳ���2099��\n\n���������룡", MB_OK);
				return TRUE;
			}
			if (nVal < 2000)
			{
				g_set_time.nIndex = 3;
				MessageBox("���ֵ���ܵ���2000��\n\n���������룡", MB_OK);
				return TRUE;
			}
			g_set_time.tmNow.tm_year = nVal;

			// ��
			memcpy(czBuf, g_set_time.czInfo + 4, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			if (nVal == 0)
			{
				g_set_time.nIndex = 5;
				MessageBox("�·�ֵ����Ϊ���·�\n\n���������룡", MB_OK);
				return TRUE;
			}
			if (nVal > 12)
			{
				g_set_time.nIndex = 5;
				MessageBox("�·�ֵ���ܴ���12�·�\n\n���������룡", MB_OK);
				return TRUE;
			}
			g_set_time.tmNow.tm_mon = nVal;

			// ��
			memcpy(czBuf, g_set_time.czInfo + 6, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			if (nVal == 0)
			{
				g_set_time.nIndex = 7;
				MessageBox("����ֵ����Ϊ��\n\n���������룡", MB_OK);
				return TRUE;
			}
			if (nVal > 31)
			{
				g_set_time.nIndex = 7;
				MessageBox("����ֵ������Χ\n\n���������룡", MB_OK);
				return TRUE;
			}
			g_set_time.tmNow.tm_mday = nVal;

			// Сʱ
			memcpy(czBuf, g_set_time.czInfo + 8, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			if (nVal > 23)
			{
				g_set_time.nIndex = 9;
				MessageBox("Сʱֵ������Χ\n\n���������룡", MB_OK);
				return TRUE;
			}
			g_set_time.tmNow.tm_hour = nVal;

			// ��
			memcpy(czBuf, g_set_time.czInfo + 11, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			g_set_time.tmNow.tm_min = nVal;

			// ��
			memcpy(czBuf, g_set_time.czInfo + 14, 2);
			czBuf[2] = '\0';
			nVal = atoi(czBuf);
			g_set_time.tmNow.tm_sec = nVal;


			DateTimeHex_t pDateTime;
			memset(&pDateTime, 0, sizeof(DateTimeHex_t));

			pDateTime.year_h = g_set_time.tmNow.tm_year >> 8;
			pDateTime.year_l = g_set_time.tmNow.tm_year & 0xff;
			pDateTime.month = g_set_time.tmNow.tm_mon;
			pDateTime.day = g_set_time.tmNow.tm_mday;
			pDateTime.hour = g_set_time.tmNow.tm_hour;
			pDateTime.min = g_set_time.tmNow.tm_min;
			pDateTime.sec = g_set_time.tmNow.tm_sec;
			g_set_time.nIndex = 0;

			int year = 0;
			year = (uint16)pDateTime.year_l + (uint16)(pDateTime.year_h << 8);

			GUI_FMT_TRACE("set time modify time: %d-%d-%d %d:%d:%d]\n", year, pDateTime.month, pDateTime.day, pDateTime.hour, pDateTime.min, pDateTime.sec);
			iRet = systime_set_to_smiOS(pDateTime);
			if (iRet < 0)
			{
                 	 MessageBox("�ն�ʱ������ʧ�ܣ�", MB_OK);
				return FALSE;
			}
			GUI_FMT_TRACE("������ʱ�����óɹ���\n");
         	MessageBox("�ն�ʱ�����óɹ���", MB_OK);
			return TRUE;
		}
		break;
	case KEY_CANCEL:
		CancelMenu();
		return TRUE;
		break;
	}
	return FALSE;
}
PLUGIN_INFO g_Set_ParaResetTime = {
	0x20710000/*id*/,
	Set_ParaResetTimeInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	Set_ParaResetTimeDraw/*Draw*/,
	NULL/*Timer*/,
	Set_ParaResetTimeKeyMsg/*keymsg*/,
	NULL/*ipc*/
};
/************************************************************************/
/* �ն�ʱ�����ý���                                                     */
/************************************************************************/

#endif

#if DESC("�������", 1)
/************************************************************************/
/* �������                                                             */
/************************************************************************/
typedef struct _CHANGE_PWD
{
    int     nIndex;     // ��ǰ����
    char    czPwd[PASSWORD_LEN*3+5]; // ������
}CHANGE_PWD;
CHANGE_PWD g_change_pwd;
void ChangePwdInit(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;
    g_change_pwd.nIndex = PASSWORD_LEN;
    memset(g_change_pwd.czPwd, '0', sizeof(g_change_pwd.czPwd));
    g_change_pwd.czPwd[PASSWORD_LEN] = '\0';
    g_change_pwd.czPwd[PASSWORD_LEN*2+1] = '\0';
    g_change_pwd.czPwd[PASSWORD_LEN*3+2] = '\0';
}
void ChangePwdDraw(HDC hdc)
{
    ITEM_NOT_SELECT;
	SelectFont(hdc,g_guiComm.fontBig);

    TextOut(hdc,90,16,"������");

    int nLeft=108;
    int nTop=34;
    TextOut(hdc,3, nTop, "�����������");

	// ������
    nLeft+=8;
    int i = 0;
    for(i = 0; i < PASSWORD_LEN; i ++)
    {
        if(i == g_change_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop,(const char*)&g_change_pwd.czPwd[i],1);
            ITEM_NOT_SELECT;
        }
        else
        { 
            TextOutLen(hdc, nLeft, nTop,(const char*)&g_change_pwd.czPwd[i],1);
        }
        nLeft+=12;
    }
	
	Rectangle(hdc, 108, nTop, 108+83, nTop+16);
	
    nLeft+=15;
    if(i == g_change_pwd.nIndex)
    {
       ITEM_SELECT;
       TextOut(hdc, nLeft, nTop,"��ϸ");
       ITEM_NOT_SELECT;
   }
   else
   {
       TextOut(hdc, nLeft, nTop,"��ϸ");
   }
   i++;

   // ������
    nLeft=108;
    nTop += 18;
    TextOut(hdc,3, nTop, "������������");

    nLeft+=8;
    for(; i < PASSWORD_LEN*2+1; i ++)
    {
        if(i == g_change_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop,(const char*)&g_change_pwd.czPwd[i],1);
            ITEM_NOT_SELECT;
        }
        else
        { 
            TextOutLen(hdc, nLeft, nTop,(const char*)&g_change_pwd.czPwd[i],1);
        }
        nLeft+=12;
    }
	
	Rectangle(hdc, 108, nTop, 108+83, nTop+16);
	
    nLeft+=15;
    if(i == g_change_pwd.nIndex)
    {
       ITEM_SELECT;
       TextOut(hdc, nLeft, nTop,"��ϸ");
       ITEM_NOT_SELECT;
   }
   else
   {
       TextOut(hdc, nLeft, nTop,"��ϸ");
   }
   i++;

   // ��֤����
    nLeft=108;
    nTop += 18;
    TextOut(hdc,3, nTop, "���ٴ�����������");
    nTop += 16;

    nLeft+=8;
    for(; i < PASSWORD_LEN*3+2; i ++)
    {
        if(i == g_change_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop,(const char*)&g_change_pwd.czPwd[i],1);
            ITEM_NOT_SELECT;
        }
        else
        { 
            TextOutLen(hdc, nLeft, nTop,(const char*)&g_change_pwd.czPwd[i],1);
        }
        nLeft+=12;
    }
	
	Rectangle(hdc, 108, nTop, 108+83, nTop+16);

    nLeft+=15;
    if(i == g_change_pwd.nIndex)
    {
       ITEM_SELECT;
       TextOut(hdc, nLeft, nTop,"��ϸ");
       ITEM_NOT_SELECT;
   }
   else
   {
       TextOut(hdc, nLeft, nTop,"��ϸ");
   }
   i++;
   
 nTop += 17;
 
   // ȷ��
   if(g_change_pwd.nIndex == PASSWORD_LEN*3+3)
   {
       ITEM_SELECT;
       TextOut(hdc, nLeft, nTop,"ȷ��");
       ITEM_NOT_SELECT;
   }
   else
   {
       TextOut(hdc, nLeft, nTop,"ȷ��");
   }
}

// ��������ҳ�淵�ش���
void ChangePwdReturn(char* pNewStr, void* pPara)
{
    if(strlen(pNewStr) != PASSWORD_LEN && memcmp("334455", pNewStr, 6) != 0)
    {
		char msg[64];
		sprintf(msg, "���볤�ȱ���Ϊ%d�ֽ�\n\nȷ���Ƿ���������?", PASSWORD_LEN);
        if(TRUE == MessageBox(msg, MB_OKCANCEL))
        {
            InputBox(g_change_pwd.czPwd, ChangePwdReturn);
        }
    }
    else
    {
        switch(g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
            strcpy(g_change_pwd.czPwd, pNewStr);break;
        case PASSWORD_LEN*2+1:
			strcpy(g_change_pwd.czPwd+PASSWORD_LEN+1, pNewStr);break;
        case PASSWORD_LEN*3+2:
			strcpy(g_change_pwd.czPwd+(PASSWORD_LEN+1)*2, pNewStr);break;
        }
    }
}

BOOL ChangePwdKeyMsg(int nKey)
{
    switch(nKey)
    {
    case KEY_UP:// �����ڼ�����ϸ�������ƶ�
        switch(g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:return FALSE;
		case (PASSWORD_LEN*2)+1:
            g_change_pwd.nIndex=PASSWORD_LEN;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN*3)+2:
            g_change_pwd.nIndex=(PASSWORD_LEN*2)+1;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN*3)+3:
            g_change_pwd.nIndex=(PASSWORD_LEN*3)+2;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        default:
            g_change_pwd.czPwd[g_change_pwd.nIndex] ++;
            if(g_change_pwd.czPwd[g_change_pwd.nIndex] > '9')
            {
                g_change_pwd.czPwd[g_change_pwd.nIndex] = '0';
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    case KEY_DOWN:// �����ڼ�����ϸ�������ƶ�
        switch(g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
            g_change_pwd.nIndex=(PASSWORD_LEN*2)+1;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN*2)+1:
            g_change_pwd.nIndex=(PASSWORD_LEN*3)+2;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN*3)+2:
            g_change_pwd.nIndex=(PASSWORD_LEN*3)+3;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN*3)+3:return FALSE;
        default:
            g_change_pwd.czPwd[g_change_pwd.nIndex] --;
            if(g_change_pwd.czPwd[g_change_pwd.nIndex] < '0')
            {
                g_change_pwd.czPwd[g_change_pwd.nIndex] = '9';
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    case KEY_LEFT:// �޸�����
        g_change_pwd.nIndex --;
        if(g_change_pwd.nIndex < 0)
        {
            g_change_pwd.nIndex = PASSWORD_LEN*3+3;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:// �޸�����
        g_change_pwd.nIndex ++;
        if(g_change_pwd.nIndex > PASSWORD_LEN*3+3)
        {
            g_change_pwd.nIndex = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
		// ������ϸ��������������ҳ��
        switch(g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
            {
                InputBox(g_change_pwd.czPwd, ChangePwdReturn);
            }
            break;
        case (PASSWORD_LEN*2)+1:
            {
                char* pBuf = g_change_pwd.czPwd;
                InputBox(pBuf+PASSWORD_LEN+1, ChangePwdReturn);
            }
            break;
        case (PASSWORD_LEN*3)+2:
            {
                char* pBuf = g_change_pwd.czPwd;
                InputBox(pBuf+(PASSWORD_LEN*2)+2, ChangePwdReturn);
            }
            break;
        }
		// �޸�����
        if(g_change_pwd.nIndex == PASSWORD_LEN*3+3)
        {
            // �������ֵ��ж�ȡ
			char czPwd[PASSWORD_LEN*3+5];
#ifdef _MSC_VER
			memcpy(czPwd, PASSWORD_STR, PASSWORD_LEN+1);
#else
			int nRet = read_pdata_xram(GUI_PRIVATE_DATA, czPwd, PRIVATE_PASSWORD, PASSWORD_LEN);
			if(nRet < 0)
			{
				memcpy(czPwd, PASSWORD_STR, PASSWORD_LEN+1);
			}
#endif

			// �ȽϾ�����
#if LIAONING_FUN == 1
			if(memcmp(czPwd, g_change_pwd.czPwd, PASSWORD_LEN) != 0 && memcmp(PASSWORD_STR, g_change_pwd.czPwd, PASSWORD_LEN))//������������������Զ���\0
#else
			if(memcmp(czPwd, g_change_pwd.czPwd, PASSWORD_LEN) != 0 && memcmp("334455", g_change_pwd.czPwd, 6) != 0)//������������������Զ���\0
#endif			
            {
                MessageBox("��ʾ\n�������������");
                return TRUE;
            }
			// �Ƚ�����������
            if(memcmp(g_change_pwd.czPwd+PASSWORD_LEN+1, 
                g_change_pwd.czPwd+PASSWORD_LEN*2+2, PASSWORD_LEN) != 0)
            {
                MessageBox("��ʾ\n�����������벻һ�£�");
                return TRUE;
            }
#ifndef _MSC_VER
            // ��������
			nRet = write_pdata_xram(GUI_PRIVATE_DATA, g_change_pwd.czPwd+PASSWORD_LEN+1, PRIVATE_PASSWORD, PASSWORD_LEN);
		//printf("WritePDataXram return %d\n", nRet);
            if(nRet >= 0)
			{
				MessageBox("��ʾ\n�����޸ĳɹ���");
			}
			else
			{
				MessageBox("��ʾ\n�����޸�ʧ�ܣ�");
			}
#endif
            return TRUE;
        }
        break;
    }
    return FALSE;
}
PLUGIN_INFO g_layerChangePwd = {
	0x20700000/*id*/, 
	ChangePwdInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	ChangePwdDraw/*Draw*/,
	NULL/*Timer*/,
	ChangePwdKeyMsg/*keymsg*/,
	NULL/*ipc*/
};
/************************************************************************/
/* ����������                                                         */
/************************************************************************/
#endif

#if DESC("IP����",1)
int guicmd_execute(const char* cmd,  char* result)
{
    char resultBuf[500];

    FILE* fp = NULL;

    if ((fp = popen(cmd, "r")) != NULL)
    {
        while( fgets(resultBuf, sizeof(resultBuf), fp) != NULL) 
        {
            strcat(result, resultBuf);
            if( strlen(result) > sizeof(resultBuf))
            {
                break;
            }
        }
        pclose(fp);
        fp = NULL;
    }
    else
    {
        return -1;
    }
    return 0;
}

uint8 EthIPIndex;
void ip_set_init(HDC hdc)
{
    memset(&EthIP, 0x00, sizeof(OOP_ETHIP_T));
    EthIPisSet = read_db_noamal_data(g_hUdbCenter, EthIPOad.value, 0, 0, (uint8*)&EthIP, sizeof(OOP_ETHIP_T));
//	if(0 != EthIPisSet )
//    {
//        char cmd[100] = {0};
//        char result[100] = {0};
//        uint32 ipaddr[4] = {0};
//        
//        strcpy(cmd,"ifconfig FE0 | grep 'inet addr' | awk '{ print $2}' | awk -F: '{print $2}' ");
//        guicmd_execute(cmd, result);
//        sscanf(result, "%u.%u.%u.%u", &EthIP.ip[0], &EthIP.ip[1], &EthIP.ip[2], &EthIP.ip[3]);
//
//		strcpy(cmd,"ifconfig FE0 | grep 'Mask' | awk '{ print $4}' | awk -F: '{print $2}' ");
//        guicmd_execute(cmd, result);
//        sscanf(result, "%u.%u.%u.%u", &EthIP.mask[0], &EthIP.mask[1], &EthIP.mask[2], &EthIP.mask[3]);
//        
//        strcpy(cmd,"route -n|grep FE0|grep UG| awk '{ print $2}'");
//        guicmd_execute(cmd, result);
//        sscanf(result, "%u.%u.%u.%u", &EthIP.ipGateway[0], &EthIP.ipGateway[1], &EthIP.ipGateway[2], &EthIP.ipGateway[3]);
//    }
	EthIP.ipConfig = 1;
    EthIPIndex = 0;
}

void ip_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    uint8 index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "�ն�IP����");
    rc.top = 15;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top+20;
    int nLeft1 = 25;
    int nLeft2 = 120;
    TextOut(hdc, nLeft1, nTop, "IP��ַ");
    memset(czBuf, 0x00, sizeof(czBuf));
	if (EthIPisSet < 0 || EthIP.ip[0] == 0)
    {
        strcpy(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthIP.ip[0], EthIP.ip[1], EthIP.ip[2], EthIP.ip[3]);
    }
    
    if (EthIPIndex == index)
    {
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��������");
    memset(czBuf, 0x00, sizeof(czBuf));
	if (EthIPisSet < 0 || EthIP.mask[0] == 0)
    {
        strcpy(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthIP.mask[0], EthIP.mask[1], EthIP.mask[2], EthIP.mask[3]);
    }
    
    if (EthIPIndex == index)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "���ص�ַ");
    memset(czBuf, 0x00, sizeof(czBuf));
	if (EthIPisSet < 0 || EthIP.ipGateway[0] == 0)
    {
        strcpy(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthIP.ipGateway[0], EthIP.ipGateway[1], EthIP.ipGateway[2], EthIP.ipGateway[3]);
    }
    
    if (EthIPIndex == index)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 30;

    if (EthIPIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+25, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+25, nTop, "ȡ��");
    }

    index++;
    nLeft2 = 140;
    if (EthIPIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "��������");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "��������");
    }

}

void IpSetReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    switch (EthIPIndex)
    {
    case 0: //ip��ַ
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthIPisSet < 0 || EthIP.ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.ip[0], EthIP.ip[1], EthIP.ip[2], EthIP.ip[3]);
                }
                InputBox(czBuf, IpSetReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(EthIP.ip, tmp, 4);
			EthIPisSet = 0;
        }
    }
    break;
    case 1://��������
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("���������ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthIPisSet < 0 || EthIP.mask[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.mask[0], EthIP.mask[1], EthIP.mask[2], EthIP.mask[3]);
                }
                InputBox(czBuf, IpSetReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(EthIP.mask, tmp, 4);
			EthIPisSet = 0;
        }
    }
    break;
    case 2://���ص�ַ
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("���ص�ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthIPisSet < 0 || EthIP.ipGateway[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.ipGateway[0], EthIP.ipGateway[1], EthIP.ipGateway[2], EthIP.ipGateway[3]);
                }
                InputBox(czBuf, IpSetReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(EthIP.ipGateway, tmp, 4);
			EthIPisSet = 0;
        }
    }
    break;
  
    break;
    }

}

BOOL ip_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 4;
    
    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if (EthIPIndex > 0)
            EthIPIndex--;
        else
            EthIPIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (EthIPIndex < maxindex)
            EthIPIndex++;
        else
            EthIPIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (ValidatePwd() == TRUE)
        {
            switch (EthIPIndex)
            {
            case 0://IP��ַ
            {
                char czBuf[20];
                if (EthIPisSet < 0 || EthIP.ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.ip[0], EthIP.ip[1], EthIP.ip[2], EthIP.ip[3]);
                }
                InputBox(czBuf, IpSetReturn, NULL, 0x02);
                return TRUE;
            }
            case 1://��������
            {
                char czBuf[20];
                if (EthIPisSet < 0 || EthIP.mask[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.mask[0], EthIP.mask[1], EthIP.mask[2], EthIP.mask[3]);
                }
                InputBox(czBuf, IpSetReturn, NULL, 0x02);
                return TRUE;
            }
            case 2://���ص�ַ
            {
                char czBuf[20];
                if (EthIPisSet < 0 || EthIP.ipGateway[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthIP.ipGateway[0], EthIP.ipGateway[1], EthIP.ipGateway[2], EthIP.ipGateway[3]);
                }
                InputBox(czBuf, IpSetReturn, NULL, 0x02);
                return TRUE;
            }
            case 3://ȡ��
            {
                if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
                {
                    ret = write_normal_data((uint8*)&EthIP, sizeof(OOP_ETHIP_T), EthIPOad, 0, 0, 1);
                    if (ret != 0)
                    {
                        MessageBox("IP��ַ����д��ʧ�ܣ�");
                    }
                    else
                    {
                        if(0 != ipconfig_send_to_smiOS((char*)"FE0", EthIP.ipConfig, EthIP.ip, EthIP.mask, EthIP.ipGateway))
                        {
                            MessageBox("����IP��ַʧ�ܣ�");
                        }
                        uint8 flag = 0;
                        OOP_OAD_U oadnum;
                        oadnum.value = 0x451004FF;
                        write_normal_data(&flag, sizeof(uint8), oadnum, 0, 0, 1);
                    }
                }
                EthIPIndex = 0;
				CancelMenu();
                return TRUE;
            }
            case 4://����
            {
                ret = write_normal_data((uint8*)&EthIP, sizeof(OOP_ETHIP_T), EthIPOad, 0, 0, 1);
                if (ret != 0)
                {
                    MessageBox("IP��ַ����д��ʧ�ܣ�");
                }
                else
                {
                    MessageBox("IP��ַ����д��ɹ���");
                    if(0 != ipconfig_send_to_smiOS((char*)"FE0", EthIP.ipConfig, EthIP.ip, EthIP.mask, EthIP.ipGateway))
                    {
                        MessageBox("����IP��ַʧ�ܣ�");
                    }
                    uint8 flag = 0;
                    OOP_OAD_U oadnum;
                    oadnum.value = 0x451004FF;
                    write_normal_data(&flag, sizeof(uint8), oadnum, 0, 0, 1);
                }
                CancelMenu();
                EthIPIndex = 0;
                return TRUE;
            }
            }
        }
    }
    }

    return FALSE;
}
//��̫��ͨ���鿴
PLUGIN_INFO g_layer_ip_set = {
    0x006F0000/*id*/,
    ip_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ip_set_draw/*Draw*/,
    NULL/*Timer*/,
    ip_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("��̨����", 1)
/************************************************************************/
/* ��̨��������                                                        */
/************************************************************************/
typedef struct _RADIO_CFG
{
    int		nIndex;
    int		nState;	// 0���� 1����ѯ��Ϣ 2��������Ϣ

    int		nPhone; // 0��ͨ����1�ر�ͨ��
    int		nChnnl;	// Ƶ��
    int		nPower;	// ����
    int		nYareGrade;	// ������

    int		nTimeOut;
	int     nMsgFlag;
} RADIO_CFG;

RADIO_CFG g_radio_cfg;
const char* g_pczPhone[] = {"��", "�ر�"};

const char* g_pczChnnl[] = {"CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8",
                      "CH9", "CH10", "CH11", "CH12", "CH13", "CH14", "CH15", "CH16",
                      "CH17", "CH18", "CH19", "CH20", "CH21", "CH22", "CH23", "CH24",
                      "CH25", "CH26", "CH27", "CH28", "CH29", "CH30", "CH31", "CH32",
                      "CH33", "CH34", "CH35", "CH36", "CH37", "CH38", "CH39", "CH40",
                      "CH41", "CH42", "CH43", "CH44", "CH45", "CH46", "CH47", "CH48"
                     };

const int MAX_CHANNELS = 48;
const char* g_pczPower[] = {"1.0w", "2.5w", "5.0w", "7.5w", "10.0w"};
const char* g_pczYareGrade[] = {"-12dBu", "-9dBu", "-6dBu", "-3dBu", "0dBu", "+3dBu", "+6dBu", "+9dBu", "+12dBu"};
void RadioCfgInit(DWORD dwPluginId)
{
    g_radio_cfg.nIndex = 3;
    g_guiComm.bCanTrunDisp = FALSE;

    g_radio_cfg.nState = 0;
    g_radio_cfg.nPhone = -1;
    g_radio_cfg.nChnnl = -1;
    g_radio_cfg.nPower = -1;
    g_radio_cfg.nYareGrade = -1;
}
void RadioCfgDraw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[100] = {0};
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 20;
    rc.bottom = rc.top+16;
	sprintf(czBuf, "��̨��������");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

    if(g_radio_cfg.nState == 1)
    {
        TextOut(hdc, 55, 70, "���ڲ�ѯ��̨����");
        return;
    }
    else if(g_radio_cfg.nState == 2)
    {
        TextOut(hdc, 55, 70, "�������õ�̨����");
        return;
    }

    int nTop = rc.top+22;
    int nLeft1 = 5;
    int nLeft2 = 80;

    TextOut(hdc, nLeft1, nTop, "Ƶ��");
    sprintf(czBuf, "%s", (g_radio_cfg.nChnnl==-1)?"--":g_pczChnnl[g_radio_cfg.nChnnl]);
    if(g_radio_cfg.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "����");
    sprintf(czBuf, "%s", (g_radio_cfg.nPower==-1)?"--":g_pczPower[g_radio_cfg.nPower]);
    if(g_radio_cfg.nIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "������");
    sprintf(czBuf, "%s", (g_radio_cfg.nYareGrade==-1)?"--":g_pczYareGrade[g_radio_cfg.nYareGrade]);
    if(g_radio_cfg.nIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    if(g_radio_cfg.nIndex == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, 160, 50, "��ѯ����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 160, 50, "��ѯ����");
    }

    if(g_radio_cfg.nIndex == 4)
    {
        ITEM_SELECT;
        TextOut(hdc, 160, 68, "���ò���");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 160, 68, "���ò���");
    }

    if(g_radio_cfg.nIndex == 5)
    {
        ITEM_SELECT;
        TextOut(hdc, 36, 100, "��ͨ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 36, 100, "��ͨ��");
    }

    if(g_radio_cfg.nIndex == 6)
    {
        ITEM_SELECT;
        TextOut(hdc, 136, 100, "�ر�ͨ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 136, 100, "�ر�ͨ��");
    }

    PopListDraw(hdc);
}
BOOL RadioCfgKeyMsg(int nKey)
{
    if(g_radio_cfg.nState != 0)
    {
        return TRUE;
    }
    // ����Listѡ����
    if(g_guiComm.bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if(nKey == KEY_OK)
        {
            switch(g_radio_cfg.nIndex)
            {
            case 0:
                g_radio_cfg.nChnnl = nSel;
                break;
            case 1:
                g_radio_cfg.nPower = nSel;
                break;
            case 2:
                g_radio_cfg.nYareGrade = nSel;
                break;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch(nKey)
    {
    case KEY_LEFT:
    case KEY_RIGHT:
        switch(g_radio_cfg.nIndex)
        {
        case 0:
        case 1:
            g_radio_cfg.nIndex = 3;
            break;
        case 2:
            g_radio_cfg.nIndex = 4;
            break;
        case 3:
            g_radio_cfg.nIndex = 0;
            break;
        case 4:
            g_radio_cfg.nIndex = 1;
            break;
        case 5:
            g_radio_cfg.nIndex = 6;
            break;
        case 6:
            g_radio_cfg.nIndex = 5;
            break;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:
        g_radio_cfg.nIndex ++;
        if(g_radio_cfg.nIndex > 6)
        {
            g_radio_cfg.nIndex = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_UP:
        g_radio_cfg.nIndex --;
        if(g_radio_cfg.nIndex < 0)
        {
            g_radio_cfg.nIndex = 6;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
    {
//        int nTop = 42+g_radio_cfg.nIndex*18;
        switch(g_radio_cfg.nIndex)
        {
        case 0:
            PopList(g_pczChnnl, MAX_CHANNELS, (g_radio_cfg.nChnnl==-1)?g_pczChnnl[0]:g_pczChnnl[g_radio_cfg.nChnnl], 80, 6, TRUE);
            break;
            break;
        case 1:
            PopList(g_pczPower, 5, (g_radio_cfg.nPower==-1)?g_pczPower[0]:g_pczPower[g_radio_cfg.nPower], 80, 28, TRUE);
            break;
            break;
        case 2:
            PopList(g_pczYareGrade, 9, (g_radio_cfg.nYareGrade==-1)?g_pczYareGrade[0]:g_pczYareGrade[g_radio_cfg.nYareGrade],
                    80, 16, TRUE);
            break;
            break;
        case 3:
        {
            config_get_for_radio();
            printf("��ѯ��̨����---");

            g_radio_cfg.nState = 1;
            g_radio_cfg.nTimeOut = 0;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        break;
        case 4:
        {
            if(g_radio_cfg.nChnnl == -1)
            {
                g_radio_cfg.nIndex = 0;
                MessageBox("��ʾ\n\n�����õ�̨Ƶ��");
                return TRUE;
            }
            if(g_radio_cfg.nPower == -1)
            {
                g_radio_cfg.nIndex = 1;
                MessageBox("��ʾ\n\n�����õ�̨����");
                return TRUE;
            }
            if(g_radio_cfg.nYareGrade == -1)
            {
                g_radio_cfg.nIndex = 2;
                MessageBox("��ʾ\n\n�����õ�̨������");
                return TRUE;
            }
            config_send_to_radio(g_radio_cfg.nChnnl, g_radio_cfg.nPower, g_radio_cfg.nYareGrade);
            MessageBox("���õ�̨�����ɹ�");

//            g_radio_cfg.nState = 2;
            g_radio_cfg.nTimeOut = 0;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        break;
        case 5:
        case 6:
        {
            if(g_radio_cfg.nIndex == 5)
            {
                yang_send_to_radio(2);
                MessageBox("��ʾ\n\n�Ѿ���ͨ��");
            }
            else
            {
                yang_send_to_radio(1);
                MessageBox("��ʾ\n\n�Ѿ��ر�ͨ��");
            }
        }
        break;
        }
    }
    return TRUE;
    }
    return FALSE;
}
void RadioCfgTimer(int nId)
{
    if(nId != TIMER_MAIN)
    {
        return;
    }
    if(g_radio_cfg.nState == 1)
    {
        // ��ѯ��̨����

        printf("��ȡ��̨��ѯ����---");
        g_radio_cfg.nTimeOut ++;
        if(g_radio_cfg.nTimeOut > 10)
        {
            g_radio_cfg.nState = 0;
            MessageBox("��ѯ��̨������ʱ");
            return;
        }

        if(g_radio_cfg.nMsgFlag == -1)
        {
            g_radio_cfg.nState = 0;
            MessageBox("��ѯ��̨����ʧ��");
            return;
        }

        UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return;
    }
//    else if(g_radio_cfg.nState == 2)
//    {
//        // ���õ�̨����
//        char czBuf[100];
//        int nRet = ReadRamXData(Radio_Channel_Set, czBuf);
//        printf("��ȡ��̨���ò���---");
//        PRINT_HEX(czBuf, 5);
//        if(nRet < 0 || (unsigned char)czBuf[0] != 0x81)
//        {
//            g_radio_cfg.nTimeOut ++;
//            if(g_radio_cfg.nTimeOut > 10)
//            {
//                g_radio_cfg.nState = 0;
//                MessageBox("���õ�̨������ʱ");
//                return;
//            }
//        }
//        else
//        {
//            if(czBuf[0] == (char)0xFF && czBuf[1] == (char)0xFF && czBuf[2] == (char)0xFF)
//            {
//                g_radio_cfg.nState = 0;
//                MessageBox("���õ�̨����ʧ��");
//                return;
//            }
//            else
//            {
//                g_radio_cfg.nState = 0;
//                MessageBox("���õ�̨�����ɹ�");
//                return;
//            }
//        }
//    }
}

PLUGIN_INFO g_layerRadioCfg =
{
    0x20B70000/*id*/,
    RadioCfgInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    RadioCfgDraw/*Draw*/,
    RadioCfgTimer/*Timer*/,
    RadioCfgKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
/************************************************************************/
/* ��̨�������ý���                                                       */
/************************************************************************/

#endif

#if DESC("��̨����",1)

void LongCfgInit(DWORD dwPluginId)
{
    g_radio_cfg.nIndex = 0;
}
void LongCfgDraw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[100] = {0};
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 20;
    rc.bottom = rc.top+16;
	sprintf(czBuf, "ͨ������");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

    int nTop = rc.top+22;
    int nLeft1 = 36;
    int nLeft2 = 136;

    if(g_radio_cfg.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "��ͨ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "��ͨ��");
    }

    if(g_radio_cfg.nIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "�ر�ͨ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "�ر�ͨ��");
    }

    nTop = nTop+22;
    if(g_radio_cfg.nIndex == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "����ȫ1");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "����ȫ1");
    }

    if(g_radio_cfg.nIndex == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "�رճ���ȫ1");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "�رճ���ȫ1");
    }

    nTop = nTop+22;
    if(g_radio_cfg.nIndex == 4)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "����ȫ0");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "����ȫ0");
    }

    if(g_radio_cfg.nIndex == 5)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "�رճ���ȫ0");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "�رճ���ȫ0");
    }

    PopListDraw(hdc);
}
BOOL LongCfgKeyMsg(int nKey)
{
    switch(nKey)
    {
    case KEY_RIGHT:
    case KEY_DOWN:
        g_radio_cfg.nIndex ++;
        if(g_radio_cfg.nIndex > 5)
        {
            g_radio_cfg.nIndex = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_UP:
    case KEY_LEFT:
        g_radio_cfg.nIndex --;
        if(g_radio_cfg.nIndex < 0)
        {
            g_radio_cfg.nIndex = 5;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
    {
        switch(g_radio_cfg.nIndex)
        {
        case 0:
        case 1:
        {
            if(g_radio_cfg.nIndex == 0)
            {
                yang_send_to_radio(2);
                MessageBox("��ʾ\n\n�Ѿ���ͨ��");
            }
            else
            {
                yang_send_to_radio(1);
                MessageBox("��ʾ\n\n�Ѿ��ر�ͨ��");
            }
        }
        break;
        case 2:
        {
            long_send_to_radio(0);
            MessageBox("��ʾ\n\n����ȫ1�Ѵ�\n����30s�Զ��ر�");
        }
        break;
        case 3:
        {
            long_send_to_radio(1);
            MessageBox("��ʾ\n\n�����ѹر�");
        }
        break;
        case 4:
        {
            long_send_to_radio(2);
            MessageBox("��ʾ\n\n����ȫ0�Ѵ�\n����30s�Զ��ر�");
        }
        break;
        case 5:
        {
            long_send_to_radio(3);
            MessageBox("��ʾ\n\n�����ѹر�");
        }
        break;
        }
    }
    return TRUE;
    }
    return FALSE;
}

PLUGIN_INFO g_layerRadioLongSendCfg =
{
    0x20B70022/*id*/,
    LongCfgInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    LongCfgDraw/*Draw*/,
    NULL/*Timer*/,
    LongCfgKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("�ն˸�λ",1)

void reboot_terminal_Init(DWORD dwPluginId)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    g_ac_data_display_drawpage_index = 0;
    pg_guicomm->bCanTrunDisp = FALSE;
	g_bpop = TRUE;
}
void reboot_terminal_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "�����ն�";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 44;
    if (g_ac_data_display_drawpage_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȡ��");
    }

    nLeft = 164;
    if (g_ac_data_display_drawpage_index == 1)
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

BOOL reboot_terminal_KeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 0:
            CancelMenu();
            return TRUE;
        case 1:
        if(ValidatePwd() == TRUE)
        {
            if (FALSE == MessageBox("��ʾ\n�Ƿ������ն�\n����ȷ��һ��", MB_OKCANCEL))
            {
                return TRUE;
            }
            g_ac_data_display_drawpage_index = 0;
            RebootTerminal();
            PopMsg("        ���������ն�...");
//            sleep_no_interrupt(1000);     //�ȴ�ϵͳ������������������
        }
        break;
        }
        return TRUE;
    }
    return FALSE;
}
//�����ն�
PLUGIN_INFO g_layer_reboot_terminal =
{
    0x20B30000/*id*/,
    reboot_terminal_Init/*InitLayer*/,
    NULL/*UninitLayer*/,
    reboot_terminal_Draw/*Draw*/,
    NULL/*Timer*/,
    reboot_terminal_KeyMsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("����ҳ������",1)
/************************************************************************/
/* ����ҳ������                                                         */
/************************************************************************/
extern void ReadTrunDisp();
typedef struct _TRUN_DISP_CFG
{
	int		nIndex;
	int		nDrawCur;	// ��ǰ��ʾ�ĵ�һ��
}TRUN_DISP_CFG;
TRUN_DISP_CFG g_trun_disp;

void TrunDispInit(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;
	g_trun_disp.nIndex = 0;
	g_trun_disp.nDrawCur = 0;
}
void TrunDispDraw(HDC hdc)
{
    SelectFont(hdc,g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

	ReadTrunDisp();
	if(g_trun_disp.nIndex < 50 && g_trun_disp.nIndex >= g_nTotalTrunDispCnt)
	{
		g_trun_disp.nIndex = 0;
		g_trun_disp.nDrawCur = 0;
	}
	if(g_trun_disp.nIndex < 50 && g_nTotalTrunDispCnt == 0)
	{
		g_trun_disp.nIndex = 50;
	}

	char czBuf[100];
	int nLeft1 = 30;
	int nLeft2 = 90;
	int nTop = 22;

	Rectangle(hdc, 10, nTop-2, 230, nTop+59);
	nTop += 3;

	int nDrawMax = g_trun_disp.nDrawCur+4;
    if(nDrawMax > g_nTotalTrunDispCnt)
    {
        nDrawMax = g_nTotalTrunDispCnt;
    }
	PLUGIN_INFO* pPlugin;
	char* pBuf;
    for(int i = g_trun_disp.nDrawCur; i < nDrawMax; i ++)
    {
		DWORD dwId = g_nTrunDisp[i];
		pBuf = (char*)&dwId;
		pPlugin = GetPlugIn(dwId);
		sprintf(czBuf, "��%2d�� 0x%02X%02X%02X%02X%s", i+1, pBuf[3], pBuf[2], pBuf[1], pBuf[0],
			(pPlugin==NULL)?" ��Ч":"");
        if(g_trun_disp.nIndex == i)
        {
            ITEM_SELECT;
            TextOut(hdc, 40, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, 40, nTop, czBuf);
        }
        nTop+=14;
    }
	
	nTop = 86;
	TextOut(hdc, nLeft1, nTop, "��������ʱ��");
	sprintf(czBuf, "%3d", g_guiMain.nTrunDispTimeMax);
	if(g_trun_disp.nIndex == 50)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2+24, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2+24, nTop, czBuf);
    }
	TextOut(hdc, nLeft2+44, nTop, "��");

	nTop += 14;
	TextOut(hdc, nLeft1, nTop, "���Լ��ʱ��");
	sprintf(czBuf, "%3d", g_guiMain.nInterval);
	if(g_trun_disp.nIndex == 50+1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2+24, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2+24, nTop, czBuf);
    }
	TextOut(hdc, nLeft2+44, nTop, "��");

	nTop += 14;
	if(g_trun_disp.nIndex == 50+2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+24, nTop, "�ָ�ȱʡ����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+24, nTop, "�ָ�ȱʡ����");
    }
	PopListDraw(hdc);
}
void TrunDispReturn(char* pNewStr, void* pPara)
{
	char czBuf[300];
	// ��֤������ȷ��
    if(CheckNumber(pNewStr, 255) == FALSE)
    {
		switch(g_trun_disp.nIndex)
		{
		case 50:
			if(MessageBox("��������ʱ�����벻��ȷ\n\nȷ���Ƿ���������?", MB_OKCANCEL) == FALSE)
			{
				return;
			}
			sprintf(czBuf, "%d", g_guiMain.nTrunDispTimeMax);
			InputBox(czBuf, TrunDispReturn, NULL, 0x01);
			break;
		case 51:
			if(MessageBox("���Լ��ʱ�����벻��ȷ\n\nȷ���Ƿ���������?", MB_OKCANCEL) == FALSE)
			{
				return;
			}
			sprintf(czBuf, "%d", g_guiMain.nInterval);
			InputBox(czBuf, TrunDispReturn, NULL, 0x01);
			break;                
		}
    }
    else
    {
		switch(g_trun_disp.nIndex)
		{
		case 50:
			g_guiMain.nTrunDispTimeMax = atoi(pNewStr);
			break;
		case 51:
			g_guiMain.nInterval = atoi(pNewStr);
			break;                
		}

		czBuf[0] = (unsigned char)g_nTotalTrunDispCnt;
		czBuf[1] = (unsigned char)g_guiMain.nTrunDispTimeMax;
		czBuf[2] = (unsigned char)g_guiMain.nInterval;
		int nRet = write_pdata_xram(GUI_PARA, czBuf, Turn_Num_Time, Turn_Num_Time_Len);
		ASSERT(nRet > 0);
    }
}
const char* g_czTrunDisp1[]={"���ҳ��","����һ��","ɾ��ҳ��",};
const char* g_czTrunDisp2[]={"���ҳ��","����һ��","ɾ��ҳ��",};
const char* g_czTrunDisp3[]={"���ҳ��","����һ��","����һ��","ɾ��ҳ��",};
const char* g_czTrunDisp4[]={"���ҳ��","ɾ��ҳ��",};
extern void WriteTrunDisp();
void UpTrunDisp()
{
	ASSERT(g_trun_disp.nIndex > 0);
	int nId = g_nTrunDisp[g_trun_disp.nIndex];
	g_nTrunDisp[g_trun_disp.nIndex] = g_nTrunDisp[g_trun_disp.nIndex-1];
	g_nTrunDisp[g_trun_disp.nIndex-1] = nId;

	g_trun_disp.nIndex --;
	if(g_trun_disp.nIndex < g_trun_disp.nDrawCur)
	{
		g_trun_disp.nDrawCur = g_trun_disp.nIndex;
	}
	WriteTrunDisp();
	UpdateWindow(g_guiComm.hMainWnd, TRUE);
}
void DownTrunDisp()
{
	ASSERT(g_trun_disp.nIndex < g_nTotalTrunDispCnt-1);
	int nId = g_nTrunDisp[g_trun_disp.nIndex];
	g_nTrunDisp[g_trun_disp.nIndex] = g_nTrunDisp[g_trun_disp.nIndex+1];
	g_nTrunDisp[g_trun_disp.nIndex+1] = nId;

	g_trun_disp.nIndex ++;
	if(g_trun_disp.nIndex > g_trun_disp.nDrawCur+3)
	{
		g_trun_disp.nDrawCur = g_trun_disp.nIndex-3;
	}
	WriteTrunDisp();
	UpdateWindow(g_guiComm.hMainWnd, TRUE);
}
void DeleteTrunDisp()
{
	for(int i = g_trun_disp.nIndex; i < g_nTotalTrunDispCnt-1; i ++)
	{
		g_nTrunDisp[i] = g_nTrunDisp[i+1];
	}
	g_nTotalTrunDispCnt --;
	if(g_trun_disp.nIndex >= g_nTotalTrunDispCnt)
	{
		g_trun_disp.nIndex --;
	}
	if(g_trun_disp.nDrawCur > 0)
	{
		g_trun_disp.nDrawCur --;
	}
	WriteTrunDisp();
	UpdateWindow(g_guiComm.hMainWnd, TRUE);
}


extern BOOL ReadConfig(BOOL bAll);
extern BOOL RestoreConfigFromXml();
extern int g_nCurIndex;
extern int g_nCurMenuCnt;
extern int g_nCurSelect;
extern int g_nMaxMenuChar;
extern BOOL g_bAllMenu;
BOOL TrunDispKeyMsg(int nKey)
{
	// ����List��������
    if(g_guiComm.bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if(nKey == KEY_OK)
        {
			if(nSel == 0)
			{// ���ҳ��
				PLUGIN_INFO* pPlugin = GetPlugIn(g_nTrunDisp[g_trun_disp.nIndex]);
				if(pPlugin != NULL)
				{
					PopLayer(pPlugin->dwId);
				}
				UpdateWindow(g_guiComm.hMainWnd, TRUE);
			}
			else
			{
				if(g_nTotalTrunDispCnt == 1)
				{
					switch(nSel)
					{
					case 1:DeleteTrunDisp();break;
					}
				}
				else if(g_trun_disp.nIndex == 0)
				{
					switch(nSel)
					{
					case 1:DownTrunDisp();break;
					case 2:DeleteTrunDisp();break;
					}
				}
				else if(g_trun_disp.nIndex == g_nTotalTrunDispCnt-1)
				{
					switch(nSel)
					{
					case 1:UpTrunDisp();break;
					case 2:DeleteTrunDisp();break;
					}
				}
				else
				{
					switch(nSel)
					{
					case 1:UpTrunDisp();break;
					case 2:DownTrunDisp();break;
					case 3:DeleteTrunDisp();break;
					}
				}
			}
        }
        return TRUE;
    }
    switch(nKey)
    {
	case KEY_LEFT:
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_UP:
		g_trun_disp.nIndex --;
		if(g_trun_disp.nIndex < 50 && g_trun_disp.nIndex >= g_nTotalTrunDispCnt)
		{
			if(g_nTotalTrunDispCnt == 0)
			{
				g_trun_disp.nIndex = 52;
			}
			else
			{
				g_trun_disp.nIndex = g_nTotalTrunDispCnt-1;
				g_trun_disp.nDrawCur = g_trun_disp.nIndex-3;
				if(g_trun_disp.nDrawCur < 0)g_trun_disp.nDrawCur = 0;
			}
		}
		else if(g_trun_disp.nIndex < 0)
		{
			g_trun_disp.nIndex = 52;
		}
		else
		{
			if(g_trun_disp.nIndex < g_trun_disp.nDrawCur)
			{
				g_trun_disp.nDrawCur = g_trun_disp.nIndex;
			}
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;

    case KEY_DOWN:
        g_trun_disp.nIndex ++;
		if(g_trun_disp.nIndex < 50 && g_trun_disp.nIndex >= g_nTotalTrunDispCnt)
		{
			g_trun_disp.nIndex = 50;
		}
		else if(g_trun_disp.nIndex > 52)
		{
			if(g_nTotalTrunDispCnt == 0)
			{
				g_trun_disp.nIndex = 50;
			}
			else
			{
				g_trun_disp.nIndex = 0;
				g_trun_disp.nDrawCur = 0;
			}
		}
		else if(g_trun_disp.nIndex < 50)
		{
			if(g_trun_disp.nIndex > g_trun_disp.nDrawCur+3)
			{
				g_trun_disp.nDrawCur = g_trun_disp.nIndex-3;
			}
		}
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return TRUE;
        break;
	case KEY_OK:
		{// ȷ�ϰ���
			switch(g_trun_disp.nIndex)
			{
			case 50:// �����޸��ܼ�״̬
				if(ValidatePwd() == TRUE)
				{
					char czBuf[20];
					sprintf(czBuf, "%d", g_guiMain.nTrunDispTimeMax);
					InputBox(czBuf, TrunDispReturn, NULL, 0x01);
				}
				break;
			case 51:
				if(ValidatePwd() == TRUE)
				{
					char czBuf[20];
					sprintf(czBuf, "%d", g_guiMain.nInterval);
					InputBox(czBuf, TrunDispReturn, NULL, 0x01);
				}
				break;
			case 52:
				if(ValidatePwd() == TRUE)
				{
					if(TRUE != MessageBox("�Ƿ�ȷ�ϻָ�\nҳ��ȱʡ����\n�ò�������˵������棡", MB_OKCANCEL))
					{
						break;
					}
					RestoreConfigFromXml(NULL);
					ReadConfig(g_bAllMenu);

					ASSERT(g_guiMain.pluginCur != NULL);
					if(g_guiMain.pluginCur->pfnUninitLayer != NULL)
					{
						g_guiMain.pluginCur->pfnUninitLayer();
					}
					// �������˵����ID��ȷ����ǰ��plug
					g_guiMain.pluginCur = GetPlugIn(g_guiMain.nMenuRootId);
					ASSERT(g_guiMain.pluginCur != NULL);

					g_nParentIndex = -1;
					g_nCurIndex = -1;
					g_nCurMenuCnt = 0;
					g_nCurSelect = 0;
					g_nMaxMenuChar = 0;
					
					// ��ʼ����ǰҳ��⣬��Uninitialize���ͷ�
					ASSERT(g_guiMain.pluginCur != NULL);
					g_guiComm.bCanTrunDisp = TRUE;
					if(g_guiMain.pluginCur->pfnInitLayer != NULL)
					{
						g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
					}
					UpdateWindow(g_guiComm.hMainWnd, TRUE);
				}
				break;
			default:
				if(ValidatePwd() == TRUE)
				{
					int nLeft = 90;
					int nTop = (g_trun_disp.nIndex-g_trun_disp.nDrawCur)*14 + 40;

					if(g_nTotalTrunDispCnt == 1)
					{
						PopList(g_czTrunDisp4, 2, g_czTrunDisp1[0], nLeft, nTop, FALSE);
					}
					else if(g_trun_disp.nIndex == 0)
					{
						PopList(g_czTrunDisp1, 3, g_czTrunDisp1[0], nLeft, nTop, FALSE);
					}
					else if(g_trun_disp.nIndex == g_nTotalTrunDispCnt-1)
					{
						PopList(g_czTrunDisp2, 3, g_czTrunDisp1[0], nLeft, nTop, FALSE);
					}
					else
					{
						PopList(g_czTrunDisp3, 4, g_czTrunDisp1[0], nLeft, nTop, FALSE);
					}
				}
				break;
			}
        }
		return TRUE;
    }
    return FALSE;
}
PLUGIN_INFO g_layerTrunDisp = {
	0x20690000/*id*/, 
	TrunDispInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	TrunDispDraw/*Draw*/,
	NULL/*Timer*/,
	TrunDispKeyMsg/*keymsg*/,
	NULL/*ipc*/
};
/************************************************************************/
/* ����ҳ�����ý���                                                     */
/************************************************************************/
#endif

#if DESC("���ݳ�ʼ��",1)

void data_initialization_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    g_guiComm.bCanTrunDisp = FALSE;
}
void data_initialization_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "���ݳ�ʼ��";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 44;
    if (g_ac_data_display_drawpage_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȡ��");
    }

    nLeft = 164;
    if (g_ac_data_display_drawpage_index == 1)
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

int data_initialization()
{
    PopMsg("�������ݳ�ʼ��...");

    CLEAR_DATA_T ClearData;
    ClearData.recordOAD.logicId = 0;
    ClearData.bClassTag = FALSE;
    int ret = 0;
    ret = db_clear_data(g_hUdbCenter, &ClearData);
    if (ret == ERR_OK)
    {
        GUI_FMT_TRACE("db_clear_data ok. ret(%d)\n", ret);
    }
    else
    {
        GUI_FMT_TRACE("db_clear_data failed. ret(%d)\n", ret);
    }
//  ret |= data_init_sent_to_acmeter();
//  if (ret != ERR_OK)
    {
//      GUI_FMT_TRACE("param_init_sent_to_acmeter err. ret(%d)\n", ret);
    }

    PopMsg(NULL);
    return ret;

}

BOOL data_initialization_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 0:
            CancelMenu();
            return TRUE;
        case 1:
            if (data_initialization() != 0)
            {
                MessageBox("���ݳ�ʼ��ʧ��!");
            }
            else
            {
                MessageBox("���ݳ�ʼ���ɹ�!");
            }
            return TRUE;

        }
        return TRUE;
    }
    return FALSE;
}

//���ݳ�ʼ��
PLUGIN_INFO g_layer_data_initialization =
{
    0x20B40002/*id*/,
    data_initialization_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    data_initialization_draw/*Draw*/,
    NULL/*Timer*/,
    data_initialization_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("������ʼ��",1)

void param_initialization_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    g_guiComm.bCanTrunDisp = FALSE;
	g_bpop = TRUE;
}
void param_initialization_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "������ʼ��";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 44;
    if (g_ac_data_display_drawpage_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȡ��");
    }

    nLeft = 164;
    if (g_ac_data_display_drawpage_index == 1)
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
    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 0:
            CancelMenu();
            break;
        case 1:
            if (db_recover_default(g_hUdbCenter, 0) == 0)
            {
#ifdef AREA_FUJIAN
            /*���ز�ģ�鷢�ͳ�ʼ��������ȴ�����*/
                uint8 head = 17,tail = 20,len=0;
                uint8 LocalAddr[6] = { 0 };
                uint8 apduhead[17] = {0x09,0x32,0x1e,0xf2,0x09,0x02,0x01,0x00,0x06,0x02,0x08,0x01,0x00,0x00,0x32,0x00,0x0a};
                get_addr(LocalAddr);
                uint8 apdu[tail+head+2+6]={0};
                uint8 index = 0;
                memcpy(apdu,apduhead,head);
                index +=head;
                apdu[index++] = 0x07;
                apdu[index++] = 0x05;
                memcpy(&apdu[index],LocalAddr,6);
                index+=6;
                uint8 apdutail[tail] = {0x01,0x00,0x00,0x0f,0x68,0x0f,0x00,0x43,0x00,0x00,0x00,0x80,0x25,0x33,0x01,0x02,0x00,0x1e,0x16,0x00};
                
                memcpy(&apdu[index],apdutail,tail);
                index+=tail;
                len = index;
                gui_mannual_amr_send(apdu, len);
#endif
#if defined (AREA_JIANGSU)&&(PRODUCT_ZCU_1)
                PopMsg("          ������ʼ���ɹ���\n          ��������Ӧ��...");
#else
                PopMsg("    ������ʼ���ɹ���\n    ��������Ӧ��...");
#endif
                //��������Ĭ�ϲ����Ľӿ� �������
//                app_setdefault_cfg_area(g_hUdbCenter);
//                RebootTerminal();
				

            }
            else
            {
                MessageBox("������ʼ��ʧ��!");
                CancelMenu();
            }
            break;
        }
        return TRUE;
    }
    return FALSE;
}

//������ʼ��
PLUGIN_INFO g_layer_param_initialization =
{
    0x20B40000/*id*/,
    param_initialization_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    param_initialization_draw/*Draw*/,
    NULL/*Timer*/,
    param_initialization_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("���ɲ�����ʼ��",1)

void ac_param_initialization_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    g_guiComm.bCanTrunDisp = FALSE;
}
void ac_param_initialization_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "���ɲ�����ʼ��";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 44;
    if (g_ac_data_display_drawpage_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȡ��");
    }

    nLeft = 164;
    if (g_ac_data_display_drawpage_index == 1)
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

BOOL ac_param_initialization_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 0:
            CancelMenu();
            return TRUE;
        case 1:
            if (param_init_sent_to_acmeter() == 0)
            {
                MessageBox("���ɲ�����ʼ���ɹ�!");
                CancelMenu();
            }
            else
            {
                MessageBox("���ɲ�����ʼ��ʧ��!");
            }
            return TRUE;

        }
        return TRUE;
    }
    return FALSE;
}

//���ɲ�����ʼ��
PLUGIN_INFO g_layer_ac_param_initialization =
{
    0x20B43000/*id*/,
    ac_param_initialization_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ac_param_initialization_draw/*Draw*/,
    NULL/*Timer*/,
    ac_param_initialization_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("�������ݳ�ʼ��",1)

void data_param_initialization_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "�������ݳ�ʼ��";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 44;
    if (g_ac_data_display_drawpage_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȡ��");
    }

    nLeft = 164;
    if (g_ac_data_display_drawpage_index == 1)
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

BOOL data_param_initialization_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 0:
            CancelMenu();
            return TRUE;
        case 1:
            if (data_init_sent_to_acmeter() == 0)
            {
                MessageBox("�������ݳ�ʼ���ɹ�!");
                CancelMenu();
            }
            else
            {
                MessageBox("�������ݳ�ʼ��ʧ��!");
            }
            return TRUE;

        }
        return TRUE;
    }
    return FALSE;
}

//�������ݳ�ʼ��
PLUGIN_INFO g_layer_data_param_initialization =
{
    0x20B43001/*id*/,
    ac_param_initialization_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    data_param_initialization_draw/*Draw*/,
    NULL/*Timer*/,
    data_param_initialization_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("U������������־",1)

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

    if (0 != access("/udisk/package", F_OK))
    {
        g_UpdateStatus = UPDATE_STATUS_NO_PACKAGE;
        return 1;
    }

    system("cp -r /udisk/package/* /tmp/update/");
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
    int nLeft = 84;
	int nLeft1 = 90;
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
        TextOut(hdc, nLeft, nTop, "�����ļ���...");
        nTop = 60;
        TextOut(hdc, nLeft1, nTop, "��ʱԼ30s");
        nTop = 80;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        TextOut(hdc, nLeft1, nTop, "��Ҫ�ε�U��");

        return;
    }

//Ҫ����240*128��Ļ����ʾƫ�������
    if (g_UpdateStatus == UPDATE_STATUS_INSTALL)
    {
        nTop = 40;
        TextOut(hdc, nLeft1, nTop, "���ڰ�װ...");
        nTop = 60;
        TextOut(hdc, nLeft1, nTop, "��ʱԼ1����");
        nTop = 80;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        TextOut(hdc, nLeft1, nTop, "���԰ε�U��");

        return;
    }

    nTop = 36;
    SelectFont(hdc, pg_guicomm->fontBig);

    if (0 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "��װ����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "��װ����");
    }

    nTop += 30;
    if (1 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ִ�нű�");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ִ�нű�");
    }

    nTop += 30;
    if (2 == g_UpdateIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "����");
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
//    char *productName[] = {(char *)"��Чֵ", (char *)"������", (char *)"ר��" , (char *)""};
//    char buf[200] = {0};

    if (ValidatePwd() == TRUE)
    {
//        sprintf(buf, "%s%s", message, productName[installType]);
        if (FALSE == MessageBox(message, MB_OKCANCEL))
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
	int nleft = 100;
	int ntop = 36;
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
	ntop+=30;
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
	ntop+=30;
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

//    index++;
//	ntop+=28;
//	sprintf(czbuf,"�������ݿ��ļ�");
//	if(index == logexportindex)
//	{
//        ITEM_SELECT;
//        TextOut(hdc, nleft, ntop, czbuf);
//        ITEM_NOT_SELECT;
//	}else
//	{
//        TextOut(hdc, nleft, ntop, czbuf);
//	}

    switch(logflag)
    {
        case 2:
        {
            pthread_cancel(copythreadid);
            pthread_join(copythreadid, NULL);

            system("ps -ef|grep /tmp/udisk |grep -v grep|awk '{print $2}' |xargs kill");
            system("sync");
            logflag = -1;

            MessageBox("����ʱ�䳬��15����\n�Զ�������");

        }
        break;
        case 1:
        {
            pthread_join(copythreadid, NULL);
            GUI_FMT_TRACE("�������\n");

            logflag = -1;

            
			MessageBox("������ɣ�");
        }
        break;
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

    if(logflag == 0)
    {
        time(&t);
        if(t - logstarttime>90000)
        {
            GUI_FMT_DEBUG("copy timeout!!\n");
            logflag = 2;
            ClosePopLayer();
        }
    }
    else if(logflag == 1)
    {
        ClosePopLayer();
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
    char cmd[512]		= {0};
	char logPath[512]	= {0};
	char AppPath[512]	= {0};

    //�����ļ���������ip+ʱ��
    char pstatus[128] = "127.0.0.1";
    time_t t;
    time(&t);
    logstarttime = t;
    tm *p = localtime(&t);
    tm ptime;
    memcpy(&ptime,p,sizeof(tm));
    sprintf(cmd, "ifconfig FE0 | grep 'inet addr' | awk '{ print $2}' | awk -F: '{print $2}'");
    FILE* pf = popen(cmd, "r");  
    if(pf != NULL)
    {
        fgets(pstatus, 128, pf);
    }
    pclose(pf);
    if(pstatus[strlen(pstatus)-1] == '\n')
        pstatus[strlen(pstatus)-1] = '\0';

    switch(logexportindex)
    {
	    case 0:         //������־�ļ�
	    {
	    	sprintf(logPath,"/tmp/udisk/log_%s_%02d.%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min, ptime.tm_sec);
			
	        sprintf(cmd, "mkdir -p %s", logPath);
	        system(cmd);

			sprintf(cmd, "tar -zcvf %s/log.tar.gz /data/app/", logPath);
            system(cmd);

			sprintf(cmd, "tar -zcvf %s/tmplog.tar.gz /tmp/data/", logPath);
            system(cmd);

            sprintf(cmd, "tar -zcvf %s/runlog.tar.gz /run/log/", logPath);
            system(cmd);

            sprintf(cmd, "tar -zcvf %s/logapplog.tar.gz /log/app/", logPath);
            system(cmd);

			sprintf(cmd, "tar -zcvf %s/varlog.tar.gz /var/log/*.log ", logPath);
            system(cmd);

	        sprintf(cmd, "tar -zcvf %s/messages.tar.gz /var/log/messages*", logPath);
            system(cmd);
            break;
	    }
	    case 1:         //����Ӧ�ó���
	    {
	        sprintf(AppPath,"/udisk/Apps_%s_%02d.%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min, ptime.tm_sec);
	        
	        sprintf(cmd, "mkdir -p %s", AppPath);
	        system(cmd);
	        sprintf(cmd, "cp -rf /usr/local/extapps/* %s/", AppPath);
	        system(cmd);
	        break;
	    }
	    case 2:         //���������ļ�
	    {
	        sprintf(logPath,"/udisk/log_%s_%02d.%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min, ptime.tm_sec);
			
	        sprintf(cmd, "mkdir -p %s", logPath);
	        system(cmd);
	        
	        sprintf(cmd, "tar -zcvf %s/log.tar.gz /data/app/", logPath);
            system(cmd);

	        sprintf(cmd, "tar -zcvf %s/messages.tar.gz /var/log/messages*", logPath);
            system(cmd);

			sprintf(cmd, "tar -zcvf %s/tmplog.tar.gz /tmp/data/", logPath);
            system(cmd);

			sprintf(cmd, "tar -zcvf %s/varlog.tar.gz /var/log/*.log ", logPath);
            system(cmd);
//
//            sprintf(cmd, "tar -zcvf %s/data.tar.gz /data/app/", logPath);
//            system(cmd);
            
	        sprintf(AppPath,"/udisk/Apps_%s_%02d.%02d.%02d", pstatus, ptime.tm_hour, ptime.tm_min, ptime.tm_sec);
	        
	        sprintf(cmd, "mkdir -p %s", AppPath);
	        system(cmd);
	        sprintf(cmd, "cp -rf /usr/local/extapps/* %s/", AppPath);
	        system(cmd);
	        break;
	    }
    }
    
//    system(cmd);
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
	int maxindex = 2;
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
                PopMsg("                 ���ڱ���\n\n               ����ʱ��ϳ�\n\n         ��������ն˻��߰γ�U��");
                return FALSE;
            }
        }
    }
	}
	return FALSE;
}


//��־����
PLUGIN_INFO g_smiOSLogExport = {
	0x20680002/*id*/, 
	LogExportInit/*InitLayer*/,
	LogExportUnInit/*UninitLayer*/,
	LogExportDraw/*Draw*/,
	LogExportTimer/*Timer*/,
	LogExportKeyMsg/*keymsg*/,
	NULL/*ipc*/
};

//U������
PLUGIN_INFO g_smiOSUUpdate = {
	0x20B60000/*id*/, 
	InitUdiskUpdateDraw/*InitLayer*/,
	NULL/*UninitLayer*/,
	UdiskUpdateDraw/*Draw*/,
	UdiskUpdateTimer/*Timer*/,
	UdiskUpdateKey/*keymsg*/,
	NULL/*ipc*/
};

#endif

#if DESC("��־���",1)
uint8 logClearindex;

void LogClear(void)
{
    char cmd[100] = {0};

    //sprintf(cmd, "find /data/app/ -name log -type d |xargs rm -rfv");
    sprintf(cmd,"find /data/app -path '/data/app/*/log/*' |xargs sudo rm -rf");
    system(cmd);
    sprintf(cmd,"find /tmp/data/app -path '/tmp/data/app/*/log/*' |xargs sudo rm -rf");
    system(cmd);
    sprintf(cmd,"find /log/app -path '/log/app/*/*' |xargs sudo rm -rf");
    system(cmd);
    sprintf(cmd,"find /run/log -path '/run/log/*/*' |xargs sudo rm -rf");
    system(cmd);
	//��������ϵͳ������sudo��������
	sprintf(cmd,"find /data/app -path '/data/app/*/log/*' |xargs rm -rf");
    system(cmd);
	sprintf(cmd,"find /tmp/data/app -path '/tmp/data/app/*/log/*' |xargs rm -rf");
    system(cmd);
    sprintf(cmd,"find /log/app -path '/log/app/*/*' |xargs rm -rf");
    system(cmd);
    sprintf(cmd,"find /run/log -path '/run/log/*/*' |xargs rm -rf");
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

    int nTop = 98;
    int nLeft = 44;
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

    nLeft = 164;
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
    0x20B50000,     /* id */
    InitLogClear,   /* InitLayer */
    NULL,           /* UninitLayer */
    LogClearDraw,   /* Draw */
    NULL,           /* Timer */
    LogClearKey,    /* keymsg */
    NULL            /* ipc */
};

#endif

#if DESC("��ȫģʽ",1)

void setSafeONOFFInit(DWORD dwPluginId)
{
	g_guiComm.bCanTrunDisp = FALSE;

    dictionary *ini = NULL;
    ini = iniparser_load("/data/app/desktopGui/safemode.ini");
    GUI_FMT_DEBUG("��ȡ�����ļ�/data/app/desktopGui/safemode.ini \n");
    if(ini!=NULL)
    {
//        safe_status= iniparser_getint(ini,"SAFE_MODE:SAFE_BUTTON",0);
        mac_status= iniparser_getint(ini,"SAFE_MODE:MAC_MODE",1);
        hplc_mac_status= iniparser_getint(ini,"SAFE_MODE:HPLC_MAC_MODE",0);
        g_Relay485Flag= iniparser_getint(ini,"SAFE_MODE:AC485FLAG",0);
        router_state= iniparser_getint(ini,"SAFE_MODE:ROUTER_STATE_STUDY",1);
        #ifdef AREA_JIANGSU
        g_MeterreadFlag= iniparser_getint(ini,"SAFE_MODE:RS485PATCH",1);
        #else
        g_MeterreadFlag= iniparser_getint(ini,"SAFE_MODE:RS485PATCH",0);
        #endif
        iniparser_freedict(ini);
    }

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

void setSafeONOFFDraw1(HDC hdc)
{
    char      czbuf[32];
	SelectFont(hdc,g_guiComm.fontSmall);
	int nTop = 20;
    int nLeft = 50;
    int nLeft2 = 140;

    //������ʾ
    ITEM_NOT_SELECT;
    sprintf(czbuf, "��ȫģʽ:");//�òɼ��ܿ��أ������ܿ���
    TextOut(hdc, nLeft, nTop, czbuf);
    
    if(safe_index == 0)
     {
         ITEM_SELECT;
         TextOut(hdc, nLeft2, nTop, g_safeModel_list[safe_status]);
         ITEM_NOT_SELECT;
     }
     else
     {
         TextOut(hdc, nLeft2, nTop, g_safeModel_list[safe_status]);
     }

    nTop += 18;
    sprintf(czbuf, "MACУ��:");
    TextOut(hdc, nLeft, nTop, czbuf);

    if(safe_index == 1)
    {
     	ITEM_SELECT;
     	TextOut(hdc, nLeft2, nTop, g_macModel_list[mac_status]);
     	ITEM_NOT_SELECT;
    }
    else
    {
     	TextOut(hdc, nLeft2, nTop, g_macModel_list[mac_status]);
    }

    nTop += 18;
    sprintf(czbuf, "�ز�MACУ��:");
    TextOut(hdc, nLeft, nTop, czbuf);

    if(safe_index == 2)
    {
    	 ITEM_SELECT;
    	 TextOut(hdc, nLeft2, nTop, g_macModel_list[hplc_mac_status]);
    	 ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_macModel_list[hplc_mac_status]);
    }

    nTop += 18;
    sprintf(czbuf, "����485ͨ��:");
    TextOut(hdc, nLeft, nTop, czbuf);

    if(safe_index == 3)
    {
    	 ITEM_SELECT;
    	 TextOut(hdc, nLeft2, nTop, g_ac_485_list[g_Relay485Flag]);
    	 ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_ac_485_list[g_Relay485Flag]);
    }

    nTop += 18;
    sprintf(czbuf, "·��ѧϰ:");
    TextOut(hdc, nLeft, nTop, czbuf);

    if(safe_index == 4)
    {
     	ITEM_SELECT;
     	TextOut(hdc, nLeft2, nTop, g_routerstudy_list[router_state]);
     	ITEM_NOT_SELECT;
    }
    else
    {
     	TextOut(hdc, nLeft2, nTop, g_routerstudy_list[router_state]);
    }

    if(safe_index == 5)
    {
        ITEM_SELECT;
        TextOut(hdc, 20, 108, "��һҳ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 20, 108, "��һҳ");
    }
    
    if(safe_index == 6)
    {
        ITEM_SELECT;
        TextOut(hdc, 180, 108, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 180, 108, "ȷ��");
    }

     PopListDraw(hdc);
}

void setSafeONOFFDraw2(HDC hdc)
{
    char      czbuf[32];
    SelectFont(hdc,g_guiComm.fontSmall);
    int nTop = 20;
    int nLeft = 50;
    int nLeft2 = 140;

    sprintf(czbuf, "������:");
    TextOut(hdc, nLeft, nTop, czbuf);
    if(safe_index == 7)
    {
    	ITEM_SELECT;
    	TextOut(hdc, nLeft2, nTop, g_meterread_list[g_MeterreadFlag]);
    	ITEM_NOT_SELECT;
    }
    else
    {
    	TextOut(hdc, nLeft2, nTop, g_meterread_list[g_MeterreadFlag]);
    }

    if(safe_index == 8)
    {
        ITEM_SELECT;
        TextOut(hdc, 20, 108, "��һҳ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 20, 108, "��һҳ");
    }

    if(safe_index == 9)
    {
        ITEM_SELECT;
        TextOut(hdc, 180, 108, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 180, 108, "ȷ��");
    }
    
    PopListDraw(hdc);
}

void setSafeONOFFDraw(HDC hdc)
{
    if(safe_index < 7)
    {
        return setSafeONOFFDraw1(hdc);
    }
    else
    {
        return setSafeONOFFDraw2(hdc);
    }
}

 BOOL setSafeONOFFMsg(int nKey)
 {

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
             else if(safe_index == 7)
                g_MeterreadFlag = nSel;

             UpdateWindow(g_guiComm.hMainWnd, TRUE);
         }
         return TRUE;
     }

     switch(nKey)
     {
     case KEY_UP:
         safe_index--;
         if(safe_index < 0)
         {
             safe_index = 9;
         }

         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     case KEY_DOWN:
         safe_index ++;
         if(safe_index > 9)
         {
             safe_index = 0;
         }

         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     case KEY_OK:
         if(ValidatePwd() == TRUE)
         {// ȷ�ϴ���
         
            if (safe_index == 5)
            {
                safe_index = 7;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            }
            else if(safe_index == 8)
            {
                safe_index = 0;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            }
            
            switch(safe_index)
            {
            	case 0:
					PopList(g_safeModel_list, 2, g_safeModel_list[safe_status], 140, 32, FALSE);
					break;
             	case 1:
					PopList(g_macModel_list, 2, g_macModel_list[mac_status], 140, 50, FALSE);
					break;
				case 2:
					PopList(g_macModel_list, 2, g_macModel_list[hplc_mac_status], 140, 68, FALSE);
					break;
                case 3:
					PopList(g_ac_485_list, 2, g_ac_485_list[g_Relay485Flag], 140, 85, FALSE);
					break;
                case 4:
					PopList(g_routerstudy_list, 2, g_routerstudy_list[router_state], 140, 100, FALSE);
					break;
                case 7:
					PopList(g_meterread_list, 2, g_meterread_list[g_MeterreadFlag], 140, 32, FALSE);
					break;
             	case 6:
                case 9:
                    write_safe_cfg("/data/app/desktopGui/safemode.ini");
                    ret = write_safemode();
//                    write_cfg_by_key("/data/app/desktopGui/safemode.ini", "SAFE_BUTTON", safe_status);
//                    write_cfg_by_key("/data/app/taskManager/taskManager.ini", "SAFE_BUTTON", mac_status);
//                    write_cfg_by_key("/usr/local/extapps/acMeter/bin/acMeter.ini", "AC485FLAG", g_Relay485Flag);

					UpdateWindow(g_guiComm.hMainWnd, TRUE);
                    g_bpop = TRUE;
                    if(ret == 0)
                    {
                        PopMsg("              ���óɹ�!\n            ��������Ӧ��...");
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

#endif

//�ú��������ʵ�����ֵ��ʾ,��ֹ����ֵԾ��
void valueDisplayProcess_strUnit(int64 energy, char* str, size_t n,uint32 weight,const char* unit)
{
    double val_kWh = energy;
    //const char *units[8]={"w","wh","var"};

    if (str == NULL || n <= 0)
    {
        return;
    }

    for(uint32 i =1;i<=weight;i++)
    {
        val_kWh = val_kWh *0.1;
    }

    //val_kWh = energy * 0.0001;//�����ֵ���Լ���ڲ�������10000���洢������

    //��λ����Ӧ�������ֵ��������ʾȫ��������
    //��λ��
    int64 val_autoAdap=(int64)val_kWh;
    uint8 digitsCnt=0;
    while(val_autoAdap)
    {
        val_autoAdap = val_autoAdap/10;
        digitsCnt++;
    }
    if(digitsCnt<=6)
    {
        //val_kWh = energy * 0.0001;//�����ֵ���Լ���ڲ�������10000���洢������
        snprintf(str, n - 1, "%6.4fk%s", val_kWh,unit);
        str[n - 1] = '\0';
    }
    else if(digitsCnt>6 && digitsCnt<=9)
    {
        val_kWh = val_kWh*0.001;//�����ֵ���Լ���ڲ�������10000���洢������
        snprintf(str, n - 1, "%6.4fM%s", val_kWh,unit);
        str[n - 1] = '\0';
    }
    else
    {
        val_kWh = val_kWh*0.001*0.001;//�����ֵ���Լ���ڲ�������10000���洢������
        snprintf(str, n - 1, "%6.4fG%s", val_kWh,unit);
        str[n - 1] = '\0';
    }
}



#if DESC("��ǰ����", 1)

void CurPowerSep1(HDC hdc);
void CurPowerSep2(HDC hdc);


void CurPowerDrawTotal(HDC hdc)
{	
	char czBuf[70] = { 0 };
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontBig);
	
	TextOut(hdc,10,20,"��ǰ���й�����");
	TextOut(hdc,10,70,"��ǰ���޹�����");

	//�����8·�����й������޹�����

	sprintf(czBuf, "%6.4f %s", (double)g_group_data.gruop_active_power / 10000, "kW");
	TextOut(hdc,40,40,czBuf);

    // ���޹�����
	sprintf(czBuf, "%6.4f %s", (double)g_group_data.gruop_reactive_power / 10000, "kvar");
    TextOut(hdc, 40, 90, czBuf);
	
}



void CurPowerInit(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(g_ctrl_data, 0, sizeof(g_ctrl_data));
    memset(&g_group_data, 0, sizeof(g_group_data));

	//��ȡ�����й����޹�����
	//ע��,�������ȡ���ǵ�һ·��������й������޹�����
	//���й������޹�,�����Ȱ�8·ȫ��������,Ȼ�����
    read_db_noamal_data(g_hUdbCenter, 0x23010300, 0, 0, (uint8*)&g_group_data.gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23010400, 0, 0, (uint8*)&g_group_data.gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x24010500, 0, 0, (uint8*)&g_ctrl_data[0].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24010600, 0, 0, (uint8*)&g_ctrl_data[0].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24020500, 0, 0, (uint8*)&g_ctrl_data[1].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24020600, 0, 0, (uint8*)&g_ctrl_data[1].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24030500, 0, 0, (uint8*)&g_ctrl_data[2].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24030600, 0, 0, (uint8*)&g_ctrl_data[2].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24040500, 0, 0, (uint8*)&g_ctrl_data[3].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24040600, 0, 0, (uint8*)&g_ctrl_data[3].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24050500, 0, 0, (uint8*)&g_ctrl_data[4].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24050600, 0, 0, (uint8*)&g_ctrl_data[4].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24060500, 0, 0, (uint8*)&g_ctrl_data[5].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24060600, 0, 0, (uint8*)&g_ctrl_data[5].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24070500, 0, 0, (uint8*)&g_ctrl_data[6].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24070600, 0, 0, (uint8*)&g_ctrl_data[6].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24080500, 0, 0, (uint8*)&g_ctrl_data[7].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_hUdbCenter, 0x24080600, 0, 0, (uint8*)&g_ctrl_data[7].plus_reactive_power, sizeof(int32));  
}


void CurPowerDraw(HDC hdc)
{

	switch(g_ac_data_display_drawpage_index)
		{
			case 0:   //���й��͹��޹�����
				CurPowerDrawTotal(hdc);
				break;
			case 1:   //ǰ��·����
				CurPowerSep1(hdc);
				break;
			case 2:
				CurPowerSep2(hdc);
				break;
		} 
}



//ǰ��·������ʾҳ��
void CurPowerSep1(HDC hdc)
{
	char czBuf[70] = { 0 };
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
	
	TextOut(hdc,15,20,"��1·����");
	TextOut(hdc,15,50,"��2·����");
	TextOut(hdc,15,80,"��3·����");
	TextOut(hdc,15,110,"��4·����");

	for(int i = 0;i<4;i++)
	{
		sprintf(czBuf, "%6.4f %s", (double)g_ctrl_data[i].plus_active_power / 10000, "kW");
		TextOut(hdc, 90, 20+i*30, czBuf);
		bzero(czBuf,sizeof(czBuf));	
	}
	
}

//����·������ʾҳ��
void CurPowerSep2(HDC hdc)
{
	char czBuf[70] = { 0 };
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
	
	TextOut(hdc,15,20,"��5·����");
	TextOut(hdc,15,50,"��6·����");
	TextOut(hdc,15,80,"��7·����");
	TextOut(hdc,15,110,"��8·����");

	for(int i = 0;i<4;i++)
	{
		sprintf(czBuf, "%6.4f %s", (double)g_ctrl_data[i+4].plus_active_power / 10000, "kW");
		TextOut(hdc, 90, 20+i*30, czBuf);
		bzero(czBuf,sizeof(czBuf));	
	}
	
}


//������Ϣ
BOOL CurPowerKeyMsg(int nKey)
{
	//3��ҳ��ѭ����ʾ
	switch (nKey)
		{
			case KEY_DOWN:
				g_ac_data_display_drawpage_index>=2? g_ac_data_display_drawpage_index=0: ++g_ac_data_display_drawpage_index;
				break;

			case KEY_UP:
				g_ac_data_display_drawpage_index<=0?g_ac_data_display_drawpage_index=2 : --g_ac_data_display_drawpage_index;
				break;

			case KEY_CANCEL:
				CancelMenu();
				break;
		}
	return TRUE;
}


//��ǰ���й�/�޹�����ҳ��
PLUGIN_INFO g_layer_cur_powerTotal = {
    0x20080001/*id*/,
    CurPowerInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    CurPowerDraw/*Draw*/,
    NULL/*Timer*/,
    CurPowerKeyMsg/*keymsg*/,
    NULL/*ipc*/
};


void G_CurPowerSep1(HDC hdc);
void G_CurPowerSep2(HDC hdc);


void G_CurPowerDrawTotal(HDC hdc)
{	
	char czBuf[70] = { 0 };
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontBig);
	
	TextOut(hdc,10,20,"��ǰ���й�����");
	TextOut(hdc,10,70,"��ǰ���޹�����");

	//�����8·�����й������޹�����
	int64 totalActivePow=0;
	int64 totalReactivePow=0;
	
	for(int i=0;i<8;i++)
	{
		totalActivePow+=g_ctrl_data[i].gruop_active_power;
		totalReactivePow+=g_ctrl_data[i].gruop_reactive_power;
	}
	
	sprintf(czBuf, "%6.4f %s", (double)totalActivePow / 10000, "kW");
	TextOut(hdc,40,40,czBuf);


    // ���޹�����
	sprintf(czBuf, "%6.4f %s", (double)totalReactivePow / 10000, "kvar");
    TextOut(hdc, 40, 90, czBuf);
	
}



void G_CurPowerInit(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(g_ctrl_data, 0, sizeof(g_ctrl_data));

	//��ȡ�����й����޹�����
	//ע��,�������ȡ���ǵ�һ·��������й������޹�����
	//���й������޹�,�����Ȱ�8·ȫ��������,Ȼ�����
    read_db_noamal_data(g_hUdbCenter, 0x23010300, 0, 0, (uint8*)&g_ctrl_data[0].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23010400, 0, 0, (uint8*)&g_ctrl_data[0].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23020300, 0, 0, (uint8*)&g_ctrl_data[1].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23020400, 0, 0, (uint8*)&g_ctrl_data[1].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23030300, 0, 0, (uint8*)&g_ctrl_data[2].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23030400, 0, 0, (uint8*)&g_ctrl_data[2].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23040300, 0, 0, (uint8*)&g_ctrl_data[3].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23040400, 0, 0, (uint8*)&g_ctrl_data[3].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23050300, 0, 0, (uint8*)&g_ctrl_data[4].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23050400, 0, 0, (uint8*)&g_ctrl_data[4].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23060300, 0, 0, (uint8*)&g_ctrl_data[5].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23060400, 0, 0, (uint8*)&g_ctrl_data[5].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23070300, 0, 0, (uint8*)&g_ctrl_data[6].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23070400, 0, 0, (uint8*)&g_ctrl_data[6].gruop_reactive_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23080300, 0, 0, (uint8*)&g_ctrl_data[7].gruop_active_power, sizeof(int64));
    read_db_noamal_data(g_hUdbCenter, 0x23080400, 0, 0, (uint8*)&g_ctrl_data[7].gruop_reactive_power, sizeof(int64));  
}


void G_CurPowerDraw(HDC hdc)
{

	switch(g_ac_data_display_drawpage_index)
		{
			case 0:   //���й��͹��޹�����
				G_CurPowerDrawTotal(hdc);
				break;
			case 1:   //ǰ��·����
				G_CurPowerSep1(hdc);
				break;
			case 2:
				G_CurPowerSep2(hdc);
				break;
		} 
}


//ǰ��·������ʾҳ��
void G_CurPowerSep1(HDC hdc)
{
	char czBuf[70] = { 0 };
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
	
	TextOut(hdc,10,20,"�ܼ���1�й�����");
	TextOut(hdc,10,50,"�ܼ���2�й�����");
	TextOut(hdc,10,80,"�ܼ���3�й�����");
	TextOut(hdc,10,110,"�ܼ���4�й�����");

	for(int i = 0;i<4;i++)
	{
		sprintf(czBuf, "%6.4f %s", (double)g_ctrl_data[i].gruop_active_power / 10000, "kW");
		TextOut(hdc, 110, 20+i*30, czBuf);
		bzero(czBuf,sizeof(czBuf));	
	}
	
}

//����·������ʾҳ��
void G_CurPowerSep2(HDC hdc)
{
	char czBuf[70] = { 0 };
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
	
	TextOut(hdc,10,20,"�ܼ���5�й�����");
	TextOut(hdc,10,50,"�ܼ���6�й�����");
	TextOut(hdc,10,80,"�ܼ���7�й�����");
	TextOut(hdc,10,110,"�ܼ���8�й�����");

	for(int i = 0;i<4;i++)
	{
		sprintf(czBuf, "%6.4f %s", (double)g_ctrl_data[i+4].gruop_active_power / 10000, "kW");
		TextOut(hdc, 110, 20+i*30, czBuf);
		bzero(czBuf,sizeof(czBuf));	
	}
	
}


//��ǰ���й�/�޹�����ҳ��
PLUGIN_INFO g_layer_gruop_cur_powerTotal = {
    0x20000000/*id*/,
    G_CurPowerInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    G_CurPowerDraw/*Draw*/,
    NULL/*Timer*/,
    CurPowerKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

#endif



#if DESC("��ǰ����", 1)

int curEleQ_DM = 0;

//8·�ܼ���
typedef struct{
	//��i·�ܼ������/���й�����
	OOP_HENERGYS_T zjfpg[2];
	
	//��i·�ܼ�������޹�����
	OOP_HENERGYS_T day_reactiveEleQ;
	
	//��i·�ܼ�������޹�����
	OOP_HENERGYS_T month_reactiveEleQ;
}Group;

Group groupData[8];



/**
	��ҳ�滭�����еĲ���
	@param numOfLu: �ڼ�·,1-8·
	@dayOrMonth:�ջ�����    ,0����,1����
	
**/
void CurElecQPlateDraw(HDC hdc,int numOfLu,int dayOrMonth)
{	
	char czBuf[30] = { 0 };
	sprintf(czBuf, "�ܼ��� %d",numOfLu+1);

	ITEM_NOT_SELECT;

	SelectFont(hdc,g_guiComm.fontSmall);

	TextOut(hdc,15,20,czBuf);

 

	bzero(czBuf,sizeof(czBuf));
	dayOrMonth ==0? sprintf(czBuf, "%s%s%s", "��" ,"��", "�й�����"):sprintf(czBuf, "%s%s%s", "��" ,"��", "�й�����");
	TextOut(hdc,15,35,czBuf);

	bzero(czBuf,sizeof(czBuf));
	dayOrMonth ==0? sprintf(czBuf, "%s%s%s", "��" ,"��", "�޹�����"):sprintf(czBuf, "%s%s%s", "��" ,"��", "�޹�����");
	TextOut(hdc,120,35,czBuf);
	
	
	TextOut(hdc,15,50,"��");

	//TextOut(hdc,180,50,"kvarh");
	
	TextOut(hdc,15,65,"��");

	TextOut(hdc,15,80,"��");

	TextOut(hdc,15,95,"ƽ");

	TextOut(hdc,15,110,"��");
	
}


void CurElecQDrawTotal(HDC hdc,int numOfLu,int dayOrMonth)
{
	char czBuf[70] = { 0 };

	//�Ȼ�plate
	CurElecQPlateDraw(hdc,numOfLu,dayOrMonth);

	ITEM_NOT_SELECT;
	SelectFont(hdc,g_guiComm.fontSmall);
	
	
	//groupData[i]��ʾ��i+1·,zjfpg[0-4]��ʾ�յ��ܼ��ƽ��,zjfpg[5-9]��ʾ�µ��ܼ��ƽ��
	bzero(czBuf,sizeof(czBuf));
	//sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].zjfpg[dayOrMonth].nValue[0] / 10000, "kWh");
    valueDisplayProcess_strUnit(groupData[numOfLu].zjfpg[dayOrMonth].nValue[0], czBuf,70 , 4, "Wh");
    TextOut(hdc,40,50,czBuf);

	bzero(czBuf,sizeof(czBuf));
	//sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].zjfpg[dayOrMonth].nValue[1] / 10000, "kWh");
    valueDisplayProcess_strUnit(groupData[numOfLu].zjfpg[dayOrMonth].nValue[1], czBuf,70 , 4, "Wh");
    TextOut(hdc,40,65,czBuf);

	bzero(czBuf,sizeof(czBuf));
	//sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].zjfpg[dayOrMonth].nValue[2] / 10000, "kWh");
    valueDisplayProcess_strUnit(groupData[numOfLu].zjfpg[dayOrMonth].nValue[2], czBuf,70 , 4, "Wh");
    TextOut(hdc,40,80,czBuf);
	
	bzero(czBuf,sizeof(czBuf));
	//sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].zjfpg[dayOrMonth].nValue[3] / 10000, "kWh");
    valueDisplayProcess_strUnit(groupData[numOfLu].zjfpg[dayOrMonth].nValue[3], czBuf,70 , 4, "Wh");
    TextOut(hdc,40,95,czBuf);

	bzero(czBuf,sizeof(czBuf));
	//sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].zjfpg[dayOrMonth].nValue[4] / 10000, "kWh");
    valueDisplayProcess_strUnit(groupData[numOfLu].zjfpg[dayOrMonth].nValue[4], czBuf,70 , 4, "Wh");
    TextOut(hdc,40,110,czBuf);

	//
	bzero(czBuf,sizeof(czBuf));
//	dayOrMonth == 0?  sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].day_reactiveEleQ.nValue[0] / 10000, "kvarh")
//					:	sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].month_reactiveEleQ.nValue[0] / 10000, "kvarh");
    valueDisplayProcess_strUnit(dayOrMonth == 0? groupData[numOfLu].day_reactiveEleQ.nValue[0]:
                                                   groupData[numOfLu].month_reactiveEleQ.nValue[0], czBuf,70 , 4, "varh");
    TextOut(hdc,150,50,czBuf);
    bzero(czBuf,sizeof(czBuf));
//	dayOrMonth == 0?  sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].day_reactiveEleQ.nValue[1] / 10000, "kvarh")
//					:	sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].month_reactiveEleQ.nValue[1] / 10000, "kvarh");
    valueDisplayProcess_strUnit(dayOrMonth == 0? groupData[numOfLu].day_reactiveEleQ.nValue[1]:
                                                   groupData[numOfLu].month_reactiveEleQ.nValue[1], czBuf,70 , 4, "varh");
    TextOut(hdc,150,65,czBuf);	
    bzero(czBuf,sizeof(czBuf));
//	dayOrMonth == 0?  sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].day_reactiveEleQ.nValue[2] / 10000, "kvarh")
//					:	sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].month_reactiveEleQ.nValue[2] / 10000, "kvarh");
    valueDisplayProcess_strUnit(dayOrMonth == 0? groupData[numOfLu].day_reactiveEleQ.nValue[2]:
                                                   groupData[numOfLu].month_reactiveEleQ.nValue[2], czBuf,70 , 4, "varh");
   
    TextOut(hdc,150,80,czBuf);	
    bzero(czBuf,sizeof(czBuf));
//	dayOrMonth == 0?  sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].day_reactiveEleQ.nValue[3] / 10000, "kvarh")
//					:	sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].month_reactiveEleQ.nValue[3] / 10000, "kvarh");
    valueDisplayProcess_strUnit(dayOrMonth == 0? groupData[numOfLu].day_reactiveEleQ.nValue[3]:
                                                   groupData[numOfLu].month_reactiveEleQ.nValue[3], czBuf,70 , 4, "varh");
   
    TextOut(hdc,150,95,czBuf);	
    bzero(czBuf,sizeof(czBuf));
//	dayOrMonth == 0?  sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].day_reactiveEleQ.nValue[4] / 10000, "kWh")
//					:	sprintf(czBuf, "%6.4f %s", (double)groupData[numOfLu].month_reactiveEleQ.nValue[4] / 10000, "kWh");
    valueDisplayProcess_strUnit(dayOrMonth == 0? groupData[numOfLu].day_reactiveEleQ.nValue[4]:
                                                   groupData[numOfLu].month_reactiveEleQ.nValue[4], czBuf,70 , 4, "varh");
    TextOut(hdc,150,110,czBuf);	

}


void CurEleQInit(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(g_ctrl_data, 0, sizeof(g_ctrl_data));

	//��ȡ8��������ĵ��ա��µ��й����޹�����
	//int32            plus_active_power;         //�����й����� 0x24010500
    //int32            plus_reactive_power;       //�����޹����� 0x24010600
    //int64            gruop_active_power;        //�ܼ����й����� 0x23010300
    //int64            gruop_reactive_power;      //�ܼ����޹����� 0x23010400
    //OOP_HENERGYS_T   gruop_day_energy;        //�ܼ������й����� 0x23010700
    //OOP_HENERGYS_T   gruop_day_ractenergy;    //�ܼ������޹����� 0x23010800
    //OOP_HENERGYS_T   gruop_month_energy;      //�ܼ������й����� 0x23010900
    //OOP_HENERGYS_T   gruop_month_ractenergy;  //�ܼ������޹����� 0x23010a00
    //OOP_BITSTR8_T    ctrlPwTurn;              //0x230x0e00 �ܼ��鹦���ִ�����  ���ƶ�ȡ
    //OOP_BITSTR8_T    ctrlEcTurn;              //0x230x0f00 �ܼ������ִ�����  ���ƶ�ȡ
    //OOP_TGSETTING_T  ctrl_seting;             //��������״̬         0x23011000
    //OOP_TGCURSTATE_T ctrl_status;             //���Ƶ�ǰ״̬         0x23011100
    //OOP_PULSESETS_T  plus_param;              //��������       0x24010400
    //OOP_PULSESETS_B_T plus_b_param;           //��������       0x24011C00
    //OOP_OCTETVAR16_T pulse_addr;              //�����ַ       0x24010200
    //OOP_TGSETS_T     gruop_param;             //��������       0x23010200

	uint32 group1[2] ={0x23010700, 0x23010900};
	//��һ·����
	for(int j =0;j<2;j++)
	{
		read_db_noamal_data(g_hUdbCenter,group1[j],0,0,(uint8*)&(groupData[0].zjfpg[j]),sizeof(OOP_HENERGYS_T));
	}
	read_db_noamal_data(g_hUdbCenter, 0x23010800, 0, 0, (uint8*)&(groupData[0].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x23010a00, 0, 0, (uint8*)&(groupData[0].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));



	uint32 group2[2] ={0x23020700, 0x23020900};
	//�ڶ�·����
	for(int j =0;j<2;j++)
	{
		read_db_noamal_data(g_hUdbCenter,group2[j],0,0,(uint8*)&(groupData[1].zjfpg[j]),sizeof(OOP_HENERGYS_T));
	}
	read_db_noamal_data(g_hUdbCenter, 0x23020800, 0, 0, (uint8*)&(groupData[1].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x23020a00, 0, 0, (uint8*)&(groupData[1].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));

	
	uint32 group3[2] ={0x23030700, 0x23030900};
	//����·����
	for(int j =0;j<2;j++)
	{
		read_db_noamal_data(g_hUdbCenter,group3[j],0,0,(uint8*)&(groupData[2].zjfpg[j]),sizeof(OOP_HENERGYS_T));
	}
	read_db_noamal_data(g_hUdbCenter, 0x23030800, 0, 0, (uint8*)&(groupData[2].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x23030a00, 0, 0, (uint8*)&(groupData[2].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));
	

	uint32 group4[2] ={0x23040700, 0x23040900};
	//����·����
	for(int j =0;j<2;j++)
	{
		read_db_noamal_data(g_hUdbCenter,group4[j],0,0,(uint8*)&(groupData[3].zjfpg[j]),sizeof(OOP_HENERGYS_T));
	}
	read_db_noamal_data(g_hUdbCenter, 0x23040800, 0, 0, (uint8*)&(groupData[3].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_hUdbCenter, 0x23040a00, 0, 0, (uint8*)&(groupData[3].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));



	uint32 group5[2] ={0x23050700, 0x23050900};
	//����·����
		for(int j =0;j<2;j++)
		{
			read_db_noamal_data(g_hUdbCenter,group5[j],0,0,(uint8*)&(groupData[4].zjfpg[j]),sizeof(OOP_HENERGYS_T));
		}
		read_db_noamal_data(g_hUdbCenter, 0x23050800, 0, 0, (uint8*)&(groupData[4].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
		read_db_noamal_data(g_hUdbCenter, 0x23050a00, 0, 0, (uint8*)&(groupData[4].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));


	uint32 group6[2] ={0x23060700, 0x23060900};
	//����·����
		for(int j =0;j<2;j++)
		{
			read_db_noamal_data(g_hUdbCenter,group6[j],0,0,(uint8*)&(groupData[5].zjfpg[j]),sizeof(OOP_HENERGYS_T));
		}
		read_db_noamal_data(g_hUdbCenter, 0x23060800, 0, 0, (uint8*)&(groupData[5].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
		read_db_noamal_data(g_hUdbCenter, 0x23060a00, 0, 0, (uint8*)&(groupData[5].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));


	uint32 group7[2] ={0x23070700, 0x23070900};
	//����·����
		for(int j =0;j<2;j++)
		{
			read_db_noamal_data(g_hUdbCenter,group7[j],0,0,(uint8*)&(groupData[6].zjfpg[j]),sizeof(OOP_HENERGYS_T));
		}
		read_db_noamal_data(g_hUdbCenter, 0x23070800, 0, 0, (uint8*)&(groupData[6].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
		read_db_noamal_data(g_hUdbCenter, 0x23070a00, 0, 0, (uint8*)&(groupData[6].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));


	uint32 group8[2] ={0x23080700, 0x23080900};
	//�ڰ�·����
		for(int j =0;j<2;j++)
		{
			read_db_noamal_data(g_hUdbCenter,group8[j],0,0,(uint8*)&(groupData[7].zjfpg[j]),sizeof(OOP_HENERGYS_T));
		}
		read_db_noamal_data(g_hUdbCenter, 0x23080800, 0, 0, (uint8*)&(groupData[7].day_reactiveEleQ) , sizeof(OOP_HENERGYS_T));
		read_db_noamal_data(g_hUdbCenter, 0x23080a00, 0, 0, (uint8*)&(groupData[7].month_reactiveEleQ), sizeof(OOP_HENERGYS_T));

		
}


void CurElecQDraw(HDC hdc)
{
	
	CurElecQDrawTotal(hdc,g_ac_data_display_drawpage_index,curEleQ_DM);
			
}




//������Ϣ
BOOL CurEleQKeyMsg(int nKey)
{
	
	switch (nKey)
		{
			case KEY_DOWN:
				g_ac_data_display_drawpage_index>=7? g_ac_data_display_drawpage_index=0: ++g_ac_data_display_drawpage_index;
				break;

			case KEY_UP:
				g_ac_data_display_drawpage_index<=0?g_ac_data_display_drawpage_index=7 : --g_ac_data_display_drawpage_index;
				break;

			case KEY_CANCEL:
				CancelMenu();
				break;
			
			//���Ҽ��л��պ��µ�������ʾ
			case KEY_RIGHT:
				curEleQ_DM ==0? curEleQ_DM =1: curEleQ_DM =0;
				break;
			
			case KEY_LEFT:
				curEleQ_DM ==0? curEleQ_DM =1: curEleQ_DM =0;
				break;
				
		}
	return TRUE;
}


//��ǰ���й�/�޹�����ҳ��
PLUGIN_INFO g_layer_cur_EleQ = {
    0x20010000/*id*/,
    CurEleQInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    CurElecQDraw/*Draw*/,
    NULL/*Timer*/,
    CurEleQKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

#endif


uint32 g_event_num;
int    g_event_index;

OOP_PCTRLLOG_T *g_pctrllogEventlist;
OOP_PCTRLLOG_T g_pctrllogEvent;

void get_event_data(DB_CLIENT fp, uint32 oad_value, uint8** p_event_list, uint32 event_len)
{
    int                ret = ERR_NORMAL;
//    uint32             len     = 0;
    time_t             timep   = 0;
    uint32             handle  = 0;
    uint8*             ptmp    =  *p_event_list;

    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;
//    OOP_METER_T        oopMeter;
//    NOMAL_OAD_T        nomalOAD;
//    OOP_OCTETVAR16_T   m_tmnAddr;
    STEP_READ_RECORD_T stepReadRecord;

    g_event_num = 0;
    g_event_index = 0;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
//    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0;
    inReadRecord.cType = COL_NO_STORE;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = timep;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad_value;

//    inReadRecord.MAC.nNum = 6;
//
//    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
//    nomalOAD.logicId = 0;
//    nomalOAD.oad.value = 0x40010200;
//    ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&m_tmnAddr, &len);
//    if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
//    {
//        GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
//        return ;
//    }
//     memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);

     ret = db_read_record_start(fp, &inReadRecord, &handle, &g_event_num);
    if (0 != ret || 0 == g_event_num)
    {
        GUI_FMT_DEBUG(" db_read_record_start  ret=%d, num%d!\n", ret, g_event_num);
        return ;
    }

    ptmp = (uint8*)malloc(event_len*g_event_num);
    if (ptmp == NULL)
    {
        GUI_FMT_DEBUG(" malloc  err !\n");
        g_event_num = 0;
        return ;
    }
    memset(ptmp, 0, event_len*g_event_num);
    
    memcpy(&stepReadRecord.MAC, &inReadRecord.MAC, sizeof(OOP_OCTETVAR64_T));

    for(uint32 i=0; i<g_event_num; i++)
    {
        /* �����һ����¼ */
        ret = db_read_record_step(fp, handle, &stepReadRecord, &stepRecordOut);
        if (0 == ret)
        {
            memcpy(ptmp+(i*event_len), stepRecordOut.record[0].outDataBuf, event_len);
        }
    }
    *p_event_list = ptmp;

    /*��������¼*/
     db_read_record_end(fp, handle);
}

#if DESC("���ؼ�¼",1)
//����״̬���¼���¼��,ʹ��OOP_PCTRLLOG_T������
//OOP_PCTRLLOG_T *g_pctrllogEventlist;


void power_ctr_rcrd_init(DWORD dwPluginId)
{
	g_event_index = 0;
    g_pctrllogEventlist = NULL;
    get_event_data(g_hUdbCenter, 0x32000200, (uint8**)&g_pctrllogEventlist, sizeof(OOP_PCTRLLOG_T));

}

void power_ctr_rcrd_Uninit()
{
    if (g_pctrllogEventlist != NULL)
    {
        free(g_pctrllogEventlist);
        g_pctrllogEventlist = NULL;
    }
}


/*
�ú��������Ǽ�
*/
void power_ctr_rcrd_Platedraw(HDC hdc)
{
    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
    int nLeft = 20;
    int nTop = 20;
    switch (g_pctrllogEvent.source.oi)
    {
    case 0x8103:
        nTop +=15;
        TextOut(hdc, nLeft, nTop, "ʱ");
        nTop +=15;
        TextOut(hdc, nLeft, nTop, "��");
        break;
    case 0x8104:
        nTop +=15;
        TextOut(hdc, nLeft, nTop, "��");
        nTop +=15;
        TextOut(hdc, nLeft, nTop, "��");
        break;
    case 0x8105:
        TextOut(hdc, nLeft, nTop, "Ӫ");
        nTop +=15;
        TextOut(hdc, nLeft, nTop, "ҵ");
        nTop +=15;
        TextOut(hdc, nLeft, nTop, "��");
        nTop +=15;
        TextOut(hdc, nLeft, nTop, "ͣ");
        break;
    case 0x8106:
        nTop +=15;
        TextOut(hdc, nLeft, nTop, "��");
        nTop +=15;
        TextOut(hdc, nLeft, nTop, "��");
        break;
    default:
        nTop +=30;
        TextOut(hdc, nLeft, nTop, "��");
        break;
    }
    nTop +=15;
    TextOut(hdc, nLeft, nTop, "��");
    nTop +=15;
    TextOut(hdc, nLeft, nTop, "��");
    nTop +=15;
    TextOut(hdc, nLeft, nTop, "¼");
    nTop = 74;
    TextOut(hdc, 60, nTop, "��բǰ����");
    nTop +=18;
    TextOut(hdc, 60, nTop, "��բ����");
    nTop +=18;
    TextOut(hdc, 60, nTop, "��ʱ��ֵ");
    
}

void power_ctr_rcrd_TotalDraw(HDC hdc)
{
	char czBuf[30] = { 0 };
    int nTop = 20;
	//��ȡ��i���ִεĹ�����բ��¼��Ԫ
	memset(&g_pctrllogEvent, 0, sizeof(OOP_PCTRLLOG_T));
    if (g_pctrllogEventlist != NULL)
    {
        g_pctrllogEvent = g_pctrllogEventlist[g_event_index];
    }

	//���Ȼ�һ��ģ��
	power_ctr_rcrd_Platedraw(hdc);

	//���ִ�
	if(g_pctrllogEvent.turnState.nValue == 0)
    {
        sprintf(czBuf, "�ִ� -");
    }
	else
	{
		for (int i = 0; i < 8; i++)
        {
            if ((g_pctrllogEvent.turnState.nValue >> i & 0x01) != 0)
            {
                sprintf(czBuf, "%s %d ","�ִ�", i + 1);
                break;
            }
        }
	}
	//	sprintf(czBuf, "%s %d ","�ִ�", (uint8)g_pctrllogEvent.turnState.nValue);
    TextOut(hdc, 60, nTop, czBuf);
    nTop +=18;
    if (g_pctrllogEvent.objCtrl == 0)
    {
        sprintf(czBuf, "���ƶ���: �ܼ��� -");
    }
    else
    {
        sprintf(czBuf, "���ƶ���: �ܼ��� %d", g_pctrllogEvent.objCtrl&0x0F);
    }
    TextOut(hdc, 60, nTop, czBuf);

	//��ʱ��
    nTop +=18;
	SelectFont(hdc, g_guiComm.fontSmall);
	bzero(czBuf,sizeof(czBuf));
	if(g_pctrllogEvent.tmStart.year == 0)
		sprintf(czBuf,  "-��-��-��-:-");
	else
	{
		sprintf(czBuf, "%d��%d��%d��%d:%d ",           (uint16)g_pctrllogEvent.tmStart.year,
													(uint8)g_pctrllogEvent.tmStart.month,
													(uint8)g_pctrllogEvent.tmStart.day,
													(uint8)g_pctrllogEvent.tmStart.hour,
													(uint8)g_pctrllogEvent.tmStart.minute);
	}
    TextOut(hdc, 60, nTop, czBuf);

    nTop +=18;
	//����բǰ����:llsetings
	SelectFont(hdc, g_guiComm.fontSmall);
	bzero(czBuf,sizeof(czBuf));
	if ((g_pctrllogEvent.nIndex == 0) || (g_pctrllogEvent.power == unlawPower))
    {
        sprintf(czBuf, "-------- kW");
    }
    else
    {
        float64 tmp = g_pctrllogEvent.power;
        sprintf(czBuf, "%6.4f kW", tmp / 10000);
    }
	TextOut(hdc, 150, nTop, czBuf);

    nTop +=18;
	//����բ����
	SelectFont(hdc, g_guiComm.fontSmall);
	bzero(czBuf,sizeof(czBuf));
	if ((g_pctrllogEvent.nIndex == 0) || (g_pctrllogEvent.power2Min == unlawPower))
    {
        sprintf(czBuf, "-------- kW");
    }
    else
    {
        float64 tmp = g_pctrllogEvent.power2Min;
        sprintf(czBuf, "%6.4f kW", tmp / 10000);
    }
    TextOut(hdc, 150, nTop, czBuf);

    nTop +=18;
	//����ʱ��ֵ
	SelectFont(hdc, g_guiComm.fontSmall);
	bzero(czBuf,sizeof(czBuf));
	if ((g_pctrllogEvent.nIndex == 0)||(g_pctrllogEvent.llSetting == unlawPower))
    {
        sprintf(czBuf, "-------- kW");
    }
    else
    {
        float64 tmp = g_pctrllogEvent.llSetting;
        sprintf(czBuf, "%6.4f kW", tmp / 10000);
    }
    TextOut(hdc, 150, nTop, czBuf);

	
}



void power_ctr_rcrd_Draw(HDC hdc)
{
	power_ctr_rcrd_TotalDraw(hdc);

}

BOOL power_ctr_rcrd_Key_Msg(int nkey)
{
    switch (nkey)
    {
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        g_event_index++;
        if ((uint32)g_event_index >= g_event_num)
        {
            g_event_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    case KEY_LEFT:
    {
        g_event_index--;
        if (g_event_index < 0)
        {
            g_event_index = (g_event_num==0)?0: g_event_num-1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }
}

//���ؼ�¼
PLUGIN_INFO g_layer_power_ctr_evt_display = {
    0x20040000/*id*/,
    power_ctr_rcrd_init/*InitLayer*/,
    power_ctr_rcrd_Uninit/*UninitLayer*/,
    power_ctr_rcrd_Draw/*Draw*/,
    NULL/*Timer*/,
    power_ctr_rcrd_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("��ؼ�¼",1)

OOP_ECTRLLOG_T *g_ectrllogEventlist;
OOP_ECTRLLOG_T g_ectrllogEvent;
uint8  g_ectrllogflag;        


void elect_ctr_evt_init(DWORD dwPluginId)
{
	g_event_index = 0;
    g_ectrllogflag = 0xFF;
    g_ectrllogEventlist = NULL;
    get_event_data(g_hUdbCenter, 0x32010200, (uint8**)&g_ectrllogEventlist, sizeof(OOP_ECTRLLOG_T));
}
void elect_ctr_evt_Uninit()
{
    if (g_ectrllogEventlist != NULL)
    {
        free(g_ectrllogEventlist);
        g_ectrllogEventlist = NULL;
    }
}

void elect_ctr_rcrd_Platedraw(HDC hdc)
{
    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontSmall);
    if (g_ectrllogEvent.source.oi == 0x8107 )
        TextOut(hdc, 20, 30, "��");
    else if(g_ectrllogEvent.source.oi == 0x8108)
        TextOut(hdc, 20, 30, "��");
    TextOut(hdc, 20, 45, "��");
    TextOut(hdc, 20, 60, "��");
    TextOut(hdc, 20, 75, "��");
    TextOut(hdc, 20, 90, "¼");
    
    
    if (g_ectrllogEvent.source.oi == 0x8107)
    {
        uint8 index=0xFF;
        OOP_8107_CONFIG_T config_8107;
        memset(&config_8107,0,sizeof(config_8107));
        read_db_noamal_data(g_hUdbCenter, 0x81070200, 0, 0, (uint8*)&config_8107, sizeof(config_8107));
        if(config_8107.nNum !=0 && config_8107.item[0].object !=0)
        {
            for(uint8 i=0;i<config_8107.nNum;i++)
            {
                if((uint16)g_ectrllogEvent.objCtrl == config_8107.item[i].object)
                {
                    index = i;
                    break;
                }
            }
            if(index!=0xff)
            {
//                if(config_8107.item[index].type==1)
//                {
//                    TextOut(hdc, 50, 90, "��բʱʣ����");
//                    g_ectrllogflag = 1;
//                }
//                else if(config_8107.item[index].type==0)
//                {
//                    TextOut(hdc, 50, 90, "��բʱʣ�����");
//                    g_ectrllogflag = 0;
//                }
                //ע��,698��Լ�в������ֵ����ػ��ǵ�ѿ�,����صĵ�λͳһдΪkwh/Ԫ
                if(g_ectrllogEvent.source.oi == 0x8107)
                    TextOut(hdc, 50, 90, "��բʱʣ�����/��");
                else
                    TextOut(hdc, 50, 90, "��բʱʣ�����");
            }
            else
            {
                TextOut(hdc, 50, 90, "��բʱʣ�����");
            }
        }
        else
        {
            g_ectrllogflag = 0xFF;
            TextOut(hdc, 50, 90, "��բʱʣ�����");
        }
        
    }      
    else if(g_ectrllogEvent.source.oi == 0x8108)
        TextOut(hdc, 50, 90, "��բʱ�����ܵ���");
    else
        TextOut(hdc, 50, 90, "��բʱʣ�����");
}

void elect_ctr_rcrd_TotalDraw(HDC hdc)
{
	char czBuf[30] = { 0 };
	//��ȡ��i���ִεĵ����բ��¼��Ԫ
	memset(&g_ectrllogEvent, 0, sizeof(OOP_PCTRLLOG_T));
    if (g_ectrllogEventlist != NULL)
    {
        g_ectrllogEvent = g_ectrllogEventlist[g_event_index];
    }
	//���Ȼ�һ��ģ��
	elect_ctr_rcrd_Platedraw(hdc);
	//���ִ�
	SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;
    int j = 0;
	if(g_ectrllogEvent.turnState.nValue == 0)
	{
		sprintf(czBuf, "%s", "�ִ� -");
		TextOut(hdc, 50, 30, czBuf);
	}
		
	else
	{
		bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf, "%s ", "�ִ�");
	    TextOut(hdc, 50, 30, czBuf);
		for (int i = 0; i < 8; i++)
        {
            if ((g_ectrllogEvent.turnState.nValue >> i & 0x01) != 0)
            {
                sprintf(czBuf, "%d ", i+1);
                TextOut(hdc, 80 + 10*j++, 30, czBuf);
            }
        }
	}
	//	sprintf(czBuf, "%s %d ","�ִ�", (uint8)g_pctrllogEvent.turnState.nValue);
    if (g_ectrllogEvent.objCtrl == 0)
    {
        sprintf(czBuf, "���ƶ���: �ܼ��� -");
    }
    else
    {
        sprintf(czBuf, "���ƶ���: �ܼ��� %d", g_ectrllogEvent.objCtrl&0x0F);
    }
    TextOut(hdc, 50, 50,czBuf);

	//��ʱ��
	SelectFont(hdc, g_guiComm.fontSmall);
	bzero(czBuf,sizeof(czBuf));
	if(g_ectrllogEvent.tmStart.year == 0)
		sprintf(czBuf,  "-��-��-��-:-");
	else
	{
		sprintf(czBuf, "%04d��%02d��%02d��%02d:%02d ",           (uint16)g_ectrllogEvent.tmStart.year,
													(uint8)g_ectrllogEvent.tmStart.month,
													(uint8)g_ectrllogEvent.tmStart.day,
													(uint8)g_ectrllogEvent.tmStart.hour,
													(uint8)g_ectrllogEvent.tmStart.minute);
	}
    TextOut(hdc, 50, 70, czBuf);
    if(g_ectrllogEvent.tmStart.year != 0)
    {  
        //����բʱʣ�����:energy?
        SelectFont(hdc, g_guiComm.fontSmall);
        bzero(czBuf,sizeof(czBuf));
//        if(g_ectrllogflag == 1)
//        {
//            sprintf(czBuf, "%12.4f Ԫ", g_ectrllogEvent.energy * 0.0001);
//        }
//        else 
//        {
//            MakeFormatedStringOfEnergy((double)g_ectrllogEvent.energy, czBuf);
//        }
        if(g_ectrllogEvent.source.oi == 0x8107)
            sprintf(czBuf, "%12.4f kwh/Ԫ", g_ectrllogEvent.energy * 0.0001);
        else
            sprintf(czBuf, "%12.4f kwh", g_ectrllogEvent.energy * 0.0001);
    }
    else
    {
        sprintf(czBuf, "------ kWh");
    }

	TextOut(hdc, 50, 110, czBuf);
}



void elect_ctr_rcrd_Draw(HDC hdc)
{
	elect_ctr_rcrd_TotalDraw(hdc);

}

BOOL elect_ctr_rcrd_Key_Msg(int nkey)
{
	switch (nkey)
	{
		case KEY_DOWN:
				g_event_index>=7? g_event_index=0: ++g_event_index;
				break;

			case KEY_UP:
				g_event_index<=0?g_event_index=7 : --g_event_index;
				break;

			case KEY_CANCEL:
				g_event_index = 0;
				CancelMenu();
				break;
	}
	return TRUE;
}


//��ؼ�¼
PLUGIN_INFO g_layer_elect_ctr_evt_display = {
    0x20050000/*id*/,
    elect_ctr_evt_init/*InitLayer*/,
    elect_ctr_evt_Uninit/*UninitLayer*/,
    elect_ctr_rcrd_Draw/*Draw*/,
    NULL/*Timer*/,
    power_ctr_rcrd_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("ң�ؼ�¼",1)
OOP_RCTRLLOG_T *g_rctrllogEventlist;
OOP_RCTRLLOG_T g_rctrllogEvent;

//ң�ؼ�¼��Ԫ�е�prePower[8]��llPower[8]�����8�ִε���բǰ����բ����
//oad�е�index������ִ�ֵ

void rct_ctr_evt_init(DWORD dwPluginId)
{
	g_event_index = 0;
    g_rctrllogEventlist = NULL;
    get_event_data(g_hUdbCenter, 0x31150200, (uint8**)&g_rctrllogEventlist, sizeof(OOP_RCTRLLOG_T));
}

void rct_ctr_evt_Uninit()
{
    if (g_rctrllogEventlist != NULL)
    {
        free(g_rctrllogEventlist);
        g_rctrllogEventlist = NULL;
    }
}


void rct_ctr_rcrd_Platedraw(HDC hdc)
{
  	ITEM_NOT_SELECT;
	SelectFont(hdc,g_guiComm.fontSmall);

	TextOut(hdc, 20, 30, "ң");
	TextOut(hdc, 20, 50, "��");
	TextOut(hdc, 20, 70, "��");
	TextOut(hdc, 20, 90, "¼");

	TextOut(hdc, 50, 70, "��բǰ����");
	TextOut(hdc, 50, 90, "��բ����");
	
}

void rct_ctr_rcrd_TotalDraw(HDC hdc)
{
	char czBuf[40] = { 0 };

	//���Ȼ�һ��ģ��
	rct_ctr_rcrd_Platedraw(hdc);

	//�Ȼ�ȡÿ��ÿ���ִε��¼��ṹ
	memset(&g_rctrllogEvent, 0, sizeof(OOP_RCTRLLOG_T));
    if (g_rctrllogEventlist != NULL)
    {
        g_rctrllogEvent = g_rctrllogEventlist[g_event_index];
    }

	//��ȡ��ǰ�ִ�
	uint8 turn  = g_rctrllogEvent.source.oad.nIndex;
	//ȷ��������Ч����
	if(turn != 0)
	{
		bzero(czBuf,sizeof(czBuf));
		sprintf(czBuf,"�̵��� %d ", turn);
		TextOut(hdc, 50, 30, czBuf);

		bzero(czBuf,sizeof(czBuf));
		sprintf(czBuf,"%04hu��%02hu��%02hu��%02hu:%02hu",g_rctrllogEvent.tmStart.year,
								g_rctrllogEvent.tmStart.month,
								g_rctrllogEvent.tmStart.day,
								g_rctrllogEvent.tmStart.hour,
								g_rctrllogEvent.tmStart.minute);
		TextOut(hdc, 50, 50, czBuf);



		
		bzero(czBuf,sizeof(czBuf));
		if (g_rctrllogEvent.llPower[0] == unlawPower)
		    sprintf(czBuf,"----  kW");
		else
		    sprintf(czBuf,"%6.4f kW",(float64)g_rctrllogEvent.llPower[0] / 10000);
        TextOut(hdc, 130, 70, czBuf);

		bzero(czBuf,sizeof(czBuf));
		if (g_rctrllogEvent.prePower[0] == unlawPower)
		    sprintf(czBuf,"----  kW");
		else
			sprintf(czBuf,"%6.4f kW",(float64)g_rctrllogEvent.prePower[0] / 10000);
		TextOut(hdc, 130, 90, czBuf);

	}
	else
	{
		//û�ж�������
		bzero(czBuf,sizeof(czBuf));
		sprintf(czBuf,"�̵��� - ");
		TextOut(hdc, 50, 30, czBuf);

		bzero(czBuf,sizeof(czBuf));
		sprintf(czBuf,"-��-��-��-:- ");
		TextOut(hdc, 50, 50, czBuf);
		
		bzero(czBuf,sizeof(czBuf));
		sprintf(czBuf,"----  kW");
		TextOut(hdc, 130, 70, czBuf);

		bzero(czBuf,sizeof(czBuf));
		sprintf(czBuf,"----  kW");
		TextOut(hdc, 130, 90, czBuf);
		
	}	
}

void rct_ctr_rcrd_Draw(HDC hdc)
{
	rct_ctr_rcrd_TotalDraw(hdc);

}

BOOL rct_ctr_rcrd_Key_Msg(int nkey)
{
	switch (nkey)
	{
		case KEY_DOWN:
				g_event_index>=7? g_event_index=0: ++g_event_index;
				break;

			case KEY_UP:
				g_event_index<=0?g_event_index=7 : --g_event_index;
				break;

			case KEY_CANCEL:
				g_event_index = 0;
				CancelMenu();
				break;
	}
	return TRUE;
}

//ң�ؼ�¼
PLUGIN_INFO g_layer_rct_ctr_evt_display = {
    0x20060000/*id*/,
    rct_ctr_evt_init/*InitLayer*/,
    rct_ctr_evt_Uninit/*UninitLayer*/,
    rct_ctr_rcrd_Draw/*Draw*/,
    NULL/*Timer*/,
    power_ctr_rcrd_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};


#endif



#if DESC("ʧ���¼",1)
OOP_POWERLOG_T *g_powerlogEventlist;
uint32 g_powerPage = 0;     
uint32 powerPageMax;
uint32 powerCurPage;


void pwct_ctr_evt_init(DWORD dwPluginId)
{
	g_event_index = 0;
    g_powerlogEventlist = NULL;
    get_event_data(g_hUdbCenter, 0x31060200, (uint8**)&g_powerlogEventlist, sizeof(OOP_POWERLOG_T));

    powerCurPage = 1;
    if(g_event_num%5==0)
        powerPageMax = g_event_num/5;
    else
        powerPageMax =  g_event_num/5+1;
}
void pwct_ctr_evt_Uninit()
{
    if (g_powerlogEventlist != NULL)
    {
        free(g_powerlogEventlist);
        g_powerlogEventlist = NULL;
    }
}

void pwct_ctr_rcrd_Platedraw(HDC hdc)
{
  	ITEM_NOT_SELECT;
	SelectFont(hdc,g_guiComm.fontSmall);

	TextOut(hdc, 15, 40, "ʧ");
	TextOut(hdc, 15, 60, "��");
	TextOut(hdc, 15, 80, "��");
	TextOut(hdc, 15, 100, "¼");

	TextOut(hdc, 90, 20, "��ʼʱ��");
	TextOut(hdc, 180, 20, "�ָ�ʱ��");
	
}


void pwct_ctr_rcrd_TotalDraw(HDC hdc)
{
	pwct_ctr_evt_init(0x20070000);
	char czBuf[40] = { 0 };
    uint32 num = 0;
	//���Ȼ�һ��ģ��
	pwct_ctr_rcrd_Platedraw(hdc);

	//��ȡͣ���¼��Ԫ
	OOP_POWERLOG_T g_powerlogEvent[g_event_num];
	memset(g_powerlogEvent, 0, sizeof(OOP_POWERLOG_T)*g_event_num);

    if ((g_powerlogEventlist != NULL) && (g_event_num != 0))
    {
//   	    for(uint32 j = 0;j < g_event_num;j++)
//   	    {
//   		    (g_powerlogEvent.source.nValue = 1 ��ʾ���¼���һ���ϵ��¼�,���м�¼��ͣ��ͻظ�ʱ��
//   		    if((g_powerlogEventlist[j].source.nValue == 1)&&(g_powerlogEventlist[j].tmStart.year !=0))
//            if(g_powerlogEventlist[j].tmStart.year !=0)
//   		    {
//   		        g_powerlogEvent[num++] = g_powerlogEventlist[j];
//   		    }
//   	   }
//       g_event_num = num;

       for(uint32 j = 0;j < g_event_num;j++)
   	   {
   		        g_powerlogEvent[num++] = g_powerlogEventlist[j];
   	   }

   		//�����¼�list,�Ӽ�¼��Ԫ�л�ȡͣ��ļ�¼(�¼�����Դenum=ͣ��)
   		for(uint32 i = 0;i < 5;i++)
   		{
//   			if (g_powerlogEvent[g_powerPage*5 + i].source.nValue != 1){
//   				break;
//   			}
       		bzero(czBuf,sizeof(czBuf));
            if(g_powerlogEvent[g_powerPage*5 + i].tmStart.month!=0 &&g_powerlogEvent[g_powerPage*5 + i].nIndex!=0)
       		{
                sprintf(czBuf, "%02d.%02hu/%02hu %02hu:%02hu",
       					g_powerPage*5 + i + 1,
       					g_powerlogEvent[g_powerPage*5 + i].tmStart.month,
       					g_powerlogEvent[g_powerPage*5 + i].tmStart.day,
       					g_powerlogEvent[g_powerPage*5 + i].tmStart.hour,
       					g_powerlogEvent[g_powerPage*5 + i].tmStart.minute);
       		    TextOut(hdc, 30, 20+18*(i+1), czBuf);
            }
       
       		bzero(czBuf,sizeof(czBuf));
            if(g_powerlogEvent[g_powerPage*5 + i].tmEnd.month!=0&&g_powerlogEvent[g_powerPage*5 + i].nIndex!=0)
            {
       		    sprintf(czBuf, "%02hu/%02hu %02hu:%02hu", 
       					g_powerlogEvent[g_powerPage*5 + i].tmEnd.month,
       					g_powerlogEvent[g_powerPage*5 + i].tmEnd.day,
       					g_powerlogEvent[g_powerPage*5 + i].tmEnd.hour,
       					g_powerlogEvent[g_powerPage*5 + i].tmEnd.minute);
       		    TextOut(hdc, 140, 20+18*(i+1), czBuf);	
            }

            bzero(czBuf,sizeof(czBuf));
            if(g_powerlogEvent[g_powerPage*5 + i].nIndex!=0)
            {
                sprintf(czBuf,g_powerlogEvent[g_powerPage*5 + i].source.nValue==0?"ͣ��":"�ϵ�");
                TextOut(hdc, 140+65, 20+18*(i+1), czBuf);
            }
   		}
    }
	else
	{
		//û��ͣ���¼
	    TextOut(hdc, 90, 50, "û��ͣ���¼");
    }
}

void pwct_ctr_rcrd_TotalDrawNew(HDC hdc)
{
    char czBuf[60] = { 0 };
    pwct_ctr_rcrd_Platedraw(hdc);
    //��ȡͣ���¼��Ԫ
	OOP_POWERLOG_T powerlogEvent[g_event_num];
	memset(powerlogEvent, 0, sizeof(OOP_POWERLOG_T)*g_event_num);
    
    if ((g_powerlogEventlist != NULL) && (g_event_num != 0))
    {
        memcpy(powerlogEvent, g_powerlogEventlist,sizeof(OOP_POWERLOG_T)*g_event_num);

        for(uint32 i = 0;i < 5;i++)
        {
            if(powerCurPage*5-5 + i+1 > g_event_num)
                break;

//            bzero(czBuf,sizeof(czBuf));
//            if(powerlogEvent[powerCurPage*5-5 + i].nIndex!=0)
//            {
//                sprintf(czBuf,powerlogEvent[powerCurPage*5-5 + i].source.nValue==0?"ͣ��":"�ϵ�");
//                TextOut(hdc, 30, 20+18*(i+1), czBuf);
//            }
            
            bzero(czBuf,sizeof(czBuf));
            if(powerlogEvent[powerCurPage*5-5 + i].tmStart.month!=0 &&powerlogEvent[powerCurPage*5-5 + i].nIndex!=0)
       		{
                sprintf(czBuf, "%02d.%s %02hu/%02hu %02hu:%02hu",
       					powerCurPage*5-5 + i+1,
       					powerlogEvent[powerCurPage*5-5 + i].source.nValue==0?"ͣ��":"�ϵ�",
       					powerlogEvent[powerCurPage*5-5 + i].tmStart.month,
       					powerlogEvent[powerCurPage*5-5 + i].tmStart.day,
       					powerlogEvent[powerCurPage*5-5 + i].tmStart.hour,
       					powerlogEvent[powerCurPage*5-5 + i].tmStart.minute);
       		    TextOut(hdc, 30, 20+18*(i+1), czBuf);
            }
            
            bzero(czBuf,sizeof(czBuf));
            if(powerlogEvent[powerCurPage*5-5 + i].tmEnd.month!=0 &&powerlogEvent[powerCurPage*5-5 + i].nIndex!=0)
       		{
                sprintf(czBuf, "%02hu/%02hu %02hu:%02hu",
       					powerlogEvent[powerCurPage*5-5 + i].tmEnd.month,
       					powerlogEvent[powerCurPage*5-5 + i].tmEnd.day,
       					powerlogEvent[powerCurPage*5-5 + i].tmEnd.hour,
       					powerlogEvent[powerCurPage*5-5 + i].tmEnd.minute);
       		    TextOut(hdc, 165, 20+18*(i+1), czBuf);
            }
        }
    }
    else
	{
		//û��ͣ���¼
	    TextOut(hdc, 90, 50, "û��ͣ���¼");
    }


}


void pwct_ctr_rcrd_Draw(HDC hdc)
{
	//pwct_ctr_rcrd_TotalDraw(hdc);
    pwct_ctr_rcrd_TotalDrawNew(hdc);

}

BOOL pwct_ctr_rcrd_Key_MsgNew(int nkey)
{
    switch (nkey)
	{
        case KEY_CANCEL:
			CancelMenu();
			break;
        case KEY_DOWN:
            powerCurPage++;
            if(powerCurPage>powerPageMax)
                powerCurPage = 1;
            break;
        case KEY_UP:
            powerCurPage--;
            if(powerCurPage<1)
                powerCurPage = powerPageMax;
            break;
    }
    return FALSE;
}


BOOL pwct_ctr_rcrd_Key_Msg(int nkey)
{
	//uint32 powerPageMax;
	if (g_event_num%5 == 0)
		powerPageMax = g_event_num/5 - 1;
	else
		powerPageMax = g_event_num/5;
	switch (nkey)
	{
		case KEY_CANCEL:
			g_event_index = 0;
			CancelMenu();
			break;
		case KEY_DOWN:
			g_powerPage>=powerPageMax? g_powerPage=0: ++g_powerPage;
			break;
		case KEY_UP:
			g_powerPage<=0?g_powerPage=powerPageMax : --g_powerPage;
			break;
	}
	return TRUE;
}

//ʧ���¼
PLUGIN_INFO g_layer_pwct_ctr_evt_display = {
    0x20070000/*id*/,
    pwct_ctr_evt_init/*InitLayer*/,
    pwct_ctr_evt_Uninit/*UninitLayer*/,
    pwct_ctr_rcrd_Draw/*Draw*/,
    NULL/*Timer*/,
    //pwct_ctr_rcrd_Key_Msg/*keymsg*/,
    pwct_ctr_rcrd_Key_MsgNew,
    NULL/*ipc*/
};


#endif



#if DESC("����״̬",1)
//ע�⣺����״̬ʵ������ң��״̬,��ң�Ŷ�Ӧ�ż̵���(����)

OOP_SWITCHIN_STATE_T    switchState;    //ң��״̬
OOP_SWITCHIN_FLAG_T     switchFlag;     //�����־

void switch_status_display_init(DWORD dwPluginId)
{
    memset(&switchState, 0, sizeof(OOP_SWITCHIN_STATE_T));
    memset(&switchFlag, 0, sizeof(OOP_SWITCHIN_FLAG_T));
}

#define SWITCH_SHOW(S, F, i) 0 == ((F.inputFlag.bitValue >> i) & 0x01)  ? "--" : (0 == S.state[i].st ? "��" : "��")

void switch_status_display_draw(HDC hdc)
{
    char czBuf[64] = {0};
    GUI_FMT_DEBUG("switch_status_display_draw begin \n");
    read_db_noamal_data(g_hUdbCenter, 0xf2030200, 0, 0, (uint8*)&switchState, sizeof(OOP_SWITCHIN_STATE_T));
    read_db_noamal_data(g_hUdbCenter, 0xf2030400, 0, 0, (uint8*)&switchFlag, sizeof(OOP_SWITCHIN_FLAG_T));
    GUI_FMT_DEBUG("switch_status_display_draw end \n");
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    TextOut(hdc, 96, 25, "ң��״̬");

    bzero(czBuf, sizeof(czBuf));
    sprintf(czBuf, "���: 1   2   3   4   5   6   7   8");
    TextOut(hdc, 10, 45, czBuf);

	bzero(czBuf,sizeof(czBuf));
	sprintf(czBuf, "״̬: %s  %s  %s  %s  %s  %s  %s  %s", SWITCH_SHOW(switchState, switchFlag, 0), 
	                                                SWITCH_SHOW(switchState, switchFlag, 1),
	                                                SWITCH_SHOW(switchState, switchFlag, 2),
	                                                SWITCH_SHOW(switchState, switchFlag, 3),
	                                                SWITCH_SHOW(switchState, switchFlag, 4),
	                                                SWITCH_SHOW(switchState, switchFlag, 5),
	                                                SWITCH_SHOW(switchState, switchFlag, 6),
	                                                SWITCH_SHOW(switchState, switchFlag, 7));
	TextOut(hdc, 10, 60, czBuf);
#ifdef AREA_JIANGSU
    bzero(czBuf, sizeof(czBuf));
    sprintf(czBuf, "���: 9   10  11  12  13  14  15  16");
    TextOut(hdc, 10, 85, czBuf);

	bzero(czBuf,sizeof(czBuf));
	sprintf(czBuf, "״̬: %s  %s  %s  %s  %s  %s  %s  %s", SWITCH_SHOW(switchState, switchFlag, 8), 
	                                                SWITCH_SHOW(switchState, switchFlag, 9),
	                                                SWITCH_SHOW(switchState, switchFlag, 10),
	                                                SWITCH_SHOW(switchState, switchFlag, 11),
	                                                SWITCH_SHOW(switchState, switchFlag, 12),
	                                                SWITCH_SHOW(switchState, switchFlag, 13),
	                                                SWITCH_SHOW(switchState, switchFlag, 14),
	                                                SWITCH_SHOW(switchState, switchFlag, 15));
	TextOut(hdc, 10, 100, czBuf);
#endif
}

//����״̬
PLUGIN_INFO g_layer_switch_state_display = {
    0x20030000/*id*/,
    switch_status_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    switch_status_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif

OOP_POWCON_TIME_T ptPowconTime;
PCSEG_INFO_T g_PCSegData[16]; //-8������ʱ�εķֲ� ����һ�㷭��-
uint8        g_bPCSegNo = 0; //-����ʱ������-

uint8 appctrl_cal_period()//-���㹦��ʱ��-
{
    memset(&ptPowconTime ,0x0,sizeof(OOP_POWCON_TIME_T));
    int ret = read_db_noamal_data(g_hUdbCenter, 0x81010200, 0, 0, (uint8*)&ptPowconTime, sizeof(OOP_POWCON_TIME_T));
    if(0 != ret)
    {
        GUI_FMT_DEBUG("Get PowconTime time 0x%x failed, ret %d!\n", 0x81010200, ret);
    }
    uint8 aBuf[12] = {0};
    uint8 ctrlState[48] = {0};
    uint8 i,j;        //-ѭ������-
    uint8 k = 0;
    char temp[400] = {0};
    uint16 offset = 0;

    memcpy(aBuf, ptPowconTime.period, sizeof(ptPowconTime.period));
    for(i = 0; i < 12; i++)
    {
        for(j = 0; j < 4; j++)
        {
            ctrlState[k] = ((aBuf[i] >> (2 * j)) & 0x03);
            if(3 == ctrlState[k])
            {
                ctrlState[k] = 0;
            }
            k++;
        }
    }
    memset(g_PCSegData ,0x0,sizeof(g_PCSegData));
    g_bPCSegNo = 0;
    
    g_PCSegData[0].home = 0;
    g_PCSegData[0].end = 30;
    g_PCSegData[0].index = ctrlState[0];

    for(k = 0; k < 48; k++)
    {
        if(g_PCSegData[g_bPCSegNo].index != ctrlState[k])
        {
            g_PCSegData[g_bPCSegNo].end = 30 * k;
            g_bPCSegNo++;
            g_PCSegData[g_bPCSegNo].home = 30 * k;
            g_PCSegData[g_bPCSegNo].index = ctrlState[k];
        }
    }
    g_PCSegData[g_bPCSegNo].end = 30 * k;
    g_bPCSegNo++;

    for(i = 0; i < g_bPCSegNo; i++)
    {
        offset += sprintf(temp + offset, " (%d %d~%d, %d)", i + 1, g_PCSegData[i].home, g_PCSegData[i].end, 
                          g_PCSegData[i].index);
    }
    return g_bPCSegNo;
}

uint8 g_GuiCtrlGroup = 0;
uint8 ctrl_PageIndex;    //ʱ�οض�ҳ��ʾ���·�ҳ
uint8 ctrl_PCSegPageNum; //ʱ�ο�ҳ��

#if DESC("ʱ�οز���",1)
OOP_8103_CONFIG_T   g_GuiPeriodParam;           //0x81030200 ʱ�ο�
uint8 g_GuiPeriodPlan = 0;

void ctrl_period_param_display_init(DWORD dwPluginId)
{
    g_GuiCtrlGroup = 0;
    memset(&g_GuiPeriodParam, 0, sizeof(OOP_8103_CONFIG_T));
    read_db_noamal_data(g_hUdbCenter, 0x81030200, 0, 0, (uint8*)&g_GuiPeriodParam, sizeof(OOP_8103_CONFIG_T));
    appctrl_cal_period();
    ctrl_PageIndex = 1;
    
    g_bPCSegNo%4 ==0? ctrl_PCSegPageNum= g_bPCSegNo/4 : ctrl_PCSegPageNum = g_bPCSegNo/4 +1;
}

void ctrl_period_param_Total_draw(HDC hdc)
{
    char czBuf[100];
    char czBuf1[100];
    uint16 offset = 0;
    int nTop = 32;
    int nLeft = 1;
    int nLeft2 = 121;
    int tmpTop = 14;
    uint32 i = 0;
    OOP_CFGUNIT_8109_T* ptCfg = NULL;
    char* PlanName[] = { (char*)"һ", (char*)"��", (char*)"��" };
    int64 tempnum = 0;
    int64 tempnum1 = 0;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "ʱ�οز��� �ܼ���%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* �����Ƿ�������� */
    for (i = 0; i < g_GuiPeriodParam.nNum; i++)
    {
        if (0x2301 + g_GuiCtrlGroup == g_GuiPeriodParam.item[i].object)
        {
            ptCfg = &(g_GuiPeriodParam.item[i]);
            break;
        }
    }

    if (NULL == ptCfg)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "δ���÷���");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);

    nTop += tmpTop;
    if (0 == ((ptCfg->plan >> g_GuiPeriodPlan) & 0x01))
    {
        sprintf(czBuf, "��%s������Ч", PlanName[g_GuiPeriodPlan]);
        TextOut(hdc, nLeft, nTop, czBuf);
        return;
    }
    sprintf(czBuf, "��%s����", PlanName[g_GuiPeriodPlan]);
    TextOut(hdc, nLeft, nTop, czBuf);

    sprintf(czBuf, "����ϵ��  %d", ptCfg->floatCoe);
    TextOut(hdc, nLeft2, nTop, czBuf);
    char PowconTime[40];
    /* �������ʱ�ζ�ֵ */
    offset = 0;

    int64 defalueValue[48]={0};
    uint8 cnt = 0;
    for(uint32 i = 0;i < 48;i++)
    {  
        if(g_PCSegData[i].index != 0)
            defalueValue[i] = ptCfg->value[g_GuiPeriodPlan].defValue[cnt++];
    }

    
    for (i = 0+4*(ctrl_PageIndex-1); i < 4+4*(ctrl_PageIndex-1); i++)
     {
         if(i+1> g_bPCSegNo)
             break;
         
         if (g_PCSegData[i].index == 0)
             sprintf(PowconTime, "[%02d:%02d~%02d:%02d]", g_PCSegData[i].home/60, g_PCSegData[i].home%60, g_PCSegData[i].end/60, g_PCSegData[i].end%60);
         else
             sprintf(PowconTime, "[%02d:%02d~%02d:%02d]", g_PCSegData[i].home/60, g_PCSegData[i].home%60, g_PCSegData[i].end/60, g_PCSegData[i].end%60);
         if (/*((ptCfg->value[g_GuiPeriodPlan].valid >> i) & 0x01)&&*/(g_PCSegData[i].index != 0))
         {
             tempnum = defalueValue[i] / (10000L);
             tempnum1 = defalueValue[i] - tempnum * (10000L);
             if (defalueValue[i] > 90000000
                 || defalueValue[i] < -90000000)
             {
                 if (defalueValue[i] < 0)
                 {
                     tempnum1 = 0 - tempnum1;
                 }
                 valueDisplayProcess_strUnit(defalueValue[i], czBuf1, 100, 4, "W");
                 offset += sprintf(czBuf + offset, "%s:      ", PowconTime);
                 offset += sprintf(czBuf + offset, "%s", czBuf1);  
             }
             else
             {
                 if (defalueValue[i] < 0)
                 {
                     tempnum1 = 0 - tempnum1;
                 }
                 valueDisplayProcess_strUnit(defalueValue[i], czBuf1, 100, 4, "W");
                 offset += sprintf(czBuf + offset, "%s:      ", PowconTime);
                 offset += sprintf(czBuf + offset, "%s", czBuf1);                    
             }
         }
         else
         {
             offset += sprintf(czBuf + offset, "%s:      ----", PowconTime);
         }
         nTop += tmpTop;
         TextOut(hdc, 3, nTop, czBuf);
         offset = 0;
     }
     if(ctrl_PageIndex< ctrl_PCSegPageNum)
         sprintf(czBuf, "��һҳ");
     else
         sprintf(czBuf, "��һҳ");
    

//    if (ctrl_PageIndex == 0)
//    {
//        for (i = 0; i < 4; i++)
//        {
//            if (g_PCSegData[i].index == 0)
//                sprintf(PowconTime, "[%02d:%02d~%02d:%02d]", g_PCSegData[i].home/60, g_PCSegData[i].home%60, g_PCSegData[i].end/60, g_PCSegData[i].end%60);
//            else
//                sprintf(PowconTime, "[%02d:%02d~%02d:%02d]", g_PCSegData[i].home/60, g_PCSegData[i].home%60, g_PCSegData[i].end/60, g_PCSegData[i].end%60);
//            if (((ptCfg->value[g_GuiPeriodPlan].valid >> i) & 0x01)&&(g_PCSegData[i].index != 0))
//            {
//                tempnum = ptCfg->value[g_GuiPeriodPlan].defValue[i] / (10000L);
//                tempnum1 = ptCfg->value[g_GuiPeriodPlan].defValue[i] - tempnum * (10000L);
//                if (ptCfg->value[g_GuiPeriodPlan].defValue[i] > 90000000
//                    || ptCfg->value[g_GuiPeriodPlan].defValue[i] < -90000000)
//                {
//                    if (ptCfg->value[g_GuiPeriodPlan].defValue[i] < 0)
//                    {
//                        tempnum1 = 0 - tempnum1;
//                    }
//                    //offset += sprintf(czBuf + offset, "%s: %-8lld.%-1lld", PowconTime, tempnum, tempnum1 / 1000);
//                    valueDisplayProcess_strUnit(ptCfg->value[g_GuiPeriodPlan].defValue[i], czBuf1, 100, 4, "W");
//                    //double tempDefVal= tempnum+ tempnum1 *0.0001;
//                    //offset += sprintf(czBuf + offset, "%s:      %-9.4f", PowconTime, tempDefVal);
//                    offset += sprintf(czBuf + offset, "%s:      ", PowconTime);
//                    offset += sprintf(czBuf + offset, "%s", czBuf1);
//                }
//                else
//                {
//                    if (ptCfg->value[g_GuiPeriodPlan].defValue[i] < 0)
//                    {
//                        tempnum1 = 0 - tempnum1;
//                    }
//                    //offset += sprintf(czBuf + offset, "%s: %-5lld.%04lld", PowconTime, tempnum, tempnum1);
//                    valueDisplayProcess_strUnit(ptCfg->value[g_GuiPeriodPlan].defValue[i], czBuf1, 100, 4, "W");
//                    //double tempDefVal= tempnum+ tempnum1 *0.0001;
//                    //offset += sprintf(czBuf + offset, "%s:      %-9.4f", PowconTime, tempDefVal);
//                    offset += sprintf(czBuf + offset, "%s:      ", PowconTime);
//                    offset += sprintf(czBuf + offset, "%s", czBuf1);
//                }
//            }
//            else
//            {
//                offset += sprintf(czBuf + offset, "%s:      ----", PowconTime);
//            }
//            nTop += tmpTop;
//            TextOut(hdc, 3, nTop, czBuf);
//            offset = 0;
//        }
//        sprintf(czBuf, "��һҳ");
//    }
//    else{
//        for (i = 4; i < 8; i++)
//        {
//            if (g_PCSegData[i].index == 0)
//                sprintf(PowconTime, "[%02d:%02d~%02d:%02d]", g_PCSegData[i].home/60, g_PCSegData[i].home%60, g_PCSegData[i].end/60, g_PCSegData[i].end%60);
//            else
//                sprintf(PowconTime, "[%02d:%02d~%02d:%02d]", g_PCSegData[i].home/60, g_PCSegData[i].home%60, g_PCSegData[i].end/60, g_PCSegData[i].end%60);
//            if (((ptCfg->value[g_GuiPeriodPlan].valid >> i) & 0x01)&&(g_PCSegData[i].index != 0))
//            {
//                tempnum = ptCfg->value[g_GuiPeriodPlan].defValue[i] / (10000L);
//                tempnum1 = ptCfg->value[g_GuiPeriodPlan].defValue[i] - tempnum * (10000L);
//                if (ptCfg->value[g_GuiPeriodPlan].defValue[i] > 90000000
//                    || ptCfg->value[g_GuiPeriodPlan].defValue[i] < -90000000)
//                {
//                    if (ptCfg->value[g_GuiPeriodPlan].defValue[i] < 0)
//                    {
//                        tempnum1 = 0 - tempnum1;
//                    }       
//                    //offset += sprintf(czBuf + offset, "%s: %-8lld.%-1lld", PowconTime, tempnum, tempnum1 / 1000);
//                    valueDisplayProcess_strUnit(ptCfg->value[g_GuiPeriodPlan].defValue[i], czBuf1, 100, 4, "W");
//                    //double tempDefVal= tempnum+ tempnum1 *0.0001;
//                    //offset += sprintf(czBuf + offset, "%s:      %-9.4f", PowconTime, tempDefVal);
//                    offset += sprintf(czBuf + offset, "%s:      ", PowconTime);
//                    offset += sprintf(czBuf + offset, "%s", czBuf1);
//                }
//                else
//                {
//                    if (ptCfg->value[g_GuiPeriodPlan].defValue[i] < 0)
//                    {
//                        tempnum1 = 0 - tempnum1;
//                    }
//                    //offset += sprintf(czBuf + offset, "%s: %-5lld.%04lld", PowconTime, tempnum, tempnum1);
//                    valueDisplayProcess_strUnit(ptCfg->value[g_GuiPeriodPlan].defValue[i], czBuf1, 100, 4, "W");
//                    //double tempDefVal= tempnum+ tempnum1 *0.0001;
//                    //offset += sprintf(czBuf + offset, "%s:      %-9.4f", PowconTime, tempDefVal);
//                    offset += sprintf(czBuf + offset, "%s:      ", PowconTime);
//                    offset += sprintf(czBuf + offset, "%s", czBuf1);
//                }
//            }
//            else
//            {
//                offset += sprintf(czBuf + offset, "%s:      ----", PowconTime);
//            }
//            nTop += tmpTop;
//            TextOut(hdc, 3, nTop, czBuf);
//            offset = 0;
//        }
//        sprintf(czBuf, "��һҳ");
//    }
    ITEM_SELECT;
    TextOut(hdc, 200, 110, czBuf);
    ITEM_NOT_SELECT;
}

BOOL ctrl_period_Key_Msg(int nkey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
        
	switch (nkey)
	{
		case KEY_RIGHT:
		{
		    if(g_GuiPeriodPlan < 2)
		    {
		        g_GuiPeriodPlan++;
		        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
		    }
		    break;
		}
		case KEY_LEFT:
		{
    		if(g_GuiPeriodPlan > 0)
    	    {
    	        g_GuiPeriodPlan--;
    	        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    	    }
    	    break;
		}
		case KEY_DOWN:
		{
		    if(g_GuiCtrlGroup < 7)
		    {
		        g_GuiCtrlGroup++;
		        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
		    }
		    break;
		}
		case KEY_UP:
		{
    		if(g_GuiCtrlGroup > 0)
    	    {
    	        g_GuiCtrlGroup--;
    	        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    	    }
    	    break;
		}
		case KEY_OK:
		{
            
            ctrl_PageIndex++;
            if(ctrl_PageIndex>ctrl_PCSegPageNum)
                ctrl_PageIndex = 1;
            
//    		if(ctrl_PageIndex == 0)
//    	    {
//    	        ctrl_PageIndex = 1;
//
//    	    }else{
//			     ctrl_PageIndex = 0;
//			}
			UpdateWindow(pg_guicomm->hMainWnd, TRUE);
    	    break;
		}
		case KEY_CANCEL: CancelMenu(); break;
        default: break;
	}
	return TRUE;
}


//ʱ�οز���
PLUGIN_INFO g_layer_periodCtrl_param_display = {
    0x20100100/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ctrl_period_param_Total_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("���ݿز���",1)
OOP_8104_CONFIG_T   g_GuiRestParam;             //0x81040200 ���ݿ�
uint8 g_GuiRestParam_curPage=0;  //��ǰѡ���ҳ��,һ��2ҳ,ÿҳ4���ִ�
uint8 g_GuiRestParam_curPick = 0; //��ǰѡ�е��ִ�

void ctrl_rest_param_display_init(DWORD dwPluginId)
{
    g_GuiCtrlGroup = 0;
    memset(&g_GuiRestParam, 0, sizeof(OOP_8104_CONFIG_T));
    read_db_noamal_data(g_hUdbCenter, 0x81040200, 0, 0, (uint8*)&g_GuiRestParam, sizeof(OOP_8104_CONFIG_T));
}

void ctrl_rest_param_Total_draw(HDC hdc)
{
    char czBuf[100];
    int nTop = 32;
    int nLeft = 16;
    int tmpTop = 16;
    uint32 i = 0;
    OOP_CFGUNIT_810A_T* ptCfg = NULL;
    int64  tempnum = 0;
    char* PlanName[] = {(char*)"һ", (char*)"��", (char*)"��", (char*)"��", (char*)"��", (char*)"��", (char*)"��"};

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "���ݿز��� �ܼ���%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* �����Ƿ�������� */
    for (i = 0; i < g_GuiRestParam.nNum; i++)
    {
        if (0x2301 + g_GuiCtrlGroup == g_GuiRestParam.item[i].object)
        {
            ptCfg = &(g_GuiRestParam.item[i]);
            break;
        }
    }

    if (NULL == ptCfg)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "δ���÷���");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += 16;
    tempnum = ptCfg->defValue - ((ptCfg->defValue / 10L) * 10L);
    if (ptCfg->defValue < 0)
    {
        tempnum = 0 - tempnum;
    }

    sprintf(czBuf, "��ֵ   %13lld.%lld W", ptCfg->defValue / 10, tempnum);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "��ʼ�޵�ʱ��    %02d:%02d:%02d", ptCfg->startTime.hour, ptCfg->startTime.minute, ptCfg->startTime.second);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "�޵�����ʱ��    %d min", ptCfg->duration);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "ÿ���޵���");
    for (i = 1; i < 8; i++)
    {
        if ((ptCfg->powLimit.nValue >> i) & 0x01)
        {
            TextOut(hdc, 114 + 16 * (i - 1), nTop, PlanName[i - 1]);
        }
    }
}

BOOL ctrl_rest_Key_Msg(int nkey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
        
	switch (nkey)
	{
		case KEY_UP:
		{
			if(g_GuiCtrlGroup > 0)
			{
			    g_GuiCtrlGroup--;
			    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
			}
			break;
		}
		case KEY_DOWN:
		{
			if(g_GuiCtrlGroup < 7)
			{
			    g_GuiCtrlGroup++;
			    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
			}
			break;
		}
		case KEY_CANCEL:
			CancelMenu();
			break;
	}
	return TRUE;
}


//���ݿز���
PLUGIN_INFO g_layer_restCtrl_param_display = {
    0x20110100/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ctrl_rest_param_Total_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_rest_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("�µ�ز���",1)

OOP_8108_CONFIG_T   g_GuiMonthParam; 
OOP_HENERGYS_T  g_groupsMonthEnergy[8];

uint8 g_GuiMonth_curGroup = 0;

void ctrl_month_param_display_init(DWORD dwPluginId)
{
    memset(&g_GuiMonthParam, 0, sizeof(OOP_8108_CONFIG_T));
    read_db_noamal_data(g_hUdbCenter, 0x81080200, 0, 0, (uint8*)&g_GuiMonthParam, sizeof(OOP_8108_CONFIG_T));
    g_GuiCtrlGroup = 0;
}

void ctrl_month_param_Total_draw(HDC hdc)
{
    char czBuf[100];
    int nTop = 32;
    int nLeft = 24;
    int tmpTop = 20;
    uint32 i = 0;
    OOP_CFGUNIT_810D_T* ptCfg = NULL;
    int64  tempnum = 0;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "�µ�ز��� �ܼ���%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* �����Ƿ�������� */
    for (i = 0; i < g_GuiMonthParam.nNum; i++)
    {
        if (0x2301 + g_GuiCtrlGroup == g_GuiMonthParam.item[i].object)
        {
            ptCfg = &(g_GuiMonthParam.item[i]);
            break;
        }
    }

    if (NULL == ptCfg)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "δ���÷���");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += 16;
    tempnum = ptCfg->defValue - ((ptCfg->defValue / 10000L) * 10000L);
    if (ptCfg->defValue < 0)
    {
        tempnum = 0 - tempnum;
    }

    sprintf(czBuf, "��ֵ         %lld.%04lld kWh", ptCfg->defValue / 10000L, tempnum);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "����ϵ��      %d%%", ptCfg->limitCoe);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "����ϵ��      %d%%", ptCfg->floatCoe);
    TextOut(hdc, nLeft, nTop, czBuf);
}


BOOL ctrl_month_Key_Msg(int nkey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
        
	switch (nkey)
	{
		case KEY_UP:
		{
			if(g_GuiCtrlGroup > 0)
			{
			    g_GuiCtrlGroup--;
			    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
			}
			break;
		}
		case KEY_DOWN:
		{
			if(g_GuiCtrlGroup < 7)
			{
			    g_GuiCtrlGroup++;
			    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
			}
			break;
		}
		case KEY_CANCEL:
			CancelMenu();
			break;
	}
	return TRUE;
}

//�µ�ز���
PLUGIN_INFO g_layer_monthCtrl_param_display = {
    0x20240100/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ctrl_month_param_Total_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_month_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("�¸��ز���",1)

OOP_POWCON_LOW_T    g_GuiDownParam[OOP_MAX_TG]; //0x81067fff �¸���

void ctrl_down_param_display_init(DWORD dwPluginId)
{
	g_GuiCtrlGroup = 0;
}

void ctrl_down_param_Total_draw(HDC hdc)
{
    char czBuf[100];
    uint8 offset = 0;
    int nTop = 32;
    int nLeft = 6;
    int nLeft2= 126;
    int tmpTop = 14;
    uint8 i = 0;
    LCONSCHEME* ptCfg = NULL;
    char* turnName[] = { (char*)"һ��", (char*)"����", (char*)"����", (char*)"����",
                         (char*)"����", (char*)"����", (char*)"����", (char*)"����" };
    
    OOP_POWCON_LOW_T    tGuiDownParam; //0x81067fff �¸���
    memset(&tGuiDownParam, 0, sizeof(OOP_POWCON_LOW_T));
    read_db_noamal_data(g_hUdbCenter, 0x81067fff, 0, g_GuiCtrlGroup + 1, (uint8*)&tGuiDownParam, sizeof(OOP_POWCON_LOW_T));

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "�¸��ز��� �ܼ���%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* �����Ƿ�������� */
    if (0x2301 + g_GuiCtrlGroup != tGuiDownParam.object)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "δ���÷���");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    ptCfg = &(tGuiDownParam.scheme);

    nTop += tmpTop;
    sprintf(czBuf, "����ʱ��  %d min", ptCfg->slipTime);
    TextOut(hdc, nLeft, nTop, czBuf);

    sprintf(czBuf, "����ϵ��  %d%%", ptCfg->floatCoefficient);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "������ʱ  %d min", ptCfg->delayTime);
    TextOut(hdc, nLeft, nTop, czBuf);

    sprintf(czBuf, "����ʱ��  %d min", ptCfg->conTime * ((uint16)30));
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop + 6;
    sprintf(czBuf, "�澯ʱ��(��λmin):");
    TextOut(hdc, 1, nTop, czBuf);

    for (i = 0; i < 8; i++)
    {
        offset += sprintf(czBuf + offset, "%s:%-3d", turnName[i], ptCfg->waringTime[i]);
        if (3 == i % 4)
        {
            nTop += tmpTop;
            TextOut(hdc, 1, nTop, czBuf);
            offset = 0;
        }
        else
        {
            offset += sprintf(czBuf + offset, "  ");
        }
    }
    if(offset > 0)
    {
        nTop += tmpTop;
        TextOut(hdc, 1, nTop, czBuf);
    }
}


BOOL down_month_Key_Msg(int nkey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
        
	switch (nkey)
	{
		case KEY_UP:
		{
			if(g_GuiCtrlGroup > 0)
			{
			    g_GuiCtrlGroup--;
			    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
			}
			break;
		}
		case KEY_DOWN:
		{
			if(g_GuiCtrlGroup < 7)
			{
			    g_GuiCtrlGroup++;
			    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
			}
			break;
		}
		case KEY_CANCEL:
			CancelMenu();
			break;
	}
	return TRUE;
}



//�¸��ز���
PLUGIN_INFO g_layer_downCtrl_param_display = {
    0x20120100/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ctrl_down_param_Total_draw/*Draw*/,
    NULL/*Timer*/,
    down_month_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};


#endif



#if DESC("��ͣ�ز���",1)

OOP_8105_CONFIG_T   g_GuiStopParam;             //0x81050200 ��ͣ��

void ctrl_stop_param_display_init(DWORD dwPluginId)
{
    g_GuiCtrlGroup = 0;

    memset(&g_GuiStopParam, 0, sizeof(OOP_8105_CONFIG_T));
    read_db_noamal_data(g_hUdbCenter, 0x81050200, 0, 0, (uint8*)&g_GuiStopParam, sizeof(OOP_8105_CONFIG_T));
}

void ctrl_stop_param_Total_draw(HDC hdc)
{
    char czBuf[100];
    int nTop = 32;
    int nLeft = 5;
    int tmpTop = 16;
    uint32 i = 0;
    OOP_CFGUNIT_810B_T* ptCfg = NULL;
    int64  tempnum = 0;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "��ͣ�ز��� �ܼ���%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* �����Ƿ�������� */
    for (i = 0; i < g_GuiStopParam.nNum; i++)
    {
        if (0x2301 + g_GuiCtrlGroup == g_GuiStopParam.item[i].object)
        {
            ptCfg = &(g_GuiStopParam.item[i]);
            break;
        }
    }

    if (NULL == ptCfg)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "δ���÷���");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += 16;
    tempnum = ptCfg->defValue - ((ptCfg->defValue / 10L) * 10L);
    if (ptCfg->defValue < 0)
    {
        tempnum = 0 - tempnum;
    }

    sprintf(czBuf, "��ֵ   %13lld.%lld W", ptCfg->defValue / 10, tempnum);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "��ʼʱ��    %04d/%02d/%02d", ptCfg->startTime.year, ptCfg->startTime.month, ptCfg->startTime.day);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "����ʱ��    %04d/%02d/%02d", ptCfg->endTime.year, ptCfg->endTime.month, ptCfg->endTime.day);
    TextOut(hdc, nLeft, nTop, czBuf);
}

BOOL ctrl_stop_Key_Msg(int nkey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
        
    switch (nkey)
    {
        case KEY_UP:
        {
            if(g_GuiCtrlGroup > 0)
            {
                g_GuiCtrlGroup--;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            }
            break;
        }
        case KEY_DOWN:
        {
            if(g_GuiCtrlGroup < 7)
            {
                g_GuiCtrlGroup++;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            }
            break;
        }
        case KEY_CANCEL:
            CancelMenu();
            break;
    }
    return TRUE;
}

//��ͣ�ز���
PLUGIN_INFO g_layer_stopCtrl_param_display = {
    0x20290100/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ctrl_stop_param_Total_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_stop_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};

#endif



#if DESC("3Kֵ",1)

OOP_METER_T g_MeterT[16];            //60010200 ���ܱ���
  
uint32 g_3kCurPage = 0;

//3kֵΪ60010203.��ѹ�������������
void kivp_display_init(DWORD dwPluginId)
{
    //g_meterCurPage = (dwPluginId) & 0x0F;
    //appdb_meter_list_record_init(g_hUdbCenter);
    /*set_gui_infonum(g_3kCurPage + 1);

    memset(&g_MeterT, 0, sizeof(OOP_METER_T));
    read_db_noamal_data(g_hUdbCenter, 0x60000200, 0, g_3kCurPage*4 +1, (uint8*)&g_MeterT[g_3kCurPage*4], sizeof(OOP_METER_T));

	read_db_noamal_data(g_hUdbCenter, 0x60000200, 0, g_3kCurPage*4+2, (uint8*)&g_MeterT[g_3kCurPage*4+1], sizeof(OOP_METER_T));

	read_db_noamal_data(g_hUdbCenter, 0x60000200, 0, g_3kCurPage*4+3, (uint8*)&g_MeterT[g_3kCurPage*4+2], sizeof(OOP_METER_T));

	read_db_noamal_data(g_hUdbCenter, 0x60000200, 0, g_3kCurPage*4+4, (uint8*)&g_MeterT[g_3kCurPage*4+3], sizeof(OOP_METER_T));
    */
    memset(&meterList,0x00,sizeof(DESK_METER_LIST_T));
    meter_info_read_all(&meterList);
}

void kivp_plate_draw(HDC hdc)
{
	char czBuf[100];

	SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;
	bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "������");
   	TextOut(hdc, 2, 20, czBuf);

	bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "Ki");
   	TextOut(hdc, 55, 20, czBuf);

	bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "Kv");
   	TextOut(hdc, 115, 20, czBuf);
	
	bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "Kp");
   	TextOut(hdc, 175, 20, czBuf);

}

void kivp_Total_draw(HDC hdc)
{
	//kivp_display_init(0x20130000);
	
	char czBuf[100];

	kivp_plate_draw(hdc);
	if (meterList.meterNum == 0)
        TextOut(hdc, 60, 50, "�޵���¼");
	SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

	//�ж��Ƿ�������,�ж�����:g_MeterT[i].nIndex == 0
	if(4*g_3kCurPage < meterList.meterNum){
        for(int i =0;i<4;i++)
        {
			if (meterList.meterInfo[g_3kCurPage*4+i].pn != 0){
        		bzero(czBuf,sizeof(czBuf));
        		sprintf(czBuf,"%d.",meterList.meterInfo[g_3kCurPage*4+i].pn);
        		TextOut(hdc,20, 40+20*i, czBuf);
        		bzero(czBuf,sizeof(czBuf));
        		sprintf(czBuf, "%hu", meterList.meterInfo[g_3kCurPage*4+i].CT);
        		TextOut(hdc, 55, 40+20*i, czBuf);
                sprintf(czBuf, "%hu", meterList.meterInfo[g_3kCurPage*4+i].PT);
        		TextOut(hdc, 115, 40+20*i, czBuf);
                sprintf(czBuf, "%u", (meterList.meterInfo[g_3kCurPage*4+i].PT)*(meterList.meterInfo[g_3kCurPage*4+i].CT));
        		TextOut(hdc, 175, 40+20*i, czBuf);
                
        	}
		}
	}
}

BOOL kivp_Key_Msg(int nkey)
{
	uint32 curPageMax;
	if (meterList.meterNum%4 == 0)
		curPageMax = meterList.meterNum/4 - 1;
	else
		curPageMax = meterList.meterNum/4;
	switch (nkey)
	{
		case KEY_DOWN:
			g_3kCurPage>=curPageMax? g_3kCurPage=0: ++g_3kCurPage;
			break;

		case KEY_UP:
			g_3kCurPage<=0?g_3kCurPage=curPageMax : --g_3kCurPage;
			break;
			
		case KEY_CANCEL:
			g_3kCurPage = 0;
			CancelMenu();
			break;
	}
	return TRUE;

}


//3kֵ
PLUGIN_INFO g_layer_kivp_display = {
    0x20130000/*id*/,
    kivp_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    kivp_Total_draw/*Draw*/,
    NULL/*Timer*/,
    kivp_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("�������",1)

OOP_PTCT_T  sdata[8];

void kivp_puls_display_init(DWORD dwPluginId)
{

    memset(sdata, 0, sizeof(OOP_PTCT_T)*8);
    read_db_noamal_data(g_hUdbCenter, 0x24010300, 0, 0, (uint8*)&sdata[0], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24020300, 0, 0, (uint8*)&sdata[1], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24030300, 0, 0, (uint8*)&sdata[2], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24040300, 0, 0, (uint8*)&sdata[3], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24050300, 0, 0, (uint8*)&sdata[4], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24060300, 0, 0, (uint8*)&sdata[5], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24070300, 0, 0, (uint8*)&sdata[6], sizeof(OOP_PTCT_T));

    read_db_noamal_data(g_hUdbCenter, 0x24080300, 0, 0, (uint8*)&sdata[7], sizeof(OOP_PTCT_T));

}

void kivp_puls_plate_draw(HDC hdc)
{
    char czBuf[100];

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;
    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "����");
   	TextOut(hdc, 2, 20, czBuf);

    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "Ki");
   	TextOut(hdc, 55, 20, czBuf);

    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "Kv");
   	TextOut(hdc, 115, 20, czBuf);
    
    bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "Kp");
   	TextOut(hdc, 175, 20, czBuf);

}

void kivp_puls_Total_draw(HDC hdc)
{

    char czBuf[100];

    kivp_puls_plate_draw(hdc);
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    for(int i =0;i<4;i++)
    {
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,"��%d·",i+1+4*g_3kCurPage);
        TextOut(hdc,5, 40+20*i, czBuf);
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf, "%hu", sdata[g_3kCurPage*4+i].ct);
        TextOut(hdc, 55, 40+20*i, czBuf);
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf, "%hu", sdata[g_3kCurPage*4+i].pt);
        TextOut(hdc, 115, 40+20*i, czBuf);
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf, "%u", (sdata[g_3kCurPage*4+i].pt)*(sdata[g_3kCurPage*4+i].ct));
        TextOut(hdc, 175, 40+20*i, czBuf);

    }
}

BOOL kivp_puls_Key_Msg(int nkey)
{
	uint32 curPageMax = 1;
	switch (nkey)
	{
		case KEY_DOWN:
			g_3kCurPage>=curPageMax? g_3kCurPage=0: ++g_3kCurPage;
			break;

		case KEY_UP:
			g_3kCurPage<=0?g_3kCurPage=curPageMax : --g_3kCurPage;
			break;
			
		case KEY_CANCEL:
			g_3kCurPage = 0;
			CancelMenu();
			break;
	}
	return TRUE;

}


PLUGIN_INFO g_layer_kivp_puls_display = {
    0x20130001/*id*/,
    kivp_puls_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    kivp_puls_Total_draw/*Draw*/,
    NULL/*Timer*/,
    kivp_puls_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("������",1)
//OOP_METER_T g_MeterT[16];            //60010200 ���ܱ���
uint32 g_meterCurPage = 0;
void meterParam_display_init(DWORD dwPluginId)
{
   // g_meterCurPage = (dwPluginId) & 0x0F;
//    set_gui_infonum(g_meterCurPage + 1);
	//int i = 0;
    g_meterCurPage = 0;
    appdb_meter_list_record_init(g_hUdbCenter);
    /*memset(&g_MeterT, 0, sizeof(OOP_METER_T)*16);
	for(i=1; i<=16; i++)
	{
		read_db_noamal_data(g_hUdbCenter, 0x60000200, 0, i, (uint8*)&g_MeterT[i-1], sizeof(OOP_METER_T));
	}*/
}

void meterParam_plate_draw(HDC hdc)
{
	char czBuf[100];

	SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

	bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "�ֱ��");
   	TextOut(hdc, 10, 20, czBuf);

	bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "����");
   	TextOut(hdc, 80, 20, czBuf);

	bzero(czBuf,sizeof(czBuf));
    sprintf(czBuf, "���ַ");
   	TextOut(hdc, 150, 20, czBuf);
}

void meterParam_Total_draw(HDC hdc)
{
//	meterParam_display_init(0x20140000);
		
	meterParam_plate_draw(hdc);

	char czBuf[100];
	int i = 0;
    if (g_gruop_oopMeter.nNum == 0)
        TextOut(hdc, 60, 50, "�޵���¼");
	SelectFont(hdc, g_guiComm.fontSmall);
	if(g_meterCurPage*4 < g_gruop_oopMeter.nNum){
	    for(i =0;i<4;i++)
	    {   
	        if (g_gruop_oopMeter.oopMeter[g_meterCurPage*4+i].nIndex != 0){
    			sprintf(czBuf, "%hu", g_gruop_oopMeter.nValue[g_meterCurPage*4+i]);
    			TextOut(hdc, 30, 50+20*i, czBuf);
    			uint8 prtl = g_gruop_oopMeter.oopMeter[g_meterCurPage*4+i].basic.protocol;
    	        if (prtl > 5)
    	            prtl = 0;
    			sprintf(czBuf, "%s", g_prtl[prtl]);
    			TextOut(hdc, 60, 50+20*i, czBuf);
    
    			uint8 Addr[6] = { 0 };
    	        memcpy(Addr, g_gruop_oopMeter.oopMeter[g_meterCurPage*4+i].basic.tsa.add, 6);
    			bzero(czBuf,sizeof(czBuf));
    			sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
    			TextOut(hdc, 150, 50+20*i, czBuf);
		    }
	    }
	}
}

BOOL meterParam_Key_Msg(int nkey)
{
	uint32 curPageMax;
	if (g_gruop_oopMeter.nNum%4 == 0)
		curPageMax = g_gruop_oopMeter.nNum/4 - 1;
	else
		curPageMax = g_gruop_oopMeter.nNum/4;
	switch (nkey)
	{
		case KEY_DOWN:
			g_meterCurPage>=curPageMax? g_meterCurPage=0: ++g_meterCurPage;
			break;

		case KEY_UP:
			g_meterCurPage<=0?g_meterCurPage=curPageMax : --g_meterCurPage;
			break;
			
		case KEY_CANCEL:
			g_meterCurPage = 0;
			CancelMenu();
			break;
	}
	return TRUE;

}


//������
PLUGIN_INFO g_layer_meterParam_display = {
    0x20140000/*id*/,
    meterParam_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    meterParam_Total_draw/*Draw*/,
    NULL/*Timer*/,
    meterParam_Key_Msg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("��������",1)

int64 curPows[96];


int db_readRecord(DB_CLIENT fd, READ_RECORD_T *ptRecord, uint32 recordLen)
{
	int        ret = 0;   //DBMSG_SUCCESS=0;
    READ_RECORD_T      inReadRecordTmp;
    STEP_READ_RECORD_T stepReadRecord;
    uint32 handle;
    uint32 recNum;
    int32  nRet = 0;
    RECORD_UNIT_T       recordOut;
    memset(&recordOut, 0, sizeof(RECORD_UNIT_T));
    if(fd <= 0 || NULL == ptRecord)
    {
        return 0x2000; //DBMSG_ERR_INPUT = 0x2000;
    }

    if (recordLen > DATA_LEN_MAX)
    {
        return ERR_O_RANGE;
    }
    /* ֻ�����һ�� */
    inReadRecordTmp = *ptRecord;
    inReadRecordTmp.sortType = DOWN_SORT;

    ret = db_read_record_start(fd, &inReadRecordTmp, &handle, &recNum);
    if (0 != ret)
    {
        return (0x10000000 + ret);
    }
    if(0 == recNum)
    {
        return ERR_NORESOURCE;
    }
    
    memcpy(&stepReadRecord.MAC, &inReadRecordTmp.MAC, sizeof(OOP_OCTETVAR64_T));

	uint32 cnt = 0,index = 0;
	uint32 lastRecordTime;
	//�ȶ�һ��
	ret = db_read_record_step(fd, handle, &stepReadRecord, &recordOut);
    if (0 != ret)
    {
    	return ret;
    }
    
    index = (recordOut.colEndTm - inReadRecordTmp.cStart) / 900;
    memcpy(&curPows[index], recordOut.record[0].outDataBuf, recordLen);
	lastRecordTime = recordOut.colEndTm;
    cnt ++;

	while((cnt < 96)&&(cnt < recNum))
	{
		ret = db_read_record_step(fd, handle, &stepReadRecord, &recordOut);
    	if (0 != ret)
    	{
    	    return ret;
    	}
		//�жϸü�¼��ʱ�������һ���Ƿ���ڵ���15����,�����,��ô��¼������;���������
		if((lastRecordTime - recordOut.colEndTm) > 14 * 60)
		{
            //�����ȡ�����������������ĸ����
            index = (recordOut.colEndTm - inReadRecordTmp.cStart) / 900;
			memcpy(&curPows[index], recordOut.record[0].outDataBuf, recordLen);
	        lastRecordTime = recordOut.colEndTm;
            cnt ++;
		}
	}

    /*��������¼*/
    ret = db_read_record_end(fd, handle);
    if (0 != ret)
    {
        return (0x20000000 + ret);
    }

    return nRet;

}

void power_curve_TimeSetToday(time_t* tStart,time_t* tEnd)
{
	time_t timp;
	struct tm *tmStart;
	struct tm *tmEnd;

	time(&timp);

	//����ǰʱ��ת��һ��
	tmEnd = localtime(&timp);
	
	tmStart =(tm*)malloc(sizeof(tm));
	memcpy(tmStart,tmEnd,sizeof(tm));

	//��tmEnd��hour��min��sec�޸�
	tmStart->tm_hour = 0;
	tmStart->tm_min = 0;
	tmStart->tm_sec = 0;
	*tStart =  mktime(tmStart);
    
    tmEnd->tm_hour = 23;
    tmEnd->tm_min = 59;
	tmEnd->tm_sec = 59;
	*tEnd = mktime(tmEnd);

	free(tmStart);

}


void power_curve_TimeSet(time_t* tStart,time_t* tEnd)
{
	//���ǲ���һ��ʱ��ĺ���
	time_t timp;
	struct tm *tmStart;
	struct tm *tmEnd;

	//��ȡ��ǰʱ��Ĵ�ֵ
	time(&timp);

	//��ȡ����ͬһʱ�̵Ĵ�ֵ
	timp = timp - 60*60*24;

	//������ͬһʱ�̵Ĵ�ֵת���ɽṹ	
	tmStart = localtime(&timp);
	
	tmEnd =(tm*)malloc(sizeof(tm));
	memcpy(tmEnd,tmStart,sizeof(tm));

	//������ͬһʱ�̵Ľṹ��hour��min��sec�޸�Ϊ0
	tmStart->tm_hour = 0;
	tmStart->tm_min = 0;
	tmStart->tm_sec = 0;

	//�õ�tmEnd
	//memcpy(tmEnd,tmStart,sizeof(tm));

	//��tmEnd��hour��min��sec�޸�
	tmEnd->tm_hour = 23;
	tmEnd->tm_min = 59;
	tmEnd->tm_sec = 59;

	*tStart =  mktime(tmStart);
	*tEnd = mktime(tmEnd);
	
	//bzero(czBuf,sizeof(czBuf));
	//sprintf(czBuf, "%d,%d,%d,%d,%d",tmStart->tm_mon+1,tmStart->tm_mday,tmStart->tm_hour,tmStart->tm_min,tmStart->tm_sec);
	//TextOut(hdc, 50, 50, czBuf);	

	//bzero(czBuf,sizeof(czBuf));
	//sprintf(czBuf, "%d,%d,%d,%d,%d",tmEnd->tm_mon+1,tmEnd->tm_mday,tmEnd->tm_hour,tmEnd->tm_min,tmEnd->tm_sec);
	//TextOut(hdc, 50, 80, czBuf);

	free(tmEnd);

}


int readCurveData(DB_CLIENT fp, uint32 oad_value, uint8 logic_id, uint16 info_num,uint8* pbuf,uint32 buf_len)
{
	//
	int                ret = ERR_NORMAL;
    uint32             len     = 0;
    //time_t             timep = 0;

    READ_RECORD_T      inReadRecord;
    OOP_METER_T        oopMeter;
    NOMAL_OAD_T        nomalOAD;
    OOP_OCTETVAR16_T   m_tmnAddr;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&oopMeter, 0, sizeof(OOP_METER_T));

//    time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep
//    timep +=28800;
	//���ߵĿ�ʼʱ��ͽ���ʱ��
	//��ʼʱ��Ϊ���쵽�����24Сʱ������
	time_t start,end;
	//power_curve_TimeSet(&start,&end);

    //���ս���0��0�ֿ�ʼ
    power_curve_TimeSetToday(&start,&end);

    inReadRecord.recordOAD.logicId = logic_id;
    inReadRecord.recordOAD.infoNum = info_num;
    inReadRecord.cType = COL_TM_STORE;  //����ʱ������Ϊ�ɼ��洢ʱ��
    inReadRecord.cStart = start + 28800;
    inReadRecord.cEnd = end + 28800;
	//inReadRecord.cType = COL_TM_STORE;
    //inReadRecord.cStart = timep - 87300;
    //inReadRecord.cEnd = timep;

    inReadRecord.sortType = DOWN_SORT;
    //inReadRecord.recordOAD.optional = 0;  //��OAD����50020200
    inReadRecord.recordOAD.optional = 1;
	inReadRecord.recordOAD.road.oadMain.value = 0x50020200;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad_value;

    inReadRecord.MAC.nNum = 6;

    if(info_num == 0)
    {
        memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
        nomalOAD.logicId = logic_id;
        nomalOAD.oad.value = 0x40010200;
        ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&m_tmnAddr, &len);
        if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
        {
            GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
            return -1;
        }
         memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);
    }
    else
    {
        if (app_get_meter_list(info_num, &oopMeter) != ERR_OK)
        {
            GUI_FMT_DEBUG(" app_get_meter_list %d !\n", info_num);
            return -1;
        }
        memcpy(inReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
    }

	ret = db_readRecord(fp, &inReadRecord, buf_len);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG(" db_read_record %08x ret=%d !\n", oad_value, ret);
        return -1;
    }
    //memcpy(pbuf, stepRecordOut.record[0].outDataBuf, buf_len);

    return 0;

}

uint8 groupCurveIndex = 0;
uint8 CurveLastIndex = 0;
void power_curve_display_init(DWORD dwPluginId)
{
    //stopCtrlGroup = (dwPluginId) & 0x0F;
    //set_gui_infonum(stopCtrlGroup + 1);

    //memset(&g_GuiStopParam, 0, sizeof(OOP_8105_CONFIG_T));
    //read_db_noamal_data(g_hUdbCenter, 0x81050200, 0, 0, (uint8*)&g_GuiStopParam, sizeof(OOP_8105_CONFIG_T));
    //appdb_meter_list_record_init(g_hUdbCenter);
    //memset(&curPows, 0 , sizeof(OOP_INT4V_T)*96);
    //readCurveData(g_hUdbCenter,0x20040200,0,g_gruop_oopMeter.nValue[g_guiComm.nCurPoint],(uint8*)&curPows,sizeof(OOP_INT4V_T));
    uint32 oadvalue;
    memset(curPows, 0 , sizeof(int64)*96);
    oadvalue = 0x23010300 + (0x00010000) *groupCurveIndex;
    readCurveData(g_hUdbCenter,oadvalue,0,0,(uint8*)&curPows,sizeof(int64));
    CurveLastIndex = groupCurveIndex;
}

void power_curve_display_plate_draw(HDC hdc)
{
	ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontBig);
	
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
	
//	char czBuf[100];
//	sprintf(czBuf, "�ܼ���%2d %s����������", g_guiComm.nCurPoint,
//		(g_guiComm.nIndexPoint==1)?"��":"��");

	rc.top = 18;
    rc.bottom = rc.top+16;
//    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

	// ������
	int nLeft = 10;
	int nTop = 34;
	int nBottom = 110;
	int nRight = 220;
	
	MoveTo(hdc, nLeft-1, nTop-1);
    LineTo(hdc, nLeft-3, nTop+8);
	MoveTo(hdc, nLeft-1, nTop-1);
    LineTo(hdc, nLeft+1, nTop+8);
	
	MoveTo(hdc, nRight+1, nBottom+1);
    LineTo(hdc, nRight-8, nBottom-1);
    LineTo(hdc, nRight+1, nBottom+1);
    LineTo(hdc, nRight-8, nBottom+3);

	MoveTo(hdc, nLeft-1, nTop-1);
	LineTo(hdc, nLeft-1, nBottom+1);
	LineTo(hdc, nRight+1, nBottom+1);

	double dSetp = (double)(nRight-nLeft-10)/96;	// 24����
	// ��x��ʱ��
	int nTmpTop = nBottom+2;
	// 0��
	int nTmpLeft = nLeft;
	TextOut(hdc, nTmpLeft-3, nTmpTop, "0");

	// 5��
	nTmpLeft = (int)(dSetp*20)+nLeft;
	TextOut(hdc, nTmpLeft-3, nTmpTop, "5");
	MoveTo(hdc, nTmpLeft, nTmpTop);
	LineTo(hdc, nTmpLeft, nTmpTop-3);

	// 10��
	nTmpLeft = (int)(dSetp*40)+nLeft;
	TextOut(hdc, nTmpLeft-6, nTmpTop, "10");
	MoveTo(hdc, nTmpLeft, nTmpTop);
	LineTo(hdc, nTmpLeft, nTmpTop-3);

	// 15��
	nTmpLeft = (int)(dSetp*60)+nLeft;
	TextOut(hdc, nTmpLeft-6, nTmpTop, "15");
	MoveTo(hdc, nTmpLeft, nTmpTop);
	LineTo(hdc, nTmpLeft, nTmpTop-3);

	// 20��
	nTmpLeft = (int)(dSetp*80)+nLeft;
	TextOut(hdc, nTmpLeft-6, nTmpTop, "20");
	MoveTo(hdc, nTmpLeft, nTmpTop);
	LineTo(hdc, nTmpLeft, nTmpTop-3);

	TextOut(hdc,224, nTmpTop, "h");
}

void power_curve_Totaldraw(HDC hdc)
{
	//power_curve_display_init(0x20020100);

	power_curve_display_plate_draw(hdc);
    //�ܼ���ű仯ʱ��ˢ������
    if(CurveLastIndex != groupCurveIndex)
    {
        uint32 oadvalue;
        memset(curPows, 0 , sizeof(int64)*96);
        oadvalue = 0x23010300 + (0x00010000) *groupCurveIndex;
        readCurveData(g_hUdbCenter,oadvalue,0,0,(uint8*)&curPows,sizeof(int64));
        CurveLastIndex = groupCurveIndex;
    }

	char czBuf[100];
    SelectFont(hdc, g_guiComm.fontSmall);
	TextOut(hdc, 85, 20, "�ܼ���");
    sprintf(czBuf, "%d", groupCurveIndex + 1);
    //set_gui_infonum((uint16)g_gruop_oopMeter.nValue[g_guiComm.nCurPoint]);
    ITEM_SELECT;
    TextOut(hdc, 130, 20, czBuf);
    ITEM_NOT_SELECT;
	//ͨ������ķ���,��
	//bzero(czBuf,sizeof(czBuf));
    //sprintf(czBuf,"%d  %d  %d  %d",curPows[5].nValue[0],curPows[5].nValue[1],
    //								curPows[5].nValue[2],curPows[5].nValue[3]);
   	//TextOut(hdc, 20, 20, czBuf);

	//ͨ������ķ���,�ɹ������е�96������ȫ��ץ������

	//���ȱ���96������,������е����ֵ
	int32 powMax = 0;
	for(uint32 i =0;i<sizeof(curPows)/sizeof(curPows[0]);i++)
	{
		if(curPows[i]> powMax)
			powMax = curPows[i];
	}

    //���������ֵ��Ӧ�ñ�Ĭ��
    MEMZERO(czBuf, sizeof(czBuf));
    if(powMax < 10000)
    {
        sprintf(czBuf, "%.1f W", (double)powMax/10); //�����W 
    }
    else
    {
        sprintf(czBuf, "%.2f kW", (double)powMax / 10000); //�����KW 
    }
    TextOut(hdc, 4, 17, czBuf);

	// ����96����,����������һ��
	int nLeft = 10;
	int nTop = 40;
	int nBottom = 110;
	int nRight = 210;
	
	double dSetp = (double)(nRight-nLeft)/96;	// 96����
	int nHeight = nBottom-nTop;
	BOOL bMoveTo = FALSE;
	int nVal;
	for(int i = 0; i < 96; i ++)
	{
        double rate = 0;
        rate = (double)curPows[i]/powMax;
        if (rate >= 1)
            rate = 1;
		int nTmpTop = nBottom - (int)(rate * nHeight);
		//int nTmpTop = nBottom - (int)(((double)curPows[i]/powMax)*nHeight);
		nVal = nLeft+(int)(i*dSetp);
		if(bMoveTo == FALSE)
		{
			MoveTo(hdc, nVal, nTmpTop);
			// ���ǵ��п���ֻ��һ���¶��ĵ�,����ʹ�øú������õ��ֵ
			SetPixel(hdc, nVal, nTmpTop, PIXEL_black);
			bMoveTo = TRUE;
		}
		else
		{
			SetPixel(hdc, nVal, nTmpTop, PIXEL_black);
			LineTo(hdc, nVal, nTmpTop);
		}
	}
}

void pointReturn(char* pNewStr, void* pPara)
{
    uint32 nPoint = atoi(pNewStr);
    /*bool flag = false;
    if(nPoint >= VIR_BEGIN)
    {
        char bzbuf[100];
        sprintf(bzbuf, "���������Ϊ%d\nȷ���Ƿ���������?", (VIR_BEGIN-1));
        if(TRUE == MessageBox(bzbuf, MB_OKCANCEL))
        {
            sprintf(bzbuf, "%d", g_gruop_oopMeter.nValue[g_guiComm.nCurPoint]);
            InputBox(bzbuf, pointReturn, NULL, 0x01);
        }
    }else{
        for (uint32 i = 0; i < g_gruop_oopMeter.nNum; i++){
            if(g_gruop_oopMeter.nValue[i] == nPoint){
                g_guiComm.nCurPoint = i;
                flag = true;
                g_gruop_oopMeter.nValue[g_guiComm.nCurPoint] = nPoint;
            }
        }
        if (flag == false){
            char bzbuf[100];
            sprintf(bzbuf, "%d���ڵ�����\nȷ���Ƿ���������?", nPoint);
            if(TRUE == MessageBox(bzbuf, MB_OKCANCEL))
            {
                sprintf(bzbuf, "%d", g_gruop_oopMeter.nValue[g_guiComm.nCurPoint]);
                InputBox(bzbuf, pointReturn, NULL, 0x01);
            }
       }
    }*/
    if(nPoint > CtrlGrp_ROUTE)
    {
        char bzbuf[100];
        sprintf(bzbuf, "�ܼ������Ϊ%d\nȷ���Ƿ���������?", CtrlGrp_ROUTE);
        if(TRUE == MessageBox(bzbuf, MB_OKCANCEL))
        {
            sprintf(bzbuf, "%d", groupCurveIndex + 1);
            InputBox(bzbuf, pointReturn, NULL, 0x01);
        }
    }
    else{
        groupCurveIndex = nPoint - 1;
    }
}

BOOL powerCurve_Key_Msg(int nkey)
{
    switch (nkey)
    {
        case KEY_CANCEL:
            groupCurveIndex = 0;
            CancelMenu();
            break;
        case KEY_UP:
        case KEY_LEFT:
            if (groupCurveIndex == 0)
                groupCurveIndex = 7;
            else
                groupCurveIndex--;
            power_curve_display_init(0x20020100);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case KEY_DOWN:
        case KEY_RIGHT:
            if (groupCurveIndex == 7)
                groupCurveIndex = 0;
            else
                groupCurveIndex++;
            power_curve_display_init(0x20020100);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case KEY_OK:// ȷ��ʱ����
            char bzbuf[50];
            sprintf(bzbuf, "%d", groupCurveIndex + 1);
            InputBox(bzbuf, pointReturn, NULL, 0x01);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
    }
    return TRUE;
}


//��������
PLUGIN_INFO g_layer_powerCurve_display = {
    0x20020100/*id*/,
    power_curve_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    power_curve_Totaldraw/*Draw*/,
    NULL/*Timer*/,
    powerCurve_Key_Msg/*keymsg*/,
    NULL/*ipc*/
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
    int nLeft = 72;

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

#if DESC("ϵͳ�汾��ʾ",1)
void sys_ver_Init(DWORD dwPluginId)
{

}

int get_sys_ver(char * temp)
{
    char buf[1024] = {0};

    if(temp == NULL)
        return -1;
    
    FILE* pipe = fopen("/proc/version", "r");  
    if (!pipe)  
    {
        pipe = fopen("/proc/version", "r");
    }
    if (!pipe)
    {
        GUI_FMT_DEBUG("��ѯϵͳ�汾ʧ��, /proc/version��\n");
        return -1;
    }

    fseek(pipe, -1024L, SEEK_END);
    while (fgets(buf, 1024, pipe) != NULL)
    {
        GUI_FMT_TRACE("%s", buf);
        char *pfile = NULL;
        if ((pfile = strstr(buf,"+++OS_RK3358")) != NULL)
        {
            if (strlen(pfile) < 256)
            {
                strcpy(temp, pfile);
            }
            else
            {
                GUI_FMT_DEBUG("��ѯϵͳ�汾ʧ��, �������len[%d]\n", strlen(pfile));
            }
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

void sys_ver_Draw(HDC hdc)
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
	strcpy(pcz, "ϵͳ��ǰ�汾");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);
    
    int nTop = 80;
    int nLeft = 52;

    memset(pcz, 0, 40);
    if (get_sys_ver(pcz) == 0)
    {
        TextOut(hdc, nLeft, nTop, pcz);
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "-----");
    }

    return;
}


PLUGIN_INFO g_layer_sys_ver=
{
    0x00A000FF, // 0x00A000FF
    sys_ver_Init,
    NULL,
    sys_ver_Draw,
    NULL,
    NULL,
    NULL
};

#endif

#if DESC("��Կ�汾��ʾ",1)

TESAMInfo_t tESAMInfoBuff;

void symKey_Init(HDC hdc)
{
    memset(&tESAMInfoBuff, 0, sizeof(TESAMInfo_t));
    read_pdata_xram("/data/app/stAmr/cfg/tESAMInfo", (char*)&tESAMInfoBuff, 0, sizeof(TESAMInfo_t));

}

void symKey_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
       return;

    SelectFont(hdc,pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;
    int esamFlag = 0;
    int nTop = 20;
    int nLeft = 10;
    char pcz[150];
    for (int i = 0;i < 16; i++){
        if(tESAMInfoBuff.symKeyVersion[i] != 0){
            esamFlag = 1;
            break;
        }
    }
    bzero(pcz,sizeof(pcz));
    if (esamFlag == 0)
        TextOut(hdc, nLeft, nTop, "��Կ״̬  ����");
    else
        TextOut(hdc, nLeft, nTop, "��Կ״̬  ��ʽ");
    nTop += 18;
    TextOut(hdc, nLeft, nTop, "ESAM���к�");

    nTop += 18;
    bzero(pcz,sizeof(pcz));
    sprintf(pcz, "%02X%02X%02X%02X%02X%02X%02X%02X", tESAMInfoBuff.esamSerial[0], tESAMInfoBuff.esamSerial[1], tESAMInfoBuff.esamSerial[2],
    tESAMInfoBuff.esamSerial[3], tESAMInfoBuff.esamSerial[4], tESAMInfoBuff.esamSerial[5], tESAMInfoBuff.esamSerial[6], tESAMInfoBuff.esamSerial[7]);
    TextOut(hdc, nLeft, nTop, pcz);

    nTop += 18;
    bzero(pcz,sizeof(pcz));
    sprintf(pcz, "ESAM�汾�� %02X%02X%02X%02X", tESAMInfoBuff.esamVersion[0], tESAMInfoBuff.esamVersion[1], tESAMInfoBuff.esamVersion[2], tESAMInfoBuff.esamVersion[3]);
    TextOut(hdc, nLeft, nTop, pcz);

    nTop += 18;
    TextOut(hdc, nLeft, nTop, "�Գ���Կ�汾");

    nTop += 18;
    bzero(pcz,sizeof(pcz));
    for (int i = 0;i < 16; i++){
        if(i == 0){
            sprintf(pcz, "%02X", tESAMInfoBuff.symKeyVersion[i]);
        }else {
            sprintf(pcz, "%s%02X", pcz, tESAMInfoBuff.symKeyVersion[i]);
        }
    }
    TextOut(hdc, nLeft, nTop, pcz);

    return;
}


PLUGIN_INFO g_layer_symKey_ver=
{
    0x000c0001, // 0x000c0001
    symKey_Init,
    NULL,
    symKey_Draw,
    NULL,
    NULL,
    NULL
};

#endif

#if DESC("����״̬��ʾ",1)

char  device[50] = {0};
uint8 mmc_result[1024] = {0};
int   dis_index = 0;  //
int   data_flag = 0;  // 0 ��ʾ�������ݣ� ������ʾ��ȡʧ��

int calData(uint8 *buf, uint16 len, uint32 *data)
{
    uint16  i = 0;

    if(buf == NULL || data == NULL)
    {
        return -1;
    }
    
    if(len <= 0 )
    {
        return -2;
    }
    
    //����ֵ
    *data = buf[0];
    
    for(i=1; i<len;i++)
    {
        *data += buf[i]<<(i*8);
    }
    
    return 0;
} 

int StringHextoHex(char *str, unsigned char *out, int *outlen)
{
    //str�е�������' '���
    if(str == NULL || out == NULL || outlen == NULL)
    {
        return -1;
    }
    
    int index = 0;
    int outIndex = 0;
    uint8 data = 0;
    int flag = 1;                //��Ϊ�Ǹ�4λ
    while(str[index] != '\0')
    {
      //���Ƿ�ո�
      if(str[index] == ' ')
      {
        flag = 1;
        out[outIndex++] = data;
        data = 0;
        index++;
        continue;
      }
      else if(str[index] >= '0' && str[index] <= '9')
      {
        data += (str[index] - '0') << (4 * flag);
      }
      else if(str[index] >= 'A' && str[index] <= 'Z')
      {
        data += (str[index] - 'A' + 10) << (4 * flag);
      }
      else if(str[index] >= 'a' && str[index] <= 'z')
      {
        data += (str[index] - 'a' + 10) << (4 * flag);
      }
      else if(str[index] == '\n')
      {
        //������һ������
        index++;
        continue;
      }
      else
      {
        GUI_FMT_DEBUG("str[%d] = %d\n", index, str[index]);
        return -2;
      }
      index++;
      flag = 0;
    }

    //�����һ������
//    if(str[index]=='\0' && index >= 2 && str[index - 1] != ' ')
//    {
//       out[outIndex] = data;
//    }

    *outlen = outIndex + 1;
    return 0;
}

uint32 MmcCmdExecute(const char *cmd, char *result)
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

int get_root_mmc_device(char * device)
{
    char buf[1024] = {0};
    int  offset = 0; //�豸��ƫ�ƶȣ����pfile
    int  has_mmc_device = 0;  //0��ʾδ�ҵ� 
    if(device == NULL)
        return -1;
    
    FILE* pipe = fopen("/proc/diskstats", "r");
    if (!pipe)
    {
        pipe = fopen("/proc/diskstats", "r");
    }
    if (!pipe)
    {
        GUI_FMT_DEBUG("��ѯmmc�豸ʧ�� ������[-2]\n");
        return -2;
    }
    
    // fseek(pipe, -1024L, SEEK_END);
    while (fgets(buf, 1024, pipe) != NULL)
    {
        GUI_FMT_DEBUG("%s", buf);
        char *pfile = NULL;
        char *pfend = NULL;

        //�ҵ� 
        if ((pfile = strstr(buf, "mmc")) != NULL)
        {
            //pfile ��ǰ�ƶ������豸��
            pfile -= 2;
            
            if(pfile[0] == '0')
            {
                pfile += 2;
                //ȷ����mmc��ͷ���׸��ַ���
                pfend = pfile;
                offset = 0;
                while((pfend[offset] != ' '))
                {
                    offset++;
                    //offset �����ܴ���50
                    if(offset > 50)
                    {
                        GUI_FMT_DEBUG("�豸�ų��ȴ���50\n");
                        return -3;
                    }
                }
                has_mmc_device = 1;
                memcpy(device, pfile, offset);
                break;
            }
            else
            {
                continue;
            }
        }
        
        memset(buf, 0x0, sizeof(buf));
    }

    if(0 == has_mmc_device)
    {
        GUI_FMT_DEBUG("�豸δ�ҵ�\n");
        return -4;
    }
    return 0;
}


int get_mmc_information(uint8 *result)
{
    if(result == NULL)
    {
        GUI_FMT_DEBUG("get_mmc_information() ���Ϊ��ָ��");
        return -1;
    }
    int ret = 0;
    char *pdata = NULL;//ָ��Ҫ�����ݣ�Ŀǰ��ָresponse 0 : 

    //ָ����Ҫ��

    //char mmc_cmd1[512] = "mmc read health /dev/";
    char mmc_cmd[512] = "cd /usr/bin/ && ./mmc read health /dev/";

    uint8 data[2014] = {0};
    int   datalen    = 0;
    char  tmp[MAX_RESULT_LEN + 1]  = {0};
    
    ret = get_root_mmc_device(device);
    if(ret != 0)
    {
        GUI_FMT_DEBUG("��ѯmmc�豸ʧ��, ret = %d\n", ret);
        return -2;
    }
    strcat(mmc_cmd, device);
    
    GUI_FMT_DEBUG("%d, %s \n", ret, mmc_cmd);

    ret = MmcCmdExecute(mmc_cmd, tmp);
    if(0 != ret)
    {
        GUI_FMT_DEBUG("./mmc ����ִ��ʧ��, ret = %d\n", ret);
        return -3;
    }

    pdata = strstr(tmp, "response 0 : \n");
    
    if(pdata != NULL)
    {
        pdata += strlen("response 0 : \n");
    }
    else
    {
        GUI_FMT_DEBUG("����ִ�н������response 0, ��Ϊ./mmc ����ִ��ʧ��\n");
        return -3;
    }

    GUI_BUF_TRACE(pdata, strlen(pdata), "��ӡ���%s , %d \n", pdata, strlen(pdata));
    ret = StringHextoHex(pdata, data, &datalen);

    GUI_BUF_DEBUG(data, datalen, "ת����16���� \n", data[24]);

    memcpy(result, data, datalen);
    
    return 0;
}
void disk_status_init(HDC hdc)
{
    data_flag = get_mmc_information( mmc_result);
}

void disk_status_draw0(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if(pg_guicomm == NULL)
       return;
    
    uint32 data = 0;
    char      czBuf[32];
    
    SelectFont(hdc,pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 20;
    rc.bottom = rc.top+16;
    char pcz[40];
    strcpy(pcz, "���̵�ǰ״̬");
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER|DT_VCENTER);

    SelectFont(hdc,pg_guicomm->fontSmall);


    int nTop = 40;
    int lefts_pace = 20;
    
    int nLeft = lefts_pace;
    int space2 = 12; // ��� �ֶ����ֶ�֮��ļ��
    int space  = 190;   //data ��ʼλ��
    
    TextOut(hdc, nLeft, nTop, "Bad Block Manufacturer:");   
    nLeft = space;

    if(data_flag != 0 || calData(&mmc_result[24], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }
    
    nTop += space2;
    nLeft = lefts_pace;
    TextOut(hdc, nLeft, nTop, "Bad Block Runtime System:");
    nLeft = space;
    if(data_flag != 0 || calData(&mmc_result[28], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }

    nTop += space2;
    nLeft = lefts_pace;
    TextOut(hdc, nLeft, nTop, "Bad Block Runtime MLC:");    
    nLeft = space;
    if(data_flag != 0 || calData(&mmc_result[36], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }

    nTop += space2;
    nLeft = lefts_pace;
    TextOut(hdc, nLeft, nTop, "Max Erase Count EUDA:");
    nLeft = space;
    if(data_flag != 0 || calData(&mmc_result[128], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }

    nTop += space2;
    nLeft = lefts_pace;
    TextOut(hdc, nLeft, nTop, "Min Erase Count EUDA:");
    nLeft = space;
    if(data_flag != 0 || calData(&mmc_result[132], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }

    nTop += space2;
    nLeft = lefts_pace;
    TextOut(hdc, nLeft, nTop, "Avg Erase Count EUDA:");
    nLeft = space;
    if(data_flag != 0 || calData(&mmc_result[136], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }

    nTop += space2;
    nLeft = lefts_pace;
    TextOut(hdc, nLeft, nTop, "Health Device Level EUDA:");
    
    nLeft = space;
    if(data_flag != 0 || calData(&mmc_result[176], 4, &data) != 0)
    {
         TextOut(hdc, nLeft, nTop, "---");
    }
    else
    {
         sprintf(czBuf, "%d%%", data);
         TextOut(hdc, nLeft, nTop, czBuf);
    }

}
void disk_status_draw(HDC hdc)
{
    if(dis_index == 0)
    {
        disk_status_draw0(hdc);
    }
    else
    {
        disk_status_draw0(hdc);
    }
}

BOOL disk_status_Msg(int nKey)
{
     switch(nKey)
     {
     case KEY_UP:
         dis_index--;
         if(dis_index < 0)
             dis_index = 1;
         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     case KEY_DOWN:
         dis_index ++;
         if(dis_index > 1)
             dis_index = 0;
         UpdateWindow(g_guiComm.hMainWnd, TRUE);
         return TRUE;

     default:
        return FALSE;
     }

     return FALSE;
}

PLUGIN_INFO g_disk_status=
{
    0x00B000F1, // 0x00A000F9
    disk_status_init,
    NULL,
    disk_status_draw,
    NULL,
    disk_status_Msg,
    NULL
};
#endif


BOOL query_switch = FALSE;

#if DESC("��·״̬",1)

void loop_state_display_init(DWORD dwPluginId)
{
    query_switch=FALSE;
}

void loop_state_display_draws(HDC hdc)
{
    OOP_LOOPSTAT_T loopStat;

    memset(&loopStat, 0, sizeof(OOP_LOOPSTAT_T));
    read_db_noamal_data(g_hUdbCenter, 0x20500200, 0, 0, (uint8*)&loopStat, sizeof(OOP_LOOPSTAT_T));

    char tmp_status[3][256];

    for (int i =0; i < 3; i++)
    {
        switch(loopStat.loopstat[i])
        {
            case 0:
            {
                sprintf(tmp_status[i], "������·����");
                break;
            }
            case 1:
            {
                sprintf(tmp_status[i], "һ/���λ�·��·");
                break;
            }
            case 2:
            {
                sprintf(tmp_status[i], "������·��·");
                break;
            }
            default:
            {
                sprintf(tmp_status[i], "��·���������豸");
                break;
            }
        }     
    }

    SelectFont(hdc,g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    char czBuf[100] = {0};
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 20;
    rc.bottom = rc.top+16;
	sprintf(czBuf, "������·״̬");
    TextOut(hdc, 5, 20, czBuf);

    ITEM_SELECT;
    TextOut(hdc, 160, 20, "��ѯ");
    ITEM_NOT_SELECT;

    int nTop = rc.top+22;
    int nLeft1 = 5;
    int nLeft2 = 100;

    GUI_FMT_DEBUG("query_switch_draws:, %x\n", query_switch);

    TextOut(hdc, nLeft1, nTop, "A�������·");
    sprintf(czBuf, "%s", (query_switch==FALSE)?"------------":tmp_status[0]);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "B�������·");
    sprintf(czBuf, "%s", (query_switch==FALSE)?"------------":tmp_status[1]);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "C�������·");
    sprintf(czBuf, "%s", (query_switch==FALSE)?"------------":tmp_status[2]);
    TextOut(hdc, nLeft2, nTop, czBuf);
}

BOOL LoopStateKeyMsg(int nKey)
{
    switch(nKey)
    {
    case KEY_OK:
    {
        query_switch = TRUE;
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        GUI_FMT_DEBUG("query_switch_KeyMsg:, %x\n", query_switch);
        break;
    }
    return TRUE;
    }
    return FALSE;
}



//��·״̬
PLUGIN_INFO g_layer_loop_state_display = {
    0x20500200/*id*/,
    loop_state_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    loop_state_display_draws/*Draw*/,
    NULL/*Timer*/,
    LoopStateKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
    
#endif

#if DESC("Ѳ���ǰ汾",1)

void inspector_ver_display_init(DWORD dwPluginId)
{
    query_switch=FALSE;
}

void inspector_ver_display_draws(HDC hdc)
{
    OOP_TASPCUSEMOD_T version;

    memset(&version, 0, sizeof(OOP_TASPCUSEMOD_T));
    read_db_noamal_data(g_hUdbCenter, 0x40400200, 0, 0, (uint8*)&version, sizeof(OOP_TASPCUSEMOD_T));

    SelectFont(hdc,g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    char czBuf[100] = {0};
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 20;
    rc.bottom = rc.top+16;
	//sprintf(czBuf, "");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER|DT_VCENTER);

    ITEM_SELECT;
    TextOut(hdc, 160, 20, "��ѯ");
    ITEM_NOT_SELECT;

    int nTop = rc.top+22;
    int nLeft1 = 5;
    int nLeft2 = 100;

    TextOut(hdc, nLeft1, nTop, "Ѳ���ǰ汾");
    sprintf(czBuf, "%s", (query_switch==FALSE)?"------------":version.runsoftver.value);
    TextOut(hdc, nLeft2, nTop, czBuf);

}

BOOL InspectorVerKeyMsg(int nKey)
{
    switch(nKey)
    {
    case KEY_OK:
    {
        query_switch = TRUE;
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        break;
    }
    return TRUE;
    }
    return FALSE;
}



//Ѳ���ǰ汾
PLUGIN_INFO g_layer_inspector_ver_display = {
    0x40400200/*id*/,
    inspector_ver_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    inspector_ver_display_draws/*Draw*/,
    NULL/*Timer*/,
    InspectorVerKeyMsg/*keymsg*/,
    NULL/*ipc*/
};
    
#endif

#if DESC("���ɵ�Դ����",1)

//unix udp �󶨵��ļ�����(ȫ·��)��Ŀ���ַ
#define ACMETER_MSG_PATH       "/tmp/dev"
#define ACMETER_APP_DEST       "/tmp/dev/acmeter"
#define ACMETER_FILEPATH_LEN   256

static char acconf[15]; // acconf: AC Conf(���ɵ�Դ����)
static char tmp_acconf[15];
static int g_fUdpAcMeter = 0;
static int set_switch = 0; 
static uint8 set_index;
static uint8 tmp_sel;
static int flg = 0;


#if 1
pthread_mutex_t g_fUdpAcMeterLock;
#define ACMETER_BUFF_LEN       4096        //��������������
uint8   g_AcPiid = 0;     //�ϱ�����֡���к�
#define AC_ADD_VALUE_CLEAR(count, mcount, value) ((count) + (value) >= (mcount) ? 0 : (count) + (value))
#define OUT_DATA_MAX           2048

/*
*********************************************************************
* @name      : puAmr_acMeter_fd_init
* @brief     �������뽻��APP�����ľ��
* @param[in] ��
* @param[out]��
* @return    ��������
*********************************************************************
*/
int puAmr_acMeter_fd_init(void)
{
    if (access(ACMETER_APP_DEST, F_OK) != 0)
    {
        GUI_FMT_DEBUG("%s is nonexistent\n", ACMETER_APP_DEST);
        return ERR_NODEV;
    }

    if (g_fUdpAcMeter <= 0)
    {
        char filename[ACMETER_FILEPATH_LEN] = {0};
        sprintf(filename, "%s/%s_acMeter", ACMETER_MSG_PATH, GUI_APP_NAME);
        g_fUdpAcMeter = task_pfmsg_init(filename);
    }
    if (g_fUdpAcMeter <= 0)
    {
        GUI_FMT_DEBUG("%d is invalid\n", g_fUdpAcMeter);
        return ERR_INVALID;
    }

    return ERR_OK;
}


/*
*********************************************************************
* @name      : puAmr_acMeter_revsnd_data
* @brief     ���뽻��APP������ͬ���ӿ�
* @param[in] ��
* @param[out]��
* @return    ��������
*********************************************************************
*/
int puAmr_acMeter_revsnd_unit(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen)
{
    int32 i = 0;
    int32 iRet = 0;
    uint8 czBuf[ACMETER_BUFF_LEN] = {0};
    uint8 sndBuf[ACMETER_BUFF_LEN] = {0};
    char  sender[ACMETER_FILEPATH_LEN] = {0};

    if (sndLen > sizeof(sndBuf))
    {
        return ERR_OPTION;
    }

    if (g_fUdpAcMeter <= 0)
    {
        if (0 != puAmr_acMeter_fd_init())
        {
            return ERR_INVALID;
        } 
    }

    /* ���Լ��� */
    while (0 != pthread_mutex_trylock(&g_fUdpAcMeterLock))
    {
        if(i++ > 500)
        {
            GUI_FMT_DEBUG("puAmr_acMeter_revsnd_data lock faild g_fUdpAcMeterLock[]\n", g_fUdpAcMeterLock);
            return ERR_NORESOURCE;
        }
        usleep(10000);
    }

    /* ���� 5s��ʱ */
    task_msg_settime(g_fUdpAcMeter, 5, 0);
    iRet = task_pfmsg_recv(&g_fUdpAcMeter, czBuf, ACMETER_BUFF_LEN, sender, ACMETER_FILEPATH_LEN);
    pthread_mutex_unlock(&g_fUdpAcMeterLock);

    if (iRet <= 0)
    {
        GUI_FMT_DEBUG("MSGAC recv faild, ret %d\n", iRet);
        return ERR_NORESOURCE;
    }

    GUI_BUF_DEBUG(czBuf, iRet, "MSGAC RECV:");

    *rcvLen = iRet;
    memcpy(pOutBuf, czBuf, *rcvLen);

    return ERR_OK;
}

/*
*********************************************************************
* @name      : puAmr_acMeter_revsnd
* @brief     ���뽻��APP������ͬ���ӿ�
* @param[in] ��
* @param[out]��
* @return    ��������
*********************************************************************
*/
int puAmr_acMeter_revsnd(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen)
{
    uint32 i = 0;
    int8   ret = ERR_NORMAL;

    while (i < 1)
    {
        ret = puAmr_acMeter_revsnd_unit(sndData, sndLen, pOutBuf, rcvLen);
        if (ERR_OK == ret)
        {
            break;
        }
        else
        {
            i++;
            usleep(10000);
        }
    }

    return ret;
}

/*
*********************************************************************
* @name      : puAmr_acMeter_get
* @brief     �������ɵ�ͨ�ýӿ�
* @param[in] ��
* @param[out]��
* @return    ��������
*********************************************************************
*/
int puAmr_acMeter_get(OOP_OAD_U oad, uint8 *prcvbuf, uint16 *prcvLen)
{
    int            ret = ERR_OK;
    //uint8          rcvbuf[OUT_DATA_MAX] = {0};
    //uint16         rcvLen = 0;
    uint8          sndbuf_tmp[25] = {0x68,0x17,0x00,0x43,0x05,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xA7,0xE6,0x05,0x01,0x00,0x41,0x04,0x02,0x00,0x00,0xFE,0x66,0x16};

    ret = puAmr_acMeter_revsnd(sndbuf_tmp, 25, prcvbuf, prcvLen);
    GUI_BUF_DEBUG(prcvbuf, *prcvLen, "rcvbuf:");
    
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG("puAmr_acMeter_revsnd failed. ret(%d), oad(0x%08x)\n", ret, oad.value);
    }

    //memcpy(pOutBuf, rcvbuf, rcvLen);
    
    return ret;

}

/**********************************************************************
* @brief     ����ȡ���ɶ��ѹ����
* @param[in] ��
* @param[out]��
* @return    ��
**********************************************************************/
void get_vol_type(void)
{
    uint8          rcvbuf[OUT_DATA_MAX] = {0};
    uint16         rcvLen = 0;

    if (flg == 1)
        return;
    
    OOP_OAD_U oad = {0};
    int ret = 0;

    oad.value = 0x41040200;
    ret = puAmr_acMeter_get(oad, rcvbuf, &rcvLen);
    if(ret != 0)
    {
        GUI_FMT_DEBUG("puAmr_acMeter_get ���ѹʧ�� err ret[%d]\n", ret);
        if(acconf[0] == '\0')
        {
            GUI_FMT_DEBUG("����-----------");
            strcpy(acconf, "-----------");
        }
            
        return;
    }

    GUI_BUF_DEBUG(rcvbuf, rcvLen, "MSGAC RECV:");

    if(rcvbuf[24] == 0x32)
    {
        GUI_FMT_DEBUG("������������220V");
        strcpy(acconf, "��������220V");
        flg = 1;
    }
    else if(rcvbuf[24] == 0x31)
    {
        GUI_FMT_DEBUG("������������100V");
        strcpy(acconf, "��������100V");
        flg = 1;
    }
    else if(rcvbuf[24] == 0x20)
    {
        GUI_FMT_DEBUG("������������57.7V");
        strcpy(acconf, "��������57.7V");
        flg = 1;
    }
    else
    {
        if(acconf[0] == '\0')
        {
            GUI_FMT_DEBUG("����-----------");
            strcpy(acconf, "-----------");
        }  
    }

    return;
}
#endif

void ProductVoltageCfgInit(DWORD dwPluginId)
{
    set_index = 0;
    set_switch = 0;
    int ret = 0;
    //get_vol_type();
    if (g_fUdpAcMeter <= 0)
    {
        char filename[ACMETER_FILEPATH_LEN] = {0};
        sprintf(filename, "%s/%s_acMeter", ACMETER_MSG_PATH, GUI_APP_NAME);
        g_fUdpAcMeter = task_pfmsg_init(filename);
    }
    
    if (flg == 0)
    {
        uint8 sndBuf[25] = {0x68,0x17,0x00,0x43,0x55,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x00,0x23,0x14,0x05,0x01,0x00,0x41,0x04,0x02,0x00,0x00,0xfe,0x66,0x16};
        ret = task_pfmsg_send(g_fUdpAcMeter, sndBuf, 25, (char *)ACMETER_APP_DEST);
        GUI_FMT_DEBUG("���Ͳ�ѯ���ѹ��Ϣ������[%d]\n", ret);
    }
}

void ProductVoltageCfgDraw(HDC hdc)
{
        //char czbuf[10];
        //char CheckSumFlag=0;
        int nLeft1, nLeft2, y;
        nLeft1 = 10;
        nLeft2 = 90;
        //nAdd = 13;
        y = 20;
    
        get_vol_type();
        SelectFont(hdc,g_guiComm.fontSmall);
    
        ITEM_NOT_SELECT;
        TextOut(hdc, nLeft1, y, "���ɵ�Դ");
        
        if (set_index == 0)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2-15, y, (set_switch == 0)?acconf:tmp_acconf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2-15, y, (set_switch == 0)?acconf:tmp_acconf);
        }

        if (set_index == 1)
        {
            ITEM_SELECT
            TextOut(hdc, nLeft2-4, y + 60, "����");
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2-4, y + 60, "����");
        }

        PopListDraw(hdc);
        
}

BOOL ProductVoltageCfgKeyMsg(int nKey)
{
    if(g_guiComm.bPopList == TRUE)
	{
	    int nSel=0; 
        PopListKeyMsg(nKey, &nSel);

        if(nKey == KEY_OK)
        {
            if (nSel == 0)
            {
                tmp_sel = 0;
                set_switch = 1;
                strcpy(tmp_acconf, "��������100V");
            }
            else if (nSel == 1)
            {
                tmp_sel = 1;
                set_switch = 1;
                strcpy(tmp_acconf, "��������220V");
            }
            else
            {
                tmp_sel = 2;
                set_switch = 1;
                strcpy(tmp_acconf, "��������57.7V");
            }
        }
    }
    else
    {    
        switch(nKey)
        {
            case KEY_UP:
    	    {
    	        if (set_index > 0)
    	            set_index--;
    	        else
    	            set_index = 1;
    	        UpdateWindow(g_guiComm.hMainWnd, TRUE);
    	        return TRUE;
    	    }
    	    case KEY_DOWN:
    	    {
    	        if (set_index < 1)
    	            set_index++;
    	        else
    	            set_index = 0;
    	        UpdateWindow(g_guiComm.hMainWnd, TRUE);
    	        return TRUE;
    	    }
            case KEY_CANCEL:
            {
                if (strcmp(acconf, "��������100V") == 0)
                {
                    tmp_sel = 0;
                }
                else if (strcmp(acconf, "��������220V") == 0)
                {
                    tmp_sel = 1;
                }
                else
                {
                    tmp_sel = 2;
                }
                return FALSE;
            }
            case KEY_OK:
            {
                if (ValidatePwd() == TRUE)
	            {
		            if (set_index == 0)
		            {  
                        const char * ListVoltage[] = {"��������100V","��������220V","��������57.7V"};
                        PopList(ListVoltage, 3, acconf, 75, 20, FALSE);
                        return TRUE;
                    }
                    else
                    {
                        int ret = 0;
                        if (g_fUdpAcMeter <= 0)
                        {
                            char filename[ACMETER_FILEPATH_LEN] = {0};
                            sprintf(filename, "%s/%s_acMeter", ACMETER_MSG_PATH, GUI_APP_NAME);
                            g_fUdpAcMeter = task_pfmsg_init(filename);
                        }
                        
                        if (tmp_sel == 0)
                        {
                            strcpy(acconf, "��������100V");
                            uint8 sndBuf[17] = {0x68,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x68,0x05,0x05,0x0A,0xFF,0x43,0xFF,0xFF,0x20,0x16};
                            ret = task_pfmsg_send(g_fUdpAcMeter, sndBuf, 17, (char *)ACMETER_APP_DEST);
                            GUI_FMT_DEBUG("������������100V������[%d]\n", ret);
                        }
                        else if (tmp_sel == 1)
                        {
                            strcpy(acconf, "��������220V");
                            uint8 sndBuf[17] = {0x68,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x68,0x05,0x05,0x0A,0xFF,0x55,0xFF,0xFF,0x32,0x16};
                            ret = task_pfmsg_send(g_fUdpAcMeter, sndBuf, 17, (char *)ACMETER_APP_DEST);
                            GUI_FMT_DEBUG("������������220V������[%d]\n", ret);
                        }
                        else
                        {
                            strcpy(acconf, "��������57.7V");
                            uint8 sndBuf[17] = {0x68,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x68,0x05,0x05,0x0A,0xFF,0x44,0xFF,0xFF,0x21,0x16};
                            ret = task_pfmsg_send(g_fUdpAcMeter, sndBuf, 17, (char *)ACMETER_APP_DEST);
                            GUI_FMT_DEBUG("������������57.7V������[%d]\n", ret);
                        }

                        if (ret == 0)
                        {
                            MessageBox("����ɹ���");
                            return TRUE;
                        }
                        else
    		            {
    		                MessageBox("����ʧ�ܣ�");
    		                return FALSE;
    		            }
                    }
                }
            }
                break;
             default:
        	    return (FALSE);
        		break;
    	}
    }
    return (TRUE);
}

//���ɵ�Դ����
PLUGIN_INFO g_layerProductVoltageCfg = {
    0x20B70001/*id*/,
    ProductVoltageCfgInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    ProductVoltageCfgDraw/*Draw*/,
    NULL/*Timer*/,
    ProductVoltageCfgKeyMsg/*keymsg*/,
    NULL/*ipc*/
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
        nLeft = 61;
        sprintf(buf, "���ڰ�װ...(%u��)", g_CoWaitTimer);
        TextOut(hdc, nLeft, nTop, buf);
        nTop = 60;
        nLeft = 77;
        TextOut(hdc, nLeft, nTop, "��ʱԼ1����");      
        nTop = 80;
        nLeft = 69;
        TextOut(hdc, nLeft, nTop, "�벻Ҫ�رյ�Դ");
        nTop = 100;
        nLeft = 77;
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
    nLeft = 60;

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

    nLeft = 152;
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
            
            if (g_CoWaitTimer <= 179)
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
const char* ac_nType[] = {"------", "��������220V", "��������100V", "��������57.7V"};
static int nState;
uint8 ac_set_index;
void ac_FeatureCfgInit(DWORD dwPluginId)
{
    ac_set_index = 0;
	return;
}

void ac_FeatureCfgDraw(HDC hdc)
{
	int nLeft1, nLeft2, y;
	nLeft1 = 20;
	nLeft2 = 90;
	y = 40;

	SelectFont(hdc,g_guiComm.fontSmall);

	TextOut(hdc, nLeft1, y, "���ɵ�Դ");
    if (ac_set_index == 0)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft2 + 50, y, ac_nType[nState]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 + 50, y, ac_nType[nState]);
    }

    if (ac_set_index == 1)
    {
        ITEM_SELECT
        TextOut(hdc, nLeft2 + 60, y + 60, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 + 60, y + 60, "����");
    }
    PopListDraw(hdc);
}

BOOL ac_FeatureCfgKeyMsg(int nKey)
{
    int maxindex = 1;
    if (g_guiComm.bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {   
            switch (ac_set_index)
            {
            case 0:
            {
                nState = nSel;
            }
            break;
            default:
            break;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
        }
        return TRUE;
    }
   switch (nKey)
    {
	    case KEY_UP:
	    {
	        if (ac_set_index > 0)
	            ac_set_index--;
	        else
	            ac_set_index = maxindex;
	        UpdateWindow(g_guiComm.hMainWnd, TRUE);
	        return TRUE;
	    }
	    case KEY_DOWN:
	    {
	        if (ac_set_index < maxindex)
	            ac_set_index++;
	        else
	            ac_set_index = 0;
	        UpdateWindow(g_guiComm.hMainWnd, TRUE);
	        return TRUE;
	    }
	    case KEY_OK:
	    {
	    	if (ValidatePwd() == TRUE)
	        {
		        if (ac_set_index == 0)
		        {    
		            PopList(ac_nType, 4, ac_nType[nState], 120, 20, FALSE);
		            return TRUE;
		        }
		        //��������
		        if (ac_set_index == 1)
		        {
		            //ret = write_normal_data((uint8*)&MeterFile, sizeof(OOP_METER_T), MeterFileOad, g_guiComm.nCurPoint, 0, 2);
		            if (ac_set_index == 0)
		            {
		                MessageBox("��ǰ�������ñ���ɹ���");
		                ac_set_index = 0;
		                return TRUE;
		            }
		            else
		            {
		                MessageBox("��ǰ�������ñ���ʧ�ܣ�");
		                return FALSE;
		            }

		        }
	    	}
	    }
    }
    return FALSE;
}

uint8 GprsExpandPageIndex;    //��������Ϣ��Ҫ��ҳ��ʾ���·�ҳ
uint8 GprsnumnetPageIndex;   //���ҷ�ҳ
OOP_APNGROUP_T GprsExpand;   //���������ýṹ��
OOP_NETCONFIG_T Gprsnumnet;  //���������ýṹ��
//static int GprsConfigHandle;        //��db���ݾ����0�ɹ�������ʧ��
OOP_OAD_U GPRSExpandOad;     //��OAD
extern PLUGIN_INFO g_layer_gprs2_numnet;
uint8 apnIndex;
void gprs_numnet_init(HDC hdc)
{
    GprsExpandPageIndex = 0;
    GprsnumnetPageIndex = 0;
    apnIndex = 0;
    GPRSExpandOad.value = 0x45000e00;
    memset(&GprsExpand, 0x00, sizeof(OOP_APNGROUP_T));
    read_db_noamal_data(g_hUdbCenter, GPRSExpandOad.value, 0, 0, (uint8*)&GprsExpand, sizeof(OOP_APNGROUP_T));
}

void gprs2_numnet_init(HDC hdc)
{   
    GPRSExpandOad.value = 0x45010e00;
    memset(&GprsExpand, 0x00, sizeof(OOP_APNGROUP_T));
    read_db_noamal_data(g_hUdbCenter, GPRSExpandOad.value, 0, 0, (uint8*)&GprsExpand, sizeof(OOP_APNGROUP_T));
}

void gprs1_numnet_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    uint8 index = 0;
    char czBuf[300];
    sprintf(czBuf, "��%d���������",GprsnumnetPageIndex+1);
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = rc.top+20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    memset(&Gprsnumnet, 0, sizeof(OOP_NETCONFIG_T));
    Gprsnumnet = GprsExpand.net[GprsnumnetPageIndex];

    if (GprsExpandPageIndex == 0)  //��һҳ
    {   
        TextOut(hdc, nLeft1, nTop, "��Ӫ��");
        if ((Gprsnumnet.factory==255) || (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0))
        {
            sprintf(czBuf,  "%s", g_operator_seclct[4]);
        }
        else 
        {
            sprintf(czBuf, "%s", g_operator_seclct[Gprsnumnet.factory]);
        }
        if (apnIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else{
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += 15;
        index++;
        TextOut(hdc, nLeft1, nTop, "��������");
        memset(czBuf, 0x00, sizeof(czBuf));
        if ((Gprsnumnet.network==255) || (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0))
        {
            sprintf(czBuf, "%s", g_net_mode[4]);
        }
        else 
        {
            sprintf(czBuf, "%s", g_net_mode[Gprsnumnet.network]);
        }

        if (apnIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else{
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        nTop += 15;
        index++;
        TextOut(hdc, nLeft1, nTop, "APN");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (Gprsnumnet.apn.apn.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {
            sprintf(czBuf, "%s", Gprsnumnet.apn.apn.value);
        }
        if (apnIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else{
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        nTop += 15;
        index++;
        TextOut(hdc, nLeft1, nTop, "�û���");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (Gprsnumnet.apn.username.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", Gprsnumnet.apn.username.value);
        }
        if (apnIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else{
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        nTop += 15;
        index++;
        TextOut(hdc, nLeft1, nTop, "����");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (Gprsnumnet.apn.pwd.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", Gprsnumnet.apn.pwd.value);
        }
        if (apnIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else{
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        
        nTop += 15;
        index++;
        TextOut(hdc, nLeft1, nTop, "�����ַ");
        if (Gprsnumnet.apn.proxyip[0] == 0)
        {
            sprintf(czBuf, "-.-.-.-");
        }
        else
        {
            sprintf(czBuf, "%d.%d.%d.%d", Gprsnumnet.apn.proxyip[0], Gprsnumnet.apn.proxyip[1],Gprsnumnet.apn.proxyip[2], Gprsnumnet.apn.proxyip[3]);
        }
        if (apnIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else{
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
   }
    else                                            //�ڶ�ҳ
   {
        index = 0;
        TextOut(hdc, nLeft1, nTop, "����˿�");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (Gprsnumnet.apn.proxyport == 65535)
        {
            sprintf(czBuf, "%d", 0);
        }
        else
        {
        
            sprintf(czBuf, "%d", Gprsnumnet.apn.proxyport);
        }
        if (apnIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else{
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        
        nTop += 15;
        index++;
        TextOut(hdc, nLeft1, nTop, "��Ȩ��ʽ");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (((Gprsnumnet.auth>4)&&(Gprsnumnet.auth != 255)) || (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0))
        {
            sprintf(czBuf, "-");
        }
        else if(Gprsnumnet.auth == 255)
        {
            sprintf(czBuf, "%s ", g_auth_mode[4]);
        }
        else
        {
            sprintf(czBuf, "%s ", g_auth_mode[Gprsnumnet.auth]);
        }
        
        if (apnIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else{
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        
        nTop += 15;
        index++;
        TextOut(hdc, nLeft1, nTop, "IP��ַ");
        if (Gprsnumnet.master.master[0].ip[0] == 0)
        {
            sprintf(czBuf, "-.-.-.-");
        }
        else
        {
            sprintf(czBuf, "%d.%d.%d.%d", Gprsnumnet.master.master[0].ip[0], Gprsnumnet.master.master[0].ip[1], Gprsnumnet.master.master[0].ip[2], Gprsnumnet.master.master[0].ip[3]);
        }
        if (apnIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else{
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        
        nTop += 15;
        index++;
        TextOut(hdc, nLeft1, nTop, "�˿�");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (Gprsnumnet.master.master[0].port==0)
        {
            sprintf(czBuf, "%d", 0);
        }
        else
        {
        
            sprintf(czBuf, "%d ", Gprsnumnet.master.master[0].port);
        }
        if (apnIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else{
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        
        nTop += 15;
        index++;
        TextOut(hdc, nLeft1, nTop, "���õ�ַ");
        if (Gprsnumnet.master.master[1].ip[0] == 0)
        {
            sprintf(czBuf, "-.-.-.-");
        }
        else
        {
            sprintf(czBuf, "%d.%d.%d.%d", Gprsnumnet.master.master[1].ip[0], Gprsnumnet.master.master[1].ip[1], Gprsnumnet.master.master[1].ip[2], Gprsnumnet.master.master[1].ip[3]);
        }
        if (apnIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else{
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        nTop += 15;
        index++;
        TextOut(hdc, nLeft1, nTop, "���ö˿�");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (Gprsnumnet.master.master[1].port == 65535)
        {
            sprintf(czBuf, "%d", 0);
        }
        else
        {

            sprintf(czBuf, "%d", Gprsnumnet.master.master[1].port);
        }
        if (apnIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else{
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
    }
    PopListDraw(hdc);
}

void ApnConfigReturn(char* pNewStr, void* pPara)
{
    char czBuf[64] = { 0 };
    uint8 tmp[4] = { 0 };
    if (GprsExpandPageIndex == 0)
    {
        switch(apnIndex)
        {
            case 5:
            {
                if(FALSE == commfun_IP(pNewStr,tmp))
                {
                    if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                    {
                        sprintf(czBuf,"%d.%d.%d.%d", GprsExpand.net[GprsnumnetPageIndex].apn.proxyip[0], GprsExpand.net[GprsnumnetPageIndex].apn.proxyip[1],
                                                     GprsExpand.net[GprsnumnetPageIndex].apn.proxyip[2], GprsExpand.net[GprsnumnetPageIndex].apn.proxyip[3]);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x02);
                
                    }
                }else
                {
                    memcpy(GprsExpand.net[GprsnumnetPageIndex].apn.proxyip,tmp,4);
                    if (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0)
                    {
                        GprsExpand.nNum +=1;
                        set_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex);
                    }
                }
            }
             break;
            case 2:
            {
                if(strlen(pNewStr) > 64)
                {
                    if (TRUE == MessageBox("APN���ó��Ȳ��ܳ���64\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                    {
                        sprintf(czBuf,"%s",GprsExpand.net[GprsnumnetPageIndex].apn.apn.value);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                    }
                }
                else
                {
                    GprsExpand.net[GprsnumnetPageIndex].apn.apn.nNum = strlen(pNewStr);
                    strcpy(GprsExpand.net[GprsnumnetPageIndex].apn.apn.value, pNewStr);
                    if (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0)
                    {
                        GprsExpand.nNum +=1;
                        set_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex);
                    }
                }
            }
            break;
            case 3:
            {
                if(strlen(pNewStr) > 64)
                {
                    if (TRUE == MessageBox("�û������Ȳ��ܳ���64\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                    {
                        sprintf(czBuf,"%s",GprsExpand.net[GprsnumnetPageIndex].apn.username.value);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                    }
                }
                else
                {
                    GprsExpand.net[GprsnumnetPageIndex].apn.username.nNum = strlen(pNewStr);
                    strcpy(GprsExpand.net[GprsnumnetPageIndex].apn.username.value, pNewStr);
                    if (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0)
                    {
                        GprsExpand.nNum +=1;
                        set_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex);
                    }
                }
            }
            break;
            case 4:
            {
                if(strlen(pNewStr) > 64)
                {
                    if (TRUE == MessageBox("���볤�Ȳ��ܳ���64\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                    {
                        sprintf(czBuf,"%s",GprsExpand.net[GprsnumnetPageIndex].apn.pwd.value);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                    }
                }
                else
                {
                    GprsExpand.net[GprsnumnetPageIndex].apn.pwd.nNum = strlen(pNewStr);
                    strcpy(GprsExpand.net[GprsnumnetPageIndex].apn.pwd.value, pNewStr);
                    if (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0)
                    {
                        GprsExpand.nNum +=1;
                        set_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex);
                    }
                }
            }
            break;
        }
    }
    else
    {
        switch(apnIndex)
        {
            case 2:
            {
                if(FALSE == commfun_IP_unpack(pNewStr,tmp))
                {
                    if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", GprsExpand.net[GprsnumnetPageIndex].master.master[0].ip[0], GprsExpand.net[GprsnumnetPageIndex].master.master[0].ip[1],
                                                      GprsExpand.net[GprsnumnetPageIndex].master.master[0].ip[2], GprsExpand.net[GprsnumnetPageIndex].master.master[0].ip[3]);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x02);
                    }
                }
                else
                {
                    memcpy(GprsExpand.net[GprsnumnetPageIndex].master.master[0].ip,tmp,4);
                    if (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0)
                    {
                        GprsExpand.nNum +=1;
                        set_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex);
                    }
                    if (get_bit_value((uint8 *)&GprsExpand.net[GprsnumnetPageIndex].master.rsv,32,0) == 0)
                    {
                        GprsExpand.net[GprsnumnetPageIndex].master.nNum +=1;
                        set_bit_value((uint8 *)&GprsExpand.net[GprsnumnetPageIndex].master.rsv,32,0);
                    }
                }
            }
             break;
            case 3:
            {
                uint32 port = atoi(pNewStr);
                if((port<65535)&&(port>0))
                {
                    GprsExpand.net[GprsnumnetPageIndex].master.master[0].port = port;
                    if (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0)
                    {
                        GprsExpand.nNum +=1;
                        set_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex);
                    }
                    if (get_bit_value((uint8 *)&GprsExpand.net[GprsnumnetPageIndex].master.rsv,32,0) == 0)
                    {
                        GprsExpand.net[GprsnumnetPageIndex].master.nNum +=1;
                        set_bit_value((uint8 *)&GprsExpand.net[GprsnumnetPageIndex].master.rsv,32,0);
                    }
                }
                else
                {
                    if (TRUE == MessageBox("�˿ڱ������0С��65535\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                    {
                        sprintf(czBuf, "%d", GprsExpand.net[GprsnumnetPageIndex].master.master[0].port);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x01);
                    }
                }
            }
            break;
            case 4:
            {
                if(FALSE == commfun_IP_unpack(pNewStr,tmp))
                {
                    if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", GprsExpand.net[GprsnumnetPageIndex].master.master[1].ip[0], GprsExpand.net[GprsnumnetPageIndex].master.master[1].ip[1],
                                                      GprsExpand.net[GprsnumnetPageIndex].master.master[1].ip[2], GprsExpand.net[GprsnumnetPageIndex].master.master[1].ip[3]);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x02);
                    }
                }
                else
                {
                    memcpy(GprsExpand.net[GprsnumnetPageIndex].master.master[1].ip,tmp,4);
                    if (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0)
                    {
                        GprsExpand.nNum +=1;
                        set_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex);
                    }
                    if (get_bit_value((uint8 *)&GprsExpand.net[GprsnumnetPageIndex].master.rsv,32,1) == 0)
                    {
                        GprsExpand.net[GprsnumnetPageIndex].master.nNum +=1;
                        set_bit_value((uint8 *)&GprsExpand.net[GprsnumnetPageIndex].master.rsv,32,1);
                    }
                }
            }
             break;
            case 5:
            {
                uint32 port = atoi(pNewStr);
                if((port<65535)&&(port>0))
                {
                    GprsExpand.net[GprsnumnetPageIndex].master.master[1].port = port;
                    if (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0)
                    {
                        GprsExpand.nNum +=1;
                        set_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex);
                    }
                    if (get_bit_value((uint8 *)&GprsExpand.net[GprsnumnetPageIndex].master.rsv,32,1) == 0)
                    {
                        GprsExpand.net[GprsnumnetPageIndex].master.nNum +=1;
                        set_bit_value((uint8 *)&GprsExpand.net[GprsnumnetPageIndex].master.rsv,32,1);
                    }
                }
                else
                {
                    if (TRUE == MessageBox("�˿ڱ������0С��65535\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                    {
                        sprintf(czBuf, "%d", GprsExpand.net[GprsnumnetPageIndex].master.master[1].port);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x01);
                    }
                }
            }
            break;
            case 0:
            {
                uint32 port = atoi(pNewStr);
                if((port<65535)&&(port>0))
                {
                    GprsExpand.net[GprsnumnetPageIndex].apn.proxyport = port;
                    if (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0)
                    {
                        GprsExpand.nNum +=1;
                        set_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex);
                    }
                }
                else
                {
                    if (TRUE == MessageBox("�˿ڱ������0С��65535\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                    {
                        sprintf(czBuf, "%d", GprsExpand.net[GprsnumnetPageIndex].apn.proxyport);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x01);
                    }
                }
            }
            break;
        }
   }
}

BOOL gprs_numnet_keymsg(int nKey)
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
            if (GprsExpandPageIndex == 0)
            {
                switch (apnIndex)
                {
                    case 0://��Ӫ��
                    {
                        if(nSel == 4)
                           GprsExpand.net[GprsnumnetPageIndex].factory = 255;
                        else
                           GprsExpand.net[GprsnumnetPageIndex].factory = nSel;
                        if (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0)
                        {
                            GprsExpand.nNum +=1;
                            set_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex);
                        }
                    }
                    break;
                    case 1://��������
                    {
                        if(nSel == 4)
                           GprsExpand.net[GprsnumnetPageIndex].network = 255;
                        else
                           GprsExpand.net[GprsnumnetPageIndex].network = nSel;
                        if (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0)
                        {
                            GprsExpand.nNum +=1;
                            set_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex);
                        }
                    }
                    break;
                    default:
                    return FALSE;
                }
            }
            else
            {
                switch (apnIndex)
                {
                    case 1://��Ȩ��ʽ
                    {
                        if(nSel == 4)
                           GprsExpand.net[GprsnumnetPageIndex].auth = 255;
                        else
                           GprsExpand.net[GprsnumnetPageIndex].auth  = nSel;
                        if (get_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex) == 0)
                        {
                            GprsExpand.nNum +=1;
                            set_bit_value((uint8 *)&GprsExpand.resv,32,GprsnumnetPageIndex);
                        }
                    }
                    break;
                    default:
                    return FALSE;
                }
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch (nKey)
        {
    case KEY_LEFT:
        if (GprsnumnetPageIndex == 0)
        {
            GprsnumnetPageIndex = 7;
        }
        else 
        {
            GprsnumnetPageIndex--;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
        
    case  KEY_RIGHT:
        if (GprsnumnetPageIndex == 7)
        {
            GprsnumnetPageIndex = 0;
        }
        else 
        {
            GprsnumnetPageIndex++;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
        
    case  KEY_UP:
        if(apnIndex == 0)
        {
            apnIndex = 5;
            if(GprsExpandPageIndex == 0)
                GprsExpandPageIndex = 1;
            else 
                GprsExpandPageIndex=0;
        }
        else
        {
            apnIndex--;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case  KEY_DOWN:

        if(apnIndex == 5)
        {
            apnIndex = 0;
            if(GprsExpandPageIndex == 0)
                GprsExpandPageIndex = 1;
            else 
               GprsExpandPageIndex=0;
        }
        else 
        {
            apnIndex++;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
    {
        if (ValidatePwd() == TRUE)
        {
            int nTop = 40;
            int nLeft1 = 80;
            if (GprsExpandPageIndex == 0)
            {
                switch (apnIndex)
                {
                    case 0: //��Ӫ��
                    {
                        PopList(g_operator_seclct, 5, g_operator_seclct[0], nLeft1, nTop, FALSE);
                        return TRUE;
                    }
                    case 1://��������
                    {
                        PopList(g_net_mode, 5, g_net_mode[0], nLeft1, nTop+15, FALSE);
                        return TRUE;
                    }
                    case 2://APN
                    {
                        char czBuf[64];
                        sprintf(czBuf,"%s",GprsExpand.net[GprsnumnetPageIndex].apn.apn.value);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                        return TRUE;
                    }
                    case 3://�û���
                    {
                        char czBuf[64];
                        sprintf(czBuf,"%s",GprsExpand.net[GprsnumnetPageIndex].apn.username.value);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                        return TRUE;
                    }
                    case 4://����
                    {
                        char czBuf[64];
                        sprintf(czBuf,"%s",GprsExpand.net[GprsnumnetPageIndex].apn.pwd.value);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                        return TRUE;
                    }
                    case 5://�����ַ
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d.%d.%d.%d", GprsExpand.net[GprsnumnetPageIndex].apn.proxyip[0], GprsExpand.net[GprsnumnetPageIndex].apn.proxyip[1],
                                                      GprsExpand.net[GprsnumnetPageIndex].apn.proxyip[2], GprsExpand.net[GprsnumnetPageIndex].apn.proxyip[3]);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x02);
                        return TRUE;
                    }
                }
            }
            else
            {
                switch (apnIndex)
                {
                    case 0: //����˿�
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GprsExpand.net[GprsnumnetPageIndex].apn.proxyport);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 1://��Ȩ��ʽ
                    {
                        PopList(g_auth_mode, 5, g_auth_mode[0], nLeft1, nTop+15, FALSE);
                        return TRUE;
                    }
                    case 2://IP��ַ
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d.%d.%d.%d", GprsExpand.net[GprsnumnetPageIndex].master.master[0].ip[0], GprsExpand.net[GprsnumnetPageIndex].master.master[0].ip[1],
                                                      GprsExpand.net[GprsnumnetPageIndex].master.master[0].ip[2], GprsExpand.net[GprsnumnetPageIndex].master.master[0].ip[3]);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x02);
                        return TRUE;
                    }
                    case 3://�˿�
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GprsExpand.net[GprsnumnetPageIndex].master.master[0].port);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                    case 4://���õ�ַ
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d.%d.%d.%d", GprsExpand.net[GprsnumnetPageIndex].master.master[1].ip[0], GprsExpand.net[GprsnumnetPageIndex].master.master[1].ip[1],
                                                      GprsExpand.net[GprsnumnetPageIndex].master.master[1].ip[2], GprsExpand.net[GprsnumnetPageIndex].master.master[1].ip[3]);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x02);
                        return TRUE;
                    }
                    case 5://���ö˿�
                    {
                        char czBuf[20];
                        sprintf(czBuf, "%d", GprsExpand.net[GprsnumnetPageIndex].master.master[1].port);
                        InputBox(czBuf, ApnConfigReturn, NULL, 0x01);
                        return TRUE;
                    }
                }
            }
        }
        return FALSE;
    }
    case KEY_CANCEL:
    if(TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
    {
        GPRSExpandOad.value = 0x45000e00;
        int ret = write_normal_data((uint8*)&GprsExpand, sizeof(OOP_APNGROUP_T), GPRSExpandOad, 0, 0, 2);
        if(ret == 0){
            MessageBox("�������óɹ���");
            return FALSE;
        }else{
            MessageBox("��������ʧ�ܣ�");
            return FALSE;
        }
    }
    return FALSE;
    }
    return FALSE;
}


//���߹���1����������
PLUGIN_INFO g_layer_gprs_numnet = {
    0x1103000A/*id*/,
    gprs_numnet_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs1_numnet_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_numnet_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//���߹���ͨ�����ò鿴
PLUGIN_INFO g_layer_gprs2_display = {
    0x11030120/*id*/,
    gprs2_numnet_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs1_numnet_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_numnet_keymsg/*keymsg*/,
    NULL/*ipc*/
};

typedef struct _UP_STATUS
{
	uint8 up_mode;								// ����ģʽ
	uint8 up_resuilt;								// �������
	uint8 up_prog;								// ��������

} UP_STSTUS;

void UpdateStatusInit(DWORD dwPluginId)
{
    int         ret = -1;
    uint32      len = 0;
    NOMAL_OAD_T normal;
    
    memset(&g_tGuiVer, 0, sizeof(OOP_VERSION_S_T));
    MEMZERO(&normal, sizeof(normal));

    normal.oad.value = 0x4300A300;
    ret = db_read_nomal(g_hUdbCenter, &normal, sizeof(OOP_VERSION_S_T), (uint8*)&g_tGuiVer, &len);
    if ((ret != ERR_OK) || (len == 0))
    {
        GUI_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);

        //����һ��
        sleep(1);
        db_read_nomal(g_hUdbCenter, &normal, sizeof(OOP_VERSION_S_T), (uint8*)&g_tGuiVer, &len);
    }
}
char updateBuf[60];
char updateEnd[60];
void UpdateStatusDraw (HDC hdc) 
{
    ITEM_NOT_SELECT;
    SelectFont(hdc,g_guiComm.fontBig);

    //uint16 index=0;
    char temp[60];
    bzero(temp,sizeof(temp));

    int nTop = 24;
    TextOut (hdc, 10, nTop, "��ǰ�汾");

    memcpy(temp, g_tGuiVer.softVer, 4);
    temp[4] = ' ';
    memcpy(temp + 5, g_tGuiVer.softDate, 6);
    TextOut(hdc,90, nTop, temp);
    
    nTop += 20;
    TextOut (hdc, 10, nTop, "����ģʽ");
    TextOut (hdc, 90, nTop, "Զ������");
    nTop += 20;
    TextOut (hdc, 10, nTop, "��������");
    if (strstr(statusInfoFresh.text,"Զ���ļ��������"))
    {
        memcpy(temp, statusInfoFresh.text + 4, strlen(statusInfoFresh.text)- 4);
        if (strcmp(updateBuf, temp)!= 0)
            strcpy(updateBuf, temp);
    }
    
    if(strstr(statusInfoFresh.text,"����ɹ�"))
    {
        strcpy(updateBuf, "�ļ��������100%");
        TextOut (hdc, 90, nTop, updateBuf);
    }
    else if (strstr(updateBuf, "�������"))
    {
        TextOut (hdc, 90, nTop, updateBuf);
        if (NULL == strstr(updateBuf,"�ļ��������100%"))
        {
            MEMZERO(updateEnd, sizeof(updateEnd));
        }
    }
    else
        TextOut (hdc, 90, nTop, "---");

    nTop += 20;
    TextOut (hdc, 10, nTop, "�������");
    if (strstr(statusInfoFresh.text,"����ɹ�"))
    {
        if (strcmp(updateEnd, statusInfoFresh.text)!= 0)
            strcpy(updateEnd, statusInfoFresh.text);
    }
    if (strstr(updateEnd,"����ɹ�"))
        TextOut (hdc, 90, nTop, updateEnd);
    else
        TextOut (hdc, 90, nTop, "---");

} 

//����״̬
PLUGIN_INFO g_UpdateStatusDraw = {
    0x20600002/*id*/,
    UpdateStatusInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    UpdateStatusDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

int wireflag;
void g_ChannelDebugStatusInit(DWORD dwPluginId)
{
    //memset(&DiagInfoDisp, 0, sizeof(WIRE_DIAG_INFO_T));
    memset(&ModuleInfoDisp, 0, sizeof(WIRELESS_INFO_T));
    memset(&TimeDisp, 0, sizeof(WIRELESS_TIME_T));
    uint8 sendbuf[1] = {1};
    gui_send_wirelessmsg(0, WIRELESS_IOP_GET_NUM, sendbuf, 1);
    gui_send_wirelessmsg(0, WIRELESS_IOP_GET_MODULEINFO, sendbuf, 1);
    //gui_send_wirelessmsg(0, WIRELESS_IOP_GET_DIAGINFO, sendbuf, 1);
    wireflag = 0;
    char cmd[200];
    char result[100];
    memset(cmd,0x00,sizeof(cmd));
    memset(result,0x00,sizeof(result));
    memset(g_GWIP,0x00,sizeof(g_GWIP));
    sprintf(cmd, "ifconfig %s | grep 'inet addr' | awk '{ print $2}' | awk -F: '{print $2}' ", "ppp0");
    cmd_execute(cmd,result);
    sscanf(result,"%u.%u.%u.%u",(uint32*)&g_GWIP[0],(uint32*)&g_GWIP[1],(uint32*)&g_GWIP[2],(uint32*)&g_GWIP[3]);
}

void ChannelDebugStatusDraw(HDC hdc)
{
	char temp[60];
	char buf[60];
    char czBuf[100] = {0};
	bzero(temp,sizeof(temp));
	int nOff = 5;
    if(wireflag == 0){
       sleep_s(1);
	   wireflag = 1;
	}
	TextOut(hdc,70,20+nOff,"ͨ��ͨ����Ϣ");
	TextOut(hdc,10,35+nOff,"GPRS����ʱ��");
	TextOut(hdc,10,50+nOff,"��ǿ�ź�");
	TextOut(hdc,10,65+nOff,"ģ���ͺ�");
	TextOut(hdc,10,80+nOff,"ģ��IMEI");
    TextOut(hdc,10,95+nOff,"����IP");

    time_t             timep = 0; 
    time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep
	uint32 successTimes = timep - TimeDisp.SuccessTimes;
	if (successTimes < 60)
		successTimes = 1;
	else
	    successTimes = successTimes/60;
	/*-GPRS����ʱ��-*/
	if(1 != toolInfoFresh.channel1.conStatus)
	{
		TextOut(hdc,100,35+nOff,"---");
	}
	else
	{
		sprintf(temp,"%u ��",successTimes);
		TextOut(hdc,100,35+nOff,temp);
	}

	bzero(temp, sizeof(temp));
	/*-��ǿ�ź�-*/
	if((toolInfoFresh.channel1.signalStrengh == 0) || (toolInfoFresh.channel1.signalStrengh == 65535))
	{
		TextOut(hdc,100,50+nOff,"---");
	}
	else
	{	
		sprintf(temp,"%d",toolInfoFresh.channel1.signalStrengh);		
		TextOut(hdc,100,50+nOff,temp);
	}
	/*-ģ����ͺ�-*/
	/*-ģ��IMEI-*/
	bzero(buf, sizeof(buf));
	bzero(temp, sizeof(temp));
	if(ModuleInfoDisp.instatus != 1)
	{			
		TextOut(hdc,100,65+nOff,"--------");
		TextOut(hdc,100,80+nOff,"--------");
	}
	else					// �ַ���ȡ				
	{
		memcpy(&temp, ModuleInfoDisp.version.manuExten, 8);
		TextOut(hdc,100,65+nOff,temp);
		sprintf(buf,"%s",ModuleInfoDisp.IMEI.value);		
		TextOut(hdc,100,80+nOff,buf);
	}
        //����IP

    memset(czBuf, 0x00, sizeof(czBuf));
    if (g_GWIP[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", g_GWIP[0], g_GWIP[1], g_GWIP[2], g_GWIP[3]);
    }
    TextOut(hdc, 100, 95+nOff, czBuf);
}
//ͨѶ��Ϣ
PLUGIN_INFO g_ChannelDebugStatusDraw = {
    0x20600003/*id*/,
    g_ChannelDebugStatusInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    ChannelDebugStatusDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

OOP_TASK_FAIL_NUM_LIST_T failNumList;
uint16 failIndex;
void MeterReadFailStatInit(DWORD dwPluginId)
{
    failIndex = 0;
    memset(&failNumList, 0, sizeof(failNumList));
    NOMAL_OAD_T normal = {0};
    normal.oad.value = 0xFF300B00;
    uint32 len = 0;
    normal.classtag = 5;
    normal.infoNum = 0;
    db_read_nomal(g_hUdbCenter, &normal, sizeof(OOP_TASK_FAIL_NUM_LIST_T), (uint8*)&failNumList, &len);
}

void MeterReadFailStatDraw(HDC hdc)
{
    int nTop = 20;
    char czBuf[100];
    SelectFont(hdc,g_guiComm.fontSmall);
    bzero(czBuf,sizeof(czBuf));
#ifdef AREA_CHONGQING
    snprintf(czBuf, sizeof(czBuf), "%u-%u %s", 3,4,
        g_guiMain.pPrentMenu->pPlugStruct->czCaption);
#else
     nTop = 10;
#endif
    TextOut(hdc, 10, nTop, czBuf);
    nTop+=20;
    TextOut(hdc, 8, nTop, "�������  �����ַ");

    for(uint16 i = 0;i+failIndex*6 < failNumList.FailNum && i < 6; i++){
        nTop+=14;
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,"%03d", failNumList.Pn[i+failIndex*6]);
        TextOut(hdc, 15, nTop, czBuf);
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,"%02X%02X%02X%02X%02X%02X", failNumList.FailAddr[i+failIndex*6][5],failNumList.FailAddr[i+failIndex*6][4],
        failNumList.FailAddr[i+failIndex*6][3], failNumList.FailAddr[i+failIndex*6][2], failNumList.FailAddr[i+failIndex*6][1],
        failNumList.FailAddr[i+failIndex*6][0]);
        TextOut(hdc, 55, nTop, czBuf);
    }
}

BOOL  MeterReadFailStatMsg(int nKey)
{
    PLUGIN_INFO* pPlugin;
    uint16 Maxindex_num;
    if (failNumList.FailNum%6 == 0 && failNumList.FailNum/6 >= 1)
        Maxindex_num = failNumList.FailNum/6 - 1;
    else
        Maxindex_num = failNumList.FailNum/6;
    switch(nKey)
    {
        case KEY_UP:
        case KEY_LEFT: 
            if(failIndex == 0)
               failIndex = Maxindex_num;
            else
               failIndex--;
           UpdateWindow(g_guiComm.hMainWnd, TRUE);
           return TRUE;
        case KEY_DOWN:
        case KEY_RIGHT: 
            if(failIndex == Maxindex_num)
               failIndex = 0;
            else
               failIndex++;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;

        case KEY_OK:
              pPlugin = GetPlugIn(0x0042000c);
                  if(pPlugin == NULL)
                {
                    ASSERT(FALSE);
                    return FALSE;
                }
               g_guiMain.pluginCur = pPlugin;
            if(g_guiMain.pluginCur->pfnInitLayer != NULL)
            {
               g_guiMain.pluginCur->pfnInitLayer(g_guiMain.pluginCur->dwId);
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
    }
    return FALSE;
}
    
PLUGIN_INFO g_MeterReadFailStat= {
    0x0041000c/*id*/, 
    MeterReadFailStatInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    MeterReadFailStatDraw/*Draw*/,
    NULL,
    MeterReadFailStatMsg/*keymsg*/,
    NULL/*ipc*/
};

#if DESC("������Ϣ", 1)
#ifdef AREA_CHONGQING
static char czBuf2[50] = {'\0'};
static char czBuf1[50] = {'\0'};
#endif
void GprsStatusDraw(HDC hdc)
{
    char czBuf[100] = {0};

    // ��״̬��Ϣ
    SelectFont(hdc,g_guiComm.fontBig);

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 20;
    rc.bottom = 36;
    DrawText(hdc, "��ǰͨ��������Ϣ", strlen("��ǰͨ��������Ϣ"), &rc, DT_CENTER);


#ifdef AREA_CHONGQING
    if (strstr(statusInfoFresh.text,"��̨") || strstr(statusInfoFresh.text,"����������"))
    {   
        bzero(czBuf1,sizeof(czBuf1));
        memcpy(czBuf1, statusInfoFresh.text, strlen(statusInfoFresh.text));
    }
    else
    {   bzero(czBuf2,sizeof(czBuf2));
        memcpy(czBuf2, statusInfoFresh.text, strlen(statusInfoFresh.text)); 
    }

    if(strlen(czBuf2) != 0)
    {
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,  "����: %s", czBuf2);
        TextOut(hdc,10,50,czBuf);
    }
    else
    {
        TextOut(hdc, 10, 50, "����: ------");
    }
    if(strlen(czBuf1) != 0)
    {
        bzero(czBuf,sizeof(czBuf));
        sprintf(czBuf,  "��̨: %s", czBuf1);
        TextOut(hdc, 10, 80, czBuf);
    }
    else
    {
        TextOut(hdc, 10, 80, "��̨: ------");
    }
#else
    rc.top = 80;
    rc.bottom = 96;
    strcpy(czBuf, statusInfoFresh.text);

    if(strlen(czBuf) != 0)
    {
        DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER);
    }
    else
    {
        DrawText(hdc, "--------", strlen("--------"), &rc, DT_CENTER);
    }
#endif
}
//������Ϣ
PLUGIN_INFO g_gprsStatusDraw = {
    0x20600004/*id*/,
    NULL/*InitLayer*/,
    NULL/*UninitLayer*/,
    GprsStatusDraw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("�ͻ����", 1)

void clientno_set_init(HDC hdc)
{   
    memset(ClientAddr, 0, sizeof(ClientAddr));
    get_clientno(ClientAddr);
}

void clientno_set_draw(HDC hdc)
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
    sprintf(czBuf, "�ն˿ͻ����");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, pg_guicomm->fontSmall);
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", ClientAddr[0], ClientAddr[1], ClientAddr[2], ClientAddr[3], ClientAddr[4], 
                                               ClientAddr[5]);
    rc.top += 30;
    rc.bottom = rc.top + 15;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
}

//ͨ�ŵ�ַ����
PLUGIN_INFO g_layer_clientno_set = {
    0x00B70000/*id*/,
    clientno_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    clientno_set_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("����״̬�鿴",1)
uint16 tsakID;
OOP_ACQ_MONITOR_T g_acq;

int get_task_status(uint16 id)
{
//    OOP_TASK_T oopTask;
    memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
    
//    if (app_get_task_list(id, &oopTask) != 0)
//        return 0;
    return read_db_noamal_data(g_hUdbCenter, 0x60340200, 0, id, (uint8*)&g_acq, sizeof(OOP_ACQ_MONITOR_T));
}

void taskStatusIdReturn(char* pNewStr, void* pPara)
{
    int value = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (value > 255 && TRUE == MessageBox("����ID����255\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {           
        sprintf(tmp, "%d", tsakID);
        InputBox(tmp, taskStatusIdReturn, NULL, 0x01);
    }
    else
    {
        if (value != 0){
            tsakID = value;
//          memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
            get_task_status(tsakID);
        }
    }
}

void task_status_init(DWORD dwPluginId)
{
    tsakID = 1;
//    memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
    get_task_status(tsakID);
    printf("taskid %d state %d \n", tsakID, g_acq.taskState);
    g_guiComm.bCanTrunDisp = FALSE;

}

void task_status_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;
    
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 80;
    int nLeft3 = 120;
    int nLeft4 = 190;
    char czBuf[64] = { 0 };
    TextOut(hdc, nLeft1, nTop, "����ID:");
    sprintf(czBuf, "%hu", tsakID);
    ITEM_SELECT;
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;

    TextOut(hdc, nLeft3, nTop, "����״̬:");
    TextOut(hdc, nLeft4, nTop, g_showtaskStatus[g_acq.taskState]);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�ɼ�����:");
    sprintf(czBuf, "%hu", g_acq.sum);
    TextOut(hdc, nLeft2, nTop, czBuf);

    TextOut(hdc, nLeft3, nTop, "�ɼ��ɹ���:");
    sprintf(czBuf, "%hu", g_acq.success);
    TextOut(hdc, nLeft4, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "���ͱ�����:");
    sprintf(czBuf, "%hu", g_acq.send);
    TextOut(hdc, nLeft2, nTop, czBuf);

    TextOut(hdc, nLeft3, nTop, "���ձ�����:");
    sprintf(czBuf, "%hu", g_acq.receive);
    TextOut(hdc, nLeft4, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��ʼʱ��:");
    sprintf(czBuf, "%02d-%02d %02d:%02d:%02d", g_acq.startTime.month, g_acq.startTime.day,
        g_acq.startTime.hour, g_acq.startTime.minute, g_acq.startTime.second);
    TextOut(hdc, nLeft2 - 10, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����ʱ��:");
    sprintf(czBuf, "%02d-%02d %02d:%02d:%02d", g_acq.endTime.month, g_acq.endTime.day,
        g_acq.endTime.hour, g_acq.endTime.minute, g_acq.endTime.second);
    TextOut(hdc, nLeft2 - 10, nTop, czBuf);

}

BOOL task_status_keymsg(int nKey)
{
    switch (nKey)
    {

    case KEY_UP:
    case KEY_LEFT:

        if (tsakID == 1)
        {
            tsakID = 255;
        }
        else
        {
            tsakID--;
        }
//        memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
        get_task_status(tsakID);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:
    case KEY_RIGHT:
        if (tsakID == 255)
        {
            tsakID = 1;
        }
        else
        {
            tsakID++;
        }
//        memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
        get_task_status(tsakID);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
        
    case KEY_OK:
        char bzbuf[50];
        sprintf(bzbuf, "%d", tsakID);
        InputBox(bzbuf, taskStatusIdReturn, NULL, 0x01);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }

    return FALSE;
}

//����״̬�鿴
PLUGIN_INFO g_layer_task_status =
{
    0x11040005/*id*/,
    task_status_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    task_status_draw/*Draw*/,
    NULL/*Timer*/,
    task_status_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("�㶨�ų������¼�",1)

OOP_EVENT_T g_magneticEvent;
OOP_EVENT_T *g_magneticEventlist;
void magneticEvent_display_init(DWORD dwPluginId)
{
    g_magneticEventlist = NULL;
    get_event_data(g_hUdbCenter, 0x302a0200, (uint8**)&g_magneticEventlist, sizeof(OOP_EVENT_T));
}
void magneticEvent_display_Uninit()
{
    if (g_magneticEventlist != NULL)
    {
        free(g_magneticEventlist);
        g_magneticEventlist = NULL;
    }
    
}

//�㶨�ų������¼�
void magneticEvent_display_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 85;
    int nTop = 22;
    int tmpTop = 25;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = nTop;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "�ų������¼�");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    memset(&g_magneticEvent, 0, sizeof(OOP_EVENT_T));
    if (g_magneticEventlist != NULL)
    {
        g_magneticEvent = g_magneticEventlist[g_event_index];
    }

    //�ų������¼�
    nTop += tmpTop + 10;
    TextOut(hdc, nLeft, nTop, "��¼���");
    sprintf(czBuf, "%u", g_magneticEvent.nIndex);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����ʱ��");
    if (g_magneticEvent.tmStart.year == 0 && g_magneticEvent.tmStart.month == 0 && g_magneticEvent.nIndex == 0)
    {
        sprintf(czBuf, "--��--��--��--:--");
    }
    else
    {
        sprintf(czBuf, "%04hu-%02hu-%02hu %02hu:%02hu:%02hu",
            g_magneticEvent.tmStart.year,
            g_magneticEvent.tmStart.month,
            g_magneticEvent.tmStart.day,
            g_magneticEvent.tmStart.hour,
            g_magneticEvent.tmStart.minute,
            g_magneticEvent.tmStart.second);
    }

    TextOut(hdc, nLeft2, nTop, czBuf);  
}
BOOL event_display_keymsg(int nKey)
{
    //g_event_num = 5;
    switch (nKey)
    {
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        g_event_index++;
        if ((uint32)g_event_index >= g_event_num)
        {
            g_event_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    case KEY_LEFT:
    {
        g_event_index--;
        if (g_event_index < 0)
        {
            g_event_index = (g_event_num==0)?0: g_event_num-1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}
//�㶨�ų������¼�
PLUGIN_INFO g_layer_magneticEvent_display = {
    0x00080000/*id*/,
    magneticEvent_display_init/*InitLayer*/,
    magneticEvent_display_Uninit/*UninitLayer*/,
    magneticEvent_display_draw/*Draw*/,
    NULL/*Timer*/,
    event_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("��ͨ�ɼ���������",1)
MEASURE_MAIN g_normal_plan_main_data;
MEASURE_MAIN    g_normal_plan_csd_data;
OOP_PLAN_NORMAL_T   g_guiNormalPlan;
//������ͨ������Ϣ��Ż�ȡ��ͨ�ɼ�����
int normal_plan_read(uint16 id,OOP_PLAN_NORMAL_T *PlanInfo)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60140200;
    NormalOad.infoNum = id;

    ret = db_read_nomal(g_hUdbCenter, &NormalOad, sizeof(OOP_PLAN_NORMAL_T), (uint8*)PlanInfo, &len);

    if(ret !=0 ||len!=sizeof(OOP_PLAN_NORMAL_T))
        return -1 ;
    return 0;
}

uint8       g_normalPlanMsType;

//ɾ����ͨ�ɼ�����
void delete_normal_plan(uint16 infonum)
{
    RECORD_OAD_T RecordDel;
    RecordDel.logicId = 0;
    RecordDel.infoNum = infonum;
    RecordDel.road.oadMain.value = 0;
    RecordDel.road.oadCols.nNum = 1;
    RecordDel.road.oadCols.oad[0] = NormalPlanOad;
    CLEAR_DATA_T clear;
    clear.recordOAD = RecordDel;
    clear.bClassTag = FALSE;
    int ret;
    ret = db_clear_oad_data(g_hUdbCenter, &clear);
    if (ret == 0)
    {
        GUI_FMT_DEBUG("��ͨ�ɼ�����ɾ���ɹ� infonum %d \n", infonum);
    }
    else
    {
        GUI_FMT_DEBUG("��ͨ�ɼ�����ɾ��ʧ�� infonum %d \n", infonum);
    }

}
//������ͨ�ɼ��������
void normal_PlanIdReturn(char* pNewStr, void* pPara)
{
    int nLen = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (nLen > 255 && TRUE == MessageBox("����ֵ����255\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        sprintf(tmp, "%d", g_guiComm.nCurPoint);
        InputBox(tmp, normal_PlanIdReturn, NULL, 0x01);
    }
    else
    {
        g_guiComm.nCurPoint = nLen;
    }
}


//���ô洢���
void MemDepthReturn(char* pNewStr, void* pPara)
{
    int nLen = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (nLen > 65535 && TRUE == MessageBox("���ô洢��ȴ���65535\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        sprintf(tmp, "%d", g_guiNormalPlan.memDepth);
        InputBox(tmp, MemDepthReturn, NULL, 0x01);
    }
    else
    {
        g_guiNormalPlan.memDepth = nLen;
    }
}

//����ֵ
void normalPlanValueReturn(char* pNewStr, void* pPara)
{
    int nLen = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (nLen > 65535 && TRUE == MessageBox("����ֵ����65535\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        if (g_guiNormalPlan.recdRow.choice == 1)
        {
            sprintf(tmp, "%d", g_guiNormalPlan.recdRow.num);
        }
        else if (g_guiNormalPlan.recdRow.choice == 3)
        {
            sprintf(tmp, "%d", g_guiNormalPlan.recdRow.ti.value);
        }
        else if (g_guiNormalPlan.recdRow.choice == 4)
        {
            sprintf(tmp, "%d", g_guiNormalPlan.recdRow.rd.ti.value);
        }
        InputBox(tmp, normalPlanValueReturn, NULL, 0x01);
    }
    else
    {
        if (g_guiNormalPlan.recdRow.choice == 1)
        {
            g_guiNormalPlan.recdRow.num = nLen;
        }
        else if (g_guiNormalPlan.recdRow.choice == 3)
        {
            g_guiNormalPlan.recdRow.ti.value = nLen;
        }
        else if (g_guiNormalPlan.recdRow.choice == 4)
        {
            g_guiNormalPlan.recdRow.rd.ti.value = nLen;
        }
    }
}

void normal_plan_set_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;

    nCurPoint = 0;
    g_normal_plan_main_data.ppDetail = NULL;
    g_normal_plan_main_data.pCurPlugin = NULL;
    g_normal_plan_main_data.nIndex = 0;
}

void normal_plan_set_draw(HDC hdc)
{
    // ������������
    if (g_normal_plan_main_data.pCurPlugin != NULL)
    {
        ASSERT(g_normal_plan_main_data.pCurPlugin->pfnDraw != NULL);
        g_normal_plan_main_data.pCurPlugin->pfnDraw(hdc);
        return;
    }


    if(nCurPoint != g_guiComm.nCurPoint)
    {
        memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
        normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
        g_normalPlanMsType = g_guiNormalPlan.ms.choice;
        set_gui_infonum((uint16)g_guiComm.nCurPoint);
        nCurPoint = g_guiComm.nCurPoint;
    }
    int flag = 0;  //��ʶ�����Ƿ����
    char czBuf[300] = { 0 };
    int index = 0;
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 5;
    int nLeft2 = 65;
    TextOut(hdc, nLeft1, nTop, "������ţ�");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    if (g_guiNormalPlan.planID != 0)
        flag = 1;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�洢��ȣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", g_guiNormalPlan.memDepth);
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�ɼ����ͣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.recdRow.choice;
        if (choice > 4)
            choice = 0;
        sprintf(czBuf, "%s", g_normalPlanType[choice]);
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "ֵ�뵥λ��");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.recdRow.choice;
        if (choice > 4 || choice == 0 || choice == 2)
            sprintf(czBuf, "%s", "-");
        else
        {
            if (choice == 1) //�ɼ��ϵ�N��
            {
                sprintf(czBuf, "%d", g_guiNormalPlan.recdRow.num);
            }
            else if (choice == 3) //��ʱ�����ɼ�
            {
                sprintf(czBuf, "%d", g_guiNormalPlan.recdRow.ti.value);
            }
            else //����
            {
                sprintf(czBuf, "%d", g_guiNormalPlan.recdRow.rd.ti.value);
            }
        }
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.recdRow.choice;
        if (choice > 4 || choice == 0 || choice == 2)
            sprintf(czBuf, "%s", "-");
        else
        {
            if (choice == 1) //�ɼ��ϵ�N��
            {
                sprintf(czBuf, "��");
            }
            else if (choice == 3) //��ʱ�����ɼ�
            {
                sprintf(czBuf, "%s", g_ti[g_guiNormalPlan.recdRow.ti.unit]);
            }
            else //����
            {
                sprintf(czBuf, "%s", g_ti[g_guiNormalPlan.recdRow.rd.ti.unit]);
            }
        }
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
    }
    

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��Ƽ��ϣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.ms.choice;
        if (choice > 7)
            choice = 0;
        sprintf(czBuf, "%s", g_ms[choice]);
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�洢ʱ�꣺");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.tmFlag;
        if (choice > 7)
            choice = 0;
        sprintf(czBuf, "%s", g_tmFlag[choice]);
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��ѡ�����");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "��������");
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }

    index++;
    nLeft2 = 50;
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "ɾ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "ɾ��");
    }

    index++;
    int nLeft3 = 90;
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "��������");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "��������");
    }

    PopListDraw(hdc);
}

BOOL normal_plan_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 10;

    if (g_normal_plan_main_data.pCurPlugin != NULL)
    {
        if (g_normal_plan_main_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_normal_plan_main_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }

        switch (nKey)
        {
            case KEY_CANCEL:
                if (g_normal_plan_main_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_normal_plan_main_data.pCurPlugin->pfnUninitLayer();
                }
                g_normal_plan_main_data.pCurPlugin = NULL;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                break;
        }
        return TRUE;
    }
    
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (g_normal_plan_main_data.nIndex)
            {
            case 2: //�ɼ�����
            {
                g_guiNormalPlan.recdRow.choice = nSel;
            }
            break;
            case 4: //��λ
            {
                g_guiNormalPlan.recdRow.ti.unit = nSel;
            }
            break;
            case 5: //�������
            {
            /**
                g_guiNormalPlan.ms.choice = nSel;
                if (nSel != 0 && nSel != 1) //����"�ޱ��"�Ҳ���"ȫ���û���ַ"
                {
                    g_normalPlanMsType = nSel;
                    g_normal_plan_main_data.ppDetail = g_normal_plan_ms_set_plugin;
                    g_normal_plan_main_data.nPluginCnt = sizeof(g_normal_plan_ms_set_plugin) / sizeof(g_normal_plan_ms_set_plugin[0]);
                    g_normal_plan_main_data.nCurPlugin = 0;
                    g_normal_plan_main_data.pCurPlugin = g_normal_plan_main_data.ppDetail[g_normal_plan_main_data.nCurPlugin];
                    ASSERT(g_normal_plan_main_data.pCurPlugin != NULL);

                    if (g_normal_plan_main_data.pCurPlugin->pfnInitLayer != NULL)
                    {
                        g_normal_plan_main_data.pCurPlugin->pfnInitLayer(g_normal_plan_main_data.pCurPlugin->dwId);
                    }
                }
            **/
            }
            break;
            case 6: //�洢ʱ��
            {
                g_guiNormalPlan.tmFlag = nSel;
            }
            break;
            default:
                return FALSE;
            }
            g_guiNormalPlan.planID = g_guiComm.nCurPoint;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch (nKey)
    {
    case KEY_LEFT:
    {
        if (g_guiComm.nCurPoint > 0)
            g_guiComm.nCurPoint--;
        if (g_guiComm.nCurPoint == 0)
            //g_guiComm.nCurPoint = MAX_PLAN_INFONUM;
            g_guiComm.nCurPoint = 255;
        memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
        normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
        set_gui_infonum(g_guiComm.nCurPoint);
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    {
        //if (g_guiComm.nCurPoint < MAX_PLAN_INFONUM)
        if (g_guiComm.nCurPoint < 255)
        {
            g_guiComm.nCurPoint++;
        }
        else
        {
            g_guiComm.nCurPoint = 1;
        }
        memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
        normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
        set_gui_infonum(g_guiComm.nCurPoint);
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_UP:
    {
        if (g_normal_plan_main_data.nIndex > 0)
            g_normal_plan_main_data.nIndex--;
        else
            g_normal_plan_main_data.nIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    {
        if (g_normal_plan_main_data.nIndex < maxindex)
            g_normal_plan_main_data.nIndex++;
        else
            g_normal_plan_main_data.nIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (g_normal_plan_main_data.nIndex == 0)
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            //InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            InputBox(bzbuf, normal_PlanIdReturn, NULL, 0x01);
            return TRUE;
        }
        
        //���ô洢���
        if (g_normal_plan_main_data.nIndex == 1)
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiNormalPlan.memDepth);
            InputBox(bzbuf, MemDepthReturn, NULL, 0x01);
            return TRUE;
        }
        //���òɼ�����
        if (g_normal_plan_main_data.nIndex == 2)
        {
            PopList(g_normalPlanType, 5, g_normalPlanType[0], 70, 40, FALSE);
            return TRUE;
        }
        //����ֵ
        if (g_normal_plan_main_data.nIndex == 3)
        {
            char bzbuf[50];
            if (g_guiNormalPlan.recdRow.choice == 1)
            {
                sprintf(bzbuf, "%d", g_guiNormalPlan.recdRow.num);
                InputBox(bzbuf, normalPlanValueReturn, NULL, 0x01);
            }
            else if (g_guiNormalPlan.recdRow.choice == 3)
            {
                sprintf(bzbuf, "%d", g_guiNormalPlan.recdRow.ti.value);
                InputBox(bzbuf, normalPlanValueReturn, NULL, 0x01);
            }
            else if (g_guiNormalPlan.recdRow.choice == 4)
            {
                sprintf(bzbuf, "%d", g_guiNormalPlan.recdRow.rd.ti.value);
                InputBox(bzbuf, normalPlanValueReturn, NULL, 0x01);
            }
            return TRUE;
        }
        //���õ�λ
        if (g_normal_plan_main_data.nIndex == 4 && (g_guiNormalPlan.recdRow.choice == 3 || g_guiNormalPlan.recdRow.choice == 4))
        {
            PopList(g_ti, 6, g_ti[0], 70, 40, FALSE);
            return TRUE;
        }
        //���ñ�Ƽ���
        //if (g_normal_plan_main_data.nIndex == 5)
        //{
        //    PopList(g_ms, 8, g_ms[0], 50, 40, FALSE);
        //    return TRUE;
        //}
        //���ô洢ʱ��
        if (g_normal_plan_main_data.nIndex == 6)
        {
            PopList(g_tmFlag, 8, g_tmFlag[0], 50, 40, FALSE);
            return TRUE;
        }

        //����csd
        if (g_normal_plan_main_data.nIndex == 7)
        {
            /**
            g_normal_plan_main_data.ppDetail = g_normal_plan_csd_set_plugin;
            g_normal_plan_main_data.nPluginCnt = sizeof(g_normal_plan_csd_set_plugin) / sizeof(g_normal_plan_csd_set_plugin[0]);
            g_normal_plan_main_data.nCurPlugin = 0;
            g_normal_plan_main_data.pCurPlugin = g_normal_plan_main_data.ppDetail[g_normal_plan_main_data.nCurPlugin];
            ASSERT(g_normal_plan_main_data.pCurPlugin != NULL);

            if (g_normal_plan_main_data.pCurPlugin->pfnInitLayer != NULL)
            {
                g_normal_plan_main_data.pCurPlugin->pfnInitLayer(g_normal_plan_main_data.pCurPlugin->dwId);
            }
            **/
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }

        //ȡ��
        if (g_normal_plan_main_data.nIndex == 8)
        {
            if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
            {
                ret = write_normal_data((uint8*)&g_guiNormalPlan, sizeof(OOP_PLAN_NORMAL_T), NormalPlanOad, g_guiComm.nCurPoint, 0, 2);
                if (ret == 0)
                {
                    MessageBox("��ǰ�������ñ���ɹ���");
                }
            }
            g_normal_plan_main_data.nIndex = 0;
            return FALSE;
        }
        //ɾ��
        if (g_normal_plan_main_data.nIndex == 9)
        {
            if (TRUE == MessageBox("ȷ��ɾ����ǰ�ɼ�������", MB_OKCANCEL))
            {
                delete_normal_plan(g_guiComm.nCurPoint);
                g_guiComm.nCurPoint = 1;
                normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            g_normal_plan_main_data.nIndex = 0;
            return FALSE;
        }
        //��������
        if (g_normal_plan_main_data.nIndex == 10)
        {
            GUI_FMT_DEBUG("write_normal_data %d\n", g_guiNormalPlan.ms.choice);
            ret = write_normal_data((uint8*)&g_guiNormalPlan, sizeof(OOP_PLAN_NORMAL_T), NormalPlanOad, g_guiComm.nCurPoint, 0, 2);
            if (ret == 0)
            {
                MessageBox("��ǰ�ɼ��������ñ���ɹ���");
                g_normal_plan_main_data.nIndex = 0;
                return TRUE;
            }
            else
            {
                MessageBox("��ǰ�ɼ��������ñ���ʧ�ܣ�");
                return FALSE;
            }
        }
    }
    }

    return FALSE;
}

//��ͨ��������
PLUGIN_INFO g_layer_normal_plan_set = {
    0x11030080/*id*/,
    normal_plan_set_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    normal_plan_set_draw/*Draw*/,
    NULL/*Timer*/,
    normal_plan_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

