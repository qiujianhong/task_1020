/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：UDP消息相关访问接口
* @date： 2019.10.23
*********************************************************************
*/
#ifndef __MSGUDP_H__
#define __MSGUDP_H__

#include "../../include/baseCommon.h"
#include "../../include/message.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DEBUG_UDP
#ifdef DEBUG_UDP
  #include <errno.h>
  extern int errno;
  #define UPRINT(x...) ({ if (getenv("DEBUG_UDP") != NULL)  printf(x);})
#else
  #define UPRINT(x...)
#endif

/*
 ********************************************************************
 *                                                                    *
 * 1-UDP消息相关数据类型定义                                          *
 *                                                                    *
 ********************************************************************
*/

/* 句柄命名 */
#define HMSG void*

/*创建UDP消息句柄的相关参数*/
typedef struct _PARA_UDP_T
{
    char*         appName; //APP名称
    uint32        srcIP;   //本地IP地址
    uint16        srcPort; //本地端口号
    uint8         bBlock;  //0：非阻塞 1：阻塞
    uint8         res[1];  //为了对齐
}PRAR_UDP_T;

/* 考虑字节对齐, 没有严格按照报文格式, 收到的消息解析成如下格式 */
typedef struct tagMsgInfo
{
    uint32 IP;        //目的端ip
    uint16 port;
    uint8  res[2];
    
    uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128];
    char   DestAddr[128];

    uint16 IID;
    uint16 IOP;

    uint32 MsgLen; 
    uint8  MsgData[0];
}MSG_INFO_T;

/*句柄所指缓存*/
typedef struct _HANDLE_UDP_T
{
    char               appName[100];     //app名称
    struct sockaddr_in srcAddr;
    int                msgSocket;
}HANDLE_UDP_T;

//
// UDP消息相关接口
//

/**********************************************************************
* @name      : msg_udp_package
* @brief     ：指定格式消息组装成
* @param[in] ：MSG_INFO_T *pMsgInfo  结构消息
               char *sourAddr        源端地址
* @param[out]：uint8 **ppBuf         组装消息
* @return    ：消息长度
* @Create    : 
* @Date      ：2019-10-29
* @Update    :
**********************************************************************/
uint16 msg_udp_package(MSG_INFO_T *pMsgInfo, char *sourAddr, uint8 **ppBuf);

/**********************************************************************
* @name      : msg_udp_split
* @brief     ：接受的消息解析成指定格式
* @param[in] ：uint8 *pBuf           接受到的报文
               uint16 bufLen         报文长度
* @param[out]：MSG_INFO_T **ppMsgOut 解析出来的消息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-10-29
* @Update    :
**********************************************************************/
int msg_udp_split(uint8 *pBuf, uint16 bufLen,uint32 ipAddr, uint16 port, MSG_INFO_T **ppMsgOut);

/**
*********************************************************************
* @brief：     创建UDP操作相关句柄
* @param[in]： paraUdp - 相关参数
* @param[out]：          
* @return：    !NULL   - 成功创建句柄
               NULL    - 错误
*********************************************************************
*/
HMSG msg_handle_create(PRAR_UDP_T paraUdp);

/**
*********************************************************************
* @brief：     接收消息
* @param[in]： hUdp     - 句柄
* @param[out]：ppMsgOut - 读出来的消息指针，用完一定要free          
* @return：    = 0      - 成功
               < 0      - 错误
*********************************************************************
*/
int msg_udp_read(HMSG handleUdp, MSG_INFO_T **ppMsgOut);

/**
*********************************************************************
* @brief：     发送消息
* @param[in]： handleUdp - 句柄
               pMsgInfo  - 要发送的消息 
* @param[out]：          
* @return：    = 0       - 成功
               < 0       - 错误
*********************************************************************
*/
int msg_udp_write(HMSG handleUdp, MSG_INFO_T* pMsgInfo);

/**
*********************************************************************
* @brief：     销毁UDP操作相关句柄
* @param[in]： handleUdp - 句柄
* @param[out]：          
* @return：    = 0       - 成功
               < 0       - 错误
*********************************************************************
*/
int msg_handle_close(HMSG handleUdp);

/**
*********************************************************************
* @brief：     接收消息解析成 MESSAGE_INFO_T
* @param[in]： uint8 *pBuf           接受到的报文
               uint16 bufLen         报文长度
* @param[out]：MSG_INFO_T **ppMsgOut 解析出来的消息
* @return    ：0-成功/错误码
*********************************************************************
*/
int msgnew_udp_split(uint8 *pBuf, uint16 bufLen, MESSAGE_INFO_T **ppMsgOut);
uint16 msgnew_udp_package(MESSAGE_INFO_T *pMsgInfo, uint8 **ppBuf);

#ifdef __cplusplus
}
#endif
#endif //__MSGUDP_H__
