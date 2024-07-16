/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控mqtt
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include <sys/stat.h>
#include <errno.h>
#include "baseCommon.h"
#include "framecomm.h"
#include "iot_mqtt.h"
#include "iot_init.h"
#include "iot_inspect.h"
#include "iot_update.h"
#include "mqtt_client.h"
#include "task_msg.h"
#include "appmsg_split.h"
#include "task_base.h"
#include "devinfo.h"
#include "hal.h"
#include <stdlib.h>
#include <sys/stat.h>

char PrivateDir[50]="/data/app/iotManager/data/";
char PackLogDir[50]="/var/log/";
char SysLogDir[50]="/log/SysLog/"; 
uint16 packnum = 0;

iot_mqtt_type mqtt_type;
pthread_mutex_t   g_IotSendlock;
int mqtt_topic_write(uint8* SendBuf,uint32 SendLen,char* topic);
int mqtt_topic_read(char* topic,int *qos,int *mid,uint8* RecvBuf);
void check_system_upgrade();
extern int getshellresult(const char* cmd,char result[][500],int size);
extern LockQueue_T        LockWarringMsg;
extern LockQueue_T        LockRecvMsg;
extern MQTT_CLIENT_T * i_MqttClient;
extern void ssplit(char *src,const char *delim,char dest[][255],int *num);
extern int32 taskmng_plc_queue_read(LockQueue_T *pLockQueue, uint8 *buf);
extern int32 taskmng_plc_queue_write(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len);
extern uint16 msg_mqtt_package(MSG_INFO_T *pMsgInfo, uint32 bufLenMax, uint8 *buf);
extern uint16 iot_get_next_index();
extern char      g_AppVer[APPMSG_NAME_LEN];
extern uint16 get_len_offset(uint8 *indata,uint8 *bytes);
extern uint8 set_len_offset(uint16 eleLen, uint8 *indata);
extern ALL_CONTAINER_AND_APP_T allcontainerinfo;
extern char * strim(char *str);
extern int read_pdata_xram(const char *filename, char *buf, unsigned offset, unsigned len);
extern int write_pdata_xram(const char *filename, const char *buf, unsigned offset, unsigned len);
void app_action_notice(SEQ_STR *docker,SEQ_STR *app,uint8 type,uint8 errcode);
uint8 sizeAndmd5_judge(char *filepath,uint32 size,char* md5);

int mqtt_topic_write(uint8* SendBuf,uint32 SendLen,char* topic)
{
    return mqtt_client_send_msg(i_MqttClient,topic,0,SendBuf, SendLen);
}

int mqtt_read(uint8* RecvBuf)
{
    return mqtt_client_recv(i_MqttClient,RecvBuf, MAX_MSG_LEN);
}

void print_exec_faild(uint8 num,char p[][500])
{
    if (num == 0)
        return;
    uint8 i=0;
    for(i=0;i<num;i++)
    {
        MQTTMSG_FMT_DEBUG("%s\n",p[i]);
    }
}

/**********************************************************************
* @name      : strdelch
* @brief     : 通用函数去除字符串中的特定字符
* @param[in] : str 源字符串 ch 需要去除的字符
* @param[out]: 
* @return    : 去除后的字符串
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

char* strdelch(char* str,char ch)
{
    char* current = str;
    char* tail = str;
    while(*tail)
    {
        if(*tail == ch)
        {
            tail++;
        }
        else
        {
            *current ++ = *tail++;
        }
    }
    *current =0;
    return str;    
}
/**********************************************************************
* @name      : wireless_send_msg
* @brief     ：对外消息发送
* @param[in] ：char *pTopic              指定主题, NULL使用默认主题
               MESSAGE_INFO_T *pMsgInfo  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-9
* @Update    :
**********************************************************************/
int iot_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
{
    int    ret = 0;
    uint8 *pMsg = NULL;
    uint16 msglen = 0;
    
    if(NULL == pMsgInfo)
    {
        return -1;
    }
    
    msglen = appmsg_package(pMsgInfo, &pMsg);
    if(0 == msglen)
    {
        MQTTMSG_FMT_DEBUG("App send appmsg_package fail\n");
        return -1;
    }

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_IotSendLock);
    if(NULL != pTopic && 'M' == pMsgInfo->destaddr[0])
    {
        ret = mqtt_client_send_msg(i_MqttClient, pTopic, qos, pMsg, (int)msglen);
    }
    else
    {
        ret = mqtt_client_send(i_MqttClient, pMsg, (int)msglen);
    }
    PTHREAD_UNLOCK_SAFE(&g_IotSendLock);

    if(0 == ret)
    {
        MQTTMSG_BUF_TRACE(pMsg, msglen, "Send msg to %s:", pMsgInfo->destaddr);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg failed: index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                            pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
    }

    free(pMsg);
    return ret;


}

/*******************************************************************************
* 函数名称: taskmng_register
* 函数功能: 注册到系统管理服务
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
int  taskmng_register(IOT_APPDATA_T * pdata)
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8 request[160] = {0};
    uint8 offset = 0;
    uint8 len = 0;
    pid_t pid = getpid();
    Date_Time_S tTime;
    char *smios = "M-smiOS";

    app_get_maketime(APP_PUBLISH_TIME, &tTime);

    /* 进程号 */
    appmsg_memcpy_r(request, &pid, sizeof(pid_t));
    offset += 4;

    /* APP名称 */
    len = strlen(IOT_APP_NAME_M);
    request[offset++] = len;
    memcpy(&request[offset], IOT_APP_NAME_M, len);
    offset += len;

    /* 订阅事件 */
    request[offset++] = 0;

    /* 版本信息 */
    request[offset++] = strlen(g_AppVer);
    memcpy(request + offset, g_AppVer, strlen(g_AppVer));
    offset += strlen(g_AppVer);

    /* 发布日期 */
    request[offset++] = tTime.year_h;
    request[offset++] = tTime.year_l;
    request[offset++] = tTime.month;
    request[offset++] = tTime.day;
    request[offset++] = tTime.hour;
    request[offset++] = tTime.min;
    request[offset++] = tTime.sec;

    /* APPID */
    request[offset++] = pdata->appidlen;
    if(pdata->appidlen > 0)
    {
        memcpy(request + offset, pdata->appid, pdata->appidlen);
    }
    offset += pdata->appidlen;
    
    pMsgInfo = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T) + offset);
    if(NULL == pMsgInfo)
    {
        MQTTMSG_FMT_DEBUG("Login to os malloc failed %d\n", sizeof(MESSAGE_INFO_T) + offset);
        return -1;
    }
    memset(pMsgInfo, 0, sizeof(MESSAGE_INFO_T) + offset);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = iot_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_SYSTEM;
    pMsgInfo->IOP = SYSTEM_IOP_APP_REGISTER;
    
    pMsgInfo->sourlen = strlen(IOT_APP_NAME_M);
    memcpy(pMsgInfo->souraddr , IOT_APP_NAME_M, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->msglen = offset;
    memcpy(pMsgInfo->playload, request, offset);
    MQTTMSG_BUF_DEBUG(pMsgInfo,sizeof(MESSAGE_INFO_T)+pMsgInfo->msglen,"IID %04x IOP %04x",pMsgInfo->IID,pMsgInfo->IOP);
    ret = iot_send_msg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : iot_mqttmsg_init
* @brief     ：MQTT线程初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2022-5-26
* @Update    :
**********************************************************************/

int iot_mqttmsg_init(void *pPublic, void **ppPrivate)
{
    IOT_APPDATA_T *pdata = pPublic;
    iot_mqtt_type *ptData = NULL;
    ptData    = (iot_mqtt_type *)malloc(sizeof(iot_mqtt_type));
    if(ptData == NULL)
    {
        MQTTMSG_FMT_DEBUG("初始化失败\n");
            return -1;
    }
    memset(ptData,0,sizeof(iot_mqtt_type));
    if ( pthread_mutex_init ( &g_IotSendLock, NULL ) != 0 )
    {
        printf ("init pthread g_IotSendlock lock error!\n" );
        exit ( 1 );
    }
    if ( pthread_mutex_init ( &g_MqttIndexlock, NULL ) != 0 )
    {
        printf ("init pthread g_IotSendlock lock error!\n" );
        exit ( 1 );
    }
    ptData->status = MSG_INIT;
    *ppPrivate = ptData;
     
    if(access(PrivateDir,R_OK)!=0)
    {
        char cmd[100]={0};
        sprintf(cmd,"mkdir %s",PrivateDir);
        system(cmd);
    }
    check_system_upgrade(); 
    taskmng_register(pdata);
    return 0;
}
/**********************************************************************
* @name      : appmsg_set_len_offset
* @brief     ：设置元素长度
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：const uint8 *indata      数据Buffer
* @return    ：长度
* @Create    : 
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
uint8 appmsg_set_len_offset(uint16 eleLen, uint8 *indata)
{
    if(eleLen < 0x80)
    {
        indata[0] = eleLen;
        return 1;
    }
    else if(eleLen <= 0xffff)
    {
        indata[0] = 0x82;
        indata[1] = (uint8)((eleLen >> 8) & 0xff);
        indata[2] = (uint8)(eleLen & 0xff);
        return 3;
    }
        
    return 0;
}

/**********************************************************************
* @name      : app_get_maketime
* @brief     ：解析定制的编译时间
* @param[in] ：char *ptime        例如 2020/5/22 13:19:20
* @param[out]：Date_Time_S *pTime 编译时间
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void app_get_maketime(char *ptime, Date_Time_S *pTime)
{
    uint32 a, b, c, d, e, f;
    sscanf(ptime, "%u/%u/%u %u:%u:%u", &a, &b, &c, &d, &e, &f);

    pTime->year_h = (uint8)((a & 0xff00) >> 8);
    pTime->year_l = (uint8)(a & 0xff);
    pTime->month = (uint8)b;
    pTime->day = (uint8)c;

    pTime->hour = (uint8)d;
    pTime->min = (uint8)e;
    pTime->sec = (uint8)f;
}


/**********************************************************************
* @name      : appmsg_axdr_package
* @brief     ：指定格式消息组装成
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  结构消息
* @param[out]：uint8 **ppBuf             组装消息
* @return    ：消息长度
* @Create    : 
* @Date      ：2021-7-7
* @Update    :
**********************************************************************/
uint16 iot_appmsg_package(MSG_INFO_T *pMsgInfo, uint8 **ppBuf)
{
    uint16   bufLen = 0;
    uint8 lenBuf[3] = {0};
    uint8    offset = 0;
    uint8  *pMsgBuf = NULL;
    uint8    *pTemp = NULL;
    
    bufLen = 1 + 2 + 2 + pMsgInfo->SourLen + 1 + pMsgInfo->DestLen + 1 + 4;   //计算head和Tag长度
    /* 计算MSG's-Length 长度  */
    offset = appmsg_set_len_offset(pMsgInfo->MsgLen, lenBuf);
    if(0 == offset)
    {
   //     DPRINT("appmsg_udp_package offset wrong\n");
        return 0;
    }

    bufLen += offset + pMsgInfo->MsgLen;

    pMsgBuf = (uint8 *)malloc(bufLen);
    if(NULL == pMsgBuf)
    {
//        DPRINT("appmsg_udp_package malloc failed,size %p\n", pMsgBuf);

        return 0;
    }

    memset(pMsgBuf, 0, bufLen);

    pTemp = pMsgBuf;

    /* PRM PRIORITY */
    *pTemp = ((pMsgInfo->MsgPRIORITY << 1) & 0xfe) + (pMsgInfo->MsgRPM & 0x01);
    pTemp++;

    /* INDEX */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->MsgIndex;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->MsgIndex);
#endif
    pTemp += sizeof(uint16);

    /* LABEL */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->MsgLabel;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->MsgLabel);
#endif
    pTemp += sizeof(uint16);

    /* SOURCE */
    memcpy(pTemp, pMsgInfo->SourAddr, pMsgInfo->SourLen);
    pTemp += pMsgInfo->SourLen + 1;

    /* DESTINATION */
    memcpy(pTemp, pMsgInfo->DestAddr, pMsgInfo->DestLen);
    pTemp += pMsgInfo->DestLen + 1;

    /* MSG's-TAG */
#if IS_LITTLE_ENDIAN
    *((uint32 *)pTemp) =((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff);
#else
    *((uint32 *)pTemp) = htonl(((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff));
#endif

    pTemp += sizeof(uint32);

    /* MSG's-Length */
    memcpy(pTemp, lenBuf, offset);
    pTemp += offset;

    if(pMsgInfo->MsgLen)
    {
        memcpy(pTemp, pMsgInfo->MsgData, pMsgInfo->MsgLen);  //MSG's-Payload
    }

    *ppBuf = pMsgBuf;

    return bufLen;
}


/**********************************************************************
* @name      : msg_mqtt_package
* @brief     ：指定格式消息组装成
* @param[in] ：MSG_INFO_T *pMsgInfo  结构消息
               bufLenMax             buf最大长度
* @param[out]：uint8 *buf            组装消息
* @return    ：消息长度
* @Create    : 
* @Date      ：2019-10-29
* @Update    :
**********************************************************************/
uint16 msg_mqtt_package(MSG_INFO_T *pMsgInfo, uint32 bufLenMax, uint8 *buf)
{
    uint16   bufLen = 0;
    uint8 lenBuf[3] = {0};
    uint8    offset = 0;
    uint8  *pMsgBuf = buf;
    uint8    *pTemp = NULL;

    if ((pMsgInfo == NULL) || (buf == NULL))
    {
     //   MPRINT("msg_mqtt_package param wrong\n");
        return 0;
    }
    
    bufLen = 1 + 2 + 2 + strlen(pMsgInfo->SourAddr) + 1 + strlen(pMsgInfo->DestAddr) + 1 + 4;   //计算head和Tag长度

    /* 计算MSG's-Length 长度  */
    offset = set_len_offset(pMsgInfo->MsgLen, lenBuf);
    if(0 == offset)
    {
    //    MPRINT("msg_mqtt_package offset wrong\n");
        return 0;
    }

    bufLen += offset + pMsgInfo->MsgLen;

    if (bufLen > bufLenMax)
    {
      ///  MPRINT("msg_mqtt_package bufLenMax wrong\n");
        return 0;
    }

    memset(pMsgBuf, 0, bufLen);

    pTemp = pMsgBuf;

    /* PRM PRIORITY */
    *pTemp = ((pMsgInfo->MsgPRIORITY << 1) & 0xfe) + (pMsgInfo->MsgRPM & 0x01);
    pTemp++;

    /* INDEX */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->MsgIndex;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->MsgIndex);
#endif
    pTemp += sizeof(uint16);

    /* LABEL */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->MsgLabel;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->MsgLabel);
#endif
    pTemp += sizeof(uint16);

    /* SOURCE */
    memcpy(pTemp, pMsgInfo->SourAddr, strlen(pMsgInfo->SourAddr));
    pTemp += strlen(pMsgInfo->SourAddr) + 1;

    /* DESTINATION */
    memcpy(pTemp, pMsgInfo->DestAddr, strlen(pMsgInfo->DestAddr));
    pTemp += strlen(pMsgInfo->DestAddr) + 1;

    /* MSG's-TAG */
