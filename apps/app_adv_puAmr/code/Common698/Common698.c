/****************************************************************
* Copyright (c) 2006,�Ͼ����������������޹�˾
*            All rights reserved.
*
* �ļ����ƣ�Common698.c
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
#include  "../ptrl/all.h"

#define SECS_IN_HALFDAY 0x0A8C0L
#define FOURYEARS	(3*365+366)

const unsigned int YrTab[] = { 366, 365, 365, 365 };
const unsigned int MonTabLeap[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

pthread_mutex_t App_GetData_lock;       // ���������ݵĻ�����
                                        // �����ȡ����ʱ����Լ����Ҳ����ͬʱ��ȡ��������ʹ�õ���ͬ��ȫ�ֱ���

pthread_mutex_t CalRemainEn_lock;		// ��������ʣ������Ļ����ź���
									// Ŀǰʣ����������ڹ�Լ�·�ʱ�����
									// �Լ�����ش�������Ҳ�����
									// �ֱ���������ͬ�߳���ߣ����ԼӸ�����

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
�������:
1���ܹ����·��
2���ܹ�����ļ�״̬
	��غ���:
	#include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
		int stat(const char *file_name, struct stat *buf);
3��
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
		����ļ��������Ŀ¼�������򴴽�Ŀ¼���ļ������ڴ����ļ�
	-*/
	if(!commfun_FileExists(filepath))
	{
		commfun_ExtractFilePath(filepath, path);
		if(!commfun_PathExists(path))
			if(mkdir(path,0770)<0)
			{
				perror("����Ŀ¼���ɹ�");
				return ERROR;
			}
		if( (filehandle = open(filepath, O_WRONLY | O_CREAT | O_TRUNC)) < 0) //�½��ļ�
		{
			perror("�����ļ����ɹ�");
			close(filehandle);
			return ERROR;
		}
	}

    //puts("��ʼд��־");
    // �¼���¼����־�ļ�����1����С��ת�浽.sub�ļ���
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
        //-�����ļ���С-
        filehandle = open(filepath,O_RDWR | O_CREAT);
        FileLength = lseek(filehandle,0,SEEK_END);//-���ļ�ָ�붨λ���ļ�β������ֵ���Ե����ļ�����ʹ��-
        if(FileLength > FILE_MAX_NUM)
            ftruncate(filehandle,0);
        close(filehandle);
    }


	//-������־��Ϣ-
	fp = fopen(filepath, "a+");
	if(fp == NULL)
	{
		perror("Error: open the file wrongly!\n");
		return ERROR;
	}
	//- д������ -
	t = time(NULL);
	ptm = localtime(&t);
	sprintf(timebuf, "%4d-%02d-%02d %02d:%02d:%02d  ", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, 	ptm->tm_sec);
	fprintf(fp,"%s",timebuf);

	//- д��������Ϣ -
	fprintf(fp,"%s",string);
	len_str = strlen(string);
	if((string[len_str-1] != 0x0a)&&(string[len_str-1] != 0x0d))
		fprintf(fp,"%s","\n");
	fclose(fp);
	//puts("д��־���");
	return 0;

}

//-��������������ת��Ϊascii-
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
��  ��: WriteLogSecurity
��  ��: ��¼����APDU����鿴��־
��  ��:

��  ��:
		��
����ֵ:
		��
