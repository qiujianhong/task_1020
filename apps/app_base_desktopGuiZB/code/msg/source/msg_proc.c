/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级APP对外通信模块
* @date：    2019-12-24
* @history： 
*********************************************************************
*/
//#include "baseCommon.h"
#include "gui_c_prtl.h"
#include "db_api.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "appmsg_split.h"

#include "msg_base.h"
#include "task_proc.h"
#include "out_msg.h"
#include "label_id.h"
#include "commfun.h"
#include "desk_meter.h"
#ifdef AREA_CHONGQING
#include "chongqing.h"
#endif
extern  DB_AGENT   g_dbAgent;
extern uint32 gethstatus;
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
    uint8       recvBuf[APP_RECV_BUF_LEN] = {0};
    int         recvlen = 0;
    int         ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8       appstatus = GUI_STATUS_INIT;  //正式代码改为 GUI_STATUS_INIT
//    int         dbagent = gui_get_dbagent();
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    uint8 devnamelen=0;
    /* 先发送一次APP注册消息 */
    ret = gui_login_to_smios();
    if(0 != ret)
    {
        GUI_FMT_TRACE("gui_login_to_smios failed %d\n", ret);
    }

    /* 循环收消息 */
    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    for(;;)
    {
        recvlen = gui_recv_msg(recvBuf, APP_RECV_BUF_LEN);
        if(recvlen > 0)
        {
            ret = appmsg_split(recvBuf, (uint16)recvlen , &pMsgInfo);
            if(NULL != pMsgInfo)
            {
                GUI_BUF_TRACE(recvBuf, recvlen, "Recv msg from %s:", pMsgInfo->souraddr);
            }
            else
            {
                GUI_BUF_TRACE(recvBuf, recvlen, "Recv msg:");
            }
            if(0 == ret && NULL != pMsgInfo)
            {
                /* 注册应答 */
                if(MESSAGE_IID_SYSTEM == pMsgInfo->IID && SYSTEM_IOP_APP_REGISTER == pMsgInfo->IOP)
                {
                    if(TRUE == *pMsgInfo->playload)
                    {
                        appstatus = GUI_STATUS_WORK;
//                        appstatus = GUI_STATUS_SEEK;
                        GUI_FMT_DEBUG("=======gui login to OS=======\n");
                        ret = gui_seek_app();  //查询注册的APP
                        if(0 != ret)
                        {
                            GUI_FMT_TRACE("gui_seek_app failed %d\n", ret);
                        }
                    }
                    goto goon;
                }

                /* 获取以太网状态 */
                if(MESSAGE_IID_SYSTEM == pMsgInfo->IID && SYSTEM_IOP_GET_ETHSTATUS == pMsgInfo->IOP)
                {
                    memcpy_r((uint8*)&gethstatus,pMsgInfo->playload,4);
                    GUI_FMT_DEBUG("gethstatus %d\n", gethstatus);
                    goto goon;
                }
                /* 获取以太网状态 */
                if(MESSAGE_IID_SYSTEM == pMsgInfo->IID && SYSTEM_IOP_EVENT_ETHSTATUS == pMsgInfo->IOP)
                {
                    devnamelen = pMsgInfo->playload[0];
                    gethstatus = pMsgInfo->playload[devnamelen+1];
                    GUI_FMT_DEBUG("gethstatus %d\n", gethstatus);
                    goto goon;
                }
                /* 心跳管理 */
                if(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_HERTBEAT == pMsgInfo->IOP)
                {
                    ret = gui_heart_bear(pMsgInfo);
                    if(0 != ret)
                    {
                        GUI_FMT_DEBUG("gui send heart failed %d\n", ret);
                    }
                    goto goon;
                }

                /* 查询版本号 */
                if(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_VERSION == pMsgInfo->IOP)
                {
                    ret = gui_get_version(pMsgInfo);
                    if(0 != ret)
                    {
                        GUI_FMT_DEBUG("advance get version failed %d\n", ret);
                    }
                    goto goon;
                }

                /* 拨号统计 */
                if(MESSAGE_IID_WIRELESS == pMsgInfo->IID && WIRELESS_IOP_GET_NUM == pMsgInfo->IOP)
                {
                    ret = gui_get_Connecttime(pMsgInfo);
                    if(0 != ret)
                    {
                        GUI_FMT_DEBUG("advance get Connecttime failed %d\n", ret);
                    }
                    goto goon;
                }

                /* 查询模块信息 */
                if(MESSAGE_IID_WIRELESS == pMsgInfo->IID && WIRELESS_IOP_GET_MODULEINFO == pMsgInfo->IOP)
                {
                    ret = gui_get_moduleinfo(pMsgInfo);
                    if(0 != ret)
                    {
                        GUI_FMT_DEBUG("advance get moduleinfo failed %d\n", ret);
                    }
                    goto goon;
                }

                /* 查询拔号信息 */
                if(MESSAGE_IID_WIRELESS == pMsgInfo->IID && WIRELESS_IOP_GET_DIAGINFO == pMsgInfo->IOP)
                {
                    ret = gui_get_diaginfo(pMsgInfo);
                    if(0 != ret)
                    {
                        GUI_FMT_DEBUG("advance get diaginfo failed %d\n", ret);
                    }
                    goto goon;
                }
                /* 查询APP  ID */
                if(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_GETAPPID == pMsgInfo->IOP)
                {
                    ret = gui_get_appid(pMsgInfo);
                    if(0 != ret)
                    {
                        GUI_FMT_DEBUG(" gui_get_appid failed %d\n", ret);
                    }
                    goto goon;
                }
                
                /* 桌面消息处理 */
                if(MESSAGE_IID_DESKGUI == pMsgInfo->IID)
                {
                    ret = desktop_msg_handle(pMsgInfo);
                    if(0 != ret)
                    {   
                        GUI_FMT_TRACE("desktop msg handle failed %d\n",ret);
                    }
                    goto goon;
                }
#if defined (PRODUCT_ZCU_1)

                /* 电台查询参数 */
                if(MESSAGE_IID_RADIO == pMsgInfo->IID && RADIO_IOP_GET_CONFIG == pMsgInfo->IOP)
                {
                    ret = get_radio_cfg_msg(pMsgInfo);
                    if(TRUE != ret)
                    {   
                        GUI_FMT_TRACE("get_radio_cfg_msg failed %d\n",ret);
                    }
                    goto goon;
                }

                /* 电台查询参数 */
                if(MESSAGE_IID_RADIO == pMsgInfo->IID && RADIO_IOP_GET_VERSION == pMsgInfo->IOP)
                {
                    ret = deal_at_ver(pMsgInfo);
                    if(TRUE != ret)
                    {   
                        GUI_FMT_TRACE("get_radio_cfg_msg failed %d\n",ret);
                    }
                    goto goon;
                }
                
#endif
#if defined(AREA_FUJIAN) && defined (PRODUCT_ZCU_3)
                if(MESSAGE_IID_RSPSAMPLE == pMsgInfo->IID && RSPSAMPLE_IOP_WORKMODE_GET == pMsgInfo->IOP)
                {
                    gui_485_workmode_get_msg(pMsgInfo);
                    goto goon;
                }
                if(MESSAGE_IID_RSPSAMPLE == pMsgInfo->IID && RSPSAMPLE_IOP_WORKMODE_SET == pMsgInfo->IOP)
                {
                    gui_485_workmode_set_msg(pMsgInfo);
                    goto goon;
                }
#endif

                /* 处于初始化状态后续操作停止 */
                if(GUI_STATUS_INIT == appstatus)
                {
                    goto goon;
                }

                /* APP查询返回, 全部关注的接口都返回, 则结束查询 */
                if(MESSAGE_IID_SYSTEM == pMsgInfo->IID && SYSTEM_IOP_GET_REGISTERAPP == pMsgInfo->IOP)
                {
                    if(TRUE != gui_recv_regapp(pMsgInfo))
                    {
                        GUI_FMT_TRACE("gui_recv_regapp failed %d\n",ret);
                    }
                    goto goon;
                }
                
                /* 数据中心更新 */
                if(MESSAGE_IID_DB == pMsgInfo->IID && DB_IOP_UPDATE_EVENT == pMsgInfo->IOP)
                {
                    mqtt_db_update_fun(pMsgInfo);
                    goto goon;
                }
                /* 数据中心 */
                ret = db_agent_msg_entry(g_dbAgent, pMsgInfo);
                if(DB_AGENT_UNKONW_MSG != ret)
                {
                    goto goon;
                }

                /* APP查询返回, 全部关注的接口都返回, 则结束查询 */
#if defined(AREA_CHONGQING) && defined(PRODUCT_CCU)
                if(MESSAGE_IID_CTS == pMsgInfo->IID && CTS_IOP_READ_LIST_REQ == pMsgInfo->IOP)
                {
                    meter_trans_amr_recv_check_0010(pMsgInfo);
                    goto goon;
                }
#endif
                
                if(MESSAGE_IID_CTS == pMsgInfo->IID && CTS_IOP_READ_ONE_REQ == pMsgInfo->IOP)
                {
                    //不影响原有的逻辑
#if defined(AREA_CHONGQING) && defined(PRODUCT_CCU)
                    if(meter_trans_amr_recv_check(pMsgInfo) < 0)
#endif
                    {
                        gui_mannual_amr_rcv(pMsgInfo->playload);
                        GUI_FMT_TRACE("gui_mannual_amr_rcv \n",ret);
                    }
                    goto goon;
                }

                /* 高级APP + 采集任务调度 + 无线拨号 + 模组 */
                ret = task_recv_outmsg(pMsgInfo);
                if(TASK_UNKNOW_MSG != ret)
                {
                    goto goon;
                }
#ifdef AREA_FUJIAN
                /* 网络信号搜索结果返回 */
                if(MESSAGE_IID_WIRELESS == pMsgInfo->IID && WIRELESS_IOP_GTOPTSCAN_LAST_AT == pMsgInfo->IOP)
                {
                    if(TRUE != gui_recv_scanData(pMsgInfo->playload))
                    {
                        GUI_FMT_TRACE("gui_recv_regapp failed %d\n",ret);
                    }
                    goto goon;
                }
#endif

goon:
                free(pMsgInfo);
                pMsgInfo = NULL;
            }
        }

        /* 查询其他APP信息 */
        if(GUI_STATUS_WORK != appstatus)
        {
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            if(curtime.tv_sec - oldtime.tv_sec > 5)  //5s发送一次消息
            {
                if(GUI_STATUS_INIT == appstatus)
                {
                    ret = gui_login_to_smios();
                    if(0 != ret)
                    {
                        GUI_FMT_TRACE("gui_login_to_smios failed %d\n", ret);
                    }
                }
//                else if(GUI_STATUS_SEEK == appstatus)
//                {
//                    ret = gui_seek_app();
//                    if(0 != ret)
//                    {
//                        GUI_FMT_TRACE("gui_seek_app failed %d\n", ret);
//                    }
//                }

                oldtime = curtime;
            }
        }
    }
}


