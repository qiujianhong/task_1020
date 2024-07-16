/*-
****************************************************************************************
 * Copyright (c) 2004,南京新联电子仪器有限责任公司
 *            All rights reserved.
 * 程序名称：MetSTD.c
 * 版本号：1.0
 * 功能：提供面向对象协议表抄表接口
 * 开发人：qxc
 * 开发时间：2016-06-30
 * 修改者:
 * 修改时间:
 * 修改简要说明:
 * 其他：
*****************************************************************************************
-*/

 #include "../../include/CtsAll.h"


#define DEBUG_METOOP //版本稳定后去掉此项

#ifdef DEBUG_METOOP
#define METOOP_ENV  "metoop"
#endif
#define bEnv  (getenv(METOOP_ENV) != NULL)  //调试信息的环境变量

#define POINTDAY_BC_NUM             3 //日冻结数据补抄次数

extern uint8 ComNo[10];
extern uint8 total_col_flg[AMR_THREAD_NUM];
extern uint8 patch_policy_flg[AMR_THREAD_NUM];



/****************************************************************************
*函数名称：IsOopMetEventNeedSave
*功能：     判断面向对象电表全事件方案各事件是否需要存储
*输入参数：
*输出参数：
*返回：TRUE 表示该事件需要存储
       FALSE 表示该事件不需要存储
****************************************************************************/
BOOL IsOopMetEventNeedSave(uint8 pipe,OOP_OAD_U mainOad,CSD_t* csd,uint8 csdnum,uint8 *pBuf, uint16 rcvLen, BOOL IsInPeriod, CTS_RUN_TASK_T *RunTaskInfo, SCHEME_TASK_RUN_INFO *pCbTask,uint32 *EventSeq)
{
    int         i;
    OOP_OAD_U     FrzTmOad ;        //事件记录序号

    uint32      LastRecNum = 0,RecvRecNum = 0;              //上次和本次抄读事件记录序号
    uint8       RecNoBuf[4];
    uint8       needsave = 0;
    uint16      offset = 0,DataLen = 0;
    uint16 MeterIndex,pn;  //当前抄表电表索引


    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    FrzTmOad.value=0x20220200;
    for(i = 0; i < csdnum; i++)
    {
        if(offset > rcvLen)
            return FALSE;

        if(cmpOad(csd[i].oad,FrzTmOad) == TRUE)
        {
            break;
        }

        if(taskmng_oop_next_read_data_get(pBuf+offset,&DataLen,rcvLen-offset,NULL_DATA,FALSE) == FALSE)
            return FALSE;

        offset += DataLen;
    }
     //台体测试回的事件序号是NULL 先暂时处理下
	if(pBuf[offset] == DT_NULL)
	{

		TASK_FMT_TRACE( pipe, COMM_LOG, "事件序号返回空\n");
		return TRUE;
	}
    if(i >= csdnum || pBuf[offset++] != DT_DOUBLE_LONG_UNSIGNED)
        return FALSE;

    //本次回复事件序号
    memcpy(RecNoBuf,&pBuf[offset],4);
    memrev(RecNoBuf,4);
    memcpy(&RecvRecNum,RecNoBuf,4);

    *EventSeq=RecvRecNum;
    LastRecNum=GetTaskReportSeq( pipe, RunTaskInfo->TaskId,RunTaskInfo->logicid, mainOad, pn);
    TASK_FMT_TRACE( pipe, COMM_LOG, "读取文件内容 RecvRecNum is %d, LastRecNum is %d,pipe is %d,RunTaskInfo->TaskId is %d,mainOad is %x,pn is %d\n",RecvRecNum,LastRecNum, pipe, RunTaskInfo->TaskId, mainOad, pn);
    if(LastRecNum == 0)
    {
        if(IsInPeriod == TRUE)
           needsave = 1;            //事件第一次记录
    }
    else
    {
        if((LastRecNum !=RecvRecNum)||(IsInPeriod == TRUE))//现场的电表事件的恢复后事件序号可能不变化
        {
            needsave = 1;
        }
    }
    if(needsave ==1)
    {
        SetTaskReportSeq( pipe, RunTaskInfo->TaskId, RunTaskInfo->logicid,mainOad, pn,RecvRecNum);
        return TRUE;
    }
    return FALSE;
}

/****************************************************************************
*函数名称：Prepare_OOP_meter_read
*功能：  抄表前的准备工作：获取档案的波特率,获取随机数,以及任务的开始时间等处理
*输入参数：
*输出参数：
*返回：无

****************************************************************************/

void Prepare_OOP_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T   *RunTaskInfo,S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
{

    uint16 sn;
    uint16 MeterIndex;
    OOP_METER_BASIC_T MeterBasicInfo;

    MeterIndex=RunTaskInfo->MeterIndex;
    sn=pCbTask->MsInfo[MeterIndex].Sn;

    //读取测量点信息
    taskmng_mtr_basic_info_get(sn,RunTaskInfo->logicid,&MeterBasicInfo);
    MtrPara->MtrNum=sn;
   // MtrPara->Speed=get_meter_read_speed(MeterBasicInfo.baud);
   //速度直接用枚举值
    MtrPara->Speed=MeterBasicInfo.baud;

    memcpy_r(MtrPara->MetAddr,MeterBasicInfo.tsa.add,6);
    memcpy(&MtrPara->MtrCommAddr,&MeterBasicInfo.tsa,sizeof(OOP_TSA_T));
    memset(Comm_Info_Oop,0,sizeof(S_COMM_INFO_MET));

    //判断电表ESAM序列号是否存在
    Init_ReadMtr(pipe);
    Comm_Info_Oop->MetStat = CB_STATE_INIT;
    Oop_LoadPrifle_Init(&Comm_Info_Oop->cbLPPara,pCbTask,RunTaskInfo);

}
/****************************************************************************
*函数名称：Cb_State_Init_Oop
*功能：  面向对象规约485抄表通道的初始化,初始化成功,就走数据发送,
       初始化失败抄表结束
*输入参数：
*输出参数：
*返回：无

-           无
****************************************************************************/

void Cb_State_Init_Oop(S_COMM_INFO_MET *Comm_Info_Oop,uint8 pipe,cbMtrPara  *MtrPara)
{

//      if(TRUE==get_rs485_dev(pipe))
//      {
//
//        if(rs485_RS232_Init(&Rs485ComNo[pipe-2],MtrPara->Speed)!=0)
//        {
//            rs485_RS232_UnInit(&Rs485ComNo[pipe-2]);//先临时添加
//            TASK_FMT_TRACE( pipe, REC_LOG, "初始化失败,pipe is %d\n",pipe);
//            Comm_Info_Oop->MetStat = CB_STATE_FINISH;
//            return;
//        }
//
//      }
//      else
//      {
//
//        TASK_FMT_TRACE( pipe, REC_LOG, "没有获取到模组,pipe is %d\n",pipe);
//        Comm_Info_Oop->MetStat = CB_STATE_FINISH;
//        return;
//      }
      Comm_Info_Oop->MetStat = CB_STATE_SEND;
      return;
}

/****************************************************************************
*函数名称：Meter_Oop_Read_Item
*功能：  面向对象规约485抄表发送函数

*输入参数：
*输出参数：
*返回：无

-           无
****************************************************************************/

void Meter_Oop_Read_Item(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
{
    uint8 tmp_buff[MAX_LINK_LENGTH+4] = {0};
    uint8 CsdIndex;
    int tmp_len = 0;
    int ret;

    uint16 pn,tmpMsIndex,MeterIndex;

    OOP_OAD_U temoad;
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);

    //Cheeck_Cb_Pipe(pCbTask,Comm_Info_Oop,MtrPara,pipe,lstTime);
    ExecMeter485Relay(pipe,&Comm_Info_Oop->MetStat,pCbTask->Priority);

    if((Comm_Info_Oop->MetStat ==CB_STATE_INIT)||(Comm_Info_Oop->MetStat ==CB_STATE_RESTART)||(Comm_Info_Oop->MetStat ==CB_STATE_PAUSE))
    {
        return ;
    }
    CsdIndex=RunTaskInfo->CsdIndex;
    memset((uint8*)&temoad,0x00,sizeof(OOP_OAD_U));
    if(CsdIndex<pCbTask->CsdTask.RoadNum)
    {

        if(0 > InvokeGetRequstTest(pipe, &MtrPara->MtrCommAddr, &pCbTask->CsdTask.RoadTask[CsdIndex],Comm_Info_Oop,pCbTask,RunTaskInfo))//组面向对象协议帧
        {
            Oop_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo,pipe,temoad);
            return;
        }
        else
        {
            LinkLayer_t* pLinkLayer = FindLinkLayer(pipe,NULL);//获取协议帧缓冲区
            tmp_len = addCharDataByte(tmp_buff,pLinkLayer->sendBuff,(int)pLinkLayer->sendLen,4);

            ret = rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,MtrPara->Speed,PARITY_EVEN,tmp_buff, tmp_len);
            //ret=rs485_RS232_send(&Rs485ComNo[pipe-2],tmp_buff, tmp_len);
            if(ret !=0)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "测量点号 %d\n,发送结果 ret is %d\n",MtrPara->MtrNum,ret);
                Comm_Info_Oop->MetStat = CB_STATE_FINISH;
                taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_SENDFRMNUM,NULL);
                gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastSendTime=time(NULL);
                return;
            }

            TASK_FMT_TRACE( pipe, REC_LOG, "测量点号 %d ,报文发送 speed %d\n",MtrPara->MtrNum,MtrPara->Speed);
            TASK_BUF_TRACE( pipe, REC_LOG, tmp_buff, tmp_len);
            Comm_Info_Oop->MetStat = CB_STATE_RECV;         //发送成功，准备接收
            taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_SENDFRMNUM,NULL);
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastSendTime=time(NULL);
        }
    }
    else
    {
        Comm_Info_Oop->MetStat = CB_STATE_FINISH;
    }
    if(Comm_Info_Oop->MetStat != CB_STATE_RECV )//0xff  所有数据项已经抄完不再处于接收状态
    {
        Comm_Info_Oop->MetStat = CB_STATE_FINISH;       //默认为结束状态，只有在查找到合适的数据项后才转入接收状态，其他都算结束
    }
    return;
}
/****************************************************************************
*函数名称：Deal_Receive_Result_Oop
*功能：  对面向对象规约485抄表返回的数据的结果进行处理
        抄读成功就切换下一个数据项进行抄读;
        抄读失败会进行重试

*输入参数：
*输出参数：
*返回：无

-           无
****************************************************************************/

