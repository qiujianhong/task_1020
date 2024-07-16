/*
****************************************************************************
* Copyright (c) 2009,�Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* �������ƣ�commfun.c
* �� �� �ţ�1.0
* ��    �ܣ�ϵͳ����������
��������Ϊ������̬�⣬������ģ��ʹ�ã���Ҫ�ǽ����еĹ����������뱾ģ���ڣ������ظ�����
* ԭʼ�����˼�ʱ�䣺
*           gujun    2013-04-09
* �޸���Ա��ʱ�估�޸����ݣ�
* ������
* ע�⣺����̬�����ʵ��һЩ�������������漰�߼��������Բ��������ȫ����������̬����
****************************************************************************
*/

#ifndef _DLL_IMPLICIT_
#define _DLL_IMPLICIT_
#endif

#include "sys_cfg.h"
#include "commfun.h"
#include "hal.h"
#include "lcd.h"
#include "framecomm.h"
#include "storage.h"
#include "msg_id.h"
#include "appmsg_split.h"

//Ĭ������debug����ӡ
uint32 g_debug_enable = 0xffffffff;
//Ĭ������trace����ӡ
uint32 g_trace_enable = 0xffffffff;
//Ĭ������log����¼
uint32 g_log_enable = 0xffffffff;



extern LCD_DEVICE_T* app_dev;
char* commfun_TrimLeft(char* pBuf)
{
    while(*pBuf != '\0')
    {
        if(*pBuf == ' ')
        {
            pBuf ++;
        }
        else
        {
            break;
        }
    }
    return pBuf;
}


//��ȡ�ں˰汾��
BOOL commfun_GetCoreVersion(char* pBuf)
{
    char czBuf[300];
    char *Buf;
    FILE* pfsrc = fopen("/mnt/boot.sh", "r+");
    if(pfsrc == NULL)
    {
        return FALSE;
    }
    while(fgets(czBuf, 298, pfsrc) != NULL)
    {
        Buf = commfun_TrimLeft(czBuf);
        if(memcmp(Buf, "#OS_", 4) == 0)
        {
            sprintf(pBuf, "%s", &Buf[4]);
            fclose(pfsrc);
            return TRUE;
        }
    }
    fclose(pfsrc);
    return FALSE;
}

BOOL commfun_GetCoreTime(char* pBuf)
{
    char czBuf[300];
    char *Buf;
    int  i;
    FILE* pfsrc = fopen("/proc/version", "r+");

    if(pfsrc == NULL)
    {
        return FALSE;
    }
    while(fgets(czBuf, 298, pfsrc) != NULL)
    {
        Buf = commfun_TrimLeft(czBuf);

        i = 0;
        while (Buf[i] != '#')
        {
            i++;
        }
        i++;                                    //����#

        while(Buf[i] != ' ')
        {
            i++;
        }
        i++;                                    //�����ո�

        sprintf(pBuf, "%s", &Buf[i]);
        fclose(pfsrc);
        return TRUE;

    }
    fclose(pfsrc);
    return FALSE;
}

/*�ҺϷ��ı���68��ͷ����Ҫָ���뱨��*/
char *commfun_memchr_gb(char *buf, uint8 sync, uint32 buf_len)
{
    int i;
    char *head;

    if ((buf == NULL)||(buf_len <= 9))
        return NULL;

    head = buf;
    for(i=0; i<buf_len-9; i++)
    {
        if((*head == sync)&&(*(head+7) == sync)&&(*(head+8)&0x80))
        {
            return head;
        }
        head++;
    }
    return NULL;
}

/*�ҺϷ��ı���68��ͷ����Ҫָ���ͱ���*/
char *commfun_memchr_gb_send(char *buf, uint8 sync, uint32 buf_len)
{
    int i;
    char *head;

    if ((buf == NULL)||(buf_len <= 9))
        return NULL;

    head = buf;
    for(i=0; i<buf_len-9; i++)
    {
        if((*head == sync) && (*(head+7) == sync) && !(*(head+8)&0x80))
        {
            return head;
        }
        head++;
    }
    return NULL;
}

