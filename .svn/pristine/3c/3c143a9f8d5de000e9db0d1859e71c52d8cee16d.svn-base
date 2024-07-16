/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控消息处理
* @date：    2020-6-30
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef __SEC_PCAP_H
#define __SEC_PCAP_H
    
#ifdef __cplusplus
        extern "C" {
#endif

//以太网帧头
typedef struct
{
    uint8 dst_mac[6];
    uint8 src_mac[6];
    uint16 eth_type;
}SEC_ETH_HDR_T;

//IP报文头
typedef struct
{
    int header_len:4; //同一个字节，低字节在前，高字节在后
    int version:4;
    uint8 tos:8;
    int total_len:16;
    int ident:16;
    int flags:16;
    uint8 ttl:8;
    uint8 protocol:8;
    int checksum:16;
    uint8 srcIP[4];
    uint8 dstIP[4];
}SEC_IP_HDR_T;

//TCP报文头
typedef struct
{
    uint16 sport;
    uint16 dport;
    uint32 seq;
    uint32 ack;    
    uint8 reserved:4;
    uint8 head_len:4;
    uint8 flags;
    uint16 wind_size;
    uint16 check_sum;
    uint16 urg_ptr;
}SEC_TCP_HDR_T;

//UDP报文头
typedef struct
{
    uint16 sport;
    uint16 dport;
    uint16 tot_len;
    uint16 check_sum;
}SEC_UDP_HDR_T;

/* 线程私有数据 */
typedef struct tag_SEC_PCAPDATA
{
    uint8         appStep;
    uint8         res[3];
    time_t        timeflag;  //定时查询标记
    int           tasksocket;
}SEC_PCAPDATA_T;

#ifdef __cplusplus
}
#endif

#endif


