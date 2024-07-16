
/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#include "ac_lib.h"
#include "ac_sample.h"
#include "ac_save.h"

/*******************************************************************************
* 调节日志路径
*******************************************************************************/

//调节日志路径
char       *gDownRecordPath[MAX_ALLOWED_DOWN_NUM] = {"ac485comm0", "ac485comm1", 
													 "ac485comm2", "ac485comm3", 
													 "ac485comm4", "ac485comm5", 
													 "ac485comm6", "ac485comm7"};  //调节日志路径

/*******************************************************************************
* 共享缓存队列
*******************************************************************************/
LockQueue_T *pDealRecvDown[RUN_DOWN_NUM];					 //抄表线程下行接收共享内存指针
LockQueue_T *pDealSendDown[RUN_DOWN_NUM];					 //抄表线程下行发送共享内存指针
LockQueue_T *pUdpRecvDown;					                 //UDP透传下行接收共享内存指针
LockQueue_T *pUdpSendDown;					                 //UDP透传下行发送共享内存指针
LockQueue_T *pModuleRecvUp; 					 //模组信息上行接收共享内存指针
LockQueue_T *pConfirmDown; 					     //回复交采模块应答共享内存指针
LockQueue_T *pTimeRecvDown;					                 //对时上行接收共享内存指针
LockQueue_T *pTimeSendDown;					                 //对时下行发送共享内存指针

/*******************************************************************************
* 全局变量
*******************************************************************************/
PAST_TIME_FLAG_TYPE   gTimeFlag;
PAST_TIME_FLAG_TYPE   gSaveTimeFlag;

