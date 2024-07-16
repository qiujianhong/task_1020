/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控mqtt
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"
#include "task_msg.h"
#include "db_api.h"
#include "appmsg_split.h"
#include "mqtt_client.h"
#include "task_base.h"
#include "task_id.h"
#include "msg_id.h"
#include "storage.h"

#include "sec_comm.h"
#include "sec_mqtt.h"
#include "sec_mgr.h"
#include <linux/param.h>
#include <pwd.h>
#include <grp.h>

SEC_SESSION_LIMIT_T g_session_limit = {.monthFlowLimit=0, .flowSpeedLimit=0};
SEC_SRVTIME_LIMIT_T g_srvtime_limit = {0};
SEC_SESSIONAPPS_T  g_session_info = {0}; //流量会话信息
OOP_DATETIME_S_T   g_currtime;     /*当前时间,全局时间变量*/
OOP_DATETIME_S_T   g_lasttime;     /*当前时间,全局时间变量*/
uint32 g_sshtime_limit = 0;        //默认0分钟
SEC_PROCINFOS_T g_proc_info = {0}; //进程信息
/*以下为时间过秒,过分,过时,过日,过月，过年标记*/
uint8   pastminflag;
uint8   pasthourflag;
uint8   pastdayflag;
uint8   pastmonflag;
uint8   pastyearflag;

#if DESC("公共支持功能",1)

//获取指定目录下文件属性
int sec_dirfileattr_get(char* dir, KEY_DIR_FILE_VECTOR* fileattri ,int maxfp)
{
    int  num = 0;
    int  ret = 0;
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    char file_name[300] = {0};//文件绝对路径+文件名

    if(NULL==dir ||NULL==fileattri)
    {
        return -1;
    }

    if((dp = opendir(dir)) == NULL)
    {
        return -2;
    }

    while((entry = readdir(dp)) != NULL)
    {
        strcpy(file_name,dir);
        strcat(file_name,entry->d_name);

        ret = stat(file_name,&statbuf);
        if( ret==0 )
        {        
            if(S_ISDIR(statbuf.st_mode))
            {
                continue;
            }
            else
            {
                fileattri->file_vector[num].mode  = statbuf.st_mode;        //权限
                fileattri->file_vector[num].mtime = statbuf.st_mtime;       //修改时间
                if(strlen(entry->d_name) < KEY_MAX_FILE_LEN)
                {
                    strcat(fileattri->file_vector[num].name, entry->d_name);//文件名
                }
                else
                {
                    memcpy(fileattri->file_vector[num].name,entry->d_name,KEY_MAX_FILE_LEN);
                }

                num++;
                if(num >= maxfp) 
                {
                    break;
                }
            }       
        }
        else
        {
            perror("lstat");
        }
    }
    
    closedir(dp);
    return num;
}

//以太网远程服务端口以及外联信息监控
//解析以太网端点信息
/****************************************************************
Proto Recv-Q Send-Q Local Address           Foreign Address             State
tcp        0      0         0.0.0.0:54321         0.0.0.0:*               LISTEN
tcp        0      0         0.0.0.0:21            0.0.0.0:*               LISTEN
tcp        0      0         0.0.0.0:23            0.0.0.0:*               LISTEN
tcp        0    138         192.168.10.27:23      192.168.10.146:52719    ESTABLISHED
tcp        0      0         192.168.10.27:23      192.168.10.145:51640    ESTABLISHED
tcp        0      0         192.168.10.27:21      192.168.10.146:52703    ESTABLISHED
tcp        0      0         192.168.10.27:23      192.168.10.146:52680    ESTABLISHED
tcp        0      0         10.10.232.23:45061    120.195.49.236:2145     ESTABLISHED 
netstat: no support for `AF INET6 (tcp)' on this system.
*****************************************************************/
int sec_tcpinfo_get(POINT_INFO* info, int maxlen)
{
    uint8 i=0;
    FILE* fp;
    int recv_q              = 0;
    int send_q              = 0;    
    char ptrl[10]           ={0};
    char state[20]          ={0};

    char local_addr[50]     ={0};//IP和PORT 格式 xx.xx.xx.xx:xxxx
    char local_ip[20]       ={0};
    int  local_port     = 0;

    char foreign_addr[50]    ={0};//IP和PORT 格式 xx.xx.xx.xx:xxxx
    char foreign_ip[20]      ={0};
    int  foreign_port    = 0;
    char buf[512];   
    char cmd[128];   
    char *tmp;   

    if(NULL == info)
    {
        return -1;
    }
    
    strcpy(cmd, "netstat -atn");
    fp = popen(cmd, "r");   
    if(NULL == fp)   
    {   
        SEC_FMT_DEBUG("popen error\n");
        return -2;   
    }
    
    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
        tmp =buf;
        while(*tmp && isspace(*tmp))
        {
            ++tmp;
        }
        
        if(strncmp(tmp, "tcp", strlen("tcp")) == 0)
        {
            sscanf(tmp, "%s%*[ ]%d%*[ ]%d%*[ ]%s%*[ ]%s%*[ ]%s", ptrl, &recv_q, &send_q, local_addr, foreign_addr, state);
            //printf("local_addr   = %s\n",local_addr);
            //printf("foreign_addr = %s\n",foreign_addr);
            sscanf(local_addr,   "%[^:]:%d", local_ip,   &local_port);
            sscanf(foreign_addr, "%[^:]:%d", foreign_ip, &foreign_port);
            //printf("LOCAL--PORT:%u\n",local_port);
            //printf("ESTABLISHED--IP  :%s\n",foreign_ip);
            //printf("ESTABLISHED--PORT:%u\n",foreign_port);
            
            strcpy(info[i].state, state);
            strcpy(info[i].local_ip  , local_ip);
            strcpy(info[i].foreign_ip, foreign_ip);
            info[i].local_port   = local_port;
            info[i].foreign_port = foreign_port;
            i++;
            
            if(i >= maxlen)
            {
                break;
            }
        }
    }

    pclose(fp);
    return i;
}

BOOL sec_is_illegal_addr(LINK_ADDR* cmp_addr, uint8 cchcnt, POINT_INFO netinfo)
{
    uint8 i=0;
    
    for(i = 0; i < cchcnt; i++)
    {
        if(0 == strcmp(netinfo.foreign_ip, cmp_addr[i].ipAddr) 
            && netinfo.foreign_port == cmp_addr[i].port)
        {
            break;
        }
    }
    
    if(i == cchcnt)
    {
        return FALSE;
    }
    else
    {   
        return TRUE;
    }
}

