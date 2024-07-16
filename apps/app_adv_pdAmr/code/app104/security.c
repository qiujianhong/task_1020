#include "security.h"
#include "../include/protocol104.h"
#include "framecomm.h"
#include "TESAM.h"

static uint8   gSecurityEnable = 0;
//�ӽ��ܽӿڻ���
static uint8   DAEasmUsing = 0;                                     //��������оƬʹ��Ȩ
static uint8   DAEsamSndbuf[DA_ESAM_MAX_LEN];
static uint8   DAEsamRcvbuf[DA_ESAM_MAX_LEN];
static uint8   gDAEsamDecBuf[DA_ESAM_MAX_LEN];                      //���ܻ�����
static uint8   gDAEsamSTemBuf0[DA_ESAM_MAX_LEN];                    //���ܷ�����ʱ������
static uint8   gDAEsamSTemBuf1[DA_ESAM_MAX_LEN];                    //���ܷ�����ʱ������

//���Ӳ�������
static uint8   gHostRandNum[SECURITY_MAX_PIPE][8];                  //��վ�·��������
static uint8   gNetRandNum[SECURITY_MAX_PIPE][8];                   //�����·��������
static uint8   gMtuRandNum[SECURITY_MAX_PIPE][8];                   //�ն˲����������
static uint8   gToolsID[SECURITY_MAX_PIPE][8];                      //������ID
static uint16  gdCertBufIndx[SECURITY_MAX_PIPE][129] = {{0}};       //����֤��ÿ֡����ʼλ��
static uint8   gDAEsamCertBuf[SECURITY_MAX_PIPE][DA_ESAM_MAX_LEN];  //�ն˼���֤�黺����
static uint8   gCertCode[SECURITY_MAX_PIPE] = {0xff};               //֤���ʶ�����ڱȽ�
static uint8   bfgCertCodeDown[SECURITY_MAX_PIPE] = {0};            //֤�����ؿ�ʼ��־
static uint8   bfgMtuCertSendEn[SECURITY_MAX_PIPE][5] = {{0}};      //��վ�ٲ��ն�֤�飬�ն˷���֤���߼����Ʊ�־
                                                                    //��һ���ֽڴ�ŷ�����վ��Ӧ�����ͣ���Ϊ0��ʾ��֤����Ҫ����
                                                                    //�������ֽ���֤��ĳ���
static uint8   gCertSendDelay[SECURITY_MAX_PIPE] = {5};             //����֤��ȴ���ʱ
static uint8   bfgDAEsamRunEn[SECURITY_MAX_PIPE] = {0};             //����������֤ͨ����־��1��վ��֤,2������֤,3֤�������

//����/�ϱ�����
#if SECURITY_FRAME_FETCH == 1
static uint8   gRecvTempBuf[SECURITY_MAX_PIPE][SECURITY_FRAME_MAX_LEN+50] = {{0}};
static int     gRecvTempLen[SECURITY_MAX_PIPE] = {0};
#endif
//static uint8   gSendTempBuf1[DA_ESAM_MAX_LEN];//�ϱ�������ʱ���壬���ڱ�������������ʱ����
//static uint8   gSendTempBuf3[DA_ESAM_MAX_LEN];//�ϱ�������ʱ���壬���ڱ�������������ʱ����
//static uint8   gLogBuf[2048] = {0};

/*
********************************************************************
�������ܵ�Ԫ����������ģ��
��������    ����ȡ����оƬʹ��Ȩ��
�������    ����
�������    ����

��������ֵ  ��1��ʾ��ȡ����
              0��ʾδ��ȡ���� �������߳���ʹ��

����        ���Ӿ���
��������    ��2020��8��8��
*******************************************************************/
static int GetDAEsamUse(void)
{
    uint8 i = 0;

    while(i++<3)
    {
        if(DAEasmUsing)
        {
            usleep(100);
        }
        else
        {
            DAEasmUsing = 1;
            return 1;
        }
    }

    return 0;
}

/*
********************************************************************
�������ܵ�Ԫ����������ģ��
��������    ���ͷż���оƬʹ��Ȩ��
�������    ����
�������    ����

��������ֵ  ����

����        ���Ӿ���
��������    ��2020��8��8��
*******************************************************************/
static void ReleaseADEsam(void)
{
    usleep(10000);//��Ϊ���ζ�������Ҫ��ʱ10ms������ͳһ������
    DAEasmUsing = 0;

    return ;
}

/*
********************************************************************
�������ܵ�Ԫ����������ģ��
��������    �����104/101 ҵ���Ƿ���Ҫ��Ҫ��ǩ
�������    ���������ݻ�����pinbuf ��Ϊ���ܺ����������
�������    ��*ptype����ǩ����

��������ֵ  ��1��ʾ��Ҫ��ǩ��0��ʾ������ǩ

����        ���Ӿ���
��������    ��2020��8��
*******************************************************************/
static int SignNeedCheck(uint8 apptype,uint8 *ptype)
{
    uint16 rtn = 0;

    switch(apptype)
    {
        case APP_TYPE_FILE_ABOUT:   //01 �ļ����������
        case APP_TYPE_PARA_ACK:     //0x03 ����ȷ�ϵ�
        case APP_TYPE_CTRL_SELECT:  //0x05 ң��ѡ��ң�س���
        case APP_TYPE_CTRL_RUN:     //0x07ң��ִ��
        case APP_TYPE_UPDATA:       //0x08�������
        {
            rtn = 1;
            *ptype  = DA_ESAM_INFO_SIGN_CHECK;
            break;
        }
        default:
        {
            rtn = 0;
            break;
        }
    }

    return rtn;
}

/*
********************************************************************
�������ܵ�Ԫ����������ģ��
��������    �����104/101 ҵ���Ƿ���Ҫ��Ҫ����
�������    ��*pInPdu ����Դ������
                  apptype ��Ӧ������
�������    ����

��������ֵ  ��1��ʾ��Ҫ���ܣ�0��ʾ�������

����        ���Ӿ���
��������    ��2019��8��8��
*******************************************************************/
uint8 CheckBufEnTry(uint8 *pInPdu,uint8 apptype)
{
    uint8 rtn = 0;
    S104FRAME_HEAD *p104;  //104��Լ֡��ʽ

    //if(apptype == PRTL_GB104)
    {
        p104 = (S104FRAME_HEAD  *)pInPdu;
        if(p104->sCrtlType12.ibit.fir == 0)//I֡
        {
            //��Ҫ������������ı��ģ�pjhtest
            rtn = 1;
        }
    }
#if 0
    else
    {
        if(pInPdu[0] != 0x10)
        {
            //��Ҫ������������ı��ģ�pjhtest
            rtn = 1;
        }
    }
#endif
    return rtn;
}

/**********************************************************************
* @name      : security_get_serialnum
* @brief     ����ȡ8�ֽڼ���оƬ���к�
* @param[in] ��
* @param[out]��pOutBuf ��Ż���
* @return    ��1��ʾ��ȡ�ɹ���0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
static uint16 DAMakeFrameHead(uint8 *pOutBuf, uint16 bLen, DA_SFP_S_NUM_U frameType)
{
    uint16 index = 0;

    frameType.ubit.keyport = 0; //��Դ���ն�
    pOutBuf[index++] = 0xEB;
    pOutBuf[index++] = bLen>>8;//���ȸ��ֽ���ǰ�����ֽ��ں�
    pOutBuf[index++] = bLen&0xff;
    pOutBuf[index++] = 0xEB;
    pOutBuf[index++] = frameType.commtype[0];
    pOutBuf[index++] = frameType.commtype[1];

    return index;
}

/*
********************************************************************
�������ܵ�Ԫ��GB101��Լ����ģ��
��������        ������У���
�������        : p ���ݣ�len ���ݳ���
�������        ��
��������ֵ      ��У���
��     ��       : �Ӿ���
��������        ��2014��5��3��
�޸���          ��������
�޸�˵��        ��
�޸�����        ��
*******************************************************************/
static uint8 chksum(uint8 *p,uint16 len)
{
    uint16 i ;
    uint8 bchksum = 0;

    for(i=0;i<len;i++)
        bchksum += *(p+i);
    return bchksum;
}

/**********************************************************************
* @name      : DAAddFrameTail
* @brief     ����֡������У��ͼ�������
* @param[in] ��pOutBuf֡����   bLen��������+�����򳤶�
* @param[out]��pOutBuf������Ļ���
* @return    �����������ȫЭ�鱨�ĳ���
* @Create    : �µ���
* @Date      ��2020-03-06
* @Update    :

**********************************************************************/
static uint16 DAAddFrameTail(uint8 *pOutBuf, uint16 bLen)
{
    uint16 index = bLen + 4; //����4�ֽ�ͷ

    pOutBuf[index++] = chksum((uint8 *)&pOutBuf[4],bLen);
    pOutBuf[index++] = 0xD7;
    //PRTL_BUF_DEBUG(pOutBuf, index, "SECURITY���ͱ���:\n");

    return index;
}

/**********************************************************************
* @name      : DACheckFrame
* @brief     �����֡��ʽ
* @param[in] ��pInBuf��Ҫ����֡���棬inLen���泤��
* @param[out]��
* @return    ��>0֡���ȣ�0֡��������Ҫ�������棬-1δ��鵽��Ч֡
* @Create    : �µ���
* @Date      ��2020-03-10
* @Update    :

**********************************************************************/
static int DACheckFrame(uint8* pInBuf, int inLen)
{
    DASAFE_FRAME_HEAD *p = (DASAFE_FRAME_HEAD *)pInBuf;

    //���ͷ
    if (p->headH != 0xEB || p->headL != 0xEB)
    {
        return -1;
    }

    //���������򳤶�
    int len = p->lenH << 8;
    len += p->lenL;
    //4ͷ+1У���+1������
    if (len + 6 > SECURITY_FRAME_MAX_LEN)
    {
        PRTL_FMT_DEBUG("SECURITY֡����,��:%u,�:%u\n", len, SECURITY_FRAME_MAX_LEN);

        return -1;
    }

    //֡��������4ͷ+�����򳤶�+1�����+1������
    if (len + 6 > inLen)
        return 0;

    //���У��ͼ�������
    uint8 cs = chksum(&pInBuf[4], len);

    if (cs != pInBuf[4+len] || pInBuf[5+len] != 0xD7)
    {
        PRTL_FMT_DEBUG("SECURITYУ��ͻ����������,У���%u��%u,������:%u\n", cs, pInBuf[4+len], pInBuf[5+len]);

        return -1;
    }

    return len + 6;
}

