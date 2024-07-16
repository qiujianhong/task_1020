
/*
*********************************************************************
* Copyright(C) 2021 China Electric Power Research Institute
* All rights reserved. 
* @brief：   分钟采集相关操作API接口
* @date：    2021/6/15
* @history： 
* @user details： 
1. 注册初始化
    min_gather_init            - 注册回调函数，启动文件生成线程
2. 设置文件路径和终端地址(发生变化需要及时再次调用)
   min_gather_set_file
3. 恢复APP重启之前定的冻结数据
   (1) min_gather_prepare(TRUE) -  初始化数据准备
   (2) min_gather_input         -  读取数据中心所有数据，调用此函数写入
   (3) min_gather_prepare(FALSE)-  初始化数据完成
4. 冻结功能打开关闭(APP开始工作之后打开)
   (1) min_gather_frozen_start - 打开
   (2) min_gather_input        - 写入数据中心的冻结数据调用此接口写入
   (3) min_gather_frozen_stop  - 关闭
5. 实时功能打开关闭(可以在APP工作之后打开)
   (1) min_gather_real_start(TRUE) - 打开实时数据功能打开(TRUE-开始生成全量数据)
   (2) min_gather_input            - 读取数据中心最新的实时数据，调用此函数写入
   (3) min_gather_real_start(FALSE)- 打开实时数据功能打开(FALSE-全量数据生成完成)
   (4) min_gather_input            - 写入数据中心的实时数据调用此接口写入
   (5) min_gather_real_stop        - 关闭实时数据功能
6. 回调函数添加数据单元接口
   (1) min_gather_insert_item      - 冻结数据添加数据单元
   (2) min_gather_insert_data      - 实时数据添加数据单元

补充说明:
    江苏需求SFTP统一配置参数，存放文件路径就在sftp连接到的根目录下
    端口：822
    用户名：admin
    密码：admin(密码简单，系统不支持)
(1) 新增端口
    修改/etc/ssh/sshd_conig, 增加一行，内容: Port 822
(2) 新增用户名
    chattr -i /etc/passwd /etc/shadow /etc/group /etc/gshadow
    useradd -d /home/admin -m admin
    chmod 755 -R /home/admin
    passwd admin
*********************************************************************
*/
#ifndef __MIN_GATHER_H
#define __MIN_GATHER_H

#include "cJSON.h"
#include "db_struct.h"

