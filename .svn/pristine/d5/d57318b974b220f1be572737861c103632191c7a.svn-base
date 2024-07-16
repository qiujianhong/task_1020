/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控公共数据
* @date：    2020-6-30
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef	__UART_COMM_H
#define __UART_COMM_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define  APP_RECV_BUF_LEN      8192	/*recvfrom接收缓冲区大小*/

#define UART_APP_NAME     "uartManager"
#define UART_INITFILE     UART_APP_NAME".ini"
#define UART_DATAFILE     UART_APP_NAME".json"

#define APP_SUBTOPIC1      "+/M-uartManager/#"
#define APP_SUBTOPIC2      "+/uartManager/#"
#define APP_SUBTOPIC3      "M-mapManager/Broadcast/#"

#define APP_DEFAULT_TOPIC "M-uartManager/M-smiOS"

#define UART_DB_CLIENT_NUM   4

#define UART_MAP_ID_MIN    1
#define UART_MAP_ID_MAX    5

#define UART_APPID_MAX     32

/* 串口服务APP公共数据 */
typedef struct tag_UART_APPDATA_T
{
    uint32      mqttip;       //mqtt地址
    uint16      mqttport;     //mqtt端口
    uint8       appidlen;
    uint8       appid[UART_APPID_MAX];
    char       *appName;      //APP名称
    int         dbAgent;
    time_t      msgtime;  //消息收到的时间
}UART_APPDATA_T;

#ifdef __cplusplus
}
#endif

#endif