#if SECURITY_FRAME_FETCH == 1
static int DAGetFrame(int pipe, uint8* pInBuf, int inLen, uint8* pOutBuf)
{
    int ret = 0;
    int offset = 0;

    //����δ����
    if (inLen > 0 && gRecvTempLen[pipe] + inLen < sizeof(gRecvTempBuf[pipe]))
    {
        memcpy(gRecvTempBuf[pipe], pInBuf, inLen);
        gRecvTempLen[pipe] += inLen;
        inLen = 0;
    }
    else
        PRTL_FMT_DEBUG("SECURITY����֡��������\n");

    while (offset + sizeof(DASAFE_FRAME_HEAD) + 2 < gRecvTempLen[pipe])
    {
        int len = DACheckFrame(&gRecvTempBuf[pipe][offset], gRecvTempLen[pipe]-offset);

        if (len < 0)
        {
            offset++;
            continue;
        }

        if (len == 0)
            break;

        PRTL_FMT_DEBUG("SECURITY�յ�֡,����%u\n", len);
        //������ȷ֡
        memcpy(pOutBuf, &gRecvTempBuf[pipe][offset], len);
        ret = len;
        offset += ret;
        break;
    }

    //�Ƴ��ѽ���������
    MEMMOVE_S(gRecvTempBuf[pipe], 1024, &gRecvTempBuf[pipe][offset], 1024-offset, gRecvTempLen[pipe] - offset);
    gRecvTempLen[pipe] -= offset;

    return ret;
}
#endif

/*
********************************************************************
�������ܵ�Ԫ����������ģ��
��������    ������/���ܳ��ִ������֡
�������    ��*dType����������
              commtype�������͵��ֽ�
�������    ��*pOutBuf,��֡��Ż�����

��������ֵ  ����֡���ݳ���

����        ���Ӿ���
��������    ��2019��8��15��(���ս����)
*******************************************************************/
static int MakeErrFram(uint8 *pOutBuf,uint16 dType,DA_SFP_S_NUM_U frameType)
{
    uint16 index = 6;

    frameType.ubit.encry = 0;
    pOutBuf[index++] = 0x1f;    //Ӧ�����͹�����̴���0x1f
    pOutBuf[index++] = 0;       //��ȫ��չ�����ȸ��ֽ�
    pOutBuf[index++] = 0x02;    //��ȫ��չ�����ȵ��ֽ�
    pOutBuf[index++] = dType>>8;
    pOutBuf[index++] = dType&0xff;

    DAMakeFrameHead(pOutBuf, index-4, frameType);
    return DAAddFrameTail(pOutBuf, index-4);
}

