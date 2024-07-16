/*
*********************************************************************
* @filename: security_msg.h
* @brief   : 安全APP消息交互通信
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2020-8-4      create
*********************************************************************
*/
#ifndef    __SECURITY_MSG_H
#define __SECURITY_MSG_H

#ifdef __cplusplus
    extern "C" {
#endif

typedef int  SECURITY_H;

#define SECURITY_FILEPATH_LEN   256
#define SECURITY_MSG_MAX_LEN    8192

#define SECURITY_MSG_PATH       "/tmp/dev/"
#define SECURITY_APP_DEST       "/tmp/dev/smiSecurity"

/* 消息结构 */
typedef struct tagSecurityOut
{
    uint8  CLA;       //命令类型
    uint8  INS;       //同步序号
    uint8  P1;        //参考符号
    uint8  P2;        //参考符号

    uint16 len;        //长度
    uint8  res[2];

    uint8 *data;       //消息内容
}SECURITY_SEND_T;

typedef struct tagSecurityRecv
{
    uint8  SW1;       //状态字
    uint8  SW2;       //状态字
    uint16 len;       //长度

    uint8  data[0];   //消息内容
}SECURITY_RECV_T;

typedef enum
{
    SECURITY_SUCCESS     = 0,
    SECURITY_ERR_INPUT   = 0x1100,
    SECURITY_ERR_MALLOC,
    SECURITY_ERR_TOO_LONG,
    SECURITY_ERR_SEND_FAILED,
    SECURITY_ERR_RECV_FAILED,
    SECURITY_ERR_BIND,
    SECURITY_ERR_SOCKET,
    SECURITY_ERR_OUT_TIME,
    SECURITY_ERR_DIR,
}security_errcode_e;

/**
*********************************************************************
* @brief：     初始化APP安全交互 一个APP只能存在一个 
* @param[in]： char *appname   APP名称
* @param[out]：
* @return：    错误码 security_errcode_e
*********************************************************************
*/
int security_app_init(char *appname);

/**
*********************************************************************
* @brief：     安全客户端初始化 
* @param[in]： uint32 clientid   客户端id 可以使用相同的dbclientid
* @param[out]：
* @return：    SECURITY_H        小于等于0无效
*********************************************************************
*/
SECURITY_H security_client_init(uint32 clientid);

/**
*********************************************************************
* @brief：     安全加固APP消息发收 
* @param[in]： SECURITY_H handle        客户端句柄
               SECURITY_SEND_T *pOut    发送消息
               uint32 timeout           超时时间 单位ms
* @param[out]：SECURITY_RECV_T **ppRecv 接收消息 使用完成后自己释放
* @return：    错误码 security_errcode_e
*********************************************************************
*/
int security_send_recv_msg(SECURITY_H handle, SECURITY_SEND_T *pOut, SECURITY_RECV_T **ppRecv, uint32 timeout);

#ifdef __cplusplus
}
#endif

#endif

