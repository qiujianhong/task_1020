/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   蓝牙APP 数据
* @date：    2021-7-8
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#ifndef PRODUCT_SCU
#include "hal.h"
#include "devinfo.h"
#endif
#include "iniparser.h"
#include "framecomm.h"
#include "appmsg_split.h"
#include "storage.h"
#include "ble_data.h"
#include "ble_mqtt.h"
#if PIN_ENABLE == 1
#include "OopAll.h"
#endif

#define BLE_INI_FILE     "btManager.ini"
#define BLE_DATA_FILE    "bt.data"
BLE_DATA_T g_tBLEPublic;
char       g_bledatafile[256] = {0};


/**********************************************************************
* @name      : ble_clean_queue
* @brief     ：清队列
* @param[in] ：pQueue  缓冲区结构指针
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
void ble_clean_queue(BLE_QUEUE_T *pQueue)
{
    BLE_DEV_MSG_T *pTemp = NULL;
    pthread_mutex_lock(&pQueue->f_lock);
    pTemp = pQueue->pHead;
    while(pTemp)
    {
        pQueue->pHead = pTemp->pNext;
        free(pTemp);
        pTemp = pQueue->pHead;
    }
    pthread_mutex_unlock(&pQueue->f_lock);
}

/**********************************************************************
* @name      : ble_read_queue
* @brief     ：读队列
* @param[in] ：pQueue      缓冲区结构指针
* @param[out]：
* @return    ：BLE_DEV_MSG_T *消息队列首消息
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
BLE_DEV_MSG_T *ble_read_queue(BLE_QUEUE_T *pQueue)
{
    BLE_DEV_MSG_T *pHead = NULL;
    
    pthread_mutex_lock(&pQueue->f_lock);

    pHead = pQueue->pHead;
    if(pHead)
    {
        pQueue->pHead = pHead->pNext;
        pHead->pNext = NULL;
    }

    pthread_mutex_unlock(&pQueue->f_lock);
    return pHead;
}

/**********************************************************************
* @name      : ble_write_queue
* @brief     ：写队列 按照先后顺序
* @param[in] ：pQueue        缓冲区结构指针
               pMsgProc      需要写入队列的数据(需要提前申请内存)
               uint8 bOrder  是否排序
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
void ble_write_queue(BLE_QUEUE_T *pQueue, BLE_DEV_MSG_T *pMsgProc, uint8 bOrder)
{
    BLE_DEV_MSG_T *pPre = NULL;
    BLE_DEV_MSG_T *pNext = NULL;

    /* 配置不区分先后顺序 */
    pthread_mutex_lock(&pQueue->f_lock);

    pPre = pQueue->pHead;
    if(FALSE == bOrder)
    {
        if(NULL == pPre)
        {
            pQueue->pHead = pMsgProc;
            pMsgProc->pNext = NULL;
        }
        else
        {
            pNext = pPre->pNext;
            while(pNext)
            {
                pPre = pNext;
                pNext = pPre->pNext;
            }
            pPre->pNext = pMsgProc;
            pMsgProc->pNext = NULL;
        }

        goto end;
    }

    /* 消息按照优先级排序 */
    if(NULL == pPre)
    {
        pQueue->pHead = pMsgProc;
        pMsgProc->pNext = NULL;
    }
    else if(pPre->priority > pMsgProc->priority)
    {
        pMsgProc->pNext = pQueue->pHead;
        pQueue->pHead = pMsgProc;
    }
    else
    {
        pNext = pPre->pNext;
        while(pNext)
        {
            if(pNext->priority > pMsgProc->priority)
            {
                break;
            }
            pPre = pNext;
            pNext = pPre->pNext;
        }
        pPre->pNext = pMsgProc;
        pMsgProc->pNext = pNext;
    }

end:
    pthread_mutex_unlock(&pQueue->f_lock);
}

