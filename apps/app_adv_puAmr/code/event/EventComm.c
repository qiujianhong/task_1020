/*-***************************************************************
* Copyright (c) 2006,�Ͼ����������������޹�˾
*            All rights reserved.
* �������ƣ�eventcomm.c
* �汾�ţ�1.0
* ���ܣ�ʵ���¼��ļ���������ϱ�����
* ԭʼ�����˼�ʱ�䣺÷���� 2020��04��13��
* �޸���Ա��ʱ�估�޸�����:��
* ������
******************************************************************-*/
#include "framecomm.h"
#include "EventComm.h"


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


#if DESC("ʱ��ת��",1)

/**********************************************************************
* @name      : systm_to_ooptm
* @brief     ��ϵͳʱ������ת��ΪOOP_DATETIME_S_T
* @param[in] ��time_t              sys_tm            ϵͳʱ��
* @param[out]��OOP_DATETIME_S_T   *poop_time      
* @return    ��
* @Create    : ÷����
* @Date      ��2020-4-13
* @Update    :
**********************************************************************/
void systm_to_ooptm(time_t sys_tm, OOP_DATETIME_S_T *poop_time)
{
    if(poop_time ==NULL)
        return;
    
    struct tm systime;
    struct  timeval tv;
    struct  timezone tz;
    gettimeofday(&tv, &tz);
    
    tv.tv_sec = sys_tm;
    FastSecondToDate(&tv, &tz, &systime);

    
    poop_time->year = systime.tm_year+1900; 
    poop_time->month = systime.tm_mon+1;
    poop_time->day = systime.tm_mday;
    poop_time->hour = systime.tm_hour;
    poop_time->minute = systime.tm_min;
    poop_time->second = systime.tm_sec;
    
    return;
}

/**********************************************************************
* @name      : rtctm_to_ooptm
* @brief     ��ʱ���ʽת�� rtc_tת��ΪOOP_DATETIME_S_T
* @param[in] ��rtc_t rtc_tm 
* @param[out]��OOP_DATETIME_S_T *poop_time
* @return    ��
* @Create    : ÷����
* @Date      ��2020-4-13
* @Update    :
**********************************************************************/
void rtctm_to_ooptm(rtc_t rtc_tm, OOP_DATETIME_S_T *poop_time)
{
    if(poop_time ==NULL)
        return;
    
    poop_time->year = 2000+commfun_BcdToHex(rtc_tm.yy);
    poop_time->month = commfun_BcdToHex(rtc_tm.mm);
    poop_time->day = commfun_BcdToHex(rtc_tm.dd);
    poop_time->hour = commfun_BcdToHex(rtc_tm.hh);
    poop_time->minute = commfun_BcdToHex(rtc_tm.nn);
    poop_time->second = commfun_BcdToHex(rtc_tm.ss);
    return;
}

/**********************************************************************
* @name      : ooptm_to_rcttm
* @brief     ��ʱ���ʽת�� OOP_DATETIME_S_Tת��Ϊrtc_t
* @param[in] ��OOP_DATETIME_S_T oop_time
* @param[out]��rtc_t *prtc_tm     
* @return    ��
* @Create    : ÷����
* @Date      ��2019-12-22
* @Update    :
**********************************************************************/
void ooptm_to_rcttm(OOP_DATETIME_S_T oop_time,rtc_t *prtc_tm)
{
    if(prtc_tm ==NULL)
        return;

    prtc_tm->yy =(uint8)commfun_HexToBcd(oop_time.year%100);
    prtc_tm->mm =(uint8)commfun_HexToBcd(oop_time.month);
    prtc_tm->dd =(uint8)commfun_HexToBcd(oop_time.day);
    prtc_tm->hh =(uint8)commfun_HexToBcd(oop_time.hour);
    prtc_tm->nn =(uint8)commfun_HexToBcd(oop_time.minute);
    prtc_tm->ss =(uint8)commfun_HexToBcd(oop_time.second);
    return ;
}

