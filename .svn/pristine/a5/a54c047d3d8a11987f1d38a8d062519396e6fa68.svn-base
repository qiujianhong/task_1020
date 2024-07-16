
#include "../include/CtsAll.h"

//#define LOG_MAX_SIZE  1024000         /*日志大小 字节数    512K*/
extern int RcdLock(int fd, int cmd, int type, off_t offset, int whence, off_t len);
#define writew_lock(fd, offset, len) \
            RcdLock(fd, F_SETLKW, F_WRLCK, offset, SEEK_SET, len)

#define un_lock(fd, offset, len) \
            RcdLock(fd, F_SETLK, F_UNLCK, offset, SEEK_SET, len)
//extern inline BOOL FileExists(const char *filepath);
//extern inline int ExtractFilePath(const char *src, char *path);
//extern inline BOOL PathExists(const char *pathname);
//extern BOOL makePathedDir(const char *path);
//extern inline ssize_t GetFileSize(const char *filename);
/***************************************************************************
*名称：将HEX格式转换为BCD格式
*描述：
*参数：HEX十六进制
*返回：BCD码
****************************************************************************/
uint8 HexToBcd(uint8 hex)
{
  uint8 xch;
  uint8 rv = 0;
  if(hex > 99) return 0x99;
  xch = hex % 10;
  rv += xch;
  xch = (hex / 10) % 10;
  xch <<= 4;
  rv += xch;
  return rv;
}

uint8 BcdToHex(uint8 bcd)
{
  uint8 rv;
  rv = ((bcd) >> 4) * 10 + (bcd & 0x0f);
  return rv;
}
/*******************************************************************************
* 函数名称: GetDistanceBinTimeByDay()
* 函数功能: 取得两个日期的差值
* 输入参数: DateTime_T dt1, 时间点1
            DateTime_T dt2  时间点2
            
* 输出参数: 无
* 返 回 值:  计算两个日期的差值dt2>dt1返回为正值，否则为负，单位为:天
*******************************************************************************/
int GetDistanceBinTimeByDay(DateTime_T dt1, DateTime_T dt2)
{
  

    time_t sec1 = DateTime_TToSec(dt1);
    time_t sec2 = DateTime_TToSec(dt2);

    time_t sec = sec2 - sec1;


    int dis = sec / MINS_IN_DAY;
    return dis;
}
/*******************************************************************************
* 函数名称: DateTime_TToSec()
* 函数功能: 取得格式对应的秒数
* 输入参数: DateTime_T PointTime
            时间
* 输出参数: 无
* 返 回 值:  系统秒数
*******************************************************************************/
time_t DateTime_TToSec(DateTime_T PointTime)
{

    struct tm *p = NULL;

    time_t timep;

    time(&timep);
    p = localtime(&timep);

    p->tm_year = PointTime.Year + 100;
    p->tm_mon = PointTime.Month - 1;
    p->tm_mday = PointTime.Day;
    p->tm_hour = PointTime.Hour;
    p->tm_min = PointTime.Minute;
    p->tm_sec = PointTime.Second;
    p->tm_wday = 0;

    time_t sec_t = FastDateToSecond(p);

    return sec_t;
}
/*******************************************************************************
* 函数名称: taskmng_cur_bin_time_get
* 函数功能: 获取终端当前时间，转换成BIN格式
* 输入参数: 无
* 输出参数: 无
* 返 回 值:  无
*******************************************************************************/
void taskmng_cur_bin_time_get(DateTime_T *pTime)
{
    time_t now;
    struct tm tNowtime;

    time(&now);
    LOCALTIME_R(&now, &tNowtime);

    pTime->Second   = (uint8)tNowtime.tm_sec;
    pTime->Minute   = (uint8)tNowtime.tm_min;
    pTime->Hour     = (uint8)tNowtime.tm_hour;
    pTime->Day      = (uint8)tNowtime.tm_mday;
    pTime->Month    = (uint8)tNowtime.tm_mon + 1;
    pTime->Year     = (uint8)(tNowtime.tm_year - 100);
}

/*******************************************************************************
* 函数名称: taskmng_cur_bcd_time_get
* 函数功能: 获取终端当前时间，转换成BCD格式
* 输入参数: 无
* 输出参数: 无
* 返 回 值:  无
*******************************************************************************/
void taskmng_cur_bcd_time_get(DateTime_T *pTime)
{
    time_t now;
    struct tm tNowtime;

    time(&now);
    LOCALTIME_R(&now, &tNowtime);

    pTime->Second   = HexToBcd((uint8)tNowtime.tm_sec);
    pTime->Minute   = HexToBcd((uint8)tNowtime.tm_min);
    pTime->Hour     = HexToBcd((uint8)tNowtime.tm_hour);
    pTime->Day      = HexToBcd((uint8)tNowtime.tm_mday);
    pTime->Month    = HexToBcd((uint8)tNowtime.tm_mon + 1);
    pTime->Year     = HexToBcd((uint8)(tNowtime.tm_year - 100));
}
/*******************************************************************************
* 函数名称: DateTimeSToChar
* 函数功能: 将OOP_DATETIME_S_T格式的时间转为7字节的char数组
* 输入参数: 无
* 输出参数: 无
* 返 回 值:  无
*******************************************************************************/
void DateTimeSToChar(OOP_DATETIME_S_T *pDataTime_s,char *pTime)
{
  
    pTime[0] = pDataTime_s->second;
    pTime[1] = pDataTime_s->minute;
    pTime[2] = pDataTime_s->hour;
    pTime[3] = pDataTime_s->day;
    pTime[4] = pDataTime_s->month;
    memcpy(&pTime[5],&pDataTime_s->year,2);
}

//void memcpy_r(uint8 *de,uint8 *sr,uint16 len)
//{
//    uint16 i = 0;
//    for(i=0; i<len; i++)
//    {
//        de[i] = sr[len-i-1];
//    }
//}
BOOL cmpOad(OOP_OAD_U sOad,OOP_OAD_U dOad)
{
    if(sOad.value==dOad.value)
    {
        return TRUE;
    }
        return FALSE;

}
void memrev(uint8* buff,uint16 len)
{
    uint16 i;
    char tmp;
    for(i=0; i<len/2; i++)
    {
        tmp = buff[i];
        buff[i] = buff[len-i-1];
        buff[len-i-1] = tmp;
    }
    return;
}
/***************************************************************************
*名称：将HEX格式转换为BCD格式
*描述：
*参数：HEX十六进制
*返回：BCD码
****************************************************************************/
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

/****************************************************************************
*名称：将BCD格式转换为HEX格式
*描述：
*参数：BCD码
*返回：十六进制
****************************************************************************/
int commfun_BcdToHex(char compress_bcd)
{
    int rv;

    rv = ((compress_bcd)>>4)*10+(compress_bcd&0x0f);
    return rv;
}

//将rtc时钟的格式转换为距离1970年1月1日0时0分0秒的秒数
//sec_cancle 是用来决定是否忽略秒的，如果是TRUE的话，则在转换的时候忽略秒
//原因是任务执行的时候，最小的上报单位是分，那么可以在检查任务参数的时候就确认任务是否已经到了需要执行的秒数，在真正上报的时候直接都按0秒开始上报就可以了
/**********在commfun_rtc2time的基础上把秒也计算进去*******/
//time_t commfun_rtc2time2(rtc_t src_rtc)
//{
//    /*
//
//    关于 time_t
//    而日历时间（Calendar Time）是通过time_t数据类型来表示的
//    用time_t表示的时间（日历时间）是从一个时间点（例如：1970年1月1日0时0分0秒）到此时的秒数。
//    在time.h中，我们也可以看到time_t是一个长整型数：
//
//    关于 tm
//    struct tm {
//    int tm_sec; // 秒 – 取值区间为[0,59]
//    int tm_min; // 分 - 取值区间为[0,59]
//    int tm_hour; // 时 - 取值区间为[0,23]
//    int tm_mday; // 一个月中的日期 - 取值区间为[1,31]
//    int tm_mon; // 月份（从一月开始，0代表一月） - 取值区间为[0,11]
//    int tm_year; // 年份，其值等于实际年份减去1900
//    int tm_wday; // 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推
//    int tm_yday; // 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推
//    int tm_isdst; // 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。
//    };
//    */
//    struct tm des_tm;       //标准的tm时钟结构
//    des_tm.tm_year = (commfun_BcdToHex(src_rtc.yy) > 70 ? commfun_BcdToHex(src_rtc.yy) : (commfun_BcdToHex(src_rtc.yy) + 100));
//    des_tm.tm_mon = commfun_BcdToHex(src_rtc.mm) - 1;
//    des_tm.tm_mday = commfun_BcdToHex(src_rtc.dd);
//    des_tm.tm_hour = commfun_BcdToHex(src_rtc.hh);
//    des_tm.tm_min = commfun_BcdToHex(src_rtc.nn);
//    des_tm.tm_sec = commfun_BcdToHex(src_rtc.ss);   //将秒数置为0
//    des_tm.tm_isdst = -1;           //忽略闰秒
//    return FastDateToSecond(&des_tm);     //从1970年1月1日0时0分0秒到此时的秒数
//}

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

BOOL DT_Time2DateTimeHex(time_t linuxTime,DateTimeHex_t *dateTimeHex)
{
    struct tm tm_Time;
    LOCALTIME_R(&linuxTime, &tm_Time);
    dateTimeHex->year_h = (tm_Time.tm_year+1900)>>8;
    dateTimeHex->year_l = (uint8)(tm_Time.tm_year+1900);
    dateTimeHex->month = tm_Time.tm_mon+1;
    dateTimeHex->day = tm_Time.tm_mday;
    dateTimeHex->hour = tm_Time.tm_hour;
    dateTimeHex->min = tm_Time.tm_min;
    dateTimeHex->sec = tm_Time.tm_sec;
    return TRUE;
}
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

BOOL DT_IsLeap(uint16 year)
{
    if(((year%4==0)&&(year%100!=0))||year%400==0)
        return TRUE;
    else
        return FALSE;
}

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

//BOOL DT_DateTimeBCD2Time(DateTimeBcd_t *dateTimeBCD,time_t *linuxTime)
//{
//    struct tm ptm ={0};
//    if(dateTimeBCD == NULL)
//        return FALSE;
//    if(FALSE == DT_DateTimeBCDIsValid(dateTimeBCD))
//    {
//         printf("DT_DateTimeBCD2Time\n");
//        printf("时间非法\n");
//        return FALSE;
//    }
//    //commfun_printstring((uint8*)dateTimeBCD,7);
//    uint16 year;
//    year = commfun_BcdToHex(dateTimeBCD->year_h);
//    year *= 100;
//    year += commfun_BcdToHex(dateTimeBCD->year_l);
//    ptm.tm_year = year-1900;
//    //ptm.tm_year = (commfun_BcdToHex(dateTimeBCD->year_l) >= 70 ? commfun_BcdToHex(dateTimeBCD->year_l) : (commfun_BcdToHex(dateTimeBCD->year_l) + 100));
//    ptm.tm_mon = commfun_BcdToHex(dateTimeBCD->month)-1;
//    ptm.tm_mday = commfun_BcdToHex(dateTimeBCD->day);
//    ptm.tm_hour = commfun_BcdToHex(dateTimeBCD->hour);
//    ptm.tm_min = commfun_BcdToHex(dateTimeBCD->min);
//    ptm.tm_sec = commfun_BcdToHex(dateTimeBCD->sec);
//
//    *linuxTime = FastDateToSecond(&ptm);
//    //printf("linuxTime=%d time = %d\n",*linuxTime,time(NULL));
//    return TRUE;
//}

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




/*******************************************************************************
* 函数名称: GetEnvValue
* 函数功能: 获取终端相应环境变量的数值
* 输入参数: const char *env_name, 环境变量的名称
            int default_value     不存在此环境变量时返回的默认值
* 输出参数: 无
* 返 回 值:  环境变量的对应值
*******************************************************************************/
int GetEnvValue(const char *env_name, int default_value)
{
    char * str_env = getenv(env_name);
    
    if(NULL == str_env)
    {
        return default_value;
    }

    return atoi(str_env);
    
}

