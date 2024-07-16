/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#include "CcoTTL.h"

#include "CcoCommon.h"
#include "CcoDebug.h"
#include "CcoFiles.h"
#include "CcoLib.h"
#include "CcoPrtl.h"

unsigned char buffer[1000];


/*******************************************************************************
* 函数名称: LibVersion
* 函数功能: 读动态库版本接口
* 输入参数：无
* 输出参数：无
* 返 回 值: 无
*******************************************************************************/
int LibVersion(void)
{
	printf("-COMPILE TIME : %s %s\n",__TIME__,__DATE__);
	printf("-GCC VERSION  : %s\n",__VERSION__);
	return 0;
}

/*******************************************************************************
* 函数名称: PlcSetSpeed
* 函数功能: 设置串口波特率
* 输入参数：fd			        串口文件句柄
*		    speed		        串口波特率
* 输出参数：按输入参数进行串口参数设置
* 返 回 值: 1			        成功
*		    0			        失败
*******************************************************************************/
int32 PlcSetSpeed(int32 fd, int32 speed)
{
	int32 i; 
	int32 status; 
	int32 speed_arr[] = { B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300 };
	int32 name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200,  300};
	struct termios Opt; 	                    /*Opt中存储波特率数据结构*/
	
	tcgetattr(fd, &Opt); 	                    /*获取termios结构*/
	
	for (i = 0; i < sizeof(speed_arr) / sizeof(int32); i++) 
	{ 
		if  (speed == name_arr[i])			    /*查找支持的波特率*/
		{     
			tcflush(fd, TCIOFLUSH);     		/*清除串口的输入输出队列中的数据*/
			cfsetispeed(&Opt, speed_arr[i]);    /*设置输入波特率*/
			cfsetospeed(&Opt, speed_arr[i]);    /*设置输出波特率*/

			/*设置串口新的termios结构*/
			status = tcsetattr(fd, TCSANOW, &Opt);  
			if (status != 0) 
			{        
				return -1;     
			}    
			tcflush(fd, TCIOFLUSH); 
			return 1;						  
		}  
	}
	return -1;     
}

/*******************************************************************************
* 函数名称: PlcSetParity
* 函数功能: 设置串口参数
* 输入参数：fd			        串口文件句柄
*		    databits		    串口数据位
*		    stopbits            停止位
*		    parity              奇偶校验
* 输出参数：按输入参数进行串口参数设置
* 返 回 值: 1			        成功
*		    0			        失败
*******************************************************************************/
int32 PlcSetParity(int32 fd, int32 databits, int8 stopbits, int8 parity)
{ 
	struct termios options; 

	/*读取串口termios结构*/
	if (tcgetattr( fd,&options)  !=  0) 	
	{ 
		return 0;  
	}

	/*清除原字符大小*/
	options.c_cflag &= ~CSIZE;

	/*设置数据位数*/
	switch (databits) 											
	{   
	case 7:				                        /*设置7位数据位数*/
		options.c_cflag |= CS7;  						
		break;
		
	case 8:                                     /*设置8位数据位数*/
		options.c_cflag |= CS8; 						
		break;
		
	default:    
		return 0;  
	}

	switch (parity) 
    {   
	case 'n':						            /*无校验*/
	case 'N':    
		options.c_cflag &= ~PARENB;   	        /*清奇偶校验位*/
		options.c_iflag &= ~INPCK;     			/*奇偶校验无效*/ 
		break;  
		
	case 'o':   								/*奇校验*/
	case 'O':     																
		options.c_cflag |= (PARODD | PARENB);   /*设置为奇效验*/  
		options.c_iflag |= INPCK;               /*使奇偶校验有效*/ 
		break;  
		
	case 'e':  							        /*偶校验*/
	case 'E':   
		options.c_cflag |= PARENB;     		    /* 使奇偶校验有效*/ 
		options.c_cflag &= ~PARODD;   		    /* 设置偶效验*/     
		options.c_iflag |= INPCK;       	    /* 输入奇偶校验生效 */
		break;
		
	case 'S': 									/*space校验*/
	case 's':                                   /*as no parity*/   
  	    options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;  
		
	default:   
		return 0;  
    }  
    
    /* 设置停止位*/  
	switch (stopbits)
    {   
	case 1:    							        /*1位停止位*/
		options.c_cflag &= ~CSTOPB;  
		break;  
		
	case 2:    									/*2位停止位*/
		options.c_cflag |= CSTOPB;  
   	    break;
   	
	default:    
	 	return 0; 
    } 
	
    /* 设置输入奇偶校验选项 */ 
	if (parity != 'n')
	{
		options.c_iflag |= INPCK; 		        /* 输入奇偶检测生效 */
    }
	tcflush(fd,TCIFLUSH);					    /* 清输入输出队列 */
	options.c_cc[VTIME] = 100; 				    /* 设置超时15 seconds*/   
	options.c_cc[VMIN] = 0; 				    /* 更新设置，使马上生效 */
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /* 输入 */
	options.c_oflag  &= ~OPOST;   			    /* 输出 */	
	options.c_iflag &= ~IGNCR;				    /*不忽略收到的回车字符*/
	options.c_iflag &= ~ICRNL;                  /*不转换*/
	options.c_iflag &= ~(IXON | IXOFF | IXANY);	/*关硬件流控*/
	
	if (tcsetattr(fd,TCSANOW,&options) != 0)   	/* 对串口进行参数设置 */
	{ 
		return 0;  
	} 
	return 1;  
}

