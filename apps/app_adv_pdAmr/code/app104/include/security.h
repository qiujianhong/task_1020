#ifndef _SECURITY_H_
#define _SECURITY_H_

#include "baseCommon.h"
#include "comm_port.h"

#define CERT_SEND_MAX_LEN       240    //证书上送时单帧证书数据长度
#define SECURITY_FRAME_MAX_LEN  1000   //最大帧长度
#define SECURITY_MAX_PIPE       MAX_CHANNEL_NUM     //最大支持的信道数
#define SECURITY_FRAME_FETCH    0      //解包前是否进行有效帧提取 0不提取 1提取

struct encryption_reg_req
{
	int   buflen;
	uint8* bufdata;
};

void security_enable(uint8 enable);
int frame_data_unpack(uint16 nFrameLen, uint8 *pFrameBuff, uint8 *pOutBuf, uint8 *pAppType);
int security_get_serialnum(uint8* pOutBuf);
int security_get_randnumsign(uint8* pInBuf, uint8* pOutBuf);
int security_chk_randnumsign(uint8* pInBuf, uint8* pOutBuf);
int security_get_keyversion(uint8* pOutBuf);
int security_update_key(uint8* pInBuf, uint8* pOutBuf);
int security_restore_key(uint8* pInBuf, uint8* pOutBuf);
int security_update_hostcert(uint8 cerID, uint8* cerbuf, uint16 cerlen, uint8* pOutBuf);
int security_update_mtucert(uint8 cerID, uint8* cerbuf, uint16 cerlen, uint8* pOutBuf);
int security_encryte(int pipe, uint16 len,uint8 *pinbuf,uint8 *poutbuf,uint8 bType);
int security_decryte(int pipe, uint8* pInBuf,uint16 bLen,uint8 *pOutBuf);
int security_unpackframe(int pipe, uint8 *pInPdu, int nInPduLen, uint8 *pOutBuf);
int security_packappframe(int pipe, uint8 appType, uint8* pInPdu, uint8 nInPduLen, uint8* pOutBuf);
int security_report(uint8* pPipe, uint8* pOutBuf);

#endif

