/*
*********************************************************************
* Copyright(C) 2018 China Electric Power Research Institute
* All rights reserved. 
* @brief：   公共数据类型定义.
* @date：    2019/11/07
* @history： 
*********************************************************************
*/

#ifndef __DATA_COMMON_H__
#define __DATA_COMMON_H__

#include "oopType.h"
#include "oopType_fujian.h"

#ifdef __cplusplus
extern "C" {
#endif 

typedef unsigned long             offset_t;
typedef unsigned long            addr_t;
typedef unsigned short int mtrnum_t; //测量点号数据类型
#define JSOADNUM 45

enum enumInitState
{
    //#if COMM_RETRY == 1    不要删掉，为了移植的时候能够搜索到该标识
    RECV_LOGIN = 17,        //收到登录报文            17
    RECV_HEART,                //发送心跳报文            18
    RECV_QUIT,                //发送退出登录报文    19
    CONNECT_LOGIN,            //发送登录报文            20
    CONNECT_HEART,            //发送心跳报文            21
    CONNECT_QUIT,            //发送退出登录报文    22

    //#if WDASN_RADIO_EN == 1    不要删掉，为了移植的时候能够搜索到该标识
    D_WDASN_SCAN,            //扫描当前信道            23
    D_WDASN_LINK,            //附着申请                24
    D_WDASN_CONNECT,        //连接申请                25
    D_WDASN_SUCCESS,        //连接成功                26
    D_WDASN_A1A1,            //设置行政区码和地址    27
    D_WDASN_START,            //启动连接                28
    D_WDASN_WAIT,            //连接等待                29
    D_WDASN_SET,            //设置登陆次数和间隔    30

    //#if ETH_MODEL == 1        不要删掉，为了移植的时候能够搜索到该标识
    RESTART_MODEL,            //关闭以太模块            31
    START_MODEL,            //启动以太模块            32
    INIT_MODEL,                //初始化以太模块        33
    CONFIG_IP,                //设置终端IP地址        34
    CONFIG_ROUTER,            //设置终端网关            35

    //#if TD_SCDMA == 1        //TD-SCDMA功能
    TD_AT_DSQ_0,            //关闭网络信号分贝值的上报  36
    TD_AT_CREG_0,            //关闭网络上报（可选输入指令）37
    TD_AT_CFUN_5,            //激活USIM卡 AT+CFUN=5,0\r38
    TD_AT_CFUN_1,            //激活协议栈   39
    TD_AT_DCTA,                //开机后自动附着GPRS   40
    TD_AT_COPS,                //搜网指令   41
    /*APP 端输入“AT+COPS=0\r” ，返回“0\r”，(此指令为搜网指令),
    此后LC6311 会返回很多数据，直到返回“+CREG：1”表示开机成功
    “^DACTI:2”表示当前接入技术是TD;
    “+CIEV：2,2”表示信号等级为2 级；
    “+CIEV：8,0”表示短消息存储未满；
    “+CREG：2”表示搜网中；
    “+CREG：1”表示搜网成功并注册成功。
    【说明：“+CREG:n”其中“n”的值可为0[未注册]，1[注册成功]，2[搜网]，3[网络拒绝注册]，4[网络注册状态未知]，5[漫游]，开机后只有当CREG返回的是1 或5 时，才能做LC6311 所支持的业务】；
    鉴于对CREG：4 的协议理解，建议当+CREG：4 时，APP 处理器在UI 显示为可提供服务状态，即等同于+CREG：1或+CREG:5】*/
    TD_AT_CGEQREQ,            //设置PS业务为交互式“U64/D128”42
    TD_AT_ATS0,                //This S-parameter controls the automatic answering feature of the MT.If set to 0, automatic answering is disabled.
    TD_AT_TEST,            //测试命令44
    AT_TD_DDTM_TD,                  /*设置TD模式 45 */
    AT_TD_DDTM_GPRS,                        //设置GPRS模式46
    AT_TD_CHECKDDTM ,             //GSM或TD模式检查  47

    //#if WORK_MODE == 1
    WAIT_CLIENT,            //等待客户端连接

};

/* 以下oop中有重复 */
#if 0
enum enumFailState
{
    //#if WDASN_RADIO_EN == 1    不要删掉，为了移植的时候能够搜索到该标识
    CONNECT_FAIL = 16,        //WDASN 连接失败
    //#if ETH_MODEL == 1        不要删掉，为了移植的时候能够搜索到该标识
    FAIL_INITETH,        //初始化以太模块失败
};
#endif

typedef struct
{
    uint8    type;                //ID类别
    uint16    bDataId;            //源字典ID（字典格式）
}S_CbIDInfo;
#define USER_DEFINED_DATA_FILENAME  (SPDATA_PRIFIX"User_Defined_Data_File")//私有文件名


typedef struct
{
    uint8  TURN_ONE_CTRL        :1;    //轮次1控制输出(0表示继电器输出  1表示继电器不动作)
    uint8  TURN_ONE_REDLIGHT    :1;    //轮次1红灯
    uint8  TURN_ONE_BLUELIGHT   :1;    //轮次1绿灯
    uint8  TURN_TWO_CTRL        :1;    //轮次2控制输出(0表示继电器输出  1表示继电器不动作)
    uint8  TURN_TWO_REDLIGHT    :1;    //轮次2红灯
    uint8  TURN_TWO_BLUELIGHT   :1;    //轮次2绿灯
    uint8  POWER_CTRL_LIGHT     :1;    //功控灯
    uint8  ELE_CTRL_LIGHT       :1;    //电控灯
    uint8  ALARM_OUTPUT         :1;    //告警继电器(0表示继电器输出  1表示继电器不动作)
    uint8  PROTECT_LIGHT        :1;    //保电灯
    uint8  PrioOut              :1;    //预动
    uint8  reserved             :5;    //预留
}INTERFACE_WORD_BITS_TYPE;

typedef struct
{
    uint8  bit0        :1;
    uint8  bit1        :1;
    uint8  bit2        :1;
    uint8  bit3        :1;
    uint8  bit4        :1;
    uint8  bit5        :1;
    uint8  bit6        :1;
    uint8  bit7        :1;
}YM_BITS_TYPE;//遥信位

//#if    FAC_MODE == 1        //出厂模式
typedef struct
{
    uint8 FacMode;    //0x55:出厂模式；0xAA：送检模式
}S_FacMode;

#define FACMODE_FILENAME  (SPDATA_PRIFIX"FacMode")//私有文件名
//#endif

typedef struct {
    char manufacture[4 + 2];            //厂商代号
    char model[8 + 2];                    //模块型号
    char firmare_version[4 + 2];        //软件版本号
    char release_date[6 + 2];            //软件发布日期
    char hw_version[4 + 2];                //硬件版本号
    char hw_release_date[6 + 2];        //硬件发布日期
}MYGMR_Info;

//#if REMOTE_VPN_EN == 1
typedef struct {
    uint8   vpn_enable;         //1-有效；其他-无效
    uint8   vpn_interface;      //VPN拨号的设备：0-eth0,1-eth1
    char    vpn_server[128];    //VPN服务器地址
    char    vpn_user[128];      //VPN用户名
    char    vpn_pwd[128];       //VPN密码
}REMOTE_VPN_INFO;

enum VPN_STEP
{
    VPN_STEP_INIT,      //初始化
    VPN_STEP_SCRIPT,    //配置拨号的脚本
    VPN_STEP_DHCP,      //自动获取IP地址
    VPN_STEP_VPNDIAL,   //VPN拨号
    VPN_STEP_SUCC,      //成功
    VPN_STEP_FAIL       //失败
};

//#endif

//#if HARDWARE_TYPE == 4

/********微功率无线下载电表信息存储文件***************/
//   2字节           下载电表信息总数
//   6字节            第N个从节点地址
//   2字节            第N个从节点信息
//   3字节            第N个从节点版本
#define     Download_NODE_INFO_FILE     "DownLoadNodeInfo"


/********微功率无线在网电表信息存储文件***************/
//   2字节           在网电表信息总数
//   6字节            第N个从节点地址
//   1字节            第N个从节点中继级别
#define     ONLINE_NODE_INFO_FILE        "OnLineNodeInfo"

/********微功率无线组网信息存储文件***************/
//   2字节           在网电表信息总数
//   6字节            第N个从节点地址
//   1字节            第N个从节点中继级别
#define     BUG_NODE_INFO_FILE            "BugNodeInfo"

/********删除载波节点信息文件***************/
//   1字节            从节点数量
//   6字节            第N个从节点地址
#define     PLC_DEL_SLAVE_INFO_FILE     "PlcDelSlaveInfoFile"

/********载波中继路由信息信息文件***************/
//   6字节    BCD     从节点地址
//   1字节    BIN     中继路由个数N
//   1字节    BIN     第1个中继路由中继级别m1
//   6字节    BCD     第1个中继路由第1级中继从节点地址
//   6字节    BCD     第1个中继路由第m1级中继从节点地址
//   ...      ...     ...
//   1字节    BIN     第1个中继路由信任度

//   1字节    BIN     第N个中继路由中继级别m1
//   6字节    BCD     第N个中继路由第1级中继从节点地址
//   6字节    BCD     第N个中继路由第m1级中继从节点地址
//   ...      ...     ...
//   1字节    BIN     第N个中继路由信任度
#define     PLC_TRUNK_ROUTER_INFO_FILE     "PlcTrunkRouterInfoFile"
//#endif

//#ifdef EXTEND_PARA
#define PARA_EXT_RTU_ENUM           10000           //终端扩展参数的起始ID
#define PARA_EXT_MTR_ENUM           11000           //测量点扩展参数的起始ID
#define PARA_AREA_EXT_RTU_ENUM      12000           //各地区终端扩展参数的起始ID
#define PARA_AREA_EXT_MTR_ENUM      13000           //各地区测量点扩展参数的起始ID
//#endif

//#ifdef EXTEND_DATA
//#define RTDATA_EXT_AREA_ENUM        15000           //扩展的实时数据的起始ID
//#endif


/*EEPROM私有外存申请 8K铁电, 私有数据从6K+256开始 */
#define EEPROM_PULSE_CHECK_ADDR     (6144+256)
#define EEPROM_PULSE_CHECK_LEN      4
#define EEPROM_REALTIME_ADDR        (EEPROM_PULSE_CHECK_ADDR+EEPROM_PULSE_CHECK_LEN)/*脉冲采集时间地址*/
#define EEPROM_REALTIME_LEN         8
#define EEPROM_PULSE_ADDR           (EEPROM_REALTIME_ADDR + EEPROM_REALTIME_LEN)  /*脉冲个数地址*/
#define EEPROM_PULSE_LEN            (8 * sizeof(uint32))
#define EEPROM_PERIOD_ADDR          (EEPROM_PULSE_ADDR + EEPROM_PULSE_LEN)        /*脉冲时间地址*/
#define EEPROM_PERIOD_LEN           (8 * sizeof(uint32))


//#if OOPEAM_PRTL_MEM_EN == 1
typedef enum
{
    NORMALPLAN=0,           // 普通采集方案
    EVENTPLAN,              // 事件采集方案
    TRANSPARENTPLAN,        // 透明任务方案
    REPORTPLAN,             // 上报方案
    SCRIPTPLAN,             // 脚本方案
    ENCRYPTPLAN,            //转加密方案
    MONITORPLAN,            // 监控方案
}ePlanType;

// 任务执行状态
typedef enum
{
    undo=0,                 // 未执行（0），
    doing,                  // 执行中（1），
    finish,                 // 已执行（2）
}eTaskState;

typedef struct{
    OOP_OAD_U m_oad;//主OAD
    uint8 s_oadNum;//OAD个数
    OOP_OAD_U *s_oad;//
}ROAD_t;
//#endif


typedef struct{
    uint8 year_h;
    uint8 year_l;
    uint8 month;
    uint8 day;
    uint8 hour;
    uint8 min;
    uint8 sec;
}__attribute__ ((packed))DateTimeBcd_t;

typedef struct{
    uint8 year_h;
    uint8 year_l;
    uint8 month;
    uint8 day;
    uint8 hour;
    uint8 min;
    uint8 sec;
}__attribute__ ((packed))DateTimeHex_t;


typedef struct{
    uint8 buadRate;        //波特率0~10 300bps~115200bps,255 自适应
    uint8 parity;        //校验位0无校验，1奇校验，2偶校验
    uint8 byteBits;        //数据位，5~8 表示5~8位数据位
    uint8 stopBits;     //停止位，1~2 表示1~2位停止位
    uint8 flowCtrl;     //流控0无，1硬件，2软件
}CommDCB_t;

typedef struct{
    uint8   year_h;
    uint8  year_l;
    uint8   month;
    uint8   day_of_month;
    uint8   day_of_week;
    uint8   hour;
    uint8   min;
    uint8   sec;
    uint8  milliseconds_h;
    uint8  milliseconds_l;
}__attribute__ ((packed))Date_Time_t;

enum eDatatype{
    NULL_DATA,                //无数据
    ARRAY,                    //数组 的元素在对象属性或方法描述中定义
    STRUCTURE,                //
    TBOOLEAN,                //
    BIT_STRING,
    DOUBLE_LONG,
    DOUBLE_LONG_UNSIGNED,

    OCTET_STRING = 9,
    VISIBLE_STRING,

    UTF8_STRIGN = 12,
    BCD,
    INTEGER = 15,
    LONG_SIGNED,
    UNSIGNED,
    LONG_UNSIGNED,

    LONG64_SIGNED = 20,
    LONG64_UNSIGNED,
    ENUM,
    FLOAT32,
    FLOAT64,
    DATE_TIME,
    TDATE,
    TIME,
    DATE_TIME_S,
    DATETIMEBCD = 50,
    DATETIMEBCD_H,
    DATETIMEBCD_S,
    DATEBCD,
    TIMEBCD,

    eOI = 80,
    eOAD,
    eROAD,
    eOMD,
    eTI,
    eTSA,
    eMAC,
    eRAND,
    eREGION,
    eSCALER_UNIT,
    eRSD,
    eCSD,
    eMS,
    eSID,
    eSID_MAC,
    eCOMDCB,
    eRCSD,


    e645DATETIME=0xE0, //自定义645记录时间格式，YYMMDD hhmmss
    e645DATETIME_S,    //自定义645记录时间格式，YYMMDD hhmm
    e645COMBCD,           //自定义645压缩BCD方式标识整形的数据
    e645COMBCD_UNSIGNED,//自定义645压缩BCD方式标识无符号整形的数据
    DOUBLE_LONG_CUSTOM,//自定义有符号数负数最高位为1
    LONG64_SIGNED_CUSTOM,
    LONG_SIGNED_CUSTOM,
    INTEGER_CUSTOM,
    eDATA,
    e645DATETIME_M,    //自定义645记录时间格式, YYMMDDHH
    e645DATETIME_W,    //自定义645记录时间格式, YYMMDDWW
    e645DATETIME_WS,   //自定义645记录时间格式, YYMMDDWW hhmmss
    e645COMBCD_UNSIGNED_60,  //需要乘以60后存储的格式(645单位分，OOP单位秒)
    eDatatypeMax = 255,
};
typedef struct
{
  uint16  OadId;        //OADID
  uint16  bDataId;      //源字典ID（字典格式）
  uint8   NumOrLen;     //
  uint8   type;         //ID类别
  uint16  mtrtyp;        //测量点号 终端或交采0:终端，1交采
  int8    scaler;     //数据字典格式的小数点位数
  uint16  DataType;  //目的数据类型
}FrozenOadInfoList_t;
typedef struct
{
    uint16  DataId;         /*--考虑正反向数据根据数据符号判断用--*/
    uint8   SrcDecLen;      /*--源数据小数位数--*/
    uint8   SrcDataLen;     /*--源具体数据字节数--*/
    uint8   SrcDataType;    /*--源数据类型--*/
    uint8   DesDataType;    /*--目的数据类型--*/
    uint8   DesBuf[200];   /*-目的缓冲区-*/
    uint8   type;        //ID类别 RA_DATA ...
    uint8   desDataLen;    //目标数据长度
    uint16    mtrtyp;       //测量点号 终端或交采0:终端，1交采
    int8   scaler;
    uint8   desOffset;//数据偏移地址，用于1个源ID对应多个数据项时使用
}DataFormat_t;

typedef enum
{
    Frozen_0000=0x0000,    //普通数据
    Frozen_5000=0x5000,    //瞬时冻结
    Frozen_5001,        //秒冻结
    Frozen_5002,        //分钟冻结
    Frozen_5003,        //小时冻结
    Frozen_5004,        //日冻结
    Frozen_5005,        //结算日冻结
    Frozen_5006,        //月冻结
    Frozen_5007,        //年冻结
    Frozen_5008,        //时区表切换冻结
    Frozen_5009,        //日时段表切换冻结
    Frozen_500A,        //费率电价切换冻结
    Frozen_500B,        //阶梯切换冻结
    Frozen_5010,        //电压合格率月冻结
    Frozen_5011,        //阶梯结算冻结
}FrozenType_t;

typedef enum{
    SecMod_NULL=0x08,                            //无安全 明文
    SecMod_PlainMAC=0x04,                        //明文+ MAC
    SecMod_Cipher=0x02,                            //密文
    SecMod_CipherMAC=0x01,                        //密文+ MAC
}SecurityMode_t;

typedef union{
    uint16 secMod;
    struct{
        uint16 proxy:4;
        uint16 action:4;
        uint16 set:4;
        uint16 get:4;
    };
}SecModWd_t;//安全模式字

typedef struct _DateFileStruct {
    uint64 time;
    char chFileName[20];
}DateFileStruct;

//#if OOPEAM_PRTL_MEM_EN == 1
typedef struct
{
    uint16 mDTableNum;      // 记录电表测量点整体数据偏移表文件的数量
    uint16 mDTableCurNo;    // 当前偏移表文件的序号
    uint16 oadTableNum;     // 记录OAD数据偏移表文件的数量
    uint16 oadTableCurNo;   // 当前OAD偏移表文件的序号
}FileConfigInfo_t;

typedef struct
{
    mtrnum_t        metSn;                  // 档案序号
    OOP_OAD_U       OOP_OAD_U;                   // OAD
    uint16          sOadDataLen;            // OAD对应的数据长度
    uint16          fixDataRecLen;          // 用于事件采集方案时的事件开头的固定数据
                                            // 如果不想存储可填0
    BOOL            needWriteTimeTag;       // 是否需要写入3个时标
    DateTimeBcd_t   taskStartTime;          // 采集启动时间
    DateTimeBcd_t   taskSuccessTime;        // 采集成功时间
    DateTimeBcd_t   taskStoreTime;          // 采集存储时间
    uint8           *sOadDataBuf;           // OAD对应的数据缓冲区
    uint8           *fixDataRecBuf;         // 用于事件采集方案时的事件开头的固定数据缓冲区
}OadStoreInfo_t;

typedef struct
{
    mtrnum_t        metSn;                  // 档案序号
    OOP_OAD_U       sOad;                   // OAD
    DateTimeBcd_t   queryTime;              // 查询时间
    BOOL            needReadTimeTag;        // 是否需要读取3个时标
    //uint8           *sOadDataBuf;         // OAD对应的数据缓冲区
}OadQueryInfo_t;

typedef struct
{
    DateTimeBcd_t   taskStartTime;          // 采集启动时间
    DateTimeBcd_t   taskSuccessTime;        // 采集成功时间
    DateTimeBcd_t   taskStoreTime;          // 采集存储时间
    uint8           *OutDataBuf;            // 读出输出的数据缓冲区，如果是事件包含了记录的一些固定数据
}ReadOutData_t;

typedef struct
{
    ePlanType       planType;                   // 方案类型-普通、事件等
    OOP_OAD_U       planMainOad;                // 方案的主OAD数据-实时、冻结、事件OAD等,实时数据用0x00000000标识
}PlanTypeInfo_t;

typedef struct{
    uint8 planNo; //方案号    
    OOP_OAD_U sOad; //查询的OAD
    uint32 meterSnDataStartOffset;//测量点偏移位置
    uint32 oadDataOffset;//OAD偏移位置 
    uint16 oadDataLen;//OAD长度
    uint16 fixReclen;//固定记录长度
}MsCsdInfo_t;

typedef struct SelectTimeTagInfo_t_
{
    DateTimeBcd_t   startTime;                    // 起始时间
    DateTimeBcd_t   endTime;                      // 结束时间
    OOP_TI_T            ti;                           // 时间间隔
    BOOL            firstStartTimeFlag;           // 起始点的第一个点的标志
    uint8           theLastNTimes;                // 上第n次
}SelectTimeTagInfo_t;                          // 时标信息

typedef struct _TimeFileStruct {
    unsigned int time;
    char chFileName[10];
}TimeFileStruct;

typedef struct
{
    uint8           curPlanNo;                  // 当前方案号
    PlanTypeInfo_t  planTypeInfo;               // 方案类型信息
}PlanInfo_t;

typedef struct
{
    mtrnum_t        metSn;                  // 档案序号
    OOP_OAD_U            sOad;                   // OAD
}OadReadInfo_t;


//typedef struct
//{
//    uint8           taskId;                 //任务ID       unsigned
//    eTaskState      taskState;              //任务执行状态
//    DateTimeHex_t   taskStartTime;          //任务执行开始时间   date_time_s，
//    DateTimeHex_t   taskEndTime;            //任务执行结束时间   date_time_s，
//    uint16          sumNum;                 //采集总数量         long-unsigned，
//    uint16          successNum;             //采集成功数量       long-unsigned，
//    uint16          sendFrameNum;           //已发送报文条数     long-unsigned，
//    uint16          recvFrameNum;           //已接收报文条数     long-unsigned
//    BOOL            firstfinshed;           //第一次抄表完成标志
//}TaskMonitorUnit_t;
//#endif

//typedef enum{
//    UDTASK_INIT,      //任务监控数据初始化
//    UDTASK_START,      //任务启动
//    UDTASK_END,          //任务结束
//    UDTASK_STATE,      //更新任务状态
//    UDTASK_SUCCESSNUM,//更新任务成功数量+1
//    UDTASK_SENDFRMNUM,//更新已发送报文条数+1
//    UDTASK_RECVFRMNUM,//更新已接收报文条数+1
//    UDTASK_FIRFINSH,  //第一次抄表完成
//}UpdateTaskCmd_t;
typedef struct
{
    uint16 frozenPeriod;                    // 冻结周期
    OOP_OAD_U   sOad;                            // OAD
    uint16 storeDepth;                      // 存储深度

}FrozenIncludeOadInfo_t;

typedef struct
{
    uint8   factory[8];  /**< 厂商代码           */
    uint8   softVer[8];  /**< 软件版本号         */
    uint8   softDate[8]; /**< 软件版本日期       */
    uint8   hardVer[8];  /**< 硬件版本号         */
    uint8   hardDate[8]; /**< 硬件版本日期       */
    uint8   extend[8];   /**< 厂家扩展信息       */
}__attribute__ ((packed)) FACTORY_VER_TMP_T;

typedef struct
{
    uint8   factory[8];  /**< 厂商代码           */
    uint8   device[16];  /**< 设备编号           */
    uint8   softVer[8];  /**< 软件版本号         */
    uint8   softDate[8]; /**< 软件版本日期       */
    uint8   cfginfo[16]; /**< 终端配置容量信息码       */
    uint8   prtlVer[8];  /**< 终端通信协议.版本号       */
    uint8   hardVer[8];  /**< 硬件版本号         */
    uint8   hardDate[8]; /**< 硬件版本日期       */
    uint8   extend[8];   /**< 厂家扩展信息       */
}__attribute__ ((packed)) FACTORY376_VER_TMP_T;


#define REALDATA_OAD_MAINOI   0x0000      //方案中一般的普通采集数据主OAD中OI用0x0000来表示方便建文件夹
extern const char* planTypeName[];

#ifdef __cplusplus
}
#endif

#endif // __DATA_TYPE_H__
