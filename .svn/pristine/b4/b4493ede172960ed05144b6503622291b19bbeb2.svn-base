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

#include <linux/connector.h>
#define _LINUX_TIME_H
#include <linux/cn_proc.h>

#define  MAX_MSGSIZE 256
#ifndef  SOL_NETLINK
#define  SOL_NETLINK 270
#endif

int sd = -1;  //套接字描述符
struct nlmsghdr *nlhdr = NULL;
struct sigaction sigint_action;
struct sockaddr_nl l_local, daddr;
struct iovec iov;
struct msghdr msg;
struct cn_msg * cnmsg;

#if DESC("公共支持功能",1)

/**********************************************************************
* @name      : sec_fivetuple_cmp
* @brief     ：比较两个会话五元组是否相等
* @param[in] ：
* @param[out]：
* @return    ：=0 相等，否则不相等
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
void change_cn_proc_mode(int mode)
{
    int ret;
    int * connector_mode;
    
    memset(nlhdr, 0, sizeof(NLMSG_SPACE(MAX_MSGSIZE)));
    memset(&iov, 0, sizeof(struct iovec));
    memset(&msg, 0, sizeof(struct msghdr));
    
    cnmsg = (struct cn_msg *)NLMSG_DATA(nlhdr);
    connector_mode = (int *)cnmsg->data;
    * connector_mode = mode;

    nlhdr->nlmsg_len = NLMSG_LENGTH(sizeof(struct cn_msg) + sizeof(enum proc_cn_mcast_op));
    nlhdr->nlmsg_pid = getpid();
    nlhdr->nlmsg_flags = 0;
    nlhdr->nlmsg_type = NLMSG_DONE;
    nlhdr->nlmsg_seq = 0;

    cnmsg->id.idx = CN_IDX_PROC;
    cnmsg->id.val = CN_VAL_PROC;
    cnmsg->seq = 0;
    cnmsg->ack = 0;
    cnmsg->len = sizeof(enum proc_cn_mcast_op);

    iov.iov_base = (void *)nlhdr;
    iov.iov_len = nlhdr->nlmsg_len;
    msg.msg_name = (void *)&daddr;
    msg.msg_namelen = sizeof(daddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    
    ret = sendmsg(sd, &msg, 0);
    if (ret == -1) 
    {
        SEC_FMT_DEBUG("sendmsg error:\n");
        exit(-1);
    }
 }

void sigint_handler(int signo)
{
    change_cn_proc_mode(PROC_CN_MCAST_IGNORE);
    SEC_FMT_DEBUG("process event: turn off process event listening.\n");
    close(sd);
    exit(0);
}

void getNameByPid(pid_t pid,char*task_name)
{
    FILE *fp = NULL;
    char proc_pid_path[128];
    char buf[1024];

    sprintf(proc_pid_path,"/proc/%d/status",pid);

    fp = fopen(proc_pid_path,"r");
    if(NULL != fp)
    {
        if(fgets(buf,1024-1,fp) != NULL)
        {
            sscanf(buf,"%*s%s",task_name);
        }

        fclose(fp);
    }
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
int sec_procmonitor_init(void *pPublic, void **ppPrivate)
{
    memset(&sigint_action, 0, sizeof(struct sigaction));

    sigint_action.sa_flags = SA_ONESHOT;
    sigint_action.sa_handler = &sigint_handler;
    sigaction(SIGINT, &sigint_action, NULL);

    nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSGSIZE));
    if(nlhdr == NULL) 
    {
        SEC_FMT_DEBUG("malloc:\n");
        exit(-1);
    }

    daddr.nl_family = AF_NETLINK;
    daddr.nl_pid = 0;
    daddr.nl_groups = CN_IDX_PROC;

    sd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);

    l_local.nl_family = AF_NETLINK;
    l_local.nl_groups = CN_IDX_PROC;
    l_local.nl_pid = getpid();

    if (bind(sd, (struct sockaddr *)&l_local, sizeof(struct sockaddr_nl)) == -1)
    {
        SEC_FMT_DEBUG("bind\n");
        close(sd);
        return -1;
    }

    change_cn_proc_mode(PROC_CN_MCAST_LISTEN);
    SEC_FMT_DEBUG("process event: turn on process event listening.\n");

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
int sec_procmonitor_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int ret = 0;
    struct proc_event * procevent;
    char ppName[APP_NAME_MAX] = {0};    
    char ptName[APP_NAME_MAX] = {0};
    char pName[APP_NAME_MAX] = {0};
    char tName[APP_NAME_MAX] = {0};
    
    memset(nlhdr, 0, NLMSG_SPACE(MAX_MSGSIZE));
    memset(&iov, 0, sizeof(struct iovec));
    memset(&msg, 0, sizeof(struct msghdr));

    iov.iov_base = (void *)nlhdr;
    iov.iov_len = NLMSG_SPACE(MAX_MSGSIZE);
    msg.msg_name = (void *)&daddr;
    msg.msg_namelen = sizeof(daddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    
    ret = recvmsg(sd, &msg, 0);
    if (ret == 0) 
    {
        SEC_FMT_DEBUG("Exit.\n");
        exit(0);
    }
    else if (ret == -1) 
    {
        SEC_FMT_DEBUG("recvmsg:\n");
        exit(1);
    }
    else 
    {
        cnmsg = (struct cn_msg *)NLMSG_DATA(nlhdr);
        procevent = (struct proc_event *)cnmsg->data;
        switch (procevent->what) 
        {
            case PROC_EVENT_NONE:
            {
                SEC_FMT_DEBUG("process event: acknowledge for turning on processevent listening\n");
            }break;
                
            case PROC_EVENT_FORK:
            {
                getNameByPid(procevent->event_data.fork.parent_pid,ppName);
                getNameByPid(procevent->event_data.fork.parent_tgid,ptName);
                getNameByPid(procevent->event_data.fork.child_pid,pName);
                getNameByPid(procevent->event_data.fork.child_tgid,tName);
                
                SEC_FMT_DEBUG("process event: fork\n");
                SEC_FMT_DEBUG("parent(pid:%s, tgid:%s)-->child(pid:%s,tgid:%s)\n",
                    ppName,ptName,pName,tName);
            }break;
                
            case PROC_EVENT_EXEC:
            {
                getNameByPid(procevent->event_data.exec.process_pid,pName);
                getNameByPid(procevent->event_data.exec.process_tgid,tName);
                SEC_FMT_DEBUG("process event: exec\n");
                SEC_FMT_DEBUG("pid:%s, tgid:%s\n",pName,tName);
            }break;
            
//            case PROC_EVENT_UID:
//            {
//                SEC_FMT_DEBUG("process event: uid\n");
//                SEC_FMT_DEBUG("process tid:%d, pid:%d, uid:%d->%d\n",
//                    procevent->event_data.id.process_pid,
//                    procevent->event_data.id.process_tgid,
//                    procevent->event_data.id.r.ruid,
//                    procevent->event_data.id.e.euid);
//            }break;
//            
//            case PROC_EVENT_GID:
//            {
//                SEC_FMT_DEBUG("process event: gid\n");
//                SEC_FMT_DEBUG("process tid:%d, pid:%d, uid:%d->%d\n",
//                    procevent->event_data.id.process_pid,
//                    procevent->event_data.id.process_tgid,
//                    procevent->event_data.id.r.rgid,
//                    procevent->event_data.id.e.egid);
//            }break;
//            
//            case PROC_EVENT_EXIT:
//            {
//                SEC_FMT_DEBUG("process event: exit\n");
//                SEC_FMT_DEBUG("tid:%d, pid:%d, exit code:%d\n",
//                    procevent->event_data.exit.process_pid,
//                    procevent->event_data.exit.process_tgid,
//                    procevent->event_data.exit.exit_code);
//            }break;
            
            default:
            {
                //SEC_FMT_DEBUG("Unkown process action\n");
            }break;
        }
    }

    return ret;
}

#endif

