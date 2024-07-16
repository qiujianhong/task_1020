/*
*********************************************************************
* Copyright(C) 2022 China Electric Power Research Institute
* All rights reserved. 
* @brief：   面向对象数据类型定义(福建)
* @date：    2022/03/17
* @history： 
*
*********************************************************************
*/
#ifndef __OOP_TYPE_FUJIAN_H__
#define __OOP_TYPE_FUJIAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 代理类型枚举 */
typedef enum tag_OOP_PROXY_TYPE
{
    OOP_PROXY_GET_LIST         = 1,        /**< 代理读取若干个服务器的若干个对象属性                                */
    OOP_PROXY_GET_RECORD       = 2,        /**< 代理读取一个服务器的一个记录型对象属性                               */
    OOP_PROXY_SET_LIST         = 3,        /**< 代理设置若干个服务器的若干个对象属性                                */
    OOP_PROXY_SET_GET_LIST     = 4,        /**< 代理设置后读取若干个服务器的若干个对象属性                             */
    OOP_PROXY_ACT_LIST         = 5,        /**< 代理操作若干个服务器的若干个对象方法                                */
    OOP_PROXY_ACT_GET_LIST     = 6,        /**< 代理设置后读取若干个服务器的若干个对象属性                             */
    OOP_PROXY_TRANS            = 7,        /**< 代理透明转发命令                                          */
    OOP_PROXY_INNER_TRANS      = 8,        /**< 代理终端内部转发命令                                        */
    OOP_PROXY_SMART_TRANS      = 50        /**< 代理智能透明转发命令(福建)                                    */
}OOP_PROXY_TYPE_E;

/** @brief 报文格式字类型   */
typedef enum tag_OOP_MESS_FORMAT_Type
{
    OOP_MESS_FORMAT_TRANS           = 0,     /**< 透明传输                                */
    OOP_MESS_FORMAT_645             = 0x10,  /**< 标准的645报文                            */
    OOP_MESS_FORMAT_645_TIME        = 0x11,  /**< 含时间格式为YYMMDDhhmm的标准的645报文                */
    OOP_MESS_FORMAT_645_TIMES       = 0x12,  /**< 含时间格式为YYMMDDhhmmss的标准的645报文              */
    OOP_MESS_FORMAT_645_CLD         = 0x13,  /**< 只含控制码、数据域长度、数据域的645报文                    */
    OOP_MESS_FORMAT_645_CLD_TIME    = 0x14,  /**< 只含控制码、数据域长度、数据域（时间格式为YYMMDDhhmm）的645报文                             */
    OOP_MESS_FORMAT_645_CLD_TIMES   = 0x15,  /**< 只含控制码、数据域长度、数据域（时间格式为YYMMDDhhmmss）的645报文                           */
    OOP_MESS_FORMAT_3761            = 0x20,  /**< 标准376.1报文                           */
    OOP_MESS_FORMAT_3761_SIMPLE     = 0x21,  /**< 简要376.1报文                           */
    OOP_MESS_FORMAT_3761_SIMPLE_TDC = 0x22,  /**< 带Td_c时标修正的376.1简要报文                 */
    OOP_MESS_FORMAT_3761_SIMPLE_TDD = 0x23,  /**< 带Td_d时标修正的376.1简要报文                 */
    OOP_MESS_FORMAT_3761_SIMPLE_TDM = 0x24,  /**< 带Td_m时标修正的376.1简要报文                 */
    OOP_MESS_FORMAT_3762            = 0x30,  /**< 标准376.2报文                           */
    OOP_MESS_FORMAT_3762_SIMPLE     = 0x31,  /**< 简要376.2报文                           */
    OOP_MESS_FORMAT_3763            = 0x40,  /**< 标准376.3报文                           */
    OOP_MESS_FORMAT_698             = 0x50,  /**< 标准698.45报文                          */
    OOP_MESS_FORMAT_698_APDU        = 0x51,  /**< 698.45APDU域的报文                      */
    OOP_MESS_FORMAT_698_APDU_AMEND  = 0x52,  /**< 需要进行时间或随机数修正的698.45APDU域的报文                   */
    OOP_MESS_FORMAT_698_AMEND       = 0x53   /**< 需要进行时间或随机数修正的698.45的报文                        */
}OOP_MESS_FORMAT_E;