/**********************************************************************
* @name      : security_get_serialnum
* @brief     ����ȡ8�ֽڼ���оƬ���к�
* @param[in] ��
* @param[out]��pOutBuf ��Ż���
* @return    ��1��ʾ��ȡ�ɹ���0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_get_serialnum(uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        int len =  DAEsamCommand(DA_ESAM_SERI_READ,DAEsamRcvbuf,DAEsamSndbuf,NULL,NULL);
        PRTL_FMT_DEBUG("security_get_serialnum() len=%d \n", len);

        if(len>0)
        {
            memcpy(pOutBuf, &DAEsamRcvbuf[4], 8);

            ret = 1;
        }

        ReleaseADEsam();
    }

    PRTL_FMT_DEBUG("security_get_serialnum() ret=%d \n", ret);
    return ret;
}

/**********************************************************************
* @name      : security_get_randnum
* @brief     ����ȡ8�ֽ������
* @param[in] ��pInBuf ���������
* @param[out]��pOutBuf ��ȡ�������
* @return    ��1��ʾ��ȡ�ɹ���0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_get_randnum(uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        gDAEsamSTemBuf0[0] = 0;//�ֽڳ���8
        gDAEsamSTemBuf0[1] = 0;//���ȸ��ֽ�
        gDAEsamSTemBuf0[2] = 8;//���ȵ��ֽ�
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)gNetRandNum,8);
        //����վ���������ǩ��
        int len =  DAEsamCommand(DA_ESAM_RAND_READ,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len > 0)
        {
            memcpy((char *)pOutBuf,(char *)&DAEsamRcvbuf[4],8);//�����8�ֽ�
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_gen_randnumsign
* @brief     �����������ǩ��
* @param[in] ��pInBuf  ���������
* @param[out]��pOutBuf ǩ�����
* @return    ��1��ʾ��ȡ�ɹ���0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_get_randnumsign(uint8* pInBuf, uint8* pOutBuf)
{
    int ret = 0;

    //ǩ��
    if(GetDAEsamUse())          //���������оƬʹ��Ȩ
    {
        gDAEsamSTemBuf0[0] = 0; //�ֽڳ���8
        gDAEsamSTemBuf0[1] = 0; //���ȸ��ֽ�
        gDAEsamSTemBuf0[2] = 8; //���ȵ��ֽ�
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)pInBuf,8);
        int len =  DAEsamCommand(DA_ESAM_RAND_SIGN,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len>0)
        {
            memcpy((char *)gMtuRandNum,(char *)&DAEsamRcvbuf[4],8);//�����8�ֽ�
            memcpy((char *)pOutBuf,(char *)&DAEsamRcvbuf[4],72);

            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_chk_randnumsign
* @brief     ����������ǩ��
* @param[in] ��pInBuf  ǩ������
* @param[out]��pOutBuf �����
* @return    ��1��ʾ��ȡ�ɹ���0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_chk_randnumsign(uint8* pInBuf, uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())                  //���������оƬʹ��Ȩ
    {
        gDAEsamSTemBuf0[0] = pInBuf[64];//ȡ��վ��Կ��ʶ
        gDAEsamSTemBuf0[1] = 0;
        gDAEsamSTemBuf0[2] = 64;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)&pInBuf[0],64);

        int len =  DAEsamCommand(DA_ESAM_RAND_SIGN_CHECK,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len > 0)
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);

            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_chk_randnumsign
* @brief     ����������ǩ��
* @param[in] ��pInBuf  ǩ������
* @param[out]��pOutBuf �����
* @return    ��1��ʾ��ȡ�ɹ���0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_chk_toolrandnumsign(uint8* pInBuf, uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())                  //���������оƬʹ��Ȩ
    {
        gDAEsamSTemBuf0[0] = pInBuf[64];//ȡ��վ��Կ��ʶ
        gDAEsamSTemBuf0[1] = 0;
        gDAEsamSTemBuf0[2] = 64;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)&pInBuf[0],64);

        int len =  DAEsamCommand(DA_TOOL_RAND_SIGN_CHECK,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len > 0)
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);

            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_get_keyversion
* @brief     ����ȡ��Կ�汾
* @param[in] ��
* @param[out]��pOutBuf 8�ֽ���Կ�汾
* @return    ��1��ʾ��ȡ�ɹ���0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_get_keyversion(uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        int len =  DAEsamCommand(DA_ESAM_KEY_VER,DAEsamRcvbuf,DAEsamSndbuf,NULL,NULL);

        if(len > 0)
        {
            memcpy((char *)&pOutBuf[0],(char *)&DAEsamRcvbuf[4],1);
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_update_key
* @brief     ��������Կ
* @param[in] ��pInBuf  ��Կ����
* @param[out]��pOutBuf ���½��
* @return    ��1��ʾ��ȡ�ɹ���0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_update_key(uint8* pInBuf, uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        gDAEsamSTemBuf0[0] = pInBuf[245];//ȡ��վ��Կ��ʶ
        gDAEsamSTemBuf0[1] = 0;
        gDAEsamSTemBuf0[2] = 245;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)pInBuf,245);
        int len =  DAEsamCommand(DA_ESAM_KYE_UPDATA,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len > 0)
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_restore_key
* @brief     ���ָ���Կ
* @param[in] ��pInBuf  ��Կ����
* @param[out]��pOutBuf �ָ����
* @return    ��1��ʾ��ȡ�ɹ���0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_restore_key(uint8* pInBuf, uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        gDAEsamSTemBuf0[0] = pInBuf[245];//ȡ��վ��Կ��ʶ
        gDAEsamSTemBuf0[1] = 0;
        gDAEsamSTemBuf0[2] = 245;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)pInBuf,245);
        int len =  DAEsamCommand(DA_ESAM_KYE_REBACK,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len>0)
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_restore_key
* @brief     ���ָ���Կ
* @param[in] ��pInBuf  ��Կ����
* @param[out]��pOutBuf �ָ����
* @return    ��1��ʾ��ȡ�ɹ���0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_restore_toolkey(uint8* pInBuf, uint16 inLen, uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        gDAEsamSTemBuf0[0] = 0;//ȡ��վ��Կ��ʶ
        gDAEsamSTemBuf0[1] = inLen >> 8;
        gDAEsamSTemBuf0[2] = inLen;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)pInBuf,inLen);
        int len =  DAEsamCommand(DA_TOOL_KEY_REBACK, DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len>0)
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_get_publickey
* @brief     ����ȡ��Կ
* @param[in] ��
* @param[out]��pOutBuf ���ܺõĹ�Կ����
* @return    ��>0��ʾ��Կ���ȣ�0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_get_publickey(int pipe, uint8* pOutBuf)
{
    int ret = 0;

    //��ȡ��Կ
    ret = GetDAEsamUse();
    PRTL_FMT_DEBUG("security_get_publickey() �������оƬʹ��Ȩ ret=%d\n", ret);
    if(ret)//���������оƬʹ��Ȩ
    {
        int len =  DAEsamCommand(DA_ESAM_READ_PUBLIC_KEY,DAEsamRcvbuf,DAEsamSndbuf,NULL,NULL);

        if(len > 0)
        {
            //����
            int index = 1;
            int encrylen = len - 4 + 16 + 3;//-4 (9000,len), +16 (ID + R1), +3 (1Ӧ������, 2����)
            gDAEsamSTemBuf0[index++] = encrylen>>8;
            gDAEsamSTemBuf0[index++] = encrylen&0xff;
            memcpy((char *)&gDAEsamSTemBuf0[index],(char *)gToolsID[pipe],8);
            index += 8;
            memcpy((char *)&gDAEsamSTemBuf0[index],(char *)gMtuRandNum[pipe],8);
            index += 8;
            gDAEsamSTemBuf0[index++] = TOOL_TYPE_MTU_KEY_RES_ACK;
            gDAEsamSTemBuf0[index++] = 0;
            gDAEsamSTemBuf0[index++] = 0x40;
            memcpy((char *)&gDAEsamSTemBuf0[index],(char *)&DAEsamRcvbuf[4],len-4);

            memset(DAEsamRcvbuf, 0, sizeof(DAEsamRcvbuf));
            usleep(10000);

            len = DAEsamCommand(DA_TOOL_ENCRYPTE,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,gMtuRandNum[pipe]);

            if(len>0)
            {
                memcpy((char *)&pOutBuf[0],(char *)&DAEsamRcvbuf[4],len-4);
                ret = len - 4;
            }
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_get_mtutestcert
* @brief     ����ȡ�ն˲���֤��
* @param[in] ��
* @param[out]��pOutBuf ֤�黺��
* @return    ��>0֤�鳤�ȣ�0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_get_mtutestcert(uint8* pOutBuf)
{
    int ret = 0;

    //��ȡ�ն˳�ʼ֤�鳤��
    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        int len = DAEsamCommand(DA_ESAM_READ_MTU_CERTLEN,DAEsamRcvbuf,DAEsamSndbuf,NULL,NULL);

        if(len > 0)
        {//��ȡ�ն˳�ʼ֤��
            usleep(10000);
            gDAEsamSTemBuf0[1] = DAEsamRcvbuf[4];//֤�鳤�ȸ��ֽ�
            gDAEsamSTemBuf0[2] = DAEsamRcvbuf[5];//֤�鳤�ȵ��ֽ�
            memset(DAEsamRcvbuf, 0, sizeof(DAEsamRcvbuf));
            len = DAEsamCommand(DA_ESAM_READ_MTU_CERT,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

            if(len > 0)
            {//֤���д
                int index = 2;
                gDAEsamSTemBuf0[1] = DAEsamRcvbuf[index++];//֤�鳤�ȸ��ֽ�
                gDAEsamSTemBuf0[2] = DAEsamRcvbuf[index++];//֤�鳤�ȵ��ֽ�
                int cerlen = (gDAEsamSTemBuf0[1]<<8) + gDAEsamSTemBuf0[2];
                memcpy((char *)&pOutBuf[0],(char *)&DAEsamRcvbuf[index],cerlen);
                ret = cerlen;
            }
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_get_mtucert
* @brief     ����ȡ�ն�֤��
* @param[in] ��cerID ֤���ʶ  cerbuf ֤�黺��  cerlen֤�鳤��
* @param[out]��pOutBuf ֤�黺��
* @return    ��>0֤�鳤�ȣ�-1��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_get_mtucert(uint8* pOutBuf)
{
    int ret = 0;

    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        int len =  DAEsamCommand(DA_ESAM_MTU_CERT_GET,DAEsamRcvbuf,DAEsamSndbuf,NULL,NULL);

        if(len > 0)
        {
            int cerlen = (DAEsamRcvbuf[2]<<8) + DAEsamRcvbuf[3];
            memcpy((char *)pOutBuf,(char *)&DAEsamRcvbuf[4],cerlen);
            ret = cerlen;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_update_hostcert
* @brief     ����������֤��
* @param[in] ��cerID ֤���ʶ  cerbuf ֤�黺��  cerlen֤�鳤��
* @param[out]��pOutBuf ���½��
* @return    ��1��ʾ��ȡ�ɹ���0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_update_hostcert(uint8 cerID, uint8* cerbuf, uint16 cerlen, uint8* pOutBuf)
{
    int ret = 0;

    //֤�����
    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        gDAEsamSTemBuf0[0] = cerID;
        gDAEsamSTemBuf0[1] = cerlen>>8;
        gDAEsamSTemBuf0[2] = cerlen&0xff;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)cerbuf,cerlen);
        int len = DAEsamCommand(DA_ESAM_HOST_CERT_UPDATA,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len>0)//���³ɹ�
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_update_mtucert
* @brief     �������ն�֤��
* @param[in] ��cerID ֤���ʶ  cerbuf ֤�黺��  cerlen֤�鳤��
* @param[out]��pOutBuf ���½��
* @return    ��1��ʾ��ȡ�ɹ���0��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_update_mtucert(uint8 cerID, uint8* cerbuf, uint16 cerlen, uint8* pOutBuf)
{
    int ret = 0;

    //֤�����
    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        gDAEsamSTemBuf0[0] = cerID;
        gDAEsamSTemBuf0[1] = cerlen>>8;
        gDAEsamSTemBuf0[2] = cerlen&0xff;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)cerbuf,cerlen);
        int len = DAEsamCommand(DA_ESAM_MTU_CERT_UPDATA,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        if(len>0)//���³ɹ�
        {
            memcpy((char *)&pOutBuf[0],(char *)DAEsamRcvbuf,2);
            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_decryte_tool
* @brief     ��������֤����
* @param[in] ��pInBuf ԭʼ���Ļ���   inLen ԭʼ���ĳ���
* @param[out]��pOutBuf ���ܽ������
* @return    ��>0���ܽ�����ȣ�-1��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_decryte_tool(int pipe, uint8* pInBuf, int inLen, uint8* pOutBuf)
{
    int ret = 0;

    //����
    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        int    deciplen = inLen;//���ܳ���
        uint16 datalen  = deciplen + 16;//toolid 8�ֽ�+ R1 8�ֽ�

        gDAEsamSTemBuf0[1] = datalen>>8;
        gDAEsamSTemBuf0[2] = datalen&0xff;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)gToolsID[pipe],8);
        memcpy((char *)&gDAEsamSTemBuf0[11],(char *)gMtuRandNum[pipe],8);
        memcpy((char *)&gDAEsamSTemBuf0[19],(char *)&pInBuf[0],deciplen);
        int len =  DAEsamCommand(DA_TOOL_DECIPHER,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,gMtuRandNum[pipe]);

        if(len > 0)
        {
            //���ܳɹ��������Ǽ���оƬ�������ݸ�ʽ
            //9000(2�ֽ�)+Le(2�ֽ�)+ReqData
            int cerlen = len - 4;//�������������
            int index  = 4;

            memcpy((char *)&pOutBuf[0],(char *)&DAEsamRcvbuf[index],cerlen);
            ret = cerlen;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_get_authsign
* @brief     ����֤��Ϣǩ��
* @param[in] ��pInBuf ԭʼ���Ļ���   inLen ԭʼ���ĳ���
* @param[out]��pOutBuf ǩ���������
* @return    ��>0ǩ��������ȣ�-1��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_get_infosign(uint8* pInBuf, int inLen, uint8* pOutBuf)
{
    int ret = 0;
    int len = 0;

    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        gDAEsamSTemBuf0[1] = inLen >> 8;
        gDAEsamSTemBuf0[2] = inLen & 0xff;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)pInBuf,inLen);

        len = DAEsamCommand(DA_ESAM_INFO_SIGN,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,NULL);

        //���͸���վ
        if(len > 0)
        {
            memcpy((char *)&pOutBuf[0],(char *)&DAEsamRcvbuf[4],len - 4);

            ret = len - 4;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_chk_infosign
* @brief     ����֤��Ϣǩ����ǩ��������Ӧ�������򳤶ȺͰ�ȫ��չ������
* @param[in] ��pInBuf ԭʼ���Ļ���   inLen ԭʼ���ĳ���
* @param[out]��pOutBuf ǩ���������
* @return    ��1 ��֤�ɹ� 0��֤ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_chk_infosign(uint8* pInBuf, int inLen, uint8* pOutBuf, BOOL appFrame)
{
    //��ȡ��ǩ���ݼ����ݳ���
    int ret = 0;
    int len = 0;
    int extlen = 0;

    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        int index1 = 0, index2 = 0;

        if (appFrame)
        {
            len = pInBuf[index2++];//�����򳤶�
            //ȥ��Ӧ������������,����104ԭʼ����
            memcpy((char *)&gDAEsamDecBuf[index1+3],(char *)&pInBuf[index2],len);
            index2 += len;
            index1 += len;

            //�Ƿ��а�ȫ��չ��
            if (inLen > pInBuf[0] + 4) //�����򳤶�+1�����򳤶�λ+2��ȫ��չ������+1 asKID
            {
                //��ȡ��չ������
                extlen = pInBuf[index2++] << 8;
                extlen += pInBuf[index2++];
                //������Ч
                if (index2 + extlen > inLen)
                {
                    PRTL_FMT_DEBUG("SECURITY��֤104ǩ��ʧ��,��չ����������%u����\n", extlen);

                    return ret;
                }
            }
            else {
                PRTL_FMT_DEBUG("SECURITY��֤104ǩ��ʧ��,����չ������\n");

                return ret;
            }
        }
        else {
            extlen = inLen;
        }

        //�Ƿ��а�ȫ��չ��
        if (extlen > 1) //�����򳤶�+1�����򳤶�λ+2��ȫ��չ������+1 asKID
        {
            extlen--;//��ȥasKID
            //������չ������
            memcpy((char *)&gDAEsamDecBuf[index1+3],(char *)&pInBuf[index2],extlen);
            index1 += extlen;
            index2 += extlen;
            gDAEsamDecBuf[0] = pInBuf[index2]; //asKID
            gDAEsamDecBuf[1] = index1 >> 8;
            gDAEsamDecBuf[2] = index1;

            len = DAEsamCommand(DA_ESAM_INFO_SIGN_CHECK,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamDecBuf,NULL);

            if(len > 0)
            {
                if (pOutBuf != NULL)
                    memcpy(pOutBuf, &DAEsamRcvbuf[4], 2);

                ret = 1;
            }
            else {
                PRTL_FMT_DEBUG("SECURITY��֤104ǩ��ʧ��\n");
            }
        }
        else {
            PRTL_FMT_DEBUG("SECURITY��֤104ǩ��ʧ��,��չ����������%u����\n", extlen);
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_chk_cert
* @brief     ����֤֤��
* @param[in] ��certId ֤���ʶ pInBuf ԭʼ���Ļ���   inLen ԭʼ���ĳ���
* @param[out]��pOutBuf ��֤�������
* @return    ��1 ��֤�ɹ� 0��֤ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_chk_cert(uint8* pInBuf, int inLen, uint8* pOutBuf)
{
    //��ȡ��ǩ���ݼ����ݳ���
    int ret = 0;
    int len = 0;

    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        gDAEsamDecBuf[1] = inLen>>8;
        gDAEsamDecBuf[2] = inLen&0xff;

        memcpy((char *)&gDAEsamDecBuf[3],(char *)pInBuf,inLen);
        len = DAEsamCommand(DA_TOO_CERT_CHECK,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamDecBuf,NULL);

        if(len > 0)
        {
            if (pOutBuf != NULL)
                memcpy(pOutBuf, &DAEsamRcvbuf[4], 2);

            ret = 1;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_encryte
* @brief     �����ܴ�������Ӧ���������
* @param[in] ��len ������Դ����  pinbuf,����Դ��Ż�����  bType ����ԴӦ������
* @param[out]��*poutbuf���������ݴ�Ż�����
* @return    ����֡���ص����ݳ��ȣ�-1��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_encryte(int pipe, uint16 len,uint8 *pinbuf,uint8 *poutbuf,uint8 bType)
{
    int    len1     = 0;
    uint16 index     = 0;
    uint16 encrtlen = 0;
    int    rtnlen     = -1;
    BOOL   addRandNum = FALSE;

    switch(bType)
    {
        case APP_TYPE_CTRL_SELECT_ACK://��Ҫ��������
        {
            if (security_get_randnum(gMtuRandNum[pipe]))
            {
                addRandNum = TRUE;
            }
            else
            {
                return -1;
            }
            break;
        }
        case APP_TYPE_NOMMAL:
        default:
        {
            break;
        }
    }

    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        //��������
        index = 3;
        gDAEsamSTemBuf1[index++] = bType;
        gDAEsamSTemBuf1[index++] = len;
        memcpy((char *)&gDAEsamSTemBuf1[index],(char *)pinbuf,len);
        index += len;
        gDAEsamSTemBuf1[index++] = 0;//��չ�����ݳ��ȸ��ֽ�
        if (addRandNum) {
            gDAEsamSTemBuf1[index++] = 8;//��չ�����ݳ��ȵ��ֽ�
            memcpy((char *)&gDAEsamSTemBuf1[index],(char *)gMtuRandNum[pipe],8);
            index += 8;
        }
        else
            gDAEsamSTemBuf0[index++] = 0;        //��չ�����ݳ��ȵ��ֽ�

        encrtlen = index - 3;

        //�����ݽ��м��ܴ���
        gDAEsamSTemBuf1[0] = 0;
        gDAEsamSTemBuf1[1] = encrtlen>>8;
        gDAEsamSTemBuf1[2] = encrtlen&0xff;

        memcpy((char *)&gDAEsamSTemBuf1[3],(char *)gDAEsamSTemBuf0,encrtlen);
        len1 =  DAEsamCommand(DA_ESAM_ENCRYPTE,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf1,gHostRandNum[pipe]);

        if(len1 > 0)
        {
            rtnlen = len1 -4;
            memcpy((char *)poutbuf,(char *)&DAEsamRcvbuf[4],rtnlen);
        }
        else
        {
            rtnlen = -1;
        }

        ReleaseADEsam();
    }
    else
        return -1;

    return rtnlen;
}

/**********************************************************************
* @name      : security_decryte
* @brief     �����ܴ�������Ӧ���������
* @param[in] ��len ������Դ����  pinbuf,����Դ��Ż�����
* @param[out]��*poutbuf������֮���֡����
* @return    ����֡���ص����ݳ��ȣ�-1��ʾ����ʧ��
* @Create    : �µ���
* @Date      ��2020-03-03
* @Update    :

**********************************************************************/
int security_decryte(int pipe, uint8* pInBuf,uint16 bLen,uint8 *pOutBuf)
{
    int   ret = 0;
    int   len = 0;

    //����
    if(GetDAEsamUse())//���������оƬʹ��Ȩ
    {
        gDAEsamSTemBuf0[0] = 0;
        gDAEsamSTemBuf0[1] = bLen>>8;
        gDAEsamSTemBuf0[2] = bLen&0xff;
        memcpy((char *)&gDAEsamSTemBuf0[3],(char *)pInBuf,bLen);
        len =    DAEsamCommand(DA_ESAM_DECIPHER,DAEsamRcvbuf,DAEsamSndbuf,gDAEsamSTemBuf0,gHostRandNum[pipe]);

        if(len > 4)//���ܳɹ�
        {
            len = (DAEsamRcvbuf[2]<<8) + DAEsamRcvbuf[3];
            memcpy((char *)&pOutBuf[0],(char *)&DAEsamRcvbuf[4],len);//�����ջ�������ԭΪ����

            ret = len;
        }

        ReleaseADEsam();
    }

    return ret;
}

