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

#ifdef __cplusplus
extern "C" {
#endif

#include "dataType.h"
#include "baseCommon.h"
#include "oopType.h"
//#include "sys_cfg.h"  //各种宏开关
#include "rtc.h"
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

#define ETH_FIRST_NAME              "eth0"
#define ETH_FIRST_IP                "192.168.9.177"
#define NET_SECOND_NAME             "eth1"
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
#define   g_peventRootPath      "/mnt/data/";   // "/data/";

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



//#define LOG_MAX_SIZE    512000          /*日志大小 字节数    512K*/

#define writew_lock(fd, offset, len) \
            RcdLock(fd, F_SETLKW, F_WRLCK, offset, SEEK_SET, len)

#define un_lock(fd, offset, len) \
            RcdLock(fd, F_SETLK, F_UNLCK, offset, SEEK_SET, len)

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

typedef struct
{        
    uint16    home;      
    uint16    end;     
    uint8    index;
}PCSEG_INFO_T;      /*-功控时段信息-*/

#define BCD2BIN(val)    (((val) & 0x0f) + ((val)>>4)*10)
#define BIN2BCD(val)    ((((val)/10)<<4) + (val)%10)

#define BCD_TO_BIN(val) ((val)=BCD2BIN(val))
#define BIN_TO_BCD(val) ((val)=BIN2BCD(val))


void commfun_selectsleep(unsigned sec, unsigned us);
uint8 CalWeekday(int y, int m, int d);
int lighton_lcd(void);
int lightoff_lcd(void);

int systime_set_to_smiOS(DateTimeHex_t pDateTime);
int addr_send_to_radio(uint8* czCode, uint8* czAddr, uint8* oopAddr);
int config_send_to_radio(uint8 channel, uint8 power, uint8 sensitivity);
int yang_send_to_radio(uint8 power);
int long_send_to_radio(uint8 cmd);
int config_get_for_radio();
int modem_version_get_for_radio();


//int IPconfig_set_to_smiOS(char* Name, int nIP, int NetMask, int NetGw);
int ipconfig_send_to_smiOS(char* Name, uint8 type, uint8* IP, uint8* NetMask, uint8* NetGw);
int param_init_sent_to_acmeter(void);
int data_init_sent_to_acmeter(void);
void gui_mannual_amr_rcv(uint8 *rptmsg);
int gui_mannual_amr_send(uint8* playload, uint16 length);


int RebootTerminal();
int SetATinteractiveEvent(char Sign);

int chartoint32(char* pBuf);
void ModuleAckProc();


typedef struct
{
	uint8 state;
	uint8 nIP[4];
	uint16 nPort;
}ETH_CONNECT_STATE;


typedef struct
{
	uint8 num;
	ETH_CONNECT_STATE ethstate[1];
}ETH_CONNECT_INFO;


typedef struct
{
	char dev;
	char wirelessmode;
	uint32	nSignal;
}WIRELESS_EVNET_INFO;

#define DPRINT_GUI(d, b, l) \
    do{\
        if(NULL != getenv("DEBUG_GUI"))\
        {\
            printf(d);\
            uint16 iii = 0;\
            for(; iii < l; iii++)\
            {\
                printf("%02x ", b[iii]);\
            }\
            printf("\n");\
        }\
    }while(0);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _COMMFUN_H */

