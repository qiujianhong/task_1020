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
#define ESAM_BUFF_LEN           4096                //ESAM��������������

int     g_UDPEASMid = 0;
pthread_mutex_t g_fUdpEsamLock;

/* ��ʱʹ�õ�appid��У�� */
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
        printf("���ݳ���len: %d / ", len);

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
�������ܵ�Ԫ�����ܴ���ģ��
��������    ��EsamͨѶУ��ͼ���
�������    ��У�����ݻ�������len У�黺�������ݳ���
�������    ����
��������ֵ  ��У���

����        ���Ӿ���
��������    ��2019��8��7��
�޸���      ��
�޸�˵��    ��
�޸�����    ��
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
�������ܵ�Ԫ�����ܴ���ģ��
��������    ��д���ݵ�����оƬ
�������    ��pDev �豸ָ�룬buf д�����ݻ�������len д�����ݳ���

�������    ����

��������ֵ  ���ɹ�����д�����ݳ��ȣ�ʧ�ܷ���-1

����        ���Ӿ���
��������    ��2019��8��7��
�޸���      ��
�޸�˵��    ��
�޸�����    ��
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
�������ܵ�Ԫ�����ܴ���ģ��
��������    ���Ӽ���оƬ��ȡ��������
�������    ��pDev �豸ָ�룬maxlen �����ݻ�������󳤶�

�������    ��buf�����ݻ��������������ݲ���У���

��������ֵ  ���ɹ�����д�����ݳ��ȣ�ʧ�ܷ���-1

