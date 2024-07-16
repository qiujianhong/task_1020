#include "baseCommon.h"
#include "esam.h"
#include "TESAM.h"
#include "framecomm.h"

#include "security_msg.h"
#include "task_msg.h"
#include "gen_auth_data.h"

#define TESAM_IOC_MAGIC         'y'
#define TESAM_IOC_SET_CS        _IOW(TESAM_IOC_MAGIC, 5, unsigned char)
#define TESAM_IOC_SET_PWR        _IOW(TESAM_IOC_MAGIC, 6, unsigned char)

#undef GPIO_OUT_HIGH
#define GPIO_OUT_HIGH           1

#undef GPIO_OUT_LOW
#define GPIO_OUT_LOW            0

#define TESAM_RST_PIN           TESAM_IOC_SET_PWR
#define TESAM_CS_PIN            TESAM_IOC_SET_CS
#define TURN_OFF_TESAM          0X6501              //power off
#define TURN_ON_TESAM           0X6502              //power on
#define GET_POWER_STATUS        0X6503              //get status

#define SET_CS_LOW              0X6504              //Set CS/SSN = 0    enable CS
#define SET_CS_HIGH             0X6505              //Set CS/SSN = 1    disable CS

#define CS_DELAY                50                  //50 us
#define CHECK_STATUS_MAX_COUNT  100000              //500 times
#define CHECK_STATUS_DELAY      200000              //200 ms  CHECK_STATUS_MAX_COUNT * CHECK_STATUS_DELAY = 3s

#define START_FLAG_VALUE        0x55

#if HARDWARE_TYPE == 2
#define DEVICE_TESAM_16         "/sys/bus/spi/devices/spi0.0/tesam"
#else
#define DEVICE_TESAM_16         "/sys/bus/spi/devices/spi1.0/tesam"
#endif
#define DEVICE_TESAM_12         "/dev/tesam"
#define MAX_READMTR_NUM         32
#define ESAM_BUFF_LEN           4096                //ESAM交互缓冲区长度

int     g_UDPEASMid = 0;
pthread_mutex_t g_fUdpEsamLock;

/* 临时使用的appid和校验 */
uint8 g_Esam_App_id[24] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                           0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
                           0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,};

uint8 g_Esam_AC_test[16] = {0x14, 0x16, 0x01, 0xA1, 0x3C, 0xE5, 0x70, 0xFF,
                            0xB2, 0xD4, 0xF6, 0x41, 0x61, 0x02, 0x02, 0x11};

//static  int g_fd_esam = -1;
//static  SecurityData_t s_SecurityData;
//static  ReadMtrPara_t  s_ReadMtrPara[MAX_READMTR_NUM];
uint8 g_TESAMInfoBuff[2048];
uint16 g_TESAMInfoLen;

int Close_Fd_Esam(int fd);
void DAEsamSendvPrint(uint8 *pBuf,int len);
int writelog(const char *filepath, const char *fmt, ...);

void TESAM_CS_CONTROL(int value)
{
    int fd = -1;
    fd = open(DEVICE_TESAM_12,O_RDWR|O_NOCTTY);

    ioctl(fd, TESAM_CS_PIN, value);

    close(fd);
}

void printBuf_TESAM(uint8 *buf, uint16 len, char *str_word)
{
#if 0
    uint16 i = 0;

    char logstr[2000];
    char tmp[2000] = {0};

    sprintf(logstr, " F%d: len: %d / ", step, len);
    write_log(ESAM_DATA_LOG, logstr);

    memset(logstr, 0x00, sizeof(logstr));
    memset(tmp, 0x00, sizeof(tmp));

    for(i = 0; i < len; i++)
    {
        sprintf(logstr, "%02x ", buf[i]);
        strcat(tmp, logstr);
    }
    strcpy(logstr, tmp);

    write_log(ESAM_DATA_LOG, logstr);
//#endif

    if (str_word != NULL)
        printf("%s: \n", str_word);

    if (len != 0)
        printf("数据长度len: %d / ", len);

    for (i = 0; i < len; i++)
    {
        printf("%02x ", buf[i]);
    }

    if (buf != NULL)
    {
        printf("\n");
    }
#endif
}

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

#if 0
void SetTesamPowerOn(int fdd,uint8 ver)
{
    int nRet;
    int fd = -1;
    fd = open(DEVICE_TESAM_12,O_RDWR|O_NOCTTY);
    if(ver==0x12)
    {
        nRet = ioctl(fd, GET_POWER_STATUS, 1);
        if(nRet == TURN_OFF_TESAM)
        {
            ioctl(fd, TURN_ON_TESAM, 1); //turn on tesam
            usleep(100000);
        }
    }
    else
    {
        ioctl(fd, TESAM_RST_PIN, GPIO_OUT_LOW);
    }


    Close_Fd_Esam(fd);
}

void SetTesamPowerOff(int fdd,uint8 ver)
{
    int nRet;
    int fd = -1;
    fd = open(DEVICE_TESAM_12,O_RDWR|O_NOCTTY);
    if(ver==0x12)
    {
        nRet = ioctl(fd, GET_POWER_STATUS, 1);

        if(nRet == TURN_ON_TESAM)
        {
            ioctl(fd, TURN_OFF_TESAM, 1); //turn off tesam
            usleep(100000);
        }
    }
    else
        ioctl(fd, TESAM_RST_PIN, GPIO_OUT_HIGH);

    Close_Fd_Esam(fd);
}
#endif

