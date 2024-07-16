/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved.
* @brief：   遥信脉冲主功能
* @date：    2021-7-24
* @history：
*********************************************************************
*/
#include "hal.h"
#include "baseCommon.h"
#include "appmsg_split.h"
#include "iniparser.h"
#include "framecomm.h"
#include "storage.h"

#include "rsp_data.h"
#include "rsp_mqtt.h"

#define RSP_INI_FILE   "rspSample.ini"
#define RSP_USER_FILE  "userparam.dat"

RSP_DATA_T g_tRspData;
char       g_RspDataFile[256] = {0};

/**********************************************************************
* @name      : rsp_write_queue
* @brief     ：写入队列
* @param[in] ：RSP_REPORT_LIST_T *pList     上报队列
               RSP_REPORT_DATA_T *pNode     写入内容
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-3-21
* @Update    :
**********************************************************************/
void rsp_write_queue(RSP_REPORT_LIST_T *pList, RSP_REPORT_DATA_T *pNode)
{
    memcpy(&pList->list[pList->tail], pNode, sizeof(RSP_REPORT_DATA_T));
    pList->tail = (pList->tail + 1) % RSP_REPORT_CACHE;
}

/**********************************************************************
* @name      : rsp_read_queue
* @brief     ：读出队列
* @param[in] ：RSP_REPORT_LIST_T *pList    上报队列
* @param[out]：RSP_REPORT_DATA_T *pNode     上报内容
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2022-3-21
* @Update    :
**********************************************************************/
int rsp_read_queue(RSP_REPORT_LIST_T *pList, RSP_REPORT_DATA_T *pNode)
{
    if(pList->head == pList->tail)
    {
        return -1;
    }

    memcpy(pNode, &pList->list[pList->head], sizeof(RSP_REPORT_DATA_T));
    memset( &pList->list[pList->head], 0, sizeof(RSP_REPORT_DATA_T));
    pList->head = (pList->head + 1) % RSP_REPORT_CACHE;

    return 0;
}

/**********************************************************************
* @name      : rsp_write_msg_queue
* @brief     ：写入队列
* @param[in] ：RSP_MSG_LIST_T *pList     上报队列
               uint8 *pMsg               消息
               uint16 msglen             长度
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2022-6-16
* @Update    :
**********************************************************************/
int rsp_write_msg_queue(RSP_MSG_LIST_T *pList, uint8 *pMsg, uint16 msglen)
{
    if(msglen > RSP_MSG_MAXLEN)
    {
        return -1;
    }
    pList->list[pList->tail].len = msglen;
    memcpy(pList->list[pList->tail].msg, pMsg, msglen);
    pList->tail = (pList->tail + 1) % RSP_MSG_CACHE;

    return 0;
}

/**********************************************************************
* @name      : rsp_read_msg_queue
* @brief     ：读出队列
* @param[in] ：RSP_MSG_LIST_T *pList    上报队列
               uint16 buflen            缓存大小
* @param[out]：uint8 *pMsg               消息内容
* @return    ：0-消息长度
* @Create    : 王津剑
* @Date      ：2022-6-16
* @Update    :
**********************************************************************/
int rsp_read_msg_queue(RSP_MSG_LIST_T *pList, uint8 *pMsg, uint16 buflen)
{
    int len = 0;
    if(pList->head == pList->tail)
    {
        return 0;
    }

    len = pList->list[pList->head].len;
    memcpy(pMsg, pList->list[pList->head].msg, MIN(buflen, len));

    pList->list[pList->head].len = 0;
    memset(pList->list[pList->head].msg, 0, RSP_MSG_MAXLEN);
    pList->head = (pList->head + 1) % RSP_MSG_CACHE;

    return 0;
}