/*�ҺϷ��ı���68��ͷ�����ص�һ��68ͷ*/
char *commfun_memchr_gb_head(char *buf, uint8 sync, uint32 buf_len)
{
    int i;
    char *head;

    if ((buf == NULL)||(buf_len <= 9))
        return NULL;

    head = buf;
    for(i=0; i<buf_len-9; i++)
    {
        if((*head == sync)&&(*(head+7) == sync))
        {
            return head;
        }
        head++;
    }
    return NULL;
}

//��ȡ�м̲�����Ĳ����ʣ����м�����ʹ��
/*
uint8 commfun_GetMtrMode(mtrnum_t mtr_id)
{
	NOMAL_OAD_T nomalOAD;
	int nRet;
	uint32 readLen;
	OOP_METER_T tMeter;

    uint8 bSpeed = 0;
	nomalOAD.infoNum = mtr_id;
	nomalOAD.logicId = 0;
	nomalOAD.oad.value = 0x60000200;

	//nRet = db_read_nomal(g_hUdbCenter, &nomalOAD, sizeof(OOP_METER_T), (uint8 *)&tMeter ,&readLen);
	nRet = -1;
	if(nRet < 0 || readLen == 0)
		bSpeed = 2400 / 300;
	else
		bSpeed = tMeter.basic.baud;

    switch(bSpeed)
    {
    case 0:
        bSpeed = 0;
        break;
    case 2:
        bSpeed = 1;
        break;
    case 4:
        bSpeed = 2;
        break;
    case 8:
        bSpeed = 3;
        break;
    case 16:
        bSpeed = 4;
        break;
    case 24:
        bSpeed = 5;
        break;
    case 32:
        bSpeed = 6;
        break;
    case 64:
        bSpeed = 7;
        break;
    default:
        bSpeed = 3;
        break;
    }
    return (bSpeed << 5) + 0x0B;            //Ĭ��0x6B 2400 e 8 1
}*/


//�������ʱ�����ն˵�ǰʱ�����
time_t commfun_tmdiff(char *pointtime)
{
    struct tm   tMetertime;

    tMetertime.tm_year = commfun_BcdToHex(pointtime[5])+100;//��
    tMetertime.tm_mon  = commfun_BcdToHex(pointtime[4])-1;//��
    tMetertime.tm_mday = commfun_BcdToHex(pointtime[3]);
    tMetertime.tm_hour = commfun_BcdToHex(pointtime[2]);
    tMetertime.tm_min  = commfun_BcdToHex(pointtime[1]);
    tMetertime.tm_sec  = commfun_BcdToHex(pointtime[0]);

    return (time(NULL) - mktime(&tMetertime));
}

//ȡ�õ�ǰ��daydiff��ǰ������ʱ��
void commfun_GetLastNDate(int daydiff, time_t basetime, rtc_t *pLastNRtc)
{
    struct tm tLstLsttime;
    time_t curTime = basetime - (24 * 60 * 60 * daydiff);
    LOCALTIME_R(&curTime, &tLstLsttime);

    pLastNRtc->yy = commfun_HexToBcd((uint8)(tLstLsttime.tm_year - 100));   //��
    pLastNRtc->mm = commfun_HexToBcd((uint8)(tLstLsttime.tm_mon + 1));      //��
    pLastNRtc->dd = commfun_HexToBcd((uint8)tLstLsttime.tm_mday);           //��
    pLastNRtc->hh = commfun_HexToBcd((uint8)tLstLsttime.tm_hour);           //ʱ
    pLastNRtc->nn = commfun_HexToBcd((uint8)tLstLsttime.tm_min);            //��
    pLastNRtc->ss = commfun_HexToBcd((uint8)tLstLsttime.tm_sec);            //��
}

void commfun_selectsleep(unsigned sec, unsigned us)
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = us;
    select(0,0,0,0,&tv);
}

