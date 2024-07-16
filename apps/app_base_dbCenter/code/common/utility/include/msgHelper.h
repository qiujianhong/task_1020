#ifndef __MSG_HELPER_H__
#define __MSG_HELPER_H__

#include "baseType.h"
#include "xdrCustom.h"

#ifdef __cplusplus
extern "C" {
#endif

// #define MSG_BIG_ENCODE

//最大头部数据大小
#define MAX_MSG_HEADER (268)
//最大载荷大小
#define MAX_PAYLOAD_LEN (8192)
//最大消息消息消息大小
#define MAX_MSG_LEN (8192 + MAX_MSG_HEADER)

//00050013,除数据内容外，其他数据最大长度 
//接口协议中 17 =OPTIONAL+信息点号+SEQUENCE +OPTIONAL+数据时标长度
#define MAX_STEP_HEADER (17)

//最大的源地址字符串大小
#define MAX_SOURCE_LENGTH (128)
//最大的目标地址字符串大小
#define MAX_DESTINATION_LENGTH MAX_SOURCE_LENGTH
// XDR包单字节长度最大值
#define XDR_SIGNEL_LEN (0x80)
// MSG中PRM标准位偏移量
#define MSG_PRM_OFFSET (0)
// MSG中PRIORITY偏移量
#define MSG_PRIORITY_OFFSET (0)
// MSG中INDEX偏移量
#define MSG_INDEX_OFFSET (1)
// MSG中LABEL偏移量
#define MSG_LABEL_OFFSET (3)
// MSG中SOURCE偏移量
#define MSG_SOURCE_OFFSET (5)
// MSG中PYLOAD单字节长度LEN偏移量
#define MSG_PYLOAD_OFFSET_1 (1)
// MSG中PYLOAD变长长度LEN偏移量
#define MSG_PYLOAD_OFFSET_2 (2)
// MSG中PYLOAD变长长度LEN偏移量
#define MSG_PYLOAD_OFFSET_3 (3)

//从buf中获取PRM 标志位返回 0 或 1
#define GET_PRM(buf) (((uint8 *)buf)[MSG_PRM_OFFSET] & 0x01)
//获取优先级返回0 ~127
#define GET_PRIORITY(buf) (((uint8 *)buf)[MSG_PRIORITY_OFFSET] >> 1)
//返回消息源地址
#define GET_SOURCE(buf) (((uint8 *)buf) + MSG_SOURCE_OFFSET)
//返回消息目的地址
#define GET_DESTINATION(buf) (GET_SOURCE(((uint8 *)buf)) + (strlen((sint8 *)GET_SOURCE((buf)))) + 1)
//返回TAG地址 传输的参数不是buf 是des的地址
#define GET_TAG_ADDR(des) (des + strlen(des) + 1)
//返回MSG中payload长度的地址 传入的参数不是buf 传入参数时tag地址
#define GET_LENGTH_ADDR(tagaddr) (tagaddr + 4)
//返回MSG中payload长度 传入的参数时payload len地址
#define GET_LENGTH(lenaddr) (lenaddr[0] > 0x80 ? (lenaddr[1] << 8) + lenaddr[2] : lenaddr[0])

#if defined MSG_BIG_ENCODE
//返回INDEX值
#define GET_INDEX(buf) ((((uint8 *)buf)[MSG_INDEX_OFFSET] << 8) + ((uint8 *)buf)[MSG_INDEX_OFFSET + 1])
//返回LABEL值
#define GET_LABEL(buf) ((((uint8 *)buf)[MSG_LABEL_OFFSET] << 8) + ((uint8 *)buf)[MSG_LABEL_OFFSET + 1])
//返回TAG的值
#define GET_TAG(tagaddr) ((tagaddr[0] << 24) + (tagaddr[1] << 16) + (tagaddr[2] << 8) + tagaddr[3])
#else
//返回INDEX值
#define GET_INDEX(buf) ((((uint8 *)buf)[MSG_INDEX_OFFSET]) + (((uint8 *)buf)[MSG_INDEX_OFFSET + 1] << 8))
//返回LABEL值
#define GET_LABEL(buf) ((((uint8 *)buf)[MSG_LABEL_OFFSET] ) + (((uint8 *)buf)[MSG_LABEL_OFFSET + 1] << 8))
//返回TAG的值
#define GET_TAG(tagaddr) ((tagaddr[3] << 24) + (tagaddr[2] << 16) + (tagaddr[1] << 8) + tagaddr[0])

#endif



/* @brief:消息体类型  */
typedef enum _msg_type
{
    UDP = 0,
    TCP,
    IPC,
    MQTT
} MSG_TYPE_E;

/* @brief:数据包的封装 由长度及载荷构成 */
typedef struct raw_obj
{
    sint32 length;
    //最大消息体目前使用MAX_MSG_LEN
    uint8 payload[MAX_MSG_LEN];
} RAW_OBJ_T;

/* @brief:消息内容结构体封装  */
typedef struct _msg_obj
{
    uint8 prm;           // prm标志位
    uint8 priority;      //优先级
    uint16 index;        //索引
    uint16 label;        //标识
    sint8 *source;       //原地址
    sint8 *destination;  //目标地址
    uint32 tag;          //信息标识
    uint32 length;       //消息载荷长度
    uint8 *payload;      //消息载荷内存地址
} MSG_OBJ_T;

sint32 pro_parser_msg(uint8 *rxbuf, uint32 rxbuf_len, MSG_OBJ_T *msgObj);

const sint8 *str_msg_type(MSG_TYPE_E type);

sint32 build_msg(MSG_OBJ_T *msgObj, uint8 *payload, sint32 payload_len, RAW_OBJ_T *rsp);

sint32 build_login_msg(MSG_OBJ_T *msgObj, MQT_PLUGIN_T *mqt, RAW_OBJ_T *rsp);

#ifdef __cplusplus
}
#endif

#endif