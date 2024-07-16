/*
****************************************************************************
* Copyright (c) 2009,南京新联电子股份有限公司
* All rights reserved.
* 程序名称：commdrv.c
* 版 本 号：1.0
* 功    能：系统公共函数库
新增一个C文件，本文件主要包含公共设备操作函数，比如串口、网络等等
* 原始开发人及时间：
*           gujun    2013-07-04
* 修改人员、时间及修改内容：
* 其他： 
* 注意：本动态库仅仅实现一些公共函数，不涉及逻辑处理，所以不允许出现全部变量、静态变量 
****************************************************************************
*/

#ifndef _DLL_IMPLICIT_
#define _DLL_IMPLICIT_
#endif

#include <net/if.h>
#include <net/if_arp.h>
#include <netdb.h>
extern int h_errno;

#include "CcoCommdrv.h"

#if 0
/*********************************************************************
所属功能单元：232动态库
功能描述：锁串口
输入参数：
-		fd 串口文件句柄
-		type 锁类型 F_RDLCK 共享锁；F_WRLCK 互斥锁
输出参数：
-		无 
函数返回值：

内部处理流程：
********************************************************************/
int32 commdrv_Set_lock(int32 fd, int32 type)
{
	struct flock lock;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 1;
	lock.l_type = type;

	if (fcntl(fd, F_SETLK, &lock) == 0)
		return 1;
	else
		return -1;
}


/*通讯本地参数*/
/*********************************************************************
所属功能单元：232动态库
功能描述：设置串口波特率
输入参数：
-		fd		打开串口的文件句柄
-		speed	串口波特率
输出参数：
-		无
函数返回值：
-		无 
内部处理流程：先读取termios结构，然后调用cfsetispeed cfsetospeed，
设置串口的输入、输出波特率。
********************************************************************/
int32 commdrv_Set_speed(int32 fd, int32 speed)
{
	int32 i; 
	int32 status; 
	int32 speed_arr[] = { B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300 };
	int32 name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200,  300};
	struct termios Opt; 	/*Opt中存储波特率数据结构*/

	tcgetattr(fd, &Opt); 	/*获取termios结构*/

	for ( i = 0;  i < sizeof(speed_arr) / sizeof(int32);  i++) 
	{ 
		if  (speed == name_arr[i])									/*查找支持的波特率*/
		{     
			tcflush(fd, TCIOFLUSH);     						/*清除串口的输入输出队列中的数据*/
			cfsetispeed(&Opt, speed_arr[i]);  			/*设置输入波特率*/
			cfsetospeed(&Opt, speed_arr[i]);   			/*设置输出波特率*/
			status = tcsetattr(fd, TCSANOW, &Opt);  /*设置串口新的termios结构*/
			if  (status != 0) 
			{        
				//perror("tcsetattr fd1");  
				return -1;     
			}    
			tcflush(fd,TCIOFLUSH); 
			return 1;						  
		}  
	}
	return -1;     
}

