#include "OopAll.h"
#include "hal.h"
#include "esam.h"
#include "framecomm.h"
#include "AppCommon.h"
#include "Common698.h"
#include "oopVarUpdate.h"
#include "security_msg.h"
#include "task_msg.h"
#include "gen_auth_data.h"
#include "storage.h"
#include "sm3sum.h"

#define MAX_READMTR_NUM			  32
#define ESAM_BUFF_LEN             4096        //ESAM交互缓冲区长度

/* 临时使用的appid和校验 */
uint8 g_Esam_App_id[24] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                           0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
                           0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,};

uint8 g_Esam_AC_test[16] = {0x14, 0x16, 0x01, 0xA1, 0x3C, 0xE5, 0x70, 0xFF,
                            0xB2, 0xD4, 0xF6, 0x41, 0x61, 0x02, 0x02, 0x11};

int    g_UDPEASMid = 0;
pthread_mutex_t g_fUdpEsamLock;

static  SecurityData_t s_SecurityData;
static  ReadMtrPara_t  s_ReadMtrPara[MAX_READMTR_NUM];
uint8 g_TESAMInfoBuff[2048] = {0};
uint16 g_TESAMInfoLen = 0;

static BOOL            isInit = FALSE;

void DAEsamSendvPrint(uint8 *pBuf,int len);

void ReverseBuf_TESAM(uint8 *buf, uint16 len)
{
#if(0)
    uint16  i,j;
    uint8 temp;

    if(len <= 0)
    {
        return;
    }

    for(i=0,j=len-1; i<j; ++i,--j)
    {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
    }
    return;
#endif
}

/*
*********************************************************************
* @name      : esam_device_set_lock
* @brief     ：esam尝试加锁
* @param[in] ：gTesamDev
               val 0 解锁 1 加锁
* @param[out]：
* @return    ：
*********************************************************************
*/
int esam_device_set_lock(ESAM_DEVICE_T *gTesamDev, int val)
{
    int i = 0;

    /* 尝试加解锁 */
    while (ERR_OK != gTesamDev->esam_set_lock(gTesamDev, val))
    {
        if (i++ > 100)
        {
            return -1;
        }
        usleep(10000);
    }

    return 0;
}

/*
*********************************************************************
* @name      : esam_power_open
* @brief     ：esam打开电源
* @param[in] ：
* @param[out]：
* @return    ：
*********************************************************************
*/
int esam_power_open(void)
{
    ESAM_DEVICE_T* dev;

    dev = (ESAM_DEVICE_T*)hal_device_get(HW_DEVICE_ID_YX_ESAM);
    if (dev == NULL)
    {
        TESAM_FMT_DEBUG("ESAM设备获取句柄失败\n");
        return -1;
    }
    
    if (!isInit)
    {
        if (esam_device_set_lock(dev, 1) != ERR_OK)
        {
            TESAM_FMT_DEBUG("ESAM设备加锁失败\n");
            hal_device_release((HW_DEVICE *)dev);
            return -2;
        }

        //复位下esam
        dev->esam_set_power(dev, PWR_OFF);
        usleep(50000);
        dev->esam_set_power(dev, PWR_ON);
        usleep(50000);

        TESAM_FMT_DEBUG("复位esam电源成功\n");
        
        if (esam_device_set_lock(dev, 0) != ERR_OK)
        {
            TESAM_FMT_DEBUG("ESAM设备解锁失败\n");
            hal_device_release((HW_DEVICE *)dev);
            return -3;
        }
        
        isInit = TRUE;
    }

    hal_device_release((HW_DEVICE *)dev);
    return 0;
}

void esam_device_release(void)
{
    #if 0
    if ((isInit) && (gTesamDev != NULL))
    {
        isInit = FALSE;
        if (esam_device_set_lock(gTesamDev, 1) != ERR_OK)
        {
            TESAM_FMT_DEBUG("TESAM esam_set_lock faild\n");
            return;
        }
        gTesamDev->esam_set_power(gTesamDev, PWR_OFF);
        if (esam_device_set_lock(gTesamDev, 0) != ERR_OK)
        {
            TESAM_FMT_DEBUG("TESAM esam_set_unlock faild\n");
            return;
        }
        hal_device_release((HW_DEVICE *)gTesamDev);
        TESAM_FMT_TRACE("esam_device_release ok\n");
    }
    else
    {
        TESAM_FMT_TRACE("esam_device_release err\n");
    }
    #endif
    
    return;
}

/* 组件认证 */
int TESAM_AUTH_FLOW(int *sockedid)
{
    int ret = 0;
    int msglen = 0;
    unsigned int auth_len = 64;
    char sender[SECURITY_FILEPATH_LEN] = {0};

    uint8 backbuf[4096] = {0};
    uint8 auth_data[64] = {0};
    uint8 auth_time[8] = {0};
    uint8 send_test_buf[217] = {0};

    uint8 send_buf[121] = {0x55, 0x80, 0x5A, 0x06, 0x00, 0x00, 0x71, 0x10, 
                           0x15, 0xBF, 0x90, 0x60, 0xEF, 0x0E, 0xE9, 0x3B, 
                           0xC7, 0xF5, 0xB8, 0xD9, 0xCE, 0x9C, 0xF8, 0x55,
                           0X97, 0X7b, 0X18, 0Xb9, 0X40, 0Xc3, 0Xc4, 0X37,
                           0X22, 0X79, 0X54, 0X49, 0X3e, 0X19, 0X7b, 0X88,
                           0X95, 0X6b, 0X30, 0Xc2, 0Xad, 0Xc1, 0X4f, 0X55,
                           0X08, 0Xb7, 0Xd7, 0X11, 0Xce, 0X43, 0X95, 0X18,
                           0X1c, 0X45, 0X6e, 0X45, 0X02, 0X16, 0Xdc, 0Xfe,
                           0Xce, 0X2a, 0X44, 0X81, 0Xdf, 0Xaf, 0Xe1, 0Xdf,
                           0Xf7, 0X1b, 0Xbc, 0X3, 0Xe2, 0Xae, 0X23, 0Xf0,
                           0X6, 0X53, 0X7e, 0Xa5, 0Xf8, 0X53, 0Xd5, 0X87,
                           0Xd1, 0X72, 0X61, 0Xc0, 0X0f, 0Xe6, 0Xab, 0X79,
                           0Xe9, 0Xb9, 0X4f, 0X65, 0Xb0, 0Xff, 0Xc3, 0Xb6,
                           0X38, 0X0f, 0Xd4, 0X00, 0X9d, 0X33, 0X51, 0X48,
                           0Xcf, 0Xd1, 0Xce, 0X9d, 0Xaf, 0X39, 0Xc5, 0X72,
                           0xfa};

    ret = sdt_msg_auth(send_buf, 121, g_Esam_App_id, 24, g_Esam_AC_test, 16, auth_data, &auth_len, auth_time);

    memcpy(send_test_buf, send_buf, 121);
    memcpy(send_test_buf + 121, g_Esam_App_id, 24);
    memcpy(send_test_buf + 121 + 24, auth_data, 64);
    memcpy(send_test_buf + 121 + 24 + 64, auth_time, 8);
 
    msglen = sizeof(send_test_buf);
    ret = task_pfmsg_send(*sockedid, send_test_buf, msglen, SECURITY_APP_DEST);
    if(0 != ret)
    {
        TESAM_BUF_DEBUG(send_test_buf, msglen, "Esam auth send error %d", ret);
        return ret;
    }
    TESAM_BUF_DEBUG(send_test_buf, msglen, "Esam auth send msg");

    memset(backbuf, '\0', 1024);

    task_msg_settime(*sockedid, 2, 0);

    ret = task_pfmsg_recv(sockedid, backbuf, 4096, sender, SECURITY_FILEPATH_LEN);
    if(ret <= 0)
    {
        TESAM_FMT_DEBUG("TESAM recv faild, ret %d\n", ret);
        return -1;
    }
    TESAM_BUF_DEBUG(backbuf, ret, "ESAM RECV:");

    return 0;
}

