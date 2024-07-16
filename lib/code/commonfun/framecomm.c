/*
*********************************************************************
* @file    framecomm.c
* @brief�� ��������
* @author��all
* @date��   2019.12.24
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/

#include "framecomm.h"
//#include "zlog.h"
#include "iniparser.h"
#include <net/if.h>    

uint32 gdelaytime=0;      //��ʱʱ��


#define SECS_IN_HALFDAY 0x0A8C0L
#define FOURYEARS   (3*365+366)
const unsigned int MonTabLeap[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const unsigned int YrTab[] = { 366, 365, 365, 365 };
uint8              g_HasRealtz_min = FALSE;
int                g_Realtz_min = 0;

/* ��ȡʵ��ʱ�� */
int TIME_GET_REAL_TZ()
{
    FILE *fp;
    char ucSystem[256] = {0};
    char timeshow[256] = {0};
    int    tz_min = 0;
    
    /* ��route -n�л�ȡ */
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

/* localtime������������ ʹ�� FastSecondToDate �滻 */
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

    //ϵͳĬ�ϱ���ʱ������ȡ����ʱ����Ϣʱ���ֶ�����
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

    /*��ķ����ɭ���㹫ʽ �������ڼ� 0-��ʾ����*/
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

/* localtime_r������������ ʹ�� LOCALTIME_R �滻 */
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

/* mktime������������ ʹ�� FastDateToSecond �滻 */
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

    //ϵͳĬ�ϱ���ʱ������ȡ����ʱ����Ϣʱ���ֶ�����
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
* @brief     ����ȡ���ڵ�IP��ַ
* @param[in] ��char *ifname ��������
* @param[out]��
* @return    ��IP��ַ
* @Create    :
* @Date      ��2020-6-30
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
            /*��ȡ�ӿ�����*/
            intrface = ifc.ifc_len / sizeof (struct ifreq);

            /*����PPPλ��*/
            while ( (strcmp(buf[intrface].ifr_name, ifname)) && (intrface >= 0) )
                intrface--;

            if (intrface >= 0)
            {
                /*��ȡ��ǰ������IP��ַ*/ 
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


#if DESC("�ַ�������", 1)

/**
*********************************************************************
* @brief:       �ַ����ָ�
* @param[in]��   src     Դ�ַ������׵�ַ
* @param[in]��   delim   �ָ��ַ���
* @param[out]:  dest    �ָ����ַ�����
* @param[out]:  num     �ָ����ַ�������
* @author:      ����
* @Date��        2020-05-22
* @return��      void
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
* @brief:       �ַ���ȫתΪСд
* @param[in]��   src     Դ�ַ������׵�ַ
* @author:      ����
* @Date��        2020-01-20
* @return��      void
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
* @brief:       �ַ���ȫתΪСд
* @param[in]��   src     Դ�ַ������׵�ַ
                 len     ���볤��
* @author:      ����
* @Date��        2020-01-20
* @return��      void
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
* @brief:       �ַ���ȫתΪСд
* @param[in]��   src     Դ�ַ������׵�ַ
* @author:      ����
* @Date��        2020-01-20
* @return��      void
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
* @brief:       �ַ���ȫתΪСд
* @param[in]��   src     Դ�ַ������׵�ַ
                 len     ���볤��
* @author:      ����
* @Date��        2020-01-20
* @return��      void
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
* @brief:       �ַ���תΪ�ֽڴ����ַ�����Χ0~9,a~f,A~F
*               �����ַ������һ���ֽ�ǰ�油0
* @param[in]��   str      �ַ���
* @param[in]��   strLen   �ַ�������
* @param[out]�� octet     �ֽڴ�
* @param[in]��   octetlen �ֽڴ�����
* @author:      ����
* @Date��        2021-02-01
* @return��      <0 �������� �ֽڴ�����
*********************************************************************
*/
int strt2octet(char *str, int strLen, unsigned char *octet, int octetLen)
{
    int i = 0, len = 0;

    if (str == NULL || octet == NULL)
    {
        return ERR_PNULL;
    }

    //У�������ַ����ĺϷ���
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

    //�ֽڴ�����
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
* @brief:       �ֽڴ�תΪ�ַ������ַ�����Χ0~9,a~f,A~F (16����)
*               �����ַ������һ���ֽ�ǰ�油0
* @param[in]��   str      �ַ���
* @param[in]��   strLen   �ַ�������
* @param[out]�� octet     �ֽڴ�
* @param[in]��   octetlen �ֽڴ�����
* @author:      ����
* @Date��        2021-02-01
* @return��      <0 �������� �ַ�������
*********************************************************************
*/
int octet2str(unsigned char *octet, int octetLen, char *str, int strLen)
{
    int i = 0, len = 0;
    
    if (str == NULL || octet == NULL)
    {
        return ERR_PNULL;
    }

    //�ַ�������
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
* @brief:       ���Ǳ�׼�ַ�����ĩβ����'\0'��ת��ΪC�淶�ַ���
* @param[in]��   str      �ַ���
* @param[in]��   strLen   �ַ�������
* @param[out]�� fmtstr   �淶�ַ���
* @param[in]��   fmtlen   �淶�ַ�������
* @author:      ����
* @Date��        2021-02-01
* @return��      <0 �������� �ַ�������
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

#if DESC("�ļ�����",1)

/*
  ��  ��: rcd_lock
  ��  ��: ��fcntl��ʽʵ�ּ�¼��
  ��  ��:
    fd   - �����ļ����ļ�������
    cmd  - ��������(F_SETLK, F_SETLKW, F_GETLK)
    type - ������(F_RDLCK, F_WRLCK, F_UNLCK)
  ����ֵ:
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
        
    strncpy(path, src, len - frag);  //!������������Զ����ַ���������\0'
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
        
    if( (ptr = next_path_get(ptr, dirName, 128)) != NULL)  //��ø�Ŀ��
    {
        if(!bRelative) // ����������Ǿ���·�������ڴ���Ŀ¼��ǰ��'/'��ʾ����·��
            strcpy(filepath, "/");
        else
            filepath[0] = '\0';
            
        strcat(filepath, dirName);

        if( mkdir(filepath, 0755) < 0)
            if( errno != EEXIST)
                return FALSE;

    }
    else //��Ŀ¼��ȡ����
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
        
    if( S_ISREG(sta.st_mode)) //��ͨ�ķ�
    {
        return sta.st_size;
    }
    else 
        return -1;
}

/**
*********************************************************************
* @brief:       ���ļ�·������ȡ�ļ���
* @param[in]��   src         �����ļ�·��
* @param[in]:   withSubfix  ��ȡ�ļ����Ƿ����׺
* @param[out]��  filename    ��ȡ���ļ���

* @return��      void
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
* @brief:       ���ļ�·������ȡ�ļ���չ��
* @param[in]��   src         �����ļ�·��
* @param[out]��  ext         ��ȡ����չ��

* @return��      void
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

#if DESC("��ȫ����",1)
/**
*********************************************************************
* @brief:       �ڴ濽����������ȫ�汾��
* @detail:      ���count>destsz,��count>SIZE_MAX, dest��destsz������Ч��
*               ������Ŀ�귶Χ[dest, dest + destsz)��ÿ��λ��
*               ����Ϊ��C11��׼��K3.7.1.1����
* @param[out]��  dest        Ŀ�Ļ�����
* @param[in]��   destsz      Ŀ�Ļ�������С
* @param[in]��   src         Դ����������
* @param[in]��   count     Դ����ĳ�ԱԪ�ش�С
* @return��      =0,��ȷ��!=0,����
* @author:      ����
* @Date��        2020-03-30
*********************************************************************
*/
int memcpy_s(void * __restrict dest, size_t destsz, const void * __restrict src, size_t count)
{
    //��ָ��
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //�������
    if(destsz>SIZE_MAX)
    {
        return ERR_O_RANGE;
    }

    //����Խ��
    if(destsz < count)
    {
        memset(dest, 0, destsz);
        return ERR_OPTION;
    }

    //�������
    if(count>SIZE_MAX)
    {
        memset(dest, 0, destsz);
        return ERR_O_RANGE;
    }
    
    //��ָ��
    if(src == NULL)
    {
        memset(dest, 0, destsz);
        return ERR_PNULL;
    }

    //��ַ�ص�
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
* @brief:       �ڴ��ƶ���������ȫ�汾��
* @detail:      ���count>destsz,��count>SIZE_MAX, dest��destsz������Ч��
*               ������Ŀ�귶Χ[dest, dest + destsz)��ÿ��λ��
*               ����Ϊ��C11��׼��K3.7.1.2����
* @param[out]��  dest        Ŀ�Ļ�����
* @param[in]��   destsz      Ŀ�Ļ�������С
* @param[in]��   src         Դ����������
* @param[in]��   count     Դ����ĳ�ԱԪ�ش�С
* @return��      =0,��ȷ��!=0,����
* @author:      ����
* @Date��        2020-03-30
*********************************************************************
*/
int memmove_s(void * dest, size_t destsz, const void * src, size_t count)
{
    //��ָ��
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //�������
    if(destsz>SIZE_MAX)
    {
        return ERR_O_RANGE;
    }

    //����Խ��
    if(destsz < count)
    {
        memset(dest, 0, destsz);
        return ERR_OPTION;
    }

    //�������
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
* @brief:       �ڴ����ú�������ȫ�汾��
* @detail:      ���count>destsz,��count>SIZE_MAX, dest��destsz������Ч��
*               ���ڽ�ch�洢��Ŀ�귶Χ[dest, dest + destsz)��ÿ��λ��
*               ����Ϊ��C11��׼��K3.7.4.1����
* @param[out]��  dest        Ŀ�Ļ�����
* @param[in]��   destsz      Ŀ�Ļ�������С
* @param[in]��   ch          ����ֽ�
* @param[in]��   count       Դ����ĳ�ԱԪ�ش�С
* @return��      =0,��ȷ��!=0,����
* @author:      ����
* @Date��        2020-03-30
*********************************************************************
*/
int memset_s(void * dest, size_t destsz, int ch, size_t count)
{
    //��ָ��
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //�������
    if(destsz>SIZE_MAX)
    {
        return ERR_O_RANGE;
    }

    memset(dest, ch, MIN(destsz, count));

    //�������
    if(count>SIZE_MAX)
    {
        return ERR_O_RANGE;
    }
    
    //����Խ��
    if(destsz < count)
    {
        return ERR_OVERFLOW;
    }
    
    return ERR_OK;
}

/**
*********************************************************************
* @brief:       �ڴ淴ת������������ȫ�汾��
* @detail:      ���count>destsz,��count>SIZE_MAX, dest��destsz������Ч��
*               ������Ŀ�귶Χ[dest, dest + destsz)��ÿ��λ��
* @param[out]��  dest        Ŀ�Ļ�����
* @param[in]��   destsz      Ŀ�Ļ�������С
* @param[in]��   src         Դ����������
* @param[in]��   count     Դ����ĳ�ԱԪ�ش�С
* @return��      =0,��ȷ��!=0,����
* @author:      ����
* @Date��        2020-03-30
*********************************************************************
*/
int memcpy_rs(void * __restrict dest, size_t destsz, const void * __restrict src, size_t count)
{
    //��ָ��
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //�������
    if(destsz>SIZE_MAX)
    {
        return ERR_O_RANGE;
    }

    //����Խ��
    if(destsz < count)
    {
        memset(dest, 0, destsz);
        return ERR_OPTION;
    }

    //�������
    if(count>SIZE_MAX)
    {
        memset(dest, 0, destsz);
        return ERR_O_RANGE;
    }

    //��ָ��
    if(src == NULL)
    {
        memset(dest, 0, destsz);
        return ERR_PNULL;
    }

    //��ַ�ص�
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
* @brief:       �ַ������Ⱥ�������ȫ�汾��
* @detail:      �ɹ�ʱ�Կ��ַ���β���ֽڴ�str�ĳ��ȣ����str�ǿ�ָ���򷵻��㣬
*               �����ǰstrsz���ַ���δ�ҵ����ַ����򷵻�strsz
*               ����Ϊ��C11��׼��K3.7.4.4����
* @param[in]��   str        Ҫ�����Կ��ַ���β���ַ���
* @param[in]��   strsz      ��������ַ�������
* @return��      �ַ�������
* @author:      ����
* @Date��        2020-03-30
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
* @brief:       �ַ���������������ȫ�汾��
* @detail:      �������dest��destsz������Ч������д��dest[0]
                ����Ϊ��C11��׼��K3.7.1.3����
* @param[out]��  dest        Ŀ���ַ�������
* @param[in]��   destsz      Ŀ�Ļ�������С
* @param[in]��   src         Դ�ַ�������
* @return��      =0,��ȷ��!=0,����
* @author:      ����
* @Date��        2020-03-30
*********************************************************************
*/

int strcpy_s(char * __restrict dest, size_t destsz, const char * __restrict src)
{
    int srcsz = strnlen_s(src, destsz);
    
    //��ָ��
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //��������򳤶�Ϊ0
    if((destsz>SIZE_MAX) || (destsz == 0))
    {
        return ERR_O_RANGE;
    }

    //��ָ��
    if(src == NULL)
    {
        dest[0] = '\0';
        return ERR_PNULL;
    }

    //�ض�
    if(destsz <= srcsz)
    {
        dest[0] = '\0';
        return srcsz;
    }

    //��ַ�ص�
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
* @brief:       �ַ���������������ȫ�汾��
* @detail:      �������dest��destsz������Ч������д��dest[0]
                ����Ϊ��C11��׼��K3.7.1.4����
* @param[out]��  dest        Ŀ���ַ�������
* @param[in]��   destsz      Ŀ�Ļ�������С
* @param[in]��   src         Դ�ַ�������
* @param[in]��   count       �������ַ�����
* @return��      =0,��ȷ��!=0,����
* @author:      ����
* @Date��        2020-03-30
*********************************************************************
*/
int strncpy_s(char * __restrict dest, size_t destsz, const char * __restrict src, size_t count)
{
    int srcsz = strnlen_s(src, destsz);
    int srccn = strnlen_s(src, count);
    
    //��ָ��
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //��������򳤶�Ϊ0
    if((destsz>SIZE_MAX) || (destsz == 0))
    {
        return ERR_O_RANGE;
    }

    //�������
    if(count>SIZE_MAX)
    {
        dest[0] = '\0';
        return ERR_O_RANGE;
    }

    //��ָ��
    if(src == NULL)
    {
        dest[0] = '\0';
        return ERR_PNULL;
    }

    //�ض�
    if((count>=destsz)&&(destsz <= srccn))
    {
        dest[0] = '\0';
        return srccn;
    }

    //��ַ�ص�
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
* @brief:       �ַ������Ӻ�������ȫ�汾��
* @detail:      �������dest��destsz������Ч������д��dest[0]
                ����Ϊ��C11��׼��K3.7.2.1����
* @param[out]��  dest        Ŀ���ַ�������
* @param[in]��   destsz      Ŀ�Ļ�������С
* @param[in]��   src         Դ�ַ�������
* @param[in]��   count       �������ַ�����
* @return��      =0,��ȷ��!=0,����
* @author:      ����
* @Date��        2020-03-30
*********************************************************************
*/
int strcat_s(char * __restrict dest, size_t destsz, const char * __restrict src)
{
    int i = 0;
    int srcsz = strnlen_s(src, destsz);

    //��ָ��
    if(dest == NULL)
    {
        return ERR_PNULL;
    }

    //��������򳤶�Ϊ0
    if((destsz>SIZE_MAX) || (destsz == 0))
    {
        return ERR_O_RANGE;
    }

    //��ָ��
    if(src == NULL)
    {
        dest[0] = '\0';
        return ERR_PNULL;
    }

    //dest������ֹ��
    while(dest[i++] != '\0')
    {
        if(i == destsz)
        {
            dest[0] = '\0';
            return ERR_FORMAT;
        }
    }
    
    //�ض�
    if(destsz <= strnlen_s(dest, destsz) + srcsz)
    {
        dest[0] = '\0';
        return srcsz;
    }

    //��ַ�ص�
    if((dest>src)&&(dest<src+srcsz))
    {        
        dest[0] = '\0';
        return ERR_PTR;
    }

    strcat(dest, src);

    return ERR_OK; 
}

#endif

#if DESC("�ڴ��������",1)
/**
*********************************************************************
* @brief:       ����һ������(�ٶ�����С����洢)
* @param[out]��  dstArray    Ŀ������
* @param[in]��   dstStep     Ŀ������ĳ�ԱԪ�ش�С
* @param[out]��  srcArray    Դ����
* @param[in]��   srcStep     Դ����ĳ�ԱԪ�ش�С
* @param[in]��   count       ����Ԫ�ظ���
* @return��      void
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
* @brief:       ��ת�������Ӹ��ֽڿ�ʼ
* @param[out]��  dest      Ŀ�����ݻ�����
* @param[in]��   src       Դ���ݻ�����
* @param[in]��   count     �����ֽ���
* @return��      void*     Ŀ�Ļ�����д��β��
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
* @brief:       ��ת�������Ӹ��ֽڿ�ʼ
* @param[out]��  dest      Ŀ�����ݻ�����
* @param[in]��   src       Դ���ݻ�����
* @param[in]��   count     �����ֽ���
* @return��      void*     Ŀ�Ļ�����д��β��
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
* @brief:       ��ַ����
* @param[out]��  dest      Ŀ�����ݻ�����
* @param[in]��   src       Դ���ݻ�����
* @param[in]��   count     �����ֽ���
* @param[in]��   isRev     �Ƿ�ѡ��ת TRUE ��ת���� FLASE ֱ�ӿ���
* @return��      void*     Ŀ�Ļ�����д��β��
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
* @brief:       ָ�����ȵ��ڴ����ݷ�ת
* @param[out]��  buff:     �ڴ���ڵ�ַ
* @param[in]��   len:      ���ݳ���
* @return��      void*     Ŀ�Ļ�����д��β��
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
* @brief:       ָ�����ȵ��ڴ����ݰ�λ��ת
* @param[out]��  buf       �ڴ���ڵ�ַ
* @param[in]��   buf       �ڴ���ڵ�ַ
* @param[in]��   nBits     ������
* @return��
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
    memrev(buff, byteNum);//�Ȱ��ֽڷ�ת
    for(i = 0; i < byteNum; i++) //��ÿ���ֽڰ�λ��ת
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
* @brief:       memcpy_bitrev ����ָ�����ȵ��ڴ����ݲ���λ��ת
* @param[out]��  buff      Ŀ�����ݻ�����
* @param[in]��   buff      Դ���ݻ�����
* @param[in]��   len       ���ݳ���
* @return��      void      ��
*********************************************************************
*/
void memcpy_bitrev(uint8 *dest, const uint8 *src, int count)
{
    uint16 i;
    uint8  j;
    uint8  tmp;
    
    memcpy_r(dest, src, count);//�ȷ�ת����
        
    for (i = 0; i < count; i++) //��ÿ���ֽڰ�λ��ת
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
* @brief:       ȥ���������ظ�Ԫ��
* @param[in]��   array       Դ����
* @param[in]��   num         ����Ԫ�ظ���
* @param[in]��   size        ÿ��Ԫ�ش�С
* @return��      void
* @author:      ����
* @Date��        2020-03-28
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
* @brief:       ȥ������ͬ�����������ظ�Ԫ�أ�ɾ��array2��array1��ͬ��Ԫ��
* @param[in]��   array1    ��һ������
* @param[in]��   num1      ��һ������Ԫ�ظ���
* @param[in]��   array2    �ڶ�������
* @param[in]��   num2      �ڶ�������Ԫ�ظ���
* @param[in]��   size      ÿ��Ԫ�ش�С
* @return��      void
* @author:      ����
* @Date��        2020-03-28
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
* @brief:       �ж�ֵ�Ƿ������������
* @param[in]��   array     �����׵�ַ
* @param[in]��   num       �����Ա����
* @param[in]��   size      ÿ��Ԫ�ش�С
* @param[in]��   value     Ҫ�жϵ�ֵ
* @return��      =TRUE,FALSE
* @author:      ����
* @Date��        2020-04-12
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

#if DESC("698Э�鹫������",1)

/**
*********************************************************************
* @brief��      �Ƚ�TSA�Ƿ����
* @param[in]:  pstTSA1 ����Ƚϵ�ͨ�ŵ�ַ1
* @param[in]:  pstTSA2 ����Ƚϵ�ͨ�ŵ�ַ2
* @return��     FALSE-����ȣ�TRUE-���
* @author:     ����
* @Date��       2020-03-28
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

#if DESC("ϵͳ����",1)
/**
*********************************************************************
* @brief��      ��ʵ�����
* @param[in]�� NA
* @param[out]��NA
* @author:     ����
* @return��     =TRUE  ���ͨ�������е���ʵ��     ��=FALSE ��鲻ͨ�����Ѿ���ʵ������
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
* @brief��      ������ɱ
* @param[in]�� app����
* @param[out]��NA
* @author:     ����
* @return��     void
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
    static time_t accbaseTime = 0; //�ۻ�ʱ�����ֵ
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

    //��ȡϵͳ���е����ʱ��
    clock_gettime(CLOCK_MONOTONIC, &accsecs);
    wtAccSecs = accsecs.tv_sec;

    if(bfirstRun == TRUE)
    {
        bfirstRun = FALSE;

        //��ȡ����ʱ�����ֵ
        lseek(fd, 0, SEEK_SET);
        ret |= read(fd, &baseTime, sizeof(time_t));

        //��ȡ���ʱ������ֵ
        lseek(fd, sizeof(time_t), SEEK_SET);
        ret |= read(fd, &acclimitTime, sizeof(time_t));

        //��ȡ�ϴε����ʱ��
        lseek(fd, 2*sizeof(time_t), SEEK_SET);
        ret |= read(fd, &rdAccSecs, sizeof(time_t));

        accbaseTime = wtAccSecs;
    }

    //�������ʱ��
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
* @brief��      ���жϵ�˯�ߺ���
* @param[in]�� seconds ����
* @param[out]��NA
* @author:     ����
* @return��     NA
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
* @brief��      ��ȫ����ʱ���������źŵ�ʹʵ����ʱС��Ԥ��
* @param[in]�� s ��ʱ����
* @param[out]��NA
* @author:     ����
* @return��     NA
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
* @brief��      ��ȫ������ʱ���������źŵ�ʹʵ����ʱС��Ԥ��
* @param[in]�� s ��ʱ����
* @param[out]��NA
* @author:     ����
* @return��     NA
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
* @brief��      ��ȫ΢����ʱ���������źŵ�ʹʵ����ʱС��Ԥ��
* @param[in]�� s ��ʱ����
* @param[out]��NA
* @author:     ����
* @return��     NA
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
* @brief��      ����λ��ĳλ��ֵ������Ϊ1
* @param[in]�� rsv           ���ݱ�־λ
               bitStringlen ��־λ����
               bitpos       ���ݼ��λ(�����±��0��ʼ)
* @param[out]��NA
* @author:     ����
* @return��     =TRUE  ��Ч     ��=FALSE ��Ч
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
* @brief��      ���λ��ĳλ��ֵ�����Ϊ0
* @param[in]�� rsv           ���ݱ�־λ
               bitStringlen ��־λ����
               bitpos       ���ݼ��λ(�����±��0��ʼ)
* @param[out]��NA
* @author:     ����
* @return��     =TRUE  ��Ч     ��=FALSE ��Ч
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
* @brief��      ��ȡλ��ĳλ��ֵ
* @param[in]�� rsv           ���ݱ�־λ
               bitStringlen ��־λ����
               bitpos       ���ݼ��λ(�����±��0��ʼ)
* @param[out]��NA
* @author:     ����
* @return��     =TRUE  ��Ч     ��=FALSE ��Ч
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
* @brief��      ��ȡ����
* @param[in]�� rsv           ���ݱ�־λ
               bitStringlen ��־λ����
               bitpos       ���ݼ��λ(�����±��0��ʼ)
* @param[out]��NA
* @author:     ����
* @return��     =TRUE  ��Ч     ��=FALSE ��Ч
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

#if DESC("ת�뺯��",1)

/*-�Ѵ����ŵ�ԭ��ȥ������λ-*/
int Int_ABS(int value)
{
    int val;
    val = value&0x7fffffff;
    return val;
}
/*-�Ѵ����ŵ�ԭ��ȥ������λ-*/
uint16 Int16_ABS(uint16 value)
{
    uint16 val;
    val = (value&0x7fff);
    return val;
}
/*-��long longת��Ϊnλ��bcd��-*/
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
     int re=0;   // ����ת��Ϊ10���ƵĽ��
     int k=16;   // 16����
     int n=1;    // λȨ
     while(hex != 0)  
     {
         re += (hex%10)*n;  // ȡ����λλ��ֵ�������Զ�Ӧ��λȨֵ
         hex /= 10;   // ȥ��16�����������λ���ε�λ��Ϊ���λ
         n *= k;     // λȨ����16
     }
     *dec=re; // ���ת����Ľ��
}

/*-��longת��Ϊnλ��bcd��-*/
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
**      ��nλ��bcd��ת��Ϊlong(32b)  һ���ֽ���λ  **
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


#if DESC("����ʱ��ת������",1)

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
    return FastDateToSecond(&des_tm);//��1970��1��1��0ʱ0��0�뵽��ʱ������
}

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

/**********************************************************************
* @brief��      �ַ���תOOP��ʱ���ʽ
* @param[in] ��char *typestr       ���������ַ���
* @param[out]: pTime     ��ʱ���ʽ
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-10-8
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

    //*linuxTim = mktime(&ptm);
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

    //*linuxTime = mktime(&ptm);
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
* @brief��     BCD����ʱ���ʽת��ΪOOP�̸�ʽ
* @param[in]�� dateTimeBCD     BCD����ʱ���ʽ
* @param[out]��dateTimeHex     HEX����ʱ���ʽ
* @return��    ת���ɹ�
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

/*-��ɴ�rtc�ṹ����������ת����2000��1��1��0:0:0��ʼ���������-*/
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


/*-��ɴӳ�������rtc�ṹ��ת��-*/
void Sec_To_Rtc(uint32 time, rtc_t *prtc)
{
    const unsigned int MonTab[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    {//-ʱ����-
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

    {//-��������-
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
* �������ƣ�DT_RTC_Interval
* ��������������ʱ����
* ����������2��ʱ��
* �����������
* ����ֵ��  ����ʱ���о�����������
* ��   �ߣ�
* �������ڣ�
* �޸ļ�¼����
* �޸���  ����
* �޸���������
* �޸����ڣ���
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
* ��������: checksum_calc
* ��������: �����ۼӺ�
* �������: DataLen  ���ݳ���
*           SrcData  ���ݻ�����
* �������: ��
* �� �� ֵ: �ۼӺ�
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
* ��������: cmd_exec
* ��������: ִ��shell���� ����
* �������: DataLen  ���ݳ���
*           SrcData  ���ݻ�����
* �������: ��
* �� �� ֵ: �ۼӺ�
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
* ��������: app_indocker_check
* ��������: ���app�Ƿ�������������
* �������:  
* �������:  
* �� �� ֵ:  TRUE  �������� FALSE ���������� 
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
* ��������: wildcard_remove
* ��������: ֧��ͨ�����remove����
* �������: filename  �ļ�·�������Դ�ͨ�䣩
* �������: ��
* �� �� ֵ: �ۼӺ�
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

    //���ļ�·���л�ȡĿ¼·��
    file_path_extract(filename, tmppath);

    //ȥ��ͨ������ļ�·��
    strcpy_s(tmpname, 128, filename+strlen(tmppath));
    if(tmpname[strlen(tmpname)-1] == '*')
    {
        tmpname[strlen(tmpname)-1] = 0;
    }
    
    sprintf(subpath, "%s%s", tmppath, tmpname);
    
    //��Ŀ¼
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

