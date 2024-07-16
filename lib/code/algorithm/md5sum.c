/*
**********************************************************
@文件名称: list.c
@功能说明: 链表操作
**********************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "md5sum.h"

/*MD5 -START-*/
typedef struct tagMD5CTX
{
    unsigned int count[2];
    unsigned int state[4];
    unsigned char buffer[64];
    unsigned char PADDING[64];
}MD5_CTX;


#define MD5_F(x,y,z) ((x & y) | (~x & z))
#define MD5_G(x,y,z) ((x & z) | (y & ~z))
#define MD5_H(x,y,z) (x^y^z)
#define MD5_I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))

#define FF(a,b,c,d,x,s,ac)      { a += MD5_F(b,c,d) + x + ac; a = ROTATE_LEFT(a,s); a += b; }
#define GG(a,b,c,d,x,s,ac)      { a += MD5_G(b,c,d) + x + ac; a = ROTATE_LEFT(a,s); a += b; }
#define HH(a,b,c,d,x,s,ac)      { a += MD5_H(b,c,d) + x + ac; a = ROTATE_LEFT(a,s); a += b; }
#define II(a,b,c,d,x,s,ac)      { a += MD5_I(b,c,d) + x + ac; a = ROTATE_LEFT(a,s); a += b; }
void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);
void MD5Final(MD5_CTX *context,unsigned char digest[16]);
void MD5Transform(unsigned int state[4],unsigned char block[64]);
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);

unsigned char MD5_PADDING[]= {0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


void MD5Init(MD5_CTX *context)
{
    context->count[0] = 0;
    context->count[1] = 0;
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;

    memcpy(context->PADDING, MD5_PADDING, 64);
}

void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen)
{
    unsigned int i = 0,index = 0,partlen = 0;
    index = (context->count[0] >> 3) & 0x3F;
    partlen = 64 - index;
    context->count[0] += inputlen << 3;
    if(context->count[0] < (inputlen << 3))
        context->count[1]++;
    context->count[1] += inputlen >> 29;

    if(inputlen >= partlen)
    {
        memcpy(&context->buffer[index],input,partlen);
        MD5Transform(context->state,context->buffer);
        for(i = partlen; i+64 <= inputlen; i+=64)
            MD5Transform(context->state,&input[i]);
        index = 0;
    }
    else
    {
        i = 0;
    }
    memcpy(&context->buffer[index],&input[i],inputlen-i);
}

void MD5Final(MD5_CTX *context,unsigned char digest[16])
{
    unsigned int index = 0,padlen = 0;
    unsigned char bits[8];
    index = (context->count[0] >> 3) & 0x3F;
    padlen = (index < 56)?(56-index):(120-index);
    MD5Encode(bits, context->count, 8);
    MD5Update(context, context->PADDING, padlen);
    MD5Update(context, bits, 8);
    MD5Encode(digest, context->state, 16);
}

void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len)
{
    unsigned int i = 0,j = 0;
    while(j < len)
    {
        output[j] = input[i] & 0xFF;
        output[j+1] = (input[i] >> 8) & 0xFF;
        output[j+2] = (input[i] >> 16) & 0xFF;
        output[j+3] = (input[i] >> 24) & 0xFF;
        i++;
        j+=4;
    }
}

void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len)
{
    unsigned int i = 0,j = 0;
    while(j < len)
    {
        output[i] = (input[j]) |
                    (input[j+1] << 8) |
                    (input[j+2] << 16) |
                    (input[j+3] << 24);
        i++;
        j+=4;
    }
}

