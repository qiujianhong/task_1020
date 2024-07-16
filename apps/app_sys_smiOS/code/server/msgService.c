/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 消息服务接口
* @date： 2019-11-9
*********************************************************************
*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "baseType.h"
#include "hal.h"
#include "eeprom.h"
#include "constant.h"
#include "cJSON.h"
#include "globalConfiguration.h"
#include "mqttService.h"
#include "msgHandler.h"
#include "msgPqueue.h"
#include "msgService.h"
#include "smiOSConfig.h"
#include "tcpService.h"
#include "threadpool.h"
#include "udpService.h"
#include "xdrCustom.h"
#include "systemCtrl.h"

#include "framecomm.h"
#include <sys/prctl.h>

static MSG_SERVICE_T *s_msgService = NULL;
int             rebootflag         = 0;         //系统重启标志
SET_IP_INFO_T   setIpInfo           ;           //ip设置信息
bool            setFlag            = 0;
pthread_t       g_nTimeCheck       = 0;         //定时检测
int             parainitflag  =0 ;      // 恢复出厂参数标志
sint32 queue_message(MSG_HANDER_OBJ_T *msgHanderObj);
extern void check_sntp_timing(void);
extern void smios_check_ethernet(void);
extern void check_power_on_off_event(void);
/**
*********************************************************************
* @brief： 获取递增INDEX号
* @param[in]：
               none
* @param[out]：
               none
* @return： 0
*********************************************************************
*/
static uint16 get_msg_index(void)
{
    return __sync_add_and_fetch(&s_msgService->msgIndex, 1);
}

/**
*********************************************************************
* @brief： 通过客户端对象获取服务名称
* @param[in]：
               none
* @param[out]：
               none
* @return： 0
*********************************************************************
*/
sint8 *get_service_name_by_client(MSG_CLIENT_T *client)
{
    UNUSED(client);
    sint8 *result = NULL;

    //客户端类型
    switch (client->type)
    {
        case UDP:
            result = g_configObj.serviceCfg.udpServiceName;
            break;
        case MQTT:
            result = g_configObj.serviceCfg.mqttServiceName;
            break;
        case TCP:
            result = g_configObj.serviceCfg.tcpServiceName;
            break;
        default:
            MQTTMSG_FMT_DEBUG("not support client type\n");
            break;
    }

    return result;
}

/**
*********************************************************************
* @brief： DB消息句柄处理
* @param[in]：
               arg         入参说明
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void handle_task_db(long arg)
{
    sint64 threadID                = (sint64)arg;
    char ThreadName[20];
    sprintf(ThreadName,"task_db-%ld",threadID);
    prctl(PR_SET_NAME,ThreadName);
    sint32 ret                     = -1;
    MSG_QUEUE_NODE_T node          = {0};
    MSG_HANDER_OBJ_T *msgHanderObj = (MSG_HANDER_OBJ_T *)arg;
    RAW_OBJ_T *rawObjReq           = NULL;
    RAW_OBJ_T *rawObjRsp           = NULL;
    MSG_OBJ_T *msgReq              = NULL;
    MSG_OBJ_T *msgRsp              = NULL;
    node.msgobj                    = &msgHanderObj;
    MSG_CLIENT_T client            = {0};

    UNUSED(threadID);
    do
    {
        //等待信号量
        sem_wait(&s_msgService->dbQueueSem);
        //从消息队列中取出消息
        ret = msg_queue_obj_pop(s_msgService->dbPq, &node);
        if (ret != 0)
        {
            MQTTMSG_FMT_DEBUG("recv dbQueueSem but msg_queue_pop is none\n");
            continue;
        }

        memcpy(&msgHanderObj, node.msgobj, node.len);
        msgHanderObj->getReqDataFp(msgHanderObj->obj, &rawObjReq);
        msgHanderObj->getRspDataFp(msgHanderObj->obj, &rawObjRsp);
        msgHanderObj->getReqDataMsgFp(msgHanderObj->obj, &msgReq);
        msgHanderObj->getRspDataMsgFp(msgHanderObj->obj, &msgRsp);
        msgHanderObj->getClientInfo(msgHanderObj->obj, &client);
        //消息处理函数
        msg_handler(rawObjReq, rawObjRsp, msgReq, msgRsp, &client);
        if (msgHanderObj != NULL)
        {
            msgHanderObj->handerFp(msgHanderObj);  //执行消息的回复
        }

    } while (1);
}

int cmd_execute(const char* cmd,  char* result)
{
    char resultBuf[500];

    FILE* fp = NULL;

    if ((fp = popen(cmd, "r")) != NULL)
    {
        while( fgets(resultBuf, sizeof(resultBuf), fp) != NULL)
        {
            strcat(result, resultBuf);
            if( strlen(result) > sizeof(resultBuf))
            {
                break;
            }
        }
        pclose(fp);
        fp = NULL;
    }
    else
    {
        return -1;
    }
    return 0;
}

int act_set_ip(uint8 bEvent)
{
    SET_IP_INFO_T *pTemp = &setIpInfo;
    char setCmd[256] = {0};
    char strtemp[256] = {0};
    char result[20] = {0};

    /* 先停止 */
    system("killall udhcpc");
    usleep(100000);

    if(0 == setIpInfo.type)
    {
        /* DHCP设置 */
        sprintf(setCmd, "udhcpc -i %s &", pTemp->nodeName);
        if(system(setCmd) < 0)
        {
            MQTTMSG_FMT_DEBUG("%s:%s error!\n",__FUNCTION__,setCmd);
            return -1;
        }
        return 0;
    }
    if(pTemp->netMask[0] == 0)
    {
        pTemp->netMask[0] = 255;
        pTemp->netMask[1] = 255;
        pTemp->netMask[2] = 255;
        pTemp->netMask[3] = 0;
    }
    sprintf(setCmd,"ifconfig %s %d.%d.%d.%d netmask %d.%d.%d.%d up", pTemp->nodeName,
                   pTemp->ipVal[0],pTemp->ipVal[1],pTemp->ipVal[2],pTemp->ipVal[3],
                   pTemp->netMask[0],pTemp->netMask[1],pTemp->netMask[2],pTemp->netMask[3]);
    if(system(setCmd) < 0)
    {
        MQTTMSG_FMT_DEBUG("%s:%s error!\n",__FUNCTION__,setCmd);
        return -1;
    }
    SMIOS_FMT_DEBUG("IP地址设置: %s\n", setCmd);

    if(TRUE == bEvent)
    {
        //system("killall mosquitto");
        //sleep(1);
        //system("mosquitto -c /etc/mosquitto/mosquitto.conf >/dev/null 2>&1 &");
        //SMIOS_FMT_DEBUG("重启mosquitto\n");
    }

    memset(setCmd,0x0,sizeof(setCmd));

