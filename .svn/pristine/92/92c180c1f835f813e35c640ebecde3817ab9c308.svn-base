/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 安全软算法模块依赖接口
* @date：   2022-05-13
*********************************************************************
*/

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "light_sm.h"
#include <stdio.h>
#include "OopAll.h"
#include "framecomm.h"
#include "sm4Callback.h"

#if DESC("安全软算法库依赖接口", 1)
#define RANDOM_MIX 1

static uint8 flash_sim[1536];

#ifndef RANDOM_MIX
static uint32 g_Counter = 0;
#endif

BOOLEAN Flash_clean_long(uint32 addr, uint32 len)
{
    uint32 i;
    for (i = 0; i < (4*len); i++) 
    {
        flash_sim[addr+i] = 0;
    }
    return TRUE;
}

BOOLEAN Flash_clean_bytes(uint32 addr, uint32 len)
{
    uint32 i;
    for (i = 0; i < len; i++) 
    {
        flash_sim[addr + i] = 0;
    }
    return TRUE;
}

static uint32 Flash_getWord(uint32 addr, uint32 offset)
{
    return *((uint32 *)(flash_sim + addr) + offset);
}
static BOOLEAN Flash_setWord(uint32 addr, uint32 src)
{
    memcpy(flash_sim+addr, &src, 4);
    return TRUE;
}

static uint32 Flash_getData_byte(uint32 addr, uint32 offset)
{
    return flash_sim[addr + offset];
}

static BOOLEAN Flash_WriteByte(uint32 addr, uint8 src)
{
    memcpy(flash_sim+addr, &src, 1);
    return TRUE;
}

