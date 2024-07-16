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

#define FIRST_DEFINE_METPARAM
 #include "../../include/CtsAll.h"

#if OS_METDLT645_EN == 1

#define DEBUG_STD //�汾�ȶ���ȥ������

#ifdef DEBUG_STD
  #define STD_ENV  "metstd"
#endif

/*
*****************************************************************************************
*- ���������ID�궨������ -
*****************************************************************************************
*/
#define STD_CMDID_READ          0x01    /*-������-*/
#define STD_CMDID_READNEXT      0x02    /*-����������-*/
#define STD_CMDID_REREAD        0x03    /*-�ض�����-*/
#define STD_CMDID_WRITE         0x04    /*-д����-*/
#define STD_CMDID_SETTIME       0x08    /*-�㲥Уʱ(YYMMDDhhmmss)-*/
#define STD_CMDID_SETADDR       0x0A    /*-д�豸��ַ(�ù㲥��ַ�����µ�ַ)-*/
#define STD_CMDID_SETRATE       0x0C    /*-����ͨ�Ų�����-*/
#define STD_CMDID_MDFPSW        0x0F    /*-�޸�����-*/
#define STD_CMDID_CLRMAXAD      0x10    /*-�����������-*/

/*
*****************************************************************************************
*-��վ����֡���-
*****************************************************************************************
*/
#define STD_CMDID_ONE           0x80    //-����Ӧ��֡�޺���֡-
#define STD_CMDID_NEXT          0xA0    //-����Ӧ��֡�к���֡-
#define STD_CMDID_ERROR         0xC0    //-�쳣Ӧ��֡-

/*
*****************************************************************************************
*-����-
*****************************************************************************************
*/
#define STD_FILL_CODE_DD        0xDD    /*-0xAA+0x33�����ݻ�������������ַ�(����û��)-*/
#define STD_FILL_CODE_EE        0xEE    /*-0xBB+0x33�����ݻ���ֽ�����ַ�(С��û��)-*/
//static uint8 STD_BROADCAST_ADDRESS[6] = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99}; /*-�㲥��ַ-*/
#define STD_GAP_VALUE           0x33    /*-������DATA����ʱ+33H����ʱ-33H-*/
#define STD_AWAKE_CODE          0xFE    /*-������Ϣǰ�ȷ���1~4���ֽ�FEH���ڻ��ѽ��շ�-*/
#define STD_GAP_VALUE           0x33    /*-������DATA����ʱ+33H����ʱ-33H-*/
#define STD_ADDR_SITE           12

#define STD_SYNC                            0x68

void STD_MakeFrame(uint8 * pMetAddr, uint16 fraDI, uint8 cmdDI, uint8 * pBuf, uint8 * pSndLen, uint8 bStdType);
uint8 STD_CheckRspFrame(uint8 pipe, uint8 * pBuf, uint16 rcvLen, uint8 bStdType,S_COMM_INFO_MET* pComm_Info_97);
extern cbMetPara STD2007Para;
extern uint8 ComNo[10];


#endif
/****************************************************************************
*ģ���ţ�
*���ƣ�JudgeIfSpecialCombin
*���ܣ��ж��Ƿ�Ϊ��Ҫ���⴦���OI
*���������
*���������
*����
*���أ���
****************************************************************************/
BOOL JudgeIfSpecialCombin(OOP_OAD_U pOadInfo)
{
    switch(pOadInfo.nObjID){
        case 0x4000:
            return TRUE;
        default:
            return FALSE;
    }
}
/****************************************************************************
*ģ���ţ�
*���ƣ�GetDI97
*���ܣ���ȡҪ����97��������
*���������
*���������
*����
*���أ���
****************************************************************************/

