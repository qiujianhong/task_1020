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
#undef	MIN
#undef	MAX
#undef	MEMCPY_S
#undef	MEMMOVE_S
#undef	MEMSET_S

//拼接16位数据
#define MAKEWORD(l, h)      ((uint16)(((uint8)((l) & 0xff)) | ((uint16)((uint8)((h) & 0xff))) << 8))
//拼接32位数据
#define MAKELONG(l, h)      ((uint32)((uint16)(l) & 0xffff) | ((uint32)((uint16)(h) & 0xffff)) << 16)
#define MAKEDWORD(hw, lw)   ((hw)<<16 | ((lw) & 0xffff))

//取低16位数据
#define LOWORD(l)           ((uint16)((uint32)(l) & 0xffff))
//取高16位数据
#define HIWORD(l)           ((uint16)((uint32)(l) >> 16))
//取低8位数据
#define LOBYTE(w)           ((uint8)((uint16)(w) & 0xff))
//取高8位数据
#define HIBYTE(w)           ((uint8)((uint16)(w) >> 8))

//最小值
#define MIN(a, b)			(((a)<(b))?(a):(b))
//最大值
#define MAX(a, b)			(((a)>(b))?(a):(b))

#define MAKE_SHORT(buf)		MAKEWORD(((unsigned char*)(buf))[0], ((unsigned char*)(buf))[1])


#define DEPART_SHORT(usv, buf) { \
	varus_t _tmp_varus; \
	_tmp_varus.us = usv; \
	((unsigned char *)(buf))[0] = _tmp_varus.uc[0]; \
	((unsigned char *)(buf))[1] = _tmp_varus.uc[1]; \
}

#define MAKE_LONG(buf)		MAKELONG(MAKEWORD(((unsigned char*)(buf))[0], ((unsigned char*)(buf))[1]),\
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

/*定义只接纳非0的malloc*/
#define MALLOC_S(size) ((size) > 0? malloc(size): NULL)
    
/*定义释放后置为空的free*/
#define FREE_S(ptr) if(ptr) {free(ptr); ptr = NULL;}

/* 防止线程退出后 没有释放锁资源(不带条件变量) */
#define PTHREAD_LOCK_SAFE(lockptr) \
        pthread_cleanup_push(pthread_mutex_unlock, lockptr);\
        pthread_mutex_lock(lockptr);

/* 线程自己退出或者外界pthread_cancel时执行清理(不带条件变量) */
#define PTHREAD_UNLOCK_SAFE(lockptr) \
        pthread_mutex_unlock(lockptr);\
        pthread_cleanup_pop(0);

#ifdef __cplusplus
}
#endif

#endif /* __MACRO_H__ */
