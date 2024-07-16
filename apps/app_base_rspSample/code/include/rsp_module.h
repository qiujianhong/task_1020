/*
*********************************************************************
* Copyright(C) 2022 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2022-6-16
*********************************************************************
*/
#ifndef __RSP_MODULE_H__
#define __RSP_MODULE_H__


#ifdef __cplusplus
extern "C" {
#endif

int rsp_uart_open(char *name);
int rsp_uart_set(int fd, int baud, uint8 databits, uint8 stopbits, char parity);
int rsp_uart_send(int fd, uint8 *data, int len);
int rsp_uart_recv(int fd, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime);
void rsp_uart_close(int fd);

int rsp_send_link_comm(int fd, uint8 index);
int rsp_send_report_resp(int fd, uint8 index, uint16 dt);
int rsp_send_get_comm(int fd, uint8 index, uint16 dt);
int rsp_send_set_config(int fd, uint8 index, uint8 *cfg, uint8 num);
int rsp_send_set_shake(int fd, uint8 index, uint16 shaketime);
int rsp_send_set_time(int fd, uint8 index);
int rsp_send_res_time(int fd, uint8 index);
int rsp_send_set_file(int fd, uint8 index, uint32 curNO, uint32 maxNo, uint8 *data, uint16 len);
int rsp_proc_msg_ctrl(uint8 *apdu, uint16 len, uint16 *status, uint8 *num);

int rsp_proc_msg_rmt(uint8 *apdu, uint16 len, RSP_RMT_D_T *ptRmt);
int rsp_proc_msg_pulse_d(uint8 *apdu, uint16 len, RSP_PULSE_D_T *ptPulseD);
int rsp_proc_msg_pulasea(uint8 *apdu, uint16 len, RSP_PULSE_A_T *ptPulseA);
int rsp_proc_msg_mapinfo(uint8 *apdu, uint16 apdulen, RSP_MAP_INFO_T *ptMap);
int rsp_proc_msg_linkinfo(uint8 *apdu, uint16 apdulen, RSP_LINK_INFO_T *ptLink);
int rsp_proc_msg_485can(uint8 *apdu, uint16 len, uint8 *type);
int rsp_send_set_485can(int fd, uint8 index, uint8 type);


#ifdef __cplusplus
}
#endif

#endif // __RSP_MODULE_H__

