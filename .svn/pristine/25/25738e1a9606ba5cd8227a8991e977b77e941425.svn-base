#include "common.h"
#include "framecomm.h"
#include "msg_id.h"
#include "task_id.h"

/** @brief 授权状态   */
typedef enum tag_STA_PORT_AUTHORIZE
{
    STA_PORT_AUTH_PERMANENT = 0,   //永久授权
    STA_PORT_AUTH_PROVISIONAL = 1, //临时授权
    STA_PORT_AUTH_FORBIDDEN = 2,   //关闭授权
}STA_PORT_AUTHORIZE_E;

typedef struct tag_STA_PORT_AUTHORIZE_INFO
{
    STA_PORT_AUTHORIZE_E status; //授权状态
    BOOLEAN isAuthChg;   //授权状态变化           1：是 0：否
    BOOLEAN isAuthedcmd; //安全认证交互命令 1：是 0：否
    BOOLEAN isSuccLocal; //本地安全认证通过         1：是 0：否
    uint8 authTime;      //授权时长
}STA_PORT_AUTHORIZE_INFO;


#define UEVENT_BUFFER_SIZE 2048  
uint8  g_udisk_status = 0;    //0-拔出 1-插入
extern int g_msgMonitor;
extern STA_PORT_AUTHORIZE_INFO g_portUSB;
extern int task_send_labelmsg(int sockfd, uint32 msgid, uint16 label, uint8 *msgdata, uint16 datalen, uint32 dest);
extern uint8 gdelay;

int hotplug_sock_init()  
{  
    const int buffersize = 1024;  
    int ret;  

    struct sockaddr_nl snl;  
    bzero(&snl, sizeof(struct sockaddr_nl));  
    snl.nl_family = AF_NETLINK;  
    snl.nl_pid = getpid();  
    snl.nl_groups = 1;  

    int s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);  
    if (s == -1)   
    {  
        perror("socket");  
        return -1;  
    }  
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));  

    ret = bind(s, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));  
    if (ret < 0)   
    {  
        perror("bind");  
        close(s);  
        return -1;  
    }  

    return s;  
}  

/**********************************************************************
* @name      : pulse_task_entry
* @brief     ：U盘检测线程入口
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2019-12-23
* @Update    : 2020-6-1
**********************************************************************/
int hotplug_detect_entry(uint32 dbip, uint16 dbport)  
{  
    int ret = 0;
    uint8 sendmsg[1] = {2};
    uint16 msglen = 1;
    int socket = hotplug_sock_init();  

    while(1)  
    {  
        /* Netlink message buffer */  
        char buf[UEVENT_BUFFER_SIZE * 2] = {0};  
        recv(socket, &buf, sizeof(buf), 0);  
        //printf("%s\n", buf);  

        /* USB设备的插拔会出现字符信息，通过比较不同的信息确定特定设备的插拔，在这添加比较代码*/
        if((memcmp(buf,"add@",4)==0) && (memcmp(buf+strlen(buf)-4,"/sd",3)==0))
        {
            EVENT_FMT_DEBUG("USB disk add.\n");
            g_udisk_status = 1;

            //未授权时的接入
            if(g_portUSB.status == STA_PORT_AUTH_FORBIDDEN)
            {
                ret = task_send_labelmsg(g_msgMonitor, MSG_APPAMIN_UDISK_AUTHORIZE, 0, sendmsg, msglen, APPMAIN_MSG);
                EVENT_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", APPMAIN_MSG, MSG_APPAMIN_UDISK_AUTHORIZE, 0, ret); 
            }
        }

        if((memcmp(buf,"remove@",7)==0) && (memcmp(buf+strlen(buf)-4,"/sd",3)==0))
        {
            EVENT_FMT_DEBUG("USB disk remove.\n");            
            g_udisk_status = 0;
        }
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
    
    return 0;  
}
