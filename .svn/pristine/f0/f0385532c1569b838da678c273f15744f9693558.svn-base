/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块PPP操作
* @date：    2019-10-24
* @history： 
*********************************************************************
*/

#include "baseCommon.h"
#include <net/if.h> /*网络ifi_xxx结构定义*/
#include "message.h"
#include "appmsg_struct.h"

#include "wireless.h"
#include "ppp_oper.h"

/*网卡数据流量结构*/
typedef struct tagIfStats{
  uint64 rx_bytes;        /*接收数据字节数*/
  uint32 rx_packets;      /*接收数据包数*/
  uint32 rx_errors;       
  uint32 rx_dropped;
  uint32 rx_fifo_errors;
  uint32 rx_frame_errors;
  uint32 rx_compressed;
  uint32 rx_multicast;
  
  uint64 tx_bytes;        /*发送数据字节数*/
  uint32 tx_packets;      /*发送数据包数*/
  uint32 tx_errors;
  uint32 tx_dropped;
  uint32 tx_fifo_errors;
  uint32 collisions;
  uint32 tx_carrier_errors;
  uint32 tx_compressed;
}IF_STATE_T;

/**********************************************************************
* @name      : wireless_check_pppif
* @brief     ：判断无线网口是否存在
* @param[in] ：uint8 ifnum 端口编号
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-10-24
* @Update    : 2020-12-17
**********************************************************************/
BOOLEAN wireless_check_pppif(uint8 ifnum)
{
    int fd;
    struct ifreq ifr; 
    char ucPPPName[8] = {0};
    int ret = 0;

    sprintf(ucPPPName, "ppp%d", ifnum);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return 0;
    }

    strcpy(ifr.ifr_name, ucPPPName);
    ret = ioctl(fd, SIOCGIFFLAGS, &ifr);
    close(fd);
    return 0 == ret ? TRUE : FALSE;
}

/**********************************************************************
* @name      : wireless_check_ppp_porc
* @brief     ：判断ppp进程是否存在
* @param[in] ：uint8 ifnum 端口编号
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-12-18
* @Update    :
**********************************************************************/
BOOLEAN wireless_check_ppp_porc(uint8 ifnum)
{
    FILE *fp;
    pid_t pid = 0;
    char ucSystem[256] = {0};

    sprintf(ucSystem, "ps -aux|grep \"pppd debug unit %d\"|grep -v grep|awk '{print $2}'", ifnum);
    fp = popen(ucSystem, "r");
    if(NULL != fp)
    {
        fscanf(fp, "%d", &pid); 
        pclose(fp);
    }
    return pid > 0 ? TRUE : FALSE;
}

/**********************************************************************
* @name      : wireless_ppp_off
* @brief     ：关闭PPP端口与进程
* @param[in] ：uint8 ifnum 端口编号
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-10-24
* @Update    : 2020-12-17
**********************************************************************/
void wireless_ppp_off(uint8 ifnum)
{
    FILE *fp;
    pid_t pid = 0;
    char ucPid[24] = {0};
    char ucSystem[256] = {0};

    sprintf(ucPid, "/var/run/ppp%d.pid", ifnum);

    fp = fopen(ucPid, "r");
    if(fp)
    {
        fscanf(fp, "%d", &pid); 
        fclose(fp);
        WIRELESS_FMT_DEBUG("获取ppp%d 进程id %d\n", ifnum, pid);
    }
    else
    {
        /* 防止拨号执行一半 没有生成pid文件 */
        sprintf(ucSystem, "ps -aux|grep \"pppd debug unit %d\"|grep -v grep|awk '{print $2}'", ifnum);
        fp = popen(ucSystem, "r");
        if(fp)
        {
            fscanf(fp, "%d", &pid); 
            pclose(fp);
            if(pid > 0)
            {
                WIRELESS_FMT_DEBUG("命令行获取ppp%d 进程id %d!\n", ifnum, pid);
            }
        }
        if(0 == pid)
        {
            memset(ucSystem, 0, 256);
            sprintf(ucSystem, "ps |grep \"pppd debug unit %d\"|grep -v grep|awk '{print $1}'", ifnum);
            fp = popen(ucSystem, "r");
            if(fp)
            {
                fscanf(fp, "%d", &pid); 
                pclose(fp);
                if(pid > 0)
                {
                    WIRELESS_FMT_DEBUG("命令行获取ppp%d 进程id %d!\n", ifnum, pid);
                }
            }
        }
    }

    if(pid > 0)
    {
        kill(pid, SIGINT);
        WIRELESS_FMT_DEBUG("ppp%d 进程id %d, 停止\n", ifnum, pid);
        sleep(1);

        pid = 0;
        memset(ucSystem, 0, 256);
        sprintf(ucSystem, "ps -aux|grep \"pppd debug unit %d\"|grep -v grep|awk '{print $2}'", ifnum);
        fp = popen(ucSystem, "r");
        if(fp)
        {
            fscanf(fp, "%d", &pid); 
            pclose(fp);
        }
        if(0 == pid)
        {
            memset(ucSystem, 0, 256);
            sprintf(ucSystem, "ps |grep \"pppd debug unit %d\"|grep -v grep|awk '{print $1}'", ifnum);
            fp = popen(ucSystem, "r");
            if(fp)
            {
                fscanf(fp, "%d", &pid); 
                pclose(fp);
            }
        }
        if(pid > 0)
        {
            /* kill -9，SIGKILL信号不会被进程所“截获”，它只能由主机系统内核处理 */
            memset(ucSystem, 0, 256);
            sprintf(ucSystem, "kill -9 %d", pid);
            system(ucSystem);
            WIRELESS_FMT_DEBUG("ppp%d 进程%d停止失败,kill -9 %d\n", ifnum, pid, pid);
            sleep(1);
        }
    }

    if (remove(ucPid) != 0)
    {
        memset(ucSystem, 0, 256);
        sprintf(ucSystem, "rm -f /var/run/ppp%d.pid", ifnum);
        system(ucSystem);
    }
    
    sleep(1);
}

