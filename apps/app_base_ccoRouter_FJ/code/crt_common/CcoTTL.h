/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/


#ifndef __CCO_TTL_H__
#define __CCO_TTL_H__

#include "CcoRedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <linux/input.h>
#include <linux/hidraw.h>
#define MOUSE_DEV_PATH    "/dev/hidraw0"

/*******************************************************************************
 * ��������
*******************************************************************************/
extern int32 PlcInitTTL(COMM_PARA_T *pCommPara);
extern int32 PlcRecvTTL(int fd_232, int32 speed, uint8 *pBuffer, int32 nBufLen);
extern int32 PlcSendTTL(int32 fd_232, uint8 *pBuffer, int32 nBufLen);
extern void PlcCloseTTL (int32 fd_232);




#ifdef __cplusplus
}
#endif

#endif // __CCO_TTL_H__