����        ���Ӿ���
��������    ��2019��8��7��
�޸���      ��
�޸�˵��    ��
�޸�����    ��
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
            case RTN_SUCCESS ://0x9000 �ɹ�
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
            case RTN_ERR_INS:         //0x6D00 INS��֧��
            case RTN_ERR_CLA:         //0x6E00 CLA��֧��
            case RTN_ERR_P1P2:         //0x6A86 P1P2����ȷ
            case RTN_ERR_LC_LEN:    //0x6700 LC���ȴ���
            case RTN_ERR_SAVE:         //0x6581 �洢������
            case RTN_ERR_COUN:         //0x6901 ����������ȷ
            case RTN_ERR_RADN:         //0x6903 �������Ч
            case RTN_ERR_OUT:         //0x6904 �ⲿ��֤����
            case RTN_ERR_BAG:         //0x6905 ���ݰ���Ŵ�
            case RTN_ERR_TIMER:        //0x6907 TIMER��ʱ
            case RTN_ERR_STRU:         //0x6981 �������ļ��ṹ������
            case RTN_ERR_DAL:         //0x6982 �Ựδ����
            case RTN_ERR_COND:         //0x6985 ʹ������������
            case RTN_ERR_CACL:         //0x6988 �������
            case RTN_ERR_MAC:         //0x6989 MACУ���
            case RTN_ERR_DATA:         //0x6A80 ����ȷ��������
            case RTN_ERR_ENN:         //0x6A81 ���ܲ�֧��
            case RTN_ERR_FILE:         //0x6A82 �ļ�δ�ҵ�
            case RTN_ERR_NONE_SA:     //0x6A 84  ���㹻���ļ��洢�ռ�
            case RTN_ERR_KEY:         //0x6A 88 ��Կδ�ҵ�
            {
                #if 1
                    TESAM_FMT_DEBUG("оƬ���ش������ bRtnCmd = %04x \n", bRtnCmd);
                #endif
                bTimes = 4;
                rtn = -1;
                break;

            }
            case RTN_ERR_LRC:         //0x6A 90 LRCУ�����
            {
                #if 1
                    TESAM_FMT_DEBUG("оƬ���ش������ bRtnCmd = %04x \n",bRtnCmd);
                #endif
                rtn = -1;
                break;
            }
            default:
            {
                #if 1
                    TESAM_FMT_DEBUG("оƬ���ش������ bRtnCmd = %04x \n",bRtnCmd);
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
�������ܵ�Ԫ�����ܴ���ģ��
��������    ������������оƬID  оƬ���к���֡
�������    ����֡������

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��7��
����˵��    �� ����00 b0 99 05 00 02 00 08
                   ���أ� 9000+0008+ID
                      ID��оƬ���к�8�ֽ�
*******************************************************************/
int  ADEsamSeriRead(uint8 *pcmdbuf)
{
    TESAM_FMT_DEBUG("ADEsamSeriRead() ����������оƬID  оƬ���к���֡ \n");
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
�������ܵ�Ԫ�����ܴ���ģ��
��������    ������������оƬ��Կ�汾��֡
�������    ����֡������

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��7��
����˵��    �� ����80 1A 00 00 00 00
                   ���أ� 9000+0001+KeyVersion
                      Keyversion��1�ֽ�
*******************************************************************/
int  ADEsamKeyVerRead(uint8 *pcmdbuf)
{//80 1A 00 00 00 00
    TESAM_FMT_DEBUG("ADEsamKeyVerRead() ����������оƬ��Կ�汾��֡ \n");
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
�������ܵ�Ԫ�����ܴ���ģ��
��������    ������������оƬ�������֡
�������    ����֡������

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��7��
�޸���      ��
�޸�˵��    ��
�޸�����    ��
*******************************************************************/
int  DAEsamRandomRead(uint8 *pcmdbuf)
{//00 84 00 08 00 00
    TESAM_FMT_DEBUG("DAEsamRandomRead() ����������оƬ�������֡ \n");
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
�������ܵ�Ԫ�����ܴ���ģ��
��������    ������վ/�����������ǩ��
�������    ����֡������
�������    ����
��������ֵ  ��֡����
����        ���Ӿ���
��������    ��2019��8��7��
���ͣ�80 16 00 80 00 08  +  R1   .
���أ�9000+0048+R2+ Sign1
         R1�������������8�ֽ�
                R2���ն��������8�ֽ�
                Sign1��ǩ������� 64�ֽ�
*******************************************************************/
int  DAEsamRandSign(uint8 *pcmdbuf,uint8 *pdatabuf)
{//80 16 00 80 lenH + lenL + R1
    TESAM_FMT_DEBUG("DAEsamRandSign() ����վ/�����������ǩ�� \n");
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
�������ܵ�Ԫ�����ܴ���ģ��
��������    ����֤��վǩ�����
�������    ��cmdbuf ��֡������,databuf �������ݻ�����
                  adKID ǩ����Կ����asKID  ,����Ϊ5����վΪ1��2��3��4
�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��7��
����˵��
            ���ͣ�80 18 00 +asKID+ 00 40 + Sign2
            ���أ�9000+0000
            Sign2��ǩ�������64�ֽ�
*******************************************************************/

int DAEsamRandSignCheck(uint8 *pcmdbuf,uint8 *pdatabuf)
{//80 18 00 05 00 40 + Sign2

    int   index   = 0;
    uint8 adKID   = 0;
    int   datalen = 0;

    TESAM_FMT_DEBUG("DAEsamRandSignCheck() ��֤��վǩ����� \n");
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
�������ܵ�Ԫ�����ܴ���ģ��
��������    ������������оƬ֤����֡
�������    ����֡������

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��7��
����  80 30 01 00 00 00
*******************************************************************/
int  DAReadTerminalCert(uint8 *pcmdbuf)
{
    TESAM_FMT_DEBUG("DAReadTerminalCert() ����������оƬ֤����֡ \n");
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
�������ܵ�Ԫ�����ܴ���ģ��
��������    ����Կ������֡
�������    ����֡������

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��8��
��Կ���»�ָ�����ͷ801C00  +asKID+Lc+ newKeyVersion +UpdateKeyData +Skeydata
��Կ�汾����0 Ϊ���£� ��������ͷ��        801C00
asKID��                 ǩ����Կ������  1�ֽ�
Lc��                       �������ݳ��ȣ�    2�ֽ�
newKeyVersion��                 ��Կ�汾��         1�ֽ�
UpdateKeyData��               ��Կ���ݣ�        5*36�ֽ�
Skeydata��                  ǩ�������        64�ֽ�
                                    �����ܳ�245�ֽ�
*******************************************************************/
int DAEsamKeyUpdata(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAEsamKeyUpdata() ��Կ������֡ \n");
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
    pcmdbuf[index++] = datalen&0xff;//��245���ֽ�
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
�������ܵ�Ԫ�����ܴ���ģ��
��������    ����Կ�ָ���֡
�������    ����֡������

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��8��
��Կ���»�ָ�����ͷ  80 1C 01 +asKID+Lc+ newKeyVersion +UpdateKeyData +Skeydata
��Կ�汾����0 Ϊ���£� ��������ͷ��        801C01
asKID��                    ǩ����Կ������  1�ֽ�
Lc��                       �������ݳ��ȣ�    2�ֽ�
newKeyVersion��                 ��Կ�汾��         1�ֽ�
UpdateKeyData��               ��Կ���ݣ�        5*36�ֽ�
Skeydata��                  ǩ�������        64�ֽ�
                                    �����ܳ�245�ֽ�
*******************************************************************/
int DAEsamKeyReback(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAEsamKeyReback() ��Կ�ָ���֡ \n");
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
    pcmdbuf[index++] = datalen&0xff;//��245���ֽ�
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;
    return index;
}

/*
********************************************************************
�������ܵ�Ԫ�����ܴ���ģ��
��������    ���������·���Կ�ָ���֡
�������    ����֡������

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��13��

����:  80 1C 02 00+Lc+ KeyData
*******************************************************************/
int DAToolKeyReback(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAToolKeyReback() �������·���Կ�ָ���֡ \n");
    int index = 0;
    int datalen = 0;

    datalen = (pdatabuf[1]<<8) + pdatabuf[2];
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x1C;
    pcmdbuf[index++] = 0x02;
    pcmdbuf[index++] = 0x00;
    pcmdbuf[index++] = datalen>>8;
    pcmdbuf[index++] = datalen&0xff;//��185���ֽ�
    memcpy((char *)&pcmdbuf[index],(char *)&pdatabuf[3],datalen);
    index += datalen;
	pcmdbuf[index] = DACalEsamLrc(&pcmdbuf[1],index-1);
	index++;

    return index;
}

/*
********************************************************************
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ��������֡
�������    ��pcmdbuf��֡������ Pdatabuf,��Ҫ���ܵ����ݣ�ǰ�����ֽ�Ϊ����

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��8��
����:  80 2C 60 01 + Lc +  IVData + enTcData
            IVData 16�ֽڣ���ʼ����:  ��վ����� +  ��վ�������λȡ��
            enTcData  :��Ҫ���ܵ����ݰ���MAC
*******************************************************************/
int DAEsamDecipher(uint8 *pcmdbuf,uint8 *pdatabuf,uint8 *pRandNum)
{
    TESAM_FMT_DEBUG("DAEsamDecipher() ������֡ \n");
    int index = 0;
    int datalen = 0;
    uint8 i=0;

    datalen = (pdatabuf[1]<<8)  + pdatabuf[2];//���ȵ��ֽ��ں󣬸��ֽ���ǰ
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x2C;
    pcmdbuf[index++] = 0x60;
    pcmdbuf[index++] = 0x01;
    pcmdbuf[index++] = (datalen+16)>>8;  //��sendlen���ֽ�,���ȸ��ֽ�
    pcmdbuf[index++] = (datalen+16)&0xff;//��sendlen���ֽ�,���ȵ��ֽ�

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
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ��������֡
�������    ��pcmdbuf��֡������ Pdatabuf,��Ҫ���ܵ����ݣ�ǰ�����ֽ�Ϊ����

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��8��
����:  80 26 60 01 + Lc +  IVData + enTcData
            IVData 16�ֽڣ���ʼ����:  ��վ����� +  ��վ�������λȡ��
            enTcData  :��Ҫ���ܵ�����
*******************************************************************/
int DAEsamEncryte (uint8 *pcmdbuf,uint8 *pdatabuf,uint8 *pRandNum)
{
    TESAM_FMT_DEBUG("DAEsamEncryte() ������֡ \n");
    int index = 0;
    int datalen = 0;
    uint8 i=0;

    datalen = (pdatabuf[1]<<8) + pdatabuf[2];//���ȵ��ֽ��ں󣬸��ֽ���ǰ

    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x26;
    pcmdbuf[index++] = 0x60;
    pcmdbuf[index++] = 0x01;
    pcmdbuf[index++] = (datalen+16)>>8;    //��sendlen���ֽ�,���ȸ��ֽ�
    pcmdbuf[index++] = (datalen+16)&0xff;//��sendlen���ֽ�,���ȵ��ֽ�

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
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ������վ�·���֤����֤��֡
�������    ��pcmdbuf��֡������ Pdatabuf,��Ҫ���ܵ����ݣ�ǰ�����ֽ�Ϊ����

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��8��
����:  80 08 00 + asKID + Lc + CerID + Cer+Date+Scer
*******************************************************************/
int DAEsamIinfoSignCheck(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAEsamIinfoSignCheck() ����վ�·���֤����֤��֡ \n");
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
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ��CA��֤��/��վ/����֤�������֡
�������    ��pcmdbuf��֡������ Pdatabuf,��Ҫ���ܵ����ݣ�ǰ�����ֽ�Ϊ����

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��8��
����:  80 22+cerID+00+Lc+Cer  :
                         cerID---CA,��֤��0����վ֤��1~4��5 ���� ֤��
*******************************************************************/
int DAEsamHostCertUpdata(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAEsamHostCertUpdata() CA��֤��/��վ/����֤�������֡ \n");
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
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ��д���ն�֤��
�������    ��pcmdbuf��֡������ Pdatabuf,��Ҫ���ܵ����ݣ�ǰ�����ֽ�Ϊ����

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��8��
����:  80 24 00 00 +Lc+Cer
*******************************************************************/
int DAWriteEsamMtuCert(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAWriteEsamMtuCert() д���ն�֤�� \n");
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
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ����ȡ�ն˲���֤�鳤����֡
�������    ��

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��12��
���ͣ�00 B0 81 00 00 02 00 02
���أ�9000+0000+Len
               Len��2���ֽ�
*******************************************************************/
int DAEsamReadMtuTCertLen(uint8 *pcmdbuf)
{
    TESAM_FMT_DEBUG("DAEsamReadMtuTCertLen() ��ȡ�ն˲���֤�鳤����֡ \n");
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
�ر�ע��  " �ն�֤��Ļ�д"
    ��������Կ�ָ��󣬶Գ���Կ�ָ�������̬���ն�֤��ᱻ�������Ҫ��оƬ��
�����������Ե��ն�֤���д��֤���д������д�ն˲���֤��ʱ��ȷ���Ѱ�װ
���Ե�CA��֤�顣
����
    1. ��ȡ�ն˲���֤��ĳ���
    2. ��ȡ�ն˲���֤��
    3. д���ն�֤��
*/
/*
********************************************************************
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ����ȡ�ն˲���֤�鳤����֡
�������    ��

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��12��
���ͣ� 00 B0 81 02 00 02+Len
*******************************************************************/
int DAEsamReadMtuTCert(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAEsamReadMtuTCert() ��ȡ�ն˲���֤�鳤����֡ \n");
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
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ����֤֤�������֤�����Ч����֡
�������    ��

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��12��
���ͣ�    80 1E 00 00 + Lc + Cer    Lc����ʾ���ȣ�2�ֽ�
���أ�    9000+0000

*******************************************************************/
int DAToolCertCheck(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAToolCertCheck() ��֤֤�������֤�����Ч����֡ \n");
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
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ������Ϣǩ����֡
�������    ��

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��13��
���ͣ�    80 0A 00 80 + Lc+ TotalReqData    Lc: 2�ֽڣ�Ҫǩ��������TotalReqData
�ɹ����أ� 9000+0040+ SrReq     ��SrReq����TotalReqData ǩ�����
*******************************************************************/
int DAEsamIinfoSign(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAEsamIinfoSign() ����Ϣǩ����֡ \n");
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
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ���Թ����������ǩ������ǩ
�������    ��

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��12��
���ͣ�    80 20 00 00 00 40 +Sr1
���أ�    9000+0000
*******************************************************************/
int DAToolRandSignCheck(uint8 *pcmdbuf,uint8 *pdatabuf)
{
    TESAM_FMT_DEBUG("DAToolRandSignCheck() �Թ����������ǩ������ǩ \n");
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
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ����ȡ��Կ��֡
�������    ��

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��12��
���ͣ�    80 30 00 00 00 00
���أ�    9000+0040+pubKey
*******************************************************************/

int DAEsamReadPublicKey(uint8 *pcmdbuf)
{
    TESAM_FMT_DEBUG("DAEsamReadPublicKey() ��ȡ��Կ��֡ \n");
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
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ��������ͨѶ������֡
�������    ��pcmdbuf��֡������ Pdatabuf,��Ҫ���ܵ����ݣ�ǰ�����ֽ�Ϊ����

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��13��
����:  80266202+Lc+ID+R1+IVData+ UpData
            ID ����֤�鹤��ID��R1�ն��������
            IDIVData 16�ֽڣ���ʼ����:  ��վ����� +    ��վ�������λȡ��
            UpData  :��Ҫ���ܵ�����
*******************************************************************/
int DAToolEncryte (uint8 *pcmdbuf,uint8 *pdatabuf,uint8 *pRandNum)
{
    TESAM_FMT_DEBUG("DAToolEncryte() ������ͨѶ������֡ \n");
    int   index   = 0;
    int   datalen = 0;
    uint8 i       = 0;

    datalen = (pdatabuf[1]<<8) + pdatabuf[2];//���ȵ��ֽ��ں󣬸��ֽ���ǰ

    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x26;
    pcmdbuf[index++] = 0x62;
    pcmdbuf[index++] = 0x02;
    pcmdbuf[index++] = (datalen+16)>>8;    //��sendlen���ֽ�,���ȸ��ֽ�
    pcmdbuf[index++] = (datalen+16)&0xff;//��sendlen���ֽ�,���ȵ��ֽ�
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
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ����֤�������ͨѶ������֡
�������    ��pcmdbuf��֡������ Pdatabuf,��Ҫ���ܵ����ݣ�ǰ�����ֽ�Ϊ����

�������    ����

��������ֵ  ��֡����

����        ���Ӿ���
��������    ��2019��8��13��
����:  80 2C 62 02 + Lc+ ID+R1+IVData+ enReqData+MAC
    Lc����ʾ���ȣ�2�ֽ� ID��֤�������ID��8�ֽڣ�
    R1��������������֤ʱ�ն�оƬ�������������8�ֽ�
    IVData����ʼ�����������R1+��R1��λȡ����16�ֽ�
    enReqData��֤�������������ģ���ÿһ֡����Ҫ������
*******************************************************************/
int DAToolDecipher(uint8 *pcmdbuf,uint8 *pdatabuf,uint8 *pRandNum)
{
    TESAM_FMT_DEBUG("DAToolDecipher() ��֤�������ͨѶ������֡ \n");
    int   index   = 0;
    int   datalen = 0;
    uint8 i       = 0;

    datalen = (pdatabuf[1]<<8)  + pdatabuf[2];//���ȵ��ֽ��ں󣬸��ֽ���ǰ
    pcmdbuf[index++] = 0x55;
    pcmdbuf[index++] = 0x80;
    pcmdbuf[index++] = 0x2C;
    pcmdbuf[index++] = 0x62;
    pcmdbuf[index++] = 0x02;
    pcmdbuf[index++] = (datalen+16)>>8;  //+ 16Ϊ��ʼ��������,��sendlen���ֽ�,���ȸ��ֽ�
    pcmdbuf[index++] = (datalen+16)&0xff;//��sendlen���ֽ�,���ȵ��ֽ�
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
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ����ӡ��������оƬ��������
�������    ��pBufҪ��ӡ�����ݻ�����
�������    ����
��������ֵ  ��֡����
����        ���Ӿ���
��������    ��2019��8��13��
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
�������ܵ�Ԫ��������/���ܴ���ģ��
��������    ����ӡ��������оƬ��������
�������    ��pBufҪ��ӡ�����ݻ�����
�������    ����
��������ֵ  ��֡����
����        ���Ӿ���
��������    ��2019��8��13��
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

/* �����֤ */
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
�������ܵ�Ԫ�����ܴ���ģ��
��������    �������оƬͨѶ����ִ���߼�
�������    ��bType�� ��������rcvbuf, ����оƬ�������ݻ�����
                    ��sndbuf ����оƬ����������֡������
                        databuf������������ݻ�������
                        databuf[0]���ؼ���
                        databuf[1]���������ݳ��ȸ��ֽ�
                        databuf[2]���������ݳ��ȵ��ֽ�
�������    ����

��������ֵ  ���ɹ����ؼ���оƬ�ķ������ݳ��ȣ�ʧ�ܷ���-1

����        ���Ӿ���
��������    ��2019��8��7��
�޸���      ��
�޸�˵��    ��
�޸�����    ��
*******************************************************************/
int DAEsamCommand(uint8 bType,uint8 *rcvbuf,uint8 *sndbuf,uint8 *databuf,uint8 *pRandNum)
{
    uint16 bSendLen = 0;
    int    len2 = 0;
    
    switch(bType)
    {
        case DA_ESAM_SERI_READ://��оƬ���к� ID
        {
            bSendLen = ADEsamSeriRead(sndbuf);
            break;
        }
        case DA_ESAM_KEY_VER://��ȡоƬ��Կ�汾1B
        {
            bSendLen = ADEsamKeyVerRead(sndbuf);
            break;
        }
        case DA_ESAM_RAND_READ://����������оƬ�����
        {
            bSendLen = DAEsamRandomRead(sndbuf);
            break;
        }
        case DA_ESAM_RAND_SIGN://�����������ǩ��
        {
            bSendLen =  DAEsamRandSign(sndbuf,databuf);
            break;
        }
        case DA_ESAM_RAND_SIGN_CHECK://�������ǩ��������֤
        {
            bSendLen =  DAEsamRandSignCheck(sndbuf,databuf);
            break;
        }
        case DA_ESAM_KYE_UPDATA://��Կ����
        {
            bSendLen = DAEsamKeyUpdata((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_ESAM_KYE_REBACK://��Կ�ָ�
        {
            bSendLen = DAEsamKeyReback((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_ESAM_ENCRYPTE://���ܴ���
        {
            bSendLen =  DAEsamEncryte((uint8 *)sndbuf,(uint8 *)databuf,(uint8 *)pRandNum);
            break;
        }
        case DA_ESAM_DECIPHER://���ܴ���
        {
            bSendLen =  DAEsamDecipher((uint8 *)sndbuf,(uint8 *)databuf,(uint8 *)pRandNum);
            break;
        }
        case DA_ESAM_INFO_SIGN://����Ϣǩ����֡
        {
            bSendLen =  DAEsamIinfoSign((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_ESAM_INFO_SIGN_CHECK://����Ϣǩ��������֤
        {
            bSendLen =    DAEsamIinfoSignCheck((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_ESAM_MTU_CERT_GET://�ն�֤���ȡ
        {
            bSendLen = DAReadTerminalCert(sndbuf);
            break;
        }
        case DA_ESAM_MTU_CERT_UPDATA://�ն�֤�����
        {
            bSendLen =    DAWriteEsamMtuCert((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }

        case DA_ESAM_HOST_CERT_UPDATA://��վ/����֤�����
        {
            bSendLen =    DAEsamHostCertUpdata((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_ESAM_READ_MTU_CERTLEN://��ȡ�ն˲���֤�鳤����֡
        {
            bSendLen =    DAEsamReadMtuTCertLen((uint8 *)sndbuf);
            break;
        }
        case DA_ESAM_READ_MTU_CERT://��ȡ�ն˲���֤�鳤����֡
        {
            bSendLen =    DAEsamReadMtuTCert((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_TOO_CERT_CHECK://��֤֤�������֤�����Ч����֡
        {
            bSendLen = DAToolCertCheck((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_TOOL_RAND_SIGN_CHECK://�Թ����������ǩ������ǩ
        {
            bSendLen = DAToolRandSignCheck((uint8 *)sndbuf,(uint8 *)databuf);
            break;
        }
        case DA_ESAM_READ_PUBLIC_KEY://��ȡ��Կ
        {
            bSendLen = DAEsamReadPublicKey((uint8 *)sndbuf);
            break;
        }
        case DA_TOOL_ENCRYPTE://��֤�������ͨѶ���ܴ��� 0xb8
        {
            bSendLen =  DAToolEncryte((uint8 *)sndbuf,(uint8 *)databuf,(uint8 *)pRandNum);
            break;
        }
        case DA_TOOL_DECIPHER://��֤�������ͨѶ���ܴ��� 0xb9
        {
            bSendLen =  DAToolDecipher((uint8 *)sndbuf,(uint8 *)databuf,(uint8 *)pRandNum);
            break;
        }
        case DA_TOOL_KEY_REBACK://����֤�鹤���·�����Կ�ָ�����
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

    /* ͨ���ж��Ƿ���ڰ�ȫ�ӹ�APPͨ���ļ� */
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

        /* ���Լ��� */
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
        /* 1. 1ms���յ���Ϣ, �ȴ�0.1s��ʱ����ն��� */
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

        /* 2. ������Ϣ */
        TESAM_BUF_DEBUG(sendmsg, msglen, "Esam send msg");
        ret = task_pfmsg_send(g_UDPEASMid, sendmsg, msglen, SECURITY_APP_DEST);
        if(0 != ret)
        {
            pthread_mutex_unlock(&g_fUdpEsamLock);
            TESAM_FMT_DEBUG("TESAM send faild, ret %d\n", ret);
            return 0;
        }

        /* 3. ���� 2s��ʱ */
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
            TESAM_FMT_DEBUG("DAEsamWrite() ���ͽ��:%d \n", len1);
            usleep(delayus);//��ֹ���������ݣ�ͳһ��ʱ2ms
            if(len1 > 0)
            {
                TESAM_BUF_DEBUG(sndbuf, bSendLen, "���ͱ���: ");
                len2 = DAEsamRead(pDev, rcvbuf, DA_ESAM_MAX_LEN);
                TESAM_FMT_DEBUG("DAEsamRead() ���ս��:%d \n", len2);
                if(len2 > 0)
                {
                    TESAM_BUF_DEBUG(rcvbuf, len2, "���ձ���: ");
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

