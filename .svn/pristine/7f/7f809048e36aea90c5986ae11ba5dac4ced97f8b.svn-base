/*
*********************************************************************
* Copyright(C) 2024 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   负荷控制数据
* @date：    2024-06-27
* @history： 
*********************************************************************
*/
#ifndef __LOADCTRLLER_DATA_H__
#define __LOADCTRLLER_DATA_H__

#ifdef __cplusplus
    extern "C" {
#endif

typedef enum CTRL_TYPE
{
    CTRL_DOWN_TYPE          = 0, /* 当前功率下浮控 */
    CTRL_STOP_TYPE          = 1, /* 营业报停控 */
    CTRL_REST_TYPE          = 2, /* 厂休控 */
    CTRL_PERIOD_TYPE        = 3, /* 时段功控 */
    CTRL_BUY_TYPE           = 4, /* 购电控 */
    CTRL_MONTH_TYPE         = 5, /* 月电控 */
    CTRL_REMOTE_TYPE        = 6, /* 遥控分闸 */
    CTRL_CLOSE_TYPE         = 7, /* 遥控合闸 */
    CTRL_PROTCET_TYPE       = 8, /* 保电 */
    CTRL_CALLFEE_TYPE       = 9, /* 催费告警 */

    CTRL_POWER_TYPE         = 0xf1, /* 功控 */
    CTRL_ELEC_TYPE          = 0xf2, /* 电控 */
    CTRL_STATUS_TYPE        = 0xf3, /* 控制状态 */
    CTRL_TIME_TYPE          = 0xf4, /* 时间 */
    CTRL_PULSE_TYPE         = 0xf5, /* 脉冲 */
    CTRL_NULL_TYPE          = 0xff,
}CTRL_TYPE_E;

#define LCTRL_DB_QUEUE_SIZE       200
typedef struct tagLcDBChgDATA
{
    pthread_mutex_t    flock;
    uint8              bInitArg;
    uint8              bInitData;
    uint16             num;
    uint32             OAD[LCTRL_DB_QUEUE_SIZE];
    uint16             infonum[LCTRL_DB_QUEUE_SIZE];
}LCTRL_DBCHG_T;

typedef struct tagLoadCtrllerDATA
{
    uint32          mqttip;       //mqtt地址
    uint16          mqttport;     //mqtt端口
    uint8           hasDB;
    uint8           appidlen;
    uint8           appid[24];

    uint8           hasMap;       //存在模组管理器
    uint8           turnMax;      //最大轮次
    uint8           modeturn;     //模组轮次
    uint8           ctrlMode;     //控制模组序号

    uint8           protectStatus;//保电状态
    uint8           hasMaster;    //连接主站状态
    
    /* 总加组控制数据（功率、电量）统计完成后直接写入, 不用等数据更新 */
    int64           actPower[OOP_MAX_TG];   //总加组有功功率
    int64           aveActPower[OOP_MAX_TG];//总加组滑差时间内平均有功功率
    int64           monActEn[OOP_MAX_TG];   //总加组月有功电量

    /* 数据变更通知(OAD、初始化) */
    LCTRL_DBCHG_T   statDbChg;
    LCTRL_DBCHG_T   ctrlDbChg;
}LCTRL_DATA_T;


/* 对外同步接口 */
int lctrl_send_recv_appmsg(uint8 frameno, uint16 IID, uint16 IOP, uint8 *pSendMsg, uint16 sendlen, 
                                      uint8 *pRecvbuf, uint16 buflen, uint16 *pRecvlen, uint8 timeout);

void lctrl_set_ctrl_mode(uint8 port, uint8 subport);

int lctrl_write_oad_notify(LCTRL_DBCHG_T *pDbChg, uint32 Oad, uint16 infoNum);
uint32 lctrl_read_oad_notify(LCTRL_DBCHG_T *pDbChg, uint16 *infonum);
void lctrl_set_dataflag(LCTRL_DBCHG_T *pDbChg);
uint8 lctrl_get_dataflag(LCTRL_DBCHG_T *pDbChg);
void lctrl_set_argflag(LCTRL_DBCHG_T *pDbChg);
uint8 lctrl_get_argflag(LCTRL_DBCHG_T *pDbChg);

int lctrl_write_ctrl(uint8 type, uint8 *data, uint32 datalen);
int lctrl_read_ctrl(uint8 type, uint8 *databuf, uint32 buflen, uint32 *readlen);

void guard_set_lock_init(void);
uint8 get_guard_state(void);
int set_guard_state(uint8 guardState);
void  remote_set_lock_init(void);
uint8 get_remote_state(uint8 turn, uint8 *type, TELECON_TRIP *pTrip, TELECON_CLOSE *pClose);
void  set_remote_state_invalid(uint8 turn);
void set_remote_state_valid(uint8 turn, uint8 type, TELECON_TRIP *pTrip, TELECON_CLOSE *pClose);
void fee_set_lock_init(void);
uint8 get_fee_state(OOP_CALLFEE_T *pFee);
int set_fee_state(uint8 state, OOP_CALLFEE_T *pFee);

int lctrl_send_deskmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
uint8 lctrl_check_master_connect();

#ifdef __cplusplus
}
#endif

#endif //__LA_DATA_H__

