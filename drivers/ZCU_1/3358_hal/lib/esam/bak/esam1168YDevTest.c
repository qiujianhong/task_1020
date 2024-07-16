#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>//read,write等等
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
//#include "sc1168y.h"

#if 0
#define SPI_IOC_MAGIC			'x'

#define SPI_MSGSIZE(N) \
	((((N)*(sizeof (struct spi_ioc_transfer))) < (1 << _IOC_SIZEBITS)) \
		? ((N)*(sizeof (struct spi_ioc_transfer))) : 0)
#define SPI_IOC_MESSAGE(N) _IOW(SPI_IOC_MAGIC, 0, char[SPI_MSGSIZE(N)])
#endif

#define ESAM_GET_DATA		_IOW('E', 0x01, struct encryption_reg_req)
#define ESAM_SET_DATA		_IOW('E', 0x02, struct encryption_reg_req)

struct encryption_reg_req {
	int buflen;
	char* bufdata;
};


typedef struct
{	
	uint8_t 	CLA;			//命令类别
	uint8_t 	INS;			//命令类型中的指令代码
	uint8_t 	P1;				//完成一个指令代码的参考符号
	uint8_t 	P2;
	uint8_t 	Len1;			//后续DATA长度，不包括LRC1,两字节表示
	uint8_t 	Len2;
	uint8_t 	command[2];		//数据域
	uint8_t  	LRC1;			//对CLA INS P1 P2 Len1 Len2 DATA 数据,每个字节异或，然后再取反
		
}TypeEsamIdCommand;

typedef struct
{	
	uint8_t 	CLA;			//命令类别
	uint8_t 	INS;			//命令类型中的指令代码
	uint8_t 	P1;				//完成一个指令代码的参考符号
	uint8_t 	P2;
	uint8_t 	Len1;			//后续DATA长度，不包括LRC1,两字节表示
	uint8_t 	Len2;
	uint8_t  	LRC1;			//对CLA INS P1 P2 Len1 Len2 DATA 数据,每个字节异或，然后再取反
		
}TypeEsamNormalCommand;


static char  CalEsamLrc(char *buf,int len)
{
	int  i;
	char LRC = 0;
	
	for(i=0;i<len;i++)
	{
		LRC ^= buf[i];
	}
	
	LRC = ~LRC;
	
	return LRC;
}


static void EsamWrite(int fd,char *buf,int len)
{		
	int wlen = len +1;
	char *wbuf = NULL;
	int i;

	//pthread_mutex_lock(&MutexEsam);
	wbuf = new char[wlen];

	memset(wbuf,0,sizeof(wlen));

	wbuf[0] = 0x55;
	memcpy(&wbuf[1],buf,len);

	struct encryption_reg_req encryption_reg_req_st;

	encryption_reg_req_st.buflen = wlen;
	encryption_reg_req_st.bufdata = (char *)wbuf;
	//WriteSysCommLogFun(CommLogTypeEsamW,wbuf,wlen);
	printf("EsamWrite:");
	for(int i=0;i<wlen;i++)
		printf("%02x ",wbuf[i]);

	printf("\n\n");
	ioctl(fd, ESAM_SET_DATA, &encryption_reg_req_st); 
//	ioctl(fd, SPI_IOC_MESSAGE(0), &encryption_reg_req_st); 
//#define debug
#ifdef debug

	if(0x90==buf[0] && 0x00==buf[1]) {
		printf("Esam read:");

		for(i = 0; i < ((buf[2] << 8) + buf[3]); i++) //4:sw1 sw2 len1 len2 1:检验
		{
			printf("%02x ", buf[i]);
		}
		printf("\n\n");
	} else {
		printf("result != 9000\n\n");
	}
#endif

	delete []wbuf;
	//pthread_mutex_unlock(&MutexEsam);
}

