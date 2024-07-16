/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   APP升级模块
* @date：    2019-10-31
* @history： 
*********************************************************************
*/

#include "common.h"
#include "framecomm.h"
#include "msg_id.h"
#include "task_id.h"
#include "appUpdate.h"
#include "oopVarUpdate.h"
#include "appcomm.h"

#if AREA_FUJIAN

//升级成功标记
uint8                gSucflag = 0;
APP_UPDATE_STATUS_E  gUpdateFlag = UP_IDLE;



/**********************************************************************
* @name      : update_open_ppd_channel
* @brief     ：打开ftp命令通道
* @param[in] ：ppd      - ftp命令通道
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-23
* @Update    : 2020-6-1
**********************************************************************/
int update_open_ppd_channel(FILE **ppd)
{
    FILE *pd = NULL;
    pd = popen("/mnt/extapps/ftp -n -v > /mnt/data/app/puAmr/log/ftp.log" , "w" );
    if (pd == NULL)
    {
        return ERR_ACCESS;
    }

    *ppd = pd;
    
    return ERR_OK;
}

/**********************************************************************
* @name      : update_open_fpd_channel
* @brief     ：打开ftp信息通道
* @param[in] ：ppd      - ftp命令通道
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-23
* @Update    : 2020-6-1
**********************************************************************/
int update_open_fpd_channel(FILE **fpd)
{
    FILE *pd = NULL;
    
    pd = popen("tail -f /mnt/data/app/puAmr/log/ftp.log" , "r" );
    if (pd == NULL)
    {
        return ERR_ACCESS;
    }
    
    *fpd = pd;
    
    return ERR_OK;
}


/**********************************************************************
* @name      : get_update_status
* @brief     ：获取升级状态
* @param[in] ：
* @param[out]：sucflag 1 - 成功 0 - 失败
* @return    ：
* @Create    : 
* @Date      ：2019-12-23
* @Update    : 2020-6-1
**********************************************************************/
void app_update_status_get(uint8 *sucflag)
{
    uint32 j = 0;
    char   acBuf[1024] = {0};
    FILE  *pfd = NULL;
    int    ret = 0;
    char  *pfile = NULL;
    uint8  size[128] = {0};
    uint8  progress[128] = {0};
    char   str[128] = {0};
    
    while (1)
    {
        usleep(10000);
        
        if (gUpdateFlag != UP_RUN || *sucflag == 1)
        {
            UPDATE_FMT_DEBUG("获取升级状态结束 gUpdateFlag[%d] sucflag[%d]\n", gUpdateFlag, *sucflag);
            break;
        }
            
        /*打开信息输出文件*/
        if (pfd == NULL)
        {
            ret = update_open_fpd_channel(&pfd);
            if (ret != ERR_OK)
            {
                UPDATE_FMT_DEBUG("打开信息输出文件失败!\n");
                continue;
            }
        }

        ret = ERR_NORMAL;
        while (fgets(acBuf, sizeof(acBuf), pfd) != NULL)
        {
            if ((pfile = strstr(acBuf, "213 ")) != NULL)
            {
                memcpy(size, pfile + strlen("213 "), 20);
                for (j = 0; j < 20; j ++)
                {
                    if (size[j] == '\n')
                    {
                        size[j] = '\0';
                        break;
                    }
                }
                UPDATE_FMT_DEBUG("文件大小%s\n", size);
                sprintf(str, "远程传输文件大小%s字节", size);
                appmain_refresh_status_bar(30, str);
            }
            else if ((pfile = strstr(acBuf, "Bytes transferred: ")) != NULL)
            {
                memcpy(progress, pfile + strlen("Bytes transferred: "), 20);
                for (j = 0; j < 20; j ++)
                {
                    if ((progress[j] == '\n') || (progress[j] == '\r'))
                    {
                        progress[j] = '\0';
                        break;
                    }
                }
                UPDATE_FMT_DEBUG("升级进度:%s\\%s\r", progress, size);
                sprintf(str, "进度:%s\\%s", progress, size);
                appmain_refresh_status_bar(30, str);
            }
            /*else if ((pfile = strstr(acBuf, "226 Successfully transferred")) != NULL)
            {
                UPDATE_FMT_DEBUG("%s", acBuf);
                UPDATE_FMT_DEBUG("升级成功\n");
                sprintf(str, "远程传输文件成功");
                appmain_refresh_status_bar(60, str);
                *sucflag = 1;
                ret = ERR_OK;
            }*/
            else if ((pfile = strstr(acBuf, "226 Transfer ok")) != NULL)
            {
                UPDATE_FMT_DEBUG("%s", acBuf);
                UPDATE_FMT_DEBUG("升级成功\n");
                sprintf(str, "远程传输文件成功");
                appmain_refresh_status_bar(60, str);
                *sucflag = 1;
                ret = ERR_OK;
            }
            else if ((pfile = strstr(acBuf, "226 ")) != NULL)
            {
                UPDATE_FMT_DEBUG("%s", acBuf);
                UPDATE_FMT_DEBUG("升级成功\n");
                sprintf(str, "远程传输文件成功");
                appmain_refresh_status_bar(60, str);
                *sucflag = 1;
                ret = ERR_OK;
            }
            
            MEMZERO(acBuf, sizeof(acBuf));
            MEMZERO(progress, sizeof(progress));

            if (ret == ERR_OK)
            {
                break;
            }
        }

        if (pfd != NULL)
        {
            fclose(pfd);
            pfd = NULL;
        }
    }

    pthread_exit(NULL);
    return;
}

