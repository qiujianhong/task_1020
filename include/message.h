/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief：   统一终端平台消息定义
* @date：    2019/12/27
* @history： 
*********************************************************************
*/

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "dataType.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define  NAME_BROADCAST                "Broadcast"

/*通用消息接口*/
#define MESSAGE_IID_COMMON             0x0000
#define COMMON_IOP_URGENT_POWERDOWN    0x0001    //紧急停电（事件）
#define COMMON_IOP_SYSTEM_POWERDOWN    0x0002    //系统停电（事件）
#define COMMON_IOP_SYSTEM_RESTART      0x0003    //系统复位（事件）
#define COMMON_IOP_SYSTEM_POWERON      0x0004    //系统上电（事件）
#define COMMON_IOP_SYSTEM_TIMING       0x0005    //系统校时（事件）
#define COMMON_IOP_HERTBEAT            0x0010    //心跳检测
#define COMMON_IOP_MAINTAIN            0x0011    //执行维护命令
#define COMMON_IOP_TRANSPORT           0x0012    //透明传输
#define COMMON_IOP_VERSION             0x0013    //版本信息
#define COMMON_IOP_ERRMSG              0x0014    //错误信息
#define COMMON_IOP_GETAPPID            0x0016    //查询APPID


/*系统管理器消息接口*/
#define MESSAGE_IID_SYSTEM             0x0001
#define SYSTEM_IOP_EVENT_ETHSTATUS     0x0001    //以太网插拔
#define SYSTEM_IOP_EVENT_IPCHANGE      0x0002    //IP地址变更事件
#define SYSTEM_IOP_APP_REGISTER        0x0010    //App请求注册
#define SYSTEM_IOP_APP_UNREGISTER      0x0011    //App取消注册
#define SYSTEM_IOP_GET_REGISTERAPP     0x0012    //查询已注册App
#define SYSTEM_IOP_GET_APP             0x0013    //查询指定App信息
#define SYSTEM_IOP_SUB_EVENT           0x0020    //事件订阅
#define SYSTEM_IOP_UNSUB_EVENT         0x0021    //事件取消订阅
#define SYSTEM_IOP_SEEK_SUBEVENT       0x0022    //查询订阅的事件信息
#define SYSTEM_IOP_REQUIRE_RESTART     0x0030    //请求系统重启
#define SYSTEM_IOP_GET_POWERSTATUS     0x0031    //查询电源状态
#define SYSTEM_IOP_GET_ETHSTATUS       0x0032    //查询以太网插拔状态
//#define SYSTEM_IOP_REG_SERVERHOST      0x0033    //注册服务主站地址
//#define SYSTEM_IOP_UNREG_SERVERHOST    0x0034    //取消注册服务主站地址
#define SYSTEM_IOP_SET_IPADDR          0x0035    //设置本机IP地址
#define SYSTEM_IOP_ADD_ROUTE           0x0036    //添加路由表
#define SYSTEM_IOP_DEL_ROUTE           0x0037    //删除路由表
#define SYSTEM_IOP_RCT                 0x0038    //时钟同步
#define SYSTEM_IOP_START_SNTP          0x0039    //启用SNTP对时
#define SYSTEM_IOP_GET_DEVINFO         0x003A    //获取设备信息
#define SYSTEM_IOP_SYSTEM_CMD          0x003B    //执行system命令
#define SYSTEM_IOP_GET_CONTAINER       0x003C    //获取容器信息
#define SYSTEM_IOP_GET_CONTAINER_APP   0x003D    //获取容器内App信息
#define SYSTEM_IOP_GET_CUR_DEVINFO     0x003E    //获取设备当前运行信息
#define SYSTEM_IOP_APP_MANAGER         0x0040    //APP管理
#define SYSTEM_IOP_SERVICE_START       0x0040    //启动服务
#define SYSTEM_IOP_SERVICE_STOP        0x0041    //关闭服务

