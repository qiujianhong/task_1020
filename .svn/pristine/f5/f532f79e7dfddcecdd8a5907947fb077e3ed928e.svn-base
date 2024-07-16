/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   串口APP消息接口
* @date：    2021-7-30
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "cJSON.h"
#include "uart_api.h"

#define UART_STRING_TO_PARITY(portstring, parityE) \
do\
{\
    if(0 == memcmp(portstring, "even", strlen("even")))\
    {\
        parityE = UART_PARITY_EVEN;\
    }\
    else if(0 == memcmp(portstring, "odd", strlen("odd")))\
    {\
        parityE = UART_PARITY_ODD;\
    }\
    else if(0 == memcmp(portstring, "none", strlen("none")))\
    {\
        parityE = UART_PARITY_NONE;\
    }\
    else\
    {\
        parityE = 255;\
    }\
}while(0)

#define UART_STRING_TO_MODE(modestring, modeE) \
do\
{\
    if(0 == memcmp(modestring, "only", strlen("only")))\
    {\
        modeE = UART_MODE_ONLY;\
    }\
    else if(0 == memcmp(modestring, "master", strlen("master")))\
    {\
        modeE = UART_MODE_MASTER;\
    }\
    else if(0 == memcmp(modestring, "slave", strlen("slave")))\
    {\
        modeE = UART_MODE_SLAVE;\
    }\
    else\
    {\
        modeE = UART_MODE_UNLL;\
    }\
}while(0)

#define UART_STRING_TO_MSGSTATUS(statusstring, statusE) \
do\
{\
    if(0 == memcmp(statusstring, "OK", strlen("OK")) || 0 == memcmp(statusstring, "ok", strlen("ok")))\
    {\
        statusE = UART_RESULT_OK;\
    }\
    else if(0 == memcmp(statusstring, "TaskTimeOut", strlen("TaskTimeOut")))\
    {\
        statusE = UART_RESULT_TIME_OUT;\
    }\
    else if(0 == memcmp(statusstring, "TaskFull", strlen("TaskFull")))\
    {\
        statusE = UART_RESULT_TASK_FULL;\
    }\
    else if(0 == memcmp(statusstring, "ParamSerialError", strlen("ParamSerialError")))\
    {\
        statusE = UART_RESULT_PARAM_ERR;\
    }\
    else if(0 == memcmp(statusstring, "DataError", strlen("DataError")))\
    {\
        statusE = UART_RESULT_DATA_ERR;\
    }\
    else\
    {\
        statusE = UART_RESULT_DATA_OTHER;\
    }\
}while(0)

#define UART_PORTSTR(pPort)      UART_TYPE_485 == (pPort)->portType ? "RS485" : "RS232", (pPort)->portNo&0x000000ff

#define UART_DATABITS_GET(c) (((c)&0x03)>>0) //数据位
#define UART_PARIBITS_GET(c) (((c)&0x0c)>>2) //校验位
#define UART_STOPBITS_GET(c) (((c)&0x10)>>4) //停止位
#define UART_FLOWCTRL_GET(c) (((c)&0x60)>>5) //流控制

#define UART_DATABITS_SET(d) (((d)<<0)&0x03) //数据位
#define UART_PARIBITS_SET(p) (((p)<<2)&0x0c) //校验位
#define UART_STOPBITS_SET(s) (((s)<<4)&0x10) //停止位
#define UART_FLOWCTRL_SET(f) (((f)<<5)&0x60) //流控制

#define UART_CTRLBITS_SET(d,p,s,f) ((((f)<<5)&0x60)|(((s)<<4)&0x10)|(((p)<<2)&0x0c)|((d)&0x03))

#define UART_CTRL_TO_DATABITS(ctrl, databits) \
do\
{\
    if(UART_DATABITS_GET(ctrl) == 0)\
    {\
        databits = 5;\
    }\
    else if(UART_DATABITS_GET(ctrl) == 1)\
    {\
        databits = 6;\
    }\
    else if(UART_DATABITS_GET(ctrl) == 2)\
    {\
        databits = 7;\
    }\
    else if(UART_DATABITS_GET(ctrl) == 3)\
    {\
        databits = 8;\
    }\
}while(0)

#define UART_DATABITS_TO_CTRL(ctrl, databits) \
do\
{\
    if(databits == 5)\
    {\
        ctrl |= UART_DATABITS_SET(0);\
    }\
    else if(databits == 6)\
    {\
        ctrl |= UART_DATABITS_SET(1);\
    }\
    else if(databits == 7)\
    {\
        ctrl |= UART_DATABITS_SET(2);\
    }\
    else if(databits == 8)\
    {\
        ctrl |= UART_DATABITS_SET(3);\
    }\
}while(0)

#define UART_CTRL_TO_PARITY(ctrl, parityE) \
do\
{\
    if(UART_PARIBITS_GET(ctrl) == 0)\
    {\
        parityE = UART_PARITY_NONE;\
    }\
    else if(UART_PARIBITS_GET(ctrl) == 1)\
    {\
        parityE = UART_PARITY_ODD;\
    }\
    else if(UART_PARIBITS_GET(ctrl) == 2)\
    {\
        parityE = UART_PARITY_EVEN;\
    }\
    else\
    {\
        parityE = 255;\
    }\
}while(0)

#define UART_PARITY_TO_CTRL(ctrl, parityE) \
do\
{\
    if(parityE == UART_PARITY_NONE)\
    {\
        ctrl |= UART_PARIBITS_SET(0);\
    }\
    else if(parityE == UART_PARITY_ODD)\
    {\
        ctrl |= UART_PARIBITS_SET(1);\
    }\
    else if(parityE == UART_PARITY_EVEN)\
    {\
        ctrl |= UART_PARIBITS_SET(2);\
    }\
}while(0)

#define UART_CTRL_TO_STOPBITS(ctrl, stopbits) \
do\
{\
    if(UART_STOPBITS_GET(ctrl) == 0)\
    {\
        stopbits = 1;\
    }\
    else if(UART_STOPBITS_GET(ctrl) == 1)\
    {\
        stopbits = 2;\
    }\
}while(0)

#define UART_STOPBITS_TO_CTRL(ctrl, stopbits) \
do\
{\
    if(stopbits == 1)\
    {\
        ctrl |= UART_STOPBITS_SET(0);\
    }\
    else if(stopbits == 2)\
    {\
        ctrl |= UART_STOPBITS_SET(1);\
    }\
}while(0)

#define UART_CTRL_TO_FLOWCTRL(ctrl, flowctrl) \
do\
{\
    if(UART_FLOWCTRL_GET(ctrl) == 0)\
    {\
        flowctrl = UART_FLOWCTRL_NONE;\
    }\
    else if(UART_FLOWCTRL_GET(ctrl) == 1)\
    {\
        flowctrl = UART_FLOWCTRL_HARD;\
    }\
    else if(UART_FLOWCTRL_GET(ctrl) == 2)\
    {\
        flowctrl = UART_FLOWCTRL_SOFT;\
    }\
}while(0)

#define UART_FLOWCTRL_TO_CTRL(ctrl, flowctrl) \
do\
{\
    if(flowctrl == UART_FLOWCTRL_NONE)\
    {\
        ctrl |= UART_FLOWCTRL_SET(0);\
    }\
    else if(flowctrl == UART_FLOWCTRL_HARD)\
    {\
        ctrl |= UART_FLOWCTRL_SET(1);\
    }\
    else if(flowctrl == UART_FLOWCTRL_SOFT)\
    {\
        ctrl |= UART_FLOWCTRL_SET(2);\
    }\
}while(0)

extern void FastSecondToDate(struct timeval* tv, struct timezone* tz, struct tm* tm);
extern char *db_data_to_base64(uint8 *pData, uint32 datalen);
extern int db_base64_to_data(char *pBase, uint8 *pBuf, uint32 buflen, uint32 *datalen);

uint8 uart_ctrloctet_set(UART_MSG_PARAM_T *param)
{
    uint8 edatabits = 0;
    uint8 eparity = 0;
    uint8 estopbits = 0;
    uint8 eflowctrl = 0;

    switch(param->dataBits)
    {
        case 5: edatabits = 0; break;
        case 6: edatabits = 1; break;
        case 7: edatabits = 2; break;
        case 8: edatabits = 3; break;
        default:break;
    }

    switch(param->parity)
    {
        case UART_PARITY_NONE: eparity = 0; break;
        case UART_PARITY_ODD:  eparity = 1; break;
        case UART_PARITY_EVEN: eparity = 2; break;
        default:break;
    }

    switch(param->stopBits)
    {
        case 1: estopbits = 0; break;
        case 2: estopbits = 1; break;
        default:break;
    }

    switch(param->flowCtrl)
    {
        case UART_FLOWCTRL_NONE: eflowctrl = 0; break;
        case UART_FLOWCTRL_HARD: eflowctrl = 1; break;
        case UART_FLOWCTRL_SOFT: eflowctrl = 2; break;
        default:break;
    }

    return UART_CTRLBITS_SET(edatabits, eparity, estopbits, eflowctrl);    
}

#if DESC("JSON消息接口", 1)