#if defined PRODUCT_ZCU_N || defined PRODUCT_ZCU_3 || defined PRODUCT_ZCU_1
    //无docker的情况下使用虚拟网口
    sprintf(setCmd, "ifconfig %s:0 down && ifconfig %s:0 172.17.0.1 up",ETH_FIRST_NAME, ETH_FIRST_NAME);
    system(setCmd);
#endif

    memset(setCmd,0x0,sizeof(setCmd));

    /* 网关不同, 设置 */
    sprintf(setCmd,"route -n|grep %s|grep UG| awk '{ print $2}'",ETH_FIRST_NAME);
    cmd_execute(setCmd, result);
    sprintf(strtemp, "%d.%d.%d.%d", pTemp->gateWay[0],pTemp->gateWay[1],pTemp->gateWay[2],pTemp->gateWay[3]);
    if(0 != strcmp(result, strtemp))
    {
        /* 先删除旧网关 */
        sprintf(setCmd, "route del default %s", pTemp->nodeName);
        system(setCmd);
        usleep(10000);

        if(0 != pTemp->gateWay[0])
        {
            sprintf(setCmd, "route add default gw %d.%d.%d.%d %s",
                            pTemp->gateWay[0],pTemp->gateWay[1],pTemp->gateWay[2],pTemp->gateWay[3], pTemp->nodeName);
            if(system(setCmd) < 0)
            {
                MQTTMSG_FMT_DEBUG("%s:%s error !\n",__FUNCTION__,setCmd);
                return -1;
            }
        }
    }

    if(FALSE == bEvent)
    {
        return 1;
    }
    //发送IP地址变更事件
    uint32 plugTag                    = 0x00010002;
    uint8  payload[200]               = {0};
    int   offset                      = 0;

    payload[offset++] = strlen(pTemp->nodeName);
    memcpy(&payload[offset], pTemp->nodeName, strlen(pTemp->nodeName));
    offset+=strlen(pTemp->nodeName);

    payload[offset++] = 4;
    memcpy(&payload[offset],pTemp->ipVal,4);
    offset+=4;

    payload[offset++] = 4;
    memcpy(&payload[offset],pTemp->netMask,4);
    offset+=4;

    payload[offset++] = 4;
    memcpy(&payload[offset],pTemp->gateWay,4);
    offset+=4;
    MQTTMSG_FMT_DEBUG("%s:send 00010002 ip change event\n",__FUNCTION__);

    general_event_handler(plugTag, payload, offset);
    return 1;
}