/**********************************************************************
* @name      : security_recv_certdata
* @brief     ������֤�����ݣ������֡����
* @param[in] ��pipͨ���� pAppPdu���ݻ��� appLen���ݳ���
* @param[out]��pCertId֤���ʶ
* @return    ��-1��ʾ����ʧ�� 0֤��δ�������               >0֤�鳤��
* @Create    : �µ���
* @Date      ��2020-03-09
* @Update    :

**********************************************************************/
int security_recv_certdata(int pipe, uint8 appType, uint8* pAppPdu, int appLen, uint8* pCertId)
{
    int   index = 0;
    int   cerlen = 0;
    uint8 bCertCode = 0;    //֤���ʶ
    uint8 bTotalNo;         //��֡��
    uint8 bCurrenNo;        //��ǰ֡��

    if (appType != TOOL_TYPE_CERT_FILE_RES)
        bCertCode = pAppPdu[index++];

    bTotalNo    = pAppPdu[index++];
    bCurrenNo   = pAppPdu[index++];

    PRTL_FMT_DEBUG("security_recv_certdata() pipe:%u, ֤���ʶ:%u, ��֡��:%u, ��ǰ֡��:%u\n", pipe, bCertCode, bTotalNo, bCurrenNo);

    if((bfgCertCodeDown[pipe] == 0)&&(bCurrenNo == 1))//��֡�·�
    {
        gCertCode[pipe]   = bCertCode;  //֤���ʶ
        bfgCertCodeDown[pipe] = 1;
    }

    if(gCertCode[pipe] == bCertCode)
    {
        cerlen  = appLen - 3;//֤���ʶ1�ֽڣ���֡��1�ֽڣ���ǰ֡���1�ֽ�

        if (appType == TOOL_TYPE_CHECK_RES_START)
            cerlen -= 8;//֤�������ID8�ֽ�

        int dCertBufIndx = gdCertBufIndx[pipe][bCurrenNo-1];
        if((dCertBufIndx + cerlen ) < DA_ESAM_MAX_LEN)
        {
            //����֤��������İ�
            memcpy((char *)&gDAEsamCertBuf[pipe][dCertBufIndx],(char *)&pAppPdu[index],cerlen);

            if (appType == TOOL_TYPE_CHECK_RES_START)
                memcpy((char *)gToolsID[pipe],(char *)&pAppPdu[index + cerlen],8);//�������֤��ID

            if(bCurrenNo<129)
            {
                gdCertBufIndx[pipe][bCurrenNo] = dCertBufIndx + cerlen;//������һ֡����ʼ���λ��

                if(bTotalNo != bCurrenNo)//��ǰ֡�������һ֡
                {
                    return 0;
                }

                *pCertId = bCertCode;

                //���һ֡,֤���������,����&��ǩ
                //��ȡ֤��������İ�����
                return gdCertBufIndx[pipe][bCurrenNo];
            }
            else
                PRTL_FMT_DEBUG("SECURITY֤�����̫��,pipe:%u,���:%u\n", pipe, bCurrenNo);
        }
        else
            PRTL_FMT_DEBUG("SECURITY֤��̫��,pipe:%u,���:%u,�Ѵ泤��:%u,��֡����:%u\n",
                           pipe, bCurrenNo, dCertBufIndx, cerlen);
    }
    else
        PRTL_FMT_DEBUG("SECURITY֤����쳣,pipe:%u,���:%u,ǰ֡:%u,��֡:%u\n",
                       pipe, bCurrenNo, gCertCode[pipe], bCertCode);

    return -1;
}

/**********************************************************************
* @name      : security_handle_certdata
* @brief     ����װ/����֤�鱨�Ĵ���
* @param[in] ��pipͨ���� pAppPdu���ݻ��� appLen���ݳ���
* @param[out]��pOutBuf���ر��Ļ���
* @return    �����ر��ĳ���
* @Create    : �µ���
* @Date      ��2020-03-09
* @Update    :

**********************************************************************/
int security_handle_certdata(int pipe, DASAFE_FRAME_HEAD* head, uint8* pAppPdu, int appLen, uint8* pOutBuf)
{
    int   len = 0;
    int   index = 0;
    int   cerlen = 0;
    uint8 bCenrCode = 0;    //֤���ʶ

    cerlen = security_recv_certdata(pipe, head->apptype, pAppPdu, appLen, &bCenrCode);

    if (cerlen == 0)
        return 0;

    if (cerlen > 0)
    {
        if (head->apptype == APP_TYPE_CERT_CHANGE_RES)
        {
            //֤��������İ�����
            cerlen = security_decryte(pipe, gDAEsamCertBuf[pipe], cerlen, gDAEsamCertBuf[pipe]);

            if (cerlen > 0)
            {
                usleep(10000);
                if (security_chk_infosign(gDAEsamCertBuf[pipe], cerlen, NULL, FALSE) == 0)
                {
                    cerlen = 0;
                }
            }
            else
                PRTL_FMT_DEBUG("SECURITY֤�����ʧ��,pipe:%u,Ӧ������:%02X,֤���:%u,֤�鳤��:%u\n",
                               pipe, head->apptype, bCenrCode, cerlen);
        }

        if (cerlen > 72)
        {
            index = 6;

            if (head->apptype == APP_TYPE_CERT_CHANGE_RES)
                pOutBuf[index++] = APP_TYPE_CERT_CHANGE_ACK; //0x71�ն˷���֤����½��
            else if (head->apptype == APP_TYPE_CERT_DOWN_RES)
                pOutBuf[index++] = APP_TYPE_CERT_DOWN_ACK;
            pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
            pOutBuf[index++] = 2;

            if (head->apptype == APP_TYPE_CERT_CHANGE_RES)
                len = security_update_hostcert(bCenrCode, gDAEsamCertBuf[pipe]+1, cerlen-72, &pOutBuf[index]);
            else if (head->apptype == APP_TYPE_CERT_DOWN_RES)
                len = security_update_mtucert(bCenrCode, gDAEsamCertBuf[pipe]+1, cerlen-72, &pOutBuf[index]);

            if (len > 0)
            {
                index += 2;
                bfgCertCodeDown[pipe] = 0;
                gCertCode[pipe] = 0xff;
                memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }
            else
                PRTL_FMT_DEBUG("SECURITY֤�����ʧ��,pipe:%u,Ӧ������:%02X,֤���:%u,֤�鳤��:%u\n",
                               pipe, head->apptype, bCenrCode, cerlen);
        }
    }

    bfgCertCodeDown[pipe] = 0;
    gCertCode[pipe] = 0xff;
    memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

    return MakeErrFram(pOutBuf,DASAFE_ERR_CERT_DOWN,head->commtype);
}

