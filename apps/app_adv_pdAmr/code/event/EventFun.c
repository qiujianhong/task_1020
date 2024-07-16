/*-***************************************************************
* Copyright (c) 2006,�Ͼ����������������޹�˾
*            All rights reserved.
* �������ƣ�EventFun.c
* �汾�ţ�1.0
* ���ܣ�ʵ���¼��ļ�⹦��
* ԭʼ�����˼�ʱ�䣺���� 2006��12��25��
* �޸���Ա��ʱ�估�޸�����:��
* ������
******************************************************************-*/
#include "Event.h"
#include "EventFun.h"
#include "commfun.h"
#include "framecomm.h"
#include "oopVarUpdate.h"
#include "storage.h"
#include "../app104/include/pw_all.h"


EVENT_GLOBAL_T  event_global;       //-������������ݽṹ-
int16 vc_angle[3] = {0};            //-�����ѹ�����н�-
extern uint8 bCheckParaTemp[128];   //-����ն˲���ʱ����ʱ��Ŷ�ȡ�Ĳ���-
extern mtrnum_t MtrACID;

#if WRITE_LOG_EN
extern uint8 bLogBuf[1000];
#endif

/*-�Ѵ����ŵ�ԭ��ȥ������λ-*/
int Int_ABS(int value)
{
    //int val;
    //val = value&0x7fffffff;
    return abs(value);
}

/*-�Ѵ����ŵ�ԭ��ȥ������λ-*/
uint16 Int16_ABS(uint16 value)
{
    uint16 val;
    val = (value&0x7fff);

    return val;
}

/*********************************************************************
�������ܵ�Ԫ�����¼�ģ�����
����������    �Ƚϻ�����ֵ�Լ���Сֵ
���������    bItemNum-�Ƚϵ���Ŀ
              Max
              Min
              awSrc[]-�Ƚϵ�ֵ
���������    ��
��������ֵ��  wMax-���ֵ
�ڲ��������̣�
********************************************************************/
void CmpValint(char bItemNum, int awSrc[],int *MaxVal,int *MinVal)
{
    uint8    i;
    int     wMax = awSrc[0];
    int     wMin = awSrc[0];

    for(i = 0; i < bItemNum; i++)
    {
        if(wMax < awSrc[i])
            wMax = awSrc[i];
        if(wMin > awSrc[i])
            wMin = awSrc[i];
    }

    *MaxVal = wMax;
    *MinVal = wMin;
}

/*********************************************************************
�������ܵ�Ԫ�����¼�ģ�����
����������    �Ƚϻ�����ֵ�Լ���Сֵ
���������    bItemNum-�Ƚϵ���Ŀ
              Max
              Min
              awSrc[]-�Ƚϵ�ֵ
���������    ��
��������ֵ��  wMax-���ֵ
�ڲ��������̣�
********************************************************************/
void CmpValuint16(char bItemNum, uint16 awSrc[],uint16 *MaxVal,uint16 *MinVal)
{
    uint8    i;
    uint16     wMax = awSrc[0];
    uint16     wMin = awSrc[0];

    for(i = 0; i < bItemNum; i++)
    {
        if(wMax < awSrc[i])
            wMax = awSrc[i];
        if(wMin > awSrc[i])
            wMin = awSrc[i];
    }
    *MaxVal = wMax;
    *MinVal = wMin;

}

int ReadRamXData(int off, char *buf, int len)
{
    char bBuf[EVENT_RAMX_FILE_SZIE+1] = {0};

    if (off < 0 || len <= 0 || off + len > EVENT_RAMX_FILE_SZIE)
    {
        EVENT_FMT_DEBUG("ReadRamXData����, ����:%u,ƫ��:%u\n", len, off);
        memset(buf, 0, len);

        return 0;
    }

    read_pdata_xram("^EventS104", bBuf, 0, EVENT_RAMX_FILE_SZIE);
    memcpy(buf, &bBuf[off], len);

    return len;
}

int WriteRamXData(int off, char *buf, int len)
{
    char bBuf[EVENT_RAMX_FILE_SZIE+1] = {0};

    if (off < 0 || len <= 0 || off + len > EVENT_RAMX_FILE_SZIE)
    {
        EVENT_FMT_DEBUG("WriteRamXData����, ����:%u,ƫ��:%u\n", len, off);

        return 0;
    }

    read_pdata_xram("^EventS104", bBuf, 0, EVENT_RAMX_FILE_SZIE);
    memcpy(&bBuf[off], buf, len);

    return write_pdata_xram("^EventS104", bBuf, 0, EVENT_RAMX_FILE_SZIE);
}

#if 0
//��ȡ��ǰʱ��
void GetCurnRtcTm(rtc_t *prtc)
{
    time_t timep;
    time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep
    Sec_To_Rtc(timep, prtc);

    return;
}
#endif

