/*-
****************************************************************************************
 * Copyright (c) 2004,�Ͼ��������������������ι�˾
 *            All rights reserved.
 * �������ƣ�Security.c
 * �汾�ţ�1.0
 * ���ܣ��������Э��ջ-Ӧ�ò�-��ȫ����ӿ�
 * �����ˣ�qxc
 * ����ʱ�䣺2016-06-30
 * �޸���:
 * �޸�ʱ��:
 * �޸ļ�Ҫ˵��:
 * ������ 
*****************************************************************************************
-*/
#include "CtsAll.h"

#define SAFE_MODE   1       //1���� 0�����ð�ȫģʽ

uint8 s_randPara[16]={0};       //���������

extern int8 ReadMtrGetRand(uint8 *pOutRand);
/****************************************************************************
*ģ���ţ�                                                                  
*���ƣ�SecurityRequest                                                                      
*���ܣ���ȫ��������                                                                      
*���������                                                                  
*���������                                                                  
*����                                                                      
*���أ���                                                                    
****************************************************************************/
void SecurityRequest(uint8 pipe,uint8 *inData,uint16 inLen)
{
    AppLayerList_t* pAppLayer;
    pAppLayer = FindAppLayer(pipe,NULL);
    uint8 recvbuf[8192]={0};
    uint16 recvlen=0;
    
    if(inLen+30>sizeof(pAppLayer->sendApduList->apdu))
        return;
    //ֻ֧������+�������ʽ��ȡ

    recvbuf[recvlen++] = SERVERID_SECURITYREQUEST;
    recvbuf[recvlen++] =0;

    if(inLen>=0x80)
    {
        recvbuf[recvlen++] = 0x82;
        recvbuf[recvlen++] = (uint8)(inLen>>8);
        recvbuf[recvlen++] = (uint8)(inLen);
    }
    else
        recvbuf[recvlen++] = inLen;

    memcpy(recvbuf+recvlen,inData,inLen);//�����������ݵ�Ԫ
    recvlen += inLen;
    recvbuf[recvlen++] = 1;//�����
    recvbuf[recvlen++] = 16;
    memcpy(recvbuf+recvlen,s_randPara,16);
    recvlen+=16;

    
    memcpy(pAppLayer->sendApduList->apdu,recvbuf,recvlen);
    pAppLayer->sendApduList->apduLen = recvlen;

}
/****************************************************************************
*ģ���ţ�                                                                  
*���ƣ�SecurityConfirm                                                                      
*���ܣ���ȫ����ȷ��                                                                      
*���������                                                                  
*���������                                                                  
*����                                                                      
*���أ���                                                                    
****************************************************************************/
eOopCommRslt_t SecurityConfirm(AppLayerList_t *appLayerList)
{
    //printf("App Layer Service:SecurityConfirm\n");
    if(appLayerList->recvApduList->apduLen <= 3)
        return eOopComm_FrameNotIntergrity;

    uint16 offSet = 1;
    if(appLayerList->recvApduList->apdu[offSet++] != 0x00)
        return eOopComm_SecurityNotMatch;
    uint16 PlainDataLen = appLayerList->recvApduList->apdu[offSet++];
    if(PlainDataLen==0x82)
    {
        PlainDataLen = appLayerList->recvApduList->apdu[offSet++];
        PlainDataLen <<= 8;
        PlainDataLen += appLayerList->recvApduList->apdu[offSet++];
    }
    else if(PlainDataLen==0x81)
    {
        PlainDataLen = appLayerList->recvApduList->apdu[offSet++];
    }
    else if(PlainDataLen>0x82 || PlainDataLen == 0x80)
        return eOopComm_SecurityNotMatch;
    uint8 *pPlainData = appLayerList->recvApduList->apdu+offSet;
    offSet += PlainDataLen;
    if(appLayerList->recvApduList->apdu[offSet++] != 0x01    //����MAC
        || appLayerList->recvApduList->apdu[offSet++] != 0x00)//����MAC
        return eOopComm_SecurityNotMatch;
    uint8 MACLen =  appLayerList->recvApduList->apdu[offSet++];
    
    if(MACLen !=4)
        return eOopComm_SecurityNotMatch;

    if(gSafeButton==1)
    {
        if(gSafeButtonHPLC == 0 &&(appLayerList->pipe>=oop_pipe_get(0,0)||appLayerList->pipe<PLC_NUM) )
        {

        }else
        {

            uint8 *pMAC = appLayerList->recvApduList->apdu+offSet;

            if(ESAM_OK != Verify_ReadMtrMAC(appLayerList->pipe,appLayerList->secMod,pPlainData,PlainDataLen,pMAC,&PlainDataLen))
            {
                return eOopComm_SecurityNotMatch;

            }


        }
    }

    memmove(appLayerList->recvApduList->apdu,pPlainData,PlainDataLen);
    
    appLayerList->recvApduList->apduLen = PlainDataLen;
    
    return eOopComm_Success;
}