/**********************************************************************
* @name      : uart_json_notify_only_split
* @brief     : 串口独占模式通知解析
* @param[in] : MESSAGE_JSON_T *pMsg      接收消息JSON格式
* @param[out]: UART_ONLY_T *pMsgOnly     解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int uart_json_notify_mode_split(MESSAGE_JSON_T *pMsg, UART_MODEINFO_T *pMsgOnly)
{
    cJSON *jRoot = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pMsgOnly)
    {
        return -1;
    }

    if (0 != strcmp(pMsg->sourapp, UART_APP_NAME) || 0 != strcmp(pMsg->operat, MQTT_TOPIC_OPER_REPORT) || 
        NULL == strstr(pMsg->infopath, UART_TOPIC_MODECHG))
    {
        return -2;
    }

    memset(pMsgOnly, 0, sizeof(UART_MODEINFO_T));
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "port");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -3;
    }
    UART_STRING_TO_PORT(jTemp->valuestring, pMsgOnly->port.portType, pMsgOnly->port.portNo);

    jTemp = cJSON_GetObjectItem(jRoot, "mode");
    if (cJSON_IsString(jTemp) == FALSE || strlen(jTemp->valuestring) == 0)
    {
        return -4;
    }

    UART_STRING_TO_MODE(jTemp->valuestring, pMsgOnly->mode);
    
    jTemp = cJSON_GetObjectItem(jRoot, "lastAppName");
    if(NULL != jTemp && cJSON_String == jTemp->type && 0 != strlen(jTemp->valuestring))
    {
        memcpy(pMsgOnly->lastApp, jTemp->valuestring, MIN(63, strlen(jTemp->valuestring)));
    }
    
    jTemp = cJSON_GetObjectItem(jRoot, "currAppName");
    if(NULL != jTemp && cJSON_String == jTemp->type && 0 != strlen(jTemp->valuestring))
    {
        memcpy(pMsgOnly->curApp, jTemp->valuestring, MIN(63, strlen(jTemp->valuestring)));
    }

    return 0;
}

/**********************************************************************
* @name      : uart_json_notify_only_package
* @brief     : 串口独占模式通知封装
* @param[in] : uint32 token              消息序号
               UART_ONLY_T *pMsgOnly     串口独占模式通知
               int topiclen              主题缓存容量
* @param[out]: char *topic               主题
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
char *uart_json_notify_mode_package(uint32 token, UART_MODEINFO_T *pMsgOnly, char *topic, int topiclen)
{
    cJSON *jRoot = NULL;
    cJSON *jParam = NULL;
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm;
    char   TIME[36] = {0};
    char   strtemp[36] = {0};
    char  *msgstr = NULL;
    
    if(NULL == pMsgOnly || NULL == topic || 0 == topiclen)
    {
        return NULL;
    }

    jRoot = cJSON_CreateObject();
    if(NULL == jRoot)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);

    cJSON_AddItemToObject(jRoot, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(jRoot, "timestamp", cJSON_CreateString(TIME));

    sprintf(strtemp, "%s-%u", UART_TYPE_485 == pMsgOnly->port.portType ? "RS485" : "RS232", pMsgOnly->port.portNo);
    cJSON_AddItemToObject(jRoot, "port", cJSON_CreateString(strtemp));

    cJSON_AddItemToObject(jRoot, "mode", cJSON_CreateString(UART_MODE_ONLY == pMsgOnly->mode ? "only" : UART_MODE_MASTER == pMsgOnly->mode ? "master" : "slave"));
    
    cJSON_AddItemToObject(jRoot, "lastAppName", cJSON_CreateString(pMsgOnly->lastApp));
    cJSON_AddItemToObject(jRoot, "currAppName", cJSON_CreateString(pMsgOnly->curApp));

    jParam = cJSON_CreateObject();
    if(NULL == jParam)
    {
        cJSON_Delete(jRoot);
        return NULL;
    }
    
    snprintf(topic, topiclen, "%s/Broadcast/JSON/%s/%s/%s-%u/%s", UART_APP_NAME, MQTT_TOPIC_OPER_REPORT, MQTT_TOPIC_INFO_NOTIFY,
                                                            UART_PORTSTR(&pMsgOnly->port), UART_TOPIC_MODECHG);
    msgstr = cJSON_Print(jRoot);
    cJSON_Delete(jRoot);
    return msgstr;
}

/**********************************************************************
* @name      : uart_json_get_info_req_split
* @brief     : 串口信息查询请求消息解析
* @param[in] : MESSAGE_JSON_T *pMsg      接收消息JSON格式
* @param[out]: UART_PORT_T *pRSPort      解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int uart_json_get_info_req_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort)
{
    cJSON *jRoot = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pRSPort)
    {
        return -1;
    }

    if(0 != strcmp(pMsg->destapp, UART_APP_NAME) || 0 != strcmp(pMsg->operat, MQTT_TOPIC_OPER_GET) ||
       0 != strcmp(pMsg->infotype, MQTT_TOPIC_INFO_REQ) || NULL == strstr(pMsg->infopath, UART_TOPIC_UARTINFO))
    {
        return -2;
    }

    memset(pRSPort, 0, sizeof(UART_PORT_T));
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "port");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -3;
    }
    UART_STRING_TO_PORT(jTemp->valuestring, pRSPort->portType, pRSPort->portNo);

    return 0;
}

/**********************************************************************
* @name      : uart_json_get_info_req_package
* @brief     : 串口信息查询请求消息封装
* @param[in] : uint32 token              消息序号
               char *sourapp             请求APP名称
               UART_PORT_T *pRSPort      端口
               int topiclen              主题缓存容量
* @param[out]: char *topic               主题
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
char *uart_json_get_info_req_package(uint32 token, char *sourapp, UART_PORT_T *pRSPort, char *topic, int topiclen)
{
    cJSON *jRoot = NULL;
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm;
    char   TIME[36] = {0};
    char   strtemp[36] = {0};
    char  *msgstr = NULL;
    
    if(NULL == sourapp || NULL == pRSPort || NULL == topic || 0 == topiclen)
    {
        return NULL;
    }

    jRoot = cJSON_CreateObject();
    if(NULL == jRoot)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);

    cJSON_AddItemToObject(jRoot, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(jRoot, "timestamp", cJSON_CreateString(TIME));

    sprintf(strtemp, "%s-%u", UART_PORTSTR(pRSPort));
    cJSON_AddItemToObject(jRoot, "port", cJSON_CreateString(strtemp));

    snprintf(topic, topiclen, "%s/%s/JSON/%s/%s/%s-%u/%s", sourapp, UART_APP_NAME, MQTT_TOPIC_OPER_GET, MQTT_TOPIC_INFO_REQ,
                                                           UART_PORTSTR(pRSPort), UART_TOPIC_UARTINFO);
    msgstr = cJSON_Print(jRoot);
    cJSON_Delete(jRoot);
    return msgstr;
}

/**********************************************************************
* @name      : uart_json_get_info_res_split
* @brief     : 串口信息查询响应消息解析
* @param[in] : MESSAGE_JSON_T *pMsg          接收消息JSON格式
* @param[out]: UART_MODESTAUS_T *pModeStatus 解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int uart_json_get_info_res_split(MESSAGE_JSON_T *pMsg, UART_MODESTAUS_T *pModeStatus)
{
    cJSON *jRoot = NULL;
    cJSON *jStatus = NULL;
    cJSON *jParam = NULL;
    cJSON *jList = NULL;
    cJSON *jApp = NULL;
    cJSON *jTemp = NULL;
    int    i = 0;
    
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pModeStatus)
    {
        return -1;
    }

    if(0 != strcmp(pMsg->sourapp, UART_APP_NAME) || 0 != strcmp(pMsg->operat, MQTT_TOPIC_OPER_GET) ||
       0 != strcmp(pMsg->infotype, MQTT_TOPIC_INFO_RES) || NULL == strstr(pMsg->infopath, UART_TOPIC_UARTINFO))
    {
        return -2;
    }

    memset(pModeStatus, 0, sizeof(UART_MODESTAUS_T));
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "port");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -3;
    }
    UART_STRING_TO_PORT(jTemp->valuestring, pModeStatus->port.portType, pModeStatus->port.portNo);

    jTemp = cJSON_GetObjectItem(jRoot, "mode");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -4;
    }
    UART_STRING_TO_MODE(jTemp->valuestring, pModeStatus->mode);

    jStatus = cJSON_GetObjectItem(jRoot, "status");
    if(NULL == jStatus || cJSON_Object != jStatus->type)
    {
        return -5;
    }
    
    jTemp = cJSON_GetObjectItem(jStatus, "onlyAppName");
    if(NULL != jTemp && cJSON_String == jTemp->type && 0 != strlen(jTemp->valuestring))
    {
        memcpy(pModeStatus->onlyApp, jTemp->valuestring, MIN(63, strlen(jTemp->valuestring)));
    }

    jParam = cJSON_GetObjectItem(jStatus, "param");
    if(NULL == jParam || cJSON_Object != jParam->type)
    {
        return -6;
    }
    jTemp = cJSON_GetObjectItem(jParam, "baudRate");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        UART_STRING_TO_BAUD(jTemp->valueint, pModeStatus->param.baudRate);
    }
    jTemp = cJSON_GetObjectItem(jParam, "byteSize");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
       pModeStatus->param.dataBits = jTemp->valueint;
    }
    jTemp = cJSON_GetObjectItem(jParam, "parity");
    if(NULL != jTemp && cJSON_String == jTemp->type && 0 != strlen(jTemp->valuestring))
    {
       UART_STRING_TO_PARITY(jTemp->valuestring, pModeStatus->param.parity);
    }
    jTemp = cJSON_GetObjectItem(jParam, "stopBits");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
       pModeStatus->param.stopBits = jTemp->valueint;
    }

    jList = cJSON_GetObjectItem(jStatus, "appList");
    if(NULL == jList || cJSON_Array != jList->type)
    {
        return -7;
    }
    pModeStatus->num = MIN(UART_MONITOR_MAX, cJSON_GetArraySize(jList));
    for(i = 0; i < pModeStatus->num; i++)
    {
        jApp = cJSON_GetArrayItem(jList, i);
        if(NULL == jApp || cJSON_Object != jApp->type)
        {
            continue;
        }
        jTemp = cJSON_GetObjectItem(jParam, "name");
        if(NULL != jTemp && cJSON_String == jTemp->type && 0 != strlen(jTemp->valuestring))
        {
            memcpy(pModeStatus->monitorApp[i], jTemp->valuestring, MIN(63, strlen(jTemp->valuestring)));
        }
    }
    return 0;
}

/**********************************************************************
* @name      : uart_json_get_info_res_package
* @brief     : 串口信息查询响应消息封装
* @param[in] : uint32 token                   消息序号
               char *destapp                  目的APP
               UART_MODESTAUS_T *pModeStatus  模式与状态
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
char *uart_json_get_info_res_package(uint32 token, char *destapp, UART_MODESTAUS_T *pModeStatus, char *topic, int topiclen)
{
    cJSON *jRoot = NULL;
    cJSON *jStatus = NULL;
    cJSON *jParam = NULL;
    cJSON *jMonitor = NULL;
    cJSON *jAppObj = NULL;
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm;
    char   TIME[36] = {0};
    char   strtemp[36] = {0};
    char  *msgstr = NULL;
    int    baudList[] = {300, 600, 1200, 2400, 4800, 7200, 9600, 19200, 38400, 57600, 115200};
    char  *parity[] = {"none", "odd", "even"};
    uint8  parame;
    uint8  i = 0;
    
    if(NULL == destapp || NULL == pModeStatus || NULL == topic || 0 == topiclen)
    {
        return NULL;
    }

    jRoot = cJSON_CreateObject();
    if(NULL == jRoot)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);
    cJSON_AddItemToObject(jRoot, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(jRoot, "timestamp", cJSON_CreateString(TIME));
    sprintf(strtemp, "%s-%u", UART_PORTSTR(&(pModeStatus->port)));
    cJSON_AddItemToObject(jRoot, "port", cJSON_CreateString(strtemp));
    cJSON_AddItemToObject(jRoot, "mode", cJSON_CreateString(UART_MODE_ONLY == pModeStatus->mode ? "only" : UART_MODE_MASTER == pModeStatus->mode ? "master" : "slave"));

    jStatus = cJSON_CreateObject();
    if(NULL == jStatus)
    {
        cJSON_Delete(jRoot);
        return NULL;
    }
    cJSON_AddItemToObject(jRoot, "status", jStatus);
    cJSON_AddItemToObject(jStatus, "onlyAppName", cJSON_CreateString(pModeStatus->onlyApp));

    /* 参数 */
    jParam = cJSON_CreateObject();
    if(NULL == jParam)
    {
        cJSON_Delete(jRoot);
        return NULL;
    }
    cJSON_AddItemToObject(jStatus, "param", jParam);
    parame = pModeStatus->param.baudRate;
    cJSON_AddItemToObject(jParam, "baudRate", cJSON_CreateNumber(parame > UART_BAUD_115200 ? 0 : baudList[parame]));
    cJSON_AddItemToObject(jParam, "byteSize", cJSON_CreateNumber(pModeStatus->param.dataBits));
    parame = pModeStatus->param.parity;
    cJSON_AddItemToObject(jParam, "parity", cJSON_CreateString(parame > UART_PARITY_EVEN ? "" : parity[parame]));
    cJSON_AddItemToObject(jParam, "byteSize", cJSON_CreateNumber(pModeStatus->param.stopBits));

    /* 监听APP */
    jMonitor = cJSON_CreateArray();
    if(NULL == jMonitor)
    {
        cJSON_Delete(jRoot);
        return NULL;
    }
    cJSON_AddItemToObject(jStatus, "appList", jMonitor);
    for(i = 0; i < pModeStatus->num; i++)
    {
        cJSON_AddItemToArray(jMonitor, jAppObj = cJSON_CreateObject());
        cJSON_AddItemToObject(jAppObj, "name", cJSON_CreateString(pModeStatus->monitorApp[i]));
    }

    snprintf(topic, topiclen, "%s/%s/JSON/%s/%s/%s-%u/%s", UART_APP_NAME, destapp, MQTT_TOPIC_OPER_GET, MQTT_TOPIC_INFO_RES,
                                                           UART_PORTSTR(&(pModeStatus->port)), UART_TOPIC_UARTINFO);
    msgstr = cJSON_Print(jRoot);
    cJSON_Delete(jRoot);
    return msgstr;
}