/**********************************************************
* 函数名称：
        XADR_GetEleNum
* 功能描述：
        获取串类型数据元素长度
* 输入描述：
        indata:数据Buffer
* 返回值：
        bytes:编码的字节个数据
**********************************************************/
uint16 XADR_GetEleNum(const uint8 *indata,uint8 *bytes)
{
    uint16 EleNum;
    uint8 nbytes = 1;
    EleNum = indata[0];
    if(EleNum==0x81)
    {
        EleNum = indata[1];
        nbytes = 2;
    }
    else if(EleNum==0x82)
    {
        EleNum = indata[1];
        EleNum <<= 8;
        EleNum += indata[2];
        nbytes = 3;
    }
    else if(EleNum==0x80 || EleNum>0x82)
        return 0;
    if(bytes!=NULL)
        *bytes = nbytes;
    return EleNum;
}

/*********************************************************************
所属功能单元：通用
功能描述：检测缓冲区是否全为FF
输入参数：
输出参数：无
函数返回值:
            TRUE: 全为FF  FALSE:不都是FF
内部处理流程：
********************************************************************/
BOOL CheckFF(uint8 *buf, uint16 len)
{
    uint16 i = 0;

    for (i = 0; i < len; i++)
    {
        if (buf[i] != 0xff)
        {
            return FALSE;
        }
    }

    return TRUE;
}

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



//获取前daycnt天的零点时刻
time_t GetDaycntTime(uint8 DayCnt)
{
    time_t now = time(NULL);
    time_t rettime;
    struct tm *tmnow = localtime(&now);
    tmnow->tm_hour = 0;
    tmnow->tm_min=0;
    tmnow->tm_sec=0;
    rettime = FastDateToSecond(tmnow);
    rettime -=(DayCnt*86400);
    return rettime;

}
    
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
/****************************************************************************
*模块编号：                                                                  
*名称：                                                                      
*功能：                                                                      
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
****************************************************************************/
BOOL taskmng_oop_next_read_data_analysis(const uint8 *inData,uint16 *offset,uint16 inDataLen,enum tag_OOP_DATATYPE DataType,BOOL bWithType)
{
    uint16 dataLen;
    uint8 nchioce;
    uint8 dataNum;
    uint8 i;
    uint16 offSet=*offset;
    enum tag_OOP_DATATYPE dataType;
    
    if(bWithType)
    {   
        dataType = DataType;
    }
    else
    {   
        dataType = (enum eDatatype)inData[offSet++];
    }
        
    if(dataType == DT_NULL) 
    {   
        *offset = offSet;
        return TRUE;
    }
    if((dataType == DT_STRUCTURE)
        ||(dataType == DT_ARRAY)
        ||(dataType == DT_OCTET_STRING)
        ||(dataType == DT_VISIBLE_STRING)
        ||(dataType == DT_UTF8_STRING)
        ||(dataType == DT_BIT_STRING)
        ||(dataType == DT_MAC)
        ||(dataType == DT_RN)
        ||(dataType == DT_TSA))
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
        if((dataType != DT_ARRAY) && (dataType != DT_OCTET_STRING) && (dataType != DT_VISIBLE_STRING) \
            && (dataType != DT_MAC) &&(dataLen == 0)) 
        {      
            return FALSE;
        }
        
        if(dataType == DT_BIT_STRING)
        {      
            dataLen /= 8;
        }
        
        if((dataType == DT_STRUCTURE)
            ||(dataType == DT_ARRAY))
        {
            uint16 eleNum = dataLen;
            while(eleNum--)
            {
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
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
    else if(dataType == DT_RSD)
    {
        uint8 eleNum=0;
        nchioce = inData[offSet++];
        switch(nchioce)
        {
            case 0://
                break;
            case 1:
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_OAD,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                break;
            case 2:
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_OAD,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                break;  
            case 3:
                eleNum = inData[offSet++];
                for(i=0;i<eleNum;i++)
                {
                    if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_OAD,TRUE))
                        return FALSE;
                    if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                    if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                    if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                }
                break;
            case 4:
            case 5:
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
            case 6:
            case 7:
            case 8:
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_TI,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
            case 9:
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_UNSIGNED,TRUE))
                    return FALSE;
                break;
            case 10:
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_UNSIGNED,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
                
        }
    }
    else if(dataType == DT_CSD)
    {
        nchioce = inData[offSet++];
        if(nchioce == 0)
        {
            dataLen = getDataTypeLength(DT_OAD);
            offSet +=dataLen;
        }
        else if(nchioce == 1)
        {
            dataLen = getDataTypeLength(DT_OAD);
            offSet +=dataLen;
            dataNum = inData[offSet++];
            offSet +=dataLen*dataNum;
        }
        else 
            return FALSE;

    }
    else if(dataType == DT_ROAD)
    {
        dataLen = getDataTypeLength(DT_OAD);
        offSet +=dataLen;
        dataNum = inData[offSet++];
        offSet +=dataLen*dataNum;
        printf("ROAD LEN = %d\n",dataLen+1+dataLen*dataNum);
    }
    else if(dataType == DT_RCSD)
    {
        dataNum = inData[offSet++];
        for(i=0;i<dataNum;i++)
        {
            if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_CSD,TRUE))
                return FALSE;
        }
    }
    else if(dataType == DT_MS)
    {
        nchioce = inData[offSet++];
        if((nchioce == 0)||(nchioce == 1))
        {
        
        }
        else if(nchioce == 2)//一组用户类型
        {
            dataNum = inData[offSet++];
            dataLen = getDataTypeLength(DT_UNSIGNED);
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
            dataLen = getDataTypeLength(DT_LONG_UNSIGNED);
            offSet +=dataLen*dataNum;
        }
        else if(nchioce == 5)//一组用户类型区间
        {
            dataNum = inData[offSet++];
            
            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength(DT_UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=DT_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength(DT_UNSIGNED);
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
                if(inData[offSet++]!=DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet +=dataLen;
                if(inData[offSet++]!=DT_TSA)
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
                if(inData[offSet++]!=DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength(DT_LONG_UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength(DT_LONG_UNSIGNED);
                offSet +=dataLen;
            }
        }
        else
            return FALSE;

    }
    else if(dataType == DT_REGION)
    {
        if(inData[offSet++]>3)
            return FALSE;
        if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
            return FALSE;
        if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
            return FALSE;
    }
    else if(dataType == DT_SID)
    {
        if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_DOUBLE_LONG_UNSIGNED,TRUE))
            return FALSE;
        if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_OCTET_STRING,TRUE))
            return FALSE;   
    }
    else if(dataType == DT_MAC)
    {
        if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_SID,TRUE))
            return FALSE;
        if(FALSE == taskmng_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_MAC,TRUE))
            return FALSE;
    }
    else if(dataType == DT_TI)
    {
        if(inData[offSet++]>5)
            return FALSE;
        offSet += 2;
    }
    else
    {
        
        dataLen = getDataTypeLength(dataType);
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
/****************************************************************************
*模块编号：                                                                  
*名称：                                                                      
*功能：                                                                      
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
****************************************************************************/
BOOL taskmng_oop_next_read_data_get(const uint8 *inData,uint16 *offset,uint16 inDataLen,enum tag_OOP_DATATYPE DataType,BOOL bWithType)
{
    uint16 dataLen;
    uint8 nchioce;
    uint8 dataNum;
    uint8 i;
    uint16 offSet=*offset;
    enum tag_OOP_DATATYPE dataType;
    
    if(bWithType)
    {   
        dataType = DataType;
    }
    else
    {   
        dataType = (enum eDatatype)inData[offSet++];
    }
        
    if(dataType == DT_NULL) 
    {   
        *offset = offSet;
        return TRUE;
    }
    if((dataType == DT_STRUCTURE)
        ||(dataType == DT_ARRAY)
        ||(dataType == DT_OCTET_STRING)
        ||(dataType == DT_VISIBLE_STRING)
        ||(dataType == DT_UTF8_STRING)
        ||(dataType == DT_BIT_STRING)
        ||(dataType == DT_MAC)
        ||(dataType == DT_RN)
        ||(dataType == DT_TSA))
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
        if((dataType != DT_ARRAY) && (dataType != DT_OCTET_STRING) && (dataType != DT_VISIBLE_STRING) \
            && (dataType != DT_MAC) &&(dataLen == 0)) 
        {      
            return FALSE;
        }
        
        if(dataType == DT_BIT_STRING)
        {      
            dataLen /= 8;
        }
        
        if((dataType == DT_STRUCTURE)
            ||(dataType == DT_ARRAY))
        {

            if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
            {         
                return FALSE;
            }   
        }
        else
        {
            offSet+=dataLen;
        }
    }
    else if(dataType == DT_RSD)
    {
        uint8 eleNum=0;
        nchioce = inData[offSet++];
        switch(nchioce)
        {
            case 0://
                break;
            case 1:
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_OAD,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                break;
            case 2:
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_OAD,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                break;  
            case 3:
                eleNum = inData[offSet++];
                for(i=0;i<eleNum;i++)
                {
                    if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_OAD,TRUE))
                        return FALSE;
                    if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                    if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                    if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                }
                break;
            case 4:
            case 5:
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
            case 6:
            case 7:
            case 8:
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_TI,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
            case 9:
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_UNSIGNED,TRUE))
                    return FALSE;
                break;
            case 10:
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_UNSIGNED,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
                
        }
    }
    else if(dataType == DT_CSD)
    {
        nchioce = inData[offSet++];
        if(nchioce == 0)
        {
            dataLen = getDataTypeLength(DT_OAD);
            offSet +=dataLen;
        }
        else if(nchioce == 1)
        {
            dataLen = getDataTypeLength(DT_OAD);
            offSet +=dataLen;
            dataNum = inData[offSet++];
            offSet +=dataLen*dataNum;
        }
        else 
            return FALSE;

    }
    else if(dataType == DT_ROAD)
    {
        dataLen = getDataTypeLength(DT_OAD);
        offSet +=dataLen;
        dataNum = inData[offSet++];
        offSet +=dataLen*dataNum;
        printf("ROAD LEN = %d\n",dataLen+1+dataLen*dataNum);
    }
    else if(dataType == DT_RCSD)
    {
        dataNum = inData[offSet++];
        for(i=0;i<dataNum;i++)
        {
            if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_CSD,TRUE))
                return FALSE;
        }
    }
    else if(dataType == DT_MS)
    {
        nchioce = inData[offSet++];
        if((nchioce == 0)||(nchioce == 1))
        {
        
        }
        else if(nchioce == 2)//一组用户类型
        {
            dataNum = inData[offSet++];
            dataLen = getDataTypeLength(DT_UNSIGNED);
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
            dataLen = getDataTypeLength(DT_LONG_UNSIGNED);
            offSet +=dataLen*dataNum;
        }
        else if(nchioce == 5)//一组用户类型区间
        {
            dataNum = inData[offSet++];
            
            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength(DT_UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=DT_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength(DT_UNSIGNED);
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
                if(inData[offSet++]!=DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet +=dataLen;
                if(inData[offSet++]!=DT_TSA)
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
                if(inData[offSet++]!=DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength(DT_LONG_UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength(DT_LONG_UNSIGNED);
                offSet +=dataLen;
            }
        }
        else
            return FALSE;

    }
    else if(dataType == DT_REGION)
    {
        if(inData[offSet++]>3)
            return FALSE;
        if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
            return FALSE;
        if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_NULL,FALSE))
            return FALSE;
    }
    else if(dataType == DT_SID)
    {
        if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_DOUBLE_LONG_UNSIGNED,TRUE))
            return FALSE;
        if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_OCTET_STRING,TRUE))
            return FALSE;   
    }
    else if(dataType == DT_MAC)
    {
        if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_SID,TRUE))
            return FALSE;
        if(FALSE == taskmng_oop_next_read_data_get(inData,&offSet,inDataLen,DT_MAC,TRUE))
            return FALSE;
    }
    else if(dataType == DT_TI)
    {
        if(inData[offSet++]>5)
            return FALSE;
        offSet += 2;
    }
    else
    {
        
        dataLen = getDataTypeLength(dataType);
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
/*******************************************************************************
* 函数名称: CalCs
* 函数功能: 计算指定的缓冲区数据的字节累加和
* 输入参数: pBuf  源缓冲区数据
*           len   源缓冲区数据长度
* 输出参数: 无
* 返 回 值: 字节累加和
*******************************************************************************/
uint8  CalCs(uint8 *pBuf, uint16 len)
{
    uint8 cs = 0;
    uint16 i;    
    
    for(i = 0; i < len; i++)
        cs += *(pBuf + i);
        
    return cs;            
}

/*******************************************************************************
* 函数名称: AddGap
* 函数功能: 将缓冲区的每个字节数据 加 一个数
* 输入参数: pRecvBuf  接收缓冲区数据
*           offs      截止偏移
*           gapValue  每个字节数据欲加的数据
* 输出参数: pRecvBuf  加过相同字节数据的缓冲区数据
* 返 回 值: 无
*******************************************************************************/
void AddGap(uint8 *pRecvBuf, uint8 offs, uint8  gapValue)
{
    uint8 i;
    
    if(offs == 0) 
        return;
        
    for(i = 0; i < offs; i++)
    {
        *(pRecvBuf + i) += gapValue;
    }
}
/*-
***************************************************************************
*模块编号：
*名称：取系统时间
*功能：取系统时间
*输入参数：
*输出参数：rtc格式的系统时间
*处理：
*返回：
***************************************************************************
-*/
void InterTaskReadRTC(rtc_t *prtc)
{
    time_t the_time;
    struct tm *tm_time;
    
    time(&the_time);
    tm_time = localtime(&the_time);
    
//  printf("date: %d, %02d, %02d\n", tm_time->tm_year, tm_time->tm_mon+1, tm_time->tm_mday);
//  printf("time: %02d, %02d, %02d\n", tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
//  printf("week: %02x\n", tm_time->tm_wday);
    
    prtc->ss = HexToBcd(tm_time->tm_sec);
    prtc->nn = HexToBcd(tm_time->tm_min);
    prtc->hh = HexToBcd(tm_time->tm_hour);
    prtc->dd = HexToBcd(tm_time->tm_mday);
    prtc->mm = HexToBcd(tm_time->tm_mon+1);
    prtc->yy = HexToBcd((tm_time->tm_year)%100);
    prtc->ww = HexToBcd(tm_time->tm_wday);
    
}
/*******************************************************************************
* 函数名称: commfun_645_frame_get 
* 函数功能://找到合法的645回码报文
* 输入参数:
        buf 消息内容
        sync 一般0x68
        buf_len 报文总长度

* 输出参数: reallen 识别出的645报文长度
* 返 回 值: 指向645报文头的指针
*******************************************************************************/
char *commfun_645_frame_get(char *buf, uint8 sync, uint32 buf_len,uint16 *reallen)
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
            *reallen=head[9]+0x0C;
            return head;
        }
        head++;
    }
    return NULL;
}

