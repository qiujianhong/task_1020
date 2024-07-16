/*
*********************************************************************
* @filename: msg.c
* @brief   : 消息通信
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2019-12-18    create
*********************************************************************
*/
#include "baseCommon.h"
#include <sys/un.h>

#include "task_msg.h"

#define DPTASK_MSG(b, bl, x...) \
    if(getenv("DIAG_TASKMSG") != NULL)\
    {\
        struct timeval msgtv; \
        struct tm     *pMsgtm; \
        uint16 iii;\
        uint8 *bTemp = (uint8*)b;\
        gettimeofday(&msgtv, NULL);\
        pMsgtm = localtime(&msgtv.tv_sec);\
        printf("[%4d-%02d-%02d %02d:%02d:%02d.%06ld] [TID: %lu, TASK_MSG] ", pMsgtm->tm_year + 1900, pMsgtm->tm_mon + 1, \
             pMsgtm->tm_mday, pMsgtm->tm_hour, pMsgtm->tm_min, pMsgtm->tm_sec, msgtv.tv_usec, pthread_self());\
        printf(x);\
        printf("Msg(%d): ", bl);\
        for(iii = 0; iii < bl; iii++)\
        {\
          printf("%02x ", bTemp[iii]);\
        }\
        printf("\n\n");\
    }

char g_IPCPATH[FILEPATH_LEN] = {0};

#define OFFSET(type, member)  ((unsigned long)((type*)0)->member)
#define TASK_FILE_PATH        ((0 != g_IPCPATH[0]) ? g_IPCPATH : TASK_MSG_FILEPATH)
/**********************************************************************
* @name      : task_msg_prepare
* @brief     ：任务之间消息准备
* @param[in] ：char *dir
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-18
* @Update    :
**********************************************************************/
int task_msg_prepare(char *dir)
{
    if(NULL == dir)
    {
        return MSG_ERR_INPUT;
    }
    
    if(0 != access(dir, F_OK))
    {
        if(0 != mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            return MSG_ERR_DIR;
        }
    }
    
    memcpy(g_IPCPATH, dir, strlen(dir));
    return MSG_SUCCESS;
}

/**********************************************************************
* @name      : task_msg_init
* @brief     ：任务之间消息初始化
* @param[in] ：uint32 id      taskid
* @param[out]：
* @return    ：socket句柄
* @Create    : 王津剑
* @Date      ：2019-12-18
* @Update    :
**********************************************************************/
int task_msg_init(uint32 id)
{
    int  ret = 0;
    char filepath[FILEPATH_LEN] = {0};
    int  sockfd = 0;
    int  len = 0;
    struct sockaddr_un addr;
    struct timeval tv = {0};
    int    opt = 1;
    
    sprintf(filepath, "%s/%u", TASK_FILE_PATH, id);

    unlink(filepath);
    
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);

    /* 设置socket复用 */
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, ( const char* )&opt, sizeof(opt));

    memset(&addr, 0, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filepath);
    len = OFFSET(struct sockaddr_un, sun_path) + strlen(filepath) + 1;
    
    /* 与本地文件进行绑定 */
    ret = bind(sockfd, (struct sockaddr *)&addr, len);
    if(0 != ret)
    {
        printf("%s ", filepath);
        perror(" bind socket failed");
        close(sockfd);
        sockfd = -1;
    }

    /* 设置发送超时0s, 设置阻塞时间非0会触发系统漏洞kernel BUG at kernel/posix-cpu-timers.c */
    ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(tv));
    if(0 != ret)
    {
        printf("%s ", filepath);
        perror(" send timeout failed");
        close(sockfd);
        sockfd = -1;
    }

    return sockfd;
}

/**********************************************************************
* @name      : task_msg_settime
* @brief     ：接收超时时间设置
* @param[in] ：int sockfd      句柄
               uint32 sec      秒
               uint32 usec     微秒
* @param[out]：
* @return    ：socket句柄
* @Create    : 王津剑
* @Date      ：2019-12-18
* @Update    :
**********************************************************************/
int task_msg_settime(int sockfd, uint32 sec, uint32 usec)
{
    struct timeval tv = {0};
    tv.tv_sec = sec;
    tv.tv_usec = usec;
    return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
}

void task_msg_nonblocking_set(int sockfd) 
{
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (flag < 0) 
    {
        return;
    }
    fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);

}