/** @brief 福建错误代码类型   */
typedef enum tag_OOP_FJ_ERR_Type
{
    OOP_FJ_ERR_SUCCESS          = 0,     /**< 正确                */
    OOP_FJ_ERR_METER_ADD_SAME   = 2,     /**< 表地址重复             */
    OOP_FJ_ERR_MESS_TOO_LONG    = 3,     /**< 报文超长              */
    OOP_FJ_ERR_MESS_CHECK       = 4,     /**< 报文校验错误            */
    OOP_FJ_ERR_NO_SPACE         = 5,     /**< 存储空间不足            */
    OOP_FJ_ERR_ADD_CHECK        = 6,     /**< 地址校验失败            */
    OOP_FJ_ERR_DATA_SIGN_CHECK  = 7,     /**< 数据签名校验错误          */
    OOP_FJ_ERR_CIPHERTEXT_CHECK = 8,     /**< 密文校验错误            */
    OOP_FJ_ERR_MAC_CHECK        = 9,     /**< 对称MAC验证失败         */
    OOP_FJ_ERR_PROTECT_POWER    = 10,    /**< 处于保电状态            */
    OOP_FJ_ERR_RESQ_TIME_OUT    = 11,    /**< 响应超时              */
    OOP_FJ_ERR_TASK_NO_EXECUTE  = 12,    /**< 任务未执行             */
    OOP_FJ_ERR_NO_TASK          = 13,    /**< 任务未配置             */
    OOP_FJ_ERR_NO_RESP          = 14,    /**< 运用层无响应            */
    OOP_FJ_ERR_BUSY             = 15,    /**< 信道忙               */
    OOP_FJ_ERR_MODULE_EXCEPTION = 16,    /**< 通信模块异常            */
    OOP_FJ_OTHER                = 255    /**< 其他错误              */
}OOP_FJ_ERR_E;

/** @brief 外部设备数据采集模式   */
typedef enum tag_OOP_GATHER_Type
{
    OOP_GATHER_GW               = 0,     /**< 国网协议              */
    OOP_GATHER_FJ               = 1      /**< 福建增补              */
}OOP_GATHER_E;

/** @brief 加密芯片类型   */
typedef enum tag_OOP_ESAM_Type
{
    OOP_ESAM_2013               = 0,     /**< 2013版              */
    OOP_ESAM_698_45             = 1      /**< 698.45版            */
}OOP_ESAM_E;

/** @brief 协议封拆包方式   */
typedef enum tag_OOP_Pack_Type
{
    OOP_PACK_TRANS_ALL          = 0,     /**< 全透明转发                      */
    OOP_ESAM_TRANS_GW_3762      = 1,     /**< 376.2国网协议透明转发模式            */
    OOP_ESAM_TRANS_FJ_3762      = 2,     /**< 376.2国网协议透明转发模式            */
    OOP_ESAM_NOTICE_FJ_3762     = 3,     /**< 376.2福建增补协议V1.2任务预告抄表(广播)方式            */
    OOP_ESAM_PARALLEL_GW_3762   = 4,     /**< 376.2 国网并发抄表方式              */
    OOP_ESAM_SEARCH_FJ_3762     = 8,     /**< 376.2 福建增补协议V1.2搜表          */
    OOP_ESAM_TRANS_SEARCH       = 9      /**< 透明信道搜表                      */
}OOP_PACK_E;

/** @brief 通信对象类型   */
typedef enum tag_OOP_Comm_Type
{
    OOP_COMM_TYPE_TERMINAL      = 0,     /**< 终端本体                      */
    OOP_COMM_TYPE_CHIP_MODULE   = 1,     /**< 本地通信模块通信芯片模块  */
    OOP_COMM_TYPE_NET_MODULE    = 2,     /**< 本地通信模块网络管理模块（路由模块）            */
    OOP_COMM_TYPE_METER         = 3,     /**< 电能表类设备            */
    OOP_COMM_TYPE_AC            = 4,     /**< 交采              */
    OOP_COMM_TYPE_METER_COMM    = 5,     /**< 电能表通信模块          */
    OOP_COMM_TYPE_COLLECTOR     = 6,     /**< 采集器本体模块                   */
    OOP_COMM_TYPE_COLLECTOR_COMM= 7,     /**< 采集器通信模块                    */
    OOP_COMM_TYPE_TELECOMM      = 8,     /**< 远程通信模块              */
    OOP_COMM_TYPE_WATER_METER   = 9,     /**< 水表            */
    OOP_COMM_TYPE_GAS_METER     = 10,    /**< 气表            */
    OOP_COMM_TYPE_HEAT_METER    = 11     /**< 热表              */
}OOP_COMM_E;


