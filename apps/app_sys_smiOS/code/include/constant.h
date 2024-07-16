/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� constant.h
* @date�� 2019-11-9
*********************************************************************
*/
#ifndef __CONSTANT_H__
#define __CONSTANT_H__

#include "baseConstant.h"

#ifdef __cplusplus
extern "C"
{
#endif

//����IP��Ϣ�����ļ�·��
#define	IP_INFO_DIR				"/data/app/smiOS"
//smiOS�����ļ�Ĭ��·��
#define CFG_PATH                "/usr/local/extapps/smiOS/bin/smiOS.ini"

//Ĭ�ϵķ�������
#define SERVICE_NAME_BASE       "smiOS"

//���볤��
#define PASSWD_LEN (64)
#define MAX_LINE_NUM (1024)
//token��󳤶�
#define TOKEN_MAX_LEN (64)
//#define MAX_ADDR_LEN (64)
//IP����
#define IP_LEN (16)
//�˿��ַ�������
#define PORT_STR_LEN (6)
//�����볤��
#define ERROR_CODE_LEN (6)
#define MAX_CONTENT_LEN (1024)
//token��Чʱ��
#define MAX_TOKEN_VALID_TIME (60 * 10)

#define APP_MAX_SUB_NUM (16)
#define APP_MAX_SUB_POUINT_NUM (128)

#define APP_MAX_SHARE_DIR_LEN (128)
#define APP_START_LIST_FILE_NAME_LEN (128)
#define DOCKER_START_LIST_FILE_NAME_LEN (128)
#define APP_SIGN_FILE_PATH_LEN (128)
#define APP_REGISTER_LIST_FILE_NAME_LEN (128)
#define LOCAL_IP_FILE_NAME_LEN (128)

#ifdef __cplusplus
}
#endif

#endif