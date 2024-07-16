
/******************************************************************************/
/** \file light_sm.h
 ** \date 2020-04-20
 ** \ver. 1.4.2
 ******************************************************************************/

#ifndef __LIGHT_SM_H__
#define __LIGHT_SM_H__

#include <stdint.h>

typedef uint8_t boolean_t;
//#define SMS_WIN_NET_OPEN

#ifndef TRUE
#define TRUE    0x01
#endif

#ifndef FALSE
#define FALSE   0x00
#endif

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/

#define MIN_STORAGE 256

#define ID_LEN      8
#define PINFO_LEN      8


#define TEST_STAT	0x10
#define NORM_STAT	0x11

#define ERR_AUTH    0x0a
#define ERR_PAD     0x05
#define ERR_WRITE   0xFF

#define VERSION			0x51

/******************************************************************************
 * Global type definitions
 ******************************************************************************/

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/


#ifdef _WIN32

//win
#ifdef Dll_EXPORTS  
#define DllAPI __declspec(dllexport)
#else
#define DllAPI 
#endif

#ifdef CALL_BACK_EXPORT
# define CALL_BACK_FUNC __declspec(dllexport)
#else
# define CALL_BACK_FUNC 
//# define CALL_BACK_FUNC
#endif


//android
#elif defined ANDROID

//#else ifdef ANDROID
#define DllAPI
# define CALL_BACK_FUNC extern


//else for any system : linux\mcu..
#else



#define DllAPI
# define CALL_BACK_FUNC extern


#endif




#ifdef  __cplusplus
extern "C" {
#endif

#ifndef SMS_WIN_NET_OPEN
typedef boolean_t (*flash_set_zeros)(uint32_t addr, uint32_t len);
typedef boolean_t (*flash_set_words)(uint32_t addr, uint32_t len, uint32_t *arr);
typedef void (*flash_get_words)(uint32_t addr, uint32_t len, uint32_t *arr);

typedef boolean_t (*flash_set_bytes)(uint32_t addr, uint32_t len, uint8_t *arr);
typedef void (*flash_get_bytes)(uint32_t addr, uint32_t len, uint8_t *arr);

typedef boolean_t (*flash_clean_t)(uint32_t addr, uint32_t len);

typedef boolean_t (*random_gen_t)(uint32_t *ranL, uint32_t len);
typedef boolean_t(*ikb_gen_t)(uint8_t *id, uint32_t idlen, uint8_t *ikb, uint32_t *ikblen);
typedef boolean_t(*ikc_gen_t)(uint8_t *id, uint32_t idlen, uint8_t *ikc, uint32_t *ikclen);
typedef boolean_t(*ika_gen_t)(uint8_t *id, uint32_t idlen, uint8_t *ika, uint32_t *ikalen);
typedef boolean_t(*sikb_gen_t)(uint8_t *id, uint32_t idlen, uint8_t *sikb, uint32_t *sikblen);
typedef boolean_t(*sikc_gen_t)(uint8_t *id, uint32_t idlen, uint8_t *sikc, uint32_t *sikclen);
typedef boolean_t(*sika_gen_t)(uint8_t *id, uint32_t idlen, uint8_t *sika, uint32_t *sikalen);

CALL_BACK_FUNC flash_set_zeros callback_flash_setWords_zero;
CALL_BACK_FUNC flash_set_words callback_flash_setWords;
CALL_BACK_FUNC flash_get_words callback_flash_getWords;
CALL_BACK_FUNC flash_clean_t callback_flash_cleanWords;

CALL_BACK_FUNC flash_set_bytes callback_flash_setBytes;
CALL_BACK_FUNC flash_get_bytes callback_flash_getBytes;
CALL_BACK_FUNC flash_clean_t callback_flash_cleanBytes;

CALL_BACK_FUNC random_gen_t callback_getRandomWords;
CALL_BACK_FUNC ikb_gen_t callback_getDataIKB;
CALL_BACK_FUNC ikc_gen_t callback_getDataIKC;
CALL_BACK_FUNC ika_gen_t callback_getDataIKA;
CALL_BACK_FUNC sikb_gen_t callback_getDataSIKB;
CALL_BACK_FUNC sikc_gen_t callback_getDataSIKC;
CALL_BACK_FUNC sika_gen_t callback_getDataSIKA;

#endif

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/
#ifdef SMS_WIN_NET_OPEN
DllAPI boolean_t SetFileLoc(uint8_t* f_name, uint32_t rand_flag);
DllAPI boolean_t SetKeyDataLoc(uint8_t *IK, uint32_t IKlen, uint8_t *SIK, uint32_t SIKlen, uint8_t *SSK, uint32_t SSKlen);
#endif
DllAPI boolean_t SetStorageSector(uint32_t bgn, uint32_t bytelen);
DllAPI boolean_t SetID(uint8_t *devid, uint32_t idlen);
DllAPI uint8_t getStatus(uint8_t* devid, uint32_t idlen);
DllAPI uint8_t SM4SetIK(uint8_t* keySets);
DllAPI uint8_t setStatus(uint8_t status);
DllAPI uint8_t Master_initConnection(uint8_t *info, uint32_t infolen);
DllAPI uint8_t Master_initConnectionext(uint8_t *info, uint32_t infolen, uint8_t *outData, uint32_t *outLen);
DllAPI uint8_t setConnection(uint8_t* Pinfo, uint32_t PInfolen);
DllAPI uint8_t getPinfo(uint8_t* Pinfo_plain);
DllAPI uint8_t getConnection(uint8_t *Pinfo, uint32_t* PInfolen);
DllAPI uint8_t Master_SM4SetIK(uint8_t* keySets);
DllAPI uint8_t Master_SM4SetSIK(uint8_t* dataSIK_from_srv);
DllAPI uint8_t Master_SM4UpdateBegin( uint8_t* cRanA);
DllAPI boolean_t Master_SM4UpdateEnd(uint8_t cRanCat[48], uint32_t cRanCat_len, uint8_t *ciphB, uint32_t ciphB_len);
DllAPI uint8_t Master_SM4InitSIK(uint8_t* keyData);
DllAPI uint8_t Master_SM4Encrypt(uint8_t auth, uint8_t *inData, uint32_t inLen, uint8_t *outData, uint32_t *outLen);
DllAPI uint8_t Master_SM4Decrypt(uint8_t auth, uint8_t *inData, uint32_t inLen, uint8_t *outData, uint32_t *outLen);
DllAPI uint8_t Master_SM4Mac(uint8_t *inData, uint32_t inLen, uint8_t *macData, uint32_t *outLen);
DllAPI uint8_t Master_SM4MacVerify(uint8_t *inData, uint32_t inLen, uint8_t *macData, uint32_t outLen);
DllAPI uint8_t Master_SM4MacNoSession(uint8_t *inData, uint32_t inLen, uint8_t *ran, uint32_t ranLen, uint8_t *macData, uint32_t *outLen);// IK2 plaintext + mac
DllAPI uint8_t Master_SM4MacVerifyNoSession(uint8_t *inData, uint32_t inLen, uint8_t *macData, uint32_t outLen);// IK2 plaintext + mac

DllAPI boolean_t ivecReset(boolean_t rmEK);
DllAPI boolean_t statReset(void);
DllAPI uint8_t wrp_get_key(uint8_t status, uint8_t *devid, uint8_t* keyData);

DllAPI uint8_t version(void);

#ifdef  __cplusplus
}
#endif

#endif /* __LIGHT_SM_H__ */

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/



