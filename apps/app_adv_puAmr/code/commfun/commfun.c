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

#include "commfun.h"
#include "mem.h"     //TODO ���C�����ֻ���������
#include "data_id.h"
#include "para_id.h"

//Ĭ������debug����ӡ
uint32 g_debug_enable = 0xffffffff;
//Ĭ������trace����ӡ
uint32 g_trace_enable = 0xffffffff;
//Ĭ������log����¼
uint32 g_log_enable = 0xffffffff;

#if DESC("����ʱ��ת������",1)

/**
*********************************************************************
* @brief��      �����ж�
* @param[in]��  year    �������
* @param[out]��NA   
* @return��     ת���ɹ�
*********************************************************************
*/
BOOL DT_IsLeap(uint16 year)
{
    if(((year%4==0)&&(year%100!=0))||year%400==0)
        return TRUE;
    else
        return FALSE;
}

/**
*********************************************************************
* @brief��      �ж϶̸�ʽʱ���Ƿ���Ч
* @param[in]�� dateTime    OOP��ʱ���ʽ
* @param[out]��NA      
* @return��    1-��Ч 0-��Ч
*********************************************************************
*/
BOOL DT_DateTimeShortIsValid(OOP_DATETIME_S_T *DateTime)
{
    const uint8 MonTab[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    //�ж�ʱ���Ƿ�Ϸ�
    if(DateTime == NULL)
    {
        return FALSE;
    }

    if((DateTime->year > 2099)||(DateTime->year < 1900))
    {
        return FALSE;
    }
    if((DateTime->month > 12)||(DateTime->month == 0))
    {
        return FALSE;
    }
    if((DateTime->day > MonTab[DateTime->month-1])||(DateTime->day == 0))
    {
        if((DateTime->month==2)&&(TRUE == DT_IsLeap(DateTime->year)))
        {
            if(DateTime->day>29)
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    if(DateTime->hour>23)
    {
        return FALSE;
    }
    if(DateTime->minute>59)
    {
        return FALSE;
    }
    if(DateTime->second>59)
    {
        return FALSE;
    }
    return TRUE;
}

/**
*********************************************************************
* @brief��       OOP��ʱ���ʽת��Ϊϵͳʱ���ʽ
* @param[in]��   dateTime     ��ʱ���ʽ
* @param[out]�� linuxTime     Linuxϵͳʱ�䣬1970������������
* @return��      ת���ɹ�
*********************************************************************
*/
BOOL DT_DateTimeShort2Time(OOP_DATETIME_S_T *dateTime,time_t *linuxTim)
{
    struct tm ptm;
    
    if(dateTime == NULL)
    {
        return FALSE;
    }
    
    if(FALSE == DT_DateTimeShortIsValid(dateTime))
    {
        return FALSE;
    }

    ptm.tm_year = dateTime->year-1900;
    ptm.tm_mon = dateTime->month-1;
    ptm.tm_mday = dateTime->day;
    ptm.tm_hour = dateTime->hour;
    ptm.tm_min = dateTime->minute;
    ptm.tm_sec = dateTime->second;

    //*linuxTim = mktime(&ptm);
    *linuxTim = FastDateToSecond(&ptm);
    return TRUE;
}

/**
*********************************************************************
* @brief��      ϵͳʱ���ʽת��Ϊ16����ʱ���ʽ
* @param[in]�� linuxTime       Linuxϵͳʱ�䣬1970������������
* @param[out]��dateTime        OOP��ʱ���ʽ
* @return��     ת���ɹ�
*********************************************************************
*/
BOOL DT_Time2DateTimeShort(time_t linuxTime, OOP_DATETIME_S_T *dateTime)
{
    struct  tm systime;
    struct  timeval tv;
    struct  timezone tz;
    gettimeofday(&tv, &tz);
    
    tv.tv_sec = linuxTime;
    FastSecondToDate(&tv, &tz, &systime);

    dateTime->year = systime.tm_year+1900; 
    dateTime->month = systime.tm_mon+1;
    dateTime->day = systime.tm_mday;
    dateTime->hour = systime.tm_hour;
    dateTime->minute = systime.tm_min;
    dateTime->second = systime.tm_sec;
    dateTime->week = systime.tm_wday;  //0-��ʾ����

    return TRUE;
}

/**
*********************************************************************
* @brief��      �ж�16����ʱ���Ƿ���Ч
* @param[in]�� dateTimeHex     �ն�16���Ʊ�ʾʱ��
* @param[out]��NA      
* @return��    1-��Ч 0-��Ч
*********************************************************************
*/
BOOL DT_DateTimeHexIsValid(DateTimeHex_t *DateTime)
{
    const uint8 MonTab[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    uint16 tmpyear;
    //�ж�ʱ���Ƿ�Ϸ�
    if(DateTime == NULL)
    {
        return FALSE;
    }
    tmpyear = (uint16)DateTime->year_h<<8;
    tmpyear += DateTime->year_l;
    if((tmpyear>2099)||(tmpyear<1900))
    {
        return FALSE;
    }
    if((DateTime->month > 12)||(DateTime->month == 0))
    {
        return FALSE;
    }
    if((DateTime->day > MonTab[DateTime->month-1])||(DateTime->day == 0))
    {
        if((DateTime->month==2)&&(TRUE == DT_IsLeap(tmpyear)))
        {
            if(DateTime->day>29)
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    if(DateTime->hour>23)
    {
        return FALSE;
    }
    if(DateTime->min>59)
    {
        return FALSE;
    }
    if(DateTime->sec>59)
    {
        return FALSE;
    }
    return TRUE;
}

/**
*********************************************************************
* @brief��      16����ʱ���ʽת��Ϊϵͳʱ���ʽ
* @param[in]��dateTimeHex     �ն�16���Ʊ�ʾʱ��
* @param[out]�� linuxTime       Linuxϵͳʱ���ʽ
* @return��     ת���ɹ�
*********************************************************************
*/
BOOL DT_DateTimeHex2Time(DateTimeHex_t *dateTimeHex,time_t *linuxTim)
{
    struct tm ptm;
    uint16 year;
    if(dateTimeHex == NULL)
        return FALSE;
    if(FALSE == DT_DateTimeHexIsValid(dateTimeHex))
        return FALSE;
    year = (uint16)dateTimeHex->year_h<<8;
    year += dateTimeHex->year_l;
    ptm.tm_year = year-1900;
    ptm.tm_mon = dateTimeHex->month-1;
    ptm.tm_mday = dateTimeHex->day;
    ptm.tm_hour = dateTimeHex->hour;
    ptm.tm_min = dateTimeHex->min;
    ptm.tm_sec = dateTimeHex->sec;

    *linuxTim = FastDateToSecond(&ptm);

    return TRUE;
}

/**
*********************************************************************
* @brief��      ϵͳʱ���ʽת��Ϊ16����ʱ���ʽ
* @param[in]�� linuxTime       Linuxϵͳʱ���ʽ
* @param[out]��dateTimeHex     �ն�16���Ʊ�ʾʱ��
* @return��     ת���ɹ�
*********************************************************************
*/
BOOL DT_Time2DateTimeHex(time_t linuxTime,DateTimeHex_t *dateTimeHex)
{
    struct tm systime;
    struct  timeval tv;
    struct  timezone tz;
    gettimeofday(&tv, &tz);
    
    tv.tv_sec = linuxTime;
    FastSecondToDate(&tv, &tz, &systime);

    dateTimeHex->year_h = (systime.tm_year+1900)>>8;
    dateTimeHex->year_l = (uint8)(systime.tm_year+1900);
    dateTimeHex->month = systime.tm_mon+1;
    dateTimeHex->day = systime.tm_mday;
    dateTimeHex->hour = systime.tm_hour;
    dateTimeHex->min = systime.tm_min;
    dateTimeHex->sec = systime.tm_sec;
    return TRUE;
}

/**
*********************************************************************
* @brief��      �ж�BCD����ʱ���Ƿ���Ч
* @param[in]�� dateTimeHex     �ն�16���Ʊ�ʾʱ��
* @param[out]��NA      
* @return��    1-��Ч 0-��Ч
*********************************************************************
*/
BOOL DT_DateTimeBCDIsValid(DateTimeBcd_t *DateTime)
{
    uint8 i,*pdateTime;
    DateTimeHex_t hexDateTime;
    pdateTime = (uint8*)DateTime;
    for(i=0; i<7; i++)
    {
        //�ж��Ƿ���BCDʱ�䣬�������BCDʱ������Ϊ��HEXʱ��
        if((pdateTime[i]&0x0F)>0x09||(pdateTime[i]&0xF0)>0x90)
        {
            break;
        }
    }
    if(i==7)//BCDʱ��
    {
        uint16 year;
        year = commfun_BcdToHex(DateTime->year_h);
        year *= 100;
        year += commfun_BcdToHex(DateTime->year_l);
        hexDateTime.year_h = (uint8)(year>>8);
        hexDateTime.year_l =(uint8)year;
        hexDateTime.month =(uint8)commfun_BcdToHex(DateTime->month);
        hexDateTime.day =(uint8)commfun_BcdToHex(DateTime->day);
        hexDateTime.hour =(uint8)commfun_BcdToHex(DateTime->hour);
        hexDateTime.min =(uint8)commfun_BcdToHex(DateTime->min);
        hexDateTime.sec =(uint8)commfun_BcdToHex(DateTime->sec);
    }
    else
    {
        return FALSE;
    }
    return(DT_DateTimeHexIsValid(&hexDateTime));
}

/**
*********************************************************************
* @brief��      BCD����ʱ���ʽת��Ϊϵͳʱ���ʽ
* @param[in]��dateTimeHex     �ն�16���Ʊ�ʾʱ��
* @param[out]�� linuxTime       Linuxϵͳʱ���ʽ
* @return��     ת���ɹ�
*********************************************************************
*/
BOOL DT_DateTimeBCD2Time(DateTimeBcd_t *dateTimeBCD,time_t *linuxTime)
{
    struct tm ptm;
    if(dateTimeBCD == NULL)
        return FALSE;
    if(FALSE == DT_DateTimeBCDIsValid(dateTimeBCD))
    {
         printf("DT_DateTimeBCD2Time\n");
        printf("ʱ��Ƿ�\n");
        return FALSE;
    }
    //commfun_printstring((uint8*)dateTimeBCD,7);
    uint16 year;
    year = commfun_BcdToHex(dateTimeBCD->year_h);
    year *= 100;
    year += commfun_BcdToHex(dateTimeBCD->year_l);
    ptm.tm_year = year-1900;
    //ptm.tm_year = (commfun_BcdToHex(dateTimeBCD->year_l) >= 70 ? commfun_BcdToHex(dateTimeBCD->year_l) : (commfun_BcdToHex(dateTimeBCD->year_l) + 100));
    ptm.tm_mon = commfun_BcdToHex(dateTimeBCD->month)-1;
    ptm.tm_mday = commfun_BcdToHex(dateTimeBCD->day);
    ptm.tm_hour = commfun_BcdToHex(dateTimeBCD->hour);
    ptm.tm_min = commfun_BcdToHex(dateTimeBCD->min);
    ptm.tm_sec = commfun_BcdToHex(dateTimeBCD->sec);

    *linuxTime = FastDateToSecond(&ptm);

    //printf("linuxTime=%d time = %d\n ",*linuxTime,time(NULL));
    return TRUE;
}

/**
*********************************************************************
* @brief��      ϵͳʱ���ʽת��ΪBCD����ʱ���ʽ
* @param[in]�� linuxTime       Linuxϵͳʱ���ʽ
* @param[out]��dateTimeHex     �ն�16���Ʊ�ʾʱ��
* @return��     ת���ɹ�
*********************************************************************
*/
BOOL DT_Time2DateTimeBCD(time_t linuxTime,DateTimeBcd_t *dateTimeBCD)
{

    struct tm tm_Time;
    LOCALTIME_R(&linuxTime, &tm_Time);
    dateTimeBCD->year_h = (tm_Time.tm_year+1900)%10000/1000*16+(tm_Time.tm_year+1900)%1000/100;
    dateTimeBCD->year_l =(tm_Time.tm_year+1900)%100/10*16+(tm_Time.tm_year+1900)%10;
    dateTimeBCD->month =(tm_Time.tm_mon+1)%100/10*16+(tm_Time.tm_mon+1)%10;
    dateTimeBCD->day =(tm_Time.tm_mday)%100/10*16+(tm_Time.tm_mday)%10;
    dateTimeBCD->hour =(tm_Time.tm_hour)%100/10*16+(tm_Time.tm_hour)%10;
    dateTimeBCD->min =(tm_Time.tm_min)%100/10*16+(tm_Time.tm_min)%10;
    dateTimeBCD->sec =(tm_Time.tm_sec)%100/10*16+(tm_Time.tm_sec)%10;
    return TRUE;
}

/**
*********************************************************************
* @brief��     HEX����ʱ���ʽת��ΪBCD����ʱ���ʽ
* @param[in]�� dateTimeHex     HEX����ʱ���ʽ
* @param[out]��dateTimeBCD     BCD����ʱ���ʽ
* @return��    ת���ɹ�
*********************************************************************
*/
BOOL DT_DateTimeHex2BCD(DateTimeHex_t *dateTimeHex,DateTimeBcd_t *dateTimeBCD)
{
    uint16 year;
    if(dateTimeHex == NULL)
        return FALSE;
    if(FALSE == DT_DateTimeHexIsValid(dateTimeHex))
        return FALSE;
    year = (uint16)dateTimeHex->year_h<<8;
    year += dateTimeHex->year_l;
    dateTimeBCD->year_h = (uint8)commfun_HexToBcd(year/100);
    dateTimeBCD->year_l =(uint8)commfun_HexToBcd(year%100);
    dateTimeBCD->month =(uint8)commfun_HexToBcd(dateTimeHex->month);
    dateTimeBCD->day =(uint8)commfun_HexToBcd(dateTimeHex->day);
    dateTimeBCD->hour =(uint8)commfun_HexToBcd(dateTimeHex->hour);
    dateTimeBCD->min =(uint8)commfun_HexToBcd(dateTimeHex->min);
    dateTimeBCD->sec =(uint8)commfun_HexToBcd(dateTimeHex->sec);
    return TRUE;
}

/**
*********************************************************************
* @brief��     BCD����ʱ���ʽת��ΪBCD����ʱ���ʽ
* @param[in]�� dateTimeBCD     BCD����ʱ���ʽ
* @param[out]��dateTimeHex     HEX����ʱ���ʽ
* @return��    ת���ɹ�
*********************************************************************
*/
BOOL DT_DateTimeBCD2Hex(DateTimeBcd_t *dateTimeBCD,DateTimeHex_t *dateTimeHex)
{
    uint16 year;
    if(dateTimeBCD == NULL)
        return FALSE;
    if(FALSE == DT_DateTimeBCDIsValid(dateTimeBCD))
        return FALSE;
    year = commfun_BcdToHex(dateTimeBCD->year_h);
    year *= 100;
    year += commfun_BcdToHex(dateTimeBCD->year_l);
    dateTimeHex->year_h = (uint8)(year>>8);
    dateTimeHex->year_l =(uint8)year;
    dateTimeHex->month =(uint8)commfun_BcdToHex(dateTimeBCD->month);
    dateTimeHex->day =(uint8)commfun_BcdToHex(dateTimeBCD->day);
    dateTimeHex->hour =(uint8)commfun_BcdToHex(dateTimeBCD->hour);
    dateTimeHex->min =(uint8)commfun_BcdToHex(dateTimeBCD->min);
    dateTimeHex->sec =(uint8)commfun_BcdToHex(dateTimeBCD->sec);
    return TRUE;
}

/**
*********************************************************************
* @brief��      ��ʱ����ת��Ϊ��
* @param[in]��  ti    ʱ����
* @param[out]��NA   
* @return��     ת���ɹ�
*********************************************************************
*/
long DT_TI2Seconds(OOP_TI_T ti)
{
    long longtime=0;
    if(ti.unit>5)
        return 0;
    if(ti.value == 0)
        return 0;
    if(ti.unit<4)
    {
        if(ti.unit==0)
            longtime = ti.value;
        else if(ti.unit==1)
            longtime = (long)ti.value*60;
        else if(ti.unit==2)
            longtime = (long)ti.value*3600;
        else
            longtime = (long)ti.value*86400;
    }
    return longtime;
}

/**
*********************************************************************
* @brief��      ��һ���������
* @param[in]��  time  ���ӵ�ʱ��
* @param[in]��  ti    ����ʱ����
* @param[out]��NA   
* @return��     void
*********************************************************************
*/
void DT_TimeAddInv(time_t *time,OOP_TI_T ti)
{
    const uint8 MonTab[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    long longtime=0;
    uint16 month,year;
    DateTimeHex_t hexDateTime;
    DT_Time2DateTimeHex(*time,&hexDateTime);
    if(ti.unit>5)
        return;
    if(ti.value == 0)
        return;
    if(ti.unit<4)
    {
        if(ti.unit==0)
            longtime = ti.value;
        else if(ti.unit==1)
            longtime = (long)ti.value*60;
        else if(ti.unit==2)
            longtime = (long)ti.value*3600;
        else
            longtime = (long)ti.value*86400;
        longtime += *time;
    }
    else
    {
        year = ((uint16)hexDateTime.year_h<<8)+(uint16)hexDateTime.year_l;
        if(ti.unit == 4)//��
        {
            month = hexDateTime.month-1;
            month += ti.value;
            year += month/12;
            if(year>2099)
                return;
            hexDateTime.year_h = (uint8)(year>>8);
            hexDateTime.year_l = (uint8)year;
            hexDateTime.month = month%12 +1;
        }
        else if(ti.unit == 5)//��
        {
            year += ti.value;
            if(year>2099)
                return;
            
            hexDateTime.year_h = (uint8)(year>>8);
            hexDateTime.year_l = (uint8)year; 
        }
        uint8 maxday;
        maxday = MonTab[hexDateTime.month-1];
        if(hexDateTime.month==2 && (TRUE == DT_IsLeap(year)))
            maxday+=1;
        if(hexDateTime.day>maxday)//ʹʱ�����Ϊ�Ϸ���ʱ��
            hexDateTime.day = maxday;
        DT_DateTimeHex2Time(&hexDateTime,&longtime);
    }
    *time = longtime;
}

/**
*********************************************************************
* @brief��      ��һ���������
* @param[in]��  time  ������ʱ��
* @param[in]��  ti    ������ʱ����
* @param[out]��NA   
* @return��     void
*********************************************************************
*/
void DT_TimeSubInv(time_t *time,OOP_TI_T ti)
{
    const uint8 MonTab[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    long longtime=0;
    uint16 month,year;
    DateTimeHex_t hexDateTime;
    DT_Time2DateTimeHex(*time,&hexDateTime);
    if(ti.unit>5)
        return;
    if(ti.value == 0xFFFF)
        return;
    if(ti.unit<4)
    {
        if(ti.unit==0)
            longtime = ti.value;
        else if(ti.unit==1)
            longtime = (long)ti.value*60;
        else if(ti.unit==2)
            longtime = (long)ti.value*3600;
        else
            longtime = (long)ti.value*86400;
        if(*time<longtime)
            return;
        longtime = *time - longtime;
    }
    else
    {
        year = ((uint16)hexDateTime.year_h<<8)+(uint16)hexDateTime.year_l;
        if(ti.unit == 4)//��
        {
            month = year*12 + hexDateTime.month-1;
            if(month<ti.value)
                return;
            month -= ti.value;
            year = month/12;
            if(year<1900)
                return;
            hexDateTime.year_h = (uint8)(year>>8);
            hexDateTime.year_l = (uint8)year;
            hexDateTime.month = month%12 +1;
        }
        else if(ti.unit == 5)//��
        {
            if(year<ti.value)
                return;
            year -= ti.value;
            if(year<1900)
                return;

            hexDateTime.year_h = (uint8)(year>>8);
            hexDateTime.year_l = (uint8)year;           
        }
        uint8 maxday;
        maxday = MonTab[hexDateTime.month-1];
        if(hexDateTime.month==2 && (TRUE == DT_IsLeap(year)))
            maxday+=1;
        if(hexDateTime.day>maxday)//ʹʱ�����Ϊ�Ϸ���ʱ��
            hexDateTime.day = maxday;
        DT_DateTimeHex2Time(&hexDateTime,&longtime);
    }
    *time = longtime;
}

#endif

#if DESC("�����ӡ����",1)
void commfun_printstring(uint8 *buf, int16 len)
{
    uint16 i = 0;
    if(len <= 0)
        return;
    for(i = 0; i < len; i++)
        printf("%02x ", buf[i]);
    putchar('\n');
}

//��ӡ��ǰʱ��
//blnNewline        �Ƿ���
//blnEnv            ���Ը��ݻ������������Ƿ��ӡ
void commfun_printcurtime(BOOL blnNewline, BOOL blnPrint)
{
    if(blnPrint == FALSE)
        return;

    time_t      t;
    struct tm   tsm;
    t = time(NULL);
    LOCALTIME_R(&t, &tsm);
    printf("%4d-%02d-%02d %02d:%02d:%02d  ", tsm.tm_year + 1900, tsm.tm_mon + 1, tsm.tm_mday, tsm.tm_hour, tsm.tm_min, tsm.tm_sec);
    if(blnNewline == TRUE)
        printf("\n");

    return;
}

//������д����־��
//blnPrint      ���Ը��ݻ������������Ƿ��ӡ
//filepath      ��־·��
//buf           ������Ϣ
//len           ���ĳ���
//fmt           ������Ϣ�����Լ�¼����
void commfun_writebuflog(BOOL blnPrint, const char *filepath, char *buf, int len, const char *fmt, ...)
{
    int         i;
    va_list     ap;
    char        msgbuf[8100];
    int         msglen = 0;

    memset(msgbuf, 0x0, sizeof(msgbuf));
    va_start(ap, fmt);
    vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);
    va_end(ap);

    msglen = strlen(msgbuf);
    char logbuf[len * 3 + msglen + 10];
    memset(logbuf, 0x0, sizeof(logbuf));
    if(msglen > 0)
        strcat(logbuf, msgbuf);
    if(len > 0)
    {
        for(i = 0; i < len; i++)
            sprintf(logbuf + msglen + i * 3, " %02X", buf[i]);
    }
    writelog(filepath, logbuf);

    //��ӡ������Ϣ
    if(blnPrint == TRUE)
    {
        commfun_printcurtime(FALSE, blnPrint);      //��ӡ��ǰʱ��
        printf("%s\n", logbuf);
    }
}

#endif

#if DESC("ת�뺯��",1)
/**
*********************************************************************
* @brief��      ʮ��������תѹ��BCD��
* @param[in]�� hex ʮ��������
* @param[out]��NA
* @return��     ѹ��BCD��
*********************************************************************
*/
char commfun_HexToBcd(int hex)
{
    uint8 xch;
    char rv=0;

    if(hex>99) return 0x99;
    xch = hex%10;
    rv += xch;
    xch = (hex/10)%10;
    xch <<= 4;
    rv += xch;

    return rv;
}

/**
*********************************************************************
* @brief��      ѹ��BCD��תʮ��������
* @param[in]�� compress_bcd ѹ��BCD��
* @param[out]��NA
* @return��     ʮ��������
*********************************************************************
*/
int commfun_BcdToHex(char compress_bcd)
{
    int rv;

    rv = ((compress_bcd)>>4)*10+(compress_bcd&0x0f);
    return rv;
}

/*********************************************************************
�������ܵ�Ԫ��GSM��̬��
������������16������ת��������Ascii��
���������
bin     16������
high    Ascii���λ
low     Ascii���λ
�������:
��
��������ֵ:
��
�ڲ���������:��16�������������Ascii��
********************************************************************/
void commfun_BinToAscii(uint8 bin, uint8 *high, uint8 *low)
{
    *high = bin>>4;
    *low = bin&0x0f;

    if ( *high <= 0x09 )
        *high += '0';
    else
    {
        *high += 'A' - 0x0A;
    }

    if ( *low <= 0x09 )
        *low += '0';
    else
        *low += 'A' - 0x0A;
    return;
}

/*********************************************************************
�������ܵ�Ԫ��GSM��̬��
����������������Ascii��ת����һ��16������
���������
high    ת��Ϊ16���Ƹ�λ
low     ת��Ϊ16���Ƶ�λ
���������
��
��������ֵ��
ת�����16������
�ڲ���������: ��highת���ɽ��Ƹ�λ, lowת����16���Ƶ�λ
********************************************************************/
uint8 commfun_AsciiToBin(uint8 high, uint8 low)
{
    if( (low>='A')&&(low<='F') )
    {
        low = low - 'A' + 0x0a;
    }
    else if( (low>='a')&&(low<='f') )
    {
        low = low - 'a' + 0x0a;
    }
    else if( (low >='0')&&(low<='9') )
    {
        low = low - '0' + 0x00;
    }
    else
        return 0xFF;

    if( (high>='A')&&(high<='F') )
    {
        high = high - 'A' + 0x0a;
    }
    else if( (high>='a')&&(high<='f') )
    {
        high = high - 'a' + 0x0a;
    }
    else if( (high>='0')&&(high<='9') )
    {
        high = high - '0' + 0x00;
    }
    else
        return 0xFF;
    //      printf("low = %02x, high = %02x\n", low, high);
    return ((0x0f)&low) | (high<<4);
}

// bcd��ת��Ϊascii��
int commfun_BCD2ASC(char *asc, char *bcd, int len, int fmt)
{
    int i, odd;
    unsigned char c;

    odd = len&0x01;

    if ( odd && !fmt)
        *asc++ = ((*bcd++)&'\x0F') + '0';

    len >>= 1;

    for (i=0; i<len; i++)
    {
        c = *bcd++;
        *asc++ = (c>>4) + '0';
        *asc++ = (c&'\x0F') + '0';
    }

    if ( odd && fmt)
        *asc = ((*bcd)>>4) + '0';

    asc[len] = '\0';

    return (i+odd);
}

// �ַ�����BCDת������
// ascii��ת��Ϊbcd��
int commfun_ASC2BCD(char *bcd, const char *asc, int len, int fmt)
{
    int i, odd;
    char c;

    odd = len&0x01;
    if ( odd && !fmt)
        *bcd++ = (*asc++) & '\x0F';

    len >>= 1;

    for (i=0; i<len; i++)
    {
        c = (*asc++) << 4;
        c |= (*asc++) & '\x0F';

        *bcd++ = c;
    }

    if ( odd && fmt)
        *bcd = (*asc) << 4;

    return (i+odd);
}

/*********************************************************************
����������    IP4����ת��ΪHex�ַ���
���������    int nIp��IP��ַ
char* pVal��HexIP�ַ���
���������
��������ֵ��
�ڲ��������̣�
********************************************************************/
void commfun_IPToHexStr(int nIP, char* pVal)
{
	unsigned char b1 = nIP&0xFF;
    unsigned char b2 = (nIP>>8)&0xFF;
    unsigned char b3 = (nIP>>16)&0xFF;
    unsigned char b4 = (nIP>>24)&0xFF;

	pVal[0] = b1;
	pVal[1] = b2;
	pVal[2] = b3;
	pVal[3] = b4;
}

/*********************************************************************
����������    IP4����ת��Ϊ�ַ���
���������    int nIp��IP��ַ
char* pVal��IP�ַ���
���������
��������ֵ��
�ڲ��������̣�
********************************************************************/
void commfun_IPToStr(int nIp, char* pVal)
{
    unsigned char b1 = nIp&0xFF;
    unsigned char b2 = (nIp>>8)&0xFF;
    unsigned char b3 = (nIp>>16)&0xFF;
    unsigned char b4 = (nIp>>24)&0xFF;
    sprintf(pVal, "%d.%d.%d.%d", b1, b2, b3, b4);
}

/*********************************************************************
����������    IP4����ת��Ϊ�ַ���
���������    int nIp��IP��ַ
char* pVal��IP�ַ���
���������
��������ֵ��
�ڲ��������̣�
********************************************************************/

void commfun_HexIPToInt(uint8 *pczIp, int *pnIp)
{
	char p1[4];
    memcpy(p1, pczIp, 4);

	int b1 = p1[3];
    int b2 = p1[2];
    int b3 = p1[1];
    int b4 = p1[0];

	*pnIp = b4 | (b3<<8) | (b2<<16) | (b1<<24);
}



/*********************************************************************
����������    IP�ַ���ת��Ϊ4�ֽ�����
���������    char* pczIp��IP�ַ���
iint* pnIp������4�ֽ�IP
���������   TRUE��ת���ɹ�
��������ֵ��
�ڲ��������̣�
********************************************************************/
BOOL commfun_IpToInt(char* pczIp, int* pnIp)
{
    if(pczIp == NULL || strlen(pczIp) == 0)
        return FALSE;

    char* p1[4];
    char czBuf[20];
    strcpy(czBuf, pczIp);

    char* pTmp = p1[0] = czBuf;
    int nIndex = 1;
    while(*pTmp != '\0')
    {
        if(*pTmp == '.')
        {
            *pTmp = '\0';
            pTmp ++;
            if(*pTmp == '\0')
            {
                break;
            }
            else
            {
                if(*pTmp > '9' || *pTmp < '0')
                    return FALSE;
                if(nIndex > 4)
                    return FALSE;
                p1[nIndex] = pTmp;
                nIndex ++;
            }
        }
        else if(*pTmp > '9' || *pTmp < '0')
        {
            return FALSE;
        }
        pTmp++;
    }
    if(nIndex != 4)
    {
        return FALSE;
    }
    int b1 = atoi(p1[3]);
    int b2 = atoi(p1[2]);
    int b3 = atoi(p1[1]);
    int b4 = atoi(p1[0]);
    if(b1 > 0xFF || b2 > 0xFF || b3 > 0xFF || b4 > 0xFF)
    {
        return FALSE;
    }
#if HENAN_FUN == 1
    if(b4 == 0 || b1 == 0 || b1 == 0xff)
        return FALSE;
#endif
    *pnIp = b4 | (b3<<8) | (b2<<16) | (b1<<24);
    return TRUE;
}

//���ַ���ת��Ϊ��д�ַ���
void commfun_toupper(char* pStr)
{
    int i = 0;
    while(pStr[i])  //���ڲ��汾��Ϣת��Ϊ��д��ĸ
    {
        pStr[i] = toupper(pStr[i]);
        i++;
    }
}

//���ַ���ת��ΪСд�ַ���
void commfun_tolower(char* pStr)
{
    int i = 0;
    while(pStr[i])  //���ڲ��汾��Ϣת��Ϊ��д��ĸ
    {
        pStr[i] = tolower(pStr[i]);
        i++;
    }
}

#endif

BOOL commfun_EthEnable(uint32 eth_id)
{
    ETH_INFO eth_info;
    memset((void *)&eth_info, 0x0, sizeof(ETH_INFO));
    if(ReadXPara(TMN_RUN_PARA, eth_id, (char *)&eth_info, 0) > 0 && eth_info.eth_en == 0)
        return FALSE;
    else
        return TRUE;
}


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

BOOL commfun_IpConfig(uint8 bEnable, int nIp, uint32 eth_id, char *eth_name)
{
    //�����ն�����IP��ַ
    ETH_INFO eth_info;
    char czIp[20];
    char czCmdBuf[100];

    memset((void *)&eth_info, 0x0, sizeof(ETH_INFO));
    memset(czIp, 0x0, sizeof(czIp));
    memset(czCmdBuf, 0x0, sizeof(czCmdBuf));

    //д�������ֵ�
    eth_info.eth_en = bEnable;  //��Ч��
    eth_info.eth_ip = nIp;      //ip��ַ
    WriteXPara(TMN_RUN_PARA, eth_id, (char *)&eth_info, 0);

    //��������IP
    commfun_IPToStr(eth_info.eth_ip, czIp);
    memset(czCmdBuf, 0x00, 100);
    if(bEnable == 1)    //��Ч
    {
        sprintf(czCmdBuf, "ifconfig %s %s up", eth_name, czIp);
        system(czCmdBuf);
        return TRUE;
    }
    else
    {
        sprintf(czCmdBuf, "ifconfig %s down", eth_name);
        system(czCmdBuf);
        return TRUE;
    }
}

//�������ֵ��ж�ȡ�ն����أ��û����õ�ֵ��
int commfun_GetGatewayPara()
{
    char czData[10];
    int uGateway = -1;

    memset(czData, 0x0, sizeof(czData));
    if(ReadXPara(TMN_RUN_PARA, Default_Gw_Addr_ID, czData, 0) < 0)
        uGateway = -1;
    else
        memcpy((char*)&uGateway, czData, 4);
    return uGateway;
}

//�������ֵ��ж�ȡ�ն˵��������루�û����õ�ֵ��
int commfun_GetNetmaskPara()
{
    char czData[10];
    int uNetmask = -1;

    memset(czData, 0x0, sizeof(czData));
    if(ReadXPara(TMN_RUN_PARA, Mask_Addr_698_ID, czData, 0) < 0)
        uNetmask = -1;
    else
        memcpy((char*)&uNetmask, czData, 4);
    return uNetmask;
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
uint8 commfun_GetMtrMode(mtrnum_t mtr_id)
{
    uint8 bSpeed = 0;
    if(ReadXPara(MTR_PARA_TYPE, Mt_Para_Speed_ID, (char *)&bSpeed, mtr_id) < 0 || bSpeed == 0xff)       //ͨѶ����
        bSpeed = 2400 / 300;        //Ĭ��Ϊ2400������
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
}

#if FAC_MODE == 1
int commfun_ReadFacMode(S_FacMode* pFacMode)
{
    if (ReadPDataXram(FACMODE_FILENAME, (char *)pFacMode, 0, sizeof(S_FacMode)) < 0 || (pFacMode->FacMode != 0x55 && pFacMode->FacMode != 0xAA))
        pFacMode->FacMode = 0x55;
    return sizeof(S_FacMode);
}

int commfun_WriteFacMode(S_FacMode* pFacMode)
{
    return WritePDataXram(FACMODE_FILENAME, (char *)pFacMode, 0, sizeof(S_FacMode));
}
#endif


//��rtcʱ�ӵĸ�ʽת��Ϊ����1970��1��1��0ʱ0��0�������
//sec_cancle �����������Ƿ������ģ������TRUE�Ļ�������ת����ʱ�������
//ԭ��������ִ�е�ʱ����С���ϱ���λ�Ƿ֣���ô�����ڼ�����������ʱ���ȷ�������Ƿ��Ѿ�������Ҫִ�е��������������ϱ���ʱ��ֱ�Ӷ���0�뿪ʼ�ϱ��Ϳ�����
time_t commfun_rtc2time(rtc_t src_rtc)
{
    /*

    ���� time_t
    ������ʱ�䣨Calendar Time����ͨ��time_t������������ʾ��
    ��time_t��ʾ��ʱ�䣨����ʱ�䣩�Ǵ�һ��ʱ��㣨���磺1970��1��1��0ʱ0��0�룩����ʱ��������
    ��time.h�У�����Ҳ���Կ���time_t��һ������������

    ���� tm
    struct tm {
    int tm_sec; // �� �C ȡֵ����Ϊ[0,59]
    int tm_min; // �� - ȡֵ����Ϊ[0,59]
    int tm_hour; // ʱ - ȡֵ����Ϊ[0,23]
    int tm_mday; // һ�����е����� - ȡֵ����Ϊ[1,31]
    int tm_mon; // �·ݣ���һ�¿�ʼ��0����һ�£� - ȡֵ����Ϊ[0,11]
    int tm_year; // ��ݣ���ֵ����ʵ����ݼ�ȥ1900
    int tm_wday; // ���� �C ȡֵ����Ϊ[0,6]������0���������죬1��������һ���Դ�����
    int tm_yday; // ��ÿ���1��1�տ�ʼ������ �C ȡֵ����Ϊ[0,365]������0����1��1�գ�1����1��2�գ��Դ�����
    int tm_isdst; // ����ʱ��ʶ����ʵ������ʱ��ʱ��tm_isdstΪ������ʵ������ʱ�Ľ���tm_isdstΪ0�����˽����ʱ��tm_isdst()Ϊ����
    };
    */
    struct tm des_tm;       //��׼��tmʱ�ӽṹ
    des_tm.tm_year = (commfun_BcdToHex(src_rtc.yy) > 70 ? commfun_BcdToHex(src_rtc.yy) : (commfun_BcdToHex(src_rtc.yy) + 100));
    des_tm.tm_mon = commfun_BcdToHex(src_rtc.mm) - 1;
    des_tm.tm_mday = commfun_BcdToHex(src_rtc.dd);
    des_tm.tm_hour = commfun_BcdToHex(src_rtc.hh);
    des_tm.tm_min = commfun_BcdToHex(src_rtc.nn);
    des_tm.tm_sec = 0;  //��������Ϊ0
    des_tm.tm_isdst = -1;           //��������
    //return mktime(&des_tm);     //��1970��1��1��0ʱ0��0�뵽��ʱ������
    return FastDateToSecond(&des_tm);

}


//��rtcʱ�ӵĸ�ʽת��Ϊ����1970��1��1��0ʱ0��0�������
//sec_cancle �����������Ƿ������ģ������TRUE�Ļ�������ת����ʱ�������
//ԭ��������ִ�е�ʱ����С���ϱ���λ�Ƿ֣���ô�����ڼ�����������ʱ���ȷ�������Ƿ��Ѿ�������Ҫִ�е��������������ϱ���ʱ��ֱ�Ӷ���0�뿪ʼ�ϱ��Ϳ�����
/**********��commfun_rtc2time�Ļ����ϰ���Ҳ�����ȥ*******/
time_t commfun_rtc2time2(rtc_t src_rtc)
{
    /*

    ���� time_t
    ������ʱ�䣨Calendar Time����ͨ��time_t������������ʾ��
    ��time_t��ʾ��ʱ�䣨����ʱ�䣩�Ǵ�һ��ʱ��㣨���磺1970��1��1��0ʱ0��0�룩����ʱ��������
    ��time.h�У�����Ҳ���Կ���time_t��һ������������

    ���� tm
    struct tm {
    int tm_sec; // �� �C ȡֵ����Ϊ[0,59]
    int tm_min; // �� - ȡֵ����Ϊ[0,59]
    int tm_hour; // ʱ - ȡֵ����Ϊ[0,23]
    int tm_mday; // һ�����е����� - ȡֵ����Ϊ[1,31]
    int tm_mon; // �·ݣ���һ�¿�ʼ��0����һ�£� - ȡֵ����Ϊ[0,11]
    int tm_year; // ��ݣ���ֵ����ʵ����ݼ�ȥ1900
    int tm_wday; // ���� �C ȡֵ����Ϊ[0,6]������0���������죬1��������һ���Դ�����
    int tm_yday; // ��ÿ���1��1�տ�ʼ������ �C ȡֵ����Ϊ[0,365]������0����1��1�գ�1����1��2�գ��Դ�����
    int tm_isdst; // ����ʱ��ʶ����ʵ������ʱ��ʱ��tm_isdstΪ������ʵ������ʱ�Ľ���tm_isdstΪ0�����˽����ʱ��tm_isdst()Ϊ����
    };
    */
    struct tm des_tm;       //��׼��tmʱ�ӽṹ
    des_tm.tm_year = (commfun_BcdToHex(src_rtc.yy) > 70 ? commfun_BcdToHex(src_rtc.yy) : (commfun_BcdToHex(src_rtc.yy) + 100));
    des_tm.tm_mon = commfun_BcdToHex(src_rtc.mm) - 1;
    des_tm.tm_mday = commfun_BcdToHex(src_rtc.dd);
    des_tm.tm_hour = commfun_BcdToHex(src_rtc.hh);
    des_tm.tm_min = commfun_BcdToHex(src_rtc.nn);
    des_tm.tm_sec = commfun_BcdToHex(src_rtc.ss);   //��������Ϊ0
    des_tm.tm_isdst = -1;           //��������
    
    return FastDateToSecond(&des_tm);

}


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

// ��ȡ�������峣����ַ��Ĭ�����峣��
static int GetACPulseConstInfo(uint16 *pConstAddr, uint32 *pDefaultConst)
{
    int    nret = 0;
    uint8  LineMode = 0; // ���߷�ʽ ����
    uint8  VoltType = 0; // �������߶��ѹ���� 57.7V/220V
    uint16 PulseConstAddr = 0;  // ���峣����ַ
    uint32 DefaultConst = 0;    // Ĭ�����峣��

    nret = ReadFromEEPROM((char *)&LineMode, FM_AC_POWER_TYPE_ADDR, 1);
    nret += ReadFromEEPROM((char *)&VoltType, FM_AC_Config_577_ADDR, 1);
    if (nret < 2)
    {
        return -1;
    }

    if (LineMode == AC_LINE_MODE_3P3L)
    {
        PulseConstAddr = FM_PULSE_CONST_100_ADDR;
        DefaultConst   = AC_DFT_CONST_3P3L_100;
    }
    else
    {
        if (VoltType == AC_VOLT_TYPE_577)
        {
            PulseConstAddr = FM_PULSE_CONST_577_ADDR;
            DefaultConst   = AC_DFT_CONST_3P4L_577;
        }
        else
        {
            PulseConstAddr = FM_PULSE_CONST_220_ADDR;
            DefaultConst   = AC_DFT_CONST_3P4L_220;
        }
    }
    *pConstAddr = PulseConstAddr;
    *pDefaultConst = DefaultConst;

    return 0;
}

// �������ȡ�������峣��
uint32 commfun_ReadACPulseConst(void)
{
    uint8 PhaseFlag= 0;
    uint32 mACPulseConst = 0;
    int    nret = 0;
    uint16 PulseConstAddr = 0;  // ���峣����ַ
    uint32 DefaultConst = 0;    // Ĭ�����峣��

    // ��ȡ�������峣����ַ��Ĭ�����峣��
    if (GetACPulseConstInfo(&PulseConstAddr, &DefaultConst) < 0)
    {
        return 0;
    }

    ReadFromEEPROM((char*)&PhaseFlag, FM_SELF_ADPTIVE_ADDR, sizeof(uint8));

    if (PhaseFlag == 0x55)
    {
        DefaultConst = 10000;
        mACPulseConst = DefaultConst;
        return mACPulseConst;
    }

    nret = ReadFromEEPROM((char *)&mACPulseConst, PulseConstAddr, 4);
    if((nret < 0) || (mACPulseConst == 0)
            || ((mACPulseConst > 0) && (mACPulseConst % 100 != 0)))
    {
        mACPulseConst = DefaultConst;
    }

    return mACPulseConst;
}

// д�������峣��������
int commfun_WriteACPulseConst(uint32 PulseConst)
{
    int    nret = 0;
    uint16 PulseConstAddr = 0;  // ���峣����ַ
    uint32 DefaultConst = 0;    // Ĭ�����峣��
    uint32 SavePulseConst = 0;  // ���ڴ洢�����峣��

    // ��ȡ�������峣����ַ��Ĭ�����峣��
    if (GetACPulseConstInfo(&PulseConstAddr, &DefaultConst) < 0)
    {
        return -1;
    }

    // �����峣�����Ϸ�������100������������ֵΪĬ�����峣�����洢
    if ((PulseConst == 0) || ((PulseConst > 0) && (PulseConst % 100 != 0)))
    {
        SavePulseConst = DefaultConst;
    }
    else
    {
        SavePulseConst = PulseConst;
    }
    nret = WriteToEEPROM((char *)&SavePulseConst, PulseConstAddr, 4);
    if (nret != 4)
    {
        return -1;
    }

    return 0;
}

int commfun_RelayIPCLen(RELAY_IPC_PULS *pRelayIpcPuls)
{
    return pRelayIpcPuls->RelayIPCLen + 12;
}

#if EVENT_2014_EN == 1
static void GetGradeFileName(uint16 GradeNum, char *pFileName)
{
    char tmpFile[255];
    sprintf(tmpFile, "%s%03d", EVENT_GRADE_FILE, GradeNum);
    strcpy(pFileName, tmpFile);
}

static uint16 GetGradeCS(char *pInInfo, int bLen)
{
    uint16 tmpCS = 0;
    int i = 0;

    for(i = 0; i < bLen; i++)
        tmpCS += pInInfo[i];
    tmpCS += 0x3333;

    return tmpCS;
}

//��˽������ж�ȡ���Ѿ��·����������ʶ����
static uint16 GetGradeParaCount(uint16 GradeNum)
{
    char GradeFile[255];                //˽���ļ�����
    int bRet = 0;
    uint16 DI0DI1_Num_File = 0;         //˽������д�ŵ��������ʶ����
    uint16 GradeCS = 0;                 //У���

    memset(GradeFile, 0x0, sizeof(GradeFile));
    GetGradeFileName(GradeNum, GradeFile);

    bRet = ReadPDataXram(GradeFile, (char *)&DI0DI1_Num_File, 0, sizeof(uint16));      //��ȡ˽������д�ŵ��������ʶ����
    if(bRet < 0 || DI0DI1_Num_File == 0 || DI0DI1_Num_File == 0xffff)
        DI0DI1_Num_File = 0;

    if(DI0DI1_Num_File > 0)
    {
        MET_GRADE_INFO fileGradeInfo[DI0DI1_Num_File];
        //��ȡ˽������д洢�ġ����ܱ����ݷּ�������
        bRet = ReadPDataXram(GradeFile, (char *)fileGradeInfo, 2, sizeof(MET_GRADE_INFO) * DI0DI1_Num_File);
        if(bRet < 0)
            DI0DI1_Num_File = 0;
        if(DI0DI1_Num_File > 0)
        {
            //��ȡУ���
            bRet = ReadPDataXram(GradeFile, (char *)&GradeCS, 2 + sizeof(MET_GRADE_INFO) * DI0DI1_Num_File, 2);
            if(bRet < 0 || GradeCS == 0xffff)
                DI0DI1_Num_File = 0;
            else
            {
                //��֤У���
                if(GetGradeCS((char *)fileGradeInfo, sizeof(MET_GRADE_INFO) * DI0DI1_Num_File) != GradeCS)
                    DI0DI1_Num_File = 0;
            }
        }
    }

    return DI0DI1_Num_File;
}

/****************************************************************************
���ƣ� commfun_WriteGradeInfo
���ܣ�
-           ����վ�·��ġ�F106�����ܱ����ݷּ�������д�뵽˽�������
�������̣�
-           1����˽������ж�ȡ��ȡ��վ�Ѿ��·��Ĳ�����������Ϣ
-           2���������·��ķּ�����д��˽����棨���ݵȼ�Ϊ0�Ĳ��棩
-           3��������1��ȡ�Ĳ�����Ϣ�뱾���·��Ľ��бȶԣ�ȥ���ظ��Ĳ�����û���ظ���д�뵽˽�������
-           4������У���
���������
-           GradeNum        ���ݷּ����
-           pInbuf          ��վ�·���F106����
���������
-           ��
���أ�
-           ��
****************************************************************************/
void commfun_WriteGradeInfo(uint16 GradeNum, char *pInbuf)
{
    char GradeFile[255];                //˽���ļ�����
    uint16 DI0DI1_Num = pInbuf[0];      //���δ�����������ʶ
    uint16 DI0DI1_Num_File = 0;         //˽������д�ŵ��������ʶ����
    uint16 DI0DI1_Num_Write = 0;        //�����ļ��ķ�����������
    uint16 GradeCS = 0;                 //У���
    int i, j;

    if(GradeNum == 0 || GradeNum >= 255 || DI0DI1_Num == 0)
        return;

    memset(GradeFile, 0x0, sizeof(GradeFile));
    GetGradeFileName(GradeNum, GradeFile);

    //�����վ�·��ĵ��ܱ����ݷּ�����
    MET_GRADE_INFO dealGradeInfo[DI0DI1_Num];
    memcpy((void *)&dealGradeInfo, pInbuf + 1, sizeof(MET_GRADE_INFO) * DI0DI1_Num);

    DI0DI1_Num_File = GetGradeParaCount(GradeNum);      //��˽������ж�ȡ���Ѿ��·����������ʶ����

    MET_GRADE_INFO writeGradeInfo[DI0DI1_Num + DI0DI1_Num_File];      //׼��д��˽������
    memset((void *)writeGradeInfo, 0x0, sizeof(writeGradeInfo));

    DI0DI1_Num_Write = 0;
    //�������·������ݷּ��������������
    for(i = 0; i < DI0DI1_Num; i++)
    {
        if(dealGradeInfo[i].MtrGrade != 0)
        {
            memcpy((void *)&writeGradeInfo[DI0DI1_Num_Write], (void *)&dealGradeInfo[i], sizeof(MET_GRADE_INFO));
            DI0DI1_Num_Write++;
        }
    }

    if(DI0DI1_Num_File > 0)
    {
        MET_GRADE_INFO FileGradeInfo[DI0DI1_Num_File];      //�ļ��д�ŵ����ݷּ�����
        ReadPDataXram(GradeFile, (char *)FileGradeInfo, 2, sizeof(MET_GRADE_INFO) * DI0DI1_Num_File);

        for(i = 0; i < DI0DI1_Num_File; i++)
        {
            //��֤�Ѿ��·��Ĳ����Ƿ��Ϊ��Ч����
            if(FileGradeInfo[i].MtrGrade == 0xff || FileGradeInfo[i].MtrGrade == 0 || FileGradeInfo[i].MtrDI0DI1 == 0xffffffffl)
            {
                FileGradeInfo[i].MtrGrade = 0xff;
                FileGradeInfo[i].MtrDI0DI1 = 0xffffffffl;
                continue;
            }

            memcpy((void *)&writeGradeInfo[DI0DI1_Num_Write], (void *)&FileGradeInfo[i], sizeof(MET_GRADE_INFO));
            DI0DI1_Num_Write++;

            //���˽������д�����Ч�ķ������������Ҫ�����еĲ������бȽϣ�ɾ���ظ�����Ч�Ĳ���
            for(j = 0; j < DI0DI1_Num; j++)
            {
                //������������ظ����������ݵȼ��Ƿ�Ϊ0��������Ҫ�ٴ���
                if(FileGradeInfo[i].MtrDI0DI1 == dealGradeInfo[j].MtrDI0DI1)
                {
                    DI0DI1_Num_Write--;
                    break;
                }
            }
        }
    }

    RemovePDataXram(GradeFile);         //�Ƴ����ļ�������д��
    if(DI0DI1_Num_Write > 0)
    {
        WritePDataXram(GradeFile, (char *)&DI0DI1_Num_Write, 0, sizeof(uint16));                            //�������ʶ����
        WritePDataXram(GradeFile, (char *)writeGradeInfo, 2, sizeof(MET_GRADE_INFO) * DI0DI1_Num_Write);    //�����ܱ����ݷּ�������
        GradeCS = GetGradeCS((char *)writeGradeInfo, sizeof(MET_GRADE_INFO) * DI0DI1_Num_Write);
        WritePDataXram(GradeFile, (char *)&GradeCS, 2 + sizeof(MET_GRADE_INFO) * DI0DI1_Num_Write, 2);      //У���
    }
}

//��˽������ж�ȡ��F106�����ܱ����ݷּ�������
int commfun_ReadGradeInfo(uint16 GradeNum, char *pOutbuf)
{
    uint16 DI0DI1_Num_File = 0;         //˽������д�ŵ��������ʶ����
    char GradeFile[255];                //˽���ļ�����

    if(GradeNum == 0 || GradeNum >= 255)
        return -1;

    DI0DI1_Num_File = GetGradeParaCount(GradeNum);      //��˽������ж�ȡ���Ѿ��·����������ʶ����

    if(DI0DI1_Num_File == 0)
        return -1;

    memset(GradeFile, 0x0, sizeof(GradeFile));
    GetGradeFileName(GradeNum, GradeFile);

    pOutbuf[0] = DI0DI1_Num_File;
    ReadPDataXram(GradeFile, pOutbuf + 1, 2, sizeof(MET_GRADE_INFO) * DI0DI1_Num_File);

    return 1 + sizeof(MET_GRADE_INFO) * DI0DI1_Num_File;
}

//������ʼ����ʱ����������ܱ����ݷּ�������
//GradeNum 0xffff-ɾ�����в���  ����-ɾ����Ӧ����ȼ�����
void commfun_ClearGradeInfo(uint16 GradeNum)
{
    int i;
    char GradeFile[255];                //˽���ļ�����

    if(GradeNum == 0xffff || GradeNum > 255)
    {
        for(i = 0; i < 255; i++)
        {
            GetGradeFileName(i, GradeFile);
            RemovePDataXram(GradeFile);
        }
    }
    else
    {
        GetGradeFileName(GradeNum, GradeFile);
        RemovePDataXram(GradeFile);
    }
}

//д�롰F107�����ܱ����ݷּ����ڲ�����
void commfun_WriteEventReadCycle(uint16 GradeNum, uint8 CycleVal, uint8 CycleUnit)
{
    EVENT_READ_CYCLE ReadCycle[256];
    int bRet = 0;

    if(GradeNum == 0 || GradeNum > 256)
        return;

    bRet = ReadXPara(TMN_PARA_EXTEND, EventReadCycle_ID, (char *)ReadCycle, 0);
    if(bRet < 0)
        memset((void *)ReadCycle, 0x0, sizeof(ReadCycle));
    ReadCycle[GradeNum - 1].CycleUnit = CycleUnit;
    ReadCycle[GradeNum - 1].CycleVal = CycleVal;
    WriteXPara(TMN_PARA_EXTEND, EventReadCycle_ID, (char *)ReadCycle, 0);
    return;
}

//��ȡ��F107�����ܱ����ݷּ����ڲ�����
int commfun_ReadEventReadCycle(uint16 GradeNum, EVENT_READ_CYCLE *pReadCycle)
{
    EVENT_READ_CYCLE ReadCycle[256];
    int bRet = 0;

    if(GradeNum == 0 || GradeNum > 256)
        return -1;

    bRet = ReadXPara(TMN_PARA_EXTEND, EventReadCycle_ID, (char *)ReadCycle, 0);
    if(bRet < 0)
        return -1;
    pReadCycle->CycleUnit = ReadCycle[GradeNum - 1].CycleUnit;
    pReadCycle->CycleVal = ReadCycle[GradeNum - 1].CycleVal;
    return sizeof(EVENT_READ_CYCLE);
}

//���ܱ��¼����������������ֵ���645��Լ��ʽ�Ķ�Ӧ��ϵ
#if 1
#if (SHANXI1_FUN == 1 && HARDWARE_TYPE == 1) //������������Ҫ���ϱ��¼�ʱ���ϱ������������
const EventID_To_DI EventIDToDI[] = {
    //���ɿ����󶯻�ܶ�
    {RT_DATA,  0,  E_EVENT_COUNT_00_ID,     0x03360000,     0xffffffff,     1,
    {0x03360001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //����Ǵ���
    {RT_DATA,  10,  E_EVENT_COUNT_10_ID,    0x03300D00,     0xffffffff,     1,
    {0x03300D01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //����ť�Ǵ���
    {RT_DATA,  11,  E_EVENT_COUNT_11_ID,    0x03300E00,     0xffffffff,     1,
    {0x03300E01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�ų�����
    {RT_DATA,  12,  E_EVENT_COUNT_12_ID,    0x03350000,     0xffffffff,     1,
    {0x03350001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��Դ�쳣
    {RT_DATA,  13,  E_EVENT_COUNT_13_ID,    0x03370000,     0xffffffff,     1,
    {0x03370001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��բ�ɹ�
    {RT_DATA,  14,  E_EVENT_COUNT_14_ID,    0x1D000001,     0xffffffff,     1,
    {0x1D000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��բ�ɹ�
    {RT_DATA,  15,  E_EVENT_COUNT_15_ID,    0x1E000001,     0xffffffff,     1,
    {0x1E000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A��ʧѹ
    {RT_DATA,  16,  E_EVENT_COUNT_16_ID,    0x10010001,     0x0000ff00,     2,
    {0x10010002, 0x1001ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A��Ƿѹ
    {RT_DATA,  17,  E_EVENT_COUNT_17_ID,    0x11010001,     0x0000ff00,     2,
    {0x11010002, 0x1101ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A���ѹ
    {RT_DATA,  18,  E_EVENT_COUNT_18_ID,    0x12010001,     0x0000ff00,     2,
    {0x12010101, 0x12012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A��ʧ��
    {RT_DATA,  19,  E_EVENT_COUNT_19_ID,    0x18010001,     0x0000ff00,     2,
    {0x18010002, 0x1801ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�����
    {RT_DATA,  20,  E_EVENT_COUNT_20_ID,    0x19010001,     0x0000ff00,     2,
    {0x19010002, 0x1901ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�����
    {RT_DATA,  21,  E_EVENT_COUNT_21_ID,    0x1C010001,     0x0000ff00,     2,
    {0x1C010101, 0x1C011201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�๦�ʷ���
    {RT_DATA,  22,  E_EVENT_COUNT_22_ID,    0x1B010001,     0x0000ff00,     2,
    {0x1B010101, 0x1B011201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�����
    {RT_DATA,  23,  E_EVENT_COUNT_23_ID,    0x13010001,     0x0000ff00,     2,
    {0x13010002, 0x1301ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�����
    {RT_DATA,  24,  E_EVENT_COUNT_24_ID,    0x1A010001,     0x0000ff00,     2,
    {0x1A010101, 0x1A012101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B��ʧѹ
    {RT_DATA,  32,  E_EVENT_COUNT_32_ID,    0x10020001,     0x0000ff00,     2,
    {0x10020002, 0x1002ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B��Ƿѹ
    {RT_DATA,  33,  E_EVENT_COUNT_33_ID,    0x11020001,     0x0000ff00,     2,
    {0x11020002, 0x1102ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B���ѹ
    {RT_DATA,  34,  E_EVENT_COUNT_34_ID,    0x12020001,     0x0000ff00,     2,
    {0x12020101, 0x12022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B��ʧ��
    {RT_DATA,  35,  E_EVENT_COUNT_35_ID,    0x18020001,     0x0000ff00,     2,
    {0x18020101, 0x18022101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�����
    {RT_DATA,  36,  E_EVENT_COUNT_36_ID,    0x19020001,     0x0000ff00,     2,
    {0x19020002, 0x1902ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�����
    {RT_DATA,  37,  E_EVENT_COUNT_37_ID,    0x1C020001,     0x0000ff00,     2,
    {0x1C020101, 0x1C021201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�๦�ʷ���
    {RT_DATA,  38,  E_EVENT_COUNT_38_ID,    0x1B020001,     0x0000ff00,     2,
    {0x1B020101, 0x1B021201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�����
    {RT_DATA,  39,  E_EVENT_COUNT_39_ID,    0x13020001,     0x0000ff00,     2,
    {0x13020002, 0x1302ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�����
    {RT_DATA,  40,  E_EVENT_COUNT_40_ID,    0x1A020001,     0x0000ff00,     2,
    {0x1A020101, 0x1A022101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C��ʧѹ
    {RT_DATA,  48,  E_EVENT_COUNT_48_ID,    0x10030001,     0x0000ff00,     2,
    {0x10030002, 0x1003ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C��Ƿѹ
    {RT_DATA,  49,  E_EVENT_COUNT_49_ID,    0x11030001,     0x0000ff00,     2,
    {0x11030002, 0x1103ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C���ѹ
    {RT_DATA,  50,  E_EVENT_COUNT_50_ID,    0x12030001,     0x0000ff00,     2,
    {0x12030101, 0x12032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C��ʧ��
    {RT_DATA,  51,  E_EVENT_COUNT_51_ID,    0x18030001,     0x0000ff00,     2,
    {0x18030101, 0x18032101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�����
    {RT_DATA,  52,  E_EVENT_COUNT_52_ID,    0x19030001,     0x0000ff00,    2,
    {0x19030002, 0x1903ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�����
    {RT_DATA,  53,  E_EVENT_COUNT_53_ID,    0x1C030001,     0x0000ff00,     2,
    {0x1C030101, 0x1C031201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�๦�ʷ���
    {RT_DATA,  54,  E_EVENT_COUNT_54_ID,    0x1B030001,     0x0000ff00,     2,
    {0x1B030101, 0x1B031201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�����
    {RT_DATA,  55,  E_EVENT_COUNT_55_ID,    0x13030001,     0x0000ff00,     2,
    {0x13030002, 0x1303ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�����
    {RT_DATA,  56,  E_EVENT_COUNT_56_ID,    0x1A030001,     0x0000ff00,     2,
    {0x1A030101, 0x1A032101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��ѹ������
    {RT_DATA,  64,  E_EVENT_COUNT_64_ID,    0x14000001,     0x0000ff00,     2,
    {0x14000101, 0x14001201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�����������
    {RT_DATA,  65,  E_EVENT_COUNT_65_ID,    0x15000001,     0x0000ff00,     2,
    {0x15000101, 0x15001201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��ѹ��ƽ�����
    {RT_DATA,  66,  E_EVENT_COUNT_66_ID,    0x16000001,     0x0000ff00,     2,
    {0x16000002, 0x1600ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //������ƽ�����
    {RT_DATA,  67,  E_EVENT_COUNT_67_ID,    0x17000001,     0x0000ff00,     2,
    {0x17000002, 0x1700ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //������Դʧ��
    {RT_DATA,  68,  E_EVENT_COUNT_68_ID,    0x03060000,     0xffffffff,     1,
    {0x03060001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�������
    {RT_DATA,  69,  E_EVENT_COUNT_69_ID,    0x03110000,     0xffffffff,     1,
    {0x03110001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��������
    {RT_DATA,  70,  E_EVENT_COUNT_70_ID,    0x03120000,     0xffffffff,     6,
    {0x03120101, 0x03120201, 0x03120301, 0x03120401, 0x03120501, 0x03120601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�ܹ�������������
    {RT_DATA,  71,  E_EVENT_COUNT_71_ID,    0x1F000001,     0x0000ff00,     2,
    {0x1F000101, 0x1F000601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�������ز�ƽ��
    {RT_DATA,  72,  E_EVENT_COUNT_72_ID,    0x20000001,     0x0000ff00,     2,
    {0x20000101, 0x20001301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��������
    {RT_DATA,  73,  E_EVENT_COUNT_73_ID,    0x21000000,     0xffffffff,     1,
    {0x21000001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //ȫʧѹ
    {RT_DATA,  74,  E_EVENT_COUNT_74_ID,    0x03050000,     0xffffffff,     1,
    {0x03050001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��̴���
    {RT_DATA,  80,  E_EVENT_COUNT_75_ID,    0x03300000,     0xffffffff,     1,
    {0x03300001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�������
    {RT_DATA,  81,  E_EVENT_COUNT_81_ID,    0x03300100,     0xffffffff,     1,
    {0x03300101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�������������������̫���ˣ��������¼���¼�ˣ�
    {RT_DATA,  82,  E_EVENT_COUNT_82_ID,    0x03300200,     0xffffffff,     1,
    //{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    {0x03300201, 0x03300202, 0x03300203, 0x03300204, 0x03300205, 0x03300206, 0x03300207, 0x03300208, 0x03300209, 0x0330020A}},

    //�¼�����
    {RT_DATA,  83,  E_EVENT_COUNT_83_ID,    0x03300300,     0xffffffff,     1,
    {0x03300301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //Уʱ
    {RT_DATA,  84,  E_EVENT_COUNT_84_ID,    0x03300400,     0xffffffff,     1,
    {0x03300401, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //ʱ�α���
    {RT_DATA,  85,  E_EVENT_COUNT_85_ID,    0x03300500,     0xffffffff,     1,
    //{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    {0x03300501, 0x03300502, 0x03300503, 0x03300504, 0x03300505, 0x03300506, 0x03300507, 0x03300508, 0x03300509, 0x0330050A}},

    //ʱ������
    {RT_DATA,  86,  E_EVENT_COUNT_86_ID,    0x03300600,     0xffffffff,     1,
    {0x03300601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�����ձ��
    {RT_DATA,  87,  E_EVENT_COUNT_87_ID,    0x03300700,     0xffffffff,     1,
    {0x03300701, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�ڼ��ձ��
    {RT_DATA,  88,  E_EVENT_COUNT_88_ID,    0x03300800,     0xffffffff,     1,
    {0x03300801, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�й���Ϸ�ʽ���
    {RT_DATA,  89,  E_EVENT_COUNT_89_ID,    0x03300900,     0xffffffff,     1,
    {0x03300901, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�޹���Ϸ�ʽ1���
    {RT_DATA,  90,  E_EVENT_COUNT_90_ID,    0x03300A00,     0xffffffff,     1,
    {0x03300A01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�޹���Ϸ�ʽ2���
    {RT_DATA,  91,  E_EVENT_COUNT_91_ID,    0x03300B00,     0xffffffff,     1,
    {0x03300B01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�����ձ��
    {RT_DATA,  92,  E_EVENT_COUNT_92_ID,    0x03300C00,     0xffffffff,     1,
    {0x03300C01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //���ʲ�������
    {RT_DATA,  93,  E_EVENT_COUNT_93_ID,    0x03300F00,     0xffffffff,     1,
    {0x03300F01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //���ݱ���
    {RT_DATA,  94,  E_EVENT_COUNT_94_ID,    0x03301000,     0xffffffff,     1,
    {0x03301001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��Կ����
    {RT_DATA,  95,  E_EVENT_COUNT_95_ID,    0x03301200,     0xffffffff,     1,
    {0x03301201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    {0xff,   0xff,  0xffff,                 0xffffffff,     0xffffffff,     0xff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
};
#elif (HUABEI_FUN == 1 && HARDWARE_TYPE == 4)
const EventID_To_DI EventIDToDI[] = {
    //���ɿ����󶯻�ܶ�
    {RT_DATA,  0,  E_EVENT_COUNT_00_ID,     0x03360000,     0xffffffff,     1,
    {0x03360001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //����Ǵ���
    {RT_DATA,  10,  E_EVENT_COUNT_10_ID,    0x03300D00,     0xffffffff,     1,
    {0x03300D01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //����ť�Ǵ���
    {RT_DATA,  11,  E_EVENT_COUNT_11_ID,    0x03300E00,     0xffffffff,     1,
    {0x03300E01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�ų�����
    {RT_DATA,  12,  E_EVENT_COUNT_12_ID,    0x03350000,     0xffffffff,     1,
    {0x03350001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��Դ�쳣
    {RT_DATA,  13,  E_EVENT_COUNT_13_ID,    0x03370000,     0xffffffff,     1,
    {0x03370001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��բ�ɹ�
    {RT_DATA,  14,  E_EVENT_COUNT_14_ID,    0x1D000001,     0xffffffff,     1,
    {0x1D000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��բ�ɹ�
    {RT_DATA,  15,  E_EVENT_COUNT_15_ID,    0x1E000001,     0xffffffff,     1,
    {0x1E000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A��ʧѹ
    {RT_DATA,  16,  E_EVENT_COUNT_16_ID,    0x10010001,     0x0000ff00,     2,
    {0x10010002, 0x1001ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A��Ƿѹ
    {RT_DATA,  17,  E_EVENT_COUNT_17_ID,    0x11010001,     0x0000ff00,     2,
    {0x11010002, 0x1101ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A���ѹ
    {RT_DATA,  18,  E_EVENT_COUNT_18_ID,    0x12010001,     0x0000ff00,     2,
    {0x12010002, 0x1201ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A��ʧ��
    {RT_DATA,  19,  E_EVENT_COUNT_19_ID,    0x18010001,     0x0000ff00,     2,
    {0x18010002, 0x1801ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�����
    {RT_DATA,  20,  E_EVENT_COUNT_20_ID,    0x19010001,     0x0000ff00,     2,
    {0x19010002, 0x1901ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�����
    {RT_DATA,  21,  E_EVENT_COUNT_21_ID,    0x1C010001,     0x0000ff00,     2,
    {0x1C010002, 0x1C01ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�๦�ʷ���
    {RT_DATA,  22,  E_EVENT_COUNT_22_ID,    0x1B010001,     0x0000ff00,     2,
    {0x1B010002, 0x1B01ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�����
    {RT_DATA,  23,  E_EVENT_COUNT_23_ID,    0x13010001,     0x0000ff00,     2,
    {0x13010002, 0x1301ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�����
    {RT_DATA,  24,  E_EVENT_COUNT_24_ID,    0x1A010001,     0x0000ff00,     2,
    {0x1A010002, 0x1A01ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B��ʧѹ
    {RT_DATA,  32,  E_EVENT_COUNT_32_ID,    0x10020001,     0x0000ff00,     2,
    {0x10020002, 0x1002ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B��Ƿѹ
    {RT_DATA,  33,  E_EVENT_COUNT_33_ID,    0x11020001,     0x0000ff00,     2,
    {0x11020002, 0x1102ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B���ѹ
    {RT_DATA,  34,  E_EVENT_COUNT_34_ID,    0x12020001,     0x0000ff00,     2,
    {0x12020002, 0x1202ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B��ʧ��
    {RT_DATA,  35,  E_EVENT_COUNT_35_ID,    0x18020001,     0x0000ff00,     2,
    {0x18020002, 0x1802ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�����
    {RT_DATA,  36,  E_EVENT_COUNT_36_ID,    0x19020001,     0x0000ff00,     2,
    {0x19020002, 0x1902ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�����
    {RT_DATA,  37,  E_EVENT_COUNT_37_ID,    0x1C020001,     0x0000ff00,     2,
    {0x1C020002, 0x1C02ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�๦�ʷ���
    {RT_DATA,  38,  E_EVENT_COUNT_38_ID,    0x1B020001,     0x0000ff00,     2,
    {0x1B020002, 0x1B02ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�����
    {RT_DATA,  39,  E_EVENT_COUNT_39_ID,    0x13020001,     0x0000ff00,     2,
    {0x13020002, 0x1302ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�����
    {RT_DATA,  40,  E_EVENT_COUNT_40_ID,    0x1A020001,     0x0000ff00,     2,
    {0x1A020002, 0x1A02ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C��ʧѹ
    {RT_DATA,  48,  E_EVENT_COUNT_48_ID,    0x10030001,     0x0000ff00,     2,
    {0x10030002, 0x1003ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C��Ƿѹ
    {RT_DATA,  49,  E_EVENT_COUNT_49_ID,    0x11030001,     0x0000ff00,     2,
    {0x11030002, 0x1103ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C���ѹ
    {RT_DATA,  50,  E_EVENT_COUNT_50_ID,    0x12030001,     0x0000ff00,     2,
    {0x12030002, 0x1203ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C��ʧ��
    {RT_DATA,  51,  E_EVENT_COUNT_51_ID,    0x18030001,     0x0000ff00,     2,
    {0x18030002, 0x1803ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�����
    {RT_DATA,  52,  E_EVENT_COUNT_52_ID,    0x19030001,     0x0000ff00,    2,
    {0x19030002, 0x1903ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�����
    {RT_DATA,  53,  E_EVENT_COUNT_53_ID,    0x1C030001,     0x0000ff00,     2,
    {0x1C030002, 0x1C03ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�๦�ʷ���
    {RT_DATA,  54,  E_EVENT_COUNT_54_ID,    0x1B030001,     0x0000ff00,     2,
    {0x1B030002, 0x1B03ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�����
    {RT_DATA,  55,  E_EVENT_COUNT_55_ID,    0x13030001,     0x0000ff00,     2,
    {0x13030002, 0x1303ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�����
    {RT_DATA,  56,  E_EVENT_COUNT_56_ID,    0x1A030001,     0x0000ff00,     2,
    {0x1A030002, 0x1A03ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��ѹ������
    {RT_DATA,  64,  E_EVENT_COUNT_64_ID,    0x14000001,     0x0000ff00,     2,
    {0x14000002, 0x1400ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�����������
    {RT_DATA,  65,  E_EVENT_COUNT_65_ID,    0x15000001,     0x0000ff00,     2,
    {0x15000002, 0x1500ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��ѹ��ƽ�����
    {RT_DATA,  66,  E_EVENT_COUNT_66_ID,    0x16000001,     0x0000ff00,     2,
    {0x16000002, 0x1600ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //������ƽ�����
    {RT_DATA,  67,  E_EVENT_COUNT_67_ID,    0x17000001,     0x0000ff00,     2,
    {0x17000002, 0x1700ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //������Դʧ��
    {RT_DATA,  68,  E_EVENT_COUNT_68_ID,    0x03060000,     0xffffffff,     1,
    {0x03060001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�������
    {RT_DATA,  69,  E_EVENT_COUNT_69_ID,    0x03110000,     0xffffffff,     1,
    {0x03110001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��������
    {RT_DATA,  70,  E_EVENT_COUNT_70_ID,    0x03120000,     0xffffffff,     6,
    {0x03120101, 0x03120201, 0x03120301, 0x03120401, 0x03120501, 0x03120601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�ܹ�������������
    {RT_DATA,  71,  E_EVENT_COUNT_71_ID,    0x1F000001,     0x0000ff00,     2,
    {0x1F000101, 0x1F000601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�������ز�ƽ��
    {RT_DATA,  72,  E_EVENT_COUNT_72_ID,    0x20000001,     0x0000ff00,     2,
    {0x20000101, 0x20001301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��������
    {RT_DATA,  73,  E_EVENT_COUNT_73_ID,    0x21000000,     0xffffffff,     1,
    {0x21000001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //ȫʧѹ
    {RT_DATA,  74,  E_EVENT_COUNT_74_ID,    0x03050000,     0xffffffff,     1,
    {0x03050001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��̴���
    {RT_DATA,  80,  E_EVENT_COUNT_75_ID,    0x03300000,     0xffffffff,     1,
    {0x03300001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�������
    {RT_DATA,  81,  E_EVENT_COUNT_81_ID,    0x03300100,     0xffffffff,     1,
    {0x03300101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�������������������̫���ˣ��������¼���¼�ˣ�
    {RT_DATA,  82,  E_EVENT_COUNT_82_ID,    0x03300200,     0xffffffff,     1,
    //{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    {0x03300201, 0x03300202, 0x03300203, 0x03300204, 0x03300205, 0x03300206, 0x03300207, 0x03300208, 0x03300209, 0x0330020A}},

    //�¼�����
    {RT_DATA,  83,  E_EVENT_COUNT_83_ID,    0x03300300,     0xffffffff,     1,
    {0x03300301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //Уʱ
    {RT_DATA,  84,  E_EVENT_COUNT_84_ID,    0x03300400,     0xffffffff,     1,
    {0x03300401, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //ʱ�α���
    {RT_DATA,  85,  E_EVENT_COUNT_85_ID,    0x03300500,     0xffffffff,     1,
    //{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    {0x03300501, 0x03300502, 0x03300503, 0x03300504, 0x03300505, 0x03300506, 0x03300507, 0x03300508, 0x03300509, 0x0330050A}},

    //ʱ������
    {RT_DATA,  86,  E_EVENT_COUNT_86_ID,    0x03300600,     0xffffffff,     1,
    {0x03300601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�����ձ��
    {RT_DATA,  87,  E_EVENT_COUNT_87_ID,    0x03300700,     0xffffffff,     1,
    {0x03300701, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�ڼ��ձ��
    {RT_DATA,  88,  E_EVENT_COUNT_88_ID,    0x03300800,     0xffffffff,     1,
    {0x03300801, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�й���Ϸ�ʽ���
    {RT_DATA,  89,  E_EVENT_COUNT_89_ID,    0x03300900,     0xffffffff,     1,
    {0x03300901, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�޹���Ϸ�ʽ1���
    {RT_DATA,  90,  E_EVENT_COUNT_90_ID,    0x03300A00,     0xffffffff,     1,
    {0x03300A01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�޹���Ϸ�ʽ2���
    {RT_DATA,  91,  E_EVENT_COUNT_91_ID,    0x03300B00,     0xffffffff,     1,
    {0x03300B01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�����ձ��
    {RT_DATA,  92,  E_EVENT_COUNT_92_ID,    0x03300C00,     0xffffffff,     1,
    {0x03300C01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //���ʲ�������
    {RT_DATA,  93,  E_EVENT_COUNT_93_ID,    0x03300F00,     0xffffffff,     1,
    {0x03300F01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //���ݱ���
    {RT_DATA,  94,  E_EVENT_COUNT_94_ID,    0x03301000,     0xffffffff,     1,
    {0x03301001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��Կ����
    {RT_DATA,  95,  E_EVENT_COUNT_95_ID,    0x03301200,     0xffffffff,     1,
    {0x03301201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    {0xff,   0xff,  0xffff,                 0xffffffff,     0xffffffff,     0xff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
};
#else
const EventID_To_DI EventIDToDI[] = {
    //���ɿ����󶯻�ܶ�
    {RT_DATA,  0,  E_EVENT_COUNT_00_ID,     0x03360000,     0xffffffff,     1,
    {0x03360001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //����Ǵ���
    {RT_DATA,  10,  E_EVENT_COUNT_10_ID,    0x03300D00,     0xffffffff,     1,
    {0x03300D01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //����ť�Ǵ���
    {RT_DATA,  11,  E_EVENT_COUNT_11_ID,    0x03300E00,     0xffffffff,     1,
    {0x03300E01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�ų�����
    {RT_DATA,  12,  E_EVENT_COUNT_12_ID,    0x03350000,     0xffffffff,     1,
    {0x03350001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��Դ�쳣
    {RT_DATA,  13,  E_EVENT_COUNT_13_ID,    0x03370000,     0xffffffff,     1,
    {0x03370001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��բ�ɹ�
    {RT_DATA,  14,  E_EVENT_COUNT_14_ID,    0x1D000001,     0xffffffff,     1,
    {0x1D000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��բ�ɹ�
    {RT_DATA,  15,  E_EVENT_COUNT_15_ID,    0x1E000001,     0xffffffff,     1,
    {0x1E000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A��ʧѹ
    {RT_DATA,  16,  E_EVENT_COUNT_16_ID,    0x10010001,     0x0000ff00,     2,
    {0x10010101, 0x10012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A��Ƿѹ
    {RT_DATA,  17,  E_EVENT_COUNT_17_ID,    0x11010001,     0x0000ff00,     2,
    {0x11010101, 0x11012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A���ѹ
    {RT_DATA,  18,  E_EVENT_COUNT_18_ID,    0x12010001,     0x0000ff00,     2,
    {0x12010101, 0x12012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A��ʧ��
    {RT_DATA,  19,  E_EVENT_COUNT_19_ID,    0x18010001,     0x0000ff00,     2,
    {0x18010101, 0x18012101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�����
    {RT_DATA,  20,  E_EVENT_COUNT_20_ID,    0x19010001,     0x0000ff00,     2,
    {0x19010101, 0x19012101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�����
    {RT_DATA,  21,  E_EVENT_COUNT_21_ID,    0x1C010001,     0x0000ff00,     2,
    {0x1C010101, 0x1C011201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�๦�ʷ���
    {RT_DATA,  22,  E_EVENT_COUNT_22_ID,    0x1B010001,     0x0000ff00,     2,
    {0x1B010101, 0x1B011201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�����
    {RT_DATA,  23,  E_EVENT_COUNT_23_ID,    0x13010001,     0x0000ff00,     2,
    {0x13010101, 0x13012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A�����
    {RT_DATA,  24,  E_EVENT_COUNT_24_ID,    0x1A010001,     0x0000ff00,     2,
    {0x1A010101, 0x1A012101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B��ʧѹ
    {RT_DATA,  32,  E_EVENT_COUNT_32_ID,    0x10020001,     0x0000ff00,     2,
    {0x10020101, 0x10022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B��Ƿѹ
    {RT_DATA,  33,  E_EVENT_COUNT_33_ID,    0x11020001,     0x0000ff00,     2,
    {0x11020101, 0x11022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B���ѹ
    {RT_DATA,  34,  E_EVENT_COUNT_34_ID,    0x12020001,     0x0000ff00,     2,
    {0x12020101, 0x12022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B��ʧ��
    {RT_DATA,  35,  E_EVENT_COUNT_35_ID,    0x18020001,     0x0000ff00,     2,
    {0x18020101, 0x18022101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�����
    {RT_DATA,  36,  E_EVENT_COUNT_36_ID,    0x19020001,     0x0000ff00,     2,
    {0x19020101, 0x19022101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�����
    {RT_DATA,  37,  E_EVENT_COUNT_37_ID,    0x1C020001,     0x0000ff00,     2,
    {0x1C020101, 0x1C021201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�๦�ʷ���
    {RT_DATA,  38,  E_EVENT_COUNT_38_ID,    0x1B020001,     0x0000ff00,     2,
    {0x1B020101, 0x1B021201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�����
    {RT_DATA,  39,  E_EVENT_COUNT_39_ID,    0x13020001,     0x0000ff00,     2,
    {0x13020101, 0x13022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B�����
    {RT_DATA,  40,  E_EVENT_COUNT_40_ID,    0x1A020001,     0x0000ff00,     2,
    {0x1A020101, 0x1A022101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C��ʧѹ
    {RT_DATA,  48,  E_EVENT_COUNT_48_ID,    0x10030001,     0x0000ff00,     2,
    {0x10030101, 0x10032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C��Ƿѹ
    {RT_DATA,  49,  E_EVENT_COUNT_49_ID,    0x11030001,     0x0000ff00,     2,
    {0x11030101, 0x11032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C���ѹ
    {RT_DATA,  50,  E_EVENT_COUNT_50_ID,    0x12030001,     0x0000ff00,     2,
    {0x12030101, 0x12032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C��ʧ��
    {RT_DATA,  51,  E_EVENT_COUNT_51_ID,    0x18030001,     0x0000ff00,     2,
    {0x18030101, 0x18032101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�����
    {RT_DATA,  52,  E_EVENT_COUNT_52_ID,    0x19030001,     0x0000ff00,     2,
    {0x19030101, 0x19032101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�����
    {RT_DATA,  53,  E_EVENT_COUNT_53_ID,    0x1C030001,     0x0000ff00,     2,
    {0x1C030101, 0x1C031201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�๦�ʷ���
    {RT_DATA,  54,  E_EVENT_COUNT_54_ID,    0x1B030001,     0x0000ff00,     2,
    {0x1B030101, 0x1B031201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�����
    {RT_DATA,  55,  E_EVENT_COUNT_55_ID,    0x13030001,     0x0000ff00,     2,
    {0x13030101, 0x13032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C�����
    {RT_DATA,  56,  E_EVENT_COUNT_56_ID,    0x1A030001,     0x0000ff00,     2,
    {0x1A030101, 0x1A032101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��ѹ������
    {RT_DATA,  64,  E_EVENT_COUNT_64_ID,    0x14000001,     0x0000ff00,     2,
    {0x14000101, 0x14001201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�����������
    {RT_DATA,  65,  E_EVENT_COUNT_65_ID,    0x15000001,     0x0000ff00,     2,
    {0x15000101, 0x15001201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��ѹ��ƽ�����
    {RT_DATA,  66,  E_EVENT_COUNT_66_ID,    0x16000001,     0x0000ff00,     2,
    {0x16000101, 0x16001301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //������ƽ�����
    {RT_DATA,  67,  E_EVENT_COUNT_67_ID,    0x17000001,     0x0000ff00,     2,
    {0x17000101, 0x17001301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //������Դʧ��
    {RT_DATA,  68,  E_EVENT_COUNT_68_ID,    0x03060000,     0xffffffff,     1,
    {0x03060001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�������
    {RT_DATA,  69,  E_EVENT_COUNT_69_ID,    0x03110000,     0xffffffff,     1,
    {0x03110001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��������
    {RT_DATA,  70,  E_EVENT_COUNT_70_ID,    0x03120000,     0xffffffff,     6,
    {0x03120101, 0x03120201, 0x03120301, 0x03120401, 0x03120501, 0x03120601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�ܹ�������������
    {RT_DATA,  71,  E_EVENT_COUNT_71_ID,    0x1F000001,     0x0000ff00,     2,
    {0x1F000101, 0x1F000601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�������ز�ƽ��
    {RT_DATA,  72,  E_EVENT_COUNT_72_ID,    0x20000001,     0x0000ff00,     2,
    {0x20000101, 0x20001301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��������
    {RT_DATA,  73,  E_EVENT_COUNT_73_ID,    0x21000000,     0xffffffff,     1,
    {0x21000001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //ȫʧѹ
    {RT_DATA,  74,  E_EVENT_COUNT_74_ID,    0x03050000,     0xffffffff,     1,
    {0x03050001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��̴���
    {RT_DATA,  80,  E_EVENT_COUNT_75_ID,    0x03300000,     0xffffffff,     1,
    {0x03300001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�������
    {RT_DATA,  81,  E_EVENT_COUNT_81_ID,    0x03300100,     0xffffffff,     1,
    {0x03300101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�������������������̫���ˣ��������¼���¼�ˣ�
    {RT_DATA,  82,  E_EVENT_COUNT_82_ID,    0x03300200,     0xffffffff,     1,
    //{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    {0x03300201, 0x03300202, 0x03300203, 0x03300204, 0x03300205, 0x03300206, 0x03300207, 0x03300208, 0x03300209, 0x0330020A}},

    //�¼�����
    {RT_DATA,  83,  E_EVENT_COUNT_83_ID,    0x03300300,     0xffffffff,     1,
    {0x03300301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //Уʱ
    {RT_DATA,  84,  E_EVENT_COUNT_84_ID,    0x03300400,     0xffffffff,     1,
    {0x03300401, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //ʱ�α���
    {RT_DATA,  85,  E_EVENT_COUNT_85_ID,    0x03300500,     0xffffffff,     1,
    //{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    {0x03300501, 0x03300502, 0x03300503, 0x03300504, 0x03300505, 0x03300506, 0x03300507, 0x03300508, 0x03300509, 0x0330050A}},

    //ʱ������
    {RT_DATA,  86,  E_EVENT_COUNT_86_ID,    0x03300600,     0xffffffff,     1,
    {0x03300601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�����ձ��
    {RT_DATA,  87,  E_EVENT_COUNT_87_ID,    0x03300700,     0xffffffff,     1,
    {0x03300701, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�ڼ��ձ��
    {RT_DATA,  88,  E_EVENT_COUNT_88_ID,    0x03300800,     0xffffffff,     1,
    {0x03300801, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�й���Ϸ�ʽ���
    {RT_DATA,  89,  E_EVENT_COUNT_89_ID,    0x03300900,     0xffffffff,     1,
    {0x03300901, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�޹���Ϸ�ʽ1���
    {RT_DATA,  90,  E_EVENT_COUNT_90_ID,    0x03300A00,     0xffffffff,     1,
    {0x03300A01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�޹���Ϸ�ʽ2���
    {RT_DATA,  91,  E_EVENT_COUNT_91_ID,    0x03300B00,     0xffffffff,     1,
    {0x03300B01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //�����ձ��
    {RT_DATA,  92,  E_EVENT_COUNT_92_ID,    0x03300C00,     0xffffffff,     1,
    {0x03300C01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //���ʲ�������
    {RT_DATA,  93,  E_EVENT_COUNT_93_ID,    0x03300F00,     0xffffffff,     1,
    {0x03300F01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //���ݱ���
    {RT_DATA,  94,  E_EVENT_COUNT_94_ID,    0x03301000,     0xffffffff,     1,
    {0x03301001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //��Կ����
    {RT_DATA,  95,  E_EVENT_COUNT_95_ID,    0x03301200,     0xffffffff,     1,
    {0x03301201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    {0xff,   0xff,  0xffff,                 0xffffffff,     0xffffffff,     0xff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
};
#endif
#else
const EventID_To_DI EventIDToDI[] = {
    //���ɿ����󶯻�ܶ�
    {RT_DATA,  0,  E_EVENT_COUNT_00_ID,     0x03360000,     0xffffffff,
    {0x03360001, 0x03360002, 0x03360003, 0x03360004, 0x03360005, 0x03360006, 0x03360007, 0x03360008, 0x03360009, 0x0336000A}},

    //����Ǵ���
    {RT_DATA,  10,  E_EVENT_COUNT_10_ID,    0x03300D00,     0xffffffff,
    {0x03300D01, 0x03300D02, 0x03300D03, 0x03300D04, 0x03300D05, 0x03300D06, 0x03300D07, 0x03300D08, 0x03300D09, 0x03300D0A}},

    //����ť�Ǵ���
    {RT_DATA,  11,  E_EVENT_COUNT_11_ID,    0x03300E00,     0xffffffff,
    {0x03300E01, 0x03300E02, 0x03300E03, 0x03300E04, 0x03300E05, 0x03300E06, 0x03300E07, 0x03300E08, 0x03300E09, 0x03300E0A}},

    //�ų�����
    {RT_DATA,  12,  E_EVENT_COUNT_12_ID,    0x03350000,     0xffffffff,
    {0x03350001, 0x03350002, 0x03350003, 0x03350004, 0x03350005, 0x03350006, 0x03350007, 0x03350008, 0x03350009, 0x0335000A}},

    //��Դ�쳣
    {RT_DATA,  13,  E_EVENT_COUNT_13_ID,    0x03370000,     0xffffffff,
    {0x03370001, 0x03370002, 0x03370003, 0x03370004, 0x03370005, 0x03370006, 0x03370007, 0x03370008, 0x03370009, 0x0337000A}},

    //��բ�ɹ�
    {RT_DATA,  14,  E_EVENT_COUNT_14_ID,    0x1D000001,     0xffffffff,
    {0x1D00FF01, 0x1D00FF02, 0x1D00FF03, 0x1D00FF04, 0x1D00FF05, 0x1D00FF06, 0x1D00FF07, 0x1D00FF08, 0x1D00FF09, 0x1D00FF0A}},

    //��բ�ɹ�
    {RT_DATA,  15,  E_EVENT_COUNT_15_ID,    0x1E000001,     0xffffffff,
    {0x1E00FF01, 0x1E00FF02, 0x1E00FF03, 0x1E00FF04, 0x1E00FF05, 0x1E00FF06, 0x1E00FF07, 0x1E00FF08, 0x1E00FF09, 0x1E00FF0A}},

    //A��ʧѹ
    {RT_DATA,  16,  E_EVENT_COUNT_16_ID,    0x10010001,     0x0000ff00,
    {0x10010101, 0x10010102, 0x10010103, 0x10010104, 0x10010105, 0x10010106, 0x10010107, 0x10010108, 0x10010109, 0x1001010A}},

    //A��Ƿѹ
    {RT_DATA,  17,  E_EVENT_COUNT_17_ID,    0x11010001,     0x0000ff00,
    {0x11010101, 0x11010102, 0x11010103, 0x11010104, 0x11010105, 0x11010106, 0x11010107, 0x11010108, 0x11010109, 0x1101010A}},

    //A���ѹ
    {RT_DATA,  18,  E_EVENT_COUNT_18_ID,    0x12010001,     0x0000ff00,
    {0x12010101, 0x12010102, 0x12010103, 0x12010104, 0x12010105, 0x12010106, 0x12010107, 0x12010108, 0x12010109, 0x1201010A}},

    //A��ʧ��
    {RT_DATA,  19,  E_EVENT_COUNT_19_ID,    0x18010001,     0x0000ff00,
    {0x18010101, 0x18010102, 0x18010103, 0x18010104, 0x18010105, 0x18010106, 0x18010107, 0x18010108, 0x18010109, 0x1801010A}},

    //A�����
    {RT_DATA,  20,  E_EVENT_COUNT_20_ID,    0x19010001,     0x0000ff00,
    {0x19010101, 0x19010102, 0x19010103, 0x19010104, 0x19010105, 0x19010106, 0x19010107, 0x19010108, 0x19010109, 0x1901010A}},

    //A�����
    {RT_DATA,  21,  E_EVENT_COUNT_21_ID,    0x1C010001,     0x0000ff00,
    {0x1C01FF01, 0x1C01FF02, 0x1C01FF03, 0x1C01FF04, 0x1C01FF05, 0x1C01FF06, 0x1C01FF07, 0x1C01FF08, 0x1C01FF09, 0x1C01FF0A}},

    //A�๦�ʷ���
    {RT_DATA,  22,  E_EVENT_COUNT_22_ID,    0x1B010001,     0x0000ff00,
    {0x1B01FF01, 0x1B01FF02, 0x1B01FF03, 0x1B01FF04, 0x1B01FF05, 0x1B01FF06, 0x1B01FF07, 0x1B01FF08, 0x1B01FF09, 0x1B01FF0A}},

    //A�����
    {RT_DATA,  23,  E_EVENT_COUNT_23_ID,    0x13010001,     0x0000ff00,
    {0x13010101, 0x13010102, 0x13010103, 0x13010104, 0x13010105, 0x13010106, 0x13010107, 0x13010108, 0x13010109, 0x1301010A}},

    //A�����
    {RT_DATA,  24,  E_EVENT_COUNT_24_ID,    0x1A010001,     0x0000ff00,
    {0x1A010101, 0x1A010102, 0x1A010103, 0x1A010104, 0x1A010105, 0x1A010106, 0x1A010107, 0x1A010108, 0x1A010109, 0x1A01010A}},

    //B��ʧѹ
    {RT_DATA,  32,  E_EVENT_COUNT_32_ID,    0x10020001,     0x0000ff00,
    {0x10020101, 0x10020102, 0x10020103, 0x10020104, 0x10020105, 0x10020106, 0x10020107, 0x10020108, 0x10020109, 0x1002010A}},

    //B��Ƿѹ
    {RT_DATA,  33,  E_EVENT_COUNT_33_ID,    0x11020001,     0x0000ff00,
    {0x11020101, 0x11020102, 0x11020103, 0x11020104, 0x11020105, 0x11020106, 0x11020107, 0x11020108, 0x11020109, 0x1102010A}},

    //B���ѹ
    {RT_DATA,  34,  E_EVENT_COUNT_34_ID,    0x12020001,     0x0000ff00,
    {0x12020101, 0x12020102, 0x12020103, 0x12020104, 0x12020105, 0x12020106, 0x12020107, 0x12020108, 0x12020109, 0x1202010A}},

    //B��ʧ��
    {RT_DATA,  35,  E_EVENT_COUNT_35_ID,    0x18020001,     0x0000ff00,
    {0x18020101, 0x18020102, 0x18020103, 0x18020104, 0x18020105, 0x18020106, 0x18020107, 0x18020108, 0x18020109, 0x1802010A}},

    //B�����
    {RT_DATA,  36,  E_EVENT_COUNT_36_ID,    0x19020001,     0x0000ff00,
    {0x19020101, 0x19020102, 0x19020103, 0x19020104, 0x19020105, 0x19020106, 0x19020107, 0x19020108, 0x19020109, 0x1902010A}},

    //B�����
    {RT_DATA,  37,  E_EVENT_COUNT_37_ID,    0x1C020001,     0x0000ff00,
    {0x1C02FF01, 0x1C02FF02, 0x1C02FF03, 0x1C02FF04, 0x1C02FF05, 0x1C02FF06, 0x1C02FF07, 0x1C02FF08, 0x1C02FF09, 0x1C02FF0A}},

    //B�๦�ʷ���
    {RT_DATA,  38,  E_EVENT_COUNT_38_ID,    0x1B020001,     0x0000ff00,
    {0x1B02FF01, 0x1B02FF02, 0x1B02FF03, 0x1B02FF04, 0x1B02FF05, 0x1B02FF06, 0x1B02FF07, 0x1B02FF08, 0x1B02FF09, 0x1B02FF0A}},

    //B�����
    {RT_DATA,  39,  E_EVENT_COUNT_39_ID,    0x13020001,     0x0000ff00,
    {0x13020101, 0x13020102, 0x13020103, 0x13020104, 0x13020105, 0x13020106, 0x13020107, 0x13020108, 0x13020109, 0x1302010A}},

    //B�����
    {RT_DATA,  40,  E_EVENT_COUNT_40_ID,    0x1A020001,     0x0000ff00,
    {0x1A020101, 0x1A020102, 0x1A020103, 0x1A020104, 0x1A020105, 0x1A020106, 0x1A020107, 0x1A020108, 0x1A020109, 0x1A02010A}},

    //C��ʧѹ
    {RT_DATA,  48,  E_EVENT_COUNT_48_ID,    0x10030001,     0x0000ff00,
    {0x10030101, 0x10030102, 0x10030103, 0x10030104, 0x10030105, 0x10030106, 0x10030107, 0x10030108, 0x10030109, 0x1003010A}},

    //C��Ƿѹ
    {RT_DATA,  49,  E_EVENT_COUNT_49_ID,    0x11030001,     0x0000ff00,
    {0x11030101, 0x11030102, 0x11030103, 0x11030104, 0x11030105, 0x11030106, 0x11030107, 0x11030108, 0x11030109, 0x1103010A}},

    //C���ѹ
    {RT_DATA,  50,  E_EVENT_COUNT_50_ID,    0x12030001,     0x0000ff00,
    {0x12030101, 0x12030102, 0x12030103, 0x12030104, 0x12030105, 0x12030106, 0x12030107, 0x12030108, 0x12030109, 0x1203010A}},

    //C��ʧ��
    {RT_DATA,  51,  E_EVENT_COUNT_51_ID,    0x18030001,     0x0000ff00,
    {0x18030101, 0x18030102, 0x18030103, 0x18030104, 0x18030105, 0x18030106, 0x18030107, 0x18030108, 0x18030109, 0x1803010A}},

    //C�����
    {RT_DATA,  52,  E_EVENT_COUNT_52_ID,    0x19030001,     0x0000ff00,
    {0x19030101, 0x19030102, 0x19030103, 0x19030104, 0x19030105, 0x19030106, 0x19030107, 0x19030108, 0x19030109, 0x1903010A}},

    //C�����
    {RT_DATA,  53,  E_EVENT_COUNT_53_ID,    0x1C030001,     0x0000ff00,
    {0x1C03FF01, 0x1C03FF02, 0x1C03FF03, 0x1C03FF04, 0x1C03FF05, 0x1C03FF06, 0x1C03FF07, 0x1C03FF08, 0x1C03FF09, 0x1C03FF0A}},

    //C�๦�ʷ���
    {RT_DATA,  54,  E_EVENT_COUNT_54_ID,    0x1B030001,     0x0000ff00,
    {0x1B03FF01, 0x1B03FF02, 0x1B03FF03, 0x1B03FF04, 0x1B03FF05, 0x1B03FF06, 0x1B03FF07, 0x1B03FF08, 0x1B03FF09, 0x1B03FF0A}},

    //C�����
    {RT_DATA,  55,  E_EVENT_COUNT_55_ID,    0x13030001,     0x0000ff00,
    {0x13030101, 0x13030102, 0x13030103, 0x13030104, 0x13030105, 0x13030106, 0x13030107, 0x13030108, 0x13030109, 0x1303010A}},

    //C�����
    {RT_DATA,  56,  E_EVENT_COUNT_56_ID,    0x1A030001,     0x0000ff00,
    {0x1A030101, 0x1A030102, 0x1A030103, 0x1A030104, 0x1A030105, 0x1A030106, 0x1A030107, 0x1A030108, 0x1A030109, 0x1A03010A}},

    //��ѹ������
    {RT_DATA,  64,  E_EVENT_COUNT_64_ID,    0x14000001,     0x0000ff00,
    {0x1400FF01, 0x1400FF02, 0x1400FF03, 0x1400FF04, 0x1400FF05, 0x1400FF06, 0x1400FF07, 0x1400FF08, 0x1400FF09, 0x1400FF0A}},

    //�����������
    {RT_DATA,  65,  E_EVENT_COUNT_65_ID,    0x15000001,     0x0000ff00,
    {0x1500FF01, 0x1500FF02, 0x1500FF03, 0x1500FF04, 0x1500FF05, 0x1500FF06, 0x1500FF07, 0x1500FF08, 0x1500FF09, 0x1500FF0A}},

    //��ѹ��ƽ�����
    {RT_DATA,  66,  E_EVENT_COUNT_66_ID,    0x16000001,     0x0000ff00,
    {0x1600FF01, 0x1600FF02, 0x1600FF03, 0x1600FF04, 0x1600FF05, 0x1600FF06, 0x1600FF07, 0x1600FF08, 0x1600FF09, 0x1600FF0A}},

    //������ƽ�����
    {RT_DATA,  67,  E_EVENT_COUNT_67_ID,    0x17000001,     0x0000ff00,
    {0x1700FF01, 0x1700FF02, 0x1700FF03, 0x1700FF04, 0x1700FF05, 0x1700FF06, 0x1700FF07, 0x1700FF08, 0x1700FF09, 0x1700FF0A}},

    //������Դʧ��
    {RT_DATA,  68,  E_EVENT_COUNT_68_ID,    0x03060000,     0xffffffff,
    {0x03060001, 0x03060002, 0x03060003, 0x03060004, 0x03060005, 0x03060006, 0x03060007, 0x03060008, 0x03060009, 0x0306000A}},

    //�������
    {RT_DATA,  69,  E_EVENT_COUNT_69_ID,    0x03110000,     0xffffffff,
    {0x03110001, 0x03110002, 0x03110003, 0x03110004, 0x03110005, 0x03110006, 0x03110007, 0x03110008, 0x03110009, 0x0311000A}},

    //��������
    {RT_DATA,  70,  E_EVENT_COUNT_70_ID,    0x03120000,     0xffffffff,
    {0x03120101, 0x03120102, 0x03120103, 0x03120104, 0x03120105, 0x03120106, 0x03120107, 0x03120108, 0x03120109, 0x0312010A}},

    //�ܹ�������������
    {RT_DATA,  71,  E_EVENT_COUNT_71_ID,    0x1F000001,     0x0000ff00,
    {0x1F00FF01, 0x1F00FF02, 0x1F00FF03, 0x1F00FF04, 0x1F00FF05, 0x1F00FF06, 0x1F00FF07, 0x1F00FF08, 0x1F00FF09, 0x1F00FF0A}},

    //�������ز�ƽ��
    {RT_DATA,  72,  E_EVENT_COUNT_72_ID,    0x20000001,     0x0000ff00,
    {0x2000FF01, 0x2000FF02, 0x2000FF03, 0x2000FF04, 0x2000FF05, 0x2000FF06, 0x2000FF07, 0x2000FF08, 0x2000FF09, 0x2000FF0A}},

    //��������
    {RT_DATA,  73,  E_EVENT_COUNT_73_ID,    0x21000000,     0xffffffff,
    {0x21000001, 0x21000002, 0x21000003, 0x21000004, 0x21000005, 0x21000006, 0x21000007, 0x21000008, 0x21000009, 0x2100000A}},

    //ȫʧѹ
    {RT_DATA,  74,  E_EVENT_COUNT_74_ID,    0x03050000,     0xffffffff,
    {0x03050001, 0x03050002, 0x03050003, 0x03050004, 0x03050005, 0x03050006, 0x03050007, 0x03050008, 0x03050009, 0x0305000A}},

    //��̴���
    {RT_DATA,  80,  E_EVENT_COUNT_75_ID,    0x03300000,     0xffffffff,
    {0x03300001, 0x03300002, 0x03300003, 0x03300004, 0x03300005, 0x03300006, 0x03300007, 0x03300008, 0x03300009, 0x0330000A}},

    //�������
    {RT_DATA,  81,  E_EVENT_COUNT_81_ID,    0x03300100,     0xffffffff,
    {0x03300101, 0x03300102, 0x03300103, 0x03300104, 0x03300105, 0x03300106, 0x03300107, 0x03300108, 0x03300109, 0x0330010A}},

    //�������������������̫���ˣ��������¼���¼�ˣ�
    {RT_DATA,  82,  E_EVENT_COUNT_82_ID,    0x03300200,     0xffffffff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    //{0x03300201, 0x03300202, 0x03300203, 0x03300204, 0x03300205, 0x03300206, 0x03300207, 0x03300208, 0x03300209, 0x0330020A}},

    //�¼�����
    {RT_DATA,  83,  E_EVENT_COUNT_83_ID,    0x03300300,     0xffffffff,
    {0x03300301, 0x03300302, 0x03300303, 0x03300304, 0x03300305, 0x03300306, 0x03300307, 0x03300308, 0x03300309, 0x0330030A}},

    //Уʱ
    {RT_DATA,  84,  E_EVENT_COUNT_84_ID,    0x03300400,     0xffffffff,
    {0x03300401, 0x03300402, 0x03300403, 0x03300404, 0x03300405, 0x03300406, 0x03300407, 0x03300408, 0x03300409, 0x0330040A}},

    //ʱ�α���
    {RT_DATA,  85,  E_EVENT_COUNT_85_ID,    0x03300500,     0xffffffff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    //{0x03300501, 0x03300502, 0x03300503, 0x03300504, 0x03300505, 0x03300506, 0x03300507, 0x03300508, 0x03300509, 0x0330050A}},

    //ʱ������
    {RT_DATA,  86,  E_EVENT_COUNT_86_ID,    0x03300600,     0xffffffff,
    {0x03300601, 0x03300602, 0x03300603, 0x03300604, 0x03300605, 0x03300606, 0x03300607, 0x03300608, 0x03300609, 0x0330060A}},

    //�����ձ��
    {RT_DATA,  87,  E_EVENT_COUNT_87_ID,    0x03300700,     0xffffffff,
    {0x03300701, 0x03300702, 0x03300703, 0x03300704, 0x03300705, 0x03300706, 0x03300707, 0x03300708, 0x03300709, 0x0330070A}},

    //�ڼ��ձ��
    {RT_DATA,  88,  E_EVENT_COUNT_88_ID,    0x03300800,     0xffffffff,
    {0x03300801, 0x03300802, 0x03300803, 0x03300804, 0x03300805, 0x03300806, 0x03300807, 0x03300808, 0x03300809, 0x0330080A}},

    //�й���Ϸ�ʽ���
    {RT_DATA,  89,  E_EVENT_COUNT_89_ID,    0x03300900,     0xffffffff,
    {0x03300901, 0x03300902, 0x03300903, 0x03300904, 0x03300905, 0x03300906, 0x03300907, 0x03300908, 0x03300909, 0x0330090A}},

    //�޹���Ϸ�ʽ1���
    {RT_DATA,  90,  E_EVENT_COUNT_90_ID,    0x03300A00,     0xffffffff,
    {0x03300A01, 0x03300A02, 0x03300A03, 0x03300A04, 0x03300A05, 0x03300A06, 0x03300A07, 0x03300A08, 0x03300A09, 0x03300A0A}},

    //�޹���Ϸ�ʽ2���
    {RT_DATA,  91,  E_EVENT_COUNT_91_ID,    0x03300B00,     0xffffffff,
    {0x03300B01, 0x03300B02, 0x03300B03, 0x03300B04, 0x03300B05, 0x03300B06, 0x03300B07, 0x03300B08, 0x03300B09, 0x03300B0A}},

    //�����ձ��
    {RT_DATA,  92,  E_EVENT_COUNT_92_ID,    0x03300C00,     0xffffffff,
    {0x03300C01, 0x03300C02, 0x03300C03, 0x03300C04, 0x03300C05, 0x03300C06, 0x03300C07, 0x03300C08, 0x03300C09, 0x03300C0A}},

    //���ʲ�������
    {RT_DATA,  93,  E_EVENT_COUNT_93_ID,    0x03300F00,     0xffffffff,
    {0x03300F01, 0x03300F02, 0x03300F03, 0x03300F04, 0x03300F05, 0x03300F06, 0x03300F07, 0x03300F08, 0x03300F09, 0x03300F0A}},

    //���ݱ���
    {RT_DATA,  94,  E_EVENT_COUNT_94_ID,    0x03301000,     0xffffffff,
    {0x03301001, 0x03301002, 0x03301003, 0x03301004, 0x03301005, 0x03301006, 0x03301007, 0x03301008, 0x03301009, 0x0330100A}},

    //��Կ����
    {RT_DATA,  95,  E_EVENT_COUNT_95_ID,    0x03301200,     0xffffffff,
    {0x03301201, 0x03301202, 0x03301203, 0x03301204, 0x03301205, 0x03301206, 0x03301207, 0x03301208, 0x03301209, 0x0330120A}},

    {0xff,   0xff,  0xffff,                 0xffffffff,     0xffffffff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
};
#endif


//����645��ԼID����EventIDToDI�еĳ�Ա
const EventID_To_DI *GetEventIDToDI(uint32 MtrDI)
{
    int DICount = (sizeof(EventIDToDI) / sizeof(EventIDToDI[0])) - 1;       //EventIDToDI��Ա������
    int i = 0;

    for(i = 0; i < DICount; i++)
    {
        if(EventIDToDI[i].DI_EventCount == MtrDI)     //�����¼�������ص�������
            return &EventIDToDI[i];
    }
    return NULL;
}

const EventID_To_DI *GetEventIDToDIByIndex(uint8 IDIndex)
{
    int DICount = (sizeof(EventIDToDI) / sizeof(EventIDToDI[0])) - 1;       //EventIDToDI��Ա������
    int i = 0;

    for(i = 0; i < DICount; i++)
    {
        if(EventIDToDI[i].WorkStatBit == IDIndex)     //״̬�ֶ�Ӧ��λ��Ϣ
            return &EventIDToDI[i];
    }
    return NULL;
}
#endif

#if ACADDR_FROM_PRTL == 1
/*
  ��  ��: commfun_ReadACCommAddr
  ��  ��: ��ȡ����ͨѶ��ַ
  ��  ��: ��
  ��  ��: pAddr ����ͨѶ��ַ�����ֽ���ǰ��BCD��ʽ

  ����ֵ: > 0 �ɹ�
          < 0 ʧ��
 */
int commfun_ReadACCommAddr(char *const pAddr)
{
    static uint32 Offset = 0xFFFFFFFF;
    char TmpBuf[10] = {0};
    char   AddrDft[3][10] = {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}
        ,{0x00,0xFF,0x00,0xFF,0x00,0xFF}
        ,{0xFF,0x00,0xFF,0x00,0xFF,0x00}};
    int    i = 0;
    int  ret = 0;

    if (pAddr == NULL)
    {
        return -1;
    }

    // �״�ִ��ʱ���½��ɵ�ַƫ��
    if (Offset == 0xFFFFFFFF)
    {
        ret = ReadXPara(TMN_PARA_EXTEND, TMN_AC_ADDR_OFFSET, TmpBuf, 0x0);
        if (ret > 0)
        {
            memcpy((void*)&Offset, TmpBuf, 4);
        }
        else
        {
            return -1;
        }
    }

    ret = ReadPDataXram(AC_EEPROM_FILE, pAddr, Offset, 6);
    if (ret < 0)
    {
        return -1;
    }

    for (i = 0; i < 3; i++)
    {
        if (memcmp(AddrDft[i], pAddr, 6) == 0)
        {
            break;
        }
    }
    if (i < 3) // �Ƿ���ַ����Ϊ���ɵ�ַĬ��ֵ0
    {
        memset(pAddr, 0x00, 6);
    }

    return 6;
}

/*
  ��  ��: commfun_WriteACCommAddr
  ��  ��: ���潻��ͨѶ��ַ
  ��  ��: pAddr ����ͨѶ��ַ�����ֽ���ǰ��BCD��ʽ
  ��  ��: ��

  ����ֵ: > 0 �ɹ�
          < 0 ʧ��
 */
int commfun_WriteACCommAddr(const char *const pAddr)
{
    static uint32 Offset = 0xFFFFFFFF;
    char TmpBuf[10] = {0};
    int  ret = 0;

    if (pAddr == NULL)
    {
        return -1;
    }

    // �״�ִ��ʱ���½��ɵ�ַƫ��
    if (Offset == 0xFFFFFFFF)
    {
        ret = ReadXPara(TMN_PARA_EXTEND, TMN_AC_ADDR_OFFSET, TmpBuf, 0x0);
        if (ret > 0)
        {
            memcpy((void*)&Offset, TmpBuf, 4);
        }
        else
        {
            return -1;
        }
    }

    ret = WritePDataXram(AC_EEPROM_FILE, pAddr, Offset, 6);
    if (ret >= 0)
    {
        return 6;
    }
    else
    {
        return -1;
    }
}
#endif

/*********************************************************************
�������ܵ�Ԫ��
����������    ��ȡ�ն��豸���
���������    ��
���������    ��
��������ֵ��  ��ȡ���ݳ���
*********************************************************************/
int commfun_ReadTmnSerialNum(char *pTmnSerialNum)
{
    //�豸��ţ��ն˳���ǰ��������(SN),һ��Ϊ����
    if (ReadXPara(TMN_RUN_PARA, TMN_Serial_Num_ID, (char*)pTmnSerialNum, 0) < 0)
        memset(pTmnSerialNum, 0x0, 8);
    return 8;
}

/*********************************************************************
�������ܵ�Ԫ��
����������    ��ȡ�ն��豸���
���������    ��
���������    ��
��������ֵ��  ��ȡ���ݳ���
*********************************************************************/
int commfun_WriteTmnSerialNum(char *pTmnSerialNum)
{
    //�豸��ţ��ն˳���ǰ��������(SN),һ��Ϊ����
    WriteXPara(TMN_RUN_PARA, TMN_Serial_Num_ID, (char*)pTmnSerialNum, 0);
    return 8;
}

/*********************************************************************
�������ܵ�Ԫ��
����������    ��ȡAPN���û���������������Ϣ
���������    ��
���������    apn_user:�û���, apn_pwd:����, withPostfix:��׺��,
              netType: 1-GPRS 2-CDMA
��������ֵ��  ��ȡ���ݳ���
*********************************************************************/
int commfun_GetApnInfo(char *apn_user, char *apn_pwd, char netType, BOOL withPostfix)
{
    int     nRet;
    char    apnName[50];
    char    serialNum[25];  //�ն˱��
    char    tmpUser[32], tmpPwd[32], tmpBuf[100];
    int     i, tmpReturn = 0;       //��������ֵ������1��ʾȡĬ���û�����0��ʾ�û����õ��û���

    memset(apnName, 0x0, sizeof(apnName));
    memset(serialNum, 0x0, sizeof(serialNum));
    memset(tmpUser, 0x0, sizeof(tmpUser));
    memset(tmpPwd, 0x0, sizeof(tmpPwd));
    memset(tmpBuf, 0x0, sizeof(tmpBuf));
    /*
    VPN��APN�û���������Ӧ��ͨ�������ڽ��������úͲ�ѯ��
    APN�û�����ʽΪ���ƶ�GPRSΪA@fjep.fj������CDMAΪA@fjep.vpdn.fj��
    ��VPNΪfjep.fjʱ���豸Ӧ�ܹ�����ģ�����ͺ��豸�����Զ��޸��û�����APN�û���Ϊ�̶�����������������޸ģ�Ҳ��������������䶯��
    ��VPNΪ��������ʱ���ֶ������û��������롣
    */
    #if LONG_NEW_APN==1
    nRet = ReadXPara(TMN_RUN_PARA, APN_Addr_Len_ID, tmpBuf, 0);
    if(nRet>0 &&tmpBuf[0]>16&&tmpBuf[0]<=32)
    {
         memset(tmpBuf, 0x0, sizeof(tmpBuf));
         nRet = ReadXPara(TMN_PARA_EXTEND, NEW_APN_Addr_ID, tmpBuf, 0);
         if (nRet < 0 || ((unsigned char)tmpBuf[0] == NONECHAR && (unsigned char)tmpBuf[1] == NONECHAR))
        {
            memset(tmpBuf, 0, 32);
        }
    }
    else
    #endif
    {
        memset(tmpBuf, 0x0, sizeof(tmpBuf));
        nRet = ReadXPara(TMN_RUN_PARA, APN_Addr_ID, tmpBuf, TMN_ID);

        if (nRet < 0 || ((unsigned char)tmpBuf[0] == NONECHAR && (unsigned char)tmpBuf[1] == NONECHAR))
        {
            memset(tmpBuf, 0, 16);
        }
    }
    strcpy(apnName, tmpBuf);

    // ȡ�û���
    if (strcmp(apnName, "FJEP.FJ") == 0 || strcmp(apnName, "fjep.fj") == 0)         // ����Ǹ��������û���
    {
        nRet = commfun_ReadTmnSerialNum(serialNum);
        //printf(" nRet = %d serialNum[0] = %02x serialNum[1] = %02x\n", nRet, serialNum[0], serialNum[1]);
        if ((nRet < 0) || ((uint8)serialNum[0] == NONECHAR || (uint8)serialNum[1] == NONECHAR))
        {
            sprintf(serialNum, "%s", "000000000000");
        }
        serialNum[12] = '\0';

        if (withPostfix == TRUE)
        {
            // CDMA��׺����GPRS��һ��
            if (netType == 2)
            {
                sprintf(tmpUser, "%s@fjep.vpdn.fj", serialNum);
            }
            else
            {
                sprintf(tmpUser, "%s@fjep.fj", serialNum);
            }

        }
        else
        {
            sprintf(tmpUser, "%s", serialNum);
        }

        i = 0;  // ת��ΪСд
        while(tmpUser[i])
        {
            tmpUser[i] = tolower(tmpUser[i]);
            i++;
        }

        tmpReturn = 1;
    }
    else
    {
        memset(tmpBuf, 0, sizeof(tmpBuf));
        nRet = ReadXPara(TMN_RUN_PARA, SPECIALNET_NAME_ID, tmpBuf, TMN_ID);
        memcpy(tmpUser, tmpBuf, 32);
    }

    if (strlen(tmpUser) > 32)
        memcpy(apn_user, tmpUser, 32);
    else
        strcpy(apn_user, tmpUser);

    // ��ȡ����
    memset(tmpBuf, 0x00, sizeof(tmpBuf));
    if (strcmp(apnName, "FJEP.FJ") == 0 || strcmp(apnName, "fjep.fj") == 0)         // ����Ǹ��������û���
    {
        // APN��ʼ�����ʽΪ���ն˱�ŵ�����
        memset(tmpBuf, 0x0, sizeof(tmpBuf));
        for(i = 0; i < strlen(serialNum); i++)
            tmpBuf[i] = serialNum[strlen(serialNum)-1-i];
        memset(tmpPwd, 0x0, sizeof(tmpPwd));
        memcpy(tmpPwd, tmpBuf, strlen(tmpBuf));
    }
    else
    {
        memset(tmpBuf, 0, sizeof(tmpBuf));
        nRet = ReadXPara(TMN_RUN_PARA, SPECIALNET_PWD_ID, (char *)tmpBuf, TMN_ID);
        memcpy(tmpPwd, tmpBuf, strlen(tmpBuf));
    }

    if(strlen(tmpPwd) > 32)
        memcpy(apn_pwd, tmpPwd, 32);
    else
        strcpy(apn_pwd, tmpPwd);

    return tmpReturn;
}

#if DOUBLE_CHANNEL_EN == 1      // ˫ͨ��
//��ȡ˫ͨ���ŵ�ʹ�õĵ�̨����
//pRadioType        ��̨����
//pFasterType       ���ٵ�̨��ǰ�ĵ�������
BOOL commfun_DBChRadioType(uint8 *pRadioType, uint8 *pFasterType)
{
    uint8 tmpTpye = 0;          //��̨����
    uint8 tmpFasterType = 0;    //����Ǹ��ٵ�̨�����ٵ�̨��ǰ�ĵ�������

    uint8 dbchlRadioType = 0;       // ��ͨ��̨���Ǹ��ٵ�̨�ı�ʶ
    uint8 radio_code_id = 0;

    ReadXPara(TMN_PARA_EXTEND, DOUBLE_CHANNEL_RADIO_TYPE_ID, (char *)&dbchlRadioType, 0);
    if(dbchlRadioType == 1)         //Radio
        tmpTpye = DBCH_TYPE_RADIO;
    else if(dbchlRadioType == 2)    //WDASN
        tmpTpye = DBCH_TYPE_WDASN;
    else if (dbchlRadioType == DBCH_TYPE_FASTER)                           //FASTER_RADIO
    {
        tmpTpye = DBCH_TYPE_FASTER;
        //���ٵ�̨�Ļ�����Ҫ��ѯ��ǰ��������
        if(ReadRamXData(MODEM_RSCODE_ID, (char *)&radio_code_id) > 0)
        {
            switch(radio_code_id)
            {
            case 0x30:
                tmpFasterType = DBCH_TYPE_FASTER_30;
                break;

            case 0x31:
                tmpFasterType = DBCH_TYPE_FASTER_31;
                break;

            case 0x33:
                tmpFasterType = DBCH_TYPE_FASTER_33;
                break;

            case 0x34:
                tmpFasterType = DBCH_TYPE_FASTER_34;
                break;

            default:
                tmpFasterType = DBCH_TYPE_FASTER;
                break;
            }
        }
        else
        {
            tmpFasterType = DBCH_TYPE_FASTER;
        }
    }
    else
    {
        return FALSE;
    }
    *pRadioType = tmpTpye;
    *pFasterType = tmpFasterType;
    return TRUE;
}

/*********************************************************************
�����������ж��ն��Ƿ�������˫ͨ��ģʽ��
���������
-           ��
���������
-           ��
��������ֵ��
-           TRUE            ˫ͨ��ģʽ
-           FALSE           ��ͨģʽ
�ڲ��������̣�
-           1����ȡ��ǰͨ�����ͣ������ǡ���ͨ��̨��WDASN�����ٵ�̨���е�һ�֣��ſ�����˫ͨ��ģʽ
-           2����ȡ˫ͨ��ʹ�ܱ�ʶ��ʹ�ܱ�ʶ��������Ч������˫ͨ��ģʽ
********************************************************************/
BOOL CheckIsDoubleChannel(void)
{
    uint8 cchType[2] = {0x0, 0x0};          // ��ǰ�ŵ�����
    uint8 blnExtChnlEn = 0;

    // ��ǰͨ������
    if (ReadXPara(TMN_RUN_PARA, Cch_Type_ID, (char *)cchType, TMN_ID) < 0)
        return FALSE;

    //if(cchType[0] != Radio_ID && cchType[0] != WDASN_Radio_ID && cchType[0] != FASTER_RADIO_ID)         //��ͨ��̨��WDASN�����ٵ�̨
    if(cchType[0] != 8 && cchType[0] != 11 && cchType[0] != FASTER_RADIO_ID)         //��ͨ��̨��WDASN�����ٵ�̨
        return FALSE;

    if(ReadXPara(TMN_PARA_EXTEND, DOUBLE_CHANNEL_EXT_EN, (char *)&blnExtChnlEn, 0) > 0 && blnExtChnlEn == 0x55)
        return TRUE;

    return FALSE;
}
#endif

//485�ڹ�������Ӧ
void commfun_SelfAdaptRs485Fun()
{
    uint16      i = 0;
    char        btemp = 0;
    char        type = 0;
    int8       ComId = 0 ;
    uint16      gMtrNum_485[CB_PIPE_NUM] = {0};
    int8       stat = 0 ;
    uint16      tmpnum = 0;
    for (i = 1; i <= MTR485_MAX_NUM; i++)
    {

        if( ReadXPara(MTR_PARA_TYPE, Mt_Para_State_ID, (char *)&stat, i) < 0 || (stat != 1))
            continue;

        if (ReadXPara(MTR_PARA_TYPE, Mt_Para_Com_ID, (char *)&ComId, i) < 0)            //�˿ں�
            continue;

        if(ComId == PORT_PRTL_485_1)
            gMtrNum_485[PIPE_METER_485_1 - 1] ++;
        else if(ComId == PORT_PRTL_485_2)
            gMtrNum_485[PIPE_METER_485_2 - 1] ++;
        else
            continue;
    }

    // ���ݶ˿�����û������485������˿ڹ���,
    // �����˾��������,δ������Ϊά����,����Ҫ�ֶ�����
    if (gMtrNum_485[PIPE_METER_485_1 - 1] > 0)
    {
        btemp = 1;
    }
    else
    {
        btemp = 0;
    }
    if(ReadXPara(TMN_PARA_EXTEND, MTR_NUM_OF_485_I_ID, (char *)&tmpnum, 0) < 0 || (gMtrNum_485[PIPE_METER_485_1 - 1] != tmpnum))
        WriteXPara(TMN_PARA_EXTEND, MTR_NUM_OF_485_I_ID, (char *)&gMtrNum_485[PIPE_METER_485_1 - 1], 0x0);

    if(ReadXPara(TMN_PARA_EXTEND, RS485_1_PORT_FUNC, (char *)&type, 0) < 0 || (btemp != type))
        WriteXPara(TMN_PARA_EXTEND, RS485_1_PORT_FUNC, (char *)&btemp, 0);

    // �����˾��������,δ������Ϊά����,����Ҫ�ֶ�����
    if (gMtrNum_485[PIPE_METER_485_2 - 1] > 0)
    {
        btemp = 1;
    }
    else
    {
        btemp = 0;
    }
    if(ReadXPara(TMN_PARA_EXTEND, MTR_NUM_OF_485_II_ID, (char *)&tmpnum, 0) < 0 || (gMtrNum_485[PIPE_METER_485_2 - 1] != tmpnum))
        WriteXPara(TMN_PARA_EXTEND, MTR_NUM_OF_485_II_ID, (char *)&gMtrNum_485[PIPE_METER_485_2 - 1], 0x0);

    if(ReadXPara(TMN_PARA_EXTEND, RS485_2_PORT_FUNC, (char *)&type, 0) < 0 || (btemp != type))
        WriteXPara(TMN_PARA_EXTEND, RS485_2_PORT_FUNC, (char *)&btemp, 0);
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

/****************************************************************************
*ģ���ţ�
*���ƣ�
*���ܣ�
*���������
*���������
*����
*���أ���
****************************************************************************/
uint8 getDataTypeLength1(enum eDatatype dataType)
{
  switch(dataType)
  {
        case NULL_DATA:
            return 0;
        case TBOOLEAN:
        case UNSIGNED:
        case INTEGER:
        case INTEGER_CUSTOM:
        case ENUM:
            return 1;

        case LONG_SIGNED:
        case LONG_SIGNED_CUSTOM:
        case LONG_UNSIGNED:
        case eOI:
        case eSCALER_UNIT:
            return 2;

        case TIME:
        case TIMEBCD:
            return 3;
        //4
        case DOUBLE_LONG:
        case DOUBLE_LONG_CUSTOM:
        case DOUBLE_LONG_UNSIGNED:
        case FLOAT32:
        case eOAD:
        case eOMD:
        case DATEBCD:
        case e645DATETIME_M:
        case e645DATETIME_W:
            return 4;
        //5
        case TDATE:
        case eCOMDCB:
            return 5;
        case DATETIMEBCD_S:
        case e645DATETIME:
            return 6;
        //7
        case DATE_TIME_S:
            return 7;
        //8
        case LONG64_SIGNED:
        case LONG64_SIGNED_CUSTOM:
        case LONG64_UNSIGNED:
        case FLOAT64:
            return 8;
        //9
        case DATETIMEBCD_H:
            return 9;
        //10
        case DATE_TIME:
            return 10;
        default:
            return 0xFF;
  }
}
/****************************************************************************
*ģ���ţ�
*���ƣ�
*���ܣ�
*���������
*���������
*����
*���أ���
****************************************************************************/
BOOL getNextReadData1(const uint8 *inData,uint16 *offset,uint16 inDataLen,enum eDatatype DataType,BOOL bWithType)
{
    uint16 dataLen;
    uint8 nchioce;
    uint8 dataNum;
    uint8 i;
    uint16 offSet=*offset;
    enum eDatatype dataType;

    if(bWithType)
    {
        dataType = DataType;
    }
    else
    {
        dataType = (enum eDatatype)inData[offSet++];
    }

    if(dataType == NULL_DATA)
    {
        *offset = offSet;
        return TRUE;
    }
    if((dataType == STRUCTURE)
        ||(dataType == ARRAY)
        ||(dataType == OCTET_STRING)
        ||(dataType == VISIBLE_STRING)
        ||(dataType == UTF8_STRIGN)
        ||(dataType == BIT_STRING)
        ||(dataType == eMAC)
        ||(dataType == eRAND)
        ||(dataType == eTSA))
    {
        dataLen = inData[offSet++];
        if(dataLen == 0x81)
            dataLen = inData[offSet++];
        else if(dataLen == 0x82)
        {
            dataLen = inData[offSet++];
            dataLen <<= 8;
            dataLen += inData[offSet++];
        }
        else if(dataLen>0x82||dataLen==0x80)
        {
            return FALSE;
        }
        if((dataType != ARRAY) && (dataType != OCTET_STRING) && (dataType != VISIBLE_STRING) \
            && (dataType != eMAC) &&(dataLen == 0))
        {
            return FALSE;
        }

        if(dataType == BIT_STRING)
        {
            dataLen /= 8;
        }

        if((dataType == STRUCTURE)
            ||(dataType == ARRAY))
        {
            uint16 eleNum = dataLen;
            while(eleNum--)
            {
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,NULL_DATA,FALSE))
                {
                    return FALSE;
                }
            }
        }
        else
        {
            offSet+=dataLen;
        }
    }
    else if(dataType == eRSD)
    {
        uint8 eleNum=0;
        nchioce = inData[offSet++];
        switch(nchioce)
        {
            case 0://
                break;
            case 1:
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,eOAD,TRUE))
                    return FALSE;
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,NULL_DATA,FALSE))
                    return FALSE;
                break;
            case 2:
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,eOAD,TRUE))
                    return FALSE;
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,NULL_DATA,FALSE))
                    return FALSE;
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,NULL_DATA,FALSE))
                    return FALSE;
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,NULL_DATA,FALSE))
                    return FALSE;
                break;
            case 3:
                eleNum = inData[offSet++];
                for(i=0;i<eleNum;i++)
                {
                    if(FALSE == getNextReadData1(inData,&offSet,inDataLen,eOAD,TRUE))
                        return FALSE;
                    if(FALSE == getNextReadData1(inData,&offSet,inDataLen,NULL_DATA,FALSE))
                        return FALSE;
                    if(FALSE == getNextReadData1(inData,&offSet,inDataLen,NULL_DATA,FALSE))
                        return FALSE;
                    if(FALSE == getNextReadData1(inData,&offSet,inDataLen,NULL_DATA,FALSE))
                        return FALSE;
                }
                break;
            case 4:
            case 5:
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,DATE_TIME_S,TRUE))
                    return FALSE;
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,eMS,TRUE))
                    return FALSE;
                break;
            case 6:
            case 7:
            case 8:
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,DATE_TIME_S,TRUE))
                    return FALSE;
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,DATE_TIME_S,TRUE))
                    return FALSE;
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,eTI,TRUE))
                    return FALSE;
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,eMS,TRUE))
                    return FALSE;
                break;
            case 9:
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,UNSIGNED,TRUE))
                    return FALSE;
                break;
            case 10:
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,UNSIGNED,TRUE))
                    return FALSE;
                if(FALSE == getNextReadData1(inData,&offSet,inDataLen,eMS,TRUE))
                    return FALSE;
                break;

        }
    }
    else if(dataType == eCSD)
    {
        nchioce = inData[offSet++];
        if(nchioce == 0)
        {
            dataLen = getDataTypeLength1(eOAD);
            offSet +=dataLen;
        }
        else if(nchioce == 1)
        {
            dataLen = getDataTypeLength1(eOAD);
            offSet +=dataLen;
            dataNum = inData[offSet++];
            offSet +=dataLen*dataNum;
        }
        else
            return FALSE;

    }
    else if(dataType == eROAD)
    {
        dataLen = getDataTypeLength1(eOAD);
        offSet +=dataLen;
        dataNum = inData[offSet++];
        offSet +=dataLen*dataNum;
        printf("ROAD LEN = %d\n",dataLen+1+dataLen*dataNum);
    }
    else if(dataType == eRCSD)
    {
        dataNum = inData[offSet++];
        for(i=0;i<dataNum;i++)
        {
            if(FALSE == getNextReadData1(inData,&offSet,inDataLen,eCSD,TRUE))
                return FALSE;
        }
    }
    else if(dataType == eMS)
    {
        nchioce = inData[offSet++];
        if((nchioce == 0)||(nchioce == 1))
        {

        }
        else if(nchioce == 2)//һ���û�����
        {
            dataNum = inData[offSet++];
            dataLen = getDataTypeLength1(UNSIGNED);
            offSet +=dataLen*dataNum;
        }
        else if(nchioce == 3)
        {
            dataNum = inData[offSet++];
            while(dataNum--)
            {
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet += dataLen;
            }
        }
        else if(nchioce == 4)
        {
            dataNum = inData[offSet++];
            dataLen = getDataTypeLength1(LONG_UNSIGNED);
            offSet +=dataLen*dataNum;
        }
        else if(nchioce == 5)//һ���û���������
        {
            dataNum = inData[offSet++];

            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength1(UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength1(UNSIGNED);
                offSet +=dataLen;
            }
        }
        else if(nchioce == 6)//һ���û���ַ����
        {
            dataNum = inData[offSet++];
            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=eTSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet +=dataLen;
                if(inData[offSet++]!=eTSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet +=dataLen;
            }
        }
        else if(nchioce == 7)//һ�������������
        {
            dataNum = inData[offSet++];

            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=LONG_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength1(LONG_UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=LONG_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength1(LONG_UNSIGNED);
                offSet +=dataLen;
            }
        }
        else
            return FALSE;

    }
    else if(dataType == eREGION)
    {
        if(inData[offSet++]>3)
            return FALSE;
        if(FALSE == getNextReadData1(inData,&offSet,inDataLen,NULL_DATA,FALSE))
            return FALSE;
        if(FALSE == getNextReadData1(inData,&offSet,inDataLen,NULL_DATA,FALSE))
            return FALSE;
    }
    else if(dataType == eSID)
    {
        if(FALSE == getNextReadData1(inData,&offSet,inDataLen,DOUBLE_LONG_UNSIGNED,TRUE))
            return FALSE;
        if(FALSE == getNextReadData1(inData,&offSet,inDataLen,OCTET_STRING,TRUE))
            return FALSE;
    }
    else if(dataType == eSID_MAC)
    {
        if(FALSE == getNextReadData1(inData,&offSet,inDataLen,eSID,TRUE))
            return FALSE;
        if(FALSE == getNextReadData1(inData,&offSet,inDataLen,eMAC,TRUE))
            return FALSE;
    }
    else if(dataType == eTI)
    {
        if(inData[offSet++]>5)
            return FALSE;
        offSet += 2;
    }
    else
    {

        dataLen = getDataTypeLength1(dataType);
        if(0xFF != dataLen)
            offSet +=dataLen;
        else
            return FALSE;
    }
    if(offSet>inDataLen)
        return FALSE;
    *offset = offSet;
    return TRUE;
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

