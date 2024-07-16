/*
****************************************************************************
* Copyright (c) 2013,南京新联电子股份有限公司
* All rights reserved.
* 程序名称：commfun.h
* 版 本 号：1.0
* 功    能：系统公共函数库
本函数库为单独动态库，供其他模块使用，主要是将所有的公共函数放入本模块内，减少重复代码
* 原始开发人及时间：
*           gujun    2013-04-09
* 修改人员、时间及修改内容：
* 其他：
* 注意：本动态库仅仅实现一些公共函数，不涉及逻辑处理，所以不允许出现全部变量、静态变量
****************************************************************************
*/

#ifndef _COMMFUN_H
#define _COMMFUN_H

#include "dataType.h"
#include "baseCommon.h"
#include "oopType.h"
#include "sys_cfg.h"  //各种宏开关
#include "rtc.h"
#include "framecomm.h"
#include "mettype.h"

#define    CHAR_TYPE_NONE             "\033[0m"
#define    COLOR_FONT_RED             "\033[31m"
#define    COLOR_FONT_GREEN           "\033[32m"
#define    COLOR_FONT_YELLOW          "\033[33m"
#define    UNDER_LINE                 "\033[4m"
#define    HIGH_LIGHT                 "\033[1m"

#define COMMFUN_ABS(a,b)                    ((a)>(b)?(a)-(b):(b)-(a))

#define COMMFUN_MASK_INDEX(a)   ((a) / 8)   //a从0开始，返回值为0
#define COMMFUN_MASK_BIT(a)     ((a) % 8)

#define ETH_FIRST_NAME              "FE0"
#define ETH_FIRST_IP                "192.168.9.177"
#define NET_SECOND_NAME             "FE1"
#define ETH_SECOND_IP               "10.0.0.1"

#define FM_PULSE_CONST_220_ADDR     0x40                          // 三相四线 220V脉冲常数存储地址
#define FM_AC_POWER_TYPE_ADDR       (FM_PULSE_CONST_220_ADDR + 4) // 交采接线方式 0x55:三相三线 0xAA:三相四线 其他:无线，按三相四线显示
#define FM_AC_Config_577_ADDR       (FM_AC_POWER_TYPE_ADDR + 1)   // 交采三相四线额定电压配置 0x55:57.7V 0xAA:220V 其他:无线，按220V处理
#define FM_CheckSumOKFlag_ADDR      (FM_AC_Config_577_ADDR + 1)   // 交采校验和一致性标志 0x55:不一致 0xCC:一致 其他:未知
#define FM_PULSE_CONST_100_ADDR     (FM_CheckSumOKFlag_ADDR+ 1)   // 三相三线 100V脉冲常数存储地址
#define FM_PULSE_CONST_577_ADDR     (FM_PULSE_CONST_100_ADDR+ 4)  // 三相四线 57.7V脉冲常数存储地址
#define FM_SELF_ADPTIVE_ADDR        (FM_PULSE_CONST_577_ADDR+ 2)  // 交采兼容模式 0x55--兼容模式
#define FM_AC_MEAIC_TYPE            (FM_SELF_ADPTIVE_ADDR + 2)    // 计量芯片类型 1:ATT7022E 2:RN8302
#define FM_GUI_MEAIC_TYPE            (FM_AC_MEAIC_TYPE+ 2)  //存储地区及hardwaretype信息，3表示专变，4表示集中器
#define MAC_REPORT_CCH_COUNT        10//上报通道数量

#if SHANGHAI_FUN == 0
#define AC_DFT_CONST_3P4L_220       6400                          // 终端交采三相四线220V默认脉冲常数
#define AC_DFT_CONST_3P4L_577       20000                         // 终端交采三相四线57.7V默认脉冲常数

