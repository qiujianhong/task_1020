/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程通用操作实现
* @date：    2019-12-3
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"

#include "macro.h"
#include "task_id.h"
#include "appmsg_split.h"
#include "storage.h"

#include "appcomm.h"
#include "appcrc16.h"
#include "AppCommon.h"
#include "msg_id.h"
#include "commClass.h"
#include "ssal.h"
#include "oopSecurity.h"

CHANNEL_CACHE_T    g_tChannel[MAX_CHANNEL_NUM];
uint8              g_tOpenNumChannel = 0;      //通道打开的总数
uint8              g_tOpenId[MAX_CHANNEL_NUM]; //打开通道的记录, 增加

uint16             g_ApduSplitDeafult = 1400;

/* 初始化读取 时时更新 */
pthread_mutex_t    g_fLocalAddr;
uint8              g_pLocalAddr[SERVER_ADDR_MAXLEN + 1] = {0};   //终端地址 第0位是长度
pthread_mutex_t    g_fClientNO;
uint8              g_pClientNO[SERVER_ADDR_MAXLEN + 1] = {0};    //终端地址 第0位是长度
pthread_mutex_t    g_fGroupAddr;
OOP_GRPADDR_T      g_tGroupAddr;
pthread_mutex_t    g_fPassword;
OOP_VISIBLEVAR32_T g_tPassword = {0, {0}};
pthread_mutex_t    g_fContext;
OOP_ASSOCIATION_CONTEXT_T g_tContext = {0};
pthread_mutex_t    g_fVersion;
Factory_Version_t  g_tVersion = {{0}, {0}, {0}, {0}, {0}, {0}};

//是否启用通信代理功能
BOOLEAN g_useCommProxy = FALSE;

//是否启用安全防护功能
BOOLEAN g_useSecureDefend = FALSE;

extern ProxyLockQueue ProxyQueue;
extern ProxyLockQueue ProxyrespondeQueue;
extern ProxyLockQueue TransQueue;
struct manual_amr_result_struc
{
    uint8   protocol;   //规约类型
    uint8   piid;       //piid
    char    state[16];  //返回结果状态
    char    time[16];   //返回结果时间
    char    data[16];   //返回结果数据(正向有功)
};
extern struct manual_amr_result_struc g_manual_amr_rst;
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
extern BOOL recvbit;
//#endif

struct line_net_result_struc
{
    uint8   protype;        //查询状态（是否超时）0,初始，1测试中2.测试完成3.测试超时
    uint8    routestate;    //返回路由学习状态 0.未完成 1.已完成 2.初始状态
    uint16  linefilenum;    //当前档案个数
    uint16  netnum;         //返回组网成功个数
};
extern struct line_net_result_struc g_line_net_rst;

extern uint32  BCD_change_to_long(uint8 *src,uint8 n);

/**********************************************************************
* @name      : app_get_initlock
* @brief     ：app读取配置初始化锁
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void app_get_initlock()
{
    pthread_mutex_init(&g_fLocalAddr, NULL);
    pthread_mutex_init(&g_fClientNO, NULL);
    pthread_mutex_init(&g_fGroupAddr, NULL);
    pthread_mutex_init(&g_fPassword, NULL);
    pthread_mutex_init(&g_fContext, NULL);
    pthread_mutex_init(&g_fVersion, NULL);
}

/**********************************************************************
* @name      : app_set_localaddr
* @brief     ：设置本地地址
* @param[in] ：uint8 len    地址长度
               uint8 *pAddr 地址内容
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
void app_set_localaddr(uint8 len, uint8 *pAddr)
{
    if(0 == len || NULL == pAddr || len > SERVER_ADDR_MAXLEN)
    {
        return;
    }
    pthread_mutex_lock(&g_fLocalAddr);
    g_pLocalAddr[0] = len;
    memcpy(g_pLocalAddr + 1, pAddr, len);
    pthread_mutex_unlock(&g_fLocalAddr);
}

/**********************************************************************
* @name      : app_get_localaddr
* @brief     ：获取本地地址
* @param[in] ：
* @param[out]：uint8 **ppAddr  地址内容
* @return    ：uint8 len       地址长度
* @Create    : 
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
uint8 app_get_localaddr(uint8 **ppAddr)
{
    if(NULL == ppAddr)
    {
        return 0;
    }
    pthread_mutex_lock(&g_fLocalAddr);
    *ppAddr = &(g_pLocalAddr[1]);
    pthread_mutex_unlock(&g_fLocalAddr);
    return g_pLocalAddr[0];
}

/**********************************************************************
* @name      : app_set_clientNO
* @brief     ：设置客户编号
* @param[in] ：uint8 len    地址长度
               uint8 *pAddr 地址内容
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2022-6-10
* @Update    :
**********************************************************************/
void app_set_clientNO(uint8 len, uint8 *pAddr)
{
    if(0 == len || NULL == pAddr || len > SERVER_ADDR_MAXLEN)
    {
        return;
    }
    pthread_mutex_lock(&g_fClientNO);
    g_pClientNO[0] = len;
    memcpy(g_pClientNO + 1, pAddr, len);
    pthread_mutex_unlock(&g_fClientNO);
}

/**********************************************************************
* @name      : app_get_clientNO
* @brief     ：获取客户编号
* @param[in] ：
* @param[out]：uint8 **ppAddr  地址内容
* @return    ：uint8 len       地址长度
* @Create    : 
* @Date      ：2022-6-10
* @Update    :
**********************************************************************/
uint8 app_get_clientNO(uint8 **ppAddr)
{
    if(NULL == ppAddr)
    {
        return 0;
    }
    pthread_mutex_lock(&g_fClientNO);
    *ppAddr = &(g_pClientNO[1]);
    pthread_mutex_unlock(&g_fClientNO);
    return g_pClientNO[0];
}

/**********************************************************************
* @name      : app_set_localaddr
* @brief     ：设置组地址
* @param[in] ：OOP_GRPADDR_T *pAddr  组地址
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-7
* @Update    :
**********************************************************************/
void app_set_groupaddr(OOP_GRPADDR_T *pAddr)
{
    if(NULL == pAddr)
    {
        return;
    }
    pthread_mutex_lock(&g_fGroupAddr);
    g_tGroupAddr = *pAddr;
    pthread_mutex_unlock(&g_fGroupAddr);
}

/**********************************************************************
* @name      : app_get_groupaddr
* @brief     ：获取组地址
* @param[in] ：
* @param[out]：OOP_GRPADDR_T *pAddr  组地址
* @return    ：
* @Create    : 
* @Date      ：2019-12-7
* @Update    :
**********************************************************************/
void app_get_groupaddr(OOP_GRPADDR_T *pAddr)
{
    if(pAddr)
    {
        pthread_mutex_lock(&g_fGroupAddr);
        memcpy(pAddr, &g_tGroupAddr, sizeof(OOP_GRPADDR_T));
        pthread_mutex_unlock(&g_fGroupAddr);
    }
}

/**********************************************************************
* @name      : app_set_password
* @brief     ：设置认证密码
* @param[in] ：OOP_VISIBLEVAR32_T *password
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-14
* @Update    :
**********************************************************************/
void app_set_password(OOP_VISIBLEVAR32_T *password)
{
    if(NULL == password)
    {
        return;
    }
    pthread_mutex_lock(&g_fPassword);
    g_tPassword = *password;
    pthread_mutex_unlock(&g_fPassword);
}

/**********************************************************************
* @name      : app_get_password
* @brief     ：获取密码
* @param[in] ：
* @param[out]：uint8 **ppPwd    密码内容地址
* @return    ：密码长度
* @Create    : 
* @Date      ：2019-12-14
* @Update    :
**********************************************************************/
uint8 app_get_password(uint8 **ppPwd)
{
    if(NULL == ppPwd)
    {
        return 0;
    }
    pthread_mutex_lock(&g_fPassword);
    *ppPwd = (uint8 *)g_tPassword.value;
    pthread_mutex_unlock(&g_fPassword);
    return g_tPassword.nNum;
}

/**********************************************************************
* @name      : app_check_password
* @brief     ：判断密码是否一致
* @param[in] ：uint8 *pPwd   密码内容地址
               uint8 len     密码长度
* @param[out]：
* @return    ：TRUE-一致/FALSE-不一致
* @Create    : 
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
BOOLEAN app_check_password(uint8 *pPwd, uint8 len)
{
    BOOLEAN ret;
    
    if(NULL == pPwd || len != g_tPassword.nNum)
    {
        return FALSE;
    }
    pthread_mutex_lock(&g_fPassword);
    ret = (0 == memcmp(pPwd, g_tPassword.value, len)) ? TRUE : FALSE;
    pthread_mutex_unlock(&g_fPassword);

    return ret;
}

/**********************************************************************
* @name      : app_set_password
* @brief     ：设置应用语境信息
* @param[in] ：OOP_VISIBLEVAR32_T *password
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
void app_set_context(OOP_ASSOCIATION_CONTEXT_T *pContext)
{
    if(NULL == pContext)
    {
        return;
    }
    pthread_mutex_lock(&g_fContext);
    g_tContext = *pContext;
    pthread_mutex_unlock(&g_fContext);
}

/**********************************************************************
* @name      : app_get_password
* @brief     ：获取应用语境信息
* @param[in] ：
* @param[out]：
* @return    &g_tContext
* @Create    : 
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
void *app_get_context()
{
    return &g_tContext;
}

/**********************************************************************
* @name      : app_set_version
* @brief     ：设置设备版本
* @param[in] ：Factory_Version_t *pVersion
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
void app_set_version(Factory_Version_t *pVersion)
{
    if(NULL == pVersion)
    {
        return;
    }
    pthread_mutex_lock(&g_fVersion);
    g_tVersion = *pVersion;

#if defined (PRODUCT_ZCU_1) || defined (PRODUCT_ZCU_3) 
	/* 保存到私有文件 */
    write_pdata_xram("/data/app/stAmr/cfg/version", (char*)&g_tVersion, 0, sizeof(Factory_Version_t));
#endif
    pthread_mutex_unlock(&g_fVersion);
}

/**********************************************************************
* @name      : app_get_version
* @brief     ：获取应用语境信息
* @param[in] ：
* @param[out]：
* @return    : &g_tVersion
* @Create    : 
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
void *app_get_version()
{
    return &g_tVersion;
}

/**********************************************************************
* @name      : app_get_opencha_num
* @brief     ：app读取打开的通道数量
* @param[in] ：
* @param[out]：
* @return    ：g_tOpenNumChannel
* @Create    : 
* @Date      ：2020-4-27
* @Update    :
**********************************************************************/
uint8 app_get_opencha_num()
{
    return g_tOpenNumChannel;
}

/**********************************************************************
* @name      : app_get_opencha_id
* @brief     ：app读取打开的通道id
* @param[in] ：uint8 no  序号
* @param[out]：
* @return    ：g_tOpenId[no]
* @Create    : 
* @Date      ：2020-4-27
* @Update    :
**********************************************************************/
uint8 app_get_opencha_id(uint8 no)
{
    if(no >= g_tOpenNumChannel)
    {
        return MAX_CHANNEL_NUM - 1;
    }
    return g_tOpenId[no];
}

/**********************************************************************
* @name      : app_init_channel
* @brief     ：初始化通道
* @param[in] ：uint32 ip     数据中心IP
               uint16 port   数据中心Port
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
void app_init_channel(uint32 ip, uint16 port)
{
    uint8 i;
    memset(g_tChannel, 0, MAX_CHANNEL_NUM * sizeof(CHANNEL_CACHE_T));
    for(i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        g_tChannel[i].id = i;
        pthread_mutex_init(&(g_tChannel[i].fWork), NULL);
        pthread_mutex_init(&(g_tChannel[i].fRecv), NULL);
        pthread_mutex_init(&(g_tChannel[i].fSend), NULL);
    }
	pthread_mutex_init(&(ProxyQueue.QueueLock), NULL);
    pthread_mutex_init(&(ProxyrespondeQueue.QueueLock), NULL);
    g_tOpenNumChannel = 0;
    memset(g_tOpenId, 0, MAX_CHANNEL_NUM);

}

uint32 CalcApplSplitLen(uint8 medium)
{
    if (medium == COMM_MEDIA_485) 
    {
        return RS485_SEND_FRAM_MINLEN;
    }
    else if (medium == COMM_MEDIA_GSM)
    {
        return SMS_FRAM_MAXLEN;
    }
    else if (medium == COMM_MEDIA_RADIO)
    {
        return RADIO_FRAM_MAXLEN;
    }
    else
    {
        return LOCAL_SEND_FRAM_MINLEN;
    }
}

/**********************************************************************
* @name      : app_open_channel
* @brief     ：打开通道
* @param[in] ：uint8 id                       通道id
               CHANNEL_INFO_T *pChannelInfo   通道配置
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
void app_open_channel(uint8 id, CHANNEL_INFO_T *pChannelInfo)
{
    uint8 i;
    struct timespec curtime = {0};
    clock_gettime(CLOCK_MONOTONIC, &curtime);

    if(id >= MAX_CHANNEL_NUM || NULL == pChannelInfo)
    {
        return;
    }
    g_tChannel[id].bOpen = TRUE;
    g_tChannel[id].tChannel = *pChannelInfo;
    if(0 == g_tChannel[id].tChannel.dailcnt)
    {
        g_tChannel[id].tChannel.dailcnt = 3;
    }
    if(0 == g_tChannel[id].tChannel.dailtime)
    {
        g_tChannel[id].tChannel.dailtime = 30;
    }
    g_tChannel[id].linkState = LINK_LOGIN;
    g_tChannel[id].leftHB = 0;
    g_tChannel[id].bNeedClose = FALSE;
    g_tChannel[id].lostHBCount = 0;
    g_tChannel[id].lostHBtime = 0;
    g_tChannel[id].heartbeatflag = 0;
    g_tChannel[id].startTime = 0;
    g_tChannel[id].endTime = 0;
    g_tChannel[id].longFlag = 0;
    g_tChannel[id].workFlag = 0;

    /* 清空 */
    app_unlock_channel(id, APP698_PROC_LOCK);

    g_tChannel[id].bSendApduBreak = 0;
    g_tChannel[id].bSafe = 0;
    g_tChannel[id].procLen = 0;
    g_tChannel[id].blocknum = 0;
    bzero(g_tChannel[id].ucMess, APP_RECV_MSG_MAXLEN);

    g_tChannel[id].msgnum = 0;
    g_tChannel[id].totalLen = 0;
    bzero(g_tChannel[id].recv, APP_RECV_IPC_MAXLEN);

    g_tChannel[id].sendMessLen = 0;
    g_tChannel[id].curblock = 0;
    g_tChannel[id].permSend = 0;
    g_tChannel[id].protocol = 0;
    bzero(g_tChannel[id].sendMess, APP_SEND_MSG_MAXLEN);

    if(NULL != g_tChannel[id].extMsg)
    {
        free(g_tChannel[id].extMsg);
        g_tChannel[id].extMsg = NULL;
    }
    g_tChannel[id].bmsgExtLen = FALSE;
    g_tChannel[id].bmsgExtOver = FALSE;
    g_tChannel[id].bMsgExtProc = FALSE;
    g_tChannel[id].recvExtLen = 0;
    g_tChannel[id].extMsgLen = 0;
    g_tChannel[id].extOutMsglen = 0;
    g_tChannel[id].extIndex = 0;
    g_tChannel[id].extSendFlag = FALSE;

    g_tChannel[id].clientid = db_client_create(DB_YC_AGENT, YC_DB_CLIENT_CHANNEL + id);
    if(g_tChannel[id].clientid <= 0)
    {
        APP698_FMT_DEBUG("Channel %d db_client_create failed!\n", id);
    }

    if(g_tChannel[id].pFreeFunc && g_tChannel[id].pApduBreak)
    {
        g_tChannel[id].pFreeFunc(&g_tChannel[id].pApduBreak);
    }

    if(g_tChannel[id].pBreakBakMsg)
    {
        free(g_tChannel[id].pBreakBakMsg);
    }
    g_tChannel[id].pBreakBakMsg = NULL;
    g_tChannel[id].BreakMsgLen = 0;
    g_tChannel[id].pApduBreak = app_apdubreak_init();
    g_tChannel[id].pFreeFunc = app_apdubreak_free;

    g_tChannel[id].tContext.ApplSplitLen = CalcApplSplitLen(pChannelInfo->medium);
    g_tChannel[id].checkTimeOK = curtime.tv_sec;

    /* 打开的通道号刷新 */
    for(i = 0; i < g_tOpenNumChannel; i++)
    {
        if(g_tOpenId[i] == id && id > 0)
        {
            return;
        }
    }
    g_tOpenId[i] = id;
    g_tOpenNumChannel++;
}