/**********************************************************************
* @name      : task_recv_msg
* @brief     ：任务读取消息
* @param[in] ：int sockfd      socket句柄
               uint16 buflen   消息缓存大小
* @param[out]：uint8 *pbuf     接收缓存消息
               uint32 *psender 发送者
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2019-12-18
* @Update    :
**********************************************************************/
int task_recv_msg(int sockfd, uint8 *pbuf, uint16 buflen, uint32 *psender)
{
    int  ret = 0;
    int  len = sizeof(struct sockaddr_un);
    struct sockaddr_un addr;
    char *temp = NULL;
    
    ret = recvfrom(sockfd, pbuf, buflen, 0, (struct sockaddr*)&addr, (socklen_t *)&len);
    if(ret > 0)
    {
        char filepath[FILEPATH_LEN];
        bzero(filepath, FILEPATH_LEN);
        strcpy(filepath, addr.sun_path);
        temp = filepath + strlen(TASK_FILE_PATH) + 1;
        *psender = atoi(temp);
        DPTASK_MSG(pbuf, ret, "Recv from 0x%x\n", *psender);
    }
    return ret;
}

/**********************************************************************
* @name      : task_send_msg
* @brief     ：任务发送(异步)消息,响应请求的消息
* @param[in] ：int sockfd      socket句柄
               uint32 msgid    消息id
               uint8 *msgdata  消息内容
               uint16 datalen  内容长度
               uint32 dest     接收方
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-18
* @Update    :
**********************************************************************/
int task_send_respmsg(int sockfd, uint32 msgid, uint16 label, uint8 *msgdata, uint16 datalen, uint32 dest)
{
    int  ret = 0;
    int  len = 0;
    struct sockaddr_un addr;
    char filepath[FILEPATH_LEN] = {0};
    TASK_MSG_t *pmsg = NULL;
    uint16 msglen = 0;

    if(sockfd <= 0 || NULL == msgdata || 0 == datalen || 0 == dest)
    {
        return MSG_ERR_INPUT;
    }

    if(datalen > TASK_MSG_DATA_MAX_LEN)
    {
        return MSG_ERR_TOO_LONG;
    }
    
    sprintf(filepath, "%s/%u", TASK_FILE_PATH, dest);
    
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filepath);
    len = OFFSET(struct sockaddr_un, sun_path) + strlen(filepath) + 1;

    msglen = sizeof(TASK_MSG_t) + datalen;

    pmsg = (TASK_MSG_t *)malloc(msglen);
    if(NULL == pmsg)
    {
        return MSG_ERR_MALLOC;
    }
    memset(pmsg, 0, sizeof(TASK_MSG_t));
    pmsg->response = 1;
    pmsg->msg_id = msgid;
    pmsg->msglen = datalen;
    pmsg->label = label;
    memcpy(pmsg->msg, msgdata, datalen);
    
    /* 非阻塞状态下发送 */
    ret = sendto(sockfd, pmsg, msglen, MSG_DONTWAIT, (struct sockaddr*)&addr, len);
    DPTASK_MSG(pmsg, msglen, "Send to 0x%x, label %d, msgid 0x%x, msglen %d, ret %d\n", dest, label, msgid, datalen, ret);
    free(pmsg);
    if(ret <= 0)
    {
        return MSG_ERR_SEND_FAILED;
    }
    return MSG_SUCCESS;
}


/**********************************************************************
* @name      : task_send_msg
* @brief     ：任务发送(异步)消息
* @param[in] ：int sockfd      socket句柄
               uint32 msgid    消息id
               uint8 *msgdata  消息内容
               uint16 datalen  内容长度
               uint32 dest     接收方
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-18
* @Update    :
**********************************************************************/
int task_send_msg(int sockfd, uint32 msgid, uint8 *msgdata, uint16 datalen, uint32 dest)
{
    int  ret = 0;
    int  len = 0;
    struct sockaddr_un addr;
    char filepath[FILEPATH_LEN];
    TASK_MSG_t *pmsg = NULL;
    uint16 msglen = 0;

    if(sockfd <= 0 || 0 == dest || (NULL == msgdata && 0 != datalen))
    {
        return MSG_ERR_INPUT;
    }

    if(datalen > TASK_MSG_DATA_MAX_LEN)
    {
        return MSG_ERR_TOO_LONG;
    }

    bzero(filepath, FILEPATH_LEN);
    sprintf(filepath, "%s/%u", TASK_FILE_PATH, dest);
    
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filepath);
    len = OFFSET(struct sockaddr_un, sun_path) + strlen(filepath) + 1;

    msglen = sizeof(TASK_MSG_t) + datalen;

    pmsg = (TASK_MSG_t *)malloc(msglen);
    if(NULL == pmsg)
    {
        return MSG_ERR_MALLOC;
    }
    bzero(pmsg, sizeof(TASK_MSG_t));
    pmsg->msg_id = msgid;
    pmsg->msglen = datalen;
    if(datalen > 0)
    {
        memcpy(pmsg->msg, msgdata, datalen);
    }

    /* 非阻塞状态下发送 */
    ret = sendto(sockfd, pmsg, msglen, MSG_DONTWAIT, (struct sockaddr*)&addr, len);
    DPTASK_MSG(pmsg, msglen, "Send to 0x%x, msgid 0x%x, msglen %d, ret %d\n", dest, msgid, datalen, ret);
    free(pmsg);
    if(ret <= 0)
    {
        return MSG_ERR_SEND_FAILED;
    }
    return MSG_SUCCESS;
}

