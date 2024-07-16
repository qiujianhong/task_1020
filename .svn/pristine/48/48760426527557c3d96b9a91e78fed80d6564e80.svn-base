#include "CtsAll.h"

extern uint8 s_randPara[16];

#pragma pack(1)
typedef struct{
    uint8 id;//服务ID
    uint8 choice;
    uint8 piid;
}ActionRequestHead_t;
typedef struct{
    uint8 id;//服务ID
    uint8 choice;
    uint8 piid;
    uint8 oad[4];
}ActionRequestNormalHead_t;
typedef struct{
    uint8 id;//服务ID
    uint8 choice;
    uint8 piid;
    uint8 oadNum;
}ActionRequestNormalListHead_t;

typedef struct{
    uint8 oad[4];
    //uint8 *dataBuff;
}ActionResponseNormalHead_t;

typedef struct{
    uint8 oadNum;
}ActionResponseNormalListHead_t;

#pragma pack()


/****************************************************************************
*模块编号：                                                                  
*名称：                                                                      
*功能：                                                                      
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
****************************************************************************/
void ActionRequestNormal(AppLayerList_t *pAppLayer,uint8 *inData,uint16 inLen)
{
    uint16 offSet = 0;
    uint8 *apdu = pAppLayer->sendApduList->apdu+pAppLayer->sendApduList->offSet;
    //SOAD *oad = (SOAD*)inData;
    if(inLen<=4)
        return;
    if(inLen>sizeof(pAppLayer->sendApduList->apdu)-pAppLayer->sendApduList->offSet)
        return;
    memcpy(apdu+offSet,inData,inLen);//填入OAD和Data
    offSet += inLen;
    
    pAppLayer->sendApduList->apduLen += offSet;
    pAppLayer->sendApduList->offSet += offSet;
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
void ActionRequest(uint8 pipe,OOP_TSA_T *sa,uint8 nChoice,uint8 *inData,uint16 inLen)
{
    LinkLayerAddr_t a;
    LinkLayerCtrl_t ctrl;
    AppLayerList_t* pAppLayer;
    ActionRequestHead_t *pRequestHead;
    pAppLayer = FindAppLayer(pipe,NULL);
    memset(pAppLayer->sendApduList,0x00,sizeof(ApduList_t));//APDU初始化
    pAppLayer->sa = *sa;//保存服务器地址
    a.af_len = sa->len;
    a.af = sa->af;
    memcpy_r(a.sa,sa->add,a.af_len+1);
    ctrl.ctrl = 0;
    ctrl.ctrl_fun = CTRLFUN_DATAEXCHANGE;
    ctrl.ctrl_prm = 1;
    pRequestHead = (ActionRequestHead_t*)pAppLayer->sendApduList->apdu;
    if((nChoice==0)||(nChoice>5))
        return ;//服务不支持
    pRequestHead->id = SERVERID_ACTIONREQUEST;
    pRequestHead->choice = nChoice;
    if(nChoice == 4)
        pRequestHead->choice = 1;
    pRequestHead->piid = 0;
    pAppLayer->sendApduList->apduLen = 3;
    pAppLayer->sendApduList->offSet = 3;
    switch(nChoice)
    {
        case 1://Normal
        case 4://搞一个特殊的4 转换为密文
            ActionRequestNormal(pAppLayer,inData,inLen);
            break;
        case 2://NormalList
            ActionRequestNormal(pAppLayer,inData,inLen);
            break;
        case 3://ThenGetNormalList
            ActionRequestNormal(pAppLayer,inData,inLen);
            break;
        default:
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "nChoice %d error \n",nChoice);
        }
            return;
    }
    if(pAppLayer->sendApduList->apduLen <= sizeof(ActionRequestHead_t))
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "apduLen %d error \n",pAppLayer->sendApduList->apduLen);
        return;
    }
    //printf("sa_len=%d: ", a.af_len+1);
    //commfun_printstring( a.sa, a.af_len+1);
    //printf("send buf len=%d: ", pAppLayer->sendApduList->apduLen);
    //commfun_printstring( pAppLayer->sendApduList->apdu, pAppLayer->sendApduList->apduLen);
    //时间标签
    *(pAppLayer->sendApduList->apdu+pAppLayer->sendApduList->apduLen) = 0x00;
    pAppLayer->sendApduList->apduLen+=1;

    if(nChoice == 4)
    {
        uint16 len=0;
        uint8 mac[4];
        uint8 cipherbuf[1000]={0};
        uint8 mntAddr[8]={0};
        uint8 attachData[19]={0};
        uint16 lc1=0;
        uint8 index = 0;
        memcpy_r(&mntAddr[2],LocalAddr,6);
        //获取密文数据 
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "明文apdu \n");
        TASK_BUF_DEBUG(pipe, RELAY_LOG, pAppLayer->sendApduList->apdu, pAppLayer->sendApduList->apduLen);
        len = security_broad_data_get(mntAddr,pAppLayer->sendApduList->apdu, pAppLayer->sendApduList->apduLen,cipherbuf);
        if(len > 0)
        {
            
            if(security_cipher_data_mac_get(mntAddr,cipherbuf,len,mac)==4)
            {
                lc1 = 16+1+len+4;
                memcpy_r(attachData,&lc1,2);
                index+=2;
                memcpy(&attachData[index],s_randPara,16);
                index+=16;
                attachData[index++] = 1;
                security_cipher_pack(pipe,cipherbuf,len,attachData,mac);
            }
        }
    }
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "ActionRequest apdu \n");
    TASK_BUF_DEBUG(pipe, RELAY_LOG, pAppLayer->sendApduList->apdu, pAppLayer->sendApduList->apduLen);
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
eOopCommRslt_t ActionConfirmNormal(AppLayerList_t *appLayerList)
{
    //SetRequestNormalHead_t *sendApdu;
    //SetResponseNormalHead_t *recvApdu; 
    uint16 apduLen = appLayerList->recvApduList->apduLen -appLayerList->recvApduList->offSet ;
    //uint16 offset = appLayerList->recvApduList->offSet;
    //SOAD oad ;
    if(apduLen < 6)
    {
        printf("APP Layer err =%d,apdulen =%d,headlen=%d\n",eOopComm_FrameNotIntergrity,apduLen,5);
        return eOopComm_FrameNotIntergrity;
    }
    if(appLayerList->recvApduList->apdu[appLayerList->recvApduList->offSet+5]==0)
    {
        appLayerList->recvApduList->offSet += 6;
    }
    else
    {

        appLayerList->recvApduList->offSet += 6;
        if(FALSE == taskmng_oop_next_read_data_get(appLayerList->sendApduList->apdu,&appLayerList->sendApduList->offSet,appLayerList->sendApduList->apduLen,DT_NULL,FALSE))
        {
          return eOopComm_FrameNotIntergrity;
        }
    }
    return eOopComm_Success;
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
eOopCommRslt_t ActionConfirmNormalList(AppLayerList_t *appLayerList)
{
    ActionRequestNormalListHead_t *sendApdu;
    ActionResponseNormalListHead_t *recvApdu; 
    uint16 apduLen = appLayerList->recvApduList->apduLen -appLayerList->recvApduList->offSet ;
    uint16 offset = appLayerList->recvApduList->offSet;
    uint8 i;
    //SOAD oad ;
    eOopCommRslt_t rslt;
    if(apduLen < 1)
        return eOopComm_FrameNotIntergrity;
    sendApdu = (ActionRequestNormalListHead_t *)appLayerList->sendApduList->apdu;
    recvApdu =  (ActionResponseNormalListHead_t *)(appLayerList->recvApduList->apdu + offset);
    
    if(sendApdu->oadNum != recvApdu->oadNum)
        return eOopComm_OADNotMatch;
    appLayerList->recvApduList->offSet += 1;
    for(i=0;i<recvApdu->oadNum;i++)
    {
        rslt = ActionConfirmNormal(appLayerList);
        if(eOopComm_Success != rslt)
            return rslt;
    }
    return eOopComm_Success;
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
eOopCommRslt_t ActionThenGetConfirmList(AppLayerList_t *appLayerList)
{
    ActionRequestNormalListHead_t *sendApdu;
    ActionResponseNormalListHead_t *recvApdu; 
    uint16 apduLen = appLayerList->recvApduList->apduLen -appLayerList->recvApduList->offSet ;
    uint16 offset = appLayerList->recvApduList->offSet;
    uint8 i;
    //SOAD oad ;
    eOopCommRslt_t rslt;
    //uint8 getRslt;
    if(apduLen < 1)
        return eOopComm_FrameNotIntergrity;
    sendApdu = (ActionRequestNormalListHead_t *)appLayerList->sendApduList->apdu;
    recvApdu =  (ActionResponseNormalListHead_t *)(appLayerList->recvApduList->apdu + offset);
    
    if(sendApdu->oadNum != recvApdu->oadNum)
        return eOopComm_OADNotMatch;
    appLayerList->recvApduList->offSet += 1;
    TASK_FMT_TRACE(-1, RELAY_LOG,"recvApdu->oadNum is %d \n",recvApdu->oadNum);
    for(i=0;i<recvApdu->oadNum;i++)
    {
        rslt = ActionConfirmNormal(appLayerList);
        TASK_FMT_TRACE(-1, RELAY_LOG,"rslt is %d \n",rslt);
        if(eOopComm_Success != rslt)
            return rslt;
        TASK_FMT_TRACE(-1, RELAY_LOG,"11appLayerList->recvApduList->offSet is %d\n",appLayerList->recvApduList->offSet);
        if(FALSE == taskmng_oop_next_read_data_get(appLayerList->recvApduList->apdu,&appLayerList->recvApduList->offSet,appLayerList->recvApduList->apduLen,
                            DT_OAD,TRUE))
        {      
            return eOopComm_FrameNotIntergrity;
        }
        if(appLayerList->recvApduList->apdu[appLayerList->recvApduList->offSet++]==0x00)
        {//错误信息
            appLayerList->recvApduList->offSet++;
        }
        else
        {
            TASK_FMT_TRACE(-1, RELAY_LOG,"22appLayerList->recvApduList->offSet is %d\n",appLayerList->recvApduList->offSet);
            if(FALSE == taskmng_oop_next_read_data_get(appLayerList->recvApduList->apdu,&appLayerList->recvApduList->offSet,appLayerList->recvApduList->apduLen,
                            DT_NULL,FALSE))
            {     
            return eOopComm_FrameNotIntergrity;
            }
        }
    }
    return eOopComm_Success;
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
eOopCommRslt_t ActionConfirm(AppLayerList_t *appLayerList)
{
    if(appLayerList->recvApduList->apduLen <= 3)
        return eOopComm_FrameNotIntergrity;
    if((appLayerList->sendApduList->apdu[1]) !=appLayerList->recvApduList->apdu[1])
    {//发送与接收的服务必须一致
            return eOopComm_ServerIDNotMatch;
    }
    appLayerList->recvApduList->offSet = 3;
    switch(appLayerList->recvApduList->apdu[1])
    {
        case 1://Normal
             return(ActionConfirmNormal(appLayerList));
        case 2://NormalList
            return(ActionConfirmNormalList(appLayerList));
        case 3://
            return(ActionThenGetConfirmList(appLayerList));
        default:
            return eOopComm_ServerIDNotMatch;
    }
    
}

