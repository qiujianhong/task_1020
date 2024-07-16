/****************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
*
* 文件名称：Common698.c
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
#include  "../ptrl/all.h"

#define SECS_IN_HALFDAY 0x0A8C0L
#define FOURYEARS	(3*365+366)

const unsigned int YrTab[] = { 366, 365, 365, 365 };
const unsigned int MonTabLeap[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

pthread_mutex_t App_GetData_lock;       // 建立读数据的互斥锁
                                        // 任务获取数据时，规约解释也可能同时获取数据其中使用到共同的全局变量

pthread_mutex_t CalRemainEn_lock;		// 建立计算剩余电量的互斥信号量
									// 目前剩余电量计算在规约下发时会计算
									// 以及购电控处理函数里也会计算
									// 分别在两个不同线程里边，所以加个互斥

extern uint16 CommBuadConvert_Tabl[];
//extern uint8  CommPtrlConvert_Tabl[]; //{0,DLT645,DLT645_2007,DLTOOP,DLT188};
extern uint8  DispPtrlConvert_Tabl[];

void Assert(char* pFileName, int nLine)
{
	printf("error is %s----%d\n", pFileName, nLine);
}


/*
********************************************************************************************
This function is just for test to show message or buffer contents !
The usage of this function as follow
msg:
	The memsge you want to prompt
len:
	How many characters in the buffer which u want to show
buf:
	The buffer which you to show(eg:data..etc)
n:
	0,you can pause the program from running,when you enter the key ENTER,it will go on;
	N (any figure >0),the program will pause N ms and then run again;
	the value of N is best between with 0 and 500,otherwise it may affect other program's or thread's running!
author:
	zjf
*********************************************************************************************
*/
void Debug_Show( const char* msg, const char* buf, int len, int n )
{
	int i;

	puts( msg );

	if( len < 0 )
		return;
	if( len > 0 )
	{
		for( i = 0; i < len; i++ )
			printf("0x%02x ", buf[i] );
		printf("\n");
	}
	if( n == 0 )
		HALT();
	else
		usleep( n*1000);
}

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
补充意见:
1、能够检测路径
2、能够检测文件状态
	相关函数:
	#include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
		int stat(const char *file_name, struct stat *buf);
3、
 */
int write_log(const char *filepath, const char *string)
{
	FILE *fp = NULL;
	int filehandle;
	long FileLength;
	int len_str;
	time_t t;
	struct tm   *ptm;
	char timebuf[100];
	char path[PATH_MAX_LEN+1];

	/*-
		检测文件存在与否，目录不存在则创建目录，文件不存在创建文件
	-*/
	if(!commfun_FileExists(filepath))
	{
		commfun_ExtractFilePath(filepath, path);
		if(!commfun_PathExists(path))
			if(mkdir(path,0770)<0)
			{
				perror("创建目录不成功");
				return ERROR;
			}
		if( (filehandle = open(filepath, O_WRONLY | O_CREAT | O_TRUNC)) < 0) //新建文件
		{
			perror("创建文件不成功");
			close(filehandle);
			return ERROR;
		}
	}

    //puts("开始写日志");
    // 事件记录的日志文件超过1定大小后转存到.sub文件中
    if ((memcmp(filepath, EVENT_LOG_FILE, sizeof(EVENT_LOG_FILE)) == 0)
        || (memcmp(filepath, EN_REMAIN_APPMAIN_LOG_FILE, sizeof(EN_REMAIN_APPMAIN_LOG_FILE)) == 0))
    {
        char pathsub[64];

        if(commfun_GetFileSize(filepath) > FILE_MAX_NUM)
        {
            memset(pathsub, 0x0, sizeof(pathsub));
            strcat(pathsub, filepath);
            strcat(pathsub, ".sub");
            remove(pathsub);
            rename(filepath, pathsub);
        }
    }
    else if (memcmp(filepath, SECURITY_LOG_FILE, sizeof(SECURITY_LOG_FILE)) == 0)
    {
        char pathsub[64];

        if(commfun_GetFileSize(filepath) > FILE_MAX_NUM*2)
        {
            memset(pathsub, 0x0, sizeof(pathsub));
            strcat(pathsub, filepath);
            strcat(pathsub, ".sub");
            remove(pathsub);
            rename(filepath, pathsub);
        }
    }
    else
    {
        //-限制文件大小-
        filehandle = open(filepath,O_RDWR | O_CREAT);
        FileLength = lseek(filehandle,0,SEEK_END);//-将文件指针定位到文件尾，返回值可以当作文件长度使用-
        if(FileLength > FILE_MAX_NUM)
            ftruncate(filehandle,0);
        close(filehandle);
    }


	//-保存日志信息-
	fp = fopen(filepath, "a+");
	if(fp == NULL)
	{
		perror("Error: open the file wrongly!\n");
		return ERROR;
	}
	//- 写入日期 -
	t = time(NULL);
	ptm = localtime(&t);
	sprintf(timebuf, "%4d-%02d-%02d %02d:%02d:%02d  ", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, 	ptm->tm_sec);
	fprintf(fp,"%s",timebuf);

	//- 写入数据信息 -
	fprintf(fp,"%s",string);
	len_str = strlen(string);
	if((string[len_str-1] != 0x0a)&&(string[len_str-1] != 0x0d))
		fprintf(fp,"%s","\n");
	fclose(fp);
	//puts("写日志完成");
	return 0;

}

//-将缓冲区的数据转换为ascii-
int Databuf_to_ASCII(unsigned char *buf,int num)
{
	int i;
    int max = CCH1_BUFF_SIZE_RCV/3;
	char tmpbuf[CCH1_BUFF_SIZE_RCV]={0};
	char tmp[10] = {0};

	for(i = 0; i < num && i < max; i++)
	{
		sprintf(tmp, "%02x ", buf[i]);
		strcat(tmpbuf, tmp);
	}
	strcpy((char*)buf, tmpbuf);

	return 0;
}

/*********************************************************************
名  称: WriteLogSecurity
功  能: 记录明文APDU方便查看日志
输  入:

输  出:
		无
返回值:
		无
*********************************************************************/
void WriteLogSecurity(BOOL IsSecurityApdu, BOOL recvflag, const char *fileName, const uint8 *dataBuf, const uint16 dataLen)
{
	char debug_msg[dataLen * 3 + 10];
	int i;
	char logbuf[100];
    char* pcz_word_1[] = {"安全加解密报文明文", "普通明文"};
    char* pcz_word_2[] = {"接受", "发送"};

    sprintf(logbuf, "%s%s", (IsSecurityApdu == TRUE)?pcz_word_1[0]:pcz_word_1[1], (recvflag == TRUE)?pcz_word_2[0]:pcz_word_2[1]);
	write_log(fileName, logbuf);

	memset(debug_msg, 0x0, sizeof(debug_msg));

	for (i = 0; i < dataLen; i++)
    {
		sprintf(debug_msg + i * 3, "%02x ", dataBuf[i]);
    }

	write_log(fileName, debug_msg);

	return;
}


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
	-1: 输入数据的缓冲区地址不正确
	-2: 数据缓冲区内数据的长度为0
