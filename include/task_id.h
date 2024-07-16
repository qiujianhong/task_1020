/*
*********************************************************************
* @filename: task_id.h
* @brief   : 本地IPC UDP通信方式 task id 定义
* @describe: 32位 前20位是模块id  后12位是模块内task编号
             前16位为0留给内部模块使用
* @history: 
*    auther      date          describe
*    王津剑      2019-12-23    create
*********************************************************************
*/
#ifndef    __TASK_ID_H
#define __TASK_ID_H


/*-------------------------------------db分配来时-----------------------------------------*/
/* 用采698 db分配 */
#define DB_YC_START          0x1000
#define DB_YC_AGENT          (DB_YC_START + 1)  //用采agent
#define DB_GUI_AGENT         (DB_YC_START + 2)  //显示agent

#define YC_DB_CLIENT_START   (DB_YC_START + 0x100)
#define DB_CLIENT_APPMAIN    (YC_DB_CLIENT_START + 1)
#define DB_CLIENT_COMM       (YC_DB_CLIENT_START + 2)
#define DB_CLIENT_PULSE      (YC_DB_CLIENT_START + 3)
#define DB_CLIENT_ACPORT     (YC_DB_CLIENT_START + 4)
#define DB_CLIENT_STATIS     (YC_DB_CLIENT_START + 5)
#define DB_CLIENT_UPDATE     (YC_DB_CLIENT_START + 6)
#define DB_CLIENT_REPORT     (YC_DB_CLIENT_START + 7)
#define DB_CLIENT_EVENT      (YC_DB_CLIENT_START + 8)
#define DB_CLIENT_GUI        (YC_DB_CLIENT_START + 9)
#define DB_CLIENT_GUI_UPDATE (YC_DB_CLIENT_START + 10)
#define DB_CLIENT_CTRL       (YC_DB_CLIENT_START + 11)
#define DB_CLIENT_PUAMR_GUI  (YC_DB_CLIENT_START + 12)
#define DB_CLIENT_OTHERTIME  (YC_DB_CLIENT_START + 13)
#define DB_CLIENT_SMS0       (YC_DB_CLIENT_START + 14)
#define DB_CLIENT_SMS1       (YC_DB_CLIENT_START + 15)
#define DB_CLIENT_OUTMSG     (YC_DB_CLIENT_START + 16)
#define DB_CLIENT_LCMTRANS   (YC_DB_CLIENT_START + 17)
#define DB_CLIENT_COMM_ETH   (YC_DB_CLIENT_START + 18)
#define DB_CLIENT_COMM_PPP   (YC_DB_CLIENT_START + 19)
#define DB_CLIENT_COMM_BLE   (YC_DB_CLIENT_START + 20)
#define DB_CLIENT_COMM_RADIO (YC_DB_CLIENT_START + 21)
#define DB_CLIENT_PROCON     (YC_DB_CLIENT_START + 22)
#define DB_CLIENT_EM         (YC_DB_CLIENT_START + 23)
#define DB_CLIENT_GUI_METER  (YC_DB_CLIENT_START + 24)
#define DB_CLIENT_COMM_2     (YC_DB_CLIENT_START + 25)

#define YC_DB_CLIENT_CHANNEL (DB_YC_START + 0x200)   //分给通道的taskid


/* 配网104 db分配 */
#define DB_PD_START          0x2000
#define DB_PD_AGENT          (DB_PD_START + 1)  //配网agent

#define PD_DB_CLIENT_START      (DB_PD_START + 0x100)
#define DB_PD_CLIENT_UPDATE     (PD_DB_CLIENT_START + 1)
#define DB_PD_CLIENT_APP104     (PD_DB_CLIENT_START + 2)
#define DB_PD_CLIENT_COMM       (PD_DB_CLIENT_START + 3)
#define DB_PD_CLIENT_PULSE      (PD_DB_CLIENT_START + 4)
#define DB_PD_CLIENT_104FRAME   (PD_DB_CLIENT_START + 5)
#define DB_PD_CLIENT_GUI        (PD_DB_CLIENT_START + 6)
#define DB_PD_CLIENT_STAT       (PD_DB_CLIENT_START + 7) //监测统计线程句柄
#define DB_PD_CLIENT_EVENT      (PD_DB_CLIENT_START + 8) //配网事件使用

/* 交采app db分配 */
#define DB_AC_START          0x3000
#define DB_AC_AGENT          (DB_AC_START + 1)  //交采app agent

#define AC_DB_CLIENT_START   (DB_AC_AGENT + 0x100)
#define DB_AC_CLIENT_SAVE    (AC_DB_CLIENT_START + 1)
#define DB_AC_CLIENT_UDP     (AC_DB_CLIENT_START + 2)
#define DB_AC_CLIENT_UPDATE  (AC_DB_CLIENT_START + 3) //数据中心更新
#define DB_AC_CLIENT_RPT    (AC_DB_CLIENT_START + 4)


/* 回路巡检 */
#define DB_LC_START          0x4000
#define DB_LC_AGENT          (DB_LC_START + 1)  //回路巡检app agent
#define DB_LC_CLIENT_MONITOR (DB_LC_START + 2)
#define DB_LC_CLIENT_GUI     (DB_LC_START + 3)

//台区状态管理
#define DB_PDS_START        0X5000
#define DB_PDS_AGENT        (DB_PDS_START+1)
#define DB_PDS_CLIENT  (DB_PDS_START+2)
#define DB_PDS_CLIENT_DBUPDATE  (DB_PDS_START+3)
#define DB_PDS_CLIENT_COMMON    (DB_PDS_START+4)
#define DB_PDS_CLIENT_LINELOSS    (DB_PDS_START+5)

