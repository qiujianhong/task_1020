/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控初始化
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#ifndef PRODUCT_SCU
#include "hal.h"
#include "devinfo.h"
#endif
#include <arpa/inet.h>
#include "framecomm.h"
#include "iniparser.h"
#include "cJSON.h"
#include "task_msg.h"
#include "db_api.h"
#include "uart_comm.h"
#include "uart_api.h"
#include "uart_service.h"
#ifndef PRODUCT_SCU
#include "hal.h"
#endif

extern int uart_mqtt_init(UART_APPDATA_T *pData);
extern UART_INFO_T g_uart_info[];
extern int g_uart_info_num;
char  g_uart_datafile[256] = {0};

/**********************************************************************
* @name      : uart_mode_tostring
* @brief     ：串口模式转描述
* @param[in] ：mode     串口模式
* @param[out]：modestr 串口模式字符串
* @return    ：0-成功/失败
* @Create    : 贺宁
* @Date      ：2021-8-7
* @Update    :
**********************************************************************/
int uart_mode_tostring(UART_MODE_E         mode ,char *modestr)
{
    switch(mode)
    {
        case UART_MODE_ONLY:   strcpy(modestr, "only");   break;
        case UART_MODE_MASTER: strcpy(modestr, "master"); break;
        case UART_MODE_SLAVE:  strcpy(modestr, "slave");  break;
        default: break;
    }

    return 0;
}

/**********************************************************************
* @name      : uart_parity_tostring
* @brief     ：串口校验位转描述
* @param[in] ：mode     串口模式
* @param[out]：modestr 串口模式字符串
* @return    ：0-成功/失败
* @Create    : 贺宁
* @Date      ：2021-8-7
* @Update    :
**********************************************************************/
int uart_parity_tostring(UART_PARITY_E          parity ,char *paritystr)
{
    switch(parity)
    {
        case UART_PARITY_NONE: strcpy(paritystr, "none"); break;
        case UART_PARITY_ODD:  strcpy(paritystr, "odd"); break;
        case UART_PARITY_EVEN: strcpy(paritystr, "even"); break;
        default: break;
    }

    return 0;
}

/**********************************************************************
* @name      : uart_datafile_create
* @brief     ：创建数据文件
* @param[in] ：char *filename    配置文件名称
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 贺宁
* @Date      ：2021-7-28
* @Update    :
**********************************************************************/
int uart_datafile_create(char *filename)
{
    int     ret = 0;
    cJSON  *json = NULL;
    cJSON  *jList = NULL;
    cJSON  *jArray = NULL;
    cJSON  *item = NULL;    
    cJSON  *jAPPList = NULL;
    cJSON  *jAPPArray = NULL;
    int     i = 0;
    UART_INFO_T *pUartInfo = NULL;
    UART_DEVDES_T *pUart = NULL;
    char desc[8] = {0};
    uint32 baudRate = 0;

    json = cJSON_CreateObject();
    if(NULL == json)
    {
        return ERR_PNULL;
    }

    jList = cJSON_CreateArray();
    if(NULL == jList)
    {
        ret = ERR_PNULL;
        goto end;
    }
    
    cJSON_AddItemToObject(json, "UART_CFG", jList);

    for(i = 0; i < g_uart_info_num; i++)
    {
        jArray = cJSON_CreateObject();
        if(NULL == jArray)
        {
            ret = ERR_PNULL;
            goto end;
        }

        pUartInfo = &g_uart_info[i];
        pUart = &pUartInfo->uart;
        
        cJSON_AddItemToArray(jList, jArray);
        cJSON_AddItemToObject(jArray, "name", cJSON_CreateString(pUartInfo->comname));

        cJSON_AddItemToObject(jArray, "port", cJSON_CreateString(pUart->devname));

        memset(desc, '\0', sizeof(desc));
        uart_mode_tostring(pUartInfo->mode, desc);
        cJSON_AddItemToObject(jArray, "mode", cJSON_CreateString(desc));

        if(pUartInfo->mode == UART_MODE_ONLY)
        {
            cJSON_AddItemToObject(jArray, "appName", cJSON_CreateString(pUartInfo->onlyApp));
        }

        if((pUartInfo->mode == UART_MODE_ONLY) || (pUartInfo->mode == UART_MODE_SLAVE))
        {        
            item = cJSON_CreateObject();
            if(NULL == item)
            {
                ret = ERR_PNULL;
                goto end;
            }

            cJSON_AddItemToObject(jArray, "param", item);
            UART_BAUD_TO_STRING(baudRate, pUart->param.baudRate);

            cJSON_AddItemToObject(item, "baudRate", cJSON_CreateNumber(baudRate));
            cJSON_AddItemToObject(item, "byteSize", cJSON_CreateNumber(pUart->param.dataBits));

            memset(desc, '\0', sizeof(desc));
            uart_parity_tostring(pUart->param.parity, desc);
            cJSON_AddItemToObject(item, "parity", cJSON_CreateString(desc));
            cJSON_AddItemToObject(item, "stopBits", cJSON_CreateNumber(pUart->param.stopBits));
        }

        if(pUartInfo->mode == UART_MODE_SLAVE)
        {
            jAPPList = cJSON_CreateArray();
            if(NULL == jAPPList)
            {
                ret = ERR_PNULL;
                goto end;
            }

            cJSON_AddItemToObject(jArray, "appList", jAPPList);
            jAPPArray = cJSON_CreateObject();
            if(NULL == jAPPArray)
            {
                ret = ERR_PNULL;
                goto end;
            }

            //放一个空的做示例
            cJSON_AddItemToArray(jAPPList, jAPPArray);
            cJSON_AddItemToObject(jAPPArray, "name", cJSON_CreateString(""));
        }        
    }

    cJSON_write(json, filename);

end:
    cJSON_Delete(json);   

    return ret;
}

