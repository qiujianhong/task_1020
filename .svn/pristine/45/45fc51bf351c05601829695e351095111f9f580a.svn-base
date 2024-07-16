/*
*********************************************************************
* @filename: task_log.c
* @brief   : 进程/线程模板日志 不能依赖日志库 保证日志输出
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2020-6-30     create
*********************************************************************
*/
#include "baseCommon.h"
#include "task_log.h"

char            g_taskerr_log[256] = {0};
uint8           g_fTasklogInitOK  = 0;
pthread_mutex_t g_fTaskLogLock;                /*接收缓冲区互斥锁*/

/**********************************************************************
* @name      : task_log_init
* @brief     ：日志锁初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
void task_log_init(char *filename)
{
    char    err_log_bak[256] = {0};
    struct  stat sta;

    if(0 != g_fTasklogInitOK)
    {
        pthread_mutex_init(&g_fTaskLogLock, NULL);
        g_fTasklogInitOK = 1;
    }
    if(NULL != filename)
    {
        strcpy(g_taskerr_log, filename);

        /* 日志大小维护 */
        sprintf(err_log_bak, "%s.old", g_taskerr_log);

        if(0 == stat(filename, &sta))
        {
            if(S_ISREG(sta.st_mode)) //普通文仿
            {
                if(sta.st_size >= 1024 * 1024)
                {
                    if(0 != rename(filename, err_log_bak))
                    {
                        /* 重命名失败就删除 */
                        remove(filename);
                    }
                }
            }
        }
    }
}

/**********************************************************************
* @name      : task_get_filesize
* @brief     ：获取日志大小
* @param[in] ：char *filename  日志文件
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-3-12
* @Update    :
**********************************************************************/
ssize_t task_get_filesize(const char *filename)
{
    struct     stat sta;
    
    if( stat( filename, &sta) < 0)
    {
        return -1;
    }
    if( S_ISREG(sta.st_mode)) //普通文件
    {
        return sta.st_size;
    }
    else 
    {
        return -1;
    }
}

/**********************************************************************
* @name      : task_write_log
* @brief     ：写日志
* @param[in] ：char *fmt       日志内容
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-3-12
* @Update    :
**********************************************************************/
int TASK_ERROR_LOG(char *fmt, ...)
{
    int         fd;
    char        path[256];
    va_list     ap;
    char pathsub[8100];    
    struct timeval tv;
    struct tm     *stime = NULL;

    if(0 == g_taskerr_log[0] || NULL == fmt)
    {
        return -1;
    }

    pthread_mutex_lock(&g_fTaskLogLock);
    if(0 != access(g_taskerr_log, F_OK)) 
    {
        if( (fd = open(g_taskerr_log, O_WRONLY | O_CREAT | O_TRUNC)) < 0)
        {
            pthread_mutex_unlock(&g_fTaskLogLock);
            return -1;
        }
        fchmod(fd, 0664);
        close(fd);
    }

    if(task_get_filesize(g_taskerr_log) > 512000)
    {
        memset(path, 0x0,sizeof(path));
        strcat(path, g_taskerr_log);
        strcat(path, ".sub");
        remove(path);
        rename(g_taskerr_log, path);
    }
    
    if( (fd = open(g_taskerr_log, O_CREAT |O_WRONLY | O_APPEND)) < 0) //将内容附加到文件末尾
    {
        pthread_mutex_unlock(&g_fTaskLogLock);
        return -1;
    }
   
    gettimeofday(&tv, NULL);
    stime = localtime(&tv.tv_sec);

    /* 写入日期 */
    sprintf(pathsub, "[%04d-%02d-%02d %02d:%02d:%02d.%06ld] ",  stime->tm_year + 1900, stime->tm_mon + 1, 
                     stime->tm_mday, stime->tm_hour, stime->tm_min, stime->tm_sec, tv.tv_usec);
    write(fd, pathsub, strlen(pathsub));

    /* 写入日志字符串 */  
    va_start(ap, fmt);  
    vsnprintf(pathsub, sizeof(pathsub), fmt, ap); /* pathsub再次回收利用，用来保存写入的字符串 */
    va_end(ap);
    write(fd, pathsub, strlen(pathsub));

    pthread_mutex_unlock(&g_fTaskLogLock);
    close(fd);
    return 0;
}


