/****************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
*
* 文件名称：Common698.h
* 文件标识：
* 版本号：1.0
*
* 当前修订版本：
* 修改简要说明:04规约通用库文件
* 修订者：
* 完成日期：
*
* 原始版本：1.0
* 原作者　：郑健锋
* 完成日期：
*******************************************************************/
#ifndef _COMMON698_H
#define _COMMON698_H

#define ALL_EN_FLAG 123456L
#define DEPRINT(en_flag,fmt...) ({\
	if(getenv("DEBUG") != NULL){\
	if((atoi(getenv("DEBUG")) == en_flag)||(atoi(getenv("DEBUG")) == ALL_EN_FLAG))\
		{printf("%s----%d...  ", __FILE__, __LINE__);	printf(fmt);}}\
})

extern pthread_mutex_t App_GetData_lock;        // 建立读数据的互斥锁
                                                // 任务获取数据时，规约解释也可能同时获取数据其中使用到共同的全局变量
extern pthread_mutex_t CalRemainEn_lock;		// 建立计算剩余电量的互斥信号量

/*
  名  称: write_log
  功  能: 写日志(本身带写入的时间),超过20K，重新从文件头开始写
  输  入:
      filepath - 文件路径
      str      - 字符串
  输  出:
    (none)
  返回值:
    < 0 - 错误
    = 0 - 成功

 */
int write_log(const char *filepath, const char *str);

// 记录明文APDU方便查看日志
void WriteLogSecurity(BOOL IsSecurityApdu, BOOL recvflag, const char *fileName, const uint8 *dataBuf, const uint16 dataLen);

/*-把带符号的原码去掉符号位-*/
int Int_ABS(int value);
uint16 Int16_ABS(uint16 value);
/*
********************************************************************
函数名称:
	CheckDataValid
函数功能:
	检查缓冲区内的数据是否全部是格式1的数据；如果是格式1将数据转换为格式2,
	如果不是的话，对缓冲区内的数据不进行任何处理。
输入:
	buf 			输出缓冲
	char_src		格式1
	char_dst		格式2
	num				缓冲区内数据的长度
返回值:
	1:	数据不是格式1的内容，数据内容为有效值，不需要转换
	0:	数据内容全部是格式1的，已经将数据转换为格式2
内部处理流程:
检查缓冲区内的数据是否为num个char_src,如果不是的话，不改变缓冲区buf内
的数据；如果是的话，将缓冲区内的数据转换为char_det。

一般用来判断缓冲区内是否全部是0xff(无效值)，
如果是的话将数据换为0xee或者0(根据规约要求，并参考用户的要求，将无效数据
置为0xee或者0，一般累计时间值置为0)。
********************************************************************
*/
int CheckDataValid(void * buf, unsigned char CHAR_DST , unsigned char CHAR_SRC, int num);



/*
***************************************************************************
*模块编号：
*名称：参数校验函数
*功能：参数校验
*输入参数：参数存放缓冲区－para；参数长度－len；
*输出参数：校验码缓冲区－verify；
*处理：检查参数的校验和是否正确
*返回：校验结果，1成功，0失败
***************************************************************************
*/
int VerifyPData(char* verify,const char* para, int len);

/*-完成从rtc结构到长整数的转化，2000年1月1日0:0:0开始-*/
uint32 RTC_To_Sec(rtc_t *prtc);

/*-完成从长整数到rtc结构的转化-*/
void Sec_To_Rtc(uint32 time, rtc_t *prtc);

uint16 BcdToHex(uint8 compress_bcd);
uint8 HexToBcd(uint16 hex);
BOOL CheckFF(uint8 *buf, uint16 len);

uint32 CldToMin(rtc_t *ptCld);/*-将日历转换为以2000-1-1-0:0:0 起始的分钟数-*/
uint32 CldToSec(rtc_t *ptCld);/*-完成从rtc结构到秒数的转化，2000年1月1日0:0:0开始-*/

uint16 GetWMax(char bItemNum, uint16 awSrc[]);/*-比较出指定数目的最大值-*/
void CmpValuint16(char bItemNum, uint16 awSrc[],uint16 *MaxVal,uint16 *MinVal);
void CmpValint(char bItemNum, int awSrc[],int *MaxVal,int *MinVal);

void TranHexRealTime(rtc_t *prealTime, struct rtc_hex *prealTime_hex);/*-时间转化成hex码-*/

