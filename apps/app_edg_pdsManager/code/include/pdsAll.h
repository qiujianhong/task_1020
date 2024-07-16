#ifndef __PDSALL_H__
#define __PDSALL_H__

#include "baseCommon.h"
#include "framecomm.h"
#include "db_api.h"
#include "task_id.h"
#include "appmsg_split.h"
#include "task_msg.h"
#include "msg_id.h"

#define DBCENTER_NAME "M-dbCenter"
#define SMIOS_NAME  "M-smiOS"
#define TASKMANAGER_NAME  "M-taskManager"
#define MAX_MSG_LEN 5120
#define MAX_BREAK_COUNT 100     //断路器最大个数

#define USERTYPE_FRAME_BREAKER      212    //框架断路器 用户类型212 智能总开关
#define USERTYPE_LOW_BREAKER        213     //低压断路器 分支层
#define USERTYPE_METERBOX_BREAKER   220    //表箱断路器

extern void *memcpy_r(void *dest, const void *src, int count);
extern void Sec_To_Rtc(uint32 time, rtc_t *prtc);
extern uint16 get_len_offset(uint8 *indata,uint8 *bytes);
#endif