/** @brief  停电工作电池状态 4E020200         */
typedef struct tag_OOP_Battery_status
{
    uint8   capacity;                   /**< 电池有效容量 (有效范围0~100)             */
    uint8   res[1];
    uint16  voltage;                    /**< 电池电压（单位：V, 换算 -2）              */
}OOP_BATTERY_STATUS_T;

/** @brief  现场信号信息 4E030200         */
typedef struct tag_OOP_Signal_Info
{
    uint16  lac;                        /**< 基站号              */
    uint16  cell_id;                    /**< 小区号              */
    uint16  arfcn;                      /**< 频点号              */
    int16   rx_level;                   /**< 接收信号强度           */
    int16   power;                      /**< 当前发射功率           */
    uint8   res[2];
}OOP_SINGAL_INFO_T;

/** @brief  端口超时响应时间 6E030200         */
#define OOP_MAX_PORT            8

typedef struct tag_OOP_Port_TI
{
    OOP_OAD_U  oad;                     /**< 端口              */
    OOP_TI_T   ti;                      /**< 响应超时时间          */
}OOP_PORT_TI_T;

typedef struct tag_OOP_Ports_TI
{
    uint32        nNum;                /**< 端口数量              */
    OOP_PORT_TI_T param[OOP_MAX_PORT]; /**< 延时参数              */
}OOP_PORTS_TI_T;

//一组用户地址，原来的结构体无法满足群组档案1500的要求
#define OOP_MAX_MSPARAM_F     (1500)    /**< 电表集合数            */

/** @brief 一组用户地址       */
typedef struct Meter_Set_Add_FJ
{
    uint16      size;
    OOP_TSA_T   add[OOP_MAX_MSPARAM_F];
}OOP_MSAF_T;

/** @brief 电表集合(参数) */
typedef struct Meter_Set_Param_FJ
{
    uint8               choice;     /**< choice = 0(无电能表)               */
                                    /**< choice = 1(所有电能表)                */
    union
    {
        uint16          size;
        OOP_MST_T       mst;        /**< choice = 2(一组用户类型)         */
        OOP_MSAF_T      msa;        /**< choice = 3(一组用户地址)         */
        OOP_MSI_T       msi;        /**< choice = 4(一组用户配置序号)       */
        OOP_MSRT_T      msrt;       /**< choice = 5(一组用户类型区间)       */
        OOP_MSRA_T      msra;       /**< choice = 6(一组用户地址区间)       */
        OOP_MSRI_T      msri;       /**< choice = 7(一组用户序号区间)       */
    };
}OOP_MSPF_T;

/** @brief  群组档案单元(透明具体任务单元, 建议数据中心存储会用tsa最后三位作为信息点号) 6E060200         */
typedef struct tag_OOP_Group_METER
{
    OOP_TSA_T   groupAdd;                /**< 群组地址            */
    OOP_MSPF_T  ms;                      /**< 通信地址集合          */
}OOP_GROUP_METER_T;

/** @brief  群组档案单元按新结构体存入数据中心，福建规范要求每个群组地址最大支持1500个档案，需要
存2个OAD，分别是6E050200和6E05ffff(暂定), 6E050200不够存时，剩余档案存入6E05ffff*/
#define         GROUPMETER_NUM        750

/** @brief 福建群组档案中的表地址      */
typedef struct tag_ADDR
{
    uint8   add[6];
}OOP_ADDR_T;

typedef struct tag_OOP_GROUPMETER_NEW_T
{
    OOP_TSA_T  groupAddr;                      //群组地址
    uint8      choice;                         //ms类型
    uint32     num;                            //关联档案数量
    OOP_ADDR_T addr[GROUPMETER_NUM];           //关联档案
}OOP_GROUPMETER_NEW_T;

/** @brief  群组档案集 6E050200         */
#define OOP_MAX_GROUP_METER             254

