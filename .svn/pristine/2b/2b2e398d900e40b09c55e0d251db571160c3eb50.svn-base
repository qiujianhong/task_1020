/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#ifndef __CCO_MAIN_H__
#define __CCO_MAIN_H__

#include "CcoRedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

//地区宏定义
#define AREA_ZHEJIANG  1    //是否浙江地区模式

/*******************************************************************************
* 本地对上通信 UDP通信参数 *********************------<测试修改>------**********
*******************************************************************************/

//消息是否用mqtt的方式编译开关------<测试修改>------
#define MSG_MQTT  1    //消息是否用mqtt的方式

//本地通信模块是否鼎信编译开关------<测试修改>------
#define DOWN_DEV_TYPE  2   //本地通信模块类型, 0:智芯, 1:鼎信, 2:HAL接口

//mqtt ip地址与端口号
extern char mqttIP[100];
extern int mqttport;

//是否关注的app均已注册
extern uint8 gMapManagerAppLoginFlag;
extern uint8 gDBCenterAppLoginFlag;

//本app是否已经注册成功
extern uint8 bAppSelfLoginState;

#define IPC_SOCK_INIT           0               //初始化socket连接
#define IPC_SOCK_RECV           1               //socket数据接收
#define IPC_SOCK_SEND           2               //socket数据发送
#define IPC_SOCK_CLOSE          3               //关闭socket连接
#define IPC_SOCK_LOGIN          4               //登录到系统管理器
#define IPC_SOCK_LOGIN_WAIT     5               //登录等待回复
#define IPC_SOCK_MODULE         6               //查询模组信息
#define IPC_SOCK_RETRY          7               //重试


extern uint8 gAmrThreadFlag[MAX_DOWN_THREAD_NUM];                    //自动抄表线程标志
extern uint8 gRebootAmrThread[MAX_DOWN_THREAD_NUM];                  //重启抄表线程标志

//appname: app英文名称(用于查询appid)
extern char *AppIdName;

//appid: 对应app的ID信息
#define  APP_ID_LEN             24
extern char  AppId[APP_ID_LEN];


#ifdef __cplusplus
}
#endif

#endif // __CCO_MAIN_H__