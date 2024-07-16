/*
*********************************************************************
* Copyright(C) 2018 China Electric Power Research Institute
* All rights reserved. 
* @brief：   基础宏定义
* @date：    2018/10/24
* @history： 
*********************************************************************
*/

#ifndef __MACRO_H__
#define __MACRO_H__

#ifdef __cplusplus
extern "C" {
#endif

#undef  MAKEWORD
#undef  MAKELONG
#undef  LOWORD
#undef  HIWORD
#undef  LOBYTE
#undef  HIBYTE
#undef    MIN
#undef    MAX
#undef    MEMCPY_S
#undef    MEMMOVE_S
#undef    MEMSET_S

#define MAKEWORD(l, h)      ((uint16)(((uint8)((l) & 0xff)) | ((uint16)((uint8)((h) & 0xff))) << 8))
#define MAKELONG(l, h)      ((uint32)((uint16)(l) & 0xffff) | ((uint32)((uint16)(h) & 0xffff)) << 16)
#define MAKEDWORD(hw, lw)   ((hw)<<16 | ((lw) & 0xffff))

#define LOWORD(l)           ((uint16)((uint32)(l) & 0xffff))
#define HIWORD(l)           ((uint16)((uint32)(l) >> 16))
#define LOBYTE(w)           ((uint8)((uint16)(w) & 0xff))
#define HIBYTE(w)           ((uint8)((uint16)(w) >> 8))

#define MIN(a, b)            (((a)<(b))?(a):(b))
#define MAX(a, b)            (((a)>(b))?(a):(b))

#define MAKE_SHORT(buf)        MAKEWORD(((unsigned char*)(buf))[0], ((unsigned char*)(buf))[1])


#define DEPART_SHORT(usv, buf) { \
    varus_t _tmp_varus; \
    _tmp_varus.us = usv; \
    ((unsigned char *)(buf))[0] = _tmp_varus.uc[0]; \
    ((unsigned char *)(buf))[1] = _tmp_varus.uc[1]; \
}

#define MAKE_LONG(buf)        MAKELONG(MAKEWORD(((unsigned char*)(buf))[0], ((unsigned char*)(buf))[1]),\
                                     MAKEWORD(((unsigned char*)(buf))[2], ((unsigned char*)(buf))[3]))

#define DEPART_LONG(ulv, buf) { \
    varul_t _tmp_varul; \
    _tmp_varul.ul = ulv; \
    ((unsigned char *)(buf))[0] = _tmp_varul.uc[0]; \
    ((unsigned char *)(buf))[1] = _tmp_varul.uc[1]; \
    ((unsigned char *)(buf))[2] = _tmp_varul.uc[2]; \
    ((unsigned char *)(buf))[3] = _tmp_varul.uc[3]; \
}

#define DESC(desc,v) (v)

#define MEMCPY_S(pdest, buflen, psour, n)               memcpy((void *)(pdest), (void *)(psour), MIN(buflen, n))
#define MEMMOVE_S(pdest, destlen, psour, sourlen, n)    memmove((void *)(pdest), (void *)(psour), MIN(destlen, MIN(sourlen, n)))
#define MEMSET_S(dest, destsz, ch, count)               memset((void *)(pdest), ch, MIN(destsz, count))
#define MEMZERO(dst, size)                              memset(dst, 0, size)

/* 目的地址, 原地址，长度 */
#define MEMCPY_R(dest, src, count)  \
do{\
    char *pmDest = (char *)dest;\
    char *pmSrc = (char *)src;\
    int  mcount = (int)count;\
    if(NULL == pmDest || NULL == pmSrc || count <= 0)\
    {\
        break;\
    }\
    while(mcount--)\
    {\
        *(char *)pmDest++ = *(char *)(pmSrc+mcount); \
    }\
}while(0);