/*-把float转化为规约格式-*/
uint32 float_change_to_FK_4BYTE(float32 src);
/*-n位的BCD加BCD码-*/
void BCD_add_BCD(uint8 * dec,uint8 * src1,uint8 * src2,uint8 n);
/*--把n位的bcd码转化为long	一个字节两位-*/
uint32  BCD_change_to_long(uint8 *src,uint8 n);
/*8888888888888888888888888888888888888888888888888888888888*/
/*-把long转化为n位的bcd码-*/
void long_change_to_BCD(uint8 *dec,uint8 n,uint32 src);
/*-把long long转化为n位的bcd码-*/
void longlong_change_to_BCD(uint8 *dec,uint8 n,uint64 src);
/*--把n位的bcd码转化为float	一个字节两位-*/
//float32  BCD_change_to_float(uint8 *src,uint8 n);
/*-把浮点数转化为n位的bcd码-*/
//void float_change_to_BCD(uint8 *dec,uint8 n,float32 src);
/*-ONE BYTE BCD INC -*/
void BCD_INC(uint8 *src);



uint8 Bcd2Hex(uint8 BcdNum);

/*-lib-*/
//float32 FK_2UBYTE_to_float(uint16 dec);
//uint16 float_to_FK_2UBYTE(float32 src);
uint16 long_to_FK_2UBYTE(int src);

uint32  long_to_PWG3G2G1UBYTE(int src);
uint16  long_to_PWG3G2G1UBYTE_NEW(int64 src);
/*-比较键值是否相等-*/
uint8 ifthiskey(uint8 keyvalue);

/**************************************************************************
 *  func:将100*科学计数法规约格式的数据转换为标准float格式数据,不放大
 *  in:  uword  2byte (里面的数据含有G3G2G1)
 *  out: uint16  2byte  *
 *
 **********************************************************************/
uint32 FK_2UBYTE_to_ULWORD(uint16 dec);
int FK_2G321_long(uint16 src);         //-将100*科学计数法数据转化为ｌｏｎｇ-
int64 FK_2G321_long_New(uint16 src);         //-将100*科学计数法数据转化为ｌｏｎｇ-
uint16 PwWord_2G321(int src,uint32 ctpt); //-将2字节功率二次值　转化为100*科学计数法数2003-12-16 13:02-
int8 FkFdxs_sChar(uint8 coef);  //-将功率电量等定值浮动系数ＢＣＤ格式转化为signed uint8-
uint8 sChar_FkFdxs(int8 coef);  //-将功率电量等定值浮动系数signed uint8 转化为BCD格式(D7:0/1 上/下浮)-

/***************************************************************
  func:  把负控规约的4字节一次值电量(带G的4字节BCD码)转化为long  不做任何放大-
  in:   带G的4字节BCD码负控规约格式  psrc(kwh)
  out:  int  (kwh)
  note:  G=1 表示的数不能超过0xffffffff!
 **************************************************************/
uint32 FK4Byte_long(uint8 *psrc);

int64 FK4Byte_slong_LONG(uint8 *psrc);

uint64  BCD_change_to_long_long (uint8 *src,uint8 n);

/***************************************************************
  func:  把存放在数据字典中的负控1次值电量值(单位：kwh) 转化为
            负控规约的4字节电量kwh 或mwh(一次值)(带G的BCD码）
            只考虑为正的情况！
  in:    src:存放在数据字典中的1次值long(单位：kwh)(自然日的日电量)
  out:   带G的4字节BCD码负控规约格式 int  (带G的BCD码）
  note:  分辨率1kwh
 **************************************************************/
uint32 slong_FK4Byte(int src);
uint32 slong_FK4Byte_GD(int64 src);
/***************************************************************
  func:  把负控规约的4字节一次值电量(带G的4字节BCD码)转化为long  不作任何放大-
  in:   带G的4字节BCD码负控规约格式  psrc(kwh)
  out:  int  (kwh)
  note:  G=1 表示的数不能超过0xffffffff!
 **************************************************************/
int64 FK4Byte_slong(uint8 *psrc);
/***************************************************************
  func:  把负控规约的4字节费率格式(带G的4字节BCD码，Z，S3)转化为float 不作任何放大-
  in:   带G的4字节BCD码负控规约格式  psrc(厘kwh)(g=1 mwh元)  z=1 清0 s3=1 为负)
  out:  float32  (kwh)
  note:  G=1 表示的数不能超过0xffffffff!
 **************************************************************/
