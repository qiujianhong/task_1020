/*-
****************************************************************************************
 * Copyright (c) 2004,�Ͼ��������������������ι�˾
 *            All rights reserved.
 * �������ƣ�MetSTD.c
 * �汾�ţ�1.0
 * ���ܣ��ó�����ɲ������Լ����֡����֡��������ݵĴ洢����
 * �����ˣ��ȱ�
 * ����ʱ�䣺2006��12��22��
 * �޸���:
 * �޸�ʱ��:
 * �޸ļ�Ҫ˵��:
 * ������
*****************************************************************************************
-*/

 #include "../../include/CtsAll.h"
 #include "../../include/metstdcap.h"


extern uint8 ComNo[10];
extern uint8 LocalAddr[6];    //�ն˵�ַ



/****************************************************************************
���ƣ� Prepare_645_07_meter_read
���ܣ�
-           Ϊ645_07��Լ�ĵ��ĳ�����׼�������ִ������Ĳ�������Ϣ
�������̣�
-           
���������
-           TASK_RUN_INFO *pCbTask, uint8* pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara
-       
���������
-           ��
���أ�
-           ��
****************************************************************************/

void Prepare_Cap_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
{


    uint16 sn;
    uint16 MeterIndex;
    OOP_METER_BASIC_T MeterBasicInfo;
    //task_id=pCbTask->TaskId;
    MeterIndex=RunTaskInfo->MeterIndex;
    sn=pCbTask->MsInfo[MeterIndex].Sn;
    //��ȡ��������Ϣ
    memset((void *)MtrPara, 0x0, sizeof(cbMtrPara));

    //��ȡ��������Ϣ
    taskmng_mtr_basic_info_get(sn,RunTaskInfo->logicid,&MeterBasicInfo);
    MtrPara->MtrNum=sn;
    //MtrPara->Speed=4800;
    MtrPara->Speed=4;
    memcpy_r(MtrPara->MetAddr,MeterBasicInfo.tsa.add,6);
    memcpy((uint8 *)&MtrPara->MtrCommAddr,(uint8*)&MeterBasicInfo.tsa,sizeof(OOP_TSA_T));
    memset(Comm_Info_Oop,0,sizeof(S_COMM_INFO_MET));
 
    Comm_Info_Oop->pMtrPara = MtrPara;
    Comm_Info_Oop->MetStat = CB_STATE_INIT;
    //��ȡͨ�Ŷ˿���ϢCOM
    //STD2007_LoadPrifle_Init(&Comm_Info_Oop->cbLPPara,pCbTask,RunTaskInfo);//�����ʵĵط����㲹���ģ�����������������
}

/****************************************************************************
*�������ƣ�Meter_645_07_Read_Item
*���ܣ�  645_07�������ͱ���
*���������
*���������
*���أ�
****************************************************************************/

void Meter_Cap_Read_Item(SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 *DI07Num,uint32 *DI07Buff)
{   

   
    uint16 CsdIndex;
    int ret=0;
    uint8 Data[30]={0};
    uint8 DInum;
  
    if(Comm_Info_Oop->MetStat ==CB_STATE_INIT)
    {
        return;
    }
    CsdIndex=RunTaskInfo->CsdIndex;
    

    if(CsdIndex<pCbTask->CsdTask.RoadNum)
    {
        TASK_FMT_TRACE( pipe, COMM_LOG, "Comm_Info_Oop->mapindex11  is %d,CsdIndex is %d\n",Comm_Info_Oop->mapindex,CsdIndex);
        DInum=GetCapDI(&pCbTask->CsdTask.RoadTask[CsdIndex],Comm_Info_Oop->mapindex);
        if(DInum==0)
        {
            Comm_Info_Oop->mapindex++;
            if(Comm_Info_Oop->mapindex>3)
            {
                Comm_Info_Oop->MetStat = CB_STATE_FINISH;
            }
            return;
        }
        TASK_FMT_TRACE( pipe, COMM_LOG, "Comm_Info_Oop->mapindex  is %d\n",Comm_Info_Oop->mapindex);
        STDCAP_MakeFrameEx(LocalAddr, MtrPara->MetAddr, 0x00, Comm_Info_Oop->mapindex, 0x01, Comm_Info_Oop->pSndBuf, &Comm_Info_Oop->SndLen,Data, 0x00);
        TASK_BUF_TRACE( pipe, COMM_LOG, Comm_Info_Oop->pSndBuf, Comm_Info_Oop->SndLen);
        ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,MtrPara->Speed,PARITY_EVEN,Comm_Info_Oop->pSndBuf, (int32)Comm_Info_Oop->SndLen);
        if(ret !=0)
        {
            TASK_FMT_TRACE( pipe, COMM_LOG, "����ʧ�� ret is %d\n",ret);
        }
        TASK_BUF_TRACE( pipe, COMM_LOG, Comm_Info_Oop->pSndBuf, Comm_Info_Oop->SndLen);
        Comm_Info_Oop->MetStat = CB_STATE_RECV;         //���ͳɹ���׼������
    }
    else
    {
        Comm_Info_Oop->MetStat = CB_STATE_FINISH;
    }
    if(Comm_Info_Oop->MetStat != CB_STATE_RECV )//0xff  �����������Ѿ����겻�ٴ��ڽ���״̬
    {
        Comm_Info_Oop->MetStat = CB_STATE_FINISH;       //Ĭ��Ϊ����״̬��ֻ���ڲ��ҵ����ʵ���������ת�����״̬�������������
    }
}