/*********************************************************************
所属功能单元：232动态库
功能描述：设置串口参数
输入参数：
-		fd				打开串口的文件句柄
-		databits	串口数据位
-		stopbits	停止位
-		parity		奇偶校验		
输出参数：
-		无
函数返回值：
-		TRUE			成功
-		FALSE			失败
内部处理流程：按输入参数进行串口参数设置
********************************************************************/
int32 commdrv_Set_Parity(int32 fd, int32 databits, int8 stopbits, int8 parity)
{ 
	struct termios options; 
	if ( tcgetattr( fd,&options)  !=  0) 	/*读取串口termios结构*/
	{ 
		//perror("SetupSerial 1");     
		return(FALSE);  
	}
	options.c_cflag &= ~CSIZE; 							/*清除原字符大小*/
	switch (databits) 											/*设置数据位数*/
	{   
	case 7:				
		options.c_cflag |= CS7;  						/*设置7位数据位数*/
		break;
	case 8:     
		options.c_cflag |= CS8; 						/*设置8位数据位数*/
		break;   
	default:    
		//fprintf(stderr,"Unsupported data size\n"); 
		return (FALSE);  
	}

	switch (parity) 
	{   
	case 'n':																			/*无校验*/
	case 'N':    
		options.c_cflag &= ~PARENB;   							/*清奇偶校验位*/
		options.c_iflag &= ~INPCK;     							/*奇偶校验无效*/ 
		break;  
	case 'o':   																	/*奇校验*/
	case 'O':     																
		options.c_cflag |= (PARODD | PARENB); 			/*设置为奇效验*/  
		options.c_iflag |= INPCK;             			/*使奇偶校验有效*/ 
		break;  
	case 'e':  																		/*偶校验*/
	case 'E':   
		options.c_cflag |= PARENB;     							/* 使奇偶校验有效*/ 
		options.c_cflag &= ~PARODD;   							/* 设置偶效验*/     
		options.c_iflag |= INPCK;       						/* 输入奇偶校验生效 */
		break;
	case 'S': 																		/*space校验*/
	case 's':  /*as no parity*/   
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;  
	default:   
		//fprintf(stderr,"Unsupported parity\n");    
		return (FALSE);  
	}  
	/* 设置停止位*/  
	switch (stopbits)
	{   
	case 1:    																		/*1位停止位*/
		options.c_cflag &= ~CSTOPB;  
		break;  
	case 2:    																		/*2位停止位*/
		options.c_cflag |= CSTOPB;  
		break;
	default:    
		//fprintf(stderr,"Unsupported stop bits\n");  
		return (FALSE); 
	} 

	/* 设置输入奇偶校验选项 */ 
	if (parity != 'n')   
		options.c_iflag |= INPCK; 													/* 输入奇偶检测生效 */
	tcflush(fd,TCIFLUSH);																	/* 清输入输出队列 */
	options.c_cc[VTIME] = 100; 														/* 设置超时15 seconds*/   
	options.c_cc[VMIN] = 0; 															/* 更新设置，使马上生效 */
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /* 输入 */
	options.c_oflag  &= ~OPOST;   												/* 输出 */	
	options.c_iflag &= ~IGNCR;							/*不忽略收到的回车字符*/
	options.c_iflag &= ~ICRNL;              /*不转换*/
	options.c_iflag &= ~(IXON | IXOFF | IXANY);	/*关硬件流控*/
	if (tcsetattr(fd,TCSANOW,&options) != 0)   						/* 对串口进行参数设置 */
	{ 
		return (FALSE);  
	} 
	return (TRUE);  
}

/*********************************************************************
所属功能单元：232动态库
功能描述：初始化串口接口
输入参数：
-		pCommPara		本地通讯参数
-		pPeerAddr		通讯对端参数
-		pCommStatus	通讯状态参数
输出参数：
-		无
函数返回值：
-		1			成功
-		-1		失败
内部处理流程：打开串口并进行串口初始化
********************************************************************/
int32 commdrv_Init_232(RS232_COMM_PARA *pCommPara)
{
	int   fd_232 = -1;                  /* 232标识符 */
	uint8 Devicename_232[30];					/*设备名，如"/dev/ttyS0" */
	int   dwBaudRate_232 = 0;           /* 波特率 */
	int8 byByteSize_232 = 0;           /* 数据位, 7-8 */
	int8 byParity_232   = 0;           /* 校验 0-4=None,Odd,Even,Mark,Space */
	int8 byStopBits_232 = 0;           /* 停止位 0,1,2 = 1, 1.5, 2 */    

	memcpy(Devicename_232, pCommPara->sDeviceName, 30);
	dwBaudRate_232 = pCommPara->dwBaudRate;
	byByteSize_232 = pCommPara->byByteSize;
	byParity_232 = pCommPara->byParity;
	byStopBits_232 = pCommPara->byStopBits;

	fd_232 = open((char*)Devicename_232, O_RDWR|O_NOCTTY|O_NONBLOCK);	/*打开串口*/
	if (fd_232 == -1)																						/*若打开串口失败，返回-1，并通知应用层*/
	{ 			
		printf("file:%s,line:%d,Can't Open Serial Port %s\n", __FILE__, __LINE__, Devicename_232);
		return -1;		
	}	

	if (commdrv_Set_lock(fd_232, F_WRLCK) == -1)	/*锁串口*/
	{
		printf("if (commdrv_Set_lock(fd_232, F_WRLCK) == -1) \n");
		close(fd_232);
		return -1;
	}

	if (commdrv_Set_speed(fd_232, dwBaudRate_232) == -1)	/*设置串口波特率*/
	{
		printf("Set speed Error %s\n", Devicename_232);
		close(fd_232);
		return -1;
	}			

	if (commdrv_Set_Parity(fd_232, byByteSize_232, byStopBits_232, byParity_232) == FALSE) /*设置串口参数 数据位 校验 停止位*/ 
	{
		printf("Set Parity Error %s   byByteSize_232 %d, byStopBits_232 %d, byParity_232 %d\n",
			Devicename_232, byByteSize_232, byStopBits_232, byParity_232);
		close(fd_232);
		return -1;
	}
	return fd_232;																							/*返回串口文件句柄*/
}