/**********************************************************************
* @name      : app_unlock_channel
* @brief     ：通道加的锁 智能解锁
* @param[in] ：uint8 id         通道id
               uint8 lockuser   加锁用户
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-7
* @Update    :
**********************************************************************/
void app_unlock_channel(uint8 id, uint8 lockuser)
{
    if(id >= MAX_CHANNEL_NUM || APP698_NO_LOCK == lockuser)
    {
        return;
    }

    if(APP698_PROC_LOCK == lockuser)
    {
        pthread_mutex_trylock(&(g_tChannel[id].fWork));
        pthread_mutex_unlock(&(g_tChannel[id].fWork));
    }
    
    if(lockuser == g_tChannel[id].recvLockUser)
    {
        pthread_mutex_trylock(&(g_tChannel[id].fRecv));
        pthread_mutex_unlock(&(g_tChannel[id].fRecv));
        g_tChannel[id].recvLockUser = APP698_NO_LOCK;
    }

    if(lockuser == g_tChannel[id].sendLockUser)
    {
        pthread_mutex_trylock(&(g_tChannel[id].fSend));
        pthread_mutex_unlock(&(g_tChannel[id].fSend));
        g_tChannel[id].sendLockUser = APP698_NO_LOCK;
    }
}

/**********************************************************************
* @name      : app_set_channelwork
* @brief     ：设置通道工作状态
* @param[in] ：uint8 id      通道id
               uint8 bwork   工作状态
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-12-12
* @Update    :
**********************************************************************/
int app_set_channelworklock(uint8 id, uint8 bwork)
{
    if(id >= MAX_CHANNEL_NUM || FALSE == g_tChannel[id].bOpen)
    {
        return APP698_ERR_NOT_OPEN;
    }
    if(TRUE == bwork)
    {
        return pthread_mutex_trylock(&(g_tChannel[id].fWork));
    }
    else
    {
        return pthread_mutex_unlock(&(g_tChannel[id].fWork));
    }
}

/**********************************************************************
* @name      : app_set_channel
* @brief     ：设置通道
* @param[in] ：uint8 id                       通道id
               CHANNEL_INFO_T *pChannelInfo   通道配置
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
void app_set_channel(uint8 id, CHANNEL_INFO_T *pChannelInfo)
{
    if(id >= MAX_CHANNEL_NUM || NULL == pChannelInfo)
    {
        return;
    }
    if(0 == pChannelInfo->dailcnt)
    {
        pChannelInfo->dailcnt = 3;
    }
    if(0 == pChannelInfo->dailtime)
    {
        pChannelInfo->dailtime = 30;
    }

    g_tChannel[id].tChannel = *pChannelInfo;
    
}

/**********************************************************************
* @name      : app_open_channel
* @brief     ：关闭通道
* @param[in] ：uint8 id                       通道id
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
void app_close_channel(uint8 id)
{
    if(id >= MAX_CHANNEL_NUM)
    {
        return;
    }
    g_tChannel[id].bOpen = FALSE;
    app_set_preconnect(id, LINK_NO, 0);//断开预连接
    g_tChannel[id].bNeedClose = FALSE;

    if(g_tChannel[id].pFreeFunc)
    {
        g_tChannel[id].pFreeFunc(&g_tChannel[id].pApduBreak);
    }
    if(g_tChannel[id].pBreakBakMsg)
    {
        free(g_tChannel[id].pBreakBakMsg);
    }
    g_tChannel[id].pBreakBakMsg = NULL;
    g_tChannel[id].BreakMsgLen = 0;

    g_tChannel[id].pApduBreak = NULL;
    g_tChannel[id].pFreeFunc = NULL;
    g_tChannel[id].longFlag = 0;
    g_tChannel[id].workFlag = 0;

    g_tChannel[id].startTime = 0;
    g_tChannel[id].endTime = 0;
    
}

/**********************************************************************
* @name      : app_set_preconnect
* @brief     ：预连接操作, 调用者加锁
* @param[in] ：uint8  id             通道id
               uint8  linkState      预连接状态
               uint16 heartbeat      心跳时间
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
void app_set_preconnect(uint8 id, uint8 linkState, uint16 heartbeat)
{
    if(id >= MAX_CHANNEL_NUM)
    {
        return;
    }
    g_tChannel[id].linkState = linkState;

    if(LINK_LOGIN == linkState)
    {
        g_tChannel[id].leftHB = heartbeat;
        
        /* 预连接后 需要分配一个最低权限的应用连接权限 Todo */
        g_tChannel[id].bConnect = TRUE;
    }
    else if(LINK_HEART == linkState)
    {
        g_tChannel[id].leftHB = heartbeat;
    }
    else
    {
        /* 断开连接 */
        g_tChannel[id].bConnect = FALSE;
        memset(&(g_tChannel[id].tContext), 0, sizeof(APP_CONTEXT_t));
        
        g_tChannel[id].bSendApduBreak = FALSE;
        g_tChannel[id].bSafe = FALSE;
        memset(g_tChannel[id].ucMess, 0, APP_RECV_MSG_MAXLEN);
        
        g_tChannel[id].procLen = 0;
        g_tChannel[id].blocknum = 0;
        memset(g_tChannel[id].recv, 0, APP_RECV_IPC_MAXLEN);
        
        g_tChannel[id].sendMessLen = 0;
        g_tChannel[id].curblock = 0;
        g_tChannel[id].permSend = 0;
        memset(g_tChannel[id].sendMess, 0, APP_SEND_MSG_MAXLEN);
    }
}

/**********************************************************************
* @name      : app_update_leftHB
* @brief     ：刷新剩余心跳
* @param[in] ：uint8  id             通道id
               uint16 heartbeat      心跳时间
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
void app_update_leftHB(uint8 id, uint16 heartbeat)
{
    if(id >= MAX_CHANNEL_NUM)
    {
        return;
    }
    g_tChannel[id].leftHB = heartbeat;
}

/**********************************************************************
* @name      : app_decrese_heartbeat
* @brief     ：获取剩余心跳
* @param[in] ：uint8  id             通道id
* @param[out]：
* @return    ：uint16                剩余心跳
* @Create    : 
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
uint16 app_get_leftHB(uint8 id)
{
    if(id >= MAX_CHANNEL_NUM)
    {
        return 0;
    }
    return g_tChannel[id].leftHB;
}

/**********************************************************************
* @name      : app_set_connect
* @brief     : 设置连接配置
* @param[in] ：uint8  id             通道id
               uint8 bConncet        连接标记 TRUE/FLASE
               APP_CONTEXT_t *pAppContext 应用连接配置
* @param[out]：
* @return    ：uint16                剩余心跳
* @Create    : 
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
void app_set_connect(uint8 id, uint8 bConncet, APP_CONTEXT_t *pAppContext)
{
    if(id >= MAX_CHANNEL_NUM)
    {
        return;
    }

    if(bConncet)
    {
        if(NULL == pAppContext)
        {
            return;
        }
        memcpy(&(g_tChannel[id].tContext), pAppContext, sizeof(APP_CONTEXT_t));
    }
    else
    {
        /* 设置应用连接为最低权限TOdo */
        app_connect_init(id);
    }

}

/**********************************************************************
* @name      : app_get_connect
* @brief     : 获取连接配置
* @param[in] ：uint8  id             通道id
* @param[out]：APP_CONTEXT_t *pAppContext 应用连接配置
* @return    ：uint8 bConncet        连接标记 TRUE/FLASE
* @Create    : 
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
BOOLEAN app_get_connect(uint8 id, APP_CONTEXT_t *pAppContext)
{
    if(id >= MAX_CHANNEL_NUM || NULL == pAppContext)
    {
        return FALSE;
    }
    memcpy(pAppContext, &(g_tChannel[id].tContext), sizeof(APP_CONTEXT_t));
    return g_tChannel[id].bConnect;
}

/**********************************************************************
* @name      : app_get_channel
* @brief     ：获取通道
* @param[in] ：uint8  id             通道id
* @param[out]：
* @return    ：CHANNEL_CACHE_T *     通道缓存
* @Create    : 
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
CHANNEL_CACHE_T *app_get_channel(uint8 id)
{
    if(id >= MAX_CHANNEL_NUM)
    {
        return NULL;
    }
    return &(g_tChannel[id]);
}

/**********************************************************************
* @name      : app_insert_recvmsg
* @brief     ：收到的报文添加到通道的队列缓存中
* @param[in] ：uint8  id             通道id
               uint8 *pMsg           报文内容
               uint16 msgLen         报文长度
               uint8  lockuser        加锁用户
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
int app_insert_recvmsg(uint8 id, uint8 *pMsg, uint16 msgLen, uint8 lockuser)
{
    struct timespec curtime = {0};
    clock_gettime(CLOCK_MONOTONIC, &curtime);

    if(id >= MAX_CHANNEL_NUM)
    {
        return APP698_ERR_ID_NOTEXIST;
    }
    else if(FALSE == g_tChannel[id].bOpen || TRUE == g_tChannel[id].bNeedClose)
    {
        return APP698_ERR_NOT_OPEN;
    }

    if(g_tChannel[id].totalLen + msgLen > APP_RECV_IPC_MAXLEN)
    {
        return APP698_ERR_CACHE_NOT_ENOUGH;
    }

    pthread_mutex_lock(&(g_tChannel[id].fRecv));
    g_tChannel[id].sendLockUser = lockuser;
    if(0 == g_tChannel[id].totalLen)
    {
        /* 队列为0需要刷新一下时间 */
        g_tChannel[id].checkTimeOK = curtime.tv_sec;
    }
    
    memcpy(g_tChannel[id].recv + g_tChannel[id].totalLen, pMsg, msgLen);
    g_tChannel[id].totalLen += msgLen;
    g_tChannel[id].msgnum++;
    
    pthread_mutex_unlock(&(g_tChannel[id].fRecv));
    g_tChannel[id].sendLockUser = APP698_NO_LOCK;

    return APP698_SECCESS;
}

/**********************************************************************
* @name      : app_get_recvmsg
* @brief     ：获取缓存队列中第一个报文
               删除缓存中开头错误的报文
* @param[in] ：uint8  id             通道id
               uint8 *pbuf           缓存
               uint16 bufLen         缓存大小
               uint8  lockuser        加锁用户
* @param[out]：
* @return    ：报文长度
* @Create    : 
* @Date      ：2019-12-3
* @Update    : 2020-4-22
**********************************************************************/
uint16 app_get_recvmsg(uint8 id, uint8 *pbuf, uint16 bufLen, uint8 lockuser)
{
    uint16 msglen = 0;
    uint16 i = 0;
    uint8  bMsgOk = FALSE;
    uint8 *pTemp = NULL;
    uint16 len = 0;
    struct timespec curtime = {0};
    clock_gettime(CLOCK_MONOTONIC, &curtime);

    if(id >= MAX_CHANNEL_NUM || FALSE == g_tChannel[id].bOpen || 0 == g_tChannel[id].totalLen)
    {
        g_tChannel[id].checkTimeOK = curtime.tv_sec;
        return 0;
    }
    
    pthread_mutex_lock(&(g_tChannel[id].fRecv));
    g_tChannel[id].sendLockUser = lockuser;

    if(g_tChannel[id].totalLen > APP_RECV_IPC_MAXLEN)
    {
        g_tChannel[id].totalLen = APP_RECV_IPC_MAXLEN;
    }

    pTemp = g_tChannel[id].recv;
    /* 去掉开头不是0x68或者长度不正确的消息内容 */
    while(i < g_tChannel[id].totalLen)
    {
        if(0x68 == pTemp[i])
        {
            if(i + APP_698_MESSAGE_MIN_LEN - 1 > g_tChannel[id].totalLen)
            {
                break;
            }
            /* 协议一致性 */
            if(0x68 == pTemp[i+1] && 0x68 == pTemp[i+2])
            {
                i++;
                continue;
            }

            /* 防止扩展长度消息帧头分报发送过来 */
            if(pTemp[i + 2] >= 0x40 && 0 == app_check_message_head(pTemp + i, g_tChannel[id].totalLen - i))
            {
                /* 先清空 解锁 再添加扩展消息 */
                uint8 recvtemp[APP_RECV_IPC_MAXLEN] = {0};
                uint16 templen = g_tChannel[id].totalLen - i;
                memcpy(recvtemp, pTemp, templen);
                memset(g_tChannel[id].recv, 0 , APP_RECV_IPC_MAXLEN);
                g_tChannel[id].totalLen = 0;
                pthread_mutex_unlock(&(g_tChannel[id].fRecv));
                g_tChannel[id].sendLockUser = APP698_NO_LOCK;
                app_externmsg_reproc(id, recvtemp, templen, lockuser);
                return 0;
            }

            len = pTemp[i + 1] + (((uint16)pTemp[i + 2]) << 8) + 2;
            /* 长度异常 */
            if(len > LOCAL_RECV_FRAM_MAXLEN || i + len - 1 >= APP_RECV_IPC_MAXLEN)
            {
                i++;
                continue;
            }
            if(i + len > g_tChannel[id].totalLen)
            {
                break;
            }
            /* 结尾存在16 */
            if(0x16 == pTemp[i + len - 1])
            {
                bMsgOk = TRUE;
                break;
            }
        }
        i++;
    }

    if(0 != i)
    {
        g_tChannel[id].checkTimeOK = curtime.tv_sec;
        if(i >= g_tChannel[id].totalLen)
        {
            g_tChannel[id].totalLen = 0;
            memset(g_tChannel[id].recv, 0, i);
            pthread_mutex_unlock(&(g_tChannel[id].fRecv));
            g_tChannel[id].sendLockUser = APP698_NO_LOCK;
            return 0;
        }
        else
        {
            memmove(g_tChannel[id].recv, g_tChannel[id].recv + i, g_tChannel[id].totalLen - i);
            g_tChannel[id].totalLen -= i;
            memset(g_tChannel[id].recv + g_tChannel[id].totalLen, 0, APP_RECV_IPC_MAXLEN - g_tChannel[id].totalLen);
        }
    }
    else if(g_tChannel[id].checkTimeOK + 10 < curtime.tv_sec)
    {
        /* 超过10s检查失败 */
        g_tChannel[id].checkTimeOK = curtime.tv_sec;
        if(TRUE != bMsgOk && 0x68 == g_tChannel[id].recv[0])
        {
            g_tChannel[id].recv[0] = 0;
        }
    }

    if(TRUE == bMsgOk)
    {
        g_tChannel[id].checkTimeOK = curtime.tv_sec;
        msglen = g_tChannel[id].recv[1] + (((uint16)g_tChannel[id].recv[2]) << 8) + 2;
        memcpy(pbuf, g_tChannel[id].recv, MIN(msglen, bufLen));
    }
    
    pthread_mutex_unlock(&(g_tChannel[id].fRecv));
    g_tChannel[id].sendLockUser = APP698_NO_LOCK;

    return MIN(msglen, bufLen);
}

/**********************************************************************
* @name      : app_get_recvmsg
* @brief     ：获取缓存队列中第一个报文
               删除缓存中开头错误的报文
* @param[in] ：uint8  id             通道id
               uint8 *pbuf           缓存
               uint16 bufLen         缓存大小
               uint8  lockuser        加锁用户
* @param[out]：
* @return    ：报文长度
* @Create    : 
* @Date      ：2019-12-3
* @Update    : 2020-4-22
**********************************************************************/
uint16 app_get_645recv(uint8 id, uint8 *pbuf, uint16 bufLen, uint8 lockuser)
{
    uint16 msglen = 0;
    uint16 i = 0;
    uint8  bMsgOk = FALSE;
    uint8 *pTemp = NULL;
    uint16 len = 0;
    struct timespec curtime = {0};
    clock_gettime(CLOCK_MONOTONIC, &curtime);

    if(id >= MAX_CHANNEL_NUM || FALSE == g_tChannel[id].bOpen || 0 == g_tChannel[id].totalLen)
    {
        g_tChannel[id].checkTimeOK = curtime.tv_sec;
        return 0;
    }
    
    pthread_mutex_lock(&(g_tChannel[id].fRecv));
    g_tChannel[id].sendLockUser = lockuser;

    if(g_tChannel[id].totalLen > APP_RECV_IPC_MAXLEN)
    {
        g_tChannel[id].totalLen = APP_RECV_IPC_MAXLEN;
    }

    pTemp = g_tChannel[id].recv;
    /* 去掉开头不是0x68或者长度不正确的消息内容 */
    while(i < g_tChannel[id].totalLen)
    {
        if(0x68 == pTemp[i])
        {
            if(i + APP_698_MESSAGE_MIN_LEN - 1 > g_tChannel[id].totalLen)
            {
                break;
            }

            //第2个0x68
            if(pTemp[i+7] != 0x68)
            {
                i++;
                continue;
            }
                
            len = pTemp[i + 9] + APP_645PRTL_MINLEN;
            
            /* 长度异常 */
            if(len > LOCAL_RECV_FRAM_MAXLEN || i + len - 1 >= APP_RECV_IPC_MAXLEN)
            {
                i++;
                continue;
            }
            if(i + len > g_tChannel[id].totalLen)
            {
                break;
            }
            
            /* 结尾存在16 */
            if(0x16 == pTemp[i + len - 1])
            {
                bMsgOk = TRUE;
                break;
            }
        }
        i++;
    }

    if(0 != i)
    {
        g_tChannel[id].checkTimeOK = curtime.tv_sec;
        if(i >= g_tChannel[id].totalLen)
        {
            g_tChannel[id].totalLen = 0;
            memset(g_tChannel[id].recv, 0, i);
            pthread_mutex_unlock(&(g_tChannel[id].fRecv));
            g_tChannel[id].sendLockUser = APP698_NO_LOCK;
            return 0;
        }
        else
        {
            memmove(g_tChannel[id].recv, g_tChannel[id].recv + i, g_tChannel[id].totalLen - i);
            g_tChannel[id].totalLen -= i;
            memset(g_tChannel[id].recv + g_tChannel[id].totalLen, 0, APP_RECV_IPC_MAXLEN - g_tChannel[id].totalLen);
        }
    }
    else if(g_tChannel[id].checkTimeOK + 10 < curtime.tv_sec)
    {
        /* 超过10s检查失败 */
        g_tChannel[id].checkTimeOK = curtime.tv_sec;
        if(TRUE != bMsgOk && 0x68 == g_tChannel[id].recv[0])
        {
            g_tChannel[id].recv[0] = 0;
        }
    }

    if(TRUE == bMsgOk)
    {
        g_tChannel[id].checkTimeOK = curtime.tv_sec;
        msglen = len;
        memcpy(pbuf, g_tChannel[id].recv, MIN(msglen, bufLen));
    }
    
    pthread_mutex_unlock(&(g_tChannel[id].fRecv));
    g_tChannel[id].sendLockUser = APP698_NO_LOCK;

    return MIN(msglen, bufLen);
}