#if ANHUI_DFT_CONST_2W == 1
#define AC_DFT_CONST_3P3L_100       20000                     // 终端交采三相三线100V默认脉冲常数(安徽特殊功能脉冲常数20000)
#elif((SHANXI_DFT_CONST_2W == 1) && (SHANXI_FUN == 1))
#define AC_DFT_CONST_3P3L_100       20000
#else
#define AC_DFT_CONST_3P3L_100       8000                          // 终端交采三相三线100V默认脉冲常数
#endif

#endif

#define AC_LINE_MODE_3P4L           0xAA                          // 交采接线方式:三相四线
#define AC_LINE_MODE_3P3L           0x55                          // 交采接线方式:三相三线
#define AC_LINE_MODE_ERR            0xFF                          // 交采接线方式错误，默认按三相四线处理
#define AC_VOLT_TYPE_220            0xAA                          // 交采三相四线额定电压:220V
#define AC_VOLT_TYPE_577            0x55                          // 交采三相四线额定电压:57.7V
#define AC_VOLT_TYPE_ERR            0xFF                          // 交采三相四线额定电压错误，默认按220V处理
#define AC_ADA_MODE                 0x55                          // 交采支持兼容模式
#if ANHUI_FUN==1
/*事件接口类使用的一些数据,主要是appmain中的事件模块和交采模块共用*/
#define SP_SERCH_EVT_FLAG           1           //特殊搜表事件标识
#else
#define SP_SERCH_EVT_FLAG           0           //特殊搜表事件标识
#endif

#define SP_SINGLE_RECORD_LEN        2000         //终端事件单条完整记录的最大数据长度
#define SP_RPT_EVT_RECORD_LEN       2100         //存储一条事件上报数据的最大长度

#define RECORD_DATA_POS             10          //终端事件数据存储文件，记录数据的起始偏移
#define MAX_EVT_RECORD_NUM          15          //终端事件最大记录条数
#define SINGLE_RECORD_LEN           500         //终端事件单条完整记录的最大数据长度
#define RPT_EVT_RECORD_LEN          600         //存储一条事件上报数据的最大长度

#define   PATH_MAX_LEN              256         //定义路径最大长度
#define   g_peventRootPath      "/data/";   // "/data/";

//-信道类型-
#define SFP_CCH_RS                  0x04        //rs:0x4-
#define SFP_CCH_SMS                 0x01        //短信:0x01-
#define SFP_CCH_GPRS                0x02        //GPRS:0x02-
#define SFP_CCH_DTMF                0x03        //DTMF:0x03-
#define SFP_CCH_Ethernet            0x04        //Ethernet(以太网):0x04-
#define SFP_CCH_Infrared            0x05        //infrared(红外):0x05-
#define SFP_CCH_RS232               0x06        //RS232:0x06-
#define SFP_CCH_CSD                 0x07        //CSD:0x07-
#define SFP_CCH_RADIO               0x08        //radio:0x8-
#define SFP_CCH_CDMA                0x09        //CDMA:0x9-
#define SFP_CCH_WDASN_RADIO         0x0b        //radio:0xb-
#define SFP_CCH_HIGHSPEED_RADIO     0x0c        //radio:0xc-

typedef struct
{
    int eth_ip;         // IP地址
    uint8  eth_en;      // 网口有效性
}__attribute__ ((packed)) ETH_INFO;     //以太网口相关信息

BOOL DT_IsLeap(uint16 year);
BOOL DT_DateTimeShortIsValid(OOP_DATETIME_S_T *DateTime);
BOOL DT_DateTimeShort2Time(OOP_DATETIME_S_T *dateTime,time_t *linuxTim);
BOOL DT_Time2DateTimeShort(time_t linuxTime, OOP_DATETIME_S_T *dateTime);
BOOL DT_DateTimeHexIsValid(DateTimeHex_t *DateTime);
BOOL DT_DateTimeHex2Time(DateTimeHex_t *dateTimeHex,time_t *linuxTim);
BOOL DT_Time2DateTimeHex(time_t linuxTime,DateTimeHex_t *dateTimeHex);
BOOL DT_DateTimeBCDIsValid(DateTimeBcd_t *DateTime);
BOOL DT_DateTimeBCD2Time(DateTimeBcd_t *dateTimeBCD,time_t *linuxTime);
BOOL DT_Time2DateTimeBCD(time_t linuxTime,DateTimeBcd_t *dateTimeBCD);

