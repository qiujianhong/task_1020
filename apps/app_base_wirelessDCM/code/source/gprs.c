/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块GPRS和GSM功能
* @date：    2019-10-29
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "hal.h"
#include "uart.h"
#include <arpa/inet.h>
#include "oopType.h"
#include "db_api.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"

#include "wireless.h"
#include "gprs_gsm.h"
#include "ppp_oper.h"
#include "at_oper.h"

extern uint8 Isusb;

/* 获取网络注册信息 */
uint8 get_network_state(char *pBuf)
{
    char *temp = pBuf;

    while(*temp)
    {
        if(',' == *temp)
        {
          break;
        }
        temp++;
    }
    if(*temp)
    {
        temp++;
        return *temp - '0';
    }
    return 0;
}

/* 获取运营商代码 与OPERATOR_E对应 */
uint8 get_operator_code(char *pBuf)
{
    /* 中国移动CMCC: 46000 46002 46004 46007 */
    /* 中国电信CTCC: 46003 46005 46011 */
    /* 中国联通CUCC: 46001 46006 46009 */
    uint8 uNetopt = WIRELESS_OPT_NONE;
    
    /*验证AT命令应答是否正确*/
    if(NULL != strstr(pBuf, "CMCC") || NULL != strstr(pBuf, "MOBILE")
       || NULL != strstr(pBuf, "46000") || NULL != strstr(pBuf, "46002")
       || NULL != strstr(pBuf, "46004") || NULL != strstr(pBuf, "46007"))
    {   
        uNetopt = WIRELESS_CMCC;
    }
    else if(NULL != strstr(pBuf, "CTCC") || NULL != strstr(pBuf, "TELECOM") || NULL != strstr(pBuf, "CHN-CT")
            || NULL != strstr(pBuf, "46003") || NULL != strstr(pBuf, "46005") 
            || NULL != strstr(pBuf, "46011"))
    {
        uNetopt = WIRELESS_CTCC;
    }
    else if(NULL != strstr(pBuf, "CUCC") || NULL != strstr(pBuf, "UNICOM")
            || NULL != strstr(pBuf, "46001") || NULL != strstr(pBuf, "46006") 
            || NULL != strstr(pBuf, "46009"))
    {
        uNetopt = WIRELESS_CUCC;
    }

    return uNetopt;
}

/* 通过IMSI判断运营商 */
uint8 get_operator_IMSI(uint8 *pBuf)
{
    /* 中国移动CMCC: 46000 46002 46004 46007 */
    /* 中国电信CTCC: 46003 46005 46011 */
    /* 中国联通CUCC: 46001 46006 46009 */
    uint8 uNetopt = WIRELESS_OPT_NONE;
    
    /*验证AT命令应答是否正确*/
    if(0 == memcmp(pBuf, "46000", 5) || 0 == memcmp(pBuf, "46002", 5)
       || 0 == memcmp(pBuf, "46004", 5) || 0 == memcmp(pBuf, "46007", 5))
    {   
        uNetopt = WIRELESS_CMCC;
    }
    else if(0 == memcmp(pBuf, "46003", 5) || 0 == memcmp(pBuf, "46005", 5) 
            || 0 == memcmp(pBuf, "46011", 5))
    {
        uNetopt = WIRELESS_CTCC;
    }
    else if(0 == memcmp(pBuf, "46001", 5) || 0 == memcmp(pBuf, "46006", 5) 
            || 0 == memcmp(pBuf, "46009", 5))
    {
        uNetopt = WIRELESS_CUCC;
    }

    return uNetopt;
}

/* 通过CCID判断运营商 */
uint8 get_operator_CCID(uint8 *pBuf)
{
    /* 中国移动CMCC: 898600 898602 898604 898607 */
    /* 中国电信CTCC: 898603 898611 */
    /* 中国联通CUCC: 898601 898606 898609 */
    uint8 uNetopt = WIRELESS_OPT_NONE;
    
    /*验证AT命令应答是否正确*/
    if(0 == memcmp(pBuf, "898600", 6) || 0 == memcmp(pBuf, "898602", 6)
       || 0 == memcmp(pBuf, "898604", 6) || 0 == memcmp(pBuf, "898607", 6))
    {   
        uNetopt = WIRELESS_CMCC;
    }
    else if(0 == memcmp(pBuf, "898603", 6) || 0 == memcmp(pBuf, "898611", 6))
    {
        uNetopt = WIRELESS_CTCC;
    }
    else if(0 == memcmp(pBuf, "898601", 6) || 0 == memcmp(pBuf, "898606", 6) 
            || 0 == memcmp(pBuf, "898609", 6))
    {
        uNetopt = WIRELESS_CUCC;
    }

    return uNetopt;
}

/**********************************************************************
* @name      : wireless_AT_INIT
* @brief     ：外置协议栈AT_INIT 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_INIT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    UART_DEVICE_T* pUart = NULL; /*串口句柄*/
    uint8 devname[256] = {0};
    
    if(NULL != ptHandle->pUart)
    {
        WIRELESS_FMT_DEBUG("拨号端口%d 残留句柄释放\n", AT_DEV_NUM(ifnum));
        wireless_uart_uninit(ptHandle->pUart);
        ptHandle->pUart = NULL;
    }

    pUart = (UART_DEVICE_T *)wireless_uart_init(dev); /*打开串口*/
    if (NULL == pUart)
    {
        if(stepCnt > 3)
        {
            WIRELESS_FMT_DEBUG("拨号端口%d 多次打开串口%s失败 重启模块!\n", AT_DEV_NUM(ifnum), dev);
            /* 重启设备 */
            return AT_RESTART;
        }
        *delay = 5000;
        return AT_INIT;
    }
    pUart->uart_get_real_dev_name(pUart, devname);
    WIRELESS_FMT_DEBUG("拨号端口%d 打开串口%s成功(设备%s)\n", AT_DEV_NUM(ifnum), dev, devname);
    ptHandle->pUart = pUart;
    *delay = 1000;
    return AT_ST;
}