/*无线模块拨号管理App消息接口*/
#define MESSAGE_IID_WIRELESS           0x0002
#define WIRELESS_IOP_DIAG_EVENT        0x0001    //拨号状态变化事件
#define WIRELESS_IOP_GET_MODULEINFO    0x0010    //查询模块信息
#define WIRELESS_IOP_GET_DIAGINFO      0x0011    //查询拨号状态
#define WIRELESS_IOP_SEND_SMS          0x0012    //发送短信
#define WIRELESS_IOP_REVC_SMS          0x0013    //接收短信
#define WIRELESS_IOP_SET_DIAGARG       0x0014    //修改拨号参数
#define WIRELESS_IOP_REDIAG            0x0015    //重新拨号
#define WIRELESS_IOP_GET_GPS           0x0016    //获取GPS定位信息
#define WIRELESS_IOP_SET_AUTH          0x0017    //设置鉴权方式
#define WIRELESS_IOP_SET_SMSNO         0x0018    //设置短信中心号码
#define WIRELESS_IOP_GET_NUM           0x0019    //拨号统计
#define WIRELESS_IOP_TRANS_AT          0x0020    //透明AT指令
#define WIRELESS_IOP_GTOPTSCAN_AT      0xFF01    //网络信号搜索
#define WIRELESS_IOP_GTOPTSCAN_LAST_AT 0xFF02    //提供最近1次信号搜索情况

/*本地抄表模块管理App消息接口*/
#define MESSAGE_IID_CCOROUTER          0x0003
#define CCOROUTER_IOP_REPORT_EVENT     0x0001    //上报事件数据
#define CCOROUTER_IOP_OFFLINE_EVENT    0x0002    //上报离网事件
#define CCOROUTER_IOP_PORT_CFG         0x0010    //抄表端口功能配置
#define CCOROUTER_IOP_ADD_METER        0x0020    //添加表档案
#define CCOROUTER_IOP_DEL_METER        0x0021    //删除表档案
#define CCOROUTER_IOP_CLEAR_METER      0x0022    //清除表档案
#define CCOROUTER_IOP_REPORT_METER     0x0031    //上报抄表数据
//#define CCOROUTER_IOP_REPORT_EVENT     0x0032    //上报事件数据
#define CCOROUTER_IOP_TRANS_DATA       0x0033    //数据透传
#define CCOROUTER_IOP_START_SEEKMETER  0x0034    //启动搜表
#define CCOROUTER_IOP_PAUSE_SEEKMETER  0x0035    //暂停搜表
#define CCOROUTER_IOP_REPORT_SEEKDATA  0x0036    //上报搜表数据
#define CCOROUTER_IOP_GET_NODE_DELAY   0x0037    //获取从节点监控延时
#define CCOROUTER_IOP_MONITOR_NODE     0x0038    //从节点监控
#define CCOROUTER_IOP_GET_BROAD_DELAY  0x0039    //获取广播延时
#define CCOROUTER_IOP_BROADCAST        0x003a    //广播
#define CCOROUTER_IOP_SET_ADDR         0x003b    //模块地址设置
#define CCOROUTER_IOP_RECORD_NUM       0x003d    //当前配置档案数
#define CCOROUTER_IOP_ACTIVE_READMETER 0x003e    //主动抄表
#define CCOROUTER_IOP_PAUSE_READMETER  0x0040    //暂停抄表
#define CCOROUTER_IOP_RECOV_READMETER  0x0041    //恢复抄表
#define CCOROUTER_IOP_RESTAR_READMETER 0x0042    //重启抄表
#define CCOROUTER_IOP_SEEK_METER       0x0043    //查询档案
#define CCOROUTER_IOP_GET_HPLC         0x0050    //查询HPLC信息
#define CCOROUTER_IOP_REPORT_HPLC      0x0051    //上报HPLC查询信息
#define CCOROUTER_IOP_SET_HPLC_FRE     0x0052    //HPLC频点设置
#define CCOROUTER_IOP_GET_HPLC_FRE     0x0053    //HPLC频点查询
#define CCOROUTER_IOP_GET_HPLC_AREA    0x0054    //HPLC台区识别
#define CCOROUTER_IOP_GET_HPLC_OFFLINE 0x0055    //HPLC离网感知
#define CCOROUTER_IOP_HPLC_REFUSE      0x0056    //HPLC拒绝节点上报
#define CCOROUTER_IOP_GET_SEEKMETER_ST 0x0060    //查询抄表状态
#define CCOROUTER_IOP_GET_SEEKMODULE   0x0061    //查询抄表模块信息

