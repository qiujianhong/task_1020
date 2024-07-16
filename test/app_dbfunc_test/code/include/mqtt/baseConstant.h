#ifndef __BASE_CONSTANT_H__
#define __BASE_CONSTANT_H__

#ifdef __cplusplus
extern "C"
{
#endif

//IP地址字符串格式最大长度
#define IP_LEN (16)
//Port字符串最大长度
#define PORT_LEN (6)
//连接类型的长度 TCP UDP IPC MQTT 
#define CONNECT_TYPE_LEN (8)
//UNIX SOCKET IPC通讯路径
#define UNIX_SOCKET_PATH_LEN (128)
//客户端键名长度
#define CLIENT_KEY_LEN (256)
//路径通用长度
#define PATH_LEN (128)
//APP 最大名称长度
#define MAX_APP_NAME_LEN (64)
// Device Name 最大名称长度
#define MAX_DEVICE_NAME_LEN (128)
// UDP服务超时检测间隔
#define UDP_TIMER_POLL_INTERVAL (1)
// UDP最大传输单元设置
#define MTU_UDP (1472)
//最小有效报文大小
#define MIN_RAW_BUF_LEN (13)
// TCP Socket 监听数量
#define TCP_LISTEN_NUM (10)
// MQTT 用户ID最大长度
#define MAX_USER_ID_LEN (128)
// MQTT TOPIC最大长度
#define MAX_TOPIC_LEN (256)
// MQTT 用户名最大长度
#define MAX_USER_NAME_LEN (32)
// MQTT 密码最大长度
#define MAX_PASSWD_LEN (32)
// MSG执行命令的个数
#define MAX_CMD_NUM (16)
//最多订阅标签个数
#define MAX_SUB_NUM (16)
//最大URL长度
#define MAX_URL_LEN (128)
//最大的时间字符串长度
#define MAX_TIME_STR_LEN (64)

#ifdef __cplusplus
}
#endif

#endif
