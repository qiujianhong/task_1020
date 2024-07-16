/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：EventFun.h
* 版本号：1.0
* 功能：对事件检测过程中用到的数据结构，调度函数进行声明
* 原始开发人及时间：董宁 2006年12月25日
* 修改人员、时间及修改内容:无
* 其他：
******************************************************************-*/
#ifndef _EVENTFUN_H
#define _EVENTFUN_H

typedef struct
{
    uint8   power_type;                         //测量点接线方式
    int32   cur_a;                              //A相电流
    int32   cur_b;                              //B相电流
    int32   cur_c;                              //C相电流
    uint32  cur_gate;                           //额定电流-从数据字典读出后*10,方便和3位小数的电流比较
    uint16  vlt_gate;                           //额定电压
    uint32  cur_lmt;                            //电压断相、失压时电流限值-
    uint16  vlt_a;                              //A相电压
    uint16  vlt_b;                              //B相电压
    uint16  vlt_c;                              //C相电压
    uint8   batt_status;                        //电池状态
    uint8   cb_interval;                        //抄表间隔
}EVENT_GLOBAL_T;

//宏定义
#define BIT0                         0x01
#define BIT1                         0x02
#define BIT2                         0x04
#define BIT3                         0x08
#define BIT4                         0x10
#define BIT5                         0x20
#define BIT6                         0x40
#define BIT7                         0x80

//事件标记定义
#define BREAK_CIRCUIT                 0x00001     //电流开路
#define CUR_RVS                       0x00002     //电流反极性
#define BREAK_PHASE                   0x00004     //电压断相
#define LOSE_VOLT                     0x00008     //电压缺相（失压）
#define PHASE_REVER                   0x00010     //逆相序
#define SUPASS_VLT                    0x00020     //电压超上限
#define LOW_VLT                       0x00040     //电压低下限
#define HIGH_CUR                      0x00080     //电流超上限
#define HIGHER_CUR                    0x00100     //电流超上上限
#define CUR_NBLC                      0x00800     //电流不平衡
#define VLT_NBLC                      0x01000     //电压不平衡
#define SHORT_CIRCUIT                 0x100000    //电流短路

#define A_PHASE                       0x01         //a相
#define B_PHASE                       0x02         //b相
#define C_PHASE                       0x04         //c相

#define METER_STATUS_LABEL             0x13

#define     Tmn_Ram_Info_ID      0                      // 终端内存自检信息,1字节
#define     Tmn_Flash_Info_ID    (Tmn_Ram_Info_ID+1)    // 终端Flash自检信息,格式同上
#define     Tmn_Eeprom_Info_ID   (Tmn_Flash_Info_ID+1)  // 终端EEPROM自检信息,格式同上
#define     Tmn_Rtc_Info_ID      (Tmn_Eeprom_Info_ID+1) // 终端RTC自检信息,格式同上
#define     RTUSelfCheckAddr     (Tmn_Rtc_Info_ID+1)    // 2字节，终端事件记录，D0表示终端停电，D1表示自检异常，其他备用
#define     GBLostvltAddr        (RTUSelfCheckAddr+2)   // 9字节，电压断相记录地址
#define     GBLowvltAddr         (GBLostvltAddr+9)      // 9字节，电压失压记录地址
#define     GBVltUpAddr          (GBLowvltAddr+9)       // 9字节，电压越上限记录地址
#define     GBVltDownAddr        (GBLowvltAddr+9)       // 9字节，电压越下限记录地址
#define     GBCurrUpAddr         (GBVltDownAddr+9)      // 9字节，电流越上限记录地址，也称为电流过载
#define     GBCurrDownAddr       (GBCurrUpAddr+9)       // 9字节，电流越下限告警地址，也称为电流轻载
#define     GBVltUnbAddr         (GBCurrDownAddr+9)     // 9字节，电压不平衡记录地址
#define     GBCurrUnbAddr        (GBVltUnbAddr+9)       // 9字节，电流不平衡记录地址
#define     GBPhaseReveAddr      (GBCurrUnbAddr+9)      // 9字节，电流反极性记录地址
#define     GBVltAbnormalAddr    (GBPhaseReveAddr+9)    // 9字节，相序异常(电压)记录地址
#define     GBCurrAbnormalAddr   (GBVltAbnormalAddr+9)  // 9字节，相序异常(电流)记录地址
#define     EVENT_RAMX_FILE_SZIE (GBCurrAbnormalAddr+9)

//函数声明
int   WriteRamXData(int off, char *buf, int len);
int   ReadRamXData(int off, char *buf, int len);

int   WriteEventToDB(mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, OOP_EVTBUF_T eventData, uint8 bType);
void  OopDateTimeSToLinuxTime(OOP_DATETIME_S_T *dateTime, time_t *linuxTim);

void  FillEventGlobal(mtrnum_t mtr_id, OOP_METER_T *oopMeter);                          //测量点数据全局结构初始化
void  CurReverseEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType);             //电流反极性检测
void  VltBreakEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType);               //电压失压、断相判断
void  PhaseReverseEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter);                        //逆相序判断
void  PhaseReverseEvent07(mtrnum_t mtr_id, uint8 type);                                 //逆相序判断
void  VltCurImbalanceEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType);        //电压不平衡事件发生/恢复
void  VltCurImbalanceEvent07(mtrnum_t mtr_id, uint8 type);                              //电压不平衡事件发生/恢复
void  VltSupassEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter,uint8 bType);               //电压越限及恢复
void  VltSupassEvent07(mtrnum_t mtr_id, uint8 type);                                    //电压越限及恢复
void  CurHighEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType);                //电流越限及恢复
void  SaveFileRecBfg(uint8 bEventType,uint8 bPhase,uint8 msn);

//获取当前时间
void  GetCurrentOopDateTimeS(OOP_DATETIME_S_T *dateTime);

#endif
