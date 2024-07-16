

#ifndef __CTSMAIN_H__
#define __CTSMAIN_H__
#define LOGIC_ID_YC 0      //
#define LOGIC_ID_PD 7      //配网逻辑设备号7

#define MSG_MQTT 1     //消息是否用mqtt的方式
#define DBCENTER_MQTT 1       //数据中心是否用mqtt的方式

#define PLC_NUM 2   //定义1路载波抄表模块 第2路给蓝牙用

#ifdef PRODUCT_ECU
#define MAX_485_NUM 10   //能源控制器开6路485抄表 2路can口 最后两路给mbus

#else

#define MAX_485_NUM 4   //2路485抄表 2路can口
#endif

#ifdef AREA_FUJIAN
#define COMM_MODULE_NUM 1  //一路采集通信模块任务
#define ACMETER_NUM 1  //一路本体任务 包括交采
#else
#define COMM_MODULE_NUM 0  
#define ACMETER_NUM 0  
#endif
#define AMR_THREAD_NUM (PLC_NUM+MAX_485_NUM+COMM_MODULE_NUM+ACMETER_NUM)

#define FAC_VERSION "XL01"            //厂商版本号
#define FAC_CODE "XLDZ"                //厂商代码
#define FAC_PUB_TIME "XL-2020-05-30"    //发布日期

#define APPNAME "M-taskManager"     
#define LOCAL_IP "192.168.9.18"
#define DATACENTER_IP "192.168.9.18"     //数据中心ip
#define DATACENTER_PORT 8598    //数据中心端口
#define DBCENTER_NAME "M-dbCenter"

#define CCO_APP_NAME "M-ccoRouter"
#define CCO_IP "192.168.9.18"
#define CCO_PORT 9012

//#define TASK_PORT  11  //定义任务管理线程端口
#define UP_PORT 9023

#define MAPMANAGER_APP_NAME "M-mapManager"
#define WIRELESSDCM_APP_NAME "M-wirelessDCM"
#define PUAMR_APP_NAME "M-puAmr"

#define METER_FILE_MODIFY 0xAA            //电表档案修改
#define METER_FILE_CLEAR 0x55            //电表档案全清
#define METER_FILE_NULL 0x00            //电表档案无改变

#define MSG_INIT 0               //初始化连接
#define MSG_RECV 1               //数据接收
#define MSG_SEND 2               //s数据发送
#define MSG_CLOSE 3               //关闭连接
#define MSG_LOGIN 4           //登录到系统管理器
#define MSG_LOGIN_WAIT 5       //登录等待回复 
extern char     CommLog[100];
extern char     TaskLog[100];
extern char     RecLog[100];
extern char     RelayLog[100];
extern char     UpLog[100];
extern char     MqttLog[100];
extern char     TgLog[100];

#define COMM_LOG ((CommLog[0]!=0)?CommLog:"/data/app/taskManager/log/ctscomm.log")  //报文日志
#define TASK_LOG ((TaskLog[0]!=0)?TaskLog:"/data/app/taskManager/log/ctstask.log")  //任务以及档案日志
#define REC_LOG ((RecLog[0]!=0)?RecLog:"/data/app/taskManager/log/ctsrec.log")    //抄表日志
#define UP_LOG ((UpLog[0]!=0)?UpLog:"/data/app/taskManager/log/ctsup.log")        //上行消息日志
#define RELAY_LOG ((RelayLog[0]!=0)?RelayLog:"/data/app/taskManager/log/ctsrelay.log")            //穿透日志
#define MQTT_LOG ((MqttLog[0]!=0)?MqttLog:"/data/app/taskManager/log/ctsmqtt.log")         //mqtt日志
#define TG_LOG ((TgLog[0]!=0)?TgLog:"/data/app/taskManager/log/ctstg.log")     //总加组抄表线程日志

#define LOT_METER_SPRDT (83) //单相居民用户物联网表
#define LOT_METER_TPIND (84) //三相工业用户物联网表
#define LOT_METER_TPCHK (99) //三相计量考核物联网表


#define DB_AGENT_CTS 0x1000     
#define DB_CLIENT_MAX 20       //客户端最大个数
#define DB_CLIENT_MAIN (DB_AGENT_CTS+0X100)
#define DB_CLIENT_TASK (DB_CLIENT_MAIN+1)
#define DB_CLIENT_TG (DB_CLIENT_TASK+1)
#define DB_CLIENT_WRITE (DB_CLIENT_TG+1)
#define DB_CLIENT_AMR (DB_CLIENT_WRITE+1) //不能往后面加 只能往前面加


#define IPC_MQTT 0XA0000000
#define IPC_MAIN (IPC_MQTT + 1)
#define IPC_TASK (IPC_MAIN + 1)
#define IPC_AMR (IPC_TASK + 1)

#define MSG_DBUPDATE 0X2200      
#define MSG_MODULE_UPDATE (MSG_DBUPDATE+1)
#define MSG_SEARCH_METER (MSG_MODULE_UPDATE+1)
#define MSG_EVENT_REPORT (MSG_SEARCH_METER+1)
#define MSG_HPLC_INFO (MSG_EVENT_REPORT+1)
#define MSG_POWER_ON_OFF_COL (MSG_HPLC_INFO+1)
#define MSG_POWER_ON_OFF_MET (MSG_POWER_ON_OFF_COL+1)
#define MSG_NODE_REJECT (MSG_POWER_ON_OFF_MET+1)
#define MSG_RECORD_FILE_DEL (MSG_NODE_REJECT+1)
#define MSG_HPLC_CHANNEL_CHANGE (MSG_RECORD_FILE_DEL+1) 
#define MSG_HPLC_ACROSSAREA_CHANGE (MSG_HPLC_CHANNEL_CHANGE+1)
#define MSG_HPLC_NODEREJECT_CHANGE (MSG_HPLC_ACROSSAREA_CHANGE+1)
#define MSG_METERFILE_CLEAR (MSG_HPLC_NODEREJECT_CHANGE+1)
#define MSG_DATA_INIT (MSG_METERFILE_CLEAR+1)
#define MSG_CCO_STA_INFO (MSG_DATA_INIT+1)
#define MSG_MODULE_INFO (MSG_CCO_STA_INFO+1)
#define MSG_FILE_TRANSFER (MSG_MODULE_INFO+1)
#define MSG_HPLC_UPDATE (MSG_FILE_TRANSFER+1)

#ifdef AREA_JIANGXI
#define MSG_TMN_POWER_ON_OFF (MSG_HPLC_UPDATE+1)
#endif
#ifdef AREA_FUJIAN
#define MSG_PRIORITY_UPDATE  (MSG_HPLC_UPDATE+1)
#define MSG_COLLECTION_UPDATE (MSG_PRIORITY_UPDATE+1)
#define MSG_PRIORITY_CHANGE  (MSG_COLLECTION_UPDATE+1)
#endif

extern void* DownThread ( void* arg );
extern void* AmrThread ( void* arg );
extern void* AmrThread_485 ( void* arg );
extern void* UpThread ( void* arg );
extern void* TaskThread ( void* arg );
extern void* MqttThread ( void* arg );
extern void taskmng_amr_thread_queue_init ( void* arg );
extern void taskmng_init_file_load();
extern void taskmng_privatedir_prepare(char * Dir);
extern void * TGThread(void * arg);
extern void Init_Rs485_Pipe();
extern void Init_Get_Grop_Meter();

#endif