/**********************************************************************
* @name      : task_send_labelmsg
* @brief     ：任务发送带标签的异步消息
* @param[in] ：int sockfd      socket句柄
               uint32 msgid    消息id
               uint16 label    消息标签
               uint8 *msgdata  消息内容
               uint16 datalen  内容长度
               uint32 dest     接收方
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-18
* @Update    :
**********************************************************************/
int task_send_labelmsg(int sockfd, uint32 msgid, uint16 label, uint8 *msgdata, uint16 datalen, uint32 dest)
{
    int  ret = 0;
    int  len = 0;
    struct sockaddr_un addr;
    char filepath[FILEPATH_LEN] = {0};
    TASK_MSG_t *pmsg = NULL;
    uint16 msglen = 0;

    if(sockfd <= 0 || 0 == dest || (NULL == msgdata && 0 != datalen))
    {
        return MSG_ERR_INPUT;
    }

    if(datalen > TASK_MSG_DATA_MAX_LEN)
    {
        return MSG_ERR_TOO_LONG;
    }
    
    sprintf(filepath, "%s/%u", TASK_FILE_PATH, dest);
    
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filepath);
    len = OFFSET(struct sockaddr_un, sun_path) + strlen(filepath) + 1;

    msglen = sizeof(TASK_MSG_t) + datalen;

    pmsg = (TASK_MSG_t *)malloc(msglen);
    if(NULL == pmsg)
    {
        return MSG_ERR_MALLOC;
    }
    memset(pmsg, 0, sizeof(TASK_MSG_t));
    pmsg->msg_id = msgid;
    pmsg->msglen = datalen;
    pmsg->label = label;
    if(datalen > 0)
    {
        memcpy(pmsg->msg, msgdata, datalen);
    }
    
    /* 非阻塞状态下发送 */
    ret = sendto(sockfd, pmsg, msglen, MSG_DONTWAIT, (struct sockaddr*)&addr, len);
    DPTASK_MSG(pmsg, msglen, "Send to 0x%x, label %d, msgid 0x%x, msglen %d, ret %d\n", dest, label, msgid, datalen, ret);
    free(pmsg);
    if(ret <= 0)
    {
        return MSG_ERR_SEND_FAILED;
    }
    return MSG_SUCCESS;
}

