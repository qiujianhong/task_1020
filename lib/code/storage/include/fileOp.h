/*
***********************************************************************************
  Copyright (c) 2006-2007,�Ͼ����������������޹�˾
             All rights reserved.
 
  �ļ���: FileOp.h
  ��  ��: �ļ�ϵͳ������ͷ�ļ�
  ������:    zhangyu
  ������:    
    
  �޶���ʷ:
  
  2006-11-22 : 
        v1.0, zhangyu, �½�
    
***********************************************************************************
*/

#ifndef _FILE_OP_H
#define    _FILE_OP_H

#include "../../include/baseCommon.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef PATH_MAX
    #define PATH_MAX    256
#endif

#ifndef NAME_MAX
    #define NAME_MAX    64
#endif

BOOL makePathedDir(const char *path);

ssize_t FGetFileSize(int fd);

ssize_t GetFileSize(const char *filename);

long        GetFilesSizeRecu(const char *filename);

BOOL GetDiskUsage(const char *diskpath, uint32 *free, uint32 *total);

int ExtractFilePath(const char *src, char *path);

int ExtractFileExt(const char *src, char *ext);

int ExtractFileName(const char *src, char *filename);

// ��ȡ����׺ȫ�����ļ�
int ExtractFileNameWithPostfix(const char *src, char *filename);

BOOL FileExists(const char *filepath);
BOOL PathExists(const char *pathname);

#ifdef __cplusplus
}
#endif

#endif //#ifndef _FILE_OP_H
