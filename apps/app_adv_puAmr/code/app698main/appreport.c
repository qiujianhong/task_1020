/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程主动上报线程实现
* @date：    2019-12-6
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "appcomm.h"
#include "db_api.h"
#include "oopSecurity.h"
#include "appreport.h"
#include "EventIO.h"
#include "AutoTask.h"
#include "oopVarUpdate.h"
#include "framecomm.h"
#include "iniparser.h"
#include "app_gui.h"
#include "hal.h"
#include "led.h"
#include "watchdog.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"
#include "task_id.h"
#include "area.h"
#include "ssal.h"

/* 是否复位发送退出登录 */
#define APP_LOG_OUT_PERMIT   0

#ifdef AREA_FUJIAN
extern int g_rpt_basetime;
#endif
extern uint8 gdelay;

/**********************************************************************
* @name      : app_preconnect_mix
* @brief     ：混合模式预连接管理
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               uint32 period                时间段s
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-6
* @Update    :
**********************************************************************/
void app_preconnect_mix(CHANNEL_CACHE_T *ptChannel, uint32 period)
{
    if(LINK_LOGIN == ptChannel->linkState)
    {
        if(ptChannel->lostHBCount >= ptChannel->tChannel.dailcnt && ptChannel->leftHB + 15 < CHANNEL_GET_HEART(ptChannel))
        {
            APP698_FMT_DEBUG("Channel %d login master %u.%u.%u.%u:%u failed, try %d\n", ptChannel->id, 
                             (ptChannel->tChannel.ip >> 24) & 0xff, (ptChannel->tChannel.ip >> 16) & 0xff,
                             (ptChannel->tChannel.ip >> 8) & 0xff, ptChannel->tChannel.ip & 0xff, 
                              ptChannel->tChannel.port, ptChannel->tChannel.dailcnt);
            send_emergency_ipcmsg(ptChannel->id, PRO_LOGIN_ERROR, &ptChannel->id, 1);
            ptChannel->lostHBCount = 0;
            return;
        }

        uint16 msglen = 0;
        uint8  uMsg[256] = {0};
        int    errCode = 0;
        if(ptChannel->leftHB > period)
        {
            ptChannel->leftHB -= period;
            return;
        }
        msglen = app_linkrequest_package(LINK_LOGIN - 1, CHANNEL_GET_HEART(ptChannel), uMsg, 100);
        if(1 == ssal_get_status())
        {
            msglen = ssal_linkrequest_package(ptChannel->tChannel.ip, uMsg, msglen, uMsg, 256);
        }
        if(0 == msglen)
        {
            APP698_FMT_DEBUG("Channel %d package linkrequest login failed\n", ptChannel->id);
        }
        else
        {
            errCode = send_emergency_ipcmsg(ptChannel->id, PRO_LOGIN_FRAME, uMsg, msglen);
            if(0 != errCode)
            {
                APP698_FMT_DEBUG("Channel %d send linkrequest login failed, code = %d\n", ptChannel->id, errCode);
            }
        }
        ptChannel->lostHBCount += 1; //计数加1
        ptChannel->leftHB = CHANNEL_GET_HEART(ptChannel);
    }
    else if(LINK_QUIT == ptChannel->linkState)
    {
        /* 连接清空 */
        app_set_preconnect(ptChannel->id, LINK_QUIT, 0);
        ptChannel->linkState = LINK_NO;
    }
    else if(LINK_NO != ptChannel->linkState)
    {
        /* 只维护时间 */
        if(ptChannel->leftHB > period)
        {
            ptChannel->leftHB -= period;
        }
        else
        {
            ptChannel->leftHB = 0;
        }
    }
}