/**********************************************************************
* @name      : task_syncmsg_safe
* @brief     ：任务发送接收同步消息, 安全接口, 避免和异步接口使用同一个sockfd
               上层调用需要加锁保证收发唯一性
* @param[in] ：int sockfd      socket句柄
               uint32 msgid    消息id
               uint8 *msgdata  消息内容
               uint16 datalen  内容长度
               uint32 dest     接收方
               uint16 buflen   返回消息缓存大小
               uint8 outtime   超时时间 单位s
* @param[out]：uint8 *msgbuf   返回消息
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-10-28
* @Update    :
**********************************************************************/
int task_syncmsg_safe_long_outtime(int sockfd, uint32 msgid, uint8 *msgdata, uint16 datalen, uint32 dest,
                      uint8 *msgbuf, uint16 buflen, uint16 outtime)
{
    int  ret = 0;
    int  errcode = 0;
    int  len = 0;
    struct sockaddr_un addr;
    char filepath[FILEPATH_LEN] = {0};
    TASK_MSG_t *pmsg = NULL;
    uint16 msglen = 0;
    static uint8 index = 0;
    uint8 curindex = 0;
    uint32 sender = 0;

    struct timespec ctime = {0};
    struct timespec stime = {0};
    struct timeval tv = {0, 100000};   /* 一次接收超时时间0.1s */
    
    if(sockfd <= 0 || 0 == dest || (NULL == msgdata && 0 != datalen))
    {
        return MSG_ERR_INPUT;
    }

    if(datalen > TASK_MSG_DATA_MAX_LEN)
    {
        return MSG_ERR_TOO_LONG;
    }
    
    sprintf(filepath, "%s/%u", TASK_FILE_PATH, dest);
    
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filepath);
    len = OFFSET(struct sockaddr_un, sun_path) + strlen(filepath) + 1;

    msglen = sizeof(TASK_MSG_t) + datalen;

    pmsg = (TASK_MSG_t *)malloc(TASK_MSG_MAX_LEN);  //申请最大容量消息
    if(NULL == pmsg)
    {
        return MSG_ERR_MALLOC;
    }
    memset(pmsg, 0, sizeof(TASK_MSG_t));
    pmsg->msg_id = msgid;
    pmsg->syncflag = TRUE;
    curindex = index++;
    pmsg->syncindex = curindex;
    pmsg->msglen = datalen;
    if(datalen > 0)
    {
        memcpy(pmsg->msg, msgdata, datalen);
    }
    /* 1. 发送 */
    /* 非阻塞状态下发送 */
    ret = sendto(sockfd, pmsg, msglen, MSG_DONTWAIT, (struct sockaddr*)&addr, len);
    DPTASK_MSG(pmsg, msglen, "Send syncmsg to 0x%x, index %d, msgid 0x%x, msglen %d, ret %d\n", 
               dest, curindex, msgid, datalen, ret);
    if(ret <= 0)
    {
        free(pmsg);
        return MSG_ERR_SEND_FAILED;
    }
    
    /* 2. 获取本socket绑定的地址 */
    memset(&addr, 0, sizeof(struct sockaddr_un));
    getsockname(sockfd, (struct sockaddr *)&addr, (socklen_t *)&len);

    /* 3. 设置超时时间 */
    ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
    if(0 != ret)
    {
        errcode = MSG_ERR_SOCKET;
        goto end;
    }

    /* 4. 接收等待 */
    clock_gettime(CLOCK_MONOTONIC, &stime);
    for(;;)
    {
        memset(pmsg, 0, TASK_MSG_MAX_LEN);
        ret = task_recv_msg(sockfd, (uint8 *)pmsg, TASK_MSG_MAX_LEN, &sender);
        if(ret > 0)
        {
            if(sender == dest && TRUE == pmsg->syncflag && curindex == pmsg->syncindex)
            {
                if(buflen < ret)
                {
                    errcode = MSG_ERR_TOO_LONG;
                }
                else
                {
                    memcpy(msgbuf, pmsg, ret);
                }
                DPTASK_MSG(pmsg, ret, "Recv syncmsg from 0x%x, index %d\n", sender, pmsg->syncindex);
                goto end;
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &ctime);
        if(ctime.tv_sec - stime.tv_sec > outtime)
        {
            errcode = MSG_ERR_OUT_TIME;
            goto end;
        }
    }

end:
    free(pmsg);
    return errcode;
}