typedef struct tag_OOP_Group_METERS
{
    uint32            nNum;                       /**< 群组档案单元数量          */
    OOP_GROUP_METER_T param[OOP_MAX_GROUP_METER]; /**< 群组档案单元参数          */
}OOP_GROUP_METERS_T;

/** @brief  群组地址档案记录 6E050300         */
typedef struct tag_OOP_Group_num
{
    OOP_TSA_T   groupAdd;                /**< 群组地址            */
    uint16      num;                     /**< 数量              */
}OOP_GROUP_NUM_T;

typedef struct tag_OOP_Group_numS
{
    uint32            nNum;                       /**< 群组档案单元数量          */
    OOP_GROUP_NUM_T   num[OOP_MAX_GROUP_METER];   /**< 群组档案数量            */
}OOP_GROUP_NUMS_T;

/** @brief  链路测试 6E087f00         */
typedef struct tag_OOP_LINK_TEST
{
    OOP_OCTETVAR2K_T    msg;                      /**< 下行报文内容	*/
    uint16              resLen;                   /**< 响应报文长度	*/
}OOP_LINK_TEST_T;

/* 条件触发 */
typedef struct tag_OOP_Trigger_condition
{
    uint8             type;                       /**< 触发类型          */
    union
    {
        OOP_BITSTR8_T yx;                         /**< type == 1, 遥信变位       */
        struct{
            uint16            start;              /**< 截取开始       */
            uint16            end;                /**< 截取结束       */
            OOP_OCTETVAR256_T features;           /**< 特征字节       */
        }features;                                /**< type == 4, 特诊匹配       */
    }data;                                        /**< 触发内容          */
    uint8            object;                      /**< 任务对象 0-本方案所有具体任务, 2-本方案相同地址对象的具体任务        */
    uint16           time;                        /**< 触发后任务方案有效时长         */
}OOP_TRIGGER_T;

/* 任务类别 */
typedef struct tag_OOP_TASK_TYPE
{
    uint8                   taskType;       /**< 任务类别                       */
    union
    {
        uint8               broadcast;     /**< tasktype == 1, 广播, 单位秒       */
        uint16              lognum;        /**< tasktype == 4, 通信日志, 最大记录条数            */
        OOP_TRIGGER_T       condition;     /**< tasktype == 5, 条件触发                 */
    }addi_param;                           /**< 附加参数                         */
}OOP_TASK_TYPE_T;

/* 数据加工方式 */
typedef struct tag_OOP_Data_maching
{
    uint8             type;                       /**< 加工方式           */

    /* 附加参数,目前没有 */
    
}OOP_DATA_MACHING_T;

/** @brief  任务配置单元 6E120200         */
typedef struct tag_OOP_TRANS_TASK
{
    uint16              taskid;         /**< 透明任务方案号                    */
    OOP_OAD_U           oad;            /**< 端口                         */

    OOP_TASK_TYPE_T     taskTypeData;   /* 任务类别 */
    
    uint8               priority;       /**< 执行优先级                        */
    uint8               bmonopoly;      /**< 执行独占性 0-不独占, 1-独占            */
    OOP_DATETIME_S_T    startTime;      /**< 开始时间                         */
    OOP_DATETIME_S_T    endTime;        /**< 结束时间                         */
    OOP_TI_T            cycle;          /**< 任务周期                         */
    OOP_TI_T            retryCycle;     /**< 失败重试周期                       */
    uint8               turnRetryNum;   /**< 每轮次失败重试次数                    */
    uint8               turnNum;        /**< 失败允许重试轮次                     */
    uint8               packType;       /**< 协议封拆包方式 OOP_PACK_E           */
    uint8               bRelation;      /**< 多帧关联任务 0-不关联, 1-关联 */
    uint8               commType;       /**< 通信对象类型 OOP_COMM_E            */
    uint16               storeNum;       /**< 数据存储次数                       */
    uint8               reportid;       /**< 上报方案编号                       */
    OOP_DATA_MACHING_T  machtype;       /**< 数据加工方式                       */
    uint16              startScriptID;  /**< 任务开始前脚本ID                    */
    uint16              endScriptID;    /**< 任务结束后脚本ID                    */
}OOP_TRANS_TASK_T;

