/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   线程模板异常日志
* @date：    2020-6-29
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef    __TASK_LOG_H
#define __TASK_LOG_H
    
#ifdef __cplusplus
        extern "C" {
#endif

/* 死循环处理 */
#define SIGDEADLOOP           (SIGRTMIN + 10)

void task_log_init(char *filename);

/* 日志接口是适用于用户进程/线程模板中 记录异常日志 */
int TASK_ERROR_LOG(char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif

