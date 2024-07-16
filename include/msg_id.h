/*
*********************************************************************
* @filename: msg_id.h
* @brief   : 本地IPC UDP通信方式 task id 定义
* @describe: 32位 前16位是模块id  后16位是模块内task编号
* @history: 
*    auther      date          describe
*    王津剑      2019-12-23    create
*********************************************************************
*/
#ifndef    __MSG_ID_H
#define __MSG_ID_H

#define MSG_START    0x10000000

/* 通信模块 */
#define MSG_COMM_START                (MSG_START + 0x100)
#define MSG_COMM_APPAMIN              (MSG_COMM_START + 1)
#define MSG_COMM_GETDIAL              (MSG_COMM_START + 2)
#define MSG_COMM_DIALEVENT            (MSG_COMM_START + 3)
#define MSG_COMM_SETAPN               (MSG_COMM_START + 4)
#define MSG_COMM_GETETH               (MSG_COMM_START + 5)
#define MSG_COMM_ADDROUTE             (MSG_COMM_START + 6)
#define MSG_COMM_DELROUTE             (MSG_COMM_START + 7)
#define MSG_COMM_ETHEVENT             (MSG_COMM_START + 8)
#define MSG_COMM_GUISTATUS            (MSG_COMM_START + 9) //IOP: DESKGUI_IOP_FRESH_STATE
#define MSG_COMM_GUITOOLS             (MSG_COMM_START + 10)//IOP: DESKGUI_IOP_FRESH_TOOL
#define MSG_COMM_PROXYSEND            (MSG_COMM_START + 11) //IID:0x1776 IOP: 0x0011 协议数据发送
#define MSG_COMM_PROXYCONNECT         (MSG_COMM_START + 12) //IID:0x1776 IOP: 0x0012 通信代理与主站物理连接通知
#define MSG_COMM_GET_MODULEINFO       (MSG_COMM_START + 13) //IOP: WIRELESS_IOP_GET_MODULEINFO
#define MSG_COMM_ETHCHANGEIP          (MSG_COMM_START + 14) //IOP: SYSTEM_IOP_EVENT_IPCHANGE
#define MSG_COMM_MAPSLOT              (MSG_COMM_START + 15) //IOP: MODULE_IOP_GET_PORTINFO
#define MSG_COMM_MAPINFO              (MSG_COMM_START + 16) //IOP: MODULE_IOP_GET_INFO
#define MSG_COMM_MAPEVENT             (MSG_COMM_START + 17) //IOP: MODULE_IOP_STATUS_EVENT
#define MSG_COMM_REDIAG               (MSG_COMM_START + 18) //IOP: WIRELESS_IOP_REDIAG
#define MSG_COMM_SMSNO                (MSG_COMM_START + 19) //IOP: WIRELESS_IOP_SET_SMSNO
#define MSG_COMM_AUTH                 (MSG_COMM_START + 20) //IOP: WIRELESS_IOP_SET_AUTH
#define MSG_COMM_SMSSEND              (MSG_COMM_START + 21) //IOP: WIRELESS_IOP_SEND_SMS
#define MSG_COMM_SMSRECV              (MSG_COMM_START + 22) //IOP: WIRELESS_IOP_REVC_SMS
#define MSG_COMM_POWER_RESTART        (MSG_COMM_START + 23) //IOP: WIRELESS_IOP_POWER_RESTART
#define MSG_COMM_GETUART              (MSG_COMM_START + 24) //查询串口APP是否存在及其独占的APP(异步)
#define MSG_COMM_SETUART              (MSG_COMM_START + 25) //设置串口参数(异步)
#define MSG_COMM_REGAPP               (MSG_COMM_START + 26) //从模式注册(异步)
#define MSG_COMM_UARTEVENT            (MSG_COMM_START + 27) //串口通知(被动)
#define MSG_COMM_SENDUART             (MSG_COMM_START + 28) //串口发送(同步)
#define MSG_COMM_RECVUART             (MSG_COMM_START + 29) //串口接收(被动)
#define MSG_COMM_UART_MODESET         (MSG_COMM_START + 30) //串口模式设置(同步)
#define MSG_COMM_GETBTCFG             (MSG_COMM_START + 31) //蓝牙参数查询(异步)
#define MSG_COMM_SETBTCFG             (MSG_COMM_START + 32) //蓝牙参数设置(异步)
#define MSG_COMM_GETBTPORT            (MSG_COMM_START + 33) //蓝牙端口查询(异步)
#define MSG_COMM_SETBTPORT            (MSG_COMM_START + 34) //蓝牙端口设置(异步)
#define MSG_COMM_BTRECVMSG            (MSG_COMM_START + 35) //蓝牙报文通知(被动)
#define MSG_COMM_BTPORTEVENT          (MSG_COMM_START + 36) //蓝牙端口通知(被动)
#define MSG_COMM_BTSENDMSG            (MSG_COMM_START + 37) //蓝牙发送数据(同步)
#define MSG_COMM_GETREGINFO           (MSG_COMM_START + 38) //查询无线网络注册信息
#define MSG_COMM_GETMODULEINFO        (MSG_COMM_START + 39) //查询无线模块信息
#define MSG_COMM_DELAPN               (MSG_COMM_START + 40) //删除APN
#define MSG_COMM_NO_RECV_TIME         (MSG_COMM_START + 41) //无线网络数据闲置时长
#define MSG_COMM_APP_TRANS            (MSG_COMM_START + 42) //无线网络数据闲置时长