/** @brief  全局透明任务方案更新                   */
typedef struct tag_OOP_TRANS_update
{
    uint16            transTaskid;       /**< 透明任务方案号                      */
    uint8             priority;          /**< 执行优先级                        */
}OOP_TRANS_UPDATE_T;

/** @brief  报文比对参数                   */
typedef struct tag_OOP_MESS_COMPARE_Param
{
    OOP_OCTETVAR256_T  feature;       /**< 特征字节                      */
    uint16             start;         /**< 截取开始                      */
    uint16             len;           /**< 截取长度                      */
    uint16             taskid;        /**< 触发任务方案号                   */
}OOP_MESSCOM_PARAM_T;


/** @brief 数据比对格式   */
typedef enum tag_OOP_DATAComp_Type
{
    OOP_DATA_COMP_BIG_ENDIAN     = 0,     /**< 通用数据（正序数据排序即高字节在前）                */
    OOP_DATA_COMP_SMALL_ENDIAN   = 1,     /**< 通用数据（逆序数据排序即高字节在后）*/
    OOP_DATA_COMP_BIT            = 2,     /**< 通用bit位判断（截取长度固定为1，对应位与区间1配置进行异或为真则验证通过）            */
    OOP_DATA_COMP_645            = 10,    /**< DL/T645（均按无符号数处理）         */
    OOP_DATA_COMP_645_BIT        = 11,    /**< DL/T645 bit位判断（截取长度固定为1，对应位与区间1配置进行异或为真则验证通过）              */
    OOP_DATA_COMP_698            = 20,    /**< DL/T698.45数据类型（均按无符号数处理）          */
    OOP_DATA_COMP_698_BIT        = 21,    /**< DL/T698.45 bit位判断（截取长度固定为1，对应位与区间1配置进行异或为真则验证通过）                   */
}OOP_DATA_COMP_E;

/** @brief  验证区间                   */
typedef struct tag_OOP_DATA_COMPARE_section
{
    OOP_OCTETVAR256_T  section1;      /**< 验证通过数据区间1                 */
    OOP_OCTETVAR256_T  section2;      /**< 验证通过数据区间2                 */
    uint16             taskid;        /**< 触发任务方案号                   */
}OOP_DATACOMP_SECTION_T;

/** @brief  数据比对参数                   */
typedef struct tag_OOP_DATA_COMPARE_Param
{
    OOP_OCTETVAR256_T  feature;       /**< 特征字节                      */
    uint16             start;         /**< 截取开始                      */
    uint8              len  ;         /**< 截取长度                      */
    uint8              datatype;      /**< 数据格式类型 OOP_DATA_COMP_E         */
    uint8              num;           /**< 验证区间组数量        */
    OOP_DATACOMP_SECTION_T param[6];  /**< 验证区间组         */
}OOP_DATACOMP_PARAM_T;

/** @brief  返回报文处理方式                   */
typedef struct tag_OOP_RECV_Mess_Proc
{
    uint8               type;            /**< 验证方式                       */
    union
    {
                                            /**< type == 0, 不验证（视同验证通过）       */
        OOP_MESSCOM_PARAM_T   messCompare;  /**< type == 1, 报文比对不一致验证;        type == 2, 对数据进行判断后触发相应方案*/
        OOP_DATACOMP_PARAM_T  dataCompare;  /**< type == 3, 数据判断参数            */
                                            /**< type == 4, 不验证（视同验证不通过）       */
    };
}OOP_RECV_MESS_PROC_T;

/** @brief  任务配置单元 6E140200         */
typedef struct tag_OOP_TRANS_TASK_NODE
{
    uint16              transTaskid;    /**< 透明任务方案号                    */
    uint16              nodeid;         /**< 具体任务序号                    */
    OOP_MSC_T           ms;             /**< 通信地址集合          */
    OOP_COMDCB_T        uartParam;      /**< 端口通信控制块          */
    uint8               reqMessType;    /**< 请求报文格式字 OOP_MESS_FORMAT_E          */
    OOP_OCTETVAR2K_T    reqMess;        /**< 请求报文内容                             */
    uint8               resMessType;    /**< 响应报文格式字 OOP_MESS_FORMAT_E          */
    uint16              resMaxLen;      /**< 响应报文预计最长长度                    */
    uint8               resMinLen;      /**< 成功响应报文最短有效长度                  */

    OOP_RECV_MESS_PROC_T resMessProc;   /**< 返回报文验证处理方式                    */
}OOP_TRANS_TASK_NODE_T;