/**********************************************************************
* @name      : app_get_recvmsg
* @brief     ：获取缓存队列中第一个报文
               删除缓存中开头错误的报文
* @param[in] ：uint8  id             通道id
               uint8 *pbuf           缓存
               uint16 bufLen         缓存大小
               uint8  lockuser        加锁用户
* @param[out]：
* @return    ：报文长度
* @Create    : 
* @Date      ：2019-12-3
* @Update    : 2020-4-22
**********************************************************************/
uint16 app_get_recvmsg_s(uint8 id, uint8 *pbuf, uint16 bufLen, uint8 lockuser)
{
    uint16 msglen = 0;
    uint16 i = 0;
    uint8  bMsgOk = FALSE;
    uint8 *pTemp = NULL;
    uint16 len = 0;

    if(id >= MAX_CHANNEL_NUM || FALSE == g_tChannel[id].bOpen || 0 == g_tChannel[id].totalLen)
    {
        return 0;
    }
    
    pthread_mutex_lock(&(g_tChannel[id].fRecv));
    g_tChannel[id].sendLockUser = lockuser;

    if(g_tChannel[id].totalLen > APP_RECV_IPC_MAXLEN)
    {
        g_tChannel[id].totalLen = APP_RECV_IPC_MAXLEN;
    }

    pTemp = g_tChannel[id].recv;
    /* 去掉开头不是0x68或者长度不正确的消息内容 */
    while(i < g_tChannel[id].totalLen)
    {
        if(0x68 == pTemp[i])
        {
            if(i + APP_698_MESSAGE_MIN_LEN - 1 > g_tChannel[id].totalLen)
            {
                break;
            }
            /* 协议一致性 */
            if(0x68 == pTemp[i+1] && 0x68 == pTemp[i+2])
            {
                i++;
                continue;
            }

            len = pTemp[i + 1] + (((uint16)pTemp[i + 2]) << 8) + 2;
            /* 长度异常 */
            if(len > LOCAL_RECV_FRAM_MAXLEN || i + len - 1 >= APP_RECV_IPC_MAXLEN)
            {
                i++;
                continue;
            }
            if(i + len > g_tChannel[id].totalLen)
            {
                break;
            }
            /* 结尾存在16 */
            if(0x16 == pTemp[i + len - 1])
            {
                bMsgOk = TRUE;
                break;
            }
        }
        i++;
    }

    if(0 != i)
    {
        if(i >= g_tChannel[id].totalLen)
        {
            g_tChannel[id].totalLen = 0;
            memset(g_tChannel[id].recv, 0, i);
            pthread_mutex_unlock(&(g_tChannel[id].fRecv));
            g_tChannel[id].sendLockUser = APP698_NO_LOCK;
            return 0;
        }
        else
        {
            memmove(g_tChannel[id].recv, g_tChannel[id].recv + i, g_tChannel[id].totalLen - i);
            g_tChannel[id].totalLen -= i;
            memset(g_tChannel[id].recv + g_tChannel[id].totalLen, 0, APP_RECV_IPC_MAXLEN - g_tChannel[id].totalLen);
        }
    }

    if(TRUE == bMsgOk)
    {
        msglen = g_tChannel[id].recv[1] + (((uint16)g_tChannel[id].recv[2]) << 8) + 2;
        memcpy(pbuf, g_tChannel[id].recv, MIN(msglen, bufLen));
    }
    
    pthread_mutex_unlock(&(g_tChannel[id].fRecv));
    g_tChannel[id].sendLockUser = APP698_NO_LOCK;

    return MIN(msglen, bufLen);
}

/**********************************************************************
* @name      : app_remove_firstrecvmsg
* @brief     ：清除缓存队列中第一个报文
               增加清除无效报文的操作 从尾部减小
* @param[in] ：uint8  id             通道id
               uint8 lockuser        加锁用户
* @param[out]：
* @return    ：报文长度
* @Create    : 
* @Date      ：2019-12-3
* @Update    : 2020-4-22
**********************************************************************/
void app_remove_firstrecvmsg(uint8 id, uint8 lockuser)
{
    uint16 msglen = 0;
    struct timespec curtime = {0};
    clock_gettime(CLOCK_MONOTONIC, &curtime);

    if(id >= MAX_CHANNEL_NUM)
    {
        return;
    }
    
    if(TRUE == g_tChannel[id].bmsgExtOver && TRUE == g_tChannel[id].bMsgExtProc)
    {
        if(g_tChannel[id].extMsg)
        {
            free(g_tChannel[id].extMsg);
        }
        g_tChannel[id].extMsg = NULL;
        g_tChannel[id].recvExtLen = 0;
        g_tChannel[id].extMsgLen = 0;
        g_tChannel[id].bmsgExtLen = FALSE;
        g_tChannel[id].bmsgExtOver = FALSE;
        g_tChannel[id].bMsgExtProc = FALSE;
        return;
    }

    pthread_mutex_lock(&(g_tChannel[id].fRecv));
    g_tChannel[id].sendLockUser = lockuser;
    
    msglen = g_tChannel[id].recv[1] + (((uint16)g_tChannel[id].recv[2]) << 8) + 2;

    if(msglen > g_tChannel[id].totalLen)
    {
        msglen = g_tChannel[id].totalLen;
        g_tChannel[id].totalLen = 0;
        memset(g_tChannel[id].recv, 0, msglen);
    }
    else
    {
        memmove(g_tChannel[id].recv, g_tChannel[id].recv + msglen, g_tChannel[id].totalLen - msglen);
        g_tChannel[id].totalLen -= msglen;
        memset(g_tChannel[id].recv + g_tChannel[id].totalLen, 0, APP_RECV_IPC_MAXLEN - g_tChannel[id].totalLen);
    }
    /* 队列移除需要刷新一下时间 */
    g_tChannel[id].checkTimeOK = curtime.tv_sec;

    pthread_mutex_unlock(&(g_tChannel[id].fRecv));
    g_tChannel[id].sendLockUser = APP698_NO_LOCK;
}

/**********************************************************************
* @name      : app_externmsg_reproc
* @brief     ：扩展698报文预处理
* @param[in] ：uint8 id              通道
               uint8 *pMsg           报文内容
               uint16 msgLen         报文长度
               uint8 lockuser        加锁用户
* @param[out]：
* @return    ：错误码
* @Create    : 
* @Date      ：2020-7-20
* @Update    :
**********************************************************************/
int app_externmsg_reproc(uint8 id, uint8 *pMsg, uint16 msgLen, uint8 lockuser)
{
    CHANNEL_CACHE_T *ptChannel = app_get_channel(id);

    if(NULL == ptChannel || NULL == pMsg || 0 == msgLen)
    {
        return APP698_ERR_INPUT;
    }

    if(FALSE == ptChannel->bOpen || TRUE == ptChannel->bNeedClose)
    {
        return APP698_ERR_NOT_OPEN;
    }

    if(TRUE == ptChannel->bmsgExtLen)
    {
        if(TRUE == ptChannel->bmsgExtOver)
        {
            /* 已经存在完整的消息先不管 */
            return APP698_SECCESS;
        }

        /* 已经是在处理超大消息 */
        if(ptChannel->recvExtLen + msgLen > ptChannel->extMsgLen)
        {
            uint16 newLen = ptChannel->recvExtLen + msgLen - ptChannel->extMsgLen;
            uint8 *pNewMsg = pMsg + msgLen - newLen;
            /* 超过长度的消息添加到缓存中 */
            app_insert_recvmsg(id, pNewMsg, newLen, lockuser);
            memcpy(ptChannel->extMsg + ptChannel->recvExtLen, pMsg, msgLen - newLen);
            ptChannel->recvExtLen += msgLen - newLen;
        }
        else
        {
            memcpy(ptChannel->extMsg + ptChannel->recvExtLen, pMsg, msgLen);
            ptChannel->recvExtLen += msgLen;
        }

        if(ptChannel->recvExtLen >= ptChannel->extMsgLen)
        {
            ptChannel->bmsgExtOver = TRUE;  //设置接收完成标记
            return APP698_ERR_EXT_MSG_OVER;
        }
        ptChannel->leftHB = CHANNEL_GET_HEART(ptChannel);
    }
    else
    {
        /* 判断是否扩展长度消息 */
        if( 0x68 == *pMsg && 0x68 == pMsg[1] && 0x68 == pMsg[2])
        {
            /* 协议一致性 */
            return APP698_ERR_NOT_EXT_MSG;
        }

        if(msgLen > 3 && 0x68 == *pMsg && pMsg[2] >= 0x40 && 
            (COMM_MEDIA_485       != ptChannel->tChannel.medium && 
             COMM_MEDIA_BLUETOOTH != ptChannel->tChannel.medium &&
             COMM_MEDIA_RADIO     != ptChannel->tChannel.medium &&
             COMM_MEDIA_232       != ptChannel->tChannel.medium &&
             CHL_ACMETER          != ptChannel->id))
        {
            if(0 != app_check_message_head(pMsg, msgLen))
            {
                return APP698_ERR_NOT_EXT_MSG;
            }
            if(NULL == ptChannel->extMsg)
            {
                free(ptChannel->extMsg);
            }
            ptChannel->extMsgLen = ((((uint16)(pMsg[2] & 0x3f)) << 8) + pMsg[1]) * ((uint32)1024) + 2;
            ptChannel->extMsg = malloc(ptChannel->extMsgLen);
            if(NULL == ptChannel->extMsg)
            {
                /* 失败就放到缓存中处理 */
                return FALSE;
            }
            memset(ptChannel->extMsg, 0, ptChannel->extMsgLen);
            memcpy(ptChannel->extMsg, pMsg, msgLen);

            ptChannel->bmsgExtLen = TRUE;
            ptChannel->bmsgExtOver = FALSE;
            ptChannel->bMsgExtProc = FALSE;
            ptChannel->recvExtLen = msgLen;
            ptChannel->leftHB = CHANNEL_GET_HEART(ptChannel);

            /* 防止收到的就是整个大消息 */
            if(ptChannel->recvExtLen >= ptChannel->extMsgLen)
            {
                ptChannel->bmsgExtOver = TRUE;  //设置接收完成标记
                return APP698_ERR_EXT_MSG_OVER;
            }
            return APP698_ERR_EXT_MSG_FIRST;
        }
        else
        {
            return APP698_ERR_NOT_EXT_MSG;
        }
    }

    return APP698_SECCESS;
}

/**********************************************************************
* @name      : app_check_message
* @brief     ：报文合法性检查
* @param[in] ：uint8 *pMsg           报文内容
               uint16 msgLen         报文长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
int app_check_message(uint8 *pMsg, uint32 msgLen)
{
    uint32 len = 0;
    uint8 *pTemp = NULL;
    uint16 CRC = 0;
    uint16 CS = 0;

    if(NULL == pMsg || msgLen < APP_698_MESSAGE_MIN_LEN)
    {
        return APP698_ERR_INPUT;
    }
    
    /* 1 检查首尾 */
    if(0x68 != *pMsg || 0x16 != pMsg[msgLen-1])
    {
        return APP698_ERR_MSG_HEAD_TAIL;
    }

    /* 2 检查长度 */
    if(pMsg[2] >= 0x40)
    {
        len = pMsg[2] & 0x3f;
        len = (len << 8) + pMsg[1];
        len = len * 1024 +2;
    }
    else
    {
        len = pMsg[1] + (((uint16)pMsg[2]) << 8) + 2;
    }
    if(len != msgLen)
    {
        return APP698_ERR_MSG_TOO_LONG;
    }

    /* 3 帧头校验 HCS */
    pTemp = pMsg + APP_698_A_REGIN_OFFSET;
    len = APP_698_LEN_REGIN_SIZE + APP_698_CTL_REGIN_SIZE + (*pTemp & 0x0f) + 2 + APP_698_CA_REGIN_SIZE; //帧长度
    CRC = app_crc16_get(pMsg + 1, len);
    pTemp = pMsg + 1 + len;
    CS = *pTemp + ((uint16)*(pTemp + 1)) * 0x100;
    if(CRC != CS)
    {
        return APP698_ERR_HCS;
    }

    /* 4 帧校验FCS */
    CRC = app_crc16_get(pMsg + 1, msgLen - 4);
    pTemp = pMsg + msgLen - 3;
    CS = *pTemp + ((uint16)*(pTemp + 1)) * 0x100;
    if(CRC != CS)
    {
        return APP698_ERR_FCS;
    }

    return 0;
}

/**********************************************************************
* @name      : app_check_message_head
* @brief     ：报文针头合法性检查
* @param[in] ：uint8 *pMsg           报文内容
               uint16 msgLen         报文长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2022-7-23
* @Update    :
**********************************************************************/
int app_check_message_head(uint8 *pMsg, uint32 msgLen)
{
    uint32 len = 0;
    uint8 *pTemp = NULL;
    uint16 CRC = 0;
    uint16 CS = 0;

    if(NULL == pMsg || msgLen < APP_698_HEAD_MIN_LEN)
    {
        return APP698_ERR_INPUT;
    }
    
    /* 1 检查首尾 */
    if(0x68 != *pMsg)
    {
        return APP698_ERR_MSG_HEAD_TAIL;
    }

    /* 2 帧头校验 HCS */
    pTemp = pMsg + APP_698_A_REGIN_OFFSET;
    len = APP_698_LEN_REGIN_SIZE + APP_698_CTL_REGIN_SIZE + (*pTemp & 0x0f) + 2 + APP_698_CA_REGIN_SIZE; //帧长度
    if(len + 3 > msgLen)
    {
        return APP698_ERR_CACHE_NOT_ENOUGH;
    }
    
    CRC = app_crc16_get(pMsg + 1, len);
    pTemp = pMsg + 1 + len;
    CS = *pTemp + ((uint16)*(pTemp + 1)) * 0x100;
    if(CRC != CS)
    {
        return APP698_ERR_HCS;
    }

    return 0;
}


/**********************************************************************
* @name      : app_cut_message
* @brief     ：报文去除头尾非法字符
* @param[in] ：uint8 *pMsg           报文内容
               uint16 msgLen         报文长度
* @param[out]：uint16 *offset        偏移长度
               uint16 *newlen        新消息长度
* @return    ：TRUE-成功/FALSE-失败
* @Create    : 
* @Date      ：2020-3-31
* @Update    :
**********************************************************************/
BOOLEAN app_cut_message(uint8 *pMsg, uint16 msgLen, uint16 *offset, uint16 *newlen)
{
    uint16 i = 0;
    uint16 leftlen = msgLen;
    uint16 len = 0;
    uint8 *pTemp = pMsg;

    while(i < msgLen && leftlen >= APP_698_MESSAGE_MIN_LEN)
    {
        if(0x68 != pTemp[i])
        {
            i++;
            leftlen--;
            continue;
        }
        len = pTemp[i + 1] + (((uint16)pTemp[i + 2]) << 8) + 2;
        if(len > leftlen || 0x16 != pTemp[i + len - 1])
        {
            i++;
            leftlen--;
            continue;
        }
        if(0 == app_check_message(pTemp + i, len))
        {
            *offset = i;
            *newlen = len;
            return TRUE;
        }
        else
        {
            i++;
            leftlen--;
            continue;
        }
    }
    return FALSE;
}

