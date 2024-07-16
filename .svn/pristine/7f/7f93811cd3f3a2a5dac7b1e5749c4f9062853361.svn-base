/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块主线程功能
* @date：    2019-10-23
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "hal.h"
#include "remote_cu.h"
#include "uart.h"
#include "appmsg_split.h"
#include "gui_c_prtl.h"
#include "task_base.h"

#include "wireless.h"
#include "gprs_gsm.h"
#include "wire_mqtt.h"
#include "msg_proc.h"
#include "at_oper.h"

WIRELESS_MANAGER_T g_Manager;

uint8 Isusb=0;

#if DESC("MQTT消息处理", 1)

/**********************************************************************
* @name      : wireless_get_versionmsg
* @brief     ：获取版本
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-5-2
* @Update    :
**********************************************************************/
int wireless_app_versionmsg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    WIRELESS_DATA_T *ptWireData = (WIRELESS_DATA_T*)pPublic;

    if(NULL == pInMsg || NULL == pArg)
    {
        return -1;
    }
    
    if(NULL != strstr(pInMsg->souraddr, "mapManager"))
    {
        MQTTMSG_FMT_DEBUG("获取到模组管理器\n");
        ptWireData->mapReg = TRUE;
    }
    if(NULL != strstr(pInMsg->souraddr, "desktopGui"))
    {
        MQTTMSG_FMT_DEBUG("获取到桌面APP\n");
        ptWireData->guiReg = TRUE;
    }
    return 0;
}

/**********************************************************************
* @name      : wireless_get_appidmsg
* @brief     ：获取版本
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-5-2
* @Update    :
**********************************************************************/
int wireless_get_appidmsg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    char         uTopic[256] = {0};
    uint8           buf[360] = {0};
    uint8              *temp = NULL;
    MESSAGE_INFO_T *ptMsgNew = (MESSAGE_INFO_T*)buf;
    WIRELESS_DATA_T *ptWireData = (WIRELESS_DATA_T*)pPublic;

    if(NULL == pInMsg || NULL == pArg)
    {
        return -1;
    }
    
    /* 构造返回消息 */
    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pInMsg->priority;
    ptMsgNew->index = pInMsg->index;
    ptMsgNew->label = pInMsg->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptMsgNew->destlen = pInMsg->sourlen;

    ptMsgNew->IID = pInMsg->IID;
    ptMsgNew->IOP = pInMsg->IOP;

    temp = ptMsgNew->playload;

    /* APPID */
    *temp = ptWireData->appidlen;
    if(ptWireData->appidlen > 0)
    {
        memcpy(temp + 1, ptWireData->appid, ptWireData->appidlen);
    }

    ptMsgNew->msglen = ptWireData->appidlen + 1;
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pInMsg->souraddr);
    return wireless_send_msg(uTopic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : wireless_transport_proc
* @brief     ：透传(是针对模块1)
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-1-25
* @Update    :
**********************************************************************/
int wireless_transport_proc(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    int                   ret = 0;
    WIRELESS_QUEUE_T     *ptMsgQueue = NULL;
    uint8                *temp = NULL;
    uint32                msgLen = 0;
    WIRELESS_DATA_T *ptWireData = (WIRELESS_DATA_T*)pPublic;

    if(NULL == pMsg || NULL == pArg)
    {
        return -1;
    }

    /* 检查 */
    if(pMsg->msglen <= 3)
    {
        return wireless_send_errmsg(pMsg, -1);
    }
    
    temp = pMsg->playload;
    if(1 == *temp && ptWireData->atdata0.stepStatus >= GSM_CPIN && TRUE == ptWireData->atdata0.workFlag)
    {
        ptMsgQueue = ptWireData->atdata0.ptMsgQueue;
    }
    else if(2 == *temp && ptWireData->atdata1.stepStatus >= GSM_CPIN && TRUE == ptWireData->atdata1.workFlag)
    {
        ptMsgQueue = ptWireData->atdata1.ptMsgQueue;
    }
    
    if(ptMsgQueue)
    {
        /* 写入AT线程缓存队列 */
        msgLen = sizeof(MESSAGE_INFO_T) + pMsg->msglen;
        if(0 != write_queue(ptMsgQueue, (uint8 *)pMsg, msgLen))
        {
            MQTTMSG_FMT_DEBUG("wireless_sendsms_req write_queue(%d) failed\n", *(pMsg->playload));
            ptMsgQueue = NULL;
        }
    }
    
    if(NULL == ptMsgQueue)
    {
        MESSAGE_INFO_T   tMsgNew;
        char             uTopic[256] = {0};
        
        /* 构造返回消息 */
        memset(&tMsgNew, 0, sizeof(MESSAGE_INFO_T));
        tMsgNew.rpm = 0;
        tMsgNew.priority = pMsg->priority;
        tMsgNew.index = pMsg->index;
        tMsgNew.label = pMsg->label;

        tMsgNew.sourlen = strlen(WIRELESS_DCM_NAME);
        memcpy(tMsgNew.souraddr, WIRELESS_DCM_NAME, tMsgNew.sourlen);

        memcpy(tMsgNew.destaddr, pMsg->souraddr, pMsg->sourlen);
        tMsgNew.destlen = pMsg->sourlen;

        tMsgNew.IID = pMsg->IID;
        tMsgNew.IOP = pMsg->IOP;

        tMsgNew.msglen = 0;
        
        sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
        ret = wireless_send_msg(uTopic, &tMsgNew, 0);
    }
    return ret;
}

/**********************************************************************
* @name      : wireless_dcmmodule_req
* @brief     ：无线模块查询消息处理函数
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic          全局数据
* @param[out]：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-29
* @Update    : 2020-1-9
**********************************************************************/
int wireless_dcmmodule_req(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    int             ret = 0;
    uint8           dcmID = 0;
    uint8           modID = 0;
    MESSAGE_INFO_T *ptMsgNew = NULL;
    NET_PARA_T     *ptNetPara = NULL;   /* 网络参数 */
    WIRELESS_INFO_T tInfo;
    uint8           playLoad[512] = {0};
    uint32          bufLen = 0;
    uint32          msgLen = 0;
    WIRELESS_DATA_T *ptWireData = (WIRELESS_DATA_T*)pPublic;
    char             uTopic[256] = {0};

    if(NULL == pMsg || NULL == pArg)
    {
        return -1;
    }

    if(1 != pMsg->msglen)
    {
        MQTTMSG_FMT_DEBUG("查询模块信息长度 %d 错误\n", pMsg->msglen);
        return wireless_send_errmsg(pMsg, -1);
    }
    dcmID = *(pMsg->playload);
    memset(&tInfo, 0, sizeof(WIRELESS_INFO_T));
    if(1 == dcmID || 2 == dcmID)
    {
        if(1 == dcmID)
        {
            modID = g_Manager.DCM0Port;
            ptNetPara = ptWireData->pppdata0.ptNetPara;
        }
        if(2 == dcmID)
        {
            modID = g_Manager.DCM1Port;
            ptNetPara = ptWireData->pppdata1.ptNetPara;
        }

        /* 插入状态 */
        if(WIRELESS_UNUSED_COMMPORT != modID)
        {
            tInfo.instatus = TRUE;
        }
        memcpy(tInfo.version.manuNum, ptNetPara->VERINFO.manuNum, 4);
        memcpy(tInfo.version.manuExten, ptNetPara->VERINFO.manuExten, 8);
        memcpy(tInfo.version.softversion, ptNetPara->VERINFO.softversion, 4);
        memcpy(tInfo.version.softdata, ptNetPara->VERINFO.softdata, 6);
        memcpy(tInfo.version.hwversion, ptNetPara->VERINFO.hwversion, 4);
        memcpy(tInfo.version.hwdata, ptNetPara->VERINFO.hwdata, 6);

        /* 模块类型信息 */
        tInfo.typeinfo.num = 2;
        memcpy(tInfo.typeinfo.value, ptNetPara->nettype, 2);

        tInfo.IMEI.num = strlen(ptNetPara->IMEI);
        if(tInfo.IMEI.num > 0)
        {
            memcpy(tInfo.IMEI.value, ptNetPara->IMEI, tInfo.IMEI.num);
        }
    }
    else
    {
        MQTTMSG_FMT_DEBUG("查询模块信息未知设备号 %d\n", dcmID);
    }

    bufLen = appmsg_wireinfo_package(&tInfo, playLoad, 512);

    msgLen = sizeof(MESSAGE_INFO_T) + bufLen;
    ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
    if(NULL == ptMsgNew)
    {
        MQTTMSG_FMT_DEBUG("wireless_dcmmodule_req malloc failed\n");
        return -1;
    }
    memset(ptMsgNew, 0, msgLen);

    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pMsg->priority;
    ptMsgNew->index = pMsg->index;
    ptMsgNew->label = pMsg->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
    ptMsgNew->destlen = pMsg->sourlen;

    ptMsgNew->IID = pMsg->IID;
    ptMsgNew->IOP = pMsg->IOP;

    ptMsgNew->msglen = bufLen;
    memcpy(ptMsgNew->playload, playLoad, bufLen);
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
    ret = wireless_send_msg(uTopic, ptMsgNew, 0);
    free(ptMsgNew);
    return ret;
}

/**********************************************************************
* @name      : wireless_dial_req
* @brief     ：无线模块查询拨号状态消息处理函数
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic          全局数据
* @param[out]：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-29
* @Update    : 2020-1-9
**********************************************************************/
int wireless_dial_req(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    int              ret = 0;
    uint8            dcmID = 0;
    uint8            modID = 0;
    WIRE_DIAG_INFO_T tInfo;
    uint8            playLoad[1024] = {0};
    uint32           bufLen = 0;
    MESSAGE_INFO_T  *ptMsgNew = NULL;
    NET_PARA_T      *ptNetPara = NULL;   /* 网络参数 */
    WIRELESS_HANDLE_T *ptPPPHandle = NULL;
    uint32             msgLen = 0;
    WIRELESS_DATA_T   *ptWireData = (WIRELESS_DATA_T*)pPublic;
    char               uTopic[256] = {0};

    if(NULL == pMsg || NULL == pArg)
    {
        return -1;
    }

    if(1 != pMsg->msglen)
    {
        MQTTMSG_FMT_DEBUG("查询模块拨号状态消息长度 %d\n", pMsg->msglen);
        return wireless_send_errmsg(pMsg, -1);
    }
    dcmID = *(pMsg->playload);

    if(1 == dcmID)
    {
        modID = g_Manager.DCM0Port;
        ptNetPara = ptWireData->pppdata0.ptNetPara;
        ptPPPHandle = &ptWireData->pppdata0;
    }
    else if(2 == dcmID)
    {
        modID = g_Manager.DCM1Port;
        ptNetPara = ptWireData->pppdata1.ptNetPara;
        ptPPPHandle = &ptWireData->pppdata1;
    }
    else
    {
        MQTTMSG_FMT_DEBUG("查询模块拨号状态未知设备号 %d\n", dcmID);
    }

    memset(&tInfo, 0, sizeof(WIRE_DIAG_INFO_T));
    tInfo.simstatus = PPP_DAIL_STATUS_OFFLINE;
    tInfo.diagstatus = PPP_DAIL_STATUS_OFFLINE;
    if(WIRELESS_UNUSED_COMMPORT != modID && (1 == dcmID || 2 == dcmID))
    {
        tInfo.simstatus = 1 - ptPPPHandle->bsimOK; //SIM卡状态
        tInfo.diagstatus = ptPPPHandle->dialState;    //拨号状态

        tInfo.CCID.num = strlen((char*)ptNetPara->ICCID);
        memcpy(tInfo.CCID.value, ptNetPara->ICCID, tInfo.CCID.num);

        tInfo.IMSI.num = strlen((char*)ptNetPara->IMSI);
        memcpy(tInfo.IMSI.value, ptNetPara->IMSI, tInfo.IMSI.num);

        tInfo.phonenum.num = strlen((char*)ptNetPara->SIMNum);
        if(tInfo.phonenum.num > 0)
        {
            memcpy(tInfo.phonenum.value, ptNetPara->SIMNum, tInfo.phonenum.num);//手机号
        }
        tInfo.csqvalue = (uint8)ptNetPara->csqvalue;  //信号强度
        tInfo.operat = ptNetPara->operat;           //运营商
        tInfo.networkType = ptNetPara->networkType;   //网络制式
        
        tInfo.IP[0] = (ptNetPara->ipAddr >> 24) & 0xff;
        tInfo.IP[1] = (ptNetPara->ipAddr >> 16) & 0xff;
        tInfo.IP[2] = (ptNetPara->ipAddr >> 8) & 0xff;
        tInfo.IP[3] = ptNetPara->ipAddr & 0xff;

        tInfo.MASK[0] = (ptNetPara->mask >> 24) & 0xff;
        tInfo.MASK[1] = (ptNetPara->mask >> 16) & 0xff;
        tInfo.MASK[2] = (ptNetPara->mask >> 8) & 0xff;
        tInfo.MASK[3] = ptNetPara->mask & 0xff;

        tInfo.GETWAY[0] = (ptNetPara->getway >> 24) & 0xff;
        tInfo.GETWAY[1] = (ptNetPara->getway >> 16) & 0xff;
        tInfo.GETWAY[2] = (ptNetPara->getway >> 8) & 0xff;
        tInfo.GETWAY[3] = ptNetPara->getway & 0xff;

        /* 拨号所选APN */
        if(PPP_DAIL_STATUS_ONLINE == ptPPPHandle->dialState)
        {
            tInfo.APN.nNum = ptPPPHandle->apn.nNum;
            memcpy(tInfo.APN.value, ptPPPHandle->apn.value, tInfo.APN.nNum);
        }
    }
    else
    {
        tInfo.simstatus = 0;
        tInfo.operat = WIRELESS_OPT_NONE;     //运营商
        tInfo.networkType = OOP_NET_UNKNOW;   //网络制式
    }

    bufLen = appmsg_wirediag_package(&tInfo, playLoad, 1024);

    msgLen = sizeof(MESSAGE_INFO_T) + bufLen;
    ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
    if(NULL == ptMsgNew)
    {
        return -1;
    }
    memset(ptMsgNew, 0, msgLen);

    ptMsgNew->rpm= 0;
    ptMsgNew->priority = pMsg->priority;
    ptMsgNew->index = pMsg->index;
    ptMsgNew->label = pMsg->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
    ptMsgNew->destlen = pMsg->sourlen;

    ptMsgNew->IID = pMsg->IID;
    ptMsgNew->IOP = pMsg->IOP;

    ptMsgNew->msglen = bufLen;
    memcpy(ptMsgNew->playload, playLoad, bufLen);

    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
    ret = wireless_send_msg(uTopic, ptMsgNew, 0);
    free(ptMsgNew);
    return ret;
}

/**********************************************************************
* @name      : wireless_sendsms_req
* @brief     ：无线模块发送短信处理函数
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic        全局数据
* @param[out]：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-29
* @Update    :
**********************************************************************/
int wireless_sendsms_req(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    int               ret = 0;
    WIRELESS_QUEUE_T *ptMsgQueue = NULL;
    uint32            msgLen = 0;
    uint8             retbool = 1;
    MESSAGE_INFO_T   *ptMsgNew = NULL;
    uint8             dcmID = 0;
    char              uTopic[256] = {0};
    WIRELESS_DATA_T  *ptWireData = (WIRELESS_DATA_T*)pPublic;

    if(NULL == pMsg || NULL == pArg)
    {
        return -1;
    }

    if(pMsg->msglen < 4)
    {
        MQTTMSG_FMT_DEBUG("发送短信消息长度 %d\n", pMsg->msglen);
        return wireless_send_errmsg(pMsg, -1);
    }

    dcmID = *pMsg->playload;
    if(dcmID < 1 || dcmID > 2)
    {
        retbool = 0;
        MQTTMSG_FMT_DEBUG("发送短信未知设备号 %d\n", dcmID);
    }

    if(1 == dcmID)
    {
        if(WIRELESS_UNUSED_COMMPORT == g_Manager.DCM0Port)
        {
            retbool = 0;
        }
        if(PPP_DAIL_STATUS_OFFLINE == ptWireData->atdata0.dialState)
        {
            retbool = 0;
        }
        ptMsgQueue = ptWireData->atdata0.ptMsgQueue;
    }
    if(2 == dcmID)
    {
        if(WIRELESS_UNUSED_COMMPORT == g_Manager.DCM1Port)
        {
            retbool = 0;
        }
        if(PPP_DAIL_STATUS_OFFLINE == ptWireData->atdata0.dialState)
        {
            retbool = 0;
        }
        ptMsgQueue = ptWireData->atdata1.ptMsgQueue;
    }

    if(retbool)
    {
        /* 写入AT线程缓存队列 */
        msgLen = sizeof(MESSAGE_INFO_T) + pMsg->msglen;
        if(0 != write_queue(ptMsgQueue, (uint8 *)pMsg, msgLen))
        {
            MQTTMSG_FMT_DEBUG("wireless_sendsms_req write_queue(%d) failed\n", *(pMsg->playload));
            retbool = 0;
        }
    }

    if(0 == retbool)
    {
        /* 构造返回消息 */
        msgLen = sizeof(MESSAGE_INFO_T) + 1;
        ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
        if(NULL == ptMsgNew)
        {
            MQTTMSG_FMT_DEBUG("wireless_sendsms_req malloc failed\n");
            return -1;
        }
        memset(ptMsgNew, 0, msgLen);

        ptMsgNew->rpm = 0;
        ptMsgNew->priority = pMsg->priority;
        ptMsgNew->index = pMsg->index;
        ptMsgNew->label = pMsg->label;

        ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
        memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

        memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
        ptMsgNew->destlen = pMsg->sourlen;

        ptMsgNew->IID = pMsg->IID;
        ptMsgNew->IOP = pMsg->IOP;

        ptMsgNew->msglen = 1;
        *ptMsgNew->playload = retbool;
        
        sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
        ret = wireless_send_msg(uTopic, ptMsgNew, 0);
        free(ptMsgNew);
    }
    return ret;
}

/**********************************************************************
* @name      : wireless_receivesms_req
* @brief     ：无线模块接收短信处理函数
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic        全局数据
* @param[out]：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-29
* @Update    :
**********************************************************************/
int wireless_receivesms_req(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    uint32 msgLen = 0;
    uint8  dcmID = 0;
    char   uTopic[256] = {0};
    uint8  retbool = 1;
    int    ret = 0;
    WIRELESS_QUEUE_T *ptMsgQueue = NULL;
    WIRELESS_DATA_T  *ptWireData = (WIRELESS_DATA_T*)pPublic;
    MESSAGE_INFO_T   *ptMsgNew = NULL;

    if(NULL == pMsg || NULL == pArg)
    {
        return -1;
    }
    
    if(1 != pMsg->msglen)
    {
        MQTTMSG_FMT_DEBUG("接收短信消息长度 %d\n", pMsg->msglen);
        return wireless_send_errmsg(pMsg, -1);
    }

    dcmID = *pMsg->playload;
    if(dcmID < 1 || dcmID > 2)
    {
        retbool = 0;
        MQTTMSG_FMT_DEBUG("接收短信未知设备号 %d\n", dcmID);
    }
    
    if(1 == dcmID)
    {
        if(WIRELESS_UNUSED_COMMPORT == g_Manager.DCM0Port)
        {
            retbool = 0;
        }
        if(PPP_DAIL_STATUS_OFFLINE == ptWireData->atdata0.dialState)
        {
            retbool = 0;
        }
        ptMsgQueue = ptWireData->atdata0.ptMsgQueue;
    }
    if(2 == dcmID)
    {
        if(WIRELESS_UNUSED_COMMPORT == g_Manager.DCM1Port)
        {
            retbool = 0;
        }
        if(PPP_DAIL_STATUS_OFFLINE == ptWireData->atdata1.dialState)
        {
            retbool = 0;
        }
        ptMsgQueue = ptWireData->atdata1.ptMsgQueue;
    }

    /* 写入AT 线程缓存队列 */
    if(1 == retbool)
    {
        msgLen = sizeof(MESSAGE_INFO_T) + pMsg->msglen;
        if(0 != write_queue(ptMsgQueue, (uint8 *)pMsg, msgLen))
        {
            MQTTMSG_FMT_DEBUG("wireless_sendsms_req write_queue(%d) failed\n", *(pMsg->playload));
            retbool = 0;
        }
    }

    if(0 == retbool)
    {
        /* 构造返回消息 */
        msgLen = sizeof(MESSAGE_INFO_T) + 1;
        ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
        if(NULL == ptMsgNew)
        {
            MQTTMSG_FMT_DEBUG("wireless_receivesms_req malloc failed\n");
            return -1;
        }
        memset(ptMsgNew, 0, msgLen);

        ptMsgNew->rpm = 0;
        ptMsgNew->priority = pMsg->priority;
        ptMsgNew->index = pMsg->index;
        ptMsgNew->label = pMsg->label;

        ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
        memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

        memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
        ptMsgNew->destlen = pMsg->sourlen;

        ptMsgNew->IID = pMsg->IID;
        ptMsgNew->IOP = pMsg->IOP;

        ptMsgNew->msglen = 1;
        *ptMsgNew->playload = 0;
        
        sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
        ret = wireless_send_msg(uTopic, ptMsgNew, 0);
        free(ptMsgNew);
        return ret;
    }

    return 0;
}

/**********************************************************************
* @name      : wireless_set_diagapn
* @brief     ：无线拨号模块收到修改拨号参数消息
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic        全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-14
* @Update    :
**********************************************************************/
int wireless_set_diagapn(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    int         ret = 0;
    uint8       retbool = FALSE;
    uint32      msgLen = 0;
    uint8       i;
    char        strtmp[2048] = {0};
    uint16      offset = 0;
    MESSAGE_INFO_T    *ptMsgNew = NULL;
    char               uTopic[256] = {0};
    WIRE_DIAG_APN_T    userAPN;
    NET_PARA_T        *ptNetPara = NULL;   /* 网络参数 */
    WIRELESS_HANDLE_T *ptPPPHandle = NULL;
    WIRELESS_DATA_T   *ptWireData = (WIRELESS_DATA_T*)pPublic;
    //char *stackStr[] = {"自动切换" , "内置", "外置"};
    char *netStr[] = {"自动" , "2G", "3G", "4G", "5G"};
    char *authStr[] = {"null" , "chap", "pap", "pap&chap"};

    memset(&userAPN, 0, sizeof(WIRE_DIAG_APN_T));
    if(NULL == pMsg || NULL == pArg)
    {
        return -1;
    }
    
    if(pMsg->msglen < 6)
    {
        MQTTMSG_FMT_DEBUG("设置拨号参数非法, 消息长度 %d\n", pMsg->msglen);
        return wireless_send_errmsg(pMsg, -1);
    }

    memset(&userAPN, 0, sizeof(WIRE_DIAG_APN_T));
    ret = appmsg_wireapn_split(pMsg->playload, pMsg->msglen, &userAPN, TRUE);
    if(0 != ret)
    {
        char *errStr[] = {"消息错误", "协议栈不支持", "运营商参数过多", "网络不支持", "鉴权方式不支持"};
        MQTTMSG_FMT_DEBUG("设置拨号参数错误, %s\n", ret < 5 ? errStr[ret -1] : "未知");
        goto end;
    }

    for(i = 0; i < userAPN.diagnum; i++)
    {
        offset += sprintf(strtmp + offset, "(%d-%s-%s-%s) ", userAPN.tDiagApn[i].operat, userAPN.tDiagApn[i].apn.value,
                                           userAPN.tDiagApn[i].username.value, userAPN.tDiagApn[i].pwd.value);
    }
    MQTTMSG_FMT_DEBUG("%s设置设备%d拨号参数: 网络锁定%s, 鉴权方式%s, 拨号参数:\n%s\n", 
                      pMsg->souraddr, userAPN.id, netStr[userAPN.network], 
                      userAPN.authtype <= 3 ? authStr[userAPN.authtype] : "自动", strtmp);

    if(1 == userAPN.id)
    {
        ptPPPHandle = &ptWireData->pppdata0;
        ptNetPara = ptWireData->pppdata0.ptNetPara;
    }
    else if(2 == userAPN.id)
    {
        ptPPPHandle = &ptWireData->pppdata1;
        ptNetPara = ptWireData->pppdata1.ptNetPara;
    }
    else
    {
        goto end;
    }

    /* 写到配置文件中 */
    wireless_write_user_apn(ptPPPHandle->userfile, userAPN.id, &userAPN);
    wireless_write_authtype(ptPPPHandle->userfile, userAPN.id, userAPN.authtype);

    /* 用户锁定网络检查 认证方式 */
    if(userAPN.network != ptNetPara->networkSet || ptNetPara->authType != userAPN.authtype)
    {
        ptNetPara->networkSet = userAPN.network;
        ptNetPara->authType = userAPN.authtype;
        if(ptPPPHandle->stepStatus >= AT_SYSINFO)
        {
            WIRELESS_FMT_DEBUG("设备%d的锁网或认证方式变更, 需要重启\n", userAPN.id);
            /* 为了保证重拨顺利,重启设备 */
            wireless_restart_dev(userAPN.id - 1, ptPPPHandle->commport + ((ptPPPHandle->subport << 4) & 0xf0));
            goto cfgcopy;
        }
    }
    
    /* 运营商参数检查 */
    i = ptPPPHandle->apnIndex;
    if(FALSE == ptPPPHandle->bUserApn || 
       0 != memcmp(&(ptNetPara->userAPN.tDiagApn[i]), &(userAPN.tDiagApn[i]), sizeof(WIRE_APN_T)))
    {
        memcpy(&(ptNetPara->userAPN), &userAPN, sizeof(WIRE_DIAG_APN_T));
        if(ptPPPHandle->stepStatus >= AT_SYSINFO)
        {
            ptPPPHandle->apnIndex = 0;
            WIRELESS_FMT_DEBUG("设备%d的APN变更, 需要重启\n", userAPN.id);
            /* 为了保证重拨顺利,重启设备 */
            wireless_restart_dev(userAPN.id - 1, ptPPPHandle->commport + ((ptPPPHandle->subport << 4) & 0xf0));
        }
    }
    
cfgcopy:
    retbool = TRUE;
    memcpy(&(ptNetPara->userAPN), &userAPN, sizeof(WIRE_DIAG_APN_T));
    
end:
    /* 构造返回消息 */
    msgLen = sizeof(MESSAGE_INFO_T) + 1;
    ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
    if(NULL == ptMsgNew)
    {
        MQTTMSG_FMT_DEBUG("wireless_set_diagapn malloc failed\n");
        return -1;
    }
    memset(ptMsgNew, 0, msgLen);

    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pMsg->priority;
    ptMsgNew->index = pMsg->index;
    ptMsgNew->label = pMsg->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
    ptMsgNew->destlen = pMsg->sourlen;

    ptMsgNew->IID = pMsg->IID;
    ptMsgNew->IOP = pMsg->IOP;

    ptMsgNew->msglen = 1;
    *ptMsgNew->playload = retbool;
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
    ret = wireless_send_msg(uTopic, ptMsgNew, 0);
    free(ptMsgNew);
    return ret;
}

/**********************************************************************
* @name      : wireless_redial_req
* @brief     ：无线拨号模块收到重新拨号消息 
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic        全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-14
* @Update    :
**********************************************************************/
int wireless_redial_req(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    int             ret = 0;
    uint8         dcmID = 0;
    uint8       retbool = 0;
    uint32       msgLen = 0;
    MESSAGE_INFO_T    *ptMsgNew = NULL;
    char            uTopic[256] = {0};
    WIRELESS_DATA_T *ptWireData = (WIRELESS_DATA_T*)pPublic;

    if(NULL == pMsg || NULL == pArg)
    {
        return -1;
    }
    
    if(1 != pMsg->msglen)
    {
        MQTTMSG_FMT_DEBUG("重新拨号消息长度 %d\n", pMsg->msglen);
        return wireless_send_errmsg(pMsg, -1);
    }

    dcmID = *pMsg->playload;
    WIRELESS_FMT_DEBUG("接收%s重新拨号模块%d消息\n", pMsg->souraddr, dcmID);
    if(1 == dcmID)
    {
        if(ptWireData->pppdata0.commport > 0)
        {
            /* 为了保证重拨顺利,重启设备 */
            wireless_restart_dev(0, ptWireData->pppdata0.commport + ((ptWireData->pppdata0.subport << 4) & 0xf0));
            retbool = 1;
        }
    }
    else if(2 == dcmID)
    {
        if(ptWireData->pppdata1.commport > 0)
        {
            /* 为了保证重拨顺利,重启设备 */
            wireless_restart_dev(1, ptWireData->pppdata1.commport + ((ptWireData->pppdata1.subport << 4) & 0xf0));
            retbool = 1;
        }
    }
    else
    {
        retbool = 0;
        MQTTMSG_FMT_DEBUG("重新拨号未知设备号 %d\n", dcmID);
    }

    /* 构造返回消息 */
    msgLen = sizeof(MESSAGE_INFO_T) + 1;
    ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
    if(NULL == ptMsgNew)
    {
        MQTTMSG_FMT_DEBUG("wireless_redial_req malloc failed\n");
        return -1;
    }
    memset(ptMsgNew, 0, msgLen);

    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pMsg->priority;
    ptMsgNew->index = pMsg->index;
    ptMsgNew->label = pMsg->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
    ptMsgNew->destlen = pMsg->sourlen;

    ptMsgNew->IID = pMsg->IID;
    ptMsgNew->IOP = pMsg->IOP;

    ptMsgNew->msglen = 1;
    *ptMsgNew->playload = retbool;
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
    ret = wireless_send_msg(uTopic, ptMsgNew, 0);
    free(ptMsgNew);
    return ret;
}

/**********************************************************************
* @name      : wireless_gps_req
* @brief     ：无线拨号模块收到获取GPS定位信息
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic        全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
int wireless_gps_req(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    int             ret = 0;
    uint32       msgLen = 0;
    uint8         dcmID = 0;
    MESSAGE_INFO_T *ptMsgNew = NULL;
    char    uTopic[256] = {0};
    uint8   palyout[512] = {0};
    uint16  outlen = 0;
    GPS_T   tGpsPosition;
    WIRELESS_DATA_T *ptWireData = (WIRELESS_DATA_T*)pPublic;

    if(NULL == pMsg || NULL == pArg)
    {
        return -1;
    }

    if(1 != pMsg->msglen)
    {
        MQTTMSG_FMT_DEBUG("获取GPS定位信息消息长度 %d\n", pMsg->msglen);
        return wireless_send_errmsg(pMsg, -1);
    }

    dcmID = *pMsg->playload;
    if(1 == dcmID)
    {
        if(ptWireData->atdata0.stepStatus >= GSM_CPIN && TRUE == ptWireData->atdata0.workFlag)
        {
            /* 写入AT线程缓存队列 */
            msgLen = sizeof(MESSAGE_INFO_T) + pMsg->msglen;
            if(0 != write_queue(ptWireData->atdata0.ptMsgQueue, (uint8 *)pMsg, msgLen))
            {
                MQTTMSG_FMT_DEBUG("wireless_gps_req write_queue(%d) failed\n", *(pMsg->playload));
            }
            return 0;
        }
    }
    else if(2 == dcmID)
    {
        if(ptWireData->atdata1.stepStatus >= GSM_CPIN && TRUE == ptWireData->atdata1.workFlag)
        {
            msgLen = sizeof(MESSAGE_INFO_T) + pMsg->msglen;
            if(0 != write_queue(ptWireData->atdata1.ptMsgQueue, (uint8 *)pMsg, msgLen))
            {
                MQTTMSG_FMT_DEBUG("wireless_gps_req write_queue(%d) failed\n", *(pMsg->playload));
            }
            return 0;
        }
    }
    else
    {
        MQTTMSG_FMT_DEBUG("获取GPS定位信息未知设备号 %d\n", dcmID);
    }

    /* 构造返回消息 */
    memset(&tGpsPosition, 0 ,sizeof(GPS_T));
    memset(&(tGpsPosition.utctime), 0xff, sizeof(OOP_DATETIME_T));
    tGpsPosition.mode = 4;
    tGpsPosition.ststus = 0;
    outlen = appmsg_gps_package(&tGpsPosition, palyout, 512, TRUE);

    msgLen = sizeof(MESSAGE_INFO_T) + outlen;
    ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
    if(NULL == ptMsgNew)
    {
        MQTTMSG_FMT_DEBUG("wireless_gps_req malloc failed\n");
        return -1;
    }
    memset(ptMsgNew, 0, msgLen);

    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pMsg->priority;
    ptMsgNew->index = pMsg->index;
    ptMsgNew->label = pMsg->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
    ptMsgNew->destlen = pMsg->sourlen;

    ptMsgNew->IID = pMsg->IID;
    ptMsgNew->IOP = pMsg->IOP;

    ptMsgNew->msglen = outlen;
    memcpy(ptMsgNew->playload, palyout, outlen);
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
    ret = wireless_send_msg(uTopic, ptMsgNew, 0);
    free(ptMsgNew);
    return ret;
}