void MD5Transform(unsigned int state[4], unsigned char block[64])
{
    unsigned int a = state[0];
    unsigned int b = state[1];
    unsigned int c = state[2];
    unsigned int d = state[3];
    unsigned int x[64];
    MD5Decode(x,block,64);
    FF(a, b, c, d, x[ 0], 7, 0xd76aa478); /* 1 */
    FF(d, a, b, c, x[ 1], 12, 0xe8c7b756); /* 2 */
    FF(c, d, a, b, x[ 2], 17, 0x242070db); /* 3 */
    FF(b, c, d, a, x[ 3], 22, 0xc1bdceee); /* 4 */
    FF(a, b, c, d, x[ 4], 7, 0xf57c0faf); /* 5 */
    FF(d, a, b, c, x[ 5], 12, 0x4787c62a); /* 6 */
    FF(c, d, a, b, x[ 6], 17, 0xa8304613); /* 7 */
    FF(b, c, d, a, x[ 7], 22, 0xfd469501); /* 8 */
    FF(a, b, c, d, x[ 8], 7, 0x698098d8); /* 9 */
    FF(d, a, b, c, x[ 9], 12, 0x8b44f7af); /* 10 */
    FF(c, d, a, b, x[10], 17, 0xffff5bb1); /* 11 */
    FF(b, c, d, a, x[11], 22, 0x895cd7be); /* 12 */
    FF(a, b, c, d, x[12], 7, 0x6b901122); /* 13 */
    FF(d, a, b, c, x[13], 12, 0xfd987193); /* 14 */
    FF(c, d, a, b, x[14], 17, 0xa679438e); /* 15 */
    FF(b, c, d, a, x[15], 22, 0x49b40821); /* 16 */

    /* Round 2 */
    GG(a, b, c, d, x[ 1], 5, 0xf61e2562); /* 17 */
    GG(d, a, b, c, x[ 6], 9, 0xc040b340); /* 18 */
    GG(c, d, a, b, x[11], 14, 0x265e5a51); /* 19 */
    GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa); /* 20 */
    GG(a, b, c, d, x[ 5], 5, 0xd62f105d); /* 21 */
    GG(d, a, b, c, x[10], 9,  0x2441453); /* 22 */
    GG(c, d, a, b, x[15], 14, 0xd8a1e681); /* 23 */
    GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8); /* 24 */
    GG(a, b, c, d, x[ 9], 5, 0x21e1cde6); /* 25 */
    GG(d, a, b, c, x[14], 9, 0xc33707d6); /* 26 */
    GG(c, d, a, b, x[ 3], 14, 0xf4d50d87); /* 27 */
    GG(b, c, d, a, x[ 8], 20, 0x455a14ed); /* 28 */
    GG(a, b, c, d, x[13], 5, 0xa9e3e905); /* 29 */
    GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8); /* 30 */
    GG(c, d, a, b, x[ 7], 14, 0x676f02d9); /* 31 */
    GG(b, c, d, a, x[12], 20, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH(a, b, c, d, x[ 5], 4, 0xfffa3942); /* 33 */
    HH(d, a, b, c, x[ 8], 11, 0x8771f681); /* 34 */
    HH(c, d, a, b, x[11], 16, 0x6d9d6122); /* 35 */
    HH(b, c, d, a, x[14], 23, 0xfde5380c); /* 36 */
    HH(a, b, c, d, x[ 1], 4, 0xa4beea44); /* 37 */
    HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9); /* 38 */
    HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60); /* 39 */
    HH(b, c, d, a, x[10], 23, 0xbebfbc70); /* 40 */
    HH(a, b, c, d, x[13], 4, 0x289b7ec6); /* 41 */
    HH(d, a, b, c, x[ 0], 11, 0xeaa127fa); /* 42 */
    HH(c, d, a, b, x[ 3], 16, 0xd4ef3085); /* 43 */
    HH(b, c, d, a, x[ 6], 23,  0x4881d05); /* 44 */
    HH(a, b, c, d, x[ 9], 4, 0xd9d4d039); /* 45 */
    HH(d, a, b, c, x[12], 11, 0xe6db99e5); /* 46 */
    HH(c, d, a, b, x[15], 16, 0x1fa27cf8); /* 47 */
    HH(b, c, d, a, x[ 2], 23, 0xc4ac5665); /* 48 */

    /* Round 4 */
    II(a, b, c, d, x[ 0], 6, 0xf4292244); /* 49 */
    II(d, a, b, c, x[ 7], 10, 0x432aff97); /* 50 */
    II(c, d, a, b, x[14], 15, 0xab9423a7); /* 51 */
    II(b, c, d, a, x[ 5], 21, 0xfc93a039); /* 52 */
    II(a, b, c, d, x[12], 6, 0x655b59c3); /* 53 */
    II(d, a, b, c, x[ 3], 10, 0x8f0ccc92); /* 54 */
    II(c, d, a, b, x[10], 15, 0xffeff47d); /* 55 */
    II(b, c, d, a, x[ 1], 21, 0x85845dd1); /* 56 */
    II(a, b, c, d, x[ 8], 6, 0x6fa87e4f); /* 57 */
    II(d, a, b, c, x[15], 10, 0xfe2ce6e0); /* 58 */
    II(c, d, a, b, x[ 6], 15, 0xa3014314); /* 59 */
    II(b, c, d, a, x[13], 21, 0x4e0811a1); /* 60 */
    II(a, b, c, d, x[ 4], 6, 0xf7537e82); /* 61 */
    II(d, a, b, c, x[11], 10, 0xbd3af235); /* 62 */
    II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb); /* 63 */
    II(b, c, d, a, x[ 9], 21, 0xeb86d391); /* 64 */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

