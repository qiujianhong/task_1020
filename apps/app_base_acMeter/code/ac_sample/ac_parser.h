/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#ifndef _AC_PARSER_H_
#define _AC_PARSER_H_

#include "ac_redef.h"

#ifndef APP_NONECHAR
#define APP_NONECHAR    0xee                            /*字节型无效数据类型*/
#endif

extern int ac_Hormonic_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_Energy_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_Demand_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_Volt_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_VoltHD_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_Curr_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_zero_line_Curr_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_zero_Curr_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_Power_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_ActPower_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_ReaPower_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_AppPower_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_PowerFactor_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_uint16_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_uint32_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_int16_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_int32_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_HZ_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_PhAngle_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_Version_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_TransStatus_parser(OOP_OAD_U oad, uint8* pInMsg, uint32 inLen, void* destData, void* destDataBak);
extern int ac_MeterSstate_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_Config_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_PowerType_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int get_info_map(OOP_OAD_U oad, OAD_INFO_MAP_T **ppDataMap);
extern int ac_Set_normal_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_get_40160201_parser_fj(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_get_40160201_parser_js(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_get_40160200_parser_sd(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_get_40140200_parser_sd(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_Get_normal_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
extern int ac_rpt_Topo_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);

extern int ac_rpt_meter_event_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);
#endif