/**********************************************************************
* @name      : app_get_frame
* @brief     ：解析报文帧头数据, 同时处理扰码
* @param[in] ：uint8 *pMsg           报文内容
               uint32 msgLen         报文长度
               BOOLEAN bSC           SC是否转换
* @param[out]：FRAME_HEAD_T *pFrame  帧头信息
* @return    ：错误码
* @Create    : 
* @Date      ：2019-12-4
* @Update    :
**********************************************************************/
int app_get_frame(uint8 *pMsg, uint32 msgLen, FRAME_HEAD_T *pFrame, BOOLEAN bSC)
{
    uint8 *temp = NULL;
    uint16 i;

    if(NULL == pMsg || msgLen < APP_698_MESSAGE_MIN_LEN || NULL == pFrame)
    {
        return APP698_ERR_INPUT;
    }

    /*长度*/
    temp = pMsg + 1;
    pFrame->msglen = *temp + (((uint16)temp[1]) << 8);
    if(temp[1] >= 0x40)
    {
        /* 扩展长度 */
        pFrame->msglen = (pFrame->msglen & 0x3fff) * 1024;
        pFrame->bExtmsg = 1;
    }
    else
    {
        pFrame->bExtmsg = 0;
    }
    
    temp += APP_698_LEN_REGIN_SIZE;

    /* 控制域 */
    pFrame->funCode = *temp & 0x07;
    pFrame->sc = (*temp >> 3) & 0x01;
    pFrame->separate = (*temp >> 5) & 0x01;
    pFrame->prm = (*temp >> 6) & 0x01;
    pFrame->dir = (*temp >> 7) & 0x01;
    temp += APP_698_CTL_REGIN_SIZE;

    /* SA */
    pFrame->sAddr.len = (*temp & 0x0f) + 1;
    pFrame->sAddr.logicAddr = (*temp >> 4) & 0x03;
    pFrame->sAddr.type = (*temp >> 6) & 0x03;
    /* 逻辑地址扩展 bit5=1 后面一个字节是扩展逻辑地址(2-255) */
    if(pFrame->sAddr.logicAddr > 1)
    {
        temp++;
        pFrame->sAddr.logicAddr = *temp;
        pFrame->sAddr.len -= 1;
    }
    temp++;
    appmsg_memcpy_r(pFrame->sAddr.addr, temp, pFrame->sAddr.len);
    temp += pFrame->sAddr.len;

    /* CA */
    pFrame->cAddr = *temp;
    temp++;

    /* 计算帧头长度 */
    temp += APP_698_CS_REGIN_SIZE;  //HCS
    pFrame->headlen = (uint8)(temp - pMsg) - 1;

    /* APDU */
    pFrame->pApdu = temp;
    pFrame->apduLen = pFrame->msglen - pFrame->headlen - 2;

    /* 扰码处理 */
    if(pFrame->sc && TRUE == bSC)
    {
        for(i = 0; i < pFrame->apduLen; i++)
        {
            pFrame->pApdu[i] -= 0x33;
        }
    }

    /* 分帧格式解析 */
    if(pFrame->separate)
    {
        pFrame->tSepara.block = *temp + (((uint16)(*(temp + 1)&0x0f)) << 8);
        pFrame->tSepara.type = (*(temp + 1)&0xf0) >> 6;
        pFrame->pApdu += 2;
        pFrame->apduLen -= 2;
    }

    return APP698_SECCESS;
}

/**********************************************************************
* @name      : app_frame_to_package
* @brief     ：组装报文, 帧头大小不需要准确填写(不支持长度扩展)
* @param[in] ：FRAME_HEAD_T *pFrame           帧头信息
               uint16 bufLen                  buf大小
               BOOLEAN bSC                       是否处理扰码
* @param[out]：uint8 *pBuf                    报文缓存
* @return    ：消息长度
* @Create    : 
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
uint16 app_frame_to_package(FRAME_HEAD_T *pFrame, uint8 *pBuf, uint16 bufLen, BOOLEAN bSC)
{
    uint8 *temp = NULL;
    uint16 msgLen = 0;
    uint16 CRC = 0;
    uint16 i;

    if(NULL == pFrame || TRUE == pFrame->bExtmsg || NULL == pBuf || pFrame->headlen + pFrame->apduLen + 4 > bufLen 
       || pFrame->headlen < APP_698_HEAD_MIN_LEN)
    {
        return 0;
    }
    temp = pBuf;

    /* 起始字符 */
    *temp = 0x68;
    temp++;

    /* 长度域L */
    *temp = pFrame->msglen & 0xff;
    *(temp+1) = (pFrame->msglen & 0x3f00) >> 8;
    temp += APP_698_LEN_REGIN_SIZE;

    /* 控制域C */
    *temp = (pFrame->funCode & 0x07) + (pFrame->sc << 3) + (pFrame->separate << 5) +\
            (pFrame->prm << 6) + (pFrame->dir << 7);
    temp += APP_698_CTL_REGIN_SIZE;

    /* SA */
    if(pFrame->sAddr.logicAddr > 1)
    {
        /* 扩展逻辑地址 */
        *temp = (pFrame->sAddr.len & 0x0f) + (0x01 << 5) + ((pFrame->sAddr.type & 0x03) << 6);
        temp++;
        *temp = pFrame->sAddr.logicAddr;
    }
    else
    {
        *temp = ((pFrame->sAddr.len - 1) & 0x0f) + (pFrame->sAddr.logicAddr << 4) + ((pFrame->sAddr.type & 0x03) << 6);
    }
    temp++;
    appmsg_memcpy_r(temp, pFrame->sAddr.addr, pFrame->sAddr.len);
    temp += pFrame->sAddr.len;

    /* CA */
    *temp = pFrame->cAddr;
    temp += APP_698_CA_REGIN_SIZE;

    /* 先跳过HCS */
    msgLen = (uint8)(temp - pBuf - 1) + APP_698_CS_REGIN_SIZE;
    temp += APP_698_CS_REGIN_SIZE;
    pFrame->headlen = msgLen;

    /* 分帧格式 */
    if(pFrame->separate)
    {
        if(pFrame->headlen + 1 + APP_698_SEPARA_REGIN_SIZE > bufLen)
        {
            return 0;
        }
        *temp = pFrame->tSepara.block & 0xff;
        *(temp+1) = ((pFrame->tSepara.block >> 8) & 0x0f) + ((pFrame->tSepara.type << 6) & 0xc0);
        msgLen += APP_698_SEPARA_REGIN_SIZE;
        temp += APP_698_SEPARA_REGIN_SIZE;
    }
    
    if(1 + msgLen + pFrame->apduLen + APP_698_CS_REGIN_SIZE + 1 > bufLen)
    {
        return 0;
    }

    /* APDU内容 */
    if(0 != pFrame->apduLen && NULL != pFrame->pApdu)
    {
        memcpy(temp, pFrame->pApdu, pFrame->apduLen);
        temp += pFrame->apduLen;
        msgLen += pFrame->apduLen;
    }

    /* 扰码处理 */
    if(pFrame->sc && TRUE == bSC)
    {
        for(i = 1 + pFrame->headlen; i < 1 + msgLen; i++)
        {
            *(pBuf + i) += 0x33;
        }
    }

    /* 跳过FCS */
    temp += APP_698_CS_REGIN_SIZE;
    msgLen += APP_698_CS_REGIN_SIZE;

    /* 结束标记 */
    *temp = 0x16;

    /* 长度域回填 */
    pBuf[1] = msgLen & 0xff;
    pBuf[2] = (msgLen & 0x3f00) >> 8;
    
    /* 计算HCS */
    CRC = app_crc16_get(pBuf+1, pFrame->headlen - APP_698_CS_REGIN_SIZE);
    temp = pBuf + 1 + pFrame->headlen - APP_698_CS_REGIN_SIZE;
    *temp = CRC & 0xff;
    *(temp+1) = (CRC >> 8) & 0xff;

    /* 计算FCS */
    CRC = app_crc16_get(pBuf+1, msgLen - APP_698_CS_REGIN_SIZE);
    temp = pBuf + 1 + msgLen - APP_698_CS_REGIN_SIZE;
    *temp = CRC & 0xff;
    *(temp+1) = (CRC >> 8) & 0xff;

    pFrame->msglen = msgLen;
    
    return msgLen + 2;
}

/**********************************************************************
* @name      : app_frame_to_bigpackage
* @brief     ：长度扩展报文封装, 帧头大小不长度需要准确填写
* @param[in] ：FRAME_HEAD_T *pFrame           帧头信息
               uint32 tarMsgLen               目标消息大小(不包括起止位, 必须是1024的整数倍)
               BOOLEAN bSC                    是否处理扰码
* @param[out]：uint8 **ppBigMsg               消息内容, 使用后需要进行free操作
               uint32 *pApduLeft              剩余没有封装的APDU
* @return    ：报文长度
* @Create    : 
* @Date      ：2020-7-22
* @Update    :
**********************************************************************/
uint32 app_frame_to_bigpackage(FRAME_HEAD_T *pFrame, uint8 **ppBigMsg, uint32 tarMsgLen, uint32 *pApduLeft, BOOLEAN bSC)
{
    uint8 *pMsg = NULL;
    uint8 *pTemp = NULL;
    uint16 tempLen = 0;
    uint32 apduLen = 0;
    uint8 *pApduTemp = NULL;
    uint16 CRC = 0;

    if(NULL == pFrame || NULL == ppBigMsg || NULL == pApduLeft || NULL == pFrame->pApdu)
    {
        return 0;
    }

    /* 校验长度 */
    #if LONG_ENCRYPTION_EN == 1
    if(FALSE == pFrame->bExtmsg || (tarMsgLen / 1024) > APP_SEND_MSG_MAXLEN)
    #else
    if(FALSE == pFrame->bExtmsg || (0 != tarMsgLen % 1024 || tarMsgLen / 1024 > APP_SEND_MSG_MAXLEN))
    #endif
    {
        return 0;
    }
    /* apdu长度不足 */
    tempLen = APUD_COUNT_FRAME_LEN(pFrame->sAddr.len, pFrame->sAddr.logicAddr);
    if(tempLen + pFrame->apduLen + APP_698_CS_REGIN_SIZE < tarMsgLen)
    {
        return 0;
    }

    pMsg = (uint8*)malloc(tarMsgLen + 2);
    if(NULL == pMsg)
    {
        return 0;
    }
    memset(pMsg, 0, tarMsgLen + 2);
    pTemp = pMsg;

    /* 起始字符 */
    *pTemp = 0x68;
    pTemp++;

    /* 长度域L */
    pFrame->msglen = tarMsgLen;
    tempLen = ((tarMsgLen / 1024) & 0x3fff) + 0x4000;
    memcpy(pTemp, &tempLen, sizeof(uint16));
    pTemp += sizeof(uint16);

    /* 控制域C */
    *pTemp = (pFrame->funCode & 0x07) + (pFrame->sc << 3) + (pFrame->separate << 5) +\
            (pFrame->prm << 6) + (pFrame->dir << 7);
    pTemp += APP_698_CTL_REGIN_SIZE;

    /* SA */
    if(pFrame->sAddr.logicAddr > 1)
    {
        /* 扩展逻辑地址 */
        *pTemp = (pFrame->sAddr.len & 0x0f) + (0x01 << 5) + ((pFrame->sAddr.type & 0x03) << 6);
        pTemp++;
        *pTemp = pFrame->sAddr.logicAddr;
    }
    else
    {
        *pTemp = ((pFrame->sAddr.len - 1) & 0x0f) + (pFrame->sAddr.logicAddr << 4) + ((pFrame->sAddr.type & 0x03) << 6);
    }
    pTemp++;
    appmsg_memcpy_r(pTemp, pFrame->sAddr.addr, pFrame->sAddr.len);
    pTemp += pFrame->sAddr.len;

    /* CA */
    *pTemp = pFrame->cAddr;
    pTemp += APP_698_CA_REGIN_SIZE;
    
    /* 计算HCS */
    tempLen = (uint16)(pTemp - pMsg) - 1;
    CRC = app_crc16_get(pMsg + 1, tempLen);
    *pTemp = CRC & 0xff;
    *(pTemp+1) = (CRC >> 8) & 0xff;

    pFrame->headlen = tempLen + APP_698_CS_REGIN_SIZE;
    pTemp += APP_698_CS_REGIN_SIZE;

    /* 不需要支持链路分帧 */
   
    /* APDU内容 */
    apduLen = tarMsgLen - APP_698_CS_REGIN_SIZE - pFrame->headlen;
    if(apduLen > pFrame->apduLen)
    {
        free(pMsg);
        return 0;
    }
    memcpy(pTemp, pFrame->pApdu, apduLen);
    pApduTemp = pTemp;
    pTemp += apduLen;

    /* 扰码处理 */
    if(pFrame->sc && TRUE == bSC)
    {
        while(pApduTemp < pTemp)
        {
            *pApduTemp += 0x33;
            pApduTemp++;
        }
    }

    /* 计算FCS */
    CRC = app_crc16_get(pMsg + 1, pFrame->headlen + apduLen);

    *pTemp = CRC & 0xff;
    *(pTemp+1) = (CRC >> 8) & 0xff;
    pTemp += APP_698_CS_REGIN_SIZE;

    /* 结束标记 */
    *pTemp = 0x16;

    *pApduLeft = pFrame->apduLen - apduLen;
    *ppBigMsg = pMsg;
    return tarMsgLen + 2;
}

/**********************************************************************
* @name      : app_frame_to_package
* @brief     ：组装报文, 帧头大小不需要准确填写(不支持长度扩展)
* @param[in] ：FRAME_HEAD_T *pFrame           帧头信息
               uint16 bufLen                  buf大小
               BOOLEAN bSC                       是否处理扰码
* @param[out]：uint8 *pBuf                    报文缓存
* @return    ：消息长度
* @Create    : 
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
uint32 app_frame_to_adaptpackage(FRAME_HEAD_T *pFrame, uint8 *pBuf, uint32 bufLen, BOOLEAN bSC)
{
    uint8 *temp = NULL;
    uint32 msgLen = 0;
    uint16 CRC = 0;
    uint16 i;
    uint16 tempLen = 0;

    if(NULL == pFrame || NULL == pBuf || pFrame->headlen + pFrame->apduLen + 4 > bufLen 
       || pFrame->headlen < APP_698_HEAD_MIN_LEN)
    {
        return 0;
    }
    temp = pBuf;

    /* 起始字符 */
    *temp = 0x68;
    temp++;

    /* 长度域L */
    /*if (pFrame->bExtmsg)
    {
        //计算长度L
        pFrame->msglen = pFrame->headlen + pFrame->apduLen;
        if (pFrame->msglen%1024 != 0)
        {
            memset(pFrame->pApdu + pFrame->apduLen, 0, 1024 - (pFrame->msglen%1024));
            pFrame->apduLen += 1024 - (pFrame->msglen%1024);
            pFrame->msglen = (pFrame->msglen/1024 + 1)*1024;
        }
        tempLen = ((pFrame->msglen / 1024) & 0x3fff) + 0x4000;
        memcpy(temp, &tempLen, sizeof(uint16));
    }
    else
    {*/
        *temp = pFrame->msglen & 0xff;
        *(temp+1) = (pFrame->msglen & 0x3f00) >> 8;
    //}

    temp += APP_698_LEN_REGIN_SIZE;

    /* 控制域C */
    *temp = (pFrame->funCode & 0x07) + (pFrame->sc << 3) + (pFrame->separate << 5) +\
            (pFrame->prm << 6) + (pFrame->dir << 7);
    temp += APP_698_CTL_REGIN_SIZE;

    /* SA */
    if(pFrame->sAddr.logicAddr > 1)
    {
        /* 扩展逻辑地址 */
        *temp = (pFrame->sAddr.len & 0x0f) + (0x01 << 5) + ((pFrame->sAddr.type & 0x03) << 6);
        temp++;
        *temp = pFrame->sAddr.logicAddr;
    }
    else
    {
        *temp = ((pFrame->sAddr.len - 1) & 0x0f) + (pFrame->sAddr.logicAddr << 4) + ((pFrame->sAddr.type & 0x03) << 6);
    }
    temp++;
    appmsg_memcpy_r(temp, pFrame->sAddr.addr, pFrame->sAddr.len);
    temp += pFrame->sAddr.len;

    /* CA */
    *temp = pFrame->cAddr;
    temp += APP_698_CA_REGIN_SIZE;

    /* 先跳过HCS */
    msgLen = (uint8)(temp - pBuf - 1) + APP_698_CS_REGIN_SIZE;
    temp += APP_698_CS_REGIN_SIZE;
    pFrame->headlen = msgLen;

    /* 分帧格式 */
    if(pFrame->separate)
    {
        if(pFrame->headlen + 1 + APP_698_SEPARA_REGIN_SIZE > bufLen)
        {
            return 0;
        }
        *temp = pFrame->tSepara.block & 0xff;
        *(temp+1) = ((pFrame->tSepara.block >> 8) & 0x0f) + ((pFrame->tSepara.type << 6) & 0xc0);
        msgLen += APP_698_SEPARA_REGIN_SIZE;
        temp += APP_698_SEPARA_REGIN_SIZE;
    }
    
    if(1 + msgLen + pFrame->apduLen + APP_698_CS_REGIN_SIZE + 1 > bufLen)
    {
        return 0;
    }

    /* APDU内容 */
    if(0 != pFrame->apduLen && NULL != pFrame->pApdu)
    {
        memcpy(temp, pFrame->pApdu, pFrame->apduLen);
        temp += pFrame->apduLen;
        msgLen += pFrame->apduLen;
    }

    /* 扰码处理 */
    if(pFrame->sc && TRUE == bSC)
    {
        for(i = 1 + pFrame->headlen; i < 1 + msgLen; i++)
        {
            *(pBuf + i) += 0x33;
        }
    }

    /* 跳过FCS */
    temp += APP_698_CS_REGIN_SIZE;
    msgLen += APP_698_CS_REGIN_SIZE;

    /* 结束标记 */
    *temp = 0x16;

    /* 长度域回填 */
    if (pFrame->bExtmsg)
    {
        //重新计算长度L
        if (msgLen%1024 != 0)
        {
            memset(pFrame->pApdu + pFrame->apduLen, 0, 1024 - (msgLen%1024));
            pFrame->apduLen += 1024 - (msgLen%1024);
            msgLen = (msgLen/1024 + 1)*1024;
        }
        tempLen = ((msgLen / 1024) & 0x3fff) + 0x4000;
        memcpy(&pBuf[1], &tempLen, sizeof(uint16));
    }
    else
    {
        pBuf[1] = msgLen & 0xff;
        pBuf[2] = (msgLen & 0x3f00) >> 8;
    }

    /* 计算HCS */
    CRC = app_crc16_get(pBuf+1, pFrame->headlen - APP_698_CS_REGIN_SIZE);
    temp = pBuf + 1 + pFrame->headlen - APP_698_CS_REGIN_SIZE;
    *temp = CRC & 0xff;
    *(temp+1) = (CRC >> 8) & 0xff;

    /* 计算FCS */
    CRC = app_crc16_get(pBuf+1, msgLen - APP_698_CS_REGIN_SIZE);
    temp = pBuf + 1 + msgLen - APP_698_CS_REGIN_SIZE;
    *temp = CRC & 0xff;
    *(temp+1) = (CRC >> 8) & 0xff;

    pFrame->msglen = msgLen;
    
    return msgLen + 2;
}

