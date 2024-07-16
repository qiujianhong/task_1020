
#include "CtsAll.h"
#include "hal.h"
#include "esam.h"
#include "security_msg.h"
#include "task_msg.h"
#include "gen_auth_data.h"

#define TESAM_IOC_MAGIC         'y'
#define TESAM_IOC_SET_CS        _IOW(TESAM_IOC_MAGIC, 5, unsigned char)
#define TESAM_IOC_SET_PWR        _IOW(TESAM_IOC_MAGIC, 6, unsigned char)

#define GPIO_OUT_HIGH           1
#define GPIO_OUT_LOW            0

#define TESAM_RST_PIN            TESAM_IOC_SET_PWR
#define TESAM_CS_PIN                TESAM_IOC_SET_CS


#define DEVICE_TESAM_16           "/sys/bus/spi/devices/spi1.0/tesam"

#define DEVICE_TESAM_12           "/dev/tesam"
#define MAX_READMTR_NUM              32
#define ESAM_BUFF_LEN             4096        //ESAM交互缓冲区长度

/* 临时使用的appid和校验 */
uint8 g_Esam_App_id[24] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                           0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
                           0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,};

uint8 g_Esam_AC_test[16] = {0x14, 0x16, 0x01, 0xA1, 0x3C, 0xE5, 0x70, 0xFF,
                            0xB2, 0xD4, 0xF6, 0x41, 0x61, 0x02, 0x02, 0x11};

int    g_UDPEASMid = 0;
pthread_mutex_t g_fUdpEsamLock;

static  ReadMtrPara_t  s_ReadMtrPara[MAX_READMTR_NUM];

uint8  g_TESAMInfoBuff[2048];
uint16 g_TESAMInfoLen;

static BOOL            isInit = FALSE;
extern uint8           s_randPara[16];
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
        TESAM_FMT_TRACE("ESAM设备获取句柄失败\n");
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

        dev->esam_set_power(dev, PWR_ON);
        usleep(50000);

        TESAM_FMT_DEBUG("打开esam电源成功\n");
        
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
    }
    #endif

    return;
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

    printf("开始读取ESAM随机数 \n");
    if (TESAM_RcvSndDataAndCheck(tmpbuf, sndLen, sndbuf, &rcvLen) != ERR_OK)
    {
        return ESAM_FAIL;
    }
    memcpy(pOutRand,sndbuf,16);
    

    return ESAM_OK;
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
    
    uint16 rcvLen = 0;
    uint16 sndLen = 0;
    uint8 sndbuf[] = {0x55, 0x80, 0x36, 0x00, 0xFF, 0x00, 0x00};


    *OutbufLen = 0;

    sndbuf[4] = P2;
    // 获取芯片序列号
    sndLen = sizeof(sndbuf);
    printf("获取终端ESAM信息\n");
    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, pOutBuf, &rcvLen) != ERR_OK)
    {
        return ESAM_FAIL;
    }

    *OutbufLen = rcvLen;

    return ESAM_OK;
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
        printf("TESAM recv faild, ret %d\n", ret);
        return -1;
    }
    TESAM_BUF_DEBUG(backbuf, ret, "ESAM RECV:");

    return 0;
}