/*获取PPP网口的IP地址*/
uint32 wireless_get_pppIP(uint8 ifnum)
{
    int fd;
    struct ifreq ifr; 
    char ucPPPName[8] = {0};
    uint32 ipAddr = 0;

    sprintf(ucPPPName, "ppp%d", ifnum);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return 0;
    }

    strcpy(ifr.ifr_name, ucPPPName);
    if(0 == ioctl(fd, SIOCGIFFLAGS, &ifr))
    {
        if(0 == ioctl(fd, SIOCGIFADDR, &ifr))
        {
            ipAddr = ((struct sockaddr_in*)(&(ifr.ifr_addr)))->sin_addr.s_addr;
            ipAddr = ntohl(ipAddr);
        }
    }

    close(fd);
    return ipAddr;
}

/*获取PPP网口的MASK地址*/
uint32 wireless_get_pppMASK(uint8 ifnum)
{
    int fd;
    struct ifreq ifr; 
    char ucPPPName[8] = {0};
    uint32 ipMASK = 0;

    sprintf(ucPPPName, "ppp%d", ifnum);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return 0;
    }

    strcpy(ifr.ifr_name, ucPPPName);
    if(0 == ioctl(fd, SIOCGIFFLAGS, &ifr))
    {
        if(0 == ioctl(fd, SIOCGIFNETMASK, &ifr))
        {
            memcpy(&ipMASK, &(((struct sockaddr_in*)(&(ifr.ifr_addr)))->sin_addr), sizeof(uint32));
        }
    }

    close(fd);
    return ipMASK;
}


/*获取PPP网口的网关地址*/
uint32 wireless_get_pppgetway(uint8 ifnum)
{
    FILE *fp;
    char ucSystem[256] = {0};
    char ucIP[24] = {0};
    uint32 Addr = 0;

#if 0
    /* 从ifconfig中获取 */
    sprintf(ucSystem, "ifconfig ppp%d|grep \"inet addr\"|awk '{print $3}'", ifnum);
    fp = popen(ucSystem, "r");
    if(NULL != fp)
    {
        fscanf(fp, "%s", ucIP);
        WIRELESS_FMT_TRACE("Get ppp%d getway: %s\n", ifnum, ucIP);
        pclose(fp);
        Addr = inet_addr(ucIP + strlen("P-t-P:"));
    }
    
#else
    /* 从route -n中获取 */
    sprintf(ucSystem, "route -n|grep ppp%d|grep -v \"UH\"|grep -v \"255\"|awk '{print $2}'", ifnum);
    fp = popen(ucSystem, "r");
    if(NULL != fp)
    {
        fscanf(fp, "%s", ucIP);
        //WIRELESS_FMT_TRACE("网口ppp%d网关%s\n", ifnum, ucIP);
        pclose(fp);
        if(ucIP[0] == 0)
        {
            return 0;
        }
        Addr = inet_addr(ucIP);
    }
#endif

    return ntohl(Addr);
}