/**********************************************************************
* @name      : app_log_channel_recvbuf
* @brief     ：日志输出通道前256个内容
* @param[in] ：uint8 id        通道ID
               uint8 lockuser  加锁者
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-5
* @Update    :
**********************************************************************/
void app_log_channel_recvbuf(uint8 id, uint8 lockuser)
{
    if(0 == id || id >= MAX_CHANNEL_NUM || FALSE == g_tChannel[id].bOpen || FALSE == g_tChannel[id].bConnect)
    {
        return;
    }

    pthread_mutex_lock(&(g_tChannel[id].fRecv));
    g_tChannel[id].sendLockUser = lockuser;
    
    APP698_BUF_DEBUG(g_tChannel[id].recv, MIN(256, g_tChannel[id].totalLen), 
                     "Channel %d recvbuf len %d:", id, g_tChannel[id].totalLen);

    pthread_mutex_unlock(&(g_tChannel[id].fRecv));
    g_tChannel[id].sendLockUser = APP698_NO_LOCK;
}

/**********************************************************************
* @name      : app_channel_starttime
* @brief     ：设置通道开始处理时间
* @param[in] ：uint8 id        通道ID
               uint8 bStart    是否开始
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-5
* @Update    :
**********************************************************************/
void app_channel_proctime(uint8 id, uint8 bStart)
{
    struct timespec curtime = {0};

    if(0 == id || id >= MAX_CHANNEL_NUM || FALSE == g_tChannel[id].bOpen || FALSE == g_tChannel[id].bConnect)
    {
        return;
    }

    clock_gettime(CLOCK_MONOTONIC, &curtime);
    if(TRUE == bStart)
    {
        g_tChannel[id].startTime = (uint32)curtime.tv_sec;
        g_tChannel[id].workFlag = 1;
    }
    else
    {
        g_tChannel[id].endTime = (uint32)curtime.tv_sec;
        g_tChannel[id].workFlag = 0;
    }
    g_tChannel[id].longFlag = 0;
}

