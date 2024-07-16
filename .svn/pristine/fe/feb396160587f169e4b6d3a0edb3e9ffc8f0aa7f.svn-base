/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   控制模块继电器和其他设备相关
* @date：    2020-4-5
* @history： 
*********************************************************************
*/
#ifndef __APPCTRL_DEV_H_
#define __APPCTRL_DEV_H_

#ifdef __cplusplus
        extern "C" {
#endif

#define CONTROL_SET_RTS          0X6501  //正常工作模式
#define CONTROL_CLEAR_RTS        0X6502  //复位脚拉低复位模块
#define CONTROL_GET_MODSTATE     0x6506  //模块插入识别 （返回0 为有模块， 1为没有）


#define SWITCH_CLOSE            0x55  //-闸当前状态：合闸-
#define SWITCH_OPEN             0xaa  //-闸当前状态：拉闸-

#define TURN_OFF_ALARM          0x55  //-未处于告警状态-
#define TURN_ON_ALARM           0xaa  //-处于告警状态-

#define TURN_OFF_STATE          0x55  //-未处于跳闸状态-
#define TURN_ON_STATE           0xaa  //-处于跳闸状态-

#define TURN_NON_FLAG           0x55  //-该轮次未投入-
#define TURN_SET_FLAG           0xaa  //-该轮次已投入-

#define TURN_INTERVAL           60    //-补跳时间间隔-
#define OUTPUT_DELAY            2     //-闸控制输出前延时-

// 1字节 接口板输出状态标志
// D3~D0: 每位对应置1，代表分闸； 对应置0，代表合闸
// D4：    置1，代表打开预动；置0，代表预动关闭
// D5：    置1，代表告警继电器输出；置0，代表预动关闭
// D6：    置1，代表蜂鸣器输出；置0，代表蜂鸣器不输出
#define PREPARE_BIT   0x10        //预动
#define RELAY_BIT     0x20        //告警继电器
#define BELL_BIT      0x40        //蜂鸣器

/*-******************************************************************-*/
/*-                            数据结构                                 -*/
/*-******************************************************************-*/

typedef struct
{
    uint8  TURN_1_CTRL          :1;    //轮次1控制输出(0表示继电器输出  1表示继电器不动作)
    uint8  TURN_1_REDLIGHT      :1;    //轮次1红灯(0-熄灭；1-点亮)
    uint8  TURN_1_BLUELIGHT     :1;    //轮次1绿灯(0-熄灭；1-点亮)
    uint8  TURN_2_CTRL          :1;    //轮次2控制输出(0表示继电器输出  1表示继电器不动作)
    uint8  TURN_2_REDLIGHT      :1;    //轮次2红灯(0-熄灭；1-点亮)
    uint8  TURN_2_BLUELIGHT     :1;    //轮次2绿灯(0-熄灭；1-点亮)
    uint8  TURN_3_CTRL          :1;    //轮次3控制输出(0表示继电器输出  1表示继电器不动作)
    uint8  TURN_3_REDLIGHT      :1;    //轮次3红灯(0-熄灭；1-点亮)
    uint8  TURN_3_BLUELIGHT     :1;    //轮次3绿灯(0-熄灭；1-点亮)
    uint8  TURN_4_CTRL          :1;    //轮次4控制输出(0表示继电器输出  1表示继电器不动作)
    uint8  TURN_4_REDLIGHT      :1;    //轮次4红灯(0-熄灭；1-点亮)
    uint8  TURN_4_BLUELIGHT     :1;    //轮次4绿灯(0-熄灭；1-点亮)
    uint8  POWER_CTRL_LIGHT     :1;    //功控灯(0-熄灭；1-点亮) 有控制投入时
    uint8  ELE_CTRL_LIGHT       :1;    //电控灯(0-熄灭；1-点亮) 有控制投入时
    uint8  ALARM_OUTPUT         :1;    //告警继电器(0表示继电器输出  1表示继电器不动作)
    uint8  PROTECT_LIGHT        :1;    //保电灯(0-熄灭；1-点亮) 有控制投入时
}__attribute__((packed))SWITCH_BITS_T;

typedef struct tagCtrlSwitch
{
    uint32     num;
    uint8      status[OOP_MAX_TURN];
}CTRL_SWITCH_STATUS_T;

typedef struct tagCtrlRs232
{
    uint8 port;
    uint8 subport;
    uint8 res[2];
    char  deviceName[30];        /*设备名 */
    void *pDev;
    int   fd;
}CTRL_RS232_T;


void appctrl_switch_init(uint8 port, uint8 turnMax);
void appctrl_switch_init_zcu_1();
void appctrl_control_state(uint8 acm1, uint8 acm2, SWITCH_INTERFACE_T *pInterface);
int appctrl_set_switch_close(SWITCH_INTERFACE_T *pInterface);
int appctrl_set_switch_zcu1(SWITCH_INTERFACE_T *pInterface);
int appctrl_set_switch_zcu3(uint8 acm1, uint8 subport1, SWITCH_INTERFACE_T *pInterface);
int appctrl_set_switch(uint8 acm1, uint8 subport1, uint8 acm2, uint8 subport2, SWITCH_INTERFACE_T *pInterface);
int appctrl_get_switch_input_zcu1(uint8 acm1, uint8 acm2, SWITCH_INTERFACE_T *pInterface);
int appctrl_get_switch_input(uint8 acm1, uint8 subport1, uint8 acm2, uint8 subport2, SWITCH_INTERFACE_T *pInterface);
void appctrl_status_write(uint8 bDay);
int appctrl_init_alarm_switch(uint8 acm, uint8 subport);
void appctrl_alarm_out();

#ifdef __cplusplus
}
#endif

#endif