CB_STATE Deal_Receive_Result_Oop(S_COMM_INFO_MET *Comm_Info_Oop,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, BOOL actResult,uint8 pipe,OOP_OAD_U Suboad)
{
    uint16 pn,tmpMsIndex,MeterIndex;
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    if(actResult)                          //该帧交换正确
    {
        Oop_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo,pipe,Suboad);
        Comm_Info_Oop->ComFrame++;          //总帧数增加
        Comm_Info_Oop->ErrFrame = 0;        //失败帧数和错误帧数清零，准备重新计数
        Comm_Info_Oop->ErrTimes = 0;
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_RECVFRMNUM,NULL);//pCbTask->TaskMonitor.recvFrameNum ++;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt = 0;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = TRUE;
    }
    else                                            //该帧交换失败
    {
        if(Comm_Info_Oop->ErrTimes >= (patch_policy_flg[pipe] == 0 ? ERR_TIMES_MAX : 0))        //同一帧的失败次数
        {
            if(Comm_Info_Oop->ErrFrame >= ERR_FRAME_MAX)      //失败次数过多，不再继续，准备退出
            {
                gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = FALSE;
                gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt++;
                gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastFailTime=time(NULL);
                Comm_Info_Oop->ErrFrame = ERR_FRAME_MAX;
                Comm_Info_Oop->MetStat = CB_STATE_FINISH;
                return Comm_Info_Oop->MetStat;
            }
            else
            {
                Comm_Info_Oop->ErrFrame++;                        //失败帧数增加
                Comm_Info_Oop->ComFrame++;                        //总帧数增加

                Comm_Info_Oop->ErrTimes = 0;                         //重试次数清零

                Oop_ToNextReadItem(TRUE,Comm_Info_Oop,pCbTask,RunTaskInfo,pipe,Suboad);
            }

            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = FALSE;
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt++;
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastFailTime=time(NULL);
        }
        else
        {
            Comm_Info_Oop->ErrTimes++;                              //重发当前数据项

        }
    }
    Comm_Info_Oop->MetStat = CB_STATE_SEND;                    //转入发送流程
    return Comm_Info_Oop->MetStat;
}

/****************************************************************************
*函数名称：Meter_Oop_recvive_data
*功能：  接收面向对象规约485抄表返回的数据
       并对数据进行处理

*输入参数：
*输出参数：
*返回：无

****************************************************************************/

void Meter_Oop_recvive_data( SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
{
    BOOL    actResult = TRUE;       //对回码处理的结果

    int32 tmp_len = 0;
    eOopCommRslt_t ret;
    AppLayerList_t *pAppLayer;
    uint8 CsdIndex,SubOadIndex;
    uint8 tmp_buff[MAX_LINK_LENGTH+4] = {0};

    uint16 pn,MeterIndex,tmpMsIndex;
    uint16 apduDataLen = 0;
    OOP_OAD_U Suboad;
//    OOP_OAD_U Tempoad;

    SubOadIndex=RunTaskInfo->SubOadIndex;
    CsdIndex=RunTaskInfo->CsdIndex;
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);


    memset(Comm_Info_Oop->pRcvBuf, 0x00, sizeof(Comm_Info_Oop->pRcvBuf));

    Comm_Info_Oop->RcvLen = rs485_RS232_recv(pipe,Comm_Info_Oop->pRcvBuf, sizeof(Comm_Info_Oop->pRcvBuf));

    TASK_FMT_TRACE( pipe, REC_LOG, "测量点号 %d,Comm_Info_Oop->RcvLen is %d\n",MtrPara->MtrNum,Comm_Info_Oop->RcvLen);

    TASK_BUF_TRACE( pipe, REC_LOG, Comm_Info_Oop->pRcvBuf, Comm_Info_Oop->RcvLen);
    ret = LinkDataReceive(pipe,Comm_Info_Oop->pRcvBuf,Comm_Info_Oop->RcvLen,&MtrPara->MtrCommAddr);

    TASK_FMT_TRACE( pipe, REC_LOG, "解析结果 is %d\n",ret);
    gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastRecvTime=time(NULL);


    memset((uint8*)&Suboad,0x00,sizeof(OOP_OAD_U));
	Suboad=pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex];
    if((ret == eOopComm_Success)||(ret == eOopComm_GetNext))
    {
        pAppLayer = FindAppLayer(pipe, NULL);
        Comm_Info_Oop->RcvLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
        memcpy(Comm_Info_Oop->pRcvBuf,pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet,Comm_Info_Oop->RcvLen);

        if((pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x0000))//OAD 方式使用 getRequestNormal 应答时GetResponseNormal
        {
            //if(Comm_Info_Oop->pRcvBuf[0]==0x01)
            {
                //返回的结果是数据


//                 Suboad=pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex];
//                 Tempoad.value=bin2_int32u(pAppLayer->recvApduList->apdu+3);
//                 if(Tempoad.value==Suboad.value)
                 {
                     TASK_FMT_TRACE( pipe, REC_LOG, "测量点号 %d,apdu %d\n",MtrPara->MtrNum,pAppLayer->recvApduList->apduLen);

                    TASK_BUF_TRACE( pipe, REC_LOG, pAppLayer->recvApduList->apdu, pAppLayer->recvApduList->apduLen);
                    apduDataLen=Oop_AUTK_Recv_NormalList(pipe, Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, *MtrPara,NULL, pCbTask,RunTaskInfo, Comm_Info_Oop);
                 }
            }

        }
        else
        {
            if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
            {
                //实时数据转存
                if(Comm_Info_Oop->pRcvBuf[0]==0x01)
                {
                    //返回的结果是数据
                     apduDataLen=Oop_AUTK_Recv(pipe, pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],Comm_Info_Oop->pRcvBuf+1, (uint16)Comm_Info_Oop->RcvLen-1, *MtrPara,NULL, pCbTask,RunTaskInfo);
                }


            }
            else
            {
                    apduDataLen=Oop_AUTK_Recv_Profile(pipe, pCbTask->CsdTask.RoadTask[CsdIndex].MainOad,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, *MtrPara,pCbTask,RunTaskInfo);
            }
        }
        TASK_FMT_TRACE(pipe, REC_LOG, "apduDataLen %d Comm_Info_Oop->RcvLen  %d Comm_Info_Oop->pRcvBuf[1 + apduDataLen] %d\n",
                apduDataLen,Comm_Info_Oop->RcvLen ,Comm_Info_Oop->pRcvBuf[1 + apduDataLen]);
        if((Comm_Info_Oop->pRcvBuf[1 + apduDataLen] == 1) && (apduDataLen != 0))
        {

            OOPRS485FollowReport(pipe, pn, &Comm_Info_Oop->pRcvBuf[1 + apduDataLen],
                 Comm_Info_Oop->RcvLen - 1 - apduDataLen);
        }
        if(ret == eOopComm_GetNext)
        {
            LinkLayer_t* pLinkLayer = FindLinkLayer(pipe,NULL);//获取协议帧缓冲区

            tmp_len = addCharDataByte(tmp_buff,pLinkLayer->sendBuff,(int32)pLinkLayer->sendLen,4);

            rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,MtrPara->Speed,PARITY_EVEN,tmp_buff, tmp_len);
            taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_RECVFRMNUM,NULL);
            return;
        }
    }
    else if(ret == eOopComm_SegRecv)
    {
        //协议栈分帧接收中,发送分帧确认帧后继续接收数据
        LinkLayer_t* pLinkLayer = FindLinkLayer(pipe,NULL);//获取协议帧缓冲区
        tmp_len = addCharDataByte(tmp_buff,pLinkLayer->sendBuff,(int)pLinkLayer->sendLen,4);

        rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,MtrPara->Speed,PARITY_EVEN,tmp_buff, tmp_len);
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_RECVFRMNUM,NULL);
        return;
   }
    else if(ret >=eOopComm_Fail)
    {
        actResult = FALSE;              //电表没有回码 或者 回码格式非法
    }
    else if(ret ==eOopComm_SecurityNotMatch)
    {
        actResult = FALSE;              //ESMA验证错误,只抄一次
        Comm_Info_Oop->ErrTimes = ERR_TIMES_MAX;
        Comm_Info_Oop->ErrFrame = ERR_FRAME_MAX;
    }
    Deal_Receive_Result_Oop(Comm_Info_Oop,pCbTask,RunTaskInfo,actResult,pipe,Suboad);

}

/*******************************************************************************
* 函数名称: OopEventCntReset
* 函数功能: 防止面向对象电表事件个数由0变为1时终端不存储事件
* 输入参数:
* 输出参数:
* 返 回 值: 无
*******************************************************************************/
void OopEventCntReset(OOP_OAD_U mainOad,cbMtrPara mCbMtrPara,uint8 pipe)
{

    int         ret = -1;
    uint32      RecNum = 0;                                 //上次和本次抄读事件记录序号
    uint32       len;
    NOMAL_OAD_T NormalOad ;
    OOP_OAD_U     temoad;
    if(FALSE == FindEvtRecNoIDByOad(mainOad))
    {
        return;
    }
    memset((uint8*)&temoad,0x00,sizeof(OOP_OAD_U));
    if(FALSE==GetEventRecordNo(mainOad,&temoad))
    {
        return;

    }
    NormalOad.logicId = 0;
    NormalOad.infoNum = mCbMtrPara.MtrNum;
    NormalOad.oad = temoad;//根据具体的事件来写事件次数
    ret = db_read_nomal(gAmrThreadArg[pipe].DBClientHandle, &NormalOad, 4, (uint8*)&RecNum, &len);

    if(ret !=0 || RecNum == 0xFFFFFFFF)
    {
        RecNum = 0;

        ret = db_write_nomal(gAmrThreadArg[pipe].DBClientHandle, &NormalOad, (uint8*)&RecNum,4);

    }

    return;
}

