#ifndef _DESK_METER_H_
#define _DESK_METER_H_

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_METER_NUM   2040   //档案最大个数
#define METER_PRTL_97  1  
#define METER_PRTL_07  2 
#define METER_PRTL_OOP  3  


//单个档案结构体 目前只需要信息点号 地址 规约
typedef struct
{
    uint16 pn;
    OOP_TSA_T tsa;
    uint8 prtl;
    OOP_OAD_U port;
    uint8 searchFlag;       //搜表用 记录下是否在搜表结果里
    uint16 PT;                /**< PT         */
    uint16 CT;                /**< CT         */
}DESK_METER_T;

typedef enum
{
    DESK_METER_INIT = 0,//初始态
    DESK_METER_CHANGE,  //收到档案变更事件
    DESK_METER_OK,  //已加载完成
}DESK_METER_STATUS_E;
typedef struct
{
    DESK_METER_STATUS_E meterStatus;    //档案加载状态
    uint16 meterNum;                    //档案个数
    DESK_METER_T meterInfo[MAX_METER_NUM];  //档案信息
}DESK_METER_LIST_T;

#define MAX_DBUPDATE_QUEUE_CNT 2000
typedef struct
{
    uint16 infonum;     //更新的信息点号
    OOP_OAD_U oad;      //更新的数据标识
}DB_UPDATE_T;                   //数据中心上报数据变更事件后发送给线程的结构体

typedef struct
{
    uint8   isvalid;                           //是否有效
    DB_UPDATE_T   dbupdate;     //队列存储缓冲区
}DB_UPDATE_T_T;
//锁共享缓冲区结构
typedef struct
{
    pthread_mutex_t QueueLock;                  //接收缓冲区互斥锁
    int16  QueueHead;                          //队列头
    int16  QueueTail;                          //队列尾
    DB_UPDATE_T_T Queue[MAX_DBUPDATE_QUEUE_CNT];               //通讯数据队列
} DBUPDATE_QUEUE_T;


//线程使用的变量
typedef struct
{
    DB_CLIENT clientid;

}METER_THREAD_ARG_T;

void meter_thread_param_init();
void* meter_thread(void *arg);
void mqtt_db_update_fun(MESSAGE_INFO_T *ptMsg);
int meter_info_read_from_pn(uint16 pn,DESK_METER_T *meterInfo);
int meter_info_read_all(DESK_METER_LIST_T *meterList);
uint16 hplc_info_read_from_meter();

#ifdef __cplusplus
}
#endif

#endif