#if IS_LITTLE_ENDIAN
    *((uint32 *)pTemp) =((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff);
#else
    *((uint32 *)pTemp) = htonl(((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff));
#endif
    pTemp += sizeof(uint32);

    /* MSG's-Length */
    memcpy(pTemp, lenBuf, offset);
    pTemp += offset;

    memcpy(pTemp, pMsgInfo->MsgData, pMsgInfo->MsgLen);  //MSG's-Payload

    return bufLen;
}
void packge_docker_notice_0006(uint8 type,BOOL result)
{
    uint16 index = 0;
    uint8 buff[255]={0};
    IOT_FMT_DEBUG("产生系统升级通知事件\n");
    OOP_DATETIME_S_T StartTime;
    memset(&StartTime,0x00,sizeof(OOP_DATETIME_S_T));
    DT_Time2DateTimeShort(time(NULL),&StartTime);
    memcpy_r(&buff[index],&StartTime.year,sizeof(uint16));
    index+=sizeof(uint16);
    buff[index++]=StartTime.month;
    buff[index++]=StartTime.day;
    buff[index++]=StartTime.hour;
    buff[index++]=StartTime.minute;
    buff[index++]=StartTime.second;
    buff[index++] = type;
    buff[index++] = !result;
    buff[index++] = 0x00;
    
    MSG_INFO_T *MsgSend = (MSG_INFO_T*)malloc(sizeof(MSG_INFO_T)+index);
    if(MsgSend == NULL)
    {
        return;
    }
    memset(MsgSend,0,sizeof(MSG_INFO_T)+index);
    MsgSend->MsgRPM = 1;
    MsgSend->MsgPRIORITY = 1;
    MsgSend->MsgIndex = iot_get_next_index();
    MsgSend->MsgLabel = 0;
    MsgSend->SourLen = strlen(IOT_APP_NAME_M);
    memcpy(MsgSend->SourAddr,IOT_APP_NAME_M,MsgSend->SourLen);
    MsgSend->DestLen = strlen("Broadcast");
    memcpy(MsgSend->DestAddr, "Broadcast", MsgSend->DestLen);
    MsgSend->IID = MESSAGE_IID_IOT;
    MsgSend->IOP = IOT_IOP_SYS_WARRING;
    MsgSend->MsgLen = index;
    memcpy(MsgSend->MsgData,buff,index);
    taskmng_plc_queue_write (&LockWarringMsg, (uint8*)MsgSend, sizeof(MSG_INFO_T)+MsgSend->MsgLen );
    
    free(MsgSend);
}


/**********************************************************************
* @name      : packge_docker_notice_0011
* @brief     ：组0001容器操作通知报文
* @param[in] ：uint8 type   通知类别 dockername 容器名 {安装（0），升级（1），卸载（2），启动（4），停止（4），配置参数（5）}，
* @param[out]：BOOL result* 结果
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void packge_docker_notice_0001(uint8 type,BOOL result,char *dockername,uint16 len)
{
    uint16 index = 0;
    uint16 tmplen = 0;
    uint8 buff[255]={0};
    IOT_FMT_DEBUG("产生容器告警事件 通知类别 %d 结果 %d \n",type,!result);
    OOP_DATETIME_S_T StartTime;
    memset(&StartTime,0x00,sizeof(OOP_DATETIME_S_T));
    tmplen = strlen(dockername);
    buff[index++] = tmplen;
    memcpy(&buff[index],dockername,tmplen);
    index+=tmplen;
    DT_Time2DateTimeShort(time(NULL),&StartTime);
    memcpy_r(&buff[index],&StartTime.year,sizeof(uint16));
    index+=sizeof(uint16);
    buff[index++]=StartTime.month;
    buff[index++]=StartTime.day;
    buff[index++]=StartTime.hour;
    buff[index++]=StartTime.minute;
    buff[index++]=StartTime.second;
    buff[index++] = type;
    buff[index++] = !result;
    buff[index++] = 0x00;
    
    MSG_INFO_T *MsgSend = (MSG_INFO_T*)malloc(sizeof(MSG_INFO_T)+index);
    if(MsgSend == NULL)
    {
        return;
    }
    memset(MsgSend,0,sizeof(MSG_INFO_T)+index);
    MsgSend->MsgRPM = 1;
    MsgSend->MsgPRIORITY = 1;
    MsgSend->MsgIndex = iot_get_next_index();
    MsgSend->MsgLabel = 0;
    MsgSend->SourLen = strlen(IOT_APP_NAME_M);
    memcpy(MsgSend->SourAddr,IOT_APP_NAME_M,MsgSend->SourLen);
    MsgSend->DestLen = strlen("Broadcast");
    memcpy(MsgSend->DestAddr, "Broadcast", MsgSend->DestLen);
    MsgSend->IID = MESSAGE_IID_IOT;
    MsgSend->IOP = IOT_IOP_DOCKER_ACTION;
    MsgSend->MsgLen = index;
    memcpy(MsgSend->MsgData,buff,index);
    taskmng_plc_queue_write (&LockWarringMsg, (uint8*)MsgSend, sizeof(MSG_INFO_T)+MsgSend->MsgLen );
    //taskmng_plc_queue_write (&LockRecvMsg, (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+MsgSend->MsgLen );
    free(MsgSend);
}

/**********************************************************************
* @name      : packge_report_msg
* @brief     ：组回复报文
* @param[in] ：MSG_INFO_T *MsgRecv 消息报文 buff 回复的报文 msglen 回复报文长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void packge_report_msg(MSG_INFO_T *MsgRecv,uint8 *buff,uint16 msglen)
{

    MSG_INFO_T *MsgSend = (MSG_INFO_T*)malloc(sizeof(MSG_INFO_T)+msglen);
    memset(MsgSend,0x00,sizeof(MSG_INFO_T)+msglen);
    MsgSend->MsgRPM = 0;
    MsgSend->MsgPRIORITY = MsgRecv->MsgPRIORITY;
    MsgSend->MsgIndex = MsgRecv->MsgIndex;
    MsgSend->MsgLabel = MsgRecv->MsgLabel;
    uint32 MSGLen = 0;
    uint8 *MSGBuf= NULL;
    MsgSend->SourLen = strlen(IOT_APP_NAME_M);
    memcpy(MsgSend->SourAddr,IOT_APP_NAME_M,MsgSend->SourLen);

    MsgSend->DestLen = MsgRecv->SourLen;
    memcpy(MsgSend->DestAddr,MsgRecv->SourAddr,MsgSend->DestLen);
    MsgSend->IID = MsgRecv->IID;
    MsgSend->IOP = MsgRecv->IOP;
    MsgSend->MsgLen = msglen;
    memcpy(MsgSend->MsgData,buff,msglen);
 //   taskmng_plc_queue_write (&LockRecvMsg, (uint8*)MsgSend, sizeof(MSG_INFO_T)+MsgSend->MsgLen );
    //直接发送确认帧
    char topic[1000]={0};
    sprintf(topic,"%s/%s",MsgSend->SourAddr,MsgSend->DestAddr);
    //组消息
    MQTTMSG_FMT_DEBUG("topic %s IID %04x IOP %04x\n",topic,MsgSend->IID,MsgSend->IOP);
    MSGLen = iot_appmsg_package(MsgSend,&MSGBuf);    
    MQTTMSG_BUF_DEBUG(MSGBuf, MSGLen,"");
    mqtt_topic_write(MSGBuf, MSGLen,topic); 
}



/*******************************************************************************
* 函数名称: container_exec_0012
* 函数功能: 执行0012容器配置
* 输入参数：docker_config_T     容器配置结构体
*       
* 输出参数：
* 内部处理: 先判断容器是否执行，判断正在执行才可以进行动态配置参数
* 返 回 值: uint8                0 正确 其他错误
*******************************************************************************/

uint8 container_exec_0012(docker_config_T *dock_config)
{
    char buff[10][500]={{0}};
    uint8 ret=0;
    docker_config_T * tmpconfig = dock_config;
    char cmd[500] = {0};
    memset(cmd,0x00,sizeof(cmd));
    if(tmpconfig->dockername ==NULL)
    {
        MQTTMSG_FMT_DEBUG("docker name NULL \n");
        return 1;
    }
    sprintf(cmd,"docker ps | grep %s ",tmpconfig->dockername);
  //  IOT_FMT_DEBUG("exec cmd %s\n",cmd);
    ret = getshellresult(cmd,buff,10);
    if(ret==0)
    {
        MQTTMSG_FMT_DEBUG("命令执行失败 cmd %s \n",cmd);
        print_exec_faild(ret,buff);
        return 1;
    }
         
    memset(cmd,0x00,sizeof(cmd));
    //目前只支持一个一个数据项单独设置
#ifdef PRODUCT_CCU
    sprintf(cmd,"container config %s -cpu %d -mem %d -disk %d | grep Success",tmpconfig->dockername,tmpconfig->iot_docker_config_T.cpu_core,tmpconfig->iot_docker_config_T.mem_limitval,\
       tmpconfig->iot_docker_config_T.disk_limitval);
   // IOT_FMT_DEBUG("exec cmd %s\n",cmd);
    memset(buff,0,sizeof(buff));
   //MQTTMSG_FMT_DEBUG("执行容器安装 CMD: %s\n",cmd);
   ret = getshellresult(cmd,buff,10);
    if(ret ==0)
    {
        MQTTMSG_FMT_DEBUG("参数配置失败 CMD: %s\n",cmd);
      //  print_exec_faild(ret,buff);
        return 1;
    }
         
    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"container monitor -t cpu %d %s | grep Success",tmpconfig->iot_docker_config_T.cpu_limit/100,tmpconfig->dockername);
  //  IOT_FMT_DEBUG("exec cmd %s\n",cmd);
 // MQTTMSG_FMT_DEBUG("执行容器安装 CMD: %s\n",cmd);
    ret = getshellresult(cmd,buff,10);
//    if(ret ==0)
//    {
//        MQTTMSG_FMT_DEBUG("参数配置失败 CMD: %s\n",cmd);
//  //      print_exec_faild(ret,buff);
//        return 1;
//    }
        
    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"container monitor -t  memory %d %s | grep Success",tmpconfig->iot_docker_config_T.mem_limit/100,tmpconfig->dockername);
   // MQTTMSG_FMT_DEBUG("执行容器安装 CMD: %s\n",cmd);
   ret = getshellresult(cmd,buff,10);
//    if(ret ==0)
//    {
//        MQTTMSG_FMT_DEBUG("参数配置失败 CMD: %s\n",cmd);
// //       print_exec_faild(ret,buff);
//        return 1;
//    }
        
    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    
    sprintf(cmd,"container monitor -t storage %d %s | grep Success",tmpconfig->iot_docker_config_T.disk_limit/100,tmpconfig->dockername);
  //  MQTTMSG_FMT_DEBUG("执行容器安装 CMD: %s\n",cmd);
    ret = getshellresult(cmd,buff,10);
//    if(ret ==0)
//    {
//        MQTTMSG_FMT_DEBUG("参数配置失败 CMD: %s\n",cmd);
// //       print_exec_faild(ret,buff);
//        return 1;
//    }
#else
	sprintf(cmd,"container config %s -cpu %d -mem %d -disk %d | grep success",tmpconfig->dockername,tmpconfig->iot_docker_config_T.cpu_core,tmpconfig->iot_docker_config_T.mem_limitval,\
       tmpconfig->iot_docker_config_T.disk_limitval);
   // IOT_FMT_DEBUG("exec cmd %s\n",cmd);
    memset(buff,0,sizeof(buff));
   //MQTTMSG_FMT_DEBUG("执行容器安装 CMD: %s\n",cmd);
   ret = getshellresult(cmd,buff,10);
    if(ret ==0)
    {
        MQTTMSG_FMT_DEBUG("参数配置失败 CMD: %s\n",cmd);
      //  print_exec_faild(ret,buff);
        return 1;
    }
         
    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"container monitor -t cpu %d %s | grep success",tmpconfig->iot_docker_config_T.cpu_limit/100,tmpconfig->dockername);
  //  IOT_FMT_DEBUG("exec cmd %s\n",cmd);
 // MQTTMSG_FMT_DEBUG("执行容器安装 CMD: %s\n",cmd);
    ret = getshellresult(cmd,buff,10);
    if(ret ==0)
    {
        MQTTMSG_FMT_DEBUG("参数配置失败 CMD: %s\n",cmd);
  //      print_exec_faild(ret,buff);
        return 1;
    }
        
    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"container monitor -t  memory %d %s | grep success",tmpconfig->iot_docker_config_T.mem_limit/100,tmpconfig->dockername);
   // MQTTMSG_FMT_DEBUG("执行容器安装 CMD: %s\n",cmd);
   ret = getshellresult(cmd,buff,10);
    if(ret ==0)
    {
        MQTTMSG_FMT_DEBUG("参数配置失败 CMD: %s\n",cmd);
 //       print_exec_faild(ret,buff);
        return 1;
    }
        
    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    
    sprintf(cmd,"container monitor -t storage %d %s | grep success",tmpconfig->iot_docker_config_T.disk_limit/100,tmpconfig->dockername);
  //  MQTTMSG_FMT_DEBUG("执行容器安装 CMD: %s\n",cmd);
    ret = getshellresult(cmd,buff,10);
    if(ret ==0)
    {
        MQTTMSG_FMT_DEBUG("参数配置失败 CMD: %s\n",cmd);
 //       print_exec_faild(ret,buff);
        return 1;
    }
#endif
    
        
    /*挂载的宿主机文件夹 	array visible-string,
    应用存储文件路径      visible-string,
    映射的硬件接口     	array visible-string,
    端口资源配置参数      visible-string
    暂时不做
    */
    return 0;
}

uint16 get_containerinfo_to_out(uint8 *payload,uint16 index,uint16 j)
{
    uint16 tmp=0;
    tmp = strlen((char *)allcontainerinfo.containers[j].container.ContainerID);
    payload[index++] = tmp;
    memcpy(&payload[index],allcontainerinfo.containers[j].container.ContainerID,tmp);
    index+=tmp;
    payload[index++] = strlen((char *)allcontainerinfo.containers[j].container.ContainerName);
    memcpy(&payload[index],allcontainerinfo.containers[j].container.ContainerName,strlen((char *)allcontainerinfo.containers[j].container.ContainerName));
    index+=strlen((char *)allcontainerinfo.containers[j].container.ContainerName);
    payload[index++] = allcontainerinfo.containers[j].container.CpuCores;
    memcpy_r(&payload[index],&allcontainerinfo.containers[j].container.cpuLimit,sizeof(uint16));
    index+=sizeof(uint16);
    uint16 tmpcode = 0;
    tmpcode = atoi((char *)allcontainerinfo.containers[j].container.MemLimit);
    memcpy_r(&payload[index],&tmpcode,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&payload[index],&allcontainerinfo.containers[j].container.memLimit,sizeof(uint16));
    index+=sizeof(uint16);
    tmpcode = atoi((char *)allcontainerinfo.containers[j].container.StorageLimit);
    memcpy_r(&payload[index],&tmpcode,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&payload[index],&allcontainerinfo.containers[j].container.stroLimit,sizeof(uint16));
    index+=sizeof(uint16);
    uint8 filePathnum = strlen((char *)allcontainerinfo.containers[j].container.FilePath);
    uint8 tmpbuf[3]={0};
    uint8 tmpindex=0;
    tmpindex = set_len_offset(filePathnum, tmpbuf);
    memcpy(&payload[index],tmpbuf,tmpindex);
    index+=tmpindex;
    memcpy(&payload[index],allcontainerinfo.containers[j].container.FilePath,filePathnum);
    index+=filePathnum;
    tmp=0;
    tmp = strlen((char *)allcontainerinfo.containers[j].container.DownloadFilePath);
    payload[index++] = tmp;
    memcpy(&payload[index],allcontainerinfo.containers[j].container.DownloadFilePath,tmp);
    index+=tmp;
    uint8 Interfacenum = strlen((char *)allcontainerinfo.containers[j].container.Interface);
    payload[index++] = Interfacenum;
    memcpy(&payload[index],allcontainerinfo.containers[j].container.Interface,Interfacenum);
    index+=Interfacenum;
    tmp  = strlen((char *)allcontainerinfo.containers[j].container.PortConfig);
    payload[index++] = tmp;
    memcpy(&payload[index],allcontainerinfo.containers[j].container.PortConfig,tmp);
    index+=tmp;           //端口资源配置参数
    tmp=0;
    tmp = strlen((char *)allcontainerinfo.containers[j].container.ContainerState);
    payload[index++] = tmp; 
    memcpy(&payload[index],allcontainerinfo.containers[j].container.ContainerState,tmp);
    index+=tmp;
    memcpy_r(&payload[index],&allcontainerinfo.containers[j].container.CpuRate,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&payload[index],&allcontainerinfo.containers[j].container.MenRate,sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&payload[index],&allcontainerinfo.containers[j].container.StorageRate,sizeof(uint16));
    index+=sizeof(uint16);
    payload[index++]=4;
    payload[index++]=allcontainerinfo.containers[j].container.ContainerIP[0];
    payload[index++]=allcontainerinfo.containers[j].container.ContainerIP[1];
    payload[index++]=allcontainerinfo.containers[j].container.ContainerIP[2];
    payload[index++]=allcontainerinfo.containers[j].container.ContainerIP[3];
    tmp=0;
    tmp = strlen((char *)allcontainerinfo.containers[j].container.ImageName);
    payload[index++] = tmp; 
    memcpy(&payload[index],allcontainerinfo.containers[j].container.ImageName,tmp);
    index+=tmp;
    memcpy_r(&payload[index],&allcontainerinfo.containers[j].container.CreateTime.year,sizeof(uint16));
    index+=sizeof(uint16);
    payload[index++]=allcontainerinfo.containers[j].container.CreateTime.month;
    payload[index++]=allcontainerinfo.containers[j].container.CreateTime.day;
    payload[index++]=allcontainerinfo.containers[j].container.CreateTime.hour;
    payload[index++]=allcontainerinfo.containers[j].container.CreateTime.minute;
    payload[index++]=allcontainerinfo.containers[j].container.CreateTime.second;
    memcpy_r(&payload[index],&allcontainerinfo.containers[j].container.StartUpTime.year,sizeof(uint16));
    index+=sizeof(uint16);
    payload[index++]=allcontainerinfo.containers[j].container.StartUpTime.month;
    payload[index++]=allcontainerinfo.containers[j].container.StartUpTime.day;
    payload[index++]=allcontainerinfo.containers[j].container.StartUpTime.hour;
    payload[index++]=allcontainerinfo.containers[j].container.StartUpTime.minute;
    payload[index++]=allcontainerinfo.containers[j].container.StartUpTime.second; 
    memcpy_r(&payload[index],&allcontainerinfo.containers[j].container.RunTime,sizeof(uint32));
    index+=sizeof(uint32);
    return index;
}

uint16 packge_0013_report(SEQ_STR *con,uint8 *payload)
{
    uint16 index = 0,j=0;
 //   pthread_mutex_lock(&allcontainerinfo.lock);
    if(allcontainerinfo.count==0)
    {
      //  pthread_mutex_unlock(&allcontainerinfo.lock);
        payload[index++]=0;
        return index;
    }
    else
    {
        if(con->len == 0)
        {
            payload[index++]=allcontainerinfo.count;
        }
        else
        {
            //payload[index++]=1;
        }
    }
    for(j=0;j<allcontainerinfo.count;j++)
    {
        if(con->len == 0)
        {
            //全部容器
               index = get_containerinfo_to_out(payload,index,j);
        }
        else
        {
            if(strcmp((char *)con->buff,(char *)allcontainerinfo.containers[j].container.ContainerName)==0)
            {
                payload[index++]=1;
                index =  get_containerinfo_to_out(payload,index,j);
                break;
            } 
        }
      
    }
  //  pthread_mutex_unlock(&allcontainerinfo.lock);
    return index;
}


