
#include "CtsAll.h"
#include "ble.h"

#define MAX_BLE_SLAVE_PORT  30   //蓝牙从模式最多30个端口 2到31
extern uint8 gdelay;

/*******************************************************************************
* 函数名称: bt_port_config
* 函数功能: 抄表前配置下端口 一次最多能配30个
* 输入参数: pipe 端口号
        msindex 开始配置的索引
* 输出参数: 
* 返 回 值: 0                 配置成功
*        -1                配置失败
*******************************************************************************/
int bt_port_config(uint8 pipe,uint16 msindex)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int ret;
    uint16 i;
    uint16 allmeternum = pAmrThreadArg->TmnAmrInfo.MtrCnt; 
    BLE_PORTS_CONF_T bleports;
    memset(&bleports,0x00,sizeof(BLE_PORTS_CONF_T));
    uint8 addr[6]={0};
    uint8 addrtmp[6]={0};
    uint8 payload[MAX_MSG_LEN]={0};
    uint16 len;
    uint16 iid = 0x000A;
    uint16 iop = 0x0014;
    uint16 recvlen;
    uint8 recvbuf[MAX_MSG_LEN] = {0};
    for(i =0;i<MAX_BLE_SLAVE_PORT;i++)
    {
        //判断是否到达表上限
        if(i+msindex>=allmeternum)
            break;
        pAmrThreadArg->pMtrAmrInfo[msindex+i].blePort = 2+i;    //端口号从2开始
        taskmng_mtr_addr_get(pAmrThreadArg->pMtrAmrInfo[msindex+i].Pn,0,addr);
        bleports.tConf[i].port =2+i;
        bleports.tConf[i].mode = BLE_MODE_SLAVE;
        memcpy_r(addrtmp,addr,6);
        ble_addr_to_mac(addrtmp,6,BLE_MAC_METER,bleports.tConf[i].MAC);
        //memcpy_r(bleports.tConf[i].MAC,addr,6);
        
        bleports.tConf[i].match.mode = BLE_MATCH_JW;
        bleports.tConf[i].match.len = 0;
    }
    bleports.num = i;
    len = ble_port_config_package(&bleports,payload,MAX_MSG_LEN);
    if(len==0)
        return -1;
    ret = taskmng_up_msg_fun_deal(iid,iop,len,payload,BLE_APP_NAME,&recvlen,recvbuf);
    if(ret>0)
    {
        if(recvbuf[0]==1)
        {
            TASK_FMT_TRACE( pipe, REC_LOG,"蓝牙端口配置成功 \n");
            return 0;
        }
    }
    return -1;
}

