/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 消息处理接口
* @date： 2019-11-9
*********************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_arp.h>

#include "smiOSConfig.h"
#include "appManager.h"
#include "constant.h"
#include "mqttService.h"
#include "msgHandler.h"
#include "msgHelper.h"
#include "framecomm.h"
#include "systemCtrl.h"
#include "tcpService.h"
#include "udpService.h"
#include "xdrCustom.h"
#include "msgService.h"
#include "hal.h"
#include "rtc.h"
#include "appManagerCommon.h"
#include "message.h"
#include "gui_c_prtl.h"
#include <sys/vfs.h>
#include "systemInfo.h"
#include "dockerManagerCommon.h"

extern ALL_CONTAINER_AND_APP_T allcontainerinfo;
extern int get_app_id(uint8* appid,int maxlen);
extern void sys_dev_run_info_get(DEV_RUN_INTO_T *info);

BLUE_TOOTH_PARAM_T gBlueToothParam;
extern uint8 gBlueQueryFlag;

extern uint8 gConatinerQueryFlag;
extern uint8 gAppQueryFlag;
extern ALL_CONTAINER_AND_APP_T containerinfo;

//app订阅hash表
static qhashtbl_t* appSubTbl = NULL;

char sntpsource[100]={0};
uint32 sntpperiod=0;
uint32 lastsntptime=0;
//去除首尾的空格
char * strim(char *str)
{
    char *strend,*sp,*ep;
    int len;
    sp = str;
    strend = str + strlen(str) - 1;
    ep = strend;
 
    while(sp<=strend && isspace(*sp))// *sp == ' '也可以
        sp++;
    while(ep>=sp && isspace(*ep))
        ep--;
    len = (ep < sp) ? 0:(ep-sp)+1;//(ep < sp)判断是否整行都是空格
    sp[len] = '\0';
    return sp;

}

static void update_dynamic_subscription_table(uint32 subscribeInfo)
{
    sint8 i = 0;
    sint32 IOP = 0;
    sint32 IID = 0;
    sint8 key[10] = {};

    IOP = subscribeInfo & 0xffff0000;
    IID = subscribeInfo & 0x0000ffff;

    for (i = 0; i < 16; i++)
    {
        if(IID & (1 << i))
        {
            sprintf(key,"%08x",IOP + i);
            appSubTbl->putint(appSubTbl,key,1);
            MQTTMSG_FMT_DEBUG("=======add sub key %s\n",key);
        }
    }


}

static sint32 is_general_sub_event(uint32 tag)
{
    sint8 key[10] = {0};
    sprintf(key,"%08x",tag);
    if( appSubTbl->getint(appSubTbl,key) > 0)
    {
        return 1;
    }
    return 0;
}

sint32
general_event_handler(uint32 eventID ,uint8 *req_xdr, sint32 req_xdr_len)
{
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);

    //sint32 sendCount = 0;
    //sint8 key[10] = {0};


    app_manager_event_handle(eventID, req_xdr, req_xdr_len);
    
    SMIOS_FMT_DEBUG("产生事件:[%08x] \n",eventID);
//    if(sendCount <= 0)
//    {
//        sprintf(key,"%08x",eventID);
//        appSubTbl->remove(appSubTbl,"key");
//        MQTTMSG_FMT_DEBUG("=====remove key :%s\n",key);
//
//    }
    return 0;
}

/**
*********************************************************************
* @brief： 心跳检查响应
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_heartbeat_check(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{
    uint32 threadStatus = 0;

    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);


    HEART_FMT_DEBUG("recv heartbeat: [%s]\n", client->clientName);
    threadStatus = pop_u32(&req_xdr);
    //心跳处理
    app_manager_heart_beat_process(client->clientName, threadStatus);

    return 0;
}

/**
*********************************************************************
* @brief：     数据中心数据更新事件
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
//static sint32
//data_update_event(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
//{
//    UNUSED(req_xdr);
//    UNUSED(req_xdr_len);
//    UNUSED(rsp_xdr);
//    UNUSED(rsp_xdr_max_len);
//    UNUSED(client);
//    uint8 *tmp_xdr = req_xdr;
//    uint8 did      = pop_u8(&tmp_xdr);
//    
//    uint16 infoNum = pop_u16(&tmp_xdr);
//    uint32 dataTag = pop_u32(&tmp_xdr);
//
//    MQTTMSG_FMT_TRACE("recv data_update_event [%u]:[%u]:[%u]\n", did, infoNum, dataTag);
//    //app事件处理
//    app_manager_event_handle(DB_UPDATE, req_xdr, req_xdr_len);
//
//    return 0;
//}

/**
*********************************************************************
* @brief：     数据中心初始化事件
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
//static sint32
//db_init_event(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
//{
//    UNUSED(req_xdr);
//    UNUSED(req_xdr_len);
//    UNUSED(rsp_xdr);
//    UNUSED(rsp_xdr_max_len);
//    UNUSED(client);
//
//    uint8 *tmp_xdr          = req_xdr;
//    uint8 dataClassIDOption = 0;
//    uint8 dataClassID       = 0;
//    uint8 did               = 0;
//    uint8 type              = 0;
//
//    type = pop_u8(&tmp_xdr);
//    if (type == 0)
//    {
//        did               = pop_u8(&tmp_xdr);
//        dataClassIDOption = pop_u8(&tmp_xdr);
//        if (dataClassIDOption)
//        {
//            dataClassID = pop_u8(&tmp_xdr);
//        }
//    }
//    else if (type == 1)
//    {
//        did = pop_u8(&tmp_xdr);
//    }
//    else
//    {
//        return 0;
//    }
//
//    MQTTMSG_FMT_DEBUG("recv db_init_event type:[%u] did:[%u] dataClassIDOption:[%u] dataClassID:[%u]", type, did,
//                dataClassIDOption, dataClassID);
//    app_manager_event_handle(DB_INIT, req_xdr, req_xdr_len);
//    return 0;
//}

/*
sint32 msg_execute_commands(uint8 *       req_xdr,
                            sint32        req_xdr_len,
                            uint8 *       rsp_xdr,
                            sint32        rsp_xdr_max_len,
                            MSG_CLIENT_T *client);

sint32 msg_transmission(uint8 *       req_xdr,
                        sint32        req_xdr_len,
                        uint8 *       rsp_xdr,
                        sint32        rsp_xdr_max_len,
                        MSG_CLIENT_T *client);

sint32 msg_event_fast_power_cut(uint8 *       req_xdr,
                                sint32        req_xdr_len,
                                uint8 *       rsp_xdr,
                                sint32        rsp_xdr_max_len,
                                MSG_CLIENT_T *client);

sint32 msg_event_system_power_off(uint8 *       req_xdr,
                                  sint32        req_xdr_len,
                                  uint8 *       rsp_xdr,
                                  sint32        rsp_xdr_max_len,
                                  MSG_CLIENT_T *client);

sint32 msg_event_system_restart(uint8 *       req_xdr,
                                sint32        req_xdr_len,
                                uint8 *       rsp_xdr,
                                sint32        rsp_xdr_max_len,
                                MSG_CLIENT_T *client);

sint32 msg_event_system_power_on(uint8 *       req_xdr,
                                 sint32        req_xdr_len,
                                 uint8 *       rsp_xdr,
                                 sint32        rsp_xdr_max_len,
                                 MSG_CLIENT_T *client);
*/

/**
*********************************************************************
* @brief：     APP注册请求
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_app_login_request(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{
    uint8 *componentName    = NULL;
    uint8   comName[100]    = {0};
    uint32 componentNameLen = 0;
    uint32 subInfoLen       = 0;
    uint32 *pSubInfo        = NULL;
    uint32 i                = 0;
    // uint32             subInfo[APP_MAX_SUB_NUM] = {0};
    APP_SUB_INFO_RAW_T sub = {0};
   
    char version[200]         = {0};

    sint32 pid             = 0;
    sint32 ret             = -1;
    DATA_TIME_T *dataTime  = NULL;
    uint8 *tmp_xdr         = rsp_xdr;
    uint8 *head_xdr         = req_xdr;
    MSG_DATE_T msgData     = {0};
    uint8 *app    = NULL;
    uint8 appid[100]={0};
    uint32 appidlen=0;

    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr_max_len);
    pid              = pop_u32(&req_xdr);
    componentNameLen = pop_nbyte(&req_xdr, &componentName);
    strncpy((char*)comName, (char*)componentName, componentNameLen);
    subInfoLen = pop_nint(&req_xdr, (uint8 **)(&pSubInfo));
    if (subInfoLen > 0)
    {
        subInfoLen           = subInfoLen > APP_MAX_SUB_NUM ? APP_MAX_SUB_NUM : subInfoLen;
        sub.subscribeInfoNum = subInfoLen;
        for (i = 0; i < subInfoLen; i++)
        {
            LOGIN_FMT_TRACE("pSubInfo[%d] = %08x\n", i, pSubInfo[i]);
            sub.subscribeInfo[i] = ntohl(pSubInfo[i]);  //字节序转换
            update_dynamic_subscription_table(sub.subscribeInfo[i]);
        }
    }

//#ifdef PRODUCT_ECU
//    //能源控制器的版本信息还是double-long-unsigned 
////    版本信息格式定义：
////    byte3   byte2   byte1   byte0
////    主版本号    子版本号    修正版本号 编译版本号
////    每个版本号段取值从0～255。
////    例如，版本号1.0.1.123：
////    主版本号[1].子版本号[0].修正版本号[1].编译版本号[123]
//    
//    //把版本号转成字符串统一存储 取的时候再转
//    uint8 tmpver[4] = {0};
//    tmpver[0] = pop_u8(&req_xdr);
//    tmpver[1] = pop_u8(&req_xdr);
//    tmpver[2] = pop_u8(&req_xdr);
//    tmpver[3] = pop_u8(&req_xdr);
//    sprintf(version,"%d.%d.%d.%d",tmpver[0],tmpver[1],tmpver[2],tmpver[3]);
//#else
    //别的app还没修改版本为字符串 检查版本开始的第五个字节是不是07
    uint32 versionnum;
    uint8* versioninfo = NULL;
    uint32 versionLen = 0;

    if(req_xdr[4] == 0x07)
    {
        LOGIN_FMT_DEBUG("注册消息还未修改 版本按uint32来\n");
        versionnum = pop_u32(&req_xdr);
        sprintf(version,"%d",versionnum);
    }else
    {
        versionLen = pop_nbyte(&req_xdr, &versioninfo);
        strncpy(version,(char*)versioninfo,versionLen);

    }

//#endif
    


    //version = pop_u32(&req_xdr);
    pop_fixbyte(&req_xdr, (uint8 **)&dataTime, XDR_DATA_TIME_LEN);
    dataTime->year = ntohs(dataTime->year);
    //新增appid
    appidlen = req_xdr_len - (req_xdr - head_xdr);
    if ((appidlen > 0) && (appidlen <= (APPID_LEN+1)))
    {
        appidlen = pop_nbyte(&req_xdr, &app);
        memcpy((char*)appid, (char*)app, appidlen);
    }
    else
    {
        appidlen = APPID_LEN;
        memset(appid, 0x00, APPID_LEN);
    }
    
    LOGIN_FMT_DEBUG("进程号:[%d]\n", pid);
    LOGIN_FMT_DEBUG("组件名称:[%s]\n", comName);
    for (i = 0; i < subInfoLen; i++)
    {
        LOGIN_FMT_DEBUG("订阅事件[%u]:[%08x]\n", i, sub.subscribeInfo[i]);
    }
    LOGIN_FMT_DEBUG("版本信息:[%s]\n", version);
    LOGIN_FMT_DEBUG("发布日期:[%4d-%02d-%02d %02d:%02d:%02d]\n", (uint32)dataTime->year,
                (uint32)dataTime->month, (uint32)dataTime->day, (uint32)dataTime->hour, (uint32)dataTime->minute,
                (uint32)dataTime->second);
    LOGIN_BUF_DEBUG(appid,appidlen, "APP-ID:");

    memset(&msgData, 0, sizeof(MSG_DATE_T));
    msgData.pid           = pid;
    msgData.subscribeInfo = sub;
    strcpy(msgData.version,version);
    memcpy(&(msgData.msgClient), client, sizeof(MSG_CLIENT_T));
    msgData.appidlen = appidlen;
    memcpy(msgData.appid,appid,appidlen);
    //app注册处理
    ret = app_manager_login_process(&msgData, dataTime);
    LOGIN_FMT_DEBUG("app_manager_login_process ret = %d\n", ret);

    if (0 == ret)
    {
        alive_app_name_save((char*)comName);
        LOGIN_FMT_DEBUG("[%s]注册成功\n",comName);
        push_u8(&rsp_xdr, XDR_TRUE);
    }
    else
    {
        push_u8(&rsp_xdr, XDR_FALSE);
    }

    return rsp_xdr - tmp_xdr;
}

/**
*********************************************************************
* @brief：     APP注销请求
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_app_logout_request(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);
    uint8 *tmp_xdr = rsp_xdr;
    LOGIN_FMT_DEBUG("[%s]取消注册\n", client->clientName);

    if (0 == app_manager_logout_process(client->clientName))
    {
        push_u8(&rsp_xdr, XDR_TRUE);
    }
    else
    {
        push_u8(&rsp_xdr, XDR_FALSE);
    }

    return rsp_xdr - tmp_xdr;
}

/**
*********************************************************************
* @brief：     APP查询已注册APP信息
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_query_login_apps(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{
    MQT_PLUGIN_T *mqtPluginArray = NULL;
    uint32 mqtPluginNum          = 0;
    uint32 i                     = 0;
    uint8 *tmp_xdr               = rsp_xdr;

    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    // 查询app注册信息
    SMIOS_FMT_TRACE("查询已注册APP信息\n");
    app_manager_all_app_query(&mqtPluginArray, &mqtPluginNum);
    SMIOS_FMT_TRACE("已注册APP数量%d\n", mqtPluginNum);

    MQT_PLUGIN_T *pMqtPlutin = mqtPluginArray;
    if (NULL == pMqtPlutin)
    {
        SMIOS_FMT_DEBUG("[msgHandler] plugin array is null\n");
    }

    push_u8(&rsp_xdr, mqtPluginNum);
    for (i = 0; i < mqtPluginNum; i++)
    {
        /* code */
        push_mqt_plugin(&rsp_xdr, mqtPluginArray + i);
    }

    if (NULL != mqtPluginArray)
    {
        free(mqtPluginArray);
        mqtPluginArray = NULL;
    }

    return rsp_xdr - tmp_xdr;
}

