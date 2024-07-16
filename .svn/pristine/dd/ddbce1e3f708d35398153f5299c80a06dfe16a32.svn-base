/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：提供数据库访问接口
* @date： 2019.10.23
*********************************************************************
*/
#ifndef __STORAGE_H__
#define __STORAGE_H__

#include "../include/db_struct.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define MEMPRINT(x...) ({ if (getenv("DEBUG_STORAGE") != NULL)  printf(x);})

/*
 ********************************************************************
 *                                                                    *
 * 1-存储相关数据类型定义                                              *
 *                                                                    *
 ********************************************************************
*/

/* 句柄命名 */
#define HUDP void*



//
// UDP相关接口
//
uint8 set_len_offset(uint16 eleLen, uint8 *indata);
uint16 get_len_offset(uint8 *indata,uint8 *bytes);
/**
*********************************************************************
* @brief：     创建UDP操作相关句柄
* @param[in]： IP      - 数据中心IP地址
               port    - 数据中心端口
               appName - app名称
* @param[out]：          
* @return：    >0      - 成功创建句柄
               =<0     - 错误
*********************************************************************
*/
HUDP db_handle_create(const char *appName, const char *IP, uint16 port);

/**
*********************************************************************
* @brief：     销毁UDP操作相关句柄
* @param[in]： hUdp    - 句柄
* @param[out]：          
* @return：    = 0     - 成功
               < 0     - 错误
*********************************************************************
*/
int db_handle_close(HUDP hUdp);



//
// 基本接口操作
//

/**
*********************************************************************
* @brief：     读普通数据
* @param[in]： readNomal - 普通数据刷选条件
* @param[out]：outData   - 数据项成功读取后的返回地址缓冲区
               len       - 数据项成功读取后的返回地址缓冲区长度 
               inMaxLen  - 传入buf最大长度，防止outData溢出             
* @return：    = 0       - 成功
               < 0       - 错误
*********************************************************************
*/
int nomal_data_read(HUDP hUdp, const NOMAL_OAD_T nomalOAD, uint32 inMaxLen, uint8 *outData, uint32 *outLen);

/**
*********************************************************************
* @brief：     写普通数据
* @param[in]： writeNomal- 普通数据刷选条件
               inData    - 写入的数据项内容
               len       - 写入的长度 
* @param[out]：NULL             
* @return：    = 0       - 成功
               < 0       - 错误
*********************************************************************
*/
int nomal_data_write(HUDP hUdp, const NOMAL_OAD_T nomalOAD, uint8 *inData, uint32 inLen);

/**
*********************************************************************
* @brief：     开始读记录型数据
* @param[in]： startGetRecord - 记录型数据刷选条件
* @param[out]：startRecOut    - 返回值              
* @return：    = 0            - 成功
               < 0            - 错误
*********************************************************************
*/
int record_data_start_read(HUDP hUdp, const READ_RECORD_T startReadRecord, uint32 *handle, uint32 *recNum);

/**
*********************************************************************
* @brief：     单步读记录型数据
* @param[in]： stepGetRecord  - 单步读记录数据
* @param[out]: stepGetRecord  - 返回值              
* @return：    = 0            - 成功
               < 0            - 错误
*********************************************************************
*/
int record_data_step_read(HUDP hUdp, uint32 handle, const STEP_READ_RECORD_T stepReadRecord, RECORD_UNIT_T *stepRecordOut);

/**
*********************************************************************
* @brief：     结束记录型数据读取
* @param[in]： handle       -句柄
* @param[out]：NULL
* @return：    = 0          - 成功
               < 0          - 错误
*********************************************************************
*/
int record_data_end_read(HUDP hUdp, uint32 handle);

/**
*********************************************************************
* @brief：     读最近的一条记录
* @param[in]： inReadRecord  - 筛选需要的数据
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordData - 数据记录
               outRecordLen  - 数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_data_read(HUDP hUdp, const READ_RECORD_T inReadRecord, uint32 maxBufLen, RECORD_UNIT_T *recordOut);

/**
*********************************************************************
* @brief：     写记录型数据
* @param[in]： inData  - 输入数据
* @param[out]：storeNo - 存储序号
* @return： 
*********************************************************************
*/
int record_data_write(HUDP hUdp, const WRITE_RECORD_T inData, uint32 *storeNo);

