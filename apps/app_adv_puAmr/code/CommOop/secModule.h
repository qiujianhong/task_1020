/*
*********************************************************************
* @filename: secModule.h
* @brief   : 安全软算法模块
* @describe: 
* @history : 
*********************************************************************
*/

#ifndef __SEC_MODULE_H
#define __SEC_MODULE_H

#define SEND_APDU_LEN   (4096 - 50)
#define SEND_BUF_LEN    4096
#define RECV_APDU_LEN   (4096 - 50)
#define RECV_BUF_LEN    4096

uint32 make_apdu_to_package(OOP_TSA_T tsa, uint8 *apdu, uint32 apduLen, uint8 *outMsg, uint32 maxLen);

int get_extModule_devid_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen);
int get_extModule_devid_parse(uint8 *rcvbuf, uint32 buflen, uint8 *devid, uint32 *idlen);
int get_esam_factory_ik(uint8 *devid, uint32 idlen, uint8 *ik, uint32 *iklen);
int set_terminal_id_and_factory_ik(uint8 *devid, uint32 idlen, uint8 *ik, uint32 iklen);
int act_extModule_init_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen);
int act_extModule_init_parse(uint8 *rcvbuf, uint32 buflen);
int get_extModule_keyStatus_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen);
int get_extModule_keyStatus_parse(uint8 *rcvbuf, uint32 buflen, uint8 *keyStatus);
int update_terminal_keyStatus(uint8 modKeyStatus, uint8 *devid, uint32 idlen);
int negotiate_begin_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen);
int negotiate_begin_parse(uint8 *rcvbuf, uint32 buflen, uint8 *cRanCat, uint32 *cRanCat_len);
int negotiate_end_packaged(OOP_TSA_T tsa, uint8 *cRanCat, uint32 cRanCat_len, uint8 *sendbuf, uint32 *buflen);
int negotiate_end_parse(uint8 *rcvbuf, uint32 buflen);
int cipher_encryption_packaged(OOP_TSA_T tsa, uint8 *apdu, uint32 apduLen, uint8 isMac, uint8 *sendbuf, uint32 *buflen);
int cipher_encryption_parse(uint8 *rcvbuf, uint32 buflen, uint8 *apdu, uint32 *apduLen);
int plainMac_encryption_packaged(OOP_TSA_T tsa, uint8 *apdu, uint32 apduLen, uint8 *sendbuf, uint32 *buflen);
int plainMac_encryption_parse(uint8 *rcvbuf, uint32 buflen, uint8 *apdu, uint32 *apduLen);
int plainRN_encryption_packaged(OOP_TSA_T tsa, uint8 *apdu, uint32 apduLen, uint8 *sendbuf, uint32 *buflen);
int plainRN_encryption_parse(uint8 *rcvbuf, uint32 buflen, uint8 *apdu, uint32 *apduLen);
int plainRN_MAC_encryption_parse(uint8 *rcvbuf, uint32 buflen, uint8 *apdu, uint32 *apduLen);
int plainRN_MAC_encryption_packaged(OOP_TSA_T tsa, uint8 *apdu, uint32 apduLen, uint8 *sendbuf, uint32 *buflen);
int act_extModule_key_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen);
int act_extModule_key_parse(uint8 *rcvbuf, uint32 buflen);

int sm_act_msg_package(OOP_TSA_T tsa, OOP_OMD_U omd, uint8* inData, uint32 inLen, uint8* sendbuf, uint32 bufLenMax);
int sm_get_msg_package(OOP_TSA_T tsa, OOP_OAD_U oad, uint8* sendbuf, uint32 bufLenMax);
int get_extModule_yx_st_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen);
int get_extModule_yx_st_fg_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen);
int get_extModule_yx_st_parse(uint8 *rcvbuf, uint32 buflen, OOP_SWITCHIN_STATE_T *st);
int get_extModule_yx_fg_packaged(OOP_TSA_T tsa, uint8 *sendbuf, uint32 *buflen);
int get_extModule_yx_fg_parse(uint8 *rcvbuf, uint32 buflen, OOP_SWITCHIN_FLAG_T *fg);

void init_extModule_callback_fun(void);

#endif

