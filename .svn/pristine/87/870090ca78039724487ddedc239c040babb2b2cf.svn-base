/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程SSAL公共部分
* @date：    2021-8-31
* @history： 
*********************************************************************
*/
#ifndef	__SSAL_H
#define __SSAL_H

#ifdef __cplusplus
        extern "C" {
#endif

typedef enum e_ssal_type
{
    SSAL_APPLY_DATA    = 0,    //应用数据报文          主站与终端之间需要进行加密传输的业务报文
    SSAL_LINK_MANAGER  = 1,    //链路管理报文	       终端的链路报文，包括：登录报文、心跳报文等
    SSAL_GET_TERINAL   = 2,    //获取终端基本信息报文  网关获取终端基本信息的报文，包括请求报文和响应报文
    SSAL_SESSION_CONSE = 3,    //终端会话协商报文      网关与终端之间进行身份认证和会话密钥协商的报文，包括请求报文和响应报文
    SSAL_SET_TIMEOUT   = 4,    //更改时效门限          更改终端会话密钥时效门限的报文
    SSAL_UPDATE_SECRET = 5,    //终端链路密钥更新      更新终端保存的用于协商的链路密钥的报文
    SSAL_GET_CERTI     = 6,    //终端网关证书更新      更新终端保存的网关证书的报文
    SSAL_GET_GWLINK    = 7,    //查询网关链接状态      查询网关与所有客户机链接状态的报文
    SSAL_GET_ALLLINK   = 8,    //查询网关与所有客户机链接状态的报文 
                               //                      安全接入区设备需要进行密钥协商时的协商触发报文，由安全接入区设备发出，触发网关进行会话密钥协商
    SSAL_LINK_HB       = 9,    //链路心跳报文          在链路中，作为客户端设备需向服务端设备发送该报文以进行链路维护，服务端需按报文格式响应
                               //                      该报文以明文方式传输
    SSAL_SESSION_APPLY = 10,   //终端会话申请          终端向网关申请进行会话密钥协商，主要供互联网移动终端使用
    SSAL_SESSION_CONF  = 11,   //终端会话确认          会话协商完成确认报文，主要供互联网移动终端使用
    SSAL_GW_QUERY      = 12,   //网关查询指令          附录c里面的查询指令，按照SSAL协议封装作为数据域。
                               //                      该报文与其他报文不同，不会穿透网关，网关收到该报文后，发送回复信息

    SSAL_T_NULL        = 13,   //无效
}SSAL_TYPE_E;

typedef enum e_ssal_citype
{
    SSAL_CIPHER_CLE     = 0,    //明文
    SSAL_CIPHER_CBC     = 1,    //CBC密文
    SSAL_CIPHER_CLE_MAC = 2,    //明文+MAC
    SSAL_CIPHER_ECB     = 3,    //ECB密文
    SSAL_CIPHER_CBC_MAC = 4,    //CBC密文+MAC
    SSAL_CIPHER_EBC_MAC = 5,    //ECB密文+MAC
}SSAL_CIPHER_E;

typedef enum e_ssal_dev
{
    SSAL_DEV_COMM_END   = 0,    //采集前置/通信后置
    SSAL_DEV_GATHER     = 1,    //采集设备
    SSAL_DEV_PHONE      = 2,    //现场服务终端（掌机）
    SSAL_DEV_OFFICE     = 3,    //移动办公设备(营业办公设备)
    SSAL_DEV_COMM_FRONT = 4,    //通信前置
    SSAL_DEV_AAA        = 5,    //3A认证服务器
    SSAL_DEV_TEST       = 6,    //在线性能检测服务器
    SSAL_DEV_MAINTAIN   = 7,    //接入区运维专用设备
    SSAL_DEV_CHECK_8    = 8,    //运检专业设备
    SSAL_DEV_CHECK_9    = 9,    //运检专业设备
    SSAL_DEV_CHECK_10   = 10,   //运检专业设备
    SSAL_DEV_CHECK_11   = 11,   //运检专业设备
    SSAL_DEV_CHECK_12   = 12,   //运检专业设备
    SSAL_DEV_CHECK_13   = 13,   //运检专业设备
    SSAL_DEV_CHECK_14   = 14,   //运检专业设备
    SSAL_DEV_CHECK_15   = 15,   //运检专业设备
    SSAL_DEV_COMM_16    = 16,   //信通专业设备（移动终端等）
    SSAL_DEV_COMM_17    = 17,   //信通专业设备（移动终端等）
    SSAL_DEV_COMM_18    = 18,   //信通专业设备（移动终端等）
    SSAL_DEV_COMM_19    = 19,   //信通专业设备（移动终端等）
    SSAL_DEV_COMM_20    = 20,   //信通专业设备（移动终端等）
    SSAL_DEV_COMM_21    = 21,   //信通专业设备（移动终端等）
    SSAL_DEV_COMM_22    = 22,   //信通专业设备（移动终端等）
    SSAL_DEV_COMM_23    = 23,   //信通专业设备（移动终端等）
    SSAL_DEV_APP        = 24,   //网上国网APP终端
    SSAL_DEV_SEPAR_SSAL = 25,   //隔离装置（SSAL型）
    SSAL_DEV_GWTWAY     = 26,   //信息网络安全接入网关（I型）
}SSAL_DEV_E;

typedef struct tagSSALC
{
    uint8     res     :7;    //保留
    uint8     GAflag  :1;    //网关地址GA 标记
    uint8     TPflag  :1;    //时间标签TP 标记
    uint8     CIflag  :1;    //通信信息CI 标记
    uint8     TAflag  :1;    //目的地址TA 标记
    uint8     SAflag  :1;    //源地址SA 标记
    uint8     DAflag  :1;    //设备地址DA 标记
    uint8     DATflag :1;    //设备地址类型DAT 标记
    uint8     SVflag  :1;    //SSAL协议版本SV 标记
    uint8     FCflag  :1;    //SSAL协议版本SV 标记
}SSAL_C_T;

//SSAL协议版本SV
typedef struct tagSSALFC
{
    uint8     dir;           //0-主站发出的下行帧; 1-终端发出的上行帧
    uint8     prm;           //0-被动应答; 1-主动发出
    uint8     consult;       //触发协商标志: 0-主站下行传输帧永远不会触发网关启动与终端的会话密钥协商;
                             //              1-主站下行的传输帧可能触发网关与目标终端的会话密钥协商，网关需要根据终端情况进行具体判断是否启动会话密钥协商
                             //主要协商方式：网关发起的会话秘钥协商请求后，终端发出密钥协商响应，进行会话秘钥协商，终端需记录是否完成与网关的协商；
                             //辅助协商方式：如果终端记录与网关协商成功则不启用辅助协商方式，如果终端记录与网关协商不成功，
                             //              则心跳报文触发协商标志位置置1，尝试进行第二次协商
    uint8     ssaltype;      //报文类型      SSAL_TYPE_E
}SSAL_FC_T;

//SSAL协议版本SV
typedef struct tagSSALVersion
{
    uint8     version;       //版本号
    uint8     cipherType;    //加密算法 SSAL_CIPHER_E
}SSAL_VERSION_T;

//设备地址类型DAT
typedef struct tagSSALdevAddrType
{
    uint8     devType;      //设备类型 SSAL_DEV_E
    uint8     addrType;     //地址类型 0-单地址;1-组地址;2-广播地址
    uint8     proType;      //协议类型 1-16规约（698面向对象）; 2-13规约（1376.1）; 3-移动互联网APP
    uint8     res[1];
}SSAL_DAT_T;

typedef struct tagSSALdevAddr
{
    uint8 mslen;            //主站地址长度
    uint8 termlen;          //行政地址+逻辑地址长度
	uint8 MP[7];            //主站地址
	uint8 DP[16];           //设备地址
}SSAL_DA_T;

typedef struct tagSSALSA
{
    uint8 sourcelen;        //源地址长度
    uint8 portlen;          //端口长度
	uint8 SA[16];           //源地址
	uint8 PORT[2];
}SSAL_SA_T;

typedef struct tagSSALTA
{
    uint8 destlen;          //目的地址长度
    uint8 portlen;          //端口长度
    uint8 TA[16];           //目的地址
    uint8 PORT[2];
}SSAL_TA_T;

typedef struct tagSSALCI
{
    uint8 type;             //通信信道类型
    uint8 len;              //信道信息长度
    uint8 CIC[15];          // 通道信息
}SSAL_CI_T;

typedef struct tagSSALTP
{
	uint8 yyH;
	uint8 yyL;
	uint8 mon;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;
}SSAL_TP_T;

typedef struct tagSSALGA
{
	uint8          GAL;      // 网关长度
	uint8          GAC[31];  // 网关信息
}SSAL_GA_T;


/* SSAL报文解析帧头 */
typedef struct tagSSALHead
{
    uint16         seq;      //帧序号SEQ
    
    SSAL_C_T       c;        //控制码
    SSAL_FC_T      fc;       //功能码FC
    SSAL_VERSION_T sv;       //SSAL协议版本SV
    SSAL_DAT_T     dat;      //设备地址类型DAT
    SSAL_DA_T      da;       //设备地址DA
    SSAL_SA_T      sa;       //源地址SA
    SSAL_TA_T      ta;       //目的地址TA 
    SSAL_CI_T      ci;       //通信信息CI
    SSAL_TP_T      tp;       //时间标签
    SSAL_GA_T      ga;       //网关地址

    uint16         datalen;  //数据长度
    uint16         retcode;  //返回码 应答有
    uint8         *data;     //数据
}SSAL_HEAD_T;

//终端返回错误码
//安全认证错误（Base：0x1000）
#define SAL_RECVINFO_ERROR_TERMINAL_DECIPHERING 0x1001       //终端解密错误
#define SAL_RECVINFO_ERROR_TERMINAL_SIGNCHECK   0x1002       //终端验签失败
#define SAL_RECVINFO_ERROR_TERMINAL_MAC         0x1003       //终端MAC校验失败
#define SAL_RECVINFO_ERROR_TERMINAL_SN          0x1004       //会话计数器错误
#define SAL_RECVINFO_ERROR_GW_DECIPHERING       0x1005       //网关解密错误
#define SAL_RECVINFO_ERROR_GW_SIGNCHECK         0x1006       //网关验签失败
#define SAL_RECVINFO_ERROR_GW_MAC               0x1007       //网关MAC校验失败
#define SAL_RECVINFO_ERROR_GW_PASSWD            0x1008       //网关密码单元故障
#define SAL_RECVINFO_ERROR_LINK_PASSWD          0x1009       //链路设备密码单元故障
//帧校验错误(Base：0x2000)
#define SAL_RECVINFO_ERROR_DATA					0x2000       //帧解析错误(终端维护)
#define SAL_RECVINFO_ERROR_SAL_SV               0x2001       //协议版本错误
#define SAL_RECVINFO_ERROR_DES_MISMATCHING      0x2002       //加解密算法标志不匹配
#define SAL_RECVINFO_ERROR_DEVICE_TYPE          0x2003       //设备类型无法识别
#define SAL_RECVINFO_ERROR_CONTROL              0x2004       //控制域无法识别
#define SAL_RECVINFO_ERROR_DIR                  0x2005       //传输方向位错误
#define SAL_RECVINFO_ERROR_APDU_LEN_ABMORMAL    0x2006       //数据域长度异常（小于4字节）
#define SAL_RECVINFO_ERROR_APDU_LEN             0x2007       //数据域长度不匹配
//链路错误(Base：0x3000)
#define SAL_RECVINFO_ERROR_NO_TARGET_NDOE       0x3001       //目标节点不存在
#define SAL_RECVINFO_ERROR_NO_SESSION_LINK      0x3002       //当前会话链路未建立
#define SAL_RECVINFO_ERROR_SEND_INFO            0x3003       //报文发送失败
#define SAL_RECVINFO_ERROR_CI                   0x3004       //信道错误
#define SAL_RECVINFO_ERROR_SESSION_NEGOTIATION  0x3005       //当前链路会话协商失败

typedef enum e_ssal_check
{
    SSAL_CHECK_OK       = 0,
    SSAL_CHECK_ERROR    = 1,
    SSAL_CHECK_DROP     = 2,
}SSAL_CHECK_E;


typedef enum e_ssal_err
{
    SSAL_SUCCESS        = 0,
    SSAL_ERR_INPUT      = 1,
    SSAL_ERR_MSG        = 2,
    SSAL_ERR_CS         = 3,
    SSAL_ERR_UNKNOW     = 4,
    SSAL_ERR_UNSUPPORT  = 5,

    SSAL_ERR_OTHER      = 255,
}SSAL_COMM_E;

void ssal_set_status(uint8 status);
uint8 ssal_get_status();
int ssal_msg_split(uint8 *pMsg, uint16 msglen, SSAL_HEAD_T *pSsalHead);
uint8 *ssal_msg_package(SSAL_HEAD_T *pSsalHead, uint16 *pMsglen);
int ssal_find_first_msg(uint8 id, SSAL_HEAD_T *pSsalHead, uint16 *pStart, uint16 *pLen, uint8 lockuser);
void ssal_remove_recvmsg(uint8 id, uint16 start, uint16 msglen, uint8 lockuser);
int ssal_check_support(SSAL_HEAD_T *pSsalHead, uint16 *pRetcode);
void ssal_recv_to_send_head(SSAL_HEAD_T *pSsalHead, SSAL_HEAD_T *pSsalSend);
uint16 ssal_linkrequest_package(uint32 ip, uint8 *link698, uint16 len698, uint8 *ssallink, uint16 linkmax);
uint16 ssal_commdata_package(uint32 ip, uint8 *frame698, uint16 len698, uint8 *ssalframe, uint16 framemax);

#ifdef __cplusplus
}
#endif

#endif


