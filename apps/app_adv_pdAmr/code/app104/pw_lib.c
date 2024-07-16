/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2020-01-03
*********************************************************************
*/

#include "pw_lib.h"


/*打印字符串*/
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
* 函数名称: pw_lock_queue_write
* 函数功能: 写队列
* 输入参数：plockqueue          锁缓冲区结构指针
            buf                 需要写入队列的数据
            buf_len             数据长度
            pipe                通道号
* 输出参数：无
* 内部处理: 若队列不满，将buf中的数据放入队列中；若队列满，丢弃该数据
* 返 回 值: >0                  成功
            =0                  失败
*******************************************************************************/
int32 pw_lock_queue_write(LOCK_QUEUE_T *pLockQueue, uint8 *buf, int32 buf_len, uint16 pipe)
{
    int32 flags = 0;

    //锁缓冲区
    pthread_mutex_lock(&pLockQueue->QueueLock);

    //判队列中是否还有空的位置
    if (pLockQueue->Queue[pLockQueue->QueueTail].QueueLen == 0)
    {
    	pLockQueue->Queue[pLockQueue->QueueTail].QueuePipe = pipe;
        pLockQueue->Queue[pLockQueue->QueueTail].QueueLen = buf_len;
        memcpy(pLockQueue->Queue[pLockQueue->QueueTail].QueueBuf, buf, buf_len);
        flags = 1;
        pLockQueue->QueueTail = (pLockQueue->QueueTail + 1) % (MAX_QUEUE_CNT);
    }

    //解缓冲区
    pthread_mutex_unlock(&pLockQueue->QueueLock);

    return flags;
}

/*******************************************************************************
* 函数名称: pw_lock_queue_read
* 函数功能: 读队列
* 输入参数：pLockQueue          锁缓冲区结构指针
*           buf                 读数据缓冲区，将队列中的数据取出放入该缓冲区
			pipe                通道号
* 输出参数：plockqueue          队列中的数据
* 内部处理: 判队列中是否有数据，有则从队列中取出数据放入buf中，每执行一次，取一个数据
* 返 回 值: len                 读到的数据长度
*******************************************************************************/
int32 pw_lock_queue_read(LOCK_QUEUE_T *pLockQueue, uint8 *buf, uint16 *pipe)
{
    int32 len = 0;
    uint8 *pdata_buf;

    //锁缓冲区
    pthread_mutex_lock(&pLockQueue->QueueLock);

    //若队列中有数据，则取到buf中，解开锁
    if ((pLockQueue->QueueHead != pLockQueue->QueueTail)
        || ((pLockQueue->QueueHead == pLockQueue->QueueTail)
            && (pLockQueue->Queue[pLockQueue->QueueHead].QueueLen != 0)))
    {
        pdata_buf = pLockQueue->Queue[pLockQueue->QueueHead].QueueBuf;
        len = pLockQueue->Queue[pLockQueue->QueueHead].QueueLen;
		*pipe = pLockQueue->Queue[pLockQueue->QueueHead].QueuePipe;

        //取队列中的数据
        memcpy(buf, pdata_buf, len);

        //清数据
        memset(pdata_buf, 0, len);
        pLockQueue->Queue[pLockQueue->QueueHead].QueueLen = 0;

        //队列头向后移动一位
        pLockQueue->QueueHead= (pLockQueue->QueueHead + 1) % (MAX_QUEUE_CNT);
    }

    pthread_mutex_unlock(&pLockQueue->QueueLock);

    return len;
}


/*******************************************************************************
* 函数名称:	BcdToHex
* 函数功能:	BCD格式转换成BIN格式
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	BIN值
*******************************************************************************/
uint8 bcd_to_hex(uint8 compress_bcd)
{
  uint8 rv;
  
  rv = ((compress_bcd)>>4)*10+(compress_bcd&0x0f);
  
  return rv;
}


/*******************************************************************************
* 函数名称:	BcdTimeToSec
* 函数功能:	计算时间到现在经过的秒数
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	无
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

    tMetertime.tm_year = Bin_MeterTime[5]+100;//年
    tMetertime.tm_mon  = Bin_MeterTime[4]-1;//月
    tMetertime.tm_mday = Bin_MeterTime[3];
    tMetertime.tm_hour = Bin_MeterTime[2];
    tMetertime.tm_min  = Bin_MeterTime[1];
    tMetertime.tm_sec  = Bin_MeterTime[0];

    MeterSec = mktime(&tMetertime);

    return MeterSec;
}

/*******************************************************************************
* 函数名称:	hex_time_to_sec
* 函数功能:	计算时间到现在经过的秒数
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	无
*******************************************************************************/
time_t hex_time_to_sec(DATE_TIME_T *PointTime)
{
    struct tm p;

    time_t timep;

    time(&timep);

    LOCALTIME_R(&timep,&p);

    p.tm_year = PointTime->Year + 100; //2000年之后的时间
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
* 函数名称:	HexToBcd
* 函数功能:	BIN格式转换成BCD格式
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	BCD值
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
* 函数名称:	GetCurBinTime
* 函数功能:	获取终端当前时间，转换成BIN格式
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	无
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
* 函数名称:	GetCurBcdTime
* 函数功能:	获取终端当前时间，转换成BCD格式
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	无
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
*所属功能单元	应用层主调度
*功能描述		获取系统实时时钟
*输入参数
*	pRtc
*输出参数
*	none
*函数返回值
*内部处理流程：	略
*******************************************************
*/
void sys_bcd_time_get( SYSTEM_BCD_TIME_T *pRtc )
{
	time_t tmpTime;
	struct tm *p;
	uint8 temp8;

	/*得到当前时间*/
	time( &tmpTime );
	p = localtime( &tmpTime );

	/*BIN格式转换成BCD格式*/
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
* @brief     ：系统时间秒数转换为OOP_DATETIME_S_T
* @param[in] ：time_t              sys_tm            系统时间
* @param[out]：OOP_DATETIME_S_T   *poop_time      
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-4-13
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
* @brief     ：OOP_DATETIME_S_T转换为系统时间秒数
* @param[in] ：OOP_DATETIME_S_T oop_time 
* @param[out]：time_t *psys_tm        系统时间    
* @return    ：
* @Create    : 梅安宁
* @Date      ：2019-12-22
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
* 函数名称:	cp56time2a_to_ooptm
* 函数功能:	104协议cp56time2a格式时间，转换成面向对象格式时间
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	无
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
* 函数名称:	cp56time2a_to_ooptm
* 函数功能:	104协议cp56time2a格式时间，转换成面向对象格式时间
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	无
*******************************************************************************/
void ooptm_to_cp56time2a(OOP_DATETIME_S_T oop_time, uint8 *cp56time)
{
    uint16  dtemp = 0;
    dtemp = oop_time.second*1000;
    cp56time[0] =  dtemp&0xff;
	cp56time[1] =  dtemp>>8;
	cp56time[2] =  (oop_time.minute)&0x3f;
	cp56time[3] =  (oop_time.hour)&0x1f;
	cp56time[4] =  (oop_time.day)&0x1f;//没有考虑周
	cp56time[5] =  (oop_time.month)&0x0f;
	cp56time[6] =  (oop_time.year - 2000)&0x7f;
}

//翻转复制
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