/*********************************************************************
�������ܵ�Ԫ���¼����ģ��
���ƣ�EventFunc.c
������������������Լ�¼�ת���洢ΪGB101/104��ʽ����SOE��⡢�ϱ�
���������bEventType:��D0`D6��ʾ�¼����ͣ����λ��������ͬһ��ŵ������¼�
          bPhaseΪ��λ��־��1����A�࣬2����B�࣬3����C�࣬���Ϊ0��ʾ����Ҫ��¼��λ��Ϣ
          ���λ��ʾ������ָ���1����
          msn Ϊ������ţ�0��ʾ�ն�
�����������
��������ֵ����
�ڲ��������̣� �ֵ������ת�ɹ�Լ���ݸ�ʽ
********************************************************************/
void SaveFileRecBfg(uint8 bEventType,uint8 bPhase,uint8 msn)
{
    uint8 buf[10] = {0};

    EVENT_FMT_DEBUG("SaveFileRecBfg, ERC=%u, MTR=%8X\n", bEventType, msn);

    switch(bEventType&0x7F)//-���λ��������ͬһ��ŵ������¼�-
    {
        case ERC_NO_09://����������
        {//-���������Լ�¼��ַ-
            ReadRamXData(GBPhaseReveAddr,(char*)buf, 9);

             /*-  ���߿���(1QF)A����������Ը澯   D0λ��ʾ�Ƿ���������ԣ�λD4��ʾ�Ƿ��б仯
                             ���߿���(1QF)B����������Ը澯   D1λ��ʾ�Ƿ���������ԣ�λD5��ʾ�Ƿ��б仯
                             ���߿���(1QF)C����������Ը澯   D2λ��ʾ�Ƿ���������ԣ�λD6��ʾ�Ƿ��б仯-*/
            if(bPhase&0x80)//-����-
            {
                buf[msn] = buf[msn]|(bPhase&0x07);//-�����λ��Ϣ-
                if(bPhase&0x01)//-A��-
                {
                    buf[msn] = buf[msn]|0x10;
                }
                if(bPhase&0x02)//-B��-
                {
                    buf[msn] = buf[msn]|0x20;
                }
                if(bPhase&0x04)//-C��-
                {
                    buf[msn] = buf[msn]|0x40;
                }
            }
            else//-�ָ�-
            {
                if(bPhase&0x01)//-A��-
                {
                    buf[msn] = (buf[msn]&0xFE)|0x10;
                }
                if(bPhase&0x02)//-B��-
                {
                    buf[msn] = (buf[msn]&0xFD)|0x20;
                }
                if(bPhase&0x04)//-C��-
                {
                    buf[msn] = (buf[msn]&0xFB)|0x40;
                }
            }
            WriteRamXData(GBPhaseReveAddr, (char*)buf, 9);
            EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBPhaseReveAddr);
            break;
        }
        case ERC_NO_10:
        {   //-�����ʧѹʹ��ͬһ����ţ�����λ�����λ�����֣����λΪ1��ʾ���࣬Ϊ0��ʾʧѹ-
            if(bEventType&0x80)
            {//-��ѹ�����¼��ַ-
                ReadRamXData(GBLostvltAddr,(char*)buf, 9);

                 /*-  ���߿���(1QF)A�����澯   D0λ��ʾ�Ƿ���࣬λD4��ʾ�Ƿ��б仯
                                 ���߿���(1QF)B�����澯   D1λ��ʾ�Ƿ���࣬λD5��ʾ�Ƿ��б仯
                                 ���߿���(1QF)C�����澯   D2λ��ʾ�Ƿ���࣬λD6��ʾ�Ƿ��б仯-*/
                if(bPhase&0x80)//-����-
                {
                    buf[msn] = buf[msn]|(bPhase&0x07);//-�����λ��Ϣ-
                    if(bPhase&0x01)//-A��-
                    {
                        buf[msn] = buf[msn]|0x10;
                    }
                    if(bPhase&0x02)//-B��-
                    {
                        buf[msn] = buf[msn]|0x20;
                    }
                    if(bPhase&0x04)//-C��-
                    {
                        buf[msn] = buf[msn]|0x40;
                    }
                }
                else//-�ָ�-
                {
                    if(bPhase&0x01)//-A��-
                    {
                        buf[msn] = (buf[msn]&0xFE)|0x10;
                    }
                    if(bPhase&0x02)//-B��-
                    {
                        buf[msn] = (buf[msn]&0xFD)|0x20;
                    }
                    if(bPhase&0x04)//-C��-
                    {
                        buf[msn] = (buf[msn]&0xFB)|0x40;
                    }
                }
                WriteRamXData(GBLostvltAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBLostvltAddr);
            }
            else
            {//-��ѹʧѹ��¼��ַ
                ReadRamXData(GBLowvltAddr, (char*)buf, 9);

                /*-���߿���(1QF)A��ʧѹ�澯   D0λ��ʾ�Ƿ�ʧѹ��λD4��ʾ�Ƿ��б仯
                       ���߿���(1QF)B��ʧѹ�澯   D1λ��ʾ�Ƿ�ʧѹ��λD5��ʾ�Ƿ��б仯
                        ���߿���(1QF)C��ʧѹ�澯   D2λ��ʾ�Ƿ�ʧѹ��λD6��ʾ�Ƿ��б仯-*/
                if(bPhase&0x80)//-����-
                {
                    buf[msn] = buf[msn]|(bPhase&0x07);//-�����λ��Ϣ-
                    if(bPhase&0x01)//-A��-
                    {
                        buf[msn] = buf[msn]|0x10;
                    }
                    if(bPhase&0x02)//-B��-
                    {
                        buf[msn] = buf[msn]|0x20;
                    }
                    if(bPhase&0x04)//-C��-
                    {
                        buf[msn] = buf[msn]|0x40;
                    }
                }
                else//-�ָ�-
                {
                    if(bPhase&0x01)//-A��-
                    {
                        buf[msn] = (buf[msn]&0xFE)|0x10;
                    }
                    if(bPhase&0x02)//-B��-
                    {
                        buf[msn] = (buf[msn]&0xFD)|0x20;
                    }
                    if(bPhase&0x04)//-C��-
                    {
                        buf[msn] = (buf[msn]&0xFB)|0x40;
                    }
                }
                WriteRamXData(GBLowvltAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBLowvltAddr);
            }

            break;
        }
        case ERC_NO_11://��ѹ/����������
        {   //-��ѹ/����������ʹ��ͬһ����ţ�����λ�����λ�����֣����λΪ1��ʾ��ѹ��ƽ�⣬Ϊ0��ʾ������ƽ��-
            if(bEventType&0x80)
            {//-��ѹ�������¼��ַ-
                ReadRamXData(GBVltAbnormalAddr, (char*)buf, 9);

                //-buf[msn]��������:-
               /*-���߿���(1QF)��ѹ������澯   D0λ��ʾ�Ƿ��ѹ������λD4��ʾ�Ƿ��б仯-*/
                if(bPhase&0x80)//-����-
                {
                    buf[msn] = buf[msn]|0x01|0x10;
                }
                else//-�ָ�-
                {
                    buf[msn] = (buf[msn]&0xFE)|0x10;
                }
                WriteRamXData(GBVltAbnormalAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBVltAbnormalAddr);
            }
            else
            {//-�����������¼��ַ
                ReadRamXData(GBCurrAbnormalAddr, (char*)buf, 9);

                //-buf[msn]��������:-
               /*-���߿���(1QF)����������澯   D0λ��ʾ�ǵ���������λD4��ʾ�Ƿ��б仯-*/
                if(bPhase&0x80)//-����-
                {
                    buf[msn] = buf[msn]|0x01|0x10;
                }
                else//-�ָ�-
                {
                    buf[msn] = (buf[msn]&0xFE)|0x10;
                }
                WriteRamXData(GBCurrAbnormalAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBCurrAbnormalAddr);
            }

            break;
        }
        case ERC_NO_14:
        {
            ReadRamXData(RTUSelfCheckAddr, (char*)buf, 9);
            if(bEventType&0x80)//ͣ���¼
            {//-ͣ���¼����Ҫ��¼ͣ����ϵ�-
                if(bPhase&0x80)
                    buf[0] = buf[0]|0x01;
                else
                    buf[0] = buf[0]&0xFE;
                buf[1] = buf[1]|0x01;
            }
            else//�Լ�����¼
            {//-�ն��Լ�ֻ��Ҫ��¼�Ƿ�����û�лָ������-
                buf[0] = buf[0]|0x02;
                buf[1] = buf[1]|0x02;
            }
            WriteRamXData(RTUSelfCheckAddr,(char*)buf, 9);
            EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", RTUSelfCheckAddr);
            break;
        }
        case ERC_NO_17:
        {   //-��ѹ��ƽ�ͺ͵�����ƽ��ʹ��ͬһ����ţ�����λ�����λ�����֣����λΪ1��ʾ��ѹ��ƽ�⣬Ϊ0��ʾ������ƽ��-
            if(bEventType&0x80)
            {//-��ѹ��ƽ���¼��ַ-
                ReadRamXData(GBVltUnbAddr, (char*)buf, 9);

                //-buf[msn]��������:-
               /*-���߿���(1QF)��ѹ��ƽ��澯   D0λ��ʾ�Ƿ��ѹ��ƽ�⣬λD4��ʾ�Ƿ��б仯-*/
                if(bPhase&0x80)//-����-
                {
                    buf[msn] = buf[msn]|0x01|0x10;
                }
                else//-�ָ�-
                {
                    buf[msn] = (buf[msn]&0xFE)|0x10;
                }
                WriteRamXData(GBVltUnbAddr, (char*)buf, 9);
            }
            else
            {//-������ƽ���¼��ַ
                ReadRamXData(GBCurrUnbAddr, (char*)buf, 9);

                //-buf[msn]��������:-
               /*-���߿���(1QF)������ƽ��澯   D0λ��ʾ�ǵ�����ƽ�⣬λD4��ʾ�Ƿ��б仯-*/
                if(bPhase&0x80)//-����-
                {
                    buf[msn] = buf[msn]|0x01|0x10;
                }
                else//-�ָ�-
                {
                    buf[msn] = (buf[msn]&0xFE)|0x10;
                }
                WriteRamXData(GBCurrUnbAddr, (char*)buf, 9);
            }
            EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBCurrUnbAddr);
            break;
        }
        case ERC_NO_24:
        {   //-Խ���޺�Խ����ʹ��ͬһ����ţ�����λ�����λ�����֣����λΪ1��ʾԽ���ޣ�Ϊ0��ʾԽ����-
            if(bEventType&0x80)
            {//-��ѹԽ���޼�¼��ַ-
                ReadRamXData(GBVltUpAddr, (char*)buf, 9);
                //-buf[msn]��������:-
                /*-���߿���(1QF)A��Խ���޸澯   D0λ��ʾ�Ƿ�Խ���ޣ�λD4��ʾ�Ƿ��б仯
                                     ���߿���(1QF)B��Խ���޸澯   D1λ��ʾ�Ƿ�Խ���ޣ�λD5��ʾ�Ƿ��б仯
                                     ���߿���(1QF)C��Խ���޸澯   D2λ��ʾ�Ƿ�Խ���ޣ�λD6��ʾ�Ƿ��б仯-*/
                if(bPhase&0x80)//-����-
                {
                    buf[msn] = buf[msn]|(bPhase&0x07);//-�����λ��Ϣ-
                    if(bPhase&0x01)//-A��-
                    {
                        buf[msn] = buf[msn]|0x10;
                    }
                    if(bPhase&0x02)//-B��-
                    {
                        buf[msn] = buf[msn]|0x20;
                    }
                    if(bPhase&0x04)//-C��-
                    {
                        buf[msn] = buf[msn]|0x40;
                    }
                }
                else//-�ָ�-
                {
                    if(bPhase&0x01)//-A��-
                    {
                        buf[msn] = (buf[msn]&0xFE)|0x10;
                    }
                    if(bPhase&0x02)//-B��-
                    {
                        buf[msn] = (buf[msn]&0xFD)|0x20;
                    }
                    if(bPhase&0x04)//-C��-
                    {
                        buf[msn] = (buf[msn]&0xFB)|0x40;
                    }
                }
                WriteRamXData(GBVltUpAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBVltUpAddr);
            }
            else
            {//-��ѹԽ���޼�¼��ַ
                ReadRamXData(GBVltDownAddr, (char*)buf, 9);

                //-buf[msn]��������:-
                /*-���߿���(1QF)A��Խ���޸澯   D0λ��ʾ�Ƿ�Խ���ޣ�λD4��ʾ�Ƿ��б仯
                                     ���߿���(1QF)B��Խ���޸澯   D1λ��ʾ�Ƿ�Խ���ޣ�λD5��ʾ�Ƿ��б仯
                                     ���߿���(1QF)C��Խ���޸澯   D2λ��ʾ�Ƿ�Խ���ޣ�λD6��ʾ�Ƿ��б仯-*/
                if(bPhase&0x80)//-����-
                {
                    buf[msn] = buf[msn]|(bPhase&0x07);//-�����λ��Ϣ-
                    if(bPhase&0x01)//-A��-
                    {
                        buf[msn] = buf[msn]|0x10;
                    }
                    if(bPhase&0x02)//-B��-
                    {
                        buf[msn] = buf[msn]|0x20;
                    }
                    if(bPhase&0x04)//-C��-
                    {
                        buf[msn] = buf[msn]|0x40;
                    }
                }
                else//-�ָ�-
                {
                    if(bPhase&0x01)//-A��-
                    {
                        buf[msn] = (buf[msn]&0xFE)|0x10;
                    }
                    if(bPhase&0x02)//-B��-
                    {
                        buf[msn] = (buf[msn]&0xFD)|0x20;
                    }
                    if(bPhase&0x04)//-C��-
                    {
                        buf[msn] = (buf[msn]&0xFB)|0x40;
                    }
                }
                WriteRamXData(GBVltDownAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBVltDownAddr);
            }

            break;
        }
        case ERC_NO_25:
        {   //-�������غ�����ʹ��ͬһ����ţ�����λ�����λ�����֣����λΪ1��ʾ���أ�Ϊ0��ʾ����-
            if(bEventType&0x80)
            {//-�������ؼ�¼��ַ-
                ReadRamXData(GBCurrUpAddr, (char*)buf, 9);

                //-buf[msn]��������:-
                /*-���߿���(1QF)A��������ظ澯   D0λ��ʾ�Ƿ�������أ�λD4��ʾ�Ƿ��б仯
                                     ���߿���(1QF)B��������ظ澯   D1λ��ʾ�Ƿ�������أ�λD5��ʾ�Ƿ��б仯
                                     ���߿���(1QF)C��������ظ澯   D2λ��ʾ�Ƿ�������أ�λD6��ʾ�Ƿ��б仯-*/
                if(bPhase&0x80)//-����-
                {
                    buf[msn] = buf[msn]|(bPhase&0x07);//-�����λ��Ϣ-
                    if(bPhase&0x01)//-A��-
                    {
                        buf[msn] = buf[msn]|0x10;
                    }
                    if(bPhase&0x02)//-B��-
                    {
                        buf[msn] = buf[msn]|0x20;
                    }
                    if(bPhase&0x04)//-C��-
                    {
                        buf[msn] = buf[msn]|0x40;
                    }
                }
                else//-�ָ�-
                {
                    if(bPhase&0x01)//-A��-
                    {
                        buf[msn] = (buf[msn]&0xFE)|0x10;
                    }
                    if(bPhase&0x02)//-B��-
                    {
                        buf[msn] = (buf[msn]&0xFD)|0x20;
                    }
                    if(bPhase&0x04)//-C��-
                    {
                        buf[msn] = (buf[msn]&0xFB)|0x40;
                    }
                }
                WriteRamXData(GBCurrUpAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBCurrUpAddr);
            }
            else
            {//-�������ؼ�¼��ַ
                ReadRamXData(GBCurrDownAddr, (char*)buf, 9);

                //-buf[msn]��������:-
                /*-���߿���(1QF)A��������ظ澯   D0λ��ʾ�Ƿ�������أ�λD4��ʾ�Ƿ��б仯
                                     ���߿���(1QF)B��������ظ澯   D1λ��ʾ�Ƿ�������أ�λD5��ʾ�Ƿ��б仯
                                     ���߿���(1QF)C��������ظ澯   D2λ��ʾ�Ƿ�������أ�λD6��ʾ�Ƿ��б仯-*/
                if(bPhase&0x80)//-����-
                {
                    buf[msn] = buf[msn]|(bPhase&0x07);//-�����λ��Ϣ-
                    if(bPhase&0x01)//-A��-
                    {
                        buf[msn] = buf[msn]|0x10;
                    }
                    if(bPhase&0x02)//-B��-
                    {
                        buf[msn] = buf[msn]|0x20;
                    }
                    if(bPhase&0x04)//-C��-
                    {
                        buf[msn] = buf[msn]|0x40;
                    }
                }
                else//-�ָ�-
                {
                    if(bPhase&0x01)//-A��-
                    {
                        buf[msn] = (buf[msn]&0xFE)|0x10;
                    }
                    if(bPhase&0x02)//-B��-
                    {
                        buf[msn] = (buf[msn]&0xFD)|0x20;
                    }
                    if(bPhase&0x04)//-C��-
                    {
                        buf[msn] = (buf[msn]&0xFB)|0x40;
                    }
                }
                WriteRamXData(GBCurrDownAddr, (char*)buf, 9);
                EVENT_FMT_DEBUG("SaveFileRecBfg, ID:%u\n", GBCurrDownAddr);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

/**********************************************************************
* @name      : OopDateTimeSToRtc
* @brief     ��ʱ���ʽת�� rtc_tת��ΪOOP_DATETIME_S_T
* @param[in] ��OOP_DATETIME_S_T *dateTimeHex
* @param[out]��rtc_t *rtcTm
* @return    ��
* @Create    : ÷����
* @Date      ��2019-12-22
* @Update    :
**********************************************************************/
void ConvertRtcToBuff(rtc_t rtcTm, uint8* buff)
{
    if(!buff)    return;
    uint16 year = 2000+commfun_BcdToHex(rtcTm.yy);

    buff[0] = year>>8;
    buff[1] = year&0xff;
    buff[2] = commfun_BcdToHex(rtcTm.mm);
    buff[3] = commfun_BcdToHex(rtcTm.dd);
    buff[4] = commfun_BcdToHex(rtcTm.hh);
    buff[5] = commfun_BcdToHex(rtcTm.nn);
    buff[6] = commfun_BcdToHex(rtcTm.ss);
    return;
}

/**********************************************************************
* @name      : OopDateTimeSToRtc
* @brief     ��ʱ���ʽת�� rtc_tת��ΪOOP_DATETIME_S_T
* @param[in] ��rtc_t rtcTm
* @param[out]��OOP_DATETIME_S_T *dateTime
* @return    ��
* @Create    : ÷����
* @Date      ��2020-1-20
* @Update    :
**********************************************************************/
void ConvertRtcToOopDateTime(rtc_t rtcTm, OOP_DATETIME_S_T* dateTime)
{
    if(!dateTime)    return;
    dateTime->year = 2000+commfun_BcdToHex(rtcTm.yy);
    dateTime->month = commfun_BcdToHex(rtcTm.mm);
    dateTime->day = commfun_BcdToHex(rtcTm.dd);
    dateTime->hour = commfun_BcdToHex(rtcTm.hh);
    dateTime->minute = commfun_BcdToHex(rtcTm.nn);
    dateTime->second = commfun_BcdToHex(rtcTm.ss);
    return;
}

/**********************************************************************
* @name      : OopDateTimeSToRtc
* @brief     ��ʱ���ʽת�� OOP_DATETIME_S_Tת��Ϊrtc_t
* @param[in] ��OOP_DATETIME_S_T *dateTimeHex
* @param[out]��rtc_t *rtcTm
* @return    ��
* @Create    : ÷����
* @Date      ��2019-12-22
* @Update    :
**********************************************************************/
void OopDateTimeSToRtc(OOP_DATETIME_S_T *dateTimeHex,rtc_t *rtcTm)
{
    rtcTm->yy =(uint8)commfun_HexToBcd(dateTimeHex->year%100);
    rtcTm->mm =(uint8)commfun_HexToBcd(dateTimeHex->month);
    rtcTm->dd =(uint8)commfun_HexToBcd(dateTimeHex->day);
    rtcTm->hh =(uint8)commfun_HexToBcd(dateTimeHex->hour);
    rtcTm->nn =(uint8)commfun_HexToBcd(dateTimeHex->minute);
    rtcTm->ss =(uint8)commfun_HexToBcd(dateTimeHex->second);
    return ;
}

/**********************************************************************
* @name      : OopDateTimeSToLinuxTime
* @brief     ��OOP_DATETIME_S_Tת��ΪLinuxʱ������
* @param[in] ��OOP_DATETIME_S_T *dateTime
* @param[out]��time_t linuxTime                 Linuxʱ��
* @return    ��
* @Create    : ÷����
* @Date      ��2019-12-22
* @Update    :
**********************************************************************/
void OopDateTimeSToLinuxTime(OOP_DATETIME_S_T *dateTime, time_t *linuxTim)
{
    struct tm ptm;

    if(dateTime == NULL)
    {
        return ;
    }

    ptm.tm_year = dateTime->year-1900;
    ptm.tm_mon = dateTime->month-1;
    ptm.tm_mday = dateTime->day;
    ptm.tm_hour = dateTime->hour;
    ptm.tm_min = dateTime->minute;
    ptm.tm_sec = dateTime->second;

    *linuxTim = mktime(&ptm);
}

/**********************************************************************
* @name      : LinuxTimeToOopDateTimeS
* @brief     ��Linuxʱ������ת��ΪOOP_DATETIME_S_T
* @param[in] ��time_t linuxTime                 Linuxʱ��
* @param[out]��OOP_DATETIME_S_T *dateTime
* @return    ��
* @Create    : ÷����
* @Date      ��2019-12-22
* @Update    :
**********************************************************************/
void LinuxTimeToOopDateTimeS(time_t linuxTime, OOP_DATETIME_S_T *dateTime)
{
    struct tm tm_Time;

    LOCALTIME_R(&linuxTime, &tm_Time);

    dateTime->year = tm_Time.tm_year+1900;
    dateTime->month = tm_Time.tm_mon+1;
    dateTime->day  = tm_Time.tm_mday;
    dateTime->week = tm_Time.tm_wday;
    dateTime->hour = tm_Time.tm_hour;
    dateTime->minute = tm_Time.tm_min;
    dateTime->second = tm_Time.tm_sec;

    return;
}

/**********************************************************************
* @name      : GetCurrentOopDateTimeS
* @brief     ����ȡ��ǰOOP_DATETIME_S_T��ʽʱ��
* @param[in] ��
* @param[out]��OOP_DATETIME_S_T *dateTime
* @return    ��
* @Create    : ÷����
* @Date      ��2019-12-22
* @Update    :

**********************************************************************/
void GetCurrentOopDateTimeS(OOP_DATETIME_S_T *dateTime)
{
    time_t timep;
    time(&timep);
    LinuxTimeToOopDateTimeS(timep, dateTime);
    return;
}

/*���ݴ�����¼�OI,����Ҫ��ȡ�ļ�¼�¼���ŵ������ֵ�ID*/
uint16 GetEventRecNumIDByOI(uint16 EvtOI)
{
    uint16 ID = 0;
/*
    switch(EvtOI)
    {
    case 0x3105:
        ID = LastSucc_3105EventRecNO;            //3105H��ʱ�ӳ���
        break;
    case 0x310B:
        ID = LastSucc_310BEventRecNO;            //310BH�����ܱ�ʾ���½�
        break;
    case 0x310C:
        ID = LastSucc_310CEventRecNO;            //310CH������������
        break;
    case 0x310D:
        ID = LastSucc_310DEventRecNO;            //310DH�����ܱ����
        break;
    case 0x310E:
        ID = LastSucc_310EEventRecNO;            //310EH�����ܱ�ͣ��
        break;
    case 0x310F:
        ID = LastSucc_310FEventRecNO;            //310FH������ʧ��
        break;
    default:
        ID=0;
        break;
    }
*/    return ID;
}

/*****************************************************************************
�������ܵ�Ԫ��
����������Ϊ�¼��ָ���׼������������ǰ�¼���ż�¼���ֵ���
���������������ţ�oad
�����������
��������ֵ����
�ڲ��������̣�
***************************************************************************-*/

int PrepareResumeEvent(uint16 EvtOI,mtrnum_t mtr_id)
{/*
    uint32 curRecNum = 0;//��ǰ�¼���¼���¼����
    uint16 uIdIndex = 0U;//EvtOI�¼���¼��ŵ��ֵ�����
    uint8 IDbuff[RECORD_DATA_POS + MAX_EVT_RECORD_NUM*SINGLE_RECORD_LEN];
    const char *szRoot;
    char  filename[200] = {0};

    memset(IDbuff,0xFF,RECORD_DATA_POS + MAX_EVT_RECORD_NUM*SINGLE_RECORD_LEN);
    szRoot ="/mnt/data/";
    sprintf(filename, "%sE_DATA/%04X", szRoot, EvtOI);

    //��ȡ�¼����
    if(read_pdata_xram(filename, (char *)&curRecNum, 2, 4) < 0)
    {
        return -1;
    }
    #ifdef EE_DEBUG_EN
    DPRINT("curRecNum =%ld,mtr_id = %d\n",curRecNum,mtr_id);
    #endif
    uIdIndex = GetEventRecNumIDByOI(EvtOI);
    if(WriteMtrXData(mtr_id,RT_DATA,uIdIndex,(char*)&curRecNum)<0)
    {
        return -1;
    }

*/    return 1;
}

/**********************************************************************
* @name      : getMsgLenOffset
* @brief     ����ȡoctet-string����Ԫ�س���,ƫ����
* @param[in] ��const uint8 *indata      ����Buffer
* @param[out]��uint8 *bytes             ƫ����
* @return    ������
* @Create    : ÷����
* @Date      ��2020-1-19
* @Update    :
**********************************************************************/
uint16 getMsgLenOffset(uint8 *indata,uint8 *bytes)
{
    uint16 EleNum;
    uint8 nbytes = 1;

    EleNum = indata[0];
    if(EleNum==0x81)
    {
        EleNum = indata[1];
        nbytes = 2;
    }
    else if(EleNum==0x82)
    {
        EleNum = indata[1];
        EleNum <<= 8;
        EleNum += indata[2];
        nbytes = 3;
    }
    else if(EleNum==0x80 || EleNum>0x82)
    {
        return 0;
    }

    if(bytes != NULL)
        *bytes = nbytes;

    return EleNum;
}

/**********************************************************************
* @name      : SetEventNum
* @brief     ���ն��¼���¼������¼
* @param[in] ��EVENT_COMM_INFO *eventCommInfo     �¼�������Ϣ
* @param[out]��
* @return    ��0 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2020-03-03
* @Update    :
**********************************************************************/
int SetEventNum(EVENT_COMM_INFO *eventCommInfo)
{
    int             ret     = -1;
    NOMAL_OAD_T     nomalOAD;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    //�¼���¼����
    eventCommInfo->eventNum++;
    nomalOAD.oad.value = Uint16To32(eventCommInfo->eventOAD.nObjID, 0X0400);
    nomalOAD.classtag = CLASS_DATA_EVENT;
    ret = db_write_nomal(g_sDataCenterClient, &nomalOAD, (uint8 *)&eventCommInfo->eventNum, 2);
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_write_nomal oad[%08X]��¼�¼�����=%dʧ�ܷ���%d\n", nomalOAD.oad.value, eventCommInfo->eventNum, ret);
    }
    return ret;
}


/**********************************************************************
* @name      : SetEventStat
* @brief     ���ն��¼���ǰֵ��¼
* @param[in] ��mtrnum_t          mtr_id            �������(�����ն˱��壬��0��ʼ)
               EVENT_COMM_INFO *eventCommInfo     �¼�������Ϣ
               OOP_EVTSOURCE_T  eventSource       �¼�����Դ
               uint8            bType             �¼�״̬ 0(ֻ��¼�¼����޷������ָ�����)��1������2�ָ�
* @param[out]��
* @return    ��0 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2020-03-03
* @Update    :
**********************************************************************/
int SetEventStat(mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, uint8 bType)
{
    int             ret     = -1;
    uint32          time    = 0;
    NOMAL_OAD_T     nomalOAD;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    eventCommInfo->eventState.source = eventSource;
    eventCommInfo->eventState.dwCount = eventCommInfo->eventNum;
    eventCommInfo->eventState.dbIdx = eventCommInfo->storagenum[mtr_id];

    //�ָ��¼�
    if(bType == 2)
    {
        OopDateTimeSToLinuxTime(&eventCommInfo->endtime[mtr_id], (time_t *)&time);
        eventCommInfo->eventState.dwTimes += time - eventCommInfo->eventState.tmStart;
    }
    else
    {
        OopDateTimeSToLinuxTime(&eventCommInfo->starttime[mtr_id], (time_t *)&time);
        eventCommInfo->eventState.tmStart = time;
    }

    //�¼���ǰֵ��¼
    nomalOAD.oad.value = Uint16To32(eventCommInfo->eventOAD.nObjID, 0X0700);
    nomalOAD.classtag = CLASS_DATA_EVENT;
    ret = db_write_nomal(g_sDataCenterClient, &nomalOAD, (uint8 *)&eventCommInfo->eventState, sizeof(EVTSTATE));
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_write_nomal oad[%08X]��¼�¼���ǰֵʧ�� ����%d\n", nomalOAD.oad.value, ret);
    }
    return ret;
}


/**********************************************************************
* @name      : SetEventTime
* @brief     ���ն��¼�ʱ��״̬��¼
* @param[in] ��mtrnum_t          mtr_id            �������(�����ն˱��壬��0��ʼ)
               EVENT_COMM_INFO *eventCommInfo     �¼�������Ϣ
               OOP_EVTSOURCE_T  eventSource       �¼�����Դ
               OOP_EVTBUF_T     eventData         �¼�������������
               uint8            bType             �¼�״̬ 0(ֻ��¼�¼����޷������ָ�����)��1������2�ָ�
* @param[out]��
* @return    ��0 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2019-12-22
* @Update    :
**********************************************************************/
int SetEventTime(mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, uint8 bType)
{
    int             ret     = -1;
    //uint32          time    = 0;
    NOMAL_OAD_T     nomalOAD;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    eventCommInfo->eventTime.source = eventSource;
    eventCommInfo->eventTime.dbIdx = eventCommInfo->storagenum[mtr_id];

    //�ָ��¼�
    if(bType == 2)
    {
        eventCommInfo->eventTime.tmEnd = eventCommInfo->endtime[mtr_id];
    }
    else
    {
        eventCommInfo->eventTime.tmStart = eventCommInfo->starttime[mtr_id];
    }

    //�¼�ʱ��״̬��¼
    nomalOAD.oad.value = Uint16To32(eventCommInfo->eventOAD.nObjID, 0X0A00);
    nomalOAD.classtag = CLASS_DATA_EVENT;
    ret = db_write_nomal(g_sDataCenterClient, &nomalOAD, (uint8 *)&eventCommInfo->eventTime, sizeof(EVTTIME));
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_write_nomal oad[%08X]��¼�¼�ʱ��״̬ʧ�� ����%d\n", nomalOAD.oad.value, ret);
    }
    return ret;
}


