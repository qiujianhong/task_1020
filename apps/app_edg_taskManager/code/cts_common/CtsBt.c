
#include "CtsAll.h"
#include "ble.h"

#define MAX_BLE_SLAVE_PORT  30   //������ģʽ���30���˿� 2��31
extern uint8 gdelay;

/*******************************************************************************
* ��������: bt_port_config
* ��������: ����ǰ�����¶˿� һ���������30��
* �������: pipe �˿ں�
        msindex ��ʼ���õ�����
* �������: 
* �� �� ֵ: 0                 ���óɹ�
*        -1                ����ʧ��
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
        //�ж��Ƿ񵽴������
        if(i+msindex>=allmeternum)
            break;
        pAmrThreadArg->pMtrAmrInfo[msindex+i].blePort = 2+i;    //�˿ںŴ�2��ʼ
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
            TASK_FMT_TRACE( pipe, REC_LOG,"�����˿����óɹ� \n");
            return 0;
        }
    }
    return -1;
}

/*******************************************************************************
* ��������: bt_seq_is_valid
* ��������: �ж�����֡�Ƿ���Ч֡
* �������: port     �յ��Ķ˿ں�
* �������: OutSeq    �����Ӧ��֡����
* �� �� ֵ: 1         ��Ч
*           0         ��Ч
*******************************************************************************/
uint8 bt_seq_is_valid(uint8 Pipe,uint8* OutSeq, uint16 port)
{
    int i = 0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    for(i = 0; i < 3; i++)
    {
        //����֡�����������Ч֮֡��

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
* ��������: bt_paral_amr_frame_make
* ��������: ���г�����֡
* �������: 
* �������: 
* �� �� ֵ: 1                  ȫ�����
*           0                  ֡δ������ɣ��л�������
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
    uint8        FrameBuff[500]={0};  //֡���ݻ�����
    uint8        PayLoad[500];
    int          MsIndex;    //��ǰ�߳��е�����
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
    //����3������
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
                pAmrThreadArg->pMtrAmrInfo[MsIndex].LastSendTime = time(NULL);//��ǰ������һ�η���ʱ��
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
                //return 1; //���������
            }else     //û��Ҫ���ı���
            {
                result = 0 ;
                continue;
                //return 0;
            }
        }
        else
        {
            //�ж��Ƿ���Ҫ���ͷ�֡ȷ��֡
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
                TASK_FMT_TRACE( Pipe, REC_LOG,"Pn %d ���ͷ�֡ȷ��֡ \n",Pn);
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
            index+=FrameLen;          //��������PayLoad����

            PackUpMsgData(iid,iop,index,PayLoad,BLE_APP_NAME,&sendindex);

            TASK_FMT_TRACE( Pipe, REC_LOG,"Sendframe msgindex %d \n",sendindex);
            TASK_BUF_TRACE( Pipe, REC_LOG,FrameBuff, FrameLen);
            taskmng_task_monitor_update(Pipe,CurTask->TaskId,logicid,UDTASK_SENDFRMNUM,NULL);

        }
    }

    return result;
}
/*******************************************************************************
* ��������: taskmng_paral_meter_read_data_unpack
* ��������: �����е�645֡
* �������: AppData
* �������: MeterPrtlData
* �� �� ֵ: 0   ������֡(���ݳ��Ȳ�Ϊ0��)
            -1  ��
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
    //645��֡����
    tsa=taskmng_tsa_get(pn,0);
    ret = taskmng_645_meter_data_unpack(recvlen,recvbuf 
        , MeterPrtlData->MtrAddr, &MeterPrtlData->CtrlCode, &MeterPrtlData->DataLen, MeterPrtlData->DataBuff);

    //�������645֡��ʽʧ��,��ô����698.45��Э����н���
    if(CHECK_RESULT_FAILUER == ret)
    {
        ret = LinkDataReceive(oop_pipe_get(Pipe,seqindex), recvbuf, recvlen,&tsa);
        TASK_FMT_TRACE(Pipe, REC_LOG,"LinkDataReceive 698��֡����ֵ %d \n",ret);
        if(ret >=eOopComm_Fail)//��֡ʧ��ֱ�ӷ���
        {
            TASK_FMT_DEBUG(Pipe, REC_LOG,"LinkDataReceive 698��֡ʧ�ܷ���ֵ %d \n",ret);
            return -1;
        }
        //��ȡ����ַ
        pAppLayer = FindAppLayer(oop_pipe_get(Pipe,seqindex), NULL);
        if(pAppLayer == NULL)
            return -1;
        //tsa�ڸ�Ϊ���
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
* ��������: taskmng_data_reading_normal_bt
* ��������: �����ճ���������
* �������: ��
* �������: ��
* �� �� ֵ: >0                  �ɹ�
*           <0                  ʧ��
*******************************************************************************/
int bt_data_reading_normal(uint8 Pipe)
{
    int             ret;
    uint16          i;
    int             RecvLen = 0;                              //�������ݳ���
    uint8           PayloadBuf[1000] ;        //payload������

    AmrThreadArg_T  *pAmrThreadArg;
    MESSAGE_INFO_T      *MsgInfo = NULL;
    uint8           RecvBuf[1000] ;
    //AMR_PLC_INFO_REPORT_T  AmrReportInfo;    //�ϱ��ĳ�������
    BLE_MSG_EVENT_T blerecv;
    MeterPrtlFmt_T  MeterPrtlData;
    uint8           SendIndex=0;
    uint16          MtrIndex = 0;      
    char            apdu[2];                                  //apdu�ķ�������

    uint8           TaskId;
    uint8 logicid;
    int             OverTimeCnt = 5;  //����5�볬ʱ
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    BOOL  EventRet = TRUE;
    CTS_RUN_TASK_T *tmpTask = NULL;
    //���û���� ��������
    if(pAmrThreadArg->TmnAmrInfo.MtrCnt==0)
        return 1;
    if(pAmrThreadArg->TaskModifyFlag!=TASK_CFG_INITED)
        return 1;//����δ��ʼ�� ����

    ret = taskmng_task_runlist_check(Pipe,&tmpTask);
    if(ret < 0)
    {
        TASK_FMT_DEBUG( Pipe, REC_LOG, "��ǰ����Ҫ�������� ����\n");
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        gFreeFlag[Pipe] = 1;
        pAmrThreadArg->SendQueueFlag = 0;
        return 1; //��ǰ����Ҫ�������� ����
    }

    SCHEME_TASK_RUN_INFO SchemeTaskInfo ;
    //memset(&SchemeTaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
    SCHEME_TASK_RUN_INFO RecvTaskInfo ;
    //memset(&RecvTaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
    CTS_RUN_TASK_T *RecvRunTask = NULL;
    //��һ������ʼִ��ʱ�������ǰ�����������ڲ��������޷�ʶ���ϱ��ĳ������Ƕ�Ӧ�ĸ��ģ��ʶ���
    //����������΢��ʱ�������ʱ������
    if(tmpTask->IsNewTurn==1 && TRUE ==taskmng_send_queue_valid_check(Pipe) )
    {
        TASK_FMT_DEBUG( Pipe, REC_LOG, "task %d queue valid! SendQueueFlag %d \n",tmpTask->TaskId,pAmrThreadArg->SendQueueFlag);
        if(pAmrThreadArg->SendQueueFlag<2)
        {
            pAmrThreadArg->SendQueueFlag++;
            return 1;
        }
    }
    
    //��ȡ����������Ϣ
    taskmng_task_info_get(tmpTask->TaskId,tmpTask->logicid,&SchemeTaskInfo);
    
    //�����Ҫ��������͵�ǰ�Ĳ�һ���򳭵�ǰ���ȼ����ߵ�admin  
    if(tmpTask->TaskId != pAmrThreadArg->TmnAmrInfo.CurrentTaskId ||tmpTask->logicid!=pAmrThreadArg->TmnAmrInfo.CurrentLogicid)
    {
        TASK_FMT_DEBUG( Pipe, REC_LOG, "taskchange lastid %d now %d  logicid %d \n",pAmrThreadArg->TmnAmrInfo.CurrentTaskId,tmpTask->TaskId,tmpTask->logicid);
        pAmrThreadArg->TmnAmrInfo.CurrentTaskId = tmpTask->TaskId;
        pAmrThreadArg->TmnAmrInfo.CurrentLogicid = tmpTask->logicid;
        //���б����¿�ʼ����
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
    }
    //������һ�� ���³���
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
            TASK_FMT_DEBUG( Pipe, REC_LOG, "ȫ������ ����%d ����&&&&&&&&&&&\n",tmpTask->TaskId);
            pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
            //�˴�����2���taskmng_plc_task_run�� ����ִ�иú���
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

            case 0x0001:  //��������֪ͨ
            {
                if(ble_msg_event_split(PayloadBuf,MsgInfo->msglen,&blerecv)!=0)
                {
                    TASK_FMT_DEBUG(Pipe, REC_LOG,"ble_msg_event_split error \n");
                    break;
                }
                if(bt_seq_is_valid(Pipe,&SendIndex,blerecv.port)==0)
                {
                    TASK_FMT_DEBUG(Pipe, REC_LOG,"δ��ȡ�����Ͷ������� \n");
                    break;
                }

                ret = bt_paral_meter_read_data_unpack(Pipe,SendIndex,blerecv.msg,blerecv.len,&MeterPrtlData,&MtrIndex,apdu);
                //TASK_FMT_TRACE(Pipe, REC_LOG,"��֡ret %d ���� %d prtl %d \n",ret,AmrReportInfo.nMeterFilesID,MeterPrtlData.PrtlMode);
                if(ret<0)
                {   
                    taskmng_pn_state_refresh(Pipe,SendIndex);
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                    break;
                }
                

                pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastRecvTime = time(NULL);
                taskmng_col_state_write(Pipe,MtrIndex);
                //Pn = AmrReportInfo.nMeterFilesID;
                //TASK_FMT_TRACE( Pipe, REC_LOG,"�յ��ϱ����� taskid %d CurrentTaskId %d \n",pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastTaskId,pAmrThreadArg->TmnAmrInfo.CurrentTaskId);
                TaskId = pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastTaskId;
                logicid = pAmrThreadArg->TmnAmrInfo.CurrentLogicid;
                //�����ǰ�����ѱ�� �򲻴���
                if(TaskId!=pAmrThreadArg->TmnAmrInfo.CurrentTaskId )
                    break;
                taskmng_task_info_get(TaskId,logicid,&RecvTaskInfo);
                taskmng_run_task_get( Pipe,TaskId,logicid,&RecvRunTask); 
                taskmng_task_monitor_update(Pipe,TaskId,logicid,UDTASK_RECVFRMNUM,NULL);
                if(MeterPrtlData.PrtlMode==PRTL_OOP &&ret == eOopComm_SegRecv)
                {
                    TASK_FMT_TRACE( Pipe, REC_LOG,"�յ���·��֡����\n");
                    pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].NeedSendSeg=1;
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                    break;
                }
                if(MeterPrtlData.PrtlMode == PRTL_OOP)
                {
                    if(ret ==eOopComm_GetNext )
                    {
                        TASK_FMT_TRACE( Pipe, REC_LOG,"�յ�Ӧ�÷�֡���� ��������\n");
                        pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].NeedSendSeg=1;
                        taskmng_oop_meter_data_record_save(Pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&RecvTaskInfo,RecvRunTask,NULL,1);
                        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                        break;
                    }
                    //TASK_FMT_TRACE( Pipe, REC_LOG, "��������Ľ�����DataLen %d \n",MeterPrtlData.DataLen);
                    
                    //TASK_BUF_TRACE( Pipe, REC_LOG,MeterPrtlData.DataBuff, MeterPrtlData.DataLen);
                    //ʵʱ���ݲ��õ���GetResponseNormalList��������
                    if(apdu[0] == 0x85 && apdu[1] == 0x02)
                    {
                        //printx("���صķ�������:GetResponseNormalList!\n");
                        //����GetRequestNormalList�������ͷ�����н���
                        taskmng_oop_meter_data_normal_list_save(Pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&RecvTaskInfo,RecvRunTask);
                    }

                    //���õ���GetRequestRecord��������
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
                            if(RecvTaskInfo.SchemeType == EVENTPLAN) //�¼������洢
                            {
                                EventRet = taskmng_645_meter_data_save_event(Pipe,&MeterPrtlData.DataBuff[4], MeterPrtlData.DataLen-4, MtrIndex,&RecvTaskInfo,RecvRunTask);
                            }
                            else
                            {
                                if(MeterPrtlData.PrtlMode==PRTL_97)//97��ȫ����ʵʱ���ݴ洢
                                {
                                    taskmng_645_meter_data_save(Pipe,&MeterPrtlData.DataBuff[2], MeterPrtlData.DataLen-2,MtrIndex,&RecvTaskInfo,RecvRunTask,NULL);
                                }
                                else
                                {
                                    if(RecvTaskInfo.AcqInfo.Acqtype ==ACQTYP_NULL)//�ɼ���ǰ����
                                        taskmng_645_meter_data_save(Pipe,&MeterPrtlData.DataBuff[4], MeterPrtlData.DataLen-4,MtrIndex,&RecvTaskInfo,RecvRunTask,NULL);
                                    else//07����������
                                        taskmng_645_meter_data_save_frz(Pipe,&MeterPrtlData.DataBuff[4], MeterPrtlData.DataLen-4,MtrIndex,&RecvTaskInfo,RecvRunTask);
                                }
                            }
                            taskmng_645_map_index_move(Pipe,&RecvTaskInfo, MtrIndex,EventRet);       //�ƶ�����һ��������
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
                        TASK_FMT_DEBUG( Pipe, REC_LOG, "Queue index %d ��ʱ PN %d \n",i,pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn);
                        taskmng_pn_state_refresh( Pipe, i);
                        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                    }
                }
            }
            if(flag==0)
            {
                TASK_FMT_TRACE( Pipe, REC_LOG, "���Ͷ�����Ч �л������� \n");
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
* ��������: taskmng_bt_task_creat
* ��������: ������в����ж�
* �������: 
* �������: ��
* �� �� ֵ: ��
*******************************************************************************/
void bt_task_creat(uint8 Pipe)
{
    //AmrThreadArg_T* pAmrThreadArg;

    //pAmrThreadArg = &gAmrThreadArg[Pipe];

    //�ϵ�����
    if(gPowerOnFlag[Pipe]==TRUE)
    {
        gPowerOnFlag[Pipe]=FALSE;

    }

    //ÿ���Ӵ���������������
    if(gPastTimeFlag[Pipe].BitSect.Minute==TRUE)
    {
        //TASK_FMT_TRACE( Pipe, REC_LOG, "���ִ����������� ��\n");
        taskmng_plc_task_queue_add(Pipe,MS_NORMAL_AMR);

    }
//    //��͸����Ĳ���
//    if(pAmrThreadArg->RelayTrans.isValid ==0)
//    {
//       if(taskmng_relay_exist_check(Pipe)==TRUE)
//       {
//           TASK_FMT_DEBUG( Pipe, RELAY_LOG, "������͸���� ��\n");
//           taskmng_plc_task_queue_add(Pipe,MS_RELAY_TRANS);
//       }
//    }

}


/*******************************************************************************
* ��������: BtThread
* ��������: ���������߳�
* ���������arg                 ָ���������ָ��
* �����������
* �ڲ����������������
* �� �� ֵ: ��
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
    taskmng_amr_thread_queue_init ( arg );     //�̶߳��г�ʼ��
    taskmng_past_time_deal_init ( CommPipe );  //ʱ��������ʼ��
    taskmng_all_state_init ( CommPipe );      //״̬�ֳ�ʼ��
    taskmng_all_flag_init ( CommPipe );       //��־��ʼ��

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
        taskmng_time_manage(CommPipe);                    //ʱ�������
        taskmng_task_run_ergodic(CommPipe);   //һ��������һ���п��ܻ�����������ʱ��û�жϵ������
        bt_task_creat(CommPipe);                  //����Ĳ����ж�
        taskmng_plc_task_run(CommPipe);                    //�����ִ�д���

        usleep(200000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
    pthread_exit ( NULL );
}



