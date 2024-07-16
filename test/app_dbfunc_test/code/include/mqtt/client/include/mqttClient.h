#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#include "baseConstant.h"
#include "xlChannel.h"

XL_CLIENT_T *new_mqtt_client(void);

void del_mqtt_client(XL_CLIENT_T *c);

sint32 mqtt_clien_read_plus(XL_CLIENT_T *c,
                            char *       topic,
                            sint32 *     qos,
                            sint32 *     mid,
                            char *       data,
                            sint32       dataLen,
                            long         msecs);

sint32 mqtt_clien_write_plus(XL_CLIENT_T *c,
                             char *       topic,
                             sint32       qos,
                             sint32 *     mid,
                             const char * data,
                             sint32       dataLen);

#endif