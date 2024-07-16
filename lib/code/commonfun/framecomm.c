/*
*********************************************************************
* @file    framecomm.c
* @brief： 公共内容
* @author：all
* @date：   2019.12.24
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#include "framecomm.h"
//#include "zlog.h"
#include "iniparser.h"
#include <net/if.h>    

uint32 gdelaytime=0;      //延时时间


#define SECS_IN_HALFDAY 0x0A8C0L
#define FOURYEARS   (3*365+366)
const unsigned int MonTabLeap[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const unsigned int YrTab[] = { 366, 365, 365, 365 };
uint8              g_HasRealtz_min = FALSE;
int                g_Realtz_min = 0;

/* 获取实际时区 */
int TIME_GET_REAL_TZ()
{
    FILE *fp;
    char ucSystem[256] = {0};
    char timeshow[256] = {0};
    int    tz_min = 0;
    
    /* 从route -n中获取 */
    sprintf(ucSystem, "date");
    fp = popen(ucSystem, "r");
    if(NULL != fp)
    {
        fgets(timeshow, 256, fp);
        printf("%s\n", timeshow);
        pclose(fp);
        
        if(NULL != strstr(timeshow, "CST"))
        {
            tz_min = -480;
        }
    }
    return tz_min;
}

/* localtime存在性能问题 使用 FastSecondToDate 替换 */
void FastSecondToDate(struct timeval* tv, struct timezone* tz, struct tm* tm)
{
    static const int kHoursInDay = 24;
    static const int kMinutesInHour = 60;
    static const int kDaysFromUnixTime = 2472632;
    static const int kDaysFromYear = 153;
    static const int kMagicUnkonwnFirst = 146097;
    static const int kMagicUnkonwnSec = 1461;
    tm->tm_sec  = tv->tv_sec % kMinutesInHour;
    int i       = tv->tv_sec / kMinutesInHour;
    tm->tm_min  = i % kMinutesInHour; //nn
    i /= kMinutesInHour;

    //系统默认北京时区，获取不到时区信息时，手动处理
    if(tz->tz_minuteswest == 0)
    {
        if(FALSE == g_HasRealtz_min)
        {
            g_HasRealtz_min = TRUE;
            g_Realtz_min = TIME_GET_REAL_TZ();
        }
        tz->tz_minuteswest = g_Realtz_min;
    }
    tm->tm_hour = (i - tz->tz_minuteswest / kMinutesInHour) % kHoursInDay; // hh
    tm->tm_mday = (i - tz->tz_minuteswest / kMinutesInHour) / kHoursInDay;
    int a = tm->tm_mday + kDaysFromUnixTime;
    int b = (a*4  + 3)/kMagicUnkonwnFirst;
    int c = (-b*kMagicUnkonwnFirst)/4 + a;
    int d =((c*4 + 3) / kMagicUnkonwnSec);
    int e = -d * kMagicUnkonwnSec;
    e = e/4 + c;
    int m = (5*e + 2)/kDaysFromYear;
    tm->tm_mday = -(kDaysFromYear * m + 2)/5 + e + 1;
    tm->tm_mon = (-m/10)*12 + m + 2;
    tm->tm_year = b*100 + d  - 6700 + (m/10);

    /*基姆拉尔森计算公式 计算星期几 0-表示周日*/
    int y = tm->tm_year + 1900;
    m = tm->tm_mon + 1;
    d = tm->tm_mday;
    if(m==1 || m==2)
    {
        m+=12;
        y--;
    }
    
    tm->tm_wday = ((d + 2*m + 3*(m+1)/5 + y + y/4 -y/100 + y/400) + 1) % 7;
}

/* localtime_r存在性能问题 使用 LOCALTIME_R 替换 */
void LOCALTIME_R(time_t* times, struct tm* tm)
{
    struct  timeval tv = {0};
    struct  timezone tz = {0};
    gettimeofday(&tv, &tz);

    if(NULL != times && NULL != tm)
    {
        tv.tv_sec = *times;
        FastSecondToDate(&tv, &tz, tm);
    }
}

/* mktime存在性能问题 使用 FastDateToSecond 替换 */
time_t FastDateToSecond(struct tm* tm)
{
    uint32 year = 0;
    uint32 month = 0;
    uint32 day = 0;
    uint32 dayTemp = 0;

    time_t sceond = 0;

    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);

    //系统默认北京时区，获取不到时区信息时，手动处理
    if(tz.tz_minuteswest == 0)
    {
        if(FALSE == g_HasRealtz_min)
        {
            g_HasRealtz_min = TRUE;
            g_Realtz_min = TIME_GET_REAL_TZ();
        }
        tz.tz_minuteswest = g_Realtz_min;
    }

    year = tm->tm_year + 1900;
    if(year < 1970 || year >= 2038)
    {
        return 0;
    }

    month = (tm->tm_mon + 1 + 9) % 12;
    year -= month / 10;
    day = year * 365 + year / 4 - year / 100 + year / 400 + (month * 306 + 5)/10 + (tm->tm_mday - 1);

    month = (1 + 9) % 12;
    year  = 1970 - month / 10;
    dayTemp = year * 365 + year / 4 - year / 100 + year / 400 + (month * 306 + 5)/10 + (1 - 1);

    sceond = 24 * 3600 * (day - dayTemp) + tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;
    sceond += tz.tz_minuteswest * 60;

    return sceond;
}

/**********************************************************************
* @name      : Get_Interface_IP
* @brief     ：获取网口的IP地址
* @param[in] ：char *ifname 网口名称
* @param[out]：
* @return    ：IP地址
* @Create    :
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
uint32 Get_Interface_IP(char *ifname)
{ 
    register int fd, intrface;
    struct ifreq buf[12]; 
    struct ifconf ifc; 
    uint32 ipAddr = 0;

    if(NULL == ifname)
    {
        return 0;
    }
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) 
    { 
        ifc.ifc_len = sizeof(buf); 
        ifc.ifc_buf = (caddr_t) buf; 

        if (!ioctl(fd, SIOCGIFCONF, (char *) &ifc)) 
        {
            /*获取接口数量*/
            intrface = ifc.ifc_len / sizeof (struct ifreq);

            /*查找PPP位置*/
            while ( (strcmp(buf[intrface].ifr_name, ifname)) && (intrface >= 0) )
                intrface--;

            if (intrface >= 0)
            {
                /*获取当前网卡的IP地址*/ 
                if (!(ioctl (fd, SIOCGIFADDR, (char *)&buf[intrface]))) 
                {
                    ipAddr = ((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr.s_addr;
                    if (0 != ipAddr)
                    {
                        close(fd);
                        return ntohl(ipAddr);
                    }
                }           
            }
        } 
        else 
            perror ("ioctl error"); 

        close(fd);
    }
    return 0;
}


#if DESC("字符串处理", 1)

/**
*********************************************************************
* @brief:       字符串分割
* @param[in]：   src     源字符串的首地址
* @param[in]：   delim   分割字符串
* @param[out]:  dest    分割后的字符串组
* @param[out]:  num     分割后的字符串个数
* @author:      贺宁
* @Date：        2020-05-22
* @return：      void
*********************************************************************
*/
void split(char *src, const char *delim, char **dest, int *num) 
{
    char *saveptr = NULL;
    char *pNext = NULL;
    int count = 0;

    if (src == NULL || strlen(src) == 0) 
    {
        return;
    }

    if (delim == NULL || strlen(delim) == 0)
    {
        return;
    }

    pNext = (char *)strtok_r(src, delim, &saveptr);

    while(pNext != NULL) 
    {
        *dest++ = pNext;
        ++count;
        pNext = (char *)strtok_r(NULL, delim, &saveptr); 
    }  

    *num = count;
}   

/**
*********************************************************************
* @brief:       字符串全转为小写
* @param[in]：   src     源字符串的首地址
* @author:      贺宁
* @Date：        2020-01-20
* @return：      void
*********************************************************************
*/
void str2lower(char * s)
{
    int i ;

    if (s==NULL) return;
    i=0 ;
    while (s[i]) {
        s[i] = (char)tolower((int)s[i]);
        i++ ;
    }
}