#define CCOROUTER_IOP_MTREVENT_RPT     0x0003    //福建扩展，事件上报通知

/*模组管理器消息接口*/
#define MESSAGE_IID_MODULE             0x0004
#define MODULE_IOP_STATUS_EVENT        0x0001    //模组插拔事件
#define MODULE_IOP_GET_INFO            0x0010    //查询模组信息
#define MODULE_IOP_GET_PORTINFO        0x0011    //查询模组槽位信息
#define MODULE_IOP_GET_STATUS          0x0012    //查询模组插拔状态
#define MODULE_IOP_RESTORE             0x0013    //模组复位
#define MODULE_IOP_RESTART             0x0014    //模组重启
#define MODULE_IOP_FILE_TRANSFER       0x0015    //文件传输
#define MODULE_IOP_GET_ALL             0x0016    //查询所有模组信息

/*数据中心消息接口*/
#define MESSAGE_IID_DB                 0x0005
#define DB_IOP_UPDATE_EVENT            0x0001    //数据更新事件
#define DB_IOP_INIT_EVENT              0x0002    //初始化事件
#define DB_IOP_READ_NORMAL_DATA        0x0010    //读普通数据
#define DB_IOP_WRITE_NORMAL_DATA       0x0011    //写普通数据
#define DB_IOP_READ_RECORD_START       0x0012    //开始读记录型数据
#define DB_IOP_READ_RECORD_STEP        0x0013    //单步读取记录
#define DB_IOP_READ_RECORD_END         0x0014    //结束记录型数据读取
#define DB_IOP_WRITE_RECORD            0x0015    //写记录型数据
#define DB_IOP_UPDATE_RECORD           0x0016    //更新记录数据
#define DB_IOP_GET_RECORD_NUM          0x0020    //获取存储记录条数
#define DB_IOP_GET_RECORD_DEPTH        0x0021    //获取存储记录深度
#define DB_IOP_INIT_DATA               0x0030    //数据初始化
#define DB_IOP_CLEAR_ONE_DATA          0x0031    //清除指定数据
#define DB_IOP_RECOVERY_DEFAULT        0x0032    //恢复出厂
#define DB_IOP_READ_RECORD_START_FJ    0xF012    //开始读记录型数据(福建特有)
#define DB_IOP_CLEAR_ONE_DATA_FJ       0xF031    //清除指定数据(福建特有)

/*安全在线监测消息接口*/
#define MESSAGE_IID_SCS                0x0006
#define SCS_IOP_ETH_PORT_OPEN          0x0001    //以太网远程端口开放监控
#define SCS_IOP_ETH_PORT_CONN          0x0002    //以太网外联信息监控
#define SCS_IOP_KEY_PATH_CHGE          0x0003    //关键目录文件变更监控
#define SCS_IOP_ETH_LOGIN_CONSOLE      0x0004    //以太网登录控制台监控
#define SCS_IOP_DANGER_OPREATE         0x0005    //危险命令操作监控
#define SCS_IOP_LOCAL_MAINTAIN         0x0006    //SSH服务异常监控
#define SCS_IOP_PASSWARD_CHANGED       0x0007    //口令信息变更异常监控
#define SCS_IOP_UART_PORT_LOGIN        0x0008    //串口登录控制台监控
#define SCS_IOP_SESSION_ABNORMAL       0x0009    //流量会话异常监控
#define SCS_IOP_BANDWIDTH_ABNORMAL     0x000A    //流量带宽异常监控
#define SCS_IOP_PROCESS_ABNORMAL       0x000B    //进程异常监控
#define SCS_IOP_LOGIN_ABNORMAL         0x000C    //用户登录异常监控
#define SCS_IOP_SERVICE_OPEN           0x0017    //启动服务
#define SCS_IOP_SERVICE_CLOSE          0x0018    //关闭服务
#define SCS_IOP_FLOW_PARAM_SET         0x0010    //配置流量超限参数
#define SCS_IOP_FLOW_PARAM_GET         0x0011    //查询流量超限参数
#define SCS_IOP_TIME_OUT_SET           0x0012    //配置长时间未关闭服务判断参数

