#ifndef __CONSTANT_H__
#define __CONSTANT_H__

#include "baseConstant.h"

#ifdef __cplusplus
extern "C" {
#endif

// dbCenter数据库工具名称
#define SQLITE3_TOOL_NAME   "sqlite3"
//dbCenter配置文件默认名
#define LOG_CFG_NAME        "dbCenterData.cfg"
//日志文件的默认配置名
#define LOG_LOCAL_NAME      "dbCenter.ini"
//权限库表名
#define SAFE_POWER_NAME     "power.db"
#define SAFE_POWER_TABLE    "app_power_list"
//数据库磁盘路径
#define CHECH_DATA_PATH     "/data/app"

//程序PID文件存储路径
#define PID_PATH            "/var/run/dbCenter.pid"
#define FILE_PATH           "/data/app/dbCenter"
//配置文件路径
#define CFG_PATH            "/data/app/dbCenter/cfg"
//日志存储路径
#define LOG_DIR             "/data/app/dbCenter/log"
//BAD_DB_PATH
#define BAD_DB_PATH         "/data/app/dbCenter/bad"
//BAD_DB_PATH
#define BAD_BACKUP_DB_PATH  "/data/app/dbCenter/bad/backup"
#define BASEPATH            "/data/app/dbCenter/data/"
#define BACKUPPATH          "/data/app/dbCenter/data/backup/"
#define TMPFSPATH           "/tmp/data/"

#define WALCOUNT            500

//循环删除表数量(超出该数量后开始删除)
#define CYCLEDELETENUM      1000
//#自救删除百分比
#define AUTODELETENUM       10

#define DELETEINTERVAL      3600


#define IPCDOMAINPATH      "/run/dbcenter.sock"

#define DEFOULTDEPTH        10000

//#最大的客户端接入量
#define MAXCLIENTACCEPT     100
//#数据处理引擎线程数
#define HANDERTHREADNNUM    10
//#数据处理任务队列
#define HANDERTASKQUEUESIZE 128

#define UDPSERVERIP         "127.0.0.1"

#define UDPSERVERPORT       8598

//#dbCenter 远程调试服务IP 
#define DEBUGSQLIP          "127.0.0.1"
//#dbCenter 远程调试服务端口
#define DEBUGSQLPORT         8590

//#dbCenter TCP 服务IP
#define TCPSERVERIP         "127.0.0.1"
//#dbCenter TCP 服务端口
#define TCPSERVERPORT       8599

//#MQTT borker IP
#define MQTTBROKERIP        "172.17.0.1"
//#define MQTTBROKERIP        "127.0.0.1"

//#MQTT broker 端口
#define MQTTBROKERPORT      1883
//#MQTT 用户名
#define MQTTUSERNAME        "username"
//#MQTT 密码
#define MQTTPASSWORD        "passwd"

//#smiOS连接类型
#define SMIOSCONNECTTYPE    "MQTT"
//#smiOS连接地址
#define SMIOSIP             "127.0.0.1"
//#smiOS连接端口
#define SMIOSPORT           9001

#define IPCSMIOSDOMAINPATH "/run/smiOS.sock"

//默认ZLOG输出器 CAT 名称
#define ZLOG_CAT_NAME       "db"
//默认的服务名称
#define SERVICE_BASE_NAME     "dbCenter"

//广播消息主题
#define BROADCAST_NAME		  "Broadcast"

//smiOS名称
#define SMIOS_NAME_BASE          "smiOS"
// #define SMIOS_MQTT_NAME     "M-smiOS"
//smiOS 连接超时
#define SMIOS_LOGIN_TIMEOUT (5)
//smiOS POLL间隔秒
#define SMIOS_POLL_INTERVAL (1)

//#define DB_DELE_HD_TRIG_RATE 0.1   //0.3 -》01
#define DB_MAX_DELE_SUM 2
#define DB_MAX_DELE_SUM 2
#define DB_DELE_TIMER_INTERVAL 5000

#define DESC(desc,v) (v)


/*
 * Gives us 8 prio classes with 13-bits of data for each class
 */
#define IOPRIO_CLASS_SHIFT (13)
#define IOPRIO_PRIO_MASK ((1UL << IOPRIO_CLASS_SHIFT) - 1)

#define IOPRIO_PRIO_CLASS(mask) ((mask) >> IOPRIO_CLASS_SHIFT)
#define IOPRIO_PRIO_DATA(mask) ((mask)&IOPRIO_PRIO_MASK)
#define IOPRIO_PRIO_VALUE(class, data) (((class) << IOPRIO_CLASS_SHIFT) | data)

#define ioprio_valid(mask) (IOPRIO_PRIO_CLASS((mask)) != IOPRIO_CLASS_NONE)

    enum
    {
        IOPRIO_CLASS_NONE,
        IOPRIO_CLASS_RT,
        IOPRIO_CLASS_BE,
        IOPRIO_CLASS_IDLE,
    };

/*
 * 8 best effort priority levels are supported
 */
#define IOPRIO_BE_NR (8)

    enum
    {
        IOPRIO_WHO_PROCESS = 1,
        IOPRIO_WHO_PGRP,
        IOPRIO_WHO_USER,
    };

#ifdef __cplusplus
}
#endif

#endif