内部处理流程:
检查缓冲区内的数据是否为num个char_src,如果不是的话，不改变缓冲区buf内
的数据；如果是的话，将缓冲区内的数据转换为char_det。

一般用来判断缓冲区内是否全部是0xff(无效值)，
如果是的话将数据换为0xee或者0(根据规约要求，并参考用户的要求，将无效数据
置为0xee或者0，一般累计时间值置为0)。
********************************************************************
*/
int CheckDataValid(void * buf, unsigned char CHAR_DST , unsigned char CHAR_SRC, int num)
{
	int i,j=0;
	char *p=NULL;

	if(buf == NULL)
		return -1;
	else
		p=buf;
	if(num == 0)
		return -2;
	for(i=0;i<num;i++)
	{
		if(p[i] ^ CHAR_SRC)
			return 1;
		else
			j++;
	}
	if(num == j)
		memset(p,CHAR_DST,num);

	return 0;
}

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
int VerifyPData(char* verify,const char* para, int len)
{
	int i;
	short CS_Base=0x423;//-检验码基数-
	short verify_variable;

	memcpy(&verify_variable,verify,2);

  	for(i=0;i < len; i++)
  	{
    		CS_Base += (short)para[i];
  	}
  	if(verify_variable == CS_Base)
  	{
  		return 1;
  	}
  	else
  	{
    		memcpy(verify,&CS_Base,2);
    		return 0;
  	}
}

/*-完成从rtc结构到长整数的转化，2000年1月1日0:0:0开始至今的秒数-*/
uint32 RTC_To_Sec(rtc_t *prtc)
{
    uint32 time;
    uint16 i;
    const unsigned int MonTab1[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    time = (BcdToHex(prtc->yy) / 4) * FOURYEARS +  (BcdToHex(prtc->yy) % 4) * 365;
    //	days in four-year blocks */			   	    //	days in normal years

    /*	If we're past the leap year in the four-year block or if
     *	we're past February then account for leap day
     */
    if ((BcdToHex(prtc->yy) % 4) != 0 || BcdToHex(prtc->mm) > 2)
		time++;
    //	(time) is the number of days to the beginning of this year
    for (i = 1; i < BcdToHex(prtc->mm); i++)
	{
		time += MonTab1[i - 1];
    }
    //	(time) is the number of days to the beginning of this month

    time += BcdToHex(prtc->dd) - 1;
//    return ((time * 24 + BcdToHex(prtc->hh)) * 60 + BcdToHex(prtc->nn)) * 60 + BcdToHex(prtc->ss) + _70_to_80_bias;
    return ((time * 24 + BcdToHex(prtc->hh)) * 60 + BcdToHex(prtc->nn)) * 60 + BcdToHex(prtc->ss) ;
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
       prtc->hh = HexToBcd(usTmp);

       usTmp = (uint16) (secs - (uint32) usTmp * (uint32) (60 * 60));

       usTmp2 = usTmp / 60;
       prtc->nn = HexToBcd(usTmp2);

       prtc->ss = HexToBcd(usTmp - (usTmp2 * 60));
    }

    {//-周年月日-
	uint32 days = time / (SECS_IN_HALFDAY * 2);
	uint16 i, j;
	uint16 usDays;

	prtc->ww = HexToBcd((2+days)%7);	//- 1980/1/1 is Tuesday-

//	i = (uint16) (days / FOURYEARS) * 4;
	i = (uint16) (days / FOURYEARS) ;

	prtc->yy = HexToBcd((uint16)(i*4));

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
	prtc->yy = HexToBcd(BcdToHex(prtc->yy)+i);

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
	prtc->mm = HexToBcd(j + 1);

	prtc->dd = HexToBcd(usDays + 1);
    }

}

uint16 BcdToHex(uint8 compress_bcd)
{
  uint16 rv;
  rv = ((compress_bcd)>>4)*10+(compress_bcd&0x0f);
  return rv;
}

uint8 HexToBcd(uint16 hex)
{
	uint8 xch;
	uint8 rv=0;
	if(hex>99)
		return 0x99;
	xch = hex%10;
	rv += xch;
	xch = (hex/10)%10;
	xch <<= 4;
	rv += xch;
	return rv;
}