/**********************************************************************
* @name      : WriteEventToDB
* @brief     ���ն��¼�д�����ݿ�
* @param[in] ��mtrnum_t          mtr_id            �������(�����ն˱��壬��0��ʼ)
               EVENT_COMM_INFO *eventCommInfo     �¼�������Ϣ
               OOP_EVTSOURCE_T  eventSource       �¼�����Դ
               OOP_EVTBUF_T     eventData         �¼�������������
               uint8            bType             �¼�״̬ 0(ֻ��¼�¼����޷������ָ�����)��1������2�ָ�
* @param[out]��
* @return    ��0 �ɹ��� <0 ʧ��
* @Create    : ÷����
* @Date      ��2019-12-22
* @Update    :
**********************************************************************/
int WriteEventToDB(mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, OOP_EVTBUF_T eventData, uint8 bType)
{
    time_t             timep        = 0;
    int                ret          = 0;
    //uint8              *bDatabuf    = NULL;
    //uint32             bufLen       = 0;
    OOP_EVENT_T        oopEvent;        //��׼�¼���Ԫ
    OOP_CLOCKFLYLOG_T  clockEvent;      //ʱ�ӳ����¼���Ԫ
    OOP_POWERLOG_T     powerEvent;      //ͣ�ϵ��¼���Ԫ
    OOP_SMLOG_T        smEvent;         //δ֪���ܱ��¼���Ԫ
    OOP_PROGRAMLOG_T   programEvent;    //����¼�
    UPDATE_RECORD_T    updateRecordData;
    WRITE_RECORD_T     recInData;

    memset(&recInData,0,sizeof(WRITE_RECORD_T));
    memset(&updateRecordData,0,sizeof(UPDATE_RECORD_T));
    memset(&oopEvent,0,sizeof(OOP_EVENT_T));
    memset(&clockEvent,0,sizeof(OOP_CLOCKFLYLOG_T));
    memset(&powerEvent,0,sizeof(OOP_POWERLOG_T));
    memset(&smEvent,0,sizeof(OOP_SMLOG_T));
    memset(&programEvent,0,sizeof(OOP_PROGRAMLOG_T));

    oopEvent.source = eventSource;
    oopEvent.data = eventData;
    time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep

    if(bType <= 1)//����
    {
        //�¼���¼����
        GetCurrentOopDateTimeS(eventCommInfo->starttime + mtr_id);
        oopEvent.nIndex = eventCommInfo->recordnum +1;
        oopEvent.tmStart = eventCommInfo->starttime[mtr_id];
        if(bType == 0)                       //bType=1�¼�����ʱ������¼����ʱ�䣬���¼��ָ�ʱ����
            oopEvent.tmEnd = oopEvent.tmStart;
//            oopEvent.state

        recInData.recordOAD.classtag = CLASS_DATA_EVENT;
        recInData.recordOAD.road.oadCols.nNum = 1;
        recInData.recordOAD.road.oadCols.oad[0] = eventCommInfo->eventOAD;
//        recInData.MAC = MAC;
        recInData.colStartTm = timep;
        recInData.colEndTm = timep;
        recInData.colStoreTm = eventCommInfo->recordnum +1;
        recInData.inDatalen = sizeof(OOP_EVENT_T);
        memcpy(recInData.inDataBuf, &oopEvent, sizeof(OOP_EVENT_T));

        //ͣ�ϵ��¼�
        if (eventCommInfo->eventOAD.value == 0X31060200)
        {
            memcpy(&powerEvent, &oopEvent, sizeof(OOP_EVENT_T));

            if(event_3106.ishappen == 0xaa)
            {
                powerEvent.tmStart = event_3106.tmnStarttime;
                powerEvent.tmEnd = event_3106.tmnEndtime;
                EVENT_FMT_DEBUG("�ն�ͣ��ʱ��%u-%u-%u %u:%u:%u \n", event_3106.tmnStarttime.year, event_3106.tmnStarttime.month, event_3106.tmnStarttime.day, event_3106.tmnStarttime.hour, event_3106.tmnStarttime.minute, event_3106.tmnStarttime.second);
                EVENT_FMT_DEBUG("�ն��ϵ�ʱ��%u-%u-%u %u:%u:%u \n", event_3106.tmnEndtime.year, event_3106.tmnEndtime.month, event_3106.tmnEndtime.day, event_3106.tmnEndtime.hour, event_3106.tmnEndtime.minute, event_3106.tmnEndtime.second);
            }

            powerEvent.flag.nValue = event_3106.eventFlag;

            recInData.inDatalen = sizeof(OOP_POWERLOG_T);
            memcpy(recInData.inDataBuf, &powerEvent, sizeof(OOP_POWERLOG_T));
        }
        else
        {
            recInData.inDatalen = sizeof(OOP_EVENT_T);
            memcpy(recInData.inDataBuf, &oopEvent, sizeof(OOP_EVENT_T));
        }

        EVENT_FMT_DEBUG("WriteEventToDB write, infoNum=%u, OAD=%8X\n", mtr_id, eventCommInfo->eventOAD.value);

        ret = db_write_record(g_sDataCenterClient, &recInData, &eventCommInfo->storagenum[mtr_id]);
        if (ret != ERR_OK)
        {
            EVENT_FMT_DEBUG("failed, infoNum=%u, OAD=%8X\n", mtr_id, eventCommInfo->eventOAD.value);
            return ret;
        }

        //�¼���¼����
        SetEventNum(eventCommInfo);
        //�¼���ǰֵ��¼
        SetEventStat(mtr_id, eventCommInfo, eventSource, bType);
        //�¼�ʱ��״̬��¼
        SetEventTime(mtr_id, eventCommInfo, eventSource, bType);

        if(bType == 1) //���ܱ����
        {
            //��¼�洢��ź��¼���ʼʱ��
            write_pdata_xram("^Event", (char *)&eventCommInfo->storagenum[mtr_id], (eventCommInfo->EventStoragNumRecAddr+(mtr_id)*(4)), 4);
            write_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(mtr_id)*(4)), 4);
        }

        eventCommInfo->recordnum++;
    }
    else if(bType == 2)
    {
        //�ָ��¼���¼����
        read_pdata_xram("^Event", (char *)&eventCommInfo->storagenum[mtr_id], (eventCommInfo->EventStoragNumRecAddr+(mtr_id)*(4)), 4);
        read_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(mtr_id)*(4)), 4);

        LinuxTimeToOopDateTimeS(timep, &eventCommInfo->starttime[mtr_id]);
        GetCurrentOopDateTimeS(&eventCommInfo->endtime[mtr_id]);
        oopEvent.nIndex = eventCommInfo->recordnum - 1;         //�¼�����ʱ���Ѿ�+1
        oopEvent.tmStart = eventCommInfo->starttime[mtr_id];
        oopEvent.tmEnd = eventCommInfo->endtime[mtr_id];
    //            oopEvent.state

        updateRecordData.recordOAD.road.oadCols.nNum = 1;
        updateRecordData.recordOAD.road.oadCols.oad[0] = eventCommInfo->eventOAD;
//        updateRecordData.MAC = MAC;
        updateRecordData.filter.choice = 2;
        updateRecordData.filter.storeNo = eventCommInfo->storagenum[mtr_id];
        updateRecordData.inDatalen = sizeof(OOP_EVENT_T);
        memcpy(updateRecordData.inDataBuf, &oopEvent, sizeof(OOP_EVENT_T));

        //ͣ�ϵ��¼�
        if (eventCommInfo->eventOAD.value == 0X31060200)
        {
            memcpy(&powerEvent, &oopEvent, sizeof(OOP_EVENT_T));
            powerEvent.flag.nValue = event_3106.eventParam.cParam.flag;

            updateRecordData.inDatalen = sizeof(OOP_POWERLOG_T);
            memcpy(updateRecordData.inDataBuf, &powerEvent, sizeof(OOP_POWERLOG_T));
        }
        else
        {
            updateRecordData.inDatalen = sizeof(OOP_EVENT_T);
            memcpy(updateRecordData.inDataBuf, &oopEvent, sizeof(OOP_EVENT_T));
        }

        EVENT_FMT_DEBUG("WriteEventToDB update, infoNum=%u, OAD=%8X\n", mtr_id, eventCommInfo->eventOAD.value);

        ret = db_update_record(g_sDataCenterClient, &updateRecordData);
        if (ret != ERR_OK)
        {
            EVENT_FMT_DEBUG("failed, infoNum=%u, OAD=%8X\n", mtr_id, eventCommInfo->eventOAD.value);
            return ret;
        }
    }

    return ERR_OK;
}

/*******************************************************************************
�������ܵ�Ԫ���¼�Ԥ������
�����������������Ӧ�Ĺ��ܴ�����Ԥ�����ɸ澯���������(��ȡ�������������F25)
���������
  mtr_id  �������
�����������
��������ֵ����
�ڲ��������̣���ȡ��ǰ����������ݼ��������д���ü��������Ӧ��ȫ����Ϣ�ṹ��,��
    ���ѹ�������ڴ�,Ϊ֮��ĸ澯�����ṩ���ݻ���(���߷�ʽ,�����ѹ,����,��ǰ����
    �й�����,��һ�α��ʱ���ع���״̬,�����,���������ɵ���Զֵ����,���ѹ)
********************************************************************************/
void FillEventGlobal(mtrnum_t mtr_id, OOP_METER_T *oopMeter)
{
    uint16         cur_rate = 0;    //-���������ɵ���Զֵ����-

    //PROG_INFO_AND_EV_T prog_info;//-����һ���ṹ�壬����������������������̴�������Ϣ-
    //OOP_ENERGY_T  fw_act_en;     //�����й�����

    //-��ǰ�������ݶ�ȡ-
    event_global.vlt_a = APP_NONEWORD;
    event_global.vlt_b = APP_NONEWORD;
    event_global.vlt_c = APP_NONEWORD;
    event_global.cur_a = APP_NONELWORD;
    event_global.cur_b = APP_NONELWORD;
    event_global.cur_c = APP_NONELWORD;
    //event_global.fw_act_en = APP_NONELWORD;
    //memset(&prog_info,APP_NONECHAR,sizeof(PROG_INFO_AND_EV_T));
    //-4byte�����һ�α��ʱ��-
    //memset(&event_global.prog_time,APP_NONECHAR,4);
    event_global.power_type = oopMeter->basic.pwrType;

#if 0
    if(event_3000.eventCommInfo.checkflag == 1 || event_3001.eventCommInfo.checkflag == 1 ||
       event_3002.eventCommInfo.checkflag == 1 || event_3003.eventCommInfo.checkflag == 1 ||
       event_3005.eventCommInfo.checkflag == 1 || event_301D.eventCommInfo.checkflag == 1 ||
       event_301E.eventCommInfo.checkflag == 1 || event_3119.eventCommInfo.checkflag == 1)
    {
        READ_RECORD_T startReadRecord;
        RECORD_UNIT_T stepRecordOut;
        time_t timep = {0};
        int  ret = 0;
        OOP_DWORD3V_T curTemp;       //������
        OOP_DWORD3V_T valTemp;       //��ѹ

        time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep

        //��ȡ ����
        startReadRecord.recordOAD.road.oadMain.value = 0;
        startReadRecord.recordOAD.road.oadCols.nNum = 1;
        startReadRecord.recordOAD.logicId = 0;
        startReadRecord.recordOAD.infoNum = mtr_id;
        startReadRecord.cType = 0x00;
        startReadRecord.cStart = 0;
        startReadRecord.cEnd = timep;
        startReadRecord.sortType = 1;
        startReadRecord.MAC.nNum = 6;
        memcpy(startReadRecord.MAC.value, oopMeter->basic.tsa.add, 6);

        if (mtr_id != 0)
        {
            startReadRecord.recordOAD.road.oadCols.oad[0].value = 0x20010200;
            ret = db_read_record(g_sDataCenterClient, &startReadRecord, sizeof(OOP_DWORD3V_T), &stepRecordOut);
            if (ret == ERR_OK)
            {
                memcpy(&curTemp, stepRecordOut.record[0].outDataBuf, sizeof(OOP_DWORD3V_T));
                EVENT_FMT_TRACE("curTemp.nNum=[%u] \n", curTemp.nNum);
                if(curTemp.nNum >= 3)
                {
                    event_global.cur_c = curTemp.nValue[2];
                    EVENT_FMT_TRACE("Read Electric OK!event_global.cur_c=[%u] \n", event_global.cur_c);
                }

                if(curTemp.nNum >= 2)
                {
                    event_global.cur_b = curTemp.nValue[1];
                    EVENT_FMT_TRACE("Read Electric OK!event_global.cur_b=[%u] \n", event_global.cur_b);
                }

                if(curTemp.nNum >= 1)
                {
                    event_global.cur_a = curTemp.nValue[0];
                    EVENT_FMT_TRACE("Read Electric OK!event_global.cur_a=[%u] \n", event_global.cur_a);
                }
            }
            else
                EVENT_FMT_DEBUG("Read Electric current error!infonum=%u OAD=[0x20010200]ret=%d\n", mtr_id, ret);

            //��ȡ ��ѹ
            startReadRecord.recordOAD.road.oadCols.oad[0].value = 0x20000200;
            ret = db_read_record(g_sDataCenterClient, &startReadRecord, sizeof(OOP_DWORD3V_T), &stepRecordOut);
            if (ret == ERR_OK)
            {
                memcpy(&valTemp, stepRecordOut.record[0].outDataBuf, sizeof(OOP_DWORD3V_T));
                EVENT_FMT_TRACE("valTemp.nNum=[%u] \n", valTemp.nNum);
                if(valTemp.nNum >= 3)
                {
                    event_global.vlt_c = valTemp.nValue[2];
                    EVENT_FMT_TRACE("Read Voltage OK!event_global.vlt_c=[%u] \n", event_global.vlt_c);
                }

                if(valTemp.nNum >= 2)
                {
                    event_global.vlt_b = valTemp.nValue[1];
                    EVENT_FMT_TRACE("Read Voltage OK!event_global.vlt_b=[%u] \n", event_global.vlt_b);
                }

                if(valTemp.nNum >= 1)
                {
                    event_global.vlt_a = valTemp.nValue[0];
                    EVENT_FMT_TRACE("Read Voltage OK!event_global.vlt_a=[%u] \n", event_global.vlt_a);
                }
            }
            else
                EVENT_FMT_DEBUG("Read Voltage error!infonum=%u OAD=[0x20000200] ret=%d \n", mtr_id, ret);
        }
    }
#else
    if(event_3000.eventCommInfo.checkflag == 1 || event_3001.eventCommInfo.checkflag == 1 ||
       event_3002.eventCommInfo.checkflag == 1 || event_3003.eventCommInfo.checkflag == 1 ||
       event_3005.eventCommInfo.checkflag == 1 || event_300F.eventCommInfo.checkflag == 1 ||
       event_301D.eventCommInfo.checkflag == 1 || event_301E.eventCommInfo.checkflag == 1 ||
       event_3119.eventCommInfo.checkflag == 1)
    {
        if (g_LatestMeterReadData.valTemp[mtr_id].nNum >= 1)
            event_global.vlt_a = g_LatestMeterReadData.valTemp[mtr_id].nValue[0];
        if (g_LatestMeterReadData.valTemp[mtr_id].nNum >= 2)
            event_global.vlt_b = g_LatestMeterReadData.valTemp[mtr_id].nValue[1];
        if (g_LatestMeterReadData.valTemp[mtr_id].nNum >= 3)
            event_global.vlt_c = g_LatestMeterReadData.valTemp[mtr_id].nValue[2];
        if (g_LatestMeterReadData.curTemp[mtr_id].nNum >= 1)
            event_global.cur_a = g_LatestMeterReadData.curTemp[mtr_id].nValue[0];
        if (g_LatestMeterReadData.curTemp[mtr_id].nNum >= 2)
            event_global.cur_b = g_LatestMeterReadData.curTemp[mtr_id].nValue[1];
        if (g_LatestMeterReadData.curTemp[mtr_id].nNum >= 3)
            event_global.cur_c = g_LatestMeterReadData.curTemp[mtr_id].nValue[2];
    }
#endif

#if 0
    //��ȡ �����й�����-
    if(event_3119.eventCommInfo.checkflag == 1)
    {
        startReadRecord.recordOAD.road.oadCols.oad[0].value = 0x00100200;
        ret = db_read_record(g_sDataCenterClient, &startReadRecord, sizeof(OOP_ENERGY_T), &stepRecordOut);
        if (ret == ERR_OK)
        {
            memcpy(&fw_act_en, stepRecordOut.record[0].outDataBuf, sizeof(OOP_ENERGY_T));
            event_global.fw_act_en = fw_act_en.nValue[0];
            EVENT_FMT_TRACE("Read Electric energy OK! event_global.fw_act_en=%u \n", event_global.fw_act_en);
        }
        else
            EVENT_FMT_DEBUG("Read Electric energy error!infonum=%u OAD=[0x00100200] ret=%d\n", mtr_id, ret);
    }
#endif

    //���ѹ
    event_global.vlt_gate = oopMeter->basic.stVolt;
    EVENT_FMT_TRACE("Read event_global.vlt_gate=%u \n", event_global.vlt_gate);

    //�����
    event_global.cur_gate = (uint32)oopMeter->basic.stAmp*100;  //��100��3λС���ĵ����Ƚ�
    EVENT_FMT_TRACE("Read event_global.cur_gate=%u \n", event_global.cur_gate);
/*
    //������ƽ����
    startReadRecord.recordOAD.road.oadMain.value = 0x20270200;
    ret = db_read_record(g_sDataCenterClient, &startReadRecord, sizeof(uint16), &stepRecordOut);
    if (ret != ERR_OK)
    {
        //-���������ɵ���Զֵ����-
        cur_rate = 120;    //-Ĭ��Ϊ120-
    }
    else
    {
        memcpy(&cur_rate, stepRecordOut.record[0].outDataBuf, sizeof(uint16));
        if(cur_rate == 0||cur_rate == 0xffff)
        {
            cur_rate = 120;
        }
    }
*/
    EVENT_FMT_DEBUG("������%u�¼����,��������%u,3���ѹvlt[0]=%d,vlt[1]=%d,vlt[2]=%d\n",mtr_id,event_global.power_type,event_global.vlt_a,event_global.vlt_b,event_global.vlt_c);
    EVENT_FMT_DEBUG("3�����cur[0]=%ld,cur[1]=%ld,cur[2]=%ld\n",event_global.cur_a,event_global.cur_b,event_global.cur_c);

    //-���������ɵ���Զֵ����-
    cur_rate = 120; //-Ĭ��Ϊ120-
    event_global.cur_lmt = event_global.cur_gate*cur_rate/100;//-��������������-
    EVENT_FMT_TRACE("Read event_global.cur_lmt=%u \n", event_global.cur_lmt);

    event_global.cb_interval = CB_INTERVAL;//-��������Ҫ��Ϊ1����-
}