/**********************************************************************
* @name      : docker_exec_0013
* @brief     ：容器查询消息执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息报文 int MSGLen 大小
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/
void docker_exec_0013(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 payload[2048]={0};  
    uint16 index=0;
    SEQ_STR dockername;
    memset(&dockername,0,sizeof(SEQ_STR));

    do{
        if(MsgRecv->MsgData[index++]==1)
        {
            dockername.len =  MsgRecv->MsgData[index++];
            memcpy(dockername.buff,&MsgRecv->MsgData[index],dockername.len);
            index+= dockername.len;
        }
        if(MsgRecv->MsgLen!=index)
        {
            MQTTMSG_FMT_DEBUG("index %d msglen %d \n",index,MsgRecv->MsgLen);
            payload[0] = 0;
            break;
        }
        index = packge_0013_report(&dockername,payload);
    }while(0);
             
    packge_report_msg(MsgRecv,payload,index); 
}


/**********************************************************************
* @name      : docker_exec_0012
* @brief     ：容器配置消息执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息报文 int MSGLen 大小
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0012(MSG_INFO_T *MsgRecv,int MSGLen)
{
    //uint8 payload[2048];
    uint16 index=0;
    uint8 filenum=0,i=0,ret=0;
    docker_config_T docker_config;
    BOOL result = TRUE;
    uint8 result_buf[500]={0};
    memset(&docker_config,0x00,sizeof(docker_config_T));
    //memcpy(payload,MsgRecv->MsgData,MsgRecv->MsgLen);
    docker_config.dockernamelen = MsgRecv->MsgData[index++];
    memcpy(docker_config.dockername,&MsgRecv->MsgData[index],docker_config.dockernamelen);
    index+=docker_config.dockernamelen;
    docker_config.iot_docker_config_T.cpu_core = MsgRecv->MsgData[index++];
    memcpy_r(&docker_config.iot_docker_config_T.cpu_limit,&MsgRecv->MsgData[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&docker_config.iot_docker_config_T.mem_limitval,&MsgRecv->MsgData[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&docker_config.iot_docker_config_T.mem_limit,&MsgRecv->MsgData[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&docker_config.iot_docker_config_T.disk_limitval,&MsgRecv->MsgData[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&docker_config.iot_docker_config_T.disk_limit,&MsgRecv->MsgData[index],sizeof(uint16));
    index+=sizeof(uint16);

    IOT_FMT_DEBUG("monitor container config CPU %d MEM %d DISK %d \n",docker_config.iot_docker_config_T.cpu_limit,docker_config.iot_docker_config_T.mem_limit,docker_config.iot_docker_config_T.disk_limit);
    filenum = MsgRecv->MsgData[index++];
    docker_config.iot_docker_config_T.host_folder = filenum;
    for(i=0;i<filenum;i++)
    {
        docker_config.iot_docker_config_T.host_info[i].len = MsgRecv->MsgData[index++];
        memcpy(docker_config.iot_docker_config_T.host_info[i].buff,&MsgRecv->MsgData[index],docker_config.iot_docker_config_T.host_info[i].len);
        index+=docker_config.iot_docker_config_T.host_info[i].len;
    }
    docker_config.iot_docker_config_T.strogefilelen = MsgRecv->MsgData[index++];
    memcpy(docker_config.iot_docker_config_T.stroge_file,&MsgRecv->MsgData[index],docker_config.iot_docker_config_T.strogefilelen);
    index+=docker_config.iot_docker_config_T.strogefilelen;
    filenum = MsgRecv->MsgData[index++];
    docker_config.iot_docker_config_T.dev_infolen = filenum;
    for(i=0;i<filenum;i++)
    {
        docker_config.iot_docker_config_T.dev_info[i].len = MsgRecv->MsgData[index++];
        memcpy(docker_config.iot_docker_config_T.dev_info[i].buff,&MsgRecv->MsgData[index],docker_config.iot_docker_config_T.dev_info[i].len);
        index+=docker_config.iot_docker_config_T.dev_info[i].len;
    }
    docker_config.iot_docker_config_T.port_para = MsgRecv->MsgData[index++];
    memcpy(docker_config.iot_docker_config_T.port_val,&MsgRecv->MsgData[index],docker_config.iot_docker_config_T.port_para);
    ret = container_exec_0012(&docker_config);
    if(ret != 0)
    {
        result = FALSE;
    }
    result_buf[0] = result;
    packge_report_msg(MsgRecv,result_buf,1);
    sleep(2);
    packge_docker_notice_0001(5,result,(char*)docker_config.dockername,docker_config.dockernamelen);
}

/**********************************************************************
* @name      : docker_exec_0011
* @brief     ：容器操作消息执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息报文 int MSGLen 大小
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0011(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 payload[2048];
    uint16 index=0;
    char cmd[256] = {0};
    uint16 tmplen = 0;
    uint8 dockername[100]={0};
    uint8 type = 0 ;
    uint8 msg_type = 0;
    char buff[5][500]={{0}};
    BOOL result = TRUE;
    uint8 code[128] = {0};
    memcpy(payload,MsgRecv->MsgData,MsgRecv->MsgLen);
    //总长度
    tmplen = payload[index++];
    memcpy(dockername,&payload[index],tmplen);
    index+=tmplen;
    type = payload[index++];
    if(tmplen == 0)  //没有容器名
    {
        MQTTMSG_FMT_DEBUG("消息错误，没有容器名\n");
        result = FALSE;
    }
#ifdef PRODUCT_CCU
    switch(type)
    {
        case 0:                                //启动容器
        {
            
            msg_type = 3;
            MQTTMSG_FMT_DEBUG("收到命令：启动容器 %s\n",dockername);
            sprintf(cmd,"container start %s | grep SUCCESS",dockername);
            if(getshellresult(cmd,buff,5)!=1)
            {
                result = FALSE;
            }                  
            MQTTMSG_FMT_DEBUG("启动容器成功\n",dockername);
        }break;
        case 1:                             //停止容器
        {
            msg_type = 4;
            MQTTMSG_FMT_DEBUG("收到命令：停止容器 %s\n",dockername);
            sprintf(cmd,"container stop %s | grep SUCCESS",dockername);
            if(getshellresult(cmd,buff,5)!=1)
            {
                result = FALSE;
            }
            MQTTMSG_FMT_DEBUG("停止容器成功\n");
        }break;
        case 2:                         //卸载容器 先停止后卸载 停止已经停止的容器是允许的
        {
            msg_type = 2;
            MQTTMSG_FMT_DEBUG("收到命令：卸载容器 %s\n",dockername);
            sprintf(cmd,"container uninstall %s | grep SUCCESS",dockername);
            if(getshellresult(cmd,buff,5)!=1)
            {
                result = FALSE;
            }
            MQTTMSG_FMT_DEBUG("收到命令：卸载容器成功 %s\n",dockername);
        }break;
        default:
            break;
    } 
#else
	switch(type)
    {
        case 0:                                //启动容器
        {
            
            msg_type = 3;
            MQTTMSG_FMT_DEBUG("收到命令：启动容器 %s\n",dockername);
            sprintf(cmd,"container start %s | grep success",dockername);
            if(getshellresult(cmd,buff,5)!=1)
            {
                result = FALSE;
            }                  
            MQTTMSG_FMT_DEBUG("启动容器成功\n",dockername);
        }break;
        case 1:                             //停止容器
        {
            msg_type = 4;
            MQTTMSG_FMT_DEBUG("收到命令：停止容器 %s\n",dockername);
            sprintf(cmd,"container stop %s | grep success",dockername);
            if(getshellresult(cmd,buff,5)!=1)
            {
                result = FALSE;
            }
            MQTTMSG_FMT_DEBUG("停止容器成功\n");
        }break;
        case 2:                         //卸载容器 先停止后卸载 停止已经停止的容器是允许的
        {
            msg_type = 2;
            MQTTMSG_FMT_DEBUG("收到命令：卸载容器 %s\n",dockername);
            sprintf(cmd,"container uninstall %s | grep success",dockername);
            if(getshellresult(cmd,buff,5)!=2)
            {
                result = FALSE;
            }
            MQTTMSG_FMT_DEBUG("收到命令：卸载容器成功 %s\n",dockername);
        }break;
        default:
            break;
    } 
#endif
     code[0]=result;
     packge_report_msg(MsgRecv,code,1);
     sleep(2);
     packge_docker_notice_0001(msg_type,result,(char *)dockername,tmplen);
}


/**********************************************************************
* @name      : docker_exec_0010
* @brief     ：容器安装消息执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息报文 int MSGLen 大小
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0010(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    uint8 AppSec=0,AppInfoSec =0;
    uint8 AppSigSec=0,i=0,ret = 0;
    uint16 index=0,tmplen=0;
    BOOL result = TRUE;
    SEQ_STR app;
    container_image con_image;
    docker_config_T con_info;
    app_info appinfo;
    char cmd[1024]={0};
    char pathname[500]={0};
    char buff[20][500]={0};
    memset(&appinfo,0,sizeof(appinfo));
    memset(&con_info,0,sizeof(docker_config_T));
    memset(&con_image,0,sizeof(container_image));
    memset(&app,0,sizeof(SEQ_STR));
    con_info.dockernamelen  = data[index++];
    memcpy(con_info.dockername,&data[index],con_info.dockernamelen);
    index+=con_info.dockernamelen;
    AppSec = data[index++];
    if(AppSec==1) //有app
    {
        app.len = data[index++];
        memcpy(app.buff,&data[index],app.len);
        index+=app.len;
    }
    if(data[index++] == 1) //有容器镜像
    {
        con_image.ImageNamelen = data[index++];
        memcpy(con_image.ImageName,&data[index],con_image.ImageNamelen);
        index+=con_image.ImageNamelen;
        con_image.ImageSavelen = data[index++];
        memcpy(con_image.ImageSave,&data[index],con_image.ImageSavelen);
        index+=con_image.ImageSavelen;
    }
    con_info.iot_docker_config_T.cpu_core = data[index++];
    memcpy_r(&con_info.iot_docker_config_T.cpu_limit,&data[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&con_info.iot_docker_config_T.mem_limitval,&data[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&con_info.iot_docker_config_T.mem_limit,&data[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&con_info.iot_docker_config_T.disk_limitval,&data[index],sizeof(uint16));
    index+=sizeof(uint16);
    memcpy_r(&con_info.iot_docker_config_T.disk_limit,&data[index],sizeof(uint16));
    index+=sizeof(uint16);
    con_info.iot_docker_config_T.host_folder = data[index++];
    for(i=0;i<con_info.iot_docker_config_T.host_folder;i++)
    {
        con_info.iot_docker_config_T.host_info[i].len = data[index++];
        memcpy(con_info.iot_docker_config_T.host_info[i].buff,&data[index],con_info.iot_docker_config_T.host_info[i].len);
        index+=con_info.iot_docker_config_T.host_info[i].len;
    }
    con_info.iot_docker_config_T.strogefilelen = data[index++]; //存储文件夹
    memcpy(con_info.iot_docker_config_T.stroge_file,&data[index],con_info.iot_docker_config_T.strogefilelen);
    index+=con_info.iot_docker_config_T.strogefilelen;
    con_info.iot_docker_config_T.dev_infolen = data[index++];
    for(i=0;i<con_info.iot_docker_config_T.dev_infolen;i++)
    {
        con_info.iot_docker_config_T.dev_info[i].len = data[index++];
        memcpy(con_info.iot_docker_config_T.dev_info[i].buff,&data[index],con_info.iot_docker_config_T.dev_info[i].len);
        index+=con_info.iot_docker_config_T.dev_info[i].len;
    }
    con_info.iot_docker_config_T.port_para = data[index++];
    memcpy(con_info.iot_docker_config_T.port_val,&data[index],con_info.iot_docker_config_T.port_para);
    index+=con_info.iot_docker_config_T.port_para;
    AppInfoSec = data[index++];
    if(AppInfoSec==1)
    {
        tmplen = data[index++];
        memcpy(appinfo.app_version,&data[index],tmplen);
        index+=tmplen;
        tmplen = data[index++];
        memcpy(appinfo.file_info_t.filename,&data[index],tmplen);
        index+=tmplen;
        tmplen = data[index++];
        memcpy(appinfo.file_info_t.filetype,&data[index],tmplen);
        index+=tmplen;
        tmplen = data[index++];
        memcpy(appinfo.file_info_t.filepath,&data[index],tmplen);
        index+=tmplen;
        memcpy_r(&appinfo.file_info_t.filesize,&data[index],sizeof(uint32));
        index+=sizeof(uint32);
        tmplen = data[index++];
        memcpy(appinfo.file_info_t.filemd5,&data[index],tmplen);
        index+=tmplen;
        AppSigSec = data[index++];
        if(AppSigSec==1)
        {
            tmplen = data[index++];
            memcpy(appinfo.file_info_t.signinfo_t.signname,&data[index],tmplen);
            index+=tmplen;
            tmplen = data[index++];
            memcpy(appinfo.file_info_t.signinfo_t.signpath,&data[index],tmplen);
            index+=tmplen;
            tmplen = data[index++];
            memcpy_r(&appinfo.file_info_t.signinfo_t.signsize,&data[index],sizeof(uint32));
            index+=sizeof(uint32);
            memcpy(appinfo.file_info_t.signinfo_t.signmd5,&data[index],tmplen);
            index+=tmplen;
        }
        appinfo.app_config_t.cpucore = data[index++];
        memcpy_r(&appinfo.app_config_t.cpuLimit,&data[index],sizeof(uint16));
        index+=sizeof(uint16);
        memcpy_r(&appinfo.app_config_t.mem_limit,&data[index],sizeof(uint16));
        index+=sizeof(uint16);
        memcpy_r(&appinfo.app_config_t.memLimit,&data[index],sizeof(uint16));
        index+=sizeof(uint16);
    }    
    do{
        if(index!=msglen)   //报文正确性判断
        {
            MQTTMSG_FMT_DEBUG("index %d msglen %d \n",index,msglen);
            result = FALSE;
            break;
        }
        //先判断终端内是否有这个容器，如果有先卸载
        sprintf(cmd,"docker ps | grep %s",con_info.dockername);
        ret = getshellresult(cmd,buff,20);
        memset(cmd,0,sizeof(cmd));
        memset(buff,0,sizeof(buff));
        if(ret==1)
        {
            MQTTMSG_FMT_DEBUG("检查到有同名容器先卸载\n");
            sprintf(cmd,"container uninstall %s | grep failed",con_info.dockername);
            ret = getshellresult(cmd,buff,20);
            {
                if (ret !=0 )
                {
                    MQTTMSG_FMT_DEBUG("容器卸载失败 CMD: %s\n",cmd);
                    result = FALSE;
                    break;
                }
            }
        }

       // sprintf(cmd,"container uninstall %s",con_info.dockername);

        
        /*确定文件是否存在*/
        /*判断app文件是否存在*/

        if(AppInfoSec == 1 )    //app文件有效和md5值判断
        {
            sprintf(pathname,"%s/%s",appinfo.file_info_t.filepath,appinfo.file_info_t.filename);
            if(access(pathname,R_OK)!=0)
            {
                MQTTMSG_FMT_DEBUG("路径不存在 %s\n",pathname);
                result = FALSE;
                break;
            }
            ret = sizeAndmd5_judge(pathname,appinfo.file_info_t.signinfo_t.signsize,(char *)appinfo.file_info_t.signinfo_t.signmd5);
            if(ret != 0)
            {
                result = FALSE;
                break;
            }
            if(AppSec !=1)
            {
                
            }
            if(AppSec == 1)
            {

            }
            memset(pathname,0,sizeof(pathname));
            memset(cmd,0,sizeof(cmd));
            sprintf(pathname,"%s/%s",appinfo.file_info_t.filepath,appinfo.file_info_t.filename);
#ifdef PRODUCT_CCU
            sprintf(cmd,"container install %s %s -v /lib/hal_lib:/lib/hal_lib -v /data/app:/data/app -v /dev:/dev -v /tmp/dev:/tmp/dev -v /tmp/data:/tmp/data -v /data/devinfo:/data/devinfo -v /sys:/sys ",con_info.dockername,pathname);            
#else
            sprintf(cmd,"container install %s %s -v /lib/hal_lib:/lib/hal_lib -v /data/app:/data/app -v /dev:/dev -v /tmp/dev:/tmp/dev -v /tmp/data:/tmp/data -v /data/devinfo:/data/devinfo -v /sys:/sys -dev-dir",con_info.dockername,pathname);
#endif
            if(AppSigSec==1)   //签名文件有效和md5值判断
            {
                memset(pathname,0,sizeof(pathname));
                sprintf(pathname,"%s/%s",appinfo.file_info_t.signinfo_t.signpath,appinfo.file_info_t.signinfo_t.signname);
                if(access(pathname,R_OK)!=0)
                {
                    result = FALSE;
                    break;
                }
                else
                {
                    ret = sizeAndmd5_judge(pathname,appinfo.file_info_t.signinfo_t.signsize,(char *)appinfo.file_info_t.signinfo_t.signmd5);
                    if(ret != 0)
                    {
                        result = FALSE;
                        break;
                    } 
                }   
            }
        }
        else
        {          
            memset(pathname,0,sizeof(pathname));
#ifdef PRODUCT_CCU

            sprintf(cmd,"container install %s -v /data/app:/data/app -v /lib/hal_lib:/lib/hal_lib -v /data/devinfo:/data/devinfo -v /dev:/dev ",con_info.dockername);
#else
            sprintf(cmd,"container install %s -v /data/app:/data/app -v /lib/hal_lib:/lib/hal_lib -v /data/devinfo:/data/devinfo -v /dev:/dev -dev-dir",con_info.dockername);
#endif
        }
        /*组命令*/
        uint16 offset= strlen(cmd);
        for(i=0;i<con_info.iot_docker_config_T.host_folder;i++)
        {
            if(access((char *)con_info.iot_docker_config_T.host_info[i].buff,R_OK)==0)
            {
                sprintf(cmd+offset,"-v %s:%s ",con_info.iot_docker_config_T.host_info[i].buff,con_info.iot_docker_config_T.host_info[i].buff);
            }          
        }
        offset= strlen(cmd);
        if(access((char *)con_info.iot_docker_config_T.stroge_file,R_OK)==0)
        {
            sprintf(cmd+offset,"-v %s:%s ",con_info.iot_docker_config_T.stroge_file,con_info.iot_docker_config_T.stroge_file);
        }
        offset= strlen(cmd);
        for(i=0;i<con_info.iot_docker_config_T.dev_infolen;i++)
        {           
            if(access((char *)con_info.iot_docker_config_T.dev_info[i].buff,R_OK)==0)
            {
                offset= strlen(cmd);
                sprintf(cmd+offset,"-dev %s ",con_info.iot_docker_config_T.dev_info[i].buff);
            }          
        }
        offset= strlen(cmd);
        if(con_info.iot_docker_config_T.cpu_core<2 || con_info.iot_docker_config_T.cpu_core>4)
            con_info.iot_docker_config_T.cpu_core=4;
        if(strlen((char*)con_info.iot_docker_config_T.port_val)!=0&&strlen((char *)con_info.iot_docker_config_T.port_val)!=0)
        {
            sprintf(cmd+offset," -p %d:%d -dev-dir",atoi((char *)con_info.iot_docker_config_T.port_val),atoi((char *)con_info.iot_docker_config_T.port_val));
        }
        offset =strlen(cmd);
#ifdef PRODUCT_CCU
        sprintf(cmd+offset," | grep SUCCESS");
#else
		sprintf(cmd+offset," | grep success");
#endif

       // MQTTMSG_FMT_DEBUG("执行容器安装 CMD: %s\n",cmd);
        ret = getshellresult(cmd,buff,20);
        {
            MQTTMSG_FMT_DEBUG("执行容器安装命令 CMD: %s\n",cmd);
            if(ret < 1)
            {
                MQTTMSG_FMT_DEBUG("执行容器安装失败 ret %d CMD: %s\n", ret,cmd);
                //打印失败报文
                print_exec_faild(ret,buff);
                result = FALSE;
                break;
            }
        }
        /*验证*/
        memset(cmd,0,sizeof(cmd));
        memset(buff,0,sizeof(buff));
        sprintf(cmd,"docker ps | grep %s",con_info.dockername);
        if(getshellresult(cmd,buff,20)<1)
        {
            MQTTMSG_FMT_DEBUG("复检容器安装失败 \n");
            result = FALSE;
            break; 
        }
        /*设置APP阈值(option)*/
        if(AppSec==1 && AppInfoSec==1)
        {
            //存在app信息的app名字要用文件名字去掉.tar
            memset(cmd,0,sizeof(cmd));
            memset(buff,0,sizeof(buff));
            char name[100]={0};
            char*p=NULL;
            p=strstr(appinfo.file_info_t.filename,".tar");
#ifdef PRODUCT_CCU
        	if(p!=NULL)
            {
                memcpy(name,appinfo.file_info_t.filename,p-appinfo.file_info_t.filename);
                sprintf(cmd,"appm -t -cpu %d -mem %d -c %s -n %s | grep SUCCESS",appinfo.app_config_t.cpuLimit/100,appinfo.app_config_t.memLimit/100,con_info.dockername,name);

            }
            else
            {
                sprintf(cmd,"appm -t -cpu %d -mem %d -c %s -n %s | grep SUCCESS",appinfo.app_config_t.cpuLimit/100,appinfo.app_config_t.memLimit/100,con_info.dockername,app.buff);
            }
#else
			if(p!=NULL)
            {
                memcpy(name,appinfo.file_info_t.filename,p-appinfo.file_info_t.filename);
                sprintf(cmd,"appm -t -cpu %d -mem %d -c %s -n %s | grep success",appinfo.app_config_t.cpuLimit/100,appinfo.app_config_t.memLimit/100,con_info.dockername,name);

            }
            else
            {
                sprintf(cmd,"appm -t -cpu %d -mem %d -c %s -n %s | grep success",appinfo.app_config_t.cpuLimit/100,appinfo.app_config_t.memLimit/100,con_info.dockername,app.buff);
            }
#endif

        //    MQTTMSG_FMT_DEBUG("执行容器安装 CMD: %s\n",cmd);
            ret = getshellresult(cmd,buff,20);
//            if (ret == 0)
//            {  
//                MQTTMSG_FMT_DEBUG("参数配置失败 CMD: %s\n",cmd);
//                print_exec_faild(ret,buff);
//                result = FALSE;
//                break;
//            }
        }
        memset(cmd,0,sizeof(cmd));
        memset(buff,0,sizeof(buff));
        //sprintf(cmd,"appm -t -cpu %d -mem %d -c %s -n %s",appinfo.app_config_t.cpuLimit,appinfo.app_config_t.memLimit,con_info.dockername,name);
        ret = container_exec_0012(&con_info);
        if(ret != 0)
        {
            result = FALSE;
            break;
        }
    }while(0);
    /*执行安装命令*/
    /*配置参数（容器和app）*/
    /*返回报文和操作告警*/
    uint8 result_buf[100]={0};
    result_buf[0]=result;
    packge_report_msg(MsgRecv,result_buf,1);
    sleep(2);
    packge_docker_notice_0001(0,result,(char*)con_info.dockername,con_info.dockernamelen);
}

