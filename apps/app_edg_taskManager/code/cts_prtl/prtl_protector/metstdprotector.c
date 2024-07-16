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
#include "../../include/metstdprotector.h"




/*
*****************************************************************************************
*- ���������ID�궨������ -
*****************************************************************************************
*/
#define STD2007_CMDID_READ          0x11    /*-������-*/
#define STD2007_CMDID_READNEXT      0x12    /*-����������-*/
#define STD2007_CMDID_REREAD        0x13    /*-�ض�����-*/
#define STD2007_CMDID_WRITE         0x14    /*-д����-*/
#define STD2007_CMDID_SETTIME       0x08    /*-�㲥Уʱ(YYMMDDhhmmss)-*/
#define STD2007_CMDID_SETADDR       0x0A    /*-д�豸��ַ(�ù㲥��ַ�����µ�ַ)-*/
#define STD2007_CMDID_SETRATE       0x0C    /*-����ͨ�Ų�����-*/
#define STD2007_CMDID_MDFPSW        0x0F    /*-�޸�����-*/
#define STD2007_CMDID_CLRMAXAD      0x10    /*-�����������-*/

/*
*****************************************************************************************
*-����-
*****************************************************************************************
*/
#define STD2007_FILL_CODE_DD        0xDD    /*-0xAA+0x33�����ݻ�������������ַ�(����û��)-*/
#define STD2007_FILL_CODE_EE        0xEE    /*-0xBB+0x33�����ݻ���ֽ�����ַ�(С��û��)-*/
//static uint8 STD_BROADCAST_ADDRESS[6] = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99}; /*-�㲥��ַ-*/
#define STD2007_GAP_VALUE           0x33    /*-������DATA����ʱ+33H����ʱ-33H-*/
#define STD2007_ADDR_SITE           14

#define STD2007_SYNC                0x68
#define POINTDAY_BC_NUM             3 //�ն������ݲ�������



extern uint8 ComNo[10];
extern uint8 STD2007_CheckRspFrame(uint8 pipe, /*cbTaskAllot_T * pTaskAllot,*/ uint8 * pBuf, uint16 rcvLen, uint8 bStdType,S_COMM_INFO_MET* pCommInfo);

extern void STD2007_MakeFrame(uint8 *pMetAddr, uint32 fraDI, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, uint8 bStdType);
extern cbMetPara STD2007Para;
extern void STD2007_LoadPrifle_Init(cbLPPara_t *pCbLpPara ,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T   *RunTaskInfo);
extern uint8 STD2007_CheckRspFrame(uint8 pipe, uint8 *pBuf, uint16 rcvLen, uint8 bStdType,S_COMM_INFO_MET* pCommInfo);
extern void  STD07_ToNextReadItem(BOOL NextCSDFlg,S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
extern uint8 STD2007_CheckRspFrame(uint8 pipe, uint8 *pBuf, uint16 rcvLen, uint8 bStdType,S_COMM_INFO_MET* pCommInfo);
extern void ReduceGap(uint8 *pRecvBuf, uint8 offs, uint8  gapValue);



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

void Prepare_Protector_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
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
    //MtrPara->Speed=get_meter_read_speed(MeterBasicInfo.baud);
    MtrPara->Speed=MeterBasicInfo.baud;
    memcpy_r(MtrPara->MetAddr,MeterBasicInfo.tsa.add,6);
    memcpy((uint8 *)&MtrPara->MtrCommAddr,(uint8*)&MeterBasicInfo.tsa,sizeof(OOP_TSA_T));
    memset(Comm_Info_Oop,0,sizeof(S_COMM_INFO_MET));
 
    Comm_Info_Oop->pMtrPara = MtrPara;
    Comm_Info_Oop->MetStat = CB_STATE_INIT;
    //��ȡͨ�Ŷ˿���ϢCOM
    STD2007_LoadPrifle_Init(&Comm_Info_Oop->cbLPPara,pCbTask,RunTaskInfo);//�����ʵĵط����㲹���ģ�����������������
}

/****************************************************************************
*�������ƣ�Meter_645_07_Read_Item
*���ܣ�  645_07�������ͱ���
*���������
*���������
*���أ�
****************************************************************************/

