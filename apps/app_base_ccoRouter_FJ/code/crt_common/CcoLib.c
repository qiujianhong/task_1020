/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/


#include "CcoLib.h"

#include "CcoCommon.h"
#include "CcoDebug.h"
#include "CcoFiles.h"
#include "CcoPrtl.h"
#include "CcoTTL.h"



LockQueue_T *pAmrRecvDown[MAX_DOWN_THREAD_NUM];                      //�����߳����н��չ����ڴ�ָ��
#ifdef AREA_FUJIAN
LockQueue_T  g_CcoRecvDown[MAX_DOWN_THREAD_NUM];                     //������Լ�����߳����н��չ����ڴ�ָ��
#endif
LockQueue_T *pAmrSendDown[MAX_DOWN_THREAD_NUM];                      //�����߳����з��͹����ڴ�ָ��
LockQueue_T *pAmrRecvUp;                        //�����߳����н��չ����ڴ�ָ��(���ڴ����ز�ģ����Ϣ����)
LockQueue_T *pAmrSendUp;                        //�����߳����з��͹����ڴ�ָ��
LockQueue_T  gAmrRecvDownSearchMeter[MAX_DOWN_THREAD_NUM];           //�����߳����н��չ����ڴ�ָ��(���ڴ����ѱ��������)
LockQueue_T  gAmrRecvDownReportModule[MAX_DOWN_THREAD_NUM];          //�����߳����н��չ����ڴ�ָ��(���ڴ��������ϱ�ģ����Ϣ)

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