/****************************************************************************
*�������ƣ�Deal_Receive_Result_645_07
*���ܣ�����  645_07�������ձ��ĵĽ��
*���������
*���������
*���أ�
****************************************************************************/

void Meter_Cap_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff)
{
    uint8               *ptr=NULL;
    
   

   
    OOP_OAD_U mainOAD;
    
    mainOAD.value = 0x00;

    memset(Comm_Info_Oop->pRcvBuf, 0x00, sizeof(Comm_Info_Oop->pRcvBuf));


    Comm_Info_Oop->RcvLen = rs485_RS232_recv(pipe,Comm_Info_Oop->pRcvBuf, sizeof(Comm_Info_Oop->pRcvBuf));
    
    TASK_BUF_TRACE( pipe, COMM_LOG,Comm_Info_Oop->pRcvBuf, Comm_Info_Oop->RcvLen);
    
    
    if(Comm_Info_Oop->RcvLen > 9)   //��Ч���ĵ���С����Ϊ9���ֽ�
    {       
        ptr = (uint8*)commfun_memchr_cap((char*)Comm_Info_Oop->pRcvBuf, 0x68, Comm_Info_Oop->RcvLen);//�ҵ�645���ĵ�68ͷ
    }
    TASK_FMT_TRACE( pipe, COMM_LOG, "ptr is %p,Comm_Info_Oop->pRcvBuf is %p\n",ptr,Comm_Info_Oop->pRcvBuf);
    if(Comm_Info_Oop->RcvLen <= 0 || ptr == NULL)
    {
        Comm_Info_Oop->MetStat =  CB_STATE_FINISH;
        return ;
    }
    else
    {
        Comm_Info_Oop->RcvLen -= (ptr - Comm_Info_Oop->pRcvBuf);//�����������ĳ���
        TASK_FMT_TRACE( pipe, COMM_LOG, "Comm_Info_Oop->RcvLen is %d\n",Comm_Info_Oop->RcvLen);
        memmove(Comm_Info_Oop->pRcvBuf, ptr, Comm_Info_Oop->RcvLen);

        TASK_BUF_TRACE( pipe, COMM_LOG, Comm_Info_Oop->pRcvBuf, Comm_Info_Oop->RcvLen);

//        if(STDCAP_CheckRspFrame(pipe,Comm_Info_Oop->pRcvBuf,Comm_Info_Oop->RcvLen) !=0)
//        {
//             Comm_Info_Oop->MetStat =  CB_STATE_FINISH;
//             return;
//        }
        STDCAP_AUTK_Recv(pipe,mainOAD,Comm_Info_Oop,Comm_Info_Oop->pRcvBuf+14, (uint16)Comm_Info_Oop->RcvLen-14-2, *MtrPara,NULL, pCbTask,RunTaskInfo);
                
    }
    //Comm_Info_Oop->MetStat =  CB_STATE_FINISH;
    return;
}