uint8 GetDI97(AcqInfo_t AcqInfo,CSD_TASK_INFO   *csd, uint8 subOadIndex,uint16 *DI97Buff)
{
    const MapOopTo645_NEW *pMapOopTo645;
    OadInfoMap pOadInfo;
 
    OOP_OAD_U mainOAD,subOAD;
    uint8 di97Num = 0;
    if(csd->MainOad.nObjID==0x0000)//OAD
    {
        mainOAD.nObjID = 0x0000;
        mainOAD.attID = 0x00;
        mainOAD.nIndex = 0x00;
        subOAD = csd->OadListInfo[subOadIndex];
    }
    else//ROAD
    {
        if(FALSE==taskmng_oad_info_get(csd->MainOad, &pOadInfo))
        {
            
            return 0;
        }
        if(AcqInfo.Acqtype == ACQTYP_NULL)
        {
            if((csd->MainOad.nObjID & 0xF000) != 0x5000)
            {
                //ֻ�ж���֧��ͨ��ʵʱ����ת��
                subOadIndex = csd->SubOadNum;//ȫ��������
            }
            mainOAD.nObjID = 0x0000;
            mainOAD.attID = 0x00;
            mainOAD.nIndex = 0x00;
        }
        else
        {
            mainOAD = csd->MainOad;
        }
        switch(pOadInfo.class)
        {
        case 7:
        case 9:
        case 24:
            if(subOadIndex>=csd->SubOadNum)
                return 0;
            subOAD = csd->OadListInfo[subOadIndex];
            break;
        default:
            return 0;
        }
    }


    if(FALSE==taskmng_oad_info_get(subOAD, &pOadInfo))
    {
            return 0;
    }
    pMapOopTo645= Find645InfoPtrByOad(mainOAD,subOAD);
     if(pMapOopTo645==NULL)
        return 0;
    if(pMapOopTo645->Oad.value == 0x20140200)
    {
        pMapOopTo645++;
  
    }
    while(pMapOopTo645->Oad.value==subOAD.value)
    {
        if((DI97Buff!=NULL)&&(pMapOopTo645->DI97 !=0xFFFF))
        {
            DI97Buff[di97Num]= pMapOopTo645->DI97;
            di97Num++;
        }
        pMapOopTo645++;

    }
    return di97Num;
}



/*-
***************************************************************************
*ģ���ţ�7-9-1-1
*���ƣ���֯����������֡
*���ܣ������Լ�������֡
*�������������ַpMetAddr�����ݱ�ʶfraDI�������ʶcmdDI��֡��Ż�������ַpBuf��֡����ŵ�ַpSndLen
*�����������֡��Ļ�������֡��
           ������� bStdType   0-��ͨ���� 1-������ױ�

*�������ݵ���Լ��֡
*���أ���
***************************************************************************
-*/
void STD_MakeFrame(uint8 *pMetAddr, uint16 fraDI, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, uint8 bStdType)
{
    uint8 tmpSnd[20];
    uint8 i;

    memset(tmpSnd, 0x00, sizeof(tmpSnd));
    for(i = 0; i < 4; i++)
    {
        tmpSnd[i] = 0xFE;
    }
    tmpSnd[4] = 0x68;
    if (bStdType == 4)
    {
        memcpy(&tmpSnd[5], pMetAddr, 6); /*- ����ַ -*/
        for (i=0;i<6;i++)
        {
            if (tmpSnd[10-i] == 0x00)
            {
                tmpSnd[10-i] = 0xAA;
            }
        }
    }
    else
    {
        memcpy(&tmpSnd[5], pMetAddr, 6); /*- ����ַ -*/
    }
    tmpSnd[11] = 0x68;

    if (fraDI == 0x0000)  //���Ե��
    {
        tmpSnd[12] = 0x05;
        tmpSnd[13] = 0x00;
        tmpSnd[14] = CalCs(&tmpSnd[4], 10);     /*- У��� -*/
        tmpSnd[15] = 0x16;
        memcpy(pBuf, tmpSnd, 16);               /*- ����ͻ�����pBuf-*/
        *pSndLen = 16;
        return;
    }
    /*-����������֡-*/
    tmpSnd[12] = cmdDI;

    switch(cmdDI)
    {
        case STD_CMDID_READ:
        case STD_CMDID_READNEXT:
            tmpSnd[13] = 0x02;
            break;
        case STD_CMDID_REREAD:
            tmpSnd[13] = 0x00;
            break;
        default:
            tmpSnd[13] = 0x02;
    }
    memcpy(&tmpSnd[14], (uint8 *)&fraDI, sizeof(uint16));   /*- ��Ŀ��ʶ���� -*/

    AddGap(&tmpSnd[14], 2, STD_GAP_VALUE);
    tmpSnd[16] = CalCs(&tmpSnd[4], 12);         /*- У��� -*/
    tmpSnd[17] = 0x16;
    tmpSnd[18] = 0x00;
    tmpSnd[19] = 0x00;

    if (bStdType == 1)                          //������ױ�
    {
        memcpy(pBuf, &tmpSnd[4], 16);
        *pSndLen = 14;
    }
    else
    {
        memcpy(pBuf, tmpSnd, 20);               /*- ����ͻ�����pBuf*/
        *pSndLen = 18;                          /*-�еı�֧�ֺ��������0   2006-7-21 10:03-*/
    }
    return;
}

