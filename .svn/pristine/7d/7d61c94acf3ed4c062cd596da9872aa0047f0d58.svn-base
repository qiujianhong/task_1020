/*
*********************************************************************
* @filename: comm_gather.h
* @brief   : 分钟采集公共内部数据结构
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2021-6-15     create
*********************************************************************
*/
#ifndef __COMM_GATHER_H
#define __COMM_GATHER_H

#include "rbtree.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define MIN_GATHER_FROZEN_NAMR "frz_data_"
#define MIN_GATHER_REAL_NAMR   "realtime_data_"

typedef enum
{
    MIN_GATHER_SUCCESS      = 0,
    MIN_GATHER_ERR_INPUT    = 0x2100,
    MIN_GATHER_ERR_NOTSUPPORT,
    MIN_GATHER_ERR_NOTREG,
    MIN_GATHER_ERR_MALLOC,
    MIN_GATHER_ERR_JSON,
    MIN_GATHER_ERR_RBTREE,
    MIN_GATHER_ERR_FILE,
    MIN_GATHERR_ERR_UNKNOW,
}MIN_GATHER_ERRCODE_E;


/* 数据属性 */
typedef struct tagMinGatherFrozenItem
{
    uint32   id;
    uint16   type;
    uint16   intv;
    uint16   num;
    uint32   SuccessTime;
    uint32   StoreTime;
    cJSON   *jsonItem;
}MG_FROZEN_ITEM_T;

/* 表数据集合 */
typedef struct tagMinGatherFrozenMeter
{
    uint8             Addr[6];
    uint16            listNum;
    RBRoot           *listItem;
    cJSON            *jsonRoot;
}MG_FROZEN_METER_T;

/* 冻结集合 */
typedef struct tagMinGatherFrozenData
{
    uint32            filetime;
    OOP_DATETIME_S_T  fileday;
    RBRoot           *meterRoot;
}MG_FROZEN_T;

/* 实时集合 */
typedef struct tagMinGatherRealNode
{
    struct tagMinGatherRealNode *pNext;
    uint32            mintimes;
    OOP_DATETIME_S_T  min;
    uint8             dataflag[28];  /* 数据存在标识(一共24个) */
    uint8             Addr[6];
    uint16            data_20000201; /* A相电压 换算:-1 */
    uint16            data_20000202; /* B相电压 换算:-1 */
    uint16            data_20000203; /* C相电压 换算:-1 */
    
    int32             data_20010201; /* A相电流 换算:-3 */
    int32             data_20010202; /* B相电流 换算:-3 */
    int32             data_20010203; /* C相电流 换算:-3 */
    
    int32             data_20010400; /* 零线电流 换算:-3 */
    int32             data_20010500; /* 剩余电流 换算:-3 */
    
    int32             data_20040201; /* 总有功功率 换算:-1 */
    int32             data_20040202; /* A相有功功率 换算:-1 */
    int32             data_20040203; /* B相有功功率 换算:-1 */
    int32             data_20040204; /* C相有功功率 换算:-1 */

    int16             data_43140506; /* 室外温度 换算:-1 */
    int16             data_43140505; /* 室内温度 换算:-1 */
    int16             data_43140501; /* 空调设置温度 换算:-1 */
    uint8             data_43140400; /* 空调开关机状态 */
    uint8             data_43110400; /* 热水器开关机状态 */
    int16             data_43110502; /* 热水器加热温度 换算:-1 */

    uint8             res[2];

    uint32            data_00100201; /* 正向有功总电能 换算:-2 */
    uint32            data_00100203; /* 正向有功峰电能 换算:-2 */
    uint32            data_00100205; /* 正向有功谷电能 换算:-2 */
    uint32            data_00200201; /* 反向有功总电能 换算:-2 */
    uint32            data_00200203; /* 反向有功峰电能 换算:-2 */
    uint32            data_00200205; /* 反向有功谷电能 换算:-2 */
}MG_REAL_NODE_T;

typedef struct tagMinGatherRealData
{
    uint8           bFirst;
    uint8           res[3];
    MG_REAL_NODE_T *pAllData;
    MG_REAL_NODE_T *pNewData;
}MG_REAL_T;

void min_gather_unlock_file(void *arg);
void min_gather_time_t(time_t times, OOP_DATETIME_S_T *curTime);
time_t min_gather_time_s(OOP_DATETIME_S_T *dateTime);

MG_FROZEN_T *min_gather_create_frozen(time_t linuxTime);
int min_gather_create_meter(uint8 *mac, MG_FROZEN_METER_T *pMeter);
void min_gather_destory_frozen(MG_FROZEN_T *ptFrozen);
void min_gather_forzen_flag(char *filepath, uint8 flag);
uint8 min_gather_forzen_getflag(char *filepath, uint32 *times);
int min_gather_forzen_store(uint8 *addr, char *filepath, MG_FROZEN_T *ptFrozen);
uint32 min_gather_forzen_getstore(char *filepath);
int min_gather_forzen_zip(char *filepath, OOP_DATETIME_S_T *ziptime);
int min_gather_frozen_add(MG_FROZEN_METER_T *pGdate, uint16 type, uint32 itemId, uint32 storeTm, uint32 succTm, 
                          uint32 intv, cJSON *jsonData);

void min_gather_real_clear(MG_REAL_NODE_T *pDataNode);
void min_gather_destroy_allnode(MG_REAL_NODE_T **ppDataHead);
MG_REAL_NODE_T *min_gather_get_node(MG_REAL_NODE_T **ppDataHead, uint8 *Addr, OOP_DATETIME_S_T *datamin);
int min_gather_real_add(MG_REAL_NODE_T *pDataNode, uint32 type, uint32 storeTm, uint8 *data, uint16 datalen);
void min_gather_real_all(MG_REAL_NODE_T **ppAllHead, MG_REAL_NODE_T **ppNewHead);
int min_gather_real_store(uint8 *addr, char *filepath, MG_REAL_NODE_T *pDataList, OOP_DATETIME_S_T *storemin);
void min_gather_real_delete(uint8 *addr, char *filepath, uint8 hours);

#ifdef __cplusplus
}
#endif

#endif

