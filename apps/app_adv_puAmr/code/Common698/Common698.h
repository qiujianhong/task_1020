/****************************************************************
* Copyright (c) 2006,�Ͼ����������������޹�˾
*            All rights reserved.
*
* �ļ����ƣ�Common698.h
* �ļ���ʶ��
* �汾�ţ�1.0
*
* ��ǰ�޶��汾��
* �޸ļ�Ҫ˵��:04��Լͨ�ÿ��ļ�
* �޶��ߣ�
* ������ڣ�
*
* ԭʼ�汾��1.0
* ԭ���ߡ���֣����
* ������ڣ�
*******************************************************************/
#ifndef _COMMON698_H
#define _COMMON698_H

#define ALL_EN_FLAG 123456L
#define DEPRINT(en_flag,fmt...) ({\
	if(getenv("DEBUG") != NULL){\
	if((atoi(getenv("DEBUG")) == en_flag)||(atoi(getenv("DEBUG")) == ALL_EN_FLAG))\
		{printf("%s----%d...  ", __FILE__, __LINE__);	printf(fmt);}}\
})

extern pthread_mutex_t App_GetData_lock;        // ���������ݵĻ�����
                                                // �����ȡ����ʱ����Լ����Ҳ����ͬʱ��ȡ��������ʹ�õ���ͬ��ȫ�ֱ���
extern pthread_mutex_t CalRemainEn_lock;		// ��������ʣ������Ļ����ź���

/*
  ��  ��: write_log
  ��  ��: д��־(�����д���ʱ��),����20K�����´��ļ�ͷ��ʼд
  ��  ��:
      filepath - �ļ�·��
      str      - �ַ���
  ��  ��:
    (none)
  ����ֵ:
    < 0 - ����
    = 0 - �ɹ�

 */
int write_log(const char *filepath, const char *str);

// ��¼����APDU����鿴��־
void WriteLogSecurity(BOOL IsSecurityApdu, BOOL recvflag, const char *fileName, const uint8 *dataBuf, const uint16 dataLen);

/*-�Ѵ����ŵ�ԭ��ȥ������λ-*/
int Int_ABS(int value);
uint16 Int16_ABS(uint16 value);
/*
********************************************************************
��������:
	CheckDataValid
��������:
	��黺�����ڵ������Ƿ�ȫ���Ǹ�ʽ1�����ݣ�����Ǹ�ʽ1������ת��Ϊ��ʽ2,
	������ǵĻ����Ի������ڵ����ݲ������κδ���
����:
	buf 			�������
	char_src		��ʽ1
	char_dst		��ʽ2
	num				�����������ݵĳ���
����ֵ:
	1:	���ݲ��Ǹ�ʽ1�����ݣ���������Ϊ��Чֵ������Ҫת��
	0:	��������ȫ���Ǹ�ʽ1�ģ��Ѿ�������ת��Ϊ��ʽ2
�ڲ���������:
��黺�����ڵ������Ƿ�Ϊnum��char_src,������ǵĻ������ı仺����buf��
�����ݣ�����ǵĻ������������ڵ�����ת��Ϊchar_det��

һ�������жϻ��������Ƿ�ȫ����0xff(��Чֵ)��
����ǵĻ������ݻ�Ϊ0xee����0(���ݹ�ԼҪ�󣬲��ο��û���Ҫ�󣬽���Ч����
��Ϊ0xee����0��һ���ۼ�ʱ��ֵ��Ϊ0)��
********************************************************************
*/
int CheckDataValid(void * buf, unsigned char CHAR_DST , unsigned char CHAR_SRC, int num);



/*
***************************************************************************
*ģ���ţ�
*���ƣ�����У�麯��
*���ܣ�����У��
*���������������Ż�������para���������ȣ�len��
*���������У���뻺������verify��
*������������У����Ƿ���ȷ
*���أ�У������1�ɹ���0ʧ��
***************************************************************************
*/
int VerifyPData(char* verify,const char* para, int len);

/*-��ɴ�rtc�ṹ����������ת����2000��1��1��0:0:0��ʼ-*/
uint32 RTC_To_Sec(rtc_t *prtc);