void Init_port_param()
{
	uint16 i=0;

	for(i = 0; i < g_uart_info_num; i++)
	{
		if(g_uart_info[i].uart.comType == COM_UART)
		{
			g_uart_info[i].uart.param.baudRate = UART_BAUD_9600;
			g_uart_info[i].uart.param.dataBits =8;
			g_uart_info[i].uart.param.parity = UART_PARITY_EVEN;
			g_uart_info[i].uart.param.stopBits =1;
		}
		else if(g_uart_info[i].uart.comType == COM_CAN)
		{
			g_uart_info[i].uart.param.baudRate = UART_BAUD_500K;
			g_uart_info[i].uart.param.dataBits =8;
			g_uart_info[i].uart.param.parity = UART_PARITY_EVEN;
			g_uart_info[i].uart.param.stopBits =1;

		}
	}
}

/**********************************************************************
* @name      : uart_paramcfg_proc
* @brief     ：参数配置文件处理
* @param[in] ：char *curpath   当前文件夹路径
               char *cfgpath   配置文件夹路径
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-8-6
* @Update    :
**********************************************************************/
int uart_datafile_deal(char *curpath, char *cfgpath)
{
    cJSON  *json = NULL;
    cJSON  *jList = NULL;
    cJSON  *jArray = NULL;
    cJSON  *item = NULL;
    cJSON  *subitem = NULL;
    cJSON  *jAPPList = NULL;
    cJSON  *jAPPArray = NULL;    
    int     i = 0, j = 0, k = 0;
    int    num = 0;
    UART_INFO_T *pUartInfo = NULL;
    UART_DEVDES_T *pUart = NULL;

    /* 判断是否需要文件拷贝 */
    if(0 != access(cfgpath, F_OK))
    {
        if(0 != mkdir(cfgpath, 0755))
        {
            INIT_FMT_DEBUG("%s create failed!", cfgpath);
        }
    }

    /* 读取参数 */
    sprintf(g_uart_datafile, "%s/"UART_DATAFILE, cfgpath);

    /* 判断是否需要文件拷贝 */
    if(0 != access(g_uart_datafile, F_OK))
    {
        char cmd[512] = {0};
        sprintf(cmd, "cp -r %s/"UART_DATAFILE" %s", curpath, cfgpath);
        system(cmd);
        INIT_FMT_DEBUG("%s\n", cmd);
    }
    
    json = cJSON_read(g_uart_datafile);
    if(!cJSON_IsObject(json))
    {
        INIT_FMT_DEBUG("cJSON_Parse error, no User config.\n");
        return ERR_PNULL;
    }

    jList = cJSON_GetObjectItem(json, "UART_CFG");
    if(!cJSON_IsArray(jList))
    {
        INIT_FMT_DEBUG("cJSON_Parse error, no list item.\n");
        goto end;
    }

    num = cJSON_GetArraySize(jList);
    
    for(i = 0; i < num; i++)
    {
        jArray = cJSON_GetArrayItem(jList, i);
        if(!cJSON_IsObject(jArray))
        {
            continue;
        }
        
        item = cJSON_GetObjectItem(jArray, "name");
        if(!cJSON_IsString(item))
        {
            continue;
        }

        for(j = 0; j < g_uart_info_num; j++)
        {
            if(strcmp(item->valuestring, g_uart_info[j].comname) != 0)
            {
                continue;
            }

            pUartInfo = &g_uart_info[j];
            pUart = &pUartInfo->uart;

            //串口模式
            item = cJSON_GetObjectItem(jArray, "mode");
            if(cJSON_IsString(item))
            {
                if(strcmp(item->valuestring, "only") == 0)
                {
                    pUartInfo->mode = UART_MODE_ONLY;
                }
                else if(strcmp(item->valuestring, "master") == 0)
                {
                    pUartInfo->mode = UART_MODE_MASTER;
                }
                else if(strcmp(item->valuestring, "slave") == 0)
                {
                    pUartInfo->mode = UART_MODE_SLAVE;
                }
            }

            if(pUartInfo->mode == UART_MODE_ONLY)
            {                
                item = cJSON_GetObjectItem(jArray, "appName");
                if(cJSON_IsString(item))
                {
                    strcpy(pUartInfo->onlyApp, item->valuestring);
                }
            }

//            if((pUartInfo->mode == UART_MODE_ONLY) || (pUartInfo->mode == UART_MODE_SLAVE))
            {              
                item = cJSON_GetObjectItem(jArray, "param");
                if(cJSON_IsObject(item))
                {
                    subitem = cJSON_GetObjectItem(item, "baudRate");
                    if(cJSON_IsNumber(subitem))
                    {
                        UART_STRING_TO_BAUD(subitem->valueint, pUart->param.baudRate);
                    }
                    
                    subitem = cJSON_GetObjectItem(item, "byteSize");
                    if(cJSON_IsNumber(subitem))
                    {
                        pUart->param.dataBits = subitem->valueint;
                    }
                    
                    subitem = cJSON_GetObjectItem(item, "parity");
                    if(cJSON_IsString(subitem))
                    {
                        if(strcmp(subitem->valuestring, "none") == 0)
                        {
                            pUart->param.parity = UART_PARITY_NONE;
                        }
                        else if(strcmp(subitem->valuestring, "odd") == 0)
                        {
                            pUart->param.parity = UART_PARITY_ODD;
                        }
                        else if(strcmp(subitem->valuestring, "even") == 0)
                        {
                            pUart->param.parity = UART_PARITY_EVEN;
                        }                
                    }
                    
                    subitem = cJSON_GetObjectItem(item, "stopBits");
                    if(cJSON_IsNumber(subitem))
                    {
                        pUart->param.stopBits = subitem->valueint;
                    }
                }
            }

            if(pUartInfo->mode == UART_MODE_SLAVE)
            {
                jAPPList = cJSON_GetObjectItem(jArray, "appList");
                if(cJSON_IsArray(jAPPList))
                {
                    pUartInfo->listNum = cJSON_GetArraySize(jAPPList);
                    for(k = 0; k < pUartInfo->listNum; k++)
                    {
                        jAPPArray = cJSON_GetArrayItem(jAPPList, k);
                        if(!cJSON_IsObject(jAPPArray))
                        {
                            break;
                        }

                        item = cJSON_GetObjectItem(jAPPArray, "name");
                        if(cJSON_IsString(item))
                        {
                            strcpy(pUartInfo->appList[k], item->valuestring);
                        }
                    }
                }
            }

            break;
        }
    }
end:    
    cJSON_Delete(json);  
   
    return 0;
}