void Meter_Protector_Read_Item(SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 *DI07Num,uint32 *DI07Buff)
{   

    uint32              DI07 = 0;
    uint16 CsdIndex,SubOadIndex,MeterIndex,pn,tmpMsIndex;
    int ret=0;
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);

    //Cheeck_Cb_Pipe(pCbTask,Comm_Info_Oop,MtrPara,pipe,lstTime);
    ExecMeter485Relay(pipe,&Comm_Info_Oop->MetStat,pCbTask->Priority);
    if(Comm_Info_Oop->MetStat ==CB_STATE_INIT)
    {
        return;
    }
    CsdIndex=RunTaskInfo->CsdIndex;
    SubOadIndex=RunTaskInfo->SubOadIndex;

    if(CsdIndex<pCbTask->CsdTask.RoadNum)
    {
        *DI07Num = GetDI07_485(pCbTask->AcqInfo,&pCbTask->CsdTask.RoadTask[CsdIndex],SubOadIndex,Comm_Info_Oop->cbLPPara.endNum,DI07Buff,pCbTask,RunTaskInfo,pipe);//��ȡ��������ݱ�ʶ

        if(*DI07Num == 0)
        {
            STD07_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);
            return;
        }
        else
        {
            DI07 = DI07Buff[*DI07Index];

            STD2007_MakeFrameEx(MtrPara->MetAddr,DI07, STD2007_CMDID_READ, Comm_Info_Oop->pSndBuf, &Comm_Info_Oop->SndLen, Comm_Info_Oop,pCbTask,RunTaskInfo);
        }

        ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,MtrPara->Speed,PARITY_EVEN,Comm_Info_Oop->pSndBuf, (int32)Comm_Info_Oop->SndLen);
        if(ret !=0)
        {
            TASK_FMT_TRACE( pipe, COMM_LOG, "����ʧ�� ret is %d\n",ret);
        }
        TASK_BUF_TRACE( pipe, COMM_LOG, Comm_Info_Oop->pSndBuf, Comm_Info_Oop->SndLen);
        Comm_Info_Oop->MetStat = CB_STATE_RECV;         //���ͳɹ���׼������
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_SENDFRMNUM,NULL);
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastSendTime=time(NULL);
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

CB_STATE Deal_Receive_Result_Protector(S_COMM_INFO_MET *Comm_Info_Oop,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,BOOL actResult,uint8 pipe)
{
    uint16 pn,tmpMsIndex,MeterIndex;
    
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);


    eOopCommRslt_t ret = 0;
    if(actResult)                          //��֡������ȷ
    {

        STD07_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);
        Comm_Info_Oop->ComFrame++;          //��֡������
    
        Comm_Info_Oop->ErrFrame = 0;        //ʧ��֡���ʹ���֡�����㣬׼�����¼���
        Comm_Info_Oop->ErrTimes = 0;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt = 0;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = TRUE;
    }
    else                                            //��֡����ʧ��
    {
        TASK_FMT_TRACE( pipe, COMM_LOG,"ERR_FRAME_MAX is %d,ERR_TIMES_MAX is %d\n", ERR_FRAME_MAX,ERR_TIMES_MAX);
        if(Comm_Info_Oop->ErrTimes >= 0)        //ͬһ֡��ʧ�ܴ���
        {

            if(Comm_Info_Oop->ErrFrame >= 10)      //ʧ�ܴ������࣬���ټ�����׼���˳�
            {
                Comm_Info_Oop->ErrFrame = ERR_FRAME_MAX;
                Comm_Info_Oop->MetStat = CB_STATE_FINISH;
                return Comm_Info_Oop->MetStat;
            }
            else
            {
                if(0 == ret)                                                  //û�н��յ�����
                {
                Comm_Info_Oop->ErrFrame++;                        //ʧ��֡������
                Comm_Info_Oop->ComFrame++;                        //��֡������
                }

                Comm_Info_Oop->ErrTimes = 0;                         //���Դ�������
                STD07_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);//�������һ��������ط��ϣ��������������б�
            }
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = FALSE;
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt++;
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastFailTime=time(NULL);
        }
        else
        {
            if(ret==0)                                                           //û�н�������
            {
                Comm_Info_Oop->ErrTimes++;                              //�ط���ǰ������
            }
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = FALSE;
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt++;
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastFailTime=time(NULL);
        }
    }
    Comm_Info_Oop->MetStat = CB_STATE_SEND;                    //ת�뷢������
    return Comm_Info_Oop->MetStat;
}



/****************************************************************************
*�������ƣ�Deal_Receive_Result_645_07
*���ܣ�����  645_07�������ձ��ĵĽ��
*���������
*���������
*���أ�
****************************************************************************/