/**********************************************************************
* @name      : wireless_AT_ST
* @brief     ：外置协议栈AT_ST 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_ST(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; 
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    
    wireless_uart_recv(pUart, (uint8 *)at_get_buf, sizeof(at_get_buf));
    WIRELESS_FMT_DEBUG("模块开机答复: %s\n", at_get_buf);

    return AT_AT;
}

/**********************************************************************
* @name      : wireless_AT_AT
* @brief     ：外置协议栈AT_AT 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_AT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; 
    int  at_status = 0;

    if(0 == at_check_model(ifnum, pUart, 1))
    {
        at_status = at_check(ifnum, pUart, "ATE0\r\n", "OK");     /*串口发送AT命令*/
        if(0 == at_status)
        {
            MYGMR_INFO_T info;
            if(0 == at_get_mygmr(ifnum, pUart, &info))
            {
                /* 拷贝版本信息 */
                memcpy(ptHandle->ptNetPara->VERINFO.manuNum, info.manufacture, 4);
                memcpy(ptHandle->ptNetPara->VERINFO.softversion, info.firmare_version, 4);
                memcpy(ptHandle->ptNetPara->VERINFO.softdata, info.release_date, 6);
                memcpy(ptHandle->ptNetPara->VERINFO.hwversion, info.hw_version, 4);
                memcpy(ptHandle->ptNetPara->VERINFO.hwdata, info.hw_release_date, 6);
                memcpy(ptHandle->ptNetPara->VERINFO.manuExten, info.model, 8);
            }
        }
        wireless_power_status(TRUE);
        return 0 == at_status ? AT_GMM : AT_RESTART;
    }
    else if(stepCnt >= 0)
    {
        if(ptHandle->sendfaild >= 5)
        {
            if(0xFF == ptHandle->commport)
            {
                WIRELESS_FMT_DEBUG("拨号端口%d 模块初始化失败, 切换拨号方式!\n", AT_DEV_NUM(ifnum));
                if(Isusb !=0xAA)
                {
                    wireless_change_type(ptHandle->commtype == WIRELESS_COMMTYPE_UART ? WIRELESS_COMMTYPE_USB : WIRELESS_COMMTYPE_UART);
                }
            }
            else
            {
                WIRELESS_FMT_DEBUG("拨号端口%d 模块初始化失败, 重启模块\n", AT_DEV_NUM(ifnum));
                /* 重启设备 */
            }
            wireless_hw_reboot(10);
            return AT_RESTART;
        }
        else
        {
            ptHandle->sendfaild += 1;
            *delay = 2000;
            return AT_INIT;
        }
    }
    *delay = 1000;
    return AT_AT;
}

/**********************************************************************
* @name      : wireless_AT_GMM
* @brief     ：外置协议栈AT_GMM 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_GMM(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    int  at_status = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    void *pDev = ptHandle->pUart;

    memset(at_get_buf, 0, sizeof(at_get_buf));
    at_status = at_get(ifnum, pDev, "AT$MYTYPE?\r\n", at_get_buf, sizeof(at_get_buf));
    if (0 != at_status)
    {
        return AT_RESTART;
    }
    
    ptHandle->ptNetPara->moduleType = at_get_module_type(at_get_buf, ptHandle->ptNetPara->nettype);
    wireless_get_IMEI(ifnum, ptHandle->pUart, ptHandle->ptNetPara->IMEI, 17);
    return AT_CPIN;
}

/**********************************************************************
* @name      : wireless_AT_CPIN
* @brief     ：外置协议栈GSM_CPIN 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2022-5-26
* @Update    :
**********************************************************************/
uint32 wireless_AT_CPIN(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    void *pUart = ptHandle->pUart; /*串口句柄*/
    int   ret = 0;
    uint16 CheckTime = ptHandle->ptNetPara->SimCheckTime;
    
    if(stepCnt < 1)
    {
        at_get(ifnum, pUart, "AT+CPIN?\r\n", at_get_buf, AT_LENTH); 
    }
    else
    {
        /* 没有SIM卡 10s读有一次GPS */
        at_get_nolog(ifnum, pUart, "AT+CPIN?\r\n", at_get_buf, AT_LENTH); 
    }
    if(NULL != strstr(at_get_buf, "READY"))
    {
        ret = 0;
    }
    else
    {
        ret = 1;
    }
    
    if (0 == ret)
    {
        WIRELESS_FMT_DEBUG("拨号端口%d 检测到SIM卡插入\n", ifnum);
        return AT_CCID;
    }

    if(CheckTime > 0 && stepCnt > CheckTime * 6)
    {
        /* 稳定性测试过程中可能导致模块识别SIM出问题, 需要复位进行恢复. 只在拨号流程中实现即可 */
        WIRELESS_FMT_DEBUG("%s%d 模块%d_%d没有检测到SIM卡, 重启!\n", AT_DEV_NAMENUM(ifnum), ptHandle->commport, 
                                                                     ptHandle->subport);
        return AT_RESTART;
    }

    if(stepCnt >= 12 && 0 == stepCnt % 6)
    {
        memset(at_get_buf, 0, AT_LENTH);
        sprintf(at_get_buf, "设备%d没有检测到SIM卡", ifnum + 1);
        wireless_send_diagStatus(ifnum, at_get_buf, strlen(at_get_buf), 10);
    }
    
    if(stepCnt < 1)
    {
        WIRELESS_FMT_DEBUG("%s%d 模块%d_%d没有检测到SIM卡, 继续检测!\n", AT_DEV_NAMENUM(ifnum), ptHandle->commport, 
                                                                         ptHandle->subport);
    }
    *delay = 10000;
    return AT_CPIN;
}

/**********************************************************************
* @name      : wireless_AT_CCID
* @brief     ：外置协议栈AT_CCID 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_CCID(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    char *temp = NULL;
    void *pDev = ptHandle->pUart; 
    
    if(stepCnt < 1)
    {
        at_get(ifnum, pDev, "AT$MYCCID\r\n", at_get_buf, sizeof(at_get_buf)); /*获取SIM卡系列号*/
    }
    else
    {
        at_get_nolog(ifnum, pDev, "AT$MYCCID\r\n", at_get_buf, sizeof(at_get_buf)); /*获取SIM卡系列号*/
    }
	
    temp = strstr((char *)at_get_buf, "$MYCCID");
    /*若未收到'ERROR'，则SIM卡正常*/
    if (NULL != temp) /*检测SIM卡成功，通知应用层，继续下一步*/
    {
        /* 写入CCID */
        ptHandle->bsimOK = 0;
        memcpy(ptHandle->ptNetPara->ICCID, temp+strlen("$MYCCID:")+2, 20);
        *delay = 1000;
        return WIRELESS_COMMTYPE_USB == ptHandle->commtype ? AT_CSQ : AT_CIMI;;
    }

    if(stepCnt >= 12)
    {
        WIRELESS_FMT_DEBUG("%s%d 模块%d_%d没有读到SIM卡号, 重启!\n", AT_DEV_NAMENUM(ifnum), ptHandle->commport, 
                                                                     ptHandle->subport);
        return AT_RESTART;
    }
    
    if(stepCnt < 1)
    {
        WIRELESS_FMT_DEBUG("%s%d 模块%d_%d没有读到SIM卡号, 继续检测!\n", AT_DEV_NAMENUM(ifnum), ptHandle->commport, 
                                                                         ptHandle->subport);
    }
    *delay = 10000;
    return AT_CCID;
}

/**********************************************************************
* @name      : wireless_AT_CIMI
* @brief     ：外置协议栈AT_CIMI 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_CIMI(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pUart = ptHandle->pUart; /*串口句柄*/
    char  at_get_buf[AT_LENTH] = {0};   /*设备发送缓冲区*/
    int   at_status = 0;

    /*获取SIM卡系列号*/
    memset(at_get_buf, 0, AT_LENTH);
    at_status = at_get(ifnum, pUart, "AT+CIMI\r\n", at_get_buf, AT_LENTH); 
    if (0 != at_status)
    {   
        return AT_RESTART;
    }
    
    /*若未收到'ERROR'，则SIM卡正常*/
    if(NULL == strstr(at_get_buf, "ERROR")) /*检测SIM卡成功，通知应用层，继续下一步*/
    {
        WIRELESS_FMT_DEBUG("%s%d 检测到SIM卡, IMSI: %s\n", AT_DEV_NAMENUM(ifnum), at_get_buf);
        wireless_get_IMSI(at_get_buf, ptHandle->ptNetPara->IMSI, 15);
    }
    else
    {
        WIRELESS_FMT_DEBUG("%s%d 没有获取到IMSI\n", AT_DEV_NUM(ifnum));
    }

    *delay = 100;
    return AT_CSQ;
}


