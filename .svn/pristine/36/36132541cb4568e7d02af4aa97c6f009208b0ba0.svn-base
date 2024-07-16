/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控mqtt
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"
#include "task_msg.h"
#include "db_api.h"
#include "appmsg_split.h"
#include "mqtt_client.h"
#include "task_base.h"
#include "task_id.h"
#include "msg_id.h"
#include "mapmsg.h"

#include "lc_comm.h"
#include "lc_mqtt.h"
#include "lc_monitor.h"
#include "uart_api.h"
#include "lc_mod.h"

extern BOOL g_mod_status_chg;
extern int g_lcm_slot;
extern void *g_pUart; /*串口句柄*/
extern void *memcpy_r(void *dest, const void *src, int count);
extern void lc_uart_uninit(void **pDev);
extern uint8 g_recvRetry;
extern void *lc_uart_init(char *device);
extern LC_WORK_STATUS_T g_workstatus[MOD_SAMPLE_PERIOD];
uint32 g_secNum = 0;
LC_UART_VDEV_T g_uart_dev = {0};

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
int lc_get_map_slot(int sockfd, const char *type)
{
    uint8 sendbuf[128] = {0};
    uint8 size = 0;
    int   ret = 0;    
    
    size = strlen(type);
    sendbuf[0] = size;
    memcpy(sendbuf + 1, type, size);

    ret = task_send_labelmsg(sockfd, MSG_LC_MAP_SLOT, 0, sendbuf, size + 1, LC_MSG_TASK);
    
    if(0 == ret)
    {
        LCMONIT_FMT_TRACE("Send to 0x%x: msg 0x%x, label %d, ret %d\n", LC_MSG_TASK, MSG_LC_MAP_SLOT, 0, ret);
    }
    else
    {
        LCMONIT_FMT_DEBUG("Send to 0x%x: msg 0x%x, label %d, ret %d\n", LC_MSG_TASK, MSG_LC_MAP_SLOT, 0, ret);
    }

    return ret;
}


/**********************************************************************
* @name      : lc_mod_reset_send
* @brief     ：发送模块重启消息
* @param[in] ：type       模块型号
               uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：消息长度
* @Date      ：2020-5-30
* @Update    :
**********************************************************************/
void lc_mod_restart_send(uint32 slot, uint32 time)
{
    int ret = 0;
    uint8 buf[24] = {0};
    uint16 offset = 0;

    if(0 == slot || slot > 5)
    {
        return;
    }

    appmsg_memcpy_r(buf, &slot, sizeof(uint32));
    offset += sizeof(uint32);
    appmsg_memcpy_r(buf + offset, &time, sizeof(uint32));
    offset += sizeof(uint32);

    ret = lc_send_mapmsg((uint16)slot, MODULE_IOP_RESTART, buf, offset);
    LCMONIT_FMT_DEBUG("slot(%d) is poweroff %ds, ret(%d).\n", slot, time, ret);
}

#ifdef PRODUCT_SCU 
/**********************************************************************
* @name      : lc_uart_mode_check
* @brief     ：串口模块查询
* @param[in] ：type       模块型号
               uint16 buflen  缓存大小
* @param[out]：uint8* recvbuf 缓存
* @return    ：消息长度
* @Date      ：2020-5-30
* @Update    :
**********************************************************************/
void lc_uart_mode_check(void *pPrivate)
{
    uint8    msg[64] = {0};
    uint16   msglen = 0;
    UART_PORT_T tPort;
    LC_MONITOR_DATA_T *ptData = (LC_MONITOR_DATA_T *)pPrivate;

    tPort.portType = UART_TYPE_485;
    tPort.portNo = 4;

    ptData->transport = tPort;

    msglen = uart_axdr_get_info_req_package(&tPort, msg, 64);

    lc_send_uartmsg(0, UART_IOP_GET_INFO, msg, msglen);
}

#endif

#if DESC("收消息处理",1)