/**
*********************************************************************
* @brief：     APP查询指定名称已注册APP信息
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_query_app_by_name(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{
    MQT_PLUGIN_T mqtPlugin;
    uint8 *queryAppName    = NULL;
    uint8 *tmp_xdr         = rsp_xdr;
    pop_nbyte(&req_xdr, &queryAppName);
    memset(&mqtPlugin, 0, sizeof(MQT_PLUGIN_T));

    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    // app
    SMIOS_FMT_TRACE("query app reginster info\n");
    sint32 ret = app_manager_one_app_query(&mqtPlugin, (char *)queryAppName);
    if (-1 == ret)
    {
        SMIOS_FMT_DEBUG("Not found the app[%s]\n", queryAppName);
    }

    push_mqt_plugin(&rsp_xdr, &mqtPlugin);

    return rsp_xdr - tmp_xdr;
}

/**
*********************************************************************
* @brief：     APP事件订阅
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_event_subscribe(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{
    APP_SUB_INFO_RAW_T sub = {0};
    uint32 i               = 0;
    uint32 subInfoLen      = 0;
    uint32 *pSubInfo       = NULL;
    uint8 *tmp_xdr         = rsp_xdr;

    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    subInfoLen = pop_nint(&req_xdr, (uint8 **)(&pSubInfo));

    if (subInfoLen > 0)
    {
        subInfoLen           = subInfoLen > APP_MAX_SUB_NUM ? APP_MAX_SUB_NUM : subInfoLen;
        sub.subscribeInfoNum = subInfoLen;
        for (i = 0; i < subInfoLen; i++)
        {
            sub.subscribeInfo[i] = ntohl(pSubInfo[i]);  //字节序转换
            //更新订阅列表
            update_dynamic_subscription_table(sub.subscribeInfo[i]);

        }
    }

    //app事件订阅
    if (0 == app_manager_sub_event(client->clientName, &sub, 1))
    {
        push_u8(&rsp_xdr, XDR_TRUE);
    }
    else
    {
        push_u8(&rsp_xdr, XDR_FALSE);
    }
    return rsp_xdr - tmp_xdr;
}

/**
*********************************************************************
* @brief：     APP事件取消订阅
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_event_unsubscribe(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{
    APP_SUB_INFO_RAW_T sub = {0};
    uint32 i               = 0;
    uint32 subInfoLen      = 0;
    uint32 *pSubInfo       = NULL;
    uint8 *tmp_xdr         = rsp_xdr;

    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    subInfoLen = pop_nint(&req_xdr, (uint8 **)(&pSubInfo));

    if (subInfoLen > 0)
    {
        subInfoLen           = subInfoLen > APP_MAX_SUB_NUM ? APP_MAX_SUB_NUM : subInfoLen;
        sub.subscribeInfoNum = subInfoLen;
        for (i = 0; i < subInfoLen; i++)
        {
            sub.subscribeInfo[i] = ntohl(pSubInfo[i]);  //字节序转换
        }
    }

    //取消订阅
    if (0 == app_manager_sub_event(client->clientName, &sub, 0))
    {
        push_u8(&rsp_xdr, XDR_TRUE);
    }
    else
    {
        push_u8(&rsp_xdr, XDR_FALSE);
    }

    return rsp_xdr - tmp_xdr;
}

/**
*********************************************************************
* @brief：     获取APP订阅信息
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_get_subscribe_info(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{
    uint8 *queryAppName    = NULL;
    APP_SUB_INFO_RAW_T sub = {0};
    uint8 *tmp_xdr         = rsp_xdr;

    pop_nbyte(&req_xdr, &queryAppName);
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    //获取订阅信息
    if (0 == app_manager_get_sub_info((sint8 *)queryAppName, &sub))
    {
        push_sub_info(&rsp_xdr, &sub);
    }
    else
    {
        push_u8(&rsp_xdr, 0);
    }

    return rsp_xdr - tmp_xdr;
}

/**
*********************************************************************
* @brief：     请求系统重启
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32 msg_request_system_restart(uint8 *req_xdr,
                                         sint32 req_xdr_len,
                                         uint8 *rsp_xdr,
                                         sint32 rsp_xdr_max_len,
                                         MSG_CLIENT_T *client)
{
    uint8 *tmp_xdr = rsp_xdr;
    uint32  rtcTag                    = 0x00000003;     //系统复位事件
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);
    tmp_xdr[0] = 1;     //返回ture
    tmp_xdr++;
    
    //系统重启操作放到回码后处理
    //system_reboot();
    general_event_handler(rtcTag,NULL, 0);
    rebootflag = 1;
    push_u8(&rsp_xdr, XDR_TRUE);

    return 1;
}

/**
*********************************************************************
* @brief：     获取电源状态
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_get_power_status(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{
    sint32 powerStatus = 1;
    uint8 *tmp_xdr     = rsp_xdr;
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    system_get_power_state(&powerStatus);
    push_u32(&rsp_xdr, powerStatus);

    return rsp_xdr - tmp_xdr;
}

/**
*********************************************************************
* @brief：     获取设备以太网状态
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32 msg_get_ethernet_status(uint8 *req_xdr,
                                      sint32 req_xdr_len,
                                      uint8 *rsp_xdr,
                                      sint32 rsp_xdr_max_len,
                                      MSG_CLIENT_T *client)
{
    sint8 dvName[MAX_DEVICE_NAME_LEN] = {0};
    uint8 *deviceName                 = NULL;
    sint32 ethernetStatus             = -1;
    uint32 n                          = 0;
    uint8 *tmp_xdr                    = rsp_xdr;
    //uint32 plugTag                      = 0x00010001;
    //uint8  payload[100]                 = {0};
    //uint8  offset                   = 0; 
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    n = pop_nbyte(&req_xdr, &deviceName);
    strncpy(dvName, (sint8 *)deviceName, n > sizeof(dvName) ? sizeof(dvName) : n);
    
    system_get_ethernet_state(dvName, &ethernetStatus);
    MQTTMSG_FMT_TRACE("system_get_ethernet_state deviceName:%s,ethernetStatus:%d\n", dvName,ethernetStatus);
    push_u32(&rsp_xdr, ethernetStatus);

//插拔事件
//  payload[offset++] = strlen(dvName);
//  strncpy((char*)(&payload[offset]),(const char*)dvName,payload[0]);
//  offset += payload[0];
//  payload[offset++] = ethernetStatus;
//  general_event_handler(plugTag,payload, offset);

    return rsp_xdr - tmp_xdr;
}

/**
*********************************************************************
* @brief：     配置本机ip地址
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32 msg_set_local_ip(uint8 *req_xdr,
                                      sint32 req_xdr_len,
                                      uint8 *rsp_xdr,
                                      sint32 rsp_xdr_max_len,
                                      MSG_CLIENT_T *client)
{
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    int     offset = 0;
    uint8   nodeLen = 0;
    bool    ackRet = 0;
    
    SET_IP_INFO_T       *pSetIpInfo = &setIpInfo;
    
    if(req_xdr == NULL || rsp_xdr == NULL)
    {
        return -1;
    }
    //pthread_mutex_lock(&pSetIpInfo->ipLock);
    memset(pSetIpInfo,0x0,sizeof(SET_IP_INFO_T));
    
    nodeLen = req_xdr[offset++];
    if(nodeLen >= sizeof(pSetIpInfo->nodeName))
    {
        SMIOS_FMT_DEBUG("%s:nodeName len [%d] too long !\n",__FUNCTION__,nodeLen);
        return 1;
    }
    strncpy(pSetIpInfo->nodeName,(const char*)(&req_xdr[offset]),nodeLen);
    offset += nodeLen;

    pSetIpInfo->type = req_xdr[offset++];
    if(1 == pSetIpInfo->type)
    {
        if(req_xdr[offset] != 4)
        {
            SMIOS_FMT_DEBUG("%s:ip len [%d] eroor !\n",__FUNCTION__,req_xdr[offset]);
            return 1;
        }
        offset++;
        memcpy(pSetIpInfo->ipVal,&req_xdr[offset],sizeof(pSetIpInfo->ipVal));
        offset += sizeof(pSetIpInfo->ipVal);

        if(req_xdr[offset] != 4)
        {
            SMIOS_FMT_DEBUG("%s:mask len [%d] eroor !\n",__FUNCTION__,req_xdr[offset]);
            return 1;
        }
        offset++;
        memcpy(pSetIpInfo->netMask,&req_xdr[offset],sizeof(pSetIpInfo->netMask));
        offset += sizeof(pSetIpInfo->netMask);

        if(req_xdr[offset] != 4)
        {
            SMIOS_FMT_DEBUG("%s:gateWay len [%d] error !\n",__FUNCTION__,req_xdr[offset]);
            return 1;
        }
        offset++;
        memcpy(pSetIpInfo->gateWay,&req_xdr[offset],sizeof(pSetIpInfo->gateWay));
        offset += sizeof(pSetIpInfo->gateWay);
    }
    
    if(offset != req_xdr_len)
    {
        SMIOS_FMT_DEBUG("%s:send in len [%d] differ from parsing len [%d] !\n",__FUNCTION__,req_xdr_len,offset);
        return 1;
    }
    setFlag = 1;
    
    ackRet = 1;
    *rsp_xdr = ackRet;
    return 1;
}

/**
*********************************************************************
* @brief：     添加路由表
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32 msg_set_add_route(uint8 *req_xdr,
                                      sint32 req_xdr_len,
                                      uint8 *rsp_xdr,
                                      sint32 rsp_xdr_max_len,
                                      MSG_CLIENT_T *client)
{
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    int     offset = 0;
    uint8   nodeLen = 0;
    char    nodeName[100] = {0};    //设备节点名
    int     ipNum = 0;
    uint8   ipVal[4] = {0};         //ip
    uint8   netMask[4] = {0};       //掩码
    uint8   gateway[4]={0};         //网关地址
    char    addCmd[256] = {0};
    bool    ackRet = 0;
    int     i;
    
    if(req_xdr == NULL || rsp_xdr == NULL)
    {
        return -1;
    }

    nodeLen = req_xdr[offset++];
    strncpy(nodeName,(const char*)(&req_xdr[offset]),nodeLen);
    offset += nodeLen;
    ipNum = req_xdr[offset++];
    if(ipNum == 0)
    {
        SMIOS_FMT_DEBUG("%s:ip Num [%d] error!\n",__FUNCTION__,ipNum);
        return 1;
    }
    for(i= 0; i< ipNum; i++)
    {
        memset(addCmd,0x00,sizeof(addCmd));
        if(req_xdr[offset] != 4)
        {
            SMIOS_FMT_DEBUG("%s:ip len [%d] error!\n",__FUNCTION__,req_xdr[offset]);
            return 1;
        }
        offset++;
        memcpy(ipVal,&req_xdr[offset],4);
        offset += 4;
        
        if(req_xdr[offset] != 4)
        {
            memset(netMask, 0xFF, 4);
        }
        else
        {
            offset++;
            memcpy(netMask,&req_xdr[offset],4);
            offset += 4;
        }
        
        if(req_xdr[offset] != 4)
        {
            memset(gateway, 0, 4);
        }
        else
        {
            offset++;
            memcpy(gateway,&req_xdr[offset],4);
            offset += 4;
        }
        if(0xFF == netMask[0] && 0xFF == netMask[1] && 0xFF == netMask[2] && 0xFF == netMask[3])
        {
            sprintf(addCmd,"route add -host %d.%d.%d.%d dev %s",ipVal[0],ipVal[1],ipVal[2],ipVal[3],nodeName);
        }
        else
        {
            sprintf(addCmd,"route add -net %d.%d.%d.%d netmask %d.%d.%d.%d gw %d.%d.%d.%d dev %s",ipVal[0],ipVal[1],ipVal[2],ipVal[3],
            netMask[0],netMask[1],netMask[2],netMask[3],gateway[0],gateway[1],gateway[2],gateway[3],nodeName);
        }
        SMIOS_FMT_DEBUG("%s\n", addCmd);
        if(system(addCmd) < 0)
        {
            SMIOS_FMT_DEBUG("%s:%s error !\n",__FUNCTION__,addCmd);
            return 1;
        }
    }
    
    if(offset != req_xdr_len)
    {
        SMIOS_FMT_DEBUG("%s:Sendin len [%d] is different from parsing len [%d] !\n",__FUNCTION__,req_xdr_len,offset);
        return 1;
    }

    ackRet = 1;
    *rsp_xdr = ackRet;
    return 1;
    
}

/**
*********************************************************************
* @brief：     删除路由表
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32 msg_set_del_route(uint8 *req_xdr,
                                      sint32 req_xdr_len,
                                      uint8 *rsp_xdr,
                                      sint32 rsp_xdr_max_len,
                                      MSG_CLIENT_T *client)
{
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    int     offset = 0;
    uint8   nodeLen = 0;
    char    nodeName[100] = {0};    //设备节点名
    int     ipNum = 0;
    uint8   ipVal[4] = {0};         //ip
    uint8   netMask[4] = {0};
    uint8   gateway[4]={0};         //网关地址
    char    addCmd[256] = {0};
    bool    ackRet = 0;
    int     i;
    
    if(req_xdr == NULL || rsp_xdr == NULL)
    {
        return -1;
    }

    nodeLen = req_xdr[offset++];
    strncpy(nodeName,(const char*)(&req_xdr[offset]),nodeLen);
    offset += nodeLen;
    ipNum = req_xdr[offset++];
    if(ipNum == 0)
    {
        SMIOS_FMT_DEBUG("%s:ip Num [%d] error!\n",__FUNCTION__,ipNum);
        return 1;
    }
    for(i= 0; i< ipNum; i++)
    {
        memset(addCmd,0x00,sizeof(addCmd));
        if(req_xdr[offset] != 4)
        {
            SMIOS_FMT_DEBUG("%s:ip len [%d] error!\n",__FUNCTION__,req_xdr[offset]);
            return 1;
        }
        offset++;
        memcpy(ipVal,&req_xdr[offset],4);
        offset += 4;
        
        if(req_xdr[offset] != 4)
        {
            memset(netMask, 0xFF, 4);
        }
        else
        {
            offset++;
            memcpy(netMask,&req_xdr[offset],4);
            offset += 4;
        }
        
        if(req_xdr[offset] != 4)
        {
            memset(gateway, 0, 4);
        }
        else
        {
            offset++;
            memcpy(gateway,&req_xdr[offset],4);
            offset += 4;
        }

        if(0xFF == netMask[0] && 0xFF == netMask[1] && 0xFF == netMask[2] && 0xFF == netMask[3])
        {
            sprintf(addCmd,"route del -host %d.%d.%d.%d dev %s",ipVal[0],ipVal[1],ipVal[2],ipVal[3],nodeName);
        }
        else
        {
            sprintf(addCmd,"route del -net %d.%d.%d.%d netmask %d.%d.%d.%d gw %d.%d.%d.%d dev %s",ipVal[0],ipVal[1],ipVal[2],ipVal[3],
            netMask[0],netMask[1],netMask[2],netMask[3],gateway[0],gateway[1],gateway[2],gateway[3],nodeName);
        }
        SMIOS_FMT_DEBUG("%s\n", addCmd);
        if(system(addCmd) < 0)
        {
            SMIOS_FMT_DEBUG("%s:%s error !\n",__FUNCTION__,addCmd);
            return 1;
        }
    }
    
    if(offset != req_xdr_len)
    {
        SMIOS_FMT_DEBUG("%s:Sendin len [%d] is different from parsing len [%d] !\n",__FUNCTION__,req_xdr_len,offset);
        return 1;
    }

    ackRet = 1;
    *rsp_xdr = ackRet;
    return 1;


}

int rtcTimeSet(struct tm rtc1)
{
    int iRet = 0;
    uint32 rtcTag = 0x00000005;
    
#if (DISABLE_HAL == 0)
        RTC_DEVICE_T* dev = (RTC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_RTC);
        if(NULL == dev)
        {
            MQTTMSG_FMT_DEBUG("dev is null\n");
            printf("dev is null");
            return -1;
        }
        
        iRet = dev->rtc_time_set(dev, &rtc1);
        if(iRet >= 0)
        {
            MQTTMSG_FMT_DEBUG("rtc set time: %02x-%02x-%02x %02x:%02x:%02x\n", rtc1.tm_year, rtc1.tm_mon, rtc1.tm_mday, rtc1.tm_hour, rtc1.tm_min, rtc1.tm_sec);
            printf("rtc set time: %02x-%02x-%02x %02x:%02x:%02x", rtc1.tm_year, rtc1.tm_mon, rtc1.tm_mday, rtc1.tm_hour, rtc1.tm_min, rtc1.tm_sec);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("rtc set time is failed!\n");
            printf("rtc set time is failed!");
        }
          
        hal_device_release((HW_DEVICE *)dev);
        
        MQTTMSG_FMT_DEBUG("rtc time sync success, now make the time change event,rtcTag = %08x !\n",rtcTag);
        printf("rtc time sync success, now make the time change event,rtcTag = %08x !",rtcTag);
        general_event_handler(rtcTag,NULL, 0);
    
#endif
    return iRet;
}

//时钟同步
static sint32 msg_set_rtc_sync(uint8 *req_xdr,
                                      sint32 req_xdr_len,
                                      uint8 *rsp_xdr,
                                      sint32 rsp_xdr_max_len,
                                      MSG_CLIENT_T *client)
{
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    int iRet = 0;
    struct tm rtc1;

    uint32 rtcTag                     = 0x00000005;


    if(req_xdr == NULL || rsp_xdr == NULL)
    {
        return -1;
    }

//add here for subsitute for hal
    uint8   dateSet[8] = {0};

    if(req_xdr_len != 20)
    {
        SMIOS_FMT_DEBUG("输入dateSet的长度 %d 不正确\n\n",req_xdr_len);
    }
    
    memcpy(dateSet,req_xdr,sizeof(dateSet));

    memset(&rtc1,0x0,sizeof(struct tm));

    rtc1.tm_year = ( dateSet[0]*256 + dateSet[1] ) - 1900;
    rtc1.tm_mon =  dateSet[2] - 1;
    rtc1.tm_mday = dateSet[3] ;
    rtc1.tm_hour = dateSet[5] ;
    rtc1.tm_min =  dateSet[6] ;
    rtc1.tm_sec =  dateSet[7] ;

#if (DISABLE_HAL == 0)
    RTC_DEVICE_T* dev = (RTC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_RTC);
    if(NULL == dev)
    {
        SMIOS_FMT_DEBUG("dev is null\n");
        return -1;
    }

    iRet = dev->rtc_time_set(dev, &rtc1);
    if(iRet >= 0)
    {
        SMIOS_FMT_DEBUG("rtc set time: %d-%d-%d %d:%d:%d\n", rtc1.tm_year+1900, rtc1.tm_mon+1, rtc1.tm_mday, rtc1.tm_hour, rtc1.tm_min, rtc1.tm_sec);
        rsp_xdr[0] = 01;
    }
    else
    {
        rsp_xdr[0] = 01;
        SMIOS_FMT_DEBUG("rtc set time is failed!\n");
    }
      
    hal_device_release((HW_DEVICE *)dev);

    SMIOS_FMT_TRACE("rtc time sync success, now make the time change event,rtcTag = %08x !\n",rtcTag);
    general_event_handler(rtcTag,NULL, 0);

#endif
    return 1;

}


/**
*********************************************************************
* @brief： ERROR MSG API
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    len             数据长度
*********************************************************************
*/
static sint32 error_msg_handler(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, uint32 tag)
{
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr_max_len);
    
    sint32 len = 0;
    uint8 *tmp = rsp_xdr;

    push_u32(&tmp, tag);
    push_u16(&tmp, -1);  //错误码-1

    len = tmp - rsp_xdr;

    return len;
}