/**********************************************************************
* @name      : wireless_AT_CSQ
* @brief     ：外置协议栈AT_CSQ 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_CSQ(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char               *ptemp;
    int          signal_value;
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    void *pDev = ptHandle->pUart;
    int  at_status = 0;

    if(stepCnt < 1)
    {
        at_status = at_get(ifnum, pDev, "AT+CSQ\r\n", at_get_buf, sizeof(at_get_buf)); /*发送AT+CSQ*/
        ptemp = strstr((char *)at_get_buf, "+CSQ:");
        signal_value = at_get_csqvalue(ptemp);
        if(signal_value <= 0)
        {
            memset(at_get_buf, 0, sizeof(at_get_buf));
            at_status = at_get(ifnum, pDev, "AT+CESQ\r\n", at_get_buf, sizeof(at_get_buf));
            if (0 == at_status)
            {
                ptemp = strstr((char *)at_get_buf, "+CESQ:");
                signal_value = at_get_cesqvalue(ptemp);
            }
        }
        if(0 != at_status)
        {
            return AT_RESTART;
        }

    }
    else
    {
        signal_value = wireless_get_csqvalue(ifnum, pDev);
    }

    if(WIRELESS_COMMTYPE_USB != ptHandle->commtype)
    {
        ptHandle->ptNetPara->networkType = WIRELESS_OPT_NONE;
        ptHandle->ptNetPara->csqvalue = signal_value;
        wireless_send_change(ifnum, ptHandle->ptNetPara->networkType, (uint16)ptHandle->ptNetPara->csqvalue);
    }
    
    /*信号的大小必须在大于6，否则信号弱,回码格式：+CSQ: 9,99 */
    if (signal_value >= MIN_CSQ_VALUE)
    {
        return AT_SYSINFO;
    }
    else if(stepCnt >= AT_GET_CNT)
    {
        WIRELESS_FMT_DEBUG("%s%d信号强度过低, 重启切换APN!\n", AT_DEV_NAMENUM(ifnum));
        wireless_set_nextapn(ptHandle);
        /* 重启设备 */
        return AT_RESTART;
    }
    if(stepCnt < 1)
    {
        WIRELESS_FMT_DEBUG("%s%d信号强度过低, 重新检测!\n", AT_DEV_NAMENUM(ifnum));
    }
    *delay = 5000;
    return AT_CSQ;
}

/**********************************************************************
* @name      : wireless_AT_SYSINFO
* @brief     ：外置协议栈AT_SYSINFO 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_SYSINFO_default(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    char opera[4][20] = {"中国移动", "中国联通", "中国电信", "专网"};
    char network[4][5] = {"2G", "3G", "4G", "5G"};
    void *pDev = ptHandle->pUart; 

    uint8 operat = WIRELESS_OPT_NONE;
    uint8 networkType = 0xff;

    if(stepCnt < 1)
    {
        /* 在这里先设置锁网 */
        sprintf(at_get_buf,"AT$MYSYSINFO=%d\r\n", 
        ptHandle->ptNetPara->networkSet < 4 ? ptHandle->ptNetPara->networkSet + 1 : 1);
        at_check(ifnum, pDev, at_get_buf, "OK");
        memset(at_get_buf, 0, AT_LENTH);
        //查询网络类型(运营商2G,3G,4G,5G)
        at_get(ifnum, pDev, "AT+COPS?\r\n", at_get_buf, sizeof(at_get_buf));     /*发送AT+CREG?*/
    }
    else
    {
        //查询网络类型(运营商2G,3G,4G,5G)
        at_get_nolog(ifnum, pDev, "AT+COPS?\r\n", at_get_buf, sizeof(at_get_buf));     /*发送AT+CREG?*/
    }
    /* 获取运营商名称 */
    operat = get_operator_code(at_get_buf);
    if(WIRELESS_OPT_NONE == operat)
    {
        /* 注册信息查询不到，可以通过IMSI CCID判断运营商 */
        operat = get_operator_IMSI(ptHandle->ptNetPara->IMSI);
        if(WIRELESS_OPT_NONE == operat)
        {
            operat = get_operator_CCID(ptHandle->ptNetPara->ICCID);
        }
    }
    if(WIRELESS_OPT_NONE == operat)
    {
        if (stepCnt > 70)
        {
            WIRELESS_FMT_DEBUG("%s%d获取运营商失败, 重启!\n", AT_DEV_NAMENUM(ifnum));
            /* 重启设备 */
            return AT_RESTART;
        }
        if(stepCnt < 1)
        {
            WIRELESS_FMT_DEBUG("%s%d获取运营商: 未知, 重试!\n", AT_DEV_NAMENUM(ifnum));
        }
        *delay = 2000;
        return AT_SYSINFO;
    }

    networkType = wireless_get_network(ifnum, pDev, NULL, TRUE);
    ptHandle->ptNetPara->operat = operat;
    ptHandle->ptNetPara->networkType = networkType;

    WIRELESS_FMT_DEBUG("%s%d获取运营商: %s, 网络类型: %s\n", AT_DEV_NAMENUM(ifnum), 
                       operat <= WIRELESS_ZWCC ? opera[operat] : "未知", 
                       networkType <= OOP_5G ? network[networkType] : "无网络");

    if(WIRELESS_COMMTYPE_USB != ptHandle->commtype)
    {
        wireless_send_change(ifnum, ptHandle->ptNetPara->networkType, (uint16)ptHandle->ptNetPara->csqvalue);
    }

    /* 获取指定的APN */
    if(0 != wireless_get_apn(ptHandle))
    {
        WIRELESS_FMT_DEBUG("%s%d匹配拨号参数失败: 重试!\n", AT_DEV_NAMENUM(ifnum));
        *delay = 5000;
        return AT_SYSINFO;
    }

    //设置APN
    memset(at_get_buf, 0, sizeof(at_get_buf));
    sprintf(at_get_buf,"AT$MYNETCON=%d,\"APN\",\"%s\"\r\n",0, ptHandle->apn.value);
    at_check(ifnum, pDev, at_get_buf, "OK");

    //设置用户名密码
    memset(at_get_buf, 0, sizeof(at_get_buf));
    if(0 == ptHandle->user.nNum && 0 == ptHandle->pwd.nNum)
    {
        WIRELESS_FMT_DEBUG("拨号%d没有用户名、密码!\n", AT_DEV_NUM(ifnum));
    }
    else
    {
        sprintf(at_get_buf, "AT$MYNETCON=%d,\"USERPWD\",\"%s,%s\"\r\n", 0, ptHandle->user.value, ptHandle->pwd.value);
        at_check(ifnum, pDev, at_get_buf, "OK");
    }
    //设置鉴权模式
    if(ptHandle->ptNetPara->authType < 4)
    {
        memset(at_get_buf, 0, sizeof(at_get_buf));
        if(1 == ptHandle->ptNetPara->authType)
        {
            sprintf(at_get_buf, "AT$MYNETCON=%d,\"AUTH\",%d\r\n", 0, 2);
        }
        else if(2 == ptHandle->ptNetPara->authType)
        {
            sprintf(at_get_buf, "AT$MYNETCON=%d,\"AUTH\",%d\r\n", 0, 1);
        }
        else
        {
            sprintf(at_get_buf, "AT$MYNETCON=%d,\"AUTH\",%d\r\n", 0, ptHandle->ptNetPara->authType);
        }
        at_check(ifnum, pDev, at_get_buf, "OK");
    }

    return AT_CREG;
}