/**********************************************************************
* @name      : app_preconnect_client
* @brief     ：客户机预连接管理
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               uint32 period                时间段s
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-6
* @Update    :
**********************************************************************/
void app_preconnect_client(CHANNEL_CACHE_T *ptChannel, uint32 period)
{
    uint16 msglen = 0;
    int    errCode = 0;

    switch(ptChannel->linkState)
    {
        case LINK_LOGIN:
        {
            if(ptChannel->lostHBCount >= ptChannel->tChannel.dailcnt && ptChannel->leftHB + 15 < CHANNEL_GET_HEART(ptChannel))
            {
                APP698_FMT_DEBUG("Channel %d login master %u.%u.%u.%u:%u failed, try %d\n", ptChannel->id, 
                                 (ptChannel->tChannel.ip >> 24) & 0xff, (ptChannel->tChannel.ip >> 16) & 0xff,
                                 (ptChannel->tChannel.ip >> 8) & 0xff, ptChannel->tChannel.ip & 0xff, 
                                  ptChannel->tChannel.port, ptChannel->tChannel.dailcnt);
                send_emergency_ipcmsg(ptChannel->id, PRO_LOGIN_ERROR, &ptChannel->id, 1);
                ptChannel->lostHBCount = 0;
                return;
            }
            if(ptChannel->leftHB > period)
            {
                ptChannel->leftHB -= period;
                return;
            }
            uint8  uMsg[256] = {0};
            msglen = app_linkrequest_package(LINK_LOGIN - 1, CHANNEL_GET_HEART(ptChannel), uMsg, 100);
            if(1 == ssal_get_status())
            {
                msglen = ssal_linkrequest_package(ptChannel->tChannel.ip, uMsg, msglen, uMsg, 256);
            }
            if(0 == msglen)
            {
                APP698_FMT_DEBUG("Channel %d package linkrequest login failed\n", ptChannel->id);
            }
            else
            {
                errCode = send_emergency_ipcmsg(ptChannel->id, PRO_LOGIN_FRAME, uMsg, msglen);
                if(0 != errCode)
                {
                    APP698_FMT_DEBUG("Channel %d send linkrequest failed, code %d\n", ptChannel->id, errCode);
                }
            }
            ptChannel->lostHBCount += 1; //计数加1
            ptChannel->leftHB = ptChannel->tChannel.dailtime;
            break;
        }
        case LINK_HEART:
        {
            if(ptChannel->lostHBCount >= ptChannel->tChannel.dailcnt && ptChannel->leftHB + 15 < CHANNEL_GET_HEART(ptChannel))
            {
                APP698_FMT_DEBUG("Channel %d lost heartbeat, master %u.%u.%u.%u:%u, try %d\n", ptChannel->id, 
                                 (ptChannel->tChannel.ip >> 24) & 0xff, (ptChannel->tChannel.ip >> 16) & 0xff,
                                 (ptChannel->tChannel.ip >> 8) & 0xff, ptChannel->tChannel.ip & 0xff, 
                                  ptChannel->tChannel.port, ptChannel->tChannel.dailcnt);
                send_emergency_ipcmsg(ptChannel->id, PRO_HEARTBEAT_ERROR, &ptChannel->id, 1);
                ptChannel->lostHBCount = 0;
                return;
            }
            
            if(ptChannel->leftHB > period)
            {
                ptChannel->leftHB -= period;
                return;
            }
            if(ptChannel->heartbeatflag == 1)
            {
                return;
            }
#if (defined AREA_SHANDONG)
            /* 山东专变停电不发心跳 */
            if(0x55 == get_tmn_halt())
            {
                return;
            }
#endif
            uint8  uMsg[256] = {0};
            msglen = app_linkrequest_package(LINK_HEART - 1, CHANNEL_GET_HEART(ptChannel), uMsg, 100);
            if(1 == ssal_get_status())
            {
                msglen = ssal_linkrequest_package(ptChannel->tChannel.ip, uMsg, msglen, uMsg, 256);
            }
            if(0 == msglen)
            {
                APP698_FMT_DEBUG("Channel %d package linkrequest login failed\n", ptChannel->id);
            }
            else
            {
                errCode = send_emergency_ipcmsg(ptChannel->id, PRO_HEART_FRAME, uMsg, msglen);
                if(0 != errCode)
                {
                    APP698_FMT_DEBUG("Channel %d send linkrequest heart failed, code %d\n", ptChannel->id, errCode);
                }
            }
            ptChannel->lostHBCount += 1; //计数加1
            ptChannel->leftHB = CHANNEL_GET_HEART(ptChannel);
            break;
        }
        case LINK_QUIT:
        {
            
            uint8  uMsg[100] = {0};
            msglen = app_linkrequest_package(LINK_QUIT - 1, CHANNEL_GET_HEART(ptChannel), uMsg, 100);
            if(0 == msglen)
            {
                APP698_FMT_DEBUG("Channel %d package quit login failed\n", ptChannel->id);
                ptChannel->linkState = LINK_LOGIN;
            }
            else
            {
                ptChannel->linkState = LINK_CHANGE;
                errCode = send_emergency_ipcmsg(ptChannel->id, PRO_HEART_FRAME, uMsg, msglen);
                APP698_FMT_DEBUG("Channel %d send quit login ret %d\n", ptChannel->id, errCode);
                ptChannel->lostHBCount = 0;
                ptChannel->leftHB = CHANNEL_GET_HEART(ptChannel);
                //超时时间设置设置为心跳, 心跳周期没有复位就重新执行一次登录操作
    
                ptChannel->bConnect = FALSE;
                send_emergency_ipcmsg(ptChannel->id, PROT_CLOSE_ACK, &(ptChannel->id), sizeof(uint8));
            }
        }
        default:
        {
            break;
        }
    }
        
}