void Protector_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff)
{
    uint8               *ptr=NULL;
    BOOL        actResult = TRUE;       //�Ի��봦��Ľ��
    eOopCommRslt_t ret = 0;

    uint32              DI07 = 0;                                   //���ݱ�ʶ

    uint8 CsdIndex,SubOadIndex;
    uint16 MeterIndex,pn,tmpMsIndex;
    uint16 reallen=0;
    
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);

    SubOadIndex=RunTaskInfo->SubOadIndex;
    CsdIndex=RunTaskInfo->CsdIndex;

    memset(Comm_Info_Oop->pRcvBuf, 0x00, sizeof(Comm_Info_Oop->pRcvBuf));


    Comm_Info_Oop->RcvLen = rs485_RS232_recv(pipe,Comm_Info_Oop->pRcvBuf, sizeof(Comm_Info_Oop->pRcvBuf));
    
    TASK_BUF_TRACE( pipe, COMM_LOG,Comm_Info_Oop->pRcvBuf, Comm_Info_Oop->RcvLen);
    
    
    if(Comm_Info_Oop->RcvLen > 9)   //��Ч���ĵ���С����Ϊ9���ֽ�
    {
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastRecvTime=time(NULL);
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_RECVFRMNUM,NULL);
        ptr = (uint8*)commfun_645_frame_get((char*)Comm_Info_Oop->pRcvBuf, STD2007Para.Sync, Comm_Info_Oop->RcvLen,&reallen);//�ҵ�645���ĵ�68ͷ
    }
    if(Comm_Info_Oop->RcvLen <= 0 || ptr == NULL)
    {
        actResult = FALSE;              //���û�л��� ���� �����ʽ�Ƿ�
    }
    else
    {

        Comm_Info_Oop->RcvLen = reallen;//��645���ĳ���

        memmove(Comm_Info_Oop->pRcvBuf, ptr, Comm_Info_Oop->RcvLen);
        ret = STD2007_CheckRspFrame(pipe, Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, 0,Comm_Info_Oop);

        if((ret == 1)||(ret == 2))    //��Ч,�������ݷ���
        {
            #if 1
            Comm_Info_Oop->ErrFrame=0;
            OOP_OAD_U mainOad;
            if(DI07Num == 1)//�����ڶ��������,һ֡������
            {
                if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0)
                {

                    memset(&mainOad,0x00,sizeof(OOP_OAD_U));
                    Protector_AUTK_Recv(pipe,mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],Comm_Info_Oop->pRcvBuf+14, (uint16)Comm_Info_Oop->RcvLen-14-2, *MtrPara,NULL, pCbTask,RunTaskInfo);
                }
                else
                {

                    mainOad = pCbTask->CsdTask.RoadTask[CsdIndex].MainOad;
                    if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
                    {
                      
                        Protector_AUTK_Recv(pipe,mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],Comm_Info_Oop->pRcvBuf+14, (uint16)Comm_Info_Oop->RcvLen-14-2, *MtrPara, NULL,pCbTask,RunTaskInfo);
                    }
                    else
                    {
                        if(mainOad.nObjID == 0x5002)
                        {
                            {
                                STDProtector_AUTK_Recv_Profile(pipe,mainOad,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen-2, *MtrPara, pCbTask,RunTaskInfo);
                            }
                        }
                        else if((mainOad.nObjID & 0xF000)== 0x3000)
                            STDProtector_AUTK_Recv_Event(pipe,mainOad,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen-2, *MtrPara, Comm_Info_Oop,pCbTask,DI07Index,DI07Num,RunTaskInfo);
                        else
                            STDProtector_AUTK_Recv_Frz(pipe,mainOad,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen-2, *MtrPara, Comm_Info_Oop,pCbTask,RunTaskInfo);
                    }
                }
                actResult = TRUE;
            }
            else//��Ҫ��֡��
            {
                DI07 = DI07Buff[*DI07Index];
                if(*DI07Index<DI07Num)//OAD ��Ӧ��07���ݱ�ʶ��û��ͨѶ��
                {

                    if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0)
                    {
                        memset(&mainOad,0x00,sizeof(OOP_OAD_U));
                        STD2007_FrameCombin(mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],DI07,*DI07Index,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, CombinBuff, CombinLen);
                    }
                    else
                    {
                        if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
                        memset(&mainOad,0x00,sizeof(OOP_OAD_U));
                        else
                        mainOad = pCbTask->CsdTask.RoadTask[CsdIndex].MainOad;
                        if((mainOad.nObjID & 0xF000)== 0x3000)
                        STDProtector_AUTK_Recv_Event(pipe,mainOad,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen-2, *MtrPara, Comm_Info_Oop,pCbTask,DI07Index,DI07Num,RunTaskInfo);
                        else
                        STD2007_FrameCombin(mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],DI07,*DI07Index,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, CombinBuff, CombinLen);
                    }
                    (*DI07Index)++;
                    if(*DI07Index>=DI07Num)
                    {
                        *DI07Index = 0;
                        if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x0000)
                        {
                            memset(&mainOad,0x00,sizeof(OOP_OAD_U));
                            Protector_AUTK_Recv(pipe,mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],CombinBuff, *CombinLen, *MtrPara,NULL, pCbTask,RunTaskInfo);
                        }
                        else
                        {
                            if(mainOad.nObjID == 0x5002)
                                STDProtector_AUTK_Recv_Profile(pipe,mainOad,CombinBuff, *CombinLen, *MtrPara, pCbTask,RunTaskInfo);
                            else if((mainOad.nObjID & 0xF000)== 0x3000)
                            {
                            }
                            else
                                STDProtector_AUTK_Recv_Frz(pipe,mainOad,CombinBuff, *CombinLen, *MtrPara, Comm_Info_Oop,pCbTask,RunTaskInfo);
                        }
                    }
                    else
                    {
                        Comm_Info_Oop->MetStat = CB_STATE_SEND;
                        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_RECVFRMNUM,NULL);
                        return;
                    }
                }
                actResult = TRUE;
            }
            #endif
        }
        else if(ret == 3 || ret == 5) //����Ч���ݣ���ֵ�������Ĳ�һ��
        {
            Comm_Info_Oop->ErrFrame=0;
            Comm_Info_Oop->ErrTimes = ERR_TIMES_MAX;        //��ʧ�ܴ��������������ԣ�ֱ�ӽ����Դ�����Ϊ���ֵ
            actResult = FALSE;
        }
        else if(ret == 4)                 //����֡
        {
            Comm_Info_Oop->ErrFrame = 0;
            Comm_Info_Oop->ErrTimes = ERR_TIMES_MAX;             //����Ƿ���֡���������ԣ�ֱ�ӽ����Դ�����Ϊ���ֵ
            actResult = FALSE;
        }
        else                                 //������Ч������
        {
            actResult = FALSE;
        }
    }
    Deal_Receive_Result_Protector(Comm_Info_Oop,pCbTask,RunTaskInfo,actResult,pipe);
}