/*通信代理消息接口*/
#define MESSAGE_IID_PROXY              0x1776
#define PROXY_IOP_PROTOCOL_DATA_SEND   0x0011    //协议数据发送
#define PROXY_IOP_PHYSIC_CONNECT       0x0012    //proxy与主站物理连接状态通知
#define PROXY_IOP_APPLY_CONNECT        0x0013    //用采与主站应用连接状态通知

/*桌面消息接口*/
#define MESSAGE_IID_DESKGUI            0x0007
#define DESKGUI_IOP_DISPLAY_ENENT      0x0001    //轮显通知
#define DESKGUI_IOP_APP_ACTIVATE       0x0002    //app激活事件
#define DESKGUI_IOP_APP_REG            0x0010    //app注册
#define DESKGUI_IOP_APP_UNREG          0x0011    //app退出并返回桌面
#define DESKGUI_IOP_FRESH_STATE        0x0012    //刷新状态栏
#define DESKGUI_IOP_FRESH_TOOL         0x0013    //刷新工具栏
#define DESKGUI_IOP_PUSH_HINT          0x0014    //弹出信息提示框
#define DESKGUI_IOP_POP_HINT           0x0015    //取消信息提示框
#define DESKGUI_IOP_EXIT_DISPLAY       0x0016    //app退出轮显
#define DESKGUI_IOP_LIVE_APP           0x0017    //当前活跃app
#define    DESKGUI_IOP_SCREEN_CTR      0x0018    //屏显控制

/*遥信脉冲管理*/
#define MESSAGE_IID_RSPSAMPLE          0x0008
#define RSPSAMPLE_IOP_RMT_EVENT        0x0001    //遥信变位事件
#define RSPSAMPLE_IOP_RMT_STATE        0x0011    //遥信状态
#define RSPSAMPLE_IOP_PULSE_STATIC     0x0012    //脉冲计数及统计时间
#define RSPSAMPLE_IOP_CONFIGURE        0x0013    //遥信脉冲功能配置
#define RSPSAMPLE_IOP_SHAKE_TIME       0x0014    //遥信防抖时间
//福建专三扩充的消息
#define RSPSAMPLE_IOP_WORKMODE_GET     0x0020    //查询485-2口当前工作模式
#define RSPSAMPLE_IOP_WORKMODE_SET     0x0021    //设置485-2口当前工作模式

/*串口服务管理*/
#define MESSAGE_IID_UART               0x0009
#define UART_IOP_NOTIFY_MODE           0x0001    //模式变更通知
#define UART_IOP_NOTIFY_RECV           0x0002    //接收数据通知
#define UART_IOP_GET_INFO              0x0010    //串口信息查询
#define UART_IOP_SET_PARAM             0x0011    //串口参数设置
#define UART_IOP_SET_LISTEN            0x0012    //从模式APP注册
#define UART_IOP_SEND_DATA             0x0013    //数据发送
#define UART_IOP_SET_ABNORMAL          0x0014    //通信异常通知
#define UART_IOP_UNSET_LISTEN          0x0015    //取消从模式APP注册
#define UART_IOP_ONLYMODE_SET          0x0016    //串口模式设置

