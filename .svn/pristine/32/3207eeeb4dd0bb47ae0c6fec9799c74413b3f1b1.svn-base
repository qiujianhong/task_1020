#ifndef _CM_FUN_H_
#define _CM_FUN_H_
#ifdef __cplusplus
extern "C" {
#endif

int cm_version_print(char *buf, uint32 buflen);
int cm_app_init(void **ppPublicData);
    int cm_thread_proc(void *pArg, void *pPublic, void *pPrivate);

    int cm_thread_init(void *pPublic, void **ppPrivate);
typedef enum
{
    CM_INIT = 0,        //初始态
    CM_PRESS ,          //检测到testService开始加压 并且开始检测有没有性能检测的进程
    CM_OVER,            //检测到性能检测的性能后开始超频 并且降低APP的sleep时间 如果持续30秒没有检测到性能检测的线程就恢复正常频率
    CM_TEMP,            //检测到testService两分钟后进入该状态 检测cpu温度是否到75度 同时检测是否测试结束
    CM_END,             //结束态 检测testService有没有结束
}CM_STATUS;
typedef struct
{
    uint32 beginTime;       //开始加cpu负荷的时间
    CM_STATUS upflag;             //标记现在处于加压状态0 正常 1在加压 2cpu_stress在运行了不给加压
	//uint8 cpflag;             //标记cpu超频状态0 正常 1超频 2降频
    uint32 sleeptime;       //线程休眠时间 微秒
    uint32 lastTime;        //加压的时候1秒钟检查一次
    uint8  fileFlag;        //判断到CPU温度超过75度后置标记 
    uint32 lastFreqTime;      //记录下没有性能检测进程的时间 30秒后退出
}CM_PRIVATE_T;

#ifdef __cplusplus
}
#endif
#endif 