/**********************************************************************
* @name      : wireless_auth_req
* @brief     ：无线拨号模块收到设置鉴权方式
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic        全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
int wireless_auth_req(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    int             ret = 0;
    uint8         dcmID = 0;
    uint8      authType = 0;
    uint8       retbool = 0;
    uint32       msgLen = 0;
    MESSAGE_INFO_T    *ptMsgNew = NULL;
    char            uTopic[256] = {0};
    WIRELESS_DATA_T   *ptWireData = (WIRELESS_DATA_T*)pPublic;
    NET_PARA_T        *ptNetPara = NULL;   /* 网络参数 */
    WIRELESS_HANDLE_T *ptPPPHandle = NULL;

    if(NULL == pMsg || NULL == pArg)
    {
        return -1;
    }
    
    if(2 != pMsg->msglen)
    {
        MQTTMSG_FMT_DEBUG("设置鉴权方式消息长度 %d\n", pMsg->msglen);
        return wireless_send_errmsg(pMsg, -1);
    }

    dcmID = *pMsg->playload;
    if(1 == dcmID)
    {
        ptPPPHandle = &ptWireData->pppdata0;
        ptNetPara = ptWireData->pppdata0.ptNetPara;
    }
    else if(2 == dcmID)
    {
        ptPPPHandle = &ptWireData->pppdata1;
        ptNetPara = ptWireData->pppdata1.ptNetPara;
    }
    else
    {
        MQTTMSG_FMT_DEBUG("设置鉴权方式未知设备号 %d\n", dcmID);
        goto end;
    }
    
    authType = *(pMsg->playload + 1);
    MQTTMSG_FMT_DEBUG("设备%d认证方式%d\n", dcmID, authType);

    if(authType < 4 || 255 == authType)
    {
        if(ptNetPara->authType != authType)
        {
            ptNetPara->authType = authType;
            if(WIRELESS_UNUSED_COMMPORT != ptPPPHandle->commport && authType < 4)
            {
                /* 为了保证重拨顺利,重启设备 */
                wireless_restart_dev(dcmID - 1, ptPPPHandle->commport + ((ptPPPHandle->subport << 4) & 0xf0));
                WIRELESS_FMT_DEBUG("设备%d认证方式变化 需要重启\n", dcmID);
            }
            wireless_write_authtype(ptPPPHandle->userfile, dcmID, authType);
        }
        retbool = 1;
    }
end:
    /* 构造返回消息 */
    msgLen = sizeof(MESSAGE_INFO_T) + 1;
    ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
    if(NULL == ptMsgNew)
    {
        MQTTMSG_FMT_DEBUG("wireless_auth_req malloc failed\n");
        return -1;
    }
    memset(ptMsgNew, 0, msgLen);

    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pMsg->priority;
    ptMsgNew->index = pMsg->index;
    ptMsgNew->label = pMsg->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
    ptMsgNew->destlen = pMsg->sourlen;

    ptMsgNew->IID = pMsg->IID;
    ptMsgNew->IOP = pMsg->IOP;

    ptMsgNew->msglen = 1;
    *ptMsgNew->playload = retbool;
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
    ret = wireless_send_msg(uTopic, ptMsgNew, 0);
    free(ptMsgNew);
    return ret;
}

/**********************************************************************
* @name      : wireless_smscenter_req
* @brief     ：无线拨号模块收到设置短信中心号码
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic        全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
int wireless_smscenter_req(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    int             ret = 0;
    uint8         *temp = NULL;
    uint8         dcmID = 0;
    uint8          type = 0;
    uint16          len = 0;
    uint8 number[SMS_PDU_PHONE_LEN + 1] = {0};
    uint8       retbool = 1;
    uint8       bretmsg = 1;
    uint32       msgLen = 0;
    MESSAGE_INFO_T *ptMsgNew = NULL;
    char    uTopic[256] = {0};
    WIRELESS_DATA_T   *ptWireData = (WIRELESS_DATA_T*)pPublic;
    NET_PARA_T        *ptNetPara = NULL;   /* 网络参数 */
    WIRELESS_HANDLE_T *ptATHandle = NULL;

    if(NULL == pMsg || NULL == pArg)
    {
        return -1;
    }
    
    if(pMsg->msglen < 3)
    {
        MQTTMSG_FMT_DEBUG("设置短信参数消息长度 %d\n", pMsg->msglen);
        return wireless_send_errmsg(pMsg, -1);
    }

    temp = pMsg->playload;

    dcmID = *temp;
    if(1 == dcmID)
    {
        ptATHandle = &ptWireData->atdata0;
        ptNetPara = ptWireData->atdata0.ptNetPara;
    }
    else if(2 == dcmID)
    {
        ptATHandle = &ptWireData->atdata1;
        ptNetPara = ptWireData->atdata1.ptNetPara;
    }
    else
    {
        MQTTMSG_FMT_DEBUG("设置短信参数未知设备号 %d\n", dcmID);
        retbool = 0;
        goto end;
    }
    temp++;

    type = *temp;
    temp++;

    len = *temp;
    temp++;
    if(len > 0)
    {
        memcpy(number, temp, MIN(SMS_PDU_PHONE_LEN, len));
    }
    MQTTMSG_FMT_DEBUG("设备%d接收设置短信中心号码%s, 类型%d\n", dcmID, number, type);
    
    if(type <= 1)
    {
        if(ptATHandle->stepStatus < GSM_CSCA)
        {
            memcpy(ptNetPara->centerNnm, number, SMS_PDU_PHONE_LEN);
            ptNetPara->smsType = type;
            wireless_write_centernum(ptATHandle->userfile, dcmID, number, SMS_PDU_PHONE_LEN, type);
        }
        else
        {
            msgLen = sizeof(MESSAGE_INFO_T) + pMsg->msglen;
            if(0 != write_queue(ptATHandle->ptMsgQueue, (uint8 *)pMsg, msgLen))
            {
                MQTTMSG_FMT_DEBUG("wireless_smscenter_req write_queue(%d) failed\n", dcmID);
                retbool = 0;
            }
            else
            {
                bretmsg = 0;
            }
        }
    }
    else
    {
        retbool = 0;
    }

    if(0 == bretmsg)
    {
        return 0;
    }
end:
    /* 构造返回消息 */
    msgLen = sizeof(MESSAGE_INFO_T) + 1;
    ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
    if(NULL == ptMsgNew)
    {
        MQTTMSG_FMT_DEBUG("wireless_smscenter_req malloc failed\n");
        return -1;
    }
    memset(ptMsgNew, 0, msgLen);

    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pMsg->priority;
    ptMsgNew->index = pMsg->index;
    ptMsgNew->label = pMsg->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
    ptMsgNew->destlen = pMsg->sourlen;

    ptMsgNew->IID = pMsg->IID;
    ptMsgNew->IOP = pMsg->IOP;

    ptMsgNew->msglen = 1;
    *ptMsgNew->playload = retbool;
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
    ret = wireless_send_msg(uTopic, ptMsgNew, 0);
    free(ptMsgNew);
    return ret;
}

/**********************************************************************
* @name      : wireless_diagstats_req
* @brief     ：无线拨号模块收到拨号统计
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic        全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
int wireless_diagstats_req(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    int             ret = 0;
    uint8         *temp = NULL;
    uint8         dcmID = 0;
    uint32       msgLen = 0;
    MESSAGE_INFO_T  *ptMsgNew = NULL;
    char             uTopic[256] = {0};
    WIRELESS_DATA_T *ptWireData = (WIRELESS_DATA_T*)pPublic;
    uint16           connectNUM = 0;
    uint16           failedNUM = 0;
    uint32           successTime = 0;

    if(NULL == pMsg || NULL == pArg)
    {
        return -1;
    }

    temp = pMsg->playload;
    dcmID = *temp;

    if(1 == dcmID)
    {
        connectNUM = ptWireData->pppdata0.ConnectNum;
        failedNUM = ptWireData->pppdata0.FailedNum;
        successTime = ptWireData->pppdata0.SuccessTimes;
    }
    else if(2 == dcmID)
    {
        connectNUM = ptWireData->pppdata1.ConnectNum;
        failedNUM = ptWireData->pppdata1.FailedNum;
        successTime = ptWireData->pppdata1.SuccessTimes;
    }
    else
    {
        MQTTMSG_FMT_DEBUG("不支持的设备 %d\n", dcmID);
        return wireless_send_errmsg(pMsg, -1);
    }
    
    /* 构造返回消息 */
    msgLen = sizeof(MESSAGE_INFO_T) + 8;
    ptMsgNew = (MESSAGE_INFO_T *)malloc(msgLen);
    if(NULL == ptMsgNew)
    {
        MQTTMSG_FMT_DEBUG("wireless_diagstats_req malloc failed\n");
        return -1;
    }
    memset(ptMsgNew, 0, msgLen);

    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pMsg->priority;
    ptMsgNew->index = pMsg->index;
    ptMsgNew->label = pMsg->label;

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pMsg->souraddr, pMsg->sourlen);
    ptMsgNew->destlen = pMsg->sourlen;

    ptMsgNew->IID = pMsg->IID;
    ptMsgNew->IOP = pMsg->IOP;

    ptMsgNew->msglen = 8;
    temp = ptMsgNew->playload;
    appmsg_memcpy_r(temp, &connectNUM, 2);
    appmsg_memcpy_r(temp + 2, &failedNUM, 2);
    appmsg_memcpy_r(temp + 4, &successTime, 4);
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pMsg->souraddr);
    ret = wireless_send_msg(uTopic, ptMsgNew, 0);
    free(ptMsgNew);
    return ret;
}

