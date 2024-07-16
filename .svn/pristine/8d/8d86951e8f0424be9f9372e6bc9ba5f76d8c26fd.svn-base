/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控业务
* @date：    2020-6-30
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef __SEC_SERVICE_H
#define __SEC_SERVICE_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define KEY_MAX_FILE_NUM    128 //关键目录扫描的最多文件个数-->lib目录当前文件数量91->更新70->128
#define KEY_MAX_FILE_LEN    30  //关键目录扫描的文件名最大长度
#define KEY_FILE_ATTR_LEN   36  //关键目录扫描的文件属性最大长度

#define STATION_COMM_PARA_CNT   4                   //主站通信参数个数,包括eth  主备、GPRS主备
#define TMN_SECURITY_DETECT     "security_manager_record"
#define TcpRemoteSrvPortLen     sizeof(SEC_PORTOPEN_T)      //最大记录10个以太网远程服务端口,上报标记(1) + 端口(2)
#define TcpExtConnInfoLen       sizeof(SEC_CONNECT_T)     //最大记录10个以太网外联的信息,上报标记(1) +(IP + ":"+PORT  30),    192.168.10.146:2145
#define UsbDevAccessInfoLen     2                   //USB设备接入信息,      上报标记(1)+ 信息(1)
#define LocalMaintenanceInfoLen 2+2+2               //本地维护端口信息,上报标记(1)+ 信息(1)(232 485 红外各自使用各自的标记)
#define KeyDirChangeInfoLen     sizeof(KEY_DIR_FILE_VECTOR)     //关键目录变更信息,上报标记(1)+ 目录路径(50)
#define TcpLoginTtyInfoLen      4                   //以太网登录控制台信息,  数量
#define SerialLoginTtyInfoLen   4                   //串口登录控制台信息,  数量
#define FlowLimitParamLen       sizeof(SEC_SESSION_LIMIT_T)            //流量超限参数 月累计流量限值(4) 流速限值(4)
#define SverLimitParamLen       sizeof(SEC_SRVTIME_LIMIT_T)            //服务长时间未关闭参数

#define RecordSartAddr              0
#define TcpRemoteSrvPortAddr        RecordSartAddr                                      //以太网远程服务端口起始地址
#define TcpExtConnInfoAddr          TcpRemoteSrvPortAddr+TcpRemoteSrvPortLen            //以太网外联的信息起始地址
#define KeyDirChangeInfoAddr        TcpExtConnInfoAddr+TcpExtConnInfoLen                //关键目录变更信息起始地址
#define TcpLoginTtyInfoAddr         KeyDirChangeInfoAddr+KeyDirChangeInfoLen            //以太网登录控制台信息起始地址
#define SerialLoginTtyInfoAddr      TcpLoginTtyInfoAddr+TcpLoginTtyInfoLen              //串口登录控制台信息起始地址
#define FlowLimitParamAddr          SerialLoginTtyInfoAddr+SerialLoginTtyInfoLen        //流量限值
#define SverLimitParamAddr          FlowLimitParamAddr+FlowLimitParamLen                //服务长时间未关闭参数

#define SEC_DEFAULT_TIME    (0)  //默认判断时长
#define SEC_DEFAULT_FLOW    (0)  //默认流量
#define SEC_DEFAULT_BAND    (0)  //默认带宽

typedef struct
{
    char ipAddr[20];       /*IP地址，用于TCP/IP客户端通道*/
    uint16 port;                /*端口号*/
}LINK_ADDR;

typedef struct
{
    char    state[20];
    char    local_ip[20];
    char    foreign_ip[20];
    uint16  local_port;
    uint16  foreign_port;
}POINT_INFO;

//用户登录类型
typedef enum
{
    LOGIN_ETH = 0, //以太网
    LOGIN_UART,     //串口
}SEC_LOGINTYPE_E;

//用户登录信息结构体
typedef struct
{
    char    user[20];     //登录的用户名
    char    tty[20];      //登录的终端
    char    time[20];     //登录的时间
    char    host[32];     //登录的主机
}SEC_LOGININFO_T;