#ifdef __cplusplus
    extern "C" {
#endif

typedef enum
{
    MIN_GATHER_TYPE_REAL     = 0,
    MIN_GATHER_TYPE_MIN      = 0x5002,
    MIN_GATHER_TYPE_HOUR     = 0x5003,
    MIN_GATHER_TYPE_DAY      = 0x5004,
    MIN_GATHER_TYPE_MON      = 0x5006,
}MIN_GATHER_TYPE_E;

typedef enum
{
    MIN_GATHER_OAD_20000201     = 0x20000201, /* A相电压 uint16 换算:-1 */
    MIN_GATHER_OAD_20000202     = 0x20000202, /* B相电压 uint16 换算:-1 */
    MIN_GATHER_OAD_20000203     = 0x20000203, /* C相电压 uint16 换算:-1 */
    MIN_GATHER_OAD_20010201     = 0x20010201, /* A相电流 int32 换算:-3 */
    MIN_GATHER_OAD_20010202     = 0x20010202, /* B相电流 int32 换算:-3 */
    MIN_GATHER_OAD_20010203     = 0x20010203, /* C相电流 int32 换算:-3 */
    MIN_GATHER_OAD_20010400     = 0x20010400, /* 零线电流 int32 换算:-3 */
    MIN_GATHER_OAD_20010500     = 0x20010500, /* 剩余电流 int32 换算:-3 */
    MIN_GATHER_OAD_20040201     = 0x20040201, /* 总有功功率 int32 换算:-1 */
    MIN_GATHER_OAD_20040202     = 0x20040202, /* A相有功功率 int32 换算:-1 */
    MIN_GATHER_OAD_20040203     = 0x20040203, /* B相有功功率 int32 换算:-1 */
    MIN_GATHER_OAD_20040204     = 0x20040204, /* C相有功功率 int32 换算:-1 */
    MIN_GATHER_OAD_43140506     = 0x43140506, /* 室外温度 int16 换算:-1 */
    MIN_GATHER_OAD_43140505     = 0x43140505, /* 室内温度 int16 换算:-1 */
    MIN_GATHER_OAD_43140501     = 0x43140501, /* 空调设置温度 int16 换算:-1 */
    MIN_GATHER_OAD_43140400     = 0x43140400, /* 空调开关机状态 int8 */
    MIN_GATHER_OAD_43110502     = 0x43110502, /* 热水器加热温度 int16 换算:-1 */
    MIN_GATHER_OAD_43110400     = 0x43110400, /* 热水器开关机状态 int8 */
    MIN_GATHER_OAD_00100201     = 0x00100201, /* 正向有功总电能 uint32 换算:-2 */
    MIN_GATHER_OAD_00100203     = 0x00100203, /* 正向有功峰电能 uint32 换算:-2 */
    MIN_GATHER_OAD_00100205     = 0x00100205, /* 正向有功谷电能 uint32 换算:-2 */
    MIN_GATHER_OAD_00200201     = 0x00200201, /* 反向有功总电能 uint32 换算:-2 */
    MIN_GATHER_OAD_00200203     = 0x00200203, /* 反向有功峰电能 uint32 换算:-2 */
    MIN_GATHER_OAD_00200205     = 0x00200205, /* 反向有功谷电能 uint32 换算:-2 */
}MIN_GATHER_OAD_E;

/**
*********************************************************************
* @brief：     冻结数据处理函数
* @param[in]： int             cid       数据中心客户端
               uint16          type      类型(MIN_GATHER_TYPE_E)
               WRITE_RECORD_T *pData     写入数据
               uint32          intv      数据间隔
               void*           pGdate    采集数据
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
typedef int (*FROZEN_DATA_FUN)(int cid, uint16 type, WRITE_RECORD_T*, uint32, void*);

/* 处理函数 */
typedef struct tagMinGatherFun
{
    uint32           OAD;
    FROZEN_DATA_FUN  func;
}MIN_GATHER_FUN_T;

//int froen_real_00100200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
//{
//    OOP_ENERGY_T ENERGY_T;
//    
//    if(MIN_GATHER_TYPE_REAL == type)
//    {
//        memcpy(ENERGY_T,pData->inDataBuf,pData->inDatalen);
//        min_gather_insert_data(pGdate, MIN_GATHER_OAD_00100201, pData->colStoreTm, &ENERGY_T.nValue[0], sizeof(uint32));
//    }
//
//    else
//    {
//        cJSON *jData_00100201 = NULL;
//        cJSON *jData_00100202 = NULL;
//        jData_00100201 = cJSON_CreateNumber(0);
//        jData_00100202 = cJSON_CreateNumber(0);
//        min_gather_insert_item(pGdate, type, 0x00100201, pData->colStoreTm, pData->colEndTm, intv, jData_00100201);
//        min_gather_insert_item(pGdate, type, 0x00100201, pData->colStoreTm, pData->colEndTm, intv, jData_00100202);
//    }
//    return 0;
//}
//MIN_GATHER_FUN_T g_Frozen[]
//{
//    {0x00100200, froen_real_00100200},
//};
//uint32 g_FrozenNUm = sizeof(g_Frozen)/sizeof(MIN_GATHER_FUN_T);
//
//MIN_GATHER_FUN_T g_Real[]
//{
//    {0x00100200, froen_real_00100200},
//    {0x00200200, froen_real_00200200},
//    {0x20000200, froen_real_20000200},
//    {0x20010200, froen_real_20010200},
//    {0x20010400, froen_real_20010400},
//    {0x20010500, froen_real_20040200},
//    {0x20040200, froen_real_20040200},
//};
//uint32 g_RealNUm = sizeof(g_Real)/sizeof(MIN_GATHER_FUN_T);

/**
*********************************************************************
* @brief：     分钟采集初始化
* @param[in]： char *filedir             文件存储路径
               MIN_GATHER_FUN_T *frofun  冻结数据处理函数
               uint32 fronum             冻结数据处理函数数量
               MIN_GATHER_FUN_T *realfun 实时数据处理函数
               uint32 realnum            实时数据处理函数数量
               uint8  timemin            冻结时间(0点timemin分, 最大59)
               uint8  zipmin             压缩时间(0点zipmin压缩，zipmin > timemin, 0无效，最大60)
               
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int min_gather_init(MIN_GATHER_FUN_T *frofun, uint32 fronum, MIN_GATHER_FUN_T *realfun, uint32 realnum, uint8 timemin, 
                    uint8 zipmin);

/**
*********************************************************************
* @brief：     设置文件路径
* @param[in]： char *filedir             文件存储路径
               uint8 *addr               终端地址(6个字节)
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
void min_gather_set_file(char *filedir, uint8 *addr);

/**
*********************************************************************
* @brief：     分钟内采集数据整理(用于APP首次启动后, 从数据中心获取数据后，生成增量JSON数据)
* @param[in]： uint8 flag    1-开始(将数据中心所有数据读出，调用min_gather_input写入); 0-停止
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int min_gather_prepare(uint8 flag);

/**
*********************************************************************
* @brief：     分钟内采冻结开始
* @param[in]： 
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int min_gather_frozen_start();

/**
*********************************************************************
* @brief：     分钟内采冻结停止
* @param[in]： uint8 bstore   是否需要生成已经存在的内容
* @param[out]：
* @return：    
*********************************************************************
*/
void min_gather_frozen_stop(uint8 bstore);

/**
*********************************************************************
* @brief：     分钟采集实时数据采集开始
* @param[in]： uint8 bAllFlag     全量生成标记(TRUE-生成全量标记, FALSE-全量标记完成)
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int min_gather_real_start(uint8 bAllFlag);

/**
*********************************************************************
* @brief：     分钟采集实时数据采集结束
* @param[in]： 
* @param[out]：
* @return：    
*********************************************************************
*/
void min_gather_real_stop();

/**
*********************************************************************
* @brief：     分钟采集数据输入
* @param[in]： int cid                   数据中心客户端
               WRITE_RECORD_T *pData     写入数据
               uint32 intv               数据间隔(例如，15分钟采集就填写15)
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int min_gather_input(int cid, WRITE_RECORD_T *pData, uint32 intv);

/**
*********************************************************************
* @brief：     分钟采集增加冻结数据
* @param[in]： void *pGdate              采集数据
               uint16 type               类型(MIN_GATHER_TYPE_E)
               uint32 itemId             数据标识
               uint32 storeTm            存储时间
               uint32 succTm             成功时间
               uint32 intv               间隔
               cJSON *jsonData           单个数据
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int min_gather_insert_item(void *pGdate, uint16 type, uint32 itemId, uint32 storeTm, uint32 succTm, uint32 intv, 
                           cJSON *jsonData);

/**
*********************************************************************
* @brief：     分钟采集添加实时数据
* @param[in]： void *pGdate              采集数据
               uint32 type               数据标识(MIN_GATHER_OAD_E)
               uint32 storeTm            存储时间
               uint8 *data               数据内容
               uint16 datalen            数据长度
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int min_gather_insert_data(void *pGdate, uint32 type, uint32 storeTm, uint8 *data, uint16 datalen);

#ifdef __cplusplus
}
#endif

#endif