/**********************************************************************
* @name      : wireless_modinfo_ack
* @brief     ：查询模组信息 返回消息处理
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic        全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-29
* @Update    : 2020-1-9
**********************************************************************/
int wireless_modinfo_ack(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    uint16          moduleID = 0;
    uint8           subport = 0xFF;
    uint8           i;
    MODS_INFO_NEW_T tModes;
    MOD_INFO_NEW_T  *ptModule = NULL;
    WIRELESS_DATA_T *ptWireData = (WIRELESS_DATA_T*)pPublic;
    char *statusname[] = {"识别", "未识别", "未接入"};

    if(NULL == pMsg || NULL == pArg || 0 == pMsg->msglen)
    {
        return -1;
    }

    moduleID = pMsg->label;
    appmsg_modsinfo_split_new(pMsg->playload, &tModes);
    if(FALSE == tModes.bSubMod && 1 == tModes.num)
    {
        ptModule = &(tModes.info[0]);
    }
    else
    {
        subport = ((moduleID & 0xf0) >> 4);
        for(i = 0; i < tModes.num; i++)
        {
            if(tModes.info[i].subport == subport)
            {
                ptModule = &(tModes.info[i]);
                break;
            }
        }
    }

    if(NULL == ptModule)
    {
        WIRELESS_FMT_DEBUG("获取模组%02X信息失败\n", moduleID);
        return -1;
    }

    WIRELESS_FMT_DEBUG("获取%s%02X信息: 模组%s, 型号 %s, ID %s, 厂商编码 %s, 通道数量 %d\n", 
                       FALSE == tModes.bSubMod ? "模组" : "子模组", moduleID, 
                       ptModule->status < 3 ? statusname[ptModule->status] : "未知状态",
                       ptModule->type.value, ptModule->Id.value, ptModule->manuCode.value, ptModule->num);

    /* 状态更新 */ 
    if(g_Manager.DCM0Port == moduleID)
    {
        memcpy(&(ptWireData->module[0]), ptModule, sizeof(MOD_INFO_NEW_T));
        if(ptModule->num < 3 || 0 != ptModule->status)
        {
            WIRELESS_FMT_DEBUG("模组%02X通道偏少或者状态异常\n", moduleID);
        }
        else
        {
            g_Manager.DCM0Status = WIRELESS_SEEK_ONLINE;
            g_Manager.DCM0SubPort = FALSE == tModes.bSubMod ? 0 : subport;
            WIRELESS_FMT_DEBUG("通道1设备%s, 通道2设备%s\n", ptModule->channel[1].dev.value, ptModule->channel[2].dev.value);
        }
    }
    
    if(g_Manager.DCM1Port == moduleID)
    {
         memcpy(&(ptWireData->module[1]), ptModule, sizeof(MOD_INFO_NEW_T));
        if(ptModule->num < 3 || 0 != ptModule->status)
        {
            WIRELESS_FMT_DEBUG("模组%02X通道偏少或者状态异常\n", moduleID);
        }
        else
        {
            g_Manager.DCM1Status = WIRELESS_SEEK_ONLINE;
            g_Manager.DCM1SubPort = FALSE == tModes.bSubMod ? 0 : subport;
            WIRELESS_FMT_DEBUG("通道1设备%s, 通道2设备%s\n", ptModule->channel[1].dev.value, ptModule->channel[2].dev.value);
        }
    }
 
    return 0;
}

/**********************************************************************
* @name      : wireless_modport_ack
* @brief     ：查询模组槽位信息 返回消息处理
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic        全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-29
* @Update    : 2020-1-9
**********************************************************************/
int wireless_modport_ack(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    uint8  i, j;
    uint8  num = 0;
    int    port[10] = {0};
    uint8 *pTemp = NULL;
    char   tempstring[256] = {0};
    uint8  offset = 0;
    WIRELESS_DATA_T   *ptWireData = (WIRELESS_DATA_T*)pPublic;

    if(NULL == pMsg || NULL == pArg || 0 == pMsg->msglen)
    {
        return -1;
    }
    
    pTemp = pMsg->playload;
    num = *pTemp;
    pTemp++;

    offset = sprintf(tempstring, "获取远程通信模块数量%d, 模组位置: ", num);
    for(i = 0; i < 10 && i < num; i++)
    {
        appmsg_memcpy_r(&(port[i]), pTemp, sizeof(int));
        offset += sprintf(tempstring + offset, "%02X ", port[i]);
        pTemp += 4;
    }
    WIRELESS_FMT_DEBUG("%s\n", tempstring);

    /* 超过2个按照槽位号 子模块号排序 */
    num = MIN(num, 10);
    if(2 <= num)
    {
        uint8 tempport = 0;
        for(i = 0; i < num - 1; i++)
        {
            for(j = i + 1; j < num; j++)
            {
                if((port[i] & 0x0f) < (port[j] & 0x0f))
                {
                    continue;
                }
                else if((port[i] & 0x0f) == (port[j] & 0x0f) && (port[i] & 0xf0) < (port[j] & 0xf0))
                {
                    continue;  //槽位相同 按照子模块排序
                }
                tempport = port[i];
                port[i] = port[j];
                port[j] = tempport;
            }
        }
    }

    g_Manager.GetStatus = FALSE;
    if(0 == num)
    {
        /* 关闭已经打开的模块 */
        if(0 != g_Manager.DCM0Port)
        {
            memset(&(ptWireData->module[0]), 0, sizeof(MOD_INFO_NEW_T));
            ptWireData->pppdata0.workFlag = FALSE;
            ptWireData->pppdata0.commport = 0;
            ptWireData->pppdata0.workAgain = TRUE;
            ptWireData->pppdata0.dialState = PPP_DAIL_STATUS_OFFLINE;

            ptWireData->atdata0.workFlag = FALSE;
            ptWireData->atdata0.commport = 0;
            ptWireData->atdata0.workAgain = TRUE;
            ptWireData->atdata0.dialState = PPP_DAIL_STATUS_OFFLINE;
            
            g_Manager.DCM0Port = 0;
            g_Manager.DCM0Status = WIRELESS_SEEK_WAIT;
        }
        if(0 != g_Manager.DCM1Port)
        {
            memset(&(ptWireData->module[1]), 0, sizeof(MOD_INFO_NEW_T));
            ptWireData->pppdata1.workFlag = FALSE;
            ptWireData->pppdata1.commport = 0;
            ptWireData->pppdata1.workAgain = TRUE;
            ptWireData->pppdata1.dialState = PPP_DAIL_STATUS_OFFLINE;

            ptWireData->atdata1.workFlag = FALSE;
            ptWireData->atdata1.commport = 0;
            ptWireData->atdata1.workAgain = TRUE;
            ptWireData->atdata1.dialState = PPP_DAIL_STATUS_OFFLINE;

            g_Manager.DCM1Port = 0;
            g_Manager.DCM1Status = WIRELESS_SEEK_WAIT;
        }
        return 0;
    }

    /* 检查端口是否发生变化 */
    if(1 == num)
    {
        if(port[0] != g_Manager.DCM0Port && port[0] != g_Manager.DCM1Port)
        {
            if(g_Manager.DCM0Port > 0)
            {
                memset(&(ptWireData->module[0]), 0, sizeof(MOD_INFO_NEW_T));
            }
            g_Manager.DCM0Port = port[0];
            ptWireData->pppdata0.workFlag = FALSE;
            ptWireData->pppdata0.commport = 0;
            ptWireData->pppdata0.workAgain = TRUE;
            ptWireData->pppdata0.dialState = PPP_DAIL_STATUS_OFFLINE;
            ptWireData->atdata0.workFlag = FALSE;
            ptWireData->atdata0.commport = 0;
            ptWireData->atdata0.workAgain = TRUE;
            ptWireData->atdata0.dialState = PPP_DAIL_STATUS_OFFLINE;
            
            g_Manager.DCM0Status = WIRELESS_SEEK_INFO;

            if(g_Manager.DCM1Port > 0)
            {
                memset(&(ptWireData->module[1]), 0, sizeof(MOD_INFO_NEW_T));
            }
            g_Manager.DCM1Port = 0;
            ptWireData->pppdata1.workFlag = FALSE;
            ptWireData->pppdata1.commport = 0;
            ptWireData->pppdata1.workAgain = TRUE;
            ptWireData->pppdata1.dialState = PPP_DAIL_STATUS_OFFLINE;
            ptWireData->atdata1.workFlag = FALSE;
            ptWireData->atdata1.commport = 0;
            ptWireData->atdata1.workAgain = TRUE;
            ptWireData->atdata1.dialState = PPP_DAIL_STATUS_OFFLINE;
            
            g_Manager.DCM1Status = WIRELESS_SEEK_WAIT;
        }
    }
    else
    {
        if(port[0] != g_Manager.DCM0Port)
        {
            if(g_Manager.DCM0Port > 0)
            {
                memset(&(ptWireData->module[0]), 0, sizeof(MOD_INFO_NEW_T));
            }
            g_Manager.DCM0Port = port[0];
            ptWireData->pppdata0.workFlag = FALSE;
            ptWireData->pppdata0.commport = 0;
            ptWireData->pppdata0.workAgain = TRUE;
            ptWireData->pppdata0.dialState = PPP_DAIL_STATUS_OFFLINE;
            ptWireData->atdata0.workFlag = FALSE;
            ptWireData->atdata0.commport = 0;
            ptWireData->atdata0.workAgain = TRUE;
            ptWireData->atdata0.dialState = PPP_DAIL_STATUS_OFFLINE;
            
            g_Manager.DCM0Status = WIRELESS_SEEK_INFO;

        }
        if(port[1] != g_Manager.DCM1Port)
        {
            if(g_Manager.DCM1Port > 0)
            {
                memset(&(ptWireData->module[1]), 0, sizeof(MOD_INFO_NEW_T));
            }
            g_Manager.DCM1Port = port[1];
            ptWireData->pppdata1.workFlag = FALSE;
            ptWireData->pppdata1.commport = 0;
            ptWireData->pppdata1.workAgain = TRUE;
            ptWireData->pppdata1.dialState = PPP_DAIL_STATUS_OFFLINE;
            ptWireData->atdata1.workFlag = FALSE;
            ptWireData->atdata1.commport = 0;
            ptWireData->atdata1.workAgain = TRUE;
            ptWireData->atdata1.dialState = PPP_DAIL_STATUS_OFFLINE;
            
            g_Manager.DCM1Status = WIRELESS_SEEK_INFO;
        }
    }

    return 0;
}

/**********************************************************************
* @name      : wireless_modonline_ack
* @brief     ：查询模组插拔状态 返回消息处理
* @param[in] ：MSG_INFO_T *pMsg 消息
               void *pArg       句柄
               void *pPublic        全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-29
* @Update    : 2020-1-9
**********************************************************************/
int wireless_modonline_ack(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    uint16          moduleID = 0;
    uint8             *pTemp = NULL;
    uint8            bOnline = 0;
    uint8            bChange = 0;
    WIRELESS_DATA_T *ptWireData = (WIRELESS_DATA_T*)pPublic;

    if(NULL == pMsg || NULL == pArg)
    {
        return -1;
    }

    moduleID = pMsg->label;

    pTemp = pMsg->playload;
    if(0 == *pTemp)
    {
        bOnline = TRUE;
    }
    else
    {
        bOnline = FALSE;
    }

    pTemp++;
    bChange = *pTemp;
    WIRELESS_FMT_DEBUG("模组%02X状态: %s %s\n", moduleID, TRUE == bOnline ? "插入" : "拔出", 
                                              FALSE == bChange ? "不变" : "变位");
    if(TRUE == bOnline)
    {
        if(g_Manager.DCM0Port == moduleID)
        {
            if(FALSE == ptWireData->pppdata0.workFlag || 0 == ptWireData->pppdata0.commport)
            {
                ptWireData->pppdata0.commport = (g_Manager.DCM0Port & 0x0f);
                ptWireData->pppdata0.subport = g_Manager.DCM0SubPort;
                ptWireData->pppdata0.workFlag = TRUE;
                ptWireData->pppdata0.dialState = PPP_DAIL_STATUS_OFFLINE;
                memcpy(ptWireData->pppdata0.devName, ptWireData->module[0].channel[2].dev.value, 256);
                if(0 == ptWireData->pppdata0.subport)
                {
                    sprintf(ptWireData->pppdata0.halName, "%s%d_2", UART_ECU_DEVICE_ID_STR, ptWireData->pppdata0.commport);
                }
                else
                {
                    sprintf(ptWireData->pppdata0.halName, "%s%d_%d_2", UART_ECU_DEVICE_ID_STR, ptWireData->pppdata0.commport, ptWireData->pppdata0.subport);
                }
                memset(ptWireData->pppdata0.ptNetPara->gpsHalName, 0, 32);
                ptWireData->pppdata0.commtype = WIRELESS_COMMTYPE_USB;
            }
            if(FALSE == ptWireData->atdata0.workFlag || 0 == ptWireData->atdata0.commport)
            {
                ptWireData->atdata0.commport = (g_Manager.DCM0Port & 0x0f);
                ptWireData->atdata0.subport = g_Manager.DCM0SubPort;
                ptWireData->atdata0.workFlag = TRUE;
                ptWireData->atdata0.dialState = PPP_DAIL_STATUS_OFFLINE;
                memcpy(ptWireData->atdata0.devName, ptWireData->module[0].channel[1].dev.value, 256);
                if(0 == ptWireData->pppdata0.subport)
                {
                    sprintf(ptWireData->atdata0.halName, "%s%d_1", UART_ECU_DEVICE_ID_STR, ptWireData->atdata0.commport);
                }
                else
                {
                    sprintf(ptWireData->atdata0.halName, "%s%d_%d_1", UART_ECU_DEVICE_ID_STR, ptWireData->atdata0.commport, ptWireData->atdata0.subport);
                }
                memset(ptWireData->atdata0.ptNetPara->gpsHalName, 0, 32);
                ptWireData->atdata0.commtype = WIRELESS_COMMTYPE_USB;
            }
            g_Manager.DCM0Status = WIRELESS_SEEK_WAIT;
        }
        if(g_Manager.DCM1Port == moduleID)
        {
            if(FALSE == ptWireData->pppdata1.workFlag || 0 == ptWireData->pppdata1.commport)
            {
                ptWireData->pppdata1.commport = (g_Manager.DCM1Port & 0x0f);
                ptWireData->pppdata1.subport = g_Manager.DCM1SubPort;
                ptWireData->pppdata1.workFlag = TRUE;
                ptWireData->pppdata1.dialState = PPP_DAIL_STATUS_OFFLINE;
                memcpy(ptWireData->pppdata1.devName, ptWireData->module[1].channel[2].dev.value, 256);
                if(0 == ptWireData->pppdata1.subport)
                {
                    sprintf(ptWireData->pppdata1.halName, "%s%d_2", UART_ECU_DEVICE_ID_STR, ptWireData->pppdata1.commport);
                }
                else
                {
                    sprintf(ptWireData->pppdata1.halName, "%s%d_%d_2", UART_ECU_DEVICE_ID_STR, ptWireData->pppdata1.commport, ptWireData->pppdata1.subport);
                }
                memset(ptWireData->pppdata1.ptNetPara->gpsHalName, 0, 32);
                ptWireData->pppdata1.commtype = WIRELESS_COMMTYPE_USB;
            }
            if(FALSE == ptWireData->atdata1.workFlag || 0 == ptWireData->atdata1.commport)
            {
                ptWireData->atdata1.commport = (g_Manager.DCM1Port & 0x0f);
                ptWireData->atdata1.subport = g_Manager.DCM1SubPort;
                ptWireData->atdata1.workFlag = TRUE;
                ptWireData->atdata1.dialState = PPP_DAIL_STATUS_OFFLINE;
                memcpy(ptWireData->atdata1.devName, ptWireData->module[1].channel[1].dev.value, 256);
                if(0 == ptWireData->atdata1.subport)
                {
                    sprintf(ptWireData->atdata1.halName, "%s%d_1", UART_ECU_DEVICE_ID_STR, ptWireData->atdata1.commport);
                }
                else
                {
                    sprintf(ptWireData->atdata1.halName, "%s%d_%d_1", UART_ECU_DEVICE_ID_STR, ptWireData->atdata1.commport, ptWireData->atdata1.subport);
                }
                memset(ptWireData->atdata1.ptNetPara->gpsHalName, 0, 32);
                ptWireData->atdata1.commtype = WIRELESS_COMMTYPE_USB;
            }
            g_Manager.DCM1Status = WIRELESS_SEEK_WAIT;
        }
    }

    return 0;
}

