/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程外部通信线程
* @date：    2019-12-3
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "comm.h"
#include "ble.h"
#include "appcomm.h"
#include "appoutmsg.h"
#include "framecomm.h"
#include "EventIO.h"
#include "label_id.h"
#include "appctrl.h"
#include "oopStorage.h"
#include "appctrl_module.h"
#include "hal.h"
#include "devinfo.h"


extern CTRL_DEV_H_T  g_tCtrlModuleHandle;
extern ProxyLockQueue ProxyrespondeQueue;
extern void sta_restart_time_store(void);
extern void sta_mod_event(TASK_MSG_t *pMsg);
extern void comm_set_rediag(uint8 id);
#ifdef  AREA_FUJIAN
extern void rpt_trans_task_msg(int clientid, TASK_MSG_t *pMsg);
extern void rpt_mtr_event_msg(int clientid, TASK_MSG_t *pMsg);
#endif

pthread_mutex_t  g_fAPPMainMsg;
int g_APPMainFd = 0;
int g_APPMainSync = 0;
OOP_GEOGADD_A_T  g_tNewOOPGPS;

APP_MSG_FUN_T g_AppOutMsg[] = 
{
#ifdef ADV_ST_AMR
    {MSG_CTRL_MOD_GET_PORT,   appctrl_modport_ack},
    {MSG_CTRL_MOD_EVENT,      appctrl_mod_event},
    {MSG_CTRL_GUI_HINT,       appctrl_gui_msg},
    {MSG_CTRL_IF_NOHINT,      appctrl_if_msg},
#endif
	{MSG_APPAMIN_IOT_EVT,     advance_iot_evt_msg},
};
uint8 g_nAppMsgNum = sizeof(g_AppOutMsg)/sizeof(APP_MSG_FUN_T);
extern uint8 gdelay;

/**********************************************************************
* @name      : app_msg_sync_init
* @brief     ：本机内部同步接口初始化
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-3-15
* @Update    :
**********************************************************************/
void app_msg_sync_init()
{
    if(g_APPMainSync <= 0)
    {
        g_APPMainSync = task_msg_init(APPMAIN_SYNCMSG);
        if(g_APPMainSync <= 0)
        {
            APP698_FMT_DEBUG("Appmain task msg init failed!\n");
            return;
        }
        pthread_mutex_init(&g_fAPPMainMsg, NULL);
    }

    memset(&g_tNewOOPGPS, 0, sizeof(g_tNewOOPGPS));
    g_tNewOOPGPS.type = 1;
}

