/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��·���mqtt
* @date��    2020-6-30
* @history�� 
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
* @brief     : ͨ�ú���ȥ���ַ����е��ض��ַ�
* @param[in] : str Դ�ַ��� ch ��Ҫȥ�����ַ�
* @param[out]: 
* @return    : ȥ������ַ���
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     ��������Ϣ����
* @param[in] ��char *pTopic              ָ������, NULLʹ��Ĭ������
               MESSAGE_INFO_T *pMsgInfo  ������Ϣ
               int qos                   ��Ϣ����
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-1-9
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

    /* ���˶���MQTT */
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
* ��������: taskmng_register
* ��������: ע�ᵽϵͳ�������
* �������: 
* �������: 
* �� �� ֵ: 
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

    /* ���̺� */
    appmsg_memcpy_r(request, &pid, sizeof(pid_t));
    offset += 4;

    /* APP���� */
    len = strlen(IOT_APP_NAME_M);
    request[offset++] = len;
    memcpy(&request[offset], IOT_APP_NAME_M, len);
    offset += len;

    /* �����¼� */
    request[offset++] = 0;

    /* �汾��Ϣ */
    request[offset++] = strlen(g_AppVer);
    memcpy(request + offset, g_AppVer, strlen(g_AppVer));
    offset += strlen(g_AppVer);

    /* �������� */
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
* @brief     ��MQTT�̳߳�ʼ��
* @param[in] ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2022-5-26
* @Update    :
**********************************************************************/