void security_rand_get()
{
    uint8 buf[1000]={0};
    //uint16 len;
    int ret;
    ret = ReadMtrGetRand(buf);
    if(ret!=ESAM_OK)
    {
        TASK_FMT_DEBUG( -1, REC_LOG, "security_rand_get error\n");
        return;
    }
    memcpy(s_randPara,buf,16); //�����Ĭ��16���ֽ�
    TASK_FMT_TRACE(-1,REC_LOG,"��ȡ�������\n");
    TASK_BUF_TRACE(-1,REC_LOG, s_randPara, 16);
}


/*******************************************************************************
* ��������: security_rand1_get
* ��������:��ȡ�����1 
* �������:
* �������: �����1
* �� �� ֵ:��������� 0��ʾ��esam����ʧ��
*******************************************************************************/
int security_rand1_get(uint8* buf)
{
    uint8 sendbuf[] = {0X55,0x80, 0x04, 0x00,0x08,0x00,0x00};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    ret = TESAM_RcvSndDataAndCheck(sendbuf,sizeof(sendbuf),recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        return 0;
    }
    memcpy(buf,recvbuf,recvlen);
    return recvlen;
}

/*******************************************************************************
* ��������: security_sessondata1_get
* ��������:��ȡsessondata1 ��mac1
* �������:
* �������: 
* �� �� ֵ:���� 0��ʾ��esam����ʧ��
*******************************************************************************/
int security_sessondata1_get(uint8* inbuf,uint16 inlen,uint32 counter,uint8* outbuf)
{
    uint16 index = 0;
    uint16 len;
    uint32 realcounter = counter+1;
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0x72;
    sendbuf[index++] = 0x00;
    sendbuf[index++] = 0x00;
    len = inlen+4+1;    //��Կ������+������4�ֽ�+һ���ֽ�FLG
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    memcpy(&sendbuf[index],inbuf,inlen);
    index+=inlen;
    memcpy_r(&sendbuf[index],&realcounter,4);
    index+=4;
    sendbuf[index++] = 0x00;
    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        return 0;
    }
    memcpy(outbuf,recvbuf,recvlen);
    return recvlen;

}

/*******************************************************************************
* ��������: security_sessonkey_get
* ��������:��ȡ�Ự��Կ�� 80740000
* �������:
* �������: 
* �� �� ֵ:���� 0��ʾ��esam����ʧ��
*******************************************************************************/
int security_sessonkey_get(uint8* key,uint16 keylen,uint8* sessiondata2,uint16 sessiondata2len,
        uint8* mac2,uint16 mac2len,uint8* outbuf)
{
    uint16 index = 0;
    uint16 len;
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0x74;
    sendbuf[index++] = 0x00;
    sendbuf[index++] = 0x00;
    len = keylen+sessiondata2len+mac2len;
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    memcpy(&sendbuf[index],key,keylen);
    index+=keylen;
    memcpy(&sendbuf[index],sessiondata2,sessiondata2len);
    index+=sessiondata2len;
    memcpy(&sendbuf[index],mac2,mac2len);
    index+=mac2len;

    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        return 0;
    }
    memcpy(outbuf,recvbuf,recvlen);
    return recvlen;

}