/**********************************************************************
* @name      : wireless_mod_event
* @brief     ：查询控制端口号 返回消息处理
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic        全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-9
* @Update    :
**********************************************************************/
int wireless_mod_event(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    uint32 port = 0;
    uint8  status = 0;
    uint8  no = 0;
    uint8 *temp = NULL;
    WIRELESS_DATA_T   *ptWireData = (WIRELESS_DATA_T*)pPublic;
    NET_PARA_T        *ptNetPara = NULL;   /* 网络参数 */
    WIRELESS_HANDLE_T *ptPPPHandle = NULL;
    WIRELESS_HANDLE_T *ptATHandle = NULL;

    if(NULL == pMsg || NULL == pArg || 0 == pMsg->msglen)
    {
        return -1;
    }

    if(pMsg->IOP > 0x1000)
    {
        return 0;
    }

    temp = pMsg->playload;
    appmsg_memcpy_r(&port, temp, sizeof(uint32));
    temp += sizeof(uint32);
    status = *temp;

    if(FALSE == ptWireData->mapReg)
    {
        return -1;
    }

    WIRELESS_FMT_DEBUG("模组%02X %s\n", port, 0 == status ? "插入" : "拔出");
    if(1 == status)
    {
        /* 拔出 */
        if(port == g_Manager.DCM0Port)
        {
            ptPPPHandle = &ptWireData->pppdata0;
            ptATHandle = &ptWireData->atdata0;
            ptNetPara = ptWireData->pppdata0.ptNetPara;
            g_Manager.DCM0Port = 0;
            g_Manager.DCM0Status = WIRELESS_SEEK_WAIT;
            no = 0;
            wireless_send_diagStatus(1, "远程通信模块1被拔出", strlen("远程通信模块1被拔出"), 60);
        }
        else if(port == g_Manager.DCM1Port)
        {
            ptPPPHandle = &ptWireData->pppdata1;
            ptATHandle = &ptWireData->atdata1;
            ptNetPara = ptWireData->pppdata1.ptNetPara;
            g_Manager.DCM1Port = 0;
            g_Manager.DCM1Status = WIRELESS_SEEK_WAIT;
            no = 1;
            wireless_send_diagStatus(2, "远程通信模块2被拔出", strlen("远程通信模块2被拔出"), 60);
        }
        else
        {
            return 0;
        }
        
        memset(&(ptWireData->module[no]), 0, sizeof(MOD_INFO_NEW_T));
        /* 线程需要停止 */
        ptPPPHandle->commport = 0;
        ptATHandle->commport = 0;
        ptPPPHandle->workFlag = FALSE;
        ptATHandle->workFlag = FALSE;
        ptPPPHandle->workAgain = TRUE;
        ptATHandle->workAgain = TRUE;
        ptPPPHandle->dialState = PPP_DAIL_STATUS_OFFLINE;
        ptATHandle->dialState = PPP_DAIL_STATUS_OFFLINE;
        ptATHandle->ptNetPara->smsStatusOK = FALSE;
        ptPPPHandle->bsimOK = PPP_DAIL_STATUS_OFFLINE;
        ptATHandle->bsimOK = PPP_DAIL_STATUS_OFFLINE;
        ptATHandle->bExistGPS = 0;

        /* 数据清除 */
        bzero(ptNetPara->VERINFO.manuNum, 4);
        bzero(ptNetPara->VERINFO.manuExten, 8);
        bzero(ptNetPara->VERINFO.softversion, 4);
        bzero(ptNetPara->VERINFO.softdata, 6);
        bzero(ptNetPara->VERINFO.hwversion, 4);
        bzero(ptNetPara->VERINFO.hwdata, 6);

        bzero(ptNetPara->IMSI, 16);
        bzero(ptNetPara->ICCID, 21);             /* SIM 卡 ICCID */
        ptNetPara->networkType = OOP_NET_UNKNOW; /* 网络类型 */
        bzero(ptNetPara->IMEI, 17);

        ptNetPara->operat = WIRELESS_OPT_NONE;
        ptNetPara->csqvalue = 0;    /* 信号强度 */
        ptNetPara->basecsqvalue = 0;/* 基准信号强度 */
        bzero(ptNetPara->SIMNum, 24);  /* SIM 卡号码 */
        
        ptNetPara->ipAddr = 0;      /* 网口IP地址 */
        ptNetPara->mask = 0;        /* 子网掩码 */   
        ptNetPara->getway = 0;      /* 网关 */   

        ptNetPara->pGps->mode = 4;
        ptNetPara->pGps->ststus = 0;

        /* 离线时需要给显示发送状态 */
        wireless_send_change(no, ptNetPara->operat, 0);
        wireless_set_pppstatus(no, 1);
    }
    else
    {
        /* 插入 */
        g_Manager.GetStatus = TRUE;
    }
    return 0;
}

/**********************************************************************
* @name      : wireless_mod_restore
* @brief     ：模组复位 返回消息处理
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic        全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-4-7
* @Update    :
**********************************************************************/
int wireless_mod_restore(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    uint8 ifnum = 0;
    uint8 devid = 0;
    //WIRELESS_DATA_T *ptWireData = (WIRELESS_DATA_T*)pPublic;
    
    if(NULL == pMsg || NULL == pArg || 0 == pMsg->msglen)
    {
        return -1;
    }

    ifnum = ((pMsg->label >> 8) & 0xff);
    devid = (pMsg->label & 0xff);

    WIRELESS_FMT_DEBUG("设备%d模组%02X复位结果: %s\n", ifnum + 1, devid, 0 == *pMsg->playload ? "失败" : "成功");

    /*if(0 != *pMsg->playload)
    {
        if(0 == ifnum)
        {
            ptWireData->pppdata0.workFlag = FALSE;
            ptWireData->atdata0.workFlag = FALSE;
        }
        if(1 == ifnum)
        {
            ptWireData->pppdata1.workFlag = FALSE;
            ptWireData->atdata1.workFlag = FALSE;
        }
    }*/
    return 0;
}

/**********************************************************************
* @name      : wireless_mod_restart
* @brief     ：槽位断电 返回消息处理
* @param[in] ：MESSAGE_INFO_T *pMsg 消息
               void *pArg           句柄
               void *pPublic        全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
int wireless_mod_restart(MESSAGE_INFO_T *pMsg, void *pArg, void *pPublic)
{
    uint8 ifnum = 0;
    uint8 devid = 0;
    //WIRELESS_DATA_T *ptWireData = (WIRELESS_DATA_T*)pPublic;
    
    if(NULL == pMsg || NULL == pArg || 0 == pMsg->msglen)
    {
        return -1;
    }

    ifnum = ((pMsg->label >> 8) & 0xff);
    devid = (pMsg->label & 0xff);

    WIRELESS_FMT_DEBUG("设备%d重启槽位%d结果: %s\n", ifnum + 1, devid, 0 == *pMsg->playload ? "失败" : "成功");

    /*if(0 != *pMsg->playload)
    {
        if(0 == ifnum)
        {
            ptWireData->pppdata0.workFlag = FALSE;
            ptWireData->atdata0.workFlag = FALSE;
        }
        if(1 == ifnum)
        {
            ptWireData->pppdata1.workFlag = FALSE;
            ptWireData->atdata1.workFlag = FALSE;
        }
    }*/
    return 0;
}


WIRELESS_MSGFUN_T g_tMsgFunc[] = 
{/*  IID,                    IOP,                         rpm, 消息处理函数 */
    {MESSAGE_IID_COMMON,     COMMON_IOP_HERTBEAT,         1,   wireless_heart_bear},
    {MESSAGE_IID_COMMON,     COMMON_IOP_VERSION,          1,   wireless_get_versionmsg},
    {MESSAGE_IID_COMMON,     COMMON_IOP_VERSION,          0,   wireless_app_versionmsg},
    {MESSAGE_IID_COMMON,     COMMON_IOP_GETAPPID,         1,   wireless_get_appidmsg},
    
    {MESSAGE_IID_WIRELESS,   WIRELESS_IOP_GET_MODULEINFO, 1,   wireless_dcmmodule_req},
    {MESSAGE_IID_WIRELESS,   WIRELESS_IOP_GET_DIAGINFO,   1,   wireless_dial_req},
    {MESSAGE_IID_WIRELESS,   WIRELESS_IOP_SEND_SMS,       1,   wireless_sendsms_req},
    {MESSAGE_IID_WIRELESS,   WIRELESS_IOP_REVC_SMS,       1,   wireless_receivesms_req},
    {MESSAGE_IID_WIRELESS,   WIRELESS_IOP_SET_DIAGARG,    1,   wireless_set_diagapn},
    {MESSAGE_IID_WIRELESS,   WIRELESS_IOP_REDIAG,         1,   wireless_redial_req},
    {MESSAGE_IID_WIRELESS,   WIRELESS_IOP_GET_GPS,        1,   wireless_gps_req},
    //{MESSAGE_IID_WIRELESS,   WIRELESS_IOP_SET_AUTH,       1,   wireless_auth_req},
    {MESSAGE_IID_WIRELESS,   WIRELESS_IOP_SET_SMSNO,      1,   wireless_smscenter_req},
    {MESSAGE_IID_WIRELESS,   WIRELESS_IOP_GET_NUM,        1,   wireless_diagstats_req},
    {MESSAGE_IID_WIRELESS,   WIRELESS_IOP_TRANS_AT,       1,   wireless_transport_proc},


    {MESSAGE_IID_MODULE,     MODULE_IOP_GET_INFO,         0,   wireless_modinfo_ack},
    {MESSAGE_IID_MODULE,     MODULE_IOP_GET_PORTINFO,     0,   wireless_modport_ack},
    {MESSAGE_IID_MODULE,     MODULE_IOP_GET_STATUS,       0,   wireless_modonline_ack},
    {MESSAGE_IID_MODULE,     MODULE_IOP_STATUS_EVENT,     1,   wireless_mod_event},
    {MESSAGE_IID_MODULE,     MODULE_IOP_RESTORE,          0,   wireless_mod_restore},
    {MESSAGE_IID_MODULE,     MODULE_IOP_RESTART,          0,   wireless_mod_restart},

    {MESSAGE_IID_SELF,       SELF_IOP_GET_VERSION,        1,   wireless_get_selfversion},
};
uint8 g_nMsgNum = sizeof(g_tMsgFunc)/sizeof(WIRELESS_MSGFUN_T);

/**********************************************************************
* @name      : wireless_mqttproc_init
* @brief     ：mqtt消息处理初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int wireless_mqttproc_init(void *pPublic, void **ppPrivate)
{
    WIRELESS_MSGHANDLE_T *ptMsgData = NULL;

    ptMsgData = (WIRELESS_MSGHANDLE_T *)malloc(sizeof(WIRELESS_MSGHANDLE_T));
    if(NULL == ptMsgData)
    {
        MQTTMSG_FMT_DEBUG("wireless_mqttproc_init malloc failed\n");
        return -1;
    }
    
    memset(ptMsgData, 0, sizeof(WIRELESS_MSGHANDLE_T));
    ptMsgData->AppStep = WIRELESS_STATUS_REGAPP;

    *ppPrivate = ptMsgData;
    return 0;
}

/**********************************************************************
* @name      : wireless_mqttmsg_proc
* @brief     ：mqtt消息处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2021-3-22
* @Update    :
**********************************************************************/
int wireless_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int         ret = 0;
    int         recvlen = 0;
    uint8       i;
    char        topic[256] = {0};
    WIRELESS_MSGHANDLE_T *ptMsgHandle = (WIRELESS_MSGHANDLE_T*)pPrivate;
    WIRELESS_DATA_T      *ptWireData = (WIRELESS_DATA_T*)pPublic;
    struct timespec       curtime = {0};
    MESSAGE_INFO_T       *pMsgInfo = NULL;
    
    /* 1 收消息 */
    recvlen = wireless_recv_msg(topic, ptMsgHandle->recvBuf, WIRELESS_MQTT_MSG_LEN);
    if(recvlen > 0)
    {
        ret = appmsg_split(ptMsgHandle->recvBuf, (uint16)recvlen, &pMsgInfo);
        if(0 == ret && NULL != pMsgInfo)
        {
            /* 用于线程维护日志记录 */
            TASK_SET_USERLOG(pArg, "mqtt msg from %s iid %04x, iop %04x", pMsgInfo->souraddr, pMsgInfo->IID, pMsgInfo->IOP);
            MQTTMSG_BUF_TRACE(ptMsgHandle->recvBuf, recvlen, "Recv msg from %s, iid 0x%x, iop 0x%x", 
                              pMsgInfo->souraddr, pMsgInfo->IID, pMsgInfo->IOP);
            if(0 == strcmp(pMsgInfo->souraddr, WIRELESS_DCM_NAME))
            {
                /* 消除自发自收 */
                WIRELESS_FMT_DEBUG("Revc msg form %s is the same name, iid %d, iop %d\n", 
                                    pMsgInfo->souraddr, pMsgInfo->IID, pMsgInfo->IOP);
                goto freemsg;
            }

            /* 注册应答 */
            if(MESSAGE_IID_SYSTEM == pMsgInfo->IID && SYSTEM_IOP_APP_REGISTER == pMsgInfo->IOP)
            {
                if(WIRELESS_STATUS_REGAPP == ptMsgHandle->AppStep)
                {
                    ptMsgHandle->AppStep = WIRELESS_STATUS_WORK;
                    printf("Wireless register success\n");
                    WIRELESS_FMT_DEBUG("无线注册成功\n");
                    if(0 != ret)
                    {
                        WIRELESS_FMT_DEBUG("wireless_seek_app failed %d\n", ret);
                    }
                    BASE_TASK_SET_OK(pArg); //基础任务完成准备工作
                    goto freemsg;
                }
            }

//#if defined PRODUCT_CCU || defined PRODUCT_ECU || defined PRODUCT_ECU_G
            if(pMsgInfo->IID == MESSAGE_IID_DESKGUI)
            {
                appGUI_recv_back_handle(pMsgInfo);
                goto freemsg;
            }
//#endif
            for(i = 0; i < g_nMsgNum; i++)
            {
                if(g_tMsgFunc[i].IID == pMsgInfo->IID && g_tMsgFunc[i].IOP == pMsgInfo->IOP && 
                   g_tMsgFunc[i].rpm == pMsgInfo->rpm)
                {
                    if(g_tMsgFunc[i].pProcMsg)
                    {
                        g_tMsgFunc[i].pProcMsg(pMsgInfo, (void *)ptMsgHandle, (void *)ptWireData);
                        goto freemsg;
                    }
                }
            }
            
            if(i == g_nMsgNum && !(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_ERRMSG == pMsgInfo->IOP))
            {
                wireless_send_errmsg(pMsgInfo, -1);
            }
freemsg:
            free(pMsgInfo);
            pMsgInfo = NULL;
        }
    }

    /* 2. 定时处理 */
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    if(curtime.tv_sec - ptMsgHandle->timeflag < 5)
    {
        return 0;
    }
    ptMsgHandle->timeflag = curtime.tv_sec;
    /* APP相关 */
    switch(ptMsgHandle->AppStep)
    {
        case WIRELESS_STATUS_REGAPP:
        {
            wireless_login_to_smios(ptWireData->appid, ptWireData->appidlen);
            break;
        }
        default: break;
    }
    return 0;
}
#endif

