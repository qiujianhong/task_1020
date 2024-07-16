/*
*********************************************************************
* @filename: label_id.h
* @brief   : 通信消息label定义
* @describe: 
* @history: 
*    auther      date          describe
*    梅安宁      2020-03-05    create
*********************************************************************
*/
#ifndef    __LABEL_ID_H
#define __LABEL_ID_H

#define LABEL_START    0x3000

/* 停上电事件透传 */
#define LABEL_HALT_EVENT                (LABEL_START + 0x106)

//stamr扩展模处理时，(label & 0x0f00)为通道端口
/* 同步透传接口 */
#define LABEL_HALT_TRANS                (LABEL_START + 0x007)

/* 异常步透传接口        遥信   */
#define LABEL_HALT_TRANS_YX             (LABEL_START + 0x008)

/* 异常步透传接口        重要报文   */
#define LABEL_HALT_TRANS_CT             (LABEL_START + 0x009)

/* 异常步透传接口 连接报文   */
#define LABEL_HALT_TRANS_CN             (LABEL_START + 0x00a)

#endif