/****************************************************************************
���ƣ� Protector_AUTK_Recv
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

uint16 Protector_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{
    uint16               offSet=0;
    OadInfoMap       pOadInfo;
    const  MapOopTo645_NEW *pMapOopTo645;

    uint8                tmpBufer[255];           //Э��ת������ʱ��255���ֽ�
    OOP_OAD_U                 findOAD;
    OOP_OAD_U                 tmpOad;

    //BOOL result=FALSE;
    DataSource sourcedata;
    uint8 desbuffer[2000];;
    uint16 desLen=0, MeterIndex,pn,task_id;
    OadStoreInfo StoreInfo;
    DateTimeBcd_t taskStoreTime;
    DateTimeBcd_t taskStartTime;
    time_t longTm;
    time_t colStartTm;
    uint8 MtrAddr[6];
    uint16 MsIndex;
    

    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    task_id=pCbTask->TaskId;

    findOAD = subOAD;

    memset((uint8*)&StoreInfo,0x00,sizeof(OadStoreInfo));
    if(pCbTask->TaskId>255)
    {
        //�ڲ�������
        //������

    }
    memset(tmpBufer,0x00,255);
    memcpy(tmpBufer,pBuf,rcvLen);
    if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
    {
        tmpOad.nObjID = 0x0000;
        tmpOad.attID = 0x00;
        tmpOad.nIndex = 0x00;
        pMapOopTo645 = Find645InfoPtrByOad(tmpOad,subOAD);//
    }
    else
    {
        pMapOopTo645 = Find645InfoPtrByOad(mainOAD,subOAD);//
    }
    if(pMapOopTo645==NULL)
    {
            return 0;
    }
    if(FALSE==taskmng_oad_info_get(findOAD, &pOadInfo))
    {
        return 0;
    }
    if((pMapOopTo645->DI07 & 0xFF00FF00) == 0x05001000)
    {
        //����ı�������
        if(subOAD.nObjID == 0x2004 || subOAD.nObjID == 0x2005)
        {
            if(subOAD.nObjID == 0x2005)
                offSet = rcvLen / 2;
        }
    }
    //������-0x33
    ReduceGap(tmpBufer, rcvLen, STD2007_GAP_VALUE);
    
    //����ת��
    memcpy(sourcedata.sourcebuffer,tmpBufer,rcvLen);
    sourcedata.reclen=rcvLen;
    Convert_Protector_To_Struct(sourcedata ,desbuffer,&desLen,pMapOopTo645);

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
    taskmng_mtr_addr_get(pn,RunTaskInfo->logicid,MtrAddr);
    memcpy(StoreInfo.MAC.nValue,MtrAddr,6);
    StoreInfo.recordOAD.infoNum=pn;
    StoreInfo.recordOAD.logicId=RunTaskInfo->logicid;

    StoreInfo.recordOAD.road.oadMain=mainOAD;
    StoreInfo.recordOAD.road.oadCols.nNum=1;
    StoreInfo.recordOAD.road.oadCols.oad[0]=subOAD;
    StoreInfo.recordOAD.road.oadCols.rsv=0;
    
    StoreInfo.recordOAD.classtag = 5;
    StoreInfo.colStartTm=colStartTm;
    StoreInfo.colStoreTm=longTm;
    StoreInfo.colEndTm=time(NULL);
    TASK_FMT_TRACE ( pipe,COMM_LOG, "r�����:pCbTask->TaskId is %d\n",pCbTask->TaskId);
    TASK_FMT_TRACE ( pipe,COMM_LOG, "subOAD is %x\n",subOAD.value);
    TASK_BUF_TRACE( pipe,COMM_LOG, desbuffer,desLen);
    taskmng_meter_data_save(desbuffer,desLen,StoreInfo,pipe);   

    

    return offSet;
}

/****************************************************************************
���ƣ� STD64507_AUTK_Recv_Profile
���ܣ�
-           64507�������߽����������ղɼ�������������
�������̣�
?���������
-           pipe                ͨ����
-           pBuf                ���յ��ĵ����뱨��
-           rcvLen              ���յĳ���
-           mCbMtrPara          ��ǰ����Ĳ��������Ϣ
-           pCbTask         ��ǰ������Ϣ
���������
-           ��
���أ�
-           0:���ݽ�������
****************************************************************************/
uint16 STDProtector_AUTK_Recv_Profile(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{
    uint8 offSet=14;
    uint8 profileBuff[5+17+24+8+16+16+6];
    uint16 CsdIndex; 
    #if(1 == OOP_CRV_ADDREAD_485)
    uint16 MeterIndex;
    uint16 Pn;
    #endif
   
    CsdIndex=RunTaskInfo->CsdIndex;
    #if(1 == OOP_CRV_ADDREAD_485)
    MeterIndex=RunTaskInfo->MeterIndex;
    Pn=pCbTask->MsInfo[MeterIndex].Sn;
    #endif
   
    memset(profileBuff,0x00,sizeof(profileBuff));
    //��鸺�����߸�ʽ�Ƿ���ȷ
    do
    {
        offSet = 14;
        if(pBuf[offSet++]!=0xD3 || pBuf[offSet++]!=0xD3)
            break;
        //if(pBuf[offSet++]!=0x5D)
        //  break;
        offSet++;
        memcpy(profileBuff,pBuf+offSet,5);
        offSet += 5;
        //��ѹ����
        //if(pBuf[offSet++]!=0xDD)
        //  break;

        if(pBuf[offSet]!=0xDD)
        {
            memcpy(profileBuff+5,pBuf+offSet,17);
            offSet += 17;
        }
        //����
        if(pBuf[offSet++]!=0xDD)
            break;
        if(pBuf[offSet]!=0xDD)
        {
            memcpy(profileBuff+22,pBuf+offSet,24);
            offSet += 24;
        }
        //��������
        if(pBuf[offSet++]!=0xDD)
            break;
        if(pBuf[offSet]!=0xDD)
        {
            memcpy(profileBuff+46,pBuf+offSet,8);
            offSet += 8;
        }
        //�й�����
        if(pBuf[offSet++]!=0xDD)
            break;
        if(pBuf[offSet]!=0xDD)
        {
            memcpy(profileBuff+54,pBuf+offSet,16);
            //commfun_printstring(profileBuff+54,16);
            offSet += 16;
        }
        //�޹�����
        if(pBuf[offSet++]!=0xDD)
            break;
        if(pBuf[offSet]!=0xDD)
        {
            memcpy(profileBuff+70,pBuf+offSet,16);
            offSet += 16;
        }
        //��ǰ����
        if(pBuf[offSet++]!=0xDD)
            break;
        if(pBuf[offSet]!=0xDD)
        {
            memcpy(profileBuff+86,pBuf+offSet,6);
            offSet += 6;
        }
        if(pBuf[offSet++]!=0xDD)
            break;
        //У����ж�
        offSet++;
        //�������ж�
        if(pBuf[offSet++]!=0x18)
            break;
        DateTimeBcd_t storeTm;
        memcpy_r(&storeTm.year_l,profileBuff,5);
        ReduceGap(&storeTm.year_l, 5, STD2007_GAP_VALUE);
        storeTm.year_h = 0x20;
        storeTm.sec = 0x00;

        #if(1 == OOP_CRV_ADDREAD_485) //�������߳�����ʼʱ��
        UpdateCrvNewStartTime(oad.nObjID, &storeTm, pCbTask->TaskId, pCbTask->SchemeId, Pn, &pCbTask->AcqInfo);
        #endif

        uint8 i,subOadNum;
        subOadNum = pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum;
        if(subOadNum == 0)
            break;
        i=0;
        TASK_FMT_TRACE ( pipe,REC_LOG, "���ɼ�¼����subOadNum is %d\n",subOadNum);
        TASK_BUF_TRACE(pipe,REC_LOG,pBuf, rcvLen);
        while(i<subOadNum)
        {
            uint16 bias = CaclBiasByOad(oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i]);
            TASK_FMT_TRACE ( pipe,REC_LOG, "��06000001�е�bias is %d,oad.value is %x,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i].value is %x\n",bias,oad.value,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i].value);
            if(bias!=0xFFFF)
            {

                STD64507_AUTK_Recv(pipe,oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i],&profileBuff[bias],sizeof(profileBuff)-bias,mCbMtrPara,&storeTm,pCbTask,RunTaskInfo);
            }
            else
            {
                uint8 testEnergy[20];
                memset(testEnergy,0x33,20);
                STD64507_AUTK_Recv(pipe,oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i],testEnergy,20,mCbMtrPara,&storeTm,pCbTask,RunTaskInfo);
            }
            i++;
        }
    }
    while(0);
    return 0;
}

