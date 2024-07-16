#include "CtsAll.h"


#pragma pack(1)
typedef struct{
    uint8 id;//服务ID
    uint8 choice;
    uint8 piid;
}SetRequestHead_t;
typedef struct{
    uint8 id;//服务ID
    uint8 choice;
    uint8 piid;
    uint8 oad[4];
}SetRequestNormalHead_t;
typedef struct{
    uint8 id;//服务ID
    uint8 choice;
    uint8 piid;
    uint8 oadNum;
}SetRequestNormalListHead_t;


typedef struct{
    uint8 oad[4];
    //uint8 *dataBuff;
}SetResponseNormalHead_t;

typedef struct{
    uint8 oadNum;
}SetResponseNormalListHead_t;

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
void SetRequestNormal(AppLayerList_t *pAppLayer,uint8 *inData,uint16 inLen)
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
void SetRequest(uint8 pipe,OOP_TSA_T *sa,uint8 nChoice,uint8 *inData,uint16 inLen)
{
    LinkLayerAddr_t a;
    LinkLayerCtrl_t ctrl;
    AppLayerList_t* pAppLayer;
    SetRequestHead_t *pRequestHead;
    pAppLayer = FindAppLayer(pipe,NULL);
    memset(pAppLayer->sendApduList,0x00,sizeof(ApduList_t));//APDU初始化
    pAppLayer->sa = *sa;//保存服务器地址
    a.af_len = sa->len;
    memcpy_r(a.sa,sa->add,a.af_len+1);
    ctrl.ctrl = 0;
    ctrl.ctrl_fun = CTRLFUN_DATAEXCHANGE;
    ctrl.ctrl_prm = 1;
    pRequestHead = (SetRequestHead_t*)pAppLayer->sendApduList->apdu;
    if((nChoice==0)||(nChoice>5))
        return ;//服务不支持
    pRequestHead->id = SERVERID_SETREQUEST;
    pRequestHead->choice = nChoice;
    pRequestHead->piid = 0;
    pAppLayer->sendApduList->apduLen = 3;
    pAppLayer->sendApduList->offSet = 3;
    switch(nChoice)
    {
        case 1://Normal
            SetRequestNormal(pAppLayer,inData,inLen);
            break;
        case 2://NormalList
            SetRequestNormal(pAppLayer,inData,inLen);
            break;
        case 3://ThenGetNormalList
            SetRequestNormal(pAppLayer,inData,inLen);
            break;
        default:
            return;
    }
    
    if(pAppLayer->sendApduList->apduLen <= sizeof(SetRequestHead_t))
        return;
    //printf("sa_len=%d: ", a.af_len+1);
    //commfun_printstring( a.sa, a.af_len+1);
    //printf("send buf len=%d: ", pAppLayer->sendApduList->apduLen);
    //commfun_printstring( pAppLayer->sendApduList->apdu, pAppLayer->sendApduList->apduLen);
    //时间标签
    *(pAppLayer->sendApduList->apdu+pAppLayer->sendApduList->apduLen) = 0x00;
    pAppLayer->sendApduList->apduLen+=1;
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
eOopCommRslt_t SetConfirmNormal(AppLayerList_t *appLayerList)
{
    //SetRequestNormalHead_t *sendApdu;
    //SetResponseNormalHead_t *recvApdu; 
    uint16 apduLen = appLayerList->recvApduList->apduLen -appLayerList->recvApduList->offSet ;
    //uint16 offset = appLayerList->recvApduList->offSet;
    //SOAD oad ;
    if(apduLen < 5)
    {
        printf("APP Layer err =%d,apdulen =%d,headlen=%d\n",eOopComm_FrameNotIntergrity,apduLen,5);
        return eOopComm_FrameNotIntergrity;
    }
    appLayerList->recvApduList->offSet += 5;
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
eOopCommRslt_t SetConfirmNormalList(AppLayerList_t *appLayerList)
{
    SetRequestNormalListHead_t *sendApdu;
    SetResponseNormalListHead_t *recvApdu; 
    uint16 apduLen = appLayerList->recvApduList->apduLen -appLayerList->recvApduList->offSet ;
    uint16 offset = appLayerList->recvApduList->offSet;
    uint8 i;
    //SOAD oad ;
    eOopCommRslt_t rslt;
    if(apduLen < 1)
        return eOopComm_FrameNotIntergrity;
    sendApdu = (SetRequestNormalListHead_t *)appLayerList->sendApduList->apdu;
    recvApdu =  (SetResponseNormalListHead_t *)(appLayerList->recvApduList->apdu + offset);
    
    if(sendApdu->oadNum != recvApdu->oadNum)
        return eOopComm_OADNotMatch;
    appLayerList->recvApduList->offSet += 1;
    for(i=0;i<recvApdu->oadNum;i++)
    {
        rslt = SetConfirmNormal(appLayerList);
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
eOopCommRslt_t SetThenGetConfirmList(AppLayerList_t *appLayerList)
{
    SetRequestNormalListHead_t *sendApdu;
    SetResponseNormalListHead_t *recvApdu; 
    uint16 apduLen = appLayerList->recvApduList->apduLen -appLayerList->recvApduList->offSet ;
    uint16 offset = appLayerList->recvApduList->offSet;
    uint8 i;
    //SOAD oad ;
    eOopCommRslt_t rslt;
    //uint8 getRslt;
    if(apduLen < 1)
    {   
        return eOopComm_FrameNotIntergrity;
    }
    sendApdu = (SetRequestNormalListHead_t *)appLayerList->sendApduList->apdu;
    recvApdu =    (SetResponseNormalListHead_t *)(appLayerList->recvApduList->apdu + offset);
    

    if(sendApdu->oadNum != recvApdu->oadNum)
        return eOopComm_OADNotMatch;
    appLayerList->recvApduList->offSet += 1;

    for(i=0;i<recvApdu->oadNum;i++)
    {
        rslt = SetConfirmNormal(appLayerList);
        if(eOopComm_Success != rslt)
        {      
            return rslt;
        }
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
eOopCommRslt_t SetConfirm(AppLayerList_t *appLayerList)
{
    //printf("App Layer Service:GetConfirm\n");
    if(appLayerList->recvApduList->apduLen <= 3)
        return eOopComm_FrameNotIntergrity;
    //if((appLayerList->sendApduList->apdu[1]) !=appLayerList->recvApduList->apdu[1])
    //{//发送与接收的服务必须一致
    //        return eOopComm_ServerIDNotMatch;
    //}
    appLayerList->recvApduList->offSet = 3;
    switch(appLayerList->recvApduList->apdu[1])
    {
        case 1://Normal
             return(SetConfirmNormal(appLayerList));
        case 2://NormalList
            return(SetConfirmNormalList(appLayerList));
        case 3://
            return(SetThenGetConfirmList(appLayerList));
        default:
            return eOopComm_ServerIDNotMatch;
    }
    
}


