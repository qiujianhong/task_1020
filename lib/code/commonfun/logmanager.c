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
#include "iniparser.h"

DIAG_INFO_T g_logModule[] = 
{
    {"STAT"    ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //0
    {"RPT"     ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //1
    {"PRTL"    ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //2
    {"EVENT"   ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //3
    {"COMM"    ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //4   
    {"PULSE"   ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //5   
    {"APP698"  ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //6
    {"YCDB"    ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //7   
    {"YCMSG"   ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //8   
    {"GUI"     ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //9
    {"PDDB"    ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //10   
    {"PDMSG"   ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //11  
    {"APP104"  ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //12
    {"UPDATE"  ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //13
    {"ACCOMM"  ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //14 交采串口收发报文
    {"ACDB"    ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //15 交采读写数据中心
    {"TESAM"   ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //16 ESAM交互信息
    {"FRAME104",  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //17 配网104与主站之间的交互
    {"DB104"   ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //18 配网104与数据中心之间的交互
    {"CCOUP"   ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //19 本地通信app与消息中心的交互
    {"CCODOWN" ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //20 本地通信app与载波模块的交互
    {"CTRL"    ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //21 控制
    {"ACUDP"   ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //22 交采UDP服务
    {"ACMQTT"  ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //23 交采MQTT通信
    {"CCOGUI"  ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //24 本地通信app显示相关日志
    {"RSP"     ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //25 遥信脉冲app与mqtt消息日志
    {"RSPDOWN" ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //26 目前不用
    {"TASK"  ,    0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //27 采集任务调度管理APP
    {"MDLMGR"  ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //28 模组管理APP
    {"MDLMSG"  ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //29 模组管理APP
    {"MDLUP"  ,   0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //30 模组管理APP
    {"MSG"     ,  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //31 APP之间的MQTT消息
    {"WIRELESS",  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //32 无线拨号的日志
    {"AC485COM",  0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //33 交采app与485模块串口通信
    {"LCMONITOR", 0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //34 回路监控日志
    {"TMCHK",     0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //35 自动对时日志
    {"MSGAC",     0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //36 交采通讯
    {"SELFCHK",   0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //37 自检测
    {"PDS",       0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //38 台区状态管理日志
    {"PDSDB",     0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //39 台区状态管理数据日志
    {"AUTH",      0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //40 安全在线监测
    {"ACMSG",     0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //41 698交采模块通信相关
    {"STORE",     0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //42 存储   
    {"INIT",      0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //43 初始化 
    {"ACSAMP",    0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //44 交采数据采集
    {"SMIOS",     0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //45 系统管理器
    {"LOGIN",     0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //46 登录
    {"HEART",     0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //47 心跳
    {"SECMODULE", 0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //48 安全软算法
    {"BLE",       0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //49 蓝牙
    {"UART",      0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //50 串口
    {"USB",       0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //51 USB 
    {"SEC",       0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //52 SEC
	{"IOT",       0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //53 IOT	
    {"TRANS",     0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //54 TRANS
	{"FREEZE",     0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //55 统计冻结
    {"RADIO",     0,  LOG_MAX_SIZE,  LOG_SAVE_NUM}, //56 电台
};

uint32 g_logModuleSize = sizeof(g_logModule)/sizeof(DIAG_INFO_T);

//合并日志文件信息
DIAG_INFO_T g_logMergeInfo = {0};

//配置文件路径，由每个APP初始化
char g_ini_file[256] = {0};
//APP日志名称，合并日志的名称
char g_log_name[256] = {0};

DIAG_WARNING_T g_emerge_status = {0, 0, 1, 0}; //紧急状态，需同步日志到非易失性存储介质
char g_logpath[PATH_MAX_LEN+1] = {"/data/app/log"};
char g_tmppath[PATH_MAX_LEN+1] = {"/tmp/data/app/log"};
uint8 g_urgent_deal = 0;
uint8 g_recover_deal = 0;

#ifdef AREA_FUJIAN 
char g_record_substr[][20] = {"/comm.log","ctsrec","cco_down"};
uint8 g_record_substr_num = sizeof(g_record_substr)/sizeof(*g_record_substr);
char* g_record_path = "/data/app/record";
uint8 g_delete_day = 10;    //删除多少天前的文件
uint8 g_record_flag;    //标记要不要按照时间来备份文件 从配置文件中读取 0表示不按时间片备份 1表示按照时间片备份
#endif

/**********************************************************************
* @name      : diag_sig_handler
* @brief     ：信号处理
* @param[in] ：signo 信号  
* @return    ：无
* @Create    : 贺宁
* @Date      ：2021-06-29
* @Update    :
**********************************************************************/
void diag_sig_handler(int signo)
{
    char  cmd[PATH_MAX_LEN + 1];

    if(TRUE == g_emerge_status.bData)
    {
        return;
    }
    
    g_emerge_status.warHalt = TRUE;

    sprintf(cmd, "cp -f %s/*.log %s", g_tmppath, g_logpath);
    system(cmd);

    //sprintf(cmd, "ln -sf %s/*.log %s", g_logpath, g_tmppath);
    //system(cmd);
    
    g_emerge_status.warHalt = FALSE;
}

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
//    int i = 0;
    DIR           *dirp;
    struct dirent *entp;
    char subpath[PATH_MAX_LEN + 1];
    char subfix[128] = {0};
        
//    for(i = 0; i < g_logPathNum; i++)
//    {
        //打开目录
        if ((dirp = opendir(g_logpath)) == NULL)
        {
            return;
        }
        
        while ((entp = readdir(dirp)) != NULL)
        {
            memset(subpath, 0, sizeof(subpath));
            memset(subfix, 0, sizeof(subfix));

            if((strcmp(entp->d_name, ".") == 0) || (strcmp(entp->d_name, "..") == 0))
            {
                continue;
            }

            //获取文件后缀
            file_extension_extract(entp->d_name, subfix);
            sprintf(subpath, "%s/%s", g_logpath, entp->d_name);

#ifdef AREA_FUJIAN
            //将特殊后缀的日志拷到指定目录下
            char cmd[1000] = {0};
            if(strcmp("record",subfix)==0)
            {
                if(!path_exist(g_record_path))
                {
                    path_dir_make(g_record_path);
                }
                sprintf(cmd,"mv -f %s %s",subpath,g_record_path);
                system(cmd);
                printf("日志转存 %s \n",cmd);
                continue;
            }
#endif

            //当前版本删除后缀非0的文件
            if(1 == strlen(subfix) && ('1' <= subfix[0] && subfix[0] <= '9'))
            {
                sleep(1);
                remove(subpath);
                continue;
            }
            
            //打包后缀不为.log的文件
            if((strlen(subfix) != 0) && (strcmp(subfix, "log") != 0))
            {
                diag_log_tar(subpath, LOG_PACK_MAX);
                sleep(1);
            }
        }

        closedir(dirp);
//    }    

    return;
}

/*-----------------------------Main-------------------------------*/
void *log_manager_entry(void *arg)
{
    //static struct timespec   lstTime;
    //struct timespec curTime;
    int state = 1;
    char  cmd[PATH_MAX_LEN + 1];
    
    //clock_gettime(CLOCK_MONOTONIC, &lstTime);
#ifdef AREA_FUJIAN
    static struct timespec   lstTime;
    struct timespec curTime;
    clock_gettime(CLOCK_MONOTONIC, &lstTime);

#endif

    signal(SIGTERM, diag_sig_handler);
    
    while(1)
    {
        //clock_gettime(CLOCK_MONOTONIC, &curTime);

        //检测电源状态
        //diag_power_state(&state);

        if(state == 0) //掉电
        {
            g_emerge_status.warPower = TRUE;
            g_recover_deal = FALSE;
        }
        else //上电
        {
            g_emerge_status.warPower = FALSE;
            g_urgent_deal = FALSE;
        }

        //进入紧急状态，将日志从临时区同步到备份区
        if((g_emerge_status.warHalt == TRUE)||(g_emerge_status.warPower == TRUE))
        {
            if(g_urgent_deal== FALSE && g_emerge_status.bData == FALSE)
            {
                g_urgent_deal = TRUE;
                sprintf(cmd, "cp -f %s/*.log %s", g_tmppath, g_logpath);
                system(cmd);

                //sprintf(cmd, "ln -sf %s/*.log %s", g_logpath, g_tmppath);
                //system(cmd);
            }
        }
        
        // 10s检测一次日志文件,查看是否有.log.sub的文件需要压缩
        //if ((abs(curTime.tv_sec - lstTime.tv_sec) >= 10) || (lstTime.tv_sec == 0))
        //{
            diag_log_backup();
            
        //    lstTime = curTime;
        //}
#ifdef AREA_FUJIAN
        
        
        if(g_record_flag == 1)
        {
            clock_gettime(CLOCK_MONOTONIC, &curTime);
            char            cmd[1000] =
            {
                0
            };
        
            //一个小时检查一次/mnt/data/app/record目录下是否需要删除老文件
            if(((abs(curTime.tv_sec - lstTime.tv_sec) >= 3600) || (lstTime.tv_sec == 0)))
            {
                sprintf(cmd, " find %s -mtime +%d -exec rm -rf {} \\;", g_record_path, g_delete_day);
                system(cmd);
                printf("检查老备份文件 %s \n", cmd);
                lstTime = curTime;
            }
        }
        
#endif
       
        sleep(1);
    }
    
    return 0;
}

#if DESC("写日志函数",1)

#ifdef AREA_FUJIAN
/**
*********************************************************************
* @brief:       检查日志是否需要特别处理 福建的把通信日志每小时压缩一次 上行698通信 ccoRouter的3762日志 485抄表日志
* @param[in]：   filepath    日志文件
* @param[in]：  
* @param[in]：  
* @param[out]:  NA
* @author:      
* @return：      
*********************************************************************
*/
BOOL diag_need_report_check(char* filepath)
{
    uint8 i;
    for(i=0;i<g_record_substr_num;i++)
    {
        if(strstr(filepath,g_record_substr[i])!=NULL)
            return TRUE;
    }
    return FALSE;
}

#endif

/**
*********************************************************************
* @brief:       打开日志
* @param[in]：   logfile    日志文件
* @param[in]：   symfile    链接文件
* @param[in]：   log        日志内容
* @param[out]:  NA
* @author:      贺宁
* @return：      void
*********************************************************************
*/
int diag_log_open(const char *tmpfile, const char *logfile, DIAG_TYPE_E diagtype, int modid, int *fd, int *tmpfd)
{
    int packedid = 0;
    char pathseg[PATH_MAX_LEN+1] = {"\0"}; 
    char rename1[PATH_MAX_LEN + 1];       //更换前的文件名
    char rename2[PATH_MAX_LEN + 1];       //更换后的文件名
    char tmppath[PATH_MAX_LEN + 1];          //文件路径
    char logpath[PATH_MAX_LEN + 1];          //文件路径
    char cmd[PATH_MAX_LEN + 1];
    char chkfile[PATH_MAX_LEN+1] = {0};        
#ifdef AREA_FUJIAN
    static int lasthour = -1;
    OOP_DATETIME_S_T timeNow;
#endif
    
    if(!file_exist(tmpfile) && FALSE == g_emerge_status.bData) 
    {
        file_path_extract(tmpfile, tmppath);
        
        if( !path_exist(tmppath)) 
        {
            path_dir_make(tmppath);
        }
    }

    if(!file_exist(logfile)) 
    {
        file_path_extract(logfile, logpath);
        
        if( !path_exist(logpath)) 
        {
            path_dir_make(logpath);
        }
    }

    //紧急状态，只写备份区，退出紧急状态后会同步到临时区
    if((g_emerge_status.warHalt == TRUE) || (g_emerge_status.warPower == TRUE) || TRUE == g_emerge_status.bData)
    {
        if(g_urgent_deal == FALSE && FALSE == g_emerge_status.bData)
        {
            g_urgent_deal = TRUE;
            //rename(tmpfile, logfile);
            sprintf(cmd, "mv -f %s %s", tmpfile, logfile);
            system(cmd);            
            //symlink(logfile, tmpfile);
        }
        
        strcpy_s(chkfile, 128, logfile);
    }
    else
    {
        if(g_recover_deal == FALSE && FALSE == g_emerge_status.bData)
        {
            g_recover_deal = TRUE;
            sprintf(cmd, "rm -f %s/*.log", g_tmppath);
            system(cmd);
            
            sprintf(cmd, "cp -fu %s/*.log %s", g_logpath, g_tmppath);
            system(cmd);
        }
            
        strcpy_s(chkfile, 128, tmpfile);
    }

    if(DIAG_IS_MERGE(g_logModule[modid].bitCtrl))
    {
        if(file_size_get(chkfile) > g_logMergeInfo.sizeLimit)
        {    
            for(packedid = 0; packedid < g_logMergeInfo.packedNum; packedid++)
            {
                sprintf(pathseg, "%s.%d", logfile, packedid);
        
                if(!file_exist(pathseg))
                {
//                    rename(chkfile, pathseg);
                    memset(cmd, 0, sizeof(cmd));
                    sprintf(cmd, "mv -f %s %s", chkfile, pathseg);
                    system(cmd);
                    break;
                }
            }

            //分片日志个数已经达到上限，删除第一个分片，然后依次改名
            if(packedid == g_logMergeInfo.packedNum)
            {
                for(packedid = 1; packedid < g_logMergeInfo.packedNum; packedid++)
                {
                    memset_s(rename1, sizeof(rename1), 0x0, sizeof(rename1));
                    memset_s(rename2, sizeof(rename2), 0x0, sizeof(rename2));
                    sprintf(rename1, "%s.%d", logfile, packedid);           //改名前
                    sprintf(rename2, "%s.%d", logfile, packedid - 1);       //改名后
                    rename(rename1, rename2);                
                }
                
                sprintf(pathseg, "%s.%d", logfile, packedid-1);
                //rename(chkfile, pathseg);
                memset(cmd, 0, sizeof(cmd));
                sprintf(cmd, "mv -f %s %s", chkfile, pathseg);
                system(cmd);
            }
        }
    }
    else
    {
#ifdef AREA_FUJIAN
        if(g_record_flag==1 && diag_need_report_check(chkfile))
        {
            //每小时重命名一个
            DT_Time2DateTimeShort(time(NULL),&timeNow);
            if(lasthour != -1 && timeNow.hour != lasthour)
            {
                memset(pathseg,0x00,sizeof(pathseg));
                //重命名一个特殊的后缀
                sprintf(pathseg,"%s.%02d%02d%02d%02d.record",chkfile,timeNow.year-2000,timeNow.month,timeNow.day,timeNow.hour);
                memset(cmd,0,sizeof(cmd));
                sprintf(cmd,"mv -f %s %s",chkfile,pathseg);
                system(cmd);
                printf("日志转存 %s\n",cmd);
            }
            lasthour = timeNow.hour;
        }else
        {
#endif
        if(file_size_get(chkfile) > g_logModule[modid].sizeLimit)
        {
            for(packedid = 0; packedid < g_logModule[modid].packedNum; packedid++)
            {
                sprintf(pathseg, "%s.%d", logfile, packedid);

                if(!file_exist(pathseg))
                {
//                    rename(chkfile, pathseg);
                    memset(cmd, 0, sizeof(cmd));
                    sprintf(cmd, "mv -f %s %s", chkfile, pathseg);
                    system(cmd);
                    break;
                }
            }

            //分片日志个数已经达到上限，删除第一个分片，然后依次改名
            if(packedid == g_logModule[modid].packedNum)
            {
                for(packedid = 1; packedid < g_logModule[modid].packedNum; packedid++)
                {
                    memset_s(rename1, sizeof(rename1), 0x0, sizeof(rename1));
                    memset_s(rename2, sizeof(rename2), 0x0, sizeof(rename2));
                    sprintf(rename1, "%s.%d", logfile, packedid);           //改名前
                    sprintf(rename2, "%s.%d", logfile, packedid-1);       //改名后
                    rename(rename1, rename2);                
                }
                
                sprintf(pathseg, "%s.%d", logfile, packedid-1);
                //rename(chkfile, pathseg);
                memset(cmd, 0, sizeof(cmd));
                sprintf(cmd, "mv -f %s %s", chkfile, pathseg);
                system(cmd);                
            }
        }
#ifdef AREA_FUJIAN
        }
#endif
    }

    //紧急状态，只写备份区，退出紧急状态后会同步到临时区
    if((g_emerge_status.warHalt == TRUE) || (g_emerge_status.warPower == TRUE) || TRUE == g_emerge_status.bData)
    {        
        if( (*fd = open(logfile, O_CREAT |O_WRONLY | O_APPEND, 0664)) < 0) //将内容附加到文件末尾
        {
            return -1;
        }
    }
    else
    {
        //关键日志，临时区和备份区都写
        if(diagtype == DIAG_EMERG)
        {
            if( (*fd = open(logfile, O_CREAT |O_WRONLY | O_APPEND, 0664)) < 0) //将内容附加到文件末尾
            {
                return -1;
            }
        }

        //一般日志，只写临时区，等待同步到备份区
        if( (*tmpfd = open(tmpfile, O_CREAT |O_WRONLY | O_APPEND, 0664)) < 0) //将内容附加到文件末尾
        {
            return -1;
        }        
    }

    return 0;
}

/**
*********************************************************************
* @brief:       写日志
* @param[in]：   logfile    日志文件
* @param[in]：   log        日志内容
* @param[out]:  NA
* @author:      贺宁
* @return：      void
*********************************************************************
*/
int diag_log_vprintf(int fd, const char *fmt, va_list ap)
{
    char pathsub[8192] = {"\0"}; 
    
    vsnprintf(pathsub, sizeof(pathsub), fmt, ap);

    write(fd, pathsub, strlen(pathsub));
    
    return 0;
}

/**
*********************************************************************
* @brief:       写日志
* @param[in]：   logfile    日志文件
* @param[in]：   log        日志内容
* @param[out]:  NA
* @author:      贺宁
* @return：      void
*********************************************************************
*/
int diag_log_write(int fd, const char *fmt, ...)
{
    char pathsub[8192] = {"\0"}; 
    va_list ap;
    
    va_start(ap, fmt);  
    vsnprintf(pathsub, sizeof(pathsub), fmt, ap);
    va_end(ap); 

    write(fd, pathsub, strlen(pathsub));
    
    return 0;
}

#endif

#if DESC("通用调试打印",1)

/* 解锁文件 */
void diag_unlock_file(void *arg)
{
    flock(*(int*)arg, LOCK_UN); //Unlock
    close(*(int*)arg);
}

/**
*********************************************************************
* @brief:       日志打印内容控制
* @param[in]：   module     模块名
* @param[in]：   printType  打印类型 1-debug 0-trace
* @param[in]：   infoType   信息类型 0-时戳 1-详情 2-内容
* @author:      贺宁
* @return：      void
*********************************************************************
*/
BOOLEAN diag_print_check(uint32 bitflag, const char *module, DIAG_TYPE_E printType)
{
    char debug[32] = {"DEBUG_"};
    char trace[32] = {"TRACE_"};
    char diag[32] = {"DIAG_"};  

    strcat(diag,module);
    strcat(debug,module);
    strcat(trace,module);

    if((printType == DIAG_LOGCC)||(printType == DIAG_DEBUG)) //DEBUG与LOGCC屏幕打印处理相同
    {
        return (DIAG_IS_DEBUG(bitflag)||
            (getenv(debug) != NULL)||(getenv(diag) != NULL)||
            (getenv("DEBUG_ALL") != NULL)||(getenv("DIAG_ALL") != NULL));
        
    }
    else if(printType == DIAG_TRACE) //TRACE
    {
        return (DIAG_IS_TRACE(bitflag)||
            (getenv(trace) != NULL)||(getenv(diag) != NULL)||
            (getenv("TRACE_ALL") != NULL)||(getenv("DIAG_ALL") != NULL));

    }

    return TRUE;
}

/**
*********************************************************************
* @brief:       写日志
* @param[in]：   logfile    日志文件
* @param[in]：   log        日志内容
* @param[out]:  NA
* @author:      贺宁
* @return：      void
*********************************************************************
*/
void diag_vprintf(int fd, const char *fmt, va_list ap)
{
    if(fd >= 0)
    {
        (void)diag_log_vprintf(fd, fmt, ap);
    }
    else
    {
        vprintf(fmt, ap);
    }
}

/**
*********************************************************************
* @brief:       写日志
* @param[in]：   logfile    日志文件
* @param[in]：   log        日志内容
* @param[out]:  NA
* @author:      贺宁
* @return：      void
*********************************************************************
*/
void diag_printf(int fd, const char *fmt, ...)
{
    va_list ap;
    
    va_start(ap, fmt);  
    diag_vprintf(fd, fmt, ap);
    va_end(ap);   
}

/**
*********************************************************************
* @brief:       缓冲区打印控制
* @param[in]：   logfile    日志文件
* @param[in]：   log        日志内容
* @param[out]:  NA
* @author:      贺宁
* @return：      void
*********************************************************************
*/
void diag_buffer_ctrl(int fd, const void *buf, uint32 len)
{
    uint32 offset = 0;
    char logbuf[8192] = {0};
    
    //写入打印内容
    sprintf(logbuf, " L-%d:", len);
    
    offset = strlen(logbuf);
    
    while(len--)
    {
        if(offset+8 > 8191)
        {
            diag_printf(fd, "%s", logbuf);
            memset(logbuf, 0, offset);
            offset = 0;
        }
        
        sprintf(logbuf+offset,"%02X ", *(char*)buf++);
        offset += 3;
    }

    sprintf(logbuf+offset, "\n");

    diag_printf(fd, "%s", logbuf);
}

/**
*********************************************************************
* @brief:       打印内容构造
* @param[in]：   logfile    日志文件
* @param[in]：   log        日志内容
* @param[out]:  NA
* @author:      贺宁
* @return：      void
*********************************************************************
*/
void diag_output_ctrl(int fd, const void *buf, uint32 len, const char *fmt, va_list ap)
{    
    if(fmt != NULL)
    {
        diag_vprintf(fd, fmt, ap);
    }
    
    if(buf != NULL)
    {
        diag_buffer_ctrl(fd, buf, len);
    }
}

/**
*********************************************************************
* @brief:       控制日志信息输出
* @param[in]：   logfile    日志文件
* @param[in]：   log        日志内容
* @param[out]:  NA
* @author:      贺宁
* @return：      void
*********************************************************************
*/
BOOLEAN diag_logout_ctrl(uint32 bitflag, char *file, int line, const char *func, int modid, char *subfix, 
                         DIAG_TYPE_E diagtype, const void *buf, uint32 len, const char *fmt, va_list ap)
{
    int fd = -1;
    int tmpfd = -1;
    BOOLEAN logprint = FALSE;
    char logcc[32] = {"LOGCC_"}; 
    char trlog[32] = {"TRLOG_"}; 
    struct timeval tv;
    struct  timezone tz;     
    struct tm      tm; 
    char modname[32] = {0};
    char filename[PATH_MAX_LEN+1] = {0};
    char logfile[PATH_MAX_LEN+1] = {0};        
    char tmpfile[PATH_MAX_LEN+1] = {0};    
    const char *module =  g_logModule[modid].module;

    if(diagtype == DIAG_DEBUG)
    {
        logprint = TRUE;
    }
    else if(diagtype == DIAG_LOGCC)
    {
        strcat_s(logcc, 32, module);
        if(DIAG_IS_LOGCC(bitflag)||
            (getenv(logcc) != NULL)||(getenv("LOGCC_ALL") != NULL))
        {
            logprint = TRUE;
        }
    }
    else if(diagtype == DIAG_TRACE)
    {
        strcat_s(trlog, 32, module);
        if(DIAG_IS_TRLOG(bitflag)||
            (getenv(trlog) != NULL)||(getenv("TRLOG_ALL") != NULL))
        {
            logprint = TRUE;
        }
    }

    if(logprint)
    {
        //这个函数只会返回成功
        file_name_extract(file, TRUE, filename);

        strcpy_s(modname, 32, module);
        
        str2lower(modname);
            
        //支持自定义日志名
        if(subfix != NULL)
        {
            sprintf(logfile, "%s/%s.log", g_logpath, subfix);
        }
        else
        {
            if(DIAG_IS_MERGE(bitflag)) //合并日志,根据app名构造日志名称
            {
                sprintf(logfile, "%s/%s.log", g_logpath, g_log_name);
            }
            else //根据模块名构造日志名称
            {
                sprintf(logfile, "%s/%s.log", g_logpath, modname);
            }
        }

        //替换/data为/tmp/data
        sprintf(tmpfile, "/tmp%s", logfile);
        
        //打开日志文件
        if(diag_log_open(tmpfile, logfile, diagtype, modid, &fd, &tmpfd) < 0)
        {
            return FALSE;
        }

        if(fd >= 0)
        {
            //开始写的时候就要加锁了,多线程下要基于文件加锁，记录锁会失效，阻塞型
            /* 防止线程退出后 没有释放锁资源(不带条件变量) */
            pthread_cleanup_push(diag_unlock_file, &fd);
            flock(fd, LOCK_EX);
            
            gettimeofday(&tv, &tz);
            FastSecondToDate(&tv, &tz, &tm);

            if(DIAG_IS_MERGE(bitflag))
            {
                //写入时间戳,模块名,文件名,行号
                diag_log_write(fd, "[%4d-%02d-%02d %02d:%02d:%02d.%06ld][%s][%s:%d] ", 
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                    tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec, module, filename, line);

            }
            else
            {
                //写入时间戳,文件名，行号
                diag_log_write(fd, "[%4d-%02d-%02d %02d:%02d:%02d.%03ld %s:%d]", 
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                    tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000, filename, line);
            }

            diag_output_ctrl(fd, buf, len, fmt, ap);

            flock(fd, LOCK_UN); //Unlock
            close(fd);
            pthread_cleanup_pop(0);
        }

        if(tmpfd >= 0)
        {
            //开始写的时候就要加锁了,多线程下要基于文件加锁，记录锁会失效，阻塞型
            /* 防止线程退出后 没有释放锁资源(不带条件变量) */
            pthread_cleanup_push(diag_unlock_file, &tmpfd);
            flock(tmpfd, LOCK_EX);
            
            gettimeofday(&tv, &tz);
            FastSecondToDate(&tv, &tz, &tm);

            if(DIAG_IS_MERGE(bitflag))
            {
                //写入时间戳,模块名,文件名,行号
                diag_log_write(tmpfd, "[%4d-%02d-%02d %02d:%02d:%02d.%06ld][%s][%s:%d] ", 
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                    tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec, module, filename, line);

            }
            else
            {
                //写入时间戳,文件名，行号
                diag_log_write(tmpfd, "[%4d-%02d-%02d %02d:%02d:%02d.%03ld %s:%d]", 
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                    tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000, filename, line);
            }

            diag_output_ctrl(tmpfd, buf, len, fmt, ap);

            flock(tmpfd, LOCK_UN); //Unlock
            close(tmpfd);
            pthread_cleanup_pop(0);
        }        
    }
    
    return logprint;
}

/**
*********************************************************************
* @brief:       控制前缀信息输出内容
* @param[in]：   logfile    日志文件
* @param[in]：   log        日志内容
* @param[out]:  NA
* @author:      贺宁
* @return：      void
*********************************************************************
*/
BOOLEAN diag_display_ctrl(uint32 bitflag, char *file, int line, const char *func, const char *module, DIAG_TYPE_E diagtype)
{
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm; 
    char filename[128] = {0};

    //内容允许输出，才处理时戳和文件定位信息，否则是无意义的
    if(diag_print_check(bitflag, module, diagtype))
    {
        gettimeofday(&tv, &tz);
        FastSecondToDate(&tv, &tz, &tm);

        //这个函数只会返回成功
        file_name_extract(file, TRUE, filename);

        if(diagtype == DIAG_TRACE)
        {
            printf(HIGH_LIGHT COLOR_FONT_GREEN"[%4d-%02d-%02d %02d:%02d:%02d.%06ld][T][%s][%s:%d] "CHAR_TYPE_NONE, 
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec,
                module, filename, line);//终端或屏幕打印应该打印模块名，不然容易混淆  
        }
        else if(diagtype == DIAG_DEBUG)
        {
            printf(HIGH_LIGHT COLOR_FONT_RED"[%4d-%02d-%02d %02d:%02d:%02d.%06ld][D][%s][%s:%d] "CHAR_TYPE_NONE, 
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec,
                module, filename, line);//终端或屏幕打印应该打印模块名，不然容易混淆 
        }
        else
        {
            printf(HIGH_LIGHT COLOR_FONT_YELLOW"[%4d-%02d-%02d %02d:%02d:%02d.%06ld][L][%s][%s:%d] "CHAR_TYPE_NONE, 
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec,
                module, filename, line);//终端或屏幕打印应该打印模块名，不然容易混淆 
        }

        return TRUE;
    }

    return FALSE;
}

/**
*********************************************************************
* @brief: 诊断信息打印函数
* @param[in]：   file      打印信息所在文件
* @param[in]：   line      打印信息所在行号
* @param[in]：   func      打印信息所在函数
* @param[in]：   modid     模块ID，@ref g_logModule
* @param[in]：   logfile   日志文件，为空则不写日志
* @param[in]：   diagtype  诊断类型0-TRACE不记日志 1-DEBUG必记日志 2-LOGCC受控记日志
* @param[in]：   buf       需要输出的buffer
* @param[in]：   len       buffer长度
* @param[in]：   fmt       格式化输入参数
* @author:      贺宁
* @return：      void
*
* @detail:使用环境变量 

* export DIAG_模块名=1            打开本模块所有类型打印                         unset DIAG_模块名 关闭
* export DIAG_ALL=1            打开所有模块所有类型打印                        unset DIA_ALL 关闭  
* export DEBUG_模块名=1           打开本模块DEBUG类型打印                      unset DEBUG_模块名 关闭
* export TRACE_模块名=1           打开本模块TRACE类型打印                      unset TRACE_模块名 关闭
* export DEBUG_ALL=1           打开所有模块DEBUG类型打印                     unset DEBUG_ALL 关闭
* export TRACE_ALL=1           打开所有模块TRACE类型打印                     unset TRACE_ALL 关闭
* export LOGCC_模块名=1           打开本模块LOGCC类型日志记录                    unset LOGCC_模块名 关闭
* export LOGCC_ALL=1           打开所有模块LOGCC类型日志记录                   unset LOGCC_ALL 关闭  
* export TRLOG_模块名=1           打开本模块TRACE类型日志记录                    unset TRLOG_模块名 关闭
* export TRLOG_ALL=1           打开所有模块TRACE类型日志记录                   unset TRLOG_ALL 关闭 

* 用法举例：按模块定义TRACE宏(模块名_BUF_TRACE)，DEBUG宏(模块名_BUF_DEBUG)，LOGCC宏(模块名_BUF_LOGCC)，以上报模块为例
* #define RPT_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__, __LINE__, __FUNCTION__, "RPT", RPT_LOG, 0, buf, len, fmt, ##__VA_ARGS__)
* #define RPT_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__, __LINE__, __FUNCTION__, "RPT", RPT_LOG, 1, buf, len, fmt, ##__VA_ARGS__)
* #define RPT_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__, __LINE__, __FUNCTION__, "RPT", RPT_LOG, 2, buf, len, fmt, ##__VA_ARGS__)
* 
* 如果buf传空指针，大部分打印是这种情形，其行为同格式化输出，如果模块定义了BUF类打印，可以这么定义FMT类打印
* #define RPT_FMT_TRACE(fmt,...)  RPT_BUF_TRACE(NULL, 0, fmt, ##__VA_ARGS__)
* #define RPT_FMT_DEBUG(fmt,...)  RPT_BUF_DEBUG(NULL, 0, fmt, ##__VA_ARGS__)
* #define RPT_FMT_LOGCC(fmt,...)  RPT_BUF_LOGCC(NULL, 0, fmt, ##__VA_ARGS__)
*
* 如果buf传空指针，大部分打印是这种情形，其行为同格式化输出，如果模块未定义BUF类打印，可以这么定义FMT类打印
* #define RPT_FMT_TRACE(fmt,...)  diag_info_print(__FILE__, __LINE__, __FUNCTION__, "RPT", RPT_LOG, 0, NULL, 0, fmt, ##__VA_ARGS__)
* #define RPT_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__, __LINE__, __FUNCTION__, "RPT", RPT_LOG, 1, NULL, 0, fmt, ##__VA_ARGS__)
* #define RPT_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__, __LINE__, __FUNCTION__, "RPT", RPT_LOG, 2, NULL, 0, fmt, ##__VA_ARGS__)
*
*********************************************************************
*/
void diag_info_print(char *file, int line, const char *func, int modid, char *subfix, DIAG_TYPE_E diagtype, const void *buf, uint32 len, const char *fmt, ...)
{
    va_list ap;
    dictionary *ini = NULL;
    char key[64] = {0};
    struct stat fileinfo = {0};
    static time_t lstmodify = 0;
    const char *module = NULL;
    int bitflag = 0;  
    uint32 i = 0;
    char *cfginfo[3] = {0};
    int cfgnum = 0;
    int mergesize = 0;
    int mergecnt = 0;

    if((modid >= g_logModuleSize) || ((fmt == NULL) && (buf == NULL)))
    {
        return;
    }

    module = g_logModule[modid].module;
    bitflag = g_logModule[modid].bitCtrl;
    
    //读取配置文件
    if(strnlen_s(g_ini_file, 256) != 0)
    {
        //通过文件修改时间，检测配置文件是否更新
        stat(g_ini_file, &fileinfo);
        
        if(fileinfo.st_mtime != lstmodify)
        {
            lstmodify = fileinfo.st_mtime;
            
            ini = iniparser_load(g_ini_file);
            
            if(NULL != ini)
            {
                //获取打印控制参数
                bitflag = iniparser_getint(ini, "LOGCTRL_CONF:LOGALL", 0);

                for(i = 0; i < g_logModuleSize; i++)
                {
                    sprintf(key, "LOGCTRL_CONF:LOG%s", g_logModule[i].module);
                    memset_s(cfginfo, sizeof(cfginfo), 0, sizeof(cfginfo));

                    split(iniparser_getstring(ini, key, "0"), ",", cfginfo, &cfgnum);

                    if(cfginfo[0] != NULL)
                    {
                        g_logModule[i].bitCtrl = bitflag | atoi(cfginfo[0]);
                    }

                    if(cfginfo[1] != NULL)
                    {
                        g_logModule[i].sizeLimit = atoi(cfginfo[1])*1024*1024;
                    }

                    if(cfginfo[2] != NULL)
                    {
                        g_logModule[i].packedNum = atoi(cfginfo[2]);
                    }

                    if(DIAG_IS_MERGE(g_logModule[i].bitCtrl))
                    {
                        mergesize += g_logModule[i].sizeLimit * g_logModule[i].packedNum;
                        g_logMergeInfo.sizeLimit = (g_logMergeInfo.sizeLimit*mergecnt + g_logModule[i].sizeLimit)/(mergecnt+1);
                        mergecnt++;
                    }
                                    
                    memset_s(key, 64, 0, 64);

                    if(g_logModule[i].bitCtrl != 0)
                    {
                        printf("logCtrl: module = %-8s, bitCtrl = %2d, sizeLimit = %8d,  packedNum = %4d\n", 
                            g_logModule[i].module, g_logModule[i].bitCtrl,g_logModule[i].sizeLimit, g_logModule[i].packedNum);
                    }
                }
                
                iniparser_freedict(ini);

                if(g_logMergeInfo.sizeLimit > 0)
                {
                    g_logMergeInfo.packedNum = mergesize/g_logMergeInfo.sizeLimit;
                }
            }

            bitflag = g_logModule[modid].bitCtrl;

            printf("mergeCnt: %d mergeLimit: %d mergePacked: %d\n", mergecnt, g_logMergeInfo.sizeLimit, g_logMergeInfo.packedNum);
        }
    }

    va_start(ap, fmt);

    //处理日志打印
    diag_logout_ctrl(bitflag, file, line, func, modid, subfix, diagtype, buf, len, fmt, ap);
    /*if(diag_logout_ctrl(bitflag, file, line, func, modid, logpath, subfix, diagtype, &fd, buf, len, fmt, ap))
    {
        diag_output_ctrl(fd, buf, len, fmt, ap);
        flock(fd, LOCK_UN); //Unlock
        close(fd);
    }*/

    //处理屏幕打印 
    if(diag_display_ctrl(bitflag, file, line, func, module, diagtype))
    {
        diag_output_ctrl(-1, buf, len, fmt, ap);            
    }

    va_end(ap);
    
    return;
}

#endif

#if DESC("开源日志库zlog",1)

/**
*********************************************************************
* @brief:       DIAG模块初始化
* @param[in]：   
* @param[in]：  
* @return：      void
*********************************************************************
*/
int diag_init(char *conf, uint8 bNoTmp)
{
    int       errCode;
    pthread_t thidLog = 0;
    dictionary *ini = NULL;
    char  *plogpath = NULL;
    char  cmd[PATH_MAX_LEN + 1];

    strcpy_s(g_ini_file, 256, conf);

	bNoTmp = TRUE;

    ini = iniparser_load(g_ini_file);  
    if(NULL != ini)
    {
        //获取log路径
        plogpath = iniparser_getstring(ini, "FILEPATH_CONF:LOGPATH", "/data/app/log");  

        memset_s(g_logpath, sizeof(g_logpath), 0, sizeof(g_logpath));
        strcpy_s(g_logpath, sizeof(g_logpath), plogpath);
#ifdef AREA_FUJIAN
        g_record_flag = iniparser_getint(ini, "FILEPATH_CONF:TIMEBACKFLAG", 1);
        
#endif
    }
    iniparser_freedict(ini); 

    path_dir_make(g_logpath);
    sprintf(g_tmppath, "/tmp%s", g_logpath);
    
    if(FALSE == bNoTmp)
    {
        //日志从备份区同步到临时区
        path_dir_make(g_tmppath);
        //只同步未压缩的，压缩好的就是备份的，不需要同步
        sprintf(cmd, "cp -rf %s/*.log %s", g_logpath, g_tmppath);
        system(cmd);
        g_emerge_status.bData = FALSE;
    }
    else
    {
        g_emerge_status.bData = TRUE;
    }

    //启动日志维护线程
    errCode = pthread_create(&thidLog, NULL, log_manager_entry, NULL);
    if (errCode != 0)
    {
        printf("Thread app_main_task create error,err: %d\n", errCode);
        exit(0);
        return -1;
    }
    return file_name_extract(g_ini_file, FALSE, g_log_name);
    //return zlog_init(ZLOG_CONFIG_PATH);
}

/**
*********************************************************************
* @brief:       FATAL级别日志
* @param[in]：   
* @param[in]：  
* @return：      void
*********************************************************************
*/
void diag_info(char *cname,
    const char *file, const char *func,long line, int level, const char *format, ...)
{
    //zlog_category_t *cat = NULL;
    va_list args;

    //cat = zlog_get_category(cname);

    //每次打印重新载入配置，这样配置修改后就直接生效了，不需要重启
    //zlog_reload(ZLOG_CONFIG_PATH);
    
    va_start(args, format);  
    //vzlog(cat, file, strlen(file), func, strlen(func), line, level, format, args);
    va_end(args);   
}

#endif