/****************************************************************************
*函数名称：InvokeGetRequstTest
*功能：  面向对象规约485抄表组发送帧函数

*输入参数：
*输出参数：
*返回：无
****************************************************************************/

int InvokeGetRequstTest(uint8 pipe,OOP_TSA_T *sa,CSD_TASK_INFO  *csd, S_COMM_INFO_MET *pCommInfo,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo )
{
    uint8 rsd[30];
    uint8 rsdLen=0;
    uint8 csdNum;
    uint16 CsdIndex;
    uint16 *SubOadIndex;
    uint16 oadindex=0;

    uint16 offset;
    uint16 task_id;

    GetRecord_t getRecord;
    cbLPPara_t *pCbLPPara;
    OadInfoMap  pOadInfo;
    RCSD_t rcsd;
    OOP_OAD_U subOAD;
    uint8 i,j;
    uint8 OadBuf[255];
    uint16 tmpoffset=0;

    task_id=pCbTask->TaskId;
    SubOadIndex=&RunTaskInfo->SubOadIndex;

    CsdIndex=RunTaskInfo->CsdIndex;

    if(csd->MainOad.nObjID==0x0000)//OAD
    {

        if(*SubOadIndex < csd->SubOadNum)//全部抄完了
        {
            pCommInfo->sendnum = 0;
            if((*SubOadIndex+Rs485ReadNum)<csd->SubOadNum)
            {
                for(j=0;j<Rs485ReadNum;j++)
                {
                    subOAD = csd->OadListInfo[*SubOadIndex+j];
                    if(FALSE==taskmng_oad_info_get(subOAD,&pOadInfo))
                    {

                      TASK_FMT_TRACE ( pipe,REC_LOG, "OAD不支持subOAD.value is %x\n",subOAD.value);
                       continue;
                    }
                    memcpy_r((uint8*)&OadBuf[tmpoffset],(uint8*)&csd->OadListInfo[*SubOadIndex+j].value,4);
                    tmpoffset =tmpoffset+4;
                }
                pCommInfo->sendnum = Rs485ReadNum;
            }
            else
            {
                oadindex = *SubOadIndex;
                while(oadindex<csd->SubOadNum)
                {
                    subOAD = csd->OadListInfo[oadindex];
                    if(FALSE==taskmng_oad_info_get(subOAD,&pOadInfo))
                    {
                       TASK_FMT_TRACE ( pipe,REC_LOG, "OAD不支持subOAD.value is %x\n",subOAD.value);
                       pCommInfo->sendnum++;
                       oadindex++;
                       continue;
                    }
                    memcpy_r((uint8*)&OadBuf[tmpoffset],(uint8*)&csd->OadListInfo[oadindex].value,4);
                    tmpoffset =tmpoffset+4;
                    pCommInfo->sendnum++;
                    oadindex++;
                }

            }
            OopGet.requst(pipe,sa,2,(uint8*)OadBuf, tmpoffset);
            return 0;
        }
        return -1;
    }
    else//ROAD
    {

        if(FALSE==taskmng_oad_info_get(csd->MainOad,&pOadInfo))
        {
            return -1;
        }

        pCbLPPara = &pCommInfo->cbLPPara;
        OOP_OAD_U FrzTimeOad;
        memset((uint8*)&FrzTimeOad,0x00,sizeof(OOP_OAD_U));
        if(pOadInfo.class==9)//冻结类
        {
            FrzTimeOad.value = 0x20210200;//数据冻结时间
        }
        else if(pOadInfo.class==7 || pOadInfo.class==24)   //事件类
        {

            FrzTimeOad.value = 0x20220200;//事件记录序号
        }
        else
        {
            FrzTimeOad.value = 0x201E0200;//事件发生时间
        }
		#ifdef AREA_SHANDONG
        if((csd->MainOad.nObjID == 0x5002)||(csd->MainOad.nObjID == 0x5003))
        {

            if(((taskmng_energy_oad_check(csd->OadListInfo[*SubOadIndex])==FALSE)&&(csd->OadListInfo[*SubOadIndex].value !=0x20210200))||(pCbTask->energynum==0x00))
            {
                //AcqInfo.Acqtype = ACQTYP_NULL;
                pCbTask->StoreTimeType = STRTMTYP_STARTTM;
                pCbTask->AcqInfo.Acqtype = ACQTYP_NULL;
            }

        }
		#endif
        if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)//通过实时数据转存
        {

            if((csd->MainOad.nObjID & 0xF000) != 0x5000)
            {
                //只有冻结支持通过实时数据转存
                RunTaskInfo->SubOadIndex = csd->SubOadNum;//全部抄完了
                return -1;
            }
            pCbLPPara->startTime = pCbLPPara->endTime;
            while(*SubOadIndex < csd->SubOadNum)//全部抄完了
            {

                subOAD = csd->OadListInfo[*SubOadIndex];
                if(FALSE==taskmng_oad_info_get(csd->MainOad,&pOadInfo)||(subOAD.value == 0x20210200))//从OAD不支持，转到下一个从OAD
                {
                    (*SubOadIndex)++;
                    continue;
                }
                OopGet.requst(pipe,sa,1,(uint8*)&subOAD, 4);
                return 0;
            }
            return -1;
        }
        else if(pCbTask->AcqInfo.Acqtype == ACQTYP_LASTN)
        {
            rsd[rsdLen++] = 9;//上N次,如果方案配置的是采集上几次记录的话，用RSD中的方法9
            rsd[rsdLen++] = pCbTask->AcqInfo.LastN;
            pCbLPPara->startTime = pCbLPPara->endTime;
        }
        else
        {
            OOP_TI_T    ti;
            OOP_TI_T    delayTi;
            if(pCbTask->AcqInfo.Acqtype != ACQTYP_TI)//如果配置的为按照冻结时标采集默认间隔为15分钟
            {

                if(0x5002 == pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID)
                {
                    //时标间隔为任务频率
                    get_task_run_frequence(task_id,&ti);
                    if((ti.unit >= TASK_CYCLE_FLAG_HOUR)
                        ||((ti.unit == TASK_CYCLE_FLAG_MIN)&&(ti.value > 15)))
                    {
                        //任务频率大于15分钟，时标间隔设为15分钟
                        ti.unit = TASK_CYCLE_FLAG_MIN;
                        ti.value = 15;
                    }
                }
                else if(0x5003 == pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID)
                {
                    ti.unit = TASK_CYCLE_FLAG_HOUR;//小时
                    ti.value = 1;//默认1小时
                }
                else
                {
                    ti.unit = 0;
                    ti.value = 0;
                }
            }
            else
            {
                ti.unit = pCbTask->AcqInfo.TI.unit;
                ti.value = pCbTask->AcqInfo.TI.value;
            }
            rsd[rsdLen++] = 2;//起始时间+结束时间+时间间隔RSD中的方法2
            DateTimeBcd_t tmpTm;
            rsd[rsdLen++] = (uint8)(FrzTimeOad.nObjID>>8);
            rsd[rsdLen++] = (uint8)(FrzTimeOad.nObjID);
            rsd[rsdLen++] = 0x02;
            rsd[rsdLen++] = 0x00;
            rsd[rsdLen++] = DATE_TIME_S;
            time_t tmpTime;
            delayTi.value = pCbTask->DelayTime;
            delayTi.unit = pCbTask->DelayTimeUnit;
            if((csd->MainOad.nObjID !=0x5002)&&(csd->MainOad.nObjID !=0x5003))
            {



                memcpy_r((uint8*)&tmpTm,(uint8*)RunTaskInfo->LastTurnBeginTime,7);
                DT_DateTimeBCD2Time(&tmpTm, &tmpTime);
                if((csd->MainOad.nObjID == 0x5004) && (pCbTask->AcqInfo.Acqtype == ACQTYP_BC
                    #if OOP_DAY_ADDREAD_485 == 1//采用日任务补抄起始时间
                    || (pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM) || (pCbTask->AcqInfo.Acqtype == ACQTYP_TI)
                    #endif
                    ))
                {
                    tmpTime = pCbLPPara->startTime;
                    DT_TimeSubInv(&tmpTime,pCbLPPara->tmInv);
                }
                tmpTime = tmpTime+60;
                DT_Time2DateTimeHex(tmpTime,(DateTimeHex_t*)(rsd+rsdLen));//开始时间

                rsdLen += 7;
                rsd[rsdLen++] = DATE_TIME_S;

                memcpy_r((uint8*)&tmpTm,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);
                DT_DateTimeBCD2Time(&tmpTm,&tmpTime);
                DT_TimeSubInv(&tmpTime, delayTi);       //减去任务开始时间里包含的延时，这是抄表和任务实际启动时间无关
                if((csd->MainOad.nObjID == 0x5004) && (pCbTask->AcqInfo.Acqtype == ACQTYP_BC
                    #if OOP_DAY_ADDREAD_485 == 1//采用日任务补抄起始时间
                    || (pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM) || (pCbTask->AcqInfo.Acqtype == ACQTYP_TI)
                    #endif
                    ))
                {
                    tmpTime = pCbLPPara->startTime;
                    //DT_TimeAddInv(&tmpTime,pCbLPPara->tmInv);
                }

                tmpTime += 60;
                DT_Time2DateTimeHex(tmpTime,(DateTimeHex_t*)(rsd+rsdLen));//结束时间
                rsdLen += 7;
            }
            else
            {
                DT_Time2DateTimeHex(pCbLPPara->startTime,(DateTimeHex_t*)(rsd+rsdLen));//方法2中的开始时间

                rsdLen += 7;
                rsd[rsdLen++] = DATE_TIME_S;
                if(ti.value==0)
                {
                    tmpTime = pCbLPPara->endTime;
                }
                else
                {
                    tmpTime = pCbLPPara->startTime ;
                    DT_TimeAddInv(&tmpTime,pCbLPPara->tmInv);
                    time_t  endTimeVal;
                    // 加上延时时间作为开始筛选的时间
                    endTimeVal = pCbLPPara->endTime;

                    DT_TimeSubInv(&endTimeVal, delayTi);        //减去任务开始时间里包含的延时，这是抄表和任务实际启动时间无关

                    if (tmpTime > endTimeVal)
                    {
                        tmpTime = endTimeVal;
                    }
                    if (tmpTime == pCbLPPara->startTime)
                      tmpTime++;    //oop表曲线抄表结束时间增加1s，至少抄一个点
                }
                DT_Time2DateTimeHex(tmpTime,(DateTimeHex_t*)(rsd+rsdLen));//方法2中的结束时间
                rsdLen += 7;
            }
            rsd[rsdLen++] = eTI;//方法2中的数据间隔
            if(csd->MainOad.nObjID == 0x5004)
                ti.unit = TASK_CYCLE_FLAG_DAY;
            else if(csd->MainOad.nObjID == 0x5005)
                ti.unit = TASK_CYCLE_FLAG_DAY;
            else if(csd->MainOad.nObjID == 0x5006)
                ti.unit = TASK_CYCLE_FLAG_MON;
            else if(csd->MainOad.nObjID == 0x5002)
            {
                if(ti.unit == TASK_CYCLE_FLAG_HOUR)
                {
                    ti.unit = TASK_CYCLE_FLAG_MIN;
                    ti.value *= 60;
                }
                else
                    ti.unit = TASK_CYCLE_FLAG_MIN;
            }
            rsd[rsdLen++] = ti.unit;//间隔单位
            rsd[rsdLen++] = (uint8)(ti.value>>8);//2个字节的long_unsignedl类型的间隔
            rsd[rsdLen++] = (uint8)ti.value;
            //rsdLen = 18;
        }
        csdNum =csd->SubOadNum;
        rcsd.csdNum = csdNum + 1;
        getRecord.rcsdLen = (uint16)rcsd.csdNum*(4+1)+1;
        getRecord.rcsd = malloc(getRecord.rcsdLen);

        offset = 1;

        //if(pOadInfo->classId==9)
        {
            //第一列加入数据冻结时间
            getRecord.rcsd[offset++] = 0;//0表示是OAD
            memcpy_r((uint8*)&getRecord.rcsd[offset],(uint8*)&FrzTimeOad.value,4);
            offset =offset +4;

        }
        i=0;
        #ifdef AREA_SHANDONG
         if((csd->MainOad.nObjID ==0x5002)||(csd->MainOad.nObjID ==0x5003))
         {
            csdNum = pCbTask->energynum;
         }
        #endif
        while(i<csdNum)
        {
            //第一列中已加入数据冻结时间OAD，后面不再增加
            if(cmpOad(csd->OadListInfo[i],FrzTimeOad)==FALSE)
            {
                getRecord.rcsd[offset++] = 0;
                memcpy_r((uint8*)&getRecord.rcsd[offset],(uint8*)&csd->OadListInfo[i].value,4);
                offset =offset +4;
            }
            i++;
        }
        getRecord.rcsd[0] = (offset-1)/5;//表示有多少个CSD;
        getRecord.rcsdLen = offset;
        getRecord.oad = csd->MainOad;
        getRecord.rsdLen = rsdLen;
        getRecord.rsd = rsd;

        OopGet.requst(pipe,sa,3,(uint8*)&getRecord,sizeof(GetRecord_t) );
        free(getRecord.rcsd);
    }
    return 0;
}
/****************************************************************************
*函数名称：Compare_Buffer
*功能：  比较两个buffer内容是否完全一致

*输入参数：
*输出参数：
*返回：TRUE 两个buffer完全一样
       FALSE 两个buffer不完全一样

****************************************************************************/