/**********************************************************************
* @name      : ble_check_userfile
* @brief     ：检查用户配置文件, 如果没有就创建
               文件大小 4 + 1024 * 2; 前4个字节中存放校验
* @param[in] ：char *filename 文件名
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
void ble_check_userfile(char *filename)
{
    char check[4] = {0xea, 0xeb, 0xec, 0x01}; /* 最后一位是消息版本号, 每修改一次增加1 */
    char buf[4] = {0};

    if(0 == access(filename, F_OK))
    {
        if(0 == read_pdata_xram(filename, buf, 0, 4))
        {
            BLE_FMT_TRACE("数据文件版本号: %02x.%02x.%02x.%02x, 匹配%02x.%02x.%02x.%02x\n", 
                   buf[0], buf[1], buf[2], buf[3], check[0], check[1], check[2], check[3]);
            if(0 == memcmp(buf, check, 4))
            {
                return;
            }
        }
        BLE_FMT_DEBUG("创建新的数据文件\n");
    }

    char writebuf[24 + 32 + 48 * BLE_PORT_MAX] = {0};
    
    write_pdata_xram(filename, check, 0, 4);
    write_pdata_xram(filename, writebuf, 4, 24 + 32 + 48 * BLE_PORT_MAX);
}

/**********************************************************************
* @name      : ble_read_config
* @brief     ：读取蓝牙配置 存放位置 4
* @param[in] ：
* @param[out]：BLE_CONFIG_T *pConfig  蓝牙配置
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int ble_read_config(BLE_CONFIG_T *pConfig)
{
    uint8 bConfig = FALSE;
    
    if(NULL == pConfig)
    {
        return -1;
    }

    read_pdata_xram(g_bledatafile, (char *)&bConfig, 4, 1);
    if(TRUE == bConfig)
    {
        return read_pdata_xram(g_bledatafile, (char *)pConfig, 4 + 1, sizeof(BLE_CONFIG_T));
    }
    return -1;
}

/**********************************************************************
* @name      : ble_write_config
* @brief     ：保存蓝牙配置 存放位置 4
* @param[in] ：BLE_CONFIG_T *pConfig  蓝牙配置
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int ble_write_config(BLE_CONFIG_T *pConfig)
{
    uint8 bConfig = TRUE;
    if(NULL == pConfig)
    {
        return -1;
    }
    write_pdata_xram(g_bledatafile, (char *)&bConfig, 4, 1);
    return write_pdata_xram(g_bledatafile, (char *)pConfig, 4 + 1, sizeof(BLE_CONFIG_T));
}

/**********************************************************************
* @name      : ble_read_match
* @brief     ：读取本机配对参数 存放位置 4 + 24
* @param[in] ：
* @param[out]：BLE_MATCH_T *pMatch    配对参数
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int ble_read_match(BLE_MATCH_T *pMatch)
{
    uint8 bConfig = FALSE;
    
    if(NULL == pMatch)
    {
        return -1;
    }

    read_pdata_xram(g_bledatafile, (char *)&bConfig, 4 + 24, 1);
    if(TRUE == bConfig)
    {
        return read_pdata_xram(g_bledatafile, (char *)pMatch, 4 + 24 + 1, sizeof(BLE_MATCH_T));
    }
    return -1;
}

/**********************************************************************
* @name      : ble_write_match
* @brief     ：保存蓝牙配置 存放位置 4 + 24
* @param[in] ：BLE_MATCH_T *pMatch   配对参数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int ble_write_match(BLE_MATCH_T *pMatch)
{
    uint8 bConfig = TRUE;
    if(NULL == pMatch)
    {
        return -1;
    }
    write_pdata_xram(g_bledatafile, (char *)&bConfig, 4 + 24, 1);
    return write_pdata_xram(g_bledatafile, (char *)pMatch, 4 + 24 + 1, sizeof(BLE_CONFIG_T));
}

/**********************************************************************
* @name      : ble_read_portcfg
* @brief     ：读取本机配对参数 存放位置 4 + 24 + 32 + port * 48
* @param[in] ：uint32 port            端口号
* @param[out]：BLE_MATCH_T *pMatch    配对参数
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int ble_read_portcfg(uint32 port, BLE_PORT_CONF_T *pPortcfg)
{
    uint8 bConfig = FALSE;
    
    if(NULL == pPortcfg || port >= BLE_PORT_MAX)
    {
        return -1;
    }

    read_pdata_xram(g_bledatafile, (char *)&bConfig, 4 + 24 + 32 + port * 48, 1);
    if(TRUE == bConfig)
    {
        return read_pdata_xram(g_bledatafile, (char *)pPortcfg, 4 + 24 + 32 + port * 48 + 1, sizeof(BLE_PORT_CONF_T));
    }
    return -1;
}

/**********************************************************************
* @name      : ble_write_match
* @brief     ：保存蓝牙配置 存放位置 4 + 24
* @param[in] ：uint32 port            端口号
               BLE_MATCH_T *pMatch   配对参数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int ble_write_portcfg(uint32 port, BLE_PORT_CONF_T *pPortcfg)
{
    uint8 bConfig = TRUE;
    if(NULL == pPortcfg || port >= BLE_PORT_MAX)
    {
        return -1;
    }
    write_pdata_xram(g_bledatafile, (char *)&bConfig, 4 + 24 + 32 + port * 48, 1);
    return write_pdata_xram(g_bledatafile, (char *)pPortcfg, 4 + 24 + 32 + port * 48 + 1, sizeof(BLE_PORT_CONF_T));
}

/**********************************************************************
* @name      : ble_init_config
* @brief     ：初始化配置文件
* @param[in] ：
* @param[out]：uint32 *ip    服务端地址
               uint16 *port  服务端端口
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-8
* @Update    : 
**********************************************************************/
int ble_init_config(uint32 *ip, uint16 *port)
{
    dictionary *ini = NULL;

    uint32 ipaddr = 0;
    char  *stringvalue = NULL;
    int    intvalue = 0;
    char   curpath[256] = {0};
    char   filename[256] = {0};
    int    i;
    
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("wireless get err path!\n");
    }
    //获取当前目录绝对路径，即去掉程序名

    for (i = cnt; i >= 0; --i)
    {
        if (curpath[i] == '/')
        {
            curpath[i+1] = '\0';
            break;
        }
    }

    sprintf(filename, "%s%s", curpath, BLE_INI_FILE);
    /* 日志模块初始化 */
    diag_init(filename, TRUE);
    
    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* ip获取 */
        stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "172.17.0.1");
        ipaddr = inet_addr(stringvalue);
        *ip = ntohl(ipaddr);

        /* 端口获取 */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        *port = (uint16)intvalue;
        
        /* 获取data路径 */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:DATAPATH", "/data/app/btManager/commFile");
        if(0 != access(stringvalue, F_OK))
        {
            /* 必须先创建文件夹 */
            if(0 != mkdir(stringvalue, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
            {
                printf("Create data filepath %s, failed\n", stringvalue);
            }
        }
        sprintf(g_bledatafile, "%s/%s", stringvalue, BLE_DATA_FILE);
        iniparser_freedict(ini);
    }
    else
    {
        printf("wireless get no %s!\n", BLE_INI_FILE);
        ipaddr = inet_addr("172.17.0.1");
        *ip = ntohl(ipaddr);
        *port = 1883;
        sprintf(g_bledatafile, "%s/%s", "/data/app/btManager/commFile", BLE_DATA_FILE);
    }

    /* 恢复配置文件 */
    ble_check_userfile(g_bledatafile);
    if(0 == ble_read_config(&g_tBLEPublic.tDevStatus.config))
    {
        g_tBLEPublic.tDevStatus.bConfig = TRUE;
        BLE_FMT_DEBUG("历史配置, 名称: %s, MAC: %02X:%02X:%02X:%02X:%02X:%02X, 功率: %d, 广播间隔%dms, 扫描间隔%dms\n",
                      g_tBLEPublic.tDevStatus.config.name, g_tBLEPublic.tDevStatus.config.MAC[0],
                      g_tBLEPublic.tDevStatus.config.MAC[1], g_tBLEPublic.tDevStatus.config.MAC[2],
                      g_tBLEPublic.tDevStatus.config.MAC[3], g_tBLEPublic.tDevStatus.config.MAC[4],
                      g_tBLEPublic.tDevStatus.config.MAC[5], g_tBLEPublic.tDevStatus.config.power,
                      g_tBLEPublic.tDevStatus.config.broadcast, g_tBLEPublic.tDevStatus.config.scan);
    }
    else
    {
        g_tBLEPublic.tDevStatus.bConfig = TRUE;
        BLE_FMT_DEBUG("无历史配置, 使用默认配置\n");
        g_tBLEPublic.tDevStatus.config.broadcast = 40;
        g_tBLEPublic.tDevStatus.config.scan = 100;
        g_tBLEPublic.tDevStatus.config.power = 0;
        memcpy(g_tBLEPublic.tDevStatus.config.name, "000", 3);
        g_tBLEPublic.tDevStatus.config.len = 3;
        memset(g_tBLEPublic.tDevStatus.config.MAC, 0, 6);
        g_tBLEPublic.tDevStatus.config.MAC[0] = 0xC2;
    }

    if(0 == ble_read_match(&g_tBLEPublic.tDevStatus.match))
    {
        g_tBLEPublic.tDevStatus.bMatch = TRUE;
        BLE_FMT_DEBUG("主模式历史配对参数: %d[%s]\n", g_tBLEPublic.tDevStatus.match.mode,
                                                      g_tBLEPublic.tDevStatus.match.pwd);
    }
    else
    {
        g_tBLEPublic.tDevStatus.bMatch = TRUE;
        BLE_FMT_DEBUG("主模式历史配对参数, 使用默认配置\n");
        g_tBLEPublic.tDevStatus.match.mode = 1;
        g_tBLEPublic.tDevStatus.match.pwd[0] = 0;
        g_tBLEPublic.tDevStatus.match.len = 0;
    }

    for(i = 0; i < BLE_PORT_MAX; i++)
    {
        if(0 == ble_read_portcfg(i, &(g_tBLEPublic.tDevStatus.PortCfg[i].porttCfg)))
        {
            g_tBLEPublic.tDevStatus.PortCfg[i].bConfig = TRUE;
            BLE_FMT_DEBUG("端口%d历史配置, 模式: %s, MAC: %02X:%02X:%02X:%02X:%02X:%02X, 配对参数: %d[%s]\n",
                          g_tBLEPublic.tDevStatus.PortCfg[i].porttCfg.port, 
                          BLE_MODE_MASTER == g_tBLEPublic.tDevStatus.PortCfg[i].porttCfg.mode ? "主" : "从",
                          g_tBLEPublic.tDevStatus.PortCfg[i].porttCfg.MAC[0],
                          g_tBLEPublic.tDevStatus.PortCfg[i].porttCfg.MAC[1],
                          g_tBLEPublic.tDevStatus.PortCfg[i].porttCfg.MAC[2],
                          g_tBLEPublic.tDevStatus.PortCfg[i].porttCfg.MAC[3],
                          g_tBLEPublic.tDevStatus.PortCfg[i].porttCfg.MAC[4],
                          g_tBLEPublic.tDevStatus.PortCfg[i].porttCfg.MAC[5],
                          g_tBLEPublic.tDevStatus.PortCfg[i].porttCfg.match.mode,
                          g_tBLEPublic.tDevStatus.PortCfg[i].porttCfg.match.pwd);
        }
    }
    
    return 0;
}