/**********************************************************************
* @name      : ooptm_to_systm
* @brief     ��OOP_DATETIME_S_Tת��Ϊϵͳʱ������
* @param[in] ��OOP_DATETIME_S_T oop_time 
* @param[out]��time_t *psys_tm        ϵͳʱ��    
* @return    ��
* @Create    : ÷����
* @Date      ��2019-12-22
* @Update    :
**********************************************************************/
void ooptm_to_systm(OOP_DATETIME_S_T oop_time, time_t *psys_tm)
{
    struct tm ptm;
    
    if(psys_tm == NULL)
    {
        return ;
    }
    
    ptm.tm_year = oop_time.year-1900;
    ptm.tm_mon = oop_time.month-1;
    ptm.tm_mday = oop_time.day;
    ptm.tm_hour = oop_time.hour;
    ptm.tm_min = oop_time.minute;
    ptm.tm_sec = oop_time.second;

    *psys_tm = FastDateToSecond(&ptm);
    return;
}

/**********************************************************************
* @name      : systm_to_ooptm
* @brief     ��ϵͳʱ������ת��Ϊrtc_t
* @param[in] ��time_t              sys_tm            ϵͳʱ��
* @param[out]��rtc_t *prtc_tm     
* @return    ��
* @Create    : ÷����
* @Date      ��2020-4-13
* @Update    :
**********************************************************************/
void systm_to_rtctm(time_t sys_tm, rtc_t *prtc_tm)
{/*
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

    *//*
     * Since usDays must be between 0 and FOURYEARS (sum of days in 4
     * years), then we shouldn't need to do the mod in the loop below.
     *//*
    i = 0;
    while (usDays >= YrTab[i]) {
        usDays -= YrTab[i];

        // i = (i + 1) % 4;
        i++;

    }

    *//*  (days) = offset of days into year
     *  (i) = offset into 4-year period
     *//*
    //prtc->yy += i;
    prtc->yy = commfun_HexToBcd(commfun_HexToBcd(prtc->yy)+i);

    j = 0;
    if (i == 0)
        while (usDays >= MonTabLeap[j])
        usDays -= MonTabLeap[j++];
    else
        while (usDays >= MonTab[j])
        usDays -= MonTab[j++];

    *//*  (usDays) = offset of days into month
     *  (j) = month of year - 1
     *//*
    prtc->mm = commfun_HexToBcd(j + 1);

    prtc->dd = commfun_HexToBcd(usDays + 1);
    }
*/
}

/**********************************************************************
* @name      : current_tm_to_rtctm
* @brief     ����ȡ��ǰrtc��ʽʱ��
* @param[in] ��
* @param[out]��rtc_t *prtc_tm
* @return    ��
* @Create    : ÷����
* @Date      ��2020-4-13
* @Update    :
**********************************************************************/
void current_tm_to_rtctm(rtc_t *prtc_tm)
{
    time_t timep;
    time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep
//    Sec_To_Rtc(timep, prtc_tm);
    return;
}

/**********************************************************************
* @name      : current_tm_to_ooptm
* @brief     ����ȡ��ǰOOP_DATETIME_S_T��ʽʱ��
* @param[in] ��
* @param[out]��OOP_DATETIME_S_T *poop_time      
* @return    ��
* @Create    : ÷����
* @Date      ��2020-4-13
* @Update    :
**********************************************************************/
void current_tm_to_ooptm(OOP_DATETIME_S_T *poop_time)
{
    if(poop_time ==NULL)
        return;
    
    time_t timep;
    time(&timep);
    systm_to_ooptm(timep, poop_time);
    return;
}

#endif