BOOL Compare_Buffer(uint8 *buf1 ,uint8 *buf2,uint32 buflen1 ,uint32 buflen2)
{
    uint32 i=0;
    if(buflen2==0)
    {
        return FALSE;
    }
    if(buflen1 !=buflen2)
    {
        return FALSE;
    }
    for(i=0;i<buflen2;i++)
    {
        if(buf1[i]!=buf2[i])
        {
            return FALSE;
        }
    }
    return TRUE;
}


/****************************************************************************
*函数名称：Oop_AUTK_Recv
*功能：  面向对象规约485抄表返回数据按照0AD进行存储

*输入参数：
*输出参数：
*返回：无
****************************************************************************/

uint16 Oop_AUTK_Recv_NormalList(uint8 pipe,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop)
{
    uint8 Oadnum=0;
    uint8 i=0;
    uint16 offset=0;
    OOP_OAD_U oad;
    uint16 rslt;
    uint8 result=0;
    DATA_CACHE_T meterData;
    uint16 pn;
    uint16 MeterIndex;

    memset((uint8*)&meterData,0x00,sizeof(meterData));
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    Oadnum = pBuf[offset++];

    for(i=0;i<Oadnum;i++)
    {
        Comm_Info_Oop->SucOADNum++;
        memcpy_r((uint8*)&oad.value,pBuf+offset,4);
        TASK_FMT_TRACE(pipe, REC_LOG,"Oop_AUTK_Recv_NormalList i %d oad %08x \n",i,oad.value);
        offset =offset+4;
        result = pBuf[offset++];//是否有数据
        if(0==result)
        {
            offset++;//DAR错误码
            continue;
        }
        //需要判断OAD是否在抄表CSD中，判断返回的报文中是否有相应的CSD并且存储;
        rslt = Oop_Autk_Save_Buf(pipe, oad,pBuf+offset, rcvLen-offset, mCbMtrPara, NULL,pCbTask,RunTaskInfo,&meterData);

        if(rslt == 0)
        {
            taskmng_oop_next_read_data_get(pBuf,&offset,rcvLen,NULL_DATA,FALSE);
        }
        else
        {

            offset+=rslt;
        }
    }
    taskmng_rs485_cache_buf_write_to_db(pipe,pn,LOGIC_ID_YC,&meterData);
    return offset;
}


/****************************************************************************
*函数名称：Oop_AUTK_Recv
*功能：  面向对象规约485抄表返回数据按照0AD进行存储

*输入参数：
*输出参数：
*返回：无
****************************************************************************/

uint16 Oop_AUTK_Recv(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{
    uint16 offSet=0;
    time_t longTm;
    time_t colStartTm;

    uint16 pn;
    uint16 MsIndex;
    OadStoreInfo StoreInfo;
    uint16 CsdIndex;
    uint8 MtrAddr[6];
    uint8 desbuffer[2000];
    uint16 MeterIndex;
    uint16 deslength=0;
    OadInfoMap pOadInfo;
    DateTimeBcd_t taskStoreTime;
    DateTimeBcd_t taskStartTime;
    uint8 pwrType;


    CsdIndex=RunTaskInfo->CsdIndex;
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);


    //根据OAD查找OAD属性描述

    if(FALSE==taskmng_oad_info_get(oad, &pOadInfo))
    {
        return 0;
    }
    //保存采集到数据

    if(pBuf[0]==0x00)
    {

        if((pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID&0xF000) == 0x5000)
        {
            if(oad.value==0x20210200)
            {
                //clr_bit_value(RunTaskInfo->MtrResult,128,MsIndex);
                taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
            }
        }

        return 1;
    }
    pwrType=taskmng_mtr_conntype_get(pn,RunTaskInfo->logicid);
    Convert_Oop_Data_To_Struct(pBuf,&offSet,rcvLen,desbuffer,&pOadInfo,&deslength,pwrType);

    //memcpy_r((uint8*)&taskStartTime,(uint8*)RunTaskInfo->ThisTurnRealBeginTime,7);//采集启动时间n

    memcpy_r((uint8*)&taskStartTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);//采集启动时间
    DT_DateTimeBCD2Time(&taskStartTime,&colStartTm);

    if(StoreTime==NULL)
    {
        time_t storeTm;
        OOP_TI_T delayTi;
        DT_DateTimeBCD2Time(&taskStartTime,&storeTm);
        delayTi.unit = pCbTask->DelayTimeUnit;
        delayTi.value = pCbTask->DelayTime;
        DT_TimeSubInv(&storeTm, delayTi);
        taskStoreTime = taskmng_acq_store_time_get(pCbTask->StoreTimeType,storeTm);
        DT_DateTimeBCD2Time(&taskStoreTime,&longTm);
        if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5004 || pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5006
            ||pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5002||pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5005)// 日月曲线数据

            {
                //set_bit_value(RunTaskInfo->MtrResult,128,MsIndex);//不需要抄读
                taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
                taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
            }

    }
    else
    {
        if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5004 || pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5006
            ||pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5002||pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5005)// 日月曲线数据
        {

            if(oad.nObjID == 0x2021)
            {
                if(StoreTime->sec == 0xFF)//林洋表回日冻结时间秒数可能为0xFF
                {
                    StoreTime->sec = 0;
                }
                if(DT_DateTimeBCD2Time(StoreTime,&longTm)==FALSE)//应答的时间不合法
                {
                    //不存储
                    return 0;
                }
                //检测是否需要补抄
            IsNeedBcMeterOopFrzDayData(pCbTask,pBuf,RunTaskInfo,pipe);
            //set_bit_value(RunTaskInfo->MtrResult,128,MsIndex);
            taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
            taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);

            }
            DT_DateTimeBCD2Time(StoreTime,&longTm);

        }
       else
        {
            DT_DateTimeBCD2Time(StoreTime,&longTm);
        }
    }
    taskmng_mtr_addr_get(pn,RunTaskInfo->logicid,MtrAddr);
    memcpy(StoreInfo.MAC.nValue,MtrAddr,6);
    StoreInfo.recordOAD.infoNum=pn;
    StoreInfo.recordOAD.logicId=RunTaskInfo->logicid;
    if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x0000)
    {

        StoreInfo.recordOAD.road.oadMain.value= 0x00;
        StoreInfo.recordOAD.road.oadCols.nNum=1;
        StoreInfo.recordOAD.road.oadCols.oad[0]=oad;
        StoreInfo.recordOAD.road.oadCols.rsv=0;
    }
    else
    {
        StoreInfo.recordOAD.road.oadMain=pCbTask->CsdTask.RoadTask[CsdIndex].MainOad;
        StoreInfo.recordOAD.road.oadCols.nNum=1;
        StoreInfo.recordOAD.road.oadCols.oad[0]=oad;
        StoreInfo.recordOAD.road.oadCols.rsv=0;
    }
    StoreInfo.recordOAD.classtag = 5;
    StoreInfo.colStartTm=colStartTm;
    StoreInfo.colStoreTm=longTm;
    StoreInfo.colEndTm=time(NULL);
    //TASK_FMT_TRACE( pipe,COMM_LOG, "当前任务号:pCbTask->TaskId is %d\n",pCbTask->TaskId);

    taskmng_meter_data_save(desbuffer,deslength,StoreInfo,pipe);


    return offSet;


}



