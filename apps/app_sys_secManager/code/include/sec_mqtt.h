/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控MQTT接口
* @date：    2020-6-30
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef	__SEC_MQTT_H
#define __SEC_MQTT_H
    
#ifdef __cplusplus
        extern "C" {
#endif

//以太网端口开放信息
typedef struct 
{
    uint8 nNum;
    uint16 port[SEC_RECORD_MAX];
}SEC_PORTOPEN_T;

typedef struct
{
    char  ip[20]; //点分十进制
    uint16 port;
}SEC_CONNINFO_T;

//以太网外联信息
typedef struct 
{
    uint8 nNum;
    SEC_CONNINFO_T connect[SEC_RECORD_MAX];
}SEC_CONNECT_T;

//登录控制台类型
typedef enum 
{
    CON_LOGIN_IN = 0,
    CON_LOGIN_OUT = 1,
    CON_LOGIN_FAIL = 2,
}SEC_LOGINSTATUS_E;

//以太网登录控制台检测
typedef struct 
{
    uint8 status;
}SEC_ETHLOGIN_T;

//串口登录控制台检测
typedef struct 
{
    uint8 status;
}SEC_SERIALSLOGIN_T;

//命令类型
typedef enum
{
    SEC_CMD_RM = 0,    //删除
    SEC_CMD_MV = 1,    //重命名 
    SEC_CMD_CP = 2,    //复制 
    SEC_CMD_KILL = 3,  //进程结束 
}SEC_DANGERCMD_E;

//危险操作命令
typedef struct 
{
    SEC_DANGERCMD_E cmd;
}SEC_DANGERCMD_T;

//口令信息变更
typedef struct 
{
    char usrname[SEC_RECORD_MAX];
}SEC_PWDCHG_T;

//关键目录文件变更
typedef struct 
{
    uint8 nNum;
    char *path[SEC_RECORD_MAX];
}SEC_PATHCHG_T;


//命令类型
typedef enum
{
    SEC_SSH_OPEN = 0,               //打开
    SEC_SSH_CLOSE = 1,              //关闭
    SEC_SSH_ABNORMAL = 2,           //服务异常
    SEC_SSH_LONGTIMENOCLOSE = 3,    //长时间未关闭
}SEC_SSHABNORMAL_E;

//SSH服务异常
typedef struct 
{
    uint8 status;
}SEC_SSHABNORMAL_T;

//登录异常类型
typedef enum 
{
    ABNORMAL_BLAST_LOGIN = 1, //爆破登录
}SEC_LOGINABNORMAL_E;

//用户登录异常
typedef struct
{
    uint8   type;         //异常类型
    char    usrname[64];  //失败的用户名
    char    loginip[20];  //登录失败的IP
}SEC_LOGINABNORMAL_T;

//进程类型
typedef enum 
{
    SEC_PROC_UNKNOW = 0, //未知
    SEC_PROC_HOST   = 1, //宿主机内进程    
    SEC_PROC_CONT   = 2, //容器内进程
}SEC_PROCTYPE_E;

//会话流量异常
typedef struct
{
    char    remoteIP[20];  //远程IP
    uint16  remotePort;    //远程端口
    char    localIP[20];   //本地IP
    uint16  localPort;     //本地端口
    char    protocol[4];   //协议类型
    char    process[32];   //关联进程
    uint8   procType;      //进程类型 @ref SEC_PROCTYPE_E
}SEC_SESSIONABNORMAL_T;

//会话流量异常
typedef struct
{
    uint8 nNum;
    SEC_SESSIONABNORMAL_T flow[SEC_RECORD_MAX];
}SEC_SESSIONFLOWS_T;

//会话带宽异常
typedef struct
{
    char    remoteIP[20];  //远程IP
    uint16  remotePort;    //远程端口
    char    localIP[20];   //本地IP
    uint16  localPort;     //本地端口
    char    protocol[4];   //协议类型
    char    process[32];   //关联进程
    uint32  flowLimit;     //流速限值（单位:Kbps）
    uint32  curFlow;       //当前流速（单位:Kbps）
}SEC_BANDWIDTH_ABNORMAL_T;

//会话流量异常
typedef struct
{
    uint8 nNum;
    SEC_BANDWIDTH_ABNORMAL_T flow[SEC_RECORD_MAX];
}SEC_SESSIONBANDWIDTHS_T;

//流量限值参数
typedef struct
{
    uint32 monthFlowLimit;   //月累积流量限值 （单位：KB）
    uint32 flowSpeedLimit;   //月累流速限值 (单位：Kbps)
}SEC_SESSION_LIMIT_T;

//进程类型
typedef enum 
{
    SEC_SRV_SSH   = 1,    
    SEC_SRV_SFTP   = 2,
}SEC_SRVTYPE_E;

//服务长时间未关闭参数
typedef struct
{
    uint8 type;   //服务类型 @ref SEC_SRVTYPE_E
    uint32 limit; //时间限值 （单位：分钟）
}SEC_TIME_LIMIT_T;

//服务长时间未关闭参数
typedef struct
{
    uint8 nNum;
    SEC_TIME_LIMIT_T service[2];
}SEC_SRVTIME_LIMIT_T;

//进程异常类型
typedef enum 
{
    SEC_PROC_ADD   = 1,    //进程增加
}SEC_PROCABNTYPE_E;

//进程异常
typedef struct
{
    uint8            type;         //异常类型 @ref SEC_PROCABNTYPE_E
    OOP_DATETIME_S_T stime;        //进程启动时间
    char             name[64];     //进程名
    char             user[64];     //进程用户
    char             group[64];    //进程组
    char             cmdline[128]; //进程命令
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