int Init_Fd_Esam(void)
{
    
    /* 通过判断是否存在安全加固APP通信文件 */
    if(0 == access(SECURITY_APP_DEST, F_OK))
    {
        if(g_UDPEASMid <= 0)
        {
            char filename[SECURITY_FILEPATH_LEN] = {0};
            sprintf(filename, "%s/%s_esam", SECURITY_MSG_PATH, ADV_APP_NAME);
            g_UDPEASMid = task_pfmsg_init(filename);
            if(0 != TESAM_AUTH_FLOW(&g_UDPEASMid))
            {
                TESAM_FMT_TRACE("GetTEsam_Info err22\n");
                return NO_ESAM;
            }
            
            pthread_mutex_init(&g_fUdpEsamLock, NULL);
        }
        
        if(g_UDPEASMid <= 0)
        {
            TESAM_FMT_TRACE("GetTEsam_Info err11\n");
            return NO_ESAM;
        }
    }
    else
    {
        if (esam_power_open() < 0)
        {
            return NO_ESAM;
        }
    }
    
    if (ESAM_FAIL == GetTEsam_Info(0xFF,g_TESAMInfoBuff,&g_TESAMInfoLen))
    {
        g_TESAMInfoLen = 0;
        
        if (0 != access(SECURITY_APP_DEST, F_OK))
        {
            isInit = FALSE;
            if (esam_power_open() < 0)
            {
                return NO_ESAM;
            }
        }

        return NO_ESAM;
    }
    
    return 0;
}


/*
********************************************************************
        所属功能单元：2016版ESAM 模块的操作
        功能描述：
              设置客户机下发的随机随机数并保存，用于MAC计算
        输入参数：
        无
        输出参数：

        函数返回值：
    返回检测的结果
*******************************************************************
*/
int8 SetSecurityData(SecurityData_t SecData)
{
    s_SecurityData = SecData;
    return 0;
}
/*
********************************************************************
        所属功能单元：2012版ESAM 模块的操作
        功能描述：获取终端信息
        输入参数：
        无
        输出参数：
              pBuf   输出数据缓冲区
        函数返回值：
    返回检测的结果
*******************************************************************
*/
int8 ReadRandomAndSN_Esam(uint8 *pBuf)
{
    uint16 rcvLen = 0;

    uint16 sndLen = 0;
    // 获取随机数指令
    uint8 sndbuf_rm[] = {0x55, 0x80, 0x1A, 0x08, 0x00, 0x00, 0x00};
    // 获取序列号指令
    uint8 sndbuf_sn[] = {0x55, 0x80, 0x0E, 0x00, 0x02, 0x00, 0x00};

    // 获取随机数
    sndLen = sizeof(sndbuf_rm);
    TESAM_FMT_TRACE("获取随机数:\n");

    if ((TESAM_RcvSndDataAndCheck(sndbuf_rm, sndLen, pBuf, &rcvLen, FALSE) == ESAM_FAIL)
            || (rcvLen != 8))
    {
        return ESAM_FAIL;
    }

    // 获取芯片序列号
    sndLen = sizeof(sndbuf_sn);
    TESAM_FMT_TRACE("获取芯片序列号:\n");

    if ((TESAM_RcvSndDataAndCheck(sndbuf_sn, sndLen, &pBuf[8], &rcvLen, FALSE) == ESAM_FAIL)
            || (rcvLen != 8))
    {
        return ESAM_FAIL;
    }

    return ESAM_OK;
}

/*
********************************************************************
        所属功能单元：ESAM 模块的CRC校验
        功能描述：校验验证
        输入参数：
                dataBuf     数据区
                dataLen     数据长度
        输出参数：
                无
        函数返回值：
                TURE: 校验正确
                FALSE: 校验错误
*******************************************************************
*/
BOOL Crc_check_esam(uint8 *dataBuf, uint16 dataLen, uint8 lrc)
{
    uint8 temp = 0;
    uint16 i = 0;

    temp = dataBuf[0];
    for (i = 0; i < dataLen - 1; i++)
    {
        temp ^= dataBuf[i+1];
    }

    temp = ~temp;
    printf(" temp = %02x lrc = %02x\n", temp, lrc);
    if (temp == lrc)
    {
        return TRUE;
    }

    return FALSE;
}

/*
********************************************************************
        所属功能单元：ESAM 模块的CRC校验
        功能描述：校验验证
        输入参数：
                dataBuf     数据区
                dataLen     数据长度
        输出参数：
                无
        函数返回值：
    校验值
*******************************************************************
*/
uint8 Crc_cal_esam(uint8 *dataBuf, uint16 dataLen)
{
    uint8 temp = 0;
    uint16 i = 0;

    temp = dataBuf[0];
    for (i = 0; i < dataLen - 2; i++)
    {
        temp ^= dataBuf[i+1];
    }

    temp = ~temp;

    return temp;
}

/*******************************************************************************
* 函数名称: Bcd2Hex
* 函数功能: 将单字节BCD数据转Hex格式数据
* 输入参数: BcdNum  需要转换的BCD码
* 输出参数: 无
* 返 回 值: 转换后HEX码
*******************************************************************************/
uint8 Bcd2Hex(uint8 BcdNum)
{
    uint8 HexNum = 0;

    HexNum = ((BcdNum >> 4) & 0x0F) * 10 + (BcdNum & 0x0F);

    return (HexNum);
}


/*
********************************************************************
        所属功能单元：2012版ESAM 模块的操作
        功能描述：收发ESAM数据并检测结果
        输入参数：
                sndData    发送的数据区
                sndLen      发送的数据长度
        输出参数：pOutBuf

        函数返回值：
    返回检测的结果
*******************************************************************
*/
void writeEsamLog(uint8 errflg,uint8 esamVer,char *inbuf, uint16 inLen)
{
    uint16 i;
    char writeBuf[3000];
    memset(writeBuf, 0x00, sizeof(writeBuf));

    sprintf(writeBuf, "Send:%02x ", errflg);
    sprintf(writeBuf+8, "%02x ", esamVer);
    for(i=0; i<inLen; i++)
        sprintf(writeBuf+11+i*3, "%02x ", inbuf[i]);
    //writelog("/mnt/log/TESAMErr.log", "%s", writeBuf);
}

void writeEsamRecvLog(uint8 errflg,uint8 esamVer,char *inbuf, uint16 inLen)
{
    uint16 i;
    char writeBuf[3000];
    memset(writeBuf, 0x00, sizeof(writeBuf));
    sprintf(writeBuf, "Recv:%02x ", errflg);
    sprintf(writeBuf+8, "%02x ", esamVer);
    for(i=0; i<inLen; i++)
        sprintf(writeBuf+11+i*3, "%02x ", inbuf[i]);
    //writelog("/mnt/log/TESAMErr.log", "%s", writeBuf);
}