/*
********************************************************************
所属功能单元：加密处理模块
功能描述    ：Esam通讯校验和计算
输入参数    ：校验数据缓冲区，len 校验缓冲区数据长度
输出参数    ：无
函数返回值  ：校验和

作者        ：朴军海
创建日期    ：2019年8月7日
修改者      ：
修改说明    ：
修改日期    ：
*******************************************************************/
uint8  DACalEsamLrc(uint8 *buf,int len)
{
    int   i;
    uint8 LRC = 0;

    for(i=0;i<len;i++)
    {
        LRC ^= buf[i];
    }

    LRC = ~LRC;

    return LRC;
}

/*
********************************************************************
所属功能单元：加密处理模块
功能描述    ：写数据到加密芯片
输入参数    ：pDev 设备指针，buf 写入数据缓冲区，len 写入数据长度

输出参数    ：无

函数返回值  ：成功返回写入数据长度，失败返回-1

作者        ：朴军海
创建日期    ：2019年8月7日
修改者      ：
修改说明    ：
修改日期    ：
*******************************************************************/
int DAEsamWrite(ESAM_DEVICE_T* pDev,uint8 *buf,int len)
{
#if 0
    struct encryption_reg_req encryption_reg_req_st;

    encryption_reg_req_st.buflen = len;
    encryption_reg_req_st.bufdata = buf;

    if(ioctl(fd, ESAM_SET_DATA, &encryption_reg_req_st)< 0 )
    {
        len =  -1;
    }
#else
    int rtn = 0;

    if ((rtn = pDev->esam_data_write(pDev, buf, len)) < 0) {
        TESAM_FMT_DEBUG("DAEsamWrite() TESAM data write failed, ret:%d\n", rtn);

        return -1;
    }
#endif
    DAEsamSendvPrint((uint8 *)buf,len);

    return len;
}

/*
********************************************************************
所属功能单元：加密处理模块
功能描述    ：从加密芯片读取返回数据
输入参数    ：pDev 设备指针，maxlen 读数据缓冲区最大长度

输出参数    ：buf读数据缓冲区，返回数据不带校验和

函数返回值  ：成功返回写入数据长度，失败返回-1

作者        ：朴军海
创建日期    ：2019年8月7日
修改者      ：
修改说明    ：
修改日期    ：
*******************************************************************/
int DAEsamRead(ESAM_DEVICE_T* pDev, uint8 *buf, int maxlen)
{
    int  len = 0;
    char lrc = 0;
    int  rtn = -1;
    uint16 bRtnCmd = 0;
    uint8 bTimes = 0;

    while(bTimes++<3)
    {
        rtn = pDev->esam_data_read(pDev, buf, maxlen);
        TESAM_FMT_DEBUG("pDev->esam_data_read() result:%d\n", rtn);
        if (rtn < 0) 
        {
            TESAM_FMT_DEBUG("TESAM data read failed, result:%d\n", rtn);
            return -1;
        }

        bRtnCmd = (buf[0]<<8) + buf[1];
        switch(bRtnCmd)
        {
            case RTN_SUCCESS ://0x9000 成功
            {
                len = (int)((buf[2]<<8) + buf[3]);
                if(len == 0)
                {
                    lrc = DACalEsamLrc(buf,4);
                    if(lrc != buf[4])
                    {
                        usleep(10);
                        continue;
                    }
                    else
                    {
                        rtn = 4;
                        break;
                    }
                }

                if(len <= (maxlen-5))
                {
                    lrc = DACalEsamLrc(buf,len+4);

                    if(lrc != buf[len+4])
                    {
                        usleep(10);
                    }
                    else
                    {
                        rtn  = len+4;
                        return rtn;
                    }
                }
                else
                {
                    TESAM_FMT_DEBUG("TESAM data read len error, len:%d, max:%d, bRtnCmd:%04x \n", len, maxlen, bRtnCmd);
                    return -1;
                }

                break;
            }
            case RTN_ERR_INS:         //0x6D00 INS不支持
            case RTN_ERR_CLA:         //0x6E00 CLA不支持
            case RTN_ERR_P1P2:         //0x6A86 P1P2不正确
            case RTN_ERR_LC_LEN:    //0x6700 LC长度错误
            case RTN_ERR_SAVE:         //0x6581 存储器故障
            case RTN_ERR_COUN:         //0x6901 计数器不正确
            case RTN_ERR_RADN:         //0x6903 随机数无效
            case RTN_ERR_OUT:         //0x6904 外部认证错误
            case RTN_ERR_BAG:         //0x6905 数据包序号错
            case RTN_ERR_TIMER:        //0x6907 TIMER超时
            case RTN_ERR_STRU:         //0x6981 命令与文件结构不兼容
            case RTN_ERR_DAL:         //0x6982 会话未建立
            case RTN_ERR_COND:         //0x6985 使用条件不满足
            case RTN_ERR_CACL:         //0x6988 计算错误
            case RTN_ERR_MAC:         //0x6989 MAC校验错
            case RTN_ERR_DATA:         //0x6A80 不正确的数据域
            case RTN_ERR_ENN:         //0x6A81 功能不支持
            case RTN_ERR_FILE:         //0x6A82 文件未找到
            case RTN_ERR_NONE_SA:     //0x6A 84  无足够的文件存储空间
            case RTN_ERR_KEY:         //0x6A 88 密钥未找到
            {
                #if 1
                    TESAM_FMT_DEBUG("芯片返回错误代码 bRtnCmd = %04x \n", bRtnCmd);
                #endif
                bTimes = 4;
                rtn = -1;
                break;

            }
            case RTN_ERR_LRC:         //0x6A 90 LRC校验错误
            {
                #if 1
                    TESAM_FMT_DEBUG("芯片返回错误代码 bRtnCmd = %04x \n",bRtnCmd);
                #endif
                rtn = -1;
                break;
            }
            default:
            {
                #if 1
                    TESAM_FMT_DEBUG("芯片返回错误代码 bRtnCmd = %04x \n",bRtnCmd);
                #endif
                rtn = -1;
                break;
            }
        }
    }

    return rtn;
}