/**********************************************************************
* @name      : app_preconnect_client_shanxi
* @brief     ：山西客户机预连接管理
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               uint32 period                时间段s
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-6
* @Update    :
**********************************************************************/
void app_preconnect_client_shanxi(CHANNEL_CACHE_T *ptChannel, uint32 period)
{
    uint16 msglen = 0;
    int    errCode = 0;

    switch(ptChannel->linkState)
    {
        case LINK_LOGIN:
        {
            /* 登陆失败不重新连接 */
            if(ptChannel->leftHB > period)
            {
                ptChannel->leftHB -= period;
                return;
            }
            uint8  uMsg[100] = {0};
            msglen = app_linkrequest_package(LINK_LOGIN - 1, CHANNEL_GET_HEART(ptChannel), uMsg, 100);
            if(0 == msglen)
            {
                APP698_FMT_DEBUG("Channel %d package linkrequest login failed\n", ptChannel->id);
            }
            else
            {
                errCode = send_emergency_ipcmsg(ptChannel->id, PRO_LOGIN_FRAME, uMsg, msglen);
                if(0 != errCode)
                {
                    APP698_FMT_DEBUG("Channel %d send linkrequest failed, code %d\n", ptChannel->id, errCode);
                }
            }
            ptChannel->lostHBCount += 1; //计数加1
            switch(ptChannel->lostHBCount)
            {
                case 1:
                case 2:ptChannel->leftHB = 30; break;
                case 3:ptChannel->leftHB = 300; break;
                default: ptChannel->leftHB = 1800; break;
            }
            break;
        }
        case LINK_HEART:
        {
            if(ptChannel->lostHBCount >= ptChannel->tChannel.dailcnt || 
               ptChannel->heartbeatflag > 2 * CHANNEL_GET_HEART(ptChannel))
            {
                APP698_FMT_DEBUG("Channel %d lost heartbeat, master %u.%u.%u.%u:%u, try %d\n", ptChannel->id, 
                                 (ptChannel->tChannel.ip >> 24) & 0xff, (ptChannel->tChannel.ip >> 16) & 0xff,
                                 (ptChannel->tChannel.ip >> 8) & 0xff, ptChannel->tChannel.ip & 0xff, 
                                  ptChannel->tChannel.port, ptChannel->tChannel.dailcnt);
                send_emergency_ipcmsg(ptChannel->id, PRO_HEARTBEAT_ERROR, &ptChannel->id, 1);
                ptChannel->lostHBCount = 0;
                return;
            }

            ptChannel->lostHBtime += period;
            if(ptChannel->leftHB > period)
            {
                ptChannel->leftHB -= period;
                return;
            }
            if(ptChannel->heartbeatflag == 1)
            {
                ptChannel->lostHBtime = 0;
                return;
            }
            uint8  uMsg[100] = {0};
            msglen = app_linkrequest_package(LINK_HEART - 1, CHANNEL_GET_HEART(ptChannel), uMsg, 100);
            if(0 == msglen)
            {
                APP698_FMT_DEBUG("Channel %d package linkrequest login failed\n", ptChannel->id);
            }
            else
            {
                errCode = send_emergency_ipcmsg(ptChannel->id, PRO_HEART_FRAME, uMsg, msglen);
                if(0 != errCode)
                {
                    APP698_FMT_DEBUG("Channel %d send linkrequest heart failed, code %d\n", ptChannel->id, errCode);
                }
            }
            ptChannel->lostHBCount += 1; //计数加1
            switch(ptChannel->lostHBCount)
            {
                case 1:
                case 2:ptChannel->leftHB = 30; break;
                default: ptChannel->leftHB = 300; break;
            }
            break;
        }
        case LINK_QUIT:
        {
            
            uint8  uMsg[100] = {0};
            msglen = app_linkrequest_package(LINK_QUIT - 1, CHANNEL_GET_HEART(ptChannel), uMsg, 100);
            if(0 == msglen)
            {
                APP698_FMT_DEBUG("Channel %d package quit login failed\n", ptChannel->id);
                ptChannel->linkState = LINK_LOGIN;
            }
            else
            {
                ptChannel->linkState = LINK_CHANGE;
                errCode = send_emergency_ipcmsg(ptChannel->id, PRO_HEART_FRAME, uMsg, msglen);
                APP698_FMT_DEBUG("Channel %d send quit login ret %d\n", ptChannel->id, errCode);
                ptChannel->lostHBCount = 0;
                ptChannel->leftHB = CHANNEL_GET_HEART(ptChannel);
                //超时时间设置设置为心跳, 心跳周期没有复位就重新执行一次登录操作
    
                ptChannel->bConnect = FALSE;
                send_emergency_ipcmsg(ptChannel->id, PROT_CLOSE_ACK, &(ptChannel->id), sizeof(uint8));
            }
        }
        default:
        {
            break;
        }
    }
        
}