static void EsamRead(int fd, char *buf, int len)
{
	int i = 0;
	char lrc = 0;
	int le = 0;

	struct encryption_reg_req encryption_reg_req_st;

	//pthread_mutex_lock(&MutexEsam);

	encryption_reg_req_st.buflen = 128;
	encryption_reg_req_st.bufdata = (char *)buf;
	ioctl(fd, ESAM_GET_DATA, &encryption_reg_req_st);  //首先读出4个字节
//	ioctl(fd, SPI_IOC_MESSAGE(0), &encryption_reg_req_st);  //首先读出4个字节

	printf("Esam read:");
//#define debug
#ifdef debug
	for(i = 0; i < ((buf[2] << 8) + buf[3]); i++) //4:sw1 sw2 len1 len2 1:检验
	{
		printf("%02x ", buf[i]);
	}

	printf("\n\n");
#endif
#if 1
	if(0x90==buf[0] && 0x00==buf[1])
	{
		le = ((int)(buf[2])<<8) + buf[3];
		printf("le=%d\n",le);
		if(le <= (len-5))
		{
			encryption_reg_req_st.buflen = le+4+1;	//4:90 00 le(high) le(low) 1：需要读出检验字节 
			encryption_reg_req_st.bufdata = (char *)buf;
			ioctl(fd, ESAM_GET_DATA, &encryption_reg_req_st);

			for(i = 0; i < (le + 4 + 1); i++) //4:sw1 sw2 len1 len2 1:检验
			{
				printf("%02x ",buf[i]);
			}
			printf("\n\n");
			printf("bufflen %d\n", encryption_reg_req_st.buflen);
			//WriteSysCommLogFun(CommLogTypeEsamR,buf,le+4+1);
			lrc = CalEsamLrc(buf,le+4);	
			if(buf[4+le] != lrc)
			{
				printf("LRC2 != lrc  LRC2=%02x lrc=%02x \n",buf[4+le],lrc);
			}
		}
	}
//#define debug
#ifdef debug
	else if (0x6a == buf[0])
	{
		printf("the CLK is unormal\n");
		for(i = 0; i < ((buf[3]<<8) + buf[4]); i++)
		{
			printf("%02x ",buf[i]);
		}
		printf("\n\n");
	}
#endif
	else {
		printf("result != 9000\n");
		for(i = 0; i < ((buf[3]<<8) + buf[4]); i++)
		{
			printf("%02x ",buf[i]);
		}
		printf("\n\n");
	}
#endif
}



//读esam芯片序列号
static int  ReadIdCommandToEsam(int fd)
{
	TypeEsamNormalCommand Frame;
	memset(&Frame,0,sizeof(Frame));
#if 1
	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0xFF; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;

#else
	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0x02; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;
	
	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0x03; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;

	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0x04; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;

	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0x05; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;

	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0x06; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;

	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0x07; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;

	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0x08; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;

	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0x09; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;

	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0x0A; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;

	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0x0B; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;

	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0x0C; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;
#endif

	Frame.LRC1 = CalEsamLrc((char *)&Frame,sizeof(Frame)-1);			// LRC不算

	EsamWrite(fd,(char *)&Frame,sizeof(Frame));
	
	return 0;
}

//读对称密钥版本
static int  ReadKeyVersionCommandToEsam(int fd)
{
	TypeEsamNormalCommand Frame;
	memset(&Frame,0,sizeof(Frame));
	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0x04; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;

	Frame.LRC1 = CalEsamLrc((char *)&Frame,sizeof(Frame)-1);			// LRC不算

	EsamWrite(fd,(char *)&Frame,sizeof(Frame));

	return 0;
}

//读终端证书cmd
static int  ReadTerminalCertCommandToEsam(int fd)
{

	TypeEsamNormalCommand Frame;
	memset(&Frame,0,sizeof(Frame));
	Frame.CLA	= 0x80; 
	Frame.INS	= 0x36; 
	Frame.P1	= 0x00; 
	Frame.P2	= 0x0B; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x00;

	Frame.LRC1 = CalEsamLrc((char *)&Frame,sizeof(Frame)-1);			// LRC不算
	EsamWrite(fd,(char *)&Frame,sizeof(Frame));

	return 0;
}

static int  InfoSecureReadTerminalCertToFile(int EsamFd)
{
	int i = 0;
	int				SecureLen   = 0;
	char 		RxBuf[2048];
	
	memset(RxBuf,0,sizeof(RxBuf));
	
	ReadTerminalCertCommandToEsam(EsamFd);
	usleep(20*1000);
	EsamRead(EsamFd,RxBuf,1000);
	
	return 0;
}


int main()
{
	int fd = 0;
	char RxBuf[4096];

	fd = open("/dev/encryption1",O_RDWR|O_NOCTTY);
	printf("/dev/encryption1 fd=%d\n",fd);
	if(fd < 0)
	{
		printf("/dev/encryption0 fd=%d\n",fd);
		exit(1);
	}

//	while (1)
//	{
#define NDLY
		ReadIdCommandToEsam(fd);
#ifdef NDLY
		usleep(20*1000);
#endif
		EsamRead(fd, RxBuf, 1024);
#ifdef NDLY
		usleep(20*1000);
#endif

		ReadKeyVersionCommandToEsam(fd);
#ifdef NDLY
		usleep(20*1000);
#endif
		EsamRead(fd, RxBuf, 1024);
#ifdef NDLY
		usleep(20*1000);
#endif

		InfoSecureReadTerminalCertToFile(fd);
#ifdef NDLY
		usleep(20*1000);
#endif
//	}
	close(fd);			
		
	return 0;		
}
