#ifndef __XLM_PLAT_FORM_INTERNAL_H__
#define __XLM_PLAT_FORM_INTERNAL_H__

#include <pthread.h>

#include "baseType.h"
#include "xlChannel.h"
#include "xlmPlatformDef.h"
#ifdef __cplusplus
extern "C" {
#endif

/* @brief:udp通讯服务结构体封装  */
typedef struct _xl_platform_obj_
{
    XL_CHANNEL_CTX_T   *ctx;           //新联MQTT平台连接对象
    pthread_t          tid;            //ThreadID
    MQTT_TOPIC_T       subTopic[SUB_TOPIC_NUM];
    MQTT_TOPIC_T       cmdPubTopic;
    MQTT_TOPIC_T       reportPubTopic;
} XL_PLATFORM_T;

#ifdef __cplusplus
}
#endif

#endif