/*******************************************************************************
* ��������: security_taskdata_get
* ��������:��ȡ������������ 80760301 Уʱ
* �������:
* �������: 
* �� �� ֵ:���� 0��ʾ��esam����ʧ��
*******************************************************************************/
int security_taskdata_get(uint8* key,uint16 keylen,uint8* taskdata,uint16 taskdatalen,uint8* outbuf)
{
    uint16 index = 0;
    uint16 len;
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    time_t timenow = time(NULL);
    DateTimeHex_t timehex;
    DT_Time2DateTimeHex(timenow,&timehex);
    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0x76;
    sendbuf[index++] = 0x03;
    sendbuf[index++] = 0x01;
    len = 1+keylen+2+7+taskdatalen;
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    sendbuf[index++]=keylen/32;
    memcpy(&sendbuf[index],key,keylen);
    index+=keylen;
    sendbuf[index++] = 00;
    sendbuf[index++] = 7; //Уʱʱ�䳤�� 
    memcpy(&sendbuf[index],&timehex,7);
    index+=7;
    memcpy(&sendbuf[index],taskdata,taskdatalen);
    index+=taskdatalen;

    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        return 0;
    }
    memcpy(outbuf,recvbuf,recvlen);
    return recvlen;

}

void security_request_encrypt(uint8 pipe,OOP_TSA_T*sa,uint8* taskdata,uint16 tasklen,uint8* sid,uint8* appenddata,uint8* mac )
{
    uint16 apduLen;
    uint8 *apdu;
    LinkLayerAddr_t a;
    LinkLayerCtrl_t ctrl;
    AppLayerList_t* pAppLayer;
    pAppLayer = FindAppLayer(pipe,NULL);
    apdu = pAppLayer->sendApduList->apdu;
    //a.af_len = sa->len;
    //memcpy_r(a.sa,sa->add,a.af_len+1);
    a.af_len = sa->len;
    //�߼���ַΪ1ʱҲҪ���ȥ
    if(sa->oldvxd == 1)
    {
        a.af_logicAddr = 1;
    }

    //�ж���չ�߼���ַ
    if(sa->flag == 1)
    {
        //a.af_len++;
        a.af_logicAddr = 2;
        a.sa[0] = sa->vxd;
        memcpy_r(&a.sa[1],sa->add,a.af_len);
    }else
        memcpy_r(a.sa,sa->add,a.af_len+1);
    ctrl.ctrl = 0;
    ctrl.ctrl_fun = CTRLFUN_DATAEXCHANGE;
    ctrl.ctrl_prm = 1;
   
    apduLen = 0;
    apdu[apduLen++] = SERVERID_SECURITYREQUEST;
    apdu[apduLen++] =1; //����
    apdu[apduLen++] = tasklen;   
    memcpy(apdu+apduLen,taskdata,tasklen);
    apduLen+=tasklen;
    apdu[apduLen++] =0; //������֤�� SID_MAC
    memcpy(apdu+apduLen,sid,4);
    apduLen+=4;
    apdu[apduLen++] = 8;
    memcpy(apdu+apduLen,appenddata,8);
    apduLen+=8;
    apdu[apduLen++] = 4;
    memcpy(apdu+apduLen,mac,4);
    apduLen+=4;

    pAppLayer->sendApduList->apduLen = apduLen;

    Client_Send_Apdu(pipe,&a,ctrl.ctrl, pAppLayer->sendApduList->apdu, pAppLayer->sendApduList->apduLen);
}
//698��Ҫmac
void security_request_encrypt_698(uint8 pipe,OOP_TSA_T*sa,uint8* taskdata,uint16 tasklen,uint8* sid,uint8* appenddata )
{
    uint16 apduLen;
    uint8 *apdu;
    LinkLayerAddr_t a;
    LinkLayerCtrl_t ctrl;
    AppLayerList_t* pAppLayer;
    pAppLayer = FindAppLayer(pipe,NULL);
    apdu = pAppLayer->sendApduList->apdu;
    a.af_len = sa->len;
    memcpy_r(a.sa,sa->add,a.af_len+1);
    ctrl.ctrl = 0;
    ctrl.ctrl_fun = CTRLFUN_DATAEXCHANGE;
    ctrl.ctrl_prm = 1;
   
    apduLen = 0;
    apdu[apduLen++] = SERVERID_SECURITYREQUEST;
    apdu[apduLen++] =1; //����
    apdu[apduLen++] = tasklen;   
    memcpy(apdu+apduLen,taskdata,tasklen);
    apduLen+=tasklen;
    apdu[apduLen++] =3; //������֤�� SID
    memcpy(apdu+apduLen,sid,4);
    apduLen+=4;
    apdu[apduLen++] = 2;
    memcpy(apdu+apduLen,appenddata,2);
    apduLen+=2;


    pAppLayer->sendApduList->apduLen = apduLen;

    Client_Send_Apdu(pipe,&a,ctrl.ctrl, pAppLayer->sendApduList->apdu, pAppLayer->sendApduList->apduLen);
}


