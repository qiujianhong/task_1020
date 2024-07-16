/*
*********************************************************************
* Copyright(C) 2024 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   负荷控制APP数据初始化
* @date：    2024-06-05
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "oopType.h"
#include "appmsg_split.h"
#include "mqtt_client.h"
#include "db_api.h"
#include "framecomm.h"
#include "task_id.h"
#include "task_base.h"
#include "gui_c_prtl.h"

#include "loadCtrller_data.h"
#include "loadCtrller_mqtt.h"
#include "loadCtrller_msg.h"

int              g_dbAgent = 0;
LCTRL_APPMSG_T   g_appmsg;

extern void appctrl_set_key(uint8 status);
extern void lctrl_get_allmodule();

/**********************************************************************
* @name      : lctrl_appmsg_power_off
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-6-6
* @Update    :
**********************************************************************/
int lctrl_appmsg_power_off(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    MQTTMSG_FMT_DEBUG("终端停电\n");
    //evt_emerge_set(COMMON_IOP_URGENT_POWERDOWN);
    
    return 0;
}

/**********************************************************************
* @name      : lctrl_appmsg_power_on
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-6-6
* @Update    :
**********************************************************************/
int lctrl_appmsg_power_on(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    MQTTMSG_FMT_DEBUG("终端上电\n");
    //evt_emerge_set(COMMON_IOP_SYSTEM_POWERON);
    
    return 0;
}

/**********************************************************************
* @name      : lctrl_appmsg_power_restart
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-6-6
* @Update    :
**********************************************************************/
int lctrl_appmsg_power_restart(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    MQTTMSG_FMT_DEBUG("终端复位\n");
    //evt_emerge_set(COMMON_IOP_SYSTEM_RESTART);
    
    return 0;
}

/**********************************************************************
* @name      : lctrl_modport_ack
* @brief     ：查询模组端口 返回消息处理
* @param[in] ：int cid          数据中心cid
               uint16 label     消息label
               uint8* pMsg      消息
               uint16 msglen    消息长度
* @param[out]：uint32* nextStep 下一个步骤
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-7-1
* @Update    : 
**********************************************************************/
int lctrl_modport_ack(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    LCTRL_DATA_T    *ptLCData = (LCTRL_DATA_T*)pPublic;
    uint8           *temp = NULL;
    uint16           offset = 0;
    uint32           i = 0;
    uint32           j = 0;
    uint8            type = 0;
    
    OOP_MODULE_INFO_NEW_T   tmodelInfo;
    uint8                  no = 0;
    uint8                port = 0;
    uint8             subport = 0;
    uint8                 num = 0;

    if(NULL == pInMsg || NULL == pPublic)
    {
        return -1;
    }

    temp = pInMsg->playload;
    memset(&tmodelInfo, 0, sizeof(OOP_MODULE_INFO_NEW_T));

    tmodelInfo.nNum = temp[offset++];
    if(tmodelInfo.nNum > OOP_MAX_MODULE)
    {
        tmodelInfo.nNum = OOP_MAX_MODULE;
    }
    for(i = 0; i < tmodelInfo.nNum; i++)
    {
        tmodelInfo.item[i].nSlot = i + 1;
        type = temp[offset++];
        if(0 != type)
        {
            continue;
        }
        tmodelInfo.item[i].modType.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].modType.value, temp + offset, tmodelInfo.item[i].modType.nNum);
        offset += tmodelInfo.item[i].modType.nNum;
        
        tmodelInfo.item[i].modId.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].modId.value, temp + offset, tmodelInfo.item[i].modId.nNum);
        offset += tmodelInfo.item[i].modId.nNum;
        
        tmodelInfo.item[i].nSoftVer.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].nSoftVer.value, temp + offset, tmodelInfo.item[i].nSoftVer.nNum);
        offset += tmodelInfo.item[i].nSoftVer.nNum;

        tmodelInfo.item[i].softDate.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].softDate.value, temp + offset, tmodelInfo.item[i].softDate.nNum);
        offset += tmodelInfo.item[i].softDate.nNum;

        tmodelInfo.item[i].nHardVer.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].nHardVer.value, temp + offset, tmodelInfo.item[i].nHardVer.nNum);
        offset += tmodelInfo.item[i].nHardVer.nNum;

        tmodelInfo.item[i].hardDate.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].hardDate.value, temp + offset, tmodelInfo.item[i].hardDate.nNum);
        offset += tmodelInfo.item[i].hardDate.nNum;

        tmodelInfo.item[i].manuf.nNum = temp[offset++];
        memcpy(tmodelInfo.item[i].manuf.value, temp + offset, tmodelInfo.item[i].manuf.nNum);
        offset += tmodelInfo.item[i].manuf.nNum;

        tmodelInfo.item[i].nChNum = temp[offset++];
        for(j = 0; j < tmodelInfo.item[i].nChNum; j++)
        {
            tmodelInfo.item[i].chInfo[j].nType = temp[offset++];
            tmodelInfo.item[i].chInfo[j].nFunc = temp[offset++];
            tmodelInfo.item[i].chInfo[j].nDriveName.nNum = temp[offset++];
            memcpy(tmodelInfo.item[i].chInfo[j].nDriveName.value, temp + offset, tmodelInfo.item[i].chInfo[j].nDriveName.nNum);
            offset += tmodelInfo.item[i].chInfo[j].nDriveName.nNum;

            if(0 == memcmp(tmodelInfo.item[i].modType.value, "GK", 2) &&
               LCTRL_MAP_FUNC_CTRL == tmodelInfo.item[i].chInfo[j].nFunc && 0 == num)
            {
                port = tmodelInfo.item[i].nSlot;
                subport = j;
                num++;
            }
        }
        
    }

    if(0 == num)
    {
        if(0 != ptLCData->ctrlMode)
        {
            CTRL_FMT_DEBUG("没有控制模组\n");
        }
        ptLCData->ctrlMode = 0;
        lctrl_set_ctrl_mode(0, 0);
    }

    if(port == ptLCData->ctrlMode)
    {
        return 0;
    }

    no = port - 1;
    CTRL_FMT_DEBUG("控制模组序号%d, 型号: %s, ID: %s, 厂商: %s. 通道: %d\n", tmodelInfo.item[no].nSlot,
                       tmodelInfo.item[no].modType.value, tmodelInfo.item[no].modId.value, 
                       tmodelInfo.item[no].manuf.value, subport);

    ptLCData->ctrlMode = port;
    lctrl_set_ctrl_mode(port, subport);
    
    return 0;
}

