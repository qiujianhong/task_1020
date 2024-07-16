/*
*********************************************************************
* @filename: msg.h
* @brief   : 消息通信
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2019-12-18    create
*********************************************************************
*/
#ifndef    __TASK_MSG_H
#define __TASK_MSG_H

#ifdef __cplusplus
    extern "C" {
#endif

#define FILEPATH_LEN        256
#define TASK_MSG_FILEPATH   "/tmp/ipc_socket"

#define TASK_MSG_DATA_MAX_LEN    8176
#define TASK_MSG_MAX_LEN         (TASK_MSG_DATA_MAX_LEN + 16)

/* 消息结构 */
typedef struct tagTaskMsg
{
    uint32 msg_id;    //消息id
    uint32 sender;    //发送方 task_id
    uint8  syncflag;  //同步消息标记
    uint8  syncindex; //同步序号
    uint16 label;     //消息标记
    uint8  response;  //回复其它APP的消息
    uint8  res[1];    //保留
    uint16 msglen;    //消息长度
    
    uint8  msg[0];    //消息内容
}TASK_MSG_t;

typedef enum
{
    MSG_SUCCESS     = 0,
    MSG_ERR_INPUT   = 0x1000,
    MSG_ERR_MALLOC,
    MSG_ERR_TOO_LONG,
    MSG_ERR_SEND_FAILED,
    MSG_ERR_RECV_FAILED,
    MSG_ERR_BIND,
    MSG_ERR_SOCKET,
    MSG_ERR_OUT_TIME,
    MSG_ERR_DIR,
}msg_errcode_e;

int task_msg_prepare(char *dir);
int task_msg_init(uint32 id);
int task_msg_settime(int sockfd, uint32 sec, uint32 usec);
int task_recv_msg(int sockfd, uint8 *pbuf, uint16 buflen, uint32 *psender);
int task_send_msg(int sockfd, uint32 msgid, uint8 *msgdata, uint16 datalen, uint32 dest);
int task_send_respmsg(int sockfd, uint32 msgid, uint16 label, uint8 *msgdata, uint16 datalen, uint32 dest);
int task_send_labelmsg(int sockfd, uint32 msgid, uint16 label, uint8 *msgdata, uint16 datalen, uint32 dest);
int task_syncmsg_safe(int sockfd, uint32 msgid, uint8 *msgdata, uint16 datalen, uint32 dest,
                      uint8 *msgbuf, uint16 buflen, uint8 outtime);
int task_syncmsg_safe_long_outtime(int sockfd, uint32 msgid, uint8 *msgdata, uint16 datalen, uint32 dest,
                      uint8 *msgbuf, uint16 buflen, uint16 outtime);

int task_syncmsg_label_safe(int sockfd, uint32 msgid, uint16 label, uint8 *msgdata, uint16 datalen, uint32 dest,
    uint8 *msgbuf, uint16 buflen, uint8 outtime);
int task_ack_syncmsg(int sockfd, uint32 msgid, uint8 *msgdata, uint16 datalen, uint32 dest, uint8 index);

int task_pfmsg_init(char *filename);
int task_pfmsg_recv(int *pSockfd, uint8 *pbuf, uint16 buflen, char *sender, uint16 sendlen);
int task_pfmsg_send(int sockfd, uint8 *msg, uint16 msglen, char *dest);
void task_msg_nonblocking_set(int sockfd) ;

#ifdef __cplusplus
}
#endif

#endif

