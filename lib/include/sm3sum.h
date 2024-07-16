/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   计算SM3值
* @date：    2024-6-1
* @history： 
* @User deatil:
*********************************************************************
*/

#ifndef __SM3_SUM_H_
#define __SM3_SUM_H_


#ifdef __cplusplus
    extern "C" {
#endif

#define LK_GVALUE_LEN       64 
#define LK_WORD_SIZE        32 
#define LK_GVALUE_BITLEN    256
#define LK_HASH_NMEMB       8

//常量
// 0 <= j <= 15
#define LK_T0   0x79cc4519
// 16 <= j <= 63
#define LK_T1   0x7a879d8a
//循环左移
#define LOOPSHFT(a, n) ( ((a) << (n)) | ((a) >> (LK_WORD_SIZE - (n))))
//布尔函数
#define LK_FF0(x, y, z) ((x)^(y)^(z))
#define LK_FF1(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define LK_GG0(x, y, z) ((x)^(y)^(z))
#define LK_GG1(x, y, z) (( (x) & (y) ) | ((~x) & (z)))

//置换函数
#define LK_P0(x) (\
    (x)^(LOOPSHFT((x), 9))^(LOOPSHFT((x), 17)) )
#define LK_P1(x) (\
    (x)^(LOOPSHFT((x), 15))^(LOOPSHFT((x), 23)) )

//标准中给出的IV初始值
#define LK_INIT_VALUE(t) {\
    lk_sm3_context_t *x = (t);\
    x->v[0] = 0x7380166f;\
    x->v[1] = 0x4914b2b9;\
    x->v[2] = 0x172442d7;\
    x->v[3] = 0xda8a0600;\
    x->v[4] = 0xa96f30bc;\
    x->v[5] = 0x163138aa;\
    x->v[6] = 0xe38dee4d;\
    x->v[7] = 0xb0fb0e4e;\
    bzero(x->data, LK_GVALUE_LEN);\
    x->total = 0;\
    x->len = 0;}

#define LK_LE_ONE(t) {\
    lk_sm3_context_t *x = (t);\
    unsigned int l_z, l_d;\
    for (l_z = 0; l_z < LK_HASH_NMEMB; l_z++) {\
        l_d = x->v[l_z];\
        x->output[l_z*4] = ((l_d >> 24) & 0x000000ff);\
        x->output[l_z*4 + 1] = ((l_d >> 16) & 0x000000ff);\
        x->output[l_z*4 + 2] = ((l_d >> 8) & 0x000000ff);\
        x->output[l_z*4 + 3] = (l_d & 0x000000ff);\
    }}

//大端转化
#define LK_GE_ONE(c) (\
    ((c&0x00000000000000ffUL) << 56) | (((c&0x000000000000ff00UL) << 40)) |\
    ((c&0x0000000000ff0000UL) << 24) | (((c&0x00000000ff000000UL) << 8)) |\
    ((c&0x000000ff00000000UL)  >> 8) | (((c&0x0000ff0000000000UL) >> 24)) |\
    ((c&0x00ff000000000000UL) >> 40) | (((c&0xff00000000000000UL) >> 56)) )
#define LK_GE(w, c) \
    int j2;\
    for (j = 0; j <= 15; j++) {\
        j2 = j*4;\
        w[j] = ((c[j2] << 24) | ((c[j2+1] << 16)) |\
         (c[j2+2] << 8) | (c[j2+3]));\
    }


//压缩计算摘要函数
#define LK_MSG_CF(t) {\
    unsigned int j;\
    lk_sm3_context_t *x = t;\
    unsigned char *data = x->data;\
    unsigned int W1[68];\
    unsigned int W2[64];\
    unsigned int a,b,c,d,e,f,g,h;\
    a = x->v[0];\
    b = x->v[1];\
    c = x->v[2];\
    d = x->v[3];\
    e = x->v[4];\
    f = x->v[5];\
    g = x->v[6];\
    h = x->v[7];\
    LK_GE(W1, data)\
for ( j = 16; j <= 67; j++ ) {\
        W1[j] = LK_P1(W1[j-16]^W1[j-9]^(LOOPSHFT(W1[j-3], 15))) ^ LOOPSHFT(W1[j-13], 7) ^ W1[j-6];\
    }\
    for ( j = 0; j <= 63; j++ ) {\
        W2[j] = W1[j] ^ W1[j+4];\
    }\
    for ( j = 0; j <= 63; j++ ) {\
       unsigned int T, ss1, ss2, tt1, tt2;\
       if ( j >= 0 && j <= 15 )\
           T = LK_T0;\
       else\
           T = LK_T1;\
        ss1 = LOOPSHFT( (LOOPSHFT(a, 12) + e + LOOPSHFT(T, j)), 7 );\
        ss2 = ss1 ^ LOOPSHFT(a, 12);\
        if ( j >= 0 && j <= 15 ) {\
            tt1 = LK_FF0(a, b, c) + d + ss2 + W2[j];\
            tt2 = LK_GG0(e, f, g) + h + ss1 + W1[j];\
        } else {\
            tt1 = LK_FF1(a, b, c) + d + ss2 + W2[j];\
            tt2 = LK_GG1(e, f, g) + h + ss1 + W1[j];\
        }\
        d = c;\
        c = LOOPSHFT(b, 9);\
        b = a;\
        a = tt1;\
        h = g;\
        g = LOOPSHFT(f, 19);\
        f = e;\
        e = LK_P0(tt2);\
    }\
    x->v[0] = a ^ x->v[0];\
    x->v[1] = b ^ x->v[1];\
    x->v[2] = c ^ x->v[2];\
    x->v[3] = d ^ x->v[3];\
    x->v[4] = e ^ x->v[4];\
    x->v[5] = f ^ x->v[5];\
    x->v[6] = g ^ x->v[6];\
    x->v[7] = h ^ x->v[7];\
    x->len = 0;\
}

typedef struct lk_sm3_context_s
{
	unsigned int  len;
	unsigned int  total;
	unsigned char data[LK_GVALUE_LEN];
	unsigned int  v[LK_HASH_NMEMB];
	unsigned char output[LK_WORD_SIZE];
} lk_sm3_context_t;


/**********************************************************************
* @name      : GetFileSM3
* @brief     ：输入文件路径，获取SM3加密编码
* @param[in] ：const char   *filePath        待加密的文件
               unsigned int  sm3hexSize     *sm3hex的数组大小
* @param[out]：unsigned char *sm3hex         filepath计算得到的sm3编码
**********************************************************************/
int GetFileSM3(const char* filePath, unsigned int sm3hexSize, unsigned char *sm3hex);

/**********************************************************************
* @name      : GetFileSM3
* @brief     ：输入文件路径，计算[startoff, endOff]区间数据的sm3编码
* @param[in] ：const char   *filePath        待加密的文件
               unsigned int  startoff       起始偏移
               unsigned int  startoff       结束偏移
* @param[out]：unsigned char *pOutSm3        计算得到的sm3编码
       return：0加密失败，1加密成功
**********************************************************************/
int Sm3Region(char* pFileName, unsigned int startOff,unsigned int endOff,unsigned char *pOutSm3);

/**********************************************************************
* @name      : calc_sm3
* @brief     ：输入报文，获取SM3加密编码
* @param[in] ：const char    *pBuf            待加密报文
               int            nSize           报文长度
* @param[out]：unsigned char *pOut            sm3
**********************************************************************/
int calc_sm3(unsigned char *pBuf, int nSize, unsigned char *pOut);

#ifdef __cplusplus
}
#endif

#endif