/**********************************************************************
* @name      : uart_json_set_param_req_split
* @brief     : 串口参数设置请求消息解析
* @param[in] : MESSAGE_JSON_T *pMsg       接收消息JSON格式
* @param[out]: UART_APPREG_T *pAppReg     解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int uart_json_set_param_req_split(MESSAGE_JSON_T *pMsg, UART_APPREG_T *pAppReg)
{
    cJSON *jRoot = NULL;
    cJSON *jParam = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pAppReg)
    {
        return -1;
    }

    if(0 != strcmp(pMsg->destapp, UART_APP_NAME) || 0 != strcmp(pMsg->operat, MQTT_TOPIC_OPER_SET) ||
       0 != strcmp(pMsg->infotype, MQTT_TOPIC_INFO_REQ) || NULL == strstr(pMsg->infopath, UART_TOPIC_ONLYREG))
    {
        return -2;
    }

    memset(pAppReg, 0, sizeof(UART_APPREG_T));
    UART_STRING_TO_PORT(pMsg->infopath, pAppReg->port.portType, pAppReg->port.portNo);
    
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "appName");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -3;
    }
    memcpy(pAppReg->onlyApp, jTemp->valuestring, MIN(63, strlen(jTemp->valuestring)));

    jParam = cJSON_GetObjectItem(jRoot, "param");
    if(NULL == jParam || cJSON_Object != jTemp->type)
    {
        return -4;
    }
    
    jTemp = cJSON_GetObjectItem(jParam, "baudRate");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        UART_STRING_TO_BAUD(jTemp->valueint, pAppReg->param.baudRate);
    }
    jTemp = cJSON_GetObjectItem(jParam, "byteSize");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
       pAppReg->param.dataBits = jTemp->valueint;
    }
    jTemp = cJSON_GetObjectItem(jParam, "parity");
    if(NULL != jTemp && cJSON_String == jTemp->type && 0 != strlen(jTemp->valuestring))
    {
       UART_STRING_TO_PARITY(jTemp->valuestring, pAppReg->param.parity);
    }
    jTemp = cJSON_GetObjectItem(jParam, "stopBits");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
       pAppReg->param.stopBits = jTemp->valueint;
    }

    return 0;
}

/**********************************************************************
* @name      : uart_json_set_param_req_package
* @brief     : 串口参数设置请求消息封装
* @param[in] : uint32 token                   消息序号
               char *sourapp                  请求APP
               UART_APPREG_T *pAppReg         注册APP
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
char *uart_json_set_param_req_package(uint32 token, char *sourapp, UART_APPREG_T *pAppReg, char *topic, int topiclen)
{
    cJSON *jRoot = NULL;
    cJSON *jParam = NULL;
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm;
    char   TIME[36] = {0};
    char  *msgstr = NULL;
    int    baudList[] = {300, 600, 1200, 2400, 4800, 7200, 9600, 19200, 38400, 57600, 115200};
    char  *parity[] = {"none", "odd", "even"};
    uint8  parame;
    
    if(NULL == sourapp || NULL == pAppReg || NULL == topic || 0 == topiclen)
    {
        return NULL;
    }

    jRoot = cJSON_CreateObject();
    if(NULL == jRoot)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);

    cJSON_AddItemToObject(jRoot, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(jRoot, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(jRoot, "appName", cJSON_CreateString(pAppReg->onlyApp));

    jParam = cJSON_CreateObject();
    if(NULL == jParam)
    {
        cJSON_Delete(jRoot);
        return NULL;
    }
    cJSON_AddItemToObject(jRoot, "param", jParam);

    /* 参数 */
    parame = pAppReg->param.baudRate;
    cJSON_AddItemToObject(jParam, "baudRate", cJSON_CreateNumber(parame > UART_BAUD_115200 ? 0 : baudList[parame]));
    cJSON_AddItemToObject(jParam, "byteSize", cJSON_CreateNumber(pAppReg->param.dataBits));
    parame = pAppReg->param.parity;
    cJSON_AddItemToObject(jParam, "parity", cJSON_CreateString(parame > UART_PARITY_EVEN ? "" : parity[parame]));
    cJSON_AddItemToObject(jParam, "stopBits", cJSON_CreateNumber(pAppReg->param.stopBits));

    snprintf(topic, topiclen, "%s/%s/JSON/%s/%s/%s-%u/%s", sourapp, UART_APP_NAME, MQTT_TOPIC_OPER_SET, MQTT_TOPIC_INFO_REQ,
                                                           UART_PORTSTR(&(pAppReg->port)), UART_TOPIC_ONLYPARAM);
    msgstr = cJSON_Print(jRoot);
    cJSON_Delete(jRoot);
    return msgstr;
}

/**********************************************************************
* @name      : uart_json_set_param_res_split
* @brief     : 串口参数设置响应消息解析
* @param[in] : MESSAGE_JSON_T *pMsg       接收消息JSON格式
* @param[out]: UART_PORT_T *pRSPort       解析结果
               uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int uart_json_set_param_res_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK)
{
    cJSON *jRoot = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pRSPort || NULL == bOK)
    {
        return -1;
    }

    if(0 != strcmp(pMsg->sourapp, UART_APP_NAME) || 0 != strcmp(pMsg->operat, MQTT_TOPIC_OPER_SET) ||
       0 != strcmp(pMsg->infotype, MQTT_TOPIC_INFO_RES) || NULL == strstr(pMsg->infopath, UART_TOPIC_ONLYREG))
    {
        return -2;
    }

    memset(pRSPort, 0, sizeof(UART_PORT_T));
    UART_STRING_TO_PORT(pMsg->infopath, pRSPort->portType, pRSPort->portNo);
    
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "status");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -3;
    }
    
    *bOK = (0 == strcmp(jTemp->valuestring, "OK") || 0 == strcmp(jTemp->valuestring, "ok")) ? TRUE : FALSE;
    return 0;
}

/**********************************************************************
* @name      : uart_json_set_param_res_package
* @brief     : 串口参数设置响应消息封装
* @param[in] : uint32 token                   消息序号
               char *destapp                  目的APP
               UART_PORT_T *pRSPort           端口
               char *status                   结果
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
char *uart_json_set_param_res_package(uint32 token, char *destapp, UART_PORT_T *pRSPort, char *status, char *topic, int topiclen)
{
    cJSON *jRoot = NULL;
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm;
    char   TIME[36] = {0};
    char  *msgstr = NULL;
    
    if(NULL == destapp || NULL == pRSPort || NULL == status || NULL == topic || 0 == topiclen)
    {
        return NULL;
    }

    jRoot = cJSON_CreateObject();
    if(NULL == jRoot)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);
    cJSON_AddItemToObject(jRoot, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(jRoot, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(jRoot, "status", cJSON_CreateString(status));

    snprintf(topic, topiclen, "%s/%s/JSON/%s/%s/%s-%u/%s", UART_APP_NAME, destapp, MQTT_TOPIC_OPER_SET, MQTT_TOPIC_INFO_RES,
                                                           UART_PORTSTR(pRSPort), UART_TOPIC_ONLYPARAM);
    msgstr = cJSON_Print(jRoot);
    cJSON_Delete(jRoot);
    return msgstr;
}

/**********************************************************************
* @name      : uart_json_set_monitor_req_split
* @brief     : 数据监听注册请求消息解析
* @param[in] : MESSAGE_JSON_T *pMsg       接收消息JSON格式
* @param[out]: UART_APPREG_T *pAppReg     解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int uart_json_set_monitor_req_split(MESSAGE_JSON_T *pMsg, UART_APPREG_T *pAppReg)
{
    cJSON *jRoot = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pAppReg)
    {
        return -1;
    }

    if(0 != strcmp(pMsg->destapp, UART_APP_NAME) || 0 != strcmp(pMsg->operat, MQTT_TOPIC_OPER_SET) ||
       0 != strcmp(pMsg->infotype, MQTT_TOPIC_INFO_REQ) || NULL == strstr(pMsg->infopath, UART_TOPIC_MONITORREG))
    {
        return -2;
    }

    memset(pAppReg, 0, sizeof(UART_APPREG_T));
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "appName");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -3;
    }
    memcpy(pAppReg->onlyApp, jTemp->valuestring, MIN(63, strlen(jTemp->valuestring)));

    jTemp = cJSON_GetObjectItem(jRoot, "port");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -4;
    }
    UART_STRING_TO_PORT(jTemp->valuestring, pAppReg->port.portType, pAppReg->port.portNo);

    return 0;
}

/**********************************************************************
* @name      : uart_json_set_monitor_req_package
* @brief     : 数据监听注册请求请求消息封装
* @param[in] : uint32 token                   消息序号
               char *sourapp                  请求APP
               UART_APPREG_T *pAppReg         注册APP
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
char *uart_json_set_monitor_req_package(uint32 token, char *sourapp, UART_APPREG_T *pAppReg, char *topic, int topiclen)
{
    cJSON *jRoot = NULL;
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm;
    char   TIME[36] = {0};
    char   strtemp[36] = {0};
    char  *msgstr = NULL;
    
    if(NULL == sourapp || NULL == pAppReg || NULL == topic || 0 == topiclen)
    {
        return NULL;
    }

    jRoot = cJSON_CreateObject();
    if(NULL == jRoot)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);

    cJSON_AddItemToObject(jRoot, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(jRoot, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(jRoot, "appName", cJSON_CreateString(pAppReg->onlyApp));
    sprintf(strtemp, "%s-%u", UART_PORTSTR(&(pAppReg->port)));
    cJSON_AddItemToObject(jRoot, "port", cJSON_CreateString(strtemp));

    snprintf(topic, topiclen, "%s/%s/JSON/%s/%s/%s-%u/%s", sourapp, UART_APP_NAME, MQTT_TOPIC_OPER_SET, MQTT_TOPIC_INFO_REQ,
                                                           UART_PORTSTR(&(pAppReg->port)), UART_TOPIC_MONITORREG);
    msgstr = cJSON_Print(jRoot);
    cJSON_Delete(jRoot);
    return msgstr;
}

/**********************************************************************
* @name      : uart_json_set_monitor_res_split
* @brief     : 数据监听注册请求响应消息解析
* @param[in] : MESSAGE_JSON_T *pMsg       接收消息JSON格式
* @param[out]: UART_PORT_T *pRSPort       解析结果
               uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int uart_json_set_monitor_res_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK)
{
    cJSON *jRoot = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pRSPort || NULL == bOK)
    {
        return -1;
    }

    if(0 != strcmp(pMsg->sourapp, UART_APP_NAME) || 0 != strcmp(pMsg->operat, MQTT_TOPIC_OPER_SET) ||
       0 != strcmp(pMsg->infotype, MQTT_TOPIC_INFO_RES) || NULL == strstr(pMsg->infopath, UART_TOPIC_MONITORREG))
    {
        return -2;
    }

    memset(pRSPort, 0, sizeof(UART_PORT_T));
    UART_STRING_TO_PORT(pMsg->infopath, pRSPort->portType, pRSPort->portNo);
    
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "status");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -3;
    }
    
    *bOK = (0 == strcmp(jTemp->valuestring, "OK") || 0 == strcmp(jTemp->valuestring, "ok")) ? TRUE : FALSE;
    return 0;
}

/**********************************************************************
* @name      : uart_json_set_monitor_res_package
* @brief     : 数据监听注册请求响应消息封装
* @param[in] : uint32 token                   消息序号
               char *destapp                  目的APP
               UART_PORT_T *pRSPort           端口
               char *status                   结果
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
char *uart_json_set_monitor_res_package(uint32 token, char *destapp, UART_PORT_T *pRSPort, char *status, char *topic, int topiclen)
{
    cJSON *jRoot = NULL;
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm;
    char   TIME[36] = {0};
    char  *msgstr = NULL;
    
    if(NULL == destapp || NULL == pRSPort || NULL == status || NULL == topic || 0 == topiclen)
    {
        return NULL;
    }

    jRoot = cJSON_CreateObject();
    if(NULL == jRoot)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);
    cJSON_AddItemToObject(jRoot, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(jRoot, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(jRoot, "status", cJSON_CreateString(status));

    snprintf(topic, topiclen, "%s/%s/JSON/%s/%s/%s-%u/%s", UART_APP_NAME, destapp, MQTT_TOPIC_OPER_SET, MQTT_TOPIC_INFO_RES,
                                                           UART_PORTSTR(pRSPort), UART_TOPIC_MONITORREG);
    msgstr = cJSON_Print(jRoot);
    cJSON_Delete(jRoot);
    return msgstr;
}

/**********************************************************************
* @name      : uart_json_data_send_split
* @brief     : 数据发送消息解析
* @param[in] : MESSAGE_JSON_T *pMsg           接收消息JSON格式
* @param[out]: UART_SEND_DATA_T *pSendMsg     解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int uart_json_data_send_split(MESSAGE_JSON_T *pMsg, UART_SEND_DATA_T *pSendMsg)
{
    cJSON *jRoot = NULL;
    cJSON *jParam = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pSendMsg)
    {
        return -1;
    }

    if(0 != strcmp(pMsg->destapp, UART_APP_NAME) || NULL == strstr(pMsg->infopath, UART_TOPIC_DATA))
    {
        return -2;
    }

    memset(pSendMsg, 0, sizeof(UART_SEND_DATA_T));
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "port");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -3;
    }
    UART_STRING_TO_PORT(jTemp->valuestring, pSendMsg->port.portType, pSendMsg->port.portNo);

    jTemp = cJSON_GetObjectItem(jRoot, "prio");
    if(NULL != jTemp && cJSON_String == jTemp->type && 0 != strlen(jTemp->valuestring))
    {
        pSendMsg->prio = atoi(jTemp->valuestring);
    }

    jTemp = cJSON_GetObjectItem(jRoot, "prm");
    if(NULL != jTemp && cJSON_String == jTemp->type && 0 != strlen(jTemp->valuestring))
    {
        pSendMsg->prm = atoi(jTemp->valuestring);
    }
    
    jTemp = cJSON_GetObjectItem(jRoot, "frametimeout");
    if(NULL != jTemp && cJSON_String == jTemp->type && 0 != strlen(jTemp->valuestring))
    {
        pSendMsg->frametimeout = atoi(jTemp->valuestring);
    }

    jTemp = cJSON_GetObjectItem(jRoot, "tasktimeout");
    if(NULL != jTemp && cJSON_String == jTemp->type && 0 != strlen(jTemp->valuestring))
    {
        pSendMsg->tasktimeout = atoi(jTemp->valuestring);
    }

    jParam = cJSON_GetObjectItem(jRoot, "param");
    if(NULL != jParam && cJSON_Object == jTemp->type)
    {
        pSendMsg->bParam = 1;
        jTemp = cJSON_GetObjectItem(jParam, "baudRate");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
            UART_STRING_TO_BAUD(jTemp->valueint, pSendMsg->param.baudRate);
        }
        jTemp = cJSON_GetObjectItem(jParam, "byteSize");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
           pSendMsg->param.dataBits = jTemp->valueint;
        }
        jTemp = cJSON_GetObjectItem(jParam, "parity");
        if(NULL != jTemp && cJSON_String == jTemp->type && 0 != strlen(jTemp->valuestring))
        {
           UART_STRING_TO_PARITY(jTemp->valuestring, pSendMsg->param.parity);
        }
        jTemp = cJSON_GetObjectItem(jParam, "stopBits");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
           pSendMsg->param.stopBits = jTemp->valueint;
        }
    }

    jTemp = cJSON_GetObjectItem(jRoot, "estsize");
    if(NULL != jTemp && cJSON_String == jTemp->type && 0 != strlen(jTemp->valuestring))
    {
        pSendMsg->estsize = atoi(jTemp->valuestring);
    }

    jTemp = cJSON_GetObjectItem(jRoot, "data");
    if(NULL != jTemp && cJSON_String == jTemp->type && 0 != strlen(jTemp->valuestring))
    {
        uint32 len = 0;
        if(0 != db_base64_to_data(jTemp->valuestring, pSendMsg->data, UART_DATA_MAXLEN, &len))
        {
            return -4;
        }
        pSendMsg->datalen = len;
    }
    return 0;
}

/**********************************************************************
* @name      : uart_json_data_send_package
* @brief     : 数据发送消息封装
* @param[in] : uint32 token                   消息序号
               char *sourapp                  请求APP
               UART_SEND_DATA_T *pSendMsg     发送内容
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
char *uart_json_data_send_package(uint32 token, char *sourapp, UART_SEND_DATA_T *pSendMsg, char *topic, int topiclen)
{
    cJSON *jRoot = NULL;
    cJSON *jParam = NULL;
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm;
    char   TIME[36] = {0};
    char   strtemp[36] = {0};
    char  *datastr = NULL;
    int    baudList[] = {300, 600, 1200, 2400, 4800, 7200, 9600, 19200, 38400, 57600, 115200};
    char  *parity[] = {"none", "odd", "even"};
    uint8  parame;
    char  *msgstr = NULL;
    
    if(NULL == sourapp || NULL == pSendMsg || NULL == topic || 0 == topiclen)
    {
        return NULL;
    }

    jRoot = cJSON_CreateObject();
    if(NULL == jRoot)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);
    cJSON_AddItemToObject(jRoot, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(jRoot, "timestamp", cJSON_CreateString(TIME));
    
    sprintf(strtemp, "%s-%u", UART_PORTSTR(&(pSendMsg->port)));
    cJSON_AddItemToObject(jRoot, "port", cJSON_CreateString(strtemp));
    
    cJSON_AddItemToObject(jRoot, "prio", cJSON_CreateNumber(pSendMsg->prio));
    
    cJSON_AddItemToObject(jRoot, "prm", cJSON_CreateNumber(pSendMsg->prm));

    cJSON_AddItemToObject(jRoot, "byteTimeout", cJSON_CreateNumber(pSendMsg->bytetimeout));
    
    //sprintf(strtemp, "%u", pSendMsg->frametimeout);
    cJSON_AddItemToObject(jRoot, "frametimeout", cJSON_CreateNumber(pSendMsg->frametimeout));

    //sprintf(strtemp, "%u",pSendMsg->tasktimeout);
    cJSON_AddItemToObject(jRoot, "tasktimeout", cJSON_CreateNumber(pSendMsg->tasktimeout));

    if(1 == pSendMsg->bParam)
    {
        /* 参数 */
        jParam = cJSON_CreateObject();
        if(NULL == jParam)
        {
            cJSON_Delete(jRoot);
            return NULL;
        }
        cJSON_AddItemToObject(jRoot, "param", jParam);
        parame = pSendMsg->param.baudRate;
        cJSON_AddItemToObject(jParam, "baudRate", cJSON_CreateNumber(parame > UART_BAUD_115200 ? 0 : baudList[parame]));
        cJSON_AddItemToObject(jParam, "byteSize", cJSON_CreateNumber(pSendMsg->param.dataBits));
        parame = pSendMsg->param.parity;
        cJSON_AddItemToObject(jParam, "parity", cJSON_CreateString(parame > UART_PARITY_EVEN ? "" : parity[parame]));
        cJSON_AddItemToObject(jParam, "stopBits", cJSON_CreateNumber(pSendMsg->param.stopBits));
    }
    
    cJSON_AddItemToObject(jRoot, "estsize", cJSON_CreateNumber(pSendMsg->estsize));

    if(0 == pSendMsg->datalen)
    {
        cJSON_AddItemToObject(jRoot, "data", cJSON_CreateString(""));
    }
    else
    {
        datastr = db_data_to_base64(pSendMsg->data, pSendMsg->datalen);
        if(NULL == datastr)
        {
            cJSON_Delete(jRoot);
            return NULL;
        }
        cJSON_AddItemToObject(jRoot, "data", cJSON_CreateString(datastr));
    }
    
    snprintf(topic, topiclen, "%s/%s/JSON/%s/%s/%s-%u/%s", sourapp, UART_APP_NAME, MQTT_TOPIC_OPER_TRANS,MQTT_TOPIC_INFO_NOTIFY,
                                                                    UART_PORTSTR(&(pSendMsg->port)), UART_TOPIC_DATA);
    msgstr = cJSON_Print(jRoot);
    cJSON_Delete(jRoot);
    if(datastr)
    {
        free(datastr);
    }
    return msgstr;
}