/**********************************************************************
* @name      : app_preconnect_server
* @brief     ：服务器预连接管理
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               uint32 period                时间段s
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-6
* @Update    :
**********************************************************************/
void app_preconnect_server(CHANNEL_CACHE_T *ptChannel, uint32 period)
{
    if(LINK_QUIT == ptChannel->linkState)
    {
        /* 应用连接清空 退出后不维护心跳 */
        app_set_preconnect(ptChannel->id, LINK_QUIT, 0);
        ptChannel->linkState = LINK_NO;
    }
    else if(LINK_NO != ptChannel->linkState)
    {
        /* 只维护时间 */
        if(ptChannel->leftHB > period)
        {
            ptChannel->leftHB -= period;
        }
        else
        {
            ptChannel->leftHB = 0;
        }

        if(0 == ptChannel->leftHB && COMM_MEDIA_LISTEN == ptChannel->tChannel.medium && ptChannel->tChannel.heart > 0)
        {
            if(ptChannel->lostHBCount >= ptChannel->tChannel.dailcnt && ptChannel->leftHB + 15 < CHANNEL_GET_HEART(ptChannel))
            {
                APP698_FMT_DEBUG("Channel %d lost heartbeat, master %u.%u.%u.%u:%u, try %d\n", ptChannel->id, 
                                 (ptChannel->tChannel.ip >> 24) & 0xff, (ptChannel->tChannel.ip >> 16) & 0xff,
                                 (ptChannel->tChannel.ip >> 8) & 0xff, ptChannel->tChannel.ip & 0xff, 
                                  ptChannel->tChannel.port, ptChannel->tChannel.dailcnt);
                send_emergency_ipcmsg(ptChannel->id, PRO_HEARTBEAT_ERROR, &ptChannel->id, 1);
                ptChannel->lostHBCount = 0;
                return;
            }
            ptChannel->lostHBCount += 1; //计数加1
            ptChannel->leftHB = CHANNEL_GET_HEART(ptChannel);
        }
    }
}

/**********************************************************************
* @name      : app_preconnect_manager
* @brief     ：预连接管理
* @param[in] ：uint32 period    时间差
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
void app_preconnect_manager(uint32 period)
{
    uint8 id;
    CHANNEL_CACHE_T *ptChannel = NULL;

    for(id = 0; id < MAX_CHANNEL_NUM; id++)
    {
        ptChannel = app_get_channel(id);
        if(NULL == ptChannel || FALSE == ptChannel->bOpen)
        {
            continue;
        }
        /* 判断通信介质 */
        if(COMM_MEDIA_GPRS != ptChannel->tChannel.medium && COMM_MEDIA_ETH != ptChannel->tChannel.medium && 
           COMM_MEDIA_LISTEN != ptChannel->tChannel.medium)
        {
            continue;
        }

        switch(ptChannel->tChannel.workmode)
        {
            case MODE_MIX: //混合模式
            {
                app_preconnect_mix(ptChannel, period);
                break;
            }
            case MODE_CLIENT: //客户机模式
            {
                /* 主动发起预连接 */
#ifdef AREA_SHANXI
                app_preconnect_client_shanxi(ptChannel, period);
#else
                app_preconnect_client(ptChannel, period);
#endif
                break;
            }
            case MODE_SERVER: //服务器模式
            {
                app_preconnect_server(ptChannel, period);
                break;
            }
            default: break;
        }
    }
}

/**********************************************************************
* @name      : app_quitlogin_proc
* @brief     ：客户端模式主动退出登录操作
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-11-4
* @Update    :
**********************************************************************/
void app_quitlogin_proc()
{
#if APP_LOG_OUT_PERMIT

    uint8  maxnum = 0;
    uint8  i;
    uint8  id = 0;
    CHANNEL_CACHE_T *ptChannel = NULL;

    maxnum = app_get_opencha_num();
    for(i = 0; i < maxnum; i++)
    {
        id = app_get_opencha_id(i);
        ptChannel = app_get_channel(id);
        if(NULL != ptChannel && TRUE == ptChannel->bOpen && TRUE == ptChannel->bConnect 
           && MODE_CLIENT == ptChannel->tChannel.workmode)
        {
            ptChannel->linkState = LINK_QUIT;
        }
    }
#endif
}