/****************************************************************************
*函数名称：Oop_AUTK_Recv
*功能：  面向对象规约485抄表返回数据按照0AD进行存储

*输入参数：
*输出参数：
*返回：无
****************************************************************************/

uint16 Oop_Autk_Save_Buf(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,DATA_CACHE_T *meterData)
{
    uint16 offSet=0;
    time_t longTm;
    time_t colStartTm;

    uint16 pn;
    uint16 MsIndex;
    uint16 CsdIndex;
    uint8 desbuffer[2000];
    uint16 MeterIndex;
    uint16 deslength=0;
    OadInfoMap pOadInfo;
    DateTimeBcd_t taskStoreTime;
    DateTimeBcd_t taskStartTime;
    uint8 pwrType;


    CsdIndex=RunTaskInfo->CsdIndex;
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);


    //根据OAD查找OAD属性描述

    if(FALSE==taskmng_oad_info_get(oad, &pOadInfo))
    {
        TASK_FMT_TRACE(pipe, REC_LOG, "find oad fail,oad is %0x8\n", oad.value);
        return 0;
    }
    //保存采集到数据

    if(pBuf[0]==0x00)
    {

        if((pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID&0xF000) == 0x5000)
        {
            if(oad.value==0x20210200)
            {
                //clr_bit_value(RunTaskInfo->MtrResult,128,MsIndex);
                taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
            }
        }

        return 1;
    }
    pwrType=taskmng_mtr_conntype_get(pn,RunTaskInfo->logicid);
    Convert_Oop_Data_To_Struct(pBuf,&offSet,rcvLen,desbuffer,&pOadInfo,&deslength,pwrType);
    memcpy_r((uint8*)&taskStartTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);//采集启动时间
    DT_DateTimeBCD2Time(&taskStartTime,&colStartTm);

    if(StoreTime==NULL)
    {
        time_t storeTm;
        OOP_TI_T delayTi;
        DT_DateTimeBCD2Time(&taskStartTime,&storeTm);
        delayTi.unit = pCbTask->DelayTimeUnit;
        delayTi.value = pCbTask->DelayTime;
        DT_TimeSubInv(&storeTm, delayTi);
        taskStoreTime = taskmng_acq_store_time_get(pCbTask->StoreTimeType,storeTm);
        DT_DateTimeBCD2Time(&taskStoreTime,&longTm);
        //TASK_FMT_TRACE(pipe, REC_LOG, "taskStoreTime is %d,pCbTask->TaskId is %d,oad is %08x offSet %d \n", longTm, pCbTask->TaskId, oad.value,offSet);
       // TASK_FMT_TRACE(pipe, REC_LOG,"taskStoreTime:%02x%02x-%02x-%02x %02x:%02x:%02x\n", taskStoreTime.year_h, taskStoreTime.year_l,
       //     taskStoreTime.month, taskStoreTime.day, taskStoreTime.hour, taskStoreTime.min, taskStoreTime.sec);
        if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5004 || pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5006
            ||pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5002||pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5005)// 日月曲线数据

            {
                if(pCbTask->AcqInfo.Acqtype != ACQTYP_BC)
                {
                    taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
                    taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
                }
            }

    }
    else
    {
        if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5004 || pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5006
            ||pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5002||pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5005)// 日月曲线数据
        {

            if(oad.nObjID == 0x2021 && patch_policy_flg[pipe] != 2)
            {
                if(StoreTime->sec == 0xFF)//林洋表回日冻结时间秒数可能为0xFF
                {
                    StoreTime->sec = 0;
                }
                if(DT_DateTimeBCD2Time(StoreTime,&longTm)==FALSE)//应答的时间不合法
                {
                    //不存储
                    return 0;
                }
                //检测是否需要补抄
                if(pCbTask->AcqInfo.Acqtype != ACQTYP_BC)
                {
                    IsNeedBcMeterOopFrzDayData(pCbTask,pBuf,RunTaskInfo,pipe);
                    taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
                    taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
                }

            }
            DT_DateTimeBCD2Time(StoreTime,&longTm);

        }
       else
        {
            DT_DateTimeBCD2Time(StoreTime,&longTm);
        }
    }
    //先缓存数据
    taskmng_rs485_cache_buf_save(pipe,pn,pCbTask->CsdTask.RoadTask[CsdIndex].MainOad,oad,desbuffer,deslength,colStartTm,longTm,meterData);

    return offSet;
}

/****************************************************************************
*函数名称：Oop_AUTK_Recv
*功能：  处理面向对象规约485抄表返回的记录型数据的存储
*输入参数：
*输出参数：
*返回：无
****************************************************************************/