/*******************************************************************************
* ��������: security_decrypt_sec_mac
* ��������:���� 80780000 ���ļ�mac
* �������:
* �������: 
* �� �� ֵ:���� 0��ʾ��esam����ʧ��
*******************************************************************************/
int security_decrypt_sec_mac(uint8* key,uint16 keylen,uint8* secdata,uint16 seclen,uint8 *mac,uint8* outbuf)
{
    uint16 index = 0;
    uint16 len;
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0x78;
    sendbuf[index++] = 0x00;
    sendbuf[index++] = 0x00;
    len = 1+keylen+4+2+seclen+4;
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    sendbuf[index++]=keylen/32;
    memcpy(&sendbuf[index],key,keylen);
    index+=keylen;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0x24;
    sendbuf[index++] = 0x13;
    sendbuf[index++] = 0x00;
    len = seclen+4;
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    memcpy(&sendbuf[index],secdata,seclen);
    index+=seclen;
    memcpy(&sendbuf[index],mac,4);
    index+=4;
    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        return 0;
    }
    memcpy(outbuf,recvbuf,recvlen);
    return recvlen;

}

/*******************************************************************************
* ��������: security_endata2_get
* ��������:��ȡendata2 80D40300
* �������:
* �������: 
* �� �� ֵ:���� 0��ʾ��esam����ʧ��
*******************************************************************************/
int security_endata2_get(uint8* key,uint16 keylen,uint8* secdata,uint16 seclen,uint8* outbuf)
{
    uint16 index = 0;
    uint16 len;
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0xD4;
    sendbuf[index++] = 0x03;
    sendbuf[index++] = 0x00;
    len = 1+keylen+seclen;
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    sendbuf[index++]=keylen/32;
    memcpy(&sendbuf[index],key,keylen);
    index+=keylen;

    memcpy(&sendbuf[index],secdata,seclen);
    index+=seclen;

    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        return 0;
    }
    memcpy(outbuf,recvbuf,recvlen);
    return recvlen;

}

/*******************************************************************************
* ��������: security_esamset_data_get
* ��������:��ȡesam������������ 80760301
* �������:
* �������: 
* �� �� ֵ:���� 0��ʾ��esam����ʧ��
*******************************************************************************/
int security_esamset_data_get(uint8* key,uint16 keylen,uint8* endata2,uint16 endata2len,uint8* secdata,uint16 seclen,uint8* outbuf)
{
    uint16 index = 0;
    uint16 len;
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0x76;
    sendbuf[index++] = 0x03;
    sendbuf[index++] = 0x01;
    len = 1+keylen+2+endata2len+seclen;
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    sendbuf[index++]=keylen/32;
    memcpy(&sendbuf[index],key,keylen);
    index+=keylen;
    len = endata2len;
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    memcpy(&sendbuf[index],endata2,endata2len);
    index+=endata2len;
    memcpy(&sendbuf[index],secdata,seclen);
    index+=seclen;

    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        return 0;
    }
    memcpy(outbuf,recvbuf,recvlen);
    return recvlen;

}