int act_set_mac(void)
{
    SET_IP_INFO_T *pTemp = &setIpInfo;
    char setCmd[256] = {0};
    char result[20] = {0};
    char cmpstr[20] = {0};

    sprintf(setCmd,"ifconfig %s | grep 'HWaddr' | awk '{ print $5}' ", pTemp->nodeName);
    memset(result, 0x00, sizeof(result));
    cmd_execute(setCmd, result);
    result[17] = 0;

    sprintf(cmpstr, "%02X:%02X:%02X:%02X:%02X:%02X", pTemp->mac[0], pTemp->mac[1], pTemp->mac[2], pTemp->mac[3],
                                                     pTemp->mac[4], pTemp->mac[5]);
    SMIOS_FMT_DEBUG("%s oldmac %s, newmac %s\n", pTemp->nodeName, result, cmpstr);
    if(0 != strcmp(result, cmpstr) && 0 != strcmp(cmpstr, "00:00:00:00:00:00") && 0 != strcmp(cmpstr, "FF:FF:FF:FF:FF:FF"))
    {
/*         sprintf(setCmd, "ifconfig %s down && ifconfig %s hw ether %s && ifconfig %s up",
                        pTemp->nodeName, pTemp->nodeName, cmpstr, pTemp->nodeName); */
        /*在down和up之间加上延时  */
        sprintf(setCmd,"ifconfig %s down",pTemp->nodeName);
        system(setCmd);
        SMIOS_FMT_DEBUG("cmd [%s]\n",setCmd);

        sleep(1);
        memset(setCmd,0,sizeof(setCmd));
        sprintf(setCmd,"ifconfig %s hw ether %s",pTemp->nodeName, cmpstr);
        system(setCmd);
        SMIOS_FMT_DEBUG("cmd [%s]\n",setCmd);

        sleep(1);
        memset(setCmd,0,sizeof(setCmd));
        sprintf(setCmd,"ifconfig %s up",pTemp->nodeName);
        system(setCmd);
        SMIOS_FMT_DEBUG("cmd [%s]\n",setCmd);

    }
    return 0;
}

int local_mac_info_save(void)
{
    SET_IP_INFO_T * ptemp = NULL;
    uint8 mactemp[8] = {0};
    /* MAC保存在铁电 */
    EEPROM_DEVICE_T* dev = (EEPROM_DEVICE_T*)hal_device_get(HW_DEVICE_ID_EEPROM);
    int  ret = 0;
    ptemp = &setIpInfo;
    if (NULL == dev)
    {
        SMIOS_FMT_DEBUG("eeprom_write null\n\n");
        return -1;
    }

    /* 关闭写保护 */
    dev->eeprom_write_protect_enable(dev, 0);
    memcpy(mactemp, ptemp->mac, 6);
    mactemp[6] = mactemp[7] =  mactemp[0] + mactemp[1] + mactemp[2] + mactemp[3] + mactemp[4] + mactemp[5];
    ret = dev->eeprom_write(dev, mactemp, 0, 8);
    if (ret < 0)
    {
        MQTTMSG_FMT_DEBUG("eeprom_write err = %d\n\n", ret);
    }
    /* 打开写保护 */
    dev->eeprom_write_protect_enable(dev, 1);
    hal_device_release((HW_DEVICE *)dev);

    return 0;
}

int local_ip_info_save(void)
{
    MQTTMSG_FMT_DEBUG("save the local ip info ...\n");
    FILE* fp = NULL;
    char nodeName[128] = {0};
    char ipVal[16] = {0};
    char netMask[16] = {0};
    char gateWay[16] = {0};
    SET_IP_INFO_T * ptemp = NULL;
    ptemp = &setIpInfo;

    local_mac_info_save();

    //其他写文件
    strcpy(setIpInfo.nodeName,ptemp->nodeName);
    sprintf(nodeName,"%s",ptemp->nodeName);
    sprintf(ipVal,"%d.%d.%d.%d",ptemp->ipVal[0],ptemp->ipVal[1],ptemp->ipVal[2],ptemp->ipVal[3]);
    sprintf(netMask,"%d.%d.%d.%d",ptemp->netMask[0],ptemp->netMask[1],ptemp->netMask[2],ptemp->netMask[3]);
    sprintf(gateWay,"%d.%d.%d.%d",ptemp->gateWay[0],ptemp->gateWay[1],ptemp->gateWay[2],ptemp->gateWay[3]);

    //pthread_mutex_unlock(&ptemp->ipLock);

    fp = fopen(g_configObj.serviceCfg.localIpInfoFile,"w");
    if(NULL == fp)
    {
        MQTTMSG_FMT_DEBUG("open localIpInfoFile failed!\n");
        return -1;
    }
    char recordContext[IP_RECORD_MAX_LENGTH + 1];
    memset(recordContext, 0, IP_RECORD_MAX_LENGTH);
    sprintf(recordContext,"{\"nodename\":\"%s\",\"type\":%d,\"ip\":\"%s\",\"netmask\":\"%s\",\"gateway\":\"%s\"}",
                          nodeName,ptemp->type, ipVal, netMask,gateWay);
    fwrite(recordContext,1,strlen(recordContext),fp);
    fclose(fp);
    fp = NULL;

    return 1;
}