/*蓝牙服务管理*/
#define MESSAGE_IID_BLE                0x000A
#define BLE_IOP_RECV_EVENT             0x0001    //报文通知
#define BLE_IOP_PORT_EVENT             0x0002    //蓝牙端口通知
#define BLE_IOP_SEND_DATA              0x0010    //发送数据
#define BLE_IOP_TRAN_DATA              0x0011    //蓝牙转发
#define BLE_IOP_SET_CONFIG             0x0012    //蓝牙参数配置
#define BLE_IOP_GET_CONFIG             0x0013    //蓝牙参数查询
#define BLE_IOP_SET_PORT               0x0014    //蓝牙端口配置
#define BLE_IOP_LINK_INFO              0x0015    //蓝牙连接信息
#define BLE_IOP_CHECK_METER            0x0016    //蓝牙检表配置

//自定义获取蓝牙sn
#define BLE_IOP_BT_SN                  0x0090    //蓝牙连接信息
#define BLE_IOP_SET_BT_SN              0x0091    //蓝牙连接信息

/*电台服务管理*/
#define MESSAGE_IID_RADIO              0x200A
#define RADIO_IOP_RECV_EVENT           0x0001    //报文通知
#define RADIO_IOP_SEND_DATA            0x0010    //发送数据
#define RADIO_IOP_SET_CONFIG           0x0011    //电台参数配置
#define RADIO_IOP_SET_ADDR             0x0012    //电台参数配置
#define RADIO_IOP_GET_CONFIG           0x0013    //电台参数查询
#define RADIO_IOP_GET_STATUS           0x0014    //电台状态查询
#define RADIO_IOP_SET_YANG             0x0015    //电台状态查询
#define RADIO_IOP_LONG_SENG            0x0016    //电台长发
#define RADIO_IOP_GET_VERSION          0x0017    //电台版本查询


/*物联管理*/
#define MESSAGE_IID_IOT                0x000B
#define IOT_IOP_DOCKER_ACTION          0x0001
#define IOT_IOP_DOCKER_WARRING         0x0002
#define IOT_IOP_APP_ACTION             0x0003
#define IOT_IOP_APP_WARRING            0x0004
#define IOT_IOP_SYS_MOVE_UP            0x0006
#define IOT_IOP_SYS_WARRING            0x0005
#define IOT_IOP_DOCKER_INSTALL         0x0010
#define IOT_IOP_DOCKER_ACTION1         0x0011
#define IOT_IOP_DOCKER_SET             0x0012
#define IOT_IOP_DOCKER_FIND            0x0013
#define IOT_IOP_APP_INSTALL            0x0014
#define IOT_IOP_APP_ACTION1            0x0015
#define IOT_IOP_APP_SET                0x0016
#define IOT_IOP_APP_FIND               0x0017
#define IOT_IOP_APP_LICENSE_SET        0x0018
#define IOT_IOP_APP_LICENSE_FIND       0x0019
#define IOT_IOP_SYS_MOVE_UP1           0x001A
#define IOT_IOP_SYS_VERSION_FIND       0x001B
#define IOT_IOP_LOG_INFO_FIND          0x001C
#define IOT_IOP_LOG_PACK               0x001D
#define IOT_IOP_LOG_DELECT             0x001E
#define IOT_IOP_SYS_INSPECT            0x001F


/*采集任务调度管理APP消息接口*/
#define MESSAGE_IID_CTS                0x1003
#define CTS_IOP_READ_LIST_REQ          0x0010    //透明转发
#define CTS_IOP_READ_ONE_REQ           0x0011    //代理
#define CTS_IOP_SET_LIST_REQ           0x0021    //请求代理设置若干个服务器的若干个对象属性
#define CTS_IOP_SET_READ_LIST_REQ      0x0013    //当日抄读状态
#define CTS_IOP_ACT_LIST_REQ           0x0014    //请求代理操作若干个服务器的若干个对象方法
#define CTS_IOP_ACT_READ_LIST_REQ      0x0015    //请求代理操作后读取若干个服务器的若干个对象方法和属性
#define CTS_IOP_TRANSPORT_REQ          0x0016    //请求代理操作透明转发命令
#define CTS_IOP_CARRIRT_TRANS          0x0020    //载波/微功率无线接口透明转发
#define CTS_IOP_SEEK_METER             0x0012    //搜表操作