int8 TESAM_RcvSndDataAndCheckUnit(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen, BOOL needReverse)
{
    int32 i = 0;
    int32 iRet = 0;
    uint8 tmpBuf[10] = {0};
    uint8 czBuf[ESAM_BUFF_LEN] = {0};
    uint8 sndBuf[ESAM_BUFF_LEN] = {0};
    uint16 dataLen = 0;

    if (sndLen > sizeof(sndBuf))
    {
        return ESAM_FAIL;
    }

    // 不包括0x55, 从CLA 开始算校验
    memcpy(sndBuf, sndData, sndLen);
    dataLen = sndLen + 1;
    sndBuf[sndLen] = Crc_cal_esam(&sndData[1], (dataLen - 1));

    /* 通过判断是否存在安全加固APP通信文件 */
    if(0 == access(SECURITY_APP_DEST, F_OK))
    {
        unsigned int auth_len = 64;
        char sender[SECURITY_FILEPATH_LEN] = {0};
        uint16 msglen = 0;

        uint8 sendmsg[4096] = {0};
        uint8 auth_data[64] = {0};
        uint8 auth_time[8] = {0};

        iRet = sdt_msg_auth(sndBuf, dataLen, g_Esam_App_id, 24, g_Esam_AC_test, 16, auth_data, &auth_len, auth_time);

        memcpy(sendmsg, sndBuf, dataLen);
        memcpy(sendmsg + dataLen, g_Esam_App_id, 24);
        memcpy(sendmsg + dataLen + 24, auth_data, 64);
        memcpy(sendmsg + dataLen + 24 + 64, auth_time, 8);
     
        msglen = dataLen + 24 + 64 + 8;

        if(g_UDPEASMid <= 0)
        {
            return ESAM_FAIL;
        }

        /* 尝试加锁 */
        while(0 != pthread_mutex_trylock(&g_fUdpEsamLock))
        {
            if(i++ > 500)
            {
                TESAM_FMT_DEBUG("TESAM SEND lock faild\n");
                return ESAM_FAIL;
            }
            usleep(10000);
        }

        i = 0;
        /* 1. 1ms内收到消息, 等待0.1s的时间清空队列 */
        task_msg_settime(g_UDPEASMid, 0, 1000);
        iRet = task_pfmsg_recv(&g_UDPEASMid, czBuf, ESAM_BUFF_LEN, sender, SECURITY_FILEPATH_LEN);
        if(iRet > 0)
        {
            while(i < 100)
            {
                i++;
                task_pfmsg_recv(&g_UDPEASMid, czBuf, ESAM_BUFF_LEN, sender, SECURITY_FILEPATH_LEN);
            }
        }

        /* 2. 发送消息 */
        TESAM_BUF_TRACE(sendmsg, msglen, "TESAM SEND:");
        iRet = task_pfmsg_send(g_UDPEASMid, sendmsg, msglen, SECURITY_APP_DEST);
        if(0 != iRet)
        {
            pthread_mutex_unlock(&g_fUdpEsamLock);
            TESAM_FMT_DEBUG("TESAM send faild, ret %d\n", iRet);
            return ESAM_FAIL;
        }

        /* 3. 接收 2s超时 */
        task_msg_settime(g_UDPEASMid, 2, 0);
        iRet = task_pfmsg_recv(&g_UDPEASMid, czBuf, ESAM_BUFF_LEN, sender, SECURITY_FILEPATH_LEN);
        pthread_mutex_unlock(&g_fUdpEsamLock);
        if(iRet <= 0)
        {
            TESAM_FMT_DEBUG("TESAM recv faild, ret %d\n", iRet);
            return ESAM_FAIL;
        }
    }
    else
    {
        ESAM_DEVICE_T* dev = NULL;

        dev = (ESAM_DEVICE_T*)hal_device_get(HW_DEVICE_ID_YX_ESAM);
        if(dev == NULL)
        {
            TESAM_FMT_DEBUG("ESAM设备获取句柄失败\n");
            return ESAM_FAIL;
        }

        TESAM_BUF_TRACE(sndBuf, dataLen, "TESAM SEND:");
        //esam设备加锁，如果获取失败，重试95次
        #if 1
        /*while (i++)
        {
            iRet = dev->esam_set_lock(dev, 1);
            if (ERR_OK == iRet)
            {
                break;
            }
            else
            {
                usleep(10000);
            }

            if (i > 100)
            {
                TASK_FMT_TRACE(-1,REC_LOG, "esam设备加锁失败 错误码[%d] 文件锁[%d]\n", iRet, dev->fd_lock);
                hal_device_release((HW_DEVICE *)dev);
                return ESAM_FAIL;
            }
        }*/
        dev->esam_set_lock(dev, 1);
        #else
        int fd = open("/tmp/dev/esam_lck", O_CREAT |O_WRONLY | O_APPEND, 0664); 
        flock(fd, LOCK_EX);
        #endif
        
        iRet = dev->esam_data_write(dev, sndBuf, dataLen);
        if(iRet < 0)
        {
            TESAM_FMT_DEBUG("esam设备写失败 错误码[%d] 文件锁[%d]\n", iRet, dev->fd_lock);
            dev->esam_set_lock(dev, 0);
            if (iRet != ERR_OK)
            {
                TESAM_FMT_DEBUG("esam设备解锁失败 错误码[%d]\n", iRet);
            }
            hal_device_release((HW_DEVICE *)dev);
            return ESAM_FAIL;
        }

        memset(czBuf, 0x00, sizeof(czBuf));  
        iRet = dev->esam_data_read(dev, czBuf,sizeof(czBuf));
        if(iRet < 0)
        {
            TESAM_FMT_DEBUG("esam设备读失败 错误码[%d] 文件锁[%d]\n", iRet, dev->fd_lock);
            iRet = dev->esam_set_lock(dev, 0);
            if (iRet != ERR_OK)
            {
                TESAM_FMT_DEBUG("esam设备解锁失败 错误码[%d] 文件锁[%d]\n", iRet, dev->fd_lock);
            }
            hal_device_release((HW_DEVICE *)dev);
            return ESAM_FAIL;
        }

        //esam设备解锁
        #if 1
        dev->esam_set_lock(dev, 0);
        #else
        flock(fd, LOCK_UN); //Unlock
        close(fd);
        #endif 

        hal_device_release((HW_DEVICE *)dev);
    }

    tmpBuf[0] = czBuf[4];
    tmpBuf[1] = czBuf[3];         // 数据长度
    dataLen = 0;
    memcpy((char *)&(dataLen), tmpBuf, sizeof(uint16));

    if (dataLen > (sizeof(czBuf) - 5))
    {
        TESAM_BUF_DEBUG(czBuf, 100, "TESAM RECV(打印100字):");
        return ESAM_FAIL;
    }

    TESAM_BUF_TRACE(czBuf, dataLen + 6, "TESAM RECV:");

    if ((czBuf[0] == 0x55) && (czBuf[1] == 0x90) && (czBuf[2] == 0x00)
            && ((Crc_check_esam(&czBuf[3], (dataLen+2), czBuf[dataLen+5]) == TRUE)
                || (Crc_check_esam(&czBuf[1], (dataLen+4), czBuf[dataLen+5]) == TRUE)))
    {

        if (rcvLen != NULL)
            *rcvLen = dataLen;

        // 需要逆序
        if (needReverse == TRUE)
        {
            ReverseBuf_TESAM(&czBuf[5], dataLen);
        }

        if (pOutBuf != NULL)
        {
            memcpy(pOutBuf, &czBuf[5], dataLen);
        }
    }
    else
    {
        return ESAM_FAIL;
    }

    return ESAM_OK;
}

/*
********************************************************************
        所属功能单元：2012版ESAM 模块的操作
        功能描述：收发ESAM数据并检测结果，如果失败则重试
        输入参数：
                sndData    发送的数据区
                sndLen      发送的数据长度
        输出参数：pOutBuf

        函数返回值：
    返回检测的结果
*******************************************************************
*/
int8 TESAM_RcvSndDataAndCheck(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen, BOOL needReverse)
{
    uint32 i = 0;
    int8   ret = ESAM_FAIL;

    while (i < 2)
    {
        ret = TESAM_RcvSndDataAndCheckUnit(sndData, sndLen, pOutBuf, rcvLen, needReverse);
        if (ESAM_OK == ret)
        {
            break;
        }
        else
        {
            i++;
            usleep(10000);
        }
    }

    return ret;
}

/*
********************************************************************
        所属功能单元：2016版ESAM 模块的操作
        功能描述：获取终端信息
        输入参数：
        无
        输出参数：
              pOutBuf   输出数据缓冲区
        OutbufLen   输出数据的长度
        函数返回值：
    返回检测的结果
*******************************************************************
*/
int8 GetTEsam_Info(uint8 P2,uint8 *pOutBuf, uint16 *OutbufLen)
{
    uint8 TesamVer=0;
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 sndbuf[] = {0x55, 0x80, 0x36, 0x00, 0xFF, 0x00, 0x00};
#if SAL_PRTL == 1
    //SAL协议与698协议发给ESAM的数据头不一样
    if(P2 == 0xFE)
    {
        sndbuf[3] = 0x01;
		P2 = 0xFF;
    }
	else if(P2 == 0x1B)
    {
        sndbuf[3] = 0x01;
    }
	else if(P2 == 0x1C)
    {
        sndbuf[3] = 0x01;
    }
#endif
    TesamVer = 0x16;

    *OutbufLen = 0;
    if(TesamVer==0x12)
    {
        sndbuf[2] = 0x0E;
        sndbuf[4] = 0x02;
    }
    else
        sndbuf[4] = P2;
    // 获取芯片序列号
    sndLen = sizeof(sndbuf);
    TESAM_FMT_TRACE("获取终端ESAM信息:\n");

    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, pOutBuf, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }
    //ReverseBuf_TESAM(pOutBuf, rcvLen);

    *OutbufLen = rcvLen;

    return ESAM_OK;
}
/*
********************************************************************
        所属功能单元：2016版ESAM 模块的操作
        功能描述： 会话协商
        输入参数：
        pBuf = ucOutSessionInit(32)+ucOutSign(64)
        输出参数：
              pOutBuf   cSessionData(64)+cSign(64)
        OutbufLen   输出数据的长度
        函数返回值：
    返回检测的结果
*******************************************************************
*/
int8 NegotiateSession_Esam(uint8 *pInSessionInit,uint8 *pInSign, uint8 *pOutBuf, uint16 *OutbufLen)
{
    uint8 sndbuf[2000] = {0};

    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 tmpbuf[] = {0x55, 0x80, 0x02, 0x00, 0x00,0x00,0x60};
    uint8 *pSessionData;
    uint8 *pOutSign;
    *OutbufLen = 0;

    // 会话密钥协商
    sndLen = sizeof(tmpbuf);
    TESAM_FMT_TRACE("会话密钥协商:\n");

    memcpy(sndbuf, tmpbuf, sizeof(tmpbuf));

    ReverseBuf_TESAM(pInSessionInit, 32);
    ReverseBuf_TESAM(pInSign, 64);

    memcpy(sndbuf+sndLen,pInSessionInit,32);
    sndLen += 32;

    memcpy(sndbuf+sndLen,pInSign,64);
    sndLen += 64;

    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, pOutBuf, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }
    if(rcvLen!=0x70)
        return ESAM_FAIL;
    pSessionData = pOutBuf;
    pOutSign = pOutBuf+48;

    ReverseBuf_TESAM(pSessionData, 48);
    ReverseBuf_TESAM(pOutSign, 64);

    *OutbufLen = rcvLen;

    return ESAM_OK;
}
/*
********************************************************************
        所属功能单元：2016版ESAM 模块的操作
        功能描述： 根据随机数和明文数据 获取MAC
        输入参数：
        pRand  随机数16字节
        pPlainData 明文数据
        inDataLen 明文数据长度
        输出参数：
              pOutMAC MAC
        函数返回值：
    返回检测的结果
*******************************************************************
*/

