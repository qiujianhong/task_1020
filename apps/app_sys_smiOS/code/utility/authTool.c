/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： authTool.c
* @date： 2019-11-9
*********************************************************************
*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "authTool.h"
#include "constant.h"
#include "openssl/bn.h"
#include "openssl/md5.h"
#include "framecomm.h"

/**
*********************************************************************
* @brief： 创建接入的token
* @param[in]： none
* @param[out]：token_out
* @return：    0
*********************************************************************
*/
int creat_access_token(char *token_out)
{
    BIGNUM *       bn     = NULL;
    // struct timeval now    = {0};
    int            bits   = 128;
    int            top    = -1;
    int            bottom = 1;
    char *         out    = NULL;


    // gettimeofday(&now, NULL);

    bn = BN_new();
    BN_rand(bn, bits, top, bottom);
    out = BN_bn2hex(bn);
    AUTH_FMT_DEBUG("out :%s",out);
    // snprintf(token_out, TOKEN_MAX_LEN, "%ld_%s",
    //          now.tv_sec + MAX_TOKEN_VALID_TIME, out);
    snprintf(token_out, TOKEN_MAX_LEN, "%s",out);
    BN_free(bn);
    return 0;
}

/**
*********************************************************************
* @brief： 检查token有效性
* @param[in]： token_out
* @param[out]：none
* @return：    0    success
              -1   fail
*********************************************************************
*/
int check_token(char *token_out)
{
    long int valid_time = 0;
    struct timespec now        = {0};

    AUTH_FMT_DEBUG("check_token:%s", token_out);
    sscanf(token_out, "%ld_%*s", &valid_time);
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    if (valid_time >= now.tv_sec)
    {
        return 0;
    }
    return -1;
}

/**
*********************************************************************
* @brief： 生存md5 password
* @param[in]： pin
* @param[out]：pout
* @return：    0
*********************************************************************
*/
int md5_password(char *pin, char *pout)
{
    unsigned char MD5result[16];
    MD5_CTX       md5_ctx;
    MD5_Init(&md5_ctx);
    MD5_Update(&md5_ctx, pin, strlen(pin));
    MD5_Final(MD5result, &md5_ctx);
    sprintf(pout,
            "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            MD5result[0], MD5result[1], MD5result[2], MD5result[3],
            MD5result[4], MD5result[5], MD5result[6], MD5result[7],
            MD5result[8], MD5result[9], MD5result[10], MD5result[11],
            MD5result[12], MD5result[13], MD5result[14], MD5result[15]);
    return 0;
}
