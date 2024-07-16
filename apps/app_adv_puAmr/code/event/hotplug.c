#include "common.h"
#include "framecomm.h"
#include "msg_id.h"
#include "task_id.h"

/** @brief ��Ȩ״̬   */
typedef enum tag_STA_PORT_AUTHORIZE
{
    STA_PORT_AUTH_PERMANENT = 0,   //������Ȩ
    STA_PORT_AUTH_PROVISIONAL = 1, //��ʱ��Ȩ
    STA_PORT_AUTH_FORBIDDEN = 2,   //�ر���Ȩ
}STA_PORT_AUTHORIZE_E;

typedef struct tag_STA_PORT_AUTHORIZE_INFO
{
    STA_PORT_AUTHORIZE_E status; //��Ȩ״̬
    BOOLEAN isAuthChg;   //��Ȩ״̬�仯           1���� 0����
    BOOLEAN isAuthedcmd; //��ȫ��֤�������� 1���� 0����
    BOOLEAN isSuccLocal; //���ذ�ȫ��֤ͨ��         1���� 0����
    uint8 authTime;      //��Ȩʱ��
}STA_PORT_AUTHORIZE_INFO;


#define UEVENT_BUFFER_SIZE 2048  
uint8  g_udisk_status = 0;    //0-�γ� 1-����
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
* @brief     ��U�̼���߳����
* @param[in] ��void *pArg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-12-23
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

        /* USB�豸�Ĳ�λ�����ַ���Ϣ��ͨ���Ƚϲ�ͬ����Ϣȷ���ض��豸�Ĳ�Σ�������ӱȽϴ���*/
        if((memcmp(buf,"add@",4)==0) && (memcmp(buf+strlen(buf)-4,"/sd",3)==0))
        {
            EVENT_FMT_DEBUG("USB disk add.\n");
            g_udisk_status = 1;

            //δ��Ȩʱ�Ľ���
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