/**
*********************************************************************
* @brief： 从记录的/data/app/smiOS/localIpInfo   文件中获取IP信息
* @param[in]：
* @param[out]：
               ipinfo        ip信息
* @return： 返回0 正常退出 返回-1 文件读取失败
*********************************************************************
*/
int load_ip_info(SET_IP_INFO_T *ipinfo)
{
    FILE *fp = NULL;
    char buf[256] = {0};
    uint32 temp[4] = {0};
    int iRet = 0;

    EEPROM_DEVICE_T* dev = (EEPROM_DEVICE_T*)hal_device_get(HW_DEVICE_ID_EEPROM);
    if (NULL != dev)
    {
        uint8 mactemp[8] = {0};
        uint8 MACCheck = 0;
        iRet = dev->eeprom_read(dev, mactemp, 0, 8);
        if(8 != iRet)
        {
            SMIOS_FMT_DEBUG("eeprom_read err = %d\n\n", iRet);
            memset(ipinfo->mac, 0, 6);
        }
        hal_device_release((HW_DEVICE *)dev);
        MACCheck = mactemp[0] + mactemp[1] + mactemp[2] + mactemp[3] + mactemp[4] + mactemp[5];
        SMIOS_BUF_DEBUG(mactemp, 8, "MAC原始数据, check %02X", MACCheck);
        if(MACCheck == mactemp[6] && MACCheck == mactemp[7])
        {
            memcpy(ipinfo->mac, mactemp, 6);
        }
        else
        {
            SMIOS_FMT_DEBUG("MAC地址校验错误,重新生成\n\n");
            memset(ipinfo->mac, 0, 6);
        }
    }

    if((fp = fopen(g_configObj.serviceCfg.localIpInfoFile, "r")) == NULL)
    {
        SMIOS_FMT_DEBUG("can not open file [%s]\n",g_configObj.serviceCfg.localIpInfoFile);
        return -1;
    }
    sint32 ret = fread(buf, 1 ,256, fp);

    if(ret > 0)
    {
        fclose(fp);
        fp = NULL;
    }

    cJSON* root=  cJSON_Parse(buf);
    if(NULL == root)
    {
        SMIOS_FMT_DEBUG("Parse local ip info error\n");
        return -1;
    }

    cJSON* item_node = cJSON_GetObjectItem(root, "nodename");
    if(item_node)
    {
        sprintf(ipinfo->nodeName,"%s", item_node->valuestring);
    }

    cJSON* item_type = cJSON_GetObjectItem(root, "type");
    if(item_type)
    {
        ipinfo->type = item_type->valueint;
    }

    cJSON* item_ip = cJSON_GetObjectItem(root, "ip");
    if(item_ip)
    {
        memset(temp, 0, sizeof(temp));
        sscanf(item_ip->valuestring, "%u.%u.%u.%u", &temp[0], &temp[1], &temp[2], &temp[3]);
        ipinfo->ipVal[0] = temp[0];
        ipinfo->ipVal[1] = temp[1];
        ipinfo->ipVal[2] = temp[2];
        ipinfo->ipVal[3] = temp[3];
    }

    cJSON* item_netmask = cJSON_GetObjectItem(root,"netmask");
    if(item_netmask)
    {
        memset(temp, 0, sizeof(temp));
        sscanf(item_netmask->valuestring, "%u.%u.%u.%u", &temp[0], &temp[1], &temp[2], &temp[3]);
        ipinfo->netMask[0] = temp[0];
        ipinfo->netMask[1] = temp[1];
        ipinfo->netMask[2] = temp[2];
        ipinfo->netMask[3] = temp[3];
    }

    cJSON* item_gateway = cJSON_GetObjectItem(root,"gateway");
    if(item_gateway)
    {
        memset(temp, 0, sizeof(temp));
        sscanf(item_gateway->valuestring, "%u.%u.%u.%u", &temp[0], &temp[1], &temp[2], &temp[3]);
        ipinfo->gateWay[0] = temp[0];
        ipinfo->gateWay[1] = temp[1];
        ipinfo->gateWay[2] = temp[2];
        ipinfo->gateWay[3] = temp[3];
    }
    return 0;
}