char* shellcmd(const char* cmd, char* buff, int size)
{
    char temp[256];
    FILE* fp = NULL;
    int offset = 0;
    int len;

    fp = popen(cmd, "r");
    if(fp == NULL)
    {
        return NULL;
    }

    while(fgets(temp, sizeof(temp), fp) != NULL)
    {
        len = strlen(temp);
        if(offset + len < size)
        {
            strcpy(buff+offset, temp);
            offset += len;
        }
        else
        {
            buff[offset] = 0;
            break;
        }
    }

    if(fp != NULL)
    {
        pclose(fp);
    }

    return buff;
}
//获取内存总量单位MB
uint32 get_memtotal(void )
{
    char str[100]={0};
    char mem[20]={0};
    shellcmd("cat /proc/meminfo | grep MemTotal",str,100);
    char* p;
    char* s ;
    p = strstr(str,":");
    s = strstr(str,"kB");
    int index1 = p-str;
    memcpy(mem,&str[index1+1],s-p-1);
    p = strim(mem);

    return atoi(p)/1024;
}
//获取处理器主频
uint32 get_cpufreq(void)
{
    char str[100]={0};
    shellcmd("cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq",str,100);
    return atoi(str)/1000;
}
//获取存储容量
uint32 get_storage(void)
{
    uint32 storage;
#ifdef BOARD_3358
    struct statfs diskInfo;  
    statfs("/",&diskInfo);
    storage = diskInfo.f_bsize/1024*diskInfo.f_blocks/1024;
    statfs("/data",&diskInfo);
    storage+=(diskInfo.f_bsize/1024*diskInfo.f_blocks/1024);
#endif
#ifdef BOARD_801
    char str[100]={0};
    shellcmd("fdisk -l | grep dev/mmcblk0: | awk '{print $5}'", str, 100);
    storage = atoll(str)/1024;
    storage = storage/1024;
#endif
    return storage;
}