/**********************************************************************
* @name      : app_msg_entry
* @brief     ：本机内部通信线程
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
void *app_msg_entry(void *pArg)
{
    OUTMSG_THREAD_T *ptMsgHandle = (OUTMSG_THREAD_T*)pArg;
    int              clientid = *ptMsgHandle->pClientId;
    uint8            recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t      *pMsg = (TASK_MSG_t *)recvBuf;
    int              recvlen = 0;
    uint32           sender = 0;
    int              ret = 0;
    uint8            i;

	static uint16           label = 0;
    ProxyHeadInfo_t  ProxyHead[10];
	ProxyResponse    ProxyResponsedata;
	
    struct timespec  curtime = {0};
    struct timespec  oldtime = {0};
    uint8            bFalg = FALSE;
    static uint8 timeCnt = 0;
    int id = 0;


    ptMsgHandle->timeStep = CTRL_LABEL_MODPORT;

    if(g_APPMainFd <= 0)
    {
        g_APPMainFd = task_msg_init(APPMAIN_MSG);
        if(g_APPMainFd <= 0)
        {
            APP698_FMT_DEBUG("Appmain task msg init failed!\n");
            return 0;
        }
    }
    task_msg_settime(g_APPMainFd, 0, 10000);  //设置阻塞超时时间为0.01s
    APP698_FMT_TRACE("Appmain task client id %d\n", clientid);

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    while(1)
    {
        Proc_Proxy(&label,ProxyHead);
        *ptMsgHandle->pRunFlag = 0;
        recvlen = task_recv_msg(g_APPMainFd, recvBuf, TASK_MSG_MAX_LEN, &sender);
        if(recvlen > 0)
        {
			APP698_BUF_TRACE(pMsg->msg, pMsg->msglen,"app msg recv msgid 0x%x label 0x%x:", pMsg->msg_id, pMsg->label);
            bFalg = FALSE;
            for(i = 0; i < g_nAppMsgNum; i++)
            {
                if(pMsg->msg_id == g_AppOutMsg[i].msgid)
                {
                    if(NULL != g_AppOutMsg[i].func)
                    {
                        ret = g_AppOutMsg[i].func(clientid, pMsg->label, pMsg->msg, pMsg->msglen, &(ptMsgHandle->timeStep));
                        EVENT_FMT_TRACE("AppMain proc msg 0x%x: label %d, ret %d\n", pMsg->msg_id, pMsg->label, ret);
                    }
                    bFalg = TRUE;
                    break;
                }
            }
            if(TRUE == bFalg)
            {
                continue;
            }
            switch(pMsg->msg_id)
            {
                case MSG_APPAMIN_RESTART:
                {
                    sta_restart_time_store();
                }
                break;
                case MSG_APPAMIN_CTS_READ_LIST:
                {   
                    if(pMsg->label == LABEL_HALT_EVENT)
                    {
                        ret = Pro_RelayHalt(pMsg->msg, pMsg->msglen);
                        if(0 != ret)
                        {
                            EVENT_FMT_DEBUG("Relay Halt msg error, ret %d\n", ret);
                        }
                        continue;
                    }
                    else if (((pMsg->label & 0xf0ff) == LABEL_HALT_TRANS_YX) || 
                             ((pMsg->label & 0xf0ff) == LABEL_HALT_TRANS_CT) || 
                             ((pMsg->label & 0xf0ff) == LABEL_HALT_TRANS_CN))
                    {
                        ret = app_module_ctrl_msg_proc(&g_tCtrlModuleHandle, pMsg);
                        if (ERR_OK != ret)
                        {
                            SM_FMT_DEBUG("扩展控制模块消息处理失败, ret %d\n", ret);
                        }
                        continue;
                    }
                    ProxyResponsedata.ServerID=0x01;
                    //Proc_Proxy(&FrameHeadInfo, label, &SecMod, &PIID,&Useid);
                    if(pMsg->msglen>0)
                    {
                        
                        ProxyResponsedata.FrameHeadInfo= ProxyHead[pMsg->label].FrameHeadInfo;
                        ProxyResponsedata.SecMod= ProxyHead[pMsg->label].SecMod;
                        ProxyResponsedata.PIID=ProxyHead[pMsg->label].PIID;
                        ProxyResponsedata.UseID=ProxyHead[pMsg->label].Useid;
                        
                        ProxyResponsedata.ResponseApduLen=pMsg->msglen;
                        memcpy(ProxyResponsedata.ResponseApdu,pMsg->msg,pMsg->msglen);
      
                        PRTL_BUF_LOGCC(pMsg->msg,pMsg->msglen,"原始消息报文");
                        PRTL_FMT_LOGCC("载波透传报文形式,ProxyResponsedata.UseID is %d\n",ProxyResponsedata.UseID);
                        ProxyWriteQueue(&ProxyrespondeQueue,(uint8*)&ProxyResponsedata,sizeof(ProxyResponse));
                    }
                }       
                break;
                case MSG_APPAMIN_CTS_READ_ONE:
                {
                    //Proc_Proxy(&FrameHeadInfo, label, &SecMod, &PIID,&Useid);
                    ProxyResponsedata.ServerID=0x02;
                    if(pMsg->msglen>0)
                    {
                        
                        ProxyResponsedata.FrameHeadInfo= ProxyHead[pMsg->label].FrameHeadInfo;
                        ProxyResponsedata.SecMod= ProxyHead[pMsg->label].SecMod;
                        ProxyResponsedata.PIID=ProxyHead[pMsg->label].PIID;
                        ProxyResponsedata.UseID=ProxyHead[pMsg->label].Useid;
                        ProxyResponsedata.ResponseApduLen=pMsg->msglen;
                        memcpy(ProxyResponsedata.ResponseApdu,pMsg->msg,pMsg->msglen);

                        PRTL_BUF_LOGCC(pMsg->msg,pMsg->msglen,"原始消息报文");
                        ProxyWriteQueue(&ProxyrespondeQueue,(uint8*)&ProxyResponsedata,sizeof(ProxyResponse));
                    }
                }       
                break;

                case MSG_APPAMIN_CTS_SETREAD_LIST:
                {
                    RecvReadMeterStatus(pMsg->msg, pMsg->msglen, pMsg->label);
                    ProxyResponsedata.ServerID=0x04;
                    continue;
                }       
                break;

                case MSG_APPAMIN_CTS_TRANSPORT:
                {
                    ProxyResponsedata.ServerID=0x07;
                }       
                break;
                
                case MSG_APPAMIN_ETH_PORT_OPEN:  //以太网远程端口开放监控
                case MSG_APPAMIN_ETH_PORT_CONN:  //以太网外联信息监控
                case MSG_APPAMIN_KEY_PATH_CHG:   //关键目录文件变更监控
                case MSG_APPAMIN_ETH_LOGIN_CONSOLE: //以太网登录控制台监控
                case MSG_APPAMIN_DANGER_OPREATE: //危险命令操作监控 
                case MSG_APPAMIN_LOCAL_MAINTAIN: //本地维护信息监控 
                case MSG_APPAMIN_UDISK_AUTHORIZE: //USB设备接入信息监控
                case MSG_APPAMIN_PORT485_AUTHORIZE://485维护口接入信息监控
                case MSG_APPAMIN_PASSWARD_CHANGED: //口令信息变更
                case MSG_APPAMIN_UART_PORT_LOGIN:  //串口登录控制台
                case MSG_APPAMIN_SESSION_ABNORMAL: //流量会话异常
                case MSG_APPAMIN_BANDWIDTH_ABNORMAL://流量带宽异常
                case MSG_APPAMIN_PROCESS_ABNORMAL:   //进程异常
                case MSG_APPAMIN_LOGIN_ABNORMAL:     //登录异常
                {
                    CheckERC_3140(clientid, pMsg->msg_id, pMsg->msg, pMsg->msglen);
                    continue;
                }       
                break;   
                case MSG_APPAMIN_APPLYSTATUS:
                {
                    //收到确认帧
                    proxy_apply_ack_set(0);
                }       
                break;


                case MSG_APPAMIN_MOD_EVENT:
                {
                    sta_mod_event(pMsg);
                }break;
#ifdef  AREA_FUJIAN
                case MSG_APPAMIN_TRANSTASK_RPT:
                {
                    rpt_trans_task_msg(clientid, pMsg);
                }break; 
                
                case MSG_APPAMIN_MTREVENT_RPT:
                {
                    rpt_mtr_event_msg(clientid, pMsg);
                }break;                   
#endif                
                default:
                    continue;
                break;
            }

            memset(recvBuf, 0, recvlen);
        }

        /* 其他定时处理 控制模块开启 */

        clock_gettime(CLOCK_MONOTONIC, &curtime);
        if(curtime.tv_sec - oldtime.tv_sec >= 5)
        {
#ifdef ADV_ST_AMR
            //5s定时发送一次
            oldtime = curtime;
            if(CTRL_LABEL_MODPORT == ptMsgHandle->timeStep)
            {
                appctrl_getmsg_port(g_APPMainFd);
            }
#endif
            if(app_comm_proxy_isuse())
            {
                //10s的定时发送
                if(++timeCnt == 2)
                {
                    timeCnt = 0;

                    for(id = 0; id < MAX_CHANNEL_NUM; id++)
                    {
                        if((proxy_apply_ack_get(id) != TRUE) && (proxy_apply_retry_get(id) > 0))
                        {
                            proxy_coninfo_notify(id);
                            proxy_apply_retry_sub(id);
                        }
                    }
                }
            }
        }

        usleep(10);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
    return 0;
}