/*********************************************************************
所属功能单元：通用
功能描述：检测缓冲区是否全为FF
输入参数：
输出参数：无
函数返回值:
            TRUE: 全为FF  FALSE:不都是FF
内部处理流程：
********************************************************************/
BOOL CheckFF(uint8 *buf, uint16 len)
{
	uint16 i = 0;

	for (i = 0; i < len; i++)
    {
        if (buf[i] != 0xff)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*********************************************************************
所属功能单元：给事件模块调用
功能描述：将日历转换为以2000-1-1-0:0:0 起始的分钟数
输入参数：*prtc rtc结构指针
输出参数：无
函数返回值：长整数时间(min)
内部处理流程：
********************************************************************/
uint32 CldToMin(rtc_t *ptCld)
{
	unsigned long lMin=0;

	lMin = RTC_To_Sec(ptCld)/60;
	return lMin;
}
/*********************************************************************
所属功能单元：给事件模块调用
功能描述：完成从rtc结构到秒数的转化，2000年1月1日0:0:0开始-
输入参数：*ptrtc  rtc结构指针
输出参数：无
函数返回值：长整数时间(sec)
内部处理流程：
********************************************************************/
uint32 CldToSec(rtc_t *ptCld)
{
	return RTC_To_Sec(ptCld);
}


/*********************************************************************
所属功能单元：给事件模块调用
功能描述：    比较出指定数目的最大值
输入参数：    bItemNum-比较的数目
		      awSrc[]-比较的值
输出参数：    无
函数返回值：  wMax-最大值
内部处理流程：
********************************************************************/

uint16 GetWMax(char bItemNum, uint16 awSrc[])
{
    uint8 i;
    uint16	wMax=0;
    for(i=0; i<bItemNum; i++)
    	wMax = (awSrc[i]>wMax) ? awSrc[i]:wMax;
    return wMax;
}


/*********************************************************************
所属功能单元：给事件模块调用
功能描述：    比较获得最大值以及最小值
输入参数：    bItemNum-比较的数目
              Max
              Min
		      awSrc[]-比较的值
输出参数：    无
函数返回值：  wMax-最大值
内部处理流程：
********************************************************************/
void CmpValuint16(char bItemNum, uint16 awSrc[],uint16 *MaxVal,uint16 *MinVal)
{
    uint8    i;
    uint16 	wMax = awSrc[0];
	uint16 	wMin = awSrc[0];

    for(i = 0; i < bItemNum; i++)
    {
        if(wMax < awSrc[i])
			wMax = awSrc[i];
		if(wMin > awSrc[i])
			wMin = awSrc[i];
    }
    *MaxVal = wMax;
    *MinVal = wMin;

}

/*********************************************************************
所属功能单元：给事件模块调用
功能描述：    比较获得最大值以及最小值
输入参数：    bItemNum-比较的数目
              Max
              Min
		      awSrc[]-比较的值
输出参数：    无
函数返回值：  wMax-最大值
内部处理流程：
********************************************************************/
void CmpValint(char bItemNum, int awSrc[],int *MaxVal,int *MinVal)
{
    uint8    i;
    int 	wMax = awSrc[0];
	int 	wMin = awSrc[0];

    for(i = 0; i < bItemNum; i++)
    {
        if(wMax < awSrc[i])
			wMax = awSrc[i];
		if(wMin > awSrc[i])
			wMin = awSrc[i];
    }
    *MaxVal = wMax;
    *MinVal = wMin;

}



/**********************************************************
* 函数名称：TranHexRealTime
* 所属功能单元：规约解释动态库
* 功能描述：时间转化成hex码模块
* 输入描述：
*				BCD格式的时间变量指针:struct calendar *prealTime;
*
* 输出描述：
*			hex格式的时间变量指针:struct cal_hex *prealTime_hex;
*
* 内部处理流程：调用BcdToHex()函数将当前时间转换Bcd码为Hex码
**********************************************************/
void TranHexRealTime( rtc_t *prealTime, struct rtc_hex *prealTime_hex)
{
	prealTime_hex->ss = BcdToHex(prealTime->ss);
	prealTime_hex->nn = BcdToHex(prealTime->nn);
	prealTime_hex->hh = BcdToHex(prealTime->hh);
	prealTime_hex->dd = BcdToHex(prealTime->dd);
	prealTime_hex->mm = BcdToHex(prealTime->mm);
	prealTime_hex->yy = BcdToHex(prealTime->yy);
}

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
**		把n位的bcd码转化为long(32b)	一个字节两位	**
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

/*---------------------------------------------------
  func:把float转化为规约格式,不做任何放大-
  in: float32 src （浮点数)
  out: 带G的4字节BCD码,一般用于电量一次值规约传输格式   G=0 kwh;G=1 Mwh-
  *************************************************/
uint32 float_change_to_FK_4BYTE(float32 src)
{
	uint8 tmp[4];
	uint8 flag=0;

   	if (src < 0)
   	{
   		src = -src;
   		flag = 0x10;
   	}

	if (src > (1e10))							//10^10  科学计数法
	{
		return 0;
	}

	if (src > 9999999)              			//9e6:9*10^6
	{
		src /= 1000;
		long_change_to_BCD(tmp, 7, (uint32)src);
		tmp[3] |= 0x40;              			//G=1 Mwh
		tmp[3] |= flag;
		return (*(uint32*)tmp);
	}

	long_change_to_BCD(tmp, 7, (uint32)src);
	tmp[3] |= flag;      						//G位或

	return (*(uint32*)tmp);
}

/*********************************************************************
 * 函数名称:long_to_FK_2UBYTE
 * 说明: func:将标准float格式数据,不放大，转换为100*科学计数法规约格式的数据
 *  (转换为2字节部颁规约BCD码)
 * 入口参数:
 * in:   float32 src
 * 出口:
 *  out:  uword  2byte (里面的数据含有G3G2G1)
 * 作者: 李庚清
 * 时间 : 2004-12-15 11:37
*********************************************************************/
uint16  long_to_FK_2UBYTE(int src)
{
	 int8 n;
	 uint8 i;
	 uint32 tmp;
	 uint8 flag=0;

	 if(src<0)
	 {
	 	src=-src;
	 	flag=0x10;
	 }

	 if(src > 9990000 )
	   return 0x8000;

	 if(src>=1000)
	 {
	 	for(i=0,n=0;i<3;i++)
	 	  {
	 	  	if( src < 10000) break;
	 	  	n++;
	 	  	src/=10;
	 	  }
		 n++;
 	     src= src / 10;
	 }
	 else
	 {
	   n=0;
	 }

	 n=4-n;

	tmp=((uint16)n<<5)+(uint16)(src/100)+flag;
	i=(uint16)src%100;

	return ((tmp<<8)+((uint8)(i/10)*16+i%10));

}



/*********************************************************************
 *  in: int src
 *  out:  uword  2byte (里面的数据含有G3G2G1)
 * 作者:
 * 时间 :
 * func: 将int型数据转化为带G3G2G1格式的规约格式数据(BCD *倍率)
*********************************************************************/
uint32  long_to_PWG3G2G1UBYTE(int src)
{
#if long_to_PWG3G2G1UBYTE_test_en
	Debug_Show("the input data : src = ", (char *)&src, 4, 1);
#endif
	int8 n=0;
	uint8 i=0;
	uint32 tmp=0;
	uint8 flag=0;

	if(src == 0)
		return 0;
	if(src<0)
	{
		src=-src;
		flag=0x10;
	}
	if(src > 999000000 )
		return 0x8000;      //-越界-

	if(src>=1000)
	{
		for(i=0,n=0;i<5;i++)
		{
			if( src < 10000)
				break;
			n++;
			src/=10;
		}
		n++;
		src= src / 10;
	}
	else
	{
		n=0;
	}

	n=4-n+2;
	tmp=((uint16)n<<5)+(uint16)(src/100)+flag;
	i=(uint16)src%100;

	return ((tmp<<8)+((uint8)(i/10)*16+i%10));
}


/*********************************************************************
 *  in: int src
 *  out:  uword  2byte (里面的数据含有G3G2G1)
 * 作者:
 * 时间 :
 * func: 将int型数据转化为带G3G2G1格式的规约格式数据(BCD *倍率)
*********************************************************************/
uint16  long_to_PWG3G2G1UBYTE_NEW(int64 src)
{
#if 1
	uint32 tmp;				//最终的结果
	uint16 tmpVal = 0;		//存放整数部分
	uint16 n;				//存放幂次信息
	uint16 flag=0;			//存放符号标志
	if(src<0)
	{
		src=-src;
		flag=0x1000;
	}

    if(src < 10LL)
        return (0x8000 + flag);
    else if(src > 99900000000LL)
        return (0x0999 + flag);

	if(src <= 9999LL)
	{
		n = 7;
		src /= 10;
	}
	else if(src <= 99999LL)
	{
		n = 6;
		src /= 100;
	}
	else if(src <= 999999LL)
	{
		n = 5;
		src /= 1000;
	}
	else if(src <= 9999999LL)
	{
		n = 4;
		src /= 10000;
	}
	else if(src <= 99999999LL)
	{
		n = 3;
		src /= 100000;
	}
	else if(src <= 999999999LL)
	{
		n = 2;
		src /= 1000000;
	}
	else if(src <= 9999999999LL)
	{
		n = 1;
		src /= 10000000;
	}
	else if(src <= 99999999999LL)
	{
		n = 0;
		src /= 100000000;
	}
	tmp = (uint32)src;
	long_change_to_BCD((uint8 *)&tmpVal,4,tmp);		//计算整数部分的BCD码
	//printf("tmp = %ld	n = %d	src = %lld	tmpVal = %04X\n", tmp, n, src, tmpVal);
	tmpVal += ( n * 0x2000  + flag);
	return tmpVal;
#else
	#if 0
	printf("src=%lld\n",src);
	#endif
	int8 n=0;
	uint8 i=0;
	uint32 tmp=0;
	uint8 flag=0;
	if(src == 0)
		return 0;
	if(src<0)
	{
		src=-src;
		flag=0x10;
	}
	src=src/10;
	if(src > 9990000000 )
		return 0x8000;      //-越界-

	if(src>=1000)
	{
		for(i=0,n=0;i<6;i++)
		{
			if( src < 10000)
				break;
			n++;
			src/=10;
		}
		n++;
		src= src / 10;
	}
	else
	{
		n=0;
	}
	n=4-n+3;
	tmp=((uint16)n<<5)+(uint16)(src/100)+flag;
	i=(uint16)src%100;

	return ((tmp<<8)+((uint8)(i/10)*16+i%10));
#endif
}
/*****************************************************
 *   将100*科学计数法数据转化为 BIN       100A *10^N)
 *  IN:  SRC (100*A) ;  n(幂)
 *  out: ulword  展开后的数据（只能保留三位有效数字）
 *****************************************************/
uint32 fkpow(uint32 src,int8 n)
{
  switch(n)
  {
  	case 4:
  	  src=src*10000;
  	  break;
  	case 3:
  	  src=src*1000;
  	  break;
  	case 2:
  	  src=src*100;
  	  break;
  	case 1:
  	  src=src*10;
  	  break;
  	case -1:
  	 src=src/10;
  	  break;
  	case -2:
  	  src=src/100;
  	  break;
  	case -3:
  	  src=src/1000;
  	  break;
  	default:
  	  break;
  }

  return(src);
}

/*****************************************************
 *   将100*科学计数法数据转化为 BIN       100A *10^N)
 *  IN:  SRC (100*A) ;  n(幂)
 *  out: ulword  展开后的数据（只能保留三位有效数字）
 *****************************************************/
int64 fkpow_New(int64 src,int8 n)
{
  switch(n)
  {
  	case 4:
  	  src=src*10000;
  	  break;
  	case 3:
  	  src=src*1000;
  	  break;
  	case 2:
  	  src=src*100;
  	  break;
  	case 1:
  	  src=src*10;
  	  break;
  	case -1:
  	 src=src/10;
  	  break;
  	case -2:
  	  src=src/100;
  	  break;
  	case -3:
  	  src=src/1000;
  	  break;
  	default:
  	  break;
  }
  return(src);
}
/**************************************************************************
 *  func:将100*科学计数法规约格式的数据转换为标准long,不放大
 *  in:  uword  2byte (里面的数据含有G3G2G1)
 *  out: uint32  2byte  *
 *
 **********************************************************************/
uint32 FK_2UBYTE_to_ULWORD(uint16 dec)
{
	int8 n;
	uint32 tmp;

	if((dec&0xff)>0x99) return 0;
	tmp=((dec&0xf00)>>8)*100+((dec&0xf0)>>4)*10+(dec&0xf);
	n=4-(dec>>13);

	return (fkpow(tmp,n));
}

/**************************************************************************
 *  func:将规约格式的数据转化为ｂｉｎ(一般将功率格式转化w）我们认为15kwh /10000-->1.5w
 *          (保存时放大１０,即保存１５ｗ)
 *  in:   		2byte (里面的数据含有G3G2G1)
 *  out:		展开后的数据（只能保留三位有效数字）
 *  return :	ERROR(-1)表示输入值格式错误
 **********************************************************************/
int FK_2G321_long(uint16 src)
{
	int8 n;
	int tmp;

	if((src&0xff)>0x99)
	{
		return -1;       //-如果第１字节非ｂｃｄ码-
	}
	tmp=((src&0xf00)>>8)*100+((src&0xf0)>>4)*10+(src&0xf);  //-3位数据位-
	n=4-(src>>13);   //-g3g2g1-

	if (src&0x1000)     //-d4:1 s-
	{
		tmp=-tmp;
	}
	return (fkpow(tmp*100,n));
}


/**************************************************************************
 *  func:将规约格式的数据转化为ｂｉｎ(一般将功率格式转化w）我们认为15kwh /10000-->1.5w
 *          (保存时放大１０,即保存１５ｗ)
 *  in:   		2byte (里面的数据含有G3G2G1)
 *  out:		展开后的数据（只能保留三位有效数字）
 *  return :	ERROR(-1)表示输入值格式错误
 **********************************************************************/
int64 FK_2G321_long_New(uint16 src)
{
	int8 n;
	int tmp;

	if((src&0xff)>0x99)
	{
		return -1;       //-如果第１字节非ｂｃｄ码-
	}
	tmp=((src&0xf00)>>8)*100+((src&0xf0)>>4)*10+(src&0xf);  //-3位数据位-
	n=4-(src>>13);   //-g3g2g1-

	if (src&0x1000)     //-d4:1 s-
	{
		tmp=-tmp;
	}
	return (fkpow_New(tmp*10000,n));
}


/**************************************************************************
 *  func:将功率、需量等２次值(我们认为15kwh /10000-->1.5w,保存时放大10倍,即保存15ｗ)
 *       转换为100*科学计数法规约格式的数据(单位kw）
 *  in:   功率2次值(unit:*10 wh) ,该路ct×pt
 *  out:  uint16  2byte (里面的数据含有G3G2G1) (一次值)
 **********************************************************************/
uint16 PwWord_2G321(int src,uint32 ctpt)
{
	float32 fsrc;

	fsrc=(float32)src;

	if (ctpt!=10000)            //- 判断是否是总路-
	{
		//-转化为一次值-
		fsrc=fsrc* (float32)ctpt;
		fsrc=fsrc/100.0;       //-  ---> 两位小数kw-
	}

	return (long_to_PWG3G2G1UBYTE((int)fsrc));

}
/**************************************************************************
 *  func:将功率电量等定值浮动系数ＢＣＤ格式转化为signed char
 *     浮动系数上浮为正，下浮为负。
 *  in:   uint8 coef(浮动系数ＢＣＤ,D7:0上浮,1下浮)
 *  out:  int8
 **********************************************************************/
int8 FkFdxs_sChar(uint8 coef)
{
	int8 tmp;

	if (coef&0x80)
	{
		//-浮动系数下浮-
		tmp=(int8)BcdToHex(coef&0x7f);
		tmp=-tmp;   //-变负-
	}
	else
	{
		tmp=(int8)BcdToHex(coef);
	}
	return(tmp);
}
/**************************************************************************
 *  func:将功率电量等定值浮动系数signed char 转化为 ＢＣＤ格式(D7:0/1 上、下浮)
 *  in:  int8 coef(上浮为正，下浮为负)
 *  out:   uint8 coef(浮动系数ＢＣＤ,D7:0/1 上、下浮)
 *  date:   @@03-5-21 8:57
 **********************************************************************/
uint8 sChar_FkFdxs(int8 coef)
{
	uint8 dest;

	if (coef<0)
	{
		coef=-coef;
		dest=(uint8)coef;
		dest=HexToBcd(dest)|0x80;
	}
	else
	{
		dest=(uint8)coef;
		dest=HexToBcd(dest);
	}
	return(dest);
}
/***************************************************************
  func:  把负控规约的4字节一次值电量(带G的4字节BCD码)转化为long  -
  in:   带G的4字节BCD码负控规约格式  psrc(kwh)
  out:  int  (kwh)
  note:  G=1 表示的数不能超过0xffffffff(4294967295)!
 **************************************************************/
uint32 FK4Byte_long(uint8 *psrc)
{
	uint32 dest;

	if (psrc[3]&0x40)
	{
		dest=BCD_change_to_long(psrc,7);	/*--把n位的bcd码转化为long,一字节两位-*/
		if (dest>4294967)
			dest=0xffffffff;
		else
			dest=dest*1000;         //g=1 放大1000倍-
	}
	else
		dest=BCD_change_to_long(psrc,7);	/*--把n位的bcd码转化为long,一字节两位-*/
	return(dest);
}

/***************************************************************
  func:  把存放在数据字典中的1次值电量值(单位：kwh) 转化为
            负控规约的4字节电量kwh 或mwh(一次值)(带G的BCD码）
  in:    src:存放在数据字典中的1次值long(单位：kwh) 可能是负值
  out:   带G的4字节BCD码负控规约格式 int  (带G的BCD码）
  note:  分辨率1kwh
 **************************************************************/
uint32 slong_FK4Byte(int src)
{
  return (float_change_to_FK_4BYTE((float32)src));
}

uint32 slong_FK4Byte_GD(int64 src)
{
  return (float_change_to_FK_4BYTE((float32)src));
}


uint64  BCD_change_to_long_long (uint8 *src,uint8 n)
{
	uint8 i;
	uint64 tmp=0;

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

/***************************************************************
  func:  把负控规约的4字节一次值电量(带G的4字节BCD码)转化为long  不作任何放大-
  in:   带G的4字节BCD码负控规约格式  psrc(kwh)
  out:  int  (kwh)
  note:  G=1 表示的数不能超过0xffffffff(4,294,967,295)
 **************************************************************/
int64 FK4Byte_slong(uint8 *psrc)
{
	int dest;
	uint8 flg=0;

	if (psrc[3]&0x10)	//-s3-
		flg=1;			//-为负-
	if (psrc[3]&0x40)	//-G-
	{
		dest=BCD_change_to_long(psrc,7);	//--把n位的bcd码转化为long,一字节两位-
		if (dest>4294967)
			dest=0xffffffff;
		else
			dest=dest*1000;		//g=1 放大1000倍-
	}
	else
		dest=BCD_change_to_long(psrc,7);	//--把n位的bcd码转化为long,一字节两位-
	if (flg)
		dest=-dest;
	return(dest);
}


int64 FK4Byte_slong_LONG(uint8 *psrc)
{
	int64 dest;
	uint8 flg=0;

	if (psrc[3]&0x10)	//-s3-
		flg=1;			//-为负-
	if (psrc[3]&0x40)	//-G-
	{

		dest=BCD_change_to_long_long(psrc,7);	//--把n位的bcd码转化为long,一字节两位-

		if (dest>9999999)
			dest=0xffffffff;
		else
			dest=dest*1000;		//g=1 放大1000倍-
	}
	else
		dest=BCD_change_to_long_long(psrc,7);	//--把n位的bcd码转化为long,一字节两位-
	if (flg)
		dest=-dest;
	return(dest);
}

#if ANOLOGY_EN == 1
int FK_2UBYTE_TO_LONG(uint16 dec)
{
	int8 n;
	int tmp;

	if((dec&0xff)>0x99) return 0;
	tmp=((dec&0xf00)>>8)*100+((dec&0xf0)>>4)*10+(dec&0xf);
	n=4-(dec>>13);

	tmp=fkpow(tmp,n);
	if(((dec>>12)&1)==1)
	{
		tmp=-tmp;
	}
	return tmp;
}
#endif

/***************************************************************
  func:  把负控规约的4字节费率格式(带G的4字节BCD码，Z，S3)转化为float 不作任何放大-
  in:   带G的4字节BCD码负控规约格式  psrc(厘kwh)(g=1 mwh元)  z=1 清0 s3=1 为负)
  out:  float32  (kwh)
  note:  G=1 表示的数不能超过0xffffffff!
 **************************************************************/
float32 QuanFee4Byte_float(uint8 *psrc)
{
	uint32 dest;
	float32 fdest;

	if (psrc[3]&0x40)         //-判断G -
	{
		dest=BCD_change_to_long(psrc,7);	/*--把n位的bcd码转化为long,一字节两位-*/
		if (dest>4294967)
			dest=0xffffffff;
		else
			dest=dest*1000;         //g=1 放大1000倍-
	}
	else
		dest=BCD_change_to_long(psrc,7);	/*--把n位的bcd码转化为long,一字节两位-*/
	fdest=(float32)dest;
	if (psrc[3]&0x10)  //-s3=0/1:正值或负值-
		fdest=-fdest;
	return(fdest);
}



/***************************************************************
  func:  把存放在数据字典中的1次值电量值float(单位：kwh/厘) 转化为
            负控规约的4字节电量kwh 或mwh(一次值)(带G的BCD码,and s3）
  in:    src:存放在数据字典中的1次值float (单位：kwh)
  out:   带G的4字节BCD码负控电费规约格式  (带G的BCD码,s3=1 为负）
  note:  分辨率1kwh/厘
 **************************************************************/
uint32 float_QuanFee4Byte(float32 src)
{
	uint32 dest;

	if (src < 0)
	{
		dest = float_change_to_FK_4BYTE(-src);
		dest = dest|0x10000000;
		return(dest);
	}
	else
	{
		return (float_change_to_FK_4BYTE(src));
	}
}

/******************************************************************************
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
int64 GetRecentDotDayQuan(uint8* pbuf,uint8 i)
{
	 uint64 q1=0,q2=0;

	 while (i > 0)
	 {
		pbuf=pbuf+8;	//--数据字典中电量每个ID 以8字节为存储单位；实际冻结点往后累加基础冻结点--

		memcpy(&q2,pbuf,8);
		if((q2==APP_NONELLWORD)||(q2==NONE_LLWORD))
			q2=0;
		else
			q1 = q1+q2;
		i--;
	}
	if( i== 0x0)
		return q1;
	else
		return 0;

}

void PrintHexData(char* pHex, int nHexLen)
{
    ASSERT(pHex != NULL && nHexLen > 0);
    char czBuf[4096];
    char* pTmp = czBuf;
    if(nHexLen > 1024)nHexLen=1024;
    int i;
    for(i = 0; i < nHexLen; i ++)
    {
        sprintf(pTmp, "%02X ", (unsigned char)pHex[i]);
        pTmp += 3;
    }
    czBuf[pTmp-czBuf] = '\n';
    czBuf[pTmp-czBuf+1] = '\0';
    printf(czBuf);
}


//将数组逆序  buf:首地址  len:数组长度
void ReverseBuf(uint8 *buf, uint16 len)
{
	uint16  i,j;
	uint8 temp;

	if(len <= 0)
	{
		return;
	}

	for(i=0,j=len-1; i<j; ++i,--j)
	{
		temp = buf[i];
		buf[i] = buf[j];
		buf[j] = temp;
	}
	return;
}


/**********************************************************
* 函数名称：单字节BIT_STRING 位逆序
* 功能描述：
* 输入描述：
* 输出参数:
* 返回值：
**********************************************************/
uint8 BitSringReverseBuf(uint8 bitVal)
{
    uint8 tmpVal = 0;
    uint8 i = 0;

    for (i = 0; i < 8; i++)
    {
        tmpVal |= (((bitVal>>(7-i))&0x01)<<i);
    }

    return tmpVal;
}

const OOP_OAD_U* GetEvtCfgCuptObj(uint16 EvtOI,uint8 *OadNum)
{
	
	const OOP_OAD_U *pOad=NULL;
/*	uint8 oadNum;
	switch(EvtOI)
	{
		case 0x3101:
			pOad = Evt3101CfgCuptObj;
			oadNum = sizeof(Evt3101CfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		case 0x3104:
			pOad = Evt3104CfgCuptObj;
			oadNum = sizeof(Evt3104CfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		case 0x3109:
			pOad = Evt3109CfgCuptObj;
			oadNum = sizeof(Evt3109CfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		case 0x310B:
			pOad = Evt310BCfgCuptObj;
			oadNum = sizeof(Evt310BCfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		case 0x310C:
			pOad = Evt310CCfgCuptObj;
			oadNum = sizeof(Evt310CCfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		case 0x310D:
			pOad = Evt310DCfgCuptObj;
			oadNum = sizeof(Evt310DCfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		case 0x310E:
			pOad = Evt310ECfgCuptObj;
			oadNum = sizeof(Evt310ECfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		case 0x310F:
			pOad = Evt310FCfgCuptObj;
			oadNum = sizeof(Evt310FCfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		case 0x3110:
			pOad = Evt3110CfgCuptObj;
			oadNum = sizeof(Evt3110CfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		case 0x3113:
			pOad = Evt3113CfgCuptObj;
			oadNum = sizeof(Evt3113CfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		case 0x3114:
			pOad = Evt3114CfgCuptObj;
			oadNum = sizeof(Evt3114CfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		case 0x3115:
			pOad = Evt3115CfgCuptObj;
			oadNum = sizeof(Evt3115CfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		case 0x3119:
			pOad = Evt3119CfgCuptObj;
			oadNum = sizeof(Evt3119CfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		case 0x3202:
			pOad = Evt3202CfgCuptObj;
			oadNum = sizeof(Evt3202CfgCuptObj)/sizeof(OOP_OAD_U);
			break;
		default:
			oadNum = 0;
			break;
	}
	if(OadNum!=NULL)
		*OadNum = oadNum;*/
	return pOad;
}

uint8 GetEvtCuptObjLen(uint16 EvtOI,OOP_OAD_U subOad)
{
    return 0;
    /*
	if(subOad.OI == 0x2024)
	{
		if((EvtOI == 0x3105) || (EvtOI == 0x310B) || (EvtOI == 0x310C) || (EvtOI == 0x310D)
			|| (EvtOI == 0x310E) || (EvtOI == 0x310F) || (EvtOI == 0x3113) || (EvtOI == 0x311C) )
		{
			return 9;
		}
		else if(EvtOI == 0x3115 || EvtOI == 0x3030)
		{
			return 5;
		}
		else if((EvtOI == 0x3119) || (EvtOI == 0x3106) || (EvtOI == 0x310A))
		{
			return 2;
		}
		else if((EvtOI == 0x3200) || (EvtOI == 0x3201) || (EvtOI == 0x3202) || (EvtOI == 0x3203))
		{
			return 3;
		}
		else
		{
			return 1;
		}
	}
	else if (subOad.OI == 0x810e)
	{
	    return 3;
	}
	else if (subOad.OI == 0x810F)
	{
	    return 3;
	}
	else if (subOad.OI == 0x8110)
	{
	    return 9;
	}

	if(EvtOI == 0x3101)
	{
		if(subOad.OI == 0x4300)
		{
			return 6;//
		}
	}
	else if((EvtOI == 0x3104))
	{
		if(subOad.OI == 0xF203)
		{
			return 6;//
		}
	}
	else if(EvtOI == 0x3105)
	{
		if(subOad.OI == 0x3313)
		{
			return 8;//
		}
	}
	else if(EvtOI == 0x3030)
	{
		if(subOad.OI == 0x3312)
		{
			if(subOad.AttributeIndex==6)
			    return 8;
			else
			    return 255;
		}
	}
	else if(EvtOI == 0x3109)
	{
		if(subOad.OI == 0x4401)
		{
			return 10;//
		}
	}
	else if((EvtOI == 0x310F) || (EvtOI == 0x3113))
	{
		if(subOad.OI == 0x6041)
		{
			return 8;//
		}
	}
	else if(EvtOI == 0x3110)
	{
		if(subOad.OI == 0x2200)
		{
			return 5;//
		}
	}
	else if(EvtOI == 0x3111)
	{
		if(subOad.OI == 0x3303)
		{
			return 255;//
		}
	}
	else if(EvtOI == 0x3112)
	{
		if(subOad.OI == 0x3304)
		{
			return 30;//
		}
	}
	else if(EvtOI == 0x3202)  //购电参数设置记录
	{
		if(subOad.OI == 0x810C)
		{
			return 43;//
		}
	}
	else if(EvtOI == 0x3118)
	{
	    if(subOad.OI == 0x3302)
	    {
			return 102;//先随便写个102，该事件比较特殊，设置之后才填写具体的长度
	    }
	}
	else if(EvtOI == 0x311C)
	{
	    if(subOad.OI == 0x330F)
		{
			if(subOad.AttributeIndex==6)
		    {
				return 6;
			}
			else
		    {
				return 100;//先随便写个100，该事件比较特殊，需要在设置任务号之后再填写具体的长度
		    }
	    }
	}
#if SECURITY_DEFEND == 1
	else if(EvtOI == 0x3140)
	{
		if(subOad.OI == 0x3141)
		{
			switch(subOad.AttributeIndex)
			{
				case 4:
					return 4;
				case 5:
					return 3;
				case 6:
					return 27;
				case 7:
					return 3;
				case 8:
					return 27;
				case 9:
					return 3;
				case 10:
					return 3;
				case 11:
					return 52;
				case 12:
					return 3;
				case 13:
					return 3;
				case 14:
					return 3;
			}
		}
	}
#endif
	#if HENAN_FUN == 1
	else if(EvtOI == 0x3411)
	{
		if(subOad.OI == 0x3412)
		{
			return 255;//
		}
	}
	#endif
	uint16 index = 0;
	const OadInfoList_t  *pOadInfo = OopObjectList;
	while(index<METER_OAD_ID_NUM)
	{//
		if((subOad.OI == pOadInfo[index].oad.OI) && ((subOad.AttrId) == pOadInfo[index].oad.AttrId)
			&& (subOad.AttributeIndex == pOadInfo[index].oad.AttributeIndex))
		{
			break;
		}
		index++;
	}
	if(index < METER_OAD_ID_NUM)
	{
		return pOadInfo[index].dataSumLen;
	}
	else
	{
		return 100;
	}*/
}
BOOL InitEvtPara(uint8 ClassId,uint16 EvtOI)
{/*
	const OOP_OAD_U *pExFixOad,*pCfgOad;
	OOP_OAD_U *pFixOad;
	uint8 ExFixOadNum,FixOadNum,CfgOadNum;
	ClrEvtPara(ClassId,EvtOI);
//初始化固定列
	FixOadNum = sizeof(EvtStandFixCuptObj)/sizeof(OOP_OAD_U);
	pExFixOad = GetEvtFixCuptObj(EvtOI,&ExFixOadNum);
	pFixOad = (OOP_OAD_U *)malloc(sizeof(OOP_OAD_U)*(FixOadNum+ExFixOadNum));
	memcpy((uint8*)pFixOad,(uint8*)EvtStandFixCuptObj,sizeof(EvtStandFixCuptObj));
	memcpy((uint8*)(pFixOad+FixOadNum),(uint8*)pExFixOad,ExFixOadNum*sizeof(OOP_OAD_U));
	InitEvtOadInfoFile(EvtOI,pFixOad,FixOadNum+ExFixOadNum);
	free(pFixOad);
//初始化可配置列
	pCfgOad = GetEvtCfgCuptObj(EvtOI,&CfgOadNum);
	if(pCfgOad!=NULL && CfgOadNum!=0)
	{
		GenEvtOadInfoPara(EvtOI,pCfgOad,CfgOadNum);
	}
*/	return TRUE;
}

uint16 ReadEvtOadInfoPara(uint16 EvtOI,const OOP_OAD_U outOadInfo[],uint16 MaxOadNum)
{
    uint8 buff[200] = {0};
    uint8 TotalOadNum, FixOadNum,CfgOadNum = 0;
    char  filename[200] = {'\0'};
    uint16		i = 0;
    int         fd = -1;
    sprintf(filename, "/mnt/data/E_PARA/%04X",EvtOI);
    // 文件注册,记录测量点偏移表的文件
    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        return 0;
    }
    close(fd);
    ReadPDataXram(filename, (char *)buff, 0, 2);
    TotalOadNum = buff[0];
    FixOadNum = buff[1];
	if(TotalOadNum<FixOadNum)
	{
		TotalOadNum = FixOadNum;
	}
	CfgOadNum = TotalOadNum - FixOadNum;
    ReadPDataXram(filename, (char *)&buff, 4+FixOadNum*5, CfgOadNum*5);
	if(CfgOadNum>MaxOadNum)
		CfgOadNum = MaxOadNum;
    for(i = 0; i < CfgOadNum; i++)
    {
    	memcpy((uint8*)&outOadInfo[i],&buff[i*5],  4);
    }
    return CfgOadNum;
}

BOOL SetEvtValidFlg(uint16 EvtOI,uint8 validFlg)
{//设置事件有效标识
    char  filename[200] = {'\0'};
    int         fd = -1;

    sprintf(filename, "/mnt/data/E_PARA/%04X",EvtOI);
    // 文件注册,记录测量点偏移表的文件
    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        return FALSE;
    }
    close(fd);
    WritePDataXram(filename, (char *)&validFlg, 2, 1);
	return TRUE;
}

uint8 GetEvtValidFlg(uint16 EvtOI)
{//设置事件有效标识
    char  filename[200] = {'\0'};
    int         fd = -1;
	uint8 validFlg=FALSE;
    sprintf(filename, "/mnt/data/E_PARA/%04X",EvtOI);
    // 文件注册,记录测量点偏移表的文件
    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        return 0;
    }
    close(fd);
    ReadPDataXram(filename, (char *)&validFlg, 2, 1);
	return validFlg;
}