/*MD5 -END-*/

int GetDataMD5(unsigned char *pData, unsigned int dataLen, unsigned char *pOutMd5)
{
    MD5_CTX md5Ctx;

    if(0 == pData || 0 == dataLen)
    {
        return 0;
    }
    MD5Init(&md5Ctx);
    MD5Update(&md5Ctx, pData, dataLen);
    MD5Final(&md5Ctx, pOutMd5);

    return 16;
}

/*MD5 -START-*/
#define F(x_md5, y_md5, z_md5) (((x_md5) & (y_md5)) | ((~x_md5) & (z_md5)))
#define G(x_md5, y_md5, z_md5) (((x_md5) & (z_md5)) | ((y_md5) & (~z_md5)))
#define H(x_md5, y_md5, z_md5) ((x_md5) ^ (y_md5) ^ (z_md5))
#define I(x_md5, y_md5, z_md5) ((y_md5) ^ ((x_md5) | (~z_md5)))
#define RL(x_md5, y_md5) (((x_md5) << (y_md5)) | ((x_md5) >> (32 - (y_md5))))           //x向左循环移y位
#define PP(x_md5) (x_md5<<24)|((x_md5<<8)&0xff0000)|((x_md5>>8)&0xff00)|(x_md5>>24)     //将x高低位互换,例如PP(aabbccdd)=ddccbbaa
//#define FF(a_md5, b_md5, c_md5, d_md5, x_md5, s_md5, ac_md5) a_md5 = b_md5 + (RL((a_md5 + F(b_md5,c_md5,d_md5) + x_md5 + ac_md5),s_md5))
//#define GG(a_md5, b_md5, c_md5, d_md5, x_md5, s_md5, ac_md5) a_md5 = b_md5 + (RL((a_md5 + G(b_md5,c_md5,d_md5) + x_md5 + ac_md5),s_md5))
//#define HH(a_md5, b_md5, c_md5, d_md5, x_md5, s_md5, ac_md5) a_md5 = b_md5 + (RL((a_md5 + H(b_md5,c_md5,d_md5) + x_md5 + ac_md5),s_md5))
//#define II(a_md5, b_md5, c_md5, d_md5, x_md5, s_md5, ac_md5) a_md5 = b_md5 + (RL((a_md5 + I(b_md5,c_md5,d_md5) + x_md5 + ac_md5),s_md5))

unsigned A_MD5,B_MD5,C_MD5,D_MD5,a_md5,b_md5,c_md5,d_md5,i_md5,x_md5[16];  		 		//i临时变量,len文件长,flen_md5[2]为64位二进制表示的文件初始长度