/*-��ɴӳ�������rtc�ṹ��ת��-*/
void Sec_To_Rtc(uint32 time, rtc_t *prtc);

uint16 BcdToHex(uint8 compress_bcd);
uint8 HexToBcd(uint16 hex);
BOOL CheckFF(uint8 *buf, uint16 len);

uint32 CldToMin(rtc_t *ptCld);/*-������ת��Ϊ��2000-1-1-0:0:0 ��ʼ�ķ�����-*/
uint32 CldToSec(rtc_t *ptCld);/*-��ɴ�rtc�ṹ��������ת����2000��1��1��0:0:0��ʼ-*/

uint16 GetWMax(char bItemNum, uint16 awSrc[]);/*-�Ƚϳ�ָ����Ŀ�����ֵ-*/
void CmpValuint16(char bItemNum, uint16 awSrc[],uint16 *MaxVal,uint16 *MinVal);
void CmpValint(char bItemNum, int awSrc[],int *MaxVal,int *MinVal);

void TranHexRealTime(rtc_t *prealTime, struct rtc_hex *prealTime_hex);/*-ʱ��ת����hex��-*/

/*-��floatת��Ϊ��Լ��ʽ-*/
uint32 float_change_to_FK_4BYTE(float32 src);
/*-nλ��BCD��BCD��-*/
void BCD_add_BCD(uint8 * dec,uint8 * src1,uint8 * src2,uint8 n);
/*--��nλ��bcd��ת��Ϊlong	һ���ֽ���λ-*/
uint32  BCD_change_to_long(uint8 *src,uint8 n);
/*8888888888888888888888888888888888888888888888888888888888*/
/*-��longת��Ϊnλ��bcd��-*/
void long_change_to_BCD(uint8 *dec,uint8 n,uint32 src);
/*-��long longת��Ϊnλ��bcd��-*/
void longlong_change_to_BCD(uint8 *dec,uint8 n,uint64 src);
/*--��nλ��bcd��ת��Ϊfloat	һ���ֽ���λ-*/
//float32  BCD_change_to_float(uint8 *src,uint8 n);
/*-�Ѹ�����ת��Ϊnλ��bcd��-*/
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
/*-�Ƚϼ�ֵ�Ƿ����-*/
uint8 ifthiskey(uint8 keyvalue);

/**************************************************************************
 *  func:��100*��ѧ��������Լ��ʽ������ת��Ϊ��׼float��ʽ����,���Ŵ�
 *  in:  uword  2byte (��������ݺ���G3G2G1)
 *  out: uint16  2byte  *
 *
 **********************************************************************/
uint32 FK_2UBYTE_to_ULWORD(uint16 dec);
int FK_2G321_long(uint16 src);         //-��100*��ѧ����������ת��Ϊ�����-
int64 FK_2G321_long_New(uint16 src);         //-��100*��ѧ����������ת��Ϊ�����-
uint16 PwWord_2G321(int src,uint32 ctpt); //-��2�ֽڹ��ʶ���ֵ��ת��Ϊ100*��ѧ��������2003-12-16 13:02-
int8 FkFdxs_sChar(uint8 coef);  //-�����ʵ����ȶ�ֵ����ϵ���£ãĸ�ʽת��Ϊsigned uint8-
uint8 sChar_FkFdxs(int8 coef);  //-�����ʵ����ȶ�ֵ����ϵ��signed uint8 ת��ΪBCD��ʽ(D7:0/1 ��/�¸�)-

/***************************************************************
  func:  �Ѹ��ع�Լ��4�ֽ�һ��ֵ����(��G��4�ֽ�BCD��)ת��Ϊlong  �����κηŴ�-
  in:   ��G��4�ֽ�BCD�븺�ع�Լ��ʽ  psrc(kwh)
  out:  int  (kwh)
  note:  G=1 ��ʾ�������ܳ���0xffffffff!
 **************************************************************/
uint32 FK4Byte_long(uint8 *psrc);

int64 FK4Byte_slong_LONG(uint8 *psrc);

uint64  BCD_change_to_long_long (uint8 *src,uint8 n);

