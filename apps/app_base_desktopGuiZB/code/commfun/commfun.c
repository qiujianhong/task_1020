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
#include "appoutmsg.h"
#include "msg_id.h"
#include "appmsg_split.h"
#include "db_api.h"
#include "out_msg.h"

//Ĭ������debug����ӡ
uint32 g_debug_enable = 0xffffffff;
//Ĭ������trace����ӡ
uint32 g_trace_enable = 0xffffffff;
//Ĭ������log����¼
uint32 g_log_enable = 0xffffffff;

extern LCD_DEVICE_T* gui_dev;
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

    return (time(NULL) - FastDateToSecond(&tMetertime));
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

/****************************************************************************
* ��������: commfun_msqrt
* ��������: ��ƽ����
* �������: x  ��������
* �������: ��
* �� �� ֵ: ƽ����
****************************************************************************/
double commfun_msqrt(double x)
{
    double s = 0.0, last = 0.0;
    DoubleUnionType xx;
    int i = 0;

    if (x > 0)
    {
        xx.d = x;
        i = xx.df.exp;
        if(i == 1023 )
        {
            // e0
            s = x;
        }
        else if(i > 1023)
        {
            i = (i - 1023) / 2 + 1023;
            xx.df.exp = i + 1;
            s = xx.d;
        }
        else if(i == 1022  )
        {
            // e-1
            s = 1;
        }
        else
        {
            i = (i - 1023) / 2 + 1023;
            xx.df.exp = i ;
            s = xx.d;
        }
        do
        {
            last = s;
            s = (x / s + s) / 2;
        }
        while(s < last);

        return last;
    }

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
    if(gui_dev != NULL)
	{
		ret = gui_dev->lcd_bkl_set(gui_dev, LCD_BKL_ON);
	}

//    GUI_FMT_DEBUG("lighton_lcd ret %d\n", ret);
	return ret;
}


int lightoff_lcd(void)
{
    int ret = -1;
	if(gui_dev != NULL)
	{
		ret = gui_dev->lcd_bkl_set(gui_dev, LCD_BKL_OFF);
	}
	
//	GUI_FMT_TRACE("lightoff_lcd ret %d\n", ret);
	return ret;

}

struct manual_amr_result_struc
{
    uint8   protocol;   //��Լ����
    uint8   piid;       //piid
    char    state[16];  //���ؽ��״̬
    char    time[16];   //���ؽ��ʱ��
    char    data[16];   //���ؽ������(�����й�)
};
extern struct manual_amr_result_struc g_manual_amr_rst;

/**********************************************************************
* @name      : systime_set_to_smiOS
* @brief     ����ϵͳ���������Ͷ�ʱ����
* @param[in] ��
* @return    ��
* @Date      ��2020-3-18
* @Update    :
**********************************************************************/
int gui_mannual_amr_send(uint8* playload, uint16 length)
{
    int    ret = 0;
	uint8  sendbuf[101] = {0};
    uint16 msgindex=0;
	if(length >100)
	{
		length = 100;
	}

	sendbuf[0] = length;
	memcpy(sendbuf+1, playload, length);

	ret = gui_send_ctsmsg(0, CTS_IOP_READ_ONE_REQ, sendbuf, length+1,&msgindex);
    if (ret != ERR_OK)
    {
        GUI_FMT_TRACE("gui_mannual_amr_send err!\n");
    }
    
    return ret;
}

void gui_mannual_amr_rcv(uint8 *rptmsg)
{
    if (rptmsg[3] == g_manual_amr_rst.piid && rptmsg[8] == 0) //����
    {
        sprintf(g_manual_amr_rst.state, "������%d", rptmsg[9]);
    }
    else if (rptmsg[3] == g_manual_amr_rst.piid)
    {
        if (rptmsg[9] != 0)    //����
        {
            uint32 value;
            sprintf(g_manual_amr_rst.state, "�ɹ�");
            
            if (g_manual_amr_rst.protocol == 3)  //698Э��
            {
                //value = rptmsg[53] * 256 * 256 * 256 + rptmsg[54] * 256 * 256 + rptmsg[55] * 256 + rptmsg[56];
                memcpy_r(&value,&rptmsg[35],4);
            }
            else
            {
                uint8 manualArmValue[4];
                manualArmValue[0] = rptmsg[24] - 0x33;
                manualArmValue[1] = rptmsg[25] - 0x33;
                manualArmValue[2] = rptmsg[26] - 0x33;
                manualArmValue[3] = rptmsg[27] - 0x33;
                value = BCD_change_to_long(manualArmValue, 8);
                GUI_FMT_DEBUG("manualArmValue = %d %d %d %d, value = %d\n", manualArmValue[0], manualArmValue[1],
                    manualArmValue[2], manualArmValue[3], value);
            }
            sprintf(g_manual_amr_rst.data, "%.2f kWh", value / 100.0);
        }
        else
        {
            sprintf(g_manual_amr_rst.state, "δ��ͨ");
            sprintf(g_manual_amr_rst.data, "--");
        }
    }

    if (rptmsg[/*16*/3] == g_manual_amr_rst.piid)
    {
        time_t tm;
        tm = time(NULL);
        strftime(g_manual_amr_rst.time, sizeof(g_manual_amr_rst.time), "%H:%M:%S", localtime((time_t*)&tm));
    }
}