/****************************************************************************
*ģ����?
*���ƣ����ݽ��յ��ı��ģ��ж�֡����Ч��
*���ܣ������Լ����жϷ���֡����Ч��
*���������ͨ���ţ�pipe������������ṹ��(*pTaskAllot)�����յ����Ӧ������ݻ�����ָ��(*pBuf)�����ĳ���(rcvLen)
*�����������
*���� ������ֽ��Ƿ����ͬ��ͷ����0x68��
*       �����߸��ֽ��Ƿ�Ҳ����ͬ��ͷ����0x68��
*   ������һ���ֽ��Ƿ���ڽ����루���ݱ����ڵı��ĳ��ȣ���ȡ�����һ���ֽڣ�����0x16��
*   ��ⱨ�ĵ�У�����Ƿ�������ڶ����ֽڣ�
*   ��ⱨ���еı��ַ�Ƿ����pTaskAllot.MetAddr[]��
*       ��������У�鱨���Ƿ�ɹ����֡�
*   ���ݿ����루���ĵ�8�ֽڣ����жϴ�վ�Ƿ�����Ӧ���Ƿ��к���֡��
*   ���������D6λΪ1����ʾ��վ�쳣Ӧ��
*   ���������D6λΪ0����ʾ��վ����Ӧ��
*   ���������D5λΪ0����ʾ�޺���֡��
*   ���������D5λΪ1����ʾ�к���֡��
*���أ� -0У��ʧ��-
*       -1У��ɹ��޺�������֡-
*       -2У��ɹ��к�������֡-
*       -3У��ɹ�����Ӧ��֡-
*       -4У��ɹ��쳣Ӧ��֡-
*       -5У��ɹ�����Ӧ��֡-
****************************************************************************/

int STDCAP_CheckRspFrame(uint8 pipe, uint8 *pBuf, uint16 rcvLen)
{
    if(memcmp(pBuf+5,LocalAddr,4)!=0)
    {
        return -1;
    }
    if(*(pBuf+10) !=0x00)
    {
         return -2;
    }
    return 0;
}


/****************************************************************************
*ģ���ţ�7-9-1-1
*���ƣ���֯����������֡
*���ܣ������Լ�������֡
*�������������ַpMetAddr�����ݱ�ʶfraDI�������ʶcmdDI��֡��Ż�������ַpBuf��֡����ŵ�ַpSndLen
                  ���ݲɼ��������¼������ݱ�ʶ�룬
*�����������֡��Ļ�������֡��


*�������ݵ���Լ��֡
*���أ���
****************************************************************************/
void STDCAP_MakeFrameEx(uint8 *SourAddr, uint8 *DestAddr, uint8 S_Type, uint8 Command, uint8 CmdType, uint8 *pBuf, uint8 *pSndLen,uint8 *Data, uint8 DataLen)
{
    uint8 tmpSnd[80];
    uint8 offset=0;
    uint16 tmpoffset1=0;
    uint16 tmpoffset2=0;
    uint16 tmpoffset3=0;
    
    memset(tmpSnd, 0x00, sizeof(tmpSnd));

    tmpSnd[offset++] = 0x68;
    memcpy(&tmpSnd[offset], SourAddr, 4); /*- Դ��ַ -*/
    offset=offset+4;
    memcpy(&tmpSnd[offset], DestAddr, 4); /*- Դ��ַ -*/
    offset=offset+4;
    tmpoffset1 = offset;//����֡ͷ��λ��
    tmpSnd[offset++] = 0x68;
    tmpSnd[offset++] = S_Type;  /*- Դ�豸���� -*/
    tmpoffset2 = offset++;    /*- ���ݳ��ȣ���֡�����ݳ��� -*/
   
    tmpSnd[offset++] = CmdType;   /*- �������� -*/
    tmpSnd[offset++] = Command;   /*- �������� -*/
    if(DataLen !=0)
    {
        memcpy(&tmpSnd[offset],Data,DataLen);//������
        offset =offset+DataLen;
    }
    tmpoffset3 = offset;
    TASK_BUF_TRACE( 2, COMM_LOG, tmpSnd, offset);
    TASK_FMT_TRACE( 2, COMM_LOG,"tmpoffset1 is %d,tmpoffset2 is %d,tmpoffset3 is %d\n",tmpoffset1,tmpoffset2,tmpoffset3);
    tmpSnd[tmpoffset2] = tmpoffset3-tmpoffset2-1;
    tmpSnd[offset++] = CalCs(&tmpSnd[tmpoffset1], tmpoffset3-tmpoffset1);
    tmpSnd[offset++] = 0x16;

    memcpy(pBuf,tmpSnd,offset);
    *pSndLen = offset;
   return;
}