/**********************************************************************
* @name      : rsp_check_userfile
* @brief     ：检查用户配置文件, 如果没有就创建
               文件大小 4 + 1024 * 2; 前4个字节中存放校验
* @param[in] ：char *filename 文件名
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
void rsp_check_userfile(char *filename)
{
    char check[4] = {0xea, 0xeb, 0xec, 0x02}; /* 最后一位是消息版本号, 每修改一次增加1 */
    char buf[4] = {0};

    if(0 == access(filename, F_OK))
    {
        if(0 == read_pdata_xram(filename, buf, 0, 4))
        {
            RSP_FMT_TRACE("File version %02x %02x %02x %02x, check %02x %02x %02x %02x\n",
                   buf[0], buf[1], buf[2], buf[3], check[0], check[1], check[2], check[3]);
            if(0 == memcmp(buf, check, 4))
            {
                return;
            }
        }
        RSP_FMT_DEBUG("Create new userfile\n");
    }

    char writebuf[1024 * 2] = {0};

    write_pdata_xram(filename, check, 0, 4);
    write_pdata_xram(filename, writebuf, 4, 1024 * 2);
}

/**********************************************************************
* @name      : rsp_get_config
* @brief     ：获取遥信脉冲配置 存放位置 4 + id * 1024
* @param[in] ：uint8 id       设备编号(1-3)
               uint8 num      配置数量
* @param[out]：uint8 *config  遥信脉冲配置
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int rsp_get_config(uint8 id, uint8 *config, uint8 num)
{
    if(id > 2 || NULL == config)
    {
        return -1;
    }

    return read_pdata_xram(g_RspDataFile, (char *)config, 4 + id * 1024, num);
}

/**********************************************************************
* @name      : rsp_write_config
* @brief     ：保存遥信脉冲配置 存放位置 4 + id * 1024
* @param[in] ：uint8 id       设备编号(0-2)
               uint8 *config  遥信脉冲配置
               uint8 num      配置数量
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int rsp_write_config(uint8 id, uint8 *config, uint8 num)
{
    if(id > 2 || NULL == config)
    {
        return -1;
    }

    return write_pdata_xram(g_RspDataFile, (char *)config, 4 + id * 1024, num);
}

/**********************************************************************
* @name      : rsp_get_shaketime
* @brief     ：获取遥信防抖时间 存放位置 4 + id * 1024 + 16
* @param[in] ：uint8 id           设备编号(0-2)
* @param[out]：uint16 *shaketime  防抖时间
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int rsp_get_shaketime(uint8 id, uint16 *shaketime)
{
    if(id > 2 || NULL == shaketime)
    {
        return -1;
    }

    return read_pdata_xram(g_RspDataFile, (char *)shaketime, 4 + id * 1024 + 16, sizeof(uint16));
}

/**********************************************************************
* @name      : rsp_write_shaketime
* @brief     ：保存遥信防抖时间 存放位置 4 + id * 1024 + 16
* @param[in] ：uint8 id           设备编号(0-2)
               uint16 shaketime   防抖时间
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int rsp_write_shaketime(uint8 id, uint16 shaketime)
{
    if(id > 2)
    {
        return -1;
    }

    return write_pdata_xram(g_RspDataFile, (char *)&shaketime, 4 + id * 1024 + 16, sizeof(uint16));
}

/**********************************************************************
* @name      : rsp_get_curtime
* @brief     ：当前时间转换成标准格式
* @param[in] ：
* @param[out]：OOP_DATETIME_T *pcurTime 标准时间
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
void rsp_get_curtime(OOP_DATETIME_T *pCurTime)
{
    struct  tm systime;
    struct  timeval tv;
    struct  timezone tz;
    gettimeofday(&tv, &tz);

    FastSecondToDate(&tv, &tz, &systime);

    pCurTime->msec = tv.tv_usec/1000;
    pCurTime->second = (uint8)systime.tm_sec;
    pCurTime->minute = (uint8)systime.tm_min;
    pCurTime->hour = (uint8)systime.tm_hour;
    pCurTime->wday = (uint8)systime.tm_wday;
    pCurTime->mday = (uint8)systime.tm_mday;
    pCurTime->month = (uint8)systime.tm_mon + 1;         /*月份0代表1月*/
    pCurTime->year = (uint8)systime.tm_year + 1900;         /*年份0代表1900年*/
}

