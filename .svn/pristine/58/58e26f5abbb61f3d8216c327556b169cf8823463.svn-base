/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#include "CcoLib.h"

#include "CcoCommon.h"
#include "CcoDebug.h"
#include "CcoFiles.h"
#include "CcoPrtl.h"
#include "CcoTTL.h"



LockQueue_T *pAmrRecvDown[MAX_DOWN_THREAD_NUM];                      //抄表线程下行接收共享内存指针
#ifdef AREA_FUJIAN
LockQueue_T  g_CcoRecvDown[MAX_DOWN_THREAD_NUM];                     //福建规约抄表线程下行接收共享内存指针
#endif
LockQueue_T *pAmrSendDown[MAX_DOWN_THREAD_NUM];                      //抄表线程下行发送共享内存指针
LockQueue_T *pAmrRecvUp;                        //抄表线程上行接收共享内存指针(用于处理载波模块信息更新)
LockQueue_T *pAmrSendUp;                        //抄表线程上行发送共享内存指针
LockQueue_T  gAmrRecvDownSearchMeter[MAX_DOWN_THREAD_NUM];           //抄表线程下行接收共享内存指针(用于处理搜表接收数据)
LockQueue_T  gAmrRecvDownReportModule[MAX_DOWN_THREAD_NUM];          //抄表线程下行接收共享内存指针(用于处理主动上报模块信息)

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

