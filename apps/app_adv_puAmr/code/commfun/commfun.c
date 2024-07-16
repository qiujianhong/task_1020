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

#include "commfun.h"
#include "mem.h"     //TODO 这个C中名字换掉则不用了
#include "data_id.h"
#include "para_id.h"

//默认所有debug都打印
uint32 g_debug_enable = 0xffffffff;
//默认所有trace都打印
uint32 g_trace_enable = 0xffffffff;
//默认所有log都记录
uint32 g_log_enable = 0xffffffff;

#if DESC("日期时间转换函数",1)

/**
*********************************************************************
* @brief：      闰年判断
* @param[in]：  year    年份数据
* @param[out]：NA   
* @return：     转换成功
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
* @brief：      判断短格式时间是否有效
* @param[in]： dateTime    OOP短时间格式
* @param[out]：NA      
* @return：    1-有效 0-无效
*********************************************************************
*/
BOOL DT_DateTimeShortIsValid(OOP_DATETIME_S_T *DateTime)
{
    const uint8 MonTab[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    //判断时间是否合法
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
* @brief：       OOP短时间格式转换为系统时间格式
* @param[in]：   dateTime     短时间格式
* @param[out]： linuxTime     Linux系统时间，1970年以来的秒数
* @return：      转换成功
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
* @brief：      系统时间格式转换为16进制时间格式
* @param[in]： linuxTime       Linux系统时间，1970年以来的秒数
* @param[out]：dateTime        OOP短时间格式
* @return：     转换成功
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
    dateTime->week = systime.tm_wday;  //0-表示周日

    return TRUE;
}

/**
*********************************************************************
* @brief：      判断16进制时间是否有效
* @param[in]： dateTimeHex     终端16进制表示时间
* @param[out]：NA      
* @return：    1-有效 0-无效
*********************************************************************
*/
BOOL DT_DateTimeHexIsValid(DateTimeHex_t *DateTime)
{
    const uint8 MonTab[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    uint16 tmpyear;
    //判断时间是否合法
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
* @brief：      16进制时间格式转换为系统时间格式
* @param[in]：dateTimeHex     终端16进制表示时间
* @param[out]： linuxTime       Linux系统时间格式
* @return：     转换成功
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
* @brief：      系统时间格式转换为16进制时间格式
* @param[in]： linuxTime       Linux系统时间格式
* @param[out]：dateTimeHex     终端16进制表示时间
* @return：     转换成功
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
* @brief：      判断BCD编码时间是否有效
* @param[in]： dateTimeHex     终端16进制表示时间
* @param[out]：NA      
* @return：    1-有效 0-无效
*********************************************************************
*/
BOOL DT_DateTimeBCDIsValid(DateTimeBcd_t *DateTime)
{
    uint8 i,*pdateTime;
    DateTimeHex_t hexDateTime;
    pdateTime = (uint8*)DateTime;
    for(i=0; i<7; i++)
    {
        //判断是否是BCD时间，如果不是BCD时间则认为是HEX时间
        if((pdateTime[i]&0x0F)>0x09||(pdateTime[i]&0xF0)>0x90)
        {
            break;
        }
    }
    if(i==7)//BCD时间
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
* @brief：      BCD编码时间格式转换为系统时间格式
* @param[in]：dateTimeHex     终端16进制表示时间
* @param[out]： linuxTime       Linux系统时间格式
* @return：     转换成功
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
        printf("时间非法\n");
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
* @brief：      系统时间格式转换为BCD编码时间格式
* @param[in]： linuxTime       Linux系统时间格式
* @param[out]：dateTimeHex     终端16进制表示时间
* @return：     转换成功
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
* @brief：     HEX编码时间格式转换为BCD编码时间格式
* @param[in]： dateTimeHex     HEX编码时间格式
* @param[out]：dateTimeBCD     BCD编码时间格式
* @return：    转换成功
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
* @brief：     BCD编码时间格式转换为BCD编码时间格式
* @param[in]： dateTimeBCD     BCD编码时间格式
* @param[out]：dateTimeHex     HEX编码时间格式
* @return：    转换成功
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
* @brief：      将时间间隔转换为秒
* @param[in]：  ti    时间间隔
* @param[out]：NA   
* @return：     转换成功
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
* @brief：      加一个间隔秒数
* @param[in]：  time  被加的时间
* @param[in]：  ti    加上时间间隔
* @param[out]：NA   
* @return：     void
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
        if(ti.unit == 4)//月
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
        else if(ti.unit == 5)//年
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
        if(hexDateTime.day>maxday)//使时间调整为合法的时间
            hexDateTime.day = maxday;
        DT_DateTimeHex2Time(&hexDateTime,&longtime);
    }
    *time = longtime;
}

/**
*********************************************************************
* @brief：      减一个间隔秒数
* @param[in]：  time  被减的时间
* @param[in]：  ti    减掉的时间间隔
* @param[out]：NA   
* @return：     void
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
        if(ti.unit == 4)//月
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
        else if(ti.unit == 5)//年
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
        if(hexDateTime.day>maxday)//使时间调整为合法的时间
            hexDateTime.day = maxday;
        DT_DateTimeHex2Time(&hexDateTime,&longtime);
    }
    *time = longtime;
}

#endif

#if DESC("各类打印函数",1)
void commfun_printstring(uint8 *buf, int16 len)
{
    uint16 i = 0;
    if(len <= 0)
        return;
    for(i = 0; i < len; i++)
        printf("%02x ", buf[i]);
    putchar('\n');
}

//打印当前时间
//blnNewline        是否换行
//blnEnv            可以根据环境变量决定是否打印
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

//将报文写入日志中
//blnPrint      可以根据环境变量决定是否打印
//filepath      日志路径
//buf           报文信息
//len           报文长度
//fmt           调试信息，可以记录文字
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

    //打印调试信息
    if(blnPrint == TRUE)
    {
        commfun_printcurtime(FALSE, blnPrint);      //打印当前时间
        printf("%s\n", logbuf);
    }
}

#endif

#if DESC("转码函数",1)
/**
*********************************************************************
* @brief：      十六进制数转压缩BCD码
* @param[in]： hex 十六进制数
* @param[out]：NA
* @return：     压缩BCD码
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
* @brief：      压缩BCD码转十六进制数
* @param[in]： compress_bcd 压缩BCD码
* @param[out]：NA
* @return：     十六进制数
*********************************************************************
*/
int commfun_BcdToHex(char compress_bcd)
{
    int rv;

    rv = ((compress_bcd)>>4)*10+(compress_bcd&0x0f);
    return rv;
}

/*********************************************************************
所属功能单元：GSM动态库
功能描述：将16进制数转换成两个Ascii码
输入参数：
bin     16进制数
high    Ascii码高位
low     Ascii码地位
输出参数:
无
函数返回值:
无
内部处理流程:将16进制数拆成两个Ascii码
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
所属功能单元：GSM动态库
功能描述：将两个Ascii码转换成一个16进制数
输入参数：
high    转换为16进制高位
low     转换为16进制地位
输出参数：
无
函数返回值：
转换后的16进制数
内部处理流程: 将high转换成进制高位, low转换成16进制低位
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

// bcd码转换为ascii码
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

// 字符串、BCD转换函数
// ascii码转换为bcd码
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
功能描述：    IP4数据转换为Hex字符串
输入参数：    int nIp：IP地址
char* pVal：HexIP字符串
输出参数：
函数返回值：
内部处理流程：
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
功能描述：    IP4数据转换为字符串
输入参数：    int nIp：IP地址
char* pVal：IP字符串
输出参数：
函数返回值：
内部处理流程：
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
功能描述：    IP4数据转换为字符串
输入参数：    int nIp：IP地址
char* pVal：IP字符串
输出参数：
函数返回值：
内部处理流程：
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
功能描述：    IP字符串转换为4字节数据
输入参数：    char* pczIp：IP字符串
iint* pnIp：返回4字节IP
输出参数：   TRUE：转换成功
函数返回值：
内部处理流程：
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

//将字符串转化为大写字符串
void commfun_toupper(char* pStr)
{
    int i = 0;
    while(pStr[i])  //将内部版本信息转换为大写字母
    {
        pStr[i] = toupper(pStr[i]);
        i++;
    }
}

//将字符串转化为小写字符串
void commfun_tolower(char* pStr)
{
    int i = 0;
    while(pStr[i])  //将内部版本信息转换为大写字母
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
    //设置终端网口IP地址
    ETH_INFO eth_info;
    char czIp[20];
    char czCmdBuf[100];

    memset((void *)&eth_info, 0x0, sizeof(ETH_INFO));
    memset(czIp, 0x0, sizeof(czIp));
    memset(czCmdBuf, 0x0, sizeof(czCmdBuf));

    //写入数据字典
    eth_info.eth_en = bEnable;  //有效性
    eth_info.eth_ip = nIp;      //ip地址
    WriteXPara(TMN_RUN_PARA, eth_id, (char *)&eth_info, 0);

    //设置网卡IP
    commfun_IPToStr(eth_info.eth_ip, czIp);
    memset(czCmdBuf, 0x00, 100);
    if(bEnable == 1)    //有效
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

//从数据字典中读取终端网关（用户设置的值）
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

//从数据字典中读取终端的子网掩码（用户设置的值）
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
uint8 commfun_GetMtrMode(mtrnum_t mtr_id)
{
    uint8 bSpeed = 0;
    if(ReadXPara(MTR_PARA_TYPE, Mt_Para_Speed_ID, (char *)&bSpeed, mtr_id) < 0 || bSpeed == 0xff)       //通讯速率
        bSpeed = 2400 / 300;        //默认为2400波特率
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


//将rtc时钟的格式转换为距离1970年1月1日0时0分0秒的秒数
//sec_cancle 是用来决定是否忽略秒的，如果是TRUE的话，则在转换的时候忽略秒
//原因是任务执行的时候，最小的上报单位是分，那么可以在检查任务参数的时候就确认任务是否已经到了需要执行的秒数，在真正上报的时候直接都按0秒开始上报就可以了
time_t commfun_rtc2time(rtc_t src_rtc)
{
    /*

    关于 time_t
    而日历时间（Calendar Time）是通过time_t数据类型来表示的
    用time_t表示的时间（日历时间）是从一个时间点（例如：1970年1月1日0时0分0秒）到此时的秒数。
    在time.h中，我们也可以看到time_t是一个长整型数：

    关于 tm
    struct tm {
    int tm_sec; // 秒 – 取值区间为[0,59]
    int tm_min; // 分 - 取值区间为[0,59]
    int tm_hour; // 时 - 取值区间为[0,23]
    int tm_mday; // 一个月中的日期 - 取值区间为[1,31]
    int tm_mon; // 月份（从一月开始，0代表一月） - 取值区间为[0,11]
    int tm_year; // 年份，其值等于实际年份减去1900
    int tm_wday; // 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推
    int tm_yday; // 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推
    int tm_isdst; // 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。
    };
    */
    struct tm des_tm;       //标准的tm时钟结构
    des_tm.tm_year = (commfun_BcdToHex(src_rtc.yy) > 70 ? commfun_BcdToHex(src_rtc.yy) : (commfun_BcdToHex(src_rtc.yy) + 100));
    des_tm.tm_mon = commfun_BcdToHex(src_rtc.mm) - 1;
    des_tm.tm_mday = commfun_BcdToHex(src_rtc.dd);
    des_tm.tm_hour = commfun_BcdToHex(src_rtc.hh);
    des_tm.tm_min = commfun_BcdToHex(src_rtc.nn);
    des_tm.tm_sec = 0;  //将秒数置为0
    des_tm.tm_isdst = -1;           //忽略闰秒
    //return mktime(&des_tm);     //从1970年1月1日0时0分0秒到此时的秒数
    return FastDateToSecond(&des_tm);

}


//将rtc时钟的格式转换为距离1970年1月1日0时0分0秒的秒数
//sec_cancle 是用来决定是否忽略秒的，如果是TRUE的话，则在转换的时候忽略秒
//原因是任务执行的时候，最小的上报单位是分，那么可以在检查任务参数的时候就确认任务是否已经到了需要执行的秒数，在真正上报的时候直接都按0秒开始上报就可以了
/**********在commfun_rtc2time的基础上把秒也计算进去*******/
time_t commfun_rtc2time2(rtc_t src_rtc)
{
    /*

    关于 time_t
    而日历时间（Calendar Time）是通过time_t数据类型来表示的
    用time_t表示的时间（日历时间）是从一个时间点（例如：1970年1月1日0时0分0秒）到此时的秒数。
    在time.h中，我们也可以看到time_t是一个长整型数：

    关于 tm
    struct tm {
    int tm_sec; // 秒 – 取值区间为[0,59]
    int tm_min; // 分 - 取值区间为[0,59]
    int tm_hour; // 时 - 取值区间为[0,23]
    int tm_mday; // 一个月中的日期 - 取值区间为[1,31]
    int tm_mon; // 月份（从一月开始，0代表一月） - 取值区间为[0,11]
    int tm_year; // 年份，其值等于实际年份减去1900
    int tm_wday; // 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推
    int tm_yday; // 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推
    int tm_isdst; // 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。
    };
    */
    struct tm des_tm;       //标准的tm时钟结构
    des_tm.tm_year = (commfun_BcdToHex(src_rtc.yy) > 70 ? commfun_BcdToHex(src_rtc.yy) : (commfun_BcdToHex(src_rtc.yy) + 100));
    des_tm.tm_mon = commfun_BcdToHex(src_rtc.mm) - 1;
    des_tm.tm_mday = commfun_BcdToHex(src_rtc.dd);
    des_tm.tm_hour = commfun_BcdToHex(src_rtc.hh);
    des_tm.tm_min = commfun_BcdToHex(src_rtc.nn);
    des_tm.tm_sec = commfun_BcdToHex(src_rtc.ss);   //将秒数置为0
    des_tm.tm_isdst = -1;           //忽略闰秒
    
    return FastDateToSecond(&des_tm);

}


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

    return (time(NULL) - FastDateToSecond(&tMetertime));
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

/****************************************************************************
* 函数名称: commfun_msqrt
* 函数功能: 开平方根
* 输入参数: x  被开方数
* 输出参数: 无
* 返 回 值: 平方根
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

// 获取交采脉冲常数地址与默认脉冲常数
static int GetACPulseConstInfo(uint16 *pConstAddr, uint32 *pDefaultConst)
{
    int    nret = 0;
    uint8  LineMode = 0; // 接线方式 三相
    uint8  VoltType = 0; // 三相四线额定电压类型 57.7V/220V
    uint16 PulseConstAddr = 0;  // 脉冲常数地址
    uint32 DefaultConst = 0;    // 默认脉冲常数

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

// 从铁电读取交采脉冲常数
uint32 commfun_ReadACPulseConst(void)
{
    uint8 PhaseFlag= 0;
    uint32 mACPulseConst = 0;
    int    nret = 0;
    uint16 PulseConstAddr = 0;  // 脉冲常数地址
    uint32 DefaultConst = 0;    // 默认脉冲常数

    // 获取交采脉冲常数地址与默认脉冲常数
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

// 写交采脉冲常数至铁电
int commfun_WriteACPulseConst(uint32 PulseConst)
{
    int    nret = 0;
    uint16 PulseConstAddr = 0;  // 脉冲常数地址
    uint32 DefaultConst = 0;    // 默认脉冲常数
    uint32 SavePulseConst = 0;  // 用于存储的脉冲常数

    // 获取交采脉冲常数地址与默认脉冲常数
    if (GetACPulseConstInfo(&PulseConstAddr, &DefaultConst) < 0)
    {
        return -1;
    }

    // 若脉冲常数不合法，不是100的整数倍，则赋值为默认脉冲常数并存储
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

//从私有外存中读取的已经下发的数据项标识数量
static uint16 GetGradeParaCount(uint16 GradeNum)
{
    char GradeFile[255];                //私有文件名称
    int bRet = 0;
    uint16 DI0DI1_Num_File = 0;         //私有外存中存放的数据项标识数量
    uint16 GradeCS = 0;                 //校验和

    memset(GradeFile, 0x0, sizeof(GradeFile));
    GetGradeFileName(GradeNum, GradeFile);

    bRet = ReadPDataXram(GradeFile, (char *)&DI0DI1_Num_File, 0, sizeof(uint16));      //读取私有外存中存放的数据项标识数量
    if(bRet < 0 || DI0DI1_Num_File == 0 || DI0DI1_Num_File == 0xffff)
        DI0DI1_Num_File = 0;

    if(DI0DI1_Num_File > 0)
    {
        MET_GRADE_INFO fileGradeInfo[DI0DI1_Num_File];
        //读取私有外存中存储的“电能表数据分级参数”
        bRet = ReadPDataXram(GradeFile, (char *)fileGradeInfo, 2, sizeof(MET_GRADE_INFO) * DI0DI1_Num_File);
        if(bRet < 0)
            DI0DI1_Num_File = 0;
        if(DI0DI1_Num_File > 0)
        {
            //读取校验和
            bRet = ReadPDataXram(GradeFile, (char *)&GradeCS, 2 + sizeof(MET_GRADE_INFO) * DI0DI1_Num_File, 2);
            if(bRet < 0 || GradeCS == 0xffff)
                DI0DI1_Num_File = 0;
            else
            {
                //验证校验和
                if(GetGradeCS((char *)fileGradeInfo, sizeof(MET_GRADE_INFO) * DI0DI1_Num_File) != GradeCS)
                    DI0DI1_Num_File = 0;
            }
        }
    }

    return DI0DI1_Num_File;
}

/****************************************************************************
名称： commfun_WriteGradeInfo
功能：
-           将主站下发的“F106：电能表数据分级参数”写入到私有外存中
处理流程：
-           1、从私有外存中读取读取主站已经下发的参数数量及信息
-           2、将本次下发的分级参数写入私有外存（数据等级为0的不存）
-           3、将步骤1读取的参数信息与本次下发的进行比对，去除重复的参数，没有重复的写入到私有外存内
-           4、计算校验和
输入参数：
-           GradeNum        数据分级类号
-           pInbuf          主站下发的F106报文
输出参数：
-           无
返回：
-           无
****************************************************************************/
void commfun_WriteGradeInfo(uint16 GradeNum, char *pInbuf)
{
    char GradeFile[255];                //私有文件名称
    uint16 DI0DI1_Num = pInbuf[0];      //本次处理的数据项标识
    uint16 DI0DI1_Num_File = 0;         //私有外存中存放的数据项标识数量
    uint16 DI0DI1_Num_Write = 0;        //存入文件的分类数据数量
    uint16 GradeCS = 0;                 //校验和
    int i, j;

    if(GradeNum == 0 || GradeNum >= 255 || DI0DI1_Num == 0)
        return;

    memset(GradeFile, 0x0, sizeof(GradeFile));
    GetGradeFileName(GradeNum, GradeFile);

    //获得主站下发的电能表数据分级参数
    MET_GRADE_INFO dealGradeInfo[DI0DI1_Num];
    memcpy((void *)&dealGradeInfo, pInbuf + 1, sizeof(MET_GRADE_INFO) * DI0DI1_Num);

    DI0DI1_Num_File = GetGradeParaCount(GradeNum);      //从私有外存中读取的已经下发的数据项标识数量

    MET_GRADE_INFO writeGradeInfo[DI0DI1_Num + DI0DI1_Num_File];      //准备写入私有外存的
    memset((void *)writeGradeInfo, 0x0, sizeof(writeGradeInfo));

    DI0DI1_Num_Write = 0;
    //将本次下发的数据分级参数存入变量中
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
        MET_GRADE_INFO FileGradeInfo[DI0DI1_Num_File];      //文件中存放的数据分级参数
        ReadPDataXram(GradeFile, (char *)FileGradeInfo, 2, sizeof(MET_GRADE_INFO) * DI0DI1_Num_File);

        for(i = 0; i < DI0DI1_Num_File; i++)
        {
            //验证已经下发的参数是否均为有效参数
            if(FileGradeInfo[i].MtrGrade == 0xff || FileGradeInfo[i].MtrGrade == 0 || FileGradeInfo[i].MtrDI0DI1 == 0xffffffffl)
            {
                FileGradeInfo[i].MtrGrade = 0xff;
                FileGradeInfo[i].MtrDI0DI1 = 0xffffffffl;
                continue;
            }

            memcpy((void *)&writeGradeInfo[DI0DI1_Num_Write], (void *)&FileGradeInfo[i], sizeof(MET_GRADE_INFO));
            DI0DI1_Num_Write++;

            //如果私有外存中存在有效的分类参数，则需要与现有的参数进行比较，删除重复和无效的参数
            for(j = 0; j < DI0DI1_Num; j++)
            {
                //如果有数据项重复，无论数据等级是否为0，均不需要再处理
                if(FileGradeInfo[i].MtrDI0DI1 == dealGradeInfo[j].MtrDI0DI1)
                {
                    DI0DI1_Num_Write--;
                    break;
                }
            }
        }
    }

    RemovePDataXram(GradeFile);         //移除旧文件，重新写入
    if(DI0DI1_Num_Write > 0)
    {
        WritePDataXram(GradeFile, (char *)&DI0DI1_Num_Write, 0, sizeof(uint16));                            //数据项标识数量
        WritePDataXram(GradeFile, (char *)writeGradeInfo, 2, sizeof(MET_GRADE_INFO) * DI0DI1_Num_Write);    //“电能表数据分级参数”
        GradeCS = GetGradeCS((char *)writeGradeInfo, sizeof(MET_GRADE_INFO) * DI0DI1_Num_Write);
        WritePDataXram(GradeFile, (char *)&GradeCS, 2 + sizeof(MET_GRADE_INFO) * DI0DI1_Num_Write, 2);      //校验和
    }
}