/**
*********************************************************************
* @brief:       字符串全转为小写
* @param[in]：   src     源字符串的首地址
                 len     输入长度
* @author:      贺宁
* @Date：        2020-01-20
* @return：      void
*********************************************************************
*/
void str2lower_n(char * s, int len)
{
    int i ;

    if (s==NULL) return;
    i=0 ;
    while (s[i] && (i < len)) 
    {
        s[i] = (char)tolower((int)s[i]);
        i++ ;
    }
}

/**
*********************************************************************
* @brief:       字符串全转为小写
* @param[in]：   src     源字符串的首地址
* @author:      贺宁
* @Date：        2020-01-20
* @return：      void
*********************************************************************
*/
void str2upper(char * s)
{
    int i ;

    if (s==NULL) return;
    i=0 ;
    while (s[i]) {
        s[i] = (char)toupper((int)s[i]);
        i++ ;
    }
}

/**
*********************************************************************
* @brief:       字符串全转为小写
* @param[in]：   src     源字符串的首地址
                 len     输入长度
* @author:      贺宁
* @Date：        2020-01-20
* @return：      void
*********************************************************************
*/
void str2upper_n(char * s, int len)
{
    int i ;

    if (s==NULL) return;
    i=0 ;
    while (s[i] && (i < len)) 
    {
        s[i] = (char)toupper((int)s[i]);
        i++ ;
    }
}

/**
*********************************************************************
* @brief:       字符串转为字节串，字符串范围0~9,a~f,A~F
*               奇数字符串最后一个字节前面补0
* @param[in]：   str      字符串
* @param[in]：   strLen   字符串长度
* @param[out]： octet     字节串
* @param[in]：   octetlen 字节串长度
* @author:      贺宁
* @Date：        2021-02-01
* @return：      <0 错误，其它 字节串长度
*********************************************************************
*/
int strt2octet(char *str, int strLen, unsigned char *octet, int octetLen)
{
    int i = 0, len = 0;

    if (str == NULL || octet == NULL)
    {
        return ERR_PNULL;
    }

    //校验输入字符串的合法性
    for(i = 0; i < strLen; i++)
    {
        if(((str[i] >= '0')&&(str[i] <= '9')) 
            || ((str[i] >= 'a')&&(str[i] <= 'f')) 
            || ((str[i] >= 'A')&&(str[i] <= 'F')) || 0 == str[i]) 
        {
            continue;
        }
        else
        {
            return ERR_O_RANGE;
        }
    }

    str2lower_n(str, strLen);

    //字节串长度
    len = (strLen / (2 * sizeof(char))) + strLen % (2 * sizeof(char));
    if(octetLen < len)
    {
        return ERR_OVERFLOW;
    }
    
    for (i = 0; i < len; i++)
    {
        sscanf(str + 2 * i, "%02x", (unsigned int *)(octet + i));
    }
 
    return len;
}

/**
*********************************************************************
* @brief:       字节串转为字符串，字符串范围0~9,a~f,A~F (16进制)
*               奇数字符串最后一个字节前面补0
* @param[in]：   str      字符串
* @param[in]：   strLen   字符串长度
* @param[out]： octet     字节串
* @param[in]：   octetlen 字节串长度
* @author:      贺宁
* @Date：        2021-02-01
* @return：      <0 错误，其它 字符串长度
*********************************************************************
*/
int octet2str(unsigned char *octet, int octetLen, char *str, int strLen)
{
    int i = 0, len = 0;
    
    if (str == NULL || octet == NULL)
    {
        return ERR_PNULL;
    }

    //字符串长度
    len = 2*octetLen;
    if(strLen < len)
    {
        return ERR_OVERFLOW;
    }

    for(i = 0; i < octetLen; i++)
    {
        sprintf(&str[i*2], "%02x", octet[i]);
    }

    return len;
}

/**
*********************************************************************
* @brief:       将非标准字符串（末尾不含'\0'）转换为C规范字符串
* @param[in]：   str      字符串
* @param[in]：   strLen   字符串长度
* @param[out]： fmtstr   规范字符串
* @param[in]：   fmtlen   规范字符串长度
* @author:      贺宁
* @Date：        2021-02-01
* @return：      <0 错误，其它 字符串长度
*********************************************************************
*/
int strformat(char *fmtstr, int fmtlen, char *str, int strLen)
{
    if (str == NULL || fmtstr == NULL)
    {
        return ERR_PNULL;
    }

    if(fmtlen < strLen+1)
    {
        return ERR_OVERFLOW;
    }

    strcpy(fmtstr, str);
    fmtstr[strLen] = '\0';

    return strLen+1;
}

#endif

#if DESC("文件操作",1)

/*
  名  称: rcd_lock
  功  能: 用fcntl方式实现记录锁
  参  数:
    fd   - 锁定文件的文件描述符
    cmd  - 锁定命令(F_SETLK, F_SETLKW, F_GETLK)
    type - 锁类型(F_RDLCK, F_WRLCK, F_UNLCK)
  返回值:
*/
int rcd_lock(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
  struct flock locker;

  locker.l_type   = type;
  locker.l_start  = offset;
  locker.l_whence = SEEK_SET;
  locker.l_len    = len;

  return fcntl(fd, cmd, &locker);
}

BOOLEAN file_exist(const char *filepath)
{
    struct stat sta;
    
    if( stat(filepath, &sta) < 0)
        return FALSE;
        
    if( S_ISREG(sta.st_mode))
        return TRUE;

    return FALSE;
}

int file_path_extract(const char *src, char *path)
{
    int     len, frag;
    char    *off;
    
    if( !src || !path) return -1;
        
    len = strlen(src);
    
    off = (char *)&src[len - 1];
        
    frag = 0;
    while((off != &src[0]) && (*off != '/'))
    {
        off--;
        frag++;
    }
        
    strncpy(path, src, len - frag);  //!这个函数不会自动在字符串后面势\0'
    path[len - frag] = '\0';
    return 0;
}

BOOLEAN path_exist(const char *pathname)
{
    struct stat sta;
    
    if( stat(pathname, &sta) < 0)
        return FALSE;
        
    if( S_ISDIR(sta.st_mode))
        return TRUE;

    return FALSE;
}

static char *next_path_get(const char *src, const char *dirName, unsigned bufLen)
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