/*****************************************************************************
�������ܵ�Ԫ��
�������������������Ը澯
����������������
�����������
��������ֵ��
�ڲ��������̣��б�������a)    90��<��Ӧ���ѹ������н�< 270��
                        b)    �������߲���B��
       �������ߣ�UI�н���90��270��Ϊ������
       �������ߣ���A��нǣ���120��300��Ϊ��������C��нǣ���60��240��Ϊ������
***************************************************************************-*/
void CurReverseEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType)
{
    uint8  i;
    uint16 vlt_lmt = 0;                         //��ѹ���෧ֵ
    uint16 ccAngle[3],vvAngle[3];               //��������ѹ���
    uint32 cur_para=0;                          //������ֵ
    uint32 dwERFlgTmp = 0;
    uint16 angle[3];                            //-��ѹ�����н�-
    int16  bLen;
    uint8  bCurReverseStat=0,bLastCurReverseStat=0;
    uint8  A_polarity_last = 0,B_polarity_last = 0,C_polarity_last = 0;
    uint8  bEventFlag = 0;
    float32  tmpFloat;
    OOP_EVTSOURCE_T eventSource; //�¼�����Դ
    OOP_EVTBUF_T    eventData;   //�¼���������
    //int ret = 0;

    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

#if DELIVER_LOG_EN == 1
    static uint8 writeTimes = 0;
#endif

#if HEBEI_FUN == 1//�ֳ�Ĭ�ϳ���ʱ��Ϊ2����
    static  uint8 phaselastTimes[3] = {0};    //��ʾ����ʱ��
    uint16  lastTimesLmt = 0;                 //����ʱ����ֵ
    char    paralimit;
    MtrProperty mtrPro;
    //����������A�������쳣������
    //��ͨ���������������������ɳ�ǰ��ѹ30��仯Ϊ�ͺ��ѹ50�㣬�������ǹ�˾�ϱ�����
    //�쳣�ǲ��Եģ�Ҫ��ҲӦ���ǵ��������ԣ�����Ϊ�����������ֳ���Ա��Ϊ����仯��˲��ģ���˲�Ӧ���ϱ��κ��¼���
    //���Ϲ�ͨ�������й�˾Фѧѫȷ�ϵ����ݣ������θ���������ˮ�й��繫˾��ά���೤��13091192670����ϵ��Ҫ�������з���Ա�������ϵ��

    //����޸�Ϊ�����ж�2���Ӳ��㳬ʱ
    if (GetXParaInfo(mtr_id, &mtrPro) == 0 &&  mtrPro.category == MTR_AC)
    {
        if(read_pdata_xram(PARA_ERC11_FILE, &paralimit, 0, 1) == 0 && paralimit == 0x55)
            lastTimesLmt = 12;
    }
#endif

    cur_para = event_global.cur_gate*2/100;    //-Ĭ��Ϊ��������2%-
    EVENT_FMT_TRACE("������������ֵ cur_para=%lu \n",cur_para);

    //-�������Ĭ��Ϊ75%-
    vlt_lmt = event_global.vlt_gate*75/100;
    if (mtr_id == 0 && para_query_float32(0x8028, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt = tmpFloat*10;
    }

    if (event_global.power_type == 2)            //-��������-
    {
        if((Int_ABS(event_global.cur_a)<=(cur_para))||(Int_ABS(event_global.cur_b)<=(cur_para))||(Int_ABS(event_global.cur_c)<=(cur_para)))
        {
            //-�б��������ǰֵ�ܵ���������֮��ֵ,����-
            EVENT_FMT_DEBUG("�б��������ǰֵ�ܵ���������֮��ֵ,����\n");
            //write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);
            return;
        }

        if((event_global.vlt_a<=vlt_lmt)||(event_global.vlt_b<=vlt_lmt)||(event_global.vlt_c<=vlt_lmt))
        {
            //-��ȱ�಻��-
            EVENT_FMT_DEBUG("ȱ�಻��\n");
            //write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);

            return;
        }
    }

    EVENT_FMT_TRACE("���߷�ʽ = %d (2:3/3; 3:3/4)\n",event_global.power_type);

    if (event_global.power_type == 1)            //-��������-
    {
        if((Int_ABS(event_global.cur_a)<=(cur_para))||(Int_ABS(event_global.cur_c)<=(cur_para)))
        {
            //-�б��������ǰֵ�ܵ���������֮��ֵ,����-
            EVENT_FMT_DEBUG("�б��������ǰֵ�ܵ���������֮��ֵ,����\n");
            //write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);

            return;
        }

        if((event_global.vlt_a<=vlt_lmt)||(event_global.vlt_c<=vlt_lmt))
        {
            //-��ȱ�಻��-
            EVENT_FMT_DEBUG("ȱ�಻��\n");
            //write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);

            return;
        }
    }

    vvAngle[0] = 0;                                //-a���ѹ���-
    vvAngle[1] = event_3119.oopWord4V[mtr_id].nValue[1];                            //-b���ѹ���-
    vvAngle[2] = event_3119.oopWord4V[mtr_id].nValue[2];                            //-c���ѹ���-
    ccAngle[0] = event_3119.oopWord4C[mtr_id].nValue[0];                            //-a��������-
    ccAngle[1] = event_3119.oopWord4C[mtr_id].nValue[1];                            //-b��������-
    ccAngle[2] = event_3119.oopWord4C[mtr_id].nValue[2];                            //-c��������-

    if ((ccAngle[0] == APP_NONELWORD) && (ccAngle[1] == APP_NONELWORD) && (ccAngle[2] == APP_NONELWORD))
    {
        EVENT_FMT_DEBUG( "���������Чֱ�ӷ���\n");

        return;
    }

    EVENT_FMT_TRACE("a���ѹ���=%d,b���ѹ���=%d,c���ѹ���=%d,\n", vvAngle[0],vvAngle[1],vvAngle[2]);
    EVENT_FMT_TRACE("a��������=%d,b��������=%d,c��������=%d,\n", ccAngle[0],ccAngle[1],ccAngle[2]);

    if (read_pdata_xram("^Event",(char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4)>=0)
    {
        //-��ȡ�¼�״̬��־-
        if(((vvAngle[1]+vvAngle[2])>=60) && (vvAngle[1]>vvAngle[2]) && (event_global.power_type == 2))
        {
            return;
        }
        if(((vvAngle[1]+vvAngle[2])>=60) && (vvAngle[1]>vvAngle[2]) && (event_global.power_type == 1))
        {
            return;
        }
    }

    if (dwERFlgTmp == 0xffffffff) //�ļ�ϵͳ��ʼΪ0xffֵ
    {
        dwERFlgTmp = 0;
    }

    EVENT_FMT_DEBUG("��ȡ�¼�״̬1 %08X\n", dwERFlgTmp);

    if((ccAngle[0] == 0) && (ccAngle[1] == 0) && (ccAngle[2] == 0))
    {
        return;
    }

    memset(vc_angle, 0x0, sizeof(vc_angle));
    for(i=0; i<3; i++)
    {
        if(vvAngle[i] <= ccAngle[i])
            angle[i] = ccAngle[i] - vvAngle[i];
        else
            angle[i] =  360 + ccAngle[i] - vvAngle[i];
        vc_angle[i] = angle[i];
    }

    if (event_global.power_type == 1)
    {
        angle[0] -= 30;
        if(angle[0]<0)
            vc_angle[0] = angle[0]+360;
        else
            vc_angle[0] = angle[0];
        angle[2] += 30;
        if(angle[2]>360)
            vc_angle[2] = angle[2]-360;
        else
            vc_angle[2] = angle[2];
    }

    EVENT_FMT_TRACE("ת����       a���ѹ���=%d,b���ѹ���=%d,c���ѹ���=%d,\n", vvAngle[0],vvAngle[1],vvAngle[2]);
    EVENT_FMT_TRACE("ת����       a��������=%d,b��������=%d,c��������=%d,\n", ccAngle[0],ccAngle[1],ccAngle[2]);
    EVENT_FMT_TRACE("��ѹ�����н� a��н�=%d,    b��н�=%d,    c��н�=%d,\n", vc_angle[0],vc_angle[1],vc_angle[2]);

    for(i = 0; i < 3; i++)
    {
        //-ѭ���������-
        if( event_global.power_type == 2)    //-��������-
        {
            if((vc_angle[i]>90) && (vc_angle[i]<270))
                bCurReverseStat |= (0x01<<(i*2));
            else
                bCurReverseStat |= (0x02<<(i*2));
        }

        if( event_global.power_type == 1)    //-��������-
        {
            if(i != 1)  //-B�಻��-
            {
                if((vc_angle[i]>90) && (vc_angle[i]<270))
                    bCurReverseStat |= (0x01<<(i*2));
                else
                    bCurReverseStat |= (0x02<<(i*2));
            }
            else
            {
                bCurReverseStat |= 0;
            }
        }
    }

#if 0
    if(bfirst[mtr_id]==0)
    {
        bfirst[mtr_id]=1;
        write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);
#if DEBUG_EN_9 == 1
        DPRINT("�״μ�⣬д��ԭʼֵ������\n");
#endif
        return;
    }
#endif

    bLen =  read_pdata_xram("^Event", (char *)&bLastCurReverseStat, (CurReverseRecAddr+mtr_id), 1);
    if (bLen < 0)
    {
        EVENT_FMT_DEBUG("��ȡ�¼�״̬ʧ��");
        return;
    }

    if(bLastCurReverseStat == 0xff)
    {
        write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id), 1);
        EVENT_FMT_DEBUG("�¼�������־Ϊ�Ƿ�������д��\n");

        return;
    }

    EVENT_FMT_TRACE("�¼�������־ bLastCurReverseStat = %02X        bCurReverseStat = %02X\n",bLastCurReverseStat,bCurReverseStat);

    A_polarity_last = bLastCurReverseStat&0x03 ;
    B_polarity_last = (bLastCurReverseStat&0x0c)>>2 ;
    C_polarity_last = (bLastCurReverseStat&0x30)>>4 ;

#if HEBEI_FUN == 1
    if(bType==2)//-���������Իָ����-
    {
        if((bCurReverseStat&0x03)==2)
        {
            if(A_polarity_last==1)//�ϴη���
            {
                EVENT_FMT_DEBUG("A ��ָ� :phaselastTimes[0] = %d  %d\n",phaselastTimes[0],lastTimesLmt);
                if(phaselastTimes[0]-- <= lastTimesLmt)
                {
                    bEventFlag |= 0x01; //-a�෴���Իָ�-
                    phaselastTimes[0] = 0;
                }
            }
            else
            {
                if(phaselastTimes[0] != 0)
                    phaselastTimes[0] = 0;
            }
        }

        if(((bCurReverseStat&0x0c)>>2)==2)
        {
            if(B_polarity_last==1)//�ϴη���
            {
                EVENT_FMT_DEBUG("B ��ָ� :phaselastTimes[1] = %d  %d\n",phaselastTimes[1],lastTimesLmt);
                if(phaselastTimes[1]-- <= lastTimesLmt)
                {
                    bEventFlag |= 0x02; //-a�෴���Իָ�-
                    phaselastTimes[1] = 0;
                }
            }
            else
            {
                if(phaselastTimes[1] != 0)
                    phaselastTimes[1] = 0;
            }
        }

        if(((bCurReverseStat&0x30)>>4)==2)
        {
            if(C_polarity_last==1)//�ϴη���
            {
                EVENT_FMT_DEBUG("C ��ָ� :phaselastTimes[2] = %d  %d\n",phaselastTimes[2],lastTimesLmt);
                if(phaselastTimes[2]-- <= lastTimesLmt)
                {
                    bEventFlag |= 0x04; //-a�෴���Իָ�-
                    phaselastTimes[2] = 0;
                }
            }
            else
            {
                if(phaselastTimes[2] != 0)
                    phaselastTimes[2] = 0;
            }
        }

    }
    else if(bType==1)//-���������Է������-
    {
        if((bCurReverseStat&0x03)==1)//����
        {
            if(A_polarity_last!=1)//��δ����
            {
                EVENT_FMT_DEBUG("A �෢�� :phaselastTimes[0] = %d  %d\n",phaselastTimes[0],lastTimesLmt);
                if(phaselastTimes[0]++ >= lastTimesLmt)
                {
                    bEventFlag |= 0x01; //-a�෴���Է���-
                    phaselastTimes[0] = lastTimesLmt*2;
                }
            }
            else
            {
                if(phaselastTimes[0] != lastTimesLmt * 2)
                    phaselastTimes[0] = lastTimesLmt*2;
            }
        }

        if(((bCurReverseStat&0x0c)>>2)==1)//����
        {
            if(B_polarity_last!=1)//��δ����
            {
                EVENT_FMT_DEBUG("B �෢�� :phaselastTimes[1] = %d  %d\n",phaselastTimes[1],lastTimesLmt);
                if(phaselastTimes[1]++ >= lastTimesLmt)
                {
                    bEventFlag |= 0x02; //-a�෴���Է���-
                    phaselastTimes[1] = lastTimesLmt*2;
                }
            }
            else
            {
                if(phaselastTimes[1] != lastTimesLmt * 2)
                    phaselastTimes[1] = lastTimesLmt*2;
            }
        }


        if(((bCurReverseStat&0x30)>>4)==1)//����
        {
            if(C_polarity_last!=1)//��δ����
            {
                EVENT_FMT_DEBUG("C �෢�� :phaselastTimes[2] = %d  %d\n",phaselastTimes[2],lastTimesLmt);
                if(phaselastTimes[2]++ >= lastTimesLmt)
                {
                    bEventFlag |= 0x04; //-a�෴���Է���-
                    phaselastTimes[2] = lastTimesLmt*2;
                }
            }
            else
            {
                if(phaselastTimes[2] != lastTimesLmt * 2)
                    phaselastTimes[2] = lastTimesLmt*2;
            }
        }

    }

#else
    if(bType==2)//-���������Իָ����-
    {
        if(((bCurReverseStat&0x03)==2)&&(A_polarity_last==1))
            bEventFlag |= 0x01;    //-a�෴���Իָ�-
        if((((bCurReverseStat&0x0c)>>2)==2)&&(B_polarity_last==1))
            bEventFlag |= 0x02;    //-b�෴���Իָ�-
        if((((bCurReverseStat&0x30)>>4)==2)&&(C_polarity_last==1))
            bEventFlag |= 0x04;    //-c�෴���Իָ�-
    }
    else if(bType==1)//-���������Է������-
    {
        if(((bCurReverseStat&0x03)==1)&&(A_polarity_last!=1))
            bEventFlag |= 0x01;    //-a�෴���Է���-
        if((((bCurReverseStat&0x0c)>>2)==1)&&(B_polarity_last!=1))
            bEventFlag |= 0x02;    //-b�෴���Է���-
        if((((bCurReverseStat&0x30)>>4)==1)&&(C_polarity_last!=1))
            bEventFlag |= 0x04;    //-c�෴���Է���-
    }