/*
********************************************************************
所属功能单元：加密处理模块
功能描述    ：读配网加密芯片ID  芯片序列号组帧
输入参数    ：组帧缓冲区

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月7日
流程说明    ： 发送00 b0 99 05 00 02 00 08
                   返回： 9000+0008+ID
                      ID：芯片序列号8字节
*******************************************************************/
int  ADEsamSeriRead(uint8 *pcmdbuf)
{
    TESAM_FMT_DEBUG("ADEsamSeriRead() 读配网加密芯片ID  芯片序列号组帧 \n");
    int index = 0;

    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0xb0;
    pcmdbuf[index++] = 0x99;
    pcmdbuf[index++] = 0x05;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x02;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x08;
	pcmdbuf[index]  = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;
    return index;
}

/*
********************************************************************
所属功能单元：加密处理模块
功能描述    ：读配网加密芯片密钥版本组帧
输入参数    ：组帧缓冲区

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月7日
流程说明    ： 发送80 1A 00 00 00 00
                   返回： 9000+0001+KeyVersion
                      Keyversion：1字节
*******************************************************************/
int  ADEsamKeyVerRead(uint8 *pcmdbuf)
{//80 1A 00 00 00 00
    TESAM_FMT_DEBUG("ADEsamKeyVerRead() 读配网加密芯片密钥版本组帧 \n");
    int index = 0;

    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x1A;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
	pcmdbuf[index]  	= DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;
    return index;
}

