/*-
****************************************************************************************
 * Copyright (c) 2004,南京新联电子仪器有限责任公司
 *            All rights reserved.
 * 程序名称：Security.c
 * 版本号：1.0
 * 功能：面向对象协议栈-应用层-安全传输接口
 * 开发人：qxc
 * 开发时间：2016-06-30
 * 修改者:
 * 修改时间:
 * 修改简要说明:
 * 其他： 
*****************************************************************************************
-*/
#include "CtsAll.h"

#define SAFE_MODE   1       //1启用 0不启用安全模式

uint8 s_randPara[16]={0};       //抄表随机数

extern int8 ReadMtrGetRand(uint8 *pOutRand);
/****************************************************************************
*模块编号：                                                                  
*名称：SecurityRequest                                                                      
*功能：安全传输请求                                                                      
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
****************************************************************************/
void SecurityRequest(uint8 pipe,uint8 *inData,uint16 inLen)
{
    AppLayerList_t* pAppLayer;
    pAppLayer = FindAppLayer(pipe,NULL);
    uint8 recvbuf[8192]={0};
    uint16 recvlen=0;
    
    if(inLen+30>sizeof(pAppLayer->sendApduList->apdu))
        return;
    //只支持明文+随机数方式读取

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

    memcpy(recvbuf+recvlen,inData,inLen);//加入明文数据单元
    recvlen += inLen;
    recvbuf[recvlen++] = 1;//随机数
    recvbuf[recvlen++] = 16;
    memcpy(recvbuf+recvlen,s_randPara,16);
    recvlen+=16;

    
    memcpy(pAppLayer->sendApduList->apdu,recvbuf,recvlen);
    pAppLayer->sendApduList->apduLen = recvlen;

}
/****************************************************************************
*模块编号：                                                                  
*名称：SecurityConfirm                                                                      
*功能：安全传输确认                                                                      
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
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
    if(appLayerList->recvApduList->apdu[offSet++] != 0x01    //不带MAC
        || appLayerList->recvApduList->apdu[offSet++] != 0x00)//不是MAC
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
    memcpy(s_randPara,buf,16); //随机数默认16个字节
    TASK_FMT_TRACE(-1,REC_LOG,"获取到随机数\n");
    TASK_BUF_TRACE(-1,REC_LOG, s_randPara, 16);
}


/*******************************************************************************
* 函数名称: security_rand1_get
* 函数功能:获取随机数1 
* 输入参数:
* 输出参数: 随机数1
* 返 回 值:随机数长度 0表示和esam交互失败
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
* 函数名称: security_sessondata1_get
* 函数功能:获取sessondata1 和mac1
* 输入参数:
* 输出参数: 
* 返 回 值:长度 0表示和esam交互失败
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
    len = inlen+4+1;    //密钥包长度+计数器4字节+一个字节FLG
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
* 函数名称: security_sessonkey_get
* 函数功能:获取会话密钥包 80740000
* 输入参数:
* 输出参数: 
* 返 回 值:长度 0表示和esam交互失败
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
* 函数名称: security_taskdata_get
* 函数功能:获取任务密文数据 80760301 校时
* 输入参数:
* 输出参数: 
* 返 回 值:长度 0表示和esam交互失败
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
    sendbuf[index++] = 7; //校时时间长度 
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
    //逻辑地址为1时也要填进去
    if(sa->oldvxd == 1)
    {
        a.af_logicAddr = 1;
    }

    //判断扩展逻辑地址
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
    apdu[apduLen++] =1; //密文
    apdu[apduLen++] = tasklen;   
    memcpy(apdu+apduLen,taskdata,tasklen);
    apduLen+=tasklen;
    apdu[apduLen++] =0; //数据验证码 SID_MAC
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
//698不要mac
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
    apdu[apduLen++] =1; //密文
    apdu[apduLen++] = tasklen;   
    memcpy(apdu+apduLen,taskdata,tasklen);
    apduLen+=tasklen;
    apdu[apduLen++] =3; //数据验证码 SID
    memcpy(apdu+apduLen,sid,4);
    apduLen+=4;
    apdu[apduLen++] = 2;
    memcpy(apdu+apduLen,appenddata,2);
    apduLen+=2;


    pAppLayer->sendApduList->apduLen = apduLen;

    Client_Send_Apdu(pipe,&a,ctrl.ctrl, pAppLayer->sendApduList->apdu, pAppLayer->sendApduList->apduLen);
}


/*******************************************************************************
* 函数名称: security_decrypt_sec_mac
* 函数功能:解密 80780000 密文加mac
* 输入参数:
* 输出参数: 
* 返 回 值:长度 0表示和esam交互失败
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
* 函数名称: security_endata2_get
* 函数功能:获取endata2 80D40300
* 输入参数:
* 输出参数: 
* 返 回 值:长度 0表示和esam交互失败
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
* 函数名称: security_esamset_data_get
* 函数功能:获取esam设置任务密文 80760301
* 输入参数:
* 输出参数: 
* 返 回 值:长度 0表示和esam交互失败
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
* 函数名称: security_sessonkey_get_698
* 函数功能:获取会话密钥包 80740100
* 输入参数:
* 输出参数: 
* 返 回 值:长度 0表示和esam交互失败
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
* 函数名称: security_decrypt_sec_mac
* 函数功能:解密 80780000 密文加mac
* 输入参数:
* 输出参数: 
* 返 回 值:长度 0表示和esam交互失败
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
    //如果没有mac 则用A6
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
* 函数名称: security_ER0_get
* 函数功能:获取er0 84540001 645表
* 输入参数:
* 输出参数: 
* 返 回 值:长度 0表示和esam交互失败
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
    len = keylen+8+8;   //分散因子8字节 R08字节

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
* 函数名称: security_secdata_get_645
* 函数功能:获取任务密文 84560404 645表
* 输入参数:
* 输出参数: 
* 返 回 值:长度 0表示和esam交互失败
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
    //时间
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
* 函数名称: security_report_response_mac_get
* 函数功能:获取事件上报后回复的mac 
发送：800E4807+LC+电能表+ Data2+00000000+Data3
返回：9000+0004+MAC
* 输入参数:
* 输出参数: 
* 返 回 值:
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
*模块编号：                                                                  
*名称：security_request_unpack                                                               
*功能：解析电表上报的数据                                                                     
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
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
    
    //只支持明文
    if(apdu[index++] != 0 )
    {
        return eOopComm_SecurityNotMatch;
    }
    PlainDataLen = get_len_offset(&apdu[index],&lenoffset);
    index+=lenoffset;
    PlainData = &apdu[index];
    index+=PlainDataLen;

    //解析随机数加mac
    if(apdu[index++] == 2)
    {
        //随机数
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
        //校验随机数加mac 获取回码的mac
       // security_rn_mac_verify(rnbuf,rnlen,mac,outmac);
        appLayerList->macflag = 1;
        memcpy(appLayerList->rnbuf,rnbuf,12);
    }
    
    memmove(appLayerList->recvApduList->apdu,PlainData,PlainDataLen);
    appLayerList->recvApduList->apduLen = PlainDataLen;
    
    return eOopComm_Success;
}


/****************************************************************************
*模块编号：                                                                  
*名称：security_responst                                                                      
*功能：安全传输响应                                                                     
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
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

    memcpy(sendbuf+index,inData,inLen);//加入明文数据单元
    index += inLen;
    
    sendbuf[index++] = 1;//CHOICE OPTIONAL
    sendbuf[index++] = 0;
    sendbuf[index++] = 4;
    //获取mac
    security_report_response_mac_get(metno,pAppLayer->rnbuf,12,inData,inLen,mac);
    memcpy(sendbuf+index,mac,4);
    index+=4;
        

    memcpy(pAppLayer->sendApduList->apdu,sendbuf,index);
    pAppLayer->sendApduList->apduLen = index;

}


/*******************************************************************************
* 函数名称: security_report_response_mac_get
* 函数功能:广播数据加密
发送：800A4808+LC+终端地址+Rand1+AGCTR+Data
返回：9000+Len+Data1

* 输入参数: mntaddr 终端地址8字节
* 输出参数: 
* 返 回 值:
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
    //sendbuf[index++] = 0x01;  //广播计数器用一个很大的数值1000
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
* 函数名称: security_cipher_data_mac_get
* 函数功能:密文计算mac
发送：800E4808+LC+终端地址+Rand1+801E400A+
LC1+Rand1+01+Data1
返回：9000+0004+MAC

* 输入参数: mntaddr 终端地址8字节
* 输出参数: 
* 返 回 值:
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
    len = 16+1+datalen+4;  //LC1：后续数据长度+4，2字节
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
*模块编号：                                                                  
*名称：security_cipher_pack                                                                     
*功能：组密文数据下发                                                              
*输入参数： inData 密文                                                                 
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
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
    sendbuf[index++] = 19;  //附加数据 长度为2+16+1
    memcpy(&sendbuf[index],attachData,19);
    index+=19;
    sendbuf[index++] = 4;
    memcpy(&sendbuf[index],mac,4);
    index+=4;
        
    memcpy(pAppLayer->sendApduList->apdu,sendbuf,index);
    pAppLayer->sendApduList->apduLen = index;

}

/*******************************************************************************
* 函数名称: security_normal_taskdata_get
* 函数功能:获取任务密文数据 80760300 
* 输入参数:
* 输出参数: 
* 返 回 值:长度 0表示和esam交互失败
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


