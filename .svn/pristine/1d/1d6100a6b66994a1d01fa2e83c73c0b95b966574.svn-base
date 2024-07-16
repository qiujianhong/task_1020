/*-
****************************************************************************************
 * Copyright (c) 2004,南京新联电子仪器有限责任公司
 *            All rights reserved.
 * 程序名称：get.c
 * 版本号：1.0
 * 功能：面向对象协议栈-应用层-GET服务接口
 * 开发人：qxc
 * 开发时间：2016-06-30
 * 修改者:
 * 修改时间:
 * 修改简要说明:
 * 其他： 
*****************************************************************************************
-*/

#include "CtsAll.h"


#pragma pack(1)
typedef struct{
    uint8 id;//服务ID
    uint8 choice;
    uint8 piid;
}GetRequestHead_t;

typedef struct{
    uint8 id;//服务ID
    uint8 choice;
    uint8 piid;
    uint8 oad[4];
}GetRequestNormalHead_t;
typedef struct{
    uint8 id;//服务ID
    uint8 choice;
    uint8 piid;
    uint8 oadNum;
}GetRequestNormalListHead_t;

typedef struct{
    uint8 id;//服务ID
    uint8 choice;
    uint8 piid;
    uint8 recNum;
}GetRequestRecordListHead_t;

typedef struct{
    uint8 id;//服务ID
    uint8 choice;
    uint8 piid;
    uint8 frameNo[2];//帧序号
}GetRequestNextHead_t;


typedef struct{
    uint8 oad[4];
    //uint8 *dataBuff;
}GetResponseNormalHead_t;

typedef struct{
    uint8 oadNum;
}GetResponseNormalListHead_t;