/**********************************************************************
* @name      : wireless_AT_CREG_JS
* @brief     ：外置协议栈AT_CREG 状态机处理(江苏)
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_SYSINFO_JS(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    char opera[4][20] = {"中国移动", "中国联通", "中国电信", "专网"};
    char network[4][5] = {"2G", "3G", "4G", "5G"};
    void *pDev = ptHandle->pUart; 
    char *temp = NULL;
    uint8 ccid[21] = {0};
    
    uint8 operat = WIRELESS_OPT_NONE;
    uint8 networkType = 0xff;

    if(stepCnt < 1)
    {
        /* 在这里先设置锁网 */
        sprintf(at_get_buf,"AT$MYSYSINFO=%d\r\n", 
        ptHandle->ptNetPara->networkSet < 4 ? ptHandle->ptNetPara->networkSet + 1 : 1);
        at_check(ifnum, pDev, at_get_buf, "OK");
    }
    
    networkType = wireless_get_network(ifnum, pDev, &operat, TRUE);
    if(WIRELESS_OPT_NONE == operat)
    {
        if (stepCnt > 24)
        {
            WIRELESS_FMT_DEBUG("%s%d入网失败, 重启!\n", AT_DEV_NAMENUM(ifnum));
            /* 重启设备 */
            return AT_RESTART;
        }
        if(stepCnt < 1)
        {
            WIRELESS_FMT_DEBUG("%s%d等待入网, 重试!\n", AT_DEV_NAMENUM(ifnum));
        }
        *delay = 5000;
        return AT_SYSINFO;
    }

    ptHandle->ptNetPara->operat = operat;
    ptHandle->ptNetPara->networkType = networkType;

    WIRELESS_FMT_DEBUG("%s%d获取运营商: %s, 网络类型: %s\n", AT_DEV_NAMENUM(ifnum), 
                       operat <= WIRELESS_ZWCC ? opera[operat] : "未知", 
                       networkType <= OOP_5G ? network[networkType] : "无网络");

    if(WIRELESS_COMMTYPE_USB != ptHandle->commtype)
    {
        wireless_send_change(ifnum, ptHandle->ptNetPara->networkType, (uint16)ptHandle->ptNetPara->csqvalue);
    }

    /* 更新CCID */
    memset(at_get_buf, 0, sizeof(at_get_buf));
    at_get_nolog(ifnum, pDev, "AT$MYCCID\r\n", at_get_buf, sizeof(at_get_buf)); /*获取SIM卡系列号*/
    temp = strstr((char *)at_get_buf, "$MYCCID");
    /*若未收到'ERROR'，则SIM卡正常*/
    if (NULL != temp) /*检测SIM卡成功，通知应用层，继续下一步*/
    {
        /* 写入CCID */
        memcpy(ccid, temp+strlen("$MYCCID:")+2, 20);
        if(0 != memcmp(ccid, ptHandle->ptNetPara->ICCID, 20))
        {
            memcpy(ptHandle->ptNetPara->ICCID, ccid, 20);
            WIRELESS_FMT_DEBUG("%s%d CCID变化%s\n", AT_DEV_NAMENUM(ifnum), ccid);
        }
    }

    /* 获取指定的APN */
    if(0 != wireless_get_apn(ptHandle))
    {
        WIRELESS_FMT_DEBUG("%s%d匹配拨号参数失败: 重试!\n", AT_DEV_NAMENUM(ifnum));
        *delay = 5000;
        return AT_SYSINFO;
    }

    //设置APN
    memset(at_get_buf, 0, sizeof(at_get_buf));
    sprintf(at_get_buf,"AT$MYNETCON=%d,\"APN\",\"%s\"\r\n",0, ptHandle->apn.value);
    at_check(ifnum, pDev, at_get_buf, "OK");

    //设置用户名密码
    memset(at_get_buf, 0, sizeof(at_get_buf));
    if(0 == ptHandle->user.nNum && 0 == ptHandle->pwd.nNum)
    {
        WIRELESS_FMT_DEBUG("拨号%d没有用户名、密码!\n", AT_DEV_NUM(ifnum));
    }
    else
    {
        sprintf(at_get_buf, "AT$MYNETCON=%d,\"USERPWD\",\"%s,%s\"\r\n", 0, ptHandle->user.value, ptHandle->pwd.value);
        at_check(ifnum, pDev, at_get_buf, "OK");
    }
    //设置鉴权模式
    if(ptHandle->ptNetPara->authType < 4)
    {
        memset(at_get_buf, 0, sizeof(at_get_buf));
        if(1 == ptHandle->ptNetPara->authType)
        {
            sprintf(at_get_buf, "AT$MYNETCON=%d,\"AUTH\",%d\r\n", 0, 2);
        }
        else if(2 == ptHandle->ptNetPara->authType)
        {
            sprintf(at_get_buf, "AT$MYNETCON=%d,\"AUTH\",%d\r\n", 0, 1);
        }
        else
        {
            sprintf(at_get_buf, "AT$MYNETCON=%d,\"AUTH\",%d\r\n", 0, ptHandle->ptNetPara->authType);
        }
        at_check(ifnum, pDev, at_get_buf, "OK");
    }

    return AT_CREG;
}

/**********************************************************************
* @name      : wireless_AT_SYSINFO
* @brief     ：外置协议栈AT_SYSINFO 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_SYSINFO(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    if(0 == ptHandle->bJS1_8G)
    {
        return wireless_AT_SYSINFO_default(ifnum, dev, stepCnt, cnt, ptHandle, delay);
    }
    else
    {
        return wireless_AT_SYSINFO_JS(ifnum, dev, stepCnt, cnt, ptHandle, delay);
    }
}

/**********************************************************************
* @name      : wireless_AT_CREG
* @brief     ：外置协议栈AT_CREG 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_CREG(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    if(0 == wireless_check_newwork_reg(ifnum, ptHandle->pUart, TRUE))
    {
        return AT_CGDCONT;
    }
    else
    {
        if (stepCnt > 30)
        {
            WIRELESS_FMT_DEBUG("拨号%d检测网络没有注册, 重启切换APN!\n", AT_DEV_NUM(ifnum));
            wireless_set_nextapn(ptHandle);
            /* 重启设备 */
            return AT_RESTART;
        }
        else
        {
            *delay = 10000;
            return AT_CREG;
        }
    } 
}