#if DESC("公共操作", 1)
uint8  g_SFRebootNeed = FALSE;
uint8  g_HWRebootNeed = FALSE;
uint32 g_HWOFFTime = 5;

uint8  g_needChangeType = FALSE;
uint8  g_ChangeType = 0;

/**********************************************************************
* @name      : wireless_soft_reboot
* @brief     ：模块软复位AT指令
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-3-27
* @Update    :
**********************************************************************/
void wireless_soft_reboot()
{
    g_SFRebootNeed = TRUE;
}

/**********************************************************************
* @name      : wireless_hw_reboot
* @brief     ：模块硬件复位
* @param[in] ：uint32 powerOffTime    断电时间s(3~120)
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-3-27
* @Update    :
**********************************************************************/
void wireless_hw_reboot(uint32 powerOffTime)
{
    g_HWRebootNeed = TRUE;
    g_HWOFFTime = powerOffTime;
}

/**********************************************************************
* @name      : wireless_change_type
* @brief     ：切换拨号方式
* @param[in] ：uint8 type
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-4-14
* @Update    :
**********************************************************************/
void wireless_change_type(uint8 type)
{
    if(type <= WIRELESS_COMMTYPE_UART)
    {
        g_ChangeType = type;
        g_needChangeType = TRUE;
    }
}

pthread_mutex_t g_fWIrelessReStart;
uint8 g_rebootdev[2] = {0};
/**********************************************************************
* @name      : wireless_restart_dev
* @brief     ：重启模块设置标记
* @param[in] ：uint8 ifnum   设备编号(0-1)
               uint8 devid   模组位置(1-5, 0-设置无效) 
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
void wireless_restart_dev(uint8 ifnum, uint8 devid)
{
    if(ifnum < 2)
    {
        pthread_mutex_lock(&g_fWIrelessReStart);
        g_rebootdev[ifnum] = devid;
        pthread_mutex_unlock(&g_fWIrelessReStart);
        if(0 == wireless_check_1_8G())
        {
            wireless_soft_reboot();
        }
        else
        {
            wireless_hw_reboot(10);
        }
    }
}

/**********************************************************************
* @name      : wireless_restart_dev
* @brief     ：重启模块设置标记
* @param[in] ：uint8 ifnum   设备编号(0-1)
* @param[out]：uint8 devid   模组位置(1-5)
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint8 wireless_get_restartdev(uint8 ifnum)
{
    uint8 id = 0;
    if(ifnum < 2)
    {
        pthread_mutex_lock(&g_fWIrelessReStart);
        id = g_rebootdev[ifnum];
        pthread_mutex_unlock(&g_fWIrelessReStart);
    }
    return id;
}


/**********************************************************************
* @name      : wireless_num_manager
* @brief     ：拨号次数维护
* @param[in] ：WIRELESS_MANAGER_T *ptWireData     全局数据
               uint8 ifnum                        设备序号
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-5-31
* @Update    :
**********************************************************************/
void wireless_num_manager(WIRELESS_DATA_T *ptWireData, uint8 ifnum)
{
    WIRELESS_HANDLE_T *ptHandle = NULL;
    TIME_STRUCT_T curDay; /* 当前日期 */
    
    get_curtime_ms(&curDay);
    ptHandle = ifnum == 0 ? &(ptWireData->pppdata0) : &(ptWireData->pppdata1);
    if(curDay.year != ptHandle->curDay.year || curDay.month != ptHandle->curDay.month || 
       curDay.day != ptHandle->curDay.day)
    {
        ptHandle->curDay.year = curDay.year;
        ptHandle->curDay.month = curDay.month;
        ptHandle->curDay.day = curDay.day;

        ptHandle->ConnectNum = 0;
        ptHandle->FailedNum = 0;
        wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 0, ptHandle->ConnectNum);
        wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
    }
}

#endif

#if DESC("模组相关操作", 1)
/**********************************************************************
* @name      : wireless_get_modport
* @brief     ：组织获取模组端口消息
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-9
* @Update    :
**********************************************************************/
int wireless_get_mapVersion()
{
    int   ret = 0;
    ret =  wireless_send_modmsg(0, MESSAGE_IID_COMMON, COMMON_IOP_VERSION, NULL, 0);
    MQTTMSG_FMT_TRACE("发送查询模组管理器版本, 结果%d\n", ret);
    return ret;
}

/**********************************************************************
* @name      : wireless_get_modport
* @brief     ：组织获取模组端口消息
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-9
* @Update    :
**********************************************************************/
int wireless_get_modport()
{
    uint8 sendbuf[128] = {0};
    uint8 size = 0;
    int   ret = 0;
    size = strlen(MODE_DIAG_TYPE_NAME);
    sendbuf[0] = size;
    memcpy(sendbuf + 1, MODE_DIAG_TYPE_NAME, size);
    
    ret =  wireless_send_modmsg(0, MESSAGE_IID_MODULE, MODULE_IOP_GET_PORTINFO, sendbuf, size + 1);
    MQTTMSG_FMT_DEBUG("发送查询模组端口, 结果%d\n", ret);
    return ret;
}