/**
*********************************************************************
* @brief： OTHER 消息句柄处理
* @param[in]：
               arg         入参说明
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void handle_task_other(long arg)
{
    sint64 threadID                = (sint64)arg;
    char ThreadName[20];
    sprintf(ThreadName,"task_other-%ld",threadID);
    prctl(PR_SET_NAME,ThreadName);
    sint32 ret                     = -1;
    MSG_QUEUE_NODE_T node          = {0};
    MSG_HANDER_OBJ_T *msgHanderObj = (MSG_HANDER_OBJ_T *)arg;
    RAW_OBJ_T *rawObjReq           = NULL;
    RAW_OBJ_T *rawObjRsp           = NULL;
    MSG_OBJ_T *msgReq              = NULL;
    MSG_OBJ_T *msgRsp              = NULL;
    node.msgobj                    = &msgHanderObj;
    MSG_CLIENT_T client            = {0};

    UNUSED(threadID);
    do
    {
        //等信号量
        sem_wait(&s_msgService->otherQueueSem);
        //从队列中取出消息
        ret = msg_queue_obj_pop(s_msgService->otherPq, &node);
        if (ret != 0)
        {
            MQTTMSG_FMT_DEBUG("recv otherQueueSem but msg_queue_pop is none\n");
            continue;
        }

        memcpy(&msgHanderObj, node.msgobj, node.len);
        msgHanderObj->getReqDataFp(msgHanderObj->obj, &rawObjReq);
        msgHanderObj->getRspDataFp(msgHanderObj->obj, &rawObjRsp);
        msgHanderObj->getReqDataMsgFp(msgHanderObj->obj, &msgReq);
        msgHanderObj->getRspDataMsgFp(msgHanderObj->obj, &msgRsp);
        msgHanderObj->getClientInfo(msgHanderObj->obj, &client);
        //消息处理
        msg_handler(rawObjReq, rawObjRsp, msgReq, msgRsp, &client);
        if (msgHanderObj != NULL)
        {
            msgHanderObj->handerFp(msgHanderObj);  //执行消息的回复
        }
        check_sntp_timing();
        if (setFlag != 0)
        {
            setFlag = 0;
            local_ip_info_save();
            sleep(3);
            if(act_set_ip(TRUE) < 0)
            {
                MQTTMSG_FMT_DEBUG("act_set_ip failed!\n");
            }
        }
        if (rebootflag != 0)
        {
            rebootflag = 0;
            if(act_after_secs(10,system_reboot) < 0)
            {
                MQTTMSG_FMT_DEBUG("system_reboot failed!\n");
            }
        }
        if(parainitflag !=0)
        {
            parainitflag = 0;
            if(act_after_secs(1,SendParaInitToDB) < 0)
            {
                MQTTMSG_FMT_DEBUG("SendParaInitToDB failed!\n");
            }
        }

    } while (1);
}

/**
*********************************************************************
* @brief： 系统管理器定时管理
* @param[in]：
               pArg         入参说明
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void *smios_time_check(void *pArg)
{
    UNUSED(pArg);
    prctl(PR_SET_NAME,"time_check");

    while(1)
    {
        /* 以太网口状态检查 */
        smios_check_ethernet();

        /* 停上电 */
        check_power_on_off_event();
        sleep(1);
    }
    g_nTimeCheck = 0;
    return 0;
}

//消息订阅主题
static MQTT_SERVER_SUB_TOPIC_T sub[] = {{.topic = "+/M-smiOS", .qos = 0},
                                        {.topic = "M-desktopGui/Broadcast/0007/#", .qos = 0}};

static void mqtt_subscribe_fp(MQTT_SERVER_SUB_TOPIC_T **subTopic, sint32 *subNum)
{
    *subTopic = sub;
    *subNum   = sizeof(sub) / sizeof(sub[0]);
}