/**********************************************************************
* @name      : sec_loginusr_get
* @brief     ：获取登录用户数
* @param[in] ：loginType  登录类型 @ref SEC_LOGINTYPE_E
* @param[out]：
* @return    ：登录用户数量
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_loginusr_get(uint8 loginType)
{
    FILE* fp;
    char buf[512];   
    char cmd[128];   
    char *tmp;   
    int usrNum = 0;
    
    strcpy(cmd, "who | awk '{print $2}'");
    fp = popen(cmd, "r");   
    if(NULL == fp)   
    {   
        SEC_FMT_DEBUG("popen error\n");
        return -2;   
    }
    
    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
        tmp =buf;
        while(*tmp && isspace(*tmp))
        {
            ++tmp;
        }

        if(loginType == LOGIN_ETH)
        {
            if(strncmp(tmp, "pts", strlen("pts")) == 0)
            {
                usrNum++;
            }
        }
        else
        {
            if(strncmp(tmp, "tty", strlen("tty")) == 0)
            {
                usrNum++;
            }
        }
    }

    pclose(fp);
    
    return usrNum;
}

/**********************************************************************
* @name      : sec_sshstatus_get
* @brief     ：获取SSH当前状态
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_sshstatus_get(void)
{
    return 0;
}

/**********************************************************************
* @name      : sec_sessionapp_get
* @brief     ：获取流量会话关联APP
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_session_get(SEC_SESSIONAPP_T* map, int maxlen)
{
    uint8 i=0;
    uint8 mapNum = 0;
    FILE* fp;
    char buf[512];   
    char cmd[128];   
    char *tmp;    
    char hostIP[20] = {0};
    char containerIP[20] = {0};
    char containerList[32][32] = {"0"};
    uint8 containerListNum = 0;
    
    strcpy(cmd, "netstat -ntup | grep -v \"172.17.0.1:1883\"| awk -F '[ :/]+' '{print $1,$4,$5,$6,$7,$10}'");
    fp = popen(cmd, "r");   
    if(NULL == fp)   
    {   
        SEC_FMT_DEBUG("popen error\n");
        return -2;   
    }
    
    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
        tmp =buf;
        while(*tmp && isspace(*tmp))
        {
            ++tmp;
        }

        if((strncmp(tmp, "tcp", strlen("tcp")) == 0) || (strncmp(tmp, "udp", strlen("udp")) == 0))
        {
            sscanf(tmp, "%s%*[ ]%s%*[ ]%hu%*[ ]%s%*[ ]%hu%*[ ]%s", map[mapNum].protocol, map[mapNum].local_ip, &map[mapNum].local_port, map[mapNum].foreign_ip, &map[mapNum].foreign_port, map[mapNum].process);
            str2upper(map[mapNum].protocol);
            map[mapNum].procType = SEC_PROC_HOST;
            mapNum++;
        }
    }

    pclose(fp);

    //获取本机IP
    memset(cmd, 0, sizeof(cmd));
    memset(buf, 0, sizeof(buf));
    
    strcpy(cmd, "ifconfig | awk -F \"[ :]+\" 'NR==2{print $4}'");
    fp = popen(cmd, "r");   
    if(NULL == fp)   
    {   
        SEC_FMT_DEBUG("popen error\n");
        return -2;   
    }

    if(fgets(buf, sizeof(buf), fp) != NULL)
    {
        sscanf(buf, "%[^'\n']", hostIP);
        SEC_FMT_TRACE("本机IP:%s\n", hostIP);
    }
    pclose(fp);

    //获取容器列表
    memset(cmd, 0, sizeof(cmd));
    memset(buf, 0, sizeof(buf));
    strcpy(cmd, "docker ps | awk '{if(NR>1){print $NF}}'");
    fp = popen(cmd, "r");   
    if(NULL == fp)   
    {   
        SEC_FMT_DEBUG("popen error\n");
        return -2;   
    }
    
    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
        tmp =buf;
        while(*tmp && isspace(*tmp))
        {
            ++tmp;
        }

        sscanf(tmp, "%[^'\n']", containerList[containerListNum++]);
    }
    pclose(fp);

    for(i = 0; i < containerListNum; i++)
    {
        //获取容器IP
        memset(cmd, 0, sizeof(cmd));
        memset(buf, 0, sizeof(buf));
        memset(containerIP, 0, sizeof(containerIP));
        
        sprintf(cmd, "docker exec %s ifconfig | awk -F \"[ :]+\" 'NR==2{print $4}'", containerList[i]);
        fp = popen(cmd, "r");   
        if(NULL == fp)   
        {   
            SEC_FMT_DEBUG("popen error\n");
            return -2;   
        }

        if(fgets(buf, sizeof(buf), fp) != NULL)
        {
            sscanf(buf, "%[^'\n']", containerIP);
            SEC_FMT_TRACE("容器 %s IP:%s\n", containerList[i], containerIP);
        }
        pclose(fp);

        //获取容器内五元组信息
        memset(cmd, 0, sizeof(cmd));
        memset(buf, 0, sizeof(buf));

        sprintf(cmd, "docker exec %s netstat -ntup | grep -v \"172.17.0.1:1883\\|127.0.0.1\" | awk -F '[ :/]+' '{print $1,$4,$5,$6,$7,$10}'", containerList[i]);
        fp = popen(cmd, "r");   
        if(NULL == fp)   
        {   
            SEC_FMT_DEBUG("popen error\n");
            return -2;   
        }
        
        while(fgets(buf, sizeof(buf), fp) != NULL)
        {
            tmp =buf;
            while(*tmp && isspace(*tmp))
            {
                ++tmp;
            }

            if((strncmp(tmp, "tcp", strlen("tcp")) == 0) || (strncmp(tmp, "udp", strlen("udp")) == 0))
            {
                sscanf(tmp, "%s%*[ ]%s%*[ ]%hu%*[ ]%s%*[ ]%hu%*[ ]%s", map[mapNum].protocol, map[mapNum].local_ip, &map[mapNum].local_port, map[mapNum].foreign_ip, &map[mapNum].foreign_port, map[mapNum].process);
                str2upper(map[mapNum].protocol);
                map[mapNum].procType = SEC_PROC_CONT;
                
                if(strcmp(map[mapNum].local_ip, containerIP) == 0)
                {
                    memset(map[mapNum].local_ip, 0, strlen(map[mapNum].local_ip));
                    memcpy(map[mapNum].local_ip, hostIP, strlen(hostIP));
                }
                mapNum++;
            }
        }

        pclose(fp);
    }
    
    return mapNum;
}

/**********************************************************************
* @name      : sec_session_cmp
* @brief     ：比较两个会话是否相等
* @param[in] ：
* @param[out]：
* @return    ：=0 相等，否则不相等
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_session_cmp(SEC_SESSIONAPP_T *session1, SEC_SESSIONAPP_T *session2)
{
    if(strcmp(session1->local_ip, session2->local_ip) != 0)
    {
        return -1;
    }

    if(session1->local_port != session2->local_port)
    {
        return -1;
    }

    if(strcmp(session1->foreign_ip, session2->foreign_ip) != 0)
    {
        return -1;
    }

    if(session1->foreign_port != session2->foreign_port)
    {
        return -1;
    }

    if(strcmp(session1->protocol, session2->protocol) != 0)
    {
        return -1;
    }

    if(strcmp(session1->process, session2->process) != 0)
    {
        return -1;
    }

    return 0;
}

/**********************************************************************
* @name      : sec_sessionapp_update
* @brief     ：刷新流量会话关联APP
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_session_update(void)
{
    int ret=0;
    SEC_SESSIONAPP_T curmap[SEC_RECORD_MAX] = {0};
    uint8 mapNum = 0;
    uint8 deleteID = 0;
    int updateTime = 0;
    int i = 0, j = 0, k = 0;

    //获取当前映射
    mapNum = sec_session_get(curmap, SEC_RECORD_MAX);

    //删除已经不存在的连接
    for(i = 0; i < g_session_info.nNum; i++)
    {
        for(j = 0; j < mapNum; j++)
        {
            if(sec_session_cmp(&g_session_info.info[i], &curmap[j]) == 0)
            {
                break;
            }
        }

        //没找到则删除
        if(j == mapNum)
        {
            if(i < g_session_info.nNum-1)
            {
                memmove(&g_session_info.info[i], &g_session_info.info[i+1],
                    (g_session_info.nNum-i-1)*sizeof(SEC_SESSIONAPP_T));
            }

            g_session_info.nNum--;
        }
    }

    //添加新的连接
    for(i = 0; i < mapNum; i++)
    {
        for(j = 0; j < g_session_info.nNum; j++)
        {
            if(sec_session_cmp(&curmap[i], &g_session_info.info[j]) == 0)
            {
                break;
            }
        }

        //没找到则添加
        if(j == g_session_info.nNum)
        {
            if(g_session_info.nNum < SEC_RECORD_MAX) //表有空闲则直接添加
            {
                memcpy(&g_session_info.info[g_session_info.nNum], &curmap[i], sizeof(SEC_SESSIONAPP_T));
                g_session_info.nNum++;
            }
            else //否则替换最低活跃度会话
            {
                deleteID = 0;
                updateTime = g_session_info.info[deleteID].updateTime;
                
                for(k = 1; k < g_session_info.nNum; k++)
                {
                    if(updateTime < g_session_info.info[k].updateTime)
                    {
                        updateTime = g_session_info.info[k].updateTime;
                        deleteID = k;
                    }
                }

                memcpy(&g_session_info.info[deleteID], &curmap[i], sizeof(SEC_SESSIONAPP_T));
            }
        }
    }

    return ret;
}

/**
*********************************************************************
* @brief：     获取系统实时时间
* @param[in]： rectify 修正秒数
* @param[out]：pTime 系统时间
* @return：     void
*********************************************************************
*/
void sec_datetime_get(uint32 rectify, OOP_DATETIME_S_T *pTime)
{
    struct tm systime;
    struct  timeval tv;
    struct  timezone tz;
    gettimeofday(&tv, &tz);

    tv.tv_sec += rectify;
    FastSecondToDate(&tv, &tz, &systime);

    pTime->second = systime.tm_sec;
    pTime->minute = systime.tm_min;
    pTime->hour = systime.tm_hour;
    pTime->day = systime.tm_mday;            
    pTime->month = systime.tm_mon + 1;    
    pTime->year = systime.tm_year + 1900; 
}