/****************************************************************************
*�������ƣ�STD64507_AUTK_Recv_Frz
*���ܣ�����  645_07�������صĶ�������
*���������
*���������
*���أ�
****************************************************************************/

uint16 STDProtector_AUTK_Recv_Frz(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{
    uint16 offSet=14;
    uint8 subOadNum;
    DateTimeBcd_t storeTm;
    time_t longTm;
    uint16 CsIndex=0;
    uint16 SubOadIndex=0;
    SubOadIndex=RunTaskInfo->SubOadIndex;
    CsIndex=RunTaskInfo->CsdIndex;
    subOadNum = pCbTask->CsdTask.RoadTask[CsIndex].SubOadNum;
    if(subOadNum == 0)
        return 0;
    //i=0;
    if(pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM || pCbTask->AcqInfo.Acqtype == ACQTYP_TI
            || pCbTask->StoreTimeType == STRTMTYP_FRZTIME)
    {
//      printf("STD64507_AUTK_Recv_Frz is BEGIN\n");
//      printf("RunTaskInfo->SubOadIndex is %d\n",RunTaskInfo->SubOadIndex);
//      printf("!!!!CsIndex is %d\n",CsIndex);
        //���ն���ʱ��ɼ�����Ҫ�ж������Ƿ���ʱ������Χ��
        if(RunTaskInfo->SubOadIndex==0)
        {
//          printf("STD64507_AUTK_Recv_Frz is BEGIN1111\n");
            if(pCbTask->CsdTask.RoadTask[CsIndex].OadListInfo[SubOadIndex].nObjID != 0x2021)//��һ�б����Ƕ���ʱ��
            {
//              printf("STD64507_AUTK_Recv_Frz is BEGIN2222\n");
                pComm_Info->cbLPPara.endNum = 0;
                return 0;
            }
            if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5005 || pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5006)
            {
                memcpy_r(&storeTm.year_l,&pBuf[offSet],4);
                ReduceGap(&storeTm.year_l, 4, STD2007_GAP_VALUE);
                storeTm.min = 0;
            }
            else
            {
                memcpy_r(&storeTm.year_l,&pBuf[offSet],5);
                ReduceGap(&storeTm.year_l, 5, STD2007_GAP_VALUE);
            }

            storeTm.year_h = 0x20;
            storeTm.sec= 0x00;
            //commfun_printstring((uint8*)&storeTm,7);//��ӡ�ɼ����Ĵ洢ʱ��
            if(DT_DateTimeBCD2Time(&storeTm,&longTm)==FALSE)//Ӧ���ʱ�䲻�Ϸ�
            {
                //���洢
//                printf("STD64507_AUTK_Recv_Frz is BEGIN4444\n");
                pComm_Info->cbLPPara.endNum = 0;
                return 0;
            }
            if(pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM || pCbTask->AcqInfo.Acqtype == ACQTYP_TI)
            {
                //ʹ��ǰ�պ�գ�������ʼ��������
                if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5003)
                {
                    if(longTm < pComm_Info->cbLPPara.startTime)
                    {
                        //����ʱ�����ʼʱ�仹С������Ҫ����������
                        pComm_Info->cbLPPara.endNum = 0;
                        return 0;
                    }
                }
                else
                {
//                  printf("STD64507_AUTK_Recv_Frz is BEGIN5555\n");
//                  printf("longTm is %ld\n",longTm);
//                  printf("pComm_Info->cbLPPara.startTime is %ld\n",pComm_Info->cbLPPara.startTime);
                    if(longTm <= pComm_Info->cbLPPara.startTime)
                    {
//                      printf("STD64507_AUTK_Recv_Frz is BEGIN6666\n");
                        //����ʱ�����ʼʱ�仹С������Ҫ����������
                        pComm_Info->cbLPPara.endNum = 0;
                        #if(1 == OOP_DAY_ADDREAD_485)
                        if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
                        {
//                          printf("STD64507_AUTK_Recv_Frz is BEGIN6666\n");
                            pComm_Info->cbLPPara.storeTm = storeTm;//����洢ʱ��
                        }
                        #else
                        if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
                        {
                        //    pCbTask->pCurMtr->needCBagain = TRUE;//��Ҫ����
//                          printf("STD64507_AUTK_Recv_Frz is BEGIN7777\n");
                        }
                        #endif
                        return 0;
                    }
                }
                if(longTm > pComm_Info->cbLPPara.endTime)
                {
                    //���ڲɼ���ʱ�����࣬���洢������������һ����¼
                    return 0;
                }
                if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
                {
                    #if(0 == OOP_DAY_ADDREAD_485)
               //     pCbTask->pCurMtr->needCBagain = FALSE;//���ɹ�����Ҫ����,��ע�͵�
                    pComm_Info->cbLPPara.endNum = 0;//��������
                    #endif
                }
            }
            else if(pCbTask->AcqInfo.Acqtype == ACQTYP_LASTN)//��N�εĲ��Ƚ�ʱ��
            {
//                printf("���ɼ���N�βɼ���������\r\n");
           //     pCbTask->pCurMtr->needCBagain = FALSE;//���ɹ�����Ҫ����,��ע�͵�
                pComm_Info->cbLPPara.endNum = 0;//��������
            }

            pComm_Info->cbLPPara.storeTm = storeTm;//����洢ʱ��
            if(pCbTask->StoreTimeType != STRTMTYP_FRZTIME)//������ǰ��ն���ʱ��洢����������
            {
                pComm_Info->cbLPPara.endNum = 0;
            }
        }
    }
    else
    {
        if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
        {
          //  pCbTask->pCurMtr->needCBagain = FALSE;//���ɹ�����Ҫ����
        }
    }
