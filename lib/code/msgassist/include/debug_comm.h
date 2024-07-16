/*
*********************************************************************
* @filename: debug_comm.h
* @brief   : 消息通信调试
* @describe: 
* @history: 
*    auther      date        describe
*    王津剑      2020-1-3    create
*********************************************************************
*/
#ifndef    __DEBUG_COMM_H
#define __DEBUG_COMM_H

#ifdef __cplusplus
    extern "C" {
#endif

#define DEBUG_ASSIST
#ifdef DEBUG_ASSIST
  #include <errno.h>
  extern int errno;
  #define DPRINT(x...) \
  if(getenv("DIAG_ASSIST") != NULL)\
  {\
      struct timeval msgtv; \
      struct tm     *pMsgtm; \
      gettimeofday(&msgtv, NULL);\
      pMsgtm = localtime(&msgtv.tv_sec);\
      printf("[%4d-%02d-%02d %02d:%02d:%02d.%06ld] [TID: %lu, DB_MSGASSIST] ", pMsgtm->tm_year + 1900, pMsgtm->tm_mon + 1, \
             pMsgtm->tm_mday, pMsgtm->tm_hour, pMsgtm->tm_min, pMsgtm->tm_sec, msgtv.tv_usec, pthread_self());\
      printf(x);\
  }
#else
  #define DPRINT(x...)
#endif

#ifdef __cplusplus
}
#endif

#endif