BOOL SetEvtRptFlg(uint16 EvtOI,uint8 rptFlg)
{//设置事件上报标识
    char  filename[200] = {'\0'};
    int         fd = -1;
    int   ret = -1;

    sprintf(filename, "/mnt/data/E_PARA/%04X",EvtOI);
    // 文件注册,记录测量点偏移表的文件
    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        return FALSE;
    }
    close(fd);
    ret = WritePDataXram(filename, (char *)&rptFlg, 3, 1);
    if(ret < 0)
        return FALSE;
    else
    return TRUE;
}

uint8 GetEvtRptFlg(uint16 EvtOI)
{//获取事件上报标识
    char  filename[200] = {'\0'};
    int         fd = -1;
	uint8 rptFlg=FALSE;
    sprintf(filename, "/mnt/data/E_PARA/%04X",EvtOI);
    // 文件注册,记录测量点偏移表的文件
    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        return 0;
    }
    close(fd);
    ReadPDataXram(filename, (char *)&rptFlg, 3, 1);
	return rptFlg;
}

BOOL DelEvtOadInfoPara(uint16 EvtOI,OOP_OAD_U delOadInfo)
{
    uint8 buff[200] = {0};
    uint8 TotalOadNum, FixOadNum,CfgOadNum = 0;
    char  filename[200] = {'\0'};
    uint16		k = 0;
    int         fd = -1;

    sprintf(filename, "/mnt/data/E_PARA/%04X",EvtOI);
    // 文件注册,记录测量点偏移表的文件
    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        return FALSE;
    }
    close(fd);
    ReadPDataXram(filename, (char *)buff, 0, 2);
    TotalOadNum = buff[0];
    FixOadNum = buff[1];
	if(TotalOadNum<FixOadNum)
	{
		TotalOadNum = FixOadNum;
	}
	CfgOadNum = TotalOadNum - FixOadNum;
    ReadPDataXram(filename, (char *)&buff, 4+FixOadNum*5, CfgOadNum*5);


	for(k=0;k<CfgOadNum;k++)
	{
		if(memcmp((uint8*)&delOadInfo,&buff[k*5],4)==0)
			break;
	}
	if(k>=CfgOadNum)
    {
    	return TRUE;
	}
	CfgOadNum--;
	memcpy(buff+k*5,buff+(k+1)*5,(CfgOadNum-k)*5);
    TotalOadNum = FixOadNum + CfgOadNum;
    WritePDataXram(filename, (char *)&TotalOadNum, 0, 1);
    WritePDataXram(filename, (char *)buff, 4+(uint16)FixOadNum*5, (uint16)CfgOadNum*5);
    return TRUE;
}