long DT_TI2Seconds(OOP_TI_T ti);
void DT_TimeAddInv(time_t *time,OOP_TI_T ti);
void DT_TimeSubInv(time_t *time,OOP_TI_T ti);

BOOL commfun_EthEnable(uint32 eth_id);
void commfun_IPToStr(int nIp, char* pVal);
void commfun_IPToHexStr(int nIP, char* pVal);
void commfun_HexIPToInt(uint8 *pczIp, int *pnIp);
BOOL commfun_IpToInt(char* pczIp, int* pnIp);
char* commfun_TrimLeft(char* pBuf);
BOOL commfun_IpConfig(uint8 bEnable, int nIp, uint32 eth_id, char *eth_name);       //设置网卡IP
int commfun_GetGatewayPara();   //从数据字典中读取终端网关（用户设置的值）
int commfun_GetNetmaskPara();   //从数据字典中读取终端的子网掩码（用户设置的值）
BOOL commfun_GetCoreVersion(char* pBuf);
BOOL commfun_GetCoreTime(char* pBuf);
char *commfun_memchr_gb(char *buf, uint8 sync, uint32 buf_len);
char *commfun_memchr_gb_send(char *buf, uint8 sync, uint32 buf_len);
void commfun_toupper(char* pStr);   //将字符串转化为大写字符串
void commfun_tolower(char* pStr);   //将字符串转化为小写字符串
char *commfun_memchr_gb_head(char *buf, uint8 sync, uint32 buf_len);
uint8 commfun_GetMtrMode(mtrnum_t mtr_id);
#if FAC_MODE == 1       //出厂模式
int commfun_ReadFacMode(S_FacMode* pFacMode);
int commfun_WriteFacMode(S_FacMode* pFacMode);
#endif
void commfun_printstring(uint8 *buf, int16 len);
void commfun_printcurtime(BOOL blnNewline, BOOL blnPrint);      //打印当前时间
void commfun_writebuflog(BOOL blnPrint, const char *filepath, char *buf, int len, const char *fmt, ...);        //将报文写入日志中
char commfun_HexToBcd(int hex);                                 //将HEX格式转换为BCD格式
int commfun_BcdToHex(char compress_bcd);                        //将BCD格式转换为HEX格式
time_t commfun_rtc2time(rtc_t src_rtc);                         //将rtc时钟的格式转换为距离1970年1月1日0时0分0秒的秒数
time_t commfun_rtc2time2(rtc_t src_rtc);                            //将rtc时钟的格式转换为距离1970年1月1日0时0分0秒的秒数
void commfun_GetLastNDate(int daydiff, time_t basetime, rtc_t *pLastNRtc);      //取得当前第daydiff天前的日期时标
void commfun_selectsleep(unsigned sec, unsigned us);
void commfun_BinToAscii(uint8 bin, uint8 *high, uint8 *low);        //将16进制数转换成两个Ascii码
uint8 commfun_AsciiToBin(uint8 high, uint8 low);                    //将两个Ascii码转换成一个16进制数
int commfun_BCD2ASC(char *asc, char *bcd, int len, int fmt);        // bcd码转换为ascii码
int commfun_ASC2BCD(char *bcd, const char *asc, int len, int fmt);  // ascii码转换为bcd码
BOOL commfun_makePathedDir(const char *path);
BOOL commfun_FileExists(const char *filepath);                      // 检测文件是否存在
BOOL commfun_PathExists(const char *pathname);                      // 判断文件夹是否存在
int commfun_ExtractFilePath(const char *src, char *path);           // 从包含有完整路径的文件名中提取出其路径
int commfun_GetFileSize(const char *filename);
uint32 commfun_ReadACPulseConst(void);                              // 从铁电读取交采脉冲常数
int commfun_WriteACPulseConst(uint32 PulseConst);                   // 写交采脉冲常数至铁电
int commfun_RelayIPCLen(RELAY_IPC_PULS *pRelayIpcPuls);             // 有RELAY_IPC_PULS结构体的RelayIPCLen成员来计算整个结构体大小
time_t commfun_tmdiff(char *pointtime);
int commfun_ReadTmnSerialNum(char *pTmnSerialNum);              // 获取终端编号
int commfun_WriteTmnSerialNum(char *pTmnSerialNum);              // 写终端编号
int commfun_GetApnInfo(char *apn_user, char *apn_pwd, char netType, BOOL withPostfix);      // 获取用户名和密码
BOOL CheckFF(uint8 *buf, uint16 len);
void commfun_SelfAdaptRs485Fun();
typedef struct
{
    unsigned long mant2;        // mantissa2
    unsigned long mant   : 20;  // mantissa
    unsigned long exp    : 11;  // exponent
    unsigned long sign   : 1;   // sign
} DBL_PAT;