/**********************************************************************
* @name      : lctrl_mode_event
* @brief     ：查询控制端口号 返回消息处理
* @param[in] ：int cid          数据中心cid
               uint16 label     消息label
               uint8* pMsg      消息
               uint16 msglen    消息长度
* @param[out]：uint32* nextStep 下一个步骤
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-31
* @Update    : 
**********************************************************************/
int lctrl_mode_event(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint32 port = 0;
    uint8  status = 0;
    uint8 *temp = NULL;
    LCTRL_DATA_T    *ptLCData = (LCTRL_DATA_T*)pPublic;
    
    if(NULL == pInMsg || NULL == pArg || 0 == pInMsg->msglen)
    {
        return -1;
    }
    temp = pInMsg->playload;

    appmsg_memcpy_r(&port, temp, sizeof(uint32));
    temp += sizeof(uint32);
    status = *temp;

    CTRL_FMT_DEBUG("模组%d %s\n", port, 0 == status ? "插入" : "拔出");
    if(0 == port && port > OOP_MAX_MODULE)
    {
        return -2;
    }
    
    if(1 == status)
    {
        /* 拔出 */
        if(port == ptLCData->ctrlMode)
        {
            ptLCData->ctrlMode = 0;
            lctrl_set_ctrl_mode(0, 0);
            CTRL_FMT_DEBUG("控制模组拔出\n");
        }
    }
    else
    {
        /* 插入 */
        lctrl_get_allmodule();
    }
    return 0;
}