*********************************************************************/
void WriteLogSecurity(BOOL IsSecurityApdu, BOOL recvflag, const char *fileName, const uint8 *dataBuf, const uint16 dataLen)
{
	char debug_msg[dataLen * 3 + 10];
	int i;
	char logbuf[100];
    char* pcz_word_1[] = {"��ȫ�ӽ��ܱ�������", "��ͨ����"};
    char* pcz_word_2[] = {"����", "����"};

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
	-1: �������ݵĻ�������ַ����ȷ
	-2: ���ݻ����������ݵĳ���Ϊ0
�ڲ���������:
��黺�����ڵ������Ƿ�Ϊnum��char_src,������ǵĻ������ı仺����buf��
�����ݣ�����ǵĻ������������ڵ�����ת��Ϊchar_det��

һ�������жϻ��������Ƿ�ȫ����0xff(��Чֵ)��
����ǵĻ������ݻ�Ϊ0xee����0(���ݹ�ԼҪ�󣬲��ο��û���Ҫ�󣬽���Ч����
��Ϊ0xee����0��һ���ۼ�ʱ��ֵ��Ϊ0)��
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
*ģ���ţ�
*���ƣ�����У�麯��
*���ܣ�����У��
*���������������Ż�������para���������ȣ�len��
*���������У���뻺������verify��
*������������У����Ƿ���ȷ
*���أ�У������1�ɹ���0ʧ��
***************************************************************************
*/
int VerifyPData(char* verify,const char* para, int len)
{
	int i;
	short CS_Base=0x423;//-���������-
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

/*-��ɴ�rtc�ṹ����������ת����2000��1��1��0:0:0��ʼ���������-*/
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


/*-��ɴӳ�������rtc�ṹ��ת��-*/
void Sec_To_Rtc(uint32 time, rtc_t *prtc)
{
    const unsigned int MonTab[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    {//-ʱ����-
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

    {//-��������-
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
�������ܵ�Ԫ��ͨ��
������������⻺�����Ƿ�ȫΪFF
���������
�����������
��������ֵ:
            TRUE: ȫΪFF  FALSE:������FF
�ڲ��������̣�
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
�������ܵ�Ԫ�����¼�ģ�����
����������������ת��Ϊ��2000-1-1-0:0:0 ��ʼ�ķ�����
���������*prtc rtc�ṹָ��
�����������
��������ֵ��������ʱ��(min)
�ڲ��������̣�
********************************************************************/
uint32 CldToMin(rtc_t *ptCld)
{
	unsigned long lMin=0;

	lMin = RTC_To_Sec(ptCld)/60;
	return lMin;
}
/*********************************************************************
�������ܵ�Ԫ�����¼�ģ�����
������������ɴ�rtc�ṹ��������ת����2000��1��1��0:0:0��ʼ-
���������*ptrtc  rtc�ṹָ��
�����������
��������ֵ��������ʱ��(sec)
�ڲ��������̣�
********************************************************************/
uint32 CldToSec(rtc_t *ptCld)
{
	return RTC_To_Sec(ptCld);
}


/*********************************************************************
�������ܵ�Ԫ�����¼�ģ�����
����������    �Ƚϳ�ָ����Ŀ�����ֵ
���������    bItemNum-�Ƚϵ���Ŀ
		      awSrc[]-�Ƚϵ�ֵ
���������    ��
��������ֵ��  wMax-���ֵ
�ڲ��������̣�
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
�������ܵ�Ԫ�����¼�ģ�����
����������    �Ƚϻ�����ֵ�Լ���Сֵ
���������    bItemNum-�Ƚϵ���Ŀ
              Max
              Min
		      awSrc[]-�Ƚϵ�ֵ
���������    ��
��������ֵ��  wMax-���ֵ
�ڲ��������̣�
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
�������ܵ�Ԫ�����¼�ģ�����
����������    �Ƚϻ�����ֵ�Լ���Сֵ
���������    bItemNum-�Ƚϵ���Ŀ
              Max
              Min
		      awSrc[]-�Ƚϵ�ֵ
���������    ��
��������ֵ��  wMax-���ֵ
�ڲ��������̣�
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
* �������ƣ�TranHexRealTime
* �������ܵ�Ԫ����Լ���Ͷ�̬��
* ����������ʱ��ת����hex��ģ��
* ����������
*				BCD��ʽ��ʱ�����ָ��:struct calendar *prealTime;
*
* ���������
*			hex��ʽ��ʱ�����ָ��:struct cal_hex *prealTime_hex;
*
* �ڲ��������̣�����BcdToHex()��������ǰʱ��ת��Bcd��ΪHex��
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
**		��nλ��bcd��ת��Ϊlong(32b)	һ���ֽ���λ	**
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
  func:��floatת��Ϊ��Լ��ʽ,�����κηŴ�-
  in: float32 src ��������)
  out: ��G��4�ֽ�BCD��,һ�����ڵ���һ��ֵ��Լ�����ʽ   G=0 kwh;G=1 Mwh-
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

	if (src > (1e10))							//10^10  ��ѧ������
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
	tmp[3] |= flag;      						//Gλ��

	return (*(uint32*)tmp);
}

/*********************************************************************
 * ��������:long_to_FK_2UBYTE
 * ˵��: func:����׼float��ʽ����,���Ŵ�ת��Ϊ100*��ѧ��������Լ��ʽ������
 *  (ת��Ϊ2�ֽڲ����ԼBCD��)
 * ��ڲ���:
 * in:   float32 src
 * ����:
 *  out:  uword  2byte (��������ݺ���G3G2G1)
 * ����: �����
 * ʱ�� : 2004-12-15 11:37
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
 *  out:  uword  2byte (��������ݺ���G3G2G1)
 * ����:
 * ʱ�� :
 * func: ��int������ת��Ϊ��G3G2G1��ʽ�Ĺ�Լ��ʽ����(BCD *����)
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
		return 0x8000;      //-Խ��-

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
 *  out:  uword  2byte (��������ݺ���G3G2G1)
 * ����:
 * ʱ�� :
 * func: ��int������ת��Ϊ��G3G2G1��ʽ�Ĺ�Լ��ʽ����(BCD *����)
*********************************************************************/
uint16  long_to_PWG3G2G1UBYTE_NEW(int64 src)
{
#if 1
	uint32 tmp;				//���յĽ��
	uint16 tmpVal = 0;		//�����������
	uint16 n;				//����ݴ���Ϣ
	uint16 flag=0;			//��ŷ��ű�־
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
	long_change_to_BCD((uint8 *)&tmpVal,4,tmp);		//�����������ֵ�BCD��
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
		return 0x8000;      //-Խ��-

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
 *   ��100*��ѧ����������ת��Ϊ BIN       100A *10^N)
 *  IN:  SRC (100*A) ;  n(��)
 *  out: ulword  չ��������ݣ�ֻ�ܱ�����λ��Ч���֣�
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
 *   ��100*��ѧ����������ת��Ϊ BIN       100A *10^N)
 *  IN:  SRC (100*A) ;  n(��)
 *  out: ulword  չ��������ݣ�ֻ�ܱ�����λ��Ч���֣�
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
 *  func:��100*��ѧ��������Լ��ʽ������ת��Ϊ��׼long,���Ŵ�
 *  in:  uword  2byte (��������ݺ���G3G2G1)
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
 *  func:����Լ��ʽ������ת��Ϊ����(һ�㽫���ʸ�ʽת��w��������Ϊ15kwh /10000-->1.5w
 *          (����ʱ�Ŵ󣱣�,�����棱����)
 *  in:   		2byte (��������ݺ���G3G2G1)
 *  out:		չ��������ݣ�ֻ�ܱ�����λ��Ч���֣�
 *  return :	ERROR(-1)��ʾ����ֵ��ʽ����
 **********************************************************************/
int FK_2G321_long(uint16 src)
{
	int8 n;
	int tmp;

	if((src&0xff)>0x99)
	{
		return -1;       //-����ڣ��ֽڷǣ�����-
	}
	tmp=((src&0xf00)>>8)*100+((src&0xf0)>>4)*10+(src&0xf);  //-3λ����λ-
	n=4-(src>>13);   //-g3g2g1-

	if (src&0x1000)     //-d4:1 s-
	{
		tmp=-tmp;
	}
	return (fkpow(tmp*100,n));
}


/**************************************************************************
 *  func:����Լ��ʽ������ת��Ϊ����(һ�㽫���ʸ�ʽת��w��������Ϊ15kwh /10000-->1.5w
 *          (����ʱ�Ŵ󣱣�,�����棱����)
 *  in:   		2byte (��������ݺ���G3G2G1)
 *  out:		չ��������ݣ�ֻ�ܱ�����λ��Ч���֣�
 *  return :	ERROR(-1)��ʾ����ֵ��ʽ����
 **********************************************************************/
int64 FK_2G321_long_New(uint16 src)
{
	int8 n;
	int tmp;

	if((src&0xff)>0x99)
	{
		return -1;       //-����ڣ��ֽڷǣ�����-
	}
	tmp=((src&0xf00)>>8)*100+((src&0xf0)>>4)*10+(src&0xf);  //-3λ����λ-
	n=4-(src>>13);   //-g3g2g1-

	if (src&0x1000)     //-d4:1 s-
	{
		tmp=-tmp;
	}
	return (fkpow_New(tmp*10000,n));
}


/**************************************************************************
 *  func:�����ʡ������ȣ���ֵ(������Ϊ15kwh /10000-->1.5w,����ʱ�Ŵ�10��,������15��)
 *       ת��Ϊ100*��ѧ��������Լ��ʽ������(��λkw��
 *  in:   ����2��ֵ(unit:*10 wh) ,��·ct��pt
 *  out:  uint16  2byte (��������ݺ���G3G2G1) (һ��ֵ)
 **********************************************************************/
uint16 PwWord_2G321(int src,uint32 ctpt)
{
	float32 fsrc;

	fsrc=(float32)src;

	if (ctpt!=10000)            //- �ж��Ƿ�����·-
	{
		//-ת��Ϊһ��ֵ-
		fsrc=fsrc* (float32)ctpt;
		fsrc=fsrc/100.0;       //-  ---> ��λС��kw-
	}

	return (long_to_PWG3G2G1UBYTE((int)fsrc));

}
/**************************************************************************
 *  func:�����ʵ����ȶ�ֵ����ϵ���£ãĸ�ʽת��Ϊsigned char
 *     ����ϵ���ϸ�Ϊ�����¸�Ϊ����
 *  in:   uint8 coef(����ϵ���£ã�,D7:0�ϸ�,1�¸�)
 *  out:  int8
 **********************************************************************/
int8 FkFdxs_sChar(uint8 coef)
{
	int8 tmp;

	if (coef&0x80)
	{
		//-����ϵ���¸�-
		tmp=(int8)BcdToHex(coef&0x7f);
		tmp=-tmp;   //-�为-
	}
	else
	{
		tmp=(int8)BcdToHex(coef);
	}
	return(tmp);
}
/**************************************************************************
 *  func:�����ʵ����ȶ�ֵ����ϵ��signed char ת��Ϊ �£ãĸ�ʽ(D7:0/1 �ϡ��¸�)
 *  in:  int8 coef(�ϸ�Ϊ�����¸�Ϊ��)
 *  out:   uint8 coef(����ϵ���£ã�,D7:0/1 �ϡ��¸�)
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
  func:  �Ѹ��ع�Լ��4�ֽ�һ��ֵ����(��G��4�ֽ�BCD��)ת��Ϊlong  -
  in:   ��G��4�ֽ�BCD�븺�ع�Լ��ʽ  psrc(kwh)
  out:  int  (kwh)
  note:  G=1 ��ʾ�������ܳ���0xffffffff(4294967295)!
 **************************************************************/
uint32 FK4Byte_long(uint8 *psrc)
{
	uint32 dest;

	if (psrc[3]&0x40)
	{
		dest=BCD_change_to_long(psrc,7);	/*--��nλ��bcd��ת��Ϊlong,һ�ֽ���λ-*/
		if (dest>4294967)
			dest=0xffffffff;
		else
			dest=dest*1000;         //g=1 �Ŵ�1000��-
	}
	else
		dest=BCD_change_to_long(psrc,7);	/*--��nλ��bcd��ת��Ϊlong,һ�ֽ���λ-*/
	return(dest);
}

/***************************************************************
  func:  �Ѵ���������ֵ��е�1��ֵ����ֵ(��λ��kwh) ת��Ϊ
            ���ع�Լ��4�ֽڵ���kwh ��mwh(һ��ֵ)(��G��BCD�룩
  in:    src:����������ֵ��е�1��ֵlong(��λ��kwh) �����Ǹ�ֵ
  out:   ��G��4�ֽ�BCD�븺�ع�Լ��ʽ int  (��G��BCD�룩
  note:  �ֱ���1kwh
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
  func:  �Ѹ��ع�Լ��4�ֽ�һ��ֵ����(��G��4�ֽ�BCD��)ת��Ϊlong  �����κηŴ�-
  in:   ��G��4�ֽ�BCD�븺�ع�Լ��ʽ  psrc(kwh)
  out:  int  (kwh)
  note:  G=1 ��ʾ�������ܳ���0xffffffff(4,294,967,295)
 **************************************************************/
int64 FK4Byte_slong(uint8 *psrc)
{
	int dest;
	uint8 flg=0;

	if (psrc[3]&0x10)	//-s3-
		flg=1;			//-Ϊ��-
	if (psrc[3]&0x40)	//-G-
	{
		dest=BCD_change_to_long(psrc,7);	//--��nλ��bcd��ת��Ϊlong,һ�ֽ���λ-
		if (dest>4294967)
			dest=0xffffffff;
		else
			dest=dest*1000;		//g=1 �Ŵ�1000��-
	}
	else
		dest=BCD_change_to_long(psrc,7);	//--��nλ��bcd��ת��Ϊlong,һ�ֽ���λ-
	if (flg)
		dest=-dest;
	return(dest);
}


int64 FK4Byte_slong_LONG(uint8 *psrc)
{
	int64 dest;
	uint8 flg=0;

	if (psrc[3]&0x10)	//-s3-
		flg=1;			//-Ϊ��-
	if (psrc[3]&0x40)	//-G-
	{

		dest=BCD_change_to_long_long(psrc,7);	//--��nλ��bcd��ת��Ϊlong,һ�ֽ���λ-

		if (dest>9999999)
			dest=0xffffffff;
		else
			dest=dest*1000;		//g=1 �Ŵ�1000��-
	}
	else
		dest=BCD_change_to_long_long(psrc,7);	//--��nλ��bcd��ת��Ϊlong,һ�ֽ���λ-
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
  func:  �Ѹ��ع�Լ��4�ֽڷ��ʸ�ʽ(��G��4�ֽ�BCD�룬Z��S3)ת��Ϊfloat �����κηŴ�-
  in:   ��G��4�ֽ�BCD�븺�ع�Լ��ʽ  psrc(��kwh)(g=1 mwhԪ)  z=1 ��0 s3=1 Ϊ��)
  out:  float32  (kwh)
  note:  G=1 ��ʾ�������ܳ���0xffffffff!
 **************************************************************/
float32 QuanFee4Byte_float(uint8 *psrc)
{
	uint32 dest;
	float32 fdest;

	if (psrc[3]&0x40)         //-�ж�G -
	{
		dest=BCD_change_to_long(psrc,7);	/*--��nλ��bcd��ת��Ϊlong,һ�ֽ���λ-*/
		if (dest>4294967)
			dest=0xffffffff;
		else
			dest=dest*1000;         //g=1 �Ŵ�1000��-
	}
	else
		dest=BCD_change_to_long(psrc,7);	/*--��nλ��bcd��ת��Ϊlong,һ�ֽ���λ-*/
	fdest=(float32)dest;
	if (psrc[3]&0x10)  //-s3=0/1:��ֵ��ֵ-
		fdest=-fdest;
	return(fdest);
}



/***************************************************************
  func:  �Ѵ���������ֵ��е�1��ֵ����ֵfloat(��λ��kwh/��) ת��Ϊ
            ���ع�Լ��4�ֽڵ���kwh ��mwh(һ��ֵ)(��G��BCD��,and s3��
  in:    src:����������ֵ��е�1��ֵfloat (��λ��kwh)
  out:   ��G��4�ֽ�BCD�븺�ص�ѹ�Լ��ʽ  (��G��BCD��,s3=1 Ϊ����
  note:  �ֱ���1kwh/��
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
int64 GetRecentDotDayQuan(uint8* pbuf,uint8 i)
{
	 uint64 q1=0,q2=0;

	 while (i > 0)
	 {
		pbuf=pbuf+8;	//--�����ֵ��е���ÿ��ID ��8�ֽ�Ϊ�洢��λ��ʵ�ʶ���������ۼӻ��������--

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


//����������  buf:�׵�ַ  len:���鳤��
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
* �������ƣ����ֽ�BIT_STRING λ����
* ����������
* ����������
* �������:
* ����ֵ��
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
	else if(EvtOI == 0x3202)  //����������ü�¼
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
			return 102;//�����д��102�����¼��Ƚ����⣬����֮�����д����ĳ���
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
				return 100;//�����д��100�����¼��Ƚ����⣬��Ҫ�����������֮������д����ĳ���
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
//��ʼ���̶���
	FixOadNum = sizeof(EvtStandFixCuptObj)/sizeof(OOP_OAD_U);
	pExFixOad = GetEvtFixCuptObj(EvtOI,&ExFixOadNum);
	pFixOad = (OOP_OAD_U *)malloc(sizeof(OOP_OAD_U)*(FixOadNum+ExFixOadNum));
	memcpy((uint8*)pFixOad,(uint8*)EvtStandFixCuptObj,sizeof(EvtStandFixCuptObj));
	memcpy((uint8*)(pFixOad+FixOadNum),(uint8*)pExFixOad,ExFixOadNum*sizeof(OOP_OAD_U));
	InitEvtOadInfoFile(EvtOI,pFixOad,FixOadNum+ExFixOadNum);
	free(pFixOad);
//��ʼ����������
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
    // �ļ�ע��,��¼������ƫ�Ʊ���ļ�
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
{//�����¼���Ч��ʶ
    char  filename[200] = {'\0'};
    int         fd = -1;

    sprintf(filename, "/mnt/data/E_PARA/%04X",EvtOI);
    // �ļ�ע��,��¼������ƫ�Ʊ���ļ�
    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        return FALSE;
    }
    close(fd);
    WritePDataXram(filename, (char *)&validFlg, 2, 1);
	return TRUE;
}

uint8 GetEvtValidFlg(uint16 EvtOI)
{//�����¼���Ч��ʶ
    char  filename[200] = {'\0'};
    int         fd = -1;
	uint8 validFlg=FALSE;
    sprintf(filename, "/mnt/data/E_PARA/%04X",EvtOI);
    // �ļ�ע��,��¼������ƫ�Ʊ���ļ�
    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        return 0;
    }
    close(fd);
    ReadPDataXram(filename, (char *)&validFlg, 2, 1);
	return validFlg;
}

BOOL SetEvtRptFlg(uint16 EvtOI,uint8 rptFlg)
{//�����¼��ϱ���ʶ
    char  filename[200] = {'\0'};
    int         fd = -1;
    int   ret = -1;

    sprintf(filename, "/mnt/data/E_PARA/%04X",EvtOI);
    // �ļ�ע��,��¼������ƫ�Ʊ���ļ�
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
{//��ȡ�¼��ϱ���ʶ
    char  filename[200] = {'\0'};
    int         fd = -1;
	uint8 rptFlg=FALSE;
    sprintf(filename, "/mnt/data/E_PARA/%04X",EvtOI);
    // �ļ�ע��,��¼������ƫ�Ʊ���ļ�
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
    // �ļ�ע��,��¼������ƫ�Ʊ���ļ�
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
    BOOL ConfigAcEn = TRUE;//Ĭ�����ý��ɲ�����
    uint16 id = 1;
    uint8 i = 0;
    char  acaddr[6] = {0};
    uint8 tsa_acaddr[17] = {0};  //��ַ

    nRet = ReadFromEEPROM((char*)&ac_powertype, FM_AC_POWER_TYPE_ADDR, 1);
    if(nRet > 0 && ac_powertype == AC_LINE_MODE_ERR)
    {
        //ConfigAcEn = FALSE;//��ʾ�ն�û�н���
    }

    //����Ĭ�ϵĲ��������
    if(ConfigAcEn)
    {
        for(i=2; i<=4; i++)
        {
            switch(i)
            {
            case 2://������Ϣ
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
                uint8 ptrlType = 3; // 07��Լ
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

                uint8 ac_port[4] = {0x08,0xF2,0x02,0x01};  //�˿�
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

                uint8 Tariffer = 4; //������
                if(WriteXPara(MTR_PARA_TYPE, Mt_Para_Tariffer_ID, (char *)&Tariffer , id) < 0)	return;

                uint8 UserType = 0; //�û�����
                #if ANHUI_FUN == 1 || JIANG_SU_FUN == 1
                UserType = 106; //�û�����
                #endif
                if(WriteXPara(MTR_PARA_TYPE, Meter_UserType_ID, (char *)&UserType, id) < 0)	return;

                //ת��3761�Ľ�������
                uint8 connType = 0;
                if(AC_LINE_MODE_3P3L == ac_powertype)
                    connType = 2;
                else
                    connType = 3;

                switch(connType)
                {
                case 1://oop����
                    connType = 1;
                    break;
                case 2://oop��������
                    connType = 3;
                    break;
                case 3://oop��������
                    connType = 4;
                    break;
                default:
                    connType = 0;
                    break;
                }
                if(WriteXPara(MTR_PARA_TYPE, Mtr_Cnnt_Style_ID, (char *)&connType, id) < 0)	return;

#if STD_698PRO_PATCH == 1
                uint16 Volt = 2200;  //��ѹ ����-1
                uint16 Curr = 15;    //���� ����-1

		uint8 nType =0;
		nRet = ReadFromEEPROM((char*)&nType, FM_AC_POWER_TYPE_ADDR, 1);
		if (nRet >0 && nType == AC_LINE_MODE_3P4L) // �������߷�����:220V��57.7V
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

            case 3://��չ��Ϣ
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

            case 4://������Ϣ
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


