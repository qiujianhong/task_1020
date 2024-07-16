/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程主动上报线程实现
* @date：    2019-12-6
* @history： 
*********************************************************************
*/
#ifdef PRODUCT_ZCU_1
#include "baseCommon.h"
#include "appcomm.h"
#include "db_api.h"
#include "oopSecurity.h"
#include "lcmtrans.h"
#include "oopVarUpdate.h"
#include "framecomm.h"
#include "iniparser.h"
#include "hal.h"
#include "uart.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"
#include "task_id.h"
#include "AutoTask.h"
#include "area.h"

uint8 gAddr[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
char *g_uart_parity_desc[] = {"none", "odd", "even"};
uint8 g_lcmtrans_piid = 0; //事件上报帧序号
extern void current_tm_to_ooptm(OOP_DATETIME_S_T *poop_time);

#if DESC("串口操作",1)
/**********************************************************************
* @name      : uart_open
* @brief     ：打开串口,防重复打开
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_open(LC_UART_VDEV_T *pUart)
{
    int ret = ERR_OK;
    UART_DEVICE_T* dev = NULL;
    
    if((pUart == NULL) || (pUart->desc == NULL))
    {
        return ERR_PNULL;
    }

    dev = (UART_DEVICE_T*)hal_device_get(pUart->halname);
    if(NULL == dev)
    {
        return ERR_PNULL;    
    }

    ret = dev->uart_get_real_dev_name(dev, (uint8*)pUart->desc);
    if(ret <= 0)
    {
        LCMONIT_FMT_DEBUG("uart_get_real_dev_name failed. dev(%s), ret(%d)\n", pUart->halname, ret);
        return ERR_OPTION;    
    }

    // 打开设备
    ret = dev->uart_param_set(dev, pUart->param.baudRate, pUart->param.dataBits, pUart->param.stopBits, pUart->param.parity);
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("uart_param_set failed. dev(%s), ret(%d)\n", pUart->desc, ret);
        hal_device_release((HW_DEVICE *)dev);
        return ERR_INVAL;
    }

    pUart->fd = (int)dev;

    LCMONIT_FMT_DEBUG("open %s success: baudrate[%d]-databit[%d]-parity[%s]-stopbit[%d].\n", 
        pUart->halname, pUart->param.baudRate, pUart->param.dataBits, g_uart_parity_desc[pUart->param.parity], pUart->param.stopBits);  
    
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_open
* @brief     ：打开串口
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_close(LC_UART_VDEV_T *pUart)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T* )pUart->fd;

    if(NULL == pUart)
    {
        return ERR_PNULL;
    }
    
    if(dev != NULL)
    {
        hal_device_release((HW_DEVICE *)dev);

        dev = NULL;
    }

    pUart->fd = (int)dev;
    
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_write
* @brief     ：向串口写数据
* @param[in] ：uart    串口句柄
* @param[in] ：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_write(LC_UART_VDEV_T *pUart, uint8* buf, int len)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T* )pUart->fd;
    int ret = 0;  

    if(pUart== NULL)
    {
        return ERR_PNULL;
    }
    
    if(dev == NULL)  
    {  
        LCMONIT_FMT_DEBUG("dev is invalid\n");
        return ERR_INVALID;  
    }

    ret = dev->uart_data_send(dev, buf, len);    
    if(ret == 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &pUart->sendTm);
        pUart->sendCnt++;
        return ERR_OK;
    }
    else
    {        
        LCMONIT_FMT_DEBUG("%s send failed.\n", pUart->halname);
        return ERR_NORMAL;
    }
      
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_read
* @brief     ：从串口读数据
* @param[in] ：uart    串口句柄
* @param[out]：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：读到的数据长度
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_read(LC_UART_VDEV_T *pUart, uint8* buf, int len)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T* )pUart->fd;
    int fd = -1;
    int recv_len = 0;                                       
    int data_len = 0;                                       
    int count = 0;
    int delay = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;
    uint32 baud = pUart->param.baudRate;
    int frameus = pUart->frametimeout*1000;//微秒计算
    int octetus = pUart->bytetimeout*1000;//微秒计算
    
    if(dev == NULL)
    {
        return ERR_INVAL;
    }

    fd = dev->uart_get_handle(dev);
    if(fd < 0)
    {
        return ERR_INVAL;
    }

    //等待串口可读
    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = frameus;
    
    do 
    {
        n = select(fd+1, &set, NULL, NULL, &timeout); //超时返回0
    }while(n < 0);

    if(n == 0)
    {
        return 0;
    }

    /*串口接收数据超时时间按20个字节间隔时间来计算，300bps 发送一个字节需要36667us
    一个字节包括起始位、停止位、校验位、8位数据 共11位，则每串耗时11/baud秒*/
    if((baud%300 != 0 ) || baud == 0)
    {   
        delay = 11000000/9600;
    }
    else
    {   
        delay = 11000000/baud;
    }
    
    while(1)
    {
        recv_len = dev->uart_data_recv(dev, buf+data_len, len-data_len);

        // 收到字节后延时一个字节的时间再去接收
        if(recv_len > 0)   
        {
            data_len += recv_len;
            count = 0;
        }
        else
        { 
            //字节读取超时
            if(count > octetus)
            {
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                break;                    
            }
        }

        //长度已经超过缓冲区
        if(data_len > len)
        {
            clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
            pUart->recvCnt++;
            break;                    
        }

        usleep(delay);

        count += delay;
    }

    data_len = (data_len < 0) ? 0 : data_len;
    
    return data_len;
}

