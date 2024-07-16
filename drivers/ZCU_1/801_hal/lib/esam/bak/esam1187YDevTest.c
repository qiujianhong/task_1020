#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>//read,write等等
#include <fcntl.h>
#include <string.h>
#include <stdint.h>

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

	//pthread_mutex_lock(&MutexEsam);
	wbuf = new char[wlen];
	
	memset(wbuf,0,sizeof(wlen));
	
	wbuf[0] = 0x55;
	memcpy(&wbuf[1],buf,len);

	struct encryption_reg_req encryption_reg_req_st;

	encryption_reg_req_st.buflen = wlen;
	encryption_reg_req_st.bufdata = (char *)wbuf;
	//WriteSysCommLogFun(CommLogTypeEsamW,wbuf,wlen);
	ioctl(fd, ESAM_SET_DATA, &encryption_reg_req_st);
	
	printf("EsamWrite:");
	for(int i=0;i<wlen;i++)
		printf("%02x ",wbuf[i]);

	printf("\n");

	delete []wbuf;
	//pthread_mutex_unlock(&MutexEsam);
	
}

#if 0
static void EsamRead(int fd,char *buf,int len)
{
	int i = 0;
	char lrc = 0;
	int le = 0;

	struct encryption_reg_req encryption_reg_req_st;

	//pthread_mutex_lock(&MutexEsam);

	encryption_reg_req_st.buflen = 4;
	encryption_reg_req_st.bufdata = (char *)buf;
	ioctl(fd, ESAM_GET_DATA, &encryption_reg_req_st);  //首先读出4个字节

	if(0x90==buf[0] && 0x00==buf[1])
	{
		le = ((int)(buf[2])<<8) + buf[3];
		printf("le=%d\n",le);
		if(le <= (len-5))
		{
			encryption_reg_req_st.buflen = le+4+1;	//4:90 00 le(high) le(low) 1：需要读出检验字节 
			encryption_reg_req_st.bufdata = (char *)buf;
			ioctl(fd, ESAM_GET_DATA, &encryption_reg_req_st);
			for(i=0;i<(le+4+1);i++) //4:sw1 sw2 len1 len2 1:检验
			{
				printf("%02x ",buf[i]);
			}
			printf("\n\n");
			//WriteSysCommLogFun(CommLogTypeEsamR,buf,le+4+1);
			lrc = CalEsamLrc(buf,le+4);	
			if(buf[4+le] != lrc)
			{
				printf("LRC2 != lrc  LRC2=%02x lrc=%02x \n",buf[4+le],lrc);
			}
		}
		
	}
	else
	{
		printf("result != 9000\n");
		for(i=0;i<4;i++)
		{
			printf("%02x ",buf[i]);
		}
		printf("\n\n");
		//WriteSysCommLogFun(CommLogTypeEsamR,buf,4);
	}
	//pthread_mutex_unlock(&MutexEsam);


}
#else
static void EsamRead(int fd, char *buf, int len)
{
	int i = 0;
	char lrc = 0;
	int le = 0;

	struct encryption_reg_req encryption_reg_req_st;

	//pthread_mutex_lock(&MutexEsam);

	encryption_reg_req_st.buflen = 4;
	encryption_reg_req_st.bufdata = (char *)buf;
	ioctl(fd, ESAM_GET_DATA, &encryption_reg_req_st);  //首先读出4个字节

	if(0x90==buf[0] && 0x00==buf[1])
	{
		le = (int)((buf[2]<<8) + buf[3]);
		printf("le=%d\n",le);
		if(le <= (len-5))
		{
			encryption_reg_req_st.buflen = le+4+1;	//4:90 00 le(high) le(low) 1：需要读出检验字节 
			encryption_reg_req_st.bufdata = (char *)buf;

			ioctl(fd, ESAM_GET_DATA, &encryption_reg_req_st);
			for(i=0;i<(le+4+1);i++) //4:sw1 sw2 len1 len2 1:检验
			{
				printf("%02x ",buf[i]);
			}
			printf("\n\n");
			//WriteSysCommLogFun(CommLogTypeEsamR,buf,le+4+1);
			lrc = CalEsamLrc(buf,le+4);	
			if(buf[4+le] != lrc)
			{
				printf("LRC2 != lrc  LRC2=%02x lrc=%02x \n",buf[4+le],lrc);
			}
		}
		
	}
	else
	{
		printf("result != 9000\n");
		for(i=0;i<4;i++)
		{
			printf("%02x ",buf[i]);
		}
		printf("\n\n");
		//WriteSysCommLogFun(CommLogTypeEsamR,buf,4);
	}
	//pthread_mutex_unlock(&MutexEsam);
}
#endif




