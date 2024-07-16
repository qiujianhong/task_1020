/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2020-01-03
*********************************************************************
*/

#include "pw_lib.h"


/*��ӡ�ַ���*/
void pw_string_print(uint8 *buf, int16 len)
{
    uint16 i = 0;
	
    if(len <= 0)
    {
		return;
	}        
	
    for(i = 0; i < len; i++)
    {
		printf("%02x ", buf[i]);
	}        
	
    putchar('\n');
}


/*******************************************************************************
* ��������: pw_lock_queue_write
* ��������: д����
* ���������plockqueue          ���������ṹָ��
            buf                 ��Ҫд����е�����
            buf_len             ���ݳ���
            pipe                ͨ����
* �����������
* �ڲ�����: �����в�������buf�е����ݷ�������У���������������������
* �� �� ֵ: >0                  �ɹ�
            =0                  ʧ��
*******************************************************************************/
int32 pw_lock_queue_write(LOCK_QUEUE_T *pLockQueue, uint8 *buf, int32 buf_len, uint16 pipe)
{
    int32 flags = 0;

    //��������
    pthread_mutex_lock(&pLockQueue->QueueLock);

    //�ж������Ƿ��пյ�λ��
    if (pLockQueue->Queue[pLockQueue->QueueTail].QueueLen == 0)
    {
    	pLockQueue->Queue[pLockQueue->QueueTail].QueuePipe = pipe;
        pLockQueue->Queue[pLockQueue->QueueTail].QueueLen = buf_len;
        memcpy(pLockQueue->Queue[pLockQueue->QueueTail].QueueBuf, buf, buf_len);
        flags = 1;
        pLockQueue->QueueTail = (pLockQueue->QueueTail + 1) % (MAX_QUEUE_CNT);
    }

    //�⻺����
    pthread_mutex_unlock(&pLockQueue->QueueLock);

    return flags;
}

/*******************************************************************************
* ��������: pw_lock_queue_read
* ��������: ������
* ���������pLockQueue          ���������ṹָ��
*           buf                 �����ݻ��������������е�����ȡ������û�����
			pipe                ͨ����
* ���������plockqueue          �����е�����
* �ڲ�����: �ж������Ƿ������ݣ�����Ӷ�����ȡ�����ݷ���buf�У�ÿִ��һ�Σ�ȡһ������
* �� �� ֵ: len                 ���������ݳ���
*******************************************************************************/
int32 pw_lock_queue_read(LOCK_QUEUE_T *pLockQueue, uint8 *buf, uint16 *pipe)
{
    int32 len = 0;
    uint8 *pdata_buf;

    //��������
    pthread_mutex_lock(&pLockQueue->QueueLock);

    //�������������ݣ���ȡ��buf�У��⿪��
    if ((pLockQueue->QueueHead != pLockQueue->QueueTail)
        || ((pLockQueue->QueueHead == pLockQueue->QueueTail)
            && (pLockQueue->Queue[pLockQueue->QueueHead].QueueLen != 0)))
    {
        pdata_buf = pLockQueue->Queue[pLockQueue->QueueHead].QueueBuf;
        len = pLockQueue->Queue[pLockQueue->QueueHead].QueueLen;
		*pipe = pLockQueue->Queue[pLockQueue->QueueHead].QueuePipe;

        //ȡ�����е�����
        memcpy(buf, pdata_buf, len);

        //������
        memset(pdata_buf, 0, len);
        pLockQueue->Queue[pLockQueue->QueueHead].QueueLen = 0;

        //����ͷ����ƶ�һλ
        pLockQueue->QueueHead= (pLockQueue->QueueHead + 1) % (MAX_QUEUE_CNT);
    }

    pthread_mutex_unlock(&pLockQueue->QueueLock);

    return len;
}


/*******************************************************************************
* ��������:	BcdToHex
* ��������:	BCD��ʽת����BIN��ʽ
* �������:	��
* �������:	��
* �� �� ֵ:	BINֵ
*******************************************************************************/
uint8 bcd_to_hex(uint8 compress_bcd)
{
  uint8 rv;
  
  rv = ((compress_bcd)>>4)*10+(compress_bcd&0x0f);
  
  return rv;
}