//获取CPU核数
uint32 get_cpucores(void )
{
    char str[100]={0};
   
    shellcmd("cat /proc/cpuinfo |grep processor |wc -l",str,100);


    return atoi(str);
}

uint8 CharToNum(uint8 str)
{
    uint8 ret = 0;
    if(str>='0' && str<='9')
    {
        ret = str - '0';
    }
    if(str>='a' && str<='z')
    {
        ret = str - 'a'+10;
    }
    if(str>='A' && str<='Z')
    {
        ret = str - 'A'+10;
    }
    return ret;
}
uint8 chartobcd(char* str)
{
    uint8 ret = 0;
    ret = (CharToNum(str[0])<<4)|(CharToNum(str[1]));
    return ret;
}
void ssplit(char *src,const char *delim,char dest[][100],int *num)
{
    char* p = src;
    char* tmp;
    int i =0;
    tmp = strsep(&p,delim);
    while(tmp!=NULL)
    {
        strcpy(dest[i],tmp);
        tmp = strsep(&p,delim);
        i++;
    }
    *num = i;
}
//获取Mac地址
void getmac(uint8* out)
{
    char cmd[1000] = {0};
    char str[100]={0};
    sprintf(cmd,"ifconfig %s | grep 'HWaddr' | awk '{ print $5}'", ETH_FIRST_NAME);
    shellcmd(cmd,str,100);
    sscanf(str,"%X:%X:%X:%X:%X:%X",(uint32*)&out[0], (uint32*)&out[1], (uint32*)&out[2],
                  (uint32*)&out[3], (uint32*)&out[4], (uint32*)&out[5]);

//    char str[100]={0};
//    shellcmd("cat /sys/class/net/FE0/address",str,100);
//    char dest[10][100];
//    memset(dest,0x00,sizeof(dest));
//    int count;
//    int i;
//    ssplit(strim(str),":", dest, &count);
//    for(i=0;i<count;i++)
//    {
//        *out++ = chartobcd(dest[i]);
//    }
}


void get_cpuframe(uint8* frame)
{
    char cmd[1000] = {0};
    char str[100]={0};
    sprintf(cmd,"uname -m");
    shellcmd(cmd,str,100);

    strcpy((char*)frame,strim(str));
}