/**
*********************************************************************
* @brief：     更新记录数据
* @param[in]： data - 需要更新的数据
* @param[out]：NULL
* @return： 
*********************************************************************
*/
int record_data_update(HUDP hUdp, const UPDATE_RECORD_T data);

/**
*********************************************************************
* @brief：获取存储记录条数
* @param[in]：
* @param[out]：NULL
* @return： 
*********************************************************************
*/
int record_num_get(HUDP hUdp, GET_RECORD_NUM_T tRecord, uint32 *recNum);

/**
*********************************************************************
* @brief：获取存储记录深度
* @param[in]：
* @param[out]：NULL
* @return： 
*********************************************************************
*/
int record_depth_get(HUDP hUdp, OOP_ROAD_T road, uint32 *recNum);

/**
*********************************************************************
* @brief：数据初始化
* @param[in]：
* @param[out]：NULL
* @return： 
*********************************************************************
*/
int data_clear(HUDP hUdp, CLEAR_DATA_T tData);

/**
*********************************************************************
* @brief：指定数据初始化
* @param[in]： logicId - 逻辑设备号
               infoNum - 信息点
               road    - 主DI 次DI
* @param[out]：NULL
* @return：    =0      - 成功
               <0      - 错误
*********************************************************************
*/
int data_by_OAD_clear(HUDP hUdp, CLEAR_DATA_T tData);

/**
*********************************************************************
* @brief:      恢复出厂
* @param[in]： logicId - 逻辑设备号
* @param[out]：NULL
* @return：    =0      - 成功
               <0      - 错误
*********************************************************************
*/
int default_setting_recover(HUDP hUdp, uint8 logicId);



//
// 初始化操作
//

/**
*********************************************************************
* @brief：      存储子系统初始化. 应用程序在调用存储库其他函数前必须先调用本函数
* @param[in]：  NULL
* @param[out]： NULL
* @return：     < 0        - 错误
                = 0        - 成功
*********************************************************************
*/
int init_mem(void);

/**
*********************************************************************
* @brief： 清除参数区：删除所有测量点参数、终端功能参数, 删除终端
           运行参数文件中和系统基本正常运行无关的参数内容。
* @param[in]： NULL
* @param[out]：NULL
* @return：    < 0 - 错误
               = 0 - 成功
*********************************************************************
*/
int init_param_area(HUDP hUdp);

/**
*********************************************************************
* @brief： 清数据区(历史及当前时间段的日和月数据,
           实时数据, 私有类数据)
* @param[in]： NULL
* @param[out]：NULL
* @return：   < 0 - 错误
              = 0 - 成功
*********************************************************************
*/
int init_data_area(HUDP hUdp);


//
// 业务数据参数接口操作
//

/**
*********************************************************************
* @brief：读交互类参数, 普通的读取过程会先从ramdisk开始, 读取成功就直接返回；如果失败，再
          从nand flash的文件系统区中读取对应参数文件。对于重要的系统运行参数在前两种介质均
          读取失败后还会从eeprom中尝试读取参数。
* @param[in]： oad     - 参数项ID
               mtrNum  - 测量点号, TMN_ID代表终端
               para    - 返回缓冲区
               len     - 缓冲区长度
               inMaxLen- 传入buf最大长度，防止outData溢出 
* @param[out]：NULL
* @return：    < 0     - 错误
               >=0     - 参数项长度
*********************************************************************
*/
int read_xpara(HUDP hUdp, OOP_OAD_U oad, uint32 inMaxLen, char *para, uint32 *len, mtrnum_t mtrNum);