/**********************************************************************
* @name      : rsp_init_config
* @brief     ：初始化配置文件
* @param[in] ：
* @param[out]：uint32 *ip    服务端地址
               uint16 *port  服务端端口
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_init_config(RSP_DATA_T *pRspData)
{
    dictionary *ini = NULL;
    uint32 ipaddr = 0;
    char  *stringvalue = NULL;
    int    intvalue = 0;
    char   curpath[256] = {0};
    char   filename[256] = {0};
    int cnt = readlink("/proc/self/exe", curpath, 256);

    if(cnt < 0 || cnt >= 256)
    {
        printf("wireless get err path!\n");
    }
    //获取当前目录绝对路径，即去掉程序名
    int i;
    for (i = cnt; i >=0; --i)
    {
        if (curpath[i] == '/')
        {
            curpath[i+1] = '\0';
            break;
        }
    }

    sprintf(filename, "%s%s", curpath, RSP_INI_FILE);
    /* 日志模块初始化 */
    diag_init(filename, TRUE);

    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* ip获取 */
        stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "172.17.0.1");
        ipaddr = inet_addr(stringvalue);
        pRspData->mqttip = ntohl(ipaddr);

        /* 端口获取 */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        pRspData->mqttport = (uint16)intvalue;

        /* 获取data路径 */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:DATAPATH", "/data/app/rspSample/data");
        if(0 != access(stringvalue, F_OK))
        {
            /* 必须先创建文件夹 */
            if(0 != mkdir(stringvalue, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
            {
                printf("Create data filepath %s, failed\n", stringvalue);
            }
        }
        sprintf(g_RspDataFile, "%s/%s", stringvalue, RSP_USER_FILE);

        /* 配置 */
        stringvalue = iniparser_getstring(ini, "YM_CONF:TYPE", "IIC");
        pRspData->bIIC = 0 == strcmp(stringvalue, "IIC") ? TRUE : FALSE;

        stringvalue = iniparser_getstring(ini, "YM_CONF:PULSE_TYPE", "SYS");
        pRspData->pulseType = 0 == strcmp(stringvalue, "IO") ? RSP_PULSE_IO : RSP_PULSE_SYS;
        pRspData->tRspDev.pulseLocate = iniparser_getint(ini, "YM_CONF:PULSE_LOCATE", 0);

        intvalue = iniparser_getint(ini, "YM_CONF:YX_SUM", 8);
        pRspData->tRspDev.sum = (uint8)intvalue;
        if(pRspData->tRspDev.sum > 16)
        {
            pRspData->tRspDev.sum = 16;
        }

        intvalue = iniparser_getint(ini, "YM_CONF:PULSE_SUM", 8);
        pRspData->tRspDev.pulsesum = (uint8)intvalue;
        if(pRspData->tRspDev.pulsesum > 8)
        {
            pRspData->tRspDev.pulsesum = 8;
        }

        intvalue = iniparser_getint(ini, "YM_CONF:YM_SHARE", 0);
        pRspData->tRspDev.bShare = (uint8)intvalue;
        if(pRspData->tRspDev.bShare > 1)
        {
            pRspData->tRspDev.bShare = 1;
        }

        /* 模组 */
        intvalue = iniparser_getint(ini, "MAP_CONF:SUPPORT", 1);
        pRspData->bMAP = (uint8)intvalue;
        if(pRspData->bMAP > 1)
        {
            pRspData->bMAP = 1;
        }

        intvalue = iniparser_getint(ini, "MAP_CONF:PROT_SUM", 4);
        if(intvalue > 4 || intvalue < 0)
        {
            intvalue = 4;
        }
        pRspData->tRspModel[0].sum = intvalue;
        pRspData->tRspModel[1].sum = intvalue;

        intvalue = iniparser_getint(ini, "YM_CONF:CTRL_CHECK", 0);
        pRspData->bGetCtrl = 1 == intvalue ? 1 : 0;

        intvalue = iniparser_getint(ini, "YM_CONF:SYS_YK_NUM", 0);
        pRspData->tRspDev.yksum = intvalue > 16 ? 0 : intvalue;
        pRspData->tRspDev.ykLocate = iniparser_getint(ini, "YM_CONF:SYS_YK_LOCATE", 0);

        iniparser_freedict(ini);
    }
    else
    {
        printf("rspSample get no %s!\n", RSP_INI_FILE);
        sprintf(g_RspDataFile, "%s/%s", "/data/app/rspSample/data", RSP_USER_FILE);
        pRspData->tRspDev.sum = 2;
    }

    rsp_check_userfile(g_RspDataFile);
    return 0;
}

