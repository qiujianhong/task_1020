/*
****************************************************************************
* Copyright (c) 2013,�Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* �������ƣ�commdrv.h 
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

#ifndef __AC_METER_COMMDRV_H__
#define __AC_METER_COMMDRV_H__

#include "ac_redef.h"

extern char *commdrv_getnetip(char *netsocket);


#define CONTROL_SET_RTS			0X6501
#define CONTROL_CLEAR_RTS		0X6502
#define CONTROL_GET_RTSSTATE	0X6503

typedef struct
{
	uint8 sDeviceName[30];		/*�豸������"/dev/ttyS0" */
	int dwBaudRate;			/*������*/
	int8 byByteSize;			/*����λ, 4-8 */
	int8 byParity;				/*У�� 'n'  'e'  'o'  's' = None,Odd,Even,Space*/
	int8 byStopBits;			/*ֹͣλ 0,1,2 = 1, 1.5, 2*/
	int fd_232;
} RS232_COMM_PARA;

extern void commdrv_Close_232 (int fd_232);					//���ڹرսӿ�
extern int commdrv_Init_232(RS232_COMM_PARA *pCommPara);		//��ʼ�����ڽӿ�
extern int commdrv_Recv_232 (int fd_232, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime, uint8 endbyte);		//�������ݽ��սӿ�
extern int commdrv_ReadSerial(int fd_232, int speed, uint8 *buf, int read_len);
extern int commdrv_Send_232(int fd_232, uint8* pBuffer, int nBufLen);					//�������ݷ��ͽӿ�
extern int commdrv_Send_232_RTS(int fd_232, uint8* pBuffer, int nBufLen);				//�������ݷ��ͽӿڣ���RTSʹ�ܣ�
extern int commdrv_Set_Parity(int fd, int databits, int8 stopbits, int8 parity);	//���ô��ڲ���
extern int commdrv_Set_speed(int fd, int speed);		//���ô��ڲ�����

#endif /* #ifndef __AC_METER_COMMDRV_H__ */