void md5(void)//MD5核心算法,供64轮
{
  a_md5=A_MD5,b_md5=B_MD5,c_md5=C_MD5,d_md5=D_MD5;

  /**//* Round 1 */
  FF (a_md5, b_md5, c_md5, d_md5, x_md5[ 0],  7, 0xd76aa478); /**//* 1 */
  FF (d_md5, a_md5, b_md5, c_md5, x_md5[ 1], 12, 0xe8c7b756); /**//* 2 */
  FF (c_md5, d_md5, a_md5, b_md5, x_md5[ 2], 17, 0x242070db); /**//* 3 */
  FF (b_md5, c_md5, d_md5, a_md5, x_md5[ 3], 22, 0xc1bdceee); /**//* 4 */
  FF (a_md5, b_md5, c_md5, d_md5, x_md5[ 4],  7, 0xf57c0faf); /**//* 5 */
  FF (d_md5, a_md5, b_md5, c_md5, x_md5[ 5], 12, 0x4787c62a); /**//* 6 */
  FF (c_md5, d_md5, a_md5, b_md5, x_md5[ 6], 17, 0xa8304613); /**//* 7 */
  FF (b_md5, c_md5, d_md5, a_md5, x_md5[ 7], 22, 0xfd469501); /**//* 8 */
  FF (a_md5, b_md5, c_md5, d_md5, x_md5[ 8],  7, 0x698098d8); /**//* 9 */
  FF (d_md5, a_md5, b_md5, c_md5, x_md5[ 9], 12, 0x8b44f7af); /**//* 10 */
  FF (c_md5, d_md5, a_md5, b_md5, x_md5[10], 17, 0xffff5bb1); /**//* 11 */
  FF (b_md5, c_md5, d_md5, a_md5, x_md5[11], 22, 0x895cd7be); /**//* 12 */
  FF (a_md5, b_md5, c_md5, d_md5, x_md5[12],  7, 0x6b901122); /**//* 13 */
  FF (d_md5, a_md5, b_md5, c_md5, x_md5[13], 12, 0xfd987193); /**//* 14 */
  FF (c_md5, d_md5, a_md5, b_md5, x_md5[14], 17, 0xa679438e); /**//* 15 */
  FF (b_md5, c_md5, d_md5, a_md5, x_md5[15], 22, 0x49b40821); /**//* 16 */

 /**//* Round 2 */
  GG (a_md5, b_md5, c_md5, d_md5, x_md5[ 1],  5, 0xf61e2562); /**//* 17 */
  GG (d_md5, a_md5, b_md5, c_md5, x_md5[ 6],  9, 0xc040b340); /**//* 18 */
  GG (c_md5, d_md5, a_md5, b_md5, x_md5[11], 14, 0x265e5a51); /**//* 19 */
  GG (b_md5, c_md5, d_md5, a_md5, x_md5[ 0], 20, 0xe9b6c7aa); /**//* 20 */
  GG (a_md5, b_md5, c_md5, d_md5, x_md5[ 5],  5, 0xd62f105d); /**//* 21 */
  GG (d_md5, a_md5, b_md5, c_md5, x_md5[10],  9, 0x02441453); /**//* 22 */
  GG (c_md5, d_md5, a_md5, b_md5, x_md5[15], 14, 0xd8a1e681); /**//* 23 */
  GG (b_md5, c_md5, d_md5, a_md5, x_md5[ 4], 20, 0xe7d3fbc8); /**//* 24 */
  GG (a_md5, b_md5, c_md5, d_md5, x_md5[ 9],  5, 0x21e1cde6); /**//* 25 */
  GG (d_md5, a_md5, b_md5, c_md5, x_md5[14],  9, 0xc33707d6); /**//* 26 */
  GG (c_md5, d_md5, a_md5, b_md5, x_md5[ 3], 14, 0xf4d50d87); /**//* 27 */
  GG (b_md5, c_md5, d_md5, a_md5, x_md5[ 8], 20, 0x455a14ed); /**//* 28 */
  GG (a_md5, b_md5, c_md5, d_md5, x_md5[13],  5, 0xa9e3e905); /**//* 29 */
  GG (d_md5, a_md5, b_md5, c_md5, x_md5[ 2],  9, 0xfcefa3f8); /**//* 30 */
  GG (c_md5, d_md5, a_md5, b_md5, x_md5[ 7], 14, 0x676f02d9); /**//* 31 */
  GG (b_md5, c_md5, d_md5, a_md5, x_md5[12], 20, 0x8d2a4c8a); /**//* 32 */

  /**//* Round 3 */
  HH (a_md5, b_md5, c_md5, d_md5, x_md5[ 5],  4, 0xfffa3942); /**//* 33 */
  HH (d_md5, a_md5, b_md5, c_md5, x_md5[ 8], 11, 0x8771f681); /**//* 34 */
  HH (c_md5, d_md5, a_md5, b_md5, x_md5[11], 16, 0x6d9d6122); /**//* 35 */
  HH (b_md5, c_md5, d_md5, a_md5, x_md5[14], 23, 0xfde5380c); /**//* 36 */
  HH (a_md5, b_md5, c_md5, d_md5, x_md5[ 1],  4, 0xa4beea44); /**//* 37 */
  HH (d_md5, a_md5, b_md5, c_md5, x_md5[ 4], 11, 0x4bdecfa9); /**//* 38 */
  HH (c_md5, d_md5, a_md5, b_md5, x_md5[ 7], 16, 0xf6bb4b60); /**//* 39 */
  HH (b_md5, c_md5, d_md5, a_md5, x_md5[10], 23, 0xbebfbc70); /**//* 40 */
  HH (a_md5, b_md5, c_md5, d_md5, x_md5[13],  4, 0x289b7ec6); /**//* 41 */
  HH (d_md5, a_md5, b_md5, c_md5, x_md5[ 0], 11, 0xeaa127fa); /**//* 42 */
  HH (c_md5, d_md5, a_md5, b_md5, x_md5[ 3], 16, 0xd4ef3085); /**//* 43 */
  HH (b_md5, c_md5, d_md5, a_md5, x_md5[ 6], 23, 0x04881d05); /**//* 44 */
  HH (a_md5, b_md5, c_md5, d_md5, x_md5[ 9],  4, 0xd9d4d039); /**//* 45 */
  HH (d_md5, a_md5, b_md5, c_md5, x_md5[12], 11, 0xe6db99e5); /**//* 46 */
  HH (c_md5, d_md5, a_md5, b_md5, x_md5[15], 16, 0x1fa27cf8); /**//* 47 */
  HH (b_md5, c_md5, d_md5, a_md5, x_md5[ 2], 23, 0xc4ac5665); /**//* 48 */

  /**//* Round 4 */
  II (a_md5, b_md5, c_md5, d_md5, x_md5[ 0],  6, 0xf4292244); /**//* 49 */
  II (d_md5, a_md5, b_md5, c_md5, x_md5[ 7], 10, 0x432aff97); /**//* 50 */
  II (c_md5, d_md5, a_md5, b_md5, x_md5[14], 15, 0xab9423a7); /**//* 51 */
  II (b_md5, c_md5, d_md5, a_md5, x_md5[ 5], 21, 0xfc93a039); /**//* 52 */
  II (a_md5, b_md5, c_md5, d_md5, x_md5[12],  6, 0x655b59c3); /**//* 53 */
  II (d_md5, a_md5, b_md5, c_md5, x_md5[ 3], 10, 0x8f0ccc92); /**//* 54 */
  II (c_md5, d_md5, a_md5, b_md5, x_md5[10], 15, 0xffeff47d); /**//* 55 */
  II (b_md5, c_md5, d_md5, a_md5, x_md5[ 1], 21, 0x85845dd1); /**//* 56 */
  II (a_md5, b_md5, c_md5, d_md5, x_md5[ 8],  6, 0x6fa87e4f); /**//* 57 */
  II (d_md5, a_md5, b_md5, c_md5, x_md5[15], 10, 0xfe2ce6e0); /**//* 58 */
  II (c_md5, d_md5, a_md5, b_md5, x_md5[ 6], 15, 0xa3014314); /**//* 59 */
  II (b_md5, c_md5, d_md5, a_md5, x_md5[13], 21, 0x4e0811a1); /**//* 60 */
  II (a_md5, b_md5, c_md5, d_md5, x_md5[ 4],  6, 0xf7537e82); /**//* 61 */
  II (d_md5, a_md5, b_md5, c_md5, x_md5[11], 10, 0xbd3af235); /**//* 62 */
  II (c_md5, d_md5, a_md5, b_md5, x_md5[ 2], 15, 0x2ad7d2bb); /**//* 63 */
  II (b_md5, c_md5, d_md5, a_md5, x_md5[ 9], 21, 0xeb86d391); /**//* 64 */

  A_MD5 += a_md5;
  B_MD5 += b_md5;
  C_MD5 += c_md5;
  D_MD5 += d_md5;
}