/*******************************************************************************
* ��������: security_sessonkey_get_698
* ��������:��ȡ�Ự��Կ�� 80740100
* �������:
* �������: 
* �� �� ֵ:���� 0��ʾ��esam����ʧ��
*******************************************************************************/
int security_sessonkey_get_698(uint8* key,uint16 keylen,uint8* sessiondata2,uint16 sessiondata2len,
        uint8* mac2,uint16 mac2len,uint8* outbuf)
{
    uint16 index = 0;
    uint16 len;
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0x74;
    sendbuf[index++] = 0x01;
    sendbuf[index++] = 0x00;
    len = keylen+sessiondata2len+mac2len;
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    memcpy(&sendbuf[index],key,keylen);
    index+=keylen;
    memcpy(&sendbuf[index],sessiondata2,sessiondata2len);
    index+=sessiondata2len;
    memcpy(&sendbuf[index],mac2,mac2len);
    index+=mac2len;

    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        return 0;
    }
    memcpy(outbuf,recvbuf,recvlen);
    return recvlen;

}

/*******************************************************************************
* ��������: security_decrypt_sec_mac
* ��������:���� 80780000 ���ļ�mac
* �������:
* �������: 
* �� �� ֵ:���� 0��ʾ��esam����ʧ��
*******************************************************************************/
int security_decrypt_sec_mac_698(uint8* key,uint16 keylen,uint8* secdata,uint16 seclen,uint8 *mac,uint8* outbuf)
{
    uint16 index = 0;
    uint16 len;
    uint8 mactmp[4]={0};
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0x78;
    sendbuf[index++] = 0x00;
    sendbuf[index++] = 0x00;
    if(memcmp(mac,mactmp,4) == 0)
        len = 1+keylen+4+2+seclen;
    else
        len = 1+keylen+4+2+seclen+4;
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    sendbuf[index++]=keylen/32;
    memcpy(&sendbuf[index],key,keylen);
    index+=keylen;
    sendbuf[index++] = 0x81;
    sendbuf[index++] = 0x1C;
    sendbuf[index++] = 0x00;
    //���û��mac ����A6
    if(memcmp(mac,mactmp,4) == 0)
        sendbuf[index++] = 0xA6;
    else
        sendbuf[index++] = 0xA7;
    len = seclen+4;
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    memcpy(&sendbuf[index],secdata,seclen);
    index+=seclen;
    if(memcmp(mac,mactmp,4) != 0)
    {
        memcpy(&sendbuf[index],mac,4);
        index+=4;
    }
    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        return 0;
    }
    memcpy(outbuf,recvbuf,recvlen);
    return recvlen;

}


/*******************************************************************************
* ��������: security_ER0_get
* ��������:��ȡer0 84540001 645��
* �������:
* �������: 
* �� �� ֵ:���� 0��ʾ��esam����ʧ��
*******************************************************************************/
int security_ER0_get(uint8* key,uint16 keylen,uint8* meterno,uint8* R0,uint8* outbuf)
{
    uint16 index = 0;
    uint16 len;
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x84;
    sendbuf[index++] = 0x54;
    sendbuf[index++] = 0x00;
    sendbuf[index++] = 0x01;
    len = keylen+8+8;   //��ɢ����8�ֽ� R08�ֽ�

    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    memcpy(&sendbuf[index],key,keylen);
    index+=keylen;
    memcpy(&sendbuf[index],meterno,8);
    index+=8;
    memcpy(&sendbuf[index],R0,8);
    index+=8;

    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        return 0;
    }
    memcpy(outbuf,recvbuf,recvlen);
    return recvlen;

}