/**********************************************************************
* @name      : appdb_get_localaddr
* @brief     ：获取本地地址
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-7
* @Update    :
**********************************************************************/
int appdb_get_localaddr(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    OOP_OCTETVAR16_T tLocalAddr;

    if(0x40010200 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
    memset(&tLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&tLocalAddr, &readlen);
    if(0 == ret && readlen == sizeof(OOP_OCTETVAR16_T))
    {
        app_set_localaddr(tLocalAddr.nNum, tLocalAddr.value);
        GetMntAddr(tLocalAddr);
    }
    else
    {
        APP698_FMT_DEBUG("Appmain get no local address!\n");
    }
    return 0;
}

/**********************************************************************
* @name      : appdb_get_clientNO
* @brief     ：获取客户编号
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-10
* @Update    :
**********************************************************************/
int appdb_get_clientNO(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    OOP_OCTETVAR16_T tLocalAddr;

    if(0x40030200 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
    memset(&tLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&tLocalAddr, &readlen);
    if(0 == ret && readlen == sizeof(OOP_OCTETVAR16_T))
    {
        app_set_clientNO(tLocalAddr.nNum, tLocalAddr.value);
    }
    else
    {
        APP698_FMT_DEBUG("Appmain get no clientNO!\n");
    }
    return 0;
}

/**********************************************************************
* @name      : appdb_get_groupaddr
* @brief     ：获取组地址
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-7
* @Update    :
**********************************************************************/
int appdb_get_groupaddr(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int              ret = 0;
    uint32           readlen = 0;
    NOMAL_OAD_T      tOAD;
    OOP_GRPADDR_T    tGroupAddr;

    if(0x40050200 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
    memset(&tGroupAddr, 0, sizeof(OOP_GRPADDR_T));
    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_GRPADDR_T), (uint8 *)&tGroupAddr, &readlen);
    if(0 == ret && readlen == sizeof(OOP_GRPADDR_T))
    {
        app_set_groupaddr(&tGroupAddr);
    }
    else
    {
        APP698_FMT_DEBUG("Appmain get no group address!\n");
    }
    return 0;
}

/**********************************************************************
* @name      : appdb_get_password
* @brief     ：获取密码
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-14
* @Update    :
**********************************************************************/
int appdb_get_password(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD;
    OOP_VISIBLEVAR32_T tPwd;

    if(0x44010200 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
    memset(&tPwd, 0, sizeof(OOP_VISIBLEVAR32_T));
    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_VISIBLEVAR32_T), (uint8 *)&tPwd, &readlen);
    if(0 != ret || readlen != sizeof(OOP_VISIBLEVAR32_T))
    {
        APP698_FMT_DEBUG("Using default password!\n");
        tPwd.nNum = sizeof("02000000");
        memcpy(tPwd.value, "02000000", tPwd.nNum);
    }
    app_set_password(&tPwd);
    return 0;
}

/**********************************************************************
* @name      : appdb_get_context
* @brief     ：获取应用语境信息
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
int appdb_get_context(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD;
    OOP_ASSOCIATION_CONTEXT_T tContext;

    if(0x44000300 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
    memset(&tContext, 0, sizeof(OOP_ASSOCIATION_CONTEXT_T));
    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_ASSOCIATION_CONTEXT_T), (uint8 *)&tContext, &readlen);
    if(0 != ret ||readlen != sizeof(OOP_ASSOCIATION_CONTEXT_T))
    {
        APP698_FMT_DEBUG("Using default context!\n");
        uint8 nPCB[8] = {0x00,0x00,0x00,0x03,0xBF,0xFF,0xFF,0xFF};
        uint8 nFCB[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0xFF,0xFF};
        tContext.VER = 0x17;
        tContext.nRecvSize = LOCAL_RECV_FRAM_MAXLEN;
        tContext.nSendSize = LOCAL_SEND_FRAM_MAXLEN;
        tContext.nMaxPDUSize = LOACL_PROC_FRAM_MAXLEN;
        memcpy(tContext.PCB, nPCB, 8);
        memcpy(tContext.FCB, nFCB, 16);
        tContext.nTimeout = 600;
    }
    app_set_context(&tContext);
    return 0;
}

/**********************************************************************
* @name      : appdb_get_version
* @brief     ：获取版本信息
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
int appdb_get_version(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD;
    OOP_VERSION_T      tVersion;
    Factory_Version_t  tVer;
    OOP_VISIBLESTR4_T  factory;

    if(0x43000300 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    memset(&tVer, 0, sizeof(Factory_Version_t));
    memset(&factory, 0, sizeof(OOP_VISIBLESTR4_T));

    tOAD.oad.value = 0xfff00400;
    memset(&tVersion, 0, sizeof(OOP_VERSION_T));
    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_VISIBLESTR4_T), (uint8 *)&factory, &readlen);
    if(0 == ret && readlen == sizeof(OOP_VISIBLESTR4_T))
    {
        memcpy(&tVer.factory[0], factory.value, 4);
    }
    else
    {
        APP698_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, tOAD.logicId, tOAD.infoNum, tOAD.oad.value);
        APP698_FMT_LOGCC("fill in the default factory\n");
        memcpy(&tVer.factory[0], &g_tVer.factory[0], 4);
    }

    memcpy(&tVer.softVer[0], &g_tVer.softVer[0], 4);
    memcpy(&tVer.extend[0], &g_tVer.extend[0], 8);

    memcpy(&tVer.softDate[0], &g_tVer.softDate[0], 6);
    memcpy(&tVer.hardVer[0], &g_tVer.hardVer[0], 4);
    memcpy(&tVer.hardDate[0], &g_tVer.hardDate[0], 6);

    char ver[64] = {0};

    if (app_update_version_get(ver) == 0)
    {
        memcpy(&tVer.softVer[0], ver, 4);
    }

    app_set_version(&tVer);
    VersionCheck(tVersion);

    return 0;
}

/**********************************************************************
* @name      : app_report_init
* @brief     ：自动上报初始化 注册
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-6
* @Update    :
**********************************************************************/
void app_report_init()
{
    //终端事件模块
    InitEvent();
    
    //任务上报的初始化
    rpt_task_init();

    cache_meter_init();

#ifdef AREA_FUJIAN
    rpt_task_init_fj();
#endif
}