/**********************************************************************
* @name      : wireless_AT_CGDCONT
* @brief     ：外置协议栈AT_CGDCONT 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_CGDCONT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    int              at_status = 0;
    char  at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    void *pDev = ptHandle->pUart;

    /* 设置PDP类型，IP-TCP/IP, PPP-点对点协议 */
    sprintf((char *)at_get_buf, "AT+CGDCONT=1,\"IP\",\"%s\"\r\n", ptHandle->apn.value);
    at_status = at_check(ifnum, pDev, at_get_buf, "OK");
    if (0 != at_status)
    {
        return AT_RESTART;
    }
    else
    {
        *delay = 100;
        return AT_CGATT;
    }
}

/**********************************************************************
* @name      : wireless_AT_CGATT
* @brief     ：外置协议栈AT_CGATT 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_CGATT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    int             at_status = 0;
    char at_get_buf[AT_LENTH] = {0};  /*AT命令返回内容*/
    void *pDev = ptHandle->pUart;

    //先发送一次CGATT，促使模块上线
    at_status = at_check(ifnum, pDev, "AT+CGATT=1\r\n", "OK");
    at_status = at_get(ifnum, pDev, "AT+CGATT?\r\n", at_get_buf, sizeof(at_get_buf));/*发送AT+CREG?*/
    if (0 != at_status)
    {
        return AT_RESTART;
    }
    
    /*判收到的字符串是否为 +CGATT: 1*/
    if (0 == is_str_exist((char *)at_get_buf, "1"))
    {
        *delay = 100;
        return WIRELESS_COMMTYPE_USB == ptHandle->commtype ? AT_ATDT : AT_GETINFO;
    }
    else if(stepCnt > 10)
    {
        wireless_set_nextapn(ptHandle);
        WIRELESS_FMT_DEBUG("%s%d模块没有上线, 切换APN\n", AT_DEV_NAMENUM(ifnum));
        ptHandle->ConnectNum++;
        wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 0, ptHandle->ConnectNum);
        ptHandle->FailedNum++;
        wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
        return AT_RESTART;
    }
    if(stepCnt < 1)
    {
        WIRELESS_FMT_DEBUG("%s%d模块没有上线, 重试\n", AT_DEV_NAMENUM(ifnum));
    }
    *delay = 2000;
    return AT_CGATT;
}

/**********************************************************************
* @name      : wireless_AT_CGATT
* @brief     ：外置协议栈AT_GETINFO 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_GETINFO(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    void *pDev = ptHandle->pUart;

    /* 信号强度 和 网络制式 */
    ptHandle->ptNetPara->csqvalue = wireless_get_csqvalue(ifnum, pDev);
    ptHandle->ptNetPara->networkType = wireless_get_network(ifnum, pDev, NULL, FALSE);
    wireless_send_change(ifnum, ptHandle->ptNetPara->networkType, ptHandle->ptNetPara->csqvalue);

    wireless_get_phonenum(ifnum, ptHandle->pUart, ptHandle->ptNetPara->SIMNum, 24);

    *delay = 100;
    return AT_ATDT;
}

/**********************************************************************
* @name      : wireless_AT_ATDT
* @brief     ：外置协议栈AT_ATDT 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_ATDT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    int   at_status = 0;
    uint8 ch_data_at = 0;    /* ppp模式数据模式切换计数器 */
    void *pDev = ptHandle->pUart;

    ptHandle->ConnectNum++;
    wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 0, ptHandle->ConnectNum);

    if(*cnt != 0)//ppp拨号失败进行重新打开串口将模块从数据态切换会at态
    {
        wireless_uart_uninit(pDev);
        sleep(2);
        pDev = wireless_uart_init(dev); /*打开串口*/
        if(NULL == pDev)
        {
            ptHandle->pUart = NULL;
            WIRELESS_FMT_DEBUG("重新打开串口%s失败\n", dev);
            return AT_RESTART;
        }
        WIRELESS_FMT_DEBUG("重新打开串口%s, 切换到at模式\n", dev);
        ptHandle->pUart = pDev;
        for(ch_data_at = 0; ch_data_at<3; ch_data_at++)
        {
            wireless_uart_send(ptHandle->pUart, (uint8 *)"+", 1);//写+++ 
            usleep(10000);
        }
        at_check(ifnum, pDev, "AT\r\n", "OK");       /*串口发送AT命令*/
    }

    if(1 == ptHandle->ptNetPara->moduleType) /*检测到GPRS模块或模块型号未知*/
    {
        /* CDMA2000 */
        at_status = at_check(ifnum, pDev, "ATDT#777\r\n", "CONNECT");
    }
    else
    {
        //发现目前有很多模块发送ATDT命令后必须带空格，也有部分模块不需要带空格
        //为了对两种模块做兼容，增加了重试功能，将两种命令都发一遍
        //ATDT是1, 对应国网通道0
        at_status = at_check(ifnum, pDev, "ATDT *99***1#\r\n", "CONNECT");
        if(0 != at_status)
        {
            at_status = at_check(ifnum, pDev, "ATDT*99***1#\r\n", "CONNECT");
        }
    }
    if (0 != at_status)
    {
        wireless_set_nextapn(ptHandle);
        WIRELESS_FMT_DEBUG("%s%d切换APN\n", AT_DEV_NAMENUM(ifnum));
        *delay = 5000;
        return AT_RESTART;
    }
    else
    {
        return AT_PPP;
    }
}

