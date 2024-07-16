/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   计算MD5值
* @date：    2022-7-6
* @history： 
* @User deatil:
*********************************************************************
*/

#ifndef __MD5_SUM_H_
#define __MD5_SUM_H_


#ifdef __cplusplus
    extern "C" {
#endif

int GetDataMD5(unsigned char *pData, unsigned int dataLen, unsigned char *pOutMd5);
int Md5Region(char* pFileName, unsigned int startOff,unsigned int endOff,unsigned char *pOutMd5);

#ifdef __cplusplus
}
#endif

#endif