int8 GetDataMac(uint8 *pRand,uint8 *pPlainData,uint16 inDataLen,uint8 *pOutMAC)
{
    uint8 sndbuf[ESAM_BUFF_LEN] = {0};

    uint16 dataLen = 0;
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 tmpbuf[] = {0x55, 0x80, 0x0E, 0x40, 0x02};

    if(inDataLen>sizeof(sndbuf)-(6+16))//6个字节帧固定字符+16个字节随机数
    {
        TESAM_BUF_TRACE((uint8*)&inDataLen, 2, "输入的数据长度超过缓冲区空间:");
        return ESAM_FAIL;
    }
    if(pRand==NULL)
        pRand = s_SecurityData.ClientRand;
    sndLen = sizeof(tmpbuf);
    TESAM_FMT_TRACE("获取数据MAC:\n");

    dataLen = inDataLen + 16;

    sndbuf[sndLen++] = (uint8)(dataLen>>8);
    sndbuf[sndLen++] = (uint8)(dataLen);

    memcpy(sndbuf, tmpbuf, sizeof(tmpbuf));

    ReverseBuf_TESAM(pRand, 16);
    memcpy(sndbuf+sndLen,pRand,16);
    sndLen += 16;

    ReverseBuf_TESAM(pPlainData, inDataLen);
    memcpy(sndbuf+sndLen,pPlainData,inDataLen);
    sndLen += inDataLen;

    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, pOutMAC, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }
    if(rcvLen!=4)
        return ESAM_FAIL;

    return ESAM_OK;
}

int8 GetRandAndDataMac(uint8 *pPlainData,uint16 inDataLen,uint8 *pOutMAC,uint8 *pOutRand)
{
    uint8 sndbuf[ESAM_BUFF_LEN] = {0};

    uint16 dataLen = 0;
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 tmpbuf[] = {0x55, 0x80, 0x14, 0x01, 0x03};

    if(inDataLen>sizeof(sndbuf)-6)//6个字节帧固定字符
    {
        TESAM_BUF_TRACE((uint8*)&inDataLen, 2, "输入的数据长度超过缓冲区空间:");
        return ESAM_FAIL;
    }

    sndLen = sizeof(tmpbuf);
    TESAM_FMT_TRACE("获取随机数和数据MAC:\n");

    dataLen = inDataLen;

    sndbuf[sndLen++] = (uint8)(dataLen>>8);
    sndbuf[sndLen++] = (uint8)(dataLen);

    memcpy(sndbuf, tmpbuf, sizeof(tmpbuf));

    ReverseBuf_TESAM(pPlainData, inDataLen);
    memcpy(sndbuf+sndLen,pPlainData,inDataLen);
    sndLen += inDataLen;

    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, sndbuf, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }
    if(rcvLen!=16)
        return ESAM_FAIL;

    memcpy(pOutRand,sndbuf,12);
    ReverseBuf_TESAM(pOutRand, 12);

    memcpy(pOutMAC,sndbuf+12,4);
    ReverseBuf_TESAM(pOutMAC, 4);

    return ESAM_OK;
}
/*
********************************************************************
        所属功能单元：2016版ESAM 模块的操作
        功能描述：校验数据
        输入参数：
              sid:CLA+INS+P1+P2
              pAttachData:LC(2Bytes)
        pInBuff   : 密文或密文数据
        inDataLen : 输入数据长度
        outDataLen: 输出数据长度
        pInMAC:MAC(4Bytes)

        输出参数：
              明文数据
        函数返回值：
    返回检测的结果
*******************************************************************
*/
int8 TEsamDataVerifi(SID_t sid,uint8 *pAttachData,uint8 *pInBuff,uint16 inDataLen,uint8 *pInMAC,uint8 *pOutBuff, uint16 *outDataLen)
{
    uint8 sndbuf[ESAM_BUFF_LEN] = {0};

    //uint16 dataLen = 0;
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 tmpbuf[] = {0x55, 0x00, 0x00, 0x00, 0x00};

    if(inDataLen>sizeof(sndbuf)-(6+4))//6个字节帧固定字符+4字节MAC
    {
        TESAM_BUF_TRACE((uint8*)&inDataLen, 2, "输入的数据长度超过缓冲区空间:");
        return ESAM_FAIL;
    }

    sndLen = sizeof(tmpbuf);
    TESAM_FMT_TRACE("数据验证:\n");

    tmpbuf[1] = sid.CLA;
    tmpbuf[2] = sid.INS;
    tmpbuf[3] = sid.P1;
    tmpbuf[4] = sid.P2;
    memcpy(sndbuf, tmpbuf, sizeof(tmpbuf));

    sndbuf[sndLen++] = (uint8)(pAttachData[0]);
    sndbuf[sndLen++] = (uint8)(pAttachData[1]);

    ReverseBuf_TESAM(pInBuff, inDataLen);
    memcpy(sndbuf+sndLen,pInBuff,inDataLen);
    sndLen += inDataLen;
    if(pInMAC!=NULL)
    {
        ReverseBuf_TESAM(pInMAC, 4);
        memcpy(sndbuf+sndLen,pInMAC,4);
        sndLen += 4;
    }
    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, sndbuf, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }
    //if(rcvLen!=16)
    //  return ESAM_FAIL;
    if(pOutBuff!=NULL)
    {
        memcpy(pOutBuff,sndbuf,rcvLen);
        ReverseBuf_TESAM(pOutBuff, rcvLen);
        *outDataLen = rcvLen;
    }

    return ESAM_OK;
}

int8 ResponseFrameDeal_long(SecurityMode_t secMode,uint8 *pInBuff,uint32 inDataLen,uint8 *pOutBuff,uint32 *pOutDataLen)
{
    uint8 sndbuf[ESAM_BUFF_LEN] = {0};

    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8  tmpbuf[] = {0x55, 0x80, 0x1C, 0x00, 0x11};
    int    ret = 0;
    int8   rst = ESAM_OK;
    uint8  TerDataHash[32] = {0}; //sm3哈希值

    if ((pInBuff == NULL) || (pOutBuff == NULL) || (pOutDataLen == NULL))
    {
        APP698_FMT_DEBUG("入参指针错误\n");
        return ESAM_FAIL;
    }

    if (inDataLen > 512*1024 - 6)//6个字节帧固定字符
    {
        APP698_FMT_DEBUG("输入的数据长度[%d]超过缓冲区空间\n", inDataLen);
        return ESAM_FAIL;
    }
    
    if(s_SecurityData.SecType==1)
    {
        APP698_FMT_DEBUG("不支持的加密类型[%d]\n", s_SecurityData.SecType);
        return ESAM_FAIL;
    }
    else
    {
        if(secMode == SecMod_Cipher)
            tmpbuf[4] = 0x96;
        else if(secMode == SecMod_CipherMAC)
            tmpbuf[4] = 0x97;
    }
    sndLen = sizeof(tmpbuf);
    APP698_FMT_DEBUG("应答帧加密和计算MAC:\n");

    switch (secMode)
    {
    case SecMod_PlainMAC:
        {
            uint8 DataMAC[4] = {0};
            uint32 PlainMACLen = 0;
            
            ret = calc_sm3(pInBuff, inDataLen, TerDataHash);
            if (ret != 0)
            {
                rst = ESAM_FAIL;
                APP698_FMT_DEBUG("计算sm3哈希值错误\n");
                break;
            }

            memcpy(sndbuf, tmpbuf, sizeof(tmpbuf));

            PlainMACLen = 32; //哈希值长度
            sndbuf[sndLen++] = (uint8)(PlainMACLen>>8);
            sndbuf[sndLen++] = (uint8)(PlainMACLen);
            
            memcpy(sndbuf+sndLen, TerDataHash, PlainMACLen);
            sndLen += PlainMACLen;
            
            if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, sndbuf, &rcvLen, FALSE) == ESAM_FAIL)
            {
                rst = ESAM_FAIL;
                APP698_BUF_DEBUG(sndbuf, MIN(sndLen, 2048), "sndLen[%d]-esam交互失败", sndLen);
                break;
            }

            if (rcvLen < sizeof(DataMAC))
            {
                
                rst = ESAM_FAIL;
                APP698_FMT_DEBUG("esam交互后返回长度[%d]不对\n", rcvLen);
                break;
            }
            else
            {
                memcpy(DataMAC, sndbuf + rcvLen - 4, 4);
            }

            //将内容重新组合成明文+MAC
            memcpy(pOutBuff, pInBuff, inDataLen);
            memcpy(&pOutBuff[inDataLen], DataMAC, 4);
            
            *pOutDataLen = inDataLen + 4;
        }
        break;
    case SecMod_Cipher:
    case SecMod_CipherMAC:
        {
            uint32 len = 0;
            uint32 count = inDataLen/2045;
            uint32 soffset = 0;
            uint32 doffset = 0;

            for (int i = 0; i < count + 1; i++)
            {
                if ((inDataLen - soffset) > 2045)
                {
                    len = 2045;
                }
                else
                {
                    len = inDataLen - soffset;
                }
                
                memcpy(sndbuf, tmpbuf, sizeof(tmpbuf));
                
                sndLen = sizeof(tmpbuf);
                sndbuf[sndLen++] = (uint8)(len>>8);
                sndbuf[sndLen++] = (uint8)(len);
                
                memcpy(sndbuf+sndLen, &pInBuff[soffset], len);
                sndLen  += len;
                soffset += len;
                
                if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, sndbuf, &rcvLen, FALSE) == ESAM_FAIL)
                {
                    rst = ESAM_FAIL;
                    APP698_BUF_DEBUG(sndbuf, MIN(sndLen, 2048), "sndLen[%d]-esam交互失败", sndLen);
                    break;
                }

                len = rcvLen + 1; //加上报文类型
                memcpy_r(&pOutBuff[doffset], &len, 2);
                doffset += 2;
                if (secMode == SecMod_CipherMAC)
                {
                    pOutBuff[doffset] = 1;
                }
                else
                {
                    pOutBuff[doffset] = 2;
                }
                doffset += 1;
                memcpy(&pOutBuff[doffset], &sndbuf, rcvLen);
                doffset += rcvLen;
            }

            if (rst == ESAM_FAIL)
            {
                break;
            }
            else
            {
                *pOutDataLen = doffset;
            }
        }
        break;
    default:
        break;
    }

    if (rst == ESAM_FAIL)
    {
        *pOutDataLen = 0;
    }

    return rst;
}