float32 QuanFee4Byte_float(uint8 *psrc);



/*-
*****************************************************************************
Func:
	得到最近点的电量
InPut:
	uint8 *pbuf:	存放着基础冻结点的数据；
	uint8 i:		冻结密度－1；
Return:
	int64 q1:		根据冻结密度不同，返回实际冻结点前的基础冻结点的电量的累加值
Proc Info:
	关于累加办法的说明:

		如果是按照冻结密度1(15分钟一个点)，不需要累加；
		如果是按照冻结密度2(30分钟一个点)，某个时刻Point(i)的电量值=同一时刻(Point(i)的delta值+Point(i+1)的delta值)；
		如果是按照冻结密度3(60分钟一个点)，某个时刻Point(i)的电量值=同一时刻(Point(i)的delta值+Point(i+1)的delta值+Point(i+2)的delta值+Point(i+3)的delta值)；

		如果某个基础冻结点的数据为0xffffffffffffffff(8bytes)，认为该基础冻结点的数据为0；
Declaration:
	在函数的注释中，统一称每15分钟间隔的点称为基础冻结点；根据冻结密度计算的时间点称为实际冻结点
zjf
*****************************************************************************
-*/
int64 GetRecentDotDayQuan(uint8* pbuf,uint8 i);

/***************************************************************
  func:  把存放在数据字典中的1次值电量值float(单位：kwh/厘) 转化为
            负控规约的4字节电量kwh 或mwh(一次值)(带G的BCD码,and s3）
  in:    src:存放在数据字典中的1次值float (单位：kwh)
  out:   带G的4字节BCD码负控电费规约格式  (带G的BCD码,s3=1 为负）
  note:  分辨率1kwh/厘
 **************************************************************/
uint32 float_QuanFee4Byte(float32 src);

#if ANOLOGY_EN == 1
int FK_2UBYTE_TO_LONG(uint16 dec);
#endif

//-将缓冲区的数据转换为ascii-
int Databuf_to_ASCII(unsigned char *buf,int num);

#define HALT() do{ printf("Press the ENTER key to continue !\n"); fgetc( stdin ); }while(0)
/*
This function is just for test to show message or buffer contents !
The usage of this function as follow
msg:		The comments u want to show (prompt)
len:		How many characters in the buffer which u want to show
msg1:	The buffer which u to show(eg:data..etc)
n:	0,you can pause the program from running,when you enter the key ENTER,it will go on;
	n (any figure >0),the program will pause n*1000us and then run again;
*/
void Debug_Show( const char* msg, const char* msg1, int len, int n );


void Assert(char* pFileName, int nLine);
#define ASSERT(x)	if((x)!=TRUE){Assert(__FILE__, __LINE__);}

void PrintHexData(char* pHex, int nHexLen);

void ReverseBuf(uint8 *buf, uint16 len);
uint8 BitSringReverseBuf(uint8 bitVal);

extern void SetResetFlag(uint8 resetFlag, uint8 acClrFlg, uint8 killFlag);
void SetBeingResetFlag(void);
void ResetRealProc(void);

const OOP_OAD_U* getEvtOadInfo(uint16 OI, uint8* oad_num);
int GetEvtSubOadOffset(uint16 EvtOI, OOP_OAD_U oad, uint8 index, uint16 *outoffset, uint16 *outLen);
#if REWRITEPORT == 1
uint16 getEvtRptData(uint8* pEvtData, uint8* rptInfo,uint16* rpt_eveIO);
#else
uint16 getEvtRptData(uint8* pEvtData, uint8* rptInfo);
#endif
void rptOverDeal(uint8* pBuf, uint8 ret);

extern BOOL InitEvtPara(uint8 ClassId,uint16 EvtOI);
extern uint16 ReadEvtOadInfoPara(uint16 EvtOI,const OOP_OAD_U outOadInfo[],uint16 MaxOadNum);
extern BOOL SetEvtValidFlg(uint16 EvtOI,uint8 validFlg);
extern uint8 GetEvtValidFlg(uint16 EvtOI);
extern BOOL SetEvtRptFlg(uint16 EvtOI,uint8 rptFlg);
extern BOOL DelEvtOadInfoPara(uint16 EvtOI,OOP_OAD_U delOadInfo);
extern void ClrEvtData(uint8 ClassId,uint16 EvtOI);
extern void ClrEvtPara(uint8 ClassId,uint16 EvtOI);
extern void SetACPara();

#endif