#endif

    if(bEventFlag)
    {
#if HEBEI_FUN == 1
        if(bEventFlag & 0x01)
        {
            EVENT_FMT_TRACE("A��CheckERC_3119�¼�\n");
            bLastCurReverseStat= (bLastCurReverseStat&0xfc)|(bCurReverseStat & 0x03);
        }
        if (bEventFlag & 0x02)
        {
            EVENT_FMT_TRACE("B��CheckERC_3119�¼�\n");
            bLastCurReverseStat= (bLastCurReverseStat&0xf3)|(bCurReverseStat & 0x0c);
        }
        if(bEventFlag & 0x04)
        {
            EVENT_FMT_TRACE("C��CheckERC_3119�¼�\n");
            bLastCurReverseStat= (bLastCurReverseStat&0xcf)|(bCurReverseStat & 0x30);
        }
        bCurReverseStat = bLastCurReverseStat;
#endif
        if(bType==1)
        {
            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                SaveFileRecBfg(ERC_NO_09,(bEventFlag&0x07)|0x80,mtr_id);//��¼����������(��������)����
            }
        }
        else if(bType==2)
        {
            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                SaveFileRecBfg(ERC_NO_09,(bEventFlag&0x07),mtr_id);//��¼����������(��������)�ָ�
            }
        }

        write_pdata_xram("^Event", (char *)&bCurReverseStat, (CurReverseRecAddr+mtr_id),1);

        //TODO �¼�д�����ݿ�
        //�¼�����Դ
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter->basic.tsa;

        //�¼���������
        eventData.oadNum = 2;
        eventData.cols.nNum = 2;
        eventData.cols.oad[0].value = 0X20034200;
        eventData.cols.oad[1].value = 0X00104201;
        eventData.idLen[0] = sizeof(OOP_WORD4V_T);
        eventData.idLen[1] = sizeof(OOP_ENERGY_T);
        eventData.len = eventData.idLen[0] + eventData.idLen[1];
        memcpy(eventData.buf, &event_3119.oopWord4C[mtr_id], eventData.idLen[0]);
        memcpy(eventData.buf+eventData.idLen[0], &event_3119.oopEnergys[mtr_id], eventData.idLen[1]);

        if(bType==1)//����
        {
            //д���ݿ�
            int ret = WriteEventToDB(mtr_id, &event_3119.eventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("������mtr_id=%hu  �����������¼���¼CheckERC_3119ʧ�ܷ���%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_TRACE("������mtr_id=%hu  �����������¼���¼CheckERC_3119�ɹ� storagenum=%u \n", mtr_id, event_3119.eventCommInfo.storagenum[mtr_id]);

        }
        else if(bType==2)
        {
            //�ָ��¼���¼����
            int ret = WriteEventToDB(mtr_id, &event_3119.eventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("������mtr_id=%hu  �����������¼��ָ���¼CheckERC_3119ʧ�ܷ���%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_TRACE("������mtr_id=%hu  �����������¼��ָ���¼CheckERC_3119�ɹ� , storagenum=%u !\n", mtr_id, event_3119.eventCommInfo.storagenum[mtr_id]);
        }
    }
}

/*****************************************************************************
�������ܵ�Ԫ��
������������ѹ����澯
����������������
�����������
��������ֵ��
�ڲ��������̣����ݵ�ǰ������ĵ�����ѹ����ֵ����ص����޲����ж��Ƿ���࣬
��������ø澯�������ɴ��������ɸ澯���ݲ���д�����ϱ�����
***************************************************************************-*/
void VltBreakEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter,uint8 bType)
{
    int    ret = 0;
    uint16 vlt[3] = {0};                        //-��ǰ�����ѹ-
    uint32 cur[3] = {0};                        //-��ǰ�������-
    uint32 vlt_lmt = 0;                         //-��ѹ�ж�ֵ-
    uint16 lastTimesLmt = 0;                    //����ʱ����ֵ
    uint16 vltgate = 0;                         //���ѹ    �������û�ж������ѹ��ֱ�ӷ��أ�����û��ʹ��event_global.vlt_gate
    uint32 vlt_resume = 0;                      //��ѹ���ࡢȱ��ָ�����
    static uint16 lastTimes[4][3][MTR485_MAX_NUM] = {{{0}}};    //�¼�����/�ָ�����ʱ��
    uint32 vlt_lmt1 = 0;                        //��ѹ������ֵ(ʧѹ��ֵ)
    float32  tmpFloat   = 0;

    uint32 cur_lmt = 0;                         //�����ж�ֵ(��������)
    uint32 ERFlg = 0;
    uint8  order = 0;                           //��λ
    uint8  is_now_flag = 0;
    int16  len = 0;
    uint8  bData = 0;
    uint8  bphase = 0;
    uint32 dwERFlgTmp = 0;
    uint16 dTemp = 0;
    uint8  tmp_flag = 0;
    OOP_EVTSOURCE_T eventSource; //�¼�����Դ
    OOP_EVTBUF_T    eventData;   //�¼���������

    //-��ȡʵʱ��ѹ������-
    vlt[0] = event_global.vlt_a;                //-ȡa���ѹ-
    vlt[1] = event_global.vlt_b;                //-ȡb���ѹ-
    vlt[2] = event_global.vlt_c;                //-ȡc���ѹ-
    cur[0] = Int_ABS(event_global.cur_a);       //-ȡa�����-
    cur[1] = Int_ABS(event_global.cur_b);       //-ȡb�����-
    cur[2] = Int_ABS(event_global.cur_c);       //-ȡc�����-

    if ((vlt[0] == APP_NONEWORD) && (vlt[1] == APP_NONEWORD) && (vlt[2] == APP_NONEWORD))
    {
        EVENT_FMT_DEBUG( "��ѹ��Чֱ�ӷ���\n");

        return;
    }

    if((cur[0] == 0x0)&&(vlt[0] == 0x0)&&(cur[1] == 0x0)&&(vlt[1] == 0x0)&&(cur[2] == 0x0)&&(vlt[2] == 0x0))
    {
        EVENT_FMT_DEBUG("��ѹ������Ϊ0����ͣ��\n");

        return;//-��ѹ������Ϊ0����ͣ��-
    }

    //-��ȡ��ѹ�������-
    vlt_lmt = event_global.vlt_gate*78/100;  /*- �������Ĭ��Ϊ78%-*/
    if (mtr_id == 0 && para_query_float32(0x8028, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt = tmpFloat*10;
    }

    EVENT_FMT_DEBUG("��ѹ��������vlt_lmt=%lu\n",vlt_lmt);
    //-��ȡ��ѹ������ֵ,��Ϊʧѹ ����-
    vlt_lmt1 = event_global.vlt_gate*78/100;  /*- �������Ĭ��Ϊ78%-*/
    EVENT_FMT_DEBUG("��ѹʧѹ����vlt_lmt1=%lu\n",vlt_lmt1);

    //-���ѹ-
    if(event_global.power_type == 2)            //-��������-
        vltgate = 2200;
    else if(event_global.power_type == 1)       //-��������-
        vltgate = 1000;
    else
        vltgate = 2200;

    EVENT_FMT_DEBUG("û�ж�ȡ����ǰ������Ķ��ѹ��ȡĬ��ֵ    vltgate = %d\n",vltgate);

    vlt_resume = vltgate * 85/100;              //-ʧѹ�Ͷ���ָ�����Ĭ��Ϊ85%-
    EVENT_FMT_DEBUG("ʧѹ�Ͷ���ָ�����Ĭ��vlt_resume = %lu\n",vlt_resume);

    //-ʧѹ����ʱ����ֵ-
    lastTimesLmt = 1;
    if((lastTimesLmt == 0) || (lastTimesLmt == 0xffff))
    {
        lastTimesLmt = 1;
    }

    EVENT_FMT_DEBUG("ʧѹ�������ʱ����ֵlastTimesLmt=%d\n",lastTimesLmt);

    cur_lmt = event_global.cur_gate * 5 /1000;  //-��ö�ȱ�������ֵ-
    EVENT_FMT_DEBUG("�������ֵevent_global.cur_gate = %lu    ����������ֵcur_lmt = %lu\n",event_global.cur_gate,cur_lmt);

    //-ѭ���������-
    for(order = 0; order < 3; order++)
    {
        if((cur[order] == APP_NONELWORD)||(vlt[order] == APP_NONEWORD)
                ||(cur[order] == NONE_LWORD)||(vlt[order] == NONE_WORD))    //-������ѹΪ��Ч-
        {
            continue;
        }

        if(event_global.power_type == 1 && order == 1)    //��������B�಻�ж�
        {
            continue;
        }

        tmp_flag = 0;
        if(bType == 1)              //-��ѹ����-
        {
            tmp_flag = (cur[order] <= cur_lmt) && (vlt[order] <= vlt_lmt);
        }
        else if(bType == 2)         //-��ѹ����ָ�-
        {
            tmp_flag = !((cur[order] <= cur_lmt) && (vlt[order] <= vlt_lmt));
        }
        else if(bType == 3)         //-ʧѹ�����������-
        {
#if 0//PHASE_LOW_EVENT_TYPE0 == 1
            tmp_flag = (cur[order] > cur_lmt) && ((vlt[order] <= vlt_lmt1) && (vlt[order] > vlt_lmt));
#else
            tmp_flag = (cur[order] > cur_lmt) && (vlt[order] <= vlt_lmt1);
#endif
        }
        else if(bType == 4)         //-ʧѹ�ָ�����-
        {
#if 0//PHASE_LOW_EVENT_TYPE0 == 1
            tmp_flag = !((cur[order] > cur_lmt) && ((vlt[order] <= vlt_lmt1) && (vlt[order] > vlt_lmt)));
#else
            tmp_flag = !((cur[order] > cur_lmt) && (vlt[order] <= vlt_lmt1));
#endif
        }
        else
        {
            return;
        }

        if(tmp_flag)                //���������Ӧ�ķ�������������Ӧ��ʱ������ۼ�
        {
            if(lastTimes[bType-1][order][mtr_id] >= lastTimesLmt-1)
            {
                is_now_flag = 1;
            }
            else
            {
                lastTimes[bType-1][order][mtr_id]++;
                is_now_flag = 0;
            }
            EVENT_FMT_DEBUG("��%d�����㷢����������ʼͳ��ʱ�䣬Ŀǰʱ��lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
        }
        else
        {
            EVENT_FMT_DEBUG("��%d�಻���㷢��������ʱ������\n",order+1);
            lastTimes[bType-1][order][mtr_id] = 0;
            is_now_flag = 0;
        }

        if((bType == 1)||(bType ==2))    //-���༰�ָ����-
            len =  read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+3), 1);
        else    //-ʧѹ���ָ����-
            len =  read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+4), 1);

        if (bData == 0xff) //�ļ�ϵͳ��ʼΪ0xffֵ
        {
            bData = 0;
        }

        if (len < 0)
        {
            EVENT_FMT_DEBUG("��ȡ�¼�״̬ʧ��");
            return;
        }

        EVENT_FMT_DEBUG("��ȡ�¼�״̬%02X\n", bData);

        if((bType == 1)||(bType ==3))    //-�¼��������-
        {
            EVENT_FMT_DEBUG("�¼��������\n");
            if(is_now_flag)
            {
                EVENT_FMT_DEBUG("�¼�����������㷢������\n");
                switch(order)
                {
                case 0:
                    if(!(bData&A_PHASE))
                    {
                        bData |= A_PHASE;    //-A��-
                        bphase |= A_PHASE;    //-A��-
                        if(bType == 1)
                        {
                            ERFlg = BREAK_PHASE;//-������-
                            EVENT_FMT_DEBUG("��A�������\n");
                        }
                        else
                        {
                            ERFlg = LOSE_VOLT;    //-ʧѹ���-
                            EVENT_FMT_DEBUG("��A��ʧѹ���\n");
                        }
                    }
                    else
                    {
                        EVENT_FMT_DEBUG("A���Ѿ���¼��\n");
                    }
                    break;
                case 1:
                    if(!(bData&B_PHASE))
                    {
                        bData |= B_PHASE;
                        bphase |= B_PHASE;
                        if(bType == 1)
                        {
                            ERFlg = BREAK_PHASE;//-������-
                            EVENT_FMT_DEBUG("��B�������\n");
                        }
                        else
                        {
                            ERFlg = LOSE_VOLT ;    //-ʧѹ���-
                            EVENT_FMT_DEBUG("��B��ʧѹ���\n");
                        }
                    }
                    else
                    {
                        EVENT_FMT_DEBUG("B���Ѿ���¼��\n");
                    }
                    break;
                case 2:
                    if(!(bData&C_PHASE))
                    {
                        bData |= C_PHASE;
                        bphase |= C_PHASE;
                        if(bType == 1)
                        {
                            ERFlg = BREAK_PHASE;//-������-
                            EVENT_FMT_DEBUG("��C�������\n");
                        }
                        else
                        {
                            ERFlg = LOSE_VOLT ;    //-ʧѹ���-
                            EVENT_FMT_DEBUG("��C��ʧѹ���\n");
                        }
                    }
                    else
                    {
                        EVENT_FMT_DEBUG("C���Ѿ���¼��\n");
                    }
                    break;
                default:
                    break;
                }
                if(bType == 1)
                    write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+3), 1);
                else
                    write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+4), 1);
            }
        }
        else//-�¼��ָ����-
        {
            if((is_now_flag)&&( bData &(1<<order)))
            {
                EVENT_FMT_DEBUG("�¼��ָ����\n");
                switch(order)
                {
                case 0:
                    bData &= ~A_PHASE;    //-���A����-
                    bphase |= A_PHASE;
                    if(bType == 2)
                    {
                        ERFlg = BREAK_PHASE;//-������-
                        EVENT_FMT_DEBUG("��A�����ָ����\n");
                    }
                    else
                    {
                        ERFlg = LOSE_VOLT;    //-ʧѹ���-
                        EVENT_FMT_DEBUG("��A��ʧѹ�ָ����\n");
                    }
                    break;

                case 1:
                    bData &= ~B_PHASE;
                    bphase |= B_PHASE;
                    if(bType == 2)
                    {
                        ERFlg = BREAK_PHASE;//-������-
                        EVENT_FMT_DEBUG("��B�����ָ����\n");
                    }
                    else
                    {
                        ERFlg = LOSE_VOLT;    //-ʧѹ���-
                        EVENT_FMT_DEBUG("��B��ʧѹ�ָ����\n");
                    }
                    break;

                case 2:
                    bData &= ~C_PHASE;
                    bphase |= C_PHASE;
                    if(bType == 2)
                    {
                        ERFlg = BREAK_PHASE;//-������-
                        EVENT_FMT_DEBUG("��C�����ָ����\n");
                    }
                    else
                    {
                        ERFlg = LOSE_VOLT;    //-ʧѹ���-
                        EVENT_FMT_DEBUG("��C��ʧѹ�ָ����\n");
                    }
                    break;

                default:
                    break;
                }
                if(bType == 2)
                    write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+3), 1);
                else
                    write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+4), 1);
            }
        }
    }

//#if DEBUG_EN_10 == 1
    if((bType == 1)&&(is_now_flag)&&(ERFlg == BREAK_PHASE))
    {
    	yx_calculate_sta_set(CALC_YX_PHASE_LOSS, 0, 1);
        //EVENT_FMT_DEBUG("��ѹ���෢��\n");
    }
    if((bType == 3)&&(is_now_flag)&&(ERFlg == LOSE_VOLT))
    {
    	yx_calculate_sta_set(CALC_YX_VOLT_LOSS, 0, 1);
        //EVENT_FMT_DEBUG("��ѹʧѹ����\n");
    }
    if((bType == 2)&&(is_now_flag)&&(ERFlg == BREAK_PHASE))
    {
    	yx_calculate_sta_set(CALC_YX_PHASE_LOSS, 0, 0);
        //EVENT_FMT_DEBUG("��ѹ����ָ�\n");
    }
    if((bType == 4)&&(is_now_flag)&&(ERFlg == LOSE_VOLT))
    {
    	yx_calculate_sta_set(CALC_YX_VOLT_LOSS, 0, 0);
        //EVENT_FMT_DEBUG("��ѹʧѹ�ָ�\n");
    }
//#endif

    if(ERFlg)//-�¼�������ָ�-
    {
        uint32  aTemp = 0;
        EVENT_COMM_INFO* pCommInfo = NULL;

        if((bType == 1)&&(ERFlg == BREAK_PHASE))
        {
            //-���෢���ű�������¼�-
            len = read_pdata_xram("^Event", (char *)&aTemp, (BreakphaseAddr+(mtr_id)*4), 4);
            if(len>=0)
            {
                aTemp++;//-��¼����¶������-
                write_pdata_xram("^Event", (char *)&aTemp, (BreakphaseAddr+(mtr_id)*4), 4);
            }
        }

        if((bType==3)&&(ERFlg == LOSE_VOLT))
        {
            if(read_pdata_xram("^Event", (char *)&dTemp, (LostvltAddr+(mtr_id)*2), 2)>=0)
                dTemp++;    //-ʧѹ�¼�����,�����б仯-
            write_pdata_xram("^Event", (char *)&dTemp, (LostvltAddr+(mtr_id)*2), 2);
        }

        //�¼�����Դ
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter->basic.tsa;

        //�¼���������
        eventData.oadNum = 2;
        eventData.cols.nNum = 2;
        eventData.cols.oad[0].value = 0X20002200;
        eventData.cols.oad[1].value = 0X20012200;
        eventData.idLen[0] = sizeof(OOP_WORD3V_T);
        eventData.idLen[1] = sizeof(OOP_WORD3V_T);
        eventData.len = eventData.idLen[0] + eventData.idLen[1];

        if (bType < 3)
        {
            memcpy(eventData.buf, &event_3003.oopWord3V[mtr_id], eventData.idLen[0]);
            memcpy(eventData.buf+eventData.idLen[0], &event_3003.oopWord3C[mtr_id], eventData.idLen[1]);
            pCommInfo = &event_3003.eventCommInfo;
        }
        else {
            memcpy(eventData.buf, &event_3000.oopWord3V[mtr_id], eventData.idLen[0]);
            memcpy(eventData.buf+eventData.idLen[0], &event_3000.oopWord3C[mtr_id], eventData.idLen[1]);
            pCommInfo = &event_3000.eventCommInfo;
        }

        //֪ͨ104Э���ȡ
        if((bType == 2)||(bType == 4))//-�¼��ָ�-
        {
            len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);
            dwERFlgTmp &= ~ERFlg;//-����ָ���ǵ��˲�����ȫ����������-
            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);

            //�����ʧѹʹ��ͬһ����ţ������͵����λ�����֣����λΪ1��ʾ���࣬Ϊ0��ʾʧѹ
            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                if(bType == 2)
                    SaveFileRecBfg(ERC_NO_10|0x80,(bphase&0x07),mtr_id);
                else if (bType == 4)
                    SaveFileRecBfg(ERC_NO_10,bphase&0x07,mtr_id);
            }

            //�ָ��¼���¼����
            ret = WriteEventToDB(mtr_id, pCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("������mtr_id=%hu  �����¼��ָ���¼CheckERC_3003ʧ�ܷ���%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_TRACE("������mtr_id=%hu  �����¼��ָ���¼CheckERC_3003�ɹ� , storagenum=%u !\n", mtr_id, pCommInfo->storagenum[mtr_id]);
        }
        else//-�¼�����-
        {
            len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);
            dwERFlgTmp |= ERFlg;//-���淢����ǵ��˲�����ȫ����������-
            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);

            //�����ʧѹʹ��ͬһ����ţ�����λ�����λ�����֣����λΪ1��ʾ���࣬Ϊ0��ʾʧѹ
            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                if(bType == 1)
                    SaveFileRecBfg(ERC_NO_10|0x80,(bphase&0x07)|0x80,mtr_id);
                else if (bType == 3)
                    SaveFileRecBfg(ERC_NO_10,     (bphase&0x07)|0x80,mtr_id);
            }

            //д���ݿ�
            ret = WriteEventToDB(mtr_id, pCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("������mtr_id=%hu  �����¼���¼CheckERC_3003ʧ�ܷ���%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("������mtr_id=%hu  �����¼���¼CheckERC_3003�ɹ� storagenum=%u \n", mtr_id, pCommInfo->storagenum[mtr_id]);
        }
    }
}

