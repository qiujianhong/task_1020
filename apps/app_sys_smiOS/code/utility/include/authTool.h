/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： authTool.h
* @date： 2019-11-9
*********************************************************************
*/
#ifndef __AUTH_TOOL_H__
#define __AUTH_TOOL_H__

#ifdef __cplusplus
extern "C" {
#endif

//生成password
int md5_password(char *pin, char *pout);
int creat_access_token(char *token_out);
int check_token(char *token_out);

#ifdef __cplusplus
}
#endif

#endif