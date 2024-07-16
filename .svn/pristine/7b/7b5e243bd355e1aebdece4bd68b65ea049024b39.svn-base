/*-
****************************************************************************************
 * Copyright (c) 2004,南京新联电子仪器有限责任公司
 *            All rights reserved.
 * 程序名称：service.c
 * 版本号：1.0
 * 功能：面向对象协议栈-应用层
 * 开发人：qxc
 * 开发时间：2016-06-30
 * 修改者:
 * 修改时间:
 * 修改简要说明:
 * 其他： 
*****************************************************************************************
-*/
#include "CtsAll.h"

AppLayerList_t *AppLayerHeadPtr = NULL;

const OopGet_t OopGet={
    GetRequest,
    NULL,
    NULL,
    GetConfirm,
};

const OopSet_t OopSet={
    SetRequest,
    NULL,
    NULL,
    SetConfirm,
};

const OopAction_t OopAction={
    ActionRequest,
    NULL,
    NULL,
    ActionConfirm,
};


const OopConnect_t OopConnect={
    ConnectRequest,
    NULL,
    NULL,
    ConnectConfirm,
};

const OopReport_t OopReport = {
    ReportRequest,
    NULL,
    ReportResponse,
    NULL,
};
/*
const OopRelease_t OopRelease={
    ReleaseRequest,
    NULL,
    NULL,
    ReleaseConfirm,
    ReleaseNotification,
};
*/
//const ConnectInfo_t ClientConnectInfo=
//{
//    0x00,//请求的应用连接的ID
//    {0x00,0x17},//协议版本号
//    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//协议一致性
//    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},//功能一致性
//    MAX_LINK_LENGTH,//发送帧长度
//    MAX_LINK_LENGTH,//接收帧长度
//    1,//接收帧个数
//    MAX_APDU_LENGTH,//最大可处理APDU尺寸
//    8,
//    "02000000",
//    0,
//    {0x00,},//随机数
//    
//};

const ConnectInfo_t ClientConnectInfo=
{
    0x00,//请求的应用连接的ID
    {0x00,0x0c},//协议版本号
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//协议一致性
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//功能一致性
    MAX_LINK_LENGTH,//发送帧长度
    MAX_LINK_LENGTH,//接收帧长度
    1,//接收帧个数
    MAX_APDU_LENGTH,//最大可处理APDU尺寸
    8,
    "02000000",
    0,
    {0x00,},//随机数
    
};