/*******************************************************************************
* ��������: PlcWriteQueue
* ��������: д����
* ���������plockqueue          ���������ṹָ��
            buf                 ��Ҫд����е�����
            buf_len             ���ݳ���
* �����������
* �ڲ�����: �����в�������buf�е����ݷ�������У���������������������
* �� �� ֵ: >0                  �ɹ�
            =0                  ʧ��
*******************************************************************************/
int32 PlcWriteQueue(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len)
{
    int32 flags = 0;
	//printx("*PlcWriteQueue() pLockQueue:%p, &pLockQueue->QueueLock:%p, buf_len:%d \n", pLockQueue, &pLockQueue->QueueLock, buf_len);
	//PrintBuf(buf, buf_len);

    //��������
    pthread_mutex_lock(&pLockQueue->QueueLock);

    //�ж������Ƿ��пյ�λ��
    if (pLockQueue->Queue[pLockQueue->QueueTail].QueueLen == 0)
    {
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
* ��������: PlcWriteQueue
* ��������: ������
* ���������pLockQueue          ���������ṹָ��
*           buf                 �����ݻ��������������е�����ȡ������û�����
* ���������plockqueue          �����е�����
* �ڲ�����: �ж������Ƿ������ݣ�����Ӷ�����ȡ�����ݷ���buf�У�ÿִ��һ�Σ�ȡһ������
* �� �� ֵ: len                 ���������ݳ���
*******************************************************************************/
int32 PlcReadQueue(LockQueue_T *pLockQueue, uint8 *buf)
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
* ��������:	SecToDateTime_T()
* ��������:	secת����DateTime_T
* �������: time_t sec_t
            ʱ��
* �������:	��
* �� �� ֵ:	DateTime_T ��ʽʱ��
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
* ��������:	SecToDateTime_T()
* ��������:	secת����DateTime_T
* �������: time_t sec_t
            ʱ��
* �������:	��
* �� �� ֵ:	DateTime_T ��ʽʱ��
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
*ģ���ţ�
*���ƣ���BCD��ʽת��ΪHEX��ʽ
*���ܣ���BCD��ʽת��ΪHEX��ʽ
*���������BCD��ʽ������
*���������
*����
*���أ�HEX��ʽ������
***************************************************************************
-*/

uint8 BcdToHex(uint8 compress_bcd)
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

    tMetertime.tm_year = Bin_MeterTime[5]+100;//��
    tMetertime.tm_mon  = Bin_MeterTime[4]-1;//��
    tMetertime.tm_mday = Bin_MeterTime[3];
    tMetertime.tm_hour = Bin_MeterTime[2];
    tMetertime.tm_min  = Bin_MeterTime[1];
    tMetertime.tm_sec  = Bin_MeterTime[0];

    MeterSec = FastDateToSecond(&tMetertime);

    return MeterSec;
}



/*******************************************************************************
* ��������:	HexToBcd
* ��������:	BIN��ʽת����BCD��ʽ
* �������:	��
* �������:	��
* �� �� ֵ:	BCDֵ
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
* ��������:	GetCurBinTime
* ��������:	��ȡ�ն˵�ǰʱ�䣬ת����BIN��ʽ
* �������:	��
* �������:	��
* �� �� ֵ:	��
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
* ��������:	GetCurBcdTime
* ��������:	��ȡ�ն˵�ǰʱ�䣬ת����BCD��ʽ
* �������:	��
* �������:	��
* �� �� ֵ:	��
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


//uint32תchar*
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

//uint32תchar*��С�ˣ�ǰ�ͺ�ߣ�
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


//uint16תchar*
void Uint16ToCharArr(const uint16 num, uint8* arr)
{
	uint8 arrTemp[2];
	memset(arrTemp, 0x00, sizeof(arrTemp));
	arrTemp[0] = HIBYTE(num);
	arrTemp[1] = LOBYTE(num);
	memcpy(arr, arrTemp, 2);
}

//uint16תchar*��С�ˣ�ǰ�ͺ�ߣ�
void Uint16ToCharArr_r(const uint16 num, uint8* arr)
{
	uint8 arrTemp[2];
	memset(arrTemp, 0x00, sizeof(arrTemp));
	arrTemp[1] = HIBYTE(num);
	arrTemp[0] = LOBYTE(num);
	memcpy(arr, arrTemp, 2);
}

//char*תuint32
void CharArrToUint32(const uint8* arr, uint32 *num)
{
	*num = MAKETOLONG(*(arr+3), *(arr+2), *(arr+1), *arr);
}

//char*תuint32��С�ˣ�ǰ�ͺ�ߣ�
void CharArrToUint32_r(const uint8* arr, uint32 *num)
{
	*num = MAKETOLONG(*(arr+0), *(arr+1), *(arr+2), *(arr+3));
}


//char*תuint16
void CharArrToUint16(const uint8* arr, uint16 *num)
{
	*num = MAKEWORD(*(arr+1), *arr);
}

//char*תuint16��С�ˣ�ǰ�ͺ�ߣ�
void CharArrToUint16_r(const uint8* arr, uint16 *num)
{
	*num = MAKEWORD(*arr, *(arr+1));
}


//���ݿɱ䳤����װ����buff
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


//���������ı��ģ�ȷ���ɱ䳤
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

//���ݶ˿ںţ����ز�ͬ�������±�
int8 PortIDToArrIndex(uint32 nPortID, uint8 *nTtlIndex)
{
	uint8   nTtlIndexTemp = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�
	BOOL bSuccess = FALSE;  //�˿ں��Ƿ���ȷ

	if(MAX_DOWN_THREAD_NUM > MAX_ALLOWED_DOWN_THREAD_NUM)
	{
		CCOUP_FMT_DEBUG("if(MAX_DOWN_THREAD_NUM > MAX_ALLOWED_DOWN_THREAD_NUM) �����쳣! \n");
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
		CCOUP_FMT_DEBUG(" PortIDToArrIndex()�˿ںŴ���, δ�鵽�����±�! \n");
		return -1;
	}
	
	return 1;
}

//���ݵ��ȹ���APP�˿ںţ����ز�ͬ�������±�
int8 CtsPortIDToArrIndex(uint16 nPortID, uint8 *nTtlIndex)
{
	uint8   nTtlIndexTemp = 0;  //��ͬ�Ķ˿ںŶ�Ӧ��ͬ�������±�

	if(MAX_CTS_THREAD_NUM > MAX_ALLOWED_CTS_THREAD_NUM)
	{
		CCOUP_FMT_DEBUG("CtsPortIDToArrIndex() if(MAX_CTS_THREAD_NUM > MAX_ALLOWED_CTS_THREAD_NUM) �����쳣! \n");
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
		CCOUP_FMT_DEBUG("CtsPortIDToArrIndex() PortIDToArrIndex()�˿ںŴ���, δ�鵽�����±�! \n");
		return -1;
	}
	*nTtlIndex = nTtlIndexTemp;
	return 1;
}

/*******************************************************************************
* ��������:	crc16
* ��������:	crcѭ������У�����
* �������:	uint8 * ptr,
            uint8 len
* �������:	��
* �� �� ֵ:	����
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
* ��������: IsCloseInnerDelayState
* ��������: �Ƿ���Խ��������ڲ���ʱ״̬
* �������: int Condition, �����ۼӵ�����
            int *Cnt,      ��������������ӦΪ��̬����
            int CntMax     �����������ֵ
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
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
 * �����б������������
 *
*******************************************************************************/

/* ������Ա�Ԫ�� */
uint8 visit_Files(AMR_METER_FILES_INFO_T c)
{
    uint32  nPortID;   //�˿ں�
    //uint16  nMeterFilesID; //�������
    uint8   aMeterFilesAddr[6];  //������ַ
    uint8   aMeterFilesMac[6];  //����MAC
    uint8   nProtocolType;  //Э������
    //uint8   nCommPara;  //ͨ�Ų���

    nPortID = c.nPortID;
    //nMeterFilesID = c.stAmrMeterFiles.nMeterFilesID;
    memcpy(aMeterFilesAddr, c.stAmrMeterFiles.aMeterFilesAddr, sizeof(aMeterFilesAddr));
    memcpy(aMeterFilesMac, c.stAmrMeterFiles.aMeterFilesMac, sizeof(aMeterFilesMac));
    nProtocolType = c.stAmrMeterFiles.nProtocolType;
    //nCommPara = c.stAmrMeterFiles.nCommPara;
    printx("������Ϣ: �˿ں�:%d, ������ַ:%02x %02x %02x %02x %02x %02x, ����MAC:%02x %02x %02x %02x %02x %02x, Э������:%d. \n", 
        nPortID, aMeterFilesAddr[0], aMeterFilesAddr[1],aMeterFilesAddr[2],aMeterFilesAddr[3],
        aMeterFilesAddr[4],aMeterFilesAddr[5], aMeterFilesMac[0],aMeterFilesMac[1],aMeterFilesMac[2],aMeterFilesMac[3],
        aMeterFilesMac[4],aMeterFilesMac[5],nProtocolType);
    
    return TRUE;
}

/* ��ʼ��˳�����Ա� */
uint8 InitList_Files(AMR_METER_FILES_LIST_T *L) 
{ 
    L->nMeterFilesNum=0;
    return TRUE;
}
 
/* ��ʼ������˳�����Ա�L�Ѵ��ڡ������������LΪ�ձ��򷵻�TRUE�����򷵻�FALSE */
uint8 ListEmpty_Files(AMR_METER_FILES_LIST_T L)
{ 
	if(L.nMeterFilesNum==0)
		return TRUE;
	else
		return FALSE;
}
 
/* ��ʼ������˳�����Ա�L�Ѵ��ڡ������������L����Ϊ�ձ� */
uint8 ClearList_Files(AMR_METER_FILES_LIST_T *L)
{ 
    L->nMeterFilesNum=0;
    return TRUE;
}
 
/* ��ʼ������˳�����Ա�L�Ѵ��ڡ��������������L������Ԫ�ظ��� */
uint32 ListLength_Files(AMR_METER_FILES_LIST_T L)
{
	return L.nMeterFilesNum;
}
 
/* ��ʼ������˳�����Ա�L�Ѵ��ڣ�1��i��ListnMeterFilesNum(L) */
/* �����������e����L�е�i������Ԫ�ص�ֵ,ע��i��ָλ�ã���1��λ�õ������Ǵ�0��ʼ */
uint8 GetElem_Files(AMR_METER_FILES_LIST_T L,int i,AMR_METER_FILES_INFO_T *e)
{
    if(L.nMeterFilesNum==0 || i<1 || i>L.nMeterFilesNum)
            return FALSE;
    *e=L.stMeterFilesData[i-1];
 
    return TRUE;
}
 
/* ��ʼ������˳�����Ա�L�Ѵ��� */
/* �������������L�е�1����e�����ϵ������Ԫ�ص�λ�� */
/* ������������Ԫ�ز����ڣ��򷵻�ֵΪ0 */
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

/* ��ʼ������˳�����Ա�L�Ѵ��� */
/* �������������L�е�1����e�����ϵ������Ԫ�ص�λ�� */
/* ������������Ԫ�ز����ڣ��򷵻�ֵΪ0 */
int LocateElemByPara_Files(AMR_METER_FILES_LIST_T L, AMR_METER_FILES_INFO_T e, AMR_FILES_CMP_PARA_E cmp)
{
    int i = 0;
    if (L.nMeterFilesNum==0)
    {
        return 0;
    }
    switch (cmp)
    {
        /*case CMP_FILES_ID://�������
        {
            for(i=0;i<L.nMeterFilesNum;i++)
            {
                if (L.stMeterFilesData[i].stAmrMeterFiles.nMeterFilesID==e.stAmrMeterFiles.nMeterFilesID)
                    break;
            }
            break;
        }*/
        case CMP_FILES_ADDR://������ַ
        {
            for(i=0;i<L.nMeterFilesNum;i++)
            {
                if (!(memcmp(L.stMeterFilesData[i].stAmrMeterFiles.aMeterFilesAddr, e.stAmrMeterFiles.aMeterFilesAddr, 6)))
                    break;
            }
            break;
        }
        case CMP_FILES_ADDR_PROTOCAL://ͬʱ�ȶԵ�ַ�͹�Լ
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
/* ��ʼ������˳�����Ա�L�Ѵ��� */
/* ���������ͨ��������ַ�����ҵ�����š� */
/* ������������Ԫ�ز����ڣ��򷵻�ֵΪ0 */
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
/* ��ʼ������˳�����Ա�L�Ѵ��� */
/* ���������ͨ��������ַ�����µ�����Ϣ�� */
/* ������������Ԫ�ز����ڣ��򷵻�ֵΪ0 */
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
/* ��ʼ������˳�����Ա�L�Ѵ��� */
/* ���������ͨ��������ţ����ҵ���λ��i����1��ʼ���� */
/* ������������Ԫ�ز����ڣ��򷵻�ֵΪ0 */
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
/* ��ʼ������˳�����Ա�L�Ѵ��� */
/* ���������ͨ��������ţ����ҵ���λ��i����1��ʼ���� */
/* ������������Ԫ�ز����ڣ��򷵻�ֵΪ0 */
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
 
/* ��ʼ������˳�����Ա�L�Ѵ���,1��i��ListnMeterFilesNum(L)�� */
/* �����������L�е�i��λ��֮ǰ�����µ�����Ԫ��e��L�ĳ��ȼ�1 */
uint8 ListInsert_Files(AMR_METER_FILES_LIST_T *L,int i,AMR_METER_FILES_INFO_T e)
{ 
	int k;
	if (L->nMeterFilesNum==MAX_FILES_SUMMARY_NUM)  /* ˳�����Ա��Ѿ��� */
		return FALSE;
	if (i<1 || i>L->nMeterFilesNum+1)/* ��i�ȵ�һλ��С���߱����һλ�ú�һλ�û�Ҫ��ʱ */
		return FALSE;
 
	if (i<=L->nMeterFilesNum)        /* ����������λ�ò��ڱ�β */
	{
		for(k=L->nMeterFilesNum-1;k>=i-1;k--)  /* ��Ҫ����λ��֮�������Ԫ������ƶ�һλ */
			L->stMeterFilesData[k+1]=L->stMeterFilesData[k];
	}
	L->stMeterFilesData[i-1]=e;          /* ����Ԫ�ز��� */
	L->nMeterFilesNum++;
 
	return TRUE;
}
 
/* ��ʼ������˳�����Ա�L�Ѵ��ڣ�1��i��ListnMeterFilesNum(L) */
/* ���������ɾ��L�ĵ�i������Ԫ�أ�����e������ֵ��L�ĳ��ȼ�1 */
uint8 ListDelete_Files(AMR_METER_FILES_LIST_T *L,int i,AMR_METER_FILES_INFO_T *e) 
{ 
    int k;
    if (L->nMeterFilesNum==0)               /* ���Ա�Ϊ�� */
		return FALSE;
    if (i<1 || i>L->nMeterFilesNum)         /* ɾ��λ�ò���ȷ */
        return FALSE;
    *e=L->stMeterFilesData[i-1];
    if (i<L->nMeterFilesNum)                /* ���ɾ���������λ�� */
    {
        for(k=i;k<L->nMeterFilesNum;k++)/* ��ɾ��λ�ú��Ԫ��ǰ�� */
			L->stMeterFilesData[k-1]=L->stMeterFilesData[k];
    }
    L->nMeterFilesNum--;
    return TRUE;
}

/* ��ʼ������˳�����Ա�L�Ѵ��ڣ�1��i��ListnMeterFilesNum(L) */
/* ���������ɾ��L�ĵ�i������Ԫ�أ�����e������ֵ��L�ĳ��ȼ�1 */
uint8 ListDeleteByIndex_Files(AMR_METER_FILES_LIST_T *L,int i) 
{ 
    int k;
    if (L->nMeterFilesNum==0)               /* ���Ա�Ϊ�� */
		return FALSE;
    if (i<1 || i>L->nMeterFilesNum)         /* ɾ��λ�ò���ȷ */
        return FALSE;
    
    if (i<L->nMeterFilesNum)                /* ���ɾ���������λ�� */
    {
        for(k=i;k<L->nMeterFilesNum;k++)/* ��ɾ��λ�ú��Ԫ��ǰ�� */
			L->stMeterFilesData[k-1]=L->stMeterFilesData[k];
    }
    L->nMeterFilesNum--;
    return TRUE;
}

 
/* ��ʼ������˳�����Ա�L�Ѵ��� */
/* ������������ζ�L��ÿ������Ԫ����� */
uint8 ListTraverse_Files(AMR_METER_FILES_LIST_T L)
{
	printx("ˢ�µ����б�AMR_METER_FILES_LIST_T...begin \n");
	int i;
    for(i=0;i<L.nMeterFilesNum;i++)
            visit_Files(L.stMeterFilesData[i]);
	printx("ˢ�µ����б�AMR_METER_FILES_LIST_T...end \n");
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
 * ������л�������������
 *
*******************************************************************************/

uint8 visit_Task(PLCTASK_DETAIL c)
{
	printx(" ������:%d, ����˿�:%d, ������Ч��:%d, �������ȼ�:%d, ����·�ɲ���:%d \n", (int)c.TaskName,
		(int)c.TaskArrIndex, (int)c.TaskStat, (int)c.Priority, (int)c.RoutrtCtrl);
	return TRUE;
}
 
/* ��ʼ��˳�����Ա� */
uint8 InitList_Task(PLCTASK_QUEUE *L) 
{ 
	//printx(" uint8 InitList_Task(PLCTASK_QUEUE *L)...begin  \n");
    *L=(PLCTASK_QUEUE)malloc(sizeof(PLCTASK_NODE)); /* ����ͷ���,��ʹLָ���ͷ��� */
    if(!(*L)) /* �洢����ʧ�� */
    {
		printx(" uint8 InitList_Task(PLCTASK_QUEUE *L)...FALSE  \n");
        return FALSE;
	}
			
    (*L)->next=NULL; /* ָ����Ϊ�� */
 	//printx(" uint8 InitList_Task(PLCTASK_QUEUE *L)...end  \n");
    return TRUE;
}
 
/* ��ʼ������˳�����Ա�L�Ѵ��ڡ������������LΪ�ձ��򷵻�TRUE�����򷵻�FALSE */
uint8 ListEmpty_Task(PLCTASK_QUEUE L)
{ 
    if(L->next)
            return FALSE;
    else
            return TRUE;
}
 
/* ��ʼ������˳�����Ա�L�Ѵ��ڡ������������L����Ϊ�ձ� */
uint8 ClearList_Task(PLCTASK_QUEUE *L)
{ 
	PLCTASK_QUEUE p,q;
	p=(*L)->next;           /*  pָ���һ����� */
	while(p)                /*  û����β */
	{
		q=p->next;
		free(p);
		p=q;
	}
	(*L)->next=NULL;        /* ͷ���ָ����Ϊ�� */
	return TRUE;
}
 
/* ��ʼ������˳�����Ա�L�Ѵ��ڡ��������������L������Ԫ�ظ��� */
int ListLength_Task(PLCTASK_QUEUE L)
{
    int i=0;
    PLCTASK_QUEUE p=L->next; /* pָ���һ����� */
    while(p)                        
    {
        i++;
        p=p->next;
    }
    return i;
}
 
/* ��ʼ������˳�����Ա�L�Ѵ��ڣ�1��i��ListLength(L)+1 */
/* �����������e����L�е�i������Ԫ�ص�ֵ */
uint8 GetElem_Task(PLCTASK_QUEUE L,int i,PLCTASK_DETAIL *e)
{
	int j;
	PLCTASK_QUEUE p;		/* ����һ���p */
	p = L->next;		/* ��pָ������L�ĵ�һ����� */
	j = 1;		/*  jΪ������ */
	while (p && j<i)  /* p��Ϊ�ջ��߼�����j��û�е���iʱ��ѭ������ */
	{   
		p = p->next;  /* ��pָ����һ����� */
		++j;
	}
	if ( !p || j>i ) 
		return FALSE;  /*  ��i��Ԫ�ز����� */
	*e = p->stPlcTaskDetail;   /*  ȡ��i��Ԫ�ص����� */
	return TRUE;
}
 
/* ��ʼ������˳�����Ա�L�Ѵ��� */
/* �������������L�е�1����e�����ϵ������Ԫ�ص�λ�� */
/* ������������Ԫ�ز����ڣ��򷵻�ֵΪ0 */
int LocateElem_Task(PLCTASK_QUEUE L,PLCTASK_DETAIL e)
{
    int i=0;
    PLCTASK_QUEUE p=L->next;
    while(p)
    {
        i++;
		if (memcmp((uint8 *)&(p->stPlcTaskDetail), (uint8 *)&e, sizeof(e)) == 0) /* �ҵ�����������Ԫ�� */
                return i;
        p=p->next;
    }
 
    return 0;
}

/* ��ʼ������˳�����Ա�L�Ѵ��� */
/* �������������L�е�1����e�����ϵ����ͬ����������ͬ�˿ںţ�������Ԫ�ص�λ�� */
/* ������������Ԫ�ز����ڣ��򷵻�ֵΪ0 */
int LocateElemSameNamePort_Task(PLCTASK_QUEUE L,PLCTASK_DETAIL e)
{
    int i=0;
    PLCTASK_QUEUE p=L->next;
    while(p)
    {
        i++;
		if ((p->stPlcTaskDetail.TaskName==e.TaskName) && (p->stPlcTaskDetail.TaskArrIndex==e.TaskArrIndex)) /* �ҵ�����������Ԫ�� */
                return i;
        p=p->next;
    }
 
    return 0;
}


/* ��ʼ������˳�����Ա�L�Ѵ��� */
/* ����������������ȼ��ͺ������, ����L�е�1����e�����ϵ������Ԫ�ص�λ�򣨴�1��ʼ���� */
/* ���ص�һ�����ȼ��ͺ������λ�á������ǰ�������Ϊ��,�򷵻�1; ����ҵ�,�򷵻ؾ���λ��;���δ�ҵ�,�򷵻����һ���������һ��λ�� */
int LocateElemPriorityLater_Task(PLCTASK_QUEUE L,PLCTASK_DETAIL e)
{
	//printx(" int LocateElemByPara_Task(PLCTASK_QUEUE L,PLCTASK_DETAIL e, AMR_ADD_TASK_PARA_E addpara)...begin  \n");
    int i=0;
    PLCTASK_QUEUE p=L->next;

	while(p)
    {
        i++;
        if(p->stPlcTaskDetail.Priority > e.Priority) /* �ҵ�����������Ԫ�� */
            return i;
        p=p->next;
    }
 
    return i+1;  //����Ϊ��ʱ���������1��λ�ã������޽ϴ���ֵ���ȼ�ʱ����������
}


 
/* ��ʼ������˳�����Ա�L�Ѵ���,1��i��ListLength(L)�� */
/* �����������L�е�i��λ��֮ǰ�����µ�����Ԫ��e��L�ĳ��ȼ�1 */
uint8 ListInsert_Task(PLCTASK_QUEUE *L,int i,PLCTASK_DETAIL e)
{ 
	//printx(" uint8 ListInsert_Task(PLCTASK_QUEUE *L,int i,PLCTASK_DETAIL e)...begin...i:%d  \n", i);
	int j;
	PLCTASK_QUEUE p,s;
	p = *L;   
	j = 1;
	while (p && j < i)     /* Ѱ�ҵ�i����� */
	{
		p = p->next;
		++j;
	} 
	if (!p || j > i) 
		return FALSE;   /* ��i��Ԫ�ز����� */
	s = (PLCTASK_QUEUE)malloc(sizeof(PLCTASK_NODE));  /*  �����½��(C���Ա�׼����) */
	s->stPlcTaskDetail = e;  
	s->next = p->next;      /* ��p�ĺ�̽�㸳ֵ��s�ĺ��  */
	p->next = s;          /* ��s��ֵ��p�ĺ�� */
	//printx(" uint8 ListInsert_Task(PLCTASK_QUEUE *L,int i,PLCTASK_DETAIL e)...end...i:%d  \n", i);
	return TRUE;
}
 
/* ��ʼ������˳�����Ա�L�Ѵ��ڣ�1��i��ListLength(L) */
/* ���������ɾ��L�ĵ�i������Ԫ�أ�����e������ֵ��L�ĳ��ȼ�1 */
uint8 ListDelete_Task(PLCTASK_QUEUE *L,int i,PLCTASK_DETAIL *e) 
{ 
	int j;
	PLCTASK_QUEUE p,q;
	p = *L;
	j = 1;
	while (p->next && j < i)	/* ����Ѱ�ҵ�i��Ԫ�� */
	{
        p = p->next;
        ++j;
	}
	
	if (!(p->next) || j > i) 
	    return FALSE;           /* ��i��Ԫ�ز����� */
	q = p->next;
	p->next = q->next;			/* ��q�ĺ�̸�ֵ��p�ĺ�� */
	*e = q->stPlcTaskDetail;               /* ��q����е����ݸ�e */
	free(q);                    /* ��ϵͳ���մ˽�㣬�ͷ��ڴ� */
	return TRUE;
}

/* ��ʼ������˳�����Ա�L�Ѵ��ڣ�1��i��ListLength(L) */
/* ���������ɾ��L�ĵ�i������Ԫ�أ�L�ĳ��ȼ�1 */
uint8 ListDeleteByIndex_Task(PLCTASK_QUEUE *L,int i) 
{ 
	int j;
	PLCTASK_QUEUE p,q;
	p = *L;
	j = 1;
	while (p->next && j < i)	/* ����Ѱ�ҵ�i��Ԫ�� */
	{
        p = p->next;
        ++j;
	}
	
	if (!(p->next) || j > i) 
	    return FALSE;           /* ��i��Ԫ�ز����� */
	q = p->next;
	p->next = q->next;			/* ��q�ĺ�̸�ֵ��p�ĺ�� */
	free(q);                    /* ��ϵͳ���մ˽�㣬�ͷ��ڴ� */
	return TRUE;
}

 
/* ��ʼ������˳�����Ա�L�Ѵ��� */
/* ������������ζ�L��ÿ������Ԫ����� */
uint8 ListTraverse_Task(PLCTASK_QUEUE L)
{
	printx(" \n");
	printx(" ˢ���������PLCTASK_QUEUE...begin \n");
    PLCTASK_QUEUE p=L->next;
    while(p)
    {
        visit_Task(p->stPlcTaskDetail);
        p=p->next;
    }
	printx(" ˢ���������PLCTASK_QUEUE...end \n");
    printx(" \n");
    return TRUE;
}


/*******************************************************************************
* ��������:	
* ��������:	��1���˿� ��Ϣ������ʱ����
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
uint32 InfoMsgFactory_1(INFO_TASK_TPYE_T InfoTaskType, INFO_DATA_T *InfoMsgData, uint16	 nMeterFilesID)
{
	int i = 0;
	int nCtsTtlIndex = 0;  //���ֵ��ȹ���CTS���̶߳˿�
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
		case TYPE_SETTERMALADDRESS://�����ն˵�ַ
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x003b;
			uint8 arrTemp[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x66, /*�ն˵�ַ*/
				
			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
		}
		case TYPE_INFOADDMETERFILES://��ӱ���
        {
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0020;
			uint8 arrTemp[] = {0x0b, /*��������11*/
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x01,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x01,/*ͨ�Ų���*/
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x02,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x02,/*ͨ�Ų���*/
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x03,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x03,/*ͨ�Ų���*/
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x04,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x04,/*ͨ�Ų���*/
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x05,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x05,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x05,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x05,/*ͨ�Ų���*/
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x06,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x06,/*ͨ�Ų���*/
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x07,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x07,/*ͨ�Ų���*/
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x08,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x08,/*ͨ�Ų���*/
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x09,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x09,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x09,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x09,/*ͨ�Ų���*/
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x0a,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x0a,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x0a,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x0a,/*ͨ�Ų���*/
				//0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x17,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x17,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x17,/*MAC��ַ*/ 0x03,/*Э������07*/ 0x17,/*ͨ�Ų���*/
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x00, 0x33,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x33,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x33,/*MAC��ַ*/ 0x03,/*Э������07*/ 0x33,/*ͨ�Ų���*/
				//0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x18, 0x52,/*�������*/ 0x10, 0x01, 0x70, 0x02, 0x18, 0x52,/*������ַBCD��*/ 0x10, 0x01, 0x70, 0x02, 0x18, 0x52,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x52,/*ͨ�Ų���*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			
            break;
        }
		case TYPE_INFODELETEMETERFILES://ɾ������
        {
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0021;
			uint8 arrTemp[] = {0x04, /*��������8*/
				0x01, 0x01,/*�������*/
				0x01, 0x02,/*�������*/
				0x01, 0x03,/*�������*/
				0x01, 0x04,/*�������*/
				//0x01, 0x05,/*�������*/
				//0x01, 0x06,/*�������*/
				//0x01, 0x07,/*�������*/
				//0x01, 0x08,/*�������*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
		case TYPE_INFOCLEARMETERFILES://�������
        {
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0022;
			InfoMsgData->nPayloadLength = 0;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
		case TYPE_INFOCURRENTFILESNUM://��ǰ��������
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x003D;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
		case TYPE_INFOQUERYFILESDETAIL://��ѯ����
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0043;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x00, 0x03,/*���������ʼ*/ 0x00, 0x0a,/*��ѯ������*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
		case TYPE_INFOROUTERDEAL_PAUSE://��ͣ����
		{
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0040;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
        case TYPE_INFOROUTERDEAL_RECO://�ָ�����
        {
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0041;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
        case TYPE_INFOROUTERDEAL_REST://��������
        {
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0042;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
		case TYPE_INFOACTIVEMETER://��������
        {
        	InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x003E;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x01,/*�������*/ 0x01,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB4, 0x16,/*������*/
				
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
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x02,/*�������*/ 0x02,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB5, 0x16,/*������*/
				
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
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x03,/*�������*/ 0x03,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB6, 0x16,/*������*/
				
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
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x04,/*�������*/ 0x04,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB7, 0x16,/*������*/
				
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
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x05,/*�������*/ 0x05,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x05,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB8, 0x16,/*������*/
				
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
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x06,/*�������*/ 0x06,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB9, 0x16,/*������*/
				
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
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x07,/*�������*/ 0x07,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xBA, 0x16,/*������*/
				
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
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x08,/*�������*/ 0x08,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xBB, 0x16,/*������*/
				
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
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x01, 0x17,/*�������*/ 0x11,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x17,/*������ַ*/ 0x3d,/*�����ĳ���*/
					0x68, 0x3b, 0x00, 0x43, 0x05, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x7c, 0x1f, 0x05, 0x03, 0x00, 0x50, 
					0x04, 0x02, 0x00, 0x02, 0x20, 0x21, 0x02, 0x00, 0x1c, 0x07, 0xe3, 0x0c, 0x16, 0x00, 0x01, 0x00, 0x1c, 0x07, 
					0xe3, 0x0c, 0x17, 0x00, 0x01, 0x00, 0x54, 0x03, 0x00, 0x00, 0x02, 0x00, 0x20, 0x21, 0x02, 0x00, 0x00, 0x10, 
					0x40, 0x02, 0x00, 0x00, 0xc2, 0x95, 0x16,/*������*/
				
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
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x00, 0x33,/*�������*/ 0x33,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x33,/*������ַ*/ 0x2f,/*�����ĳ���*/
					0x68, 0x2d, 0x00, 0x43, 0x05, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x90, 0x93, 0x10, 0x00, 0x09, 0x05, 0x02, 0x00, 
					0x01, 0x10, 0x20, 0x02, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
					0x00, 0x00, 0x00, 0x00, 0x45, 0x46, 0x16, /*������*/
				
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
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x18, 0x52,/*�������*/ 0x20,/*ͨ�ſ�����*/ 0x10, 0x01, 0x70, 0x02, 0x18, 0x52,/*������ַ*/ 0x10,/*�����ĳ���*/
					//0x68, 0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x34, 0x33, 0x39, 0x38, 0xaa, 0x16,/*������*/
					0x68, 0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x33, 0x32, 0x35, 0x35, 0xa1, 0x16,/*������*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
            break;
        }
		case TYPE_INFOSEARCHMETER://�����ѱ�
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0034;
			uint8 arrTemp[] = {
				0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x5D, 0xE7, 0xE1, 0xD9,/*��ʼʱ�� 1575477721��*/ 0x00, 0x00, 0x00, 0x05,/*����ʱ��*/ 0x01,/*�ӽڵ��ط�����*/ 0x01,/*����ȴ�ʱ��Ƭ����*/
				
			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
            break;
        }
		case TYPE_INFODATATRANSMIT://����͸��
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0033;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x10,/*�����ĳ���*/ 
						0x68, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB4, 0x16,/*������*/
					
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
					0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x10,/*�����ĳ���*/ 
						0x68, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB5, 0x16,/*������*/
					
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
					0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x42,/*�����ĳ���*/ 
					0x68, 0x40, 0x00, 0x43, 0x05, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x84, 0x07, 0x05, 0x03, 0x00, 0x50, 0x04, 0x02, 0x00, 0x02, 0x20, 0x21, 0x02, 0x00, 0x1c, 
					0x07, 0xe3, 0x0b, 0x19, 0x00, 0x01, 0x00, 0x1c, 0x07, 0xe3, 0x0b, 0x1a, 0x00, 0x01, 0x00, 0x54, 0x03, 0x00, 0x00, 0x03, 0x00, 0x20, 0x21, 0x02, 0x00, 0x00, 0x00, 
					0x10, 0x02, 0x00, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x8c, 0xd0, 0x16,/*������*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x1852)  //645����
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x10,/*�����ĳ���*/ 
					0x68, 0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x33, 0x32, 0x35, 0x35, 0xa1, 0x16,/*������*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x1853)  //376.2����
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x2F,/*�����ĳ���*/ 
					0x68, 0x2f, 0x00, 0x43, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 
					0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0xf1, 0x01, 0x00, 0x02, 0x00, 0x10, 0x00, 0x68, 0x52, 0x18, 
					0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x33, 0x32, 0x35, 0x35, 0xa1, 0x16, 0x93, 0x16,/*������*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
            break;
        }
		case TYPE_INFOSLAVEMONITOR://�ӽڵ���
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0038;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x02,/*Э������07*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*������ַ*/ 
						0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x10,/*�����ĳ���*/ 
						0x68, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB4, 0x16,/*������*/
					
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
					0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x02,/*Э������07*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*������ַ*/ 
						0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x10,/*�����ĳ���*/ 
						0x68, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB5, 0x16,/*������*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
            break;
        }
		case TYPE_INFOGETBROADCASTDELAY://��ȡ�㲥��ʱ
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0039;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x02,/*Э������07*/ 0x12,/*�����ĳ���*/ 
						0x68, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x68, 0x08, 0x06, 0x33, 0x5b, 0x49, 0x3a, 0x45, 0x4c, 0x16, 0x16,/*������*/
						//0x68, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x68, 0x08, 0x06, 0x00, 0x28, 0x16, 0x07, 0x12, 0x19, 0xB4, 0x16,/*������*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			
            break;
        }
		case TYPE_INFOBROADCASTDEAL://�㲥
        {
            InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x003A;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
					0xf2, 0x09, 0x02, 0x01,/*�˿ں�*/ 0x02,/*Э������07*/ 0x00,/*ͨѶ��*/ 0x12,/*�����ĳ���*/ 
						0x68, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x68, 0x08, 0x06, 0x33, 0x5b, 0x49, 0x3a, 0x45, 0x4c, 0x16, 0x16,/*������*/
						//0x68, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x68, 0x08, 0x06, 0x00, 0x28, 0x16, 0x07, 0x12, 0x19, 0xB4, 0x16,/*������*/
					
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
* ��������:	
* ��������:	��2���˿� ��Ϣ������ʱ����
* �������:	
* �������:	
* �� �� ֵ: 
*******************************************************************************/
uint32 InfoMsgFactory_2(INFO_TASK_TPYE_T InfoTaskType, INFO_DATA_T *InfoMsgData, uint16	 nMeterFilesID)
{
	
	int i = 0;
	int nCtsTtlIndex = 1;  //���ֵ��ȹ���CTS���̶߳˿�
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
		case TYPE_INFOADDMETERFILES://��ӱ���
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0020;
			uint8 arrTemp[] = {0x01, /*��������8*/
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x01,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x01,/*ͨ�Ų���*/
				//0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x02,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x02,/*ͨ�Ų���*/
				//0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x03,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x03,/*ͨ�Ų���*/
				//0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x04,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x04,/*ͨ�Ų���*/
				//0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x05,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x05,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x05,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x05,/*ͨ�Ų���*/
				//0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x06,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x06,/*ͨ�Ų���*/
				//0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x07,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x07,/*ͨ�Ų���*/
				//0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x08,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x08,/*ͨ�Ų���*/
				//0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x09,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x09,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x09,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x09,/*ͨ�Ų���*/
				//0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x0a,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x0a,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x0a,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x0a,/*ͨ�Ų���*/
				//0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x17,/*�������*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x17,/*������ַBCD��*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x17,/*MAC��ַ*/ 0x03,/*Э������07*/ 0x17,/*ͨ�Ų���*/
				//0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x18, 0x52,/*�������*/ 0x10, 0x01, 0x70, 0x02, 0x18, 0x52,/*������ַBCD��*/ 0x10, 0x01, 0x70, 0x02, 0x18, 0x52,/*MAC��ַ*/ 0x02,/*Э������07*/ 0x52,/*ͨ�Ų���*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			
			break;
		}
		case TYPE_INFODELETEMETERFILES://ɾ������
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0021;
			uint8 arrTemp[] = {0x08, /*��������8*/
				0x01, 0x01,/*�������*/
				0x01, 0x02,/*�������*/
				0x01, 0x03,/*�������*/
				0x01, 0x04,/*�������*/
				0x01, 0x05,/*�������*/
				0x01, 0x06,/*�������*/
				0x01, 0x07,/*�������*/
				0x01, 0x08,/*�������*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOCLEARMETERFILES://�������
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0022;
			InfoMsgData->nPayloadLength = 0;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOCURRENTFILESNUM://��ǰ��������
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x003D;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOQUERYFILESDETAIL://��ѯ����
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0043;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x00, 0x01,/*���������ʼ*/ 0x00, 0x01,/*��ѯ������*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOROUTERDEAL_PAUSE://��ͣ����
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0040;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOROUTERDEAL_RECO://�ָ�����
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0041;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOROUTERDEAL_REST://��������
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0042;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/

			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFOACTIVEMETER://��������
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x003E;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x01,/*�������*/ 0x01,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB4, 0x16,/*������*/
				
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
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x02,/*�������*/ 0x02,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB5, 0x16,/*������*/
				
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
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x03,/*�������*/ 0x03,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB6, 0x16,/*������*/
				
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
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x04,/*�������*/ 0x04,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB7, 0x16,/*������*/
				
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
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x05,/*�������*/ 0x05,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x05,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB8, 0x16,/*������*/
				
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
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x06,/*�������*/ 0x06,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB9, 0x16,/*������*/
				
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
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x07,/*�������*/ 0x07,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x07,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xBA, 0x16,/*������*/
				
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
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x08,/*�������*/ 0x08,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x08,/*������ַ*/ 0x10,/*�����ĳ���*/
					0x68, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xBB, 0x16,/*������*/
				
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
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x01, 0x17,/*�������*/ 0x11,/*ͨ�ſ�����*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x17,/*������ַ*/ 0x42,/*�����ĳ���*/
					0x68, 0x40, 0x00, 0x43, 0x05, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x84, 0x07, 0x05, 0x03, 0x00, 0x50, 0x04, 0x02, 0x00, 0x02, 0x20, 0x21, 0x02, 0x00, 0x1c, 
					0x07, 0xe3, 0x0b, 0x19, 0x00, 0x01, 0x00, 0x1c, 0x07, 0xe3, 0x0b, 0x1a, 0x00, 0x01, 0x00, 0x54, 0x03, 0x00, 0x00, 0x03, 0x00, 0x20, 0x21, 0x02, 0x00, 0x00, 0x00, 
					0x10, 0x02, 0x00, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x8c, 0xd0, 0x16,/*������*/
				
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
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x18, 0x52,/*�������*/ 0x20,/*ͨ�ſ�����*/ 0x10, 0x01, 0x70, 0x02, 0x18, 0x52,/*������ַ*/ 0x10,/*�����ĳ���*/
					//0x68, 0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x34, 0x33, 0x39, 0x38, 0xaa, 0x16,/*������*/
					0x68, 0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x33, 0x32, 0x35, 0x35, 0xa1, 0x16,/*������*/
				
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			break;
		}
		case TYPE_INFOSEARCHMETER://�����ѱ�
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0034;
			uint8 arrTemp[] = {
				0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x5D, 0xE7, 0xE1, 0xD9,/*��ʼʱ�� 1575477721��*/ 0x00, 0x00, 0x00, 0x05,/*����ʱ��*/ 0x01,/*�ӽڵ��ط�����*/ 0x01,/*����ȴ�ʱ��Ƭ����*/
				
			};
			InfoMsgData->nPayloadLength = sizeof(arrTemp);
			for(i=0; i<InfoMsgData->nPayloadLength; i++)
			{
				InfoMsgData->aPayloadData[i] = arrTemp[i];
			}
			break;
		}
		case TYPE_INFODATATRANSMIT://����͸��
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0033;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
					0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x10,/*�����ĳ���*/ 
						0x68, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB4, 0x16,/*������*/
					
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
					0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x10,/*�����ĳ���*/ 
						0x68, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB5, 0x16,/*������*/
					
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
					0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x42,/*�����ĳ���*/ 
					0x68, 0x40, 0x00, 0x43, 0x05, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x84, 0x07, 0x05, 0x03, 0x00, 0x50, 0x04, 0x02, 0x00, 0x02, 0x20, 0x21, 0x02, 0x00, 0x1c, 
					0x07, 0xe3, 0x0b, 0x19, 0x00, 0x01, 0x00, 0x1c, 0x07, 0xe3, 0x0b, 0x1a, 0x00, 0x01, 0x00, 0x54, 0x03, 0x00, 0x00, 0x03, 0x00, 0x20, 0x21, 0x02, 0x00, 0x00, 0x00, 
					0x10, 0x02, 0x00, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x8c, 0xd0, 0x16,/*������*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x1852)  //645����
			{
				uint8 arrTemp[] = {
					0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x10,/*�����ĳ���*/ 
					0x68, 0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x33, 0x32, 0x35, 0x35, 0xa1, 0x16,/*������*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			else if(nMeterFilesID == 0x1853)  //376.2����
			{
				uint8 arrTemp[] = {
					0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x2F,/*�����ĳ���*/ 
					0x68, 0x2f, 0x00, 0x43, 0x04, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 
					0x52, 0x18, 0x02, 0x70, 0x01, 0x10, 0xf1, 0x01, 0x00, 0x02, 0x00, 0x10, 0x00, 0x68, 0x52, 0x18, 
					0x02, 0x70, 0x01, 0x10, 0x68, 0x11, 0x04, 0x33, 0x32, 0x35, 0x35, 0xa1, 0x16, 0x93, 0x16,/*������*/
					
				};
				InfoMsgData->nPayloadLength = sizeof(arrTemp);
				for(i=0; i<InfoMsgData->nPayloadLength; i++)
				{
					InfoMsgData->aPayloadData[i] = arrTemp[i];
				}
			}
			break;
		}
		case TYPE_INFOSLAVEMONITOR://�ӽڵ���
		{
			InfoMsgData->IID_ID = 0x0003;
			InfoMsgData->IOP_ID = 0x0038;
			if(nMeterFilesID == 0x0101)
			{
				uint8 arrTemp[] = {
					0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x02,/*Э������07*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,/*������ַ*/ 
						0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x10,/*�����ĳ���*/ 
						0x68, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB4, 0x16,/*������*/
					
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
					0x00, 0x00, 0x00, 0x01,/*�˿ں�*/ 0x02,/*Э������07*/ 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,/*������ַ*/ 
						0x00,/*ͨѶ��*/ 0x00, 0x1e,/*֡��ʱʱ��*/ 0x00, 0x1e,/*�ַ���ʱʱ��*/ 0x10,/*�����ĳ���*/ 
						0x68, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0xB5, 0x16,/*������*/
					
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
* ��������: ModuleCheck
* ��������: �ز�ģ���Ƿ������
* �������: fd                  �豸�ļ�������
* �������: ��
* �� �� ֵ:  ����0 Ϊ��ģ�飬 1Ϊû��  -1 Ϊʧ��
*******************************************************************************/
int ModuleCheck(uint8 index)
{

    //if(GetEnvValue("PLC_NO_CHECK_MODULE",0) != 0)
    //   return 0;
    int ModuleStat = -1;//Ĭ��ģ��״̬������
    int fd = 0;

    fd = open(PLC_CHANNEL_CTRL, O_RDONLY);

    if (fd <= 0)
    {
        CCODOWN_FMT_DEBUG(index, "ModuleCheck open /dev/tty_io fail!\n");
        return ModuleStat;
    }

    //��ع������߳�Ӧ�ÿ�ת
    //if(CheckBattery() < 0)
    //    return ModuleStat;

    ModuleStat = ioctl(fd, CONTROL_GET_MODSTATE, 5);
    close(fd);
    return ModuleStat;

}

/*******************************************************************************
* ��������: PlcResetRouter
* ��������: ��λ�ز�ģ��������ѭ�����ȴ���ʱ���߳�����
* �������: fd                  �豸�ļ�������
* �������: ��
* �� �� ֵ:  >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int PlcResetRouter(uint8 index)
{
#if 0
    #if JIANGSU_JZQ_FUN == 1//�˺겻���������,�򿪻ᵼ�¹����ͼ컥���Բ��Բ���
    sint8   nRet;
    uint8   ModuleData[2];

    //���ݳ�ʼ�����ߵ��Ը�λ���ģ�飬����·�����½������������������ϵ紩͸̨��Ͳ���
    //��˶�ģ������ʧ�ܣ�����ģ������Ϊ�����ʱ�����и�λ
    nRet = ReadRamXData(PLC_DEBUG_BUF11, ModuleData);
    if ((nRet <= 0) || ((ModuleData[1] == 'D') && (ModuleData[0] == 'G'))
         || ((ModuleData[1] == 'L') && (ModuleData[0] == 'X'))
         || ((ModuleData[1] == 'R') && (ModuleData[0] == 'H'))
         || (ModuleData[1] == 0xff) || (ModuleData[0] == 0xff))
    {
        printx("���ݳ�ʼ�����ߵ��Ը�λ���ģ�飬����·�����½������������������ϵ紩͸̨��Ͳ���\n");
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
    CCODOWN_FMT_DEBUG(index, "����[%d] deviceName:%s, dev:%p, gReloadDevFlag=%d \n", 
        index, gDevHalInfo[index].deviceName, gDevHalInfo[index].dev, gReloadDevFlag[index]);

    if (gDevHalInfo[index].dev == NULL)
    {
        CCODOWN_FMT_DEBUG(index, "����[%d] hal_device_get(%s) fail!\n", index, gDevHalInfo[index].deviceName);
        sleep(2);
        return -1;
    }

    // ���豸
    ret = gDevHalInfo[index].dev->uart_param_set(gDevHalInfo[index].dev, 9600, 8, 1, PARITY_EVEN);  //TRUE:1, FALSE:0
    CCODOWN_FMT_DEBUG(index, "����[%d] dev->uart_param_set() ret=%d \n", index, ret);
    if(ret == 0)
    {
        CCODOWN_FMT_DEBUG(index, "����[%d]�򿪳ɹ�%u bps PlcResetRouter Success \n", index, 9600);
    }
    else
    {
        CCODOWN_FMT_DEBUG(index, "����[%d]��ʧ��PlcResetRouter Error \n", index);
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
        STAT_FMT_DEBUG("����ַ 0x%02x%02x%02x%02x%02x%02x ����ɹ� �յ���Ϣʱ��:%02d/%02d %02d:%02d:%02d.%ld ���з���ʱ��:%02d/%02d %02d:%02d:%02d.%ld �����յ�ʱ��:%02d/%02d %02d:%02d:%02d.%ld ��Ϣ�ظ�ʱ��:%02d/%02d %02d:%02d:%02d.%ld\n",
            addr[5],addr[4],addr[3],addr[2],addr[1],addr[0],
            msgrecvtm.tm_mon + 1, msgrecvtm.tm_mday,msgrecvtm.tm_hour, msgrecvtm.tm_min, msgrecvtm.tm_sec, msgrecvtv->tv_usec,
            downsendtm.tm_mon + 1, downsendtm.tm_mday,downsendtm.tm_hour, downsendtm.tm_min, downsendtm.tm_sec, downsendtv->tv_usec,
            downrecvtm.tm_mon + 1, downrecvtm.tm_mday,downrecvtm.tm_hour, downrecvtm.tm_min, downrecvtm.tm_sec, downrecvtv->tv_usec,
            msgsendtm.tm_mon + 1, msgsendtm.tm_mday,msgsendtm.tm_hour, msgsendtm.tm_min, msgsendtm.tm_sec, tv.tv_usec);

    }else
    {
        STAT_FMT_DEBUG("����ַ 0x%02x%02x%02x%02x%02x%02x ����ʧ�� �յ���Ϣʱ��:%02d/%02d %02d:%02d:%02d.%ld ���з���ʱ��:%02d/%02d %02d:%02d:%02d.%ld ʧ��ʱ��:%02d/%02d %02d:%02d:%02d.%ld\n",
            addr[5],addr[4],addr[3],addr[2],addr[1],addr[5],
            msgrecvtm.tm_mon + 1, msgrecvtm.tm_mday,msgrecvtm.tm_hour, msgrecvtm.tm_min, msgrecvtm.tm_sec, msgrecvtv->tv_usec,
            downsendtm.tm_mon + 1, downsendtm.tm_mday,downsendtm.tm_hour, downsendtm.tm_min, downsendtm.tm_sec, downsendtv->tv_usec,
            msgsendtm.tm_mon + 1, msgsendtm.tm_mday,msgsendtm.tm_hour, msgsendtm.tm_min, msgsendtm.tm_sec, tv.tv_usec);

    }
    

}