/**********************************************************************
* @name      : msg_task_start
* @brief     ：启动消息task任务
* @param[in] ：uint32 dbip
               uint16 dbport
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-24
* @Update    :
**********************************************************************/
int msg_task_start(uint32 ip, uint16 port)
{
    int ret = 0;
    //pthread_t taskid = 0;
    //pthread_t updateid = 0;
    pthread_t outmsgid = 0;

    /* 对外通信线程启动初始化 */
    ret =  gui_outmsg_init(MSG_TYPE_CHOOSE, ip, port);
    if(0 != ret)
    {
        printf("Out Msg update init error! err:%d\n", ret);
        return -1;
    }
    
//    ret = pthread_create(&taskid, NULL, task_msg_entry, NULL);
//    if (ret != 0)
//    {
//        /* 关闭线程 */
//      gui_outmsg_uninit();
//        printf("Msg Task thread Create error! err:%d\n", ret);
//        return -1;
//    }
    
    /* update msg线程启动 */
//  db_updata_init();
//    ret = pthread_create(&updateid, NULL, db_update_entry, NULL);
//    if (ret != 0)
//    {
//        pthread_cancel(taskid);
//        pthread_join(taskid, NULL);
//      gui_outmsg_uninit();
//        printf("Msg Udp update thread Create error! err:%d\n", ret);
//        return -1;
//    }

    /* out msg线程启动 */
    ret = pthread_create(&outmsgid, NULL, out_msg_entry, NULL);
    if (ret != 0)
    {
        //pthread_cancel(taskid);
        //pthread_join(taskid, NULL);
        //pthread_cancel(updateid);
        //pthread_join(updateid, NULL);
        gui_outmsg_uninit();
        printf("Msg out thread Create error! err:%d\n", ret);
        return -1;
    }

//    while(TRUE != gui_start_goon())
    {
//        sleep(1);
    }

    printf("Main start MQTT msg task!\n");

    return 0;
}

