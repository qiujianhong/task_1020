/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� pidfile.h
* @date�� 2019-11-9
*********************************************************************
*/

#ifndef __PIDFILE_H__
#define __PIDFILE_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <string.h>
#include <errno.h>
#include "baseType.h"

#ifdef __cplusplus
extern "C" {
#endif

#define set_pid_file(file_name) i_set_pid_file(file_name)

typedef uint8 Bool;
/* @brief:��������pid_file����  */
sint32 i_set_pid_file(const sint8 *file_name);

Bool singleton_check(char *appName);
#ifdef __cplusplus
}
#endif

#endif