/**********************************************************************
* @name      : app_bluetooth_mac
* @brief     ：去掉蓝牙开头的FE, 判断是否需要返回地址的645消息
* @param[in] ：uint8   id          通道ID
               uint8  *pMsg        消息内容
               uint16 *pLen        消息长度
* @param[out]：uint8  *pMsg        新消息内容
               uint16 *pLen        新消息长度
* @return    ：TRUE/FALSE
* @Create    : 
* @Date      ：2020-3-19
* @Update    :
**********************************************************************/
BOOLEAN app_bluetooth_mac(uint8 id, uint8 *pMsg, uint16 *pLen)
{
    if(LOCATE_232 != id || NULL == pMsg || NULL == pLen)
    {
        return FALSE;
    }

    uint8  newMsg[2048] = {0};
    uint16 newLen = 0;
    uint16 i = 0;

    while(i < *pLen)
    {
        if(pMsg[i] != 0xfe)
        {
            newMsg[newLen++] = pMsg[i];
        }
        i++;
    }

    memcpy(pMsg, newMsg, newLen);
    *pLen = newLen;

    /* 检查新消息是否查询地址 */
    if(newLen == 16 && 0x68 == newMsg[0] && 0x68 == newMsg[7] && 0x11 == newMsg[8] && 0x04 == newMsg[9] 
       && 0x34 == newMsg[10] && 0x37 == newMsg[11] && 0x33 == newMsg[12] && 0x37 == newMsg[13])
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**********************************************************************
* @name      : app_bluetooth_mac_reply
* @brief     ：组装回复蓝牙地址报文
* @param[in] ：uint8   id          通道ID
* @param[out]：
* @return    ：错误码
* @Create    : 
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
int app_bluetooth_mac_reply(uint8 id)
{
	uint8 sendbuff[] = {0xfe, 0xfe, 0xfe, 0xfe, 0x68, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
	0x68, 0x11, 0x04, 0x34, 0x37, 0x33, 0x37, 0xcf, 0x16};
    uint8 *pAddr = NULL;
    uint8 addrLen = 0;
    uint8 MacAddr[6] = {0};
    addrLen = app_get_localaddr(&pAddr);
    if(addrLen >= 6)
    {
        memcpy(MacAddr, pAddr + addrLen - 6, 6);
    }
    else if(addrLen > 0)
    {
        /* 浙江等地区 终端地址是4个字节 */
        memcpy(MacAddr + 6 - addrLen, pAddr, addrLen);
    }
    else
    {
        APP698_FMT_DEBUG("app_bluetooth_mac_reply addrLen err[%d]\n", addrLen);
        return ERR_ADDRESS;
    }

    sendbuff[5] = pAddr[5];
    sendbuff[6] = pAddr[4];
    sendbuff[7] = pAddr[3];
    sendbuff[8] = pAddr[2];
    sendbuff[9] = pAddr[1];
    sendbuff[10] = pAddr[0];
    sendbuff[18] = 0xFF&(0xBA + sendbuff[5] + sendbuff[6] + sendbuff[7] + sendbuff[8] + sendbuff[9] + sendbuff[10]);

    APP698_BUF_DEBUG(sendbuff, sizeof(sendbuff), "app_bluetooth_mac_reply send:");

    return send_emergency_ipcmsg(id, 0, sendbuff, sizeof(sendbuff));
}

/**********************************************************************
* @name      : app_set_sendblock
* @brief     ：设置通道链路分帧发送序号, 调用者加锁
* @param[in] ：CHANNEL_CACHE_T *ptChannel     通道句柄
               uint16 blocknum                收到的确认分帧序号
* @param[out]：
* @return    ：错误码
* @Create    : 
* @Date      ：2019-12-4
* @Update    :
**********************************************************************/
int app_set_sendblock(CHANNEL_CACHE_T *ptChannel, uint16 blocknum)
{
    int errcode = 0;
    
    if(0 == ptChannel->sendMessLen)
    {
        goto end;
    }
    
    if(blocknum != ptChannel->curblock % (LINK_SEPARATE_MAX_BLOCK + 1))
    {
        ptChannel->permSend = 1; //序号不对，执行重发
        errcode = APP698_ERR_SEND_ERRORMSG;
        goto end;
    }

    if(ptChannel->permSend)
    {
        errcode = APP698_ERR_BUSY;
        goto end;
    }
    
    ptChannel->curblock++;
    ptChannel->permSend = 1;

end:
    return errcode;
}

/**********************************************************************
* @name      : app_insert_procmesstail
* @brief     ：向处理消息缓存队列尾部追加数据
* @param[in] ：CHANNEL_CACHE_T *ptChannel     通道句柄
               uint8 *pBuf                    缓存
               uint16 bufLen                  缓存大小
               uint16 blockid                 收到的确认分帧序号
* @param[out]：
* @return    ：错误码
* @Create    : 
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
int app_insert_procmesstail(CHANNEL_CACHE_T *ptChannel, uint8 *pBuf, uint16 bufLen, uint16 blockid)
{
    int errCode = APP698_SECCESS;
    if(ptChannel->procLen + bufLen > APP_RECV_MSG_MAXLEN)
    {
        errCode = APP698_ERR_MSG_TOO_LONG;
        bufLen = APP_RECV_MSG_MAXLEN - ptChannel->procLen;
    }
    memcpy(ptChannel->ucMess + ptChannel->procLen, pBuf, bufLen);
    ptChannel->procLen += bufLen;
    ptChannel->blocknum = blockid;
    return errCode;
}

/**********************************************************************
* @name      : app_update_procmess_frame
* @brief     ：更新处理消息缓存队列和帧头数据
* @param[in] ：CHANNEL_CACHE_T *ptChannel     通道句柄
* @param[out]：FRAME_HEAD_T *pFrame          帧头信息   
* @return    ：
* @Create    : 
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
void app_update_procmess_frame(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *pFrame)
{
    ptChannel->ucMess[1] = (ptChannel->procLen-2) % 0xff;
    ptChannel->ucMess[2] = ((ptChannel->procLen-2) >> 8 ) % 0x3f;
    if(pFrame)
    {
        pFrame->msglen = ptChannel->procLen - 2;
        pFrame->pApdu = ptChannel->ucMess + 1 + pFrame->headlen;
        pFrame->apduLen = pFrame->msglen - pFrame->headlen - 2;
    }
}

/**********************************************************************
* @name      : app_linkrequest_package
* @brief     ：构造预连接报文
* @param[in] ：uint8 type          请求类型
               uint16 beatPeriod   心跳周期
               uint16 bufLen       返回消息缓存大小
* @param[out]：uint8 *pBuf         返回消息缓存
* @return    ：消息长度
* @Create    : 
* @Date      ：2019-12-6
* @Update    :
**********************************************************************/
uint16 app_linkrequest_package(uint8 type, uint16 beatPeriod, uint8 *pBuf, uint16 bufLen)
{
    FRAME_HEAD_T tFrame;
    LinkRequest_t apdu;
    struct tm ptm;
    time_t now_t;
    uint8 *pAddr = NULL;
    struct timeval tv;
    uint16 ms;

    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    memset(&apdu, 0, sizeof(LinkRequest_t));
    /* APDU内容是小端的 */
    apdu.id = SERVERID_LINKREQUEST;
    apdu.piid = 0;
    apdu.type = type;
    apdu.beatPeriod[0] = (beatPeriod&0xFF00)>>8;
    apdu.beatPeriod[1] = (uint8)beatPeriod;

    now_t = time(NULL);
    LOCALTIME_R(&now_t, &ptm);
    apdu.dateTime.year_h = ((ptm.tm_year+1900)&0xFF00)>>8;
    apdu.dateTime.year_l = (uint8)(ptm.tm_year+1900);
    apdu.dateTime.month = (ptm.tm_mon+1);
    apdu.dateTime.day_of_month = (ptm.tm_mday);
    apdu.dateTime.day_of_week = (ptm.tm_wday);
    apdu.dateTime.hour = (ptm.tm_hour);
    apdu.dateTime.min = (ptm.tm_min);
    apdu.dateTime.sec = (ptm.tm_sec);
    //apdu.dateTime.milliseconds_h = 0;
    //apdu.dateTime.milliseconds_l = 0;
    gettimeofday(&tv,NULL);
    ms = tv.tv_usec/1000 ;
    apdu.dateTime.milliseconds_h = (ms&0xff00)>>8;
    apdu.dateTime.milliseconds_l = ms&0xff;

    tFrame.funCode = 1;
    tFrame.sc = 0;
    tFrame.separate = 0;
    tFrame.prm = 0;
    tFrame.dir = 1;
    
    tFrame.sAddr.len = app_get_localaddr(&pAddr);
    if(0 == tFrame.sAddr.len)
    {
        return 0;
    }
    memcpy(tFrame.sAddr.addr, pAddr, tFrame.sAddr.len);
    tFrame.sAddr.type = 0;
    tFrame.cAddr = 0;
    
    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.apduLen = sizeof(LinkRequest_t);
    tFrame.pApdu = (uint8 *)&(apdu);

    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;
    return app_frame_to_package(&tFrame, pBuf, bufLen, FALSE);
}

/**********************************************************************
* @name      : app_connect_init
* @brief     ：应用连接初始化 建立最低权限的应用连接
* @param[in] ：uint8 id            通道ID
* @param[out]：uint8 *pBuf         返回消息缓存
* @return    ：消息长度
* @Create    : 
* @Date      ：2019-12-6
* @Update    :
**********************************************************************/
void app_connect_init(uint8 id)
{
	if(id >= MAX_CHANNEL_NUM || FALSE == g_tChannel[id].bOpen)
	{
		return;
    }

	g_tChannel[id].bConnect = TRUE;
    g_tChannel[id].tContext.prtlVersion = g_tContext.VER;
    g_tChannel[id].tContext.recvFrameLen = g_tContext.nRecvSize;
    g_tChannel[id].tContext.sendFrameLen = g_tContext.nSendSize;
    g_tChannel[id].tContext.recvApduLen = g_tContext.nMaxPDUSize;
    g_tChannel[id].tContext.recvWindowSize = 1;
    g_tChannel[id].tContext.securityMode = 0;
    g_tChannel[id].tContext.timeOut = 600;
    memcpy(g_tChannel[id].tContext.prtlConformance, g_tContext.PCB, 8);
    memcpy(g_tChannel[id].tContext.funcConformance, g_tContext.FCB, 16);
    g_tChannel[id].tContext.estalishTime = time(NULL);

    g_tChannel[id].tContext.ApplSplitLen = app_apdu_default_len();
}

/**********************************************************************
* @name      : app_set_apdu_default_len
* @brief     ：设置应用分帧默认大小
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-4-13
* @Update    :
**********************************************************************/
void app_set_apdu_default_len(uint16 len)
{
    g_ApduSplitDeafult = len;
}

/**********************************************************************
* @name      : app_apdu_default_len
* @brief     ：获取应用分帧默认大小
* @param[in] ：
* @param[out]：
* @return    ：大小
* @Create    : 
* @Date      ：2020-4-13
* @Update    :
**********************************************************************/
uint16 app_apdu_default_len()
{
    return g_ApduSplitDeafult;
}

/**********************************************************************
* @name      : app_comm_proxy_set
* @brief     ：设置通信代理业务启用标志
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-4-13
* @Update    :
**********************************************************************/
void app_comm_proxy_set(BOOLEAN isUse)
{
    g_useCommProxy = isUse;
}

/**********************************************************************
* @name      : app_comm_proxy_get
* @brief     ：获取通信代理业务启用标志
* @param[in] ：
* @param[out]：
* @return    ：大小
* @Create    : 
* @Date      ：2020-4-13
* @Update    :
**********************************************************************/
BOOLEAN app_comm_proxy_isuse()
{
    return g_useCommProxy;
}


/**********************************************************************
* @name      : app_secure_defend_set
* @brief     ：设置安全防护业务启用标志
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-4-13
* @Update    :
**********************************************************************/
void app_secure_defend_set(BOOLEAN isUse)
{
    g_useSecureDefend = isUse;
}

/**********************************************************************
* @name      : app_secure_defend_get
* @brief     ：获取安全防护业务启用标志
* @param[in] ：
* @param[out]：
* @return    ：大小
* @Create    : 
* @Date      ：2020-4-13
* @Update    :
**********************************************************************/
BOOLEAN app_secure_defend_isuse()
{
    return g_useSecureDefend;
}

/**********************************************************************
* @name      : app_check_master_connect
* @brief     ：查看是否连接上主站
* @param[in] ：
* @param[out]：
* @return    ：TRUE/FALSE
* @Create    : 
* @Date      ：2020-4-11
* @Update    :
**********************************************************************/
uint8 app_check_master_connect()
{
    uint8  maxnum = 0;
    uint8  i;
    uint8  id = 0;

    maxnum = app_get_opencha_num();
    for(i = 0; i < maxnum; i++)
    {
        id = app_get_opencha_id(i);
        
        if(FALSE == g_tChannel[id].bOpen)
        {
            continue;
        }
        if(MODE_MIX == g_tChannel[id].tChannel.workmode)
        {
            return TRUE;
        }
        else if(MODE_CLIENT == g_tChannel[id].tChannel.workmode && TRUE == g_tChannel[id].bConnect)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*******************************************************************************
* 函数名称:	UnPackFrameData
* 函数功能:	解帧
* 输入参数:	FrameLen            帧长度
*           FrameBuff           帧数据
* 输出参数:	AppData             应用层数据
* 返 回 值: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
int8 UnPackRouterFrameData(uint16 FrameLen, uint8 *FrameBuff, AppData_T *AppData)
{
	//CCODOWN_FMT_DEBUG(0, "UnPackFrameData()...start...FrameLen=%d \n", FrameLen);
	
    uint16  FrameDataLen=0;                     //帧数据总长度
    uint16  DataIndex = 0;                      //指针偏移
    int     i = 0;
	uint8   CheckSum = 0;  //校验和

	//CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 111 \n");



    //计算帧数据长度, 偏移到信息域R
    memcpy(&FrameDataLen, &FrameBuff[1], 2);
    DataIndex = 4;
   
	//CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 222 \n");

	//判断报文长度是否合法
	if(FrameDataLen > FrameLen)
	{
		//报文长度大于buff长度, 判断该帧报文不完整
		
		return CHECK_RESULT_FAILUER;
	}
	
	//CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 333 \n");

    //解析信息域, 偏移到地址域
    memcpy(AppData->RInfo, &FrameBuff[DataIndex], 6);
    DataIndex += 6;

	//CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 444 \n");

    //判断通讯模块标识，偏移到应用数据域
    if (AppData->RInfo[0]& 0x04)  //存在地址域
    {
        //偏移掉A1和A3，从节点上行无A2
        DataIndex += 12;
    }
	
	//CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 555 \n");

    AppData->AFN = FrameBuff[DataIndex];
    for (i = 0; i < 8; i++)
    {
        if (FrameBuff[DataIndex+1] & (0x01 << i))
        {
            AppData->Fn = FrameBuff[DataIndex+2] * 8 + (i + 1);
            break;
        }
    }
    DataIndex += 3;
	
	//CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 666 \n");

	if(FrameDataLen < (DataIndex + 2))
	{
		
		return CHECK_RESULT_FAILUER;
	}
    AppData->DataUnitLen = FrameDataLen - (DataIndex + 2);

	//CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 777 \n");
	
	if(AppData->DataUnitLen > sizeof(AppData->DataUnitBuff))
	{
		
		return CHECK_RESULT_FAILUER;
	}
    memcpy(AppData->DataUnitBuff, &FrameBuff[DataIndex], AppData->DataUnitLen);
	
	//CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 888 \n");

	//校验和检查
    for(i=3; i<FrameDataLen-2; i++)
	{
		CheckSum += FrameBuff[i];
	}
    
	if(CheckSum != FrameBuff[FrameDataLen-2])
	{
		
		return CHECK_RESULT_FAILUER;
	}
	
	//CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... 999 \n");

	//帧尾检查
	if(FrameBuff[FrameDataLen-1] != 0x16)
	{
		
		return CHECK_RESULT_FAILUER;
	}
	
	//CCODOWN_FMT_DEBUG(0, "UnPackFrameData()... aaa \n");

    if (AppData->AFN == 0x00)
    {
        if (AppData->Fn == 1)                   //确认
        {
            return CHECK_RESULT_CONFIRM;
        }
        else if (AppData->Fn == 2)              //否认
        {
            return CHECK_RESULT_DENY;
        }
        else
        {
            return CHECK_RESULT_FAILUER;
        }
    }
    else
    {
        //缓存信道标识
       

        return CHECK_RESULT_SUCCEED;
    }


}

/*******************************************************************************
* 函数名称: ProxyWriteQueue
* 函数功能: 写队列
* 输入参数：plockqueue          锁缓冲区结构指针
            buf                 需要写入队列的数据
            buf_len             数据长度
* 输出参数：无
* 内部处理: 若队列不满，将buf中的数据放入队列中；若队列满，丢弃该数据
* 返 回 值: >0                  成功
            =0                  失败
*******************************************************************************/
int32 ProxyWriteQueue(ProxyLockQueue *ProxyQueue, uint8 *buf, int32 buf_len)
{
    int32 flags = 0;
	//printx("*PlcWriteQueue() pLockQueue:%p, &pLockQueue->QueueLock:%p, buf_len:%d \n", pLockQueue, &pLockQueue->QueueLock, buf_len);
	//PrintBuf(buf, buf_len);

    //锁缓冲区
    pthread_mutex_lock(&ProxyQueue->QueueLock);
	PRTL_FMT_LOGCC("ProxyQueue.QueueTail is %d\n",ProxyQueue->QueueTail);
	PRTL_FMT_LOGCC("buf_len is %d\n",buf_len);
	PRTL_FMT_LOGCC("ProxyQueue.Queue[ProxyQueue.QueueTail].Queue_Len is %d\n",ProxyQueue->Queue[ProxyQueue->QueueTail].Queue_Len);
    //判队列中是否还有空的位置
    if(ProxyQueue->Queue[ProxyQueue->QueueTail].Queue_Len == 0)
    {
        ProxyQueue->Queue[ProxyQueue->QueueTail].Queue_Len = buf_len;
        memcpy((uint8*)ProxyQueue->Queue[ProxyQueue->QueueTail].Queue_Data, buf, buf_len);
        flags = 1;
        ProxyQueue->QueueTail = (ProxyQueue->QueueTail + 1) % (10);
    }
	PRTL_FMT_LOGCC("ProxyQueue.QueueTail11 is %d\n",ProxyQueue->QueueTail);
    //解缓冲区
    pthread_mutex_unlock(&ProxyQueue->QueueLock);

    return flags;
}

/*******************************************************************************
* 函数名称: ProxyReadQueue
* 函数功能: 读队列
* 输入参数：pLockQueue          锁缓冲区结构指针
*           buf                 读数据缓冲区，将队列中的数据取出放入该缓冲区
* 输出参数：plockqueue          队列中的数据
* 内部处理: 判队列中是否有数据，有则从队列中取出数据放入buf中，每执行一次，取一个数据
* 返 回 值: len                 读到的数据长度
*******************************************************************************/
int32 ProxyReadQueue(ProxyLockQueue *ProxyQueue, uint8 *buf)
{
    int32 len = 0;
    uint8 *pdata_buf;

    //锁缓冲区
    pthread_mutex_lock(&ProxyQueue->QueueLock);  

    //若队列中有数据，则取到buf中，解开锁
    if ((ProxyQueue->QueueHead != ProxyQueue->QueueTail) || 
        (ProxyQueue->QueueHead == ProxyQueue->QueueTail && ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len != 0))
    {   
        pdata_buf = ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Data;  
        len = ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len;
        
        //取队列中的数据
        memcpy(buf, pdata_buf, len); 

        //清数据
        memset(pdata_buf, 0, len);   
        ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len = 0;

        //队列头向后移动一位
        ProxyQueue->QueueHead = (ProxyQueue->QueueHead + 1) % (10); 
		PRTL_FMT_LOGCC("ProxyQueue->QueueHead is %d\n",ProxyQueue->QueueHead);
		PRTL_FMT_LOGCC("ProxyQueue->QueueTail is %d\n",ProxyQueue->QueueTail);
		PRTL_FMT_LOGCC("ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len is %d\n",ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len);
		PRTL_FMT_LOGCC("len is %d\n",len);
    }
        
    pthread_mutex_unlock(&ProxyQueue->QueueLock);
    
    return len;
}

/*******************************************************************************
* 函数名称: ProxyCheckQueue
* 函数功能: 检查队列
* 输入参数：pLockQueue          锁缓冲区结构指针
* 输出参数：
* 内部处理: 判队列中是否有数据
* 返 回 值: TRUE/FALSE
*******************************************************************************/
BOOLEAN ProxyCheckQueue(ProxyLockQueue *ProxyQueue)
{
    uint8 bExist = FALSE;

    pthread_mutex_lock(&ProxyQueue->QueueLock);  
    if ((ProxyQueue->QueueHead != ProxyQueue->QueueTail) || 
        (ProxyQueue->QueueHead == ProxyQueue->QueueTail && ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len != 0))
    {   
        bExist = TRUE;
    }
    pthread_mutex_unlock(&ProxyQueue->QueueLock);
    return bExist;
}


/*******************************************************************************
* 函数名称: ProxyReadQueue
* 函数功能: 读队列
* 输入参数：pLockQueue          锁缓冲区结构指针
*           buf                 读数据缓冲区，将队列中的数据取出放入该缓冲区
* 输出参数：plockqueue          队列中的数据
* 内部处理: 判队列中是否有数据，有则从队列中取出数据放入buf中，每执行一次，取一个数据
* 返 回 值: len                 读到的数据长度
*******************************************************************************/
int32 TransReadQueue(ProxyLockQueue *ProxyQueue, uint8 *buf)
{
    int32 len = 0;
    uint8 *pdata_buf;

    //锁缓冲区
    pthread_mutex_lock(&ProxyQueue->QueueLock);  

    //若队列中有数据，则取到buf中，解开锁
    if ((ProxyQueue->QueueHead != ProxyQueue->QueueTail)
        || ((ProxyQueue->QueueHead == ProxyQueue->QueueTail)
            && (ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len != 0)))
    {   
        pdata_buf = ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Data;  
        len = ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len;
        
        //取队列中的数据
        memcpy(buf, pdata_buf, len); 

        //清数据
        memset(pdata_buf, 0, len);   
        ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len = 0;

        //队列头向后移动一位
        ProxyQueue->QueueHead= (ProxyQueue->QueueHead + 1) % (10); 
		PRTL_FMT_LOGCC("ProxyQueue.QueueTail is %d\n",ProxyQueue->QueueHead);
		PRTL_FMT_LOGCC("ProxyQueue.QueueTail is %d\n",ProxyQueue->QueueTail);
		PRTL_FMT_LOGCC("ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len is %d\n",ProxyQueue->Queue[ProxyQueue->QueueHead].Queue_Len);
		PRTL_FMT_LOGCC("len is %d\n",len);
    }
        
    pthread_mutex_unlock(&ProxyQueue->QueueLock);
    
    return len;
}

/****************************************************************************
*模块编号：
*名称：
*功能：
*输入参数：
*输出参数：
*处理：
*返回：0:成功，1:无时间标签，-2:帧不完整，-3格式错误，-4 时间失效
****************************************************************************/
int Chk_TimeTag(const uint8* inData,uint16 IndataLen)
{
    uint16 offSet = 0;
    if(inData[offSet]==0x00)
    {
        return 1;
    }

    if(inData[offSet] != 1)
    {
        return -3;
    }

    offSet++;
    if(IndataLen-offSet <10)
    {
        return -2;
    }
    DateTimeHex_t Tag_Time;
    OOP_TI_T Tag_TI;
    memcpy((uint8*)&Tag_Time,inData+offSet,7);
    //memrev((uint8*)&Tag_Time,7);
    offSet +=7;
    Tag_TI.unit = inData[offSet++];
    Tag_TI.value = inData[offSet++];
    Tag_TI.value = (Tag_TI.value<<8)+inData[offSet++];
    if(Tag_TI.value == 0)
    {
        return 0;
    }
    time_t LinuxTagTimeStart, LinuxTagTimeEnd;
    if(FALSE == DT_DateTimeHex2Time(&Tag_Time,&LinuxTagTimeEnd))
        return -3;
    LinuxTagTimeStart = LinuxTagTimeEnd;
    DT_TimeAddInv(&LinuxTagTimeEnd,Tag_TI);
    DT_TimeSubInv(&LinuxTagTimeStart,Tag_TI);
    if((time(NULL) < LinuxTagTimeStart) || (time(NULL) > LinuxTagTimeEnd))
        return -4;
    return 0;
}

/****************************************************************************
*模块编号：
*名称：
*功能：
*输入参数：
*输出参数：
*处理：
*返回：APDU检查结果  -1:数据帧不完整,-2:服务不支持 ,>0解析的帧长度
****************************************************************************/
int SerialChkApdu(uint8 SevID,uint8 nChoice,const uint8 *InBuff,uint16 InLen)
{
    uint8 i,j;
    uint32 offSet = 0;
    switch(SevID)
    {
    case SERVERID_LINKRESPONSE:
    {
        apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE);
        apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE);
//        apdu_next_position_get(InBuff,&offSet,InLen,DATETIMEBCD_H,TRUE);
//        apdu_next_position_get(InBuff,&offSet,InLen,DATETIMEBCD_H,TRUE);
//        if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DATETIMEBCD_H,TRUE))
//            return -1;
    }
    break;
    case SERVERID_CONNECTREQUEST:
    {
        uint8 SecType;
        apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE);
        apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE);
        offSet += 8;
        offSet += 16;
        apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE);
        apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE);
        apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE);
        apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE);
        if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_DOUBLE_LONG_UNSIGNED,TRUE))
            return -1;
        SecType = InBuff[offSet++];
        switch(SecType)
        {
        case 0:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_NULL,TRUE))
                return -1;
            break;
        case 1:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_VISIBLE_STRING,TRUE))
                return -1;
            break;
        case 2:
        case 3:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OCTET_STRING,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OCTET_STRING,TRUE))
                return -1;
            break;
        default:
            return -2;
        }
    }
    break;
    case SERVERID_RELEASEREQUEST:
    {
        if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
            return -1;
    }
    break;
    case SERVERID_GETREQUEST:
    {
        //uint8 nChoice;
        uint8 seqNum;
        //nChoice = m_BufferRead[m_OffsetRead++];
        switch(nChoice)
        {
        case 1:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                return -1;
            break;
        case 2:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            seqNum = InBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                    return -1;
            }
            break;
        case 3:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_RSD,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_RCSD,TRUE))
                return -1;
            break;
        case 4:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            seqNum = InBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_RSD,TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_RCSD,TRUE))
                    return -1;
            }
            break;
        case 5:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE))
                return -1;
            break;
        #if GET_OPTIMIZATION_EN == 1
        case 13:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_RSD,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_RCSD,TRUE))
                return -1;
            break;
        #endif
        #if GET_SIMPLIFYRECORD_EN == 1
        case 23:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_RSD,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_ROAD,TRUE))
                return -1;
            break;
        #endif
        default:
            return -2;
        }
    }
    break;
    case SERVERID_SETREQUEST:
    {
        //uint8 nChoice;
        uint8 seqNum;
        //nChoice = m_BufferRead[m_OffsetRead++];

        switch(nChoice)
        {
        case 1:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_NULL,FALSE))
                return -1;
            break;
        case 2:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            seqNum = InBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_NULL,FALSE))
                    return -1;
            }
            break;
        case 3:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            seqNum = InBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_NULL,FALSE))
                    return -1;
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                    return -1;
            }
            break;
        default:
            return -2;
        }
    }
    break;
    case SERVERID_ACTIONREQUEST:
    {
        //uint8 nChoice;
        uint8 seqNum;
        //nChoice = m_BufferRead[m_OffsetRead++];
        switch(nChoice)
        {
        case 1:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OMD,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_NULL,FALSE))
                return -1;
            break;
        case 2:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            seqNum = InBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OMD,TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_NULL,FALSE))
                    return -1;
            }
            break;
        case 3:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            seqNum = InBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OMD,TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_NULL,FALSE))
                    return -1;
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                    return -1;
            }
            break;
        default:
            return -2;
        }
    }
    break;
    case SERVERID_REPORTRESPONSE:
    {
        //uint8 nChoice;
        uint8 seqNum;
        //nChoice = m_BufferRead[m_OffsetRead++];
        switch(nChoice)
        {
        case 1:
        case 2:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            seqNum = InBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OMD,TRUE))
                    return -1;
            }
            break;
        default:
            return -2;
        }
    }
    break;
    case SERVERID_PROXYREQUEST:
    {
        //uint8 nChoice;
        uint8 seqNum;
        //nChoice = m_BufferRead[m_OffsetRead++];
        switch(nChoice)
        {
        case 1:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE))
                return -1;
            seqNum = InBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_TSA,TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE))
                    return -1;
                uint8 oadNum = InBuff[offSet++];
                for(j=0; j<oadNum; j++)
                {
                    if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                        return -1;
                }
            }
            break;
        case 2:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_TSA,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_RSD,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_RCSD,TRUE))
                return -1;
            break;
        case 3:
        case 5:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE))
                return -1;
            seqNum = InBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_TSA,TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE))
                    return -1;
                uint8 oadNum = InBuff[offSet++];
                for(j=0; j<oadNum; j++)
                {
                    if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                        return -1;
                    if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_NULL,FALSE))
                        return -1;
                }
            }
            break;
        case 4:
        case 6:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE))
                return -1;
            seqNum = InBuff[offSet++];
            for(i=0; i<seqNum; i++)
            {
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_TSA,TRUE))
                    return -1;
                if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE))
                    return -1;
                uint8 oadNum = InBuff[offSet++];
                for(j=0; j<oadNum; j++)
                {
                    if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                        return -1;
                    if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_NULL,FALSE))
                        return -1;
                    if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                        return -1;
                    if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                        return -1;
                }
            }
            break;
        case 7:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OAD,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_COMDCB,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_LONG_UNSIGNED,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OCTET_STRING,TRUE))
                return -1;
            break;
        default:
            return -2;
        }
    }
    break;
    case SERVERID_SECURITYREQUEST:
    {
        uint8 AuthType;
        if(InBuff[offSet++] > 0x01)//应用数据单元CHIOCE 0:明文，1密文
            return -1;
        if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_OCTET_STRING,TRUE))
            return -1;
        AuthType = InBuff[offSet++];
        if(AuthType > 0x03)
            return -1;
        switch(AuthType)
        {
        case 0:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_SIDMAC,TRUE))
                return -1;
            break;
        case 1:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_RN,TRUE))
                return -1;
            break;
        case 2:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_RN,TRUE))
                return -1;
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_MAC,TRUE))
                return -1;
            break;
        case 3:
            if(FALSE == apdu_next_position_get(InBuff,&offSet,InLen,DT_SID,TRUE))
                return -1;
            break;
        }
    }
    break;
    default:
        return -2;
        break;
    }
    return offSet;
}

