/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块PPP操作相关定义申明
* @date：    2019-10-24
* @history： 
*********************************************************************
*/
#ifndef __PPP_OPER_H__
#define __PPP_OPER_H__
        
#ifdef __cplusplus
        extern "C" {
#endif

#define LCP_AUTH				14		/*LCP链路协商*/
#define IPCP_AUTH				15		/*IPCP链路协商*/


BOOLEAN wireless_check_pppif(uint8 ifnum);
BOOLEAN wireless_check_ppp_porc(uint8 ifnum);
void wireless_ppp_off(uint8 ifnum);
uint32 wireless_get_pppIP(uint8 ifnum);
uint32 wireless_get_pppMASK(uint8 ifnum);
uint32 wireless_get_pppgetway(uint8 ifnum);
uint64 wireless_get_flow(uint8 ifnum);
void wireless_flush_table(uint8 ifnum);
void wireless_add_route(uint8 ifnum, uint32 getway, uint32 ipaddr);


#ifdef __cplusplus
}
#endif

#endif //__PPP_OPER_H__

