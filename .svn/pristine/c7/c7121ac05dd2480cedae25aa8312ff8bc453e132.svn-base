/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块PPP操作相关定义申明
* @date：    2019-10-24
* @history： 
*********************************************************************
*/
#ifndef __IOT_MQTT_H__
#define __IOT_MQTT_H__
        
#ifdef __cplusplus
        extern "C" {
#endif

#define STATUS_INIT  0
#define STATUS_LOGIN 1
#define STATUS_WAIT  2
#define STATUS_SEND  3
#define STATUS_RECV  4

#define WG_SUBTOPIC1      "+/M-iotManager/#"
#define WG_SUBTOPIC2      "+/iotManager/#"
#define WG_SUBTOPIC3      "alarm/broadcast/notify/#"
#define WG_SUBTOPIC4      "operate/broadcast/notify/#"
#define WG_DEFAULT_TOPIC "M-iotManager/M-smiOS"
#define JSON_REPORT_FLAG "/broadcast/notify" 
#define WUGUAN_APP_NAME_M     "M-iotManager"
#define  APP_RECV_BUF_LEN           2048	/*recvfrom接收缓冲区大小*/

MQTT_CLIENT_T   *i_MqttClient;
pthread_mutex_t  g_IotSendLock;
pthread_mutex_t  g_MqttIndexlock;
pthread_mutex_t g_IotIndexlock;

#define WG_TOPIC_LEN  125
#define MAX_MSG_LEN 8192

typedef struct
{
    uint8 status;
    uint8 logined;    //是否已经登录系统管理器
}wg_mqttattr;

typedef struct tag_IOT_MSGDATA
{
    char          topic[WG_TOPIC_LEN];
    uint8         recvBuf[APP_RECV_BUF_LEN];
}WG_MSGDATA_T;

typedef struct tagMsgInfo
{
    uint32 IP;        //目的端ip
    uint16 port;
    uint8  res[2];
    
    uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128];
    char   DestAddr[128];

    uint16 IID;
    uint16 IOP;

    uint32 MsgLen; 
    uint8  MsgData[0];
}MSG_INFO_T;

uint16 iot_get_next_index();

#ifdef __cplusplus
}
#endif

#endif //__PPP_OPER_H__