/*****************************************************************************
�������ܵ�Ԫ��
����������������澯
����������������
�����������
��������ֵ��1�������࣬0��������
�ڲ��������̣��������ý����������¼������������Դ��������Real_cal.cȫ�ֱ���
       ��ע����04�¼��淶����ȱ��ʱ����������
***************************************************************************-*/
void PhaseReverseEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter)
{
    uint8    lastTimes    = 0;      //-��������/�ָ�ʱ�䣬0����ѹ������1������������-
    uint8    lastTimesLmt = 1;      //-����ʱ����ֵ��Ĭ��1����-
    int16    len          = 0;
    uint8    bData        = 0;
    uint32   ERFlg        = 0;      //-��ѹ���������� 1��������2���ָ�-
    uint8    bType        = 0;      //-0����ѹ��������/�ָ���1��������������/�ָ�-
    uint8    happenFlg    = 0;
    uint8    recoverFlg   = 0;
    uint32   dwERFlgTmp   = 0;
    uint16   vlt_lmt      = 0;
    float32  tmpFloat     = 0;

    //-�������Ĭ��Ϊ75%-
    vlt_lmt = event_global.vlt_gate*75/100;
    if (mtr_id == 0 && para_query_float32(0x8028, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt = tmpFloat*10;
    }

    EVENT_FMT_DEBUG("��ѹ��������=%d\n", vlt_lmt);

    //-�������ж�-
    if (event_global.vlt_a == APP_NONEWORD || event_global.vlt_b == APP_NONEWORD || event_global.vlt_c == APP_NONEWORD)
    {
        EVENT_FMT_DEBUG("�������ߵ�ѹ��Чʱ����������\n");

        return;
    }

    if( (event_global.vlt_a<vlt_lmt) || (event_global.vlt_b<vlt_lmt)
       || (event_global.vlt_c<vlt_lmt))    //-��ȱ��ʱ����������-
    {
        EVENT_FMT_DEBUG("�������߶�ȱ��ʱ����������\n");

        return;
    }

    len = read_pdata_xram("^Event", (char *)&bData, (PhaseAbnormalAddr+(mtr_id)*4+3), 1);
    if (len < 0)//-δ���뵽�ڴ���ڴ���-
    {
        EVENT_FMT_DEBUG("��ȡ�¼�״̬ʧ��");
        return;
    }

    if(bData == 0xFF)
        bData = 0;

    if((event_300F.meterRsw.metWord[6]&0x0001) == 1)
    {
        bType = 1;
    }
    else
    {
        bType = 0;
    }

    if (event_300F.nDelay > 0)
        lastTimesLmt = event_300F.nDelay;

    EVENT_FMT_DEBUG("��ȡ�¼�֮ǰ״̬%02X,��ǰ״̬:%02X,����ʱ����ֵ:%u\n", bData, bType, lastTimesLmt);

    if(bType)
    {
        if(!(bData & 1))//��δ����
        {
            len = read_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
            if(len < 0 || (lastTimes == 0xff))
                lastTimes = 0;
            printf("���� :lastTimes = %d %d\n",lastTimes,lastTimesLmt);
            if(lastTimes++ >= lastTimesLmt)
            {
                bData |= 1;
                ERFlg = 1;
                lastTimes = lastTimesLmt * 2;
                write_pdata_xram("^Event", (char *)&bData, (PhaseAbnormalAddr+(mtr_id)*4+3), 1);
            }

            write_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
        }
        else
        {
            len = read_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
            if(len < 0 || (lastTimes == 0xff))
                lastTimes = 0;
            if(lastTimes != lastTimesLmt * 2)
            {
                lastTimes = lastTimesLmt * 2;
                write_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
            }
        }
    }
    else
    {
        if(bData & 1)
        {
            len = read_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
            if(len < 0 || (lastTimes == 0xff))
                lastTimes = 2*lastTimesLmt;
            printf("�ָ�: lastTimes = %d %d\n",lastTimes,lastTimesLmt);
            if(lastTimes-- <= lastTimesLmt)
            {
                bData &= ~(1);
                ERFlg = 2;  //-�ָ�-
                lastTimes = 0;
                write_pdata_xram("^Event", (char *)&bData, (PhaseAbnormalAddr+(mtr_id)*4+3), 1);
            }
            write_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*3), 1);
        }
        else
        {
            len = read_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
            if(len < 0 || (lastTimes == 0xff))
                lastTimes = 0;
            if(lastTimes != 0)
            {
                lastTimes = 0;
                write_pdata_xram("^Event", (char *)&lastTimes, (PhaseRvsLastTimesAddr+(mtr_id)*2), 1);
            }
        }
    }

    if(ERFlg ==1)  //-����-
    {
        happenFlg = 1;
    }

    if(ERFlg==2)  //-�ָ�-
    {
        recoverFlg = 1;
    }

    if(happenFlg) //����
    {
        len = read_pdata_xram("^Event",(char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*(4)), 4);
        dwERFlgTmp |= PHASE_REVER;    //-���淢����ǵ��˲�����ȫ����������-
        write_pdata_xram("^Event",(char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);

        if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
        {
            SaveFileRecBfg(ERC_NO_11|0x80,0x80,mtr_id);//��¼��ѹ��������
        }
    }
    else if(recoverFlg) //�ָ�
    {
        len = read_pdata_xram("^Event",(char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);
        dwERFlgTmp &= ~PHASE_REVER;    //-���淢����ǵ��˲�����ȫ����������-
        write_pdata_xram("^Event",(char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);

        if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
        {
            SaveFileRecBfg(ERC_NO_11|0x80,0,mtr_id);//��¼��ѹ������ָ�
        }
    }
    else
        return;

#if 0
    int ret = 0;
    OOP_EVTSOURCE_T eventSource;    //�¼�����Դ
    OOP_EVTBUF_T    eventData;      //�¼���������
    //�¼�����ԴNULL
    eventSource.choice = DT_NULL;

    //�¼���������
    eventData.oadNum = 32;
    eventData.cols.nNum = 32;
//    eventData.oadNum = 16;
//    eventData.cols.nNum = 16;
    eventData.cols.oad[0].value = 0X00102201;
    eventData.cols.oad[1].value = 0X00202201;
    eventData.cols.oad[2].value = 0X00302201;
    eventData.cols.oad[3].value = 0X00402201;
    eventData.idLen[0] = sizeof(uint32);
    eventData.idLen[1] = sizeof(uint32);
    eventData.idLen[2] = sizeof(int);
    eventData.idLen[3] = sizeof(int);
    eventData.cols.oad[4].value = 0X00112201;
    eventData.cols.oad[5].value = 0X00212201;
    eventData.cols.oad[6].value = 0X00312201;
    eventData.cols.oad[7].value = 0X00412201;
    eventData.idLen[4] = sizeof(uint32);
    eventData.idLen[5] = sizeof(uint32);
    eventData.idLen[6] = sizeof(int);
    eventData.idLen[7] = sizeof(int);
    eventData.cols.oad[8].value = 0X00122201;
    eventData.cols.oad[9].value = 0X00222201;
    eventData.cols.oad[10].value = 0X00322201;
    eventData.cols.oad[11].value = 0X00422201;
    eventData.idLen[8] = sizeof(uint32);
    eventData.idLen[9] = sizeof(uint32);
    eventData.idLen[10] = sizeof(int);
    eventData.idLen[11] = sizeof(int);
    eventData.cols.oad[12].value = 0X00132201;
    eventData.cols.oad[13].value = 0X00232201;
    eventData.cols.oad[14].value = 0X00332201;
    eventData.cols.oad[15].value = 0X00432201;
    eventData.idLen[12] = sizeof(uint32);
    eventData.idLen[13] = sizeof(uint32);
    eventData.idLen[14] = sizeof(int);
    eventData.idLen[15] = sizeof(int);
    eventData.cols.oad[16].value = 0X00108201;
    eventData.cols.oad[17].value = 0X00208201;
    eventData.cols.oad[18].value = 0X00308201;
    eventData.cols.oad[19].value = 0X00408201;
    eventData.idLen[16] = sizeof(uint32);
    eventData.idLen[17] = sizeof(uint32);
    eventData.idLen[18] = sizeof(int);
    eventData.idLen[19] = sizeof(int);
    eventData.cols.oad[20].value = 0X00118201;
    eventData.cols.oad[21].value = 0X00218201;
    eventData.cols.oad[22].value = 0X00318201;
    eventData.cols.oad[23].value = 0X00418201;
    eventData.idLen[20] = sizeof(uint32);
    eventData.idLen[21] = sizeof(uint32);
    eventData.idLen[22] = sizeof(int);
    eventData.idLen[23] = sizeof(int);
    eventData.cols.oad[24].value = 0X00128201;
    eventData.cols.oad[25].value = 0X00228201;
    eventData.cols.oad[26].value = 0X00328201;
    eventData.cols.oad[27].value = 0X00428201;
    eventData.idLen[24] = sizeof(uint32);
    eventData.idLen[25] = sizeof(uint32);
    eventData.idLen[26] = sizeof(int);
    eventData.idLen[27] = sizeof(int);
    eventData.cols.oad[28].value = 0X00138201;
    eventData.cols.oad[29].value = 0X00238201;
    eventData.cols.oad[30].value = 0X00338201;
    eventData.cols.oad[31].value = 0X00438201;
    eventData.idLen[28] = sizeof(uint32);
    eventData.idLen[29] = sizeof(uint32);
    eventData.idLen[30] = sizeof(int);
    eventData.idLen[31] = sizeof(int);
    eventData.len = 4*32;

//    eventData.len = 4*16;
    if(happenFlg)
    {
        //д���ݿ�
        ret = WriteEventToDB(0, &event_300F.eventCommInfo, eventSource, eventData, 1);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_300F �ն������쳣�¼���¼ʧ�ܷ���%d\n", ret);
            return;
        }

        EVENT_FMT_TRACE("CheckERC_300F �ն������쳣�¼� ��¼�ɹ�, storagenum=%u !\n", event_300F.eventCommInfo.storagenum[0]);
    }
    else
    {
        //д���ݿ�
        ret = WriteEventToDB(0, &event_300F.eventCommInfo, eventSource, eventData, 2);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_300F �ն������쳣�ָ��¼� ��¼ʧ�ܷ���%d\n", ret);
            return;
        }

        EVENT_FMT_TRACE("CheckERC_300F �ն������쳣�ָ��¼� ��¼�ɹ�, storagenum=%u !\n", event_300F.eventCommInfo.storagenum[0]);
    }
#endif
    return;
}