/*******************************************************************************
* 函数名称: bt_seq_is_valid
* 函数功能: 判断上行帧是否有效帧
* 输入参数: port     收到的端口号
* 输出参数: OutSeq    输出对应的帧索引
* 返 回 值: 1         有效
*           0         无效
*******************************************************************************/
uint8 bt_seq_is_valid(uint8 Pipe,uint8* OutSeq, uint16 port)
{
    int i = 0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    for(i = 0; i < 3; i++)
    {
        //上行帧序号在所发有效帧之内

        if((pAmrThreadArg->AmrSendFrameInfo.Queue[i].bleport == port)
                && (pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag == 1))
        {
            *OutSeq = i;
            return 1;
        }
    }

    return 0;
}
/*******************************************************************************
* 函数名称: bt_paral_amr_frame_make
* 函数功能: 并行抄表组帧
* 输入参数: 
* 输出参数: 
* 返 回 值: 1                  全部完成
*           0                  帧未抄读完成，切换到发送
*******************************************************************************/
int bt_paral_amr_frame_make(uint8 Pipe,CTS_RUN_TASK_T *CurTask,SCHEME_TASK_RUN_INFO SchemeTaskInfo)
{
    uint8 i;
    int ret;
    uint16 Pn;
    uint8 prtl;
    uint8 MtrAddr[6];
    uint8 index;
    uint16       FrameLen = 0;
    uint8        FrameBuff[500]={0};  //帧数据缓冲区
    uint8        PayLoad[500];
    int          MsIndex;    //当前线程中的索引
    DI_INFO_T    TmpDiInfo; 
    //MESSAGE_INFO_T    *MsgSend;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    //uint8   j;
    uint8 logicid;
    uint16 iid = 0x000A;
    uint16 iop = 0x0010;
    int result = 0;
    uint16 sendindex;
    //蓝牙3个并发
    for(i=0;i<3;i++)
    {
        FrameLen = 0;
        if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==0)
        {
            memset(&TmpDiInfo,0x00,sizeof(DI_INFO_T));
            ret = taskmng_next_broad_DI_get(Pipe,&TmpDiInfo,&MsIndex,&SchemeTaskInfo,CurTask);
            //TASK_FMT_TRACE( Pipe, REC_LOG, "taskmng_next_broad_DI_get %d \n",ret);
            if(ret == SEARCH_SUCCESS)
            {
                Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
                logicid = CurTask->logicid;
                taskmng_mtr_addr_prtl_get( Pn, logicid,MtrAddr, &prtl);
                pAmrThreadArg->AmrSendFrameInfo.QueueCnt++;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag =1;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn = Pn;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].bleport = pAmrThreadArg->pMtrAmrInfo[MsIndex].blePort;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].MsIndex = MsIndex;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime = time(NULL);
                pAmrThreadArg->pMtrAmrInfo[MsIndex].LastSendTime = time(NULL);//当前电表最近一次发送时间
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].PrtlMode = prtl;
                memcpy(pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,MtrAddr,6);
                memcpy((void *)&pAmrThreadArg->AmrSendFrameInfo.Queue[i].DiInfo,(void*)&TmpDiInfo,sizeof(DI_INFO_T));
                //TASK_FMT_TRACE( Pipe, REC_LOG, "Pn %d MsIndex %d prtl %d CommBeginTime %ld \n",Pn,MsIndex,prtl,time(NULL));
                FrameLen = taskmng_meter_frame_make(Pipe,i,pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime,pAmrThreadArg->AmrSendFrameInfo.Queue[i],FrameBuff,CurTask->TaskId,CurTask->logicid);
                result = 0 ;
            }
            else if(ret == SEARCH_FINISH)
            {
                result = 1 ;
                continue;
                //return 1; //该任务完成
            }else     //没有要抄的表了
            {
                result = 0 ;
                continue;
                //return 0;
            }
        }
        else
        {
            //判断是否需要发送分帧确认帧
            if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].NeedSendSeg == 1)
            {
                LinkLayer_t* pLinkLayer = FindLinkLayer(oop_pipe_get(Pipe,i),NULL);
                if(NULL==pLinkLayer)
                {
                    pAmrThreadArg->AmrSendFrameInfo.Queue[i].NeedSendSeg = 0;
                    continue;
                }
                FrameLen = pLinkLayer->sendLen;
                memcpy(FrameBuff,pLinkLayer->sendBuff,FrameLen);
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].NeedSendSeg = 0;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime = time(NULL);
                Pn = pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn;
                memcpy(MtrAddr,pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,6);
                TASK_FMT_TRACE( Pipe, REC_LOG,"Pn %d 发送分帧确认帧 \n",Pn);
                result = 0 ;
            }
        }
        if(FrameLen>0)
        {
            memset(PayLoad,0x00,sizeof(PayLoad));
            index = 0;
            uint32 bleport = pAmrThreadArg->AmrSendFrameInfo.Queue[i].bleport;
            memcpy_r(&PayLoad[index],(uint8*)&bleport,4);
            index+=4;

            if(FrameLen<0x80)
            {
                PayLoad[index++] = FrameLen;
            }
            else
            {
                PayLoad[index++] = 0x82;
                PayLoad[index++] = (uint8)(FrameLen>>8);
                PayLoad[index++] = (uint8)FrameLen;
            }
            memcpy(&PayLoad[index],FrameBuff,FrameLen);
            index+=FrameLen;          //主动抄表PayLoad长度

            PackUpMsgData(iid,iop,index,PayLoad,BLE_APP_NAME,&sendindex);

            TASK_FMT_TRACE( Pipe, REC_LOG,"Sendframe msgindex %d \n",sendindex);
            TASK_BUF_TRACE( Pipe, REC_LOG,FrameBuff, FrameLen);
            taskmng_task_monitor_update(Pipe,CurTask->TaskId,logicid,UDTASK_SENDFRMNUM,NULL);

        }
    }

    return result;
}
/*******************************************************************************
* 函数名称: taskmng_paral_meter_read_data_unpack
* 函数功能: 解上行的645帧
* 输入参数: AppData
* 输出参数: MeterPrtlData
* 返 回 值: 0   含有用帧(数据长度不为0的)
            -1  无
*******************************************************************************/