/**********************************************************************
* @name      : uart_datafile_init
* @brief     ：模型初始化, 生效配置文件
* @param[in] ：char *datapath    配置文件路径
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-7-28
* @Update    :
**********************************************************************/
void uart_datafile_init(char *curpath, char *cfgpath)
{
    char   datafile[256] = {0};
    int    ret = 0;    
    
    /* 文件夹判断 */
    if(0 != access(curpath, F_OK))
    {
        if(0 != mkdir(curpath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            INIT_FMT_DEBUG("%s创建失败\n", curpath);
            return;
        }
    }
    
    sprintf(datafile, "%s/%s", curpath, UART_DATAFILE);
    
    /* 配置文件处理 */
    if(0 != access(datafile, F_OK))
    {
        /* 不存在配置文件, 创建一个新的 */
        ret = uart_datafile_create(datafile);
        if(0 != ret)
        {
            INIT_FMT_DEBUG("%s创建失败, %d\n", datafile, ret);
        }
    }

    ret = uart_datafile_deal(curpath, cfgpath);
    if(0 != ret)
    {
        INIT_FMT_DEBUG("%s处理失败, %d\n", datafile, ret);
    }

    return;
}

/**********************************************************************
* @name      : init_config
* @brief     ：初始化配置文件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-2-27
* @Update    : 
**********************************************************************/
int uart_init_config(uint32 *ip, uint16 *port)
{
    dictionary *ini = NULL;

    char  *intername = NULL;
    uint32 ipmask = 0xffffffff;
    uint32 ipaddr = 0;
    char  *stringvalue = NULL;
    int    intvalue = 0;

    char   curpath[256] = {0};
    char   filename[256] = {0};
    int i;

	//初始化端口参数
	Init_port_param();
    //获取当前目录绝对路径，即去掉程序名
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("wireless get err path!\n");
    }
    else
    {
        for (i = cnt; i >=0; --i)
        {
            if (curpath[i] == '/')
            {
                curpath[i+1] = '\0';
                break;
            }
        }
    }
    
    sprintf(filename, "%s%s", curpath, UART_INITFILE);
    
    /* 日志模块初始化 */
    diag_init(filename, TRUE);
    
    printf("filename is %s!\n", filename);
    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* ip获取 */
        intername = iniparser_getstring(ini, "MQTT_CONF:INTERNAME", NULL);
        if(NULL != intername)
        {
            ipaddr = Get_Interface_IP(intername);
            if(0 != ipaddr)
            {
                stringvalue = iniparser_getstring(ini, "MQTT_CONF:IPMASK", "255.255.255.255");
                ipmask = inet_addr(stringvalue);
                ipmask = ntohl(ipmask);
                if(0xffffffff != ipmask)
                {
                    //通过掩码获取服务端地址
                    ipaddr = (ipaddr & ipmask) + 0x01;
                }
            }
            else
            {
                /* 无接口时用IP */
                stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "127.0.0.1");
                ipaddr = inet_addr(stringvalue);
                ipaddr = ntohl(ipaddr);
            }
        }
        else
        {
            /* INTERNAME为空用IP */
            stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "127.0.0.1");
            ipaddr = inet_addr(stringvalue);
            ipaddr = ntohl(ipaddr);
        }
        *ip = ipaddr;

        /* 端口获取 */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        *port = (uint16)intvalue;
        
        /* 获取ipc路径 */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:IPCPATH", "/mnt/ipc_socket");
        task_msg_prepare(stringvalue);

        //获取数据文件路径
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:CFGPATH", "/data/app/"UART_APP_NAME"/config");

        //数据文件获取
        uart_datafile_init(curpath, stringvalue);
    }
    else
    {
        printf("Get no ini file %s!\n", filename);

        *ip = 0x7f000001;
        *port = 1883;
        task_msg_prepare("/mnt/ipc_socket");
        return 0;
    }

    return 0;
}

