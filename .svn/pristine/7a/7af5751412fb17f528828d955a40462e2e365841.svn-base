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
#include <pcap.h>
#include "sec_pcap.h"
#include <net/if.h>

extern SEC_SESSIONAPPS_T  g_session_info;

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
int sec_fivetuple_cmp(SEC_SESSIONAPP_T *session1, SEC_SESSIONAPP_T *session2)
{
    if(strcmp(session1->protocol, session2->protocol) != 0)
    {
        return -1;
    }

    if((strcmp(session1->local_ip, session2->local_ip) == 0)
        && (session1->local_port == session2->local_port)
        && (strcmp(session1->foreign_ip, session2->foreign_ip) == 0)
        && (session1->foreign_port == session2->foreign_port))
    {
        return 0;
    }

    if((strcmp(session1->foreign_ip, session2->local_ip) == 0)
        && (session1->foreign_port == session2->local_port)
        && (strcmp(session1->local_ip, session2->foreign_ip) == 0)
        && (session1->local_port == session2->foreign_port))
    {
        return 0;
    }

    return -1;
}

void sec_pcap_callback(unsigned char * arg,const struct pcap_pkthdr *packet_header,const unsigned char *packet_content)
{
    SEC_ETH_HDR_T *ethernet = NULL;
    SEC_IP_HDR_T *ip = NULL;
    SEC_TCP_HDR_T *tcp = NULL;
    SEC_UDP_HDR_T *udp = NULL;
    SEC_SESSIONAPP_T session = {0};
    int flowcnt = 0;    
    const uint8* flowptr = NULL;
    int i = 0;


    uint32 eth_len=sizeof(SEC_ETH_HDR_T);
    uint32 udp_len=sizeof(SEC_UDP_HDR_T);

    ethernet=(SEC_ETH_HDR_T *)packet_content;
    
    if(ntohs(ethernet->eth_type) == 0x0800)
    {
        ip=(SEC_IP_HDR_T*)(packet_content+eth_len);
        
        if(ip->version == 4)
        {
            sprintf(session.local_ip,   "%d.%d.%d.%d", ip->srcIP[0],ip->srcIP[1],ip->srcIP[2],ip->srcIP[3]);
            sprintf(session.foreign_ip, "%d.%d.%d.%d", ip->dstIP[0],ip->dstIP[1],ip->dstIP[2],ip->dstIP[3]);

            if(ip->protocol==6)
            {
                tcp=(SEC_TCP_HDR_T*)(packet_content+eth_len+ip->header_len*4);
                
                flowptr = packet_content+eth_len+ip->header_len*4+tcp->head_len*4;
                flowcnt = ntohs(ip->total_len)-ip->header_len*4-tcp->head_len*4;

                session.local_port = ntohs(tcp->sport);
                session.foreign_port = ntohs(tcp->dport);
                strcpy(session.protocol, "TCP");

                SEC_BUF_TRACE(flowptr, flowcnt, "协议:%s 源:%s:%d 目的:%s:%d 内容:\n", 
                    session.protocol, session.local_ip,session.local_port,
                    session.foreign_ip, session.foreign_port);
            }
            else if(ip->protocol==17)
            {
                udp=(SEC_UDP_HDR_T*)(packet_content+eth_len+ip->header_len*4);

                flowptr = packet_content+eth_len+ip->header_len*4+udp_len;
                flowcnt = ntohs(ip->total_len)-ip->header_len*4-udp_len;

                session.local_port = ntohs(udp->sport);
                session.foreign_port = ntohs(udp->dport);
                strcpy(session.protocol, "UDP");

                SEC_BUF_TRACE(flowptr, flowcnt, "协议:%s 源:%s:%d 目的:%s:%d 内容:\n", 
                    session.protocol, session.local_ip,session.local_port,
                    session.foreign_ip, session.foreign_port);

            }

            if((ip->protocol==6) || (ip->protocol==17))
            {
                //将流量刷新到会话列表
                for(i = 0; i < g_session_info.nNum; i++)
                {
                    if(sec_fivetuple_cmp(&session, &g_session_info.info[i]) == 0)
                    {
                        break;
                    }
                }

                //没找到流则刷新列表后再匹配一次
                if(i == g_session_info.nNum)
                {
                    sec_session_update();

                    for(i = 0; i < g_session_info.nNum; i++)
                    {
                        if(sec_fivetuple_cmp(&session, &g_session_info.info[i]) == 0)
                        {
                            break;
                        }
                    }
                }

                //找到流后刷新流信息
                if(i < g_session_info.nNum)
                {
                    while(g_session_info.info[i].calcUpdate);
                    g_session_info.info[i].calcFlow +=  flowcnt;
                    while(g_session_info.info[i].monUpdate);
                    g_session_info.info[i].monthFlow +=  flowcnt;
                    g_session_info.info[i].updateTime = get_time(TIME_MONOTONIC);

                    SEC_FMT_DEBUG("进程 %s 月累积流量 %dB 流速流量 %dB\n", g_session_info.info[i].process, g_session_info.info[i].monthFlow, g_session_info.info[i].calcFlow);
                }
            }
        }
    }
}

/**********************************************************************
* @name      : sec_localip_get
* @brief     ：获取本机IP
* @param[in] ：char *ifname 接口名
* @param[out]：
* @return    ：点分十进制IP地址字符串
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
char* sec_localip_get(char *ifname)
{                                                                                           
    int sock;
    struct sockaddr_in sin;
    struct ifreq ifr;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0); 
    if (sock == -1)
    {
        perror("socket");
        return NULL;
    }
    
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0) 
    {
        perror("ioctl");
        return NULL;
    }   

    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    return inet_ntoa(sin.sin_addr);
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
int sec_packetcap_init(void *pPublic, void **ppPrivate)
{
//    int ret = 0;
//    SEC_MGRDATA_T *ptData = NULL;
//    
//    ptData = (SEC_MGRDATA_T *)malloc(sizeof(SEC_MGRDATA_T));
//    if(NULL == ptData)
//    {
//        SEC_FMT_DEBUG("malloc failed\n");
//        return -1;
//    }
//    memset(ptData, 0, sizeof(SEC_MGRDATA_T));
//
//    *ppPrivate = ptData;
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
int sec_packetcap_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int ret = 0;
    char ifname[32]="FE0",errbuf[1024] = {0};
    char *hostIP = NULL;
    struct bpf_program filter = {0};

    pcap_t *pcap_handle=pcap_open_live(ifname,65535,1,0,errbuf);
    if(pcap_handle==NULL)
    {
        SEC_FMT_DEBUG("pcap_open_live failed, %s\n",errbuf);
        return 0;
    }
    
    hostIP = sec_localip_get(ifname);

    sprintf(errbuf, "not port 8888 and host %s", hostIP);

    ret = pcap_compile(pcap_handle,&filter,errbuf,1,0);
    if(ret < 0)
    {
        SEC_FMT_DEBUG("pcap_compile failed, ret %d\n", ret);
        return 0;
    }

    ret = pcap_setfilter(pcap_handle,&filter);
    if(ret < 0)
    {
        SEC_FMT_DEBUG("pcap_setfilter failed, ret %d\n", ret);
        return 0;
    }

    ret = pcap_loop(pcap_handle,-1,sec_pcap_callback,NULL);
    if(ret < 0)
    {
        SEC_FMT_DEBUG("pcap_loop failed, ret %d\n", ret);
        return 0;
    }

    pcap_close(pcap_handle);
        
    return ret;
}

#endif