uint16 Oop_AUTK_Recv_Profile(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{
    uint8   i = 0;
    uint16  offset = 0;
    uint16  rslt = 0;
    uint8   recNum;
    uint8   csdBuff[sizeof(CSD_t)*50];
    RCSD_t   rcsd;
    time_t        tmpTime;
    time_t        FrzTime;
    time_t        StartTime;
    time_t        EndTime;
    DateTimeBcd_t StoreTm;
   // uint16 rcsdOff = 21;
    uint16 MeterIndex;
    uint16 FindOffset;
    uint8 tmpbuff[7];
    OOP_TI_T delayTi;
    //uint8  rtpOadOff = 0;
    uint16 CsdIndex;
    BOOL   IsInPeriod = FALSE;
    uint32 EventSeq;
    DATA_CACHE_T meterData;
    uint16 pn;

    MeterIndex=RunTaskInfo->MeterIndex;
    memset((uint8*)&meterData,0x00,sizeof(meterData));
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    OadInfoMap OadInfo;
//    //事件上报时把固定列CSD先填进去
//    char RcsdData[500] = {0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
//                          0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00
//                         };

    CsdIndex=RunTaskInfo->CsdIndex;
    //根据OAD查找OAD属性描述

    memset((uint8*)&StoreTm,0x00,sizeof(DateTimeBcd_t));
    if(FALSE==taskmng_oad_info_get(oad,&OadInfo))
    {

        return 0;
    }
    rcsd.csdNum = pBuf[offset++];
    if(rcsd.csdNum>50)
        return 0;

    //准备RCSD数据
//    RcsdData[rcsdOff++] = 1;
//    memcpy(RcsdData+rcsdOff, &oad, 4);
//    memrev((uint8*)(RcsdData+rcsdOff), 2);
//    rcsdOff += 4;
//    rtpOadOff = rcsdOff;
//    RcsdData[rcsdOff++] = rcsd.csdNum;


    memset(csdBuff,0x00,sizeof(csdBuff));
    rcsd.csd = (CSD_t*)csdBuff;
    for(i=0; i<rcsd.csdNum; i++)
    {
        rcsd.csd[i].choice = pBuf[offset++];
        if(rcsd.csd[i].choice != 0)
            break;

        rcsd.csd[i].oad.nObjID = pBuf[offset++];
        rcsd.csd[i].oad.nObjID = (rcsd.csd[i].oad.nObjID<<8)+pBuf[offset++];
        rcsd.csd[i].oad.attID = pBuf[offset++] ;
        rcsd.csd[i].oad.nIndex = pBuf[offset++];

        //填事件关联OAD
        //过滤自添加的2022事件记录序号
        if(pCbTask->SchemeType == EVENTPLAN)
        {
            if(checkIsExist2022(pCbTask,oad,pipe) == 0)
            {
                if(pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i].nObjID == 0x2022)
                {
                    //RcsdData[rtpOadOff]--;
                    continue;
                }
            }
        }
       // memcpy(RcsdData+rcsdOff, pBuf+(offset-4), 4);
        //rcsdOff += 4;
    }

    if(i!=rcsd.csdNum)
    {
        return 0;
    }

    if(offset>=rcvLen)
        return 0;
    if(pBuf[offset++]!=ARRAY)//1表示是SEQUENCE OF A-RecordRow，代表有记录
        return 0;

    recNum = pBuf[offset++];//有多少条记录

    if(recNum < 1)//记录条数小于1条
    {
        if(pCbTask->SchemeType == EVENTPLAN && pCbTask->AcqInfo.Acqtype == ACQTYP_LASTN)
            OopEventCntReset(oad,mCbMtrPara,pipe);//有疑问的地方？？？？？？？
        return 0;
    }

    while(recNum--)
    {
        FindOffset=0;
        if((oad.nObjID == 0x5002||oad.nObjID == 0x5003)//分钟冻结和小时冻结 按冻结时标，主OAD是5002或者是5003
                ||pCbTask->StoreTimeType == STRTMTYP_FRZTIME)
        {
            FindOffset = offset;
            if((oad.nObjID & 0xF000) == 0x5000)//冻结类型查找冻结时标
            {
                for(i=0; i<rcsd.csdNum; i++)
                {
                    if(rcsd.csd[i].oad.nObjID == 0x2021)//冻结时标
                    {
                        ((DateTimeHex_t*)(pBuf+FindOffset+1))->sec = 0x00;
                        //测试时标有回复为空的情况
                        if(pBuf[FindOffset] !=0)
                        {
                            DT_DateTimeHex2BCD((DateTimeHex_t*)(pBuf+FindOffset+1),&StoreTm);
                        }
                        #if(1 == OOP_CRV_ADDREAD_485)
                        if(oad.nObjID == 0x5002||oad.nObjID == 0x5003)//分钟冻结和小时冻结
                        {
//                            if(PastMin >= 60)
                            {
                                //更新曲线抄读起始时间
                                UpdateCrvNewStartTime(oad.nObjID, &StoreTm, pCbTask->TaskId, pCbTask->SchemeId, pCbTask->MsInfo[MeterIndex].Sn, &pCbTask->AcqInfo);
                            }
                        }
                        #endif
                        break;
                    }
                    taskmng_oop_next_read_data_get(pBuf,&FindOffset,rcvLen,NULL_DATA,FALSE);
                }
            }
            else//事件类型查找事件发生时间
            {
                for(i=0; i<rcsd.csdNum; i++)
                {
                    if(rcsd.csd[i].oad.nObjID == 0x201E)//事件发生时间
                    {
                        DT_DateTimeHex2BCD((DateTimeHex_t*)(pBuf+FindOffset+1),&StoreTm);//如果对应的返回数据的CSD是时间发生时间
                        break;
                    }
                    taskmng_oop_next_read_data_analysis(pBuf,&FindOffset,rcvLen,NULL_DATA,FALSE);//对每个对应抄读回来的CSD数据进行解析；
                }
            }
            if(i>=rcsd.csdNum)//表没有回数据冻结时间
            {

                return 0;
            }
        }
        else
        {
            memcpy_r(tmpbuff,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);
            if((oad.nObjID == 0x5004) && (pCbTask->AcqInfo.Acqtype == ACQTYP_BC
                #if OOP_DAY_ADDREAD_485 == 1//采用日任务补抄起始时间
                || (pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM) || (pCbTask->AcqInfo.Acqtype == ACQTYP_TI)
                #endif
            ))
            {
                tmpTime =RunTaskInfo->ProfilePara.startTime;
                DT_Time2DateTimeBCD(tmpTime,(DateTimeBcd_t*)tmpbuff);
            }
            if(pCbTask->SchemeType == EVENTPLAN)//事件采集方案秒标识事件的n次
            {
                tmpbuff[6] = recNum;
            }
            DT_DateTimeBCD2Time((DateTimeBcd_t*)tmpbuff,&FrzTime);
            if(pCbTask->StoreTimeType == STRTMTYP_STARTTM)
            {
                delayTi.unit = pCbTask->DelayTimeUnit;
                delayTi.value = pCbTask->DelayTime;
                DT_TimeSubInv(&FrzTime, delayTi);
            }
            StoreTm = taskmng_acq_store_time_get(pCbTask->StoreTimeType,FrzTime);

            if(pCbTask->SchemeType == EVENTPLAN && pCbTask->AcqInfo.Acqtype == ACQTYP_LASTN)
            {
                FindOffset = offset;
                /*240609 不影响原有逻辑 判断下电表停电事件的事件结束时间  */
                if(oad.nObjID == 0x3011)
                {
                    uint16 tmpoffset = offset;
                    uint8 flag = 0;
                    for(i=0;i<rcsd.csdNum;i++)
                    {
                        if(rcsd.csd[i].oad.nObjID == 0x2020)
                        {
                            if(pBuf[tmpoffset++] == DT_DATETIME_S)
                            {
                                DT_DateTimeHex2Time((DateTimeHex_t *) (pBuf + tmpoffset), &tmpTime);
                                flag = 1;
                                break;
                            }
                        }
                        taskmng_oop_next_read_data_get(pBuf, &tmpoffset, rcvLen, NULL_DATA, FALSE);
                    }

                    if(flag == 1 )
                    {
                        /*如果有事件结束时间 则判断下是不是上个周期内  */
                        memcpy_r(tmpbuff, (uint8 *) RunTaskInfo->LastTurnBeginTime, 7);
                        DT_DateTimeBCD2Time((DateTimeBcd_t *) tmpbuff, &StartTime);
                        memcpy_r(tmpbuff, (uint8 *) RunTaskInfo->ThisTurnBeginTime, 7);
                        DT_DateTimeBCD2Time((DateTimeBcd_t *) tmpbuff, &EndTime);
                        TASK_FMT_TRACE(pipe, REC_LOG, "StartTime is %d,EndTime is %d,tmpTime is %d\n", StartTime, EndTime, tmpTime);
                       //240628测试的时候直接对时到0点6分 然后抄的事件结束时间是5分 任务也未配延时
                       //先直接把结束时间的判断放到当前时间
                        //if(tmpTime >= StartTime && tmpTime < EndTime)
                        if(tmpTime >= StartTime && tmpTime < time(NULL))    
                        {
                            pn  = pCbTask->MsInfo[MeterIndex].Sn;
                            TASK_FMT_TRACE(pipe, REC_LOG, "pn %d 新发生上电\n", pn);
                            OOP_METER_BASIC_T basic;
                            uint8 addr[6]={0};
                            memset(&basic,0,sizeof(basic));
                            taskmng_mtr_basic_info_get(pn,0,&basic);
                            if(basic.protocol != 0)
                            {
                                memcpy_r(addr,basic.tsa.add,6);
                                taskmng_meter_time_add(pipe,pn,basic.protocol,addr);
                            }
                        }
                    }


                }
                for(i=0; i<rcsd.csdNum; i++)
                {
                    if(rcsd.csd[i].oad.nObjID == 0x2020)//事件结束时间,现场的电表事件发生后恢复，事件的发生序号可能不变；
                    {
                        DT_DateTimeHex2Time((DateTimeHex_t*)(pBuf+FindOffset+1),&tmpTime);
                        break;
                    }
                    if(rcsd.csd[i].oad.nObjID == 0x201E)//事件发生时间
                    {
                        DT_DateTimeHex2Time((DateTimeHex_t*)(pBuf+FindOffset+1),&tmpTime);
                        break;
                    }
                    taskmng_oop_next_read_data_get(pBuf,&FindOffset,rcvLen,NULL_DATA,FALSE);
                }

                if(i>=rcsd.csdNum)//表没有返回事件发生时间
                {
                    IsInPeriod = FALSE;
                }
                else
                {

                    memcpy_r(tmpbuff,(uint8*)RunTaskInfo->LastTurnBeginTime,7);
                    DT_DateTimeBCD2Time((DateTimeBcd_t*)tmpbuff,&StartTime);
                    memcpy_r(tmpbuff,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);
                    DT_DateTimeBCD2Time((DateTimeBcd_t*)tmpbuff,&EndTime);
                    TASK_FMT_TRACE ( pipe,REC_LOG, "StartTime is %d,EndTime is %d,tmpTime is %d\n",StartTime,EndTime,tmpTime);
                    if(tmpTime >= StartTime && tmpTime < EndTime)
                    {
                        IsInPeriod = TRUE;
                    }
                }

                if(IsOopMetEventNeedSave(pipe,oad,rcsd.csd,rcsd.csdNum,pBuf+offset, rcvLen-offset, IsInPeriod, RunTaskInfo, pCbTask,&EventSeq) == FALSE)
                {
                    TASK_FMT_TRACE ( pipe,REC_LOG, "存储过了不在存了 is %d\n",pCbTask->MsInfo[MeterIndex].Sn);
                    return 0;//有疑问的地方？？？？？？
                }
            }
        }
        //查找数据冻结时标
        #ifdef AREA_SHANDONG
        if((oad.nObjID == 0x5002||oad.nObjID == 0x5003))
        {
            if(pCbTask->Isenergyoad ==TRUE)
            {
                rcsd.csdNum = pCbTask->energynum;
            }
            else
            {
                rcsd.csdNum = pCbTask->energynum+1;
            }
        }
        #endif
        for(i=0; i<rcsd.csdNum; i++)
        {
            if(rcvLen<offset)
                return 0;

            #if(1 == OOP_DAY_ADDREAD_485)
            if(rcsd.csd[i].oad.nObjID == 0x2021) //数据冻结时标
            {
                if((oad.nObjID == 0x5004) &&((pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM) || (pCbTask->AcqInfo.Acqtype == ACQTYP_TI)))
                {
                    tmpTime =RunTaskInfo->ProfilePara.startTime;
                    DT_Time2DateTimeBCD(tmpTime,(DateTimeBcd_t*)tmpbuff);
                    UpdateDayNewStartTime(oad.nObjID,(DateTimeBcd_t *)tmpbuff,pCbTask->TaskId,pCbTask->SchemeId,pCbTask->MsInfo[MeterIndex].Sn,&pCbTask->AcqInfo);
                }
            }
            #endif

            //需要判断OAD是否在抄表CSD中，判断返回的报文中是否有相应的CSD并且存储;
            //rslt = Oop_AUTK_Recv(pipe, rcsd.csd[i].oad,pBuf+offset, rcvLen-offset, mCbMtrPara, &StoreTm,pCbTask,RunTaskInfo);
            rslt = Oop_Autk_Save_Buf(pipe, rcsd.csd[i].oad,pBuf+offset, rcvLen-offset, mCbMtrPara, &StoreTm,pCbTask,RunTaskInfo,&meterData);
            //TASK_FMT_TRACE ( pipe,COMM_LOG, "OAD rslt is %d\n",rslt);
            if(rslt == 0)
            {
                taskmng_oop_next_read_data_get(pBuf,&offset,rcvLen,NULL_DATA,FALSE);
            }
            else
            {

                offset+=rslt;
            }
        }
        taskmng_rs485_cache_buf_write_to_db(pipe,pn,LOGIC_ID_YC,&meterData);

    }
    if((pCbTask->SchemeType ==EVENTPLAN) && (pCbTask->rptFlag == TRUE) )
    {

        uint32 EventSeqReport;
        uint16 Pn = pCbTask->MsInfo[MeterIndex].Sn;//测量点号
        TASK_FMT_TRACE( pipe, REC_LOG, "该事件方案需要上报！\n");
        time_t StartTime;
        DateTimeBcd_t tmpTime;
        memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);
        DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StartTime);

        {
            EventSeqReport = GetTaskReportSeq( pipe, RunTaskInfo->TaskId,RunTaskInfo->logicid, oad, Pn);
            TASK_FMT_TRACE( pipe, REC_LOG, "该事件已经上报的记录序号为%ld  \n",EventSeqReport );
            {
                Save698EventData(pipe, pBuf, rcvLen, Pn, oad, StartTime);
                SetTaskReportSeq( pipe, RunTaskInfo->TaskId, RunTaskInfo->logicid, oad, Pn,EventSeq);
            }

        }
    }
    return offset;
}
/****************************************************************************
名称： GetCurvePatchTime
功能： 根据补抄任务设置的补抄轮次，获取补抄起始时间
输入参数：
输出参数：
返回： 补抄起始时间
****************************************************************************/
time_t GetCurveTaskBeginTime(time_t startTime, SCHEME_TASK_RUN_INFO * pCbTask)
{

    uint32 CycleTime = 0;
    OOP_TI_T Ti;

    if(pCbTask->AcqInfo.LastN < 1)
        return startTime;

    if(FALSE == get_task_run_frequence(pCbTask->TaskId, &Ti))
    {
        return startTime;
    }

    if((Ti.unit != TASK_CYCLE_FLAG_MIN && Ti.unit != TASK_CYCLE_FLAG_HOUR))
        return startTime;

    if(Ti.value == 0xffff)
        return startTime;

    if(Ti.unit == TASK_CYCLE_FLAG_MIN)
        CycleTime = Ti.value * 60;
    else
        CycleTime = Ti.value * 3600;

    TASK_FMT_TRACE(-1, COMM_LOG, "startTime is %d,pCbTask->AcqInfo.LastN is %d,CycleTime is %d\n", startTime, pCbTask->AcqInfo.LastN, CycleTime);

#ifdef AREA_SHANDONG
    startTime -= (CycleTime * (pCbTask->AcqInfo.LastN));

#else

    startTime -= (CycleTime * (pCbTask->AcqInfo.LastN));
#endif

    TASK_FMT_TRACE(-1, COMM_LOG, "startTime is %d,pCbTask->TaskId is %d\n", startTime, pCbTask->TaskId);
    return startTime;
}

