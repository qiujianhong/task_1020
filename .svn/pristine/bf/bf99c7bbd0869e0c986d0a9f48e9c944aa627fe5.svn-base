/*
************************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限公司
             All rights reserved.
 
  文件名: FileOp.c
  功  用: 文件系统操作实用库
  创建人:    zhangyu
  创建日:    
    
  修订历史:
  
  2006-11-22 : 
            v1.0, zhangyu, 新建
    
***********************************************************************************
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "fileOp.h"

extern int errno;
/***************************************************************
                                                        内部调用
 ***************************************************************/

 
 
/***************************************************************
                                                    公共调用接口
 ***************************************************************/

/*
  名  称: GetFileSize
  功  能: 获取文件容量
  输  入: 
      文件名
  输  出: 
  返回值:
            -1  : 错误
            >=0 : 文件/子目录占用字节数
 */
inline ssize_t GetFileSize(const char *filename)
{
    struct     stat sta;
    
    if( stat( filename, &sta) < 0)
        return -1;
        
    if( S_ISREG(sta.st_mode)) //普通文件
    {
        return sta.st_size;
    }
    else 
        return -1;
}

/*
  名  称: FGetFileSize
  功  能: 获取文件尺寸
  输  入: 
      文件描述符fd
  输  出: 
  返回值:
            -1  : 错误
            >=0 : 文件/子目录占用字节数
 */
inline ssize_t FGetFileSize(int fd)
{
    struct stat sta;
    
    if(fstat(fd, &sta) < 0)    
        return -1;
    
    return sta.st_size;    
}

/*
  名  称: GetFileSizesRecu
  功  能: 获取文件或者子目录内文件所实际占用的总字节数
  输  入: 
  输  出: 
  返回值:
            -1  : 错误
            >=0 : 文件/子目录占用字节数
 */
long GetFilesSizeRecu(const char *filename)
{
    struct                 stat sta;
    long                    size = 0, iret;
    DIR                        *dirp;
    struct dirent *entp;
    char                    path[PATH_MAX + 1];

    if( stat( filename, &sta) < 0)
    {
        puts(strerror(errno));
        return -1;
    }
        
    if( S_ISREG(sta.st_mode)) //普通文件
    {
        return sta.st_size;
    }
    else if ( S_ISDIR(sta.st_mode)) //子目录
    {
        if( (dirp = opendir(filename)) == NULL)
            return -1;
            
        while((entp = readdir(dirp)) != NULL)    {
            if( (strcmp(entp->d_name, ".") == 0) || ((strcmp(entp->d_name, "..") == 0)))
                continue;
                
            strcpy(path, filename);
            strcat(path, "/");
            strcat(path, entp->d_name);
            
            //printf("->ready to test %s\n", path);            
            if( (iret = GetFilesSizeRecu(path)) < 0)
            {
                closedir(dirp);
                return -1;
            }
            size += iret;            
        }
        
//        printf("->return from: [%s]\n", filename);        
        closedir(dirp);
    }
    else
        return -1;
    

    return size;
}


/***************************************************************
                                                    创建多层目录
 ***************************************************************/

/*
  名  称: getNextPath
  功  能: 获得src字符串中的第一个目录路径
  输  入:
      src    - 输入的字符串首地址      
      bufLen - 缓冲区长度
  输  出: 
      dirName - 返回的目录名称('\0'结尾),输入时给足够空间
  返回值:
            !NULL - 下一个目录路径的起始地址
            NULL  - 表示解析结束了
            
 */
inline static char *getNextPath(const char *src, const char *dirName, unsigned bufLen)
{
    int     cnt  = 0;
    char *pdst, *ptr;
    
    if((NULL == src) || (NULL == dirName)) return NULL;    
    
    pdst = (char *)dirName;

    for(ptr = (char *)src; (*ptr != '\0') && (*ptr == '/'); ptr++); //去掉多余的路径名前导'/'
            
    while( (*ptr != '\0') && (cnt < bufLen - 1) && (*ptr != '/') ) {
        *pdst++ = *ptr++;
        cnt++;
    }    
    *pdst = '\0';
    
    if(*ptr == '\0')
        return NULL;    
            
    return ptr;
}

/*
  名  称: makePathedDir
  功  能: 按照path字符串指定的路径逐个生成子目录
  输  入:
          path - 待生成的路径目录(绝对路径)
  输  出: 
  返回值:

 */
BOOL makePathedDir(const char *path)
{
    char         *ptr;
    char         filepath[PATH_MAX + 1];
    char         dirName[128];
    BOOL      bRelative = FALSE;
    
    ptr = (char *)path;        
    if(NULL == path) return FALSE;
    
    if('/' != *ptr)  
        bRelative = TRUE;
        
    if( (ptr = getNextPath(ptr, dirName, 128)) != NULL)  //获得根目录
    {
        if(!bRelative) // 如果创建的是绝对路径，则在创建目录名前加'/'表示绝对路径
            strcpy(filepath, "/");
        else
            filepath[0] = '\0';
            
        strcat(filepath, dirName);

        if( mkdir(filepath, 0770) < 0)
            if( errno != EEXIST)
                return FALSE;

    }
    else //根目录提取错误
        return FALSE;
    
    while( (ptr = getNextPath(ptr, dirName, 128) ) != NULL)
    {
        strcat(filepath, "/");
        strcat(filepath, dirName);

        if( mkdir(filepath, 0770) < 0)
            if(errno != EEXIST)
                return FALSE;

    }
    
    if( *dirName != '\0')
    {
        strcat(filepath, "/");
        strcat(filepath, dirName);

        if( mkdir(filepath, 0777) < 0)
            if(errno != EEXIST)
                return FALSE;

    }
    
    return TRUE;
}

