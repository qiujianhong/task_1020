/*
****************************************************************************
* Copyright (c) 2009,南京新联电子股份有限公司
* All rights reserved.
* 程序名称：commfun.c
* 版 本 号：1.0
* 功    能：系统公共函数库
本函数库为单独动态库，供其他模块使用，主要是将所有的公共函数放入本模块内，减少重复代码
* 原始开发人及时间：
*           gujun    2013-04-09
* 修改人员、时间及修改内容：
* 其他：
* 注意：本动态库仅仅实现一些公共函数，不涉及逻辑处理，所以不允许出现全部变量、静态变量
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

//默认所有debug都打印
uint32 g_debug_enable = 0xffffffff;
//默认所有trace都打印
uint32 g_trace_enable = 0xffffffff;
//默认所有log都记录
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


//获取内核版本号
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
        i++;                                    //跳过#

        while(Buf[i] != ' ')
        {
            i++;
        }
        i++;                                    //跳过空格

        sprintf(pBuf, "%s", &Buf[i]);
        fclose(pfsrc);
        return TRUE;

    }
    fclose(pfsrc);
    return FALSE;
}

/*找合法的报文68开头，主要指回码报文*/
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

/*找合法的报文68开头，主要指发送报文*/
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

/*找合法的报文68开头，返回第一个68头*/
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

//获取中继测量点的波特率，供中继命令使用
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
    return (bSpeed << 5) + 0x0B;            //默认0x6B 2400 e 8 1
}*/


//计算给定时间与终端当前时间的秒
time_t commfun_tmdiff(char *pointtime)
{
    struct tm   tMetertime;

    tMetertime.tm_year = commfun_BcdToHex(pointtime[5])+100;//年
    tMetertime.tm_mon  = commfun_BcdToHex(pointtime[4])-1;//月
    tMetertime.tm_mday = commfun_BcdToHex(pointtime[3]);
    tMetertime.tm_hour = commfun_BcdToHex(pointtime[2]);
    tMetertime.tm_min  = commfun_BcdToHex(pointtime[1]);
    tMetertime.tm_sec  = commfun_BcdToHex(pointtime[0]);

    return (time(NULL) - mktime(&tMetertime));
}

//取得当前第daydiff天前的日期时标
void commfun_GetLastNDate(int daydiff, time_t basetime, rtc_t *pLastNRtc)
{
    struct tm tLstLsttime;
    time_t curTime = basetime - (24 * 60 * 60 * daydiff);
    LOCALTIME_R(&curTime, &tLstLsttime);

    pLastNRtc->yy = commfun_HexToBcd((uint8)(tLstLsttime.tm_year - 100));   //年
    pLastNRtc->mm = commfun_HexToBcd((uint8)(tLstLsttime.tm_mon + 1));      //月
    pLastNRtc->dd = commfun_HexToBcd((uint8)tLstLsttime.tm_mday);           //日
    pLastNRtc->hh = commfun_HexToBcd((uint8)tLstLsttime.tm_hour);           //时
    pLastNRtc->nn = commfun_HexToBcd((uint8)tLstLsttime.tm_min);            //分
    pLastNRtc->ss = commfun_HexToBcd((uint8)tLstLsttime.tm_sec);            //分
}

void commfun_selectsleep(unsigned sec, unsigned us)
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = us;
    select(0,0,0,0,&tv);
}

/*
  名  称: getNextPath
  功  能: 获得src字符串中的第一个目录路径
  输  入:
    src    - 输入的字符串首地址
    bufLen - 缓冲区长度
  输  出:
    dirName - 返回的目录名称('\0'结尾),输入时给足够空间
  返回值:
            !NULL - 下一个目录路径的起始地址
            NULL  - 表示解析结束了

 */
inline static char *getNextPath(const char *src, const char *dirName, unsigned bufLen)
{
    int     cnt  = 0;
    char *pdst, *ptr;

    if((NULL == src) || (NULL == dirName)) return NULL;

    pdst = (char *)dirName;

    for(ptr = (char *)src; (*ptr != '\0') && (*ptr == '/'); ptr++); //去掉多余的路径名前导'/'

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
  名  称: commfun_makePathedDir
  功  能: 按照path字符串指定的路径逐个生成子目录
  输  入:
        path - 待生成的路径目录(绝对路径)
  输  出:
  返回值:

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

    if( (ptr = getNextPath(ptr, dirName, 128)) != NULL)  //获得根目录
    {
        if(!bRelative) // 如果创建的是绝对路径，则在创建目录名前加'/'表示绝对路径
            strcpy(filepath, "/");
        else
            filepath[0] = '\0';

        strcat(filepath, dirName);

        if( mkdir(filepath, 0770) < 0)
            if( errno != EEXIST)
                return FALSE;

    }
    else //根目录提取错误
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
  名  称: FileExists
  功  能: 检测文件是否存在
  输  入:
            filepath - 查找文件是否存在
  输  出:

  返回值:
            TRUE    - 存在
            FALSE - 不存在
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
  名  称: commfun_PathExists
  功  能: 检测目录是否存在
  输  入:
            filepath - 查找文件是否存在
  输  出:

  返回值:
            TRUE    - 存在
            FALSE - 不存在
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
  名  称: commfun_ExtractFilePath
  功  能: 从包含有完整路径的文件名中提取出其路径
  输  入:
    src  - 源字符串
  输  出:
    path - 目的写入字符串
  返回值:
            -1 - 失败
            0    - 成功
 */
int commfun_ExtractFilePath(const char *src, char *path)
{
    int     len, frag;
    char    *off;

    if( !src || !path)
        return -1;

    len = strlen(src);
    off = (char *)&src[len - 1];//-去掉'\0'-
    frag = 0;
    while((off != &src[0]) && (*off != '/'))//-文件路径名从后往前，抽取路径名-
    {
        off--;
        frag++;
    }

    strncpy(path, src, len - frag);  //!这个函数不会自动在字符串后面加'\0'
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
* 函数名称: CalWeekday
* 函数功能: 基姆拉尔森计算星期公式
* 输入参数: int y   年(如:2008)
*           int m   月
*           int d   日
* 输出参数: 无
* 返 回 值:  星期(1-7) 1-6 :星期一~六; 7:星期日
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

//检测是否插入U盘
int commfun_CanUseUDisk(void)
{
    FILE* pf = fopen("/dev/sda1", "r+");
    if(pf == NULL)
    {
        return 1;// U盘没有插入
    }
    fclose(pf);

    BOOL bUsb = FALSE;
    pf = fopen("/proc/mounts", "r+");
    if(pf == NULL)
    {
        //MessageBox("读取U盘挂载点失败\n不能继续备份!");
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
    // 1表示没有挂载U盘，0表示U盘已经挂载
    return (bUsb==TRUE)?0:1;
}

/**********************************************************
函数功能:RebootByTaskd
        通过taskd重启终端
函数输入值:无
函数返回值:无
*************************************************************/
void RebootByTaskd(void)
{
       uint16 aTime = 0;
       //DMSG    message;

    /* 向守护进程发送信息，重启终端 */
    //message.mtype = DMSG_REBOOT;
    //message.data0 = 0xFE;
    //SendDMsg(&message);

    for (aTime = 0; aTime < 30; aTime++)
    {
        //KeepMeAlive();      //守护进程监视
        sleep(1);
    }   //等待10秒后还没有重启，自动重启
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