int8 ResponseFrameDeal(SecurityMode_t secMode,uint8 *pInBuff,uint16 inDataLen,uint8 *pOutBuff,uint16 *pOutDataLen)
{
    uint8 sndbuf[ESAM_BUFF_LEN] = {0};

    //uint16 dataLen = 0;
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 tmpbuf[] = {0x55, 0x80, 0x1C, 0x00, 0x11};

    if(inDataLen>sizeof(sndbuf)-6)//6个字节帧固定字符
    {
        TESAM_BUF_TRACE((uint8*)&inDataLen, 2, "输入的数据长度超过缓冲区空间:");
        return ESAM_FAIL;
    }
    if(s_SecurityData.SecType==1)
    {
        *pOutDataLen = inDataLen+4;
        memcpy(pOutBuff,pInBuff,inDataLen);
        return GetDataMac(NULL,pInBuff,inDataLen,pOutBuff+inDataLen);
    }
    else
    {
        if(secMode == SecMod_Cipher)
            tmpbuf[4] = 0x96;
        else if(secMode == SecMod_CipherMAC)
            tmpbuf[4] = 0x97;
        #if SAL_PRTL == 1
        else if(secMode == SecMod_SAL)
        {
            tmpbuf[2] = 0x3C;
            tmpbuf[3] = 0x30;
            tmpbuf[4] = 0x12;
        }
        #endif
    }
    sndLen = sizeof(tmpbuf);
    TESAM_FMT_TRACE("应答帧加密和计算MAC:\n");

    //tmpbuf[1] = s_SecurityData.sid.CLA;
    //tmpbuf[2] = s_SecurityData.sid.INS;
    //tmpbuf[3] = s_SecurityData.sid.P1;
    //tmpbuf[4] = s_SecurityData.sid.P2;
    memcpy(sndbuf, tmpbuf, sizeof(tmpbuf));

    //dataLen = inDataLen+s_SecurityData.ClientRandLen;
    sndbuf[sndLen++] = (uint8)(inDataLen>>8);
    sndbuf[sndLen++] = (uint8)(inDataLen);

    //ReverseBuf_TESAM(s_SecurityData.ClientRand, s_SecurityData.ClientRandLen);
// memcpy(sndbuf+sndLen,s_SecurityData.ClientRand,s_SecurityData.ClientRandLen);
// sndLen += s_SecurityData.ClientRandLen;

    ReverseBuf_TESAM(pInBuff, inDataLen);
    memcpy(sndbuf+sndLen,pInBuff,inDataLen);
    sndLen += inDataLen;

    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, sndbuf, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }
// if(rcvLen<s_SecurityData.ClientRandLen)
// 	return ESAM_FAIL;
    //rcvLen -= s_SecurityData.ClientRandLen;
    if(pOutBuff!=NULL)
    {
        memcpy(pOutBuff,sndbuf,rcvLen);
        ReverseBuf_TESAM(pOutBuff, rcvLen);
    }

    *pOutDataLen = rcvLen;
    return ESAM_OK;

}
int8 TESAMDataUpdate(SID_t sid,uint8 *pAttachData,uint8 attachLen,uint8 *pInBuff,uint16 inDataLen,uint8 *pInMAC)
{
    uint8 sndbuf[ESAM_BUFF_LEN] = {0};

    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 tmpbuf[] = {0x55, 0x00, 0x00, 0x00, 0x00};

    if(inDataLen>sizeof(sndbuf)-(6+4))//6个字节帧固定字符+4字节MAC
    {
        TESAM_BUF_TRACE((uint8*)&inDataLen, 2, "输入的数据长度超过缓冲区空间:");
        return ESAM_FAIL;
    }
    if(attachLen<2)
        return ESAM_FAIL;
    sndLen = sizeof(tmpbuf);
    TESAM_FMT_TRACE("ESAM数据更新:\n");

    tmpbuf[1] = sid.CLA;
    tmpbuf[2] = sid.INS;
    tmpbuf[3] = sid.P1;
    tmpbuf[4] = sid.P2;
    memcpy(sndbuf, tmpbuf, sizeof(tmpbuf));
    memcpy(sndbuf+sndLen,pAttachData,attachLen);
    sndLen += attachLen;
    ReverseBuf_TESAM(pInBuff, inDataLen);
    memcpy(sndbuf+sndLen,pInBuff,inDataLen);

    sndLen += inDataLen;
    if(pInMAC!=NULL)
    {
        ReverseBuf_TESAM(pInMAC, 4);
        memcpy(sndbuf+sndLen,pInMAC,4);
        sndLen += 4;
    }
    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, sndbuf, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }
    if(rcvLen!=0)
        return ESAM_FAIL;

    return ESAM_OK;
}

/*
********************************************************************
        所属功能单元：2016版ESAM 模块的操作
        功能描述：抄表获取随机数
        输入参数：
        输出参数：
              pOutRand :随机数
        函数返回值：
    返回检测的结果
*******************************************************************
*/
int8 ReadMtrGetRand(uint8 *pOutRand)
{
    uint8 sndbuf[2000] = {0};
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 tmpbuf[] = {0x55, 0x80, 0x04, 0x00, 0x10,0x00,0x00};

    sndLen = sizeof(tmpbuf);
    TESAM_FMT_TRACE("抄表-获取随机数:\n");

    memcpy(sndbuf, tmpbuf, sizeof(tmpbuf));

    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, sndbuf, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }
    memcpy(pOutRand,sndbuf,16);
    ReverseBuf_TESAM(pOutRand, 16);

    return ESAM_OK;
}