/*
  名  称: FileExists
  功  能: 检测文件是否存在
  输  入:
             filepath - 查找文件是否存在
  输  出:
 
  返回值:
             TRUE     - 存在
             FALSE - 不存在
 */
BOOL FileExists(const char *filepath)
{
    struct stat sta;
    
    if( stat(filepath, &sta) < 0)
        return FALSE;
        
    if( S_ISREG(sta.st_mode))
        return TRUE;

    return FALSE;
}

/*
  名  称: PathExists
  功  能: 检测目录是否存在
  输  入:
             filepath - 查找文件是否存在
  输  出:
 
  返回值:
             TRUE     - 存在
             FALSE - 不存在
 */
BOOL PathExists(const char *pathname)
{
    struct stat sta;
    
    if( stat(pathname, &sta) < 0)
        return FALSE;
        
    if( S_ISDIR(sta.st_mode))
        return TRUE;

    return FALSE;
}


/*
  名  称: deleteFile
  功  能: 删除文件
  输  入: 
          fileName - 文件名
  输  出: 
  返回值:

 */
BOOL deleteFile(const char *fileName)
{
    remove(fileName);    
    return TRUE;
}

/*
  名  称: deleteDir
  功  能: 删除目录
  输  入: 
          fileName - 文件名
  输  出: 
  返回值:

 */
BOOL deleteDir(const char *dirName)
{
    if(PathExists(dirName))
        remove(dirName);
    
    return TRUE;
}

/***************************************************************
                                            全路径文件名操作函数
 ***************************************************************/
 
/*
  名  称: ExtractFilePath
  功  能: 从包含有完整路径的文件名中提取出其路径
  输  入:
      src  - 源字符串
  输  出: 
      path - 目的写入字符串
  返回值:
            -1 - 失败
            0     - 成功
 */
inline int ExtractFilePath(const char *src, char *path)
{
    int     len, frag;
    char     *off;
    
    if( !src || !path) return -1;
        
    len = strlen(src);
    
    //printf("%d\n", len);

    off = (char *)&src[len - 1];
    
    //printf("%s\n", off);
    
    frag = 0;
    while((off != &src[0]) && (*off != '/'))
    {
        off--;
        frag++;
    }
        
    strncpy(path, src, len - frag);  //!这个函数不会自动在字符串后面加'\0'
    path[len - frag] = '\0';
    return 0;
}

/*
  名  称: ExtractFileExt
  功  能: 提取文件的扩展名
  输  入:
  输  出: 
  返回值:

 */
int ExtractFileExt(const char *src, char *ext)
{
    int     len, frag;
    char     *off;
    
    if( !src || !ext) return -1;
        
    len = strlen(src);
    
    off = (char *)&src[len - 1];
    
    frag = 0;
    while((off != &src[0]) && (*off != '/') && (*off != '.'))
    {
        off--;
        frag++;
    }

    if( *off == '/')
    {
        *ext = '\0';
        return 0;
    }
    
    strncpy(ext, off + 1, len - frag);
    return 0;
}

/*
  名  称: ExtractFileName
  功  能: 提取不包含路径的文件名字符串
  输  入:
  输  出: 
  返回值:

 */
inline int ExtractFileName(const char *src, char *filename)
{
    int     len, cnt, i;
    char     *off;
    char  name[NAME_MAX + 1];
    char  name2[NAME_MAX + 1];

    memset(name, 0x00, sizeof(name));

    if( !src || !filename) return -1;
        
    len = strlen(src);
    
    off = (char *)&src[len - 1];

    cnt = 0;
    while((off != &src[0]) && (*off != '/'))
    {
        name[cnt++] = *off--;
    }
    //printf("%s\n", name);
    
    memset(name2, 0x0, sizeof(name2));
    for( i = 0; i < cnt; i++)
    {
        name2[i] = name[cnt - 1 - i];
    }
    for(i = 0; i < strlen(name2); i++)
    {
        if(name2[i] == '.')
            break;
        *filename++ = name2[i];
    }
    return 0;
}


/*
  名  称: ExtractFileNameWithPostfix
  功  能: 提取不包含路径的文件名字符串(包含后缀名)
  输  入:
  输  出: 
  返回值:

 */
int ExtractFileNameWithPostfix(const char *src, char *filename)
{
    int     len, cnt, i;
    char     *off;
    char  name[NAME_MAX + 1];
    char  name2[NAME_MAX + 1];

    memset(name, 0x00, sizeof(name));
    
    if( !src || !filename) return -1;
        
    len = strlen(src);
    
    off = (char *)&src[len - 1];

    cnt = 0;
    while((off != &src[0]) && (*off != '/'))
    {
        name[cnt++] = *off--;
    }
    
    memset(name2, 0x0, sizeof(name2));

    for( i = 0; i < cnt; i++)
    {
        name2[i] = name[cnt - 1 - i];
    }

    for(i = 0; i < strlen(name2); i++)
    {
        //*filename++ = name2[i];
        filename[i] = name2[i];
    }

    return 0;
}

/*
  名  称: GetDiskUsage
  功  能: 获得磁盘的使用状况
  输  入:
          diskpath - 该磁盘在文件系统中的挂载点          
  输  出: 
          free  - 剩余的字节容量
          total - 总的字节容量
  返回值:

 */
inline BOOL GetDiskUsage(const char *diskpath, uint32 *free, uint32 *total)
{
    struct statfs info;
    
    if(!free || !total) return FALSE;
    
    if( statfs(diskpath, &info) < 0)
        return FALSE;
    
    *free     = info.f_bsize * info.f_bfree;
    *total    = info.f_bsize * info.f_blocks;
    
    return TRUE;
}