/**
*********************************************************************
* @brief：      过时标记更新
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void sec_spantime_update(void)
{   
    if(g_currtime.year != g_lasttime.year)
    {
        pastyearflag = 1;
        pastmonflag = 1;
        pastdayflag = 1;
        pasthourflag = 1;
        pastminflag = 1;
    }
    else
    {
        pastyearflag = 0;
        if(g_currtime.month != g_lasttime.month)
        {
            pastmonflag = 1;
            pastdayflag = 1;
            pasthourflag = 1;
            pastminflag = 1;
        }
        else
        {
            pastmonflag = 0;
            if(g_currtime.day != g_lasttime.day)
            {
                pastdayflag = 1;
                pasthourflag = 1;
                pastminflag = 1;
            }
            else
            {
                pastdayflag = 0;
                if(g_currtime.hour != g_lasttime.hour)
                {
                    pasthourflag = 1;
                    pastminflag = 1;
                }
                else
                {
                    pasthourflag = 0;
                    if(g_currtime.minute != g_lasttime.minute)
                        pastminflag = 1;
                    else
                        pastminflag = 0;
                }
            }
        }
    }
}

/**********************************************************************
* @name      : sec_flow_stat
* @brief     ：流量统计
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_flow_stat(void *pArg, void *pPublic, void *pPrivate)
{
    int i = 0;

    //获取当前日历时间
    sec_datetime_get(0, &g_currtime);
    //过分过日等判断
    sec_spantime_update();

    //过分计算流速
    if(pastminflag)
    {
        for(i = 0; i < g_session_info.nNum; i++)
        {
            g_session_info.info[i].flowSpeed = g_session_info.info[i].calcFlow*8/60/1024; //单位Kbps
            g_session_info.info[i].calcUpdate = TRUE;
            g_session_info.info[i].calcFlow = 0;
            g_session_info.info[i].calcUpdate = FALSE;
            SEC_FMT_DEBUG("当前累积流量%dB 流速 %d Kbps\n", g_session_info.info[i].calcFlow, g_session_info.info[i].flowSpeed);
        }
    }
    
    //过月清除流量累积数据
    if(pastmonflag)
    {
        for(i = 0; i < g_session_info.nNum; i++)
        {
            g_session_info.info[i].monUpdate = TRUE;
            g_session_info.info[i].monthFlow = 0;
            g_session_info.info[i].monUpdate = FALSE;
        }
    }

    memcpy(&g_lasttime, &g_currtime, sizeof(OOP_DATETIME_S_T));

    return 0;
}

/**********************************************************************
* @name      : sec_procnum_get
* @brief     ：获取系统当前进程数量
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_procnum_get(int* procinfo, int maxlen)
{
    uint32 procNum = 0;
    DIR *dirp;
    struct dirent *entry;
    int pid = -1;

    dirp = opendir("/proc");
    if(dirp != NULL) 
    {
        while((entry = readdir (dirp)) != NULL) 
        {
            if ((strcmp(entry->d_name, ".") == 0) 
                ||(strcmp(entry->d_name, "..") == 0) 
                || (atoll(entry->d_name) <= 0))
            {
                continue;
            }

            if(procNum < maxlen)
            {
                pid = atoll(entry->d_name);
                if(pid > 299) //大于299的才是用户进程
                {
                    procinfo[procNum++] = pid;
                }
            }
            else
            {
                SEC_FMT_DEBUG("进程数量大于 %d\n", maxlen);
                break;
            }
        }
        
        closedir(dirp);
    }

    return procNum;
}

/**********************************************************************
* @name      : sec_procinfo_get
* @brief     ：获取系统当前进程信息
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_procinfo_get(int pid, SEC_PROCABNORMAL_T* procinfo)
{
    char path[128] = {0};
    int sysStartTime = 0;
    uint64 procStartTime = 0;
    int uid = -1;
    int gid = -1;
    FILE* fp = NULL;
    char cmd_buf[256] = {0};
    struct passwd *pwd;
    struct group *grp;
    char *pStr = NULL;

    //获取进程启动时间
    if ((fp = fopen("/proc/uptime", "r")) != NULL)
    {
        fscanf(fp, "%d %*d", &sysStartTime);

        sysStartTime = get_time(TIME_REALTIME) - sysStartTime;
        fclose(fp);
    }

    sprintf(path, "/proc/%d/stat", pid);

    fp = fopen(path, "r");
    if(fp != NULL)
    {
        fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*d %llu",  &procStartTime);
        
        DT_Time2DateTimeShort(sysStartTime + procStartTime/HZ, &procinfo->stime); 

        fclose(fp);
    }

    //获取进程用户
    sprintf(path, "/proc/%d/status", pid);
    fp = fopen(path, "r");
    if(fp != NULL)
    {
        while( fgets(cmd_buf,sizeof(cmd_buf),fp) > 0)
        {
            //获取进程名
            if( (pStr = strstr(cmd_buf,"Name:")) != NULL)
            {
                if(strlen(pStr+strlen("Name:")) > sizeof(procinfo->name)-1)
                {
                    fclose(fp);
                    SEC_FMT_DEBUG("进程名过长 %d>%d\n", strlen(pStr+strlen("Name:")), sizeof(procinfo->name)-1);
                    return -1;
                }
                
                sscanf(pStr+strlen("Name:"), "%s", procinfo->name);
                continue;
            }
            
            //获取Uid
            if( (pStr = strstr(cmd_buf,"Uid:")) != NULL)
            {
                sscanf(pStr+strlen("Uid:"), "%d", &uid);
                pwd = getpwuid(uid);
                strcpy(procinfo->user, pwd->pw_name);
                continue;
            }

             //获取Gid
            if( (pStr = strstr(cmd_buf,"Gid:")) != NULL)
            {
                sscanf(pStr+strlen("Gid:"), "%d", &gid);
                grp = getgrgid(gid);
                strcpy(procinfo->group, grp->gr_name);
                continue;
            }
        }
        
        fclose(fp);
    }
    
    //获取进程cmd
    sprintf(path, "/proc/%d/cmdline", pid);
    fp = fopen(path, "r");
    if(fp != NULL)
    {
        fscanf(fp, "%s",procinfo->cmdline);
        
        fclose(fp);
    }

    return 0;
}

/**********************************************************************
* @name      : sec_procinfo_get
* @brief     ：获取系统当前进程信息
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
BOOL sec_procfilter_get(SEC_PROCABNORMAL_T* procinfo)
{
    //无命令的不报
    if(strlen(procinfo->cmdline) == 0)
    {
        return FALSE;
    }

    //appm docker container runc
    if((strstr(procinfo->cmdline, "appm") != NULL)||(strstr(procinfo->cmdline, "docker") != NULL)
        ||(strstr(procinfo->cmdline, "container") != NULL) || (strstr(procinfo->cmdline, "runc") != NULL))
    {
        return FALSE;
    }

    return TRUE;
}

#endif

#if DESC("安全在线监测",1)
/**********************************************************************
* @name      : sec_dangercmd_check
* @brief     ：危险命令检测
* @param[in] ：void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_dangercmd_check(void *pPublic, void *pPrivate)
{
    /**危险操作信息检测通过history来查找**/
    //history不记录连续的重复指令, 这意味着如果连续输入了两条相同的非法操作指令,即使后一次的指令间隔很久很久才输入,也不会产生对应的安全事件
    //目前测试这个能满足要求,如果需要完善,需要操作系统方面进行支持(因为连续输入相同命令history不会记录,导致.ash_history文件的内容修改时间都不会更新,这里无法检测)
    FILE* fp_history;
    char cmd_buf[256] = {0};    //history指令buf,指令最大长度暂定256
    static int history_offset_old = 0;
    SEC_DANGERCMD_T dangercmd;

    fp_history = fopen("/home/sysadm/.ash_history", "r");
    
    if( fp_history )
    {
        //未检测到以前的记录信息,重新获取
        if( history_offset_old==0 )
        {
            fseek(fp_history, 0, SEEK_END);
            history_offset_old = ftell(fp_history);
        }
        //以前的记录存在,开始分析新产生的记录信息是否包含危险操作指令
        else
        {
            //定位到上次分析的位置
            fseek(fp_history, history_offset_old, SEEK_SET);    
    
            //检测危险指令
            while( fgets(cmd_buf,sizeof(cmd_buf),fp_history) >0  )
            {
                SEC_FMT_TRACE("危险命令操作监控\n");
    
                //检测rm命令:D0：置“1”：发生文件/目录删除命令操作
                //现在只检测指令而不管该指令是否是否生效
                if( strstr(cmd_buf,"rm ") != NULL)//"rm "添加空格确保是完整指令
                {
                    dangercmd.cmd = SEC_CMD_RM;
                    SEC_FMT_DEBUG("检测到危险命令:rm\n");
                    sec_dangercmd_send(&dangercmd);
                }
    
                //检测mv命令:D1：置“1”：发生文件/目录重命名命令操作
                //现在只检测指令而不管该指令是否是否生效
                if( strstr(cmd_buf,"mv ") != NULL)//"mv "添加空格确保是完整指令
                {
                    dangercmd.cmd = SEC_CMD_MV;
                    SEC_FMT_DEBUG("检测到危险命令:mv\n");
                    sec_dangercmd_send(&dangercmd);
                }
    
                //检测cp命令:D2：置“1”：发生文件/目录复制命令操作
                //现在只检测指令而不管该指令是否是否生效
                if( strstr(cmd_buf,"cp ") != NULL)//"cp "添加空格确保是完整指令
                {
                    dangercmd.cmd = SEC_CMD_CP;
                    SEC_FMT_DEBUG("检测到危险命令:cp\n");
                    sec_dangercmd_send(&dangercmd);
                }
                  
                //检测kill pkill命令D3：置“1”：发生文件/目录进程结束命令操作
                //现在只检测指令而不管该指令是否是否生效
                if( strstr(cmd_buf,"kill ") != NULL)//"kill "添加空格确保是完整指令(kill 可以检测pkill这里)
                {
                    dangercmd.cmd = SEC_CMD_KILL;
                    SEC_FMT_DEBUG("检测到危险命令:kill/pkill\n");
                    sec_dangercmd_send(&dangercmd);
                }
            }
    
            //记忆本次分析的位置
            history_offset_old = ftell(fp_history);
        }
    
        fclose(fp_history);
    }

    return 0;
}