/*
  ��  ��: getNextPath
  ��  ��: ���src�ַ����еĵ�һ��Ŀ¼·��
  ��  ��:
    src    - ������ַ����׵�ַ
    bufLen - ����������
  ��  ��:
    dirName - ���ص�Ŀ¼����('\0'��β),����ʱ���㹻�ռ�
  ����ֵ:
            !NULL - ��һ��Ŀ¼·������ʼ��ַ
            NULL  - ��ʾ����������

 */
inline static char *getNextPath(const char *src, const char *dirName, unsigned bufLen)
{
    int     cnt  = 0;
    char *pdst, *ptr;

    if((NULL == src) || (NULL == dirName)) return NULL;

    pdst = (char *)dirName;

    for(ptr = (char *)src; (*ptr != '\0') && (*ptr == '/'); ptr++); //ȥ�������·����ǰ��'/'

    while( (*ptr != '\0') && (cnt < bufLen - 1) && (*ptr != '/') ) {
        *pdst++ = *ptr++;
        cnt++;
    }
    *pdst = '\0';

    if(*ptr == '\0')
        return NULL;

    return ptr;
}

/*
  ��  ��: commfun_makePathedDir
  ��  ��: ����path�ַ���ָ����·�����������Ŀ¼
  ��  ��:
        path - �����ɵ�·��Ŀ¼(����·��)
  ��  ��:
  ����ֵ:

 */
BOOL commfun_makePathedDir(const char *path)
{
    char        *ptr;
    char        filepath[PATH_MAX + 1];
    char        dirName[128];
    BOOL      bRelative = FALSE;

    ptr = (char *)path;
    if(NULL == path) return FALSE;

    if('/' != *ptr)
        bRelative = TRUE;

    if( (ptr = getNextPath(ptr, dirName, 128)) != NULL)  //��ø�Ŀ¼
    {
        if(!bRelative) // ����������Ǿ���·�������ڴ���Ŀ¼��ǰ��'/'��ʾ����·��
            strcpy(filepath, "/");
        else
            filepath[0] = '\0';

        strcat(filepath, dirName);

        if( mkdir(filepath, 0770) < 0)
            if( errno != EEXIST)
                return FALSE;

    }
    else //��Ŀ¼��ȡ����
        return FALSE;

    while( (ptr = getNextPath(ptr, dirName, 128) ) != NULL)
    {
        strcat(filepath, "/");
        strcat(filepath, dirName);

        if( mkdir(filepath, 0770) < 0)
            if(errno != EEXIST)
                return FALSE;

    }

    if( *dirName != '\0')
    {
        strcat(filepath, "/");
        strcat(filepath, dirName);

        if( mkdir(filepath, 0777) < 0)
            if(errno != EEXIST)
                return FALSE;

    }

    return TRUE;
}

/*
  ��  ��: FileExists
  ��  ��: ����ļ��Ƿ����
  ��  ��:
            filepath - �����ļ��Ƿ����
  ��  ��:

  ����ֵ:
            TRUE    - ����
            FALSE - ������
 */
BOOL commfun_FileExists(const char *filepath)
{
    struct stat sta;

    if( stat(filepath, &sta) < 0)
        return FALSE;

    if( S_ISREG(sta.st_mode))
        return TRUE;

    return FALSE;
}

/*
  ��  ��: commfun_PathExists
  ��  ��: ���Ŀ¼�Ƿ����
  ��  ��:
            filepath - �����ļ��Ƿ����
  ��  ��:

  ����ֵ:
            TRUE    - ����
            FALSE - ������
 */
BOOL commfun_PathExists(const char *pathname)
{
    struct stat sta;

    if( stat(pathname, &sta) < 0)
        return FALSE;

    if( S_ISDIR(sta.st_mode))
        return TRUE;

    return FALSE;
}

/*
  ��  ��: commfun_ExtractFilePath
  ��  ��: �Ӱ���������·�����ļ�������ȡ����·��
  ��  ��:
    src  - Դ�ַ���
  ��  ��:
    path - Ŀ��д���ַ���
  ����ֵ:
            -1 - ʧ��
            0    - �ɹ�
 */