/**********************************************************************
* @name      : uart_transmit
* @brief     ：串口数据传输
* @param[in] ：void *pDev                   串口句柄
               int nBufLen                  最大接收长度
               int rsptime                  数据返回时间(20ms)
* @param[out]：
* @return    ：接收到数据的长度
* @Create    : 贺宁
* @Date      ：2020-3-13
* @Update    :
**********************************************************************/
int uart_transmit(LC_UART_VDEV_T *pUart, uint8* pSend, int sendLen, uint8* pRecv, int maxRecv)
{
    int    ret = 0;

    ret = uart_write(pUart, pSend, sendLen);
    if(ret != 0)
    {
        return 0;
    }
    
    return uart_read(pUart, pRecv, maxRecv);
}

#endif

int lcm_workstatus_store(DB_CLIENT clientid, OOP_WORKINFO_T *workinfo)
{
    int ret = 0;
    NOMAL_OAD_T nomalOAD = {0};
    MEMZERO(&workinfo, sizeof(OOP_WORKINFO_T));
    OOP_LOOPSTAT_T loopStat = {0};
    int i = 0;
    char phasename[3] = {'A','B','C'};

    for(i = 0; i < workinfo->nNum; i++)
    {
        loopStat.loopstat[i] = workinfo->item[i].curloopstat;

        LCMONIT_FMT_DEBUG("\r\n 相    序: %c\
                           \r\n 回路状态: %d\
                           \r\n 温    度: %d\
                           \r\n 最大频率: %d\
                           \r\n 最小频率: %d\
                           \r\n 工频电流: %d\
                           \r\n 阻抗模值: [%d %d %d]\
                           \r\n 阻抗角度: [%d %d %d]\
                           \r\n 阻抗频率: [%d %d %d]\n",
                            phasename[i],
                            workinfo->item[i].curloopstat,workinfo->item[i].temp,
                            workinfo->item[i].maxfreq, workinfo->item[i].minfreq,workinfo->item[i].curvalid,
                            workinfo->item[i].impval.impeval[0], workinfo->item[i].impval.impeval[1],workinfo->item[i].impval.impeval[2],
                            workinfo->item[i].impang.impeang[0], workinfo->item[i].impang.impeang[1],workinfo->item[i].impang.impeang[2],
                            workinfo->item[i].impfrq.impefreq[0], workinfo->item[i].impfrq.impefreq[1],workinfo->item[i].impfrq.impefreq[2]);
    }

    //工况信息
    nomalOAD.classtag = CLASS_DATA_NORMAL;
    nomalOAD.oad.value = 0x40400300;
    ret = db_write_nomal(clientid, &nomalOAD, (uint8*)workinfo, sizeof(OOP_WORKINFO_T));
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("lc_workstatus_store failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
        ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return ret;
    }

    //回路状态
    loopStat.nNum = 3;
    
    nomalOAD.oad.value = 0x20500200;
    ret = db_write_nomal(clientid, &nomalOAD, (uint8*)&loopStat, sizeof(loopStat));
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("lc_loopStatus_store failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
        ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return ret;
    }

    return ret;
}

int lcm_versioninfo_store(DB_CLIENT clientid, OOP_TASPCUSEMOD_T *version)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};

    LCMONIT_FMT_DEBUG("\r\n 模块ID            : %02x%02x%02x%02x%02x%02x%02x%02x\
                       \r\n 厂商代码          : %s\
                       \r\n 设备编号          : %s\
                       \r\n 运行区软件版本号  : %s\
                       \r\n 运行区软件发布日期: %04d-%02d-%02d\
                       \r\n 备份区软件版本号  : %s\
                       \r\n 备份区软件发布日期: %04d-%02d-%02d\
                       \r\n 配置容量信息码    : %s\
                       \r\n 通信协议版本号    : %s\
                       \r\n 硬件版本号        : %s\
                       \r\n 硬件发布日期      : %04d-%02d-%02d\n", 
                       version->modID.nValue[0],version->modID.nValue[1],version->modID.nValue[2],version->modID.nValue[3],
                       version->modID.nValue[4],version->modID.nValue[5],version->modID.nValue[6],version->modID.nValue[7],
                       version->manufcode.value, version->devcode, version->runsoftver.value, 
                       version->runsoftdate.year, version->runsoftdate.month, version->runsoftdate.mday,
                       version->baksoftver,
                       version->baksoftdate.year, version->baksoftdate.month, version->baksoftdate.mday,
                       version->cfgcapinfo.value, version->protover.value, version->hardver.value,
                       version->harddate.year, version->harddate.month, version->harddate.mday);

    //版本信息
    nomalOAD.classtag = CLASS_DATA_NORMAL;
    nomalOAD.oad.value = 0x40400200;
    ret = db_write_nomal(clientid, &nomalOAD, (uint8*)version, sizeof(OOP_TASPCUSEMOD_T));
    if(ret != 0)
    {
        LCMONIT_FMT_DEBUG("lc_workstatus_store failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
        ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return ret;
    }
 
    return ret;
}

#if DESC("报文操作",1)