/** @brief  全局透明具体任务删除                   */
typedef struct tag_OOP_TRANS_TASK_NODE_DEL
{
    uint16            transTaskid;       /**< 透明任务方案号         */
    OOP_MSP_T         ms;                /**< 通信地址集合          */
}OOP_TRANS_NODE_DEL_T;

/** @brief  一个透明方案结果响应报文    */
typedef struct tag_OOP_TRANS_RES_MSG
{
    uint8               errCode;        /**< 错误码 OOP_FJ_ERR_E         */
    OOP_OCTETVAR2K_T    resultData;     /**< 返回数据                        */
}OOP_TRANS_RES_MSG;

/** @brief  一个透明方案结果 6E160200         */
typedef struct tag_OOP_TRANS_TASK_RESULT
{
    uint16              transTaskid;    /**< 透明任务方案号                    */
    uint16              nodeid;         /**< 具体任务序号                     */
    OOP_TSA_T           groupAdd;       /**< 群组地址            */
    uint8               commType;       /**< 通信对象类型 OOP_COMM_E          */

    OOP_DATETIME_S_T    storeTime;      /**< 采集存储时标                     */
    OOP_DATETIME_S_T    readTime;       /**< 终端抄表时间                     */
    uint16              storeNO;        /**< 存储序号                       */
    uint8               retryTurn;      /**< 重试轮次                       */
    uint8               result;         /**< 任务结果 0-失败, 1-成功            */
    uint8               resultMessType; /**< 响应报文格式字 OOP_MESS_FORMAT_E   */

    OOP_TRANS_RES_MSG   transMsg;       /**< 响应报文      */
}OOP_TRANS_TASK_RESULT_T;

/** @brief  主动上报方案单元 6E180200         */
typedef struct tag_OOP_TRANS_REPORT
{
    uint8               reportid;       /**< 上报方案编号                     */
    OOP_OADS_T          channel;        /**< 上报通道                       */
    uint8               priority;       /**< 上报优先级                      */
    OOP_TI_T            timeout;        /**< 上报响应超时时间                   */
    uint8               retry;          /**< 最大上报次数                     */
    uint8               resultFlag;     /**< 结果验证标识 0-直接上报,1-验证通过上报,2-验证不通过上报                     */
    uint8               reportTime;     /**< 上报时刻                       */
    REPORT_CONTEXT      data;           /**< 上报内容                       */
}OOP_TRANS_REPORT_T;

/** @brief 透明任务方案采集状态 6E210200 */
typedef struct tag_OOP_TRANS_TASK_RLUESTATE
{
    uint16              transTaskid;    /**< 透明任务方案号                    */
    OOP_DATETIME_S_T    storeTime;      /**< 采集存储时标                     */
    uint16              storenum;       /**< 存储次数                       */
    OOP_DATETIME_S_T    firststartTime; /**< 任务执行开始时间                   */
    OOP_DATETIME_S_T    laststartTime;  /**< 任务最后开始时间                   */
    uint16              paramSum;       /**< 配置总数量                      */
    uint16              success;        /**< 采集成功数量                     */
    uint16              machingSum;     /**< 加工成功数量                     */
    uint16              sendNum;        /**< 已发送报文条数                    */
    uint16              receNum;        /**< 已接收报文条数                    */
}OOP_TRANS_TASK_RLUESTATE_T;

/** @brief 数据比对格式   */
typedef enum tag_OOP_CUR_TASK_E
{
    OOP_TASK_TIMING              = 0,   /**< 定时任务                */
    OOP_TASK_BROADCAST           = 1,   /**< 广播任务                */
    OOP_TASK_SEARCH_METER        = 2,   /**< 搜表任务                */
    OOP_TASK_FREE                = 3,   /**< 空闲任务                */
    OOP_TASK_CONDITION           = 5,   /**< 条件触发任务              */
    OOP_TASK_THROUGH             = 6,   /**< 主站穿透                */
    OOP_TASK_NOTASK              = 7,   /**< 无任务                 */
    OOP_TASK_PORT_ERR            = 255, /**< 端口异常               */
}OOP_CUR_TASK_E;