/*
********************************************************************
		所属功能单元：2016版ESAM 模块的操作
		功能描述：抄表校验MAC
		输入参数：
		输出参数：
		函数返回值：
	返回检测的结果
*******************************************************************
*/
int8 ReadMtrVerifyMAC(SecurityMode_t secMod,uint8 *pPlainData,uint16 inDataLen,uint8 *pInMAC,uint8 *pInRand,uint8 *pInEsamSerialNo,uint16 *pOutDataLen)
{
    uint8 sndbuf[ESAM_BUFF_LEN] = {0};

    uint16 dataLen = 0;
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 tmpbuf[] = {0x55, 0x80, 0x0E, 0x48, 0x87};
    if(secMod==SecMod_Cipher)
    {
        tmpbuf[2] = 0x0C;
        tmpbuf[4] = 0x07;
    }
    else if(secMod==SecMod_CipherMAC)
    {
        tmpbuf[2] = 0x12;
        tmpbuf[4] = 0x07;
    }
    if(inDataLen + 6+4+8+16>sizeof(sndbuf))//6字节固定,4个字节MAC,16字节随机数,8字节ESAM序列号
    {
        TESAM_BUF_TRACE((uint8*)&inDataLen, 2, "输入的数据长度超过缓冲区空间:");
        return ESAM_FAIL;
    }

    sndLen = sizeof(tmpbuf);
    TESAM_FMT_TRACE("获取随机数和数据MAC:\n");

    dataLen = inDataLen + 4 + 8 + 16;

    sndbuf[sndLen++] = (uint8)(dataLen>>8);
    sndbuf[sndLen++] = (uint8)(dataLen);

    memcpy(sndbuf, tmpbuf, sizeof(tmpbuf));

    ReverseBuf_TESAM(pInEsamSerialNo, 8);
    memcpy(sndbuf+sndLen,pInEsamSerialNo,8);
    sndLen += 8;

    ReverseBuf_TESAM(pInRand, 16);
    memcpy(sndbuf+sndLen,pInRand,16);
    sndLen += 16;

    ReverseBuf_TESAM(pPlainData, inDataLen);
    memcpy(sndbuf+sndLen,pPlainData,inDataLen);
    sndLen += inDataLen;
    if(secMod!=SecMod_Cipher)
    {
        ReverseBuf_TESAM(pInMAC, 4);
        memcpy(sndbuf+sndLen,pInMAC,4);
        sndLen += 4;
    }
    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, sndbuf, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }
    if(secMod!=SecMod_PlainMAC)
    {
        memcpy(pPlainData,sndbuf,rcvLen);
        *pOutDataLen = rcvLen;
    }
    else
    {
        *pOutDataLen = inDataLen;
    }
    return ESAM_OK;
}

/*
********************************************************************
        所属功能单元：2016版ESAM 模块的操作
        功能描述：初始化抄表安全应用环境
        输入参数：
        输出参数：
              pInEsamSerialNo :表号8 bytes
        函数返回值：
    返回检测的结果
*******************************************************************
*/
int8 Init_ReadMtr(uint8 pipe,uint8 *pInMtrNum)
{
    uint8 TesamVer=0;
	if(pipe>=MAX_READMTR_NUM)
		return ESAM_FAIL;
    memset(&s_ReadMtrPara[pipe],0x00,sizeof(s_ReadMtrPara[pipe]));
	if(pInMtrNum!=NULL)
    	memcpy(s_ReadMtrPara[pipe].EsamSerialNo,pInMtrNum,sizeof(s_ReadMtrPara[pipe].EsamSerialNo));
    //printf("ESAM序列号:");
    //commfun_printstring(pInMtrNum,8);
    if(NO_ESAM == Init_Fd_Esam())
    {
        return ESAM_FAIL;
    }
       

    TesamVer = 0x16;
    //需要判断终端ESAM序列号 是否是16版
    if(TesamVer==0x12)
    {
        //非16版ESAM，随便赋值一个随机数
        srand(time(NULL));
        *((uint32*)&s_ReadMtrPara[pipe].Rand[4]) = rand();
        *((uint32*)&s_ReadMtrPara[pipe].Rand[8]) = rand();
        *((uint32*)&s_ReadMtrPara[pipe].Rand[12]) = rand();
        return ESAM_OK;
    }
    else
    {
        if(ESAM_OK == ReadMtrGetRand(s_ReadMtrPara[pipe].Rand))
        {
            return ESAM_OK;
        }
        else
            return ESAM_FAIL;
    }
}
/*
********************************************************************
        所属功能单元：2016版ESAM 模块的操作
        功能描述：设置安全模式开关
        输入参数：
        输出参数：
              secModEn:1:开启，其他值未开启
        函数返回值：
    返回检测的结果
*******************************************************************
*/
int8 Set_MtrSecModEn(uint8 pipe,uint8 secModEn)
{
	if(pipe>=MAX_READMTR_NUM)
		return ESAM_FAIL;
	s_ReadMtrPara[pipe].SecModEn = secModEn;
	return ESAM_OK;
}

/*
********************************************************************
        所属功能单元：2016版ESAM 模块的操作
        功能描述：设置安全模式开关
        输入参数：
        输出参数：
              secModEn:1:开启，其他值未开启
        函数返回值：
    返回检测的结果
*******************************************************************
*/
int8 Set_ReadMtrNum(uint8 pipe,uint8 *pInMtrNum)
{
	if(pipe>=MAX_READMTR_NUM)
		return ESAM_FAIL;
    memcpy(s_ReadMtrPara[pipe].EsamSerialNo,pInMtrNum,sizeof(s_ReadMtrPara[pipe].EsamSerialNo));
	return ESAM_OK;

}

/*
********************************************************************
		所属功能单元：2016版ESAM 模块的操作
		功能描述：抄表数据校验
		输入参数：
		输出参数：
		函数返回值：
	返回检测的结果
*******************************************************************
*/
int8 Verify_ReadMtrMAC(uint8 pipe,SecurityMode_t secMod,uint8 *pPlainData,uint16 inDataLen,uint8 *pInMAC,uint16 *pOutDataLen)
{
    uint8 TesamVer=0;
	if(pipe>=MAX_READMTR_NUM)
		return ESAM_FAIL;
    
    TesamVer = 0x16;
    //需要判断终端ESAM序列号 是否是16版
    //非16版ESAM，直接返回OK
    if(TesamVer==0x12)
    {
        *pOutDataLen = inDataLen;
        if(secMod==SecMod_PlainMAC)
            return ESAM_OK;
        else
            return ESAM_FAIL;
    }
    else
        return(ReadMtrVerifyMAC(secMod,pPlainData,inDataLen,pInMAC,s_ReadMtrPara[pipe].Rand,s_ReadMtrPara[pipe].EsamSerialNo,pOutDataLen));
}

/*
********************************************************************
        所属功能单元：2016版ESAM 模块的操作
        功能描述：获取抄表环境的随机数
        输入参数：
        输出参数：
        函数返回值：
  		>0 :随机数长度
  		<=0:无随机数
*******************************************************************
*/
int8 Get_ReadMtrRand(uint8 pipe,uint8 *pOutRand)
{
	if(pipe>=MAX_READMTR_NUM)
		return ESAM_FAIL;
    memcpy(pOutRand,s_ReadMtrPara[pipe].Rand,sizeof(s_ReadMtrPara[pipe].Rand));
    return(sizeof(s_ReadMtrPara[pipe].Rand));
}

//根据OI获取默认的安全模式字
#if JIANG_SU_FUN == 1 && (HARDWARE_TYPE == 1 || HARDWARE_TYPE == 2 || HARDWARE_TYPE == 4)   // 针对江苏改成当主站对终端进行GET服务操作时，不判断对象的级别，最低级别也能访问。
#define GEN_SECMODWD_TMN(get,set,action,proxy)\
		(((uint16)0x08<<12)|((uint16)set<<8)|((uint16)action<<4)|((uint16)proxy<<0))
#else
#define GEN_SECMODWD_TMN(get,set,action,proxy)\
		(((uint16)get<<12)|((uint16)set<<8)|((uint16)action<<4)|((uint16)proxy<<0))
#endif
#define GEN_SECMODWD(get,set,action,proxy)\
            (((uint16)get<<12)|((uint16)set<<8)|((uint16)action<<4)|((uint16)proxy<<0))