/*******************************************************************************
* 函数名称: PlcWriteQueue
* 函数功能: 写队列
* 输入参数：plockqueue          锁缓冲区结构指针
            buf                 需要写入队列的数据
            buf_len             数据长度
* 输出参数：无
* 内部处理: 若队列不满，将buf中的数据放入队列中；若队列满，丢弃该数据
* 返 回 值: >0                  成功
            =0                  失败
*******************************************************************************/
int32 PlcWriteQueue(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len)
{
    int32 flags = 0;
	//printx("*PlcWriteQueue() pLockQueue:%p, &pLockQueue->QueueLock:%p, buf_len:%d \n", pLockQueue, &pLockQueue->QueueLock, buf_len);
	//PrintBuf(buf, buf_len);

    //锁缓冲区
    pthread_mutex_lock(&pLockQueue->QueueLock);

    //判队列中是否还有空的位置
    if (pLockQueue->Queue[pLockQueue->QueueTail].QueueLen == 0)
    {
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
* 函数名称: PlcWriteQueue
* 函数功能: 读队列
* 输入参数：pLockQueue          锁缓冲区结构指针
*           buf                 读数据缓冲区，将队列中的数据取出放入该缓冲区
* 输出参数：plockqueue          队列中的数据
* 内部处理: 判队列中是否有数据，有则从队列中取出数据放入buf中，每执行一次，取一个数据
* 返 回 值: len                 读到的数据长度
*******************************************************************************/
int32 PlcReadQueue(LockQueue_T *pLockQueue, uint8 *buf)
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

void commfun_printstring(uint8 *buf, int16 len)
{
    uint16 i = 0;
    if(len <= 0)
        return;
    for(i = 0; i < len; i++)
        printf("%02x ", buf[i]);
    putchar('\n');
}

/*******************************************************************************
* 函数名称:	SecToDateTime_T()
* 函数功能:	sec转换成DateTime_T
* 输入参数: time_t sec_t
            时间
* 输出参数:	无
* 返 回 值:	DateTime_T 格式时间
*******************************************************************************/
DateTime_T SecToDateTime_T(time_t sec_t)
{
    struct tm p;
    DateTime_T bt;

    LOCALTIME_R(&sec_t,&p);

    bt.Year  = p.tm_year - 100;
    bt.Month  = p.tm_mon + 1;
    bt.Day  = p.tm_mday;
    bt.Hour  = p.tm_hour;
    bt.Minute  = p.tm_min ;
    bt.Second  = p.tm_sec ;

    return bt;
}

/*******************************************************************************
* 函数名称:	SecToDateTime_T()
* 函数功能:	sec转换成DateTime_T
* 输入参数: time_t sec_t
            时间
* 输出参数:	无
* 返 回 值:	DateTime_T 格式时间
*******************************************************************************/
DateTime_T DateTimeToBCDDateTime_T(DateTime_T dt_t)
{
    DateTime_T bt;

    bt.Year  = HexToBcd(dt_t.Year);
    bt.Month  = HexToBcd(dt_t.Month);
    bt.Day  = HexToBcd(dt_t.Day);
    bt.Hour  = HexToBcd(dt_t.Hour);
    bt.Minute  = HexToBcd(dt_t.Minute);
    bt.Second  = HexToBcd(dt_t.Second);

    return bt;
}


/*-
***************************************************************************
*模块编号：
*名称：将BCD格式转换为HEX格式
*功能：将BCD格式转换为HEX格式
*输入参数：BCD格式的数据
*输出参数：
*处理：
*返回：HEX格式的数据
***************************************************************************
-*/

uint8 BcdToHex(uint8 compress_bcd)
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
time_t BcdTimeToSec(uint8* BcdMeterTime)
{
    char 		Bin_MeterTime[6];
    int 		i;
    struct tm 	tMetertime;
    time_t   	MeterSec;

    for(i = 0; i < 6; i++)
    {
        Bin_MeterTime[i] = BcdToHex(BcdMeterTime[i]);
    }

    tMetertime.tm_year = Bin_MeterTime[5]+100;//年
    tMetertime.tm_mon  = Bin_MeterTime[4]-1;//月
    tMetertime.tm_mday = Bin_MeterTime[3];
    tMetertime.tm_hour = Bin_MeterTime[2];
    tMetertime.tm_min  = Bin_MeterTime[1];
    tMetertime.tm_sec  = Bin_MeterTime[0];

    MeterSec = FastDateToSecond(&tMetertime);

    return MeterSec;
}



/*******************************************************************************
* 函数名称:	HexToBcd
* 函数功能:	BIN格式转换成BCD格式
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	BCD值
*******************************************************************************/
uint8 HexToBcd(uint8 hex)
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
void GetCurBinTime(DateTime_T *pTime)
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
void GetCurBcdTime(DateTime_T *pTime)
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


//uint32转char*
void Uint32ToCharArr(const uint32 num, uint8* arr)
{
	uint8 arrTemp[4];
	memset(arrTemp, 0x00, sizeof(arrTemp));
	arrTemp[0] = HIBYTE(HIWORD(num));
	arrTemp[1] = LOBYTE(HIWORD(num));
	arrTemp[2] = HIBYTE(LOWORD(num));
	arrTemp[3] = LOBYTE(LOWORD(num));
	memcpy(arr, arrTemp, 4);
}

//uint32转char*（小端，前低后高）
void Uint32ToCharArr_r(const uint32 num, uint8* arr)
{
	uint8 arrTemp[4];
	memset(arrTemp, 0x00, sizeof(arrTemp));
	arrTemp[3] = HIBYTE(HIWORD(num));
	arrTemp[2] = LOBYTE(HIWORD(num));
	arrTemp[1] = HIBYTE(LOWORD(num));
	arrTemp[0] = LOBYTE(LOWORD(num));
	memcpy(arr, arrTemp, 4);
}


//uint16转char*
void Uint16ToCharArr(const uint16 num, uint8* arr)
{
	uint8 arrTemp[2];
	memset(arrTemp, 0x00, sizeof(arrTemp));
	arrTemp[0] = HIBYTE(num);
	arrTemp[1] = LOBYTE(num);
	memcpy(arr, arrTemp, 2);
}

//uint16转char*（小端，前低后高）
void Uint16ToCharArr_r(const uint16 num, uint8* arr)
{
	uint8 arrTemp[2];
	memset(arrTemp, 0x00, sizeof(arrTemp));
	arrTemp[1] = HIBYTE(num);
	arrTemp[0] = LOBYTE(num);
	memcpy(arr, arrTemp, 2);
}

//char*转uint32
void CharArrToUint32(const uint8* arr, uint32 *num)
{
	*num = MAKETOLONG(*(arr+3), *(arr+2), *(arr+1), *arr);
}

//char*转uint32（小端，前低后高）
void CharArrToUint32_r(const uint8* arr, uint32 *num)
{
	*num = MAKETOLONG(*(arr+0), *(arr+1), *(arr+2), *(arr+3));
}


//char*转uint16
void CharArrToUint16(const uint8* arr, uint16 *num)
{
	*num = MAKEWORD(*(arr+1), *arr);
}

//char*转uint16（小端，前低后高）
void CharArrToUint16_r(const uint8* arr, uint16 *num)
{
	*num = MAKEWORD(*arr, *(arr+1));
}


//根据可变长，组装报文buff
uint32 ChangeLenToBuff(const uint16 len, uint16 *IndexID, uint8 *buff)
{
	if(len >= 0 && len < 128)
	{
		*buff = (uint8)len;
		*IndexID += 1;
	}
	else if(len >= 128 && len <= 65535)
	{
		*buff = 0x82;
		*(buff+1) = HIBYTE(len);
		*(buff+2) = LOBYTE(len);
		*IndexID += 3;
	}
	else if(len < 0 || len > 65535)
	{
		return 0;
	}

	return 1;
}


//根据连续的报文，确定可变长
uint32 BuffToChangeLen(const uint8 *buff, uint16 *IndexID, uint16 *len)
{
	if((uint8)(*buff) >= 0x00 && (uint8)(*buff) < 0x80)
	{
		*len = *buff;
		*IndexID += 1;
	}
    else if((uint8)(*buff)==0x81)
    {
        *len = *(buff+1);
        *IndexID += 2;
    }
    else if((uint8)(*buff)==0x82)
    {
        CharArrToUint16(buff+1, len);
        *IndexID += 3;
    }
    else if((uint8)(*buff)==0x80 || (uint8)(*buff)>0x82)
    {
		return 0;
	}
	return 1;
}

//根据端口号，返回不同的数组下标
int8 PortIDToArrIndex(uint32 nPortID, uint8 *nTtlIndex)
{
	uint8   nTtlIndexTemp = 0;  //不同的端口号对应不同的数组下标
	BOOL bSuccess = FALSE;  //端口号是否正确

	if(MAX_DOWN_THREAD_NUM > MAX_ALLOWED_DOWN_THREAD_NUM)
	{
		CCOUP_FMT_DEBUG("if(MAX_DOWN_THREAD_NUM > MAX_ALLOWED_DOWN_THREAD_NUM) 参数异常! \n");
		return -1;
	}
	for(nTtlIndexTemp=0; nTtlIndexTemp<MAX_DOWN_THREAD_NUM; nTtlIndexTemp++)
	{
		CCOUP_FMT_DEBUG("PortIDToArrIndex() nPortID:%08x, gPortID[%d]:%08x, bSetPortIDFlag[%d]=%d \n", 
			nPortID, nTtlIndexTemp, gPortID[nTtlIndexTemp], nTtlIndexTemp, bSetPortIDFlag[nTtlIndexTemp]);
		if((nPortID == gPortID[nTtlIndexTemp]) && (1 == bSetPortIDFlag[nTtlIndexTemp]))
		{
			bSuccess = TRUE;
			*nTtlIndex = nTtlIndexTemp;
			break;
		}
	}
	if(!bSuccess)
	{
		CCOUP_FMT_DEBUG(" PortIDToArrIndex()端口号错误, 未查到数组下标! \n");
		return -1;
	}
	
	return 1;
}

//根据调度管理APP端口号，返回不同的数组下标
int8 CtsPortIDToArrIndex(uint16 nPortID, uint8 *nTtlIndex)
{
	uint8   nTtlIndexTemp = 0;  //不同的端口号对应不同的数组下标

	if(MAX_CTS_THREAD_NUM > MAX_ALLOWED_CTS_THREAD_NUM)
	{
		CCOUP_FMT_DEBUG("CtsPortIDToArrIndex() if(MAX_CTS_THREAD_NUM > MAX_ALLOWED_CTS_THREAD_NUM) 参数异常! \n");
		return -1;
	}
	for(nTtlIndexTemp=0; nTtlIndexTemp<MAX_CTS_THREAD_NUM; nTtlIndexTemp++)
	{
		if(nPortID == gCtsRouterAppPort[nTtlIndexTemp])
		{
			break;
		}
	}
	if(nTtlIndexTemp >= MAX_CTS_THREAD_NUM)
	{
		CCOUP_FMT_DEBUG("CtsPortIDToArrIndex() PortIDToArrIndex()端口号错误, 未查到数组下标! \n");
		return -1;
	}
	*nTtlIndex = nTtlIndexTemp;
	return 1;
}

/*******************************************************************************
* 函数名称:	crc16
* 函数功能:	crc循环冗余校验计算
* 输入参数:	uint8 * ptr,
            uint8 len
* 输出参数:	无
* 返 回 值:	检验
*******************************************************************************/
uint16 crc16(uint8 * ptr, uint8 len)
{
    static uint16 crc_ba[16] =
    {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    };

    uint16 crc = 0xffff;
    uint8 da;
    while(len--!=0)
    {
        da = ((uint8)(crc/256))/16;
        crc <<= 4;
        crc ^= crc_ba[da^(*ptr/16)];
        da = ((uint8)(crc/256)/16);
        crc <<= 4;
        crc ^= crc_ba[da^(*ptr&0x0f)];
        ptr++;
    }
    return (crc ^ 0xffff);
}

/*******************************************************************************
* 函数名称: IsCloseInnerDelayState
* 函数功能: 是否可以结束任务内部延时状态
* 输入参数: int Condition, 产生累加的条件
            int *Cnt,      计数器，计数器应为静态变量
            int CntMax     计数器的最大值
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int IsCloseInnerDelayState(int Condition, int *Cnt, int CntMax)
{
    if(Condition)
    {
        //printx("InnerDelayState(meet the condition %d)\n",(*Cnt));
        (*Cnt)++;
    }
    if((*Cnt) >= CntMax)
    {
        (*Cnt) = 0;
        return 1;
    }
    return 0;
}

/*******************************************************************************
 *
 * 档案列表操作基本函数
 *
*******************************************************************************/

/* 输出线性表元素 */
uint8 visit_Files(AMR_METER_FILES_INFO_T c)
{
    uint32  nPortID;   //端口号
    //uint16  nMeterFilesID; //档案序号
    uint8   aMeterFilesAddr[6];  //档案地址
    uint8   aMeterFilesMac[6];  //档案MAC
    uint8   nProtocolType;  //协议类型
    //uint8   nCommPara;  //通信参数

    nPortID = c.nPortID;
    //nMeterFilesID = c.stAmrMeterFiles.nMeterFilesID;
    memcpy(aMeterFilesAddr, c.stAmrMeterFiles.aMeterFilesAddr, sizeof(aMeterFilesAddr));
    memcpy(aMeterFilesMac, c.stAmrMeterFiles.aMeterFilesMac, sizeof(aMeterFilesMac));
    nProtocolType = c.stAmrMeterFiles.nProtocolType;
    //nCommPara = c.stAmrMeterFiles.nCommPara;
    printx("档案信息: 端口号:%d, 档案地址:%02x %02x %02x %02x %02x %02x, 档案MAC:%02x %02x %02x %02x %02x %02x, 协议类型:%d. \n", 
        nPortID, aMeterFilesAddr[0], aMeterFilesAddr[1],aMeterFilesAddr[2],aMeterFilesAddr[3],
        aMeterFilesAddr[4],aMeterFilesAddr[5], aMeterFilesMac[0],aMeterFilesMac[1],aMeterFilesMac[2],aMeterFilesMac[3],
        aMeterFilesMac[4],aMeterFilesMac[5],nProtocolType);
    
    return TRUE;
}

/* 初始化顺序线性表 */
uint8 InitList_Files(AMR_METER_FILES_LIST_T *L) 
{ 
    L->nMeterFilesNum=0;
    return TRUE;
}
 
/* 初始条件：顺序线性表L已存在。操作结果：若L为空表，则返回TRUE，否则返回FALSE */
uint8 ListEmpty_Files(AMR_METER_FILES_LIST_T L)
{ 
	if(L.nMeterFilesNum==0)
		return TRUE;
	else
		return FALSE;
}
 
/* 初始条件：顺序线性表L已存在。操作结果：将L重置为空表 */
uint8 ClearList_Files(AMR_METER_FILES_LIST_T *L)
{ 
    L->nMeterFilesNum=0;
    return TRUE;
}
 
/* 初始条件：顺序线性表L已存在。操作结果：返回L中数据元素个数 */
uint32 ListLength_Files(AMR_METER_FILES_LIST_T L)
{
	return L.nMeterFilesNum;
}
 
/* 初始条件：顺序线性表L已存在，1≤i≤ListnMeterFilesNum(L) */
/* 操作结果：用e返回L中第i个数据元素的值,注意i是指位置，第1个位置的数组是从0开始 */
uint8 GetElem_Files(AMR_METER_FILES_LIST_T L,int i,AMR_METER_FILES_INFO_T *e)
{
    if(L.nMeterFilesNum==0 || i<1 || i>L.nMeterFilesNum)
            return FALSE;
    *e=L.stMeterFilesData[i-1];
 
    return TRUE;
}
 
/* 初始条件：顺序线性表L已存在 */
/* 操作结果：返回L中第1个与e满足关系的数据元素的位序。 */
/* 若这样的数据元素不存在，则返回值为0 */
int LocateElem_Files(AMR_METER_FILES_LIST_T L,AMR_METER_FILES_INFO_T e)
{
    int i;
    if (L.nMeterFilesNum==0)
            return 0;
    for(i=0;i<L.nMeterFilesNum;i++)
    {
            if (memcmp((uint8 *)&(L.stMeterFilesData[i]), (uint8 *)&e, sizeof(e)) == 0)
                break;
    }
    if(i>=L.nMeterFilesNum)
            return 0;
 
    return i+1;
}

/* 初始条件：顺序线性表L已存在 */
/* 操作结果：返回L中第1个与e满足关系的数据元素的位序。 */
/* 若这样的数据元素不存在，则返回值为0 */
int LocateElemByPara_Files(AMR_METER_FILES_LIST_T L, AMR_METER_FILES_INFO_T e, AMR_FILES_CMP_PARA_E cmp)
{
    int i = 0;
    if (L.nMeterFilesNum==0)
    {
        return 0;
    }
    switch (cmp)
    {
        /*case CMP_FILES_ID://档案序号
        {
            for(i=0;i<L.nMeterFilesNum;i++)
            {
                if (L.stMeterFilesData[i].stAmrMeterFiles.nMeterFilesID==e.stAmrMeterFiles.nMeterFilesID)
                    break;
            }
            break;
        }*/
        case CMP_FILES_ADDR://档案地址
        {
            for(i=0;i<L.nMeterFilesNum;i++)
            {
                if (!(memcmp(L.stMeterFilesData[i].stAmrMeterFiles.aMeterFilesAddr, e.stAmrMeterFiles.aMeterFilesAddr, 6)))
                    break;
            }
            break;
        }
        case CMP_FILES_ADDR_PROTOCAL://同时比对地址和规约
        {
            for(i=0;i<L.nMeterFilesNum;i++)
            {
                if (!(memcmp(L.stMeterFilesData[i].stAmrMeterFiles.aMeterFilesAddr, e.stAmrMeterFiles.aMeterFilesAddr, 6))
                    && (L.stMeterFilesData[i].stAmrMeterFiles.nProtocolType==e.stAmrMeterFiles.nProtocolType))
                    break;
            }
            break;
        }
        default:
            break;
    }
    
    if(i>=L.nMeterFilesNum)
        return 0;
 
    return i+1;
}
#if 0
/* 初始条件：顺序线性表L已存在 */
/* 操作结果：通过档案地址，查找档案序号。 */
/* 若这样的数据元素不存在，则返回值为0 */
uint16 GetMeterIDByAddr_Files(AMR_METER_FILES_LIST_T L, AMR_METER_FILES_INFO_T e)
{
    int i;
    if (L.nMeterFilesNum==0)
    {
    	return 0;
	}
	for(i=0;i<L.nMeterFilesNum;i++)
    {
    	if (!(memcmp(L.stMeterFilesData[i].stAmrMeterFiles.aMeterFilesAddr, e.stAmrMeterFiles.aMeterFilesAddr, 6)))
    		return L.stMeterFilesData[i].stAmrMeterFiles.nMeterFilesID;
    }
    
    if(i>=L.nMeterFilesNum)
    	return 0;
 
    return 0;
}
#endif
/* 初始条件：顺序线性表L已存在 */
/* 操作结果：通过档案地址，更新档案信息。 */
/* 若这样的数据元素不存在，则返回值为0 */
uint16 UpdateByAddr_Files(AMR_METER_FILES_LIST_T L, AMR_METER_FILES_INFO_T e)
{
    int i;
    if (L.nMeterFilesNum==0)
    {
    	return 0;
	}
	for(i=0;i<L.nMeterFilesNum;i++)
    {
    	if (!(memcmp(L.stMeterFilesData[i].stAmrMeterFiles.aMeterFilesAddr, e.stAmrMeterFiles.aMeterFilesAddr, 6)))
    	{
    		memcpy(&L.stMeterFilesData[i], &e, sizeof(AMR_METER_FILES_INFO_T));
    	}
		break;
    }
    
    if(i>=L.nMeterFilesNum)
    {
    	return 0;
    }
    return 1;
}

#if 0
/* 初始条件：顺序线性表L已存在 */
/* 操作结果：通过档案序号，查找档案位置i（从1开始）。 */
/* 若这样的数据元素不存在，则返回值为0 */
uint16 GetIndexByFilesID_Files(AMR_METER_FILES_LIST_T L, uint16 id)
{
    int i;
    if (L.nMeterFilesNum==0)
    {
    	return 0;
	}
	for(i=0;i<L.nMeterFilesNum;i++)
    {
    	if (L.stMeterFilesData[i].stAmrMeterFiles.nMeterFilesID == id)
    	{
    		return (i+1);
    	}
    }
    
    if(i>=L.nMeterFilesNum)
    {
    	return 0;
    }
    return 0;
}
#endif
/* 初始条件：顺序线性表L已存在 */
/* 操作结果：通过档案序号，查找档案位置i（从1开始）。 */
/* 若这样的数据元素不存在，则返回值为0 */
uint16 GetIndexByAddr_Files(AMR_METER_FILES_LIST_T L, uint8 addr[6])
{
    int i;
    if (L.nMeterFilesNum==0)
    {
        return 0;
    }
    for(i=0;i<L.nMeterFilesNum;i++)
    {
        if (memcmp(L.stMeterFilesData[i].stAmrMeterFiles.aMeterFilesAddr, addr, 6) == 0)
        {
            return (i+1);
        }
    }
    
    if(i>=L.nMeterFilesNum)
    {
        return 0;
    }
    return 0;
}
 
/* 初始条件：顺序线性表L已存在,1≤i≤ListnMeterFilesNum(L)， */
/* 操作结果：在L中第i个位置之前插入新的数据元素e，L的长度加1 */
uint8 ListInsert_Files(AMR_METER_FILES_LIST_T *L,int i,AMR_METER_FILES_INFO_T e)
{ 
	int k;
	if (L->nMeterFilesNum==MAX_FILES_SUMMARY_NUM)  /* 顺序线性表已经满 */
		return FALSE;
	if (i<1 || i>L->nMeterFilesNum+1)/* 当i比第一位置小或者比最后一位置后一位置还要大时 */
		return FALSE;
 
	if (i<=L->nMeterFilesNum)        /* 若插入数据位置不在表尾 */
	{
		for(k=L->nMeterFilesNum-1;k>=i-1;k--)  /* 将要插入位置之后的数据元素向后移动一位 */
			L->stMeterFilesData[k+1]=L->stMeterFilesData[k];
	}
	L->stMeterFilesData[i-1]=e;          /* 将新元素插入 */
	L->nMeterFilesNum++;
 
	return TRUE;
}
 
/* 初始条件：顺序线性表L已存在，1≤i≤ListnMeterFilesNum(L) */
/* 操作结果：删除L的第i个数据元素，并用e返回其值，L的长度减1 */
uint8 ListDelete_Files(AMR_METER_FILES_LIST_T *L,int i,AMR_METER_FILES_INFO_T *e) 
{ 
    int k;
    if (L->nMeterFilesNum==0)               /* 线性表为空 */
		return FALSE;
    if (i<1 || i>L->nMeterFilesNum)         /* 删除位置不正确 */
        return FALSE;
    *e=L->stMeterFilesData[i-1];
    if (i<L->nMeterFilesNum)                /* 如果删除不是最后位置 */
    {
        for(k=i;k<L->nMeterFilesNum;k++)/* 将删除位置后继元素前移 */
			L->stMeterFilesData[k-1]=L->stMeterFilesData[k];
    }
    L->nMeterFilesNum--;
    return TRUE;
}

/* 初始条件：顺序线性表L已存在，1≤i≤ListnMeterFilesNum(L) */
/* 操作结果：删除L的第i个数据元素，并用e返回其值，L的长度减1 */
uint8 ListDeleteByIndex_Files(AMR_METER_FILES_LIST_T *L,int i) 
{ 
    int k;
    if (L->nMeterFilesNum==0)               /* 线性表为空 */
		return FALSE;
    if (i<1 || i>L->nMeterFilesNum)         /* 删除位置不正确 */
        return FALSE;
    
    if (i<L->nMeterFilesNum)                /* 如果删除不是最后位置 */
    {
        for(k=i;k<L->nMeterFilesNum;k++)/* 将删除位置后继元素前移 */
			L->stMeterFilesData[k-1]=L->stMeterFilesData[k];
    }
    L->nMeterFilesNum--;
    return TRUE;
}

 
/* 初始条件：顺序线性表L已存在 */
/* 操作结果：依次对L的每个数据元素输出 */
uint8 ListTraverse_Files(AMR_METER_FILES_LIST_T L)
{
	printx("刷新档案列表AMR_METER_FILES_LIST_T...begin \n");
	int i;
    for(i=0;i<L.nMeterFilesNum;i++)
            visit_Files(L.stMeterFilesData[i]);
	printx("刷新档案列表AMR_METER_FILES_LIST_T...end \n");
    return TRUE;
}
 
void unionL_Files(AMR_METER_FILES_LIST_T *La,AMR_METER_FILES_LIST_T Lb)
{
	int La_len,Lb_len,i;
	AMR_METER_FILES_INFO_T e;
	La_len=ListLength_Files(*La);
	Lb_len=ListLength_Files(Lb);
	for (i=1;i<=Lb_len;i++)
	{
		GetElem_Files(Lb,i,&e);
		if (!LocateElem_Files(*La,e))
			ListInsert_Files(La,++La_len,e);
	}
}


/*******************************************************************************
 *
 * 任务队列基本函数（链表）
 *
*******************************************************************************/

uint8 visit_Task(PLCTASK_DETAIL c)
{
	printx(" 任务名:%d, 任务端口:%d, 任务有效性:%d, 任务优先级:%d, 任务路由操作:%d \n", (int)c.TaskName,
		(int)c.TaskArrIndex, (int)c.TaskStat, (int)c.Priority, (int)c.RoutrtCtrl);
	return TRUE;
}
 
/* 初始化顺序线性表 */
uint8 InitList_Task(PLCTASK_QUEUE *L) 
{ 
	//printx(" uint8 InitList_Task(PLCTASK_QUEUE *L)...begin  \n");
    *L=(PLCTASK_QUEUE)malloc(sizeof(PLCTASK_NODE)); /* 产生头结点,并使L指向此头结点 */
    if(!(*L)) /* 存储分配失败 */
    {
		printx(" uint8 InitList_Task(PLCTASK_QUEUE *L)...FALSE  \n");
        return FALSE;
	}
			
    (*L)->next=NULL; /* 指针域为空 */
 	//printx(" uint8 InitList_Task(PLCTASK_QUEUE *L)...end  \n");
    return TRUE;
}
 
/* 初始条件：顺序线性表L已存在。操作结果：若L为空表，则返回TRUE，否则返回FALSE */
uint8 ListEmpty_Task(PLCTASK_QUEUE L)
{ 
    if(L->next)
            return FALSE;
    else
            return TRUE;
}
 
/* 初始条件：顺序线性表L已存在。操作结果：将L重置为空表 */
uint8 ClearList_Task(PLCTASK_QUEUE *L)
{ 
	PLCTASK_QUEUE p,q;
	p=(*L)->next;           /*  p指向第一个结点 */
	while(p)                /*  没到表尾 */
	{
		q=p->next;
		free(p);
		p=q;
	}
	(*L)->next=NULL;        /* 头结点指针域为空 */
	return TRUE;
}
 
/* 初始条件：顺序线性表L已存在。操作结果：返回L中数据元素个数 */
int ListLength_Task(PLCTASK_QUEUE L)
{
    int i=0;
    PLCTASK_QUEUE p=L->next; /* p指向第一个结点 */
    while(p)                        
    {
        i++;
        p=p->next;
    }
    return i;
}
 
/* 初始条件：顺序线性表L已存在，1≤i≤ListLength(L)+1 */
/* 操作结果：用e返回L中第i个数据元素的值 */
uint8 GetElem_Task(PLCTASK_QUEUE L,int i,PLCTASK_DETAIL *e)
{
	int j;
	PLCTASK_QUEUE p;		/* 声明一结点p */
	p = L->next;		/* 让p指向链表L的第一个结点 */
	j = 1;		/*  j为计数器 */
	while (p && j<i)  /* p不为空或者计数器j还没有等于i时，循环继续 */
	{   
		p = p->next;  /* 让p指向下一个结点 */
		++j;
	}
	if ( !p || j>i ) 
		return FALSE;  /*  第i个元素不存在 */
	*e = p->stPlcTaskDetail;   /*  取第i个元素的数据 */
	return TRUE;
}
 
/* 初始条件：顺序线性表L已存在 */
/* 操作结果：返回L中第1个与e满足关系的数据元素的位序。 */
/* 若这样的数据元素不存在，则返回值为0 */
int LocateElem_Task(PLCTASK_QUEUE L,PLCTASK_DETAIL e)
{
    int i=0;
    PLCTASK_QUEUE p=L->next;
    while(p)
    {
        i++;
		if (memcmp((uint8 *)&(p->stPlcTaskDetail), (uint8 *)&e, sizeof(e)) == 0) /* 找到这样的数据元素 */
                return i;
        p=p->next;
    }
 
    return 0;
}

/* 初始条件：顺序线性表L已存在 */
/* 操作结果：返回L中第1个与e满足关系（相同任务名，相同端口号）的数据元素的位序。 */
/* 若这样的数据元素不存在，则返回值为0 */
int LocateElemSameNamePort_Task(PLCTASK_QUEUE L,PLCTASK_DETAIL e)
{
    int i=0;
    PLCTASK_QUEUE p=L->next;
    while(p)
    {
        i++;
		if ((p->stPlcTaskDetail.TaskName==e.TaskName) && (p->stPlcTaskDetail.TaskArrIndex==e.TaskArrIndex)) /* 找到这样的数据元素 */
                return i;
        p=p->next;
    }
 
    return 0;
}


/* 初始条件：顺序线性表L已存在 */
/* 操作结果：查找优先级滞后的任务, 返回L中第1个与e满足关系的数据元素的位序（从1开始）。 */
/* 返回第一个优先级滞后的任务位置。如果当前任务队列为空,则返回1; 如果找到,则返回具体位置;如果未找到,则返回最后一个任务的下一个位置 */
int LocateElemPriorityLater_Task(PLCTASK_QUEUE L,PLCTASK_DETAIL e)
{
	//printx(" int LocateElemByPara_Task(PLCTASK_QUEUE L,PLCTASK_DETAIL e, AMR_ADD_TASK_PARA_E addpara)...begin  \n");
    int i=0;
    PLCTASK_QUEUE p=L->next;

	while(p)
    {
        i++;
        if(p->stPlcTaskDetail.Priority > e.Priority) /* 找到这样的数据元素 */
            return i;
        p=p->next;
    }
 
    return i+1;  //链表为空时，添加至第1个位置；链表无较大数值优先级时，添加至最后
}


 
/* 初始条件：顺序线性表L已存在,1≤i≤ListLength(L)， */
/* 操作结果：在L中第i个位置之前插入新的数据元素e，L的长度加1 */
uint8 ListInsert_Task(PLCTASK_QUEUE *L,int i,PLCTASK_DETAIL e)
{ 
	//printx(" uint8 ListInsert_Task(PLCTASK_QUEUE *L,int i,PLCTASK_DETAIL e)...begin...i:%d  \n", i);
	int j;
	PLCTASK_QUEUE p,s;
	p = *L;   
	j = 1;
	while (p && j < i)     /* 寻找第i个结点 */
	{
		p = p->next;
		++j;
	} 
	if (!p || j > i) 
		return FALSE;   /* 第i个元素不存在 */
	s = (PLCTASK_QUEUE)malloc(sizeof(PLCTASK_NODE));  /*  生成新结点(C语言标准函数) */
	s->stPlcTaskDetail = e;  
	s->next = p->next;      /* 将p的后继结点赋值给s的后继  */
	p->next = s;          /* 将s赋值给p的后继 */
	//printx(" uint8 ListInsert_Task(PLCTASK_QUEUE *L,int i,PLCTASK_DETAIL e)...end...i:%d  \n", i);
	return TRUE;
}
 
/* 初始条件：顺序线性表L已存在，1≤i≤ListLength(L) */
/* 操作结果：删除L的第i个数据元素，并用e返回其值，L的长度减1 */
uint8 ListDelete_Task(PLCTASK_QUEUE *L,int i,PLCTASK_DETAIL *e) 
{ 
	int j;
	PLCTASK_QUEUE p,q;
	p = *L;
	j = 1;
	while (p->next && j < i)	/* 遍历寻找第i个元素 */
	{
        p = p->next;
        ++j;
	}
	
	if (!(p->next) || j > i) 
	    return FALSE;           /* 第i个元素不存在 */
	q = p->next;
	p->next = q->next;			/* 将q的后继赋值给p的后继 */
	*e = q->stPlcTaskDetail;               /* 将q结点中的数据给e */
	free(q);                    /* 让系统回收此结点，释放内存 */
	return TRUE;
}

/* 初始条件：顺序线性表L已存在，1≤i≤ListLength(L) */
/* 操作结果：删除L的第i个数据元素，L的长度减1 */
uint8 ListDeleteByIndex_Task(PLCTASK_QUEUE *L,int i) 
{ 
	int j;
	PLCTASK_QUEUE p,q;
	p = *L;
	j = 1;
	while (p->next && j < i)	/* 遍历寻找第i个元素 */
	{
        p = p->next;
        ++j;
	}
	
	if (!(p->next) || j > i) 
	    return FALSE;           /* 第i个元素不存在 */
	q = p->next;
	p->next = q->next;			/* 将q的后继赋值给p的后继 */
	free(q);                    /* 让系统回收此结点，释放内存 */
	return TRUE;
}

 
/* 初始条件：顺序线性表L已存在 */
/* 操作结果：依次对L的每个数据元素输出 */
uint8 ListTraverse_Task(PLCTASK_QUEUE L)
{
	printx(" \n");
	printx(" 刷新任务队列PLCTASK_QUEUE...begin \n");
    PLCTASK_QUEUE p=L->next;
    while(p)
    {
        visit_Task(p->stPlcTaskDetail);
        p=p->next;
    }
	printx(" 刷新任务队列PLCTASK_QUEUE...end \n");
    printx(" \n");
    return TRUE;
}


/*******************************************************************************
* 函数名称:	
* 函数功能:	第1个端口 消息产生临时函数
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
uint32 InfoMsgFactory_1(INFO_TASK_TPYE_T InfoTaskType, INFO_DATA_T *InfoMsgData, uint16	 nMeterFilesID)
{
	int i = 0;
	int nCtsTtlIndex = 0;  //区分调度管理CTS的线程端口
	memset(InfoMsgData, 0x00, sizeof(*InfoMsgData));
	
	static uint16 nMsgIndex = 0;
	nMsgIndex++;
	
	//head
    InfoMsgData->IP  = inet_addr(gCtsRouterAppIP[nCtsTtlIndex]);
    InfoMsgData->IP  = ntohl(InfoMsgData->IP);
    InfoMsgData->port = gCtsRouterAppPort[nCtsTtlIndex];
	InfoMsgData->MsgPRIORITY = 0;
	InfoMsgData->MsgRPM = 1;
	InfoMsgData->MsgIndex = nMsgIndex;
	InfoMsgData->MsgLabel = 0x0000;
	InfoMsgData->SourLen = strlen(gCtsRouterAppName[nCtsTtlIndex]);
	memset(InfoMsgData->SourAddr, 0x00, InfoMsgData->SourLen);
	memcpy(InfoMsgData->SourAddr, gCtsRouterAppName[nCtsTtlIndex], InfoMsgData->SourLen);
	InfoMsgData->DestLen = strlen(gCcoRouterAppName);
	memset(InfoMsgData->DestAddr, 0x00, InfoMsgData->DestLen);
	memcpy(InfoMsgData->DestAddr, gCcoRouterAppName, InfoMsgData->DestLen);
	
	switch (InfoTaskType)
	{
		case TYPE_SETTERMALADDRESS://设置终端地址
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x003b;
			uint8 arrTemp[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x66, /*终端地址*/
				
			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
		}
		case TYPE_INFOADDMETERFILES://添加表档案
        {
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0020;
			uint8 arrTemp[] = {0x0b, /*档案数量11*/
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x01,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*MAC地址*/ 0x02,/*协议类型07*/ 0x01,/*通信参数*/
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x02,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*MAC地址*/ 0x02,/*协议类型07*/ 0x02,/*通信参数*/
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x03,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,/*MAC地址*/ 0x02,/*协议类型07*/ 0x03,/*通信参数*/
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x04,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,/*MAC地址*/ 0x02,/*协议类型07*/ 0x04,/*通信参数*/
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x05,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x05,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x05,/*MAC地址*/ 0x02,/*协议类型07*/ 0x05,/*通信参数*/
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x06,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,/*MAC地址*/ 0x02,/*协议类型07*/ 0x06,/*通信参数*/
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x07,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,/*MAC地址*/ 0x02,/*协议类型07*/ 0x07,/*通信参数*/
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x08,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,/*MAC地址*/ 0x02,/*协议类型07*/ 0x08,/*通信参数*/
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x09,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x09,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x09,/*MAC地址*/ 0x02,/*协议类型07*/ 0x09,/*通信参数*/
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x0a,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x0a,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x0a,/*MAC地址*/ 0x02,/*协议类型07*/ 0x0a,/*通信参数*/
				//0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x17,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x17,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x17,/*MAC地址*/ 0x03,/*协议类型07*/ 0x17,/*通信参数*/
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x00, 0x33,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x33,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x33,/*MAC地址*/ 0x03,/*协议类型07*/ 0x33,/*通信参数*/
				//0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x18, 0x52,/*档案序号*/ 0x10, 0x01, 0x70, 0x02, 0x18, 0x52,/*档案地址BCD码*/ 0x10, 0x01, 0x70, 0x02, 0x18, 0x52,/*MAC地址*/ 0x02,/*协议类型07*/ 0x52,/*通信参数*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			
            break;
        }
		case TYPE_INFODELETEMETERFILES://删除表档案
        {
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0021;
			uint8 arrTemp[] = {0x04, /*档案数量8*/
				0x01, 0x01,/*档案序号*/
				0x01, 0x02,/*档案序号*/
				0x01, 0x03,/*档案序号*/
				0x01, 0x04,/*档案序号*/
				//0x01, 0x05,/*档案序号*/
				//0x01, 0x06,/*档案序号*/
				//0x01, 0x07,/*档案序号*/
				//0x01, 0x08,/*档案序号*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
		case TYPE_INFOCLEARMETERFILES://清除表档案
        {
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0022;
			InfoMsgData->nPayloadLength = 0;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
		case TYPE_INFOCURRENTFILESNUM://当前档案数量
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x003D;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
		case TYPE_INFOQUERYFILESDETAIL://查询档案
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0043;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x00, 0x03,/*档案序号起始*/ 0x00, 0x0a,/*查询档案数*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
		case TYPE_INFOROUTERDEAL_PAUSE://暂停抄表
		{
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0040;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
        case TYPE_INFOROUTERDEAL_RECO://恢复抄表
        {
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0041;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
        case TYPE_INFOROUTERDEAL_REST://重启抄表
        {
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0042;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
		case TYPE_INFOACTIVEMETER://主动抄表
        {
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x003E;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x01,/*档案序号*/ 0x01,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB4, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0102)
			{
				uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x02,/*档案序号*/ 0x02,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB5, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0103)
			{
				uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x03,/*档案序号*/ 0x03,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB6, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0104)
			{
				uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x04,/*档案序号*/ 0x04,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB7, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0105)
			{
				uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x05,/*档案序号*/ 0x05,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x05,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB8, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0106)
			{
				uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x06,/*档案序号*/ 0x06,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB9, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0107)
			{
				uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x07,/*档案序号*/ 0x07,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xBA, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0108)
			{
				uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x08,/*档案序号*/ 0x08,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xBB, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0117)
			{
				uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x01, 0x17,/*档案序号*/ 0x11,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x17,/*档案地址*/ 0x3d,/*抄表报文长度*/
					0x68, 0x3b, 0x00, 0x43, 0x05, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x7c, 0x1f, 0x05, 0x03, 0x00, 0x50, 
					0x04, 0x02, 0x00, 0x02, 0x20, 0x21, 0x02, 0x00, 0x1c, 0x07, 0xe3, 0x0c, 0x16, 0x00, 0x01, 0x00, 0x1c, 0x07, 
					0xe3, 0x0c, 0x17, 0x00, 0x01, 0x00, 0x54, 0x03, 0x00, 0x00, 0x02, 0x00, 0x20, 0x21, 0x02, 0x00, 0x00, 0x10, 
					0x40, 0x02, 0x00, 0x00, 0xc2, 0x95, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0033)
			{
				uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x00, 0x33,/*档案序号*/ 0x33,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x33,/*档案地址*/ 0x2f,/*抄表报文长度*/
					0x68, 0x2d, 0x00, 0x43, 0x05, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x90, 0x93, 0x10, 0x00, 0x09, 0x05, 0x02, 0x00, 
					0x01, 0x10, 0x20, 0x02, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
					0x00, 0x00, 0x00, 0x00, 0x45, 0x46, 0x16, /*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x1852)
			{
				uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x18, 0x52,/*档案序号*/ 0x20,/*通信控制字*/ 0x10, 0x01, 0x70, 0x02, 0x18, 0x52,/*档案地址*/ 0x10,/*抄表报文长度*/
					//0x68, 0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x34, 0x33, 0x39, 0x38, 0xaa, 0x16,/*抄表报文*/
					0x68, 0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x33, 0x32, 0x35, 0x35, 0xa1, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
            break;
        }
		case TYPE_INFOSEARCHMETER://启动搜表
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0034;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x5D, 0xE7, 0xE1, 0xD9,/*开始时间 1575477721秒*/ 0x00, 0x00, 0x00, 0x05,/*持续时间*/ 0x01,/*从节点重发次数*/ 0x01,/*随机等待时间片个数*/
				
			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
		case TYPE_INFODATATRANSMIT://数据透传
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0033;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x10,/*抄表报文长度*/ 
						0x68, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB4, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			if(nMeterFilesID == 0x0102)
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x10,/*抄表报文长度*/ 
						0x68, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB5, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0117)
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x42,/*抄表报文长度*/ 
					0x68, 0x40, 0x00, 0x43, 0x05, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x84, 0x07, 0x05, 0x03, 0x00, 0x50, 0x04, 0x02, 0x00, 0x02, 0x20, 0x21, 0x02, 0x00, 0x1c, 
					0x07, 0xe3, 0x0b, 0x19, 0x00, 0x01, 0x00, 0x1c, 0x07, 0xe3, 0x0b, 0x1a, 0x00, 0x01, 0x00, 0x54, 0x03, 0x00, 0x00, 0x03, 0x00, 0x20, 0x21, 0x02, 0x00, 0x00, 0x00, 
					0x10, 0x02, 0x00, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x8c, 0xd0, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x1852)  //645报文
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x10,/*抄表报文长度*/ 
					0x68, 0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x33, 0x32, 0x35, 0x35, 0xa1, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x1853)  //376.2报文
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x2F,/*抄表报文长度*/ 
					0x68, 0x2f, 0x00, 0x43, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 
					0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0xf1, 0x01, 0x00, 0x02, 0x00, 0x10, 0x00, 0x68, 0x52, 0x18, 
					0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x33, 0x32, 0x35, 0x35, 0xa1, 0x16, 0x93, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
            break;
        }
		case TYPE_INFOSLAVEMONITOR://从节点监控
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0038;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x02,/*协议类型07*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*档案地址*/ 
						0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x10,/*抄表报文长度*/ 
						0x68, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB4, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			if(nMeterFilesID == 0x0102)
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x02,/*协议类型07*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*档案地址*/ 
						0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x10,/*抄表报文长度*/ 
						0x68, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB5, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
            break;
        }
		case TYPE_INFOGETBROADCASTDELAY://获取广播延时
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0039;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x02,/*协议类型07*/ 0x12,/*抄表报文长度*/ 
						0x68, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x68, 0x08, 0x06, 0x33, 0x5b, 0x49, 0x3a, 0x45, 0x4c, 0x16, 0x16,/*抄表报文*/
						//0x68, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x68, 0x08, 0x06, 0x00, 0x28, 0x16, 0x07, 0x12, 0x19, 0xB4, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			
            break;
        }
		case TYPE_INFOBROADCASTDEAL://广播
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x003A;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*端口号*/ 0x02,/*协议类型07*/ 0x00,/*通讯字*/ 0x12,/*抄表报文长度*/ 
						0x68, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x68, 0x08, 0x06, 0x33, 0x5b, 0x49, 0x3a, 0x45, 0x4c, 0x16, 0x16,/*抄表报文*/
						//0x68, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x68, 0x08, 0x06, 0x00, 0x28, 0x16, 0x07, 0x12, 0x19, 0xB4, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			
            break;
        }
        default:
            break;
		
		
	}
	return 1;
}