/**********************************************************************
* @name      : lctrl_check_ctrlOAD
* @brief     ：判断是否一般控制需要更新的OAD
* @param[in] ：uint32 OAD       OAD
* @param[out]：
* @return    ：TRUE-是/FALSE-否
* @Create    : 王津剑
* @Date      ：2024-6-8
* @Update    :
**********************************************************************/
int lctrl_check_ctrlOAD(uint32 OAD)
{
    LCTRL_DB_CHECK_T dblist[] = {
        {0x80000200, 0xf0f0ffff},
        {0x80007fff, 0xFFFFFFFF},
        {0x800081ff, 0xFFFFFFFF},
        {0x800082ff, 0xFFFFFFFF},
        {0x80010300, 0xFFFFFFFF},
        {0x80010400, 0xFFFFFFFF},
        {0x80010500, 0xFFFFFFFF},
        {0x80020300, 0xFFFFFFFF},
        {0x80027fff, 0xFFFFFFFF},
        //0x810306ff 0x810406ff 0x810506ff 0x810606ff 0x810706ff 0x810806ff
        {0x810306ff, 0xfff0ffff},
        {0x81037fff, 0xFFFFFFFF},
        {0x81067fff, 0xFFFFFFFF},

        //0x2301 0x2302 0x2303 0x2304 0x2305 0x2306 0x2307 0x2308  
        {0x23010200, 0xfff0ffff},
        {0x23010300, 0xfff0ffff},
        {0x23010500, 0xfff0ffff},
        {0x23010900, 0xfff0ffff},
        {0x23010e00, 0xfff0ffff},
        {0x23010f00, 0xfff0ffff},
        {0x23011300, 0xfff0ffff},

        {0x40180200, 0xFFFFFFFF},
        {0xf2060400, 0xFFFFFFFF},
        {0xf2050200, 0xFFFFFFFF},
    };
    uint16 dbnum = sizeof(dblist)/sizeof(LCTRL_DB_CHECK_T);
    uint16 i = 0;

    for(i= 0; i < dbnum; i++)
    {
        if((dblist[i].dataflag & dblist[i].maskInfo) == (OAD & dblist[i].maskInfo))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**********************************************************************
* @name      : lctrl_check_ctrlOAD
* @brief     ：判断是否一般控制需要更新的OAD
* @param[in] ：uint32 OAD       OAD
* @param[out]：
* @return    ：TRUE-是/FALSE-否
* @Create    : 王津剑
* @Date      ：2024-6-28
* @Update    :
**********************************************************************/
int lctrl_check_statOAD(uint32 OAD)
{
    LCTRL_DB_CHECK_T dblist[] = {
        {0x24010300, 0xfff0ffff},
        {0x24010400, 0xfff0ffff},
        {0x24011C00, 0xfff0ffff},

        {0x40160200, 0xFFFFFFFF},
    };
    uint16 dbnum = sizeof(dblist)/sizeof(LCTRL_DB_CHECK_T);
    uint16 i = 0;

    for(i= 0; i < dbnum; i++)
    {
        if((dblist[i].dataflag & dblist[i].maskInfo) == (OAD & dblist[i].maskInfo))
        {
            return TRUE;
        }
    }
    return FALSE;
}


/**********************************************************************
* @name      : lctrl_appmsg_dbchange
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-6-28
* @Update    :
**********************************************************************/
int lctrl_appmsg_dbchange(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    LCTRL_DATA_T *ptLAData = (LCTRL_DATA_T*)pPublic;
    uint8     *temp = NULL;
    uint16     offset = 0;
    uint8      logic = 0;
    uint16     info = 0;
    uint16     classtag = 0;
    uint32     mainOAD = 0;
    uint32     slaveOAD = 0;
    uint8      oadNum = 0;
    uint8      i = 0;

    temp = pInMsg->playload;

    logic = temp[offset++];
    MEMCPY_R(&info, temp + offset, 2);
    offset += 2;
    MEMCPY_R(&classtag, temp + offset, 2);
    offset += 2;
    if(0 != logic || CLASS_DATA_EVENT == classtag || CLASS_DATA_COLLEC == classtag)
    {
        return 0;
    }
    
    if(1 == temp[offset++])
    {
        MEMCPY_R(&mainOAD, temp + offset, 4);
        offset += 4;
    }
    oadNum = temp[offset++];
    for(i = 0; i < oadNum; i++)
    {
        MEMCPY_R(&slaveOAD, temp + offset, 4);
        offset += 4;
        if(TRUE == lctrl_check_ctrlOAD(slaveOAD))
        {
            lctrl_write_oad_notify(&(ptLAData->ctrlDbChg), slaveOAD, info);
        }
        if(TRUE == lctrl_check_statOAD(slaveOAD))
        {
            lctrl_write_oad_notify(&(ptLAData->statDbChg), slaveOAD, info);
        }
    }

    return 0;
}

/**********************************************************************
* @name      : lctrl_appmsg_dbinit
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-6-28
* @Update    :
**********************************************************************/
int lctrl_appmsg_dbinit(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    LCTRL_DATA_T *ptLAData = (LCTRL_DATA_T*)pPublic;
    uint8     *temp = NULL;
    uint8      type = 0;
    uint8      logic = 0;
    uint16     classtag = 0;

    temp = pInMsg->playload;
    type = *temp;
    if(0 == type)
    {
        /* 数据初始化 */
        temp++;
        logic = *temp;
        temp++;
        if(*temp)
        {
            temp++;
            MEMCPY_R(&classtag, temp, 2);
        }
        if(0 == logic &&(0 == classtag || CLASS_DATA_NORMAL == classtag))
        {
            lctrl_set_dataflag(&(ptLAData->ctrlDbChg));
            lctrl_set_dataflag(&(ptLAData->statDbChg));
        }
        if(0 == logic &&(0 == classtag || CLASS_DATA_INIT == classtag))
        {
            lctrl_set_argflag(&(ptLAData->ctrlDbChg));
            lctrl_set_argflag(&(ptLAData->statDbChg));
        }
    }
    else if(0 == type)
    {
        /* 恢复出厂参数 */
        temp++;
        logic = *temp;
        if(0 == logic)
        {
            lctrl_set_argflag(&(ptLAData->ctrlDbChg));
            lctrl_set_argflag(&(ptLAData->statDbChg));
        }
    }
    
    return 0;
}

/**********************************************************************
* @name      : lctrl_gui_hint
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-6-11
* @Update    :
**********************************************************************/
int lctrl_gui_hint(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    CTRL_FMT_DEBUG("弹显被按键取消\n");
    appctrl_set_key(TRUE);
    return 0;
}

#if DESC("负荷控制消息处理" ,1)
/**********************************************************************
* @name      : lctrl_IOP_msg_alarm
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-7-4
* @Update    :
**********************************************************************/
int lctrl_IOP_msg_alarm(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8 status = 0;
    uint8 bRet = 0;

    status = *pInMsg->playload;
    CTRL_FMT_DEBUG("告警操作%s\n", 0 == status ? "触发" : "解除");

    lctrl_send_backmsg(pInMsg, &bRet, 1);
    return 0;
}

/**********************************************************************
* @name      : lctrl_IOP_msg_trip
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-7-4
* @Update    :
**********************************************************************/
int lctrl_IOP_msg_trip(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    LCTRL_DATA_T    *ptLCData = (LCTRL_DATA_T*)pPublic;
    uint8            num = 0;
    uint8            i = 0;
    TELECON_TRIP     trip;
    uint8           *temp = NULL;
    uint16           offset = 0;
    uint8            bRet = 0;

    if(0 != ptLCData->protectStatus)
    {
        MQTTMSG_FMT_DEBUG("保电状态，不执行跳闸\n");
        lctrl_send_backmsg(pInMsg, &bRet, 1);
    }
    
    memset(&trip, 0, sizeof(TELECON_TRIP));

    temp = pInMsg->playload;
    num = temp[offset++];
    for(i = 0; i < num; i++)
    {
        MEMCPY_R(&trip.nRelayIdx.value, temp + offset, 4);
        offset += 4;

        trip.nWarnDelay = temp[offset++];

        MEMCPY_R(&trip.nWarnDelay, temp + offset, 2);
        offset += 2;

        trip.bAutoClose = temp[offset++];

        set_remote_state_valid(trip.nRelayIdx.value & 0xFF, 1, &trip, NULL);
        MQTTMSG_FMT_DEBUG("遥控跳闸: %08X, 告警时间: %dmin, 限电时间: %d, %s自动合闸\n", trip.nRelayIdx.value,
                          trip.nWarnDelay, trip.nWarnDelay, trip.bAutoClose ? "" : "非");
    }
    bRet = 1;
    lctrl_send_backmsg(pInMsg, &bRet, 1);
    return 0;
}

/**********************************************************************
* @name      : lctrl_IOP_msg_close
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-7-4
* @Update    :
**********************************************************************/
int lctrl_IOP_msg_close(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8            num = 0;
    uint8            i = 0;
    TELECON_CLOSE    close;
    uint8           *temp = NULL;
    uint16           offset = 0;
    uint8            bRet = 1;

    memset(&close, 0, sizeof(TELECON_CLOSE));

    temp = pInMsg->playload;
    num = temp[offset++];
    for(i = 0; i < num; i++)
    {
        MEMCPY_R(&close.nRelayIdx.value, temp + offset, 4);
        offset += 4;
        close.nState = temp[offset++];

        set_remote_state_valid(close.nRelayIdx.value & 0xFF, 2 + close.nState, NULL, &close);
        MQTTMSG_FMT_DEBUG("遥控合闸: %08X, %s合闸\n", close.nRelayIdx.value,
                          0 == close.nState ? "允许" : "直接");
    }

    lctrl_send_backmsg(pInMsg, &bRet, 1);
    return 0;
}

/**********************************************************************
* @name      : lctrl_IOP_msg_select
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-7-4
* @Update    :
**********************************************************************/
int lctrl_IOP_msg_select(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8  status = 0;
    uint32 OAD= 0;
    uint8  bRet = 0;

    status = *pInMsg->playload;
    MEMCPY_R(&OAD, pInMsg->playload + 1, 4);
    CTRL_FMT_DEBUG("%s遥控单元%08X\n", 0 == status ? "选择" : "撤销", OAD);

    lctrl_send_backmsg(pInMsg, &bRet, 1);
    return 0;
}

/**********************************************************************
* @name      : lctrl_IOP_msg_protect
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-7-15
* @Update    :
**********************************************************************/
int lctrl_IOP_msg_protect(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8  status = 0;
    char  *pname[3] = {"投入", "解除", "解除自动保电"};
    uint8  bRet = 0;

    status = *pInMsg->playload;
    CTRL_FMT_DEBUG("保电操作\n", status < 3 ? pname[status] : "未知");

    set_guard_state(1 + status);

    bRet = status < 3 ? 1 : 0;
    lctrl_send_backmsg(pInMsg, &bRet, 1);
    return 0;
}

/**********************************************************************
* @name      : lctrl_IOP_msg_fee_input
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-7-15
* @Update    :
**********************************************************************/
int lctrl_IOP_msg_fee_input(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    OOP_CALLFEE_T  param;
    uint8         *temp = 0;
    uint16         offset = 0;
    uint8          bytesnum = 0;
    uint16         len = 0;
    uint8  bRet = 1;

    temp = pInMsg->playload;
    param.period[0] = temp[offset++];
    param.period[1] = temp[offset++];
    param.period[2] = temp[offset++];

    GET_LEN_OFFSET(len, temp + offset, bytesnum);
    offset += bytesnum;
    
    param.nLen = len;
    memcpy(param.info, temp + offset, MIN(200, len));
    
    MQTTMSG_FMT_DEBUG("催费告警投入%08X%08X%08X\n", param.period[0], param.period[1], param.period[2]);
    set_fee_state(1, &param);

    lctrl_send_backmsg(pInMsg, &bRet, 1);
    return 0;
}

/**********************************************************************
* @name      : lctrl_IOP_msg_fee_remove
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-7-15
* @Update    :
**********************************************************************/
int lctrl_IOP_msg_fee_remove(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8  bRet = 1;

    MQTTMSG_FMT_DEBUG("催费告警解除\n");
    set_fee_state(2, NULL);
    
    lctrl_send_backmsg(pInMsg, &bRet, 1);
    return 0;
}



#endif

LCTRL_MSGFUN_T g_tLcAppMsgFunc[] = 
{/*  IID,                    IOP,                          rpm, 消息处理函数 */
    {MESSAGE_IID_COMMON,     COMMON_IOP_URGENT_POWERDOWN,  1,   lctrl_appmsg_power_off},
    {MESSAGE_IID_COMMON,     COMMON_IOP_SYSTEM_POWERON,    1,   lctrl_appmsg_power_on},
    {MESSAGE_IID_COMMON,     COMMON_IOP_SYSTEM_RESTART,    1,   lctrl_appmsg_power_restart},
    {MESSAGE_IID_COMMON,     COMMON_IOP_HERTBEAT,          1,   lctrl_heart_bear},
    {MESSAGE_IID_COMMON,     COMMON_IOP_VERSION,           0,   lctrl_version_msg},

    {MESSAGE_IID_MODULE,     MODULE_IOP_STATUS_EVENT,      1,   lctrl_mode_event},
    {MESSAGE_IID_MODULE,     MODULE_IOP_GET_ALL,           0,   lctrl_modport_ack},


    {MESSAGE_IID_DB,         DB_IOP_UPDATE_EVENT,          1,   lctrl_appmsg_dbchange},
    {MESSAGE_IID_DB,         DB_IOP_INIT_EVENT,            1,   lctrl_appmsg_dbinit},

    {MESSAGE_IID_DESKGUI,    DESKGUI_IOP_PUSH_HINT,        0,   lctrl_gui_hint},

    {MESSAGE_IID_RSPSAMPLE,  RSPSAMPLE_IOP_PULSE_STATIC,   0,   lctrl_appmsg_recv_sync},

    {MESSAGE_IID_LOADCTRL,   LOADCTRL_IOP_ALARM,           0,   lctrl_IOP_msg_alarm},
    {MESSAGE_IID_LOADCTRL,   LOADCTRL_IOP_TRIP,            0,   lctrl_IOP_msg_trip},
    {MESSAGE_IID_LOADCTRL,   LOADCTRL_IOP_CLOSE,           0,   lctrl_IOP_msg_close},
    {MESSAGE_IID_LOADCTRL,   LOADCTRL_IOP_SELECT,          0,   lctrl_IOP_msg_select},
    {MESSAGE_IID_LOADCTRL,   LOADCTRL_IOP_PROTECT,         0,   lctrl_IOP_msg_protect},
    {MESSAGE_IID_LOADCTRL,   LOADCTRL_IOP_FEE_INPUT,       0,   lctrl_IOP_msg_fee_input},
    {MESSAGE_IID_LOADCTRL,   LOADCTRL_IOP_FEE_REMOVE,      0,   lctrl_IOP_msg_fee_remove},
    {MESSAGE_IID_LOADCTRL,   LOADCTRL_IOP_PERIOD_CHANGE,   0,   NULL},
    {MESSAGE_IID_LOADCTRL,   LOADCTRL_IOP_INPUT,           0,   NULL},
    {MESSAGE_IID_LOADCTRL,   LOADCTRL_IOP_REMOVE,          0,   NULL},
    {MESSAGE_IID_LOADCTRL,   LOADCTRL_IOP_RELAY_MODE,      0,   NULL},
};
uint8 g_nLcAppMsgNum = sizeof(g_tLcAppMsgFunc)/sizeof(LCTRL_MSGFUN_T);

LCTRL_SYNCMSG_T g_lctrlAppSync[LCTRL_APP_SYNC_NUM];

/**********************************************************************
* @name      : lctrl_send_recv_appmsg
* @brief     ：同步消息收发
* @param[in] ：uint8 frameno        通道序号(0,1-2 通道)
               uint16 IID          IID
               uint16 IOP          IOP
               uint8 *pSendMsg     发送数据
               uint16 sendlen      发送长度
               uint16 buflen       接收缓存大小
               uint8 timeout       超时时间(s)
* @param[out]：uint8 *pRecvbuf      接收内容
               uint16 *pRecvlen    接收消息长度
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-6-28
* @Update    :
**********************************************************************/
int lctrl_send_recv_appmsg(uint8 frameno, uint16 IID, uint16 IOP, uint8 *pSendMsg, uint16 sendlen, 
                                      uint8 *pRecvbuf, uint16 buflen, uint16 *pRecvlen, uint8 timeout)
{
    uint8           sendbuf[LCTRL_APPMSG_MAXLEN] = {0};
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
    struct timespec outtime;
    int             rettime = 0;

    if(sendlen > 4096 || frameno >= LCTRL_APP_SYNC_NUM)
    {
        return -2;
    }
    
    pMsgInfo = (MESSAGE_INFO_T *)sendbuf;
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 0;

    pMsgInfo->index = lctrl_get_next_index();
    pMsgInfo->label = frameno;
    
    pMsgInfo->sourlen = strlen(LCTRL_APP_NAME);
    memcpy(pMsgInfo->souraddr, LCTRL_APP_NAME, pMsgInfo->sourlen);

    switch(IID)
    {
        case MESSAGE_IID_MODULE:
        {
            pMsgInfo->destlen = strlen("M-mapManager");
            memcpy(pMsgInfo->destaddr, "M-mapManager", pMsgInfo->destlen);
            sprintf(uTopic, "%s/%s", LCTRL_APP_NAME, "M-mapManager");
            break;
        }
        case MESSAGE_IID_RSPSAMPLE:
        {
            pMsgInfo->destlen = strlen("M-rspSample");
            memcpy(pMsgInfo->destaddr, "M-rspSample", pMsgInfo->destlen);
            sprintf(uTopic, "%s/%s", LCTRL_APP_NAME, "M-rspSample");
            break;
        }

        default: return -3;
    }

    pMsgInfo->IID = IID;
    pMsgInfo->IOP = IOP;

    pMsgInfo->msglen = sendlen;
    if(sendlen > 0)
    {
        memcpy(pMsgInfo->playload, pSendMsg, sendlen);
    }

    pthread_mutex_lock(&g_lctrlAppSync[frameno].lock);

    ret = lctrl_send_msg(uTopic, pMsgInfo, 0);
    if(0 != ret)
    {
        pthread_mutex_unlock(&g_lctrlAppSync[frameno].lock);
        return ret;
    }

    g_lctrlAppSync[frameno].bWait = TRUE;
    g_lctrlAppSync[frameno].IID = IID;
    g_lctrlAppSync[frameno].IOP = IOP;
    g_lctrlAppSync[frameno].index = pMsgInfo->index;
    g_lctrlAppSync[frameno].recvlen = 0;
    memset(g_lctrlAppSync[frameno].recv, 0, LCTRL_APPMSG_MAXLEN);

    clock_gettime(CLOCK_MONOTONIC, &outtime);
    outtime.tv_sec += timeout;
    rettime = pthread_cond_timedwait(&g_lctrlAppSync[frameno].notify, &g_lctrlAppSync[frameno].lock, &outtime);
    if(0 != rettime)
    {
        /* 超时 */
        g_lctrlAppSync[frameno].bWait = FALSE;
        pthread_mutex_unlock(&g_lctrlAppSync[frameno].lock);
        return 4099;
    }

    g_lctrlAppSync[frameno].bWait = FALSE;
    memcpy(pRecvbuf, g_lctrlAppSync[frameno].recv, MIN(g_lctrlAppSync[frameno].recvlen, buflen));
    *pRecvlen = MIN(g_lctrlAppSync[frameno].recvlen, buflen);
    pthread_mutex_unlock(&g_lctrlAppSync[frameno].lock);

    return 0;
}

/**********************************************************************
* @name      : lctrl_appmsg_recv_sync
* @brief     ：接收处理
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-6-28
* @Update    :
**********************************************************************/
int lctrl_appmsg_recv_sync(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    int ret = 0;
    uint32 ustime = 5;
    uint8 frameno = pInMsg->label;

    if(frameno >= LCTRL_APP_SYNC_NUM)
    {
        return -1;
    }
    
    /* 2s加锁失败放弃消息 防止外部消息卡死 */
    if(0 != pthread_mutex_trylock(&(g_lctrlAppSync[frameno].lock)))
    {
        while(ustime < 1000000)
        {
            usleep(ustime);
            if(0 == pthread_mutex_trylock(&(g_lctrlAppSync[frameno].lock)))
            {
                goto lock;
            }
            ustime *= 10;
        }
        usleep(500000);
        if(0 != pthread_mutex_trylock(&(g_lctrlAppSync[frameno].lock)))
        {
            return -1;
        }
    }

lock:
    if(FALSE == g_lctrlAppSync[frameno].bWait || g_lctrlAppSync[frameno].index != pInMsg->index ||
       g_lctrlAppSync[frameno].IID != pInMsg->IID || g_lctrlAppSync[frameno].IOP != pInMsg->IOP)
    {
        pthread_mutex_unlock(&g_lctrlAppSync[frameno].lock);
        return 4099;
    }

    g_lctrlAppSync[frameno].recvlen = pInMsg->msglen;
    memcpy(g_lctrlAppSync[frameno].recv, pInMsg->playload, pInMsg->msglen);
    g_lctrlAppSync[frameno].bWait = FALSE;
    
    pthread_cond_signal(&(g_lctrlAppSync[frameno].notify));
    pthread_mutex_unlock(&(g_lctrlAppSync[frameno].lock));
    return ret;
}


/**********************************************************************
* @name      : loadctrl_mqttmsg_init
* @brief     ：mqtt消息处理初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-6-6
* @Update    :
**********************************************************************/
int loadctrl_mqttmsg_init(void *pPublic, void **ppPrivate)
{
    memset(&g_appmsg, 0, sizeof(LCTRL_APPMSG_T));
    
    /* 创建db_agent */
    g_dbAgent = db_agent_create(AGENT_TYPE_THREAD, DB_LOADCTRLLER_AGENT, lctrl_send_dbmsg, 5);
    {
        if(g_dbAgent < 0)
        {
            return -1;
        }
    }

    *ppPrivate = &g_appmsg;
    return 0;
}

/**********************************************************************
* @name      : la_mqttmsg_proc
* @brief     ：mqtt消息处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2024-6-6
* @Update    :
**********************************************************************/
int loadctrl_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int         ret = 0;
    int         recvlen = 0;
    uint8       i;
    LCTRL_APPMSG_T  *ptHandle = (LCTRL_APPMSG_T*)pPrivate;
    LCTRL_DATA_T    *ptLCData = (LCTRL_DATA_T*)pPublic;
    MESSAGE_INFO_T  *pMsgInfo = NULL;
    struct timespec  curtime = {0};

    /* 1 收消息 */
    recvlen = lctrl_recv_msg(ptHandle->topic, ptHandle->recv, LCTRL_APPMSG_MAXLEN);
    if(recvlen > 0)
    {
        ret = appmsg_split(ptHandle->recv, (uint16)recvlen, &pMsgInfo);
        if(0 == ret && NULL != pMsgInfo)
        {
            /* 用于线程维护日志记录 */
            TASK_SET_USERLOG(pArg, "mqtt msg from %s iid %04x, iop %04x", pMsgInfo->souraddr, pMsgInfo->IID, 
                                   pMsgInfo->IOP);
            MQTTMSG_BUF_TRACE(ptHandle->recv, recvlen, "Recv msg from %s, iid 0x%x, iop 0x%x", 
                              pMsgInfo->souraddr, pMsgInfo->IID, pMsgInfo->IOP);
            if(0 == strcmp(pMsgInfo->souraddr, LCTRL_APP_NAME))
            {
                /* 消除自发自收 */
                MQTTMSG_FMT_DEBUG("Revc msg form %s is the same name, iid %d, iop %d\n",
                              pMsgInfo->souraddr, pMsgInfo->IID, pMsgInfo->IOP);
                goto freemsg;
            }

            /* 注册应答 */
            if(MESSAGE_IID_SYSTEM == pMsgInfo->IID && SYSTEM_IOP_APP_REGISTER == pMsgInfo->IOP &&
               0 == strcmp(pMsgInfo->destaddr, LCTRL_APP_NAME))
            {
                if(FALSE == ptHandle->bReg)
                {
                    ptHandle->bReg = TRUE;
                    MQTTMSG_FMT_DEBUG("注册成功\n");
                    BASE_TASK_SET_OK(pArg); //基础任务完成准备工作
                    goto freemsg;
                }
            }

            /* 数据中心 */
            ret = db_agent_msg_entry(g_dbAgent, pMsgInfo);
            if(DB_AGENT_UNKONW_MSG != ret)
            {
                if(0 != ret)
                {
                    MQTTMSG_BUF_DEBUG(ptHandle->recv, recvlen, "Drop msg from %s, ret %d:", pMsgInfo->souraddr, ret);
                }
                goto freemsg;
            }

            /* 显示 */
            if(pMsgInfo->IID == MESSAGE_IID_DESKGUI)
            {
                appGUI_recv_back_handle(pMsgInfo);
            }

            for(i = 0; i < g_nLcAppMsgNum; i++)
            {
                if(g_tLcAppMsgFunc[i].IID == pMsgInfo->IID && g_tLcAppMsgFunc[i].IOP == pMsgInfo->IOP && 
                   g_tLcAppMsgFunc[i].rpm == pMsgInfo->rpm)
                {
                    if(g_tLcAppMsgFunc[i].pProcMsg)
                    {
                        g_tLcAppMsgFunc[i].pProcMsg(pMsgInfo, (void *)ptHandle, (void *)ptLCData);
                        goto freemsg;
                    }
                }
            }

freemsg:
            free(pMsgInfo);
            pMsgInfo = NULL;
        }
    }

    /* 2. 定时处理 */
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    if(curtime.tv_sec - ptHandle->timeflag < 5)
    {
        return 0;
    }
    ptHandle->timeflag = curtime.tv_sec;
    if(FALSE == ptHandle->bReg)
    {
        lctrl_login_to_smios(ptLCData->appid, ptLCData->appidlen);
    }
    else if(FALSE == ptLCData->hasDB)
    {
        lctrl_get_dbCenter();
    }
#ifdef PRODUCT_ECU
    else if(FALSE == ptLCData->hasMap)
    {
        lctrl_get_mapManager();
    }
    else if(0 == ptLCData->ctrlMode)
    {
        lctrl_get_allmodule();
    }
#endif
    
    return 10000;
}

