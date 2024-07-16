
/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#ifndef __AC_METER_REPORT_H__
#define __AC_METER_REPORT_H__

#include "ac_redef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif


/*******************************************************************************
* 全局变量
*******************************************************************************/

/*******************************************************************************
* 枚举和结构体
*******************************************************************************/

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

//服务层数据结构定义
#define MAX_APDU_LENGTH 4096
enum{
	SERVERID_LINKREQUEST = 1,
	SERVERID_CONNECTREQUEST,
	SERVERID_RELEASEREQUEST,
	SERVERID_GETREQUEST=5,
	SERVERID_SETREQUEST,
	SERVERID_ACTIONREQUEST,
	SERVERID_REPORTRESPONSE,
	SERVERID_PROXYREQUEST,
	SERVERID_SECURITYREQUEST=16,

	SERVERID_LINKRESPONSE = 129,
	SERVERID_CONNECTRESPONSE,
	SERVERID_RELEASERESPONSE,
	SERVERID_RELEASENOTIFICATION,
	SERVERID_GETRESPONSE,
	SERVERID_SETRESPONSE,
	SERVERID_ACTIONRESPONSE,
	SERVERID_REPORTREQUEST,
	SERVERID_PROXYRESPONSE,
	SERVERID_SECURITYRESPONSE=144,
};

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
	
#define APP_RECV_MSG_MAXLEN     16388   //最大考虑首尾2个字节 

#define SERVER_ADDR_MAXLEN         16

typedef struct tagServerCLIENTAddr
{
    uint8   len;       //服务端地址长度
    uint8   logicAddr; //逻辑地址扩展 0-255
    uint8   type;
    uint8   res[1];
    uint8   addr[SERVER_ADDR_MAXLEN];
}SERVER_ADDR_T;

typedef struct tagLinkSeparate
{
    uint16     block;  //序号
    uint8      type;   //分帧类型
    uint8      res;
}LINK_SEPARATE_T;

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

#define APUD_COUNT_FRAME_LEN(alen, logica) (APP_698_HEAD_BASE_SIZE + alen + (logica <= 1 ? 0 : 1))

/*******************************************************************************
* 函数声明
*******************************************************************************/

extern uint16 udp_apdu_analyse(uint8 *pApdu, uint16 nApduLen, uint8 *pOutApdu, uint16 msgmaxlen);
extern uint16 app_frame_to_package(FRAME_HEAD_T *pFrame, uint8 *pBuf, uint16 bufLen, BOOLEAN bSC);
extern int app_check_message(uint8 *pMsg, uint32 msgLen);
extern int app_get_frame(uint8 *pMsg, uint32 msgLen, FRAME_HEAD_T *pFrame, BOOLEAN bSC);
extern int app_check_rpt_message(uint8 *pMsg, uint32 msgLen);

extern OOP_DAR_E get_convert_by_oad(uint8 *pDataBuffer, uint16 nDataLen, OAD_INFO_T *pOadInfo, RESULT_DATA_T *outData, uint16 outDataMax, BOOL bHasOad);

#ifdef __cplusplus
}
#endif

#endif // __AC_METER_REPORT_H__

