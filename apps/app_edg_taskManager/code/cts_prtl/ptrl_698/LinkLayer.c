/*-
****************************************************************************************
 * Copyright (c) 2004,南京新联电子仪器有限责任公司
 *            All rights reserved.
 * 程序名称：LinkLayer.c
 * 版本号：1.0
 * 功能：面向对象协议栈-链路层
 * 开发人：qxc
 * 开发时间：2016-06-30
 * 修改者:
 * 修改时间:
 * 修改简要说明:
 * 其他： 
*****************************************************************************************
-*/
#include "CtsAll.h"
//#include "../../include/CtsMain.h"
//#include "../../include/CommFun.h"

LinkLayer_t *LinkLayerHeadPtr = NULL;

/****************************************************************************
*模块编号：                                                                  
*名称：                                                                      
*功能：                                                                      
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
****************************************************************************/
void Client_Send_Apdu(uint8 pipe,LinkLayerAddr_t *a,uint8 ctrl,uint8 *apdu,uint16 apduLen)
{
	LinkLayer_t *LinkLayer;
    uint16 frameLen;
    LinkLayer = FindLinkLayer(pipe,NULL);
    LinkLayer->a = *a;
    LinkLayer->ctrl.ctrl= ctrl;
    //10时转加密任务应用连接建立不上 还在查
    LinkLayer->a.ca = 0x00;//固定为0x10
    //LinkLayer->a.af_logicAddr = 0;
    //LinkLayer->a.af_typ = 0;
    //printf("send buf len=%d: ", apdu);
    //commfun_printstring( apdu, apduLen);
    /*if(LinkLayer->timeOut > (time(NULL))-LinkLayer->lastTime)
    {//上一次数据传输还未完成
        
        free(LinkLayer->apdu);
        LinkLayer->apdu = NULL;
        LinkLayer->apduLen = 0;
        printf("Link:segment err: ");
        return;
    }*/
    if(!LinkLayer->ctrl.ctrl_seg)
    {//不分帧清除分帧状态
        LinkLayer->recvSegflg.Flag = 0;
        LinkLayer->sendSegflg.Flag = 0;
    }
    if(LinkLayer->apdu!=NULL)
    {
        free(LinkLayer->apdu);
        LinkLayer->apdu = NULL;
        LinkLayer->apduLen = 0;
    }
    LinkLayer->ctrl.ctrl_dir = 0;
    LinkLayer->lastTime = time(NULL);
    if(apduLen>(sizeof(LinkLayer->sendBuff)-LINK_FRAME_LEN))//需要进行分帧传输
    {
        ///printf("Link:segment first: ");
        LinkLayer->ctrl.ctrl_seg = 1;
        LinkLayer->apdu= (uint8*)malloc(apduLen);
        LinkLayer->apduLen = apduLen;
        LinkLayer->apduOffset = 0;
        LinkLayer->frameCnt = 1;
		//TASK_BUF_TRACE( LinkLayer->pipe, REC_LOG, LinkLayer->apdu, LinkLayer->apduLen);
        frameLen = LinkLayer_PackFrame(pipe,&LinkLayer->a,LinkLayer->apdu+LinkLayer->apduOffset,sizeof(LinkLayer->sendBuff)-19);
        LinkLayer->apduOffset += sizeof(LinkLayer->sendBuff)-LINK_FRAME_LEN;
    }
    else
    {
        //printf("Link:segment contiue: ");
        //TASK_BUF_TRACE( LinkLayer->pipe, REC_LOG, apdu, apduLen);
        frameLen = LinkLayer_PackFrame(pipe,&LinkLayer->a,apdu,apduLen);
    }
    LinkLayer->sendLen = frameLen;
    
    //通过物理通道传输
}