/**********************************************************************
* @name      : wireless_pingmsg_modinfo
* @brief     ：组织获取模组终端信息消息
* @param[in] ：WIRELESS_MANAGER_T *ptHandle       句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-9
* @Update    :
**********************************************************************/
int wireless_get_modinfo(WIRELESS_MANAGER_T *ptHandle)
{
    uint32 port = 0;
    uint32 temp = 0;
    int    ret = 0;
    if(ptHandle->DCM0Port > 0 && WIRELESS_SEEK_INFO == ptHandle->DCM0Status)
    {
        port = (ptHandle->DCM0Port & 0x0f);
        appmsg_memcpy_r(&temp, &port, sizeof(int));
        ret = wireless_send_modmsg(ptHandle->DCM0Port, MESSAGE_IID_MODULE, MODULE_IOP_GET_INFO, 
                                   (uint8 *)&temp, sizeof(int));
        MQTTMSG_FMT_DEBUG("发送查询模组端口%02X信息, 结果%d\n", ptHandle->DCM0Port, ret);
    }
    if(ptHandle->DCM1Port > 0 && WIRELESS_SEEK_INFO == ptHandle->DCM1Status)
    {
        port = (ptHandle->DCM1Port & 0x0f);
        appmsg_memcpy_r(&temp, &port, sizeof(int));
        ret |= wireless_send_modmsg(ptHandle->DCM1Port, MESSAGE_IID_MODULE, MODULE_IOP_GET_INFO, 
                                    (uint8 *)&temp, sizeof(int));
        MQTTMSG_FMT_DEBUG("发送查询模组端口%02X信息, 结果%d\n", ptHandle->DCM1Port, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : wireless_get_modonline
* @brief     ：组织查询模组插拔状态消息
* @param[in] ：WIRELESS_MANAGER_T *ptMsgHandle       句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-29
* @Update    :
**********************************************************************/
int wireless_get_modonline(WIRELESS_MANAGER_T *ptHandle)
{
    int port = 0;
    int temp = 0;
    int ret = 0;
    if(ptHandle->DCM0Port > 0 && WIRELESS_SEEK_ONLINE == ptHandle->DCM0Status)
    {
        temp = ptHandle->DCM0Port;
        appmsg_memcpy_r(&port, &temp, sizeof(int));
        ret = wireless_send_modmsg(ptHandle->DCM0Port, MESSAGE_IID_MODULE, MODULE_IOP_GET_STATUS, 
                                   (uint8 *)&port, sizeof(int));
        MQTTMSG_FMT_DEBUG("发送查询模组端口%02X状态, 结果%d\n", ptHandle->DCM0Port, ret);

    }
    if(ptHandle->DCM1Port > 0 && WIRELESS_SEEK_ONLINE == ptHandle->DCM1Status)
    {
        temp = ptHandle->DCM1Port;
        appmsg_memcpy_r(&port, &temp, sizeof(int));
        ret = wireless_send_modmsg(ptHandle->DCM1Port, MESSAGE_IID_MODULE, MODULE_IOP_GET_STATUS, 
                                   (uint8 *)&port, sizeof(int));
        MQTTMSG_FMT_DEBUG("发送查询模组端口%02X状态, 结果%d\n", ptHandle->DCM1Port, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : wireless_mode_restart_proc
* @brief     ：模组复位-重启操作
* @param[in] ：WIRELESS_MANAGER_T *ptWireData     全局数据
               WIRELESS_MANAGER_T *ptHandle       线程句柄
               uint8 ifnum                        设备序号(0-1)
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-5-12
* @Update    :
**********************************************************************/
void wireless_mode_restart_proc(WIRELESS_DATA_T *ptWireData, WIRELESS_MANAGER_T *ptHandle, uint8 ifnum)
{
    uint8  devid = 0;
    struct timespec curtime = {0};
    WIRELESS_HANDLE_T   *ptATdata = NULL;
    WIRELESS_HANDLE_T   *ptPPPdata = NULL;
    
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    
    if(TRUE == ptHandle->DCMRestore[ifnum] && ptHandle->tDCMStart[ifnum] + 16 <= curtime.tv_sec)
    {
        /* 防止重启时间过长 */
        ptHandle->DCMRestore[ifnum] = FALSE;
    }
    
    /* 重启模块检查 */
    devid = wireless_get_restartdev(ifnum);
    if(0 == (devid & 0x0f))
    {
        return;
    }

    if(0 == ifnum)
    {
        ptATdata = &(ptWireData->atdata0);
        ptPPPdata = &(ptWireData->pppdata0);
        if(devid != ptHandle->DCM0Port)
        {
            /* 复位成功, 模组离线, DCM0Port刷新为0 */
            ptHandle->lastPort = devid;
            ptHandle->lastRestart = curtime.tv_sec;
            ptHandle->DCMRestore[ifnum] = FALSE;
            WIRELESS_FMT_DEBUG("模组%02X复位流程结束!\n", devid);
            wireless_restart_dev(0, 0);
            return;
        }
    }
    else if(1 == ifnum)
    {
        ptATdata = &(ptWireData->atdata1);
        ptPPPdata = &(ptWireData->pppdata1);
        if(devid != ptHandle->DCM1Port)
        {
            /* 复位成功, 模组离线, DCM1Port刷新为0 */
            ptHandle->lastPort = devid;
            ptHandle->lastRestart = curtime.tv_sec;
            ptHandle->DCMRestore[ifnum] = FALSE;
            WIRELESS_FMT_DEBUG("模组%02X复位流程结束!\n", devid);
            wireless_restart_dev(1, 0);
            return;
        }
    }
    else
    {
        return;
    }
    
    if(ptHandle->lastPort == devid && curtime.tv_sec < ptHandle->lastRestart + 11)
    {
        /* 10s内复位相同的模块 不处理 */
        WIRELESS_FMT_DEBUG("11s内复位相同的模组%X, 不处理!\n", devid);
        wireless_restart_dev(ifnum, 0);
        return;
    }

    ptATdata->workAgain = TRUE;
    ptPPPdata->workAgain = TRUE;
    if(NULL != ptATdata->pUart || NULL != ptPPPdata->pUart)
    {
        /* 等待句柄全部关闭 */
        return;
    }

    if(FALSE == ptHandle->DCMRestore[ifnum])
    {
        WIRELESS_FMT_DEBUG("模组%2X资源释放完成,开始复位\n", devid);
        if(curtime.tv_sec <= ptHandle->lastRestart + 5)
        {
            WIRELESS_FMT_DEBUG("%ds前模组%2X断电, 等待%ds复位模组%X\n", curtime.tv_sec - ptHandle->lastRestart, 
                                ptHandle->lastPort, ptHandle->lastRestart + 5 - curtime.tv_sec, devid);
            sleep(ptHandle->lastRestart + 5 - curtime.tv_sec);
        }
        wireless_restore_mode(ifnum, devid);
        ptHandle->DCMRestore[ifnum] = TRUE;
        ptHandle->tDCMStart[ifnum] = curtime.tv_sec;
    }
    else if(ptHandle->tDCMStart[ifnum] + 10 <= curtime.tv_sec)
    {
        WIRELESS_FMT_DEBUG("模组%2X复位失败, 10s内没有离线, 重启槽位%d\n", devid, devid & 0x0f);
        if(curtime.tv_sec <= ptHandle->lastRestart + 5)
        {
            if((ptHandle->lastPort & 0x0f) == (devid & 0x0f))
            {
                WIRELESS_FMT_DEBUG("%ds前模组%2X断电, 相同槽位, 结束\n", curtime.tv_sec - ptHandle->lastRestart, 
                                    ptHandle->lastPort);
                wireless_restart_dev(ifnum, 0);
                ptHandle->DCMRestore[ifnum] = FALSE;
                return;
            }
            else
            {
                WIRELESS_FMT_DEBUG("%ds前模组%2X断电, 等待%ds重启槽位%d\n", curtime.tv_sec - ptHandle->lastRestart, 
                                    ptHandle->lastPort, ptHandle->lastRestart + 5 - curtime.tv_sec, devid & 0x0f);
                sleep(ptHandle->lastRestart + 5 - curtime.tv_sec);
            }
        }
        wireless_reboot_mode(ifnum, devid & 0x0f);
        wireless_restart_dev(ifnum, 0);
        ptHandle->lastPort = devid;
        ptHandle->lastRestart = curtime.tv_sec;
        ptHandle->DCMRestore[ifnum] = FALSE;
    }
}

/**********************************************************************
* @name      : wireless_manager_init
* @brief     ：资源管理初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int wireless_manager_init(void *pPublic, void **ppPrivate)
{
    WIRELESS_DATA_T      *ptWireData = (WIRELESS_DATA_T*)pPublic;

    while(FALSE == ptWireData->bMapExist)
    {
        sleep(1);
    }

    WIRELESS_FMT_DEBUG("模组方式\n");
    pthread_mutex_init(&g_fWIrelessReStart, NULL);

    memset(&g_Manager, 0, sizeof(WIRELESS_MANAGER_T));
    g_Manager.GetStatus = TRUE;

    while(FALSE == ptWireData->mapReg)
    {
        sleep(2);
        wireless_get_mapVersion();
    }
    *ppPrivate = &g_Manager;

    return 0;
}

/**********************************************************************
* @name      : wireless_manager_proc
* @brief     ：资源管理处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2021-3-22
* @Update    :
**********************************************************************/
int wireless_manager_proc(void *pArg, void *pPublic, void *pPrivate)
{
    struct timespec curtime = {0};
    WIRELESS_DATA_T    *ptWireData = (WIRELESS_DATA_T*)pPublic;
    WIRELESS_MANAGER_T *ptHandle = (WIRELESS_MANAGER_T*)pPrivate;

    clock_gettime(CLOCK_MONOTONIC, &curtime);
    /* 模块操作 */
    if(curtime.tv_sec > ptHandle->timeFlag + 2)
    {
        if(TRUE == ptHandle->GetStatus)
        {
            wireless_get_modport();
        }
        else
        {
            wireless_get_modinfo(ptHandle);
            wireless_get_modonline(ptHandle);
        }
        ptHandle->timeFlag = curtime.tv_sec;
    }
    
    /* 重启模块检查 */
    wireless_mode_restart_proc(ptWireData, ptHandle, 0);
    wireless_mode_restart_proc(ptWireData, ptHandle, 1);

    /* 数量维护 */
    wireless_num_manager(ptWireData, 0);
    wireless_num_manager(ptWireData, 1);

    return 10000;
}

#endif

#if DESC("集中器相关操作", 1)

uint8 g_power_set = FALSE;

void wireless_power_status(uint8 status)
{
    g_power_set = status;
}

//通信模块识别
#define WIRELESS_DEV_GET_INIT          0
//通信模块识别
#define WIRELESS_DEV_GET_MODULE        1
//通信模块重启(启动)
#define WIRELESS_DEV_RESTART           2
//判断通信方式是USB还是串口
#define WIRELESS_DEV_COMM_TYPE         3
//准备工作
#define WIRELESS_DEV_WORK_PREPARE      4
//工作检查
#define WIRELESS_DEV_WORK_CHECK        5
//软件复位
#define WIRELESS_DEV_SOFT_RESTART      6
//切换串口
#define WIRELESS_DEV_CHANGE_TYPE       7

/**********************************************************************
* @name      : wireless_module_check
* @brief     ：获取模块类型
* @param[in] WIRELESS_CCU_T *ptHandle    线程句柄
* @param[out]：
* @return    ：下一个状态
* @Create    : 王津剑
* @Date      ：2022-3-25
* @Update    :
**********************************************************************/
uint8 wireless_module_get(WIRELESS_CCU_T *ptHandle)
{
    int ret = 0;
    REMOTE_CU_DEVICE_T *pdev = (REMOTE_CU_DEVICE_T *)ptHandle->pDevHal;
    int status = 0;

    ret = pdev->remote_cu_insert_state_get(pdev, &status);
    if(0 == ret && 1 == status)
    {
        ptHandle->bInsert = TRUE;
    }
    else
    {
        ptHandle->bInsert = FALSE;
        g_power_set = FALSE;
    }
    if(TRUE == ptHandle->bInsert)
    {
        wireless_send_diagStatus(1, "检测到通信模块插入", strlen("检测到通信模块插入"), 10);
        WIRELESS_FMT_DEBUG("检测到通信模块插入\n");
        return WIRELESS_DEV_RESTART;
    }
    return WIRELESS_DEV_GET_MODULE;
}

/**********************************************************************
* @name      : wireless_restart_module
* @brief     ：复位通信模块（断电）
* @param[in] ：WIRELESS_CCU_T *ptHandle    线程句柄
               uint32 offtimes             断电时间s(3~120)
* @param[out]：
* @return    ：0-成功/其他-失败
* @Create    : 王津剑
* @Date      ：2022-3-25
* @Update    :
**********************************************************************/
int wireless_restart_module(WIRELESS_CCU_T *ptHandle, uint32 offtimes)
{
    int ret = 0;
    char showtmp[128] = {0};
    
    REMOTE_CU_DEVICE_T *pdev = (REMOTE_CU_DEVICE_T *)ptHandle->pDevHal;

    if(0 == offtimes)
    {
        if(0 == wireless_check_1_8G())
        {
            offtimes = 5;
        }
        else
        {
            offtimes = 10;
        }
    }
    else if(offtimes < 3)
    {
        offtimes = 3;
    }
    else if(offtimes > 120)
    {
        offtimes = 120;
    }

#ifdef PRODUCT_ZCU_1
    if(TRUE == g_power_set)
    {
        ret = pdev->remote_cu_power_set(pdev, 0);
        WIRELESS_FMT_DEBUG("通信模块断电(时间%ds)%s\n", offtimes, 0 == ret ? "成功" : "失败");
        if(0 != ret)
        {
            return ret;
        }
        sprintf(showtmp, "通信模块断电%ds", offtimes);
        wireless_send_diagStatus(1, showtmp, strlen(showtmp), offtimes);
        sleep(offtimes);
    }
#else
    ret = pdev->remote_cu_power_set(pdev, 0);
    WIRELESS_FMT_DEBUG("通信模块断电(时间%ds)%s\n", offtimes, 0 == ret ? "成功" : "失败");
    if(0 != ret)
    {
        return ret;
    }
    sprintf(showtmp, "通信模块断电%ds", offtimes);
    wireless_send_diagStatus(1, showtmp, strlen(showtmp), offtimes);
    sleep(offtimes);
#endif

    ret = pdev->remote_cu_power_set(pdev, 1);
    WIRELESS_FMT_DEBUG("通信模块上电%s\n",  0 == ret ? "成功" : "失败");
    if(0 != ret)
    {
        return ret;
    }

    ret = pdev->remote_cu_on_level_set(pdev, 1);
    WIRELESS_FMT_DEBUG("远程通信单元开机电平拉高%s\n", 0 == ret ? "成功" : "失败");
    if(0 != ret)
    {
        return ret;
    }
    sleep(1);

    ret = pdev->remote_cu_reset(pdev);
    WIRELESS_FMT_DEBUG("通信模块复位%s\n", 0 == ret ? "成功" : "失败");
    if(0 != ret)
    {
        return ret;
    }

    sleep(1);
    ret = pdev->remote_cu_on_level_set(pdev, 0);
    WIRELESS_FMT_DEBUG("远程通信单元开机电平拉低%s\n", 0 == ret ? "成功" : "失败");
    if(0 != ret)
    {
        return ret;
    }
    sleep(3);

    ret = pdev->remote_cu_on_level_set(pdev, 1);
    WIRELESS_FMT_DEBUG("远程通信单元开机电平拉高%s\n", 0 == ret ? "成功" : "失败");
    if(0 != ret)
    {
        return ret;
    }
    
    sleep(1);
    WIRELESS_FMT_DEBUG("通信模块断电重启结束\n");
    return ret;
}

/**********************************************************************
* @name      : wireless_module_restart
* @brief     ：模块复位
* @param[in] ：WIRELESS_CCU_T *ptHandle    线程句柄
* @param[out]：
* @return    ：下一个状态
* @Create    : 王津剑
* @Date      ：2022-3-25
* @Update    :
**********************************************************************/
uint8 wireless_module_restart(WIRELESS_CCU_T *ptHandle, WIRELESS_DATA_T *ptWireData)
{
    int                 ret = 0;
    REMOTE_CU_DEVICE_T *pdev = (REMOTE_CU_DEVICE_T *)ptHandle->pDevHal;
    uint32              count = 0;
    WIRELESS_HANDLE_T  *ptATdata = &(ptWireData->atdata0);
    WIRELESS_HANDLE_T  *ptPPPdata = &(ptWireData->pppdata0);
    struct              timespec curtime = {0};
    uint32              offtime = ptHandle->powerofftime;
    uint8               bpoweroff = FALSE;
    int status = 0;

    ptHandle->powerofftime = 0;

    if(0 != wireless_check_1_8G())
    {
        /* 江苏要求模块断电前poweroff */
        if(NULL != ptATdata->pUart)
        {
            WIRELESS_FMT_DEBUG("通信模块%d软关机\n", 1);
            at_check(1, ptATdata->pUart, "AT$MYPOWEROFF\r\n", "OK");
            bpoweroff = TRUE;
        }
        else if(NULL != ptPPPdata->pUart)
        {
            WIRELESS_FMT_DEBUG("通信模块%d软关机\n", 1);
            at_check(1, ptPPPdata->pUart, "AT$MYPOWEROFF\r\n", "OK");
            bpoweroff = TRUE;
        }
    }

    /* 复位之前释放句柄 */
    ptATdata->workAgain = TRUE;
    ptATdata->workFlag = FALSE;
    ptATdata->commport = 0;
    sleep(1);
    ptPPPdata->workAgain = TRUE;
    ptPPPdata->workFlag = FALSE;
    ptPPPdata->commport = 0;
    while(count++ < 200)
    {
        if(NULL == ptATdata->pUart && NULL == ptPPPdata->pUart)
        {
            break;
        }
        usleep(100000);
    }

    /* 重新拨号后再次检查模块 */
    memset(ptPPPdata->ptNetPara->IMEI, 0, 17);
    memset(ptPPPdata->ptNetPara->VERINFO.manuNum, 0, 4 + 1);
    memset(ptPPPdata->ptNetPara->VERINFO.softversion, 0, 4 + 1);
    memset(ptPPPdata->ptNetPara->VERINFO.softdata, 0, 6 + 1);
    memset(ptPPPdata->ptNetPara->VERINFO.hwversion, 0, 4 + 1);
    memset(ptPPPdata->ptNetPara->VERINFO.hwdata, 0, 6 + 1);
    memset(ptPPPdata->ptNetPara->VERINFO.manuExten, 0, 8 + 1);

    clock_gettime(CLOCK_MONOTONIC, &curtime);
    ptHandle->restartTime = curtime.tv_sec;

    if(TRUE == bpoweroff)
    {
        /* 江苏要求poweroff等待10s */
        sleep(10);
    }
    else
    {
        sleep(2);
    }

    WIRELESS_FMT_DEBUG("句柄全部释放, 开始模块硬复位\n");
    /* 模块复位 */
    ret = wireless_restart_module(ptHandle, offtime);
    if(0 != ret)
    {
        return WIRELESS_DEV_GET_MODULE;
    }

    clock_gettime(CLOCK_MONOTONIC, &curtime);
    ptHandle->restartTime = curtime.tv_sec;

    count = 0;
    while(count++ < 10)
    {
        sleep(1);
        ret = pdev->remote_cu_insert_state_get(pdev, &status);
        if(0 == ret && 1 == status)
        {
            ptHandle->bInsert = TRUE;
        }
        else
        {
            ptHandle->bInsert = FALSE;
        }
        if(TRUE == ptHandle->bInsert)
        {
            return WIRELESS_DEV_COMM_TYPE;
        }
    }
    return WIRELESS_DEV_GET_MODULE;
}

/**********************************************************************
* @name      : wireless_module_commtype
* @brief     ：模块通信方式
* @param[in] WIRELESS_CCU_T *ptHandle    线程句柄
* @param[out]：
* @return    ：下一个状态
* @Create    : 王津剑
* @Date      ：2022-3-25
* @Update    :
**********************************************************************/
uint8 wireless_module_commtype(WIRELESS_CCU_T *ptHandle)
{
    uint32 count = 0;
    void  *pUartAT = NULL;
    void  *pUartPPP = NULL;
    ptHandle->bUsb = FALSE;
    ptHandle->commType = WIRELESS_COMMTYPE_UART;

    while(count++ < 13)
    {
        sleep(2);
        pUartAT = wireless_uart_init(UART_USB_AT_DEVICE_ID_STR);
        pUartPPP = wireless_uart_init(UART_USB_DIAG_DEVICE_ID_STR);
        wireless_uart_uninit(pUartAT);
        wireless_uart_uninit(pUartPPP);
        if(NULL != pUartAT && NULL != pUartPPP)
        {
            ptHandle->bUsb = TRUE;
            ptHandle->commType = WIRELESS_COMMTYPE_USB;
            break;
        }
    }

    /* 特殊要求 */
    if(0 == access("/data/app/wirelessDCM/data/bForceUart", F_OK))
    {
        ptHandle->bForceUrat = TRUE;
        ptHandle->bUsb = FALSE;
        ptHandle->commType = WIRELESS_COMMTYPE_UART;
        Isusb =0;
        WIRELESS_FMT_DEBUG("强制使用串口方式\n");
    }
    else if(0 == access("/data/app/wirelessDCM/data/bForceUsb", F_OK))
    {
        ptHandle->bForceUrat = FALSE;
        ptHandle->bUsb = TRUE;
        ptHandle->commType = WIRELESS_COMMTYPE_USB;
        Isusb =0xAA;
        WIRELESS_FMT_DEBUG("强制使用USB方式\n");
    }
    else
    {
        ptHandle->bForceUrat = FALSE;
        Isusb =0;
    }

    g_SFRebootNeed = FALSE;
    g_HWRebootNeed = FALSE;
    
    WIRELESS_FMT_DEBUG("通信模块%s支持USB, 使用%s方式拨号\n", TRUE == ptHandle->bUsb ? "" : "不", 
                                                          WIRELESS_COMMTYPE_USB == ptHandle->commType ? "USB" : "串口");
    if(0 != wireless_check_1_8G())
    {
        sleep(10);
        WIRELESS_FMT_DEBUG("江苏增加10s延时\n");
    }

    return WIRELESS_DEV_WORK_PREPARE;
}

/**********************************************************************
* @name      : wireless_module_setwork
* @brief     ：开始拨号
* @param[in] ：WIRELESS_CCU_T *ptHandle    线程句柄
* @param[out]：
* @return    ：下一个状态
* @Create    : 王津剑
* @Date      ：2022-3-25
* @Update    :
**********************************************************************/
uint8 wireless_module_setwork(WIRELESS_CCU_T *ptHandle, WIRELESS_DATA_T *ptWireData)
{
    WIRELESS_HANDLE_T *ptATdata = &(ptWireData->atdata0);
    WIRELESS_HANDLE_T *ptPPPdata = &(ptWireData->pppdata0);
    void  *pUartAT = NULL;
    void  *pUartPPP = NULL;
    uint32 count = 0;
    
    /* 设置之前释放句柄 */
    ptATdata->workAgain = TRUE;
    ptATdata->workFlag = FALSE;
    ptATdata->commport = 0;
    sleep(1);
    ptPPPdata->workAgain = TRUE;
    ptPPPdata->workFlag = FALSE;
    ptPPPdata->commport = 0;
    while(count++ < 200)
    {
        if(NULL == ptATdata->pUart && NULL == ptPPPdata->pUart)
        {
            break;
        }
        usleep(100000);
    }

    ptATdata->commtype = ptHandle->commType;
    ptPPPdata->commtype = ptHandle->commType;
    memset(ptATdata->devName, 0, 256);
    memset(ptATdata->halName, 0, 32);
    memset(ptPPPdata->devName, 0, 256);
    memset(ptPPPdata->halName, 0, 32);
    if(WIRELESS_COMMTYPE_USB == ptHandle->commType)
    {
        /* USB一起运行 */
        pUartAT = wireless_uart_init(UART_USB_AT_DEVICE_ID_STR);
        pUartPPP = wireless_uart_init(UART_USB_DIAG_DEVICE_ID_STR);
        
        wireless_uart_getname(pUartAT, (uint8*)ptATdata->devName);
        memcpy(ptATdata->halName, UART_USB_AT_DEVICE_ID_STR, strlen(UART_USB_AT_DEVICE_ID_STR));
        
        wireless_uart_getname(pUartPPP, (uint8*)ptPPPdata->devName);
        memcpy(ptPPPdata->halName, UART_USB_DIAG_DEVICE_ID_STR, strlen(UART_USB_DIAG_DEVICE_ID_STR));

        memset(ptPPPdata->ptNetPara->gpsHalName, 0, 32);
        ptATdata->commport = 0xFF;
        ptATdata->workFlag = TRUE;
        ptPPPdata->commport = 0xFF;
        ptPPPdata->workFlag = TRUE;
        wireless_uart_uninit(pUartAT);
        wireless_uart_uninit(pUartPPP);
    }
    else
    {
        /* 串口先拨号 */
        pUartPPP = wireless_uart_init(UART_REMOTE_CU_DEVICE_ID_STR);
        wireless_uart_getname(pUartPPP, (uint8*)ptPPPdata->devName);
        memcpy(ptPPPdata->halName, UART_REMOTE_CU_DEVICE_ID_STR, strlen(UART_REMOTE_CU_DEVICE_ID_STR));
        ptPPPdata->commport = 0xFF;
        ptPPPdata->workFlag = TRUE;
        ptATdata->ptNetPara->smsStatusOK = TRUE;
        wireless_uart_uninit(pUartPPP);
    }
    g_Manager.DCM0Port = 0xFF;
    
    return WIRELESS_DEV_WORK_CHECK;
}

/**********************************************************************
* @name      : wireless_module_working
* @brief     ：模块检查
* @param[in] ：WIRELESS_CCU_T *ptHandle    线程句柄
* @param[out]：
* @return    ：下一个状态
* @Create    : 王津剑
* @Date      ：2022-3-25
* @Update    :
**********************************************************************/
uint8 wireless_module_working(WIRELESS_CCU_T *ptHandle, WIRELESS_DATA_T *ptWireData)
{
    struct  timespec curtime = {0};
    
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    
    if(TRUE == g_HWRebootNeed || TRUE == g_SFRebootNeed)
    {
        if(curtime.tv_sec <= ptHandle->restartTime + 10)
        {
            WIRELESS_FMT_DEBUG("10s内存在一次复位, 不执行\n");
            g_SFRebootNeed = FALSE;
            g_HWRebootNeed = FALSE;
        }
        else
        {
            /* 硬件复位优先 */
            wireless_send_change(0, WIRELESS_OPT_NONE, 0);
            ptHandle->powerofftime = g_HWOFFTime;
            return TRUE == g_HWRebootNeed ? WIRELESS_DEV_RESTART : WIRELESS_DEV_SOFT_RESTART;
        }
    }

    if(TRUE == g_needChangeType)
    {
        if(0 == wireless_check_1_8G())
        {
            wireless_send_change(0, WIRELESS_OPT_NONE, 0);
            return WIRELESS_DEV_CHANGE_TYPE;
        }
    }

    /* 模块插拔检测 */
    if(curtime.tv_sec != ptHandle->checkTime)
    {
        WIRELESS_HANDLE_T *ptATdata = &(ptWireData->atdata0);
        WIRELESS_HANDLE_T *ptPPPdata = &(ptWireData->pppdata0);
        REMOTE_CU_DEVICE_T *pdev = (REMOTE_CU_DEVICE_T *)ptHandle->pDevHal;
        int status = 0;
        int ret =0;
        
        uint32  count = 0;
        
        ptHandle->checkTime = curtime.tv_sec;
        ret = pdev->remote_cu_insert_state_get(pdev, &status);
        if(0 == ret && 1 == status)
        {
            ptHandle->bInsert = TRUE;
        }
        else
        {
            ptHandle->bInsert = FALSE;
        }
        //WIRELESS_FMT_TRACE("通信模块状态%d, 接口返回值ret=%d\n", status, ret);
        if(FALSE == ptHandle->bInsert)
        {
            WIRELESS_FMT_DEBUG("检测到通信模块拔出\n");
            wireless_send_diagStatus(1, "通信模块拔出", strlen("通信模块拔出"), 10);
            g_Manager.DCM0Port = 0;
            /* 释放句柄 */
            ptATdata->workAgain = TRUE;
            ptATdata->workFlag = FALSE;
            ptATdata->commport = 0;
            sleep(1);
            ptPPPdata->workAgain = TRUE;
            ptPPPdata->workFlag = FALSE;
            ptPPPdata->commport = 0;
            while(count++ < 200)
            {
                if(NULL == ptATdata->pUart && NULL == ptPPPdata->pUart)
                {
                    break;
                }
                usleep(100000);
            }

            WIRELESS_FMT_DEBUG("句柄全部释放, 开始检测通信模块状态\n");
            g_power_set = FALSE;
            return WIRELESS_DEV_GET_MODULE;
        }
    }
    
    return WIRELESS_DEV_WORK_CHECK;
}

/**********************************************************************
* @name      : wireless_module_ATrestart
* @brief     ：模块软复位
* @param[in] ：WIRELESS_CCU_T *ptHandle    线程句柄
               WIRELESS_DATA_T *ptWireData     全局操作
* @param[out]：
* @return    ：下一个状态
* @Create    : 王津剑
* @Date      ：2022-3-27
* @Update    :
**********************************************************************/
uint8 wireless_module_sfrestart(WIRELESS_CCU_T *ptHandle, WIRELESS_DATA_T *ptWireData)
{
    int                ret = 0;
    REMOTE_CU_DEVICE_T *pdev = (REMOTE_CU_DEVICE_T *)ptHandle->pDevHal;
    uint32             count = 0;
    WIRELESS_HANDLE_T *ptATdata = &(ptWireData->atdata0);
    WIRELESS_HANDLE_T *ptPPPdata = &(ptWireData->pppdata0);
    struct             timespec curtime = {0};
    
    /* 复位之前释放句柄 */
    ptATdata->workAgain = TRUE;
    ptATdata->workFlag = FALSE;
    ptATdata->commport = 0;
    sleep(1);
    ptPPPdata->workAgain = TRUE;
    ptPPPdata->workFlag = FALSE;
    ptPPPdata->commport = 0;
    while(count++ < 200)
    {
        if(NULL == ptATdata->pUart && NULL == ptPPPdata->pUart)
        {
            break;
        }
        usleep(100000);
    }
    sleep(2);
    WIRELESS_FMT_DEBUG("句柄全部释放, 开始模块复位\n");
    /* 模块复位 */
    ret = pdev->remote_cu_reset(pdev);
    WIRELESS_FMT_DEBUG("通信模块软复位%s(%d)\n", 0 == ret ? "成功" : "失败", ret);
    wireless_send_diagStatus(1, "通信模块重启", strlen("通信模块重启"), 10);
    sleep(5);
    
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    ptHandle->restartTime = curtime.tv_sec;
    return 0 != ret ? WIRELESS_DEV_RESTART : WIRELESS_DEV_COMM_TYPE;
}

/**********************************************************************
* @name      : wireless_module_changeetype
* @brief     ：模块切换拨号方式
* @param[in] ：WIRELESS_CCU_T *ptHandle    线程句柄
               WIRELESS_DATA_T *ptWireData     全局操作
* @param[out]：
* @return    ：下一个状态
* @Create    : 王津剑
* @Date      ：2022-3-27
* @Update    :
**********************************************************************/
uint8 wireless_module_changeetype(WIRELESS_CCU_T *ptHandle, WIRELESS_DATA_T *ptWireData)
{
    //int                ret = 0;
    uint32             count = 0;
    WIRELESS_HANDLE_T *ptATdata = &(ptWireData->atdata0);
    WIRELESS_HANDLE_T *ptPPPdata = &(ptWireData->pppdata0);
    
    /* 复位之前释放句柄 */
    ptATdata->workAgain = TRUE;
    ptATdata->workFlag = FALSE;
    ptATdata->commport = 0;
    sleep(1);
    ptPPPdata->workAgain = TRUE;
    ptPPPdata->workFlag = FALSE;
    ptPPPdata->commport = 0;
    while(count++ < 200)
    {
        if(NULL == ptATdata->pUart && NULL == ptPPPdata->pUart)
        {
            break;
        }
        usleep(100000);
    }
    WIRELESS_FMT_DEBUG("句柄全部释放, 开始切换方式\n");
    /* 模块复位 */
    //ret = wireless_restart_module_soft();
    //WIRELESS_FMT_DEBUG("通信模块软复位%s(%d)\n", 0 == ret ? "成功" : "失败", ret);

    //clock_gettime(CLOCK_MONOTONIC, &curtime);
    //ptHandle->restartTime = curtime.tv_sec;

    ptHandle->bUsb = FALSE;
    ptHandle->commType = g_ChangeType;
    sleep(10);
    g_needChangeType = FALSE;
    return WIRELESS_DEV_WORK_PREPARE;
}


/**********************************************************************
* @name      : wireless_ccu_init
* @brief     ：集中器管理初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-25
* @Update    :
**********************************************************************/
int wireless_ccu_init(void *pPublic, void **ppPrivate)
{
    WIRELESS_DATA_T      *ptWireData = (WIRELESS_DATA_T*)pPublic;

    while(TRUE == ptWireData->bMapExist)
    {
        sleep(1);
    }
    pthread_mutex_init(&g_fWIrelessReStart, NULL);

    ptWireData->CCUmanager.bInsert = FALSE;
    *ppPrivate = &(ptWireData->CCUmanager);
    return 0;
}

/**********************************************************************
* @name      : wireless_ccu_proc
* @brief     ：集中器管理处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2022-3-25
* @Update    :
**********************************************************************/
int wireless_ccu_proc(void *pArg, void *pPublic, void *pPrivate)
{
    WIRELESS_DATA_T    *ptWireData = (WIRELESS_DATA_T*)pPublic;
    WIRELESS_CCU_T     *ptHandle = (WIRELESS_CCU_T*)pPrivate;

    /* 数量维护 */
    wireless_num_manager(ptWireData, 0);
    wireless_num_manager(ptWireData, 0);

    switch(ptHandle->step)
    {
        case WIRELESS_DEV_GET_INIT:
        {
            if(ptHandle->pDevHal)
            {
                hal_device_release((struct tag_HW_DEVICE*)ptHandle->pDevHal);
                ptHandle->pDevHal = NULL;
            }
            ptHandle->pDevHal = hal_device_get(HW_DEVICE_ID_REMOTE_CU);
            if(NULL == ptHandle->pDevHal)
            {
                /* 延时10s再次进入流程检查 */
                return 10000000;
            }
            ptHandle->step = WIRELESS_DEV_GET_MODULE;
            WIRELESS_FMT_DEBUG("打开远程通信单元控制接口\n");
            break;
        }
        case WIRELESS_DEV_GET_MODULE:
        {
            wireless_send_change(0, WIRELESS_OPT_NONE, 0);
            ptHandle->step = wireless_module_get(ptHandle);
            if(WIRELESS_DEV_GET_MODULE == ptHandle->step)
            {
                /* 延时10s再次进入流程检查 */
                return 10000000;
            }
            break;
        }
        case WIRELESS_DEV_RESTART:
        {
            ptHandle->step = wireless_module_restart(ptHandle, ptWireData);
            break;
        }
        case WIRELESS_DEV_COMM_TYPE:
        {
            wireless_send_change(0, WIRELESS_OPT_NONE, 0);
            ptHandle->step = wireless_module_commtype(ptHandle);
            break;
        }
        case WIRELESS_DEV_WORK_PREPARE:
        {
            ptHandle->step = wireless_module_setwork(ptHandle, ptWireData);
            break;
        }
        case WIRELESS_DEV_WORK_CHECK:
        {
            ptHandle->step = wireless_module_working(ptHandle, ptWireData);
            break;
        }
        case WIRELESS_DEV_SOFT_RESTART:
        {
            ptHandle->step = wireless_module_sfrestart(ptHandle, ptWireData);
            break;
        }
        case WIRELESS_DEV_CHANGE_TYPE:
        {
            ptHandle->step = wireless_module_changeetype(ptHandle, ptWireData);
            break;
        }
        default:
        {
            ptHandle->step  = WIRELESS_DEV_GET_MODULE;
            break;
        }
    }

    return 10000;
}

#endif