//  printf("pCbTask->StoreTimeType is %d\n",pCbTask->StoreTimeType);
    if(pCbTask->StoreTimeType == STRTMTYP_FRZTIME)
    {
        #if D_485_698_RTTOPOINT == 1
        //if(IsRealDataCopyToPointData())
        {
            if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004 && pComm_Info->cbLPPara.endNum == 0xff && pComm_Info->cbLPPara.storeTm.year_h == 0) //�ն���ʵʱת��,�Ѵ洢ʱ���޸�Ϊ��������ʱ��-��ʱʱ��
            {
                time_t StoreTm;
                OOP_TI_T delayTi;
                DateTimeBcd_t tmptime;
                memcpy_r((uint8*)&tmptime,RunTaskInfo->ThisTurnBeginTime,7);//�ɼ�����ʱ��
                DT_DateTimeBCD2Time(&tmptime,&StoreTm);
                delayTi.unit = pCbTask->DelayTimeUnit;
                delayTi.value = pCbTask->DelayTime;
                DT_TimeSubInv(&StoreTm, delayTi);
                pComm_Info->cbLPPara.storeTm = taskmng_acq_store_time_get(pCbTask->StoreTimeType,StoreTm);
             //   pCbTask->pCurMtr->needCBagain = FALSE;//���ɹ�����Ҫ����
            }
        }
        #endif
        storeTm = pComm_Info->cbLPPara.storeTm;