#define MSG_COMM_GETRADIOCFG          (MSG_COMM_START + 50) //电台参数查询(异步)
#define MSG_COMM_SETRADIOCFG          (MSG_COMM_START + 51) //电台参数设置(异步)
#define MSG_COMM_RADIORECVMSG         (MSG_COMM_START + 52) //电台报文通知(被动)
#define MSG_COMM_RADIOSENDMSG         (MSG_COMM_START + 53) //电台发送数据(同步)
#define MSG_COMM_RADIOSTATUS          (MSG_COMM_START + 54) //电台状态
#define MSG_COMM_IFINFO               (MSG_COMM_START + 55) //IOP: INTERFACE_IOP_FRESH_STATE
#define MSG_COMM_IFTOOLS              (MSG_COMM_START + 56) //IOP: INTERFACE_IOP_FRESH_TOOL


/* 消息管理模块 */
#define MSG_MINITOR_START             (MSG_START + 0x200)
#define MSG_MINITOR_DBUPDATE          (MSG_MINITOR_START + 1)
#define MSG_MINITOR_DBINIT            (MSG_MINITOR_START + 2)

/* 主调度模块 */
#define MSG_APPAMIN_START             (MSG_START + 0x300)
#define MSG_APPAMIN_CTS_READ_LIST     (MSG_APPAMIN_START + 1)  //IOP: 0x0010
#define MSG_APPAMIN_CTS_READ_ONE      (MSG_APPAMIN_START + 2)  //IOP: 0x0011
#define MSG_APPAMIN_CTS_SET_LIST      (MSG_APPAMIN_START + 3)  //IOP: 0x0012
#define MSG_APPAMIN_CTS_SETREAD_LIST  (MSG_APPAMIN_START + 4)  //IOP: 0x0013
#define MSG_APPAMIN_CTS_ACT_LIST      (MSG_APPAMIN_START + 5)  //IOP: 0x0014
#define MSG_APPAMIN_CTS_ACTREAD_LIST  (MSG_APPAMIN_START + 6)  //IOP: 0x0015
#define MSG_APPAMIN_CTS_TRANSPORT     (MSG_APPAMIN_START + 7)  //IOP: 0x0016
#define MSG_APPAMIN_CTS_CARRIRT_TRANS (MSG_APPAMIN_START + 8)  //IOP: 0x0020
#define MSG_APPAMIN_CTS_SEEK_METER    (MSG_APPAMIN_START + 9)  //IOP: 0x0021
#define MSG_APPAMIN_RCT               (MSG_APPAMIN_START + 10) //IOP: 0x0038
#define MSG_APPAMIN_REBOOT            (MSG_APPAMIN_START + 11) //IOP: 0x0031
#define MSG_APPAMIN_SET_IPADDR        (MSG_APPAMIN_START + 12) //IOP: 0x0035
#define MSG_APPAMIN_ETH_PORT_OPEN      (MSG_APPAMIN_START + 13) //IOP: 0x0001    //以太网远程端口开放监控
#define MSG_APPAMIN_ETH_PORT_CONN      (MSG_APPAMIN_START + 14) //IOP: 0x0002    //以太网外联信息监控
#define MSG_APPAMIN_KEY_PATH_CHG       (MSG_APPAMIN_START + 15) //IOP: 0x0003    //关键目录文件变更监控
#define MSG_APPAMIN_ETH_LOGIN_CONSOLE  (MSG_APPAMIN_START + 16) //IOP: 0x0004    //以太网登录控制台监控
#define MSG_APPAMIN_DANGER_OPREATE     (MSG_APPAMIN_START + 17) //IOP: 0x0005    //危险命令操作监控
#define MSG_APPAMIN_LOCAL_MAINTAIN     (MSG_APPAMIN_START + 18) //IOP: 0x0006    //本地维护信息监控
#define MSG_APPAMIN_SSH_PORT_AUTHORIZE (MSG_APPAMIN_START + 19) //IOP: 0x0010    //启动/关闭SSH服务
#define MSG_APPAMIN_APPLYSTATUS        (MSG_APPAMIN_START + 20) //IID:0x1776 IOP: 0x0013 应用连接状态通知
#define MSG_APPAMIN_GETPPPMODE         (MSG_APPAMIN_START + 21) //IOP: WIRELESS_IOP_GET_MODULEINFO
#define MSG_APPAMIN_GETPPPDIAG         (MSG_APPAMIN_START + 22) //IOP: WIRELESS_IOP_GET_DIAGINFO
#define MSG_APPAMIN_GETPPPGPS          (MSG_APPAMIN_START + 23) //IOP: WIRELESS_IOP_GET_GPS
#define MSG_APPAMIN_GETMAPINFO         (MSG_APPAMIN_START + 24) //IOP: MODULE_IOP_GET_INFO
#define MSG_APPAMIN_GETMAPALL          (MSG_APPAMIN_START + 25) //IOP: MODULE_IOP_GET_ALL
#define MSG_APPAMIN_RESTART            (MSG_APPAMIN_START + 26) //IOP: COMMON_IOP_SYSTEM_RESTART
#define MSG_APPAMIN_GETMAPSLOT         (MSG_APPAMIN_START + 27) //IOP: MODULE_IOP_GET_PORTINFO
#define MSG_APPAMIN_MOD_EVENT          (MSG_APPAMIN_START + 28) //IOP: MODULE_IOP_STATUS_EVENT
#define MSG_APPAMIN_module_update_cmd  (MSG_APPAMIN_START + 29) //IID:0x0007 IOP: 0x0012通知模组升级的消息对应于0004 0015
#define MSG_APPAMIN_ESAM_STATE         (MSG_APPAMIN_START + 30) //IID:0x0007 IOP: 0x0013通知显示ESAM密钥状态
#define MSG_APPAMIN_AC_PARAM_INIT      (MSG_APPAMIN_START + 31) //IID:0xFFF0 IOP: 0x0001内部自定义，交采模块参数初始化
#define MSG_APPAMIN_AC_DATA_INIT       (MSG_APPAMIN_START + 32) //IID:0xFFF0 IOP: 0x0002内部自定义，交采模块数据初始化
#define MSG_APPAMIN_AC_DEMAND_INIT     (MSG_APPAMIN_START + 33) //IID:0xFFF0 IOP: 0x0003内部自定义，交采模块需量初始化
#define MSG_APPAMIN_AC_SYSTEM_CMD      (MSG_APPAMIN_START + 34) //IID:0xFFF0 IOP: 0x0004内部自定义，执行system命令
#define MSG_APPAMIN_AC_RATE_SET        (MSG_APPAMIN_START + 35) //IID:0xFFF0 IOP: 0x0005内部自定义，交采模块费率设置
#define MSG_APPAMIN_GET_CONTAINERINFO  (MSG_APPAMIN_START + 36)
#define MSG_APPAMIN_GET_APPINFO        (MSG_APPAMIN_START + 37)
#define MSG_APPAMIN_SYSTEM_CMD         (MSG_APPAMIN_START + 38)
#define MSG_APPAMIN_IP_CHANGE          (MSG_APPAMIN_START + 39) //ip地址变更事件
#define MSG_APPAMIN_GET_DEVICEINFO     (MSG_APPAMIN_START + 40) //获取设备信息
#define MSG_APPAMIN_UDISK_AUTHORIZE    (MSG_APPAMIN_START + 41) //私有暂无IOP，普通U盘的非授权接入，加密U盘授权接入
#define MSG_APPAMIN_PORT485_AUTHORIZE  (MSG_APPAMIN_START + 42) //私有暂无IOP，485口非法接入
#define MSG_APPMAIN_MANAGER_APP        (MSG_APPAMIN_START + 43) //APP管理
#define MSG_APPMAIN_TRANSPORT          (MSG_APPAMIN_START + 44) //IOP: COMMON_IOP_TRANSPORT
#define MSG_APPMAIN_GETSINGLEINFO      (MSG_APPAMIN_START + 45) //IOP: WIRELESS_IOP_GET_SINGL
#define MSG_APPAMIN_TRANSTASK_RPT      (MSG_APPAMIN_START + 46) //IID: 1003 IOP: 0x0001  福建自定义消息，透明任务上报通知
#define MSG_APPAMIN_MTREVENT_RPT       (MSG_APPAMIN_START + 47) //IID: 0003 IOP: 0x0003  福建自定义消息，上报事件数据通知
#define MSG_APPAMIN_CTS_RPTSTATE       (MSG_APPAMIN_START + 48) //IID: 1003 IOP: 0x0030  福建自定义消息，更新透明任务采集状态
#define MSG_APPMAIN_GET_BTCFG          (MSG_APPAMIN_START + 49) //获取蓝牙参数
#define MSG_APPMAIN_GET_BTCONNECT      (MSG_APPAMIN_START + 50) //获取蓝牙连接信息
#define MSG_APPMAIN_SET_BTCFG          (MSG_APPAMIN_START + 51) //设置蓝牙参数
#define MSG_APPMAIN_SET_BTCHECK        (MSG_APPAMIN_START + 52) //蓝牙检表配置