/**********************************************************************
* @name      : task_syncmsg_safe
* @brief     ：任务发送接收同步消息, 安全接口, 避免和异步接口使用同一个sockfd
               上层调用需要加锁保证收发唯一性
* @param[in] ：int sockfd      socket句柄
               uint32 msgid    消息id
               uint8 *msgdata  消息内容
               uint16 datalen  内容长度
               uint32 dest     接收方
               uint16 buflen   返回消息缓存大小
               uint8 outtime   超时时间 单位s
* @param[out]：uint8 *msgbuf   返回消息
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-10-28
* @Update    :
**********************************************************************/
int task_syncmsg_safe(int sockfd, uint32 msgid, uint8 *msgdata, uint16 datalen, uint32 dest,
                      uint8 *msgbuf, uint16 buflen, uint8 outtime)
{
    int  ret = 0;
    int  errcode = 0;
    int  len = 0;
    struct sockaddr_un addr;
    char filepath[FILEPATH_LEN] = {0};
    TASK_MSG_t *pmsg = NULL;
    uint16 msglen = 0;
    static uint8 index = 0;
    uint8 curindex = 0;
    uint32 sender = 0;

    struct timespec ctime = {0};
    struct timespec stime = {0};
    struct timeval tv = {0, 100000};   /* 一次接收超时时间0.1s */
    
    if(sockfd <= 0 || 0 == dest || (NULL == msgdata && 0 != datalen))
    {
        return MSG_ERR_INPUT;
    }

    if(datalen > TASK_MSG_DATA_MAX_LEN)
    {
        return MSG_ERR_TOO_LONG;
    }
    
    sprintf(filepath, "%s/%u", TASK_FILE_PATH, dest);
    
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filepath);
    len = OFFSET(struct sockaddr_un, sun_path) + strlen(filepath) + 1;

    msglen = sizeof(TASK_MSG_t) + datalen;

    pmsg = (TASK_MSG_t *)malloc(TASK_MSG_MAX_LEN);  //申请最大容量消息
    if(NULL == pmsg)
    {
        return MSG_ERR_MALLOC;
    }
    memset(pmsg, 0, sizeof(TASK_MSG_t));
    pmsg->msg_id = msgid;
    pmsg->syncflag = TRUE;
    curindex = index++;
    pmsg->syncindex = curindex;
    pmsg->msglen = datalen;
    if(datalen > 0)
    {
        memcpy(pmsg->msg, msgdata, datalen);
    }
    /* 1. 发送 */
    /* 非阻塞状态下发送 */
    ret = sendto(sockfd, pmsg, msglen, MSG_DONTWAIT, (struct sockaddr*)&addr, len);
    DPTASK_MSG(pmsg, msglen, "Send syncmsg to 0x%x, index %d, msgid 0x%x, msglen %d, ret %d\n", 
               dest, curindex, msgid, datalen, ret);
    if(ret <= 0)
    {
        free(pmsg);
        return MSG_ERR_SEND_FAILED;
    }
    
    /* 2. 获取本socket绑定的地址 */
    memset(&addr, 0, sizeof(struct sockaddr_un));
    getsockname(sockfd, (struct sockaddr *)&addr, (socklen_t *)&len);

    /* 3. 设置超时时间 */
    ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
    if(0 != ret)
    {
        errcode = MSG_ERR_SOCKET;
        goto end;
    }

    /* 4. 接收等待 */
    clock_gettime(CLOCK_MONOTONIC, &stime);
    for(;;)
    {
        memset(pmsg, 0, TASK_MSG_MAX_LEN);
        ret = task_recv_msg(sockfd, (uint8 *)pmsg, TASK_MSG_MAX_LEN, &sender);
        if(ret > 0)
        {
            if(sender == dest && TRUE == pmsg->syncflag && curindex == pmsg->syncindex)
            {
                if(buflen < ret)
                {
                    errcode = MSG_ERR_TOO_LONG;
                }
                else
                {
                    memcpy(msgbuf, pmsg, ret);
                }
                DPTASK_MSG(pmsg, ret, "Recv syncmsg from 0x%x, index %d\n", sender, pmsg->syncindex);
                goto end;
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &ctime);
        if(ctime.tv_sec - stime.tv_sec > outtime)
        {
            errcode = MSG_ERR_OUT_TIME;
            goto end;
        }
    }

end:
    free(pmsg);
    return errcode;
}