/**********************************************************************
* @name      : uart_get_appid
* @brief     ：获取APPID
* @param[in] ：
* @param[out]：UART_APPDATA_T *pData  进程共有数据
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-19
* @Update    : 
**********************************************************************/
void uart_get_appid(UART_APPDATA_T *pData)
{
    int ret = 0;
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        INIT_FMT_DEBUG("devinfo hal_device_get error\n");
    }
    else
    {
        pData->appidlen = 24;
        ret = dev->devinfo_read_appid(dev, UART_APP_NAME,(char*)pData->appid, UART_APPID_MAX);
        if(ERR_OK != ret)
        {
            INIT_FMT_DEBUG("devinfo devinfo_read_appid error, ret %d\n", ret);
        }
        else
        {
            INIT_BUF_DEBUG(pData->appid, pData->appidlen, "APPID");
        }
        hal_device_release((HW_DEVICE *)dev);
    }
}

/**********************************************************************
* @name      : uart_app_init
* @brief     ：回路监控初始化
* @param[in] ：
* @param[out]：void **ppPublicData
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int uart_app_init(void **ppPublicData)
{
    UART_APPDATA_T *pData = NULL;
    int           ret = 0;
    /* 检查程序只启动了一个副本 */
    if(!singleton_check(UART_APP_NAME))
    {
        printf("%s is started!\n", UART_APP_NAME);
        return -1;
    }
    
#ifndef PRODUCT_SCU
    /* 加载hal */
    if(0 != hal_init())
    {
        printf("hal_init failed!\n");
        return -2;
    }
#endif

    pData = malloc(sizeof(UART_APPDATA_T));
    if(NULL == pData)
    {
        printf("malloc pbulic data failed!\n");
        return -3;
    }
    memset(pData, 0, sizeof(UART_APPDATA_T));
    pData->appidlen = UART_APPID_MAX;

    /* 配置文件 */
    ret = uart_init_config(&(pData->mqttip), &(pData->mqttport));
    if(0 != ret)
    {
        printf("uart_init_config failed!\n");
        free(pData);
        return -4;
    }

    /* mqtt初始化 */
    ret = uart_mqtt_init(pData);
    if(0 != ret)
    {
        printf("uart_mqtt_init failed!\n");
        free(pData);
        return -5;
    }

    /* 获取APPID */
    uart_get_appid(pData);

    *ppPublicData = pData;
    return 0;
}

/**********************************************************************
* @name      : uart_version_print
* @brief     ：版本打印
* @param[in] ：uint32 buflen     缓存长度
* @param[out]：char *buf         输出日志
* @return    ：实际长度
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int uart_version_print(char *buf, uint32 buflen)
{
    return snprintf(buf, buflen, "Version: %s, Time: %s", APP_VERSION, APP_PUBLISH_TIME);
}