//补充22版本新规范安全在线事件
#define MSG_APPAMIN_PASSWARD_CHANGED   (MSG_APPAMIN_START + 53) //IOP: 0x0007    //口令信息变更异常检测
#define MSG_APPAMIN_UART_PORT_LOGIN    (MSG_APPAMIN_START + 54) //IOP: 0x0008    //串口登录控制台检测
#define MSG_APPAMIN_SESSION_ABNORMAL   (MSG_APPAMIN_START + 55) //IOP: 0x0009    //流量会话异常监控
#define MSG_APPAMIN_BANDWIDTH_ABNORMAL (MSG_APPAMIN_START + 56) //IOP: 0x000a    //流量带宽异常监控
#define MSG_APPAMIN_PROCESS_ABNORMAL   (MSG_APPAMIN_START + 57) //IOP: 0x000b    //进程异常监控
#define MSG_APPAMIN_LOGIN_ABNORMAL     (MSG_APPAMIN_START + 58) //IOP: 0x000c    //用户登录异常监控
#define MSG_APPAMIN_FLOW_PARAM_SET     (MSG_APPAMIN_START + 59) //IOP: 0x0010    //配置流量超限参数
#define MSG_APPAMIN_FLOW_PARAM_GET     (MSG_APPAMIN_START + 60) //IOP: 0x0011    //配置流量超限参数
#define MSG_APPAMIN_TIME_OUT_SET       (MSG_APPAMIN_START + 61) //IOP: 0x0012    //配置长时间未关闭服务判断参数

