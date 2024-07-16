#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "baseType.h"
#include "cJSON.h"
#include "globalConfiguration.h"
#include "xlCmd.h"
#include "xlmPlatform.h"
#include "xlmPlatformDef.h"
#include "xlmPlatformInternal.h"

#include "framecomm.h"

static XL_PLATFORM_T *s_xl_platform = NULL;

static int platform_recv(XL_PLATFORM_T *platform,
                         sint32 *       mid,
                         sint8          topic[MAX_TOPIC_LEN],
                         sint8          msgType[PL_MSG_TYPE_LEN],
                         sint8 *        data,
                         sint32         dataMaxLen,
                         long           msecs)
{
    sint32 qos;
    sint32 msg_mid;
    sint32 ret;

    UNUSED(mid);
    UNUSED(msgType);
    ret = xl_channel_mqtt_read(platform->ctx, topic, &qos, &msg_mid, data,
                               dataMaxLen, msecs);
    if (ret > 0)
    {
        SMIOS_FMT_DEBUG("recv t:%s v:%s\n", topic, data);
        cJSON *jsonObj = cJSON_Parse(data);
        if (jsonObj == NULL)
        {
            SMIOS_FMT_DEBUG("cJSON_Parse fail [%s]\n", data);
        }
        else
        {
            char *tmp_msgType =
                cJSON_GetObjectItem(jsonObj, "msgType")->valuestring;
            if (tmp_msgType)
            {
                strncpy(msgType, tmp_msgType, PL_MSG_TYPE_LEN);
                *mid = cJSON_GetObjectItem(jsonObj, "mid")->valueint;
                SMIOS_FMT_DEBUG("msgType:[%s] mid:[%d]\n", msgType, *mid);
                return 0;
            }
        }
    }
    return -1;
}