/**
*********************************************************************
* @brief：     获取设备信息
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_get_device_info(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    uint8 *tmp_xdr     = rsp_xdr;
    DEVICE_INFO_T DeviceInfo;
    memset(&DeviceInfo,0x00,sizeof(DEVICE_INFO_T));

    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);
    //获取设备MAC
    getmac(DeviceInfo.DeviceMac);
    SMIOS_BUF_DEBUG(DeviceInfo.DeviceMac, 6, "设备mac地址为 \n");
    //获取内存总量
    DeviceInfo.DeviceMem = get_memtotal();
    //获取存储容量
    DeviceInfo.DeviceStorage = get_storage();
    //获取处理器主频
    DeviceInfo.CPUFrequency = get_cpufreq();
    DeviceInfo.CPUCores = get_cpucores();
    DeviceInfo.CPUCaches = 25;
    get_cpuframe(DeviceInfo.CPUFrame);
    SMIOS_FMT_DEBUG("cpu核数 %d 架构 %s \n",DeviceInfo.CPUCores,(char*)DeviceInfo.CPUFrame);
    push_device_info(&rsp_xdr,&DeviceInfo);
    
    return rsp_xdr - tmp_xdr;
}

int getshellresult(const char* cmd,char result[][500],int size)
{
    char temp[256];
    FILE* fp = NULL;

    int row = 0;
    fp = popen(cmd, "r");
    if(fp == NULL)
    {
        return 0;
    }

    while(fgets(temp, sizeof(temp), fp) != NULL)
    {

        strcpy(result[row], strim(temp));
        row++;
        if(row>=size)
            break;
    }

    if(fp != NULL)
    {
        pclose(fp);
    }

    return row;

}//2020-06-09T01:01:57.208792693Z 格式的时间
void timeformat(char* time,OOP_DATETIME_S_T *ooptime)
{

    OOP_DATETIME_S_T tmp;
    memset(&tmp,0x00,sizeof(OOP_DATETIME_S_T));
    int year,month,day,hour,minute,second;
    time_t tmptime;
    sscanf(time,"%d-%d-%dT%d:%d:%d.",&year,&month,&day,&hour,&minute,&second);
    tmp.year =year;
    tmp.month =month;
    tmp.day =day;
    tmp.hour =hour;
    tmp.minute =minute;
    tmp.second =second;
    DT_DateTimeShort2Time(&tmp,&tmptime);
    tmptime+=8*3600; //转为东八区
    DT_Time2DateTimeShort(tmptime,ooptime);
    //printf("time %d %d %d %d %d %d \n",ooptime->year,ooptime->month,ooptime->day,ooptime->hour,ooptime->minute,ooptime->second);
}



//百分比的字符串转为数字
//偶现查询出来的百分比是个巨大数 要特殊处理 否则程序会挂
uint16 percentdatachange(char* buf)
{
    char* t;
    char data[100]={0};
    t = strstr(buf,"%");
    if(NULL == t)
        return 0;
    if(t-buf > 10)
        return 0;
    memcpy(data,buf,t-buf);
    return atof(data)*100;
}


//IP地址转换
void IPchange(char* buf,uint8* out)
{
    char* p = buf;
    char* t;
    t = strsep(&p,".");
    while(t!=NULL)
    {
        *out++ = atoi(t);
        t = strsep(&p,".");
    }
}
void get_container_info(ALL_CONTAINER_INFO_T *allcontainer)
{
    int count;
    uint8 i,j;
    time_t timenow = time(NULL);
    char buff[20][500];
    memset(buff,0,sizeof(buff));
    count = getshellresult("docker ps --format '{{.Names}}#{{.Mounts}}#{{.Ports}}#{{.Image}}'",buff,20);
    if(count==0)
        return;
    allcontainer->containercount = count;
    for(i=0;i<count;i++)
    {
        char str[50][100];
        int num;
        memset(str,0x00,sizeof(str));
        ssplit(buff[i], "#",str, &num);
        if(num == 0)
            continue;
        allcontainer->container[i].ContainerID = i;
        strcpy((char*)allcontainer->container[i].ContainerName,str[0]);
        strcpy((char*)allcontainer->container[i].FilePath,str[1]);
        strcpy((char*)allcontainer->container[i].Interface,str[2]);
        strcpy((char*)allcontainer->container[i].ImageName,str[3]);
        allcontainer->container[i].CpuCores = 4;
        char cmd[256]={0};
        char result[1][500];
        memset(result,0x00,sizeof(result));
        sprintf(cmd,"docker inspect --format='{{.State.Status}}#{{.HostConfig.Memory}}#{{.NetworkSettings.IPAddress}}#{{.Created}}#{{.State.StartedAt}}' %s",allcontainer->container[i].ContainerName );
        if(getshellresult(cmd,result,1)!=1)
            continue;
        memset(str,0x00,sizeof(str));
        ssplit(result[0], "#",str, &num);

        strcpy((char*)allcontainer->container[i].ContainerState,str[0]);
        strcpy((char*)allcontainer->container[i].MemLimit,str[1]);
        IPchange(str[2], allcontainer->container[i].ContainerIP);
        //printf("IP %d.%d.%d.%d \n",allcontainer->container[i].ContainerIP[0],allcontainer->container[i].ContainerIP[1],
               // allcontainer->container[i].ContainerIP[2],allcontainer->container[i].ContainerIP[3]);
        timeformat(str[3],&allcontainer->container[i].CreateTime);
        timeformat(str[4],&allcontainer->container[i].StartUpTime);
        time_t starttime;
        DT_DateTimeShort2Time(&allcontainer->container[i].StartUpTime,&starttime);
        allcontainer->container[i].RunTime =timenow-starttime;
        //printf("运行时长 %d \n",allcontainer->container[i].RunTime);
    }
    memset(buff,0x00,sizeof(buff));
    count = getshellresult("docker stats --no-stream --format '{{.Name}}#{{.CPUPerc}}#{{.MemPerc}}'",buff,20);
    for(i=0;i<count;i++)
    {
        int index =-1;
        char str[50][100];
        int num;
        memset(str,0x00,sizeof(str));
        ssplit(buff[i], "#",str, &num);
        if(num == 0)
            continue;
        for(j=0;j<allcontainer->containercount;j++)
        {
            if(strcmp(str[0],(char*)allcontainer->container[j].ContainerName)==0)
            {
                index = j;
                break;
            }
        }
        if(index == -1)
            continue;
        allcontainer->container[j].CpuRate =percentdatachange(str[1]);
        allcontainer->container[j].MenRate =percentdatachange(str[2]);
        //printf(" cpurate %d mem %d \n",allcontainer->container[j].CpuRate,allcontainer->container[j].MenRate);
    }
}
/**
*********************************************************************
* @brief：     获取容器信息
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
 sint32
msg_get_container_info(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    uint8 *tmp_xdr     = rsp_xdr;
    ALL_CONTAINER_INFO_T AllContainer;
    memset(&AllContainer,0x00,sizeof(ALL_CONTAINER_INFO_T));
    uint8 i;
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);
    pthread_mutex_lock(&allcontainerinfo.lock);
    AllContainer.containercount = allcontainerinfo.count;
    for(i=0;i<AllContainer.containercount ;i++)
    {
        memcpy(&AllContainer.container[i],&allcontainerinfo.containers[i].container,sizeof(EVERY_CONTAINER_T));
    }
    pthread_mutex_unlock(&allcontainerinfo.lock);

    push_container_info(&rsp_xdr,&AllContainer);
    return rsp_xdr - tmp_xdr;
}
//Total app number 2 
uint8 get_appnum(char* buf)
{
    char data[10]={0};
    const char* str = "Total app number";
    char* p = strstr(buf,str);
    if(p==NULL)
        return 0;
    strcpy(data,p+strlen(str));
    return atoi(strim(data));
}
//2020-06-09 01:01:57 格式的时间
OOP_DATETIME_S_T timeformat_s(char* time)
{

    OOP_DATETIME_S_T tmp;
    memset(&tmp,0x00,sizeof(OOP_DATETIME_S_T));
    int year,month,day,hour,minute,second;
    
    sscanf(time,"%d-%d-%d %d:%d:%d.",&year,&month,&day,&hour,&minute,&second);
    tmp.year =year;
    tmp.month =month;
    tmp.day =day;
    tmp.hour =hour;
    tmp.minute =minute;
    tmp.second =second;
    return tmp;
}
void get_app_info(char* containername,ALL_APP_INFO_T* appinfo)
{
    int count;
    uint8 i;
    char buff[100][500];
    memset(buff,0,sizeof(buff));
    char cmd[50]={0};
    if(NULL == containername  || 0 == strlen(containername))
        return ;
    sprintf(cmd,"appm -I -c %s",containername);
    count = getshellresult(cmd,buff,100);
    if(count<=1)
        return;
    appinfo->appcount = get_appnum(buff[0]);
    INIT_FMT_DEBUG("get_app_info containername %s count %d appcount %d\n",containername,count, appinfo->appcount);
    for(i=0;i<appinfo->appcount;i++)
    {
        strcpy((char*)appinfo->appinfo[i].containername,containername);
        appinfo->appinfo[i].servicecount = 1;
    }
    i = 1;
    int appindex = -1;
    while(i<count)
    {
        if(strlen(buff[i])==0)
        {
            i++;
            continue;
        }
        char key[50]={0};
        char value[50]={0};
        char *p = strstr(buff[i],":");
        if(NULL == p)
        {
            i++;
            continue;
        }
        int len = strlen(p);
        INIT_FMT_DEBUG("buff[%d] [%s] plen %d \n",i,buff[i],len);
        if(len <= 1 )
        {
            i++;
            continue;
        }
        memcpy(key,buff[i],p-buff[i]);
        memcpy(value,p+1,strlen(p)-1);
        char* keytrim = strim(key);
        char* valuetrim = strim(value);
        double memlimit = 300;
        if(strcmp(keytrim,"App index")==0)
        {
            appindex++;
            appinfo->appinfo[appindex].appindex = atoi(valuetrim);
            INIT_FMT_DEBUG("index %d \n\n",appinfo->appinfo[appindex].appindex);
        }else if(strcmp(keytrim,"App name")==0)
        {
            strcpy((char*)appinfo->appinfo[appindex].appname,valuetrim);
            INIT_FMT_DEBUG("name %s\n\n",appinfo->appinfo[appindex].appname);
        }else if(strcmp(keytrim,"App version")==0)
        {
            strcpy((char*)appinfo->appinfo[appindex].appversion,valuetrim);
            INIT_FMT_DEBUG("version %s\n\n",appinfo->appinfo[appindex].appversion);
        }else if(strcmp(keytrim,"App hash")==0)
        {
            strcpy((char*)appinfo->appinfo[appindex].apphash,valuetrim);
            INIT_FMT_DEBUG("hash %s\n\n",appinfo->appinfo[appindex].apphash);
        }else if(strcmp(keytrim,"Service index")==0)
        {
            appinfo->appinfo[appindex].serviceinfo[0].serviceindex = atoi(valuetrim);
            INIT_FMT_DEBUG("Serviceindex %d\n\n",appinfo->appinfo[appindex].serviceinfo[0].serviceindex);
        }else if(strcmp(keytrim,"Service name")==0)
        {
            strcpy((char*)appinfo->appinfo[appindex].serviceinfo[0].servicename,valuetrim);
            INIT_FMT_DEBUG("Servicename  %s\n\n",appinfo->appinfo[appindex].serviceinfo[0].servicename);
        }else if(strcmp(keytrim,"Service enable")==0)
        {
            if(strcmp(valuetrim,"yes")==0)
                appinfo->appinfo[appindex].serviceinfo[0].serviceenable = 1;
            else
                appinfo->appinfo[appindex].serviceinfo[0].serviceenable = 0;
            INIT_FMT_DEBUG("Service enable %d\n\n",appinfo->appinfo[appindex].serviceinfo[0].serviceenable);
        }else if(strcmp(keytrim,"Service status")==0)
        {
            if(strcmp(valuetrim,"running")==0)
                appinfo->appinfo[appindex].serviceinfo[0].servicestate = 0;
            else
                appinfo->appinfo[appindex].serviceinfo[0].servicestate = 1;
            INIT_FMT_DEBUG("Service status %d\n\n",appinfo->appinfo[appindex].serviceinfo[0].servicestate);
        }else if(strcmp(keytrim,"CPU threshold")==0)
        {
            appinfo->appinfo[appindex].serviceinfo[0].cpulimit = percentdatachange(valuetrim);
            INIT_FMT_DEBUG("CPU threshold %d\n\n",appinfo->appinfo[appindex].serviceinfo[0].cpulimit);
        }else if(strcmp(keytrim,"CPU usage")==0)
        {
            appinfo->appinfo[appindex].serviceinfo[0].cpurate = percentdatachange(valuetrim);
            INIT_FMT_DEBUG("CPU usage %d\n\n",appinfo->appinfo[appindex].serviceinfo[0].cpurate);
        }else if(strcmp(keytrim,"Mem Limit")==0)
        {
            memlimit = atof(valuetrim);
            INIT_FMT_DEBUG("memlimit %f\n\n",memlimit);
        }else if(strcmp(keytrim,"Mem usage")==0)
        {
            appinfo->appinfo[appindex].serviceinfo[0].memrate = (uint16)(atof(valuetrim)*100/memlimit);
            INIT_FMT_DEBUG("Mem usage %d\n\n",appinfo->appinfo[appindex].serviceinfo[0].memrate);
        }else if(strcmp(keytrim,"Start time")==0)
        {
            appinfo->appinfo[appindex].serviceinfo[0].starttime =timeformat_s(valuetrim);

        }
        i++;
    }
}
/**
*********************************************************************
* @brief：     获取容器内app信息
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/ sint32
msg_get_app_info(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    uint8 *tmp_xdr     = rsp_xdr;
    ALL_APP_INFO_T appinfo;
    memset(&appinfo,0x00,sizeof(ALL_APP_INFO_T));
    char containername[100]={0};
    uint8 i;
    uint8* str = NULL;
    uint32 n;
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    n = pop_nbyte(&req_xdr, &str);
    strncpy(containername, (sint8 *)str, n > sizeof(containername) ? sizeof(containername) : n);
    MQTTMSG_FMT_DEBUG("msg_get_app_info containername %s\n", containername);
    pthread_mutex_lock(&allcontainerinfo.lock);
    for(i=0;i<allcontainerinfo.count;i++)
    {
        if(strcmp(containername,(char*)allcontainerinfo.containers[i].container.ContainerName)==0)
        {
            memcpy(&appinfo,&allcontainerinfo.containers[i].apps,sizeof(ALL_APP_INFO_T));
            break;
        }
    }
    pthread_mutex_unlock(&allcontainerinfo.lock);
    //get_app_info(containername,&appinfo);
    push_app_info(&rsp_xdr,&appinfo);
    return rsp_xdr - tmp_xdr;
}

OOP_DATETIME_S_T g_smiStartTime;

void smios_set_starttime()
{
    struct  tm       systime;
    struct  timeval  tv;
    struct  timezone tz;
    
    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &systime);

    memset(&g_smiStartTime, 0, sizeof(OOP_DATETIME_S_T));
    g_smiStartTime.year   = systime.tm_year + 1900; 
    g_smiStartTime.month  = systime.tm_mon + 1;
    g_smiStartTime.day    = systime.tm_mday;
    g_smiStartTime.week   = systime.tm_wday;
    g_smiStartTime.hour   = systime.tm_hour;
    g_smiStartTime.minute = systime.tm_min;
    g_smiStartTime.second = systime.tm_sec;
}

/**
*********************************************************************
* @brief：     获取设备当前运行信息
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_get_dev_run_info(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    uint8 *tmp_xdr     = rsp_xdr;
    DEV_RUN_INTO_T info;
    memset(&info,0x00,sizeof(DEV_RUN_INTO_T));

    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);
    sys_dev_run_info_get(&info);
    info.starttime = g_smiStartTime;
    SMIOS_FMT_DEBUG("查询设备当前运行信息 虚拟内存：%d 内存使用率：%d 磁盘使用率：%d cpu使用率：%d \n 操作系统版本：%s 内核：%s 启动时间：%d-%d-%d %d:%d:%d\n",
        info.virmem,info.memrate,info.storagerate,info.cpurate,info.osname,info.oskernel,
        info.starttime.year,info.starttime.month,info.starttime.day,info.starttime.hour,info.starttime.minute,info.starttime.second);
    push_dev_run_info(&rsp_xdr,&info);
    return rsp_xdr - tmp_xdr;
}

/**
*********************************************************************
* @brief：     执行system命令
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_exec_system_cmd(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    uint8 *tmp_xdr     = rsp_xdr;
    uint8 count;
    uint8 i;
    char cmd[500]={0};
    uint8* str = NULL;
    uint32 n;
    sint32 result;
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    count = pop_u8(&req_xdr);
    push_u8(&rsp_xdr,count);
    for(i=0;i<count;i++)
    {
        memset(cmd,0x00,sizeof(cmd));
        n = pop_nbyte(&req_xdr, &str);
        strncpy(cmd,(sint8 *)str, n > sizeof(cmd) ? sizeof(cmd) : n);
        result = system(cmd);
        push_s32(&rsp_xdr,result);
    }

    return rsp_xdr - tmp_xdr;
}

void sntp_timing(void)
{
    char cmd[100]={0};
    uint32 rtcTag                     = 0x00000005;
    
    sprintf(cmd,"ntpdate %s",sntpsource);
    system(cmd);
    lastsntptime = time(NULL);
    MQTTMSG_FMT_DEBUG("sntp_timing sourc %s lastsntptime %d ! \n\n",sntpsource,lastsntptime);
    general_event_handler(rtcTag,NULL, 0);
}

/**
*********************************************************************
* @brief：     启用SNTP对时
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_sntp_timing(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    uint8 *tmp_xdr     = rsp_xdr;

    uint8* str = NULL;
    uint32 n;

    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);
    n = pop_nbyte(&req_xdr, &str);
    strncpy(sntpsource, (sint8*)str, n > sizeof(sntpsource) ? sizeof(sntpsource) : n);
    sntpperiod = pop_u32(&req_xdr);
    SMIOS_FMT_DEBUG("sntp_timing sntpperiod %d ! \n\n",sntpperiod);
    sntp_timing();
    return rsp_xdr - tmp_xdr;
}

//检查是否需要周期执行sntp对时
void check_sntp_timing(void)
{
    if(sntpperiod==0||lastsntptime==0)
        return;
    uint32 timepast = time(NULL)-lastsntptime;
    if(timepast >=sntpperiod)
    {
        MQTTMSG_FMT_DEBUG("time to sntp_timing timepast %d lastsntptime %d sntpperiod %d\n\n",timepast,lastsntptime,sntpperiod);
        sntp_timing();
    }
}

//检查是否产生停电和上电事件
void check_power_on_off_event(void)
{
    static sint32 lastpowerstate = -1;
    static sint32 lastpowerstate1 = 1;
    static sint32 lastpowerstate2 = 1;
    static sint32 lastpowerstate3 = 1;
    sint32 powerstate;
    int ret = 0;
    uint32  eventtag;
        
    ret = system_get_main_power_state(&powerstate);
    if(ret==-1)
    {
        MQTTMSG_FMT_DEBUG("system_get_main_power_state fail! \n\n");
        return ;
    }
    if(powerstate == lastpowerstate1 && powerstate == lastpowerstate2 && powerstate == lastpowerstate3)
    {
        if(0 == lastpowerstate && powerstate == 1)
        {
            MQTTMSG_FMT_DEBUG("power on !!! \n");
            eventtag = 0x00000004;
            general_event_handler(eventtag,NULL, 0);
        }
        else if(1 == lastpowerstate && powerstate == 0)
        {
            MQTTMSG_FMT_DEBUG("power off !!! \n");
            eventtag = 0x00000001;
            general_event_handler(eventtag,NULL, 0);
        }
		else if(-1 == lastpowerstate && powerstate == 0)			//终端通过按键拉起，没有电源供电，直接是电池，5分钟后断电
        {
            MQTTMSG_FMT_DEBUG("power off !!! \n");
        }
        lastpowerstate = powerstate;
    }
    lastpowerstate1 = lastpowerstate2;
    lastpowerstate2 = lastpowerstate3;
    lastpowerstate3 = powerstate;
}

void app_get_maketime(char *ptime, OOP_DATETIME_S_T *pTime)
{
    uint32 a, b, c, d, e, f;
    sscanf(ptime, "%u/%u/%u %u:%u:%u", &a, &b, &c, &d, &e, &f);

    pTime->year = (uint16)a;
    pTime->month = (uint8)b;
    pTime->day = (uint8)c;

    pTime->hour = (uint8)d;
    pTime->minute = (uint8)e;
    pTime->second = (uint8)f;
}
/**
*********************************************************************
* @brief：     获取版本信息
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_get_version(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    uint8 *tmp_xdr     = rsp_xdr;
    char version[20] = {0}; 
    strcpy(version,APP_VERSION);
    OOP_DATETIME_S_T pubdate;
    memset(&pubdate,0x00,sizeof(OOP_DATETIME_S_T));
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);
    app_get_maketime(APP_PUBLISH_TIME,&pubdate);
    push_nbyte(&rsp_xdr, (uint8*)version,strlen(version));
    push_data_time_s(&rsp_xdr,&pubdate);

    return rsp_xdr - tmp_xdr;
}
 void* execcmd(void* command)
{
    MQTTMSG_FMT_DEBUG("%s, begin cmd %s \n\n", __FUNCTION__,(char*)command);
    FILE* fp = NULL;
    if ((fp = popen(command, "r")) != NULL)
    {
        pclose(fp);
        fp = NULL;
    } else {
        MQTTMSG_FMT_DEBUG("popen %s error\n", (char*)command);
    }
    MQTTMSG_FMT_DEBUG("%s, end cmd %s \n\n", __FUNCTION__,(char*)command);

    return NULL;
}
/**
*********************************************************************
* @brief：     执行维护命令
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_exec_cmd(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    uint8 *tmp_xdr     = rsp_xdr;
    uint8 count;
    uint8 i;

    uint8* str = NULL;
    uint32 n;
    char ret[100] ;
    strcpy(ret,"Success");
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);
    count = pop_u8(&req_xdr);
    char cmd[count][500];
    memset(cmd,0x00,sizeof(cmd));
    for(i=0;i<count;i++)
    {
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        n = pop_nbyte(&req_xdr, &str);
        strncpy(cmd[i],(sint8 *)str, n > sizeof(cmd[i]) ? sizeof(cmd[i]) : n);
        if ((pthread_create(&tid, &attr, execcmd, cmd[i])) == -1)
        {
            SMIOS_FMT_DEBUG("create execcmd thread failed\n\n");
        } else {
            SMIOS_FMT_DEBUG("create execcmd thread success cmd %s \n\n",cmd[i]);
        }
        pthread_attr_destroy(&attr);
    } 
    push_nbyte(&rsp_xdr, (uint8*)ret, strlen(ret));
    return rsp_xdr - tmp_xdr;
}
/**
*********************************************************************
* @brief：      透明传输
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_transparent(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    uint8 *tmp_xdr     = rsp_xdr;

    char cmd[500]={0};
    uint8* str = NULL;
    uint32 n;
    
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    memset(cmd,0x00,sizeof(cmd));
    n = pop_nbyte(&req_xdr, &str);
    strncpy(cmd,(sint8 *)str, n > sizeof(cmd) ? sizeof(cmd) : n);

    push_nbyte(&rsp_xdr, (uint8*)cmd, strlen(cmd));
    return rsp_xdr - tmp_xdr;
}

static sint32
msg_parainit(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{
    bool res;
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);
    res = pop_u8(&req_xdr);
    MQTTMSG_FMT_DEBUG("get parainit response %d\n\n",res);
    return 0;
}

//通用接口0000 0016 获取appid
static sint32
msg_get_appid(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    uint8 *tmp_xdr     = rsp_xdr;
    uint32 appidlen;
    uint8 appid[APPID_LEN]={0};
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);
    appidlen = get_app_id(appid, APPID_LEN);
    push_nbyte(&rsp_xdr, appid,appidlen);

    return rsp_xdr - tmp_xdr;
}

void ip_change(char* buf,uint8 *ip)
{
    int a,b,c,d;
    sscanf(buf,"%d.%d.%d.%d",&a,&b,&c,&d);
    ip[0] = a;
    ip[1] = b;
    ip[2] = c;
    ip[3] = d;

}

void get_route_info(char *devname,ROUTE_INFO_LIST_T *routeinfo)
{
    if(devname == NULL)
        return ;
    char cmd[100]={0};
    int count;
    int i;
    char buff[100][500];
    char dest[100],gateway[100],mask[100],other[100];

    memset(buff,0,sizeof(buff));
    
    sprintf(cmd,"route -n |grep %s",devname);

    count = getshellresult(cmd,buff,100);
    if(count<=0)
        return;
    routeinfo->num = count;
    for(i = 0;i<count;i++)
    {
        memset(dest,0x00,sizeof(dest));
        memset(gateway,0x00,sizeof(gateway));
        memset(mask,0x00,sizeof(mask));
        sscanf(buff[i],"%s %s %s %s",dest,gateway,mask,other);
        ip_change(dest,routeinfo->infolist[i].dest);
        ip_change(gateway,routeinfo->infolist[i].gateway);
        ip_change(mask,routeinfo->infolist[i].mask);

    }
}

/**
*********************************************************************
* @brief：     APP查询指定名称已注册APP信息
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_query_route(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    uint8 *devname    = NULL;
    uint8 *tmp_xdr         = rsp_xdr;
    pop_nbyte(&req_xdr, &devname);
    ROUTE_INFO_LIST_T routeinfo;
    memset(&routeinfo,0x00,sizeof(ROUTE_INFO_LIST_T));

    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    get_route_info((char*)devname,&routeinfo);
    // app
    SMIOS_FMT_DEBUG("查询路由信息 %s 路由条数 %d\n",devname,routeinfo.num);


    push_route_info(&rsp_xdr, &routeinfo);

    return rsp_xdr - tmp_xdr;
}

static sint32
msg_dbcenter_default(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    return 0;
}
//#if defined PRODUCT_CCU || defined PRODUCT_ECU

/**
*********************************************************************
* @brief： 查询蓝牙参数配置
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_bluetooth_query(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);
    memset(&gBlueToothParam,0x00,sizeof(BLUE_TOOTH_PARAM_T));
    uint32 len ;

    uint16 offset = 0;
    GUI_FMT_DEBUG("收到蓝牙配置查询回码\n");
    
    len = req_xdr[offset++];
    memcpy(gBlueToothParam.name,&req_xdr[offset],len);
    offset+=len;

    len = req_xdr[offset++];
    if(len !=6)
    {
        GUI_FMT_DEBUG("蓝牙mac地址不为6个字节\n");
        return 0;
    }
    memcpy(gBlueToothParam.mac,&req_xdr[offset],len);
    offset+=len;
    gBlueToothParam.power = req_xdr[offset++];
    memcpy_r(&gBlueToothParam.broadperiod, &req_xdr[offset],2);
    offset+=2;
    memcpy_r(&gBlueToothParam.scanperiod, &req_xdr[offset],2);
    offset+=2;

    gBlueQueryFlag = 0;
    return 0;
}
//#endif


/**
*********************************************************************
* @brief： 查询容器应答
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_container_query_rsp(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    uint8  count;
    uint8  i = 0;
    uint8 *recvbuf = req_xdr;
    uint16 index = 0;
    uint16 len;
    uint8  lenoffset;
    
    EVERY_CONTAINER_T container;

    count = recvbuf[index++];
    containerinfo.count = count;
    GUI_FMT_DEBUG("容器数量%d\n", containerinfo.count);
    for(i = 0;i < count;i++)
    {
        memset(&container, 0, sizeof(EVERY_CONTAINER_T));
        /* 容器ID 跳过 */
        container.ContainerID = recvbuf[index++];
        index += container.ContainerID;

        /* 容器名称 */
        len = recvbuf[index++];
        memcpy(container.ContainerName, recvbuf + index, len);
        index+=len;

        /* 容器配置信息 */
        /* CPU核数 */
        container.CpuCores = recvbuf[index++];
        /* CPU监控阈值 */
        index+=sizeof(uint16);
        /* 内存限值 */
        index+=sizeof(uint16);
        /* 内存监控阈值 */
        index+=sizeof(uint16);
        /* 存储限值 */
        index+=sizeof(uint16);
        /* 磁盘存储监控阈值 */
        index+=sizeof(uint16);
        /* 挂载的宿主机文件夹 */
        GET_LEN_OFFSET(len, recvbuf + index, lenoffset);
        index += lenoffset + len;
        /* 应用存储文件路径 */
        GET_LEN_OFFSET(len, recvbuf + index, lenoffset);
        index += lenoffset + len;
        /* 映射的硬件接口 */
        GET_LEN_OFFSET(len, recvbuf + index, lenoffset);
        index += lenoffset + len;
        /* 端口资源配置参数  */
        GET_LEN_OFFSET(len, recvbuf + index, lenoffset);
        index += lenoffset + len;

        /* 容器运行信息 */
        /* 容器状态 */
        GET_LEN_OFFSET(len, recvbuf + index, lenoffset);
        index += lenoffset;
        memcpy(container.ContainerState, recvbuf + index, len);
        index += len;
        /* 容器CPU使用率 */
        memcpy_r(&container.CpuRate, recvbuf + index, sizeof(uint16));
        index += sizeof(uint16);
        /* 容器内存使用率 */
        memcpy_r(&container.MenRate, recvbuf + index, sizeof(uint16));
        index += sizeof(uint16);
        /* 容器存储使用率 */
        memcpy_r(&container.StorageRate, recvbuf + index, sizeof(uint16));
        index += sizeof(uint16);
        /* 容器IP地址 */
        GET_LEN_OFFSET(len, recvbuf + index, lenoffset);
        index += lenoffset;
        if(len > 4)
        {
            memcpy(container.ContainerIP, recvbuf + index, 4);
        }
        else
        {
            memcpy(container.ContainerIP, recvbuf + index, len);
        }
        index+=len;
        /* 容器镜像名称 */
        len = recvbuf[index++];
        memcpy(container.ImageName, recvbuf + index, len);
        index += len;
        /* 容器创建时间 */
        memcpy_r(&container.CreateTime.year, recvbuf + index, sizeof(uint16));
        index += sizeof(uint16);
        container.CreateTime.month = recvbuf[index++];
        container.CreateTime.day = recvbuf[index++];
        container.CreateTime.hour = recvbuf[index++];
        container.CreateTime.minute = recvbuf[index++];
        container.CreateTime.second = recvbuf[index++];
        /* 容器启动时间 */
        memcpy_r(&container.StartUpTime.year,&recvbuf[index],sizeof(uint16));
        index+=sizeof(uint16);
        container.StartUpTime.month = recvbuf[index++];
        container.StartUpTime.day = recvbuf[index++];
        container.StartUpTime.hour = recvbuf[index++];
        container.StartUpTime.minute = recvbuf[index++];
        container.StartUpTime.second = recvbuf[index++];
        memcpy_r(&container.RunTime,&recvbuf[index],sizeof(uint32));
        index += sizeof(uint32);

        memcpy(&containerinfo.containers[i].container, &container, sizeof(EVERY_CONTAINER_T));
    }
    
    gConatinerQueryFlag = 1;
    return 0;
}