/*********************************************************************
所属功能单元：232动态库
功能描述：串口数据接收接口
输入参数：
-		fd_232		串口文件句柄
-		pBuffer		接收数据缓冲区
-		nBufLen		最大接收长度
-		rsptime     数据返回时间(20ms)
-		rcvtime     数据字节之间间隔时间(20ms)
-		endbyte     结束符，没有填0xff
输出参数：
-		pBuffer		接收到的数据
函数返回值：
-		data_len	接收到数据的长度
-		0			没有接收到数据
内部处理流程：
-		进行串口数据接收，当接收到超过15个字节或者数据接收完后
-		才将接收到的数据放入pBuffer缓冲区中，函数返回接收到数据
-		的长度
********************************************************************/
int32 commdrv_Recv_232 (int32 fd_232, uint8* pBuffer, int32 nBufLen, int32 rsptime, int32 rcvtime, uint8 endbyte)
{
	int32 recv_len = 0;		/*接收到数据长度*/
	int32 data_len = 0;		/*串口缓冲区中数据的长度*/
	int32 count = 0;
	fd_set set;
	struct timeval timeout;
	int32 n = 0;

	recv_len = 0;								
	data_len = 0;
	
	while (1)
	{
		FD_ZERO(&set);
		FD_SET(fd_232, &set);
		timeout.tv_sec = 0;
		timeout.tv_usec = 20000;
		do 
		{
			n = select(fd_232+1, &set, NULL, NULL, &timeout);
		}while(n<0);

		if (n > 0)	
			recv_len = read(fd_232, pBuffer+data_len, nBufLen-data_len);	/*调用read()，进行串口数据接收*/
		else
			recv_len = 0;

		if (recv_len > 0)	
		{
			#if(HARDWARE_TYPE == 9)
			Out2Led(LEDID_LOCAL_RESV,LED_ON);
			#endif
			data_len += recv_len;
			count = 0;

			if(endbyte != 0xff)
			{
				if(memchr(pBuffer+data_len-recv_len, endbyte, recv_len) != NULL) 
				{
					usleep(20000);
					#if(HARDWARE_TYPE == 9)
					Out2Led(LEDID_LOCAL_RESV,LED_OFF);
					#endif
					return data_len;					
				}
			}
		}
		else
		{
			if((data_len == 0)&&(count>rsptime)) 
			{
				usleep(20000);
				#if(HARDWARE_TYPE == 9)
				Out2Led(LEDID_LOCAL_RESV,LED_OFF);
				#endif
				return data_len; /*-接收数据超时-*/
			}

			if((data_len != 0)&&((count>rcvtime)||(data_len>nBufLen)))
			{
				usleep(20000);
				#if(HARDWARE_TYPE == 9)
				Out2Led(LEDID_LOCAL_RESV,LED_OFF);
				#endif
				return data_len;					
			}
		}
		count++;
	}	
	#if(HARDWARE_TYPE == 9)
	Out2Led(LEDID_LOCAL_RESV,LED_OFF);
	#endif
}