/**********************************************************************
* @name      : systime_set_to_smiOS
* @brief     ����ϵͳ���������Ͷ�ʱ����
* @param[in] ��
* @return    ��
* @Date      ��2020-3-18
* @Update    :
**********************************************************************/
int systime_set_to_smiOS(DateTimeHex_t pDateTime)
{
    uint8  sendbuf[100] = {0};
    uint16 offset = 0;
    int    ret = 0;

    struct tm systime;
    struct  timeval tv;
    struct  timezone tz;
    uint16  year;
    uint16  msec;

    sendbuf[offset++] = pDateTime.year_h;
    sendbuf[offset++] = pDateTime.year_l;
    sendbuf[offset++] = pDateTime.month;
    sendbuf[offset++] = pDateTime.day;
    sendbuf[offset++] = 0;
    sendbuf[offset++] = pDateTime.hour;
    sendbuf[offset++] = pDateTime.min;
    sendbuf[offset++] = pDateTime.sec;
    sendbuf[offset++] = 0;
    sendbuf[offset++] = 0;

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &systime);
    
    year = systime.tm_year + 1900;
    MEMCPY_R(sendbuf + offset, &year, 2);
    offset += 2;
    sendbuf[offset++] = systime.tm_mon + 1;
    sendbuf[offset++] = systime.tm_mday;
    sendbuf[offset++] = systime.tm_wday;
    sendbuf[offset++] = systime.tm_hour;
    sendbuf[offset++] = systime.tm_min;
    sendbuf[offset++] = systime.tm_sec;
    msec = tv.tv_usec / 1000;
    MEMCPY_R(sendbuf + offset, &msec, 2);
    offset += 2;

	ret = gui_send_smios(0, SYSTEM_IOP_RCT, sendbuf, offset);
    if (ret != ERR_OK)
    {
        GUI_FMT_TRACE("systime_set_to_smiOS err!\n");
    }
    
    return ret;
}


