#ifndef _MQTT_SERVER_H__
#define _MQTT_SERVER_H__

#include <pthread.h>

#include "baseType.h"
// #include "constant.h"
#include "msgServiceBase.h"

#ifdef __cplusplus
extern "C" {
#endif

// MQTT�������
#define RECONNECT_INTERVAL (5)
// MQTT���������
#define RECONNECT_INTERVAL_MAX (15)
// MQTT SELECT ��ʱʱ�� ��λ����
#define MOSQUITTO_SELECT_TIMEOUT (100)
// MQTT KEEP ALIVE ʱ�� ��λ��
#define MOSQUITTO_KEEPALIVE_TIME (60)

typedef struct _mqtt_ctx
{
    MSG_OBJ_T reqMsg;     //������Ϣͷ
    RAW_OBJ_T reqRawObj;  //������Ϣ����
    MSG_OBJ_T rspMsg;     //�ظ���Ϣͷ
    RAW_OBJ_T rspRawObj;  //�ظ���Ϣ����
} MQTT_CTX_T;

/* @brief:mqttͨѶ����ṹ���װ  */
typedef struct _mqtt_server_obj_
{
    uint32            stop;          // MQTT�����ֹͣ�����־λ
    uint16            serverPort;    // MQTT�����ϵķ���˿�
    sint8             serverIp[16];  // MQTT����󶨵�IP��ַ
    sint8             qos;  // MQTT����󶨵�IP��ַ
    sint8             userID[MAX_USER_ID_LEN];      // MQTT�û�ID
    sint8             subTopicp[MAX_TOPIC_LEN];     //���ĵı���
    sint8             userName[MAX_USER_NAME_LEN];  // mqtt ��¼�û���
    sint8             passwd[MAX_PASSWD_LEN];       // mqtt ��¼����
    pthread_t         tid;   // MQTT�����̵߳�ThreadIP
    struct mosquitto *mosq;  // mqtt mosquitto����
} MQTT_SERVICE_T;

typedef struct _mqtt_server_sub_topic
{
    char topic[MAX_TOPIC_LEN];
    sint32 qos;
} MQTT_SERVER_SUB_TOPIC_T;

typedef void (*SUBSCRIBE_CB_FP)(MQTT_SERVER_SUB_TOPIC_T *subTopic[], sint32 *subNum);

/* @brief:MQTT�����ʼ��  */
sint32 mqtt_init_system(void);
/* @brief:MQTT��������  */
sint32 mqtt_start_system(void);
/* @brief:MQTT����ֹͣ  */
void mqtt_stop_system(void);
/* @brief:MQTT��Ϣ����  */
sint32 mqtt_sendto(sint8 *dest, void *data, uint32 dataLen);

void set_subscribe_cb_fp(SUBSCRIBE_CB_FP fp);

void mqtt_set_broker_ip(sint8 ip[IP_LEN]);
void mqtt_set_broker_port(sint16 port);
void mqtt_set_sub_qos(sint8 qos);
void mqtt_set_userID(sint8 username[MAX_USER_NAME_LEN]);
void mqtt_set_username(sint8 username[MAX_USER_NAME_LEN]);
void mqtt_set_password(sint8 passwd[MAX_PASSWD_LEN]);

#ifdef __cplusplus
}
#endif

#endif