#define CTS_IOP_TRANSTASK_RPT          0x0001    //福建扩展，透明任务上报通知
#define CTS_IOP_TRANSTASK_STATE        0x0030    //福建扩展，透明任务采集状态更新
#define CTS_IOP_POWERUP_EVT       	   0x0031    //福建扩展，终端上电事件消息
#define CTS_IOP_DEL_PN_DATE            0x1022     //删除测量点数据


/*专变负荷控制APP消息接口*/
#define MESSAGE_IID_LOADCTRL           0x1004
#define LOADCTRL_IOP_ALARM             0x0010    /* 告警操作 */
#define LOADCTRL_IOP_TRIP              0x0011    /* 远程跳闸 */
#define LOADCTRL_IOP_CLOSE             0x0012    /* 远程合闸 */
#define LOADCTRL_IOP_SELECT            0x0013    /* 遥控设置 */
#define LOADCTRL_IOP_PROTECT           0x0014    /* 保电操作 */
#define LOADCTRL_IOP_FEE_INPUT         0x0015    /* 催费告警投入 */
#define LOADCTRL_IOP_FEE_REMOVE        0x0016    /* 催费告警解除 */
#define LOADCTRL_IOP_PERIOD_CHANGE     0x0017    /* 时段功控方案切换 */
#define LOADCTRL_IOP_INPUT             0x0018    /* 控制投入 */
#define LOADCTRL_IOP_REMOVE            0x0019    /* 控制解除 */
#define LOADCTRL_IOP_RELAY_MODE        0x001A    /* 继电器模式 */


/*采集任务调度管理APP消息接口*/
#define MESSAGE_IID_SELF               0x1155
#define SELF_IOP_GET_VERSION           0x9090    //查询厂商版本信息

/*厂家自定义消息接口*/
#define MESSAGE_IID_ACMETER            0xFFF0
#define ACMETER_IOP_AC_PARM_INIT       0x0001    //交采参数初始化
#define ACMETER_IOP_AC_DATA_INIT       0x0002    //交采数据初始化
#define ACMETER_IOP_AC_DEMAND_INIT     0x0003    //交采需量初始化
#define ACMETER_IOP_AC_SYSTEM_CMD      0x0004    //执行system命令
#define ACMETER_IOP_AC_RATE_SET        0x0005    //交采费率设置

/*台区状态管理APP消息接口*/
#define MESSAGE_IID_PDS                0x1004
#define PDS_IOP_TOPOLOGY_START         0x0010    //启动拓扑

/*环境监测APP消息接口*/
#define MESSAGE_IID_ENVMONITOR    0x1005
#define ENV_IOP_EVENT             0x0001    //上报事件

/*JSON接口*/
#define MESSAGE_IID_INTERFACE          0xFFF1
#define INTERFACE_IOP_POP_CTRL         0x0001    //取消控制信息提示框
#define INTERFACE_IOP_FRESH_STATE      0x0012    //刷新状态栏
#define INTERFACE_IOP_FRESH_TOOL       0x0013    //刷新工具栏
#define INTERFACE_IOP_PUSH_CTRL        0x0014    //弹出控制信息提示框
#define INTERFACE_IOP_VOICE            0x0020    //语音提示
#define INTERFACE_IOP_UPDATE_PROGRESS  0x0021    //升级进度(模组)
#define INTERFACE_IOP_UPDATE_DB        0x0022    //参数变更