#define MSG_APPAMIN_GET_CUR_DEVICEINFO     (MSG_APPAMIN_START + 62) //获取设备当前运行信息

//补充22版本物联管理事件
#define MSG_APPAMIN_IOT_EVT            (MSG_APPAMIN_START + 64) //物联管理事件消息
#define MSG_APPAMIN_INSTLL_CON         (MSG_APPAMIN_START + 65)
#define MSG_APPAMIN_UNINSTLL_CON       (MSG_APPAMIN_START + 66) 
#define MSG_APPMAIN_INSTALL_APP        (MSG_APPAMIN_START + 67) ////APP安装
#define MSG_APPAMIN_CONFIG_CON         (MSG_APPAMIN_START + 68)
#define MSG_APPAMIN_EXEC_CON           (MSG_APPAMIN_START + 69)
#define MSG_APPAMIN_EXEC_APP           (MSG_APPAMIN_START + 70)
#define MSG_APPAMIN_CONFIG_APP         (MSG_APPAMIN_START + 71)
#define MSG_APPAMIN_SETLISIENSE_APP    (MSG_APPAMIN_START + 72)
#define MSG_APPAMIN_LOGINFO_GET        (MSG_APPAMIN_START + 73)
#define MSG_APPAMIN_LOG_PACKAGE        (MSG_APPAMIN_START + 74)
#define MSG_APPAMIN_LOG_DELECT         (MSG_APPAMIN_START + 75)
#define MSG_APPAMIN_GET_LISIENSE       (MSG_APPAMIN_START + 76)
#define MSG_APPAMIN_UPDATE_SYS         (MSG_APPAMIN_START + 77)
#define MSG_APPAMIN_SYS_INPECT         (MSG_APPAMIN_START + 78)
#define MSG_APPAMIN_FILE_DELECT        (MSG_APPAMIN_START + 79)