int bt_paral_meter_read_data_unpack(uint8 Pipe,uint8 seqindex,uint8 *recvbuf,uint16 recvlen, MeterPrtlFmt_T *MeterPrtlData, uint16 *MtrIndex,char *apdu)
{
    int             ret = -1;
    int16          index;
    AppLayerList_t* pAppLayer = NULL;
    OOP_TSA_T tsa;
    
    uint16 pn = gAmrThreadArg[Pipe].AmrSendFrameInfo.Queue[seqindex].Pn;
    if(recvlen==0)
        return -1;
    //645解帧处理
    tsa=taskmng_tsa_get(pn,0);
    ret = taskmng_645_meter_data_unpack(recvlen,recvbuf 
        , MeterPrtlData->MtrAddr, &MeterPrtlData->CtrlCode, &MeterPrtlData->DataLen, MeterPrtlData->DataBuff);

    //如果查找645帧格式失败,那么按照698.45表协议进行解析
    if(CHECK_RESULT_FAILUER == ret)
    {
        ret = LinkDataReceive(oop_pipe_get(Pipe,seqindex), recvbuf, recvlen,&tsa);
        TASK_FMT_TRACE(Pipe, REC_LOG,"LinkDataReceive 698解帧返回值 %d \n",ret);
        if(ret >=eOopComm_Fail)//解帧失败直接返回
        {
            TASK_FMT_DEBUG(Pipe, REC_LOG,"LinkDataReceive 698解帧失败返回值 %d \n",ret);
            return -1;
        }
        //获取电表地址
        pAppLayer = FindAppLayer(oop_pipe_get(Pipe,seqindex), NULL);
        if(pAppLayer == NULL)
            return -1;
        //tsa内改为大端
        memcpy_r(MeterPrtlData->MtrAddr,pAppLayer->sa.add,6);

        MeterPrtlData->DataLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
        memcpy(MeterPrtlData->DataBuff, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, MeterPrtlData->DataLen);
        memcpy(apdu,pAppLayer->recvApduList->apdu,2);

    }

    index = taskmng_ms_index_get( Pipe,pn,0);

    if(index>=0)
        *MtrIndex = index;
    else
        return -1;
   taskmng_mtr_prtl_get(pn,0,&MeterPrtlData->PrtlMode);

    return ret;
}