/*******************************************************************************
* ��������: security_secdata_get_645
* ��������:��ȡ�������� 84560404 645��
* �������:
* �������: 
* �� �� ֵ:���� 0��ʾ��esam����ʧ��
*******************************************************************************/
int security_secdata_get_645(uint8* secdata,uint16 seclen,uint8* key,uint16 keylen,uint8* R2,uint8* outbuf)
{
    uint16 index = 0;
    uint16 len;
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    time_t timenow = time(NULL);
    rtc_t prtc;
    memset(&prtc,0x00,sizeof(rtc_t));
    DT_Time2RTC(timenow,&prtc);
    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x84;
    sendbuf[index++] = 0x56;
    sendbuf[index++] = 0x04;
    sendbuf[index++] = 0x04;
    len = 6+8+seclen+4+keylen+4;   
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    sendbuf[index++] = 0x01;
    sendbuf[index++] = 0x05;
    sendbuf[index++] = 0x00;
    sendbuf[index++] = 0x27;
    sendbuf[index++] = 0x00;
    sendbuf[index++] = 0x05;
    //ʱ��
    sendbuf[index++] = 0x07;
    sendbuf[index++] = prtc.yy;
    sendbuf[index++] = prtc.mm;
    sendbuf[index++] = prtc.dd;
    sendbuf[index++] = prtc.ww;
    sendbuf[index++] = prtc.hh;
    sendbuf[index++] = prtc.nn;
    sendbuf[index++] = prtc.ss;
    memcpy(&sendbuf[index],secdata,seclen);
    index+=seclen;
    sendbuf[index++] = 0x04;
    sendbuf[index++] = 0x00;
    sendbuf[index++] = 0x01;
    sendbuf[index++] = 0x0C;
    memcpy(&sendbuf[index],key,keylen);
    index+=keylen;
    memcpy_r(&sendbuf[index],R2,4);
    index+=4;


    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        return 0;
    }
    memcpy(outbuf,recvbuf,recvlen);
    return recvlen;

}

/*******************************************************************************
* ��������: security_report_response_mac_get
* ��������:��ȡ�¼��ϱ���ظ���mac 
���ͣ�800E4807+LC+���ܱ�+ Data2+00000000+Data3
���أ�9000+0004+MAC
* �������:
* �������: 
* �� �� ֵ:
*******************************************************************************/
int security_report_response_mac_get(uint8* meterNo,uint8* data2,uint16 data2len,uint8* data3,uint16 data3len,uint8* mac)
{
    uint16 index = 0;
    uint16 len;
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0x0E;
    sendbuf[index++] = 0x48;
    sendbuf[index++] = 0x07;
    len = 8+data2len+4+data3len;   
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    memcpy(&sendbuf[index],meterNo,8);
    index+=8;
    memcpy(&sendbuf[index],data2,data2len);
    index+=data2len;
    sendbuf[index++] = 0x00;
    sendbuf[index++] = 0x00;
    sendbuf[index++] = 0x00;
    sendbuf[index++] = 0x00;
    memcpy(&sendbuf[index],data3,data3len);
    index+=data3len;

    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK || recvlen != 4)
    {
        return 0;
    }
    memcpy(mac,recvbuf,recvlen);
    return recvlen;

}


/****************************************************************************
*ģ���ţ�                                                                  
*���ƣ�security_request_unpack                                                               
*���ܣ���������ϱ�������                                                                     
*���������                                                                  
*���������                                                                  
*����                                                                      
*���أ���                                                                    
****************************************************************************/
eOopCommRslt_t security_request_unpack(AppLayerList_t *appLayerList)
{
    uint8 *apdu = appLayerList->recvApduList->apdu;
    uint16 index = 1;
    uint16 PlainDataLen;
    uint8 *PlainData = NULL;
    uint8 lenoffset = 0;
    uint8 rnbuf[100] = {0};
    uint16 rnlen;
    uint8 mac[4]={0};
    uint8 maclen;
    
    //ֻ֧������
    if(apdu[index++] != 0 )
    {
        return eOopComm_SecurityNotMatch;
    }
    PlainDataLen = get_len_offset(&apdu[index],&lenoffset);
    index+=lenoffset;
    PlainData = &apdu[index];
    index+=PlainDataLen;

    //�����������mac
    if(apdu[index++] == 2)
    {
        //�����
        rnlen = get_len_offset(&apdu[index],&lenoffset);
        index+=lenoffset;
        memcpy(rnbuf,&apdu[index],rnlen);
        if(rnlen!=12)
            return eOopComm_SecurityNotMatch;
        index+=rnlen;
        //MAC
        maclen = apdu[index++];
        if(maclen!=4)
        {
            return eOopComm_SecurityNotMatch;
        }
        memcpy(mac,&apdu[index],4);
        index+=4;
        //У���������mac ��ȡ�����mac
       // security_rn_mac_verify(rnbuf,rnlen,mac,outmac);
        appLayerList->macflag = 1;
        memcpy(appLayerList->rnbuf,rnbuf,12);
    }
    
    memmove(appLayerList->recvApduList->apdu,PlainData,PlainDataLen);
    appLayerList->recvApduList->apduLen = PlainDataLen;
    
    return eOopComm_Success;
}


