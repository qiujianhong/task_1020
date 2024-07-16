/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 头文件
* @date： 2019-11-20
*********************************************************************
*/
#ifndef __SYSTEM_CTRL_H__
#define __SYSTEM_CTRL_H__

#include "baseType.h"

#ifdef __cplusplus
extern "C" {
#endif

#define POWER_ON    1
#define POWER_OFF   0

#define ETH_FIRST_NAME              "FE0"
#define ETHTOOL_GLINK          0x0000000a

#define appCnt          3
#define appInConCnt     5

struct ethtool_value {
        unsigned long   cmd;
        unsigned long   data;
};
/* @brief：延时执行接口 */
int act_after_secs(int sec, int (*fp)(void));
/* @brief：系统重启接口 */
int system_reboot(void);

/* @brief：获取电源状态接口 */
int system_get_power_state(sint32 *state);

/* @brief：获取以太网状态接口 */
int system_get_ethernet_state(sint8 * deviceName, sint32 *state);

int system_get_main_power_state(sint32 *state);

int system_get_main_cell_power(int *value);
int system_battery_power_set(uint8 val);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_INFO_H__ */