BOOLEAN path_dir_make(const char *path)
{
    char        *ptr;
    char        filepath[PATH_MAX + 1];
    char        dirName[128];
    BOOLEAN      bRelative = FALSE;
    
    ptr = (char *)path;     
    if(NULL == path) return FALSE;
    
    if('/' != *ptr)  
        bRelative = TRUE;
        
    if( (ptr = next_path_get(ptr, dirName, 128)) != NULL)  //获得根目彿
    {
        if(!bRelative) // 如果创建的是绝对路径，则在创建目录名前加'/'表示绝对路径
            strcpy(filepath, "/");
        else
            filepath[0] = '\0';
            
        strcat(filepath, dirName);

        if( mkdir(filepath, 0755) < 0)
            if( errno != EEXIST)
                return FALSE;

    }
    else //根目录提取错误
        return FALSE;
    
    while( (ptr = next_path_get(ptr, dirName, 128) ) != NULL)
    {
        strcat(filepath, "/");
        strcat(filepath, dirName);

        if( mkdir(filepath, 0755) < 0)
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

ssize_t file_size_get(const char *filename)
{
    struct  stat sta;
    
    if( stat( filename, &sta) < 0)
        return -1;
        
    if( S_ISREG(sta.st_mode)) //普通文仿
    {
        return sta.st_size;
    }
    else 
        return -1;
}

/**
*********************************************************************
* @brief:       从文件路径中提取文件名
* @param[in]：   src         完整文件路径
* @param[in]:   withSubfix  提取文件名是否带后缀
* @param[out]：  filename    提取的文件名

* @return：      void
*********************************************************************
*/
int file_name_extract(const char *src, BOOLEAN withSubfix, char *filename)
{
    int     len, cnt, i;
    char    *off;
    char  name[NAME_MAX + 1];
    char  name2[NAME_MAX + 1];

    memset(name, 0x00, sizeof(name));
    
    if( !src || !filename) return -1;
        
    len = strlen(src);
    
    off = (char *)&src[len - 1];

    cnt = 0;
    while((off != &src[0]) && (*off != '/'))
    {
        name[cnt++] = *off--;
    }
    
    memset(name2, 0x0, sizeof(name2));

    for( i = 0; i < cnt; i++)
    {
        name2[i] = name[cnt - 1 - i];
    }

    for(i = 0; i < strlen(name2); i++)
    {
        if((withSubfix == FALSE) && (name2[i] == '.'))
        {
            break;
        }
        
        *filename++ = name2[i];
    }

    return 0;
}

/**
*********************************************************************
* @brief:       从文件路径中提取文件扩展名
* @param[in]：   src         完整文件路径
* @param[out]：  ext         提取的扩展名

* @return：      void
*********************************************************************
*/
int file_extension_extract(const char *src, char *ext)
{
    int     len, frag;
    char    *off;
    
    if( !src || !ext) return -1;
        
    len = strlen(src);
    
    off = (char *)&src[len - 1];
    
    frag = 0;
    while((off != &src[0]) && (*off != '/') && (*off != '.'))
    {
        off--;
        frag++;
    }

    if( *off != '.')
    {
        *ext = '\0';
        return 0;
    }
    
    strncpy(ext, off + 1, len - frag);
    return 0;
}

#endif

#if DESC("安全函数",1)
/**
*********************************************************************
* @brief:       内存拷贝函数（安全版本）
* @detail:      如果count>destsz,或count>SIZE_MAX, dest和destsz本身有效，
*               则清零目标范围[dest, dest + destsz)的每个位置
*               该行为由C11标准，K3.7.1.1定义
* @param[out]：  dest        目的缓冲区
* @param[in]：   destsz      目的缓冲区大小
* @param[in]：   src         源缓冲区数组
* @param[in]：   count     源数组的成员元素大小
* @return：      =0,正确，!=0,错误
* @author:      贺宁
* @Date：        2020-03-30
*********************************************************************
*/
int memcpy_s(void * __restrict dest, size_t destsz, const void * __restrict src, size_t count)
{
    //空指针
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //整型溢出
    if(destsz>SIZE_MAX)
    {
        return ERR_O_RANGE;
    }

    //拷贝越界
    if(destsz < count)
    {
        memset(dest, 0, destsz);
        return ERR_OPTION;
    }

    //整型溢出
    if(count>SIZE_MAX)
    {
        memset(dest, 0, destsz);
        return ERR_O_RANGE;
    }
    
    //空指针
    if(src == NULL)
    {
        memset(dest, 0, destsz);
        return ERR_PNULL;
    }

    //地址重叠
    if((dest>src)&&(dest<src+count))
    {        
        memset(dest, 0, destsz);
        return ERR_PTR;
    }

    memcpy(dest, src, count);
    
    return ERR_OK;
}

/**
*********************************************************************
* @brief:       内存移动函数（安全版本）
* @detail:      如果count>destsz,或count>SIZE_MAX, dest和destsz本身有效，
*               则清零目标范围[dest, dest + destsz)的每个位置
*               该行为由C11标准，K3.7.1.2定义
* @param[out]：  dest        目的缓冲区
* @param[in]：   destsz      目的缓冲区大小
* @param[in]：   src         源缓冲区数组
* @param[in]：   count     源数组的成员元素大小
* @return：      =0,正确，!=0,错误
* @author:      贺宁
* @Date：        2020-03-30
*********************************************************************
*/
int memmove_s(void * dest, size_t destsz, const void * src, size_t count)
{
    //空指针
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //整型溢出
    if(destsz>SIZE_MAX)
    {
        return ERR_O_RANGE;
    }

    //拷贝越界
    if(destsz < count)
    {
        memset(dest, 0, destsz);
        return ERR_OPTION;
    }

    //整型溢出
    if(count>SIZE_MAX)
    {
        memset(dest, 0, destsz);
        return ERR_O_RANGE;
    }

    if(src == NULL)
    {
        memset(dest, 0, destsz);
        return ERR_PNULL;
    }

    memmove(dest, src, count);
    
    return ERR_OK;
}

/**
*********************************************************************
* @brief:       内存设置函数（安全版本）
* @detail:      如果count>destsz,或count>SIZE_MAX, dest和destsz本身有效，
*               则在将ch存储到目标范围[dest, dest + destsz)的每个位置
*               该行为由C11标准，K3.7.4.1定义
* @param[out]：  dest        目的缓冲区
* @param[in]：   destsz      目的缓冲区大小
* @param[in]：   ch          填充字节
* @param[in]：   count       源数组的成员元素大小
* @return：      =0,正确，!=0,错误
* @author:      贺宁
* @Date：        2020-03-30
*********************************************************************
*/
int memset_s(void * dest, size_t destsz, int ch, size_t count)
{
    //空指针
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //整型溢出
    if(destsz>SIZE_MAX)
    {
        return ERR_O_RANGE;
    }

    memset(dest, ch, MIN(destsz, count));

    //整型溢出
    if(count>SIZE_MAX)
    {
        return ERR_O_RANGE;
    }
    
    //拷贝越界
    if(destsz < count)
    {
        return ERR_OVERFLOW;
    }
    
    return ERR_OK;
}

/**
*********************************************************************
* @brief:       内存反转拷贝函数（安全版本）
* @detail:      如果count>destsz,或count>SIZE_MAX, dest和destsz本身有效，
*               则清零目标范围[dest, dest + destsz)的每个位置
* @param[out]：  dest        目的缓冲区
* @param[in]：   destsz      目的缓冲区大小
* @param[in]：   src         源缓冲区数组
* @param[in]：   count     源数组的成员元素大小
* @return：      =0,正确，!=0,错误
* @author:      贺宁
* @Date：        2020-03-30
*********************************************************************
*/
int memcpy_rs(void * __restrict dest, size_t destsz, const void * __restrict src, size_t count)
{
    //空指针
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //整型溢出
    if(destsz>SIZE_MAX)
    {
        return ERR_O_RANGE;
    }

    //拷贝越界
    if(destsz < count)
    {
        memset(dest, 0, destsz);
        return ERR_OPTION;
    }

    //整型溢出
    if(count>SIZE_MAX)
    {
        memset(dest, 0, destsz);
        return ERR_O_RANGE;
    }

    //空指针
    if(src == NULL)
    {
        memset(dest, 0, destsz);
        return ERR_PNULL;
    }

    //地址重叠
    if((dest>src)&&(dest<src+count))
    {        
        memset(dest, 0, destsz);
        return ERR_PTR;
    }

    memcpy_rev(dest, src, count);
    
    return ERR_OK;
}

/**
*********************************************************************
* @brief:       字符串长度函数（安全版本）
* @detail:      成功时以空字符结尾的字节串str的长度，如果str是空指针则返回零，
*               如果在前strsz个字符中未找到空字符，则返回strsz
*               该行为由C11标准，K3.7.4.4定义
* @param[in]：   str        要检查的以空字符结尾的字符串
* @param[in]：   strsz      检查的最大字符串长度
* @return：      字符串长度
* @author:      贺宁
* @Date：        2020-03-30
*********************************************************************
*/
size_t strnlen_s(const char * str, size_t strsz)
{
    int i = 0;
    
    if(str == NULL)
    {
        return 0;
    }

    while(str[i++] != '\0')
    {
        if(i == strsz)
        {
            return strsz;
        }
    }

    return strlen(str);
}

/**
*********************************************************************
* @brief:       字符串拷贝函数（安全版本）
* @detail:      如果出错，dest和destsz本身有效，则将零写入dest[0]
                该行为由C11标准，K3.7.1.3定义
* @param[out]：  dest        目的字符缓冲区
* @param[in]：   destsz      目的缓冲区大小
* @param[in]：   src         源字符缓冲区
* @return：      =0,正确，!=0,错误
* @author:      贺宁
* @Date：        2020-03-30
*********************************************************************
*/

int strcpy_s(char * __restrict dest, size_t destsz, const char * __restrict src)
{
    int srcsz = strnlen_s(src, destsz);
    
    //空指针
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //整型溢出或长度为0
    if((destsz>SIZE_MAX) || (destsz == 0))
    {
        return ERR_O_RANGE;
    }

    //空指针
    if(src == NULL)
    {
        dest[0] = '\0';
        return ERR_PNULL;
    }

    //截断
    if(destsz <= srcsz)
    {
        dest[0] = '\0';
        return srcsz;
    }

    //地址重叠
    if((dest>src)&&(dest<src+srcsz))
    {        
        dest[0] = '\0';
        return ERR_PTR;
    }

    strcpy(dest, src);

    return ERR_OK; 
}

/**
*********************************************************************
* @brief:       字符串拷贝函数（安全版本）
* @detail:      如果出错，dest和destsz本身有效，则将零写入dest[0]
                该行为由C11标准，K3.7.1.4定义
* @param[out]：  dest        目的字符缓冲区
* @param[in]：   destsz      目的缓冲区大小
* @param[in]：   src         源字符缓冲区
* @param[in]：   count       拷贝的字符长度
* @return：      =0,正确，!=0,错误
* @author:      贺宁
* @Date：        2020-03-30
*********************************************************************
*/
int strncpy_s(char * __restrict dest, size_t destsz, const char * __restrict src, size_t count)
{
    int srcsz = strnlen_s(src, destsz);
    int srccn = strnlen_s(src, count);
    
    //空指针
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //整型溢出或长度为0
    if((destsz>SIZE_MAX) || (destsz == 0))
    {
        return ERR_O_RANGE;
    }

    //整型溢出
    if(count>SIZE_MAX)
    {
        dest[0] = '\0';
        return ERR_O_RANGE;
    }

    //空指针
    if(src == NULL)
    {
        dest[0] = '\0';
        return ERR_PNULL;
    }

    //截断
    if((count>=destsz)&&(destsz <= srccn))
    {
        dest[0] = '\0';
        return srccn;
    }

    //地址重叠
    if((dest>src)&&(dest<src+srcsz))
    {        
        dest[0] = '\0';
        return ERR_PTR;
    }

    strncpy(dest, src, count);

    return ERR_OK; 

}

/**
*********************************************************************
* @brief:       字符串连接函数（安全版本）
* @detail:      如果出错，dest和destsz本身有效，则将零写入dest[0]
                该行为由C11标准，K3.7.2.1定义
* @param[out]：  dest        目的字符缓冲区
* @param[in]：   destsz      目的缓冲区大小
* @param[in]：   src         源字符缓冲区
* @param[in]：   count       拷贝的字符长度
* @return：      =0,正确，!=0,错误
* @author:      贺宁
* @Date：        2020-03-30
*********************************************************************
*/
int strcat_s(char * __restrict dest, size_t destsz, const char * __restrict src)
{
    int i = 0;
    int srcsz = strnlen_s(src, destsz);

    //空指针
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //整型溢出或长度为0
    if((destsz>SIZE_MAX) || (destsz == 0))
    {
        return ERR_O_RANGE;
    }

    //空指针
    if(src == NULL)
    {
        dest[0] = '\0';
        return ERR_PNULL;
    }

    //dest中无终止符
    while(dest[i++] != '\0')
    {
        if(i == destsz)
        {
            dest[0] = '\0';
            return ERR_FORMAT;
        }
    }
    
    //截断
    if(destsz <= strnlen_s(dest, destsz) + srcsz)
    {
        dest[0] = '\0';
        return srcsz;
    }

    //地址重叠
    if((dest>src)&&(dest<src+srcsz))
    {        
        dest[0] = '\0';
        return ERR_PTR;
    }

    strcat(dest, src);

    return ERR_OK; 
}

#endif

#if DESC("内存操作函数",1)
/**
*********************************************************************
* @brief:       拷贝一个数组(假定数组小端序存储)
* @param[out]：  dstArray    目的数组
* @param[in]：   dstStep     目的数组的成员元素大小
* @param[out]：  srcArray    源数组
* @param[in]：   srcStep     源数组的成员元素大小
* @param[in]：   count       拷贝元素个数
* @return：      void
*********************************************************************
*/
void arrcpy(void *dstArray, int dstStep, void *srcArray, int srcStep, int count)
{
    int i = 0;
    int step = 0;

    step = dstStep < srcStep ? dstStep: srcStep;

    for(i = 0; i < count; i++)
    {
        memcpy(((char*)dstArray + i*dstStep), ((char*)srcArray + i*srcStep), step);
    }

    return;
}

/**
*********************************************************************
* @brief:       反转拷贝，从高字节开始
* @param[out]：  dest      目的数据缓冲区
* @param[in]：   src       源数据缓冲区
* @param[in]：   count     拷贝字节数
* @return：      void*     目的缓冲区写入尾部
*********************************************************************
*/
void *memcpy_rev(void *dest, const void *src, int count)
{
    void *ptr = dest;
    
    if(NULL == dest || NULL == src || count <= 0)  
    {   
        return NULL;    
    }
    
    while(count--)
    {
        *(char *)dest++ = *(char *)(src+count); 
    }

    return ptr;
}

/**
*********************************************************************
* @brief:       反转拷贝，从高字节开始
* @param[out]：  dest      目的数据缓冲区
* @param[in]：   src       源数据缓冲区
* @param[in]：   count     拷贝字节数
* @return：      void*     目的缓冲区写入尾部
*********************************************************************
*/
void *memcpy_r(void *dest, const void *src, int count)
{
    void *ptr = dest;
    
    if(NULL == dest || NULL == src || count <= 0)  
    {   
        return NULL;    
    }
    
    while(count--)
    {
        *(char *)dest++ = *(char *)(src+count); 
    }

    return ptr;
}

/**
*********************************************************************
* @brief:       地址拷贝
* @param[out]：  dest      目的数据缓冲区
* @param[in]：   src       源数据缓冲区
* @param[in]：   count     拷贝字节数
* @param[in]：   isRev     是否选择反转 TRUE 反转拷贝 FLASE 直接拷贝
* @return：      void*     目的缓冲区写入尾部
*********************************************************************
*/
void *addrcpy(void *dest, const void *src, int count, BOOLEAN isRev)
{
    if (isRev)
    {
        return memcpy_rev(dest, src, count);
    }
    else
    {
        return memcpy(dest, src, count);
    }
}

/**
*********************************************************************
* @brief:       指定长度的内存数据反转
* @param[out]：  buff:     内存入口地址
* @param[in]：   len:      数据长度
* @return：      void*     目的缓冲区写入尾部
*********************************************************************
*/
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

/**
*********************************************************************
* @brief:       指定长度的内存数据按位反转
* @param[out]：  buf       内存入口地址
* @param[in]：   buf       内存入口地址
* @param[in]：   nBits     比特数
* @return：
*********************************************************************
*/
void membitrev(uint8* buff, uint16 nBits)
{
#if(1)
    uint16 i,byteNum;
    uint8 j;
    char tmp;
    if(nBits&0x07)
        nBits += 1;
    byteNum = nBits/8;
    if (nBits&0x07)
    {
        byteNum += 1;
    }
    memrev(buff, byteNum);//先按字节反转
    for(i = 0; i < byteNum; i++) //再每个字节按位反转
    {
        tmp = 0;
        for (j = 0; j < 8; j++)
        {
            tmp |= (((buff[i]>>(7-j))&0x01)<<j);
        }

        buff[i] = tmp;
    }
#endif
}

/**
*********************************************************************
* @brief:       memcpy_bitrev 拷贝指定长度的内存数据并按位反转
* @param[out]：  buff      目的数据缓冲区
* @param[in]：   buff      源数据缓冲区
* @param[in]：   len       数据长度
* @return：      void      无
*********************************************************************
*/
void memcpy_bitrev(uint8 *dest, const uint8 *src, int count)
{
    uint16 i;
    uint8  j;
    uint8  tmp;
    
    memcpy_r(dest, src, count);//先反转拷贝
        
    for (i = 0; i < count; i++) //再每个字节按位反转
    {
        tmp = 0;
        for (j = 0; j < 8; j++)
        {
            tmp |= (((dest[i]>>(7-j))&0x01)<<j);
        }

        dest[i] = tmp;
    }
    return;
}

/**
*********************************************************************
* @brief:       去除数组中重复元素
* @param[in]：   array       源数组
* @param[in]：   num         数组元素个数
* @param[in]：   size        每个元素大小
* @return：      void
* @author:      贺宁
* @Date：        2020-03-28
*********************************************************************
*/
void arrsingle(void *pArray, int *pNum, int size, cmpfun arrcmp)
{
    int i = 0, j = 0;
    int num = *pNum;
    BOOLEAN bEqual = FALSE;

    for(i = 0; i < num; i++)
    {
        bEqual = FALSE;
        for(j = i+1; j < num; j++)
        {
            if(arrcmp != NULL)
            {
                if(arrcmp(((char*)pArray+i*size),((char*)pArray+j*size)) == 0)
                {
                    bEqual = TRUE;
                }                
            }
            else
            {
                if(memcmp(((char*)pArray+i*size),((char*)pArray+j*size),size) == 0)
                {
                    bEqual = TRUE;
                }
            }
            
            if(bEqual == TRUE)
            {
                if(j == num-1)
                {
                    MEMZERO(((char*)pArray+j*size), size);
                }
                else
                {
                    memmove(((char*)pArray+j*size), ((char*)pArray+(j+1)*size), (num-j-1)*size);
                }

                j--;
                num--;
            }
        }
    }

    *pNum = num;

    return;
}

/**
*********************************************************************
* @brief:       去除两个同类型数组中重复元素，删除array2与array1相同的元素
* @param[in]：   array1    第一个数组
* @param[in]：   num1      第一个数组元素个数
* @param[in]：   array2    第二个数组
* @param[in]：   num2      第二个数组元素个数
* @param[in]：   size      每个元素大小
* @return：      void
* @author:      贺宁
* @Date：        2020-03-28
*********************************************************************
*/
void arrdsingle(void *array1, int *num1, void *array2, int *num2, int size, cmpfun arrcmp)
{
    int i = 0, j = 0;
    BOOLEAN bEqual = FALSE;

    arrsingle(array1, num1, size, arrcmp);
    arrsingle(array2, num2, size, arrcmp);
    
    for(i = 0; i < *num2; i++)
    {
        bEqual = FALSE;
        for(j = 0; j < *num1; j++)
        {
            if(arrcmp != NULL)
            {
                if(arrcmp(((char*)array2+i*size),((char*)array1+j*size)) == 0)
                {
                    bEqual = TRUE;
                }                
            }
            else
            {
                if(memcmp(((char*)array2+i*size),((char*)array1+j*size), size) == 0)
                {
                    bEqual = TRUE;
                }
            }        
        
           if(bEqual == TRUE)
           {
                if(i == *num2-1)
                {
                    MEMZERO(((char*)array2+i*size), size);
                }
                else
                {
                    memmove(((char*)array2+i*size), ((char*)array2+(i+1)*size), (*num2-i-1)*size);
                }
               i--;
               (*num2)--;   
           }

           break;
        }
    }

    return;
}

/**
*********************************************************************
* @brief:       判断值是否存在于数组中
* @param[in]：   array     数组首地址
* @param[in]：   num       数组成员数量
* @param[in]：   size      每个元素大小
* @param[in]：   value     要判断的值
* @return：      =TRUE,FALSE
* @author:      贺宁
* @Date：        2020-04-12
*********************************************************************
*/
BOOLEAN arrfind(void *array, int num, int size, void *value)
{
    int i = 0;

    for(i = 0; i < num; i++)
    {
        if(memcmp(((char*)array+i*size),(char*)value, size) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

#endif

#if DESC("698协议公共处理",1)

/**
*********************************************************************
* @brief：      比较TSA是否相等
* @param[in]:  pstTSA1 参与比较的通信地址1
* @param[in]:  pstTSA2 参与比较的通信地址2
* @return：     FALSE-不相等，TRUE-相等
* @author:     贺宁
* @Date：       2020-03-28
*********************************************************************
*/
BOOLEAN oop_tsa_equal(OOP_TSA_T *pstTSA1, OOP_TSA_T *pstTSA2)
{
    uint32 i = 0;
    
    if(pstTSA1->af != pstTSA2->af)
    {
        return FALSE;
    }

    for(i = 0; i < pstTSA1->len+1; i++)
    {
        if(pstTSA1->add[i] != pstTSA2->add[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

#endif

#if DESC("系统公共",1)
/**
*********************************************************************
* @brief：      单实例检查
* @param[in]： NA
* @param[out]：NA
* @author:     贺宁
* @return：     =TRUE  检查通过，运行单个实例     ，=FALSE 检查不通过，已经有实例运行
*********************************************************************
*/
BOOLEAN singleton_check(char *appName)
{
    int ret = 0;
    int fd = 0;
    struct flock flockstr;
    pid_t pid = 0;
    char lockFile[128] = {0};

    pid = getpid();
    flockstr.l_type = F_WRLCK;
    flockstr.l_start = 0;
    flockstr.l_whence = SEEK_SET;
    flockstr.l_len = 0;
    flockstr.l_pid = pid;

    sprintf(lockFile, "/var/run/%s.lock", appName);
    
    fd = open(lockFile,O_WRONLY | O_CREAT,0660); 
    if (fd < 0)
    {
        printf("open %s failed.\n", lockFile);
        return FALSE;
    }

    ret = fcntl(fd,F_SETLK,&flockstr);
    if (ret < 0)
    {
        fcntl(fd,F_GETLK,&flockstr);

        printf("the %s app is running now! and it's pid is %d\n", appName, flockstr.l_pid);
        return FALSE;
    }
    
    write(fd,(char *)&pid,sizeof(pid_t));

    return TRUE;    
}

/**
*********************************************************************
* @brief：      进程自杀
* @param[in]： app名称
* @param[out]：NA
* @author:     贺宁
* @return：     void
*********************************************************************
*/
void kill_myself_exec(char *appName)
{
    int ret = 0;
    int fd = 0;
    pid_t pid = 0;
    char appLicFile[128] = {0};
    struct timespec  accsecs = {0};
    static BOOLEAN bfirstRun = TRUE;
    static time_t rdAccSecs = 0; 
    static time_t baseTime = 0;    
    static time_t accbaseTime = 0; //累积时间基础值
    static time_t acclimitTime = 0;
    time_t wtAccSecs = 0; 
    time_t curTime = time(NULL);

    pid = getpid();

    sprintf(appLicFile, "/data/app/%s.lic", appName);

    fd = open(appLicFile,O_RDWR,0660); 
    if (fd < 0)
    {
        kill(pid, 9);
        return;
    }

    //获取系统运行的相对时间
    clock_gettime(CLOCK_MONOTONIC, &accsecs);
    wtAccSecs = accsecs.tv_sec;

    if(bfirstRun == TRUE)
    {
        bfirstRun = FALSE;

        //读取绝对时间基础值
        lseek(fd, 0, SEEK_SET);
        ret |= read(fd, &baseTime, sizeof(time_t));

        //读取相对时间限制值
        lseek(fd, sizeof(time_t), SEEK_SET);
        ret |= read(fd, &acclimitTime, sizeof(time_t));

        //读取上次的相对时间
        lseek(fd, 2*sizeof(time_t), SEEK_SET);
        ret |= read(fd, &rdAccSecs, sizeof(time_t));

        accbaseTime = wtAccSecs;
    }

    //更新相对时间
    wtAccSecs = wtAccSecs-accbaseTime + rdAccSecs;
    lseek(fd, 2*sizeof(time_t), SEEK_SET);
    ret |= write(fd,(char *)&wtAccSecs,sizeof(time_t));

    if((wtAccSecs > acclimitTime) || (curTime> (baseTime+acclimitTime)) || ret < 0)
    {
        kill(pid, 9);
    }

    close(fd);
    
    return;
}

/**
*********************************************************************
* @brief：      不中断的睡眠函数
* @param[in]： seconds 秒数
* @param[out]：NA
* @author:     贺宁
* @return：     NA
*********************************************************************
*/
void sleep_no_interrupt(unsigned int seconds)
{
    int left = seconds;

    //sleep can be wake up by alarm or Ctrl + C, but we can restart it! 
    //because the sleep function will return the left second after be wake up!    
    while (left > 0)
    {
        left = sleep(left);
    }

    return;
}

/**
*********************************************************************
* @brief：      安全秒延时，不会因信号等使实际延时小于预期
* @param[in]： s 延时秒数
* @param[out]：NA
* @author:     贺宁
* @return：     NA
*********************************************************************
*/
int sleep_s(unsigned int s)
{
    int ret = 0;
    struct timeval timeout;

    timeout.tv_sec = s;
    timeout.tv_usec = 0;

    do
    {
        ret = select(0, NULL, NULL, NULL, &timeout);
    }while(ret < 0 && errno == EINTR);

    return ret;
}

/**
*********************************************************************
* @brief：      安全毫秒延时，不会因信号等使实际延时小于预期
* @param[in]： s 延时秒数
* @param[out]：NA
* @author:     贺宁
* @return：     NA
*********************************************************************
*/
int msleep_s(unsigned int ms)
{
    int ret = 0;
    struct timeval timeout;

    timeout.tv_sec = ms/1000;
    timeout.tv_usec = (ms%1000)*1000;

    do
    {
        ret = select(0, NULL, NULL, NULL, &timeout);
    }while(ret < 0 && errno == EINTR);

    return ret;
}

/**
*********************************************************************
* @brief：      安全微秒延时，不会因信号等使实际延时小于预期
* @param[in]： s 延时秒数
* @param[out]：NA
* @author:     贺宁
* @return：     NA
*********************************************************************
*/
int usleep_s(unsigned int us)
{
    int ret = 0;
    struct timeval timeout;

    timeout.tv_sec = us/1000000;
    timeout.tv_usec = (us%1000000)*1000000;

    do
    {
        ret = select(0, NULL, NULL, NULL, &timeout);
    }while(ret < 0 && errno == EINTR);

    return ret;
}

/**
*********************************************************************
* @brief：      设置位串某位的值，设置为1
* @param[in]： rsv           数据标志位
               bitStringlen 标志位长度
               bitpos       数据检查位(数组下标从0开始)
* @param[out]：NA
* @author:     贺宁
* @return：     =TRUE  有效     ，=FALSE 无效
*********************************************************************
*/
void set_bit_value(uint8 *BS,uint16 bitStringlen,uint16 bitpos)
{
   uint8  byte_no;
   if(bitpos > 7)
   {
         byte_no = bitpos >> 3;
         BS += byte_no;
         if(bitStringlen<=byte_no) return;

         bitpos &= 0x07;
   }
   (*BS) |=   (01<<bitpos);
}

/**
*********************************************************************
* @brief：      清空位串某位的值，清空为0
* @param[in]： rsv           数据标志位
               bitStringlen 标志位长度
               bitpos       数据检查位(数组下标从0开始)
* @param[out]：NA
* @author:     贺宁
* @return：     =TRUE  有效     ，=FALSE 无效
*********************************************************************
*/
void  clr_bit_value(uint8 *BS,uint16 bitStringlen,uint16 bitpos)
{
   uint8  byte_no;
   if(bitpos > 7)
   {
         byte_no = bitpos >> 3;
         BS += byte_no;
         if(bitStringlen<=byte_no) return;

         bitpos &= 0x07;
   }
   (*BS) &=   ~(01<<bitpos);
}

/**
*********************************************************************
* @brief：      获取位串某位的值
* @param[in]： rsv           数据标志位
               bitStringlen 标志位长度
               bitpos       数据检查位(数组下标从0开始)
* @param[out]：NA
* @author:     贺宁
* @return：     =TRUE  有效     ，=FALSE 无效
*********************************************************************
*/
uint8 get_bit_value(uint8 *BS,uint16 bitStringlen,uint16 bitpos)
{
    uint8  byte_no;

    if(bitStringlen<=bitpos) 
    {
        return 0;
    }
        
    if(bitpos > 7)
    {
        byte_no = bitpos >> 3;
        BS += byte_no;
        if((bitStringlen>>3)<=byte_no) return 0;

        bitpos &= 0x07;
    }
    return (*BS) & (01<<bitpos);

}

/**
*********************************************************************
* @brief：      获取秒数
* @param[in]： rsv           数据标志位
               bitStringlen 标志位长度
               bitpos       数据检查位(数组下标从0开始)
* @param[out]：NA
* @author:     贺宁
* @return：     =TRUE  有效     ，=FALSE 无效
*********************************************************************
*/
time_t get_time(TIME_TYPE_E type)
{
    struct timespec  curtime = {0};
    
    if(type == TIME_REALTIME)
    {
        clock_gettime(CLOCK_REALTIME, &curtime);

    }
    else if(type == TIME_MONOTONIC)
    {
        clock_gettime(CLOCK_MONOTONIC, &curtime);
    }

    return curtime.tv_sec;
}

void check_endian(void)
{
    FILE *fp = NULL;
    int a = 0x1234;

    fp = fopen("/var/run/endian","w+");
    
    char *p = (char*)(&a);

    if(0x34 == *p)
    {
        fprintf(fp, "litte endian: 0x%x store \naddress %p -- value 0x%x\naddress %p -- value 0x%x\n", a, &p[0], p[0], &p[1], p[1]);
    }
    else
    {
        fprintf(fp, "big endian: 0x%x store \naddress %p -- value 0x%x\naddress %p -- value 0x%x\n", a, &p[0], p[0], &p[1], p[1]);
    }

    fclose(fp);

    return;
}

#endif

#if DESC("转码函数",1)

/*-把带符号的原码去掉符号位-*/
int Int_ABS(int value)
{
    int val;
    val = value&0x7fffffff;
    return val;
}
/*-把带符号的原码去掉符号位-*/
uint16 Int16_ABS(uint16 value)
{
    uint16 val;
    val = (value&0x7fff);
    return val;
}
/*-把long long转化为n位的bcd码-*/
void longlong_change_to_BCD(uint8 *dec,uint8 n,uint64 src)
{
    uint8 tmp=0,i;

    if((src == 0xffffff)&&( n == 6) )
    {
        dec[0]=APP_NONECHAR;
        dec[1]=APP_NONECHAR;
        dec[2]=APP_NONECHAR;
        return;
    }
    if((n == 4)&&(src == 0xffff))
    {
        dec[0]=APP_NONECHAR;
        dec[1]=APP_NONECHAR;
        return;
    }
    if( ( src == 0xffffffff)&&( n == 8) )
    {
        dec[0]=APP_NONECHAR;
        dec[1]=APP_NONECHAR;
        dec[2]=APP_NONECHAR;
        dec[3]=APP_NONECHAR;
        return;
    }
    if( ( src == 0xffffffffffLL)&&( n == 10) )
    {
        dec[0]=APP_NONECHAR;
        dec[1]=APP_NONECHAR;
        dec[2]=APP_NONECHAR;
        dec[3]=APP_NONECHAR;
        dec[4]=APP_NONECHAR;
        return;
    }
    if(n<=0) return;
    if(n>20) return;
    for(i=1;i<=n;i++)
    {
        tmp>>=4;
        tmp+=(src%10)<<4;
        src/=10;
        if((i%2)!=0)
            continue;
        *dec++=tmp;
        tmp=0;
    }
    if((n%2)!=0) *dec=(tmp>>4);
}

void hexchangetodec(uint16 hex,uint32 *dec)
{  
     int re=0;   // 保存转换为10进制的结果
     int k=16;   // 16进制
     int n=1;    // 位权
     while(hex != 0)  
     {
         re += (hex%10)*n;  // 取出各位位码值，并乘以对应的位权值
         hex /= 10;   // 去掉16进制数的最低位，次低位变为最低位
         n *= k;     // 位权乘以16
     }
     *dec=re; // 输出转换后的结果
}

/*-把long转化为n位的bcd码-*/
void long_change_to_BCD(uint8 *dec,uint8 n,uint32 src)
{
    uint8 tmp=0,i;

    if((src == 0xffffffff)&&( n == 6) )
    {
        dec[0]=APP_NONECHAR;
        dec[1]=APP_NONECHAR;
        dec[2]=APP_NONECHAR;
        return;
    }
    if( (n == 4) && (src == 0xffff))
    {
        dec[0]=APP_NONECHAR;
        dec[1]=APP_NONECHAR;
        return;
    }
    if( ( src == 0xffffffff)&&( n == 8) )
    {
        dec[0]=APP_NONECHAR;
        dec[1]=APP_NONECHAR;
        dec[2]=APP_NONECHAR;
        dec[3]=APP_NONECHAR;
        return;
    }
    if(n<=0) return;
    if(n>20) return;
    for(i=1;i<=n;i++)
    {
        tmp>>=4;
        tmp+=((uint32)src%10)<<4;
        src/=10;
        if((i%2)!=0) continue;
        *dec++=tmp;
        tmp=0;
    }
    if((n%2)!=0) *dec=(tmp>>4);
}

/*
******************************************************
**      把n位的bcd码转化为long(32b)  一个字节两位  **
******************************************************
*/
uint32  BCD_change_to_long(uint8 *src,uint8 n)
{
    uint8 i;
    uint32 tmp=0;

    if(n>20)
        return 0;
    if(n<=0)
        return 0;

    for(i=n;i>0;i--)
    {
        tmp*=10;
        if(i%2)
            tmp+=((*(src+((i-1)/2)))&0xf);
        else
            tmp+=((*(src+((i-1)/2)))>>4);
    }
    return tmp;
}

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
    uint8 c;
    uint8 a;

    odd = len&0x01;
    if ( odd && !fmt)
        *bcd++ = (*asc++) & '\x0F';

    len >>= 1;

    for (i=0; i<len; i++)
    {
        if(*asc >= 'A' && *asc <= 'F')
        {
            c = *asc - 0x37; 
        }
        else if(*asc >= 'a' && *asc <= 'f')
        {
            c  = *asc - 0x57;
        }
        else
        {
            c  = *asc - 0x30;
        }
        //printf("1 = %x, %x\n", *asc, c);
        c = c << 4;
        //printf("2 = %x\n", c);
        asc++;

        if(*asc >= 'A' && *asc <= 'F')
        {
            a = *asc - 0x37;
        }
        else if(*asc >= 'a' && *asc <= 'f')
        {
            a  = *asc - 0x57;
        }
        else
        {
            a  = *asc - 0x30;
        }
        //printf("3 = %x, %x\n", *asc, c);
        c  |= a;
        //printf("4 = %x\n", c);
        asc++;

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


#if DESC("日期时间转换函数",1)

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
    return FastDateToSecond(&des_tm);//从1970年1月1日0时0分0秒到此时的秒数
}

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

/**********************************************************************
* @brief：      字符串转OOP短时间格式
* @param[in] ：char *typestr       数据类型字符串
* @param[out]: pTime     短时间格式
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-10-8
* @Update    : 
**********************************************************************/
int DT_String2Struct(char *timestr, OOP_DATETIME_S_T *pTime)
{
    int a[6] = {0};
    if(NULL == timestr || NULL == pTime)
    {
        return -1;
    }
    sscanf(timestr, "%d-%d-%d %d:%d:%d", &(a[0]), &(a[1]), &(a[2]), &(a[3]), &(a[4]),&(a[5]));
    pTime->year  = a[0];
    pTime->month = a[1];
    pTime->day   = a[2];
    pTime->hour  = a[3];
    pTime->minute   = a[4];
    pTime->second   = a[5];
    return 0;
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

    //*linuxTim = mktime(&ptm);
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
    struct  tm systime;
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

    //*linuxTime = mktime(&ptm);
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
    struct  tm systime;
    struct  timeval tv;
    struct  timezone tz;
    gettimeofday(&tv, &tz);
    
    tv.tv_sec = linuxTime;
    FastSecondToDate(&tv, &tz, &systime);


    dateTimeBCD->year_h = (systime.tm_year+1900)%10000/1000*16+(systime.tm_year+1900)%1000/100;
    dateTimeBCD->year_l =(systime.tm_year+1900)%100/10*16+(systime.tm_year+1900)%10;
    dateTimeBCD->month =(systime.tm_mon+1)%100/10*16+(systime.tm_mon+1)%10;
    dateTimeBCD->day =(systime.tm_mday)%100/10*16+(systime.tm_mday)%10;
    dateTimeBCD->hour =(systime.tm_hour)%100/10*16+(systime.tm_hour)%10;
    dateTimeBCD->min =(systime.tm_min)%100/10*16+(systime.tm_min)%10;
    dateTimeBCD->sec =(systime.tm_sec)%100/10*16+(systime.tm_sec)%10;
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
* @brief：     BCD编码时间格式转换为OOP短格式
* @param[in]： dateTimeBCD     BCD编码时间格式
* @param[out]：dateTimeHex     HEX编码时间格式
* @return：    转换成功
*********************************************************************
*/
BOOL DT_DateTimeBCD2OOPS(DateTimeBcd_t *dateTimeBCD,OOP_DATETIME_S_T *oopDateTime)
{
    uint16 year;
    if(dateTimeBCD == NULL)
        return FALSE;
    if(FALSE == DT_DateTimeBCDIsValid(dateTimeBCD))
        return FALSE;
    year = commfun_BcdToHex(dateTimeBCD->year_h);
    year *= 100;
    year += commfun_BcdToHex(dateTimeBCD->year_l);

    oopDateTime->year = year;
    oopDateTime->month =(uint8)commfun_BcdToHex(dateTimeBCD->month);
    oopDateTime->day =(uint8)commfun_BcdToHex(dateTimeBCD->day);
    oopDateTime->hour =(uint8)commfun_BcdToHex(dateTimeBCD->hour);
    oopDateTime->minute =(uint8)commfun_BcdToHex(dateTimeBCD->min);
    oopDateTime->second =(uint8)commfun_BcdToHex(dateTimeBCD->sec);
    
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

/*-完成从rtc结构到长整数的转化，2000年1月1日0:0:0开始至今的秒数-*/
uint32 RTC_To_Sec(rtc_t *prtc)
{
    uint32 time;
    uint16 i;
    const unsigned int MonTab1[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    time = (commfun_HexToBcd(prtc->yy) / 4) * FOURYEARS +  (commfun_HexToBcd(prtc->yy) % 4) * 365;
    //  days in four-year blocks */                 //  days in normal years

    /*  If we're past the leap year in the four-year block or if
     *  we're past February then account for leap day
     */
    if ((commfun_HexToBcd(prtc->yy) % 4) != 0 || commfun_HexToBcd(prtc->mm) > 2)
        time++;
    //  (time) is the number of days to the beginning of this year
    for (i = 1; i < commfun_HexToBcd(prtc->mm); i++)
    {
        time += MonTab1[i - 1];
    }
    //  (time) is the number of days to the beginning of this month

    time += commfun_HexToBcd(prtc->dd) - 1;
//    return ((time * 24 + commfun_HexToBcd(prtc->hh)) * 60 + commfun_HexToBcd(prtc->nn)) * 60 + commfun_HexToBcd(prtc->ss) + _70_to_80_bias;
    return ((time * 24 + commfun_HexToBcd(prtc->hh)) * 60 + commfun_HexToBcd(prtc->nn)) * 60 + commfun_HexToBcd(prtc->ss) ;
}


/*-完成从长整数到rtc结构的转化-*/
void Sec_To_Rtc(uint32 time, rtc_t *prtc)
{
    const unsigned int MonTab[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    {//-时分秒-
       uint32 secs = time % (SECS_IN_HALFDAY * 2);
       uint16 usTmp;
       uint16 usTmp2;

       usTmp = ((uint16) (secs / 2)) / 1800;
       prtc->hh = commfun_HexToBcd(usTmp);

       usTmp = (uint16) (secs - (uint32) usTmp * (uint32) (60 * 60));

       usTmp2 = usTmp / 60;
       prtc->nn = commfun_HexToBcd(usTmp2);

       prtc->ss = commfun_HexToBcd(usTmp - (usTmp2 * 60));
    }

    {//-周年月日-
    uint32 days = time / (SECS_IN_HALFDAY * 2);
    uint16 i, j;
    uint16 usDays;

    prtc->ww = commfun_HexToBcd((2+days)%7);    //- 1980/1/1 is Tuesday-

//  i = (uint16) (days / FOURYEARS) * 4;
    i = (uint16) (days / FOURYEARS) ;

    prtc->yy = commfun_HexToBcd((uint16)(i*4));

    usDays = (uint16) (days - (uint32) i * (uint32) FOURYEARS);

    /*
     * Since usDays must be between 0 and FOURYEARS (sum of days in 4
     * years), then we shouldn't need to do the mod in the loop below.
     */
    i = 0;
    while (usDays >= YrTab[i]) {
        usDays -= YrTab[i];

        // i = (i + 1) % 4;
        i++;

    }

    /*  (days) = offset of days into year
     *  (i) = offset into 4-year period
     */
    //prtc->yy += i;
    prtc->yy = commfun_HexToBcd(commfun_HexToBcd(prtc->yy)+i);

    j = 0;
    if (i == 0)
        while (usDays >= MonTabLeap[j])
        usDays -= MonTabLeap[j++];
    else
        while (usDays >= MonTab[j])
        usDays -= MonTab[j++];

    /*  (usDays) = offset of days into month
     *  (j) = month of year - 1
     */
    prtc->mm = commfun_HexToBcd(j + 1);

    prtc->dd = commfun_HexToBcd(usDays + 1);
    }

}

void DT_Time2RTC(time_t time,rtc_t *prtc)
{
    
    struct tm *tm_time;
    
    tm_time = localtime(&time);

    prtc->ss = commfun_HexToBcd(tm_time->tm_sec);
    prtc->nn = commfun_HexToBcd(tm_time->tm_min);
    prtc->hh = commfun_HexToBcd(tm_time->tm_hour);
    prtc->dd = commfun_HexToBcd(tm_time->tm_mday);
    prtc->mm = commfun_HexToBcd(tm_time->tm_mon+1);
    prtc->yy = commfun_HexToBcd((tm_time->tm_year)%100);
    prtc->ww = commfun_HexToBcd(tm_time->tm_wday);
    
}

/*-*********************************************************
* 函数名称：DT_RTC_Interval
* 功能描述：计算时间间隔
* 输入描述：2个时钟
* 输出描述：无
* 返回值：  两个时间中经历过的秒数
* 作   者：
* 创建日期：
* 修改记录：无
* 修改人  ：无
* 修改描述：无
* 修改日期：无
*********************************************************-*/
uint32 DT_RTC_Interval(rtc_t *ptrF, rtc_t *ptrL)
{
    uint32 wDelay;
    uint32 T1;
    uint32 T2;

    T1 = RTC_To_Sec(ptrL);
    T2 = RTC_To_Sec(ptrF);
    if(T1>T2)
    {
        wDelay = T1-T2;
    }
    else
    {
        wDelay = T2-T1;
    }
    return wDelay;
}

#endif

/*******************************************************************************
* 函数名称: checksum_calc
* 函数功能: 计算累加和
* 输入参数: DataLen  数据长度
*           SrcData  数据缓冲区
* 输出参数: 无
* 返 回 值: 累加和
*******************************************************************************/
uint8 checksum_calc(uint8 *SrcData, int DataLen)
{
    int     i  = 0;
    uint8   CS = 0;

    for (i = 0; i < DataLen; i++)
    {
        CS += SrcData[i];
    }

    return CS;
}

/*******************************************************************************
* 函数名称: cmd_exec
* 函数功能: 执行shell命令 返回
* 输入参数: DataLen  数据长度
*           SrcData  数据缓冲区
* 输出参数: 无
* 返 回 值: 累加和
*******************************************************************************/

char* cmd_exec(const char* cmd, char* buff, int size)
{
    char temp[256];
    FILE* fp = NULL;
    int offset = 0;
    int len;

    fp = popen(cmd, "r");
    if(fp == NULL)
    {
        return NULL;
    }

    while(fgets(temp, sizeof(temp), fp) != NULL)
    {
        len = strlen(temp);
        if(offset + len < size)
        {
            strcpy(buff+offset, temp);
            offset += len;
        }
        else
        {
            buff[offset] = 0;
            break;
        }
    }

    if(fp != NULL)
    {
        pclose(fp);
    }

    return buff;
}

/*******************************************************************************
* 函数名称: app_indocker_check
* 函数功能: 检查app是否运行在容器里
* 输入参数:  
* 输出参数:  
* 返 回 值:  TRUE  在容器里 FALSE 运行在主机 
*******************************************************************************/
BOOL app_indocker_check()
{
    char* cmd = "ifconfig |grep 'FE0       '";
    char result[1024] = {0};
    cmd_exec(cmd,result,sizeof(result));
    if(strlen(result) == 0)
        return TRUE;
    else
        return FALSE;
}

/*******************************************************************************
* 函数名称: wildcard_remove
* 函数功能: 支持通配符的remove函数
* 输入参数: filename  文件路径（可以带通配）
* 输出参数: 无
* 返 回 值: 累加和
*******************************************************************************/
int wildcard_remove(char *filename)
{
    int   ret = 0;
    DIR           *dirp;
    struct dirent *entp;
    char tmppath[512] = {0};
    char tmpname[128] = {0};
    char relpath[512] = {0};
    char subpath[512] = {0};
    struct stat fileinfo = {0};

    //从文件路径中获取目录路径
    file_path_extract(filename, tmppath);

    //去掉通配符的文件路径
    strcpy_s(tmpname, 128, filename+strlen(tmppath));
    if(tmpname[strlen(tmpname)-1] == '*')
    {
        tmpname[strlen(tmpname)-1] = 0;
    }
    
    sprintf(subpath, "%s%s", tmppath, tmpname);
    
    //打开目录
    if ((dirp = opendir(tmppath)) != NULL)
    {
        while ((entp = readdir(dirp)) != NULL)
        {
            if((strcmp(entp->d_name, ".") == 0) || (strcmp(entp->d_name, "..") == 0))
            {
                continue;
            }

            sprintf(relpath, "%s%s", tmppath, entp->d_name);

            stat(relpath, &fileinfo);

            if((!S_ISDIR(fileinfo.st_mode))&&(strstr(relpath, subpath) != NULL))
            {
                ret = remove(relpath);
                if(ret != 0)
                {
                    PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", relpath, ret, strerror(errno));
                }
                else
                {
                    PRTL_FMT_DEBUG("remove \"%s\" success.\n", relpath);
                }
            }
        }
        
        closedir(dirp);
    }

    return ret;
}

