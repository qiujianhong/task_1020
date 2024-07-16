#include "CtsAll.h"

typedef struct{
    uint8 id;//服务ID
    uint8 piid;
    uint8 ptrlVer[2];//协议版本号
    uint8 ptrlConfrom[8];//协议一致性
    uint8 funcConfrom[16];//功能一致性
    uint8 sendFrameLen[2];//客户机发送的帧最大长度
    uint8 recvFrameLen[2];//客户机接收的帧最大长度
    uint8 recvFrameNum;//客户机接收的帧个数
    uint8 maxApduSize[2];//客户机最大可处理APDU尺寸
    uint8 timeOut[4];//期望的超时时间
    uint8 security[1];//安全信息
}__attribute__ ((packed)) ConnectRequestHead_t;

typedef struct{
    uint8 id;//服务ID
    uint8 piid;
    uint8 facInfo[32];//厂商版本信息
    uint8 ptrlVer[2];//协议版本号
    uint8 ptrlConfrom[8];//协议一致性
    uint8 funcConfrom[16];//功能一致性
    uint8 sendFrameLen[2];//客户机发送的帧最大长度
    uint8 recvFrameLen[2];//客户机接收的帧最大长度
    uint8 recvFrameNum;//客户机接收的帧个数
    uint8 maxApduSize[2];//客户机最大可处理APDU尺寸
    uint8 timeOut[4];//期望的超时时间
    uint8 securityRslt[1];//认证结果
    uint8 securityInfo[1];//安全信息
}__attribute__ ((packed)) ConnectResponseHead_t;