//从私有外存中读取“F106：电能表数据分级参数”
int commfun_ReadGradeInfo(uint16 GradeNum, char *pOutbuf)
{
    uint16 DI0DI1_Num_File = 0;         //私有外存中存放的数据项标识数量
    char GradeFile[255];                //私有文件名称

    if(GradeNum == 0 || GradeNum >= 255)
        return -1;

    DI0DI1_Num_File = GetGradeParaCount(GradeNum);      //从私有外存中读取的已经下发的数据项标识数量

    if(DI0DI1_Num_File == 0)
        return -1;

    memset(GradeFile, 0x0, sizeof(GradeFile));
    GetGradeFileName(GradeNum, GradeFile);

    pOutbuf[0] = DI0DI1_Num_File;
    ReadPDataXram(GradeFile, pOutbuf + 1, 2, sizeof(MET_GRADE_INFO) * DI0DI1_Num_File);

    return 1 + sizeof(MET_GRADE_INFO) * DI0DI1_Num_File;
}

//参数初始化的时候，清除“电能表数据分级参数”
//GradeNum 0xffff-删除所有参数  其他-删除对应分类等级参数
void commfun_ClearGradeInfo(uint16 GradeNum)
{
    int i;
    char GradeFile[255];                //私有文件名称

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

//写入“F107：电能表数据分级周期参数”
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

//读取“F107：电能表数据分级周期参数”
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

//电能表事件发生次数：数据字典与645规约格式的对应关系
#if 1
#if (SHANXI1_FUN == 1 && HARDWARE_TYPE == 1) //陕西计量中心要求上报事件时需上报电表所有数据
const EventID_To_DI EventIDToDI[] = {
    //负荷开关误动或拒动
    {RT_DATA,  0,  E_EVENT_COUNT_00_ID,     0x03360000,     0xffffffff,     1,
    {0x03360001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //开表盖次数
    {RT_DATA,  10,  E_EVENT_COUNT_10_ID,    0x03300D00,     0xffffffff,     1,
    {0x03300D01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //开端钮盖次数
    {RT_DATA,  11,  E_EVENT_COUNT_11_ID,    0x03300E00,     0xffffffff,     1,
    {0x03300E01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //磁场次数
    {RT_DATA,  12,  E_EVENT_COUNT_12_ID,    0x03350000,     0xffffffff,     1,
    {0x03350001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电源异常
    {RT_DATA,  13,  E_EVENT_COUNT_13_ID,    0x03370000,     0xffffffff,     1,
    {0x03370001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //跳闸成功
    {RT_DATA,  14,  E_EVENT_COUNT_14_ID,    0x1D000001,     0xffffffff,     1,
    {0x1D000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //合闸成功
    {RT_DATA,  15,  E_EVENT_COUNT_15_ID,    0x1E000001,     0xffffffff,     1,
    {0x1E000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相失压
    {RT_DATA,  16,  E_EVENT_COUNT_16_ID,    0x10010001,     0x0000ff00,     2,
    {0x10010002, 0x1001ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相欠压
    {RT_DATA,  17,  E_EVENT_COUNT_17_ID,    0x11010001,     0x0000ff00,     2,
    {0x11010002, 0x1101ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相过压
    {RT_DATA,  18,  E_EVENT_COUNT_18_ID,    0x12010001,     0x0000ff00,     2,
    {0x12010101, 0x12012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相失流
    {RT_DATA,  19,  E_EVENT_COUNT_19_ID,    0x18010001,     0x0000ff00,     2,
    {0x18010002, 0x1801ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相过流
    {RT_DATA,  20,  E_EVENT_COUNT_20_ID,    0x19010001,     0x0000ff00,     2,
    {0x19010002, 0x1901ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相过载
    {RT_DATA,  21,  E_EVENT_COUNT_21_ID,    0x1C010001,     0x0000ff00,     2,
    {0x1C010101, 0x1C011201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相功率反向
    {RT_DATA,  22,  E_EVENT_COUNT_22_ID,    0x1B010001,     0x0000ff00,     2,
    {0x1B010101, 0x1B011201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相断相
    {RT_DATA,  23,  E_EVENT_COUNT_23_ID,    0x13010001,     0x0000ff00,     2,
    {0x13010002, 0x1301ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相断流
    {RT_DATA,  24,  E_EVENT_COUNT_24_ID,    0x1A010001,     0x0000ff00,     2,
    {0x1A010101, 0x1A012101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相失压
    {RT_DATA,  32,  E_EVENT_COUNT_32_ID,    0x10020001,     0x0000ff00,     2,
    {0x10020002, 0x1002ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相欠压
    {RT_DATA,  33,  E_EVENT_COUNT_33_ID,    0x11020001,     0x0000ff00,     2,
    {0x11020002, 0x1102ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相过压
    {RT_DATA,  34,  E_EVENT_COUNT_34_ID,    0x12020001,     0x0000ff00,     2,
    {0x12020101, 0x12022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相失流
    {RT_DATA,  35,  E_EVENT_COUNT_35_ID,    0x18020001,     0x0000ff00,     2,
    {0x18020101, 0x18022101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相过流
    {RT_DATA,  36,  E_EVENT_COUNT_36_ID,    0x19020001,     0x0000ff00,     2,
    {0x19020002, 0x1902ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相过载
    {RT_DATA,  37,  E_EVENT_COUNT_37_ID,    0x1C020001,     0x0000ff00,     2,
    {0x1C020101, 0x1C021201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相功率反向
    {RT_DATA,  38,  E_EVENT_COUNT_38_ID,    0x1B020001,     0x0000ff00,     2,
    {0x1B020101, 0x1B021201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相断相
    {RT_DATA,  39,  E_EVENT_COUNT_39_ID,    0x13020001,     0x0000ff00,     2,
    {0x13020002, 0x1302ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相断流
    {RT_DATA,  40,  E_EVENT_COUNT_40_ID,    0x1A020001,     0x0000ff00,     2,
    {0x1A020101, 0x1A022101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相失压
    {RT_DATA,  48,  E_EVENT_COUNT_48_ID,    0x10030001,     0x0000ff00,     2,
    {0x10030002, 0x1003ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相欠压
    {RT_DATA,  49,  E_EVENT_COUNT_49_ID,    0x11030001,     0x0000ff00,     2,
    {0x11030002, 0x1103ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相过压
    {RT_DATA,  50,  E_EVENT_COUNT_50_ID,    0x12030001,     0x0000ff00,     2,
    {0x12030101, 0x12032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相失流
    {RT_DATA,  51,  E_EVENT_COUNT_51_ID,    0x18030001,     0x0000ff00,     2,
    {0x18030101, 0x18032101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相过流
    {RT_DATA,  52,  E_EVENT_COUNT_52_ID,    0x19030001,     0x0000ff00,    2,
    {0x19030002, 0x1903ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相过载
    {RT_DATA,  53,  E_EVENT_COUNT_53_ID,    0x1C030001,     0x0000ff00,     2,
    {0x1C030101, 0x1C031201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相功率反向
    {RT_DATA,  54,  E_EVENT_COUNT_54_ID,    0x1B030001,     0x0000ff00,     2,
    {0x1B030101, 0x1B031201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相断相
    {RT_DATA,  55,  E_EVENT_COUNT_55_ID,    0x13030001,     0x0000ff00,     2,
    {0x13030002, 0x1303ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相断流
    {RT_DATA,  56,  E_EVENT_COUNT_56_ID,    0x1A030001,     0x0000ff00,     2,
    {0x1A030101, 0x1A032101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电压逆相序
    {RT_DATA,  64,  E_EVENT_COUNT_64_ID,    0x14000001,     0x0000ff00,     2,
    {0x14000101, 0x14001201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电流逆序次数
    {RT_DATA,  65,  E_EVENT_COUNT_65_ID,    0x15000001,     0x0000ff00,     2,
    {0x15000101, 0x15001201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电压不平衡次数
    {RT_DATA,  66,  E_EVENT_COUNT_66_ID,    0x16000001,     0x0000ff00,     2,
    {0x16000002, 0x1600ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电流不平衡次数
    {RT_DATA,  67,  E_EVENT_COUNT_67_ID,    0x17000001,     0x0000ff00,     2,
    {0x17000002, 0x1700ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //辅助电源失电
    {RT_DATA,  68,  E_EVENT_COUNT_68_ID,    0x03060000,     0xffffffff,     1,
    {0x03060001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //掉电次数
    {RT_DATA,  69,  E_EVENT_COUNT_69_ID,    0x03110000,     0xffffffff,     1,
    {0x03110001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //需量超限
    {RT_DATA,  70,  E_EVENT_COUNT_70_ID,    0x03120000,     0xffffffff,     6,
    {0x03120101, 0x03120201, 0x03120301, 0x03120401, 0x03120501, 0x03120601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //总功率因数超下限
    {RT_DATA,  71,  E_EVENT_COUNT_71_ID,    0x1F000001,     0x0000ff00,     2,
    {0x1F000101, 0x1F000601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电流严重不平衡
    {RT_DATA,  72,  E_EVENT_COUNT_72_ID,    0x20000001,     0x0000ff00,     2,
    {0x20000101, 0x20001301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //潮流反向
    {RT_DATA,  73,  E_EVENT_COUNT_73_ID,    0x21000000,     0xffffffff,     1,
    {0x21000001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //全失压
    {RT_DATA,  74,  E_EVENT_COUNT_74_ID,    0x03050000,     0xffffffff,     1,
    {0x03050001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //编程次数
    {RT_DATA,  80,  E_EVENT_COUNT_75_ID,    0x03300000,     0xffffffff,     1,
    {0x03300001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电表清零
    {RT_DATA,  81,  E_EVENT_COUNT_81_ID,    0x03300100,     0xffffffff,     1,
    {0x03300101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //需量清零次数（数据项太长了，不抄读事件记录了）
    {RT_DATA,  82,  E_EVENT_COUNT_82_ID,    0x03300200,     0xffffffff,     1,
    //{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    {0x03300201, 0x03300202, 0x03300203, 0x03300204, 0x03300205, 0x03300206, 0x03300207, 0x03300208, 0x03300209, 0x0330020A}},

    //事件清零
    {RT_DATA,  83,  E_EVENT_COUNT_83_ID,    0x03300300,     0xffffffff,     1,
    {0x03300301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //校时
    {RT_DATA,  84,  E_EVENT_COUNT_84_ID,    0x03300400,     0xffffffff,     1,
    {0x03300401, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //时段表编程
    {RT_DATA,  85,  E_EVENT_COUNT_85_ID,    0x03300500,     0xffffffff,     1,
    //{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    {0x03300501, 0x03300502, 0x03300503, 0x03300504, 0x03300505, 0x03300506, 0x03300507, 0x03300508, 0x03300509, 0x0330050A}},

    //时区表编程
    {RT_DATA,  86,  E_EVENT_COUNT_86_ID,    0x03300600,     0xffffffff,     1,
    {0x03300601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //周休日编程
    {RT_DATA,  87,  E_EVENT_COUNT_87_ID,    0x03300700,     0xffffffff,     1,
    {0x03300701, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //节假日编程
    {RT_DATA,  88,  E_EVENT_COUNT_88_ID,    0x03300800,     0xffffffff,     1,
    {0x03300801, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //有功组合方式编程
    {RT_DATA,  89,  E_EVENT_COUNT_89_ID,    0x03300900,     0xffffffff,     1,
    {0x03300901, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //无功组合方式1编程
    {RT_DATA,  90,  E_EVENT_COUNT_90_ID,    0x03300A00,     0xffffffff,     1,
    {0x03300A01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //无功组合方式2编程
    {RT_DATA,  91,  E_EVENT_COUNT_91_ID,    0x03300B00,     0xffffffff,     1,
    {0x03300B01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //结算日编程
    {RT_DATA,  92,  E_EVENT_COUNT_92_ID,    0x03300C00,     0xffffffff,     1,
    {0x03300C01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //费率参数表编程
    {RT_DATA,  93,  E_EVENT_COUNT_93_ID,    0x03300F00,     0xffffffff,     1,
    {0x03300F01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //阶梯表编程
    {RT_DATA,  94,  E_EVENT_COUNT_94_ID,    0x03301000,     0xffffffff,     1,
    {0x03301001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //密钥更新
    {RT_DATA,  95,  E_EVENT_COUNT_95_ID,    0x03301200,     0xffffffff,     1,
    {0x03301201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    {0xff,   0xff,  0xffff,                 0xffffffff,     0xffffffff,     0xff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
};
#elif (HUABEI_FUN == 1 && HARDWARE_TYPE == 4)
const EventID_To_DI EventIDToDI[] = {
    //负荷开关误动或拒动
    {RT_DATA,  0,  E_EVENT_COUNT_00_ID,     0x03360000,     0xffffffff,     1,
    {0x03360001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //开表盖次数
    {RT_DATA,  10,  E_EVENT_COUNT_10_ID,    0x03300D00,     0xffffffff,     1,
    {0x03300D01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //开端钮盖次数
    {RT_DATA,  11,  E_EVENT_COUNT_11_ID,    0x03300E00,     0xffffffff,     1,
    {0x03300E01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //磁场次数
    {RT_DATA,  12,  E_EVENT_COUNT_12_ID,    0x03350000,     0xffffffff,     1,
    {0x03350001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电源异常
    {RT_DATA,  13,  E_EVENT_COUNT_13_ID,    0x03370000,     0xffffffff,     1,
    {0x03370001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //跳闸成功
    {RT_DATA,  14,  E_EVENT_COUNT_14_ID,    0x1D000001,     0xffffffff,     1,
    {0x1D000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //合闸成功
    {RT_DATA,  15,  E_EVENT_COUNT_15_ID,    0x1E000001,     0xffffffff,     1,
    {0x1E000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相失压
    {RT_DATA,  16,  E_EVENT_COUNT_16_ID,    0x10010001,     0x0000ff00,     2,
    {0x10010002, 0x1001ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相欠压
    {RT_DATA,  17,  E_EVENT_COUNT_17_ID,    0x11010001,     0x0000ff00,     2,
    {0x11010002, 0x1101ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相过压
    {RT_DATA,  18,  E_EVENT_COUNT_18_ID,    0x12010001,     0x0000ff00,     2,
    {0x12010002, 0x1201ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相失流
    {RT_DATA,  19,  E_EVENT_COUNT_19_ID,    0x18010001,     0x0000ff00,     2,
    {0x18010002, 0x1801ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相过流
    {RT_DATA,  20,  E_EVENT_COUNT_20_ID,    0x19010001,     0x0000ff00,     2,
    {0x19010002, 0x1901ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相过载
    {RT_DATA,  21,  E_EVENT_COUNT_21_ID,    0x1C010001,     0x0000ff00,     2,
    {0x1C010002, 0x1C01ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相功率反向
    {RT_DATA,  22,  E_EVENT_COUNT_22_ID,    0x1B010001,     0x0000ff00,     2,
    {0x1B010002, 0x1B01ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相断相
    {RT_DATA,  23,  E_EVENT_COUNT_23_ID,    0x13010001,     0x0000ff00,     2,
    {0x13010002, 0x1301ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相断流
    {RT_DATA,  24,  E_EVENT_COUNT_24_ID,    0x1A010001,     0x0000ff00,     2,
    {0x1A010002, 0x1A01ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相失压
    {RT_DATA,  32,  E_EVENT_COUNT_32_ID,    0x10020001,     0x0000ff00,     2,
    {0x10020002, 0x1002ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相欠压
    {RT_DATA,  33,  E_EVENT_COUNT_33_ID,    0x11020001,     0x0000ff00,     2,
    {0x11020002, 0x1102ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相过压
    {RT_DATA,  34,  E_EVENT_COUNT_34_ID,    0x12020001,     0x0000ff00,     2,
    {0x12020002, 0x1202ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相失流
    {RT_DATA,  35,  E_EVENT_COUNT_35_ID,    0x18020001,     0x0000ff00,     2,
    {0x18020002, 0x1802ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相过流
    {RT_DATA,  36,  E_EVENT_COUNT_36_ID,    0x19020001,     0x0000ff00,     2,
    {0x19020002, 0x1902ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相过载
    {RT_DATA,  37,  E_EVENT_COUNT_37_ID,    0x1C020001,     0x0000ff00,     2,
    {0x1C020002, 0x1C02ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相功率反向
    {RT_DATA,  38,  E_EVENT_COUNT_38_ID,    0x1B020001,     0x0000ff00,     2,
    {0x1B020002, 0x1B02ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相断相
    {RT_DATA,  39,  E_EVENT_COUNT_39_ID,    0x13020001,     0x0000ff00,     2,
    {0x13020002, 0x1302ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相断流
    {RT_DATA,  40,  E_EVENT_COUNT_40_ID,    0x1A020001,     0x0000ff00,     2,
    {0x1A020002, 0x1A02ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相失压
    {RT_DATA,  48,  E_EVENT_COUNT_48_ID,    0x10030001,     0x0000ff00,     2,
    {0x10030002, 0x1003ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相欠压
    {RT_DATA,  49,  E_EVENT_COUNT_49_ID,    0x11030001,     0x0000ff00,     2,
    {0x11030002, 0x1103ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相过压
    {RT_DATA,  50,  E_EVENT_COUNT_50_ID,    0x12030001,     0x0000ff00,     2,
    {0x12030002, 0x1203ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相失流
    {RT_DATA,  51,  E_EVENT_COUNT_51_ID,    0x18030001,     0x0000ff00,     2,
    {0x18030002, 0x1803ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相过流
    {RT_DATA,  52,  E_EVENT_COUNT_52_ID,    0x19030001,     0x0000ff00,    2,
    {0x19030002, 0x1903ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相过载
    {RT_DATA,  53,  E_EVENT_COUNT_53_ID,    0x1C030001,     0x0000ff00,     2,
    {0x1C030002, 0x1C03ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相功率反向
    {RT_DATA,  54,  E_EVENT_COUNT_54_ID,    0x1B030001,     0x0000ff00,     2,
    {0x1B030002, 0x1B03ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相断相
    {RT_DATA,  55,  E_EVENT_COUNT_55_ID,    0x13030001,     0x0000ff00,     2,
    {0x13030002, 0x1303ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相断流
    {RT_DATA,  56,  E_EVENT_COUNT_56_ID,    0x1A030001,     0x0000ff00,     2,
    {0x1A030002, 0x1A03ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电压逆相序
    {RT_DATA,  64,  E_EVENT_COUNT_64_ID,    0x14000001,     0x0000ff00,     2,
    {0x14000002, 0x1400ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电流逆序次数
    {RT_DATA,  65,  E_EVENT_COUNT_65_ID,    0x15000001,     0x0000ff00,     2,
    {0x15000002, 0x1500ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电压不平衡次数
    {RT_DATA,  66,  E_EVENT_COUNT_66_ID,    0x16000001,     0x0000ff00,     2,
    {0x16000002, 0x1600ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电流不平衡次数
    {RT_DATA,  67,  E_EVENT_COUNT_67_ID,    0x17000001,     0x0000ff00,     2,
    {0x17000002, 0x1700ff01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //辅助电源失电
    {RT_DATA,  68,  E_EVENT_COUNT_68_ID,    0x03060000,     0xffffffff,     1,
    {0x03060001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //掉电次数
    {RT_DATA,  69,  E_EVENT_COUNT_69_ID,    0x03110000,     0xffffffff,     1,
    {0x03110001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //需量超限
    {RT_DATA,  70,  E_EVENT_COUNT_70_ID,    0x03120000,     0xffffffff,     6,
    {0x03120101, 0x03120201, 0x03120301, 0x03120401, 0x03120501, 0x03120601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //总功率因数超下限
    {RT_DATA,  71,  E_EVENT_COUNT_71_ID,    0x1F000001,     0x0000ff00,     2,
    {0x1F000101, 0x1F000601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电流严重不平衡
    {RT_DATA,  72,  E_EVENT_COUNT_72_ID,    0x20000001,     0x0000ff00,     2,
    {0x20000101, 0x20001301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //潮流反向
    {RT_DATA,  73,  E_EVENT_COUNT_73_ID,    0x21000000,     0xffffffff,     1,
    {0x21000001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //全失压
    {RT_DATA,  74,  E_EVENT_COUNT_74_ID,    0x03050000,     0xffffffff,     1,
    {0x03050001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //编程次数
    {RT_DATA,  80,  E_EVENT_COUNT_75_ID,    0x03300000,     0xffffffff,     1,
    {0x03300001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电表清零
    {RT_DATA,  81,  E_EVENT_COUNT_81_ID,    0x03300100,     0xffffffff,     1,
    {0x03300101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //需量清零次数（数据项太长了，不抄读事件记录了）
    {RT_DATA,  82,  E_EVENT_COUNT_82_ID,    0x03300200,     0xffffffff,     1,
    //{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    {0x03300201, 0x03300202, 0x03300203, 0x03300204, 0x03300205, 0x03300206, 0x03300207, 0x03300208, 0x03300209, 0x0330020A}},

    //事件清零
    {RT_DATA,  83,  E_EVENT_COUNT_83_ID,    0x03300300,     0xffffffff,     1,
    {0x03300301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //校时
    {RT_DATA,  84,  E_EVENT_COUNT_84_ID,    0x03300400,     0xffffffff,     1,
    {0x03300401, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //时段表编程
    {RT_DATA,  85,  E_EVENT_COUNT_85_ID,    0x03300500,     0xffffffff,     1,
    //{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    {0x03300501, 0x03300502, 0x03300503, 0x03300504, 0x03300505, 0x03300506, 0x03300507, 0x03300508, 0x03300509, 0x0330050A}},

    //时区表编程
    {RT_DATA,  86,  E_EVENT_COUNT_86_ID,    0x03300600,     0xffffffff,     1,
    {0x03300601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //周休日编程
    {RT_DATA,  87,  E_EVENT_COUNT_87_ID,    0x03300700,     0xffffffff,     1,
    {0x03300701, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //节假日编程
    {RT_DATA,  88,  E_EVENT_COUNT_88_ID,    0x03300800,     0xffffffff,     1,
    {0x03300801, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //有功组合方式编程
    {RT_DATA,  89,  E_EVENT_COUNT_89_ID,    0x03300900,     0xffffffff,     1,
    {0x03300901, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //无功组合方式1编程
    {RT_DATA,  90,  E_EVENT_COUNT_90_ID,    0x03300A00,     0xffffffff,     1,
    {0x03300A01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //无功组合方式2编程
    {RT_DATA,  91,  E_EVENT_COUNT_91_ID,    0x03300B00,     0xffffffff,     1,
    {0x03300B01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //结算日编程
    {RT_DATA,  92,  E_EVENT_COUNT_92_ID,    0x03300C00,     0xffffffff,     1,
    {0x03300C01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //费率参数表编程
    {RT_DATA,  93,  E_EVENT_COUNT_93_ID,    0x03300F00,     0xffffffff,     1,
    {0x03300F01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //阶梯表编程
    {RT_DATA,  94,  E_EVENT_COUNT_94_ID,    0x03301000,     0xffffffff,     1,
    {0x03301001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //密钥更新
    {RT_DATA,  95,  E_EVENT_COUNT_95_ID,    0x03301200,     0xffffffff,     1,
    {0x03301201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    {0xff,   0xff,  0xffff,                 0xffffffff,     0xffffffff,     0xff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
};
#else
const EventID_To_DI EventIDToDI[] = {
    //负荷开关误动或拒动
    {RT_DATA,  0,  E_EVENT_COUNT_00_ID,     0x03360000,     0xffffffff,     1,
    {0x03360001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //开表盖次数
    {RT_DATA,  10,  E_EVENT_COUNT_10_ID,    0x03300D00,     0xffffffff,     1,
    {0x03300D01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //开端钮盖次数
    {RT_DATA,  11,  E_EVENT_COUNT_11_ID,    0x03300E00,     0xffffffff,     1,
    {0x03300E01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //磁场次数
    {RT_DATA,  12,  E_EVENT_COUNT_12_ID,    0x03350000,     0xffffffff,     1,
    {0x03350001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电源异常
    {RT_DATA,  13,  E_EVENT_COUNT_13_ID,    0x03370000,     0xffffffff,     1,
    {0x03370001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //跳闸成功
    {RT_DATA,  14,  E_EVENT_COUNT_14_ID,    0x1D000001,     0xffffffff,     1,
    {0x1D000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //合闸成功
    {RT_DATA,  15,  E_EVENT_COUNT_15_ID,    0x1E000001,     0xffffffff,     1,
    {0x1E000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相失压
    {RT_DATA,  16,  E_EVENT_COUNT_16_ID,    0x10010001,     0x0000ff00,     2,
    {0x10010101, 0x10012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相欠压
    {RT_DATA,  17,  E_EVENT_COUNT_17_ID,    0x11010001,     0x0000ff00,     2,
    {0x11010101, 0x11012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相过压
    {RT_DATA,  18,  E_EVENT_COUNT_18_ID,    0x12010001,     0x0000ff00,     2,
    {0x12010101, 0x12012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相失流
    {RT_DATA,  19,  E_EVENT_COUNT_19_ID,    0x18010001,     0x0000ff00,     2,
    {0x18010101, 0x18012101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相过流
    {RT_DATA,  20,  E_EVENT_COUNT_20_ID,    0x19010001,     0x0000ff00,     2,
    {0x19010101, 0x19012101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相过载
    {RT_DATA,  21,  E_EVENT_COUNT_21_ID,    0x1C010001,     0x0000ff00,     2,
    {0x1C010101, 0x1C011201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相功率反向
    {RT_DATA,  22,  E_EVENT_COUNT_22_ID,    0x1B010001,     0x0000ff00,     2,
    {0x1B010101, 0x1B011201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相断相
    {RT_DATA,  23,  E_EVENT_COUNT_23_ID,    0x13010001,     0x0000ff00,     2,
    {0x13010101, 0x13012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相断流
    {RT_DATA,  24,  E_EVENT_COUNT_24_ID,    0x1A010001,     0x0000ff00,     2,
    {0x1A010101, 0x1A012101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相失压
    {RT_DATA,  32,  E_EVENT_COUNT_32_ID,    0x10020001,     0x0000ff00,     2,
    {0x10020101, 0x10022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相欠压
    {RT_DATA,  33,  E_EVENT_COUNT_33_ID,    0x11020001,     0x0000ff00,     2,
    {0x11020101, 0x11022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相过压
    {RT_DATA,  34,  E_EVENT_COUNT_34_ID,    0x12020001,     0x0000ff00,     2,
    {0x12020101, 0x12022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相失流
    {RT_DATA,  35,  E_EVENT_COUNT_35_ID,    0x18020001,     0x0000ff00,     2,
    {0x18020101, 0x18022101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相过流
    {RT_DATA,  36,  E_EVENT_COUNT_36_ID,    0x19020001,     0x0000ff00,     2,
    {0x19020101, 0x19022101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相过载
    {RT_DATA,  37,  E_EVENT_COUNT_37_ID,    0x1C020001,     0x0000ff00,     2,
    {0x1C020101, 0x1C021201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相功率反向
    {RT_DATA,  38,  E_EVENT_COUNT_38_ID,    0x1B020001,     0x0000ff00,     2,
    {0x1B020101, 0x1B021201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相断相
    {RT_DATA,  39,  E_EVENT_COUNT_39_ID,    0x13020001,     0x0000ff00,     2,
    {0x13020101, 0x13022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相断流
    {RT_DATA,  40,  E_EVENT_COUNT_40_ID,    0x1A020001,     0x0000ff00,     2,
    {0x1A020101, 0x1A022101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相失压
    {RT_DATA,  48,  E_EVENT_COUNT_48_ID,    0x10030001,     0x0000ff00,     2,
    {0x10030101, 0x10032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相欠压
    {RT_DATA,  49,  E_EVENT_COUNT_49_ID,    0x11030001,     0x0000ff00,     2,
    {0x11030101, 0x11032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相过压
    {RT_DATA,  50,  E_EVENT_COUNT_50_ID,    0x12030001,     0x0000ff00,     2,
    {0x12030101, 0x12032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相失流
    {RT_DATA,  51,  E_EVENT_COUNT_51_ID,    0x18030001,     0x0000ff00,     2,
    {0x18030101, 0x18032101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相过流
    {RT_DATA,  52,  E_EVENT_COUNT_52_ID,    0x19030001,     0x0000ff00,     2,
    {0x19030101, 0x19032101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相过载
    {RT_DATA,  53,  E_EVENT_COUNT_53_ID,    0x1C030001,     0x0000ff00,     2,
    {0x1C030101, 0x1C031201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相功率反向
    {RT_DATA,  54,  E_EVENT_COUNT_54_ID,    0x1B030001,     0x0000ff00,     2,
    {0x1B030101, 0x1B031201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相断相
    {RT_DATA,  55,  E_EVENT_COUNT_55_ID,    0x13030001,     0x0000ff00,     2,
    {0x13030101, 0x13032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相断流
    {RT_DATA,  56,  E_EVENT_COUNT_56_ID,    0x1A030001,     0x0000ff00,     2,
    {0x1A030101, 0x1A032101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电压逆相序
    {RT_DATA,  64,  E_EVENT_COUNT_64_ID,    0x14000001,     0x0000ff00,     2,
    {0x14000101, 0x14001201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电流逆序次数
    {RT_DATA,  65,  E_EVENT_COUNT_65_ID,    0x15000001,     0x0000ff00,     2,
    {0x15000101, 0x15001201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电压不平衡次数
    {RT_DATA,  66,  E_EVENT_COUNT_66_ID,    0x16000001,     0x0000ff00,     2,
    {0x16000101, 0x16001301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电流不平衡次数
    {RT_DATA,  67,  E_EVENT_COUNT_67_ID,    0x17000001,     0x0000ff00,     2,
    {0x17000101, 0x17001301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //辅助电源失电
    {RT_DATA,  68,  E_EVENT_COUNT_68_ID,    0x03060000,     0xffffffff,     1,
    {0x03060001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //掉电次数
    {RT_DATA,  69,  E_EVENT_COUNT_69_ID,    0x03110000,     0xffffffff,     1,
    {0x03110001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //需量超限
    {RT_DATA,  70,  E_EVENT_COUNT_70_ID,    0x03120000,     0xffffffff,     6,
    {0x03120101, 0x03120201, 0x03120301, 0x03120401, 0x03120501, 0x03120601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //总功率因数超下限
    {RT_DATA,  71,  E_EVENT_COUNT_71_ID,    0x1F000001,     0x0000ff00,     2,
    {0x1F000101, 0x1F000601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电流严重不平衡
    {RT_DATA,  72,  E_EVENT_COUNT_72_ID,    0x20000001,     0x0000ff00,     2,
    {0x20000101, 0x20001301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //潮流反向
    {RT_DATA,  73,  E_EVENT_COUNT_73_ID,    0x21000000,     0xffffffff,     1,
    {0x21000001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //全失压
    {RT_DATA,  74,  E_EVENT_COUNT_74_ID,    0x03050000,     0xffffffff,     1,
    {0x03050001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //编程次数
    {RT_DATA,  80,  E_EVENT_COUNT_75_ID,    0x03300000,     0xffffffff,     1,
    {0x03300001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电表清零
    {RT_DATA,  81,  E_EVENT_COUNT_81_ID,    0x03300100,     0xffffffff,     1,
    {0x03300101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //需量清零次数（数据项太长了，不抄读事件记录了）
    {RT_DATA,  82,  E_EVENT_COUNT_82_ID,    0x03300200,     0xffffffff,     1,
    //{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    {0x03300201, 0x03300202, 0x03300203, 0x03300204, 0x03300205, 0x03300206, 0x03300207, 0x03300208, 0x03300209, 0x0330020A}},

    //事件清零
    {RT_DATA,  83,  E_EVENT_COUNT_83_ID,    0x03300300,     0xffffffff,     1,
    {0x03300301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //校时
    {RT_DATA,  84,  E_EVENT_COUNT_84_ID,    0x03300400,     0xffffffff,     1,
    {0x03300401, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //时段表编程
    {RT_DATA,  85,  E_EVENT_COUNT_85_ID,    0x03300500,     0xffffffff,     1,
    //{0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    {0x03300501, 0x03300502, 0x03300503, 0x03300504, 0x03300505, 0x03300506, 0x03300507, 0x03300508, 0x03300509, 0x0330050A}},

    //时区表编程
    {RT_DATA,  86,  E_EVENT_COUNT_86_ID,    0x03300600,     0xffffffff,     1,
    {0x03300601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //周休日编程
    {RT_DATA,  87,  E_EVENT_COUNT_87_ID,    0x03300700,     0xffffffff,     1,
    {0x03300701, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //节假日编程
    {RT_DATA,  88,  E_EVENT_COUNT_88_ID,    0x03300800,     0xffffffff,     1,
    {0x03300801, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //有功组合方式编程
    {RT_DATA,  89,  E_EVENT_COUNT_89_ID,    0x03300900,     0xffffffff,     1,
    {0x03300901, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //无功组合方式1编程
    {RT_DATA,  90,  E_EVENT_COUNT_90_ID,    0x03300A00,     0xffffffff,     1,
    {0x03300A01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //无功组合方式2编程
    {RT_DATA,  91,  E_EVENT_COUNT_91_ID,    0x03300B00,     0xffffffff,     1,
    {0x03300B01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //结算日编程
    {RT_DATA,  92,  E_EVENT_COUNT_92_ID,    0x03300C00,     0xffffffff,     1,
    {0x03300C01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //费率参数表编程
    {RT_DATA,  93,  E_EVENT_COUNT_93_ID,    0x03300F00,     0xffffffff,     1,
    {0x03300F01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //阶梯表编程
    {RT_DATA,  94,  E_EVENT_COUNT_94_ID,    0x03301000,     0xffffffff,     1,
    {0x03301001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //密钥更新
    {RT_DATA,  95,  E_EVENT_COUNT_95_ID,    0x03301200,     0xffffffff,     1,
    {0x03301201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    {0xff,   0xff,  0xffff,                 0xffffffff,     0xffffffff,     0xff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
};
#endif
#else
const EventID_To_DI EventIDToDI[] = {
    //负荷开关误动或拒动
    {RT_DATA,  0,  E_EVENT_COUNT_00_ID,     0x03360000,     0xffffffff,
    {0x03360001, 0x03360002, 0x03360003, 0x03360004, 0x03360005, 0x03360006, 0x03360007, 0x03360008, 0x03360009, 0x0336000A}},

    //开表盖次数
    {RT_DATA,  10,  E_EVENT_COUNT_10_ID,    0x03300D00,     0xffffffff,
    {0x03300D01, 0x03300D02, 0x03300D03, 0x03300D04, 0x03300D05, 0x03300D06, 0x03300D07, 0x03300D08, 0x03300D09, 0x03300D0A}},

    //开端钮盖次数
    {RT_DATA,  11,  E_EVENT_COUNT_11_ID,    0x03300E00,     0xffffffff,
    {0x03300E01, 0x03300E02, 0x03300E03, 0x03300E04, 0x03300E05, 0x03300E06, 0x03300E07, 0x03300E08, 0x03300E09, 0x03300E0A}},

    //磁场次数
    {RT_DATA,  12,  E_EVENT_COUNT_12_ID,    0x03350000,     0xffffffff,
    {0x03350001, 0x03350002, 0x03350003, 0x03350004, 0x03350005, 0x03350006, 0x03350007, 0x03350008, 0x03350009, 0x0335000A}},

    //电源异常
    {RT_DATA,  13,  E_EVENT_COUNT_13_ID,    0x03370000,     0xffffffff,
    {0x03370001, 0x03370002, 0x03370003, 0x03370004, 0x03370005, 0x03370006, 0x03370007, 0x03370008, 0x03370009, 0x0337000A}},

    //跳闸成功
    {RT_DATA,  14,  E_EVENT_COUNT_14_ID,    0x1D000001,     0xffffffff,
    {0x1D00FF01, 0x1D00FF02, 0x1D00FF03, 0x1D00FF04, 0x1D00FF05, 0x1D00FF06, 0x1D00FF07, 0x1D00FF08, 0x1D00FF09, 0x1D00FF0A}},

    //合闸成功
    {RT_DATA,  15,  E_EVENT_COUNT_15_ID,    0x1E000001,     0xffffffff,
    {0x1E00FF01, 0x1E00FF02, 0x1E00FF03, 0x1E00FF04, 0x1E00FF05, 0x1E00FF06, 0x1E00FF07, 0x1E00FF08, 0x1E00FF09, 0x1E00FF0A}},

    //A相失压
    {RT_DATA,  16,  E_EVENT_COUNT_16_ID,    0x10010001,     0x0000ff00,
    {0x10010101, 0x10010102, 0x10010103, 0x10010104, 0x10010105, 0x10010106, 0x10010107, 0x10010108, 0x10010109, 0x1001010A}},

    //A相欠压
    {RT_DATA,  17,  E_EVENT_COUNT_17_ID,    0x11010001,     0x0000ff00,
    {0x11010101, 0x11010102, 0x11010103, 0x11010104, 0x11010105, 0x11010106, 0x11010107, 0x11010108, 0x11010109, 0x1101010A}},

    //A相过压
    {RT_DATA,  18,  E_EVENT_COUNT_18_ID,    0x12010001,     0x0000ff00,
    {0x12010101, 0x12010102, 0x12010103, 0x12010104, 0x12010105, 0x12010106, 0x12010107, 0x12010108, 0x12010109, 0x1201010A}},

    //A相失流
    {RT_DATA,  19,  E_EVENT_COUNT_19_ID,    0x18010001,     0x0000ff00,
    {0x18010101, 0x18010102, 0x18010103, 0x18010104, 0x18010105, 0x18010106, 0x18010107, 0x18010108, 0x18010109, 0x1801010A}},

    //A相过流
    {RT_DATA,  20,  E_EVENT_COUNT_20_ID,    0x19010001,     0x0000ff00,
    {0x19010101, 0x19010102, 0x19010103, 0x19010104, 0x19010105, 0x19010106, 0x19010107, 0x19010108, 0x19010109, 0x1901010A}},

    //A相过载
    {RT_DATA,  21,  E_EVENT_COUNT_21_ID,    0x1C010001,     0x0000ff00,
    {0x1C01FF01, 0x1C01FF02, 0x1C01FF03, 0x1C01FF04, 0x1C01FF05, 0x1C01FF06, 0x1C01FF07, 0x1C01FF08, 0x1C01FF09, 0x1C01FF0A}},

    //A相功率反向
    {RT_DATA,  22,  E_EVENT_COUNT_22_ID,    0x1B010001,     0x0000ff00,
    {0x1B01FF01, 0x1B01FF02, 0x1B01FF03, 0x1B01FF04, 0x1B01FF05, 0x1B01FF06, 0x1B01FF07, 0x1B01FF08, 0x1B01FF09, 0x1B01FF0A}},

    //A相断相
    {RT_DATA,  23,  E_EVENT_COUNT_23_ID,    0x13010001,     0x0000ff00,
    {0x13010101, 0x13010102, 0x13010103, 0x13010104, 0x13010105, 0x13010106, 0x13010107, 0x13010108, 0x13010109, 0x1301010A}},

    //A相断流
    {RT_DATA,  24,  E_EVENT_COUNT_24_ID,    0x1A010001,     0x0000ff00,
    {0x1A010101, 0x1A010102, 0x1A010103, 0x1A010104, 0x1A010105, 0x1A010106, 0x1A010107, 0x1A010108, 0x1A010109, 0x1A01010A}},

    //B相失压
    {RT_DATA,  32,  E_EVENT_COUNT_32_ID,    0x10020001,     0x0000ff00,
    {0x10020101, 0x10020102, 0x10020103, 0x10020104, 0x10020105, 0x10020106, 0x10020107, 0x10020108, 0x10020109, 0x1002010A}},

    //B相欠压
    {RT_DATA,  33,  E_EVENT_COUNT_33_ID,    0x11020001,     0x0000ff00,
    {0x11020101, 0x11020102, 0x11020103, 0x11020104, 0x11020105, 0x11020106, 0x11020107, 0x11020108, 0x11020109, 0x1102010A}},

    //B相过压
    {RT_DATA,  34,  E_EVENT_COUNT_34_ID,    0x12020001,     0x0000ff00,
    {0x12020101, 0x12020102, 0x12020103, 0x12020104, 0x12020105, 0x12020106, 0x12020107, 0x12020108, 0x12020109, 0x1202010A}},

    //B相失流
    {RT_DATA,  35,  E_EVENT_COUNT_35_ID,    0x18020001,     0x0000ff00,
    {0x18020101, 0x18020102, 0x18020103, 0x18020104, 0x18020105, 0x18020106, 0x18020107, 0x18020108, 0x18020109, 0x1802010A}},

    //B相过流
    {RT_DATA,  36,  E_EVENT_COUNT_36_ID,    0x19020001,     0x0000ff00,
    {0x19020101, 0x19020102, 0x19020103, 0x19020104, 0x19020105, 0x19020106, 0x19020107, 0x19020108, 0x19020109, 0x1902010A}},

    //B相过载
    {RT_DATA,  37,  E_EVENT_COUNT_37_ID,    0x1C020001,     0x0000ff00,
    {0x1C02FF01, 0x1C02FF02, 0x1C02FF03, 0x1C02FF04, 0x1C02FF05, 0x1C02FF06, 0x1C02FF07, 0x1C02FF08, 0x1C02FF09, 0x1C02FF0A}},

    //B相功率反向
    {RT_DATA,  38,  E_EVENT_COUNT_38_ID,    0x1B020001,     0x0000ff00,
    {0x1B02FF01, 0x1B02FF02, 0x1B02FF03, 0x1B02FF04, 0x1B02FF05, 0x1B02FF06, 0x1B02FF07, 0x1B02FF08, 0x1B02FF09, 0x1B02FF0A}},

    //B相断相
    {RT_DATA,  39,  E_EVENT_COUNT_39_ID,    0x13020001,     0x0000ff00,
    {0x13020101, 0x13020102, 0x13020103, 0x13020104, 0x13020105, 0x13020106, 0x13020107, 0x13020108, 0x13020109, 0x1302010A}},

    //B相断流
    {RT_DATA,  40,  E_EVENT_COUNT_40_ID,    0x1A020001,     0x0000ff00,
    {0x1A020101, 0x1A020102, 0x1A020103, 0x1A020104, 0x1A020105, 0x1A020106, 0x1A020107, 0x1A020108, 0x1A020109, 0x1A02010A}},

    //C相失压
    {RT_DATA,  48,  E_EVENT_COUNT_48_ID,    0x10030001,     0x0000ff00,
    {0x10030101, 0x10030102, 0x10030103, 0x10030104, 0x10030105, 0x10030106, 0x10030107, 0x10030108, 0x10030109, 0x1003010A}},

    //C相欠压
    {RT_DATA,  49,  E_EVENT_COUNT_49_ID,    0x11030001,     0x0000ff00,
    {0x11030101, 0x11030102, 0x11030103, 0x11030104, 0x11030105, 0x11030106, 0x11030107, 0x11030108, 0x11030109, 0x1103010A}},

    //C相过压
    {RT_DATA,  50,  E_EVENT_COUNT_50_ID,    0x12030001,     0x0000ff00,
    {0x12030101, 0x12030102, 0x12030103, 0x12030104, 0x12030105, 0x12030106, 0x12030107, 0x12030108, 0x12030109, 0x1203010A}},

    //C相失流
    {RT_DATA,  51,  E_EVENT_COUNT_51_ID,    0x18030001,     0x0000ff00,
    {0x18030101, 0x18030102, 0x18030103, 0x18030104, 0x18030105, 0x18030106, 0x18030107, 0x18030108, 0x18030109, 0x1803010A}},

    //C相过流
    {RT_DATA,  52,  E_EVENT_COUNT_52_ID,    0x19030001,     0x0000ff00,
    {0x19030101, 0x19030102, 0x19030103, 0x19030104, 0x19030105, 0x19030106, 0x19030107, 0x19030108, 0x19030109, 0x1903010A}},

    //C相过载
    {RT_DATA,  53,  E_EVENT_COUNT_53_ID,    0x1C030001,     0x0000ff00,
    {0x1C03FF01, 0x1C03FF02, 0x1C03FF03, 0x1C03FF04, 0x1C03FF05, 0x1C03FF06, 0x1C03FF07, 0x1C03FF08, 0x1C03FF09, 0x1C03FF0A}},

    //C相功率反向
    {RT_DATA,  54,  E_EVENT_COUNT_54_ID,    0x1B030001,     0x0000ff00,
    {0x1B03FF01, 0x1B03FF02, 0x1B03FF03, 0x1B03FF04, 0x1B03FF05, 0x1B03FF06, 0x1B03FF07, 0x1B03FF08, 0x1B03FF09, 0x1B03FF0A}},

    //C相断相
    {RT_DATA,  55,  E_EVENT_COUNT_55_ID,    0x13030001,     0x0000ff00,
    {0x13030101, 0x13030102, 0x13030103, 0x13030104, 0x13030105, 0x13030106, 0x13030107, 0x13030108, 0x13030109, 0x1303010A}},

    //C相断流
    {RT_DATA,  56,  E_EVENT_COUNT_56_ID,    0x1A030001,     0x0000ff00,
    {0x1A030101, 0x1A030102, 0x1A030103, 0x1A030104, 0x1A030105, 0x1A030106, 0x1A030107, 0x1A030108, 0x1A030109, 0x1A03010A}},

    //电压逆相序
    {RT_DATA,  64,  E_EVENT_COUNT_64_ID,    0x14000001,     0x0000ff00,
    {0x1400FF01, 0x1400FF02, 0x1400FF03, 0x1400FF04, 0x1400FF05, 0x1400FF06, 0x1400FF07, 0x1400FF08, 0x1400FF09, 0x1400FF0A}},

    //电流逆序次数
    {RT_DATA,  65,  E_EVENT_COUNT_65_ID,    0x15000001,     0x0000ff00,
    {0x1500FF01, 0x1500FF02, 0x1500FF03, 0x1500FF04, 0x1500FF05, 0x1500FF06, 0x1500FF07, 0x1500FF08, 0x1500FF09, 0x1500FF0A}},

    //电压不平衡次数
    {RT_DATA,  66,  E_EVENT_COUNT_66_ID,    0x16000001,     0x0000ff00,
    {0x1600FF01, 0x1600FF02, 0x1600FF03, 0x1600FF04, 0x1600FF05, 0x1600FF06, 0x1600FF07, 0x1600FF08, 0x1600FF09, 0x1600FF0A}},

    //电流不平衡次数
    {RT_DATA,  67,  E_EVENT_COUNT_67_ID,    0x17000001,     0x0000ff00,
    {0x1700FF01, 0x1700FF02, 0x1700FF03, 0x1700FF04, 0x1700FF05, 0x1700FF06, 0x1700FF07, 0x1700FF08, 0x1700FF09, 0x1700FF0A}},

    //辅助电源失电
    {RT_DATA,  68,  E_EVENT_COUNT_68_ID,    0x03060000,     0xffffffff,
    {0x03060001, 0x03060002, 0x03060003, 0x03060004, 0x03060005, 0x03060006, 0x03060007, 0x03060008, 0x03060009, 0x0306000A}},

    //掉电次数
    {RT_DATA,  69,  E_EVENT_COUNT_69_ID,    0x03110000,     0xffffffff,
    {0x03110001, 0x03110002, 0x03110003, 0x03110004, 0x03110005, 0x03110006, 0x03110007, 0x03110008, 0x03110009, 0x0311000A}},

    //需量超限
    {RT_DATA,  70,  E_EVENT_COUNT_70_ID,    0x03120000,     0xffffffff,
    {0x03120101, 0x03120102, 0x03120103, 0x03120104, 0x03120105, 0x03120106, 0x03120107, 0x03120108, 0x03120109, 0x0312010A}},

    //总功率因数超下限
    {RT_DATA,  71,  E_EVENT_COUNT_71_ID,    0x1F000001,     0x0000ff00,
    {0x1F00FF01, 0x1F00FF02, 0x1F00FF03, 0x1F00FF04, 0x1F00FF05, 0x1F00FF06, 0x1F00FF07, 0x1F00FF08, 0x1F00FF09, 0x1F00FF0A}},

    //电流严重不平衡
    {RT_DATA,  72,  E_EVENT_COUNT_72_ID,    0x20000001,     0x0000ff00,
    {0x2000FF01, 0x2000FF02, 0x2000FF03, 0x2000FF04, 0x2000FF05, 0x2000FF06, 0x2000FF07, 0x2000FF08, 0x2000FF09, 0x2000FF0A}},

    //潮流反向
    {RT_DATA,  73,  E_EVENT_COUNT_73_ID,    0x21000000,     0xffffffff,
    {0x21000001, 0x21000002, 0x21000003, 0x21000004, 0x21000005, 0x21000006, 0x21000007, 0x21000008, 0x21000009, 0x2100000A}},

    //全失压
    {RT_DATA,  74,  E_EVENT_COUNT_74_ID,    0x03050000,     0xffffffff,
    {0x03050001, 0x03050002, 0x03050003, 0x03050004, 0x03050005, 0x03050006, 0x03050007, 0x03050008, 0x03050009, 0x0305000A}},

    //编程次数
    {RT_DATA,  80,  E_EVENT_COUNT_75_ID,    0x03300000,     0xffffffff,
    {0x03300001, 0x03300002, 0x03300003, 0x03300004, 0x03300005, 0x03300006, 0x03300007, 0x03300008, 0x03300009, 0x0330000A}},

    //电表清零
    {RT_DATA,  81,  E_EVENT_COUNT_81_ID,    0x03300100,     0xffffffff,
    {0x03300101, 0x03300102, 0x03300103, 0x03300104, 0x03300105, 0x03300106, 0x03300107, 0x03300108, 0x03300109, 0x0330010A}},

    //需量清零次数（数据项太长了，不抄读事件记录了）
    {RT_DATA,  82,  E_EVENT_COUNT_82_ID,    0x03300200,     0xffffffff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    //{0x03300201, 0x03300202, 0x03300203, 0x03300204, 0x03300205, 0x03300206, 0x03300207, 0x03300208, 0x03300209, 0x0330020A}},

    //事件清零
    {RT_DATA,  83,  E_EVENT_COUNT_83_ID,    0x03300300,     0xffffffff,
    {0x03300301, 0x03300302, 0x03300303, 0x03300304, 0x03300305, 0x03300306, 0x03300307, 0x03300308, 0x03300309, 0x0330030A}},

    //校时
    {RT_DATA,  84,  E_EVENT_COUNT_84_ID,    0x03300400,     0xffffffff,
    {0x03300401, 0x03300402, 0x03300403, 0x03300404, 0x03300405, 0x03300406, 0x03300407, 0x03300408, 0x03300409, 0x0330040A}},

    //时段表编程
    {RT_DATA,  85,  E_EVENT_COUNT_85_ID,    0x03300500,     0xffffffff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    //{0x03300501, 0x03300502, 0x03300503, 0x03300504, 0x03300505, 0x03300506, 0x03300507, 0x03300508, 0x03300509, 0x0330050A}},

    //时区表编程
    {RT_DATA,  86,  E_EVENT_COUNT_86_ID,    0x03300600,     0xffffffff,
    {0x03300601, 0x03300602, 0x03300603, 0x03300604, 0x03300605, 0x03300606, 0x03300607, 0x03300608, 0x03300609, 0x0330060A}},

    //周休日编程
    {RT_DATA,  87,  E_EVENT_COUNT_87_ID,    0x03300700,     0xffffffff,
    {0x03300701, 0x03300702, 0x03300703, 0x03300704, 0x03300705, 0x03300706, 0x03300707, 0x03300708, 0x03300709, 0x0330070A}},

    //节假日编程
    {RT_DATA,  88,  E_EVENT_COUNT_88_ID,    0x03300800,     0xffffffff,
    {0x03300801, 0x03300802, 0x03300803, 0x03300804, 0x03300805, 0x03300806, 0x03300807, 0x03300808, 0x03300809, 0x0330080A}},

    //有功组合方式编程
    {RT_DATA,  89,  E_EVENT_COUNT_89_ID,    0x03300900,     0xffffffff,
    {0x03300901, 0x03300902, 0x03300903, 0x03300904, 0x03300905, 0x03300906, 0x03300907, 0x03300908, 0x03300909, 0x0330090A}},

    //无功组合方式1编程
    {RT_DATA,  90,  E_EVENT_COUNT_90_ID,    0x03300A00,     0xffffffff,
    {0x03300A01, 0x03300A02, 0x03300A03, 0x03300A04, 0x03300A05, 0x03300A06, 0x03300A07, 0x03300A08, 0x03300A09, 0x03300A0A}},

    //无功组合方式2编程
    {RT_DATA,  91,  E_EVENT_COUNT_91_ID,    0x03300B00,     0xffffffff,
    {0x03300B01, 0x03300B02, 0x03300B03, 0x03300B04, 0x03300B05, 0x03300B06, 0x03300B07, 0x03300B08, 0x03300B09, 0x03300B0A}},

    //结算日编程
    {RT_DATA,  92,  E_EVENT_COUNT_92_ID,    0x03300C00,     0xffffffff,
    {0x03300C01, 0x03300C02, 0x03300C03, 0x03300C04, 0x03300C05, 0x03300C06, 0x03300C07, 0x03300C08, 0x03300C09, 0x03300C0A}},

    //费率参数表编程
    {RT_DATA,  93,  E_EVENT_COUNT_93_ID,    0x03300F00,     0xffffffff,
    {0x03300F01, 0x03300F02, 0x03300F03, 0x03300F04, 0x03300F05, 0x03300F06, 0x03300F07, 0x03300F08, 0x03300F09, 0x03300F0A}},

    //阶梯表编程
    {RT_DATA,  94,  E_EVENT_COUNT_94_ID,    0x03301000,     0xffffffff,
    {0x03301001, 0x03301002, 0x03301003, 0x03301004, 0x03301005, 0x03301006, 0x03301007, 0x03301008, 0x03301009, 0x0330100A}},

    //密钥更新
    {RT_DATA,  95,  E_EVENT_COUNT_95_ID,    0x03301200,     0xffffffff,
    {0x03301201, 0x03301202, 0x03301203, 0x03301204, 0x03301205, 0x03301206, 0x03301207, 0x03301208, 0x03301209, 0x0330120A}},

    {0xff,   0xff,  0xffff,                 0xffffffff,     0xffffffff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
};
#endif


//根据645规约ID查找EventIDToDI中的成员
const EventID_To_DI *GetEventIDToDI(uint32 MtrDI)
{
    int DICount = (sizeof(EventIDToDI) / sizeof(EventIDToDI[0])) - 1;       //EventIDToDI成员的数量
    int i = 0;

    for(i = 0; i < DICount; i++)
    {
        if(EventIDToDI[i].DI_EventCount == MtrDI)     //属于事件次数相关的数据项
            return &EventIDToDI[i];
    }
    return NULL;
}

const EventID_To_DI *GetEventIDToDIByIndex(uint8 IDIndex)
{
    int DICount = (sizeof(EventIDToDI) / sizeof(EventIDToDI[0])) - 1;       //EventIDToDI成员的数量
    int i = 0;

    for(i = 0; i < DICount; i++)
    {
        if(EventIDToDI[i].WorkStatBit == IDIndex)     //状态字对应的位信息
            return &EventIDToDI[i];
    }
    return NULL;
}
#endif

#if ACADDR_FROM_PRTL == 1
/*
  名  称: commfun_ReadACCommAddr
  功  能: 读取交采通讯地址
  输  入: 无
  输  出: pAddr 交采通讯地址，低字节在前，BCD格式

  返回值: > 0 成功
          < 0 失败
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

    // 首次执行时更新交采地址偏移
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
    if (i < 3) // 非法地址，设为交采地址默认值0
    {
        memset(pAddr, 0x00, 6);
    }

    return 6;
}

/*
  名  称: commfun_WriteACCommAddr
  功  能: 保存交采通讯地址
  输  入: pAddr 交采通讯地址，低字节在前，BCD格式
  输  出: 无

  返回值: > 0 成功
          < 0 失败
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

    // 首次执行时更新交采地址偏移
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
所属功能单元：
功能描述：    读取终端设备编号
输入参数：    无
输出参数：    无
函数返回值：  读取数据长度
*********************************************************************/
int commfun_ReadTmnSerialNum(char *pTmnSerialNum)
{
    //设备编号，终端出厂前的物理编号(SN),一般为数字
    if (ReadXPara(TMN_RUN_PARA, TMN_Serial_Num_ID, (char*)pTmnSerialNum, 0) < 0)
        memset(pTmnSerialNum, 0x0, 8);
    return 8;
}

/*********************************************************************
所属功能单元：
功能描述：    读取终端设备编号
输入参数：    无
输出参数：    无
函数返回值：  读取数据长度
*********************************************************************/
int commfun_WriteTmnSerialNum(char *pTmnSerialNum)
{
    //设备编号，终端出厂前的物理编号(SN),一般为数字
    WriteXPara(TMN_RUN_PARA, TMN_Serial_Num_ID, (char*)pTmnSerialNum, 0);
    return 8;
}

/*********************************************************************
所属功能单元：
功能描述：    获取APN的用户名和密码等相关信息
输入参数：    无
输出参数：    apn_user:用户名, apn_pwd:密码, withPostfix:后缀名,
              netType: 1-GPRS 2-CDMA
函数返回值：  读取数据长度
*********************************************************************/
int commfun_GetApnInfo(char *apn_user, char *apn_pwd, char netType, BOOL withPostfix)
{
    int     nRet;
    char    apnName[50];
    char    serialNum[25];  //终端编号
    char    tmpUser[32], tmpPwd[32], tmpBuf[100];
    int     i, tmpReturn = 0;       //函数返回值，返回1表示取默认用户名，0表示用户设置的用户名

    memset(apnName, 0x0, sizeof(apnName));
    memset(serialNum, 0x0, sizeof(serialNum));
    memset(tmpUser, 0x0, sizeof(tmpUser));
    memset(tmpPwd, 0x0, sizeof(tmpPwd));
    memset(tmpBuf, 0x0, sizeof(tmpBuf));
    /*
    VPN、APN用户名、密码应可通过按键在界面上设置和查询。
    APN用户名格式为：移动GPRS为A@fjep.fj，电信CDMA为A@fjep.vpdn.fj。
    当VPN为fjep.fj时，设备应能够根据模块类型和设备编码自动修改用户名，APN用户名为固定参数，不能清除或修改，也不随软件升级而变动；
    当VPN为其他名称时可手动设置用户名和密码。
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

    // 取用户名
    if (strcmp(apnName, "FJEP.FJ") == 0 || strcmp(apnName, "fjep.fj") == 0)         // 如果是福建特殊用户名
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
            // CDMA后缀名和GPRS不一样
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

        i = 0;  // 转化为小写
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

    // 获取密码
    memset(tmpBuf, 0x00, sizeof(tmpBuf));
    if (strcmp(apnName, "FJEP.FJ") == 0 || strcmp(apnName, "fjep.fj") == 0)         // 如果是福建特殊用户名
    {
        // APN初始密码格式为：终端编号的逆序。
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

#if DOUBLE_CHANNEL_EN == 1      // 双通道
//获取双通道信道使用的电台类型
//pRadioType        电台类型
//pFasterType       高速电台当前的调制类型
BOOL commfun_DBChRadioType(uint8 *pRadioType, uint8 *pFasterType)
{
    uint8 tmpTpye = 0;          //电台类型
    uint8 tmpFasterType = 0;    //如果是高速电台，高速电台当前的调制类型

    uint8 dbchlRadioType = 0;       // 普通电台还是高速电台的标识
    uint8 radio_code_id = 0;

    ReadXPara(TMN_PARA_EXTEND, DOUBLE_CHANNEL_RADIO_TYPE_ID, (char *)&dbchlRadioType, 0);
    if(dbchlRadioType == 1)         //Radio
        tmpTpye = DBCH_TYPE_RADIO;
    else if(dbchlRadioType == 2)    //WDASN
        tmpTpye = DBCH_TYPE_WDASN;
    else if (dbchlRadioType == DBCH_TYPE_FASTER)                           //FASTER_RADIO
    {
        tmpTpye = DBCH_TYPE_FASTER;
        //高速电台的话，需要查询当前调制类型
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
功能描述：判断终端是否运行在双通道模式下
输入参数：
-           无
输出参数：
-           无
函数返回值：
-           TRUE            双通道模式
-           FALSE           普通模式
内部处理流程：
-           1、读取当前通道类型，必须是“普通电台、WDASN、高速电台”中的一种，才可能是双通道模式
-           2、读取双通道使能标识，使能标识必须是有效，才是双通道模式
********************************************************************/
BOOL CheckIsDoubleChannel(void)
{
    uint8 cchType[2] = {0x0, 0x0};          // 当前信道类型
    uint8 blnExtChnlEn = 0;

    // 当前通道类型
    if (ReadXPara(TMN_RUN_PARA, Cch_Type_ID, (char *)cchType, TMN_ID) < 0)
        return FALSE;

    //if(cchType[0] != Radio_ID && cchType[0] != WDASN_Radio_ID && cchType[0] != FASTER_RADIO_ID)         //普通电台、WDASN、高速电台
    if(cchType[0] != 8 && cchType[0] != 11 && cchType[0] != FASTER_RADIO_ID)         //普通电台、WDASN、高速电台
        return FALSE;

    if(ReadXPara(TMN_PARA_EXTEND, DOUBLE_CHANNEL_EXT_EN, (char *)&blnExtChnlEn, 0) > 0 && blnExtChnlEn == 0x55)
        return TRUE;

    return FALSE;
}
#endif

//485口功能自适应
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

        if (ReadXPara(MTR_PARA_TYPE, Mt_Para_Com_ID, (char *)&ComId, i) < 0)            //端口号
            continue;

        if(ComId == PORT_PRTL_485_1)
            gMtrNum_485[PIPE_METER_485_1 - 1] ++;
        else if(ComId == PORT_PRTL_485_2)
            gMtrNum_485[PIPE_METER_485_2 - 1] ++;
        else
            continue;
    }

    // 根据端口上有没有配置485表决定端口功能,
    // 配置了就作抄表口,未配置作为维护口,不需要手动设置
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

    // 配置了就作抄表口,未配置作为维护口,不需要手动设置
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

/****************************************************************************
*模块编号：
*名称：
*功能：
*输入参数：
*输出参数：
*处理：
*返回：无
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
*模块编号：
*名称：
*功能：
*输入参数：
*输出参数：
*处理：
*返回：无
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
        else if(nchioce == 2)//一组用户类型
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
        else if(nchioce == 5)//一组用户类型区间
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
        else if(nchioce == 6)//一组用户地址区间
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
        else if(nchioce == 7)//一组配置序号区间
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