/**
*********************************************************************
* @brief： 消息服务初始化系统接口
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 msg_init_system(void)
{
    sint32 ret = -1;

    //msg handler初始化
    msg_handler_init(NULL);
    INIT_FMT_DEBUG("msgInitSystem\n");

    //设置订阅函数
    set_subscribe_cb_fp(mqtt_subscribe_fp);

    // MQTT_SERVER_SUB_TOPIC_T *s;
    // sint32 n = 0;
    // sint32 i = 0;
    // mqtt_subscribe_fp(&s,&n);
    // for(i = 0; i < n; i++){
    //     MQTTMSG_FMT_DEBUG("%%%%%%%%%% s[%d].topic[%s] qos[%d]\n",i,s[i].topic, s[i].qos);
    // }

    do
    {
        s_msgService = (MSG_SERVICE_T *)malloc(sizeof(MSG_SERVICE_T));
        if (NULL == s_msgService)
        {
            INIT_FMT_DEBUG("MSG_SERVICE_T malloc fail\n");
            break;
        }
        //设置请求消息接口函数
        set_queue_message_fp(queue_message);
        //从全局配置获取线程池数
        s_msgService->threadNum = g_configObj.serviceCfg.handerThreadNum;
        //从全局配置获取任务队列数量
        s_msgService->queueSize = g_configObj.serviceCfg.handerTaskQueueSize;

        //dbCenter 消息队列初始化
        s_msgService->dbPq = msg_queue_init();
        if (s_msgService->dbPq == NULL)
        {
            INIT_FMT_DEBUG("s_msgService->dbPq init fail\n");
            break;
        }

        //除dbCenter消息外的消息队列初始化
        s_msgService->otherPq = msg_queue_init();
        if (s_msgService->otherPq == NULL)
        {
            INIT_FMT_DEBUG("s_msgService->otherPq init fail\n");
            break;
        }

        //dbCenter消息队列信号量初始化
        if (-1 == sem_init(&s_msgService->dbQueueSem, 0, 0))
        {
            INIT_FMT_DEBUG("s_msgService->dbQueueSem init fail [%s]\n", strerror(errno));
            break;
        }

        //other消息队列信号量初始化
        if (-1 == sem_init(&s_msgService->otherQueueSem, 0, 0))
        {
            INIT_FMT_DEBUG("s_msgService->otherQueueSem init fail [%s]\n", strerror(errno));
            break;
        }

        return 0;
    } while (0);

    return ret;
}

/**
*********************************************************************
* @brief： 消息服务清理系统接口
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void msg_clean_up_system(void)
{
    //消息队列清除
    msg_queue_clear(s_msgService->dbPq);
}

/**
*********************************************************************
* @brief： 消息服务启动系统接口
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 msg_start_system(void)
{
    sint32 ret = -1;
    uint32 i   = 0;
    do
    {
        MQTTMSG_FMT_DEBUG("msg_start_system\n");
        //s_msgService->threadNum = 1;
        //创建线程池
        s_msgService->dbTaskPool = threadpool_create(s_msgService->threadNum, s_msgService->queueSize, 0);
        if (NULL == s_msgService->dbTaskPool)
        {
            MQTTMSG_FMT_DEBUG("dbTaskPool threadpool_create fail\n");
            break;
        }
        else
        {
            MQTTMSG_FMT_DEBUG(
                      "Thread dbTaskPool started with %d threads and "
                      "queue size of %d\n",
                      s_msgService->threadNum, s_msgService->queueSize);
        }

        for (i = 0; i < s_msgService->threadNum; i++)
        {
            //为线程池添加任务处理函数
            threadpool_add(s_msgService->dbTaskPool, &handle_task_db, (long)i, 0);
        }

        //为other task创建线程池
        s_msgService->otherTaskPool = threadpool_create(s_msgService->threadNum, s_msgService->queueSize, 0);
        if (NULL == s_msgService->otherTaskPool)
        {
            MQTTMSG_FMT_DEBUG("otherTaskPool threadpool_create fail\n");
            break;
        }
        else
        {
            MQTTMSG_FMT_DEBUG(
                      "Thread otherTaskPool started with %d threads and "
                      "queue size of %d\n",
                      s_msgService->threadNum, s_msgService->queueSize);
        }

        for (i = 0; i < s_msgService->threadNum; i++)
        {
            //为线程池添加任务处理函数
            threadpool_add(s_msgService->otherTaskPool, &handle_task_other, (long)i, 0);
        }

        //启动定时检测功能
        ret = pthread_create(&g_nTimeCheck, NULL, smios_time_check, NULL);
        if (ret != 0)
        {
            MQTTMSG_FMT_DEBUG("otherTaskPool threadpool_create fail\n");
            break;
        }
        return 0;
    } while (0);

    return ret;
}

/**
*********************************************************************
* @brief： 消息停止系统接口
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void msg_stop_system(void)
{
    MQTTMSG_FMT_DEBUG("msg_stop_system\n");
    //停止线程
    if(g_nTimeCheck > 0)
    {
        pthread_cancel(g_nTimeCheck);
        pthread_join(g_nTimeCheck, NULL);
    }
    //销毁队列
    msg_queue_deinit(s_msgService->dbPq);
    //销毁信号量
    sem_destroy(&s_msgService->dbQueueSem);
}

/**
*********************************************************************
* @brief： 请求消息接口
* @param[in]：
               msgHanderObj
* @param[out]：none
* @return： 返回值  返回0添加成功
*********************************************************************
*/
sint32 queue_message(MSG_HANDER_OBJ_T *msgHanderObj)
{
    //将消息对象添加至任务队列
    static size_t dbCount       = 0;
    static size_t dbErrCount    = 0;
    static size_t otherCount    = 0;
    static size_t otherErrCount = 0;

    MSG_QUEUE_NODE_T node = {0};
    int ret               = -1;

    MSG_OBJ_T *msgReq = NULL;

    node.msgobj = &msgHanderObj;
    node.len    = sizeof(MSG_HANDER_OBJ_T *);
    node.pri    = msgHanderObj->priority;

    //消息处理
    msgHanderObj->getReqDataMsgFp(msgHanderObj->obj, &msgReq);

    //数据库请求单独处理
    if ((msgReq->tag & 0xffff0000) == 0x00050000)
    {
        //消息入队
        ret = msg_queue_obj_push(s_msgService->dbPq, &node);
        //发送信号量
        sem_post(&s_msgService->dbQueueSem);
        if (ret == 0)
        {
            //db消息数量统计
            dbCount++;
        }
        else
        {
            //入队失败消息统计
            dbErrCount++;
        }
        MQTTMSG_FMT_TRACE(
                   "dbQ msg_queue_push pos:[%zu] ret:[%d] "
                   "dbCount:[%zu] dbErrCount:[%zu]\n",
                   node.pos, ret, dbCount, dbErrCount);
    }
    else
    {
        //消息入队
        ret = msg_queue_obj_push(s_msgService->otherPq, &node);
        //发送信号量
        sem_post(&s_msgService->otherQueueSem);
        if (ret == 0)
        {
            //other消息数量统计
            otherCount++;
        }
        else
        {
            //other消息入队失败数量统计
            otherErrCount++;
        }
        MQTTMSG_FMT_TRACE(
                   "otherQ msg_queue_push pos:[%zu] ret:[%d] "
                   "otherCount:[%zu] otherErrCount:[%zu]\n",
                   node.pos, ret, otherCount, otherErrCount);
    }

    return ret;
}