/****************************************************************************
���ƣ� STDCAP_AUTK_Recv
���ܣ�
-           ���������Լ����ر���
�������̣�
-           1���������������ж��Ǵ������ֵ����������վ���õ�����
-           2��
-           3��
���������
-           pipe                ͨ����
-           pBuf                ���յ��ĵ����뱨��
-           rcvLen              ���յĳ���
-           mCbMtrPara          ��ǰ����Ĳ��������Ϣ
-           pCbTaskID           �����������
-           bStdType            ���Լ����
���������
-           ��
���أ�
-           �Ƿ�������֡
****************************************************************************/
uint16 STDCAP_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,S_COMM_INFO_MET *Comm_Info_Oop,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{
    uint16               offSet=0;
    MapOopToCap tmppMapOopToCap;

    OOP_OAD_U                 subOAD;
    
    uint8 i=0;
    DataSource sourcedata;
    uint8 desbuffer[2000];
    uint16 offset=0;
    uint16 desLen=0, MeterIndex,pn,task_id;
 
    DateTimeBcd_t taskStoreTime;
    DateTimeBcd_t taskStartTime;
    time_t longTm;
    time_t colStartTm;
  
    uint16 MsIndex;


    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    task_id=pCbTask->TaskId;


    TASK_FMT_TRACE( pipe, COMM_LOG,"pCbTask->CsdTask.RoadTask[0].SubOadNum is %d\n",pCbTask->CsdTask.RoadTask[0].SubOadNum);
    for(i=0;i<pCbTask->CsdTask.RoadTask[0].SubOadNum;i++)
    {
        subOAD = pCbTask->CsdTask.RoadTask[0].OadListInfo[i];
        TASK_FMT_TRACE( pipe, COMM_LOG,"subOAD.value is %x\n",subOAD.value);
        offset = CaclCapBiasByOad(subOAD,&tmppMapOopToCap,Comm_Info_Oop->mapindex);
        TASK_FMT_TRACE( pipe, COMM_LOG,"offset is %x\n",offset);
        if(offset==0xFFFF)
        {
            continue;
        }
        Comm_Info_Oop->SucOADNum++;
        TASK_FMT_TRACE( pipe, COMM_LOG,"offset is %d,tmppMapOopToCap->TotalDataLen is %d\n",offset,tmppMapOopToCap.TotalDataLen);
        memcpy(sourcedata.sourcebuffer,pBuf+offset,tmppMapOopToCap.TotalDataLen);
        sourcedata.reclen=tmppMapOopToCap.TotalDataLen;
        Convert_Cap_To_Struct(sourcedata ,desbuffer,&desLen,&tmppMapOopToCap);
        //����ת��

        //frame_printf(desbuffer,desLen);
        //����ɼ�������

        memcpy_r((uint8*)&taskStartTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);//�ɼ�����ʱ��

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
            DT_DateTimeBCD2Time(&taskStoreTime,&longTm);//
            if(pCbTask->StoreTimeType == STRTMTYP_FRZTIME)
            {
                if(mainOAD.nObjID == 0x5002)
                {
                    OOP_TI_T rollTi;
                    long tisec = 0;
                    get_task_run_frequence(task_id,&rollTi);
                    tisec = DT_TI2Seconds(rollTi);
                    storeTm = storeTm - (storeTm%tisec);
                  
                }
                longTm=storeTm;
            }
        }
        else
        {
            DT_DateTimeBCD2Time(StoreTime,&longTm);
        }
        taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
        taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);


        //taskmng_meter_data_save(desbuffer,desLen,StoreInfo,pipe);   
        taskmng_cache_buf_save(pipe,pn,mainOAD,subOAD,desbuffer,desLen,colStartTm,longTm);

    }
    Comm_Info_Oop->mapindex++;//ת����һ������
    TASK_FMT_TRACE ( pipe,COMM_LOG, "��һ������ Comm_Info_Oop->mapindex %d,Comm_Info_Oop->SucOADNum is %d,pCbTask->CsdTask.RoadTask[0].SubOadNum is %d\n",Comm_Info_Oop->mapindex,Comm_Info_Oop->SucOADNum,pCbTask->CsdTask.RoadTask[0].SubOadNum);
    if(Comm_Info_Oop->SucOADNum>=pCbTask->CsdTask.RoadTask[0].SubOadNum)
    {
        Comm_Info_Oop->MetStat=CB_STATE_FINISH;
    }
    else
    {
        Comm_Info_Oop->MetStat=CB_STATE_SEND;
    }
    return offSet;
}