/**********************************************************************
* @name      : sizeAndmd5_judge
* @brief     ：判断文件的大小和md5值是否正确
* @param[in] filepath  文件绝对路径 size 文件大小 md5 md5值
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/
uint8 sizeAndmd5_judge(char *filepath,uint32 size,char* md5)
{
    if(size == 0 || md5 ==NULL)
        return 0;
    if(filepath == NULL)
        return 1;
    char cmd[500]={0};
    char buff[2][500];
    uint8 bit_str[100]= {0};
    int bitlen = 0;
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"wc -c %s | awk '{print $1}'",filepath);
    if(getshellresult(cmd,buff,1)!=1)
    {
        MQTTMSG_FMT_DEBUG("命令执行失败 ：%s \n",cmd);
        return 1; 
    }
          
    if(atoi(buff[0])!=size)
    {
      MQTTMSG_FMT_DEBUG("大小不一致 wc %d size %d\n",atoi(buff[0]),size);
      return 1;  
    }
    memset(buff,0,sizeof(buff));
    memset(cmd,0,sizeof(cmd));
    sprintf(cmd,"md5sum %s | awk '{print $1}'",filepath);
    if(getshellresult(cmd,buff,1)!=1)
        return 1;
    MQTTMSG_BUF_DEBUG(buff[0],strlen(buff[0]),"md5sum value");
    MQTTMSG_BUF_DEBUG(md5,strlen(md5),"md5 value");
 //   bitlen = strt2octet(buff[0],strlen(buff[0]),bit_str,64);
 //   if((bitlen<0) || (bitlen >64))
 //   {
 //       MQTTMSG_FMT_DEBUG("转化失败 bufflen %d \n",bitlen);
  //      return 1;
 //   }
  //  MQTTMSG_BUF_DEBUG(bit_str,bitlen,"bit_str",);
  //  MQTTMSG_BUF_DEBUG(md5,bitlen,"md5 ");
    if(memcmp(bit_str,md5,bitlen)!=0)
        return 1;
    return 0;
}

/**********************************************************************
* @name      : docker_exec_0014
* @brief     ：消息0014App安装命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0014(IOT_APPDATA_T *pdata,MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint16 index=0;
    uint8 option = 0,ret = 0,exectype = 0;//0为安装
    uint16 tmplen = 0;
    BOOL result = TRUE;
    iot_mqtt_0014 app_install;
    char pathname[2048]={0};
    char buff[20][500];
    memset(buff,0,sizeof(buff));
    memset(&app_install,0,sizeof(iot_mqtt_0014));
    app_install.docker.len = MsgRecv->MsgData[index++];
    memcpy(app_install.docker.buff,&MsgRecv->MsgData[index],app_install.docker.len);
    index +=app_install.docker.len;
    
    app_install.app.len = MsgRecv->MsgData[index++];
    memcpy(app_install.app.buff,&MsgRecv->MsgData[index],app_install.app.len);
    index +=app_install.app.len;
    tmplen = MsgRecv->MsgData[index++];     //应用版本号
    memcpy(app_install.app_config_t.app_version,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    tmplen = MsgRecv->MsgData[index++];    //文件的名字
    memcpy(app_install.app_config_t.file_info_t.filename,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    tmplen = MsgRecv->MsgData[index++];    //文件的类型
    memcpy(app_install.app_config_t.file_info_t.filetype,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    tmplen = MsgRecv->MsgData[index++];    //文件路径
    memcpy(app_install.app_config_t.file_info_t.filepath,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    memcpy_r(&app_install.app_config_t.file_info_t.filesize,&MsgRecv->MsgData[index],sizeof(uint32));
    index +=sizeof(uint32);                //文件大小
    tmplen = MsgRecv->MsgData[index++];    //文件的 md5 值 
    memcpy(app_install.app_config_t.file_info_t.filemd5,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    option = MsgRecv->MsgData[index++];
    //文件的数字签证信息  OPTIONAL
    if(option == 1)
    {
        tmplen = MsgRecv->MsgData[index++];    //数字证书文件名字
        memcpy(app_install.app_config_t.file_info_t.signinfo_t.signname,&MsgRecv->MsgData[index],tmplen);
        index +=tmplen;
        tmplen = MsgRecv->MsgData[index++];    //数字证书文件路径
        memcpy(app_install.app_config_t.file_info_t.signinfo_t.signpath,&MsgRecv->MsgData[index],tmplen);
        index +=tmplen;
        memcpy_r(&app_install.app_config_t.file_info_t.signinfo_t.signsize,&MsgRecv->MsgData[index],sizeof(uint32));
        index +=sizeof(uint32); 
        tmplen = MsgRecv->MsgData[index++];    //数字证书文件的md5值
        memcpy(app_install.app_config_t.file_info_t.signinfo_t.signmd5,&MsgRecv->MsgData[index],tmplen);
        index +=tmplen;
    }
    app_install.app_config_t.app_config_t.cpucore = MsgRecv->MsgData[index++];
    memcpy_r(&app_install.app_config_t.app_config_t.cpuLimit,&MsgRecv->MsgData[index],sizeof(uint16));
    index +=sizeof(uint16);
    memcpy_r(&app_install.app_config_t.app_config_t.mem_limit,&MsgRecv->MsgData[index],sizeof(uint16));
    index +=sizeof(uint16);
    memcpy_r(&app_install.app_config_t.app_config_t.memLimit,&MsgRecv->MsgData[index],sizeof(uint16));
    index +=sizeof(uint16);
   do{
        if(option == 1)
        {
            //校验一下数字签名文件是否存在
            sprintf(pathname,"%s%s",app_install.app_config_t.file_info_t.signinfo_t.signpath,app_install.app_config_t.file_info_t.signinfo_t.signname);
            if(access(pathname,R_OK)!=0)
            {
                result = FALSE;
                break;
            }
            else
            {
                ret = sizeAndmd5_judge(pathname,app_install.app_config_t.file_info_t.signinfo_t.signsize,(char *)app_install.app_config_t.file_info_t.signinfo_t.signmd5);
                if(ret != 0)
                {
                    result = FALSE;
                    break;
                } 
            }       
            
        }

        /*判断升级文件存不存在*/
        memset(pathname,0,sizeof(pathname));
        sprintf(pathname,"%s/%s",app_install.app_config_t.file_info_t.filepath,app_install.app_config_t.file_info_t.filename);
        if(access(pathname,R_OK)!=0)
        {
            MQTTMSG_FMT_DEBUG("检查文件是否存在 CMD -- ",pathname);
            result = FALSE;
            break;
        }
        ret = sizeAndmd5_judge(pathname,app_install.app_config_t.file_info_t.filesize,(char *)app_install.app_config_t.file_info_t.filemd5);
        if(ret != 0)
        {
             result = FALSE;
             break;
        }
        char filePath[255] = {0};
        memset(buff,0,sizeof(buff));
        //先判断当前终端是否存在该容器和App，如果存在视为升级，否则视为安装
        sprintf(filePath,"appm -I -c %s -n %s",app_install.docker.buff,app_install.app.buff); 
        ret = getshellresult(filePath,buff,20);
        if(ret > 1)
        {
            MQTTMSG_FMT_DEBUG("存在同名app，视为升级\n");
            exectype = 1;
        }
        if(exectype == 1)
        {
            uint8 tmpbuff[10]={0};
            tmpbuff[0]=1;
            packge_report_msg(MsgRecv,tmpbuff,1);
//            sleep(3);
//            app_action_notice(&app_install.docker,&app_install.app,1,result); 
//			sleep(2);
            char cmd[100] = {0};
            memset(buff,0,sizeof(buff));
            sprintf(cmd,"appm -S -c %s -n %s ",app_install.docker.buff,app_install.app.buff);
            MQTTMSG_FMT_DEBUG("升级 %s\n",cmd);
           // getshellresult(cmd, char result [ ] [ 500 , int size)
           system(cmd);
           memset(cmd,0,sizeof(cmd));
           sprintf(cmd,"appm -u -c %s -n %s ",app_install.docker.buff,app_install.app.buff);
           MQTTMSG_FMT_DEBUG("升级 %s\n",cmd);
           system(cmd);
#ifdef PRODUCT_CCU
		   memset(cmd,0,sizeof(cmd));
		   sprintf(cmd, "rm -rf /backup/app_back/*");
		   system(cmd);
		   system("sync");
#endif

        }
         //用指令安装app
        //appm -i -cpu 4 -mem 50  -t -cpu 70 -t -mem 80 -c c_master_yx -n puAmr -f puAmr.tar
        memset(filePath,0,sizeof(filePath));
        sprintf(filePath,"%s%s",app_install.app_config_t.file_info_t.filepath,app_install.app_config_t.file_info_t.filename); 
        memset(pathname,0,sizeof(pathname));
        memset(buff,0,sizeof(buff));

#ifdef PRODUCT_CCU
		if(exectype == 0)
        	sprintf(pathname,"appm -i -cpu %d -mem %d -c %s -n %s -f %s | grep Succeed",app_install.app_config_t.app_config_t.cpucore,app_install.app_config_t.app_config_t.mem_limit, app_install.docker.buff,app_install.app.buff,filePath);
        else
        	sprintf(pathname,"appm -i -c %s -n %s -f %s | grep Succeed", app_install.docker.buff,app_install.app.buff,filePath);
        MQTTMSG_FMT_DEBUG("执行安装命令 CMD -- %s\n",pathname);
        uint8 tmp_num = 0;
        do{
            memset(buff,0,sizeof(buff));
            ret = getshellresult(pathname,buff,10);
            if (ret == 0)
            {
                MQTTMSG_FMT_DEBUG("失败重试 %d \n",tmp_num);
               // print_exec_faild(ret,buff);
                tmp_num++;
                sleep(1);
            }
            else
            {
                break;
            }
        }while(tmp_num<3);
        if(tmp_num==3)
        {
            result = FALSE;
            break;
        }
        if(exectype == 0)  //不设置参数，把参数存起来
        {
            memset(buff,0,sizeof(buff));
            memset(pathname,0,sizeof(pathname));
            sprintf(pathname,"appm -t -cpu %d -mem %d -c %s -n %s",app_install.app_config_t.app_config_t.cpuLimit,app_install.app_config_t.app_config_t.memLimit,app_install.docker.buff,app_install.app.buff);
            uint8 num_T=0;
            do{
                ret = getshellresult(pathname,buff,3);
                {
                    MQTTMSG_FMT_DEBUG("执行参数设置命令 CMD -- %s \n",pathname);
                    if(ret != 0)
                    {
                        MQTTMSG_FMT_DEBUG("参数设置失败 \n");
                        print_exec_faild(ret,buff);
                        sleep(1);
                        num_T++;
                    }
                    else
                    {
                        break;
                    }
                }
            }while(num_T<3);
            if(num_T==3)
            {          
                MQTTMSG_FMT_DEBUG("参数设置失败 \n");
                result = FALSE;
                break;
            }
            ret = getshellresult(pathname,buff,3);
            {
                MQTTMSG_FMT_DEBUG("执行参数设置命令 CMD -- %s \n",pathname);
                if(ret != 0)
                {
                    MQTTMSG_FMT_DEBUG("参数设置失败 \n");
                    print_exec_faild(ret,buff);
                    
                    result = FALSE;
                    break;
                }
            }
        }
#else
		if(exectype == 0)
        	sprintf(pathname,"appm -i -cpu %d -mem %d -c %s -n %s -f %s | grep success",app_install.app_config_t.app_config_t.cpucore,app_install.app_config_t.app_config_t.mem_limit, app_install.docker.buff,app_install.app.buff,filePath);
        else
        	sprintf(pathname,"appm -i -c %s -n %s -f %s | grep success", app_install.docker.buff,app_install.app.buff,filePath);
        MQTTMSG_FMT_DEBUG("执行安装命令 CMD -- %s\n",pathname);
        ret = getshellresult(pathname,buff,10);
        if (ret == 0)
        {
            MQTTMSG_FMT_DEBUG("安装失败 \n");
           // print_exec_faild(ret,buff);
            result = FALSE;
            break;
        }
        if(exectype == 0)  //不设置参数，把参数存起来
        {

            
            memset(buff,0,sizeof(buff));
            memset(pathname,0,sizeof(pathname));
            sprintf(pathname,"appm -t -cpu %d -mem %d -c %s -n %s | grep success",app_install.app_config_t.app_config_t.cpuLimit,app_install.app_config_t.app_config_t.memLimit,app_install.docker.buff,app_install.app.buff);
            ret = getshellresult(pathname,buff,3);
            {
                MQTTMSG_FMT_DEBUG("执行参数设置命令 CMD -- %s \n",pathname);
                if(ret == 0)
                {
                    MQTTMSG_FMT_DEBUG("参数设置失败 \n");
                    print_exec_faild(ret,buff);
                    result = FALSE;
                    break;
                }
            }
        }
#endif

        //重新校对一下安装是否正确
        //appm -I -c container -n appname
        memset(pathname,0,sizeof(pathname));
        memset(buff,0,sizeof(buff));
        sprintf(pathname,"appm -I -c %s -n %s",app_install.docker.buff,app_install.app.buff);     
        ret = getshellresult(pathname,buff,10);
        {
            MQTTMSG_FMT_DEBUG("检查容器安装是否成功 CMD -- %s \n",pathname);
            if (ret < 1)
            {
                result = FALSE;
                MQTTMSG_FMT_DEBUG("容器安装失败\n");
            }
            
            
        }      
    }while(0);
    if(exectype == 0)
    {
        uint8 tmpbuff[10]={0};
        tmpbuff[0]=result;
        packge_report_msg(MsgRecv,tmpbuff,1);
        sleep(2);
		app_action_notice(&app_install.docker,&app_install.app,exectype,result); //安装消息通知
    }
    else
    {
        pdata->app_u.type_u = 1;
        pdata->app_u.result = result;
        pdata->app_u.appname = app_install.app;
        pdata->app_u.conname = app_install.docker;
       // app_action_notice(&app_install.docker,&app_install.app,1,result); //升级消息通知
    }

}