/**********************************************************************
* @name      : ble_version_print
* @brief     ：版本打印
* @param[in] ：uint32 buflen     缓存长度
* @param[out]：char *buf         输出日志
* @return    ：实际长度
* @Create    : 王津剑
* @Date      ：2021-10-28
* @Update    :
**********************************************************************/
int ble_version_print(char *buf, uint32 buflen)
{
    return snprintf(buf, buflen, "Version: %s, Time: %s", APP_VERSION, APP_PUBLISH_TIME);
}

/**********************************************************************
* @name      : ble_get_appid
* @brief     ：获取APPID
* @param[in] ：
* @param[out]：BLE_DATA_T *pData  进程共有数据
* @return    ：
**********************************************************************/
void ble_get_appid(BLE_DATA_T *pData)
{
    int ret = 0;
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        pData->appidlen = 24;
        BLE_FMT_DEBUG("devinfo hal_device_get error\n");
    }
    else
    {
        pData->appidlen = 24;
        ret = dev->devinfo_read_appid(dev, BLE_APP_NAME,(char*)pData->appid, BLE_APPID_MAX);
        if(ERR_OK != ret)
        {
            BLE_FMT_DEBUG("devinfo devinfo_read_appid error, ret %d\n", ret);
        }
        else
        {
            BLE_BUF_DEBUG(pData->appid, pData->appidlen, "APPID");
        }
        hal_device_release((HW_DEVICE *)dev);
    }

    return;
}


