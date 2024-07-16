/*
****************************************************************************
* Copyright (c) 2009,�Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* �������ƣ�commdrv.c
* �� �� �ţ�1.0
* ��    �ܣ�ϵͳ����������
����һ��C�ļ������ļ���Ҫ���������豸�������������紮�ڡ�����ȵ�
* ԭʼ�����˼�ʱ�䣺
*           gujun    2013-07-04
* �޸���Ա��ʱ�估�޸����ݣ�
* ������ 
* ע�⣺����̬�����ʵ��һЩ�������������漰�߼��������Բ��������ȫ����������̬���� 
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
�������ܵ�Ԫ��232��̬��
����������������
���������
-		fd �����ļ����
-		type ������ F_RDLCK ��������F_WRLCK ������
���������
-		�� 
��������ֵ��

�ڲ��������̣�
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


/*ͨѶ���ز���*/
/*********************************************************************
�������ܵ�Ԫ��232��̬��
�������������ô��ڲ�����
���������
-		fd		�򿪴��ڵ��ļ����
-		speed	���ڲ�����
���������
-		��
��������ֵ��
-		�� 
�ڲ��������̣��ȶ�ȡtermios�ṹ��Ȼ�����cfsetispeed cfsetospeed��
���ô��ڵ����롢��������ʡ�
********************************************************************/
int32 commdrv_Set_speed(int32 fd, int32 speed)
{
	int32 i; 
	int32 status; 
	int32 speed_arr[] = { B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300 };
	int32 name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200,  300};
	struct termios Opt; 	/*Opt�д洢���������ݽṹ*/

	tcgetattr(fd, &Opt); 	/*��ȡtermios�ṹ*/

	for ( i = 0;  i < sizeof(speed_arr) / sizeof(int32);  i++) 
	{ 
		if  (speed == name_arr[i])									/*����֧�ֵĲ�����*/
		{     
			tcflush(fd, TCIOFLUSH);     						/*������ڵ�������������е�����*/
			cfsetispeed(&Opt, speed_arr[i]);  			/*�������벨����*/
			cfsetospeed(&Opt, speed_arr[i]);   			/*�������������*/
			status = tcsetattr(fd, TCSANOW, &Opt);  /*���ô����µ�termios�ṹ*/
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
�������ܵ�Ԫ��232��̬��
�������������ô��ڲ���
���������
-		fd				�򿪴��ڵ��ļ����
-		databits	��������λ
-		stopbits	ֹͣλ
-		parity		��żУ��		
���������
-		��
��������ֵ��
-		TRUE			�ɹ�
-		FALSE			ʧ��
�ڲ��������̣�������������д��ڲ�������
********************************************************************/
int32 commdrv_Set_Parity(int32 fd, int32 databits, int8 stopbits, int8 parity)
{ 
	struct termios options; 
	if ( tcgetattr( fd,&options)  !=  0) 	/*��ȡ����termios�ṹ*/
	{ 
		//perror("SetupSerial 1");     
		return(FALSE);  
	}
	options.c_cflag &= ~CSIZE; 							/*���ԭ�ַ���С*/
	switch (databits) 											/*��������λ��*/
	{   
	case 7:				
		options.c_cflag |= CS7;  						/*����7λ����λ��*/
		break;
	case 8:     
		options.c_cflag |= CS8; 						/*����8λ����λ��*/
		break;   
	default:    
		//fprintf(stderr,"Unsupported data size\n"); 
		return (FALSE);  
	}

	switch (parity) 
	{   
	case 'n':																			/*��У��*/
	case 'N':    
		options.c_cflag &= ~PARENB;   							/*����żУ��λ*/
		options.c_iflag &= ~INPCK;     							/*��żУ����Ч*/ 
		break;  
	case 'o':   																	/*��У��*/
	case 'O':     																
		options.c_cflag |= (PARODD | PARENB); 			/*����Ϊ��Ч��*/  
		options.c_iflag |= INPCK;             			/*ʹ��żУ����Ч*/ 
		break;  
	case 'e':  																		/*żУ��*/
	case 'E':   
		options.c_cflag |= PARENB;     							/* ʹ��żУ����Ч*/ 
		options.c_cflag &= ~PARODD;   							/* ����żЧ��*/     
		options.c_iflag |= INPCK;       						/* ������żУ����Ч */
		break;
	case 'S': 																		/*spaceУ��*/
	case 's':  /*as no parity*/   
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;  
	default:   
		//fprintf(stderr,"Unsupported parity\n");    
		return (FALSE);  
	}  
	/* ����ֹͣλ*/  
	switch (stopbits)
	{   
	case 1:    																		/*1λֹͣλ*/
		options.c_cflag &= ~CSTOPB;  
		break;  
	case 2:    																		/*2λֹͣλ*/
		options.c_cflag |= CSTOPB;  
		break;
	default:    
		//fprintf(stderr,"Unsupported stop bits\n");  
		return (FALSE); 
	} 

	/* ����������żУ��ѡ�� */ 
	if (parity != 'n')   
		options.c_iflag |= INPCK; 													/* ������ż�����Ч */
	tcflush(fd,TCIFLUSH);																	/* ������������� */
	options.c_cc[VTIME] = 100; 														/* ���ó�ʱ15 seconds*/   
	options.c_cc[VMIN] = 0; 															/* �������ã�ʹ������Ч */
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /* ���� */
	options.c_oflag  &= ~OPOST;   												/* ��� */	
	options.c_iflag &= ~IGNCR;							/*�������յ��Ļس��ַ�*/
	options.c_iflag &= ~ICRNL;              /*��ת��*/
	options.c_iflag &= ~(IXON | IXOFF | IXANY);	/*��Ӳ������*/
	if (tcsetattr(fd,TCSANOW,&options) != 0)   						/* �Դ��ڽ��в������� */
	{ 
		return (FALSE);  
	} 
	return (TRUE);  
}

/*********************************************************************
�������ܵ�Ԫ��232��̬��
������������ʼ�����ڽӿ�
���������
-		pCommPara		����ͨѶ����
-		pPeerAddr		ͨѶ�Զ˲���
-		pCommStatus	ͨѶ״̬����
���������
-		��
��������ֵ��
-		1			�ɹ�
-		-1		ʧ��
�ڲ��������̣��򿪴��ڲ����д��ڳ�ʼ��
********************************************************************/
int32 commdrv_Init_232(RS232_COMM_PARA *pCommPara)
{
	int   fd_232 = -1;                  /* 232��ʶ�� */
	uint8 Devicename_232[30];					/*�豸������"/dev/ttyS0" */
	int   dwBaudRate_232 = 0;           /* ������ */
	int8 byByteSize_232 = 0;           /* ����λ, 7-8 */
	int8 byParity_232   = 0;           /* У�� 0-4=None,Odd,Even,Mark,Space */
	int8 byStopBits_232 = 0;           /* ֹͣλ 0,1,2 = 1, 1.5, 2 */    

	memcpy(Devicename_232, pCommPara->sDeviceName, 30);
	dwBaudRate_232 = pCommPara->dwBaudRate;
	byByteSize_232 = pCommPara->byByteSize;
	byParity_232 = pCommPara->byParity;
	byStopBits_232 = pCommPara->byStopBits;

	fd_232 = open((char*)Devicename_232, O_RDWR|O_NOCTTY|O_NONBLOCK);	/*�򿪴���*/
	if (fd_232 == -1)																						/*���򿪴���ʧ�ܣ�����-1����֪ͨӦ�ò�*/
	{ 			
		printf("file:%s,line:%d,Can't Open Serial Port %s\n", __FILE__, __LINE__, Devicename_232);
		return -1;		
	}	

	if (commdrv_Set_lock(fd_232, F_WRLCK) == -1)	/*������*/
	{
		printf("if (commdrv_Set_lock(fd_232, F_WRLCK) == -1) \n");
		close(fd_232);
		return -1;
	}

	if (commdrv_Set_speed(fd_232, dwBaudRate_232) == -1)	/*���ô��ڲ�����*/
	{
		printf("Set speed Error %s\n", Devicename_232);
		close(fd_232);
		return -1;
	}			

	if (commdrv_Set_Parity(fd_232, byByteSize_232, byStopBits_232, byParity_232) == FALSE) /*���ô��ڲ��� ����λ У�� ֹͣλ*/ 
	{
		printf("Set Parity Error %s   byByteSize_232 %d, byStopBits_232 %d, byParity_232 %d\n",
			Devicename_232, byByteSize_232, byStopBits_232, byParity_232);
		close(fd_232);
		return -1;
	}
	return fd_232;																							/*���ش����ļ����*/
}

/*********************************************************************
�������ܵ�Ԫ��232��̬��
�����������������ݽ��սӿ�
���������
-		fd_232		�����ļ����
-		pBuffer		�������ݻ�����
-		nBufLen		�����ճ���
-		rsptime     ���ݷ���ʱ��(20ms)
-		rcvtime     �����ֽ�֮����ʱ��(20ms)
-		endbyte     ��������û����0xff
���������
-		pBuffer		���յ�������
��������ֵ��
-		data_len	���յ����ݵĳ���
-		0			û�н��յ�����
�ڲ��������̣�
-		���д������ݽ��գ������յ�����15���ֽڻ������ݽ������
-		�Ž����յ������ݷ���pBuffer�������У��������ؽ��յ�����
-		�ĳ���
********************************************************************/
int32 commdrv_Recv_232 (int32 fd_232, uint8* pBuffer, int32 nBufLen, int32 rsptime, int32 rcvtime, uint8 endbyte)
{
	int32 recv_len = 0;		/*���յ����ݳ���*/
	int32 data_len = 0;		/*���ڻ����������ݵĳ���*/
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
			recv_len = read(fd_232, pBuffer+data_len, nBufLen-data_len);	/*����read()�����д������ݽ���*/
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
				return data_len; /*-�������ݳ�ʱ-*/
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
	
	/*���ڽ������ݳ�ʱʱ�䰴20���ֽڼ��ʱ�������㣬300bps ����һ���ֽ���Ҫ36667us
		һ���ֽڰ�����ʼλ��ֹͣλ��У��λ��8λ���� 1000/(300/11) = 36.666ms*/
	if ((speed%300 != 0 )&&(speed <= 0))	/*�������ʲ��ԣ�Ĭ��9600*/
		delay = 20*36667/(9600/300);
	else
		delay = 20*36667/(speed/300);

	
	data_len = 0;
	recv_len = 0;
	
	while (recv_len >= 0 && retry < 20)		/*read()��û������ʱ������-1*/
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
�������ܵ�Ԫ��232��̬��
�����������������ݷ��ͽӿ�
���������
-		fd_232			�����ļ����
-		pBuffer			�������ݻ�����
���������
-		�� 
��������ֵ��
-		send_len		�ɹ����͵����ݳ���
�ڲ��������̣���pBuffer�������е����ݴӴ��ڷ��ͳ�ȥ
********************************************************************/
int32 commdrv_Send_232(int32 fd_232, uint8* pBuffer, int32 nBufLen)
{
	int32 send_len;
	#if(HARDWARE_TYPE == 9)
	Out2Led(LEDID_LOCAL_SEND,LED_ON);
	#endif
	ioctl(fd_232, CONTROL_CLEAR_RTS, 1);//��RTSʹ��
	send_len = write(fd_232, pBuffer, nBufLen);		/*��������*/
	tcdrain(fd_232);
	ioctl(fd_232, CONTROL_SET_RTS, 0);
	#if(HARDWARE_TYPE == 9)
	Out2Led(LEDID_LOCAL_SEND,LED_OFF);
	#endif
	return send_len;
}

//�������ݷ��ͽӿڣ���RTSʹ�ܣ�
int32 commdrv_Send_232_RTS(int32 fd_232, uint8* pBuffer, int32 nBufLen)
{
	int send_len = 0;
	#if(HARDWARE_TYPE == 9)
	Out2Led(LEDID_LOCAL_SEND,LED_ON);
	#endif
	ioctl(fd_232, CONTROL_CLEAR_RTS, 1);
	send_len = write(fd_232, pBuffer, nBufLen);   /* �������� */
	tcdrain(fd_232);
	usleep(5000);
	ioctl(fd_232, CONTROL_SET_RTS, 0);
	#if(HARDWARE_TYPE == 9)
	Out2Led(LEDID_LOCAL_SEND,LED_OFF);
	#endif
	return send_len;
}

/*********************************************************************
�������ܵ�Ԫ��232��̬��
�������������ڹرսӿ�
���������
-		fd_232			�����ļ����
-		pCommStatus	ͨѶ״̬����
���������
-		�� 
��������ֵ��
-		��
�ڲ��������̣��ر�fd_232�ļ����
********************************************************************/
void commdrv_Close_232 (int32 fd_232)
{
	close(fd_232);
}
#endif