/*******************************************************************************
* 函数名称:	
* 函数功能:	第2个端口 消息产生临时函数
* 输入参数:	
* 输出参数:	
* 返 回 值: 
*******************************************************************************/
uint32 InfoMsgFactory_2(INFO_TASK_TPYE_T InfoTaskType, INFO_DATA_T *InfoMsgData, uint16	 nMeterFilesID)
{
	
	int i = 0;
	int nCtsTtlIndex = 1;  //区分调度管理CTS的线程端口
	memset(InfoMsgData, 0x00, sizeof(*InfoMsgData));
	
	static uint16 nMsgIndex = 0;
	nMsgIndex++;
	
	//head
    InfoMsgData->IP  = inet_addr(gCtsRouterAppIP[nCtsTtlIndex]);
    InfoMsgData->IP  = ntohl(InfoMsgData->IP);
    InfoMsgData->port = gCtsRouterAppPort[nCtsTtlIndex];
	InfoMsgData->MsgPRIORITY = 0;
	InfoMsgData->MsgRPM = 1;
	InfoMsgData->MsgIndex = nMsgIndex;
	InfoMsgData->MsgLabel = 0x0000;
	InfoMsgData->SourLen = strlen(gCtsRouterAppName[nCtsTtlIndex]);
	memset(InfoMsgData->SourAddr, 0x00, InfoMsgData->SourLen);
	memcpy(InfoMsgData->SourAddr, gCtsRouterAppName[nCtsTtlIndex], InfoMsgData->SourLen);
	InfoMsgData->DestLen = strlen(gCcoRouterAppName);
	memset(InfoMsgData->DestAddr, 0x00, InfoMsgData->DestLen);
	memcpy(InfoMsgData->DestAddr, gCcoRouterAppName, InfoMsgData->DestLen);
	
	switch (InfoTaskType)
	{
		case TYPE_INFOADDMETERFILES://添加表档案
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0020;
			uint8 arrTemp[] = {0x01, /*档案数量8*/
				0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x01,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*MAC地址*/ 0x02,/*协议类型07*/ 0x01,/*通信参数*/
				//0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x02,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*MAC地址*/ 0x02,/*协议类型07*/ 0x02,/*通信参数*/
				//0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x03,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,/*MAC地址*/ 0x02,/*协议类型07*/ 0x03,/*通信参数*/
				//0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x04,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,/*MAC地址*/ 0x02,/*协议类型07*/ 0x04,/*通信参数*/
				//0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x05,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x05,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x05,/*MAC地址*/ 0x02,/*协议类型07*/ 0x05,/*通信参数*/
				//0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x06,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,/*MAC地址*/ 0x02,/*协议类型07*/ 0x06,/*通信参数*/
				//0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x07,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,/*MAC地址*/ 0x02,/*协议类型07*/ 0x07,/*通信参数*/
				//0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x08,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,/*MAC地址*/ 0x02,/*协议类型07*/ 0x08,/*通信参数*/
				//0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x09,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x09,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x09,/*MAC地址*/ 0x02,/*协议类型07*/ 0x09,/*通信参数*/
				//0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x0a,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x0a,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x0a,/*MAC地址*/ 0x02,/*协议类型07*/ 0x0a,/*通信参数*/
				//0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x17,/*档案序号*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x17,/*档案地址BCD码*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x17,/*MAC地址*/ 0x03,/*协议类型07*/ 0x17,/*通信参数*/
				//0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x18, 0x52,/*档案序号*/ 0x10, 0x01, 0x70, 0x02, 0x18, 0x52,/*档案地址BCD码*/ 0x10, 0x01, 0x70, 0x02, 0x18, 0x52,/*MAC地址*/ 0x02,/*协议类型07*/ 0x52,/*通信参数*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			
			break;
		}
		case TYPE_INFODELETEMETERFILES://删除表档案
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0021;
			uint8 arrTemp[] = {0x08, /*档案数量8*/
				0x01, 0x01,/*档案序号*/
				0x01, 0x02,/*档案序号*/
				0x01, 0x03,/*档案序号*/
				0x01, 0x04,/*档案序号*/
				0x01, 0x05,/*档案序号*/
				0x01, 0x06,/*档案序号*/
				0x01, 0x07,/*档案序号*/
				0x01, 0x08,/*档案序号*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOCLEARMETERFILES://清除表档案
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0022;
			InfoMsgData->nPayloadLength = 0;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOCURRENTFILESNUM://当前档案数量
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x003D;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOQUERYFILESDETAIL://查询档案
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0043;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x00, 0x01,/*档案序号起始*/ 0x00, 0x01,/*查询档案数*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOROUTERDEAL_PAUSE://暂停抄表
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0040;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOROUTERDEAL_RECO://恢复抄表
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0041;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOROUTERDEAL_REST://重启抄表
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0042;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOACTIVEMETER://主动抄表
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x003E;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x01,/*档案序号*/ 0x01,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB4, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0102)
			{
				uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x02,/*档案序号*/ 0x02,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB5, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0103)
			{
				uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x03,/*档案序号*/ 0x03,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB6, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0104)
			{
				uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x04,/*档案序号*/ 0x04,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB7, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0105)
			{
				uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x05,/*档案序号*/ 0x05,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x05,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB8, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0106)
			{
				uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x06,/*档案序号*/ 0x06,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB9, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0107)
			{
				uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x07,/*档案序号*/ 0x07,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xBA, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0108)
			{
				uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x08,/*档案序号*/ 0x08,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,/*档案地址*/ 0x10,/*抄表报文长度*/
					0x68, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xBB, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0117)
			{
				uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x01, 0x17,/*档案序号*/ 0x11,/*通信控制字*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x17,/*档案地址*/ 0x42,/*抄表报文长度*/
					0x68, 0x40, 0x00, 0x43, 0x05, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x84, 0x07, 0x05, 0x03, 0x00, 0x50, 0x04, 0x02, 0x00, 0x02, 0x20, 0x21, 0x02, 0x00, 0x1c, 
					0x07, 0xe3, 0x0b, 0x19, 0x00, 0x01, 0x00, 0x1c, 0x07, 0xe3, 0x0b, 0x1a, 0x00, 0x01, 0x00, 0x54, 0x03, 0x00, 0x00, 0x03, 0x00, 0x20, 0x21, 0x02, 0x00, 0x00, 0x00, 
					0x10, 0x02, 0x00, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x8c, 0xd0, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x1852)
			{
				uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x18, 0x52,/*档案序号*/ 0x20,/*通信控制字*/ 0x10, 0x01, 0x70, 0x02, 0x18, 0x52,/*档案地址*/ 0x10,/*抄表报文长度*/
					//0x68, 0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x34, 0x33, 0x39, 0x38, 0xaa, 0x16,/*抄表报文*/
					0x68, 0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x33, 0x32, 0x35, 0x35, 0xa1, 0x16,/*抄表报文*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			break;
		}
		case TYPE_INFOSEARCHMETER://启动搜表
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0034;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x5D, 0xE7, 0xE1, 0xD9,/*开始时间 1575477721秒*/ 0x00, 0x00, 0x00, 0x05,/*持续时间*/ 0x01,/*从节点重发次数*/ 0x01,/*随机等待时间片个数*/
				
			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFODATATRANSMIT://数据透传
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0033;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
					0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x10,/*抄表报文长度*/ 
						0x68, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB4, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			if(nMeterFilesID == 0x0102)
			{
				uint8 arrTemp[] = {
					0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x10,/*抄表报文长度*/ 
						0x68, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB5, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x0117)
			{
				uint8 arrTemp[] = {
					0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x42,/*抄表报文长度*/ 
					0x68, 0x40, 0x00, 0x43, 0x05, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x84, 0x07, 0x05, 0x03, 0x00, 0x50, 0x04, 0x02, 0x00, 0x02, 0x20, 0x21, 0x02, 0x00, 0x1c, 
					0x07, 0xe3, 0x0b, 0x19, 0x00, 0x01, 0x00, 0x1c, 0x07, 0xe3, 0x0b, 0x1a, 0x00, 0x01, 0x00, 0x54, 0x03, 0x00, 0x00, 0x03, 0x00, 0x20, 0x21, 0x02, 0x00, 0x00, 0x00, 
					0x10, 0x02, 0x00, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x8c, 0xd0, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x1852)  //645报文
			{
				uint8 arrTemp[] = {
					0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x10,/*抄表报文长度*/ 
					0x68, 0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x33, 0x32, 0x35, 0x35, 0xa1, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x1853)  //376.2报文
			{
				uint8 arrTemp[] = {
					0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x2F,/*抄表报文长度*/ 
					0x68, 0x2f, 0x00, 0x43, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 
					0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0xf1, 0x01, 0x00, 0x02, 0x00, 0x10, 0x00, 0x68, 0x52, 0x18, 
					0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x33, 0x32, 0x35, 0x35, 0xa1, 0x16, 0x93, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			break;
		}
		case TYPE_INFOSLAVEMONITOR://从节点监控
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0038;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
					0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x02,/*协议类型07*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*档案地址*/ 
						0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x10,/*抄表报文长度*/ 
						0x68, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB4, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			if(nMeterFilesID == 0x0102)
			{
				uint8 arrTemp[] = {
					0x00, 0x00, 0x00, 0x01,/*端口号*/ 0x02,/*协议类型07*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*档案地址*/ 
						0x00,/*通讯字*/ 0x00, 0x1e,/*帧超时时间*/ 0x00, 0x1e,/*字符超时时间*/ 0x10,/*抄表报文长度*/ 
						0x68, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB5, 0x16,/*抄表报文*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			break;
		}
		default:
			break;
		
		
	}
	return 1;
}
#if 0
/*******************************************************************************
* 函数名称: ModuleCheck
* 函数功能: 载波模块是否插入检测
* 输入参数: fd                  设备文件描述符
* 输出参数: 无
* 返 回 值:  返回0 为有模块， 1为没有  -1 为失败
*******************************************************************************/
int ModuleCheck(uint8 index)
{

    //if(GetEnvValue("PLC_NO_CHECK_MODULE",0) != 0)
    //   return 0;
    int ModuleStat = -1;//默认模块状态不正常
    int fd = 0;

    fd = open(PLC_CHANNEL_CTRL, O_RDONLY);

    if (fd <= 0)
    {
        CCODOWN_FMT_DEBUG(index, "ModuleCheck open /dev/tty_io fail!\n");
        return ModuleStat;
    }

    //电池供电下线程应该空转
    //if(CheckBattery() < 0)
    //    return ModuleStat;

    ModuleStat = ioctl(fd, CONTROL_GET_MODSTATE, 5);
    close(fd);
    return ModuleStat;

}

/*******************************************************************************
* 函数名称: PlcResetRouter
* 函数功能: 复位载波模块后进入死循环，等待超时后线程重启
* 输入参数: fd                  设备文件描述符
* 输出参数: 无
* 返 回 值:  >0                  成功
*           <0                  失败
*******************************************************************************/
int PlcResetRouter(uint8 index)
{
#if 0
    #if JIANGSU_JZQ_FUN == 1//此宏不允许随意打开,打开会导致国网送检互换性测试不过
    sint8   nRet;
    uint8   ModuleData[2];

    //数据初始化或者调试复位宽带模块，导致路由重新进行组网，这样测试上电穿透台体就不过
    //因此读模块类型失败，或者模块类型为深国电时不进行复位
    nRet = ReadRamXData(PLC_DEBUG_BUF11, ModuleData);
    if ((nRet <= 0) || ((ModuleData[1] == 'D') && (ModuleData[0] == 'G'))
         || ((ModuleData[1] == 'L') && (ModuleData[0] == 'X'))
         || ((ModuleData[1] == 'R') && (ModuleData[0] == 'H'))
         || (ModuleData[1] == 0xff) || (ModuleData[0] == 0xff))
    {
        printx("数据初始化或者调试复位宽带模块，导致路由重新进行组网，这样测试上电穿透台体就不过\n");
        return 1;
    }
    #endif
#endif
    int ret = 0;
    int fd = 0;
    fd = open(PLC_CHANNEL_CTRL, O_RDONLY);

    if(fd <= 0)
    {
        CCODOWN_FMT_DEBUG(index, "PlcResetRouter open /dev/tty_io fail!");
        return -1;
    }

    ret = hal_device_release((HW_DEVICE *)gDevHalInfo[index].dev);
    CCODOWN_FMT_DEBUG(index, "PlcResetRouter hal_device_release() ret=%d \n", ret);

    ioctl(fd, CONTROL_CLEAR_RTS, 5);
    sleep(2);
    ioctl(fd, CONTROL_SET_RTS, 5);
    sleep(2);
    close(fd);

    gDevHalInfo[index].dev = NULL;
    gDevHalInfo[index].dev = (UART_DEVICE_T*)hal_device_get(gDevHalInfo[index].deviceName);
    CCODOWN_FMT_DEBUG(index, "串口[%d] deviceName:%s, dev:%p, gReloadDevFlag=%d \n", 
        index, gDevHalInfo[index].deviceName, gDevHalInfo[index].dev, gReloadDevFlag[index]);

    if (gDevHalInfo[index].dev == NULL)
    {
        CCODOWN_FMT_DEBUG(index, "串口[%d] hal_device_get(%s) fail!\n", index, gDevHalInfo[index].deviceName);
        sleep(2);
        return -1;
    }

    // 打开设备
    ret = gDevHalInfo[index].dev->uart_param_set(gDevHalInfo[index].dev, 9600, 8, 1, PARITY_EVEN);  //TRUE:1, FALSE:0
    CCODOWN_FMT_DEBUG(index, "串口[%d] dev->uart_param_set() ret=%d \n", index, ret);
    if(ret == 0)
    {
        CCODOWN_FMT_DEBUG(index, "串口[%d]打开成功%u bps PlcResetRouter Success \n", index, 9600);
    }
    else
    {
        CCODOWN_FMT_DEBUG(index, "串口[%d]打开失败PlcResetRouter Error \n", index);
        sleep(2);
        return -1;
    }

    sleep(1);
    return 1;
}
#endif

void currentTimeGet(struct timeval *tv)
{
    //struct timeval tv;
    struct  timezone tz; 
    gettimeofday(tv, &tz);
    //FastSecondToDate(&tv, &tz, tm);

}

void writeMsgDealTime(uint8* addr,struct timeval *msgrecvtv,struct timeval *downsendtv,struct timeval *downrecvtv,uint8 result)
{
    struct tm msgsendtm;
    struct timeval tv;
    struct  timezone tz; 
    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &msgsendtm);

    struct tm msgrecvtm,downsendtm,downrecvtm;
    FastSecondToDate(msgrecvtv,&tz,&msgrecvtm);
    FastSecondToDate(downsendtv,&tz,&downsendtm);
    FastSecondToDate(downrecvtv,&tz,&downrecvtm);

    
    if(result == 1)
    {
        STAT_FMT_DEBUG("电表地址 0x%02x%02x%02x%02x%02x%02x 抄表成功 收到消息时标:%02d/%02d %02d:%02d:%02d.%ld 下行发送时标:%02d/%02d %02d:%02d:%02d.%ld 下行收到时标:%02d/%02d %02d:%02d:%02d.%ld 消息回复时标:%02d/%02d %02d:%02d:%02d.%ld\n",
            addr[5],addr[4],addr[3],addr[2],addr[1],addr[0],
            msgrecvtm.tm_mon + 1, msgrecvtm.tm_mday,msgrecvtm.tm_hour, msgrecvtm.tm_min, msgrecvtm.tm_sec, msgrecvtv->tv_usec,
            downsendtm.tm_mon + 1, downsendtm.tm_mday,downsendtm.tm_hour, downsendtm.tm_min, downsendtm.tm_sec, downsendtv->tv_usec,
            downrecvtm.tm_mon + 1, downrecvtm.tm_mday,downrecvtm.tm_hour, downrecvtm.tm_min, downrecvtm.tm_sec, downrecvtv->tv_usec,
            msgsendtm.tm_mon + 1, msgsendtm.tm_mday,msgsendtm.tm_hour, msgsendtm.tm_min, msgsendtm.tm_sec, tv.tv_usec);

    }else
    {
        STAT_FMT_DEBUG("电表地址 0x%02x%02x%02x%02x%02x%02x 抄表失败 收到消息时标:%02d/%02d %02d:%02d:%02d.%ld 下行发送时标:%02d/%02d %02d:%02d:%02d.%ld 失败时标:%02d/%02d %02d:%02d:%02d.%ld\n",
            addr[5],addr[4],addr[3],addr[2],addr[1],addr[5],
            msgrecvtm.tm_mon + 1, msgrecvtm.tm_mday,msgrecvtm.tm_hour, msgrecvtm.tm_min, msgrecvtm.tm_sec, msgrecvtv->tv_usec,
            downsendtm.tm_mon + 1, downsendtm.tm_mday,downsendtm.tm_hour, downsendtm.tm_min, downsendtm.tm_sec, downsendtv->tv_usec,
            msgsendtm.tm_mon + 1, msgsendtm.tm_mday,msgsendtm.tm_hour, msgsendtm.tm_min, msgsendtm.tm_sec, tv.tv_usec);

    }
    

}

