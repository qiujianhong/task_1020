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

#ifndef __CCO_COMMDRV_H__
#define __CCO_COMMDRV_H__

#include "CcoRedef.h"
#include "commdrv.h"

#define HARDWARE_TYPE               4       //1:一型大终端;2:二型江苏终端;3:三型208终端;4:集中器终端;9:II型集中器

extern char *commdrv_getnetip(const char *netsocket);


#define CONTROL_SET_RTS			0X6501
#define CONTROL_CLEAR_RTS		0X6502
#define CONTROL_GET_RTSSTATE	0X6503


extern void commdrv_Close_232 (int fd_232);					//串口关闭接口
extern int commdrv_Init_232(RS232_COMM_PARA *pCommPara);		//初始化串口接口
extern int commdrv_Recv_232 (int fd_232, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime, uint8 endbyte);		//串口数据接收接口
extern int commdrv_ReadSerial(int fd_232, int speed, uint8 *buf, int read_len);
extern int commdrv_Send_232(int fd_232, uint8* pBuffer, int nBufLen);					//串口数据发送接口
extern int commdrv_Send_232_RTS(int fd_232, uint8* pBuffer, int nBufLen);				//串口数据发送接口（带RTS使能）
extern int commdrv_Set_Parity(int fd, int databits, int8 stopbits, int8 parity);	//设置串口参数
extern int commdrv_Set_speed(int fd, int speed);		//设置串口波特率

#endif /* #ifndef __CCO_COMMDRV_H__ */