//对文件中的某段数据区域计算Md5值,endOff为0xffffffff,表示文件末尾
int Md5Region(char* pFileName, unsigned int startOff,unsigned int endOff,unsigned char *pOutMd5)
{
	unsigned int len_md5,flen_md5[2];

	FILE *fp;
	if (!(fp=fopen(pFileName,"rb"))) //以二进制打开文件
	{
		printf("Can not open this file!\n");
		return 0;
	}

	fseek(fp, 0, SEEK_END);         //文件指针转到文件末尾
	if((len_md5=ftell(fp))==-1)
	{
		printf("Sorry! Can not calculate files which larger than 2 GB!\n");
		fclose(fp);
		return 0;
	}

	//endOff为0xffffffff,表示文件末尾
	if(endOff == 0xffffffff)
	{
		endOff = len_md5;
	}

    //判断所选数据段的合法性
    printf("文件大小: %d B Md5计算区域为: %d - %d B\n", len_md5, startOff, endOff);
    if(startOff > len_md5 || endOff > len_md5 || startOff > endOff)
    {
        printf("Region selected is illegal !\n");
        return 0;
    }

	rewind(fp);                         //文件指针复位到文件头
	len_md5 = endOff - startOff;	    //所选区域长度
	fseek(fp, startOff, SEEK_SET);      //文件指针指向所选区域开始处

	A_MD5=0x67452301,B_MD5=0xefcdab89,C_MD5=0x98badcfe,D_MD5=0x10325476; //初始化链接变量
	flen_md5[1]=len_md5/0x20000000;     //flen单位是bit
	flen_md5[0]=(len_md5%0x20000000)*8;
	memset(x_md5,0,64);                 //初始化x数组为0

	for(i_md5=0;i_md5<len_md5/64;i_md5++)//循环运算直至文件结束
	{
		memset(x_md5,0,64);
		fread(&x_md5,4,16,fp);
		md5();
	}

	memset(x_md5,0,64);
	fread(&x_md5,1,(len_md5%64),fp);
	((char*)x_md5)[len_md5%64]=128;     //文件结束补1,补0操作,128二进制即10000000
	if(len_md5%64>55) md5(),memset(x_md5,0,64);
	memcpy(x_md5+14,flen_md5,8);        //文件末尾加入原文件的bit长度
	md5();
	fclose(fp);

 	pOutMd5[0] = (PP(A_MD5))>>24,pOutMd5[1] = (PP(A_MD5))>>16,pOutMd5[2] = (PP(A_MD5))>>8,pOutMd5[3] = (PP(A_MD5))>>0;
	pOutMd5[4] = (PP(B_MD5))>>24,pOutMd5[5] = (PP(B_MD5))>>16,pOutMd5[6] = (PP(B_MD5))>>8,pOutMd5[7] = (PP(B_MD5))>>0;
	pOutMd5[8] = (PP(C_MD5))>>24,pOutMd5[9] = (PP(C_MD5))>>16,pOutMd5[10] = (PP(C_MD5))>>8,pOutMd5[11] = (PP(C_MD5))>>0;
	pOutMd5[12] = (PP(D_MD5))>>24,pOutMd5[13] = (PP(D_MD5))>>16,pOutMd5[14] = (PP(D_MD5))>>8,pOutMd5[15] = (PP(D_MD5))>>0;

    return 1;
}
/*MD5 -END-*/