#if DESC("OOP_TSA_T ��ַ�Ƚ�",1)
/**********************************************************************
* @name      : CompareTSA
* @brief     ���Ƚ�����OOP_TSA_T ��ַ�Ƿ���ͬ
* @param[in] ��OOP_TSA_T tsa1
               OOP_TSA_T tsa2
* @param[out]��
* @return    ��0��ͬ/<0����ͬ
* @Create    : ÷����
* @Date      ��2020-1-7
* @Update    :
**********************************************************************/
int CompareTSA(OOP_TSA_T tsa1, OOP_TSA_T tsa2)
{
    int i;
    if(tsa1.af != tsa2.af)
        return -1;
    
    for(i=0; i<=tsa1.len; i++)
    {
        if(tsa1.add[i] != tsa2.add[i])
            return -1;
    }
    return 0;
}
#endif

/**********************************************************************
* @name      : event_data_is_null
* @brief     ���жϻ����Ƿ��
* @param[in] ��data ����
               len  ����
* @param[out]��
* @return    ��TRUE - ��
**********************************************************************/
BOOL event_data_is_null(void* data, uint32 len)
{
    uint32 i = 0;
    char* pDate = (char*)data;

    if (data == NULL)
    {
        return TRUE;
    }

    for (i = 0; i < len; i++)
    {
        if (pDate[i] != 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}


/**********************************************************************
* @name      : CharToNum
* @brief     �����ַ���ʽ��16������ת��Ϊ����
* @param[in] ��
* @param[out]��
* @return    ��
**********************************************************************/
uint8 CharToNum(uint8 str)
{
    uint8 ret = 0;
    if (str >= '0' && str <= '9')
    {
        ret = str - '0';
    }
    if (str >= 'a' && str <= 'z')
    {
        ret = str - 'a' + 10;
    }
    if (str >= 'A' && str <= 'Z')
    {
        ret = str - 'A' + 10;
    }
    return ret;
}

/**********************************************************************
* @name      : event_data_is_null
* @brief     �����ַ���ת��ΪDESCRIPTOR_T type=value;mfrs=value;idformat=value;id=value;mmfrs=value;midformat=value;mid=value
* @param[in] ��type 0 �� type1 ��
*              buf  
* @param[out]��
* @return    ��TRUE - ��
**********************************************************************/
DESCRIPTOR_T UnpackDescriptor(uint8 type, char* buf)
{
    DESCRIPTOR_T Descriptor;
    memset(&Descriptor, 0x00, sizeof(DESCRIPTOR_T));

    char* p;
    char* str = buf;
    p = strsep(&str, ";");
    while (p != NULL)
    {
        //printf("%s \n",p);
        char* tmp;
        tmp = strsep(&p, "=");
        while (tmp != NULL)
        {
            if (strcmp(tmp, "type") == 0)
            {
                tmp = strsep(&p, "=");
                if (type == 0)
                    strcpy(Descriptor.type, tmp);
                else
                    Descriptor.type[0] = CharToNum(tmp[0]);
            }
            else if (strcmp(tmp, "mfrs") == 0)
            {
                tmp = strsep(&p, "=");
                Descriptor.mfrs[0] = tmp[0];
                Descriptor.mfrs[1] = tmp[1];

            }
            else if (strcmp(tmp, "idformat") == 0)
            {
                tmp = strsep(&p, "=");
                Descriptor.idformat = atoi(tmp);

            }
            else if (strcmp(tmp, "id") == 0)
            {
                tmp = strsep(&p, "=");
                uint8 i;
                for (i = 0; i < 24; i++)
                {
                    Descriptor.id[i] = (CharToNum(tmp[i * 2]) << 4) | CharToNum(tmp[i * 2 + 1]);
                }

            }
            else if (strcmp(tmp, "mmfrs") == 0)
            {
                tmp = strsep(&p, "=");
                Descriptor.mmfrs[0] = tmp[0];
                Descriptor.mmfrs[1] = tmp[1];

            }
            else if (strcmp(tmp, "midformat") == 0)
            {
                tmp = strsep(&p, "=");
                Descriptor.midformat = atoi(tmp);

            }
            else if (strcmp(tmp, "mid") == 0)
            {
                tmp = strsep(&p, "=");
                strcpy(Descriptor.mid, tmp);

            }
            tmp = strsep(&p, "=");
        }
        p = strsep(&str, ";");
    }
    return Descriptor;
}