BOOLEAN Flash_set_zero_long(uint32 addr, uint32 len)
{
    uint32 i;
    for(i = 0; i < len; i++) 
    {
        if (TRUE == Flash_setWord(addr + 4 * i, 0x00)) 
        {
            if (Flash_getWord(addr, i) != 0x00)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

BOOLEAN Flash_set_data_long(uint32 addr, uint32 len, uint32 *arr)
{
    uint32 i;
    for(i = 0; i < len; i++) 
    {
        if (TRUE == Flash_setWord(addr + 4 * i, arr[i])) 
        {
            if (Flash_getWord(addr, i) != arr[i])
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

void Flash_get_data_long(uint32 addr, uint32 len, uint32 *arr)
{
    uint32 i;
    for (i = 0; i < len; i++) 
    {
        arr[i] = Flash_getWord(addr, i);
    }
}

BOOLEAN Flash_set_data_byte(uint32 addr, uint32 len, uint8 *arr)
{
    uint32 i;
    for (i = 0; i < len; i++) 
    {
        if (TRUE == Flash_WriteByte(addr + i, arr[i])) 
        {
            if (Flash_getData_byte(addr, i) != arr[i])
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

void Flash_get_data_byte(uint32 addr, uint32 len, uint8 *arr)
{
    uint32 i;
    for (i = 0; i < len; i++) 
    {
        arr[i] = Flash_getData_byte(addr, i);
    }
}

BOOLEAN my_getRand(uint32 *ranL, uint32 len) 
{
    uint32 i = 0;
    
#ifdef RANDOM_MIX
    uint32 test[4]={0x87654321,0x87654321,0x87654321,0x87654321};
    for(i = 0; i < len; i++) {
        ranL[i] = test[i%4];
    }
#else
    srand(g_Counter++);
    for(i = 0; i < len; i++) {
        ranL[i] = (((uint8)rand()) << 24) | (((uint8)rand()) << 16) | (((uint8)rand()) << 8) | ((uint8)rand());
    }
#endif
    return TRUE;
}

#if 1
BOOLEAN my_getDataIKB(uint8 *id, uint32 idlen, uint8 *ikb, uint32 *ikblen) 
{
    if (ESAM_OK == GetTEsam_IKA_Info(id, idlen, ikb, ikblen))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
BOOLEAN my_getDataIKC(uint8 *id, uint32 idlen, uint8 *ikc, uint32 *ikclen) 
{
    if (ESAM_OK == GetTEsam_IKB_Info(id, idlen, ikc, ikclen))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
BOOLEAN my_getDataIKA(uint8 *id, uint32 idlen, uint8 *ika, uint32 *ikalen) 
{
    if (ESAM_OK == GetTEsam_IKC_Info(id, idlen, ika, ikalen))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
BOOLEAN my_getDataSIKB(uint8 *id, uint32 idlen, uint8 *sikb, uint32 *sikblen)
{
    if (ESAM_OK == GetTEsam_SIKC_Info(id, idlen, sikb, sikblen))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
BOOLEAN my_getDataSIKC(uint8 *id, uint32 idlen, uint8 *sikc, uint32 *sikclen)
{
    if (ESAM_OK == GetTEsam_SIKC_Info(id, idlen, sikc, sikclen))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
BOOLEAN my_getDataSIKA(uint8 *id, uint32 idlen, uint8 *sika, uint32 *sikalen) 
{
    if (ESAM_OK == GetTEsam_SIKC_Info(id, idlen, sika, sikalen))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#else
BOOLEAN my_getDataIKB(uint8_t *id, uint32_t idlen, uint8_t *ikb, uint32_t *ikblen) {

	uint8_t ret = 0;
	uint8_t port = 0x01;
	uint8_t sBuf[17] = { 0x80, 0x0A, 0x00, 0x8B, 0x00, 0x0B,
		0x01, 0x0B,0x00,
		//SN_ID[8]
	};
	uint8_t rBuf[20] = { 0x90, 0x00, 0x00, 0x10,
		//DATA[16]
	};

	memcpy(sBuf + 9, id, idlen);

	//发送：800A008B000B + 010B00 + SN_ID[8]
	ret = write(port, sBuf, sizeof(sBuf));
	if (ret != 0) {
		//error
	}
	//返回：9000+0010+DATA[16]
	ret = read(port, rBuf, sizeof(rBuf));
	if (ret != 0) {
		//error
	}

	memcpy(ikb, rBuf + 4, 16);
	*ikblen = 16;
	return ret;
}
BOOLEAN my_getDataIKC(uint8_t *id, uint32_t idlen, uint8_t *ikc, uint32_t *ikclen) {

	uint8_t ret = 0;
	uint8_t port = 0x01;
	uint8_t sBuf[17] = { 0x80, 0x0A, 0x00, 0x8B, 0x00, 0x0B,
		0x01, 0x0B,0x00,
		//SN_ID[8]
	};
	uint8_t rBuf[20] = { 0x90, 0x00, 0x00, 0x10,
		//DATA[16]
	};

	memcpy(sBuf + 9, id, idlen);

	//发送：800A008B000B + 010B00 + SN_ID[8]
	ret = write(port, sBuf, sizeof(sBuf));
	if (ret != 0) {
		//error
	}
	//返回：9000+0010+DATA[16]
	ret = read(port, rBuf, sizeof(rBuf));
	if (ret != 0) {
		//error
	}

	memcpy(ikc, rBuf + 4, 16);
	*ikclen = 16;
	return ret;
}
BOOLEAN my_getDataIKA(uint8_t *id, uint32_t idlen, uint8_t *ika, uint32_t *ikalen) {

	uint8_t ret = 0;
	uint8_t port = 0x01;
	uint8_t sBuf[17] = { 0x80, 0x0A, 0x00, 0x8B, 0x00, 0x0B,
		0x01, 0x0B,0x00,
		//SN_ID[8]
	};
	uint8_t rBuf[20] = { 0x90, 0x00, 0x00, 0x10,
		//DATA[16]
	};

	memcpy(sBuf + 9, id, idlen);

	//发送：800A008B000B + 010B00 + SN_ID[8]
	ret = write(port, sBuf, sizeof(sBuf));
	if (ret != 0) {
		//error
	}
	//返回：9000+0010+DATA[16]
	ret = read(port, rBuf, sizeof(rBuf));
	if (ret != 0) {
		//error
	}

	memcpy(ika, rBuf + 4, 16);
	*ikalen = 16;
	return ret;
}
BOOLEAN my_getDataSIKB(uint8_t *id, uint32_t idlen, uint8_t *sikb, uint32_t *sikblen) {

	uint8_t ret = 0;
	uint8_t port = 0x01;
	uint8_t sBuf[17] = { 0x80, 0x0A, 0x00, 0x8B, 0x00, 0x0B,
		0x01, 0x0B,0x00,
		//SN_ID[8]
	};
	uint8_t rBuf[20] = { 0x90, 0x00, 0x00, 0x10,
		//DATA[16]
	};

	memcpy(sBuf + 9, id, idlen);

	//发送：800A008B000B + 010B00 + SN_ID[8]
	ret = write(port, sBuf, sizeof(sBuf));
	if (ret != 0) {
		//error
	}
	//返回：9000+0010+DATA[16]
	ret = read(port, rBuf, sizeof(rBuf));
	if (ret != 0) {
		//error
	}

	memcpy(sikb, rBuf + 4, 16);
	*sikblen = 16;
	return ret;
}
BOOLEAN my_getDataSIKC(uint8_t *id, uint32_t idlen, uint8_t *sikc, uint32_t *sikclen) {

	uint8_t ret = 0;
	uint8_t port = 0x01;
	uint8_t sBuf[17] = { 0x80, 0x0A, 0x00, 0x8B, 0x00, 0x0B,
		0x01, 0x0B,0x00,
		//SN_ID[8]
	};
	uint8_t rBuf[20] = { 0x90, 0x00, 0x00, 0x10,
		//DATA[16]
	};

	memcpy(sBuf + 9, id, idlen);

	//发送：800A008B000B + 010B00 + SN_ID[8]
	ret = write(port, sBuf, sizeof(sBuf));
	if (ret != 0) {
		//error
	}
	//返回：9000+0010+DATA[16]
	ret = read(port, rBuf, sizeof(rBuf));
	if (ret != 0) {
		//error
	}

	memcpy(sikc, rBuf + 4, 16);
	*sikclen = 16;
	return ret;
}
BOOLEAN my_getDataSIKA(uint8_t *id, uint32_t idlen, uint8_t *sika, uint32_t *sikalen) {

	uint8_t ret = 0;
	uint8_t port = 0x01;
	uint8_t sBuf[17] = { 0x80, 0x0A, 0x00, 0x8B, 0x00, 0x0B,
		0x01, 0x0B,0x00,
		//SN_ID[8]
	};
	uint8_t rBuf[20] = { 0x90, 0x00, 0x00, 0x10,
		//DATA[16]
	};

	memcpy(sBuf + 9, id, idlen);

	//发送：800A008B000B + 010B00 + SN_ID[8]
	ret = write(port, sBuf, sizeof(sBuf));
	if (ret != 0) {
		//error
	}
	//返回：9000+0010+DATA[16]
	ret = read(port, rBuf, sizeof(rBuf));
	if (ret != 0) {
		//error
	}

	memcpy(sika, rBuf + 4, 16);
	*sikalen = 16;
	return ret;
}
#endif

#endif
    


