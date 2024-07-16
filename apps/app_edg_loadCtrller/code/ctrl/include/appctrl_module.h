/*
*********************************************************************
* @filename: appctrl_module.h
* @brief   : 安全软算法模块
* @describe: 
* @history : 
*********************************************************************
*/

#ifndef __APP_CTRL_MODULE_H
#define __APP_CTRL_MODULE_H

#include "task_msg.h"
#include "appctrl_data.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CTRL_DEV_NUM_MAX      128
#define CTRL_DEV_NUM_MAX_PORT (CTRL_DEV_NUM_MAX/CTRL_PORT_NUM)

//可能会接控制模块的端口数量，未来不同产品会有变化
#define CTRL_PORT_NUM    2

//传输失败最大次数
#define FAIL_NUM_MAX     3 

//传输超时时间
#define CTRL_TIME_OUT    5 

//遥信状态
typedef struct tagCTRL_YX_STATE
{
    uint32               num;
    uint32               sec[CTRL_DEV_NUM_MAX_PORT]; //上次更新时间
    OOP_TSA_T            tsa[CTRL_DEV_NUM_MAX_PORT];
    OOP_SWITCHIN_STATE_T  st[CTRL_DEV_NUM_MAX_PORT];
    OOP_SWITCHIN_FLAG_T   fg[CTRL_DEV_NUM_MAX_PORT];
}CTRL_YX_STATE_T;

typedef struct tagCTRL_YX_STATE_LIST
{
    pthread_mutex_t      f_lock;
    CTRL_YX_STATE_T      yxStat[CTRL_PORT_NUM];
}CTRL_YX_STATE_LIST_T;

#if 0
//遥信配置
typedef struct tagCTRL_YX_FLAG
{
    pthread_mutex_t      f_lock;
    uint32               num;
    BOOL                 isValid[CTRL_DEV_NUM_MAX];
    OOP_TSA_T            tsa[CTRL_DEV_NUM_MAX];
    OOP_SWITCHIN_FLAG_T  fg[CTRL_DEV_NUM_MAX];
}CTRL_YX_FLAG_T;
#endif

typedef enum
{
    IDLE_CTRL_STATE = 0,
    SEND_YX_STATE,
    RECV_YX_STATE,
    SEND_CT_STATE,
    RECV_CT_STATE,
    SEND_CN_STATE,
    RECV_CN_STATE,
}CTRL_STATE_E;

typedef enum
{
    CNT_IDLE = 0,
    CNT_GET_ID = 1,
    CNT_GET_IK,
    CNT_SET_ID_IK,
    CNT_ACT_INIT,
    CNT_GET_KEY,
    CNT_UPDATE,
    CNT_BEGIN_NEG,
    CNT_END_NEG,
    CNT_SUCCESS,
}CTRL_CNT_E;

/* 连接处理 */
typedef struct tagCTRL_CNT
{
    OOP_TSA_T    lastTsa;     //上一次连接的模块地址，如果切换了模块，需要重新连接
    OOP_TSA_T    curTsa;      //要重新连接的模块
    CTRL_CNT_E   cntState;    //协商状态
    uint32       idlen;       //协商设备ID长度
    uint8        devid[64];   //协商设备ID
    uint32       iklen;       //协商设备IK长度
    uint8        ik[64];      //协商设备IK
    uint8        modKeyStatus;//模块密钥状态
    uint32       cRanCat_len; //来自扩展模块所返回的 48 字节密文数据长度
    uint8        cRanCat[64]; //来自扩展模块所返回的 48 字节密文数据
    OOP_OAD_U    portOAD;     //重要操作报文端口号
    uint8        port;        //重要操作报文端口索引
    uint8        baud;        //重要操作报文波特率
    uint32       apdulen;     //重要操作报文长度
    uint8       *apdu;        //重要操作报文
}CTRL_CNT_T;

/* 端口状态 */
typedef struct tagCTRL_PORT_STATE
{
    uint32       second;      //系统运行秒数
    uint32       pn;          //控制模块索引
    CTRL_STATE_E state;       //端口状态
    uint8        failCnt;     //失败次数
}CTRL_PORT_STATE_T;

/* 设备线程数据 */
typedef struct tagCTRLDevHandle
{
    uint8             *pRunFlag;
    CTRL_QUEUE_T      *pApduSend;            //队列
    CTRL_QUEUE_T      *pApduRecv;            //队列
    CTRL_PORT_STATE_T *pState;
    CTRL_CNT_T        *connect;              //用于连接处理
}CTRL_DEV_H_T;

/* 端口配置 */
typedef struct tagCTRL_PORT_CFG
{
    uint16       port;     //序号
    OOP_OAD_U    portOAD;  //端口号
}CTRL_PORT_CFG_T;

/* 档案数据缓存结构*/
typedef struct tagCTRL_METER_LIST
{
    pthread_mutex_t f_lock;                                        //锁 多线程访问
    uint16          Num[CTRL_PORT_NUM];                            //个数
    OOP_METER_T     oopMeter[CTRL_PORT_NUM][CTRL_DEV_NUM_MAX_PORT];//每个端口的档案
}CTRL_METER_LIST_T;

//端口传输状态
extern CTRL_PORT_STATE_T gState[CTRL_PORT_NUM];

//连接协商信息
extern CTRL_CNT_T gConnect;

//重要命令队列
extern CTRL_QUEUE_T  g_send;
extern CTRL_QUEUE_T  g_recv;

extern void app_module_meter_init(void);
extern BOOL app_meter_is_module(OOP_METER_T *oopMeter);
extern int app_module_meter_update(void);
extern int app_module_ctrl_msg_proc(CTRL_DEV_H_T *handle, TASK_MSG_t *pMsgInfo);
extern int app_module_yxstate_get(OOP_TSA_T tsa, OOP_SWITCHIN_STATE_T *st, OOP_SWITCHIN_FLAG_T *fg);

#ifdef __cplusplus
}
#endif

#endif