typedef union
{
    DBL_PAT  df;
    double   d;
} DoubleUnionType;
double commfun_msqrt(double x);         //开平方根

/** @brief 上报类型 */
typedef enum
{
    RPT_TYPE_TASK = 0,  /* 任务上报 */
    RPT_TYPE_EVENT = 1, /* 事件上报 */
}RPT_TYPE_E;

#if EVENT_2014_EN == 1
typedef struct
{
    uint32 MtrDI0DI1;       // 电能表数据标识BIN 4
    uint8  MtrGrade;        // 电能表数据等级BIN 1
}__attribute__ ((packed)) MET_GRADE_INFO;       //F106：电能表数据分级参数

typedef struct
{
    uint8 CycleVal;         // 周期数值BIN 1
    uint8 CycleUnit;        // 周期单位BIN 1
}__attribute__ ((packed)) EVENT_READ_CYCLE;     //F107：电能表数据分级周期参数

typedef struct
{
    uint32 MtrDI0DI1;       // 电能表数据标识BIN 4
    uint8  MtrGrade;        // 电能表数据等级BIN 1
    uint8  DIDeal;          // 数据标识处理信息（0-允许处理（默认） 1-单抄的事件记录标识 10-重复标识 11-完整事件记录标识 12-未到抄读周期）BIN 1
    uint32 EventCountDI;    // 所属的数据记录（如果是事件分相记录，则需要记录当前记录对应的事件发生次数的标识）
}MET_GRADE_DIDeal;      //F106配置的数据项的处理标识

#define EVENT_GRADE_FILE            "^EventGradeFile"      //数据分级参数的存储文件（F106），每个数据分级存放一个文件
/*EVENT_GRADE_FILE 文件存储格式：

     2字节            5字节                      5字节                                         5字节
   数据分级     低4字节:BCD事件代码1        低4字节:BCD事件代码2                                 低4字节:BCD事件代码N      2字节
  参数数量N      高1字节:BIN事件等级        高1字节:BIN事件等级                              高1字节:BIN事件等级     校验和
├────────────┼────────────────────────────┼─────────────────────────────┼──────……………………………………───┼──────────────────────────────┼───────────┤*/

void commfun_WriteGradeInfo(uint16 GradeNum, char *pInbuf);     //将主站下发的“F106：电能表数据分级参数”写入到私有外存中
int commfun_ReadGradeInfo(uint16 GradeNum, char *pOutbuf);      //从私有外存中读取“F106：电能表数据分级参数”
void commfun_ClearGradeInfo(uint16 GradeNum);                   //参数初始化的时候，清除“电能表数据分级参数”
void commfun_WriteEventReadCycle(uint16 GradeNum, uint8 CycleVal, uint8 CycleUnit);     //写入“F107：电能表数据分级周期参数”
int commfun_ReadEventReadCycle(uint16 GradeNum, EVENT_READ_CYCLE *pReadCycle);          //读取“F107：电能表数据分级周期参数”