/**********************************************************************
* @name      : ble_app_init
* @brief     ：蓝牙初始化
* @param[in] ：
* @param[out]：void **ppPublicData
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int ble_app_init(void **ppPublicData)
{
    int           ret = 0;
    
    /* 检查程序只启动了一个副本 */
    if(!singleton_check(BT_APP_NAME))
    {
        printf("%s is started!\n", "btManager");
        return -1;
    }

    BLE_FMT_DEBUG("btManager start: Version: %s, Time: %s\n", APP_VERSION, APP_PUBLISH_TIME);

    /* 数据初始化 */
    memset(&g_tBLEPublic, 0, sizeof(BLE_DATA_T));
    pthread_mutex_init(&g_tBLEPublic.tDevPorc.cfgQueue.f_lock, NULL);
    pthread_mutex_init(&g_tBLEPublic.tDevPorc.transQueue.f_lock, NULL);
    pthread_mutex_init(&g_tBLEPublic.tDevPorc.msgQueue.f_lock, NULL);

    /* 配置文件 */
    ret = ble_init_config(&g_tBLEPublic.mqttip, &g_tBLEPublic.mqttport);
    if(0 != ret)
    {
        printf("ble_init_config failed!\n");
        return -3;
    }

    /* mqtt初始化 */
    ret = ble_mqtt_init(g_tBLEPublic.mqttip, g_tBLEPublic.mqttport);
    if(0 != ret)
    {
        printf("wireless_mqtt_init failed!\n");
        return -4;
    }

    #ifndef PRODUCT_SCU
    /* 获取APPID */
    ble_get_appid(&g_tBLEPublic);
    #endif

    #if PIN_ENABLE == 1
    /* 安全加密相关的初始化 */
    if (!SaveTESAMInfo())
    {
        APP698_FMT_DEBUG("SaveTESAMInfo failed!\n");
    }

    APP698_FMT_DEBUG("SaveTESAMInfo finish!\n");
    #endif

    *ppPublicData = &g_tBLEPublic;
    return 0;
}

/**********************************************************************
* @name      : ble_stoi
* @brief     ：字符串转整形
* @param[in] ：str  字符串
* @param[out]：
* @return    ：整形
**********************************************************************/
int ble_stoi(const char *str)
{
    assert(str != NULL);//判空断言，与函数实现无关
    if(str==NULL)
        return 0;

    int tmp=0;
    while(*str !='\0')
    {
        if(*str>'0' && *str<='9')//if(isdigit(*str)) 推荐使用函数isdigit,检查字符是否为字符‘0’~~‘9’
        {
            tmp=tmp*10+(*str -'0');//获得数字并“叠加”
        }
        str++;//指针后移
    }

    return tmp;
}