/**********************************************************************
* @name      : uart_json_data_recv_split
* @brief     : 数据接收消息解析
* @param[in] : MESSAGE_JSON_T *pMsg           接收消息JSON格式
* @param[out]: UART_RECV_DATA_T *pRecvMsg     解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int uart_json_data_recv_split(MESSAGE_JSON_T *pMsg, UART_RECV_DATA_T *pRecvMsg)
{
    cJSON *jRoot = NULL;
    cJSON *jTemp = NULL;
    uint32 len = 0;
        
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pRecvMsg)
    {
        return -1;
    }

    if(0 != strcmp(pMsg->sourapp, UART_APP_NAME) || NULL == strstr(pMsg->infopath, UART_TOPIC_DATA))
    {
        return -2;
    }

    memset(pRecvMsg, 0, sizeof(UART_RECV_DATA_T));
    
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "prm");
    if(NULL == jTemp || cJSON_Number != jTemp->type )
    {
        return -3;
    }
    pRecvMsg->prm = jTemp->valuedouble;
    
    jTemp = cJSON_GetObjectItem(jRoot, "port");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -4;
    }
    UART_STRING_TO_PORT(jTemp->valuestring, pRecvMsg->port.portType, pRecvMsg->port.portNo);

    jTemp = cJSON_GetObjectItem(jRoot, "status");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -4;
    }
    UART_STRING_TO_MSGSTATUS(jTemp->valuestring, pRecvMsg->status);

    jTemp = cJSON_GetObjectItem(jRoot, "data");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -5;
    }
    
    if(0 != db_base64_to_data(jTemp->valuestring, pRecvMsg->data, UART_DATA_MAXLEN, &len))
    {
        return -4;
    }
    pRecvMsg->datalen = len;
    
    return 0;
}


/**********************************************************************
* @name      : uart_json_data_recv_split
* @brief     : 数据接收消息解析
* @param[in] : MESSAGE_JSON_T *pMsg           接收消息JSON格式
* @param[out]: UART_RECV_DATA_T *pRecvMsg     解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int cco_json_data_recv_split(MESSAGE_JSON_T *pMsg, UART_RECV_DATA_T *pRecvMsg)
{
    cJSON *jRoot = NULL;
    cJSON *jTemp = NULL;
    uint32 len = 0;
        
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pRecvMsg)
    {
        return -1;
    }

    if(0 != strcmp(pMsg->sourapp, "ccoRouter") || 0 == strstr(pMsg->infopath, "dataTrans"))
    {
        return -2;
    }

    memset(pRecvMsg, 0, sizeof(UART_RECV_DATA_T));
    
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "data");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -5;
    }
    
    if(0 != db_base64_to_data(jTemp->valuestring, pRecvMsg->data, UART_DATA_MAXLEN, &len))
    {
        return -4;
    }
    pRecvMsg->datalen = len;
    
    jTemp = cJSON_GetObjectItem(jRoot, "status");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -3;
    }
    pRecvMsg->prm = atoi(jTemp->valuestring);
    

    jTemp = cJSON_GetObjectItem(jRoot, "reason");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -4;
    }
    UART_STRING_TO_MSGSTATUS(jTemp->valuestring, pRecvMsg->status);


    
    return 0;
}


/**********************************************************************
* @name      : uart_json_data_recv_package
* @brief     : 数据接收消息封装
* @param[in] : uint32 token                   消息序号
               char *destapp                  目的APP
               UART_RECV_DATA_T *pRecvMsg     接收消息
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
char *uart_json_data_recv_package(uint32 token, char *destapp, UART_RECV_DATA_T *pRecvMsg, char *topic, int topiclen)
{
    cJSON *jRoot = NULL;
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm;
    char   TIME[36] = {0};
    char   strtemp[36] = {0};
    char  *status[] = {"OK", "TaskTimeOut", "TaskFull", "ParamSerialError", "DataError"};
    char  *datastr = NULL;
    char  *msgstr = NULL;
    
    if(NULL == destapp || NULL == pRecvMsg || NULL == topic || 0 == topiclen)
    {
        return NULL;
    }

    jRoot = cJSON_CreateObject();
    if(NULL == jRoot)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);
    cJSON_AddItemToObject(jRoot, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(jRoot, "timestamp", cJSON_CreateString(TIME));

    cJSON_AddItemToObject(jRoot, "prm", cJSON_CreateNumber(pRecvMsg->prm));
    
    sprintf(strtemp, "%s-%u", UART_PORTSTR(&(pRecvMsg->port)));
    cJSON_AddItemToObject(jRoot, "port", cJSON_CreateString(strtemp));
    
    cJSON_AddItemToObject(jRoot, "status", 
                          cJSON_CreateString(pRecvMsg->status <= UART_RESULT_DATA_ERR ? status[pRecvMsg->status] : "Other Error"));

    if(0 == pRecvMsg->datalen)
    {
        cJSON_AddItemToObject(jRoot, "data", cJSON_CreateString(""));
    }
    else
    {
        datastr = db_data_to_base64(pRecvMsg->data, pRecvMsg->datalen);
        if(NULL == datastr)
        {
            cJSON_Delete(jRoot);
            return NULL;
        }
        cJSON_AddItemToObject(jRoot, "data", cJSON_CreateString(datastr));
    }
    snprintf(topic, topiclen, "%s/%s/JSON/%s/%s/%s-%u/%s", UART_APP_NAME, destapp, MQTT_TOPIC_OPER_TRANS,MQTT_TOPIC_INFO_NOTIFY,
                                                           UART_PORTSTR(&pRecvMsg->port), UART_TOPIC_DATA);
    msgstr = cJSON_Print(jRoot);
    cJSON_Delete(jRoot);
    if(datastr)
    {
        free(datastr);
    }
    return msgstr;
}

/**********************************************************************
* @name      : uart_json_set_mode_req_split
* @brief     : 串口模式设置请求消息解析
* @param[in] : MESSAGE_JSON_T *pMsg           接收消息JSON格式
* @param[out]: UART_MODE_T *pMode             解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int uart_json_set_mode_req_split(MESSAGE_JSON_T *pMsg, UART_MODE_T *pMode)
{
    cJSON *jRoot = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pMode)
    {
        return -1;
    }

    if(0 != strcmp(pMsg->destapp, UART_APP_NAME) || 0 != strcmp(pMsg->operat, MQTT_TOPIC_OPER_SET) ||
       0 != strcmp(pMsg->infotype, MQTT_TOPIC_INFO_REQ) || NULL == strstr(pMsg->infopath, UART_TOPIC_WORKMODE))
    {
        return -2;
    }

    memset(pMode, 0, sizeof(UART_MODE_T));
    UART_STRING_TO_PORT(pMsg->infopath, pMode->port.portType, pMode->port.portNo);
    
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "mode");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -3;
    }
    UART_STRING_TO_MODE(jTemp->valuestring, pMode->mode);
    
    jTemp = cJSON_GetObjectItem(jRoot, "appName");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -4;
    }
    memcpy(pMode->AppName, jTemp->valuestring, MIN(63, strlen(jTemp->valuestring)));

    return 0;
}

/**********************************************************************
* @name      : uart_json_set_mode_req_package
* @brief     : 串口模式设置请求消息封装
* @param[in] : uint32 token                   消息序号
               char *sourapp                  请求APP
               UART_SEND_DATA_T *pSendMsg     发送内容
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
char *uart_json_set_mode_req_package(uint32 token, char *sourapp, UART_MODE_T *pMode, char *topic, int topiclen)
{
    cJSON *jRoot = NULL;
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm;
    char   TIME[36] = {0};
    char  *msgstr = NULL;
    
    if(NULL == sourapp || NULL == pMode || NULL == topic || 0 == topiclen)
    {
        return NULL;
    }

    jRoot = cJSON_CreateObject();
    if(NULL == jRoot)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);

    cJSON_AddItemToObject(jRoot, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(jRoot, "timestamp", cJSON_CreateString(TIME));
     cJSON_AddItemToObject(jRoot, "mode", cJSON_CreateString(UART_MODE_ONLY == pMode->mode ? "only" : "share"));
    cJSON_AddItemToObject(jRoot, "appName", cJSON_CreateString(pMode->AppName));

    snprintf(topic, topiclen, "%s/%s/JSON/%s/%s/%s-%u/%s", sourapp, UART_APP_NAME, MQTT_TOPIC_OPER_SET, MQTT_TOPIC_INFO_REQ,
                                                           UART_PORTSTR(&(pMode->port)), UART_TOPIC_WORKMODE);
    msgstr = cJSON_Print(jRoot);
    cJSON_Delete(jRoot);
    return msgstr;
}

/**********************************************************************
* @name      : uart_json_set_mode_res_split
* @brief     : 串口模式设置响应消息解析
* @param[in] : MESSAGE_JSON_T *pMsg       接收消息JSON格式
* @param[out]: UART_PORT_T *pRSPort       解析结果
               uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int uart_json_set_mode_res_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK)
{
    cJSON *jRoot = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pRSPort || NULL == bOK)
    {
        return -1;
    }

    if(0 != strcmp(pMsg->sourapp, UART_APP_NAME) || 0 != strcmp(pMsg->operat, MQTT_TOPIC_OPER_SET) ||
       0 != strcmp(pMsg->infotype, MQTT_TOPIC_INFO_RES) || NULL == strstr(pMsg->infopath, UART_TOPIC_WORKMODE))
    {
        return -2;
    }

    memset(pRSPort, 0, sizeof(UART_PORT_T));
    UART_STRING_TO_PORT(pMsg->infopath, pRSPort->portType, pRSPort->portNo);
    
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "status");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -3;
    }
    
    *bOK = (0 == strcmp(jTemp->valuestring, "OK") || 0 == strcmp(jTemp->valuestring, "ok")) ? TRUE : FALSE;
    return 0;
}

/**********************************************************************
* @name      : uart_json_set_mode_res_package
* @brief     : 串口模式设置响应消息封装
* @param[in] : uint32 token                   消息序号
               char *destapp                  目的APP
               UART_PORT_T *pRSPort           端口
               char *status                   结果
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
char *uart_json_set_mode_res_package(uint32 token, char *destapp, UART_PORT_T *pRSPort, char *status, char *topic, int topiclen)
{
    cJSON *jRoot = NULL;
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm;
    char   TIME[36] = {0};
    char  *msgstr = NULL;
    
    if(NULL == destapp || NULL == pRSPort || NULL == status || NULL == topic || 0 == topiclen)
    {
        return NULL;
    }

    jRoot = cJSON_CreateObject();
    if(NULL == jRoot)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);
    cJSON_AddItemToObject(jRoot, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(jRoot, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(jRoot, "status", cJSON_CreateString(status));

    snprintf(topic, topiclen, "%s/%s/JSON/%s/%s/%s-%u/%s", UART_APP_NAME, destapp, MQTT_TOPIC_OPER_SET, MQTT_TOPIC_INFO_RES,
                                                           UART_PORTSTR(pRSPort), UART_TOPIC_WORKMODE);
    msgstr = cJSON_Print(jRoot);
    cJSON_Delete(jRoot);
    return msgstr;
}

/**********************************************************************
* @name      : uart_json_set_abnormal_req_split
* @brief     : 串口异常报告请求消息解析
* @param[in] : MESSAGE_JSON_T *pMsg           接收消息JSON格式
* @param[out]: UART_ABNORMAL_T *pAbnormal     解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int uart_json_set_abnormal_req_split(MESSAGE_JSON_T *pMsg, UART_ABNORMAL_T *pAbnormal)
{
    cJSON *jRoot = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pAbnormal)
    {
        return -1;
    }

    if(0 != strcmp(pMsg->destapp, UART_APP_NAME) || 0 != strcmp(pMsg->operat, MQTT_TOPIC_OPER_SET) ||
       0 != strcmp(pMsg->infotype, MQTT_TOPIC_INFO_REQ) || NULL == strstr(pMsg->infopath, UART_TOPIC_ABNORMAL))
    {
        return -2;
    }

    memset(pAbnormal, 0, sizeof(UART_ABNORMAL_T));
    UART_STRING_TO_PORT(pMsg->infopath, pAbnormal->port.portType, pAbnormal->port.portNo);
    
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "appName");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -3;
    }
    memcpy(pAbnormal->AppName, jTemp->valuestring, MIN(63, strlen(jTemp->valuestring)));

    jTemp = cJSON_GetObjectItem(jRoot, "cause");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -4;
    }
    if(0 == strcmp(jTemp->valuestring, "unknown"))
    {
        pAbnormal->cause = UART_ABNORMAL_ERROR_ANSWER;
    }
    else if(0 == strcmp(jTemp->valuestring, "unknown code"))
    {
        pAbnormal->cause = UART_ABNORMAL_UNKNOW_CODE;
    }
    else
    {
        return -5;
    }

    return 0;
}

/**********************************************************************
* @name      : uart_json_set_abnormal_req_package
* @brief     : 串口异常报告请求消息封装
* @param[in] : uint32 token                   消息序号
               char *sourapp                  请求APP
               UART_ABNORMAL_T *pAbnormal     异常信息
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
char *uart_json_set_abnormal_req_package(uint32 token, char *sourapp, UART_ABNORMAL_T *pAbnormal, char *topic, int topiclen)
{
    cJSON *jRoot = NULL;
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm;
    char   TIME[36] = {0};
    char  *msgstr = NULL;
    
    if(NULL == sourapp || NULL == pAbnormal || NULL == topic || 0 == topiclen)
    {
        return NULL;
    }

    jRoot = cJSON_CreateObject();
    if(NULL == jRoot)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);

    cJSON_AddItemToObject(jRoot, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(jRoot, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(jRoot, "appName", cJSON_CreateString(pAbnormal->AppName));
    if(UART_ABNORMAL_ERROR_ANSWER == pAbnormal->cause)
    {
        cJSON_AddItemToObject(jRoot, "cause", cJSON_CreateString("unknown"));
    }
    else if(UART_ABNORMAL_ERROR_ANSWER == pAbnormal->cause)
    {
        cJSON_AddItemToObject(jRoot, "cause", cJSON_CreateString("unknown code"));
    }
    else
    {
        cJSON_Delete(jRoot);
        return NULL;
    }

    snprintf(topic, topiclen, "%s/%s/JSON/%s/%s/%s-%u/%s", sourapp, UART_APP_NAME, MQTT_TOPIC_OPER_SET, MQTT_TOPIC_INFO_REQ,
                                                           UART_PORTSTR(&(pAbnormal->port)), UART_TOPIC_ABNORMAL);
    msgstr = cJSON_Print(jRoot);
    cJSON_Delete(jRoot);
    return msgstr;
}

/**********************************************************************
* @name      : uart_json_set_abnormal_res_split
* @brief     : 串口异常报告响应消息解析
* @param[in] : MESSAGE_JSON_T *pMsg       接收消息JSON格式
* @param[out]: UART_PORT_T *pRSPort       解析结果
               uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
int uart_json_set_abnormal_res_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK)
{
    cJSON *jRoot = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == pMsg || NULL == pMsg->pObject || NULL == pRSPort || NULL == bOK)
    {
        return -1;
    }

    if(0 != strcmp(pMsg->sourapp, UART_APP_NAME) || 0 != strcmp(pMsg->operat, MQTT_TOPIC_OPER_SET) ||
       0 != strcmp(pMsg->infotype, MQTT_TOPIC_INFO_RES) || NULL == strstr(pMsg->infopath, UART_TOPIC_ABNORMAL))
    {
        return -2;
    }

    memset(pRSPort, 0, sizeof(UART_PORT_T));
    UART_STRING_TO_PORT(pMsg->infopath, pRSPort->portType, pRSPort->portNo);
    
    jRoot = (cJSON*)pMsg->pObject;
    jTemp = cJSON_GetObjectItem(jRoot, "status");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -3;
    }
    
    *bOK = (0 == strcmp(jTemp->valuestring, "OK") || 0 == strcmp(jTemp->valuestring, "ok")) ? TRUE : FALSE;
    return 0;
}

/**********************************************************************
* @name      : uart_json_set_abnormal_res_package
* @brief     : 串口异常报告响应消息封装
* @param[in] : uint32 token                   消息序号
               UART_PORT_T *pRSPort           端口
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-7-31
* @Update    :
**********************************************************************/
char *uart_json_set_abnormal_res_package(uint32 token, char *destapp, UART_PORT_T *pRSPort, char *status, char *topic, int topiclen)
{
    cJSON *jRoot = NULL;
    struct timeval  tv;
    struct timezone tz;
    struct tm       tm;
    char   TIME[36] = {0};
    char  *msgstr = NULL;
    
    if(NULL == destapp || NULL == pRSPort || NULL == status || NULL == topic || 0 == topiclen)
    {
        return NULL;
    }

    jRoot = cJSON_CreateObject();
    if(NULL == jRoot)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);
    cJSON_AddItemToObject(jRoot, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(jRoot, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(jRoot, "status", cJSON_CreateString(status));

    snprintf(topic, topiclen, "%s/%s/JSON/%s/%s/%s-%u/%s", UART_APP_NAME, destapp, MQTT_TOPIC_OPER_SET, MQTT_TOPIC_INFO_RES,
                                                           UART_PORTSTR(pRSPort), UART_TOPIC_ABNORMAL);
    msgstr = cJSON_Print(jRoot);
    cJSON_Delete(jRoot);
    return msgstr;
}

#endif

#if DESC("AXDR消息接口", 1)

/**********************************************************************
* @name      : uart_json_notify_only_split
* @brief     : 串口独占模式通知解析
* @param[in] : MESSAGE_JSON_T *pMsg      接收消息JSON格式
* @param[out]: UART_ONLY_T *pMsgOnly     解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int uart_axdr_notify_mode_split(MESSAGE_INFO_T *pMsg, UART_MODEINFO_T *pMsgData)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;
    uint8 byteNum = 0;  //长度字节数
    uint32 lengh  = 0;  //长度值
    uint32 port = 0;

    if(NULL == pMsg || NULL == pMsgData)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_NOTIFY_MODE)
    {
        return -2;
    }

    memset(pMsgData, 0, sizeof(UART_MODEINFO_T));
    msgbuf = pMsg->playload;

    MEMCPY_R(&port, &msgbuf[offset],  sizeof(uint32));
    offset += sizeof(uint32);

#ifndef PRODUCT_SCU
    UART_OAD_TO_GPORT(port, pMsgData->port.portType, pMsgData->port.portNo);
#else
    if(0 < port && port <= 4)
    {
        pMsgData->port.portType = UART_TYPE_485;
        pMsgData->port.portNo = port;
    }
    else if(5 <= port && port <= 6)
    {
        pMsgData->port.portType = UART_TYPE_232;
        pMsgData->port.portNo = port - 4;
    }
    else
    {
        return -3;
    }
#endif

    pMsgData->mode = msgbuf[offset++];
    
    //OPTIONAL 上个独占APP
    if(msgbuf[offset++] == 1)
    {
        //visible-string
        GET_LEN_OFFSET(lengh, &msgbuf[offset], byteNum);
        offset += byteNum;
        if(offset + lengh >= pMsg->msglen)
        {
            return -4;
        }
        
        memcpy(&pMsgData->lastApp[0], &msgbuf[offset], lengh); 
        offset += lengh;    
    }

    //OPTIONAL 当前独占APP
    if(msgbuf[offset++] == 1)
    {
        //visible-string
        GET_LEN_OFFSET(lengh, &msgbuf[offset], byteNum);
        offset += byteNum;
        if(offset + lengh > pMsg->msglen)
        {
            return -5;
        }
        
        memcpy(&pMsgData->curApp[0], &msgbuf[offset], lengh); 
        offset += lengh;    
    }

    return 0;
}

/**********************************************************************
* @name      : uart_json_notify_only_package
* @brief     : 串口独占模式通知封装
* @param[in] : uint32 token              消息序号
               UART_ONLY_T *pMsgOnly     串口独占模式通知
               int topiclen              主题缓存容量
* @param[out]: char *topic               主题
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
uint16 uart_axdr_notify_mode_package(UART_MODEINFO_T *pMsgData, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;
    uint32 port = 0;
    uint8 byteNum = 0;  //长度字节数
    uint32 lengh  = 0;  //长度值

    //端口号
#ifndef PRODUCT_SCU
    UART_GPORT_TO_OAD(port, pMsgData->port.portType, pMsgData->port.portNo&0x000000ff);
    MEMCPY_R(&payload[offset], &port, sizeof(uint32));
    offset += sizeof(uint32);
#else    
    if(pMsgData->port.portType == UART_TYPE_485) 
    {
        port = pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
    else
    {
        port = 4 + pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
#endif

    //串口模式
    payload[offset++] = pMsgData->mode;

    if(pMsgData->mode == UART_MODE_ONLY)
    {
        //OPTIONAL 上个独占APP
        payload[offset++] = 1;
        lengh = strlen(pMsgData->lastApp)+1;
        
        SET_LEN_OFFSET(lengh, &payload[offset], byteNum);
        offset += byteNum;
        memcpy(&payload[offset], pMsgData->lastApp, lengh);
        offset += lengh;
        
        //OPTIONAL 当前独占APP
        payload[offset++] = 1;
        lengh = strlen(pMsgData->curApp)+1;
        
        SET_LEN_OFFSET(lengh, &payload[offset], byteNum);
        offset += byteNum; 
        memcpy(&payload[offset], pMsgData->curApp, lengh);
        offset += lengh;
    }
    else
    {
        payload[offset++] = 0;
        payload[offset++] = 0;
    }
    
    return offset;
}

/**********************************************************************
* @name      : uart_axdr_get_info_req_split
* @brief     : 串口信息查询请求消息解析
* @param[in] : MESSAGE_JSON_T *pMsg      接收消息JSON格式
* @param[out]: UART_PORT_T *pRSPort      解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int uart_axdr_get_info_req_split(MESSAGE_INFO_T *pMsg, UART_PORT_T *pMsgData)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;
    uint32 port = 0;

    if(NULL == pMsg || NULL == pMsgData)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_GET_INFO)
    {
        return -2;
    }

    memset(pMsgData, 0, sizeof(UART_PORT_T));
    msgbuf = pMsg->playload;

    MEMCPY_R(&port, &msgbuf[offset],  sizeof(uint32));
    offset += sizeof(uint32);
    
#ifndef PRODUCT_SCU
    UART_OAD_TO_GPORT(port, pMsgData->portType, pMsgData->portNo);
#else
    if(0 < port && port <= 4)
    {
        pMsgData->portType = UART_TYPE_485;
        pMsgData->portNo = port;
    }
    else if(5 <= port && port <= 6)
    {
        pMsgData->portType = UART_TYPE_232;
        pMsgData->portNo = port - 4;
    }
    else
    {
        return -3;
    }
#endif

    return 0;
}


/**********************************************************************
* @name      : uart_axdr_get_info_req_package
* @brief     : 串口信息查询请求消息封装
* @param[in] : uint32 token              消息序号
               char *sourapp             请求APP名称
               UART_PORT_T *pRSPort      端口
               int topiclen              主题缓存容量
* @param[out]: uint8 *payload            负荷内容
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
uint16 uart_axdr_get_info_req_package(UART_PORT_T *pMsgData, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;
    uint32 port = 0;

    //端口号
#ifndef PRODUCT_SCU
    UART_GPORT_TO_OAD(port, pMsgData->portType, pMsgData->portNo&0x000000ff);
    MEMCPY_R(&payload[offset], &port, sizeof(uint32));
    offset += sizeof(uint32);
#else    
    if(pMsgData->portType == UART_TYPE_485) 
    {
        port = pMsgData->portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
    else
    {
        port = 4 + pMsgData->portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
#endif

    return offset;
}

/**********************************************************************
* @name      : uart_axdr_get_info_res_split
* @brief     : 串口信息查询响应消息解析
* @param[in] : MESSAGE_JSON_T *pMsg          接收消息JSON格式
* @param[out]: UART_MODESTAUS_T *pModeStatus 解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int uart_axdr_get_info_res_split(MESSAGE_INFO_T *pMsg, UART_MODESTAUS_T *pMsgData)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;
    uint8 byteNum = 0;  //长度字节数
    uint32 lengh  = 0;  //长度值
    uint32 port = 0;
    uint8 ctrl = 0;
    uint8 i = 0;

    if(NULL == pMsg || NULL == pMsgData)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_GET_INFO)
    {
        return -2;
    }

    memset(pMsgData, 0, sizeof(UART_MODESTAUS_T));
    msgbuf = pMsg->playload;

    MEMCPY_R(&port, &msgbuf[offset],  sizeof(uint32));
    offset += sizeof(uint32);
    
#ifndef PRODUCT_SCU
    UART_OAD_TO_GPORT(port, pMsgData->port.portType, pMsgData->port.portNo);
#else
    if(0 < port && port <= 4)
    {
        pMsgData->port.portType = UART_TYPE_485;
        pMsgData->port.portNo = port;
    }
    else if(5 <= port && port <= 6)
    {
        pMsgData->port.portType = UART_TYPE_232;
        pMsgData->port.portNo = port - 4;
    }
    else
    {
        return -3;
    }
#endif

    pMsgData->mode = msgbuf[offset++];
    
    //OPTIONAL 串口状态
    if(msgbuf[offset++] == 1)
    {
        //OPTIONAL 独占APP名
        if(msgbuf[offset++] == 1)
        {
            //visible-string
            GET_LEN_OFFSET(lengh, &msgbuf[offset], byteNum);
            offset += byteNum;
            if(offset + lengh >= pMsg->msglen)
            {
                return -4;
            }
            memcpy(&pMsgData->onlyApp[0], &msgbuf[offset], lengh); 
            offset += lengh;    
        }

        //端口参数
        pMsgData->param.baudRate = msgbuf[offset++];

        ctrl = msgbuf[offset++];

        UART_CTRL_TO_DATABITS(ctrl, pMsgData->param.dataBits);
        UART_CTRL_TO_PARITY(ctrl, pMsgData->param.parity);
        UART_CTRL_TO_STOPBITS(ctrl, pMsgData->param.stopBits);
        UART_CTRL_TO_FLOWCTRL(ctrl, pMsgData->param.flowCtrl);

        //OPTIONAL 监听APP列表
        if(msgbuf[offset++] == 1)
        {
            pMsgData->num = msgbuf[offset++];

            for(i = 0; i < pMsgData->num; i++)
            {
                //visible-string
                GET_LEN_OFFSET(lengh, &msgbuf[offset], byteNum);
                offset += byteNum;
                
                memcpy(pMsgData->monitorApp[i], &msgbuf[offset], lengh); 
                offset += lengh;                    
            }
        }
    }

    return 0;
}

/**********************************************************************
* @name      : uart_json_get_info_res_package
* @brief     : 串口信息查询响应消息封装
* @param[in] : uint32 token                   消息序号
               char *destapp                  目的APP
               UART_MODESTAUS_T *pModeStatus  模式与状态
               int topiclen                   主题缓存容量
* @param[out]: uint8 *payload            负荷内容
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
uint16 uart_axdr_get_info_res_package(UART_MODESTAUS_T *pMsgData, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;
    uint32 port = 0;
    uint8 byteNum = 0;  //长度字节数
    uint32 lengh  = 0;  //长度值
    uint8 ctrl = 0;
    uint8 i = 0;

    //端口号
#ifndef PRODUCT_SCU
    UART_GPORT_TO_OAD(port, pMsgData->port.portType, pMsgData->port.portNo&0x000000ff);
    MEMCPY_R(&payload[offset], &port, sizeof(uint32));
    offset += sizeof(uint32);
#else    
    if(pMsgData->port.portType == UART_TYPE_485) 
    {
        port = pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
    else
    {
        port = 4 + pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
#endif

    //串口模式
    payload[offset++] = pMsgData->mode;

    if((pMsgData->mode == UART_MODE_ONLY) || (pMsgData->mode == UART_MODE_SLAVE))
    {
        //OPTIONAL 串口状态
        payload[offset++] = 1;

        if(pMsgData->mode == UART_MODE_ONLY)
        {
            //OPTIONAL 独占APP名
            payload[offset++] = 1;
            lengh = strlen(pMsgData->onlyApp)+1;
            
            SET_LEN_OFFSET(lengh, &payload[offset], byteNum);
            offset += byteNum;
        }
        else
        {
            payload[offset++] = 0;
        }
        
        //串口参数
        payload[offset++] = pMsgData->param.baudRate;

        UART_DATABITS_TO_CTRL(ctrl, pMsgData->param.dataBits);
        UART_PARITY_TO_CTRL(ctrl, pMsgData->param.parity);
        UART_STOPBITS_TO_CTRL(ctrl, pMsgData->param.stopBits);
        UART_FLOWCTRL_TO_CTRL(ctrl, pMsgData->param.flowCtrl);

        payload[offset++] = ctrl;

        if(pMsgData->mode == UART_MODE_SLAVE)
        {
            //OPTIONAL 监听APP列表
            payload[offset++] = 1;

            payload[offset++] = pMsgData->num;

            for(i = 0; i < pMsgData->num; i++)
            {
                lengh = strlen(pMsgData->monitorApp[i])+1;
                SET_LEN_OFFSET(lengh, &payload[offset], byteNum);
                offset += byteNum; 
                
                memcpy(&payload[offset], pMsgData->monitorApp[i], lengh);
                offset += lengh;
            }
        }
        else
        {
            payload[offset++] = 0;
        }        
    }
    else
    {
        payload[offset++] = 0;
    }
    
    return offset;
}

/*
*********************************************************************
* @brief     : 串口参数设置请求消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: UART_APPREG_T *pAppReg     解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_set_param_req_split(MESSAGE_INFO_T *pMsg, UART_APPREG_T *pMsgData)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;
    uint32 port = 0;
    uint8 ctrl = 0;

    if(NULL == pMsg || NULL == pMsgData)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_SET_PARAM)
    {
        return -2;
    }

    memset(pMsgData, 0, sizeof(UART_APPREG_T));
    msgbuf = pMsg->playload;

    MEMCPY_R(&port, &msgbuf[offset],  sizeof(uint32));
    offset += sizeof(uint32);
    
#ifndef PRODUCT_SCU
    UART_OAD_TO_GPORT(port, pMsgData->port.portType, pMsgData->port.portNo);
#else
    if(0 < port && port <= 4)
    {
        pMsgData->port.portType = UART_TYPE_485;
        pMsgData->port.portNo = port;
    }
    else if(5 <= port && port <= 6)
    {
        pMsgData->port.portType = UART_TYPE_232;
        pMsgData->port.portNo = port - 4;
    }
    else
    {
        return -3;
    }
#endif

    //端口参数
    pMsgData->param.baudRate = msgbuf[offset++];

    ctrl = msgbuf[offset++];

    UART_CTRL_TO_DATABITS(ctrl, pMsgData->param.dataBits);
    UART_CTRL_TO_PARITY(ctrl, pMsgData->param.parity);
    UART_CTRL_TO_STOPBITS(ctrl, pMsgData->param.stopBits);
    UART_CTRL_TO_FLOWCTRL(ctrl, pMsgData->param.flowCtrl);

    return 0;
}

/*
*********************************************************************
* @brief     : 串口参数设置请求消息封装
* @param[in] : UART_APPREG_T *pAppReg         注册APP
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_set_param_req_package(UART_APPREG_T *pMsgData, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;
    uint32 port = 0;
    uint8 ctrl = 0;

    //端口号
#ifndef PRODUCT_SCU
    UART_GPORT_TO_OAD(port, pMsgData->port.portType, pMsgData->port.portNo&0x000000ff);
    MEMCPY_R(&payload[offset], &port, sizeof(uint32));
    offset += sizeof(uint32);
#else    
    if(pMsgData->port.portType == UART_TYPE_485) 
    {
        port = pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
    else
    {
        port = 4 + pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
#endif

    //串口参数
    payload[offset++] = pMsgData->param.baudRate;

    UART_DATABITS_TO_CTRL(ctrl, pMsgData->param.dataBits);
    UART_PARITY_TO_CTRL(ctrl, pMsgData->param.parity);
    UART_STOPBITS_TO_CTRL(ctrl, pMsgData->param.stopBits);
    UART_FLOWCTRL_TO_CTRL(ctrl, pMsgData->param.flowCtrl);

    payload[offset++] = ctrl;
    
    return offset;
}

/*
*********************************************************************
* @brief     : 串口参数设置响应消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_set_param_res_split(MESSAGE_INFO_T *pMsg, uint8 *bOK)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;

    if(NULL == pMsg)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_SET_PARAM)
    {
        return -2;
    }
    
    msgbuf = pMsg->playload;
    
    *bOK = msgbuf[offset++];

    return 0;
}

/*
*********************************************************************
* @brief     : 串口参数设置响应消息封装
* @param[in] : UART_PORT_T *pRSPort           端口
               BOOLEN status                  结果
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_set_param_res_package(UART_PORT_T *pMsgData, uint8 status, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;

    payload[offset++] = status;
    
    return offset;
}

/*
*********************************************************************
* @brief     : 数据接收消息解析
* @param[in] : MESSAGE_INFO_T *pMsg           接收消息AXDR格式
* @param[out]: UART_RECV_DATA_T *pRecvMsg     解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_data_recv_split(MESSAGE_INFO_T *pMsg, UART_RECV_DATA_T *pMsgData)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;
    uint8 byteNum = 0;  //长度字节数
    uint32 lengh  = 0;  //长度值
    uint32 port = 0;

    if(NULL == pMsg || NULL == pMsgData)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_NOTIFY_RECV)
    {
        return -2;
    }

    memset(pMsgData, 0, sizeof(UART_RECV_DATA_T));
    msgbuf = pMsg->playload;

    MEMCPY_R(&port, msgbuf + offset,  sizeof(uint32));
    offset += sizeof(uint32);

    //端口号
#ifndef PRODUCT_SCU
    UART_OAD_TO_GPORT(port, pMsgData->port.portType, pMsgData->port.portNo);
#else
    if(0 < port && port <= 4)
    {
        pMsgData->port.portType = UART_TYPE_485;
        pMsgData->port.portNo = port;
    }
    else if(5 <= port && port <= 6)
    {
        pMsgData->port.portType = UART_TYPE_232;
        pMsgData->port.portNo = port - 4;
    }
    else
    {
        return -3;
    }
#endif

    //启动标志
    pMsgData->prm = msgbuf[offset++];
    
    //状态
    pMsgData->status = msgbuf[offset++];

    //数据缓冲 octet-string
    GET_LEN_OFFSET(lengh, &msgbuf[offset], byteNum);
    offset += byteNum;

    pMsgData->datalen = lengh;
    
    memcpy(pMsgData->data, msgbuf + offset, lengh); 
    offset += lengh; 

    return 0;
}

/*
*********************************************************************
* @brief     : 数据接收消息封装
* @param[in] : UART_RECV_DATA_T *pRecvMsg     接收消息
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_data_recv_package(UART_RECV_DATA_T *pMsgData, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;
    uint32 port = 0;
    uint8 byteNum = 0;  //长度字节数
    uint32 lengh  = 0;  //长度值

    //端口号
#ifndef PRODUCT_SCU
    UART_GPORT_TO_OAD(port, pMsgData->port.portType, pMsgData->port.portNo&0x000000ff);
    MEMCPY_R(&payload[offset], &port, sizeof(uint32));
    offset += sizeof(uint32);
#else    
    if(pMsgData->port.portType == UART_TYPE_485) 
    {
        port = pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
    else
    {
        port = 4 + pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
#endif

    //启动标志
    payload[offset++] = pMsgData->prm;

    //状态
    payload[offset++] = pMsgData->status;
    
    //数据缓冲 octet-string
    lengh = pMsgData->datalen;
    
    SET_LEN_OFFSET(lengh, &payload[offset], byteNum);
    offset += byteNum;
    memcpy(&payload[offset], pMsgData->data, lengh);
    offset += lengh;
       
    return offset;
}

/**********************************************************************
* @name      : uart_axdr_set_monitor_req_split
* @brief     : 数据监听注册请求消息解析
* @param[in] : MESSAGE_INFO_T *pMsg      接收消息
* @param[out]: UART_PORT_T *pRSPort      解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int uart_axdr_set_monitor_req_split(MESSAGE_INFO_T *pMsg, UART_APPREG_T *pMsgData)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;
    uint32 port = 0;

    if(NULL == pMsg || NULL == pMsgData)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_SET_LISTEN)
    {
        return -2;
    }

    memset(pMsgData, 0, sizeof(UART_APPREG_T));
    msgbuf = pMsg->playload;

    MEMCPY_R(&port, &msgbuf[offset],  sizeof(uint32));
    offset += sizeof(uint32);
    
#ifndef PRODUCT_SCU
    UART_OAD_TO_GPORT(port, pMsgData->port.portType, pMsgData->port.portNo);
#else
    if(0 < port && port <= 4)
    {
        pMsgData->port.portType = UART_TYPE_485;
        pMsgData->port.portNo = port;
    }
    else if(5 <= port && port <= 6)
    {
        pMsgData->port.portType = UART_TYPE_232;
        pMsgData->port.portNo = port - 4;
    }
    else
    {
        return -3;
    }
#endif

    return 0;
}

/**********************************************************************
* @name      : uart_axdr_set_monitor_req_package
* @brief     : 数据监听注册请求消息封装
* @param[in] : uint32 token              消息序号
               char *sourapp             请求APP名称
               UART_PORT_T *pRSPort      端口
               int topiclen              主题缓存容量
* @param[out]: uint8 *payload            负荷内容
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
uint16 uart_axdr_set_monitor_req_package(UART_APPREG_T *pMsgData, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;
    uint32 port = 0;

    //端口号
#ifndef PRODUCT_SCU
    UART_GPORT_TO_OAD(port, pMsgData->port.portType, pMsgData->port.portNo&0x000000ff);
    MEMCPY_R(&payload[offset], &port, sizeof(uint32));
    offset += sizeof(uint32);
#else    
    if(pMsgData->port.portType == UART_TYPE_485) 
    {
        port = pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
    else
    {
        port = 4 + pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
#endif

    return offset;
}

/*
*********************************************************************
* @brief     : 数据监听注册响应消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_set_monitor_res_split(MESSAGE_INFO_T *pMsg, uint8 *bOK)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;

    if(NULL == pMsg || NULL == bOK)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_SET_LISTEN)
    {
        return -2;
    }
    
    msgbuf = pMsg->playload;
    
    *bOK = msgbuf[offset++];

    return 0;
}

/*
*********************************************************************
* @brief     : 数据监听注册响应消息封装
* @param[in] : UART_PORT_T *pRSPort           端口
               BOOLEN status                  结果
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_set_monitor_res_package(UART_PORT_T *pMsgData, uint8 status, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;

    payload[offset++] = status;
    
    return offset;
}

/*
*********************************************************************
* @brief     : 数据发送消息解析
* @param[in] : MESSAGE_INFO_T *pMsg           接收消息AXDR格式
* @param[out]: UART_SEND_DATA_T *pSendMsg     解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_data_send_req_split(MESSAGE_INFO_T *pMsg, UART_SEND_DATA_T *pMsgData)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;
    uint32 port = 0;
    uint8 ctrl = 0;
    uint8 byteNum = 0;  //长度字节数
    uint32 lengh  = 0;  //长度值

    if(NULL == pMsg || NULL == pMsgData)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_SEND_DATA)
    {
        return -2;
    }

    memset(pMsgData, 0, sizeof(UART_SEND_DATA_T));
    msgbuf = pMsg->playload;

    MEMCPY_R(&port, &msgbuf[offset],  sizeof(uint32));
    offset += sizeof(uint32);

    //端口号
#ifndef PRODUCT_SCU
    UART_OAD_TO_GPORT(port, pMsgData->port.portType, pMsgData->port.portNo);
#else
    if(0 < port && port <= 4)
    {
        pMsgData->port.portType = UART_TYPE_485;
        pMsgData->port.portNo = port;
    }
    else if(5 <= port && port <= 6)
    {
        pMsgData->port.portType = UART_TYPE_232;
        pMsgData->port.portNo = port - 4;
    }
    else
    {
        return -3;
    }
#endif

    //启动标志
    pMsgData->prm = msgbuf[offset++];

    //优先级
    pMsgData->prio = msgbuf[offset++];
    
    MEMCPY_R(&pMsgData->frametimeout,  &msgbuf[offset], sizeof(uint16));
    offset += sizeof(uint16);

    MEMCPY_R(&pMsgData->bytetimeout,  &msgbuf[offset], sizeof(uint16));
    offset += sizeof(uint16);    

    MEMCPY_R(&pMsgData->tasktimeout,  &msgbuf[offset], sizeof(uint16));
    offset += sizeof(uint16);    

    //OPTIONAL 端口参数
    pMsgData->bParam = msgbuf[offset++];
    
    if(pMsgData->bParam == 1)
    {
        pMsgData->param.baudRate = msgbuf[offset++];

        ctrl = msgbuf[offset++];

        UART_CTRL_TO_DATABITS(ctrl, pMsgData->param.dataBits);
        UART_CTRL_TO_PARITY(ctrl, pMsgData->param.parity);
        UART_CTRL_TO_STOPBITS(ctrl, pMsgData->param.stopBits);
        UART_CTRL_TO_FLOWCTRL(ctrl, pMsgData->param.flowCtrl);
    }

    //预计最大接收字节数
    MEMCPY_R(&pMsgData->estsize,  &msgbuf[offset], sizeof(uint16));
    offset += sizeof(uint16);    

    //数据缓冲 octet-string
    GET_LEN_OFFSET(lengh, &msgbuf[offset], byteNum);
    offset += byteNum;

    pMsgData->datalen = lengh;
    
    memcpy(&pMsgData->data[0], &msgbuf[offset], lengh); 
    offset += lengh; 

    return 0;
}

/*
*********************************************************************
* @brief     : 数据发送消息封装
* @param[in] : UART_SEND_DATA_T *pSendMsg     发送内容
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_data_send_req_package(UART_SEND_DATA_T *pMsgData, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;
    uint32 port = 0;
    uint8 ctrl = 0;
    uint8 byteNum = 0;  //长度字节数
    uint32 lengh  = 0;  //长度值

    //端口号
#ifndef PRODUCT_SCU
    UART_GPORT_TO_OAD(port, pMsgData->port.portType, pMsgData->port.portNo&0x000000ff);
    MEMCPY_R(&payload[offset], &port, sizeof(uint32));
    offset += sizeof(uint32);
#else    
    if(pMsgData->port.portType == UART_TYPE_485) 
    {
        port = pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
    else
    {
        port = 4 + pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
#endif

    payload[offset++] = pMsgData->prm;
    payload[offset++] = pMsgData->prio;

    MEMCPY_R(&payload[offset], &pMsgData->frametimeout,  sizeof(uint16));
    offset += sizeof(uint16);

    MEMCPY_R(&payload[offset], &pMsgData->bytetimeout,  sizeof(uint16));
    offset += sizeof(uint16);    

    MEMCPY_R(&payload[offset], &pMsgData->tasktimeout,  sizeof(uint16));
    offset += sizeof(uint16);    

    //串口参数
    if(pMsgData->bParam == 1)
    {
        payload[offset++] = 1;

        payload[offset++] = pMsgData->param.baudRate;

        UART_DATABITS_TO_CTRL(ctrl, pMsgData->param.dataBits);
        UART_PARITY_TO_CTRL(ctrl, pMsgData->param.parity);
        UART_STOPBITS_TO_CTRL(ctrl, pMsgData->param.stopBits);
        UART_FLOWCTRL_TO_CTRL(ctrl, pMsgData->param.flowCtrl);

        payload[offset++] = ctrl;        
    }
    else
    {
        payload[offset++] = 0;
    }

    MEMCPY_R(&payload[offset], &pMsgData->estsize,  sizeof(uint16));
    offset += sizeof(uint16);    

    //数据缓冲 octet-string
    lengh = pMsgData->datalen;
    
    SET_LEN_OFFSET(lengh, &payload[offset], byteNum);
    offset += byteNum;
    memcpy(&payload[offset], pMsgData->data, lengh);
    offset += lengh;
    
    return offset;
}

/*
*********************************************************************
* @brief     : 数据发送响应消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: UART_PORT_T *pRSPort       解析结果
               uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_data_send_res_split(MESSAGE_INFO_T *pMsg, UART_PORT_T *pMsgData, uint8 *bOK)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;

    if(NULL == pMsg || NULL == pMsgData)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_SET_PARAM)
    {
        return -2;
    }
    
    msgbuf = pMsg->playload;
    
    *bOK = msgbuf[offset++];

    return 0;
}

/*
*********************************************************************
* @brief     : 数据发送响应消息封装
* @param[in] : UART_PORT_T *pRSPort           端口
               BOOLEN status                  结果
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_data_send_res_package(UART_PORT_T *pMsgData, uint8 status, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;

    payload[offset++] = status;
    
    return offset;
}

/*
*********************************************************************
* @brief     : 串口异常报告请求消息解析
* @param[in] : MESSAGE_INFO_T *pMsg           接收消息AXDR格式
* @param[out]: UART_ABNORMAL_T *pAbnormal     解析结果
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_set_abnormal_req_split(MESSAGE_INFO_T *pMsg, UART_ABNORMAL_T *pMsgData)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;
    uint32 port = 0;

    if(NULL == pMsg || NULL == pMsgData)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_SET_ABNORMAL)
    {
        return -2;
    }

    memset(pMsgData, 0, sizeof(UART_ABNORMAL_T));
    msgbuf = pMsg->playload;

    MEMCPY_R(&port, &msgbuf[offset],  sizeof(uint32));
    offset += sizeof(uint32);

    UART_OAD_TO_GPORT(port, pMsgData->port.portType, pMsgData->port.portNo);

    pMsgData->cause = msgbuf[offset++];

    return 0;
}

/*
*********************************************************************
* @brief     : 串口异常报告请求消息封装
               UART_ABNORMAL_T *pAbnormal     异常信息
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_set_abnormal_req_package(UART_ABNORMAL_T *pMsgData, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;
    uint32 port = 0;
    
    UART_GPORT_TO_OAD(port, pMsgData->port.portType, pMsgData->port.portNo&0x000000ff);
    MEMCPY_R(&payload[offset], &port, sizeof(uint32));
    offset += sizeof(uint32);

    payload[offset++] = pMsgData->cause;

    return offset;
}

/*
*********************************************************************
* @brief     : 串口异常报告响应消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: UART_PORT_T *pRSPort       解析结果
               uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_set_abnormal_res_split(MESSAGE_INFO_T *pMsg, UART_PORT_T *pMsgData, uint8 *bOK)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;

    if(NULL == pMsg || NULL == pMsgData)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_SET_PARAM)
    {
        return -2;
    }
    
    msgbuf = pMsg->playload;
    
    *bOK = msgbuf[offset++];

    return 0;
}

/*
*********************************************************************
* @brief     : 串口异常报告响应消息封装
* @param[in] : UART_PORT_T *pRSPort           端口
               BOOLEN status                  结果
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_set_abnormal_res_package(UART_PORT_T *pMsgData, BOOLEAN status, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;

    payload[offset++] = status;
    
    return offset;
}

/*
*********************************************************************
* @brief     : 串口模式设置请求消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_set_mode_req_split(MESSAGE_INFO_T *pMsg, UART_MODESET_T *pMsgData)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;
    uint8 byteNum = 0;  //长度字节数
    uint32 lengh  = 0;  //长度值
    uint32 port = 0;
    uint8 ctrl = 0;

    if(NULL == pMsg || NULL == pMsgData)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_ONLYMODE_SET)
    {
        return -2;
    }

    memset(pMsgData, 0, sizeof(UART_MODESTAUS_T));
    msgbuf = pMsg->playload;

    MEMCPY_R(&port, &msgbuf[offset],  sizeof(uint32));
    offset += sizeof(uint32);
    
#ifndef PRODUCT_SCU
    UART_OAD_TO_GPORT(port, pMsgData->port.portType, pMsgData->port.portNo);
#else
    if(0 < port && port <= 4)
    {
        pMsgData->port.portType = UART_TYPE_485;
        pMsgData->port.portNo = port;
    }
    else if(5 <= port && port <= 6)
    {
        pMsgData->port.portType = UART_TYPE_232;
        pMsgData->port.portNo = port - 4;
    }
    else
    {
        return -3;
    }
#endif

    pMsgData->mode = msgbuf[offset++];
    
    //OPTIONAL 独占APP名
    if(msgbuf[offset++] == 1)
    {
        //visible-string
        GET_LEN_OFFSET(lengh, &msgbuf[offset], byteNum);
        offset += byteNum;
        if(offset + lengh >= pMsg->msglen)
        {
            return -4;
        }
        memcpy(&pMsgData->onlyApp[0], &msgbuf[offset], lengh); 
        offset += lengh;    
    }

    //OPTIONAL 端口参数
    if(msgbuf[offset++] == 1)
    {
        pMsgData->bParam = TRUE;
        pMsgData->param.baudRate = msgbuf[offset++];

        ctrl = msgbuf[offset++];

        UART_CTRL_TO_DATABITS(ctrl, pMsgData->param.dataBits);
        UART_CTRL_TO_PARITY(ctrl, pMsgData->param.parity);
        UART_CTRL_TO_STOPBITS(ctrl, pMsgData->param.stopBits);
        UART_CTRL_TO_FLOWCTRL(ctrl, pMsgData->param.flowCtrl);
    }

    return 0;
}


/*
*********************************************************************
* @brief     : 串口模式设置请求消息封装
* @param[in] : UART_PORT_T *pRSPort           端口
               BOOLEN status                  结果
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_set_mode_req_package(UART_MODESET_T *pMsgData, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;
    uint32 port = 0;
    uint8 byteNum = 0;  //长度字节数
    uint32 lengh  = 0;  //长度值
    uint8 ctrl = 0;

    //端口号
#ifndef PRODUCT_SCU
    UART_GPORT_TO_OAD(port, pMsgData->port.portType, pMsgData->port.portNo&0x000000ff);
    MEMCPY_R(&payload[offset], &port, sizeof(uint32));
    offset += sizeof(uint32);
#else    
    if(pMsgData->port.portType == UART_TYPE_485) 
    {
        port = pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
    else
    {
        port = 4 + pMsgData->port.portNo;
        MEMCPY_R(&payload[offset], &port, sizeof(uint32));
        offset += sizeof(uint32);        
    }
#endif

    //串口模式
    payload[offset++] = pMsgData->mode;

    if(pMsgData->mode == UART_MODE_ONLY)
    {
        //OPTIONAL 独占APP名
        payload[offset++] = 1;
        lengh = strlen(pMsgData->onlyApp)+1;
        
        SET_LEN_OFFSET(lengh, &payload[offset], byteNum);
        offset += byteNum;
    }
    else
    {
        payload[offset++] = 0;
    }

    //是否携带参数
    if(pMsgData->bParam == TRUE)
    {
        //串口参数
        payload[offset++] = 1;
        
        payload[offset++] = pMsgData->param.baudRate;

        UART_DATABITS_TO_CTRL(ctrl, pMsgData->param.dataBits);
        UART_PARITY_TO_CTRL(ctrl, pMsgData->param.parity);
        UART_STOPBITS_TO_CTRL(ctrl, pMsgData->param.stopBits);
        UART_FLOWCTRL_TO_CTRL(ctrl, pMsgData->param.flowCtrl);

        payload[offset++] = ctrl;
    }
    else
    {
        payload[offset++] = 0;
    }
    
    return offset;
}

/*
*********************************************************************
* @brief     : 串口模式设置回码消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_set_mode_res_split(MESSAGE_INFO_T *pMsg, uint8 *bOK)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;

    if(NULL == pMsg || NULL == bOK)
    {
        return -1;
    }

    if(pMsg->IOP != UART_IOP_SET_LISTEN)
    {
        return -2;
    }
    
    msgbuf = pMsg->playload;
    
    *bOK = msgbuf[offset++];

    return 0;
}

/*
*********************************************************************
* @brief     : 串口模式设置回码消息封装
* @param[in] : UART_PORT_T *pRSPort           端口
               BOOLEN status                  结果
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_set_mode_res_package(UART_PORT_T *pMsgData, uint8 status, uint8 *payload, uint16 maxlen)
{
    uint16 offset = 0;

    payload[offset++] = status;
    
    return offset;
}

#endif