/* 消息长度, 消息内容头，长度表示占用长度 */
#define GET_LEN_OFFSET(lenout, pIndata, nBytesOut)\
do\
{\
    unsigned char *pDataIn = (unsigned char*)(pIndata);\
    lenout = pDataIn[0];\
    nBytesOut = 1;\
    if(0x81 == lenout)\
    {\
        lenout = pDataIn[1];\
        nBytesOut = 2;\
    }\
    else if(0x82 == lenout)\
    {\
        lenout = pDataIn[1];\
        lenout <<= 8;\
        lenout += pDataIn[2];\
        nBytesOut = 3;\
    }\
    else if(0x83 == lenout)\
    {\
        lenout = pDataIn[1];\
        lenout <<= 8;\
        lenout += pDataIn[2];\
        lenout <<= 8;\
        lenout += pDataIn[3];\
        nBytesOut = 4;\
    }\
    else if(0x84 == lenout)\
    {\
        lenout = pDataIn[1];\
        lenout <<= 8;\
        lenout += pDataIn[2];\
        lenout <<= 8;\
        lenout += pDataIn[3];\
        lenout <<= 8;\
        lenout += pDataIn[4];\
        nBytesOut = 5;\
    }\
    else if(0x80 == lenout || lenout > 0x85)\
    {\
        lenout = 0;\
    }\
}while(0);

/* 数据长度, 长度表示内容，长度表示占用长度 */
#define SET_LEN_OFFSET(Indatalen, pByteOut, ByteLenOut)\
do\
{\
    uint8 *pByteO = (uint8*)(pByteOut);\
    ByteLenOut = 0;\
    if((Indatalen) < 0x80)\
    {\
        pByteO[0] = (Indatalen);\
        ByteLenOut = 1;\
    }\
    else if((Indatalen) <= 0xff)\
    {\
        pByteO[0] = 0x81;\
        pByteO[1] = (Indatalen);\
        ByteLenOut = 2;\
    }\
    else if((Indatalen) <= 0xffff)\
    {\
        pByteO[0] = 0x82;\
        pByteO[1] = (unsigned char)(((Indatalen) >> 8) & 0xff);\
        pByteO[2] = (unsigned char)((Indatalen) & 0xff);\
        ByteLenOut = 3;\
    }\
    else if((Indatalen) <= 0xffffff)\
    {\
        pByteO[0] = 0x83;\
        pByteO[1] = (unsigned char)(((Indatalen) >> 16) & 0xff);\
        pByteO[2] = (unsigned char)(((Indatalen) >> 8) & 0xff);\
        pByteO[3] = (unsigned char)((Indatalen) & 0xff);\
        ByteLenOut = 4;\
    }\
    else if((Indatalen) <= 0xffffffff)\
    {\
        pByteO[0] = 0x84;\
        pByteO[1] = (unsigned char)(((Indatalen) >> 24) & 0xff);\
        pByteO[2] = (unsigned char)(((Indatalen) >> 16) & 0xff);\
        pByteO[3] = (unsigned char)(((Indatalen) >> 8) & 0xff);\
        pByteO[4] = (unsigned char)((Indatalen) & 0xff);\
        ByteLenOut = 5;\
    }\
}while(0);

/*定义只接纳非0的malloc*/
#define MALLOC_S(size) ((size) > 0? malloc(size): NULL)
    
/*定义释放后置为空的free*/
#define FREE_S(ptr) if(ptr) {free(ptr); ptr = NULL;}

/* 防止线程退出后 没有释放锁资源(不带条件变量) */
#define PTHREAD_LOCK_SAFE(lockptr) \
        pthread_cleanup_push((void*)pthread_mutex_unlock, lockptr);\
        pthread_mutex_lock(lockptr);

/* 线程自己退出或者外界pthread_cancel时执行清理(不带条件变量) */
#define PTHREAD_UNLOCK_SAFE(lockptr) \
        pthread_mutex_unlock(lockptr);\
        pthread_cleanup_pop(0);

#ifdef __cplusplus
}
#endif

#endif /* __MACRO_H__ */