/*****************************************************************************
�������ܵ�Ԫ��
������������ѹԽ�޸澯
����������������
�����������
��������ֵ����
�ڲ��������̣�������ʧѹ�涨ʱ���ڸ��ݵ�ǰ������ĵ�����ѹ����ֵ����ص����޲����жϵ�ѹ
                 �Ƿ�Խ���ޣ�Խ��������ø澯�������ɴ��������ɸ澯���ݲ���д�����ϱ�����
***************************************************************************-*/
void VltSupassEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType)
{
    uint16  vlt[3] = {0};    //-��ǰ3���ѹ-
    uint32  cur[3] = {0};    //-��ǰ3�����-
    uint16  vlt_lmt = 0;     //-��ѹ�쳣����ֵ-
    uint16  vlt_lmt1 = 0;    //-��ѹ�쳣����ֵ-
    uint32  ERFlg = 0;       //-�¼�������־-
    uint8   order = 0;       //-����-
    //uint8  bTemp = 0;
    uint8   is_now_flag = 0; //-��ѹԽ�ޱ�־-
    uint8   bData = 0;
    uint8   bphase = 0;      //-����-
    int16   len = 0;
    uint32  dwERFlgTmp = 0;
    int8    bfgPower=0;
    float32 tmpFloat = 0;

    static  uint8 lastTimes[4][3][MTR485_MAX_NUM];   //Խ�����ޡ������޵��ۼ�ʱ��
    uint8   lastTimesLmt = 1;                        //Ĭ�ϵĵ�ѹ�쳣���޳���ʱ��
    uint8   lastTimesLmt1 = 1;                       //Ĭ�ϵĵ�ѹ�쳣���޳���ʱ��
    uint16  vlt_lmt_rev = 0;                         //-��ѹ�쳣���޻ָ�ֵ-
    uint16  vlt_lmt1_rev = 0;                        //-��ѹ�쳣���޻ָ�ֵ-
    char    tmpBuf[10];
    OOP_EVTSOURCE_T eventSource; //�¼�����Դ
    OOP_EVTBUF_T    eventData;   //�¼���������

#if WRITE_LOG_EN
    uint16   i;
#endif
    //uint16    j;

    EVENT_FMT_DEBUG("ͣ���־bfgPower=%d\n",bfgPower);

    vlt[0] = event_global.vlt_a;
    vlt[1] = event_global.vlt_b;
    vlt[2] = event_global.vlt_c;
    cur[0] = Int_ABS(event_global.cur_a);
    cur[1] = Int_ABS(event_global.cur_b);
    cur[2] = Int_ABS(event_global.cur_c);

    if ((vlt[0] == APP_NONEWORD) && (vlt[1] == APP_NONEWORD) && (vlt[2] == APP_NONEWORD))
    {
        EVENT_FMT_DEBUG( "��ѹ��Чֱ�ӷ���\n");

        return;
    }

    //---ԭ�����벻����ֵ�ͷ��أ������޸�ΪĬ�ϲ��� 2500 ----return;---
    vlt_lmt = 2500;
    //--ԭ�����벻����ֵ�ͷ��أ������޸�ΪĬ�ϲ��� 1870 ----return;---
    vlt_lmt1 = 1870;

    if (mtr_id == 0 && para_query_float32(0x802a, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt = tmpFloat*10;
    }

    if (mtr_id == 0 && para_query_float32(0x8028, &tmpFloat) > 0 && tmpFloat > 0)
    {
        vlt_lmt1 = tmpFloat*10;
    }

    EVENT_FMT_DEBUG("������%d��ѹ����ֵ=%d, ����ֵ=%d\n",mtr_id,vlt_lmt,vlt_lmt1);

    memset(tmpBuf, 0x0, sizeof(tmpBuf));

    //-ԭ�����벻����ֵ�ͷ��أ������޸�ΪĬ�ϲ��� 10%---return;---
    tmpBuf[0] = 100;
    tmpBuf[1] = 0;
    vlt_lmt_rev = vlt_lmt*(1000-Int16_ABS((*(uint16 *)tmpBuf)))/1000;    //����ϵ���������ֵ���%������10��

    //-ԭ�����벻����ֵ�ͷ��أ������޸�ΪĬ�ϲ��� 10%---return;---
    tmpBuf[0] = 100;
    tmpBuf[1] = 0;
    vlt_lmt1_rev = vlt_lmt1*(1000+Int16_ABS((*(uint16 *)tmpBuf)))/1000;//����ϵ���������ֵ���%������10��,����+ϵ��

    lastTimesLmt = 1;           //�ݴ�
    lastTimesLmt1 = 1;          //�ݴ�

    EVENT_FMT_DEBUG("CheckERC24 ������%d��ѹ���޻ָ�ֵ=%d    ����ʱ��=%d\n",mtr_id,vlt_lmt_rev,lastTimesLmt);
    EVENT_FMT_DEBUG("CheckERC24 ������%d��ѹ���޻ָ�ֵ=%d    ����ʱ��=%d\n",mtr_id,vlt_lmt1_rev,lastTimesLmt1);

    if((bType == 1)||(bType == 2))
    {
        //-��ȡ��λ��¼��Ϣ-
        len = read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+7), 1);
    }
    else
    {
        //-��ȡ��λ��¼��Ϣ-
        len = read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+8), 1);
    }

    if (len < 0)
    {
        EVENT_FMT_DEBUG("��ȡ�¼�״̬ʧ��");
        return;
    }

    if (bData == 0xff) //�ļ�ϵͳ��ʼΪ0xffֵ
    {
        bData = 0;
    }

    EVENT_FMT_DEBUG("��ȡ�¼�״̬%02X\n", bData);

    if((cur[0] == 0x0)&&(vlt[0] == 0x0)&&(cur[1] == 0x0)&&(vlt[1] == 0x0)&&(cur[2] == 0x0)&&(vlt[2] == 0x0))
        return;//-��ѹ������Ϊ0����ͣ��-

    for(order = 0; order < 3; order++)
    {
        if((vlt[order] == 0xeeee)||(vlt[order] == 0xffff))
        {
            continue;
        }
        if((event_global.power_type == 2) || (event_global.power_type == 1 && order != 1))    //��������,����B��
        {
            switch (bType)
            {
            case 1:        //��ѹ��ѹ
                if(vlt[order] > vlt_lmt)        //���������Ӧ�ķ�������������Ӧ��ʱ������ۼ�
                {
                    if(lastTimes[bType-1][order][mtr_id] >= lastTimesLmt-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
					yx_calculate_sta_set(CALC_YX_VOLT_OVER_H, order, 1);
                    EVENT_FMT_DEBUG("��%d�������ѹ��ѹ�����������ѳ���ʱ��lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("��%d�಻�����ѹ��ѹ����������ʱ������\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }
                break;

            case 2:        //��ѹ��ѹ�ָ�
                if((vlt[order] <= vlt_lmt_rev) && (bData &(1<<order)))    //��ѹС�������޻ָ���ֵ ���� �Ѿ�������Խ������
                {
                    //���������Ӧ�ķ�������������Ӧ��ʱ������ۼ�
                    if(lastTimes[bType-1][order][mtr_id] >= lastTimesLmt-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
					yx_calculate_sta_set(CALC_YX_VOLT_OVER_H, order, 0);
                    EVENT_FMT_DEBUG("��%d�������ѹ��ѹ�ָ��������ѳ���ʱ��lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("��%d�಻�����ѹ��ѹ�ָ�������ʱ������\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }
                break;

            case 3:        //��ѹʧѹ
                if(vlt[order] < vlt_lmt1)        //���������Ӧ�ķ�������������Ӧ��ʱ������ۼ�
                {
                    if(lastTimes[bType-1][order][mtr_id] >= lastTimesLmt1-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
					yx_calculate_sta_set(CALC_YX_VOLT_DOWN_L, order, 1);
                    EVENT_FMT_DEBUG("��%d�������ѹʧѹ�����������ѳ���ʱ��lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("��%d�಻�����ѹʧѹ����������ʱ������\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }

                break;

            case 4:        //��ѹʧѹ�ָ�
                if((vlt[order] >= vlt_lmt1_rev) && (bData &(1<<order)))    //��ѹ�����������޻ָ���ֵ ���� �Ѿ�������Խ������
                {
                    //���������Ӧ�ķ�������������Ӧ��ʱ������ۼ�
                    if(lastTimes[bType-1][order][mtr_id] >= lastTimesLmt1-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
					yx_calculate_sta_set(CALC_YX_VOLT_DOWN_L, order, 0);
                    EVENT_FMT_DEBUG("��%d�������ѹʧѹ�ָ��������ѳ���ʱ��lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("��%d�಻�����ѹʧѹ�ָ�������ʱ������\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }

                break;

            default:
                continue;
            }
        }
        else
        {
            continue;
        }

        if((bType == 1)||(bType == 3))
        {
            if(is_now_flag)
            {
                switch(order)
                {
                case 0:
                    if(!(bData&A_PHASE))
                    {
                        bData |= A_PHASE;    //-A��-
                        bphase |= A_PHASE;    //-A��-
                        if(bType == 1)
                        {
                            ERFlg = SUPASS_VLT;    //-�����ޱ�־-
                        }
                        else
                        {
                            ERFlg = LOW_VLT;
                        }
                    }
                    break;
                case 1:

                    if(!(bData&B_PHASE))
                    {
                        bData |= B_PHASE;
                        bphase |= B_PHASE;
                        if(bType == 1)
                        {
                            ERFlg = SUPASS_VLT;    //-�����ޱ�־-
                        }
                        else
                        {
                            ERFlg = LOW_VLT;
                        }
                    }
                    break;
                case 2:
                    if(!(bData&C_PHASE))
                    {
                        bData |= C_PHASE;
                        bphase |= C_PHASE;
                        if(bType == 1)
                        {
                            ERFlg = SUPASS_VLT;    //-�����ޱ�־-
                        }
                        else
                        {
                            ERFlg = LOW_VLT;
                        }
                    }
                    break;
                default:
                    break;
                }
                if(bType == 1)
                {
                    if(write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+7), 1)<0)
                    {
                        return;
                    }
                }
                else
                {
                    if(write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+8), 1)<0)
                    {
                        return;
                    }
                }
            }
        }
        else
        {
            if(is_now_flag)
            {
                //-�ָ����г���ʱ��-
                switch(order)
                {
                case 0:
                    bData  &= ~A_PHASE;        //-����λ���-
                    bphase |= A_PHASE;
                    if(bType == 2)
                        ERFlg = SUPASS_VLT;    //-�����ޱ�־-
                    else
                        ERFlg = LOW_VLT;
                    break;
                case 1:
                    bData  &= ~B_PHASE;        //-����λ���-
                    bphase |= B_PHASE;
                    if(bType == 2)
                        ERFlg = SUPASS_VLT;    //-�����ޱ�־-
                    else
                        ERFlg = LOW_VLT;
                    break;
                case 2:
                    bData  &= ~C_PHASE;        //-����λ���-
                    bphase |= C_PHASE;
                    if(bType == 2)
                        ERFlg = SUPASS_VLT;    //-�����ޱ�־-
                    else
                        ERFlg = LOW_VLT;
                    break;
                default:
                    break;
                }
                if(bType == 2)
                {
                    if(write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+7), 1)<0)
                    {
                        return;
                    }
                }
                else
                {
                    if(write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+8), 1)<0)
                    {
                        return;
                    }
                }
            }
        }
    }

    if(ERFlg)//-�¼�����-
    {
        EVENT_COMM_INFO* pEventCommInfo;

        //�¼�����Դ
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter->basic.tsa;

        //�¼���������
        eventData.oadNum = 2;
        eventData.cols.nNum = 2;
        eventData.cols.oad[0].value = 0X20002200;
        eventData.cols.oad[1].value = 0X20012200;
        eventData.idLen[0] = sizeof(OOP_WORD3V_T);
        eventData.idLen[1] = sizeof(OOP_WORD3V_T);
        eventData.len = eventData.idLen[0]+eventData.idLen[1];

        if (bType == 3 || bType == 4)
        {
            pEventCommInfo = &event_3001.eventCommInfo;
            memcpy(eventData.buf, &event_3001.oopWord3V[mtr_id], eventData.idLen[0]);
            memcpy(eventData.buf+eventData.idLen[0], &event_3001.oopWord3C[mtr_id], eventData.idLen[1]);
        }
        else {
            pEventCommInfo = &event_3002.eventCommInfo;
            memcpy(eventData.buf, &event_3002.oopWord3V[mtr_id], eventData.idLen[0]);
            memcpy(eventData.buf+eventData.idLen[0], &event_3002.oopWord3C[mtr_id], eventData.idLen[1]);
        }

        if(bType==1 || bType==3)//����
        {
            //д���ݿ�
            int ret = WriteEventToDB(mtr_id, pEventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("������mtr_id=%hu  �����¼���¼CheckERC_3001ʧ�ܷ���%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("������mtr_id=%hu  �����¼���¼CheckERC_3001�ɹ� storagenum=%u \n", mtr_id, pEventCommInfo->storagenum[mtr_id]);
        }
        else
        {
            //�ָ��¼���¼����
            int ret = WriteEventToDB(mtr_id, pEventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("������mtr_id=%hu  �����������¼��ָ���¼CheckERC_3001ʧ�ܷ���%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("������mtr_id=%hu  �����������¼��ָ���¼CheckERC_3001�ɹ� , storagenum=%u !\n", mtr_id, pEventCommInfo->storagenum[mtr_id]);
        }

        if((bData == 0)&&((bType == 2)||(bType == 4)))
        {
            len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);
            dwERFlgTmp &= ~ERFlg;    //-���淢����ǵ��˲�����ȫ����������-
            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);

            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                if(bType == 2)
                    SaveFileRecBfg(ERC_NO_24|0x80,(bphase&0x07),mtr_id);//-Խ���޻ָ�-
                else
                    SaveFileRecBfg(ERC_NO_24,     (bphase&0x07),mtr_id);//-Խ���޻ָ�-
            }
        }
        else
        {
            len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);
            dwERFlgTmp |= ERFlg;    //-�øò�����ȫ���¼���־λ-
            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*4), 4);

            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                if(bType == 1)
                   SaveFileRecBfg(ERC_NO_24|0x80,(bphase&0x07)|0x80,mtr_id);//-Խ���޷���-
                else
                   SaveFileRecBfg(ERC_NO_24     ,(bphase&0x07)|0x80,mtr_id);//-Խ���޷���-
            }
        }
    }
}