/**********************************************************************
* @name      : App_comand_exec
* @brief     ：执行App的一系列操作
* @param[in] ：docker 容器 app app ,type操作类型enum{启动（0），停止（1），卸载（2），使能（3），去使能（4）}
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

BOOL App_comand_exec(SEQ_STR *docker,SEQ_STR *app,uint8 type)
{

    if(docker->buff==NULL || app->buff==NULL || type >4 )
    {
        return FALSE;
    }
    char cmd[1024]={0};
    char buff[20][500]={{0}};
    uint8 errcode=TRUE;
    
#ifdef PRODUCT_CCU
    switch (type)
    {
        case  0: // 启动
        {
            sprintf(cmd,"appm -s -c %s -n %s | grep started",docker->buff,app->buff);
        }break;
        case 1: //停止
        {
            sprintf(cmd,"appm -S -c %s -n %s | grep stopped",docker->buff,app->buff);
        }break;
        case 2: //卸载
        {
            sprintf(cmd,"appm -u -c %s -n %s | grep Uninstalled",docker->buff,app->buff);
        }break;
        case 3:  //使能
        {
            sprintf(cmd,"appm -e -c %s -n %s | grep enabled",docker->buff,app->buff);
        }break;
        case 4: //去使能
        {
            sprintf(cmd,"appm -d -c %s -n %s | grep disenabled",docker->buff,app->buff);
        }break;
        default:
            break;
    }

	//判断如果时启动需要看这个app有没有软件注册码 ，存在注册码且为36个0的时候，不启动回复错误 台体测试应用管理
    if(type == 0)
    {
        int ret = 0,i;
        app_license_T liscen;
        memset(&liscen,0,sizeof(liscen));
        char filePath[100] = "/data/app/iotManager/data/LICENSE";
        ret = read_pdata_xram(filePath,(char *)&liscen,0,sizeof(app_license_T));
        if(ret<0)
        {
            MQTTMSG_FMT_DEBUG("没有找到LICENSE文件 %s\n",filePath);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("收到启动APP %s 启动命令，检查是否存在 Applicense \n",app->buff);
            
            for(i=0;i<liscen.licennum;i++)
            {
                if(strcmp((char *)app->buff,liscen.licen[i].appname)==0) //找到了
                {
                    MQTTMSG_BUF_DEBUG(liscen.licen[i].applicen,liscen.licen[i].licenlen,"APP licen");
                  //  MQTTMSG_FMT_DEBUG("存在APP %s licenlen %d license %s\n",liscen.licen[i].appname,liscen.licen[i].licenlen,liscen.licen[i].applicen);
                    char tmplice[36] = {0};
                    if(memcmp(tmplice,liscen.licen[i].applicen,24)==0 )  //适配台体测试
                    {
                        MQTTMSG_FMT_DEBUG("App %s license 长度 %d 全0,不执行启动\n",app->buff,liscen.licen[i].licenlen);
                        return FALSE;
                    }
                }
            }
        }
    }
   // MQTTMSG_FMT_DEBUG("exec cmd %s\n",cmd);
    if(getshellresult(cmd,buff,20)==0)
    {
        errcode = FALSE;
        MQTTMSG_FMT_DEBUG("recv cmd %s\n",buff[0]);
    }
    //再验证一下
    //取消复检，认为只要返回没有问题，就确认无问题。
#if 0
    memset(cmd,0,sizeof(cmd));
    switch (type)
    {
            case 0: // 启动            
            case 1: //停止
            {
                sprintf(cmd,"appm -I -c %s -n %s | grep 'Service status' | awk '{print $4}'",docker->buff,app->buff);
            }break;
            case 2: //卸载
            {
                sprintf(cmd,"appm -I -c %s",docker->buff);
            }break;
            case 3:  //使能
            case 4: //去使能
            {
                sprintf(cmd,"appm -I -c %s -n %s | grep 'Service enable' | awk '{print $4}'",docker->buff,app->buff);
            }break;
            default:
                break;
    }
    memset(buff,0,sizeof(buff));
    if(getshellresult(cmd,buff,20)!=1)
    {
        errcode = FALSE;
        MQTTMSG_FMT_DEBUG("recv cmd %s\n",buff[0]);
    }
    switch (type)
    {
            case 0: // 启动
            {
                if(strcmp("running",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 1: //停止
            {
                if(strcmp("stopped",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 2: //卸载
            {
                if(strcmp("No APP installed",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 3:  //使能
            {
                if(strcmp("yes",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 4: //去使能
            {
                if(strcmp("no",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;

            default:
                break;
    }
#endif
#else
	switch (type)
    {
        case  0: // 启动
        {
            sprintf(cmd,"appm -s -c %s -n %s",docker->buff,app->buff);
        }break;
        case 1: //停止
        {
            sprintf(cmd,"appm -S -c %s -n %s",docker->buff,app->buff);
        }break;
        case 2: //卸载
        {
            sprintf(cmd,"appm -u -c %s -n %s",docker->buff,app->buff);
        }break;
        case 3:  //使能
        {
            sprintf(cmd,"appm -e -c %s -n %s",docker->buff,app->buff);
        }break;
        case 4: //去使能
        {
            sprintf(cmd,"appm -d -c %s -n %s",docker->buff,app->buff);
        }break;
        default:
            break;
    }
    uint8 offset= strlen(cmd);
	sprintf(cmd+offset," | grep success");

	//判断如果时启动需要看这个app有没有软件注册码 ，存在注册码且为36个0的时候，不启动回复错误 台体测试应用管理
    if(type == 0)
    {
        int ret = 0,i;
        app_license_T liscen;
        memset(&liscen,0,sizeof(liscen));
        char filePath[100] = "/data/app/iotManager/data/LICENSE";
        ret = read_pdata_xram(filePath,(char *)&liscen,0,sizeof(app_license_T));
        if(ret<0)
        {
            MQTTMSG_FMT_DEBUG("没有找到LICENSE文件 %s\n",filePath);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("收到启动APP %s 启动命令，检查是否存在 Applicense \n",app->buff);
            
            for(i=0;i<liscen.licennum;i++)
            {
                if(strcmp((char *)app->buff,liscen.licen[i].appname)==0) //找到了
                {
                    MQTTMSG_BUF_DEBUG(liscen.licen[i].applicen,liscen.licen[i].licenlen,"APP licen");
                  //  MQTTMSG_FMT_DEBUG("存在APP %s licenlen %d license %s\n",liscen.licen[i].appname,liscen.licen[i].licenlen,liscen.licen[i].applicen);
                    char tmplice[36] = {0};
                    if(memcmp(tmplice,liscen.licen[i].applicen,24)==0 )  //适配台体测试
                    {
                        MQTTMSG_FMT_DEBUG("App %s license 长度 %d 全0,不执行启动\n",app->buff,liscen.licen[i].licenlen);
                        return FALSE;
                    }
                }
            }
        }
    }
   // MQTTMSG_FMT_DEBUG("exec cmd %s\n",cmd);
    if(getshellresult(cmd,buff,20)==0)
    {
        errcode = FALSE;
        MQTTMSG_FMT_DEBUG("recv cmd %s\n",buff[0]);
    }
    //再验证一下
#if 0
    memset(cmd,0,sizeof(cmd));
    switch (type)
    {
            case 0: // 启动            
            case 1: //停止
            {
                sprintf(cmd,"appm -I -c %s -n %s | grep 'Service status' | awk '{print $4}'",docker->buff,app->buff);
            }break;
            case 2: //卸载
            {
                sprintf(cmd,"appm -I -c %s -n %s",docker->buff,app->buff);
            }break;
            case 3:  //使能
            case 4: //去使能
            {
                sprintf(cmd,"appm -I -c %s -n %s | grep 'Service enable' | awk '{print $4}'",docker->buff,app->buff);
            }break;
            default:
                break;
    }
    memset(buff,0,sizeof(buff));
    if(getshellresult(cmd,buff,20)!=1)
    {
        errcode = FALSE;
        MQTTMSG_FMT_DEBUG("recv cmd %s\n",buff[0]);
    }
    switch (type)
    {
            case 0: // 启动
            {
                if(strcmp("running",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 1: //停止
            {
                if(strcmp("stopped",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 2: //卸载
            {
                if(strcmp("no app info",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 3:  //使能
            {
                if(strcmp("YES",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 4: //去使能
            {
                if(strcmp("NO",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;

            default:
                break;
    }
#endif	
#endif
    
    return errcode;
}

/**********************************************************************
* @name      : app_action_notice
* @brief     ：App告警通知
* @param[in] ：docker 容器 app app ,type操作类型{安装（0），升级（1），卸载（2），启动（4），停止（4），配置参数（5），使能,6），去使能（7），下发许可（8）}，
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void app_action_notice(SEQ_STR *docker,SEQ_STR *app,uint8 type,uint8 errcode)
{
    uint16 index = 0;
    uint8 buff[500]={0};
    OOP_DATETIME_S_T StartTime;
    memset(&StartTime,0x00,sizeof(OOP_DATETIME_S_T));
    buff[index++]=docker->len;
    memcpy(&buff[index],docker->buff,docker->len);
    index+=docker->len;
    buff[index++]=app->len;
    memcpy(&buff[index],app->buff,app->len);
    index+=app->len;
    DT_Time2DateTimeShort(time(NULL),&StartTime);
    memcpy_r(&buff[index],&StartTime.year,sizeof(uint16));
    index+=sizeof(uint16);
    buff[index++]=StartTime.month;
    buff[index++]=StartTime.day;
    buff[index++]=StartTime.hour;
    buff[index++]=StartTime.minute;
    buff[index++]=StartTime.second;
    buff[index++] = type;
    buff[index++] = !errcode;
    buff[index++] = 0x00;

    
    MSG_INFO_T* msg  = malloc(sizeof(MSG_INFO_T)+index);
    
    if(msg==NULL)
        return;
    memset(msg,0,sizeof(MSG_INFO_T)+index);
    msg->MsgLen = index;
    msg->MsgRPM = 1;
    msg->MsgPRIORITY = 1;
    msg->MsgIndex = iot_get_next_index();
    msg->MsgLabel = 0;
    uint8 *MSGBuf=NULL;
    uint16 MSGLen=0;
    char topic[1000]={0};
    msg->SourLen = strlen(IOT_APP_NAME_M);
    memcpy(msg->SourAddr, IOT_APP_NAME_M, msg->SourLen);

    msg->DestLen = strlen("Broadcast");
    memcpy(msg->DestAddr, "Broadcast", msg->DestLen);
    msg->IID=MESSAGE_IID_IOT;
    msg->IOP = IOT_IOP_APP_ACTION;
    
    memcpy(msg->MsgData,buff,msg->MsgLen);
    sprintf(topic,"%s/%s/%04X/%04X",msg->SourAddr,msg->DestAddr,msg->IID,msg->IOP);
    MSGLen = iot_appmsg_package(msg,&MSGBuf);    
    MQTTMSG_BUF_DEBUG(MSGBuf, MSGLen,"");
    mqtt_topic_write(MSGBuf, MSGLen,topic); 
   // taskmng_plc_queue_write (&LockWarringMsg, (uint8*)msg, sizeof(MSG_INFO_T)+msg->MsgLen);
    free(msg);
}

/**********************************************************************
* @name      : docker_exec_0015
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0015(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 index = 0;
    BOOL errcode = TRUE;
    uint8 buff[100]={0};
    SEQ_STR docker;
    SEQ_STR app;
    memset(&docker,0,sizeof(SEQ_STR));
    memset(&app,0,sizeof(SEQ_STR));
    docker.len =  MsgRecv->MsgData[index++];
    memcpy(docker.buff,&MsgRecv->MsgData[index],docker.len);
    index+=docker.len;
    app.len = MsgRecv->MsgData[index++];
    memcpy(app.buff,&MsgRecv->MsgData[index],app.len);
    index+=app.len;
    uint8 type = MsgRecv->MsgData[index];
    uint8 realtype=0;
    errcode = App_comand_exec(&docker,&app,type);
    if(type ==0)
    {
        realtype = 3;
    }
    else if(type == 1)
    {
        realtype = 4; 
    }else if(type ==2)
    {
        realtype = 2;
    }else if(type == 3)
    {
        realtype = 6;  
    }else if(type == 4)
    {
        realtype = 7;
    }
    buff[0]=errcode;
    packge_report_msg(MsgRecv,buff,1);      
    sleep(3);
    app_action_notice(&docker,&app,realtype,errcode); 
}
int judge_folder_file(char *file)
{
    if(file == NULL)
        return -1;
    struct stat buf;
    memset(&buf,0,sizeof(buf));
    if(stat(file, &buf)==-1)
    {
        if(errno== ENOENT)
            return -1;
    }
    if(buf.st_mode == S_IFDIR)
    {
        return 1;   //文件夹
    }
    else if(buf.st_mode == S_IFREG)
    {
        return 0;
    }
    return -1;
}

void get_pack_filename(char *file)
{
    uint8 index=0;
    OOP_DATETIME_S_T StartTime;
    memset(&StartTime,0x00,sizeof(OOP_DATETIME_S_T));
    DT_Time2DateTimeShort(time(NULL),&StartTime);
    memcpy_r(&file[index],&StartTime.year,sizeof(uint16));
    index+=sizeof(uint16);
    file[index++]=StartTime.month;
    file[index++]=StartTime.day;
    file[index++]=StartTime.hour;
    file[index++]=StartTime.minute;
    file[index++]=StartTime.second;
}

/**********************************************************************
* @name      : pack_accord_logPath
* @brief     : 根据日志路径操作函数
* @param[in] : exec 操作类型 0 读取信息 1. 打包 2. 删除 filePath 操作的文件的路径 
* @param[out]：recvBuf 回复报文，msglen 报文长度
* @return    ：0 成功 -1 失败
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/
int operate_accord_logPath(char *filePath,char *packfile)
{
    char cmd[1024]={0};
    char buff[10][500]={{0}};

    sprintf(cmd,"log -t -lnp %s -tlnp %s | grep success",filePath,packfile);
    MQTTMSG_FMT_DEBUG("EXEC CMD %s\n",cmd);
    if(getshellresult(cmd,buff,10)==0)
    {
        MQTTMSG_FMT_DEBUG("EXEC CMD FAIL cmd %s\n",cmd);
        MQTTMSG_FMT_DEBUG("RECV BUFF %s\n",buff[0]);
        return -1;
    }

#ifdef PRODUCT_ECU
    memset(cmd, 0, 1024);
    sprintf(cmd, "gzip -c %s > %s", filePath, packfile);
    system(cmd);
#endif

    return 0;
}

/**********************************************************************
* @name      : logstr2datetime_s
* @brief     : 根据日志字符串时间转换为time_s函数
* @param[in] : str 字符串时间
* @param[out]: time_s 转换后的时间
* @return    : 字符串时间标准:  1970-01-01 00:00:00
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void logstr2datetime_s(char* str,OOP_DATETIME_S_T* time_s)
{
    if(str==NULL || 0 == strlen(str))
        return;
    char result[3][255];
    memset(result,0,sizeof(result));
    int num = 0;
    char key[100]={0};
    char value[100]={0};
    char *p = strstr(str," ");
    memcpy(key,str,p-str);
    memcpy(value,p+1,strlen(p)-1);    
    ssplit(key, "-",result, &num);
    time_s->year = atoi(result[0]);
    time_s->month = atoi(result[1]);
    time_s->day  = atoi(result[2]);
    memset(result,0,sizeof(result));
    num = 0;
    ssplit(value, ":",result, &num);
    time_s->hour = atoi(result[0]);
    time_s->minute = atoi(result[1]);
    time_s->second  = atoi(result[2]); 
}

/**********************************************************************
* @name      : get_logFile_info
* @brief     : 根据日志路径填充log结构体函数
* @param[in] : path 日志路径
* @param[out]: log 填充后的log结构体
* @return    : 字符串时间标准:  1970-01-01 00:00:00
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

int get_logFile_info(log_info *log,char *path)
{
    char cmd[1024] = {0};
   // char logpath[255]= {0};
    char result[15][500];
    uint8 ret = 0;

    memset(result,0,sizeof(result));
   // sprintf(logpath,"%s%s",SysLogDir,path);
    sprintf(cmd,"log -i -lnp %s",path);
    ret = getshellresult(cmd, result,13);
    if(ret!=13)
    {
        IOT_FMT_DEBUG("exec cmd error cmd %s\n",cmd);
        return -1;
    }
    uint8 i=0;
    for(i=0;i<ret;i++)
    {
       char *values = strim(result[i]); //去除两边的空格
       values = strdelch(values,'"');  //去除双引号
       values = strdelch(values,',');  //去除逗号
       if(strcmp(values,"{")==0 || strcmp(values,"}")==0 || strcmp(values,"[{")==0 || strcmp(values,"}]")==0)
      {
          continue;
      }
      char key[100]={0};
      char value[100]={0};
      char *p = strstr(values,":");
      memcpy(key,values,p-values);
      memcpy(value,p+1,strlen(p)-1);
      if(strcmp(key,"file")==0)
        {
            if(value!=NULL)
            {
                strcpy(log->logPath,value);
            }
            
        }else if(strcmp(key,"size")==0)
            {
       //     log->logSize = atoi(value);
        }else if(strcmp(key,"md5")==0)
            {
            MQTTMSG_FMT_DEBUG("日志的md5值 为 %s\n",value);
            uint8 bit_str[100] = {0};    
            int bitlen = strt2octet(value,strlen(value),bit_str,64);
            if((bitlen<0) || (bitlen >64))
            {
                MQTTMSG_FMT_DEBUG("转化失败 bufflen %d \n",bitlen);
            }
            else
            {   
                memcpy(log->logMd5,bit_str,bitlen);
                log->md5len = bitlen;
            }
        }else if(strcmp(key,"create_time")==0)
            {
            logstr2datetime_s(value,&log->logCreat);
           // strcpy(log->logMd5,value);
        }else if(strcmp(key,"type")==0)
            {
                if(strcmp(path,"/var/log/messages")==0)
                {
                    log->logType = 1;
                }else
                {
                    log->logType = atoi(value);
                }       
        }else if(strcmp(key,"container_name")==0)
            {
            if(value!=NULL)
            strcpy(log->conName,value);
        }else if(strcmp(key,"app_name")==0)
            {
            if(value!=NULL)
            strcpy(log->appName,value);
        }
    }
    char buff[10][500];
    memset(buff,0,sizeof(buff));
    memset(cmd,0,sizeof(cmd));
    sprintf(cmd,"wc -c %s | awk '{print$1}'",path);
    if(getshellresult(cmd,buff,1)!=1)
        return 1;
    log->logSize = atoi(buff[0]);
    return 1;
}

/**********************************************************************
* @name      : pack_accord_logPath
* @brief     : 根据日志类型操作函数
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/
void operate_accord_logType(uint8 type,char *source)
{
    sprintf(source,"/var/log/messages");
}

uint8 get_filepath(uint8 type,char buff[][100])
{
	if(type==0xff)
	return 0;
	char cmd[100] = {0};
	char result[10][500] = {0};
	uint8 ret =0,i=0;
	//  #ifdef PRODUCT_ECU
	//  sprintf(cmd,"ls /tmp/ | grep messages");
	//  #else
	sprintf(cmd,"ls /var/log/ | grep messages");

	//  #endif
	ret = getshellresult(cmd,result,1);
	for(i=0;i<ret;i++)
	{
	//    #ifdef PRODUCT_ECU
	//    sprintf(buff[i],"%s%s","/tmp/",result[i]);
	//    #else
	sprintf(buff[i],"%s%s","/var/log/",result[i]);
	//    #endif
	// strcpy(buff[i],result[i]);
	}
	return ret;
}

uint16 pack_report_fileinfo(log_info_T *log,uint8 *recv)
{
    uint16 index = 0,i=0;
    recv[index++] = log->lognum;
    for(i=0;i<log->lognum;i++)
    {
        recv[index++] = strlen(log->log[i].logPath);
        memcpy(&recv[index],log->log[i].logPath,strlen(log->log[i].logPath));
        index+=strlen(log->log[i].logPath);
        memcpy_r(&recv[index],&log->log[i].logSize,sizeof(uint32));
        index+=sizeof(uint32);
        if(log->log[i].md5len==0)
        {
            recv[index++] = 0;
        }
        else
        {
            recv[index++] = 1;
            recv[index++] = log->log[i].md5len;
            memcpy(&recv[index],log->log[i].logMd5,log->log[i].md5len);
            index+=log->log[i].md5len;
        }
        memcpy_r(&recv[index],&log->log[i].logCreat.year,sizeof(uint16));
        index+=sizeof(uint16);
        recv[index++] = log->log[i].logCreat.month;
        recv[index++] = log->log[i].logCreat.day;
        recv[index++] = log->log[i].logCreat.hour;
        recv[index++] = log->log[i].logCreat.minute;
        recv[index++] = log->log[i].logCreat.second;
        recv[index++] = log->log[i].logType;
        if(strlen(log->log[i].conName)!=0)
        {
            recv[index++] = 1;
            recv[index++] = strlen(log->log[i].conName);
            memcpy(&recv[index],log->log[i].conName,strlen(log->log[i].conName));
            index+=strlen(log->log[i].conName);
        }
        else
        {
            recv[index++] = 0;
        }
        if(strlen(log->log[i].appName)!=0)
        {
            recv[index++] = 1;
            recv[index++] = strlen(log->log[i].appName);
            memcpy(&recv[index],log->log[i].appName,strlen(log->log[i].appName));
            index+=strlen(log->log[i].appName);
        }
        else
        {
            recv[index++] = 0;
        }
    }
    return index;
}

/**********************************************************************
* @name      : docker_exec_001C
* @brief     : 消息App操作命令执行
* @param[in] : MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]:
* @return    :
* @Create    : 
* @Date      : 2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_001C(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data   = MsgRecv->MsgData;
    uint16 recvlen= 0;
    uint8 recvBUFF[1024] = {0};
    char logFile[255] = {0};
    uint8 filelen = 0,index=1;
    int ret = 0;
    log_info_T loginfo;
    memset(&loginfo,0,sizeof(loginfo));

    do{
        if(data[0] == 0)
        {
            filelen = data[index++];
            memcpy(logFile,&data[index],filelen);
            index+=filelen;
            ret = get_logFile_info(&loginfo.log[0],logFile);
            if(ret < 0)
            {
                loginfo.lognum = 0;
                break;
            }
            else
            {
                loginfo.lognum = 1;
                break;
            }
        }else if(data[0] == 1)
        {
            char buff[20][100];
            memset(buff,0,sizeof(buff));
            uint8 pathnum=0,i=0;;
            uint8 type = data[index++];
            pathnum = get_filepath(type,buff);
           // loginfo.lognum = pathnum;
            uint8 j=0;
            for(i=0;i<pathnum;i++)
            {            
                ret = get_logFile_info(&loginfo.log[j],buff[i]);
                if(ret < 0)
                {
                    continue;
                }
                loginfo.lognum++;
                j++;
            }
        }       
        else 
        {
            loginfo.lognum = 0;
            break;            
        }
    }while(0);
    recvlen = pack_report_fileinfo(&loginfo,recvBUFF);
    packge_report_msg(MsgRecv,recvBUFF,recvlen);
}

int judge_path_fold_dir(char *path)
{
    if(path==NULL)
        return -1;
    struct stat buf;
    stat(path,&buf);
    if(S_IFDIR& buf.st_mode)
    {
        return 1;
    }else if(S_IFREG & buf.st_mode)
    {
        return 0;
    }else if(strstr(path,".gz")!=NULL)
    {
        return 0;
    }
   return -1;
}


/**********************************************************************
* @name      : docker_exec_001D
* @brief     : 001D 日志打包
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_001D(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data   = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    int ret=0;
    uint8 recvBuff[1024] = {0};      
   // uint16 recvLen=0;
    uint16 index=1,tmplen=0,option=0;
    char tmppackPath[255] = {0};
    char SourcePath[255]={0};
    char packPath[255] = {0};
    uint8 retype = 1;
   do{
        if(data[0]==0)            //日志文件路径 [0]  visible-string，
        {
            tmplen = data[index++];
            memcpy(SourcePath,&data[index],tmplen);
            index+=tmplen;
            option  = data[index++];
            if(option == 0)
            {
                sprintf(packPath,"%s%s%d",PackLogDir,"pack",packnum++);
            }else if(option == 1)
            {
                tmplen = data[index++];
                memcpy(tmppackPath,&data[index],tmplen);
                index+=tmplen;
            }else
            {
                retype = 0;
                MQTTMSG_FMT_DEBUG("recv message error option %d\n",option);
                break;
            }

            /*判断是文件还是文件夹*/
        }
        else if(data[0]==1)     //日志类型     [1]  LOGTYPE
        {
            uint8 type = data[index++];
            operate_accord_logType(type,SourcePath);
            option   = data[index++];
            if(option == 0)  //默认文件夹下
            {
                sprintf(packPath,"%s%s%d",PackLogDir,"pack",packnum++);
            }else if(option == 1)
            {
                tmplen = data[index++];
                memcpy(tmppackPath,&data[index],tmplen);
                index+=tmplen;
            }else
            {
                retype = 0;
                MQTTMSG_FMT_DEBUG("recv message error option %d\n",option);
                break;
            }
        }
        else
        {
            retype = 0;
            MQTTMSG_FMT_DEBUG("recv message error option %d\n",option);
            break; 
        }
         if(option == 1)
         {
            /*判断是目录还是文件*/
            int filetype = judge_path_fold_dir(tmppackPath);
            if(filetype == -1 )
            {
                retype =0;
                MQTTMSG_FMT_DEBUG("path error %s\n",tmppackPath);
                break;
            }else if(filetype == 0)
            {
                //文件
                sprintf(packPath,"%s",tmppackPath);
            }else
            {
                //目录
                sprintf(packPath,"%s%s%d",tmppackPath,"pack",packnum++);
            }
        }
         if(index!=msglen)
         {
              retype = 0;
              MQTTMSG_FMT_DEBUG("recv message len != index \n");
              break;
         }
        
   }while(0);
    index=0;
    if(retype == 0)
    {
        recvBuff[index++] = 0;
    }else
    {
        ret = operate_accord_logPath(SourcePath,packPath);
        if(ret == -1)
        {
            recvBuff[index++]=0;
        }
        else
        {  
            index  = set_len_offset(strlen(packPath), recvBuff);
            memcpy(&recvBuff[index],packPath,strlen(packPath));
            index+=strlen(packPath);              
        }
    }

    packge_report_msg(MsgRecv,recvBuff,index);    
}