/**********************************************************************
* @name      : wireless_AT_PPP
* @brief     ：外置协议栈AT_PPP 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_PPP(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    char      ppp_command[600] = {0};  /* ppp命令 */
    char      devname[128] = {0};
    char      filename[128] = {0};
    char      cmd[128] = {0};
    uint8     index = 0;

    if(0 == ptHandle->devName[0])
    {
        if(0 == ptHandle->subport)
        {
            sprintf(devname, "/tmp/dev/usb_exm%d_2", ptHandle->commport);
        }
        else
        {
            sprintf(devname, "/tmp/dev/usb_exm%d_%d_2", ptHandle->commport, ptHandle->subport);
        }
    }
    else
    {
        sprintf(devname, "%s", ptHandle->devName);
    }

    if(0 == ptHandle->pppdlogMax)
    {
        sprintf(ppp_command, "pppd debug noauth unit %d nocrtscts modem %s 115200 asyncmap 0000000 "
                             "escape FF kdebug 0 0.0.0.0:0.0.0.0 noipdefault netmask 255.255.255.0 defaultroute "
                             "user \"%s\" password \"%s\" connect \"\" lcp-echo-interval 60 lcp-echo-failure 4%s",
                             ifnum, devname, ptHandle->user.value, ptHandle->pwd.value, 
                             0 == ptHandle->bIPV6 ? " noipv6" : "");
    }
    else
    {
        ptHandle->logNo++;
        if(ptHandle->logNo >= ptHandle->pppdlogMax)
        {
            ptHandle->logNo = 0;
        }
        index = ptHandle->logNo;
        sprintf(filename, "%s/ppp%d.%d.log", ptHandle->logpath, ifnum, index);
        if(0 == access(filename, F_OK))
        {
            sprintf(cmd, "rm %s/ppp%d.%d.log", ptHandle->logpath, ifnum, index);
            system(cmd);
            usleep(100000);
        }

        sprintf(ppp_command, "pppd debug noauth unit %d nocrtscts modem %s 115200 asyncmap 0000000 "
                             "escape FF kdebug 0 0.0.0.0:0.0.0.0 noipdefault netmask 255.255.255.0 defaultroute "
                             "user \"%s\" password \"%s\" connect \"\" lcp-echo-interval 60 lcp-echo-failure 4%s "
                             "logfile %s/ppp%d.%d.log",
                             ifnum, devname, ptHandle->user.value, ptHandle->pwd.value, 
                             0 == ptHandle->bIPV6 ? " noipv6" : "", ptHandle->logpath, ifnum, index);
    }
    if (TRUE == wireless_check_ppp_porc(ifnum))
    {
        WIRELESS_FMT_TRACE("pppd was running\n");
    }
    else
    {
        WIRELESS_FMT_DEBUG("\n%s\n", ppp_command);
        system(ppp_command);
    }
    if(ptHandle->pppdlogMax > 0)
    {
        wireless_write_logno(ptHandle->userfile, ifnum + 1, ptHandle->logNo);
    }
    *delay = 1000;
    return AT_LCP;
}

/**********************************************************************
* @name      : wireless_AT_LCP
* @brief     ：外置协议栈AT_PPP1 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_LCP(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    /* 检查网口是否存在 */
    if (TRUE == wireless_check_pppif(ifnum))
    {
        WIRELESS_FMT_DEBUG("%s%d pppd命令执行成功, 存在网口ppp%d!\n", AT_DEV_NAMENUM(ifnum), ifnum);
        return AT_IPCP;
    }
    else if(stepCnt > 30)
    {
        if(*cnt > 2)
        {
            *cnt = 0;
            ptHandle->FailedNum++;
            wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
            wireless_set_nextapn(ptHandle);
            WIRELESS_FMT_DEBUG("%s%d PPP失败, 没有映射网口, 切换APN\n", AT_DEV_NAMENUM(ifnum));
            *delay = 5000;
            return AT_RESTART;
        }
        else
        {
            *cnt += 1;
            ptHandle->FailedNum++;
            wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
            WIRELESS_FMT_DEBUG("%s%d PPP失败, 没有映射网口, 重试\n", AT_DEV_NAMENUM(ifnum));
            return AT_ATDT;
        }
    }

    *delay = 1000;
    return AT_LCP;
}

/**********************************************************************
* @name      : wireless_AT_IPCP
* @brief     ：外置协议栈AT_PPP2 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_IPCP(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    uint32    ipAddr = 0;
    
    /* 获取ip地址 */
    ipAddr = wireless_get_pppIP(ifnum);
    if (0 != ipAddr)
    {
        WIRELESS_FMT_DEBUG("%s%d PPP成功, 网口地址  %d.%d.%d.%d!\n", AT_DEV_NAMENUM(ifnum), 
                           (ipAddr >> 24) & 0xff, (ipAddr >> 16) & 0xff, (ipAddr >> 8) & 0xff, ipAddr & 0xff);

        ptHandle->ptNetPara->ipAddr = ipAddr;
        sleep(1);
        ptHandle->ptNetPara->getway = wireless_get_pppgetway(ifnum); /* 更新网关 */
        WIRELESS_FMT_DEBUG("%s%d 网关 %d.%d.%d.%d!\n", AT_DEV_NAMENUM(ifnum), 
                           (ptHandle->ptNetPara->getway >> 24) & 0xff, (ptHandle->ptNetPara->getway >> 16) & 0xff, 
                           (ptHandle->ptNetPara->getway >> 8) & 0xff, ptHandle->ptNetPara->getway & 0xff);
        *cnt = 0;
        return AT_WAIT;
    }
    else if(stepCnt > 120)
    {
        /* 有些信号弱的模块, 时间长 */
        if (*cnt > 2)      /*如果2次建立PPP不成功，则认为失败*/
        {
            *cnt = 0;
            ptHandle->FailedNum++;
            wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
            wireless_set_nextapn(ptHandle);
            wireless_ppp_off(ifnum);//没有获取不到ip地址结果连上服务器的情况
            WIRELESS_FMT_DEBUG("%s%d PPP失败, 没有分配到IP, 切换APN\n", AT_DEV_NAMENUM(ifnum));
            *delay = 5000;
            return AT_RESTART;
        }
        else    /*重试3次PPP拨号*/
        {
            WIRELESS_FMT_DEBUG("%s%d PPP失败, 没有分配到IP, 继续操作\n", AT_DEV_NAMENUM(ifnum));
            wireless_ppp_off(ifnum);//没有获取不到ip地址结果连上服务器的情况
            *cnt += 1;
            ptHandle->FailedNum++;
            wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
            return AT_ATDT;
        }
    }
    
    *delay = 1000;
    return AT_IPCP;
}

/**********************************************************************
* @name      : wireless_AT_RESTART
* @brief     ：外置协议栈AT_RESTART 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_AT_RESTART(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    WIRELESS_FMT_DEBUG("拨号端口%d 拨号失败\n", AT_DEV_NUM(ifnum));
    if(ptHandle->pUart && 0 == ptHandle->bJS1_8G)
    {
        wireless_uart_uninit(ptHandle->pUart);
        ptHandle->pUart = NULL;
        WIRELESS_FMT_DEBUG("拨号端口%d 句柄释放\n", AT_DEV_NUM(ifnum));
    }

    /* 重启设备 */
    wireless_restart_dev(ifnum, ptHandle->commport + ((ptHandle->subport << 4) & 0xf0));

    *delay = 20000;
    //ptHandle->workAgain = TRUE; //退出当前状态机
    return AT_RESTART;
}

/**********************************************************************
* @name      : wireless_AT_WAIT
* @brief     ：外置协议栈AT_WAIT 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-26
* @Update    :
**********************************************************************/
uint32 wireless_AT_WAIT(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    if(TRUE == ptHandle->ptNetPara->smsStatusOK)
    {
        return PPP_SUCCESS;
    }
    *delay = 1000;
    return AT_WAIT;
}

