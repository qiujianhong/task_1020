/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： smiOSModule.h
           数据中心模块
* @date： 2019-11-9
*********************************************************************
*/

#ifndef __SMIOS_MODULE_H__
#define __SMIOS_MODULE_H__

#include "baseType.h"
#include <stdint.h>
#include "message.h"

#ifdef __cplusplus
extern "C" {
#endif

// typdef sint32 (*initFp)(void);
/* @brief:枚举：消息通信类型  */
enum COMMUNICATION_TYPE_E
{
    MOD_TCP = 0,
    MOD_UDP,
    MOD_IPC,
    MOD_MQTT,
    MOD_MSG,
    MOD_DOCKER,
    MOD_APP,
    MOD_PLATFORM,
    MOD_XLFCGI,
    MOD_SYS,
    MOD_APPGUI,
    // MOD_MAX必须置于所有模块最后，防止数组越界
    MOD_MAX
};

/* @brief:结构体：模块化处理  */
typedef struct
{
    const sint8 *name;        //模块名称
    sint32 (*initFp)(void);   //模块的初始化回调函数
    sint32 (*startFp)(void);  //模块的启动回调函数
    void (*stopFp)(void);     //模块的停止回调函数
    sint32 enable;            //模块的开始或关闭使能标志位
} SERVICE_MODULE_T;

/* @brief:系统子模块初始化  */
sint32 init_system(void);

/* @brief:系统子模块启动  */
sint32 start_system(void);

/* @brief:释放各子模块内外部资源 */
void de_init_system(void);

sint32 smiOS_gui_start(void);

sint32 smiOS_gui_init(void);

int pMsgtoRawObj(char* source, char* dest, MESSAGE_INFO_T* pMsgInfo );

#ifdef __cplusplus
}
#endif
#endif