//      printf("1234STD64507_AUTK_Recv storeTm is %ld\n",storeTm);
    }
    else
    {
        uint8 tmpbuff[7];
        memcpy_r(tmpbuff,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);
        DT_DateTimeBCD2Time((DateTimeBcd_t*)tmpbuff,&longTm);
        storeTm = taskmng_acq_store_time_get(pCbTask->StoreTimeType,longTm);
//      printf("5678STD64507_AUTK_Recv storeTm is %ld\n",storeTm);
        #if(1 == OOP_DAY_ADDREAD_485)
        if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
        {
            DT_DateTimeBCD2Time(&pComm_Info->cbLPPara.storeTm,&longTm);
            storeTm = taskmng_acq_store_time_get(pCbTask->StoreTimeType,longTm);
        }
        #endif
        #if D_485_698_RTTOPOINT == 1
        //if(IsRealDataCopyToPointData())
        {
            pComm_Info->cbLPPara.storeTm = storeTm;
          //  pCbTask->pCurMtr->needCBagain = FALSE;//���ɹ�����Ҫ����
        }
        #endif
    }
    STD64507_AUTK_Recv(pipe,oad,pCbTask->CsdTask.RoadTask[CsIndex].OadListInfo[SubOadIndex],&pBuf[offSet],rcvLen-offSet,mCbMtrPara,&storeTm,pCbTask,RunTaskInfo);

    #if(1 == OOP_DAY_ADDREAD_485)
    if(pCbTask->CsdTask.RoadTask[CsIndex].MainOad.nObjID == 0x5004)
    {
        
       // IsNeedPatchDayMet07ByFrzTime(pComm_Info->cbLPPara.storeTm,storeTm,mCbMtrPara,pComm_Info,pCbTask);
    }
    #endif
    return 0;
}