BOOL judge_path_exec(char *logFile)
{
    char cmd[1024] = {0};
    char buff[1][500];
    memset(buff,0,sizeof(buff));
    if(access(logFile,R_OK)!=0)
    {
        return FALSE;
    }
 //   if(strstr(logFile,"/data/app/")!=NULL || strstr(logFile,"/var/log/")!=NULL)
 //   {
 //       return FALSE;
 //   }
    
    sprintf(cmd,"rm -rf %s",logFile);
    if(getshellresult(cmd,buff,1)!=0)
    {
        return FALSE;
    }
    return TRUE;
}
/**********************************************************************
* @name      : docker_exec_001E
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_001E(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data   = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    char logFile[255] = {0};
    uint8 filelen = 0,index=1;
    BOOL result = TRUE;

    uint8 recvBUFF[50]={0};
    do{
        if(data[0]==0)       //日志文件路径 [0]  visible-string   
        {
            filelen = data[index++];
            memcpy(logFile,&data[index],filelen);
            index+=filelen;
            result  = judge_path_exec(logFile);
            break;
        }
        else if(data[0]==1) //日志类型     [1]  LOGTYPE
        {
            uint8 logtype=0;
            logtype = data[index++];
            operate_accord_logType(logtype,logFile);
            result  = judge_path_exec(logFile);
            break;

        }else
        {
            result = FALSE;
            break;
        }
        if(index!=msglen)
        {
            break;
        }
    }while(0);
    recvBUFF[0] = result;
    packge_report_msg(MsgRecv,recvBUFF,1);
}

/**********************************************************************
* @name      : docker_exec_001F
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_001F(IOT_APPDATA_T *pData,MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data   = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    SYS_CONF sysconf;
    memset(&sysconf,0,sizeof(sysconf));
    char filePath[255] = {0};
    uint8 recvBUFF[20] = {0};
    uint8 index=0;
    BOOL result = TRUE;
    do{
    memcpy_r(&sysconf.memLimit,&data[index],sizeof(uint16));
    index+= sizeof(uint16);
    memcpy_r(&sysconf.diskLimit,&data[index],sizeof(uint16));
    index+= sizeof(uint16);
    memcpy_r(&sysconf.cpulimit,&data[index],sizeof(uint16));
    index+= sizeof(uint16);
    memcpy_r(&sysconf.cputemp_up,&data[index],sizeof(uint16));
    index+= sizeof(uint16);
    memcpy_r(&sysconf.cputemp_down,&data[index],sizeof(uint16));
    index+= sizeof(uint16);
    memcpy_r(&sysconf.sys_monitortime,&data[index],sizeof(uint16));
    index+= sizeof(uint16);
    if(index != msglen)
    {
        result = FALSE;
        break;
    }
      sysconf.sys_monitortime = sysconf.sys_monitortime*60;
//      sysconf.cpulimit        = sysconf.cpulimit*100;
//      sysconf.cpulimit        = sysconf.cpulimit*100;
//      sysconf.cpulimit        = sysconf.cpulimit*100;
      pData->app_t.devCpuLimit      =   sysconf.cpulimit;
      pData->app_t.devMemLimit      =   sysconf.memLimit;
      pData->app_t.devDiskLimit     =   sysconf.diskLimit;
      pData->app_t.devCpuTempup     =   sysconf.cputemp_up;
      pData->app_t.devCpuTempdown   =   sysconf.cputemp_down;
      pData->app_t.monitorTime      =   sysconf.sys_monitortime;
      INIT_FMT_DEBUG("系统监测参数变更 devcpulimit %d devMemLimit %d devDiskLimit %d devCpuTempdown %d devCpuTempup %d monitorTime %d\n", pData->app_t.devCpuLimit,pData->app_t.devMemLimit,pData->app_t.devDiskLimit,pData->app_t.devCpuTempdown,pData->app_t.devCpuTempup,pData->app_t.monitorTime);
     // system("mkdir /data/app/iotManager/data/");
      sprintf(filePath,"%s%s",PrivateDir,"SYSCONF");
      write_pdata_xram(filePath,(char *)&sysconf,0,sizeof(SYS_CONF));
    }while(0);
    recvBUFF[0] = result;
    packge_report_msg(MsgRecv,recvBUFF,1);
}

/**********************************************************************
* @name      : docker_exec_001B
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_001B(MSG_INFO_T *MsgRecv,int MSGLen)
{
    char buff[1][500];
    char sysversion[100]={0};
    char kerversion[100]={0};
    char tmpData[500]={0};
    //char filesysver[100]={0};
    char hardversion[100]={0};
    uint16 index = 0;
    memset(buff,0,sizeof(buff));
    if(getshellresult("uname -r",buff,1)==1)
    {
        strcpy(sysversion,buff[0]);
    }   
    memset(buff,0,sizeof(buff));
    if(getshellresult("uname -n",buff,1)==1)
    {
        strcpy(kerversion,buff[0]);    
    }
    strcpy(hardversion,"DM01");  //暂时写死
    tmpData[index++]=strlen(sysversion);
    strcpy(&tmpData[index],sysversion);
    index+=strlen(sysversion);
    tmpData[index++]=strlen(kerversion);
    strcpy(&tmpData[index],kerversion);
    index+=strlen(kerversion);
    tmpData[index++] = 0;//文件系统版本为NULL
    tmpData[index++]=strlen(hardversion);
    strcpy(&tmpData[index],hardversion);
    index+=strlen(hardversion);
    packge_report_msg(MsgRecv,(uint8 *)tmpData,index);  //应答
}

/**********************************************************************
* @name      : docker_exec_0019
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_001A(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data   = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    uint8 tmplen=0,sigtype=0,ret=0;
    uint16 index=0;
    BOOL result = TRUE;
    
    char cmd[2048] = {0};
    uint8 filemd5[255] = {0};
    uint8 signmd5[255] = {0};
    uint8 recvBuff[50] = {0};
    char buff[10][500] = {{0}};
    sys_update_info sys_info;
    memset(&sys_info,0,sizeof(sys_update_info));
    tmplen = data[index++];
    memcpy(sys_info.updatever,&data[index],tmplen);
    index+=tmplen;
    sys_info.updatetype = data[index++];
    do{
        if(sys_info.updatetype > 3)
        {
            //返回错误
            result = FALSE;
            break;
        }
        tmplen = data[index++];
        memcpy(sys_info.file_info_t.filename,&data[index],tmplen);
        index+=tmplen;
        tmplen = data[index++];
        memcpy(sys_info.file_info_t.filetype,&data[index],tmplen);
        index+=tmplen;
        tmplen = data[index++];
        memcpy(sys_info.file_info_t.filepath,&data[index],tmplen);
        index+=tmplen;
        memcpy_r(&sys_info.file_info_t.filesize,&data[index],sizeof(uint32));
        index+=sizeof(uint32);
        tmplen = data[index++];
        if(tmplen > 0)
        {
            memcpy(sys_info.file_info_t.filemd5, &data[index], tmplen);
        }
        index += tmplen;
        
        sigtype = data[index++];
        if(sigtype!=0)
        {
            tmplen = data[index++];
            memcpy(sys_info.file_info_t.signinfo_t.signname,&data[index],tmplen);
            index+=tmplen;
            tmplen = data[index++];
            memcpy(sys_info.file_info_t.signinfo_t.signpath,&data[index],tmplen);
            index+=tmplen;
            memcpy_r(&sys_info.file_info_t.signinfo_t.signsize,&data[index],sizeof(uint16));
            index+=sizeof(uint16);
            tmplen = data[index++];
            memcpy(sys_info.file_info_t.signinfo_t.signmd5,&data[index],tmplen);
            index+=tmplen;
        }
        if(index!=msglen)
        {
            MQTTMSG_FMT_DEBUG("报文验证错误 index %d msglen %d \n",index,msglen);
            result = FALSE;
            break;
        }
        //验证升级文件是否存在
       // sprintf(filePath,"%s/%s",sys_info.file_info_t.filepath,sys_info.file_info_t.filename);
       if(0 == sys_info.file_info_t.filemd5[0])
       {
            ret = sizeAndmd5_judge(sys_info.file_info_t.filepath,sys_info.file_info_t.filesize, NULL);
       }
       else
       {
            ret = sizeAndmd5_judge(sys_info.file_info_t.filepath,sys_info.file_info_t.filesize, (char *)sys_info.file_info_t.filemd5);
       }
        
        if(ret != 0)
        {
            MQTTMSG_FMT_DEBUG("升级文件大小或者MD5值校验有问题  \n");
            result = FALSE;
            break;
        }
        if(sigtype != 0)
        {
            strt2octet((char *)sys_info.file_info_t.signinfo_t.signmd5,strlen((char *)sys_info.file_info_t.signinfo_t.signmd5),filemd5,64);
            if(sigtype==1)
            {
                char filePath[1024]={0};
                memset(filePath,0,sizeof(filePath));
                sprintf(filePath,"%s/%s",sys_info.file_info_t.signinfo_t.signpath,sys_info.file_info_t.signinfo_t.signname);
                ret = sizeAndmd5_judge(filePath,sys_info.file_info_t.signinfo_t.signsize,(char *)sys_info.file_info_t.signinfo_t.signmd5);
                if(ret != 0)
                {
                    result = FALSE;
                    break;
                }
                strt2octet((char *)sys_info.file_info_t.signinfo_t.signmd5,strlen((char *)sys_info.file_info_t.signinfo_t.signmd5),signmd5,64);
            }
        }
    }while(0);
    //转换md5值
    recvBuff[0] = result;
    packge_report_msg(MsgRecv,recvBuff,1);
    if(FALSE == result)
    {
        return;
    }
    
    //拼接命令
    if(sys_info.updatetype==0)
    {
        sprintf(cmd,"update -p ");
    }else if(sys_info.updatetype==1)
    {
        sprintf(cmd,"update -fs ");
    }else if(sys_info.updatetype==2)
    {
        sprintf(cmd,"update -k ");
    }else if(sys_info.updatetype==3)
    {
        sprintf(cmd,"update -a ");
    }
    tmplen = strlen(cmd);

    sprintf(&cmd[tmplen]," -fi -fp %s%s -ch %s", sys_info.file_info_t.filepath, sys_info.file_info_t.filename ,filemd5);
    if(sigtype == 1)
    {
        tmplen = strlen(cmd);
        sprintf(&cmd[tmplen],"-si -fp %s -ch %s",sys_info.file_info_t.signinfo_t.signpath,sys_info.file_info_t.signinfo_t.signmd5);
    }
     //执行命令不需要返回值
     //先回复正确并创建文件
    MQTTMSG_FMT_DEBUG("检测到系统升级  命令 CMD: %s\n",cmd);
    ret = getshellresult(cmd, buff, 20);
    if(ret > 0 )
    {
        print_exec_faild(ret,buff);
    }

  //  memset(filePath,0,sizeof(filePath));
 //   sprintf(filePath,"%s%s",PrivateDir,"SYSUPDATE");
  //  write_pdata_xram(filePath,(char *)&sys_info,0,sizeof(sys_update_info));
    system(cmd);

    //临时操作 升级修改版本号
    if(strstr(sys_info.updatever, "1.1") != NULL)
	{
		system("cp /backup/os-version_1.1.yaml /etc/os-version.yaml");
	}
	else
	{
		system("cp /backup/os-version_1.2.yaml /etc/os-version.yaml");
	}

	system("cp /etc/os-version.yaml  /tmp/vsys");
    
}

/**********************************************************************
* @name      : docker_exec_0019
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0019(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    uint16 index=0,i=0,ret=0,type = 0;
    uint8 recvbuff[2048]={0};
    SEQ_STR app;
    app_license_T liscen;
    memset(&liscen,0,sizeof(liscen));
    memset(&app,0,sizeof(SEQ_STR));
#ifdef PRODUCT_CCU
	uint32 count = 0;
	int j = 0;
	uint8 tempvalue = 0;
	
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        MQTTMSG_FMT_DEBUG("devinfo hal_device_get error \n");
    }
    else
    {
        count = dev->devinfo_applicense_count(dev);
        if(count == 0)
        {
            MQTTMSG_FMT_DEBUG("devinfo devinfo_applicense_count error count %d\n", count);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("devinfo devinfo_applicense_count  %d\n", count);
        }

		for(i=0; i<count; i++)
		{
			ret = dev->devinfo_read_applicense_index(dev, i, liscen.licen[liscen.licennum].appnum, liscen.licen[liscen.licennum].appname, liscen.licen[liscen.licennum].applicen, 100);
	        if(ret == 0)
	        {
	            liscen.licennum ++;
	        }
	        else
	        {
	            MQTTMSG_FMT_DEBUG(" devinfo_read_applicense_index error index%d ret%d !\n", i, ret);
	        }
		}
        hal_device_release((HW_DEVICE *)dev);

		
        /*需要哪些app的LICENSE*/
        if(data[index]==0)
        {
            //全部
            type = 0;
            index++;
        }
        else
        {
            app.len = data[index++];
            memcpy(app.buff,&data[index],app.len);
            index+=app.len;
            type = 1;
        }
        if(index!=msglen)
        {
            //消息报文有错误
            MQTTMSG_FMT_DEBUG("消息报文有错误 \n");
        }
        index=0;
        if(type==0)
        {   
            recvbuff[index++]=liscen.licennum;
            for(i=0;i<liscen.licennum;i++)
            {
            	liscen.licen[i].num = strlen(liscen.licen[i].appnum);
                recvbuff[index++] = liscen.licen[i].num;
                memcpy(&recvbuff[index],liscen.licen[i].appnum,liscen.licen[i].num);
                index+=liscen.licen[i].num;

				liscen.licen[i].namelen = strlen(liscen.licen[i].appname);
                recvbuff[index++] = liscen.licen[i].namelen;
                memcpy(&recvbuff[index],liscen.licen[i].appname,liscen.licen[i].namelen);
                index+=liscen.licen[i].namelen;

				liscen.licen[i].licenlen = strlen(liscen.licen[i].applicen)/2;
				recvbuff[index++] = liscen.licen[i].licenlen;
				for(j=0; j<strlen(liscen.licen[i].applicen);j+=2)
	            {
	            	tempvalue = (liscen.licen[i].applicen[j] <= '9') ? (liscen.licen[i].applicen[j] - '0') : (liscen.licen[i].applicen[j] - 'A' + 10);
					recvbuff[index] = tempvalue << 4;
					tempvalue = (liscen.licen[i].applicen[j+1] <= '9') ? (liscen.licen[i].applicen[j+1] - '0') : (liscen.licen[i].applicen[j+1] - 'A' + 10);
	                recvbuff[index] += tempvalue;
                	index++;
	            }
            }
        }
        else if(type==1)
        {
            
            for(i=0;i<liscen.licennum;i++)
            {
                if(memcmp(app.buff,liscen.licen[i].appname,app.len)==0)
                {
                	recvbuff[index++]=1;
                    recvbuff[index++] = liscen.licen[i].num;
                    memcpy(&recvbuff[index],liscen.licen[i].appnum,liscen.licen[i].num);
                    index+=liscen.licen[i].num;
                    recvbuff[index++] = liscen.licen[i].namelen;
                    memcpy(&recvbuff[index],liscen.licen[i].appname,liscen.licen[i].namelen);
                    index+=liscen.licen[i].namelen;
					
                    liscen.licen[i].licenlen = strlen(liscen.licen[i].applicen)/2;
					recvbuff[index++] = liscen.licen[i].licenlen;
					for(j=0; j<strlen(liscen.licen[i].applicen);j+=2)
		            {
		            	tempvalue = (liscen.licen[i].applicen[j] <= '9') ? (liscen.licen[i].applicen[j] - '0') : (liscen.licen[i].applicen[j] - 'A' + 10);
						recvbuff[index] = tempvalue << 4;
						tempvalue = (liscen.licen[i].applicen[j+1] <= '9') ? (liscen.licen[i].applicen[j+1] - '0') : (liscen.licen[i].applicen[j+1] - 'A' + 10);
		                recvbuff[index] += tempvalue;
	                	index++;
		            }
                    break;
                }
            }

			if(i == liscen.licennum)	//没有找到
			{
				recvbuff[index++]=1;
			}
        }
        packge_report_msg(MsgRecv,recvbuff,index);
        return;
   
    }