/**
*********************************************************************
* @brief：     写参数数据
* @param[in]： oad     - 参数项ID
               para    - 参数存放缓冲区
               mtrNum  - 信息点号, TMN_ID代表终端
               len     - 缓冲区长度限制
* @param[out]：NULL
* @return：    < 0     - 错误
               >=0     - 参数项长度
*********************************************************************
*/
int write_xpara(HUDP hUdp, OOP_OAD_U oad, const char *para, uint32 len, mtrnum_t mtrNum);

/**
*********************************************************************
* @brief：     读取测量点当日某种类型的数据项
* @param[in]： mtrNum  - 信息点
               oad     - 数据项ID
               buf     - 数据项成功读取后的返回地址缓冲区
               len     - 缓冲区长度
               inMaxLen- 传入buf最大长度，防止outData溢出 
* @param[out]：NULL
* @return：    >= 0    - 成功读取后的数据项字节数
               < 0     - 失败
*********************************************************************
*/
int read_xdata(HUDP hUdp, mtrnum_t mtrNum, OOP_OAD_U oad, uint32 inMaxLen, char *buf, uint32 *len);

/**
*********************************************************************
* @brief：      测量点当前业务数据写入接口(扩展版本)
* @param[in]：  oad     - 数据项id
                len     - 写入的字节数
                infoNum - 测量点号
                buf     - 返回的数据项内容
                len     - 缓冲区长度限制
* @param[out]： NULL
* @return：     < 0 : 错误
                >=0 : 返回的字节数
*********************************************************************
*/
int write_xdata(HUDP hUdp, mtrnum_t mtrNum, OOP_OAD_U oad, const char *buf, uint32 len);



//
// 私有类数据接口
//

/**
*********************************************************************
* @brief：     在文件系统数据区中注册一个私有类数据文件，即生成一个私有类数据文件.
* @param[in]： filename - 注册的私有类数据文件名称
*              appName - 注册app名称
* @param[out]：NULL
* @return：    <0       - 错误
*              =0       - 成功
*********************************************************************
*/
int reg_pdata_xram(const char *appName, const char *filename);


/**
*********************************************************************
* @brief：     读私有类数据文件
* @param[in]： filename  - 文件名
               offset    - 相对于数据内容的偏移量
               len       - 读取长度
               buf       - 返回的缓冲区
* @param[out]：NULL
* @return：    <0        - 错误
               >= 0      - 成功读取后的数据项字节数
*********************************************************************
*/
int read_pdata_xram(const char *filename, char *buf, unsigned offset, unsigned len);

/**
*********************************************************************
* @brief：     写私有类数据文件
* @param[in]： filename  - 文件名
               buf       - 输入缓冲区
               offset    - 相对于数据内容的偏移量
               len       - 返回的缓冲区
* @param[out]：NULL
* @return：    <0        - 错误
               >= 0      - 写入长度
*********************************************************************
*/
int write_pdata_xram(const char *filename, const char *buf, unsigned offset, unsigned len);

/**
*********************************************************************
* @brief：     清除数据文件中的所有数据，只保留文件头。
* @param[in]： filename - 私有类数据文件名(无路径)
* @param[out]：NULL
* @return：    <0       - 错误
               =0       - 成功
*********************************************************************
*/
int clear_pdata_xram(const char *filename);

/**
*********************************************************************
* @brief：     删除数据区中，filename指定的私有类文件.filename不
               需要带文件路径.
* @param[in]： filename - 私有类数据文件名(无路径)
* @param[out]：NULL
* @return：    <0       - 错误
               =0       - 成功
*********************************************************************
*/
int remove_pdata_xram(const char *filename);

/**
*********************************************************************
* @brief：     eeprom私有区注册函数。
* @param[in]： id   - 待注册的id编号
               len  - 注册字节长度
               base - 返回的eeprom起始地址
* @param[out]：NULL
* @return：    <0   - 错误
               =0   - 成功
*********************************************************************
*/
int reg_pdata_eeprom(unsigned id, unsigned len, uint32 *base);

/**
*********************************************************************
* @brief：     读eeprom私有类数据
* @param[in]： id      - 注册获得的私有类空间id
               base    - 注册空间基地址
               offset  - 注册空间内字节偏移
               len 
* @param[out]：NULL
* @return：    <0      - 错误
               >= 0    - 成功读取后的数据项字节数
*********************************************************************
*/
int read_pdata_eeprom(unsigned id, uint32 base, uint32 offset, char *buf, unsigned len);