/**********************************************************************
* @name      : rsp_app_init
* @brief     ：遥信脉冲APP初始化
* @param[in] ：
* @param[out]：void **ppPublicData
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-24
* @Update    :
**********************************************************************/
int rsp_app_init(void **ppPublicData)
{
    int           ret = 0;
    uint8         i = 0;
    char          showtemp[256] = {0};
    uint16        offset = 0;

    /* 检查程序只启动了一个副本 */
    if(!singleton_check("rspSample"))
    {
        printf("%s is started!\n", "rspSample");
        return -1;
    }

    /* 加载hal */
    if(0 != hal_init())
    {
        printf("hal_init failed!\n");
        return -2;
    }

    /* 数据初始化 */
    memset(&g_tRspData, 0, sizeof(RSP_DATA_T));
    pthread_mutex_init(&g_tRspData.tRspDev.flock, NULL);
    pthread_mutex_init(&g_tRspData.tRspModel[0].flock, NULL);
    pthread_mutex_init(&g_tRspData.tRspModel[1].flock, NULL);
    pthread_mutex_init(&g_tRspData.flock, NULL);

    /* 配置文件 */
    ret = rsp_init_config(&g_tRspData);
    if(0 != ret)
    {
        printf("rsp_init_config failed!\n");
        return -3;
    }

    /* mqtt初始化 */
    ret = rsp_mqtt_init(g_tRspData.mqttip, g_tRspData.mqttport);
    if(0 != ret)
    {
        printf("wireless_mqtt_init failed!\n");
        return -4;
    }

    RSP_FMT_DEBUG("遥信脉冲rspSample启动, 版本号: %s, 时间: %s\n", APP_VERSION, APP_PUBLISH_TIME);

    /* 保存参数获取 */
    rsp_get_config(0, g_tRspData.tRspDev.config, RSP_PORT_MAX);
    rsp_get_shaketime(0, &(g_tRspData.tRspDev.shakeTime));
    if(0 == g_tRspData.tRspDev.shakeTime)
    {
        g_tRspData.tRspDev.shakeTime = 100;
    }
#ifdef PRODUCT_ECU
#ifdef BOARD_3358
    g_tRspData.tRspDev.config[0] = 1;
    g_tRspData.tRspDev.config[1] = 1;
#endif
#ifdef BOARD_801
    g_tRspData.tRspDev.config[6] = 1;
    g_tRspData.tRspDev.config[7] = 1;
#endif
    g_tRspData.tRspDev.sum = 0;
#endif
#ifdef PRODUCT_ZCU_3
    for(i = 0; i < RSP_PORT_MAX; i++)
    {
        if((g_tRspData.tRspDev.pulseLocate >> i) & 0x01)
        {
            g_tRspData.tRspDev.config[i] = 1;
        }
    }
#endif

    /* 参数打印输出 */
    RSP_FMT_DEBUG("本体配置(遥信数量: %d, 脉冲数量 %d, %s复用端口):\n  防抖时间: %ums, 功能: %s %s %s %s %s %s %s %s\n",
                  g_tRspData.tRspDev.sum, g_tRspData.tRspDev.pulsesum, 0 == g_tRspData.tRspDev.bShare ? "不" : "",
                  g_tRspData.tRspDev.shakeTime,
                  0 == g_tRspData.tRspDev.config[0] ? "遥信" : "脉冲",
                  0 == g_tRspData.tRspDev.config[1] ? "遥信" : "脉冲",
                  0 == g_tRspData.tRspDev.config[2] ? "遥信" : "脉冲",
                  0 == g_tRspData.tRspDev.config[3] ? "遥信" : "脉冲",
                  0 == g_tRspData.tRspDev.config[4] ? "遥信" : "脉冲",
                  0 == g_tRspData.tRspDev.config[5] ? "遥信" : "脉冲",
                  0 == g_tRspData.tRspDev.config[6] ? "遥信" : "脉冲",
                  0 == g_tRspData.tRspDev.config[7] ? "遥信" : "脉冲");
    if(g_tRspData.bIIC)
    {
        RSP_FMT_DEBUG("本体采集IIC, 脉冲采集方式%s\n", 0 == g_tRspData.pulseType ? "IO" : "SYS");
    }
    else
    {
        RSP_FMT_DEBUG("本体采集UART%s\n", 0 == g_tRspData.bGetCtrl ? "" : ", 采集控制回路接线状态检查");
    }
    /* 模组 */
    g_tRspData.tRspModel[0].no = 1;
    rsp_get_config(1, g_tRspData.tRspModel[0].config, RSP_PORT_MAX);
    rsp_get_shaketime(1, &(g_tRspData.tRspModel[0].shakeTime));
    if(0 == g_tRspData.tRspModel[0].shakeTime)
    {
        g_tRspData.tRspModel[0].shakeTime = 100;
    }
#ifdef PRODUCT_ECU
    g_tRspData.tRspModel[0].config[0] = 0;
    g_tRspData.tRspModel[0].config[1] = 0;
    g_tRspData.tRspModel[0].config[2] = 0;
    g_tRspData.tRspModel[0].config[3] = 0;
#endif

    g_tRspData.tRspModel[1].no = 2;
    rsp_get_config(2, g_tRspData.tRspModel[1].config, RSP_PORT_MAX);
    rsp_get_shaketime(2, &(g_tRspData.tRspModel[1].shakeTime));
    if(0 == g_tRspData.tRspModel[1].shakeTime)
    {
        g_tRspData.tRspModel[1].shakeTime = 100;
    }
#ifdef PRODUCT_ECU
    g_tRspData.tRspModel[1].config[0] = 0;
    g_tRspData.tRspModel[1].config[1] = 0;
    g_tRspData.tRspModel[1].config[2] = 0;
    g_tRspData.tRspModel[1].config[3] = 0;
#endif

    if(1 == g_tRspData.bMAP)
    {
        for(i = 0; i < g_tRspData.tRspModel[0].sum; i++)
        {
            offset += sprintf(showtemp + offset, " %s", 0 == g_tRspData.tRspModel[0].config[i] ? "遥信" : "脉冲");
        }
        RSP_FMT_DEBUG("模组1配置(端口数量: %d):\n  防抖时间: %ums, 功能:%s\n",
                      g_tRspData.tRspModel[0].sum, g_tRspData.tRspModel[0].shakeTime, showtemp);

        memset(showtemp, 0, 256);
        offset = 0;
        for(i = 0; i < g_tRspData.tRspModel[1].sum; i++)
        {
            offset += sprintf(showtemp + offset, " %s", 0 == g_tRspData.tRspModel[1].config[i] ? "遥信" : "脉冲");
        }
        RSP_FMT_DEBUG("模组2配置(端口数量: %d):\n  防抖时间: %ums, 功能:%s\n",
                      g_tRspData.tRspModel[1].sum, g_tRspData.tRspModel[1].shakeTime, showtemp);
    }

    *ppPublicData = &g_tRspData;
    return 0;
}

/**********************************************************************
* @name      : rsp_version_print
* @brief     ：版本打印
* @param[in] ：uint32 buflen     缓存长度
* @param[out]：char *buf         输出日志
* @return    ：实际长度
* @Create    : 王津剑
* @Date      ：2021-10-28
* @Update    :
**********************************************************************/
int rsp_version_print(char *buf, uint32 buflen)
{
    return snprintf(buf, buflen, "Version: %s, Time: %s", APP_VERSION, APP_PUBLISH_TIME);
}