/*******************************************************************************
* 函数名称: PlcSetlock
* 函数功能: 锁串口
* 输入参数：fd		            串口文件句柄
*           type		        锁类型 F_RDLCK 共享锁；F_WRLCK 互斥锁
* 输出参数：无
* 返 回 值: >0                  成功，返回文件描述符
*		    <0                  失败
*******************************************************************************/
int32 PlcSetlock(int32 fd, int32 type)
{
	struct flock lock;
	
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 1;
	lock.l_type = type;
	
	if (fcntl(fd, F_SETLK, &lock) == 0)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

/*******************************************************************************
* 函数名称: PlcInitTTL
* 函数功能: 初始化串口接口
* 输入参数：pCommPara		    串口通讯参数
* 输出参数：无
* 内部处理：打开串口并进行串口初始化
* 返 回 值: fdTTL              成功，返回文件描述符
*		    -1                  失败
*******************************************************************************/
int32 PlcInitTTL(COMM_PARA_T *pCommPara)
{
	int32 fdTTL;						        /*232标识符*/

	uint8 DeviceNameTTL[30];				    /*设备名，如"/dev/ttyS0" */
	//int32 BaudRateTTL;					        /*波特率*/
	//int8 DataBitsTTL;					        /*数据位, 7-8 */
	//int8 ParityFlagTTL;					        /*校验 0-4=None,Odd,Even,Mark,Space*/
	//int8 StopBitsTTL;					        /*停止位 0,1,2 = 1, 1.5, 2*/	

	memcpy(DeviceNameTTL, pCommPara->DeviceName, 30);
	//BaudRateTTL = pCommPara->BaudRate;
	//DataBitsTTL = pCommPara->DataBits;
	//ParityFlagTTL = pCommPara->ParityFlag;
	//StopBitsTTL = pCommPara->StopBits;

	/*打开HID串口*/
    fdTTL = open((char*)DeviceNameTTL, O_RDWR|O_NOCTTY|O_NONBLOCK);
    if (fdTTL == -1)
    {
        printf("\n mouse device open fail\n\n");
        return -1;
    }
    printf("\n open successful\n\n");

	/*返回串口文件句柄*/
	return fdTTL;					
}

/*******************************************************************************
* 函数名称: PlcRecv232Block
* 函数功能: 串口数据接收接口(阻塞版本)
* 输入参数：fdTTL		        串口文件句柄
*		    pBuffer		        接收数据缓冲区
*		    nBufLen		        最大接收长度
*		    rsptime             数据返回最大等待时间(单位：20ms)
*		    rcvtime             接收该数据帧最大间隔时间(单位：20ms)
*		    endbyte             结束符，没有填0xff
* 输出参数：pBuffer             接收到的数据
* 返 回 值: data_len            接收到数据的长度
*		    0                   没有接收到数据
*******************************************************************************/
int32 PlcRecv232Block(int32 fdTTL, uint8* pBuffer, int32 nBufLen, int32 rsptime, int32 rcvtime, uint8 endbyte)
{
	int32 recv_len;			                /*接收到数据长度*/
	int32 data_len;			                /*串口缓冲区中数据的长度*/
	int32 count=0;
	fd_set set;
	struct timeval timeout;
	int32 n = 0;
	
	recv_len = 0;								
	data_len = 0;
	
	while (1)
	{
		FD_ZERO(&set);
		FD_SET(fdTTL, &set);
		timeout.tv_sec = 0;
		timeout.tv_usec = 20000;
		do 
		{
			n = select(fdTTL+1, &set, NULL, NULL, &timeout);
		}while(n<0);

		/*调用read()，进行串口数据接收*/
		if (n > 0)	
		{
			recv_len = read(fdTTL, pBuffer+data_len, nBufLen-data_len);	
        }
		else
		{
		    recv_len = 0;
        }
        
		if (recv_len > 0)	
		{
			data_len += recv_len;
			count = 0;
            if (endbyte != 0xff)
            {
                if (memchr(pBuffer+data_len-recv_len, endbyte, recv_len) != NULL) 
                {
                    return data_len;					
                }
            }
		}
		else
		{
			if ((data_len == 0)&&(count>rsptime))
			{
				return data_len; /*-接收数据超时-*/
			}
			if ((data_len != 0) && ((count > rcvtime) || (data_len > nBufLen)))
			{
				return data_len;					
			}
		}
        usleep(20000);
		count++;
	}	
	
	return 0;
}

/*******************************************************************************
* 函数名称: PlcInitTTL
* 函数功能: 串口数据接收接口(非阻塞版本)
* 输入参数：fd_232		        串口文件句柄
*		    speed		        串口波特率
*		    pBuffer		        接收数据缓冲区
*		    nBufLen		        最大接收长度
* 输出参数：pBuffer             接收到的数据
* 内部处理：串口接收数据超时时间按20个字节间隔时间来计算，300bps 发送一个字节需要
*   36667us，一个字节包括起始位、停止位、校验位、8位数据 1000/(300/11) = 36.666ms
* 返 回 值: data_len            接收到数据的长度
*		    0                   没有接收到数据
*******************************************************************************/
int32 PlcRecvTTL(int fdTTL, int32 speed, uint8 *pBuffer, int32 nBufLen)
{
    int32 data_len;
    
    data_len = 0;

    /*调用read()，进行串口数据接收*/
    data_len = read(fdTTL, pBuffer, MAX_HID_RECV_LEN);    
    return data_len;    
}

/*******************************************************************************
* 函数名称: PlcSendTTL
* 函数功能: 串口数据发送接口
* 输入参数：fd_232		        串口文件句柄
*		    pBuffer		        发送数据缓冲区
*		    nBufLen		        最大接收长度
* 输出参数：pBuffer             接收到的数据
* 内部处理：将pBuffer缓冲区中的数据从串口发送出去
* 返 回 值: send_len            成功发送的数据长度
*******************************************************************************/
int32 PlcSendTTL(int32 fdTTL, uint8* pBuffer, int32 nBufLen)
{
	int32 send_len;
    uint8  HidBuff[MAX_HID_SEND_LEN];
    char   buff[MAX_HID_SEND_LEN] = {0};
    int    i;
    
    for(i = 0;i<((nBufLen/64)+1);i++)/*64字节分包处理*/
    {
        memset(HidBuff,0x00,sizeof(HidBuff));
        if((i+1)*64 > nBufLen)
        {
            memcpy(HidBuff,&pBuffer[i*64],nBufLen-i*64);
        }
        else
        {
            memcpy(HidBuff,&pBuffer[i*64],64);
        }
        
        memcpy(&buff[1],HidBuff,64);/*hidraw串口发送,第一个字节要补0*/
        //commfun_printstring(buff,65);
        
        /*发送数据*/
        send_len = write(fdTTL, buff, 65); 
        
        /*等待串口缓冲区数据全部发送完毕*/
        tcdrain(fdTTL);                            
    }


    
	return send_len;
}

/*******************************************************************************
* 函数名称: PlcCloseTTL
* 函数功能: 串口关闭接口
* 输入参数：fd_232		        串口文件句柄
* 输出参数：无 
* 返 回 值: 无 
*******************************************************************************/
void PlcCloseTTL (int32 fdTTL)
{
	close(fdTTL);
}