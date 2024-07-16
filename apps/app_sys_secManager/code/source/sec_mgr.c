/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��·���mqtt
* @date��    2020-6-30
* @history�� 
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
SEC_SESSIONAPPS_T  g_session_info = {0}; //�����Ự��Ϣ
OOP_DATETIME_S_T   g_currtime;     /*��ǰʱ��,ȫ��ʱ�����*/
OOP_DATETIME_S_T   g_lasttime;     /*��ǰʱ��,ȫ��ʱ�����*/
uint32 g_sshtime_limit = 0;        //Ĭ��0����
SEC_PROCINFOS_T g_proc_info = {0}; //������Ϣ
/*����Ϊʱ�����,����,��ʱ,����,���£�������*/
uint8   pastminflag;
uint8   pasthourflag;
uint8   pastdayflag;
uint8   pastmonflag;
uint8   pastyearflag;

#if DESC("����֧�ֹ���",1)

//��ȡָ��Ŀ¼���ļ�����
int sec_dirfileattr_get(char* dir, KEY_DIR_FILE_VECTOR* fileattri ,int maxfp)
{
    int  num = 0;
    int  ret = 0;
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    char file_name[300] = {0};//�ļ�����·��+�ļ���

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
                fileattri->file_vector[num].mode  = statbuf.st_mode;        //Ȩ��
                fileattri->file_vector[num].mtime = statbuf.st_mtime;       //�޸�ʱ��
                if(strlen(entry->d_name) < KEY_MAX_FILE_LEN)
                {
                    strcat(fileattri->file_vector[num].name, entry->d_name);//�ļ���
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

//��̫��Զ�̷���˿��Լ�������Ϣ���
//������̫���˵���Ϣ
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

    char local_addr[50]     ={0};//IP��PORT ��ʽ xx.xx.xx.xx:xxxx
    char local_ip[20]       ={0};
    int  local_port     = 0;

    char foreign_addr[50]    ={0};//IP��PORT ��ʽ xx.xx.xx.xx:xxxx
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
* @brief     ����ȡ��¼�û���
* @param[in] ��loginType  ��¼���� @ref SEC_LOGINTYPE_E
* @param[out]��
* @return    ����¼�û�����
* @Create    : ����
* @Date      ��2020-6-30
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
* @brief     ����ȡSSH��ǰ״̬
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int sec_sshstatus_get(void)
{
    return 0;
}

/**********************************************************************
* @name      : sec_sessionapp_get
* @brief     ����ȡ�����Ự����APP
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-30
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

    //��ȡ����IP
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
        SEC_FMT_TRACE("����IP:%s\n", hostIP);
    }
    pclose(fp);

    //��ȡ�����б�
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
        //��ȡ����IP
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
            SEC_FMT_TRACE("���� %s IP:%s\n", containerList[i], containerIP);
        }
        pclose(fp);

        //��ȡ��������Ԫ����Ϣ
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
* @brief     ���Ƚ������Ự�Ƿ����
* @param[in] ��
* @param[out]��
* @return    ��=0 ��ȣ��������
* @Create    : ����
* @Date      ��2020-6-30
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
* @brief     ��ˢ�������Ự����APP
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-30
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

    //��ȡ��ǰӳ��
    mapNum = sec_session_get(curmap, SEC_RECORD_MAX);

    //ɾ���Ѿ������ڵ�����
    for(i = 0; i < g_session_info.nNum; i++)
    {
        for(j = 0; j < mapNum; j++)
        {
            if(sec_session_cmp(&g_session_info.info[i], &curmap[j]) == 0)
            {
                break;
            }
        }

        //û�ҵ���ɾ��
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

    //����µ�����
    for(i = 0; i < mapNum; i++)
    {
        for(j = 0; j < g_session_info.nNum; j++)
        {
            if(sec_session_cmp(&curmap[i], &g_session_info.info[j]) == 0)
            {
                break;
            }
        }

        //û�ҵ������
        if(j == g_session_info.nNum)
        {
            if(g_session_info.nNum < SEC_RECORD_MAX) //���п�����ֱ�����
            {
                memcpy(&g_session_info.info[g_session_info.nNum], &curmap[i], sizeof(SEC_SESSIONAPP_T));
                g_session_info.nNum++;
            }
            else //�����滻��ͻ�Ծ�ȻỰ
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
* @brief��     ��ȡϵͳʵʱʱ��
* @param[in]�� rectify ��������
* @param[out]��pTime ϵͳʱ��
* @return��     void
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
* @brief��      ��ʱ��Ǹ���
* @param[in]�� NA
* @param[out]��NA
* @return��     void
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
* @brief     ������ͳ��
* @param[in] ��void *pArg     �߳̾��
               void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int sec_flow_stat(void *pArg, void *pPublic, void *pPrivate)
{
    int i = 0;

    //��ȡ��ǰ����ʱ��
    sec_datetime_get(0, &g_currtime);
    //���ֹ��յ��ж�
    sec_spantime_update();

    //���ּ�������
    if(pastminflag)
    {
        for(i = 0; i < g_session_info.nNum; i++)
        {
            g_session_info.info[i].flowSpeed = g_session_info.info[i].calcFlow*8/60/1024; //��λKbps
            g_session_info.info[i].calcUpdate = TRUE;
            g_session_info.info[i].calcFlow = 0;
            g_session_info.info[i].calcUpdate = FALSE;
            SEC_FMT_DEBUG("��ǰ�ۻ�����%dB ���� %d Kbps\n", g_session_info.info[i].calcFlow, g_session_info.info[i].flowSpeed);
        }
    }
    
    //������������ۻ�����
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
* @brief     ����ȡϵͳ��ǰ��������
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-30
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
                if(pid > 299) //����299�Ĳ����û�����
                {
                    procinfo[procNum++] = pid;
                }
            }
            else
            {
                SEC_FMT_DEBUG("������������ %d\n", maxlen);
                break;
            }
        }
        
        closedir(dirp);
    }

    return procNum;
}

/**********************************************************************
* @name      : sec_procinfo_get
* @brief     ����ȡϵͳ��ǰ������Ϣ
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-30
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

    //��ȡ��������ʱ��
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

    //��ȡ�����û�
    sprintf(path, "/proc/%d/status", pid);
    fp = fopen(path, "r");
    if(fp != NULL)
    {
        while( fgets(cmd_buf,sizeof(cmd_buf),fp) > 0)
        {
            //��ȡ������
            if( (pStr = strstr(cmd_buf,"Name:")) != NULL)
            {
                if(strlen(pStr+strlen("Name:")) > sizeof(procinfo->name)-1)
                {
                    fclose(fp);
                    SEC_FMT_DEBUG("���������� %d>%d\n", strlen(pStr+strlen("Name:")), sizeof(procinfo->name)-1);
                    return -1;
                }
                
                sscanf(pStr+strlen("Name:"), "%s", procinfo->name);
                continue;
            }
            
            //��ȡUid
            if( (pStr = strstr(cmd_buf,"Uid:")) != NULL)
            {
                sscanf(pStr+strlen("Uid:"), "%d", &uid);
                pwd = getpwuid(uid);
                strcpy(procinfo->user, pwd->pw_name);
                continue;
            }

             //��ȡGid
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
    
    //��ȡ����cmd
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
* @brief     ����ȡϵͳ��ǰ������Ϣ
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
BOOL sec_procfilter_get(SEC_PROCABNORMAL_T* procinfo)
{
    //������Ĳ���
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

#if DESC("��ȫ���߼��",1)
/**********************************************************************
* @name      : sec_dangercmd_check
* @brief     ��Σ��������
* @param[in] ��void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int sec_dangercmd_check(void *pPublic, void *pPrivate)
{
    /**Σ�ղ�����Ϣ���ͨ��history������**/
    //history����¼�������ظ�ָ��, ����ζ���������������������ͬ�ķǷ�����ָ��,��ʹ��һ�ε�ָ�����ܾúܾò�����,Ҳ���������Ӧ�İ�ȫ�¼�
    //Ŀǰ�������������Ҫ��,�����Ҫ����,��Ҫ����ϵͳ�������֧��(��Ϊ����������ͬ����history�����¼,����.ash_history�ļ��������޸�ʱ�䶼�������,�����޷����)
    FILE* fp_history;
    char cmd_buf[256] = {0};    //historyָ��buf,ָ����󳤶��ݶ�256
    static int history_offset_old = 0;
    SEC_DANGERCMD_T dangercmd;

    fp_history = fopen("/home/sysadm/.ash_history", "r");
    
    if( fp_history )
    {
        //δ��⵽��ǰ�ļ�¼��Ϣ,���»�ȡ
        if( history_offset_old==0 )
        {
            fseek(fp_history, 0, SEEK_END);
            history_offset_old = ftell(fp_history);
        }
        //��ǰ�ļ�¼����,��ʼ�����²����ļ�¼��Ϣ�Ƿ����Σ�ղ���ָ��
        else
        {
            //��λ���ϴη�����λ��
            fseek(fp_history, history_offset_old, SEEK_SET);    
    
            //���Σ��ָ��
            while( fgets(cmd_buf,sizeof(cmd_buf),fp_history) >0  )
            {
                SEC_FMT_TRACE("Σ������������\n");
    
                //���rm����:D0���á�1���������ļ�/Ŀ¼ɾ���������
                //����ֻ���ָ������ܸ�ָ���Ƿ��Ƿ���Ч
                if( strstr(cmd_buf,"rm ") != NULL)//"rm "��ӿո�ȷ��������ָ��
                {
                    dangercmd.cmd = SEC_CMD_RM;
                    SEC_FMT_DEBUG("��⵽Σ������:rm\n");
                    sec_dangercmd_send(&dangercmd);
                }
    
                //���mv����:D1���á�1���������ļ�/Ŀ¼�������������
                //����ֻ���ָ������ܸ�ָ���Ƿ��Ƿ���Ч
                if( strstr(cmd_buf,"mv ") != NULL)//"mv "��ӿո�ȷ��������ָ��
                {
                    dangercmd.cmd = SEC_CMD_MV;
                    SEC_FMT_DEBUG("��⵽Σ������:mv\n");
                    sec_dangercmd_send(&dangercmd);
                }
    
                //���cp����:D2���á�1���������ļ�/Ŀ¼�����������
                //����ֻ���ָ������ܸ�ָ���Ƿ��Ƿ���Ч
                if( strstr(cmd_buf,"cp ") != NULL)//"cp "��ӿո�ȷ��������ָ��
                {
                    dangercmd.cmd = SEC_CMD_CP;
                    SEC_FMT_DEBUG("��⵽Σ������:cp\n");
                    sec_dangercmd_send(&dangercmd);
                }
                  
                //���kill pkill����D3���á�1���������ļ�/Ŀ¼���̽����������
                //����ֻ���ָ������ܸ�ָ���Ƿ��Ƿ���Ч
                if( strstr(cmd_buf,"kill ") != NULL)//"kill "��ӿո�ȷ��������ָ��(kill ���Լ��pkill����)
                {
                    dangercmd.cmd = SEC_CMD_KILL;
                    SEC_FMT_DEBUG("��⵽Σ������:kill/pkill\n");
                    sec_dangercmd_send(&dangercmd);
                }
            }
    
            //���䱾�η�����λ��
            history_offset_old = ftell(fp_history);
        }
    
        fclose(fp_history);
    }

    return 0;
}

/**********************************************************************
* @name      : sec_pathchanged_check
* @brief     ���ؼ��ļ�Ŀ¼������
* @param[in] ��void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int sec_pathchanged_check(void *pPublic, void *pPrivate)
{
    char  dir_path[50] = {"/usr/local/extapps/"};
    KEY_DIR_FILE_VECTOR file_vector_de;//Ҫ̽�������Ŀ¼
    KEY_DIR_FILE_VECTOR file_vector;
    static int  keydir_init_flag = {0};
    int ret = 0;
    int       file_num = 0;
    int       i=0,j=0;
    char keypath[64] = {0};
    SEC_PATHCHG_T pathchg = {0};
    
    memset((char*)&file_vector, 0, sizeof(KEY_DIR_FILE_VECTOR));
    
    /**��Ѱ�ؼ�Ŀ¼�µ��ļ�,ͨ���Ƚϵ�ǰ�ļ�����Ŀ�޸�ʱ��ģʽ�Ȳ���,ȷ���ؼ�Ŀ¼�Ƿ����˱仯**/

    SEC_FMT_TRACE("--%s--\n",dir_path);
    memset((char*)&file_vector_de, 0, sizeof(KEY_DIR_FILE_VECTOR));
    memset((char*)&file_vector, 0, sizeof(KEY_DIR_FILE_VECTOR));

    //��ȡ���ֵ��ļ�������Ϣ
    ret = read_pdata_xram(TMN_SECURITY_DETECT, (char*)&file_vector_de, KeyDirChangeInfoAddr, sizeof(KEY_DIR_FILE_VECTOR));
    if(ret<0 || file_vector_de.filenum == 0xff)
        file_vector_de.filenum = 0;

    SEC_FMT_TRACE("��¼�е��ļ����� = %d\n",file_vector_de.filenum);
    
    //��ȡ��ǰĿ¼�ļ�������Ϣ
    file_num = sec_dirfileattr_get(dir_path, &file_vector ,KEY_MAX_FILE_NUM);
    file_vector.filenum = file_num;

    SEC_FMT_TRACE("̽�⵽���ļ����� = %d\n",file_vector.filenum);
    
    if( keydir_init_flag==1 )  //���ڳ�ʼ�������¼�,��ֹ�ϵ��¼Ϊ��ʱ��⵽�仯
    {
        //�ϴε��ļ��Ƿ���ɾ���޸ĵ�
        for(i=0; i<file_vector_de.filenum; i++)
        {   
            //��̬���ļ�(libxxxx.so),ģʽ������һ������Ϊ�ҵ��˶�Ӧ���ļ�:ʱ����ܻᱻϵͳ�Զ��޸�
            if( (strstr(file_vector_de.file_vector[i].name,"lib") != NULL) && 
                (strstr(file_vector_de.file_vector[i].name,".so") != NULL) )
            {
                //������ǰ�����ļ�Ѱ�Ҷ�Ӧ�ļ�
                for(j=0; j<file_vector.filenum; j++)
                {
                    if( file_vector_de.file_vector[i].mode == file_vector.file_vector[j].mode && 
                        (strcmp(file_vector_de.file_vector[i].name,file_vector.file_vector[j].name)==0)  )
                    {
                        break;
                    }
                }
            } 
            //�Ƕ�̬���ļ�(libxxxx.so),���еĲ�������һ�²���Ϊ�ҵ��˶�Ӧ���ļ�
            else 
            {
                //������ǰ�����ļ�Ѱ�Ҷ�Ӧ�ļ�
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

            //�ж��Ƿ��ҵ��˶�Ӧ���ļ�,δ�ҵ�ʱ��¼����ļ�������
            if( j==file_vector.filenum )
            {
                memset(keypath, 0, sizeof(keypath));
                strcpy(keypath, dir_path);                              //Ŀ¼
                strcat(keypath, file_vector_de.file_vector[i].name);    //�ļ���
            
                SEC_FMT_DEBUG("�ؼ�Ŀ¼�ļ����%s\n",keypath);
                pathchg.nNum = 1;
                pathchg.path[0] = keypath;
                sec_pathchanged_send(&pathchg);
            }
        }

        //���ε��ļ��Ƿ�������ӵ�
        for(i=0; i<file_vector.filenum; i++)
        {   
            //��̬���ļ�(libxxxx.so),ģʽ������һ������Ϊ�ҵ��˶�Ӧ���ļ�:ʱ����ܻᱻϵͳ�Զ��޸�
            if( (strstr(file_vector.file_vector[i].name,"lib") != NULL) && 
                (strstr(file_vector.file_vector[i].name,".so") != NULL) )
            {
                //�������������ļ�Ѱ�Ҷ�Ӧ�ļ�
                for(j=0; j<file_vector_de.filenum; j++)
                {
                    if( file_vector_de.file_vector[j].mode == file_vector.file_vector[i].mode &&
                        (strcmp(file_vector_de.file_vector[j].name,file_vector.file_vector[i].name)==0)  )
                    {
                        break;
                    }
                }
            }
            //�Ƕ�̬���ļ�(libxxxx.so),���еĲ�������һ�²���Ϊ�ҵ��˶�Ӧ���ļ�
            else
            {
                //�������������ļ�Ѱ�Ҷ�Ӧ�ļ�
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

            //�ж��Ƿ��ҵ��˶�Ӧ���ļ�,δ�ҵ�ʱ��¼����ļ�������
            if( j==file_vector_de.filenum)
            {
                memset(keypath, 0, sizeof(keypath));
                strcpy(keypath, dir_path);                      //Ŀ¼
                strcat(keypath, file_vector.file_vector[i].name);    //�ļ���
            
                SEC_FMT_DEBUG("�ؼ�Ŀ¼�ļ����%s\n",keypath);
                pathchg.nNum = 1;
                pathchg.path[0] = keypath;
                sec_pathchanged_send(&pathchg);                    
            }   
        }

        //��¼�����ļ�
        write_pdata_xram(TMN_SECURITY_DETECT, (char*)&file_vector, KeyDirChangeInfoAddr, sizeof(KEY_DIR_FILE_VECTOR));
    }
    else
    {
        //��û��ʼ��,�洢��ǰ��ȡ�����ļ�,�������е��ļ�������������������¼�
        keydir_init_flag = 1;
        write_pdata_xram(TMN_SECURITY_DETECT, (char*)&file_vector, KeyDirChangeInfoAddr, sizeof(KEY_DIR_FILE_VECTOR));
    }

    return 0;
}

/**********************************************************************
* @name      : sec_pwdchanged_check
* @brief     ��������Ϣ������
* @param[in] ��void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int sec_pwdchanged_check(void *pPublic, void *pPrivate)
{
    static struct stat passwd_stat_old;
    struct stat passwd_stat_new;
    SEC_PWDCHG_T pwdchg = {.usrname="root"};

    /**���������ֱ�Ӳ鿴�洢������ļ��Ƿ��޸�**/
    if( stat("/etc/passwd", &passwd_stat_new) != -1)
    {
        //����ļ��޸ĵ�ʱ��͵�ǰʱ�����1min�ڣ�˵�����һ�����ļ������˱仯
        if( (time(NULL) - passwd_stat_new.st_mtime) < 60)
        {
            if(passwd_stat_new.st_mtime != passwd_stat_old.st_mtime)
            {
                SEC_FMT_DEBUG("%s������Ϣ���\n",pwdchg.usrname);
                sec_pwdchanged_send(&pwdchg);
            }
        }
        
        passwd_stat_old = passwd_stat_new;
    }

    return 0;
}

/**********************************************************************
* @name      : sec_ethportopen_check
* @brief     ����̫��Զ�̷���˿ڿ�����Ϣ���
* @param[in] ��void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
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

    //��ȡ�ϴζ˿ڼ�¼
    ret = read_pdata_xram(TMN_SECURITY_DETECT, (char*)&lstPort, TcpRemoteSrvPortAddr, sizeof(SEC_PORTOPEN_T));
    if((ret != 0) || (lstPort.nNum == 0xff))
    {
        memset(&lstPort, 0, sizeof(SEC_PORTOPEN_T));
    }
    
    //����LISTEN״̬�ҿ��ⲿ���ʵĶ˿�
    for(i = 0; i<cnt; i++)
    {
        if( strcmp(netinfo[i].state, "LISTEN") !=0 )        //��LISTEN״̬
        {
            continue;
        }

        if( strcmp(netinfo[i].local_ip, "0.0.0.0") != 0 )   //���ⲿ�ɷ��ʵ�("0.0.0.0"->�ⲿ "127.0.0.1"->�ڲ�)
        {    
            continue;
        }
        
        newPort.port[newPort.nNum++] = netinfo[i].local_port;
        
        if(newPort.nNum >= SEC_RECORD_MAX)
        {
            break;
        }
    }

    //��⵽�Ķ˿�����һ�ζ˿ڼ�¼��û�����ϱ�
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
            SEC_FMT_DEBUG("��̫���˿ڴ�: %d\n", rptPort.port[rptPort.nNum-1]);
            
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

    //���汾�ζ˿ڼ�¼
    ret = write_pdata_xram(TMN_SECURITY_DETECT, (char*)&newPort, TcpRemoteSrvPortAddr, sizeof(SEC_PORTOPEN_T));

    return ret;
}

/**********************************************************************
* @name      : sec_ethconnect_check
* @brief     ����̫������
* @param[in] ��void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
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

    /**�¼�⵽�Ķ˿�����һ�ζ˿ڼ�¼��û�������**/
    for(i = 0;i < cnt; i++)
    {
        if(strcmp(netinfo[i].state, "ESTABLISHED") != 0)
        {
            continue;
        }

        //���˵��ڲ�mqtt����
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

    //��⵽�Ķ˿�����һ�ζ˿ڼ�¼��û�����ϱ�
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
            
            SEC_FMT_DEBUG("��̫��������Ϣ: %s:%d\n", rptConn.connect[rptConn.nNum-1].ip, rptConn.connect[rptConn.nNum-1].port);
            
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
* @brief     ����̫����¼����̨
* @param[in] ��void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int sec_ethlogin_check(void *pPublic, void *pPrivate)
{
    int ret=0;
    int lstUsrNum = 0; //�ϴε�¼�û���
    int newUsrNum = 0; //���ε�¼�û���
    SEC_ETHLOGIN_T login = {0};

    //��ȡ�ϴε�¼��
    ret = read_pdata_xram(TMN_SECURITY_DETECT, (char*)&lstUsrNum, TcpLoginTtyInfoAddr, sizeof(int));
    if((ret != 0) || (lstUsrNum == 0xffffffff))
    {
        memset(&lstUsrNum, 0, sizeof(int));
    }

    //��ȡ��¼���û���
    newUsrNum = sec_loginusr_get(LOGIN_ETH);

    if(newUsrNum > lstUsrNum)
    {
        login.status = CON_LOGIN_IN;
        sec_ethlogin_send(&login);
        SEC_FMT_DEBUG("��̫����¼����̨\n");
    }

    if(newUsrNum < lstUsrNum)
    {
        login.status = CON_LOGIN_OUT;
        sec_ethlogin_send(&login);
        SEC_FMT_DEBUG("��̫���˳�����̨\n");
    }

    //���汾�ζ˿ڼ�¼
    ret = write_pdata_xram(TMN_SECURITY_DETECT, (char*)&newUsrNum, TcpLoginTtyInfoAddr, sizeof(int));

    return ret;
}

/**********************************************************************
* @name      : sec_abnormallogin_check
* @brief     ����¼�쳣���
* @param[in] ��void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
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
        //δ��⵽��ǰ�ļ�¼��Ϣ,���»�ȡ
        if( offset_old==0 )
        {
            fseek(fp, 0, SEEK_END);
            offset_old = ftell(fp);
        }
        else //��ǰ�ļ�¼����,��ʼ�����²����ļ�¼��Ϣ�Ƿ������¼ʧ��
        {
            //��λ���ϴη�����λ��
            fseek(fp, offset_old, SEEK_SET);    
    
            //����¼ʧ��
            while( fgets(cmd_buf,sizeof(cmd_buf),fp) >0  )
            {
                SEC_FMT_TRACE("�û���¼���\n");
                
                //�����û�����nonexistent user��
                if( strstr(cmd_buf,"Invalid user") != NULL)
                {
                    login.status = CON_LOGIN_FAIL;
                    SEC_FMT_DEBUG("��̫����¼����̨ʧ�ܣ��Ƿ��û�\n");
                    sec_ethlogin_send(&login);
                }
                
                //��������롰Bad password��
                failstr = strstr(cmd_buf,"Failed password for ");
                if( failstr != NULL)
                {
                    memcpy(usr_buf, &failstr[strlen("Failed password for ")], strlen(failstr)-strlen("Failed password for "));
                    sscanf(usr_buf, "%s", usrname);
                    failstr = strstr(usr_buf,"from");
                    memcpy(ip_buf, &failstr[strlen("from ")], strlen(failstr)-strlen("from "));
                    sscanf(ip_buf, "%s", loginip);

                    //�����Ƿ��Ѿ���ʧ�ܣ������ۼӴ���
                    for(i = 0; i < SEC_RECORD_MAX; i++)
                    {
                        if((strcmp(usrname, failLogin[i].usrname) == 0) 
                            && (strcmp(loginip, failLogin[i].loginip) == 0))
                        {
                            failLogin[i].logincnt++;
                            SEC_FMT_DEBUG("����������--%d--\n", failLogin[i].logincnt);

                            if(failLogin[i].logincnt >= 20)
                            {
                                abnormal.type = ABNORMAL_BLAST_LOGIN;
                                strcpy(abnormal.usrname, failLogin[i].usrname);
                                strcpy(abnormal.loginip, failLogin[i].loginip);
                                sec_loginabnormal_send(&abnormal);
                                SEC_FMT_DEBUG("���Ƶ�¼: %s %s\n",usrname,loginip);
                                memset(&failLogin[i], 0, sizeof(SEC_FAILLOGIN_T));
                            }
                            break;
                        }
                    }

                    //���û�в鵽��¼����浽���д�
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
                                SEC_FMT_DEBUG("����������--%d--\n", failLogin[j].logincnt);
                                SEC_FMT_DEBUG("��̫����¼����̨ʧ�ܣ���������\n");
                                sec_ethlogin_send(&login);
                                break;
                            }
                        }
                    }
                }
            }
    
            //���䱾�η�����λ��
            offset_old = ftell(fp);
        }
    
        fclose(fp);
    }

    //����Ѿ���ʱ�ļ�¼
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
* @brief     ��SSH�쳣���
* @param[in] ��void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
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
                SEC_FMT_DEBUG("SSH�����\n");
            }

            if((g_sshtime_limit != 0) &&(sshInfo.conTime/60 > g_sshtime_limit))
            {
                sshInfo.conTime = 0;
                sshInfo.status = SEC_SSH_LONGTIMENOCLOSE;
                abnormal.status = sshInfo.status;
                sec_sshabnormal_send(&abnormal);
                
                SEC_FMT_DEBUG("SSH����ʱ��δ�ر�\n");
            }
        }
    }

    //�ر�ʱ������ʱ��Ҫ����
    if((isOpen == FALSE) && (sshInfo.status != SEC_SSH_CLOSE))
    {
        sshInfo.conTime = 0;
        sshInfo.status = SEC_SSH_CLOSE;
        abnormal.status = sshInfo.status;
        sec_sshabnormal_send(&abnormal);
        SEC_FMT_DEBUG("SSH����ر�\n");
    }
    
    pclose(fp);

    return 0;
}

/**********************************************************************
* @name      : sec_sessionflow_check
* @brief     ���Ự�����쳣���
* @param[in] ��void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
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

            SEC_FMT_DEBUG("Э�� %s ���� %s:%d Զ�� %s:%d ���� %s ���� %d ���� %d KB ���� %d KB\n", 
                abnormal.flow[curNum].protocol, abnormal.flow[curNum].localIP,abnormal.flow[curNum].localPort,
                abnormal.flow[curNum].remoteIP, abnormal.flow[curNum].remotePort, abnormal.flow[curNum].process, 
                abnormal.flow[curNum].procType, monthFlow, g_session_limit.monthFlowLimit);

            //��������㣬��ֹ�ظ��ϱ�
            g_session_info.info[i].monUpdate = TRUE;
            g_session_info.info[i].monthFlow = 0;
            g_session_info.info[i].monUpdate = FALSE;
        }
    }

    if(abnormal.nNum > 0)
    {
        sec_flowabnormal_send(&abnormal);
        SEC_FMT_DEBUG("�Ự�����쳣\n");
    }
    
    return ret;
}

/**********************************************************************
* @name      : sec_sessionband_check
* @brief     ���Ự�����쳣���
* @param[in] ��void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
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

            //��������㣬��ֹ�ظ��ϱ�
            g_session_info.info[i].flowSpeed = 0;

            SEC_FMT_DEBUG("Э�� %s ���� %s:%d Զ�� %s:%d ���� %s ���� %d Kbps ���� %d Kbps\n", 
                abnormal.flow[curNum].protocol, abnormal.flow[curNum].localIP,abnormal.flow[curNum].localPort,
                abnormal.flow[curNum].remoteIP, abnormal.flow[curNum].remotePort, abnormal.flow[curNum].process, 
                abnormal.flow[curNum].curFlow, g_session_limit.flowSpeedLimit);
        }
    }

    if(abnormal.nNum > 0)
    {
        sec_bandabnormal_send(&abnormal);
        SEC_FMT_DEBUG("�Ự�����쳣\n");
    }
    
    return ret;
}

/**********************************************************************
* @name      : sec_procabnormal_check
* @brief     �������쳣���
* @param[in] ��void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
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

    //��ȡ��ǰ����
    newPidNum = sec_procnum_get(pids, 65535);

    //�鿴��ǰ�����Ƿ�Ϊ����
    for(i = 0; i < newPidNum; i++)
    {
        for(j = 0; j < g_proc_info.nNum; j++)
        {
            if(pids[i] == g_proc_info.procinfo[j])
            {
                break;
            }
        }

        //û�ҵ�������������
        if(j == g_proc_info.nNum)
        {

            ret = sec_procinfo_get(pids[i], &abnormal.info[abnormal.nNum]);
            if(ret != 0)
            {
                continue;
            }

            //��һЩ����
            if(sec_procfilter_get(&abnormal.info[abnormal.nNum]) == FALSE)
            {
                continue;
            }
            
            curNum = abnormal.nNum++;
            abnormal.info[curNum].type = SEC_PROC_ADD;
            
            SEC_FMT_DEBUG("���� %d ʱ�� %4d-%02d-%02d %02d:%02d:%02d ������ %s �û� %s �û��� %s ���� %s\n", 
                abnormal.info[curNum].type, abnormal.info[curNum].stime.year, abnormal.info[curNum].stime.month, abnormal.info[curNum].stime.day,
                abnormal.info[curNum].stime.hour, abnormal.info[curNum].stime.minute, abnormal.info[curNum].stime.second,
                abnormal.info[curNum].name, abnormal.info[curNum].user, abnormal.info[curNum].group, abnormal.info[curNum].cmdline);
        }
     }

    if(abnormal.nNum > 0)
    {
        sec_procabnormal_send(&abnormal);
        SEC_FMT_DEBUG("�����쳣\n");
    }

    //�����б�
    g_proc_info.nNum = newPidNum;
    memcpy(g_proc_info.procinfo, pids, newPidNum*sizeof(int));
    
    return ret;
}

#endif

#if DESC("�߳����",1)

/**********************************************************************
* @name      : sec_manager_init
* @brief     ����ȫ�����ʼ��
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-07-09
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

    //��ʼ����������
    g_proc_info.nNum = sec_procnum_get(g_proc_info.procinfo, 65535);

    *ppPrivate = ptData;
    return 0;
}

/**********************************************************************
* @name      : sec_manager_proc
* @brief     ����ȫ������
* @param[in] ��void *pArg     �߳̾��
               void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int sec_manager_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int ret = 0;
    SEC_MGRDATA_T *ptMgrData = (SEC_MGRDATA_T *)pPrivate;

    ptMgrData->curtime = get_time(TIME_MONOTONIC);

    /*5s���һ��*/
    if(ptMgrData->curtime - ptMgrData->lsttime >= 5)
    {
        //��ȡ��̫��������Ϣ
        ptMgrData->tcpInfoNum = sec_tcpinfo_get(ptMgrData->tcpInfo, SEC_RECORD_MAX);
        
        //��̫��Զ�̶˿ڿ��ż��
        sec_ethportopen_check(pPublic, pPrivate);
        
        //��̫��������Ϣ���
        sec_ethconnect_check(pPublic, pPrivate);
        
        //�ؼ�Ŀ¼�ļ�������
        sec_pathchanged_check(pPublic, pPrivate);
        
        //��̫����¼����̨���
        sec_ethlogin_check(pPublic, pPrivate);

        //Σ������������
        sec_dangercmd_check(pPublic, pPrivate);

        //SSH�����쳣���
        sec_abnormalssh_check(pPublic, pPrivate);

        //������Ϣ������
        sec_pwdchanged_check(pPublic, pPrivate);
        
        //�Ự�����쳣���
        sec_sessionflow_check(pPublic, pPrivate);
        
        //�Ự�����쳣���
        sec_sessionband_check(pPublic, pPrivate);
        
        //�û���¼�쳣���
        sec_abnormallogin_check(pPublic, pPrivate);

        //�����쳣���
        sec_abnormalproc_check(pPublic, pPrivate);
        
        //ˢ�»ỰAPPӳ��
        sec_session_update();
        
        ptMgrData->lsttime = ptMgrData->curtime;
    }

    //����ͳ��
    sec_flow_stat(pArg, pPublic, pPrivate);
    
    sleep_s(1);
    
    return ret;
}

#endif

