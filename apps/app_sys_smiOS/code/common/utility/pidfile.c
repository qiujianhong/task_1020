/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： pidfile.c
* @date： 2019-11-9
*********************************************************************
*/


#include "baseType.h"
#include "pidfile.h"
#define FALSE	0
#define TRUE	1

/**
*********************************************************************
* @brief： 文件锁
* @param[in]： 
               file_name   文件名起始地址
* @param[out]：
               ptr         文件描述符
* @return： 返回值说明      成功返回0  失败 -1
*********************************************************************
*/
sint32 i_lock_pid_file(sint32 *ptr, const sint8 *file_name)
{
    *ptr = open(file_name, O_CREAT | O_RDWR, 0666); // 打开文件
    sint32 rc = flock(*ptr, LOCK_EX | LOCK_NB);     // 锁住文件
    if (rc && EWOULDBLOCK == errno)                 // 检查加锁是否成功
        return -1;                                  // 其他实例运行则返回-1
    return 0;                                       // 获取刀文件锁返回0
}

/**
*********************************************************************
* @brief： 设置pid_file
* @param[in]： 
               file_name         文件路径
* @param[out]：
               none

* @return： 返回值说明 成功返回0  失败 -1
*********************************************************************
*/
sint32 i_set_pid_file(const sint8 *file_name)
{
    sint32 file_fd = 0;
    sint32 writeLen = 0;
    sint8 pid_str[10] = {0};
    sint32 pidLen = 0;
    //获取刀PID文件锁后将PID写入文件
    if (0 == i_lock_pid_file(&file_fd, file_name))
    {
        sprintf(pid_str, "%d", (sint32)getpid());
        pidLen = strlen(pid_str);
        writeLen = write(file_fd, pid_str,pidLen);
        if(writeLen == pidLen)
        {
            return 0;
        }
    }
    return -1;
}


/**
*********************************************************************
* @brief：      单实例检查
* @param[in]： appName
* @param[out]：NA
* @return：     =TRUE  检查通过，运行单个实例     ，=FALSE 检查不通过，已经有实例运行
*********************************************************************
*/
Bool singleton_check(char *appName)
{
    int ret = 0;
    int fd = 0;
    struct flock flockstr;
    pid_t pid = 0;
    char lockFile[128] = {0};

    pid = getpid();
    flockstr.l_type = F_WRLCK;
    flockstr.l_start = 0;
    flockstr.l_whence = SEEK_SET;
    flockstr.l_len = 0;
    flockstr.l_pid = pid;

    sprintf(lockFile, "/var/run/%s.lock", appName);
    
    fd = open(lockFile,O_WRONLY | O_CREAT,0660); 
    if (fd < 0)
    {
        printf("open %s failed.\n", lockFile);
        return FALSE;
    }

    ret = fcntl(fd,F_SETLK,&flockstr);
    if (ret < 0)
    {
        fcntl(fd,F_GETLK,&flockstr);

        printf("the %s app is running now! and it's pid is %d\n", appName, flockstr.l_pid);
        return FALSE;
    }
    
    write(fd,(char *)&pid,sizeof(pid_t));

    return TRUE;    
}