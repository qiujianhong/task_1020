/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#ifndef _AC_698MSG_H
#define _AC_698MSG_H

//状态机
#define ACMSG_INIT        0         // 运行状态 - 初始化
#define ACMSG_RECV        1         // 运行状态 - 数据接收
#define ACMSG_SEND        2         // 运行状态 - I数据发送
#define ACMSG_CLOSE       3         // 运行状态 - 关闭连接

typedef struct tagCONNECT_REQUEST_T
{
    uint16 version;      //协议版本号
    uint8  prtlConf[8];  //协议一致性
    uint8  funcConf[16]; //功能一致性
    uint16 clientSFML;   //客户机发送帧长度
    uint16 clientRFML;   //客户机接收帧长度
    uint8  clientWS;     //客户机接收窗口大小
    uint16 clientPFML;   //客户机最大可处理帧尺寸
    uint32 timeOut;      //应用连接超时时间
    uint8  *pMechains;   //应用连接请求认证的机制信息
}CONNECT_REQUEST_T;


typedef enum AppConnectResult
{
    CONNECT_PERMISSION = 0, //允许建立应用连接
    CONNECT_ERR_PASSWORD = 1, //密码错误
    CONNECT_ERR_SYMMETRY = 2, //对称解密错误
    CONNECT_ERR_NSYMMETRY = 3, //非对称解密错误
    CONNECT_ERR_SIGNATURE = 4, //数字签名错误
    CONNECT_ERR_VERSION = 5, //协议版本不匹配
    CONNECT_ERR_OTHER = 255, //其他错误
}CONNECT_RESULT_E;

typedef struct {
    uint16 version;                //协议版本号
    uint8  prtlConf[8];            //协议一致性
    uint8  funcConf[16];           //功能一致性
    uint16 serverSFML;             //服务器发送帧最大尺寸
    uint16 serverRFML;             //服务器接收帧长度
    uint8  serverWS;               //服务器接收窗口大小
    uint16 serverPFML;             //服务器最大可处理帧尺寸
    uint32 timeOut;                //应用连接超时时间
    CONNECT_RESULT_E  connectRst;  //应用连接请求的认证响应信息
}CONNECT_RESPONSE_T;

extern OOP_DAR_E app698msg_time_set(void);
extern int ac_get_msg_package(OOP_OAD_U oad, uint8* sendbuf, uint32 bufLenMax, uint8 logic);
extern int ac_act_msg_package(OOP_OMD_U omd, uint8* inData, uint32 inLen, uint8* sendbuf, uint32 bufLenMax, uint8 logic);
extern int ac_connect_package(CONNECT_REQUEST_T* pResponse, uint8* sendbuf, uint32 bufLenMax);
extern int ac_act_frame_factory_reset(uint8* sendbuf, uint32 maxLen);
extern int ac_act_frame_power_off(uint8* sendbuf, uint32 maxLen);
extern int ac_act_parser_factory_reset(uint8* recvBuf, uint32 recvLen);
extern int ac_rpt_msg_package(uint8 *addr, uint8 isRec, uint8 piid, OOP_OAD_U oad, uint8 *pSend, uint32 lenMax);

#endif