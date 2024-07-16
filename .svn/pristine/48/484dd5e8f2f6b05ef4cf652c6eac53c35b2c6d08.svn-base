/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级APP用采进程
* @date：    2019-12-23
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"

#define     BACKUP_COUNT    (10)  //允许保存的压缩包数量
#define     LOG_PATH_MAX    (255)  //维护的最大日志目录数

/*临时版本，路径直接代码中写死*/
char * g_logRoot = "/data/app";

char g_logPath[LOG_PATH_MAX][256] = {{0}};

int g_logPathNum = 0;

/**
*********************************************************************
* @brief:       将.log.sub文件打包
* @param[in]：   filepath 文件路径
* @param[in]:   storeCnt 压缩保存的份数
* @return：      void
*********************************************************************
*/
int diag_log_tar(const char *filepath, int storeCnt)
{
    int         i;
    char        tar_path[PATH_MAX_LEN + 1];         //压缩包路径
    char        tar_cmd[PATH_MAX_LEN + 1];          //压缩包命令
    char        rename_cmd1[PATH_MAX_LEN + 1];      //更换前的文件名
    char        rename_cmd2[PATH_MAX_LEN + 1];      //更换后的文件名

    char        bFilePath[PATH_MAX_LEN];            //文件路径（不含文件名）
    char        bFileName[NAME_MAX];                //文件名，不含扩展名
    char        bFileNameWithPostfix[NAME_MAX];     //文件名，含扩展名
    char        bDirPath[PATH_MAX_LEN];             //文件路径（不含文件名）

    if (storeCnt > BACKUP_COUNT)
    {
        storeCnt = BACKUP_COUNT;
    }

    if (storeCnt < 1)
    {
        return 0;
    }

    memset(tar_cmd, 0x0, sizeof(tar_cmd));
    memset(bFilePath, 0x0, sizeof(bFilePath));
    memset(bFileName, 0x0, sizeof(bFileName));
    memset(bFileNameWithPostfix, 0x0, sizeof(bFileNameWithPostfix));
    memset(bDirPath, 0x0, sizeof(bDirPath));

    file_path_extract(filepath, bFilePath);                      //文件路径（不含文件名）
    file_name_extract(filepath, FALSE, bFileName);               //文件名，不含扩展名
    file_name_extract(filepath, TRUE, bFileNameWithPostfix);     //文件名，含扩展名
    
    //在当前路径建立以日志文件为名的临时日志目录
    sprintf(bDirPath, "%s%slog", bFilePath, bFileName);
    
    if(!path_exist(bDirPath))
    {
        path_dir_make(bDirPath);
    }

    //将日志文件移动到目录中
    sprintf(tar_cmd, "mv -f %s %s", filepath, bDirPath);

    system(tar_cmd);

    memset(tar_cmd, 0x0, sizeof(tar_cmd));


    for(i = 0; i < storeCnt; i++)       //确认能够压缩的压缩包文件
    {
        memset(tar_path, 0x0, sizeof(tar_path));
        sprintf(tar_path, "%s/%s_%04d.tar.gz", bDirPath, bFileNameWithPostfix, i);
        
        if( !file_exist(tar_path))      //文件不存在，则可以压缩
        {
            // 不要包含目录
            memset(tar_path, 0x0, sizeof(tar_path));
            sprintf(tar_path, "%s_%04d.tar.gz", bFileNameWithPostfix, i);
            sprintf(tar_cmd, "cd %s; tar -cvzf %s %s", bDirPath, tar_path, bFileNameWithPostfix);
            system(tar_cmd);
            memset(tar_path, 0x0, sizeof(tar_path));
            sprintf(tar_path, "%s/%s", bDirPath, bFileNameWithPostfix);
            remove(tar_path);
            return 1;
        }
    }

    //执行到这里，表示没有空闲的压缩包，所以需要将最旧的删除，并且将其他的挨个改名
    memset(tar_path, 0x0, sizeof(tar_path));
    sprintf(tar_path, "%s/%s_%04d.tar.gz", bDirPath, bFileNameWithPostfix, 0);
    remove(tar_path);       //删除第一个压缩包

    for(i = 1; i < storeCnt; i++) 
    {
        memset(rename_cmd1, 0x0, sizeof(rename_cmd1));
        memset(rename_cmd2, 0x0, sizeof(rename_cmd2));
        sprintf(rename_cmd1, "%s/%s_%04d.tar.gz", bDirPath, bFileNameWithPostfix, i);           //改名前
        sprintf(rename_cmd2, "%s/%s_%04d.tar.gz", bDirPath, bFileNameWithPostfix, i - 1);       //改名后
        rename(rename_cmd1, rename_cmd2);
    }

    memset(tar_path, 0x0, sizeof(tar_path));
    sprintf(tar_path, "%s_%04d.tar.gz", bFileNameWithPostfix, storeCnt - 1);
    sprintf(tar_cmd, "cd %s; tar -cvzf %s %s", bDirPath, tar_path, bFileNameWithPostfix);
    system(tar_cmd);        //压缩日志文件
    memset(tar_path, 0x0, sizeof(tar_path));
    sprintf(tar_path, "%s/%s", bDirPath, bFileNameWithPostfix);
    remove(tar_path);    
    
    return 1;
}

