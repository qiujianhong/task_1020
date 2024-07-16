/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块PPP操作相关定义申明
* @date：    2019-10-24
* @history： 
*********************************************************************
*/
#ifndef __AT_OPER_H__
#define __AT_OPER_H__

#ifdef __cplusplus
    extern "C" {
#endif


#define AT_ANSWER_LENGTH         2048
#define AT_SEND_CNT              1       /*发送AT命令次数*/
#define MIN_CSQ_VALUE            6       /*进行拨号的最小信号值*/
#define AT_GET_CNT               60       /*AT获取命令次数*/

#define AT_LENTH                 256   /*AT命令长度*/

typedef struct {
    char manufacture[4 + 2];            //厂商代号
    char model[8 + 2];                    //模块型号
    char firmare_version[4 + 2];        //软件版本号
    char release_date[6 + 2];            //软件发布日期
    char hw_version[4 + 2];                //硬件版本号
    char hw_release_date[6 + 2];        //硬件发布日期
}MYGMR_INFO_T;

#define AT_SMS_TYPE           0x10
#define AT_DEV_NAME(ifnum)    ((ifnum & 0xf0) == 0 ? "拨号端口" : "短信端口")
#define AT_DEV_NUM(ifnum)     ((ifnum & 0x0f) + 1)
#define AT_DEV_NAMENUM(ifnum)  AT_DEV_NAME(ifnum), AT_DEV_NUM(ifnum)


int at_get(uint8 ifnum, void *pDev, char *send_buf, char *get_buf, const int32 get_buf_len);
int at_get_nolog(uint8 ifnum, void *pDev, char *send_buf, char *get_buf, const int32 get_buf_len);
int at_check(uint8 ifnum, void *pDev, char *send_buf, char *check_buf);
int at_check_nolog(uint8 ifnum, void *pDev, char *send_buf, char *check_buf);
int at_check_model(uint8 ifnum, void *pDev, const int at_cnt);
int at_get_csqvalue(char *buf);
int at_get_cesqvalue(char *buf);
int at_get_mygmr(uint8 ifnum, void *pDev, MYGMR_INFO_T* buffer);
uint8 at_get_module_type(char* buf, char *rettype);
uint8 get_network_type(char *pBuf);
uint8 wireless_get_network(uint8 ifnum, void *pDev, uint8 *regOperat, uint8 bLog);
int wireless_get_csqvalue(uint8 ifnum, void *pDev);
int wireless_get_phonenum(uint8 ifnum, void *pDev, uint8 *pBuf, uint8 Len);
void wireless_get_IMSI(char *pRecv, uint8 *pBuf, uint8 buflen);
int wireless_get_IMEI(uint8 ifnum, void *pDev, char *pBuf, uint8 Len);
int wireless_check_newwork_reg(uint8 ifnum, void *pDev, uint8 bLog);

#ifdef __cplusplus
}
#endif

#endif //__AT_OPER_H__