#define MESSAGE_RPM_RECV        0
#define MESSAGE_RPM_START       1
#define MESSAGE_ADDR_LEN        64
/* 考虑字节对齐, 没有严格按照报文格式, 收到的消息解析成如下格式 */
typedef struct tagMessageInfo
{
    uint8  rpm;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  priority;    /* 优先级 */

    uint8  sourlen;
    uint8  destlen;
    char   souraddr[MESSAGE_ADDR_LEN];
    char   destaddr[MESSAGE_ADDR_LEN];
    
    uint16 index;       /* 消息序号 */
    uint16 label;       /* 消息标签 */
    
    uint16 IID;
    uint16 IOP;

    uint32 msglen; 
    uint8  playload[0];
}MESSAGE_INFO_T;

#define MQTT_TOPIC_A_XDR           "A-XDR"
#define MQTT_TOPIC_JSON            "JSON"

/* TTU-MQTT主题相关 */
#define MQTT_TOPIC_OPER_GET        "get"
#define MQTT_TOPIC_OPER_SET        "set"
#define MQTT_TOPIC_OPER_ACT        "action"
#define MQTT_TOPIC_OPER_REPORT     "report"
#define MQTT_TOPIC_OPER_TRANS      "transparant"
#define MQTT_TOPIC_INFO_REQ        "request"
#define MQTT_TOPIC_INFO_RES        "response"
#define MQTT_TOPIC_INFO_NOTIFY     "notification"
#define MQTT_TOPIC_INFO_NOTIFYOS   "notify"

/* 系统管理器服务 */
#define SYS_APP_NAME               "OS-system"
#define SYS_TOPIC_REGISTER         "register"
#define SYS_TOPIC_UNREGISTER       "unRegister"
#define SYS_TOPIC_KEEPALIVE        "keepAlive"
#define SYS_TOPIC_GETREGLIST       "getRegisterList"
#define SYS_TOPIC_GETREGAPP        "getRegisterApp"
#define SYS_TOPIC_DEVINFO          "devInfo"
#define SYS_TOPIC_DEVSATAUS        "devStatus"
#define SYS_TOPIC_POWERSATAUS      "powerStatus"
#define SYS_TOPIC_GETETH           "getEthCfg"
#define SYS_TOPIC_SETETH           "setEthCfg"
#define SYS_TOPIC_ETHSTATUS        "ethStatus"
#define SYS_TOPIC_GETAPPINFO       "getAppInfo"
#define SYS_TOPIC_RESTARTAPP       "restartApp"
#define SYS_TOPIC_GETCONTLIST      "getContainerInfoList"
#define SYS_TOPIC_GETAPPINFOLIST   "getAppInfoList"
#define SYS_TOPIC_GETAPPID         "getAppId"
#define SYS_TOPIC_GETAPPLIC        "getAppLic"
#define SYS_TOPIC_UPDAPPLIC        "updateAppLic"
#define SYS_TOPIC_SETTIME          "setTime"
#define SYS_TOPIC_ADDROUTE         "addRoute"
#define SYS_TOPIC_DELROUTE         "delRoute"
#define SYS_TOPIC_GETSNTP          "getSntpCfg"
#define SYS_TOPIC_SETSNTP          "setSntpCfg"
#define SYS_TOPIC_REBOOT           "reboot"
#define SYS_TOPIC_REBOOTEVENT      "toRebootEvent"
#define SYS_TOPIC_ETHSTATUSEVENT   "ethStatusEvent"
#define SYS_TOPIC_ETHCFGEVENT      "ethCfgEvent"
#define SYS_TOPIC_POWERSTOPEVENT   "powerStopEvent"
#define SYS_TOPIC_POWERONEVENT     "powerOnEvent"
#define SYS_TOPIC_TIMEEVENT        "setTimeEvent"
#define SYS_TOPIC_INSTLL_CON       "installContainer"
#define SYS_TOPIC_INSTLL_APP       "installAPP"
#define SYS_TOPIC_UPDATE_APP       "updateAPP"
#define SYS_TOPIC_OPERATE_APP      "OperatingAPP"
#define SYS_TOPIC_SET_APPCFG       "setAppConfig"


