/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块公用定义申明
* @date：    2019-10-23
* @history： 
*********************************************************************
*/

#ifndef __WIRELESS_H__
#define __WIRELESS_H__

#include "dataType.h"
#include "oopType.h"
#include "framecomm.h"

#ifdef __cplusplus
extern "C" {
#endif

//初始化 APP注册
#define WIRELESS_STATUS_REGAPP     0
//查询APP信息
#define WIRELESS_STATUS_SEEKAPP    1
//工作状态工作
#define WIRELESS_STATUS_WORK       2

//查询端口
#define WIRELESS_SEEK_PORT    0
//查询端口
#define WIRELESS_SEEK_INFO    1
//查询模组插拔状态
#define WIRELESS_SEEK_ONLINE  2
//模组工作状态
#define WIRELESS_SEEK_WAIT    3

//使用USB通信
#define WIRELESS_COMMTYPE_USB        0
//使用串口通信
#define WIRELESS_COMMTYPE_UART       1

#define WIRELESS_DCM_NAME             "M-wirelessDCM"

#define WIRELESS_UNUSED_COMMPORT      0
#define WIRELESS_MOD_ID_MIN           1
#define WIRELESS_MOD_ID_MAX           5
#define WIRELESS_QUEUE_SIZE           15
#define WIRELESS_BUF_LENGTH           5120	/*缓冲区大小，共用缓冲区及各线程缓冲区大小*/

#define SMS_PDU_PHONE_LEN             32        /*电话号码占字节数*/

#define MODE_DIAG_TYPE_NAME           "GY"   /* G-功能模块; Y-远程通信模块 */

typedef struct tagTime
{
    uint8 year;  /*年份0代表1900年*/
    uint8 month; /*月份0代表1月*/
    uint8 day;
    uint8 hour;
    uint8 minute;
    uint8 second;

    uint16 milliseconds;
}TIME_STRUCT_T;    /*时间结构*/

/** @brief 运营商信息	*/
typedef enum tag_OPERATOR
{
	WIRELESS_CMCC = 0,
	WIRELESS_CUCC = 1,
	WIRELESS_CTCC = 2,
	WIRELESS_ZWCC = 3, /* 专网 */
	WIRELESS_NOCC = 4, /* 任意运营商 */
	WIRELESS_OPT_NONE = 255
}OPERATOR_E;

typedef struct tagWirelessGPSSET
{
    char      setcmd[24];
    char      getcmd[24];
    uint8     gps;
    uint8     bd;
    uint8     both;
    uint8     def;
}GPS_SET_T;

typedef struct tagGprsPara
{
    WIRE_DIAG_APN_T   defaultAPN;  /* 默认的拨号参数 */
    uint16            SimCheckTime;/* SIM卡连续检测失败，重启模块时间 */
    uint8             res[1];

    uint8             networkSet; /* 用户锁定的网络 */
    WIRE_DIAG_APN_T   userAPN;    /* 用户设置的拨号参数 */
    uint8             authType;   /* 设置鉴权方式 0-null, 1-chap, 2-pap, 3-pap&chap, 255-自适应 */
                                  /* AT指令: 0-NONE,1-PAP,2-CHAP,默认为1,3-pap&chap */
    uint8             smsStatusOK; /* 短信线程状态 */
    uint8             smsType;    /* 短信格式 PDU（0），TEXT（1）*/
    uint8             centerNnm[SMS_PDU_PHONE_LEN + 1]; //设置短信中心号码
    uint32            flowSetValue;  //流量控制 单位 KB
    uint64            usedflow;      //当月使用的流量 单位B
    TIME_STRUCT_T     usertime;      //流量统计时间

    struct
    {
        uint8  manuNum[4 + 1];      /* 厂商代码 */
        uint8  softversion[4 + 1];  /* 软件版本号 */
        uint8  softdata[6 + 1];     /* 软件版本日期 */
        uint8  hwversion[4 + 1];    /* 硬件版本号 */
        uint8  hwdata[6 + 1];       /* 硬件版本日期 */
        uint8  manuExten[8 + 1];    /* 厂商扩展信息 */
    }VERINFO; /* 版本信息 */
	
    uint8      IMSI[15+1];
    uint8      ICCID[20+1];     /* SIM 卡 ICCID */
    uint8      networkType;     /* 实际网络类型 */
    
    char       IMEI[17];
    
    uint8      operat;      /* 运营商 */
    int16      csqvalue;    /* 信号强度 */
    int16      basecsqvalue;/* 基准信号强度 */
    uint8      SIMNum[24];  /* SIM 卡号码 */
    
    uint32     ipAddr;      /* 网口IP地址 */
    uint32     mask;        /* 子网掩码 */   
    uint32     getway;      /* 网关 */

    uint8      moduleType;  /* 模块型号 */
    char       nettype[3];

    char       gpsHalName[32];  /* GPS模块名称(HAL),为空表示复用AT口 */
    GPS_T     *pGps;

    GPS_SET_T *pGpsSet;
}NET_PARA_T;

typedef struct tagQueueMsg	/*通讯数据队列*/
{
	uint32 len;                           /*数据长度*/
	uint8 buf_queue[WIRELESS_BUF_LENGTH]; /*队列存储缓冲区*/
}WIRELESS_QUEUEMSG_T;

typedef struct tagLockQueue	/*锁共享缓冲区结构*/
{
	pthread_mutex_t     f_lock;	            /*接收缓冲区互斥锁*/
	int16               head;               /*队列头*/
	int16               tail;               /*队列尾*/
	WIRELESS_QUEUEMSG_T queue[WIRELESS_QUEUE_SIZE]; /*通讯数据队列*/
}WIRELESS_QUEUE_T;

#define PPP_DAIL_STATUS_ONLINE   0
#define PPP_DAIL_STATUS_OFFLINE  1

typedef struct tagWirelessHandle
{
    char              devName[256]; /* 实际设备名称/dev/tty* */
    char              halName[32]; /* hal接口设备符 */

    uint8             commtype;    /* 通信方式 */
    uint8             no;          /* 序号 0-1 */
    uint8             workFlag;    /* 工作状态 0-需要立刻停止 */
    uint8             workAgain;   /* 工作状态 重新开始 */
    
    uint8             commport;    /* 模组所在端口, 集中器设置为FF */
    uint8             subport;     /* 子端口 用于多模组 从1开始 */
    uint8             bsimOK;      /* sim卡是否插入 0-插入, 1-没有*/
    uint8             dialState;   /* 拨号状态 0-成功, 1-失败 */
    
    uint8             stackChoice; /* 协议栈 */
    uint8             apnIndex;    /* apn选择, 存在用户配置使用用户的配置 */
    uint8             bUserApn;    /* 是否用户设置的APN */
    uint8             bExistGPS;   /* 是否存在GPS模块 */

    uint32            stepStatus;  /* 状态机 */

    TIME_STRUCT_T     curDay;      /* 当前日期 */
    uint32            ConnectNum;  /* 当日连接次数 */
    uint32            FailedNum;   /* 当日失败次数 */
    uint32            SuccessTimes;/* 连接成功时间s */
    TIME_STRUCT_T     Connecttime; /* 连接时间 */
    
    OOP_VISIBLEVAR_T  apn;         /* 拨号的APN */
    OOP_VISIBLEVAR_T  user;        /* 拨号的用户名 */
    OOP_VISIBLEVAR_T  pwd;         /* 拨号的密码 */

    NET_PARA_T       *ptNetPara;   /* 网络参数 */
    WIRELESS_QUEUE_T *ptMsgQueue;  /* 线程处理消息队列 */
    void             *pUart;       /* 串口句柄 */
    char             *userfile;    /* 用户文件地址 */

    uint8             sendfaild;   /* 发送失败数量 */

    uint8             logNo;       /* 拨号日志序号 */
    uint8             pppdlogMax;  //pppd日志数量
    uint8             bIPV6;       //pppd日志上限
    char              logpath[256];//日志路径
	
    uint8             bJS1_8G;      //是否支持江苏1.8G
    uint8             curGpsMode;  //当前GPS模式
    uint8             setGpsMode;  //设置GPS模式
    uint32            getGpsTime;  //获取GPS模块时间
}WIRELESS_HANDLE_T;

typedef struct tagWirelessCCUMana
{
    uint8    bInsert;     /* 是否插入模块 */
    uint8    step;        /* 执行步骤 */
    uint8    bUsb;        /* 是否存在USB模块 */
    uint8    commType;    /* 通信方式 */

    uint8    bChangeComm; /* 切换通信方式请求 */
    uint8    bForceUrat;  /* 是否强制使用串口 */
    uint8    res[2];
    
    uint32   count;       /* 计数器 */
    uint32   powerofftime;/* 断电时间设置 */
    
    time_t   restartTime; /* 重启时间 */
    time_t   checkTime;   /* 检查时间 */

    void    *pDevHal;     /* 管理句柄 */
}WIRELESS_CCU_T;

typedef struct tagWirelessDATA
{
    uint32          mqttip;       //mqtt地址
    uint16          mqttport;     //mqtt端口
    uint8           res[1];
    uint8           appidlen;
    uint8           appid[24];

    uint8           mapReg;       //模组管理器注册
    uint8           guiReg;       //桌面GUI注册
    uint8           bMapExist;    //模组管理器是否存在
    uint8           bJS1_8G;      //是否支持江苏1.8G

    WIRELESS_CCU_T      CCUmanager;  /* 集中器方式管理 */
    MOD_INFO_NEW_T      module[2];   /* 模组信息 最多支持两路 */
    WIRELESS_HANDLE_T   atdata0;     /* 设备1 AT */
    WIRELESS_HANDLE_T   pppdata0;    /* 设备1 拨号数据 */
    WIRELESS_HANDLE_T   atdata1;     /* 设备2 AT */
    WIRELESS_HANDLE_T   pppdata1;    /* 设备2 拨号数据 */
}WIRELESS_DATA_T;


void wireless_lock_init();
int is_str_exist(char *source_str, char *res_str);
void get_curtime_ms(TIME_STRUCT_T *pTime);
void set_curtime_ms(uint32 linuxTime, TIME_STRUCT_T *pTime);
uint32 read_queue(WIRELESS_QUEUE_T *plockqueue, uint8 *buf);
int write_queue(WIRELESS_QUEUE_T *plockqueue, uint8 *buf, uint32 buf_len);
void clear_queue(WIRELESS_QUEUE_T *plockqueue);
int wireless_get_apn(WIRELESS_HANDLE_T *ptHandle);
void wireless_set_nextapn(WIRELESS_HANDLE_T *ptHandle);
void wireless_atshow_set(void *pUart);
void wireless_atshow_get(char *sendbuf, uint16 sendbuflen, TIME_STRUCT_T *pSendTime,
                         char *recvbuf, uint16 recvbuflen, TIME_STRUCT_T *pRecvTime);
void *wireless_uart_init(char *device);
void wireless_uart_uninit(void *pDev);
int wireless_uart_send(void *pDev, uint8* data, int len);
int wireless_uart_recv(void *pDev, uint8* buf, int bufLen);
int wireless_uart_getname(void *pDev, uint8* pName);
void wireless_set_pppstatus(uint8 ifno, uint8 status);
int wireless_send_change(uint8 id, uint8 netmode, uint16 csqvalue);
int wireless_send_diagStatus(uint8 id, char *str, uint16 len, uint32 times);
void wireless_get_default_conf(char *filepath, void *ptApn, uint16 *pTime, uint8 *bIpv6, uint8 *lognum,
                                            char *plogpath);
uint8 wireless_check_1_8G();

void wireless_check_userfile(char *filename);
void wireless_get_user_apn(char *filename, uint8 id, void *ptApn);
int wireless_write_user_apn(char *filename, uint8 id, void *ptApn);
uint8 wireless_get_authtype(char *filename, uint8 id);
int wireless_write_authtype(char *filename, uint8 id, uint8 type);
int wireless_get_centernum(char *filename, uint8 id, uint8 *pNum, uint8 len, uint8 *type);
int wireless_write_centernum(char *filename, uint8 id, uint8 *pNum, uint8 len, uint8 type);
uint32 wireless_get_flowset(char *filename, uint8 id);
int wireless_write_flowset(char *filename, uint8 id, uint32 value);
uint64 wireless_get_userflow(char *filename, uint8 id, TIME_STRUCT_T *pTime);
int wireless_write_userflow(char *filename, uint8 id, uint64 value);
int wireless_get_num(char *filename, uint8 id, TIME_STRUCT_T *pdaytime, uint32 *cnum, uint32 *fnum, uint32 *stime);
int wireless_write_num(char *filename, uint8 id, TIME_STRUCT_T *pdaytime, uint8 type, uint32 timeornum);
int wireless_get_logno(char *filename, uint8 id, uint8 *no);
int wireless_write_logno(char *filename, uint8 id, uint8 no);
int wireless_get_gpsmode(char *filename, uint8 id, uint8 *mode);
int wireless_write_gpsmode(char *filename, uint8 id, uint8 mode);

void wireless_restore_mode(uint8 ifnum, uint32 port);
void wireless_reboot_mode(uint8 ifnum, uint32 port);

WIRELESS_HANDLE_T *wireless_get_handle(uint8 id);
int wireless_set_apn(uint8 id, uint8 operate, char *apn, char *user, char *pwd);
int wireless_set_auth(uint8 id, uint8 authType);
int wireless_diag_again(uint8 id);
char* wireless_get_version(Date_Time_S *ptTime);
void wireless_app_stop();
void wireless_hw_reboot(uint32 powerOffTime);

void wireless_restart_dev(uint8 ifnum, uint8 devid);
uint8 wireless_get_restartdev(uint8 ifnum);
void wireless_change_type(uint8 type);
void wireless_get_appid_gui(uint8 *buf, uint8 len);
void wireless_power_status(uint8 status);

#ifdef __cplusplus
}
#endif

#endif //__WIRELESS_H__


