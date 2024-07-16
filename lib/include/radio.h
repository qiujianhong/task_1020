/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief：   电台消息接口
* @date：    2021/7/9
* @history： 
*********************************************************************
*/
#ifndef __RADIO_H_
#define __RADIO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define RADIO_MSG_MAX_LEN    4096

/* 电台参数 */
typedef struct tagRADIOConfig
{
    uint8            channel;                    /* 频道 */
    uint8            power;                      /* 功率 */
    uint8            noise;                      /* 静噪级别 */
    #if 0
    uint8            mode;                       /* 调整方式 */
    uint8            offVaule;                   /* 频率偏值 */
    #endif
    OOP_OCTETVAR16_T areaNo;                     /* 行政区码 */
    OOP_OCTETVAR16_T devAddr;                    /* 设备地址 */
    OOP_OCTETVAR16_T addr;                       /* 终端地址 */
}RADIO_CONFIG_T;

/* 报文通知 */
typedef struct tagRADIOMsgEvent
{
    uint16   len;                     /* 报文长度 */
    uint8    msg[RADIO_MSG_MAX_LEN];  /* 报文内容 */
}RADIO_MSG_EVENT_T;

/* 发送数据 */
typedef struct tagRADIOMSG
{
    uint32   len;                        /* 报文长度 */
    uint8    msg[RADIO_MSG_MAX_LEN];     /* 报文内容 */
}RADIO_MSG_T;

/**********************************************************************
* @name      : radio_config_split
* @brief     ：参数内容解析
* @param[in] ：uint8 *pMsg                  消息内容
               uint16 msglen               消息长度
* @param[out]：RADIO_CONFIG_T *pConfig      参数
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int radio_config_split(uint8 *pMsg, uint16 msglen, RADIO_CONFIG_T *pConfig);

/**********************************************************************
* @name      : radio_config_package
* @brief     ：发送数据内容封装
* @param[in] ：BLE_MSG_T *ptMsg           数据内容
               uint16 buflen              缓存长度
* @param[out]：uint8 *pBuf                消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
uint16 radio_config_package(RADIO_CONFIG_T *pConfig, uint8 *pBuf, uint16 buflen);

/**********************************************************************
* @name      : radio_msg_event_split
* @brief     ：报文通知消息内容解析
* @param[in] ：uint8 *pMsg               消息内容
               uint16 msglen             消息长度
* @param[out]：RADIO_MSG_EVENT_T *pMsgOut  解析结果
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int radio_msg_event_split(uint8 *pMsg, uint16 msglen, RADIO_MSG_EVENT_T *pMsgOut);

/**********************************************************************
* @name      : radio_msg_package
* @brief     ：发送数据内容封装
* @param[in] ：BLE_MSG_T *ptMsg           数据内容
               uint16 buflen              缓存长度
* @param[out]：uint8 *pBuf                消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
uint16 radio_msg_package(RADIO_MSG_T *ptMsg, uint8 *pBuf, uint16 buflen);

#ifdef __cplusplus
}
#endif

#endif /* __RADIO_H__ */