#define MSG_APPAMIN_POWERUP_EVT        (MSG_APPAMIN_START + 80) //IID: 1003 IOP: 0x0031  福建自定义消息，终端上电事件消息
#define MSG_APPAMIN_SERVICE_START      (MSG_APPAMIN_START + 81) //IID: 0001 IOP: 0x0040  启动SSH或SFTP服务
#define MSG_APPAMIN_SERVICE_STOP       (MSG_APPAMIN_START + 82) //IID: 0001 IOP: 0x0041  关闭SSH或SFTP服务

//自定义获取蓝牙sn
#define MSG_APPMAIN_GET_BT_SN          (MSG_APPAMIN_START + 90) //获取蓝牙SN
#define MSG_APPMAIN_SET_BT_SN          (MSG_APPAMIN_START + 91) //设置蓝牙SN

//电台管理APP
#define MSG_APPMAIN_GET_RADIOCFG       (MSG_APPAMIN_START + 95) //获取电台参数
#define MSG_APPMAIN_SET_RADIOCFG       (MSG_APPAMIN_START + 96) //设置电台参数
#define MSG_APPMAIN_SET_RADIOSTATUS    (MSG_APPAMIN_START + 97) //获取电台状态
#define MSG_APPMAIN_LONG_SEND           (MSG_APPAMIN_START + 98)//电台长发
//环境监测app
#define MSG_APPMAIN_ENVMONITOR_EVENT    (MSG_APPAMIN_START + 99) //环境监测app 事件上报
//删除测量点数据
#define MSG_APPMAIN_DEL_PN_DATE    (MSG_APPAMIN_START + 100) //测量点数据删除
//设置电台地址和行政区码
#define MSG_APPMAIN_SET_RADIOADDR       (MSG_APPAMIN_START + 101) //设置电台地址
//电台版本
#define MSG_APPMAIN_GET_RADIOVER       (MSG_APPAMIN_START + 102) //获取电台版本
#define MSG_APPMAIN_SET_RADIOSPEAK    (MSG_APPAMIN_START + 103) //设置电台通话开关

/* 遥信脉冲模块 */
#define MSG_PULSE_START               (MSG_START + 0x400)
#define MSG_PULSE_MOD_GET_INFO        (MSG_PULSE_START + 1)  //IOP: 0x0010
#define MSG_PULSE_MOD_GET_PORT        (MSG_PULSE_START + 2)  //IOP: 0x0011
#define MSG_PULSE_MOD_GET_STATUS      (MSG_PULSE_START + 3)  //IOP: 0x0012
#define MSG_PULSE_MOD_EVENT           (MSG_PULSE_START + 4)  //IOP: 0x0001
#define MSG_PULSE_POWER_DOWN          (MSG_PULSE_START + 5)  //IOP: 0x0001 0x0001
#define MSG_PULSE_GET_RMT_ENENT       (MSG_PULSE_START + 6)  //IOP: RSPSAMPLE_IOP_RMT_EVENT
#define MSG_PULSE_GET_RMT             (MSG_PULSE_START + 7)  //IOP: RSPSAMPLE_IOP_RMT_STATE
#define MSG_PULSE_GET_PULSE           (MSG_PULSE_START + 8)  //IOP: RSPSAMPLE_IOP_PULSE_STATIC
#define MSG_PULSE_SET_RSP             (MSG_PULSE_START + 9)  //IOP: RSPSAMPLE_IOP_CONFIGURE
#define MSG_PULSE_SHAKE_TIME          (MSG_PULSE_START + 10)  //IOP: RSPSAMPLE_IOP_SHAKE_TIME
#define MSG_PULSE_POWER_UP            (MSG_PULSE_START + 11)  //IOP: 0x0004