/*******************************************************************************
* ��������:	BcdTimeToSec
* ��������:	����ʱ�䵽���ھ���������
* �������:	��
* �������:	��
* �� �� ֵ:	��
*******************************************************************************/
time_t bcd_time_to_sec(uint8* BcdMeterTime)
{
    char 		Bin_MeterTime[6];
    int 		i;
    struct tm 	tMetertime;
    time_t   	MeterSec;

    for(i = 0; i < 6; i++)
    {
        Bin_MeterTime[i] = bcd_to_hex(BcdMeterTime[i]);
    }

    tMetertime.tm_year = Bin_MeterTime[5]+100;//��
    tMetertime.tm_mon  = Bin_MeterTime[4]-1;//��
    tMetertime.tm_mday = Bin_MeterTime[3];
    tMetertime.tm_hour = Bin_MeterTime[2];
    tMetertime.tm_min  = Bin_MeterTime[1];
    tMetertime.tm_sec  = Bin_MeterTime[0];

    MeterSec = mktime(&tMetertime);

    return MeterSec;
}

/*******************************************************************************
* ��������:	hex_time_to_sec
* ��������:	����ʱ�䵽���ھ���������
* �������:	��
* �������:	��
* �� �� ֵ:	��
*******************************************************************************/
time_t hex_time_to_sec(DATE_TIME_T *PointTime)
{
    struct tm p;

    time_t timep;

    time(&timep);

    LOCALTIME_R(&timep,&p);

    p.tm_year = PointTime->Year + 100; //2000��֮���ʱ��
    p.tm_mon = PointTime->Month - 1;
    p.tm_mday = PointTime->Day;
    p.tm_hour = PointTime->Hour;
    p.tm_min = PointTime->Minute;
    p.tm_sec = PointTime->Second;
    p.tm_wday = 0;

    time_t sec_t = mktime(&p);

    return sec_t;
}


/*******************************************************************************
* ��������:	HexToBcd
* ��������:	BIN��ʽת����BCD��ʽ
* �������:	��
* �������:	��
* �� �� ֵ:	BCDֵ
*******************************************************************************/
uint8 hex_to_bcd(uint8 hex)
{
    uint8 xch;
    uint8 rv = 0;

    if (hex > 99)
    {
        return 0x99;
    }
    xch = hex % 10;
    rv += xch;
    xch = (hex / 10) % 10;
    xch <<= 4;
    rv += xch;

    return rv;
}


/*******************************************************************************
* ��������:	GetCurBinTime
* ��������:	��ȡ�ն˵�ǰʱ�䣬ת����BIN��ʽ
* �������:	��
* �������:	��
* �� �� ֵ:	��
*******************************************************************************/
void cur_bin_time_get(DATE_TIME_T *pTime)
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
* ��������:	GetCurBcdTime
* ��������:	��ȡ�ն˵�ǰʱ�䣬ת����BCD��ʽ
* �������:	��
* �������:	��
* �� �� ֵ:	��
*******************************************************************************/
void cur_bcd_time_get(DATE_TIME_T *pTime)
{
    time_t now;
    struct tm tNowtime;

    time(&now);
    LOCALTIME_R(&now, &tNowtime);

    pTime->Second   = hex_to_bcd((uint8)tNowtime.tm_sec);
    pTime->Minute   = hex_to_bcd((uint8)tNowtime.tm_min);
    pTime->Hour     = hex_to_bcd((uint8)tNowtime.tm_hour);
    pTime->Day      = hex_to_bcd((uint8)tNowtime.tm_mday);
    pTime->Month    = hex_to_bcd((uint8)tNowtime.tm_mon + 1);
    pTime->Year     = hex_to_bcd((uint8)(tNowtime.tm_year - 100));
}

