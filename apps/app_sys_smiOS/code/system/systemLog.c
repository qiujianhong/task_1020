
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
* @brief�� �����Ϣ�Ƿ���Ҫд��log��������Ӧ�ļ�¼log����
* @param[in]��
               none
* @param[out]��
               none
* @return�� ����ֵ˵��
*********************************************************************
*/
SYS_LOGRECORD_TYPE sys_check_logtype(MSG_OBJ_T *pmsgobj) {
  SYS_LOGRECORD_TYPE emret = LOG_NONEED;
  sint32 tempIID = 0;
  sint32 tempIOP = 0;
  tempIID = GET_MSG_IID_FROM_TAG(pmsgobj->tag);
  tempIOP = GET_MSG_IOP_FROM_TAG(pmsgobj->tag);
  if (tempIID ==
      MSG_COMMON_IID_0000) {  //ͨ����Ϣ�ӿ�
                              // TODO
                              // ͨ����Ϣ�ӿ�����Ҫ��¼����Ϣ����׷��,���趨��Ӧ��logtype
  } else if ((tempIID == MSG_COMMON_IID_0001)) {  //ϵͳ��������Ϣ�ӿ�
    // TODO ϵͳ��Ϣ�ӿ�����Ҫ��¼����Ϣ����׷��,���趨��Ӧ��logtype
    switch (tempIOP) {
      case 0x0000:  //����ͣ��
      case 0x0001:  //ϵͳͣ��
      case 0x0002:  //ϵͳ��λ
      case 0x0003:  //ϵͳ�ϵ�
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
* @brief�� ��¼��Ϣ�ӿ�
* @param[in]��
               none
* @param[out]��
               none
* @return�� ����ֵ˵��
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