#else
    char filePath[150]={0};
    sprintf(filePath,"%s%s",PrivateDir,"LICENSE");
    if(access(filePath,R_OK)==0)
    {
        ret = read_pdata_xram(filePath,(char *)&liscen,0,sizeof(app_license_T));
        if(ret<0)
        {
            //错误回复
            recvbuff[0]=0;
            packge_report_msg(MsgRecv,recvbuff,1);
            MQTTMSG_FMT_DEBUG("读私有文件失败 \n");
            return;
            
        }
        else
        {
            /*需要哪些app的LICENSE*/
            if(data[index]==0)
            {
                //全部
                type = 0;
                index++;
            }
            else
            {
                app.len = data[index++];
                memcpy(app.buff,&data[index],app.len);
                index+=app.len;
                type = 1;
            }
            if(index!=msglen)
            {
                //消息报文有错误
                MQTTMSG_FMT_DEBUG("消息报文有错误 \n");
            }
            index=0;
            if(type==0)
            {   
                recvbuff[index++]=liscen.licennum;
                for(i=0;i<liscen.licennum;i++)
                {
                    recvbuff[index++] = liscen.licen[i].num;
                    memcpy(&recvbuff[index],liscen.licen[i].appnum,liscen.licen[i].num);
                    index+=liscen.licen[i].num;
                    recvbuff[index++] = liscen.licen[i].namelen;
                    memcpy(&recvbuff[index],liscen.licen[i].appname,liscen.licen[i].namelen);
                    index+=liscen.licen[i].namelen;
                    recvbuff[index++] = liscen.licen[i].licenlen;
                    memcpy(&recvbuff[index],liscen.licen[i].applicen,liscen.licen[i].licenlen);
                    index+=liscen.licen[i].licenlen;
                }
            }
            else if(type==1)
            {
                recvbuff[index++]=1;
                for(i=0;i<liscen.licennum;i++)
                {
                    if(memcmp(app.buff,liscen.licen[i].appname,app.len)==0)
                    {
                        recvbuff[index++] = liscen.licen[i].num;
                        memcpy(&recvbuff[index],liscen.licen[i].appnum,liscen.licen[i].num);
                        index+=liscen.licen[i].num;
                        recvbuff[index++] = liscen.licen[i].namelen;
                        memcpy(&recvbuff[index],liscen.licen[i].appname,liscen.licen[i].namelen);
                        index+=liscen.licen[i].namelen;
                        recvbuff[index++] = liscen.licen[i].licenlen;
                        memcpy(&recvbuff[index],liscen.licen[i].applicen,liscen.licen[i].licenlen);
                        index+=liscen.licen[i].licenlen;
                        break;
                    }
                }
            }
            packge_report_msg(MsgRecv,recvbuff,index);
            return;
        }
    }
    else
    {
        //错误回复
        recvbuff[0]=0;
        packge_report_msg(MsgRecv,recvbuff,1);
        MQTTMSG_FMT_DEBUG("没有找到该私有文件 %s\n",filePath);
        return;
    }
#endif    
    
}

/**********************************************************************
* @name      : docker_exec_0018
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0018(MSG_INFO_T *MsgRecv,int MSGLen)
{
    MQTTMSG_FMT_DEBUG("收到软件注册码下发指令 \n");
    uint8 *data = MsgRecv->MsgData;
    uint16 msglen = MsgRecv->MsgLen;
    uint16 index=0,count=0,i=0;
    uint8 recvbuff[10]={0};
    app_license_T liscen;
    app_license_T tmpliscen;
    uint8 result= 1;
    uint8 ret=0;
    memset(&liscen,0,sizeof(liscen));
    memset(&tmpliscen,0,sizeof(tmpliscen));
    if(data[index]==0)
    {
        recvbuff[0]=0;
        packge_report_msg(MsgRecv,recvbuff,1);
        return;
    }
    count = data[index++];
    liscen.licennum = count;
    for(i=0;i<count;i++)
    {
        liscen.licen[i].num = data[index++];
        memcpy(liscen.licen[i].appnum,&data[index],liscen.licen[i].num);
        index+=liscen.licen[i].num;
        liscen.licen[i].namelen = data[index++];
        memcpy(liscen.licen[i].appname,&data[index],liscen.licen[i].namelen);
        index+=liscen.licen[i].namelen;
        liscen.licen[i].licenlen = data[index++];
        memcpy(liscen.licen[i].applicen,&data[index],liscen.licen[i].licenlen);
        index+=liscen.licen[i].licenlen;
        
    }
    if(index!=msglen)
    {
        
        recvbuff[0]=0;
        packge_report_msg(MsgRecv,recvbuff,1);
        return;
    }
    MQTTMSG_BUF_DEBUG(liscen.licen[i].applicen, liscen.licen[i].licenlen,"收到的liscen");
    //检查是否存在异常licen 匹配24位
    char tmpcode[30]= {0};
    for(i=0;i<count;i++)
    {
        if(memcmp(liscen.licen[i].applicen,tmpcode,24)==0)
        {
            MQTTMSG_FMT_DEBUG("匹配到24位0 消息回复错误\n");
            result = 0;
            break;
        }
        
    }
    /*和私有文件中的做比对，appname相同的替换，不同的增加，比对*/
    char filePath[150]={0};
    sprintf(filePath,"%s%s",PrivateDir,"LICENSE");
    ret = read_pdata_xram(filePath,(char *)&tmpliscen,0,sizeof(tmpliscen));
    if(ret == 0)
    {
        if(tmpliscen.licennum==0)
        {
          tmpliscen.licennum = liscen.licennum;
          memcpy(tmpliscen.licen,liscen.licen,liscen.licennum*sizeof(app_license));
        }
        else
        {
            uint8 i=0,j=0,lable=0;
            for(i=0;i<liscen.licennum;i++)
            {
                for(j=0;j<tmpliscen.licennum;j++)
                {
                    lable = 0;
                    if(strcmp((char*)liscen.licen[i].appname,(char*)tmpliscen.licen[j].appname)==0)
                    {
                        MQTTMSG_FMT_DEBUG("软件注册码库找到同名应用替换 App %s\n",liscen.licen[i].appname);
                        tmpliscen.licen[j].licenlen = liscen.licen[i].licenlen;
                        memcpy(tmpliscen.licen[j].applicen,liscen.licen[i].applicen,tmpliscen.licen[j].licenlen);
                        tmpliscen.licen[j].num = liscen.licen[i].num;
                        memcpy(tmpliscen.licen[j].appnum,tmpliscen.licen[j].appnum,tmpliscen.licen[j].num);
                        lable = 1;
                        break;
                    }
                }
                if(lable!=1)
                {
                    tmpliscen.licen[tmpliscen.licennum].licenlen = liscen.licen[i].licenlen;
                    memcpy(tmpliscen.licen[tmpliscen.licennum].applicen,liscen.licen[i].applicen,liscen.licen[i].licenlen);
                    tmpliscen.licen[tmpliscen.licennum].num = liscen.licen[i].num;
                    memcpy(tmpliscen.licen[tmpliscen.licennum].appnum,liscen.licen[i].appnum,liscen.licen[i].num);
                    tmpliscen.licen[tmpliscen.licennum].namelen = liscen.licen[i].namelen;
                    memcpy(tmpliscen.licen[tmpliscen.licennum].appname,liscen.licen[i].appname,liscen.licen[i].namelen);
                    MQTTMSG_FMT_DEBUG("软件注册码库未找到同名应用新增 App %s\n",liscen.licen[i].appname);
                    tmpliscen.licennum++;  
                }
            }
        }

    }else if(access(filePath,F_OK)<0)  //异常原因丢失则创建一个
    {
          tmpliscen.licennum = liscen.licennum;
          memcpy(tmpliscen.licen,liscen.licen,liscen.licennum*sizeof(app_license));
    }
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        MQTTMSG_FMT_DEBUG("devinfo hal_device_get error\n");
    }
    else
    {
        char lictemp[100] = {0};
        int  len = 0;
        uint8 ii = 0;
        uint8 tempvalue = 0;
        for(i=0;i<tmpliscen.licennum;i++)
        {
            memset(lictemp, 0, 100);
            len = 0;
            for(ii = 0; ii < tmpliscen.licen[i].licenlen; ii++)
            {
                tempvalue = ((tmpliscen.licen[i].applicen[ii] >> 4) & 0x0F);
                lictemp[len] = (tempvalue <= 9) ? (tempvalue + '0') : (tempvalue + 'A' - 10);
                tempvalue = (tmpliscen.licen[i].applicen[ii] & 0x0F);
                lictemp[len + 1] = (tempvalue <= 9) ? (tempvalue + '0') : (tempvalue + 'A' - 10);
                len += 2;
            }
            ret = dev->devinfo_write_applicense(dev,tmpliscen.licen[i].appnum,tmpliscen.licen[i].appname, lictemp, len);
            if(ERR_OK != ret)
            {
                MQTTMSG_FMT_DEBUG("devinfo devinfo_write_applicense error, ret %d\n", ret);
            }
        }

        hal_device_release((HW_DEVICE *)dev);
    }
    ret = write_pdata_xram(filePath,(char *)&tmpliscen,0,sizeof(app_license_T));
    if(ret == 0)
    {
        for(i=0;i<tmpliscen.licennum;i++)
        {
            MQTTMSG_FMT_DEBUG("App %s 写软件注册码成功\n",tmpliscen.licen[i].appname);
        }     
    }
    /*判断都属于哪个容器*/
    uint8 j=0,k=0,flag = 0;
    //pthread_mutex_lock(&allcontainerinfo.lock);
    for(i=0;i<count;i++)
    {
        flag = 0;
        for(j=0;j<allcontainerinfo.count;j++)
        { 
            for(k=0;k<allcontainerinfo.containers[j].apps.appcount;k++)
            {
                if(memcmp(liscen.licen[i].appname,&allcontainerinfo.containers[j].apps.appinfo[k].appname,liscen.licen[i].namelen)==0)
                {
                    flag = 1;
                    SEQ_STR docker,app;
                    docker.len = strlen((char *)allcontainerinfo.containers[j].container.ContainerName);
                    strcpy((char *)docker.buff,(char *)allcontainerinfo.containers[j].container.ContainerName);
                    app.len = strlen((char *)liscen.licen[i].appname);
                    strcpy((char *)app.buff,(char *)liscen.licen[i].appname);
                    app_action_notice(&docker,&app,8,result);
                    break;
                }
            }
            if(flag==1)
            {
                break;
            }           
        }
    }
 //   pthread_mutex_unlock(&allcontainerinfo.lock);
    recvbuff[0]=1;
    packge_report_msg(MsgRecv,recvbuff,1);
 //   app_action_notice();
    
}

/**********************************************************************
* @name      : get_all_app
* @brief     : 获得所有的App信息
* @param[in] : MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]:
* @return    :
* @Create    : 
* @Date      : 2020-5-22
* @Update    :
**********************************************************************/

int get_all_app(ALL_APP_T *app_t)
{
    uint8 i,j;
   // pthread_mutex_lock(&allcontainerinfo.lock);
    if(allcontainerinfo.count==0)
    {
    //  pthread_mutex_unlock(&allcontainerinfo.lock);
        return 0;
    }
    for(i=0;i<allcontainerinfo.count;i++)
    {
        ALL_APP_INFO_T *app = &allcontainerinfo.containers[i].apps;
        for(j=0;j<app->appcount;j++)
        {
            strcpy((char *)app_t->appinfo[app_t->appcount].containername,(char *)app->appinfo[j].containername); 
            strcpy((char *)app_t->appinfo[app_t->appcount].appname,(char *)app->appinfo[j].appname);
            app_t->appcount++;
        }
    }
  //  pthread_mutex_unlock(&allcontainerinfo.lock);
    return app_t->appcount;
}


/**********************************************************************
* @name      : get_alldocker_with_app
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

int get_alldocker_with_app(MSG_INFO_T *MsgRecv,ALL_APP_T *app_t)
{
    uint8 count=0,i=0,j=0;
    SEQ_STR app;
    uint8 index=0;
    char cmd[1024]={0};
    uint8 *data = MsgRecv->MsgData;
    char buff[20][500];
    char dockerbuff[20][500];
    memset(buff,0,sizeof(buff));
    memset(dockerbuff,0,sizeof(dockerbuff));
    memset(&app,0,sizeof(SEQ_STR));
    count = getshellresult("docker ps -a --format '{{.Names}}'",dockerbuff,20);
    if(count==0)
    {
        return -1;
    }
    index++;
    app.len=data[index++];
    memcpy(app.buff,&data[index],app.len);
    index+=app.len;
/*    if(index!=MsgRecv->MsgLen)
    {
        return -1; 
    }*/
    for(i=0;i<count;i++)
    {
        memset(cmd,0,sizeof(cmd));
        sprintf(cmd,"appm -I -c %s -n %s",dockerbuff[i],app.buff);
        if(getshellresult(cmd,buff,20)>1)
        {
           app_t->appcount++; 
           strcpy((char *)app_t->appinfo[j].containername,dockerbuff[i]);
           strcpy((char *)app_t->appinfo[j].appname,(char *)app.buff);
           j++;
        }
    }
    return index;
}

/**********************************************************************
* @name      : get_allapp_in_docker
* @brief     : 获取容器内所有app的信息
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

uint8 get_allapp_in_docker(MSG_INFO_T *MsgRecv,ALL_APP_T *app_t,char *dockername)
{
    char cmd[1024]={0};
    uint8 count=0,i=0;
    char buff[100][500];
    memset(buff,0,sizeof(buff));
    if(dockername==NULL || app_t == NULL)
    {
        return 1;
    }
    sprintf(cmd,"appm -I -c %s | grep 'App name' | awk '{print $4}'",dockername);
    count = getshellresult(cmd,buff,100);
    if(count<=1)
    {
        return 1;
    }
    for(i=0;i<count;i++)
    {
        strcpy((char *)app_t->appinfo[i].appname,buff[i]);
        strcpy((char *)app_t->appinfo[i].containername,dockername);
    }
    app_t->appcount = count;
    return 0;
}

/**********************************************************************
* @name      : get_global_value
* @brief     : 获取全局信息
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/


void get_global_value(ALL_APP_T *app_t)
{
    uint8 i =0,count = 0,j=0,k=0,fixcount = 0;
    count=app_t->appcount;
    app_t->appinfo[i].appindex=0;
    ALL_APP_INFO_T *apps=NULL;
   // pthread_mutex_lock(&allcontainerinfo.lock);
    if(allcontainerinfo.count==0)
    {
    //  pthread_mutex_unlock(&allcontainerinfo.lock);
        return;
    }
    for(i=0;i<count;i++)
    {
        for(j=0;j<allcontainerinfo.count;j++)
        {
            if(strcmp((char *)app_t->appinfo[i].containername,(char *)allcontainerinfo.containers[j].container.ContainerName)==0)
            {
                apps = &allcontainerinfo.containers[j].apps;
                for(k=0;k<apps->appcount;k++)
                {
                    if(strcmp((char *)app_t->appinfo[i].appname,(char *)apps->appinfo[k].appname)==0)
                    {
                        /*填app信息*/
                        memcpy(&app_t->appinfo[i],&apps->appinfo[k],sizeof(IOT_APP_INFO_T));
                        app_t->appinfo[i].appindex++;
                        fixcount++;  
                        if(fixcount>=10)
                        {
                          //  pthread_mutex_unlock(&allcontainerinfo.lock);
                            return ;                        
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
  //  pthread_mutex_unlock(&allcontainerinfo.lock);
    app_t->appcount = fixcount;
}

/**********************************************************************
* @name      : package_recv_0017
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

uint16 package_recv_0017(ALL_APP_T *app,uint8 *data)
{
    uint16 index=0,i=0,j=0,servernum=0;
    data[index++]=app->appcount;
    for(i=0;i<app->appcount;i++)
    {
       data[index++]= app->appinfo[i].appindex;
       data[index++]= strlen((char *)app->appinfo[i].appname);
       strcpy((char *)&data[index],(char *)app->appinfo[i].appname);
       index+=strlen((char *)app->appinfo[i].appname);
       data[index++]= strlen((char *)app->appinfo[i].containername);
       strcpy((char *)&data[index],(char *)app->appinfo[i].containername);
       index+=strlen((char *)app->appinfo[i].containername);
       data[index++]= strlen((char *)app->appinfo[i].appversion);
       strcpy((char *)&data[index],(char *)app->appinfo[i].appversion);
       index+=strlen((char *)app->appinfo[i].appversion);
       data[index++]= strlen((char *)app->appinfo[i].apphash);
       strcpy((char *)&data[index],(char *)app->appinfo[i].apphash);
       index+=strlen((char *)app->appinfo[i].apphash);
       data[index++] = app->appinfo[i].servicecount;
       servernum = app->appinfo[i].servicecount;
       for(j=0;j<servernum;j++)
       {
           data[index++] = app->appinfo[i].serviceinfo[j].serviceindex;
           data[index++]= strlen((char *)app->appinfo[i].serviceinfo[j].servicename);
           strcpy((char *)&data[index],(char *)app->appinfo[i].serviceinfo[j].servicename);
           index+=strlen((char *)app->appinfo[i].serviceinfo[j].servicename);
           data[index++] = app->appinfo[i].serviceinfo[j].serviceenable;
           data[index++] = app->appinfo[i].serviceinfo[j].servicestate;
           memcpy_r(&data[index],&app->appinfo[i].serviceinfo[j].cpurate,sizeof(uint16));
           index+=sizeof(uint16);
           memcpy_r(&data[index],&app->appinfo[i].serviceinfo[j].memrate,sizeof(uint16));
           index+=sizeof(uint16);
           memcpy_r(&data[index],&app->appinfo[i].serviceinfo[j].starttime.year,sizeof(uint16));
           index+=sizeof(uint16);
           data[index++]=app->appinfo[i].serviceinfo[j].starttime.month;
           data[index++]=app->appinfo[i].serviceinfo[j].starttime.day;
           data[index++]=app->appinfo[i].serviceinfo[j].starttime.hour;
           data[index++]=app->appinfo[i].serviceinfo[j].starttime.minute;
           data[index++]=app->appinfo[i].serviceinfo[j].starttime.second;
       }
       data[index++]=app->appinfo[i].cpucore;
       memcpy_r(&data[index],&app->appinfo[i].cpuLimit,sizeof(uint16));
       index+=sizeof(uint16);
       memcpy_r(&data[index],&app->appinfo[i].memLval,sizeof(uint16));
       index+=sizeof(uint16);
       memcpy_r(&data[index],&app->appinfo[i].memLimit,sizeof(uint16));
       index+=sizeof(uint16);
    }
    return index;
}
/**********************************************************************
* @name      : docker_exec_0017
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0017(MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint8 *data = MsgRecv->MsgData;
    uint16 msglen =0;
    uint8 index=0,ret=0;
    uint8 recvbuff[2048]={0};
    ALL_APP_T app_info;
    SEQ_STR app;
    SEQ_STR docker;
    memset(&app,0,sizeof(SEQ_STR));
    memset(&docker,0,sizeof(SEQ_STR));
    memset(&app_info,0,sizeof(ALL_APP_T));
    if(data[index]==0 && data[index+1]==0)
    {
        //没有容器也没有app 全部的APP
        get_all_app(&app_info);
    }
    else if(data[index]==0 && data[index+1]!=0)
    {
        /*检查所有容器内这个名字的app*/
        ret  = get_alldocker_with_app(MsgRecv,&app_info);
        if(ret == -1)
        {
            recvbuff[0]=0;
            packge_report_msg(MsgRecv,recvbuff,1); 
        }
        else
        {
            index = ret;
        }
        
    }
    else
    {
        docker.len = data[index++];
        memcpy(docker.buff,&data[index],docker.len);
        index+=docker.len;
        if(data[index]==0)
        {
            ret = get_allapp_in_docker(MsgRecv,&app_info,(char *)docker.buff);
            if(ret == 1)
            {
                recvbuff[0]=0;
                packge_report_msg(MsgRecv,recvbuff,1); 
            }
        }
        else
        {
            app.len = data[index++];
            memcpy(app.buff,&data[index],app.len);
            index+=app.len;
            app_info.appcount = 1;
            memcpy(app_info.appinfo[0].containername,docker.buff,docker.len);
            memcpy(app_info.appinfo[0].appname,app.buff,app.len);
        }
    }
    /*获取相应的消息然后组报文*/