int wireless_get_ifflux(char *ifName, uint64 *rxbytes, uint64 *txbytes)
{
    char *temp;
    FILE *fh;
    char buf[512];
    IF_STATE_T tIfState = {0};

    if (NULL == ifName || NULL == rxbytes || NULL == txbytes)
    {
    	return -1;
    }
    fh = fopen("/proc/net/dev", "r"); /*网络设备路径*/
    if (!fh)
    {
        WIRELESS_FMT_TRACE("%s fopen PATH_PROCNET_DEV", ifName);
        return -1;
    }

    fgets(buf, sizeof buf, fh);
    fgets(buf, sizeof buf, fh);

    while (fgets(buf, sizeof buf, fh))
    {
        if (strstr(buf, ifName))
        {
          	temp = strstr(buf, ":");
          	if (temp)
          	{
                sscanf(temp+1,
                "%llu %u %u %u %u %u %u %u %llu %u %u %u %u %u %u %u",
                &tIfState.rx_bytes,
                &tIfState.rx_packets,
                &tIfState.rx_errors,
                &tIfState.rx_dropped,
                &tIfState.rx_fifo_errors,
                &tIfState.rx_frame_errors,
                &tIfState.rx_compressed,
                &tIfState.rx_multicast,

                &tIfState.tx_bytes,
                &tIfState.tx_packets,
                &tIfState.tx_errors,
                &tIfState.tx_dropped,
                &tIfState.tx_fifo_errors,
                &tIfState.collisions,
                &tIfState.tx_carrier_errors,
                &tIfState.tx_compressed);
            }
        }
    }
    fclose(fh);
    *rxbytes = tIfState.rx_bytes;
    *txbytes = tIfState.tx_bytes;
    return 0;
}

/*获取PPP网口的流量*/
uint64 wireless_get_flow(uint8 ifnum)
{
    uint64  send = 0;
    uint64  recv = 0;
    char ucPPPName[8] = {0};

    sprintf(ucPPPName, "ppp%d", ifnum);
    if(0 == wireless_get_ifflux(ucPPPName, &recv, &send))
    {
        return recv + send;
    }
    return send + recv;
}

/* 清空ppp网口路由 如果没有就增加路由表PPP0Rule */
/* 文件默认内容
#
# reserved values
#
255     local
254     main
253     default
0       unspec
#
# local
#
#1      inr.ruhep
*/
void wireless_flush_table(uint8 ifnum)
{
    FILE  *fp;
    char  ucSystem[256] = {0};
    char  tablename[24] = {0};
    char  getString[48] = {0};
    uint8 bTable = FALSE;

    sprintf(tablename, "PPP%dRule", ifnum);
    fp = fopen("/etc/iproute2/rt_tables", "r");
    if(NULL == fp)
    {
        WIRELESS_FMT_DEBUG("增加路由表%s失败\n", tablename);
    }
    while(fgets(getString, 48, fp))
    {
        if(NULL != strstr(getString, tablename))
        {
            bTable = TRUE;
        }
        memset(getString, 0, 48);
    }
    fclose(fp);

    if(FALSE == bTable)
    {
        /* 增加路由表 */
        sprintf(ucSystem, "echo \"%d%c%s\" >> /etc/iproute2/rt_tables", 101 + ifnum, 9, tablename);
        system(ucSystem);
        sleep(1);
    }
    memset(ucSystem, 0, 256);
    sprintf(ucSystem, "ip route flush table %s", tablename);
    system(ucSystem);
}

/* 添加路由规则, 让网口过来的数据包从指定网口出去 */
/* 查询指令 
ip rule show
ip route show table PPP0Rule
*/
void wireless_add_route(uint8 ifnum, uint32 getway, uint32 ipaddr)
{
    char  ucSystem[256] = {0};
    char  tablename[24] = {0};

    sprintf(tablename, "PPP%dRule", ifnum);
    /* 添加路由 */
    sprintf(ucSystem, "ip route add default via %d.%d.%d.%d dev ppp%d table %s", 
                      (getway >> 24) & 0xff, (getway >> 16) & 0xff, (getway >> 8) & 0xff, getway & 0xff,
                      ifnum, tablename);
    system(ucSystem);

    usleep(10000);
    memset(ucSystem, 0, 256);
    /* 根据源 IP 决定路由表 */
    sprintf(ucSystem, "ip rule add from %d.%d.%d.%d table %s", 
                      (ipaddr >> 24) & 0xff, (ipaddr >> 16) & 0xff, (ipaddr >> 8) & 0xff, ipaddr & 0xff,
                      tablename);
    system(ucSystem);
}