/*
********************************************************************
所属功能单元：加密处理模块
功能描述    ：读配网加密芯片随机数组帧
输入参数    ：组帧缓冲区

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月7日
修改者      ：
修改说明    ：
修改日期    ：
*******************************************************************/
int  DAEsamRandomRead(uint8 *pcmdbuf)
{//00 84 00 08 00 00
    TESAM_FMT_DEBUG("DAEsamRandomRead() 读配网加密芯片随机数组帧 \n");
    int index = 0;

    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x84;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x08;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
	pcmdbuf[index]  	= DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：加密处理模块
功能描述    ：对主站/网关随机数做签名
输入参数    ：组帧缓冲区
输出参数    ：无
函数返回值  ：帧长度
作者        ：朴军海
创建日期    ：2019年8月7日
发送：80 16 00 80 00 08  +  R1   .
返回：9000+0048+R2+ Sign1
         R1：网关随机数，8字节
                R2：终端随机数，8字节
                Sign1：签名结果， 64字节
*******************************************************************/
int  DAEsamRandSign(uint8 *pcmdbuf,uint8 *pdatabuf)
{//80 16 00 80 lenH + lenL + R1
    TESAM_FMT_DEBUG("DAEsamRandSign() 对主站/网关随机数做签名 \n");
    int index = 0;
    int datalen = 0;

    datalen = (pdatabuf[1]<<8) + pdatabuf[2];
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x16;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = datalen>>8;
    pcmdbuf[index++] = datalen&0xff;
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;
    return index;
}

/**/
/*
********************************************************************
所属功能单元：加密处理模块
功能描述    ：验证主站签名结果
输入参数    ：cmdbuf 组帧缓冲区,databuf 输入数据缓冲区
                  adKID 签名密钥索引asKID  ,网关为5，主站为1，2，3，4
输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月7日
操作说明
            发送：80 18 00 +asKID+ 00 40 + Sign2
            返回：9000+0000
            Sign2：签名结果，64字节
*******************************************************************/

int DAEsamRandSignCheck(uint8 *pcmdbuf,uint8 *pdatabuf)
{//80 18 00 05 00 40 + Sign2

    int   index   = 0;
    uint8 adKID   = 0;
    int   datalen = 0;

    TESAM_FMT_DEBUG("DAEsamRandSignCheck() 验证主站签名结果 \n");
    adKID = pdatabuf[0];
    datalen = (pdatabuf[1]<<8) + pdatabuf[2];
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x18;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = adKID;
    pcmdbuf[index++] = datalen>>8;
    pcmdbuf[index++] = datalen&0xff;
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：加密处理模块
功能描述    ：读配网加密芯片证书组帧
输入参数    ：组帧缓冲区

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月7日
发送  80 30 01 00 00 00
*******************************************************************/
int  DAReadTerminalCert(uint8 *pcmdbuf)
{
    TESAM_FMT_DEBUG("DAReadTerminalCert() 读配网加密芯片证书组帧 \n");
    int index = 0;

    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x30;
    pcmdbuf[index++] = 0x01;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
	pcmdbuf[index]  	= DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：加密处理模块
功能描述    ：密钥更新组帧
输入参数    ：组帧缓冲区

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月8日
密钥更新或恢复命令头801C00  +asKID+Lc+ newKeyVersion +UpdateKeyData +Skeydata
密钥版本大于0 为更新： 更新命令头：        801C00
asKID：                 签名密钥索引，  1字节
Lc：                       发送数据长度，    2字节
newKeyVersion：                 密钥版本，         1字节
UpdateKeyData：               密钥数据，        5*36字节
Skeydata：                  签名结果，        64字节
                                    数据总长245字节
*******************************************************************/
int DAEsamKeyUpdata(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAEsamKeyUpdata() 密钥更新组帧 \n");
    int     index = 0;
    uint8   adKID = 0;
    int     datalen = 0;

    adKID = pdatabuf[0];
    datalen = (pdatabuf[1]<<8) + pdatabuf[2];
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x1C;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = adKID;
    pcmdbuf[index++] = datalen>>8;
    pcmdbuf[index++] = datalen&0xff;//共245个字节
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：加密处理模块
功能描述    ：密钥恢复组帧
输入参数    ：组帧缓冲区

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月8日
密钥更新或恢复命令头  80 1C 01 +asKID+Lc+ newKeyVersion +UpdateKeyData +Skeydata
密钥版本大于0 为更新： 更新命令头：        801C01
asKID：                    签名密钥索引，  1字节
Lc：                       发送数据长度，    2字节
newKeyVersion：                 密钥版本，         1字节
UpdateKeyData：               密钥数据，        5*36字节
Skeydata：                  签名结果，        64字节
                                    数据总长245字节
*******************************************************************/
int DAEsamKeyReback(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAEsamKeyReback() 密钥恢复组帧 \n");
    int     index     = 0;
    uint8   asKID     = 0;
    int     datalen = 0;

    asKID = pdatabuf[0];
    datalen = (pdatabuf[1]<<8) + pdatabuf[2];
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x1C;
    pcmdbuf[index++] = 0x01;
    pcmdbuf[index++] = asKID;
    pcmdbuf[index++] = datalen>>8;
    pcmdbuf[index++] = datalen&0xff;//共245个字节
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;
    return index;
}

/*
********************************************************************
所属功能单元：加密处理模块
功能描述    ：管理工具下发密钥恢复组帧
输入参数    ：组帧缓冲区

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月13日

发送:  80 1C 02 00+Lc+ KeyData
*******************************************************************/
int DAToolKeyReback(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAToolKeyReback() 管理工具下发密钥恢复组帧 \n");
    int index = 0;
    int datalen = 0;

    datalen = (pdatabuf[1]<<8) + pdatabuf[2];
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x1C;
    pcmdbuf[index++] = 0x02;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = datalen>>8;
    pcmdbuf[index++] = datalen&0xff;//共185个字节
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：解密组帧
输入参数    ：pcmdbuf组帧缓冲区 Pdatabuf,需要解密的数据，前两个字节为长度

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月8日
发送:  80 2C 60 01 + Lc +  IVData + enTcData
            IVData 16字节，初始向量:  主站随机数 +  主站随机数按位取反
            enTcData  :需要解密的数据包含MAC
*******************************************************************/
int DAEsamDecipher(uint8 *pcmdbuf,uint8 *pdatabuf,uint8 *pRandNum)
{
    TESAM_FMT_DEBUG("DAEsamDecipher() 解密组帧 \n");
    int index = 0;
    int datalen = 0;
    uint8 i=0;

    datalen = (pdatabuf[1]<<8)  + pdatabuf[2];//长度低字节在后，高字节在前
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x2C;
    pcmdbuf[index++] = 0x60;
    pcmdbuf[index++] = 0x01;
    pcmdbuf[index++] = (datalen+16)>>8;  //共sendlen个字节,长度高字节
    pcmdbuf[index++] = (datalen+16)&0xff;//共sendlen个字节,长度低字节

    for(i=0;i<8;i++)
    {
        pcmdbuf[index]   = pRandNum[i];
        pcmdbuf[index+8] = ~pRandNum[i];
        index++;
    }

    index += 8;
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：加密组帧
输入参数    ：pcmdbuf组帧缓冲区 Pdatabuf,需要加密的数据，前两个字节为长度

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月8日
发送:  80 26 60 01 + Lc +  IVData + enTcData
            IVData 16字节，初始向量:  主站随机数 +  主站随机数按位取反
            enTcData  :需要加密的数据
*******************************************************************/
int DAEsamEncryte (uint8 *pcmdbuf,uint8 *pdatabuf,uint8 *pRandNum)
{
    TESAM_FMT_DEBUG("DAEsamEncryte() 加密组帧 \n");
    int index = 0;
    int datalen = 0;
    uint8 i=0;

    datalen = (pdatabuf[1]<<8) + pdatabuf[2];//长度低字节在后，高字节在前

    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x26;
    pcmdbuf[index++] = 0x60;
    pcmdbuf[index++] = 0x01;
    pcmdbuf[index++] = (datalen+16)>>8;    //共sendlen个字节,长度高字节
    pcmdbuf[index++] = (datalen+16)&0xff;//共sendlen个字节,长度低字节

    for(i=0;i<8;i++)
    {
        pcmdbuf[index]   =  pRandNum[i];
        pcmdbuf[index+8] = ~pRandNum[i];
        index++;
    }

    index += 8;
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：对主站下发的证书验证组帧
输入参数    ：pcmdbuf组帧缓冲区 Pdatabuf,需要加密的数据，前两个字节为长度

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月8日
发送:  80 08 00 + asKID + Lc + CerID + Cer+Date+Scer
*******************************************************************/
int DAEsamIinfoSignCheck(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAEsamIinfoSignCheck() 对主站下发的证书验证组帧 \n");
    int   index   = 0;
    uint8 asKID   = 0;
    int   datalen = 0;

    asKID   = pdatabuf[0];
    datalen = (pdatabuf[1]<<8) + pdatabuf[2];
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x08;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = asKID;
    pcmdbuf[index++] = datalen>>8;
    pcmdbuf[index++] = datalen&0xff;
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：CA根证书/主站/网关证书更新组帧
输入参数    ：pcmdbuf组帧缓冲区 Pdatabuf,需要加密的数据，前两个字节为长度

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月8日
发送:  80 22+cerID+00+Lc+Cer  :
                         cerID---CA,根证书0，主站证书1~4，5 网关 证书
*******************************************************************/
int DAEsamHostCertUpdata(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAEsamHostCertUpdata() CA根证书/主站/网关证书更新组帧 \n");
    int   index   = 0;
    uint8 cerID   = 0;
    int   datalen = 0;

    cerID   = pdatabuf[0];
    datalen = (pdatabuf[1]<<8) + pdatabuf[2];
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x22;
    pcmdbuf[index++] = cerID;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = datalen>>8;
    pcmdbuf[index++] = datalen&0xff;
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
    pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
    index++;

    return index;
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：写入终端证书
输入参数    ：pcmdbuf组帧缓冲区 Pdatabuf,需要加密的数据，前两个字节为长度

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月8日
发送:  80 24 00 00 +Lc+Cer
*******************************************************************/
int DAWriteEsamMtuCert(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAWriteEsamMtuCert() 写入终端证书 \n");
    int index = 0;
    int datalen = 0;

    datalen = (pdatabuf[1]<<8) + pdatabuf[2];
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x24;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = datalen>>8;
    pcmdbuf[index++] = datalen&0xff;
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;
    return index;
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：读取终端测试证书长度组帧
输入参数    ：

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月12日
发送：00 B0 81 00 00 02 00 02
返回：9000+0000+Len
               Len：2个字节
*******************************************************************/
int DAEsamReadMtuTCertLen(uint8 *pcmdbuf)
{
    TESAM_FMT_DEBUG("DAEsamReadMtuTCertLen() 读取终端测试证书长度组帧 \n");
    int index = 0;

    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0xB0;
    pcmdbuf[index++] = 0x81;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x02;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x02;
	pcmdbuf[index]  = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;
    return index;
}

/*
特别注意  " 终端证书的回写"
    当做完密钥恢复后，对称密钥恢复到测试态，终端证书会被清掉；需要从芯片备
份区，将测试的终端证书回写到证书读写区。回写终端测试证书时，确保已安装
测试的CA根证书。
步骤
    1. 读取终端测试证书的长度
    2. 读取终端测试证书
    3. 写入终端证书
*/
/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：读取终端测试证书长度组帧
输入参数    ：

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月12日
发送： 00 B0 81 02 00 02+Len
*******************************************************************/
int DAEsamReadMtuTCert(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAEsamReadMtuTCert() 读取终端测试证书长度组帧 \n");
    int index = 0;

    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0xB0;
    pcmdbuf[index++] = 0x81;
    pcmdbuf[index++] = 0x02;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x02;
    pcmdbuf[index++] = pdatabuf[1];
    pcmdbuf[index++] = pdatabuf[2];
	pcmdbuf[index]  = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;
    return index;
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：验证证书管理工具证书的有效性组帧
输入参数    ：

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月12日
发送：    80 1E 00 00 + Lc + Cer    Lc：表示长度，2字节
返回：    9000+0000

*******************************************************************/
int DAToolCertCheck(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAToolCertCheck() 验证证书管理工具证书的有效性组帧 \n");
    int index   = 0;
    int datalen = 0;

    datalen = (pdatabuf[1]<<8) + pdatabuf[2];
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x1E;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = datalen>>8;
    pcmdbuf[index++] = datalen&0xff;
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index]	= DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：对信息签名组帧
输入参数    ：

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月13日
发送：    80 0A 00 80 + Lc+ TotalReqData    Lc: 2字节，要签名的数据TotalReqData
成功返回： 9000+0040+ SrReq     对SrReq：对TotalReqData 签名结果
*******************************************************************/
int DAEsamIinfoSign(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAEsamIinfoSign() 对信息签名组帧 \n");
    int index   = 0;
    int datalen = 0;

    datalen = (pdatabuf[1]<<8) + pdatabuf[2];
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x0A;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = pdatabuf[1];
    pcmdbuf[index++] = pdatabuf[2];
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index]	= DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：对管理工具随机数签名的验签
输入参数    ：

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月12日
发送：    80 20 00 00 00 40 +Sr1
返回：    9000+0000
*******************************************************************/
int DAToolRandSignCheck(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAToolRandSignCheck() 对管理工具随机数签名的验签 \n");
    int index   = 0;
    int datalen = 0;

    datalen = pdatabuf[2] +  (pdatabuf[1]<<8);
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x20;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x40;
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index]	= DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：读取公钥组帧
输入参数    ：

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月12日
发送：    80 30 00 00 00 00
返回：    9000+0040+pubKey
*******************************************************************/

int DAEsamReadPublicKey(uint8 *pcmdbuf)
{
    TESAM_FMT_DEBUG("DAEsamReadPublicKey() 读取公钥组帧 \n");
    int index = 0;

    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x30;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = 0x00;
	pcmdbuf[index]  = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：管理工具通讯加密组帧
输入参数    ：pcmdbuf组帧缓冲区 Pdatabuf,需要加密的数据，前两个字节为长度

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月13日
发送:  80266202+Lc+ID+R1+IVData+ UpData
            ID 管理证书工具ID，R1终端随机数，
            IDIVData 16字节，初始向量:  主站随机数 +    主站随机数按位取反
            UpData  :需要加密的数据
*******************************************************************/
int DAToolEncryte (uint8 *pcmdbuf,uint8 *pdatabuf,uint8 *pRandNum)
{
    TESAM_FMT_DEBUG("DAToolEncryte() 管理工具通讯加密组帧 \n");
    int   index   = 0;
    int   datalen = 0;
    uint8 i       = 0;

    datalen = (pdatabuf[1]<<8) + pdatabuf[2];//长度低字节在后，高字节在前

    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x26;
    pcmdbuf[index++] = 0x62;
    pcmdbuf[index++] = 0x02;
    pcmdbuf[index++] = (datalen+16)>>8;    //共sendlen个字节,长度高字节
    pcmdbuf[index++] = (datalen+16)&0xff;//共sendlen个字节,长度低字节
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],16);//ID + R1
    index += 16;
    for(i=0;i<8;i++)
    {
        pcmdbuf[index]   =  pRandNum[i];
        pcmdbuf[index+8] = ~pRandNum[i];
        index++;
    }
    index += 8;
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3+16],datalen-16);
    index += (datalen-16);
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：与证书管理工具通讯解密组帧
输入参数    ：pcmdbuf组帧缓冲区 Pdatabuf,需要解密的数据，前两个字节为长度

输出参数    ：无

函数返回值  ：帧长度

作者        ：朴军海
创建日期    ：2019年8月13日
发送:  80 2C 62 02 + Lc+ ID+R1+IVData+ enReqData+MAC
    Lc：表示长度，2字节 ID：证书管理工具ID，8字节；
    R1：随机数，身份认证时终端芯片产生的随机数，8字节
    IVData：初始向量，随机数R1+对R1按位取反，16字节
    enReqData：证书请求数据密文，对每一帧都需要做解密
*******************************************************************/
int DAToolDecipher(uint8 *pcmdbuf,uint8 *pdatabuf,uint8 *pRandNum)
{
    TESAM_FMT_DEBUG("DAToolDecipher() 与证书管理工具通讯解密组帧 \n");
    int   index   = 0;
    int   datalen = 0;
    uint8 i       = 0;

    datalen = (pdatabuf[1]<<8)  + pdatabuf[2];//长度低字节在后，高字节在前
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x2C;
    pcmdbuf[index++] = 0x62;
    pcmdbuf[index++] = 0x02;
    pcmdbuf[index++] = (datalen+16)>>8;  //+ 16为初始向量长度,共sendlen个字节,长度高字节
    pcmdbuf[index++] = (datalen+16)&0xff;//共sendlen个字节,长度低字节
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],16);//ID + R1
    index += 16;
    for(i=0;i<8;i++)
    {
        pcmdbuf[index]   = pRandNum[i];
        pcmdbuf[index+8] = ~pRandNum[i];
        index++;
    }

    index += 8;
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3+16],datalen-16);
    index += (datalen-16);
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：打印配网加密芯片返回数据
输入参数    ：pBuf要打印的数据缓冲区
输出参数    ：无
函数返回值  ：帧长度
作者        ：朴军海
创建日期    ：2019年8月13日
*******************************************************************/
void DAEsamRecvPrint(uint8 *pBuf)
{
    int len = 0;
    int i = 0;

    if(getenv("DEBUG_DAESAM") != NULL)
    {
        len = (pBuf[2]<<8) + pBuf[3] + 4;
        printf("tesam recv len = %d \n",len);

        for(i=0;i<len;i++)
        {
            printf("%02X ",pBuf[i]);
        }

        printf("\n");
    }
}

/*
********************************************************************
所属功能单元：配网加/解密处理模块
功能描述    ：打印配网加密芯片返回数据
输入参数    ：pBuf要打印的数据缓冲区
输出参数    ：无
函数返回值  ：帧长度
作者        ：朴军海
创建日期    ：2019年8月13日
*******************************************************************/
void DAEsamSendvPrint(uint8 *pBuf,int len)
{
    int i = 0;

    if(getenv("DEBUG_DAESAM") != NULL)
    {
        printf("tesam send len = %d \n",len);

        for(i=0;i<len;i++)
        {
            printf("%02X ",pBuf[i]);
        }
        printf("\n");
    }
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

/*
********************************************************************
所属功能单元：加密处理模块
功能描述    ：与加密芯片通讯命令执行逻辑
输入参数    ：bType， 命令类型rcvbuf, 加密芯片返回数据缓冲区
                    向sndbuf 加密芯片发送数据组帧缓冲区
                        databuf，输入相关数据缓冲区，
                        databuf[0]，关键字
                        databuf[1]，输入数据长度高字节
                        databuf[2]，输入数据长度低字节
输出参数    ：无

函数返回值  ：成功返回加密芯片的返回数据长度，失败返回-1

作者        ：朴军海
创建日期    ：2019年8月7日
修改者      ：
修改说明    ：
修改日期    ：
*******************************************************************/
int DAEsamCommand(uint8 bType,uint8 *rcvbuf,uint8 *sndbuf,uint8 *databuf,uint8 *pRandNum)
{
    uint16 bSendLen = 0;
    int    len2 = 0;
    
    switch(bType)
    {
        case DA_ESAM_SERI_READ://读芯片序列号 ID
        {
            bSendLen = ADEsamSeriRead(sndbuf);
            break;
        }
        case DA_ESAM_KEY_VER://读取芯片密钥版本1B
        {
            bSendLen = ADEsamKeyVerRead(sndbuf);
            break;
        }
        case DA_ESAM_RAND_READ://读配网加密芯片随机数
        {
            bSendLen = DAEsamRandomRead(sndbuf);
            break;
        }
        case DA_ESAM_RAND_SIGN://对随机数进行签名
        {
            bSendLen =  DAEsamRandSign(sndbuf,databuf);
            break;
        }
        case DA_ESAM_RAND_SIGN_CHECK://对随机数签名进行验证
        {
            bSendLen =  DAEsamRandSignCheck(sndbuf,databuf);
            break;
        }
        case DA_ESAM_KYE_UPDATA://密钥更新
        {
            bSendLen = DAEsamKeyUpdata((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_ESAM_KYE_REBACK://密钥恢复
        {
            bSendLen = DAEsamKeyReback((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_ESAM_ENCRYPTE://加密处理
        {
            bSendLen =  DAEsamEncryte((uint8 *)sndbuf,(uint8 *)databuf,(uint8 *)pRandNum);
            break;
        }
        case DA_ESAM_DECIPHER://解密处理
        {
            bSendLen =  DAEsamDecipher((uint8 *)sndbuf,(uint8 *)databuf,(uint8 *)pRandNum);
            break;
        }
        case DA_ESAM_INFO_SIGN://对信息签名组帧
        {
            bSendLen =  DAEsamIinfoSign((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_ESAM_INFO_SIGN_CHECK://对信息签名进行验证
        {
            bSendLen =    DAEsamIinfoSignCheck((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_ESAM_MTU_CERT_GET://终端证书读取
        {
            bSendLen = DAReadTerminalCert(sndbuf);
            break;
        }
        case DA_ESAM_MTU_CERT_UPDATA://终端证书更新
        {
            bSendLen =    DAWriteEsamMtuCert((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }

        case DA_ESAM_HOST_CERT_UPDATA://主站/网关证书更新
        {
            bSendLen =    DAEsamHostCertUpdata((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_ESAM_READ_MTU_CERTLEN://读取终端测试证书长度组帧
        {
            bSendLen =    DAEsamReadMtuTCertLen((uint8 *)sndbuf);
            break;
        }
        case DA_ESAM_READ_MTU_CERT://读取终端测试证书长度组帧
        {
            bSendLen =    DAEsamReadMtuTCert((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_TOO_CERT_CHECK://验证证书管理工具证书的有效性组帧
        {
            bSendLen = DAToolCertCheck((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_TOOL_RAND_SIGN_CHECK://对管理工具随机数签名的验签
        {
            bSendLen = DAToolRandSignCheck((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_ESAM_READ_PUBLIC_KEY://读取公钥
        {
            bSendLen = DAEsamReadPublicKey((uint8 *)sndbuf);
            break;
        }
        case DA_TOOL_ENCRYPTE://与证书管理工具通讯加密处理 0xb8
        {
            bSendLen =  DAToolEncryte((uint8 *)sndbuf,(uint8 *)databuf,(uint8 *)pRandNum);
            break;
        }
        case DA_TOOL_DECIPHER://与证书管理工具通讯解密处理 0xb9
        {
            bSendLen =  DAToolDecipher((uint8 *)sndbuf,(uint8 *)databuf,(uint8 *)pRandNum);
            break;
        }
        case DA_TOOL_KEY_REBACK://管理证书工具下发的密钥恢复处理
        {
            bSendLen =  DAToolKeyReback((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        default:
        {
            bSendLen = 0;
            len2 = -1;
            break;
        }
    }

    TESAM_FMT_DEBUG("DAEsamCommand() bType=%02x, bSendLen=%d \n", bType, bSendLen);

    /* 通过判断是否存在安全加固APP通信文件 */
    if(0 == access(SECURITY_APP_DEST, F_OK))
    {
        TESAM_FMT_DEBUG("if(0 == access(SECURITY_APP_DEST, F_OK)) \n");
        
        uint32 i = 0;
        int    ret = 0;
        unsigned int auth_len = 64;
        char sender[SECURITY_FILEPATH_LEN] = {0};
        uint16 msglen = 0;

        uint8 sendmsg[4096] = {0};
        uint8 auth_data[64] = {0};
        uint8 auth_time[8] = {0};

        if(g_UDPEASMid <= 0)
        {
            char filename[SECURITY_FILEPATH_LEN] = {0};
            sprintf(filename, "%s/%s_esam", SECURITY_MSG_PATH, "pdAmr");
            g_UDPEASMid = task_pfmsg_init(filename);
            if(0 != TESAM_AUTH_FLOW(&g_UDPEASMid))
            {
                return -1;
            }
            pthread_mutex_init(&g_fUdpEsamLock, NULL);
        }
        if(g_UDPEASMid <= 0)
        {
            TESAM_FMT_DEBUG("init failed\n");
            return 0;
        }

        /* 尝试加锁 */
        while(0 != pthread_mutex_trylock(&g_fUdpEsamLock))
        {
            if(i++ > 500)
            {
                TESAM_FMT_DEBUG("TESAM SEND lock faild\n");
                return 0;
            }
            usleep(10000);
        }

        i = 0;
        /* 1. 1ms内收到消息, 等待0.1s的时间清空队列 */
        task_msg_settime(g_UDPEASMid, 0, 1000);
        ret = task_pfmsg_recv(&g_UDPEASMid, rcvbuf, DA_ESAM_MAX_LEN, sender, SECURITY_FILEPATH_LEN);
        if(ret > 0)
        {
            while(i < 100)
            {
                i++;
                task_pfmsg_recv(&g_UDPEASMid, rcvbuf, DA_ESAM_MAX_LEN, sender, SECURITY_FILEPATH_LEN);
            }
        }

        ret = sdt_msg_auth(sndbuf, bSendLen, g_Esam_App_id, 24, g_Esam_AC_test, 16, auth_data, &auth_len, auth_time);
        
        memcpy(sendmsg, sndbuf, bSendLen);
        memcpy(sendmsg + bSendLen, g_Esam_App_id, 24);
        memcpy(sendmsg + bSendLen + 24, auth_data, 64);
        memcpy(sendmsg + bSendLen + 24 + 64, auth_time, 8);
     
        msglen = bSendLen + 24 + 64 + 8;

        /* 2. 发送消息 */
        TESAM_BUF_DEBUG(sendmsg, msglen, "Esam send msg");
        ret = task_pfmsg_send(g_UDPEASMid, sendmsg, msglen, SECURITY_APP_DEST);
        if(0 != ret)
        {
            pthread_mutex_unlock(&g_fUdpEsamLock);
            TESAM_FMT_DEBUG("TESAM send faild, ret %d\n", ret);
            return 0;
        }

        /* 3. 接收 2s超时 */
        task_msg_settime(g_UDPEASMid, 2, 0);
        len2 = task_pfmsg_recv(&g_UDPEASMid, rcvbuf, DA_ESAM_MAX_LEN, sender, SECURITY_FILEPATH_LEN);
        pthread_mutex_unlock(&g_fUdpEsamLock);
        if(len2 <= 0)
        {
            TESAM_FMT_DEBUG("TESAM recv faild, ret %d\n", len2);
            return 0;
        }
    }
    else
    {
        int    len1 = 0;
        uint32 delayus = 100000;

        ESAM_DEVICE_T* pDev = NULL;

        if(DA_TOOL_ENCRYPTE == bType || DA_ESAM_ENCRYPTE == bType)
        {
            delayus = 500000;
        }

        pDev = (ESAM_DEVICE_T*)hal_device_get(HW_DEVICE_ID_PW_ESAM);

        if(pDev == NULL)
        {
            TESAM_FMT_DEBUG("TESAM open failed \n");
            return -1;
        }

        if (pDev->esam_set_lock(pDev, 1) != 0)
        {
            TESAM_FMT_DEBUG("TESAM lock failed \n");
            goto CLOSE_DEV;
        }

        if(bSendLen)
        {
            len1 = DAEsamWrite(pDev,sndbuf,bSendLen);
            TESAM_FMT_DEBUG("DAEsamWrite() 发送结果:%d \n", len1);
            usleep(delayus);//防止读不到数据，统一延时2ms
            if(len1 > 0)
            {
                TESAM_BUF_DEBUG(sndbuf, bSendLen, "发送报文: ");
                len2 = DAEsamRead(pDev, rcvbuf, DA_ESAM_MAX_LEN);
                TESAM_FMT_DEBUG("DAEsamRead() 接收结果:%d \n", len2);
                if(len2 > 0)
                {
                    TESAM_BUF_DEBUG(rcvbuf, len2, "接收报文: ");
                }
            }
        }

        pDev->esam_set_lock(pDev, 0);

CLOSE_DEV:
        len1 = hal_device_release(&pDev->base);
        TESAM_FMT_DEBUG("hal_device_release(), ret:%d\n", len1);
        if (len1 != 0)
        {
            TESAM_FMT_DEBUG("TESAM release failed,ret:%d\n", len1);
        }
    }

    return len2;
}