/**********************************************************************
* @name      : wireless_PPP_SUCCESS
* @brief     ：外置协议栈PPP_SUCCESS 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_PPP_SUCCESS(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    time_t stime = 0;
    WIRELESS_FMT_DEBUG("拨号端口%d 拨号成功\n", AT_DEV_NUM(ifnum));
    
    /* 最后一次成功时间 */
    get_curtime_ms(&ptHandle->Connecttime);
    stime = time(NULL);
    ptHandle->SuccessTimes = stime;
    wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 2, ptHandle->SuccessTimes);
    if(ptHandle->pUart)
    {
        wireless_uart_uninit(ptHandle->pUart);
        ptHandle->pUart = NULL;
        WIRELESS_FMT_DEBUG("拨号端口%d 句柄释放\n", AT_DEV_NUM(ifnum));
    }
    
    ptHandle->ptNetPara->mask = wireless_get_pppMASK(ifnum); 
    ptHandle->dialState = PPP_DAIL_STATUS_ONLINE;
    ptHandle->ptNetPara->getway = wireless_get_pppgetway(ifnum); /* 获取网关 */

    wireless_set_pppstatus(ifnum, 0);
    return PPP_CHECK;
}

/**********************************************************************
* @name      : wireless_PPP_CHECK
* @brief     ：外置协议栈PPP_CHECK 状态机处理
* @param[in] ：uint8 ifnum                  网口编号
               char *dev                    设备名称
               uint32 stepCnt               状态机联系执行次数
               uint32 *cnt                  内部次数
               WIRELESS_HANDLE_T *ptHandle  线程句柄
* @param[out]：uint32 *delaytimems          状态机切换延时时间, 单位ms, 最小识别10ms
* @return    ：下一个状态机
* @Create    : 王津剑
* @Date      ：2020-11-13
* @Update    :
**********************************************************************/
uint32 wireless_PPP_CHECK(uint8 ifnum, char *dev, uint32 stepCnt, uint32 *cnt, WIRELESS_HANDLE_T *ptHandle, uint32 *delay)
{
    uint32        ipAddr = 0;

    ipAddr = wireless_get_pppIP(ifnum);
    ptHandle->ptNetPara->ipAddr = ipAddr;
    if(0 == ipAddr)
    {
        *cnt += 1;

        /* 网口查询 */
        if(FALSE == wireless_check_pppif(ifnum))
        {
            WIRELESS_FMT_DEBUG("拨号端口%d 检查不存在网口\n", AT_DEV_NUM(ifnum));
            *delay = 2000;
            ptHandle->workAgain = TRUE; //退出当前状态机
            ptHandle->FailedNum++;
            wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
            return AT_RESTART;
        }
        
        /* 网口不能获取IP表明, 是被出问题 */
        WIRELESS_FMT_DEBUG("拨号端口%d 检查网口ip地址失败\n", AT_DEV_NUM(ifnum));
        if(*cnt > 30)
        {
            WIRELESS_FMT_DEBUG("拨号端口%d 持续检查网口ip不存在 重新拨号\n", AT_DEV_NUM(ifnum));
            *delay = 2000;
            ptHandle->workAgain = TRUE; //退出当前状态机
            ptHandle->FailedNum++;
            wireless_write_num(ptHandle->userfile, ifnum + 1, &(ptHandle->curDay), 1, ptHandle->FailedNum);
            return AT_RESTART;
        }
        *delay = 1000;
        return PPP_CHECK;
    }

#if 0
    /* 流量统计 目前没有用到 */
    lastTime = ptNetPara->usertime;
    get_curtime_ms(&curTime);
    curFlow = wireless_get_flow(ifnum);
    if(curTime.year != lastTime.year || curTime.month != lastTime.month)
    {
        /* 过月流量清0 */
        ptNetPara->usedflow = 0;
        wireless_write_userflow(ptHandle->userfile, ifnum + 1, ptNetPara->usedflow);
        ptHandle->lastFlow = curFlow;
        ptNetPara->usertime = curTime;
        bFisrtMoreValue = TRUE;
    }
    else if(curFlow < ptHandle->lastFlow)  //异常情况下 小于历史值
    {
        ptHandle->lastFlow = curFlow;
    }
    else if(curFlow - ptHandle->lastFlow > 1024 || curTime.minute != lastTime.minute)
    {   //超过1KB, 或者大于1min记录
        ptNetPara->usedflow += (curFlow - ptHandle->lastFlow);
        wireless_write_userflow(ptHandle->userfile, ifnum + 1, ptNetPara->usedflow);

        ptHandle->lastFlow = curFlow;
        ptNetPara->usertime = curTime;
    }
    
    /* 流量超过限制先记录下来 */
    if(0 < ptNetPara->flowSetValue && TRUE == bFisrtMoreValue)
    {
        if(ptNetPara->flowSetValue < ptNetPara->usedflow/1024)
        {
            bFisrtMoreValue = FALSE;
            WIRELESS_FMT_DEBUG("拨号端口%d 流量超过限定值%uKB\n", AT_DEV_NUM(ifnum), ptNetPara->flowSetValue);
        }
    }
#endif

    *delay = 1000;
    return PPP_CHECK;
}

