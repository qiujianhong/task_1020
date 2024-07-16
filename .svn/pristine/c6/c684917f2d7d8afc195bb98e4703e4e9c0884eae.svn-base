
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "baseType.h"
#include "systemInfo.h"
#include "systemLog.h"


typedef enum _sys_logrecord_type {
  LOG_NONEED,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_DEBUG,
} SYS_LOGRECORD_TYPE;
/**
*********************************************************************
* @brief： 检查消息是否需要写入log并返回相应的记录log类型
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
SYS_LOGRECORD_TYPE sys_check_logtype(MSG_OBJ_T *pmsgobj) {
  SYS_LOGRECORD_TYPE emret = LOG_NONEED;
  sint32 tempIID = 0;
  sint32 tempIOP = 0;
  tempIID = GET_MSG_IID_FROM_TAG(pmsgobj->tag);
  tempIOP = GET_MSG_IOP_FROM_TAG(pmsgobj->tag);
  if (tempIID ==
      MSG_COMMON_IID_0000) {  //通用消息接口
                              // TODO
                              // 通用消息接口里需要记录的消息在这追加,并设定相应的logtype
  } else if ((tempIID == MSG_COMMON_IID_0001)) {  //系统管理器消息接口
    // TODO 系统消息接口里需要记录的消息在这追加,并设定相应的logtype
    switch (tempIOP) {
      case 0x0000:  //紧急停电
      case 0x0001:  //系统停电
      case 0x0002:  //系统复位
      case 0x0003:  //系统上电
        emret = LOG_WARN;
        break;
      default:  // do nothing
        break;
    }
  } else {
    /*do nothing*/
  }
  return emret;
}
/**
*********************************************************************
* @brief： 记录消息接口
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 sys_recordmsg_tolog(MSG_OBJ_T *pmsgobj) {
  SYS_LOGRECORD_TYPE log_type = LOG_NONEED;
  log_type = sys_check_logtype(pmsgobj);
  switch (log_type) {
    case LOG_INFO:
      // TODO
      break;
    case LOG_WARN:
      sys_warningmsg_record("[SYS][WARNING]IID:%4d:,IOP:%4d",
                            GET_MSG_IID_FROM_TAG(pmsgobj->tag),
                            GET_MSG_IOP_FROM_TAG(pmsgobj->tag));
      break;
    case LOG_ERROR:
      // TODO
      break;
    case LOG_DEBUG:
      // TODO
      break;
    default:
      break;
  }
  return 0;
}