int commfun_ExtractFilePath(const char *src, char *path)
{
    int     len, frag;
    char    *off;

    if( !src || !path)
        return -1;

    len = strlen(src);
    off = (char *)&src[len - 1];//-ȥ��'\0'-
    frag = 0;
    while((off != &src[0]) && (*off != '/'))//-�ļ�·�����Ӻ���ǰ����ȡ·����-
    {
        off--;
        frag++;
    }

    strncpy(path, src, len - frag);  //!������������Զ����ַ��������'\0'
    path[len - frag] = '\0';
    return 0;
}

int commfun_GetFileSize(const char *filename)
{
    int fd=-1;
    int filesize=-1;
    struct stat sta;
    if( (fd = open(filename, O_RDONLY)) < 0)
    {
        return -1;
    }
    if(fstat(fd, &sta) < 0)
    {
        close(fd);
        return -1;
    }
    filesize=sta.st_size;
    close(fd);
    return filesize;
}


/*******************************************************************************
* ��������: CalWeekday
* ��������: ��ķ����ɭ�������ڹ�ʽ
* �������: int y   ��(��:2008)
*           int m   ��
*           int d   ��
* �������: ��
* �� �� ֵ:  ����(1-7) 1-6 :����һ~��; 7:������
*******************************************************************************/
uint8 CalWeekday(int y, int m, int d)
{
    if (m==1 || m==2)
    {
        m += 12;
        y--;
    }

    uint8 w = (d+1 + 2*m + 3*(m+1)/5 + y + y/4 - y/100 + y/400)%7;

    return (w == 0) ? 7 : w;
}

//����Ƿ����U��
int commfun_CanUseUDisk(void)
{
    FILE* pf = fopen("/dev/sda1", "r+");
    if(pf == NULL)
    {
        return 1;// U��û�в���
    }
    fclose(pf);

    BOOL bUsb = FALSE;
    pf = fopen("/proc/mounts", "r+");
    if(pf == NULL)
    {
        //MessageBox("��ȡU�̹��ص�ʧ��\n���ܼ�������!");
        return 2;
    }
    char czBuf[300];
    char* pBuf;
    while(fgets(czBuf, 298, pf) != NULL)
    {
        pBuf = czBuf;
        while(*pBuf != '\0')
        {
            if(*pBuf != ' ')
            {
                pBuf ++;
            }
            else
            {
                pBuf ++;
                break;
            }
        }
        if(*pBuf == '\0' || strlen(pBuf) < 14)
        {
            continue;
        }

        //printf(pBuf);
        if(memcmp(pBuf, "/var/tmp/udisk", 14) == 0)
        {
            bUsb = TRUE;
            break;
        }
    }
    fclose(pf);
    // 1��ʾû�й���U�̣�0��ʾU���Ѿ�����
    return (bUsb==TRUE)?0:1;
}

/**********************************************************
��������:RebootByTaskd
        ͨ��taskd�����ն�
��������ֵ:��
��������ֵ:��
*************************************************************/
void RebootByTaskd(void)
{
       uint16 aTime = 0;
       //DMSG    message;

    /* ���ػ����̷�����Ϣ�������ն� */
    //message.mtype = DMSG_REBOOT;
    //message.data0 = 0xFE;
    //SendDMsg(&message);

    for (aTime = 0; aTime < 30; aTime++)
    {
        //KeepMeAlive();      //�ػ����̼���
        sleep(1);
    }   //�ȴ�10���û���������Զ�����
    reboot(RB_AUTOBOOT);
    return;
}

int lighton_lcd(void)
{
    int ret = -1;
    if(app_dev != NULL)
	{
		ret = app_dev->lcd_bkl_set(app_dev, LCD_BKL_ON);
	}
	
	GUI_FMT_DEBUG("lighton_lcd ret %d\n", ret);
	return ret;
}


int lightoff_lcd(void)
{
    int ret = -1;
    if(app_dev != NULL)
	{
		ret = app_dev->lcd_bkl_set(app_dev, LCD_BKL_OFF);
	}
	
	GUI_FMT_TRACE("lightoff_lcd ret %d\n", ret);
	return ret;

}