/*    if(index!=MsgRecv->MsgLen)
    {
        MQTTMSG_FMT_DEBUG("报文解析有误 index %d msglen %d\n",index,MsgRecv->MsgLen);
    }*/
    get_global_value(&app_info);
    msglen = package_recv_0017(&app_info,recvbuff);
    packge_report_msg(MsgRecv,recvbuff,msglen);
    
}

/**********************************************************************
* @name      : docker_exec_0016
* @brief     ：消息App操作命令执行
* @param[in] ：MSG_INFO_T *MsgRecv 消息,int MSGLen 长度
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0016(MSG_INFO_T *MsgRecv,int MSGLen)
{
    iot_mqtt_0016 app_actinfo;
    char buff[10][500] = {0};
    char cmd[1024] = {0};
    //char errmess[500] = {0};
    BOOL result=TRUE;
    uint8 result_buff[100]={0};
    memset(&app_actinfo,0,sizeof(iot_mqtt_0016));
    uint8 offset=0;
    app_actinfo.docker.len = MsgRecv->MsgData[offset++]; //容器名称长度
    if(app_actinfo.docker.len>100)
        return ;
    memcpy(app_actinfo.docker.buff,&MsgRecv->MsgData[offset],app_actinfo.docker.len);
    offset+=app_actinfo.docker.len;
    app_actinfo.app.len = MsgRecv->MsgData[offset++];
    if(app_actinfo.app.len>100)
        return ;
    memcpy(app_actinfo.app.buff,&MsgRecv->MsgData[offset],app_actinfo.app.len);
    offset+=app_actinfo.app.len;
    app_actinfo.app_config_t.cpucore = MsgRecv->MsgData[offset++];
    memcpy_r(&app_actinfo.app_config_t.cpuLimit,&MsgRecv->MsgData[offset],sizeof(uint16));
    offset+=sizeof(uint16);
    memcpy_r(&app_actinfo.app_config_t.mem_limit,&MsgRecv->MsgData[offset],sizeof(uint16));
    offset+=sizeof(uint16);
    memcpy_r(&app_actinfo.app_config_t.memLimit,&MsgRecv->MsgData[offset],sizeof(uint16));
    offset+=sizeof(uint16);
    /*
    先检查app是否正在运行
    */
  //  sprintf(cmd,"appm -I -c %s -n %s | grep running",app_actinfo.docker.buff,app_actinfo.app.buff);
   // MQTTMSG_FMT_DEBUG("执行App安装 CMD : %s\n",cmd);
   // if(getshellresult(cmd,buff,10)==0)
   // {
   //     result = FALSE;
   // }
  //  else
  //  {
  //  MQTTMSG_FMT_DEBUG("App 存在 设置参数\n");
    memset(cmd,0,1024);
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"appm -t -cpu %d -mem %d -c %s -n %s | grep success ",app_actinfo.app_config_t.cpuLimit,app_actinfo.app_config_t.memLimit,app_actinfo.docker.buff,app_actinfo.app.buff);
#ifdef PRODUCT_CCU
	getshellresult(cmd,buff,10);
#else
	if(getshellresult(cmd,buff,10)==0)
    {
        result = FALSE;           
    }
#endif

    MQTTMSG_FMT_DEBUG("App 参数设置成功 cmd %s\n",cmd);
 //   }
    
    result_buff[0]=result;
    packge_report_msg(MsgRecv,result_buff,1);  //应答
    sleep(3);
    app_action_notice(&app_actinfo.docker,&app_actinfo.app,5,result);
}

/**********************************************************************
* @name      : heart_beat
* @brief     ：给系统管理器回复心跳
* @param[in] ：uint16 index 第几个
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

void heart_beat(uint16 index)
{
    uint8 payload[200] = {0};
    uint8 offset = 0;
    uint32 value = 12;
    uint8 SendBuf[MAX_MSG_LEN] = {0};
    uint16 SendLen;
    MSG_INFO_T *MsgSend = NULL;

    char topic[100] ;
    memset(topic,0x00,sizeof(topic));
    sprintf(topic,"%s/%s",IOT_APP_NAME_M,"M-smiOS");
    memcpy_r(payload, (uint8*)&value, 4);
    offset+=4;

    MsgSend = (MSG_INFO_T*)malloc(sizeof(MSG_INFO_T)+4);
    memset(MsgSend,0x00,sizeof(MSG_INFO_T)+4);
    MsgSend->MsgRPM = 0;
    MsgSend->MsgIndex = index;
    MsgSend->MsgPRIORITY = 1;
    MsgSend->MsgLabel = 1;
    MsgSend->IID = 0x0000;
    MsgSend->IOP = 0x0010;
    
    MsgSend->SourLen = strlen(IOT_APP_NAME_M);
    memcpy(MsgSend->SourAddr,IOT_APP_NAME_M,MsgSend->SourLen);
    MsgSend->DestLen = strlen("M-smiOS");
    memcpy(MsgSend->DestAddr,"M-smiOS",MsgSend->DestLen );
    MsgSend->MsgLen = offset;
    memcpy(MsgSend->MsgData,payload,offset);
    SendLen = msg_mqtt_package(MsgSend,MAX_MSG_LEN,SendBuf);
    //MQTTMSG_FMT_DEBUG("heart_beat  \n");
    mqtt_client_send_msg(i_MqttClient,topic,0,SendBuf, SendLen);
    free(MsgSend);
   
}

/**********************************************************************
* @name      : msg_mqtt_split
* @brief     ：接受的消息解析成指定格式
* @param[in] ：uint8 *pBuf           接受到的报文
               uint16 bufLen         报文长度
* @param[out]：MSG_INFO_T **ppMsgOut 解析出来的消息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-10-29
* @Update    :
**********************************************************************/
int msg_mqtt_split(uint8 *pBuf, uint16 bufLen,uint32 ipAddr, uint16 port, MSG_INFO_T **ppMsgOut)
{
    uint8  *pTemp = pBuf;
    uint8   offset = 0;
    uint16  tempLen = 0;
    MSG_INFO_T tMsgTemp;
    MSG_INFO_T *ptNewMsg = NULL;
    
    if(NULL == pBuf || NULL == ppMsgOut || 0 == bufLen)
    {
        return -1;
    }

    memset(&tMsgTemp, 0, sizeof(MSG_INFO_T));

    tMsgTemp.IP = ipAddr;
    tMsgTemp.port = port;
    
    /* PRM */
    tMsgTemp.MsgRPM = (*pTemp) & 0x01;

    /* PRIORITY */
    tMsgTemp.MsgPRIORITY = ((*pTemp)>>1) & 0x7f;

    /* INDEX */
    pTemp++;
#if IS_LITTLE_ENDIAN
    tMsgTemp.MsgIndex = *((uint16 *)pTemp);
#else
    tMsgTemp.MsgIndex = ntohs(*((uint16 *)pTemp));
#endif

    /* LABEL */
    pTemp += 2;
#if IS_LITTLE_ENDIAN
    tMsgTemp.MsgLabel = *((uint16 *)pTemp);
#else
    tMsgTemp.MsgLabel = ntohs(*((uint16 *)pTemp));
#endif
    /* SOURCE */
    pTemp += 2;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
//        MPRINT("SOURCE too long %d\n", tempLen);
        return -1;
    }
    memcpy(tMsgTemp.SourAddr, pTemp, tempLen);
    tMsgTemp.SourLen = tempLen;

    /* DESTINATION */
    pTemp += tempLen + 1;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
      //  MPRINT("DESTINATION too long %d\n", tempLen);
        return -1;
    }
    memcpy(tMsgTemp.DestAddr, pTemp, tempLen);
    tMsgTemp.DestLen = tempLen;

    /* TAG */
    pTemp += tempLen + 1;

#if IS_LITTLE_ENDIAN
    tMsgTemp.IID = *((uint16*)(pTemp+2));
    tMsgTemp.IOP = *((uint16*)pTemp);
#else
    tMsgTemp.IID = ((uint16)*pTemp)*256 + *(pTemp+1);
    tMsgTemp.IOP = ((uint16)*(pTemp+2))*256 + *(pTemp+3);
#endif
    /* MsgLen */
    pTemp += 4;
    tMsgTemp.MsgLen = get_len_offset(pTemp, &offset);
    pTemp += offset;
    
    /* 拷贝 */
    ptNewMsg = (MSG_INFO_T *)malloc(sizeof(MSG_INFO_T) + tMsgTemp.MsgLen);
    if(NULL == ptNewMsg)
    {
       // MPRINT("malloc failed,size %d\n", (uint32)sizeof(MSG_INFO_T) + tMsgTemp.MsgLen);
        return -1;
    }
    
    memcpy(ptNewMsg, &tMsgTemp, sizeof(MSG_INFO_T));
    memcpy(ptNewMsg->MsgData, pTemp, tMsgTemp.MsgLen);

    *ppMsgOut = ptNewMsg;

    return 0;
}

void check_system_upgrade()
{
    sys_update_info sys_info;
    memset(&sys_info,0,sizeof(sys_info));
    char filePath[100]={0};
    char cmd[150] = {0};
    char buff[10][500] = {0};
    uint8 ret=0;
    BOOL result = TRUE;
    sprintf(filePath,"%s%s",PrivateDir,"SYSUPDATE");
    if(access(filePath,R_OK)==0)
    {
        read_pdata_xram(filePath,(char *)&sys_info,0,sizeof(sys_update_info));
        switch(sys_info.updatetype)
        {
            case 0:
            {
              ret = getshellresult("version -d",buff,10);
              if(ret!=3)
                result = FALSE;
            }break;
            case 1:
            {
              ret = getshellresult("version -fs",buff,10);
              if(ret!=3)
                result = FALSE;
            }break;
            case 2:
            {
              ret = getshellresult("version -k",buff,10);
              if(ret!=3)
                result = FALSE;
            }break;
            case 3:
            {
              ret = getshellresult("version -k",buff,10);
              if(ret!=3)
                result = FALSE;
            }
            default:
            break;
        }
        /*执行命令 判断返回值 发送升级通知*/ 
        if(strcmp(buff[0],sys_info.updatever)==0)
            result = TRUE;
        packge_docker_notice_0006(sys_info.updatetype,result);
        sprintf(cmd,"rm -f %s",filePath);
        system(cmd);
    }
}
/**********************************************************************
* @name      : iot_mqtt_proc
* @brief     ：Mqtt线程主函数
* @param[in] ：void *pArg, void *pPublic, void *pPrivate
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

int iot_mqtt_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int ret = 0;
    iot_mqtt_type *ptData = pPrivate;
    int                 SendLen =0;
    uint8               SendBuf[5120]={0};
   // char topic[500] = {0};
    MSG_INFO_T *MsgRecv = NULL;;
    int RecvLen = 0;
    uint8 RecvBuf[8192] = {0};
    uint32 MSGLen;
    uint8 *MSGBuf= NULL;
    if(ptData->status == MSG_INIT)
        ptData->status = MSG_RECV;
    switch(ptData->status)
    {
        case MSG_LOGIN:
        {
            if(ptData->smiregtype== 1)
            {
                ptData->status = MSG_RECV;
                break;
            }
            taskmng_register((IOT_APPDATA_T *)pPublic);
            ptData->status = MSG_RECV;
        }
            break;
        case MSG_RECV:
        {
            memset(RecvBuf,0x00,sizeof(RecvBuf));
            RecvLen = 0;
            RecvLen = mqtt_read(RecvBuf);
            if(RecvLen>0)
            {   
                ret = msg_mqtt_split(RecvBuf,RecvLen,0,0,&MsgRecv);
                if(ret<0)
                    break;
                 //增加防护 以防出现死循环
                if(strcmp(MsgRecv->SourAddr,IOT_APP_NAME_M)==0)
                    break;
                if(MsgRecv->IID !=0x0000)
                {
                    MQTTMSG_FMT_DEBUG("recevice Mqtt message source %s IID %04x IOP %04x \n",MsgRecv->SourAddr,MsgRecv->IID,MsgRecv->IOP);
                    MQTTMSG_BUF_DEBUG(RecvBuf, RecvLen,"");
                }

                RecvLen = sizeof(MSG_INFO_T)+MsgRecv->MsgLen;
                switch (MsgRecv->IID)
                    {
                        case 0x0000:
                        {
                            if(MsgRecv->IOP == 0x0010)
                            {
                                heart_beat(MsgRecv->MsgIndex);
                            }
                        }break;
                        case 0x0001:
                        {   
                            if(MsgRecv->IOP == 0x0010)
                            {
                                MQTTMSG_FMT_DEBUG("smios register sucessful \n");
                                BASE_TASK_SET_OK(pArg);
                                ptData->smiregtype= 1;
                            } 
                        }break;
                        case 0x000B:
                        {
                            switch (MsgRecv->IOP)
                            {
                                case 0x0010:
                                {
                                  docker_exec_0010(MsgRecv,RecvLen); 
                                }break;
                                case 0x0011:            //容器操作
                                {
                                  docker_exec_0011(MsgRecv,RecvLen);
                                }break;
                                case 0x0012:
                                {
                                  docker_exec_0012(MsgRecv,RecvLen);
                                }break;
                                case 0x0013:
                                {
                                  docker_exec_0013(MsgRecv,RecvLen);  
                                }break;
                                case 0x0014:
                                {
                                  docker_exec_0014(pPublic,MsgRecv,RecvLen);
                                }break;
                                case 0x0015:
                                {
                                 docker_exec_0015(MsgRecv,RecvLen);   
                                }break;
                                case 0x0016:
                                {
                                  docker_exec_0016(MsgRecv,RecvLen);   
                                }break;
                                case 0x0017:   
                                {
                                  docker_exec_0017(MsgRecv,RecvLen);   
                                }break;
                                case 0x0018:
                                {
                                        //许可配置
                                   docker_exec_0018(MsgRecv,RecvLen);  
                                }break;
                                case 0x0019:
                                {
                                    //许可查询 
                                    docker_exec_0019(MsgRecv,RecvLen);
                                }break;
                                case 0x001A:
                                {
                                    //目前没有命令 系统升级
                                    docker_exec_001A(MsgRecv,RecvLen);
                                }break;
                                case 0x001B:
                                {
                                   docker_exec_001B(MsgRecv,RecvLen); 
                                }break;
                                case 0x001C:
                                {
                                    // 日志信息查询  没做
                                    docker_exec_001C(MsgRecv,RecvLen); 
                                }break;
                                case 0x001D:
                                {
                                    //日志打包     日志类型没做
                                    docker_exec_001D(MsgRecv,RecvLen); 
                                }break;
                                case 0x001E:
                                {
                                    //日志删除     日志类型没做
                                    docker_exec_001E(MsgRecv,RecvLen); 
                                }break;
                                case 0x001F:
                                {
                                    //系统配置参数     
                                    docker_exec_001F((IOT_APPDATA_T *)pPublic,MsgRecv,RecvLen); 
                                }break;
                            }
                        }break;
                    }

                free(MsgRecv);
                MsgRecv = NULL;
                usleep(2000);
            }
            else
            {
                usleep(10000);
            }
            if(ptData->smiregtype == 1)
                ptData->status = MSG_SEND;
            else
                ptData->status = MSG_LOGIN;
        }
        break;
        case MSG_SEND:
        {
            memset(SendBuf,0,sizeof(SendBuf));
            SendLen = taskmng_plc_queue_read(&LockWarringMsg, SendBuf);
            if(SendLen>0)
            {   
                //调用消息接口发送消息 
                MSG_INFO_T* msg  = (MSG_INFO_T*)SendBuf;
                char topic[1000]={0};
                sprintf(topic,"%s/%s/%04X/%04X/",msg->SourAddr,msg->DestAddr,msg->IID,msg->IOP);
                //sprintf(topic,"%s/%s/%04x/%04x",msg->SourAddr,msg->DestAddr,msg->IID,msg->IOP);
                //组消息
                MQTTMSG_FMT_DEBUG("存在通知事件上报 topic %s IID %04x IOP %04x\n",topic,msg->IID,msg->IOP);
                MSGLen = iot_appmsg_package(msg,&MSGBuf);            
                MQTTMSG_BUF_DEBUG(MSGBuf, MSGLen,"");
                mqtt_topic_write(MSGBuf, MSGLen,topic);
                memset(SendBuf,0x00,sizeof(SendBuf));
                free(MSGBuf);
                MSGBuf = NULL;
            }
            memset(SendBuf,0,sizeof(SendBuf));
            SendLen = taskmng_plc_queue_read(&LockRecvMsg, SendBuf);
            if(SendLen>0)
            {
                //调用消息接口发送消息
                MSG_INFO_T* msg  = (MSG_INFO_T*)SendBuf;
                char topic[1000]={0};
                sprintf(topic,"%s/%s",msg->SourAddr,msg->DestAddr);
                //组消息
                MQTTMSG_FMT_DEBUG("topic %s IID %04x IOP %04x\n",topic,msg->IID,msg->IOP);
                MSGLen = iot_appmsg_package(msg,&MSGBuf);    
                MQTTMSG_BUF_DEBUG(MSGBuf, MSGLen,"");
                mqtt_topic_write(MSGBuf, MSGLen,topic);
                memset(SendBuf,0x00,sizeof(SendBuf));
                free(MSGBuf);
                MSGBuf = NULL;
            }
            ptData->status = MSG_RECV;
        }
        break;
        default:
        ptData->status = MSG_INIT;
        break;
    }
    return 1000;
}


