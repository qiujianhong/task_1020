/*
*********************************************************************
* @filename: secModule.h
* @brief   : 安全软算法模块,依赖接口
* @describe: 
* @history : 
*********************************************************************
*/

#ifndef __SM4_CALLBACK_H
#define __SM4_CALLBACK_H

BOOLEAN Flash_clean_long(uint32 addr, uint32 len);
BOOLEAN Flash_clean_bytes(uint32 addr, uint32 len);
BOOLEAN Flash_set_zero_long(uint32 addr, uint32 len);
BOOLEAN Flash_set_data_long(uint32 addr, uint32 len, uint32 *arr);
void Flash_get_data_long(uint32 addr, uint32 len, uint32 *arr);
BOOLEAN Flash_set_data_byte(uint32 addr, uint32 len, uint8 *arr);
void Flash_get_data_byte(uint32 addr, uint32 len, uint8 *arr);

BOOLEAN my_getRand(uint32 *ranL, uint32 len);
BOOLEAN my_getDataIKA(uint8 *id, uint32 idlen, uint8 *ika, uint32 *ikalen);
BOOLEAN my_getDataIKB(uint8 *id, uint32 idlen, uint8 *ikb, uint32 *ikblen);
BOOLEAN my_getDataIKC(uint8 *id, uint32 idlen, uint8 *ikc, uint32 *ikclen);
BOOLEAN my_getDataSIKA(uint8 *id, uint32 idlen, uint8 *sika, uint32 *sikalen);
BOOLEAN my_getDataSIKB(uint8 *id, uint32 idlen, uint8 *sikb, uint32 *sikblen);
BOOLEAN my_getDataSIKC(uint8 *id, uint32 idlen, uint8 *sikc, uint32 *sikclen);


#endif