/**********************************************************
* 函数名称：
		GetDefaultSecMod
* 功能描述：
		根据对象OI获取默认的安全模式-终端
* 输入描述：
		OI:对象OI
* 返回值：
		默认的安全模式
**********************************************************/
SecModWd_t GetDefaultSecMod(uint16 OI)
{
    SecModWd_t secModWd;
    secModWd.secMod = 0;
    switch(OI&0xF000)
    {
    case 0x0000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL,0,0,0x04);
        break;
    case 0x1000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC,0,0,0);
    case 0x2000:
        if((OI&0x0FFF) == 0x002C)
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL,0,0,0x04);
        else if((OI&0x0FFF) == 0x002F)
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC,SecMod_CipherMAC,SecMod_NULL,0);
        else
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC,SecMod_CipherMAC,SecMod_PlainMAC,0);
        break;
    case 0x3000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC,SecMod_CipherMAC,SecMod_CipherMAC,0);
        break;
    case 0x4000:
        switch(OI&0x0FFF)
        {
        case 0x0000:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL,SecMod_CipherMAC,SecMod_NULL,0);
            break;
        case 0x0001:
		case 0x0111:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL,SecMod_CipherMAC,0,0);
            break;
        case 0x0002:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL,SecMod_PlainMAC,0,0);
            break;
        case 0x0003:
		case 0x0103:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC,SecMod_PlainMAC,0,0);
            //secModWd.secMod = GEN_SECMODWD(SecMod_PlainMAC,SecMod_CipherMAC,0,0);
			break;
        case 0x000A:
        case 0x000B:
        case 0x0018:
        case 0x0019:
        case 0x001A:
        case 0x001B:
        case 0x001C:
        case 0x001D:
        case 0x001E:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC,SecMod_CipherMAC,0,0);
            break;
        default:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC,SecMod_CipherMAC,SecMod_CipherMAC,0);
            break;
        }
        break;
    case 0x5000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC,0,SecMod_CipherMAC,0x00);
        break;
    case 0x6000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL,SecMod_NULL,SecMod_NULL,0x00);
        break;
    case 0x7000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL,SecMod_NULL,SecMod_NULL,0x00);
        break;
    case 0x8000:
        secModWd.secMod = GEN_SECMODWD_TMN(SecMod_PlainMAC,SecMod_CipherMAC,SecMod_CipherMAC,0x00);
        break;
    case 0xF000:
        switch(OI&0x0FFF)
        {
        case 0x0000:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL,SecMod_CipherMAC,SecMod_CipherMAC,0x00);
            break;
        case 0x0001:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL,SecMod_Cipher,SecMod_Cipher,0x00);
            break;
        case 0x0002:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL,SecMod_NULL,SecMod_NULL,0x00);
            break;
        case 0x0100:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL,SecMod_PlainMAC,SecMod_CipherMAC,0x00);
            break;
        case 0x0101:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL,SecMod_CipherMAC,SecMod_PlainMAC,0x00);
            break;
        default:
            secModWd.secMod = GEN_SECMODWD_TMN(SecMod_NULL,SecMod_NULL,SecMod_NULL,0x00);
            break;
        }
        break;
    }
    return secModWd;
}

/**********************************************************
* 函数名称：
		GetMtrDefaultSecMod
* 功能描述：
		根据对象OI获取默认的安全模式-抄表
* 输入描述：
		OI:对象OI
* 返回值：
		默认的安全模式
**********************************************************/
SecModWd_t GetMtrDefaultSecMod(uint16 OI)
{
    SecModWd_t secModWd;
    secModWd.secMod = 0;
    switch(OI&0xF000)
    {
    case 0x0000:
        secModWd.secMod = GEN_SECMODWD(SecMod_NULL,0,0,0x04);
        break;
    case 0x1000:
        secModWd.secMod = GEN_SECMODWD(SecMod_PlainMAC,0,0,0);
    case 0x2000:
        if((OI&0x0FFF) == 0x002C)
            secModWd.secMod = GEN_SECMODWD(SecMod_NULL,0,0,0x04);
        else if((OI&0x0FFF) == 0x002F)
            secModWd.secMod = GEN_SECMODWD(SecMod_PlainMAC,SecMod_CipherMAC,SecMod_NULL,0);
        else
            secModWd.secMod = GEN_SECMODWD(SecMod_PlainMAC,SecMod_CipherMAC,SecMod_PlainMAC,0);
        break;
    case 0x3000:
        secModWd.secMod = GEN_SECMODWD(SecMod_PlainMAC,SecMod_CipherMAC,SecMod_CipherMAC,0);
        break;
    case 0x4000:
        switch(OI&0x0FFF)
        {
        case 0x0000:
            secModWd.secMod = GEN_SECMODWD(SecMod_NULL,SecMod_CipherMAC,SecMod_NULL,0);
            break;
        case 0x0001:
		case 0x0111:
            secModWd.secMod = GEN_SECMODWD(SecMod_NULL,SecMod_CipherMAC,0,0);
            break;
        case 0x0002:
            secModWd.secMod = GEN_SECMODWD(SecMod_NULL,SecMod_PlainMAC,0,0);
            break;
        case 0x0003:
		case 0x0103:
            secModWd.secMod = GEN_SECMODWD(SecMod_PlainMAC,SecMod_PlainMAC,0,0);
            //secModWd.secMod = GEN_SECMODWD(SecMod_PlainMAC,SecMod_CipherMAC,0,0);
			break;
        case 0x000A:
        case 0x000B:
        case 0x0018:
        case 0x0019:
        case 0x001A:
        case 0x001B:
        case 0x001C:
        case 0x001D:
        case 0x001E:
            secModWd.secMod = GEN_SECMODWD(SecMod_PlainMAC,SecMod_CipherMAC,0,0);
            break;
        default:
            secModWd.secMod = GEN_SECMODWD(SecMod_PlainMAC,SecMod_CipherMAC,SecMod_CipherMAC,0);
            break;
        }
        break;
    case 0x5000:
        secModWd.secMod = GEN_SECMODWD(SecMod_PlainMAC,0,SecMod_CipherMAC,0x00);
        break;
    case 0x6000:
        secModWd.secMod = GEN_SECMODWD(SecMod_NULL,SecMod_NULL,SecMod_NULL,0x00);
        break;
    case 0x7000:
        secModWd.secMod = GEN_SECMODWD(SecMod_NULL,SecMod_NULL,SecMod_NULL,0x00);
        break;
    case 0x8000:
        secModWd.secMod = GEN_SECMODWD(SecMod_PlainMAC,SecMod_CipherMAC,SecMod_CipherMAC,0x00);
        break;
    case 0xF000:
        switch(OI&0x0FFF)
        {
        case 0x0000:
            secModWd.secMod = GEN_SECMODWD(SecMod_NULL,SecMod_CipherMAC,SecMod_CipherMAC,0x00);
            break;
        case 0x0001:
            secModWd.secMod = GEN_SECMODWD(SecMod_NULL,SecMod_Cipher,SecMod_Cipher,0x00);
            break;
        case 0x0002:
            secModWd.secMod = GEN_SECMODWD(SecMod_NULL,SecMod_NULL,SecMod_NULL,0x00);
            break;
        case 0x0100:
            secModWd.secMod = GEN_SECMODWD(SecMod_NULL,SecMod_PlainMAC,SecMod_CipherMAC,0x00);
            break;
        case 0x0101:
            secModWd.secMod = GEN_SECMODWD(SecMod_NULL,SecMod_CipherMAC,SecMod_PlainMAC,0x00);
            break;
        default:
            secModWd.secMod = GEN_SECMODWD(SecMod_NULL,SecMod_NULL,SecMod_NULL,0x00);
            break;
        }
        break;
    }
    return secModWd;
}


/**********************************************************
* 函数名称：
		GetMtrSecMod
* 功能描述：
		根据对象OI获取的安全模式,如未设置则返回默认安全模式
* 输入描述：
		OI:对象OI
* 返回值：

**********************************************************/
SecModWd_t GetMtrSecMod(uint8 pipe,uint16 OI)
{
	SecModWd_t tmpSecMod;
    tmpSecMod.secMod = 0x888F;
	if(s_ReadMtrPara[pipe].SecModEn != 1)
		return tmpSecMod;
	return GetMtrDefaultSecMod(OI);
}