int lcm_taworkstate_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    uint32 sOffset = 0;
    uint8  i = 0, j = 0;
    OOP_WORKINFO_T workinfo;
    OOP_DAR_E dar = DATA_SUCCESS;

    memcpy(&workinfo, stdata, sizeof(OOP_WORKINFO_T));
        
    LCMONIT_BUF_DEBUG(apdu, apduLen, "lc plaintext apdu.\n");

    if(apdu[sOffset++] != DT_ARRAY)
    {
        LCMONIT_FMT_DEBUG("类型错误 %d != 1\n", apdu[sOffset-1]);
        return DATA_TYPE_UNMATCHED;
    }

    //数组成员数
    workinfo.nNum = apdu[sOffset++];

    for(i = 0; i < workinfo.nNum; i++)
    {
        if(apdu[sOffset++] != DT_STRUCTURE)
        {
            LCMONIT_FMT_DEBUG("类型错误 %d != 2\n", apdu[sOffset-1]);
            return DATA_TYPE_UNMATCHED;
        }

        if(apdu[sOffset++] != 8)
        {
            LCMONIT_FMT_DEBUG("成员数错误 %d != 8\n", apdu[sOffset-1]);
            return DATA_TYPE_UNMATCHED;
        }

        //回路状态
        dar = basic_buf_to_data(E_ENUM, apdu, apduLen-sOffset, &sOffset, &workinfo.item[i].curloopstat, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            LCMONIT_BUF_DEBUG(&apdu[sOffset], apduLen-sOffset, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //温度
        dar = basic_buf_to_data(E_DOUBLE_LONG, apdu, apduLen-sOffset, &sOffset, &workinfo.item[i].temp, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            LCMONIT_BUF_DEBUG(&apdu[sOffset], apduLen-sOffset, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //频率最大值
        dar = basic_buf_to_data(E_DOUBLE_LONG, apdu, apduLen-sOffset, &sOffset, &workinfo.item[i].maxfreq, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            LCMONIT_BUF_DEBUG(&apdu[sOffset], apduLen-sOffset, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //频率最小值
        dar = basic_buf_to_data(E_DOUBLE_LONG, apdu, apduLen-sOffset, &sOffset, &workinfo.item[i].minfreq, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            LCMONIT_BUF_DEBUG(&apdu[sOffset], apduLen-sOffset, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //工频电流有效值
        dar = basic_buf_to_data(E_DOUBLE_LONG, apdu, apduLen-sOffset, &sOffset, &workinfo.item[i].curvalid, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            LCMONIT_BUF_DEBUG(&apdu[sOffset], apduLen-sOffset, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //阻抗模值
        if(apdu[sOffset++] != DT_ARRAY)
        {
            LCMONIT_FMT_DEBUG("类型错误 %d != 1\n", apdu[sOffset-1]);
            return DATA_TYPE_UNMATCHED;
        }

        workinfo.item[i].impval.nNum = apdu[sOffset++];

        for(j = 0; i < workinfo.item[i].impval.nNum; j++)
        {
            dar = basic_buf_to_data(E_DOUBLE_LONG, apdu, apduLen-sOffset, &sOffset, &workinfo.item[i].impval.impeval[j], sizeof(uint32));
            if(dar != DATA_SUCCESS)
            {
                LCMONIT_BUF_DEBUG(&apdu[sOffset], apduLen-sOffset, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            }
        }

        //阻抗角度
        if(apdu[sOffset++] != DT_ARRAY)
        {
            LCMONIT_FMT_DEBUG("类型错误 %d != 1\n", apdu[sOffset-1]);
            return DATA_TYPE_UNMATCHED;
        }
        
        workinfo.item[i].impang.nNum = apdu[sOffset++];
        
        for(j = 0; i < workinfo.item[i].impang.nNum; j++)
        {
            dar = basic_buf_to_data(E_DOUBLE_LONG, apdu, apduLen-sOffset, &sOffset, &workinfo.item[i].impang.impeang[j], sizeof(uint32));
            if(dar != DATA_SUCCESS)
            {
                LCMONIT_BUF_DEBUG(&apdu[sOffset], apduLen-sOffset, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            }
        }

        //阻抗频率
        if(apdu[sOffset++] != DT_ARRAY)
        {
            LCMONIT_FMT_DEBUG("类型错误 %d != 1\n", apdu[sOffset-1]);
            return DATA_TYPE_UNMATCHED;
        }
        
        workinfo.item[i].impfrq.nNum = apdu[sOffset++];
        
        for(j = 0; i < workinfo.item[i].impfrq.nNum; j++)
        {
            dar = basic_buf_to_data(E_DOUBLE_LONG, apdu, apduLen-sOffset, &sOffset, &workinfo.item[i].impfrq.impefreq[j], sizeof(uint32));
            if(dar != DATA_SUCCESS)
            {
                LCMONIT_BUF_DEBUG(&apdu[sOffset], apduLen-sOffset, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            }
        }
    }
    
    memcpy(&stdata, &workinfo, sizeof(OOP_WORKINFO_T));

    return ERR_OK;
}

int lcm_taversion_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata)
{
    uint32 sOffset = 0;
    OOP_TASPCUSEMOD_T ctVersion;

    memcpy(&ctVersion, stdata, sizeof(OOP_TASPCUSEMOD_T));
    
    LCMONIT_BUF_DEBUG(apdu, apduLen, "lc version plaintext apdu.\n");

    //模块型号
    memcpy_r(&ctVersion.modID.nValue, &apdu[sOffset], 8);
    sOffset += 8;

    //厂商代码
    memcpy_r(&ctVersion.manufcode.value, &apdu[sOffset], 4);
    sOffset += 4;

    //设备编号
    memcpy_r(&ctVersion.devcode.value, &apdu[sOffset], 8);
    sOffset += 8;

    //运行区软件版本号
    memcpy_r(&ctVersion.runsoftver.value, &apdu[sOffset], 4);
    sOffset += 4;

    //运行区软件发布日期
    memcpy_r(&ctVersion.runsoftdate.year, &apdu[sOffset], 2);
    sOffset += 2;

    ctVersion.runsoftdate.month = apdu[sOffset]++;
    ctVersion.runsoftdate.mday = apdu[sOffset]++;

    //备份区软件版本号
    memcpy_r(&ctVersion.baksoftver.value, &apdu[sOffset], 4);
    sOffset += 4;

    //备份区软件发布日期
    memcpy_r(&ctVersion.baksoftdate.year, &apdu[sOffset], 2);
    sOffset += 2;

    ctVersion.baksoftdate.month = apdu[sOffset]++;
    ctVersion.baksoftdate.mday = apdu[sOffset]++;

    //配置容量信息码
    memcpy_r(&ctVersion.cfgcapinfo.value, &apdu[sOffset], 11);
    sOffset += 11;

    //通信协议版本号
    memcpy_r(&ctVersion.protover.value, &apdu[sOffset], 4);
    sOffset += 4;

    //硬件版本号
    memcpy_r(&ctVersion.hardver.value, &apdu[sOffset], 4);
    sOffset += 4;

    //硬件发布日期
    memcpy_r(&ctVersion.harddate.year, &apdu[sOffset], 2);
    sOffset += 2;

    ctVersion.harddate.month = apdu[sOffset]++;
    ctVersion.harddate.mday = apdu[sOffset]++;
    
    memcpy(stdata, &ctVersion, sizeof(OOP_TASPCUSEMOD_T));
    
    return ERR_OK;
}

LC_MODPARSE_T g_ctalgParse[] = 
{
    {LCM_OAD_TAVERSION,   lcm_taversion_parse},
    {LCM_OAD_WORKSTATUS,  lcm_taworkstate_parse},
};

uint32 g_ctalgParse_Max = sizeof(g_ctalgParse)/sizeof(LC_MODPARSE_T);

/**********************************************************************
* @name      : lcm_modmsg_parse
* @brief     ：算法板协议解析
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
**********************************************************************/
int lcm_tamodmsg_parse(DB_CLIENT clientid, uint8 *recvBuf, uint16 buflen, void *stdata)
{
    int            ret = 0;
    uint16         offset = 0;
    FRAME_HEAD_T   frameInfo = {0};
    uint32         di = 0;
    uint8          i = 0;
    int            tempret = 0;
    uint32 sOffset = 0;
        
    //找到帧起始位
    while(offset < buflen)
    {
        if(VF_BEGIN == recvBuf[offset])
        {
            tempret = app_get_frame(recvBuf + offset, buflen - offset, &frameInfo, TRUE);
            if(tempret != 0)
            {
                LCMONIT_BUF_DEBUG(recvBuf + offset, buflen - offset, "lcm_frame_decap failed, tempret(%d)\n", tempret);
            }
            break;
        }
        offset++;
    }

    if(frameInfo.apduLen == 0)
    {
        LCMONIT_BUF_DEBUG(frameInfo.pApdu, sOffset, "check null pointer.\n");
        return ERR_PNULL;
    }

    //功能单一，不考虑多帧
    memcpy(&di, &frameInfo.pApdu[sOffset], sizeof(uint32));
    sOffset += sizeof(uint32);

    for(i = 0; i < g_ctalgParse_Max; i++)
    {
        if((g_ctalgParse[i].di == di)&&(g_ctalgParse[i].apduGet != NULL))
        {
            ret = g_ctalgParse[i].apduGet(clientid, &frameInfo.pApdu[sOffset], frameInfo.apduLen-sOffset, stdata);
            break;
        }
    }

    if(i == g_ctalgParse_Max)
    {
        ret = ERR_OPTION;
    }
    
    return ret;
}

/**********************************************************************
* @name      : lc_make_frame
* @brief     ：模组接口协议组帧
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
**********************************************************************/
//uint16 lcm_tamodmsg_make(uint8 *apdu, uint16 apdulen, uint8 *sendBuf, uint16 buflen)
//{
//    FRAME_HEAD_T frameInfo = {0};
////    uint8 *pAddr = NULL;
////    uint8 addrLen = 0;
//
//    frameInfo.dir = 0;
//    frameInfo.prm = 1;
//    frameInfo.funCode = 3;
//    frameInfo.apduLen = apdulen;
//    frameInfo.pApdu = apdu;
//
//    //addrLen = app_get_localaddr(&pAddr);
//
//    //填写通配地址
//    frameInfo.sAddr.len = 6;
//    memcpy(frameInfo.sAddr.addr, gAddr, 6);
//    frameInfo.sAddr.type = 1;
//    frameInfo.sAddr.logicAddr = 2;
//
//    frameInfo.headlen = APUD_COUNT_FRAME_LEN(frameInfo.sAddr.len, frameInfo.sAddr.logicAddr);
//    frameInfo.msglen = frameInfo.headlen + frameInfo.apduLen + APP_698_CS_REGIN_SIZE;
//    
//
//    return app_frame_to_package(&frameInfo, sendBuf, buflen, TRUE);
//}

/**********************************************************************
* @name      : lc_make_frame
* @brief     ：模组接口协议组帧
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
**********************************************************************/
uint16 lcm_tamodmsg_make(uint8 logicaddr, uint8* comaddr, uint8 *apdu, uint16 apdulen, uint8 *sendBuf, uint16 buflen)
{
    FRAME_HEAD_T frameInfo = {0};

    frameInfo.dir = 0;
    frameInfo.prm = 1;
    frameInfo.funCode = 3;
    frameInfo.apduLen = apdulen;
    frameInfo.pApdu = apdu;

    frameInfo.sAddr.logicAddr = logicaddr;
    frameInfo.sAddr.len = 6;

    //填写通配地址
    if(comaddr == NULL)
    {
        memcpy(frameInfo.sAddr.addr, gAddr, 6);
        frameInfo.sAddr.type = 1;
    }
    else
    {
        memcpy(frameInfo.sAddr.addr, comaddr, 6);
        frameInfo.sAddr.type = 0;
    }
    
    frameInfo.headlen = APUD_COUNT_FRAME_LEN(frameInfo.sAddr.len, frameInfo.sAddr.logicAddr);
    frameInfo.msglen = frameInfo.headlen + frameInfo.apduLen + APP_698_CS_REGIN_SIZE;
    
    return app_frame_to_package(&frameInfo, sendBuf, buflen, TRUE);
}

#endif

#if DESC("初始配置",1)

/**
*********************************************************************
* @brief：     lc_version_get
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
int lcm_taversion_get(void *pDev, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //发送报文长度
    uint8 sRecvbuf[1024] = {0};//单次接收
    uint16 sRecvLen = 0; //单次接收长度
    OOP_TASPCUSEMOD_T taVersion = {0};
    LC_UART_VDEV_T *tmpUart = (LC_UART_VDEV_T *)pDev;
    uint32 useTime = 0;
    static uint16 retryCnt = 0;
    uint16 offset = 0;
    uint8 tmpbuf[128] = {0};
    uint32 oad = LCM_OAD_TAVERSION;
    uint8 *pAddr = NULL;
    uint8 addrLen = 0;

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return ERR_PNULL;
    }

    addrLen = app_get_localaddr(&pAddr);
    if(addrLen == 0)
    {
        LCMONIT_FMT_DEBUG("获取终端地址失败\n");
        return ERR_NORMAL;
    }

    tmpbuf[offset++] = SERVERID_GETREQUEST;
    tmpbuf[offset++] = REQ_GET_NORMAL;
    tmpbuf[offset++] = RPT_ADD_VALUE_CLEAR(g_lcmtrans_piid, 63, 1);

    //构造数据域
    memcpy_r(&tmpbuf[offset], &oad, sizeof(uint32));
    offset += sizeof(uint32);

    //无时间标签
    tmpbuf[offset++] = 0x00;
    
    sendlen = lcm_tamodmsg_make(2, pAddr, tmpbuf, offset, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "读取TA模块版本信息:");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        retryCnt = 0;
        ret = lcm_tamodmsg_parse(clientid, sRecvbuf, sRecvLen, &taVersion);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
            return ERR_NORMAL;
        }
    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }

    useTime = (tmpUart->recvTm.tv_sec - tmpUart->sendTm.tv_sec)*1000 + (tmpUart->recvTm.tv_nsec - tmpUart->sendTm.tv_nsec)/1000000;

    LCMONIT_FMT_DEBUG("comm use: %d ms, success rate: recv-%d/send-%d = %.2f%%.\n", 
        useTime, tmpUart->recvCnt, tmpUart->sendCnt, tmpUart->sendCnt==0 ? 0: tmpUart->recvCnt*100.0/tmpUart->sendCnt);

    //版本信息存储
    lcm_versioninfo_store(clientid, &taVersion); 

    return ret;
}

/**
*********************************************************************
* @brief：     lc_workstate_get
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
int lcm_taworkstate_get(void *pDev, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //发送报文长度
    uint8 sRecvbuf[1024] = {0};//单次接收
    uint16 sRecvLen = 0; //单次接收长度
    OOP_WORKINFO_T workinfo = {0};
    static uint16 retryCnt = 0;
    LC_UART_VDEV_T *tmpUart = (LC_UART_VDEV_T *)pDev;
    uint32 useTime = 0;
    uint16 offset = 0;
    uint8 tmpbuf[128] = {0};
    uint32 oad = LCM_OAD_WORKSTATUS;
    uint8 *pAddr = NULL;
    uint8 addrLen = 0;

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return ERR_PNULL;
    }
    
    addrLen = app_get_localaddr(&pAddr);
    if(addrLen == 0)
    {
        LCMONIT_FMT_DEBUG("获取终端地址失败\n");
        return ERR_NORMAL;
    }

    tmpbuf[offset++] = SERVERID_GETREQUEST;
    tmpbuf[offset++] = REQ_GET_NORMAL;
    tmpbuf[offset++] = RPT_ADD_VALUE_CLEAR(g_lcmtrans_piid, 63, 1);
    
    //构造数据域
    memcpy_r(&tmpbuf[offset], &oad, sizeof(uint32));
    offset += sizeof(uint32);

    //无时间标签
    tmpbuf[offset++] = 0x00;
    
    sendlen = lcm_tamodmsg_make(2, pAddr, tmpbuf, offset, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "读取TA模块工况信息:");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {
        retryCnt = 0;
        ret = lcm_tamodmsg_parse(clientid, sRecvbuf, sRecvLen, &workinfo);
        if(ret != 0)
        {
            LCMONIT_BUF_DEBUG(sRecvbuf, sRecvLen, "lc_modmsg_parse failed,ret(%d).\n", ret);
            return ERR_NORMAL;
        }
    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }

    useTime = (tmpUart->recvTm.tv_sec - tmpUart->sendTm.tv_sec)*1000 + (tmpUart->recvTm.tv_nsec - tmpUart->sendTm.tv_nsec)/1000000;

    LCMONIT_FMT_DEBUG("comm use: %d ms, success rate: recv-%d/send-%d = %.2f%%.\n", 
        useTime, tmpUart->recvCnt, tmpUart->sendCnt, tmpUart->sendCnt==0 ? 0: tmpUart->recvCnt*100.0/tmpUart->sendCnt);

    //工况信息存储
    lcm_workstatus_store(clientid, &workinfo); 

    return ret;
}

/**
*********************************************************************
* @brief：     设置TA模块地址
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
int lcm_taaddress_set(void *pDev, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //发送报文长度
    uint8 sRecvbuf[1024] = {0};//单次接收
    uint16 sRecvLen = 0; //单次接收长度
    LC_UART_VDEV_T *tmpUart = (LC_UART_VDEV_T *)pDev;
    uint32 useTime = 0;
    static uint16 retryCnt = 0;
    uint16 offset = 0;
    uint8 tmpbuf[128] = {0};
    uint32 oad = 0x40010200;
    uint8 *pAddr = NULL;
    uint8 addrLen = 0;

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return ERR_PNULL;
    }

    addrLen = app_get_localaddr(&pAddr);
    if(addrLen == 0)
    {
        LCMONIT_FMT_DEBUG("获取终端地址失败\n");
        return ERR_NORMAL;
    }

    tmpbuf[offset++] = SERVERID_SETREQUEST;
    tmpbuf[offset++] = REQ_SET_NORMAL;
    tmpbuf[offset++] = RPT_ADD_VALUE_CLEAR(g_lcmtrans_piid, 63, 1);

    //构造数据域
    memcpy_r(&tmpbuf[offset], &oad, sizeof(uint32));
    offset += sizeof(uint32);

    tmpbuf[offset++] = DT_OCTET_STRING;
    tmpbuf[offset++] = addrLen;

    memcpy(&tmpbuf[offset], pAddr, addrLen);
    offset += 6;

    //无时间标签
    tmpbuf[offset++] = 0x00;
    
    sendlen = lcm_tamodmsg_make(0, NULL, tmpbuf, offset, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "设置TA模块设备地址:");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {

    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }

    useTime = (tmpUart->recvTm.tv_sec - tmpUart->sendTm.tv_sec)*1000 + (tmpUart->recvTm.tv_nsec - tmpUart->sendTm.tv_nsec)/1000000;

    LCMONIT_FMT_DEBUG("comm use: %d ms, success rate: recv-%d/send-%d = %.2f%%.\n", 
        useTime, tmpUart->recvCnt, tmpUart->sendCnt, tmpUart->sendCnt==0 ? 0: tmpUart->recvCnt*100.0/tmpUart->sendCnt);


    return ret;
}

/**
*********************************************************************
* @brief：     设置TA模块时间
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
int lcm_tadatetime_set(void *pDev, DB_CLIENT clientid, int socketfd)
{
    int ret = 0;
    uint8 sendbuf[128] = {0};
    uint16 sendlen = 0;   //发送报文长度
    uint8 sRecvbuf[1024] = {0};//单次接收
    uint16 sRecvLen = 0; //单次接收长度
    LC_UART_VDEV_T *tmpUart = (LC_UART_VDEV_T *)pDev;
    uint32 useTime = 0;
    static uint16 retryCnt = 0;
    uint16 offset = 0;
    uint8 tmpbuf[128] = {0};
    uint32 oad = 0x40000200;
    uint8 *pAddr = NULL;
    uint8 addrLen = 0;
    OOP_DATETIME_S_T datetime = {0};

    if(pDev == NULL)
    {
        LCMONIT_FMT_DEBUG("pDev is NULL\n");
        return ERR_PNULL;
    }

    addrLen = app_get_localaddr(&pAddr);
    if(addrLen == 0)
    {
        LCMONIT_FMT_DEBUG("获取终端地址失败\n");
        return ERR_NORMAL;
    }

    current_tm_to_ooptm(&datetime);

    tmpbuf[offset++] = SERVERID_SETREQUEST;
    tmpbuf[offset++] = REQ_SET_NORMAL;
    tmpbuf[offset++] = RPT_ADD_VALUE_CLEAR(g_lcmtrans_piid, 63, 1);

    //构造数据域
    memcpy_r(&tmpbuf[offset], &oad, sizeof(uint32));
    offset += sizeof(uint32);

    tmpbuf[offset++] = DT_DATETIME_S;
    memcpy_r(&tmpbuf[offset], &datetime.year, sizeof(uint16));
    offset += sizeof(uint16);
    tmpbuf[offset++] = datetime.month;
    tmpbuf[offset++] = datetime.day;
    tmpbuf[offset++] = datetime.hour;
    tmpbuf[offset++] = datetime.minute;
    tmpbuf[offset++] = datetime.second;

    //无时间标签
    tmpbuf[offset++] = 0x00;
    
    sendlen = lcm_tamodmsg_make(0, pAddr, tmpbuf, offset, sendbuf, sizeof(sendbuf));

    LCMONIT_BUF_DEBUG(sendbuf, sendlen, "设置TA模块日期时间:");

    sRecvLen = uart_transmit(pDev, sendbuf, sendlen, sRecvbuf, sizeof(sRecvbuf));
    if(sRecvLen != 0)
    {

    }
    else
    {
        if(++retryCnt >= LCM_SEND_RETRY)
        {
            retryCnt = 0;
            return ERR_INVALID;
        }
    }

    useTime = (tmpUart->recvTm.tv_sec - tmpUart->sendTm.tv_sec)*1000 + (tmpUart->recvTm.tv_nsec - tmpUart->sendTm.tv_nsec)/1000000;

    LCMONIT_FMT_DEBUG("comm use: %d ms, success rate: recv-%d/send-%d = %.2f%%.\n", 
        useTime, tmpUart->recvCnt, tmpUart->sendCnt, tmpUart->sendCnt==0 ? 0: tmpUart->recvCnt*100.0/tmpUart->sendCnt);

    return ret;
}


#endif

LC_UART_VDEV_T g_lcmdev = {0};

/**********************************************************************
* @name      : app_lcm_trans_work
* @brief     ：巡检仪通信任务
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2022-07-30
* @Update    :
**********************************************************************/
void *app_lcm_trans_work(void *pArg)
{
    LCMTRANS_THREAD_T *ptMsgData = (LCMTRANS_THREAD_T *)pArg;
    int ret = 0;
    char realname[64] = {0};
    void *pDev = &g_lcmdev;
    LC_UART_VDEV_T *pUart = (LC_UART_VDEV_T*)pDev;
    UART_DEVICE_T* dev = NULL;
    uint8 chID = 0; //上报通道ID
    uint8 sRecvbuf[1024] = {0};//单次接收
    uint16 sRecvLen = 0; //单次接收长度
    uint16         offset = 0;
    FRAME_HEAD_T   frameInfo = {0};
    int            tempret = 0;
    OOP_OAD_U OAD = {0};

    ptMsgData->clientid = db_client_create(DB_YC_AGENT, DB_CLIENT_LCMTRANS);
    if(ptMsgData->clientid <= 0)
    {
        LCMONIT_FMT_DEBUG("lc_monitor_init db db_client_create error!\n");
        return 0;
    }
    
    sprintf(pUart->halname, "%s", UART_DEVICE_LCM_STR);
    pUart->desc = realname;
    pUart->bytetimeout = 12;
    pUart->frametimeout = 4000;
    pUart->param.baudRate = 57600;
    pUart->param.dataBits = 8;    
    pUart->param.stopBits = 1;    
    pUart->param.parity = UART_PARITY_EVEN;
   
    ptMsgData->status = LCM_INIT;

    while(1)
    {
        *ptMsgData->pRunFlag = 0;

        ptMsgData->curtime = get_time(TIME_MONOTONIC);

        //检测串口是否异常
        dev = (UART_DEVICE_T* )pUart->fd;

        if((dev != NULL) && (dev->uart_get_status(dev)< 0))
        {
            ptMsgData->status = LCM_ERRO;
            LCMONIT_FMT_DEBUG("%s状态异常\n", pUart->halname);
        }

        switch(ptMsgData->status)
        {
            case LCM_INIT:
            {
                ret = uart_open(pDev);
                if(ret == ERR_OK)
                {
                    ptMsgData->status = LCM_CONF;
                }
                sleep(1);
            }break;
        
            case LCM_ERRO:    //重新打开巡检仪串口
            {
                LCMONIT_FMT_TRACE("LCM_OPEN.\n");
                uart_close(pDev);

                if(0 == access(pUart->desc, F_OK))
                {
                    LCMONIT_FMT_DEBUG("%s is exist\n", pUart->desc);

                    sleep_no_interrupt(2);

                    ret = uart_open(pDev);
                    if(ret == ERR_OK)
                    {
                        ptMsgData->status = LCM_CONF;
                    }
                }
            }break;

            case LCM_CONF:  //对巡检仪设置地址 对时，读取版本号
            {
                LCMONIT_FMT_TRACE("LCM_INIT.\n");

                ret |= lcm_taaddress_set(pDev, ptMsgData->clientid, 0);
                ret |= lcm_tadatetime_set(pDev, ptMsgData->clientid, 0);
                ret |= lcm_taversion_get(pDev, ptMsgData->clientid, 0);
                if(ret != ERR_OK)
                {
                    if(ret == ERR_INVALID)
                    {
                        ptMsgData->status = LCM_ERRO;
                    }
                }
                else
                {
                    ptMsgData->status = LCM_TRAN;
                }
            }break;
                
            case LCM_TRAN:  //透传
            {
                LCMONIT_FMT_TRACE("LCM_TRAN.\n");

                //读取内容直接上报
                sRecvLen = uart_read(pDev, sRecvbuf, 1024);
                if(sRecvLen > 0)
                {
                    //找到帧起始位
                    while(offset < sRecvLen)
                    {
                        if(VF_BEGIN == sRecvbuf[offset])
                        {
                            tempret = app_get_frame(sRecvbuf + offset, sRecvLen - offset, &frameInfo, TRUE);
                            if(tempret != 0)
                            {
                                LCMONIT_BUF_DEBUG(sRecvbuf + offset, sRecvLen - offset, "lcm_frame_decap failed, tempret(%d)\n", tempret);
                            }
                            break;
                        }
                        offset++;
                    }

                    if(frameInfo.apduLen == 0)
                    {
                        LCMONIT_BUF_DEBUG(frameInfo.pApdu, offset, "check null pointer.\n");
                        return 0;
                    }

                    memcpy_r(&OAD, frameInfo.pApdu+3, sizeof(OOP_OAD_U));

                    //设置时间，地址，参数和数据初始化是终端复制一份给巡检仪的，回码不给主站
                    if((OAD.nObjID==0x4300) || (OAD.nObjID==0x4001) || (OAD.value==0x40000200))
                    {
                        return 0;
                    }
                    
                    if(!rpt_channel_search(&chID))
                    {
                        return 0;
                    }
                    
                    ret = send_emergency_ipcmsg(chID, 0, sRecvbuf, sRecvLen);
                    if(ret != 0)
                    {
                        LCMONIT_FMT_DEBUG("send_emergency_ipcmsg failed. id(%d)\n", chID);
                        return 0;
                    }
                }

                //定时读取回路状态以供显示
                if(ptMsgData->curtime %5 == 0)
                {
                    ret = lcm_taworkstate_get(pDev, ptMsgData->clientid, 0);
                    if(ret != ERR_OK)
                    {
                        if(ret == ERR_INVALID)
                        {
                            ptMsgData->status = LCM_ERRO;
                        }
                    }
                }
            }break;
                
            default:break;
        }

        usleep(200);

        ptMsgData->lsttime = ptMsgData->curtime;
    }    

    return 0;
}

int lcm_esam_frame_decap(uint8 *pMsg, uint32 msgLen, LCM_ESAMFRAME_HEAD_T *pFrame)
{
    int offset = 0;
    uint8 csvalue = 0;
    int i = 0;

    offset++;

    //长度
    memcpy_r(&pFrame->msglen, &pMsg[offset], sizeof(uint16));
    offset += sizeof(uint16);

    //ESAM数据区
    pFrame->pApdu = &pMsg[offset];
    pFrame->apduLen = pFrame->msglen - 5;

    if(VF_END != pMsg[pFrame->msglen-1])
    {
        pFrame->msglen = 0;
        return -1;
    }

    for(i = 0; i < pFrame->apduLen; i++)
    {
        csvalue += pMsg[offset+i];
    }

    //检查校验位
    if(csvalue != pMsg[pFrame->msglen-2])
    {
        pFrame->msglen = 0;
        return -2;
    }

    return 0;
}

/**********************************************************************
* @name      : app_ljm_trans_work
* @brief     ：巡检仪加解密通信任务
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2022-07-30
* @Update    :
**********************************************************************/
void *app_ljm_trans_work(void *pArg)
{
    LCMTRANS_THREAD_T *ptMsgData = (LCMTRANS_THREAD_T *)pArg;
    LC_UART_VDEV_T ljmdev = {0};
    int ret = 0;
    char realname[64] = {0};
    void *pDev = &ljmdev;
    LC_UART_VDEV_T *pUart = (LC_UART_VDEV_T*)pDev;
    UART_DEVICE_T* dev = NULL;
    uint8 sRecvbuf[1024] = {0};//单次接收
    uint16 sRecvLen = 0; //单次接收长度
    uint16         offset = 0;
    LCM_ESAMFRAME_HEAD_T frameInfo = {0};
    int            tempret = 0;
    uint8 eRecvbuf[1024] = {0};//ESAM接收
    uint16 eRecvLen = 0; //ESAM接收长度
    
    sprintf(pUart->halname, "%s", UART_DEVICE_LJM_STR);
    pUart->desc = realname;
    pUart->bytetimeout = 12;
    pUart->frametimeout = 4000;
    pUart->param.baudRate = 57600;
    pUart->param.dataBits = 8;    
    pUart->param.stopBits = 1;    
    pUart->param.parity = UART_PARITY_EVEN;

    ptMsgData->status = LCM_INIT;

    while(1)
    {
        *ptMsgData->pRunFlag = 0;

        ptMsgData->curtime = get_time(TIME_MONOTONIC);

        //检测串口是否异常
        dev = (UART_DEVICE_T* )pUart->fd;

        if((dev != NULL) && (dev->uart_get_status(dev)< 0))
        {
            ptMsgData->status = LCM_ERRO;
            LCMONIT_FMT_DEBUG("%s状态异常\n", pUart->halname);
        }

        switch(ptMsgData->status)
        {
            case LCM_INIT:
            {
                ret = uart_open(pDev);
                if(ret == ERR_OK)
                {
                    ptMsgData->status = LCM_TRAN;
                }
                sleep(1);
            }break;
            
            case LCM_ERRO:    //重新打开巡检仪串口
            {
                LCMONIT_FMT_TRACE("LCM_OPEN.\n");
                uart_close(pDev);

                if(0 == access(pUart->desc, F_OK))
                {
                    LCMONIT_FMT_DEBUG("%s is exist\n", pUart->desc);

                    sleep_no_interrupt(2);

                    ret = uart_open(pDev);
                    if(ret == ERR_OK)
                    {
                        ptMsgData->status = LCM_TRAN;
                    }
                }
            }break;

                
            case LCM_TRAN:  //透传
            {
                LCMONIT_FMT_TRACE("LCM_TRAN.\n");

                //读取内容直接上报
                sRecvLen = uart_read(pDev, sRecvbuf, 1024);
                if(sRecvLen > 0)
                {
                    //找到帧起始位
                    while(offset < sRecvLen)
                    {
                        if(VF_BEGIN == sRecvbuf[offset])
                        {
                            tempret = lcm_esam_frame_decap(sRecvbuf + offset, sRecvLen - offset, &frameInfo);
                            if(tempret != 0)
                            {
                                LCMONIT_BUF_DEBUG(sRecvbuf + offset, sRecvLen - offset, "lcm_frame_decap failed, tempret(%d)\n", tempret);
                            }
                            break;
                        }
                        offset++;
                    }

                    if(frameInfo.msglen == 0)
                    {
                        break;
                    }
                    
                    //发送给ESAM
                    ret = TESAM_RcvSndDataAndCheck(frameInfo.pApdu, frameInfo.apduLen, eRecvbuf, &eRecvLen, FALSE);
                    if(ret != 0)
                    {
                        LCMONIT_FMT_DEBUG("转发给ESAM失败 返回 %d\n", ret);
                        break;
                    }

                    //ESAM回码转发给巡检仪
                    ret = uart_write(pDev, eRecvbuf, eRecvLen);
                    if(ret != 0)
                    {
                        LCMONIT_FMT_DEBUG("ESAM信息转发给巡检仪失败 返回 %d\n", ret);
                        break;
                    }
                }
            }break;
                
            default:break;
        }
        

        usleep(200);

        ptMsgData->lsttime = ptMsgData->curtime;
    }     

    return 0;
}

#endif