/**
*********************************************************************
* @brief： 查询APP应答
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_app_query_rsp(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);

    uint8  i = 0;
    uint8  j = 0;
    uint8  k = 0;
    uint8 *recvbuf = req_xdr;
    uint16 index = 0;
    uint16 len;
    uint8  count = 0;
    ALL_APP_INFO_T apps;

    memset(&apps, 0, sizeof(ALL_APP_INFO_T));
    apps.appcount = recvbuf[index++];
    for(k = 0; k < containerinfo.count; k++)
    {
        GUI_FMT_DEBUG("容器序号%d, 名称%s\n", k      , containerinfo.containers[k].container.ContainerName);
        memset(&containerinfo.containers[k].apps, 0, sizeof(ALL_APP_INFO_T));
    }
    //依次获取每个容器内的APP信息
    for(i = 0; i < apps.appcount; i++)
    {
       /* 序号 */
       apps.appinfo[i].appindex = recvbuf[index++];
       
       /* APP名称 */
       len = recvbuf[index++];
       memcpy(apps.appinfo[i].appname, recvbuf + index, len);
       index += len;
       
       /* 容器名称 */
       len = recvbuf[index++];
       memcpy(apps.appinfo[i].containername, recvbuf + index, len);
       index += len;
       
       /* APP版本号 */
       len = recvbuf[index++];
       memcpy(apps.appinfo[i].appversion, recvbuf + index, len);
       index += len;
       
       /* 哈希值 */
       len = recvbuf[index++];
       memcpy(apps.appinfo[i].apphash, recvbuf + index, len);
       index += len;

       /* 应用服务列表 */
       apps.appinfo[i].servicecount = recvbuf[index++];
       for(j = 0; j < apps.appinfo[i].servicecount; j++)
       {
            /* 服务序号 */
            apps.appinfo[i].serviceinfo[j].serviceindex = recvbuf[index++];
            /* 服务名称 */
            len = recvbuf[index++];
            memcpy(apps.appinfo[i].serviceinfo[j].servicename, recvbuf + index, len);
            index += len;
            /* 服务是否使能 */
            apps.appinfo[i].serviceinfo[j].serviceenable = recvbuf[index++];
            /* 服务状态 */
            apps.appinfo[i].serviceinfo[j].servicestate = recvbuf[index++];
            /* CPU实际使用率 */
            memcpy_r(&apps.appinfo[i].serviceinfo[j].cpurate, recvbuf + index, sizeof(uint16));
            index+=sizeof(uint16);
            /* 内存实际使用率 */
            memcpy_r(&apps.appinfo[i].serviceinfo[j].memrate, recvbuf + index, sizeof(uint16));
            index += sizeof(uint16);
            /* 启动时间 */
            memcpy_r(&apps.appinfo[i].serviceinfo[j].starttime.year, recvbuf + index, sizeof(uint16));
            index+=sizeof(uint16);
            apps.appinfo[i].serviceinfo[j].starttime.month = recvbuf[index++];
            apps.appinfo[i].serviceinfo[j].starttime.day = recvbuf[index++];
            apps.appinfo[i].serviceinfo[j].starttime.hour = recvbuf[index++];
            apps.appinfo[i].serviceinfo[j].starttime.minute = recvbuf[index++];
            apps.appinfo[i].serviceinfo[j].starttime.second = recvbuf[index++];
       }

       /* 应用配置信息 */
       /* CPU核数 */
       index++;
       /* CPU监控阈值 */
       memcpy_r(&apps.appinfo[i].serviceinfo[0].cpulimit, recvbuf + index, sizeof(uint16));
       index += sizeof(uint16);
       /* 内存限值 */
       index += sizeof(uint16);
       /* 内存监控阈值 */
       index += sizeof(uint16);

        GUI_FMT_DEBUG("容器名称%s, APP名称%s\n", apps.appinfo[i].containername, apps.appinfo[i].appname);
        for(k = 0; k < containerinfo.count; k++)
        {
            if(0 == memcmp(containerinfo.containers[k].container.ContainerName, apps.appinfo[i].containername, 100))
            {
                count = containerinfo.containers[k].apps.appcount;
                containerinfo.containers[k].apps.appinfo[count] = apps.appinfo[i];
                containerinfo.containers[k].apps.appcount = count + 1;
                GUI_FMT_DEBUG("写入容器%s\n", containerinfo.containers[k].container.ContainerName);
                break;
            }
        }
    }

    
    gAppQueryFlag = 1;
    return 0;
}


