/*
****************************************************************************
* Copyright (c) 2013,南京新联电子股份有限公司
* All rights reserved.
* 程序名称：commdrv.h 
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

#ifndef _COMMDRV_H
#define _COMMDRV_H

#ifdef __cplusplus
extern "C" {
#endif

#if HARDWARE_IV_EN == 1
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#define AF_CAN          29      /* Controller Area Network      */
#define PF_CAN          AF_CAN

int commdrv_Init_Can (char *drvcan_name);
int commdrv_Send_Can(int fd_can,canid_t can_id,char * Sendbuf, int Sendlen);
int commdrv_Recv_Can(int fd_can,char *Recvbuf);
#endif

char *commdrv_getnetip(const char *netsocket);
int commfun_SetNetip(const char *ifname, char *Ipaddr);


#define CONTROL_SET_RTS			0X6501
#define CONTROL_CLEAR_RTS		0X6502
#define CONTROL_GET_RTSSTATE	0X6503

typedef struct
{
	uint8 sDeviceName[30];		/*设备名，如"/dev/ttyS0" */
	int dwBaudRate;			/*波特率*/
	int8 byByteSize;			/*数据位, 4-8 */
	int8 byParity;				/*校验 'n'  'e'  'o'  's' = None,Odd,Even,Space*/
	int8 byStopBits;			/*停止位 0,1,2 = 1, 1.5, 2*/
	int fd_232;
} RS232_COMM_PARA;
void commdrv_Close_232 (int fd_232);					//串口关闭接口
int commdrv_Init_232(RS232_COMM_PARA *pCommPara);		//初始化串口接口
int commdrv_Recv_232 (int fd_232, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime, uint8 endbyte);		//串口数据接收接口
int commdrv_ReadSerial(int fd_232, int speed, uint8 *buf, int read_len);
int commdrv_Send_232(int fd_232, uint8* pBuffer, int nBufLen);					//串口数据发送接口
int commdrv_Send_232_RTS(int fd_232, uint8* pBuffer, int nBufLen);				//串口数据发送接口（带RTS使能）
int commdrv_Set_Parity(int fd, int databits, int8 stopbits, int8 parity);	//设置串口参数
int commdrv_Set_speed(int fd, int speed);		//设置串口波特率

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _COMMDRV_H */