typedef struct{
    uint8 lastFlg;//最后一帧标识
    uint8 frameNo[2];//帧序号
    uint8 choice;
}GetResponseNextHead_t;
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
void GetRequestNormal(AppLayerList_t *pAppLayer,uint8 *inData,uint16 inLen)
{
    uint16 offSet = 0;
    uint8 *apdu = pAppLayer->sendApduList->apdu+pAppLayer->sendApduList->offSet;
    OOP_OAD_U *oad = (OOP_OAD_U*)inData;
    if(inLen!=4)
        return;
    apdu[offSet++] = (uint8)(oad[0].nObjID >>8);
    apdu[offSet++] = (uint8)oad[0].nObjID ;
    apdu[offSet++] = oad[0].attID|(oad[0].attPro<<5);
    apdu[offSet++] = oad[0].nIndex;

    pAppLayer->sendApduList->apduLen += offSet;
    pAppLayer->sendApduList->offSet += offSet;
    SecModWd_t SecModWd;
    //SecModWd = GeSecMod(oad[0].OI);
    SecModWd = GetMtrSecMod(pAppLayer->pipe,oad[0].nObjID);
    pAppLayer->secMod = SecModWd.get;
    printf("GetRequestNormal secMod %02x \n",SecModWd.get);
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
void GetRequestNormalList(AppLayerList_t *pAppLayer,uint8 *inData,uint16 inLen)
{
    uint16 offSet = 1;
    uint8 *apdu = pAppLayer->sendApduList->apdu+pAppLayer->sendApduList->offSet;
    //uint8 oadNum = 0;

    uint8 i;
    uint16 OI;
    SecModWd_t SecModWd;
    apdu[0] = inLen/4;
    memcpy(apdu+1,inData,apdu[0]*4);
    offSet += apdu[0]*4;
    pAppLayer->secMod = SecMod_NULL;
    for(i=0;i<apdu[0];i++)
    {
        OI = apdu[4*i+1];
        OI <<= 8;
        OI += apdu[4*i+2];
        SecModWd = GetMtrSecMod(pAppLayer->pipe,OI);
        if(SecModWd.get != SecMod_NULL)
            pAppLayer->secMod = SecMod_PlainMAC;
    }

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
void GetRequestRecord(AppLayerList_t *pAppLayer,uint8 *inData,uint16 inLen)
{
    uint16 offSet = 0;
    uint8 *apdu = pAppLayer->sendApduList->apdu+pAppLayer->sendApduList->offSet;
    GetRecord_t *rec = (GetRecord_t*)inData;
    printf("Enter GetRequsetRecord!!!\n");
    if(inLen!= sizeof(GetRecord_t))
    {
        printf("Err:inLen!= sizeof(GetRecord_t)");
        return;
    }
    if((pAppLayer->sendApduList->apduLen+rec->rcsdLen+rec->rsdLen+4)>sizeof(pAppLayer->sendApduList->apdu))
    {
        printf("Err:(pAppLayer->sendApduList->apduLen+rec->rcsdLen+rec->rsdLen+4)>sizeof(pAppLayer->sendApduList->apdu)");
        return;
    }
    
    apdu[offSet++] = (uint8)(rec[0].oad.nObjID >>8);
    apdu[offSet++] = (uint8)rec[0].oad.nObjID ;
    apdu[offSet++] = rec[0].oad.attID|(rec[0].oad.attPro<<5);
    apdu[offSet++] = rec[0].oad.nIndex;
    
    memcpy(apdu+offSet,rec[0].rsd,rec[0].rsdLen);
    offSet += rec[0].rsdLen;
    memcpy(apdu+offSet,rec[0].rcsd,rec[0].rcsdLen);
    offSet += rec[0].rcsdLen;

    pAppLayer->sendApduList->apduLen += offSet;
    pAppLayer->sendApduList->offSet += offSet;

    SecModWd_t SecModWd;
    //SecModWd = GeSecMod(rec[0].oad.OI);
    SecModWd = GetMtrSecMod(pAppLayer->pipe,rec[0].oad.nObjID);
    pAppLayer->secMod = SecModWd.get;
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
void GetRequestRecordList(AppLayerList_t *pAppLayer,uint8 *inData,uint16 inLen)
{
    uint16 offSet = 1;
    uint8 *apdu = pAppLayer->sendApduList->apdu+pAppLayer->sendApduList->offSet;
    GetRecord_t *rec = (GetRecord_t*)inData;
    uint8 recNum=0;
    while(inLen>=sizeof(GetRecord_t))
    {
        if(((uint32)pAppLayer->sendApduList->apduLen+offSet+4+rec[recNum].rcsdLen+rec[recNum].rsdLen)>sizeof(pAppLayer->sendApduList->apdu))
            break;
        apdu[offSet++] = (uint8)(rec[recNum].oad.nObjID >>8);
        apdu[offSet++] = (uint8)rec[recNum].oad.nObjID ;
        apdu[offSet++] = rec[recNum].oad.attID;
        apdu[offSet++] = rec[recNum].oad.nIndex;
        memcpy(apdu+offSet,rec[recNum].rsd,rec[recNum].rsdLen);
        offSet += rec[recNum].rsdLen;
        memcpy(apdu+offSet,rec[recNum].rcsd,rec[recNum].rcsdLen);
        offSet += rec[recNum].rcsdLen;
        recNum++;
    }
    apdu[0] = recNum;
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
void GetRequestNext(AppLayerList_t *pAppLayer,uint8 *inData,uint16 inLen)
{
    uint16 offSet = 0;
    uint8 *apdu = pAppLayer->sendApduList->apdu+pAppLayer->sendApduList->offSet;
    //uint16 *frameNo = (uint16*)inData;
    printf("App Layer Service:GetRequestNext\n");
    if(inLen!=2)
        return;
    //apdu[offSet++] = (uint8)(frameNo[0]>>8);
    //apdu[offSet++] = (uint8)frameNo[0];

    apdu[offSet++] = *inData;
    apdu[offSet++] = *(inData+1);

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
void GetRequest(uint8 pipe,OOP_TSA_T *sa,uint8 nChoice,uint8 *inData,uint16 inLen)
{
    
    LinkLayerAddr_t a;
    LinkLayerCtrl_t ctrl;
    AppLayerList_t* pAppLayer;
    GetRequestHead_t *pRequestHead;
    pAppLayer = FindAppLayer(pipe,NULL);
    memset(pAppLayer->sendApduList,0x00,sizeof(ApduList_t));//APDU初始化
    memset((uint8*)&a,0x00,sizeof(LinkLayerAddr_t));
    pAppLayer->sa = *sa;//保存服务器地址
    a.af_len = sa->len;
    //逻辑地址为1时也要填进去
    if(sa->oldvxd == 1)
    {
        a.af_logicAddr = 1;
    }
    #if (defined AREA_JIANGSU)
    a.af_logicAddr = sa->oldvxd|sa->flag;
    #endif
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
    pRequestHead = (GetRequestHead_t*)pAppLayer->sendApduList->apdu;
    //特殊处理 分钟级任务的时候 传进来的nChoice会在原来的基础上加MIN_TASK_CHOICE_ADD 此时直接明文抄
    uint8 realchoice;
    if(nChoice >= MIN_TASK_CHOICE_ADD)
        realchoice = nChoice - MIN_TASK_CHOICE_ADD;
    else
        realchoice = nChoice;
    
    if((realchoice==0)||(realchoice>5))
    {   
        return ;//服务不支持
    }   
    
    pRequestHead->id = SERVERID_GETREQUEST;
    pRequestHead->choice = realchoice;
    pRequestHead->piid = 0;
    pAppLayer->sendApduList->apduLen = 3;
    pAppLayer->sendApduList->offSet = 3;
    pAppLayer->secMod = SecMod_NULL;
    
    switch(realchoice)
    {
        case 1://Normal
            GetRequestNormal(pAppLayer,inData,inLen);
            break;
        case 2://NormalList
            GetRequestNormalList(pAppLayer,inData,inLen);
            break;
        case 3://Record
            GetRequestRecord(pAppLayer,inData,inLen);
            break;
        case 4://RecordList
            GetRequestRecordList(pAppLayer,inData,inLen);
            break;
        case 5://Next
            GetRequestNext(pAppLayer,inData,inLen);
            break;
        default:
            return;
    }
    
    if(pAppLayer->sendApduList->apduLen <= sizeof(GetRequestHead_t))
        return;
   
    //printf("sa_len=%d: ", a.af_len+1);
    //commfun_printstring( a.sa, a.af_len+1);
    //printf("send buf len=%d: ", pAppLayer->sendApduList->apduLen);
    //commfun_printstring( pAppLayer->sendApduList->apdu, pAppLayer->sendApduList->apduLen);
    //时间标签
    *(pAppLayer->sendApduList->apdu+pAppLayer->sendApduList->apduLen) = 0x00;
    pAppLayer->sendApduList->apduLen+=1;
    printf("secMod %02x \n",pAppLayer->secMod);
    //根据安全模式组安全传输帧
    //#if HARDWARE_TYPE != 9
    #if (defined AREA_JIANGSU)
    uint8 userType = taskmng_usertype_get_from_tsa(*sa,LOGIC_ID_YC);
    if(127 == userType)
    {
        pAppLayer->secMod = SecMod_NULL;
    }
    #endif
#ifdef AREA_HUNAN
    //现场用明文抄物联表返回认证失败，还是改为明文加随机数
    //湖南抄物联网表负荷数据用明文抄 物联网表全部用明文抄 
//     uint8 userType = taskmng_usertype_get_from_tsa(*sa,LOGIC_ID_YC);
//    if((sa->flag == 1 && sa->vxd == 3) || LOT_METER_SPRDT == userType || LOT_METER_TPIND == userType || LOT_METER_TPCHK == userType)
//    {
//        pAppLayer->secMod = SecMod_NULL;
//    }
#endif
#ifdef AREA_SHANDONG
    //山东抄4800用明文抄 谐波电压电流也用明文抄
    if(sa->flag == 1 && (sa->vxd == 3 || sa->vxd == 5))
    {
        pAppLayer->secMod = SecMod_NULL;
    }
#endif
    if(pAppLayer->secMod != SecMod_NULL && nChoice < MIN_TASK_CHOICE_ADD)
        SecurityRequest(pipe,pAppLayer->sendApduList->apdu,pAppLayer->sendApduList->apduLen);
    //#endif

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
eOopCommRslt_t GetConfirmNormal(AppLayerList_t *appLayerList)
{
    
    GetResponseNormalHead_t *recvApdu; 
    uint16 apduLen = appLayerList->recvApduList->apduLen -appLayerList->recvApduList->offSet ;
    uint16 offset = appLayerList->recvApduList->offSet;
    OOP_OAD_U oad ;
    if(apduLen < sizeof(GetResponseNormalHead_t))
    {
        //printf("APP Layer err =%d,apdulen =%d,headlen=%d\n",eOopComm_FrameNotIntergrity,apduLen,sizeof(GetResponseNormalHead_t));
        return eOopComm_FrameNotIntergrity;
    }

    
    recvApdu =  (GetResponseNormalHead_t *)appLayerList->recvApduList->apdu+offset;
    //if(0!=memcmp(sendApdu->oad,recvApdu->oad,4))
    //  return eOopComm_OADNotMatch;//发送的OAD必须与接收的OAD一致

    oad.nObjID = recvApdu->oad[0];
    oad.nObjID = (oad.nObjID<<8)+recvApdu->oad[1];
    oad.attID = recvApdu->oad[2];
    oad.nIndex = recvApdu->oad[3];
    appLayerList->recvApduList->offSet += sizeof(GetResponseNormalHead_t);
    //调用接口类解析函数
    if(appLayerList->recvApduList->apduLen-appLayerList->recvApduList->offSet < 2)//chioce(1)+dar(1)
    {
        printf("APP Layer err =%d,apdulen =%d,offset=%d ",eOopComm_FrameNotIntergrity,appLayerList->recvApduList->apduLen,appLayerList->recvApduList->offSet );
        return eOopComm_FrameNotIntergrity;
    }
    //printf("APP Layer apdu len=%d,offet =%d ",appLayerList->recvApduList->apduLen,appLayerList->recvApduList->offSet);
    //commfun_printstring( appLayerList->recvApduList->apdu, appLayerList->recvApduList->apduLen);
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
eOopCommRslt_t GetConfirmNormalList(AppLayerList_t *appLayerList)
{
    //GetRequestNormalListHead_t *sendApdu;
    //GetResponseNormalListHead_t *recvApdu; 
    uint16 apduLen = appLayerList->recvApduList->apduLen -appLayerList->recvApduList->offSet ;
    ///uint16 offset = appLayerList->recvApduList->offSet;
    //uint8 i;
    //OOP_OAD_U oad ;
    //eOopCommRslt_t rslt;
    if(apduLen < sizeof(GetResponseNormalListHead_t))
        return eOopComm_FrameNotIntergrity;
    //sendApdu = (GetRequestNormalListHead_t *)appLayerList->sendApduList->apdu;
    //recvApdu =  (GetResponseNormalListHead_t *)(appLayerList->recvApduList->apdu + offset);
    
    //if(sendApdu->oadNum != recvApdu->oadNum)
    //  return eOopComm_OADNotMatch;
    //appLayerList->recvApduList->offSet += sizeof(GetResponseNormalListHead_t);
    //for(i=0;i<recvApdu->oadNum;i++)
    //{
    //  rslt = GetConfirmNormal(appLayerList);
    //  if(eOopComm_Success != rslt)
    //      return rslt;
    //}
    //appLayerList->recvApduList->offSet += 1;
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
eOopCommRslt_t GetConfirmRecord(AppLayerList_t *appLayerList)
{
    
    GetResponseNormalHead_t *recvApdu; 
    uint16 apduLen = appLayerList->recvApduList->apduLen -appLayerList->recvApduList->offSet ;
    uint16 offset = appLayerList->recvApduList->offSet;
    OOP_OAD_U oad ;
    //TASK_FMT_TRACE( appLayerList->pipe, REC_LOG, "GetConfirmRecord apduLen %d offset %d \n",apduLen,offset);

    if(apduLen < sizeof(GetResponseNormalHead_t))
    {
        //printf("APP Layer err =%d,apdulen =%d,headlen=%d ",eOopComm_FrameNotIntergrity,apduLen,sizeof(GetResponseNormalHead_t));
        return eOopComm_FrameNotIntergrity;
    }

    
    recvApdu =  (GetResponseNormalHead_t *)appLayerList->recvApduList->apdu+offset;
    //if(0!=memcmp(sendApdu->oad,recvApdu->oad,4))
    //  return eOopComm_OADNotMatch;//发送的OAD必须与接收的OAD一致

    oad.nObjID = recvApdu->oad[0];
    oad.nObjID = (oad.nObjID<<8)+recvApdu->oad[1];
    oad.attID = recvApdu->oad[2];
    oad.nIndex = recvApdu->oad[3];
    appLayerList->recvApduList->offSet += sizeof(GetResponseNormalHead_t);
    //调用接口类解析函数
    if(appLayerList->recvApduList->apduLen-appLayerList->recvApduList->offSet < 2)//chioce(1)+dar(1)
    {
        printf("APP Layer err =%d,apdulen =%d,offset=%d ",eOopComm_FrameNotIntergrity,appLayerList->recvApduList->apduLen,appLayerList->recvApduList->offSet );
        return eOopComm_FrameNotIntergrity;
    }
    //printf("APP Layer apdu len=%d,offet =%d ",appLayerList->recvApduList->apduLen,appLayerList->recvApduList->offSet);
    //commfun_printstring( appLayerList->recvApduList->apdu, appLayerList->recvApduList->apduLen);
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
eOopCommRslt_t GetConfirmRecordList(AppLayerList_t *appLayerList)
{
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
eOopCommRslt_t GetConfirmNext(AppLayerList_t *appLayerList)
{
    
    GetResponseNextHead_t *recvApdu; 
    uint16 apduLen = appLayerList->recvApduList->apduLen -appLayerList->recvApduList->offSet ;
    uint16 offset = appLayerList->recvApduList->offSet;
    uint16 frameNo;
    printf("App Layer Service:GetConfirmNext\n");
    if(apduLen < sizeof(GetResponseNextHead_t))
        return eOopComm_FrameNotIntergrity;

    recvApdu =  (GetResponseNextHead_t *)(appLayerList->recvApduList->apdu+offset);
    frameNo = recvApdu->frameNo[0];
    frameNo = (frameNo<<8)+recvApdu->frameNo[1];
    //if(frameNo !=  (appLayerList->recvApduList->frameNo+1))
    //  return eOopComm_FrameNoNotMatch;
    appLayerList->recvApduList->frameNo++;
    appLayerList->recvApduList->offSet += 3;//sizeof(GetResponseNextHead_t);

    if(recvApdu->choice == 1)//Sequence of Normal
    {
        if(appLayerList->recvApduList->apdu[appLayerList->recvApduList->offSet++] == 1)//Normal
        {
            
        }
        else//NormalList
        {

        }
        //GetConfirmNormal(appLayerList);
    }
    else if(recvApdu->choice == 2)//record
    {
        if(appLayerList->recvApduList->apdu[appLayerList->recvApduList->offSet++] == 1)//Record 
        {
            
        }
        else//Record List
        {

        }
    }
    printf("recvApdu->lastFlg=%d\n",recvApdu->lastFlg);
    appLayerList->sendApduList->frameNo = appLayerList->recvApduList->frameNo;
    //GetRequest(appLayerList->pipe,&appLayerList->sa,5,(uint8*)&appLayerList->sendApduList->frameNo,2);
    appLayerList->recvApduList->offSet += 1+4; //offset到50020200后面
    if(recvApdu->lastFlg == 0)//不是最后一帧
    {
        //GetRequestNext(appLayerList,&appLayerList->sendApduList->frameNo,2);
        uint8 TmpBuff[2]={0x02,0x00};
        //TmpBuff[0] = (uint8)(appLayerList->sendApduList->frameNo>>8);
        //TmpBuff[1] = (uint8)appLayerList->sendApduList->frameNo;
        TmpBuff[0] = (uint8)(frameNo>>8);
        TmpBuff[1] = (uint8)(frameNo);
        GetRequest(appLayerList->pipe,&appLayerList->sa,0x05,TmpBuff,2);
        return eOopComm_GetNext;
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
eOopCommRslt_t GetConfirm(AppLayerList_t *appLayerList)
{
    printf("App Layer Service:GetConfirm\n");
    if(appLayerList->recvApduList->apduLen <= 3)
        return eOopComm_FrameNotIntergrity;
    //if((appLayerList->sendApduList->apdu[1]) !=appLayerList->recvApduList->apdu[1])
    //{//发送与接收的服务必须一致
    //  if(appLayerList->recvApduList->apdu[1]!=5)//除了NEXT服务
    //      return eOopComm_ServerIDNotMatch;
    //}
    appLayerList->recvApduList->offSet = 3;
    //TASK_FMT_TRACE( appLayerList->pipe, REC_LOG, "GetConfirm apdu[1]%d  \n",appLayerList->recvApduList->apdu[1]);

    switch(appLayerList->recvApduList->apdu[1])
    {
        case 1://Normal
             return(GetConfirmNormal(appLayerList));
        case 2://NormalList
            return(GetConfirmNormalList(appLayerList));
        case 3://Record
            return(GetConfirmRecord(appLayerList));
        case 4://RecordList
            return(GetConfirmRecordList(appLayerList));
        case 5://Next
            return(GetConfirmNext(appLayerList));
        default:
            return eOopComm_ServerIDNotMatch;
    }
}

