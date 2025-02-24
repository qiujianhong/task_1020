
/*
*********************************************************************
* Copyright(C) 2018 China Electric Power Research Institute
* All rights reserved. 
* @brief：   面向对象数据类型定义
* @date：    2018/09/10
* @history： 
*
*-------------------------------------------------------------------
*   2021/02/25
*       1. 修改回路巡检对象4040，与最新标准保持一致；
*       2. 增加线路拓扑数据结构；
*       3. 修改搜表事件的宏定义。
*
*   2021/01/28
*       1. 修改远程通信网络类型最大支持数量；
*       2. 还原遥控OOP_RCTRLLOG_T定义;
*       3. 增加回路巡检9003支持。
*
*   2021/01/26:
*       1. 修订OOP_MAX_OADS宏定义；
*       2. 修订OOP_RCTRLLOG_T定义；
*       3. 修订LCONSCHEME定义；
*       4. 修订OOP_CFGUNIT_8109_T定义；
*       5. 修订OOP_CFGUNIT_810D_T定义。
*
*   2020/09/01:
*       1. 修订TSA数据结构，与698.45协议保持一致
*-------------------------------------------------------------------
*   2020/07/31:
*       1. 完善回路巡检工况信息结构定义
*       2. 增加负荷辨识信息结构定义
*       3. 增加线损计算信息结构定义
*-------------------------------------------------------------------
*   2020/07/13: 郝长征(许继)
*       1. 优化回路巡检工况信息结构定义
*-------------------------------------------------------------------
*   2020/07/10: 张小秋(新联)
*       1. 增加32位bit-string定义
*-------------------------------------------------------------------
*   2020/07/09: 蒋长献(林洋)
*       1. 增加OOP_TSA_T关于地址域顺序的说明，见OOP_TSA_T定义；
*       2. 去除F210结构定义中与698.45协议不一致的字段
*
*********************************************************************
*/

#ifndef __OOP_TYPE_H__
#define __OOP_TYPE_H__

#include <time.h>
#include "dataType.h"
#include "macro.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

//#define STAT_FUN  1    //统计功能开关 主要统计抄表收发的效率 用于taskManager和ccoRouter

/*
 ********************************************************************
 *                                                                  *
 * 1-OOP协议库全局数据类型定义                                          *
 *                                                                  *
 ********************************************************************
*/

#define OOP_MODULE_NAME     "oop"       /**< 协议库模块名     */

/** @brief 标准数据类型枚举 */
typedef enum tag_OOP_DATATYPE
{
    DT_NULL                 = 0,        /**< NULL               */
    DT_ARRAY                = 1,        /**< vector             */
    DT_STRUCTURE            = 2,        /**< struct             */
    DT_BOOL                 = 3,        /**< BOOL               */
    DT_BIT_STRING           = 4,        /**< CBitString         */
    DT_DOUBLE_LONG          = 5,        /**< int32              */
    DT_DOUBLE_LONG_UNSIGNED = 6,        /**< uint32             */
    DT_OCTET_STRING         = 9,        /**< COctetString       */
    DT_VISIBLE_STRING       = 10,       /**< string             */
    DT_UTF8_STRING          = 12,       /**< string             */
    DT_INTEGER              = 15,       /**< char               */
    DT_LONG                 = 16,       /**< int16              */
    DT_UNSIGNED             = 17,       /**< uint8              */
    DT_LONG_UNSIGNED        = 18,       /**< uint16             */
    DT_LONG64               = 20,       /**< int64              */
    DT_LONG64_UNSIGNED      = 21,       /**< uint64             */
    DT_ENUM                 = 22,       /**< uint8              */
    DT_FLOAT32              = 23,       /**< float32            */
    DT_FLOAT64              = 24,       /**< float64            */
    DT_DATETIME             = 25,       /**< DateTime           */
    DT_DATE                 = 26,       /**< OOP_DATE_T         */
    DT_TIME                 = 27,       /**< OOP_TIME_T         */
    DT_DATETIME_S           = 28,       /**< DateTime_S         */
    DT_OI                   = 80,       /**< OI                 */
    DT_OAD                  = 81,       /**< OOP_OAD_U          */
    DT_ROAD                 = 82,       /**< OOP_ROAD_T         */
    DT_OMD                  = 83,       /**< OOP_OMD_U          */
    DT_TI                   = 84,       /**< 时间间隔           */
    DT_TSA                  = 85,       /**< 设备地址           */
    DT_MAC                  = 86,       /**< OOP_MAC_T签名        */
    DT_RN                   = 87,       /**< 随机数              */
    DT_REGION               = 88,       /**< 区间类型           */
    DT_SCALER_UNIT          = 89,       /**< 换算单位           */
    DT_RSD                  = 90,       /**< 行选择符           */
    DT_CSD                  = 91,       /**< 列选择符           */
    DT_MS                   = 92,       /**< 电能表集合            */
    DT_SID                  = 93,       /**< 安全标识           */
    DT_SIDMAC               = 94,       /**< 安全标识+OOP_MAC_T */
    DT_COMDCB               = 95,       /**< 串口通讯控制块      */
    DT_RCSD                 = 96,       /**< 记录列选择描述符   */  
    DT_VQDS                 = 97,       /**< 带品质的变体数据类型 */
    DT_CHECKSUM             = 98,       /**< 24规范校验码类型 */
    DT_UNKNOW               = 255       /**< 未知             */
}OOP_DATATYPE_E;

/** @brief 接口类枚举  */
typedef enum tag_OOP_CLASS
{
    OOC_ENERGY              = 1,        /**< 电能量类               */
    OOC_DEMAND              = 2,        /**< 最大需量类                */
    OOC_PHASEDATA           = 3,        /**< 分相变量类                */
    OOC_POWER               = 4,        /**< 功率类                  */
    OOC_HARMONIC            = 5,        /**< 谐波类                  */
    OOC_DATA                = 6,        /**< 数据变量类                */
    OOC_EVENT               = 7,        /**< 事件变量类                */
    OOC_PARAM               = 8,        /**< 参数变量类                */
    OOC_FROZEN              = 9,        /**< 冻结变量类                */
    OOC_COLLECT             = 10,       /**< 采集监控类                */
    OOC_ARRAY               = 11,       /**< 集合接口类                */
    OOC_PULSE               = 12,       /**< 主动上报接口类          */
    OOC_CONTROL             = 13,       /**< 控制接口类                */
    OOC_REGION              = 14,       /**< 区间超限统计类          */
    OOC_RECKON              = 15,       /**< 累加平均类                */
    OOC_EXTREME             = 16,       /**< 极值工具类                */
    OOC_DISPLAY             = 17,       /**< 显示接口类                */
    OOC_FTP                 = 18,       /**< FTP接口类               */
    OOC_DEVICEMANAGE        = 19,       /**< 设备管理接口类          */
    OOC_ASSOCIATION         = 20,       /**< 应用连接接口类          */
    OOC_ESAM                = 21,       /**< ESAM接口类              */
    OOC_IO                  = 22,       /**< 输入输出接口类          */
    OOC_TG                  = 23,       /**< 总加组接口类         */
    OOC_MULTIEVENT          = 24,       /**< 分项事件类                */
    OOC_GPRS                = 25,       /**< 无线公网接口类          */
    OOC_ETHERNET            = 26,       /**< 以太网接口类         */
    OOC_MAXNUM              = 27        /**< 最大接口类数量          */
}OOP_CLASS_E;

/** @brief 应用数据单元枚举 */
typedef enum tag_OOP_TAG
{
    OOP_LINK_REQUEST            = 1,        /**< 链路维护请求     */
    OOP_CONNECT_REQUEST         = 2,        /**< 应用连接请求     */
    OOP_RELEASE_REQUEST         = 3,        /**< 断开应用连接请求   */
    OOP_GET_REQUEST             = 5,        /**< 读取请求           */
    OOP_SET_REQUEST             = 6,        /**< 设置请求           */
    OOP_ACTION_REQUEST          = 7,        /**< 操作请求           */
    OOP_REPORT_RESPONSE         = 8,        /**< 主动上报应答     */
    OOP_PROXY_REQUEST           = 9,        /**< 代理请求           */
    OOP_SECURITY_REQUEST        = 16,       /**< 安全传输请求     */
    OOP_CLIENT_ERROR_RESPONSE   = 110,      /**< 客户端异常响应      */
    OOP_LINK_RESPONSE           = 129,      /**< 链路维护响应     */
    OOP_CONNECT_RESPONSE        = 130,      /**< 应用连接应答     */
    OOP_RELEASE_RESPONSE        = 131,      /**< 断开应用连接应答   */
    OOP_RELEASE_NOTIFICATION    = 132,      /**< 断开应用连接通知   */
    OOP_GET_RESPONSE            = 133,      /**< 读取应答           */
    OOP_SET_RESPONSE            = 134,      /**< 设置应答           */
    OOP_ACTION_RESPONSE         = 135,      /**< 操作应答           */
    OOP_REPORT_NOTIFICATION     = 136,      /**< 主动上报通知     */
    OOP_PROXY_RESPONSE          = 137,      /**< 代理应答           */
    OOP_SECURITY_RESPONSE       = 144,      /**< 安全传输应答     */
    OOP_SERVER_ERROR_RESPONSE   = 238       /**< 服务器端遗产响应   */
}OOP_TAG_E;

#define OOP_LINK_REQUEST_NAME           "链路维护请求"
#define OOP_CONNECT_REQUEST_NAME        "应用连接请求"
#define OOP_RELEASE_REQUEST_NAME        "断开应用连接请求"
#define OOP_GET_REQUEST_NAME            "读取请求"
#define OOP_SET_REQUEST_NAME            "设置请求"
#define OOP_ACTION_REQUEST_NAME         "操作请求"
#define OOP_REPORT_RESPONSE_NAME        "主动上报应答"
#define OOP_PROXY_REQUEST_NAME          "代理请求"
#define OOP_SECURITY_REQUEST_NAME       "安全传输请求"
#define OOP_LINK_RESPONSE_NAME          "链路维护响应"
#define OOP_CONNECT_RESPONSE_NAME       "应用连接应答"
#define OOP_RELEASE_RESPONSE_NAME       "断开应用连接应答"
#define OOP_RELEASE_NOTIFICATION_NAME   "断开应用连接通知"
#define OOP_GET_RESPONSE_NAME           "读取应答"
#define OOP_SET_RESPONSE_NAME           "设置应答"
#define OOP_ACTION_RESPONSE_NAME        "操作应答"
#define OOP_REPORT_NOTIFICATION_NAME    "主动上报通知"
#define OOP_PROXY_RESPONSE_NAME         "代理应答"
#define OOP_SECURITY_RESPONSE_NAME      "安全传输应答"

/** @brief 读取服务类型枚举 */
typedef enum tag_OOP_GET_TYPE
{
    OOP_GET_NORMAL              = 1,        /**< 读取一个对象属性                       */
    OOP_GET_NORMALLIST          = 2,        /**< 读取多个对象属性                       */
    OOP_GET_RECORD              = 3,        /**< 读取一个记录型对象属性                  */
    OOP_GET_RECORDLIST          = 4,        /**< 读取多个记录型对象属性                  */
    OOP_GET_NEXT                = 5,        /**< 读取分帧响应的下一个数据块                    */
    OOP_GET_MD5                 = 6,        /**< 读取一个对象属性的MD5                 */
    OOP_GET_SIMPLY              = 23        /**< 读取一个精简的记录型对象属性                 */
}OOP_GET_TYPE_E;

/** @brief 设置服务类型枚举 */
typedef enum tag_OOP_SET_TYPE
{
    OOP_SET_NORMAL              = 1,        /**< 设置一个对象属性                       */
    OOP_SET_LIST                = 2,        /**< 设置多个对象属性                       */
    OOP_SET_WITHGETLIST         = 3         /**< 设置后读取多个对象属性请求                    */
}OOP_SET_TYPE_E;

/** @brief 操作服务类型枚举 */
typedef enum tag_OOP_ACTION_TYPE
{
    OOP_ACTION_NORMAL           = 1,        /**< 操作一个对象属性                       */
    OOP_ACTION_LIST             = 2,        /**< 操作多个对象属性                       */
    OOP_ACTION_WITHGETLIST      = 3         /**< 操作后读取多个对象属性请求                    */
}OOP_ACTION_TYPE_E;

typedef uint16  OI;                     /**< 对象标识           */

/** @brief 数据缓冲区-128 byte */
typedef struct tag_OOP_BUF128
{
    int64   buf[128/sizeof(int64)];
}OOP_BUF128_T;

/** @brief 数据缓冲区-256 byte */
typedef struct tag_OOP_BUF256
{
    int64   buf[256/sizeof(int64)];
}OOP_BUF256_T;

/** @brief 数据缓冲区-512 byte */
typedef struct tag_OOP_BUF512
{
    int64   buf[512/sizeof(int64)];
}OOP_BUF512_T;

/** @brief 数据缓冲区-1K byte  */
typedef struct tag_OOP_BUF1K
{
    int64   buf[1024/sizeof(int64)];
}OOP_BUF1K_T;

/** @brief 数据缓冲区-2K byte  */
typedef struct tag_OOP_BUF2K
{
    int64   buf[2048/sizeof(int64)];
}OOP_BUF2K_T;

/** @brief 数据缓冲区-3K byte  */
typedef struct tag_OOP_BUF3K
{
    int64   buf[1024*3/sizeof(int64)];
}OOP_BUF3K_T;

/** @brief 数据缓冲区-4K byte  */
typedef struct tag_OOP_BUF4K
{
    int64   buf[1024*4/sizeof(int64)];
}OOP_BUF4K_T;

/** @brief 数据缓冲区-5K byte  */
typedef struct tag_OOP_BUF5K
{
    int64   buf[1024*5/sizeof(int64)];
}OOP_BUF5K_T;

/** @brief 数据缓冲区-8K byte  */
typedef struct tag_OOP_BUF8K
{
    int64   buf[1024*8/sizeof(int64)];
}OOP_BUF8K_T;

/*
 ********************************************************************
 *                                                                  *
 * 2-通用数据类型定义                                               *
 *                                                                  *
 ********************************************************************
*/

/** @brief 数据访问结果枚举 */
typedef enum Data_Access_Result
{
    DATA_SUCCESS                        = 0,            /**< 成功                 */
    DATA_HARDWARE_FAULT                 = 1,            /**< 硬件失效               */
    DATA_TEMPORARY_FAILURE              = 2,            /**< 暂时失效               */
    DATA_READ_WRITE_DENIED              = 3,            /**< 拒绝读写               */
    DATA_OBJECT_UNDEFINED               = 4,            /**< 对象未定义                */
    DATA_OBJECT_CLASS_INCONSISTENT      = 5,            /**< 对象接口类不符合       */
    DATA_OBJECT_UNAVAILABLE             = 6,            /**< 对象不存在                */
    DATA_TYPE_UNMATCHED                 = 7,            /**< 类型不匹配                */
    DATA_SCOPE_OF_ACCESS_VIOLATED       = 8,            /**< 越界                 */
    DATA_DATA_BLOCK_UNAVAILABLE         = 9,            /**< 数据块不可用         */
    DATA_LONG_GET_ABORTED               = 10,           /**< 分帧传输已取消          */
    DATA_NO_LONG_GET_IN_PROGRESS        = 11,           /**< 不处于分帧传输状态        */
    DATA_LONG_SET_ABORTED               = 12,           /**< 块写取消               */
    DATA_NO_LONG_SET_IN_PROGRESS        = 13,           /**< 不处于块写状态          */
    DATA_INVALID_BLOCKNO                = 14,           /**< 数据块序号无效          */
    DATA_PASSWORD_FAILURE               = 15,           /**< 密码错/未授权            */
    DATA_SPEED_NOACCESS                 = 16,           /**< 通信速率不能更改       */
    DATA_TIMEZONE_OVER                  = 17,           /**< 年时区数超                */
    DATA_TIMEPERIOD_OVER                = 18,           /**< 日时段数超                */
    DATA_FEERATE_OVER                   = 19,           /**< 费率数超               */
    DATA_SECURITY_UNMATCHED             = 20,           /**< 安全认证不匹配          */
    DATA_RECHARGE_REPEAT                = 21,           /**< 重复充值               */
    DATA_ESAM_FAILURE                   = 22,           /**< ESAM验证失败           */
    DATA_SECURITY_FAILURE               = 23,           /**< 安全认证失败         */
    DATA_CUSTOMENO_INCONSISTENT         = 24,           /**< 客户编号不匹配          */
    DATA_RECHARGE_NUM_ERROR             = 25,           /**< 充值次数错误         */
    DATA_RECHARGE_VALUE_OVER            = 26,           /**< 购电超囤积                */
    DATA_ADDRESS_EXCEPTION              = 27,           /**< 地址异常               */
    DATA_SYMMETRIC_DECRYPTION_FAILURE   = 28,           /**< 对称解密错误         */
    DATA_ASYMMETRIC_DECRYPTION_FAILURE  = 29,           /**< 非对称解密错误          */
    DATA_SIGNATURE_FAILURE              = 30,           /**< 签名错误               */
    DATA_METER_SUSPEND                  = 31,           /**< 电能表挂起                */
    DATA_TIMEFLAG_INVALID               = 32,           /**< 时间标签无效         */
    DATA_REQUEST_TIMEOUT                = 33,           /**< 请求超时               */
    DATA_ESAM_P1P2_FAILURE              = 34,           /**< ESAM的P1P2不正确       */
    DATA_ESAM_LC_FAILURE                = 35,           /**< ESAM的LC错误            */
    DATA_OTHER_REASON                   = 255           /**< 其他                 */
}OOP_DAR_E;

/** @brief 福建数据访问结果枚举	*/
typedef enum FJ_Data_Access_Result
{
    ERC_SUCCESS			= 0,
    ERC_RSV				= 1,
    ERC_METER_REPEAT	= 2,
    ERC_FRAME_OVERSIZE	= 3,
    ERC_FRAME_CHECKERR	= 4,
    ERC_STORAGE_LESS	= 5,
    ERC_ADD_CHECKERR	= 6,
    ERC_SIGN_ERROR		= 7,
    ERC_CIPHER_ERROR	= 8,
    ERC_MAC_ERROR		= 9,
    ERC_PROTECT			= 10,
    ERC_TIMEOUT			= 11,
    ERC_TASK_NOEXEC		= 12,
    ERC_TASK_NOCONFIG	= 13,
    ERC_APP_NORESPONSE	= 14,
    ERC_LINK_BUSY		= 15,
    ERC_MODULE_ERROR	= 16,
    ERC_OTHER			= 255
}OOP_FJ_DAR_E;

/** @brief 执行序号和优先级 */
typedef union Invoke_ID_And_Priority
{
    uint8 value;
    struct
    {
        uint8 invoke_id : 6;            /**< 执行序号       */
        uint8 ACD       : 1;            /**< ACD位         */
        uint8 priority  : 1;            /**< 优先级          */
    };
}OOP_PIID_U;

/** @brief 长时间格式 日期-时间    */
typedef struct tag_DATETIME
{
    uint16  year;       /**< 年            */
    uint8   month;      /**< 月            */
    uint8   mday;       /**< 日            */
    uint8   wday;       /**< 周            */
    uint8   hour;       /**< 时            */
    uint8   minute;     /**< 分            */
    uint8   second;     /**< 秒            */
    uint16  msec;       /**< 毫秒     */
}OOP_DATETIME_T;

/** @brief 短时间格式 日期-时间    */
typedef struct tag_DATETIME_S
{
    uint16  year;       /**< 年            */
    uint8   month;      /**< 月            */
    uint8   day;        /**< 日            */
    uint8   hour;       /**< 时            */
    uint8   minute;     /**< 分            */
    uint8   second;     /**< 秒            */
    uint8   week;       /**< 周            */
}OOP_DATETIME_S_T;

/** @brief 日期   */
typedef struct tag_Date
{
    uint16  year;       /**< 年            */
    uint8   month;      /**< 月            */
    uint8   mday;       /**< 日            */
    uint8   wday;       /**< 周            */
}OOP_DATE_T;

/** @brief 时间   */
typedef struct tag_Time
{
    uint8   hour;       /**< 时            */
    uint8   minute;     /**< 分            */
    uint8   second;     /**< 秒            */
}OOP_TIME_T;

/** @brief 对象属性描述符    */
typedef union Object_Attribute_Descriptor
{
    uint32          value;              /**< OOP_OAD_U值                   */
    struct
    {
        uint8       nIndex;             /**< 属性数据索引         */
        struct
        {
            uint8   attID  : 5;         /**< 属性ID                   */
            uint8   attPro : 3;         /**< 属性标识(用于事件)     */
        };
        OI          nObjID;             /**< 对象标识               */
    };
}OOP_OAD_U;

/** @brief 对象属性特征描述符定义    */
typedef enum
{
    OOP_ATTPRO_CUR = 0,     /**< 实时数据属性     */
    OOP_ATTRPO_SEC,         /**< 秒冻结属性            */
    OOP_ATTPRO_MIN,         /**< 分钟冻结属性     */
    OOP_ATTPRO_HOUR,        /**< 小时冻结属性     */
    OOP_ATTPRO_DAY,         /**< 日冻结属性            */
    OOP_ATTPRO_MON,         /**< 月冻结属性            */
    OOP_ATTPRO_YEAR,        /**< 年冻结属性            */
}OOP_ATTPRO_E;

//
// 关联对象属性描述符
//

#define OOP_MAX_OADS        (24)        /**< 记录列关联OOP_OAD_U最大个数           */
#define OOP_MAX_CSDS_DEL    (32)        /**< 记录列最大个数(旧)                 */
#define OOP_MAX_CSDS        (36)        /**< 记录列最大个数                  */
#define OOP_MAX_COAD        (64)        /**< 复合型对象最大可包含OOP_OAD_U个数  */
#define OOP_MAX_RCSD        (64)        /**< APDU访问最大可带列(OOP_CSD_T)个数 */

/** @brief 一组对象属性描述符(参数)  */
typedef struct Object_Attribute_Descriptor_Group
{
    uint32      nNum;                   /**< 关联对象属性个数           */
    uint32      rsv;
    OOP_OAD_U   oad[OOP_MAX_OADS];      /**< 关联对象属性列表           */
}OOP_OADS_T;

/** @brief 记录型对象属性描述符(参数)   */
typedef struct Record_Object_Attribute_Descriptor
{
    OOP_OAD_U       oadMain;            /**< 主OOP_OAD_U           */
    OOP_OADS_T      oadCols;            /**< 关联对象属性         */
}OOP_ROAD_T;

/** @brief 对象方法描述符    */
typedef union Object_Method_Descriptor
{
    uint32      value;      /**< OMD值             */
    struct
    {
        uint8   nMode;      /**< 操作模式           */
        uint8   nModID;     /**< 方法ID               */
        OI      nObjID;     /**< 对象标识           */
    };
}OOP_OMD_U;

/** @brief 列类型        */
typedef enum Column_Selection_Type
{
    CSD_NORMAL = 0,     /**< 普通OOP_OAD_U类型  */
    CSD_RECORD = 1      /**< 记录型复合OOP_OAD_U   */
}OOP_CST_E;

/** @brief 列选择描述符(参数)   */
typedef struct Column_Selection_Descriptor
{
    uint8           choice;     /**< 类型标识           */
    union
    {
        OOP_OAD_U   oad;        /**< 单OOP_OAD_U       */
        OOP_ROAD_T  road;       /**< 记录型OOP_OAD_U */
    };
}OOP_CSD_T;

/** @brief 显示对象描述符   */
typedef struct tag_OOP_OBJ_SHOW
{
    OOP_CSD_T   cols; /**< 数组列          */
    uint8   nNum; /**< 屏序号          */
}OOP_OBJ_SHOW;


/** @brief 显示对象列表   结构体太大了会超过缓存的限制*/ 
#define MAX_OBJ_SHOW_NUM 5
typedef struct tag_OOP_OBJ_SHOW_T
{
    uint32      nNum;               /**< 数组列的个数     */
    uint32      rsv;                /**< 保留             */
    OOP_OBJ_SHOW   objs[MAX_OBJ_SHOW_NUM]; /**< 数组列          */
}OOP_OBJ_SHOW_T;

typedef struct
{
    uint8 objNumAll;
    uint8 objNumMax;
}OOP_OBJ_SHOW_PARAM_T;

/** @brief 一组列对象属性描述符(参数)   */
typedef struct Parm_Column_Selection_Descriptor
{
    uint32      nNum;               /**< 数组列的个数     */
    uint32      rsv;                /**< 保留             */
    OOP_CSD_T   cols[OOP_MAX_CSDS]; /**< 数组列          */
}OOP_PCSD_T;

/** @brief 记录型对象属性描述符(APDU) */
typedef struct
{
    OOP_OAD_U           oadMain;                /**< 主OOP_OAD_U       */
    struct
    {
        uint32          nNum;                   /**< 关联对象属性个数       */
        uint32          rsv;
        OOP_OAD_U       oad[OOP_MAX_COAD];      /**< 关联对象属性列表       */
    }oadCols;
}OOP_COAD_T;

/** @brief 列对象属性描述符(APDU)   */
typedef struct
{
    uint8           choice;     /**< 类型标识           */
    union
    {
        OOP_OAD_U   oad;        /**< 单OOP_OAD_U       */
        OOP_COAD_T  road;       /**< 记录型OOP_OAD_U     */
    };
}OOP_CCSD_T;

/** @brief 精简曲线 */
typedef struct
{
    OOP_OAD_U       oadMain;                /**< 主OOP_OAD_U       */
    struct
    {
        uint32      nNum;                   /**< 关联对象属性个数       */
        uint32      rsv;
        OOP_OAD_U   oad[OOP_MAX_COAD];      /**< 关联对象属性列表       */
        uint8       type[OOP_MAX_COAD];     /**< 列数据类型            */
    }oadCols;
}OOP_SOAD_T;

/** @brief 列对象属性描述符数组(APDU) */
typedef struct Record_Column_Selection_Descriptor
{
    uint32      nNum;
    uint32      rsv;
    OOP_CCSD_T  cols[OOP_MAX_RCSD];
}OOP_RCSD_T;

/** @brief 时间间隔单位   */
typedef enum tag_Time_Interval_Span
{
    TI_SEC  = 0,
    TI_MIN  = 1,
    TI_HOUR = 2,
    TI_DAY  = 3,
    TI_MON  = 4,
    TI_YEAR = 5
}OOP_TI_SPAN_E;

/** @brief 时间间隔     */
typedef struct tag_Time_Interval
{
    uint8       unit;                   /**< 间隔单位(见OOP_TI_SPAN_E) */
    uint16      value;                  /**< 间隔值                      */
}OOP_TI_T;

//
// 地址域
//
#ifdef AREA_FUJIAN
#define OOP_TSA_SIZE    (8)            /**< 服务器地址最大长度            */
#else
#define OOP_TSA_SIZE    (16)            /**< 服务器地址最大长度            */
#endif

/** @brief 地址域地址类型        */
typedef enum tag_AddFlag
{
    AF_Single    = 0,           /**< 单地址          */
    AF_Adapter   = 1,           /**< 通配地址       */
    AF_Group     = 2,           /**< 组地址          */
    AF_Broadcast = 3            /**< 广播地址       */
}OOP_ADDFLAG_E;

/** @brief 地址域标志      */
typedef union tag_AF
{
    uint8       body;
    struct
    {
        uint8   len     : 4;                /**< 地址长度                               */
        uint8   oldvxd  : 1;                /**< bit4取值0和1分别表示逻辑地址0和1           */
        uint8   flag    : 1;                /**< 有扩展逻辑地址                          */
        uint8   type    : 2;                /**< 地址类型                               */
    };
}OOP_AF_U;

/** @brief 地址域标志扩展        */
typedef union tag_AF_EX
{
    uint8       body;
    struct
    {
        uint8   len     : 4;                /**< 地址长度                               */
        uint8   vxdl    : 1;                /**< bit4取值0和1分别表示逻辑地址0和1           */
        uint8   vxdh    : 1;                /**< 有扩展逻辑地址                          */
        uint8   type    : 2;                /**< 地址类型                               */
    };
}OOP_AF_EX_U;

/**
 * @brief 目标服务器地址     
 * @note
 *      电能表地址是1，地址域(add)排列顺序如下
 *      OOP_TSA_T.len = 5
 *      OOP_TSA_T.add[0] = 0x00;
 *      OOP_TSA_T.add[1] = 0x00;
 *      OOP_TSA_T.add[2] = 0x00;
 *      OOP_TSA_T.add[3] = 0x00;
 *      OOP_TSA_T.add[4] = 0x00;
 *      OOP_TSA_T.add[5] = 0x01;
 */
typedef struct Target_Server_Address
{
    union
    {
        uint8       af;
        struct
        {
            uint8   len      : 4; /**< 地址长度取值范围：0…15，对应表示1…16个字节长度                                        */
            uint8   oldvxd   : 1; /**< 逻辑地址                                                             */
            uint8   flag     : 1; /**< 扩展逻辑地址标志                                                         */
            uint8   type     : 2; /**< 地址类型0表示单地址，1表示通配地址，2表示组地址，3表示广播地址                              */
        };
    };
    uint8 vxd;                      /**< 扩展逻辑地址         */
    uint8 add[OOP_TSA_SIZE-1];      /**< 地址域          */
}OOP_TSA_T;

/** @brief 服务器地址      */
typedef struct tag_SA
{
    uint8   len;
    uint8   add[OOP_TSA_SIZE];
}OOP_SA_T;

/** @brief 换算和单位      */
typedef struct tag_Scaler_Unit
{
    uint8   unit;           /**< 单位         */
    int8    scaler;         /**< 换算因子       */
}OOP_SCALER_UNIT_T;

/** @brief 时间戳信息      */
typedef struct tag_TimeFlag
{
    OOP_DATETIME_S_T    tmSend;     /**< 发送时间           */
    OOP_TI_T            tmDelay;    /**< 延时时间           */
}OOP_TIMEFLAG_T;

/** @brief 波特率定义  */
typedef enum tag_OOP_BAUD
{
    BPS300      = 0,
    BPS600      = 1,
    BPS1200     = 2,
    BPS2400     = 3,
    BPS4800     = 4,
    BPS7200     = 5,
    BPS9600     = 6,
    BPS19200    = 7,
    BPS38400    = 8,
    BPS57600    = 9,
    BPS115200   = 10,
    BPSDEF      = 255   /**< 自适应          */
}OOP_BAUD_E;

/** @brief 校验位    */
typedef enum tag_OOP_PARITY
{
    NoParity   = 0,
    OddParity  = 1,
    EvenParity = 2
}OOP_PARITY_E;

/** @brief 数据位    */
typedef enum tag_OOP_DataBits
{
    DB5 = 5,
    DB6 = 6,
    DB7 = 7,
    DB8 = 8
}OOP_DATABITS_E;

/** @brief  停止位   */
typedef enum tag_OOP_StopBits
{
    STOPBIT1 = 1,
    STOPBIT2 = 2
}OOP_STOPBITS_E;

/** @brief 流控   */
typedef enum tag_OOP_FlowCtrl
{
    FLOWCTRL_NONE = 0,  /**< 无                */
    FLOWCTRL_HARD = 1,  /**< 硬件流控       */
    FLOWCTRL_SOFT = 2   /**< 自适应          */
}OOP_FLOWCTRL_E;

/*
*********************************************************************
 OOP_COMDCB_T∷=SEQUENCE
 {
    波特率    ENUMERATED
    {
        300bps(0)，    600bps(1)，      1200bps(2)，
        2400bps(3)，  4800bps(4)，    7200bps(5)，
        9600bps(6)，  19200bps(7)，   38400bps(8)，
        57600bps(9)， 115200bps(10)， 自适应(255)
    }，
    校验位  ENUMERATED {无校验(0)，奇校验(1)，偶校验(2)}，
    数据位  ENUMERATED {5(5)，6(6)，7(7)，8(8)}，
    停止位  ENUMERATED {1(1)，2(2)}，
    流控  ENUMERATED {无(0)，硬件(1)，软件(2)}
 }
*********************************************************************
*/

/** @brief 串口通信控制块    */
typedef struct tag_ComDCB
{
    uint8   baud;           /**< 波特率              */
    uint8   parity;         /**< 校验位              */
    uint8   databits;       /**< 数据位              */
    uint8   stopbits;       /**< 停止位              */
    uint8   flowctrl;       /**< 流控             */
}OOP_COMDCB_T;

// 最大上报通道
#define OOP_MAX_CHANNEL (4)

//
// 电表集合
//

#ifdef AREA_FUJIAN
#define OOP_MAX_MSPARAM     (64)    /**< 电表集合数            */
#else
#define OOP_MAX_MSPARAM     (32)    /**< 电表集合数            */
#endif
#define OOP_MAX_MSREAD      (256)   /**< 电表集抄读合数  */
#define OOP_MAX_MSPMSI      (512)   /**< 一组配置序号           */

/** @brief 一组用户类型       */
typedef struct Meter_Set_Type
{
    uint16  size;
    uint8   type[OOP_MAX_MSPARAM];
}OOP_MST_T;

typedef struct Meter_Coll_Type
{
    uint16  size;
    uint8   type[OOP_MAX_MSREAD];
}OOP_MCT_T;

/** @brief 一组用户地址       */
typedef struct Meter_Set_Add
{
    uint16      size;
    OOP_TSA_T   add[OOP_MAX_MSPARAM];
}OOP_MSA_T;

typedef struct Meter_Coll_Add
{
    uint16 size;
    OOP_TSA_T  add[OOP_MAX_MSREAD];
} OOP_MCA_T;

/** @brief 一组用户配置序号     */
typedef struct Meter_Set_Index
{
    uint16 size;
    uint16 idx[OOP_MAX_MSPMSI];
}OOP_MSI_T;

typedef struct Meter_Coll_Index
{
    uint16 size;
    uint16 idx[OOP_MAX_MSREAD];
}OOP_MCI_T;

/** @brief 用户类型区间       */
typedef struct Meter_Region_Type
{
    uint8   nType;
    uint8   start;
    uint8   end;
}OOP_MRT_T;

/** @brief 用户地址区间       */
typedef struct Meter_Region_TSA
{
    uint8       nType;
    OOP_TSA_T   start;
    OOP_TSA_T   end;
}OOP_MRA_T;

/** @brief 用户配置序号区间     */
typedef struct Meter_Region_Index
{
    uint8   nType;
    uint16  start;
    uint16  end;
}OOP_MRI_T;

/** @brief 一组用户类型区间     */
typedef struct Meter_Set_Region_Type
{
    uint16      size;
    OOP_MRT_T   region[OOP_MAX_MSPARAM];
}OOP_MSRT_T;

typedef struct Meter_Coll_Region_Type
{
    uint16      size;
    OOP_MRT_T   region[OOP_MAX_MSREAD];
}OOP_MCRT_T;

/** @brief 一组用户地址区间     */
typedef struct Meter_Set_Region_TSA
{
    uint16      size;
    OOP_MRA_T   region[OOP_MAX_MSPARAM];
}OOP_MSRA_T;

typedef struct Meter_Coll_Region_TSA
{
    uint16      size;
    OOP_MRA_T   region[OOP_MAX_MSREAD];
}OOP_MCRA_T;

/** @brief 一组用户配置序号区间       */
typedef struct Meter_Set_Region_Index
{
    uint16      size;
    OOP_MRI_T   region[OOP_MAX_MSPARAM];
}OOP_MSRI_T;

typedef struct Meter_Coll_Region_Index
{
    uint16      size;
    OOP_MRI_T   region[OOP_MAX_MSREAD];
}OOP_MCRI_T;

/** @brief 一组设备类型		*/
typedef struct Meter_Set_Dev_Type
{
    uint16	size;
    uint16	type[OOP_MAX_MSPARAM];
}OOP_MSDT_T;

typedef struct Meter_Coll_Dev_Type
{
    uint16	size;
    uint16	type[OOP_MAX_MSREAD];
}OOP_MCDT_T;

/** @brief 特定逻辑设备		*/
typedef struct Meter_Coll_Vxd
{
    uint8	vxd;
}OOP_MCV_T;

/** @brief 电能表选择类型    */
typedef enum tag_MS_CHOICE
{
    MS_NULL         = 0,    /**< 无电能表               */
    MS_ALL          = 1,    /**< 所有电能表             */
    MS_TYPE         = 2,    /**< 用户类型               */
    MS_TSA          = 3,    /**< 用户地址               */
    MS_INDEX        = 4,    /**< 用户序号               */
    MS_REGION_TYPE  = 5,    /**< 用户类型区间           */
    MS_REGION_TSA   = 6,    /**< 用户地址区间           */
    MS_REGION_IDX   = 7,    /**< 用户序号区间           */
    MS_DEV_TYPE	    = 8,    /**< 设备类型		    */
    MS_VXD          = 9	    /**< 特定逻辑地址		  */
}OOP_MS_CHOICE_E;

/** @brief 电表集合(参数) */
typedef struct Meter_Set_Param
{
    uint8               choice;     /**< choice = 0(无电能表)               */
                                    /**< choice = 1(所有电能表)                */
    union
    {
        uint16          size;
        OOP_MST_T       mst;        /**< choice = 2(一组用户类型)         */
        OOP_MSA_T       msa;        /**< choice = 3(一组用户地址)         */
        OOP_MSI_T       msi;        /**< choice = 4(一组用户配置序号)       */
        OOP_MSRT_T      msrt;       /**< choice = 5(一组用户类型区间)       */
        OOP_MSRA_T      msra;       /**< choice = 6(一组用户地址区间)       */
        OOP_MSRI_T      msri;       /**< choice = 7(一组用户序号区间)       */
        OOP_MSDT_T      msdt;       /**< choice = 8(一组设备类型)     */
        OOP_MCV_T       msv;		/**< choice = 9(特定逻辑地址)			*/
        uint8           proto;		/**< choice = 10(特定通信协议)			*/	
    };
}OOP_MSP_T;

/** @brief 电表集合(抄读) */
typedef struct Meter_Set_Coll
{
    uint8               choice;     /**< choice = 0(无电能表)               */
                                    /**< choice = 1(所有电能表)                */
    union
    {
        uint16          size;
        OOP_MCT_T       mst;        /**< choice = 2(一组用户类型)         */
        OOP_MCA_T       msa;        /**< choice = 3(一组用户地址)         */
        OOP_MCI_T       msi;        /**< choice = 4(一组用户配置序号)       */
        OOP_MCRT_T      msrt;       /**< choice = 5(一组用户类型区间)       */
        OOP_MCRA_T      msra;       /**< choice = 6(一组用户地址区间)       */
        OOP_MCRI_T      msri;       /**< choice = 7(一组用户序号区间)       */
        OOP_MCDT_T      msdt;		/**< choice = 8(一组设备类型)			*/
        OOP_MCV_T       msv;		/**< choice = 9(特定逻辑地址)			*/
    };
}OOP_MSC_T;

/** @brief visible-string(SIZE(2))  */
typedef struct tag_OOP_VISIBLESTR2
{
    char value[4];
}OOP_VISIBLESTR2_T;

/** @brief visible-string(SIZE(4))  */
typedef struct tag_OOP_VISIBLESTR4
{
    char value[8];
}OOP_VISIBLESTR4_T;

/** @brief visible-string(SIZE(6))  */
typedef struct tag_OOP_VISIBLESTR6
{
    char value[8];
}OOP_VISIBLESTR6_T;

/** @brief visible-string(SIZE(8))  */
typedef struct tag_OOP_VISIBLESTR8
{
    char    value[16];
}OOP_VISIBLESTR8_T;

/** @brief visible-string(SIZE(11)) */
typedef struct tag_OOP_VISIBLESTR11
{
    char    value[16];
}OOP_VISIBLESTR11_T;

/** @brief visible-string(SIZE(14)) */
typedef struct tag_OOP_VISIBLESTR14
{
    char    value[16];
}OOP_VISIBLESTR14_T;

/** @brief visible-string(SIZE(15)) */
typedef struct tag_OOP_VISIBLESTR15
{
    char    value[16];
}OOP_VISIBLESTR15_T;

/** @brief visible-string(SIZE(16)) */
typedef struct tag_OOP_VISIBLESTR16
{
    char    value[32];
}OOP_VISIBLESTR16_T;

/** @brief visible-string(SIZE(20)) */
typedef struct tag_OOP_VISIBLESTR20
{
    char    value[32];
}OOP_VISIBLESTR20_T;

/** @brief visible-string(SIZE(8))  */
typedef struct tag_OOP_VISIBLEVAR8S
{
    uint32  nNum;
    char    value[16];
}OOP_VISIBLEVAR8S_T;

/** @brief visible-string(SIZE(0...16))变长 */
typedef struct tag_OOP_VISIBLESTRVAR16
{
    uint32  nNum;
    char    value[16];
}OOP_VISIBLESTRVAR16_T;

/** @brief visible-string(SIZE(32)) */
typedef struct tag_OOP_VISIBLEVAR32
{
    uint32  nNum;
    char    value[32];
}OOP_VISIBLEVAR32_T;

/** @brief visible-string(变长)   */
typedef struct tag_OOP_VISIBLESTR
{
    uint32  nNum;
    char    value[64];
}OOP_VISIBLEVAR_T;

/** @brief visible-string128(变长)    */
typedef struct tag_OOP_VISIBLESTR128
{
    uint32  nNum;
    char    value[128];
}OOP_VISIBLEVAR128_T;

/** @brief visible-string256(变长)    */
typedef struct tag_OOP_VISIBLESTR256
{
    uint32  nNum;
    char    value[256];
}OOP_VISIBLEVAR256_T;

/** @brief visible-string1K(变长) */
typedef struct tag_OOP_VISIBLESTR1K
{
    uint32  nNum;
    char    value[1024];
}OOP_VISIBLEVAR1K_T;

/** @brief octet-string(SIZE(2))    */
typedef struct tag_OOP_OCTETSTR2_T
{
    uint8  nValue[2];
}OOP_OCTETSTR2_T;

/** @brief octet-string(SIZE(3))    */
typedef struct tag_OOP_OCTETSTR3
{
    uint8  nValue[3];
}OOP_OCTETSTR3_T;

/** @brief octet-string(SIZE(4))    */
typedef struct tag_OOP_OCTETSTR4
{
    uint8  nValue[4];
}OOP_OCTETSTR4_T;

/** @brief octet-string(SIZE(6))    */
typedef struct tag_OOP_OCTETSTR6
{
    uint8  nValue[6];
}OOP_OCTETSTR6_T;

/** @brief octet-string(SIZE(8))    */
typedef struct tag_OOP_OCTETSTR8
{
    uint8  nValue[8];
}OOP_OCTETSTR8_T;

/** @brief octet-string(SIZE(32))   */
typedef struct tag_OOP_OCTETSTR32
{
    uint8 nValue[32];
}OOP_OCTETSTR32_T;

/** @brief octet-string(可变) */
typedef struct tag_OOP_OCTETVAR
{
    uint32  nNum;
    uint8   value[0];
}OOP_OCTETVAR_T;

/** @brief octet-string(SIZE(8))    */
typedef struct tag_OOP_OCTETVAR8
{
    uint32  nNum;
    uint8   value[8];
}OOP_OCTETVAR8_T;

/** @brief octet-string(SIZE(16))   */
typedef struct tag_OOP_OCTETVAR16
{
    uint32  nNum;
    uint8   value[16];
}OOP_OCTETVAR16_T;

/** @brief octet-string(SIZE(64))   */
typedef struct tag_OOP_OCTETVAR64
{
    uint32  nNum;
    uint8   value[64];
}OOP_OCTETVAR64_T;

/** @brief octet-string(SIZE(256))  */
typedef struct tag_OOP_OCTETVAR256
{
    uint32  nNum;
    uint8   value[256];
}OOP_OCTETVAR256_T;

/** @brief octet-string(SIZE(256))  */
typedef struct tag_OOP_OCTETVAR512
{
    uint32  nNum;
    uint8   value[512];
}OOP_OCTETVAR512_T;

/** @brief octet-string(SIZE(1K))   */
typedef struct tag_OOP_OCTETVAR1K
{
    uint32  nNum;
    uint8   value[1024];
}OOP_OCTETVAR1K_T;

/** @brief octet-string(SIZE(2K))   */
typedef struct tag_OOP_OCTETVAR2K
{
    uint32  nNum;
    uint8   value[2048];
}OOP_OCTETVAR2K_T;

/** @brief bit-string(SIZE(3))      */
typedef struct tag_OOP_BITSTR3
{
    uint8    nValue;         
}OOP_BITSTR3_T;

/** @brief bit-string(SIZE(8))      */
typedef struct tag_OOP_BITSTR8
{
    union
    {
        uint8    bitValue;
        uint8    nValue;
    };    
}OOP_BITSTR8_T;

/** @brief bit-string(SIZE(16))     */
typedef struct tag_OOP_BITSTR16
{
    union
    {
        uint16  bitValue;
        uint8   nValue[2];
    };
}OOP_BITSTR16_T;

/** @brief bit-string(SIZE(32))     */
typedef struct tag_OOP_BITSTR32
{
    uint8   nValue[4];
}OOP_BITSTR32_T;

/** @brief bit-string(SIZE(64))     */
typedef struct tag_OOP_BITSTR64
{
    uint8   nValue[8];
}OOP_BITSTR64_T;

/** @brief bit-string(SIZE(96))     */
typedef struct tag_OOP_BITSTR96
{
    uint8   nValue[12];
}OOP_BITSTR96_T;

/** @brief bit-string(SIZE(128))    */
typedef struct tag_OOP_BITSTR128
{
    uint8   nValue[16];
}OOP_BITSTR128_T;

/** @brief bit-string(SIZE(16)) */
typedef struct tag_OOP_BITSTRV16
{
    uint32  nNum;
    uint8   nValue[2];
}OOP_BITSTRV16_T;

/** @brief bit-string(SIZE(32)) */
typedef struct tag_OOP_BITSTRV32
{
    uint32  nNum;
    uint8   nValue[4];
}OOP_BITSTRV32_T;

/** @brief bit-string(SIZE(256))    */
typedef struct tag_OOP_BITSTRV256
{
    uint32  nNum;
    uint8   nValue[32];
}OOP_BITSTRV256_T;

/** @brief bit-string(SIZE(4096))   */
typedef struct tag_OOP_BITSTRV4K
{
    uint32  nNum;
    uint8   nValue[512];
}OOP_BITSTRV4K_T;

/** @brief bit-string(SIZE(8192))   */
typedef struct tag_OOP_BITSTRV8K
{
    uint32  nNum;
    uint8   nValue[1024];
}OOP_BITSTRV8K_T;

/** @brief DT_DATA类型实现  */
typedef struct tag_Variant
{
    uint8                   type;       /**< 数据类型                       */
    union
    {
        char                cVal;       /**< DT_INTEGER                 */
        BOOLEAN             bVal;       /**< DT_BOOL                    */
        uint8               byVal;      /**< DT_UNSIGNED                */
        OOP_DATETIME_T      dtVal;      /**< DT_DATETIME                */
        OOP_DATE_T          dVal;       /**< DT_DATE                    */
        OOP_TIME_T          tVal;       /**< DT_TIME                    */
        OOP_DATETIME_S_T    tmVal;      /**< DT_DATETIEM_S              */
        int32               nVal;       /**< DT_DOUBLE_LONG             */
        uint16              wVal;       /**< DT_LONG_UNSIGNED           */
        int16               sVal;       /**< DT_LONG                    */
        uint32              dwVal;      /**< DT_DOUBLE_LONG_UNSIGNED    */
        int64               llVal;      /**< DT_LONG64                  */
        uint64              ullVal;     /**< DT_LONG64_UNSIGNED         */
        float32             fVal;       /**< DT_FLOAT32                 */
        float64             dbVal;      /**< DT_FLOAT64                 */
        OI                  oi;         /**< DT_OI                      */
        OOP_TI_T            ti;         /**< DT_TI                      */
        OOP_TSA_T           tsa;        /**< DT_TSA                     */
        OOP_OAD_U           oad;        /**< DT_OAD                     */
        OOP_ROAD_T          road;       /**< DT_ROAD                    */
        OOP_OMD_U           omd;        /**< DT_OMD                     */
        OOP_CSD_T           csd;        /**< DT_CSD                     */
        OOP_COMDCB_T        dcb;        /**< DT_COM_DCB                 */
        OOP_OCTETVAR16_T    octVal;     /**< DT_OCTET_STRING            */
    };
}OOP_VARIANT_T;

/** @brief VQDS数据类型 */
typedef struct tag_VQDS
{
    uint8                   nChoice;    /**< 数据类型                       */
    union
    {
        char                cVal;       /**< DT_INTEGER                 */
        uint8               byVal;      /**< DT_UNSIGNED                */
        int32               nVal;       /**< DT_DOUBLE_LONG             */
        uint16              wVal;       /**< DT_LONG_UNSIGNED           */
        int16               sVal;       /**< DT_LONG                    */
        uint32              dwVal;      /**< DT_DOUBLE_LONG_UNSIGNED    */
        int64               llVal;      /**< DT_LONG64                  */
        uint64              ullVal;     /**< DT_LONG64_UNSIGNED         */
        float32             fVal;       /**< DT_FLOAT32                 */
        float64             dbVal;      /**< DT_FLOAT64                 */
    };
    OOP_BITSTR8_T           qds;        /**< 品质标志               */
}OOP_VQDS_T;

/** @brief OOP_RSD_T-selector0  */
typedef struct tag_Selector0
{
    uint8   rsv;
}OOP_SELECTOR0_T;

/** @brief OOP_RSD_T-selector1  */
typedef struct tag_Selector1
{
    OOP_OAD_U       oad;
    OOP_VARIANT_T   value;
}OOP_SELECTOR1_T;

/** @brief OOP_RSD_T-selector2  */
typedef struct tag_Selector2
{
    OOP_OAD_U       oad;
    OOP_VARIANT_T   from;
    OOP_VARIANT_T   to;
    OOP_VARIANT_T   span;
}OOP_SELECTOR2_T;

#define MAX_SELECTOR2_NUM   (8) /**<  一组selector2的最大个数    */

/** @brief OOP_RSD_T-selector3  */
typedef struct tag_Selector3
{
    uint32              nNum;
    OOP_SELECTOR2_T     item[MAX_SELECTOR2_NUM];
}OOP_SELECTOR3_T;

/** @brief OOP_RSD_T-selector4  */
typedef struct tag_SelectorP4
{
    OOP_DATETIME_S_T    time;
    OOP_MSP_T           ms;
}OOP_SELECTORP4_T;

typedef struct tag_Selector4
{
    OOP_DATETIME_S_T    time;
    OOP_MSC_T           ms;
}OOP_SELECTOR4_T;

/** @brief OOP_RSD_T-selector5  */
typedef struct tag_SelectorP5
{
    OOP_DATETIME_S_T    time;
    OOP_MSP_T           ms;
}OOP_SELECTORP5_T;

typedef struct tag_Selector5
{
    OOP_DATETIME_S_T    time;
    OOP_MSC_T           ms;
}OOP_SELECTOR5_T;

/** @brief OOP_RSD_T-selector6  */
typedef struct tag_SelectorP6
{
    OOP_DATETIME_S_T    start;
    OOP_DATETIME_S_T    end;
    OOP_TI_T            ti;
    OOP_MSP_T           ms;
}OOP_SELECTORP6_T;

typedef struct tag_Selector6
{
    OOP_DATETIME_S_T    start;
    OOP_DATETIME_S_T    end;
    OOP_TI_T            ti;
    OOP_MSC_T           ms;
}OOP_SELECTOR6_T;

/** @brief OOP_RSD_T-selector7  */
typedef struct tag_SelectorP7
{
    OOP_DATETIME_S_T    start;
    OOP_DATETIME_S_T    end;
    OOP_TI_T            ti;
    OOP_MSP_T           ms;
}OOP_SELECTORP7_T;

typedef struct tag_Selector7
{
    OOP_DATETIME_S_T    start;
    OOP_DATETIME_S_T    end;
    OOP_TI_T            ti;
    OOP_MSC_T           ms;
}OOP_SELECTOR7_T;

/** @brief OOP_RSD_T-selector8  */
typedef struct tag_SelectorP8
{
    OOP_DATETIME_S_T    start;
    OOP_DATETIME_S_T    end;
    OOP_TI_T            ti;
    OOP_MSP_T           ms;
}OOP_SELECTORP8_T;

typedef struct tag_Selector8
{
    OOP_DATETIME_S_T    start;
    OOP_DATETIME_S_T    end;
    OOP_TI_T            ti;
    OOP_MSC_T           ms;
}OOP_SELECTOR8_T;

/** @brief OOP_RSD_T-selector9  */
typedef struct tag_Selector9
{
    uint8   nLast;
}OOP_SELECTOR9_T;

/** @brief OOP_RSD_T-selector10 */
typedef struct tag_SelectorP10
{
    uint8           nTimes;
    OOP_MSP_T       ms;
}OOP_SELECTORP10_T;

typedef struct tag_Selector10
{
    uint8           nTimes;
    OOP_MSC_T       ms;
}OOP_SELECTOR10_T;

/** @brief OOP_RSD_T-selector11 */
typedef struct tag_SelectorP11
{
    OOP_DATETIME_S_T    start;
    OOP_DATETIME_S_T    end;
    uint8               nTimes;
    OOP_MSP_T           ms;
}OOP_SELECTORP11_T;

typedef struct tag_Selector11
{
    OOP_DATETIME_S_T    start;
    OOP_DATETIME_S_T    end;
    uint8               nTimes;
    OOP_MSC_T           ms;
}OOP_SELECTOR11_T;

/** @brief OOP_RSD_T-selector50	*/
typedef struct tag_Selector50
{
    uint16		nTimes;						/**< 上n条记录 */
    uint8		nCount;						/**< 记录条数（m） */
}OOP_SELECTOR50_T;

/** @brief OOP_RSD_T-selector51	*/
typedef struct tag_SelectorP51
{
    uint16				nTaskId;			/**< 透明任务方案号 */
    OOP_DATETIME_S_T	dtSave;				/**< 采集存储时间 */
    OOP_MSP_T			ms;					/**< 电表集合 */
}OOP_SELECTORP51_T;

typedef struct tag_Selector51
{
    uint16				nTaskId;			/**< 透明任务方案号 */
    OOP_DATETIME_S_T	dtSave;				/**< 采集存储时间 */
    OOP_MSC_T			ms;					/**< 电表集合 */
}OOP_SELECTOR51_T;

/** @brief OOP_RSD_T-selector52	*/
#define OOP_MAX_FPLAN		(255)

typedef struct tag_Selector52
{
    uint16				nTaskId;				/**< 透明任务方案号 */
    uint32				nNum;
    uint16				planId[OOP_MAX_FPLAN];	/**< 具体任务编号 */		
    OOP_DATETIME_S_T	dtSave;					/**< 采集存储时间 */
}OOP_SELECTOR52_T;

/** @brief OOP_RSD_T-selector53	*/
typedef struct tag_SelectorP53
{
    uint16				nTaskId;			/**< 透明任务方案号 */
    OOP_DATETIME_S_T	start;				/**< 采集存储时间起始值 */	
    OOP_DATETIME_S_T	end;				/**< 采集存储时间结束值 */
    OOP_MSP_T			ms;					/**< 电表集合 */
}OOP_SELECTORP53_T;

typedef struct tag_Selector53
{
    uint16				nTaskId;			/**< 透明任务方案号 */
    OOP_DATETIME_S_T	start;				/**< 采集存储时间起始值 */	
    OOP_DATETIME_S_T	end;				/**< 采集存储时间结束值 */
    OOP_MSC_T			ms;					/**< 电表集合 */
}OOP_SELECTOR53_T;

/** @brief OOP_RSD_T-selector54	*/
typedef struct tag_SelectorP54
{
    uint16				nTaskId;			/**< 透明任务方案号 */
    uint16				nTimes;				/**< 上n条记录 */
    uint8				nCount;				/**< 记录条数（m） */
    OOP_MSP_T			ms;					/**< 电表集合 */
}OOP_SELECTORP54_T;

typedef struct tag_Selector54
{
    uint16				nTaskId;			/**< 透明任务方案号 */
    uint16				nTimes;				/**< 上n条记录 */
    uint8				nCount;				/**< 记录条数（m） */
    OOP_MSC_T			ms;					/**< 电表集合 */
}OOP_SELECTOR54_T;

/** @brief 记录行选择描述符 */
typedef struct tag_PRSD
{
    uint8                   choice;     /**<  OOP_RSD_T选择类型         */
    union
    {
        OOP_SELECTOR1_T     sel0;       /**<  selector0-无选择       */
        OOP_SELECTOR1_T     sel1;       /**<  selector1             */
        OOP_SELECTOR2_T     sel2;       /**<  selector2             */
        OOP_SELECTOR3_T     sel3;       /**<  selector3             */
        OOP_SELECTORP4_T    sel4;       /**<  selector4             */
        OOP_SELECTORP5_T    sel5;       /**<  selector5             */
        OOP_SELECTORP6_T    sel6;       /**<  selector6             */
        OOP_SELECTORP7_T    sel7;       /**<  selector7             */
        OOP_SELECTORP8_T    sel8;       /**<  selector8             */
        OOP_SELECTOR9_T     sel9;       /**<  selector9             */
        OOP_SELECTORP10_T   sel10;      /**<  selector10            */
        OOP_SELECTORP11_T   sel11;      /**<  selector11            */
        OOP_SELECTOR50_T    sel50;      /**<  selector50			*/
        OOP_SELECTORP51_T   sel51;      /**<  selector50			*/
        OOP_SELECTOR52_T    sel52;      /**<  selector51			*/
        OOP_SELECTORP53_T   sel53;      /**<  selector52			*/
        OOP_SELECTORP54_T   sel54;      /**<  selector53			*/
    };
}OOP_PRSD_T;

typedef struct tag_RSD
{
    uint8                   choice;     /**<  OOP_RSD_T选择类型         */
    union
    {
        OOP_SELECTOR1_T     sel0;       /**<  selector0-无选择       */
        OOP_SELECTOR1_T     sel1;       /**<  selector1             */
        OOP_SELECTOR2_T     sel2;       /**<  selector2             */
        OOP_SELECTOR3_T     sel3;       /**<  selector3             */
        OOP_SELECTOR4_T     sel4;       /**<  selector4             */
        OOP_SELECTOR5_T     sel5;       /**<  selector5             */
        OOP_SELECTOR6_T     sel6;       /**<  selector6             */
        OOP_SELECTOR7_T     sel7;       /**<  selector7             */
        OOP_SELECTOR8_T     sel8;       /**<  selector8             */
        OOP_SELECTOR9_T     sel9;       /**<  selector9             */
        OOP_SELECTOR10_T    sel10;      /**<  selector10            */
        OOP_SELECTOR11_T    sel11;      /**<  selector11            */
        OOP_SELECTOR50_T    sel50;      /**<  selector50            */
        OOP_SELECTOR51_T    sel51;      /**<  selector50            */
        OOP_SELECTOR52_T    sel52;      /**<  selector51            */
        OOP_SELECTOR53_T    sel53;      /**<  selector52            */
        OOP_SELECTOR54_T    sel54;      /**<  selector53            */
    };
}OOP_RSD_T;

/*
 ********************************************************************
 *                                                                  *
 * 3-APDU数据单元定义                                             *
 *                                                                  *
 ********************************************************************
*/

/*
*********************************************************************
* 3.1-安全数据传输
*********************************************************************
*/

#define OOP_MAC_LEN     (16)    /**< OOP_MAC_T的长度     */
#define OOP_RN_LEN      (128)   /**< RAND的长度              */
#define OOP_MAX_DSLEN   (128)   /**< 数字签名最大长度       */
#define OOP_SID_LEN     (32)    /**< SID最大长度			*/

#define OOP_MAX_SKNUM	(16)	/**< 会话密钥包个数			*/
#define OOP_MAX_SKLEN	(32)	/**< 会话密钥包长度			*/

/** @brief 数据验证码  */
typedef struct tag_MAC
{
    uint32      nDNum;
    uint8       data[OOP_MAC_LEN];
}OOP_MAC_T;

/** @brief 安全标识 */
typedef struct tag_SSID
{
    uint32      nKNum;
    union
    {
        uint32  key;
        uint8	keyData[OOP_SID_LEN];
    };
}OOP_SSID_T;

/** @brief 随机数    */
typedef struct tag_RN
{
    uint32  len;
    uint8   data[OOP_RN_LEN];
}OOP_RN_T;

/** @brief 数字签名 */
typedef struct tag_DS
{
    uint8 len;
    uint8 data[OOP_MAX_DSLEN];
}OOP_DS_T;

/** @brief 会话密钥包	*/
typedef struct tag_SK
{
    uint16 nNum;
    int32  len;
    uint8  data[OOP_MAX_SKLEN*OOP_MAX_SKNUM];
}OOP_SK_T;

/** @brief 安全操作	*/
typedef enum SecurityOpt
{
    SecurityNormal    = 0,            /**< 普通安全操作*/
    SecuritySyncTime  = 1,            /**< 校时 */
    SecuritySetEsam   = 2,            /**< 设置esam参数*/	
}OOP_SECURITYOPT_E;

/** @brief 更新会话时效门限、转加密初始化、置离线计数器	*/
typedef struct tag_ESAM_PARAM
{
    OOP_OCTETVAR256_T  data;         /**< 参数数据密文*/    
    OOP_SSID_T         sid;          /**< SID*/
}OOP_ESAM_PARAM_T;

/*
*********************************************************************
* 3.2-应用连接管理
*********************************************************************
*/

#define OOP_PWD_LEN             (8)     /**< 密码域长度                */
#define OOP_MAX_SECURITYINFO    (128)   /**< 安全信息最大长度       */
#define OOC_MAX_METHODS         (10)    /**< 最大方法个数         */

/** @brief 安全认证加密信息 */
typedef struct tag_SecurityContext
{
    uint8       len;                            /**< 安全认证信息长度   */
    uint8       info[OOP_MAX_SECURITYINFO];     /**< 安全认证信息内容   */
    OOP_DS_T    ds;                             /**< 数字签名           */
}OOP_SECURITYCONTEXT_T;

/** @brief 安全认证类型   */
typedef enum tag_SecurityType
{
    SecurityOfNull       = 0,           /**< 公共连接           */
    SecurityOfPassword   = 1,           /**< 一般密码           */
    SecurityOfSymmetry   = 2,           /**< 对称加密           */
    SecurtiyOfSignature  = 3            /**< 数字签名           */
}OOP_SECURITYTYPE_E;

/** @brief 安全认证方案   */
typedef struct tag_SecurityMechanismInfo
{
    uint8                       choice;                 /**< choice = 0(公共连接)   */
    union
    {
        char                    password[OOP_PWD_LEN];  /**< choice = 1             */
        OOP_SECURITYCONTEXT_T   symmetry;               /**< choice = 2             */
        OOP_SECURITYCONTEXT_T   signature;              /**< choice = 3             */
    };
}OOP_SECURITYMECHANISMINFO_T;

/** @brief 安全认证确认数据域  */
typedef struct tag_SecurityData
{
    OOP_RN_T    rand;       /**< 随机数          */
    OOP_DS_T    ds;         /**< 数字签名       */
}OOP_SECURITYDATA_T;

/** @brief 应用连接请求认证的结果    */
typedef enum Connect_Result
{
    CONNECT_SUCCESS                         = 0,            /**< 允许建立应用连接   */
    CONNECT_PASSWORD_ERROR                  = 1,            /**< 密码错误           */
    CONNECT_SYMMETRIC_DECRYPTION_FAILURE    = 2,            /**< 对称解密错误     */
    CONNECT_ASYMMETRIC_DECRYPTION_FAILURE   = 3,            /**< 非对称解密错误      */
    CONNECT_SIGNATURE_FAILURE               = 4,            /**< 签名错误           */
    CONNECT_PROTOCOL_VERSION_UNMATCHED      = 5,            /**< 协议版本不匹配      */
    CONNECT_OTHER_REASON                    = 255           /**< 其他错误           */
}OOP_CR_E;

/** @brief 安全认证确认结果 */
typedef struct tag_SecurityResponseInfo
{
    uint8               result; /**< 安全认证结果         */
    BOOLEAN             bData;  /**< 是否包含数据域          */
    OOP_SECURITYDATA_T  data;   /**< 数据域内容                */
}OOP_SECURITYRESPONSEINFO_T;

/** @brief 一个对象访问权限描述   */
typedef struct tag_Object
{
    OI          object;                         /**< 对象OI               */
    uint8       nClass;                         /**< 接口类              */
    uint32      attNoAccess;                    /**< 不可访问权限     */
    uint32      attRead;                        /**< 只读权限           */
    uint32      attWrite;                       /**< 只写权限           */
    uint32      attReadWrite;                   /**< 可读写权限            */
    uint8       funNum;                         /**< 可执行方法个数      */
    uint8       method[OOC_MAX_METHODS];        /**< 可执行方法ID      */
}OOP_OBJECT_T;

/** @brief 应用连接对象描述 */
typedef struct tag_ASSOCIATION
{
    uint8           nSecret;                    /**< 安全类型               */
    uint32          VER;                        /**< 协议版本               */
    OOP_OBJECT_T*   arrObjs;                    /**< 对象列表               */
    uint16          nCount;                     /**< 对象列表数量         */
    uint16          nRecvSize;                  /**< 最大接受APDU尺寸单元   */
    uint16          nSendSize;                  /**< 最大发送APDU尺寸单元   */
    uint8           nPDUWindows;                /**< 发送窗口尺寸         */
    uint16          nMaxPDUSize;                /**< 最大可处理PDU尺寸       */
    uint8           PCB[8];                     /**< 协议一致性                */
    uint8           FCB[16];                    /**< 功能一致性                */
    uint32          nTimeout;                   /**< 静态超时时间         */
}OOP_ASSOCIATION_T;

/*
*********************************************************************
* 3.3-链路管理
*********************************************************************
*/

/** @brief 链路管理类型   */
typedef enum tag_LinkType
{
    Link_Login      = 0,
    Link_Heartbeat  = 1,
    Link_Logout     = 2
}OOP_LINKTYPE_E;

/** @brief 链路管理结果   */
typedef union tag_LinkResult
{
    struct
    {
        uint8   ret         : 3;            /**< 链路维护结果 */
        uint8   resv        : 4;            /**< 保留         */
        uint8   clockFlag   : 1;            /**< 始终可信标志 */
    };
    uint8       value;
}OOP_LINKRESULT_E;

/** @brief 链路管理请求   */
typedef struct tag_LinkRequest
{
    uint8               nType;
    uint16              time;
    OOP_DATETIME_T      clock;
}OOP_LINKREQUEST_T;

/** @brief 链路管理响应   */
typedef struct tag_LinkResponse
{
    OOP_LINKRESULT_E    result;
    OOP_DATETIME_T      tmRequest;
    OOP_DATETIME_T      tmReceive;
    OOP_DATETIME_T      tmRespond;
}OOP_LINKRESPONSE_T;

/*
 ********************************************************************
 *                                                                  *
 * 4-接口类数据结构                                                  *
 *                                                                  *
 ********************************************************************
*/

/*
*********************************************************************
* 4.1-class1-电能量类
*********************************************************************
*/
#define DUOXINMETER_TEST 0

//24送检，直接按照要求改成12费率
#define OOP_MAX_RATES       (12)                /**< 费率个数       */

#define OOP_FULL_RATES      (OOP_MAX_RATES+1)   /**< 总及分费率个数  */

/** @brief 0xxx-电能量(不带符号)         */
typedef struct tag_OOP_ENERGY
{
    uint32          nNum;
    uint32          rsv;
    uint32          nValue[OOP_FULL_RATES];
}OOP_ENERGY_T;

/** @brief 0xxx-电能量(带符号)        */
typedef struct tag_OOP_ENERGYS
{
    uint32          nNum;
    uint32          rsv;
    int32           nValue[OOP_FULL_RATES];
}OOP_ENERGYS_T;

/** @brief 0xxx-高精度电能量(不带符号)    */
typedef struct tag_OOP_HENERGY
{
    uint32          nNum;
    uint32          rsv;
    uint64          nValue[OOP_FULL_RATES];
}OOP_HENERGY_T;

/** @brief 0xxx-高精度电能量(带符号)   */
typedef struct tag_OOP_HENERGYS
{
    uint32          nNum;
    uint32          rsv;
    int64           nValue[OOP_FULL_RATES];
}OOP_HENERGYS_T;

/*
*********************************************************************
* 4.2-class2-需量类
*********************************************************************
*/

/** @brief 1xxx-需量(不带符号)    */
typedef struct tag_DEMAND
{
    uint32              nValue;
    OOP_DATETIME_S_T    time;
}DEMAND;

/** @brief 1xxx-需量(带符号)   */
typedef struct tag_DEMANDS
{
    int32                   nValue;
    OOP_DATETIME_S_T    time;
}DEMANDS;

/** @brief 需量数组(不带符号)   */
typedef struct tag_OOP_DEMAND
{
    uint32          nNum;
    uint32          rsv;
    DEMAND          demand[OOP_FULL_RATES];
}OOP_DEMAND_T;

/** @brief 需量数组(带符号)  */
typedef struct tag_OOP_DEMANDS
{
    uint32          nNum;
    uint32          rsv;
    DEMANDS         demand[OOP_FULL_RATES];
}OOP_DEMANDS_T;

/*
*********************************************************************
* 4.3-class3-分相变量类
*********************************************************************
*/

/** @brief long-unsigned array          */
typedef struct tag_OOP_WORD3V
{
    uint32  nNum;
    uint32  rsv;
    uint16  nValue[3];
}OOP_WORD3V_T;

/** @brief double-long-unsigned array   */
typedef struct tag_OOP_DWORD3V
{
    uint32  nNum;
    uint32  rsv;
    uint32  nValue[3];
}OOP_DWORD3V_T;

/** @brief  long array  */
typedef struct tag_OOP_LONG3V
{
    uint32  nNum;
    uint32  rsv;
    int16   nValue[3];
}OOP_LONG3V_T;

/** @brief double-long array    */
typedef struct tag_OOP_INT3V
{
    uint32  nNum;
    uint32  rsv;
    int32   nValue[3];
}OOP_INT3V_T;

/*
*********************************************************************
* 4.4-class4-功率类
*********************************************************************
*/

/** @brief long-unsigned array          */
typedef struct tag_OOP_WORD4V
{
    uint32  nNum;
    uint32  rsv;
    uint16  nValue[4];
}OOP_WORD4V_T;

/** @brief long-unsigned fixed array    */
typedef struct tag_OOP_WORD4
{
    uint16  nValue[4];
}OOP_WORD4_T;

/** @brief long array   */
typedef struct tag_OOP_LONG4V
{
    uint32  nNum;
    uint32  rsv;
    int16   nValue[4];
}OOP_LONG4V_T;

/** @brief double-long array    */
typedef struct tag_OOP_INT4V
{
    uint32  nNum;
    uint32  rsv;
    int32       nValue[4];
}OOP_INT4V_T;

/*
*********************************************************************
* 4.5-class5-谐波
*********************************************************************
*/

#define OOP_MAX_HARMONIC    (21)                /**< 谐波次数       */

/** 
* @brief    谐波含有量-                \n
*           200D-电压谐波含有量      \n
*           200E-电流谐波含有量
*/
typedef struct tag_OOP_HARMONIC
{
    uint32          nNum;
    uint32          rsv;
    int16           nValue[OOP_MAX_HARMONIC];
}OOP_HARMONIC_T;

/** @brief 电压含量 2033 2~n次谐波(不含基波)*/
typedef struct tag_OOP_HARMONIC_U_VALUE
{
    uint32  nNum;
    uint32  rsv;
    uint32  nValue[OOP_MAX_HARMONIC];
}OOP_HARMONIC_U_VALUE_T;

/** @brief 2033-电压谐波含有量			*/
typedef struct tag_OOP_HARMONIC_VOL
{
    uint32		    nNum;
    uint32			rsv;
    uint16		    nValue[OOP_MAX_HARMONIC];
}OOP_HARMONIC_VOL_T;

/** @brief 电流含量 2034 2~n次谐波(不含基波)*/
typedef struct tag_OOP_HARMONIC_I_VALUE
{
    uint32  nNum;
    uint32  rsv;
    int32   nValue[OOP_MAX_HARMONIC];
}OOP_HARMONIC_I_VALUE_T;

/** @brief 2034-电流谐波含有量			*/
typedef struct tag_OOP_HARMONIC_AMP
{
    uint32		    nNum;
    uint32			rsv;
    int32		    nValue[OOP_MAX_HARMONIC];
}OOP_HARMONIC_AMP_T;

/*
*********************************************************************
* 4.6-class07-事件对象接口类
*********************************************************************
*/

#define OOP_MAX_REPORTCHANNEL   (4)     /**< 事件上报通道最大个数 */
#define OOP_EVT_BUFSIZE         (256)   /**< 事件数据缓冲区尺寸        */
#define OOP_EVT_BUF1K           (1024)	/**< 事件数据缓冲区尺寸1K	*/

/** @brief 事件发生源  */
typedef struct tag_OOP_EVTSOURCE
{
    uint8           choice;
    union
    {
        uint8       nValue;
        OOP_TSA_T   add;
        OOP_OAD_U   oad;
        OI          oi;
    };
}OOP_EVTSOURCE_T;

/** @brief 事件当前值单元    */
typedef struct tag_EVTSTATE
{
    OOP_EVTSOURCE_T source;
    uint32          dwCount;
    uint32          dwTimes;
    uint32          tmStart;
    uint32          dbIdx;
}EVTSTATE;

/** @brief 事件时间状态单元 */
typedef struct tag_EVTTIME_BYSRC
{
    OOP_EVTSOURCE_T     source;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    uint32              dbIdx;
}EVTTIME;

#define OOP_MAX_EVTSTAT	     (48)	/**< 最大事件状态数	*/

/** @brief 事件当前值  */
typedef struct tag_OOP_EVTSTAT
{
    uint32      nNum;
    uint32      index;
    EVTSTATE    stat[OOP_MAX_EVTSTAT];
}OOP_EVTSTAT_T;

/** @brief 事件时间状态记录表  */
typedef struct tag_OOP_EVTTIME
{
    uint32      nNum;
    uint32      index;
    EVTTIME     stat[OOP_MAX_EVTSTAT];
}OOP_EVTTIME_T;

/** @brief 事件上报状态   */
typedef struct tag_OOP_REPORTSTATE
{
    OOP_OAD_U       oad;        /**< 通道     */
    uint8           state;      /**< 上报状态   */
}OOP_REPORTSTATE_T;

/** @brief 事件上报状态域    */
typedef struct tag_OOP_REPORTS
{
    uint32              nNum;
    uint32              rsv;
    OOP_REPORTSTATE_T   state[OOP_MAX_REPORTCHANNEL];
}OOP_REPORTS_T;

//事件关联数据特殊处理
#define OOP_MAX_EVTOADS     (36)        /**< 数据记录列关联OOP_OAD_U最大个数         */
/** @brief 一组对象属性描述符(参数)  */
typedef struct tag_OOP_EVTOADS
{
    uint32      nNum;                   /**< 关联对象属性个数           */
    uint32      rsv;
    OOP_OAD_U   oad[OOP_MAX_EVTOADS];       /**< 关联对象属性列表           */
}OOP_EVTOADS_T;

/** @brief 事件关联对象域(可变)    */
typedef struct tag_OOP_EVTBUF
{
    OOP_EVTOADS_T   cols;
    uint16           idLen[OOP_MAX_EVTOADS];
    uint16           len;
    uint8            buf[OOP_EVT_BUF1K];
    uint8           oadNum;
}OOP_EVTBUF_T;

typedef struct tag_OOP_EVTBUF_1K
{
    OOP_OADS_T      cols;
    uint16          idLen[OOP_MAX_OADS];
    uint16          len;
    uint8           buf[OOP_EVT_BUF1K];
    uint8           oadNum;
}OOP_EVTBUF_1K;

/** @brief 事件记录 */
typedef struct tag_OOP_EVENT
{
    uint32              nIndex;         /**< 事件记录序号 */
    OOP_DATETIME_S_T    tmStart;        /**< 事件发生时间 */
    OOP_DATETIME_S_T    tmEnd;          /**< 事件结束时间 */
    OOP_REPORTS_T       state;          /**< 事件上报状态 */
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
}OOP_EVENT_T;

/** @brief 触发事件 */
typedef struct tag_OOP_TOUCHEVENT
{
    OOP_EVTSOURCE_T srce;
    uint16          delaySt;
    uint16          delayEd;
}OOP_TOUCHEVENT_T;

typedef int32 (*pfGetEvtData)(OOP_OAD_U *pOAD, void *pData, OOP_EVTSOURCE_T *pSrce);
typedef int32 (*pfUpdEvtData)(OI obj, OOP_EVENT_T *pData, OOP_EVTSOURCE_T *pSrce);
/*
*********************************************************************
* 4.7-class08-参数变量类
*********************************************************************
*/
typedef struct tag_OOP_WALLET
{
    OOP_EVTSOURCE_T srce;
    uint32          surplus;
    uint32          purchase;
}OOP_WALLET_T;

/** @brief 24规范 Checksum数据类型 */
typedef struct tag_OOP_CHECKSUM
{
    uint8               checktype;    //校验类型0:CRC;1:md5;2:SHA1;3:SM3;255:其他
    OOP_OCTETVAR64_T    checkcode;    //校验值
}OOP_CHECKSUM_T;

/** @brief 24规范  文件校验 */
typedef struct tag_OOP_FILECHECK
{
    uint8  choice;               //choice值说明：[9]oct-string; [98]Checksum
    union
    {
        OOP_OCTETVAR64_T    md5;
        OOP_CHECKSUM_T      checksum;
    };
}OOP_FILECHECK_T;

/*
*********************************************************************
* 4.7-class09-冻结接口类
*********************************************************************
*/

#define OOP_MAX_FROZENOBJ   (64)    /**< 最大冻结对象个数           */

/** @brief 冻结对象定义   */
typedef struct tag_OOP_FROZENOBJ
{
    OOP_OAD_U       oad;
    uint16          nPeriod;
    uint16          nDepth;
}OOP_FROZENOBJ_T;

/** @brief 冻结配置     */
typedef struct tag_OOP_FROZENSETS
{
    uint32          nNum;
    uint32          rsv;
    OOP_FROZENOBJ_T objs[OOP_MAX_FROZENOBJ];
}OOP_FROZENSETS_T;

/** @brief 补冻结配置  */
typedef struct tag_OOP_FROZENTEST
{
    OOP_DATETIME_S_T    start;
    OOP_DATETIME_S_T    end;
}OOP_FROZENTEST_T;

/*
*********************************************************************
* 4.8-class12-脉冲计量接口类
*********************************************************************
*/

#ifdef PRODUCT_ZCU_1
    #if defined (BOARD_801)
    #define OOP_MAX_PULSESETS   (8)    /**< 最大脉冲数            */
    #else
    #define OOP_MAX_PULSESETS   (16)    /**< 最大脉冲数            */
    #endif
#else
    #define OOP_MAX_PULSESETS   (8)     /**< 最大脉冲数            */
#endif

/** @brief PT/CT参数  */
typedef struct tag_OOP_PTCT
{
    uint16  pt;
    uint16  ct;
}OOP_PTCT_T;

/** @brief 脉冲配置 */
typedef struct tag_OOP_PULSECFG
{
    OOP_OAD_U       port;
    uint8           property;
    uint16          K;
}OOP_PULSECFG_T;

/** @brief 一组脉冲配置单元 */
typedef struct tag_OO_PULSESETS
{
    uint32          nNum;
    uint32          rsv;
    OOP_PULSECFG_T  pulse[OOP_MAX_PULSESETS];
}OOP_PULSESETS_T;

/** @brief 脉冲配置(超大) */
typedef struct tag_OOP_PULSECFG_B
{
    OOP_OAD_U       port;
    uint8           property;
    uint32          K;
}OOP_PULSECFG_B_T;

/** @brief 一组脉冲配置单元(超大) */
typedef struct tag_OO_PULSESETS_B
{
    uint32            nNum;
    uint32            rsv;
    OOP_PULSECFG_B_T  pulse[OOP_MAX_PULSESETS];
}OOP_PULSESETS_B_T;

/** @brief 脉冲单位 */
typedef struct tag_OOP_PULSEUNIT
{
    OOP_SCALER_UNIT_T       unit5;
    OOP_SCALER_UNIT_T       unit6;
    OOP_SCALER_UNIT_T       unit7;
    OOP_SCALER_UNIT_T       unit8;
    OOP_SCALER_UNIT_T       unit9;
    OOP_SCALER_UNIT_T       unit10;
    OOP_SCALER_UNIT_T       unit11;
    OOP_SCALER_UNIT_T       unit12;
    OOP_SCALER_UNIT_T       unit13;
    OOP_SCALER_UNIT_T       unit14;
    OOP_SCALER_UNIT_T       unit15;
    OOP_SCALER_UNIT_T       unit16;
    OOP_SCALER_UNIT_T       unit17;
    OOP_SCALER_UNIT_T       unit18;
}OOP_PULSEUNIT_T;

/*
*********************************************************************
* 4.9-class13-控制对象接口类
*********************************************************************
*/

#define OOP_MAX_TG          (8)             /**< 最大总加组对象数量    */

/** @brief 总加组状态  */
typedef struct tag_OOP_TGSTATE
{
    OI      obj;
    uint8   state;
}TGSTATE;

/** @brief 一组总加组  */
typedef struct tag_OOP_TGSTATES
{
    uint32      nNum;
    uint32      rsv;
    TGSTATE     item[OOP_MAX_TG];
}OOP_TGSTATES_T;

/** @brief 轮次状态     */
typedef struct tag_OOP_TURNSTATE
{
    OI              obj;
    OOP_BITSTR8_T   state;
}TURNSTATE;

/** @brief 一组轮次状态   */
typedef struct tag_OOP_TURNSTATES
{
    uint32      nNum;
    uint32      rsv;
    TURNSTATE   item[OOP_MAX_TG];
}OOP_TURNSTATES_T;

/*
*********************************************************************
* 4.10-class14-区间统计接口类
*********************************************************************
*/

#define OOP_MAX_STATIS      (64)    /**< 最大统计记录个数           */

#define OOP_MAX_REGION      (3)     /**< 最大区间个数             */
#define OOP_REGION_OBJS     (8)     /**< 最大支持区间统计对象个数   */

/** @brief 数值类型定义   */
typedef struct tag_OOP_DIGIT
{
    uint8           choice;
    union
    {
        uint8       bVal;
        char        cVal;
        int16       lVal;
        uint16      wVal;
        int32       nVal;
        uint32      dwVal;
        int64       llVal;
        uint64      ullVal;
    };
}OOP_DIGIT_T;

/** @brief 区间类型 */
typedef struct tag_OOP_REGIOND
{
    uint8           nType;
    OOP_DIGIT_T     start;
    OOP_DIGIT_T     end;    
}OOP_REGIOND_T;

typedef struct tag_OOP_DOMAIN_T
{
    uint8               choice; // 0:OOP_DIGIT;1:OOP_REGION
    union
    {
        struct
        {
            uint8       type;
            OOP_DIGIT_T from;
            OOP_DIGIT_T to;
        };
        OOP_DIGIT_T     value;
    };
}OOP_DOMAIN_T;

/** @brief 区间统计对象   */
typedef struct tag_OOP_REGIONOBJ
{
    OOP_OAD_U       obj;
    uint32          nNum;
    OOP_DOMAIN_T    parm[OOP_MAX_REGION];
    uint8           period;
    OOP_TI_T        freq;
}OOP_REGIONOBJ_T;

/** @brief 区间统计值定义    */
typedef struct tag_OOP_REGIONRES
{
    uint32      dwTimes;
    uint32      dwCount;
}OOP_REGIONRES_T;

/** @brief 区间统计结果集合 */
typedef struct tag_OOP_REGION
{
    OOP_OAD_U       obj;
    uint32          nNum;
    OOP_REGIONRES_T value[OOP_MAX_REGION+1];
}OOP_REGION_T;

/** @brief 区间统计结果集合 */
typedef struct tag_OOP_REGIONSS
{
    uint32          nNum;
    uint32          rsv;
	OOP_REGION_T    regions[19];
}OOP_REGIONSS_T;


/** @brief 区间统计对象集合 */
typedef struct tag_OOP_REGIONS
{
    uint32          nNum;
    uint32          rsv;
    OOP_REGIONOBJ_T objs[OOP_REGION_OBJS];
}OOP_REGIONS_T;

/*
*********************************************************************
* 4.11-class15-累加平均接口类
*********************************************************************
*/

/** @brief 累计平均值定义    */
typedef struct tag_OOP_TOTALAVG
{
    OOP_OAD_U       obj;
    float64         dbTotal;
    float64         dbAvg;
}OOP_TOTALAVG_T;

/** @brief 累加平均关联对象 */
typedef struct tag_OOP_COGNATEOBJ
{
    OOP_OAD_U       obj;
    uint8           period;
    OOP_TI_T        freq;
}OOP_COGNATEOBJ_T;

#define OOP_COGNATE_OBJS    (16)     /**< 最大支持累加平均对象个数           */

/** @brief 累加平均关联对象集合   */
typedef struct tag_OOP_COGNATES
{
    uint32              nNum;
    uint32              rsv;
    OOP_COGNATEOBJ_T    objs[OOP_COGNATE_OBJS];
}OOP_COGNATES_T;

/** @brief 累计平均值定义    */
typedef struct tag_OOP_TOTALAVGS
{
    uint32              nNum;
    uint32              rsv;
    OOP_TOTALAVG_T      totalavg[OOP_COGNATE_OBJS];
}OOP_TOTALAVGS_T;

/*
*********************************************************************
* 4.12-class16-极值接口类
*********************************************************************
*/

/** @brief 极值统计结果   */
typedef struct tag_OOP_EXTREMERES
{
    OOP_OAD_U           obj;
    OOP_DIGIT_T         dbMaxValue;
    OOP_DATETIME_S_T    tmMax;
    OOP_DIGIT_T         dbMinValue;
    OOP_DATETIME_S_T    tmMin;
}OOP_EXTREMERES_T;

/** @brief 极值统计结果   */
typedef struct tag_OOP_EXTREMERESS
{
    uint32              nNum;
    uint32              rsv;

	OOP_EXTREMERES_T   extremvalue[OOP_COGNATE_OBJS];

}OOP_EXTREMERESS_T;


/*
*********************************************************************
* 4.13-class18-文件传输接口类
*********************************************************************
*/

/*
*********************************************************************
* 4.14-class19-设备管理接口类
*********************************************************************
*/

#define OOP_MAX_SUBDEVICE   (8)         /**< 最大子设备个数          */
#define OOP_MAX_PROTOCOL    (8)         /**< 最大支持协议数          */
#define OOP_MAX_SERVICES    (8)         /**< 最大支持服务数          */

/** @brief 支持的协议  */
typedef struct tag_OOP_PROTOCOL
{
    uint32              nNum;
    uint32              rsv;
    OOP_VISIBLEVAR_T    value[OOP_MAX_PROTOCOL];
}OOP_PROTOCOL_T;

/** @brief 上报通道 */
typedef struct tag_OOP_CHANNEL
{
    uint32      nNum;
    uint32      rsv;
    OOP_OAD_U   port[OOP_MAX_REPORTCHANNEL];
}OOP_CHANNEL_T;

/** @brief 子设备    */
typedef struct tag_OIS
{
    uint32  nNum;
    uint32  rsv;
    OI      oi[OOP_MAX_SUBDEVICE];
}OOP_OIS_T;

/** @brief 服务授权状态   */
typedef struct tag_OOP_SERVICE
{
    uint8   type;
    uint8   times;
}OOP_SERVICE_T;

/** @brief 一组服务授权状态 */
typedef struct tag_OOP_SERVICES
{
    uint32          nNum;
    uint32          rsv;
    OOP_SERVICE_T   item[OOP_MAX_SERVICES];
}OOP_SERVICES_T;

/** @brief 设备运行信息 */
typedef struct tag_OOP_DEVICERUNSTAT
{
    uint8               cpuCoreNum;     /**< cpu核数 */ 
    uint16              cpuMainFrq;     /**< cpu主频（单位：GHz，换算：-2） */
    uint16              cpuCache;       /**< cpu缓存（单位：MB，换算：-2） */
    OOP_VISIBLEVAR_T    cpuArchi;       /**< cpu架构 */
    uint32              phyMem;         /**< 物理内存（单位：MB，换算：-2） */
    uint32              virMem;         /**< 虚拟内存（单位：KB，换算：0） */
    uint32              diskSpace;      /**< 磁盘总空间（单位：KB，换算：0） */
    uint16              memUsage;       /**< 内存使用率（单位：%，换算：-2） */
    uint16              diskUsage;      /**< 磁盘使用率（单位：%，换算：-2） */
    uint16              cpuUsage;       /**< CPU使用率（单位：%，换算：-2） */
    OOP_VISIBLEVAR_T    osName;         /**< 操作系统名称 */
    OOP_VISIBLEVAR_T    osVer;          /**< 操作系统版本 */
    OOP_VISIBLEVAR_T    osKernel;       /**< 操作系统内核 */
    OOP_DATETIME_S_T    sysStrtm;       /**< 系统启动时间 */
    int16               tempBroad;      /**< 主板温度 */
}OOP_DEVICERUNSTAT_T;

/** @brief 设备应用信息 */
typedef struct tag_OOP_DEVICEFILE
{
    OOP_VISIBLEVAR_T	filename;
    OOP_VISIBLEVAR_T	filepath;
    uint32				filesize;
    uint8				filecrc;
    OOP_OCTETVAR64_T    filecontent;
}OOP_DEVICEFILE_T;

typedef struct tag_OOP_DEVICEFILES
{
    uint32			    nNum;
    uint32			    rsv;
    OOP_DEVICEFILE_T	item[OOP_MAX_SERVICES];
}OOP_DEVICEFILES_T;

/*
*********************************************************************
* 4.15-class20-应用连接接口类
*********************************************************************
*/

/** @brief 版本信息     */
typedef struct tag_OOP_VERSION
{
    OOP_VISIBLESTR4_T   szFactory;  /**< 厂商代码           */
    OOP_VISIBLESTR4_T   szSoftVer;  /**< 软件版本号            */
    OOP_VISIBLESTR6_T   szSoftDate; /**< 软件版本日期     */
    OOP_VISIBLESTR4_T   szHardVer;  /**< 硬件版本号            */
    OOP_VISIBLESTR6_T   szHardDate; /**< 硬件版本日期     */
    OOP_VISIBLESTR8_T   szExtend;   /**< 厂家扩展信息     */
}OOP_VERSION_T;

/** @brief 版本信息 厂家扩展     */
typedef struct tag_OOP_VERSION_S
{
    OOP_VISIBLESTR4_T   szFactory;  /**< 厂商代码           */
    OOP_VISIBLESTR4_T   szSoftVer;  /**< 软件版本号            */
    OOP_VISIBLESTR6_T   szSoftDate; /**< 软件版本日期     */
    OOP_VISIBLESTR4_T   szHardVer;  /**< 硬件版本号            */
    OOP_VISIBLESTR6_T   szHardDate; /**< 硬件版本日期     */
    OOP_VISIBLESTR8_T   szExtend;   /**< 厂家扩展信息     */

    //以下厂家设置过的版本信息
    OOP_VISIBLESTR4_T   szFactoryS;  /**< 厂商代码           */
    OOP_VISIBLESTR4_T   szSoftVerS;  /**< 软件版本号            */
    OOP_VISIBLESTR6_T   szSoftDateS; /**< 软件版本日期     */
    OOP_VISIBLESTR4_T   szHardVerS;  /**< 硬件版本号            */
    OOP_VISIBLESTR6_T   szHardDateS; /**< 硬件版本日期     */
    OOP_VISIBLESTR8_T   szExtendS;   /**< 厂家扩展信息     */
}OOP_VERSION_S_T;


/** @brief 应用连接请求   */
typedef struct tag_ConnectRequest
{
    uint16                      VER;
    uint8                       PCB[8];
    uint8                       FCB[16];
    uint16                      nSendPDUSize;
    uint16                      nRecvPDUSize;
    uint8                       nPDUWindow;
    uint16                      nMaxPDUSize;
    uint32                      nTimeout;
    OOP_SECURITYMECHANISMINFO_T szSecurity;
}OOP_CONNECTREQUEST_T;

/** @brief 应用连接请求应答 */
typedef struct tag_ConnectResponse
{
    OOP_VERSION_T               info;
    uint16                      VER;
    uint8                       PCB[8];
    uint8                       FCB[16];
    uint16                      nSendPDUSize;
    uint16                      nRecvPDUSize;
    uint8                       nPDUWindow;
    uint16                      nMaxPDUSize;
    uint32                      nTimeout;
    OOP_SECURITYRESPONSEINFO_T  szResult;
}OOP_CONNECTRESPONSE_T;

/** @brief 应用连接上下文信息  */
typedef struct tag_Association_Context
{
    uint16  VER;                /**< 协议版本               */
    uint16  nRecvSize;          /**< 最大接收APDU尺寸单元   */
    uint16  nSendSize;          /**< 最大发送APDU尺寸单元   */
    uint16  nMaxPDUSize;        /**< 最大可处理APDU尺寸      */
    uint8   PCB[8];             /**< 协议一致性块         */
    uint8   FCB[16];            /**< 功能一致性块         */
    uint32  nTimeout;           /**< 静态超时时间         */
}OOP_ASSOCIATION_CONTEXT_T;

/** @brief 属性的访问权限枚举  */
typedef enum tag_Attribute_Access
{
    NOACCESS  = 0,
    READ      = 1,
    WRITE     = 2,
    READWRITE = 3,
    NOOBJECT  = 0x10
}OOP_ATTRIBUTE_ACESS_E;

/** @brief 方法访问权限定义     */
typedef enum tag_Action_Access
{
    NOAUTHORITY = 0,
    ACCESS      = 1
}OOP_ACTION_ACCESS_E;

/** @brief 一个属性访问权限     */
typedef struct tag_Attribute_Access_Security
{
    uint8   index;
    uint8   rw;
}OOP_AAS_T;

/** @brief 一个方法的访问权限  */
typedef struct tag_Method_Access_Security
{
    uint8   index;
    BOOLEAN exec;
}OOP_MAS_T;

/** @brief 一组属性的访问权限  */
typedef struct tag_Aar_Access_Right
{
    uint32             nNum;
    uint32             rsv;
    OOP_AAS_T          aar[128];
}OOP_AAR_ACCESS_RIGHT;

/** @brief 一组方法的访问权限  */
typedef struct tag_Mar_Access_Right
{
    uint32             nNum;
    uint32             rsv;
    OOP_MAS_T          mar[128];
}OOP_MAR_ACCESS_RIGHT;

/** @brief 访问权限 */
typedef struct tag_Access_Right
{
    OOP_AAR_ACCESS_RIGHT aars;
    OOP_MAR_ACCESS_RIGHT mars;
}OOP_ACCESS_RIGHT;

/** @brief 一个可访问对象 */
typedef struct tag_Oi_Right
{
    OI               oi;
    OOP_ACCESS_RIGHT accessRight; 
}OOP_OI_RIGHT_T;

/** @brief 一组可访问对象 */
typedef struct tag_Oi_Rights
{
    uint32             nNum;
    uint32             rsv;
    OOP_OI_RIGHT_T     oiRight[2048];
}OOP_OI_RIGHTS_T;

/*
*********************************************************************
* 4.16-class21-ESAM接口类
*********************************************************************
*/

#define OOP_MAX_OI  (32)        /**< 最大对象个数 */

/** @brief 证书版本信息   */
typedef struct tag_OOP_LICENSEVER
{
    OOP_OCTETVAR16_T  data1;        /**< 终端证书版本 */
    OOP_OCTETVAR16_T  data2;        /**< 主站证书版本     */
}OOP_LICENSEVER_T;

/** @brief 秘钥更新信息   */
typedef struct tag_KEY_INFO
{
    uint8    nNum;              /**< 密钥总条数                                */
    uint8    nCurNum;           /**< 当前发送的密钥条数                        */
    uint8    nLen;              /**< 每条密钥长度                         */
    uint8    nFlag;             /**< 密钥状态标志位：00测试密钥，01正式密钥    */
}OOP_KEY_INFO_T;

/** @brief SID-OOP_MAC_T数据验证码 */
typedef struct tag_SID_MAC
{
    OOP_SSID_T            sid;      /**< 安全标识       */
    OOP_MAC_T             mac;      /**< 数据OOP_MAC_T    */
}OOP_SID_MAC_T;

/** @brief 当前计数器 */
typedef struct tag_CUR_COUNT_T
{
    uint32                asctrCnt;        /**< 应用协商计数器       */
    uint32                asctrReportCnt;  /**< 主动上报计数器       */
    uint32                agseq;           /**< 广播通信序列号       */
}OOP_CUR_COUNT_T;

/** @brief ESAM方法7-秘钥更新 */
typedef struct tag_OOPM_ESAM_7
{
    OOP_OCTETVAR2K_T    data;       /**< 密钥密文   */
    OOP_SID_MAC_T       sidMac;
}OOPM_ESAM_7_T;

/** @brief ESAM方法8-证书更新 */
typedef struct tag_OOPM_ESAM_8
{
    OOP_OCTETVAR2K_T    data;       /**< 明文数据   */
    OOP_SSID_T          sid;        /**< 安全标识   */
}OOPM_ESAM_8_T;

/** @brief ESAM方法13-内部认证    */
typedef struct tag_OOPM_ESAM_13
{
    OOP_OCTETVAR2K_T R4;            /**< 随机数密文 */
    OOP_OCTETVAR2K_T R5;            /**< 随机数 */
}OOPM_ESAM_13_T;

/** @brief ESAM方法11-红外认证请求(应答)  */
typedef struct tag_OOPM_ESAM_11_ACK
{
    OOP_OCTETVAR16_T  metNum;       /**< 表号 */
    OOP_OCTETVAR256_T esamSeq;      /**< ESAM序列号 */   
    OOP_OCTETVAR256_T enData1;      /**< 随机数1密文 */
    OOP_RN_T          rand2;        /**< 随机数2 */
}OOPM_ESAM_11_ACK_T;

/** @brief ESAM方法14-外部认证	*/
typedef struct tag_OOPM_ESAM_14
{
    OOP_OCTETVAR2K_T  R6;			/**< 随机数	*/
}OOPM_ESAM_14_T;

/*
*********************************************************************
* 4.17-class22-输入输出接口类
*********************************************************************
*/

#define OOP_MAX_COM             (8)     /**< 最大串口数量                     */
#define APP_NUM_MAX             (64)    /**< 最大APP数量                        */

/** @brief 端口授权 */
typedef struct tag_OOPM_ADDPORT
{
    uint8       index;      /**< 端口序号   */
    uint8       times;      /**< 开启时长   */
}OOPM_ADDPORT_T;

/** @brief 端口授权状态   */
typedef struct tag_OOP_PORTS
{
    uint32      nNum;
    uint32      rsv;
    uint8       item[OOP_MAX_COM];
}OOP_PORTS_T;

/** @brief APP服务组件信息    */
typedef struct tag_OOP_REG_APP_INFO
{
    OOP_VISIBLEVAR_T  appName;                  //组件名称
    OOP_VISIBLEVAR_T  version;                  //版本信息
    OOP_DATETIME_S_T  dataTime;                 //发布日期
    OOP_OCTETVAR256_T appID;                    //APP_ID    
}OOP_REG_APP_INFO_T;

/** @brief ARRAY-APP服务组件信息  */
typedef struct tag_OOP_REG_APP_INFOS
{
    uint32             nNum;
    uint32             rsv;
    OOP_REG_APP_INFO_T item[APP_NUM_MAX];
}OOP_REG_APP_INFOS_T;

/*
*********************************************************************
* 4.18-class23-总加组接口类
*********************************************************************
*/

#define OOP_MAX_TGSETS  (16)        /**< 最大总加组个数  */

/** @brief 总加组配置单元    */
typedef struct tag_OOP_TG
{
    OOP_TSA_T   add;
    uint8       tgFlag;
    uint8       opFlag;
}OOP_TG_T;

/** @brief 总加组配置  */
typedef struct tag_OOP_TGSETS
{
    uint32      nNum;
    uint32      rsv;
    OOP_TG_T    tg[OOP_MAX_TGSETS];
}OOP_TGSETS_T;

/** @brief 总加组控制设置状态  */
typedef struct tag_OOP_TGSETTING
{
    uint8               idxPeriod;
    OOP_BITSTR8_T       pcFlag;
    OOP_BITSTR8_T       pcState;
    OOP_BITSTR8_T       ecState;
    OOP_BITSTR8_T       pcTurn;
    OOP_BITSTR8_T       ecTurn;
}OOP_TGSETTING_T;

/** @brief 总加组当前状态    */
typedef struct tag_OOP_TGCURSTATE
{
    int64               pwSetting;
    int8                pwFall;
    OOP_BITSTR8_T       pcTrip;
    OOP_BITSTR8_T       mecTrip;
    OOP_BITSTR8_T       becTrip;
    OOP_BITSTR8_T       pcWarn;
    OOP_BITSTR8_T       ecWarn;
}OOP_TGCURSTATE_T;

/** @brief 总加组临时限电控状态    */
typedef struct tag_OOP_TMPCURSTATE
{
    uint8               effect;   /* 投入状态 */
    OOP_BITSTR8_T       pcTurn;   /* 轮次状态 */
    int64               setVaule; /* 当前临时限电控定值 */
    int                 floatCoe; /* 浮动系数 */
    OOP_BITSTR8_T       trip;     /* 跳闸状态 */
    OOP_BITSTR8_T       warn;     /* 告警状态 */
}OOP_TMPCURSTATE_T;


/** @brief 总加组单位及换算 */
typedef struct tag_OOP_TGUNIT
{
    OOP_SCALER_UNIT_T       unit3;
    OOP_SCALER_UNIT_T       unit4;
    OOP_SCALER_UNIT_T       unit5;
    OOP_SCALER_UNIT_T       unit6;
    OOP_SCALER_UNIT_T       unit7;
    OOP_SCALER_UNIT_T       unit8;
    OOP_SCALER_UNIT_T       unit9;
    OOP_SCALER_UNIT_T       unit10;
    OOP_SCALER_UNIT_T       unit11;
    OOP_SCALER_UNIT_T       unit12;
}OOP_TGUNIT_T;

/*
*********************************************************************
* 4.19-class24-分相事件对象接口类
*********************************************************************
*/

/** @brief 分相事件状态定义 */
typedef struct tag_MEVTSTATE
{
    uint32          dwCount;
    uint32          dwTimes;
    uint32          tmStart;
    uint32          dbIdx;  
    uint32          index;      /**< 分相事件序号 */
}MEVTSTATE;

/** @brief 分相事件统计值定义  */
typedef struct tag_MEVTTIME
{
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    uint32              dbIdx;
    uint32              index;  /**< 分相事件序号 */
}MEVTTIME;

/** @brief 分相事件记录   */
typedef struct tag_OOP_MEVENT
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       state;
    OOP_EVTBUF_T        data;
}OOP_MEVENT_T;

#define OOP_MAX_MEVTSTAT        (4)     /**< 最大多项事件状态个数 */

/** @brief 分相事件状态   */
typedef struct tag_OOP_MEVTSTAT
{
    uint32            nNum;
    uint32            index;
    MEVTSTATE         stat[OOP_MAX_MEVTSTAT];
}OOP_MEVTSTAT_T;

/** @brief 分相事件统计值    */
typedef struct tag_OOP_MEVTTIME
{
    uint32           nNum;
    uint32           index;
    MEVTTIME         stat[OOP_MAX_MEVTSTAT];
}OOP_MEVTTIME_T;

/** @brief 触发分相事件   */
typedef struct tag_OOP_TOUCHMEVENT
{
    uint8       type;
    uint16      delaySt;
    uint16      delayEd;
}OOP_TOUCHMEVENT_T;

typedef int32 (*pfGetMEvtData)(OOP_OAD_U *pOAD, void *pData);
typedef int32 (*pfUpdMEvtData)(OI obj, OOP_MEVENT_T *pData);

/*
*********************************************************************
* 4.20-class25-公网通信接口类
*********************************************************************
*/

#define OOP_MAX_GPRSOBJ         (2)             /**< 最大无线公网模块个数 */
#define OOP_MAX_HOST            (4)             /**< 最大可连接主站数       */
#define OOP_MAX_LISTENS         (4)             /**< 最大帧听端口数          */

/** @brief 工作模式 */
typedef enum tag_WORKMODE
{
    MODE_MIX    = 0,    /**< 混合模式       */
    MODE_CLIENT,        /**< 客户机模式    */
    MODE_SERVER,        /**< 服务器模式    */
    MODE_MQTT           /**< MQTT模式     */
}OOP_WORKMODE_E;

/** @brief 连接方式 */
typedef enum tag_CONNECTTYPE
{
    TYPE_TCP    = 0,    /**< TCP    */
    TYPE_UDP            /**< UDP    */
}OOP_CONNECTTYPE_E;

/** @brief 在线方式 */
typedef enum tag_LINETYPE
{
    LINE_FOREVER    = 0,    /**< 永久在线   */
    LINE_PASSIVE            /**< 被动激活   */
}OOP_LINETYPE_E;

/** @brief 连接应用方式   */
typedef enum tag_CONNECTMODE
{
    MODE_ZB = 0,        /**< 主备模式   */
    MODE_DLJ            /**< 多连接模式    */
}OOP_CONNECTMODE_E;

/** @brief 超时时间与重播次数  */
typedef union tag_TIMEDAIL
{
    uint8       body;
    struct
    {
        uint8   dailcnt  : 2;           /**< 重发次数D1-D0  */
        uint8   dailtime : 6;           /**< 超时时间D7-D2  */
    }mask;
}OOP_TIMEDAIL_U;

/** @brief APN参数    */
typedef struct tag_OOP_APN
{
    OOP_VISIBLEVAR_T    apn;                        /**< APN            */
    OOP_VISIBLEVAR_T    username;                   /**< 用户名          */
    OOP_VISIBLEVAR_T    pwd;                        /**< 密码         */
    uint8               proxyip[4];                 /**< 代理服务器地址  */
    uint16              proxyport;                  /**< 代理端口       */
}OOP_APN_T;

/** @brief GPRS通信配置 */
typedef struct tag_OOP_GPRSCONFIG
{
    uint8           workmode;                   /**< 工作模式           */
    uint8           linetype;                   /**< 在线方式           */
    uint8           contype;                    /**< 连接方式           */
    uint8           conmode;                    /**< 连接应用方式     */
    uint32          listenNum;                  /**< 侦听端口个数     */
    uint16          listen[OOP_MAX_LISTENS];    /**< 帧听端口列表     */
    OOP_APN_T       apn;
    OOP_TIMEDAIL_U  dail;                       /**< 超时时间及重发次数    */
    uint16          heart;                      /**< 心跳周期(秒)      */
}OOP_GPRSCONFIG_T;

/** @brief 短信号码定义   */
typedef struct tag_OOP_SMS_HOST
{
    uint32              nNum;
    OOP_VISIBLESTR16_T  item[OOP_MAX_HOST];
}OOP_SMS_HOST_T;

/** @brief 短信参数 */
typedef struct tag_OOP_SMS
{   
    char            centernum[17];      /**< 短信中心号码     */
    OOP_SMS_HOST_T  hostnum;            /**< 主站号码           */
    OOP_SMS_HOST_T  localnum;           /**< 短信通知目的号码   */
}OOP_SMS_T;

#define OOP_MAX_MASTER  (4)         /**< 最大连接主站数  */
#define OOP_MAX_NETTYPE (16)         /**< 最大网络类型 */

/** @brief 主站参数 */
typedef struct tag_OOP_MASTERPARAM
{
    uint8           ip[4];          /**< IP地址   */
    uint16          port;           /**< 端口 */
}OOP_MASTERPARAM_T;

/** @brief 主站参数集合   */
typedef struct tag_OOP_MASTERPARAMS
{
    uint32              nNum;
    uint32              rsv;
    OOP_MASTERPARAM_T   master[OOP_MAX_MASTER];
}OOP_MASTERPARAMS_T;

/** @brief 运营商信息  */
typedef enum tag_OOP_NETOPT
{
    OOP_CMCC = 0,
    OOP_CTCC = 1,
    OOP_CUCC = 2,
    OOP_OPT_NONE = 255
}OOP_NETOPT_E;

/** @brief 网络类型枚举   */
typedef enum tag_OOP_NETTYPE
{
    OOP_2G = 0,
    OOP_3G = 1,
    OOP_4G = 2,
    OOP_5G = 3,
    OOP_NET_UNKNOW = 255
}OOP_NETTYPE_E;

/** @brief 鉴权方式 */
typedef enum tag_OOP_AUTH
{
    AUTH_NULL = 0,
    AUTH_CHAP = 1,
    AUTH_PAP = 2,
    AUTH_PAPCHAP = 3,
    AUTH_UNKNOW = 255
}OOP_AUTH_E;

/** @brief 无线公网网络配置 */
typedef struct tag_OOP_NETCONFIG
{
    uint8               factory;                    /**< 运营商      */
    uint8               network;                    /**< 网络类型   */
    OOP_APN_T           apn;                        /**< APN 参数 */
    uint8               auth;                       /**< 鉴权方式   */
    OOP_MASTERPARAMS_T  master;                     /**< 主站参数   */
}OOP_NETCONFIG_T;

/** @brief 无线公网多网络配置  */
typedef struct tag_OOP_APNGROUP
{
    uint32          nNum;
    uint32          resv;
    OOP_NETCONFIG_T net[OOP_MAX_NETTYPE];
}OOP_APNGROUP_T;

/** @brief 运营商及网络制式 */
typedef struct tag_OOP_NETINFO
{
    uint8   nNetOpt;
    uint8   nNetType;
}OOP_NETINFO_T;

/** @brief 网络状态信息 */
typedef struct tag_OOP_NETST
{
    int8     RSRP;
    int8     SINR;
    uint16   PCI;
    uint16   ERFCN;
}OOP_NETST_T;

/** @brief 网络类型枚举   */
typedef enum tag_OOP_NETCHOOSE
{
    OOP_PRIVATE_NET_FIRST = 0, /* 优先专网模式 */
    OOP_PUBLIC_NET_ONLY,       /* 固定公网模式 */
    OOP_PRIVATE_NET_ONLY,      /* 固定专网模式 */
    OOP_PUBLIC_NET_FIRST,      /* 优先公网模式 */
    OOP_LOCAL_NET_CHOOSE,      /* 本地切换模式 */
    
    OOP_NET_CHOOSE_NULL   =255,
}OOP_NETCHOOSE_E;

/** @brief 写卡模式   */
typedef enum tag_OOP_WRITE_SIM
{
    OOP_WRITE_SIM_OP   = 0,
    OOP_WRITE_SIM_OPC,

    OOP_WRITE_SIM_NULL =255,
}OOP_WRITE_SIM_E;

/** @brief 专网软号 */
typedef struct tag_OOP_PRIVATENET
{
    OOP_VISIBLEVAR_T     IMSI;
    OOP_VISIBLEVAR_T     KI;
    uint8                writetype;
    OOP_VISIBLEVAR_T     writevalue;
    uint8                MNC_len;
}OOP_PRIVATE_NET_T;

/** @brief 模块信息 */
typedef struct tag_OOP_MODEL_INFO
{
    OOP_VISIBLESTR4_T       szFactory;  /**< 厂商代码           */
    OOP_VISIBLESTRVAR16_T   sztype;     /**< 模块型号            */
    OOP_VISIBLEVAR_T        IMEI;
    OOP_VISIBLESTRVAR16_T   szSoftVer;  /**< 软件版本号            */
}OOP_MODEL_INFO_T;


/*
*********************************************************************
* 4.21-class26-以太网通信接口类
*********************************************************************
*/

#define OOP_MAX_ETHEROBJ        (8)             /**< 最大以太网个数      */

/** @brief 以太网通信配置    */
typedef struct tag_OOP_ETHCONFIG
{
    uint8           workmode;                   /**< 工作模式           */
    uint8           contype;                    /**< 连接方式           */
    uint8           conmode;                    /**< 连接应用方式     */
    uint32          listenNum;                  /**< 侦听端口个数     */
    uint16          listen[OOP_MAX_LISTENS];    /**< 帧听端口列表     */
    uint8           proxyip[4];                 /**< 代理服务器地址      */
    uint16          proxyport;                  /**< 代理端口           */
    OOP_TIMEDAIL_U  dail;                       /**< 超时时间及重发次数    */
    uint16          heart;                      /**< 心跳周期(秒)      */
}OOP_ETHCONFIG_T;

/** @brief 以太网本地IP配置  */
typedef struct tag_OOP_ETHIP
{
    uint8               ipConfig;
    uint8               ip[4];
    uint8               mask[4];
    uint8               ipGateway[4];
    OOP_VISIBLEVAR_T    pppoeUser;
    OOP_VISIBLEVAR_T    pppoePwd;
}OOP_ETHIP_T;

/** @brief IP地址 */
typedef struct tag_OOP_IP
{
    uint8    data[4];
}OOP_IP_T;

/** @brief 以太网OOP_MAC_T地址 */
typedef struct tag_OOP_ETHMAC
{
    uint8    data[6];
}OOP_ETHMAC_T;

/*
 ********************************************************************
 *                                                                  *
 * 5-附录对象数据结构                                               *
 *                                                                  *
 ********************************************************************
*/

/*
*********************************************************************
* A.4 变量类对象
*********************************************************************
*/

/** @brief double-long-unsigned double  */
typedef struct tag_OOP_DWORD2
{
    uint32  nValue1;
    uint32  nValue2;
}OOP_DWORD2_T;

/** @brief double-long-unsigned three   */
typedef struct tag_OOP_DWORD3
{
    uint32   nValue1;
    uint32   nValue2;
    uint32   nValue3;
}OOP_DWORD3_T;

#define OOP_METWORD_NUM     (7)                 /**< 电表状态字个数  */

/** @brief 2014-电能表运行状态字    */
typedef struct tag_OOP_METWORD
{
    uint32          nNum;   
    uint32          rsv;
    uint16          metWord[OOP_METWORD_NUM];
}OOP_METWORD_T;

/** @brief 2050-电流回路状态 */
#define OOP_LOOPSTAT_NUM    (3)
typedef struct tag_OOP_LOOPSTAT
{
    uint32          nNum;
    uint32          rsv;
    uint8           loopstat[OOP_LOOPSTAT_NUM];
}OOP_LOOPSTAT_T;

/** 
* @brief    电压合格率统计值-   \n
*           2130-总电压合格率 \n
*           2131-A相电压合格率    \n
*           2132-B相电压合格率    \n
*           2133-C相电压合格率
*/
typedef struct tag_OOP_VOLTQR
{
    uint32  dwTime1;
    uint16  wRate1;
    uint16  wRate2;
    uint32  dwTime3;
    uint32  dwTime4;
}OOP_VOLTQR_T;

/** @brief 电压合格率统计    */
typedef struct tag_OOP_VOLTSTAT
{
    OOP_VOLTQR_T    day;
    OOP_VOLTQR_T    mon;
}OOP_VOLTSTAT_T;

/** @brief 2204-复位次数    */
typedef struct tag_OOP_WORD2
{
    uint16  nValue1;    /**< 日复位累计次数          */
    uint16  nValue2;    /**< 月复位累计次数          */
}OOP_WORD2_T;

/** @brief 2506-(仪表)状态ST    */
typedef struct tag_OOP_ST
{
    uint8   nTap;       /**< 阀门状态(enum) */
    uint8   nBattery;   /**< 电池电压(enum) */
}OOP_ST_T;

/** @brief 2600- 温度 */
#define OOP_TEMPERATURE_NUM	(1)
typedef struct tag_OOP_TEMPERATURE
{
    uint32			nNum;	
    uint32			rsv;
    int16           val[OOP_TEMPERATURE_NUM];
}OOP_TEMPERATURE_T;

typedef struct tag_OOP_HUMIDITY
{
    uint32			nNum;	
    uint32			rsv;
    uint16           val[OOP_TEMPERATURE_NUM];
}OOP_HUMIDITY_T;

/** @brief 26F0-虚遥信 */
typedef struct tag_OOP_VIRTUAL_YX
{
    uint32   di;                   /**< 数据项标识 */ 
    OOP_OCTETVAR256_T  data;        /**< 遥信记录数据 */
}OOP_VIRTUAL_YX_T;

/** @brief 2800-当前剩余电流最大相及剩余电流值*/
typedef struct tag_OOP_RESIVALUE
{
    uint8 nPhase;            /**> 剩余电流最大相         */
    int16 nValue;            /**> 剩余电流值               */
}OOP_RESIVALUE_T;

/** @brief 2801-当前剩余电流动作值及驱动时间*/
typedef struct tag_OOP_RESIACTION
{
    int32   nAmp;            /**>剩余电流动作值*/
    uint16  nTime;           /**>极限不驱动时间*/
}OOP_RESIACTION_T;

/** @brief 2802-剩余电流日最大相，最大值及发生的时刻*/
typedef struct tag_OOP_RESICUR
{
    uint8               nMaxPhase;      /**> 剩余电流最大相 */
    int16               nMaxValue;      /**> 剩余电流最大值 */
    OOP_DATETIME_S_T    tmHappen;       /**> 发生时刻           */
}OOP_CURMAXPHASE_T;

/** @brief 2805-电流日最大值发生时刻及三相电流值*/

#define OOP_MAX_AMPTIMEVAL	3

typedef struct tag_OOP_CURMAX
{
    OOP_DATETIME_S_T    tmHappen;   /**>分相电流日最大发生时刻*/
    uint32              nAmpA;      
    uint32              nAmpB;
    uint32              nAmpC;  
}OOP_CURMAX_T;

typedef struct tag_OOP_AMPTIMEVAL
{
    uint32              nNum;
    uint32              rsv;
    OOP_CURMAX_T        nVal[OOP_MAX_AMPTIMEVAL];
}OOP_AMPTIMEVAL_T;

/** @brief 280A-跳闸次数参数模块*/
typedef struct tag_OOP_TRIPCNT
{
    /**< 总跳闸次数                                
         闭锁性跳闸次数                          
         剩余电流保护跳闸次数                         
         电流保护次数（过载、短路）                
         电压保护次数（缺相、欠压、过压）   
         手动闭锁跳闸次数                           
         缺零保护跳闸次数                           
         试验跳闸次数（定时、远程、按键）   
         短路短延时跳闸次数                            
         短路瞬时跳闸次数                           */
    uint16 nCnt[10];
    
    /**< 剩余电流超限新增次数                         
         线路缺相新增次数                           
         剩余电流保护退出新增次数                   
         出线端失电新增次数                    
         剩余电流采样回路断线新增次数                     */
    uint8 nNewCnt[5];
}OOP_TRIPCNT_T;

/** @brief 2E20-（传感器）温湿度*/
typedef struct tag_OOP_ENV
{
    int32	temp;		// 温度
    uint32	humidity;	// 湿度
} OOP_ENV_T;

/** @brief 4B02-断路器控制字参数模块*/
#define OOP_RESWORD_NUM		(5)	
typedef struct tag_OOP_CONWORD
{
    /**< 控制字1、2、3、4、5*/
    uint32			nNum;	
    uint32			rsv;
    uint8           rescntWord[OOP_RESWORD_NUM];
}OOP_CONWORD_T;

/** @brief 4B03-定时自检整定时间*/
typedef struct tag_OOP_CHKSETTM
{
    uint8 day;
    uint8 hour;
    uint8 minute;
}OOP_CHKSETTM_T;

/** @brief 4B04-电压整定参数模块*/
typedef struct tag_OOP_VOLSET
{
    uint16 overvol;		// 过电压整定值
    uint16 undevol;		// 欠电压整定值
    uint16 fauphavol;	// 断相电压整定值
}OOP_VOLSET_T;

/** @brief 4B05-电流整定参数模块*/
typedef struct tag_OOP_AMPSET
{
    uint32 overamp;			// 额定电流整定值过载保护动作电流Ir1
    uint8 AmpOverAlm;		// 负荷电流超限报警整定值(﹡0.1In)（弃用）
    uint8 overtm;			// 过载保护动作时间t1
    uint8 stciramp;			// 短路短延时动作电流Ir2(*Ir1)
    uint8 stcirtm;			// 短路短延时动作时间t2
    uint8 insamp;			// 短路瞬时电流Ir3(*Ir1)
}OOP_AMPSET_T;

/** @brief 280C-互感器电压合格率日统计数据*/
typedef struct tag_OOP_VOLTSTIC
{
    uint32      nTotaltime;             /**> 电压监测时间             */
    uint32      nSval;                  /**> 电压合格率                */
    uint32      nOval;                  /**> 电压越限率                */
    uint32      nOverTime;              /**> 超上限时间                */
    uint32      nLowTime;               /**> 超下限时间                */
    uint16      nHighVol;               /**> 最高电压               */
    OOP_DATE_T  stHighTime;             /**> 最高电压出现时刻           */
    uint16      nLowVol;                /**> 最低电压               */
    OOP_DATE_T  stLowTime;              /**> 最低电压出现时刻           */
}OOP_VOLTSTIC_T;

/** @brief 2903-电容器数据*/
typedef struct tag_OOP_CAPSTATE
{
    uint8         type;             /**> 电容类型:共补(1) 分补(2)         */
    OOP_WORD4V_T  vol;              /**> 电压   */
    OOP_INT4V_T   amp;              /**> 电流 */
    OOP_INT4V_T   ppwr;             /**> 有功功率 */
    OOP_INT4V_T   qpwr;             /**> 无功功率      */
    OOP_LONG4V_T  pwrf;             /**> 功率因数   */
    OOP_INT4V_T   camp;             /**> 容性电流 */
    uint16        nInfo;            /**> 智能电容状态信息 */
    uint32        nVolume1;         /**> 共补电容器容量1 */
    uint32        nVolume2;         /**> 共补电容器容量2 */
    char          nTemp1;           /**> 电容器温度1 */
    char          nTemp2;           /**> 电容器温度2 */
    uint16        rate;             /**> 电容衰减率*/
}OOP_CAPSTATE_T;

/** @brief 2909-电容器数据2*/
typedef struct tag_OOP_CAPSTATE2
{
    OOP_WORD4V_T  vol;              /**> 电压   */
    OOP_INT4V_T   amp;              /**> 电流 */   
    OOP_LONG4V_T  pwrf;             /**> 功率因数	*/
}OOP_CAPSTATE2_T;

/** @brief 2910-电容器数据3*/
typedef struct tag_OOP_CAPSTATE3
{
    OOP_WORD4V_T  vol;              /**> 电压   */
    OOP_INT4V_T   amp;              /**> 电流 */
    OOP_INT4V_T   ppwr;             /**> 有功功率 */
    OOP_INT4V_T   qpwr;             /**> 无功功率      */
    OOP_LONG4V_T  pwrf;             /**> 功率因数	*/
    OOP_WORD4V_T  vol_total_harmonic;    /**> 电压谐波总畸变率*/
    OOP_WORD4V_T  amp_total_harmonic;    /**> 电流谐波总畸变率*/
    uint16 freq;      /**> 频率*/
    uint16 state1;    /**> 电容器运行状态1*/
    uint16 state2;    /**> 电容器运行状态2*/
    uint16 alarm;     /**> 电容器告警状态*/
}OOP_CAPSTATE3_T;

/** @brief 2906-门锁状态*/
typedef struct tag_OOP_LOCKSTATE
{
    uint8 drstate;	// enum{保留(0), 开门   (1), 关门  (2)}
    uint8 lkstate;	// enum{保留(0), 开锁   (1), 闭锁(2)}
}OOP_LOCKSTATE_T;

#define MAX_VARNUM 	24
/** @brief 290B-低高压桩头温度*/
typedef struct tag_OOP_SHORTEMPTVAR
{
    uint32 	nNum;
    uint32	rsv;
    int16 	temp[MAX_VARNUM];
}OOP_SHORTTEMPVAR_T;

/** @brief 290C-柜体内温湿度*/
typedef struct tag_OOP_INTHUMIVAR
{
    uint32 nNum;
    uint32 rsv;
    uint16 humi[MAX_VARNUM];
}OOP_INTHUMIVAR_T;

typedef struct tag_OOP_JPENV
{
    OOP_SHORTTEMPVAR_T 	temp;
    OOP_INTHUMIVAR_T	humi;
}OOP_JPENV_T;

/** @brief 4B00-断路器运行状态字参数块*/
typedef struct tag_OOP_STATWORD
{
    uint8 nStatWord1;              /**> 断路器状态字1 */
    uint8 nStatWord2;              /**> 断路器状态字2 */
}OOP_STATWORD_T;

/**
********************************************************
* A.5 事件类对象
********************************************************
*/

#define OOP_MAX_CHANNEL (4)     /**< 事件上报最大通道数            */

/** @brief 3000-电能表失压事件配置参数   */
typedef struct tag_OOP_EVTCFG_3000
{
    uint16          wVUp;       /**< 电压触发上限 */
    uint16          wVDown;     /**< 电压恢复下限 */
    int32           nADown;     /**< 电流触发下限 */
    uint8           nDelay;     /**< 判定延时时间 */
}OOP_EVTCFG_3000_T;

/** @brief 电能表失压统计状态  */
typedef struct tag_OOP_LOSTVSTAT
{
    uint32              nCount;
    uint32              nTimes;
    OOP_DATETIME_S_T    dtStart;
    OOP_DATETIME_S_T    dtEnd;
}OOP_LOSTVSTAT_T;

/** 
* @brief    配置参数-               \n
*           3001-电能表欠压事件      \n
*           3002-电能表过压事件
*/
typedef struct tag_OOP_EVTCFG_WD
{
    uint16          wValue;      /**< 电压触发上限/电压触发下限 */
    uint8           nDelay;      /**< 判定延时时间                */
}OOP_EVTCFG_WD_T;

/** @brief 3003-电能表断相事件配置参数   */
typedef struct tag_OOP_EVTCFG_3003
{
    uint16          wVUp;           /**< 电压触发上限 */
    int32           nAUp;           /**< 电流触发上限 */
    uint8           nDelay;         /**< 判定延时时间 */
}OOP_EVTCFG_3003_T;

/** @brief 3004-电能表失流事件配置参数   */
typedef struct tag_OOP_EVTCFG_3004
{
    uint16          wVDown;         /**< 电压触发下限 */
    int32               nAUp;           /**< 电流触发上限 */
    int32               nADown;         /**< 电流触发下限 */
    uint8           nDelay;         /**< 判定延时时间 */
}OOP_EVTCFG_3004_T;

/** @brief 3006-电能表断流事件配置参数   */
typedef struct tag_OOP_EVTCFG_3006
{
    uint16          wVDown; /**< 电压触发下限 */
    int32               nAUp;   /**< 电流触发上限 */
    uint8           nDelay; /**< 判定延时时间 */
}OOP_EVTCFG_3006_T;

/** 
* @brief    配置参数-                   \n
*           3005-电能表过流事件          \n
*           3007-电能表功率反向事件        \n
*           3008-电能表过载事件
*/
typedef struct tag_OOP_EVTCFG_ID
{
    int32               nValue;         /**< 电流触发下限、有功功率触发下限  */
    uint8           nDelay;         /**< 判定延时时间                 */
}OOP_EVTCFG_ID_T;

/** 
* @brief    配置参数-                           \n
*           3009-电能表正向有功需量超限事件        \n
*           300A-电能表反向有功需量超限事件        \n
*           300B-电能表无功需量超限事件
*/
typedef struct tag_OOP_OOP_EVTCFG_DD
{
    uint32          dwValue;        /**< 触发限值       */
    uint8           nDelay;         /**< 判定延时时间 */
}OOP_EVTCFG_DD_T;

/** 
* @brief    配置参数-                       \n
*           300C-电能表功率因数超下限事件   \n
*           301D-电能表电压不平衡事件     \n
*           301E-电能表电流不平衡事件     \n
*           302D-电能表电流严重不平衡事件       \n
*           3171-温度超限事件
*/
typedef struct tag_OOP_CFG_LU
{
    int16           nShort;
    uint8           nByte;
}OOP_CFG_LU_T;

/** 
* @brief	配置参数-						\n
*
* 3071-温度超限事件
*/
typedef struct tag_OOP_CFG_3071
{
    int16	        nShort;
}OOP_CFG_3071_T;

/** 
*@brief 配置参数-                       \n
*       300E-电能表辅助电源掉电事件      \n
*       300F-电能表电压逆相序事件     \n
*       3010-电能表电流逆相序事件     \n
*       3011-电能表掉电事件              \n
*       3030-通信模块变更事件
*/
typedef struct tag_OOP_EVTCFG_DELAY
{
    uint8   nDelay;
}OOP_EVTCFG_DELAY_T;

/** @brief 3105-电能表时钟超差事件配置参数 */
typedef struct tag_OOP_EVTCFG_3105
{
    uint16      nValue;     /**< 异常判别阈值 */
    uint8       nTID;       /**< 关联任务序号 */
}OOP_EVTCFG_3105_T;

/** @brief 3072-湿度超限事件	*/
typedef struct tag_OOP_EVTCFG_307x
{
    uint16      nValue;     /**< 异常判别阈值	*/
}OOP_EVTCFG_307x_T;

//
// 3106-终端停/上电事件
//

#define OOP_MAX_POWERCUT_MET    (8)     /**< 停上电事件最大抄读电能表个数 */

/** @brief 停电数据采集配置参数   */
typedef struct tag_DATAACQUIRE
{
    uint8            flag;                               /**< 采集标志                      */
    uint8            interval;                           /**< 停电事件抄读时间间隔(小时)    */
    uint8            limit;                              /**< 停电事件抄读时间限值(分钟)    */
    uint8            meterNum;                           /**< 需要读取停电事件电能表个数   */
    OOP_TSA_T        meter[OOP_MAX_POWERCUT_MET];        /**< 需要读取停电事件电能表     */
}OOP_DATAACQUIRE_T;

/** @brief 停电事件甄别限值参数   */
typedef struct tag_EVENTIDENTIFY
{
    uint16            minInterval;    /**< 停电时间最小有效间隔(分钟)       */
    uint16            maxInterval;    /**< 停电时间最大有效间隔(分钟)       */
    uint16            limit1;         /**< 停电事件起止时间偏差限值(分钟) */
    uint16            limit2;         /**< 停电时间时间区段偏差限值(分钟) */
    uint16            occVolt;        /**< 停电发生电压限值                 */
    uint16            recVolt;        /**< 停电恢复电压限值                 */
}OOP_EVENTIDENTIFY_T;

/** @brief 终端停/上电事件配置参数   */
typedef struct tag_OOP_POWERONCUT
{
    OOP_DATAACQUIRE_T     cParam;          /**< 停电数据采集配置参数  */
    OOP_EVENTIDENTIFY_T   lParam;          /**< 停电事件甄别限值参数  */
}OOP_POWERONCUT_T;

/** @brief 301F-电能跳闸事件配置参数 */
typedef struct tag_OOP_EVTCFG_301F
{
    int16      currtmp;     /**< 断子过热跳闸温度阈值 */
    int16      rectmp;       /**< 端子过热跳闸恢复温度阈值 */
    uint32     current;      /**< 端子过热跳闸电流阈值 */
    uint8      delay;		/**< 端子过热跳闸延时时间 */
}OOP_EVTCFG_301F_T;

/** 
* @brief    配置参数-                       \n
*           3107-终端直流模拟量越上限事件   \n
*           3108-终端直流模拟量越下限事件
*/
typedef struct tag_OOP_EVTCFG_INT
{
    int32             limit;          /**< 直流模拟量上限、下限   */
}OOP_EVTCFG_INT_T;

/** @brief 310A-设备故障类型定义    */
typedef enum tag_OOP_DEVICEFAULT
{
    FAULT_MEMORY    = 0,
    FAULT_CLOCK     = 1,
    FAULT_BOARD     = 2,
    FAULT_METERING  = 3,
    FAULT_DISPLAY   = 4,
    FAULT_PLC       = 5,
    FAULT_CARD      = 6,
    FAULT_ESAM				= 7,
    FAULT_CLOCK_BATTERY		= 8,	// 时钟电池故障
    FAULT_BACK_BATTERY		= 9,	// 备用电池故障
    FAULT_BLUETOOTH			= 10,	// 蓝牙模块故障
    FAULT_COMM_AC			= 11,	// 计量单元通讯故障
    FAULT_LOAD_OVER			= 12,	// 台区负荷越限
    FAULT_COMM_CHARGING		= 13,	// 充电桩通信故障
    FAULT_DEV_CHARGING		= 14,	// 充电桩设备故障
    FAULT_ORDER_OVER		= 15,	// 本地订单记录满
    FAULT_POWER_CHARGING	= 16,	// 超功率限值充电
    FAULT_CLOCK_LOOP		= 17,	// 回路巡检模块故障
    FAULT_MODULE_LOAD		= 18	// 负荷辨识模块故障
}OOP_DEVICEFAULT_E;

/** 
* @brief    配置参数-                   \n
*           310B-电能表示度下降事件        \n
*           311C-电能表数据变更监控记录
*/
typedef struct tag_OOP_EVTCFG_310B
{
    uint8         nTID;
}OOP_EVTCFG_310B_T;

/** 
* @brief    配置参数-               \n
*           310C-电能表超差事件      \n
*           310D-电能表飞走事件
*/
typedef struct tag_OOP_EVTCFG_310X
{
    uint32          nValue;
    uint8           nTID;
}OOP_EVTCFG_310X_T;

/** @brief 310E-电能表停走事件配置参数   */
typedef struct tag_OOP_EVTCFG_310E
{
    OOP_TI_T      threshold;        /**< 阈值 */
    uint8         nTID;
}OOP_EVTCFG_310E_T;

/** @brief 310F-终端抄表失败事件配置参数    */
typedef struct tag_OOP_EVTCFG_310F
{
    uint8       nTicks;
    uint8       nTID;
}OOP_EVTCFG_310F_T;

/** @brief 3110-通讯流量超限事件配置参数            */
typedef struct tag_OOP_EVTCFG_3110
{
    uint32            nValue;
}OOP_EVTCFG_3110_T;

/** @brief 3701-终端低电压事件*/
typedef struct tag_OOP_EVTCFG_3701
{
    uint16   ldVolt;            /**< 低电压阈值*/
    uint16   ubVolt;            /**< 三相不平衡阈值*/
    uint16   dutyVolt;          /**< 负载率阈值 */
}OOP_EVTCFG_3701_T;

/**< 电能质量分析事件参数 */
typedef struct tag_OOP_EVTCFG_PQA
{
    OOP_EVTCFG_3701_T       lowvol;                         // 3701-台区低电压参数 
    OOP_CFG_LU_T            volunb;                         // 3702-台区电压不平衡参数 
    OOP_CFG_LU_T            ampunb;                         // 3703-台区电流不平衡参数 
    OOP_CFG_LU_T            overload;                       // 3704-台区负载率限值参数 
    OOP_CFG_LU_T            overfreq;                       // 3705-台区频率限值参数 
}OOP_EVTCFG_PQA_T;

#define OOP_MAX_EVTCFGNUM		(8)    // 有功总电能量差动组最大可配置数量
/** @brief 3116-有功总电能量差动越限事件配置参数    */
typedef struct tag_OOP_EVTCFG_3116
{
    uint8       nIndex;
    OI          objDest;
    OI          objSrce;
    uint8       flag;
    char        offset;
    int64       llValue;
}OOP_EVTCFG_3116_T;

/** @brief 3116-有功总电能量差动越限事件配置参数    */
typedef struct tag_OOP_EVTCFG
{
    uint32                   nNum;
    uint32                   rsv;
    OOP_EVTCFG_3116_T        para[OOP_MAX_EVTCFGNUM];  
}OOP_EVTCFG_T;

/** @brief 311A-电能表在网状态切换事件配置参数       */
typedef struct tag_OOP_EVTCFG_311A
{
    uint16        nDelay;
}OOP_EVTCFG_311A_T;

//
// 3300-通道上报状态
//

/** @brief 事件固定域  */
typedef struct tag_OOP_EVENTFIXED
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
}OOP_EVENTFIXED_T;


//
// 3030-通信模块变更事件
//

/** @brief 通信模块变更事件单元   */
typedef struct tag_OOP_MODULELOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
    OOP_OCTETVAR16_T    add;
    OOP_VISIBLEVAR256_T preID;
    OOP_VISIBLEVAR256_T curID;
}OOP_MODULELOG_T;

//
// 3105-电能表时钟超差事件
//

/** @brief 电能表时钟超差事件单元    */
typedef struct tag_OOP_CLOCKFLYLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
    OOP_DATETIME_S_T    clkMeter;
    OOP_DATETIME_S_T    clkTerm;
}OOP_CLOCKFLYLOG_T;

/** @brief 电表时钟超差信息	*/
typedef struct tag_OOP_CHK_TIMEOVERINFO
{
    OOP_OCTETVAR16_T	add;			/**< 电能表地址		*/
    OOP_DATETIME_S_T	chkTime;		/**< 电能表当前时间	*/
    OOP_DATETIME_S_T	termTime;		/**< 终端当前时间	    */
}OOP_CHK_TIMEOVERINFO_T;

//
// 3302-编程事件记录单元
//

/** @brief 编程事件记录单元 */
typedef struct tag_OOP_PROGRAMLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
    uint32              nOADNum;
    OOP_OAD_U           oad[OOP_MAX_OADS];
}OOP_PROGRAMLOG_T;

//
// 3303-发现未知电能表事件单元
//

#define OOP_MAX_SMREPORT    (12)    /**< 搜表上报最大个数       */
#define OOP_MAX_ADDEDINFO   (4)     /**< 最大搜到的付加信息个数  */

/** @brief 搜表地址信息域    */
typedef struct tag_ADDEDINFO
{
    OOP_OAD_U         oad;          /**< 对象属性描述符  */
    OOP_VARIANT_T     value;        /**< 属性值          */
}OOP_ADDEDINFO_T;

/** @brief 一个搜表结果   */
typedef struct tag_OOP_SEARCHMET_RES
{
    OOP_TSA_T           commAdd;                    /**< 通信地址                       */
    OOP_TSA_T           collAdd;                    /**< 采集器地址                        */
    uint8               proto;                      /**< 规约类型(enum)                 */
    uint8               phase;                      /**< 相位(enum)                   */
    uint8               quality;                    /**< 信号品质                       */
    OOP_DATETIME_S_T    serchTime;                  /**< 搜到的时间                        */
    uint8               nAdded;                     /**< 附加信息个数                     */
    OOP_ADDEDINFO_T     info[OOP_MAX_ADDEDINFO];    /**< 搜到的附加信息                  */
}OOP_SEARCHMET_RES_T;

/** @brief 发现未知电能表事件单元    */
typedef struct tag_OOP_SMLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    uint32              nResNum;
    OOP_SEARCHMET_RES_T Res[OOP_MAX_SMREPORT];
}OOP_SMLOG_T;

//
// 3304-发现跨台区电能表事件单元
//

/** @brief 一个跨台区结果    */
typedef struct tag_OOP_ACROSSAREA_RES
{
    OOP_TSA_T           commAdd;    /**< 通信地址   */
    OOP_TSA_T           netAdd;     /**< 主节点地址    */
    OOP_DATETIME_S_T    change;     /**< 变更时间   */
}OOP_ACROSSAREA_RES_T;

/** @brief 发现跨台区电能表事件单元 */
typedef struct tag_OOP_UNMETERLOG
{
    uint32                  nIndex;
    OOP_DATETIME_S_T        tmStart;
    OOP_DATETIME_S_T        tmEnd;
    OOP_REPORTS_T           evtState;
    OOP_EVTSOURCE_T         source;
    OOP_EVTBUF_T            data;
    uint32                  nResNum;
    OOP_ACROSSAREA_RES_T    Res[OOP_MAX_SMREPORT];
}OOP_UNMETERLOG_T;

//
// 3305-功控事件记录
//

/** @brief 功控事件记录单元 */
typedef struct tag_OOP_PCTRLLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
    OI                  objCtrl;
    OOP_BITSTR8_T       turnState;
    int64               power2Min;
    int64               llSetting;
    int64               power;
}OOP_PCTRLLOG_T;

/** @brief 3306-电控事件记录单元    */
typedef struct tag_OOP_ECTRLLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
    OI                  objCtrl;
    OOP_BITSTR8_T       turnState;
    int64               llSetting;
    int64               energy;
}OOP_ECTRLLOG_T;

/** @brief 3307-电控告警记录单元    */
typedef struct tag_OOP_EWARNLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
    OI                  objCtrl;
    int64               llSetting;
}OOP_EWARNLOG_T;

/** @brief 3308-需量越限记录  */
typedef struct tag_OOP_DEMANDLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source; 
    OOP_EVTBUF_T        data;
    uint32              dwValue;
    OOP_DATETIME_S_T    time;
}OOP_DEMANDLOG_T;

/** @brief 3309-停上电事件单元   */
typedef struct tag_OOP_POWERLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
    OOP_BITSTR8_T       flag;
}OOP_POWERLOG_T;

/** @brief 330A-遥控记录单元  */
typedef struct tag_OOP_RCTRLLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
	uint8				cmd;
    int64               prePower[8];
    int64               llPower[8];
}OOP_RCTRLLOG_T;

/** @brief 330B-差动越限记录单元    */
typedef struct tag_OOP_DCLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    int64               dstVal;
    int64               srcVal;
    char                comVal;
    uint32              mstVal;
}OOP_DCLOG_T;

//
// 330C-事件清零记录
//

#define OOP_MAX_EVTRESETS   (8)     /**< 事件清零方法最大个数 */

/** @brief 事件清零记录单元 */
typedef struct tag_OOP_EVENTRESETLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    uint32              nRstNum;
    OOP_OMD_U           lstRst[OOP_MAX_EVTRESETS];
}OOP_EVENTRESETLOG_T;

/** @brief 330D-终端对电表校时记录单元   */
typedef struct tag_OOP_P2PCLOCKLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_DATETIME_S_T    preTime;
    char                offset;
}OOP_P2PCLOCKLOG_T;

/** @brief 电表校时信息   */
typedef struct tag_OOP_CHK_TIMEINFO
{
    OOP_OCTETVAR16_T    add;            /**< 电能表地址        */
    OOP_DATETIME_S_T    chkTime;        /**< 校时时间       */
    OOP_DATETIME_S_T    beforechkTime;  /**< 校时前时间        */
    int16               error;          /**< 误差         */
}OOP_CHK_TIMEINFO_T;
/** @brief 电表时钟超差信息   */
typedef struct tag_OOP_CHK_CLOCKOUTINFO
{
    OOP_OCTETVAR16_T    add;            /**< 电能表地址        */
    OOP_DATETIME_S_T    clkMeter;        /**< 电表时间       */
    OOP_DATETIME_S_T    clkTerm;        /**< 终端时间        */
}OOP_CHK_CLOCKOUTINFO_T;


//
// 330E-电能表在网状态切换
//

#define OOP_MAX_METEROFF    (32)    /**< 最大记录电表状态变化数量   */

/** @brief 电能表离网信息    */
typedef struct tag_OOP_METEROFF
{
    OOP_TSA_T       add;
    BOOLEAN         online;
}OOP_METEROFF_T;

/** @brief 电能表在网状态切换事件单元  */
typedef struct tag_OOP_METEROFFLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    uint32              nResNum;
    OOP_METEROFF_T      Res[32];
}OOP_METEROFFLOG_T;

/** @brief 330F-电能表数据变更监控记录单元 */
typedef struct tag_OOP_METERDIRTYLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_CSD_T           di;
    uint8               preLen;
    uint8               pre[64];
    uint8               aftLen;
    uint8               aft[64];
}OOP_METERDIRTYLOG_T;

/** @brief 3310-电能表异常插卡记录单元   */
typedef struct tag_OOP_ABNORMALCARD
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
    OOP_OCTETVAR64_T    cardNo;
    uint8               errCode;
    OOP_OCTETVAR64_T    cmdHead;
    uint16              cardState;
}OOP_ABNORMALCARD_T;

/** @brief 3311-退费事件记录单元    */
typedef struct tag_OOP_REBATELOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
    uint32              money;
}OOP_REBATELOG_T;

//
// 3314-电能表时段表编程事件
//
/** @brief 3706-断路器自检单元   */
typedef struct tag_OOP_ResCheckSelf
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    
    OOP_EVTBUF_T        data;
    OOP_BITSTR8_T       flag;
}OOP_ResCheckSelf_T;

/** @brief 时段单元 */
typedef struct tag_PERIOD
{
    uint8    hour;                /**< 时      */
    uint8    min;                 /**< 分      */
    uint8    rateNO;              /**< 费率号    */
}PERIOD;

// 日时段数
#define OOP_MAX_DAYPERIOD       (14)

/** @brief 时段表信息单元    */
typedef struct tag_DAYPERIOD
{
    uint32        nNum;
    uint32        rsv;
    PERIOD        period[OOP_MAX_DAYPERIOD];  
}DAYPERIOD;

#define OOP_TTU_MAX_PERIOD      (48)

/** @brief 时段表信息单元    */
typedef struct tag_OOP_TTU_PERIOD_T
{
    uint32        nNum;
    uint32        rsv;
    int32         nValue[OOP_TTU_MAX_PERIOD];  
}OOP_TTU_PERIOD_T;

/** @brief 电能表时段表编程事件单元 */
typedef struct tag_OOP_PROGPERIODLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
    OOP_OAD_U           di;
    DAYPERIOD           context;
}OOP_PROGPERIODLOG_T;

//
// 3315-电能表节假日编程事件
//

/** @brief 节假日信息单元    */
typedef struct tag_PUBHOLIDAY
{
    OOP_DATE_T      date;
    uint8           meterNum;           /**< 日时段表号    */
}PUBHOLIDAY;

/** @brief 电能表节假日编程事件单元 */
typedef struct tag_OOP_PROGHOLIDAYLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
    OOP_OAD_U           di;
    PUBHOLIDAY          context;
}OOP_PROGHOLIDAYLOG_T;

//
// 3316-安全变更事件
//

#define OOP_MAX_SECURITYCHGS    (8) /** @brief 安全变更单元最大个数   */

/** @brief 外连信息 */
typedef struct tag_OOP_CONNECTINFO
{
    OOP_OCTETVAR16_T        ip;
    uint16                  port;
}OOP_CONNECTINFO_T;

/** @brief 流量会话异常	*/
typedef struct
{
    OOP_OCTETVAR16_T	ipRemote;
    uint16				portRemote;
    OOP_OCTETVAR16_T	ipLocal;
    uint16				portLocal;
    OOP_VISIBLEVAR_T	proto;
    OOP_VISIBLEVAR_T	prog;
    uint8				progType;
} OOP_SECURITY_SESSION_T;

/** @brief 流量带宽异常	*/
typedef struct
{
    OOP_OCTETVAR16_T	ipRemote;
    uint16				portRemote;
    OOP_OCTETVAR16_T	ipLocal;
    uint16				portLocal;
    OOP_VISIBLEVAR_T	proto;
    OOP_VISIBLEVAR_T	prog;
    uint32				streamLimit;
    uint32				streamCur;
} OOP_SECURITY_STREAM_T;

/** @brief 进程异常	*/
typedef struct
{
    uint8				type;
    OOP_DATETIME_S_T	dtStart;
    OOP_VISIBLEVAR_T	prog;
    OOP_VISIBLEVAR_T	progUser;
    OOP_VISIBLEVAR_T	progGroup;
    OOP_VISIBLEVAR_T	progCmd;
} OOP_SECURITY_PROG_T;

/** @brief 用户登录异常	*/
typedef struct
{
    uint8				type;
    OOP_VISIBLEVAR_T	user;
    OOP_VISIBLEVAR_T	source;
} OOP_SECURITY_LOGIN_T;

/** @brief 安全变更事件单元 */
typedef struct tag_OOP_SECURITYLOG
{
    uint32                  nIndex;
    OOP_DATETIME_S_T        tmStart;
    OOP_DATETIME_S_T        tmEnd;
    OOP_REPORTS_T           evtState;
    OOP_EVTSOURCE_T         source;
    OOP_EVTBUF_T            data;
    OOP_BITSTR16_T          flag;
    uint8                   nEthPort;
    uint16                  ethPort[OOP_MAX_SECURITYCHGS];
    uint8                   nEthConnects;
    OOP_CONNECTINFO_T       ethConnects[OOP_MAX_SECURITYCHGS];
    uint8                   nLtePort;
    uint16                  ltePort[OOP_MAX_SECURITYCHGS];
    uint8                   nLteConnects;
    OOP_CONNECTINFO_T       lteConnects[OOP_MAX_SECURITYCHGS];
    OOP_BITSTR8_T           usbInfo;
    OOP_BITSTR8_T           rs232Info;
    uint8                   nChgPath;
    OOP_VISIBLEVAR_T        chgPath[OOP_MAX_SECURITYCHGS];
    OOP_BITSTR8_T           ethFlag;
    OOP_BITSTR8_T           rs232Flag;
    OOP_BITSTR8_T           errFlag;
    uint8					errSSH;
    OOP_SECURITY_SESSION_T	errSession;
    OOP_SECURITY_STREAM_T	errStream;
    OOP_SECURITY_PROG_T		errProg;
    OOP_SECURITY_LOGIN_T	errLogin;
}OOP_SECURITYLOG_T;

//
// 3317-电能表在线状态变化事件
//

/** @brief 离线信息域  */
typedef struct tag_OFFLINEINFO
{
    uint32  nTime;              /**< 离线时常                                           */
    uint8   nReason;            /**< 离线原因 enum{未知(0)，停电(1)，信道变化(2)} */
}OFFLINEINFO;

/** @brief 离线上报信息	*/
typedef struct tag_OOP_RPT_OFFLINEINFO
{
    OOP_TSA_T           add;        /**< 电能表地址		*/
    uint8				nState;		/**< 状态变化	enum{上线(0)， 离线(1)}		*/
    OFFLINEINFO			info;		/**< 离线信息	structure OPTIONAL			*/
}OOP_RPT_OFFLINEINFO;

/** @brief 电能表在线状态变化事件单元  */
typedef struct tag_OOP_METOFFLINELOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    uint8               nState;     /**< 状态变化   enum{上线(0)， 离线(1)}        */
    OFFLINEINFO         info;       /**< 离线信息   structure OPTIONAL          */
}OOP_METOFFLINELOG_T;

//
// 311F-拒绝从节点入网记录
//

#define OOP_MAX_REFUSEADDRNUM       (32)        /**< 节点拒绝信息主动上报最大个数 */

/** @brief 332F-拒绝从节点入网记录单元 */
typedef struct tag_OOP_REFUSESLAVE
{
    uint32              nNum;
    uint32              rsv;
    OOP_OCTETSTR6_T     sa[OOP_MAX_REFUSEADDRNUM];
}OOP_REFUSESLAVE_T;

typedef struct tag_OOP_REFUSENODEACCESSLOG
{
    uint32              nIndex;     /**< 事件记录序号 */
    OOP_DATETIME_S_T    tmStart;    /**< 事件发生时间 */
    OOP_DATETIME_S_T    tmEnd;      /**< 事件结束时间 */
    OOP_REPORTS_T       evtState;   /**< 事件上报状态array 通道上报状态 */
    OOP_EVTSOURCE_T     source;     /**< 事件发生源    */  
    OOP_EVTBUF_T        data;       /**< 事件关联数据 */  
    OOP_REFUSESLAVE_T   info;       /**< 拒绝从节点列表 array octet-string   */
}OOP_REFUSENODEACCESSLOG_T;

//
// 311E-参数变更记录
//

/** @brief 331C-参数变更记录单元    */
typedef struct tag_OOP_PARAMCHANGELOG
{
    uint32              nIndex;     /**< 事件记录序号 */
    OOP_DATETIME_S_T    tmStart;    /**< 事件发生时间 */
    OOP_DATETIME_S_T    tmEnd;      /**< 事件结束时间 */  
    OOP_REPORTS_T       evtState;   /**< 事件上报状态array 通道上报状态 */  
    OOP_EVTSOURCE_T     source;     /**< 事件发生源    */  
    OOP_EVTBUF_T        data;       /**< 事件关联数据 */  
    OOP_BUF1K_T         before;     /**< 参数变更前值 */
    OOP_BUF1K_T         after;      /**< 参数变更后值 */
}OOP_PARAMCHANGELOG_T;

// 
// 3120-电流互感器异常事件
//
#define OOP_LC_PHASE    (3)
#define OOP_LC_GROUPNUM (3)

//
// 3600-系统监测事件
//

typedef struct
{
    uint16	ramRate;			// 内存使用率上限阈值（单位：%，换算： - 2），
    uint16	memRate;			// 存储使用率上限阈值（单位：%，换算： - 2），
    uint16	cpuRate;			// CPU 使用率上限阈值（单位：%，换算： - 2），
    int16	tempBroadUp;		// 主板温度上限阈值（单位：℃，换算： - 1），
    int16	tempBroadDown;		// 主板温度下限阈值（单位：℃，换算： - 1），
    uint16	period;				// 监测周期（单位：分钟，换算：0）
} OOP_EVTCFG_3600_T;

typedef enum
{
    WT_CPU					= 0,	// CPU
    WT_RAM					= 1,	// 内存
    WT_MEM					= 2,	// 存储
    WT_BROAD_TEMP			= 3,	// 主板温度
    WT_CPU_OVER				= 4,	// CPU 利用率超过阀值
    WT_RAM_OVER				= 5,	// 内存使用率超过阀值
    WT_MEM_OVER				= 6,	// 磁盘空间使用率超过阀值
    WT_CONTAINER_RESTART	= 7,	// 容器异常重启 
    WT_CONTAINER_BAD		= 8,	// 容器故障
    WT_APP_RESTART			= 9,	// 应用异常重启
    WT_APP_UPGRADE			= 10,	// 应用升级
    WT_APP_BAD				= 11,	// App 异常重启
    WT_SIGN_INVALID			= 12,	// 无效签名证书
    WT_SIGN_NONE			= 13,	// 无签名证书
    WT_INSTALL				= 14,	// 安装 
    WT_UPGRADE				= 15,	// 升级 
    WT_UNINSTALL			= 16,	// 卸载 
    WT_CREATE				= 17,	// 创建 
    WT_START				= 18,	// 启动  
    WT_STOP					= 19,	// 停止 
} OOP_WARN_TYPE_E;

/** @brief 3327-系统监测事件单元	*/
typedef struct tag_OOP_MONITORLOG
{
    uint32				nIndex;			/**< 事件记录序号	*/
    OOP_DATETIME_S_T	tmStart;		/**< 事件发生时间	*/
    OOP_DATETIME_S_T	tmEnd;			/**< 事件结束时间	*/
    OOP_REPORTS_T		evtState;		/**< 事件上报状态array 通道上报状态	*/
    OOP_EVTSOURCE_T		source;			/**< 事件发生源	*/
    uint8				warn;			/**< 告警类别		*/
    uint16				value;			/**< 实际监控值	*/
    uint16				rate;			/**< 监控阈值		*/
    OOP_VISIBLEVAR_T	containerName;	/**< 容器名称		*/
    OOP_VISIBLEVAR_T	appName;		/**< 应用名称		*/
    OOP_VISIBLEVAR_T	serviceName;	/**< 服务名称		*/
}OOP_MONITORLOG_T;

//
// 3E40-疑似窃电事件
//

/** @brief 疑似窃电事件单元	*/
typedef struct tag_OOP_3E70
{
    uint32					nIndex;
    OOP_DATETIME_S_T		tmStart;
    OOP_DATETIME_S_T		tmEnd;
    OOP_REPORTS_T			evtState;
    OOP_EVTSOURCE_T			source;
    uint8					phase;
}OOP_3E70_T;

//
// 3E41-疑似窃电事件
//

/** @brief 图片视频采集事件单元	*/
typedef struct tag_OOP_3E71
{
    uint32					nIndex;
    OOP_DATETIME_S_T		tmStart;
    OOP_DATETIME_S_T		tmEnd;
    OOP_REPORTS_T			evtState;
    OOP_EVTSOURCE_T			source;
}OOP_3E71_T;

//
// 3E42-强磁场事件
//

/** @brief 强磁场事件单元	*/
typedef struct tag_OOP_POS
{
    uint16	x;
    uint16	y;
    uint16	z;
}OOP_POS_T;

typedef struct tag_OOP_3E72
{
    uint32					nIndex;
    OOP_DATETIME_S_T		tmStart;
    OOP_DATETIME_S_T		tmEnd;
    OOP_REPORTS_T			evtState;
    OOP_EVTSOURCE_T			source;
    OOP_POS_T				pos;
}OOP_3E72_T;

/** @brief 4040- 阻抗模值*/
typedef struct tag_OOP_IMPE_VAL
{
    uint32 nNum;
    uint32 rsv;
    uint32 impeval[OOP_LC_GROUPNUM];
}OOP_IMPE_VAL_T;

/** @brief 4040- 阻抗角度*/
typedef struct tag_OOP_IMPE_ANG
{
    uint32 nNum;
    uint32 rsv;
    uint32 impeang[OOP_LC_GROUPNUM];
}OOP_IMPE_ANG_T;

/** @brief 4040- 阻抗频率*/
typedef struct tag_OOP_IMPE_FREQ
{
    uint32 nNum;
    uint32 rsv;
    uint32 impefreq[OOP_LC_GROUPNUM];
}OOP_IMPE_FREQ_T;

/** @brief 工况信息 */
typedef struct tag_OOP_WORKCOND
{
    uint8               curloopstat;    /**< 电流回路状态 */
    uint32              temp;           /**< 温度 */
    uint32              maxfreq;        /**< 频率最大值 */
    uint32              minfreq;        /**< 频率最小值 */
    uint32              curvalid;       /**< 工频电流有效值 */
    OOP_IMPE_VAL_T      impval;         /**< 阻抗模值 */
    OOP_IMPE_ANG_T      impang;         /**< 阻抗角度 */
    OOP_IMPE_FREQ_T     impfrq;         /**< 阻抗频率 */
}OOP_WORKCOND_T;

#define OOP_MAX_WORKIINFO   (3)     /**< 最大工况信息     */

/** @brief 工况信息集合   */
typedef struct tag_OOP_WORKINFO
{
    uint32          nNum;
    uint32          rsv;
    OOP_WORKCOND_T  item[OOP_MAX_WORKIINFO];
}OOP_WORKINFO_T;

/** @brief 3318-电流互感器异常事件单元   */
typedef struct tag_OOP_ABNORMALCTLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T		data;
    OOP_BITSTR8_T       abnmlflg;
    OOP_WORKINFO_T      lcWarn;
    OOP_WORKINFO_T      prelcWarn;
}OOP_ABNORMALCTLOG_T;

//
// 60F1-完整节点拓扑信息
//
#define OOP_MAX_TOPCNT  (256)

/** @brief 60F2-一个节点拓扑信息	*/
typedef struct tag_OOP_AREANODETOPINFO
{
    OOP_TSA_T           add;            /**< 本节点设备地址 */
    uint8               phase;          /**< 本节点相位信息 未知（0），A（1），B（2），C（3）*/
    uint8               devtype;        /**< 本节点设备类型 分支箱（1）， 表箱（2）， 电表（3）*/
    OOP_TSA_T           parent;       	/**< 父节点设备地址 */
    OOP_TSA_T           acAdd;          /**< 父节点线路计量地址*/
}OOP_AREANODETOPINFO_T;

/** @brief 每天周期拓扑识别	*/
typedef struct tag_OOP_TOPLOGYPARAM
{
    BOOLEAN		bValid;			/**< 周期拓扑识别启动标志 */
    OOP_TIME_T	time;			/**< 开始时间 */
}OOP_TOPLOGYPARAM_T;

// 33F0-节点相位变更记录单元

#define OOP_MAX_CHGNUM  (31)

typedef struct tag_PHASECHGINFO
{
    OOP_TSA_T       addr;           // 通信地址
    OOP_BITSTR16_T  befphase;       // 节点相位变更信息：bit-string(SIZE(16))
    OOP_BITSTR16_T  aftphase;       // 节点相位变更信息：bit-string(SIZE(16))
}PHASECHGINFO;

typedef struct tag_OOP_PHASECHGINFO
{
    uint32          nNum;   
    uint32          rsv;
    PHASECHGINFO    info[OOP_MAX_CHGNUM];
}OOP_PHASECHGINFO_T;

typedef struct tag_OOP_NODEPHASECHGLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_PHASECHGINFO_T  info;
}OOP_NODEPHASECHGLOG_T;

// 33F1-节点版本变更记录单元

typedef struct tag_VERCHGINFO
{
    OOP_TSA_T           addr;           // 通信地址
    OOP_OCTETSTR3_T octVerBef;          // 升级前版本：octet-string(SIZE(3))
    OOP_OCTETSTR3_T octVerAft;          // 升级后版本：octet-string(SIZE(3))
}VERCHGINFO;

typedef struct tag_OOP_VERCHGINFO
{
    uint32          nNum;   
    uint32          rsv;
    VERCHGINFO      info[OOP_MAX_CHGNUM];
}OOP_VERCHGINFO_T;

typedef struct tag_OOP_NODEVERCHGLOG
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_VERCHGINFO_T    info;
}OOP_NODEVERCHGLOG_T;

//
// 3320-新增事件列表
//

#define OOP_MAX_NEWEVENT    (64)    /**< 最大新增上报事件个数     */
#define OOP_MAX_REPORTEVENT (32)    /**< 最大上报事件个数           */

/** @brief 新增上报事件列表 */
typedef struct tag_OOP_NEWEVENT
{
    uint32              nNum;
    uint32              rsv;
    OOP_OAD_U           item[OOP_MAX_NEWEVENT];
}OOP_NEWEVENT_T;

/** @brief 上报事件列表   */
typedef struct tag_OOP_REPORTEVENT
{
    uint32              nNum;
    uint32              rsv;
    OI                  item[OOP_MAX_REPORTEVENT];
}OOP_REPORTEVENT_T;

//
// 3402-充电计划上报事件
//
#define OOP_MAX_CHARGEPLAN       24
#define OOP_MAX_CHARGE_PERIOD    24

typedef struct tag_OOP_POWER_PERIOD
{
    OOP_DATETIME_S_T         tmStart;       /**>充电开始时间*/      
    OOP_DATETIME_S_T         tmEnd;         /**>充电结束时间*/
    int32                    nPower;        /**>充电功率*/
}OOP_POWER_PERIOD_T;

typedef struct tag_OOP_PERIOD_LIST
{
    uint32 nNum;
    OOP_POWER_PERIOD_T  period[OOP_MAX_CHARGE_PERIOD];
}OOP_PERIOD_LIST_T;

/** @brief 3322-充电计划上报记录单元  */
typedef struct tag_OOP_CHGPLANLOG
{
    uint32                   nIndex;                             /**>事件记录序号*/
    OOP_DATETIME_S_T         tmStart;                            /**>事件发生时间*/
    OOP_DATETIME_S_T         tmEnd;                              /**>事件结束时间*/
    OOP_REPORTS_T            evtState;                           /**>事件发生状态*/
    OOP_EVTSOURCE_T          source;                             /**>事件发生源*/
    OOP_OCTETVAR64_T         reqForm;                            /**>申请单*/
    OOP_VISIBLEVAR128_T      userId;                             /**>用户ID*/
    uint8                    policyEle;                          /**>决策单元*/
    uint8                    policyType;                         /**>决策类型*/
    OOP_DATETIME_S_T         policyTime;                         /**>决策时间*/
    OOP_VISIBLEVAR32_T       routerAssetNum;                     /**>路由器资产编号*/ 
    uint32                   energyNeed;                         /**>需求电量*/
    int32                    nPower;                             /**>功率*/
    uint8                    nMode;                              /**>充电模式*/
    OOP_PERIOD_LIST_T        periodList;                         /**>充电时段*/ 
    OOP_EVTBUF_T             data;                               /**>关联数据*/
}OOP_CHGPLANLOG_T;

//
// 3403-充电申请事件
//

/** @brief 3323-充电申请事件记录单元  */
typedef struct tag_OOP_CHGREQLOG
{
    uint32                   nIndex;                             /**>事件记录序号*/
    OOP_DATETIME_S_T         tmStart;                            /**>事件发生时间*/
    OOP_DATETIME_S_T         tmEnd;                              /**>事件结束时间*/
    OOP_REPORTS_T            evtState;                           /**>事件发生状态*/
    OOP_EVTSOURCE_T          source;                             /**>事件发生源*/
    OOP_OCTETVAR64_T         reqForm;                            /**>申请单*/
    OOP_VISIBLEVAR128_T      userId;                             /**>用户ID*/ 
    OOP_VISIBLEVAR32_T       routerAssetNum;                     /**>路由器资产编号*/ 
    OOP_DATETIME_S_T         tmReq;                              /**>充电申请时间*/
    uint32                   energyNeed;                         /**>需求电量*/
    OOP_DATETIME_S_T         tmCarUsed;                          /**>计划用车时间*/
    uint8                    nMode;                              /**>充电模式*/
    OOP_VISIBLEVAR32_T       authIdentily;                       /**>身份认证*/ 
    OOP_EVTBUF_T             data;                               /**>关联数据*/
}OOP_CHGREQLOG_T;
#if 0
/** @brief 3327-系统监测事件记录单元  */
typedef struct tag_OOP_SYSTEMMONITOR
{
    uint32                   nIndex;                             /**>事件记录序号*/
    OOP_DATETIME_S_T         tmStart;                            /**>事件发生时间*/
    OOP_DATETIME_S_T         tmEnd;                              /**>事件结束时间*/
    OOP_REPORTS_T            evtState;                           /**>事件发生状态*/
    OOP_EVTSOURCE_T          source;                             /**>事件发生源*/
    OOP_EVTBUF_T        	 data;
    uint8                    alarmtype;                          /**>告警类别*/
    uint16                   realityvalue;                       /**>实际值*/
    uint16                   setvalue;                           /**>设置阈值*/
    OOP_VISIBLEVAR32_T       containername;						 /**>容器名称*/
    OOP_VISIBLEVAR32_T       appname;                            /**>应用名称*/
    OOP_VISIBLEVAR32_T       servicename;                        /**>服务名称*/
}OOP_SYSTEMMONITOR_T;
#endif
//
// 锁具异常事件
//

/** @brief 3331-锁具异常事件记录单元	*/
typedef struct tag_OOP_LOCKEVENTLOG_T
{
    uint32				   nIndex;                           /**>事件记录序号*/
    OOP_DATETIME_S_T	   tmStart;                          /**>事件发生时间*/
    OOP_DATETIME_S_T	   tmEnd;                            /**>事件结束时间*/
    OOP_REPORTS_T		   evtState;                         /**>事件发生状态*/
    OOP_EVTSOURCE_T		   source;                           /**>事件发生源*/
    OOP_BITSTR16_T         nState;                           /**>锁具状态*/
    OOP_BITSTR16_T         nChgState;	                     /**>锁具变位状态*/
    OOP_EVTBUF_T		   data;                             /**>关联数据*/
}OOP_LOCKEVENTLOG_T;

//
// 低电压事件
//
typedef struct tag_OOP_LOWVOLLOG_T
{
    uint32                 nIndex;
    OOP_DATETIME_S_T       tmStart;
    OOP_DATETIME_S_T       tmEnd;
    OOP_REPORTS_T          evtState;
    OOP_EVTSOURCE_T        source;
    OOP_OCTETVAR8_T        nState;
    uint8                  nFlag;     /**< 属性标志 */
}OOP_LOWVOLLOG_T;

//
// 3E82-拓扑识别结果事件单元
//
#define MAX_TOPLOGYITEM_NUM		16

typedef enum tag_OOP_TOPLOGSTATE_E
{
    OOP_IDEN_FAILED		= 0,
    OOP_IDEN_SUCCESS	= 1
}OOP_TOPLOGSTATE_E;

typedef struct tag_OOP_TOPLOGYITEM_T
{
    OOP_DATETIME_S_T	tmSucess;		/**>识别成功时间				*/
    uint8				idState;		/**>识别状态码				*/
    uint8				nPhase;			/**>识别相位				*/
    int32				signal1;		/**>识别信号强度1				*/
    int32				signal2;		/**>识别信号强度2				*/
    int32				bwSignal;		/**>识别基波信号强度			*/
    int32				noise;			/**>背景噪声强度				*/
    OOP_BITSTR128_T		keyWords;		/**>特征字					*/
    OOP_OCTETVAR16_T	rsv;			/**>扩展信息				*/
}OOP_TOPLOGYITEM_T;

typedef struct tag_OOP_TOPLOGYLOG_T
{
    uint32					 nIndex;							 /**>事件记录序号	*/
    OOP_DATETIME_S_T		 tmStart;							 /**>事件发生时间	*/
    OOP_DATETIME_S_T		 tmEnd; 							 /**>事件结束时间	*/
    OOP_REPORTS_T			 evtState;							 /**>事件发生状态	*/
    OOP_EVTSOURCE_T 		 source;							 /**>事件发生源		*/
    OOP_EVTBUF_T			 data;								 /**>事件关联数据	*/	
    uint32					 nToplogyNum;
    OOP_TOPLOGYITEM_T	     item[MAX_TOPLOGYITEM_NUM];			 /**>识别结果信息	*/	
}OOP_TOPLOGYLOG_T;

/**
********************************************************
* A.6 参变量类对象
********************************************************
*/

/** @brief 时钟同步类型   */
typedef enum tag_OOP_SYNCTYPE
{
    SYNC_MASTER     = 0,
    SYNC_LINKAUTO   = 1,
    SYNC_GPS        = 2,
    SYNC_OTHER      = 255
}OOP_SYNCTYPE_E;

/** @brief 心跳校时参数   */
typedef struct tag_OOP_SYNCCLOCK
{
    uint8   nSum;           /**< 最近心跳时间总个数    */
    uint8   nMaxDelNum;     /**< 最大值剔除个数      */
    uint8   nMinDelNum;     /**< 最小值剔除个数      */
    uint8   nDelay;         /**< 通讯延时阈值     */
    uint8   nValidNum;      /**< 最少有效个数     */
}OOP_SYNCCLOCK_T;

/** @brief 广播校时参数   */
typedef struct tag_OOP_BROADCASTCLOCK
{
    uint16  nMinDeviate;        /**< 最小校时偏差     */
    uint16  nMaxDeviate;        /**< 最大校时偏差     */
}OOP_BROADCASTCLOCK_T;

/** @brief 时间同步周期           */
typedef struct tag_OOP_SYNCCYCLE
{
    uint32  norperiod;      /**< 北向同步周期 */
    uint32  souperiod;      /**< 南向同步周期 */
}OOP_SYNCCYCLE_T;

/** @brief 地理位置信息			*/
typedef struct tag_POSITION_OLD_
{
    uint8	    position;		/**< 方位     经度:{0：E,1：W}   纬度:{0：S,1：N}	*/
    uint8 	    degre;			/**< 度	*/
    uint8	    branch;			/**< 分	*/
    uint8 	    second;			/**< 秒	*/
}POSITION_OLD_;

/** @brief 地理位置信息           */
typedef struct tag_POSITION_
{
    uint8       position;       /**< 方位     经度:{0：E,1：W}   纬度:{0：S,1：N} */
    uint8       degre;          /**< 度    */
    uint8       branch;         /**< 分    */
    uint8       second;         /**< 秒    */
}POSITION_;

/** @brief 地理位置信息 新 */
typedef struct tag_POSITION_NEW
{
    uint8       position;       /**< 方位     经度:{0：E,1：W}   纬度:{0：S,1：N} */
    uint16      degre;          /**< 度    */
    float       branch;         /**< 分    */
}POSITION_NEW;

/** @brief 4004-设备地理位置	*/
typedef struct tag_OOP_GEOGADD_OLD
{
    POSITION_OLD_		longitude;		/**< 经度	*/
    POSITION_OLD_ 		latitude;		/**< 纬度	*/
    uint32				height;   		/**< 高度	*/
}OOP_GEOGADD_OLD_T;

/** @brief 4004-设备地理位置新  */
typedef struct tag_OOP_GEOGADD_NEW
{
    POSITION_NEW    longitude;      /**< 经度 */
    POSITION_NEW    latitude;       /**< 纬度 */
    int             height;         /**< 高度 */
}OOP_GEOGADD_NEW_T;

/** @brief 4004-设备地理位置  */
typedef struct tag_OOP_GEOGADD
{
    POSITION_       longitude;      /**< 经度 */
    POSITION_       latitude;       /**< 纬度 */
    int32           height;   		/**< 高度	*/
}OOP_GEOGADD_T;

/* 统一地理位置 */
typedef struct tag_OOP_GPSADDR_ALL
{
    uint8  type;  //0-旧协议，1-新协议
    union
    {
        OOP_GEOGADD_T      oldaddr;
        OOP_GEOGADD_NEW_T  newaddr;
    };
}OOP_GEOGADD_A_T;

#define OOP_MAX_GROUPADD        (8)     /**< 最多组地址个数  */

/** @brief 4005-组地址   */
typedef struct tag_OOP_GRPADDR
{
    uint32      nNum;
    uint32      rsv;
    OOP_SA_T    sa[OOP_MAX_GROUPADD];
}OOP_GRPADDR_T;

/** @brief 4006-时钟源   */
typedef struct tag_OOP_CLOCK
{
    uint8   clock;      /**< 时钟源(enum)    */
    uint8   state;      /**< 状态(enum)       */
}OOP_CLOCK_T;

/** @brief 4007-LCD */
typedef struct tag_OOP_LCD
{
    uint8       disTime;                /**< 上电全显时间                                                     */
    uint16      lightTime;              /**< 背光点亮时间                                                     */
    uint16      reTime;                 /**< 显示查看背光点亮时间                                             */
    uint16      noPow;                  /**< 无电按键屏幕驻留最大时间                                           */
    uint8       eDotNum;                /**< 显示电能小数位数                                                   */
    uint8       pDotNum;                /**< 显示功率(最大需量)小数位数                                         */
    uint8       nFlag;                  /**< 液晶①②字样意义 0显示当前套、备用套时段，1显示当前套、备用套费率   */
}OOP_LCD_T;

/** @brief 400C-时区时段数 */
typedef struct tag_OOP_PERIOD
{
    uint8    yPeriod;            /**< 年时区数      */
    uint8    dPeriodMet;         /**< 日时段表数   */
    uint8    dPeriod;            /**< 日时段数      */
    uint8    rate;               /**< 费率数     */
    uint8    pubHoliday;         /**< 公共假日数   */
}OOP_PERIOD_T;

#define OOP_MAX_PUBHOLIDAY      (20)    /**< 最大公共假日个数   */

/** @brief 4011-公共假日    */
typedef struct tag_OOP_PUBHOLIDAY
{
    uint32          nNum;
    uint32          rsv;
    PUBHOLIDAY      holiday[OOP_MAX_PUBHOLIDAY];    
}OOP_PUBHOLIDAY_T;

#define OOP_MAX_TIMEZONE        (14)    /**< 年时区数       */

/** @brief 时区定义 */
typedef struct tag_TIMEZONE
{
    uint8    mon;                /**< 月                   */
    uint8    day;                /**< 日                   */
    uint8    meterNum;           /**< 日时段表号、费率号   */
}TIMEZONE;

/** 
* @brief    时区表               \n
*           4014-当前套时区表 \n
*           4015-备用套时区表
*/
typedef struct tag_OOP_TIMEZONE
{
    uint32          nNum;
    uint32          rsv;
    TIMEZONE        period[OOP_MAX_TIMEZONE];
}OOP_TIMEZONE_T;

#define OOP_MAX_DAYPERIODS      (8)    /**< 最大日时段表数       */

/** 
* @brief    时段表               \n
*           4016-当前套日时段表  \n
*           4017-备用套日时段表
*/
typedef struct tag_OOP_DAYPERIOD
{
    uint32        nNum;
    uint32        rsv;
    DAYPERIOD     dPeriod[OOP_MAX_DAYPERIODS]; 
}OOP_DAYPERIOD_T;

/** 
* @brief    费率电价            \n
*           4018-当前套费率电价  \n
*           4019-备用套费率电价
*/
typedef struct tag_OOP_RATETARIFF
{
    uint32      nNum;
    uint32      rsv;
    uint32      nValue[OOP_MAX_RATES];
}OOP_RATETARIFF_T;

/** @brief 401E-报警金额限值  */
typedef struct tag_OOP_LIMITCHARGE
{
    uint32 charge1;
    uint32 charge2;
}OOP_LIMITCHARGE_T;

/** @brief 401F-其它金额限值  */
typedef struct tag_OOP_OTHERLIMIT
{
    uint32 overChar;
    uint32 cornorChar;
    uint32 switchChar;
}OOP_OTHERLIMIT_T;

/** @brief 4020-报警电量限值  */
typedef struct tag_OOP_POWERLIMIT
{
    uint32 power1;
    uint32 power2;
}OOP_POWERLIMIT_T;

/** @brief 4021-其它电量限值  */
typedef struct tag_OOP_OTHERPOWERLIMIT
{
    uint32 overPow;
    uint32 cornorPow;
    uint32 switchPow;
}OOP_OTHERPOWERLIMIT_T;

/** 
* @brief    年结算日            \n
*           401A-当前套阶梯电价  \n
*           401B-备用套阶梯电价
*/

#define OOP_MAX_YEARACCDAY      (12)    /**< 最大年结算日     */
#define OOP_MAX_STEPS           (12)    /**< 最大阶梯数            */

/** @brief 结算日    */
typedef struct tag_YEARACCDAY
{
    uint8        mon;
    uint8        day;
    uint8        hour;
}YEARACCDAY;

/** @brief 阶梯电价 */
typedef struct tag_OOP_STEPPRICE
{
    uint32          nNum1;
    uint32          sValue[OOP_MAX_STEPS];                  /**< 阶梯值      */
    uint32          nNum2;
    uint32          sPrice[OOP_MAX_STEPS];                  /**< 阶梯电价   */
    uint32          nNum3;
    YEARACCDAY      yAccDay[OOP_MAX_YEARACCDAY];            /**< 年结算日   */
}OOP_STEPPRICE_T;

//
// 4025-采集器升级结果列表
//

#define OOP_MAX_DCU     512         /**< 最大采集器个数      */

/** @brief 升级结果 */
typedef struct tag_DC_UPGRADE
{
    uint16              nIndex;
    OOP_TSA_T           add;
    uint8               result;
    OOP_DATETIME_S_T    tmRetry;
    OOP_DATETIME_S_T    tmSuccess;
    uint16              nBroadBlocks;
    uint16              nRetryBlocks;
    OOP_VERSION_T       preVer;
    OOP_VERSION_T       aftVer;
}DC_UPGRADE_T;

/** @brief 一组升级结果   */
typedef struct tag_OOP_DC_UPDATE
{
    uint32         nNum;
    uint32         rsv;
    DC_UPGRADE_T   DcUpGrade[OOP_MAX_DCU];
}OOP_DC_UPDATE_T;

/** @brief 采集器升级参数配置  */
typedef struct tag_OOP_DCU_CONFIG
{
    uint8   nBroadCount;
    uint8   nRetryDays;
}OOP_DCU_CONFIG_T;

/** @brief 4027-操作结果	*/
typedef enum
{
    OR_INIT						= 0,
    OR_START_SUCCESS			= 1,
    OR_DOWNLODING				= 2,
    OR_DOWN_SUCCESS				= 3,
    OR_CHECK_SUCCESS			= 4,
    OR_UPGRADE_WAIT				= 5,
    OR_UPGRADE_SUCCESS			= 6,
    OR_HARDVER_UNMATCH			= 20,
    OR_SOFTVER_UNMATCH			= 21,
    OR_FLASH_WRITE_FAILED		= 22,
    OR_FLASH_READ_FAILED		= 23,
    OR_PROGRAM_FAILED			= 24,
    OR_CHECK_WHOLE_FAILED		= 25,
    OR_CHECK_REALY_FAILED		= 26,
    OR_UPGRADE_TIME_BAD			= 27,
    //
    OR_SYSUPGRADE_PARAM_ERROR		= 31,
    OR_SYSUPGRADE_SIGN_ERROR		= 32,
    OR_SYSUPGRADE_CHKSUM_ERROR		= 33,
    OR_SYSUPGRADE_FROMAT_ERROR		= 34,
    OR_SYSUPGRADE_VERSION_ERROR		= 35,
    OR_SYSUPGRADE_VERSION_ROLLBACK	= 36,
    OR_SYSUPGRADE_SPACE_ERROR		= 37,
    OR_SYSUPGRADE_PROGING			= 38,
    //
    OR_CONTAINER_INSTALL_ERROR	= 40,
    OR_CONTAINER_PARAM_ERROR	= 41,
    OR_CONTAINER_PACKAGE_ERROR	= 42,
    //
    OR_APP_PACKAGE_ERROR		= 50,
    OR_APP_INSIDE				= 51,
    OR_APP_SIGN_ERROR			= 52,
    //
    OR_UPGRADE_OTHER_ERROR		= 255
} OOP_OPERATE_RESULT_E;

typedef enum
{
    OT_INSTALL		= 0,
    OT_START		= 1,
    OT_STOP			= 2,
    OT_UNINSTALL	= 3
} OOP_OPERATE_TYPE_E;

/** @brief 4030-电压合格率参数   */
typedef struct tag_OOP_VOLTPARAM
{
    uint16  uupVolt;
    uint16  lldVolt;
    uint16  upVolt;
    uint16  ldVolt;
}OOP_VOLTPARAM_T;

/** @brief 4031-电能表模组列表   */
#define OOP_MAX_METER_MOD   (4)

/** @brief 4031属性2-模组 */
typedef struct tag_OOP_METERMOD_UNIT
{
    OOP_VISIBLEVAR128_T szName;         /**< 模组描述符 */
    uint8               nVxd;           /**< 逻辑地址 */
    OOP_BITSTR16_T      modType;        /**< 模组类别 */
    OOP_OCTETSTR8_T     serialNum;      /**< 模组序列号 */
    uint8               portFlag;       /**< 模组通信端口标记,0:无,1:有 */
    OOP_OAD_U           port;           /**< 模组通信端口optional */
}OOP_METERMOD_UNIT_T;

typedef struct tag_OOP_METERMOD_LIST
{
    uint32 nNum;
    uint32 rsv;
    OOP_METERMOD_UNIT_T info[OOP_MAX_METER_MOD];
}OOP_METERMOD_LIST;

/** @brief 4031-模组版本信息     */
typedef struct tag_OOP_MODVERSION
{
    uint8                   nVxd;       /**< 模组逻辑地址 */
    OOP_VISIBLESTR4_T       szFactory;  /**< 厂商代码           */
    OOP_VISIBLESTRVAR16_T   szSoftVer;  /**< 软件版本号            */
    OOP_VISIBLESTR6_T       szSoftDate; /**< 软件版本日期     */
    OOP_VISIBLESTRVAR16_T   szHardVer;  /**< 硬件版本号            */
    OOP_VISIBLESTR6_T       szHardDate; /**< 硬件版本日期     */
    OOP_VISIBLESTRVAR16_T   szExtend;   /**< 厂家扩展信息     */
}OOP_MODVERSION_T;

typedef struct tag_OOP_MODVERSION_LIST
{
    uint32 nNum;
    uint32 rsv;
    OOP_MODVERSION_T    info[OOP_MAX_METER_MOD];
}OOP_MODVERSION_LIST;

/** @brief 4038-系统管理 */

/** @brief 文件信息 */
typedef struct tag_OOP_FILE_SIGN
{
    OOP_VISIBLEVAR_T	signName;
    OOP_VISIBLEVAR_T	signPath;
    uint32				signSize;
    //OOP_OCTETVAR64_T	signMD5;
    OOP_CHECKSUM_T      signcheck; //24规范中文件签名校验类型变更为checksum
} OOP_FILE_SIGN;

typedef struct tag_OOP_FILE_INFO
{
    OOP_VISIBLEVAR_T	fileName;
    OOP_VISIBLEVAR_T	fileType;
    OOP_VISIBLEVAR_T	filePath;
    uint32				fileSize;
    //OOP_OCTETVAR64_T	fileMD5;
    OOP_CHECKSUM_T      filecheck;  //24规范中文件校验类型变更为checksum
    OOP_FILE_SIGN		sign;
} OOP_FILE_INFO_T;

typedef struct tag_OOPM_OS_UPGRADE
{
    OOP_VISIBLEVAR_T	ver;	// 升级后版本
    uint8				type;	// 升级类型
    OOP_FILE_INFO_T		file;	// 升级文件信息
} OOPM_OS_UPGRADE_T;

/** @brief 4040-模块信息 */
typedef struct tag_OOP_TASPCUSEMOD
{
    OOP_OCTETSTR8_T     modID;          /**< 模块ID */
    OOP_VISIBLESTR4_T   manufcode;      /**< 厂商代号 */
    OOP_VISIBLESTR8_T   devcode;        /**< 设备编号 */
    OOP_VISIBLESTR4_T   runsoftver;     /**< 运行区软件版本号 */
    OOP_DATE_T          runsoftdate;    /**< 运行区软件发布日期 */
    OOP_VISIBLESTR4_T   baksoftver;     /**< 备份区软件版本号 */
    OOP_DATE_T          baksoftdate;    /**< 备份区软件发布日期 */
    OOP_VISIBLESTR11_T  cfgcapinfo;     /**< 配置容量信息码 */
    OOP_VISIBLESTR4_T   protover;       /**< 通信协议版本号 */
    OOP_VISIBLESTR4_T   hardver;        /**< 硬件版本号 */
    OOP_DATE_T          harddate;       /**< 硬件发布日期 */
}OOP_TASPCUSEMOD_T;

/** @brief 4040-安全认证信息 */
typedef struct tag_OOP_SAFEAUTHINFO
{
    OOP_OCTETSTR8_T     safemodserial;  /**< 安全模块序列号 */
    OOP_OCTETSTR4_T     cursecuritysta; /**< 当前秘钥状态 */
    OOP_OCTETSTR4_T     dblauthstate;   /**< 双向认证状态 */
}OOP_SAFEAUTHINFO_T;

#define OOP_TA_NUM (3)

/** @brief 4040-频率有效值 */
typedef struct tag_OOP_FREQVAL
{
    uint32 nNum;
    uint32 rsv;
    uint32 freq[OOP_TA_NUM];
}OOP_FREQVAL_T;

/** @brief 4040-工频电流有效值 */
typedef struct tag_OOP_WORKCUR_VALID
{
    uint32 nNum;
    uint32 rsv;
    uint32 curval[OOP_TA_NUM];
}OOP_WORKCUR_VALID_T;

/** @brief 4040-秘钥更新 */
typedef struct tag_OOP_SECURITYUPDATE
{
    uint8               num;
    OOP_OCTETVAR2K_T    securityfile;
}OOP_SECURITYUPDATE_T;

/** @brief 4040-换算及单位 */
typedef struct tag_OOP_TAUNIT
{
    OOP_SCALER_UNIT_T       unit1;
    OOP_SCALER_UNIT_T       unit2;
    OOP_SCALER_UNIT_T       unit3;
    OOP_SCALER_UNIT_T       unit4;
    OOP_SCALER_UNIT_T       unit5;
}OOP_TAUNIT_T;

/** @brief 4040-互感器信息单元 */
typedef struct tag_OOP_CTINFO_UNIT
{
    OOP_OCTETVAR256_T       id;         /**< 互感器ID            */
    OOP_VISIBLESTR4_T       szSoftVer;  /**< 软件版本号            */
    OOP_VISIBLESTR6_T       szSoftDate; /**< 软件版本日期         */
    OOP_VISIBLESTR4_T       szHardVer;  /**< 硬件版本号            */
    OOP_VISIBLESTR6_T       szHardDate; /**< 硬件版本日期         */
    OOP_VISIBLESTR8_T       szFactory;  /**< 厂商编码           */
}OOP_CTINFO_UNIT_T;

#define OOP_MAX_CTINFO  (4)

/** @brief 互感器信息  */
typedef struct tag_OOP_CTINFO
{
    uint32              nNum;
    uint32              rsv;
    OOP_CTINFO_UNIT_T   item[OOP_MAX_CTINFO];
}OOP_CTINFO_T;

/** @brief 4041-电流回路监测使能 */
typedef struct tag_OOP_CURLOOPCKENABLE
{
    BOOLEAN                 phaseA;
    BOOLEAN                 phaseB;
    BOOLEAN                 phaseC;
}OOP_CURLOOPCKENABLE_T;

/** 
* @brief    410C-电导系数   \n
*           410D-电抗系数   \n
*           410E-电阻系数   \n
*           410F-电纳系数
*/
typedef struct tag_OOP_COEFFICIENT
{
    int16   valueA;     /**< A相   */
    int16   valueB;     /**< B相   */
    int16   valueC;     /**< C相   */
}OOP_COEFFICIENT_T;

/** @brief 4111-软件备案号列表 */
typedef struct tag_OOP_SOFTREC_T
{
    OOP_VISIBLESTR2_T       type;       /**< 备案类型 */
    OOP_VISIBLESTR4_T       factory;    /**< 厂商代码           */
    uint8                   softType;   /**< 程序备案类型 */
    OOP_VISIBLESTR8_T       date;       /**< 备案日期 */
    uint8                   seq;        /**< 当日备案序号 */
    OOP_VISIBLESTR16_T      no;         /**< 备案号 */
}OOP_SOFTREC_T;

typedef struct tag_OOP_SOFTREC_LIST
{
    uint32 nNum;
    uint32 rsv;
    OOP_SOFTREC_T    info[OOP_MAX_METER_MOD];
}OOP_SOFTREC_LIST;

//
// 4116 -结算日
//

#define OOP_MAX_PAYDAY  (8) /**< 最大年结算日个数   */

/** @brief 结算日定义  */
typedef struct tag_OOP_PAYDAY
{
    uint8   day;
    uint8   hour;
}OOP_PAYDAY_T;

/** @brief 结算日数组  */
typedef struct tag_OOP_PAYDAYS
{
    uint32          nNum;
    uint32          rsv;
    OOP_PAYDAY_T    cols[OOP_MAX_PAYDAY];
}OOP_PAYDAYS_T;

#define OOP_MAX_CASCADE     (32)        /**< 最大级联地址数量   */

/** @brief 4202-级联通讯数据  */
typedef struct tag_OOP_CASCADE
{
    BOOLEAN     flag;                       /**< 级联标志           */
    OOP_OAD_U   port;                       /**< 级联通信端口号      */
    uint16      tdelay;                     /**< 总等待超时(100)   */
    uint16      bdelay;                     /**< 字节超时(10)       */
    uint8       retry;                      /**< 重发次数           */
    uint8       cycle;                      /**< 巡测周期           */
    uint8       casnum;                     /**< 级联(被)端口数   */
    OOP_TSA_T   addr[OOP_MAX_CASCADE];      /**< 级联(被)终端地址    */
}OOP_CASCADE_T;

/** @brief 4203-终端上行通信状态参数  */
typedef struct tag_OOP_UPLINKCOMMSTATE
{
    BOOLEAN    call;               /**< 允许终端与主站通话 */
    BOOLEAN    report;             /**< 允许主动上报      */
}OOP_UPLINKCOMMSTATE_T;

//
// 4204-终端广播校时
//

/** @brief 广播校时参数   */
typedef struct tag_OOP_SYNCTIME
{
    OOP_TIME_T  time;
    BOOLEAN     bValid;
}OOP_SYNCTIME_T;

/** @brief 点对点校时参数    */
typedef struct tag_OOP_P2PSYNCTIME
{
    char        offset;
    OOP_TIME_T  time;
    BOOLEAN     bValid;
}OOP_P2PSYNCTIME_T;

//
// 4800-非介入式负荷感知模块
//

/** @brief 电器设备配置   */
#define    OOP_MAX_DEV            15

typedef struct tag_OOP_ELECDEV
{
    uint16        nClsNum;          /**> 设备类号 */
    OOP_VARIANT_T devPro;           /**> 设备特征 */
}OOP_ELECDEV_T;

typedef struct tag_OOP_DEVLIST
{
    uint32 nNum;
    uint32 rsv;
    OOP_ELECDEV_T dev[OOP_MAX_DEV];
}OOP_ELECDEV_LIST_T;

/** @brief 电器设备当前数据集    */
typedef struct tag_OOP_DEVINSTANTDATA
{
    uint16 devClsID;                /**< 设备类号 */
    uint8  devStatus;                /**< 设备状态 */
    int32  power;                   /**< 功率       */
    //uint32 nEnergy;                 /**< 电量       */
}OOP_DEVINSTANTDATA_T;

typedef struct tag_OOP_DEVINSTANTDATAS
{
    uint32 nNum;
    uint32 rsv;
    OOP_DEVINSTANTDATA_T data[OOP_MAX_DEV];
}OOP_DEVINSTANTDATAS_T;


/** @brief 电器设备冻结数据集  */
typedef struct tag_OOP_DEVFREZDATA
{
    uint16 devClsID;                /**< 设备类号 */
    uint32 nEnergy;                 /**< 电量       */
    int    nPower;                  /**< 功率       */   
    uint8  startTime;               /**< 起动时间       */
    uint8  stopTime;                /**< 停止时间       */
    uint8  nCount;                  /**< 合并启停数       */
}OOP_DEVFREZDATA_T;

typedef struct tag_OOP_DEVFREZDATAS
{
    uint32 nNum;
    uint32 rsv;
    OOP_DEVFREZDATA_T data[OOP_MAX_DEV];
}OOP_DEVFREZDATAS_T;

/** @用户负荷	*/
#define OOP_MAX_LOAD_CHANGE_BUF		230
#define OOP_MAX_LOAD_CHANGE_DATA	1
#define OOP_MAX_LOAD_CHANGE_UNIT	5
#define OOP_MAX_LOAD_NUM			6

typedef struct tag_OOP_LOAD_Region{
    unsigned char	nType;
    int				start;
    int				end;
}OOP_LOAD_Region_T;

typedef struct tag_OOP_LOAD_I0_PARA
{
    OOP_LOAD_Region_T regionI0;	//电流区间
    int nIvalue;	//零线电流系数 
}OOP_LOAD_I0_PARA;

//电能表零线电流异常事件
typedef struct tag_OOP_LOAD_I0_EVECFG_T
{
    uint32 nADown;
    uint16 wValue;
    int nDelay;//相位系数 array double-long-unsigne
}OOP_LOAD_I0_EVECFG_T;

//电能表零线电流异常事件
typedef struct tag_OOP_TERMINAL_LIMIT_T
{
    uint32 ntmp1;
    uint32 ntmp2;
}OOP_TERMINAL_LIMIT_T;


//区间计量系数
typedef struct tag_OOP_LOAD_MET_PARA_T
{
    OOP_LOAD_Region_T regionI;//电流区间 Region（单位：A，换算：-3），
    uint32 nVNum;
    int nVolvalue[OOP_MAX_LOAD_NUM];  //电压系数 array double-long-unsigned，
    uint32 nINum;
    int nIvalue[OOP_MAX_LOAD_NUM];    //电流系数 array double-long-unsigned，
    uint32 nPhaNum;
    int nPhasevalue[OOP_MAX_LOAD_NUM];//相位系数 array double-long-unsigne
}OOP_LOAD_MET_PARA_T;

typedef struct tag_OOP_LOAD_JL_PARA //计量配置信息 48040200
{
    uint32 nJLNum;
    OOP_LOAD_MET_PARA_T  ametCfgCoe[OOP_MAX_LOAD_NUM];//区间计量系数 array 一区间计量系数，
    uint32 nI0Num;
    OOP_LOAD_I0_PARA	ai0Coe[OOP_MAX_LOAD_NUM];	//零线电流系数 array 一区间零线电流系数，
    uint8	dotNum;				//偏移小数位数 unsigned，
    uint16  ponitNum;			//采样周波点数 unsigned long，
    uint8	dataLen;			//采样数据长度 unsigned
}OOP_LOAD_JL_PARA_T;

typedef struct tag_OOP_LOAD_ANALYSE_PARA // 计量配置信息 48040400
{
    int nAlgorithmCompany;	//算法厂家编号
    int nAlgorithmScene;	//算法场景编号
    uint32 nNum;
    int data[OOP_MAX_LOAD_CHANGE_BUF];	//负荷辨识分析数据
}OOP_LOAD_ANALYSE_PARA_T;

typedef struct tag_OOP_LOAD_ANALYSE_DATA
{
    int nAlgorithmCompany;	//算法厂家编号
    int nAlgorithmScene;	//算法场景编号
    OOP_DATETIME_S_T time;	//数据时间
    uint32 nNum;
    int data[OOP_MAX_LOAD_CHANGE_BUF];	//负荷辨识分析数据
}OOP_LOAD_ANALYSE_DATA_T;

typedef struct tat_OOP_EVENT_RECORD
{
    uint32				nIndex;			/**< 事件记录序号	*/
    OOP_DATETIME_S_T	tmStart;		/**< 事件发生时间	*/
    OOP_DATETIME_S_T	tmEnd;			/**< 事件结束时间	*/
    uint32				source;			/**< 事件发生源，enum{1：发生，0：恢复};	*/
    OOP_REPORTS_T		state;			/**< 事件上报状态	*/
}OOP_EVENT_RECORD;

#pragma pack(1)
/** @用户设备负荷异动	331B*/
typedef struct tag_OOP_LOAD_CHANGE_UNIT
{
    OOP_EVENT_RECORD	record;
    uint32				nNum;
    OOP_LOAD_ANALYSE_DATA_T analyseData[OOP_MAX_LOAD_CHANGE_DATA];	//事件负荷异动辨识数据
}OOP_LOAD_CHANGED_UNIT_T;

/** @用户设备启停	331D*/
typedef struct tag_OOP_DEVICE_RUN_STOP_UNIT
{
    OOP_EVENT_RECORD record;
    uint32	devName;	//设备名称号
    uint16	devType;	//设备类号
    uint8	devAttr;	//设备属性(enum{0:阻性负载，1：容性负载，2：感性负载，3：混合，4：其他})
    uint8	devState;	//设备状态(enum{0：停机，1：启动，2：负载上升，3：负载下降，4：负载波动，5：其他})
    int nAlgorithmCompany;	//算法厂家编号
    int nAlgorithmScene;	//算法场景编号
    uint32 nNum;
    int data[OOP_MAX_LOAD_CHANGE_BUF];	//负荷辨识分析数据
}OOP_DEV_RUN_STOP_UNIT;
#pragma pack()

/**
********************************************************
* A.8 采集监控类对象
********************************************************
*/

#define OOP_MAX_TASK        (256)       /**< 最大任务数            */
#define OOP_MAX_NTASK       (256)       /**< 最大普通任务数      */
#define OOP_MAX_ETASK       (256)       /**< 最大事件任务数      */
#define OOP_MAX_TTASK       (256)       /**< 最大透明任务数      */
#define OOP_MAX_RTASK       (256)       /**< 最大上报任务数      */
#define OOP_MAX_CTASK	    (256)	    /**< 最大转加密任务数	*/
#define OOP_MAX_CTASKRES    (256)       /**< 最大转加密任务结果集 */
#define OOP_MAX_TTASKRES    (256)       /**< 最大透明任务结果数    */
#define OOP_TASK_PRIO       (256)       /**< 最大任务优先级      */
#define OOP_MAX_ACQ         (512)       /**< 最大采集监控数      */

//
// 6001-采集档案配置单元
//

#define OOP_MAX_ANNEX_LEN   (16)        /**< 最大附加信息域长度    */
#define OOP_MAX_ANNEX_NUM   (6)         /**< 最大附加信息域数量    */
#define OOP_METER_IOT_TYPE  (0x0101)    /**< 698协议物联网表	*/
#define OOP_METER_20_TYPE   (0x0100)    /**< 698协议20表		*/

/** @brief 协议类型枚举   */
typedef enum tag_METER_PROTO
{
    PROTO_UNKNOW = 0,
    PROTO_97,
    PROTO_07,
    PROTO_698,          /**< 本协议          */
    PROTO_188,          /**< CJ/T 188-2004  */
    PROTO_PDLOT,          /**>温湿度传感器规约*/
    PROTO_INVALID
}OOP_METER_PROTO_E;

/** @brief 电能表接线方式枚举  */
typedef enum tag_METER_PWRTYPE
{
    PWRTYPE_UNKNOWN  = 0,       /**< 未知(0)      */
    PWRTYPE_SINGLE   = 1,       /**< 单相(1)      */
    PWRTYPE_3PHASE3  = 2,       /**< 三相三线(2)    */
    PWRTYPE_3PHASE4  = 3        /**< 三相四线(3)    */
}OOP_METER_PWRTYPE_E;

/** @brief 表计基本信息   */
typedef struct tag_Meter_Basic
{
    OOP_TSA_T       tsa;                        /**< 表计地址   */
    uint8           baud;                       /**< 波特率      */
    uint8           protocol;                   /**< 规约类型   */
    OOP_OAD_U       port;                       /**< 端口     */
    char            password[OOP_PWD_LEN];      /**< 通信密码   */
    uint8           pwdLen;                     /**< 密码长度   */
    uint8           rateNum;                    /**< 费率个数   */
    uint8           userType;                   /**< 用户类型   */
    uint8           pwrType;                    /**< 接线方式   */
    uint16          stVolt;                     /**< 额定电压   */
    uint16          stAmp;                      /**< 额定电流   */
}OOP_METER_BASIC_T;

/** @brief 表计扩展信息   */
typedef struct tag_Meter_Extended
{
    OOP_TSA_T        add;               /**< 采集器地址    */
    uint8            astLen;
    uint8            assetNo[32];       /**< 资产号      */
    uint16           PT;                /**< PT         */
    uint16           CT;                /**< CT         */
}OOP_METER_EXTENDED_T;

/** @brief 附加信息单元   */
typedef struct tag_AnnexObject
{
    OOP_OAD_U   oad;
    uint32      nLen;
    uint8       value[OOP_MAX_ANNEX_LEN];
}OOP_ANNEXOBJECT_T;

/** @brief 附件信息集合   */
typedef struct tag_Meter_Annex
{
    uint32              nNum;
    uint32              rsv;
    OOP_ANNEXOBJECT_T   annexObj[OOP_MAX_ANNEX_NUM];
}OOP_METER_ANNEX_T;

/** @brief 电能表档案  */
typedef struct tag_OOP_METER
{
    uint16                  nIndex;     /**< 配置序号   */
    OOP_METER_BASIC_T       basic;      /**< 基本信息   */
    OOP_METER_EXTENDED_T    extend;     /**< 扩展信息   */
    OOP_METER_ANNEX_T       annex;      /**< 附属信息   */
}OOP_METER_T;

/** @brief 更新电能表基本信息  */
typedef struct tag_OOPM_METER_129
{
    uint16              index;
    OOP_METER_BASIC_T   basic;
}OOPM_METER_129_T;

/** @brief 更新电能表扩展信息  */
typedef struct tag_OOPM_METER_130
{
    uint16                  index;
    OOP_METER_EXTENDED_T    ext;
    OOP_METER_ANNEX_T       annex;
}OOPM_METER_130_T;

/** @brief 删除电能表档案    */
typedef struct tag_OOPM_METER_133
{
    OOP_TSA_T       add;
    OOP_OAD_U       port;
}OOP_METER_M133_T;

/** @brief 更新档案附属信息方法   */
typedef struct tag_OOPM_METER_135
{
    OOP_TSA_T           add;
    OOP_METER_ANNEX_T   annex;
}OOPM_METER_135_T;

//
// 6002-搜表
//

#define OOP_MAX_TIMINGSEARCH    (8)     /**< 定时搜表数量 */
#define OOC_MAX_CROSSMETERS     (64)    /**< 跨台区电表数量  */

/** @brief 搜表参数 */
typedef struct tag_OOP_SEARCHPARAM
{
    BOOLEAN     periodSearch;           /**< 是否启用每天周期搜表 */
    BOOLEAN     atuoUpdate;             /**< 自动更新采集档案       */
    BOOLEAN     touchEvent;             /**< 是否产生搜表事件       */
    uint8       clearChoice;            /**< 清空搜表结果选项(enum) */
}OOP_SEARCHPARAM_T;

/** @brief 定时搜表参数   */
typedef struct tag_OOP_TIMINGSEARPARAM
{
    OOP_TIME_T    start;            /**< 开始时间       */
    uint16        time;             /**< 搜表时长(min)  */
}OOP_TIMINGSEARPARAM_T;

/** @brief 定时搜表参数集合 */
typedef struct tag_OOP_TIMINGSEARCH
{
    uint32                  nNum;
    uint32                  rsv;
    OOP_TIMINGSEARPARAM_T   period[OOP_MAX_TIMINGSEARCH];
}OOP_TIMINGSEARCH_T;

//
// 6013-任务配置单元
//

#define OOP_MAX_WORKPERIOD  (16)            /**< 最多工作时段数  */      

/** @brief 任务类型 */
typedef enum tag_Plan_Type
{
    PLAN_NORMAL = 1,            /**< 普通采集方案 */
    PLAN_EVENT,                 /**< 事件采集方案 */
    PLAN_FORWARD,               /**< 透明方案       */
    PLAN_REPORT,                /**< 上报方案       */
    PLAN_SCRIPT,                /**< 脚本方案       */
    PLAN_ENCRYPT,               /**< 加密方案 */
}OOP_PLAN_TYPE_E;

/** @brief 任务状态 */
typedef enum tag_Plan_State
{
    STATE_NORMAL = 1,           /**< 正常 */
    STATE_DISABLE,              /**< 停用 */
}OOP_PLAN_STATE_E;

/** @brief 任务执行周期   */
typedef struct tag_TASKPERIOD
{
    uint8    startHour;      /**< 起始小时  */
    uint8    startMin;       /**< 起始分钟  */
    uint8    endHour;        /**< 结束小时  */
    uint8    endMin;         /**< 结束分钟  */
}TASKPERIOD;

/** @brief 任务执行周期集合 */
typedef struct tag_WORKPERIOD
{
    uint8       type;                       /**< 类型     */
    uint32      nNum;
    TASKPERIOD  period[OOP_MAX_WORKPERIOD]; /**< 时段表  */
}WORKPERIOD;

/** @brief 任务参数 */
typedef struct tag_OOP_TASK
{
    uint8               id;             /**< 任务ID                               */
    OOP_TI_T            acqFreq;        /**< 执行频率                           */
    uint8               planType;       /**< 方案类型                           */
    uint8               planID;         /**< 方案编号                           */
    OOP_DATETIME_S_T    startTime;      /**< 开始时间(任务第一次开始执行时间)    */
    OOP_DATETIME_S_T    endTime;        /**< 结束时间(可以为空)                 */
    OOP_TI_T            acqDelay;       /**< 延时                             */
    uint8               priority;       /**< 执行优先级                            */
    uint8               state;          /**< 状态1:正常,2:停用                    */
    uint16              startScriptID;  /**< 任务开始前脚本ID                    */
    uint16              endScriptID;    /**< 任务结束后脚本ID                    */
    WORKPERIOD          workPeriod;     /**< 任务运行时段                     */
}OOP_TASK_T;

/** @brief 修改任务状态   */
typedef struct tag_OOPM_TASK_130
{
    uint8       index;
    uint8       state;
}OOPM_TASK_130_T;

/** @brief 补抄   */
typedef struct tag_RetryMetering
{
    OOP_TI_T    ti;
    uint16      last;
}OOP_RETRYMETERING_T;

//
// 6015-普通采集方案
//
typedef enum tag_NORMAL_ROW_TYPE
{
    ROW_CURRENT = 0,        /**< 采集当前数据                 */
    ROW_LAST,               /**< 采集上第N次                   */
    ROW_FROZEN,             /**< 按冻结时标采集                  */
    ROW_TI,                 /**< 按时标间隔采集                  */
    ROW_RETRY,              /**< 补抄                         */
}NORMAL_ROW_TYPE_E;

typedef enum tag_Normal_TM_TYPE
{
    TM_UDF = 0,             /**< 未定义                  */
    TM_START,               /**< 任务开始时间             */
    TM_DAYSTART,            /**< 相对当日0点0分               */
    TM_LDAYEND,             /**< 相对上日23点59分             */
    TM_LDAYSTART,           /**< 相对上日0点0分               */
    TM_MONSTART,            /**< 相对当月1日0点0分           */
    TM_FROZEN,              /**< 数据冻结时标             */
    TM_LMONEND,             /**< 相对上月月末23点59分           */
}Normal_TM_TYPE_E;

typedef struct tag_Record_Row
{
    uint8   choice;             /**< 类型标识   */

    union
    {
        // 采集当前数据
        // NULL
        
        // 按上N次采集
        uint8                   num;

        // 按数据冻结时标采集
        // NULL

        // 在最近一次采集周期范围内按时标间隔采集
        OOP_TI_T                ti;

        // 补抄
        OOP_RETRYMETERING_T     rd;
    };
}OOP_RECORD_ROW_T;

/** @brief 列筛选方案  */
typedef struct tag_Record_Col_Del
{
    uint8       num;
    OOP_CSD_T   csd[OOP_MAX_CSDS_DEL];
}OOP_RECORD_COL_DEL_T;

typedef struct tag_Record_Col
{
    uint8       num;
    OOP_CSD_T   csd[OOP_MAX_CSDS];
}OOP_RECORD_COL_T;

/** @brief 普通采集方案   */
typedef struct tag_OOP_PLAN_NORMAL_DEL
{
    uint8                   planID;         /**< 方案编号       */
    uint16                  memDepth;       /**< 存储深度       */
    OOP_RECORD_ROW_T        recdRow;        /**< 采集方式       */
    OOP_RECORD_COL_DEL_T    recdCol;        /**< 记录列选择        */
    OOP_MSP_T               ms;             /**< 电表集合       */
    uint8                   tmFlag;         /**< 存储时标选择 */
}OOP_PLAN_NORMAL_DEL_T;

typedef struct tag_OOP_PLAN_NORMAL
{
    uint8               planID;         /**< 方案编号       */
    uint16              memDepth;       /**< 存储深度       */
    OOP_RECORD_ROW_T    recdRow;        /**< 采集方式       */
    OOP_RECORD_COL_T    recdCol;        /**< 记录列选择        */
    OOP_MSP_T           ms;             /**< 电表集合       */
    uint8               tmFlag;         /**< 存储时标选择 */
}OOP_PLAN_NORMAL_T;

/** @brief 修改普通方案的列筛选   */
typedef struct tag_OOPM_NTASK_130
{
    uint8               index;
    OOP_RECORD_COL_T    col;
}OOPM_NTASK_130_T;

/** @brief 修改普通方案的列筛选   */
typedef struct tag_OOPM_NTASK_200
{
    uint8               index;
    OOP_MSP_T           ms;
}OOPM_NTASK_200_T;

//
// 6016-事件采集方案
//

#define OOP_MAX_EVENT   (32)                /**< 最大事件采集个数   */

typedef enum tag_Event_Row_Type
{
    ROWE_CYCLE = 0,     // 周期采集事件数据
    ROWE_ALL,           // 根据通知采集所有事件数据
    ROWE_APPOINT,       // 根据通知采集指定事件数据
    ROWE_STORAGE,       // 根据通知存储生成的事件数据
}Event_Row_Type;

/** @brief 事件采集列选择    */
typedef struct tag_EVENTSELECT
{
    uint32      nNum;                       /**< 采集的事件个数      */
    OOP_ROAD_T  cols[OOP_MAX_EVENT];        /**< 采集的事件数据      */
}OOP_EVENTSELECT_T;

/** @brief 事件方案 */
typedef struct tag_OOP_PLAN_EVENT
{
    uint8               planID;             /**< 方案编号       */
    OOP_EVENTSELECT_T   events;             /**< 需抄读的事件 */
    OOP_MSP_T           ms;                 /**< 电表集合       */
    uint8               bReport;            /**< 上报标志(BOOL) */
    uint16              nDepth;             /**< 存储深度       */
    uint8               nType;              /**< 采集类型       */
}OOP_PLAN_EVENT_T;

/** @brief 修改事件方案上报标志   */
typedef struct tag_OOPM_REPORT_130
{
    uint8   plan;
    uint8   bReport;            /**< BOOL   */
}OOPM_REPORT_130_T;

/** @brief 事件采集方案上报通道   */
typedef struct tag_OOP_PLAN_EVENT_CHANNEL
{
    uint8               planID;             /**< 方案编号       */
    OOP_OADS_T          channel;            /**< 上报通道       */
}OOP_PLAN_EVENT_CHANNEL_T;

//
// 6018-透明方案集
//

#define OOP_MAX_TRANS_NUM   (3)         /**< 最多透明报文数量       */
#define OOP_MAX_TRANS_INFO  (4)         /**< 最多透明方案内容组数 */

/** @brief 透明转发命令   */
typedef struct tag_TRANS_CMD
{
    uint8               nIndex;         /**< 报文序号   */
    OOP_OCTETVAR256_T   data;           /**< 报文内容   */
}OOP_TRANS_CMD_T;

/** @brief 透明转发命令集合 */
typedef struct tag_CMD_ARRAY
{
    uint8               nNum;
    OOP_TRANS_CMD_T     value[OOP_MAX_TRANS_NUM];
}OOP_CMD_ARRAY_T;

/** @brief 透明方案控制标志 */
typedef struct tag_TRANS_FLAG
{
    uint8       bReport;    /**< 上报透明方案结果并等待后续报文  */
    uint16      nDelay;     /**< 等待后续报文超时时间(秒)        */
    uint8       nCompare;   /**< 结果比对标识                 */
    struct
    {
        uint8   flag;       /**< 特征字节       */
        uint16  start;      /**< 截取开始       */
        uint16  len;        /**< 截取长度       */
    }cmpParm;               /**< 结果比对参数 */
}OOP_TRANS_FLAG_T;

/** @brief 透明方案内容   */
typedef struct tag_TRANS_INFO
{
    uint16              nIndex;             /**< 序号         */
    OOP_TSA_T           addr;               /**< 通信地址       */
    uint16              startid;            /**< 开始前脚本id  */
    uint16              endid;              /**< 完成后脚本id  */
    OOP_TRANS_FLAG_T    flag;               /**< 方案控制标志 */
    OOP_CMD_ARRAY_T     cmd;                /**< 方案报文集        */
}OOP_TRANS_INFO_T;

/** @brief 透明方案内容集合 */
typedef struct tag_TRANS_ARRAY
{
    uint8               nNum;
    OOP_TRANS_INFO_T    info[OOP_MAX_TRANS_INFO];
}OOP_TRANS_ARRAY_T;

/** @brief 透明方案定义   */
typedef struct tag_OOP_PLAN_TRANS
{
    uint8               planID;         /**< 方案编号   */
    OOP_TRANS_ARRAY_T   value;          /**< 方案内容   */
    uint16              nSaveCount;     /**< 存储深度   */
}OOP_PLAN_TRANS_T;

/** @brief 添加一组报文   */
typedef struct tag_OOPM_TTASK_128
{
    uint8               index;              /**< 方案编号       */
    OOP_TSA_T           addr;               /**< 通信地址       */
    OOP_TRANS_FLAG_T    flag;               /**< 方案控制标志 */
    OOP_CMD_ARRAY_T     cmd;                /**< 方案报文集        */
}OOPM_TTASK_128_T;

/** @brief 删除一个方案一组内容   */
typedef struct tag_OOPM_TTASK_129
{
    uint8       index;
    uint32      nNum;
    OOP_TSA_T   add[OOP_MAX_TRANS_INFO];
}OOPM_TTASK_129_T;

//
// 601B-透明方案结果集
//

/** @brief 透明方案结果单元 */
typedef struct tag_TRANS_RESULT
{
    uint8               nIndex;             /**< 报文序号       */
    OOP_DATETIME_S_T    aTime;              /**< 报文响应时间 */
    OOP_OCTETVAR256_T   data;               /**< 命令结果       */
}TRANS_RESULT;

/** @brief 透明方案结果集合 */
typedef struct tag_TRANSPARENT_RES
{
    uint8           nNum;
    TRANS_RESULT    result[OOP_MAX_TRANS_NUM];  /**< 结果     */
}TRANS_RESULTS;

/** @brief 一个透明方案执行结果   */
typedef struct tag_OOP_TRANSRES
{   
    uint8               planID;                     /**< 方案编号   */
    OOP_DATETIME_S_T    eTime;                      /**< 执行时间   */
    OOP_TSA_T           addr;                       /**< 通信地址   */    
    TRANS_RESULTS       res;                        /**< 结果集      */
}OOP_TRANSRES_T;

typedef struct tag_OOP_TRANS_RESULT
{
    uint16          nIndex;     // 结果集序号，内部维护用
    uint8           flag;       // 0-未使用，1-正在更新中, 2-更新完成
    uint8           bReport;    // 0-禁止上报，1-允许但未上报，2-上报成功
    uint8           nDelay;     // 等待延时
    OOP_TRANSRES_T  res;
}OOP_TRANS_RESULT;

//
// 601D-上报方案集
//

/** @brief 上报记录型对象    */
typedef struct tag_RecordData
{
    OOP_OAD_U       oad;        /**< 主对象属性描述符   */
    OOP_PCSD_T      tRCSD;      /**< 从对象属性描述符   */
    OOP_PRSD_T      tRSD;       /**< 上报数据行选择      */
}RECORDDATA;

/** @brief 上报对象 */
typedef struct tag_REPORT_CONTEXT
{
    uint8   choice;                 /**< 类型标识       */

    union
    {
        OOP_OAD_U       oad;        /**< [0]对象属性数据  */
        RECORDDATA      record;     /**< [1]记录型对象属性   */
    };      
}REPORT_CONTEXT;

/** @brief 上报方案 */
typedef struct tag_OOP_PLAN_REPORT
{
    uint8               planID;     /**< 方案编号               */
    OOP_OADS_T          channel;    /**< 上报通道               */
    OOP_TI_T            timeout;    /**< 上报响应超时时间           */
    uint8               retry;      /**< 最大上报次数             */
    REPORT_CONTEXT      data;       /**< 上报内容               */
}OOP_PLAN_REPORT_T;

//
// 601E-转加密方案集
//

#define OOP_MAX_ENCRYPT_NUM            (16)

typedef enum tag_OOP_ENCRYPT_TYPE
{
    ENCRYPT_TYPE_NML = 0,		/**< 普通任务	*/
    ENCRYPT_TYPE_CLK = 1,		/**< 电表对时	*/
    ENCRYPT_TYPE_SET = 2,		/**< 电表ESAM信息设置	*/
    ENCRYPT_TYPE_SW  = 3		/**< 电表拉合闸 */
}OOP_ENCRYPT_TYPE_E;

typedef struct tag_OOP_ENCRYPT_METER
{
    uint16 metid;
    OOP_OCTETVAR256_T key;
}OOP_ENCRYPT_METER_T;

typedef struct tag_OOP_ENCRYPT_METERS
{
    uint32 nNum;
    uint32 rsv;
    OOP_ENCRYPT_METER_T meter[OOP_MAX_ENCRYPT_NUM];
}OOP_ENCRYPT_METERS_T;

typedef struct tag_OOP_ENCRYPT_DATA
{
    OOP_OCTETVAR256_T apdu;              /**< 加密的apdu		*/
    OOP_OCTETVAR64_T param;             /**< 参数				*/
}OOP_ENCRYPT_DATA_T;

typedef struct tag_OOP_PLAN_ENCRYPT
{
    uint8					planID;			/**< 方案编号		*/
    uint8					planType;		/**< 方案类型		*/
    OOP_ENCRYPT_DATA_T		endata;		    /**< 方案内容		*/
    OOP_ENCRYPT_METERS_T    config;         /**< 配置档案       */
}OOP_PLAN_ENCRYPT_T;

//
// 6020-转加密方案执行结果集
//

/** @brief 一个转加密方案执行结果	*/
typedef struct 
{
    uint32 nNum;
    OOP_OCTETSTR6_T addr[OOP_MAX_ENCRYPT_NUM];
}OOP_METSUCESS_T;

typedef struct tag_OOP_ENCRYPTRES
{   
    uint8				planID;						    /**< 方案编号	*/
    uint8               planType;					    /**< 执行时间	*/
    uint16              nMetTotal;                      /**< 配置档案总个数*/ 
    OOP_METSUCESS_T     ms;                             /**< 成功电能表集合*/
}OOP_ENCRYPTRES_T;

#define MAX_KEY_SIZE       128 

typedef struct 
{
    int32 nNum;
    uint8  value[MAX_KEY_SIZE];
}OOP_METKEY_T;

//
// 601F-采集规则
//

#define OOP_MAX_ACQRULE (4)         /**< 最大采集规则数量   */

/** @brief DL/T645-2007采集规则定义   */
typedef struct tag_AcqCmd_2007
{
    uint8           nMainLen;
    uint32          mainId[OOP_MAX_ACQRULE];
    uint8           nBackupLen;
    uint32          backupId[OOP_MAX_ACQRULE];
}ACQCMD_2007;

/** @brief DL/T645-1997采集规则定义   */
typedef struct tag_AcqCmd_1997
{
    uint32          nMainLen;
    uint16          mainId[OOP_MAX_ACQRULE];
    uint32          nBackupLen;
    uint16          backupId[OOP_MAX_ACQRULE];
}ACQCMD_1997;

/** @brief 采集规则 */
typedef struct tag_AcqCmd
{
    ACQCMD_2007         acqCmd07;       /**< 07采集数据标识   */
    ACQCMD_1997         acqCmd97;       /**< 97采集数据标识   */
    OOP_OCTETVAR256_T   acqCmdTrs;      /**< 透明命令       */
}ACQCMD;

/** @brief 698.45的采集规则    */
typedef struct tag_OOP_ACQRULE
{
    OOP_CSD_T       csd;            /**< 数据属性描述符      */
    ACQCMD          acqCmd;         /**< 采集规则           */
}OOP_ACQRULE_T;

/** @brief 6033-采集状态集 */
typedef struct tag_OOP_ACQ_RLUESTATE
{
    OOP_TSA_T           add;                /**< 通信地址               */
    uint8               relayLevel;         /**< 中继级别               */
    OOP_TSA_T           relayAdd;           /**< 中继地址               */
    OOP_OAD_U           port;               /**< 端口                 */
    OOP_DATETIME_S_T    time;               /**< 最后一次采集成功时间 */
    uint8               failTimes;          /**< 采集失败次数         */
    uint8               phase;              /**< 相位                 */
    uint8               phaseSeq;           /**< 相序异常               */
}OOP_ACQ_RLUESTATE_T;

/** @brief 6035-采集任务监控  */
typedef struct tag_OOP_ACQ_MONITOR
{
     uint8              id;
     uint8              taskState;  /**< 任务状态(enum)     */
     OOP_DATETIME_S_T   startTime;  /**< 任务执行开始时间   */
     OOP_DATETIME_S_T   endTime;    /**< 任务执行结束时间   */
     uint16             sum;        /**< 采集总数量            */
     uint16             success;    /**< 采集成功数量     */
     uint16             send;       /**< 已发送报文条数      */
     uint16             receive;    /**< 已接收报文条数      */
     BOOLEAN            bValid;     /**< 有效标记           */
}OOP_ACQ_MONITOR_T;

#define OOP_MAX_LNLS    4

/** @brief 6051(属性2)-线损计算数据*/
typedef struct tag_OOP_LNLS_UNIT
{
    uint32  eneSup;         /**< 供电量*/
    uint32  eneSal;         /**< 售电量*/
    uint32  eneDeliv;       /**< 倒送电量*/
    uint32  eneDistrpwrnet; /**< 分布式电源上网电量*/
    int16   nRatio;         /**< 线损率*/
    uint8   abnorFlag;      /**< 异常标记(enum) 0:正常 1:总表电量异常 2:户表电量异常*/
}OOP_LNLS_UNIT_T;

typedef struct tag_OOP_LNLS_STATISVAL
{
    uint32 nNum;
    uint32 rsv;
    OOP_LNLS_UNIT_T nVal[OOP_MAX_LNLS];
}OOP_LNLS_STATISVAL_T;

/** @brief 6051(属性4)-线损统计周期*/
typedef struct tag_OOP_LNLSPER
{
    OOP_TI_T statime;   /**< 统计周期*/
}OOP_LNLSPER_T;

//
// 6062-采集档案名单单元
//

typedef struct tag_OOP_METER_LIST
{
    OOP_TSA_T           add;        /**< 通信地址           */
    uint8               nUserType;  /**< 用户类型           */
    uint8               nProtocol;  /**< 规约类型           */
}OOP_METER_LIST_T;

//
// 6E03-端口超时响应时间
//
#define OOP_MAX_FPORT			(31)

typedef struct tag_OOP_TIMEOUT
{
    OOP_OAD_U	port;
    OOP_TI_T	ti;
}OOP_TIMEOUT_T;

typedef struct tag_OOP_PORTIMEOUT
{
    uint32				nNum;
    uint32				rsv;
    OOP_TIMEOUT_T		state[OOP_MAX_FPORT];
}OOP_PORTIMEOUT_T;

//
// 6E06-群组档案单元
//
#define OOP_MAX_MGNUM			(255)
#define OOP_MAX_MGNUM_M128		(32)

typedef struct tag_OOP_MG_COUNT
{
    OOP_TSA_T	tsa;
    uint16		count;
}OOP_MG_COUNT;

typedef struct tag_OOP_METER_GROUP
{
    OOP_TSA_T	tsa;	// 群组地址
    OOP_MSP_T	ms;		// 通信地址集合
}OOP_METER_GROUP;

/** @brief 按群组地址和具体地址删除档案	*/
typedef struct tag_OOPM_MGROUP_128
{
    uint32 				nNum;
    uint32 				rsv;	
    OOP_METER_GROUP  	param[OOP_MAX_MGNUM_M128];
}OOPM_MGROUP_128_T;

//
// 6E12-全局透明任务单元
//

#define OOP_MAX_FTASK		(255)
#define OOP_PRIO_PAUSE		(254)

typedef enum tag_OOP_TOUCH_TYPE
{
    TT_POWERON		= 0,
    TT_YX_CHANGED	= 1,
    TT_3762_EVT		= 2,
    TT_MATCHED		= 3,
    TT_EVENT_REPORT	= 4
}OOP_TOUCH_TYPE;

typedef enum tag_OOP_OBJECT_TYPE
{
    OT_BODY			= 0,
    OT_HPLC_CHIP	= 1,
    OT_HPLC_ROUTER	= 2,
    OT_METER		= 3,
    OT_AC			= 4,
    OT_GPRS			= 8
} OOP_OBJECT_TYPE;

typedef enum tag_OOP_TOUCH_OBJECT
{
    TO_FULL		= 0,
    TO_ADDRESS	= 1
}OOP_TOUCH_OBJECT;

typedef struct tag_OOP_CHAR
{
    uint16				start;		// 截取开始
    uint16				end;		// 截取结束
    OOP_OCTETVAR256_T	ch;			// 特征字串
}OOP_CHAR_T;

typedef union tag_OOP_TOUCH
{
    OOP_BITSTR8_T	yx;				// 遥信变位（enum=1）
    OOP_CHAR_T		match;			// 报文特征匹配（enum=4）
}OOP_TOUCH_U;

typedef struct tag_OOP_RAISED
{
    uint8			nType;			// 触发类型
    OOP_TOUCH_U		data;			// 触发内容
    uint8			tchObject;		// 触发后执行的具体任务对象
    uint16			exTime;			// 任务允许执行时长（分钟）
}OOP_RAISED_T;

typedef union tag_OOP_ANNEX
{
    uint8			nBrdDelay;		// 广播（每帧之间的间隔）		= 1
    uint16			nLogRecord;		// 日志（本轮次允许记录条数）		= 4
    OOP_RAISED_T	tRaised;		// 触发						= 5
}OOP_ANNEX_U;

typedef enum tag_OOP_FTASK_TYPE
{
    FT_PERIOD		= 0,	// 周期任务
    FT_BROADCAST	= 1,	// 广播任务
    FT_SEARCH		= 2,	// 搜表任务
    FT_IDLE			= 3,	// 空闲任务
    FT_LOG			= 4,	// 日志任务
    FT_TOUCH		= 5,	// 触发任务
}OOP_FTASK_TYPE;

typedef enum tag_OOP_PACKAGE_TYPE
{
    FP_FULL_TRANS	= 0,	// 全透明转发
    FP_TRANS_1376_2 = 1,	// 国网1376.2透明转发
    FP_TRANS_FJ		= 2,	// 福建1376.2透明转发
    FP_NOTICE		= 3,	// 福建预告抄表（广播）
    FP_PLREAD		= 4,	// 国网并发抄表
    FP_SEARCH_FJ	= 8,	// 福建HPLC搜表
    FP_SEARCH_TRANS	= 9		// 透明通道搜表
} OOP_PACKAGE_TYPE;

typedef struct tag_OOP_FTASK
{
    uint16              nTaskID;                    // 方案号
    OOP_OAD_U           nPort;                      // 端口号
    uint8				nType;						// 任务类型
    OOP_ANNEX_U			data;						// 任务附加参数
    uint8               nPrio;                      // 优先级
    bool                bEngross;                   // 独占模式
    OOP_DATETIME_S_T    start;                      // 任务开始时间
    OOP_DATETIME_S_T    end;                        // 任务结束时间
    OOP_TI_T            period;                     // 任务执行周期
    OOP_TI_T            tiRetry;                    // 失败重试周期
    uint8               nRetry;                     // 失败重试次数
    uint8               nTurnNum;                   // 重试轮次
    uint8				nPackage;					// 封拆包方式
    bool                bOrder;                     // 是否多帧关联任务
    uint8               objType;                    // 通信对象类型
    uint16              nSaveDepth;                 // 存储深度
    uint8              	nRptID;                     // 上报任务
    //
    uint8				nConvert;					// 数据加工方式
    //
    uint16				preScript;					// 任务开始前脚本
    uint16				aftScript;					// 任务结束后脚本
}OOP_FTASK_T;

/** Update array 参数	*/
typedef struct tag_OOPM_FTASK_130
{
    uint16				nTaskID;					// 方案号
    uint8				nPrio;						// 优先级
}OOPM_FTASK_130_T;

// 6E12-属性3
typedef struct tag_OOP_FTASKSTAT
{
    uint16              nTaskID;                    // 方案号
    OOP_OAD_U           nPort;                      // 端口号
    uint8               nPrio;                      // 优先级
    bool                bEngross;                   // 独占模式
    uint16				nSum;						// 已配置具体任务总数量
    uint16				minID;						// 最小具体任务序号
    uint16				maxID;						// 最大具体任务序号
}OOP_FTASKSTAT_T;

//
// 6E14-全局具体透明任务
//

#define OOP_MAX_DATAPROC	(6)

typedef struct tag_OOP_FRMPROC
{
    OOP_OCTETVAR256_T	ch;			// 特征字符
    uint16				start;		// 比对开始
    uint16				end;		// 比对结束
    uint16				task;		// 触发任务
}OOP_FRMPROC_T;

typedef struct tag_OOP_DPITEM
{
    OOP_OCTETVAR256_T	period1;	// 验证区间1
    OOP_OCTETVAR256_T	period2;	// 验证区间2
    uint16				task;		// 触发任务
}OOP_DPITEM_T;

typedef struct tag_OOP_DATAPROC
{
    OOP_OCTETVAR256_T	ch;							// 特征字符
    uint16				start;						// 比对开始
    uint16				end;						// 比对结束
    uint8				format;						// 数据格式类型
    uint8				num;						// 数据验证区间个数
    OOP_DPITEM_T		period[OOP_MAX_DATAPROC];	// 数据比对区间
}OOP_DATAPROC_T;

typedef union tag_OOP_PROC
{
    OOP_FRMPROC_T	frm;			// 报文比对
    OOP_DATAPROC_T	data;			// 数据比对
}OOP_PROC_U;

typedef struct tag_OOP_PLAN
{
    uint16				nTaskID;			// 方案号
    uint16				nIndex;				// 任务序号
    OOP_MSP_T			ms;					// 电表集合
    OOP_COMDCB_T		dcb;				// 通信控制块
    uint8				nRequest;			// 请求报文控制字
    OOP_OCTETVAR256_T	frame;				// 请求报文内容
    uint8				nResponse;			// 响应报文控制字
    uint16				nMaxAck;			// 预计最大响应报文
    uint8				nMinAck;			// 预计最小响应报文
    uint8				ackProc;			// 响应帧处理
    OOP_PROC_U			proc;				// 处理参数
}OOP_PLAN_T;

/**按透明任务方案号和地址删除一组透明具体任务	*/
#define OOP_MAX_DELFTASK_NUM			(32)

typedef struct tag_OOP_DElPLANBYMS
{
    uint16				nTaskID;			// 方案号
    OOP_MSP_T			ms;					// 电表集合
}OOP_DELPLANBYMS_T;

typedef struct tag_OOPM_PLAN_128
{
    uint32 				nNum;
    uint32 				rsv;	
    OOP_DELPLANBYMS_T	param[OOP_MAX_DELFTASK_NUM];
}OOPM_PLAN_128_T;

//
// 6E16-一个透明方案结果
//

/** @brief 福建错误代码定义	*/
typedef enum OOP_ERR_CODE
{
    CODE_SUCCESS						= 0,			/**< 成功					*/
    CODE_RSV							= 1,			/**< 保留					*/
    CODE_ADDR_REPEAT					= 2,			/**< 表地址重复				*/
    CODE_FRAME_OVER						= 3,			/**< 报文超长				*/
    CODE_CHECK_ERR						= 4,			/**< 报文校验错误				*/
    CODE_FLASH_ERR						= 5,			/**< 存储空间不足				*/
    CODE_ADDR_ERR						= 6,			/**< 地址校验失败				*/
    CODE_SIGNATURE_FAILURE				= 7,			/**< 数据签名校验错误			*/
    CODE_ENCRYP_FAILURE					= 8,			/**< 密文校验错误				*/
    CODE_MAC_FAILURE					= 9,			/**< 对称MAC验证失败			*/
    CODE_POWER_PROTECTION				= 10,			/**< 处于保电状态				*/
    CODE_REPONSE_TIMEOUT				= 11,			/**< 响应超时				*/
    CODE_TASK_NOT_RUN					= 12,			/**< 任务未执行				*/
    CODE_TASK_INVALID					= 13,			/**< 任务未配置				*/
    CODE_METER_NOREPONSE				= 14,			/**< 运用层无响应				*/
    CODE_CHANNEL_BUSY					= 15,			/**< 信道忙				*/
    CODE_MODULE_ERR						= 16,			/**< 通信模块异常				*/
}OOP_ERR_CODE_E;

typedef struct tag_OOP_ACK_FRAME
{
    uint8				errCode;			// 错误代码
    OOP_OCTETVAR2K_T	frame;				// 数据
}OOP_ACK_FRAME_T;

typedef struct tag_OOP_FTRANS_RESULT
{
    uint16						nTaskID;			// 方案号
    uint16						nIndex;				// 任务序号
    OOP_TSA_T					addr;				// 电表集合
    uint8						nType;				// 通信对象类型
    OOP_DATETIME_S_T			tmSave;				// 采集存储时标
    OOP_DATETIME_S_T			tmBegin;			// 终端抄表时间
    uint16						nSaveID;			// 存储序号
    uint8						nTurnNum;			// 重试轮次
    uint8						nResult;			// 任务结果
    uint8						nResponse;			// 响应报文格式字
    OOP_ACK_FRAME_T				data;				// 响应报文内容
}OOP_FTRANS_RESULT_T;

//
// 6E18
//
#define OOP_MAX_REPORTNUM		(32)
#define OOP_MAX_FREPORT			(255)

typedef enum tag_OOP_VERIFY
{
    FV_REPORT	= 0,		// 直接上报
    FV_SUCCESS	= 1,		// 验证通过上报
    FV_FAILED	= 2			// 验证失败上报
} OOP_VERIFY;

typedef struct tag_OOP_REPORT_PLAN
{
    uint8				rpID;		// 上报方案ID
    OOP_OADS_T			rpChs;		// 上报通道
    uint8				prio;		// 优先级
    OOP_TI_T			rpWait;		// 响应超时时间
    uint8				rpMax;		// 最大上报次数
    uint8				flagRet;	// 结果验证标识
    uint8				rpTimes;	// 上报时刻
    REPORT_CONTEXT		data;		// 上报内容
} OOP_REPORT_PLAN;

/** @brief 补报一次指定的未成功上报	*/
typedef struct tag_OOPM_FREPORT_132
{
    uint32 	nNum;
    uint32 	rsv;	
    uint16  nTaskID[OOP_MAX_REPORTNUM];
}OOPM_FREPORT_132_T;

//
// 6E21 一个透明任务方案采集状态
//

typedef struct tag_OOP_PLAN_ACQSTAT
{
    uint16				nTaskID;	// 透明任务方案号
    OOP_DATETIME_S_T	tmSave;		// 采集存储时标 
    uint16				nSave;		// 存储次数 
    OOP_DATETIME_S_T	tmStart;	// 任务执行开始时间
    OOP_DATETIME_S_T	tmEnd;		// 任务最后执行时间
    uint16				nSum;		// 配置总数量  
    uint16				nSucess;	// 采集成功数量 
    uint16				nConvert;	// 加工成功数量 
    uint16				nSend;		// 已发送报文条数 
    uint16				nReceive;	// 已接收确认报文条数 
} OOP_PLAN_ACQSTAT;

typedef struct tag_OOP_FTRANS_RESULT_EXT
{
    BOOL   				 bRetry;					// 当前是否属于重试
    OOP_PLAN_ACQSTAT	 acq;						// 透明任务采集状态
    OOP_FTRANS_RESULT_T  data;						// 透明结果
} OOP_FTRANS_RESULT_EXT_T;

//
// 6E22 通道采集状态
//
typedef struct tag_OOP_PORT_ACQSTAT
{
    OOP_OAD_U			nPort;		// 端口
    uint8				nType;		// 当前执行的任务类别
    uint16				nTaskID;	// 当前执行的透明任务方案号
    uint8				nPrio;		// 执行优先级 
} OOP_PORT_ACQSTAT;

//
// 6E24 一个采集状态(地址)
//
typedef struct tag_OOP_ADDR_ACQSTAT
{
    OOP_TSA_T			addr;		// 通信地址
    uint8				nType;		// 通信对象类型  
    OOP_OAD_U			nPort;		// 端口
    OOP_DATETIME_S_T	tmSucess;	// 最后一次采集成功时间 
    OOP_DATETIME_S_T	tmFail;		// 最后一次采集失败时间 
} OOP_ADDR_ACQSTAT;

//
// 日志采集状态
//
typedef struct tag_OOP_LOG_ACQSTAT
{
    uint16		nTaskID;
    uint16		maxIdx;
    uint32		tmBegin;
    uint32		tmGet;
    uint32		tmSave;
} OOP_LOG_ACQSTAT;

//
// 6E25 存储空间使用情况
//
typedef struct tag_OOP_STOREINFO
{
    uint32				datastore;		// 数据区大小
    uint32				dataUsage;		// 已经使用大小  
    uint32				paramstore;		// 参数区大小
    uint32				paramUsage;		// 已经使用大小 
} OOP_STOREINFO_T;

//
// 6E31 透明事件记录集
//

// 属性3-事件记录当前状态

typedef struct tag_OOP_FEVENTSTAT
{
    OOP_DATETIME_S_T	dtSave;			// 最近接收事件存储时标
    uint16				nSaveIndex;		// 最近接收事件存储序号
    uint16				nRecvCnt;		// 已接收确认报文条数 
} OOP_FEVENTSTAT_T;

//
// 6E32 透明事件记录单元
//

typedef struct tag_OOP_FTRANSEVTUNIT
{
    uint16						nCnt;			// 当前事件计数器
    uint16						nSaveIndex;		// 本事件存储序号
    OOP_DATETIME_S_T			dtRpt;			// 事件上报时标
    OOP_TSA_T					tsa;			// 通信地址
    uint8						nType;			// 通信对象类型
    OOP_OAD_U					port;			// 端口
    uint8						nFormat;		// 报文格式字
    OOP_OCTETVAR2K_T			data;			// 事件内容
} OOP_FTRANSEVTUNIT_T;

//
// 6E33 主动上报事件上报策略
//

typedef struct tag_OOP_FEVTRPTPLAN
{
    OOP_OADS_T		port;		// 上报通道 
    uint8			prio;		// 上报优先级
    OOP_TI_T		ti;			// 上报响应超时时间	
    uint8			retry;		// 最大上报次数
} OOP_FEVTRPTPLAN_T;

//
// 6EF1-完整节点拓扑信息
//

#define OOP_MAX_SDTOPS	(512)
#define OOP_MAX_SDMTOPS	(32)
#define OOP_MAX_SDLTOPS	(32)

typedef struct tag_OOP_TOPITEM
{
    OOP_TSA_T	nodeAdd;		// 节点地址
    uint8		nodeType;		// 节点类型
    OOP_TSA_T	partAdd;		// 父节点地址
}OOP_TOPITEM_T;

//
// 6EF2-电表拓扑信息
//
typedef struct tag_OOP_MTOPITEM
{
    OOP_TSA_T	add;		// 节点地址
    uint8		type;		// 节点类型
}OOP_MTOPITEM_T;

typedef struct tag_OOP_MTOPITEMS
{
    uint32 nNum;
    uint32 rsv;
    OOP_MTOPITEM_T item[OOP_MAX_SDMTOPS];
}OOP_MTOPS_T;

//
// 7000-
//

typedef struct tag_OOPM_FILE_WRITE
{
    OOP_VISIBLEVAR_T	file;
    uint32				offset;
    OOP_OCTETVAR2K_T	data;
} OOPM_FILE_WRITE_T;

typedef struct tag_OOPM_FILE_READ
{
    OOP_VISIBLEVAR_T	file;
    uint32				offset;
} OOPM_FILE_READ_T;

typedef struct tag_OOP_FILE_ATTRIBUTE
{
    OOP_VISIBLEVAR_T	file;
    uint32				size;
    OOP_DATETIME_S_T	tmCreate;
    OOP_DATETIME_S_T	tmModify;
    OOP_OCTETVAR64_T	md5;
} OOP_FILE_ATTRIBUTE_T;

//
// 7200- 充电计划单
//

/** @brief 4708-充电计划单单元*/
typedef struct tag_OOP_ORDCHARGE_PLAN
{
    OOP_OCTETVAR256_T        reqForm;                            /**>充电申请单*/
    OOP_VISIBLEVAR128_T      userId;                             /**>用户ID*/
    uint8                    policyEle;                          /**>决策单元*/
    uint8                    policyType;                         /**>决策类型*/
    OOP_DATETIME_S_T         policyTime;                         /**>决策时间*/
    OOP_VISIBLEVAR32_T       routerAssetNum;                     /**>路由器资产编号*/ 
    uint32                   energyNeed;                         /**>需求电量*/
    int32                    nPower;                             /**>功率*/
    uint8                    nMode;                              /**>充电模式*/
    OOP_PERIOD_LIST_T        periodList;                         /**>充电时段*/
}OOP_ORDCHARGE_PLAN_T;

// 添加充电单返回结果
typedef struct tag_OOP_ADDCHGPLAN_RST
{
    OOP_OCTETVAR256_T        reqForm;                            /**>申请单*/
    OOP_VISIBLEVAR_T         routerAssetNum;                     /**>路由器资产编号*/ 
}OOP_ADDCHGPLAN_RST_T;

//
// 7300-容器集合
//

#define OOP_MAX_CONTAINER   (16)

/** @brief F401-容器单元    */
typedef struct tag_OOP_CONTAINER_UNIT
{
    uint8               nId;            /**< 容器ID */
    OOP_VISIBLEVAR128_T name;           /**< 容器名称 */
    OOP_VISIBLEVAR_T    state;          /**< 容器状态 */
    uint8               nCpu;           /**< CPU核数 */
    OOP_VISIBLEVAR_T    ramLimit;       /**< 内存限值 */
    OOP_VISIBLEVAR_T    memoryLimit;    /**< 存储限值 */
    OOP_VISIBLEVAR128_T hostFolder;     /**< 挂载的宿主机文件夹 */
    OOP_VISIBLEVAR128_T hwItf;          /**< 映射的硬件接口 */
    OOP_VISIBLEVAR128_T appDownLoad;    /**< 应用下载文件及路径 */
    uint16              nCpuUsage;      /**< 容器CPU使用率 */
    uint16              nRamUsage;      /**< 容器内存使用率 */   
    uint16              nMemUsage;      /**< 容器存储使用率 */
    OOP_OCTETVAR16_T    ip;             /**< 容器IP地址 */
    OOP_VISIBLEVAR128_T image;          /**< 容器镜像名称 */
    OOP_DATETIME_S_T    dtCreate;       /**< 容器创建时间 */
    OOP_DATETIME_S_T    dtStart;        /**< 容器启动时间 */
    uint32              nRunTime;       /**< 容器运行时间 */
}OOP_CONTAINER_UNIT_T;

typedef struct tag_OOP_CONTAINER_LIST
{
    uint32 nNum;
    uint32 rsv;
    OOP_CONTAINER_UNIT_T container[OOP_MAX_CONTAINER];
}OOP_CONTAINER_LIST_T;

//
// 7301-应用集合
//

#define OOP_MAX_APP             (64)
#define OOP_MAX_APP_SERVER      (16)
#define OOP_MAX_APP_LICENSE	(32)

typedef struct tag_OOP_APP_SERVER_INFO
{
    uint8               nIndex;         /**< 服务索引 */
    OOP_VISIBLEVAR128_T name;           /**< 服务名称 */
    BOOLEAN             bEnable;        /**< 服务是否使能 */
    uint8               nState;         /**< 服务状态 */
    uint16              nCpuUsageLimit; /**< CPU使用率限值 */
    uint16              nCpuUsage;      /**< CPU实际使用率 */  
    uint16              nRamUsage;      /**< 内存实际使用率 */   
    OOP_DATETIME_S_T    dtStart;        /**< 启动时间 */
}OOP_APP_SERVER_INFO_T;

/** @brief F402-应用单元    */
typedef struct tag_OOP_APP_UNIT
{
    uint8                 nId;                              /**< 应用序号 */
    OOP_VISIBLEVAR128_T   nameApp;                          /**< 应用名称 */
    OOP_VISIBLEVAR128_T   nameContainer;                    /**< 容器名称 */
    OOP_VISIBLEVAR_T      version;                          /**< 应用版本 */
    OOP_VISIBLEVAR_T      hash;                             /**< 应用哈希值 */
    uint32                nServerNum;                       /**< 应用服务个数 */
    OOP_APP_SERVER_INFO_T serverInfo[OOP_MAX_APP_SERVER];   /**< 应用服务列表 */
}OOP_APP_UNIT_T;

typedef struct tag_OOP_APP_LIST
{
    uint32 nNum;
    uint32 rsv;
    OOP_APP_UNIT_T app[OOP_MAX_APP];
}OOP_APP_LIST_T;

typedef struct tag_OOPM_APP_INSTALL_T
{
    OOP_VISIBLEVAR_T	containerName;
    OOP_VISIBLEVAR_T	appName;
    OOP_VISIBLEVAR_T	appPath;
}OOPM_APP_INSTALL_T;

typedef struct tag_OOPM_APP_MAN_T
{
    OOP_VISIBLEVAR_T	containerName;
    OOP_VISIBLEVAR_T	appName;
}OOPM_APP_MAN_T;

typedef struct tag_OOPM_APP_CONFIG_T
{
    OOP_VISIBLEVAR_T	containerName;
    OOP_VISIBLEVAR_T	appName;
    uint16				wrCPU;
    uint16				wrRAM;
}OOPM_APP_CONFIG_T;


/**
********************************************************
* A.10 控制类对象
********************************************************
*/

#define OOP_MAX_TURN        (8)             /**< 最大控制轮次     */
#define OOP_MAX_CHINESEINFO (16)            /**< 最大中文信息数量   */

//
// 8000-遥控
//

/** @brief 遥控参数设定   */
typedef struct tag_OOP_TELECON_PARAM
{
    int32       limit;                      /**< 继电器拉闸电流门限值 */
    uint16      delay;                      /**< 超电流门限保护延时时间  */
}OOP_TELECON_PARAM_T;

/** @brief 遥控命令状态   */
typedef struct tag_OOP_TELECON_STATE
{
    uint32      nNum;
    uint32      rsv;
    uint8       comState[OOP_MAX_TURN];         /**< 命令状态   */
}OOP_TELECON_STATE_T;

#ifdef PRODUCT_ZCU_1
#define OOP_MAX_TRIP    (16)                    /**< 最大轮次数    */
#else
#define OOP_MAX_TRIP    (8)                     /**< 最大轮次数    */
#endif

/** @brief 遥控跳闸单元   */
typedef struct tag_TELECON_TRIP
{
    OOP_OAD_U   nRelayIdx;                      /**< 继电器序号                                        */
    uint8       nWarnDelay;                     /**< 告警延时(单位：分钟，换算：0)             */
    uint16      nTripTime;                      /**< 限电时间(单位：分钟，换算：0)             */
    uint8       bAutoClose;                     /**< 自动合闸(TRUE：自动合闸；FALSE：非自动合闸)    */
}TELECON_TRIP;

/** @brief 遥控-方法129(跳闸) */
typedef struct tag_OOP_TELECON_TRIP
{
    uint32          nNum;
    uint32          rsv;
    TELECON_TRIP    trip[OOP_MAX_TRIP];
}OOP_TELECON_TRIP_T;

/** @brief 遥控合闸单元   */
typedef struct tag_TELECON_CLOSE
{
    OOP_OAD_U       nRelayIdx;
    uint8           nState;
}TELECON_CLOSE;

/** @brief 遥控-方法130(合闸) */
typedef struct tag_OOP_TELECON_CLOSE
{
    uint32          nNum;
    uint32          rsv;
    TELECON_CLOSE   close[OOP_MAX_TRIP];
}OOP_TELECON_CLOSE_T;

//
// 8001-保电
//
#define OOP_MAX_PRTTIME (4)     /**< 最大保电时段     */

/** @brief 保电时段 */
typedef struct tag_PRTTIME
{
    uint8   start;
    uint8   end;
}PRTTIME;

/** @brief 保电时段集合   */
typedef struct tag_OOP_PRTTIME
{
    uint32  nNum;
    uint32  rsv;
    PRTTIME item[OOP_MAX_PRTTIME];
}OOP_PRTTIME_T;

/** @brief 8002-催费告警    */
typedef struct tag_OOP_CALLFEE
{
    uint8   period[3];
    uint32  nLen;
    char    info[200];
}OOP_CALLFEE_T;

/** 
* @brief    中文信息            \n
*           8003-一般中文信息 \n
*           8004-重要中文信息
*/
typedef struct tag_CHINESEINFO
{
    uint8               index;  /**<0-invalid, 1-valid      */
    OOP_DATETIME_S_T    time;
    uint8               bRead;  /**<0-looked, 1-not looked  */
    OOP_OCTETVAR256_T   info;
}CHINESEINFO;

/** @brief 中文信息集合   */
typedef struct tag_OOP_CHINESEINFOS
{
    uint32      nNum;
    uint32      rsv;
    CHINESEINFO item[OOP_MAX_CHINESEINFO];
}OOP_CHINESEINFOS_T;

/** @brief 添加中文信息   */
typedef struct tag_OOPM_ADD_CHINESEINFO
{
    uint8               index;  /**<0-invalid, 1-valid  */
    OOP_DATETIME_S_T    time;
    OOP_OCTETVAR256_T   info;
}OOPM_ADD_CHINESEINFO_T;

/** @brief 8101-终端功控时段  */
typedef struct tag_OOP_POWCON_TIME
{
    uint8        period[12]; 
}OOP_POWCON_TIME_T;

/** @brief 8102-功控告警时间  */
typedef struct tag_OOP_POWCON_WARNING
{
    uint32      nNum;
    uint32      rsv;
    uint8       time[OOP_MAX_TURN];
}OOP_POWCON_WARNING_T;

/** @brief 时段控方案  */
typedef struct tag_PERIODPLAN
{
    OOP_BITSTR8_T       flag;
    uint8               planID;
}PERIODPLAN;

/** @brief 8103-时段功控    */
typedef struct tag_OOP_POWCON_PERIOD
{
    OI          object;
    PERIODPLAN  plan;
}OOP_POWCON_PERIOD_T;

/** @brief 下浮控控制方案    */
typedef struct tag_LCONSCHEME
{
    uint8       slipTime;                   /**< 当前功率下浮空定值滑差时间        */
    int8        floatCoefficient;           /**< 当前功率下浮控定值浮动系数        */
    uint8       delayTime;                  /**< 控后总加有功功率冻结延时时间 */
    uint8       conTime;                    /**< 当前功率下浮控的控制时间       */
    uint8       waringTime[OOP_MAX_TURN];   /**< 当前功率下浮控告警时间          */
}LCONSCHEME;

/** @brief 8106-当前功率下浮控   */
typedef struct tag_OOP_POWCON_LOW
{
    OI           object;                /**< 总加组对象    */
    LCONSCHEME   scheme;                /**< 控制方案   */
}OOP_POWCON_LOW_T;

/** @brief 时段控定值参数    */
typedef struct OOP_POWERCTRLPARAM
{
    uint8    valid;                     /**< 时段号              */
    int64    defValue[OOP_MAX_TURN];    /**< 时段功控定值     */
}POWERCTRLPARAM;

#define OOP_MAX_PCPLAN  (3)             /**< 最大时段控方案数   */

/** @brief 8109-时段功控配置单元    */
typedef struct tag_OOP_CFGUNIT_8109
{
    uint16              object;                 /**< 总加组对象                */
    uint8               plan;                   /**< 方案标识               */
    POWERCTRLPARAM      value[3];               /**< 三套方案定值         */
    int8                floatCoe;               /**< 时段功控定值浮动系数 */
}OOP_CFGUNIT_8109_T;

/** @brief 时段控    */
typedef struct tag_OOP_8103_CONFIG
{
    uint32              nNum;
    uint32              rsv;
    OOP_CFGUNIT_8109_T  item[OOP_MAX_TG];
}OOP_8103_CONFIG_T;

/** @brief 810A-厂休控配置单元   */
typedef struct tag_OOP_CFGUNIT_810A
{
    OI                  object;                 /**< 总加组对象        */
    int64               defValue;               /**< 厂休控定值        */
    OOP_DATETIME_S_T    startTime;              /**< 限电起始时间 */
    uint16              duration;               /**< 限电延续时间 */
    OOP_BITSTR8_T       powLimit;               /**< 每周限电日        */
}OOP_CFGUNIT_810A_T;

/** @brief 厂休控    */
typedef struct tag_OOP_8104_CONFIG
{
    uint32              nNum;
    uint32              rsv;
    OOP_CFGUNIT_810A_T  item[OOP_MAX_TG];
}OOP_8104_CONFIG_T;

/** @brief 810B-营业报停控配置单元 */
typedef struct tag_OOP_CFGUNIT_810B
{
    uint16                  object;                 /**< 总加组对象        */
    OOP_DATETIME_S_T        startTime;              /**< 报停起始时间 */
    OOP_DATETIME_S_T        endTime;                /**< 报停结束时间 */
    int64                   defValue;               /**< 报停控功率定值  */
}OOP_CFGUNIT_810B_T;

/** @brief 营业报停控  */
typedef struct tag_OOP_8105_CONFIG
{
    uint32              nNum;
    uint32              rsv;
    OOP_CFGUNIT_810B_T  item[OOP_MAX_TG];
}OOP_8105_CONFIG_T;

/** @brief 810C-购电控配置单元   */
typedef struct tag_OOP_CFGUNIT_810C
{
    uint16      object;                 /**< 总加组对象        */
    uint32      number;                 /**< 购电单号       */
    uint8       flag;                   /**< 追加/刷新标识    */
    uint8       type;                   /**< 购电类型       */
    int64       purElectricity;         /**< 购电量(费)值  */
    int64       alarmLimit;             /**< 报警门限值        */
    int64       tripLimit;              /**< 跳闸门限值        */
    uint8       mode;                   /**< 购电控模式        */
}OOP_CFGUNIT_810C_T;

/** @brief 购电控    */
typedef struct tag_OOP_8107_CONFIG
{
    uint32              nNum;
    uint32              rsv;
    OOP_CFGUNIT_810C_T  item[OOP_MAX_TG];
}OOP_8107_CONFIG_T;

/** @brief 810D-月电控配置单元   */
typedef struct tag_OOP_CFGUNIT_810D
{
    uint16       object;                 /**< 总加组对象           */
    int64        defValue;               /**< 月电量控定值            */
    uint8        limitCoe;               /**< 报警门限值系数     */
    int8         floatCoe;               /**< 月电控定值浮动系数   */
}OOP_CFGUNIT_810D_T;

/** @brief 月电控    */
typedef struct tag_OOP_8108_CONFIG
{
    uint32              nNum;
    uint32              rsv;
    OOP_CFGUNIT_810D_T  item[OOP_MAX_TG];
}OOP_8108_CONFIG_T;

/** @brief 8F06-临时限电控 */
typedef struct tag_OOP_POWCON_TMP
{
    uint32				nNum;
	uint32				rsv;
	OOP_POWCON_LOW_T	item[OOP_MAX_TG]; //使用下浮控的参数
}OOP_POWCON_TMP_T;

/** @brief 8F06-临时限电控跳闸次数 */
typedef struct tag_OOP_POWCON_TMP_NUM
{
    OOP_DATE_T          curtime;     //当前时间
    uint8               turnnum;     //跳闸次数
}OOP_POWCON_TMP_NUM_T;

/**
********************************************************
* A.11 文件传输类对象
********************************************************
*/

/** @brief F000-文件分帧传输管理    */
typedef struct tag_OOPM_DOWNLOADFILE
{
    OOP_VISIBLEVAR128_T file;       /**< 文件路径           */
    uint32              nOffset;    /**< 偏移位置           */
    uint32              nLen;       /**< 读取长度           */
}OOPM_DOWNLOADFILE_T;

/** @brief 文件传输协议类型 */
typedef enum tag_OOP_FTP
{
    telnet  = 0,
    ftp     = 1,
    sftp    = 2,
    http    = 3,
    https   = 4
}OOP_FTP_E;

/** @brief FTP服务器信息   */
typedef struct tag_OOP_FTPSERVER
{
    OOP_IP_T            ip;
    uint16              port;
    OOP_VISIBLEVAR_T    user;
    OOP_VISIBLEVAR_T    pwd;
}OOP_FTPSERVER_T;

/** @brief 文件类别 */
typedef enum tag_OOP_FILETYPE
{
    FT_TERM     = 0,    /**< 终端文件           */
    FT_LOCAL    = 1,    /**< 本地通信模块文件   */
    FT_REMOTE   = 2,    /**< 远程通信模块文件   */
    FT_COLL     = 3,    /**< 采集器文件            */
    FT_NODE     = 4,    /**< 从节点通信模块文件    */
    FT_OTHER    = 255,  /**< 其它文件           */
}OOP_FILETYPE_E;

/** @brief 文件信息 */
typedef struct tag_OOP_FILE
{
    OOP_VISIBLEVAR_T    srceFile;
    OOP_VISIBLEVAR_T    destFile;
    uint32              nSize;
    OOP_BITSTR3_T       property;
    OOP_VISIBLEVAR_T    version;
    uint8               filetype;
}OOP_FILE_T;

/** @brief 启动FTP传输  */
typedef struct tag_OOPM_FTP
{
    OOP_FILE_T  file;
    uint8       proto;
}OOPM_FTP_T;

/** @brief 文件校验信息   */
typedef struct tag_OOP_FILEVC
{
    uint8               type;
    OOP_OCTETVAR64_T    content;   //24规范中SM3编码32位 原16位存储越界
}OOP_FILEVC_T;

/** @brief 启动块传输  */
typedef struct tag_OOPM_BLOCKTRANS
{
    OOP_FILE_T      file;
    uint32          nBlockSize;
    OOP_FILEVC_T    crc;
}OOPM_BLOCKTRANS_T;

/** @brief 写数据块 */
typedef struct tag_OOPM_WRITEBLOCK
{
    uint16              nBlock;
    OOP_OCTETVAR_T      data;
}OOPM_WRITEBLOCK_T;

/** @brief 读数据块 */
typedef struct tag_OOPM_READBLOCK
{
    uint16  nBlock;
}OOPM_READBLOCK_T;

/** @brief 读数据块返回   */
typedef struct tag_OOPM_READRET
{
    OOP_OCTETVAR2K_T    data;
}OOPM_READRET_T;

/** @brief 软件比对 */
typedef struct tag_OOP_SOFTCOMPARE
{
    uint8   cpu;
    uint8   idx;
    uint32  factorAdd;
    uint32  dataAdd;
    uint16  unencrypt;    
}OOP_SOFTCOMPARE_T;

/** @brief 读取应用文件信息	*/
typedef struct tag_OOP_APPFILEINFO
{
    OOP_VISIBLEVAR_T	filename;	/**< 应用名称 */
    OOP_VISIBLEVAR_T	filepath;	/**< 应用路径 */
    uint32				fileoffset;	/**< 应用偏移地址 */
    uint32				filesize;	/**< 读取数据长度 */
}OOP_APPFILEINFO_T;

#define OOP_MAX_COMPATIBLE   8

/**>兼容版本*/
typedef struct tag_OOP_COMPATVER
{
    uint32          nNum;
    uint32          rsv;
    OOP_VISIBLEVAR_T value[OOP_MAX_COMPATIBLE];
} OOP_COMPATVER_T;

typedef struct tag_OOPM_STARTTRANS
{
    OOP_FILE_T      file;
    uint16          nBlockSize;
    OOP_FILEVC_T    crc;
    OOP_COMPATVER_T swlist;
    OOP_COMPATVER_T hwlist;
    OOP_VISIBLEVAR_T identy;
} OOPM_STARTTRANS_T;

/** @brief 代发 */
typedef struct tag_OOPM_FILEPROXY
{
    uint8           type;                               /**< 数据类型      */
    union 
    {
    OOP_OAD_U       oad;                                /**< DT_OAD      */
    OOP_ROAD_T      road;                               /**< DT_ROAD     */
    OOP_TSA_T       tsa;                                /**< DT_TSA      */
    OOP_MSP_T       ms;                                 /**< DT_MS      */
    };
    OOPM_STARTTRANS_T transInfo;                        /**< 物联表启动传输信息     */
    BOOL            bEncode;                            /**< 是否安全传输      */
} OOPM_FILEPROXY_T;

/** @brief 文件执行命令结果 */
typedef enum
{
    TRANS_SUCC = 100,       /**< 传输或执行操作成功                    */
    CONNECTING,             /**< 正在建立连接(扩展传输)               */
    LOGINING,               /**< 正在远程登录                     */
    EXECING ,               /**< 正在执行文件                     */
    FILENOTEXIST,           /**< 文件或目录不存在                   */
    OPER_REJECT,            /**< 操作不允许(创建/删除/读写/执行)   */
    TRANS_INTER,            /**< 文件传输中断                     */
    FILECRC_FAIL,           /**< 文件校验失败                     */
    FORWD_FAIL,             /**< 转发失败                           */
    POXTRY_FAIL,            /**< 代收失败                           */
    CONNECT_FAIL,           /**< 建立连接失败(扩展传输)           */
    LOGIN_FAIL,             /**< 远程登录失败(扩展传输)           */
    SPACE_UNENOUGH,         /**< 存储空间不足                     */
    RESET_DEFAULT = 255,    /**< 复位后默认值                     */
}OOP_FILEEXC_RST_E;

/**
********************************************************
* A.12 ESAM接口类对象
********************************************************
*/

#define OOP_MAX_SECURITYOBJ     (64)                /**< 最大安全模式对象列表数量   */

//
// F101-安全模式
//

/** @brief 安全模式开关   */
typedef enum tag_SECURITYFLAG
{
    SECURITY_OFF = 0,                               /**< 不启用安全模式参数    */
    SECURITY_ON,                                    /**< 启用安全模式参数       */
}OOP_SECURITYFLAG_E;

/** @brief 对象访问权限   */
typedef struct tag_SECURITYMODE
{
    OI          object;                             /**< 对象标识   */
    uint16      access;                             /**< 权限     */
}SECURITYMODE;

/** @brief 安全模式参数列表 */
typedef struct tag_OOP_SCURITYMODE
{
    uint32          nNum;                           /**< 安全模式对象个数   */
    uint32          rsv;
    SECURITYMODE    mode[OOP_MAX_SECURITYOBJ];      /**< 安全模式对象列表   */
}OOP_SCURITYMODE_T;

#define      OOP_MAX_KEY_NUM         2048                /**< 最大密钥个数 */

/** @brief 秘钥信息单元   */
typedef struct tag_OOP_KEYINFO_UNIT
{
    OOP_TSA_T           tsa;                            /**< 电表通信地址 */
    OOP_OCTETVAR16_T    metNum;                         /**< 表号 */
    OOP_OCTETVAR256_T   keyInfo;                        /**< 秘钥信息 */   
}OOP_KEYINFO_UNIT_T;

/** @brief 秘钥包    */
typedef struct tag_OOP_KEYINFO_TABLE
{
    uint32 nNum;
    uint32 rsv; 
    OOP_KEYINFO_UNIT_T  keyInfo[0];       /**< 秘钥包 */
}OOP_KEYINFO_TABLE_T;

/**
********************************************************
* A.13 输入输出设备接口类对象
********************************************************
*/

#define OOP_MAX_RS232           (1)     /**< 最大RS232数量                      */
#define OOP_MAX_RS485           (8)     /**< 最大RS485数量                      */
#define OOP_MAX_IR              (1)     /**< 最大红外数量                     */
#ifdef PRODUCT_ZCU_1
#define OOP_MAX_SWITCHIN        (16)    /**< 最大开入数量                     */
#define OOP_MAX_SWITCHOUT       (16)    /**< 最大开出数量                     */
#else
#define OOP_MAX_SWITCHIN        (8)     /**< 最大开入数量                     */
#define OOP_MAX_SWITCHOUT       (8)     /**< 最大开出数量                     */
#endif
#define OOP_MAX_WARNINGOUT      (1)     /**< 最大告警输出数量                   */
#define OOP_MAX_DCANALOG        (2)     /**< 最大直流模拟量路数                    */
#define OOP_MAX_MUTITERMINAL    (1)     /**< 最大多功能端子数量                    */
#define OOP_MAX_PULSE           (8)     /**< 最大脉冲输入路数                   */
#define OOP_MAX_ROUTEMODULE     (1)     /**< 最大载波/微功率无线路由模块数量 */
#define OOP_MAX_BLUETOOTH       (1)     /**< 蓝牙数量                           */
#define OOP_MAX_WANRINGOUT      (1)     /**< 最大告警输出路数                   */
#define OOP_MAX_WARINGTIME      (12)    /**< 允许告警时段                     */

/** @brief 串口定义 */
typedef struct tag_SERIALPORT
{
    OOP_VISIBLEVAR_T    name;
    OOP_COMDCB_T        param;      /**< 端口参数   */
    uint8               func;       /**< 端口功能   */
}SERIALPORT;

/** @brief F200-串口  */
typedef struct tag_OOP_SERIALPORT
{
    uint32          nNum;
    uint32          rsv;
    SERIALPORT      com[OOP_MAX_COM];       /**< 串口列表   */
}OOP_SERIALPORT_T;

/** @brief 串口配置 */
typedef struct tag_SERIALPORT_CONFIG
{
    OOP_OAD_U       port;
    OOP_COMDCB_T    param;
    uint8           func;
}OOPM_SERIALPORT_CONFIG_T;

/** @brief F201-RS485   */
typedef struct tag_OOP_RS485_DEL
{
    uint32          nNum;
    uint32          rsv;
    SERIALPORT      rs485[3];       /**< RS485列表    */
}OOP_RS485_DEL_T;

typedef struct tag_OOP_RS485
{
    uint32          nNum;
    uint32          rsv;
    SERIALPORT      rs485[OOP_MAX_RS485];   /**< RS485列表    */
}OOP_RS485_T;

/** @brief 标准串行端口定义 */
typedef struct tag_PORTCFG
{
    OOP_VISIBLEVAR_T    name;
    OOP_COMDCB_T        param;              /**< 端口参数   */
}OOP_PORTCFG_T;

/** @brief F202-红外  */
typedef struct tag_OOP_IR
{
    uint32          nNum;
    uint32          rsv;
    OOP_PORTCFG_T   ir[OOP_MAX_IR];         /**< 红外列表   */
}OOP_IR_T;

/** @brief 红外配置 */
typedef struct tag_IR_CONFIG
{
    OOP_OAD_U       oad;
    OOP_COMDCB_T    param;
}OOPM_IR_CONFIG_T;

//
// F203-开关量输入
//

/** @brief 信号量输入状态    */
typedef struct tag_INPUTSTATE
{
    uint8            st;        /**< 状态ST   */
    uint8            cd;        /**< 变位CD   */
}INPUTSTATE;

/** @brief 一组信号量输入状态  */
typedef struct tag_OOP_SWITCHIN_STATE
{
    uint32          nNum;   
    uint32          rsv;
    INPUTSTATE      state[OOP_MAX_SWITCHIN];
}OOP_SWITCHIN_STATE_T;

/** @brief 信号量接入状态    */
typedef struct tag_OOP_SWITCHIN_FLAG
{
#ifdef PRODUCT_ZCU_1
    OOP_BITSTR16_T    inputFlag;    /**< 开关量接入标志      */
    OOP_BITSTR16_T    attrFlag;     /**< 开关量属性标志      */
#else
    OOP_BITSTR8_T     inputFlag;    /**< 开关量接入标志      */
    OOP_BITSTR8_T     attrFlag;     /**< 开关量属性标志      */
#endif
}OOP_SWITCHIN_FLAG_T;

/** @brief 节点状态 bit0：门节点；bit1：上盖触点；bit2：端尾盖触点；其它保留,值为0表示分，值为1表示合 */
typedef struct tag_OOP_STATUS_SIGNAL
{
    uint32            nNum;
    OOP_BITSTR8_T     ST;     /**< 状态      */
    OOP_BITSTR8_T     CT;     /**< 变位      */
}OOP_STATUS_SIGNAL_T;

//
// F204-直流模拟量 
//

/** @brief 直流模拟量参数定义  */
typedef struct tag_DCCFGPARAM
{
    int32               startValue;     /**< 量程起始值    */
    int32               endValue;       /**< 量程结束值    */
    OOP_SCALER_UNIT_T   value;          /**< 换算及单位    */
}DCCFGPARAM;

/** @brief 一组直流模拟量配置  */
typedef struct tag_OOP_ANALOG_CONFIG
{
    uint32          nNum;
    uint32          rsv;
    DCCFGPARAM      param[OOP_MAX_DCANALOG];
}OOP_ANALOG_CONFIG_T;

/** @brief 一组直流模拟量    */
typedef struct tag_OOP_ANALOG
{
    uint32  nNum;
    uint32  rsv;
    int32   value[OOP_MAX_DCANALOG];
}OOP_ANALOG_T;

//
// F205-开关量输出
//

/** @brief 开关量状态定义    */
typedef struct tag_SWITCHOUT
{
    OOP_VISIBLEVAR_T    name;           /**< 描述符          */
    uint8               current;        /**< 当前状态(enum) */
    uint8               attribute;      /**< 开关属性(enum) */
    uint8               input;          /**< 接线状态       */
}SWITCHOUT;

/** @brief 一组开关量  */
/** 保留数据结构兼容2018-6-21*/
typedef struct tag_OOP_SWITCHOUT_DEL
{
    uint32            nNum;
    uint32           rsv;
    SWITCHOUT        value[4]; 
}OOP_SWITCHOUT_DEL_T;

typedef struct tag_OOP_SWITCHOUT
{
    uint32           nNum;
    uint32           rsv;
    SWITCHOUT        value[OOP_MAX_SWITCHOUT]; 
}OOP_SWITCHOUT_T;

/** @brief 开关量状态定义    */
typedef struct tag_SWITCHOUT_EVENT
{
    OOP_VISIBLEVAR32_T  name;           /**< 描述符          */
    uint8               current;        /**< 当前状态(enum) */
    uint8               attribute;      /**< 开关属性(enum) */
    uint8               input;          /**< 接线状态       */
}SWITCHOUT_EVENT;

typedef struct tag_OOP_SWITCHOUT_EVENT
{
    uint32           nNum;
    uint32           rsv;
    SWITCHOUT_EVENT  value[OOP_MAX_SWITCHOUT]; 
}OOP_SWITCHOUT_EVENT_T;


/** @brief 开关量配置参数    */
typedef struct tag_OOPM_SWITCHOUT_127
{
    OOP_OAD_U       idx;
    uint8           property;
}OOPM_SWITCHOUT_127_T;

/** @brief 继电器跳闸次数    */
typedef struct tag_OOPM_SWITCHOUT_NUM
{
    OOP_DATE_T      day;
    uint8           month;   //月电控跳闸次数
    uint8           buy;     //购电控跳闸次数
    uint8           power;   //功控跳闸次数
    uint8           remote;  //遥控跳闸次数
}OOPM_SWITCHOUT_NUM_T;


//
// F206-告警输出
//

/** @brief 一组告警输出状态 */
typedef struct tag_OOP_WARNINGOUT
{
    uint32      nNum;
    uint32      rsv;
    uint8       state[OOP_MAX_WARNINGOUT];
}OOP_WARNINGOUT_T;

/** @brief 告警时段定义       */
typedef struct tag_WARNINGPERIOD
{
    OOP_TIME_T        startTime;      /**< 起始时间 */
    OOP_TIME_T        endTime;        /**< 结束时间 */
}WARNINGPERIOD;

/** @brief 一组告警时段       */
typedef struct tag_OOP_WARNINGTIME
{
    uint32          nNum;   
    uint32          rsv;
    WARNINGPERIOD   period[OOP_MAX_WARINGTIME];
}OOP_WARNINGTIME_T;

/** @brief 一组告警状态   */
typedef struct tag_OOP_WARNINGSTATE
{
    uint32          nNum;
    uint32          rsv;
    uint8           nState[OOP_MAX_WANRINGOUT];
}OOP_WARNINGSTATE_T;

//
// F207-多功能端子
//

/** @brief 多功能端子工作模式  */
typedef struct tag_OOP_MULTIOUT
{
    uint8   workMode;
}MULTIOUT;

/** @brief 多功能端子模式配置  */
typedef struct tag_MULTIOUT_CONFIG
{
    OOP_OAD_U       oad;
    uint8           workMode;
}OOPM_MULTIOUT_CONFIG_T;

/** @brief 一组多功能端子    */
typedef struct tag_OOP_MUTIFUNCTERMINAL
{
    uint32      nNum;
    uint32      rsv;
    MULTIOUT    item[OOP_MAX_MUTITERMINAL];
}OOP_MUTIFUNCTERMINAL_T;

//
// F208-交采接口
//

#define OOP_MAX_AC              (1)     /**< 交流采样接口个数               */

/** @brief 交采描述符  */
typedef struct tag_AC
{
    OOP_VISIBLEVAR_T    name;
}AC;

/** @brief 交采设备集合   */
typedef struct tag_OOP_AC
{
    uint32          nNum;
    uint32          rsv;
    AC              port[OOP_MAX_AC];
}OOP_AC_T;

//
// F20A-脉冲输入设备
//

/** @brief 脉冲端口描述符    */
typedef struct tag_PULSE
{
    OOP_VISIBLEVAR_T    name;
}PULSE;

/** @brief 脉冲端口集合   */
typedef struct tag_OOP_PULSE
{
    uint32          nNum;
    uint32          rsv;
    PULSE           port[OOP_MAX_PULSE];
}OOP_PULSE_T;

//
// F209-载波/微功率无线路由模块
//

#define OOP_TRANSCMD_SIZE   (256)       /**< 最大透明转发字节数            */
#define OOP_TRANSRET_SIZE   (512)       /**< 最大透明转发应答字节数      */

/** @brief 路由版本信息   */
typedef struct tag_RouteVersion
{
    OOP_VISIBLEVAR_T    factory;
    OOP_VISIBLEVAR_T    ic;
    OOP_DATE_T          dtVer;
    uint16              wVer;
}ROUTE_VERSION;

/** @brief 路由模块单元   */
typedef struct tag_ROUTEMODULE_DEL
{
    OOP_VISIBLEVAR_T    name;           // 描述符
    OOP_COMDCB_T        dcb;            // 通讯参数
    ROUTE_VERSION       verInfo;        // 版本信息
}ROUTEMODULE_DEL;

typedef struct tag_ROUTEMODULE
{
    OOP_VISIBLEVAR256_T name;           /**< 描述符      */
    OOP_COMDCB_T        dcb;            /**< 通讯参数   */
    ROUTE_VERSION       verInfo;        /**< 版本信息   */
}ROUTEMODULE;

/** @brief 透明转发命令   */
typedef struct tag_OOPM_TRANSCMD
{
    OOP_TSA_T       add;
    uint16          nTimeout;
    uint32          nLen;
    uint8           cmd[OOP_TRANSCMD_SIZE];
}OOPM_TRANSCMD_T;

/** @brief 配置端口参数   */
typedef struct tag_OOPM_PORTPARAM
{
    OOP_OAD_U       idx;
    OOP_COMDCB_T    dcb;
}OOPM_PORTPARAM_T;

#define OOP_MAX_PLNODE (1024)   /**< 最大子节点个数          */

/** @brief 子节点信息  */
typedef struct tag_OOP_CHILDNODE
{
    uint16              index;      // 从节点序号
    OOP_SA_T            add;        // 从节点通信地址
    OOP_VISIBLEVAR256_T id;         // 从节点描述符
    #if 0
    uint16              wResponse;  // 从节点响应时长
    OOP_DATETIME_S_T    dtLastOk;   // 从节点最近一次通信成功时间
    #endif
}OOP_CHILDNODE_T;

/** @brief 网络拓扑单元   */
typedef struct tag_OOP_NETTOPOLOGYINFO
{
    OOP_OCTETSTR6_T     addr;
    uint8               type;
    uint16              nid;
    uint16              proxynid;
    uint8               info;
}OOP_NETTOPOLOGYINFO_T;

typedef struct tag_OOP_NETTOPOLOGYINFO_LIST
{
    uint8                 nNum;
    OOP_NETTOPOLOGYINFO_T item[OOP_MAX_PLNODE];
}OOP_NETTOPOLOGYINFO_LIST_T;

#define OOP_MAX_NEIGHBOUR (255) /**< 最大邻居节点数量           */

/** @brief 邻居节点信息   */
typedef struct tag_OOP_NETSINFO
{
    OOP_OCTETSTR3_T     nodenid;
    OOP_OCTETSTR6_T     addr;
    uint8               nNum;
    OOP_OCTETSTR3_T     neighbour[OOP_MAX_NEIGHBOUR];
}OOP_NETSINFO_T;

#define OOP_MAX_PLC     (1)     /**< 最大路由模块个数           */

/** @brief 一组路由模块   */
typedef struct tag_OOP_ROUTEMODULE_DEL
{
    uint32              nNum;
    uint32              resv;
    ROUTEMODULE_DEL     item[OOP_MAX_PLC];
}OOP_ROUTEMODULE_DEL_T;

typedef struct tag_OOP_ROUTEMODULE
{
    uint32          nNum;
    uint32          resv;
    ROUTEMODULE     item[OOP_MAX_PLC];
}OOP_ROUTEMODULE_T;

/** @brief 属性10:相位信息    */
typedef struct tag_OOP_NODEPHASEINFO
{
    uint16              index;
    OOP_OCTETVAR16_T    addr;   /**< (SIZE(6))          */
    uint16              info;
}OOP_NODEPHASEINFO_T;

// F209 属性12:版本信息
typedef struct tag_OOP_NODEVERSIONINFO
{
    OOP_OCTETSTR6_T     addr;
    uint8               type;
    uint16              wVer;
    OOP_DATE_T          dtVer;
    OOP_VISIBLEVAR_T    factory;
    OOP_VISIBLEVAR_T    ic;
}OOP_NODEVERSIONINFO_T; 

// F209 属性30 网络基本信息
#define OOP_MAX_LEVELNUM    (64)

typedef struct tag_OOP_LEVELINFO
{
    uint8   nLevel;     // 所在层级
    uint16  wLevelNum;  // 此层级节点个数
}OOP_LEVELINFO;

typedef struct tag_OOP_NETBASEINFO
{
    uint16          wNetNum;                        // 网络节点总数
    uint16          wOnLineNum;                     // 在线站点个数
    uint32          dwStartTime;                    // 网络启动组网时长
    uint16          wNetTime;                       // 组网时长
    uint8           nBeacon;                        // 信标周期
    uint16          wRoute;                         // 路由周期
    uint16          wTopCnt;                        // 拓扑变更次数
    uint32          nLevelNum;                      // 层级个数
    OOP_LEVELINFO   levelInfo[OOP_MAX_LEVELNUM];    // 层级信息
}OOP_NETBASEINFO_T;

// F209 属性31 网络节点信息
#define OOP_MAX_DWNODENUM   (12)

typedef struct tag_OOP_NODETOPINFO
{
    uint16          nid;        // 节点标识
    uint16          proxynid;   // 代理节点标识
    OOP_BITSTR8_T   info;       // 节点信息: bit-string(SIZE(8))
}OOP_NODETOPINFO_T;

typedef struct tag_OOP_DWNODEINFO
{
    OOP_OCTETSTR6_T     addr;       // 下接从节点地址：octet-string(SIZE(6))
    uint8               nProtocol;  // 下接从节点通信协议类型
    uint8               nDevice;    // 下接从节点设备类型
}OOP_DWNODEINFO_T;

typedef struct tag_OOP_NETNODEINFO
{
    uint16                  wIndex;                         // 节点序号
    OOP_OCTETSTR6_T         nodeAddr;                       // 节点地址：octet-string(SIZE(6))
    OOP_NODETOPINFO_T       topInfo;                        // 节点拓扑信息
    OOP_BITSTR8_T           netInfo;                        // 网络信息：bit-string(SIZE(8))
    OOP_BITSTR16_T          device;                         // 设备类型：bit-string(SIZE(16))
    OOP_BITSTR8_T           phase;                          // 相位信息：bit-string(SIZE(8))
    uint16                  wProxyCnt;                      // 代理变更次数
    uint16                  wOffLineCnt;                    // 站点离线次数
    uint32                  dwOffLineTime;                  // 站点离线时间
    uint32                  dwMaxOffLine;                   // 站点离线最大时间
    uint32                  dwSuccessRateUp;                // 上行通信成功率
    uint32                  dwSuccessRateDw;                // 下行通信成功率
    OOP_OCTETSTR3_T         mainVer;                        // 主版本号: octet-string(SIZE (3))
    OOP_OCTETSTR2_T         subVer;                         // 次版本：octet-string(SIZE (2))
    uint16                  wSkipInfo;                      // 下一跳信息
    OOP_BITSTR16_T          channel;                        // 信道类型：bit-string(SIZE(16))
    uint8                   nProtocol;                      // 规约类型
    uint8                   nAreaState;                     // 台区状态
    OOP_OCTETSTR6_T         areaAddr;                       // 台区号地址：octet-string(SIZE(6))
    uint32                  nDownNum;                       // 从节点下接从节点数量
    OOP_DWNODEINFO_T        downInfo[OOP_MAX_DWNODENUM];    // 下接节点信息
}OOP_NETNODEINFO_T;

//
// F20B-蓝牙
//
#define     OOP_MAX_BTMOD       (8)

/** @brief 配对模式 */
typedef enum
{
    MOD_NC = 0,     /**< Numeric Comparison */
    MOD_JW,         /**< Just Works         */
    MOD_PE,         /**< Passkey Entry      */
    MOD_OOB,        /**< Out of Band        */
}OOP_PAIRMODE_E;

/** @brief 脉冲类型 */
typedef enum
{
    TP_ENEA = 0,        /**< 有功         */
    TP_ENEI,            /**< 无功         */
    TP_HARMP,           /**< 正向谐波       */
    TP_HARMN,           /**< 反向谐波       */
    TP_CLK,             /**< 时钟         */
}OOP_PULSETYPE_E;

/** @brief 命令类型 */
typedef enum
{
    CMD_OPEN = 0,       /**< 打开     */
    CMD_CLOSE,          /**< 关闭     */
}OOP_ERRCHKCMD_E;

/** @brief 蓝牙信息 */
typedef struct tag_OOP_BLUETOOTHINFO
{
    OOP_VISIBLEVAR256_T info;       /**< 蓝牙描述符 */
    OOP_OCTETVAR16_T    mac;        /**< MAC地址      */
}OOP_BLUETOOTHINFO_T;

/** @brief 蓝牙从设备  */
typedef struct tag_OOP_BLUETOOTHSLAVE
{
    OOP_OCTETVAR16_T    mac;        /**< MAC地址 */
    OOP_OCTETVAR64_T    assetNo;    /**< 资产号     */
}OOP_BLUETOOTHSLAVE_T;

/** @brief 蓝牙模块 */
typedef struct tag_OOP_BLUETOOTHMOD
{
    uint8               nIdx;       /**< 模块序号 */
    OOP_BLUETOOTHINFO_T info;       /**< 蓝牙信息     */
    OOP_VISIBLEVAR_T    pass;       /**< 配对密码     */
    uint8               nMod;       /**< 配对模式     */
    OOP_OCTETVAR64_T    sn;         /**< 蓝牙序列号   */
}OOP_BLUETOOTHMOD_T;

/** @brief 连接信息 */
typedef struct tag_OOP_BTCONNECTINFO
{
    OOP_OAD_U           oad;                        /**< 端口号 */
    uint32              nMasterNum;
    OOP_BLUETOOTHINFO_T master[OOP_MAX_BTMOD];      /**< 连接的主设备     */  
    uint32              nSlaveNum;
    OOP_BLUETOOTHINFO_T slave[OOP_MAX_BTMOD];       /**< 连接的从设备     */
}OOP_BTCONNECTINFO_T;

/** @brief 工作参数 */
typedef struct tag_OOP_BTWORKPARAM
{
    OOP_OAD_U           oad;                        /**< 端口号 */
    uint8               nPowerLevel;                /**< 发射功率档 */
    uint16              nBroadCastInterval;         /**< 广播间隔     */    
    uint16              nScanPeriod;                /**< 扫描间隔     */
}OOP_BTWORKPARAM_T;

/** @brief 蓝牙从设备列表  */
typedef struct tag_OOP_BLUETOOTHSLAVES
{
    uint32               nNum;
    uint32               resv;
    OOP_BLUETOOTHSLAVE_T value[OOP_MAX_BTMOD];
}OOP_BLUETOOTHSLAVES_T;

typedef struct tag_OOP_BLUETOOTHMODS
{
    uint32              nNum;
    uint32              resv;
    OOP_BLUETOOTHMOD_T  value[OOP_MAX_BLUETOOTH];
}OOP_BLUETOOTHMODS_T;

typedef struct tag_OOP_BTWORKPARAMS
{
    uint32          nNum;   
    uint32          resv;
    OOP_BTWORKPARAM_T param[OOP_MAX_BLUETOOTH];
}OOP_BTWORKPARAMS_T;

/** @brief 配置端口 */
typedef struct tag_OOPM_BLUETOOTHCFG
{
    OOP_OAD_U       oad;            /**< 端口号 */
    OOP_BLUETOOTHINFO_T info;       /**< 蓝牙信息 */
}OOPM_BLUETOOTHCFG_T;

/** @brief 误差检定 */
typedef struct tag_OOPM_BLUETOOTHERRCHK
{
    OOP_OAD_U         oad;            /**< 端口号 */
    uint8             nType;          /**< 脉冲类型enum {秒脉冲(0)，需量周期(1)，时段投切(2)，正向谐波脉冲(3)，
                                           反向谐波脉冲(4)，无功脉冲(5)，有功脉冲(6)，退出检定模式(255) } */
    uint8             interval;       /**< 信道发送间隔  单位ms */
    uint8             power;          /**< 校表发射功率  */
    OOP_OCTETVAR16_T  addr;           /**< 电能表通信地址 */
    uint8             nNum;           /**< 发射频率数量 */
    uint16            nFreq[5];       /**< 发射频率，单位：MHz，无换算 */
}OOPM_BLUETOOTHERRCHK_T;

/** @brief 配对密码 */
typedef struct tag_OOPM_BLUETOOTHPASS
{
    OOP_OAD_U        oad;            /**< 端口号 */
    OOP_VISIBLEVAR_T pass;           /**< 配对密码 */
}OOPM_BLUETOOTHPASS_T;

//
// F20C-电台
//

#define OOP_MAX_RADIO       (1) /**< 最大电台设备个数   */

/** @brief 电台版本信息   */
typedef struct tag_OOP_HSVERSION
{
    OOP_VISIBLESTR20_T  szSoftVer;  // 软件版本号
    OOP_VISIBLESTR20_T  szHardVer;  // 硬件版本号
}OOP_HSVERSION_T;

/** @brief 电台单元 */
typedef struct tag_RADIO
{
    OOP_VISIBLEVAR_T    name;
}RADIO;

/** @brief 一组电台设备   */
typedef struct tag_OOP_RADIO
{
    uint32          nNum;
    uint32          rsv;
    RADIO           port[OOP_MAX_RADIO];
}OOP_RADIO_T;

/** @brief 电台频道 */
typedef struct tag_OOP_RADIO_CHS
{
    uint32          nNum;
    uint32          rsv;
    uint8           item[OOP_MAX_RADIO];
}OOP_RADIO_CHS_T;

/** @brief 电台信号轻度   */
typedef struct tag_OOP_RADIO_SIGS
{
    uint32          nNum;
    uint32          rsv;
    uint8           item[OOP_MAX_RADIO];
}OOP_RADIO_SIGS_T;

//
// F20D-模组
//

#define OOP_MAX_MODULE_CHANNEL  (8)
#define OOP_MAX_MODULE  (5)

/** @brief 通道信息 */
typedef struct tag_OOP_MODULE_CHANNEL_INFO
{
    uint8                   nType;                  /**< 接口类型 */    
    uint8                   nFunc;                  /**< 功能配置 */
}OOP_MODULE_CHANNEL_INFO;

/** @brief 模组信息 */
typedef struct tag_OOP_MODULE_UINT
{
    uint8                   nSlot;                          /**< 槽位号 */
    OOP_VISIBLEVAR128_T     modType;                        /**< 模块设备型号 */
    OOP_VISIBLEVAR128_T     modId;                          /**< 模块设备ID */
    int32                   nSoftVer;                       /**< 软件版本 */
    OOP_VISIBLEVAR_T        softDate;                       /**< 软件发布日期 */
    int32                   nHardVer;                       /**< 硬件版本 */
    OOP_VISIBLEVAR_T        hardDate;                       /**< 硬件发布日期 */
    OOP_VISIBLEVAR128_T     manuf;                          /**< 厂商编码 */
    uint32                  nChNum;                         /**< 通道信息个数 */
    OOP_MODULE_CHANNEL_INFO chInfo[OOP_MAX_MODULE_CHANNEL]; /**< 通道信息 */
}OOP_MODULE_UINT_T;

/** @brief 模组   */
typedef struct tag_OOP_MODULE_INFO
{
    uint32              nNum;
    uint32              rsv;
    OOP_MODULE_UINT_T   item[OOP_MAX_MODULE];
}OOP_MODULE_INFO_T;

/** @brief 通道信息 */
typedef struct 
{
    uint8                   nType;                  /**< 接口类型 */    
    uint8                   nFunc;                  /**< 功能配置 */
    OOP_VISIBLEVAR128_T     nDriveName;             //驱动名称
}OOP_MODULE_CHANNEL_INFO_NEW;

/** @brief 模组信息 */
typedef struct 
{
    uint8                   nSlot;                          /**< 槽位号 */
    OOP_VISIBLEVAR128_T     modType;                        /**< 模块设备型号 */
    OOP_VISIBLEVAR128_T     modId;                          /**< 模块设备ID */
    OOP_VISIBLEVAR128_T     nSoftVer;                       /**< 软件版本 */
    OOP_VISIBLEVAR128_T     softDate;                       /**< 软件发布日期 */
    OOP_VISIBLEVAR128_T     nHardVer;                       /**< 硬件版本 */
    OOP_VISIBLEVAR128_T     hardDate;                       /**< 硬件发布日期 */
    OOP_VISIBLEVAR128_T     manuf;                          /**< 厂商编码 */
    uint32                  nChNum;                         /**< 通道信息个数 */
    OOP_MODULE_CHANNEL_INFO_NEW chInfo[OOP_MAX_MODULE_CHANNEL]; /**< 通道信息 */
}OOP_MODULE_UINT_NEW_T;

/** @brief 模组   */
typedef struct 
{
    uint32              nNum;
    uint32              rsv;
    OOP_MODULE_UINT_NEW_T   item[OOP_MAX_MODULE];
}OOP_MODULE_INFO_NEW_T;

//
// F20E-USB
//

#define OOP_MAX_USB (1)     /**< 最大USB设备个数  */

/** @brief USB单元    */
typedef struct tag_USB
{
    OOP_VISIBLEVAR_T    name;
}USB;

/** @brief 一组USB设备  */
typedef struct tag_OOP_USB
{
    uint32  nNum;
    uint32  rsv;
    USB     item[OOP_MAX_USB];
}OOP_USB_T;

//
// F20F-服务组件
//

#define OOP_MAX_PLUGIN  (32)

/** @brief 组件信息 */
typedef struct tag_OOP_PLUGIN_UINT
{
    OOP_VISIBLEVAR128_T     name;                       /**< 组件名称 */
    OOP_VISIBLEVAR_T		softver;					/**< 软件版本 */
    OOP_DATETIME_S_T        dtSoft;                     /**< 发布日期 */
    OOP_OCTETVAR64_T        appid;                      /**< app id */
}OOP_PLUGIN_UINT_T;

/** @brief 组件   */
typedef struct tag_OOP_PLUGIN_INFO
{
    uint32              nNum;
    uint32              rsv;
    OOP_PLUGIN_UINT_T   item[OOP_MAX_PLUGIN];
}OOP_PLUGIN_INFO_T;

//
// F213-容器集合
//

#define OOP_MAX_CONTAINER_OAD   (64)
#define OOP_MAX_CONTAINER_NUM   (10)

/** @brief 对象容器*/
typedef struct tag_OOP_CONTAINER_CFG
{
    uint32      nNum;
    uint32      rsv;
    OOP_OAD_U   oad[OOP_MAX_CONTAINER_OAD];
}OOP_CONTAINER_CFG_T;

/** @brief 对象容器集合*/
typedef struct tag_OOP_CONTAINER_CFGS
{
    uint32              nNum;
    uint32              rsv;
    OOP_CONTAINER_CFG_T cfg[OOP_MAX_CONTAINER_NUM];
}OOP_CONTAINER_CFGS_T;

/** @brief 对象容器操作参数*/
typedef struct tag_OOPM_CONTAINER_PARAM
{   
    OOP_OAD_U           oad;
    OOP_CONTAINER_CFG_T cfg;
}OOPM_CONTAINER_PARAM_T;

/** @brief 对象容器值域*/
typedef struct tag_OOP_CONTAINER_VAL
{
    OOP_CONTAINER_CFG_T     cfg;
    uint8                   value[OOP_MAX_CONTAINER_OAD*128];
}OOP_CONTAINER_VAL;

typedef struct tag_OOP_CONTAINER_VALS
{
    uint32              nNum;
    uint32              rsv;
    OOP_CONTAINER_VAL   item[OOP_MAX_CONTAINER_NUM];
}OOP_CONTAINER_VALS;

/** @brief 死区参数 */
typedef struct tag_OOP_DEADAREA
{
    int32       ampda;  
    uint32      acvolda;
    uint32      dcvolda;
    int32       pwrda;
    int16       freda;  /**< 频率死区   */
}OOP_DEADAREA_T;

/** @brief 死区参数 */
typedef struct tag_OOP_PT_PARA
{
    int32       ptrat_1;    /**< 一次额定值    uint16*/
    int32       ptrat_2;    /**< 二次额定值    uint16*/
}OOP_PT_PARA_T;


/** @brief 过载、重载参数    */
typedef struct tag_OOP_LOAD_PARA
{
    uint32      limit;      /**< 电流阈值*/
    int16       delay;      /**< 告警延时*/
}OOP_LOAD_PARA_T;

typedef struct tag_OOP_TTU_MIX
{
    int32       trans_cap;          /**< 配变容量   >*/
    float32     i_unbalance;        /**< 电流不平衡定值>*/
    float32     u_unbalance;        /**< 电压不平衡定值>*/
    int32       chktime_ti;         /**< 开关对时周期>*/
    float32     volharm_da;         /**< 电压谐波死区>*/  
    float32     ampharm_da;         /**< 电流谐波死区>*/  
    float32     unbanl_da;          /**< 不平衡度死区>*/  
    float32     loadrate_da;        /**< 负载率死区>*/ 
    
    float32     transshort_cap;     /**< 最小短路容量>*/  

    float32     pf_da;              /**< 功率因素死区>*/
    int32       ctrat_1;            /**< CT一次定值>*/          
    int32       ctrat_2;            /**< CT二次定值>*/
    
    float32     volover_val;        /**< 过压定值>*/    
    int32       volover_time;       /**< 持续时间>*/
    float32     vollow_val;         /**< 低压定值>*/    
    int32       vollow_time;        /**< 持续时间>*/
    float32     loadover_val;       /**< 过载定值>*/    
    int32       loadover_time;      /**< 持续时间>*/
    float32     loadheavy_val;      /**< 重载定值>*/    
    int32       loadheavy_time;     /**< 持续时间>*/
    int32       filter_time;        /**< 开入去抖动时间>*/
    float32     volt_haves;         /**< 有压定值>*/
    float32     volt_no;            /**< 无压定值>*/
    float32     current_no;         /**< 无流定值>*/
    float32     volt_zero;          /**< 电压零漂>*/
    float32     current_zero;       /**< 电流零漂>*/
    int32       zero_current_have;  /**< 零序电流独立>*/
    float32     current_over;       /**< 电流越限定值>*/
    int32       current_over_time;  /**< 电流越限时间>*/
    int32       unbalance_mode;     /**< 不平衡公式选择>*/
}OOP_TTU_MIX_T;

/*
*********************************************************************
* 自定义结构体
*********************************************************************
*/

/** @brief 交采电量数据块    */
typedef struct tag_OOP_ENERGY_BLOCK
{
    OOP_ENERGYS_T enepa_z;  /**< 组合有功电能量>*/

    OOP_ENERGY_T enepaT;    /**< 正向有功电能量>*/
    OOP_ENERGY_T enepaA;    /**< A相正向有功电能量>*/
    OOP_ENERGY_T enepaB;    /**< B相正向有功电能量>*/
    OOP_ENERGY_T enepaC;    /**< C相正向有功电能量>*/

    OOP_ENERGY_T enenaT;    /**< 反向有功电能量>*/
    OOP_ENERGY_T enenaA;    /**< A相反向有功电能量>*/
    OOP_ENERGY_T enenaB;    /**< B相反向有功电能量>*/
    OOP_ENERGY_T enenaC;    /**< C相反向有功电能量>*/

    OOP_ENERGYS_T enepiT;   /**< 正向无功电能量>*/
    OOP_ENERGYS_T enepiA;   /**< A相正向无功电能量>*/
    OOP_ENERGYS_T enepiB;   /**< B相正向无功电能量>*/
    OOP_ENERGYS_T enepiC;   /**< C相正向无功电能量>*/

    OOP_ENERGYS_T eneniT;   /**< 反向无功电能量>*/
    OOP_ENERGYS_T eneniA;   /**< A相反向无功电能量>*/
    OOP_ENERGYS_T eneniB;   /**< B相反向无功电能量>*/
    OOP_ENERGYS_T eneniC;   /**< C相反向无功电能量>*/

    OOP_ENERGY_T enepi1T;   /**< 1象限无功电能量>*/
    OOP_ENERGY_T enepi1A;   /**< A相1象限无功电能量>*/
    OOP_ENERGY_T enepi1B;   /**< B相1象限无功电能量>*/
    OOP_ENERGY_T enepi1C;   /**< C相1象限无功电能量>*/

    OOP_ENERGY_T eneni2T;   /**< 2象限无功电能量>*/
    OOP_ENERGY_T eneni2A;   /**< A相2象限无功电能量>*/
    OOP_ENERGY_T eneni2B;   /**< B相2象限无功电能量>*/
    OOP_ENERGY_T eneni2C;   /**< C相2象限无功电能量>*/

    OOP_ENERGY_T eneni3T;   /**< 3象限无功电能量>*/
    OOP_ENERGY_T eneni3A;   /**< A相3象限无功电能量>*/
    OOP_ENERGY_T eneni3B;   /**< B相3象限无功电能量>*/
    OOP_ENERGY_T eneni3C;   /**< C相3象限无功电能量>*/

    OOP_ENERGY_T enepi4T;   /**< 4象限无功电能量>*/
    OOP_ENERGY_T enepi4A;   /**< A相4象限无功电能量>*/
    OOP_ENERGY_T enepi4B;   /**< B相4象限无功电能量>*/
    OOP_ENERGY_T enepi4C;   /**< C相4象限无功电能量>*/

    OOP_ENERGY_T enepsT;    /**< 正向视在电能>*/
    OOP_ENERGY_T enepsA;    /**< A相正向视在电能>*/
    OOP_ENERGY_T enepsB;    /**< B相正向视在电能>*/
    OOP_ENERGY_T enepsC;    /**< C相正向视在电能>*/

    OOP_ENERGY_T enensT;    /**< 反向视在电能>*/
    OOP_ENERGY_T enensA;    /**< A相反向视在电能量>*/
    OOP_ENERGY_T enensB;    /**< B相反向视在电能>*/
    OOP_ENERGY_T enensC;    /**< C相反向视在电能>*/
}OOP_ENERGY_BLOCK_T;

/** @brief 交采高精度电量数据块   */
typedef struct tag_OOP_HENERGY_BLOCK 
{
    OOP_HENERGYS_T enepa_z; /**< 组合有功电能量>*/

    OOP_HENERGY_T enepaT;   /**< 正向有功电能量>*/
    OOP_HENERGY_T enepaA;   /**< A相正向有功电能量>*/
    OOP_HENERGY_T enepaB;   /**< B相正向有功电能量>*/
    OOP_HENERGY_T enepaC;   /**< C相正向有功电能量>*/

    OOP_HENERGY_T enenaT;   /**< 反向有功电能量>*/
    OOP_HENERGY_T enenaA;   /**< A相反向有功电能量>*/
    OOP_HENERGY_T enenaB;   /**< B相反向有功电能量>*/
    OOP_HENERGY_T enenaC;   /**< C相反向有功电能量>*/

    OOP_HENERGYS_T enepiT;   /**< 正向无功电能量>*/
    OOP_HENERGYS_T enepiA;   /**< A相正向无功电能量>*/
    OOP_HENERGYS_T enepiB;   /**< B相正向无功电能量>*/
    OOP_HENERGYS_T enepiC;   /**< C相正向无功电能量>*/

    OOP_HENERGYS_T eneniT;   /**< 反向无功电能量>*/
    OOP_HENERGYS_T eneniA;   /**< A相反向无功电能量>*/
    OOP_HENERGYS_T eneniB;   /**< B相反向无功电能量>*/
    OOP_HENERGYS_T eneniC;   /**< C相反向无功电能量>*/

    OOP_HENERGY_T enepi1T;   /**< 1象限无功电能量>*/
    OOP_HENERGY_T enepi1A;   /**< A相1象限无功电能量>*/
    OOP_HENERGY_T enepi1B;   /**< B相1象限无功电能量>*/
    OOP_HENERGY_T enepi1C;   /**< C相1象限无功电能量>*/

    OOP_HENERGY_T eneni2T;   /**< 2象限无功电能量>*/
    OOP_HENERGY_T eneni2A;   /**< A相2象限无功电能量>*/
    OOP_HENERGY_T eneni2B;   /**< B相2象限无功电能量>*/
    OOP_HENERGY_T eneni2C;   /**< C相2象限无功电能量>*/

    OOP_HENERGY_T eneni3T;   /**< 3象限无功电能量>*/
    OOP_HENERGY_T eneni3A;   /**< A相3象限无功电能量>*/
    OOP_HENERGY_T eneni3B;   /**< B相3象限无功电能量>*/
    OOP_HENERGY_T eneni3C;   /**< C相3象限无功电能量>*/

    OOP_HENERGY_T enepi4T;   /**< 4象限无功电能量>*/
    OOP_HENERGY_T enepi4A;   /**< A相4象限无功电能量>*/
    OOP_HENERGY_T enepi4B;   /**< B相4象限无功电能量>*/
    OOP_HENERGY_T enepi4C;   /**< C相4象限无功电能量>*/

    OOP_HENERGY_T enepsT;   /**< 正向视在电能>*/
    OOP_HENERGY_T enepsA;   /**< A相正向视在电能>*/
    OOP_HENERGY_T enepsB;   /**< B相正向视在电能>*/
    OOP_HENERGY_T enepsC;   /**< C相正向视在电能>*/

    OOP_HENERGY_T enensT;   /**< 反向视在电能>*/
    OOP_HENERGY_T enensA;   /**< A相反向视在电能>*/
    OOP_HENERGY_T enensB;   /**< B相反向视在电能>*/
    OOP_HENERGY_T enensC;   /**< C相反向视在电能>*/
}OOP_HENERGY_BLOCK_T;

/** @brief 交采需量数据块    */
typedef struct tag_OOP_DEMAND_BLOCK
{
    OOP_DEMAND_T dmnpaT;  /**< 正向有功最大需量>*/
    OOP_DEMAND_T dmnpaA;  /**< 正向有功A相最大需量>*/
    OOP_DEMAND_T dmnpaB;  /**< 正向有功B相最大需量>*/
    OOP_DEMAND_T dmnpaC;  /**< 正向有功C相最大需量>*/

    OOP_DEMAND_T dmnnaT;  /**< 反向有功最大需量>*/
    OOP_DEMAND_T dmnnaA;  /**< 反向有功A相最大需量>*/
    OOP_DEMAND_T dmnnaB;  /**< 反向有功B相最大需量>*/
    OOP_DEMAND_T dmnnaC;  /**< 反向有功C相最大需量>*/

    OOP_DEMANDS_T dmnpiT;  /**< 正向无功最大需量>*/
    OOP_DEMANDS_T dmnpiA;  /**< 正向无功A相最大需量>*/
    OOP_DEMANDS_T dmnpiB;  /**< 正向无功B相最大需量>*/
    OOP_DEMANDS_T dmnpiC;  /**< 正向无功C相最大需量>*/

    OOP_DEMANDS_T dmnniT;  /**< 反向无功最大需量>*/
    OOP_DEMANDS_T dmnniA;  /**< 反向无功A相最大需量>*/
    OOP_DEMANDS_T dmnniB;  /**< 反向无功B相最大需量>*/
    OOP_DEMANDS_T dmnniC;  /**< 反向无功C相最大需量>*/

    OOP_DEMAND_T dmnpi1T;  /**< 象限1无功最大需量>*/
    OOP_DEMAND_T dmnpi1A;  /**< 象限1无功A相最大需量>*/
    OOP_DEMAND_T dmnpi1B;  /**< 象限1无功B相最大需量>*/
    OOP_DEMAND_T dmnpi1C;  /**< 象限1无功C相最大需量>*/

    OOP_DEMAND_T dmnni2T;  /**< 象限2无功最大需量>*/
    OOP_DEMAND_T dmnni2A;  /**< 象限2无功A相最大需量>*/
    OOP_DEMAND_T dmnni2B;  /**< 象限2无功B相最大需量>*/
    OOP_DEMAND_T dmnni2C;  /**< 象限2无功C相最大需量>*/

    OOP_DEMAND_T dmnni3T;  /**< 象限3无功最大需量>*/
    OOP_DEMAND_T dmnni3A;  /**< 象限3无功A相最大需量>*/
    OOP_DEMAND_T dmnni3B;  /**< 象限3无功B相最大需量>*/
    OOP_DEMAND_T dmnni3C;  /**< 象限3无功C相最大需量>*/

    OOP_DEMAND_T dmnpi4T;  /**< 象限4无功最大需量>*/
    OOP_DEMAND_T dmnpi4A;  /**< 象限4无功A相最大需量>*/
    OOP_DEMAND_T dmnpi4B;  /**< 象限4无功B相最大需量>*/
    OOP_DEMAND_T dmnpi4C;  /**< 象限4无功C相最大需量>*/

    OOP_DEMAND_T dmnpsT;   /**< 正向视在最大需量>*/
    OOP_DEMAND_T dmnpsA;   /**< 正向视在A相最大需量>*/
    OOP_DEMAND_T dmnpsB;   /**< 正向视在B相最大需量>*/
    OOP_DEMAND_T dmnpsC;   /**< 正向视在C相最大需量>*/

    OOP_DEMAND_T dmnnsT;   /**< 反向视在最大需量>*/
    OOP_DEMAND_T dmnnsA;   /**< 反向视在A相最大需量>*/
    OOP_DEMAND_T dmnnsB;   /**< 反向视在B相最大需量>*/
    OOP_DEMAND_T dmnnsC;   /**< 反向视在C相最大需量>*/
}OOP_DEMAND_BLOCK_T;

/** @brief 交采瞬时值数据块 */
typedef struct tag_OOP_INS_BLOCK
{
    OOP_WORD4V_T    vol;            /**< 电压>*/
    OOP_INT4V_T     amp;            /**< 电流>*/
    int32           ampn;           /**< 电流零线>*/
    OOP_WORD4V_T    phu;            /**< 电压相角>*/
    OOP_WORD4V_T    phi;            /**< 电流相角>*/
    OOP_INT4V_T     ppwr;           /**< 有功功率>*/
    OOP_INT4V_T     qpwr;           /**< 无功功率>*/
    OOP_INT4V_T     spwr;           /**< 视在功率>*/
    OOP_LONG4V_T    pwrf;           /**< 功率因数>*/
    uint16          frequency;      /**< 电网频率>*/
    OOP_METWORD_T   MetStatus;      /**< 电能表状态字>*/
    uint16          volUnbd;        /**< 电压不平衡>*/
    uint16          ampUnbd;        /**< 电流不平衡>*/
    OOP_WORD4V_T    linevol;        /**< 线电压>*/
    OOP_INT4V_T  voldeviation;      /**< 电压偏差>*/
    int32       freqdeviation;      /**< 电流偏差>*/
    int32       pfa;                /**< 功率因数角>*/
}OOP_INS_BLOCK_T;

/** @brief 交采谐波数据块    */
typedef struct tag_OOP_HARMONIC_BLOCK
{
    OOP_HARMONIC_T volharmrat[3];
    OOP_HARMONIC_T ampharmrat[3];
    OOP_HARMONIC_U_VALUE_T volharmval[3];
    OOP_HARMONIC_I_VALUE_T ampharmval[3];
}OOP_HARMONIC_BLOCK_T;

/** @brief 交采电能质量数据块  */
typedef struct tag_OOP_QUALITY_BLOCK
{
    uint16          zerovol;        /**< 零序电压>*/
    int32           zeroamp;        /**< 零序电流>*/
    uint16          volUnbd_v;      /**< 电压不平衡（矢量和法）>*/
    uint16          ampUnbd_v;      /**< 电流不平衡（矢量和法）>*/
    uint16          zerovol_v;      /**< 零序电压>（矢量和法）*/
    int32           zeroamp_v;      /**< 零序电流>（矢量和法）*/
}OOP_QUALITY_BLOCK_T;

/** @brief 容器配置信息   */
typedef struct
{
    uint8               cpucores;               /**< CPU核数>*/
    uint16              cputhreshold;
    uint16               memlimit;
    uint16              memthreshold;
    uint16               disklimit;
    uint16              diskthreshold;
//    uint8               hostfilenum;
    OOP_VISIBLEVAR256_T    hostfile;
    OOP_VISIBLEVAR256_T    savefilepath;
 //   uint8               hardinterfacenum;
    OOP_VISIBLEVAR256_T    hardinterface;
    OOP_VISIBLEVAR256_T    portconfig;
}OOP_CONTAINER_CONINFO_T;

/** @brief 容器单元 */
typedef struct tag_OOP_CONTAINER
{
    OOP_VISIBLEVAR_T        containerid;            /**< 容器ID>*/
    OOP_VISIBLEVAR_T        containername;          /**< 容器名称>*/
    OOP_VISIBLEVAR_T        containerstate;         /**< 容器状态>*/
    uint16                  cpuusage;               /**< 容器CPU使用率 单位：%，换算：-2>*/
    uint16                  memusage;               /**< 容器内存使用率>*/
    uint16                  storageusage;           /**< 容器存储使用率>*/
    uint8                   ipaddress[4];           /**< 容器IP地址>*/
    OOP_VISIBLEVAR_T        imagename;              /**< 容器镜像名称>*/
    OOP_DATETIME_S_T        createtime;             /**< 容器创建时间>*/
    OOP_DATETIME_S_T        starttime;              /**< 容器启动时间>*/
    uint32                  runtime;                /**< 容器运行时间>*/
    OOP_CONTAINER_CONINFO_T con_config;
}OOP_CONTAINER_T;


/** @brief 应用文件数字签证信息 */
typedef struct tag_OOP_APPFILE_SIGN
{
    OOP_VISIBLEVAR_T    name;
    OOP_VISIBLEVAR256_T path;
    uint32              size;
    //OOP_OCTETVAR64_T    md5;
    OOP_FILECHECK_T    checkcode;  //使用24规范文件中要求的数据类型
}OOP_APPFILE_SIGN_T;


/** @brief 应用文件信息 */
typedef struct tag_OOP_APPFILE_INFO
{
    OOP_VISIBLEVAR_T    name;
    OOP_VISIBLEVAR_T    type;
    OOP_VISIBLEVAR256_T path;
    uint32              size;
    //OOP_OCTETVAR64_T    md5;
    OOP_FILECHECK_T    checkcode;  //使用24规范文件中要求的数据类型
    OOP_APPFILE_SIGN_T  sign;
}OOP_APPFILE_INFO_T;

/** @brief 应用配置信息 */
typedef struct tag_OOP_APPCFG_INFO
{
    uint8                cpus;
    uint16               cpulmt;
    uint16               memory;
    uint16               memlmt;
}OOP_APPCFG_INFO_T;

/** @brief app-info   */
typedef struct
{
    OOP_VISIBLEVAR_T        appversion;
    OOP_APPFILE_INFO_T      fileinfo;
    OOP_APPCFG_INFO_T       appconfig;
}OOP_CONTAINER_APPINFO_T;

/** @brief sys_update   */
typedef struct
{
    OOP_VISIBLEVAR_T        sysVersion;
    uint8      				updateType;
    OOP_APPFILE_INFO_T      fileinfo;
}OOP_SYS_UPDATE_T;


/** @brief 容器镜像   */
typedef struct
{
   OOP_VISIBLEVAR_T    imagename;
   OOP_VISIBLEVAR_T    imagepath;
}OOP_CONTAINER_IMAGE_T;

/** @brief 容器配置参数   */
typedef struct tag_OOP_CONTAINER_PARAM
{
  //  uint32                  tasknum;                /**< 任务编号>*/
    OOP_VISIBLEVAR_T        containername;          /**< 容器名称>*/
    OOP_VISIBLEVAR_T        appname;                /**< 应用名称>*/
    OOP_CONTAINER_IMAGE_T   image_t;              /**< 容器镜像>*/
    OOP_CONTAINER_CONINFO_T con_config;             /**<容器配置参数>*/
    OOP_CONTAINER_APPINFO_T app_info;
}OOP_CONTAINER_PARAM_T;

/*修改配置*/
typedef struct
{
    OOP_VISIBLEVAR_T con_name;
    OOP_CONTAINER_CONINFO_T con_config;
}OOP_MODIFY_CONFIG_T;

/** @brief 应用服务 */
typedef struct tag_OOP_APP_SERVICE_INFO
{
    uint8               serviceindex;
    OOP_VISIBLEVAR_T    servicename;
    bool                serviceenable;
    uint8               servicestate;
    uint16              cpulimit;
    uint16              cpuusage;
    uint16              memusage;
    OOP_DATETIME_S_T    starttime;
}OOP_APP_SERVICE_INFO_T;

#define MAX_APP_SERVICE_NUM  10
/** @brief 应用单元 */
typedef struct tag_OOP_APP_INFO
{
    uint8               appindex;
    OOP_VISIBLEVAR_T    appname;
    OOP_VISIBLEVAR_T    containername;
    OOP_VISIBLEVAR_T    appversion;
    OOP_VISIBLEVAR_T    apphash;
    uint8               appservicenum;
    OOP_APP_SERVICE_INFO_T  appservicelist[MAX_APP_SERVICE_NUM];

    OOP_APPCFG_INFO_T   appcfg;
}OOP_APP_INFO_T;


/** @brief 应用安装单元 */
typedef struct tag_OOP_APP_INSTALL
{
    OOP_VISIBLEVAR_T        containername;
    OOP_VISIBLEVAR_T        appname;
    OOP_CONTAINER_APPINFO_T appinfo;
}OOP_APP_IINSTALL_T;

/** @brief 应用安装单元 */
typedef struct tag_OOP_APP_LICENSE
{
    OOP_VISIBLEVAR_T        appno;
    OOP_VISIBLEVAR_T        appname;
    OOP_OCTETVAR64_T        applicense;
}OOP_APP_LICENSE_T;

typedef struct tag_OOP_APP_LICENSE_A
{
    uint8 num;
    OOP_APP_LICENSE_T AppLicen[24];
}OOP_APP_LICENSE_ALL_T;

/**
********************************************************
* A.14 有序充电相关数据
********************************************************
*/
#define MAX_PILE_STATE_ORDER                2
#define MAX_PILE_CONNECT_STATE_ORDER        2

/** @brief 枪序号    */
typedef enum tag_GUN_INDEX
{
    GUN_S   = 0,                        // 单枪
    GUN_A   = 1,                        // 双枪A
    GUN_B   = 2                         // 双枪B
}GUN_INDEX_E;

/** @brief 执行状态 */
typedef enum tag_ACTION_STATUS
{
    ActionNormal   = 1,
    ActionEnd      = 2,
    ActionFail     = 3
}ACTION_STATUS_E;

/** @brief 充电桩状态  */
typedef enum tag_PILE_STATUS
{
    HOLDING   = 1,
    WORKING   = 2,
    BROKEN    = 3
}PILE_STATUS_E;

/** @brief 能源路由器工作状态单元*/
typedef struct tag_PILE_WORK_STATUS
{
    uint8               applicationNumber_len;  /*充电申请单号长度*/
    uint8               applicationNumber[16];  /*单号申请单号*/
    uint8               routingAssets_len;      /*路由器资产编号长度*/
    uint8               routingAssets[22];      /*路由器资产编号*/
    GUN_INDEX_E         charge_gun_index;       /*枪序号*/
    ACTION_STATUS_E     actionState;            /*执行状态*/
    OOP_ENERGY_T        bottomEn;               /*电能示值底值*/
    OOP_ENERGY_T        currentEn;              /*当前电能示值*/
    OOP_ENERGY_T        quantityEn;             /*已充电量*/
    uint32              haveChargedTime;        /*已充时间*/
    int32               planChargePower;        /*计划充电功率*/
    int32               curChargePower;         /*当前充电功率*/
    OOP_DWORD3V_T       volt;                   /*电压*/
    OOP_INT4V_T         curr;                   /*电流*/
    PILE_STATUS_E       pileState;              /*充电桩状态*/
    uint8               userid_len;             /*用户id长度*/
    uint8               userid[64];             /*用户id*/    
    uint8               bak;
}PILE_WORK_STATUS_T;

/** @brief OAD:9003 0200 能源路由器工作状态数据项*/
typedef struct tag_OOP_PILE_WORK_STATUS
{   
    uint32              nNum;                       
    uint32              rsv;                        
    PILE_WORK_STATUS_T  item[MAX_PILE_STATE_ORDER]; 
}OOP_PILE_WORK_STATUS_T;

 /** @brief 充电服务单元*/
typedef struct tag_CHARGE_SERVICE
{
    uint8   gunState;
    uint8   bak[3];     
}CHARGE_SERVICE_T;

/** @brief 充电服务数据项*/
typedef struct tag_OOP_CHARGE_SERVICE
{   
    uint32              nNum;                       
    CHARGE_SERVICE_T    item[MAX_PILE_CONNECT_STATE_ORDER]; 
}OOP_CHARGE_SERVICE_T;

/** @brief 互感器信息*/
typedef struct tag_OOP_CTVER_INFO
{
    OOP_VISIBLEVAR_T    szType;     /**< 型号   */
    OOP_OCTETVAR64_T    szId;       /**< 设备ID  */
    OOP_VISIBLESTR4_T   szFactory;  /**< 厂商代码        */
    OOP_VISIBLESTR4_T   szSoftVer;  /**< 软件版本号       */
    OOP_VISIBLESTR6_T   szSoftDate; /**< 软件版本日期     */
    OOP_VISIBLESTR4_T   szHardVer;  /**< 硬件版本号       */
    OOP_VISIBLESTR6_T   szHardDate; /**< 硬件版本日期     */
    OOP_VISIBLESTR8_T   szExtend;   /**< 厂家扩展信息     */
}OOP_CTVER_INFO_T;

/** @brief 互感器信息40400500*/
typedef struct tag_OOP_CT_VERTIONS
{
    uint32              nNum;
    OOP_CTVER_INFO_T    ver[4]; /**< 0-核心板 1-A相CT 2-B相CT 3-C相CT */
}OOP_CT_VERTIONS_T;

#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
/** @brief 4320线损*/
typedef struct tag_OOP_Line_Loss
{
    int32       linelossvalue;
    int16       linelossrate;
}OOP_Line_Loss_T;

typedef struct tag_OOP__Switch_Hindrance
{
    OOP_TSA_T             addr;
    int32                 lineloss;
    OOP_DATETIME_S_T      beforchangetime;
    OOP_DATETIME_S_T      afterchangetime;
    int32                 Current;
}Switch_Hindrance_T; 

/** @brief 4320回路阻抗*/
typedef struct tag_OOP__Switch_Impedance
{
    uint32 num;
    Switch_Hindrance_T switchhindrance[33];
}OOP_Switch_Impedance_T;
#endif

/*
*********************************************************************
* 厂家自定义参数结构体
*********************************************************************
*/
/** @brief APP_ID */
typedef struct tag_INNER_APP_ID
{
    OOP_VISIBLEVAR_T    appName;              /**< 组件名称>*/
    OOP_OCTETVAR256_T   appID;                /**< APP_ID>*/
}OOP_INNER_APP_ID_T;

/** @brief ARRAY APP_ID */
typedef struct tag_INNER_APP_ID_LIST
{
    uint32             nNum;
    uint32             rsv;
    OOP_INNER_APP_ID_T innerAppID[64];      
}OOP_INNER_APP_ID_LIST_T;
typedef struct tag_Circuit_Breaker_Identify_Result
{
    OOP_DATETIME_S_T             Suctime;    //识别成功时间
    //uint8     signalstatus            ;       
    uint8     signalphase               ;   //识别信号相位enum{未知（0） A 相（1）B 相（2）C 相（3）三相（4）
    int32     Signalstrength1           ;   //识别信号强度1
    int32     Signalstrength2           ;   //识别信号强度2
    //int32     BaseStrength            ;
    int32     BackgroundNoise           ;   //背景噪声
    OOP_BITSTR16_T              Signatureinformation;   //特征码信息
    OOP_OCTETVAR16_T    Extendedinformation;
}Circuit_Breaker_Identify_Result;

typedef struct tag_Oop_Identify_Result
{
    
    uint8     Identifynum;
    Circuit_Breaker_Identify_Result  IdentifyInformation[4];
}Oop_Identify_Result;

typedef struct tag_OOP_IdentifyResult
{
    uint32              nIndex;
    OOP_DATETIME_S_T    tmStart;
    OOP_DATETIME_S_T    tmEnd;
    OOP_REPORTS_T       evtState;
    OOP_EVTSOURCE_T     source;
    OOP_EVTBUF_T        data;
    Oop_Identify_Result IdentifyInformations;
}OOP_IdentifyResult_T;


//3342 保护功能投退事件单元
typedef struct tag_OOP_PF_INPUT_EXIT
{
    uint32                 nIndex;     //事件记录序号
    OOP_DATETIME_S_T       tmStart;    //事件发生时间
    OOP_DATETIME_S_T       tmEnd;      //事件结束时间
    OOP_REPORTS_T          evtState;   //事件上报状态
    OOP_EVTSOURCE_T        source;     //事件发生源
    uint8                  actCase;    //动作原因
    OOP_VARIANT_T          constBef;   //上一次投入使用的定值
    OOP_VARIANT_T          constCur;   //当前要投入使用的定值
    uint8                  prodroType; //投退方式
    OOP_EVTBUF_T           data;       //关联对象数据
}OOP_PF_INPUT_EXIT_T;

/** @brief 3140 安全变更事件参数 */
typedef struct tag_OOP_EVTCFG_3140
{
    uint32          flowLimit;        //月累计流量限值
    uint32          speedLimit;       //流速限值
    uint32          sshLTNoClose;     //SSH服务长时间未关闭判断时间
}OOP_EVTCFG_3140_T;

//#ifdef AREA_HUNAN
#define OOP_MAX_TOPONODE    32
//60F2一个节点拓扑信息
typedef struct tag_OOP_NodeTopologyInfo1
{
    OOP_TSA_T   NodeDevAddr;        //本节点设备地址
    uint8       NodePhaseInfo;      //本节点相位信息enum{未知（0），A（1），B（2），C（3），三相（FF）}
    uint8       DevType;      //本节点相位信息enum{未知（0），A（1），B（2），C（3），三相（FF）}
    OOP_TSA_T   ParentNodeDevAddr;  //父节点设备地址
    OOP_TSA_T   ParentNodeLineAddr; //父节点线路计量地址
}OOP_NodeTopologyInfo1_T;

typedef struct tag_OOP_NodeTopologyInfo
{
    OOP_TSA_T   NodeDevAddr;        //本节点设备地址
    uint8       NodePhaseInfo;      //本节点相位信息enum{未知（0），A（1），B（2），C（3），三相（FF）}
    OOP_TSA_T   ParentNodeDevAddr;  //父节点设备地址
    OOP_TSA_T   ParentNodeLineAddr; //父节点线路计量地址
}OOP_NodeTopologyInfo_T;


//60F1 节点拓扑信息
typedef struct tag_OOP_NodeTopology
{
    uint32     nNum;      //
    uint32     rsv;
    OOP_NodeTopologyInfo_T item[OOP_MAX_TOPONODE];  //一个节点拓扑信息
}OOP_NodeTopology_T;

//环境温度告警事件 属性6 33610600
typedef struct tag_OOP_TemperatureWarningParam
{
    int16   LowerLimit;     //环境告警下限
    int16   UpperLimit;     //环境告警上限

}OOP_TemperatureWarningParam_T;

////线路阻抗越限事件单元 33F2
//typedef struct tag_OOP_IMPE_OVER
//{
//    uint32                 nIndex;   //事件记录序号
//    OOP_DATETIME_S_T       tmStart;  //事件发生时间
//    OOP_DATETIME_S_T       tmEnd;    //事件结束时间
//    OOP_REPORTS_T          evtState; //事件上报状态
//    OOP_EVTSOURCE_T        source;   //事件发生源
//    uint32                 impeBef;  //发生前阻抗
//    uint32                 impeAft;  //发生后阻抗
//    uint8                  curPhase; //发生时相位信息enum{未知（0），A（1），B（2），C（3），三相（FF）}
//    OOP_EVTBUF_T           data;     //关联对象数据
//}OOP_IMPE_OVER_T;

//线路阻抗越限事件单元 33F2
typedef struct tag_OOP_IMPE_OVER
{
    uint32                 nIndex;   //事件记录序号
    OOP_DATETIME_S_T       tmStart;  //事件发生时间
    OOP_DATETIME_S_T       tmEnd;    //事件结束时间
    OOP_REPORTS_T          evtState; //事件上报状态
    OOP_EVTSOURCE_T        source;   //事件发生源
    OOP_DWORD3V_T          impeBef;  //发生前阻抗
    OOP_DWORD3V_T          impeAft;  //发生后阻抗
    uint8                  curPhase; //发生时相位信息enum{未知（0），A（1），B（2），C（3），三相（FF）}
    OOP_EVTBUF_T           data;     //关联对象数据
}OOP_IMPE_OVER_T;

//线路阻抗越限事件配置参数 31F2
typedef struct tag_OOP_EVTCFG_31F2
{
    uint32                 impeLimit;   //阻抗限值
}OOP_EVTCFG_31F2_T;

//#endif
//线路阻抗越限事件配置参数 31F2
typedef struct tag_PROTECTOR_31BB

{
   uint8 faultreason;//故障原因
   uint8 faultphase;//故障相别；
   OOP_DATETIME_S_T       tmStart;  //事件发生时间
   uint16    residualcurrent;//剩余电流
   uint16   voltage[3];//三相电压
   int32  current[3];//三相电流
}OOP_PROTECTOR_31BB_T;

#ifdef AREA_HUNAN
//湖南传感器配置 端口和地址
typedef struct tag_Sensor_Param
{
    OOP_OAD_U port;
    OOP_OCTETVAR16_T addr;  //温湿度 烟雾 水浸地址都一个字节 变压器桩头温度6个字节 统一都用一个结构体 应用里自己判断长度
}OOP_SENSOR_PARAM_T;

#define MAX_DATA_ID_COUNT  20 
//一个数据项配置单元
typedef struct tag_Data_Id_Param
{
    uint32 dataid;
    uint8 datalen;
}OOP_DATA_ID_PARAM_T;

//规约曲线存储数据项
typedef struct tag_Data_Id_Store_Tab
{
    uint8 prtl; //通信协议类型 enum （DL/T645-2007（2），DL/T698.45（3））
    //uint8 metertype;    //表计类型 enum = 0（单相表（0），三相表（1））
    uint8 singledatanum;      //单相表数据项数量 
    uint8 singledatasize;       //单相表预计回复总长度
    OOP_DATA_ID_PARAM_T singledataparam[MAX_DATA_ID_COUNT]; //单相表数据项
    uint8 threedatanum;      //三相表数据项数量 
    uint8 threedatasize;
    OOP_DATA_ID_PARAM_T threedataparam[MAX_DATA_ID_COUNT];
}OOP_DATA_ID_STORE_TAB_T;

//属性 101 HPLC-CCO 模块曲线存储数据项 F2096500
typedef struct tag_Module_Curve_Store_Data
{
    BOOLEAN flag;
    uint8  period;  //采集周期 分钟
    OOP_DATA_ID_STORE_TAB_T dataidstoretab645; //DL/T645-2007 规约曲线存储数据项
    OOP_DATA_ID_STORE_TAB_T dataidstoretab698;//DL/T698.45 规约曲线存储数据项
}OOP_MODULE_CURVE_STORE_DATA_T;

typedef struct tag_OOP_To_Data_Id_Param
{
    OOP_OAD_U oad;
    uint32 dataid;
    uint8 datalen;
}OOP_TO_DATA_ID_PARAM_T;

#endif
#ifdef AREA_SHANDONG
#define MAX_HARMONIC_VALUE_NUM   50  //谐波次数
#else
#define MAX_HARMONIC_VALUE_NUM   21  //谐波次数
#endif

#ifdef AREA_SHANDONG
//2E20 山东温湿度单元
typedef struct tag_HUMITURE_2E20
{
    int32   temp;       //温度
    uint32  humidity;   //湿度
}OOP_HUMITURE_2E20_T;

//2E21 山东门锁状态单元
typedef struct tag_LOCK_STATE_2E21
{
    uint8    door_state; //门状态
    uint8    lock_state; //锁状态
}OOP_LOCK_STATE_2E21_T;
#endif
//2046 谐波电压
typedef struct
{
    uint32          nNum;
    uint64          rsv;
    uint32           nValue[MAX_HARMONIC_VALUE_NUM];
}OOP_HARMONIC_VOLTAGE_T;
//2047 谐波电流 2048谐波有功功率
typedef struct
{
    uint32          nNum;
    uint64          rsv;
    int32           nValue[MAX_HARMONIC_VALUE_NUM];
}OOP_HARMONIC_CUR_POWER_T;


typedef struct
{
    int32           positivepower;
    int32           negativepower;
}OOP_HARMONIC_ACTIVE_POWER_T;


//断路器运行状态字
typedef struct tag_BREAK_STATUS
{
    uint8 status1;
    uint8 status2;
}OOP_BREAK_STATUS_T;

typedef struct tag_BREAK_RUN_STATUS
{
    uint32          nNum;   
    uint32          rsv;
    OOP_BREAK_STATUS_T     status[8];
}OOP_BREAK_RUN_STATUS_T;

#ifdef AREA_SHAANXI
//3399 ICCID 变更事件记录单元
typedef struct tag__ICCID_CHANGE_EVENT
{
    uint32				   nIndex;	   //事件记录序号
    OOP_DATETIME_S_T	   tmStart;    //事件发生时间
    OOP_DATETIME_S_T	   tmEnd;	   //事件结束时间
    OOP_REPORTS_T		   evtState;   //事件上报状态
    OOP_EVTSOURCE_T 	   source;	   //事件发生源
    OOP_VISIBLEVAR32_T	   iccidBef;   //事件变更前 ICCID 
    OOP_VISIBLEVAR32_T	   iccidCur;   //事件变更后 ICCID
    OOP_EVTBUF_T		   data;	   //关联对象数据
}OOP_ICCID_CHANGE_EVENT_T;

/** @brief 4G监测信息 */
typedef struct tag_OOP_4GMONITOR_INFO
{
    int 			rsrp;					/**< 接收功率			*/
    int 			rsrq;					/**< 接收质量			*/
    int 			simr;					/**< 信噪比			*/
    uint32			arfcn;					/**< 当前频点	*/
}OOP_4GMONITOR_INFO_T;

#endif
typedef struct tag_Water_Temperature_T
{
    int32              supplytmp;
    int32              flowtmp;
}Water_Temperature_T;

//
// F400-日志管理
//
typedef struct tag_LOG_INFO
{
    OOP_VISIBLEVAR128_T logpathname;
    uint32              logsize;
    //OOP_OCTETVAR64_T    logmd5;
    OOP_FILECHECK_T     logcheck; //24规范文件校验的结构体变更
    OOP_DATETIME_S_T    creattime;
    uint8               logtype;
    OOP_VISIBLEVAR128_T conname;
    OOP_VISIBLEVAR128_T appname;
}OOP_LOG_INFO_T;
#define OOP_MAX_LOGS	(32)

typedef struct tag_OOP_LOG_UNIT
{
    OOP_VISIBLEVAR_T	logFile;
    uint32				logSize;
    OOP_OCTETVAR64_T	logMD5;
    OOP_DATETIME_S_T	tmCreate;
    uint8				type;
    OOP_VISIBLEVAR_T	containerName;
    OOP_VISIBLEVAR_T	appName;
} OOP_LOG_UNIT_T;

typedef struct tag_OOPM_LOG_PACKAGE
{
    OOP_VISIBLEVAR_T	logSrce;
    OOP_VISIBLEVAR_T	logDest;
} OOPM_LOG_PACKAGE_T;

/** @brief APPID配置	 */
typedef struct tag_OOPM_APPID_CFG
{
    OOP_VISIBLEVAR256_T name;	/**< APP英文名 */
    OOP_OCTETVAR256_T   id;		/**< APPID     */
}OOPM_APPID_CFG_T;

	/** @brief 扩展oad 记录任务失败表计集合 FF300A00 */
#define MAX_FAIL_ADDR_NUM   500 
typedef struct 
{
	uint16 taskid;	//透明任务方案号
	uint16 failNum; //失败的表个数
	uint8  failAddr[MAX_FAIL_ADDR_NUM][6];	//失败的表地址数组
}OOP_TASK_FAIL_TSA_LIST_T;

/** @brief 扩展oad 记录任务失败表计集合 FF300B00 */
typedef struct tag_OOP_TASK_FAIL_NUM_LIST_T
{
    uint16 FailNum; //本轮失败的表个数
    uint16 Pn[500];	//失败的表测量点
    uint8  FailAddr[500][6];	//失败的表地址数组
}OOP_TASK_FAIL_NUM_LIST_T;

typedef struct
{
    int upValue; //上键值按次数
    int dwValue; //下键值按次数
}KEYPRESS_FIR;

typedef struct
{
    int16 fieldStrength;        //场强
    int16 snr;              //信噪比
}OOP_FS_SNR_T; //场强及信噪比

typedef struct
{
    uint16 maxSig; //场强信号最大值
    uint16 minSig; //场强信号最小值
    OOP_DATETIME_S_T  minTime; //场强信号最小值时间
    OOP_DATETIME_S_T  maxTime; //场强信号最大值时间
}GPRS_DATA_INFO;

//扩充的FFF00700软件版本和发布日期
typedef struct
{
    OOP_VISIBLESTR4_T   szSoftVer;  /**< 软件版本号            */
    OOP_VISIBLESTR6_T   szSoftDate; /**< 软件版本日期     */
}OOP_INNER_SOFT_VERSION_T;
#ifdef __cplusplus
}
#endif

#endif // __OOP_TYPE_H__