void ConnectRequest(uint8 pipe,OOP_TSA_T*sa,uint8 connectId,uint32 timeOut,uint8* secret,uint8* sign)
{
    uint16 apduLen;
    ConnectRequestHead_t *apdu;
    uint8 *securityApdu;
    LinkLayerAddr_t a;
    LinkLayerCtrl_t ctrl;
    AppLayerList_t* pAppLayer;
    pAppLayer = FindAppLayer(pipe,NULL);
    apdu = (ConnectRequestHead_t*)pAppLayer->sendApduList->apdu;
   // a.af_len = sa->len;
   // memcpy_r(a.sa,sa->add,a.af_len+1);

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
    apdu->id = SERVERID_CONNECTREQUEST;
    apdu->piid = 0;
    apdu->ptrlVer[0] = pAppLayer->clientConnectInfo.prtlVersion[0];
    apdu->ptrlVer[1] = pAppLayer->clientConnectInfo.prtlVersion[1];
    memcpy(apdu->ptrlConfrom,pAppLayer->clientConnectInfo.prtlConformance,8);
    memcpy(apdu->funcConfrom,pAppLayer->clientConnectInfo.funcConformance,16);
    memcpy_r(apdu->sendFrameLen,(uint8*)&pAppLayer->clientConnectInfo.sendFrameLen,2);
    memcpy_r(apdu->recvFrameLen,(uint8*)&pAppLayer->clientConnectInfo.recvFrameLen,2);
    memcpy_r(apdu->maxApduSize,(uint8*)&pAppLayer->clientConnectInfo.maxApduSize,2);
    apdu->recvFrameNum = pAppLayer->clientConnectInfo.recvFrameNum;
    memcpy_r(apdu->timeOut,(uint8*)&timeOut,sizeof(apdu->timeOut));
    pAppLayer->timeOut = timeOut;

    apdu->security[0] = connectId;
    securityApdu = pAppLayer->sendApduList->apdu+sizeof(ConnectRequestHead_t);
    pAppLayer->sendApduList->apduLen = sizeof(ConnectRequestHead_t);
    apduLen = 0;
    switch(connectId)
    {
        case 0x01://密码
            securityApdu[apduLen++] = pAppLayer->clientConnectInfo.passWordLen;
            memcpy(securityApdu+apduLen,pAppLayer->clientConnectInfo.passWord,pAppLayer->clientConnectInfo.passWordLen);
            apduLen += 8;
            break;
        case 0x02://对称秘钥
            securityApdu[apduLen++] = 32;
            memcpy(securityApdu+apduLen,secret,32);
            apduLen+=32;
            securityApdu[apduLen++] = 4;
            memcpy(securityApdu+apduLen,sign,4);
            apduLen+=4;
            break;
        case 0x03://数据签名
            //break;
        default://无安全
            securityApdu[apduLen++] = 0x00;
            break;
    }
    securityApdu[apduLen++]= 0x00;  //时间标签
    pAppLayer->sendApduList->apduLen += apduLen;
    
    TASK_FMT_TRACE( pipe, REC_LOG, "ConnectRequest \n");
    TASK_BUF_TRACE(pipe, REC_LOG,pAppLayer->sendApduList->apdu, pAppLayer->sendApduList->apduLen);
    //调用链路层数据传输服务
    //printf("应用连接信息APDU:\n");
    //commfun_printstring(pAppLayer->sendApduList->apdu,pAppLayer->sendApduList->apduLen);
    Client_Send_Apdu(pipe,&a,ctrl.ctrl, pAppLayer->sendApduList->apdu, pAppLayer->sendApduList->apduLen);
}
/****************************************************************************
*模块编号：                                                                  
*名称：                                                                      
*功能：                                                                      
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
****************************************************************************/
eOopCommRslt_t ConnectConfirm(AppLayerList_t *appLayerList)
{
    ConnectResponseHead_t *apdu = (ConnectResponseHead_t *)appLayerList->recvApduList->apdu;
    uint16 apduLen = appLayerList->recvApduList->apduLen;
    uint16 offSet = 0;
    uint8 *securityData;
    if(apduLen<sizeof(ConnectResponseHead_t))
        return eOopComm_FrameNotIntergrity;
    //验证结果处理
    //commfun_printstring((uint8*)apdu,sizeof(ConnectResponseHead_t));
    memcpy_r((uint8*)&appLayerList->timeOut, apdu->timeOut,sizeof(appLayerList->timeOut));
    //printf("商定的超时时间=%d\n",appLayerList->timeOut);
    switch(apdu->securityRslt[0])
    {
        case 0x00://允许建立应用连接
           // switch(appLayerList->clientConnectInfo.connectId)
            {
                //case 0x02:
                //case 0x03:
                    if(apdu->securityInfo[0]== 0x00)
                        return eOopComm_ConnectNoSecData;
                    if(apdu->securityInfo[0]== 0x01)
                    {
                        //加密信息验证
                        securityData = (uint8*)apdu+sizeof(ConnectResponseHead_t);
                        appLayerList->serverConnectInfo.randLen = securityData[offSet++];
                        if(appLayerList->serverConnectInfo.randLen>sizeof(appLayerList->serverConnectInfo.rand))
                            return eOopComm_ConnectRandLenErr;
                        memcpy(appLayerList->serverConnectInfo.rand,securityData+offSet,appLayerList->serverConnectInfo.randLen);
                        offSet += appLayerList->serverConnectInfo.randLen;
                        appLayerList->serverConnectInfo.signLen = securityData[offSet++];
                        if(appLayerList->serverConnectInfo.signLen>sizeof(appLayerList->serverConnectInfo.sign))
                            return eOopComm_ConnectRandLenErr;
                        memcpy(appLayerList->serverConnectInfo.sign,securityData+offSet,appLayerList->serverConnectInfo.signLen);
                        offSet+=appLayerList->serverConnectInfo.signLen;
                        TASK_FMT_DEBUG(appLayerList->pipe, REC_LOG,"应用连接随机数 \n");
                        TASK_BUF_DEBUG(appLayerList->pipe, REC_LOG,appLayerList->serverConnectInfo.rand,appLayerList->serverConnectInfo.randLen);
                        TASK_FMT_DEBUG(appLayerList->pipe, REC_LOG,"应用连接签名信息 \n");
                        TASK_BUF_DEBUG(appLayerList->pipe, REC_LOG,appLayerList->serverConnectInfo.sign,appLayerList->serverConnectInfo.signLen);
                    }

                 //   break;
            }
            appLayerList->connectId = appLayerList->clientConnectInfo.connectId;
            appLayerList->establishTime = time(NULL);
            return eOopComm_Success;
        case 0x01:
            return(eOopComm_ConnectPswErr);
        case 0x02:
            return(eOopComm_ConnectSymKeyErr);
        case 0x03:
            return(eOopComm_ConnectAsymKeyErr);
        case 0x04:
            return(eOopComm_ConnectSignaKeyErr);
        default:
            return(eOopComm_ConnectPswErr);
    }
}