/**********************************************************************
* @name      : app_report_getdb
* @brief     ：线程初始化第一次获取数据中心数据
* @param[in] ：int sockdbfd      socket句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-6
* @Update    :
**********************************************************************/
void app_report_getdb(int sockdbfd)
{
    appdb_get_localaddr(sockdbfd, 0, 0, 0x40010200);
    appdb_get_clientNO(sockdbfd, 0, 0, 0x40030200);
    appdb_get_groupaddr(sockdbfd, 0, 0, 0x40050200);
    appdb_get_password(sockdbfd, 0, 0, 0x44010200);
    appdb_get_context(sockdbfd, 0, 0, 0x44000300);
    appdb_get_version(sockdbfd, 0, 0, 0x43000300);
    app_timingmode_read(sockdbfd, 0, 0, 0x40000300);
    app_syncclock_read(sockdbfd, 0, 0, 0x40000400);
    appdb_var_update_init(sockdbfd);
    InitLatestMeterReadDataList(sockdbfd);
}

/**********************************************************************
* @name      : esam_state_proc
* @brief     ：更新esam密钥下发状态
* @param[in] ：
* @param[out]：
* @return    ：
* @Update    :
**********************************************************************/
void esam_state_proc(time_t lstTime)
{
    time_t curTime = time(NULL);

    // 过秒执行
    if (curTime-lstTime)
    {
        if (!advance_mqtt_guiok())
        {
            return;
        }

        if (get_esam_update_state() != 0)
        {
            TESAM_FMT_DEBUG("start esam_state_guishow\n");
            esam_state_guishow(get_esam_state());
        }
    }

    return;
}

/**********************************************************************
* @name      : reset_real_proc
* @brief     ：复位命令后续处理
* @param[in] ：
* @param[out]：
* @return    ：
* @Update    :
**********************************************************************/
void reset_real_proc(time_t lstTime, time_t* rebootingTime)
{
    time_t curTime = time(NULL);
    int32  ret = 0;

    // 过秒执行
    if (curTime-lstTime)
    {
        if((get_reset_flag() == TMN_REBOOT) || (get_reset_flag() == TMN_PARA_INIT))
        {
            reboot_sent_to_smiOS();
            *rebootingTime = 0;
        }

        //统计复位过程时间，大于20秒后，将硬狗喂狗时间设置成1秒，强行重启
        if(get_reset_flag() == TMN_REBOOTING)
        {
            (*rebootingTime) ++;
            if (*rebootingTime >= 20)
            {
                *rebootingTime = 0;
                APP698_FMT_DEBUG("rebootingTime too long. need set watchdog period\n");
                WATCHDOG_DEVICE_T* dev = (WATCHDOG_DEVICE_T*)hal_device_get(HW_DEVICE_ID_WATCHDOG);
                if (dev == NULL)
                {
                    APP698_FMT_DEBUG("watchdog dev null\n");
                }
                else
                {
                    ret = dev->watchdog_period_set(dev, 1);
                    if (ret != ERR_OK)
                    {
                        APP698_FMT_DEBUG("set watchdog period 1s err\n");
                    }
                    hal_device_release((HW_DEVICE *)dev);
                    set_reset_flag(0, TMN_NOACT);
                }
            }
        }

        //延迟20秒复位
        if (get_reset_flag() == TMN_RBT_DELAY_20S)
        {
            (*rebootingTime) ++;
            APP698_FMT_DEBUG("准备重启，延时%d\n", (*rebootingTime));
            if (*rebootingTime >= 20)
            {
                *rebootingTime = 0;
                APP698_FMT_DEBUG("延迟时间到，给smiOS发重启命令\n");
                reboot_sent_to_smiOS();
                set_reset_flag(0, TMN_NOACT);
            }
        }
    }

    return;
}

extern void *app_leddev_get();

/**********************************************************************
* @name      : app_run_led_status
* @brief     ：控制运行LED
* @param[in] ：uint8 bOpen   打开/关闭
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-24
* @Update    : 
**********************************************************************/
void app_run_led_status(uint8 bOpen)
{
    LED_DEVICE_T* dev = NULL;

    dev = (LED_DEVICE_T*)app_leddev_get();
    if(NULL == dev)
    {
        CTRL_FMT_DEBUG("LED dev is null\n");
        return;
    }
    if(FALSE == bOpen)
    {
        dev->led_state_set(dev, EMLED_RUN, LED_OFF, COM_MODE, NULL, 0);
    }
    else
    {
        dev->led_state_set(dev, EMLED_RUN, LED_ON, COM_MODE, NULL, 0);
    }
}