/**********************************************************************
* @name      : sec_pathchanged_check
* @brief     ：关键文件目录变更检测
* @param[in] ：void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_pathchanged_check(void *pPublic, void *pPrivate)
{
    char  dir_path[50] = {"/usr/local/extapps/"};
    KEY_DIR_FILE_VECTOR file_vector_de;//要探测的两个目录
    KEY_DIR_FILE_VECTOR file_vector;
    static int  keydir_init_flag = {0};
    int ret = 0;
    int       file_num = 0;
    int       i=0,j=0;
    char keypath[64] = {0};
    SEC_PATHCHG_T pathchg = {0};
    
    memset((char*)&file_vector, 0, sizeof(KEY_DIR_FILE_VECTOR));
    
    /**搜寻关键目录下的文件,通过比较当前文件的数目修改时间模式等参数,确定关键目录是否发生了变化**/

    SEC_FMT_TRACE("--%s--\n",dir_path);
    memset((char*)&file_vector_de, 0, sizeof(KEY_DIR_FILE_VECTOR));
    memset((char*)&file_vector, 0, sizeof(KEY_DIR_FILE_VECTOR));

    //获取上轮的文件属性信息
    ret = read_pdata_xram(TMN_SECURITY_DETECT, (char*)&file_vector_de, KeyDirChangeInfoAddr, sizeof(KEY_DIR_FILE_VECTOR));
    if(ret<0 || file_vector_de.filenum == 0xff)
        file_vector_de.filenum = 0;

    SEC_FMT_TRACE("记录中的文件个数 = %d\n",file_vector_de.filenum);
    
    //获取当前目录文件属性信息
    file_num = sec_dirfileattr_get(dir_path, &file_vector ,KEY_MAX_FILE_NUM);
    file_vector.filenum = file_num;

    SEC_FMT_TRACE("探测到的文件个数 = %d\n",file_vector.filenum);
    
    if( keydir_init_flag==1 )  //仅在初始化后检测事件,防止上电记录为空时检测到变化
    {
        //上次的文件是否有删除修改的
        for(i=0; i<file_vector_de.filenum; i++)
        {   
            //动态库文件(libxxxx.so),模式和名字一样就认为找到了对应的文件:时间可能会被系统自动修改
            if( (strstr(file_vector_de.file_vector[i].name,"lib") != NULL) && 
                (strstr(file_vector_de.file_vector[i].name,".so") != NULL) )
            {
                //遍历当前所有文件寻找对应文件
                for(j=0; j<file_vector.filenum; j++)
                {
                    if( file_vector_de.file_vector[i].mode == file_vector.file_vector[j].mode && 
                        (strcmp(file_vector_de.file_vector[i].name,file_vector.file_vector[j].name)==0)  )
                    {
                        break;
                    }
                }
            } 
            //非动态库文件(libxxxx.so),所有的参数必须一致才认为找到了对应的文件
            else 
            {
                //遍历当前所有文件寻找对应文件
                for(j=0; j<file_vector.filenum; j++)
                {
                    if( file_vector_de.file_vector[i].mode == file_vector.file_vector[j].mode &&
                        file_vector_de.file_vector[i].mtime == file_vector.file_vector[j].mtime && 
                        (strcmp(file_vector_de.file_vector[i].name,file_vector.file_vector[j].name)==0)  )
                    {
                        break;
                    }
                }
            }

            //判断是否找到了对应的文件,未找到时记录这个文件的名字
            if( j==file_vector.filenum )
            {
                memset(keypath, 0, sizeof(keypath));
                strcpy(keypath, dir_path);                              //目录
                strcat(keypath, file_vector_de.file_vector[i].name);    //文件名
            
                SEC_FMT_DEBUG("关键目录文件变更%s\n",keypath);
                pathchg.nNum = 1;
                pathchg.path[0] = keypath;
                sec_pathchanged_send(&pathchg);
            }
        }

        //本次的文件是否有新添加的
        for(i=0; i<file_vector.filenum; i++)
        {   
            //动态库文件(libxxxx.so),模式和名字一样就认为找到了对应的文件:时间可能会被系统自动修改
            if( (strstr(file_vector.file_vector[i].name,"lib") != NULL) && 
                (strstr(file_vector.file_vector[i].name,".so") != NULL) )
            {
                //遍历上轮所有文件寻找对应文件
                for(j=0; j<file_vector_de.filenum; j++)
                {
                    if( file_vector_de.file_vector[j].mode == file_vector.file_vector[i].mode &&
                        (strcmp(file_vector_de.file_vector[j].name,file_vector.file_vector[i].name)==0)  )
                    {
                        break;
                    }
                }
            }
            //非动态库文件(libxxxx.so),所有的参数必须一致才认为找到了对应的文件
            else
            {
                //遍历上轮所有文件寻找对应文件
                for(j=0; j<file_vector_de.filenum; j++)
                {
                    if( file_vector_de.file_vector[j].mode == file_vector.file_vector[i].mode &&
                        file_vector_de.file_vector[j].mtime == file_vector.file_vector[i].mtime && 
                        (strcmp(file_vector_de.file_vector[j].name,file_vector.file_vector[i].name)==0)  )
                    {
                        break;
                    }
                }
            }

            //判断是否找到了对应的文件,未找到时记录这个文件的名字
            if( j==file_vector_de.filenum)
            {
                memset(keypath, 0, sizeof(keypath));
                strcpy(keypath, dir_path);                      //目录
                strcat(keypath, file_vector.file_vector[i].name);    //文件名
            
                SEC_FMT_DEBUG("关键目录文件变更%s\n",keypath);
                pathchg.nNum = 1;
                pathchg.path[0] = keypath;
                sec_pathchanged_send(&pathchg);                    
            }   
        }

        //记录本轮文件
        write_pdata_xram(TMN_SECURITY_DETECT, (char*)&file_vector, KeyDirChangeInfoAddr, sizeof(KEY_DIR_FILE_VECTOR));
    }
    else
    {
        //还没初始化,存储当前读取到的文件,避免所有的文件都是新增的情况产生事件
        keydir_init_flag = 1;
        write_pdata_xram(TMN_SECURITY_DETECT, (char*)&file_vector, KeyDirChangeInfoAddr, sizeof(KEY_DIR_FILE_VECTOR));
    }

    return 0;
}