/*******************************************************************************
* 函数名称: taskmng_data_reading_normal_bt
* 函数功能: 蓝牙日常抄表任务
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int bt_data_reading_normal(uint8 Pipe)
{
    int             ret;
    uint16          i;
    int             RecvLen = 0;                              //接收数据长度
    uint8           PayloadBuf[1000] ;        //payload缓冲区

    AmrThreadArg_T  *pAmrThreadArg;
    MESSAGE_INFO_T      *MsgInfo = NULL;
    uint8           RecvBuf[1000] ;
    //AMR_PLC_INFO_REPORT_T  AmrReportInfo;    //上报的抄读数据
    BLE_MSG_EVENT_T blerecv;
    MeterPrtlFmt_T  MeterPrtlData;
    uint8           SendIndex=0;
    uint16          MtrIndex = 0;      
    char            apdu[2];                                  //apdu的服务类型

    uint8           TaskId;
    uint8 logicid;
    int             OverTimeCnt = 5;  //蓝牙5秒超时
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    BOOL  EventRet = TRUE;
    CTS_RUN_TASK_T *tmpTask = NULL;
    //如果没档案 抄表不运行
    if(pAmrThreadArg->TmnAmrInfo.MtrCnt==0)
        return 1;
    if(pAmrThreadArg->TaskModifyFlag!=TASK_CFG_INITED)
        return 1;//任务未初始化 结束

    ret = taskmng_task_runlist_check(Pipe,&tmpTask);
    if(ret < 0)
    {
        TASK_FMT_DEBUG( Pipe, REC_LOG, "当前无需要抄表任务 结束\n");
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        gFreeFlag[Pipe] = 1;
        pAmrThreadArg->SendQueueFlag = 0;
        return 1; //当前无需要抄表任务 结束
    }

    SCHEME_TASK_RUN_INFO SchemeTaskInfo ;
    //memset(&SchemeTaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
    SCHEME_TASK_RUN_INFO RecvTaskInfo ;
    //memset(&RecvTaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
    CTS_RUN_TASK_T *RecvRunTask = NULL;
    //新一轮任务开始执行时，如果当前正好有这块表在补抄，则无法识别上报的抄表报文是对应哪个的，故而将
    //本轮任务稍微延时，最多延时两分钟
    if(tmpTask->IsNewTurn==1 && TRUE ==taskmng_send_queue_valid_check(Pipe) )
    {
        TASK_FMT_DEBUG( Pipe, REC_LOG, "task %d queue valid! SendQueueFlag %d \n",tmpTask->TaskId,pAmrThreadArg->SendQueueFlag);
        if(pAmrThreadArg->SendQueueFlag<2)
        {
            pAmrThreadArg->SendQueueFlag++;
            return 1;
        }
    }
    
    //获取抄表任务信息
    taskmng_task_info_get(tmpTask->TaskId,tmpTask->logicid,&SchemeTaskInfo);
    
    //如果需要抄的任务和当前的不一致则抄当前优先级更高的admin  
    if(tmpTask->TaskId != pAmrThreadArg->TmnAmrInfo.CurrentTaskId ||tmpTask->logicid!=pAmrThreadArg->TmnAmrInfo.CurrentLogicid)
    {
        TASK_FMT_DEBUG( Pipe, REC_LOG, "taskchange lastid %d now %d  logicid %d \n",pAmrThreadArg->TmnAmrInfo.CurrentTaskId,tmpTask->TaskId,tmpTask->logicid);
        pAmrThreadArg->TmnAmrInfo.CurrentTaskId = tmpTask->TaskId;
        pAmrThreadArg->TmnAmrInfo.CurrentLogicid = tmpTask->logicid;
        //所有表都重新开始抄读
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
    }
    //任务到新一轮 重新抄读
    if(tmpTask->IsNewTurn==1)
    {
        TASK_FMT_DEBUG( Pipe, REC_LOG, "task %d IsNewTurn\n",tmpTask->TaskId);
        tmpTask->IsNewTurn=0;

        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
    }

    switch(pAmrThreadArg->TotalState.NormalAmr)
    {
    case RTS_IDLE:
         for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
         {
            pAmrThreadArg->pMtrAmrInfo[i].CSDIndex = 0;
            pAmrThreadArg->pMtrAmrInfo[i].ROADIndex = 0;
            pAmrThreadArg->pMtrAmrInfo[i].SigFrameNum = 0;
            pAmrThreadArg->pMtrAmrInfo[i].MapIndex = 0;
            pAmrThreadArg->pMtrAmrInfo[i].BroadFailCnt=0;
            if(SchemeTaskInfo.CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
            {
                pAmrThreadArg->pMtrAmrInfo[i].DayFrozenFlag = 0;

            }
            taskmng_task_info_init(Pipe,&SchemeTaskInfo,tmpTask,&pAmrThreadArg->pMtrAmrInfo[i]);
        }
        taskmng_task_record_file_load(Pipe,&SchemeTaskInfo,tmpTask);
        taskmng_comm_recv_buff_clear(Pipe);
        memset(&pAmrThreadArg->AmrSendFrameInfo,0x00,sizeof(SendFrameQueue_T));
        memset(&pAmrThreadArg->meterData,0x00,sizeof(pAmrThreadArg->meterData));
        pAmrThreadArg->AmrSendFrameInfo.usetype = 0;
        pAmrThreadArg->SendQueueFlag = 0;
        if(bt_port_config(Pipe,0)==0)
        {
            pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
        }
        break;
    case RTS_SEND:
        if(bt_paral_amr_frame_make(Pipe,tmpTask,SchemeTaskInfo)==1)
        {
            TASK_FMT_DEBUG( Pipe, REC_LOG, "全部抄完 任务%d 结束&&&&&&&&&&&\n",tmpTask->TaskId);
            pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
            //此处返回2配合taskmng_plc_task_run中 继续执行该函数
            return 2;
        }
        pAmrThreadArg->TotalState.NormalAmr = RTS_RECV;
        break;
    case RTS_RECV:
        RecvLen = taskmng_plc_queue_read(pAmrRecvDown[Pipe], RecvBuf);
        if(RecvLen>0)
        {
            MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
            memcpy(PayloadBuf,MsgInfo->playload,MsgInfo->msglen);
            TASK_FMT_TRACE( Pipe, REC_LOG, "Recv msgindex %d IOP %04X payload \n",MsgInfo->index,MsgInfo->IOP);
        
            TASK_BUF_TRACE( Pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);
            switch (MsgInfo->IOP)
            {

            case 0x0001:  //蓝牙报文通知
            {
                if(ble_msg_event_split(PayloadBuf,MsgInfo->msglen,&blerecv)!=0)
                {
                    TASK_FMT_DEBUG(Pipe, REC_LOG,"ble_msg_event_split error \n");
                    break;
                }
                if(bt_seq_is_valid(Pipe,&SendIndex,blerecv.port)==0)
                {
                    TASK_FMT_DEBUG(Pipe, REC_LOG,"未获取到发送队列索引 \n");
                    break;
                }

                ret = bt_paral_meter_read_data_unpack(Pipe,SendIndex,blerecv.msg,blerecv.len,&MeterPrtlData,&MtrIndex,apdu);
                //TASK_FMT_TRACE(Pipe, REC_LOG,"解帧ret %d 电表号 %d prtl %d \n",ret,AmrReportInfo.nMeterFilesID,MeterPrtlData.PrtlMode);
                if(ret<0)
                {   
                    taskmng_pn_state_refresh(Pipe,SendIndex);
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                    break;
                }
                

                pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastRecvTime = time(NULL);
                taskmng_col_state_write(Pipe,MtrIndex);
                //Pn = AmrReportInfo.nMeterFilesID;
                //TASK_FMT_TRACE( Pipe, REC_LOG,"收到上报数据 taskid %d CurrentTaskId %d \n",pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastTaskId,pAmrThreadArg->TmnAmrInfo.CurrentTaskId);
                TaskId = pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastTaskId;
                logicid = pAmrThreadArg->TmnAmrInfo.CurrentLogicid;
                //如果当前任务已变更 则不处理
                if(TaskId!=pAmrThreadArg->TmnAmrInfo.CurrentTaskId )
                    break;
                taskmng_task_info_get(TaskId,logicid,&RecvTaskInfo);
                taskmng_run_task_get( Pipe,TaskId,logicid,&RecvRunTask); 
                taskmng_task_monitor_update(Pipe,TaskId,logicid,UDTASK_RECVFRMNUM,NULL);
                if(MeterPrtlData.PrtlMode==PRTL_OOP &&ret == eOopComm_SegRecv)
                {
                    TASK_FMT_TRACE( Pipe, REC_LOG,"收到链路分帧数据\n");
                    pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].NeedSendSeg=1;
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                    break;
                }
                if(MeterPrtlData.PrtlMode == PRTL_OOP)
                {
                    if(ret ==eOopComm_GetNext )
                    {
                        TASK_FMT_TRACE( Pipe, REC_LOG,"收到应用分帧数据 继续解析\n");
                        pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].NeedSendSeg=1;
                        taskmng_oop_meter_data_record_save(Pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&RecvTaskInfo,RecvRunTask,NULL,1);
                        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                        break;
                    }
                    //TASK_FMT_TRACE( Pipe, REC_LOG, "面向对象报文解析后DataLen %d \n",MeterPrtlData.DataLen);
                    
                    //TASK_BUF_TRACE( Pipe, REC_LOG,MeterPrtlData.DataBuff, MeterPrtlData.DataLen);
                    //实时数据采用的是GetResponseNormalList数据类型
                    if(apdu[0] == 0x85 && apdu[1] == 0x02)
                    {
                        //printx("返回的服务类型:GetResponseNormalList!\n");
                        //按照GetRequestNormalList数据类型服务进行解析
                        taskmng_oop_meter_data_normal_list_save(Pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&RecvTaskInfo,RecvRunTask);
                    }

                    //采用的是GetRequestRecord数据类型
                    if(apdu[0] == 0x85 && (apdu[1] == 0x03||apdu[1] == 0x05))
                    {

                        taskmng_oop_meter_data_record_save(Pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&RecvTaskInfo,RecvRunTask,NULL,0);
                    }
                }
                else
                {
                    if(ret ==CHECK_RESULT_SUCCEED)
                    {
                        if(taskmng_645_recv_frame_judge(Pipe,MtrIndex,&RecvTaskInfo,MeterPrtlData.DataBuff,MeterPrtlData.CtrlCode,&MeterPrtlData.DataLen)>0)
                        {
                            TASK_FMT_TRACE(Pipe, REC_LOG,"645 DataLen %d\n",MeterPrtlData.DataLen);

                            TASK_BUF_TRACE( Pipe, REC_LOG,MeterPrtlData.DataBuff, MeterPrtlData.DataLen);
                            if(RecvTaskInfo.SchemeType == EVENTPLAN) //事件方案存储
                            {
                                EventRet = taskmng_645_meter_data_save_event(Pipe,&MeterPrtlData.DataBuff[4], MeterPrtlData.DataLen-4, MtrIndex,&RecvTaskInfo,RecvRunTask);
                            }
                            else
                            {
                                if(MeterPrtlData.PrtlMode==PRTL_97)//97表全部以实时数据存储
                                {
                                    taskmng_645_meter_data_save(Pipe,&MeterPrtlData.DataBuff[2], MeterPrtlData.DataLen-2,MtrIndex,&RecvTaskInfo,RecvRunTask,NULL);
                                }
                                else
                                {
                                    if(RecvTaskInfo.AcqInfo.Acqtype ==ACQTYP_NULL)//采集当前数据
                                        taskmng_645_meter_data_save(Pipe,&MeterPrtlData.DataBuff[4], MeterPrtlData.DataLen-4,MtrIndex,&RecvTaskInfo,RecvRunTask,NULL);
                                    else//07表冻结类数据
                                        taskmng_645_meter_data_save_frz(Pipe,&MeterPrtlData.DataBuff[4], MeterPrtlData.DataLen-4,MtrIndex,&RecvTaskInfo,RecvRunTask);
                                }
                            }
                            taskmng_645_map_index_move(Pipe,&RecvTaskInfo, MtrIndex,EventRet);       //移动到下一个数据项
                            taskmng_645_frz_move_judge(Pipe,&RecvTaskInfo, RecvRunTask,MtrIndex,1); 
                        }
                    }
                    else                    
                    {
                        taskmng_645_map_index_move(Pipe,&RecvTaskInfo, MtrIndex,EventRet);       
                        taskmng_645_frz_move_judge(Pipe,&RecvTaskInfo, RecvRunTask,MtrIndex,0); 
                    }
                }

                pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt=0;
                pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].ValidFlag=0;
                if(pAmrThreadArg->AmrSendFrameInfo.QueueCnt>0)
                    pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;
                pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                break;
            }
            default:
                break;
            }
        }
        else
        {
            uint8 flag = 0;
            for(i=0;i<PARALLEL_QUEUE_NUM;i++)
            {
                if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==1)
                {
                    flag =1;
                    if((time(NULL)-pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime)>OverTimeCnt)
                    {
                        TASK_FMT_DEBUG( Pipe, REC_LOG, "Queue index %d 超时 PN %d \n",i,pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn);
                        taskmng_pn_state_refresh( Pipe, i);
                        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                    }
                }
            }
            if(flag==0)
            {
                TASK_FMT_TRACE( Pipe, REC_LOG, "发送队列无效 切换到发送 \n");
                pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
            }
        }
        //pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
        return -1;
        break;
    default:

        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        break;
    }
    return -1;
}
/*******************************************************************************
* 函数名称: taskmng_bt_task_creat
* 函数功能: 任务队列产生判断
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void bt_task_creat(uint8 Pipe)
{
    //AmrThreadArg_T* pAmrThreadArg;

    //pAmrThreadArg = &gAmrThreadArg[Pipe];

    //上电任务
    if(gPowerOnFlag[Pipe]==TRUE)
    {
        gPowerOnFlag[Pipe]=FALSE;

    }

    //每分钟创建正常抄表任务
    if(gPastTimeFlag[Pipe].BitSect.Minute==TRUE)
    {
        //TASK_FMT_TRACE( Pipe, REC_LOG, "过分创建抄表任务 ！\n");
        taskmng_plc_task_queue_add(Pipe,MS_NORMAL_AMR);

    }
//    //穿透任务的产生
//    if(pAmrThreadArg->RelayTrans.isValid ==0)
//    {
//       if(taskmng_relay_exist_check(Pipe)==TRUE)
//       {
//           TASK_FMT_DEBUG( Pipe, RELAY_LOG, "产生穿透任务 ！\n");
//           taskmng_plc_task_queue_add(Pipe,MS_RELAY_TRANS);
//       }
//    }

}


/*******************************************************************************
* 函数名称: BtThread
* 函数功能: 蓝牙抄表线程
* 输入参数：arg                 指向参数区的指针
* 输出参数：无
* 内部处理：抄表任务调度
* 返 回 值: 无
*******************************************************************************/