/****************************************************************************
���ƣ� STD64507_AUTK_Recv_Event
���ܣ�
-           64507�¼������������ղɼ�������������
�������̣�
?���������
-           pipe                ͨ����
-           pBuf                ���յ��ĵ����뱨��
-           rcvLen              ���յĳ���
-           mCbMtrPara          ��ǰ����Ĳ��������Ϣ
-           pCbTask         ��ǰ������Ϣ
���������
-           ��
���أ�
-           0:���ݽ�������
****************************************************************************/
uint16 STDProtector_AUTK_Recv_Event(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,uint8 *DI07Index,uint8 DI07Num,CTS_RUN_TASK_T *RunTaskInfo)
{
    uint8 i =0,subOadNum;
    uint16 bias;
    DateTimeBcd_t storeTm;
    time_t longTm;


    uint16 CsdIndex;            
    uint16 SubOadIndex;
    uint16 MeterIndex,pn;  //��ǰ���������� 

    OOP_OAD_U   subOad;

    CsdIndex=RunTaskInfo->CsdIndex;             
    SubOadIndex=RunTaskInfo->SubOadIndex;

    subOadNum = pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum;
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    if(subOadNum == 0)
        return 0;
    i=0;

    TASK_FMT_TRACE(pipe, COMM_LOG, "pCbTask->AcqInfo.Acqtype is %d,pCbTask->StoreTimeType is %d\n",pCbTask->AcqInfo.Acqtype,pCbTask->StoreTimeType);
    if(pCbTask->AcqInfo.Acqtype == ACQTYP_FRZTM || pCbTask->AcqInfo.Acqtype == ACQTYP_TI
            || pCbTask->StoreTimeType == STRTMTYP_FRZTIME)
    {
        //���ն���ʱ��ɼ�����Ҫ�ж������Ƿ���ʱ������Χ��
        if(pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[0].nObjID != 0x201E)//��һ�б������¼�����ʱ��
        {
            pComm_Info->cbLPPara.endNum = 0;
            return 0;
        }
        if(*DI07Index == 0)
        {
            bias = CaclBiasByOad(oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[0]);//��ȡ�¼�����ʱ���ƫ��λ��
            if(bias!=0xFFFF)
            {
                bias += 14;
                memcpy_r(&storeTm.year_l,&pBuf[bias],6);
                ReduceGap(&storeTm.year_l, 6, STD2007_GAP_VALUE);
                storeTm.year_h = 0x20;
            }
            else
            {
                memset(&storeTm,0x00,7);
            }
            if(DT_DateTimeBCD2Time(&storeTm,&longTm)==FALSE)//Ӧ���ʱ�䲻�Ϸ�
            {
                //���洢
                pComm_Info->cbLPPara.endNum = 0;
                return 0;
            }
            pComm_Info->cbLPPara.storeTm = storeTm;//����洢ʱ��
            //ǰ�պ󿪣�������ʼ�������ô���
            if(longTm < pComm_Info->cbLPPara.startTime)
            {
                //����ʱ�����ʼʱ�仹С������Ҫ����������
                pComm_Info->cbLPPara.endNum = 0;
                return 0;
            }
            if(longTm >= pComm_Info->cbLPPara.endTime)
            {
                //���ڲɼ���ʱ�����࣬���洢������������һ����¼
                return 0;
            }
        }

        if(*DI07Index == 0)
        {
            while(i<subOadNum)
            {
                bias = CaclBiasByOad(oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex]);
                if(bias!=0xFFFF)
                {
                    bias += 14;
                    STD64507_AUTK_Recv(pipe,oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],&pBuf[bias],rcvLen-bias,mCbMtrPara,NULL,pCbTask,RunTaskInfo);
                }
                i++;
            }
        }
        else if(*DI07Index == 1)//�¼��������
        {
            bias = 14;
            subOad.nObjID = 0x2022;
            subOad.attID = 0x02;
            subOad.nIndex = 0;
           
            if(DT_DateTimeBCD2Time(&pComm_Info->cbLPPara.storeTm,&longTm)==FALSE)//Ӧ���ʱ�䲻�Ϸ�
            {
                //���洢
                pComm_Info->cbLPPara.endNum = 0;
                return 0;
            }
            //ǰ�պ󿪣�������ʼ�������ô���
            if(longTm < pComm_Info->cbLPPara.startTime)
            {
                //����ʱ�����ʼʱ�仹С������Ҫ����������
                pComm_Info->cbLPPara.endNum = 0;
                return 0;
            }
            if(longTm >= pComm_Info->cbLPPara.endTime)
            {
                //���ڲɼ���ʱ�����࣬���洢������������һ����¼
                return 0;
            }
            STD64507_AUTK_Recv(pipe,oad,subOad,&pBuf[bias],rcvLen-bias,mCbMtrPara,NULL,pCbTask,RunTaskInfo);
        }
    }

    if(pCbTask->AcqInfo.Acqtype == ACQTYP_LASTN)
    {

        

        if(pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[0].nObjID != 0x2022)//��һ�б������¼���¼���
        {
            pComm_Info->cbLPPara.endNum = 0;
            return 0;
        }

        if(*DI07Index == 0)                  //��һ֡�����¼���¼���
        {
            uint32      LastRecNum = 0;                 //��һ�γ����¼���¼���
            uint32      RecvRecNum = 0;                 //���λظ��¼���¼���
            uint8       EvtCnt[3] = {0};

                        
            if(FALSE == FindEvtRecNoIDByOad(oad))
            {

                TASK_BUF_TRACE( pipe, COMM_LOG, "δ�ҵ���Ӧoad is %x\n",oad.value);

                *DI07Index = DI07Num;
                return 0;
            }

            bias = 14;
            #ifdef AREA_ZHEJIANG
            memcpy(EvtCnt,&pBuf[bias],2);
            ReduceGap(EvtCnt,2,STD2007_GAP_VALUE);
            memcpy(&RecvRecNum,EvtCnt,2);
            #else
            memcpy(EvtCnt,&pBuf[bias],3);
            ReduceGap(EvtCnt,3,STD2007_GAP_VALUE);
            memcpy(&RecvRecNum,EvtCnt,3);
            #endif
              
            LastRecNum=GetTaskReportSeq( pipe, RunTaskInfo->TaskId,RunTaskInfo->logicid, oad, pn);

            TASK_FMT_TRACE( pipe, COMM_LOG, "�ϴγ������¼����LastRecNum is %d,RecvRecNum is %d\n",LastRecNum,RecvRecNum);

            if(RecvRecNum == 0 ||RecvRecNum==LastRecNum)
            {
               *DI07Index = DI07Num;
                return 0;
            }
            else
            {
                SetTaskReportSeq( pipe, RunTaskInfo->TaskId, RunTaskInfo->logicid, oad, pn,RecvRecNum);
                
            }
            
            if(LastRecNum != RecvRecNum)
            {
                subOad.nObjID = 0x2022;
                subOad.attID = 0x02;
                subOad.nIndex = 0;
                Protector_AUTK_Recv(pipe,oad,subOad,&pBuf[bias],rcvLen-bias,mCbMtrPara,NULL,pCbTask,RunTaskInfo);
            }
            else
            {
                *DI07Index = DI07Num;
                return 0;
            }
        }
        else if(*DI07Index == 1)             //�¼���¼����
        {

            i = 1;
            while(i<subOadNum)
            { 

                bias = CaclBiasByOad(oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i]);

                if(bias!=0xFFFF)
                {
                    bias += 14;

                    Protector_AUTK_Recv(pipe,oad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i],&pBuf[bias],rcvLen-bias,mCbMtrPara,NULL,pCbTask,RunTaskInfo);
                }
                i++;
            }
            if(pCbTask->rptFlag == TRUE)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, " һ���¼����� �÷�����Ҫ�ϱ�  \n" );
                time_t StartTime;
                DateTimeBcd_t  tmpTime;
                memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);
                DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StartTime);  
                taskmng_645_event_data_save(pipe, pn,pCbTask->logicid,pCbTask->TaskId,&pCbTask->CsdTask.RoadTask[CsdIndex] , StartTime);
            }
        }

    }
    return 0;
}



