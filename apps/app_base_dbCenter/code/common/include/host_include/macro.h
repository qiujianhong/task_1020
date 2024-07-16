/*
*********************************************************************
* Copyright(C) 2018 China Electric Power Research Institute
* All rights reserved. 
* @brief��   �����궨��
* @date��    2018/10/24
* @history�� 
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

//ƴ��16λ����
#define MAKEWORD(l, h)      ((uint16)(((uint8)((l) & 0xff)) | ((uint16)((uint8)((h) & 0xff))) << 8))
//ƴ��32λ����
#define MAKELONG(l, h)      ((uint32)((uint16)(l) & 0xffff) | ((uint32)((uint16)(h) & 0xffff)) << 16)
#define MAKEDWORD(hw, lw)   ((hw)<<16 | ((lw) & 0xffff))

//ȡ��16λ����
#define LOWORD(l)           ((uint16)((uint32)(l) & 0xffff))
//ȡ��16λ����
#define HIWORD(l)           ((uint16)((uint32)(l) >> 16))
//ȡ��8λ����
#define LOBYTE(w)           ((uint8)((uint16)(w) & 0xff))
//ȡ��8λ����
#define HIBYTE(w)           ((uint8)((uint16)(w) >> 8))

//��Сֵ
#define MIN(a, b)			(((a)<(b))?(a):(b))
//���ֵ
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

/*����ֻ���ɷ�0��malloc*/
#define MALLOC_S(size) ((size) > 0? malloc(size): NULL)
    
/*�����ͷź���Ϊ�յ�free*/
#define FREE_S(ptr) if(ptr) {free(ptr); ptr = NULL;}

/* ��ֹ�߳��˳��� û���ͷ�����Դ(������������) */
#define PTHREAD_LOCK_SAFE(lockptr) \
        pthread_cleanup_push(pthread_mutex_unlock, lockptr);\
        pthread_mutex_lock(lockptr);

/* �߳��Լ��˳��������pthread_cancelʱִ������(������������) */
#define PTHREAD_UNLOCK_SAFE(lockptr) \
        pthread_mutex_unlock(lockptr);\
        pthread_cleanup_pop(0);

#ifdef __cplusplus
}
#endif

#endif /* __MACRO_H__ */