//透明转发一个字节的通讯字转化为OOP_COMDCB_T
uint8 SetComDCB(OOP_COMDCB_T comm)
{
    //uint8 commbyte=0;
	uint8	baud;			/**< 波特率				*/
	uint8	parity;			/**< 校验位				*/
	uint8	databits;		/**< 数据位				*/
	uint8	stopbits;		/**< 停止位				*/
	//uint8	flowctrl;		/**< 流控				*/

	switch(comm.baud)
	{
		case 0:
			baud=0x00;
		break;
		case 1:
			baud=0x20;
		break;
		case 2:
			baud=0x40;
		break;
		case 3:
			baud=0x60;
		break;
		case 4:
			baud=0x80;
		break;
		case 5:
			baud=0xA0;
		break;
		case 6:
			baud=0xC0;
		break;
		case 7:
			baud=0xE0;
		break;
			default:
				break;

	}
	switch(comm.parity)
	{
		case 0:
			parity=0x00;
		break;
		case 1:
			parity=0x06;
		break;
		case 2:
			parity=0x04;
		break;

		break;
			default:
				break;

	}
	switch(comm.databits)
	{
		case 5:
			databits=0x00;
		break;
		case 6:
			databits=0x01;
		break;
		case 7:
			databits=0x02;
		break;
		case 8:
			databits=0x03;
		break;

		break;
			default:
				break;

	}
	switch(comm.stopbits)
	{
		case 1:
			stopbits=0x00;
		break;
		case 2:
			stopbits=0x10;
		break;
			default:
				break;

	}

    switch(comm.flowctrl)
	{
	    case 0:

		case 1:

		case 2:
	
		break;
			default:
				return 0xFF;

	}
    return baud|parity|databits|stopbits;
}

/**********************************************************
* 函数名称：生成一个msg
* 功能描述：
* 输入描述：
*
* 返回值：
**********************************************************/
BOOL GenProxyMSG(Proxyresquest *pProxyTask,uint32 *msgid,uint8 *outdata,uint16 *outlen)
{

	uint8 lenBuf[3] = {0}; //数组数量编码
	uint8 offlen = 0;      //数组数量长度
	
    if(0 == pProxyTask->RequestApduLen)
        return FALSE;

   switch(pProxyTask->nChioce)
   {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
    case 7:
		{
			*msgid=MSG_APPAMIN_CTS_READ_ONE;
			offlen = set_len_offset(pProxyTask->RequestApduLen, lenBuf);
			memcpy(outdata, lenBuf, offlen);
		
			*outlen=pProxyTask->RequestApduLen+offlen;
			memcpy(outdata+offlen,pProxyTask->pRequestApdu,*outlen);
			return TRUE;
		}
	break;
	case 8:
	{
		uint8 offset=0;
        uint8 bytes=0;
//		OOP_COMDCB_T comm;
        uint16 datalen=0;
		
		*msgid=MSG_APPAMIN_CTS_READ_LIST;
		memcpy(outdata+offset,pProxyTask->pRequestApdu+3,4);
		offset=offset+4;
//		comm.baud=pProxyTask->pRequestApdu[7];
//		comm.parity=pProxyTask->pRequestApdu[8];
//		comm.databits=pProxyTask->pRequestApdu[9];
//		comm.stopbits=pProxyTask->pRequestApdu[10];
//		comm.flowctrl=pProxyTask->pRequestApdu[11];
		outdata[offset++]= pProxyTask->pRequestApdu[7];
		outdata[offset++]= 3+(pProxyTask->pRequestApdu[8]<<2);

//        if(0xFF==SetComDCB(comm))
//        {
//            return FALSE;
//        }
//        else
//        {
//		    outdata[offset++]=SetComDCB(comm);
//        }
		memcpy(outdata+offset,pProxyTask->pRequestApdu+12,4);
        offset=offset+4;
        datalen = get_len_offset(pProxyTask->pRequestApdu+16, &bytes);
        memcpy(outdata+offset,pProxyTask->pRequestApdu+16,datalen+bytes);
		offset=offset+datalen+bytes;
		*outlen=offset;
		PRTL_BUF_LOGCC(outdata,offset,"透传消息");
		return TRUE;
	}
	break;
	default:
	return FALSE;
   	}
}

/**********************************************************
* 函数名称：代理服务
* 功能描述：
* 输入描述：
*
* 返回值：
**********************************************************/
void ProxyRequest_ClientApdu(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, uint8 *pBreakFlag)
{

    Proxyresquest pProxyTaskLink;
    uint16		Offset=0;
    int 		rslt=1;
    int        ret = -1;
	int        result=0;

	PRTL_FMT_LOGCC("ProxyRequest_ClientApdu is BEGIN\n");
	PRTL_BUF_LOGCC(ptFrame->pApdu,ptFrame->apduLen,"apdu");
	memset((uint8*)&pProxyTaskLink,0x00,sizeof(Proxyresquest));
    if(ptFrame->pApdu[1]>7||ptFrame->pApdu[1]==0)
        return;
	PRTL_FMT_LOGCC("ProxyRequest_ClientApdu is BEGIN11\n");
    rslt = SerialChkApdu(SERVERID_PROXYREQUEST,ptFrame->pApdu[1],ptFrame->pApdu+2,ptFrame->apduLen-2);
    if (rslt < 0)
    {
        //APDU错误
        PRTL_FMT_DEBUG("apdu err rslt is %d\n", rslt);
        return;
    }
    Offset = rslt;
    Offset += 2;
	PRTL_FMT_LOGCC("ptFrame->apduLen is %d,Offset is %d\n",ptFrame->apduLen,Offset);
    //判断时间标签是否正确
    ret = Chk_TimeTag(&ptFrame->pApdu[Offset],ptFrame->apduLen-Offset);
    if(ret == 1)//无时间标签
    {
        ptChannel->tContext.timeTag.tagFlag = 0;
    }
    else if (ret == 0)//有时间标签且没超时
    {
        ptChannel->tContext.timeTag.isTimeOut = FALSE;
        ptChannel->tContext.timeTag.tagFlag = ptFrame->pApdu[Offset];
        memcpy(ptChannel->tContext.timeTag.tagBuff, &ptFrame->pApdu[Offset+1], 10);
    }
    else if (ret == -4)//有时间标签且已经超时
    {
        ptChannel->tContext.timeTag.isTimeOut = TRUE;
        ptChannel->tContext.timeTag.tagFlag = ptFrame->pApdu[Offset];
        memcpy(ptChannel->tContext.timeTag.tagBuff, &ptFrame->pApdu[Offset+1], 10);
    }
    else
    {
        //	return pProxyTaskLinkHead;
    }

    pProxyTaskLink.PIID = ptFrame->pApdu[2];
    pProxyTaskLink.ServerID = SERVERID_PROXYREQUEST;
    pProxyTaskLink.nChioce = ptFrame->pApdu[1];
    if(ptChannel->tContext.securityServer==TRUE)
        pProxyTaskLink.SecMod = ptChannel->tContext.curSecurityMode;
    else
        pProxyTaskLink.SecMod = SecMod_NULL;

	PRTL_FMT_LOGCC("ptFrame->sAddr.len is %d\n",ptFrame->sAddr.len);
	
	pProxyTaskLink.RequestApduLen=ptFrame->apduLen;
    memcpy(pProxyTaskLink.pRequestApdu,ptFrame->pApdu,pProxyTaskLink.RequestApduLen);
    pProxyTaskLink.FrameHeadInfo.pipe = ptChannel->id;
	pProxyTaskLink.FrameHeadInfo.ctrl.ctrl = ptFrame->dir|ptFrame->prm|ptFrame->separate|ptFrame->sc|ptFrame->funCode;
	memcpy((uint8*)&pProxyTaskLink.FrameHeadInfo.oad.value,ptFrame->pApdu+3,4);
	PRTL_FMT_LOGCC("pProxyTaskLink.FrameHeadInfo.ctrl.ctrl is %d\n",pProxyTaskLink.FrameHeadInfo.ctrl.ctrl);
    pProxyTaskLink.FrameHeadInfo.cAddr = ptFrame->cAddr;
	pProxyTaskLink.FrameHeadInfo.tsa.af=ptFrame->sAddr.len;
	memcpy(pProxyTaskLink.FrameHeadInfo.tsa.add,ptFrame->sAddr.addr,pProxyTaskLink.FrameHeadInfo.tsa.af);
	PRTL_FMT_LOGCC("pProxyTaskLink.FrameHeadInfo.tsa.af is %d\n",pProxyTaskLink.FrameHeadInfo.tsa.af);
    result=ProxyWriteQueue(&ProxyQueue,(uint8*)&pProxyTaskLink,sizeof(Proxyresquest));
	PRTL_FMT_LOGCC("result is %d\n",result);
	PRTL_BUF_LOGCC((uint8*)&pProxyTaskLink,sizeof(Proxyresquest),"Proxyresquest");
    return ;
}

void gui_mannual_amr_send(uint8 *apdu, uint16 len)
{
    PRTL_BUF_DEBUG(apdu, len, "发送的报文");
    uint8 *pAddr = NULL;
    Proxyresquest proxyTaskLink;
    memset((uint8*)&proxyTaskLink,0x00,sizeof(Proxyresquest));

    //获取终端地址
    app_get_localaddr(&pAddr);

    PRTL_BUF_LOGCC(pAddr, 10, "address");

    proxyTaskLink.PIID = 0x08;
    proxyTaskLink.ServerID = SERVERID_PROXYREQUEST;
    proxyTaskLink.nChioce = 0x07;
    proxyTaskLink.SecMod = SecMod_NULL;
    proxyTaskLink.FrameHeadInfo.pipe = CHL_MANUAL_AMR;
    proxyTaskLink.FrameHeadInfo.ctrl.ctrl = 0x03;
    proxyTaskLink.FrameHeadInfo.tsa.af = 0x06;
    memcpy((uint8*)proxyTaskLink.FrameHeadInfo.tsa.add, pAddr, 6);
    memcpy((uint8*)&proxyTaskLink.FrameHeadInfo.oad.value, &apdu[3], 4);
    proxyTaskLink.RequestApduLen = len;
#if 0
    uint8 temp[64] = {0x09, 0x07, 0x08, 0xF2, 0x01, 0x02, 0x01, 0x03, 0x02, 0x08, 0x01, 0x00, 0x00, 0x32, 0x00, 0x0A,
                      0x19, 0x68, 0x17, 0x00, 0x43, 0x05, 0x74, 0x17, 0x28, 0x00, 0x00, 0x60, 0x00, 0xCD, 0x06, 0x05,
                      0x01, 0x05, 0x00, 0x10, 0x02, 0x00, 0x00, 0x52, 0x09, 0x16, 0x00};
#endif
    memcpy(proxyTaskLink.pRequestApdu, apdu, proxyTaskLink.RequestApduLen);
    ProxyWriteQueue(&ProxyQueue,(uint8*)&proxyTaskLink,sizeof(Proxyresquest));
    PRTL_BUF_LOGCC((uint8*)&proxyTaskLink,sizeof(Proxyresquest),"manual_amr_keymsg");
}
void gui_line_net_rcv(uint8 *rptmsg,uint8 lenoffset)
{
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
    if(rptmsg[33+lenoffset]==0x10&&rptmsg[34+lenoffset]==0x02)//节点数量
    {
//        uint8 tmpnum[4];
//        tmpnum[0]=rptmsg[37]&0xf0;
//        tmpnum[1]=rptmsg[37]&0x0f;
//        tmpnum[2]=rptmsg[36]&0xf0;
//        tmpnum[3]=rptmsg[36]&0x0f;
//        PRTL_BUF_DEBUG(tmpnum,4,"节点数量");
//        g_line_net_rst.netnum=tmpnum[0]*4096+tmpnum[1]*256+tmpnum[2]*16+tmpnum[3];
        memcpy(&g_line_net_rst.netnum,&rptmsg[35+lenoffset],2);
    }
    else                                   //路由学习状态rptmsg[34]==0x08&&rptmsg[35]==0x00
    {
        g_line_net_rst.routestate=(rptmsg[35+lenoffset]&0x01); 
        g_line_net_rst.protype=2;
    }
    return ;
//#endif
}

void gui_mannual_amr_rcv(uint8 *rptmsg)
{
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
    if (rptmsg[16] == g_manual_amr_rst.piid && rptmsg[21] == 0) //错误
    {
        sprintf(g_manual_amr_rst.state, "错误码%d", rptmsg[22]);
    }
    else if (rptmsg[16] == g_manual_amr_rst.piid)
    {
        if (rptmsg[22] != 0)    //长度
        {
            uint32 value;
            sprintf(g_manual_amr_rst.state, "成功");
            
            if (g_manual_amr_rst.protocol == 3)  //698协议
            {
                //value = rptmsg[53] * 256 * 256 * 256 + rptmsg[54] * 256 * 256 + rptmsg[55] * 256 + rptmsg[56];
                memcpy_r(&value,&rptmsg[48],4);
            }
            else
            {
                uint8 manualArmValue[4];
                manualArmValue[0] = rptmsg[37] - 0x33;
                manualArmValue[1] = rptmsg[38] - 0x33;
                manualArmValue[2] = rptmsg[39] - 0x33;
                manualArmValue[3] = rptmsg[40] - 0x33;
                value = BCD_change_to_long(manualArmValue, 8);
                PRTL_FMT_DEBUG("manualArmValue = %d %d %d %d, value = %d\n", manualArmValue[0], manualArmValue[1],
                    manualArmValue[2], manualArmValue[3], value);
            }
            sprintf(g_manual_amr_rst.data, "%.2f kWh", value / 100.0);
        }
        else
        {
            sprintf(g_manual_amr_rst.state, "未抄通");
            sprintf(g_manual_amr_rst.data, "--");
        }
    }

    if (rptmsg[16] == g_manual_amr_rst.piid)
    {
        time_t tm;
        tm = time(NULL);
        strftime(g_manual_amr_rst.time, sizeof(g_manual_amr_rst.time), "%H:%M:%S", localtime((time_t*)&tm));
    }
//#endif
}

/**
*********************************************************************
* @brief:        判断是否是三星的taskManager，在测试三星taskManager时，代理设置频点总是失败，先做兼容
* @param[in]：   NA
* @param[out]：  NA 
* @return：      TRUE - 是 FALSE - 否
*********************************************************************
*/
BOOL isSxTaskManager()
{
    #if 1
    if ((access("/data/app/taskManager/log/sx.log", F_OK) == 0) && 
        (access("/data/app/taskManager/log/important.log", F_OK) == 0))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
    #endif

    return FALSE;
}