/**********************************************************************
* @name      : task_syncmsg_safe
* @brief     ：任务发送接收同步消息, 安全接口, 避免和异步接口使用同一个sockfd
               上层调用需要加锁保证收发唯一性
* @param[in] ：int sockfd      socket句柄
               uint32 msgid    消息id
               uint8 *msgdata  消息内容
               uint16 datalen  内容长度
               uint32 dest     接收方
               uint16 buflen   返回消息缓存大小
               uint8 outtime   超时时间 单位s
* @param[out]：uint8 *msgbuf   返回消息
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-10-28
* @Update    :
**********************************************************************/
int task_syncmsg_label_safe(int sockfd, uint32 msgid, uint16 label, uint8 *msgdata, uint16 datalen, uint32 dest,
    uint8 *msgbuf, uint16 buflen, uint8 outtime)
{
    int  ret = 0;
    int  errcode = 0;
    int  len = 0;
    struct sockaddr_un addr;
    char filepath[FILEPATH_LEN] = {0};
    TASK_MSG_t *pmsg = NULL;
    uint16 msglen = 0;
    static uint8 index = 0;
    uint8 curindex = 0;
    uint32 sender = 0;

    struct timespec ctime = {0};
    struct timespec stime = {0};
    struct timeval tv = {0, 100000};   /* 一次接收超时时间0.1s */
    
    if(sockfd <= 0 || 0 == dest || (NULL == msgdata && 0 != datalen))
    {
        return MSG_ERR_INPUT;
    }

    if(datalen > TASK_MSG_DATA_MAX_LEN)
    {
        return MSG_ERR_TOO_LONG;
    }
    
    sprintf(filepath, "%s/%u", TASK_FILE_PATH, dest);
    
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filepath);
    len = OFFSET(struct sockaddr_un, sun_path) + strlen(filepath) + 1;

    msglen = sizeof(TASK_MSG_t) + datalen;

    pmsg = (TASK_MSG_t *)malloc(TASK_MSG_MAX_LEN);  //申请最大容量消息
    if(NULL == pmsg)
    {
        return MSG_ERR_MALLOC;
    }
    memset(pmsg, 0, sizeof(TASK_MSG_t));
    pmsg->msg_id = msgid;
    pmsg->label = label;
    pmsg->syncflag = TRUE;
    curindex = index++;
    pmsg->syncindex = curindex;
    pmsg->msglen = datalen;
    if(datalen > 0)
    {
        memcpy(pmsg->msg, msgdata, datalen);
    }
    /* 1. 发送 */
    /* 非阻塞状态下发送 */
    ret = sendto(sockfd, pmsg, msglen, MSG_DONTWAIT, (struct sockaddr*)&addr, len);
    DPTASK_MSG(pmsg, msglen, "Send syncmsg to 0x%x, index %d, msgid 0x%x, msglen %d, ret %d\n", 
               dest, curindex, msgid, datalen, ret);
    if(ret <= 0)
    {
        free(pmsg);
        return MSG_ERR_SEND_FAILED;
    }
    
    /* 2. 获取本socket绑定的地址 */
    memset(&addr, 0, sizeof(struct sockaddr_un));
    getsockname(sockfd, (struct sockaddr *)&addr, (socklen_t *)&len);

    /* 3. 设置超时时间 */
    ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
    if(0 != ret)
    {
        errcode = MSG_ERR_SOCKET;
        goto end;
    }

    /* 4. 接收等待 */
    clock_gettime(CLOCK_MONOTONIC, &stime);
    for(;;)
    {
        memset(pmsg, 0, TASK_MSG_MAX_LEN);
        ret = task_recv_msg(sockfd, (uint8 *)pmsg, TASK_MSG_MAX_LEN, &sender);
        if(ret > 0)
        {
            if(sender == dest && TRUE == pmsg->syncflag && curindex == pmsg->syncindex)
            {
                if(buflen < ret)
                {
                    errcode = MSG_ERR_TOO_LONG;
                }
                else
                {
                    memcpy(msgbuf, pmsg, ret);
                }
                DPTASK_MSG(pmsg, ret, "Recv syncmsg from 0x%x, index %d\n", sender, pmsg->syncindex);
                goto end;
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &ctime);
        if(ctime.tv_sec - stime.tv_sec > outtime)
        {
            errcode = MSG_ERR_OUT_TIME;
            goto end;
        }
    }

end:
    free(pmsg);
    return errcode;
}