void ClrEvtData(uint8 ClassId,uint16 EvtOI)
{
	uint16 i;
	char   pathName[256];
	if(ClassId == 7)
	{
		memset(pathName, '\0', sizeof(pathName));
		sprintf(pathName, "rm -rf /mnt/data/E_DATA/%04X/",EvtOI);
		system(pathName);
	}
	else
	{
		for(i=0;i<4;i++)
		{
			EvtOI = (EvtOI & 0x0FFF) + (i<<12);
			memset(pathName, '\0', sizeof(pathName));
			sprintf(pathName, "rm -rf /mnt/data/E_DATA/%04X/",EvtOI);
			system(pathName);
		}
	}
}

void ClrEvtPara(uint8 ClassId,uint16 EvtOI)
{
	char   pathName[256];
	memset(pathName, '\0', sizeof(pathName));
	sprintf(pathName, "rm -rf /mnt/data/E_PARA/%04X/",EvtOI);
	system(pathName);
	ClrEvtData(ClassId,EvtOI);
}

void SetACPara()
{/*
    uint8 ac_powertype = AC_LINE_MODE_ERR;
    int nRet;
    BOOL ConfigAcEn = TRUE;//默认配置交采测量点
    uint16 id = 1;
    uint8 i = 0;
    char  acaddr[6] = {0};
    uint8 tsa_acaddr[17] = {0};  //地址

    nRet = ReadFromEEPROM((char*)&ac_powertype, FM_AC_POWER_TYPE_ADDR, 1);
    if(nRet > 0 && ac_powertype == AC_LINE_MODE_ERR)
    {
        //ConfigAcEn = FALSE;//表示终端没有交采
    }

    //设置默认的测量点参数
    if(ConfigAcEn)
    {
        for(i=2; i<=4; i++)
        {
            switch(i)
            {
            case 2://基本信息
            {
                if(WriteXPara(MTR_PARA_TYPE, Mt_Turn_Num_ID, (char *)&id, id) < 0)
                    return;

#if ACADDR_FROM_PRTL == 1
                if(commfun_ReadACCommAddr(acaddr) < 0)
                    memset(acaddr,0x0,sizeof(acaddr));
#else
				memset(acaddr,0x0,sizeof(acaddr));
#endif

                tsa_acaddr[0] = 05;
                memcpy(&tsa_acaddr[1],acaddr,6);
                if(WriteXPara(MTR_PARA_TYPE, Meter_Address, (char *)tsa_acaddr, id) < 0)
                {
                    return;
                }
                WriteXPara(MTR_PARA_TYPE, Mt_Para_Addr_ID,acaddr, id);

                uint8 buad = 3; // 2400
                uint8 commSpeed = CommBuadConvert_Tabl[buad];
                if(WriteXPara(MTR_PARA_TYPE, Mt_Para_Speed_ID, (char *)&commSpeed, id) < 0)
                {
                    return;
                }
                uint8 ptrlType = 3; // 07规约
                uint8 prtlDisp = 0;
                prtlDisp = DispPtrlConvert_Tabl[ptrlType];
                ptrlType = CommPtrlConvert_Tabl[ptrlType];

                if(WriteXPara(MTR_PARA_TYPE, Mt_Belong_Point_ID, (char *)&prtlDisp, id) < 0)
                {
                    return;
                }
                if(WriteXPara(MTR_PARA_TYPE, Mt_Para_Prtcl_ID, (char *)&ptrlType, id) < 0)
                {
                    return;
                }

                uint8 ac_port[4] = {0x08,0xF2,0x02,0x01};  //端口
                if(WriteXPara(MTR_PARA_TYPE, Meter_Port_ID, (char *)ac_port, id) < 0)
                {
                    return;
                }

                uint8 tmp1,tmp2;
                tmp1 = 0x05;
                tmp2 = 0x01;
                if(WriteXPara(MTR_PARA_TYPE, Mt_Para_Charct_ID, (char *)&tmp1, id) < 0) return;
                if(WriteXPara(MTR_PARA_TYPE, Mt_Para_Com_ID, (char *)&tmp2, id) < 0)	return;

                uint8 pswLen = 6;
                uint8 psw[6] = {0};
                if(WriteXPara(MTR_PARA_TYPE, Mt_Para_Pass_LENGTH, (char *)&pswLen, id) < 0)	return;
                if(WriteXPara(MTR_PARA_TYPE, Mt_Para_Pass_ID, (char *)psw, id) < 0)	return;

                uint8 Tariffer = 4; //费率数
                if(WriteXPara(MTR_PARA_TYPE, Mt_Para_Tariffer_ID, (char *)&Tariffer , id) < 0)	return;

                uint8 UserType = 0; //用户类型
                #if ANHUI_FUN == 1 || JIANG_SU_FUN == 1
                UserType = 106; //用户类型
                #endif
                if(WriteXPara(MTR_PARA_TYPE, Meter_UserType_ID, (char *)&UserType, id) < 0)	return;

                //转成3761的接线类型
                uint8 connType = 0;
                if(AC_LINE_MODE_3P3L == ac_powertype)
                    connType = 2;
                else
                    connType = 3;

                switch(connType)
                {
                case 1://oop单相
                    connType = 1;
                    break;
                case 2://oop三相三线
                    connType = 3;
                    break;
                case 3://oop三相四线
                    connType = 4;
                    break;
                default:
                    connType = 0;
                    break;
                }
                if(WriteXPara(MTR_PARA_TYPE, Mtr_Cnnt_Style_ID, (char *)&connType, id) < 0)	return;

#if STD_698PRO_PATCH == 1
                uint16 Volt = 2200;  //电压 换算-1
                uint16 Curr = 15;    //电流 换算-1

		uint8 nType =0;
		nRet = ReadFromEEPROM((char*)&nType, FM_AC_POWER_TYPE_ADDR, 1);
		if (nRet >0 && nType == AC_LINE_MODE_3P4L) // 三相四线分两种:220V或57.7V
		{
			uint8 nConfig = 0;
			nRet = ReadFromEEPROM((char*)&nConfig, FM_AC_Config_577_ADDR, 1);
			if (nRet > 0 && nConfig == AC_VOLT_TYPE_577)
			{
				Volt=577;
			}
			else
			{
				Volt =2200;
			}
		}
		else if (nRet >0 && nType == AC_LINE_MODE_3P3L)
		{
			Volt =1000;
		}
		else
		{
			Volt =2200;
		}

		printf("Volt = %d,Curr = %d\n",Volt,Curr);
                if(WriteXPara(MTR_PARA_TYPE, Rating_Volt_ID, (char *)&Volt, id) < 0)	return;
                if(WriteXPara(MTR_PARA_TYPE, Rating_Curr_ID, (char *)&Curr, id) < 0)	return;
#endif
                uint64 esamNo=0xFFFFFFFFFFFFFFFFLL;
                WriteMtrXData(id, RT_DATA, MtrEsamSerialNo_ID, (char *)&esamNo);
                WriteMtrXData(id, RT_DATA, MtrSecModEn_ID, (char *)&esamNo);
            }
            break;

            case 3://扩展信息
            {
                uint8 Acq[17] = {0x05,0x00,0x00,0x00,0x00,0x00,0x00};
                if(WriteXPara(MTR_PARA_TYPE, Acq_Address_ID, (char *)Acq, id) < 0)	return;

                uint8 assetLen = 6;
                uint8 asset[6] = {0};
                if(WriteXPara(MTR_PARA_TYPE, Asset_No_Length, (char *)&assetLen, id) < 0)
                    return;
                if(WriteXPara(MTR_PARA_TYPE, Asset_No_ID, (char *)asset, id) < 0)
                    return;

                uint32 ctpt = 0x00010001;
                if(WriteXPara(MTR_PARA_TYPE, MT_CT_PT_ID, (char *)&ctpt, id) < 0)
                    return;
            }
            break;

            case 4://附属信息
            {
            }
            break;
            }
        }
    }

    uint8 valid = 1;
    if(WriteXPara(MTR_PARA_TYPE, Mt_Para_State_ID, (char *)&valid,id) < 0)
        return;

  */  return;
}