/*******************************************************************************
* ������Ԫ��
* ��������������Խ�޸澯
* ����������������
* �����������
* �� �� ֵ����
* �ڲ����̣���Ĭ��ʱ���ڸ��ݵ�ǰ������ĵ�����ѹ����ֵ����ص����޲����жϵ����Ƿ�Խ���ޣ�
*              Խ��������ø澯�������ɴ��������ɸ澯���ݲ���д�����ϱ�����
*******************************************************************************/
void CurHighEvent(mtrnum_t  mtr_id, OOP_METER_T *oopMeter, uint8 bType)
{
    //uint16    i;
    uint16  vlt[3] = {0};                            //��ǰ��ѹ
    uint32  cur[3] = {0};                            //��ǰ����
    int     cur_lmt1[3]= {0},cur_lmt2[3]= {0};       //����ֵ
    uint32  ERFlg = 0;
    uint8   order;//,bTemp;
    //uint16  mtetmp = 0;
    uint8   is_now_flag = 0;
    uint8   bData=0,bphase = 0;
    int16   len;
    //uint32  dwERFlgTmp=0;
    static uint8 lastTimes[4][3][MTR485_MAX_NUM];    //Խ�����ޡ������޵��ۼ�ʱ��
    uint8   lastTimesLmt1 = 1;                       //Ĭ�ϵĵ���Խ���޳���ʱ��
    uint8   lastTimesLmt2 = 1;                       //Ĭ�ϵĵ���Խ�����޳���ʱ��
    uint16  cur_lmt1_rev = 0;                        //����Խ���޻ָ�ֵ
    uint16  cur_lmt2_rev = 0;                        //����Խ�����޻ָ�ֵ
    float32 tmpFloat;
    char    tmpBuf[10];

    vlt[0] = event_global.vlt_a;
    vlt[1] = event_global.vlt_b;
    vlt[2] = event_global.vlt_c;
    cur[0] = Int_ABS(event_global.cur_a);
    cur[1] = Int_ABS(event_global.cur_b);
    cur[2] = Int_ABS(event_global.cur_c);

    if ((event_global.cur_a == APP_NONELWORD) && (event_global.cur_b == APP_NONELWORD) && (event_global.cur_c == APP_NONELWORD))
    {
        EVENT_FMT_DEBUG( "������Чֱ�ӷ���\n");

        return;
    }

    //����������ֵ
    cur_lmt1[0] = (uint16)event_global.cur_gate * 120 / 100;
    cur_lmt1[0] &= 0x7fffffff;                        //ȥ������λ
    if (mtr_id == 0 && para_query_float32(0x802c, &tmpFloat) > 0 && tmpFloat > 0)
    {
        cur_lmt1[0] = tmpFloat*1000;
    }

    //������������ֵ
    cur_lmt2[0] = (uint16)event_global.cur_gate * 160 / 100;
    cur_lmt2[0] &= 0x7fffffff;                        //ȥ������λ
    if (mtr_id == 0 && para_query_float32(0x802e, &tmpFloat) > 0 && tmpFloat > 0)
    {
        cur_lmt2[0] = tmpFloat*1000;
    }

    EVENT_FMT_DEBUG("������%d����������ֵ=%ld ����ֵ=%ld \n", mtr_id, cur_lmt2[0], cur_lmt1[0]);

    //��ȡ��������ޣ���������ޣ��ķ���ϵ��
    tmpBuf[0] = 0;
    tmpBuf[1] = 0;

    //����ϵ���������ֵ���%������10��
    if(Int16_ABS((*(uint16 *)tmpBuf))>1000)
    {
        return;
    }
    else
    {
        cur_lmt1_rev = cur_lmt1[0]*(1000-Int16_ABS((*(uint16 *)tmpBuf)))/1000;
    }

    //��ȡ����������ޣ��������ޣ��ķ���ϵ��
    tmpBuf[0] = 0;
    tmpBuf[1] = 0;

    //����ϵ���������ֵ���%������10��
    if(Int16_ABS((*(uint16 *)tmpBuf))>1000)
    {
        return;
    }
    else
    {
        cur_lmt2_rev = cur_lmt2[0]*(1000-Int16_ABS((*(uint16 *)tmpBuf)))/1000;
    }

    //�������޵ĳ���ʱ��
    lastTimesLmt1 = 1;                        //�ݴ�

    EVENT_FMT_DEBUG("������%d�������ޣ���������ޣ��ָ�ֵ=%d ����ʱ��=%d\n",mtr_id,cur_lmt1_rev,lastTimesLmt1);

    //���������޵ĳ���ʱ��
    lastTimesLmt2 = 1;                        //�ݴ�

    EVENT_FMT_DEBUG("������%d���������ޣ��������ޣ��ָ�ֵ=%d ����ʱ��=%d\n",mtr_id,cur_lmt2_rev,lastTimesLmt2);

    //��ȡ��λ��Ϣ
    if((bType == 1) || (bType == 2))
    {
        len =  read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+9), 1);
    }
    else
    {
        len =  read_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+10), 1);
    }

    if (len < 0)
    {
        EVENT_FMT_DEBUG("��ȡ�¼�״̬ʧ��");
        return;
    }

    if (bData == 0xff) //�ļ�ϵͳ��ʼΪ0xffֵ
    {
        bData = 0;
    }

    EVENT_FMT_DEBUG("��ȡ�¼�״̬%02X\n", bData);

    //����δ��ͨ
    if ((cur[0] == 0x7fffffff) && (vlt[0] == 0xffff)
            && (cur[1] == 0x7fffffff) && (vlt[1] == 0xffff)
            && (cur[2] == 0x7fffffff) && (vlt[2] == 0xffff))
    {
        EVENT_FMT_DEBUG( "�����ɹ�ֱ�ӷ���\n");
        return;
    }

    //��ѹ������Ϊ0����ͣ��
    if ((cur[0] == 0x0) && (vlt[0] == 0x0)
        && (cur[1] == 0x0) && (vlt[1] == 0x0)
        && (cur[2] == 0x0) && (vlt[2] == 0x0))
    {
        EVENT_FMT_DEBUG( "ͣ��ֱ�ӷ���\n");
        return;
    }

    //ѭ����������Ƿ�����¼�����
    for (order = 0; order < 3; order++)
    {
        if ((cur[order] == 0x6eeeeeee) || (cur[order] == 0x7fffffff) || (cur[order] == APP_NONELWORD))
        {
            continue;
        }

        //��������,����B��
        if ((event_global.power_type == 2)
         || (event_global.power_type == 1 && order != 1))
        {
            switch (bType)
            {
            case 1:                                //����Խ����
                EVENT_FMT_DEBUG("������%d����Խ<���޷���>��� \n", mtr_id);
                EVENT_FMT_DEBUG(">cur_lmt1[0] = %ld    <cur_lmt2[0] = %ld\n", cur_lmt1[0], cur_lmt2[0]);
                if ((cur[order] > cur_lmt1[0]) && (cur[order] <= cur_lmt2[0]))
                {
                    //���������Ӧ�ķ�������������Ӧ��ʱ������ۼ�
                    if (lastTimes[bType-1][order][mtr_id] >= lastTimesLmt1-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
                    EVENT_FMT_DEBUG("��%d���������Խ���޷����������ѳ���ʱ��lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("��%d�಻�������Խ���޷���������ʱ������\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }
                break;

            case 2:                                //����Խ���޻ָ�
                EVENT_FMT_DEBUG("������%d����Խ<���޻ָ�>��� \n", mtr_id);
                EVENT_FMT_DEBUG("cur_lmt1_rev = %d\n", cur_lmt1_rev);
                //��ѹС�������޻ָ���ֵ ���� �Ѿ�������Խ������
                if((cur[order] <= cur_lmt1_rev) && (bData &(1<<order)))
                {
                    //���������Ӧ�ķ�������������Ӧ��ʱ������ۼ�
                    if (lastTimes[bType-1][order][mtr_id] >= lastTimesLmt1-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
                    EVENT_FMT_DEBUG("��%d���������Խ���޻ָ��������ѳ���ʱ��lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("��%d�಻�������Խ���޻ָ�������ʱ������\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }
                break;

            case 3:                                //����Խ������
                EVENT_FMT_DEBUG("������%d����Խ<�����޷���>��� \n", mtr_id);
                EVENT_FMT_DEBUG("cur_lmt2[0] = %ld\n", cur_lmt2[0]);
                //���������Ӧ�ķ�������������Ӧ��ʱ������ۼ�
                if (cur[order] > cur_lmt2[0])
                {
                    if (lastTimes[bType-1][order][mtr_id] >= lastTimesLmt2-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
                    EVENT_FMT_DEBUG("��%d���������Խ�����޷����������ѳ���ʱ��lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("��%d�಻�������Խ�����޷���������ʱ������\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }

                break;

            case 4:                                //����Խ�����޻ָ�
                EVENT_FMT_DEBUG("������%d����Խ<�����޻ָ�>��� \n", mtr_id);
                EVENT_FMT_DEBUG("cur_lmt2_rev = %d\n", cur_lmt2_rev);
                //��ѹ�����������޻ָ���ֵ ���� �Ѿ�������Խ������
                if((cur[order] <= cur_lmt2_rev) && (bData &(1<<order)))
                {
                    //���������Ӧ�ķ�������������Ӧ��ʱ������ۼ�
                    if (lastTimes[bType-1][order][mtr_id] >= lastTimesLmt2-1)
                    {
                        is_now_flag = 1;
                    }
                    else
                    {
                        lastTimes[bType-1][order][mtr_id]++;
                        is_now_flag = 0;
                    }
                    EVENT_FMT_DEBUG("��%d���������Խ�����޻ָ��������ѳ���ʱ��lastTimes[%d][%d][%d] = %d\n",order+1,bType-1,order,mtr_id,lastTimes[bType-1][order][mtr_id]);
                }
                else
                {
                    EVENT_FMT_DEBUG("��%d�಻�������Խ�����޻ָ�������ʱ������\n",order+1);
                    lastTimes[bType-1][order][mtr_id] = 0;
                    is_now_flag = 0;
                }
                break;

            default:
                continue;
            }
        }
        else
        {
            continue;
        }

        if ((bType == 1) || (bType == 3))       //�¼�����
        {
            if (is_now_flag)
            {
                switch(order)
                {
                case 0:
                    if (!( bData & A_PHASE))
                    {
                        bData |= A_PHASE;       //����λ��־
                        bphase |= A_PHASE;      //����λ��־
                        if (bType == 1)
                        {
                            ERFlg = HIGH_CUR;   //�õ���Խ�����¼���־
                        }
                        else
                        {
                            ERFlg = HIGHER_CUR; //�õ���Խ�������¼���־
                        }
                    }
                    break;

                case 1:
                    if (!( bData & B_PHASE))
                    {
                        bData |= B_PHASE;       //����λ��־
                        bphase |= B_PHASE;      //����λ��־
                        if (bType == 1)
                        {
                            ERFlg = HIGH_CUR;   //�õ���Խ�����¼���־
                        }
                        else
                        {
                            ERFlg = HIGHER_CUR; //�õ���Խ�������¼���־
                        }
                    }
                    break;

                case 2:
                    if (!( bData & C_PHASE))
                    {
                        bData |= C_PHASE;       //����λ��־
                        bphase |= C_PHASE;      //����λ��־
                        if (bType == 1)
                        {
                            ERFlg = HIGH_CUR;   //�õ���Խ�����¼���־
                        }
                        else
                        {
                            ERFlg = HIGHER_CUR; //�õ���Խ�������¼���־
                        }
                    }
                    break;

                default:
                    break;
                }
            }
			if (bType == 1)
            {
                yx_calculate_sta_set(CALC_YX_TR_OVERLOAD, 0, 1);   //�õ���Խ�����¼���־
            }
            else
            {
                yx_calculate_sta_set(CALC_YX_TR_HEAVYLOAD, 0, 1);   //�õ���Խ�������¼���־
            }
        }
        else                                    //�¼��ָ�
        {
            if ((is_now_flag) && ( bData &(1<<order)))
            {
                //�ָ����г���ʱ��
                switch(order)
                {
                case 0:
                    bData &= ~A_PHASE;          //�����λ��־
                    bphase |= A_PHASE;
                    if (bType == 2)
                    {
                        ERFlg = HIGH_CUR;       //�õ���Խ�����¼���־
                    }
                    else
                    {
                        ERFlg = HIGHER_CUR;     //�õ���Խ�������¼���־
                    }
                    break;

                case 1:
                    bData &= ~B_PHASE;          //�����λ��־
                    bphase |= B_PHASE;
                    if (bType == 2)
                    {
                        ERFlg = HIGH_CUR;       //�õ���Խ�����¼���־
                    }
                    else
                    {
                        ERFlg = HIGHER_CUR;     //�õ���Խ�������¼���־
                    }
                    break;

                case 2:
                    bData &= ~C_PHASE;          //�����λ��־
                    bphase |= C_PHASE;
                    if (bType == 2)
                    {
                        ERFlg = HIGH_CUR;      //�õ���Խ�����¼���־
                    }
                    else
                    {
                        ERFlg = HIGHER_CUR;    //�õ���Խ�������¼���־
                    }
                    break;

                default:
                    break;
                }
            }
			if (bType == 2)
            {
                yx_calculate_sta_set(CALC_YX_TR_OVERLOAD, 0, 0);   //�õ���Խ�����¼���־
            }
            else
            {
                yx_calculate_sta_set(CALC_YX_TR_HEAVYLOAD, 0, 0);   //�õ���Խ�������¼���־
            }
        }
    }

    if ((bType == 1)||(bType == 2))
    {
        if (write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+9), 1)<0)
        {
            return;
        }
    }
    else
    {
        if (write_pdata_xram("^Event", (char *)&bData, (EventPhaseRecAddr+(mtr_id)*(MAX_EVENT_NUM)+10), 1)<0)
        {
            return;
        }
    }

    if (ERFlg)                       //�¼�����
    {
        OOP_EVTSOURCE_T eventSource; //�¼�����Դ
        OOP_EVTBUF_T    eventData;   //�¼���������
        EVENT_COMM_INFO* pEventCommInfo;

        //TODO д�����ݿ�
        //�¼�����Դ
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter->basic.tsa;

        //�¼���������
        eventData.oadNum = 1;
        eventData.cols.nNum = 1;
        eventData.cols.oad[0].value = 0X20012200;
        eventData.idLen[0] = sizeof(OOP_WORD3V_T);
        eventData.len = eventData.idLen[0];

        pEventCommInfo = &event_3005.eventCommInfo;
        memcpy(eventData.buf, &event_3005.oopWord3C[mtr_id], eventData.idLen[0]);

        if(bType==1 || bType==3)//����
        {
            //д���ݿ�
            int ret = WriteEventToDB(mtr_id, pEventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("������mtr_id=%hu  �����¼���¼CheckERC_3005ʧ�ܷ���%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("������mtr_id=%hu  �����¼���¼CheckERC_3005�ɹ� storagenum=%u \n", mtr_id, pEventCommInfo->storagenum[mtr_id]);
        }
        else
        {
            //�ָ��¼���¼����
            int ret = WriteEventToDB(mtr_id, pEventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("������mtr_id=%hu  �����������¼��ָ���¼CheckERC_3005ʧ�ܷ���%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("������mtr_id=%hu  �����������¼��ָ���¼CheckERC_3005�ɹ� , storagenum=%u !\n", mtr_id, pEventCommInfo->storagenum[mtr_id]);
        }

        if ((bType == 1)||(bType == 3))
        {
            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                SaveFileRecBfg(ERC_NO_25|0x80,(bphase&0x07)|0x80,mtr_id);//��¼����Խ���޷���
            }
        }
        else
        {
            if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
            {
                SaveFileRecBfg(ERC_NO_25|0x80,(bphase&0x07),mtr_id);//��¼����Խ���޻ָ�
            }
        }
    }
}

/*****************************************************************************
�������ܵ�Ԫ��
������������ѹ������ƽ�⼰�ָ��澯
����������������
�����������
��������ֵ����
�ڲ��������̣��жϵ�ǰ������ĵ�ѹ������ƽ����Ƿ�Խ�޻�ָ���������ø澯������д�����ϱ�����
***************************************************************************-*/
void VltCurImbalanceEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType)
{
    int    max_cur;    //-������ƫ��-
    int    min_cur;    //-��С����ƫ��-
    int16  max_vlt;    //-����ѹƫ��-
    int16  min_vlt;    //-��С��ѹƫ��-
    uint16 vlt_para,cur_para;
    //uint32 ulTemp = 0;
    //int8  sign1 = 0,sign2 = 0;

    //int8   bTemp = 0;
    uint16 vlt[3];
    int    cur[3];
    int    dwERFlgTmp = 0;
    int16  len = 0;
    //uint16    j;

    static uint8 lastTimes[4][MTR485_MAX_NUM];    //�����ѹ��������ƽ�����ʱ�� 0 ��ѹ��ƽ�ⷢ�� 1 �ָ� 2 ������ƽ�ⷢ�� 3 �ָ�
    uint8  lastTimesLmt = 1;        //Ĭ�ϵ������ѹ��ƽ�����ʱ��
    uint8  lastTimesLmt1 = 1;       //Ĭ�ϵ����������ƽ�����ʱ��
    uint16 vlt_para_rev = 0;        //�����ѹ��ƽ��ָ�ֵ
    uint16 cur_para_rev = 0;        //���������ƽ��ָ�ֵ
    int8   vlt_status = 0, cur_status = 0; //��ѹ��������ƽ�ⷢ�����ָ���־  1 ���� 2 �ָ�
    int8   tmp_flag1 = 0, tmp_flag2 = 0;
    char   tmpBuf[10];

    //-ԭ�����벻����ֵ�ͷ��أ������޸�ΪĬ�ϲ��� 30% ----return;---
    vlt_para = 300;
    cur_para = 300;

    memset(tmpBuf, 0x0, sizeof(tmpBuf));
    //��ȡ����ʱ��ͷ���ϵ��
    //-ԭ�����벻����ֵ�ͷ��أ������޸�ΪĬ�ϲ��� 10% ----return;---
    tmpBuf[0] = 10;
    tmpBuf[1] = 0;
    vlt_para_rev = vlt_para*(1000-Int16_ABS(*(uint16 *)tmpBuf))/1000;    //����ϵ���������ֵ���%������10��

    //-ԭ�����벻����ֵ�ͷ��أ������޸�ΪĬ�ϲ��� 10% ----return;---
    tmpBuf[0] = 10;
    tmpBuf[1] = 0;
    cur_para_rev = cur_para*(1000-Int16_ABS(*(uint16 *)tmpBuf))/1000;    //����ϵ���������ֵ���%������10��

    EVENT_FMT_DEBUG("������%u��ѹ��ƽ����ֵ%d,�ָ���ֵ%d,������ƽ����ֵ%d,�ָ���ֵ%d\n", mtr_id, vlt_para, vlt_para_rev, cur_para, cur_para_rev);

    lastTimesLmt = 1;       //�ݴ�
    lastTimesLmt1 = 1;      //�ݴ�

    if((event_global.vlt_a == 0xeeee)||(event_global.vlt_b == 0xeeee)||(event_global.vlt_c == 0xeeee)
        ||(event_global.cur_a == 0xeeeeeeee)||(event_global.cur_b == 0xeeeeeeee)||(event_global.cur_c == 0xeeeeeeee)
        ||(event_global.vlt_a == 0xffff)||(event_global.vlt_b == 0xffff)||(event_global.vlt_c == 0xffff)
        ||(event_global.cur_a == 0xffffffff)||(event_global.cur_b == 0xffffffff)||(event_global.cur_c == 0xffffffff))
    {
        //-����һ���ѹ����Ϊ0-
        return;
    }

    len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*(4)), 4);
    if (len < 0)
    {
        EVENT_FMT_DEBUG("��ȡ�¼�״̬ʧ��");
        return;
    }

    if (dwERFlgTmp == -1) //�ļ�ϵͳ��ʼΪ0xffֵ
    {
        dwERFlgTmp = 0;
    }

    EVENT_FMT_DEBUG("��ȡ�¼�״̬%08X\n", dwERFlgTmp);

    //��ȡ����ѹ����ֵ�Լ���С��ѹ����ֵ
    if(event_global.power_type==2)    //-��������-
    {
        vlt[0] = event_global.vlt_a;
        vlt[1] = event_global.vlt_b;
        vlt[2] = event_global.vlt_c;
        cur[0] = event_global.cur_a;
        cur[1] = event_global.cur_b;
        cur[2] = event_global.cur_c;

        //����ֵ�����ţ����һ������
        if(cur[0]&0x80000000) cur[0] = (-1) * (cur[0]&0x7fffffff);
        if(cur[1]&0x80000000) cur[1] = (-1) * (cur[1]&0x7fffffff);
        if(cur[2]&0x80000000) cur[2] = (-1) * (cur[2]&0x7fffffff);

        CmpValuint16(3,vlt,(uint16*)&max_vlt,(uint16*)&min_vlt);
        CmpValint(3,cur,&max_cur,&min_cur);
    }
    else if(event_global.power_type==1)    //-��������-
    {
        vlt[0] = event_global.vlt_a;
        vlt[1] = event_global.vlt_c;
        cur[0] = event_global.cur_a;
        cur[1] = event_global.cur_c;

        //����ֵ�����ţ����һ������
        if(cur[0]&0x80000000) cur[0] = (-1) * (cur[0]&0x7fffffff);
        if(cur[1]&0x80000000) cur[1] = (-1) * (cur[1]&0x7fffffff);

        CmpValuint16(2,vlt,(uint16*)&max_vlt,(uint16*)&min_vlt);
        CmpValint(2,cur,&max_cur,&min_cur);
    }

    if (bType == 1)
    {
        if(max_vlt != 0)
        {
            //-��ѹ��ƽ���ж�-
            tmp_flag1 = ((uint32)(max_vlt-min_vlt)*1000/max_vlt) > vlt_para;
            tmp_flag2 = ((uint32)(max_vlt-min_vlt)*1000/max_vlt) > vlt_para_rev;
        }

        if(tmp_flag1 && !(dwERFlgTmp & VLT_NBLC))            //�����˲�ƽ�⣬������ǰû�м�¼�����¼�
        {
            //���������Ӧ�ķ�������������Ӧ��ʱ������ۼ�
            if(lastTimes[0][mtr_id] >= lastTimesLmt-1)
            {
                vlt_status = 1;
				yx_calculate_sta_set(CALC_YX_VOLT_IMBALANCE, 0, 1);
            }
            else
            {
                lastTimes[0][mtr_id]++;
                vlt_status = 0;
            }
        }
        else if(!tmp_flag2 && (dwERFlgTmp & VLT_NBLC))    //û�з�����ƽ�⣬������ǰ��¼�����¼�
        {
            //���������Ӧ�ķ�������������Ӧ��ʱ������ۼ�
            if(lastTimes[1][mtr_id] >= lastTimesLmt-1)
            {
                vlt_status = 2;
				yx_calculate_sta_set(CALC_YX_VOLT_IMBALANCE, 0, 0);
            }
            else
            {
                lastTimes[1][mtr_id]++;
                vlt_status = 0;
            }
        }
        else
        {
            lastTimes[0][mtr_id] = 0;
            lastTimes[1][mtr_id] = 0;
            vlt_status = 0;
        }
    }

    if (bType == 2)
    {
        //<start common problem>    //��ֹƽ������Ϊ0ʱ����ѹ��ƽ�ⷢ��ʱ���������ƽ�ⷢ��
        tmp_flag1 = 0;
        tmp_flag2 = 0;
        //<end common problem>
        if(max_cur != 0)
        {
            //-������ƽ���ж�-
            tmp_flag1 = ((uint32)(max_cur-min_cur)*1000/abs(max_cur)) > cur_para;
            tmp_flag2 = ((uint32)(max_cur-min_cur)*1000/abs(max_cur)) > cur_para_rev;
        }

        //0 ��ѹ��ƽ�ⷢ�� 1 �ָ� 2 ������ƽ�ⷢ�� 3 �ָ�
        if((tmp_flag1) && (!(dwERFlgTmp & CUR_NBLC)))        //�����˲�ƽ�⣬������ǰû�м�¼�����¼�
        {
            //���������Ӧ�ķ�������������Ӧ��ʱ������ۼ�
            if(lastTimes[2][mtr_id] >= lastTimesLmt1-1)
            {
                cur_status = 1;
				yx_calculate_sta_set(CALC_YX_CURR_IMBALANCE, 0, 1);
            }
            else
            {
                lastTimes[2][mtr_id]++;
                cur_status = 0;
            }
        }
        else if(!tmp_flag2 && (dwERFlgTmp & CUR_NBLC))    //û�з�����ƽ�⣬������ǰ��¼�����¼�
        {
            //���������Ӧ�ķ�������������Ӧ��ʱ������ۼ�
            if(lastTimes[3][mtr_id] >= lastTimesLmt1-1)
            {
                cur_status = 2;
				yx_calculate_sta_set(CALC_YX_CURR_IMBALANCE, 0, 0);
            }
            else
            {
                lastTimes[3][mtr_id]++;
                cur_status = 0;
            }
        }
        else
        {
            lastTimes[2][mtr_id] = 0;
            lastTimes[3][mtr_id] = 0;
            cur_status = 0;
        }
    }

    if (vlt_status != 0 || cur_status != 0)
    {
        //TODO �¼�д�����ݿ�
        OOP_EVTSOURCE_T eventSource; //�¼�����Դ
        OOP_EVTBUF_T    eventData;   //�¼���������
        EVENT_COMM_INFO* pEventCommInfo;

        //�¼�����Դ
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter->basic.tsa;

        //�¼���������
        eventData.oadNum = 1;
        eventData.cols.nNum = 1;
        eventData.idLen[0] = sizeof(OOP_WORD3V_T);
        eventData.len = eventData.idLen[0];

        if(bType==1) {
            eventData.cols.oad[0].value = 0X20002200;
            pEventCommInfo = &event_301D.eventCommInfo;
            memcpy(eventData.buf, &event_301D.oopWord3V[mtr_id], eventData.idLen[0]);
        }
        else {
            eventData.cols.oad[0].value = 0X20012200;
            pEventCommInfo = &event_301E.eventCommInfo;
            memcpy(eventData.buf, &event_301E.oopWord3C[mtr_id], eventData.idLen[0]);
        }

        //-�¼������ˣ���û�м�¼-
        if((vlt_status == 1) || (cur_status == 1))
        {
            //д���ݿ�
            int ret = WriteEventToDB(mtr_id, pEventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("������mtr_id=%hu  �����¼���¼CheckERC_301Dʧ�ܷ���%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("������mtr_id=%hu  �����¼���¼CheckERC_301D�ɹ� storagenum=%u \n", mtr_id, pEventCommInfo->storagenum[mtr_id]);

            if(vlt_status == 1)
            {
                dwERFlgTmp |= VLT_NBLC;        //-��ѹ�¼���־-

                if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
                {
                    SaveFileRecBfg(ERC_NO_17|0x80,0x80,mtr_id);//-��ѹ��ƽ��-
                }
            }
            if(cur_status == 1)
            {
                dwERFlgTmp |= CUR_NBLC;        //-�����¼���־-

                if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
                {
                    SaveFileRecBfg(ERC_NO_17,0x80,mtr_id);//-������ƽ��-
                }
            }

            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*(4)), 4);
        }

        //-�¼�û�з����������м�¼����ָ�-
        if((vlt_status == 2) || (cur_status == 2))
        {
            //�ָ��¼���¼����
            int ret = WriteEventToDB(mtr_id, pEventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("������mtr_id=%hu  �����������¼��ָ���¼CheckERC_301Dʧ�ܷ���%d\n", mtr_id, ret);
                return;
            }

            EVENT_FMT_TRACE("������mtr_id=%hu  �����������¼��ָ���¼CheckERC_301D�ɹ� , storagenum=%u !\n", mtr_id, pEventCommInfo->storagenum[mtr_id]);

            if(vlt_status == 2)
            {
                dwERFlgTmp &= ~VLT_NBLC;//-������¼���־-

                if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
                {
                    SaveFileRecBfg(ERC_NO_17|0x80,0,mtr_id);//-��ѹ��ƽ��-
                }
            }

            if(cur_status == 2)
            {
                dwERFlgTmp &= ~CUR_NBLC;//-������¼���־-

                if(mtr_id <= DA_MTR_EVENT_MAX_NUM)
                {
                    SaveFileRecBfg(ERC_NO_17,0,mtr_id);//-������ƽ��-
                }
            }

            write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id)*(4)), 4);
        }
    }
}