//读芯片ID cmd
static int  ReadIdCommandToEsam(int fd)
{
	TypeEsamIdCommand Frame;
	memset(&Frame,0,sizeof(Frame));
	Frame.CLA	= 0x00; 
	Frame.INS	= 0xb0; 
	Frame.P1	= 0x99; 
	Frame.P2	= 0x05; 
	Frame.Len1 	= 0x00;
	Frame.Len2 	= 0x02;
	Frame.command[0] = 0x00;
	Frame.command[1] = 0x08;
	
	Frame.LRC1 = CalEsamLrc((char *)&Frame,sizeof(Frame)-1);			// LRC不算
	EsamWrite(fd,(char *)&Frame,sizeof(Frame));

	return 0;
}
//读随机数cmd
static int  ReadRandomCommandToEsam(int fd)
{
	TypeEsamNormalCommand Frame;
	memset(&Frame,0,sizeof(Frame));
	Frame.CLA	= 0x00;
	Frame.INS	= 0x84;
	Frame.P1	= 0x00;
	Frame.P2	= 0x08;
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
	Frame.INS	= 0x30; 
	Frame.P1	= 0x01; 
	Frame.P2	= 0x00; 
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

	EsamRead(EsamFd,RxBuf,1000);
	
	return 0;
}




int main()
{
	int fd = 0;
	char RxBuf[4096];
	char send_fmt[20] = {0x55, 0x00, 0x84, 0x00, 0x08, 0x00, 0x00, 0x73,};

	fd = open("/dev/encryption0",O_RDWR);
	if(fd < 0)
	{
		printf("/dev/encryption0 fd=%d\n",fd);
		exit(1);
	}
	
#if 0
	struct encryption_reg_req encryption_reg_req_st;
	encryption_reg_req_st.buflen = 8;
	encryption_reg_req_st.bufdata = send_fmt;
	//WriteSysCommLogFun(CommLogTypeEsamW,wbuf,wlen);
	printf("EsamWrite:");
	for(int i=0;i<encryption_reg_req_st.buflen;i++)
		printf("%02x ",send_fmt[i]);
	printf("\n");
	ioctl(fd, ESAM_SET_DATA, &encryption_reg_req_st); 

	usleep(10*1000);
	EsamRead(fd, RxBuf, 1024);

	usleep(10*1000);
	ReadRandomCommandToEsam(fd);
	usleep(1000);
	EsamRead(fd, RxBuf, 1024);
	usleep(10*1000);
#endif

#if 0

	ReadIdCommandToEsam(fd);
	usleep(1000);
	EsamRead(fd, RxBuf, 1024);
	usleep(10*1000);
#endif
#if 1
	ReadIdCommandToEsam(fd);
	usleep(100);
	EsamRead(fd, RxBuf, 1024);
	usleep(1000);
		
	ReadRandomCommandToEsam(fd);
	usleep(100);
	EsamRead(fd, RxBuf, 1024);
	usleep(1000);
		
	InfoSecureReadTerminalCertToFile(fd);
	usleep(fd);
	
	
#endif
#if 0
	while(1)
	{
		sleep(5);
	}		
#endif
		
	return 0;		
}





