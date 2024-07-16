/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��·���MQTT�ӿ�
* @date��    2020-6-30
* @history�� 
* @User deatil:

*********************************************************************
*/
#ifndef	__SEC_MQTT_H
#define __SEC_MQTT_H
    
#ifdef __cplusplus
        extern "C" {
#endif

//��̫���˿ڿ�����Ϣ
typedef struct 
{
    uint8 nNum;
    uint16 port[SEC_RECORD_MAX];
}SEC_PORTOPEN_T;

typedef struct
{
    char  ip[20]; //���ʮ����
    uint16 port;
}SEC_CONNINFO_T;

//��̫��������Ϣ
typedef struct 
{
    uint8 nNum;
    SEC_CONNINFO_T connect[SEC_RECORD_MAX];
}SEC_CONNECT_T;

//��¼����̨����
typedef enum 
{
    CON_LOGIN_IN = 0,
    CON_LOGIN_OUT = 1,
    CON_LOGIN_FAIL = 2,
}SEC_LOGINSTATUS_E;

//��̫����¼����̨���
typedef struct 
{
    uint8 status;
}SEC_ETHLOGIN_T;

//���ڵ�¼����̨���
typedef struct 
{
    uint8 status;
}SEC_SERIALSLOGIN_T;

//��������
typedef enum
{
    SEC_CMD_RM = 0,    //ɾ��
    SEC_CMD_MV = 1,    //������ 
    SEC_CMD_CP = 2,    //���� 
    SEC_CMD_KILL = 3,  //���̽��� 
}SEC_DANGERCMD_E;

//Σ�ղ�������
typedef struct 
{
    SEC_DANGERCMD_E cmd;
}SEC_DANGERCMD_T;

//������Ϣ���
typedef struct 
{
    char usrname[SEC_RECORD_MAX];
}SEC_PWDCHG_T;

//�ؼ�Ŀ¼�ļ����
typedef struct 
{
    uint8 nNum;
    char *path[SEC_RECORD_MAX];
}SEC_PATHCHG_T;


//��������
typedef enum
{
    SEC_SSH_OPEN = 0,               //��
    SEC_SSH_CLOSE = 1,              //�ر�
    SEC_SSH_ABNORMAL = 2,           //�����쳣
    SEC_SSH_LONGTIMENOCLOSE = 3,    //��ʱ��δ�ر�
}SEC_SSHABNORMAL_E;

//SSH�����쳣
typedef struct 
{
    uint8 status;
}SEC_SSHABNORMAL_T;

//��¼�쳣����
typedef enum 
{
    ABNORMAL_BLAST_LOGIN = 1, //���Ƶ�¼
}SEC_LOGINABNORMAL_E;

//�û���¼�쳣
typedef struct
{
    uint8   type;         //�쳣����
    char    usrname[64];  //ʧ�ܵ��û���
    char    loginip[20];  //��¼ʧ�ܵ�IP
}SEC_LOGINABNORMAL_T;

//��������
typedef enum 
{
    SEC_PROC_UNKNOW = 0, //δ֪
    SEC_PROC_HOST   = 1, //�������ڽ���    
    SEC_PROC_CONT   = 2, //�����ڽ���
}SEC_PROCTYPE_E;

//�Ự�����쳣
typedef struct
{
    char    remoteIP[20];  //Զ��IP
    uint16  remotePort;    //Զ�̶˿�
    char    localIP[20];   //����IP
    uint16  localPort;     //���ض˿�
    char    protocol[4];   //Э������
    char    process[32];   //��������
    uint8   procType;      //�������� @ref SEC_PROCTYPE_E
}SEC_SESSIONABNORMAL_T;

//�Ự�����쳣
typedef struct
{
    uint8 nNum;
    SEC_SESSIONABNORMAL_T flow[SEC_RECORD_MAX];
}SEC_SESSIONFLOWS_T;

//�Ự�����쳣
typedef struct
{
    char    remoteIP[20];  //Զ��IP
    uint16  remotePort;    //Զ�̶˿�
    char    localIP[20];   //����IP
    uint16  localPort;     //���ض˿�
    char    protocol[4];   //Э������
    char    process[32];   //��������
    uint32  flowLimit;     //������ֵ����λ:Kbps��
    uint32  curFlow;       //��ǰ���٣���λ:Kbps��
}SEC_BANDWIDTH_ABNORMAL_T;

//�Ự�����쳣
typedef struct
{
    uint8 nNum;
    SEC_BANDWIDTH_ABNORMAL_T flow[SEC_RECORD_MAX];
}SEC_SESSIONBANDWIDTHS_T;

//������ֵ����
typedef struct
{
    uint32 monthFlowLimit;   //���ۻ�������ֵ ����λ��KB��
    uint32 flowSpeedLimit;   //����������ֵ (��λ��Kbps)
}SEC_SESSION_LIMIT_T;

//��������
typedef enum 
{
    SEC_SRV_SSH   = 1,    
    SEC_SRV_SFTP   = 2,
}SEC_SRVTYPE_E;

//����ʱ��δ�رղ���
typedef struct
{
    uint8 type;   //�������� @ref SEC_SRVTYPE_E
    uint32 limit; //ʱ����ֵ ����λ�����ӣ�
}SEC_TIME_LIMIT_T;

//����ʱ��δ�رղ���
typedef struct
{
    uint8 nNum;
    SEC_TIME_LIMIT_T service[2];
}SEC_SRVTIME_LIMIT_T;

//�����쳣����
typedef enum 
{
    SEC_PROC_ADD   = 1,    //��������
}SEC_PROCABNTYPE_E;

//�����쳣
typedef struct
{
    uint8            type;         //�쳣���� @ref SEC_PROCABNTYPE_E
    OOP_DATETIME_S_T stime;        //��������ʱ��
    char             name[64];     //������
    char             user[64];     //�����û�
    char             group[64];    //������
    char             cmdline[128]; //��������
}SEC_PROCABNORMAL_T;

typedef struct
{
    uint8 nNum;
    SEC_PROCABNORMAL_T info[SEC_RECORD_MAX];
}SEC_PROCABNORMALS_T;

int sec_mqtt_init(SEC_APPDATA_T *pData);
int sec_login_to_smios(uint8 *pAppId, uint8 appidlen);
int sec_send_axdrmsg(MESSAGE_INFO_T *pMsgInfo, int qos);
int sec_send_jsonmsg(MESSAGE_JSON_T *pMsgInfo, int qos);
int sec_send_mqttmsg(char *pTopic, int qos, void *msgdata, int msglen);
int sec_send_mapmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int sec_send_guimsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int sec_send_dbmsg(uint16 label, uint16 index, uint16 iop, uint8* playload, uint16 length);
int sec_send_appmsg(MESSAGE_INFO_T *pMsgInfo);
int sec_axdrmsg_respond(MESSAGE_INFO_T *pInMsg, uint8 *payload, uint32 msglen);

int sec_seek_app();
int sec_send_msg(MESSAGE_MQTT_T *pMsgMqtt, int qos);
int sec_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode);
int sec_recv_mqttmsg(char *topic, uint16 topicLen, uint8 *pBuf, uint16 buflen);
int sec_get_version(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int sec_get_appidmsg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int sec_login_to_smios(uint8 *pAppId, uint8 appidlen);
int sec_heart_bear(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int sec_recv_regapp(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int sec_notify_send(uint16 index, uint16 IOP, char *destApp, uint8 *buffer, uint16 length);

uint8 sec_dbCenter_flag();
uint8 sec_appmap_flag();
uint8 sec_appgui_flag();
int sec_get_selfversion(int sockid, MESSAGE_INFO_T *pInMsg);

void sec_mqtt_debug();
int sec_login_to_sys();
int sec_heart_to_sys(MESSAGE_JSON_T *pInMsg, void *pArg, void *pPublic);
int sec_get_appreglist();
uint8 sec_appreglist_check(MESSAGE_JSON_T *pInMsg, void *pArg, void *pPublic);
uint16 sec_get_next_index();
int sec_dangercmd_send(SEC_DANGERCMD_T *dangercmd);
int sec_pwdchanged_send(SEC_PWDCHG_T *pwdchg);
int sec_pathchanged_send(SEC_PATHCHG_T *pathchg);
int sec_ethportopen_send(SEC_PORTOPEN_T *portopen);
int sec_ethconnect_send(SEC_CONNECT_T *ethconn);
int sec_ethlogin_send(SEC_ETHLOGIN_T *login);
int sec_sshabnormal_send(SEC_SSHABNORMAL_T *abnormal);
int sec_loginabnormal_send(SEC_LOGINABNORMAL_T *abnormal);
int sec_flowabnormal_send(SEC_SESSIONFLOWS_T *abnormal);
int sec_bandabnormal_send(SEC_SESSIONBANDWIDTHS_T *abnormal);
int sec_procabnormal_send(SEC_PROCABNORMALS_T *abnormal);

#ifdef __cplusplus
}
#endif

#endif