/*找合法的报文68开头，主要指回码报文*/
char *commfun_memchr_cap(char *buf, uint8 sync, uint32 buf_len)
{
    int i;
    char *head;

    if ((buf == NULL)||(buf_len <= 9))
        return NULL;

    head = buf;
    for(i=0; i<buf_len-9; i++)
    {
        if((*head == sync)&&(*(head+9) == sync))
        {
            return head;
        }
        head++;
    }
    return NULL;
}
/**
*********************************************************************
* @brief       写日志
* @param[in]   filepath - 文件名称
               fmt      - 写文件内容
* @param[out]  
* @return      >= 0     - 成功读取后的数据项字节数
               < 0      - 失败 
*********************************************************************
*/
//int WriteCtsLog(uint8 pipe,const char *filepath, const char *fmt, ...)
//{
//  int         fd;
//  char        path[PATH_MAX_LEN + 1];
//  time_t      t;
//  struct tm   *ptm;
//  va_list     ap;
//  char pathsub[8100]; 
//    char    pathname[512];
//    char    *pfile = NULL;
//    memset(pathname,0x0,sizeof(pathname));
//    pfile = strstr(filepath,".log");
//
//    memcpy(pathname,filepath,(pfile-filepath));
//    sprintf(pathname,"%s%02d",pathname,pipe);
//    strcat(pathname,pfile);
//
//
//  if( !FileExists(pathname)) 
//  {
//      ExtractFilePath(pathname, path);
//      if( !PathExists(path)) 
//          makePathedDir(path);
//      if( (fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC)) < 0) //新建文件
//        return -1;
//      fchmod(fd, 0664);
//      close(fd);
//  }
//
//  if(GetFileSize(pathname)>LOG_MAX_SIZE)
//  {
//      memset(pathsub,0x0,sizeof(pathsub));
//      strcat(pathsub,pathname);
//      strcat(pathsub,".sub");
//      remove(pathsub);
//      rename(pathname,pathsub);
//
//  }       
//  if( (fd = open(pathname, O_CREAT |O_WRONLY | O_APPEND)) < 0) //将内容附加到文件末尾
//      return -1;
//  if( writew_lock(fd, 0, 0) < 0)
//  {
//    //perror("writew_lock");
//      close(fd);
//      return -1;
//  }
//  t = time(NULL);
//  ptm = localtime(&t);
//  /* 写入日期 */
//  sprintf(pathsub, "%4d-%02d-%02d %02d:%02d:%02d ", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
//  write(fd, pathsub, strlen(pathsub));  
//  /* 写入日志字符串 */  
//  va_start(ap, fmt);  
//  vsnprintf(pathsub, sizeof(pathsub), fmt, ap); /* pathsub再次回收利用，用来保存写入的字符串 */
//  va_end(ap);
//  write(fd, pathsub, strlen(pathsub));
//
//  /* 换行 */
//  write(fd, "\n", strlen("\n")); 
//  un_lock(fd, 0, 0); //Unlock
//  
//  close(fd);
//  return 0;
//}
void ComposeBuf(uint8 *buf, int len, uint8 *comm_log)
{
    int i;
    char *p;
  
    p = (char*)comm_log;

    if(len<=0)
    {
        return;
    }
    if (len < 1500)
    {
        for (i=0; i<len; i++)
        {   
            sprintf(p+3*i, "%02x ", buf[i]);   
        }
    }
    else
    {
        for (i=0; i<1500; i++)
        {   
            sprintf(p+3*i, "%02x ", buf[i]);
        }
    }

}
/**
*********************************************************************
* @brief       
* @param[in]   
               
* @param[out]  
* @return      
               
*********************************************************************
*/
uint16 get_meter_read_speed(uint8 baud)
{
    switch(baud)
    {
        case 0:
        return 300;
        case 1:
        return 600;
        case 2:
        return 1200;
        case 3:
        return 2400;
        case 4:
        return 4800;
        case 5:
        return 7200;
        case 6:
        return 9600;
        case 7:
        return 19200;
        case 8:
        return 38400;
        case 9:
        return 57600;
        default:
        return 2400;        
    }
}
void frame_printf(uint8* bufffer,uint32 len)
{
    uint32 i=0;
    while (i<len)
    {
        
        printf("%02X",bufffer[i]);
        printf(" ");
        i++;
    }
    printf("\n");
}

void WriteTimeLog(uint8 pipe,char* Time)
{
    TASK_FMT_TRACE(pipe,REC_LOG," %02x%02x/%02x/%02x  %02x:%02x:%02x \n",Time[6],Time[5],Time[4],Time[3],Time[2],Time[1],Time[0]);
}
void commfun_printstring(uint8 *buf, int16 len)
{
    uint16 i = 0;
    if(len <= 0)
        return;
    for(i = 0; i < len; i++)
        printf("%02x ", buf[i]);
    putchar('\n');
}

OOP_DATETIME_S_T Time_tToDataTimeS(time_t tm)
{
    OOP_DATETIME_S_T dateTimeS;
    struct tm tm_Time;
    LOCALTIME_R(&tm, &tm_Time);
    dateTimeS.year = tm_Time.tm_year + 1900;
    dateTimeS.month = tm_Time.tm_mon + 1;
    dateTimeS.day = tm_Time.tm_mday;
    dateTimeS.hour = tm_Time.tm_hour;
    dateTimeS.minute = tm_Time.tm_min;
    dateTimeS.second = tm_Time.tm_sec;
    return dateTimeS;
}

//透明转发一个字节的通讯字转化为OOP_COMDCB_T
OOP_COMDCB_T taskmng_com_dcb_get(uint8 comm)
{
    OOP_COMDCB_T comdcb;
    memset(&comdcb,0x00,sizeof(OOP_COMDCB_T));
    uint8 tmp;
    tmp = comm & 0x03 ;//bit0 bit1表示数据位
    if(tmp==0)
    {
        comdcb.databits =5;
    }else if(tmp==1)
    {
        comdcb.databits =6;
    }else if(tmp==2)
    {
        comdcb.databits =7;
    }else if(tmp==3)
    {
        comdcb.databits =8;
    }
    //bit3表示是否有校验
    tmp = (((comm&0x08)==0x08) ? 1:0);
    if(tmp==0)
    {
        comdcb.parity = 0; //无校验
    }
    else
    {
        //bit2表示奇偶校验
        tmp = (((comm&0x04)==0x04) ? 1:0);
        if(tmp == 0)
        {
            comdcb.parity = 1;//奇校验
        }else
        {
            comdcb.parity = 2;//偶校验
        }
    }
    //bit4停止位
    tmp = (((comm&0x10)==0x10) ? 1:0);
    if(tmp==0)
    {
        comdcb.stopbits = 1;
    }else
    {
        comdcb.stopbits = 2;
    }
    //bit5 6 7 波特率
    tmp = (comm>>5) & 0x07;
    comdcb.baud = tmp;
    return comdcb;
}


BOOL check_is_all_ch(uint8 *buf ,uint8 ch,uint16 len)
{
    uint16 i=0;
    
    for(i=0;i<len;i++)
    {
        if(buf[i] !=ch)
        {
            return FALSE;
        }
    }
    return TRUE;
}

void Pro_ReduceMeterGap(uint8 *pRecvBuf, uint8 offs, uint8 gapValue)
{
    uint8 i;

    if(offs == 0) 
        return;

    for(i = 0; i < offs; i++)
    {
        *(pRecvBuf + i) -= gapValue;
    }
}