/****************************************************************************
*模块编号：                                                                  
*名称：                                                                      
*功能：                                                                      
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
****************************************************************************/
void DelApduList(ApduList_t *apduList )
{
    ApduList_t *pApduList = apduList;
    ApduList_t *tmpList;
    while(pApduList!=NULL)
    {
        tmpList = pApduList->prior;
        free(pApduList);
        pApduList = tmpList;
    }
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
ApduList_t * AddApduList(ApduList_t *apduList )
{
    ApduList_t *pApduList;
    pApduList=  (ApduList_t*)malloc(sizeof(ApduList_t));
    memset(pApduList,0x00,sizeof(ApduList_t));
    pApduList->prior = apduList;
    return(pApduList);
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
AppLayerList_t* FindAppLayer(uint8 pipe,OOP_TSA_T *sa)
{
    AppLayerList_t *appLayerPtr = AppLayerHeadPtr;
    AppLayerList_t *lastAppLayerPtr = AppLayerHeadPtr;
    while(appLayerPtr != NULL)
    {
        if(appLayerPtr->pipe == pipe)
        {
            if(sa!=NULL)
            {
                
            }
            return appLayerPtr;
        }
        lastAppLayerPtr = appLayerPtr;
        appLayerPtr = appLayerPtr->next;
    }
    appLayerPtr = (AppLayerList_t*)malloc(sizeof(AppLayerList_t));
    memset(appLayerPtr,0x00,sizeof(AppLayerList_t));
    appLayerPtr->connectId= 0;
    appLayerPtr->pipe = pipe;//根据物理通道号应用层
    appLayerPtr->recvApduList=  (ApduList_t*)malloc(sizeof(ApduList_t));
    memset(appLayerPtr->recvApduList,0x00,sizeof(ApduList_t));
    appLayerPtr->recvApduList->prior = NULL;
    appLayerPtr->sendApduList=  (ApduList_t*)malloc(sizeof(ApduList_t));
    memset(appLayerPtr->sendApduList,0x00,sizeof(ApduList_t));
    appLayerPtr->sendApduList->prior = NULL;
    appLayerPtr->clientConnectInfo = ClientConnectInfo;
    appLayerPtr->next = NULL;
    if(lastAppLayerPtr!=NULL)
        lastAppLayerPtr->next = appLayerPtr;
    else
        AppLayerHeadPtr = appLayerPtr;
    return appLayerPtr;
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
void DelAppLayer(uint8 pipe,OOP_TSA_T *sa)
{
    AppLayerList_t *appLayerPtr = AppLayerHeadPtr;
    AppLayerList_t *lastAppLayerPtr = AppLayerHeadPtr;
    while(appLayerPtr != NULL)
    {
        if(appLayerPtr->pipe == pipe)
        {
            if(sa!=NULL)
            {
                
            }
            lastAppLayerPtr = appLayerPtr->next;
            if(appLayerPtr->recvApduList !=NULL)
                DelApduList(appLayerPtr->recvApduList);
            if(appLayerPtr->sendApduList !=NULL)
                DelApduList(appLayerPtr->sendApduList);
            free(appLayerPtr);
            if(appLayerPtr == AppLayerHeadPtr)
                AppLayerHeadPtr = NULL;
            return;
        }
        appLayerPtr = lastAppLayerPtr;
    }
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
void AppLayerTimeOutDeal(uint8 pipe)
{//链路层通信超时管理
    AppLayerList_t *pAppLayer;
    pAppLayer = FindAppLayer(pipe,NULL);
    if(pAppLayer->connectId>0)//预连接不可删除
    {
        if(pAppLayer->timeOut < (time(NULL)-pAppLayer->establishTime))
        {
            DelAppLayer(pipe,NULL);
        }
    }
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
eOopCommRslt_t AppLayerConfirm(uint8 pipe,uint8*inData,uint16 inLen)
{

    AppLayerList_t *appLayerList = FindAppLayer(pipe,NULL);
    if(inLen>sizeof(appLayerList->recvApduList->apdu))
        return eOopComm_AppCacheOver;
    if(inLen<1)
        return eOopComm_FrameNotIntergrity;
    //if(*inData&0x7F != appLayerList->serverId)
    //    return 
    //commfun_printstring(inData,inLen);
    memmove(appLayerList->recvApduList->apdu,inData,inLen);
    appLayerList->recvApduList->apduLen = inLen;
    //commfun_printstring(appLayerList->recvApduList->apdu,inLen);

    switch(*inData)
    {
    /*
        case SERVERID_LINKREQUEST:
            break;
        case SERVERID_CONNECTRESPONSE:
            return(OopConnect.confirm(appLayerList));
            break;
        case SERVERID_RELEASERESPONSE:
            return(OopRelease.confirm(appLayerList));
            break;
        case SERVERID_RELEASENOTIFICATION:
            return(OopRelease.Notification(appLayerList));
            break;
            */
        case SERVERID_CONNECTRESPONSE:
            return(OopConnect.confirm(appLayerList));
            break;
        case SERVERID_GETRESPONSE:
            return(OopGet.confirm(appLayerList));
            break;
        case SERVERID_SETRESPONSE:
            return(OopSet.confirm(appLayerList));
            break;
        case SERVERID_ACTIONRESPONSE:
            return(OopAction.confirm(appLayerList));
            break;
        case SERVERID_REPORTREQUEST:
            return (OopReport.requst(appLayerList));;
            break;
        /*case SERVERID_PROXYRESPONSE:
            break;*/
        case SERVERID_SECURITYREQUEST:
            {
                if(eOopComm_Success != security_request_unpack(appLayerList))
                    return eOopComm_SecurityNotMatch;
                return (AppLayerConfirm(pipe,appLayerList->recvApduList->apdu,appLayerList->recvApduList->apduLen));
            }
            break;
        case SERVERID_SECURITYRESPONSE:
            {
                eOopCommRslt_t rslt;
                if(appLayerList->recvApduList->apdu[1]==1) //安全响应为密文数据 转加密方案用
                {
                    return eOopComm_Success;
                }
                rslt = SecurityConfirm(appLayerList);
                if(eOopComm_Success != rslt)
                    return rslt;
                return(AppLayerConfirm(pipe,appLayerList->recvApduList->apdu,appLayerList->recvApduList->apduLen));
            }
            break;        
        default:
            break;
    }
    return(eOopComm_ServerIDNotMatch);
}