/* 显示模块 */
#define MSG_GUI_START                  (MSG_START + 0x500)
#define MSG_GUI_SETIP                  (MSG_GUI_START + 1)    //IOP: 0x0010
#define MSG_GUI_REBOOT                  (MSG_GUI_START + 2)    //IOP: 0x0011
#define MSG_GUI_RTC                      (MSG_GUI_START + 3)    //IOP: 0x0012
#define MSG_GUI_CONNECT_EVENT          (MSG_GUI_START + 4)    //IOP: 0x0013
#define MSG_GUI_GET_CONNECT              (MSG_GUI_START + 5)    //IOP: 0x0013
#define MSG_GUI_CHANGE_EVENT          (MSG_GUI_START + 6)    //IOP: 0x0014
#define MSG_GUI_DIAG_EVENT              (MSG_GUI_START + 7)    //IOP: 0x0015
#define MSG_GUI_AT_EVENT              (MSG_GUI_START + 8)    //IOP: 0x0016
#define MSG_GUI_AT_SWITCH              (MSG_GUI_START + 9)    //IOP: 0x0017
#define MSG_GUI_COMM_GETDIAL          (MSG_GUI_START + 10)    //IOP: 0x0018

/* 控制模块 */
#define MSG_CTRL_START                  (MSG_START + 0x600)
#define MSG_CTRL_MOD_GET_INFO         (MSG_CTRL_START + 1)  //IOP: 0x0010
#define MSG_CTRL_MOD_GET_PORT         (MSG_CTRL_START + 2)  //IOP: 0x0011
#define MSG_CTRL_MOD_GET_STATUS       (MSG_CTRL_START + 3)  //IOP: 0x0012
#define MSG_CTRL_MOD_EVENT            (MSG_CTRL_START + 4)  //IOP: 0x0001
#define MSG_CTRL_GUI_STATUS           (MSG_CTRL_START + 5)  //IOP: DESKGUI_IOP_FRESH_STATE
#define MSG_CTRL_GUI_TOOL             (MSG_CTRL_START + 6)  //IOP: DESKGUI_IOP_FRESH_TOOL
#define MSG_CTRL_GUI_HINT             (MSG_CTRL_START + 7)  //IOP: DESKGUI_IOP_PUSH_HINT
#define MSG_CTRL_GUI_NOHINT           (MSG_CTRL_START + 8)  //IOP: DESKGUI_IOP_POP_HINT
#define MSG_CTRL_IF_HINT              (MSG_CTRL_START + 9)  //IOP: INTERFACE_IOP_PUSH_CTRL
#define MSG_CTRL_IF_NOHINT            (MSG_CTRL_START + 10) //IOP: INTERFACE_IOP_POP_CTRL
#define MSG_CTRL_DB_CHG               (MSG_CTRL_START + 11) //IOP: INTERFACE_IOP_UPDATE_DB
#define MSG_CTRL_IF_VOICE             (MSG_CTRL_START + 12) //IOP: INTERFACE_IOP_VOICE


/* 给index区分模块使用, 在index的高8位 */
#define MSG_PULSE_FLAG                0x0100
#define MSG_CTRL_FLAG                 0x0200
#define MSG_APPMAIN_FLAG              0x0400
#define MSG_COMM_FLAG                 0x0800

/* 回路巡检 */
#define MSG_LC_START                  (MSG_START + 0x700)
#define MSG_LC_MAP_EVENT              (MSG_LC_START + 1) //IOP: MODULE_IOP_STATUS_EVENT
#define MSG_LC_MAP_INFO               (MSG_LC_START + 2) //IOP: MODULE_IOP_GET_INFO
#define MSG_LC_MAP_SLOT               (MSG_LC_START + 3) //IOP: MODULE_IOP_GET_PORTINFO
#define MSG_LC_MOD_RESTART            (MSG_LC_START + 4) //IOP: MODULE_IOP_RESTART
#define MSG_LC_UART_FOWARD            (MSG_LC_START + 5) //IOP: UART_IOP_SEND_DATA

/* 台区状态管理 */
#define MSG_PDS_START                  (MSG_START + 0x800)
#define MSG_PDS_TOPOSTART              (MSG_PDS_START + 1) //拓扑启动

/* 串口服务管理 */
#define MSG_UART_START                  (MSG_START + 0x900)
#define MSG_UART_GET_INFO               (MSG_UART_START + 1) //IOP: UART_IOP_GET_INFO
#define MSG_UART_SET_PARAM              (MSG_UART_START + 2) //IOP: UART_IOP_SET_PARAM
#define MSG_UART_REG_APP                (MSG_UART_START + 3) //IOP: UART_IOP_SET_LISTEN
#define MSG_UART_SET_ABNORMAL           (MSG_UART_START + 4) //IOP: UART_IOP_SET_ABNORMAL
#define MSG_UART_DATA                   (MSG_UART_START + 5)    //串口数据

#endif