int iot_mqttmsg_init(void *pPublic, void **ppPrivate)
{
    IOT_APPDATA_T *pdata = pPublic;
    iot_mqtt_type *ptData = NULL;
    ptData    = (iot_mqtt_type *)malloc(sizeof(iot_mqtt_type));
    if(ptData == NULL)
    {
        MQTTMSG_FMT_DEBUG("��ʼ��ʧ��\n");
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
* @brief     ������Ԫ�س���
* @param[in] ��const uint8 *indata      ����Buffer
* @param[out]��const uint8 *indata      ����Buffer
* @return    ������
* @Create    : 
* @Date      ��2020-1-3
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
* @brief     ���������Ƶı���ʱ��
* @param[in] ��char *ptime        ���� 2020/5/22 13:19:20
* @param[out]��Date_Time_S *pTime ����ʱ��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     ��ָ����ʽ��Ϣ��װ��
* @param[in] ��MESSAGE_INFO_T *pMsgInfo  �ṹ��Ϣ
* @param[out]��uint8 **ppBuf             ��װ��Ϣ
* @return    ����Ϣ����
* @Create    : 
* @Date      ��2021-7-7
* @Update    :
**********************************************************************/
uint16 iot_appmsg_package(MSG_INFO_T *pMsgInfo, uint8 **ppBuf)
{
    uint16   bufLen = 0;
    uint8 lenBuf[3] = {0};
    uint8    offset = 0;
    uint8  *pMsgBuf = NULL;
    uint8    *pTemp = NULL;
    
    bufLen = 1 + 2 + 2 + pMsgInfo->SourLen + 1 + pMsgInfo->DestLen + 1 + 4;   //����head��Tag����
    /* ����MSG's-Length ����  */
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
* @brief     ��ָ����ʽ��Ϣ��װ��
* @param[in] ��MSG_INFO_T *pMsgInfo  �ṹ��Ϣ
               bufLenMax             buf��󳤶�
* @param[out]��uint8 *buf            ��װ��Ϣ
* @return    ����Ϣ����
* @Create    : 
* @Date      ��2019-10-29
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
    
    bufLen = 1 + 2 + 2 + strlen(pMsgInfo->SourAddr) + 1 + strlen(pMsgInfo->DestAddr) + 1 + 4;   //����head��Tag����

    /* ����MSG's-Length ����  */
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
    IOT_FMT_DEBUG("����ϵͳ����֪ͨ�¼�\n");
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
* @brief     ����0001��������֪ͨ����
* @param[in] ��uint8 type   ֪ͨ��� dockername ������ {��װ��0����������1����ж�أ�2����������4����ֹͣ��4�������ò�����5��}��
* @param[out]��BOOL result* ���
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
* @Update    :
**********************************************************************/

void packge_docker_notice_0001(uint8 type,BOOL result,char *dockername,uint16 len)
{
    uint16 index = 0;
    uint16 tmplen = 0;
    uint8 buff[255]={0};
    IOT_FMT_DEBUG("���������澯�¼� ֪ͨ��� %d ��� %d \n",type,!result);
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
* @brief     ����ظ�����
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ���� buff �ظ��ı��� msglen �ظ����ĳ���
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
    //ֱ�ӷ���ȷ��֡
    char topic[1000]={0};
    sprintf(topic,"%s/%s",MsgSend->SourAddr,MsgSend->DestAddr);
    //����Ϣ
    MQTTMSG_FMT_DEBUG("topic %s IID %04x IOP %04x\n",topic,MsgSend->IID,MsgSend->IOP);
    MSGLen = iot_appmsg_package(MsgSend,&MSGBuf);    
    MQTTMSG_BUF_DEBUG(MSGBuf, MSGLen,"");
    mqtt_topic_write(MSGBuf, MSGLen,topic); 
}



/*******************************************************************************
* ��������: container_exec_0012
* ��������: ִ��0012��������
* ���������docker_config_T     �������ýṹ��
*       
* ���������
* �ڲ�����: ���ж������Ƿ�ִ�У��ж�����ִ�вſ��Խ��ж�̬���ò���
* �� �� ֵ: uint8                0 ��ȷ ��������
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
        MQTTMSG_FMT_DEBUG("����ִ��ʧ�� cmd %s \n",cmd);
        print_exec_faild(ret,buff);
        return 1;
    }
         
    memset(cmd,0x00,sizeof(cmd));
    //Ŀǰֻ֧��һ��һ�������������
#ifdef PRODUCT_CCU
    sprintf(cmd,"container config %s -cpu %d -mem %d -disk %d | grep Success",tmpconfig->dockername,tmpconfig->iot_docker_config_T.cpu_core,tmpconfig->iot_docker_config_T.mem_limitval,\
       tmpconfig->iot_docker_config_T.disk_limitval);
   // IOT_FMT_DEBUG("exec cmd %s\n",cmd);
    memset(buff,0,sizeof(buff));
   //MQTTMSG_FMT_DEBUG("ִ��������װ CMD: %s\n",cmd);
   ret = getshellresult(cmd,buff,10);
    if(ret ==0)
    {
        MQTTMSG_FMT_DEBUG("��������ʧ�� CMD: %s\n",cmd);
      //  print_exec_faild(ret,buff);
        return 1;
    }
         
    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"container monitor -t cpu %d %s | grep Success",tmpconfig->iot_docker_config_T.cpu_limit/100,tmpconfig->dockername);
  //  IOT_FMT_DEBUG("exec cmd %s\n",cmd);
 // MQTTMSG_FMT_DEBUG("ִ��������װ CMD: %s\n",cmd);
    ret = getshellresult(cmd,buff,10);
//    if(ret ==0)
//    {
//        MQTTMSG_FMT_DEBUG("��������ʧ�� CMD: %s\n",cmd);
//  //      print_exec_faild(ret,buff);
//        return 1;
//    }
        
    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"container monitor -t  memory %d %s | grep Success",tmpconfig->iot_docker_config_T.mem_limit/100,tmpconfig->dockername);
   // MQTTMSG_FMT_DEBUG("ִ��������װ CMD: %s\n",cmd);
   ret = getshellresult(cmd,buff,10);
//    if(ret ==0)
//    {
//        MQTTMSG_FMT_DEBUG("��������ʧ�� CMD: %s\n",cmd);
// //       print_exec_faild(ret,buff);
//        return 1;
//    }
        
    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    
    sprintf(cmd,"container monitor -t storage %d %s | grep Success",tmpconfig->iot_docker_config_T.disk_limit/100,tmpconfig->dockername);
  //  MQTTMSG_FMT_DEBUG("ִ��������װ CMD: %s\n",cmd);
    ret = getshellresult(cmd,buff,10);
//    if(ret ==0)
//    {
//        MQTTMSG_FMT_DEBUG("��������ʧ�� CMD: %s\n",cmd);
// //       print_exec_faild(ret,buff);
//        return 1;
//    }
#else
	sprintf(cmd,"container config %s -cpu %d -mem %d -disk %d | grep success",tmpconfig->dockername,tmpconfig->iot_docker_config_T.cpu_core,tmpconfig->iot_docker_config_T.mem_limitval,\
       tmpconfig->iot_docker_config_T.disk_limitval);
   // IOT_FMT_DEBUG("exec cmd %s\n",cmd);
    memset(buff,0,sizeof(buff));
   //MQTTMSG_FMT_DEBUG("ִ��������װ CMD: %s\n",cmd);
   ret = getshellresult(cmd,buff,10);
    if(ret ==0)
    {
        MQTTMSG_FMT_DEBUG("��������ʧ�� CMD: %s\n",cmd);
      //  print_exec_faild(ret,buff);
        return 1;
    }
         
    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"container monitor -t cpu %d %s | grep success",tmpconfig->iot_docker_config_T.cpu_limit/100,tmpconfig->dockername);
  //  IOT_FMT_DEBUG("exec cmd %s\n",cmd);
 // MQTTMSG_FMT_DEBUG("ִ��������װ CMD: %s\n",cmd);
    ret = getshellresult(cmd,buff,10);
    if(ret ==0)
    {
        MQTTMSG_FMT_DEBUG("��������ʧ�� CMD: %s\n",cmd);
  //      print_exec_faild(ret,buff);
        return 1;
    }
        
    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    sprintf(cmd,"container monitor -t  memory %d %s | grep success",tmpconfig->iot_docker_config_T.mem_limit/100,tmpconfig->dockername);
   // MQTTMSG_FMT_DEBUG("ִ��������װ CMD: %s\n",cmd);
   ret = getshellresult(cmd,buff,10);
    if(ret ==0)
    {
        MQTTMSG_FMT_DEBUG("��������ʧ�� CMD: %s\n",cmd);
 //       print_exec_faild(ret,buff);
        return 1;
    }
        
    memset(cmd,0x00,sizeof(cmd));
    memset(buff,0,sizeof(buff));
    
    sprintf(cmd,"container monitor -t storage %d %s | grep success",tmpconfig->iot_docker_config_T.disk_limit/100,tmpconfig->dockername);
  //  MQTTMSG_FMT_DEBUG("ִ��������װ CMD: %s\n",cmd);
    ret = getshellresult(cmd,buff,10);
    if(ret ==0)
    {
        MQTTMSG_FMT_DEBUG("��������ʧ�� CMD: %s\n",cmd);
 //       print_exec_faild(ret,buff);
        return 1;
    }
#endif
    
        
    /*���ص��������ļ��� 	array visible-string,
    Ӧ�ô洢�ļ�·��      visible-string,
    ӳ���Ӳ���ӿ�     	array visible-string,
    �˿���Դ���ò���      visible-string
    ��ʱ����
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
    index+=tmp;           //�˿���Դ���ò���
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
            //ȫ������
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
* @brief     ��������ѯ��Ϣִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ���� int MSGLen ��С
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     ������������Ϣִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ���� int MSGLen ��С
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     ������������Ϣִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ���� int MSGLen ��С
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
    //�ܳ���
    tmplen = payload[index++];
    memcpy(dockername,&payload[index],tmplen);
    index+=tmplen;
    type = payload[index++];
    if(tmplen == 0)  //û��������
    {
        MQTTMSG_FMT_DEBUG("��Ϣ����û��������\n");
        result = FALSE;
    }
#ifdef PRODUCT_CCU
    switch(type)
    {
        case 0:                                //��������
        {
            
            msg_type = 3;
            MQTTMSG_FMT_DEBUG("�յ������������ %s\n",dockername);
            sprintf(cmd,"container start %s | grep SUCCESS",dockername);
            if(getshellresult(cmd,buff,5)!=1)
            {
                result = FALSE;
            }                  
            MQTTMSG_FMT_DEBUG("���������ɹ�\n",dockername);
        }break;
        case 1:                             //ֹͣ����
        {
            msg_type = 4;
            MQTTMSG_FMT_DEBUG("�յ����ֹͣ���� %s\n",dockername);
            sprintf(cmd,"container stop %s | grep SUCCESS",dockername);
            if(getshellresult(cmd,buff,5)!=1)
            {
                result = FALSE;
            }
            MQTTMSG_FMT_DEBUG("ֹͣ�����ɹ�\n");
        }break;
        case 2:                         //ж������ ��ֹͣ��ж�� ֹͣ�Ѿ�ֹͣ�������������
        {
            msg_type = 2;
            MQTTMSG_FMT_DEBUG("�յ����ж������ %s\n",dockername);
            sprintf(cmd,"container uninstall %s | grep SUCCESS",dockername);
            if(getshellresult(cmd,buff,5)!=1)
            {
                result = FALSE;
            }
            MQTTMSG_FMT_DEBUG("�յ����ж�������ɹ� %s\n",dockername);
        }break;
        default:
            break;
    } 
#else
	switch(type)
    {
        case 0:                                //��������
        {
            
            msg_type = 3;
            MQTTMSG_FMT_DEBUG("�յ������������ %s\n",dockername);
            sprintf(cmd,"container start %s | grep success",dockername);
            if(getshellresult(cmd,buff,5)!=1)
            {
                result = FALSE;
            }                  
            MQTTMSG_FMT_DEBUG("���������ɹ�\n",dockername);
        }break;
        case 1:                             //ֹͣ����
        {
            msg_type = 4;
            MQTTMSG_FMT_DEBUG("�յ����ֹͣ���� %s\n",dockername);
            sprintf(cmd,"container stop %s | grep success",dockername);
            if(getshellresult(cmd,buff,5)!=1)
            {
                result = FALSE;
            }
            MQTTMSG_FMT_DEBUG("ֹͣ�����ɹ�\n");
        }break;
        case 2:                         //ж������ ��ֹͣ��ж�� ֹͣ�Ѿ�ֹͣ�������������
        {
            msg_type = 2;
            MQTTMSG_FMT_DEBUG("�յ����ж������ %s\n",dockername);
            sprintf(cmd,"container uninstall %s | grep success",dockername);
            if(getshellresult(cmd,buff,5)!=2)
            {
                result = FALSE;
            }
            MQTTMSG_FMT_DEBUG("�յ����ж�������ɹ� %s\n",dockername);
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
* @brief     ��������װ��Ϣִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ���� int MSGLen ��С
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
    if(AppSec==1) //��app
    {
        app.len = data[index++];
        memcpy(app.buff,&data[index],app.len);
        index+=app.len;
    }
    if(data[index++] == 1) //����������
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
    con_info.iot_docker_config_T.strogefilelen = data[index++]; //�洢�ļ���
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
        if(index!=msglen)   //������ȷ���ж�
        {
            MQTTMSG_FMT_DEBUG("index %d msglen %d \n",index,msglen);
            result = FALSE;
            break;
        }
        //���ж��ն����Ƿ�������������������ж��
        sprintf(cmd,"docker ps | grep %s",con_info.dockername);
        ret = getshellresult(cmd,buff,20);
        memset(cmd,0,sizeof(cmd));
        memset(buff,0,sizeof(buff));
        if(ret==1)
        {
            MQTTMSG_FMT_DEBUG("��鵽��ͬ��������ж��\n");
            sprintf(cmd,"container uninstall %s | grep failed",con_info.dockername);
            ret = getshellresult(cmd,buff,20);
            {
                if (ret !=0 )
                {
                    MQTTMSG_FMT_DEBUG("����ж��ʧ�� CMD: %s\n",cmd);
                    result = FALSE;
                    break;
                }
            }
        }

       // sprintf(cmd,"container uninstall %s",con_info.dockername);

        
        /*ȷ���ļ��Ƿ����*/
        /*�ж�app�ļ��Ƿ����*/

        if(AppInfoSec == 1 )    //app�ļ���Ч��md5ֵ�ж�
        {
            sprintf(pathname,"%s/%s",appinfo.file_info_t.filepath,appinfo.file_info_t.filename);
            if(access(pathname,R_OK)!=0)
            {
                MQTTMSG_FMT_DEBUG("·�������� %s\n",pathname);
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
            if(AppSigSec==1)   //ǩ���ļ���Ч��md5ֵ�ж�
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
        /*������*/
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

       // MQTTMSG_FMT_DEBUG("ִ��������װ CMD: %s\n",cmd);
        ret = getshellresult(cmd,buff,20);
        {
            MQTTMSG_FMT_DEBUG("ִ��������װ���� CMD: %s\n",cmd);
            if(ret < 1)
            {
                MQTTMSG_FMT_DEBUG("ִ��������װʧ�� ret %d CMD: %s\n", ret,cmd);
                //��ӡʧ�ܱ���
                print_exec_faild(ret,buff);
                result = FALSE;
                break;
            }
        }
        /*��֤*/
        memset(cmd,0,sizeof(cmd));
        memset(buff,0,sizeof(buff));
        sprintf(cmd,"docker ps | grep %s",con_info.dockername);
        if(getshellresult(cmd,buff,20)<1)
        {
            MQTTMSG_FMT_DEBUG("����������װʧ�� \n");
            result = FALSE;
            break; 
        }
        /*����APP��ֵ(option)*/
        if(AppSec==1 && AppInfoSec==1)
        {
            //����app��Ϣ��app����Ҫ���ļ�����ȥ��.tar
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

        //    MQTTMSG_FMT_DEBUG("ִ��������װ CMD: %s\n",cmd);
            ret = getshellresult(cmd,buff,20);
//            if (ret == 0)
//            {  
//                MQTTMSG_FMT_DEBUG("��������ʧ�� CMD: %s\n",cmd);
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
    /*ִ�а�װ����*/
    /*���ò�����������app��*/
    /*���ر��ĺͲ����澯*/
    uint8 result_buf[100]={0};
    result_buf[0]=result;
    packge_report_msg(MsgRecv,result_buf,1);
    sleep(2);
    packge_docker_notice_0001(0,result,(char*)con_info.dockername,con_info.dockernamelen);
}

/**********************************************************************
* @name      : sizeAndmd5_judge
* @brief     ���ж��ļ��Ĵ�С��md5ֵ�Ƿ���ȷ
* @param[in] filepath  �ļ�����·�� size �ļ���С md5 md5ֵ
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
        MQTTMSG_FMT_DEBUG("����ִ��ʧ�� ��%s \n",cmd);
        return 1; 
    }
          
    if(atoi(buff[0])!=size)
    {
      MQTTMSG_FMT_DEBUG("��С��һ�� wc %d size %d\n",atoi(buff[0]),size);
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
 //       MQTTMSG_FMT_DEBUG("ת��ʧ�� bufflen %d \n",bitlen);
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
* @brief     ����Ϣ0014App��װ����ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0014(IOT_APPDATA_T *pdata,MSG_INFO_T *MsgRecv,int MSGLen)
{
    uint16 index=0;
    uint8 option = 0,ret = 0,exectype = 0;//0Ϊ��װ
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
    tmplen = MsgRecv->MsgData[index++];     //Ӧ�ð汾��
    memcpy(app_install.app_config_t.app_version,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    tmplen = MsgRecv->MsgData[index++];    //�ļ�������
    memcpy(app_install.app_config_t.file_info_t.filename,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    tmplen = MsgRecv->MsgData[index++];    //�ļ�������
    memcpy(app_install.app_config_t.file_info_t.filetype,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    tmplen = MsgRecv->MsgData[index++];    //�ļ�·��
    memcpy(app_install.app_config_t.file_info_t.filepath,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    memcpy_r(&app_install.app_config_t.file_info_t.filesize,&MsgRecv->MsgData[index],sizeof(uint32));
    index +=sizeof(uint32);                //�ļ���С
    tmplen = MsgRecv->MsgData[index++];    //�ļ��� md5 ֵ 
    memcpy(app_install.app_config_t.file_info_t.filemd5,&MsgRecv->MsgData[index],tmplen);
    index +=tmplen;
    option = MsgRecv->MsgData[index++];
    //�ļ�������ǩ֤��Ϣ  OPTIONAL
    if(option == 1)
    {
        tmplen = MsgRecv->MsgData[index++];    //����֤���ļ�����
        memcpy(app_install.app_config_t.file_info_t.signinfo_t.signname,&MsgRecv->MsgData[index],tmplen);
        index +=tmplen;
        tmplen = MsgRecv->MsgData[index++];    //����֤���ļ�·��
        memcpy(app_install.app_config_t.file_info_t.signinfo_t.signpath,&MsgRecv->MsgData[index],tmplen);
        index +=tmplen;
        memcpy_r(&app_install.app_config_t.file_info_t.signinfo_t.signsize,&MsgRecv->MsgData[index],sizeof(uint32));
        index +=sizeof(uint32); 
        tmplen = MsgRecv->MsgData[index++];    //����֤���ļ���md5ֵ
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
            //У��һ������ǩ���ļ��Ƿ����
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

        /*�ж������ļ��治����*/
        memset(pathname,0,sizeof(pathname));
        sprintf(pathname,"%s/%s",app_install.app_config_t.file_info_t.filepath,app_install.app_config_t.file_info_t.filename);
        if(access(pathname,R_OK)!=0)
        {
            MQTTMSG_FMT_DEBUG("����ļ��Ƿ���� CMD -- ",pathname);
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
        //���жϵ�ǰ�ն��Ƿ���ڸ�������App�����������Ϊ������������Ϊ��װ
        sprintf(filePath,"appm -I -c %s -n %s",app_install.docker.buff,app_install.app.buff); 
        ret = getshellresult(filePath,buff,20);
        if(ret > 1)
        {
            MQTTMSG_FMT_DEBUG("����ͬ��app����Ϊ����\n");
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
            MQTTMSG_FMT_DEBUG("���� %s\n",cmd);
           // getshellresult(cmd, char result [ ] [ 500 , int size)
           system(cmd);
           memset(cmd,0,sizeof(cmd));
           sprintf(cmd,"appm -u -c %s -n %s ",app_install.docker.buff,app_install.app.buff);
           MQTTMSG_FMT_DEBUG("���� %s\n",cmd);
           system(cmd);
#ifdef PRODUCT_CCU
		   memset(cmd,0,sizeof(cmd));
		   sprintf(cmd, "rm -rf /backup/app_back/*");
		   system(cmd);
		   system("sync");
#endif

        }
         //��ָ�װapp
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
        MQTTMSG_FMT_DEBUG("ִ�а�װ���� CMD -- %s\n",pathname);
        uint8 tmp_num = 0;
        do{
            memset(buff,0,sizeof(buff));
            ret = getshellresult(pathname,buff,10);
            if (ret == 0)
            {
                MQTTMSG_FMT_DEBUG("ʧ������ %d \n",tmp_num);
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
        if(exectype == 0)  //�����ò������Ѳ���������
        {
            memset(buff,0,sizeof(buff));
            memset(pathname,0,sizeof(pathname));
            sprintf(pathname,"appm -t -cpu %d -mem %d -c %s -n %s",app_install.app_config_t.app_config_t.cpuLimit,app_install.app_config_t.app_config_t.memLimit,app_install.docker.buff,app_install.app.buff);
            uint8 num_T=0;
            do{
                ret = getshellresult(pathname,buff,3);
                {
                    MQTTMSG_FMT_DEBUG("ִ�в����������� CMD -- %s \n",pathname);
                    if(ret != 0)
                    {
                        MQTTMSG_FMT_DEBUG("��������ʧ�� \n");
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
                MQTTMSG_FMT_DEBUG("��������ʧ�� \n");
                result = FALSE;
                break;
            }
            ret = getshellresult(pathname,buff,3);
            {
                MQTTMSG_FMT_DEBUG("ִ�в����������� CMD -- %s \n",pathname);
                if(ret != 0)
                {
                    MQTTMSG_FMT_DEBUG("��������ʧ�� \n");
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
        MQTTMSG_FMT_DEBUG("ִ�а�װ���� CMD -- %s\n",pathname);
        ret = getshellresult(pathname,buff,10);
        if (ret == 0)
        {
            MQTTMSG_FMT_DEBUG("��װʧ�� \n");
           // print_exec_faild(ret,buff);
            result = FALSE;
            break;
        }
        if(exectype == 0)  //�����ò������Ѳ���������
        {

            
            memset(buff,0,sizeof(buff));
            memset(pathname,0,sizeof(pathname));
            sprintf(pathname,"appm -t -cpu %d -mem %d -c %s -n %s | grep success",app_install.app_config_t.app_config_t.cpuLimit,app_install.app_config_t.app_config_t.memLimit,app_install.docker.buff,app_install.app.buff);
            ret = getshellresult(pathname,buff,3);
            {
                MQTTMSG_FMT_DEBUG("ִ�в����������� CMD -- %s \n",pathname);
                if(ret == 0)
                {
                    MQTTMSG_FMT_DEBUG("��������ʧ�� \n");
                    print_exec_faild(ret,buff);
                    result = FALSE;
                    break;
                }
            }
        }
#endif

        //����У��һ�°�װ�Ƿ���ȷ
        //appm -I -c container -n appname
        memset(pathname,0,sizeof(pathname));
        memset(buff,0,sizeof(buff));
        sprintf(pathname,"appm -I -c %s -n %s",app_install.docker.buff,app_install.app.buff);     
        ret = getshellresult(pathname,buff,10);
        {
            MQTTMSG_FMT_DEBUG("���������װ�Ƿ�ɹ� CMD -- %s \n",pathname);
            if (ret < 1)
            {
                result = FALSE;
                MQTTMSG_FMT_DEBUG("������װʧ��\n");
            }
            
            
        }      
    }while(0);
    if(exectype == 0)
    {
        uint8 tmpbuff[10]={0};
        tmpbuff[0]=result;
        packge_report_msg(MsgRecv,tmpbuff,1);
        sleep(2);
		app_action_notice(&app_install.docker,&app_install.app,exectype,result); //��װ��Ϣ֪ͨ
    }
    else
    {
        pdata->app_u.type_u = 1;
        pdata->app_u.result = result;
        pdata->app_u.appname = app_install.app;
        pdata->app_u.conname = app_install.docker;
       // app_action_notice(&app_install.docker,&app_install.app,1,result); //������Ϣ֪ͨ
    }

}

/**********************************************************************
* @name      : App_comand_exec
* @brief     ��ִ��App��һϵ�в���
* @param[in] ��docker ���� app app ,type��������enum{������0����ֹͣ��1����ж�أ�2����ʹ�ܣ�3����ȥʹ�ܣ�4��}
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
        case  0: // ����
        {
            sprintf(cmd,"appm -s -c %s -n %s | grep started",docker->buff,app->buff);
        }break;
        case 1: //ֹͣ
        {
            sprintf(cmd,"appm -S -c %s -n %s | grep stopped",docker->buff,app->buff);
        }break;
        case 2: //ж��
        {
            sprintf(cmd,"appm -u -c %s -n %s | grep Uninstalled",docker->buff,app->buff);
        }break;
        case 3:  //ʹ��
        {
            sprintf(cmd,"appm -e -c %s -n %s | grep enabled",docker->buff,app->buff);
        }break;
        case 4: //ȥʹ��
        {
            sprintf(cmd,"appm -d -c %s -n %s | grep disenabled",docker->buff,app->buff);
        }break;
        default:
            break;
    }

	//�ж����ʱ������Ҫ�����app��û�����ע���� ������ע������Ϊ36��0��ʱ�򣬲������ظ����� ̨�����Ӧ�ù���
    if(type == 0)
    {
        int ret = 0,i;
        app_license_T liscen;
        memset(&liscen,0,sizeof(liscen));
        char filePath[100] = "/data/app/iotManager/data/LICENSE";
        ret = read_pdata_xram(filePath,(char *)&liscen,0,sizeof(app_license_T));
        if(ret<0)
        {
            MQTTMSG_FMT_DEBUG("û���ҵ�LICENSE�ļ� %s\n",filePath);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("�յ�����APP %s �����������Ƿ���� Applicense \n",app->buff);
            
            for(i=0;i<liscen.licennum;i++)
            {
                if(strcmp((char *)app->buff,liscen.licen[i].appname)==0) //�ҵ���
                {
                    MQTTMSG_BUF_DEBUG(liscen.licen[i].applicen,liscen.licen[i].licenlen,"APP licen");
                  //  MQTTMSG_FMT_DEBUG("����APP %s licenlen %d license %s\n",liscen.licen[i].appname,liscen.licen[i].licenlen,liscen.licen[i].applicen);
                    char tmplice[36] = {0};
                    if(memcmp(tmplice,liscen.licen[i].applicen,24)==0 )  //����̨�����
                    {
                        MQTTMSG_FMT_DEBUG("App %s license ���� %d ȫ0,��ִ������\n",app->buff,liscen.licen[i].licenlen);
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
    //����֤һ��
    //ȡ�����죬��ΪֻҪ����û�����⣬��ȷ�������⡣
#if 0
    memset(cmd,0,sizeof(cmd));
    switch (type)
    {
            case 0: // ����            
            case 1: //ֹͣ
            {
                sprintf(cmd,"appm -I -c %s -n %s | grep 'Service status' | awk '{print $4}'",docker->buff,app->buff);
            }break;
            case 2: //ж��
            {
                sprintf(cmd,"appm -I -c %s",docker->buff);
            }break;
            case 3:  //ʹ��
            case 4: //ȥʹ��
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
            case 0: // ����
            {
                if(strcmp("running",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 1: //ֹͣ
            {
                if(strcmp("stopped",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 2: //ж��
            {
                if(strcmp("No APP installed",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 3:  //ʹ��
            {
                if(strcmp("yes",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 4: //ȥʹ��
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
        case  0: // ����
        {
            sprintf(cmd,"appm -s -c %s -n %s",docker->buff,app->buff);
        }break;
        case 1: //ֹͣ
        {
            sprintf(cmd,"appm -S -c %s -n %s",docker->buff,app->buff);
        }break;
        case 2: //ж��
        {
            sprintf(cmd,"appm -u -c %s -n %s",docker->buff,app->buff);
        }break;
        case 3:  //ʹ��
        {
            sprintf(cmd,"appm -e -c %s -n %s",docker->buff,app->buff);
        }break;
        case 4: //ȥʹ��
        {
            sprintf(cmd,"appm -d -c %s -n %s",docker->buff,app->buff);
        }break;
        default:
            break;
    }
    uint8 offset= strlen(cmd);
	sprintf(cmd+offset," | grep success");

	//�ж����ʱ������Ҫ�����app��û�����ע���� ������ע������Ϊ36��0��ʱ�򣬲������ظ����� ̨�����Ӧ�ù���
    if(type == 0)
    {
        int ret = 0,i;
        app_license_T liscen;
        memset(&liscen,0,sizeof(liscen));
        char filePath[100] = "/data/app/iotManager/data/LICENSE";
        ret = read_pdata_xram(filePath,(char *)&liscen,0,sizeof(app_license_T));
        if(ret<0)
        {
            MQTTMSG_FMT_DEBUG("û���ҵ�LICENSE�ļ� %s\n",filePath);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("�յ�����APP %s �����������Ƿ���� Applicense \n",app->buff);
            
            for(i=0;i<liscen.licennum;i++)
            {
                if(strcmp((char *)app->buff,liscen.licen[i].appname)==0) //�ҵ���
                {
                    MQTTMSG_BUF_DEBUG(liscen.licen[i].applicen,liscen.licen[i].licenlen,"APP licen");
                  //  MQTTMSG_FMT_DEBUG("����APP %s licenlen %d license %s\n",liscen.licen[i].appname,liscen.licen[i].licenlen,liscen.licen[i].applicen);
                    char tmplice[36] = {0};
                    if(memcmp(tmplice,liscen.licen[i].applicen,24)==0 )  //����̨�����
                    {
                        MQTTMSG_FMT_DEBUG("App %s license ���� %d ȫ0,��ִ������\n",app->buff,liscen.licen[i].licenlen);
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
    //����֤һ��
#if 0
    memset(cmd,0,sizeof(cmd));
    switch (type)
    {
            case 0: // ����            
            case 1: //ֹͣ
            {
                sprintf(cmd,"appm -I -c %s -n %s | grep 'Service status' | awk '{print $4}'",docker->buff,app->buff);
            }break;
            case 2: //ж��
            {
                sprintf(cmd,"appm -I -c %s -n %s",docker->buff,app->buff);
            }break;
            case 3:  //ʹ��
            case 4: //ȥʹ��
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
            case 0: // ����
            {
                if(strcmp("running",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 1: //ֹͣ
            {
                if(strcmp("stopped",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 2: //ж��
            {
                if(strcmp("no app info",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 3:  //ʹ��
            {
                if(strcmp("YES",buff[0])!=0)
                {
                    errcode = FALSE;
                }
            }break;
            case 4: //ȥʹ��
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
* @brief     ��App�澯֪ͨ
* @param[in] ��docker ���� app app ,type��������{��װ��0����������1����ж�أ�2����������4����ֹͣ��4�������ò�����5����ʹ��,6����ȥʹ�ܣ�7�����·���ɣ�8��}��
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
        return 1;   //�ļ���
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
* @brief     : ������־·����������
* @param[in] : exec �������� 0 ��ȡ��Ϣ 1. ��� 2. ɾ�� filePath �������ļ���·�� 
* @param[out]��recvBuf �ظ����ģ�msglen ���ĳ���
* @return    ��0 �ɹ� -1 ʧ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     : ������־�ַ���ʱ��ת��Ϊtime_s����
* @param[in] : str �ַ���ʱ��
* @param[out]: time_s ת�����ʱ��
* @return    : �ַ���ʱ���׼:  1970-01-01 00:00:00
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     : ������־·�����log�ṹ�庯��
* @param[in] : path ��־·��
* @param[out]: log �����log�ṹ��
* @return    : �ַ���ʱ���׼:  1970-01-01 00:00:00
* @Create    : 
* @Date      ��2020-5-22
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
       char *values = strim(result[i]); //ȥ�����ߵĿո�
       values = strdelch(values,'"');  //ȥ��˫����
       values = strdelch(values,',');  //ȥ������
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
            MQTTMSG_FMT_DEBUG("��־��md5ֵ Ϊ %s\n",value);
            uint8 bit_str[100] = {0};    
            int bitlen = strt2octet(value,strlen(value),bit_str,64);
            if((bitlen<0) || (bitlen >64))
            {
                MQTTMSG_FMT_DEBUG("ת��ʧ�� bufflen %d \n",bitlen);
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
* @brief     : ������־���Ͳ�������
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     : ��ϢApp��������ִ��
* @param[in] : MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
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
* @brief     : 001D ��־���
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
        if(data[0]==0)            //��־�ļ�·�� [0]  visible-string��
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

            /*�ж����ļ������ļ���*/
        }
        else if(data[0]==1)     //��־����     [1]  LOGTYPE
        {
            uint8 type = data[index++];
            operate_accord_logType(type,SourcePath);
            option   = data[index++];
            if(option == 0)  //Ĭ���ļ�����
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
            /*�ж���Ŀ¼�����ļ�*/
            int filetype = judge_path_fold_dir(tmppackPath);
            if(filetype == -1 )
            {
                retype =0;
                MQTTMSG_FMT_DEBUG("path error %s\n",tmppackPath);
                break;
            }else if(filetype == 0)
            {
                //�ļ�
                sprintf(packPath,"%s",tmppackPath);
            }else
            {
                //Ŀ¼
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
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
        if(data[0]==0)       //��־�ļ�·�� [0]  visible-string   
        {
            filelen = data[index++];
            memcpy(logFile,&data[index],filelen);
            index+=filelen;
            result  = judge_path_exec(logFile);
            break;
        }
        else if(data[0]==1) //��־����     [1]  LOGTYPE
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
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
      INIT_FMT_DEBUG("ϵͳ��������� devcpulimit %d devMemLimit %d devDiskLimit %d devCpuTempdown %d devCpuTempup %d monitorTime %d\n", pData->app_t.devCpuLimit,pData->app_t.devMemLimit,pData->app_t.devDiskLimit,pData->app_t.devCpuTempdown,pData->app_t.devCpuTempup,pData->app_t.monitorTime);
     // system("mkdir /data/app/iotManager/data/");
      sprintf(filePath,"%s%s",PrivateDir,"SYSCONF");
      write_pdata_xram(filePath,(char *)&sysconf,0,sizeof(SYS_CONF));
    }while(0);
    recvBUFF[0] = result;
    packge_report_msg(MsgRecv,recvBUFF,1);
}

/**********************************************************************
* @name      : docker_exec_001B
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
    strcpy(hardversion,"DM01");  //��ʱд��
    tmpData[index++]=strlen(sysversion);
    strcpy(&tmpData[index],sysversion);
    index+=strlen(sysversion);
    tmpData[index++]=strlen(kerversion);
    strcpy(&tmpData[index],kerversion);
    index+=strlen(kerversion);
    tmpData[index++] = 0;//�ļ�ϵͳ�汾ΪNULL
    tmpData[index++]=strlen(hardversion);
    strcpy(&tmpData[index],hardversion);
    index+=strlen(hardversion);
    packge_report_msg(MsgRecv,(uint8 *)tmpData,index);  //Ӧ��
}

/**********************************************************************
* @name      : docker_exec_0019
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
            //���ش���
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
            MQTTMSG_FMT_DEBUG("������֤���� index %d msglen %d \n",index,msglen);
            result = FALSE;
            break;
        }
        //��֤�����ļ��Ƿ����
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
            MQTTMSG_FMT_DEBUG("�����ļ���С����MD5ֵУ��������  \n");
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
    //ת��md5ֵ
    recvBuff[0] = result;
    packge_report_msg(MsgRecv,recvBuff,1);
    if(FALSE == result)
    {
        return;
    }
    
    //ƴ������
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
     //ִ�������Ҫ����ֵ
     //�Ȼظ���ȷ�������ļ�
    MQTTMSG_FMT_DEBUG("��⵽ϵͳ����  ���� CMD: %s\n",cmd);
    ret = getshellresult(cmd, buff, 20);
    if(ret > 0 )
    {
        print_exec_faild(ret,buff);
    }

  //  memset(filePath,0,sizeof(filePath));
 //   sprintf(filePath,"%s%s",PrivateDir,"SYSUPDATE");
  //  write_pdata_xram(filePath,(char *)&sys_info,0,sizeof(sys_update_info));
    system(cmd);

    //��ʱ���� �����޸İ汾��
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
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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

		
        /*��Ҫ��Щapp��LICENSE*/
        if(data[index]==0)
        {
            //ȫ��
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
            //��Ϣ�����д���
            MQTTMSG_FMT_DEBUG("��Ϣ�����д��� \n");
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

			if(i == liscen.licennum)	//û���ҵ�
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
            //����ظ�
            recvbuff[0]=0;
            packge_report_msg(MsgRecv,recvbuff,1);
            MQTTMSG_FMT_DEBUG("��˽���ļ�ʧ�� \n");
            return;
            
        }
        else
        {
            /*��Ҫ��Щapp��LICENSE*/
            if(data[index]==0)
            {
                //ȫ��
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
                //��Ϣ�����д���
                MQTTMSG_FMT_DEBUG("��Ϣ�����д��� \n");
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
        //����ظ�
        recvbuff[0]=0;
        packge_report_msg(MsgRecv,recvbuff,1);
        MQTTMSG_FMT_DEBUG("û���ҵ���˽���ļ� %s\n",filePath);
        return;
    }
#endif    
    
}

/**********************************************************************
* @name      : docker_exec_0018
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
* @Update    :
**********************************************************************/

void docker_exec_0018(MSG_INFO_T *MsgRecv,int MSGLen)
{
    MQTTMSG_FMT_DEBUG("�յ����ע�����·�ָ�� \n");
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
    MQTTMSG_BUF_DEBUG(liscen.licen[i].applicen, liscen.licen[i].licenlen,"�յ���liscen");
    //����Ƿ�����쳣licen ƥ��24λ
    char tmpcode[30]= {0};
    for(i=0;i<count;i++)
    {
        if(memcmp(liscen.licen[i].applicen,tmpcode,24)==0)
        {
            MQTTMSG_FMT_DEBUG("ƥ�䵽24λ0 ��Ϣ�ظ�����\n");
            result = 0;
            break;
        }
        
    }
    /*��˽���ļ��е����ȶԣ�appname��ͬ���滻����ͬ�����ӣ��ȶ�*/
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
                        MQTTMSG_FMT_DEBUG("���ע������ҵ�ͬ��Ӧ���滻 App %s\n",liscen.licen[i].appname);
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
                    MQTTMSG_FMT_DEBUG("���ע�����δ�ҵ�ͬ��Ӧ������ App %s\n",liscen.licen[i].appname);
                    tmpliscen.licennum++;  
                }
            }
        }

    }else if(access(filePath,F_OK)<0)  //�쳣ԭ��ʧ�򴴽�һ��
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
            MQTTMSG_FMT_DEBUG("App %s д���ע����ɹ�\n",tmpliscen.licen[i].appname);
        }     
    }
    /*�ж϶������ĸ�����*/
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
* @brief     : ������е�App��Ϣ
* @param[in] : MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
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
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     : ��ȡ����������app����Ϣ
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     : ��ȡȫ����Ϣ
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
                        /*��app��Ϣ*/
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
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
        //û������Ҳû��app ȫ����APP
        get_all_app(&app_info);
    }
    else if(data[index]==0 && data[index+1]!=0)
    {
        /*�������������������ֵ�app*/
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
    /*��ȡ��Ӧ����ϢȻ���鱨��*/
/*    if(index!=MsgRecv->MsgLen)
    {
        MQTTMSG_FMT_DEBUG("���Ľ������� index %d msglen %d\n",index,MsgRecv->MsgLen);
    }*/
    get_global_value(&app_info);
    msglen = package_recv_0017(&app_info,recvbuff);
    packge_report_msg(MsgRecv,recvbuff,msglen);
    
}

/**********************************************************************
* @name      : docker_exec_0016
* @brief     ����ϢApp��������ִ��
* @param[in] ��MSG_INFO_T *MsgRecv ��Ϣ,int MSGLen ����
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
    app_actinfo.docker.len = MsgRecv->MsgData[offset++]; //�������Ƴ���
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
    �ȼ��app�Ƿ���������
    */
  //  sprintf(cmd,"appm -I -c %s -n %s | grep running",app_actinfo.docker.buff,app_actinfo.app.buff);
   // MQTTMSG_FMT_DEBUG("ִ��App��װ CMD : %s\n",cmd);
   // if(getshellresult(cmd,buff,10)==0)
   // {
   //     result = FALSE;
   // }
  //  else
  //  {
  //  MQTTMSG_FMT_DEBUG("App ���� ���ò���\n");
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

    MQTTMSG_FMT_DEBUG("App �������óɹ� cmd %s\n",cmd);
 //   }
    
    result_buff[0]=result;
    packge_report_msg(MsgRecv,result_buff,1);  //Ӧ��
    sleep(3);
    app_action_notice(&app_actinfo.docker,&app_actinfo.app,5,result);
}

/**********************************************************************
* @name      : heart_beat
* @brief     ����ϵͳ�������ظ�����
* @param[in] ��uint16 index �ڼ���
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
* @brief     �����ܵ���Ϣ������ָ����ʽ
* @param[in] ��uint8 *pBuf           ���ܵ��ı���
               uint16 bufLen         ���ĳ���
* @param[out]��MSG_INFO_T **ppMsgOut ������������Ϣ
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2019-10-29
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
    
    /* ���� */
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
        /*ִ������ �жϷ���ֵ ��������֪ͨ*/ 
        if(strcmp(buff[0],sys_info.updatever)==0)
            result = TRUE;
        packge_docker_notice_0006(sys_info.updatetype,result);
        sprintf(cmd,"rm -f %s",filePath);
        system(cmd);
    }
}
/**********************************************************************
* @name      : iot_mqtt_proc
* @brief     ��Mqtt�߳�������
* @param[in] ��void *pArg, void *pPublic, void *pPrivate
* @param[out]��
* @return    ��
* @Create    : 
* @Date      ��2020-5-22
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
                 //���ӷ��� �Է�������ѭ��
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
                                case 0x0011:            //��������
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
                                        //�������
                                   docker_exec_0018(MsgRecv,RecvLen);  
                                }break;
                                case 0x0019:
                                {
                                    //��ɲ�ѯ 
                                    docker_exec_0019(MsgRecv,RecvLen);
                                }break;
                                case 0x001A:
                                {
                                    //Ŀǰû������ ϵͳ����
                                    docker_exec_001A(MsgRecv,RecvLen);
                                }break;
                                case 0x001B:
                                {
                                   docker_exec_001B(MsgRecv,RecvLen); 
                                }break;
                                case 0x001C:
                                {
                                    // ��־��Ϣ��ѯ  û��
                                    docker_exec_001C(MsgRecv,RecvLen); 
                                }break;
                                case 0x001D:
                                {
                                    //��־���     ��־����û��
                                    docker_exec_001D(MsgRecv,RecvLen); 
                                }break;
                                case 0x001E:
                                {
                                    //��־ɾ��     ��־����û��
                                    docker_exec_001E(MsgRecv,RecvLen); 
                                }break;
                                case 0x001F:
                                {
                                    //ϵͳ���ò���     
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
                //������Ϣ�ӿڷ�����Ϣ 
                MSG_INFO_T* msg  = (MSG_INFO_T*)SendBuf;
                char topic[1000]={0};
                sprintf(topic,"%s/%s/%04X/%04X/",msg->SourAddr,msg->DestAddr,msg->IID,msg->IOP);
                //sprintf(topic,"%s/%s/%04x/%04x",msg->SourAddr,msg->DestAddr,msg->IID,msg->IOP);
                //����Ϣ
                MQTTMSG_FMT_DEBUG("����֪ͨ�¼��ϱ� topic %s IID %04x IOP %04x\n",topic,msg->IID,msg->IOP);
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
                //������Ϣ�ӿڷ�����Ϣ
                MSG_INFO_T* msg  = (MSG_INFO_T*)SendBuf;
                char topic[1000]={0};
                sprintf(topic,"%s/%s",msg->SourAddr,msg->DestAddr);
                //����Ϣ
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


