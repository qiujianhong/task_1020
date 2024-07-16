

#include "CtsAll.h"

/****************************************************************************
*模块编号：                                                                  
*名称：ReportConfirmList
*功能：通知上报若干个对象属性
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
****************************************************************************/

eOopCommRslt_t ReportRequestList(AppLayerList_t *appLayerList)
{
    
    return eOopComm_Success;
}


/****************************************************************************
*模块编号：                                                                  
*名称：                                                                      
*功能：电表上报数据后的处理 
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
****************************************************************************/
eOopCommRslt_t ReportRequest(AppLayerList_t *appLayerList)
{
    if(appLayerList->recvApduList->apduLen <= 3)
        return eOopComm_FrameNotIntergrity;
    appLayerList->recvApduList->offSet = 3;
    switch(appLayerList->recvApduList->apdu[1])
    {
        case 1://通知上报若干个对象属性
        case 2:
             return(ReportRequestList(appLayerList));

        default:
            return eOopComm_ServerIDNotMatch;
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
void ReportResponseList(AppLayerList_t *pAppLayer,uint8 *inData,uint16 inLen)
{

    uint8 *apdu = pAppLayer->sendApduList->apdu+pAppLayer->sendApduList->offSet;
    
    memcpy(apdu,inData,inLen);

    pAppLayer->sendApduList->apduLen += inLen;
}

void ReportResponse(uint8 pipe,OOP_TSA_T *sa,uint8 nChoice,uint8 *inData,uint16 inLen)
{
    LinkLayerAddr_t a;
    LinkLayerCtrl_t ctrl;
    AppLayerList_t* pAppLayer;
    uint8 *apdu;
    pAppLayer = FindAppLayer(pipe,NULL);
    memset(pAppLayer->sendApduList,0x00,sizeof(ApduList_t));//APDU初始化
    pAppLayer->sa = *sa;//保存服务器地址
    a.af_len = sa->len;
    a.af = sa->af;
    memcpy_r(a.sa,sa->add,a.af_len+1);
    ctrl.ctrl = 0;
    ctrl.ctrl_fun = CTRLFUN_DATAEXCHANGE;
    ctrl.ctrl_prm = 0;
    
    apdu = pAppLayer->sendApduList->apdu;
    //目前只支持若干个对象属性和若干个记录型对象属性
    if((nChoice==0)||(nChoice>2))
        return ;//服务不支持
    apdu[0] = SERVERID_REPORTRESPONSE;
    apdu[1] = nChoice;
    apdu[2] = pAppLayer->recvApduList->apdu[2];
    pAppLayer->sendApduList->apduLen = 3;
    pAppLayer->sendApduList->offSet = 3;
    switch(nChoice)
    {
        case 1:
        case 2:
            ReportResponseList(pAppLayer,inData,inLen);
            break;

        default:
            return;
    }
    
    //时间标签
    *(pAppLayer->sendApduList->apdu+pAppLayer->sendApduList->apduLen) = 0x00;
    pAppLayer->sendApduList->apduLen+=1;

    if(pAppLayer->macflag == 1)
    {
        security_response(pipe,pAppLayer->sendApduList->apdu, pAppLayer->sendApduList->apduLen);
        pAppLayer->macflag = 0;
    }
    
    Client_Send_Apdu(pipe,&a,ctrl.ctrl, pAppLayer->sendApduList->apdu, pAppLayer->sendApduList->apduLen);

}
