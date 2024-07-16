/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级APP对外通信模块
* @date：    2019-12-24
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
#include "out_msg.h"
#include "db_update.h"

#include "gui_c_prtl.h"

uint8  g_advMqttDebug = 0;
uint8  g_MqttrecvBuf[APP_RECV_BUF_LEN] = {0};
int    g_Mqttrecvlen = 0;
time_t g_MqttStart = 0;
uint8  g_MqttMsgStatus = 0;  //0-处理完成，1-处理中
DB_CLIENT         g_dbClien = 0;

#define  ADV_LOST_HEARTBEAT_TIME  300
#define  ADV_LONG_MQTTMSG_TIME    10

/**********************************************************************
* @name      : adv_mqtt_debug_open
* @brief     ：mqtt调试接口打开
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-3
* @Update    :
**********************************************************************/
void adv_mqtt_debug_open()
{
    advance_mqtt_debug();
    g_advMqttDebug = 1;
}

/**********************************************************************
* @name      : adv_mqtt_debug_close
* @brief     ：mqtt调试接口关闭
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-3
* @Update    :
**********************************************************************/
void adv_mqtt_debug_close()
{
    g_advMqttDebug = 0;
    advance_mqtt_debug();
}

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
        printf("appmain task_msg_init error!\n");
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
                case MSG_APPAMIN_CTS_READ_LIST:
                case MSG_APPAMIN_CTS_READ_ONE:
                case MSG_APPAMIN_CTS_SET_LIST:
                case MSG_APPAMIN_CTS_SETREAD_LIST:
                case MSG_APPAMIN_CTS_ACT_LIST:
                case MSG_APPAMIN_CTS_ACTREAD_LIST:
                case MSG_APPAMIN_CTS_TRANSPORT:
                case MSG_APPAMIN_CTS_RPTSTATE:
				case MSG_APPAMIN_POWERUP_EVT:
                {
                    uint16 iop = 0;
                    iop = task_appmain_get_send_iop(pMsg);
                    if(0 != iop)
                    {
                        ret = advance_send_ctsmsg(pMsg->label, iop, pMsg->msg, pMsg->msglen);
                        if(0 != ret)
                        {
                            YCMSG_FMT_DEBUG("Send cts msg error iop 0x%04x, code %d\n", iop, ret);
                        }
                    }

                    break;
                }
                case MSG_APPAMIN_CTS_CARRIRT_TRANS:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_taskManager(pMsg->syncindex, MESSAGE_IID_CTS, CTS_IOP_CARRIRT_TRANS, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send taskManager msg set seek meter label[%d], code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_CTS_SEEK_METER:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_taskManager(pMsg->syncindex, MESSAGE_IID_CTS, CTS_IOP_SEEK_METER, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send taskManager msg set seek meter label[%d], code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_COMM_GETDIAL:
                {
                    ret = advance_send_wirelessmsg(pMsg->label, WIRELESS_IOP_GET_DIAGINFO, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send wireless get dial msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_GET_MODULEINFO:
                {
                    ret = advance_send_wirelessmsg(pMsg->label, WIRELESS_IOP_GET_MODULEINFO, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send wireless get mode msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_SETAPN:
                {
                    ret = advance_send_wirelessmsg(pMsg->label, WIRELESS_IOP_SET_DIAGARG, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send wireless msg sent apn error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_GETETH:
                {
                    ret = advance_send_smios(pMsg->label, MESSAGE_IID_SYSTEM, SYSTEM_IOP_GET_ETHSTATUS, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg get %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_COMM_ADDROUTE:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_smios(pMsg->syncindex, MESSAGE_IID_SYSTEM, SYSTEM_IOP_ADD_ROUTE, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg add route, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_DELROUTE:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_smios(pMsg->syncindex, MESSAGE_IID_SYSTEM, SYSTEM_IOP_DEL_ROUTE, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg del route, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_MAPINFO:
                case MSG_PULSE_MOD_GET_INFO:
                case MSG_CTRL_MOD_GET_INFO:
                {
                    ret = advance_send_modmsg(pMsg->label, MODULE_IOP_GET_INFO, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send get mod info msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_MAPSLOT:
                {
                    ret = advance_send_modmsg(pMsg->label, MODULE_IOP_GET_PORTINFO, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send get mod port msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_CTRL_MOD_GET_PORT:
                {
                    ret = advance_send_modmsg(pMsg->label, MODULE_IOP_GET_ALL, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send get mod port msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_PULSE_MOD_GET_STATUS:
                case MSG_CTRL_MOD_GET_STATUS:
                {
                    ret = advance_send_modmsg(pMsg->label, MODULE_IOP_GET_STATUS, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send mod status msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_module_update_cmd:
                {
                    ret = advance_send_modmsg(pMsg->label, MODULE_IOP_FILE_TRANSFER, pMsg->msg, pMsg->msglen);
                    printf("advance_send_modmsg发送结果\n");
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send mod status msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_RCT:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_smios(pMsg->syncindex, MESSAGE_IID_SYSTEM, SYSTEM_IOP_RCT, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_REBOOT:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_smios(pMsg->syncindex, MESSAGE_IID_SYSTEM, SYSTEM_IOP_REQUIRE_RESTART, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_AC_PARAM_INIT:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_acmeter(pMsg->syncindex, MESSAGE_IID_ACMETER, ACMETER_IOP_AC_PARM_INIT, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_AC_DATA_INIT:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_acmeter(pMsg->syncindex, MESSAGE_IID_ACMETER, ACMETER_IOP_AC_DATA_INIT, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_AC_DEMAND_INIT:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_acmeter(pMsg->syncindex, MESSAGE_IID_ACMETER, ACMETER_IOP_AC_DEMAND_INIT, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_AC_SYSTEM_CMD:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_acmeter(pMsg->syncindex, MESSAGE_IID_ACMETER, ACMETER_IOP_AC_SYSTEM_CMD, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_AC_RATE_SET:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_acmeter(pMsg->syncindex, MESSAGE_IID_ACMETER, ACMETER_IOP_AC_RATE_SET, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_SET_IPADDR:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_smios(pMsg->syncindex, MESSAGE_IID_SYSTEM, SYSTEM_IOP_SET_IPADDR, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_GET_DEVICEINFO:
                {
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_DEVICEINFO  advance_send_smios\n");
                    ret = advance_send_smios(pMsg->syncindex, MESSAGE_IID_SYSTEM, SYSTEM_IOP_GET_DEVINFO, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
				case MSG_APPAMIN_GET_CUR_DEVICEINFO:
                {
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_DEVICEINFO  advance_send_smios\n");
                    ret = advance_send_smios(pMsg->syncindex, MESSAGE_IID_SYSTEM, SYSTEM_IOP_GET_CUR_DEVINFO, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_GET_CONTAINERINFO:
                {
                    /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_CONTAINERINFO  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_DOCKER_FIND, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;

                }         
                case MSG_APPAMIN_INSTLL_CON:
                {
                    /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_CONTAINERINFO  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_DOCKER_INSTALL, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;

                }
                case MSG_APPAMIN_CONFIG_CON:
                {
                    /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_CONTAINERINFO  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_DOCKER_SET, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_EXEC_CON:
                {
                    /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_CONTAINERINFO  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_DOCKER_ACTION1, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_GET_APPINFO:
                {
                    /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_CONTAINERINFO  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_APP_FIND, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send iotManager msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPMAIN_INSTALL_APP:
                {
                    /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_CONTAINERINFO  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_APP_INSTALL, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send iotManager msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_EXEC_APP:
                {
                    /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_CONTAINERINFO  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_APP_ACTION1, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send iotManager msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_CONFIG_APP:
                {
                    /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_CONTAINERINFO  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_APP_SET, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send iotManager msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;

                }
                case MSG_APPAMIN_SETLISIENSE_APP:
                {
                                        /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_CONTAINERINFO  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_APP_LICENSE_SET, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send iotManager msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
				case MSG_APPAMIN_GET_LISIENSE:
                {
                                        /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_CONTAINERINFO  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_APP_LICENSE_FIND, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send iotManager msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_LOGINFO_GET:
                {
                                        /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_CONTAINERINFO  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_LOG_INFO_FIND, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send iotManager msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_LOG_PACKAGE:
                {
                                        /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_LOG_PACKAGE  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_LOG_PACK, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send iotManager msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
				case MSG_APPAMIN_UPDATE_SYS:
                {
                                        /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_GET_CONTAINERINFO  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_SYS_MOVE_UP1, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send iotManager msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_SYS_INPECT:
                {
                    /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_SYS_INPECT  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_SYS_INSPECT, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send iotManager msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
               case MSG_APPAMIN_FILE_DELECT:
                {
                    /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_SYS_INPECT  advance_send_iotManager\n");
                    ret = advance_send_iotManager(pMsg->syncindex, MESSAGE_IID_IOT, IOT_IOP_LOG_DELECT, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send iotManager msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;
                }
                case MSG_APPAMIN_SYSTEM_CMD:
                {
                    /* 使用同步消息标记 */
                    PRTL_FMT_DEBUG("MSG_APPAMIN_SYSTEM_CMD  advance_send_smios\n");
                    PRTL_BUF_DEBUG(pMsg->msg,pMsg->msglen," ");
                    ret = advance_send_smios(pMsg->syncindex, MESSAGE_IID_SYSTEM, SYSTEM_IOP_SYSTEM_CMD, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg set rct %d eth, code %d\n", pMsg->label, ret);
                    }
                    break;

                }
                case MSG_COMM_GUITOOLS:
                case MSG_CTRL_GUI_TOOL:
                case MSG_APPAMIN_ESAM_STATE:
                {
                    ret = advance_send_guimsg(pMsg->label, DESKGUI_IOP_FRESH_TOOL, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send gui tool ret %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_GUISTATUS:
                case MSG_CTRL_GUI_STATUS:
                {
                    ret = advance_send_guimsg(pMsg->label, DESKGUI_IOP_FRESH_STATE, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send gui status ret %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_SERVICE_START:
                {
                    ret = advance_send_smios(pMsg->label, MESSAGE_IID_SYSTEM, SYSTEM_IOP_SERVICE_START, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send port authorize msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_SERVICE_STOP:
                {
                    ret = advance_send_smios(pMsg->label, MESSAGE_IID_SYSTEM, SYSTEM_IOP_SERVICE_STOP, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send port authorize msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_CTRL_GUI_HINT:
                {
                    ret = advance_send_guimsg(pMsg->label, DESKGUI_IOP_PUSH_HINT, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send gui hint ret %d\n", ret);
                    }
                    break;
                }
                case MSG_CTRL_GUI_NOHINT:
                {
                    ret = advance_send_guimsg(pMsg->label, DESKGUI_IOP_POP_HINT, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send gui no hint ret %d\n", ret);
                    }
                    break;
                }

                case MSG_COMM_PROXYSEND:
                {
                    ret = advance_send_proxymsg(pMsg->label, PROXY_IOP_PROTOCOL_DATA_SEND, pMsg->msg, pMsg->msglen, pMsg->response);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send proxy protocol data msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_PROXYCONNECT:
                {
                    ret = advance_send_proxymsg(pMsg->label, PROXY_IOP_PHYSIC_CONNECT, pMsg->msg, pMsg->msglen, pMsg->response);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send proxy master physic connect msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_APPLYSTATUS:
                {
                    ret = advance_send_proxymsg(pMsg->label, PROXY_IOP_APPLY_CONNECT, pMsg->msg, pMsg->msglen, pMsg->response);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send proxy apply connect msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_GETPPPMODE:
                {
                    /* 使用同步消息 */
                    ret = advance_send_wirelessmsg(MSG_APPMAIN_FLAG + pMsg->syncindex, WIRELESS_IOP_GET_MODULEINFO, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send wireless get mode msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_GETPPPDIAG:
                {
                    /* 使用同步消息 */
                    ret = advance_send_wirelessmsg(MSG_APPMAIN_FLAG + pMsg->syncindex, WIRELESS_IOP_GET_DIAGINFO, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send wireless get dial msg error, code %d\n", ret);
                    }
                    break;
                }                 
                case MSG_APPAMIN_GETPPPGPS:
                {
                    /* 使用同步消息 */
                    ret = advance_send_wirelessmsg(MSG_APPMAIN_FLAG + pMsg->syncindex, WIRELESS_IOP_GET_GPS, 
                                                   pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send wireless get gps msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_GETMAPINFO:
                {
                    /* 使用同步消息 */
                    ret = advance_send_modmsg(MSG_APPMAIN_FLAG + pMsg->syncindex, MODULE_IOP_GET_INFO, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send mod get info msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_GETMAPALL:
                {
                    /* 使用同步消息 */
                    ret = advance_send_modmsg(MSG_APPMAIN_FLAG + pMsg->syncindex, MODULE_IOP_GET_ALL, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send mod get info msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_GETMAPSLOT:
                {
                    /* 使用同步消息 */
                    ret = advance_send_modmsg(MSG_APPMAIN_FLAG + pMsg->syncindex, MODULE_IOP_GET_PORTINFO, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send wireless get gps msg error, code %d\n", ret);
                    }
                    break;
                }                   
                case MSG_PULSE_GET_RMT:
                {
                    ret = advance_send_rspmsg(pMsg->label, RSPSAMPLE_IOP_RMT_STATE, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send rsp get rmt msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_PULSE_GET_PULSE:
                {
                    /* 使用同步消息 */
                    ret = advance_send_rspmsg(pMsg->syncindex, RSPSAMPLE_IOP_PULSE_STATIC, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send rsp get pulse msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_PULSE_SET_RSP:
                {
                    /* 使用同步消息 */
                    ret = advance_send_rspmsg(pMsg->syncindex, RSPSAMPLE_IOP_CONFIGURE, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send rsp set configure msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_PULSE_SHAKE_TIME:
                {
                    ret = advance_send_rspmsg(pMsg->label, RSPSAMPLE_IOP_SHAKE_TIME, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send rsp set shake time msg error, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_REDIAG:
                {
                    ret = advance_send_wirelessmsg(pMsg->label, WIRELESS_IOP_REDIAG, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send wireless msg sent rediag, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_SMSNO:
                {
                    ret = advance_send_wirelessmsg(pMsg->label, WIRELESS_IOP_SET_SMSNO, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send wireless msg sent smsno, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_AUTH:
                {
                    ret = advance_send_wirelessmsg(pMsg->label, WIRELESS_IOP_SET_AUTH, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send wireless msg sent auth, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_SMSSEND:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_wirelessmsg(pMsg->syncindex, WIRELESS_IOP_SEND_SMS, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send wireless msg sent smssend, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_SMSRECV:
                {
                    /* 使用同步消息标记 */
                    ret = advance_send_wirelessmsg(pMsg->syncindex, WIRELESS_IOP_REVC_SMS, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send wireless msg sent smsrecv, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPMAIN_MANAGER_APP:
                {
                    ret = advance_send_smios(pMsg->syncindex, MESSAGE_IID_SYSTEM, SYSTEM_IOP_APP_MANAGER, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send smios msg Manager failed %d\n", ret);
                    }
                    break;
                }
				case MSG_APPMAIN_TRANSPORT:
                {
                    /* 使用同步消息 */
                    ret = advance_send_wirelessmsg(MSG_APPMAIN_FLAG + pMsg->syncindex, WIRELESS_IOP_TRANS_AT, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send wireless get dial msg error, code %d\n", ret);
                    }
                    break;
                } 
                case MSG_COMM_GETUART:
                {
                    /* 获取串口状态与模式 */
                    ret = advance_send_uartmsg(pMsg->label, MESSAGE_IID_UART, UART_IOP_GET_INFO, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send uart get msg Manager failed %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_SETUART:
                {
                    /* 串口参数设置 */
                    ret = advance_send_uartmsg(pMsg->label, MESSAGE_IID_UART, UART_IOP_SET_PARAM, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send uart get msg Manager failed %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_REGAPP:
                {
                    /* 串口从模式APP注册 */
                    ret = advance_send_uartmsg(pMsg->label, MESSAGE_IID_UART, UART_IOP_SET_LISTEN, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send uart get msg Manager failed %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_SENDUART:
                {
                    /* 串口发送消息 */
                    ret = advance_send_uartmsg(pMsg->syncindex, MESSAGE_IID_UART, UART_IOP_SEND_DATA, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send uart get msg Manager failed %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_UART_MODESET:
                {
                    /* 串口模式设置 */
                    ret = advance_send_uartmsg(pMsg->syncindex, MESSAGE_IID_UART, UART_IOP_ONLYMODE_SET, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send uart get msg Manager failed %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_GETBTCFG:
                {
                    ret = advance_send_btmsg(1, MESSAGE_IID_BLE, BLE_IOP_GET_CONFIG, NULL, 0);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send bt get cfg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_SETBTCFG:
                {
                    ret = advance_send_btmsg(1, MESSAGE_IID_BLE, BLE_IOP_SET_CONFIG, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send bt get cfg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_GETBTPORT:
                {
                    ret = advance_send_btmsg(1, MESSAGE_IID_BLE, BLE_IOP_LINK_INFO, NULL, 0);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send bt get port, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_SETBTPORT:
                {
                    ret = advance_send_btmsg(1, MESSAGE_IID_BLE, BLE_IOP_SET_PORT, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send bt set port, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_BTSENDMSG:
                {
                    ret = advance_send_btmsg(pMsg->syncindex, MESSAGE_IID_BLE, BLE_IOP_SEND_DATA, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send bt set port, code %d\n", ret);
                    }
                }
                case MSG_APPMAIN_GET_BTCFG:
                {
                    /* 使用同步消息 */
                    ret = advance_send_btmsg(MSG_APPMAIN_FLAG + pMsg->syncindex, MESSAGE_IID_BLE, BLE_IOP_GET_CONFIG, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send bt get cfg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPMAIN_GET_BTCONNECT:
                {
                    /* 使用同步消息 */
                    ret = advance_send_btmsg(MSG_APPMAIN_FLAG + pMsg->syncindex, MESSAGE_IID_BLE, BLE_IOP_LINK_INFO, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send bt get cfg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPMAIN_GET_BT_SN:
                {
                    /* 使用同步消息 */
                    ret = advance_send_btmsg(MSG_APPMAIN_FLAG + pMsg->syncindex, MESSAGE_IID_BLE, BLE_IOP_BT_SN, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send bt get cfg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPMAIN_SET_BT_SN:
                {
                    /* 使用同步消息 */
                    ret = advance_send_btmsg(MSG_APPMAIN_FLAG + pMsg->syncindex, MESSAGE_IID_BLE, BLE_IOP_SET_BT_SN, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send bt get cfg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPMAIN_SET_BTCFG:
                {
                    /* 使用同步消息 */
                    ret = advance_send_btmsg(MSG_APPMAIN_FLAG + pMsg->syncindex, MESSAGE_IID_BLE, BLE_IOP_SET_CONFIG, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send bt get cfg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPMAIN_SET_BTCHECK:
                {
                    /* 使用同步消息 */
                    ret = advance_send_btmsg(MSG_APPMAIN_FLAG + pMsg->syncindex, MESSAGE_IID_BLE, BLE_IOP_CHECK_METER, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send bt check, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_GETRADIOCFG:
                {
                    ret = advance_send_radiomsg(1, MESSAGE_IID_RADIO, RADIO_IOP_GET_CONFIG, NULL, 0);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send radio get cfg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPMAIN_SET_RADIOADDR:
                case MSG_COMM_SETRADIOCFG:
                {
                    ret = advance_send_radiomsg(1, MESSAGE_IID_RADIO, RADIO_IOP_SET_ADDR, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send radio set cfg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPMAIN_SET_RADIOCFG:
                {
                    ret = advance_send_radiomsg(MSG_APPMAIN_FLAG + pMsg->syncindex, MESSAGE_IID_RADIO, RADIO_IOP_SET_CONFIG, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send radio set cfg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPMAIN_GET_RADIOCFG:
                {
                    ret = advance_send_radiomsg(MSG_APPMAIN_FLAG + pMsg->syncindex, MESSAGE_IID_RADIO, RADIO_IOP_GET_CONFIG, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send radio set cfg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPMAIN_GET_RADIOVER:
                {
                    ret = advance_send_radiomsg(MSG_APPMAIN_FLAG + pMsg->syncindex, MESSAGE_IID_RADIO, RADIO_IOP_GET_VERSION, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send radio set cfg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPMAIN_SET_RADIOSPEAK:
                {
                    ret = advance_send_radiomsg(MSG_APPMAIN_FLAG + pMsg->syncindex, MESSAGE_IID_RADIO, RADIO_IOP_SET_YANG, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send radio set cfg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_RADIOSENDMSG:
                {
                    ret = advance_send_radiomsg(pMsg->syncindex, MESSAGE_IID_RADIO, RADIO_IOP_SEND_DATA, pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send radio send msg, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_FLOW_PARAM_SET:
                {
                    /* 使用同步消息 */
                    ret = advance_send_scs(MSG_APPMAIN_FLAG + pMsg->syncindex, MESSAGE_IID_SCS, SCS_IOP_FLOW_PARAM_SET, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send scs flow param set, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_FLOW_PARAM_GET:
                {
                    /* 使用同步消息 */
                    ret = advance_send_scs(MSG_APPMAIN_FLAG + pMsg->syncindex, MESSAGE_IID_SCS, SCS_IOP_FLOW_PARAM_GET, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send scs flow param get, code %d\n", ret);
                    }
                    break;
                }
                case MSG_APPAMIN_TIME_OUT_SET:
                {
                    /* 使用同步消息 */
                    ret = advance_send_scs(MSG_APPMAIN_FLAG + pMsg->syncindex, MESSAGE_IID_SCS, SCS_IOP_TIME_OUT_SET, 
                                             pMsg->msg, pMsg->msglen);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("Send scs time out set, code %d\n", ret);
                    }
                    break;
                }
                case MSG_COMM_APP_TRANS:
                {
                    char            uTopic[256] = {0};
                    MESSAGE_INFO_T *pMsgOut = (MESSAGE_INFO_T *)pMsg->msg;

                    sprintf(uTopic, "%s/%s", pMsgOut->souraddr, pMsgOut->destaddr);
                    ret = advance_send_msg(uTopic, pMsgOut, 0);
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
    int         ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8       appstatus = ADVANCE_STATUS_INIT;  //正式代码改为 ADVANCE_STATUS_INIT
    int         dbagent = advance_get_dbagent();
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    time_t      heart_time = 0;
    uint64      msgnum = 0;

    /* 先发送一次APP注册消息 */
    ret = advance_login_to_smios();
    if(0 != ret)
    {
        YCMSG_FMT_TRACE("advance_login_to_smios failed %d\n", ret);
    }
    g_dbClien  = db_client_create(DB_YC_AGENT, DB_CLIENT_OUTMSG);
    if(g_dbClien <= 0)
    {
        EVENT_FMT_DEBUG("g_dbClien创建失败, 返回! \n");       
    }
    /* 循环收消息 */
    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    for(;;)
    {
        g_Mqttrecvlen = advance_recv_msg(g_MqttrecvBuf, APP_RECV_BUF_LEN);
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        if(g_Mqttrecvlen > 0)
        {
            msgnum++;
            g_MqttStart = curtime.tv_sec;
            g_MqttMsgStatus = 1;

            ret = appmsg_split(g_MqttrecvBuf, (uint16)g_Mqttrecvlen , &pMsgInfo);
            if(NULL != pMsgInfo)
            {
                YCMSG_BUF_TRACE(g_MqttrecvBuf, g_Mqttrecvlen, "Recv msg from %s:", pMsgInfo->souraddr);
                if(0 == strcmp(pMsgInfo->souraddr, ADV_APP_NAME))
                {
                    YCMSG_FMT_DEBUG("Revc msg form %s is the same name, iid: %d, iop %d\n", pMsgInfo->souraddr, pMsgInfo->IID, pMsgInfo->IOP);
                    goto goon;
                }
            }
            else
            {
                YCMSG_BUF_TRACE(g_MqttrecvBuf, g_Mqttrecvlen, "Recv msg:");
            }
            if(0 == ret && NULL != pMsgInfo)
            {
                /* 注册应答 */
                if(MESSAGE_IID_SYSTEM == pMsgInfo->IID && SYSTEM_IOP_APP_REGISTER == pMsgInfo->IOP)
                {
                    heart_time = curtime.tv_sec;
                    if(/*TRUE == *pMsgInfo->playload && */ADVANCE_STATUS_INIT == appstatus)
                    {
                        appstatus = ADVANCE_STATUS_SEEK;
                        printf("=======advance login to OS=======\n");
                        YCMSG_FMT_DEBUG("=======advance login to OS=======\n");
                        ret = advance_seek_app();  //查询注册的APP
                        if(0 != ret)
                        {
                            YCMSG_FMT_DEBUG("advance_seek_app failed %d\n", ret);
                        }
                    }
                    goto goon;
                }
                
                /* 心跳管理 */
                if(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_HERTBEAT == pMsgInfo->IOP)
                {
                    heart_time = curtime.tv_sec;
                    ret = advance_heart_bear(pMsgInfo);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("advance send heart failed %d\n", ret);
                    }
                    goto goon;
                }

                /* 查询版本号 */
                if(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_VERSION == pMsgInfo->IOP)
                {
                    ret = advance_get_version(pMsgInfo);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("advance get version failed %d\n", ret);
                    }
                    goto goon;
                }
                /* 查询appid */
                if(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_GETAPPID == pMsgInfo->IOP)
                {
                    ret = advance_get_appidmsg(pMsgInfo);
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("advance get appid failed %d\n", ret);
                    }
                    goto goon;
                }

                /* 处于初始化状态后续操作停止 */
                if(ADVANCE_STATUS_INIT == appstatus)
                {
                    goto goon;
                }

                /* APP查询返回, 全部关注的接口都返回, 则结束查询 */
                if(MESSAGE_IID_SYSTEM == pMsgInfo->IID && SYSTEM_IOP_GET_REGISTERAPP == pMsgInfo->IOP)
                {
                    if(TRUE == advance_recv_regapp(pMsgInfo))
                    {
                        appstatus = ADVANCE_STATUS_WORK;
                    }
                    goto goon;
                }
                /*IOTMANAGER*/
                if(MESSAGE_IID_IOT == pMsgInfo->IID)
                {
                    if(DB_AGENT_UNKONW_MSG != task_recv_iot_msg(pMsgInfo))
                    {
                        goto goon;
                    }
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
                        YCMSG_BUF_DEBUG(g_MqttrecvBuf, g_Mqttrecvlen, "Drop msg from %s, ret %d:", pMsgInfo->souraddr, ret);
                    }
                    goto goon;
                }
            
                /* 采集任务调度 + 无线拨号 + 模组 + 控制 */
                ret = task_recv_outmsg(pMsgInfo);
                if(TASK_UNKNOW_MSG != ret)
                {
                    goto goon;
                }
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
                if(pMsgInfo->IID == MESSAGE_IID_DESKGUI)
                {
				    appGUI_recv_back_handle(pMsgInfo);
                }
//#endif
goon:
                free(pMsgInfo);
                pMsgInfo = NULL;
            }
            g_MqttMsgStatus = 0;
            memset(g_MqttrecvBuf, 0, g_Mqttrecvlen);
            g_Mqttrecvlen = 0;
        }

        /* 查询其他APP信息 */
        if(ADVANCE_STATUS_WORK != appstatus)
        {
            if(curtime.tv_sec - oldtime.tv_sec > 5)  //5s发送一次消息
            {
                if(ADVANCE_STATUS_INIT == appstatus)
                {
                    ret = advance_login_to_smios();
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("advance_login_to_smios failed %d\n", ret);
                    }
                }
                else if(ADVANCE_STATUS_SEEK == appstatus)
                {
                    ret = advance_seek_app();
                    if(0 != ret)
                    {
                        YCMSG_FMT_DEBUG("advance_seek_app failed %d\n", ret);
                    }
                }
                oldtime = curtime;
            }
        }
        else
        {
            /* 心跳时间维护 */
            if(curtime.tv_sec > heart_time + ADV_LOST_HEARTBEAT_TIME)
            {
                YCMSG_FMT_DEBUG("APP lost heartbeat more than 300s, reboot!\n");
                advance_mqtt_debug();
                sleep(1);
                exit(0);
                return 0;        
            }
            if(1 == g_advMqttDebug)
            {
                if(curtime.tv_sec - oldtime.tv_sec > 10)
                {
                    printf("Proc mqtt msg num %llu\n", msgnum);
                    YCMSG_FMT_DEBUG("Proc mqtt msg num %llu\n", msgnum);
                    oldtime = curtime;
                }
            }
        }
    }
}

/**********************************************************************
* @name      : out_msg_manager
* @brief     ：启动对外通信消息处理监控
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-8-3
* @Update    :
**********************************************************************/
void *out_msg_manager(void *pArg)
{
    struct timespec curtime = {0};

    for(;;)
    {
        if(0 == g_MqttMsgStatus)
        {
            goto delay;
        }
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        
        if(curtime.tv_sec > g_MqttStart + ADV_LONG_MQTTMSG_TIME)
        {
            YCMSG_BUF_DEBUG(g_MqttrecvBuf, g_Mqttrecvlen, "Mqtt proc time more then %d\n", ADV_LONG_MQTTMSG_TIME);
            g_MqttMsgStatus = 0;
        }
delay:
        sleep(1);
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
    pthread_t outmsgmaneid = 0;

    /* 对外通信线程启动初始化 */
    ret = advance_outmsg_init(MSG_TYPE_CHOOSE, ip, port);
    if(0 != ret)
    {
        printf("Out Msg update init error! err:%d\n", ret);
        return -1;
    }

    ret = pthread_create(&taskid, NULL, task_msg_entry, NULL);
    if (ret != 0)
    {
        /* 关闭线程 */
        advance_outmsg_uninit();
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
        advance_outmsg_uninit();
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
        advance_outmsg_uninit();
        printf("Msg out thread Create error! err:%d\n", ret);
        return -1;
    }

    ret = pthread_create(&outmsgmaneid, NULL, out_msg_manager, NULL);
    if (ret != 0)
    {
        printf("Msg out manager thread Create error! err:%d\n", ret);
        return -1;
    }

    printf("Main start MQTT msg task!\n");

    return 0;
}