/****************************************************************************
名称： GetCurvePatchTime
功能： 根据补抄任务设置的补抄轮次，获取补抄起始时间
输入参数：
输出参数：
返回： 补抄起始时间
****************************************************************************/
time_t GetDayTaskBeginTime(uint8 pipe, time_t startTime, SCHEME_TASK_RUN_INFO * pCbTask)
{

    uint32 CycleTime = 0;
    OOP_TI_T Ti;

    if(pCbTask->AcqInfo.LastN < 1)
        return startTime;

    if(FALSE == get_task_run_frequence(pCbTask->TaskId, &Ti))
    {
        return startTime;
    }

    if((Ti.unit != TASK_CYCLE_FLAG_DAY))
    {
        return startTime;
    }

    if(Ti.value == 0xffff)
    {
        return startTime;
    }

    if(Ti.unit == TASK_CYCLE_FLAG_DAY)
    {
        CycleTime = Ti.value * 60 * 60 * 24;
    }
    else
    {
        return startTime;
    }

    startTime -= (CycleTime * (pCbTask->AcqInfo.LastN));
    TASK_FMT_TRACE(pipe, REC_LOG, "startTime is %d,pCbTask->TaskId is %d\n", startTime, pCbTask->TaskId);
    DateTimeBcd_t       tmpTime;
    DT_Time2DateTimeBCD(startTime, &tmpTime);
    TASK_FMT_TRACE(pipe, REC_LOG,"计算补抄开始时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", tmpTime.year_h, tmpTime.year_l,
         tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.min, tmpTime.sec);
    return startTime;
}


/****************************************************************************
*函数名称：Oop_LoadPrifle_Init
*功能：  对抄表的开始时间进行处理
*输入参数：
*输出参数：
*返回：无
****************************************************************************/

void Oop_LoadPrifle_Init(cbLPPara_t *pCbLpPara ,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{

    uint16 CsdIndex;
    uint16 taskid;
    #if((1 == OOP_DAY_ADDREAD_485)||(1 == OOP_CRV_ADDREAD_485))
    uint16 MeterIndex;
    MeterIndex=RunTaskInfo->MeterIndex;
    #endif

    taskid=pCbTask->TaskId;
    CsdIndex=RunTaskInfo->CsdIndex;

    if((pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5002)
    ||(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5003))//分钟曲线或小时曲线
    {
        OOP_TI_T Ti;

        OOP_TI_T delayTi;
        DateTimeBcd_t tmpTime;
		OOP_TI_T TaskTi;
        get_task_run_frequence(taskid,&TaskTi);
        #ifdef AREA_SHANDONG
		if((TaskTi.unit == pCbTask->AcqInfo.TI.unit)&&(TaskTi.value = pCbTask->AcqInfo.TI.value))
		{
	        memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);
	        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->startTime);
	        delayTi.value = pCbTask->DelayTime;
	        delayTi.unit = pCbTask->DelayTimeUnit;
	        DT_TimeSubInv(&pCbLpPara->startTime, delayTi);
	        memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnEndTime,7);
	        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->endTime);
		}
		else
		{
			memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->LastTurnBeginTime,7);

	        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->startTime);
	        memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);

	        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->endTime);
		}
        #else
        //采集间隔和任务间隔一样,且有任务延时的采集本周期的数据,否则采集上一周期
        if((TaskTi.unit == pCbTask->AcqInfo.TI.unit)&&(TaskTi.value = pCbTask->AcqInfo.TI.value)&&(pCbTask->DelayTime !=0))
        {
            memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);

            DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->startTime);
            delayTi.value = pCbTask->DelayTime;
            delayTi.unit = pCbTask->DelayTimeUnit;
            DT_TimeSubInv(&pCbLpPara->startTime, delayTi);
            memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnEndTime,7);

            DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->endTime);

        }
        else
        {
            memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->LastTurnBeginTime,7);

            DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->startTime);
            memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);

            DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->endTime);
        }
        #endif
        pCbLpPara->tmInv.unit = pCbTask->AcqInfo.TI.unit;
        pCbLpPara->tmInv.value= pCbTask->AcqInfo.TI.value;


        if(pCbLpPara->tmInv.value==0)//配置的采集方式数据冻结时标
        {
            memset(&Ti,0,sizeof(Ti));
            if(0x5002 == pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID)
            {
                //时标间隔为任务频率
                get_task_run_frequence(taskid,&Ti);
                if((Ti.unit >= TASK_CYCLE_FLAG_HOUR)
                ||((Ti.unit == TASK_CYCLE_FLAG_MIN)&&(Ti.value > 15)))
                {
                    //任务频率大于15分钟，时标间隔设为15分钟
                    Ti.unit = TASK_CYCLE_FLAG_MIN;
                    Ti.value = 15;
                }
            }
            else
            {
                Ti.unit = TASK_CYCLE_FLAG_HOUR;//小时
                Ti.value = 1;//默认1小时
            }
            pCbLpPara->tmInv.unit = Ti.unit;
            pCbLpPara->tmInv.value = Ti.value;
        }
        pCbLpPara->tmInv.value *= 4;
        if(pCbTask->AcqInfo.Acqtype == ACQTYP_BC)
        {
            pCbLpPara->startTime = GetCurveTaskBeginTime(pCbLpPara->startTime, pCbTask);
            return;
        }
        #if(1 == OOP_CRV_ADDREAD_485)
        time_t OriStartTime = 0;
        OriStartTime = pCbLpPara->startTime;
        {
        pCbLpPara->startTime = GetCrvNewStartTime(OriStartTime, pCbTask->TaskId, pCbTask->SchemeId, pCbTask->MsInfo[MeterIndex].Sn, 0);
		//pCbLpPara->startTime = RunTaskInfo->StartTime;
        }
        #endif
    }
    else if((pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5004)&& ((pCbTask->AcqInfo.Acqtype == ACQTYP_BC)
    #if(1 == OOP_DAY_ADDREAD_485) //日任务配置为按照冻结时标和采集间隔进行抄读
    || (pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM) || (pCbTask->AcqInfo.Acqtype == ACQTYP_TI)
    #endif
    ))
    {
        DateTimeBcd_t tmpTime;
        //      #if(1 == OOP_DAY_ADDREAD_485)
        time_t OriStartTime = 0;
        //      #endif
        memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->LastTurnBeginTime,7);
        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->startTime);
        memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);
        DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->endTime);

        if((pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM) || (pCbTask->AcqInfo.Acqtype == ACQTYP_BC))
        {
            pCbLpPara->tmInv.unit = TASK_CYCLE_FLAG_DAY;
            pCbLpPara->tmInv.value = 1;
        }
        if(pCbTask->AcqInfo.Acqtype == ACQTYP_TI)
        {
            pCbLpPara->tmInv.unit = pCbTask->AcqInfo.TI.unit;
            pCbLpPara->tmInv.value= pCbTask->AcqInfo.TI.value;
            if(pCbLpPara->tmInv.value==0)
            {
                pCbLpPara->tmInv.unit = TASK_CYCLE_FLAG_DAY;
                pCbLpPara->tmInv.value = 1;
            }
        }
        //      #if(1 == OOP_DAY_ADDREAD_485)
        OriStartTime = pCbLpPara->startTime;

        //      #endif
        if(pCbTask->AcqInfo.Acqtype == ACQTYP_BC)
        {
            pCbLpPara->startTime = GetDayTaskBeginTime(2,OriStartTime, pCbTask);
        }
        #if(1 == OOP_DAY_ADDREAD_485) //日任务配置为按照冻结时标和采集间隔进行抄读
        if((pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM) || (pCbTask->AcqInfo.Acqtype == ACQTYP_TI))
        {
            pCbLpPara->startTime = GetDayNewStartTime(OriStartTime, pCbTask->TaskId, pCbTask->SchemeId, pCbTask->MsInfo[MeterIndex].Sn, 0);

        }
        #endif
        RunTaskInfo->ProfilePara.startTime = pCbLpPara->startTime;
        RunTaskInfo->ProfilePara.tmInv = pCbLpPara->tmInv;
    }
    else
    {
        pCbLpPara->startTime = time(NULL);
        pCbLpPara->tmInv.value = 15;
    }

}