//将DESCRIPTOR_T格式转换为描述符的字符串格式 type0 主节点 1 从节点
void MakeDescriptor(uint8 type,DESCRIPTOR_T Descriptor,char* buf)
{
    if(type==0)
    {
        sprintf(buf,"type=%s;mfrs=%c%c;idformat=2;id=%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x;mmfrs=%c%c;midformat=%d;mid=%s",
            Descriptor.type,Descriptor.mfrs[0],Descriptor.mfrs[1],Descriptor.id[0],Descriptor.id[1],Descriptor.id[2],Descriptor.id[3],Descriptor.id[4],Descriptor.id[5],Descriptor.id[6],Descriptor.id[7],
            Descriptor.id[8],Descriptor.id[9],Descriptor.id[10],Descriptor.id[11],Descriptor.id[12],Descriptor.id[13],Descriptor.id[14],Descriptor.id[15],Descriptor.id[16],Descriptor.id[17],Descriptor.id[18],
            Descriptor.id[19],Descriptor.id[20],Descriptor.id[21],Descriptor.id[22],Descriptor.id[23],Descriptor.mmfrs[0],Descriptor.mmfrs[1],Descriptor.midformat,Descriptor.mid);
    }else
    {
       sprintf(buf,"type=%d;mfrs=%c%c;idformat=2;id=%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x;mmfrs=%c%c;midformat=%d;mid=%s",
            Descriptor.type[0],Descriptor.mfrs[0],Descriptor.mfrs[1],Descriptor.id[0],Descriptor.id[1],Descriptor.id[2],Descriptor.id[3],Descriptor.id[4],Descriptor.id[5],Descriptor.id[6],Descriptor.id[7],
            Descriptor.id[8],Descriptor.id[9],Descriptor.id[10],Descriptor.id[11],Descriptor.id[12],Descriptor.id[13],Descriptor.id[14],Descriptor.id[15],Descriptor.id[16],Descriptor.id[17],Descriptor.id[18],
            Descriptor.id[19],Descriptor.id[20],Descriptor.id[21],Descriptor.id[22],Descriptor.id[23],Descriptor.mmfrs[0],Descriptor.mmfrs[1],Descriptor.midformat,Descriptor.mid);
     }


}
//将字符串转换为DESCRIPTOR_T type=value;mfrs=value;idformat=value;id=value;mmfrs=value;midformat=value;mid=value
//type 0 主 type1 从
DESCRIPTOR_T UnpackDescriptor(uint8 type,char* buf)
{
    DESCRIPTOR_T Descriptor;
    memset(&Descriptor,0x00,sizeof(DESCRIPTOR_T));
    
    char *p;
    char *str = buf;
    p = strsep(&str,";");
    while(p!=NULL)
    {
        //printf("%s \n",p);
        char *tmp;
        tmp = strsep(&p,"=");
        while (tmp!=NULL)
        {
            if(strcmp(tmp,"type")==0)
            {
                tmp = strsep(&p,"=");
                if(type == 0)
                    strcpy(Descriptor.type,tmp);
                else
                    Descriptor.type[0] = CharToNum(tmp[0]);
            }else if(strcmp(tmp,"mfrs")==0)
            {
                tmp = strsep(&p,"=");
                Descriptor.mfrs[0] = tmp[0];
                Descriptor.mfrs[1] = tmp[1];

            }else if(strcmp(tmp,"idformat")==0)
            {
                tmp = strsep(&p,"=");
                Descriptor.idformat = atoi(tmp);
    
            }else if(strcmp(tmp,"id")==0)
            {
                tmp = strsep(&p,"=");
                uint8 i;
                for(i=0;i<24;i++)
                {
                    Descriptor.id[i] = (CharToNum(tmp[i*2])<<4) |CharToNum(tmp[i*2+1]);
                }

            }else if(strcmp(tmp,"mmfrs")==0)
            {
                tmp = strsep(&p,"=");
                Descriptor.mmfrs[0] = tmp[0];
                Descriptor.mmfrs[1] = tmp[1];

            }else if(strcmp(tmp,"midformat")==0)
            {
                tmp = strsep(&p,"=");
                Descriptor.midformat = atoi(tmp);

            }else if(strcmp(tmp,"mid")==0)
            {
                tmp = strsep(&p,"=");
                strcpy(Descriptor.mid,tmp);

            }
            tmp = strsep(&p,"=");
        }
        p = strsep(&str,";");
    }
    return Descriptor;
}

//将字符格式的16进制数转换为数字
uint8 CharToNum(uint8 str)
{
    uint8 ret;
    if(str>='0' && str<='9')
    {
        ret = str - '0';
    }
    if(str>='a' && str<='z')
    {
        ret = str - 'a'+10;
    }
    if(str>='A' && str<='Z')
    {
        ret = str - 'A'+10;
    }
    return ret;
}

char* GetFileName(char* filepath,int8 pipe)
{

    if(pipe<0||pipe>=AMR_THREAD_NUM)
    {
        return filepath;
    }else
    {
        //char    *pfile = NULL;
        memset(pathname[pipe],0x0,512);
        //pfile = strstr(filepath,".log");

       // memcpy(pathname[pipe],filepath,(pfile-filepath));
       // sprintf(pathname[pipe],"%s%02d",pathname[pipe],pipe);
        sprintf(pathname[pipe],"%s%02d",filepath,pipe);
       // strcat(pathname[pipe],pfile);
        return pathname[pipe];
    }

}

void TimeT2RTC(time_t time,rtc_t *prtc)
{
    
    struct tm *tm_time;
    
    tm_time = localtime(&time);

    prtc->ss = HexToBcd(tm_time->tm_sec);
    prtc->nn = HexToBcd(tm_time->tm_min);
    prtc->hh = HexToBcd(tm_time->tm_hour);
    prtc->dd = HexToBcd(tm_time->tm_mday);
    prtc->mm = HexToBcd(tm_time->tm_mon+1);
    prtc->yy = HexToBcd((tm_time->tm_year)%100);
    prtc->ww = HexToBcd(tm_time->tm_wday);
    
}

/*******************************************************************************
* 函数名称: oop_pipe_get
* 函数功能: 根据载波并发的通道索引获取通道号5个并发 使用通道号 10 11 12 13 14 
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
uint8 oop_pipe_get(uint8 pipe,uint8 index)
{
    return (pipe+1)*10+index;
}

/*******************************************************************************
* 函数名称: Hex2Bcd
* 函数功能: 将单字节HEX格式数据转为为BCD码
* 输入参数: HexNum  需要转换的BCD码
* 输出参数: 无   
* 返 回 值: 转换后BCD码
*******************************************************************************/
uint8 Hex2Bcd(uint8 HexNum)
{
    uint8 BcdNum = 0;

    BcdNum = (HexNum / 10);
    BcdNum <<= 4;
    BcdNum += (HexNum % 10);

    return (BcdNum);
}

/*******************************************************************************
* 函数名称: Bcd2Hex
* 函数功能: 将单字节BCD数据转Hex格式数据
* 输入参数: BcdNum  需要转换的BCD码
* 输出参数: 无   
* 返 回 值: 转换后HEX码
*******************************************************************************/
uint8 Bcd2Hex(uint8 BcdNum)
{
    uint8 HexNum = 0;

    HexNum = ((BcdNum >> 4) & 0x0F) * 10 + (BcdNum & 0x0F);

    return (HexNum);
}