/**
*********************************************************************
* @brief：     启动服务
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_service_start(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    uint8 *tmp_xdr     = rsp_xdr;

    SERVICE_START_T serviceInfo;
    uint32 num;
    uint8 i;
    uint8 result = 1;
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);
    num = pop_u8(&req_xdr);
    if(num>2)
        num = 2;

    for(i=0;i<num;i++)
    {
        memset(&serviceInfo,0x00,sizeof(SERVICE_START_T));
        serviceInfo.serviceType = pop_u8(&req_xdr);
        serviceInfo.authorizeTime = pop_u32(&req_xdr);
        SMIOS_FMT_DEBUG("启动服务 %d (1 ssh 2 sftp) 授权时长 %d \n",serviceInfo.serviceType,serviceInfo.authorizeTime);
        service_start(&serviceInfo);
    }
    
    push_u8(&rsp_xdr, result);

    return rsp_xdr - tmp_xdr;
}


/**
*********************************************************************
* @brief：     停止服务
* @param[in]：
               req_xdr         数据请求序列结构体
               req_xdr_len     数据请求长度
               client          客户端对象
* @param[out]：
               rsp_xdr         数据响应结构体
               rsp_xdr_max_len 数据响应最大长度
* @return：    0
*********************************************************************
*/
static sint32
msg_service_stop(uint8 *req_xdr, sint32 req_xdr_len, uint8 *rsp_xdr, sint32 rsp_xdr_max_len, MSG_CLIENT_T *client)
{

    uint8 *tmp_xdr     = rsp_xdr;

    uint8 serviceType;
    uint8 result = 1;
    UNUSED(req_xdr);
    UNUSED(req_xdr_len);
    UNUSED(rsp_xdr);
    UNUSED(rsp_xdr_max_len);
    UNUSED(client);
    serviceType = pop_u8(&req_xdr);
    
    SMIOS_FMT_DEBUG("停止服务 %d (1 ssh 2 sftp) \n",serviceType);

    service_stop(serviceType);
    
    push_u8(&rsp_xdr, result);

    return rsp_xdr - tmp_xdr;
}

/**
*********************************************************************
* @brief：     查找对应消息的处理接口
* @param[in]：
               tag             消息tag
* @param[out]：
               none
* @return：    MSG_HANDLE
*********************************************************************
*/

static MSG_HANDLE select_handle_func(uint32 tag)
{
    MSG_HANDLE handle = NULL;
    uint16 IID        = tag >> 16;     //消息的IID
    uint16 IOP        = tag & 0xffff;  //消息的IOP

#ifdef SQL_DUMP_DEBUG
    SMIOS_FMT_TRACE("select_handle_func IID:%04x **  IOP:%04x \n\n", IID, IOP);
#endif
    switch (IID)
    {
        case 0x0000:
            switch (IOP)
            {
                case 0x0010:  //心跳检测接口
                    handle = msg_heartbeat_check;
                    break;
                case 0x0011:
                    handle = msg_exec_cmd;
                    break;
                case 0x0012:
                    handle = msg_transparent;
                    break;
                case 0x0013:
                    handle = msg_get_version;
                    break;
                case 0x0016:
                    handle = msg_get_appid;
                    break;
                default:
                    SMIOS_FMT_DEBUG("未识别 IOP[%04x]\n", IOP);
                    break;
            }
            break;
        case 0x0001:
            switch (IOP)
            {
                case 0x0010:  // APP请求注册
                    handle = msg_app_login_request;
                    break;
                case 0x0011:  // APP请求注销
                    handle = msg_app_logout_request;
                    break;
                case 0x0012:  // APP查询注册的APP
                    handle = msg_query_login_apps;
                    break;
                case 0x0013:  // APP通过名称查询注册APP信息
                    handle = msg_query_app_by_name;
                    break;
                case 0x0020:  // 事件订阅
                    handle = msg_event_subscribe;
                    break;
                case 0x0021:  // 事件取消订阅
                    handle = msg_event_unsubscribe;
                    break;
                case 0x0022:  // 查询订阅的事件信息
                    handle = msg_get_subscribe_info;
                    break;
                case 0x0030:  // 请求系统重启
                    handle = msg_request_system_restart;
                    break;
                case 0x0031:  // 查询电源状态
                    handle = msg_get_power_status;
                    break;
                case 0x0032:  // 查询以太网插拔状态
                    handle = msg_get_ethernet_status;
                    break;
                case 0x0035: //配置本机IP地址
                    handle = msg_set_local_ip;
                    break;
                case 0x0036: //添加路由表
                    handle = msg_set_add_route;
                    break;
                case 0x0037: //删除路由表
                    handle = msg_set_del_route;
                    break;
                case 0x0038: // 时钟同步
                    handle = msg_set_rtc_sync;
                    break;
                case 0x0039: // 启用SNTP对时
                    handle = msg_sntp_timing;
                    break;
                case 0x003A://获取设备信息
                    handle = msg_get_device_info;
                    break;
                case 0x003B://执行system命令
                    handle = msg_exec_system_cmd;
                    break;
//                case 0x003C://获取容器信息
//                    handle = msg_get_container_info;
//                    break;
//                case 0x003D://获取容器信息
//                    handle = msg_get_app_info;
//                    break;
                case 0x003E://获取设备当前运行信息
                    handle = msg_get_dev_run_info;
                    break;
                case 0x003F:    //查询路由
                    handle = msg_query_route;
                    break;
                case 0x0040:    //启动服务
                    handle = msg_service_start;
                    break;
                case 0x0041:    //关闭服务
                    handle = msg_service_stop;
                    break;
                default:
                    SMIOS_FMT_DEBUG("未识别 IOP[%04x]\n", IOP);
                    break;
            }
            break;
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
        case 0x000A:
            switch(IOP)
                {
                case 0x0013:
                    handle = msg_bluetooth_query;
                    break;
                default:
                    break;
            }
            break;
//#endif
        case 0x000B:
            switch(IOP)
                {
                case 0x0013:
                    handle = msg_container_query_rsp;
                    break;
                case 0x0017:
                    handle = msg_app_query_rsp;
                    break;
                default:
                    break;
            }
            break;
        case 0x0005:
            switch (IOP)
            {
//                case 0x0001:  //数据更新事件
//                    handle = data_update_event;
//                    break;
//                case 0x0002:  //初始化事件
//                    handle = db_init_event;
//                    break;
                case 0x0032:
                    handle = msg_parainit;
                    break;
                default:
                    handle = msg_dbcenter_default;
                   // SMIOS_FMT_DEBUG("未识别 IOP[%04x]\n", IOP);
                    break;
            }
            break;
        default:
            SMIOS_FMT_DEBUG("未识别 IOP[%04x]\n", IID);
            break;
    }


    return handle;
}