void* BtThread ( void* arg )
{   

    uint8   CommPipe;
    AmrThreadArg_T* pAmrThreadArg;

    pAmrThreadArg = (AmrThreadArg_T*)arg;
    CommPipe = pAmrThreadArg->CommPipe;


    char ThreadName[20];
    sprintf(ThreadName,"BtThread");
    prctl(PR_SET_NAME,ThreadName);
    taskmng_amr_thread_queue_init ( arg );     //线程队列初始化
    taskmng_past_time_deal_init ( CommPipe );  //时间过处理初始化
    taskmng_all_state_init ( CommPipe );      //状态字初始化
    taskmng_all_flag_init ( CommPipe );       //标志初始化

    memset(pAmrThreadArg->PlcTaskQueue,0x00,PLCTASK_MAXNUM*sizeof(PLCTASK_QUEUE));
    pAmrThreadArg->PlcTask_RunLink = NULL;


    while(1)
    {
        gAmrThreadFlag[CommPipe] = 1;

        
        if(MeterFileAll.FileState!=TASK_CFG_INITED ||pAmrThreadArg->TmnAmrInfo.MtrCnt==0 )
        {
            sleep(3);
            continue;
        }


        taskmng_data_init_check(CommPipe);
        taskmng_time_manage(CommPipe);                    //时间管理函数
        taskmng_task_run_ergodic(CommPipe);   //一秒钟运行一次有可能会出现任务结束时间没判断到的情况
        bt_task_creat(CommPipe);                  //任务的产生判断
        taskmng_plc_task_run(CommPipe);                    //任务的执行处理

        usleep(200000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
    pthread_exit ( NULL );
}



