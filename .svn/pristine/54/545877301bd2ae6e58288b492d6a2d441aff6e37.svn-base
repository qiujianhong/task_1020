/*
*********************************************************************
* @file    oopSecurity.h
* @brief： apdu安全传输相关
* @author：贺宁
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#ifndef __OOP_SECURITY_H__
#define __OOP_SECURITY_H__

// 长报文加解密
#define LONG_ENCRYPTION_EN 1

// 最大可处理的APDU长度
#define APDU_BUFF_SIZE_MAX		4096

/**********************************************************************
* @name      : security_mod_get
* @brief     ：根据对象OI获取默认的安全模式-终端
* @param[in] ：OI:        对象OI
* @param[out]：
* @return    ：SecModWd_t 安全模式
* @Create    : 
* @Date      ：2019-12-31
* @Update    :
**********************************************************************/
SecModWd_t security_mod_get(CHANNEL_CACHE_T *ptChannel, uint16 OI);

/**********************************************************************
* @name      : securityrequest_getapdu
* @brief     ：安全传输 解析出APDU内容 并逆向组装成报文
* @param[in] ：FRAME_HEAD_T *ptFrame        解析帧头
               uint16 outsize               返回消息最大长度
               APP_CONTEXT_t *pConnect      应用连接属性
* @param[out]：uint8 *pOutMsg               返回消息
               uint8 *pSecurity             安全传输标记
* @return    ：消息长度
* @Create    : 
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
uint32 protocol_securityrequest(FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint32 outsize, APP_CONTEXT_t *pContext, 
                                uint8 *pSecurity);

/**********************************************************
* 函数名称：
* 功能描述：
    生成一个安全APDU

* 输入描述：
* 输出描述:

* 返回值：
**********************************************************/
uint16 GenSecurityApdu(uint8 *apduData, uint16 apduDataLen,SecurityMode_t SecMod,uint8 SecLev);

/**********************************************************
* 函数名称：
* 功能描述：
    生成一个安全APDU

* 输入描述：
* 输出描述:

* 返回值：
**********************************************************/
uint32 GenSecurityApdu_long(uint8 *apduData, uint32 apduDataLen,SecurityMode_t SecMod,uint8 SecLev);

/**********************************************************
* 函数名称：
* 功能描述：
    生成一个上报安全APDU

* 输入描述：
* 输出描述:

* 返回值：
**********************************************************/
uint16 GenReportSecurityApdu(uint8 *apduData, uint16 apduDataLen);

/**********************************************************************
* @name      : esam_state_guishow
* @brief     ：esam密钥下发状态
* @param[in] ：esamState 内部协议     
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int esam_state_guishow(uint8 esamState);

#endif