/**********************************************************************
* @name      : sec_pwdchanged_check
* @brief     ：口令信息变更检测
* @param[in] ：void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_pwdchanged_check(void *pPublic, void *pPrivate)
{
    static struct stat passwd_stat_old;
    struct stat passwd_stat_new;
    SEC_PWDCHG_T pwdchg = {.usrname="root"};

    /**密码变更检测直接查看存储密码的文件是否被修改**/
    if( stat("/etc/passwd", &passwd_stat_new) != -1)
    {
        //如果文件修改的时间和当前时间差在1min内，说明最近一分钟文件发生了变化
        if( (time(NULL) - passwd_stat_new.st_mtime) < 60)
        {
            if(passwd_stat_new.st_mtime != passwd_stat_old.st_mtime)
            {
                SEC_FMT_DEBUG("%s口令信息变更\n",pwdchg.usrname);
                sec_pwdchanged_send(&pwdchg);
            }
        }
        
        passwd_stat_old = passwd_stat_new;
    }

    return 0;
}

/**********************************************************************
* @name      : sec_ethportopen_check
* @brief     ：以太网远程服务端口开放信息监控
* @param[in] ：void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_ethportopen_check(void *pPublic, void *pPrivate)
{
    SEC_MGRDATA_T *ptMgrData = (SEC_MGRDATA_T *)pPrivate;
    uint8 cnt = ptMgrData->tcpInfoNum;
    POINT_INFO* netinfo = ptMgrData->tcpInfo;
    int i=0,j=0,ret=0;
    SEC_PORTOPEN_T  lstPort = {0};
    SEC_PORTOPEN_T  newPort = {0};
    SEC_PORTOPEN_T  rptPort = {0};
    
    if(0 == cnt)
    {
        return 0;
    }

    //读取上次端口记录
    ret = read_pdata_xram(TMN_SECURITY_DETECT, (char*)&lstPort, TcpRemoteSrvPortAddr, sizeof(SEC_PORTOPEN_T));
    if((ret != 0) || (lstPort.nNum == 0xff))
    {
        memset(&lstPort, 0, sizeof(SEC_PORTOPEN_T));
    }
    
    //过滤LISTEN状态且可外部访问的端口
    for(i = 0; i<cnt; i++)
    {
        if( strcmp(netinfo[i].state, "LISTEN") !=0 )        //仅LISTEN状态
        {
            continue;
        }

        if( strcmp(netinfo[i].local_ip, "0.0.0.0") != 0 )   //仅外部可访问的("0.0.0.0"->外部 "127.0.0.1"->内部)
        {    
            continue;
        }
        
        newPort.port[newPort.nNum++] = netinfo[i].local_port;
        
        if(newPort.nNum >= SEC_RECORD_MAX)
        {
            break;
        }
    }

    //监测到的端口若上一次端口记录中没有则上报
    for(i = 0; i < newPort.nNum; i++)
    {
        for(j=0; j< lstPort.nNum; j++)
        {
            if(newPort.port[i] == lstPort.port[j])
            {
                break;
            }
        }
            
        if(j == lstPort.nNum)
        {
            rptPort.port[rptPort.nNum++] = newPort.port[i];
            SEC_FMT_DEBUG("以太网端口打开: %d\n", rptPort.port[rptPort.nNum-1]);
            
            if(rptPort.nNum >= SEC_RECORD_MAX)
            {
                break;
            }
        }        
    }

    if(rptPort.nNum > 0)
    {
        sec_ethportopen_send(&rptPort);
    }

    //保存本次端口记录
    ret = write_pdata_xram(TMN_SECURITY_DETECT, (char*)&newPort, TcpRemoteSrvPortAddr, sizeof(SEC_PORTOPEN_T));

    return ret;
}

/**********************************************************************
* @name      : sec_ethconnect_check
* @brief     ：以太网外联
* @param[in] ：void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_ethconnect_check(void *pPublic, void *pPrivate)
{
    int i=0,j=0,ret=0;
    SEC_MGRDATA_T *ptMgrData = (SEC_MGRDATA_T *)pPrivate;
    uint8 cnt = ptMgrData->tcpInfoNum;
    POINT_INFO* netinfo = ptMgrData->tcpInfo;
    uint8 cchcnt = ptMgrData->connWhiteListNum;
    LINK_ADDR* cmp_addr = ptMgrData->connWhiteList;
    SEC_CONNECT_T  lstConn = {0};
    SEC_CONNECT_T  newConn = {0};
    SEC_CONNECT_T  rptConn = {0};
    
    if(NULL == cmp_addr || NULL == netinfo)
    {
        return -1;
    }

    if(0 == cnt)
    {
        return 0;
    }
    
    ret = read_pdata_xram(TMN_SECURITY_DETECT, (char*)&lstConn, TcpExtConnInfoAddr, sizeof(SEC_CONNECT_T));
    if((ret != 0) || (lstConn.nNum == 0xff))
    {
        memset(&lstConn, 0, sizeof(SEC_CONNECT_T));
    }

    /**新监测到的端口若上一次端口记录中没有则添加**/
    for(i = 0;i < cnt; i++)
    {
        if(strcmp(netinfo[i].state, "ESTABLISHED") != 0)
        {
            continue;
        }

        //过滤掉内部mqtt连接
        if((strcmp(netinfo[i].local_ip, "172.17.0.1") == 0) &&(netinfo[i].local_port == 1883))
        {
            continue;
        }

        if((strcmp(netinfo[i].foreign_ip, "172.17.0.1") == 0) &&(netinfo[i].foreign_port == 1883))
        {
            continue;
        }

        if( sec_is_illegal_addr(cmp_addr, cchcnt, netinfo[i]) != FALSE)
        {
            continue;
        }

        newConn.connect[newConn.nNum].port = netinfo[i].foreign_port;
        strcpy(newConn.connect[newConn.nNum].ip, netinfo[i].foreign_ip);
        newConn.nNum++;
        
        if(newConn.nNum >= SEC_RECORD_MAX)
        {
            break;
        }
    }

    //监测到的端口若上一次端口记录中没有则上报
    for(i = 0; i < newConn.nNum; i++)
    {
        for(j=0; j< lstConn.nNum; j++)
        {
            if(newConn.connect[i].port == lstConn.connect[j].port &&
                0 == strcmp(newConn.connect[i].ip, lstConn.connect[j].ip))
            {
                break;
            }
        }
            
        if(j == lstConn.nNum)
        {
            memcpy(&rptConn.connect[rptConn.nNum], &newConn.connect[i], sizeof(SEC_CONNINFO_T));
            rptConn.nNum++;
            
            SEC_FMT_DEBUG("以太网外联信息: %s:%d\n", rptConn.connect[rptConn.nNum-1].ip, rptConn.connect[rptConn.nNum-1].port);
            
            if(rptConn.nNum >= SEC_RECORD_MAX)
            {
                break;
            }
        }        
    }
 
    if(rptConn.nNum > 0)
    {
        sec_ethconnect_send(&rptConn);
    }
    
    write_pdata_xram(TMN_SECURITY_DETECT, (char*)&newConn, TcpExtConnInfoAddr, sizeof(SEC_CONNECT_T));

    return 0;
}