/**
*********************************************************************
* @brief： 发送指定消息给客户端
* @param[in]：
               client           客户端对象
               data             待发送的数据地址
               dataLen          待发送的数据长度
* @param[out]：none
* @return： 返回值  返回0添加成功
*********************************************************************
*/
sint32 sendmsg_to_client(MSG_CLIENT_T *client,char* topic, void *data, uint32 dataLen)
{
    UNUSED(client);
    sint32 ret = -1;

    do
    {
        //客户端协议类型
        switch (client->type)
        {
            case UDP:
                ret = udp_sendto(client->client.uClient.ip, client->client.uClient.port, data, dataLen);
                break;
            case MQTT:
                // mqtt_sendto((sint8 *)"smiOS", data, dataLen);
                ret = mqtt_sendto((sint8 *)client->clientName,topic, data, dataLen);
                break;
            case TCP:
                ret = tcp_sendto(client->client.tClient.ip, client->client.tClient.port, data, dataLen);
                break;
            default:
                MQTTMSG_FMT_DEBUG("not support client type\n");
                break;
        }

        //ret = 0;
    } while (0);

    return ret;
}

/**
*********************************************************************
* @brief： 发送心跳请求给客户端
* @param[in]：
               client           客户端对象
* @param[out]：none
* @return： 返回值  返回0添加成功
*********************************************************************
*/
sint32 send_heartbeat_req_to_client(MSG_CLIENT_T *client)
{
    sint32 ret = -1;
    MSG_OBJ_T msgObj = {0};
    RAW_OBJ_T rsp    = {0};

    msgObj.prm         = 0x01;
    msgObj.priority    = 0;
    msgObj.index       = get_msg_index();                     //获得消息的index
    msgObj.label       = 0;
    msgObj.source      = get_service_name_by_client(client);  // SERVICE_NAME;
    msgObj.destination = client->clientName;
    msgObj.tag         = 0x00000010;

    MQTTMSG_FMT_TRACE("Send HT REQ clientName:%s service:%s\n", msgObj.destination, msgObj.source);

    //消息打包
    build_msg(&msgObj, NULL, 0, &rsp);
    if (rsp.length > 0)
    {
        //发送消息给客户端
        //sendmsg_to_client(client, rsp.payload, rsp.length);
        ret = sendmsg_to_client(client,NULL, rsp.payload, rsp.length);
    }
    return ret;
}

/**
*********************************************************************
* @brief： 发送指定事件消息给客户端
* @param[in]：
               client           客户端对象
               event            事件编号
               eventPayload     待发送事件数据地址
               payloadLen       待发送事件数据长度

* @param[out]：none
* @return： 返回值  返回0添加成功
*********************************************************************
*/
sint32
send_general_event_to_client(MSG_CLIENT_T *client, GENEERAL_EVENT_E event, uint8 *eventPayload, uint32 payloadLen)
{
    MSG_OBJ_T msgObj = {0};
    RAW_OBJ_T rsp    = {0};
    uint16 iid,iop;
    char* dest = "Broadcast";
    char topic[200]={0};
    iid = event>>16 & 0xffff;
    iop = event & 0xffff;
    //只处理系统管理器的5个紧急消息
    if(iid > 1)
    {
        return 0;
    }
    msgObj.prm         = 1;
    msgObj.priority    = 0;
    msgObj.index       = get_msg_index();                   //计算消息的index
    msgObj.label       = 0;
    msgObj.source      = get_service_name_by_client(client); //获得客户端的服务名称
    msgObj.destination = dest;
    msgObj.tag         = event;
    sprintf(topic,"M-smiOS/Broadcast/%04X/%04X", iid, iop);
    //消息打包
    build_msg(&msgObj, eventPayload, payloadLen, &rsp);
    if (rsp.length > 0)
    {
        //发送消息给客户端
        sendmsg_to_client(client,topic, rsp.payload, rsp.length);
    }
    return 0;
}