extern uint8 app_check_master_connect();

/**********************************************************************
* @name      : app_preconnect_report
* @brief     ：app预连接
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-20
* @Update    :
**********************************************************************/
void *app_preconnect_proc(void *pArg)
{
    uint8 id;
    REPORT_THREAD_T *pHandle = (REPORT_THREAD_T *)pArg;
    struct timespec  sCurtime;
    struct timespec  sLasttime;
    uint8 bLed = TRUE;
    /* 线程重启后 智能解锁 防止死锁 */
    for(id = 0; id < MAX_CHANNEL_NUM; id++)
    {
        app_unlock_channel(id, APP698_AUTO_LOCK);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &sLasttime); //系统的启动时间，不能被设置
    while(1)
    {
        *pHandle->pRunFlag = 0;
        clock_gettime(CLOCK_MONOTONIC, &sCurtime);//系统的启动时间，不能被设置

        /* 预连接管理 运行灯管理 过秒执行 */
        if(sCurtime.tv_sec != sLasttime.tv_sec)
        {
            app_preconnect_manager(sCurtime.tv_sec - sLasttime.tv_sec);

            if(TRUE == app_check_master_connect())
            {
                bLed = 1 - bLed;
                app_run_led_status(bLed);
            }
            else
            {
                if(FALSE == bLed)
                {
                    bLed = TRUE;
                    app_run_led_status(bLed);
                }
            }
        }

        usleep(200000);
        sLasttime = sCurtime;
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}

extern DB_CLIENT g_rptUdpHandle;
/**********************************************************************
* @name      : app_auto_report
* @brief     ：主动上报接收消息
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
void *app_auto_report(void *pArg)
{
    REPORT_THREAD_T *pHandle = (REPORT_THREAD_T *)pArg;
    time_t           curTime = 0;
    time_t           lastTime = 0;
#ifdef AREA_FUJIAN    
    time_t  curtime = {0};
    time_t  lsttime = {0};
#endif

    /* 再次初始化促使解锁 */
    g_rptUdpHandle = db_client_create(DB_YC_AGENT, DB_CLIENT_REPORT);
    if(g_rptUdpHandle <= 0)
    {
        APP698_FMT_DEBUG("rpt db_handle_create failed!\n");
    }

    lastTime = time(NULL);
    while(1)
    {
        *pHandle->pRunFlag = 0;
        curTime = time(NULL);
        cache_meter_entry(g_rptUdpHandle,curTime, lastTime);

#ifdef AREA_FUJIAN
        //福建切换国网和福建模式
        if(get_tmn_gather() == OOP_GATHER_FJ)
        {
            curtime = get_time(TIME_MONOTONIC)+g_rpt_basetime;
            rpt_task_fj_entry(curtime, lsttime);
            rpt_retry_fj_entry(curtime, lsttime);
            //对时任务处理
            rpt_timetask_entry(lastTime);
            lsttime = curtime;
        }
        else
        {
            /* 事件主动上报*/
            rpt_event_entry(curTime, lastTime);

            /* 电能表事件主动上报*/
            rpt_mtr_evt_entry(curTime, lastTime);

            /* 任务主动上报*/
            rpt_task_entry(curTime, lastTime);

            /* 透传返回报文上报 */
            rpt_transresult_data();

            //对时任务处理
            rpt_timetask_entry(lastTime);

        }
#else
        /* 事件主动上报*/
        rpt_event_entry(curTime, lastTime);

        /* 电能表事件主动上报*/
        rpt_mtr_evt_entry(curTime, lastTime);

        /* 任务主动上报*/
        rpt_task_entry(curTime, lastTime);

        /* 透传返回报文上报 */
        rpt_transresult_data();

        //对时任务处理
        rpt_timetask_entry(lastTime);
#endif

        usleep(200000);
        lastTime = curTime;
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}

/**********************************************************************
* @name      : app_event_time_work
* @brief     ：事件处理流程
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-12-25
* @Update    :
**********************************************************************/
void *app_event_time_work(void *pArg)
{
    REPORT_THREAD_T *pHandle = (REPORT_THREAD_T *)pArg;
    
    while(1)
    {
        *pHandle->pRunFlag = 0;

        EventHappenProc();
        usleep(200000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}

extern int systime_set_to_smiOS(DateTimeHex_t *pDateTime);

/**********************************************************************
* @name      : app_other_time_work
* @brief     ：app其他任务
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-12-24
* @Update    :
**********************************************************************/
void *app_other_time_work(void *pArg)
{
    REPORT_THREAD_T *pHandle = (REPORT_THREAD_T *)pArg;
    OOP_DATETIME_S_T curTime = {0};
    DateTimeHex_t    newTime ;
    OOP_DATETIME_S_T newEventTime = {0};
    time_t           now;
    time_t           newtimes;
    time_t           last;
    uint8            bTimeCheckflag = FALSE;
    uint8            buf[160] = {0};
    GPS_T            tGps;
    int              ret = 0;
    DB_CLIENT        dbOther = 0;
    time_t           rebootingTime = 0;
    OOP_SYNCCYCLE_T  tsync;

    memset(&newTime,0,sizeof(DateTimeHex_t));
    dbOther = db_client_create(DB_YC_AGENT, DB_CLIENT_OTHERTIME);
    if(dbOther <= 0)
    {
        APP698_FMT_DEBUG("db_handle_create failed!\n");
        return 0;
    }

    time(&last);
    while(1)
    {
        *pHandle->pRunFlag = 0;
        time(&now);
        DT_Time2DateTimeShort(now, &curTime);

		/* 代理 */
		rpt_proxyresponse_data();

        /* 更新esam密钥下发状态 */
        esam_state_proc(last);

        /* 复位命令后续处理 */
        reset_real_proc(last, &rebootingTime);

        /* 卫星授时每分钟19s对时 最大限度减少存在业务时的对时操作 */
        if(curTime.second >= 19 && FALSE == bTimeCheckflag && 2 == app_get_timingmode()&&
           0 == app_get_synccycle_param(&tsync))
        {
            bTimeCheckflag = TRUE;

            MEMZERO(&tGps, sizeof(GPS_T));
            if(0 != appmain_get_ppp_gps(1, buf, 160))
            {
                appmsg_gps_split(buf, &tGps);
            }
            else if(0 != appmain_get_ppp_gps(2, buf, 160))
            {
                appmsg_gps_split(buf, &tGps);
            }
            if(1 == tGps.ststus && 4 != tGps.mode)
            {
                /* 能源控制器需要将位置信息写入数据中心 防止模块复位时查询 */
                OOP_GEOGADD_A_T  tOOPGPS;
                tOOPGPS.type = 1;
                tOOPGPS.newaddr.height = tGps.position.height;
                tOOPGPS.newaddr.longitude.position = tGps.position.longitude.position;
                tOOPGPS.newaddr.longitude.degre = tGps.position.longitude.degre;
                tOOPGPS.newaddr.longitude.branch = tGps.position.longitude.branch;
                tOOPGPS.newaddr.latitude.position = tGps.position.latitude.position;
                tOOPGPS.newaddr.latitude.degre = tGps.position.latitude.degre;
                tOOPGPS.newaddr.latitude.branch = tGps.position.latitude.branch;
                appmain_set_gps_tempposition(&tOOPGPS);

                /* 时间修改 */
                PRTL_FMT_DEBUG("GPS get time %04d-%02d-%02d %02d:%02d:%02d\n", tGps.utctime.year, tGps.utctime.month,
                               tGps.utctime.mday, tGps.utctime.hour, tGps.utctime.minute, tGps.utctime.second);
                /* 计算新时间 增加8h */
                newEventTime.year = tGps.utctime.year;
                newEventTime.month = tGps.utctime.month;
                newEventTime.day = tGps.utctime.mday;
                newEventTime.hour = tGps.utctime.hour;
                newEventTime.minute = tGps.utctime.minute;
                newEventTime.second = tGps.utctime.second;
                newEventTime.week = tGps.utctime.wday;
                
                DT_DateTimeShort2Time(&newEventTime, &newtimes);
                newtimes += 8 * 3600;  /* 增加时区 */
                if(newtimes > now + 1 || newtimes < now - 1)
                {
                    DT_Time2DateTimeShort(newtimes, &newEventTime);
                    newTime.year_h = ((newEventTime.year >> 8) & 0xff);
                    newTime.year_l = (newEventTime.year & 0xff);
                    newTime.month  = newEventTime.month;
                    newTime.day    = newEventTime.day;
                    newTime.hour   = newEventTime.hour;
                    newTime.min    = newEventTime.minute;
                    newTime.sec    = newEventTime.second;
                    ret = systime_set_to_smiOS(&newTime);
                    if(0 == ret)
                    {
                        newEventTime = curTime;
                        newEventTime.hour = newTime.hour;
                        newEventTime.minute = newTime.min;
                        newEventTime.second = newTime.sec;

                        PRTL_FMT_DEBUG("GPS change time %04d-%02d-%02d %02d:%02d:%02d\n", newEventTime.year, newEventTime.month,
                                        newEventTime.day, newEventTime.hour, newEventTime.minute, newEventTime.second);
                        CheckERC_3114(dbOther, curTime, newEventTime);
                    }
                }
            }
        }
        if(curTime.second < 19)
        {
            bTimeCheckflag = FALSE;
        }

        usleep(10000);
        last = now;
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}