/**********************************************************************
* @name      : sec_ethlogin_check
* @brief     ：以太网登录控制台
* @param[in] ：void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_ethlogin_check(void *pPublic, void *pPrivate)
{
    int ret=0;
    int lstUsrNum = 0; //上次登录用户数
    int newUsrNum = 0; //本次登录用户数
    SEC_ETHLOGIN_T login = {0};

    //读取上次登录的
    ret = read_pdata_xram(TMN_SECURITY_DETECT, (char*)&lstUsrNum, TcpLoginTtyInfoAddr, sizeof(int));
    if((ret != 0) || (lstUsrNum == 0xffffffff))
    {
        memset(&lstUsrNum, 0, sizeof(int));
    }

    //读取登录的用户数
    newUsrNum = sec_loginusr_get(LOGIN_ETH);

    if(newUsrNum > lstUsrNum)
    {
        login.status = CON_LOGIN_IN;
        sec_ethlogin_send(&login);
        SEC_FMT_DEBUG("以太网登录控制台\n");
    }

    if(newUsrNum < lstUsrNum)
    {
        login.status = CON_LOGIN_OUT;
        sec_ethlogin_send(&login);
        SEC_FMT_DEBUG("以太网退出控制台\n");
    }

    //保存本次端口记录
    ret = write_pdata_xram(TMN_SECURITY_DETECT, (char*)&newUsrNum, TcpLoginTtyInfoAddr, sizeof(int));

    return ret;
}

/**********************************************************************
* @name      : sec_abnormallogin_check
* @brief     ：登录异常检测
* @param[in] ：void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_abnormallogin_check(void *pPublic, void *pPrivate)
{
    SEC_MGRDATA_T *ptMgrData = (SEC_MGRDATA_T *)pPrivate;
    FILE* fp;
    char cmd_buf[256] = {0};
    static int offset_old = 0;
    static SEC_FAILLOGIN_T failLogin[SEC_RECORD_MAX] = {0};
    int i = 0, j = 0;
    SEC_LOGINABNORMAL_T abnormal = {0};
    char *failstr = NULL;
    char usr_buf[256] = {0};    
    char ip_buf[256] = {0};
    char usrname[64] = {0};
    char loginip[32] = {0};
    SEC_ETHLOGIN_T login = {0};

    fp = fopen("/var/log/messages", "r");
    if(fp)
    {
        //未检测到以前的记录信息,重新获取
        if( offset_old==0 )
        {
            fseek(fp, 0, SEEK_END);
            offset_old = ftell(fp);
        }
        else //以前的记录存在,开始分析新产生的记录信息是否包含登录失败
        {
            //定位到上次分析的位置
            fseek(fp, offset_old, SEEK_SET);    
    
            //检测登录失败
            while( fgets(cmd_buf,sizeof(cmd_buf),fp) >0  )
            {
                SEC_FMT_TRACE("用户登录检测\n");
                
                //错误用户名“nonexistent user”
                if( strstr(cmd_buf,"Invalid user") != NULL)
                {
                    login.status = CON_LOGIN_FAIL;
                    SEC_FMT_DEBUG("以太网登录控制台失败：非法用户\n");
                    sec_ethlogin_send(&login);
                }
                
                //错误的密码“Bad password”
                failstr = strstr(cmd_buf,"Failed password for ");
                if( failstr != NULL)
                {
                    memcpy(usr_buf, &failstr[strlen("Failed password for ")], strlen(failstr)-strlen("Failed password for "));
                    sscanf(usr_buf, "%s", usrname);
                    failstr = strstr(usr_buf,"from");
                    memcpy(ip_buf, &failstr[strlen("from ")], strlen(failstr)-strlen("from "));
                    sscanf(ip_buf, "%s", loginip);

                    //查找是否已经有失败，有则累加次数
                    for(i = 0; i < SEC_RECORD_MAX; i++)
                    {
                        if((strcmp(usrname, failLogin[i].usrname) == 0) 
                            && (strcmp(loginip, failLogin[i].loginip) == 0))
                        {
                            failLogin[i].logincnt++;
                            SEC_FMT_DEBUG("密码错误次数--%d--\n", failLogin[i].logincnt);

                            if(failLogin[i].logincnt >= 20)
                            {
                                abnormal.type = ABNORMAL_BLAST_LOGIN;
                                strcpy(abnormal.usrname, failLogin[i].usrname);
                                strcpy(abnormal.loginip, failLogin[i].loginip);
                                sec_loginabnormal_send(&abnormal);
                                SEC_FMT_DEBUG("爆破登录: %s %s\n",usrname,loginip);
                                memset(&failLogin[i], 0, sizeof(SEC_FAILLOGIN_T));
                            }
                            break;
                        }
                    }

                    //如果没有查到记录，则存到空闲处
                    if(i == SEC_RECORD_MAX)
                    {
                        for(j = 0; j < SEC_RECORD_MAX; j++)
                        {
                            if(strlen(failLogin[j].usrname) == 0)
                            {
                                strcpy(failLogin[j].usrname, usrname);
                                strcpy(failLogin[j].loginip, loginip);
                                failLogin[j].logincnt++;
                                failLogin[j].failtime = ptMgrData->curtime;
                                login.status = CON_LOGIN_FAIL;
                                SEC_FMT_DEBUG("密码错误次数--%d--\n", failLogin[j].logincnt);
                                SEC_FMT_DEBUG("以太网登录控制台失败：错误密码\n");
                                sec_ethlogin_send(&login);
                                break;
                            }
                        }
                    }
                }
            }
    
            //记忆本次分析的位置
            offset_old = ftell(fp);
        }
    
        fclose(fp);
    }

    //清除已经过时的记录
    for(i = 0; i < SEC_RECORD_MAX; i++)
    {
        if((failLogin[i].logincnt > 0) && ((ptMgrData->curtime - failLogin[i].failtime) > 60))
        {
            memset(&failLogin[i], 0, sizeof(SEC_FAILLOGIN_T));
        }
    }
    
    return 0;
}

/**********************************************************************
* @name      : sec_abnormalssh_check
* @brief     ：SSH异常检测
* @param[in] ：void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_abnormalssh_check(void *pPublic, void *pPrivate)
{
    FILE* fp;
    char buf[512];   
    char cmd[128];   
    char *tmp;   
    static SEC_SSHINFO_T sshInfo = {.status=0xff,.conTime=0};
    SEC_SSHABNORMAL_T abnormal = {0};
    uint8 isOpen = FALSE;
        
    strcpy(cmd, "ps | grep sshd");
    fp = popen(cmd, "r");   
    if(NULL == fp)   
    {   
        SEC_FMT_DEBUG("popen error\n");
        return -2;   
    }
    
    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
        tmp =buf;
        while(*tmp && isspace(*tmp))
        {
            ++tmp;
        }

        if(strstr(tmp, "/usr/sbin/sshd") != NULL)
        {
            isOpen = TRUE;
            sshInfo.conTime += 5; 
            if(sshInfo.status != SEC_SSH_OPEN)
            {
                sshInfo.status = SEC_SSH_OPEN;
                abnormal.status = sshInfo.status;
                sec_sshabnormal_send(&abnormal);
                SEC_FMT_DEBUG("SSH服务打开\n");
            }

            if((g_sshtime_limit != 0) &&(sshInfo.conTime/60 > g_sshtime_limit))
            {
                sshInfo.conTime = 0;
                sshInfo.status = SEC_SSH_LONGTIMENOCLOSE;
                abnormal.status = sshInfo.status;
                sec_sshabnormal_send(&abnormal);
                
                SEC_FMT_DEBUG("SSH服务长时间未关闭\n");
            }
        }
    }

    //关闭时，持续时间要清零
    if((isOpen == FALSE) && (sshInfo.status != SEC_SSH_CLOSE))
    {
        sshInfo.conTime = 0;
        sshInfo.status = SEC_SSH_CLOSE;
        abnormal.status = sshInfo.status;
        sec_sshabnormal_send(&abnormal);
        SEC_FMT_DEBUG("SSH服务关闭\n");
    }
    
    pclose(fp);

    return 0;
}

/**********************************************************************
* @name      : sec_sessionflow_check
* @brief     ：会话流量异常监控
* @param[in] ：void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_sessionflow_check(void *pPublic, void *pPrivate)
{
    int ret=0;
    SEC_SESSIONFLOWS_T  abnormal = {0};
    uint32  monthFlow = 0;
    int curNum = 0;
    int i = 0;

    for(i = 0; i < g_session_info.nNum; i++)
    {
        monthFlow = g_session_info.info[i].monthFlow/1024;
        
        if((g_session_limit.monthFlowLimit != 0)
            &&(monthFlow > g_session_limit.monthFlowLimit))
        {
            curNum = abnormal.nNum++;

            strcpy(abnormal.flow[curNum].remoteIP, g_session_info.info[i].foreign_ip);
            abnormal.flow[curNum].remotePort = g_session_info.info[i].foreign_port;
            
            strcpy(abnormal.flow[curNum].localIP, g_session_info.info[i].local_ip);
            abnormal.flow[curNum].localPort = g_session_info.info[i].local_port;

            strcpy(abnormal.flow[curNum].protocol, g_session_info.info[i].protocol);

            
            strcpy(abnormal.flow[curNum].process, g_session_info.info[i].process);
            abnormal.flow[curNum].procType = g_session_info.info[i].procType;

            SEC_FMT_DEBUG("协议 %s 本地 %s:%d 远程 %s:%d 进程 %s 类型 %d 流量 %d KB 限流 %d KB\n", 
                abnormal.flow[curNum].protocol, abnormal.flow[curNum].localIP,abnormal.flow[curNum].localPort,
                abnormal.flow[curNum].remoteIP, abnormal.flow[curNum].remotePort, abnormal.flow[curNum].process, 
                abnormal.flow[curNum].procType, monthFlow, g_session_limit.monthFlowLimit);

            //报完后清零，防止重复上报
            g_session_info.info[i].monUpdate = TRUE;
            g_session_info.info[i].monthFlow = 0;
            g_session_info.info[i].monUpdate = FALSE;
        }
    }

    if(abnormal.nNum > 0)
    {
        sec_flowabnormal_send(&abnormal);
        SEC_FMT_DEBUG("会话流量异常\n");
    }
    
    return ret;
}

/**********************************************************************
* @name      : sec_sessionband_check
* @brief     ：会话带宽异常监控
* @param[in] ：void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_sessionband_check(void *pPublic, void *pPrivate)
{
    int ret=0;
    SEC_SESSIONBANDWIDTHS_T  abnormal = {0};
    int curNum = 0;
    int i = 0;

    for(i = 0; i < g_session_info.nNum; i++)
    {
        if((g_session_limit.flowSpeedLimit != 0)
            &&(g_session_info.info[i].flowSpeed > g_session_limit.flowSpeedLimit))
        {
            curNum = abnormal.nNum++;

            strcpy(abnormal.flow[curNum].remoteIP, g_session_info.info[i].foreign_ip);
            abnormal.flow[curNum].remotePort = g_session_info.info[i].foreign_port;
            
            strcpy(abnormal.flow[curNum].localIP, g_session_info.info[i].local_ip);
            abnormal.flow[curNum].localPort = g_session_info.info[i].local_port;

            strcpy(abnormal.flow[curNum].protocol, g_session_info.info[i].protocol);

            strcpy(abnormal.flow[curNum].process, g_session_info.info[i].process);
            
            abnormal.flow[curNum].flowLimit = g_session_limit.flowSpeedLimit;
            abnormal.flow[curNum].curFlow = g_session_info.info[i].flowSpeed;

            //报完后清零，防止重复上报
            g_session_info.info[i].flowSpeed = 0;

            SEC_FMT_DEBUG("协议 %s 本地 %s:%d 远程 %s:%d 进程 %s 流速 %d Kbps 限速 %d Kbps\n", 
                abnormal.flow[curNum].protocol, abnormal.flow[curNum].localIP,abnormal.flow[curNum].localPort,
                abnormal.flow[curNum].remoteIP, abnormal.flow[curNum].remotePort, abnormal.flow[curNum].process, 
                abnormal.flow[curNum].curFlow, g_session_limit.flowSpeedLimit);
        }
    }

    if(abnormal.nNum > 0)
    {
        sec_bandabnormal_send(&abnormal);
        SEC_FMT_DEBUG("会话带宽异常\n");
    }
    
    return ret;
}

/**********************************************************************
* @name      : sec_procabnormal_check
* @brief     ：进程异常监控
* @param[in] ：void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_abnormalproc_check(void *pPublic, void *pPrivate)
{
    int ret=0;
    int i = 0, j = 0;
    int newPidNum = 0;
    int   pids[65535];
    SEC_PROCABNORMALS_T abnormal = {0};
    int curNum = 0;

    //读取当前进程
    newPidNum = sec_procnum_get(pids, 65535);

    //查看当前进程是否为新增
    for(i = 0; i < newPidNum; i++)
    {
        for(j = 0; j < g_proc_info.nNum; j++)
        {
            if(pids[i] == g_proc_info.procinfo[j])
            {
                break;
            }
        }

        //没找到则是新增进程
        if(j == g_proc_info.nNum)
        {

            ret = sec_procinfo_get(pids[i], &abnormal.info[abnormal.nNum]);
            if(ret != 0)
            {
                continue;
            }

            //做一些过滤
            if(sec_procfilter_get(&abnormal.info[abnormal.nNum]) == FALSE)
            {
                continue;
            }
            
            curNum = abnormal.nNum++;
            abnormal.info[curNum].type = SEC_PROC_ADD;
            
            SEC_FMT_DEBUG("类型 %d 时间 %4d-%02d-%02d %02d:%02d:%02d 进程名 %s 用户 %s 用户组 %s 命令 %s\n", 
                abnormal.info[curNum].type, abnormal.info[curNum].stime.year, abnormal.info[curNum].stime.month, abnormal.info[curNum].stime.day,
                abnormal.info[curNum].stime.hour, abnormal.info[curNum].stime.minute, abnormal.info[curNum].stime.second,
                abnormal.info[curNum].name, abnormal.info[curNum].user, abnormal.info[curNum].group, abnormal.info[curNum].cmdline);
        }
     }

    if(abnormal.nNum > 0)
    {
        sec_procabnormal_send(&abnormal);
        SEC_FMT_DEBUG("进程异常\n");
    }

    //更新列表
    g_proc_info.nNum = newPidNum;
    memcpy(g_proc_info.procinfo, pids, newPidNum*sizeof(int));
    
    return ret;
}

#endif

#if DESC("线程入口",1)

/**********************************************************************
* @name      : sec_manager_init
* @brief     ：安全管理初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int sec_manager_init(void *pPublic, void **ppPrivate)
{
    int ret = 0;
    SEC_MGRDATA_T *ptData = NULL;
    
    ptData = (SEC_MGRDATA_T *)malloc(sizeof(SEC_MGRDATA_T));
    if(NULL == ptData)
    {
        SEC_FMT_DEBUG("malloc failed\n");
        return -1;
    }
    memset(ptData, 0, sizeof(SEC_MGRDATA_T));

    ret = reg_pdata_xram(APP_NAME, TMN_SECURITY_DETECT);
    if(ret != 0)
    {
        SEC_FMT_DEBUG("register \"%s\" file failed.\n", TMN_SECURITY_DETECT);
    }

    //初始化进程数量
    g_proc_info.nNum = sec_procnum_get(g_proc_info.procinfo, 65535);

    *ppPrivate = ptData;
    return 0;
}

/**********************************************************************
* @name      : sec_manager_proc
* @brief     ：安全管理处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_manager_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int ret = 0;
    SEC_MGRDATA_T *ptMgrData = (SEC_MGRDATA_T *)pPrivate;

    ptMgrData->curtime = get_time(TIME_MONOTONIC);

    /*5s检测一轮*/
    if(ptMgrData->curtime - ptMgrData->lsttime >= 5)
    {
        //获取以太网连接信息
        ptMgrData->tcpInfoNum = sec_tcpinfo_get(ptMgrData->tcpInfo, SEC_RECORD_MAX);
        
        //以太网远程端口开放监控
        sec_ethportopen_check(pPublic, pPrivate);
        
        //以太网外联信息监控
        sec_ethconnect_check(pPublic, pPrivate);
        
        //关键目录文件变更监控
        sec_pathchanged_check(pPublic, pPrivate);
        
        //以太网登录控制台监控
        sec_ethlogin_check(pPublic, pPrivate);

        //危险命令操作监控
        sec_dangercmd_check(pPublic, pPrivate);

        //SSH服务异常监控
        sec_abnormalssh_check(pPublic, pPrivate);

        //口令信息变更监控
        sec_pwdchanged_check(pPublic, pPrivate);
        
        //会话流量异常监控
        sec_sessionflow_check(pPublic, pPrivate);
        
        //会话带宽异常监控
        sec_sessionband_check(pPublic, pPrivate);
        
        //用户登录异常监控
        sec_abnormallogin_check(pPublic, pPrivate);

        //进程异常监控
        sec_abnormalproc_check(pPublic, pPrivate);
        
        //刷新会话APP映射
        sec_session_update();
        
        ptMgrData->lsttime = ptMgrData->curtime;
    }

    //流量统计
    sec_flow_stat(pArg, pPublic, pPrivate);
    
    sleep_s(1);
    
    return ret;
}

#endif