/**********************************************************************
* @name      : addr_send_to_radio
* @brief     �����̨�����޸ĵ�ַ����
* @param[in] ��
* @return    ��
* @Date      ��2020-3-18
* @Update    :
**********************************************************************/
int addr_send_to_radio(uint8* czCode, uint8* czAddr, uint8* oopAddr)
{
    uint8  sendbuf[100] = {0};
    uint16 offset = 0;
    int    ret = 0;
    int    nRet = 0;
    
    sendbuf[offset++] = 2;
    memcpy(sendbuf + offset, czCode, 2);
    offset += 2;

	sendbuf[offset++] = 3;
    memcpy(sendbuf + offset, czAddr, 3);
    offset += 3;

	sendbuf[offset++] = 6;
    memcpy(sendbuf + offset, oopAddr, 6);
    offset += 6;

	ret = gui_send_radio(0, RADIO_IOP_SET_ADDR, (uint8*)&sendbuf, offset);
    if(0 != ret)
    {
        GUI_FMT_DEBUG("addr_send_to_radio err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}

/**********************************************************************
* @name      : config_send_to_radio
* @brief     �����̨�����޸���������
* @param[in] ��
* @return    ��
* @Date      ��2020-3-18
* @Update    :
**********************************************************************/
int config_send_to_radio(uint8 channel, uint8 power, uint8 sensitivity)
{
    uint8  sendbuf[100] = {0};
    uint16 offset = 0;
    int    ret = 0;
    int    nRet = 0;
    
    sendbuf[offset++] = channel;

	sendbuf[offset++] = power;

	sendbuf[offset++] = sensitivity;
	ret = gui_send_radio(0, RADIO_IOP_SET_CONFIG, (uint8*)&sendbuf, offset);
    if(0 != ret)
    {
        GUI_FMT_DEBUG("config_send_to_radio err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        GUI_BUF_DEBUG(sendbuf, offset, "config_send_to_radio ok! ret = %d\n", ret);
        nRet = ERR_OK;
    }
    
    return nRet;
}

/**********************************************************************
* @name      : config_send_to_radio
* @brief     �����̨������������������
* @param[in] ��
* @return    ��
* @Date      ��2020-3-18
* @Update    :
**********************************************************************/
int yang_send_to_radio(uint8 power)
{
    uint8  sendbuf[100] = {0};
    uint16 offset = 0;
    int    ret = 0;
    int    nRet = 0;
    

	sendbuf[offset++] = power;

	ret = gui_send_radio(0, RADIO_IOP_SET_YANG, (uint8*)&sendbuf, offset);
    if(0 != ret)
    {
        GUI_FMT_DEBUG("config_send_to_radio err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}
/**********************************************************************
* @name      : config_send_to_radio
* @brief     �����̨������������������
* @param[in] ��
* @return    ��
* @Date      ��2020-3-18
* @Update    :
**********************************************************************/
int modem_version_get_for_radio()
{
    int    ret = 0;
    int    nRet = 0;

//	char  sendbuf[101] = {0};
//    uint8  sendbuf1[101] = {0};
//    uint16 sendlen=0;
//    uint8  byte[5] = {0};
//    uint8  bytenum = 0;
//
//
//    sprintf(sendbuf,"AT+VER=?\r");
//
//    SET_LEN_OFFSET(strlen(sendbuf), byte, bytenum);
//    memcpy(sendbuf1 + sendlen, byte, bytenum);
//    sendlen += bytenum;
//    memcpy(sendbuf1 + sendlen, sendbuf, strlen(sendbuf));
//    sendlen += strlen(sendbuf);

	ret = gui_send_radio(0, RADIO_IOP_GET_VERSION, NULL, 0);
    if(0 != ret)
    {
        GUI_FMT_DEBUG("RADIO_IOP_SEND_DATA! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}
/**********************************************************************
* @name      : config_send_to_radio
* @brief     �����̨������������������
* @param[in] ��
* @return    ��
* @Date      ��2020-3-18
* @Update    :
**********************************************************************/
int config_get_for_radio()
{
    int    ret = 0;
    int    nRet = 0;


	ret = gui_send_radio(0, RADIO_IOP_GET_CONFIG, NULL, 0);
    if(0 != ret)
    {
        GUI_FMT_DEBUG("config_get_for_radio err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}


/**********************************************************************
* @name      : long_send_to_radio
* @brief     �����̨���ͳ�������
* @param[in] ��cmd��0.����ȫ1 1.�رճ���ȫ1 2.����ȫ0 3.�رճ���ȫ0
* @return    ��
* @Date      ��2023-6-14
* @Update    :
**********************************************************************/
int long_send_to_radio(uint8 cmd)
{
    uint8  sendbuf[100] = {0};
    uint16 offset = 0;
    int    ret = 0;
    int    nRet = 0;
    
    switch(cmd)
    {
    case 0:
    {
        sendbuf[offset++] = 0x11;
        sendbuf[offset++] = 0x00;
        sendbuf[offset++] = 0x1e;
        GUI_FMT_DEBUG("����ȫ1");
    }
    break;
    case 1:
    {
        sendbuf[offset++] = 0x11;
        sendbuf[offset++] = 0x00;
        sendbuf[offset++] = 0x00;  //ʱ��Ϊ0��ʾ�رճ���
        GUI_FMT_DEBUG("�رճ���ȫ1");
    }
    break;
    case 2:
    {
        sendbuf[offset++] = 0x22;
        sendbuf[offset++] = 0x00;
        sendbuf[offset++] = 0x1e;
        GUI_FMT_DEBUG("����ȫ0");
    }
    break;
    case 3:
    {
        sendbuf[offset++] = 0x22;
        sendbuf[offset++] = 0x00;
        sendbuf[offset++] = 0x00;  //ʱ��Ϊ0��ʾ�رճ���
        GUI_FMT_DEBUG("�رճ���ȫ0");
    }
    break;
    }
	sendbuf[offset++] = 0x11;

	ret = gui_send_radio(0, RADIO_IOP_LONG_SENG, (uint8*)&sendbuf, offset);
    if(0 != ret)
    {
        GUI_FMT_DEBUG("long_send_to_radio err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}

/**********************************************************************
* @name      : ipconfig_to_smiOS
* @brief     ����ϵͳ�����������޸�IP����
* @param[in] ��
* @return    ��
* @Date      ��2020-3-18
* @Update    :
**********************************************************************/
int ipconfig_send_to_smiOS(char* Name, uint8 type, uint8* IP, uint8* NetMask, uint8* NetGw)
{
    uint8  sendbuf[100] = {0};
    uint16 offset = 0;
    int    ret = 0;
    int    nRet = 0;
    int len = strlen(Name);

    GUI_FMT_DEBUG("Set IP type %d\n", type);
    if(type > 1)
    {
        return -1;
    }
    
    sendbuf[offset++] = len;
    memcpy(sendbuf + offset, Name, len);
    offset += len;
    sendbuf[offset++] = type;  //�°汾����type
    if(1 == type)
    {
        /* IP */
        sendbuf[offset++] = 4;
        memcpy(sendbuf + offset, IP, 4);
        offset += 4;
    
        /* mask */
        sendbuf[offset++] = 4;
        memcpy(sendbuf + offset, NetMask, 4);
        offset += 4;
    
        /* getway */
        sendbuf[offset++] = 4;
        memcpy(sendbuf + offset, NetGw, 4);
        offset += 4;

        GUI_BUF_DEBUG(sendbuf, offset, "Set IP: %d.%d.%d.%d:%d.%d.%d.%d, getway %d.%d.%d.%d\n",
                       IP[0], IP[1], IP[2], IP[3], NetMask[0], NetMask[1], NetMask[2], NetMask[3], 
                       NetGw[0], NetGw[1], NetGw[2], NetGw[3]);
    }

	ret = gui_send_smios(0, SYSTEM_IOP_SET_IPADDR, (uint8*)&sendbuf, offset);
    if(0 != ret)
    {
        GUI_FMT_DEBUG("ipconfig_to_smiOS err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}

/**********************************************************************
* @name      : param_init_sent_to_acmeter
* @brief     ���򽻲�ģ�鷢�Ͳ�����ʼ��
* @param[in] ��
* @return    ��
* @Date      ��2020-3-18
* @Update    :
**********************************************************************/
int param_init_sent_to_acmeter(void)
{
    /*uint8  recvbuf[32] = {0};
    uint16 recLen = 0;*/
    int    ret = 0;
    int    nRet = 0;

    ret = gui_send_acmeter(0, MESSAGE_IID_ACMETER, ACMETER_IOP_AC_PARM_INIT,  NULL, 0);
    if ((ret != ERR_OK) /*|| (recLen != 1) || (recvbuf[0] != 1)*/)
    {
        PRTL_FMT_DEBUG("param_init_sent_to_acmeter err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}

/**********************************************************************
* @name      : data_init_sent_to_acmeter
* @brief     ���򽻲�ģ�鷢�Ͳ�����ʼ��
* @param[in] ��
* @return    ��
* @Date      ��2020-3-18
* @Update    :
**********************************************************************/
int data_init_sent_to_acmeter(void)
{
    /*uint8  recvbuf[32] = {0};
    uint16 recLen = 0;*/
    int    ret = 0;
    int    nRet = 0;

    ret = gui_send_acmeter(0, MESSAGE_IID_ACMETER, ACMETER_IOP_AC_DATA_INIT,  NULL, 0);
    if ((ret != ERR_OK) /*|| (recLen != 1) || (recvbuf[0] != 1)*/)
    {
        PRTL_FMT_DEBUG("data_init_sent_to_acmeter err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}

int RebootTerminal()
{
//	char  sendbuf[100] = {0};
//    char  recvbuf[32] = {0};
//    uint16 recLen = 0;
    int    ret = 0;
    int    nRet = 0;

//	sendbuf[0] = 0;
    ret = gui_send_smios(0, SYSTEM_IOP_REQUIRE_RESTART, NULL, 0);
//	ret = gui_send_syncmsg(MSG_GUI_REBOOT, (uint8*)&sendbuf, 1, (uint8*)&recvbuf, 32, &recLen);
    if ((ret != ERR_OK) )
    {
       GUI_FMT_DEBUG("Reboot_set_to_smiOS err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}

//AT�����¼�����
//sign = 0,�ر�;sign = 1,��
int SetATinteractiveEvent(char Sign)
{
	char sendbuf[10] = {0};
	char  recvbuf[32] = {0};
    uint16 recLen = 0;
    int    ret = 0;
    int    nRet = 0;

	sendbuf[0] = Sign;

	ret = gui_send_syncmsg(MSG_GUI_AT_SWITCH, (uint8*)&sendbuf, 1, (uint8*)&recvbuf, 32, &recLen);
    if ((ret != ERR_OK) || (recLen != 1) || (recvbuf[0] != 1))
    {
		GUI_FMT_DEBUG("set AT_Event err! ret = %d, reclen = %d\n", ret, recLen);
		nRet = ERR_NORMAL;
    }
    else
    {
		nRet = ERR_OK;
    }
    
    return nRet;
}