/**********************************************************************
* @name      : lc_map_slot_ack
* @brief     ：模组槽位查询返回消息
* @param[in] ：TASK_MSG_t *pMsg            模组消息
* @param[out]：LC_MONITOR_DATA_T *pHandle  私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_map_slot_ack(TASK_MSG_t *pMsg, LC_MONITOR_DATA_T *pHandle)
{
    int ret = 0;
    int i = 0;
    uint8 num = 0;
    uint16 msglen = pMsg->msglen;
    uint8 *revcbuf = pMsg->msg;
    uint8  offset = 0;

    if(msglen != 0)
    {
        num = revcbuf[offset++];

        if(num != 0)
        {
            for(i = 0; i< 1 && i < num; i++)
            {
                memcpy_r(&g_lcm_slot, &revcbuf[offset], sizeof(int));
                offset += sizeof(int);

                if(offset > msglen)
                {
                    LCMONIT_FMT_DEBUG("module slot ack msg fatal error: offset(%d) > msglen(%d).\n", offset, msglen);
                    return ERR_OVERFLOW;
                }

                LCMONIT_FMT_DEBUG("detected slot%d for \"%s\".\n", g_lcm_slot, MOD_LC_TYPE_PRFIX);
            }
        }    
    }
    
    return ret;
}

/**********************************************************************
* @name      : lc_map_info_ack
* @brief     ：模组信息查询返回结果
* @param[in] ：TASK_MSG_t *pMsg            模组消息
* @param[out]：LC_MONITOR_DATA_T *pHandle  私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_map_info_ack(TASK_MSG_t *pMsg, LC_MONITOR_DATA_T *pHandle)
{
    int ret = 0;
    
    return ret;
}

/**********************************************************************
* @name      : lc_map_event_notify
* @brief     ：模组主动上报事件
* @param[in] ：TASK_MSG_t *pMsg            模组消息
* @param[out]：LC_MONITOR_DATA_T *pHandle  私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_map_event_notify(TASK_MSG_t *pMsg, LC_MONITOR_DATA_T *pHandle)
{
    int ret = 0;
    uint16 msglen = pMsg->msglen;
    uint8 *revcbuf = pMsg->msg;
    uint8  offset = 0;
    int slot = 0;
    uint8 status = 0;

    if(msglen != 0)
    {
        memcpy_r(&slot, &revcbuf[offset], sizeof(int));
        offset += sizeof(int);  

        memcpy(&status, &revcbuf[offset], sizeof(uint8));
        offset += sizeof(uint8);  

        if(offset > msglen)
        {
            LCMONIT_FMT_DEBUG("module plug/uplug msg: offset(%d) > msglen(%d).\n", offset, msglen);
            return ERR_OVERFLOW;
        }

        //模块已经拔出，再检测到插入模块
        if((g_lcm_slot == 0) &&(status == 0))
        {
            LCMONIT_FMT_DEBUG("module plug notify: slot(%d)\n", slot);
            g_mod_status_chg = TRUE;  
            return lc_get_map_slot(pHandle->tasksocket, MOD_LC_TYPE_PRFIX); 
        }
        else if((status == 1) && (g_lcm_slot == slot)) //拔出目标模块，则清零槽位和状态，待查询到新信息
        {
            //lc_uart_uninit(&g_pUart);
            g_lcm_slot = 0;
            g_recvRetry = 0;
            LCMONIT_FMT_DEBUG("lc module unplug: slot(%d)\n", slot);
        }
    }
    
    return ret;
}

LC_MONITORFUN_T g_LcMonitorMsgFun[] = 
{
    {MSG_LC_MAP_SLOT,    lc_map_slot_ack},
    {MSG_LC_MAP_INFO,    lc_map_info_ack}, 
    {MSG_LC_MAP_EVENT,   lc_map_event_notify},
};
uint8 g_LcMonitorMsgNum = sizeof(g_LcMonitorMsgFun)/sizeof(LC_MONITORFUN_T);

#endif

/**********************************************************************
* @name      : lc_taskmsg_init
* @brief     ：task消息处理
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_monitor_init(void *pPublic, void **ppPrivate)
{
    LC_APPDATA_T *pAppData = (LC_APPDATA_T*)pPublic;
    LC_MONITOR_DATA_T *ptData = NULL;
    int ret = 0;
    
//    while(FALSE == lc_appmap_flag())
//    {
//        sleep(1);
//    }

    ptData = (LC_MONITOR_DATA_T *)malloc(sizeof(LC_MONITOR_DATA_T));
    if(NULL == ptData)
    {
        MQTTMSG_FMT_DEBUG("lc_taskmsg_init malloc failed\n");
        return -1;
    }
    memset(ptData, 0, sizeof(LC_MONITOR_DATA_T));


    ptData->fileInfo = (LC_FILE_TRANS_T *)malloc(sizeof(LC_FILE_TRANS_T));
    if(NULL == ptData->fileInfo)
    {
        UPDATE_FMT_DEBUG("lc_update_init fileInfo malloc failed\n");
        return -1;
    }

    ptData->clientid = db_client_create(DB_LC_AGENT, DB_LC_CLIENT_MONITOR);
    if(ptData->clientid <= 0)
    {
        LCMONIT_FMT_DEBUG("lc_monitor_init db db_client_create error!\n");
        return -2;
    }

    ptData->tasksocket = task_msg_init(LC_MSG_WORK);
    if(ptData->tasksocket <= 0)
    {
        LCMONIT_FMT_DEBUG("lc_monitor_init taskmsg init failed\n");
        db_client_close(ptData->clientid);
        free(ptData);
        return -3;
    }
    task_msg_settime(ptData->tasksocket, 0, 100000);

    ret = lc_get_map_slot(ptData->tasksocket, MOD_LC_TYPE_PRFIX);
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("lc_get_map_slot error!\n");
        return -3;
    }

    memset(&g_workstatus, 0xff, sizeof(g_workstatus));

    g_uart_dev.desc = pAppData->devName[LC_CCH_DATACOM];
    g_uart_dev.bytetimeout = 200;
    g_uart_dev.frametimeout = 4000;
    g_uart_dev.param.baudRate = 115200;
    g_uart_dev.param.dataBits = 8;    
    g_uart_dev.param.stopBits = 1;    
    g_uart_dev.param.parity = UART_PARITY_NONE;
    
#ifdef PRODUCT_SCU  
    g_uart_dev.fd = -1;
    ret = uart_open(&g_uart_dev);
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("lc uart open failed. ret %d\n", ret);
    }
#endif

    ptData->pUart = &g_uart_dev;
    ptData->status = LCM_GETM;

#ifdef PRODUCT_SCU
    lc_uart_mode_check(ptData);
#endif

    *ppPrivate = ptData;
    return 0;
}

/**********************************************************************
* @name      : lc_monitor_proc
* @brief     ：回路监控业务线程
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_monitor_proc(void *pArg, void *pPublic, void *pPrivate)
{
    LC_MONITOR_DATA_T *ptMsgData = (LC_MONITOR_DATA_T *)pPrivate;
    TASK_MSG_t        *pMsg = (TASK_MSG_t *)ptMsgData->recvBuf;
    uint32             sender = 0;
    int                ret = 0;
    uint8              i = 0;
    struct timespec  curtime = {0};
    static struct timespec  lsttime = {0};

    memset(ptMsgData->recvBuf, 0, sizeof(ptMsgData->recvBuf));

    ptMsgData->recvlen = task_recv_msg(ptMsgData->tasksocket, ptMsgData->recvBuf, TASK_MSG_MAX_LEN, &sender);
    if(ptMsgData->recvlen > 0)
    {
        TASK_SET_USERLOG(pArg, "task msg 0x%08x", pMsg->msg_id);

        for(i = 0; i < g_LcMonitorMsgNum; i++)
        {
            if(pMsg->msg_id == g_LcMonitorMsgFun[i].msgid)
            {
                if(g_LcMonitorMsgFun[i].pProcMsg)
                {
                    ret = g_LcMonitorMsgFun[i].pProcMsg(pMsg, ptMsgData);
                    LCMONIT_FMT_DEBUG("monitor_proc msg 0x%x, ret %d\n", pMsg->msg_id, ret);
                }
                break;
            }
        }
    }

    /* 定时操作 todo */
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    if(curtime.tv_sec != lsttime.tv_sec)
    {
        if(lsttime.tv_sec == 0)
        {
            g_secNum++;
        }
        else
        {
            g_secNum += (curtime.tv_sec-lsttime.tv_sec) ;
        }

        TASK_SET_USERLOG(pArg, "lc_monitor_deal");
        lc_monitor_deal(pPublic, pPrivate);

        if(g_secNum > MOD_SAMPLE_PERIOD)
        {
            g_secNum = MOD_SAMPLE_PERIOD;
        }

        if(g_secNum % MOD_SAMPLE_PERIOD == 0)
        {
            TASK_SET_USERLOG(pArg, "lc_workstate_analyze.\n");
            lc_workstate_analyze(ptMsgData->clientid, ptMsgData->tasksocket);

            #if defined PRODUCT_ECU || defined PRODUCT_ECU_G
            ret = lc_version_get(ptMsgData->pUart, ptMsgData->clientid, ptMsgData->tasksocket);
            #else
            ret = lcm_version_get(ptMsgData->pUart, ptMsgData->clientid, ptMsgData->tasksocket);
            #endif
            
            g_secNum = 0;
            memset(&g_workstatus, 0xff, sizeof(g_workstatus));
        }        
    }
    
    memcpy(&lsttime, &curtime, sizeof(struct timespec));
    
    return 0;
}