/**********************************************************************
* @name      : task_db_get_syncmsg
* @brief     ：给DB操作的发送等待同步消息接口, db使用接收阻塞时间0.1s
               不匹配的消息丢掉
* @param[in] ：int sockfd      socket句柄
               uint32 msgid    消息id
               uint8 *msgdata  消息内容
               uint16 datalen  内容长度
               uint32 dest     接收方
               uint16 buflen   返回消息缓存大小
               uint8 outtime   超时时间
* @param[out]：uint8 *msgbuf   返回消息
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-15
* @Update    :
**********************************************************************/
int task_db_get_syncmsg(int sockfd, uint32 msgid, uint8 *msgdata, uint16 datalen, uint32 dest,
                           uint8 *msgbuf, uint16 buflen, uint8 outtime)
{
    int  ret = 0;
    int  errcode = 0;
    int  len = 0;
    struct sockaddr_un addr;
    char filepath[FILEPATH_LEN] = {0};
    TASK_MSG_t *pmsg = NULL;
    uint16 msglen = 0;
    static uint8 index = 0;
    uint8 curindex = 0;
    uint32 sender = 0;

    struct timespec ctime = {0};
    struct timespec stime = {0};
    
    if(sockfd <= 0 || NULL == msgdata || 0 == datalen || 0 == dest)
    {
        return MSG_ERR_INPUT;
    }

    if(datalen > TASK_MSG_DATA_MAX_LEN)
    {
        return MSG_ERR_TOO_LONG;
    }
    
    sprintf(filepath, "%s/%u", TASK_FILE_PATH, dest);
    
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filepath);
    len = OFFSET(struct sockaddr_un, sun_path) + strlen(filepath) + 1;

    msglen = sizeof(TASK_MSG_t) + datalen;

    pmsg = (TASK_MSG_t *)malloc(TASK_MSG_MAX_LEN);  //申请最大容量消息
    if(NULL == pmsg)
    {
        return MSG_ERR_MALLOC;
    }
    memset(pmsg, 0, sizeof(TASK_MSG_t));
    pmsg->msg_id = msgid;
    pmsg->syncflag = TRUE;
    curindex = index++;
    pmsg->syncindex = curindex;
    pmsg->msglen = datalen;
    memcpy(pmsg->msg, msgdata, datalen);

    /* 1. 发送 */
    /* 非阻塞状态下发送 */
    ret = sendto(sockfd, pmsg, msglen, MSG_DONTWAIT, (struct sockaddr*)&addr, len);
    DPTASK_MSG(pmsg, msglen, "Send syncmsg to 0x%x, index %d, msgid 0x%x, msglen %d, ret %d\n", 
                   dest, index, msgid, datalen, ret);
    if(ret <= 0)
    {
        free(pmsg);
        return MSG_ERR_SEND_FAILED;
    }

    /* 2. 接收等待 */
    clock_gettime(CLOCK_MONOTONIC, &stime);
    for(;;)
    {
        memset(pmsg, 0, TASK_MSG_MAX_LEN);
        ret = task_recv_msg(sockfd, (uint8 *)pmsg, TASK_MSG_MAX_LEN, &sender);
        if(ret > 0)
        {
            if(sender == dest && TRUE == pmsg->syncflag && curindex == pmsg->syncindex)
            {
                if(buflen < ret)
                {
                    errcode = MSG_ERR_TOO_LONG;
                }
                else
                {
                    memcpy(msgbuf, pmsg, ret);
                }
                DPTASK_MSG(pmsg, ret, "Recv syncmsg from 0x%x, index %d, len %d\n", 
                                           sender, pmsg->syncindex, ret);
                goto end;
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &ctime);
        if(ctime.tv_sec - stime.tv_sec > outtime)
        {
            errcode = MSG_ERR_OUT_TIME;
            goto end;
        }
    }

end:
    free(pmsg);
    return errcode;
}


/**********************************************************************
* @name      : task_ack_syncmsg
* @brief     ：任务返回同步消息
* @param[in] ：int sockfd      socket句柄
               uint32 msgid    消息id
               uint8 *msgdata  消息内容
               uint16 datalen  内容长度
               uint32 dest     接收方
               uint8 index     同步消息索引
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-18
* @Update    :
**********************************************************************/
int task_ack_syncmsg(int sockfd, uint32 msgid, uint8 *msgdata, uint16 datalen, uint32 dest, uint8 index)
{
    int  ret = 0;
    int  len = 0;
    struct sockaddr_un addr;
    char filepath[FILEPATH_LEN] = {0};
    TASK_MSG_t *pmsg = NULL;
    uint16 msglen = 0;

    if(sockfd <= 0 || 0 == dest || (NULL == msgdata && 0 != datalen))
    {
        return MSG_ERR_INPUT;
    }

    if(datalen > TASK_MSG_DATA_MAX_LEN)
    {
        return MSG_ERR_TOO_LONG;
    }
    
    sprintf(filepath, "%s/%u", TASK_FILE_PATH, dest);
    
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filepath);
    len = OFFSET(struct sockaddr_un, sun_path) + strlen(filepath) + 1;

    msglen = sizeof(TASK_MSG_t) + datalen;

    pmsg = (TASK_MSG_t *)malloc(msglen);
    if(NULL == pmsg)
    {
        return MSG_ERR_MALLOC;
    }
    memset(pmsg, 0, sizeof(TASK_MSG_t));
    pmsg->msg_id = msgid;
    pmsg->syncflag = TRUE;
    pmsg->syncindex = index;
    pmsg->msglen = datalen;
    if(datalen > 0)
    {
        memcpy(pmsg->msg, msgdata, datalen);
    }
    /* 非阻塞状态下发送 */
    ret = sendto(sockfd, pmsg, msglen, MSG_DONTWAIT, (struct sockaddr*)&addr, len);
    DPTASK_MSG(pmsg, msglen, "Send ack to 0x%x, index %d, msgid 0x%x, msglen %d, ret %d\n", 
                              dest, index, msgid, datalen, ret);
    free(pmsg);
    if(ret <= 0)
    {
        return MSG_ERR_SEND_FAILED;
    }
    return MSG_SUCCESS;
}

