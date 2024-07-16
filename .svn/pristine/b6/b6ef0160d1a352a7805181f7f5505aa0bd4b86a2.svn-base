
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

#include "smiOSConfig.h"
#include "appManager.h"
#include "baseType.h"
#include "globalConfiguration.h"
#include "systemWatchDog.h"
#include "framecomm.h"
#include "hal.h"
#include "watchdog.h"
 #include <sys/prctl.h>
static pthread_t ntid;

static WATCHDOG_DEVICE_T* dev_wd = NULL;				//看门?

void sys_wd_seconds_sleep(sint32 seconds) 
{
  // struct timeval tv;
  // tv.tv_sec = seconds;
  // tv.tv_usec = 0;
  // sint32 err;
  // do 
  // {
    // err = select(0, NULL, NULL, NULL, &tv);
  // } while (err < 0 && errno == EINTR);
  sleep(seconds);
}
/**
*********************************************************************
* @brief? 看门狗喂狗函?
* @param[in]?
               none
* @param[out]?
               none
* @return? 返回值说?
*********************************************************************
*/
void sys_wd_clear(void) 
{
  hw_wdg_clear();
  return;
}
/**
*********************************************************************
* @brief?
* @param[in]?
               none
* @param[out]?
               none
* @return? 返回值说?
*********************************************************************
*/
void *sys_wd_thread_function(void *arg) 
{
  UNUSED(arg);
  sint32	seconds = 0;
  uint32	timeval;
  prctl(PR_SET_NAME,"watchdog");
  //调试暂时屏蔽看门?
//  while(1)
//  {
//  	sleep(10);
//  }
  while(dev_wd == NULL)
  {
      SMIOS_FMT_DEBUG("watchdog dev_wd is NULL!!\n");
      sleep(10);
  }
  timeval = (g_configObj.serviceCfg.wdfeedtimerval > 10 ) ? g_configObj.serviceCfg.wdfeedtimerval : 10; 	//防止设置过小
  seconds = timeval/10 ;
  
  SMIOS_FMT_DEBUG("feed sleep timeval: seconds = %d\n",seconds);
  SMIOS_FMT_DEBUG("g_configObj.serviceCfg.wdfeedtimerval:%d\n",g_configObj.serviceCfg.wdfeedtimerval);
  
  dev_wd->watchdog_period_set(dev_wd, timeval);
  
  while(1){
    SMIOS_FMT_TRACE("[event] watch dog clear\n");
    //sys_wd_clear();
    SMIOS_FMT_TRACE("watchdog run_sec: %d s\n",dev_wd->watchdog_runsec_get(dev_wd));
    dev_wd->watchdog_clear(dev_wd);
	SMIOS_FMT_TRACE("watchdog run_sec after being fed: %d s\n",dev_wd->watchdog_runsec_get(dev_wd));
#if 0

    app_manager_event_handle(EMERGENCY_BLACKOUT, NULL,0);
    app_manager_event_handle(SYSTEM_BLACKOUT, NULL, 0);
    app_manager_event_handle(SYSTEM_RESET, NULL, 0);
    app_manager_event_handle(SYSTEM_POWER_ON, NULL, 0);
    app_manager_event_handle(SYSTEM_CLOCK_CHECK, NULL, 0);

#endif
    sys_wd_seconds_sleep(seconds);
  };
  return NULL;
}

/**
*********************************************************************
* @brief? 打开看门?
* @param[in]?
               none
* @param[out]?
               none
* @return? 返回值说?
*********************************************************************
*/
sint32 sys_wd_start(void) 
{
    sint32 err;
    sint32 nret = 0;
    err = pthread_create(&ntid, NULL, sys_wd_thread_function, NULL);
    if (err != 0)
    {
        nret = -1;
        SMIOS_FMT_DEBUG("can't create thread: %s\n", strerror(err));
    }
    //pthread_join(ntid, NULL);
    pthread_detach(ntid);
    return nret;
}

/**
*********************************************************************
* @brief? 关闭看门?
* @param[in]?
               none
* @param[out]?
               none
* @return? 返回值说?
*********************************************************************
*/
sint32 sys_wd_stop(void) 
{
    sint32 err;
    //hw_wdg_stop();
#if (DISABLE_HAL == 0)
    hal_device_release((HW_DEVICE*)dev_wd);
    err = pthread_cancel(ntid);
    if (err != 0)
    {
        SMIOS_FMT_DEBUG("can't cancle thread: %s\n", strerror(err));
    }
#endif    
#ifdef SYSTEM_DEBUG
    printf("system stop\n");
#endif
    return 0;
}



/**
*********************************************************************
* @brief?
* @param[in]?
               none
* @param[out]?
               none
* @return? 返回值说?
*********************************************************************
*/
void sys_wd_get_timerval(void) 
{

}
/**
*********************************************************************
* @brief? 看门狗初始化函数
* @param[in]?
               none
* @param[out]?
               none
* @return? 返回值说?
*********************************************************************
*/
sint32 sys_wd_init(void) 
{
  	// TODO
  	//hw_wdg_open();
  	//sys_wd_get_timerval();
	//hal_init();
#if (DISABLE_HAL == 0)
    dev_wd = (WATCHDOG_DEVICE_T*)hal_device_get(HW_DEVICE_ID_WATCHDOG);
	if(NULL == dev_wd)
	{
		printf("watchdog dev is null \n");
		return -1;
	}
#endif

	//dev->watchdog_runsec_get(dev);
  
  	return 0;
}