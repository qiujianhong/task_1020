/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   电台APP 数据
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
#include "radio_data.h"
#include "radio_mqtt.h"
#if PIN_ENABLE == 1
#include "OopAll.h"
#endif

#define RADIO_INI_FILE     "radioManager.ini"
#define RADIO_DATA_FILE    "radio.data"
RADIO_DATA_T g_tRADIOPublic;
char       g_radiodatafile[256] = {0};


/**********************************************************************
* @name      : radio_clean_queue
* @brief     ：清队列
* @param[in] ：pQueue  缓冲区结构指针
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
void radio_clean_queue(RADIO_QUEUE_T *pQueue)
{
    RADIO_DEV_MSG_T *pTemp = NULL;
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
* @name      : radio_read_queue
* @brief     ：读队列
* @param[in] ：pQueue      缓冲区结构指针
* @param[out]：
* @return    ：RADIO_DEV_MSG_T *消息队列首消息
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
RADIO_DEV_MSG_T *radio_read_queue(RADIO_QUEUE_T *pQueue)
{
    RADIO_DEV_MSG_T *pHead = NULL;
    
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
* @name      : radio_write_queue
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
void radio_write_queue(RADIO_QUEUE_T *pQueue, RADIO_DEV_MSG_T *pMsgProc, uint8 bOrder)
{
    RADIO_DEV_MSG_T *pPre = NULL;
    RADIO_DEV_MSG_T *pNext = NULL;

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
* @name      : radio_check_userfile
* @brief     ：检查用户配置文件, 如果没有就创建
               文件大小 4 + 1024 * 2; 前4个字节中存放校验
* @param[in] ：char *filename 文件名
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
void radio_check_userfile(char *filename)
{
    char check[4] = {0xea, 0xeb, 0xec, 0x01}; /* 最后一位是消息版本号, 每修改一次增加1 */
    char buf[4] = {0};

    if(0 == access(filename, F_OK))
    {
        if(0 == read_pdata_xram(filename, buf, 0, 4))
        {
            RADIO_FMT_TRACE("数据文件版本号: %02x.%02x.%02x.%02x, 匹配%02x.%02x.%02x.%02x\n", 
                   buf[0], buf[1], buf[2], buf[3], check[0], check[1], check[2], check[3]);
            if(0 == memcmp(buf, check, 4))
            {
                return;
            }
        }
        RADIO_FMT_DEBUG("创建新的数据文件\n");
    }

    char writebuf[24 + 32 + 48 * RADIO_PORT_MAX] = {0};
    
    write_pdata_xram(filename, check, 0, 4);
    write_pdata_xram(filename, writebuf, 4, 24 + 32 + 48 * RADIO_PORT_MAX);
}
#if 0
/**********************************************************************
* @name      : radio_read_config
* @brief     ：读取电台配置 存放位置 4
* @param[in] ：
* @param[out]：RADIO_CONF_T *pConfig  电台配置
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_read_config(RADIO_CONF_T *pConfig)
{
    uint8 bConfig = FALSE;
    
    if(NULL == pConfig)
    {
        return -1;
    }

    read_pdata_xram(g_radiodatafile, (char *)&bConfig, 4, 1);
    if(TRUE == bConfig)
    {
        return read_pdata_xram(g_radiodatafile, (char *)pConfig, 4 + 1, sizeof(RADIO_CONF_T));
    }
    return -1;
}

/**********************************************************************
* @name      : radio_write_config
* @brief     ：保存电台配置 存放位置 4
* @param[in] ：RADIO_CONF_T *pConfig  电台配置
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_write_config(RADIO_CONF_T *pConfig)
{
    uint8 bConfig = TRUE;
    if(NULL == pConfig)
    {
        return -1;
    }
    write_pdata_xram(g_radiodatafile, (char *)&bConfig, 4, 1);
    return write_pdata_xram(g_radiodatafile, (char *)pConfig, 4 + 1, sizeof(RADIO_CONF_T));
}
#endif

/**********************************************************************
* @name      : radio_read_para
* @brief     ：读取电台配置 存放位置 4
* @param[in] ：
* @param[out]：RADIO_CONF_T *pConfig  电台配置
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_read_para(RADIO_PARA_T *pConfig)
{
    uint8 bConfig = FALSE;
    
    if(NULL == pConfig)
    {
        return -1;
    }

    read_pdata_xram(g_radiodatafile, (char *)&bConfig, 4, 1);
    if(TRUE == bConfig)
    {
        return read_pdata_xram(g_radiodatafile, (char *)pConfig, 4 + 1, sizeof(RADIO_PARA_T));
    }
    return -1;
}

/**********************************************************************
* @name      : radio_write_config
* @brief     ：保存电台配置 存放位置 4
* @param[in] ：RADIO_CONF_T *pConfig  电台配置
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_write_para(RADIO_PARA_T *pConfig)
{
    uint8 bConfig = TRUE;
    if(NULL == pConfig)
    {
        return -1;
    }
    write_pdata_xram(g_radiodatafile, (char *)&bConfig, 4, 1);
    return write_pdata_xram(g_radiodatafile, (char *)pConfig, 4 + 1, sizeof(RADIO_PARA_T));
}

/**********************************************************************
* @name      : radio_read_addr
* @brief     ：读取电台配置 存放位置 12
* @param[in] ：
* @param[out]：RADIO_CONF_T *pConfig  电台配置
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_read_addr(RADIO_ADDR_T *pConfig)
{
    uint8 bConfig = FALSE;
    
    if(NULL == pConfig)
    {
        return -1;
    }

    read_pdata_xram(g_radiodatafile, (char *)&bConfig, 12, 1);
    if(TRUE == bConfig)
    {
        return read_pdata_xram(g_radiodatafile, (char *)pConfig, 12 + 1, sizeof(RADIO_ADDR_T));
    }
    return -1;
}

/**********************************************************************
* @name      : radio_write_config
* @brief     ：保存电台配置 存放位置 12
* @param[in] ：RADIO_CONF_T *pConfig  电台配置
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_write_addr(RADIO_ADDR_T *pConfig)
{
    uint8 bConfig = TRUE;
    if(NULL == pConfig)
    {
        return -1;
    }
    write_pdata_xram(g_radiodatafile, (char *)&bConfig, 12, 1);
    return write_pdata_xram(g_radiodatafile, (char *)pConfig, 12 + 1, sizeof(RADIO_ADDR_T));
}

#if 0
/**********************************************************************
* @name      : radio_read_match
* @brief     ：读取本机配对参数 存放位置 4 + 24
* @param[in] ：
* @param[out]：RADIO_MATCH_T *pMatch    配对参数
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_read_match(RADIO_MATCH_T *pMatch)
{
    uint8 bConfig = FALSE;
    
    if(NULL == pMatch)
    {
        return -1;
    }

    read_pdata_xram(g_radiodatafile, (char *)&bConfig, 4 + 24, 1);
    if(TRUE == bConfig)
    {
        return read_pdata_xram(g_radiodatafile, (char *)pMatch, 4 + 24 + 1, sizeof(RADIO_MATCH_T));
    }
    return -1;
}

/**********************************************************************
* @name      : radio_write_match
* @brief     ：保存电台配置 存放位置 4 + 24
* @param[in] ：RADIO_MATCH_T *pMatch   配对参数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_write_match(RADIO_MATCH_T *pMatch)
{
    uint8 bConfig = TRUE;
    if(NULL == pMatch)
    {
        return -1;
    }
    write_pdata_xram(g_radiodatafile, (char *)&bConfig, 4 + 24, 1);
    return write_pdata_xram(g_radiodatafile, (char *)pMatch, 4 + 24 + 1, sizeof(RADIO_CONF_T));
}

/**********************************************************************
* @name      : radio_read_portcfg
* @brief     ：读取本机配对参数 存放位置 4 + 24 + 32 + port * 48
* @param[in] ：uint32 port            端口号
* @param[out]：RADIO_MATCH_T *pMatch    配对参数
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_read_portcfg(uint32 port, RADIO_PORT_CONF_T *pPortcfg)
{
    uint8 bConfig = FALSE;
    
    if(NULL == pPortcfg || port >= RADIO_PORT_MAX)
    {
        return -1;
    }

    read_pdata_xram(g_radiodatafile, (char *)&bConfig, 4 + 24 + 32 + port * 48, 1);
    if(TRUE == bConfig)
    {
        return read_pdata_xram(g_radiodatafile, (char *)pPortcfg, 4 + 24 + 32 + port * 48 + 1, sizeof(RADIO_PORT_CONF_T));
    }
    return -1;
}

/**********************************************************************
* @name      : radio_write_match
* @brief     ：保存电台配置 存放位置 4 + 24
* @param[in] ：uint32 port            端口号
               RADIO_MATCH_T *pMatch   配对参数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_write_portcfg(uint32 port, RADIO_PORT_CONF_T *pPortcfg)
{
    uint8 bConfig = TRUE;
    if(NULL == pPortcfg || port >= RADIO_PORT_MAX)
    {
        return -1;
    }
    write_pdata_xram(g_radiodatafile, (char *)&bConfig, 4 + 24 + 32 + port * 48, 1);
    return write_pdata_xram(g_radiodatafile, (char *)pPortcfg, 4 + 24 + 32 + port * 48 + 1, sizeof(RADIO_PORT_CONF_T));
}
#endif
extern RADIO_CONF_T g_RadioConfig;
/**********************************************************************
* @name      : radio_init_config
* @brief     ：初始化配置文件
* @param[in] ：
* @param[out]：uint32 *ip    服务端地址
               uint16 *port  服务端端口
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-8
* @Update    : 
**********************************************************************/
int radio_init_config(uint32 *ip, uint16 *port)
{
    dictionary *ini = NULL;

    uint32 ipaddr = 0;
    char  *stringvalue = NULL;
    int    intvalue = 0;
    char   curpath[256] = {0};
    char   filename[256] = {0};
    int    i;
    RADIO_PARA_T para = {0};
    RADIO_ADDR_T addr = {0};
    
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("radio get err path!\n");
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

    sprintf(filename, "%s%s", curpath, RADIO_INI_FILE);
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
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:DATAPATH", "/data/app/radioManager/commFile");
        if(0 != access(stringvalue, F_OK))
        {
            /* 必须先创建文件夹 */
            if(0 != mkdir(stringvalue, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
            {
                printf("Create data filepath %s, failed\n", stringvalue);
            }
        }
        sprintf(g_radiodatafile, "%s/%s", stringvalue, RADIO_DATA_FILE);
        iniparser_freedict(ini);
    }
    else
    {
        printf("radio get no %s!\n", RADIO_INI_FILE);
        ipaddr = inet_addr("172.17.0.1");
        *ip = ntohl(ipaddr);
        *port = 1883;
        sprintf(g_radiodatafile, "%s/%s", "/data/app/radioManager/commFile", RADIO_DATA_FILE);
    }

    /* 恢复配置文件 */
    radio_check_userfile(g_radiodatafile);
    if(0 == radio_read_para(&para))
    {
        RADIO_FMT_DEBUG("历史配置channel=%u,power=%u,sensitivity=%u\n", para.channel, para.power, para.sensitivity);

        if (para.power >= 0 && para.power <= 4 && para.sensitivity >= 0 && para.sensitivity <= 8)
        {
            RADIO_FMT_DEBUG("历史配置校验通过\n");
            g_RadioConfig.channel = para.channel;
            g_RadioConfig.power = para.power;
            g_RadioConfig.sensitivity =  para.sensitivity;
            g_tRADIOPublic.tDevStatus.bParaConfig = TRUE;
        }
        else
        {
            RADIO_FMT_DEBUG("历史配置校验不通过\n");
        }
    }
    else
    {
        RADIO_FMT_DEBUG("无历史配置, 使用默认配置\n");
#if 0
        g_tRADIOPublic.tDevStatus.config.broadcast = 40;
        g_tRADIOPublic.tDevStatus.config.scan = 100;
        g_tRADIOPublic.tDevStatus.config.power = 0;
        memcpy(g_tRADIOPublic.tDevStatus.config.name, "000", 3);
        g_tRADIOPublic.tDevStatus.config.len = 3;
        memset(g_tRADIOPublic.tDevStatus.config.MAC, 0, 6);
        g_tRADIOPublic.tDevStatus.config.MAC[0] = 0xC2;
#endif
    }

    if(0 == radio_read_addr(&addr))
    {
        RADIO_FMT_DEBUG("历史配置 areaCode=%02X %02X,devAddr=%02X %02X %02X,tmnAddr=%02X %02X %02X %02X %02X %02X\n", 
                          addr.areaCode[0], addr.areaCode[1],
                          addr.devAddr[0], addr.devAddr[1], addr.devAddr[2], addr.tmnAddr[0], addr.tmnAddr[1],
                          addr.tmnAddr[2], addr.tmnAddr[3], addr.tmnAddr[4], addr.tmnAddr[5]);

        if (addr.areaCodeLen == 2 && addr.devAddrLen == 3 && addr.tmnAddrLen == 6)
        {
            RADIO_FMT_DEBUG("历史地址配置校验通过\n");
            g_RadioConfig.areaCodeLen = 2;
            memcpy(g_RadioConfig.areaCode, addr.areaCode, sizeof(g_RadioConfig.areaCode));
            g_RadioConfig.devAddrLen = 3;
            memcpy(g_RadioConfig.devAddr, addr.devAddr, sizeof(g_RadioConfig.devAddr));
            g_RadioConfig.tmnAddrLen = 6;
            memcpy(g_RadioConfig.tmnAddr, addr.tmnAddr, sizeof(g_RadioConfig.tmnAddr));
            g_tRADIOPublic.tDevStatus.bAddrConfig = TRUE;
        }
        else
        {
            RADIO_FMT_DEBUG("历史地址配置校验不通过\n");
        }
    }
    else
    {
        RADIO_FMT_DEBUG("无历史地址配置, 使用默认配置\n");
#if 0
        g_tRADIOPublic.tDevStatus.config.broadcast = 40;
        g_tRADIOPublic.tDevStatus.config.scan = 100;
        g_tRADIOPublic.tDevStatus.config.power = 0;
        memcpy(g_tRADIOPublic.tDevStatus.config.name, "000", 3);
        g_tRADIOPublic.tDevStatus.config.len = 3;
        memset(g_tRADIOPublic.tDevStatus.config.MAC, 0, 6);
        g_tRADIOPublic.tDevStatus.config.MAC[0] = 0xC2;
#endif
    }

#if 0
    if(0 == radio_read_match(&g_tRADIOPublic.tDevStatus.match))
    {
        g_tRADIOPublic.tDevStatus.bMatch = TRUE;
        RADIO_FMT_DEBUG("主模式历史配对参数: %d[%s]\n", g_tRADIOPublic.tDevStatus.match.mode,
                                                      g_tRADIOPublic.tDevStatus.match.pwd);
    }
    else
    {
        g_tRADIOPublic.tDevStatus.bMatch = TRUE;
        RADIO_FMT_DEBUG("主模式历史配对参数, 使用默认配置\n");
        g_tRADIOPublic.tDevStatus.match.mode = 1;
        g_tRADIOPublic.tDevStatus.match.pwd[0] = 0;
        g_tRADIOPublic.tDevStatus.match.len = 0;
    }

    for(i = 0; i < RADIO_PORT_MAX; i++)
    {
        if(0 == radio_read_portcfg(i, &(g_tRADIOPublic.tDevStatus.PortCfg[i].porttCfg)))
        {
            g_tRADIOPublic.tDevStatus.PortCfg[i].bConfig = TRUE;
            RADIO_FMT_DEBUG("端口%d历史配置, 模式: %s, MAC: %02X:%02X:%02X:%02X:%02X:%02X, 配对参数: %d[%s]\n",
                          g_tRADIOPublic.tDevStatus.PortCfg[i].porttCfg.port, 
                          RADIO_MODE_MASTER == g_tRADIOPublic.tDevStatus.PortCfg[i].porttCfg.mode ? "主" : "从",
                          g_tRADIOPublic.tDevStatus.PortCfg[i].porttCfg.MAC[0],
                          g_tRADIOPublic.tDevStatus.PortCfg[i].porttCfg.MAC[1],
                          g_tRADIOPublic.tDevStatus.PortCfg[i].porttCfg.MAC[2],
                          g_tRADIOPublic.tDevStatus.PortCfg[i].porttCfg.MAC[3],
                          g_tRADIOPublic.tDevStatus.PortCfg[i].porttCfg.MAC[4],
                          g_tRADIOPublic.tDevStatus.PortCfg[i].porttCfg.MAC[5],
                          g_tRADIOPublic.tDevStatus.PortCfg[i].porttCfg.match.mode,
                          g_tRADIOPublic.tDevStatus.PortCfg[i].porttCfg.match.pwd);
        }
    }
#endif
    return 0;
}

/**********************************************************************
* @name      : radio_version_print
* @brief     ：版本打印
* @param[in] ：uint32 buflen     缓存长度
* @param[out]：char *buf         输出日志
* @return    ：实际长度
* @Create    : 王津剑
* @Date      ：2021-10-28
* @Update    :
**********************************************************************/
int radio_version_print(char *buf, uint32 buflen)
{
    return snprintf(buf, buflen, "Version: %s, Time: %s", APP_VERSION, APP_PUBLISH_TIME);
}

/**********************************************************************
* @name      : radio_get_appid
* @brief     ：获取APPID
* @param[in] ：
* @param[out]：RADIO_DATA_T *pData  进程共有数据
* @return    ：
**********************************************************************/
void radio_get_appid(RADIO_DATA_T *pData)
{
    int ret = 0;
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        pData->appidlen = 24;
        RADIO_FMT_DEBUG("devinfo hal_device_get error\n");
    }
    else
    {
        pData->appidlen = 24;
        ret = dev->devinfo_read_appid(dev, RADIO_APP_NAME,(char*)pData->appid, RADIO_APPID_MAX);
        if(ERR_OK != ret)
        {
            RADIO_FMT_DEBUG("devinfo devinfo_read_appid error, ret %d\n", ret);
        }
        else
        {
            RADIO_BUF_DEBUG(pData->appid, pData->appidlen, "APPID");
        }
        hal_device_release((HW_DEVICE *)dev);
    }

    return;
}