/** @brief 一个通道采集状态 6E220200 */
typedef struct tag_OOP_CHAN_RLUESTATE
{
    OOP_OAD_U           oad;            /**< 端口                 */
    uint8               taskType;       /**< 当前执行的任务类别 OOP_CUR_TASK_E           */
    uint16              transTaskid;    /**< 当前执行的透明任务方案号              */
    uint8               priority;       /**< 执行优先级              */
}OOP_CHAN_RLUESTATE_T;

/** @brief 通道采集状态集 6E220200 */
typedef struct tag_OOP_CHAN_RLUESTATES
{
    uint32               nNum;                /**< 端口数量              */
    OOP_CHAN_RLUESTATE_T port[OOP_MAX_PORT]; /**< 一个通道采集状态       */
}OOP_CHAN_RLUESTATES_T;

/** @brief 一个采集状态 6E240200 */
typedef struct tag_OOP_TRANS_ACQ_RLUESTATE
{
    OOP_TSA_T           add;            /**< 通信地址                     */
    uint8               commType;       /**< 通信对象类型 OOP_COMM_E        */
    OOP_OAD_U           oad;            /**< 端口                       */
    OOP_DATETIME_S_T    lastSuccTime;   /**< 最后一次采集成功时间               */
    OOP_DATETIME_S_T    lastFailTime;   /**< 最后一次采集失败时间               */
}OOP_TRANS_ACQ_RLUESTATE_T;

/** @brief 存储空间使用情况 6E250200 */
typedef struct tag_OOP_STORE_SIZE
{
    uint32              dataSize;       /**< 数据区大小   , 单位为KB         */
    uint32              dataUsed;       /**< 已经使用大小, 单位为KB           */
    uint32              paramSize;      /**< 参数区大小, 单位为KB            */
    uint32              paramUsed;      /**< 已经使用大小, 单位为KB           */
}OOP_STORE_SIZE_T;

/** @brief 事件记录当前状态 6E310300 */
typedef struct tag_OOP_TRANS_EVENT_STATUS
{
    OOP_DATETIME_S_T    lastRecvTime;   /**< 最近接收事件存储时标               */
    uint16              lastRecvNo;     /**< 最近接收事件存储序号               */
    uint16              recvSum;        /**< 已接收确认报文条数                */
}OOP_TRANS_EVENT_STATUS_T;

/** @brief 透明事件记录单元 6E320200 */
typedef struct tag_OOP_TRANS_EVENT
{
    uint16              curNum;         /**< 当前事件计数器                  */
    uint16              selfStoreNo;    /**< 本事件存储序号                  */
    OOP_DATETIME_S_T    reportTime;     /**< 事件上报时标                   */
    OOP_TSA_T           add;            /**< 通信地址                     */
    uint8               commType;       /**< 通信对象类型 OOP_COMM_E        */
    OOP_OAD_U           oad;            /**< 端口                       */
    uint8               messType;       /**< 报文格式字 OOP_MESS_FORMAT_E       */
    OOP_OCTETVAR2K_T    data;           /**< 事件内容                     */
}OOP_TRANS_EVENT_T;

/** @brief 主动上报事件上报策略 6E330200 */
typedef struct tag_OOP_TRANS_EVENT_REPORT
{
    OOP_OADS_T          channel;        /**< 上报通道                       */
    uint8               priority;       /**< 上报优先级                      */
    OOP_TI_T            timeout;        /**< 上报响应超时时间                   */
    uint8               retry;          /**< 最大上报次数                     */
}OOP_TRANS_EVENT_REPORT_T;

/** @brief 配置统计单元 6E120300 */
typedef struct tag_OOP_CONFIG_STAT
{
    uint16              taskId;         /**< 透明任务方案号                    */
    OOP_OAD_U           port;           /**< 端口                         */
    uint8               priority;       /**< 执行优先级                      */
    uint8               isOnly;         /**< 执行独占性                      */
    uint16              nodeNum;        /**< 已配置具体任务总数量                 */
    uint16              nodeMinId;      /**< 最小具体任务序号                   */
    uint16              nodeMaxId;      /**< 最大具体任务序号                   */
}OOP_CONFIG_STAT_T;


#ifdef __cplusplus
}
#endif

#endif // __OOP_TYPE_FUJIAN_H__