/********************************************************************
        所属功能单元：2012版ESAM 模块的操作
        功能描述：初始化ESAM设备
        输入参数：
        无
        输出参数：
              无
        函数返回值：
    返回fd
*******************************************************************
*/
int Init_Fd_Esam(void)
{
    /* 通过判断是否存在安全加固APP通信文件 */
    if(0 == access(SECURITY_APP_DEST, F_OK))
    {
        if(g_UDPEASMid <= 0)
        {
            char filename[SECURITY_FILEPATH_LEN] = {0};
            sprintf(filename, "%s/%s_esam", SECURITY_MSG_PATH, "taskManager");
            g_UDPEASMid = task_pfmsg_init(filename);
            if(0 != TESAM_AUTH_FLOW(&g_UDPEASMid))
            {
                return NO_ESAM;
            }
            pthread_mutex_init(&g_fUdpEsamLock, NULL);
        }
        if(g_UDPEASMid <= 0)
        {
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
        功能描述：初始化抄表安全应用环境
        输入参数：
        输出参数：
              pInEsamSerialNo :表号8 bytes
        函数返回值：
    返回检测的结果
*******************************************************************
*/
int8 Init_ReadMtr(uint8 pipe)
{

    if(pipe>=MAX_READMTR_NUM)
        return ESAM_FAIL;
    memset(&s_ReadMtrPara[pipe],0x00,sizeof(s_ReadMtrPara[pipe]));

    //if(NO_ESAM == Init_Fd_Esam())
        //return ESAM_FAIL;

  
    if(ESAM_OK == ReadMtrGetRand(s_ReadMtrPara[pipe].Rand))
    {
        //TASK_FMT_TRACE(pipe, REC_LOG, "读取esam随机数成功 \n");
        printf("读取esam随机数成功  \n");
        commfun_printstring(s_ReadMtrPara[pipe].Rand,16);
        return ESAM_OK;
    }
    else
    {
        //TASK_FMT_TRACE(pipe, REC_LOG, "读取esam随机数失败 \n");
        return ESAM_FAIL;
    }

}



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


int8 Get_ReadMtrRand(uint8 pipe,uint8 *pOutRand)
{
    if(pipe>=MAX_READMTR_NUM)
        return ESAM_FAIL;
    memcpy(pOutRand,s_ReadMtrPara[pipe].Rand,sizeof(s_ReadMtrPara[pipe].Rand));
    return(sizeof(s_ReadMtrPara[pipe].Rand));
}

//根据OI获取默认的安全模式字

#define GEN_SECMODWD_TMN(get,set,action,proxy)\
        (((uint16)get<<12)|((uint16)set<<8)|((uint16)action<<4)|((uint16)proxy<<0))

#define GEN_SECMODWD(get,set,action,proxy)\
            (((uint16)get<<12)|((uint16)set<<8)|((uint16)action<<4)|((uint16)proxy<<0))


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
    //不再判断表的安全模式 默认开
    //printf("s_ReadMtrPara[pipe].SecModEn is %d\n",s_ReadMtrPara[pipe].SecModEn);
    //SecModWd_t tmpSecMod;
    //tmpSecMod.secMod = 0x888F;
    //if(s_ReadMtrPara[pipe].SecModEn != 1)
        //return tmpSecMod;
    return GetMtrDefaultSecMod(OI);
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
        //printBuf_TESAM((uint8*)&inDataLen, 2, "输入的数据长度超过缓冲区空间:");
        return ESAM_FAIL;
    }

    sndLen = sizeof(tmpbuf);
    //printBuf_TESAM(NULL, 0, "获取随机数和数据MAC");

    dataLen = inDataLen + 4 + 8 + 16;

    sndbuf[sndLen++] = (uint8)(dataLen>>8);
    sndbuf[sndLen++] = (uint8)(dataLen);

    memcpy(sndbuf, tmpbuf, sizeof(tmpbuf));

    //ReverseBuf_TESAM(pInEsamSerialNo, 8);
    memcpy(sndbuf+sndLen,pInEsamSerialNo,8);
    sndLen += 8;

    //ReverseBuf_TESAM(pInRand, 16);
    memcpy(sndbuf+sndLen,pInRand,16);
    sndLen += 16;

    //ReverseBuf_TESAM(pPlainData, inDataLen);
    memcpy(sndbuf+sndLen,pPlainData,inDataLen);
    sndLen += inDataLen;
    if(secMod!=SecMod_Cipher)
    {
        //ReverseBuf_TESAM(pInMAC, 4);
        memcpy(sndbuf+sndLen,pInMAC,4);
        sndLen += 4;
    }
    if (TESAM_RcvSndDataAndCheck(sndbuf, sndLen, sndbuf, &rcvLen) != ERR_OK)
    {
    	TASK_BUF_DEBUG( -1, REC_LOG,sndbuf,rcvLen);
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
        功能描述：抄表数据校验
        输入参数：
        输出参数：
        函数返回值：
    返回检测的结果
*******************************************************************
*/
int8 Verify_ReadMtrMAC(uint8 pipe,SecurityMode_t secMod,uint8 *pPlainData,uint16 inDataLen,uint8 *pInMAC,uint16 *pOutDataLen)
{
    
    if(pipe>=MAX_READMTR_NUM)
        return ESAM_FAIL;
//    printf("Verify_ReadMtrMAC 随机数\n");
//    commfun_printstring(s_ReadMtrPara[pipe].Rand, 16);
//    printf("Verify_ReadMtrMAC EsamSerialNo\n");
//    commfun_printstring(s_ReadMtrPara[pipe].EsamSerialNo, 8);
#ifdef AREA_SHANXI
    int8 ret;
    ret = ReadMtrVerifyMAC(secMod,pPlainData,inDataLen,pInMAC,s_randPara,s_ReadMtrPara[pipe].EsamSerialNo,pOutDataLen);
    if(ret != ESAM_OK)
    {
        //山西需求 若MAC验证不合格，需要将表号的第11位和第12位置零后重新校验
        s_ReadMtrPara[pipe].EsamSerialNo[2] = 0;
        TASK_FMT_TRACE(-1,REC_LOG,"山西二次验证mac 表号为 \n");
        TASK_BUF_TRACE(-1,REC_LOG, s_ReadMtrPara[pipe].EsamSerialNo, 8);
        ret = ReadMtrVerifyMAC(secMod,pPlainData,inDataLen,pInMAC,s_randPara,s_ReadMtrPara[pipe].EsamSerialNo,pOutDataLen);
    }
    return ret;
#else
    
    return(ReadMtrVerifyMAC(secMod,pPlainData,inDataLen,pInMAC,s_randPara,s_ReadMtrPara[pipe].EsamSerialNo,pOutDataLen));
#endif

    //return(ReadMtrVerifyMAC(secMod,pPlainData,inDataLen,pInMAC,s_ReadMtrPara[pipe].Rand,s_ReadMtrPara[pipe].EsamSerialNo,pOutDataLen));
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

int8 TESAM_RcvSndDataAndCheckUnit(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen)
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
            if (0 != Init_Fd_Esam())
            {
                return ESAM_FAIL;
            } 
        }

        /* 尝试加锁 */
        while(0 != pthread_mutex_trylock(&g_fUdpEsamLock))
        {
            if(i++ > 500)
            {
                TASK_FMT_TRACE(-1,REC_LOG,"TESAM SEND lock faild\n");
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
        TASK_FMT_TRACE(-1,REC_LOG,"ESAM发送消息 \n");
        TASK_BUF_TRACE(-1,REC_LOG,sndBuf, dataLen);
        iRet = task_pfmsg_send(g_UDPEASMid, sendmsg, msglen, SECURITY_APP_DEST);
        if(0 != iRet)
        {
            pthread_mutex_unlock(&g_fUdpEsamLock);
            TASK_FMT_TRACE(-1,REC_LOG,"TESAM send faild, ret %d\n", iRet);
            return ESAM_FAIL;
        }

        /* 3. 接收 2s超时 */
        task_msg_settime(g_UDPEASMid, 2, 0);
        iRet = task_pfmsg_recv(&g_UDPEASMid, czBuf, ESAM_BUFF_LEN, sender, SECURITY_FILEPATH_LEN);

        TASK_FMT_TRACE(-1,REC_LOG,"ESAM收到消息\n");
        TASK_BUF_TRACE(-1,REC_LOG,czBuf, dataLen+5);
        
        pthread_mutex_unlock(&g_fUdpEsamLock);
        if(iRet <= 0)
        {
            TASK_FMT_TRACE(-1,REC_LOG,"TESAM recv faild, ret %d\n", iRet);
            return ESAM_FAIL;
        }
    }
    else
    {

        ESAM_DEVICE_T* dev = NULL;
        dev = (ESAM_DEVICE_T*)hal_device_get(HW_DEVICE_ID_YX_ESAM);
        if(dev == NULL)
        {
            TASK_FMT_TRACE(-1,REC_LOG, "ESAM设备获取句柄失败\n");
            return ESAM_FAIL;
        }

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

        TASK_FMT_TRACE(-1,REC_LOG,"ESAM发送消息 \n");
        TASK_BUF_TRACE(-1,REC_LOG,sndBuf, dataLen);

        iRet = dev->esam_data_write(dev, sndBuf, dataLen);
        if(iRet < 0)
        {
            TASK_FMT_TRACE(-1, REC_LOG, "esam设备写失败 错误码[%d] 文件锁[%d]\n", iRet, dev->fd_lock);
            
            dev->esam_set_lock(dev, 0);
            hal_device_release((HW_DEVICE *)dev);
            return ESAM_FAIL;
        }

        memset(czBuf, 0x00, sizeof(czBuf));  
        iRet = dev->esam_data_read(dev, czBuf,sizeof(czBuf));
        if(iRet < 0)
        {
            TASK_FMT_TRACE(-1, REC_LOG, "esam设备读失败 错误码[%d] 文件锁[%d]\n", iRet, dev->fd_lock);
            dev->esam_set_lock(dev, 0);
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

    TASK_FMT_TRACE(-1,REC_LOG,"ESAM收到消息\n");
    TASK_BUF_TRACE(-1,REC_LOG,czBuf, dataLen+5);

    if (dataLen > (sizeof(czBuf) - 5))
    {
        TASK_FMT_TRACE(-1,REC_LOG,"失败22\n");
        return ESAM_FAIL;
    }

    if ((czBuf[0] == 0x55) && (czBuf[1] == 0x90) && (czBuf[2] == 0x00)
            && ((Crc_check_esam(&czBuf[3], (dataLen+2), czBuf[dataLen+5]) == TRUE)
                || (Crc_check_esam(&czBuf[1], (dataLen+4), czBuf[dataLen+5]) == TRUE)))
    {
        if (rcvLen != NULL)
            *rcvLen = dataLen;

        if (pOutBuf != NULL)
        {
            memcpy(pOutBuf, &czBuf[5], dataLen);
        }
    }
    else
    {
        TASK_FMT_TRACE(-1,REC_LOG,"失败11\n");
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
int8 TESAM_RcvSndDataAndCheck(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen)
{
    uint32 i = 0;
    int8   ret = ESAM_FAIL;
    uint8 sendbuf[2000]={0};
    uint16 sendlen;
    if(sndData[0]!=0x55)
    {
        sendbuf[0] = 0x55;
        memcpy(sendbuf+1,sndData,sndLen);
        sendlen = sndLen+1;
    }else
    {
        memcpy(sendbuf,sndData,sndLen);
        sendlen = sndLen;
    }
    while (i < 2)
    {
        ret = TESAM_RcvSndDataAndCheckUnit(sendbuf, sendlen, pOutBuf, rcvLen);
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
    
    if(ret == ESAM_OK)
        return ERR_OK;
    else 
        return -1;
    
}