/**********************************************************************
* @name      : app_update_status
* @brief     ：捕获升级状态
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-23
* @Update    : 2020-6-1
**********************************************************************/
void* app_update_status(void* arg)
{
    while (gUpdateFlag != UP_RUN)
    {
        usleep(10000);
    }

    UPDATE_FMT_DEBUG("开始获取升级状态\n");
    app_update_status_get(&gSucflag);
    return NULL;
}

/**********************************************************************
* @name      : app_update_entry
* @brief     ：APP升级入口(目前主要升级FTP)
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-23
* @Update    : 2020-6-1
**********************************************************************/
int app_update_entry(uint32 dbip, uint16 dbport)  
{
    int32  ret = ERR_OK;
    FILE  *ppd = NULL;
    char   acBuf[1024] = {0};
    char   cmd[1024] = {0};
    OOP_FTPSERVER_T serverInfo;
    OOPM_FTP_T      ftpInfo;
    
    memset(acBuf, 0x00, sizeof(acBuf));
    memset(&serverInfo, 0x00, sizeof(serverInfo));
    memset(&ftpInfo, 0x00, sizeof(ftpInfo));

    uint8  updateCnt = 0;

    pthread_t pthread = 0;
    
    while (1)
    {
        if (!app_get_ftp_status())
        {
            sleep(1);
            continue;
        }

        UPDATE_FMT_DEBUG("开始升级\n");
        gSucflag = 0;
        gUpdateFlag = UP_START;

        ret = app_get_server_info(&serverInfo);
        if (ret != ERR_OK)
        {
            UPDATE_FMT_DEBUG("获取服务器信息失败!\n");
            app_clr_ftp_status();
            gUpdateFlag = UP_IDLE;
            continue;
        }

        ret = app_get_ftp_info(&ftpInfo);
        if (ret != ERR_OK)
        {
            UPDATE_FMT_DEBUG("获取升级信息失败!\n");
            app_clr_ftp_status();\
            gUpdateFlag = UP_IDLE;
            continue;
        }
        
        /*打开FTP传输管道*/
        if (ppd == NULL)
        {
            ret = update_open_ppd_channel(&ppd);
            if (ret != ERR_OK)
            {
                UPDATE_FMT_DEBUG("打开FTP传输管道失败!\n");
                app_clr_ftp_status();
                gUpdateFlag = UP_IDLE;
                continue;
            }
        }

        ret = pthread_create(&pthread, NULL, app_update_status, NULL);
        if (ret != 0)
        {
            app_clr_ftp_status();
            UPDATE_FMT_DEBUG("获取升级状态线程创建失败\n"); 
            gUpdateFlag = UP_IDLE;
            continue;
        }

        UPDATE_FMT_DEBUG("升级服务器IP:%d.%d.%d.%d\n", serverInfo.ip.data[0], serverInfo.ip.data[1], serverInfo.ip.data[2], 
            serverInfo.ip.data[3]);
        UPDATE_FMT_DEBUG("服务器端口  :%d\n", serverInfo.port);
        UPDATE_FMT_DEBUG("用户名      :%s\n", serverInfo.user.value);
        UPDATE_FMT_DEBUG("密码        :%s\n", serverInfo.pwd.value);
        UPDATE_FMT_DEBUG("升级源文件  :%s\n", ftpInfo.file.srceFile.value);
        UPDATE_FMT_DEBUG("升级目的文件:%s\n", ftpInfo.file.destFile.value);

        fprintf(ppd, "open %d.%d.%d.%d %d\n", serverInfo.ip.data[0], serverInfo.ip.data[1], serverInfo.ip.data[2], 
            serverInfo.ip.data[3], serverInfo.port);
        fprintf(ppd, "user %s %s\n", serverInfo.user.value, serverInfo.pwd.value); 
        fprintf(ppd, "binary\n"); 
        fprintf(ppd, "tick\n");
        fprintf(ppd, "prompt\n");
        fprintf(ppd, "status\n");
        fprintf(ppd, "size %s\n", ftpInfo.file.srceFile.value);
        fprintf(ppd, "get %s %s\n", ftpInfo.file.srceFile.value, ftpInfo.file.destFile.value);
        fprintf(ppd, "bye\n" ); 

        //测试发现，到关句柄时上面的指令才正常触发执行，直到所有命令运行结束后，才能正常关闭句柄，否则阻塞。
        //所以需要另外创建线程捕获ftp命令回码
        gUpdateFlag = UP_RUN;
        if (ppd != NULL)
        {
            fclose(ppd);
            ppd = NULL;
        }

        sleep(2);

        if (gSucflag == 1)
        {
            app_clr_ftp_status();
            updateCnt = 0;
            if ((strstr(ftpInfo.file.destFile.value, "update_") != NULL) &&
                (strstr(ftpInfo.file.destFile.value, ".bin") != NULL))
            {
                UPDATE_FMT_DEBUG("发现差分升级包，进入后台升级，查看diff.log日志\n"); 
                sprintf(cmd, "cp %s /data/app/tmpdiff.bin", ftpInfo.file.destFile.value);
                UPDATE_FMT_DEBUG("%s\n", cmd);
                system(cmd);
                MEMZERO(cmd, sizeof(cmd));
                sleep(5);
                sprintf(cmd, "chmod 777 %s", "/data/app/tmpdiff.bin");
                UPDATE_FMT_DEBUG("%s\n", cmd);
                system(cmd);
                MEMZERO(cmd, sizeof(cmd));
                sleep(1);
                sprintf(cmd, "cd /data/app && ./tmpdiff.bin > /data/app/diff.log &");
                UPDATE_FMT_DEBUG("%s\n", cmd);
                system(cmd);
            }else if(strstr(ftpInfo.file.destFile.value, "router_file") != NULL)
            {
                UPDATE_FMT_DEBUG("发现载波主节点升级报文\n"); 
                sprintf(cmd,"cp %s /mnt/data/app/ccoRouter/router_update_file_1", ftpInfo.file.destFile.value);
                UPDATE_FMT_DEBUG("%s\n", cmd);
                system(cmd);
            }else if(strstr(ftpInfo.file.destFile.value, "node_file") != NULL)
            {
                UPDATE_FMT_DEBUG("发现载波子节点升级报文\n"); 
                sprintf(cmd,"cp %s /mnt/data/app/ccoRouter/sub_router_update_file_1", ftpInfo.file.destFile.value);
                UPDATE_FMT_DEBUG("%s\n", cmd);
                system(cmd);
            }
           
        }
        else
        {
           UPDATE_FMT_DEBUG("升级失败，重新升级\n"); 
           updateCnt ++;
           if (updateCnt > 0)
           {
               UPDATE_FMT_DEBUG("达到最大重试次数\n");
               app_clr_ftp_status();
               updateCnt = 0;
           }
        }

        gUpdateFlag = UP_END;
    }

    return 0;  
}
#endif