/****************************************************************************
*ģ���ţ�                                                                  
*���ƣ�security_responst                                                                      
*���ܣ���ȫ������Ӧ                                                                     
*���������                                                                  
*���������                                                                  
*����                                                                      
*���أ���                                                                    
****************************************************************************/
void security_response(uint8 pipe,uint8 *inData,uint16 inLen)
{
    AppLayerList_t* pAppLayer;
    pAppLayer = FindAppLayer(pipe,NULL);
    uint8 sendbuf[8192]={0};
    uint16 index=0;
    uint8 mac[4]={0};
    uint8 metno[8]={0};
    memcpy(&metno[2],pAppLayer->sa.add,6);
    
    sendbuf[index++] = SERVERID_SECURITYRESPONSE;
    sendbuf[index++] =0;

    if(inLen>=0x80)
    {
        sendbuf[index++] = 0x82;
        sendbuf[index++] = (uint8)(inLen>>8);
        sendbuf[index++] = (uint8)(inLen);
    }
    else
        sendbuf[index++] = inLen;

    memcpy(sendbuf+index,inData,inLen);//�����������ݵ�Ԫ
    index += inLen;
    
    sendbuf[index++] = 1;//CHOICE OPTIONAL
    sendbuf[index++] = 0;
    sendbuf[index++] = 4;
    //��ȡmac
    security_report_response_mac_get(metno,pAppLayer->rnbuf,12,inData,inLen,mac);
    memcpy(sendbuf+index,mac,4);
    index+=4;
        

    memcpy(pAppLayer->sendApduList->apdu,sendbuf,index);
    pAppLayer->sendApduList->apduLen = index;

}


/*******************************************************************************
* ��������: security_report_response_mac_get
* ��������:�㲥���ݼ���
���ͣ�800A4808+LC+�ն˵�ַ+Rand1+AGCTR+Data
���أ�9000+Len+Data1

* �������: mntaddr �ն˵�ַ8�ֽ�
* �������: 
* �� �� ֵ:
*******************************************************************************/
int security_broad_data_get(uint8* mntaddr,uint8* data,uint16 datalen,uint8* outdata)
{
    uint16 index = 0;
    uint16 len;
    uint32 broadcnt = 10000;
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0x0A;
    sendbuf[index++] = 0x48;
    sendbuf[index++] = 0x08;
    len = 8+16+4+datalen;   
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    memcpy(&sendbuf[index],mntaddr,8);
    index+=8;
    memcpy(&sendbuf[index],s_randPara,16);
    index+=16;
    //sendbuf[index++] = 0x00;
    //sendbuf[index++] = 0x00;
    //sendbuf[index++] = 0x00;
    //sendbuf[index++] = 0x01;  //�㲥��������һ���ܴ����ֵ1000
    memcpy_r(&sendbuf[index],&broadcnt,4);
    index+=4;
    memcpy(&sendbuf[index],data,datalen);
    index+=datalen;

    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK )
    {
        return 0;
    }
    memcpy(outdata,recvbuf,recvlen);
    return recvlen;

}