/***************************************************************
  func:  �Ѵ���������ֵ��еĸ���1��ֵ����ֵ(��λ��kwh) ת��Ϊ
            ���ع�Լ��4�ֽڵ���kwh ��mwh(һ��ֵ)(��G��BCD�룩
            ֻ����Ϊ���������
  in:    src:����������ֵ��е�1��ֵlong(��λ��kwh)(��Ȼ�յ��յ���)
  out:   ��G��4�ֽ�BCD�븺�ع�Լ��ʽ int  (��G��BCD�룩
  note:  �ֱ���1kwh
 **************************************************************/
uint32 slong_FK4Byte(int src);
uint32 slong_FK4Byte_GD(int64 src);
/***************************************************************
  func:  �Ѹ��ع�Լ��4�ֽ�һ��ֵ����(��G��4�ֽ�BCD��)ת��Ϊlong  �����κηŴ�-
  in:   ��G��4�ֽ�BCD�븺�ع�Լ��ʽ  psrc(kwh)
  out:  int  (kwh)
  note:  G=1 ��ʾ�������ܳ���0xffffffff!
 **************************************************************/
int64 FK4Byte_slong(uint8 *psrc);
/***************************************************************
  func:  �Ѹ��ع�Լ��4�ֽڷ��ʸ�ʽ(��G��4�ֽ�BCD�룬Z��S3)ת��Ϊfloat �����κηŴ�-
  in:   ��G��4�ֽ�BCD�븺�ع�Լ��ʽ  psrc(��kwh)(g=1 mwhԪ)  z=1 ��0 s3=1 Ϊ��)
  out:  float32  (kwh)
  note:  G=1 ��ʾ�������ܳ���0xffffffff!
 **************************************************************/
float32 QuanFee4Byte_float(uint8 *psrc);



/*-
*****************************************************************************
Func:
	�õ������ĵ���
InPut:
	uint8 *pbuf:	����Ż������������ݣ�
	uint8 i:		�����ܶȣ�1��
Return:
	int64 q1:		���ݶ����ܶȲ�ͬ������ʵ�ʶ����ǰ�Ļ��������ĵ������ۼ�ֵ
Proc Info:
	�����ۼӰ취��˵��:

		����ǰ��ն����ܶ�1(15����һ����)������Ҫ�ۼӣ�
		����ǰ��ն����ܶ�2(30����һ����)��ĳ��ʱ��Point(i)�ĵ���ֵ=ͬһʱ��(Point(i)��deltaֵ+Point(i+1)��deltaֵ)��
		����ǰ��ն����ܶ�3(60����һ����)��ĳ��ʱ��Point(i)�ĵ���ֵ=ͬһʱ��(Point(i)��deltaֵ+Point(i+1)��deltaֵ+Point(i+2)��deltaֵ+Point(i+3)��deltaֵ)��

		���ĳ����������������Ϊ0xffffffffffffffff(8bytes)����Ϊ�û�������������Ϊ0��
Declaration:
	�ں�����ע���У�ͳһ��ÿ15���Ӽ���ĵ��Ϊ��������㣻���ݶ����ܶȼ����ʱ����Ϊʵ�ʶ����
zjf
*****************************************************************************
-*/
int64 GetRecentDotDayQuan(uint8* pbuf,uint8 i);

/***************************************************************
  func:  �Ѵ���������ֵ��е�1��ֵ����ֵfloat(��λ��kwh/��) ת��Ϊ
            ���ع�Լ��4�ֽڵ���kwh ��mwh(һ��ֵ)(��G��BCD��,and s3��
  in:    src:����������ֵ��е�1��ֵfloat (��λ��kwh)
  out:   ��G��4�ֽ�BCD�븺�ص�ѹ�Լ��ʽ  (��G��BCD��,s3=1 Ϊ����
  note:  �ֱ���1kwh/��
 **************************************************************/
uint32 float_QuanFee4Byte(float32 src);

#if ANOLOGY_EN == 1
int FK_2UBYTE_TO_LONG(uint16 dec);
#endif

//-��������������ת��Ϊascii-
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