/**
*********************************************************************
* @brief:       透传上报报文
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_proxyresponse_data()
{
    int ret = 0;
	ProxyResponse ProxyResponsedata;
    uint8 id;
	FRAME_HEAD_T tFrame;
	uint16 msgLen=0;
	uint8 rptmsg[APP_SEND_MSG_MAXLEN];
	uint8 tembuf[2200];
    uint8 Bytes=0;
    uint16 lenoffset=0;
 
    ret = ProxyReadQueue(&ProxyrespondeQueue, (uint8*)&ProxyResponsedata);  
    if (ret <= 0)
    {
        return;
    }
    //组织上报报文
	tFrame.funCode=ProxyResponsedata.FrameHeadInfo.ctrl.ctrl_fun;
    tFrame.sc= ProxyResponsedata.FrameHeadInfo.ctrl.ctrl_discode;   
	tFrame.separate= ProxyResponsedata.FrameHeadInfo.ctrl.ctrl_seg; 
	//tFrame.prm=ProxyResponsedata.FrameHeadInfo.ctrl.ctrl_prm;
	tFrame.prm=1;
	tFrame.dir=1;
	if(ProxyResponsedata.ServerID==0x01)
	{
		tFrame.apduLen=ProxyResponsedata.ResponseApduLen+10;
	}
	else
	{
	    lenoffset=get_len_offset(ProxyResponsedata.ResponseApdu,&Bytes);
		tFrame.apduLen=lenoffset;
	}
	
	tFrame.pApdu= tembuf;
    tFrame.sAddr.logicAddr=ProxyResponsedata.FrameHeadInfo.tsa.vxd;
    tFrame.sAddr.type =ProxyResponsedata.FrameHeadInfo.tsa.type;
	tFrame.sAddr.len= ProxyResponsedata.FrameHeadInfo.tsa.len;
	memcpy(tFrame.sAddr.addr,ProxyResponsedata.FrameHeadInfo.tsa.add,ProxyResponsedata.FrameHeadInfo.tsa.len);
    PRTL_FMT_LOGCC("tFrame.sAddr.logicAddr is %d,tFrame.sAddr.type is %d,tFrame.sAddr.len is %d\n",tFrame.sAddr.logicAddr,tFrame.sAddr.type,tFrame.sAddr.len);
    PRTL_BUF_LOGCC(tFrame.sAddr.addr,tFrame.sAddr.len,"回复路由报文");
	tFrame.cAddr= ProxyResponsedata.FrameHeadInfo.cAddr;
	tFrame.separate= 0x00;//先默认不分帧
//	tembuf[0]= 0x89;
//	tembuf[1]= ProxyResponsedata.ServerID;
//	if(ProxyResponsedata.ServerID==0x08)
//	{
//		tembuf[0]=0x87;
//	}
	if(ProxyResponsedata.ServerID==0x01)
	{
	    if((ProxyResponsedata.FrameHeadInfo.oad.value !=0xF2097F00)&&(ProxyResponsedata.FrameHeadInfo.oad.value !=0xF2090900))
        {   
    		tembuf[0]= 0x89;
    		tembuf[1]= 0x07;
    		tembuf[2]= ProxyResponsedata.PIID;
    		memcpy_r(tembuf+3,(uint8*)&ProxyResponsedata.FrameHeadInfo.oad.value,4);
    		if(ProxyResponsedata.ResponseApduLen>6)
    		{
    			tembuf[7]= 0x01;
    		}
            else
            {
                tembuf[7]= 0x00;
            }
    		memcpy(tembuf+8,ProxyResponsedata.ResponseApdu,ProxyResponsedata.ResponseApduLen);
    		PRTL_BUF_LOGCC(ProxyResponsedata.ResponseApdu,ProxyResponsedata.ResponseApduLen,"抄表报文");
    		tembuf[8+ProxyResponsedata.ResponseApduLen]=0;
    		tembuf[9+ProxyResponsedata.ResponseApduLen]=0;
        }
        else if(ProxyResponsedata.FrameHeadInfo.oad.value ==0xF2097F00)
        {
            tembuf[0]= 0x87;
    		tembuf[1]= 0x01;
            tembuf[2]= ProxyResponsedata.PIID;
  
            memcpy_r(tembuf+3,(uint8*)&ProxyResponsedata.FrameHeadInfo.oad.value,4);
            if(ProxyResponsedata.ResponseApduLen>6)
    		{
    			tembuf[7]= 0x00;
                tembuf[8]= 0x01;
                tembuf[9]= 0x09;
                memcpy(tembuf+10,ProxyResponsedata.ResponseApdu,ProxyResponsedata.ResponseApduLen);
    		    PRTL_BUF_LOGCC(ProxyResponsedata.ResponseApdu,ProxyResponsedata.ResponseApduLen,"载波透传报文");
                tembuf[10+ProxyResponsedata.ResponseApduLen]=0;
    		    tembuf[11+ProxyResponsedata.ResponseApduLen]=0;
                tFrame.apduLen=ProxyResponsedata.ResponseApduLen+12;
    		
    		}
            else
            {
                PRTL_FMT_DEBUG("载波透传报文无数据返回\n");
                tembuf[7]= 0x00;
                tembuf[8]= 0x01;
                tembuf[9]= 0x09;
    		    tembuf[10]= 0x00;
                tembuf[11]= 0x00;
    		    tembuf[12]= 0x00;
                tFrame.apduLen= 13;
            }
            
        }
        else if(ProxyResponsedata.FrameHeadInfo.oad.value ==0xF2090900)
        {
            uint16 framelen=0;
            uint8 result =0;
            AppData_T AppData;

            memset((uint8*)&AppData,0x00,sizeof(AppData_T));
            framelen = ProxyResponsedata.ResponseApdu[0];
            PRTL_BUF_LOGCC(ProxyResponsedata.ResponseApdu+1,framelen,"回复路由报文");
            result=UnPackRouterFrameData(framelen, ProxyResponsedata.ResponseApdu+1, &AppData);
            PRTL_FMT_LOGCC("载波透传报文无数据返回,ProxyResponsedata.UseID is %d\n",ProxyResponsedata.UseID);
            if(ProxyResponsedata.UseID ==SERVERID_SETREQUEST)
            {

                tembuf[0]= 0x86;
        		tembuf[1]= 0x01;
                tembuf[2]= ProxyResponsedata.PIID;
                
                
                memcpy_r(tembuf+3,(uint8*)&ProxyResponsedata.FrameHeadInfo.oad.value,4);
                if ((result==CHECK_RESULT_CONFIRM) || (isSxTaskManager()))
                {
                    tembuf[7]= 00;
                }
                else
                {
                    tembuf[7]= 03;
                }
                tembuf[8]= 0x00;
                tembuf[9]= 0x00;
                tFrame.apduLen= 10;
                
                PRTL_FMT_LOGCC("ProxyResponsedata.SecMod is %d\n",ProxyResponsedata.SecMod);
                PRTL_BUF_LOGCC(tembuf,tFrame.apduLen,"载波频点设置未加密报文");
                if(ProxyResponsedata.SecMod != SecMod_NULL)
                {

                    tFrame.apduLen = GenSecurityApdu(tembuf,tFrame.apduLen,ProxyResponsedata.SecMod,0);
                   
                }
                PRTL_BUF_LOGCC(tembuf,tFrame.apduLen,"载波频点设置11加密报文");
            }
            else
            {
                tembuf[0]= 0x85;
        		tembuf[1]= 0x01;
                tembuf[2]= ProxyResponsedata.PIID;
                
                memcpy_r(tembuf+3,(uint8*)&ProxyResponsedata.FrameHeadInfo.oad.value,4);
                if(result!=CHECK_RESULT_FAILUER)
                {
                    tembuf[7]= 01;
                    tembuf[8]= DT_UNSIGNED;
                    tembuf[9]= AppData.DataUnitBuff[0];
                    tembuf[10]= 0x00;
                    tembuf[11]= 0x00;
                    tFrame.apduLen= 12;
                }
                else
                {
                    tembuf[7]= 00;
                    tembuf[8]= 03;
                    tembuf[9]= 0x00;
                    tembuf[10]= 0x00;
                    tFrame.apduLen= 11;
                }
                PRTL_BUF_LOGCC(tembuf,tFrame.apduLen,"载波频点读取未加密报文");
                if(ProxyResponsedata.SecMod != SecMod_NULL)
                {

                    tFrame.apduLen = GenSecurityApdu(tembuf,tFrame.apduLen,ProxyResponsedata.SecMod,0);
                   
                }
                PRTL_BUF_LOGCC(tembuf,tFrame.apduLen,"载波频点读取加密报文");
            }
        }
	}
	else
	{
	    PRTL_FMT_LOGCC("lenoffset is %d\n",lenoffset);
        PRTL_FMT_LOGCC("lenoffset is %d\n",ProxyResponsedata.ResponseApduLen);
        PRTL_BUF_LOGCC(ProxyResponsedata.ResponseApdu,ProxyResponsedata.ResponseApduLen,"透传011全部消息报文");
        PRTL_BUF_LOGCC(ProxyResponsedata.ResponseApdu+Bytes,lenoffset,"透传011消息报文");
		memcpy(tembuf,ProxyResponsedata.ResponseApdu+Bytes,lenoffset);	

	}

    memset(rptmsg, 0, APP_SEND_MSG_MAXLEN);
	tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
	tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;
	PRTL_FMT_LOGCC("tFrame.apduLen is %d\n",tFrame.apduLen);
	msgLen=app_frame_to_package(&tFrame, rptmsg, LOCAL_SEND_FRAM_MAXLEN+500, FALSE);
    if(msgLen <= 0)
    {
        PRTL_FMT_DEBUG("rpt_task_report failed.\n");
        return;
    }
    PRTL_BUF_LOGCC(rptmsg,msgLen,"收到的消息");
    //上行报文发送
    id=ProxyResponsedata.FrameHeadInfo.pipe;
    if (id == CHL_MANUAL_AMR)
    {
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
        PRTL_BUF_DEBUG(rptmsg,msgLen,"收到的消息");
        //89 07代理回的3762报文长度 可能会超过127个字节 长度就会占2到3个字节
        uint8 lenoffset3762 = 0;
        
        if(rptmsg[22]==0x81)
            lenoffset3762 = 2;
        else if(rptmsg[22]==0x82)
            lenoffset3762 = 3;
        else
            lenoffset3762 = 1;
        if((rptmsg[33+lenoffset3762]==0x10&&rptmsg[34+lenoffset3762]==0x02)||(rptmsg[33+lenoffset3762]==0x08&&rptmsg[34+lenoffset3762]==0x00))
        gui_line_net_rcv(rptmsg,lenoffset3762);
        else if(rptmsg[23]==0x68&&rptmsg[32]==0x0D&&rptmsg[33]==0x00)
        {
            recvbit = rptmsg[34];
        }
        else if(rptmsg[23]==0x68&&rptmsg[32]==0x0E&&rptmsg[33]==0x03&&rptmsg[34]==0x80&&rptmsg[35]==0x01)
        {
            recvbit = rptmsg[36];
        }
        else
        gui_mannual_amr_rcv(rptmsg);
//#endif
    }
    else
    {
        if(1 == ssal_get_status() && id >= CHL_GPRS_1)
        {
            CHANNEL_CACHE_T *ptChannel = NULL;
            ptChannel = app_get_channel(id);
            msgLen = ssal_commdata_package(ptChannel->tChannel.ip, rptmsg, msgLen, rptmsg, APP_SEND_MSG_MAXLEN);
        }
        ret = send_emergency_ipcmsg(id, 0, rptmsg, msgLen);
    }

    if(ret < 0)
    {
        PRTL_FMT_DEBUG("send_emergency_ipcmsg failed. id(%d)\n", id);
        return;
    }    

    return;
} 


OOP_DAR_E transresult_to_frame(uint8 *inData,uint16 inLen,uint8 *outData, uint16 *outLen,uint16 outMaxlen)

{
    uint32 dOffset=0;
    OOP_TRANSRES_T TransResult;
    uint8 mn=0;
    
    memcpy((uint8 *)&TransResult,inData,inLen);

    outData[dOffset++]= 0x60;
    outData[dOffset++]= 0x1A;
    outData[dOffset++]= 0x02;
    outData[dOffset++]= 0x00;
    outData[dOffset++]= 0x01;
    outData[dOffset++]= 0x00;
    outData[dOffset++]= 0x60;
    outData[dOffset++]= 0x1B;
    outData[dOffset++]= 0x02;
    outData[dOffset++]= 0x00;
    outData[dOffset++]= 0x01;
    outData[dOffset++]= 0x01;
    outData[dOffset++]=DT_STRUCTURE;
    outData[dOffset++]=0x04;;

    if(basic_data_to_buf(E_UNSIGNED, outData, outMaxlen-dOffset, &dOffset, &TransResult.planID, 1) < 0)
    {
        return DATA_TYPE_UNMATCHED;
    }
    if(basic_data_to_buf(E_DATETIME_S, outData, outMaxlen-dOffset, &dOffset, &TransResult.eTime, sizeof(OOP_DATETIME_S_T)) < 0)
    {
        return DATA_TYPE_UNMATCHED;
    }
    if(basic_data_to_buf(E_TSA, outData, outMaxlen-dOffset, &dOffset, &TransResult.addr, sizeof(OOP_TSA_T)) < 0)
    {
        return DATA_TYPE_UNMATCHED;
    }
    
    outData[dOffset++]=DT_ARRAY;
    outData[dOffset++]=TransResult.res.nNum;
    for(mn=0;mn<TransResult.res.nNum;mn++)
    {
        outData[dOffset++]=DT_STRUCTURE;
        outData[dOffset++]=0x03;
        
        if(basic_data_to_buf(E_UNSIGNED, outData, outMaxlen-dOffset, &dOffset, &TransResult.res.result[mn].nIndex, 1) < 0)
        {
            return DATA_TYPE_UNMATCHED;
        } 
        if(basic_data_to_buf(E_DATETIME_S, outData, outMaxlen-dOffset, &dOffset, &TransResult.res.result[mn].aTime, sizeof(OOP_DATETIME_S_T)) < 0)
        {
            return DATA_TYPE_UNMATCHED;
        }
        outData[dOffset++]=DT_OCTET_STRING;
        outData[dOffset++]=TransResult.res.result[mn].data.nNum;
        memcpy((uint8*)&outData[dOffset],TransResult.res.result[mn].data.value,TransResult.res.result[mn].data.nNum);
        dOffset=dOffset+TransResult.res.result[mn].data.nNum;
    }
    *outLen=dOffset;
    return DATA_SUCCESS;

}

/**********************************************************
* 函数名称：代理任务处理
* 功能描述：
* 输入描述：
*
* 返回值：  应答数据
**********************************************************/
int Proc_Proxy(uint16* label,ProxyHeadInfo_t *ProxyHead )
{
	uint8 msgdata[1400];
	uint16 msglen=0;

	uint32 msgid=0;
	Proxyresquest pProxyTaskLink;
	uint16 datalen=0;
	BOOL rslt=FALSE;
    ProxyResponse ProxyResponsedata;

   	datalen=ProxyReadQueue(&ProxyQueue,(uint8*)&pProxyTaskLink);
	if(datalen<=0)
	{
		return 0;
	}
	PRTL_BUF_LOGCC((uint8*)&pProxyTaskLink,sizeof(Proxyresquest),"Proc_Proxy");
	ProxyHead[*label].FrameHeadInfo=pProxyTaskLink.FrameHeadInfo;
	ProxyHead[*label].SecMod=pProxyTaskLink.SecMod;
	ProxyHead[*label].PIID= pProxyTaskLink.PIID;
    ProxyHead[*label].Useid = pProxyTaskLink.ServerID;
	PRTL_FMT_LOGCC("Proc_Proxy1111\n");
  
	rslt=GenProxyMSG(&pProxyTaskLink,&msgid,msgdata,&msglen);
    ProxyResponsedata.UseID = pProxyTaskLink.ServerID;
    if (rslt == FALSE)//代理错误，之间释放代理
    {
        PRTL_FMT_DEBUG("代理错误 rslt = %d\n", rslt);
        ProxyResponsedata.FrameHeadInfo= pProxyTaskLink.FrameHeadInfo;
        ProxyResponsedata.SecMod= pProxyTaskLink.SecMod;
        ProxyResponsedata.ServerID=0x01;
        ProxyResponsedata.PIID=pProxyTaskLink.PIID;
        ProxyResponsedata.ResponseApdu[00] = 0xFF;
        
        ProxyResponsedata.ResponseApduLen=1;
        
        ProxyWriteQueue(&ProxyrespondeQueue,(uint8*)&ProxyResponsedata,sizeof(ProxyResponse));
    }
    else
    {
    	PRTL_FMT_LOGCC("msgid is %x\n",msgid);
    	PRTL_BUF_LOGCC(msgdata,msglen,"msgdata输出");
    	appmain_send_msg(msgid,*label,msgdata,msglen);
        *label = (*label+1)%10;
    }
        
    return 0;
}

/**********************************************************************
* @name      : channel_get_break_frame_size
* @brief     ：获取通道链路分帧大小
* @param[in] ：ptChannel   通道信息
* @param[out]：
* @return    ：分帧大小
**********************************************************************/
uint16 channel_get_break_frame_size(CHANNEL_CACHE_T  *ptChannel)
{
    uint8  medium = COMM_MEDIA_GPRS;    //通信介质
    uint16 size = 0;

    medium = ptChannel->tChannel.medium;

    switch (medium)
    {
        case COMM_MEDIA_485:
        case COMM_MEDIA_232:
        case COMM_MEDIA_BLUETOOTH:
        //case COMM_MEDIA_RADIO:
        {
            size = LOCAL_SEND_FRAM_MAXLEN;
            break;
        }
        case COMM_MEDIA_APP:
        {
            size = 4096;
            break;
        }
		case COMM_MEDIA_RADIO:
		{
			size = RADIO_FRAM_MAXLEN;
		}
		break;
    //....其他通道链路分帧大小，后续补充

        default:
        {
            size = CHANNEL_GET_BREAK_FRAME_SIZE(ptChannel);
            break;
        }
    }

    return size;
}