/**
*********************************************************************
* @brief：                 平台连接线程
* @param[in]： arg         线程参数当前未使用
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
static void *platform_thread(void *arg)
{
    UNUSED(arg);
    sint32 mid = 0;
    sint8  topic[MAX_TOPIC_LEN];
    sint8  msgType[PL_MSG_TYPE_LEN];
    sint8  data[4096];
    do
    {
        // SMIOS_FMT_DEBUG("platform_thread\n");
        // platform_TimeReq(s_xl_platform);
        memset(data, 0, 4096);
        if (0 == platform_recv(s_xl_platform, &mid, topic, msgType, data,
                               sizeof(data), 1000))
        {
            if (0 == strncmp(msgType, "TimeRsp", PL_MSG_TYPE_LEN))
            {
                platform_time_rsp(mid, data);
            }
            else if (0 == strncmp(msgType, "stateReq", PL_MSG_TYPE_LEN))
            {
                platform_get_smiOS_status(s_xl_platform, mid, data);
            }
            else if (0 == strncmp(msgType, "downloadImage", PL_MSG_TYPE_LEN) ||
                     0 == strncmp(msgType, "updateImage", PL_MSG_TYPE_LEN) ||
                     0 == strncmp(msgType, "deleteImage", PL_MSG_TYPE_LEN) ||
                     0 == strncmp(msgType, "getImageInfo", PL_MSG_TYPE_LEN))
            {
                platform_image_manager(s_xl_platform, msgType, mid, data);
            }
            else if (0 == strncmp(msgType, "startContainer", PL_MSG_TYPE_LEN) ||
                     0 == strncmp(msgType, "stopContainer", PL_MSG_TYPE_LEN) ||
                     0 == strncmp(msgType, "deleteContainer", PL_MSG_TYPE_LEN))
            {
                platform_container_manager(s_xl_platform, msgType, mid, data);
            }
            else if (0 == strncmp(msgType, "downloadApp", PL_MSG_TYPE_LEN) ||
                     0 == strncmp(msgType, "updateApp", PL_MSG_TYPE_LEN) ||
                     0 == strncmp(msgType, "stopApp", PL_MSG_TYPE_LEN) ||
                     0 == strncmp(msgType, "deleteApp", PL_MSG_TYPE_LEN) ||
                     0 == strncmp(msgType, "restartApp", PL_MSG_TYPE_LEN))
            {
                platform_app_manager(s_xl_platform, msgType, mid, data);
            }
        }

    } while (1);

    return NULL;
}

/* @brief:平台模块块初始化 */
sint32 platform_init_system(void)
{
    sint8  platformUrl[128] = {0};
    sint32 ret              = -1;
    do
    {
        INIT_FMT_DEBUG("platform_init_system\n");
        s_xl_platform = calloc(1, sizeof(XL_PLATFORM_T));
        if (s_xl_platform == NULL)
        {
            break;
        }

        snprintf(s_xl_platform->subTopic[0].topic,
                 sizeof(s_xl_platform->subTopic[0].topic),
                 "/iot/v1/devices/%s/command/set/request/%s",
                 g_configObj.platformCfg.mqttUserID, "json");
        s_xl_platform->subTopic[0].qos = 2;
        snprintf(s_xl_platform->subTopic[1].topic,
                 sizeof(s_xl_platform->subTopic[1].topic),
                 "/iot/v1/devices/%s/data/report/response/%s",
                 g_configObj.platformCfg.mqttUserID, "json");
        s_xl_platform->subTopic[1].qos = 2;

        snprintf(s_xl_platform->cmdPubTopic.topic,
                 sizeof(s_xl_platform->cmdPubTopic.topic),
                 "/iot/v1/devices/%s/command/set/response/%s",
                 g_configObj.platformCfg.mqttUserID, "json");
        s_xl_platform->cmdPubTopic.qos = 2;

        snprintf(s_xl_platform->reportPubTopic.topic,
                 sizeof(s_xl_platform->reportPubTopic.topic),
                 "/iot/v1/devices/%s/data/report/request/%s",
                 g_configObj.platformCfg.mqttUserID, "json");
        s_xl_platform->reportPubTopic.qos = 2;

        snprintf(platformUrl, sizeof(platformUrl), "mqtt://%s:%u",
                 g_configObj.platformCfg.mqttBrokerIP,
                 g_configObj.platformCfg.mqttBrokerPort);
        INIT_FMT_DEBUG("platformUrl:%s\n", platformUrl);
        if (0 != xl_channel_create(platformUrl, &s_xl_platform->ctx))
        {
            INIT_FMT_DEBUG("xl_channel_create fail url:%s\n", platformUrl);
            break;
        }

        if (0 != xl_channel_set_mqtt_user_ID(
                     s_xl_platform->ctx, g_configObj.platformCfg.mqttUserID))
        {
            INIT_FMT_DEBUG("xl_channel_set_mqtt_user_ID fail\n");
            break;
        }

        if (0 !=
            xl_channel_set_mqtt_default_topic(s_xl_platform->ctx, "mqtt_test"))
        {
            INIT_FMT_DEBUG("xl_channel_set_mqtt_user_ID fail\n");
            break;
        }

        if (0 != xl_channel_set_mqtt_sub_topic(s_xl_platform->ctx, 2,
                                               &s_xl_platform->subTopic))
        {
            INIT_FMT_DEBUG("xl_channel_set_mqtt_sub_topic fail\n");
            break;
        }

        if (0 != xl_channel_init(s_xl_platform->ctx))
        {
            INIT_FMT_DEBUG("xl_channel_init fail\n");
            break;
        }

        if (0 != xl_channel_connect(s_xl_platform->ctx))
        {
            INIT_FMT_DEBUG("xl_channel_connect fail\n");
            // break;
        }

        ret = 0;
    } while (0);

    return ret;
}

/* @brief:平台模块清理 */
void platform_clean_up_system(void)
{
    return;
}

/* @brief:平台模块启动 */
sint32 platform_start_system(void)
{
    sint32 ret = -1;

    do
    {
        if (0 !=
            pthread_create(&(s_xl_platform->tid), NULL, platform_thread, NULL))
        {
            break;
        }

        ret = 0;
    } while (0);

    return ret;
}

/* @brief:平台模块停止 */
void platform_stop_system(void)
{
    return;
}