/**********************************************************************
* @name      : task_pfmsg_init
* @brief     ：PF_UNIX udp扩展消息初始化, 默认是阻塞式的
* @param[in] ：char *filename   绑定的文件名称(全路径)
* @param[out]：
* @return    ：socket句柄
* @Create    : 王津剑
* @Date      ：2020-8-3
* @Update    :
**********************************************************************/
int task_pfmsg_init(char *filename)
{
    int  ret = 0;
    int  sockfd = 0;
    int  len = 0;
    struct sockaddr_un addr;
    struct timeval tv = {0};
    int    opt = 1;

    if(NULL == filename)
    {
        return 0;
    }
    unlink(filename);
    
    sockfd = socket(PF_UNIX, SOCK_DGRAM, 0);
    
    /* 设置socket复用 */
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, ( const char* )&opt, sizeof(opt));

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filename);
    len = sizeof(addr);

    /* 与本地文件进行绑定 */
    ret = bind(sockfd, (struct sockaddr *)&addr, len);
    if(0 != ret)
    {
        printf("%s ", filename);
        perror(" bind socket failed");
        close(sockfd);
        sockfd = -1;
    }

    /* 设置发送超时0s 防止发送卡死 */
    ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(tv));
    if(0 != ret)
    {
        printf("%s ", filename);
        perror(" send timeout failed");
        close(sockfd);
        sockfd = -1;
    }

    /* 设置发送超时1ms */
    ret = task_msg_settime(sockfd, 0, 1000);
    if(0 != ret)
    {
        printf("%s ", filename);
        perror(" recv timeout failed");
        close(sockfd);
        sockfd = -1;
    }

    return sockfd;
}

/**********************************************************************
* @name      : task_pfmsg_recv
* @brief     ：PF_UNIX udp扩展消息接收
* @param[in] ：int *pSockfd    socket句柄
               uint16 buflen   消息缓存大小
               uint16 sendlen  发送者缓存大小
* @param[out]：uint8 *pbuf     接收缓存消息
               char *sender    发送者
* @return    ：消息长度/错误码(<0)
* @Create    : 王津剑
* @Date      ：2020-8-3
* @Update    :
**********************************************************************/
int task_pfmsg_recv(int *pSockfd, uint8 *pbuf, uint16 buflen, char *sender, uint16 sendlen)
{
    int  ret = 0;
    int  sockfd = *pSockfd;
    int  len = sizeof(struct sockaddr_un);
    struct sockaddr_un addr;
    
    if(sockfd <= 0 || NULL == pbuf || NULL == sender || 0 == buflen || 0 == sendlen)
    {
        return 0;
    }

    /* 获取本socket绑定的地址, 判断地址是否存在 */
    memset(&addr, 0, sizeof(struct sockaddr_un));
    getsockname(sockfd, (struct sockaddr *)&addr, (socklen_t *)&len);
    if(0 != access(addr.sun_path, F_OK))
    {
        close(sockfd);
        *pSockfd = task_pfmsg_init(addr.sun_path);
        sockfd = *pSockfd;
    }

    len = sizeof(struct sockaddr_un);
    memset(&addr, 0, sizeof(struct sockaddr_un));
    ret = recvfrom(sockfd, pbuf, buflen, 0, (struct sockaddr*)&addr, (socklen_t *)&len);
    if(ret > 0)
    {
        len = strlen(addr.sun_path);
        memset(sender, 0, sendlen);
        memcpy(sender, addr.sun_path, MIN(sendlen, len));
    }
    return ret;
}

/**********************************************************************
* @name      : task_pfmsg_send
* @brief     ：PF_UNIX udp任务发送消息
* @param[in] ：int sockfd      socket句柄
               uint8 *msgdata  消息内容
               uint16 datalen  内容长度
               char *dest      接收方
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-8-3
* @Update    :
**********************************************************************/
int task_pfmsg_send(int sockfd, uint8 *msg, uint16 msglen, char *dest)
{
    int  ret = 0;
    int  len = 0;
    struct sockaddr_un addr;

    if(sockfd <= 0 || NULL == dest || NULL == msg || 0 == msglen)
    {
        return MSG_ERR_INPUT;
    }

    if(msglen > TASK_MSG_MAX_LEN)
    {
        return MSG_ERR_TOO_LONG;
    }
    
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, dest);
    len = sizeof(addr);

    /* 非阻塞状态下发送 */
    ret = sendto(sockfd, msg, msglen, MSG_DONTWAIT, (struct sockaddr*)&addr, len);
    if(ret <= 0)
    {
        return MSG_ERR_SEND_FAILED;
    }
    return MSG_SUCCESS;
}