/*
*******************************************************
*�������ܵ�Ԫ	Ӧ�ò�������
*��������		��ȡϵͳʵʱʱ��
*�������
*	pRtc
*�������
*	none
*��������ֵ
*�ڲ��������̣�	��
*******************************************************
*/
void sys_bcd_time_get( SYSTEM_BCD_TIME_T *pRtc )
{
	time_t tmpTime;
	struct tm *p;
	uint8 temp8;

	/*�õ���ǰʱ��*/
	time( &tmpTime );
	p = localtime( &tmpTime );

	/*BIN��ʽת����BCD��ʽ*/
	temp8 = p->tm_sec;
	if( temp8 < 10 )
	{
		pRtc->ss = temp8;
	}
	else
	{
		pRtc->ss = ( ( temp8 / 10 ) << 4 ) + ( temp8 % 10 );
	}
	temp8 = p->tm_min;
	if( temp8 < 10 )
	{
    	pRtc->nn = temp8;
	}
  	else
	{
    	pRtc->nn = ( ( temp8 / 10 ) << 4 ) + ( temp8 % 10 );
	}
	temp8 = p->tm_hour;
	if( temp8 < 10 )
	{
    	pRtc->hh = temp8;
	}
  	else
	{
    	pRtc->hh = ( ( temp8 / 10 ) << 4 ) + ( temp8 % 10 );
	}
	temp8 = p->tm_mday;
	if( temp8 < 10 )
	{
		pRtc->dd = temp8;
	}
	else
	{
		pRtc->dd = ( ( temp8 / 10 ) << 4 ) + ( temp8 % 10 );
	}
	temp8 = 1 + p->tm_mon;
 	if( temp8 < 10 )
	{
		pRtc->mm = temp8;
	}
	else
	{
		pRtc->mm = ( ( temp8 / 10 ) << 4 ) + ( temp8 % 10 );
	}
	temp8 = p->tm_year;
	if( temp8 >= 100 )
		temp8 -= 100;
	if( temp8 < 10 )
	{
		pRtc->yy = temp8;
	}
	else
	{
		pRtc->yy = ( ( temp8 / 10 ) << 4 ) + ( temp8 % 10 );
	}
	pRtc->ww = p->tm_wday;
}

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

    *psys_tm = mktime(&ptm);
    return;
}


/*******************************************************************************
* ��������:	cp56time2a_to_ooptm
* ��������:	104Э��cp56time2a��ʽʱ�䣬ת������������ʽʱ��
* �������:	��
* �������:	��
* �� �� ֵ:	��
*******************************************************************************/
void cp56time2a_to_ooptm(uint8 *cp56time, OOP_DATETIME_S_T *oop_time)
{
    oop_time->second = (cp56time[0] + (uint16)((cp56time[1] << 8) & 0xff00))/1000;
    oop_time->minute = cp56time[2] & 0x3f;
    oop_time->hour = cp56time[3] & 0x1f;
    oop_time->day = cp56time[4] & 0x1f;
    oop_time->month = cp56time[5] & 0x0f;
    oop_time->year = (uint8)(cp56time[6] & 0x7f) + 2000;
}

/*******************************************************************************
* ��������:	cp56time2a_to_ooptm
* ��������:	104Э��cp56time2a��ʽʱ�䣬ת������������ʽʱ��
* �������:	��
* �������:	��
* �� �� ֵ:	��
*******************************************************************************/
void ooptm_to_cp56time2a(OOP_DATETIME_S_T oop_time, uint8 *cp56time)
{
    uint16  dtemp = 0;
    dtemp = oop_time.second*1000;
    cp56time[0] =  dtemp&0xff;
	cp56time[1] =  dtemp>>8;
	cp56time[2] =  (oop_time.minute)&0x3f;
	cp56time[3] =  (oop_time.hour)&0x1f;
	cp56time[4] =  (oop_time.day)&0x1f;//û�п�����
	cp56time[5] =  (oop_time.month)&0x0f;
	cp56time[6] =  (oop_time.year - 2000)&0x7f;
}

//��ת����
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