//终端自维护
#define DB_OAM_START         0X6000
#define DB_OAM_AGENT        (DB_OAM_START+1)
#define DB_OAM_CLIENT_COMM  (DB_OAM_START+2)  //通信模块

//智慧能源接口APP
#define DB_IF_START          0X6100
#define DB_IF_AGENT          (DB_IF_START+0)
#define DB_IF_CLIENT_REPORT  (DB_IF_START+1)  //上报
#define DB_IF_CLIENT_ETH     (DB_IF_START+2)  //通道处理
#define DB_IF_CLIENT_SPI     (DB_IF_START+3)  //通道处理

//智慧能源接口JSON
#define DB_JSON_START          0X6200
#define DB_JSON_AGENT          (DB_JSON_START+0)
#define DB_JSON_CLIENT_DBCHG   (DB_JSON_START+1)  //上报
#define DB_JSON_CLIENT_FRMAE0  (DB_JSON_START+2)  //通道处理
#define DB_JSON_CLIENT_FRMAE1  (DB_JSON_START+3)  //通道处理

//专变负荷控制JSON
#define DB_LOADCTRLLER_START          0X6300
#define DB_LOADCTRLLER_AGENT          (DB_LOADCTRLLER_START+0)
#define DB_LOADCTRLLER_CTRL           (DB_LOADCTRLLER_START+1)  //控制
#define DB_LOADCTRLLER_STAT           (DB_LOADCTRLLER_START+2)  //统计
#define DB_LOADCTRLLER_EVENT          (DB_LOADCTRLLER_START+3)  //数据
#define DB_LOADCTRLLER_GUI            (DB_LOADCTRLLER_START+4)  //显示

/*-------------------------------------db分配结束-----------------------------------------*/



/*-------------------------------------进程分配开始-----------------------------------------*/
/* 用采task分配 */
#define YC_PROCESS_START     0x10000 * 0x1000
#define MSG_MONITOR          (YC_PROCESS_START + 1)    //对外通信管理
#define TIMER_MONITOR        (YC_PROCESS_START + 2)    //定时器管理
#define UPDATE_DB            (YC_PROCESS_START + 3)    //数据中心更新
#define APPMAIN_IPC          (YC_PROCESS_START + 4)    //通信模块交互
#define APPMAIN_MSG          (YC_PROCESS_START + 5)    //APPmain对外通信出入口
#define COMM_IPC             (YC_PROCESS_START + 6)    //主站通信对内
#define COMM_MSG             (YC_PROCESS_START + 7)    //对外通信
#define PULSE_MSG            (YC_PROCESS_START + 8)    //对外通信
#define ACPORT_MSG           (YC_PROCESS_START + 9)    //对外通信
#define STATIS_MSG           (YC_PROCESS_START + 10)   //对外通信
#define PULSE_SYNCMSG        (YC_PROCESS_START + 11)   //对外通信
#define COMM_SYNCMSG         (YC_PROCESS_START + 12)   //对外通信
#define APPMAIN_SYNCMSG      (YC_PROCESS_START + 13)   //对外通信
#define UART_SYNCMSG         (YC_PROCESS_START + 14)   //对外通信
#define COMM_TIMER           (YC_PROCESS_START + 15)    //对外通信
#define TRAN_MSG             (YC_PROCESS_START + 16)    //对外通信


/* 配网104 task分配 */
#define PD_PROCESS_START     0x10010 * 0x1000
#define PD_MSG_MONITOR       (PD_PROCESS_START + 1)    //对外通信管理
#define PD_UPDATE_DB         (PD_PROCESS_START + 2)    //数据中心更新
#define PD_APP104_IPC        (PD_PROCESS_START + 3)    //主站交互通道
#define PD_APP104_MSG        (PD_PROCESS_START + 4)    //APP对外通信出入口
#define PD_APP104_SYNC       (PD_PROCESS_START + 5)    //APP对外同步通信出入口
#define PD_COMM_IPC          (PD_PROCESS_START + 6)    //主调度通信
#define PD_COMM_MSG          (PD_PROCESS_START + 7)    //对外通信
#define PD_PULSE_MSG         (PD_PROCESS_START + 8)    //对外通信
#define PD_UPDATE_FILES      (PD_PROCESS_START + 9)    //数据中心档案更新
#define PD_COMM_SYNCMSG      (PD_PROCESS_START + 10)   //对外通信


/* GUI task 分配 */
#define GUI_PROCESS_START     0x10020 * 0x1000
#define    GUI_MSG_MONITOR         (GUI_PROCESS_START + 1)
#define GUI_MSG                 (GUI_PROCESS_START + 2)
#define GUI_COMM_MSG         (GUI_PROCESS_START + 3)
#define GUI_SYNCMSG             (GUI_PROCESS_START + 4)

/* GUI task 分配 */
#define LC_PROCESS_START     0x10030 * 0x1000
#define LC_MSG_PROC          (LC_PROCESS_START + 1)
#define LC_MSG_TASK          (LC_PROCESS_START + 2)
#define LC_MSG_UPDATE        (LC_PROCESS_START + 3)
#define LC_MSG_WORK          (LC_PROCESS_START + 4)


/* pdsManager分配 */
#define PDS_PROCESS_START     0x10040 * 0x1000
#define PDS_MQTT_IPC        (PDS_PROCESS_START+1)
#define PDS_CALC_IPC        (PDS_PROCESS_START+2)

/*-------------------------------------进程分配结束-----------------------------------------*/

#endif

