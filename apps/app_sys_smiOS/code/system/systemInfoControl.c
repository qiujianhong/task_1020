/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 系统情报处理接口
* @date： 2019-11-20
*********************************************************************
*/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "baseType.h"
#include "globalConfiguration.h"
#include "systemInfoControl.h"
#include "framecomm.h"

static sint8 sysrebooting;                      //系统是否在重启中flag
static syscallback killAppCb = NULL;            //关闭所有APP的callback变量
static syscallback EventdistribuCb = NULL;      //事件通知callback变量


static sint32 cfgMemrateWarnning;               //配置文件中设定的mem告警阈值
static sint32 cfgCpurateWarnning;               //配置文件中设定的cpu使用率告警阈值
static sint32 cfgStoragerateWarnning;           //配置文件中设定的存储使用率告警阈值

/**
*********************************************************************
* @brief： 内存占用率检测函数
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
void sys_checkmemrate_cb(void) 
{
  sint32 rate = 0;
  rate = sys_ramrate_get();
  if (rate < 0) {
    SMIOS_FMT_DEBUG("[sysinfo]memory rate update failure!\n");
  } 
  else if (rate > cfgMemrateWarnning) 
  {
    sys_warningmsg_record("memory rate:[%d] over warning level:[%d]\n",rate,cfgMemrateWarnning);
    SMIOS_FMT_DEBUG("[sysinfo]memory rate:[%d] over warning level:[%d]\n", rate,
               cfgMemrateWarnning);
  } 
  else 
  {
    SMIOS_FMT_DEBUG("[sysinfo]memory rate:[%d]\n", rate);
  }
}
/**
*********************************************************************
* @brief： CPU使用率检测函数，并根据已设定告警值来打印log
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
void sys_checkcpurate_cb(void) 
{
  sint32 rate = 0;
  rate = sys_cpurate_get();
  if (rate < 0) {
    SMIOS_FMT_DEBUG("[sysinfo]CPU rate update failure!\n");
  } 
  else if (rate > cfgCpurateWarnning*100) 
  {
    sys_warningmsg_record("CPU rate:[%d.%d] over warning level:[%d]\n", rate/100,rate%100,
               cfgCpurateWarnning);
    SMIOS_FMT_DEBUG("[sysinfo]CPU rate:[%d.%d] over warning level:[%d]\n", rate/100,rate%100,
               cfgCpurateWarnning);
  } 
  else 
  {
    SMIOS_FMT_DEBUG("[sysinfo]CPU rate:[%d]\n", rate);
  }
}
/**
*********************************************************************
* @brief： 硬盘使用率检测函数，并根据已设定告警值来打印log
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
void sys_checkstorgerate_cb(void) 
{
  sint32 rate = 0;
  rate = sys_storgaerate_get();
  if (rate < 0) 
  {
    SMIOS_FMT_DEBUG("[sysinfo]stroage used rate update failure!\n");
  }
  else if (rate > cfgStoragerateWarnning) 
  {
    sys_warningmsg_record("stroage used rate:[%d] over warning level:[%d]\n", rate,
               cfgStoragerateWarnning);
    SMIOS_FMT_DEBUG("[sysinfo]stroage used rate:[%d] over warning level:[%d]\n", rate,
               cfgStoragerateWarnning);
  } 
  else 
  {
    SMIOS_FMT_DEBUG("[sysinfo]stroage used rate:[%d]\n", rate);
  }
}
/**
*********************************************************************
* @brief： flash坏块检测函数
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
void sys_checkflash_cb(void) 
{
  // TODO
}
/**
*********************************************************************
* @brief： 系统时钟检测函数
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
void sys_checkclock_cb(void) 
{
  // TODO
}
/**
*********************************************************************
* @brief： 内存检测函数
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
void sys_checkram_cb(void) 
{
  // TODO
}
/**
*********************************************************************
* @brief： esam检测函数
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
void sys_checkesam_cb(void) 
{
  // TODO
}

/**
*********************************************************************
* @brief： 初始化处理
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 sys_control_init(void) 
{
  cfgMemrateWarnning = g_configObj.serviceCfg.memratewarnlevel;
  cfgCpurateWarnning = g_configObj.serviceCfg.cpuratewarnlevel;
  cfgStoragerateWarnning = g_configObj.serviceCfg.storagerateWarnlevel;

  sysrebooting = 0;
  sys_systeminfo_start();
  sys_wd_init();      //看门狗初始化
  return 0;
}
/**
*********************************************************************
* @brief： system启动
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 sys_control_start(void) 
{
  sint32 nret = 0 ;
  //sys_systeminfo_start();
  nret = sys_wd_start();           //启动看门狗
  return nret;
  
}
/**
*********************************************************************
* @brief： systeminfo停止
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
void sys_control_stop(void) 
{
  sys_systeminfo_stop();
  sys_wd_stop();         //停止看门狗跟看门狗的喂狗线程
}
/**
*********************************************************************
* @brief： 执行shell的reboot命令，重启系统
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
void sys_control_reboot(void) 
{
  SMIOS_FMT_DEBUG("[sysinfo] system is rebooting now!\n");
  sys_run_cmd((sint8 *)"reboot");                  //执行reboot命令
}

#if 0
/**
*********************************************************************
* @brief： 取得APP的osversion接口
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 sys_app_osversion_get(APPOSVERSION_T *pversion, sint32 *plenth) 
{
  UNUSED(pversion);
  UNUSED(plenth);
#ifdef SYSTEM_DEBUG
#define dbgname "dbgsystem"
  strncpy(pversion->appname, dbgname, sizeof(dbgname));
  sys_osversion_get(pversion->osversion);
  *plenth = 1;
#endif
  return 0;
}

/**
*********************************************************************
* @brief： 取得APP的cpu使用率接口
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 sys_app_cpurate_get(APPCPURATE_T *pcpurate, sint32 *plenth) 
{
  UNUSED(pcpurate);
  UNUSED(plenth);
#ifdef SYSTEM_DEBUG
#define dbgname "dbgsystem"
  strncpy(pcpurate->appname, dbgname, sizeof(dbgname));
  pcpurate->cpurate = (sint8)sys_cpurate_get();
  *plenth = 1;
#endif
  return 0;
}
/**
*********************************************************************
* @brief： 取得APP的ram使用率接口
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 sys_app_ramrate_get(APPRAMRATE_T *pramrate, sint32 *plenth) 
{
  UNUSED(pramrate);
  UNUSED(plenth);
#ifdef SYSTEM_DEBUG
#define dbgname "dbgsystem"
  strncpy(pramrate->appname, dbgname, sizeof(dbgname));
  pramrate->ramrate = (sint8)sys_ramrate_get();
  *plenth = 1;
#endif
  return 0;
}
/**
*********************************************************************
* @brief： 取得APP的storage使用率接口
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 sys_app_storagerate_get(APPSTORAGERATE_T *pstoragerate, sint32 *plenth) 
{
  UNUSED(pstoragerate);
  UNUSED(plenth);
#ifdef SYSTEM_DEBUG
#define dbgname "dbgsystem"
  strncpy(pstoragerate->appname, dbgname, sizeof(dbgname));
  pstoragerate->storagerate = sys_storgaerate_get();
  *plenth = 1;
#endif
  return 0;
}
/**
*********************************************************************
* @brief： 取得APP的osversion,cpu/内存/storage使用率接口
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 sys_app_info_get(APPINFO_T *pinfo, sint32 *plenth)
{
  UNUSED(pinfo);
  UNUSED(plenth);
#ifdef SYSTEM_DEBUG
#define dbgname "dbgsystem"
  APPINFO_T *ptemp = malloc(sizeof(APPINFO_T));
  strncpy(ptemp->appname, dbgname, sizeof(dbgname));
  ptemp->strogagerate = sys_storgaerate_get();
  ptemp->ramrate = sys_ramrate_get();
  ptemp->cpurate = (sint8)sys_cpurate_get();
  memcpy(pinfo, ptemp, sizeof(APPINFO_T));
  *plenth = 1;
  free(ptemp);
#endif
  return 0;
}
#endif
/**
*********************************************************************
* @brief： 重启接口
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 sys_msg_requestreboot(void) 
{
  // check rebooting状态
  if (1 == sysrebooting) 
  {
    //重复请求防止
    return 0;
  }
  //设定rebooting状态为1
  sysrebooting = 1;
  // TODO kill all app
  if (killAppCb != NULL) 
  {
    killAppCb((void *)NULL);
  } 
  else 
  {
    // printf("callback function is NULL\n");
    SMIOS_FMT_DEBUG("[SYS]APP关闭函数未注册或注册为NULL!\n");
  }
  sys_control_reboot();

  return 0;
}
/**
*********************************************************************
* @brief： 事件消息接口
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 sys_msg_eventdistribution(MSG_OBJ_T *pmsgobj) 
{
  sint32 tempiid;
  sint8 nret = 0;
  if (NULL == pmsgobj) 
  {
    nret = -1;
  }
  tempiid = (sint32)GET_MSG_IID_FROM_TAG(pmsgobj->tag);
  switch (tempiid) 
  {
    case MSG_COMMON_IID_0001:
      // TODO 调用APP的事件接口
      if (EventdistribuCb != NULL) 
      {
        EventdistribuCb((void *)pmsgobj);
      } 
      else
      {
        SMIOS_FMT_DEBUG("[SYS]事件发布回调函数未注册!\n");
      }
      break;
    default:
      break;
  }
  return nret;
}


/**
*********************************************************************
* @brief：
* @param[in]：
               none
* @param[out]：
               none
* @return：
*********************************************************************
*/
sint32 sys_register_callback(SYS_CALLBACK_TYPE cb_type, syscallback funtioncb) 
{
  sint32 nret = 0;
  switch (cb_type) 
  {
    case CB_EVENTDT:
      EventdistribuCb = funtioncb;
      break;
    case CB_KILLAPP:
      killAppCb = funtioncb;
      break;
    default:
      nret = -1;
      SMIOS_FMT_DEBUG("[SYS]register callback type error!\n");
      break;
  }
  return nret;
}