/*******************************************************************************
* ��������: security_cipher_data_mac_get
* ��������:���ļ���mac
���ͣ�800E4808+LC+�ն˵�ַ+Rand1+801E400A+
LC1+Rand1+01+Data1
���أ�9000+0004+MAC

* �������: mntaddr �ն˵�ַ8�ֽ�
* �������: 
* �� �� ֵ:
*******************************************************************************/
int security_cipher_data_mac_get(uint8* mntaddr,uint8* data,uint16 datalen,uint8* outdata)
{
    uint16 index = 0;
    uint16 len;
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;
    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0x0E;
    sendbuf[index++] = 0x48;
    sendbuf[index++] = 0x08;
    len = 8+16+4+2+16+1+datalen;   
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    memcpy(&sendbuf[index],mntaddr,8);
    index+=8;
    memcpy(&sendbuf[index],s_randPara,16);
    index+=16;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0x1E;
    sendbuf[index++] = 0x40;
    sendbuf[index++] = 0x0A;  
    len = 16+1+datalen+4;  //LC1���������ݳ���+4��2�ֽ�
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    memcpy(&sendbuf[index],s_randPara,16);
    index+=16;
    sendbuf[index++] = 0x01; 
    memcpy(&sendbuf[index],data,datalen);
    index+=datalen;

    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK || recvlen != 4)
    {
        return 0;
    }
    memcpy(outdata,recvbuf,recvlen);
    return recvlen;

}

/****************************************************************************
*ģ���ţ�                                                                  
*���ƣ�security_cipher_pack                                                                     
*���ܣ������������·�                                                              
*��������� inData ����                                                                 
*���������                                                                  
*����                                                                      
*���أ���                                                                    
****************************************************************************/
void security_cipher_pack(uint8 pipe,uint8 *inData,uint16 inLen,uint8 *attachData,uint8 *mac)
{
    AppLayerList_t* pAppLayer;
    pAppLayer = FindAppLayer(pipe,NULL);
    uint8 sendbuf[8192]={0};
    uint16 index=0;
    uint8 metno[8]={0};
    uint32 sid = 0x801E400A;
    memcpy(&metno[2],pAppLayer->sa.add,6);
    
    sendbuf[index++] = SERVERID_SECURITYREQUEST;
    sendbuf[index++] = 1;

    if(inLen>=0x80)
    {
        sendbuf[index++] = 0x82;
        sendbuf[index++] = (uint8)(inLen>>8);
        sendbuf[index++] = (uint8)(inLen);
    }
    else
        sendbuf[index++] = inLen;

    memcpy(sendbuf+index,inData,inLen);
    index += inLen;
    
    sendbuf[index++] = 0;//CHOICE sid_mac
    memcpy_r(&sendbuf[index],&sid,4);
    index+=4;
    sendbuf[index++] = 19;  //�������� ����Ϊ2+16+1
    memcpy(&sendbuf[index],attachData,19);
    index+=19;
    sendbuf[index++] = 4;
    memcpy(&sendbuf[index],mac,4);
    index+=4;
        
    memcpy(pAppLayer->sendApduList->apdu,sendbuf,index);
    pAppLayer->sendApduList->apduLen = index;

}

/*******************************************************************************
* ��������: security_normal_taskdata_get
* ��������:��ȡ������������ 80760300 
* �������:
* �������: 
* �� �� ֵ:���� 0��ʾ��esam����ʧ��
*******************************************************************************/
int security_normal_taskdata_get(uint8* key,uint16 keylen,uint8* taskdata,uint16 taskdatalen,uint8* outbuf)
{
    uint16 index = 0;
    uint16 len;
    uint8 sendbuf[1000]={0};
    int ret;
    uint8 recvbuf[100]={0};
    uint16 recvlen;

    sendbuf[index++] = 0x55;
    sendbuf[index++] = 0x80;
    sendbuf[index++] = 0x76;
    sendbuf[index++] = 0x03;
    sendbuf[index++] = 0x00;
    len = 1+keylen+taskdatalen;
    memcpy_r(&sendbuf[index],&len,2);
    index+=2;
    sendbuf[index++]=keylen/32;
    memcpy(&sendbuf[index],key,keylen);
    index+=keylen;

    memcpy(&sendbuf[index],taskdata,taskdatalen);
    index+=taskdatalen;

    ret = TESAM_RcvSndDataAndCheck(sendbuf,index,recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        return 0;
    }
    memcpy(outbuf,recvbuf,recvlen);
    return recvlen;

}