/**********************************************************************
* @name      : security_handle_toolauth
* @brief     ��֤���������֤������
* @param[in] ��pipͨ���� pAppPdu���ݻ��� appLen���ݳ���
* @param[out]��pOutBuf���ر��Ļ���
* @return    �����ر��ĳ���
* @Create    : �µ���
* @Date      ��2020-03-09
* @Update    :

**********************************************************************/
int security_handle_toolauth(int pipe, DASAFE_FRAME_HEAD* head, uint8* pAppPdu, int appLen, uint8* pOutBuf)
{
    //����:���Ƚ��գ��յ��������ĺ���֤����֤�ɹ��󣬶�ȡ���������������
    int index = 0;
    int cerlen = 0;
    uint8 bCenrCode = 0;    //֤���ʶ

    cerlen = security_recv_certdata(pipe, head->apptype, pAppPdu, appLen, &bCenrCode);

    PRTL_FMT_DEBUG("security_handle_toolauth() cerlen=%d \n", cerlen);
    if (cerlen > 0)
    {
        if (security_chk_cert(gDAEsamCertBuf[pipe], cerlen, NULL))
        {
            if (security_get_randnum(gMtuRandNum[pipe]))
            {
                index = 6;
                pOutBuf[index++] = TOOL_TYPE_CHECK_RES_START_ACK;
                pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
                pOutBuf[index++] = 8;
                memcpy((char *)&pOutBuf[index],(char *)gMtuRandNum[pipe],8);
                index += 8;

                bfgCertCodeDown[pipe] = 0;
                gCertCode[pipe] = 0xff;
                memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

                DAMakeFrameHead(pOutBuf, index-4, head->commtype);

                return DAAddFrameTail(pOutBuf, index-4);
            }
            else
                PRTL_FMT_DEBUG("SECURITY��ȡ�����ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);
        }
        else
            PRTL_FMT_DEBUG("SECURITY֤����֤ʧ��,pipe:%u,Ӧ������:%02X,֤���:%u,֤�鳤��:%u\n",
                           pipe, head->apptype, bCenrCode, cerlen);
    }
    else 
    {
        //��֡��Ӧ
        PRTL_FMT_DEBUG("security_handle_toolauth() ��֡��Ӧ \n");
        index = 6;
        pOutBuf[index++] = TOOL_TYPE_KEY_CERT_SEND_ACK;
        pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
        pOutBuf[index++] = 2;
        pOutBuf[index++] = 0x90;
        pOutBuf[index++] = 0;
        DAMakeFrameHead(pOutBuf, index-4, head->commtype);

        return DAAddFrameTail(pOutBuf, index-4);
    }

    bfgCertCodeDown[pipe] = 0;
    gCertCode[pipe] = 0xff;
    memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

    return MakeErrFram(pOutBuf,DASAFE_ERR_CERT_DOWN,head->commtype);
}

/**********************************************************************
* @name      : security_handle_appframe
* @brief     ��G101/104Ӧ������֡����
* @param[in] ��pipͨ���� pAppPdu���ݻ��� appLen���ݳ��� bType G101/104֡����
* @param[out]��pOutBuf���ر��Ļ���
* @return    �����ر��ĳ���
* @Create    : �µ���
* @Date      ��2020-03-09
* @Update    :

**********************************************************************/
int security_handle_appframe(int pipe, DASAFE_FRAME_HEAD* head, uint8* pAppPdu, int appLen, uint8* pOutBuf)
{
    int   len;
    uint8 appType = 0;
    uint8 signtype;
    uint8 app104buf[255] = {0};

    if(bfgDAEsamRunEn[pipe] == 0)//ҵ�����̷Ƿ�
    {
        PRTL_FMT_DEBUG("SECURITYҵ�����̷Ƿ�(��û����֤),pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

        return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
    }

    if (pAppPdu[0] < 4)
    {
        PRTL_FMT_DEBUG("SECURITYӦ������������%u����,pipe:%u,Ӧ������:%02X\n", pAppPdu[0], pipe, head->apptype);

        return MakeErrFram(pOutBuf,BUSS_ERR_MESS_UNPACK,head->commtype);
    }

    if(SignNeedCheck(head->apptype, &signtype))//�ж��Ƿ���Ҫ��ǩ
    {    //��ʼ��ǩ��ǩ��������Ӧ�����ͼ�����
        len = security_chk_infosign(pAppPdu, appLen, NULL, TRUE);

        if(len <= 0)
        {//��ǩʧ��
            PRTL_FMT_DEBUG("SECURITYǩ����֤ʧ��,pipe:%u,Ӧ������:%02X,����:%u\n", pipe, head->apptype, pAppPdu[0]);
            return MakeErrFram(pOutBuf,BUSS_ERR_MESS_CHACK,head->commtype);
        }
        //��ǩ�ɹ�ִ�к����߼�
    }

    //����104��֡
    len = frame_data_unpack(pAppPdu[0], &pAppPdu[1], app104buf, &appType);
    if (len <= 0) {
        int i = 0;
        PRTL_FMT_DEBUG("SECURITY 104����δ����,pipe:%u,Ӧ������:%02X,����:\n", pipe, head->apptype);
        for (; i < pAppPdu[0]; i++)
        {
            printf("%02X ", pAppPdu[i+1]);
        }
        printf("\n");

        return 0;
    }

    return security_packappframe(pipe, appType, app104buf, len, pOutBuf);
}

/**********************************************************************
* @name      : security_handle_gwframe
* @brief     ��������ͨ��Ӧ������֡����
* @param[in] ��pipͨ���� pAppPdu���ݻ��� appLen���ݳ���
* @param[out]��pOutBuf���ر��Ļ���
* @return    �����ر��ĳ���
* @Create    : �µ���
* @Date      ��2020-03-09
* @Update    :

**********************************************************************/
int security_handle_gwframe(int pipe, DASAFE_FRAME_HEAD* head, uint8 * pAppPdu, int appLen, uint8* pOutBuf)
{
    int index = 0;

    switch(head->apptype)
    {
        case APP_TYPE_NET_RAND://0x20 ���ط�����������ȴ��ն�ǩ��
        {
            int index1 = 6;
            index += 2;//��չ��ȫ������2�ֽ�
            //�������������
            memcpy((char *)gNetRandNum[pipe],(char *)&pAppPdu[index],8);//�����8�ֽ�
            pOutBuf[index1++] = APP_TYPE_MTU_AUTH;//Ӧ������0x21 �ն���������֤����
            pOutBuf[index1++] = 0; //���ֽڰ�ȫ��չ��Ϣ���ȣ����ֽڳ���Ϊ0
            pOutBuf[index1++] = 73;//8�ֽ��ն������+ 64�ֽ���վ�����ǩ��+ 1�ֽ��ն���Կ��ʶ
            //������վ�����
            memcpy((char *)gHostRandNum[pipe],(char *)&pAppPdu[index],8);//�����8�ֽ�

            if (security_get_randnumsign(&pAppPdu[index], &pOutBuf[index1])) {
                index1 += 72;
                pOutBuf[index1++] = 0x06;//��Կ��ʶ�̶�Ϊ6
                DAMakeFrameHead(pOutBuf, index1-4, head->commtype);

                return DAAddFrameTail(pOutBuf, index1-4);
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ�����ǩ��ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case APP_TYPE_NET_AUTH://0x22���ط����ն˵���֤����������ն�������֤
        {
            int index1 = 6;
            index += 2;//��չ��ȫ������2�ֽ�
            pOutBuf[index1++] = APP_TYPE_AUTH_ACK;///0x23��֤�������
            pOutBuf[index1++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
            pOutBuf[index1++] = 2;

            if (security_chk_randnumsign(&pAppPdu[index], &pOutBuf[index1]))
            {
                bfgDAEsamRunEn[pipe] = 2;
                index1 += 2;
                DAMakeFrameHead(pOutBuf, index1-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index1-4);
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ�����ǩ��ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_RAND_CHECK,head->commtype);
        }
        case APP_TYPE_NET_SERI_RES: //0x24���������ն˼�оƬ���к�
        {
            uint8 serials[10] = {0};

            if (security_get_serialnum(serials))
            {
                index  = 6;
                pOutBuf[index++] = APP_TYPE_NET_SERI_ACK;  //0x25�ն˷����ն����кż�����оƬ���к�
                pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
                pOutBuf[index++] = 32;
                //�ն����к�(ID)=== 3�ֽ��ն����� + 6�ֽڳ��̴���+ 3�ֽ���������+ 8�ֽ���������+ 4�ֽ���ˮ��
                pOutBuf[index++] = 'R';//�ն��ͺ�R13
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '3';
                pOutBuf[index++] = 'X';//���̴���XXXXXX
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = '0';//��������001
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '2';//��������20200626
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '9';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '2';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '0';//��ˮ�ţ�ȡ����оƬ���к�
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = serials[7];
                memcpy((char *)&pOutBuf[index],(char *)&serials,8);
                index += 8;
                DAMakeFrameHead(pOutBuf, index - 4, head->commtype);

                return DAAddFrameTail(pOutBuf, index - 4);
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ���к�ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        default:
            return 0;
    }
}

/**********************************************************************
* @name      : security_handle_gwframe
* @brief     ������վͨ��Ӧ������֡����
* @param[in] ��pipͨ���� pAppPdu���ݻ��� appLen���ݳ���
* @param[out]��pOutBuf���ر��Ļ���
* @return    �����ر��ĳ���
* @Create    : �µ���
* @Date      ��2020-03-09
* @Update    :

**********************************************************************/
int security_handle_hostframe(int pipe, DASAFE_FRAME_HEAD* head, uint8 * pAppPdu, int appLen, uint8* pOutBuf)
{
    int index = 0;

    switch(head->apptype)
    {
        //��վ���ն�˫����֤Ӧ������
        case APP_TYPE_HOST_RAND:    //0x50��վ���������
        {
            //������վ�����
            memcpy((char *)gHostRandNum[pipe],(char *)&pAppPdu[index],8);//�����8�ֽ�
            //��ǩ�����+ ��Կ��ʶ1�ֽ� (0x06) ���͸���վ
            index  = 6;
            pOutBuf[index++] = APP_TYPE_CLIENT_AUTH;//Ӧ������0x51 �ն�����վ��֤����
            pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
            pOutBuf[index++] = 73;

            if (security_get_randnumsign(gHostRandNum[pipe], &pOutBuf[index]))
            {
                index += 72;
                pOutBuf[index++] = 0x06;//��Կ��ʶ�̶�Ϊ6
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ�����ǩ��ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case APP_TYPE_HOST_AUTH:    //0x52��վ�����ն˵���֤����������ն�������֤
        {
            uint8 result[2] = {0};

            if (security_chk_randnumsign(&pAppPdu[index], result))
            {
                bfgDAEsamRunEn[pipe] = 1;
                index  = 6;
                pOutBuf[index++] = APP_TYPE_HOST_ACK;///0x53��֤�������
                pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
                pOutBuf[index++] = 2;
                memcpy((char *)&pOutBuf[index],(char *)result,2);
                index += 2;

                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ�����ǩ��ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }

        case APP_TYPE_HOST_SERI_RES://0x54��վ�����ն˼���оƬ���к�
        {
            index  = 6;
            pOutBuf[index++] = APP_TYPE_HOST_SERI_ACK; //0x55�ն˷����ն����кż�����оƬ���к�
            pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
            pOutBuf[index++] = 8;

            if (security_get_serialnum(&pOutBuf[index])) {
                index += 8;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ���к�ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case APP_TYPE_HOST_CODE_RES://0x56��վ�����ն�������
        {
            uint8 serials[10] = {0};

            if (security_get_serialnum(serials))
            {
                index  = 6;
                pOutBuf[index++] = APP_TYPE_HOST_CODE_ACK; //0x55�ն˷����ն����кż�����оƬ���к�
                pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
                pOutBuf[index++] = 32;
                //�ն����к�(ID)=== 3�ֽ��ն����� + 6�ֽڳ��̴���+ 3�ֽ���������+ 8�ֽ���������+ 4�ֽ���ˮ��
                pOutBuf[index++] = 'R';//�ն��ͺ�R13
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '3';
                pOutBuf[index++] = 'X';//���̴���XXXXXX
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = '0';//��������001
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '2';//��������20200626
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '9';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '2';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '0';//��ˮ�ţ�ȡ����оƬ���к�
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = serials[7];
                memcpy((char *)&pOutBuf[index],(char *)&serials,8);
                index += 8;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ���к�ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }

        case APP_TYPE_MANA_CODE_RES://0x58��վͨ������ͨ�������ն�������
        {
            uint8 serials[10] = {0};

            if (security_get_serialnum(serials))
            {
                index  = 6;
                pOutBuf[index++] = APP_TYPE_MANA_CODE_ACK; //0x55�ն˷����ն����кż�����оƬ���к�
                pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
                pOutBuf[index++] = 60;
                //�ն����к�(ID)=== 3�ֽ��ն����� + 6�ֽڳ��̴���+ 3�ֽ���������+ 8�ֽ���������+ 4�ֽ���ˮ��
                pOutBuf[index++] = 'R';//�ն��ͺ�R13
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '3';
                pOutBuf[index++] = 'X';//���̴���XXXXXX
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = '0';//��������001
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '2';//��������20200626
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '9';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '2';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '0';//��ˮ�ţ�ȡ����оƬ���к�
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = serials[7];
                memset((char *)&pOutBuf[index],0x30,28);//��ΪESN
                index += 28;
                memcpy((char *)&pOutBuf[index],(char *)serials,8);
                index += 8;

                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ���к�ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        //��վ���ն���Կ����Ӧ������
        case APP_TYPE_KEY_VER_RES:    //0x60��վ���ն�������Կ�汾
        {
            index  = 6;
            pOutBuf[index++] = APP_TYPE_KEY_VER_ACK; //0x61�ն˷�����Կ�汾
            pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
            pOutBuf[index++] = 9;

            if (security_get_keyversion(&pOutBuf[index++]))
            {
                if (security_get_randnum(&pOutBuf[index]))
                {
                    memcpy(gMtuRandNum[pipe], &pOutBuf[index], 8);
                    index += 8;
                    DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                    return DAAddFrameTail(pOutBuf, index-4);
                }
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ��Կ�汾ʧ��,pipe:%u\n", pipe);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case APP_TYPE_KEY_CHANGE_RES://0x62��վ������Կ����
        {
            if(bfgDAEsamRunEn[pipe] == 0)//ҵ�����̷Ƿ�
            {
                PRTL_FMT_DEBUG("SECURITYҵ�����̷Ƿ�(��û����֤),pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            uint8 result[2] = {0};

            if (security_update_key(&pAppPdu[index], result))
            {
                index  = 6;
                pOutBuf[index++] = APP_TYPE_KEY_CHANGE_ACK; //0x63//�ն˷�����Կ���½��
                pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
                pOutBuf[index++] = 2;
                memcpy((char *)&pOutBuf[index],(char *)result,2);
                index += 2;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY������Կʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,DASAFE_ERR_KEY_CHANGE,head->commtype);
        }
        case APP_TYPE_KEY_REBACK_RES://0x64��վ������Կ�ָ�
        {
            if(bfgDAEsamRunEn[pipe] == 0)//ҵ�����̷Ƿ�
            {
                PRTL_FMT_DEBUG("SECURITYҵ�����̷Ƿ�(��û����֤),pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            uint8 result[2] = {0};

            if (security_restore_key(&pAppPdu[index], result))
            {
                index  = 6;
                pOutBuf[index++] = APP_TYPE_KEY_REBACK_ACK; //0x63//�ն˷�����Կ���½��
                pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
                pOutBuf[index++] = 2;
                memcpy((char *)&pOutBuf[index],(char *)result,2);
                index += 2;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY�ָ���Կʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,DASAFE_ERR_KEY_REBACK,head->commtype);
        }
        //��վ���ն�֤�����Ӧ������
        case APP_TYPE_CERT_CHANGE_RES://0x70��վ����֤�����
        case APP_TYPE_CERT_DOWN_RES:  //0x72��վ����֤������
        {
            if(bfgDAEsamRunEn[pipe] == 0)//ҵ�����̷Ƿ�
            {
                PRTL_FMT_DEBUG("SECURITYҵ�����̷Ƿ�(��û����֤),pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            return security_handle_certdata(pipe, head, &pAppPdu[index], appLen, pOutBuf);
        }
        case APP_TYPE_CERT_GET_RES:   //0x74��վ��ȡ�ն�֤��
        {
            if(bfgDAEsamRunEn[pipe] == 0)//ҵ�����̷Ƿ�
            {
                PRTL_FMT_DEBUG("SECURITYҵ�����̷Ƿ�(��û����֤),pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            int cerlen = security_get_mtucert(gDAEsamCertBuf[pipe]);

            if (cerlen > 0)
            {
                bfgMtuCertSendEn[pipe][0] = APP_TYPE_CERT_GET_ACK;//�ȴ����߼��ϱ�����֡���õ�Ӧ��
                bfgMtuCertSendEn[pipe][1] = cerlen>>8;
                bfgMtuCertSendEn[pipe][2] = cerlen&0xff;
                bfgMtuCertSendEn[pipe][3] = head->commtype.commtype[0];
                bfgMtuCertSendEn[pipe][4] = head->commtype.commtype[1];
                return 0;
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ�ն�֤��ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case APP_TYPE_CERT_GET_OVER:
        {
            PRTL_FMT_DEBUG("SECURITY�յ�֤����ȡ���%02X%02X,pipe:%u\n", pAppPdu[0], pAppPdu[1], pipe);
            return 0;
        }
        default:
            return 0;
    }
}

/**********************************************************************
* @name      : security_handle_gwframe
* @brief     ����֤�������ͨ��Ӧ������֡����
* @param[in] ��pipͨ���� pAppPdu���ݻ��� appLen���ݳ���
* @param[out]��pOutBuf���ر��Ļ���
* @return    �����ر��ĳ���
* @Create    : �µ���
* @Date      ��2020-03-09
* @Update    :

**********************************************************************/
int security_handle_toolframe(int pipe, DASAFE_FRAME_HEAD* head, uint8* pAppPdu, int appLen, uint8* pOutBuf)
{
    int index = 0;
    int len = 0;
    uint8 bCenrCode = 0;
    int ret = 0;

    switch(head->apptype)
    {
        //֤�������Ӧ������ ����
        case TOOL_TYPE_CHECK_RES_START://0x30֤������߷�����֤����
        {
            ret = security_handle_toolauth(pipe, head, &pAppPdu[index], appLen, pOutBuf);
            PRTL_FMT_DEBUG("security_handle_toolframe() 0x30֤������߷�����֤���� ret=%d \n", ret);
            return ret;
        }
        case TOOL_TYPE_CHECK_RES://0x32֤������߶��ն���֤�������Ӧ
        {
            uint8 result[2] = {0};

            if (security_chk_toolrandnumsign(&pAppPdu[index], result))
            {
                bfgDAEsamRunEn[pipe] = 3;
                index  = 6;
                pOutBuf[index++] = TOOL_TYPE_CHECK_RES_ACK;//��֤�������
                pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
                pOutBuf[index++] = 2;
                memcpy((char *)&pOutBuf[index],(char *)result,2);
                index += 2;

                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY��֤�����ǩ��ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_RAND_CHECK,head->commtype);
        }
        case TOOL_TYPE_KEY_VER_RES://0x34��ȡ�ն���Կ�汾
        {
            index  = 6;
            pOutBuf[index++] = TOOL_TYPE_KEY_VER_RES_ACK;//�ն˷�����Կ�汾
            pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
            pOutBuf[index++] = 9;

            if (security_get_keyversion(&pOutBuf[index++]))
            {
                if (security_get_randnum(&pOutBuf[index]))
                {
                    memcpy(gMtuRandNum[pipe], &pOutBuf[index], 8);
                    index += 8;
                    DAMakeFrameHead(pOutBuf, index-4, head->commtype);

                    return DAAddFrameTail(pOutBuf, index-4);
                }

                PRTL_FMT_DEBUG("SECURITY��ȡ�����ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);
            }
            else
                PRTL_FMT_DEBUG("SECURITY��ȡ��Կ�汾ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case TOOL_TYPE_MTU_ID_RES://0x36��ȡ�ն����к�
        {
            uint8 serials[8];

            if (security_get_serialnum(serials))
            {
                index  = 6;
                pOutBuf[index++] = TOOL_TYPE_MTU_ID_RES_ACK; //�ն˷����ն����кż�����оƬ���к�
                pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
                pOutBuf[index++] = 24;
                //�ն����к�(ID)=== 3�ֽ��ն����� + 6�ֽڳ��̴���+ 3�ֽ���������+ 8�ֽ���������+ 4�ֽ���ˮ��
                pOutBuf[index++] = 'R';//�ն��ͺ�R13 3�ֽ�
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '3';
                pOutBuf[index++] = 'X';//���̴���XXXXXX 6�ֽ�
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = 'X';
                pOutBuf[index++] = '0';//��������001 3�ֽ�
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '2';//��������20200626 8�ֽ�
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '1';
                pOutBuf[index++] = '9';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '2';
                pOutBuf[index++] = '6';
                pOutBuf[index++] = '0';//��ˮ�ţ�ȡ����оƬ���к� 4�ֽ�
                pOutBuf[index++] = '0';
                pOutBuf[index++] = '0';
                pOutBuf[index++] = serials[7];
                //memcpy(&pOutBuf[index], serials, 8);
                //index += 8;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ���к�ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case TOOL_TYPE_MTU_SERI_RES://0x38��ȡ�ն����ð�ȫоƬ���к�
        {
            uint8 serials[8];

            index  = 6;
            pOutBuf[index++] = TOOL_TYPE_MTU_SERI_RES_ACK; //�ն˷���оƬ���кż�����оƬ���к�
            pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
            pOutBuf[index++] = 8;

            if (security_get_serialnum(serials))
            {
                memcpy(pOutBuf+index, serials, 8);
                index += 8;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ���к�ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case TOOL_TYPE_MTU_KEY_RES://0x3A��ȡ�ն˰�ȫоƬ��Կ
        {
            if(bfgDAEsamRunEn[pipe] == 0)//ҵ�����̷Ƿ�
            {
                PRTL_FMT_DEBUG("SECURITYҵ�����̷Ƿ�(��û����֤),pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            index  = 6;
            len = security_get_publickey(pipe, &pOutBuf[index]);
            PRTL_FMT_DEBUG("SECURITY��ȡ��Կ, pipe:%u, ���س���len:%u\n", pipe, len);

            if (len > 0) {
                index += len;
                head->commtype.ubit.encry = 1;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);

                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ��Կʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case TOOL_TYPE_CERT_FILE_RES://0x3C֤������߽����ɵ�֤�������ļ������ն�
        {
            if(bfgDAEsamRunEn[pipe] == 0)//ҵ�����̷Ƿ�
            {
                PRTL_FMT_DEBUG("SECURITYҵ�����̷Ƿ�(��û����֤),pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            int cerlen = security_recv_certdata(pipe, head->apptype, &pAppPdu[index], appLen, &bCenrCode);
            PRTL_FMT_DEBUG("case TOOL_TYPE_CERT_FILE_RES://0x3C cerlen=%d \n", cerlen);
            
            if (cerlen > 0) {
                //��������һ֡��ǩ��
                index  = 6;
                pOutBuf[index++] = TOOL_TYPE_CERT_FILE_RES_ACK;

                len = security_get_infosign(gDAEsamCertBuf[pipe], cerlen, &pOutBuf[index+2]);

                if (len > 0)
                {
                    pOutBuf[index++] = len>>8; //���ֽڰ�ȫ��չ��Ϣ����
                    pOutBuf[index++] = len&0xff;
                    index += len;
                    memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

                    head->commtype.ubit.encry = 0; //�Ǽ���
                    DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                    return DAAddFrameTail(pOutBuf, index-4);
                }

                PRTL_FMT_DEBUG("SECURITY��ȡ֤��ǩ��ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);
            }
            else 
            {
                //��֡��Ӧ
                PRTL_FMT_DEBUG("case TOOL_TYPE_CERT_FILE_RES://0x3C ��֡��Ӧ \n");
                index = 6;
                pOutBuf[index++] = TOOL_TYPE_KEY_CERT_SEND_ACK;
                pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
                pOutBuf[index++] = 2;
                pOutBuf[index++] = 0x90;
                pOutBuf[index++] = 0;
                head->commtype.ubit.encry = 0; //�Ǽ���
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);

                return DAAddFrameTail(pOutBuf, index-4);
            }

            return MakeErrFram(pOutBuf,BUSS_ERR_NONE_KNOW,head->commtype);
        }
        case TOOL_TYPE_CERT_FILE_DOWN://0x3E֤������߽����Ӧ��CA֤�顢��վ֤�顢����֤�顢�ն�֤�鵼���ն�
        {
            int cerlen = security_recv_certdata(pipe, head->apptype, &pAppPdu[index], appLen, &bCenrCode);

            if (cerlen == 0)
                break;

            if (cerlen > 0)
            {
                int i = 0;
                int j = 0;

                printf("�յ�֤��,type:%02X,id:%u,����:\n", head->apptype, bCenrCode);
                while (i < cerlen)
                {
                    for (j = 0; j < 33 && i < cerlen; j++)
                    {
                        printf("%02X", gDAEsamCertBuf[pipe][i++]);
                    }
                    printf("\n");
                }

                index  = 6;
                pOutBuf[index++] = TOOL_TYPE_CERT_FILE_DOWN_ACK; //�ն˷���֤����װ���
                pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
                pOutBuf[index++] = 2;

                if(bCenrCode == 0x06)//�ն�֤��
                    len = security_update_mtucert(gCertCode[pipe], gDAEsamCertBuf[pipe], cerlen, &pOutBuf[index]);
                else
                    len = security_update_hostcert(gCertCode[pipe], gDAEsamCertBuf[pipe], cerlen, &pOutBuf[index]);

                if (len > 0)
                {
                    index += 2;

                    bfgCertCodeDown[pipe] = 0;
                    gCertCode[pipe] = 0xff;
                    memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

                    DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                    return DAAddFrameTail(pOutBuf, index-4);
                }

                PRTL_FMT_DEBUG("SECURITY����֤��ʧ��,pipe:%u,Ӧ������:%02X,֤���:%u\n", pipe, head->apptype, bCenrCode);
            }

            bfgCertCodeDown[pipe] = 0;
            gCertCode[pipe] = 0xff;
            memset((char *)gdCertBufIndx[pipe], 0, 129 * 2);

            return MakeErrFram(pOutBuf,DASAFE_ERR_CERT_DOWN,head->commtype);
        }
        case TOOL_TYPE_CERT_REBACK_RES://0x40�ն˳�ʼ֤���д
        {
            if(bfgDAEsamRunEn[pipe] == 0)//ҵ�����̷Ƿ�
            {
                PRTL_FMT_DEBUG("SECURITYҵ�����̷Ƿ�(��û����֤),pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            int len = security_get_mtutestcert(gDAEsamCertBuf[pipe]);

            if (len > 0)
            {
                index  = 6;
                pOutBuf[index++] = TOOL_TYPE_CERT_REBACK_RES_ACK; //�ն˷���֤����װ���
                pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
                pOutBuf[index++] = 2;

                if (security_update_mtucert(0, gDAEsamCertBuf[pipe], len, &pOutBuf[index]))
                {
                    index += 2;
                    DAMakeFrameHead(pOutBuf, index-4, head->commtype);
                    return DAAddFrameTail(pOutBuf, index-4);
                }

                PRTL_FMT_DEBUG("SECURITY�����ն�֤��ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);
            }
            else
                PRTL_FMT_DEBUG("SECURITY��ȡ����֤��ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,DASAFE_ERR_CERT_DOWN,head->commtype);
        }
        case TOOL_TYPE_KEY_CERT_RES://0x42֤������ߵ����ն˹�Կ֤��
        {
            if(bfgDAEsamRunEn[pipe] == 0)//ҵ�����̷Ƿ�
            {
                PRTL_FMT_DEBUG("SECURITYҵ�����̷Ƿ�(��û����֤),pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            int cerlen = security_get_mtucert(gDAEsamCertBuf[pipe]);

            if (cerlen > 0)
            {
                bfgMtuCertSendEn[pipe][0] = TOOL_TYPE_KEY_CERT_SEND;//�ȴ����߼��ϱ�����֡���Ӧ��
                bfgMtuCertSendEn[pipe][1] = cerlen>>8;
                bfgMtuCertSendEn[pipe][2] = cerlen&0xff;
                bfgMtuCertSendEn[pipe][3] = head->commtype.commtype[0];
                bfgMtuCertSendEn[pipe][4] = head->commtype.commtype[1];

                return 0;
            }

            PRTL_FMT_DEBUG("SECURITY��ȡ�ն�֤��ʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,DASAFE_ERR_CERT_DOWN,head->commtype);
        }
        case TOOL_TYPE_KEY_CERT_RES_ACK://0x44֤����������ն˷���֤�鵼�����
        {
            gCertSendDelay[pipe] = 0;
            return 0;
        }
        case TOOL_TYPE_KEY_CERT_SEND_ACK://0x45��֡���ݴ���������
        {
            //Ӱ����һ֡�Ĵ���
            return 0;
        }
        case TOOL_TYPE_MTU_KEY_REBACK://0x46֤������߻ָ��ն���Կ
        {
            if(bfgDAEsamRunEn[pipe] == 0)//ҵ�����̷Ƿ�
            {
                PRTL_FMT_DEBUG("SECURITYҵ�����̷Ƿ�(��û����֤),pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

                return MakeErrFram(pOutBuf,BUSS_ERR_APP_FLOW,head->commtype);
            }

            uint8 result[2] = {0};

            if (security_restore_toolkey(&pAppPdu[index], appLen, result))
            {
                index = 6;
                pOutBuf[index++] = TOOL_TYPE_MTU_KEY_REBACK_ACK; //�ն˷�����Կ�ָ����
                pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
                pOutBuf[index++] = 0x02;
                memcpy((char *)&pOutBuf[index],(char *)result,2);
                index += 2;
                DAMakeFrameHead(pOutBuf, index-4, head->commtype);

                return DAAddFrameTail(pOutBuf, index-4);
            }

            PRTL_FMT_DEBUG("SECURITY�ָ���Կʧ��,pipe:%u,Ӧ������:%02X\n", pipe, head->apptype);

            return MakeErrFram(pOutBuf,DASAFE_ERR_KEY_REBACK,head->commtype);
        }
        //case TOOL_TYPE_APP_DATA_RES:   //0x48֤����������ն˷����������,    �����ݲ�֧��
        //TOOL_TYPE_APP_DATA_RES_ACK:  //0x49�ն���֤������߷���������ݣ��������ݲ�֧��
        default:
            break;
    }

    return 0;
}

/**********************************************************************
* @name      : security_dealframe
* @brief     ����ȫЭ���������
* @param[in] ��pipͨ���� pInPdu��ȫЭ�鱨�����뻺�� appLen���뱨�ĳ���
* @param[out]��pOutBuf������Ļ���
* @return    ������������ĳ���
* @Create    : �µ���
* @Date      ��2020-03-06
* @Update    :

**********************************************************************/
int security_dealframe(int pipe, uint8 *pInPdu, int nInPduLen, uint8 *pOutBuf)
{
    int     OutbufLen = 0;//���صķ������ݳ���
    int     applen = 0;
    int     index = 0;
    uint8*  pAppPdu;
    DASAFE_FRAME_HEAD *p = (DASAFE_FRAME_HEAD *)pInPdu;

    applen = (p->lenH<<8) + p->lenL - 2; //����֮ǰ�ĳ��ȣ�ȥ��2�ֽڱ�������

    if(p->commtype.ubit.encry == 1)
    {
        if (bfgDAEsamRunEn[pipe] == 0)
        {
            PRTL_FMT_DEBUG("SECURITY����ʧ��,��֤֮ǰ�޷�����\n");
            return 0;
        }

        //����������
        if (bfgDAEsamRunEn[pipe] < 3) //��վ���ؽ���
        {
            PRTL_FMT_DEBUG("bfgDAEsamRunEn[%u]=%d, ������վ���ؽ���\n", pipe, bfgDAEsamRunEn[pipe]);
            applen = security_decryte(pipe, &pInPdu[6], applen, &pInPdu[6]);
        }
        else //֤������߽���
        {
            PRTL_FMT_DEBUG("bfgDAEsamRunEn[%u]=%d, ����֤������߽���\n", pipe, bfgDAEsamRunEn[pipe]);
            applen = security_decryte_tool(pipe, &pInPdu[6], applen, &pInPdu[6]);
        }

        if (applen <= 0) {
            PRTL_FMT_DEBUG("SECURITY����ʧ��, pipe:%u, applen=%d\n", pipe, applen);

            return MakeErrFram(pOutBuf, BUSS_ERR_MESS_DECI, p->commtype);
        }

        PRTL_BUF_DEBUG(&pInPdu[6], applen, "SECURITY���ܺ�����,pipe:%u", pipe);
    }

    //��ȡ������
    pAppPdu = &pInPdu[6];

    p->apptype = pAppPdu[index++];
    applen--; //ȥ��Ӧ������1�ֽڵõ������򳤶�

    PRTL_FMT_DEBUG("security_dealframe() Ӧ������p->apptype=%02x, commtype[0]=%02x, commtype[1]=%02x, �Ƿ����:%u \n", 
        p->apptype, p->commtype.commtype[0], p->commtype.commtype[1], p->commtype.ubit.encry);

    switch(p->apptype)
    {
        case APP_TYPE_NOMMAL:               //0x00 ͨ��
        case APP_TYPE_FILE_ABOUT:           //0x01�ļ����������
        case APP_TYPE_PARA_ACK:             //0x03����ȷ�ϵ�
        case APP_TYPE_CTRL_SELECT:          //0x05 ң��ѡ��ң�س���
        case APP_TYPE_CTRL_RUN:             //0x07ң��ִ��
        {
            int len1 = security_handle_appframe(pipe, p, &pAppPdu[index], applen, pOutBuf);

            if (len1 < 0)
                return -1;

            OutbufLen = len1;

            break;
        }

        case APP_TYPE_NET_RAND:             //0x20 ���ط�����������ȴ��ն�ǩ��
        case APP_TYPE_NET_AUTH:             //0x22���ط����ն˵���֤����������ն�������֤
        case APP_TYPE_NET_SERI_RES:         //0x24���������ն˼�оƬ���к�
        {
            //��ȡ��չ������
            applen = pAppPdu[index++] << 8;
            applen += pAppPdu[index++];
            if (applen + sizeof(DASAFE_FRAME_HEAD) + 4 > nInPduLen)
            {
                PRTL_FMT_DEBUG("SECURITY��չ���ݳ���0x%02X����,pipe:%u\n", applen, pipe);
                return 0;
            }

            OutbufLen = security_handle_gwframe(pipe, p, &pAppPdu[index], applen, pOutBuf);
            break;
        }

        //֤�������Ӧ������ ����
        case TOOL_TYPE_CHECK_RES_START:     //0x30֤������߷�����֤����
        case TOOL_TYPE_CHECK_RES:           //0x32֤������߶��ն���֤�������Ӧ
        case TOOL_TYPE_KEY_VER_RES:         //0x34��ȡ�ն���Կ�汾
        case TOOL_TYPE_MTU_ID_RES:          //0x36��ȡ�ն����к�
        case TOOL_TYPE_MTU_SERI_RES:        //0x38��ȡ�ն����ð�ȫоƬ���к�
        case TOOL_TYPE_MTU_KEY_RES:         //0x3A��ȡ�ն˰�ȫоƬ��Կ
        case TOOL_TYPE_CERT_FILE_RES:       //0x3C֤������߽����ɵ�֤�������ļ������ն�
        case TOOL_TYPE_CERT_FILE_DOWN:      //0x3E֤������߽����Ӧ��CA֤�顢��վ֤�顢����֤�顢�ն�֤�鵼���ն�
        case TOOL_TYPE_CERT_REBACK_RES:     //0x40�ն˳�ʼ֤���д
        case TOOL_TYPE_KEY_CERT_RES:        //0x42֤������ߵ����ն˹�Կ֤��
        case TOOL_TYPE_KEY_CERT_RES_ACK:    //0x44֤����������ն˷���֤�鵼�����
        case TOOL_TYPE_KEY_CERT_SEND_ACK:   //0x45��֡���ݴ���������
        case TOOL_TYPE_MTU_KEY_REBACK:      //0x46֤������߻ָ��ն���Կ
        {
            //��ȡ��չ������
            applen = pAppPdu[index++] << 8;
            applen += pAppPdu[index++];
            if (applen + sizeof(DASAFE_FRAME_HEAD) + 4 > nInPduLen)
            {
                PRTL_FMT_DEBUG("SECURITY��չ���ݳ���0x%02X����,pipe:%u\n", applen, pipe);
                return 0;
            }

            OutbufLen = security_handle_toolframe(pipe, p, &pAppPdu[index], applen, pOutBuf);
            break;
        }

        //��վ���ն�˫����֤Ӧ������
        case APP_TYPE_HOST_RAND:            //0x50��վ���������
        case APP_TYPE_HOST_AUTH:            //0x52��վ�����ն˵���֤����������ն�������֤
        case APP_TYPE_HOST_SERI_RES:        //0x54��վ�����ն˼���оƬ���к�
        case APP_TYPE_HOST_CODE_RES:        //0x56��վ�����ն�������
        case APP_TYPE_MANA_CODE_RES:        //0x58��վͨ������ͨ�������ն�������
        //��վ���ն���Կ����Ӧ������
        case APP_TYPE_KEY_VER_RES:          //0x60��վ���ն�������Կ�汾
        case APP_TYPE_KEY_CHANGE_RES:       //0x62��վ������Կ����
        case APP_TYPE_KEY_REBACK_RES:       //0x64��վ������Կ�ָ�
        //��վ���ն�֤�����Ӧ������
        case APP_TYPE_CERT_CHANGE_RES:      //0x70��վ����֤�����
        case APP_TYPE_CERT_DOWN_RES:        //0x72��վ����֤������
        case APP_TYPE_CERT_GET_RES:         //0x74��վ��ȡ�ն�֤��
        case APP_TYPE_CERT_GET_OVER:        //0x76��վ����֤����ȡ���
        {
            applen = pAppPdu[index++] << 8;
            applen += pAppPdu[index++];
            if (applen + sizeof(DASAFE_FRAME_HEAD) + 4 > nInPduLen)
            {
                PRTL_FMT_DEBUG("SECURITY��չ���ݳ���0x%02X����,pipe:%u\n", applen, pipe);
                return 0;
            }

            OutbufLen = security_handle_hostframe(pipe, p, &pAppPdu[index], applen, pOutBuf);
            break;
        }

        default:
        {
            PRTL_FMT_DEBUG("SECURITY����ʶ��Ӧ������%02X,pipe:%u\n", p->apptype, pipe);
            break;
        }
    }

    return OutbufLen;
}

/**********************************************************************
* @name      : security_unframe
* @brief     ����ȫЭ���֡����
* @param[in] ��pipͨ���� pInPdu��ȫЭ�鱨�����뻺�� appLen���뱨�ĳ���
* @param[out]��pOutBuf������Ļ���
* @return    ������������ĳ���
* @Create    : �µ���
* @Date      ��2020-03-06
* @Update    :

**********************************************************************/
int security_unpackframe(int pipe, uint8 *pInPdu, int nInPduLen, uint8 *pOutBuf)
{
    PRTL_BUF_DEBUG(pInPdu, nInPduLen, "SECURITY�յ�����,pip:%u,",pipe);

    if (SECURITY_MAX_PIPE <= pipe) {
        PRTL_FMT_DEBUG("SECURITYͨ���Ź���,pipe:%u\n", pipe);

        return 0;
    }

#if SECURITY_FRAME_FETCH == 1
    int     applen = 0;
    uint8   dapdu[4096] = {0};

    //��ȡ��Ч֡����
    if ((applen = DAGetFrame(pipe, pInPdu, nInPduLen, dapdu)) <= 0)
        return 0;

    return security_dealframe(pipe, dapdu, applen, pOutBuf);
#else
    uint8 appType = 0;

    if (DACheckFrame(pInPdu, nInPduLen) <= 0) {
        if (gSecurityEnable) {
            PRTL_FMT_DEBUG("SECURITY֡��ʽ����,pipe:%u\n", pipe);

            return 0;
        }

        return frame_data_unpack(nInPduLen, pInPdu, pOutBuf, &appType);
    }

    return security_dealframe(pipe, pInPdu, nInPduLen, pOutBuf);
#endif
}

/**********************************************************************
* @name      : security_packappframe
* @brief     ����ȫЭ����֡������ɼ�����֡����
* @param[in] ��pipͨ���� pInPdu��ȫЭ�鱨������(G104����)���� nInPduLen���뱨�ĳ���
* @param[out]��pOutBuf������Ļ���
* @return    �����������ȫЭ�鱨�ĳ���
* @Create    : �µ���
* @Date      ��2020-03-06
* @Update    :

**********************************************************************/
int security_packappframe(int pipe, uint8 appType, uint8* pInPdu, uint8 nInPduLen, uint8* pOutBuf)
{
    if (!gSecurityEnable)
    {
        memcpy(pOutBuf, pInPdu, nInPduLen);

        return nInPduLen;
    }

    int index = 0;
    int len   = 0;
    DA_SFP_S_NUM_U frameType = {.commtype={0}};

    if(CheckBufEnTry(pInPdu, 0))//�ж��Ƿ���Ҫ����
    {
        frameType.ubit.encry = 1;
        index = DAMakeFrameHead(pOutBuf, nInPduLen, frameType);
        //���ܡ�ǩ����������д��ȫ��չ��
        len = security_encryte(pipe, nInPduLen, pInPdu, &pOutBuf[index], appType);

        if(len > 0)
        {
            index += len;
        }
        else {
            PRTL_FMT_DEBUG("SECURITY����ʧ��,pipe:%u,Ӧ������:%02X,���ݳ�:%u\n", pipe, appType, nInPduLen);
            return -1;
        }
    }
    else
    {
        index = DAMakeFrameHead(pOutBuf, nInPduLen, frameType);
        pOutBuf[index++] = appType;
        pOutBuf[index++] = nInPduLen;
        memcpy((char *)&pOutBuf[index],(char *)pInPdu,nInPduLen);
        index += nInPduLen;
        pOutBuf[index++] = 0; //���ֽڰ�ȫ��չ��Ϣ����
        pOutBuf[index++] = 0;
    }

    return DAAddFrameTail(pOutBuf, index-4);
}

/**********************************************************************
* @name      : security_report
* @brief     ����֡�ϱ��ն�֤��
* @param[in] ��
* @param[out]��pPipe֤���ϱ���ͨ����,pOutBuf�������ݻ���
* @return    ��-1δ����֤�� 0���ڷ��� >0����֡����
* @Create    : �µ���
* @Date      ��2020-03-09
* @Update    :

**********************************************************************/
int security_report(uint8* pPipe, uint8* pOutBuf)
{
    static uint8  bStep[SECURITY_MAX_PIPE] = {0};
    static uint16 gindex[SECURITY_MAX_PIPE] = {0};
    uint16 cerlen = 0;
    uint16 index = 0;
    uint16 len = 0;
    uint8  totalno =  0, curno = 0, cerid = 6; //�ն�֤���ʶ�̶�Ϊ6
    int    i = 0, ret = -1;
    DA_SFP_S_NUM_U frameType;
    memset(&frameType, 0, sizeof(DA_SFP_S_NUM_U));

    for (i = 0; i < SECURITY_MAX_PIPE; i++)
    {
        if (!bfgMtuCertSendEn[i][0])
            continue;

        ret = 0;
        cerlen = (bfgMtuCertSendEn[i][1]<<8) + bfgMtuCertSendEn[i][2];
        totalno = (cerlen + CERT_SEND_MAX_LEN-1) / CERT_SEND_MAX_LEN;
        memcpy(frameType.commtype, &bfgMtuCertSendEn[i][3], 2);

        switch(bStep[i])
        {
            case 0://��������
            {
                if(gindex[i] <  cerlen)
                {
                    if((cerlen - gindex[i])> CERT_SEND_MAX_LEN)
                    {
                        len = CERT_SEND_MAX_LEN;
                    }
                    else
                    {
                        len = cerlen - gindex[i];
                    }

                    curno = gindex[i] / CERT_SEND_MAX_LEN + 1;
                    bStep[i] = 1;
                    gCertSendDelay[i] = 5;//��ʱ5��

                    index = 6;
                    pOutBuf[index++] = bfgMtuCertSendEn[i][0];
                    pOutBuf[index++] = (len+3) >> 8; //���ֽڰ�ȫ��չ��Ϣ����
                    pOutBuf[index++] = (len+3);
                    pOutBuf[index++] = cerid;
                    pOutBuf[index++] = totalno;
                    pOutBuf[index++] = curno;

                    memcpy(&pOutBuf[index], &gDAEsamCertBuf[i][gindex[i]], len);
                    index += len;
                    gindex[i] += len;
                    *pPipe = i;

                    DAMakeFrameHead(pOutBuf, index-4, frameType);
                    return DAAddFrameTail(pOutBuf, index-4);
                }

                gindex[i] = 0;
                bStep[i] = 0;
                bfgMtuCertSendEn[i][0] = 0;
                break;
            }
            case 1://�ȴ�
            {
                if(bfgMtuCertSendEn[i][0] == APP_TYPE_CERT_GET_ACK)//����Ҫ��ʱ����ʱ��sleep
                {
                    usleep(10000);//��ʱ10����
                    bStep[i] = 0;
                }
                else
                {
                    if(gCertSendDelay[i])
                    {
                        usleep(10000);//��ʱ1��
                        gCertSendDelay[i]--;
                        if(gCertSendDelay[i] == 0)
                        {
                            bStep[i] = 0;
                        }
                    }
                    else
                    {
                        bStep[i] = 0;
                    }
                }
                break;
            }
            default:
            {
                PRTL_FMT_DEBUG("SECURITY�ϱ������쳣,pipe:%u,Ӧ������:%02X,���ݳ�:%u,����:%u\n", i, bfgMtuCertSendEn[i][0], cerlen, bStep[i]);
                gindex[i] = 0;
                bStep[i] = 0;
                bfgMtuCertSendEn[i][0] = 0;
                break;
            }
        }
    }

    return ret;
}

/**********************************************************************
* @name      : security_enable
* @brief     ��ʹ�ܰ�ȫЭ��������ϱ�
* @param[in] ��enable 0��ֹ  ���� ʹ��
* @param[out]��
* @return    ��
* @Create    : �µ���
* @Date      ��2020-04-02
* @Update    :

**********************************************************************/
void security_enable(uint8 enable)
{
    gSecurityEnable = (enable != 0);

    PRTL_FMT_DEBUG("���ð�װЭ��ʹ�ܱ�־%u\n", enable);
}