/**********************************************************************
* @name      : wireless_grps_working
* @brief     ：外置协议栈grps工作
* @param[in] ：uint8 commport               端口号
               uint8 ifnum                  网口编号
               WIRELESS_HANDLE_T *ptHandle  线程数据句柄
* @param[out]：
* @return    ：退出等待时间s
* @Create    : 王津剑
* @Date      ：2019-10-23
* @Update    : 2020-11-13
**********************************************************************/
int wireless_grps_working(uint8 commport, uint8 ifnum, WIRELESS_HANDLE_T *ptHandle)
{
    uint32  i;
    uint32  status = AT_INIT; /*连接步骤*/
    uint32  stepcnt = 0;       /*状态机次数*/
    uint32  othercount = 0;       /*其他计数次数*/
    uint32  delay = 0;
    uint16  buflen = 0;
    
    WIRELESS_WORK_FUN_T statusFuns[] = 
    {
        {2, AT_INIT,    wireless_AT_INIT,    "AT_INIT",   "打开串口", 0, 60},
        {1, AT_ST,      wireless_AT_ST,      "AT_ST",     {0}, 0, 60},
        {1, AT_AT,      wireless_AT_AT,      "AT_AT",     "检测通信模块", 0, 60},
        {1, AT_GMM,     wireless_AT_GMM,     "AT_GMM",    "读取通信模块型号", 0, 60},
        {2, AT_CPIN,    wireless_AT_CPIN,    "AT_CPIN",   "检测SIM卡", 1, 60},
        {2, AT_CCID,    wireless_AT_CCID,    "AT_CCID",   "读取SIM卡号", 1, 60},
        {1, AT_CIMI,    wireless_AT_CIMI,    "AT_CIMI",   {0}, 0, 60},
        {2, AT_CSQ,     wireless_AT_CSQ,     "AT_CSQ",    "获取信号", 0, 60},
        {2, AT_SYSINFO, wireless_AT_SYSINFO, "AT_SYSINFO","获取运营商", 0, 60},
        {2, AT_CREG,    wireless_AT_CREG,    "AT_CREG",   "网络注册", 0, 60},
        {0, AT_CGDCONT, wireless_AT_CGDCONT, "AT_CGDCONT","设置APN", 0, 60},
        {0, AT_CGATT,   wireless_AT_CGATT,   "AT_CGATT",  "检测gprs网络", 0, 60},
        {0, AT_GETINFO, wireless_AT_GETINFO, "AT_GETINFO",{0}, 0, 60},
        {0, AT_ATDT,    wireless_AT_ATDT,    "AT_ATDT",   "开始拨号", 0, 60},
        {1, AT_PPP,     wireless_AT_PPP,     "AT_PPP",    "PPP验证", 0, 60},
        {1, AT_LCP,     wireless_AT_LCP,     "AT_LCP",    "LCP链路协商", 0, 60},
        {1, AT_IPCP,    wireless_AT_IPCP,    "AT_IPCP",   "IPCP协商", 0, 60},
        {1, AT_RESTART, wireless_AT_RESTART, "AT_RESTART","拨号失败", 0, 60},

        {1, AT_WAIT,     wireless_AT_WAIT,      "AT_WAIT",     "等待短信初始化", 0, 60},
        {0, PPP_SUCCESS, wireless_PPP_SUCCESS,  "PPP_SUCCESS", "拨号成功", 0, 60},
        {0, PPP_CHECK,   wireless_PPP_CHECK,  {0},             {0}, 0, 60},
    };

    uint32 statusNum = sizeof(statusFuns)/sizeof(WIRELESS_WORK_FUN_T);

    ptHandle->bsimOK = 1;
    ptHandle->stepStatus = status;

    /* 清除PPP端口 */
    wireless_ppp_off(ifnum);
    
    while(1)
    {   
        /* 异常状态监测 */
        if(FALSE == ptHandle->workFlag || TRUE == ptHandle->workAgain)
        {
            wireless_ppp_off(ifnum);
            WIRELESS_FMT_DEBUG("拨号端口%d 状态异常, 停止工作\n", AT_DEV_NUM(ifnum));
            if(ptHandle->pUart)
            {
                wireless_uart_uninit(ptHandle->pUart);
                ptHandle->pUart = NULL;
                WIRELESS_FMT_DEBUG("拨号端口%d 句柄释放\n", AT_DEV_NUM(ifnum));
            }
            return 20;
        }

        /* 状态机切换延时 */
        if(delay >= 100)
        {
            delay -= 100;
            usleep(100000);
            continue;
        }
        else
        {
            delay = 0;
        }

        /* 状态机执行 */
        for(i = 0; i < statusNum; i++)
        {
            if(status == statusFuns[i].status)
            {
                if(NULL == statusFuns[i].func)
                {
                    WIRELESS_FMT_DEBUG("拨号端口%d 无效状态机 %d, 切换新状态 INIT\n", AT_DEV_NUM(ifnum), status);
                    status = AT_INIT;
                    break;
                }
                if((stepcnt < statusFuns[i].printNum || 0 == statusFuns[i].printNum) && 0 != strlen(statusFuns[i].name))
                {
                     WIRELESS_FMT_DEBUG("拨号端口%d 切换状态%s\n", AT_DEV_NUM(ifnum), statusFuns[i].name);
                }
                buflen = strlen(statusFuns[i].info);
                if(0 != buflen && (0 == statusFuns[i].showNum || stepcnt < statusFuns[i].showNum))
                {
                    /* 状态栏刷新 */
                    wireless_send_diagStatus(ifnum + 1, statusFuns[i].info, buflen, statusFuns[i].times);
                }
                
                status = statusFuns[i].func(ifnum, ptHandle->halName, stepcnt, &othercount, ptHandle, &delay);
                ptHandle->stepStatus = status;
                /* 状态连续运行状态 */
                stepcnt = status != statusFuns[i].status ? 0 : (stepcnt + 1);
                break;
            }
        }
        if(i >= statusNum)
        {
            WIRELESS_FMT_DEBUG("拨号端口%d 无效状态机 %d, 切换新状态 INIT\n", AT_DEV_NUM(ifnum), status);
            status = AT_INIT;
        }
    }
    return 10;
}

/**********************************************************************
* @name      : wireless_grpsnew_working
* @brief     ：内置协议栈grps工作, 不实现
* @param[in] ：uint8 commport               端口号
               uint8 ifnum                  网口编号
               WIRELESS_HANDLE_T *ptHandle  线程数据句柄
* @param[out]：
* @return    ：退出等待时间s
* @Create    : 王津剑
* @Date      ：2019-10-26
* @Update    : 2020-4-21
**********************************************************************/
int wireless_grpsnew_working(uint8 commport, uint8 ifnum, WIRELESS_HANDLE_T *ptHandle)
{
    WIRELESS_FMT_DEBUG("拨号端口%d 不支持内置协议栈, 切换到外置协议栈\n", AT_DEV_NUM(ifnum));

    return wireless_grps_working(commport, ifnum, ptHandle);
}


/**********************************************************************
* @name      : wireless_ppp0_init
* @brief     ：拨号0初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int wireless_ppp0_init(void *pPublic, void **ppPrivate)
{
    WIRELESS_DATA_T      *ptWireData = (WIRELESS_DATA_T*)pPublic;
    ptWireData->pppdata0.no = 0;
    *ppPrivate = &ptWireData->pppdata0;
    return 0;
}

/**********************************************************************
* @name      : wireless_ppp1_init
* @brief     ：拨号1初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int wireless_ppp1_init(void *pPublic, void **ppPrivate)
{
    WIRELESS_DATA_T      *ptWireData = (WIRELESS_DATA_T*)pPublic;
    ptWireData->pppdata1.no = 1;
    *ppPrivate = &ptWireData->pppdata1;
    return 0;
}

/**********************************************************************
* @name      : wireless_ppp_proc
* @brief     ：拨号处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2021-3-22
* @Update    :
**********************************************************************/
int wireless_ppp_proc(void *pArg, void *pPublic, void *pPrivate)
{
    WIRELESS_HANDLE_T *ptHandle = (WIRELESS_HANDLE_T *)pPrivate;
    int                delay = 0;

    if(FALSE != ptHandle->workFlag && WIRELESS_UNUSED_COMMPORT != ptHandle->commport)
    {
        ptHandle->dialState = PPP_DAIL_STATUS_OFFLINE;
        ptHandle->workAgain = FALSE;
        ptHandle->stepStatus = 0;
        ptHandle->sendfaild = 0;

        sleep(1);  //等待1s 数据同步

        /* 协议栈选择 */
        if(0 != ptHandle->ptNetPara->userAPN.diagnum)
        {
            ptHandle->bUserApn = TRUE;
        }
        
        /* 外置协议栈 自动 */
        delay = wireless_grps_working(ptHandle->commport, ptHandle->no, ptHandle);

        /* 数据清理 */
        ptHandle->stepStatus = 0;
        ptHandle->dialState = PPP_DAIL_STATUS_OFFLINE;
        wireless_set_pppstatus(ptHandle->no, 1);

        bzero(ptHandle->ptNetPara->ICCID, 21);/* SIM 卡 ICCID */
        ptHandle->ptNetPara->ipAddr = 0;      /* 网口IP地址 */
        ptHandle->ptNetPara->mask = 0;        /* 子网掩码 */   
        ptHandle->ptNetPara->getway = 0;      /* 网关 */  
        ptHandle->ptNetPara->operat = WIRELESS_OPT_NONE;

        sleep(1 + delay);
    }
    
    return 1000000;
}