/**
*********************************************************************
* @brief：     获得转发客户端信息
* @param[in]：
               destAppName       目的APP名称    
* @param[out]： client            客户端对象 
* @return：     0
*********************************************************************
*/
sint32 msg_get_forward_client_info(sint8 *destAppName, MSG_CLIENT_T *client)
{
    UNUSED(destAppName);
    UNUSED(client);
#ifdef TEST
    client->type = UDP;
    strncpy(client->client.uClient.ip, "10.12.42.253", sizeof(client->client.uClient.ip));
    client->client.uClient.port = 8888;
#endif
    return 0;
}

/**
*********************************************************************
* @brief：     消息转发
* @param[in]：
               msgReq       消息请求
               rawObjReq
               client       
* @param[out]： none
* @return：     none
*********************************************************************
*/
void msg_forward(MSG_OBJ_T *msgReq, RAW_OBJ_T *rawObjReq, MSG_CLIENT_T *client)
{
    sint8 *destAppName   = NULL;
    MSG_CLIENT_T fClient = {0};

    UNUSED(client);
    UNUSED(rawObjReq);
    UNUSED(msgReq);
    UNUSED(destAppName);
    destAppName = msgReq->destination;
    // msg_get_forward_client_info(destAppName, &fClient);

    // 查询目的app注册信息
    if (0 != app_manager_app_query(destAppName, &fClient))
    {
        MQTTMSG_FMT_DEBUG("can't query client %s\n", destAppName);
        return;
    }

    switch (fClient.type)
    {
        case UDP:
            MQTTMSG_FMT_DEBUG("udp_sendto ip:[%s] port:[%u] len:[%u]", fClient.client.uClient.ip, fClient.client.uClient.port,
                        rawObjReq->length);
            //UDP 转发
            udp_sendto(fClient.client.uClient.ip, fClient.client.uClient.port, rawObjReq->payload, rawObjReq->length);
            break;
        case MQTT:
            MQTTMSG_FMT_DEBUG("mqtt_sendto :[%s]  len:[%u]\n", fClient.clientName, rawObjReq->length);
            //MQTT 转发
            mqtt_sendto(fClient.clientName,NULL, rawObjReq->payload, rawObjReq->length);
            break;
        case TCP:
            MQTTMSG_FMT_DEBUG("tcp_sendto ip:[%s] port:[%u] len:[%u]", fClient.client.tClient.ip, fClient.client.tClient.port,
                        rawObjReq->length);
            //TCP 转发
            tcp_sendto(fClient.client.tClient.ip, fClient.client.tClient.port, rawObjReq->payload, rawObjReq->length);
        default:
            break;
    }
}

/**
*********************************************************************
* @brief：     发送以太网状态
* @param[in]：
               char *ifname    以太网名称
               uint8 status    状态
* @param[out]：
* @return：    
*********************************************************************
*/
void msg_send_ethernet_status(char *ifname, uint8 status)
{
    uint32 plugTag                    = 0x00010001;
    uint8  payload[64]                = {0};
    int   offset                      = 0; 

    //插拔事件
    payload[offset++] = strlen(ifname);
    memcpy(payload + offset, ifname, payload[0]);
    offset += payload[0];
    payload[offset++] = status;
    SMIOS_FMT_DEBUG("网口 %s, 状态%d\n", ifname, status);
    general_event_handler(plugTag, payload, offset);
}

/**
*********************************************************************
* @brief：     检查以太网插拔
* @param[in]：
* @param[out]： 
* @return：     
*********************************************************************
*/
void smios_check_ethernet(void)
{
    static int s_smiosEthNum = 0;  //以太网数量
    static char s_EthName[16][24];  //以太网名称 最多8个
    static int s_EthStatus[16] = {0};  //以太网状态 拔出(0)，接入（1）

    int fd, intrface;
    struct ifreq buf[16]; 
    struct ifconf ifc; 
    uint32 ipAddr = 0;
    int i, j;
    int ethstatus = 0;
    int ret = 0;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return;
    }

    ifc.ifc_len = sizeof(buf); 
    ifc.ifc_buf = (caddr_t) buf; 
    ret = ioctl(fd, SIOCGIFCONF, (char *) &ifc);
    if(0 != ret)
    {
        goto ifend;
    }

    /*获取接口数量*/
    intrface = ifc.ifc_len / sizeof(struct ifreq);
    if(intrface <= 0)
    {
        goto ifend;
    }
    
    /* 检查网口类型和IP地址 */
    for(i = 0; i < intrface; i++)
    {
        ipAddr = 0;
        if (!(ioctl (fd, SIOCGIFADDR, (char *)&buf[i]))) 
        {
            if(AF_INET != ((struct sockaddr_in*)(&buf[i].ifr_addr))->sin_family)
            {
                continue;
            }
            ipAddr = ((struct sockaddr_in*)(&buf[i].ifr_addr))->sin_addr.s_addr;
        }
        if(0 == ipAddr)
        {
            continue;
        }
        if ((ioctl (fd, SIOCGIFHWADDR, (char *)&buf[i]))) 
        {
            continue;
        }

        /* 检查以太网 */
        if(ARPHRD_ETHER !=buf[i].ifr_hwaddr.sa_family)
        {
            continue;
        }

        /* 检查是否在缓存的队列中 */
        for(j = 0; j < s_smiosEthNum; j++)
        {
            if(0 == strcmp(buf[i].ifr_name, s_EthName[j]))
            {
                /* 判断是否状态变化 */
                system_get_ethernet_state(s_EthName[j], &ethstatus);
                if(ethstatus != s_EthStatus[j])
                {
                    s_EthStatus[j] = ethstatus;
                    msg_send_ethernet_status(s_EthName[j], ethstatus);
                }
                break;
            }
        }
        /* 不在队列中 */
        if(j == s_smiosEthNum && s_smiosEthNum < 8)
        {
            strcpy(s_EthName[s_smiosEthNum], buf[i].ifr_name);
            system_get_ethernet_state(s_EthName[s_smiosEthNum], &ethstatus);
            s_EthStatus[s_smiosEthNum] = ethstatus;
            
            msg_send_ethernet_status(s_EthName[s_smiosEthNum], ethstatus);
            s_smiosEthNum++;
        }
    }

ifend:
    close(fd);
    return;
}

//#if defined PRODUCT_CCU || defined PRODUCT_ECU

int msg_appgui_handle(MSG_OBJ_T* msgReq)
{
    int ret = -1;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    if(msgReq == NULL)
    {
        return -1;
    }
    pMsgInfo = (MESSAGE_INFO_T*)calloc(1,sizeof(MESSAGE_INFO_T)+msgReq->length);
    pMsgInfo->IID = msgReq->tag >> 16;
    pMsgInfo->IOP = msgReq->tag & 0xffff;

    pMsgInfo->msglen = msgReq->length;
    memcpy(pMsgInfo->playload,msgReq->payload,pMsgInfo->msglen);
    ret = appGUI_recv_back_handle(pMsgInfo);
    free(pMsgInfo);
    pMsgInfo = NULL;
    return ret;
}

//检查是否是需要转发的桌面事件消息 1 需要转发 0发给自己的
int check_gui_event(uint16 iop,uint8* payload)
{
    int ret = 0;
    int guiid;
    switch (iop)
    {
    case 0x0001:
    {
        if(payload[0]==0)
        {
            memcpy_rev(&guiid,&payload[1], 4);
            if(guiid!=getLoginguiID())
            {
                ret = 1;
            }
        }else if(payload[0]==1)
        {
            ret = 1;
        }
    }
        break;
    case 0x0002:
    {
        memcpy_rev(&guiid,&payload[0], 4);
        if(guiid!=getLoginguiID())
        {
            ret = 1;
        }
    }
        break;
    default:
        break;
    }
    return ret;
}
//#endif

void msg_handler(RAW_OBJ_T *rawObjReq, RAW_OBJ_T *rawObjRsp, MSG_OBJ_T *msgReq, MSG_OBJ_T *msgRsp, MSG_CLIENT_T *client)
{
    MSG_HANDLE msgHandle = NULL;
    sint8 *serviceName = NULL;
    uint32 ackLen        = 0;
    uint32 errAckLen     = 0;
    uint8 ackPayload[MAX_PAYLOAD_LEN];
    uint16  IID;
    
    UNUSED(rawObjReq);
    rawObjRsp->length = 0;

    serviceName = get_service_name_by_client(client);

    MQTTMSG_FMT_TRACE("msg_handler source [%s] tag[%08x]\n", msgReq->source, msgReq->tag);
    //发送给smiOS需要处理的消息
    if (0 == strncmp(msgReq->destination, serviceName,strlen(serviceName)) || 
        0 == strcmp(msgReq->destination, "Broadcast"))
    {   
        IID = msgReq->tag >> 16;
        
        if(IID == 0x0007)       //桌面消息单独处理
        {
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
            int    guimsgcheck;
            uint16 IOP = msgReq->tag & 0xffff;
            guimsgcheck = check_gui_event(IOP, msgReq->payload);
            MQTTMSG_FMT_TRACE("收到桌面GUI消息,IID = %04x,IOP = %04x msgcheck %d ",IID,IOP,guimsgcheck);
            if(guimsgcheck==0)
                msg_appgui_handle(msgReq);
            else
            {
                if(is_general_sub_event(msgReq->tag))
                {
                    general_event_handler(msgReq->tag,msgReq->payload, msgReq->length);
                }
            }
//#endif
            return;
        }
        else
        {
            //获取消息处理函数
            msgHandle = select_handle_func(msgReq->tag);
        }
        
        if (NULL == msgHandle)
        {

            if(is_general_sub_event(msgReq->tag))
            {
                //general_event_handler(msgReq->tag,msgReq->payload, msgReq->length);
            }
            else
            {
                SMIOS_FMT_DEBUG("不支持 tag:[%08x]\n", msgReq->tag);
                errAckLen = error_msg_handler(msgReq->payload, msgReq->length, ackPayload, sizeof(ackPayload), msgReq->tag);
                if (errAckLen > 0)
                {
                    *msgRsp             = *msgReq;
                    msgRsp->prm         = (msgReq->prm&0xfe);
                    msgRsp->destination = msgReq->source;
                    msgRsp->source      = (sint8 *)serviceName;
                    msgRsp->tag         = 0x00000014;
                    build_msg(msgRsp, ackPayload, errAckLen, rawObjRsp);
                }
            }
        }
        else
        {
            memset(ackPayload, 0, sizeof(ackPayload));
            ackLen = msgHandle(msgReq->payload, msgReq->length, ackPayload, sizeof(ackPayload), client);
            if (ackLen > 0)
            {
                *msgRsp             = *msgReq;
                msgRsp->prm         = (msgReq->prm&0xfe);
                msgRsp->destination = msgReq->source;
                msgRsp->source      = (sint8 *)serviceName;
                build_msg(msgRsp, ackPayload, ackLen, rawObjRsp);
            }
        }
    }
    else
    {
        //需要转发的消息
        msg_forward(msgReq, rawObjReq, client);
    }
}

/**
*********************************************************************
* @brief：     msgHandler初始化函数
* @param[in]：
               cat       日志打印      
* @param[out]： none
* @return：     none
*********************************************************************
*/
void msg_handler_init(void *cat)
{
    //app订阅列表
    appSubTbl = qhashtbl(0, QHASHTBL_THREADSAFE);
}