//将数组逆序  buf:首地址  len:数组长度
void ReverseBuf(uint8 *buf, uint16 len)
{
    uint16  i,j;
    uint8 temp; 
    
    if(len <= 0)
    {
        return;
    }
    
    for(i=0,j=len-1; i<j; ++i,--j)
    {  
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
    }
    return;
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
eOopCommRslt_t Client_Resv_Apdu(uint8 pipe,LinkLayerAddr_t *a,LinkLayerCtrl_t ctrl,uint8 *apdu,uint16 apduLen)
{
    SegFlag_t frameSegFlg;
    uint16 frameLen;
    uint16 sendApduLen;
    uint8 MtrNo[8];
	LinkLayer_t *LinkLayer;

	LinkLayer = FindLinkLayer(pipe,NULL);
    LinkLayer->lastTime = time(NULL);   
    //TASK_FMT_TRACE( pipe, REC_LOG, "Client_Resv_Apdu LinkLayerPIPE %d ctrl.ctrl_prm %d ctrl.ctrl_seg %d \n",LinkLayer->pipe,ctrl.ctrl_prm,ctrl.ctrl_seg);
    if(ctrl.ctrl_prm)//服务器响应
    {
        //检查应答的地址与下发的地址是否一致
        //if(Cmp_ServerAddr(a,&LinkLayer->a) == 0)
        //{
        //  printf("Link Layer err:server addr not match!");
        //  return eOopComm_AddrNotMatch;
        //}
        if(LinkLayer->pipe == pipe)
        {
            AppLayerList_t *appLayerList = FindAppLayer(LinkLayer->pipe, NULL);
            appLayerList->ctrl = ctrl;
            memset(&appLayerList->sa,0x00,sizeof(OOP_TSA_T));
            appLayerList->sa.len = a->af_len;
            if(a->af_logicAddr>>1 == 1)
            {
                appLayerList->sa.flag = 1;
                appLayerList->sa.vxd = a->sa[0];
                memcpy_r(appLayerList->sa.add, a->sa+1, a->af_len);
            }else
            {
                memcpy_r(appLayerList->sa.add, a->sa, a->af_len+1);
            }

            memset(MtrNo,0x00,8);//重新写一下表号，用通讯地址来写
            memcpy_r(MtrNo,appLayerList->sa.add,6);
            ReverseBuf(MtrNo, 8);
            Set_ReadMtrNum(pipe,MtrNo);
        }
        if(ctrl.ctrl_seg)//分帧服务
        {
            LinkLayer->ctrl.ctrl_seg = ctrl.ctrl_seg;
            frameSegFlg.Flag= apdu[1];
            frameSegFlg.Flag= (frameSegFlg.Flag<<8)+apdu[0];
            if(frameSegFlg.frameType == SEGFLAG_CONFRIM_FRAME)//分帧确认帧
            {
                if(apduLen!=2)//
                        return eOopComm_FrameNotIntergrity;//数据帧不完整
                if((frameSegFlg.frameNo!=LinkLayer->sendSegflg.frameNo )//帧序号不一致
                    ||(LinkLayer->sendSegflg.frameType == SEGFLAG_LAST_FRAME))//最后一帧已经发完了
                {//该如何处理?
                    //*apdu = 14;//块序号无效
                    //LinkLayer->segType = 0;//结果
                    //frameLen = LinkLayer_PackFrame(&LinkLayer->a,apdu,1);
                }
                else
                {
                    //LinkLayer->sendSegflg.confirm = 0;//分帧传输
                    LinkLayer->sendSegflg.frameNo++;
                    //LinkLayer->sendSegflg.lastFlg = 0;
                    //LinkLayer->segType = 1;//数据
                    sendApduLen = sizeof(LinkLayer->sendBuff)-LINK_FRAME_LEN;
                    if(LinkLayer->apduOffset+sendApduLen >= LinkLayer->apduLen)
                    {
                        LinkLayer->sendSegflg.frameType = SEGFLAG_LAST_FRAME;//最后一帧
                        sendApduLen = LinkLayer->apduLen-LinkLayer->apduOffset;
                    }
                    else
                    {
                        LinkLayer->sendSegflg.frameType = SEGFLAG_MID_FRAME;//中间帧
                    }
                    frameLen = LinkLayer_PackFrame(pipe,&LinkLayer->a,LinkLayer->apdu+LinkLayer->apduOffset,sendApduLen);
                    LinkLayer->apduOffset += sendApduLen;
                }
            }
            else//分帧请求帧
            {
                if(LinkLayer->recvSegflg.frameNo == 0)//首帧
                {
                    free(LinkLayer->apdu);
                    LinkLayer->apdu = NULL;
                    LinkLayer->apduLen  = 0;
                    LinkLayer->recvSegflg.Flag = 0;
                    //if(frameSegFlg.frameNo != 0)
                    //{
                    //  
                    //}
                }
                if(frameSegFlg.frameNo != (LinkLayer->recvSegflg.frameNo))
                {//帧序号不正确，链路层出错不应答
                    return eOopComm_FrameNotIntergrity;
                }
                else
                {
                    uint16 segFrameLen;//接收到分帧块长度
                    uint8 *pOldApdu;
                    
                    if(apduLen<=2)//
                        return eOopComm_FrameNotIntergrity;//数据帧不完整
                    segFrameLen = apduLen-2;
                    pOldApdu = LinkLayer->apdu;
                    if((uint32)segFrameLen+LinkLayer->apduLen> 0xFFFF)
                    {//应答错误
                        return eOopComm_AppCacheOver;//缓冲区移除
                    }
                    else
                    {
                         LinkLayer->apdu =  (uint8*)malloc( segFrameLen+LinkLayer->apduLen);
                         if(pOldApdu!=NULL)
                         {
                            memcpy(LinkLayer->apdu,pOldApdu,LinkLayer->apduLen);
                            free(pOldApdu);
                         }
                         memcpy(LinkLayer->apdu+LinkLayer->apduLen,apdu+2,segFrameLen);
                         LinkLayer->apduLen = segFrameLen+LinkLayer->apduLen;
                         LinkLayer->recvSegflg.frameNo = frameSegFlg.frameNo;
                         LinkLayer->recvSegflg.frameType = frameSegFlg.frameType;
                         if(LinkLayer->recvSegflg.frameType==SEGFLAG_LAST_FRAME)//最后一帧不需要确认
                         {
                            //调用应用层服务
                            TASK_FMT_DEBUG(0, REC_LOG,"收到所有分帧数据\n");
                            TASK_BUF_DEBUG(0, REC_LOG,LinkLayer->apdu,LinkLayer->apduLen);
                            return(AppLayerConfirm(LinkLayer->pipe,LinkLayer->apdu,LinkLayer->apduLen));
                         }
                         else
                         {
                            LinkLayer->sendSegflg.frameType = SEGFLAG_CONFRIM_FRAME;
                            LinkLayer->sendSegflg.frameNo = LinkLayer->recvSegflg.frameNo;
                            LinkLayer->recvSegflg.frameNo ++;
                            frameLen = LinkLayer_PackFrame(pipe,&LinkLayer->a,NULL,0);
                            LinkLayer->sendLen = frameLen;
                            return eOopComm_SegRecv;
                         }
                    }
                }
                
            }
        }
        else
        {//调用应用层服务
            return(AppLayerConfirm(LinkLayer->pipe,apdu,apduLen));
        }
    }
    else//服务器上报
    {//调用上报服务
        AppLayerList_t *appLayerList = FindAppLayer(LinkLayer->pipe, NULL);
        memset(&appLayerList->sa,0x00,sizeof(OOP_TSA_T));
        appLayerList->sa.len = a->af_len;
        if(a->af_logicAddr>>1 == 1)
        {
            appLayerList->sa.flag = 1;
            appLayerList->sa.vxd = a->sa[0];
            memcpy_r(appLayerList->sa.add, a->sa+1, a->af_len);
        }else
        {
            memcpy_r(appLayerList->sa.add, a->sa, a->af_len+1);
        }

        memset(MtrNo,0x00,8);//重新写一下表号，用通讯地址来写
        memcpy_r(MtrNo,appLayerList->sa.add,6);
        ReverseBuf(MtrNo, 8);
        Set_ReadMtrNum(pipe,MtrNo);
        return(AppLayerConfirm(LinkLayer->pipe,apdu,apduLen));
    }
    return(eOopComm_Success);
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
uint16 LinkLayer_PackFrame(uint8 pipe,LinkLayerAddr_t *a,uint8*apdu,uint16 apduLen)
{
	LinkLayer_t *LinkLayer;
    uint16 offset=0;
    
    uint16 headLen = 2+1+(1+(a->af_len+1)+1);

    uint16 frameLen = headLen+2+ apduLen+2;
    uint16 tmpCrc;

	LinkLayer = FindLinkLayer(pipe,NULL);
    if(LinkLayer->ctrl.ctrl_seg)//需要分帧
        frameLen += 2;
	
    //链路层数据
    LinkLayer->sendBuff[offset++] = 0x68;
    LinkLayer->sendBuff[offset++] = frameLen & 0x00FF;
    LinkLayer->sendBuff[offset++] = (frameLen & 0xFF00)>>8;
    LinkLayer->sendBuff[offset++] = LinkLayer->ctrl.ctrl;
    LinkLayer->sendBuff[offset++] = a->af;

    memcpy(LinkLayer->sendBuff+offset,a->sa,a->af_len+1);
    offset += a->af_len+1;
    
    LinkLayer->sendBuff[offset++] = a->ca;
    //commfun_printstring( (uint8*)&LinkLayer->sendBuff[1], headLen);
    tmpCrc = CRC16_Get((uint8*)&LinkLayer->sendBuff[1] ,headLen);
    
    LinkLayer->sendBuff[offset++] =  tmpCrc & 0x00FF;
    LinkLayer->sendBuff[offset++] =  (tmpCrc & 0xFF00)>>8;
    //用户数据
    if(LinkLayer->ctrl.ctrl_seg)//需要分帧
    {
        LinkLayer->sendBuff[offset++] =  (uint8)LinkLayer->sendSegflg.Flag;
        LinkLayer->sendBuff[offset++] =  (uint8)(LinkLayer->sendSegflg.Flag>>8);
        if(LinkLayer->sendSegflg.frameType == SEGFLAG_CONFRIM_FRAME)//客户端发送分帧请求
        {
            //LinkLayer->sendBuff[offset++] = (uint8)(apduLen>>8);
            //LinkLayer->sendBuff[offset++] = (uint8)apduLen;
            memcpy(LinkLayer->sendBuff+offset,apdu,apduLen);
            offset += apduLen;  
        }
    }
    else
    {
        memcpy(LinkLayer->sendBuff+offset,apdu,apduLen);
        offset += apduLen;
    }
    tmpCrc = CRC16_Get((uint8*)&LinkLayer->sendBuff[1] ,frameLen-2);
    //链路层数据
    LinkLayer->sendBuff[offset++] =  tmpCrc & 0x00FF;
    LinkLayer->sendBuff[offset++] =  (tmpCrc & 0xFF00)>>8;
    LinkLayer->sendBuff[offset++] = 0x16;
	//TASK_BUF_TRACE( LinkLayer->pipe, REC_LOG, LinkLayer->sendBuff, offset);
    return offset;
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
eOopCommRslt_t LinkDataReceive(uint8 pipe,const uint8 *Buffer, uint16 DataLength, OOP_TSA_T *MtrCommAddr)
{
    uint16  FrameLen=0;
    uint16  Offset=0;
    eOopCommRslt_t rslt;
	LinkLayer_t *LinkLayer;
    //如果收到的长度是0 就直接给个和解不出来不一样的错误码
    if(DataLength == 0)
        return eOopComm_Fail;
    LinkLayer = FindLinkLayer(pipe,NULL);
    if(DataLength > MAX_LINK_LENGTH)
        return eOopComm_FrameNotIntergrity;
    memcpy(LinkLayer->resvBuff, Buffer, DataLength);
    LinkLayer->resvLen = DataLength;
    FrameLen = GetFrame(LinkLayer->resvBuff, LinkLayer->resvLen, &Offset);
    if (FrameLen == 0) 
    {   
        TASK_FMT_DEBUG(pipe, REC_LOG,"GetFrame error resvLen %d \n",LinkLayer->resvLen);
        return eOopComm_FrameNotIntergrity;
    }
    rslt = LinkLayer_UnPackFrame(pipe,LinkLayer->resvBuff+Offset, FrameLen,MtrCommAddr);

    
    return(rslt);
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
uint16 GetFrame(const uint8 *Buffer, uint16 DataLength, uint16 *OffSet)
{
    uint16  FrameLength=0;
    uint16  ffcs=0;
    uint16  cfcs=0;
    uint8   HeadLength=0;
    uint16      offset = *OffSet;
    
    for (offset=0;offset<DataLength;offset++)
    {
        //framelen
        if (DataLength-offset<12)
        {
            *OffSet = offset;
            return 0;
        }
        //head
        if (0x68!=Buffer[offset])
        {
            continue;
        }
        //head fcs
        HeadLength=(Buffer[offset+4]&0x7)+4+3+2;
        if (DataLength-offset<HeadLength)
        {
            continue;
        }
        memcpy(&ffcs,Buffer+offset+HeadLength-2,2);
        cfcs=CRC16_Get(Buffer+offset+1,HeadLength-3);

        if (cfcs!=ffcs)
        {
            continue;
        }
        //framelen
        memcpy(&FrameLength,Buffer+offset+1,2);
        FrameLength+=2;

        //if (FrameLength>MAX_FRAME_LENGTH_LPDU)
        //{
        //  continue;
        //}
        if (DataLength-offset<FrameLength)
        {
            continue;
        }
        //tail 
        if (0x16!=Buffer[offset+FrameLength-1])
        {
            continue;
        }
        //fcs
        memcpy(&ffcs,Buffer+offset+FrameLength-3,2);
        cfcs=CRC16_Get(Buffer+offset+1,FrameLength-4);

        if (cfcs!=ffcs)
        {
            continue;
        }
        *OffSet = offset;
        return FrameLength;
    }
    *OffSet = offset;
    return 0;
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
eOopCommRslt_t LinkLayer_UnPackFrame(uint8 pipe,uint8 *inframe,uint16 inLen,OOP_TSA_T   *MtrCommAddr)
{
    uint16  offset=0;
    LinkLayerCtrl_t ctrl;//控制吗
    LinkLayerAddr_t a;
    memset(&a,0,sizeof(a));
    uint8 tmpaddr[6];
    //head len
    offset+=3;
    //crtl
    ctrl.ctrl=inframe[offset];
    offset+=1;
    //af
    a.af = inframe[offset];
    offset+=1;
    //sa tsa中改为大端
    memcpy(a.sa,&inframe[offset],a.af_len+1);
    if(MtrCommAddr !=NULL)
    {
        memcpy_r(tmpaddr,MtrCommAddr->add,6);
        //有扩展逻辑地址
        if(a.af_logicAddr>>1 == 1)
        {
            if(memcmp(tmpaddr,a.sa+1,a.af_len) !=0)
            {
                return eOopComm_AddrNotMatch;
            }
        }else
        {
            if(memcmp(tmpaddr,a.sa,a.af_len+1) !=0)
            {
                return eOopComm_AddrNotMatch;
            }
        }
    }
    offset+=a.af_len+1;
    //ca
    a.ca=inframe[offset];
    offset+=1;
    //head fcs
    offset+=2;
    if (offset+3>=inLen)
    {
        return eOopComm_FrameNotIntergrity;
    }
    //TASK_FMT_TRACE( pipe, REC_LOG, "LinkLayer_UnPackFrame  offset %d ctrl.ctrl_dir %d \n",offset,ctrl.ctrl_dir );
    if(ctrl.ctrl_dir)//由服务器发起的或回应
    {
        //printf("Link Layer Asyn Ok\n");
        return(Client_Resv_Apdu(pipe,&a,ctrl,inframe+offset,inLen-offset-3));
    }
    else//由客户端发起的或回应
    {
        //现场遇到电表回复的报文还是我发送的抄表报文 会存在把别的表的数据存到当前表的问题
        return eOopComm_DarOther;

    }
    return(eOopComm_Success);
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
BOOLEAN Cmp_ServerAddr(LinkLayerAddr_t *sa,LinkLayerAddr_t*da)
{//比较两个地址是否一致
    //uint8 saStart[32];
    //uint8 daStart[32];
    //uint8 i,saFlgA,daFlgA;
    if(sa->af_typ != da->af_typ)//地址类型不一致
        return FALSE;
    if((sa->ca !=0) && (da->ca !=0) )//原地址有效
    {
        if(sa->ca != da->ca )//原地址不一致
            return FALSE;
    }
    if(sa->af_len!=da->af_len)
        return FALSE;
    if(memcmp(sa->sa,da->sa,sa->af_len+1) == 0)
        return TRUE;
    //去掉F,A,其他非0~9地址无效
    /*for(i=0;i<=sa->af_len;i++)
    {
        printf(saStart+i*2,"%02X",sa->sa[i]);
    }
    for(i=0;i<=da->af_len;i++)
    {
        printf(daStart+i*2,"%02X",da->sa[i]);
    }
    saFlgA = 0;
    daFlgA = 0;
    if(saStart[0]=='F'&&daStart[0]=='F')//支持第一个字符等于F
        i = 1;
    else
        i = 0;
    for(;i<2*(sa->af_len+1);i++)
    {
        if((saStart[i]<'0'||saStart[i]>'9')&&saStart[i]!='A')
            return FALSE;
        if((daStart[i]<'0'||daStart[i]>'9')&&daStart[i]!='A')
            return FALSE;
        
        if(saStart[i]=='A')
            saFlgA = 1;
        else
        {
            if(saFlgA && saStart[i-1]!='A')
                return FALSE;
        }
        if(daStart[i]=='A')
            daFlgA = 1;
        else
        {
            if(daFlgA && daStart[i-1]!='A')
                return FALSE;
        }
        
        if(saStart[i]!=daStart[i])
        {
            if(saStart[i]!='A'&&daStart[i]!='A')
                return FALSE;
        }
    }*/
    return FALSE;
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
LinkLayer_t* FindLinkLayer(uint8 pipe,uint8 *sa)
{
    LinkLayer_t *linkLayerPtr = LinkLayerHeadPtr;
    LinkLayer_t *lastLinkLayerPtr = LinkLayerHeadPtr;
    while(linkLayerPtr != NULL)
    {
        if(linkLayerPtr->pipe == pipe)
        {
            if(sa!=NULL)
            {
                
            }
            return linkLayerPtr;
        }
        lastLinkLayerPtr = linkLayerPtr;
        linkLayerPtr = linkLayerPtr->next;
    }
    linkLayerPtr = malloc(sizeof(LinkLayer_t));
    memset(linkLayerPtr,0x00,sizeof(LinkLayer_t));
    linkLayerPtr->pipe = pipe;//根据物理通道号创建链路层
    linkLayerPtr->next = NULL;
    linkLayerPtr->apdu = NULL;
    linkLayerPtr->timeOut = 5000;
    linkLayerPtr->lastTime = time(NULL);
    if(lastLinkLayerPtr!=NULL)
        lastLinkLayerPtr->next = linkLayerPtr;
    else
        LinkLayerHeadPtr = linkLayerPtr;
    return linkLayerPtr;
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
void DelLinkLayer(uint8 pipe,uint8 *sa)
{
    LinkLayer_t *linkLayerPtr = LinkLayerHeadPtr;
    LinkLayer_t *lastLinkLayerPtr = LinkLayerHeadPtr;
    while(linkLayerPtr != NULL)
    {
        if(linkLayerPtr->pipe == pipe)
        {
            if(sa!=NULL)
            {
                
            }
            lastLinkLayerPtr = linkLayerPtr->next;
            if(linkLayerPtr->apdu !=NULL)
                free(linkLayerPtr->apdu);
            free(linkLayerPtr);
            if(lastLinkLayerPtr == LinkLayerHeadPtr)
                LinkLayerHeadPtr = NULL;
            return;
        }
        linkLayerPtr = lastLinkLayerPtr;
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
void LinkLayerTimeOutDeal(uint8 pipe)
{//链路层通信超时管理
    LinkLayer_t *pLinkLayer;
    pLinkLayer = FindLinkLayer(pipe,NULL);
    if(pLinkLayer->timeOut < (time(NULL))-pLinkLayer->lastTime)
    {
        DelLinkLayer(pipe,NULL);
    }
}