int32 commdrv_ReadSerial(int fd_232, int32 speed, uint8 *buf, int32 read_len)
{
	int32 retry = 0;
	int32 delay;
	int32 recv_len;
	int32 data_len;
	
	/*串口接收数据超时时间按20个字节间隔时间来计算，300bps 发送一个字节需要36667us
		一个字节包括起始位、停止位、校验位、8位数据 1000/(300/11) = 36.666ms*/
	if ((speed%300 != 0 )&&(speed <= 0))	/*若波特率不对，默认9600*/
		delay = 20*36667/(9600/300);
	else
		delay = 20*36667/(speed/300);

	
	data_len = 0;
	recv_len = 0;
	
	while (recv_len >= 0 && retry < 20)		/*read()在没有数据时，返回-1*/
	{
		#if(HARDWARE_TYPE == 9)
		Out2Led(LEDID_LOCAL_RESV,LED_ON);
		#endif
		usleep(delay);
		recv_len = read(fd_232, buf + data_len, read_len - data_len);
		if(recv_len == 0)
			retry++;
		else
		{
			retry = 0;
			data_len += recv_len;
			if (data_len >= read_len)
			{
				data_len = read_len;
				break;
			}
		}
	}
	#if(HARDWARE_TYPE == 9)
	Out2Led(LEDID_LOCAL_RESV,LED_OFF);
	#endif
	return data_len;
}


/*********************************************************************
所属功能单元：232动态库
功能描述：串口数据发送接口
输入参数：
-		fd_232			串口文件句柄
-		pBuffer			发送数据缓冲区
输出参数：
-		无 
函数返回值：
-		send_len		成功发送的数据长度
内部处理流程：将pBuffer缓冲区中的数据从串口发送出去
********************************************************************/
int32 commdrv_Send_232(int32 fd_232, uint8* pBuffer, int32 nBufLen)
{
	int32 send_len;
	#if(HARDWARE_TYPE == 9)
	Out2Led(LEDID_LOCAL_SEND,LED_ON);
	#endif
	ioctl(fd_232, CONTROL_CLEAR_RTS, 1);//带RTS使能
	send_len = write(fd_232, pBuffer, nBufLen);		/*发送数据*/
	tcdrain(fd_232);
	ioctl(fd_232, CONTROL_SET_RTS, 0);
	#if(HARDWARE_TYPE == 9)
	Out2Led(LEDID_LOCAL_SEND,LED_OFF);
	#endif
	return send_len;
}

//串口数据发送接口（带RTS使能）
int32 commdrv_Send_232_RTS(int32 fd_232, uint8* pBuffer, int32 nBufLen)
{
	int send_len = 0;
	#if(HARDWARE_TYPE == 9)
	Out2Led(LEDID_LOCAL_SEND,LED_ON);
	#endif
	ioctl(fd_232, CONTROL_CLEAR_RTS, 1);
	send_len = write(fd_232, pBuffer, nBufLen);   /* 发送数据 */
	tcdrain(fd_232);
	usleep(5000);
	ioctl(fd_232, CONTROL_SET_RTS, 0);
	#if(HARDWARE_TYPE == 9)
	Out2Led(LEDID_LOCAL_SEND,LED_OFF);
	#endif
	return send_len;
}

/*********************************************************************
所属功能单元：232动态库
功能描述：串口关闭接口
输入参数：
-		fd_232			串口文件句柄
-		pCommStatus	通讯状态参数
输出参数：
-		无 
函数返回值：
-		无
内部处理流程：关闭fd_232文件句柄
********************************************************************/
void commdrv_Close_232 (int32 fd_232)
{
	close(fd_232);
}
#endif

