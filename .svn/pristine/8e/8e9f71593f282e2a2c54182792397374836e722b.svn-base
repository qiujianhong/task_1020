/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块其他功能：外部消息收发\配置操作
* @date：    2019-10-31
* @history： 
*********************************************************************
*/

#include "baseCommon.h"
#include "msgUdp.h"
#include "task_msg.h"
#include "db_api.h"
#include "task_id.h"
#include "msg_id.h"

#include "comm.h"
#include "msg_comm.h"

pthread_mutex_t  g_fCommCfg;
uint8            g_bCommChange = FALSE;

pthread_mutex_t  g_fCommMsg;
int              g_CommTaskSock = 0;
int              g_CommTaskSync = 0;

extern void CheckERC_3030(int fd,OOP_OAD_U sourceoad,OOP_CHILDNODE_T pre,OOP_CHILDNODE_T now);

COMM_QUEUE_LIST_T g_tPPPDBWrite;     
extern uint8 gdelay;
/**********************************************************************
* @name      : comm_change_init
* @brief     ：通信模块数据更新初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-1-13
* @Update    : 2020-1-16
**********************************************************************/
void comm_change_init()
{
    pthread_mutex_init(&g_fCommCfg, NULL);
    g_bCommChange = FALSE;
    comm_flow_init();

    pthread_mutex_init(&g_fCommMsg, NULL);

    memset(&g_tPPPDBWrite, 0, sizeof(COMM_QUEUE_LIST_T));
    pthread_mutex_init(&g_tPPPDBWrite.f_lock, NULL);
}

/**********************************************************************
* @name      : comm_change_setflag
* @brief     ：通信模块数据更新设置
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-1-13
* @Update    :
**********************************************************************/
void comm_change_setflag(uint8 flag)
{
    PTHREAD_LOCK_SAFE(&g_fCommCfg);
    g_bCommChange = flag;
    PTHREAD_UNLOCK_SAFE(&g_fCommCfg);
}

/**********************************************************************
* @name      : comm_change_getflag
* @brief     ：获取通信模块数据更新设置
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-1-13
* @Update    :
**********************************************************************/
uint8 comm_change_getflag()
{
    uint8 flag = 0;
    PTHREAD_LOCK_SAFE(&g_fCommCfg);
    flag = g_bCommChange;
    PTHREAD_UNLOCK_SAFE(&g_fCommCfg);
    return flag;
}

/**********************************************************************
* @name      : comm_change_update
* @brief     ：数据更新
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-1-13
* @Update    :
**********************************************************************/
int comm_change_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    if(0 != logicId)
    {
        return -1;
    }
    
    if(0x45000200 == value || 0x45000300 == value || 0x45000400 == value || 0x45000E00 == value
       || 0x45010200 == value || 0x45010300 == value || 0x45010400 == value || 0x45010E00 == value
       || 0x45100200 == value || 0x45100300 == value || 0x45110200 == value || 0x45110300 == value
       || 0x451004FE == value || 0x451104FE == value || 0x40010200 == value)
    {
        comm_change_setflag(TRUE);
    }
    return 0;
}

/**********************************************************************
* @name      : comm_cfgmsg_add
* @brief     ：配置消息收发线程
* @param[in] ：COMM_CFGMSG_HANDLE_T *ptHandle
               CheckFUN pFunc     判断函数
               void *pArg         存储地址
               uint8 *pRestart    外部重启标记
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
void comm_cfgmsg_add(COMM_CFGMSG_HANDLE_T *ptHandle, CheckFUN pFunc, void *pArg, uint8 *pRestart)
{
    if(ptHandle->unNum >= 12 || NULL == pFunc)
    {
        COMM_FMT_DEBUG("comm_cfgmsg_add failed\n");
        return;
    }

    ptHandle->tCheck[ptHandle->unNum].pCheckFunc = pFunc;
    ptHandle->tCheck[ptHandle->unNum].pCfg = pArg;
    ptHandle->tCheck[ptHandle->unNum].pRestart = pRestart;

    ptHandle->unNum++;
}

/**********************************************************************
* @name      : comm_add_writedb
* @brief     ：添加操作写数据中心
* @param[in] ：uint32 oad    OAD信息
               uint8 *buf    内容
               uint16 len    长度
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-9
* @Update    :
**********************************************************************/
void comm_add_writedb(uint32 oad, uint8 *buf, uint16 len)
{
    if(0 == oad || NULL == buf || 0 == len)
    {
        return;
    }
    uint8 *temp = (uint8 *)malloc(len + 4);
    if(NULL == temp)
    {
        return;
    }
    memcpy(temp, &oad, sizeof(uint32));
    memcpy(temp + 4, buf, len);

    if(0 != comm_write_queue(&g_tPPPDBWrite, temp, len + 4))
    {
        COMM_FMT_DEBUG("Write oad 0x%x to queue failed\n", oad);
    }
    free(temp);
}