/****************************************************************************
*函数名称：Oop_ToNextReadItem
*功能：  切换抄读的数据项
*输入参数：
*输出参数：
*返回：无
****************************************************************************/

void  Oop_ToNextReadItem(BOOL NextCSDFlg,S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe,OOP_OAD_U Suboad)
{
    time_t    tmpTime;
    time_t    tmpendTime;
    uint8 CSDType;
    uint16 CsdIndex,SubOadIndex;
    uint16 MeterIndex,pn,tmpMsIndex;
    OOP_TI_T    delayTi;

     delayTi.value = pCbTask->DelayTime;
     delayTi.unit = pCbTask->DelayTimeUnit;

    CsdIndex=RunTaskInfo->CsdIndex;
    SubOadIndex=RunTaskInfo->SubOadIndex;

    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);

    if(pCbTask==NULL || (CsdIndex>=pCbTask->CsdTask.RoadNum))
    {
        return;
    }

    if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID!=0x0000)
    {
        CSDType=1;
    }
    else
    {
        CSDType=0;
    }

    if(NextCSDFlg==FALSE)
    {
        if(CSDType>0)
        {
            if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
            {
                if((SubOadIndex+1)<pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum)
                {
                    RunTaskInfo->SubOadIndex++;//转到下一个从OAD
                    pComm_Info->MetStat = CB_STATE_SEND;
                    return;
                }
				#ifdef AREA_SHANDONG
				else
				{
					RunTaskInfo->CsdIndex++;//有疑问的方法，是该切换CsdIndex，还是切换SubOadIndex;
        			RunTaskInfo->SubOadIndex= 0;

					pComm_Info->MetStat = CB_STATE_SEND;
					return;
				}
				#endif
            }
            else
            {
                if((CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5002)
                        ||(CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5003)
                        #if(1 == OOP_DAY_ADDREAD_485)
                        ||(CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5004)
                        #else
                        ||((CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5004) && (pCbTask->AcqInfo.Acqtype == ACQTYP_BC))
                        #endif
                        )
                {

                    //分钟曲线需要判断所有的时间点是否都已抄完
                    tmpTime = pComm_Info->cbLPPara.startTime;
                    DT_TimeAddInv(&tmpTime,pComm_Info->cbLPPara.tmInv);
                    if(tmpTime > pComm_Info->cbLPPara.endTime)
                    {
                        if((CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5002)
                            ||(CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5003))
                        {
                            OOP_TI_T tmInv;
                            tmInv = pComm_Info->cbLPPara.tmInv;
                            tmInv.value = tmInv.value/4;
                            DT_TimeAddInv(&pComm_Info->cbLPPara.startTime,tmInv);
                        }
                        else
                            DT_TimeAddInv(&pComm_Info->cbLPPara.startTime,pComm_Info->cbLPPara.tmInv);
                    }
                    else
                        DT_TimeAddInv(&pComm_Info->cbLPPara.startTime,pComm_Info->cbLPPara.tmInv);

                    tmpendTime = pComm_Info->cbLPPara.endTime;

                    DT_TimeSubInv(&tmpendTime, delayTi);

                    if(pComm_Info->cbLPPara.startTime < tmpendTime)
                    {
                        RunTaskInfo->ProfilePara.startTime = pComm_Info->cbLPPara.startTime;
                        pComm_Info->MetStat = CB_STATE_SEND;
                        return;
                    }
                    if(pCbTask->AcqInfo.Acqtype == ACQTYP_BC)
                    {
                        taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
                        taskmng_task_record_file_set(pipe,tmpMsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
                    }
                }

            }
        }
    }
    if(CSDType==0)
    {
        RunTaskInfo->SubOadIndex = pComm_Info->sendnum+RunTaskInfo->SubOadIndex;

        if((RunTaskInfo->SubOadIndex)>=pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum)
        {

            if(pComm_Info->SucOADNum>=pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum)
            {
                taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
                taskmng_task_record_file_set(pipe,tmpMsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);

            }
            RunTaskInfo->CsdIndex++;
        }
    }
    else
    {
        #ifdef AREA_SHANDONG
        if((pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5002))
        {
        	if((pCbTask->energynum ==0)&&(Suboad.value == 0x20210200))
        	{
				RunTaskInfo->SubOadIndex=RunTaskInfo->SubOadIndex+1;
        	}
			else
			{
            	RunTaskInfo->SubOadIndex = RunTaskInfo->SubOadIndex+pCbTask->energynum;
			}
            if(RunTaskInfo->SubOadIndex>=pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum)
            {
                RunTaskInfo->CsdIndex++;
                RunTaskInfo->SubOadIndex=0;
            }
        }
        else
        {
                RunTaskInfo->CsdIndex++;
                RunTaskInfo->SubOadIndex= 0;
        }
        #else
        RunTaskInfo->CsdIndex++;//有疑问的方法，是该切换CsdIndex，还是切换SubOadIndex;
        RunTaskInfo->SubOadIndex= 0;
        #endif
    }
    CsdIndex=RunTaskInfo->CsdIndex;
    if((CsdIndex<pCbTask->CsdTask.RoadNum)&&(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x3000))
    {
        if(CSDType==1)
        {
            RunTaskInfo->SubOadIndex= 0;
        }
        if((CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5002)
            ||(CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5003)
            #if(1 == OOP_DAY_ADDREAD_485)
            ||(CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5004)
            #else
            ||((CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5004) && (pCbTask->AcqInfo.Acqtype == ACQTYP_BC))
            #endif
            )
            Oop_LoadPrifle_Init(&pComm_Info->cbLPPara,pCbTask,RunTaskInfo);
    }
    pComm_Info->MetStat = CB_STATE_SEND;

}
/****************************************************************************
*函数名称：addCharDataByte
*功能：  发送的抄表表文前加FE;
*输入参数：
*输出参数：
*返回：无
****************************************************************************/

int addCharDataByte(uint8 *tmp_buff,uint8 *sendBuff,int sendLen,uint32 num)
{
    uint32 num_tmp = 0;
    int tmp_len = sendLen + num;
    while(num--)
    {
        tmp_buff[num_tmp++] = 0xFE;
    }
    memcpy(&tmp_buff[num_tmp],&sendBuff[0],(int)sendLen);
    return tmp_len;
}

/****************************************************************************
*函数名称：IsNeedBcMeterOopFrzDayData
*功能：  根据抄读回来的时标来判断该冻结数据是否需要补抄;
*输入参数：
*输出参数：
*返回：无
****************************************************************************/

void  IsNeedBcMeterOopFrzDayData(SCHEME_TASK_RUN_INFO *pCbTask,uint8 *pBuf,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe)
{
    rtc_t      rtc_tmp;
    time_t     LastTurnBeginTime;
    time_t     ThisTurnBeginTime;
    DateTimeHex_t Dtfrztime;
    time_t     FrzTime;

    uint16 CsdIndex,MsIndex;
    uint16 MeterIndex,pn;

    CsdIndex=RunTaskInfo->CsdIndex;
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    if(pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM || pCbTask->AcqInfo.Acqtype == ACQTYP_TI
            || STRTMTYP_FRZTIME == pCbTask->StoreTimeType)
    {
        memcpy(&Dtfrztime,pBuf+1,7);
        DT_DateTimeHex2Time(&Dtfrztime,&FrzTime);
        printf("FrzTime %ld 电表冻结时间:%02d-%02d-%02d %02d:%02d:%02d\n",FrzTime,Dtfrztime.year_l
               ,Dtfrztime.month,Dtfrztime.day,Dtfrztime.hour,Dtfrztime.min,Dtfrztime.sec);
        //使用前闭后闭，参数初始化决定开
        memset(&rtc_tmp, 0, sizeof(rtc_t));
        memcpy(&rtc_tmp, RunTaskInfo->LastTurnBeginTime, 6);
        LastTurnBeginTime = commfun_rtc2time2(rtc_tmp);//上轮开始时间
        printf("LastTurnBeginTime %ld 上轮开始时间:%02x-%02x-%02x %02x:%02x:%02x \n",LastTurnBeginTime,rtc_tmp.yy,rtc_tmp.mm,rtc_tmp.dd,rtc_tmp.hh,rtc_tmp.nn,rtc_tmp.ss);
        memset(&rtc_tmp, 0, sizeof(rtc_t));
        memcpy(&rtc_tmp, RunTaskInfo->ThisTurnBeginTime, 6);
        ThisTurnBeginTime = commfun_rtc2time2(rtc_tmp);//本轮开始时间
        printf("ThisTurnBeginTime %ld 本轮开始时间:%02x-%02x-%02x %02x:%02x:%02x \n",ThisTurnBeginTime,rtc_tmp.yy,rtc_tmp.mm,rtc_tmp.dd,rtc_tmp.hh,rtc_tmp.nn,rtc_tmp.ss);
        if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5002)
        {
            if((FrzTime >= LastTurnBeginTime) && (FrzTime < ThisTurnBeginTime))//时间合法
            {
                //set_bit_value(RunTaskInfo->MtrResult,128,MsIndex);//不需要抄读;//不需要抄读
                taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
                taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
            }
        }
        else
        {
            if((FrzTime > LastTurnBeginTime) && (FrzTime <= ThisTurnBeginTime))//时间合法
            {
                //set_bit_value(RunTaskInfo->MtrResult,128,MsIndex);//不需要抄读
                taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
                taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
            }
        }

        if(pCbTask->AcqInfo.Acqtype == ACQTYP_LASTN)//上N次的不需要抄读
        {
            printf("按采集上N次采集，不补抄\r\n");
            //set_bit_value(RunTaskInfo->MtrResult,128,MsIndex);;//不需要补抄
            taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
            taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
        }

    }
    else
        {
            //set_bit_value(RunTaskInfo->MtrResult,128,MsIndex);;//不需要抄读
            taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
            taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
        }

    taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
}