/**
*********************************************************************
* @brief:       将.log.sub文件打包
* @param[in]：   NA 
* @param[out]:  NA
* @return：      void
*********************************************************************
*/
void diag_log_backup(void)
{
    int i = 0;
    DIR           *dirp;
    struct dirent *entp;
    char subpath[PATH_MAX_LEN + 1];
    char subfix[128] = {0};
        
    for(i = 0; i < g_logPathNum; i++)
    {
        //打开目录
        if ((dirp = opendir(g_logPath[i])) == NULL)
        {
            continue;
        }
        
        while ((entp = readdir(dirp)) != NULL)
        {
            memset(subpath, 0, sizeof(subpath));
            memset(subfix, 0, sizeof(subfix));

            if((strcmp(entp->d_name, ".") == 0) || (strcmp(entp->d_name, "..") == 0))
            {
                continue;
            }
            
            sprintf(subpath, "%s/%s", g_logPath[i], entp->d_name);

            //获取文件后缀
            file_extension_extract(entp->d_name, subfix);

            //打包后缀不为.log的文件
            if((strlen(subfix) != 0) && (strcmp(subfix, "log") != 0))
            {
                diag_log_tar(subpath, 1500);
                sleep(1);               
            }
        }

        closedir(dirp);
    }    

    return;
}


/*-----------------------------Main-------------------------------*/
int main(int argc, char** argv)
{
    //static struct timespec   lstTime;
    //struct timespec curTime;
    DIR           *dirp;
    struct dirent *entp;
    struct stat fileinfo = {0};
    char logpath[512] = {0};

    //clock_gettime(CLOCK_MONOTONIC, &lstTime);

    /* 检查程序只启动了一个副本 */
    if(!singleton_check("sysMaintain"))
    {
        return -1;
    }
    
    //打开目录
    if ((dirp = opendir(g_logRoot)) != NULL)
    {
        while ((entp = readdir(dirp)) != NULL)
        {
            if((strcmp(entp->d_name, ".") == 0) || (strcmp(entp->d_name, "..") == 0))
            {
                continue;
            }

            sprintf(logpath, "%s/%s/log", g_logRoot, entp->d_name);

            stat(logpath, &fileinfo);
            
            if(S_ISDIR(fileinfo.st_mode) && (g_logPathNum < LOG_PATH_MAX))
            {
                strcpy(g_logPath[g_logPathNum++], logpath);
            }
        }  

        closedir(dirp);
    }
    
    while(1)
    {
        //clock_gettime(CLOCK_MONOTONIC, &curTime);
        
        // 10s检测一次日志文件,查看是否有.log.sub的文件需要压缩
        //if ((abs(curTime.tv_sec - lstTime.tv_sec) >= 10) || (lstTime.tv_sec == 0))
        {
            diag_log_backup();
            
            //lstTime = curTime;
        }
        
        sleep(1);
    }
    
    return 0;
}