#if 1
typedef struct
{
    uint8   type;                       //ID类别
    uint8   WorkStatBit;                //状态字对应的位信息
    uint16  bDataId;                    //源字典ID（字典格式）
    uint32  DI_EventCount;              //645规约格式（事件发生次数）
    uint32  DI_MaskBolck;               //块抄的掩码标识（如果某个字节置1的话，标识该字节是单抄、块抄区分的字节）
    uint8   DINum;                      //ID个数(事件需要关联的数据项,如发生时间和发生时的正向有功电能示值)
    uint32  DI_Frid[10];                //645规约格式（需要抄读上1次事件记录相关数据标识）（如果对应标识为0xffffffff，标识不抄读）
} EventID_To_DI;
#else
typedef struct
{
    uint8   type;                       //ID类别
    uint8   WorkStatBit;                //状态字对应的位信息
    uint16  bDataId;                    //源字典ID（字典格式）
    uint32  DI_EventCount;              //645规约格式（事件发生次数）
    uint32  DI_MaskBolck;               //块抄的掩码标识（如果某个字节置1的话，标识该字节是单抄、块抄区分的字节）
    uint32  DI_LastRecord[10];          //645规约格式（上1~10次事件记录数据标识）（如果对应标识为0xffffffff，标识不抄读）
} EventID_To_DI;
#endif
const EventID_To_DI *GetEventIDToDI(uint32 MtrDI);             //根据645规约ID查找EventIDToDI中的成员
const EventID_To_DI *GetEventIDToDIByIndex(uint8 IDIndex);     //根据“状态字对应的位信息”来查找

#endif

#if ACADDR_FROM_PRTL == 1
#undef  AC_EEPROM_FILE
#define AC_EEPROM_FILE   "^ac_eeprom_file"  // 交采私有参数文件

int commfun_ReadACCommAddr(char *const pAddr);
int commfun_WriteACCommAddr(const char *const pAddr);
#endif



#if GetMewMeter_EN == 1
//给显示用的调试信息
typedef enum
{
    SEARCH_MET_IDLE,
    SEARCH_MET_START,
    SEARCH_MET_PAUSE,
    SEARCH_MET_RESUME,//新增状态，处理被抄表中断的搜表任务
}SEARCHSTS;

#define CB_FAIL_DAYCNTS_CLEAR_RECORD    30  //抄表失败几天清除档案记录,默认30天
typedef struct
{
    uint8 fail_cnt;
    uint8 BCD_DD;
    uint8 BCD_MM;
    uint8 BCD_YY;
}FailTimeAndCnt;

typedef enum
{
    SS_SEAR_START=0,        //搜索开始
    SS_READY_07,            //07搜索准备
    SS_SEND_07,         //07搜索发送
    SS_RECV_07,         //07搜索接收
    SS_FILE_07,             //07表记录文件
    SS_READY_97,            //97搜索准备
    SS_SEND_97,         //97搜索发送
    SS_RECV_97,         //97搜索接收
    SS_FILE_97,             //97表记录文件
    SS_READY_698,           //698搜索准备
    SS_SEND_698,            //698搜索发送
    SS_RECV_698,            //698搜索接收
    SS_FILE_698,            //698表记录文件
    SS_SEAR_END,            //搜索结束
}SEARCH_STATE_T;        //搜索状态


typedef struct
{
    char    mtraddr[6];         //当前搜表的表地址

    uint8   Search_Met_Stat;    //搜索表的状态
    uint8   SState;             //搜索状态

    uint8   SMetNums;           //搜索到的97加07总的电表数量
    uint8   SMetNumMax;     //最多485数量

    uint8   Order;              //表示第几轮
    uint8   ALLMetNum;          //最大表号

    uint8   SerchNum;           //正在进行到的搜索表数
    uint8   ClashMetNum;        //有冲突的电表总的数目
    uint8   DoClashNum;     //进行到第几个
    uint8   FindMetNum;     //找到合格报文的总个数
    uint8   StoreMetNum;        //找到表地址的总的表数
}__attribute__ ((packed)) AutoGetMtrInfo;