/*******************************************************************************
* 函数名称: CaculateWeekDay
* 函数功能: 从指定时间获得周信息
* 输入参数: DesTime    未计算好周的时间
* 输出参数: DesTime    计算好周的时间
* 返 回 值: 无
* 备    注: 把一月和二月看成是上一年的十三月和十四月，例：如果是2004-1-10则换算成：2003-13-10来代入公式计算。
*******************************************************************************/
void CaculateWeekDay(YYMMDDWWhhnnss_TYPE *DesTime)
{
    uint8   WeekDay = 0;                     // 周
    uint8   BinMon = 0, BinDay = 0;          // 月、日
    uint16  BinYear = 0;                     // 年

    // 将BCD码日期转换为二进制
    BinYear = Bcd2Hex(DesTime->YY) + 2000;
    BinMon = Bcd2Hex(DesTime->MM);
    BinDay = Bcd2Hex(DesTime->DD);

    // 把一月和二月看成是上一年的十三月和十四月
    if ((BinMon== 1) || (BinMon == 2))
    {
        BinMon += 12;
        BinYear--;        
    }

    // 按公式计算周(0~6:代表周一 ~ 周日)
    WeekDay = (BinDay + 2 * BinMon + 3 * (BinMon+ 1) / 5 + BinYear + BinYear / 4 - BinYear / 100 + BinYear / 400) % 7;

    // 转换周信息为(0~6:代表周日 ~ 周六)
    if (WeekDay == 7)
    {
        WeekDay = 0;
    }

    DesTime->WW = WeekDay;
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

//透明转发一个字节的通讯字转化为OOP_COMDCB_T
OOP_COMDCB_T taskmng_com_dcb_get_new(uint8* comm)
{
    OOP_COMDCB_T comdcb;
    memset(&comdcb,0x00,sizeof(OOP_COMDCB_T));
    comdcb.baud = comm[0];
    uint8 tmp;
    tmp = comm[1] & 0x03 ;//bit0 bit1表示数据位
    if(tmp==0)
    {
        comdcb.databits =5;
    }else if(tmp==1)
    {
        comdcb.databits =6;
    }else if(tmp==2)
    {
        comdcb.databits =7;
    }else if(tmp==3)
    {
        comdcb.databits =8;
    }

    //bit3-bit2（校验位）0-无校验；1-奇校验；2-偶校验。
    comdcb.parity = (comm[1]>>2)&0x03;

    //bit4停止位
    tmp = (((comm[1]&0x10)==0x10) ? 1:0);
    if(tmp==0)
    {
        comdcb.stopbits = 1;
    }else
    {
        comdcb.stopbits = 2;
    }
    //bit6-bit5（流控）：0-无；1-硬件；2-软件。
    comdcb.flowctrl = (comm[1]>>5)&0x03;
    return comdcb;
}

BOOL FrameCheck698(uint8 *srcbuf,uint16 srclen,uint8 *apdu,uint16 *apdulen)
{
    uint16 offset=0;
    uint16 Len=0;
//    OOP_TSA_T tsa;
//    uint16 tmpCrc;
//    uint16 cs=0;
    uint8 HeadAddrLen;
    uint16 Crc16=0, CrcL, CrcH;
    uint8 headlen=0;

    while(offset <= srclen)
    {
        /*1、帧头检验：0x68 */
        if(0x68 != srcbuf[offset])
        {
            offset++;
            continue;
        }

        /* 2、取帧数据域长度-长度为2个字节，有可能在缓存的首尾各一个字节*/
        Len = ((uint16)srcbuf[(offset+1)])|((uint16)srcbuf[(offset+2)<<8]);
        Len += 2;

        /* 3、再次判断长度 */
        if(Len > (srclen - offset))
        {

            offset++;
            continue;

        }

        /*4、检查帧尾16 */
        if(0x16 != srcbuf[offset + (Len-1)] )
        {
            offset++;
            continue;
        }


        HeadAddrLen = (srcbuf[offset+4] & 0x0F)+1;
        CrcL = (uint16)srcbuf[offset+6+HeadAddrLen];
        CrcH = (uint16)srcbuf[offset+7+HeadAddrLen];
        Crc16 = (CrcH << 8)|(CrcL & 0x00FF);
        if(Crc16 != CRC16_Get((const uint8*)&srcbuf[offset+1],5+HeadAddrLen))
        {
            offset++;
            continue;
        }

        CrcL = srcbuf[offset+Len-3];
        CrcH = srcbuf[offset+Len-2];
        Crc16 = ((CrcH) << 8)|((CrcL)& 0x00FF);
        if(Crc16 != CRC16_Get((const uint8*)&srcbuf[offset+1], Len-4))
        {
            offset++;
            continue;
        }
        break;
    }
    
    if(offset>srclen)
    {
        return FALSE;
    }
    headlen = HeadAddrLen+8;
    *apdulen = Len-headlen-3;
    memcpy(apdu,srcbuf+headlen+offset,*apdulen);
    return TRUE;
}

/****************************************************************************
*模块编号?
*名称：根据接收到的报文，判断帧的有效性
*功能：部颁规约电表判断返回帧的有效性
*输入参数：通道号（pipe）、抄表任务结构体(*pTaskAllot)、接收到电表应答的数据缓冲区指针(*pBuf)、报文长度(rcvLen)
*输出参数：无
*处理： 检测首字节是否等于同步头，即0x68；
*       检测第七个字节是否也等于同步头，即0x68；
*   检测最后一个字节是否等于结束码（根据报文内的报文长度，截取到最后一个字节），即0x16；
*   检测报文的校验码是否等于最后第二个字节；
*   检测报文中的表地址是否等于pTaskAllot.MetAddr[]；
*       上面属于校验报文是否成功部分。
*   根据控制码（报文第8字节），判断从站是否正常应答，是否有后续帧。
*   如果控制码D6位为1，表示从站异常应答；
*   如果控制码D6位为0，表示从站正常应答；
*   如果控制码D5位为0，表示无后续帧；
*   如果控制码D5位为1，表示有后续帧；
*返回： -0校验失败-
*       -1校验成功无后续数据帧-
*       -2校验成功有后续数据帧-
*       -3校验成功正常应答帧-
*       -4校验成功异常应答帧-
*       -5校验成功错误应答帧-
****************************************************************************/

int8 STD645_CheckRspFrame(uint8 *FrameBuff, uint32 FrameLen,uint8 *prtl)
{
    uint16  DataIndex = 0xff;                   //指针偏移
    uint8   CS = 0;
    uint16   i,DataLen;//start Common Problem
    uint8 CtrlCode;

    //滑动偏移，定位帧头
    for (i = 0; i < FrameLen; i++)
    {
        if ((FrameBuff[i] == 0x68) && (FrameBuff[i+7] == 0x68))
        {
            DataIndex = i;
            break;
        }
    }
    //未定位到帧头，判为失败
    if (DataIndex == 0xff)
    {
        return CHECK_RESULT_FAILUER;
    }
    //控制码
    CtrlCode = FrameBuff[DataIndex+8];
    //刷新数据长度
    DataLen = FrameBuff[DataIndex+9];

    //判断帧尾
    if (FrameBuff[(DataIndex + DataLen + 11)] != 0x16)
    {
        return CHECK_RESULT_FAILUER;
    }

    //判断校验和
    for (i=0; i < (DataLen + 10); i++)
    {
        CS += FrameBuff[DataIndex+i];
    }
    if (CS != FrameBuff[(DataIndex + DataLen + 10)])
    {
        return CHECK_RESULT_FAILUER;
    }
    if(CtrlCode>0x10)
    {
        *prtl= PRTL_07;
    }
    else
    {
        *prtl= PRTL_97;
    }
    return CHECK_RESULT_SUCCEED;
}




/****************************************************************************
*模块编号?
*名称：根据接收到的报文，判断帧的有效性
*功能：部颁规约电表判断返回帧的有效性
*输入参数：通道号（pipe）、抄表任务结构体(*pTaskAllot)、接收到电表应答的数据缓冲区指针(*pBuf)、报文长度(rcvLen)
*输出参数：无
*处理： 检测首字节是否等于同步头，即0x68；
*       检测第七个字节是否也等于同步头，即0x68；
*   检测最后一个字节是否等于结束码（根据报文内的报文长度，截取到最后一个字节），即0x16；
*   检测报文的校验码是否等于最后第二个字节；
*   检测报文中的表地址是否等于pTaskAllot.MetAddr[]；
*       上面属于校验报文是否成功部分。
*   根据控制码（报文第8字节），判断从站是否正常应答，是否有后续帧。
*   如果控制码D6位为1，表示从站异常应答；
*   如果控制码D6位为0，表示从站正常应答；
*   如果控制码D5位为0，表示无后续帧；
*   如果控制码D5位为1，表示有后续帧；
*返回： -0校验失败-
*       -1校验成功无后续数据帧-
*       -2校验成功有后续数据帧-
*       -3校验成功正常应答帧-
*       -4校验成功异常应答帧-
*       -5校验成功错误应答帧-
****************************************************************************/

uint8 STD07_CheckRspFrame(uint8 *pBuf, uint16 rcvLen)
{
    uint8   frameCs;
    uint16  frameLen;

    frameLen = 0x0c + pBuf[9];
    frameCs = CalCs(pBuf, frameLen - 2);

    if((pBuf[0] == 0x68)
            && (pBuf[7] == pBuf[0]) /*-北京系统电表地址补0或AA可变-*/
            && (pBuf[frameLen - 2] == frameCs)
            && (pBuf[frameLen - 1] == 0x16))
    {
        switch(frameLen)
        {
        case 0x0C:
            if((pBuf[8] == 0x8A) && (pBuf[9] == 0x00)) /*-写设备地址正常应答帧-*/
            {
                return 3;
            }
            else
            {
                return 0;
            }
        case 0x0D:
            if((pBuf[8] & 0xF0) == 0xD0)        /*-异常应答帧-*/
            {
                return 4;
            }
            else
            {
                if((pBuf[8] == 0x8C) && (pBuf[10] != 0xFF)) /*-更改波特率确认应答帧-*/
                {
                    return 3;
                }
                else
                {
                    if((pBuf[8] == 0x8C) && (pBuf[10] == 0xFF)) /*-更改波特率错误应答帧-*/
                    {
                        return 5;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }
        default:
            if(frameLen >= 0x0E)
            {
                if(0x90 == (pBuf[8] & 0xF0))        /*-无后续数据帧-*/
                {
                    return 1;
                }
                else
                {
                    if(0xB0 == (pBuf[8] & 0xF0))    /*-有后续数据帧-*/
                    {
                        return 2;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }
            else
            {
                return 0;
            }
        }
    }
    else
    {
        return 0;
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
uint16 MakeOopFrame(OOP_TSA_T *a,uint8*apdu,uint16 apduLen,uint8 *desbuf,uint8 ctr, uint8 ca)
{
    uint16 offset=0;
    
    uint16 headLen = 2+1+(1+(a->len+1)+1);

    uint16 frameLen = headLen+2+ apduLen+2;
    uint16 tmpCrc;
    
    //链路层数据
    desbuf[offset++] = 0x68;
    desbuf[offset++] = frameLen & 0x00FF;
    desbuf[offset++] = (frameLen & 0xFF00)>>8;
    desbuf[offset++] = ctr;
    desbuf[offset++] = a->af;

    memcpy_r(desbuf+offset,a->add,a->len+1);
    offset += a->len+1;
    
    desbuf[offset++] = ca;
    
    tmpCrc = CRC16_Get((uint8*)&desbuf[1] ,headLen);
    
    desbuf[offset++] =  tmpCrc & 0x00FF;
    desbuf[offset++] =  (tmpCrc & 0xFF00)>>8;


    memcpy(desbuf+offset,apdu,apduLen);
    offset += apduLen;
    
    tmpCrc = CRC16_Get((uint8*)&desbuf[1] ,frameLen-2);
    //链路层数据
    desbuf[offset++] =  tmpCrc & 0x00FF;
    desbuf[offset++] =  (tmpCrc & 0xFF00)>>8;
    desbuf[offset++] = 0x16;
    return offset;
}
/**
*********************************************************************
* @name：       DealRequestFrameFormat11
* @brief：      处理请求报文格式字11
* @param[in]   frametype        报文格式字
               frame          结构体中srcbuf 是要处理的报文
* @return    ：frame           结构体中desbuf 是处理完后的报文
* @author    : 高启翔
* @Date      ：2022-03-25
*********************************************************************
*/

BOOL DealRequestFrameFormat11(FRAME_CONVERT_T *frame)
{
    uint16 i=0;

    time_t linuxTime;      
    DateTimeHex_t    dateTimeHex = {0};
    DateTimeBcd_t dateTimeBCD;
    uint8 cs=0;
    uint16 adjusttime;
    uint8 framelen=0;
    uint8 framebuf[255]={0};
    uint16 index = 0;
    frame->dstLen = 0;
    uint8 num = 0;
    while(index < frame->srcLen)
    {
        cs = 0;
        framelen = frame->srcBuf[index++];
        //645报文
        memset(framebuf,0x00,sizeof(framebuf));
        memcpy(framebuf,&frame->srcBuf[index],framelen);
        index+= framelen;
        if(STD645_CheckRspFrame(framebuf,framelen,&frame->prtl) !=0)
        {
            return FALSE;
        }
        memcpy_r(&adjusttime,&frame->srcBuf[index],2);
        index+=2;
        
        linuxTime = time(NULL);
        switch((adjusttime>>11)&0x07)
        {
            case 0:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF);
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF);
                }
            }
            break;
            case 1:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*60;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*60;
                }
            }
            break;
            case 2:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*3600;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*3600;
                }
            }
            break;
            case 3:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*86400;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*86400;
                }
            }
            break;
            case 4:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)/100;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)/100;
                }
            }
            break;
            default:
            break;

        }
        if(adjusttime&0x4000)
        {
            linuxTime =linuxTime+frame->pipedelay/100;
        }
        DT_Time2DateTimeHex(linuxTime,&dateTimeHex);
           
        DT_DateTimeHex2BCD(&dateTimeHex,&dateTimeBCD);
        //到645报文数据域最后往前筛5个字节 找到0xcc替换
        if(framebuf[framelen-1-2] == 0xcc)
        {
            framebuf[framelen-1-2] = dateTimeBCD.year_l+0x33;
        }
        if(framebuf[framelen-1-3] == 0xcc)
        {
            framebuf[framelen-1-3] = dateTimeBCD.month+0x33;
        }
        if(framebuf[framelen-1-4] == 0xcc)
        {
            framebuf[framelen-1-4] = dateTimeBCD.day+0x33;
        }
        if(framebuf[framelen-1-5] == 0xcc)
        {
            framebuf[framelen-1-5] = dateTimeBCD.hour+0x33;
        }
        if(framebuf[framelen-1-6] == 0xcc)
        {
            framebuf[framelen-1-6] = dateTimeBCD.min+0x33;
        }

        for(i=0;i<framelen-2;i++)
        {
            cs+=framebuf[i];
        }
        framebuf[i++]=cs;
        framebuf[i++]=0x16;
        
        memcpy(frame->dstBuf+frame->dstLen,framebuf,framelen);
        frame->dstLen += framelen;
        frame->datalen645[num] = framelen;
        num++;
        if(num >= 20)
            break;
    }
    frame->num645 = num;

    return TRUE;
}
/**
*********************************************************************
* @name：       DealRequestFrameFormat12
* @brief：      处理请求报文格式字12
* @param[in]   frametype        报文格式字
               
               frame          结构体中srcbuf 是要处理的报文
* @return    ：frame           结构体中desbuf 是处理完后的报文
* @author    : 高启翔
* @Date      ：2022-03-25
*********************************************************************
*/

BOOL DealRequestFrameFormat12(FRAME_CONVERT_T *frame)
{
    uint16 i=0;
    
    time_t linuxTime;      
    DateTimeHex_t    dateTimeHex = {0};
    DateTimeBcd_t dateTimeBCD;
    uint8 cs=0;
    uint16 adjusttime;
    uint8 framelen=0;
    uint8 framebuf[255]={0};
    uint16 index = 0;
    frame->dstLen = 0;
    uint8 num = 0;
    while(index < frame->srcLen)
    {
        cs = 0;
        framelen = frame->srcBuf[index++];
        //645报文
        memset(framebuf,0x00,sizeof(framebuf));
        memcpy(framebuf,&frame->srcBuf[index],framelen);
        index+=framelen;
        if(STD645_CheckRspFrame(framebuf,framelen,&frame->prtl) !=0)
        {
            return FALSE;
        }
        
        memcpy_r((uint8*)&adjusttime,(uint8*)&frame->srcBuf[index],2);
        index+=2;
        
        linuxTime = time(NULL);
        switch((adjusttime>>11)&0x07)
        {
            case 0:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF);
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF);
                }
            }
            break;
            case 1:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*60;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*60;
                }
            }
            break;
            case 2:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*3600;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*3600;
                }
            }
            break;
            case 3:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*86400;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*86400;
                }
            }
            break;
            case 4:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)/100;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)/100;
                }
            }
            break;
            default:
            break;
    
        }
        if(adjusttime&0x4000)
        {
            linuxTime =linuxTime+frame->pipedelay/100;
        }
        DT_Time2DateTimeHex(linuxTime,&dateTimeHex);
           
        DT_DateTimeHex2BCD(&dateTimeHex,&dateTimeBCD);
    
         if(framebuf[framelen-1-2] == 0xcc)
         {
             framebuf[framelen-1-2] = dateTimeBCD.year_l+0x33;
         }
         if(framebuf[framelen-1-3] == 0xcc)
         {
             framebuf[framelen-1-3] = dateTimeBCD.month+0x33;
         }
         if(framebuf[framelen-1-4] == 0xcc)
         {
             framebuf[framelen-1-4] = dateTimeBCD.day+0x33;
         }
         if(framebuf[framelen-1-5] == 0xcc)
         {
             framebuf[framelen-1-5] = dateTimeBCD.hour+0x33;
         }
         if(framebuf[framelen-1-6] == 0xcc)
         {
             framebuf[framelen-1-6] = dateTimeBCD.min+0x33;
         }
         if(framebuf[framelen-1-7] == 0xcc)
         {
             framebuf[framelen-1-7] = dateTimeBCD.sec+0x33;
         }
    
        for(i=0;i<framelen-2;i++)
        {
            cs+=framebuf[i];
        }
        framebuf[i++]=cs;
        framebuf[i++]=0x16;
        memcpy(frame->dstBuf,framebuf,framelen);
        frame->dstLen = framelen;
        frame->datalen645[num] = framelen;
        num++;
        if(num >= 20)
            break;
    }
    frame->num645 = num;
    return TRUE;
}