const uint8 HaveDays[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
#define FOUR_YEARS_TO_DAYS  (3 * 365 + 366)

extern uint8 gdelay;
/*******************************************************************************
* 函数名称: PlcWriteQueue
* 函数功能: 写队列
* 输入参数：plockqueue 		 锁缓冲区结构指针
			buf 				需要写入队列的数据
			buf_len 			数据长度
* 输出参数：无
* 内部处理: 若队列不满，将buf中的数据放入队列中；若队列满，丢弃该数据
* 返 回 值: >0				   成功
			=0					失败
*******************************************************************************/
int32 PlcWriteQueue(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len)
{
	int32 flags = 0;

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
* 输入参数：pLockQueue 		 锁缓冲区结构指针
*			buf 				读数据缓冲区，将队列中的数据取出放入该缓冲区
* 输出参数：plockqueue 		 队列中的数据
* 内部处理: 判队列中是否有数据，有则从队列中取出数据放入buf中，每执行一次，取一个数据
* 返 回 值: len				   读到的数据长度
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


/****************************************************************************
* 函数名称: ACDataSave
* 函数功能: 交采测量点数据保存处理，每秒保存至Ramdisk，每分钟转存到Nandflash
* 输入参数: bFlag 0--只写Ramdisk，每秒更新交采测量点数据
*                 1--交采测量点数据转存，每分钟转存一次
* 输出参数: 无
* 返 回 值: 无
****************************************************************************/
void ACDataSave(uint8 bFlag)
{

}

/*******************************************************************************
* 函数名称: printfBuff
* 函数功能: 打印字符串
* 输入参数: buff 源字符串
*           len  字符串长度
*           flag 收发标志，0--发送，1--接收
* 输出参数: 无   
* 返 回 值: 无
*******************************************************************************/
void printfBuff(uint8 *buff, uint16 len, uint8 flag)
{
	if (getenv("DEBUG_AC") != NULL)
    {
        int i = 0;

	    if (len <= 0)
	    {
	        return;
	    }

	    if (flag == 1)
	    {
	        printf(" receive: \n");
	    }
	    else
	    {
	        printf(" send: \n");
	    }
	    
	    for (i=0; i<len; i++)
	    {
	        printf("%02x ", buff[i]);
	    }
	    printf("\n");
	    
	    return;
    }	
    
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
* 函数名称: app_check_07message
* 函数功能: 检测07报文校验是否正确
* 输入参数: pBuf  源缓冲区数据
*          len   源缓冲区数据长度
* 输出参数: 无
* 返 回 值: 错误码
*******************************************************************************/
int app_check_07message(uint8* pBuf, uint16 bufLen)
{
    uint8	frameCs;
    uint16	frameLen;
    uint16	DataLen = 0;

    DataLen = pBuf[9];
    frameLen = 8 + 2 + 2 + DataLen;

    if (frameLen > bufLen)
    {
        ACCOMM_FMT_LOGCC("app_check_07message err frameLen=%d, bufLen=%d \n", frameLen, bufLen);
        return -1;
    }

    frameCs = CalCs(pBuf, frameLen - 2);

    if ((pBuf[0] == 0x68)
        && (pBuf[7] == pBuf[0])
        && (pBuf[frameLen - 2] == frameCs)
        && (pBuf[frameLen - 1] == 0x16))
    {
        if (frameLen >= 0x0D)
        {
            return 1;
        }
    }
    else
    {
        return -2;
    }

    return -3;
}

/*找合法的报文68开头，主要指回码报文*/
uint8* get_07ptr_from_buf(uint8* buf, uint32 maxLen, uint32 *offset, uint32 *outLen)
{
    int i;
    uint8* head;
    uint32 len = 0;
    uint32 off = 0;
    uint16 dataLen = 0;

    if ((buf == NULL) || (maxLen <= 9))
        return NULL;

    head = buf;
    for (i = 0; i < maxLen - 9; i++)
    {
        if (*head == 0x68)
        {
            dataLen = head[9];
            len = 8 + 2 + 2 + dataLen;
            //防止数据溢出
            if ((i + len) <= maxLen)
            {
                if (app_check_07message((uint8*)head, len) == 1)
                {
                    *offset = off;
                    *outLen = len;

                    ACCOMM_FMT_LOGCC("get_07ptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                    return head;
                }
            }
        }
        off++;
        head++;
    }
    return NULL;
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
        //新增交采模块主动上报报文, 不再考虑控制域
        if((*head == sync)&&(*(head+7) == sync)/*&&(*(head+8)&0x80)*/)
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


void PrintTermShareData(TERM_SHARE_DATA_TYPE stTermSharedData)
{
	
	
}



/*******************************************************************************
*
*************************************原有的分界线******************************************
*
*******************************************************************************/



/*******************************************************************************
* 函数名称: BinTransToBCD
* 函数功能: 将二进制数转化成BCD码
* 输入参数: BinData   源数据
*           Binlength 源数据长度 1~4
* 输出参数: Bcdbuff 转换后的BCD码存放缓冲区
* 返 回 值: BCD码的有效位数
*******************************************************************************/
uint32 BinTransToBCD(uint32 BinData, uint8 *Bcdbuff, uint32 Binlength)
{
    uint32 bits = 0;
    uint32 i;
    uint8  *ptr = Bcdbuff;
    uint8  bval = 0;

    for(i = 0; i < Binlength; i++)
    {
        ptr[i] = 0x00;
    }

    while( BinData && ((uint32)((bits)/2)) < Binlength )
    {
        bval = BinData%10;
        BinData = (uint32)(BinData/10);

        if( bits%2 == 0)
        {
            *ptr = bval&0x0f;
        }
        else 
        {             
            *ptr |= (bval<<4)&0xf0;
            ptr++;
        }

        bits++;         
    }

    return bits;
}

/*******************************************************************************
* 函数名称: DoubleBinToBCD
* 函数功能: 将双字节二进制数转化成BCD码
* 输入参数: BinNum   源数据
* 输出参数: 无
* 返 回 值: 转换得到的BCD码
*******************************************************************************/
uint16 DoubleBinToBCD(uint16 BinNum)
{
    uint16 retnum = 0;

    if (BinNum > 9999)
    {   // 大于等于1万，返回9999
        retnum = 0x9999;
    }
    else
    {
        retnum = BinNum % 10;
        BinNum /= 10;

        retnum |= (BinNum%10)<<4;
        BinNum /= 10;

        retnum |= (BinNum%10)<<8;
        BinNum /= 10;

        retnum |= (BinNum%10)<<12;
    }

    return (retnum);
}

/*******************************************************************************
* 函数名称: Byte4BcdToBin
* 函数功能: 将4字节BCD码转化成二进制数
* 输入参数: bcdpoint 源BCD码
* 输出参数: 无
* 返 回 值: 转换得到二进制格式数
*******************************************************************************/
uint32 Byte4BcdToBin(uint8 *bcdpoint)
{
    uint8  temp1  = 0;
    uint32 retnum = 0;

    retnum = 0;
    temp1 = ((bcdpoint[0] >> 4) * 10) + (bcdpoint[0] % 16);
    retnum += temp1;

    temp1 = ((bcdpoint[1] >> 4) * 10) + (bcdpoint[1] % 16);
    retnum += ((uint32)temp1 * 100);

    if(bcdpoint[2] != 0)
    {
        temp1 = ((bcdpoint[2] >> 4) * 10) + (bcdpoint[2] % 16);
        retnum += ((uint32)temp1 * 10000);
    }

    if(bcdpoint[3] != 0)
    {
        temp1 = ((bcdpoint[3] >> 4) * 10) + (bcdpoint[3] % 16);
        retnum += ((uint32)temp1 * 1000000);
    }
    
    return(retnum);
}

/*******************************************************************************
* 函数名称: AddCRCFun
* 函数功能: 在缓冲区数据末尾加一个字节的累加和
* 输入参数: pData 源缓冲区
*           Len   缓冲区数据长度
* 输出参数: pData 末尾添加了累加和
* 返 回 值: 无
*******************************************************************************/
void AddCRCFun(uint8 *pData, uint16 Len)
{    
    uint8  CRCNum = 0;
    uint16 i;

    for (i = 0; i < Len; i++)
    {
        CRCNum += pData[i];
    }
    pData[Len] = CRCNum;
}

/*******************************************************************************
* 函数名称: GetCRCFun
* 函数功能: 获取缓冲区数据的CRC（累加和）
* 输入参数: pData 源缓冲区
*           Len   缓冲区数据长度
* 输出参数: 无
* 返 回 值: 源缓冲区数据的8位累加和
*******************************************************************************/
uint8 GetCRCFun(uint8 *pData, uint16 Len)
{
    uint8  CRCNum = 0;
    uint16 i;

    for (i=0; i<Len; i++)
    {
        CRCNum += pData[i];
    }

    return (CRCNum);
}

/*******************************************************************************
* 函数名称: CheckCRCFun
* 函数功能: 检测CRC是否正确
* 输入参数: pData 源缓冲区
*           Len   缓冲区数据长度
* 输出参数: 无
* 返 回 值: 0 CRC正确； -1 CRC错误
*******************************************************************************/
int CheckCRCFun(uint8 *pData, uint16 Len)
{
    uint8 CRCNum = 0;
    uint16 i;

    for(i=0; i<Len; i++)
    {
        CRCNum += pData[i];
    }

    if(pData[Len] == CRCNum)
    {
        return (0);
    }
    
    return (-1);
}

/*******************************************************************************
* 函数名称: BCDtoBIN
* 函数功能: 将单字节BCD码转化成二进制数
* 输入参数: BCDnum 源单字节BCD码
* 输出参数: 无
* 返 回 值: 转换结果
*******************************************************************************/
uint8 BCDtoBIN(uint8 BCDnum)
{
    return (((BCDnum >> 4) * 10) + (BCDnum % 16));
}

/*******************************************************************************
* 函数名称: HexToBcdFun
* 函数功能: 将单字节Bin数据转化为Bcd码
* 输入参数: hex 源Bin格式单字节数据
* 输出参数: 无
* 返 回 值: 转换结果
*******************************************************************************/
uint8 HexToBcdFun(uint8 hex)
{
    uint8 xch = 0;
    uint8 rv  = 0;
    
    if (hex > 99) return 0x99;
    xch = hex % 10;
    rv += xch;
    xch = (hex / 10) % 10;
    xch <<= 4;
    rv += xch;
    
    return rv;
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
* 函数名称: Bcd2Hex2
* 函数功能: 将双字节BCD码转为HEX格式
* 输入参数: BcdNum  需要转换的BCD码
* 输出参数: 无   
* 返 回 值: 转换后HEX码
*******************************************************************************/
uint16 Bcd2Hex2(uint16 BcdNum)
{
    uint16 HexNum = 0;

    HexNum = Bcd2Hex(BcdNum >> 8) * 100 + Bcd2Hex(BcdNum & 0xFF);

    return (HexNum);
}

/*******************************************************************************
* 函数名称: Bcd2Hex4
* 函数功能: 将4字节BCD码转为HEX格式
* 输入参数: BcdNum  需要转换的BCD码
* 输出参数: 无
* 返 回 值: 转换后HEX码
*******************************************************************************/
uint32 Bcd2Hex4(uint32 BcdNum)
{
    uint32 HexNum = 0;

    HexNum = (uint32)Bcd2Hex2(BcdNum >> 16) * 10000 + Bcd2Hex2(BcdNum & 0xFFFF);

    return (HexNum);
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
* 函数名称: Hex2Bcd2
* 函数功能: 将双字节HEX格式数据转为BCD码
* 输入参数: HexNum  需要转换的BCD码
* 输出参数: 无   
* 返 回 值: 转换后BCD码
*******************************************************************************/
uint16 Hex2Bcd2(uint16 HexNum)
{
    uint16 BcdNum = 0;

    BcdNum = Hex2Bcd(HexNum / 100) * 0x100;
    BcdNum += Hex2Bcd(HexNum % 100);

    return (BcdNum);
}

/*******************************************************************************
* 函数名称: Hex2Bcd4
* 函数功能: 将4字节HEX格式数据转为BCD码
* 输入参数: HexNum   需要转换的BCD码
* 输出参数: 无   
* 返 回 值: 转换后BCD码
*******************************************************************************/
uint32 Hex2Bcd4(uint32 HexNum)
{
    uint32 BcdNum = 0;

    BcdNum = (uint32)Hex2Bcd2(HexNum / 10000) * 0x10000;
    BcdNum += Hex2Bcd2(HexNum % 10000);

    return (BcdNum);
}

/*******************************************************************************
* 函数名称: HexToBcdSign
* 函数功能: 带方向位HEX到BCD码的转换
* 输入参数: SrcHEX     需要转换的HEX码
*           DataLen    数据长度
*           DataStyle  数据类型(0:unsingned INTEGER/1:signed INTEGER)
* 输出参数: DesBCD     转换后的BCD码   
* 返 回 值: 无
*******************************************************************************/
void HexToBcdSign(uint8 *DesBCD, BYTE4_INT32_TYPE SrcHEX, uint8 DataLen, uint8 DataStyle)
{
    BYTE4_INT32_TYPE CurData;
    uint8    SignFlag = 0;

    if (DataStyle == 1)
    {
        if (SrcHEX.SLinteger < 0)
        {
            CurData.ULinteger = Hex2Bcd4(labs(SrcHEX.SLinteger));
            SignFlag = 1;
        }
        else
        {
            CurData.ULinteger = Hex2Bcd4(SrcHEX.ULinteger);
        }
    }
    else
    {
        CurData.ULinteger = Hex2Bcd4(SrcHEX.ULinteger);
    }

    if (SignFlag == 1)
    {
        CurData.Byte4[DataLen-1] |= 0x80;
    }
    memcpy((uint8*)DesBCD, (uint8*)CurData.Byte4, DataLen);
}

/*******************************************************************************
* 函数名称: BcdToHexSign
* 函数功能: 带方向BCD位到HEX码的转换
* 输入参数: SrcBCD     需要转换的BCD码
*           DataLen    数据长度
*           DataStyle  数据类型(0:unsingned INTEGER/1:signed INTEGER)
* 输出参数: DesHEX     转换后的HEX码
* 返 回 值: 无
*******************************************************************************/
void BcdToHexSign(BYTE4_INT32_TYPE *DesHex, uint8 *SrcBCD, uint8 DataLen, uint8 DataStyle)
{
    BYTE4_INT32_TYPE CurData;

    memset(CurData.Byte4, 0, 4);
    memcpy(CurData.Byte4, SrcBCD, DataLen);
    if (DataStyle == 1)
    {
        if (CurData.Byte4[DataLen-1] & 0x80)
        {
            CurData.Byte4[DataLen-1] &= 0x7F;
            DesHex->SLinteger = Bcd2Hex4(CurData.ULinteger) * (-1);
        }
        else
        {
            DesHex->SLinteger = Bcd2Hex4(CurData.ULinteger);
        }
    }
    else
    {
        DesHex->ULinteger = Bcd2Hex4(CurData.ULinteger);
    }
}

/*******************************************************************************
* 函数名称: BcdDecSubtract
* 函数功能: Bcd码的减法计算(最大10字节限制)
* 输入参数: Minuend    被减数
*           Subtrahend 减数
*           srcLen     字节长度
* 输出参数: Difference 差值
* 返 回 值: 无
*******************************************************************************/
void BcdDecSubtract(uint8 *Minuend, uint8 *Subtrahend,uint8 *Difference,uint8 srcLen)
{    
    uint8 i = 0;
    uint8 Borrow = 0;
    uint8 Temp   = 0;
    uint8 DiffTemp[10] = {0};

    for(i=0; i<srcLen; i++)
    {
        DiffTemp[i] = 0x00;    
    }    
    for(i = 0; i < srcLen; i++)
    {
        Temp = (Minuend[i] & 0x0f) + 0x0a - Borrow - (Subtrahend[i] & 0x0f);
        if(Temp < 0x0a)
        {
            Borrow = 1;
        }
        else
        {
            Borrow = 0;
            Temp -= 0x0a;
        }
        DiffTemp[i] |= (Temp & 0x0f);
        Temp = (Minuend[i] >> 4) + 0x0a - Borrow - (Subtrahend[i] >> 4);
        if(Temp < 0x0a)
        {
            Borrow = 1;
        }
        else
        {
            Borrow = 0;
            Temp -= 0x0a;
        }
        DiffTemp[i] |= (Temp << 4);
    }
    for(i=0; i<srcLen; i++)
    {
        Difference[i] = DiffTemp[i];    
    }
}

/*******************************************************************************
* 函数名称: BcdAddition
* 函数功能: Bcd码的加法计算(最大10字节限制)
* 输入参数: summand    被加数
*           addend     加数
*           srcLen     字节长度
* 输出参数: sum        和
* 返 回 值: 无
*******************************************************************************/
void BcdAddition(uint8 *summand, uint8 *addend,uint8 *sum,uint8 srcLen)
{   
    uint8 i;
    uint8 carry = 0;
    uint8 Temp = 0;
    uint8 sumTemp[10] = {0};

    for(i = 0; i < srcLen; i++)
    {
        sumTemp[i] = 0x00;    
    }
    for(i = 0; i < srcLen; i++)
    {
        Temp = (summand[i] & 0x0f) + (addend[i] & 0x0f) + carry;
        if(Temp >= 0x0a)
        {
            carry = 1;
            Temp -= 0x0a;
        }
        else
        {
            carry = 0;
        }
        sumTemp[i] |= (Temp & 0x0f);

        Temp = (summand[i] >> 4) + (addend[i] >> 4) + carry;
        if(Temp >= 0x0a)
        {
            carry = 1;
            Temp -= 0x0a;
        }
        else
        {
            carry = 0;
        }
        sumTemp[i] |= (Temp << 4);
    }
    for(i=0; i<srcLen; i++)
    {
        sum[i] = sumTemp[i];    
    }
}

// 年、日、小时、分钟对应的秒数宏定义
#define MINUTE_SECOND 60                // 1分钟的秒数
#define HOUR_SECOND   60*MINUTE_SECOND  // 1小时的秒数
#define DAY_SECOND    24*HOUR_SECOND    // 1天的秒数
#define YEAR_SECOND   365*DAY_SECOND    // 1年的秒数

uint32 const MONTH_TABLE[12] = {        // 以年为界限,定义了每个月开始时的秒数时间数组
    0,
    DAY_SECOND*(31),
    DAY_SECOND*(31+29),
    DAY_SECOND*(31+29+31),
    DAY_SECOND*(31+29+31+30),
    DAY_SECOND*(31+29+31+30+31),
    DAY_SECOND*(31+29+31+30+31+30),
    DAY_SECOND*(31+29+31+30+31+30+31),
    DAY_SECOND*(31+29+31+30+31+30+31+31),
    DAY_SECOND*(31+29+31+30+31+30+31+31+30),
    DAY_SECOND*(31+29+31+30+31+30+31+31+30+31),
    DAY_SECOND*(31+29+31+30+31+30+31+31+30+31+30)
};

// 将时间转换为秒
uint32 MakeTimeFun(REAL_TIME_TYPE *ptm)
{
    uint8  i = 0;
    uint8  *ptime = NULL;
    uint32 year = 0;
    uint32 retsecond = 0;

    ptime = (uint8*)ptm;
    for(i = 0; i < 7; i++)
    {
        if (i == 3)
        {   // 剔除星期
            continue;
        }
        ptime[i] = BCDtoBIN(ptime[i]);
    }

    year = ptime[6] + 100 - 70;

    retsecond = year*YEAR_SECOND + ((year+1)/4)*DAY_SECOND;

    retsecond += MONTH_TABLE[ptime[5] - 1];

    if((ptime[5] > 1) && ((year + 2)%4))
    {
        retsecond -= DAY_SECOND;
    }

    retsecond +=  ((uint32)ptime[4]-1) * DAY_SECOND;

    retsecond +=  ((uint32)ptime[2]) * HOUR_SECOND;

    retsecond -= 8 * HOUR_SECOND;   // 转换为东八时区时间

    retsecond += ((uint32)ptime[1]) * MINUTE_SECOND;

    retsecond += (uint32)ptime[0];

    return (retsecond);
}

//===========================================================================================
const uint8 DAY_NUM_TABLE[12] = {0x31,0x28,0x31,0x30,0x31,0x30,0x31,0x31,0x30,0x31,0x30,0x31};

/*******************************************************************************
* 函数名称: JudgeTimeRight
* 函数功能: 判断时间是否合法，被通讯模块的电量清零通信函数所调用，实现电表电量的5次回零功能
* 输入参数: ptime  首地址
*           length 时间长度6 7
* 输出参数: 无
* 返 回 值: 0xee 不合法 , 0x00 合法 
*******************************************************************************/    
uint8 JudgeTimeRight(uint8 *ptime, uint8 length)
{    
    uint8 i;
    uint8 temp  = 0;
    uint8 temp0 = 0;

    if ((length != 6) && (length != 7))
    {
        return(0xee);    
    }
    for (i=0 ;i<length ;i++)
    {   // BCD码判定
        if((i == 3)&&(length == 7))
            continue;

        if(((ptime[i] & 0xf0)>=0xa0)||((ptime[i] & 0x0f)>=0x0a))
        {
            return(0xee);
        }
    }
    if ((ptime[0] >= 0x5a)||(ptime[1] >= 0x5a)||(ptime[2] >= 0x24))
    {
        return(0xee);
    }
    temp = ptime[4];
    if(length == 7)
    {
        temp = ptime[5];
    }
    if((temp ==0) || (temp >=0x13))
    {
        return(0xee);    
    }
    temp0 = BCDtoBIN(temp);
    temp0 -= 1;
    temp0 = DAY_NUM_TABLE[temp0];

    if(temp == 2)
    {
        temp = ptime[5];
        if(length == 7)
        {
            temp = ptime[6];    
        }
        temp = BCDtoBIN(temp);
        if((temp % 4) == 0)
        {
            temp0 += 1;
        }            
    }
    if((length == 7) && (ptime[5] > temp0))
    {
        return(0xee);    
    }
    if((length == 6) && (ptime[4] > temp0))
    {
        return(0xee);
    }
    return(0);
}

/*******************************************************************************
* 函数名称: TimeManageInit
* 函数功能: 时间管理初始化
* 输入参数: 无
* 输出参数: 无   
* 返 回 值: 无
*******************************************************************************/
void TimeManageInit(void)
{
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       systime;
    
    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &systime);

    gCommonData.CurTime.ss = Hex2Bcd((uint8)systime.tm_sec);
    gCommonData.CurTime.nn = Hex2Bcd((uint8)systime.tm_min);
    gCommonData.CurTime.hh = Hex2Bcd((uint8)systime.tm_hour);
    gCommonData.CurTime.DD = Hex2Bcd((uint8)systime.tm_mday);            
    gCommonData.CurTime.WW = Hex2Bcd((uint8)systime.tm_wday);         // 星期
    gCommonData.CurTime.MM = Hex2Bcd((uint8)systime.tm_mon + 1);      // 月份0代表1月
    gCommonData.CurTime.YY = Hex2Bcd((uint8)(systime.tm_year - 100)); // 取年份的后两位

    gCommonData.BuildTime.ss = gCommonData.CurTime.ss;
    gCommonData.BuildTime.nn = gCommonData.CurTime.nn;
    gCommonData.BuildTime.hh = gCommonData.CurTime.hh;
    gCommonData.BuildTime.DD = gCommonData.CurTime.DD;
    gCommonData.BuildTime.MM = gCommonData.CurTime.MM;
    gCommonData.BuildTime.YY = gCommonData.CurTime.YY;
}

/*******************************************************************************
* 函数名称: TimeManageStart
* 函数功能: 时间标志管理开始
* 输入参数: 无
* 输出参数: 无   
* 返 回 值: 无
*******************************************************************************/
void TimeManageStart(void)
{    
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       systime;
    
    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &systime);

    gCommonData.CurTime.ss = Hex2Bcd((uint8)systime.tm_sec);
    gCommonData.CurTime.nn = Hex2Bcd((uint8)systime.tm_min);
    gCommonData.CurTime.hh = Hex2Bcd((uint8)systime.tm_hour);
    gCommonData.CurTime.DD = Hex2Bcd((uint8)systime.tm_mday);            
    gCommonData.CurTime.WW = Hex2Bcd((uint8)systime.tm_wday);         // 星期
    gCommonData.CurTime.MM = Hex2Bcd((uint8)systime.tm_mon + 1);      // 月份0代表1月
    gCommonData.CurTime.YY = Hex2Bcd((uint8)(systime.tm_year - 100)); // 取年份的后两位

    if (gCommonData.CurTime.ss != gPrivateData.LastCompTime.ss)
    {
        gTimeFlag.BitSect.Second = TRUE;
        gCommonData.BuildTime.ss = gCommonData.CurTime.ss;
        gCommonData.BuildTime.nn = gCommonData.CurTime.nn;
        gCommonData.BuildTime.hh = gCommonData.CurTime.hh;
        gCommonData.BuildTime.DD = gCommonData.CurTime.DD;
        gCommonData.BuildTime.MM = gCommonData.CurTime.MM;
        gCommonData.BuildTime.YY = gCommonData.CurTime.YY;
    }

    if (gTimeFlag.BitSect.Second == TRUE)
    {    
        // 过月
        if (gCommonData.CurTime.MM != gPrivateData.LastCompTime.MM)
        {
            // PRINTX("past Day Hour Minute\n\r");
            // 同时产生过日、过时、过分标志
            gTimeFlag.BitSect.Month = TRUE;
            gTimeFlag.BitSect.Day = TRUE;
            gTimeFlag.BitSect.Hour = TRUE;
            gTimeFlag.BitSect.Minute = TRUE;
        }
        // 过日
        else if (gCommonData.CurTime.DD != gPrivateData.LastCompTime.DD)
        {
            // PRINTX("past Day Hour Minute\n\r");
            // 同时产生过日、过时、过分标志
            gTimeFlag.BitSect.Day = TRUE;
            gTimeFlag.BitSect.Hour = TRUE;
            gTimeFlag.BitSect.Minute = TRUE;
        }
        // 过时
        else if (gCommonData.CurTime.hh != gPrivateData.LastCompTime.hh)
        {
            // PRINTX("past Hour Minute\n\r");
            // 同时产生过时、过分标志
            gTimeFlag.BitSect.Hour = TRUE;
            gTimeFlag.BitSect.Minute = TRUE;
        }
        // 过分
        else if (gCommonData.CurTime.nn !=  gPrivateData.LastCompTime.nn)
        {
            // PRINTX("past Minute\n\r");
            // 产生过分标志
            gTimeFlag.BitSect.Minute = TRUE;
        }
    }
}

/*******************************************************************************
* 函数名称: TimeManageEnd
* 函数功能: 时间标志管理结束
* 输入参数: 无
* 输出参数: 无   
* 返 回 值: 无
*******************************************************************************/
void TimeManageEnd(void)
{
    // 刷新上次时间
    memcpy((uint8*)&gPrivateData.LastCompTime, (uint8*)&gCommonData.CurTime, sizeof(YYMMDDWWhhnnss_TYPE));

    gTimeFlag.Word = 0;       // 清除时间标志
}

/*******************************************************************************
* 函数名称: TimeManageInit
* 函数功能: 时间管理初始化
* 输入参数: 无
* 输出参数: 无   
* 返 回 值: 无
*******************************************************************************/
void SaveTimeManageRead(void)
{
    time_t now;
    struct tm *systime = NULL;    
    
    time(&now);
    systime = localtime(&now);     // 结构元素tm_sec,tm_min,tm_hour,tm_mday,tm_mon,tm_year

    gSaveCommonData.CurTime.ss = Hex2Bcd((uint8)systime->tm_sec);
    gSaveCommonData.CurTime.nn = Hex2Bcd((uint8)systime->tm_min);
    gSaveCommonData.CurTime.hh = Hex2Bcd((uint8)systime->tm_hour);
    gSaveCommonData.CurTime.DD = Hex2Bcd((uint8)systime->tm_mday);            
    gSaveCommonData.CurTime.WW = Hex2Bcd((uint8)systime->tm_wday);         // 星期
    gSaveCommonData.CurTime.MM = Hex2Bcd((uint8)systime->tm_mon + 1);      // 月份0代表1月
    gSaveCommonData.CurTime.YY = Hex2Bcd((uint8)(systime->tm_year - 100)); // 取年份的后两位
}

/*******************************************************************************
* 函数名称: TimeManageInit
* 函数功能: 时间管理初始化
* 输入参数: 无
* 输出参数: 无   
* 返 回 值: 无
*******************************************************************************/
void SaveTimeManageInit(void)
{
    time_t now;
    struct tm *systime = NULL;
    
    time(&now);
    systime = localtime(&now);          // 结构元素tm_sec,tm_min,tm_hour,tm_mday,tm_mon,tm_year

    gSaveCommonData.CurTime.ss = Hex2Bcd((uint8)systime->tm_sec);
    gSaveCommonData.CurTime.nn = Hex2Bcd((uint8)systime->tm_min);
    gSaveCommonData.CurTime.hh = Hex2Bcd((uint8)systime->tm_hour);
    gSaveCommonData.CurTime.DD = Hex2Bcd((uint8)systime->tm_mday);            
    gSaveCommonData.CurTime.WW = Hex2Bcd((uint8)systime->tm_wday);         // 星期
    gSaveCommonData.CurTime.MM = Hex2Bcd((uint8)systime->tm_mon + 1);      // 月份0代表1月
    gSaveCommonData.CurTime.YY = Hex2Bcd((uint8)(systime->tm_year - 100)); // 取年份的后两位

    gSaveCommonData.BuildTime.ss = gSaveCommonData.CurTime.ss;
    gSaveCommonData.BuildTime.nn = gSaveCommonData.CurTime.nn;
    gSaveCommonData.BuildTime.hh = gSaveCommonData.CurTime.hh;
    gSaveCommonData.BuildTime.DD = gSaveCommonData.CurTime.DD;
    gSaveCommonData.BuildTime.MM = gSaveCommonData.CurTime.MM;
    gSaveCommonData.BuildTime.YY = gSaveCommonData.CurTime.YY;
}

extern STA_PARAM_UPDATE g_staUpdateData;
/*******************************************************************************
* 函数名称: TimeManageStart
* 函数功能: 时间标志管理开始
* 输入参数: 无
* 输出参数: 无   
* 返 回 值: 无
*******************************************************************************/
void SaveTimeManageStart(void)
{    

    SaveTimeManageRead();
    
    pthread_mutex_lock(&g_staUpdateData.mutex);

    if(g_staUpdateData.timeUpdate == TRUE)
    {
        g_staUpdateData.timeUpdate = FALSE;
            
        //对时后重新读取当前时间到全局变量
        SaveTimeManageRead();
        sta_frozen_lasttime_reinit();
    }
    
    pthread_mutex_unlock(&g_staUpdateData.mutex); 

    // 过月
    if (gSaveCommonData.CurTime.MM != gSavePrivateData.LastCompTime.MM)
    {
        // PRINTX("past Day Hour Minute\n\r");
        // 同时产生过日、过时、过分标志
        gSaveTimeFlag.BitSect.Month = TRUE;
        gSaveTimeFlag.BitSect.Day = TRUE;
        gSaveTimeFlag.BitSect.Hour = TRUE;
        gSaveTimeFlag.BitSect.Minute = TRUE;
    }
    // 过日
    else if (gSaveCommonData.CurTime.DD != gSavePrivateData.LastCompTime.DD)
    {
        // PRINTX("past Day Hour Minute\n\r");
        // 同时产生过日、过时、过分标志
        gSaveTimeFlag.BitSect.Day = TRUE;
        gSaveTimeFlag.BitSect.Hour = TRUE;
        gSaveTimeFlag.BitSect.Minute = TRUE;
    }
    // 过时
    else if (gSaveCommonData.CurTime.hh != gSavePrivateData.LastCompTime.hh)
    {
        // PRINTX("past Hour Minute\n\r");
        // 同时产生过时、过分标志
        gSaveTimeFlag.BitSect.Hour = TRUE;
        gSaveTimeFlag.BitSect.Minute = TRUE;
    }
    // 过分
    else if (gSaveCommonData.CurTime.nn !=  gSavePrivateData.LastCompTime.nn)
    {
        // PRINTX("past Minute\n\r");
        // 产生过分标志
        gSaveTimeFlag.BitSect.Minute = TRUE;
    }
    else if (gSaveCommonData.CurTime.ss != gSavePrivateData.LastCompTime.ss)
    {
        gSaveTimeFlag.BitSect.Second = TRUE;
        gSaveCommonData.BuildTime.ss = gSaveCommonData.CurTime.ss;
        gSaveCommonData.BuildTime.nn = gSaveCommonData.CurTime.nn;
        gSaveCommonData.BuildTime.hh = gSaveCommonData.CurTime.hh;
        gSaveCommonData.BuildTime.DD = gSaveCommonData.CurTime.DD;
        gSaveCommonData.BuildTime.MM = gSaveCommonData.CurTime.MM;
        gSaveCommonData.BuildTime.YY = gSaveCommonData.CurTime.YY;
    }
}

/*******************************************************************************
* 函数名称: TimeManageEnd
* 函数功能: 时间标志管理结束
* 输入参数: 无
* 输出参数: 无   
* 返 回 值: 无
*******************************************************************************/
void SaveTimeManageEnd(void)
{
    // 刷新上次时间
    memcpy((uint8*)&gSavePrivateData.LastCompTime, (uint8*)&gSaveCommonData.CurTime, sizeof(YYMMDDWWhhnnss_TYPE));

    gSaveTimeFlag.Word = 0;       // 清除时间标志
}



/*******************************************************************************
* 函数名称: GetDayNum
* 函数功能: 获得指定月份的天数
* 输入参数: CurYear:指定年,        CurMon:指定月
* 输出参数: 无   
* 返 回 值: 该月份的天数
*******************************************************************************/
uint8 GetDayNum(uint16 CurYear, uint8 CurMon)
{
    if (CurMon != 2)
    {
        return HaveDays[CurMon];
    }
    else
    {
        // 需判断是否是闰年
        if ((CurYear % 4 == 0) && (CurYear % 100 != 0))
        {
            return 29;
        }
        else
        {
            return 28;
        }
    }
}

/*******************************************************************************
* 函数名称: GetNextMonth
* 函数功能: 获得若干月后时间
* 输入参数: DesDate:原时间值,BcdFlag:BCD标志(1:BCD码/0:BIN码)
* 输出参数: DesDate:计算后的时间值   
* 返 回 值: 无
*******************************************************************************/
void GetNextMonth(YYMMDDhhnnss_TYPE *DesDate, uint32 MonCnt, uint8 BcdFlag)
{
    uint8  TempDate[6] = {0};                        // 临时时间
    int    i;

    if (BcdFlag == 1)
    {
        // 将BCD码时间转换为BIN码
        TempDate[0] = (int8)Bcd2Hex(DesDate->ss);
        TempDate[1] = (int8)Bcd2Hex(DesDate->nn);
        TempDate[2] = (int8)Bcd2Hex(DesDate->hh);
        TempDate[3] = (int8)Bcd2Hex(DesDate->DD);
        TempDate[4] = (int8)Bcd2Hex(DesDate->MM);
        TempDate[5] = (int8)Bcd2Hex(DesDate->YY);
    }
    else
    {
        memcpy(TempDate, (uint8*)DesDate, 6);
    }

    for (i=0; i < MonCnt; i++)
    {
        TempDate[4]++;                            // 月++;
        if (TempDate[4] > 12)
        {
            TempDate[4] = 1;                    // 下年的1月
            TempDate[5]++;                        // 年++;
            if (TempDate[5] > 99)                // 考虑1999向2000切换
            {
                TempDate[5] = 0;
                //TempDate[6]++;
            }
        }
    }

    if (BcdFlag == 1)
    {
        // 将BIN码时间转换为BCD码
        DesDate->ss = Hex2Bcd((uint8)TempDate[0]);
        DesDate->nn = Hex2Bcd((uint8)TempDate[1]);
        DesDate->hh = Hex2Bcd((uint8)TempDate[2]);
        DesDate->DD = Hex2Bcd((uint8)TempDate[3]);
        DesDate->MM = Hex2Bcd((uint8)TempDate[4]);
        DesDate->YY = Hex2Bcd((uint8)TempDate[5]);
    }
    else
    {
        memcpy((uint8*)DesDate, TempDate, 6);
    }
}

/*******************************************************************************
* 函数名称: GetNextDay
* 函数功能: 获得若干日后时间
* 输入参数: DesDate:原时间值,BcdFlag:BCD标志(1:BCD码/0:BIN码)
* 输出参数: DesDate:计算后的时间值
* 返 回 值: 无
*******************************************************************************/
void GetNextDay(YYMMDDhhnnss_TYPE *DesDate, uint32 DayCnt, uint8 BcdFlag)
{
    uint8  TempDate[6] = {0};      // 临时时间
    uint8  DaysOfMon = 0;              // 该月天数
    int    i;

    if (BcdFlag == 1)
    {
        // 将BCD码时间转换为BIN码
        TempDate[0] = (int8)Bcd2Hex(DesDate->ss);
        TempDate[1] = (int8)Bcd2Hex(DesDate->nn);
        TempDate[2] = (int8)Bcd2Hex(DesDate->hh);
        TempDate[3] = (int8)Bcd2Hex(DesDate->DD);
        TempDate[4] = (int8)Bcd2Hex(DesDate->MM);
        TempDate[5] = (int8)Bcd2Hex(DesDate->YY);
    }
    else
    {
        memcpy(TempDate, (uint8*)DesDate, 6);
    }

    for (i=0; i<DayCnt; i++)
    {
        TempDate[3]++;                          // 日++
        DaysOfMon = GetDayNum((2000+TempDate[5]), TempDate[4]);
        if (TempDate[3] > DaysOfMon)            // 大于该月的天数
        {
            TempDate[3] = 1;                    // 下月的1日
            // 下一月
            GetNextMonth((YYMMDDhhnnss_TYPE*)TempDate, 1, 0);        
        }
    }

    if (BcdFlag == 1)
    {
        // 将BIN码时间转换为BCD码
        DesDate->ss = Hex2Bcd((uint8)TempDate[0]);
        DesDate->nn = Hex2Bcd((uint8)TempDate[1]);
        DesDate->hh = Hex2Bcd((uint8)TempDate[2]);
        DesDate->DD = Hex2Bcd((uint8)TempDate[3]);
        DesDate->MM = Hex2Bcd((uint8)TempDate[4]);
        DesDate->YY = Hex2Bcd((uint8)TempDate[5]);
    }
    else
    {
        memcpy((uint8*)DesDate, TempDate, 6);
    }
}

/*******************************************************************************
* 函数名称: GetNextHour
* 函数功能: 获得若干时后时间
* 输入参数: DesDate:原时间值,BcdFlag:BCD标志(1:BCD码/0:BIN码)
* 输出参数: DesDate:计算后的时间值
* 返 回 值: 无
*******************************************************************************/
void GetNextHour(YYMMDDhhnnss_TYPE *DesDate, uint32 HourCnt, uint8 BcdFlag)
{
    uint8   TempDate[6] = {0};          // 临时时间
    uint32  DayCnt = 0;                 // 整天数
    uint32  RemainCnt = 0;              // 剩余小时数
    int     i;

    if (BcdFlag == 1)
    {
        // 将BCD码时间转换为BIN码
        TempDate[0] = (int8)Bcd2Hex(DesDate->ss);
        TempDate[1] = (int8)Bcd2Hex(DesDate->nn);
        TempDate[2] = (int8)Bcd2Hex(DesDate->hh);
        TempDate[3] = (int8)Bcd2Hex(DesDate->DD);
        TempDate[4] = (int8)Bcd2Hex(DesDate->MM);
        TempDate[5] = (int8)Bcd2Hex(DesDate->YY);
    }
    else
    {
        memcpy(TempDate, (uint8*)DesDate, 6);
    }

    // MinCnt分钟转换的整天数
    DayCnt = HourCnt / 24;

    // 计算剩余分钟数
    RemainCnt = HourCnt - (DayCnt*24);

    // 获得若干整天后的时间
    if (DayCnt > 0)
    {
        GetNextDay((YYMMDDhhnnss_TYPE*)TempDate, DayCnt, 0);
    }

    for (i=0; i< RemainCnt; i++)
    {
        TempDate[2]++;                  // 时++
        if (TempDate[2] >= 24)          // 过日
        {
            TempDate[2] = 0;
            // 下一日
            GetNextDay((YYMMDDhhnnss_TYPE*)TempDate, 1, 0);            
        }
    }

    if (BcdFlag == 1)
    {
        // 将BIN码时间转换为BCD码
        DesDate->ss = Hex2Bcd((uint8)TempDate[0]);
        DesDate->nn = Hex2Bcd((uint8)TempDate[1]);
        DesDate->hh = Hex2Bcd((uint8)TempDate[2]);
        DesDate->DD = Hex2Bcd((uint8)TempDate[3]);
        DesDate->MM = Hex2Bcd((uint8)TempDate[4]);
        DesDate->YY = Hex2Bcd((uint8)TempDate[5]);
    }
    else
    {
        memcpy((uint8*)DesDate, TempDate, 6);
    }
}

/*******************************************************************************
* 函数名称: GetNextMin
* 函数功能: 获得若干分钟后时间
* 输入参数: DesDate:原时间值,BcdFlag:BCD标志(1:BCD码/0:BIN码)
* 输出参数: DesDate:计算后的时间值
* 返 回 值: 无
*******************************************************************************/
void GetNextMin(YYMMDDhhnnss_TYPE *DesDate, uint32 MinCnt, uint8 BcdFlag)
{
    uint8   TempDate[6] = {0};               // 临时时间
    uint32  DayCnt  = 0;                     // 整天数
    uint32  HourCnt = 0;                     // 整时数
    uint32  RemainCnt = 0;                   // 剩余分钟数
    int     i;

    if (BcdFlag == 1)
    {
        // 将BCD码时间转换为BIN码
        TempDate[0] = (int8)Bcd2Hex(DesDate->ss);
        TempDate[1] = (int8)Bcd2Hex(DesDate->nn);
        TempDate[2] = (int8)Bcd2Hex(DesDate->hh);
        TempDate[3] = (int8)Bcd2Hex(DesDate->DD);
        TempDate[4] = (int8)Bcd2Hex(DesDate->MM);
        TempDate[5] = (int8)Bcd2Hex(DesDate->YY);
    }
    else
    {
        memcpy(TempDate, (uint8*)DesDate, 6);
    }

    // MinCnt分钟转换的整天数
    DayCnt = MinCnt / 1440;

    // 计算剩余分钟数
    RemainCnt = MinCnt - (DayCnt*1440);

    // 获得若干整天后的时间
    if (DayCnt > 0)
    {
        GetNextDay((YYMMDDhhnnss_TYPE*)TempDate, DayCnt, 0);
    }

    // RemainCnt分钟转换的整时数
    HourCnt = RemainCnt / 60;

    // 计算剩余分钟数
    RemainCnt = RemainCnt - (HourCnt*60);

    // 获得若干小时后的时间
    if (HourCnt > 0)
    {
        GetNextHour((YYMMDDhhnnss_TYPE*)TempDate, HourCnt, 0);
    }

    for (i=0; i< RemainCnt; i++)
    {
        TempDate[1]++;                            // 分++
        if (TempDate[1] >= 60)                    // 过时
        {
            TempDate[1] = 0;
            // 下一时
            GetNextHour((YYMMDDhhnnss_TYPE*)TempDate, 1, 0);        
        }
    }

    if (BcdFlag == 1)
    {
        // 将BIN码时间转换为BCD码
        DesDate->ss = Hex2Bcd((uint8)TempDate[0]);
        DesDate->nn = Hex2Bcd((uint8)TempDate[1]);
        DesDate->hh = Hex2Bcd((uint8)TempDate[2]);
        DesDate->DD = Hex2Bcd((uint8)TempDate[3]);
        DesDate->MM = Hex2Bcd((uint8)TempDate[4]);
        DesDate->YY = Hex2Bcd((uint8)TempDate[5]);
    }
    else
    {
        memcpy((uint8*)DesDate, TempDate, 6);
    }
}

/*******************************************************************************
* 函数名称: GetLastMonth
* 函数功能: 获得若干月后时间
* 输入参数: DesDate:原时间值,BcdFlag:BCD标志(1:BCD码/0:BIN码)
* 输出参数: DesDate:计算后的时间值
* 返 回 值: 无
*******************************************************************************/
void GetLastMonth(YYMMDDhhnnss_TYPE *DesDate, uint32 MonCnt, uint8 BcdFlag)
{
    int8  TempDate[6]={0};     // 临时时间
    int    i;

    if (BcdFlag == 1)
    {
        // 将BCD码时间转换为BIN码
        TempDate[0] = (int8)Bcd2Hex(DesDate->ss);
        TempDate[1] = (int8)Bcd2Hex(DesDate->nn);
        TempDate[2] = (int8)Bcd2Hex(DesDate->hh);
        TempDate[3] = (int8)Bcd2Hex(DesDate->DD);
        TempDate[4] = (int8)Bcd2Hex(DesDate->MM);
        TempDate[5] = (int8)Bcd2Hex(DesDate->YY);
    }
    else
    {
        memcpy(TempDate, (uint8*)DesDate, 6);
    }

    for (i=0; i<MonCnt; i++)
    {
        TempDate[4]--;                            // 月--;
        if (TempDate[4] == 0)
        {
            TempDate[4] = 12;                     // 上月的12月
            TempDate[5]--;                        // 年--;
            if (TempDate[5] < 0)                  // 考虑2000向1999切换
            {
                TempDate[5] = 99;
                //TempDate[6]--;
            }
        }
    }

    if (BcdFlag == 1)
    {
        // 将BIN码时间转换为BCD码
        DesDate->ss = Hex2Bcd((uint8)TempDate[0]);
        DesDate->nn = Hex2Bcd((uint8)TempDate[1]);
        DesDate->hh = Hex2Bcd((uint8)TempDate[2]);
        DesDate->DD = Hex2Bcd((uint8)TempDate[3]);
        DesDate->MM = Hex2Bcd((uint8)TempDate[4]);
        DesDate->YY = Hex2Bcd((uint8)TempDate[5]);
    }
    else
    {
        memcpy((uint8*)DesDate, TempDate, 6);
    }
}

/*******************************************************************************
* 函数名称: GetLastDay
* 函数功能: 获得若干日前时间
* 输入参数: DesDate:原时间值；BcdFlag:BCD标志(1:BCD码/0:BIN码)
* 输出参数: DesDate:计算后的时间值
* 返 回 值: 无
*******************************************************************************/
void GetLastDay(YYMMDDhhnnss_TYPE *DesDate, uint32 DayCnt, uint8 BcdFlag)
{
    int8    TempDate[6] = {0};      // 临时时间
    uint8    DaysOfMon = 0;          // 该月天数
    int      i;

    if (BcdFlag == 1)
    {
        // 将BCD码时间转换为BIN码
        TempDate[0] = (int8)Bcd2Hex(DesDate->ss);
        TempDate[1] = (int8)Bcd2Hex(DesDate->nn);
        TempDate[2] = (int8)Bcd2Hex(DesDate->hh);
        TempDate[3] = (int8)Bcd2Hex(DesDate->DD);
        TempDate[4] = (int8)Bcd2Hex(DesDate->MM);
        TempDate[5] = (int8)Bcd2Hex(DesDate->YY);
    }
    else
    {
        memcpy(TempDate, (uint8*)DesDate, 6);
    }

    for (i=0; i<DayCnt; i++)
    {
        TempDate[3]--;                            // 日--
        if (TempDate[3] == 0)                     // 大于该月的天数
        {
            // 上一月
            GetLastMonth((YYMMDDhhnnss_TYPE*)TempDate, 1, 0);        
            DaysOfMon = GetDayNum((2000+TempDate[5]), TempDate[4]);
            TempDate[3] = DaysOfMon;              // 上月的最后1日
        }
    }

    if (BcdFlag == 1)
    {
        // 将BIN码时间转换为BCD码
        DesDate->ss = Hex2Bcd((uint8)TempDate[0]);
        DesDate->nn = Hex2Bcd((uint8)TempDate[1]);
        DesDate->hh = Hex2Bcd((uint8)TempDate[2]);
        DesDate->DD = Hex2Bcd((uint8)TempDate[3]);
        DesDate->MM = Hex2Bcd((uint8)TempDate[4]);
        DesDate->YY = Hex2Bcd((uint8)TempDate[5]);
    }
    else
    {
        memcpy((uint8*)DesDate, TempDate, 6);
    }
}

/*******************************************************************************
* 函数名称: GetLastHour
* 函数功能: 获得若干时前时间
* 输入参数: DesDate:原时间值,BcdFlag:BCD标志(1:BCD码/0:BIN码)
* 输出参数: DesDate:计算后的时间值
* 返 回 值: 无
*******************************************************************************/
void GetLastHour(YYMMDDhhnnss_TYPE *DesDate, uint32 HourCnt, uint8 BcdFlag)
{
    int8   TempDate[6] = {0};                    // 临时时间
    uint32  DayCnt = 0;                           // 整天数
    uint32  RemainCnt = 0;                        // 剩余小时数
    int     i;

    if (BcdFlag == 1)
    {
        // 将BCD码时间转换为BIN码
        TempDate[0] = (int8)Bcd2Hex(DesDate->ss);
        TempDate[1] = (int8)Bcd2Hex(DesDate->nn);
        TempDate[2] = (int8)Bcd2Hex(DesDate->hh);
        TempDate[3] = (int8)Bcd2Hex(DesDate->DD);
        TempDate[4] = (int8)Bcd2Hex(DesDate->MM);
        TempDate[5] = (int8)Bcd2Hex(DesDate->YY);
    }
    else
    {
        memcpy(TempDate, (uint8*)DesDate, 6);
    }

    // MinCnt分钟转换的整天数
    DayCnt = HourCnt / 24;

    // 计算剩余分钟数
    RemainCnt = HourCnt - (DayCnt*24);

    // 获得若干整天后的时间
    if (DayCnt > 0)
    {
        GetLastDay((YYMMDDhhnnss_TYPE*)TempDate, DayCnt, 0);
    }


    for (i=0; i< RemainCnt; i++)
    {
        TempDate[2]--;                          // 时--
        if (TempDate[2] < 0)                    // 过日
        {
            TempDate[2] = 23;
            // 上一日
            GetLastDay((YYMMDDhhnnss_TYPE*)TempDate, 1, 0);    
        }
    }

    if (BcdFlag == 1)
    {
        // 将BIN码时间转换为BCD码
        DesDate->ss = Hex2Bcd((uint8)TempDate[0]);
        DesDate->nn = Hex2Bcd((uint8)TempDate[1]);
        DesDate->hh = Hex2Bcd((uint8)TempDate[2]);
        DesDate->DD = Hex2Bcd((uint8)TempDate[3]);
        DesDate->MM = Hex2Bcd((uint8)TempDate[4]);
        DesDate->YY = Hex2Bcd((uint8)TempDate[5]);
    }
    else
    {
        memcpy((uint8*)DesDate, TempDate, 6);
    }
}

/*******************************************************************************
* 函数名称: GetLastMin
* 函数功能: 获得若干分钟前时间
* 输入参数: DesDate:原时间值,BcdFlag:BCD标志(1:BCD码/0:BIN码)
* 输出参数: DesDate:计算后的时间值
* 返 回 值: 无
*******************************************************************************/
void GetLastMin(YYMMDDhhnnss_TYPE *DesDate, uint32 MinCnt, uint8 BcdFlag)
{
    int8   TempDate[6] = {0};          // 临时时间
    uint32  DayCnt  = 0;                // 整天数
    uint32  HourCnt = 0;                // 整时数
    uint32  RemainCnt = 0;              // 剩余分钟数
    int        i;

    if (BcdFlag == 1)
    {
        // 将BCD码时间转换为BIN码
        TempDate[0] = (int8)Bcd2Hex(DesDate->ss);
        TempDate[1] = (int8)Bcd2Hex(DesDate->nn);
        TempDate[2] = (int8)Bcd2Hex(DesDate->hh);
        TempDate[3] = (int8)Bcd2Hex(DesDate->DD);
        TempDate[4] = (int8)Bcd2Hex(DesDate->MM);
        TempDate[5] = (int8)Bcd2Hex(DesDate->YY);
    }
    else
    {
        memcpy(TempDate, (uint8*)DesDate, 6);
    }

    // MinCnt分钟转换的整天数
    DayCnt = MinCnt / 1440;

    // 计算剩余分钟数
    RemainCnt = MinCnt - (DayCnt*1440);

    // 获得若干整天后的时间
    if (DayCnt > 0)
    {
        GetLastDay((YYMMDDhhnnss_TYPE*)TempDate, DayCnt, 0);
    }

    // RemainCnt分钟转换的整时数
    HourCnt = RemainCnt / 60;

    // 计算剩余分钟数
    RemainCnt = RemainCnt - (HourCnt*60);

    // 获得若干小时后的时间
    if (HourCnt > 0)
    {
        GetLastHour((YYMMDDhhnnss_TYPE*)TempDate, HourCnt, 0);
    }

    for (i=0; i< RemainCnt; i++)
    {
        TempDate[1]--;                            // 分--
        if (TempDate[1] < 0)                    // 过时
        {
            TempDate[1] = 59;
            // 上一时
            GetLastHour((YYMMDDhhnnss_TYPE*)TempDate, 1, 0);
        }
    }

    if (BcdFlag == 1)
    {
        // 将BIN码时间转换为BCD码
        DesDate->ss = Hex2Bcd((uint8)TempDate[0]);
        DesDate->nn = Hex2Bcd((uint8)TempDate[1]);
        DesDate->hh = Hex2Bcd((uint8)TempDate[2]);
        DesDate->DD = Hex2Bcd((uint8)TempDate[3]);
        DesDate->MM = Hex2Bcd((uint8)TempDate[4]);
        DesDate->YY = Hex2Bcd((uint8)TempDate[5]);
    }
    else
    {
        memcpy((uint8*)DesDate, TempDate, 6);
    }
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
    WeekDay++;
    if (WeekDay == 7)
    {
        WeekDay = 0;
    }

    DesTime->WW = WeekDay;
}

/*******************************************************************************
* 函数名称: CompareTime
* 函数功能: 检查2个时间值的大小
* 输入参数: DesTime    比较的目的时间
*           SrcTime    比较的源时间
* 输出参数: 无   
* 返 回 值: 0      目的时间=源时间
*           1      目的时间>源时间
*           -1     目的时间<源时间
*******************************************************************************/
int8 CompareTime(YYMMDDhhnnss_TYPE DesTime, YYMMDDhhnnss_TYPE SrcTime)
{
    // 比较年
    if (DesTime.YY > SrcTime.YY)
    {
        return 1;
    }
    else if (DesTime.YY < SrcTime.YY)
    {
        return -1;
    }
    // 比较月
    if (DesTime.MM > SrcTime.MM)
    {
        return 1;
    }
    else if (DesTime.MM < SrcTime.MM)
    {
        return -1;
    }
    // 比较日
    if (DesTime.DD > SrcTime.DD)
    {
        return 1;
    }
    else if (DesTime.DD < SrcTime.DD)
    {
        return -1;
    }    
    // 比较时
    if (DesTime.hh > SrcTime.hh)
    {
        return 1;
    }
    else if (DesTime.hh < SrcTime.hh)
    {
        return -1;
    }    
    // 比较分
    if (DesTime.nn > SrcTime.nn)
    {
        return 1;
    }
    else if (DesTime.nn < SrcTime.nn)
    {
        return -1;
    }    
    // 比较秒
    if (DesTime.ss > SrcTime.ss)
    {
        return 1;
    }
    else if (DesTime.ss < SrcTime.ss)
    {
        return -1;
    }

    return 0;
}

/*******************************************************************************
* 函数名称: Time2Sec
* 函数功能: YYMMDDWWhhnnss_TYPE转换为秒数
* 输入参数: SrcData                需要转换的日期
* 输出参数: 无   
* 返 回 值: 从2000年1月1日0:0:0开始到SrcData的秒数
*******************************************************************************/
uint32 Time2Sec(YYMMDDWWhhnnss_TYPE SrcData)
{
    uint32  SecCnt = 0;  // 秒数
    int     i;

    SecCnt = (Bcd2Hex(SrcData.YY) / 4) * FOUR_YEARS_TO_DAYS + (Bcd2Hex(SrcData.YY) % 4) * 365; 

    if (((Bcd2Hex(SrcData.YY) % 4) != 0) || (Bcd2Hex(SrcData.MM) > 2))
    {
        SecCnt++;
    }

    for (i = 1; i < Bcd2Hex(SrcData.MM); i++)
    {
        SecCnt += HaveDays[i];
    }

    SecCnt += Bcd2Hex(SrcData.DD) - 1;

    return ((SecCnt * 24 + Bcd2Hex(SrcData.hh)) * 60 + Bcd2Hex(SrcData.nn) * 60 + Bcd2Hex(SrcData.ss));
}

/*******************************************************************************
* 函数名称: TimetoSec
* 函数功能: YYMMDDWWhhnnss_TYPE转换为秒数
* 输入参数: SrcData                需要转换的日期
* 输出参数: 无   
* 返 回 值: 从1970年1月1日0:0:0开始到SrcData的秒数
*******************************************************************************/
time_t TimetoSec(YYMMDDWWhhnnss_TYPE SrcData)
{
    struct tm des_tm;       //标准的tm时钟结构
    des_tm.tm_year = (commfun_BcdToHex(SrcData.YY) > 70 ? commfun_BcdToHex(SrcData.YY) : (commfun_BcdToHex(SrcData.YY) + 100));
    des_tm.tm_mon = commfun_BcdToHex(SrcData.MM) - 1;
    des_tm.tm_mday = commfun_BcdToHex(SrcData.DD);
    des_tm.tm_hour = commfun_BcdToHex(SrcData.hh);
    des_tm.tm_min = commfun_BcdToHex(SrcData.nn);
    des_tm.tm_sec = commfun_BcdToHex(SrcData.ss);
    des_tm.tm_isdst = -1;           //忽略闰秒   
    return FastDateToSecond(&des_tm);//从1970年1月1日0时0分0秒到此时的秒数
}

/*******************************************************************************
* 函数名称: CheckBcdValidity
* 函数功能: 检查BCD码有效性
* 输入参数: SrcData  需要检查的BCD码串
*           DataLen  数据长度
* 输出参数: 无   
* 返 回 值: TRUE     有效
*           FALSE    无效
*******************************************************************************/
uint8 CheckBcdValidity(uint8 *SrcData, uint8 DataLen)
{
    int i;

    for (i = 0; i < DataLen; i++)
    {
        if ((SrcData[i] & 0x0F) >= 0x0A)
        {
            return FALSE;
        }
        if ((SrcData[i] & 0xF0) >= 0xA0)
        {
            return FALSE;
        }
    }
    
    return TRUE;
}

/*******************************************************************************
* 函数名称: CheckTimeValidity
* 函数功能: 检查时间有效性
* 输入参数: SrcTime   需要检查的时间格式
*           DataLen   数据长度
* 输出参数: 无   
* 返 回 值: TRUE    有效
*           FALSE   无效
* 备    注: 只考虑以下6种格式:
*           DDhh_TYPE、hhnnss_TYPE、MMDDhhnn_TYPE、YYMMDDhhnn_TYPE、
*           YYMMDDhhnnss_TYPE 和 YYMMDDWWhhnnss_TYPE
*******************************************************************************/
uint8 CheckTimeValidity(uint8 *SrcData, uint8 DataLen)
{
    DDhh_TYPE            SrcTime2;
    hhnnss_TYPE          SrcTime3;
    MMDDhhnn_TYPE        SrcTime4;
    YYMMDDhhnn_TYPE      SrcTime5;
    YYMMDDhhnnss_TYPE    SrcTime6;
    YYMMDDWWhhnnss_TYPE  SrcTime7;

    // 首先进行BCD码检测
    if (CheckBcdValidity(SrcData, DataLen) == FALSE)
    {
        return FALSE;
    }

    switch (DataLen)
    {
    case 0x02:                                    // DDhh
        // 套用到当前格式
        memcpy((uint8*)&SrcTime2, SrcData, DataLen);

        // 日判断
        if ((SrcTime2.DD == 0x00) || (SrcTime2.DD > 0x31))
        {    
            return FALSE;
        }
        if (SrcTime2.hh > 0x23)                   // 时判断
        {    
            return FALSE;
        }

        break;

    case 0x03:                                    // hhnnss
        // 套用到当前格式
        memcpy((uint8*)&SrcTime3, SrcData, DataLen);

        if (SrcTime3.hh > 0x23)                   // 时判断
        {    
            return FALSE;
        }
        if (SrcTime3.nn > 0x59)                   // 分判断
        {    
            return FALSE;
        }
        if (SrcTime3.ss > 0x59)                   // 秒判断
        {    
            return FALSE;
        }

        break;

    case 0x04:                                    // YYMMDDWW
        // 套用到当前格式
        memcpy((uint8*)&SrcTime4, SrcData, DataLen);

        // 月判断
        if ((SrcTime4.MM == 0x00) || (SrcTime4.MM > 0x12))
        {    
            return FALSE;
        }
        // 日判断
        if ((SrcTime4.DD == 0x00) || (SrcTime4.DD > 0x31))
        {    
            return FALSE;
        }
        if (SrcTime4.hh > 0x23)                   // 时判断
        {    
            return FALSE;
        }
        if (SrcTime4.nn > 0x59)                   // 分判断
        {    
            return FALSE;
        }

        break;

    case 0x05:                                    // YYMMDDhhnn
        // 套用到当前格式
        memcpy((uint8*)&SrcTime5, SrcData, DataLen);

        // 月判断
        if ((SrcTime5.MM == 0x00) || (SrcTime5.MM > 0x12))
        {    
            return FALSE;
        }
        // 日判断
        if ((SrcTime5.DD == 0x00) || (SrcTime5.DD > 0x31))
        {    
            return FALSE;
        }
        if (SrcTime5.hh > 0x23)                   // 时判断
        {    
            return FALSE;
        }
        if (SrcTime5.nn > 0x59)                   // 分判断
        {    
            return FALSE;
        }

        break;

    case 0x06:                                    // YYMMDDhhnnss
        // 套用到当前格式
        memcpy((uint8*)&SrcTime6, SrcData, DataLen);

        // 月判断
        if ((SrcTime6.MM == 0x00) || (SrcTime6.MM > 0x12))
        {    
            return FALSE;
        }
        // 日判断
        if ((SrcTime6.DD == 0x00) || (SrcTime6.DD > 0x31))
        {    
            return FALSE;
        }
        if (SrcTime6.hh > 0x23)                   // 时判断
        {    
            return FALSE;
        }
        if (SrcTime6.nn > 0x59)                   // 分判断
        {    
            return FALSE;
        }
        if (SrcTime6.ss > 0x59)                   // 秒判断
        {    
            return FALSE;
        }

        break;

    case 0x07:                                    // YYMMDDWWhhnnss
        // 套用到当前格式
        memcpy((uint8*)&SrcTime7, SrcData, DataLen);

        // 月判断
        if ((SrcTime7.MM == 0x00) || (SrcTime7.MM > 0x12))
        {    
            return FALSE;
        }
        // 日判断
        if ((SrcTime7.DD == 0x00) || (SrcTime7.DD > 0x31))
        {    
            return FALSE;
        }
        if (SrcTime7.hh > 0x23)                   // 时判断
        {    
            return FALSE;
        }
        if (SrcTime7.nn > 0x59)                   // 分判断
        {    
            return FALSE;
        }
        if (SrcTime7.ss > 0x59)                   // 秒判断
        {    
            return FALSE;
        }

        break;

    default:
        break;
    }

    return TRUE;
}

/*******************************************************************************
* 函数名称: CalWeek
* 函数功能: 根据时间计算星期几
* 输入参数: pTime  时间 ssmmhhDDMMYY
*           Format 格式，0--BCD码，1--BIN格式
* 输出参数: 无
* 返 回 值: 星期 1~7 -- 星期一至星期日；0 失败
*******************************************************************************/
uint8 CalWeek(const uint8 *const pTime, uint8 Format)
{
    uint8  Week = 0; // 星期
    uint16 y = 0;    // 年
    uint16 c = 20;   // 世纪减1，现在限于21世纪
    uint16 m = 0;    // 月，3~12月不变，1、2月==>13、14
    uint16 d = 0;    // 日
    uint8  tmp = 0;

    if (pTime == NULL)
    {
        return 0;
    }

    if (Format == 0) // BCD
    {
        y = Bcd2Hex(pTime[5]);
        tmp = Bcd2Hex(pTime[4] & 0x1F);
        m = (tmp >= 3 ? tmp : (tmp+12));
        d = Bcd2Hex(pTime[3] & 0x3F);
    }
    else // BIN
    {
        y = pTime[5] & 0x7F;
        tmp = pTime[4] & 0x0F;
        m = (tmp >= 3 ? tmp : (tmp+12));
        d = pTime[3] & 0x1F; // 日
    }
    
    Week = (y + y/4 + c/4 - 2*c + 26*(m+1)/10 + d - 1) % 7;
    if (Week >= 0x80) // 防止负余数
    {
        Week = 256 - Week;
    }
    if (Week == 0)
    {
        Week = 7;
    }
    
    return Week;    
}


/*******************************************************************************
* 函数名称: BCD4_To_BIN8
* 函数功能: X字节BCD码转换为Y字节BIN码
* 输入参数: X                   BCD码字节数
*           Y                   BIN码字节数
*           SrcBcd              4字节BCD码
*           Multi               放大倍数
* 输出参数: DesBin              Y字节BIN码
* 返 回 值: 无
*******************************************************************************/
void BcdX_To_BinY(uint8 X, uint8 Y, uint8 *DesBin, uint8 *SrcBcd, uint16 Multi)
{
    uint8   i;
    uint8   TempBuff[8] = {0};
    uint64  TempU64 = 0;

    for(i = 0; i < X; i++)
    {
        TempBuff[i] = ((SrcBcd[i] >> 4) * 10) + (SrcBcd[i] & 0x0f);
    }
    for(i = X; i > 0; i--)
    {
        TempU64 = TempU64 * 100 + TempBuff[i - 1];
    }
    TempU64 *= Multi;

    if (Y == 8)
    {
        *(uint64 *)DesBin = TempU64;
    }
    else if (Y == 4)
    {
        *(uint32 *)DesBin = (uint32)TempU64;
    }
    else if (Y == 2)
    {
        *(uint16 *)DesBin = (uint16)TempU64;
    }
	else if (Y == 1)
    {
        *(uint8 *)DesBin = (uint8)TempU64;
    }
}

void BcdX_To_SBinY(uint8 X, uint8 Y, uint8 *DesBin, uint8 *SrcBcd, uint16 Multi)
{
    uint8   i;
    uint8   TempBuff[4] = {0};
    uint64  TempU64 = 0;
    int8 flag = 0;

    if((SrcBcd[X - 1] & 0x80) == 0x80)
    {
        flag = -1;
        SrcBcd[X - 1] = SrcBcd[X - 1] & 0x7f;
    }

    for(i = 0; i < X; i++)
    {
        TempBuff[i] = ((SrcBcd[i] >> 4) * 10) + (SrcBcd[i] & 0x0f);
    }
    for(i = X; i > 0; i--)
    {
        TempU64 = TempU64 * 100 + TempBuff[i - 1];
    }
    TempU64 *= Multi;

    if(Y == 8)
    {
        *(uint64 *)DesBin = TempU64;
    }
    else if(Y == 4)
    {
        *(uint32 *)DesBin = (uint32)TempU64;
    }
    else if(Y == 2)
    {
        *(uint16 *)DesBin = (uint16)TempU64;
    }
	else if(Y == 1)
    {
        *(uint8 *)DesBin = (uint8)TempU64;
    }
    if(flag == -1)
    {
        *(uint64 *)DesBin |= (1 << (Y * 8 - 1));
    }

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

/**********************************************************************
* @name      : Uint16To32
* @brief     ：两字节拼接成四字节
* @param[in] ：uint16 front
               uint16 rear  
* @param[out]：
* @return    ：拼接成四字节数据
* @Create    : 梅安宁
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
uint32 Uint16To32(uint16 front, uint16 rear)
{
    uint32 value = 0;
    value = (0xffffffff&front)<<16;
    value = value|rear;
    return value;
}

/**********************************************************************
* @name      : current_tm_to_ooptm
* @brief     ：获取当前OOP_DATETIME_S_T格式时间
* @param[in] ：
* @param[out]：OOP_DATETIME_S_T *poop_time      
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-4-13
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

