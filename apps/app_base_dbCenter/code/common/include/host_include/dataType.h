/*
*********************************************************************
* Copyright(C) 2018 China Electric Power Research Institute
* All rights reserved. 
* @brief：   基础数据类型定义.
* @date：    2018/09/10
* @history： 
*********************************************************************
*/

#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif 

#undef bool
#undef uchar
#undef uint8
#undef int8
#undef uint16
#undef int16
#undef uint32
#undef int32
#undef uint64
#undef int64
#undef float32
#undef float64

#ifndef __cplusplus
#ifndef bool
#define	bool    unsigned char
#endif
#endif

#ifndef uchar
#define uchar unsigned char                             /* uchar*/
#endif

#ifndef uint8
#define uint8    unsigned char                          /* uint8 */
#endif

#ifndef int8
#define int8    signed char                             /* int8 */
#endif

#ifndef uint16
#define uint16    unsigned short                        /* uint16 */
#endif

#ifndef int16
#define int16    short                                  /* int16 */
#endif

#ifndef uint32
#define uint32    unsigned int                          /* uint32 */
#endif

#ifndef int32
#define int32    int                                    /* int32*/
#endif

#ifndef uint64
#define uint64    unsigned long long                   /* unsigned long long */
#endif

#ifndef int64
#define int64    long long                             /* long long */
#endif

#ifndef float32
#define float32    float                               /* float */
#endif

#ifndef float64
#define float64    double                              /* double */
#endif

#ifndef HANDLE
#define HANDLE	void*                                  /* void */
#endif

#ifndef NULL
#define NULL	0                                      /* NULL */
#endif

#ifndef TRUE
#define TRUE	1                                      /*TRUE value*/
#endif

#ifndef FALSE
#define FALSE	0                                      /*FALSE value*/
#endif

#ifndef _BOOL_DEFINED
#define _BOOL_DEFINED
typedef	int	BOOL;                                      /*BOOL*/
#endif

#ifndef BOOLEAN
#define 	BOOLEAN		uint8                          /*boolean*/
#endif

#ifndef INFINITE
#define INFINITE			(0xFFFFFFFF)
#endif

#ifndef NONECHAR
#define NONECHAR        0xff                            /*字节型无效数据类型*/
#endif

#ifndef NONEWORD
#define NONEWORD        0xffff                          /*双字节型无效数据类型*/
#endif

#ifndef NONELWORD
#define NONELWORD       0xffffffffL                     /*四字节型无效数据类型*/
#endif

#ifndef NONELLWORD
#define NONELLWORD      0xffffffffffffffffLL            /*八字节型无效数据类型*/
#endif

#ifndef NONE_CHAR
#define NONE_CHAR       0xff                            /*字节型无效数据类型*/
#endif

#ifndef NONE_WORD
#define NONE_WORD       0xffff                          /*双字节型无效数据类型*/
#endif

#ifndef NONE_LWORD
#define NONE_LWORD      0xffffffffL                     /*四字节型无效数据类型*/
#endif

#ifndef NONE_LLWORD
#define NONE_LLWORD     0xffffffffffffffffLL            /*八字节型无效数据类型*/
#endif

#ifndef APP_NONECHAR
#define APP_NONECHAR    0xee                            /*字节型无效数据类型*/
#endif

#ifndef APP_NONEWORD
#define APP_NONEWORD    0xeeee                          /*双字节型无效数据类型*/
#endif

#ifndef APP_NONELWORD
#define APP_NONELWORD   0xeeeeeeeeL                     /*四字节型无效数据类型*/
#endif

#ifndef APP_NONELLWORD
#define APP_NONELLWORD  0xeeeeeeeeeeeeeeeeLL            /*八字节型无效数据类型*/
#endif

#ifndef WIN32
#define __noop				((void)0)
#endif

typedef union 
{
	unsigned char uc[4];
	unsigned int ul;
} varul_t;

typedef union 
{
	unsigned char uc[2];
	unsigned short us;
} varus_t;

typedef union
{
    uint16  u16;
    int16   s16;
    uint8   u8_2[2];
}INT16_8_TYPE;                                 //2字节整形共用类型

typedef union
{
    uint32  u32;
    int32   s32;
    uint8   u8_4[4];
}INT32_8_TYPE;                            //4字节长整形共用类型

typedef union
{
    uint64  u64;
    int64   s64;
    uint8   u8_8[8];
}INT64_8_TYPE;                            //4字节长整形共用类型


#ifdef __cplusplus
}
#endif

#endif // __DATA_TYPE_H__