#define Met645_97   1
#define Met645_07   30
#define Met645_PLC  31
#define Speed_1200  1
#define Speed_2400  2
#define RS485_1         1
#define RS485_2         2

//定义存储捞表找到的表档案
#define MetAddr_97      "Met97Addr"
#define MetAddr_07      "Met07Addr"
#define MetAddr_PLC     "PLCMetAddr"

typedef struct _StoreMetStruct
{
    unsigned char MetAddr[6];           /*找到表地址*/
    unsigned char ColAddr[6];           /*对应采集器地址*/
    unsigned char MetType;              /*表类型，1--97表，30---07电表*/
    unsigned char MetSpeed;         /*表速率，1--1200，2--2400*/
    unsigned char MetCom;               /*表通讯端口，1--485的1口，2---485的2口*/
    #if HUNAN_FUN == 1
    uint8         RealPhase;            //电表相位(低半字节/(0:未知/1~3对应ABC相))/信号品质(高半字节)
    uint8         MasterAddr[6];        //主节点地址
    #endif
}__attribute__ ((packed)) StoreMetStruct;/*文件存储的结构体*/

typedef struct _MetF13Struct
{
    unsigned char MetAddr[6];           /*找到表地址*/
    unsigned char ColAddr[6];           /*对应采集器地址*/
    unsigned char MetType;              /*表类型，1--97表，30---07电表*/
}__attribute__ ((packed)) MetF13Struct;/*文件存储的结构体*/



typedef struct _SLAVE_NODE
{
    char happenTime[5];         //分时日月年 BCD
    char slave_node_addr[6];    //从节点地址
    char slave_node_type;       //从节点类型 00 采集器01 电能表
    char slave_prtl_type;       //电能表规约类型:01 97表  02 07表
    uint16 state_word_1;        //状态字1
    uint16 state_word_2;        //状态字2
    uint16 state_word_3;        //状态字3
    uint16 state_word_4;        //状态字4
    uint16 state_word_5;        //状态字5
    uint16 state_word_6;        //状态字6
    uint16 state_word_7;        //状态字7
}__attribute__ ((packed)) SLAVE_NODE_UP_Struct;/*从节点上报结构体*/


#define ACT_REPORT_FILE     "PlcReprotInfo"    //上报信息文件
#if BROADBAND_698_EXT_FUNC == 1
#define NEW_METER_FILE     "PlcNewMtrInfo"    //新增电表信息，用于规约判断是否已在档案中存在
typedef struct _ACQ_ID_INFO
{
    uint8 AcqAddr[6];
    uint8 ChipInfo[27];
    uint8 ModuleInfo[55];
}__attribute__ ((packed)) ACQ_ID_INFO;/*采集器ID信息*/
#endif

#endif

#if BEIJING_FUN == 1
int commfun_Net_Rec_Snd_Proc(char ProcFlag);
#endif
#if DOUBLE_CHANNEL_EN == 1      // 双通道
#define DBCH_TYPE_RADIO         1           //普通电台
#define DBCH_TYPE_WDASN         2           //WDASN
#define DBCH_TYPE_FASTER        3           //高速电台
#define DBCH_TYPE_FASTER_30     30
#define DBCH_TYPE_FASTER_31     31
#define DBCH_TYPE_FASTER_33     33
#define DBCH_TYPE_FASTER_34     34
BOOL commfun_DBChRadioType(uint8 *pRadioType, uint8 *pFasterType);            //获取双通道信道使用的电台类型
BOOL CheckIsDoubleChannel(void);
#endif

uint8 CalWeekday(int y, int m, int d);
void RebootByTaskd(void);

#endif /* #ifndef _COMMFUN_H */