/*-
***************************************************************************
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
***************************************************************************
-*/

uint8 STD_CheckRspFrame(uint8 pipe, uint8 *pBuf, uint16 rcvLen, uint8 bStdType,S_COMM_INFO_MET* pComm_Info_97)
{
    uint8   frameCbAddr[6]={0};
    uint8   frameCs;
    uint8   frameLen;

    frameLen = 0x0c + pBuf[9];
    if(frameLen > rcvLen)       //���ȹ���
        return 0;

    memcpy(frameCbAddr, pComm_Info_97->pMtrPara->MetAddr, 6);           /*- ����ַ -*/

    uint8   LocalCnt = 0;                // �뱾���ַ��ͬ����
    int     i = 0;

    //�ȽϷ��ص�ַ
    for (i = 0; i < 6; i++)
    {
        if (((frameCbAddr[i] & 0x0F) == (pBuf[1+i] & 0x0F)) || ((pBuf[1+i] & 0x0F) == 0x0A))
        {
            LocalCnt++;
        }

        if (((frameCbAddr[i] & 0xF0) == (pBuf[1+i] & 0xF0)) || ((pBuf[1+i] & 0xF0) == 0xA0))
        {
            LocalCnt++;
        }
    }
    if (LocalCnt == 12) //(memcmp(frameCbAddr, &pBuf[1], 6) == 0)
    {
        //return 1;
    }
    else
    {
        return 0;
    }

    frameLen = 0x0c + pBuf[9];
    frameCs = CalCs(pBuf, frameLen - 2);

    if((pBuf[0] == STD_SYNC)
        && (pBuf[7] == pBuf[0])                         /*-����ϵͳ����ַ��0��AA�ɱ�-*/
        && (pBuf[frameLen - 2] == frameCs)
        && (pBuf[frameLen - 1] == 0x16))
    {
        switch(frameLen)
        {
        case 0x0C:
            if((pBuf[8] == 0x8A) && (pBuf[9] == 0x00)) /*-д�豸��ַ����Ӧ��֡-*/
            {
                return 3;
            }
            else
            {
                return 0;
            }
        case 0x0D:
            if((pBuf[8] & 0xF0) == 0xC0)                /*-�쳣Ӧ��֡-*/
            {
                return 4;
            }
            else
            {
                if((pBuf[8] == 0x8C) && (pBuf[10] != 0xFF)) /*-���Ĳ�����ȷ��Ӧ��֡-*/
                {
                    return 3;
                }
                else
                {
                    if((pBuf[8] == 0x8C) && (pBuf[10] == 0xFF)) /*-���Ĳ����ʴ���Ӧ��֡-*/
                    {
                        return 5;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }
        default:
            if(frameLen >= 0x0C)
            {
                if(0x80 == (pBuf[8] & 0xF0))        /*-�޺�������֡-*/
                {
                    return 1;
                }
                else
                {
                    if(0xA0 == (pBuf[8] & 0xF0))    /*-�к�������֡-*/
                    {
                        return 2;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }
            else
            {
                return 0;
            }
        }
    }
    else
    {
        return 0;
    }
}

/****************************************************************************
���ƣ� Prepare_645_97_meter_read
���ܣ�
-           Ϊ645_97��Լ�ĵ��ĳ�����׼�������ִ������Ĳ�������Ϣ
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


void Prepare_645_97_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe,CTS_RUN_TASK_T   *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
{
    uint16 sn;
    uint16 MeterIndex;
    OOP_METER_BASIC_T MeterBasicInfo;
    
    MeterIndex=RunTaskInfo->MeterIndex;
    sn=pCbTask->MsInfo[MeterIndex].Sn;
    //��ȡ��������Ϣ
    memset((void *)MtrPara, 0x0, sizeof(cbMtrPara));
    taskmng_mtr_basic_info_get(sn,RunTaskInfo->logicid,&MeterBasicInfo);
    MtrPara->MtrNum=sn;
   // MtrPara->Speed=get_meter_read_speed(MeterBasicInfo.baud);
    MtrPara->Speed=MeterBasicInfo.baud;
    memcpy_r(MtrPara->MetAddr,MeterBasicInfo.tsa.add,6);
    memcpy((uint8*)&MtrPara->MtrCommAddr,(uint8*)&MeterBasicInfo.tsa,sizeof(OOP_TSA_T));
    memset(Comm_Info_Oop,0,sizeof(S_COMM_INFO_MET));

    Comm_Info_Oop->pMtrPara = MtrPara;
    Comm_Info_Oop->MetStat = CB_STATE_INIT;

}

/****************************************************************************
���ƣ� Meter_645_97_Read_Item
���ܣ� ʵ��645����97��Լ���ĳ����ĵķ���

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

void Meter_645_97_Read_Item(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI97Index,uint8 *DI97Num,uint16 *DI97Buff)
{

    uint16              DI97=0xFFFF;    //���ݱ�ʶ

 
    uint16 CsdIndex,SubOadIndex,MeterIndex,pn,tmpMsIndex;

    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);

    SubOadIndex=RunTaskInfo->SubOadIndex;
    CsdIndex=RunTaskInfo->CsdIndex;
    
    //Cheeck_Cb_Pipe(pCbTask,Comm_Info_Oop,MtrPara,pipe,lstTime);
    ExecMeter485Relay(pipe,&Comm_Info_Oop->MetStat,pCbTask->Priority);
    if(Comm_Info_Oop->MetStat ==CB_STATE_INIT)
    {
        return;
    }
    if((pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5002)||(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5003)
        ||(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5004)||(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5005))
    {
        pCbTask->AcqInfo.Acqtype = ACQTYP_NULL;
    }
    if(CsdIndex<pCbTask->CsdTask.RoadNum)
    {//
        if(*DI97Index == 0 )
        {
            *DI97Num = GetDI97(pCbTask->AcqInfo,&pCbTask->CsdTask.RoadTask[CsdIndex],SubOadIndex,DI97Buff);//��ȡ��������ݱ�ʶ

        }
    
        if(*DI97Num>0 && DI97Buff[*DI97Index]!=0xFFFF)
        {
            DI97 = DI97Buff[*DI97Index];
            
            STD_MakeFrame(MtrPara->MetAddr, DI97, STD_CMDID_READ, Comm_Info_Oop->pSndBuf, &Comm_Info_Oop->SndLen, 0);
        }
        else
        {
            STD97_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo,DI97Index);
            return;
        }

        rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,MtrPara->Speed,PARITY_EVEN,Comm_Info_Oop->pSndBuf, (int)Comm_Info_Oop->SndLen);
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_SENDFRMNUM,NULL);
        
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastSendTime=time(NULL);
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
���ƣ� Deal_Receive_Result_645_97
���ܣ� ����645����97��Լ���ĳ����ĵĽ��ս��

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

CB_STATE Deal_Receive_Result_645_97(uint8 pipe,S_COMM_INFO_MET *Comm_Info_Oop,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,BOOL actResult,uint8   *DI97Index)
{
    uint16 pn,tmpMsIndex,MeterIndex;
    
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    eOopCommRslt_t ret = 0;
    if(actResult)                          //��֡������ȷ
    {
        STD97_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo,DI97Index);
        Comm_Info_Oop->ComFrame++;          //��֡������
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_RECVFRMNUM,NULL);//pCbTask->TaskMonitor.recvFrameNum ++;
        Comm_Info_Oop->ErrFrame = 0;        //ʧ��֡���ʹ���֡�����㣬׼�����¼���
        Comm_Info_Oop->ErrTimes = 0;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = TRUE;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt = 0;
    }
    else                                            //��֡����ʧ��
    {

        if(Comm_Info_Oop->ErrTimes >= ERR_TIMES_MAX)        //ͬһ֡��ʧ�ܴ���
        {
            if(Comm_Info_Oop->ErrFrame >= ERR_FRAME_MAX)      //ʧ�ܴ������࣬���ټ�����׼���˳�
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
                STD97_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo,DI97Index);
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
���ƣ� Meter_645_97_Recvive_Data
���ܣ� ʵ��645����97��Լ���ĳ����ĵĽ���

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

void Meter_645_97_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI97Index,uint8 DI97Num,uint8 *CombinBuff,uint16 *CombinLen,uint16 *DI97Buff)
{
    BOOL        actResult = TRUE;       //�Ի��봦��Ľ��
    eOopCommRslt_t ret = 0;
    uint16              DI97=0xFFFF;                                    //���ݱ�ʶ
    uint8               *ptr=NULL;                                  //Ӧ������֡��ʼָ��

    uint16 CsdIndex,SubOadIndex;
    uint16 pn,tmpMsIndex,MeterIndex;
    uint16 reallen=0;
    
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    SubOadIndex=RunTaskInfo->SubOadIndex;
    CsdIndex=RunTaskInfo->CsdIndex;
    memset(Comm_Info_Oop->pRcvBuf, 0x0, sizeof(Comm_Info_Oop->pRcvBuf));

    Comm_Info_Oop->RcvLen =rs485_RS232_recv(pipe,Comm_Info_Oop->pRcvBuf, sizeof(Comm_Info_Oop->pRcvBuf));
    gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastRecvTime=time(NULL);
    if(Comm_Info_Oop->RcvLen > 9)   //��Ч���ĵ���С����Ϊ9���ֽ�
    {
        ptr = (uint8*)commfun_645_frame_get((char*)Comm_Info_Oop->pRcvBuf, STD2007Para.Sync, Comm_Info_Oop->RcvLen,&reallen);
    }
    if(Comm_Info_Oop->RcvLen <= 0 || ptr == NULL)
    {
        actResult = FALSE;              //���û�л��� ���� �����ʽ�Ƿ�
    }
    else
    {
        Comm_Info_Oop->RcvLen = reallen;
        memmove(Comm_Info_Oop->pRcvBuf, ptr, Comm_Info_Oop->RcvLen);
        ret = STD_CheckRspFrame(pipe, Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, 0,Comm_Info_Oop);
        if((ret == 1)||(ret == 2))    //��Ч,�������ݷ���
        {
            OOP_OAD_U mainOad;
            Comm_Info_Oop->ErrFrame=0;
            DI97 = DI97Buff[*DI97Index];

            if(*DI97Index<DI97Num)//OAD ��Ӧ��97���ݱ�ʶ��û��ͨѶ��
            {

                if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0)
                {
                    memset(&mainOad,0x00,sizeof(OOP_OAD_U));
                    STD1997_FrameCombin(mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],DI97,*DI97Index,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, CombinBuff, CombinLen);
                }
                else
                {
                    if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
                    memset(&mainOad,0x00,sizeof(OOP_OAD_U));
                    else
                    mainOad = pCbTask->CsdTask.RoadTask[CsdIndex].MainOad;
                    STD1997_FrameCombin(mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],DI97,*DI97Index,Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, CombinBuff, CombinLen);
                }
                (*DI97Index)++;

                if(*DI97Index>=DI97Num)
                {
                    *DI97Index = 0;
                    if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0)
                    {
                    memset(&mainOad,0x00,sizeof(OOP_OAD_U));

                    STD64597_AUTK_Recv(pipe,mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],CombinBuff, *CombinLen, *MtrPara, pCbTask,RunTaskInfo);
                    }
                    else
                    {
                    mainOad = pCbTask->CsdTask.RoadTask[CsdIndex].MainOad;
                    STD64597_AUTK_Recv(pipe,mainOad,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex],CombinBuff, *CombinLen, *MtrPara, pCbTask,RunTaskInfo);
                    }
                    actResult = TRUE;
                }
                else
                {
                Comm_Info_Oop->MetStat = CB_STATE_SEND;

                return;
                }
            }
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

    Deal_Receive_Result_645_97(pipe,Comm_Info_Oop,pCbTask,RunTaskInfo,actResult,DI97Index);
}
/****************************************************************************
*ģ���ţ�
*���ƣ�IsGroupByDI97
*���ܣ��ж�97�ĵ����ݱ�ʶ�Ƿ���ͬһ����
*���������DI
*���������
*����
*���أ�
    FALSE:����ͬһ��
    TRUE:
****************************************************************************/

BOOL IsGroupByDI97(uint16 sDI97,uint16 dDI97)
{
    if((sDI97&0xFFF0) != (dDI97&0xFFF0))
        return FALSE;
    if((sDI97&0x000F) == 0x000F  || (dDI97&0x000F) == 0x000F)
        return TRUE;
    else if((sDI97&0x000F) == (dDI97&0x000F))
        return TRUE;
    else
        return FALSE;

}

/****************************************************************************
*ģ���ţ�
*���ƣ�STD1997_FrameCombin
*���ܣ�1��OAD��Ӧ������ͬ��97�������,��Ҫ����֡����ƴ�յ�һ��
*���������DI
*���������
*����
*���أ�
    FALSE:����ͬһ��
    TRUE:
****************************************************************************/

BOOL STD1997_FrameCombin(OOP_OAD_U mainOad,OOP_OAD_U suboad,uint16 DI97,uint8 DI97Index,uint8 *inData,uint16 inLen,uint8 *outData,uint16 *outLen)
{
    uint16               offSet=12;
    const MapOopTo645_NEW *pMapOopTo645;

    uint16               i;
    uint16               desOffset=0;
    uint8                ItemLen=0;

    inLen = inData[9]+0x0A;
    //���ݴ�OAD ����645��Ӧ��

    pMapOopTo645 = Find645InfoPtrByOad(mainOad,suboad);
    if(pMapOopTo645==NULL)
    {
    return FALSE;
    }

    if(DI97Index==0)
    {
        ItemLen=pMapOopTo645->TotalDataLen;
        if((DI97&0xFF00)==0xA000 || (DI97&0xFF00)==0xA100 ||(DI97&0xFF00)==0xA400
        || (DI97&0xFF00)==0xA500 ||(DI97&0xFF00)==0xA800 ||(DI97&0xFF00)==0xA900)
        {
            if((DI97&0x000F)==0x000F)
            {
                ItemLen = 15;
            }
            else
            {
                ItemLen = 3;
            }
        }

        if(outData!=NULL)
        {
            memcpy(outData+desOffset,inData+offSet,ItemLen);
            desOffset+=ItemLen;
        }
        if(outLen!=NULL)
        {
            *outLen = desOffset;
        }
        return TRUE;
    }

    for(i =0; i < DI97Index; i++)       
    {

        ItemLen=pMapOopTo645->TotalDataLen;
        //97�������������������A01F��B01F,��Ҫ�������������ݵĺϲ����ڳ�����BO1F��,��Ҫ����A01F��ռ���ֽ���
        if((DI97&0xFF00)==0xB000 || (DI97&0xFF00)==0xB100 ||(DI97&0xFF00)==0xB400
        || (DI97&0xFF00)==0xB500 ||(DI97&0xFF00)==0xB800 ||(DI97&0xFF00)==0xB900)
        {
            if((DI97&0x000F)==0x000F)
            {
                ItemLen = 15;
            }
            else
            {
                ItemLen = 3;
            }
        }
        desOffset += ItemLen;
        pMapOopTo645++;
    }
    if(outData!=NULL)
    {
        ItemLen = pMapOopTo645->DataLen;
        //97�������ߵ���˵�����ڳ���B01F,B01F�ĳ��ȿ���20������4��
        if((DI97&0xFF00)==0xB000 || (DI97&0xFF00)==0xB100 ||(DI97&0xFF00)==0xB400
        || (DI97&0xFF00)==0xB500 ||(DI97&0xFF00)==0xB800 ||(DI97&0xFF00)==0xB900)
        {
            if((DI97&0x000F)==0x000F)
            {
                ItemLen = 20;
            }
            else
            {
                ItemLen = 4;
            }
        }
        memcpy(outData+desOffset,inData+offSet,ItemLen);
        desOffset+=ItemLen;
    }
    if(outLen!=NULL)
    {
        *outLen = desOffset;
    }

    return TRUE;
}


/****************************************************************************
���ƣ� STD64597_AUTK_Recv
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
uint16 STD64597_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{
    uint16               offSet=0;
    OadInfoMap       pOadInfo;
    const MapOopTo645_NEW  *pMapOopTo645;
    DateTimeBcd_t taskStoreTime;  // �ɼ�����ʱ��
    
    DateTimeBcd_t taskStartTime;   // �ɼ��洢ʱ��
    OadStoreInfo StoreInfo;
    time_t storeTm;
    time_t colStartTm;
    uint8                tmpBufer[255];           //Э��ת������ʱ��255���ֽ�
    DataSource sourcedata;
    uint8 desbuffer[2000];
    uint16 desLen,MsIndex;
    uint16 MeterIndex,pn;


    memset((uint8*)&StoreInfo,0x00,sizeof(OadStoreInfo));
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    if(pCbTask->TaskId>255)
    {//�ڲ�������
        //������

    }

    memset(tmpBufer,0x00,255);
    memcpy(tmpBufer,pBuf,rcvLen);

    if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
    {
        OOP_OAD_U tmpOad;
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
    if(FALSE==taskmng_oad_info_get(subOAD, &pOadInfo))
    {

        return 0;
    }
    //97���״̬��
    if(pMapOopTo645->Oad.value== 0x20140200)
    {
        rcvLen =14;
        pBuf[0] = ((pBuf[0]-STD_GAP_VALUE)&0x36)+STD_GAP_VALUE;
        memset(&pBuf[1],STD_GAP_VALUE,rcvLen-1);
    }

        ReduceGap(tmpBufer, rcvLen, STD_GAP_VALUE);
        memcpy(sourcedata.sourcebuffer,tmpBufer,rcvLen);
        sourcedata.reclen=rcvLen;

        Convert_6457_97_To_Struct(sourcedata ,desbuffer,&desLen,pMapOopTo645);
        memcpy_r((uint8*)&taskStartTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);//�ɼ�����ʱ��
        DT_DateTimeBCD2Time(&taskStartTime,&colStartTm);
    DT_DateTimeBCD2Time(&taskStartTime,&storeTm);
    if(pCbTask->StoreTimeType == STRTMTYP_NULL || pCbTask->StoreTimeType == STRTMTYP_STARTTM || pCbTask->StoreTimeType == STRTMTYP_FRZTIME)//97��û�ж���ʱ�꣬���Դ���������ʱ��
    {
        OOP_TI_T delayTi;
        DT_DateTimeBCD2Time(&taskStartTime,&storeTm);
        delayTi.unit = pCbTask->DelayTimeUnit;
        delayTi.value = pCbTask->DelayTime;
        DT_TimeSubInv(&storeTm, delayTi);
    }
    else
    {
        taskStoreTime = taskmng_acq_store_time_get(pCbTask->StoreTimeType,storeTm);
        DT_DateTimeBCD2Time(&taskStoreTime,&storeTm);
    }
    //set_bit_value(RunTaskInfo->MtrResult,128,MsIndex);
    taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
    taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);

    memcpy(StoreInfo.MAC.nValue,mCbMtrPara.MetAddr,6);
    StoreInfo.recordOAD.infoNum=pn;
    StoreInfo.recordOAD.logicId=RunTaskInfo->logicid;
//  {
        StoreInfo.recordOAD.road.oadMain=mainOAD;
        StoreInfo.recordOAD.road.oadCols.nNum=1;
        StoreInfo.recordOAD.road.oadCols.oad[0]=subOAD;
        StoreInfo.recordOAD.road.oadCols.rsv=0;
    //}
    StoreInfo.recordOAD.classtag = 5;
    StoreInfo.colStartTm=colStartTm;
    StoreInfo.colStoreTm=storeTm;
    StoreInfo.colEndTm=time(NULL);
    TASK_FMT_TRACE ( pipe,COMM_LOG, "�����:pCbTask->TaskId is %d\n",pCbTask->TaskId);
    taskmng_meter_data_save(desbuffer,desLen,StoreInfo,pipe);

    return offSet;


}
/****************************************************************************
*ģ���ţ�
*���ƣ�STD97_ToNextReadItem
*���ܣ��л�645����07��Լ���ĳ���������
*���������
*���������
*����
*���أ���
****************************************************************************/
void  STD97_ToNextReadItem(BOOL NextCSDFlg,S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 *DI97Index)
{

    uint16 *CsdIndex;
    CsdIndex=&RunTaskInfo->CsdIndex;
    
    if(pCbTask==NULL ||(*CsdIndex>pCbTask->CsdTask.RoadNum))
        return;
    if(NextCSDFlg==FALSE)
    {
        if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID !=0x0000)
        {
            if((pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID & 0xF000) != 0x3000)
            {
                    if((RunTaskInfo->SubOadIndex)<pCbTask->CsdTask.RoadTask[*CsdIndex].SubOadNum)
                    {
                        RunTaskInfo->SubOadIndex++;//ת����һ����OAD
                        pComm_Info->MetStat = CB_STATE_SEND;
                        return;
                    }

            }
        }
    }

    if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID==0)
    {
        RunTaskInfo->SubOadIndex++;
        if(RunTaskInfo->SubOadIndex>=pCbTask->CsdTask.RoadTask[*CsdIndex].SubOadNum)
        {
            RunTaskInfo->CsdIndex++;
            RunTaskInfo->SubOadIndex=0;
        }
    }
    else
    {
        RunTaskInfo->CsdIndex++;
    }
    *DI97Index = 0;
    if(*CsdIndex<pCbTask->CsdTask.RoadNum)
    {
        if(pCbTask->CsdTask.RoadTask[*CsdIndex].MainOad.nObjID !=0)
        {
            RunTaskInfo->SubOadIndex = 0;
        }
    }
    pComm_Info->MetStat = CB_STATE_SEND;
}