/*********************************************************************
所属功能单元：通用
功能描述：检测缓冲区是否全为0
输入参数：
输出参数：无
函数返回值:
            TRUE: 全为0  FALSE:不都是0
内部处理流程：
********************************************************************/
BOOL CheckZero(uint8 *buf, uint16 len)
{
	uint16 i = 0;

	for (i = 0; i < len; i++)
    {
        if (buf[i] != 0x00)
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL SaveTESAMInfo(void)
{
	TESAMInfo_t tESAMInfo;
    uint8  tESAMBuf[2048] = {0};
	uint16 tESAMInfoLen;

	if (NO_ESAM ==Init_Fd_Esam())
    {
        TESAM_FMT_DEBUG("Init_Fd_Esam err\n");
        return FALSE;
    }
		
	if (ESAM_FAIL == GetTEsam_Info(0xFF, (uint8*)&tESAMBuf[0], &tESAMInfoLen))
    {
        TESAM_FMT_DEBUG("GetTEsam_Info err\n");
        return FALSE;
    }

    if (tESAMInfoLen < sizeof(TESAMInfo_t))
    {
        TESAM_BUF_DEBUG(tESAMBuf, tESAMInfoLen, "GetTEsam_Info err. tESAMBuf:\n");
        return FALSE;
    }

    memcpy(&tESAMInfo, tESAMBuf, sizeof(TESAMInfo_t));

    //更新下密钥下发的状态
    if (!CheckZero(tESAMInfo.symKeyVersion, sizeof(tESAMInfo.symKeyVersion)))
    {
        set_esam_state(1);
        TESAM_BUF_DEBUG(tESAMInfo.symKeyVersion, sizeof(tESAMInfo.symKeyVersion), "对称私钥版本");
    }
    else
    {
        set_esam_state(0);
        TESAM_BUF_DEBUG(tESAMInfo.symKeyVersion, sizeof(tESAMInfo.symKeyVersion), "对称公钥版本");
    }
#if defined PRODUCT_ZCU_1 || defined PRODUCT_ZCU_3
    /* 保存到私有文件 */
    write_pdata_xram("/data/app/stAmr/cfg/tESAMInfo", (char*)&tESAMInfo, 0, sizeof(TESAMInfo_t));
#endif

#if defined (AREA_CHONGQING) && defined (PRODUCT_CCU)
    /* 保存到私有文件 */
    write_pdata_xram("/data/app/puAmr/cfg/tESAMInfo", (char*)&tESAMInfo, 0, sizeof(TESAMInfo_t));
#endif

    set_esam_update_state(1);

	return TRUE;
}

/*
********************************************************************
所属功能单元：2016版ESAM 模块的操作
功能描述：         获取IK信息(用于控制扩展模块)
输入参数：         无
输出参数：         pOutBuf   输出数据缓冲区 
              OutbufLen   输出数据的长度
函数返回值：        返回检测的结果
*******************************************************************
*/
int8 GetTEsam_IKB_Info(uint8_t *id, uint32_t idlen, uint8 *ikb, uint32 *ikblen)
{
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 sndbuf[64] = {0x55, 0x80, 0x0A, 0x00, 0x8B, 0x00, 0x0B, 0x01, 0x0B, 0x00};

    if ((id == NULL) || (ikb == NULL) || (ikblen == NULL) || (idlen == 0))
    {
        TESAM_FMT_DEBUG("输入参数为空 id[%p] pOutBuf[%p] ikblen[%p] idlen[%d]\n", id, ikb, ikblen, idlen);
        return ESAM_FAIL;
    }
    
    *ikblen = 0;

    memcpy(sndbuf + 10, id, idlen);
    
    sndLen = 10 + idlen;

    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, ikb, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }

    *ikblen = rcvLen;

    TESAM_BUF_TRACE(ikb, rcvLen, "获取终端IKB信息");

    return ESAM_OK;
}

/*
********************************************************************
所属功能单元：2016版ESAM 模块的操作
功能描述：         获取IK信息(用于控制扩展模块)
输入参数：         无
输出参数：         pOutBuf   输出数据缓冲区 
              OutbufLen   输出数据的长度
函数返回值：        返回检测的结果
*******************************************************************
*/
int8 GetTEsam_IKC_Info(uint8_t *id, uint32_t idlen, uint8 *ikc, uint32 *ikclen)
{
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 sndbuf[64] = {0x55, 0x80, 0x0A, 0x00, 0x8B, 0x00, 0x0B, 0x01, 0x0C, 0x00};

    if ((id == NULL) || (ikc == NULL) || (ikclen == NULL) || (idlen == 0))
    {
        TESAM_FMT_DEBUG("输入参数为空 id[%p] pOutBuf[%p] ikclen[%p] idlen[%d]\n", id, ikc, ikclen, idlen);
        return ESAM_FAIL;
    }
    
    *ikclen = 0;

    memcpy(sndbuf + 10, id, idlen);
    
    sndLen = 10 + idlen;

    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, ikc, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }

    *ikclen = rcvLen;

    TESAM_BUF_TRACE(ikc, rcvLen, "获取终端IKC信息");

    return ESAM_OK;
}

/*
********************************************************************
所属功能单元：2016版ESAM 模块的操作
功能描述：         获取IK信息(用于控制扩展模块)
输入参数：         无
输出参数：         pOutBuf   输出数据缓冲区 
              OutbufLen   输出数据的长度
函数返回值：        返回检测的结果
*******************************************************************
*/
int8 GetTEsam_IKA_Info(uint8_t *id, uint32_t idlen, uint8 *ika, uint32 *ikalen)
{
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 sndbuf[64] = {0x55, 0x80, 0x0A, 0x00, 0x8B, 0x00, 0x0B, 0x01, 0x0A, 0x00};

    if ((id == NULL) || (ika == NULL) || (ikalen == NULL) || (idlen == 0))
    {
        TESAM_FMT_DEBUG("输入参数为空 id[%p] pOutBuf[%p] ikalen[%p] idlen[%d]\n", id, ika, ikalen, idlen);
        return ESAM_FAIL;
    }
    
    *ikalen = 0;

    memcpy(sndbuf + 10, id, idlen);
    
    sndLen = 10 + idlen;

    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, ika, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }

    *ikalen = rcvLen;

    TESAM_BUF_TRACE(ika, rcvLen, "获取终端IKA信息");

    return ESAM_OK;
}

/*
********************************************************************
所属功能单元：2016版ESAM 模块的操作
功能描述：         获取IK信息(用于控制扩展模块)
输入参数：         无
输出参数：         pOutBuf   输出数据缓冲区 
              OutbufLen   输出数据的长度
函数返回值：        返回检测的结果
*******************************************************************
*/
int8 GetTEsam_SIKB_Info(uint8_t *id, uint32_t idlen, uint8 *sikb, uint32 *sikblen)
{
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 sndbuf[64] = {0x55, 0x80, 0x0A, 0x00, 0x8B, 0x00, 0x0B, 0x01, 0x0B, 0x01};

    if ((id == NULL) || (sikb == NULL) || (sikblen == NULL) || (idlen == 0))
    {
        TESAM_FMT_DEBUG("输入参数为空 id[%p] pOutBuf[%p] sikblen[%p] sidlen[%d]\n", id, sikb, sikblen, idlen);
        return ESAM_FAIL;
    }
    
    *sikblen = 0;

    memcpy(sndbuf + 10, id, idlen);
    
    sndLen = 10 + idlen;

    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, sikb, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }

    *sikblen = rcvLen;

    TESAM_BUF_TRACE(sikb, rcvLen, "获取终端SIKB信息");

    return ESAM_OK;
}

/*
********************************************************************
所属功能单元：2016版ESAM 模块的操作
功能描述：         获取IK信息(用于控制扩展模块)
输入参数：         无
输出参数：         pOutBuf   输出数据缓冲区 
              OutbufLen   输出数据的长度
函数返回值：        返回检测的结果
*******************************************************************
*/
int8 GetTEsam_SIKC_Info(uint8_t *id, uint32_t idlen, uint8 *sikc, uint32 *sikclen)
{
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 sndbuf[64] = {0x55, 0x80, 0x0A, 0x00, 0x8B, 0x00, 0x0B, 0x01, 0x0C, 0x01};

    if ((id == NULL) || (sikc == NULL) || (sikclen == NULL) || (idlen == 0))
    {
        TESAM_FMT_DEBUG("输入参数为空 id[%p] pOutBuf[%p] sikclen[%p] idlen[%d]\n", id, sikc, sikclen, idlen);
        return ESAM_FAIL;
    }
    
    *sikclen = 0;

    memcpy(sndbuf + 10, id, idlen);
    
    sndLen = 10 + idlen;

    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, sikc, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }

    *sikclen = rcvLen;

    TESAM_BUF_TRACE(sikc, rcvLen, "获取终端SIKC信息");

    return ESAM_OK;
}

/*
********************************************************************
所属功能单元：2016版ESAM 模块的操作
功能描述：         获取IK信息(用于控制扩展模块)
输入参数：         无
输出参数：         pOutBuf   输出数据缓冲区 
              OutbufLen   输出数据的长度
函数返回值：        返回检测的结果
*******************************************************************
*/
int8 GetTEsam_SIKA_Info(uint8_t *id, uint32_t idlen, uint8 *sika, uint32 *sikalen)
{
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 sndbuf[64] = {0x55, 0x80, 0x0A, 0x00, 0x8B, 0x00, 0x0B, 0x01, 0x0A, 0x01};

    if ((id == NULL) || (sika == NULL) || (sikalen == NULL) || (idlen == 0))
    {
        TESAM_FMT_DEBUG("输入参数为空 id[%p] pOutBuf[%p] sikalen[%p] sidlen[%d]\n", id, sika, sikalen, idlen);
        return ESAM_FAIL;
    }
    
    *sikalen = 0;

    memcpy(sndbuf + 10, id, idlen);
    
    sndLen = 10 + idlen;

    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, sika, &rcvLen, FALSE) == ESAM_FAIL)
    {
        return ESAM_FAIL;
    }

    *sikalen = rcvLen;

    TESAM_BUF_TRACE(sika, rcvLen, "获取终端SIKA信息");

    return ESAM_OK;
}