/**********************************************************************
* @name      : comm_cfgmsg_entry
* @brief     ：配置消息收发线程
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
void *comm_cfgmsg_entry(void *pArg)
{
    uint8  i;
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    COMM_CFGMSG_HANDLE_T *ptHandle = (COMM_CFGMSG_HANDLE_T *)pArg;
    uint32 dataLen = 0;
    uint8  dataBuf[BUF_LENGTH] = {0};
    int    ret = 0;

    ptHandle->dbclient = db_client_create(DB_YC_AGENT, DB_CLIENT_COMM);
    if(ptHandle->dbclient <= 0)
    {
        *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
        COMM_FMT_DEBUG("Comm db_client_create create failed!");
        return 0;

    }

    /* 初始化读取一次 */
    sleep(10);
    for(i = 0; i < ptHandle->unNum; i++)
    {
        *ptHandle->pRunFlag = 0;
        if(ptHandle->tCheck[i].pCheckFunc)
        {
            ptHandle->tCheck[i].pCheckFunc(ptHandle->dbclient, ptHandle->tCheck[i].pCfg, 
                                           ptHandle->tCheck[i].pRestart);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    while(1)
    {
        *ptHandle->pRunFlag = 0;
        
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        if(curtime.tv_sec - oldtime.tv_sec > 5 && TRUE == comm_change_getflag())
        {
            /* 配置查询与变更, 防止数据更新消息丢失 */
            comm_change_setflag(FALSE);
            for(i = 0; i < ptHandle->unNum; i++)
            {
                *ptHandle->pRunFlag = 0;
                if(ptHandle->tCheck[i].pCheckFunc)
                {
                    ptHandle->tCheck[i].pCheckFunc(ptHandle->dbclient, ptHandle->tCheck[i].pCfg, 
                                                   ptHandle->tCheck[i].pRestart);
                }
            }
            oldtime = curtime;
        }
#if 0
        else if(curtime.tv_sec - oldtime.tv_sec > 60)
        {
            /* 防止数据中心启动时没有参数返回，1min读取一次 */
            for(i = 0; i < ptHandle->unNum; i++)
            {
                *ptHandle->pRunFlag = 0;
                if(ptHandle->tCheck[i].pCheckFunc)
                {
                    ptHandle->tCheck[i].pCheckFunc(ptHandle->dbclient, ptHandle->tCheck[i].pCfg, 
                                                   ptHandle->tCheck[i].pRestart);
                }
            }
            oldtime = curtime;
        }
#endif
        dataLen = comm_read_queue(&g_tPPPDBWrite, dataBuf);
        if(dataLen > 0)
        {
            NOMAL_OAD_T tOad;
            memset(&tOad, 0, sizeof(NOMAL_OAD_T));
            tOad.oad.value = *((uint32*)dataBuf);
            if(0 == (tOad.oad.value & 0xffff))
            {
                /* 模块变更 */
                OOP_CHILDNODE_T pre;
                OOP_CHILDNODE_T now;
                OOP_VISIBLEVAR_T *devdesc = (OOP_VISIBLEVAR_T *)(dataBuf + 4);

                memset(&pre, 0, sizeof(OOP_CHILDNODE_T));
                memset(&now, 0, sizeof(OOP_CHILDNODE_T));
                tOad.oad.value += 0x0C00;
                pre.id.nNum = devdesc[0].nNum;
                memcpy(pre.id.value, devdesc[0].value, devdesc[0].nNum);
                now.id.nNum = devdesc[1].nNum;
                memcpy(now.id.value, devdesc[1].value, devdesc[1].nNum);
                CheckERC_3030(ptHandle->dbclient, tOad.oad, pre, now);
                COMM_FMT_DEBUG("PPP Mode 0x%x change form %s to %s!\n", tOad.oad.value, pre.id.value, now.id.value);
            }
            else
            {
                tOad.classtag = (0x0400 == (tOad.oad.value & 0x0f00)) ? CLASS_DATA_UNINIT : CLASS_DATA_INIT;
                ret = db_write_nomal(ptHandle->dbclient, &tOad, dataBuf + 4, dataLen - 4);
                if(0 != ret)
                {
                    COMM_FMT_DEBUG("Write oad 0x%x failed, ret %d\n", tOad.oad, ret);
                }
            }
        }
        usleep(100000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
    pthread_exit(NULL);
}

/**********************************************************************
* @name      : comm_outtimer_add
* @brief     ：对外通信线程定时器注册
* @param[in] ：COMM_OUTMSG_HANDLE_T *ptHandle   线程句柄
               uint16 outTime                   定时器时间, 单位s
               pFunc pFunc                      定时器入口函数
               void *pArg                       消息私有数据
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-5
* @Update    :
**********************************************************************/
void comm_outtimer_add(COMM_OUTMSG_HANDLE_T *ptHandle, uint16 outTime, TimeFUN pFunc, void *pArg)
{
    if(ptHandle->nTimerNum >= 16 || NULL == pFunc)
    {
        COMM_FMT_DEBUG("comm_outmsg_add failed\n");
        return;
    }

    ptHandle->tTImer[ptHandle->nTimerNum].nTime = outTime;
    ptHandle->tTImer[ptHandle->nTimerNum].pTimeFunc = pFunc;
    ptHandle->tTImer[ptHandle->nTimerNum].pArg = pArg;

    ptHandle->nTimerNum++;
}

/**********************************************************************
* @name      : comm_outmsg_add
* @brief     ：对外异步消息处理函数注册
* @param[in] ：COMM_OUTMSG_HANDLE_T *ptHandle   线程句柄
               uint16 IID
               uint16 IOP
               uint16 label                     填写0xffff表示忽略
               MsgFUN pFunc                     消息入口函数
               void *pArg                       消息私有数据
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-5
* @Update    :
**********************************************************************/
void comm_outmsg_add(COMM_OUTMSG_HANDLE_T *ptHandle, uint32 msgid, uint16 label, MsgFUN pFunc, void *pArg)
{
    if(ptHandle->nMsgNum >= 48 || NULL == pFunc)
    {
        COMM_FMT_DEBUG("comm_outmsg_add failed\n");
        return;
    }
    ptHandle->tMsg[ptHandle->nMsgNum].msgid = msgid;
    ptHandle->tMsg[ptHandle->nMsgNum].label = label;
    ptHandle->tMsg[ptHandle->nMsgNum].pMsgFunc = pFunc;
    ptHandle->tMsg[ptHandle->nMsgNum].pArg = pArg;

    ptHandle->nMsgNum++;
}

/**********************************************************************
* @name      : comm_cfgmsg_entry
* @brief     ：对外异步消息处理线程
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-5
* @Update    :
**********************************************************************/
void *comm_outmsg_entry(void *pArg)
{
    uint8  i;
    uint8       recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;
    int         recvlen = 0;
    uint32      sender = 0;
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    uint32      offsetTime = 0;
    
    COMM_MSG_T             *ptMsgProc = NULL;
    COMM_TIMER_T              *pTimer = NULL;
    COMM_OUTMSG_HANDLE_T *ptMsgHandle = (COMM_OUTMSG_HANDLE_T*)pArg;

    /* 初始化 */
    if(ptMsgHandle->socketfd <= 0)
    {
        ptMsgHandle->socketfd = task_msg_init(COMM_MSG);
        if(ptMsgHandle->socketfd <= 0)
        {
            COMM_FMT_DEBUG("COMM_MSG task msg init failed\n");
            return 0;
        }
    }
    task_msg_settime(ptMsgHandle->socketfd, 0, 1000);  //设置阻塞超时时间为0.001s
    g_CommTaskSock = ptMsgHandle->socketfd;

    if(ptMsgHandle->syncsocket <= 0)
    {
        ptMsgHandle->syncsocket = task_msg_init(COMM_SYNCMSG);
        if(ptMsgHandle->syncsocket <= 0)
        {
            COMM_FMT_DEBUG("COMM_MSG task msg init failed\n");
            return 0;
        }
    }
    g_CommTaskSync = ptMsgHandle->syncsocket;

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    for(;;)
    {
        *ptMsgHandle->pRunFlag = 0;

        /* 1 收消息处理 */
        recvlen = task_recv_msg(ptMsgHandle->socketfd, recvBuf, TASK_MSG_MAX_LEN, &sender);
        if(recvlen > 0)
        {
            COMM_BUF_TRACE(pMsg->msg, pMsg->msglen, "COMM recv msg 0x%x, label %d", pMsg->msg_id, pMsg->label);
            for(i = 0; i < ptMsgHandle->nMsgNum; i++)
            {
                ptMsgProc = &(ptMsgHandle->tMsg[i]);
                if(ptMsgProc->msgid == pMsg->msg_id && (ptMsgProc->label == pMsg->label || 0xffff == ptMsgProc->label)
                    && NULL != ptMsgProc->pMsgFunc)
                {
                    ptMsgProc->pMsgFunc(pMsg->msg, pMsg->msglen, pMsg->label, ptMsgProc->pArg);
                    /* 相同msgid的消息可能有多个处理函数，不能break */
                }
            }
            memset(recvBuf, 0, recvlen);
        }
        
        /* 2 定时器 */
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        offsetTime = curtime.tv_sec - oldtime.tv_sec;
        if(offsetTime > 0)
        {
            for(i = 0; i < ptMsgHandle->nTimerNum; i++)
            {
                pTimer = &(ptMsgHandle->tTImer[i]);
                pTimer->nTimeCnt += (uint16)offsetTime;
                if(pTimer->nTimeCnt >= pTimer->nTime && NULL != pTimer->pTimeFunc)
                {
                    pTimer->pTimeFunc(ptMsgHandle->socketfd, pTimer->pArg);
                    pTimer->nTimeCnt = 0;
                }
            }
            oldtime = curtime;
        }
        usleep(10000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}

/**********************************************************************
* @name      : comm_send_syncmsg
* @brief     ：发送同步消息消息
* @param[in] ：uint32 msgid    消息ID
               uint8 *sendbuf  发送内容
               uint16 sendlen  发送长度
               uint16 buflen   接收缓存大小
* @param[out]：uint8 *recvbuf  接收内容
               uint16 *recvLen 接收长度
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-24
* @Update    :
**********************************************************************/
int comm_send_syncmsg(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen)
{
    int         ret = 0;
    uint8       recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;

    PTHREAD_LOCK_SAFE(&g_fCommMsg);
    ret = task_syncmsg_safe(g_CommTaskSync, msgid, sendbuf, sendlen, MSG_MONITOR, recvBuf, TASK_MSG_MAX_LEN, 4);
    PTHREAD_UNLOCK_SAFE(&g_fCommMsg);

    if(0 == ret)
    {
        if(buflen < pMsg->msglen)
        {
            return -100;
        }
        memcpy(recvbuf, pMsg->msg, pMsg->msglen);
        *recvLen = pMsg->msglen;
    }

    return ret;
}

/**********************************************************************
* @name      : comm_add_route_net
* @brief     ：新增路由
* @param[in] ：char *pNetName    网口名称
               uint8 *pRouteAddr  路由地址
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-18
* @Update    :
**********************************************************************/
void comm_add_route_net(const char *pNetName, uint8 *pRouteAddr)
{
    uint8  sysCom[100] = {0};
    uint8  recvbuf[32] = {0};
    uint16 offset = 0;
    int    ret = 0;
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvbuf;

    sysCom[offset++] = strlen(pNetName);
    memcpy(sysCom + offset, pNetName, sysCom[0]);
    offset += sysCom[0];
    
    sysCom[offset++] = 1;
    sysCom[offset++] = 4;
    memcpy(sysCom + offset, pRouteAddr, 4);
    offset += 4;

    sysCom[offset++] = 4;
    memset(sysCom + offset, 0xFF, 4);
    offset += 4;

    sysCom[offset++] = 4;
    memset(sysCom + offset, 0, 4);
    offset += 4;

    if(0 == ntohl(*(uint32 *)pRouteAddr))
    {
        COMM_FMT_DEBUG("%s添加路由%u.%u.%u.%u错误\n", pNetName, pRouteAddr[0], pRouteAddr[1], pRouteAddr[2], pRouteAddr[3]);
        return;
    }

    PTHREAD_LOCK_SAFE(&g_fCommMsg);
    ret = task_syncmsg_safe(g_CommTaskSync, MSG_COMM_ADDROUTE, sysCom, offset, MSG_MONITOR, recvbuf, 32, 2);
    PTHREAD_UNLOCK_SAFE(&g_fCommMsg);

    if(0 == ret)
    {
        if(0 == pMsg->msglen || FALSE == *pMsg->msg)
        {
            ret = 1;
        }
    }
    COMM_FMT_DEBUG("%s添加路由%u.%u.%u.%u%s\n", pNetName, pRouteAddr[0], pRouteAddr[1], pRouteAddr[2], pRouteAddr[3], 
                                                0 == ret ? "成功" : "失败");
}

/**********************************************************************
* @name      : comm_del_route_net
* @brief     ：删除路由
* @param[in] ：char *pNetName    网口名称
               uint8 *pRouteAddr  路由地址
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-19
* @Update    :
**********************************************************************/
void comm_del_route_net(const char *pNetName, uint8 *pRouteAddr)
{
    uint8  sysCom[100] = {0};
    uint8  recvbuf[32] = {0};
    uint16 offset = 0;
    int    ret = 0;
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvbuf;

    sysCom[offset++] = strlen(pNetName);
    memcpy(sysCom + offset, pNetName, sysCom[0]);
    offset += sysCom[0];
    
    if(NULL == pRouteAddr)
    {
        sysCom[offset++] = 0;
    }
    else
    {
        sysCom[offset++] = 1;
        sysCom[offset++] = 4;
        memcpy(sysCom + offset, pRouteAddr, 4);
        offset += 4;

        sysCom[offset++] = 4;
        memset(sysCom + offset, 0xFF, 4);
        offset += 4;

        sysCom[offset++] = 4;
        memset(sysCom + offset, 0, 4);
        offset += 4;

        if(0 == ntohl(*(uint32 *)pRouteAddr))
        {
            COMM_FMT_DEBUG("%s添加路由%u.%u.%u.%u错误\n", pNetName, pRouteAddr[0], pRouteAddr[1], pRouteAddr[2], pRouteAddr[3]);
            return;
        }
    }
    PTHREAD_LOCK_SAFE(&g_fCommMsg);
    ret = task_syncmsg_safe(g_CommTaskSync, MSG_COMM_DELROUTE, sysCom, offset, MSG_MONITOR, recvbuf, 32, 2);  
    PTHREAD_UNLOCK_SAFE(&g_fCommMsg);

    if(0 == ret)
    {
        if(0 == pMsg->msglen || FALSE == *pMsg->msg)
        {
            ret = 1;
        }
    }
    COMM_FMT_DEBUG("%s添加路由%u.%u.%u.%u%s\n", pNetName, pRouteAddr[0], pRouteAddr[1], pRouteAddr[2], pRouteAddr[3], 
                                                0 == ret ? "成功" : "失败");
}

/**********************************************************************
* @name      : comm_del_iproute_net
* @brief     ：删除ip路由
* @param[in] ：char *pNetName    网口名称
               uint32 ipaddr     ip路由地址
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-8-13
* @Update    :
**********************************************************************/
void comm_del_iproute_net(char *pNetName, uint32 ipaddr)
{
    uint8  sysCom[100] = {0};
    uint8  recvbuf[32] = {0};
    uint16 offset = 0;
    int    ret = 0;
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvbuf;

    sysCom[offset++] = strlen(pNetName);
    memcpy(sysCom + offset, pNetName, sysCom[0]);
    offset += sysCom[0];
    
    sysCom[offset++] = 1;
    sysCom[offset++] = 4;
    appmsg_memcpy_r(sysCom + offset, &ipaddr, 4);
    offset += 4;

    sysCom[offset++] = 4;
    memset(sysCom + offset, 0xFF, 4);
    offset += 4;

    sysCom[offset++] = 4;
    memset(sysCom + offset, 0, 4);
    offset += 4;

    PTHREAD_LOCK_SAFE(&g_fCommMsg);
    ret = task_syncmsg_safe(g_CommTaskSync, MSG_COMM_DELROUTE, sysCom, offset, MSG_MONITOR, recvbuf, 32, 2);  
    PTHREAD_UNLOCK_SAFE(&g_fCommMsg);

    if(0 == ret)
    {
        if(0 == pMsg->msglen || FALSE == *pMsg->msg)
        {
            ret = 1;
        }
    }
    COMM_FMT_DEBUG("%s删除路由%d.%d.%d.%d, ret %d\n", pNetName, (ipaddr >> 24) & 0xff, (ipaddr >> 16) & 0xff, 
                                                     (ipaddr >> 8) & 0xff, ipaddr & 0xff, ret);
}


/**********************************************************************
* @name      : comm_send_get_pppmode
* @brief     ：发送获取模块信息
* @param[in] ：uint8 id  模块id
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-9
* @Update    :
**********************************************************************/
void comm_send_get_pppmode(uint8 id)
{
    int    ret = 0;

    ret = task_send_labelmsg(g_CommTaskSock, MSG_COMM_GET_MODULEINFO, id, &id, sizeof(uint8), MSG_MONITOR);
    if(0 != ret)
    {
        COMM_FMT_DEBUG("Comm send get dial %d msg, ret %d\n", id, ret);
    }
}

/**********************************************************************
* @name      : comm_send_msg
* @brief     ：发送消息
* @param[in] ：uint32 msgid    消息ID
               uint8 *msgdata  消息内容
               uint16 datalen  消息长度
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-31
* @Update    :
**********************************************************************/
int comm_send_msg(uint32 msgid, uint8 *msgdata, uint16 datalen)
{
    COMM_BUF_DEBUG(msgdata, datalen, "发送APP消息%X", msgid);
    return task_send_msg(g_CommTaskSock, msgid, msgdata, datalen, MSG_MONITOR);
}

/**********************************************************************
* @name      : comm_send_label_msg
* @brief     ：发送携带label消息
* @param[in] ：uint32 msgid    消息ID
               uint16 label    label
               uint8 *msgdata  消息内容
               uint16 datalen  消息长度
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-31
* @Update    :
**********************************************************************/
int comm_send_label_msg(uint32 msgid, uint16 label, uint8 *msgdata, uint16 datalen)
{
    //COMM_BUF_DEBUG(msgdata, datalen, "COMM send msg 0x%x, label 0x%x", msgid, label);
    return task_send_labelmsg(g_CommTaskSock, msgid, label, msgdata, datalen, MSG_MONITOR);
}

/**********************************************************************
* @name      : comm_send_respmsg
* @brief     ：发送消息
* @param[in] ：uint32 msgid    消息ID
               uint8 *msgdata  消息内容
               uint16 datalen  消息长度
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-05-03
* @Update    :
**********************************************************************/
int comm_send_respmsg(uint32 msgid, uint16 label, uint8 *msgdata, uint16 datalen)
{
    COMM_BUF_DEBUG(msgdata, datalen, "COMM send respmsg 0x%x", msgid);
    return task_send_respmsg(g_CommTaskSock, msgid, label, msgdata, datalen, MSG_MONITOR);
}

