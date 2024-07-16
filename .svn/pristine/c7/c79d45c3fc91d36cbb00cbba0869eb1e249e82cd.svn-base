/*********************************************
 * Copyright(c) 2007,南京新联电子仪器有限公司
 * 		All rights reserved.
 * 程序名称: diproc.h
 * 版本号: 1.1
 * 功能: 遥信脉冲采集接口说明
 * 其他: 
 *********************************************/
#ifndef _DIPROC_H_
#define _DIPROC_H_

#ifdef __cplusplus
extern "C" {
#endif

#define A_FLAG 0xa5             //切换A相标志
#define B_FLAG 0x5a             //切换B相标志
#define C_FLAG 0x84             //切换C相标志
#define SND_FLAG1 0x87          //送频率高八位标志
#define SND_FLAG2 0x88          //送频率低八位标志
#define CHECK_A 0x7e            //切换A相校验值
#define CHECK_B 0x7d            //切换B相校验值
#define CHECK_C 0x7b            //切换C相校验值

#define INPUT 1
#define OUTPUT 3

/*
#define SBUS_MAICHONG 1
#define SBUS_YAOXIN   2
#define SBUS_RADIO    3
#define SBUS_FREE     4
*/

/* functions declaration */

/**********************************************
  所属功能单元: 遥信脉冲采集
  功能描述: 读取DI状态，返回当前的
            所有（32个）DI的状态
  输入参数: 无
  输出参数: ym_status 当前各采集点的状态
  函数返回值: 负数表示出错，0表示成功
  内部处理流程：使用系统调用read读取DI状态
 **********************************************/
int ReadDi(uint32 *ym_status);
int ReadDiExt(uint32 *ym_status);    //8bit jiaobao 7bit baomi 6bit menjiedian 5bit duangai

/**********************************************
  所属功能单元: 遥信脉冲采集
  功能描述: 读取遥信状态，返回当前的
            所有（8路）DI的状态
  输入参数: 无
  输出参数: data 当前各路遥信状态
  函数返回值: 负数表示出错，0表示成功
  内部处理流程：使用系统调用read读取DI状态
 **********************************************/
//int ReadYX(uint8 *data);

/**********************************************
  所属功能单元: 遥信脉冲采集
  功能描述: 向电台输出指定码值
            
  输入参数: data 待写入电台的码值
  输出参数: 
  函数返回值: 负数表示出错，0表示成功
  内部处理流程：使用系统调用read读取DI状态
 **********************************************/
int WriteDT(uint8 *data);


//int SBus(char select, uint8 *data);

#ifdef __cplusplus
}
#endif

#endif