/**
*********************************************************************
* @name：       DealRequestFrameFormat13
* @brief：      处理请求报文格式字13
* @param[in]   frametype        报文格式字
               tsa         表地址
               frame          结构体中srcbuf 是要处理的报文
* @return    ：frame           结构体中desbuf 是处理完后的报文
* @author    : 高启翔
* @Date      ：2022-03-25
*********************************************************************
*/

BOOL DealRequestFrameFormat13(FRAME_CONVERT_T *frame,OOP_TSA_T *tsa)
{
    uint16 i=0;
    uint8 tmpbuf[256];
    uint16 offset=0;
    uint8 cs=0;
    uint8 datalen=0;
    uint16 inIndex = 0;
    frame->dstLen = 0;
    uint8 num = 0;
    while(inIndex < frame->srcLen)
    {
        memset(tmpbuf,0x00,sizeof(tmpbuf));
        offset = 0;
        cs = 0;
        tmpbuf[offset++]=0x68;
        memcpy_r(&tmpbuf[offset],tsa->add,6);
        offset = offset+6;
        tmpbuf[offset++]=0x68;
        tmpbuf[offset++]=frame->srcBuf[inIndex++];
        if(frame->srcBuf[0]>0x10)
        {
            frame->prtl = PRTL_07;
        }
        else
        {
            frame->prtl = PRTL_97;
        }
        datalen = frame->srcBuf[inIndex++];
        tmpbuf[offset++] = datalen;
        memcpy(&tmpbuf[offset],&frame->srcBuf[inIndex],datalen);
        inIndex+=datalen;
        offset+=datalen;

        for(i=0;i<offset;i++)
        {
            cs+=tmpbuf[i];
        }
        tmpbuf[i++]=cs;
        tmpbuf[i++]=0x16;
        
        memcpy(frame->dstBuf + frame->dstLen,tmpbuf,i);
        frame->dstLen +=i;
        frame->datalen645[num] = i;
        num++;
        if(num >= 20)
            break;

    }
    frame->num645 = num;

    return TRUE;
}
/**
*********************************************************************
* @name：       DealRequestFrameFormat14
* @brief：      处理请求报文格式字14
* @param[in]   frametype        报文格式字
               tsa         表地址
               frame          结构体中srcbuf 是要处理的报文
* @return    ：frame           结构体中desbuf 是处理完后的报文
* @author    : 高启翔
* @Date      ：2022-03-25
*********************************************************************
*/

BOOL DealRequestFrameFormat14(FRAME_CONVERT_T *frame,OOP_TSA_T *tsa)
{
    uint16 i=0;

    time_t linuxTime;      
    DateTimeHex_t    dateTimeHex = {0};
    DateTimeBcd_t dateTimeBCD;
    uint8 cs=0;
    uint8 tmpbuf[256];
    uint16 offset=0;
    uint16 adjusttime;
    uint8 datalen=0;
    uint8 databuf[255]={0};
    uint16 index = 0;
    uint8 ctrl;
    uint8 num=0;
    frame->dstLen = 0;
    while(index < frame->srcLen)
    {
        cs = 0;
        ctrl = frame->srcBuf[index++];
        datalen = frame->srcBuf[index++];
        memset(databuf,0x00,sizeof(databuf));
        memcpy(databuf,&frame->srcBuf[index],datalen);
        index+=datalen;
        
        if(ctrl>0x10)
        {
            frame->prtl = PRTL_07;
        }
        else
        {
            frame->prtl = PRTL_97;
        }
        memcpy_r((uint8*)&adjusttime,(uint8*)&frame->srcBuf[index],2);
        index+=2;
        linuxTime = time(NULL);
        switch((adjusttime>>11)&0x07)
        {
            case 0:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF);
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF);
                }
            }
            break;
            case 1:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*60;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*60;
                }
            }
            break;
            case 2:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*3600;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*3600;
                }
            }
            break;
            case 3:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*86400;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*86400;
                }
            }
            break;
            case 4:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)/100;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)/100;
                }
            }
            break;
            default:
            break;
    
        }
        if(adjusttime&0x4000)
        {
            linuxTime =linuxTime+frame->pipedelay/100;
        }
        DT_Time2DateTimeHex(linuxTime,&dateTimeHex);
           
        DT_DateTimeHex2BCD(&dateTimeHex,&dateTimeBCD);
    
        if(databuf[datalen-1] == 0xcc)
        {
            databuf[datalen-1] = dateTimeBCD.year_l+0x33;
        }
        if(databuf[datalen-2] == 0xcc)
        {
            databuf[datalen-2] = dateTimeBCD.month+0x33;
        }
        if(databuf[datalen-3] == 0xcc)
        {
            databuf[datalen-3] = dateTimeBCD.day+0x33;
        }
        if(databuf[datalen-4] == 0xcc)
        {
            databuf[datalen-4] = dateTimeBCD.hour+0x33;
        }
        if(databuf[datalen-5] == 0xcc)
        {
            databuf[datalen-5] = dateTimeBCD.min+0x33;
        }
        memset(tmpbuf,0x00,sizeof(tmpbuf));
        offset = 0;
        tmpbuf[offset++]=0x68;
        memcpy_r(&tmpbuf[offset],tsa->add,6);
        offset = offset+6;
        tmpbuf[offset++]=0x68;
        tmpbuf[offset++]= ctrl;
        tmpbuf[offset++]= datalen;
        memcpy(&tmpbuf[offset],databuf,datalen);
        offset +=datalen;
        for(i=0;i<offset;i++)
        {
            cs+=tmpbuf[i];
        }
        tmpbuf[i++]=cs;
        tmpbuf[i++]=0x16;
        
        memcpy(frame->dstBuf+frame->dstLen,tmpbuf,i);
        frame->dstLen += i;

        frame->datalen645[num] = i;
        num++;
        if(num >= 20)
            break;
    }
    frame->num645 = num;
    return TRUE;
}
/**
*********************************************************************
* @name：       DealRequestFrameFormat15
* @brief：      处理请求报文格式字15
* @param[in]   frametype        报文格式字
               tsa         表地址
               frame          结构体中srcbuf 是要处理的报文
* @return    ：frame           结构体中desbuf 是处理完后的报文
* @author    : 高启翔
* @Date      ：2022-03-25
*********************************************************************
*/

BOOL DealRequestFrameFormat15(FRAME_CONVERT_T *frame,OOP_TSA_T *tsa)
{
    uint16 i=0;

    time_t linuxTime;      
    DateTimeHex_t    dateTimeHex = {0};
    DateTimeBcd_t dateTimeBCD;
    uint8 cs=0;
    uint8 tmpbuf[256];
    uint16 offset=0;
    uint16 adjusttime;
    uint8 datalen=0;
    uint8 databuf[255]={0};
    uint16 index = 0;
    uint8 ctrl;
    frame->dstLen = 0;
    uint8 num = 0;
    while(index < frame->srcLen)
    {
        cs = 0;
        ctrl = frame->srcBuf[index++];
        datalen = frame->srcBuf[index++];
        
        memcpy(databuf,&frame->srcBuf[index],datalen);
        index+=datalen;
        
        if(ctrl>0x10)
        {
            frame->prtl = PRTL_07;
        }
        else
        {
            frame->prtl = PRTL_97;
        }
        
        memcpy_r(&adjusttime,(uint8*)&frame->srcBuf[index],2);
        index+=2;
        linuxTime = time(NULL);
        switch((adjusttime>>11)&0x07)
        {
            case 0:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF);
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF);
                }
            }
            break;
            case 1:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*60;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*60;
                }
            }
            break;
            case 2:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*3600;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*3600;
                }
            }
            break;
            case 3:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*86400;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*86400;
                }
            }
            break;
            case 4:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)/100;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)/100;
                }
            }
            break;
            default:
            break;
    
        }
        
        DT_Time2DateTimeHex(linuxTime,&dateTimeHex);
           
        DT_DateTimeHex2BCD(&dateTimeHex,&dateTimeBCD);
        
        if(databuf[datalen-1] == 0xcc)
        {
            databuf[datalen-1] = dateTimeBCD.year_l+0x33;
        }
        if(databuf[datalen-2] == 0xcc)
        {
            databuf[datalen-2] = dateTimeBCD.month+0x33;
        }
        if(databuf[datalen-3] == 0xcc)
        {
            databuf[datalen-3] = dateTimeBCD.day+0x33;
        }
        if(databuf[datalen-4] == 0xcc)
        {
            databuf[datalen-4] = dateTimeBCD.hour+0x33;
        }
        if(databuf[datalen-5] == 0xcc)
        {
            databuf[datalen-5] = dateTimeBCD.min+0x33;
        }
        if(databuf[datalen-6] == 0xcc)
        {
            databuf[datalen-6] = dateTimeBCD.sec+0x33;
        }
        memset(tmpbuf,0x00,sizeof(tmpbuf));
        offset = 0;
        tmpbuf[offset++]=0x68;
        memcpy_r(&tmpbuf[offset],tsa->add,6);
        offset = offset+6;
        tmpbuf[offset++]=0x68;
        tmpbuf[offset++]= ctrl;
        tmpbuf[offset++]= datalen;
        memcpy(&tmpbuf[offset],databuf,datalen);
        offset +=datalen;
        for(i=0;i<offset;i++)
        {
            cs+=tmpbuf[i];
        }
        tmpbuf[i++]=cs;
        tmpbuf[i++]=0x16;

        memcpy(frame->dstBuf+frame->dstLen,tmpbuf,i);
        frame->dstLen += i;
        
        frame->datalen645[num] = i;
        num++;
        if(num >= 20)
            break;
    }
    frame->num645 = num;
    return TRUE;
}

/**
*********************************************************************
* @name：       DealRequestFrameFormat31
* @brief：      处理请求报文格式字11
* @param[in]   frametype        报文格式字
               frame          结构体中srcbuf 是要处理的报文
* @return    ：frame           结构体中desbuf 是处理完后的报文
* @author    : 高启翔
* @Date      ：2022-03-25
*********************************************************************
*/

BOOL DealRequestFrameFormat31(FRAME_CONVERT_T *frame)
{
    uint16 i=0;

    uint16 offset=3;
    uint16 tmpoffset=0;
//    uint8 framelen=0;
    uint8 ctr;
    uint8 addrlen=0;
    uint16 datalen;
    uint16 totallen=0;
    uint8 cs=0;

    ctr = frame->srcBuf[tmpoffset++];

    frame->dstBuf[offset++] =ctr;
    memcpy((uint8*)&frame->dstBuf[offset],frame->srcBuf+tmpoffset,6);
    offset =offset +6;
    tmpoffset = tmpoffset+6;
    addrlen = frame->srcBuf[tmpoffset++];
    memcpy((uint8*)&frame->dstBuf[offset],frame->srcBuf+tmpoffset,addrlen);
    offset =offset +addrlen;
    tmpoffset = tmpoffset+addrlen;
    //AFN\FN
    memcpy((uint8*)&frame->dstBuf[offset],frame->srcBuf+tmpoffset,3);
    offset =offset +3;
    tmpoffset = tmpoffset+3;
    //数据长度
    memcpy_r((uint8*)&datalen,frame->srcBuf+tmpoffset,2);
    tmpoffset = tmpoffset+2;

    memcpy((uint8*)&frame->dstBuf[offset],frame->srcBuf+tmpoffset,datalen);
    tmpoffset = tmpoffset+datalen;
    offset = offset+datalen;
    for(i=3;i<offset;i++)
        cs+=frame->dstBuf[i];
    totallen =offset+2;
    
    frame->dstBuf[0] = 0x68;
    frame->dstBuf[1] = totallen&0x00FF;
    frame->dstBuf[2] = totallen>>8;
    
    frame->dstBuf[offset++] = cs;
    frame->dstBuf[offset++] = 0X16;
    frame->dstLen =offset;

    frame->prtl = PRTL_3762;
    return TRUE;
}