//向数据中心发送参数初始化0032
int SendParaInitToDB(void)
{
    MSG_CLIENT_T msgClient;
    memset(&msgClient,0x00,sizeof(MSG_CLIENT_T));
    sprintf(msgClient.clientName,"M-dbCenter");
    msgClient.type = MQTT;

    sint32 ret = -1;
    MSG_OBJ_T msgObj = {0};
    RAW_OBJ_T rsp    = {0};

    msgObj.prm         = 0x01;
    msgObj.priority    = 0;
    msgObj.index       = get_msg_index();                     //获得消息的index
    msgObj.label       = 0;
    msgObj.source      = get_service_name_by_client(&msgClient);  // SERVICE_NAME;
    msgObj.destination  =msgClient.clientName;
    msgObj.tag         = 0x00050032;

    uint8 logicid = 0;
    MQTTMSG_FMT_TRACE("Send HT REQ clientName:%s service:%s\n", msgObj.destination, msgObj.source);

    //消息打包
    build_msg(&msgObj, &logicid, 1, &rsp);
    if (rsp.length > 0)
    {
        ret = sendmsg_to_client(&msgClient,NULL, rsp.payload, rsp.length);
    }
    rebootflag=1;
    return ret;
}


/**
*********************************************************************
* @brief： 发送心跳请求给客户端
* @param[in]：
               client           客户端对象
* @param[out]：none
* @return： 返回值  返回0添加成功
*********************************************************************
*/
sint32 send_bluetooth_param_query(MSG_CLIENT_T *client)
{
    sint32 ret = -1;
    MSG_OBJ_T msgObj = {0};
    RAW_OBJ_T rsp    = {0};

    msgObj.prm         = 0x01;
    msgObj.priority    = 0;
    msgObj.index       = get_msg_index();                     //获得消息的index
    msgObj.label       = 0;
    msgObj.source      = get_service_name_by_client(client);  // SERVICE_NAME;
    msgObj.destination = client->clientName;
    msgObj.tag         = 0x000A0013;

    //消息打包
    build_msg(&msgObj, NULL, 0, &rsp);
    if (rsp.length > 0)
    {
        //发送消息给客户端
        //sendmsg_to_client(client, rsp.payload, rsp.length);
        ret = sendmsg_to_client(client,NULL, rsp.payload, rsp.length);
    }
    return ret;
}

/**
*********************************************************************
* @brief： 发送读取容器消息
* @param[in]：
               client           客户端对象
* @param[out]：none
* @return： 返回值  返回0添加成功
*********************************************************************
*/
sint32 send_iotManager_container_query(MSG_CLIENT_T *client)
{
    sint32 ret = -1;

    MSG_OBJ_T msgObj = {0};
    RAW_OBJ_T rsp    = {0};
    uint8     playload[1] = {0};

    msgObj.prm         = 0x01;
    msgObj.priority    = 0;
    msgObj.index       = get_msg_index();                     //获得消息的index
    msgObj.label       = 0;
    msgObj.source      = get_service_name_by_client(client);  // SERVICE_NAME;
    msgObj.destination = client->clientName;
    msgObj.tag         = 0x000B0013;

    //消息打包
    build_msg(&msgObj, playload, 1, &rsp);
    if (rsp.length > 0)
    {
        //发送消息给客户端
        //sendmsg_to_client(client, rsp.payload, rsp.length);
        ret = sendmsg_to_client(client,NULL, rsp.payload, rsp.length);
    }
    return ret;
}

/**
*********************************************************************
* @brief： 发送读APP消息
* @param[in]：
               client           客户端对象
* @param[out]：none
* @return： 返回值  返回0添加成功
*********************************************************************
*/
sint32 send_iotManager_app_query(MSG_CLIENT_T *client)
{
    sint32 ret = -1;

    MSG_OBJ_T msgObj = {0};
    RAW_OBJ_T rsp    = {0};
    uint8     playload[64] = {0};
    uint16    len = 0;

    msgObj.prm         = 0x01;
    msgObj.priority    = 0;
    msgObj.index       = get_msg_index();                     //获得消息的index
    msgObj.label       = 0;
    msgObj.source      = get_service_name_by_client(client);  // SERVICE_NAME;
    msgObj.destination = client->clientName;
    msgObj.tag         = 0x000B0017;

    playload[len++] = 0;
    playload[len++] = 0;
    //消息打包

    build_msg(&msgObj, playload, len, &rsp);
    if (rsp.length > 0)
    {
        //发送消息给客户端
        //sendmsg_to_client(client, rsp.payload, rsp.length);
        ret = sendmsg_to_client(client,NULL, rsp.payload, rsp.length);
    }
    return ret;
}



