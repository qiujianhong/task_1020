/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级APP对外通信模块
* @date：    2020-3-4
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "db_api.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "appmsg_split.h"
    
#include "msg_base.h"
#include "task_proc.h"
#include "mqtt_proc.h"
#include "db_update.h"
#include "gui_c_prtl.h"

    
/**********************************************************************
* @name      : task_msg_entry
* @brief     ：启动task对内消息任务
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-1-2
* @Update    :
**********************************************************************/
void *task_msg_entry(void *pArg)
{
    int         taskfd = 0;
    uint8       recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;
    int         recvlen = 0;
    uint32      sender = 0;
    int         ret = 0;
    
    taskfd = main_task_init();
    if(taskfd <= 0)
    {
        PDMSG_FMT_DEBUG("appmain task_msg_init error!\n");
        sleep(1);
        exit(0); //进程退出
    }

    /* 循环收消息 */
    for(;;)
    {
        recvlen = task_recv_msg(taskfd, recvBuf, TASK_MSG_MAX_LEN, &sender);
        if(recvlen > 0)
        {
            switch(pMsg->msg_id)
            {
                case MSG_COMM_GETDIAL:
                {
                    ret = advPd_send_wirelessmsg(pMsg->label, WIRELESS_IOP_GET_DIAGINFO, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        PDMSG_FMT_TRACE("Send wireless get dial msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_SETAPN:
                {
                    ret = advPd_send_wirelessmsg(pMsg->label, WIRELESS_IOP_SET_DIAGARG, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        PDMSG_FMT_TRACE("Send wireless msg sent apn error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_GETETH:
                {
                    ret = advPd_send_smios(pMsg->label, SYSTEM_IOP_GET_ETHSTATUS, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_TRACE("Send smios msg get %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_COMM_ADDROUTE:
                {
                    /* 使用同步消息标记 */
                    ret = advPd_send_smios(pMsg->syncindex, SYSTEM_IOP_ADD_ROUTE, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        PDMSG_FMT_TRACE("Send smios msg add route, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_DELROUTE:
                {
                    /* 使用同步消息标记 */
                    ret = advPd_send_smios(pMsg->syncindex, SYSTEM_IOP_DEL_ROUTE, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        PDMSG_FMT_TRACE("Send smios msg del route, code %d\n", ret);
                    }
                    break;
                }
                case MSG_PULSE_GET_RMT:
                {
                    ret = advPd_send_rspmsg(pMsg->label, RSPSAMPLE_IOP_RMT_STATE, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        PDMSG_FMT_TRACE("Send rsp get rmt msg add route, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_REBOOT:
                {
                    /* 使用同步消息标记 */
                    ret = advPd_send_smios(pMsg->syncindex, SYSTEM_IOP_REQUIRE_RESTART, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        PDMSG_FMT_TRACE("Send smios msg reboot, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_RCT://时钟同步
                {
                    /* 使用同步消息标记 */
                    ret = advPd_send_smios(pMsg->syncindex, SYSTEM_IOP_RCT, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        PDMSG_FMT_TRACE("Send smios msg syn time, pMsg->msg_id=%d, code %d\n", pMsg->label, ret);
                    }
                    break;
                }

                default:
                {
                    break;
                }
            }
        }
    }
}

/**********************************************************************
* @name      : out_msg_entry
* @brief     ：启动对外通信消息任务
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
void *out_msg_entry(void *pArg)
{
    uint8       recvBuf[RECV_BUF_LEN] = {0};
    int         recvlen = 0;
    int         ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8       appstatus = ADVPD_STATUS_INIT;
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    int             dbagent = advPd_get_dbagent();

    /* 先发送一次APP注册消息 */
    ret = advPd_login_to_smios();
    if(0 != ret)
    {
        PDMSG_FMT_TRACE("advance_login_to_smios failed %d\n", ret);
    }
    
    /* 循环收消息 */
    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    for(;;)
    {
        recvlen = advPd_recv_msg(recvBuf, RECV_BUF_LEN);
        if(recvlen > 0)
        {
            ret = appmsg_split(recvBuf, (uint16)recvlen , &pMsgInfo);
            if(NULL == pMsgInfo)
            {
                PDMSG_BUF_TRACE(recvBuf, recvlen, "Recv msg:");
            }
            else
            {
                PDMSG_BUF_TRACE(recvBuf, recvlen, "Recv msg from %s:", pMsgInfo->souraddr);
                if(0 == strcmp(pMsgInfo->souraddr, ADVPD_APP_NAME))
                {
                    PDMSG_FMT_DEBUG("Revc msg form %s is the same name, iid: %d, iop %d\n", pMsgInfo->souraddr, pMsgInfo->IID, pMsgInfo->IOP);
                    goto goon;
                }
            }
            if(0 == ret && NULL != pMsgInfo)
            {
                /* 注册应答 */
                if(MESSAGE_IID_SYSTEM == pMsgInfo->IID && SYSTEM_IOP_APP_REGISTER == pMsgInfo->IOP)
                {
                    if(/*TRUE == *pMsgInfo->playload && */ADVPD_STATUS_INIT == appstatus)
                    {
                        appstatus = ADVPD_STATUS_SEEK;
                        printf("=======advance login to OS=======\n");
                        PDMSG_FMT_DEBUG("=======advance login to OS=======\n");
                        ret = advPd_seek_app();  //查询注册的APP
                        if(0 != ret)
                        {
                            printf("advance_seek_app failed %d\n", ret);
                        }
                    }
                    goto goon;
                }
                
                /* 心跳管理 */
                if(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_HERTBEAT == pMsgInfo->IOP)
                {
                    ret = advPd_heart_bear(pMsgInfo);
                    if(0 != ret)
                    {
                        PDMSG_FMT_DEBUG("advance send heart failed %d\n", ret);
                    }
                    goto goon;
                }

                /* 查询版本号 */
                if(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_VERSION == pMsgInfo->IOP)
                {
                    ret = advPd_get_version(pMsgInfo);
                    if(0 != ret)
                    {
                        PDMSG_FMT_DEBUG("advance get version failed %d\n", ret);
                    }
                    goto goon;
                }
                /* 查询appid */
                if(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_GETAPPID == pMsgInfo->IOP)
                {
                    ret = advPd_get_appidmsg(pMsgInfo);
                    if(0 != ret)
                    {
                        PDMSG_FMT_DEBUG("advance get version failed %d\n", ret);
                    }
                    goto goon;
                }

                /* 处于初始化状态后续操作停止 */
                if(ADVPD_STATUS_INIT == appstatus)
                {
                    goto goon;
                }

                /* APP查询返回, 全部关注的接口都返回, 则结束查询 */
                if(MESSAGE_IID_SYSTEM == pMsgInfo->IID && SYSTEM_IOP_GET_REGISTERAPP == pMsgInfo->IOP)
                {
                    if(TRUE == advPd_recv_regapp(pMsgInfo))
                    {
                        appstatus = ADVPD_STATUS_WORK;
                    }
                    goto goon;
                }
                
                /* 数据中心更新 */
                ret = db_update_msg_proc(pMsgInfo);
                if(DB_AGENT_UNKONW_MSG != ret)
                {
                    goto goon;
                }

                /* 数据中心 */
                ret = db_agent_msg_entry(dbagent, pMsgInfo);
                if(DB_AGENT_UNKONW_MSG != ret)
                {
                    if(0 != ret)
                    {
                        PDMSG_BUF_DEBUG(recvBuf, recvlen, "Drop msg from %s, ret %d:", pMsgInfo->souraddr, ret);
                    }
                    goto goon;
                }

                /* 桌面显示 */
                if(pMsgInfo->IID == MESSAGE_IID_DESKGUI)
                {
				    appGUI_recv_back_handle(pMsgInfo);
				    goto goon;
                }

                /* 采集任务调度 + 无线拨号 + 模组 */
                ret = task_recv_mqttmsg(pMsgInfo);
                if(TASK_UNKNOW_MSG != ret)
                {
                    goto goon;
                }
goon:
                free(pMsgInfo);
                pMsgInfo = NULL;
            }
        }

        /* 查询其他APP信息 */
        if(ADVPD_STATUS_WORK != appstatus)
        {
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            if(curtime.tv_sec - oldtime.tv_sec > 5)  //5s发送一次消息
            {
                if(ADVPD_STATUS_INIT == appstatus)
                {
                    ret = advPd_login_to_smios();
                    if(0 != ret)
                    {
                        PDMSG_FMT_DEBUG("advance_login_to_smios failed %d\n", ret);
                    }
                }
                else if(ADVPD_STATUS_SEEK == appstatus)
                {
                    ret = advPd_seek_app();
                    if(0 != ret)
                    {
                        PDMSG_FMT_DEBUG("advance_seek_app failed %d\n", ret);
                    }
                }
                oldtime = curtime;
            }
        }
        //usleep(10);
    }
}


/**********************************************************************
* @name      : msg_task_start
* @brief     ：启动消息task任务
* @param[in] ：uint32 ip
               uint16 port
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-24
* @Update    :
**********************************************************************/
int msg_task_start(uint32 ip, uint16 port)
{
    int ret = 0;
    pthread_t taskid = 0;
    pthread_t updateid = 0;
    pthread_t outmsgid = 0;

    if(0 != advPd_mqtt_init(ip, port))
    {
        printf("Main init MQTT error!\n");
        return -1;
    }

    ret = pthread_create(&taskid, NULL, task_msg_entry, NULL);
    if (ret != 0)
    {
        /* 关闭线程 */
        printf("Msg Task thread Create error! err:%d\n", ret);
        return -1;
    }

    /* update msg线程启动 */
    db_updata_init();
    ret = pthread_create(&updateid, NULL, db_update_entry, NULL);
    if (ret != 0)
    {
        pthread_cancel(taskid);
        pthread_join(taskid, NULL);
        printf("Msg Udp update thread Create error! err:%d\n", ret);
        return -1;
    }

    /* out msg线程启动 */
    ret = pthread_create(&outmsgid, NULL, out_msg_entry, NULL);
    if (ret != 0)
    {
        pthread_cancel(taskid);
        pthread_join(taskid, NULL);
        pthread_cancel(updateid);
        pthread_join(updateid, NULL);
        advPd_outmsg_uninit();
        printf("Msg out thread Create error! err:%d\n", ret);
        return -1;
    }

    printf("Main start MQTT msg task!\n");

    return 0;
}