/**
*********************************************************************
* @name：       DealRequestFrameFormat51
* @brief：      处理请求报文格式字
* @param[in]   frametype        报文格式字
               tsa         表地址
               frame          结构体中srcbuf 是要处理的报文
* @return    ：frame           结构体中desbuf 是处理完后的报文
* @author    : 高启翔
* @Date      ：2022-03-25
*********************************************************************
*/


BOOL DealRequestFrameFormat51(FRAME_CONVERT_T *frame,OOP_TSA_T *tsa)
{
    uint8 ctr;
    uint8 ca;
    uint16 offset=0;
    uint16 apdulen=0;

    ctr= frame->srcBuf[offset++];
    ca = frame->srcBuf[offset++];
    memcpy_r(&apdulen,&frame->srcBuf[offset],2);
    //apdulen =bin2_int16u(frame->srcBuf+offset);
    offset=offset+2;

    frame->prtl = PRTL_OOP;
    frame->dstLen=MakeOopFrame(tsa,frame->srcBuf+offset,apdulen,frame->dstBuf,ctr,ca);
    return TRUE;
}
/**
*********************************************************************
* @name：       DealRequestFrameFormat52
* @brief：      处理请求报文格式字
* @param[in]   frametype        报文格式字
               tsa         表地址
               frame          结构体中srcbuf 是要处理的报文
* @return    ：frame           结构体中desbuf 是处理完后的报文
* @author    : 高启翔
* @Date      ：2022-03-25
*********************************************************************
*/

BOOL DealRequestFrameFormat52(uint8 pipe,FRAME_CONVERT_T *frame,OOP_TSA_T *tsa)
{
    uint8 ctr;
    uint8 ca;
    uint16 offset=0;
    uint16 apdulen=0;
    uint16 i=0;
    uint16 j=0;
    uint16 adjusttime=0;
    time_t linuxTime;
    DateTimeHex_t    dateTimeHex = {0};
    uint8 Rand[16];
    uint8 apdu[1000]={0};
    ctr= frame->srcBuf[offset++];
    ca = frame->srcBuf[offset++];
    memcpy_r(&apdulen,&frame->srcBuf[offset],2);
    offset=offset+2;
    memcpy(apdu,&frame->srcBuf[offset],apdulen);
    memcpy_r(&adjusttime,frame->srcBuf+offset+apdulen,2);
    //698里面可能有多个时间修正值
    uint16 indexlist[10]={0};
    uint8 indexnum = 0;
    //时间格式8个字节
    for(i=0;i<apdulen-7;i++)
    {
        if((apdu[i]==0x1C)&&(apdu[i+1]==0x99)&&(apdu[i+2]==0x99))
        {
            indexlist[indexnum] = i;
            indexnum++;
        }
    }
    //找到需要修正的时间就修正 如果没找到 再看下有没有随机数
    if(indexnum > 0)
    {
        linuxTime = time(NULL);
        switch((adjusttime>>11)&0x07)
        {
            case 0:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF);
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF);
                }
            }
            break;
            case 1:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*60;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*60;
                }
            }
            break;
            case 2:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*3600;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*3600;
                }
            }
            break;
            case 3:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*86400;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*86400;
                }
            }
            break;
            case 4:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)/100;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)/100;
                }
            }
            break;
            default:
            break;

        }
        if(adjusttime&0x4000)
        {
            linuxTime =linuxTime+frame->pipedelay/100;
        }
        DT_Time2DateTimeHex(linuxTime,&dateTimeHex);
        uint8* tmp = (uint8*)&dateTimeHex;
        uint8 k;
        for(j=0;j<indexnum;j++)
        {
            apdu[indexlist[j]] = 0x1C;
            for(k=0;k<7;k++)
            {
                if(apdu[indexlist[j]+1+k] == 0x99)
                {
                    apdu[indexlist[j]+1+k] = *(tmp+k);
                }
            }
        }

    }
 
    
    for(j=0;j<apdulen-17;j++)
    {
        if((apdu[j]==0x01)&&(apdu[j+1]==0x10)&&(check_is_all_ch(apdu+j+2, 0x99,16)))
        {
            break;
        }
    }
    if(j < apdulen-17)
    {
        Get_ReadMtrRand(pipe,Rand);
        memcpy(apdu+j+2,Rand,16);
    }

    frame->prtl = PRTL_OOP;
    frame->dstLen=MakeOopFrame(tsa,apdu,apdulen,frame->dstBuf,ctr,ca);
    return TRUE;
}

/**
*********************************************************************
* @name：       DealRequestFrameFormat53
* @brief：      处理请求报文格式字
* @param[in]   frametype        报文格式字
               
               frame          结构体中srcbuf 是要处理的报文
* @return    ：frame           结构体中desbuf 是处理完后的报文
* @author    : 高启翔
* @Date      ：2022-03-25
*********************************************************************
*/

BOOL DealRequestFrameFormat53(uint8 pipe,FRAME_CONVERT_T *frame)
{
    uint8 ctr;
    uint8 ca;
    uint16 offset=0;
    uint16 apdulen=0;
    uint16 i=0;
    uint16 j=0;
    uint16 adjusttime=0;;
    time_t linuxTime;
    DateTimeHex_t    dateTimeHex = {0};
    OOP_TSA_T tsa;
    uint8 Rand[16];
    uint8 apdu[1000]={0};
    ctr= frame->srcBuf[offset++];
    tsa.af = frame->srcBuf[offset++];
    memcpy(tsa.add,frame->srcBuf+offset,tsa.len+1);
    offset =offset + tsa.len+1;
    ca = frame->srcBuf[offset++];
    memcpy_r(&apdulen,&frame->srcBuf[offset],2);
    //apdulen =bin2_int16u(frame->srcBuf+offset);
    offset=offset+2;
    memcpy(apdu,&frame->srcBuf[offset],apdulen);
    
    memcpy_r((uint8*)&adjusttime,frame->srcBuf+offset+apdulen,2);

    //698里面可能有多个时间修正值
    uint16 indexlist[10]={0};
    uint8 indexnum = 0;
    //时间格式8个字节
    for(i=0;i<apdulen-7;i++)
    {
        if((apdu[i]==0x1C)&&(apdu[i+1]==0x99)&&(apdu[i+2]==0x99))
        {
            indexlist[indexnum] = i;
            indexnum++;
        }
    }
    //找到需要修正的时间就修正 如果没找到 再看下有没有随机数
    if(indexnum > 0)
    {
        linuxTime = time(NULL);
        
        switch((adjusttime>>11)&0x07)
        {
            case 0:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF);
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF);
                }
            }
            break;
            case 1:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*60;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*60;
                }
            }
            break;
            case 2:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*3600;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*3600;
                }
            }
            break;
            case 3:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)*86400;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)*86400;
                }
            }
            break;
            case 4:
            {
                if(adjusttime&0x8000)
                {
                    linuxTime =linuxTime-(adjusttime&0x03FF)/100;
                }
                else
                {
                     linuxTime =linuxTime+(adjusttime&0x03FF)/100;
                }
            }
            break;
            default:
            break;
        
        }
        if(adjusttime&0x4000)
        {
            linuxTime =linuxTime+frame->pipedelay/100;
        }
        DT_Time2DateTimeHex(linuxTime,&dateTimeHex);
        uint8* tmp = (uint8*)&dateTimeHex;
        uint8 k;
        for(j=0;j<indexnum;j++)
        {
            apdu[indexlist[j]] = 0x1C;
            for(k=0;k<7;k++)
            {
                if(apdu[indexlist[j]+1+k] == 0x99)
                {
                    apdu[indexlist[j]+1+k] = *(tmp+k);
                }
            }
        }

    }

    for(j=0;j<apdulen-17;j++)
    {
        if((apdu[j]==0x01)&&(apdu[j+1]==0x10)&&(check_is_all_ch(apdu+j+2, 0x99,16)))
        {
            break;
        }
    }
    if(j < apdulen-17)
    {
        Get_ReadMtrRand(pipe,Rand);
        memcpy(apdu+j+2,Rand,16);
    }
    frame->prtl = PRTL_OOP;
    frame->dstLen=MakeOopFrame(&tsa,frame->srcBuf+offset,apdulen,frame->dstBuf,ctr,ca);
    return TRUE;
}

/**
*********************************************************************
* @name：       Convert_Request_Frame
* @brief：      处理请求报文格式字
* @param[in]   frametype        报文格式字
               tsa         表地址
               frame          结构体中srcbuf 是要处理的报文
* @return    ：frame           结构体中desbuf 是处理完后的报文
* @author    : 高启翔
* @Date      ：2022-03-25
*********************************************************************
*/

BOOL Convert_Request_Frame(uint8 pipe,FRAME_CONVERT_T *frame,uint8 frametype,OOP_TSA_T *tsa)
{
    BOOL result=FALSE;
    
    switch((frametype>>4)&0x0f)
    {
        case 0:
        {
            memcpy(frame->dstBuf,frame->srcBuf,frame->srcLen);
            frame->dstLen = frame->srcLen;
        }
        break;
        case 1:
        {
            switch(frametype&0x0F)
            {
                case 0:
                {
                    memcpy(frame->dstBuf,frame->srcBuf,frame->srcLen);
                    frame->dstLen = frame->srcLen;
                }
                break;
                case 1:
                {
                    result=DealRequestFrameFormat11(frame);
                }
                break;
                case 2:
                {
                    result=DealRequestFrameFormat12(frame);
                }
                break;
                case 3:
                {
                   result=DealRequestFrameFormat13(frame,tsa);
                }
                break;
                case 4:
                {
                   result=DealRequestFrameFormat14(frame,tsa);
                }
                break;
                case 5:
                {
                   result=DealRequestFrameFormat15(frame,tsa);
                }
                break;

            }
        }
        break;
        case 3:
        {
            switch(frametype&0x0F)
            {
                case 0:
                {
                    memcpy(frame->dstBuf,frame->srcBuf,frame->srcLen);
                    frame->dstLen = frame->srcLen;
                }
                break;
                case 1:
                {
                    result=DealRequestFrameFormat31(frame);
                }
                break;

            }
        }
        break;
        case 4:
        {
            memcpy(frame->dstBuf,frame->srcBuf,frame->srcLen);
            frame->dstLen = frame->srcLen;
        }
        break;
        case 5:
        {
            switch(frametype&0x0F)
            {
                case 0:
                {
                    memcpy(frame->dstBuf,frame->srcBuf,frame->srcLen);
                    frame->dstLen = frame->srcLen;
                }
                break;
                case 1:
                {
                    result=DealRequestFrameFormat51(frame,tsa);
                }
                break;
                case 2:
                {
                    result=DealRequestFrameFormat52(pipe,frame,tsa);
                }
                break;
                case 3:
                {
                   result=DealRequestFrameFormat53(pipe,frame);
                }
                break;
               
            }
        }
        break;

    }
    return result;

}

/**
*********************************************************************
* @name：       Convert_Response_Frame
* @brief：      处理响应报文格式字
* @param[in]   frametype        报文格式字
               frame          结构体中srcbuf 是要处理的报文
* @return    ：frame           结构体中desbuf 是处理完后的报文
* @author    : 高启翔
* @Date      ：2022-03-25
*********************************************************************
*/