/* 串口服务APP */
#define UART_APP_NAME              "uartManager"
#define UART_TOPIC_MODECHG         "modeChange"
#define UART_TOPIC_UARTINFO        "uartInfo"  
#define UART_TOPIC_ONLYPARAM       "commParam"
#define UART_TOPIC_ONLYREG         "onlyReg"
#define UART_TOPIC_MONITORREG      "appRegister"
#define UART_TOPIC_DATA            "data"
#define UART_TOPIC_WORKMODE        "workMode"
#define UART_TOPIC_ABNORMAL        "abnormal"

/* 蓝牙APP */
#define BLE_APP_NAME               "btManager"
#define BLE_TOPOC_SETPARAM         "paramSet"
#define BLE_TOPOC_GETPARAM         "paramGet"
#define BLE_TOPOC_SETPORT          "portSet"
#define BLE_TOPOC_PORTNOTIFY       "portNotice"
#define BLE_TOPOC_SETDETECT        "detectSet"
#define BLE_TOPOC_CONNINFO         "connectInfo"
#define BLE_TOPOC_SENDDATA         "sendData"
#define BLE_TOPOC_FORWARD          "forward"
#define BLE_TOPOC_RECVNOTIFY       "frameNotice"

#define MESSAGE_APPNAME_LEN        48
#define MESSAGE_OPERINFO_LEN       16
#define MESSAGE_INFOPATH_LEN       96
#define MESSAGE_TIME_LEN           36

/*物联管理*/
#define IOT_APP_NAME              "iotManager"
#define TRDATA_APP_NAME            "MQTTTrData"
#define IOT_TOPIC_STATUS           "terminalStatus"
#define IOT_TOPIC_DATA             "terminalData"
#define IOT_TOPIC_CMD              "terminalCmd"

#define MESSAGE_APPNAME_LEN        48
#define MESSAGE_OPERINFO_LEN       16
#define MESSAGE_INFOPATH_LEN       96
#define MESSAGE_TIME_LEN           36

/* 收到的消息(包括主题)解析成如下格式 */
typedef struct tagMessageJson
{
    char   sourapp[MESSAGE_APPNAME_LEN];
    char   destapp[MESSAGE_APPNAME_LEN];

    char   operat[MESSAGE_OPERINFO_LEN];   //操作类型
    char   infotype[MESSAGE_OPERINFO_LEN]; //命令类型

    char   infopath[MESSAGE_INFOPATH_LEN]; //接口名称

    uint32 token;
    char   timestamp[MESSAGE_TIME_LEN];

    void  *pObject; /* JSON解析结果 */
}MESSAGE_JSON_T;


#define MESSAGE_TYPE_A_XDR         0
#define MESSAGE_TYPE_JSON          1

/* MQTT消息格式，对应的内存释放接口appmsg_mqtt_free() */
typedef struct tagMessageMqtt
{
    uint8  msgtype;
    uint8  res[3];

    /* 类型msgtype决定 */
    union{
           MESSAGE_INFO_T tAXDR; //MESSAGE_TYPE_A_XDR
           MESSAGE_JSON_T tJson; //MESSAGE_TYPE_JSON
    };
}MESSAGE_MQTT_T;

/* 以太网 */
#define NETWORK_TYPE_ETH            0
/* 无线公网 */
#define NETWORK_TYPE_WIRE_PUBLIC    1
/* 无线专网 */
#define NETWORK_TYPE_WIRE_PRIVATE   2

#define NETWORK_MODE_NULL           0
#define NETWORK_MODE_2G             1
#define NETWORK_MODE_3G             2
#define NETWORK_MODE_4G             3
#define NETWORK_MODE_5G             4

#ifdef __cplusplus
}
#endif

#endif /* __MESSAGE_H__ */