typedef struct
{
    char    usrname[64];  //失败的用户名
    char    loginip[20];  //登录失败的IP
    int     logincnt;     //登录次数
    time_t  failtime;     //失败记录时间
}SEC_FAILLOGIN_T;

typedef struct
{
    uint8 status;
    uint32 conTime; //SSH服务持续时间 单位：秒
}SEC_SSHINFO_T;

/* 串口服务状态 */
typedef enum tag_SEC_WORKSTATE_E
{
    SEC_CONF = 0,  //配置状态
    SEC_PARA = 1,  //参数状态 
    SEC_WORK = 2,  //工作状态 
}SEC_WORKSTATE_E;

typedef struct
{
    char    local_ip[20];
    char    foreign_ip[20];
    uint16  local_port;
    uint16  foreign_port;
    char    protocol[4];   //协议类型
    char    process[32];   //关联进程
    uint8   procType;      //进程类型 @ref SEC_PROCTYPE_E
    uint32  monthFlow;     //月累积流量 KB
    uint32  flowSpeed;     //流速 Kbp/s
    uint32  calcFlow;      //用于计算流速的累积流量
    uint8   calcUpdate;    //是否在计算流速，计算流速时抓包线程不刷新calcFlow    
    uint8   monUpdate;     //是否在清空月流量，此时抓包线程不刷新monthFlow
    int     updateTime;    //流最近一次刷新时间
}SEC_SESSIONAPP_T;

typedef struct
{
    uint8 nNum;
    SEC_SESSIONAPP_T info[SEC_RECORD_MAX];
}SEC_SESSIONAPPS_T;

typedef struct
{
    uint8 nNum;
    int   procinfo[65535];
}SEC_PROCINFOS_T;

/* 串口服务线程数据 */
typedef struct tag_SEC_MGRDATA_T
{
    uint8          status;    //状态
    uint8          res[2];
    uint8          port;
    DB_CLIENT      clientid;
    int            tasksocket;
    time_t         tasktimeout; //任务超时时间
    uint8          tcpInfoNum;  //扫描的TCP信息
    POINT_INFO     tcpInfo[SEC_RECORD_MAX];    
    uint8          connWhiteListNum; //TCP连接信息白名单
    LINK_ADDR      connWhiteList[SEC_RECORD_MAX];
    time_t         curtime;       //当前时间    
    time_t         lsttime;       //上次时间
}SEC_MGRDATA_T;


typedef struct
{
    uint16  mode;                   /*文件类型和权限*/
    time_t  mtime;                  /*文件修改时间*/
    char    name[KEY_MAX_FILE_LEN]; /*文件名*/
}__attribute__ ((packed)) FILE_ATTR;//文件属性

typedef struct
{
    uint8        filenum;   /*扫描到的文件个数*/
    FILE_ATTR    file_vector[KEY_MAX_FILE_NUM];
}__attribute__ ((packed)) KEY_DIR_FILE_VECTOR;


/* 入口函数申明 */
typedef int (*SEC_AXDRMSG_FUN)(MESSAGE_INFO_T*, SEC_APPDATA_T*, SEC_MGRDATA_T*);

typedef struct tag_SEC_AXDR_FUN_T
{
    uint32               IOP;      /** < 标识 */
    SEC_AXDRMSG_FUN     pProcMsg; /** < 处理函数 */
}SEC_AXDR_FUN_T;

/* 入口函数申明 */
typedef int (*SEC_JSONMSG_FUN)(MESSAGE_JSON_T*, SEC_APPDATA_T*, SEC_MGRDATA_T*);

typedef struct tag_SEC_JSON_FUN_T
{
    char               *infopath; /** < 标识 */
    SEC_JSONMSG_FUN    pProcMsg; /** < 处理函数 */
}SEC_JSON_FUN_T;

int sec_manager_init(void *pPublic, void **ppPrivate);
int sec_manager_proc(void *pArg, void *pPublic, void *pPrivate);
int sec_session_update(void);

#ifdef __cplusplus
}
#endif

#endif /* __SEC_SERVICE_H */