void Convert_Response_Frame(uint8 pipe,FRAME_CONVERT_T *frame,uint8 frmetype)
{
    uint8 ret;
    uint16 reallen;
    char  *ptr=NULL;
    uint16 FrameLen;
    uint16 tmpoffset;

    switch((frmetype>>4)&0x0f)
    {
        case 0:
        {
            memcpy(frame->dstBuf,frame->srcBuf,frame->srcLen);
            frame->dstLen = frame->srcLen;
        }
        break;
        case 1:
        {
            switch(frmetype&0x0F)
            {
                case 0:
                {
                    char *tmpbuf =(char*)frame->srcBuf ;
                    uint16 buflen = frame->srcLen;
                    uint16 offset = 0;
                    uint16 index = 0;
                    while(offset<buflen-9)
                    {
                        ptr = commfun_645_frame_get(&tmpbuf[offset],0x68,buflen,&reallen);
                        if(ptr!=NULL)
                        {
                            memcpy(&frame->dstBuf[index], ptr, reallen);
                            frame->dstLen += reallen;
                            offset = ptr-tmpbuf+reallen;
                            index += reallen;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                break;
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                {
                    char *tmpbuf =(char*)frame->srcBuf ;
                    uint32 buflen = frame->srcLen;
                    uint16 offset = 0;
                    uint16 index = 0;
                    while(offset<buflen-9)
                    {

                        ret=STD645_CheckRspFrame((uint8*)&tmpbuf[offset],buflen,&frame->prtl);
                        if(ret>0)
                        {
                            ptr = commfun_645_frame_get(&tmpbuf[offset],0x68,buflen,&reallen);
                            //memcpy(tmpbuf, ptr, reallen);
                            if(ptr!=NULL)
                            {
                                memcpy(&frame->dstBuf[index],ptr+8,reallen-10);
                                frame->dstLen += reallen-10;
                                offset = ptr-tmpbuf+reallen;
                                index += reallen-10;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
                break;

            }
        }
        break;
        case 3:
        {
            switch(frmetype&0x0F)
            {
                case 0:
                case 1:
                {
                    memcpy(frame->dstBuf,frame->srcBuf,frame->srcLen);
                    frame->dstLen = frame->srcLen;
                }
                break;
            }
        }
        break;
        case 4:
        {
            memcpy(frame->dstBuf,frame->srcBuf,frame->srcLen);
            frame->dstLen = frame->srcLen;
        }
            break;
        case 5:
        {
            switch(frmetype&0x0F)
            {
                case 0:
                case 3:
                {
                    FrameLen = GetFrame(frame->srcBuf, frame->srcLen, &tmpoffset); 
                    if(FrameLen>0)
                    {
                        memcpy(frame->dstBuf,frame->srcBuf+tmpoffset,FrameLen);
                        frame->dstLen = FrameLen;
                    }
                       
                }
                break;
                case 1:
                case 2:
                {
                    ret = LinkDataReceive(pipe, frame->srcBuf, frame->srcLen,NULL);
                    TASK_FMT_TRACE(pipe, REC_LOG,"LinkDataReceive 698解帧返回值 %d \n",ret);
                    if(ret >=eOopComm_Fail)//解帧失败直接返回
                    {
                        
                        return ;
                    }
                    AppLayerList_t* pAppLayer = NULL;
                    pAppLayer = FindAppLayer(pipe, NULL);
                    if(pAppLayer == NULL)
                        return ;
                    frame->dstLen =  pAppLayer->recvApduList->apduLen;
                    memcpy(frame->dstBuf, pAppLayer->recvApduList->apdu,frame->dstLen );
                    frame->ctrl = pAppLayer->ctrl;

                }
                break;

            }
        }
        break;
    }


}
/*******************************************************************************
* 函数名称: uart_baud_get
* 函数功能: 将comdcb中的波特率转换为实际的
* 输入参数: baud 波特率枚举
* 输出参数: 无
* 返 回 值: 波特率
*******************************************************************************/

int uart_baud_get(uint8 baud)
{
    int             speed = 2400;

    switch(baud)
    {
        case 0:
            speed = 300;
            break;

        case 1:
            speed = 600;
            break;

        case 2:
            speed = 1200;
            break;

        case 3:
            speed = 2400;
            break;

        case 4:
            speed = 4800;
            break;

        case 5:
            speed = 7200;
            break;

        case 6:
            speed = 9600;
            break;

        case 7:
            speed = 19200;
            break;

        case 8:
            speed = 38400;
            break;

        case 9:
            speed = 57600;
            break;

        case 10:
            speed = 115200;
            break;

        case 11:
            speed = 230400;
            break;

        case 12:
            speed = 460800;
            break;

        default:
            break;
    }
    return speed;
}

/*******************************************************************************
* 函数名称: taskmng_current_time_get
* 函数功能: 获取timeval格式的当前时间
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/

void taskmng_current_time_get(struct timeval *tv)
{

    struct  timezone tz; 
    gettimeofday(tv, &tz);

}

/*******************************************************************************
* 函数名称: taskmng_time_calc
* 函数功能: 计算两个时间的差值 tv2-tv1
* 输入参数:
* 输出参数: 
* 返 回 值: 微秒数
*******************************************************************************/
uint32 taskmng_time_calc(struct timeval *tv1,struct timeval *tv2)
{
    uint32 ret;
    if(tv2->tv_sec < tv1->tv_sec)
        return 0;
    if(tv2->tv_sec == tv1->tv_sec)
    {
        if(tv2->tv_usec < tv1->tv_usec)
            return 0;
        ret =  tv2->tv_usec - tv1->tv_usec;
        return ret;
    }
    ret = (tv2->tv_sec - tv1->tv_sec)*1000000 + tv2->tv_usec - tv1->tv_usec;
    return ret;
}

void taskmng_time_stat(uint32 num,uint32* timelist)
{
    uint32 i;
    uint32 max,min,average,all;
    if(num == 0)
        return;
    max = min = timelist[0];
    all = 0;
    for(i=0;i<num;i++)
    {
        all += timelist[i];
        if(timelist[i] > max)
            max = timelist[i];
        if(timelist[i] < min)
            min = timelist[i];
    }
    average = all/num;
    STAT_FMT_DEBUG("统计数据抄表总帧数 %d ccoRouter处理最长耗时 %u 最短耗时 %u 平均耗时 %u \n",num,max,min,average);
}

/*******************************************************************************
* 函数名称: taskmng_stat_time_write
* 函数功能: 写统计时标
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_stat_time_write(uint8 *addr,struct timeval *msgsendtv,struct timeval *downsendtv,struct timeval *downrecvtv)
{
    uint32 costtime=0;  //一帧报文ccoRouter花费的时间 微秒
    static uint32 num = 0;
    static uint32 timelist[500];
    struct tm msgrecvtm;
    struct timeval tv;
    struct  timezone tz; 
    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &msgrecvtm);

    struct tm msgsendtm,downsendtm,downrecvtm;
    FastSecondToDate(msgsendtv,&tz,&msgsendtm);
    FastSecondToDate(downsendtv,&tz,&downsendtm);
    FastSecondToDate(downrecvtv,&tz,&downrecvtm);

    costtime = taskmng_time_calc(msgsendtv,&tv) - taskmng_time_calc(downsendtv,downrecvtv);

    if(num >= 500)
    {
        num = 0;
        memset(timelist,0x00,sizeof(timelist));
    }
    timelist[num++] = costtime;
    
    
    STAT_FMT_DEBUG("电表地址 0x%02x%02x%02x%02x%02x%02x ccoRouter耗时 %u 微秒 发送消息时标:%02d/%02d %02d:%02d:%02d.%ld 收到消息时标:%02d/%02d %02d:%02d:%02d.%ld 下行发送时标:%02d/%02d %02d:%02d:%02d.%ld 下行收到时标:%02d/%02d %02d:%02d:%02d.%ld \n",
        addr[5],addr[4],addr[3],addr[2],addr[1],addr[0],costtime,
        msgsendtm.tm_mon + 1, msgsendtm.tm_mday,msgsendtm.tm_hour, msgsendtm.tm_min, msgsendtm.tm_sec, msgsendtv->tv_usec,
        msgrecvtm.tm_mon + 1, msgrecvtm.tm_mday,msgrecvtm.tm_hour, msgrecvtm.tm_min, msgrecvtm.tm_sec, tv.tv_usec,
        downsendtm.tm_mon + 1, downsendtm.tm_mday,downsendtm.tm_hour, downsendtm.tm_min, downsendtm.tm_sec, downsendtv->tv_usec,
        downrecvtm.tm_mon + 1, downrecvtm.tm_mday,downrecvtm.tm_hour, downrecvtm.tm_min, downrecvtm.tm_sec, downrecvtv->tv_usec);

    taskmng_time_stat(num,timelist);

}

void taskmng_stat_time_write_relay(struct timeval *msgsendtv,struct timeval *downsendtv,struct timeval *downrecvtv)
{
    uint32 costtime=0;  //一帧报文ccoRouter花费的时间 微秒
    static uint32 num = 0;
    static uint32 timelist[500];
    struct tm msgrecvtm;
    struct timeval tv;
    struct  timezone tz; 
    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &msgrecvtm);

    struct tm msgsendtm,downsendtm,downrecvtm;
    FastSecondToDate(msgsendtv,&tz,&msgsendtm);
    FastSecondToDate(downsendtv,&tz,&downsendtm);
    FastSecondToDate(downrecvtv,&tz,&downrecvtm);

    costtime = taskmng_time_calc(msgsendtv,&tv) - taskmng_time_calc(downsendtv,downrecvtv);

    if(num >= 500)
    {
        num = 0;
        memset(timelist,0x00,sizeof(timelist));
    }
    timelist[num++] = costtime;
    
    
    STAT_FMT_DEBUG("透传 ccoRouter耗时 %u 微秒 发送消息时标:%02d/%02d %02d:%02d:%02d.%ld 收到消息时标:%02d/%02d %02d:%02d:%02d.%ld 下行发送时标:%02d/%02d %02d:%02d:%02d.%ld 下行收到时标:%02d/%02d %02d:%02d:%02d.%ld \n",
        costtime,
        msgsendtm.tm_mon + 1, msgsendtm.tm_mday,msgsendtm.tm_hour, msgsendtm.tm_min, msgsendtm.tm_sec, msgsendtv->tv_usec,
        msgrecvtm.tm_mon + 1, msgrecvtm.tm_mday,msgrecvtm.tm_hour, msgrecvtm.tm_min, msgrecvtm.tm_sec, tv.tv_usec,
        downsendtm.tm_mon + 1, downsendtm.tm_mday,downsendtm.tm_hour, downsendtm.tm_min, downsendtm.tm_sec, downsendtv->tv_usec,
        downrecvtm.tm_mon + 1, downrecvtm.tm_mday,downrecvtm.tm_hour, downrecvtm.tm_min, downrecvtm.tm_sec, downrecvtv->tv_usec);

    taskmng_time_stat(num,timelist);

}


uint16 taskmng_stat_fun_0038_unpack(uint8 *buf,uint16 len,struct timeval *sendtv)
{
    uint8 lenoffset;
    uint16 oldlen;
    uint16 index = 0;
    struct timeval downsendtv,downrecvtv;
    oldlen = get_len_offset(buf, &lenoffset);
    index+=lenoffset;
    index+=oldlen;
    //报文前面octet-string的长度
    oldlen+=lenoffset;
    //解析下面的时间
    if(index + 16 > len)
    {
        return oldlen;
    }
    memcpy_r(&downsendtv.tv_sec,&buf[index], 4);
    index+=4;
    memcpy_r(&downsendtv.tv_usec,&buf[index], 4);
    index+=4;
    memcpy_r(&downrecvtv.tv_sec,&buf[index], 4);
    index+=4;
    memcpy_r(&downrecvtv.tv_usec,&buf[index], 4);
    index+=4;

    taskmng_stat_time_write_relay(sendtv,&downsendtv,&downrecvtv);
    return oldlen;
}


void taskmng_comdcb_default(OOP_COMDCB_T *comdcb)
{
    comdcb->baud = 6;
    comdcb->parity = 2;
    comdcb->databits =8;
    comdcb->stopbits = 1;
    comdcb->flowctrl = 0;

}