/**********************************************************************
* @name      : appmain_send_msg
* @brief     ：APPAMIN发送消息
* @param[in] ：uint32 msgid    消息ID
               uint16 label    消息label
               uint8 *pMsg     消息内容
               uint16 msglen   消息内容长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-2
* @Update    :
**********************************************************************/
int appmain_send_msg(uint32 msgid, uint16 label, uint8 *pMsg, uint16 msglen)
{
    int    ret = 0;
    ret = task_send_labelmsg(g_APPMainFd, msgid, label, pMsg, msglen, MSG_MONITOR);
    
    if(0 == ret)
    {
        APP698_BUF_TRACE(pMsg, msglen, "Send to 0x%x: msg 0x%x, label 0x%x, ret %d:", MSG_MONITOR, msgid, label, ret);
    }
    else
    {
        APP698_BUF_DEBUG(pMsg, msglen, "Send to 0x%x: msg 0x%x, label 0x%x, ret %d:", MSG_MONITOR, msgid, label, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : appmain_send_syncmsg
* @brief     ：APPAMIN发送同步消息消息, 
              目前适配消息 MSG_APPAMIN_RCT MSG_APPAMIN_REBOOT MSG_APPAMIN_SET_IPADDR MSG_APPAMIN_CTS_SEEK_METER
* @param[in] ：uint32 msgid    消息ID
               uint8 *sendbuf  发送内容
               uint16 sendlen  发送长度
               uint16 buflen   接收缓存大小
* @param[out]：uint8 *recvbuf  接收内容
               uint16 *recvLen 接收长度
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-19
* @Update    :
**********************************************************************/
int appmain_send_syncmsg_ccon(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen)
{
    int    ret = 0;
    uint8  recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;

    PTHREAD_LOCK_SAFE(&g_fAPPMainMsg);
    ret = task_syncmsg_safe(g_APPMainSync, msgid, sendbuf, sendlen, MSG_MONITOR, recvBuf, TASK_MSG_MAX_LEN, 15);
    PTHREAD_UNLOCK_SAFE(&g_fAPPMainMsg);

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
* @name      : appmain_send_syncmsg
* @brief     ：APPAMIN发送同步消息消息, 
              目前适配消息 MSG_APPAMIN_RCT MSG_APPAMIN_REBOOT MSG_APPAMIN_SET_IPADDR MSG_APPAMIN_CTS_SEEK_METER
* @param[in] ：uint32 msgid    消息ID
               uint8 *sendbuf  发送内容
               uint16 sendlen  发送长度
               uint16 buflen   接收缓存大小
* @param[out]：uint8 *recvbuf  接收内容
               uint16 *recvLen 接收长度
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-19
* @Update    :
**********************************************************************/
int appmain_send_syncmsg_with_outtime(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen,uint16 outtime)
{
    int    ret = 0;
    uint8  recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;

    PTHREAD_LOCK_SAFE(&g_fAPPMainMsg);
    ret = task_syncmsg_safe_long_outtime(g_APPMainSync, msgid, sendbuf, sendlen, MSG_MONITOR, recvBuf, TASK_MSG_MAX_LEN, outtime);
    PTHREAD_UNLOCK_SAFE(&g_fAPPMainMsg);

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
* @name      : appmain_send_syncmsg
* @brief     ：APPAMIN发送同步消息消息, 
              目前适配消息 MSG_APPAMIN_RCT MSG_APPAMIN_REBOOT MSG_APPAMIN_SET_IPADDR MSG_APPAMIN_CTS_SEEK_METER
* @param[in] ：uint32 msgid    消息ID
               uint8 *sendbuf  发送内容
               uint16 sendlen  发送长度
               uint16 buflen   接收缓存大小
* @param[out]：uint8 *recvbuf  接收内容
               uint16 *recvLen 接收长度
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-19
* @Update    :
**********************************************************************/
int appmain_send_syncmsg(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen)
{
    int    ret = 0;
    uint8  recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;

    PTHREAD_LOCK_SAFE(&g_fAPPMainMsg);
    ret = task_syncmsg_safe(g_APPMainSync, msgid, sendbuf, sendlen, MSG_MONITOR, recvBuf, TASK_MSG_MAX_LEN, 4);
    PTHREAD_UNLOCK_SAFE(&g_fAPPMainMsg);

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
* @name      : appmain_send_label_syncmsg
* @brief     ：APPAMIN发送同步消息消息, 
              目前适配消息 MSG_APPAMIN_RCT MSG_APPAMIN_REBOOT MSG_APPAMIN_SET_IPADDR MSG_APPAMIN_CTS_SEEK_METER
* @param[in] ：uint32 msgid    消息ID
               uint8 *sendbuf  发送内容
               uint16 sendlen  发送长度
               uint16 buflen   接收缓存大小
* @param[out]：uint8 *recvbuf  接收内容
               uint16 *recvLen 接收长度
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-19
* @Update    :
**********************************************************************/
int appmain_send_label_syncmsg(uint32 msgid, uint16 label, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, 
    uint16 *recvLen, uint32 overtime)
{
    int    ret = 0;
    uint8  recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;

    PTHREAD_LOCK_SAFE(&g_fAPPMainMsg);
    ret = task_syncmsg_label_safe(g_APPMainSync, msgid, label, sendbuf, sendlen, MSG_MONITOR, recvBuf, TASK_MSG_MAX_LEN, overtime);
    PTHREAD_UNLOCK_SAFE(&g_fAPPMainMsg);

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
* @name      : appmain_send_syncmsg_time
* @brief     ：APPAMIN发送同步消息消息, 支持时间设置
* @param[in] ：uint32 msgid    消息ID
               uint8 *sendbuf  发送内容
               uint16 sendlen  发送长度
               uint16 buflen   接收缓存大小
               uint8  outtime  超时时间
* @param[out]：uint8 *recvbuf  接收内容
               uint16 *recvLen 接收长度
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-19
* @Update    :
**********************************************************************/
int appmain_send_syncmsg_time(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, 
                              uint16 *recvLen, uint8 outtime)
{
    int    ret = 0;
    uint8  recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;

    PTHREAD_LOCK_SAFE(&g_fAPPMainMsg);
    ret = task_syncmsg_safe(g_APPMainSync, msgid, sendbuf, sendlen, MSG_MONITOR, recvBuf, TASK_MSG_MAX_LEN, outtime);
    PTHREAD_UNLOCK_SAFE(&g_fAPPMainMsg);

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
* @name      : reboot_sent_to_smiOS
* @brief     ：向系统管理器发送重启命令
* @param[in] ：
* @return    ：
* @Date      ：2020-3-18
* @Update    :
**********************************************************************/
int reboot_sent_to_smiOS(void)
{
    uint8  recvbuf[32] = {0};
    uint16 recLen = 0;
    int    ret = 0;
    int    nRet = 0;

    /* 设置通道状态, 需要发送退出登录消息 */
    app_quitlogin_proc();
    //sleep(3);

    /* 重启之前 需要将无线模块进行离线操作 防止终端重启首次拨号失败 */
    comm_set_rediag(1);
    comm_set_rediag(2);

    ret = appmain_send_syncmsg(MSG_APPAMIN_REBOOT, NULL, 0, recvbuf, 32, &recLen);
    if ((ret != ERR_OK) || (recLen != 1) || (recvbuf[0] != 1))
    {
        PRTL_FMT_DEBUG("reboot_sent_to_smiOS err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }

    set_reset_flag(0, TMN_REBOOTING);
    
    return nRet;
}

/**********************************************************************
* @name      : param_init_sent_to_acmeter
* @brief     ：向交采模块发送参数初始化
* @param[in] ：
* @return    ：
* @Date      ：2020-3-18
* @Update    :
**********************************************************************/
int param_init_sent_to_acmeter(void)
{
    uint8  recvbuf[32] = {0};
    uint16 recLen = 0;
    int    ret = 0;
    int    nRet = 0;

    ret = appmain_send_syncmsg(MSG_APPAMIN_AC_PARAM_INIT, NULL, 0, recvbuf, 32, &recLen);
    if ((ret != ERR_OK) || (recLen != 1) || (recvbuf[0] != 1))
    {
        PRTL_FMT_DEBUG("param_init_sent_to_acmeter err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}

/**********************************************************************
* @name      : data_init_sent_to_acmeter
* @brief     ：向交采模块发送数据初始化
* @param[in] ：
* @return    ：
* @Date      ：2020-3-18
* @Update    :
**********************************************************************/
int data_init_sent_to_acmeter(void)
{
    uint8  recvbuf[32] = {0};
    uint16 recLen = 0;
    int    ret = 0;
    int    nRet = 0;

    ret = appmain_send_syncmsg(MSG_APPAMIN_AC_DATA_INIT, NULL, 0, recvbuf, 32, &recLen);
    if ((ret != ERR_OK) || (recLen != 1) || (recvbuf[0] != 1))
    {
        PRTL_FMT_DEBUG("data_init_sent_to_acmeter err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}

/**********************************************************************
* @name      : demand_init_sent_to_acmeter
* @brief     ：向交采模块发送需量初始化
* @param[in] ：
* @return    ：
* @Date      ：2020-3-18
* @Update    :
**********************************************************************/
int demand_init_sent_to_acmeter(void)
{
    uint8  recvbuf[32] = {0};
    uint16 recLen = 0;
    int    ret = 0;
    int    nRet = 0;

    ret = appmain_send_syncmsg(MSG_APPAMIN_AC_DEMAND_INIT, NULL, 0, recvbuf, 32, &recLen);
    if ((ret != ERR_OK) || (recLen != 1) || (recvbuf[0] != 1))
    {
        PRTL_FMT_DEBUG("demand_init_sent_to_acmeter err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}

/**********************************************************************
* @name      : system_cmd_sent_to_acmeter
* @brief     ：执行system命令
* @param[in] ：
* @return    ：
* @Date      ：2020-3-18
* @Update    :
**********************************************************************/
int system_cmd_sent_to_acmeter(const char* cmd)
{
    int    ret;
    uint8  recvbuf[2000]={0};
    uint16 recvlen;
    uint8  sendbuf[500]={0};
    uint16 sendlen=0;
    uint16 index=0;
    uint32 result;
    uint8  lenoffset;

    PRTL_FMT_DEBUG("开始执行system命令 %s\n",cmd);
    sendbuf[sendlen ++] = 1;   //一次执行一个
    lenoffset = set_len_offset(strlen(cmd), &sendbuf[sendlen]);
    sendlen += lenoffset;
    memcpy(&sendbuf[sendlen], cmd, strlen(cmd));
    sendlen += strlen(cmd);
    ret = appmain_send_syncmsg(MSG_APPAMIN_AC_SYSTEM_CMD, sendbuf, sendlen, recvbuf, sizeof(recvbuf), &recvlen);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("执行system命令失败 \n");
        return -1;
    }

    if(recvbuf[index++] != 1)
        return -1;

    memcpy_r(&result, &recvbuf[index], 4);
    if(result == 0)
        return 0;
    else
        return -1;
}

/**********************************************************************
* @name      : rate_set_sent_to_acmeter
* @brief     ：向交采模块发送费率设置
* @param[in] ：
* @return    ：
* @Date      ：2020-3-18
* @Update    :
**********************************************************************/
int rate_set_sent_to_acmeter(uint8 time)
{
    uint8  sendbuf[100] = {0};
    uint8  recvbuf[32] = {0};
    uint16 recLen = 0;
    uint16 offset = 0;
    int    ret = 0;
    int    nRet = 0;

    sendbuf[offset++] = time;

    ret = appmain_send_syncmsg(MSG_APPAMIN_AC_RATE_SET, sendbuf, offset, recvbuf, 32, &recLen);
    if ((ret != ERR_OK) || (recLen != 1) || (recvbuf[0] != 1))
    {
        PRTL_FMT_DEBUG("rate_set_sent_to_acmeter err! ret = %d, recLen = %d, recvbuf[0] = %x\n", ret, recLen, recvbuf[0]);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}

/**********************************************************************
* @name      : ipconfig_to_smiOS
* @brief     ：向系统管理器发送修改IP命令
* @param[in] ：
* @return    ：
* @Date      ：2020-3-18
* @Update    :
**********************************************************************/
int ipconfig_send_to_smiOS(char* Name, uint8 type, uint8* IP, uint8* NetMask, uint8* NetGw)
{
    uint8  sendbuf[100] = {0};
    uint8  recvbuf[32] = {0};
    uint16 recLen = 0;
    uint16 offset = 0;
    int    ret = 0;
    int    nRet = 0;
    int len = strlen(Name);

    PRTL_FMT_DEBUG("Set IP type %d\n", type);
    if(type > 1)
    {
        return -1;
    }
    
    sendbuf[offset++] = len;
    memcpy(sendbuf + offset, Name, len);
    offset += len;
    sendbuf[offset++] = type;  //新版本才有type
    if(1 == type)
    {
        /* IP */
        sendbuf[offset++] = 4;
        memcpy(sendbuf + offset, IP, 4);
        offset += 4;
    
        /* mask */
        sendbuf[offset++] = 4;
        memcpy(sendbuf + offset, NetMask, 4);
        offset += 4;
    
        /* getway */
        sendbuf[offset++] = 4;
        memcpy(sendbuf + offset, NetGw, 4);
        offset += 4;

        PRTL_BUF_DEBUG(sendbuf, offset, "Set IP: %d.%d.%d.%d:%d.%d.%d.%d, getway %d.%d.%d.%d\n",
                       IP[0], IP[1], IP[2], IP[3], NetMask[0], NetMask[1], NetMask[2], NetMask[3], 
                       NetGw[0], NetGw[1], NetGw[2], NetGw[3]);
    }

    ret = appmain_send_syncmsg(MSG_APPAMIN_SET_IPADDR, sendbuf, offset, recvbuf, 32, &recLen);
    if ((ret != ERR_OK) || (recLen != 1) || (recvbuf[0] != 1))
    {
        PRTL_FMT_DEBUG("ipconfig_to_smiOS err! ret = %d, recLen = %d, recvbuf[0] = %x\n", ret, recLen, recvbuf[0]);
        return ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
#ifdef AREA_JIANGSU
    if(0 == strcmp(Name, "FE0") && (0 == NetGw[0] && 0 == NetGw[1] && 0 == NetGw[2] && 0 == NetGw[3]))
    {
        comm_del_iproute_net("FE0", 0);
    }
#endif
    return nRet;
}

/**********************************************************************
* @name      : seek_meter_to_taskManager
* @brief     ：向采集调度发送搜表信号
* @param[in] ：
* @return    ：
* @Date      ：2020-3-18
* @Update    :
**********************************************************************/
int seek_meter_to_taskManager(uint16 time)
{
    uint8  sendbuf[100] = {0};
    uint8  recvbuf[32] = {0};
    uint16 recLen = 0;
    uint16 offset = 0;
    int    ret = 0;
    int    nRet = 0;

	//端口暂定为0xf2090201
	sendbuf[offset++] = 0xf2;
	sendbuf[offset++] = 0x09;
	sendbuf[offset++] = 0x02;
	sendbuf[offset++] = 0x01;

    sendbuf[offset++] = (uint8)((time >> 8) &0xff);
    sendbuf[offset++] = (uint8)(time & 0xff);

    ret = appmain_send_syncmsg(MSG_APPAMIN_CTS_SEEK_METER, sendbuf, offset, recvbuf, 32, &recLen);
    if ((ret != ERR_OK) || (recLen != 1) || (recvbuf[0] != 1))
    {
        PRTL_FMT_DEBUG("seek_meter_to_taskManager err! ret = %d, recLen = %d, recvbuf[0] = %x\n", ret, recLen, recvbuf[0]);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}

/*******************************************************************************
* 函数名称: trans_buf_to_taskManager 
* 函数功能: 通过mqtt向taskManager发送透传
* 输入参数: buf          发送数据内容
            len      发送的数据内容长度
            port     端口号
            comm     通讯字
            overtime 超时时间
            recvbuf  接受数据
            recLen   接受长度
* 输出参数: msgindex 消息序号
* 返 回 值:  >0成功 -1 失败
*******************************************************************************/
int trans_buf_to_taskManager(uint8* buf, uint16 len, uint32 port, uint8 baud, uint32 overtime, uint8 *recvbuf, 
    uint32 recLenMax, uint16 *recLen)
{
    int    ret;
    uint8  payload[4096]={0};
    uint16 index = 0;
    
    uint8  comm = 0; //通讯字
    
    //端口号
    memcpy_r(&payload[index], &port, 4);
    index += 4;
    
    //通讯字 默认1停止位 偶校验 8数据位
    comm |= 0x03;
    comm |= (1 << 3);
    comm |= (baud << 5);
    payload[index++] = comm;
    
    //帧超时时间
    memcpy_r(&payload[index], &overtime, 2);
    index += 2;
    
    //字符超时时间
    memcpy_r(&payload[index], &overtime, 2);
    index += 2;
    if (len < 0x80)
    {
        payload[index++] = len;
    }
    else
    {
        payload[index++] = 0x82;
        payload[index++] = (uint8)(len>>8);
        payload[index++] = (uint8)len;
    }
    memcpy(&payload[index], buf, len);
    index += len;

    ret = appmain_send_label_syncmsg(MSG_APPAMIN_CTS_READ_LIST, LABEL_HALT_TRANS, payload, index, recvbuf, recLenMax, recLen, overtime);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("同步透传接口失败 ret = %d, recLen = %d, recvbuf[0] = %x\n", ret, *recLen, recvbuf[0]);
    }

    return ret;

}

/*******************************************************************************
* 函数名称: trans_buf_send_to_taskManager 
* 函数功能: 通过mqtt向taskManager发送透传(仅发送)
* 输入参数: buf          发送数据内容
            len      发送的数据内容长度
            port     端口号
            comm     通讯字
            overtime 超时时间
            recvbuf  接受数据
            recLen   接受长度
* 输出参数: msgindex 消息序号
* 返 回 值:  >0成功 -1 失败
*******************************************************************************/
int trans_buf_send_to_taskManager(uint16 label, uint8* buf, uint16 len, uint32 port, uint8 baud, uint32 overtime)
{
    int    ret;
    uint8  payload[4096]={0};
    uint16 index = 0;
    
    uint8  comm = 0; //通讯字
    
    //端口号
    memcpy_r(&payload[index], &port, 4);
    index += 4;
    
    //通讯字 默认1停止位 偶校验 8数据位
    comm |= 0x03;
    comm |= (1 << 3);
    comm |= (baud << 5);
    payload[index++] = comm;
    
    //帧超时时间
    memcpy_r(&payload[index], &overtime, 2);
    index += 2;
    
    //字符超时时间
    memcpy_r(&payload[index], &overtime, 2);
    index += 2;
    if (len < 0x80)
    {
        payload[index++] = len;
    }
    else
    {
        payload[index++] = 0x82;
        payload[index++] = (uint8)(len>>8);
        payload[index++] = (uint8)len;
    }
    memcpy(&payload[index], buf, len);
    index += len;

    ret = appmain_send_msg(MSG_APPAMIN_CTS_READ_LIST, label, payload, index);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("异步发送接口失败 ret = %d\n", ret);
    }

    return ret;
}

/**********************************************************************
* @name      : appmain_refresh_status_bar
* @brief     ：升级进度显示
* @param[in] ：progress 升级进度
			   time     显示时间长度(s)
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appmain_refresh_status_bar(uint32 time, char *str)
{
	int32  ret = -1;
	uint8  buf[128] = {0};
	uint32 offset = 0;

	if (str == NULL)
	{
		return -1;
	}

	if (strlen(str) > 100)
	{
		return -2;
	}

	memcpy_r(&buf[offset], &time, sizeof(time));
	offset += 4;

	buf[offset++] = strlen(str);
	memcpy(&buf[offset], str, strlen(str) + 1);
	offset += strlen(str) + 1;

	ret = comm_send_msg(MSG_COMM_GUISTATUS, buf, offset);
	if(0 != ret)
	{
		COMM_FMT_DEBUG("Send gui \"%s\" failed\n", buf);
	}

	return ret;
}

/**********************************************************************
* @name      : appmain_get_ppp_mode
* @brief     ：无线拨号查询模块信息
* @param[in] ：uint8 no       模块id 1-2
               uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：消息长度
* @Date      ：2020-5-29
* @Update    :
**********************************************************************/
uint16 appmain_get_ppp_mode(uint8 no, uint8* recvbuf, uint16 buflen)
{
    uint16 recLen = 0;
    int    ret = 0;

    ret = appmain_send_syncmsg(MSG_APPAMIN_GETPPPMODE, &no, sizeof(no), recvbuf, buflen, &recLen);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_FMT_DEBUG("appmain_get_ppp_mode err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
    
    return recLen;
}

/**********************************************************************
* @name      : appmain_get_ppp_diag
* @brief     ：无线拨号查询拨号信息
* @param[in] ：uint8 no       模块id 1-2
               uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：消息长度
* @Date      ：2020-5-29
* @Update    :
**********************************************************************/
uint16 appmain_get_ppp_diag(uint8 no, uint8* recvbuf, uint16 buflen)
{
    uint16 recLen = 0;
    int    ret = 0;

    ret = appmain_send_syncmsg(MSG_APPAMIN_GETPPPDIAG, &no, sizeof(no), recvbuf, buflen, &recLen);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_FMT_DEBUG("appmain_get_ppp_diag err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
    
    return recLen;
}

/**********************************************************************
* @name      : appmain_get_ppp_gps
* @brief     ：无线拨号查询位置信息
* @param[in] ：uint8 no       模块id 1-2
               uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：消息长度
* @Date      ：2020-5-29
* @Update    :
**********************************************************************/
uint16 appmain_get_ppp_gps(uint8 no, uint8* recvbuf, uint16 buflen)
{
    uint16 recLen = 0;
    int    ret = 0;

    ret = appmain_send_syncmsg(MSG_APPAMIN_GETPPPGPS, &no, sizeof(no), recvbuf, buflen, &recLen);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_FMT_DEBUG("appmain_get_ppp_gps err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
    
    return recLen;
}

/**********************************************************************
* @name      : appmain_trans_AT
* @brief     ：无线拨号AT指令透传
* @param[in] ：uint8 no        模块id 1-2
               char *sendAT   发送内容
               uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：消息长度
* @Date      ：2022-8-19
* @Update    :
**********************************************************************/
uint16 appmain_trans_AT_with_outtime(uint8 no, char *sendAT, uint8* recvbuf, uint16 buflen,uint16 outtime)
{
    uint16 recLen = 0;
    int    ret = 0;
    uint8  sendbuf[1024] = {0};
    uint16 sendlen = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;

    sendbuf[sendlen++] = no;
    SET_LEN_OFFSET(strlen(sendAT), byte, bytenum);
    memcpy(sendbuf + sendlen, byte, bytenum);
    sendlen += bytenum;
    memcpy(sendbuf + sendlen, sendAT, strlen(sendAT));
    sendlen += strlen(sendAT);
    
    ret = appmain_send_syncmsg_with_outtime(MSG_APPMAIN_TRANSPORT, sendbuf, sendlen, recvbuf, buflen, &recLen,outtime);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_FMT_DEBUG("appmain_trans_at err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
    
    return recLen;
}

/**********************************************************************
* @name      : appmain_trans_AT
* @brief     ：无线拨号AT指令透传
* @param[in] ：uint8 no        模块id 1-2
               char *sendAT   发送内容
               uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：消息长度
* @Date      ：2022-8-19
* @Update    :
**********************************************************************/
uint16 appmain_trans_AT(uint8 no, char *sendAT, uint8* recvbuf, uint16 buflen)
{
    uint16 recLen = 0;
    int    ret = 0;
    uint8  sendbuf[1024] = {0};
    uint16 sendlen = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;

    sendbuf[sendlen++] = no;
    SET_LEN_OFFSET(strlen(sendAT), byte, bytenum);
    memcpy(sendbuf + sendlen, byte, bytenum);
    sendlen += bytenum;
    memcpy(sendbuf + sendlen, sendAT, strlen(sendAT));
    sendlen += strlen(sendAT);
    
    ret = appmain_send_syncmsg(MSG_APPMAIN_TRANSPORT, sendbuf, sendlen, recvbuf, buflen, &recLen);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_FMT_DEBUG("appmain_trans_at err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
    
    return recLen;
}


/**********************************************************************
* @name      : appmain_set_gps_tempposition
* @brief     ：设置临时位置，送检使用
* @param[in] ：OOP_GEOGADD_A_T *ptGps
* @param[out]：
* @return    ：
* @Date      ：
* @Update    :
**********************************************************************/
void appmain_set_gps_tempposition(OOP_GEOGADD_A_T *ptGps)
{
    memcpy(&g_tNewOOPGPS, ptGps, sizeof(OOP_GEOGADD_A_T));
}

/**********************************************************************
* @name      : appmain_set_gps_tempposition
* @brief     ：设置临时位置，送检使用
* @param[in] ：
* @param[out]：OOP_GEOGADD_A_T *ptGps
* @return    ：0-成功/错误码
* @Date      ：
* @Update    :
**********************************************************************/
int appmain_get_gps_tempposition(OOP_GEOGADD_A_T *ptGps)
{
    if(1 != g_tNewOOPGPS.type)
    {
        return -1;
    }

    memcpy(ptGps, &g_tNewOOPGPS, sizeof(OOP_GEOGADD_A_T));
    return 0;
}

/**********************************************************************
* @name      : appmain_get_map_slot
* @brief     ：查询模组槽位信息
* @param[in] ：type       模块型号
               uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：消息长度
* @Date      ：2020-5-30
* @Update    :
**********************************************************************/
uint16 appmain_get_map_slot(const char *type, uint8* recvbuf, uint16 buflen)
{
    uint8 sendbuf[128] = {0};
    uint8 size = 0;
    int   ret = 0;
    uint16 recLen = 0;
    
    size = strlen(type);
    sendbuf[0] = size;
    memcpy(sendbuf + 1, type, size);

    ret = appmain_send_syncmsg(MSG_APPAMIN_GETMAPSLOT, sendbuf, size + 1, recvbuf, buflen, &recLen);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_BUF_DEBUG(sendbuf, size + 1, "appmain_get_map_slot err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
    
    return recLen;
}


/**********************************************************************
* @name      : appmain_get_map_info
* @brief     ：查询模组信息
* @param[in] ：uint8 no       模块id 1-5
               uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：消息长度
* @Date      ：2020-5-30
* @Update    :
**********************************************************************/
uint16 appmain_get_map_info(uint32 no, uint8* recvbuf, uint16 buflen)
{
    uint8  sendbuf[4] = {0};
    uint16 recLen = 0;
    int    ret = 0;

    if(no > 5 || 0 == no)
    {
        PRTL_FMT_DEBUG("appmain_get_map_info err! no = %d\n", no);
        return 0;
    }
    memcpy_r(sendbuf, &no, sizeof(uint32));
    
    ret = appmain_send_syncmsg(MSG_APPAMIN_GETMAPINFO, sendbuf, 4, recvbuf, buflen, &recLen);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_BUF_DEBUG(sendbuf, 4, "appmain_get_map_info err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
    
    return recLen;
}

/**********************************************************************
* @name      : appmain_get_map_all
* @brief     ：查询模组所有信息
* @param[in] ：uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：消息长度
* @Date      ：2020-5-30
* @Update    :
**********************************************************************/
uint16 appmain_get_map_all(uint8* recvbuf, uint16 buflen)
{
    uint16 recLen = 0;
    int    ret = 0;

    ret = appmain_send_syncmsg(MSG_APPAMIN_GETMAPALL, NULL, 0, recvbuf, buflen, &recLen);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_FMT_DEBUG("appmain_get_map_info err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
    
    return recLen;
}

/**********************************************************************
* @name      : appmain_get_deviceinfo
* @brief     ：003A获取设备信息
* @param[in] ：
* @param[out]：deviceinfo 设备信息
* @return    ：0 获取失败 1 获取成功
* @Date      ：
* @Update    :
**********************************************************************/
uint16 appmain_get_deviceinfo(OOP_DEVICERUNSTAT_T *deviceinfo)
{
    uint16 recLen = 0;
    int    ret = 0;
    uint8 recvbuf[1000]={0};

	memset(deviceinfo, 0, sizeof(OOP_DEVICERUNSTAT_T));
	
    ret = appmain_send_syncmsg(MSG_APPAMIN_GET_DEVICEINFO, NULL, 0, recvbuf, sizeof(recvbuf), &recLen);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_FMT_DEBUG("appmain_get_map_info err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
    appmsg_deviceinfo_split(recvbuf,deviceinfo);

	ret = appmain_send_syncmsg(MSG_APPAMIN_GET_CUR_DEVICEINFO, NULL, 0, recvbuf, sizeof(recvbuf), &recLen);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_FMT_DEBUG("appmain_get_map_info err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
	appmsg_deviceinfo_run_split(recvbuf,deviceinfo);

	//温度
	DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
	if(dev != NULL)
	{
        ret = dev->devinfo_get_cpu_temperature(dev);
		if(ret > 0)
		{
			deviceinfo->tempBroad = (int16)ret;
		}
		else
		{
			PRTL_FMT_DEBUG("hal devinfo_get_cpu_temperature ret = %d\n", ret);
		}
	    
		hal_device_release((HW_DEVICE *)dev);
	}
	else
	{
		PRTL_FMT_DEBUG("hal hal_device_get ret NULL\n");
	}
	
    return 1;
}


/**********************************************************************
* @name      : appmain_get_deviceinfo
* @brief     ：003A获取设备信息
* @param[in] ：
* @param[out]：deviceinfo 设备信息
* @return    ：0 获取失败 1 获取成功
* @Date      ：
* @Update    :
**********************************************************************/
uint16 appmain_get_radio_param(RADIO_PARAM_T *radioparam)
{
    uint16 recLen = 0;
    int    ret = 0;
    uint8 recvbuf[1000]={0};

	memset(radioparam, 0, sizeof(RADIO_PARAM_T));
	
    ret = appmain_send_syncmsg(MSG_APPMAIN_GET_RADIOCFG, NULL, 0, recvbuf, sizeof(recvbuf), &recLen);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_FMT_DEBUG("appmain_get_map_info err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
    appmsg_radioparam_split(recvbuf,radioparam);

    return 1;
}

/**********************************************************************
* @name      : appmain_manager_app
* @brief     ：APP管理
* @param[in] ：uint8 type     类型(0-安装; 1-升级; 2-卸载; 3-启动; 4-停止;)
               char *appname  app名称
* @param[out]：
* @return    ：0-成功/错误码
* @Date      ：
* @Update    : 2021-3-18
**********************************************************************/
int appmain_manager_app(uint8 type, char *appname)
{
    int    ret = 0;
    uint8  sendbuf[256] = {0};
    uint16 msglen = 0;
    uint8  recvbuf[4] = {0};
    uint16 recLen = 0;

    sendbuf[msglen++] = type;
    sendbuf[msglen++] = strlen(appname);
    memcpy(sendbuf + msglen, appname, strlen(appname));
    msglen += strlen(appname);
    ret = appmain_send_syncmsg_time(MSG_APPMAIN_MANAGER_APP, sendbuf, msglen, recvbuf, sizeof(recvbuf), &recLen, 25);
    if(0 != ret)
    {
        return ret;
    }

    return 0 == recvbuf[0] ? -1 : 0;
}

/**********************************************************************
* @name      : appmain_act_checkmeter
* @brief     ：蓝牙检定参数
* @param[in] ：check    参数
* @param[out]：
* @return    ：0-成功/错误码
* @Date      ：
* @Update    : 2021-3-18
**********************************************************************/
int appmain_act_ble_checkmeter(OOPM_BLUETOOTHERRCHK_T *data)
{
    int    i = 0;
    int    ret = 0;
    uint8  sendbuf[256] = {0};
    uint16 msglen = 0;
    uint8  recvbuf[4] = {0};
    uint16 recLen = 0;

    BLE_CHECK_T check;
    
    memset(&check, 0, sizeof(check));

    if (data == NULL)
    {
        return ERR_PTR;
    }

    check.type = data->nType;
    check.interval = data->interval;
    check.power = data->power;
    memcpy(check.MAC, data->addr.value, 6);
    check.ch_num = data->nNum;
    for (i = 0; i < check.ch_num; i++)
    {
        check.freq[i] = data->nFreq[i];
    }

    msglen = ble_check_package(&check, sendbuf, 256);
    if (msglen <= 0)
    {
        return ERR_NORMAL;
    }

    ret = appmain_send_syncmsg(MSG_APPMAIN_SET_BTCHECK, sendbuf, msglen, recvbuf, sizeof(recvbuf), &recLen);
    if (0 != ret)
    {
        return ret;
    }

    return 0 == recvbuf[0] ? -1 : 0;
}

/**********************************************************************
* @name      : appmain_get_bluetooth_cfg
* @brief     ：获取蓝牙参数配置
* @param[in] ：uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：0>成功/错误码
* @Date      ：2021-11-25
* @Update    :
**********************************************************************/
int appmain_get_bluetooth_cfg(uint8* recvbuf, uint16 buflen)
{
    int ret = 0;
    uint16 recLen = 0;
    ret = appmain_send_syncmsg(MSG_APPMAIN_GET_BTCFG, NULL, 0, recvbuf, buflen, &recLen);
    if(0 != ret)
    {
        PRTL_FMT_DEBUG("appmain_get_bluetooth_cfg failed, ret %d\n", ret);
        recLen = 0;
    }
    return (int)recLen;
}

/**********************************************************************
* @name      : appmain_get_bluetooth_connect
* @brief     ：获取蓝牙连接信息
* @param[in] ：uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：0>成功/错误码
* @Date      ：2021-11-25
* @Update    :
**********************************************************************/
int appmain_get_bluetooth_connect(uint8* recvbuf, uint16 buflen)
{
    uint16 recLen = 0;
    int ret = appmain_send_syncmsg(MSG_APPMAIN_GET_BTCONNECT, NULL, 0, recvbuf, buflen, &recLen);
    if(0 != ret)
    {
        PRTL_FMT_DEBUG("appmain_get_bluetooth_connect failed, ret %d\n", ret);
        recLen = 0;
    }
    return (int)recLen;
}

/**********************************************************************
* @name      : appmain_get_bluetooth_connect
* @brief     ：获取蓝牙连接信息
* @param[in] ：uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：0>成功/错误码
* @Date      ：2021-11-25
* @Update    :
**********************************************************************/
int appmain_get_bluetooth_sn(uint8* recvbuf, uint16 buflen)
{
    uint16 recLen = 0;
    int ret = appmain_send_syncmsg(MSG_APPMAIN_GET_BT_SN, NULL, 0, recvbuf, buflen, &recLen);
    if(0 != ret)
    {
        PRTL_FMT_DEBUG("appmain_get_bluetooth_connect failed, ret %d\n", ret);
        recLen = 0;
    }
    return (int)recLen;
}


/**********************************************************************
* @name      : appmain_set_bluetooth_sn
* @brief     ：设置蓝牙SN
* @param[in] ：uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：0>成功/错误码
* @Date      ：2021-11-25
* @Update    :
**********************************************************************/
int appmain_set_bluetooth_sn(uint8 *sendbuf, uint16 sendlen, uint8* recvbuf, uint16 buflen, uint16 *recvlen)
{
    int ret = appmain_send_syncmsg(MSG_APPMAIN_SET_BT_SN, sendbuf, sendlen, recvbuf, buflen, recvlen);
    if(0 != ret)
    {
        PRTL_FMT_DEBUG("appmain_set_bluetooth_cfg failed, ret %d\n", ret);
        *recvlen = 0;
    }
    return ret;
}

/**********************************************************************
* @name      : appmain_set_bluetooth_cfg
* @brief     ：设置蓝牙参数配置
* @param[in] ：uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：0>成功/错误码
* @Date      ：2021-11-25
* @Update    :
**********************************************************************/
int appmain_set_bluetooth_cfg(uint8 *sendbuf, uint16 sendlen, uint8* recvbuf, uint16 buflen, uint16 *recvlen)
{
    int ret = appmain_send_syncmsg(MSG_APPMAIN_SET_BTCFG, sendbuf, sendlen, recvbuf, buflen, recvlen);
    if(0 != ret)
    {
        PRTL_FMT_DEBUG("appmain_set_bluetooth_cfg failed, ret %d\n", ret);
        *recvlen = 0;
    }
    return ret;
}

/**********************************************************************
* @name      : appmain_set_bluetooth_check
* @brief     ：设置蓝牙检表配置
* @param[in] ：uint8 *sendbuf 内容
* @param[out]：uint16 sendlen 长度
* @return    ：0>成功/错误码
* @Date      ：2022-3-1
* @Update    :
**********************************************************************/
int appmain_set_bluetooth_check(uint8 *sendbuf, uint16 sendlen)
{
    uint8 recvbuf[8] = {0};
    uint16 recvlen = 0;
    int ret = appmain_send_syncmsg(MSG_APPMAIN_SET_BTCHECK, sendbuf, sendlen, recvbuf, 8, &recvlen);
    if(0 != ret || 1 != recvlen || FALSE == recvbuf[0])
    {
        PRTL_FMT_DEBUG("appmain_set_bluetooth_check failed, ret %d %d\n", ret, recvbuf[0]);
        ret = -1;
    }
    return ret;
}

/**********************************************************************
* @name      : appmain_set_flow_param
* @brief     ：配置流量超限参数
* @param[in] ：monLimit   月累计流量限值
               flowLimit  流速限值
* @param[out]：
* @return    ：0-成功/错误码
* @Date      ：
* @Update    : 2021-3-18
**********************************************************************/
int appmain_set_flow_param(uint32 monLimit, uint32 flowLimit, uint32 time)
{
    int    ret = 0;
    uint8  sendbuf[256] = {0};
    uint16 msglen = 0;
    uint8  recvbuf[4] = {0};
    uint16 recLen = 0;

    //if((monLimit != 0)&&(flowLimit !=0))
    {
        memcpy_r(&sendbuf[msglen], &monLimit, sizeof(uint32));
        msglen += sizeof(uint32);
        memcpy_r(&sendbuf[msglen], &flowLimit, sizeof(uint32));
        msglen += sizeof(uint32);

        ret = appmain_send_syncmsg(MSG_APPAMIN_FLOW_PARAM_SET, sendbuf, msglen, recvbuf, sizeof(recvbuf), &recLen);
        if (0 != ret)
        {
            return ret;
        }
    }
    
    msglen = 0;

    //数量
    sendbuf[msglen ++] = 0x01;

    //SSH服务
    sendbuf[msglen ++] = 0x01;

    //时间
    memcpy_r(&sendbuf[msglen], &time, sizeof(uint32));
    msglen += sizeof(uint32);

    ret = appmain_send_syncmsg(MSG_APPAMIN_TIME_OUT_SET, sendbuf, msglen, recvbuf, sizeof(recvbuf), &recLen);
    if (0 != ret)
    {
        return ret;
    }

    return 0 == recvbuf[0] ? -1 : 0;
}

/**********************************************************************
* @name      : appmain_get_flow_param
* @brief     ：查询流量超限参数
* @param[in] ：monLimit   月累计流量限值
               flowLimit  流速限值
* @param[out]：
* @return    ：0-成功/错误码
* @Date      ：
* @Update    : 2021-3-18
**********************************************************************/
int appmain_get_flow_param(uint32 *monLimit, uint32 *flowLimit, uint32 *time)
{
    int    ret = 0;
    uint8  sendbuf[8] = {0};
    uint16 msglen = 0;
    uint8  recvbuf[256] = {0};
    uint16 recLen = 0;

    if ((monLimit == NULL) || (flowLimit == NULL))
    {
        return ERR_PTR;
    }
    
    ret = appmain_send_syncmsg(MSG_APPAMIN_FLOW_PARAM_GET, sendbuf, msglen, recvbuf, sizeof(recvbuf), &recLen);
    if (0 != ret)
    {
        return ret;
    }

    if (recLen != 12)
    {
        return ERR_NORMAL;
    }

    memcpy_r(monLimit, recvbuf, 4);
    memcpy_r(flowLimit, &recvbuf[4], 4);

    return ERR_OK;
}

#if DESC("文件代发", 1)
/**********************************************************************
* @name      : appmain_send_proxy_to_taskManager
* @brief     ：文件代发给taskManager
* @param[in] ：uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：0>成功/错误码
* @Date      ：2021-11-25
* @Update    :
**********************************************************************/
int appmain_send_proxy_to_taskManager(uint8 *sendbuf, uint16 sendlen, uint8* recvbuf, uint16 buflen, 
    uint16 *recvlen)
{
    int ret = appmain_send_syncmsg(MSG_APPAMIN_CTS_CARRIRT_TRANS, sendbuf, sendlen, recvbuf, buflen, recvlen);
    if(0 != ret)
    {
        PRTL_FMT_DEBUG("appmain_set_bluetooth_cfg failed, ret %d\n", ret);
        *recvlen = 0;
    }
    return ret;
}
#endif

/**********************************************************************
* @name      : appmain_set_radio_speak
* @brief     ：设置电台通话
* @param[in] ：type 0 关闭 1打开
               
* @param[out]：uint8* recvbuf 缓存
* @return    ：消息长度
* @Date      ：
* @Update    :
**********************************************************************/
uint16 appmain_set_radio_speak(uint8 type, uint8* recvbuf, uint16 buflen)
{
    uint8 sendbuf[128] = {0};
    int   ret = 0;
    uint16 recLen = 0;
    uint8 status;
    //电台里1关闭 2打开
    if(type == 0)
        status = 1;
    else
        status = 2;
    sendbuf[0] = status;

    ret = appmain_send_syncmsg(MSG_APPMAIN_SET_RADIOSPEAK, sendbuf,1, recvbuf, buflen, &recLen);
    if (ret != ERR_OK || 0 == recLen)
    {
        PRTL_BUF_DEBUG(sendbuf, 1, "appmain_set_radio_speak err! ret = %d, recLen = %d\n", ret, recLen);
        return 0;
    }
    
    return recLen;
}