/**
*********************************************************************
* @brief：     读eeprom私有类数据
* @param[in]： id     - 注册的id号
               base   - 注册空间的起始地址
               offset - 注册空间内偏移量
               buf    - 数据来源缓冲区
               len    - 写入字节长度
* @param[out]：NULL
* @return：    <0     - 错误
               >= 0   - 写入长度
*********************************************************************
*/
int write_pdata_eeprom(unsigned id, uint32 base, uint32 offset, const char *buf, unsigned len);

/**
*********************************************************************
* @brief：      清除Eeprom中私有类数据区的内容
* @param[in]：  base - 注册返回的基地址
                id   - 注册返回的id
* @param[out]： NULL
* @return：     <0   - 错误
                =0   - 成功
*********************************************************************
*/
int clear_pdata_eeprom(uint32 base, int id);



//
// 日志调试
//

/* 
    * 用来向filepath指向的日志文件写入一行日志，fmt可以
    * 像printf(fmt, ...)中的fmt一样支持转义字符和可变参数.
    * 返回0表示成功，负数表示失败.
    * 注意: 不要在fmt后加'\n'换行，writelog会自动处理换行.
*/

/**
*********************************************************************
* @brief       写日志
* @param[in]   filepath - 文件名称
               fmt      - 写文件内容
* @param[out]  
* @return      >= 0     - 成功读取后的数据项字节数
               < 0      - 失败 
*********************************************************************
*/
int writelog(const char *filepath, const char *fmt, ...);

/**
*********************************************************************
* @brief       写二进制日志
* @param[in]   filepath    - 文件名称
*              strbuf      - 注释字符串
*              strbuf_len  - 注释字符串长度
*              databuf     - 16进制内容
*              databuf_len - 16进制长度
* @param[out]  
* @return      >= 0        - 成功读取后的数据项字节数
               < 0         - 失败 
*********************************************************************
*/
int writelog_bin(const char *filepath, const char *strbuf ,unsigned int strbuf_len ,const char *databuf,unsigned int databuf_len);

/**
*********************************************************************
* @brief       压缩日志文件
* @param[in]   file_path    - 日志文件路径
               backup_count - 支持压缩备份的份数 
* @param[out]  
* @return      错误信息    
*********************************************************************
*/
int tar_log_backup(const char *filepath, int backup_count);



//
// EEPROM操作
//

/**
*********************************************************************
* @brief：     读EEPROM数据
* @param[in]： buf    - 数据项成功读取后的返回地址缓冲区
               offset - 偏移
               len    - 数据长度
* @param[out]：NULL
* @return：    >= 0   - 成功读取后的数据项字节数
               < 0    - 失败
*********************************************************************
*/
int read_from_eeprom(char *buf, uint32 offset, uint32 len);

/**
*********************************************************************
* @brief：     写EEPROM数据
* @param[in]： buf    - 数据缓存
               offset - 偏移
               len    - 数据长度
* @param[out]：NULL
* @return：    >= 0    - 成功写入后的数据项字节数
               < 0     - 失败
*********************************************************************
*/

/* 
8k 铁电空间使用分布:
================================================================================

 网卡相关设置
网卡1的Mac地址6B
      /\        重要参数                  交采AC            谐波HR     eeprom私有类数据
     /  \          |                    |               |           |
    /    \   ________|_______   __________|__________   __|__   ______|______
   /      \ /                \ /                     \ /     \ /             \
  |--------|--------^---------|-----------------------|-------|---------------|
      1    +   3k - 128Bytes            3k            + 256   +  2k-256Bytes
      2     \                                                     \
      8      \                                                  \
      B       \--XPARABASE                                        \--PDATABASE

================================================================================
*/

int write_to_eeprom(const char *buf, uint32 offset, uint32 len);


#ifdef __cplusplus
}
#endif
#endif //__STORAGE_H__