/**********************************************************************
* @name      : radio_app_init
* @brief     ：电台初始化
* @param[in] ：
* @param[out]：void **ppPublicData
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int radio_app_init(void **ppPublicData)
{
    int           ret = 0;
    
    /* 检查程序只启动了一个副本 */
    if(!singleton_check(RADIO_APP_NAME))
    {
        printf("%s is started!\n", "radioManager");
        return -1;
    }

    RADIO_FMT_DEBUG("radioManager start: Version: %s, Time: %s\n", APP_VERSION, APP_PUBLISH_TIME);

#ifndef PRODUCT_SCU
    /* 加载hal */
    if(0 != hal_init())
    {
        printf("hal_init failed!\n");
        return -2;
    }
#endif

    /* 数据初始化 */
    memset(&g_tRADIOPublic, 0, sizeof(RADIO_DATA_T));
    pthread_mutex_init(&g_tRADIOPublic.tDevPorc.cfgQueue.f_lock, NULL);
    pthread_mutex_init(&g_tRADIOPublic.tDevPorc.transQueue.f_lock, NULL);
    pthread_mutex_init(&g_tRADIOPublic.tDevPorc.msgQueue.f_lock, NULL);

    /* 配置文件 */
    ret = radio_init_config(&g_tRADIOPublic.mqttip, &g_tRADIOPublic.mqttport);
    if(0 != ret)
    {
        printf("radio_init_config failed!\n");
        return -3;
    }

    /* mqtt初始化 */
    ret = radio_mqtt_init(g_tRADIOPublic.mqttip, g_tRADIOPublic.mqttport);
    if(0 != ret)
    {
        printf("radio_mqtt_init failed!\n");
        return -4;
    }

    #ifndef PRODUCT_SCU
    /* 获取APPID */
    radio_get_appid(&g_tRADIOPublic);
    #endif

    //#if PIN_ENABLE == 1
    ///* 安全加密相关的初始化 */
    //if (!SaveTESAMInfo())
    //{
    //    APP698_FMT_DEBUG("SaveTESAMInfo failed!\n");
    //}

    //APP698_FMT_DEBUG("SaveTESAMInfo finish!\n");
    //#endif

    *ppPublicData = &g_tRADIOPublic;
    return 0;
}

/**********************************************************************
* @name      : radio_stoi
* @brief     ：字符串转整形
* @param[in] ：str  字符串
* @param[out]：
* @return    ：整形
**********************************************************************/
int radio_stoi(const char *str)
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