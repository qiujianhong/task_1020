/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程通用数据和操作
* @date：    2019-12-3
* @history： 
*********************************************************************
*/
#ifndef __APPCOMM_H
#define __APPCOMM_H

#include "commuprot.h"
#include "OopAll.h"
#include "db_api.h"

#ifdef __cplusplus
        extern "C" {
#endif

#define APP_TRHEAD_FLAG_MUX       120

#define APP_698_MESSAGE_MIN_LEN   13    //报文最小长度
#define APP_698_HEAD_MIN_LEN      8     //报文帧头最小长度

#define APP_698_LEN_REGIN_SIZE    2     //报文长度域长度
#define APP_698_CTL_REGIN_SIZE    1     //报文控制域长度
#define APP_698_SA_REGIN_MAXSIZE  17    //报文SA最大长度
#define APP_698_CA_REGIN_SIZE     1     //报文CA长度
#define APP_698_CS_REGIN_SIZE     2     //报文校验长度
#define APP_698_SEPARA_REGIN_SIZE 2     //报文APDU分段标记

#define APP_698_A_REGIN_OFFSET    4     //地址域偏移

#define APP_698_HEAD_BASE_SIZE    \
    (APP_698_LEN_REGIN_SIZE + APP_698_CTL_REGIN_SIZE +  1 + APP_698_CA_REGIN_SIZE + APP_698_CS_REGIN_SIZE)

#define CHECK_RESULT_SUCCEED      0
#define CHECK_RESULT_FAILUER      -1
#define CHECK_RESULT_CONFIRM      1
#define CHECK_RESULT_DENY         2

typedef struct FrameHeadInfo_t_
{
    uint8 pipe;//报文通道号
	LinkLayerCtrl_t ctrl;//控制码
	OOP_TSA_T tsa;
	OOP_OAD_U oad;
    uint8 cAddr;
}FrameHeadInfo_t;

typedef struct ProxyHeadInfo_t_
{
    FrameHeadInfo_t FrameHeadInfo;
    SecurityMode_t   SecMod;
    uint8            Useid;
    uint16           label;
    uint8            PIID;
}ProxyHeadInfo_t;


typedef struct 
{
	uint8 PIID;
	uint8 ServerID;
	uint8 nChioce;
	SecurityMode_t SecMod;
	FrameHeadInfo_t FrameHeadInfo;		//帧头信息
//	PROXY_STATE state;					//穿透抄任务状态
//	time_t RelayStartTime;					//中继开始时间
//	time_t SendTime;						//发送的起始时间
//	uint16 TimeOut;							//任务超时时间
	uint16 RequestApduLen;					//代理请求的APDU长
	uint8  pRequestApdu[1400];					//任务请求数据
}Proxyresquest;

typedef struct 
{
	uint8 PIID;
	SecurityMode_t SecMod;
	FrameHeadInfo_t FrameHeadInfo;		//帧头信息
//	PROXY_STATE state;					//穿透抄任务状态
//	time_t RelayStartTime;					//中继开始时间
//	time_t SendTime;						//发送的起始时间
//	uint16 TimeOut;							//任务超时时间
	uint16 ResponseApduLen;					//代理请求的APDU长
	uint8  ResponseApdu[2100];					//任务请求数据
	uint8 ServerID;
    uint8 UseID;
}ProxyResponse;

typedef struct
{
    uint16  Queue_Len;                     //队列尾
    uint8   Queue_Data[2200];               //通讯数据队列
}Proxy_Queue; 

typedef struct
{
	pthread_mutex_t QueueLock;                  //接收缓冲区互斥锁
    int16  QueueHead;                          //队列头
    int16  QueueTail;                          //队列尾
    Proxy_Queue Queue[10];               //通讯数据队列
}ProxyLockQueue; 

typedef struct tag_APPURTENANT_METER_INFO    
{
   uint8 pipeaddr[6];//虚拟通道地址
   uint8 meteraddr[6];//电能表地址
   uint8 phaseinfo;
   uint16 pn;
   

}OOP_APPURTENANT_METER_INFO_T;

typedef struct tag_APPURTENANT_METER_LIST    
{
   uint8 num;//电表数量
   OOP_APPURTENANT_METER_INFO_T meterinfo[12]; 

}OOP_APPURTENANT_METER_LIST_T;

typedef struct tag_PERCEPTION_INFO    
{
   uint8 terminaladdr[6];//感知终端地址
   OOP_APPURTENANT_METER_LIST_T meterlist;  

}OOP_PERCEPTION_TERMINAL_INFO_T;

typedef struct tag_PERCEPTION_TERMINAL    
{
   //pthread_mutex_t lock;
   uint8 num;//感知终端数量
   OOP_PERCEPTION_TERMINAL_INFO_T terminalinfo[96];  
   uint16 totalpn;

}OOP_PERCEPTION_TERMINAL_LIST_T;

typedef  struct
{
    uint8 	AFN;                                //应用功能码
    uint16 	Fn;                                 //信息类标识
    uint8   ModuleID;                           //通信模块标识(0:集中器 / 1:载波表)
    uint8   DesAddr[6];                         //目的地址
    uint8   RInfo[6];                           //信息域
    uint16 	DataUnitLen;                        //应用数据长度
    uint8 	DataUnitBuff[500];                  //应用数据缓冲区
} AppData_T; 

typedef struct
{
    uint16  Queue_Len;                          //队列尾
    uint8  Queue_Data[256];               //通讯数据队列
}Trans_Queue; 

typedef struct
{
	pthread_mutex_t QueueLock;                  //接收缓冲区互斥锁
    int16  QueueHead;                          //队列头
    int16  QueueTail;                          //队列尾
    Proxy_Queue Queue[10];               //通讯数据队列
}TransLockQueue; 


typedef void *(*EntryFun)(void *);
typedef struct tagApp698Thread
{
    pthread_t          nThreadId;   /* 线程ID */
    EntryFun           pEntryFunc;  /* 线程入口函数 */
    void              *pArg;        /* 函数入参 */
    uint8             *pRunFlag;    /* 线程运行标记, 线程执行设置为0 */
}APP698_TRHEAD_T;

#define LOCAL_RECV_FRAM_MAXLEN     16384  //终端最大发送帧长度 客户点最大接收能力
#define LOCAL_SEND_FRAM_MAXLEN     2048  //终端最大接收帧长度 客户点最大发送能力
#define LOACL_PROC_FRAM_MAXLEN     16384 //终端最大处理帧长度
#define LOCAL_SEND_FRAM_MINLEN     1024  //终端最大接收帧长度 客户点最小发送能力
#define RS485_SEND_FRAM_MINLEN     512   //终端最大接收帧长度 客户点最小发送能力
#define SMS_FRAM_MAXLEN            140   //短信分帧长度
#define RADIO_FRAM_MAXLEN          255   //电台分帧



typedef struct TimeTagInfo_t_
{
    BOOL   isTimeOut;//是否超时
    uint8  tagFlag;  //时间标签值
    uint8  tagBuff[10];//时间+间隔
}TimeTagInfo_t;

typedef struct tagAppConectInfo
{
    uint16                  servSendMaxLen;      // 服务器最大发送长度
    uint16                  servRecvMaxLen;      // 服务器最大接收长度
    uint8                   servRecvMaxNum;      // 服务器最大可处理的APDU单元窗口尺寸
    uint16                  servMaxApduLen;      // 服务器最大可处理的APDU单元

    uint16                  clientSendMaxLen;    // 客户机最大发送长度
    uint16                  clientRecvMaxLen;    // 客户机最大接收长度
    uint8                   clientRecvMaxNum;    // 客户机最大可处理的APDU单元窗口尺寸
    uint16                  clientMaxApduLen;    // 客户机最大可处理的APDU单元
    uint8                      SecurityLevel;       // 应用连接协商出的密码权限
    SecurityMode_t          curSecurityMode;     // 本帧加密模式
    LinkLayerAddr_t         addrInfo;            // 下行帧的地址类型
    BOOL                    SecurityServer;         //本帧是否是安全服务 TRUE:安全传输服务，FALSE:普通服务
    TimeTagInfo_t           timeTag;             //时间标签信息
}APP_CPNNECT_INFO_T;

typedef struct{
    uint8  id;//服务ID
    uint8  piid;
    uint16 version;      //协议版本号
    uint8  prtlConf[8];  //协议一致性
    uint8  funcConf[16]; //功能一致性
    uint16 clientSFML;   //客户机发送帧长度
    uint16 clientRFML;   //客户机接收帧长度
    uint8  clientWS;     //客户机接收窗口大小
    uint16 clientPFML;   //客户机最大可处理帧尺寸
    uint32 timeOut;      //应用连接超时时间
    uint8  mechains[0];  //应用连接请求认证的机制信息
}__attribute__ ((packed)) ConnectRequest_t;


typedef struct{
    uint8   factory[4];  /**< 厂商代码           */
    uint8   softVer[4];  /**< 软件版本号         */
    uint8   softDate[6]; /**< 软件版本日期       */
    uint8   hardVer[4];  /**< 硬件版本号         */
    uint8   hardDate[6]; /**< 硬件版本日期       */
    uint8   extend[8];   /**< 厂家扩展信息       */
}__attribute__ ((packed)) Factory_Version_t;

typedef struct{
    uint8  result;    //应用连接请求认证的结果
    uint8  data[0];   //认证附加信息
}__attribute__ ((packed)) ConnectResponseInfo_t;

typedef enum AppConnectResult
{
    CONNECT_PERMISSION    = 0, //允许建立应用连接
    CONNECT_ERR_PASSWORD  = 1, //密码错误
    CONNECT_ERR_SYMMETRY  = 2, //对称解密错误
    CONNECT_ERR_NSYMMETRY = 3, //非对称解密错误
    CONNECT_ERR_SIGNATURE = 4, //数字签名错误
    CONNECT_ERR_VERSION   = 5, //协议版本不匹配
    CONNECT_ERR_OTHER     = 255, //其他错误
}CONNECT_RESULT_E;

typedef struct{
    uint8  id;//服务ID
    uint8  piid_acd;
    Factory_Version_t factoryVersion;
    uint16 version;      //协议版本号
    uint8  prtlConf[8];  //协议一致性
    uint8  funcConf[16]; //功能一致性
    uint16 serverSFML;   //服务器发送帧最大尺寸
    uint16 serverRFML;   //服务器接收帧长度
    uint8  serverWS;     //服务器接收窗口大小
    uint16 serverPFML;   //服务器最大可处理帧尺寸
    uint32 timeOut;      //应用连接超时时间
    ConnectResponseInfo_t  response;  //应用连接请求的认证响应信息
}__attribute__ ((packed)) ConnectResponse_t;

/* 连接属性 (curSecurityMode securityServer timeTag 不属于应用连接 需要删除 可以在FRAME_HEAD_T尾部添加) */
typedef struct tagAPPContex
{
    uint16 prtlVersion;              //协议版本号
    uint16 recvFrameLen;             //接收帧长度 终端接收能力
    uint16 sendFrameLen;             //发送帧长度 综合了客户机和服务器能力 用此作为分帧基准
    uint16 recvApduLen;              //Apdu长度
    uint8  prtlConformance[8];       //协议一致性  综合了客户机和服务器设置
    uint8  funcConformance[16];      //功能一致性  综合了客户机和服务器设置
    uint8  recvWindowSize;           //接收窗口大小
    uint8  securityMode;             //加密模式 SECURITY_MODE_E 应用连接携带
    uint16 ApplSplitLen;             //应用分帧大小程序定制值设置
    uint32 timeOut;                  //静态超时时间

    time_t estalishTime;             //应用连接建立时间
    SecurityMode_t curSecurityMode;  // 本帧加密模式 
    BOOL securityServer;		     //本帧是否是安全服务 TRUE:安全传输服务，FALSE:普通服务
    TimeTagInfo_t           timeTag; //时间标签信息
}APP_CONTEXT_t;

typedef enum AppSecurityMode
{
    NULL_SECURITY_MODE  = 0, //公共连接
    PWD_SECURITY_MODE   = 1, //一般密码
    SYM_SECURITY_MODE   = 2, //对称加密
    SIG_SECURITY_MODE   = 3, //数字签名
}SECURITY_MODE_E;

#define APP_RECV_BIGMSG_MAXLEN  300000  //最大考虑首尾2个字节 
#define APP_RECV_MSG_MAXLEN     16388   //最大考虑首尾2个字节 
#define APP_RECV_IPC_MAXLEN     65024   //调整到最大了，不能再增加
#define APP_SEND_MSG_MAXLEN     16388   //最大考虑首尾2个字节 
#define APP_645PRTL_MINLEN      12      //645规约最小长度68(1)+addr(6)+68(1)+C(1)+L(1)+CS(1)+16(1)

#define LINK_NO         0x00    //无操作
#define LINK_LOGIN      0x01    //登录
#define LINK_HEART      0x02    //心跳
#define LINK_QUIT       0x03    //退出登录
#define LINK_CHANGE		0x04	//状态切换

#define LOGIN_INTERVAL  30      //尝试登陆的间隔时间，10~20s
#define HEART_AHEAD     5       //心跳时间达到之前，提前发送一下心跳

typedef enum AppLockUser
{
    APP698_NO_LOCK   = 0,
    APP698_PROC_LOCK,
    APP698_RIPC_LOCK,
    APP698_SIPC_LOCK,
    APP698_AUTO_LOCK,
    APP698_DEBUG_LOCK,
}APP_LOCKUSER_E;

/* APDU断点删除回调函数 */
typedef void (*APDUBreakFree)(void **);
/* APDU断点清理回调函数 */
typedef void (*APDUBreakZero)(void *);
/* 通道结构体 */
typedef struct
{
    uint8            id;         //通道id 0~255
    uint8            bOpen;      //通道开启标记 1-开启/0-关闭
    uint8            linkState;  //预连接状态 登录-心跳
    uint8            bConnect;   //是否建立应用连接
    CHANNEL_INFO_T   tChannel;   //通道属性
    APP_CONTEXT_t    tContext;   //应用连接属性
    pthread_mutex_t  fWork;      //工作标记, 线程池用

    uint16           leftHB;         //剩余心跳时间
    uint8            lostHBCount;    //丢失与连接/心跳次数
    uint8            heartbeatflag;  //心跳对时时用 屏蔽正常心跳

    uint32           startTime;      //开始处理时的时间 struct timespec的s
    uint32           endTime;        //结束处理时的时间 struct timespec的s
    uint8            bNeedClose;     //需要执行关闭操作, 通道工作是关闭失败，需要设置此标记
    uint8            workFlag;       //工作标记
    uint8            longFlag;       //是否长时间处理标记

    uint8            bmsgExtLen;     //扩展长度的消息标签
    uint8            bmsgExtOver;    //扩展长度的接收完成
    uint8            bMsgExtProc;    //处理标记
    uint16           lostHBtime;     //心跳持续丢失时间
    uint32           recvExtLen;     //收到的长度
    uint32           extMsgLen;      //报文中消息长度 包括首尾
    uint8           *extMsg;         //超长消息缓存

    uint32           extOutMsglen;   //发送的扩展长度消息长度
    uint8            extIndex;       //消息索引
    uint8            extSendFlag;    //发送成功标记
    uint8            res1[1];

    uint8            bSafe;          //下行接收是否安全模式
    pthread_mutex_t  fRecv;          //接收缓存锁
    uint8            recvLockUser;   //通道用户   APP_LOCKUSER_E
    uint8            msgnum;         //缓存报文数
    uint16           totalLen;       //接收队缓存消息总长度
    uint8            recv[APP_RECV_IPC_MAXLEN];  //接收IPC缓存
    time_t           checkTimeOK;    //接收缓存检测成功判断时间超过10s不成功需要设置第一个字节为0

    uint16           procLen;        //处理消息长度
    uint16           blocknum;       //分帧序号
    uint8            ucMess[APP_RECV_MSG_MAXLEN]; //封装出来的完整报文

    pthread_mutex_t  fSend;           //发送缓存锁
    uint8            sendLockUser;    //通道用户  APP_LOCKUSER_E
    uint8            res2[1];
    uint16           sendMessLen;     //发送消息长度
    uint16           curblock;        //当前待发送帧序号
    uint8            permSend;        //允许发送标记
    uint8            protocol;        //私有协议-操作
    uint8            sendMess[APP_SEND_MSG_MAXLEN]; //发送缓存 可处理APDU分段发送

    DB_CLIENT        clientid;        //数据中心句柄

    uint8            bSendApduBreak;  //上行应用分帧标记 1-设置分帧，0-不分帧(结束分帧)
    uint8            res3[3];
    uint32           BreakMsgLen;
    uint8           *pBreakBakMsg;    //应用分帧缓存消息
    void            *pApduBreak;      //APDU断点
    APDUBreakFree    pFreeFunc;       //断点free回调函数

    uint8            private[12];      //私有数据, 通道不维护
                                      //0-逻辑设备 1-pApdu[1] 2-是否需要记录编程事件标记 
                                      //3-是否只读数据不解析 4-交采模块特殊命令 
                                      //5-是否时为上报组织的虚拟报文[1是, 0否]
                                      //6-上报周期值（山东特有）
                                      //7-上报周期单位（山东特有）
                                      //8-表示是否是上报触发的规约召测；1，表示是上报召测触发的，0表示是正常的规约召测
                                      //9-新的上行消息是否应用分帧 1-是 0-否
                                      //10 1表示不验安全模式 打高分批量读数据的时候
}CHANNEL_CACHE_T;

#define SERVER_ADDR_MAXLEN         16
#define RTU_ADDRESS_SINGLE           0            //单地址-
#define RTU_ADDRESS_WILD             1             //通配地址-
#define RTU_ADDRESS_TEAM             2             //组地址-
#define RTU_ADDRESS_BROADCAST        3             //广播地址-
typedef struct tagServerCLIENTAddr
{
    uint8   len;       //服务端地址长度
    uint8   logicAddr; //逻辑地址扩展 0-255
    uint8   type;      //0-单播 1-通配 2-组播 3-广播
    uint8   res[1];
    uint8   addr[SERVER_ADDR_MAXLEN];
}SERVER_ADDR_T;

#define  LINK_SEPARATE_MAX_BLOCK    4095
#define  LINK_SEPARATE_START        0
#define  LINK_SEPARATE_END          1
#define  LINK_SEPARATE_ACK          2
#define  LINK_SEPARATE_MIDDLE       3
typedef struct tagLinkSeparate
{
    uint16     block;  //序号
    uint8      type;   //分帧类型
    uint8      res;
}LINK_SEPARATE_T;

#define MESSAGE_PRM_FROM_SERVER   0
#define MESSAGE_PRM_FROM_CLIENT   1
#define MESSAGE_DIR_FROM_CLIENT   0
#define MESSAGE_DIR_FROM_SERVER   1
typedef struct tagFrameHead
{
    uint32          msglen;  //支持长度扩展的协议 不包括起始结束 字符
    uint8           bExtmsg; //是否长度扩展 该标记设置后 需要使用

    uint8           headlen; //帧头长度 不包括起始字符 包括HCS校验
    uint8           funCode; //功能码
    uint8           sc;      //扰码
    uint8           separate;//分帧标志
    uint8           prm;     //启动标志
    uint8           dir;     //方向
    uint8           cAddr;   //客户机地址
    SERVER_ADDR_T   sAddr;   //服务器地址
    
    /* 安全传输信息, 组织报文不用, 组织apdu可能用上 */
    union{
        struct
        {
            uint8   verifyType:3; //0-数据验证码 1-随机数 2-随机数+MAC 3-安全标识
            uint8   dataType  :1; //地址类型 0-明文 1-密文
            uint8   res       :2; //保留
            uint8   bProxy    :1; //1-代理 
            uint8   bSecurity :1; //1-安全传输
        };
        uint8       security; //安全传输使用
    };
    uint8           NoEsamMac;//不检验esam的mac的正确性
    uint8           res1[2];
    uint32          apduLen;   //长度扩展
    uint8          *pApdu;
    LINK_SEPARATE_T tSepara;   //链路分帧属性
}FRAME_HEAD_T;

typedef int (*DBUpdateFun)(int, uint8, uint16, uint32);

typedef enum AppErrCode
{
    APP698_SECCESS      = 0,
    APP698_ERR_BUSY,
    APP698_ERR_INPUT,
    APP698_ERR_NOT_OPEN,
    APP698_ERR_NOT_CONNECT,
    APP698_ERR_ID_NOTEXIST,
    APP698_ERR_MSG_HEAD_TAIL,
    APP698_ERR_HCS,
    APP698_ERR_FCS,
    APP698_ERR_CACHE_NOT_ENOUGH,
    APP698_ERR_MSG_TOO_LONG,
    APP698_ERR_IPC_RECONNECT,
    APP698_ERR_SEND_ERRORMSG,
    APP698_ERR_ADDR_NOTMATCH,
    APP698_ERR_NO_PROC_MESS,
    APP698_ERR_ADDR_BROADCAST,
    APP698_ERR_NOT_EXT_MSG,
    APP698_ERR_EXT_MSG_FIRST,
    APP698_ERR_EXT_MSG_OVER,
}APP_ERRORCODE_E;

/* 检查通道是否没有准备好 */
#define CHANNEL_CHECK_NOTOK(pchannel)  \
    (NULL == (pchannel) || FALSE == (pchannel)->bOpen || FALSE == (pchannel)->bConnect)

/* 获取发送接收对打允许帧长度 */
#define CHANNEL_GET_BREAK_FRAME_SIZE(pchannel)  \
    (0 == (pchannel)->tContext.sendFrameLen) ? LOCAL_SEND_FRAM_MINLEN : MIN((pchannel)->tContext.sendFrameLen, LOCAL_SEND_FRAM_MAXLEN)

/* 判断是否不允许发送 */
#define CHANNEL_NOT_NEED_SEND(pchannel) (0 == (pchannel)->permSend || 0 == (pchannel)->sendMessLen)

#define CAHNNEL_LOCK_RECV(pchannel, lockuser)   \
    {\
        pthread_mutex_lock(&((pchannel)->fRecv));\
        (pchannel)->sendLockUser = lockuser;\
    }

#define CAHNNEL_UNLOCK_RECV(pchannel)  \
    {\
        pthread_mutex_unlock(&((pchannel)->fRecv));\
        (pchannel)->sendLockUser = APP698_NO_LOCK;\
    }

/* 清空处理报文队列 */
#define CHANNEL_EMPTY_PROC_MESS(pchannel)   \
    do{\
        if(pchannel->procLen > 0)\
        {\
            memset(pchannel->ucMess, 0, APP_RECV_MSG_MAXLEN);\
            pchannel->procLen = 0;\
        }\
        pchannel->blocknum = 0;\
    }while(0);


#define CAHNNEL_LOCK_SEND(pchannel, lockuser)   \
    {\
        pthread_mutex_lock(&((pchannel)->fSend));\
        (pchannel)->sendLockUser = lockuser;\
    }

#define CAHNNEL_UNLOCK_SEND(pchannel)  \
    {\
        pthread_mutex_unlock(&((pchannel)->fSend));\
        (pchannel)->sendLockUser = APP698_NO_LOCK;\
    }

#define CHANNEL_EMPTY_SEND_MESS_NOLOCK(pchannel) \
    do{\
        memset(pchannel->sendMess, 0, APP_SEND_MSG_MAXLEN);\
        pchannel->sendMessLen = 0;\
        pchannel->curblock = 0;\
        pchannel->permSend = 0;\
        pchannel->protocol = 0;\
    }while(0);

/* 填写发送缓冲区 */
#define CHANNEL_PUT_SEND_MESS_NOLOCK(pchannel, m, l, p) \
    do{\
        memcpy(pchannel->sendMess, m, l);\
        pchannel->sendMessLen = l;\
        pchannel->curblock = 0;\
        pchannel->permSend = 1;\
        pchannel->protocol = p;\
    }while(0);

/* 设置发送缓冲区无锁 */
#define CHANNEL_SET_SEND_MESS_NOLOCK(pchannel, l, s, p) \
    do{\
        pchannel->sendMessLen = l;\
        pchannel->curblock = 0;\
        pchannel->permSend = s;\
        pchannel->protocol = p;\
    }while(0);

/* 获取动态心跳 */
#define CHANNEL_GET_HEART(pchannel)   \
    ((pchannel->tChannel.heart < CHANNEL_MIN_HEART) ? CHANNEL_MIN_HEART : pchannel->tChannel.heart)

/* 获取应用分帧大小 */
#define APDU_SPLIT_DEFAULT_MAX           app_apdu_default_len()

/* 设置帧头长度 */
#define APUD_COUNT_FRAME_LEN(alen, logica) (APP_698_HEAD_BASE_SIZE + alen + (logica <= 1 ? 0 : 1))

void app_get_initlock();
void app_set_localaddr(uint8 len, uint8 *pAddr);
uint8 app_get_localaddr(uint8 **ppAddr);
void app_set_clientNO(uint8 len, uint8 *pAddr);
uint8 app_get_clientNO(uint8 **ppAddr);
void app_set_groupaddr(OOP_GRPADDR_T *pAddr);
void app_get_groupaddr(OOP_GRPADDR_T *pAddr);
void app_set_password(OOP_VISIBLEVAR32_T *password);
uint8 app_get_password(uint8 **ppPwd);
BOOLEAN app_check_password(uint8 *pPwd, uint8 len);
void app_set_context(OOP_ASSOCIATION_CONTEXT_T *pContext);
void *app_get_context();
void app_set_version(Factory_Version_t *pVersion);
void *app_get_version();

void app_init_channel(uint32 ip, uint16 port);
void app_set_preconnect(uint8 id, uint8 linkState, uint16 heartbeat);
void app_open_channel(uint8 id, CHANNEL_INFO_T *pChannelInfo);
void app_set_channel(uint8 id, CHANNEL_INFO_T *pChannelInfo);
void app_close_channel(uint8 id);
void app_unlock_channel(uint8 id, uint8 lockuser);
int app_set_channelworklock(uint8 id, uint8 bwork);
void app_update_leftHB(uint8 id, uint16 heartbeat);
uint16 app_get_leftHB(uint8 id);
void app_set_connect(uint8 id, uint8 bConncet, APP_CONTEXT_t *pAppContext);
BOOLEAN app_get_connect(uint8 id, APP_CONTEXT_t *pAppContext);
CHANNEL_CACHE_T *app_get_channel(uint8 id);
int app_externmsg_reproc(uint8 id, uint8 *pMsg, uint16 msgLen, uint8 lockuser);
int app_insert_recvmsg(uint8 id, uint8 *pMsg, uint16 msgLen, uint8 lockuser);
uint16 app_get_recvmsg(uint8 id, uint8 *pbuf, uint16 bufLen, uint8 lockuser);
uint16 app_get_645recv(uint8 id, uint8 *pbuf, uint16 bufLen, uint8 lockuser);
uint16 app_get_recvmsg_s(uint8 id, uint8 *pbuf, uint16 bufLen, uint8 lockuser);
void app_remove_firstrecvmsg(uint8 id, uint8 lockuser);
BOOLEAN app_cut_message(uint8 *pMsg, uint16 msgLen, uint16 *offset, uint16 *newlen);
int app_check_message(uint8 *pMsg, uint32 msgLen);
int app_check_message_head(uint8 *pMsg, uint32 msgLen);
int app_get_frame(uint8 *pMsg, uint32 msgLen, FRAME_HEAD_T *pFrame, BOOLEAN bSC);
uint16 app_frame_to_package(FRAME_HEAD_T *pFrame, uint8 *pBuf, uint16 bufLen, BOOLEAN bSC);
int app_set_sendblock(CHANNEL_CACHE_T *ptChannel, uint16 blocknum);
BOOLEAN app_bluetooth_mac(uint8 id, uint8 *pMsg, uint16 *pLen);
int app_bluetooth_mac_reply(uint8 id);
int app_insert_procmesstail(CHANNEL_CACHE_T *ptChannel, uint8 *pBuf, uint16 bufLen, uint16 blockid);
void app_update_procmess_frame(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *pFrame);
uint16 app_linkrequest_package(uint8 type, uint16 beatPeriod, uint8 *pBuf, uint16 bufLen);
void app_connect_init(uint8 id);
void rpt_proxyresponse_data();
uint16 app_apdu_default_len();
uint8 app_get_opencha_num();
uint8 app_get_opencha_id(uint8 no);
void app_log_channel_recvbuf(uint8 id, uint8 lockuser);
void app_channel_proctime(uint8 id, uint8 bStart);
uint32 app_frame_to_bigpackage(FRAME_HEAD_T *pFrame, uint8 **ppBigMsg, uint32 tarMsgLen, uint32 *pApduLeft, 
                               BOOLEAN bSC);
uint32 app_frame_to_adaptpackage(FRAME_HEAD_T *pFrame, uint8 *pBuf, uint32 bufLen, BOOLEAN bSC);
void app_quitlogin_proc();


int32 ProxyWriteQueue(ProxyLockQueue *ProxyQueue, uint8 *buf, int32 buf_len);
extern int send_emergency_ipcmsg(uint8 id, uint8 protocol, uint8 *pMsg, uint16 msgLen);
extern int send_bigpackage_master(uint8 id, uint8 *pMsg, uint32 msgLen);
extern int protocol_698_addpool(uint8 channel);
extern void *app_apdubreak_init();
extern void app_apdubreak_free(void **pBreak);
extern void app_apdubreak_zero(void *pBreak);
extern int appmain_send_msg(uint32 msgid, uint16 label, uint8 *pMsg, uint16 msglen);
extern int appmain_send_syncmsg(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen);
extern int appmain_send_syncmsg_ccon(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen);
extern int appmain_send_syncmsg_time(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen, uint8 outtime);
extern int reboot_sent_to_smiOS(void);
extern int param_init_sent_to_acmeter(void);
extern int data_init_sent_to_acmeter(void);
extern int demand_init_sent_to_acmeter(void);
extern int system_cmd_sent_to_acmeter(const char* cmd);
extern int rate_set_sent_to_acmeter(uint8 time);
extern uint16 appmain_get_ppp_mode(uint8 no, uint8* recvbuf, uint16 buflen);
extern uint16 appmain_get_ppp_diag(uint8 no, uint8* recvbuf, uint16 buflen);
extern uint16 appmain_get_ppp_gps(uint8 no, uint8* recvbuf, uint16 buflen);
extern uint16 appmain_trans_AT(uint8 no, char *sendAT, uint8* recvbuf, uint16 buflen);
void appmain_set_gps_tempposition(OOP_GEOGADD_A_T *ptGps);
int appmain_get_gps_tempposition(OOP_GEOGADD_A_T *ptGps);
extern uint16 appmain_get_map_slot(const char *type, uint8* recvbuf, uint16 buflen);
extern uint16 appmain_get_map_info(uint32 no, uint8* recvbuf, uint16 buflen);
extern uint16 appmain_get_map_all(uint8* recvbuf, uint16 buflen);
extern uint16 appmain_get_deviceinfo(OOP_DEVICERUNSTAT_T *deviceinfo);
extern int appmain_manager_app(uint8 type, char *appname);
extern int appmain_act_ble_checkmeter(OOPM_BLUETOOTHERRCHK_T *check);
extern int appmain_get_bluetooth_cfg(uint8* recvbuf, uint16 buflen);
extern int appmain_get_bluetooth_sn(uint8* recvbuf, uint16 buflen);
extern int appmain_get_bluetooth_connect(uint8* recvbuf, uint16 buflen);
extern int appmain_set_bluetooth_cfg(uint8 *sendbuf, uint16 sendlen, uint8* recvbuf, uint16 buflen, uint16 *recvlen);
extern int appmain_set_bluetooth_sn(uint8 *sendbuf, uint16 sendlen, uint8* recvbuf, uint16 buflen, uint16 *recvlen);
extern int appmain_set_bluetooth_check(uint8 *sendbuf, uint16 sendlen);
extern int appmain_set_flow_param(uint32 monLimit, uint32 flowLimit, uint32 time);
extern int appmain_get_flow_param(uint32 *monLimit, uint32 *flowLimit, uint32 *time);
extern int appmain_send_proxy_to_taskManager(uint8 *sendbuf, uint16 sendlen, uint8* recvbuf, uint16 buflen, 
    uint16 *recvlen);
extern int ipconfig_send_to_smiOS(char* Name, uint8 type, uint8* IP, uint8* NetMask, uint8* NetGw);
extern int seek_meter_to_taskManager(uint16 time);
extern int trans_buf_to_taskManager(uint8* buf, uint16 len, uint32 port, uint8 baud, uint32 overtime, uint8 *recvbuf, uint32 recLenMax, uint16 *recLen);
extern int trans_buf_send_to_taskManager(uint16 label, uint8* buf, uint16 len, uint32 port, uint8 baud, uint32 overtime);
extern int appmain_refresh_status_bar(uint32 time, char *str);
extern	void ProxyRequest_ClientApdu(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, uint8 *pBreakFlag);
extern	int Proc_Proxy(uint16* label,ProxyHeadInfo_t *ProxyHead );

void app_comm_proxy_set(BOOLEAN isUse);
BOOLEAN app_comm_proxy_isuse();
void app_secure_defend_set(BOOLEAN isUse);
BOOLEAN app_secure_defend_isuse();
void proxy_apply_ack_set(uint8 id);
BOOLEAN proxy_apply_ack_get(uint8 id);
void proxy_apply_retry_sub(uint8 id);
uint8 proxy_apply_retry_get(uint8 id);
void proxy_coninfo_notify(uint8 id);

int32 ProxyReadQueue(ProxyLockQueue *ProxyQueue, uint8 *buf);
BOOLEAN ProxyCheckQueue(ProxyLockQueue *ProxyQueue);
int8 UnPackRouterFrameData(uint16 FrameLen, uint8 *FrameBuff, AppData_T *AppData);

extern uint16 channel_get_break_frame_size(CHANNEL_CACHE_T  *ptChannel);

#ifdef __cplusplus
}
#endif

#endif


