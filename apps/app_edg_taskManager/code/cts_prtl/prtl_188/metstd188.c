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
#define _DLL_IMPLICIT_
#define FIRST_DEFINE_METPARAM
 #include "../../include/CtsAll.h"
 #include "../../include/metstd2007.h"



#define DEBUG_STD2007 //�汾�ȶ���ȥ������

#ifdef DEBUG_STD2007
#define STD2007_ENV  "metstd2007"
#endif

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

#define STD2007_SYNC                0x68



extern RS485_RS232_PARA Rs485ComNo[8];
extern uint8 ComNo[10];

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

void Prepare_std188_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
{


    uint16 sn;
    uint16 MeterIndex;
    OOP_METER_BASIC_T MeterBasicInfo;
    OOP_METER_EXTENDED_T extend;
    //task_id=pCbTask->TaskId;
    MeterIndex=RunTaskInfo->MeterIndex;
    sn=pCbTask->MsInfo[MeterIndex].Sn;
    //��ȡ��������Ϣ
    memset((void *)MtrPara, 0x0, sizeof(cbMtrPara));

    //��ȡ��������Ϣ
    taskmng_mtr_basic_info_get(sn,RunTaskInfo->logicid,&MeterBasicInfo);
    taskmng_mtr_extend_info_get(sn,RunTaskInfo->logicid,&extend);
    MtrPara->MtrNum=sn;
    //MtrPara->Speed=get_meter_read_speed(MeterBasicInfo.baud);
    MtrPara->Speed=MeterBasicInfo.baud;
    memcpy_r(MtrPara->MetAddr,MeterBasicInfo.tsa.add,6);
    memcpy((uint8 *)&MtrPara->MtrCommAddr,(uint8*)&MeterBasicInfo.tsa,sizeof(OOP_TSA_T));
    memcpy((uint8 *)&MtrPara->CjqCommAddr,(uint8*)&extend.add,sizeof(OOP_TSA_T));
    memset(Comm_Info_Oop,0,sizeof(S_COMM_INFO_MET));
 
    Comm_Info_Oop->pMtrPara = MtrPara;
    Comm_Info_Oop->MetStat = CB_STATE_INIT;
        
}

/****************************************************************************
*�������ƣ�Meter_645_07_Read_Item
*���ܣ�  645_07�������ͱ���
*���������
*���������
*���أ�
****************************************************************************/

void Meter_Std188_Read_Item(SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 *DI07Num,uint32 *DI07Buff)
{   

    uint32              DI07 = 0;
    uint16 CsdIndex,SubOadIndex,MeterIndex,pn,tmpMsIndex;
    int ret=0;
    int16 MsIndex;
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
        *DI07Num = GetDI188(pCbTask->AcqInfo,&pCbTask->CsdTask.RoadTask[CsdIndex],SubOadIndex,Comm_Info_Oop->cbLPPara.endNum,DI07Buff,pCbTask,RunTaskInfo,pipe);//��ȡ��������ݱ�ʶ

        if(*DI07Num == 0)
        {
            STD07_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);
            return;
        }
        else
        {
            DI07 = DI07Buff[*DI07Index];

            STD188_MakeFrameEx(MtrPara->MetAddr,DI07, STD2007_CMDID_READ, Comm_Info_Oop->pSndBuf, &Comm_Info_Oop->SndLen, Comm_Info_Oop,pCbTask,RunTaskInfo);
        }

        ret=rs485_RS232_send(pipe,15,3000,MtrPara->Speed,PARITY_EVEN,Comm_Info_Oop->pSndBuf, (int32)Comm_Info_Oop->SndLen);
        if(ret !=0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "����ʧ�� ret is %d\n",ret);
        }
        TASK_FMT_TRACE( pipe, REC_LOG, "������� %d ,���ķ���\n",ret);
        TASK_BUF_TRACE( pipe, REC_LOG, Comm_Info_Oop->pSndBuf, Comm_Info_Oop->SndLen);
        Comm_Info_Oop->MetStat = CB_STATE_RECV;         //���ͳɹ���׼������
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_SENDFRMNUM,NULL);
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastSendTime=time(NULL);
    }
    else
    {
        Comm_Info_Oop->MetStat = CB_STATE_FINISH;
        MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
        taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
        taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
    }
    if(Comm_Info_Oop->MetStat != CB_STATE_RECV )//0xff  �����������Ѿ����겻�ٴ��ڽ���״̬
    {
        Comm_Info_Oop->MetStat = CB_STATE_FINISH;       //Ĭ��Ϊ����״̬��ֻ���ڲ��ҵ����ʵ���������ת�����״̬�������������
    }
}


/****************************************************************************
*Meter_Std188_Read_Item_New
*���ܣ�ɽ��ˮ��ͨ���ɼ�������
*���������
*���������
*���أ�
****************************************************************************/

void Meter_Std188_Read_Item_New(SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 *DI07Num,uint32 *DI07Buff)
{   

    uint32              DI07 = 0;
    uint16 CsdIndex,SubOadIndex,MeterIndex,pn,tmpMsIndex;
    int ret=0;
    //int16 MsIndex;
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
        *DI07Num = GetDI188New(pCbTask->AcqInfo,&pCbTask->CsdTask.RoadTask[CsdIndex],SubOadIndex,Comm_Info_Oop->cbLPPara.endNum,DI07Buff,pCbTask,RunTaskInfo,pipe);//��ȡ��������ݱ�ʶ

        if(*DI07Num == 0)
        {
            STD07_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);
            return;
        }
        else
        {
            DI07 = DI07Buff[*DI07Index];

            STD188_MakeFrameEx_New(MtrPara->MetAddr,DI07, STD2007_CMDID_READ, Comm_Info_Oop->pSndBuf, &Comm_Info_Oop->SndLen, Comm_Info_Oop,pCbTask,RunTaskInfo);
        }

        ret=rs485_RS232_send(pipe,15,500,MtrPara->Speed,PARITY_EVEN,Comm_Info_Oop->pSndBuf, (int32)Comm_Info_Oop->SndLen);
        if(ret !=0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "����ʧ�� ret is %d\n",ret);
        }
        TASK_FMT_TRACE( pipe, REC_LOG, "������� %d ,���ķ���\n",ret);
        TASK_BUF_TRACE( pipe, REC_LOG, Comm_Info_Oop->pSndBuf, Comm_Info_Oop->SndLen);
        Comm_Info_Oop->MetStat = CB_STATE_RECV;         //���ͳɹ���׼������
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_SENDFRMNUM,NULL);
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastSendTime=time(NULL);
    }
    else
    {
        Comm_Info_Oop->MetStat = CB_STATE_FINISH;
        //MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
        //taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
        //taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
    }
    if(Comm_Info_Oop->MetStat != CB_STATE_RECV )//0xff  �����������Ѿ����겻�ٴ��ڽ���״̬
    {
        Comm_Info_Oop->MetStat = CB_STATE_FINISH;       //Ĭ��Ϊ����״̬��ֻ���ڲ��ҵ����ʵ���������ת�����״̬�������������
    }
}

int8 CJT188_CheckRspFrame(uint8 pipe, uint8 *pBuf, uint16 rcvLen, uint16 DI, S_COMM_INFO_MET* pCommInfo, uint16 *offset,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo)
{
    uint16  head_offset = 0; /* �������� */
    uint8   CS = 0;
    uint8   i;
    uint8   data_len = 0;
    uint8   meter_addr[7]= {0};
    uint8  CtrlCode;
    uint8 user_type, meter_type;

    //�û�����
    OOP_METER_BASIC_T basicinfo;
    uint16 MeterIndex,pn;


    //�û�����
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    taskmng_mtr_basic_info_get(pn,RunTaskInfo->logicid,&basicinfo);

    user_type = basicinfo.userType;
    //������
    if(user_type == 0xC0)
    {
        meter_type = 0x10;
    }
    else if(user_type == 0xD0)
    {
        meter_type = 0x20;
    }
    else
    {
        meter_type = 0x10;  //Ĭ��ˮ��
    }

    memcpy_r(meter_addr, &pCommInfo->pMtrPara->MtrCommAddr.add[0], 7);

    head_offset = 0;
    while((head_offset + AMR_MT_PRTL_MIN_LEN_188 - 1) < rcvLen)
    {
        /* 1 ��֡��ʼ��־0x68��ˮ������*/
        if ((pBuf[head_offset] != 0x68) || (pBuf[head_offset+1] != meter_type))
        {
            head_offset ++;
            continue;
        }
        /* 2 ��֡���� */
        if((rcvLen - head_offset) < AMR_MT_PRTL_MIN_LEN_188)
        {
            memset(pBuf, 0, rcvLen);
            return AMR_CHECK_RESULT_FAILUER_188;
        }

        data_len =  pBuf[head_offset + 10];          //188��Լ�����򳤶� ��֡��ʼ��־���10���ֽ�

        /* 3 �н�β0x16 */
        if (pBuf[head_offset + data_len + 12] != 0x16)
        {
            head_offset ++;
            continue;
        }

        /* 4 ��У��� */
        for (i=0; i < (data_len + 11); i++)
        {
            CS += pBuf[head_offset+i];
        }
        if (CS != pBuf[head_offset + data_len + 11])
        {
            memset(pBuf, 0, rcvLen);
            return AMR_CHECK_RESULT_FAILUER_188;
        }

        /* 6 �ߵ�����˵��֡�Ѿ���ȷ */

        /* 6.1 ˢ�µ�ַ�� */
        if(memcmp(&pBuf[head_offset+2], &meter_addr[0], 7) != 0)
        {
            return AMR_CHECK_RESULT_FAILUER_188;
        }

        /* 6.2 ˢ�¿�����*/
        CtrlCode = pBuf[head_offset+9];
        if ((CtrlCode & 0x80) == 0x00)
        {
            /* DataIndex + ���ݳ��� + ��Լ68��LEN�ĳ��� + CS��16 */
            head_offset = head_offset + data_len + 12 + 1;
            CS = 0;
            continue;
        }

        /* 6.5 �ж��Ƿ��� ����֡*/
        if (CtrlCode & 0x40)
        {
            memset(pBuf,0,rcvLen);
            return AMR_CHECK_RESULT_DENY_188;
        }

        /* 6.5 ��ͨ���������жϷ��ͺͽ��ܵ�֡��ʶ�Ƿ���ͬ �����ͬ����֡ʧ�ܴ���*/
        if(DI != *(uint16*)(pBuf+head_offset+11))
        {
            memset(pBuf,0,rcvLen);
            return AMR_CHECK_RESULT_FAILUER_188;
        }

        *offset = head_offset;

        return AMR_CHECK_RESULT_SUCCEED_188;
    }

    return AMR_CHECK_RESULT_FAILUER_188;
}


uint16 CJT188_AUTK_Recv_Deal(uint8 pipe, uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo)
{
    uint8 i=0, Length = 0, tmp;
    uint8 profileBuff[100];
    uint16 bias;
    uint8 testEnergy[20];
    
//    uint16 u16CSD_Len = 0U;
    
    DateTimeBcd_t storeTm = {0};
//    uint32 tm = 0U;
    uint8 user_type, meter_type;
    DateTimeBcd_t tmpTime;
    OOP_TI_T ti;
    time_t oritime;
    OOP_OAD_U mainOAD, subOAD;
    uint16 CsdIndex,SubOadIndex,subOadNum;
    DateTimeHex_t dateTimeHex;



    SubOadIndex=RunTaskInfo->SubOadIndex;
    CsdIndex=RunTaskInfo->CsdIndex;
    subOadNum = pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum;
    memset(profileBuff,0x00,sizeof(profileBuff));

    //�û�����
    OOP_METER_BASIC_T basicinfo;
    uint16 MeterIndex,pn;


    //�û�����
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    taskmng_mtr_basic_info_get(pn,RunTaskInfo->logicid,&basicinfo);
    //������
    user_type = basicinfo.userType;
    if(user_type == 0xC0)
    {
        meter_type = 0x10;
    }
    else if(user_type == 0xD0)
    {
        meter_type = 0x20;
    }
    else
    {
        meter_type = 0x10;  //Ĭ��ˮ��
    }
    TASK_FMT_TRACE( pipe, REC_LOG,"������meter_type %d\n",meter_type);
    TASK_BUF_TRACE( pipe, REC_LOG,pBuf, rcvLen);
    if((0x10 == (meter_type&0xF0)) || (0x30 == (meter_type&0xF0)))	//ˮ��
    {
        //��ǰ�ۼ���������λm*m*m-BCD
        if(pBuf[0] == 0x2C)
        {
            memcpy(profileBuff,pBuf+1,4);
        }
        else
        {
            memcpy(profileBuff,pBuf,4);
        }

        //�������ۼ�����������λm*m*m-BCD
        if(pBuf[5] == 0x2C)
        {
            memcpy(profileBuff+4,pBuf+6,4);
        }
        else
        {
            memcpy(profileBuff+4,pBuf+5,4);
        }

        //ʵʱʱ��-������ʱ����-BCD
        memcpy(profileBuff+8,pBuf+10,7);

        //״̬ST
        tmp = pBuf[17];
        *(profileBuff+15) = (tmp & 0x03);		//����״̬
        *(profileBuff+16) = ((tmp>>2) & 0x01);  //��ص�ѹ
        //XL_MemCpy(profileBuff+14,pBuf+16,2);
    }
    else if(0x20 == (meter_type&0xF0)) //�ȱ�----�����������ݺ͵�λ��˳�򻹲���ȷ�� ? ���е�λ���� ?
    {
        //����������-BCD
        memcpy(profileBuff,pBuf,4);

        //��ǰ����-BCD
        memcpy(profileBuff+4,pBuf+5,4);

        //�ȹ���-BCD
        memcpy(profileBuff+8,pBuf+10,4);

        //����-BCD
        memcpy(profileBuff+12,pBuf+15,4);

        //�ۼ�����-BCD
        if(pBuf[20] == 0x2C)
        {
            memcpy(profileBuff+16,pBuf+21,4);
        }
        else
        {
            memcpy(profileBuff+16,pBuf+20,4);
        }

        //��ˮ�¶�-BCD
        memcpy(profileBuff+20,pBuf+25,3);

        //��ˮ�¶�-BCD
        memcpy(profileBuff+23,pBuf+28,3);

        //�ۼƹ���ʱ��-BCD
        memcpy(profileBuff+26,pBuf+31,3);

        //ʵʱʱ��-������ʱ����-BCD
        memcpy(profileBuff+29,pBuf+34,7);

        //״̬ST
        //XL_MemCpy(profileBuff+35,pBuf+40,2);
        tmp = pBuf[41];
        *(profileBuff+36) = (tmp & 0x03);		//����״̬
        *(profileBuff+37) = ((tmp>>2) & 0x01);  //��ص�ѹ
    }
    TASK_BUF_TRACE( pipe, REC_LOG,profileBuff, 100);
    //�洢ʱ�괦��
    memcpy_r((uint8*)&tmpTime,RunTaskInfo->ThisTurnBeginTime,7);
    DT_DateTimeBCD2Time(&tmpTime,&oritime);
    TASK_FMT_TRACE( pipe, REC_LOG,"pCbTask->StoreTimeType is %d\n",pCbTask->StoreTimeType);
    if(pCbTask->StoreTimeType == STRTMTYP_FRZTIME)
    {
        ti.unit = pCbTask->DelayTimeUnit;
        ti.value = pCbTask->DelayTime;
        DT_TimeSubInv(&oritime, ti);
        DT_Time2DateTimeBCD(oritime,&storeTm);
    }
    else if(pCbTask->StoreTimeType== STRTMTYP_STARTTM)
    {
        ti.unit = pCbTask->DelayTimeUnit;
        ti.value = pCbTask->DelayTime;
        DT_TimeSubInv(&oritime, ti);
        storeTm = taskmng_acq_store_time_get(pCbTask->StoreTimeType,oritime);
    }
    else
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID is %d\n",pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID);
        if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5003 ||
                pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5004 ||
                pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5005 ||
                pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5006)
        {
            storeTm = taskmng_acq_store_time_get(pCbTask->StoreTimeType, oritime);
        }
        else
        {
            DT_Time2DateTimeBCD(oritime, &storeTm);
        }
    }

   
    {
        if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0)  //OAD
        {
            mainOAD.value = 0x00000000;
            
            subOAD = pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex];;

            bias = CaclBiasByOad_188(meter_type, subOAD, &Length);
            if(bias != 0xFFFF)
            {
                memset(testEnergy, 0xEE, sizeof(testEnergy));
                memcpy(testEnergy, &profileBuff[bias], Length);
                TASK_BUF_TRACE( pipe, REC_LOG,testEnergy, Length);
                STD188_AUTK_Recv(pipe, mainOAD, subOAD, testEnergy, sizeof(testEnergy), mCbMtrPara, NULL, pCbTask,RunTaskInfo);
            }
        }
        else //ROAD
        {       
            while(i < subOadNum)
            {
                mainOAD.value= pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.value;
                subOAD = pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i];

                if(subOAD.value == 0x20210200)
                {
                    i++;
                    DT_DateTimeBCD2Hex(&storeTm,&dateTimeHex);
                    memcpy(testEnergy,(uint8*)&dateTimeHex,sizeof(dateTimeHex));
                    TASK_BUF_TRACE( pipe, REC_LOG,testEnergy, sizeof(testEnergy));
                    STD188_AUTK_Recv(pipe, mainOAD, subOAD, testEnergy, sizeof(testEnergy), mCbMtrPara, NULL, pCbTask,RunTaskInfo);
                    continue;
                }
                bias = CaclBiasByOad_188(meter_type, subOAD, &Length);
                if(bias != 0xFFFF)
                {
                    memset(testEnergy, 0xEE, sizeof(testEnergy));
                    memcpy(testEnergy, &profileBuff[bias], Length);
                    TASK_BUF_TRACE( pipe, REC_LOG,testEnergy, Length);
                    TASK_FMT_TRACE( pipe, REC_LOG,"�ɼ���ʽpCbTask->AcqInfo.Acqtype is %d\n",pCbTask->AcqInfo.Acqtype);
                    if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
                    {
    
                        STD188_AUTK_Recv(pipe, mainOAD, subOAD, testEnergy, sizeof(testEnergy), mCbMtrPara, NULL, pCbTask,RunTaskInfo);
                    }
                    else
                    {
                        STD188_AUTK_Recv(pipe, mainOAD, subOAD, testEnergy, sizeof(testEnergy), mCbMtrPara, &storeTm, pCbTask,RunTaskInfo);
                    }
                }

                i++;
            }
            
        }
    }

    return 0;
}



uint16 CJT188_AUTK_Recv_Deal_New(uint8 pipe, uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo)
{
    uint8 i=0, Length = 0;
    uint8 profileBuff[100];
    uint16 bias;
    uint8 testEnergy[20];
    uint8                tmpBufer[255];           //Э��ת������ʱ��255���ֽ�
    
//    uint16 u16CSD_Len = 0U;
    
    DateTimeBcd_t storeTm = {0};
//    uint32 tm = 0U;
    uint8 user_type, meter_type;
    DateTimeBcd_t tmpTime;
    OOP_TI_T ti;
    time_t oritime;
    OOP_OAD_U mainOAD, subOAD;
    uint16 CsdIndex,subOadNum;
    DateTimeHex_t dateTimeHex;

    //SubOadIndex=RunTaskInfo->SubOadIndex;
    CsdIndex=RunTaskInfo->CsdIndex;
    subOadNum = pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum;
    mainOAD.value= pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.value;
    memset(profileBuff,0x00,sizeof(profileBuff));

    //�û�����
    OOP_METER_BASIC_T basicinfo;
    uint16 MeterIndex,pn;

    memset(tmpBufer,0x00,255);
    memcpy(tmpBufer,pBuf,rcvLen);
    ReduceGap(tmpBufer, rcvLen, 0x33);
    //�û�����
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    taskmng_mtr_basic_info_get(pn,RunTaskInfo->logicid,&basicinfo);
    //������
    user_type = basicinfo.userType;
    if(user_type == 0xC0)
    {
        meter_type = 0x00;
    }
    else if(user_type == 0xD0)
    {
        meter_type = 0x50;
    }
    else
    {
        meter_type = 0x00;  //Ĭ��ˮ��
    }
    TASK_FMT_TRACE( pipe, REC_LOG,"������meter_type %d\n",meter_type);
    TASK_BUF_TRACE( pipe, REC_LOG,tmpBufer, rcvLen);
    if((0x00 == (meter_type&0xF0)) || (0x30 == (meter_type&0xF0)))	//ˮ��
    {
        //��ǰ�ۼ���������λm*m*m-BCD
        if(mainOAD.value==0x00000000)
        {
            //if(pBuf[6] == 0x2C)
            {
                memcpy(profileBuff,tmpBufer+7,4);
            }
            //�������ۼ�����������λm*m*m-BCD
            //if(pBuf[11] == 0x2C)
            {
                memcpy(profileBuff+4,tmpBufer+12,4);
            }
            //�ۼƹ���ʱ��
            memcpy(profileBuff+8,tmpBufer+37,3);
            if(check_is_all_ch(tmpBufer+37,0xEE,3)==TRUE)
            {
                memset(profileBuff+8,0x00,3);
            }
            //ʵʱʱ��-������ʱ����-BCD
            memcpy(profileBuff+11,tmpBufer+40,7);

            //״̬ST
            //tmp = pBuf[17];
            *(profileBuff+15) = tmpBufer[47];		//����״̬
            *(profileBuff+16) = tmpBufer[48];  //��ص�ѹ
        }
        else
        {
                        //if(pBuf[6] == 0x2C)
            {
                memcpy(profileBuff,tmpBufer+10,4);
            }
            //�������ۼ�����������λm*m*m-BCD
            //if(pBuf[11] == 0x2C)
            {
                memcpy(profileBuff+4,tmpBufer+15,4);
            }
            //�ۼƹ���ʱ��
            memcpy(profileBuff+8,tmpBufer+40,3);
            //ʵʱʱ��-������ʱ����-BCD
            memcpy(profileBuff+11,tmpBufer+43,7);

            //״̬ST
            //tmp = pBuf[17];
            *(profileBuff+15) = tmpBufer[50];		//����״̬
            *(profileBuff+16) = tmpBufer[51];  //��ص�ѹ

        }
        //XL_MemCpy(profileBuff+14,pBuf+16,2);
    }
    else if(0x50 == (meter_type&0xF0)) //�ȱ�----�����������ݺ͵�λ��˳�򻹲���ȷ�� ? ���е�λ���� ?
    {
        if(mainOAD.value ==0x000000)
        {
            //����������-BCD
            memcpy(profileBuff,tmpBufer+7,4);

            //��ǰ����-BCD
            memcpy(profileBuff+4,tmpBufer+12,4);

            //�ȹ���-BCD
            memcpy(profileBuff+8,tmpBufer+17,4);

            //����-BCD
            memcpy(profileBuff+12,tmpBufer+22,4);

            //�ۼ�����-BCD
            //if(pBuf[20] == 0x2C)
            {
                memcpy(profileBuff+16,tmpBufer+27,4);
            }
    //        else
    //        {
    //            memcpy(profileBuff+16,pBuf+20,4);
    //        }

            //��ˮ�¶�-BCD
            memcpy(profileBuff+20,tmpBufer+31,3);

            //��ˮ�¶�-BCD
            memcpy(profileBuff+23,tmpBufer+34,3);

            //�ۼƹ���ʱ��-BCD
            memcpy(profileBuff+26,tmpBufer+37,3);

            //ʵʱʱ��-������ʱ����-BCD
            memcpy(profileBuff+29,tmpBufer+40,7);

            //״̬ST
            //XL_MemCpy(profileBuff+35,pBuf+40,2);
            //tmp = pBuf[41];
            *(profileBuff+36) = tmpBufer[47];		//����״̬
            *(profileBuff+37) = tmpBufer[48];  //��ص�ѹ
        }
        else
        {
            //����������-BCD
            memcpy(profileBuff,tmpBufer+10,4);

            //��ǰ����-BCD
            memcpy(profileBuff+4,tmpBufer+15,4);

            //�ȹ���-BCD
            memcpy(profileBuff+8,tmpBufer+20,4);

            //����-BCD
            memcpy(profileBuff+12,tmpBufer+25,4);

            //�ۼ�����-BCD
            //if(pBuf[20] == 0x2C)
            {
                memcpy(profileBuff+16,tmpBufer+30,4);
            }
    //        else
    //        {
    //            memcpy(profileBuff+16,pBuf+20,4);
    //        }

            //��ˮ�¶�-BCD
            memcpy(profileBuff+20,tmpBufer+34,3);

            //��ˮ�¶�-BCD
            memcpy(profileBuff+23,tmpBufer+37,3);

            //�ۼƹ���ʱ��-BCD
            memcpy(profileBuff+26,tmpBufer+40,3);

            //ʵʱʱ��-������ʱ����-BCD
            memcpy(profileBuff+29,tmpBufer+43,7);

            //״̬ST
            //XL_MemCpy(profileBuff+35,pBuf+40,2);
            //tmp = pBuf[41];
            *(profileBuff+36) = tmpBufer[50];		//����״̬
            *(profileBuff+37) = tmpBufer[51];  //��ص�ѹ

        }
    }
    TASK_BUF_TRACE( pipe, REC_LOG,profileBuff, 100);
    //�洢ʱ�괦��
    memcpy_r((uint8*)&tmpTime,RunTaskInfo->ThisTurnBeginTime,7);
    DT_DateTimeBCD2Time(&tmpTime,&oritime);
    TASK_FMT_TRACE( pipe, REC_LOG,"pCbTask->StoreTimeType is %d\n",pCbTask->StoreTimeType);
    if(pCbTask->StoreTimeType == STRTMTYP_FRZTIME)
    {
        ti.unit = pCbTask->DelayTimeUnit;
        ti.value = pCbTask->DelayTime;
        DT_TimeSubInv(&oritime, ti);
        DT_Time2DateTimeBCD(oritime,&storeTm);
    }
    else if(pCbTask->StoreTimeType== STRTMTYP_STARTTM)
    {
        ti.unit = pCbTask->DelayTimeUnit;
        ti.value = pCbTask->DelayTime;
        DT_TimeSubInv(&oritime, ti);
        storeTm = taskmng_acq_store_time_get(pCbTask->StoreTimeType,oritime);
    }
    else
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID is %d\n",pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID);
        if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5003 ||
                pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5004 ||
                pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5005 ||
                pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5006)
        {
            storeTm = taskmng_acq_store_time_get(pCbTask->StoreTimeType, oritime);
        }
        else
        {
            DT_Time2DateTimeBCD(oritime, &storeTm);
        }
    }

   
    {
        if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0)  //OAD
        {
            while(i < subOadNum)
            {
                mainOAD.value = 0x00000000;
                
                subOAD = pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i];;

                bias = CaclBiasByOad_188_new(meter_type, subOAD, &Length,mainOAD);
                TASK_FMT_TRACE( pipe, REC_LOG,"subOAD.value is %x,bias is %d,mainOAD.value %x\n",subOAD.value,bias,mainOAD.value);
                
                if(bias != 0xFFFF)
                {
                    memset(testEnergy, 0xEE, sizeof(testEnergy));
                    memcpy(testEnergy, &profileBuff[bias], Length);
                    TASK_BUF_TRACE( pipe, REC_LOG,testEnergy, Length);
                    STD188_AUTK_Recv_New(pipe, mainOAD, subOAD, testEnergy, sizeof(testEnergy), mCbMtrPara, NULL, pCbTask,RunTaskInfo);
                }
                i++;
            }
        }
        else //ROAD
        {       
            while(i < subOadNum)
            {
                mainOAD.value= pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.value;
                subOAD = pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[i];

                if(subOAD.value == 0x20210200)
                {
                    i++;
                    DT_DateTimeBCD2Hex(&storeTm,&dateTimeHex);
                    memcpy(testEnergy,(uint8*)&dateTimeHex,sizeof(dateTimeHex));
                    TASK_BUF_TRACE( pipe, REC_LOG,testEnergy, sizeof(testEnergy));
                    STD188_AUTK_Recv_New(pipe, mainOAD, subOAD, testEnergy, sizeof(testEnergy), mCbMtrPara, NULL, pCbTask,RunTaskInfo);
                    continue;
                }
                bias = CaclBiasByOad_188_new(meter_type, subOAD, &Length,mainOAD);
                TASK_FMT_TRACE( pipe, REC_LOG,"subOAD.value is %x,bias is %d,mainOAD.value %x\n",subOAD.value,bias,mainOAD.value);
                if(bias != 0xFFFF)
                {
                    memset(testEnergy, 0xEE, sizeof(testEnergy));
                    memcpy(testEnergy, &profileBuff[bias], Length);
                    TASK_BUF_TRACE( pipe, REC_LOG,testEnergy, Length);
                    TASK_FMT_TRACE( pipe, REC_LOG,"�ɼ���ʽpCbTask->AcqInfo.Acqtype is %d\n",pCbTask->AcqInfo.Acqtype);
                    if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
                    {
    
                        STD188_AUTK_Recv_New(pipe, mainOAD, subOAD, testEnergy, sizeof(testEnergy), mCbMtrPara, NULL, pCbTask,RunTaskInfo);
                    }
                    else
                    {
                        STD188_AUTK_Recv_New(pipe, mainOAD, subOAD, testEnergy, sizeof(testEnergy), mCbMtrPara, &storeTm, pCbTask,RunTaskInfo);
                    }
                }

                i++;
            }
            
        }
    }

    return 0;
}


/****************************************************************************
*���ƣ�STD97_ToNextReadItem
*���ܣ���ȡ��һ����OAD,���߻�ȡ��һ��CSD
*���������
*���������
*����
*���أ���
****************************************************************************/
void  CJT188_ToNextReadItem(BOOL NextCSDFlg,S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
{

    if(pCbTask==NULL )
        return;

//    if(NextCSDFlg == FALSE)
//    {
//        //if (pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.value > 0)/* is ROAD */
//        {
//            //if((pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID & 0xF000) == 0x5000)//not event class
//            {
////                if((RunTaskInfo->SubOadIndex+1) < pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum)/* index of sub OAD not exceed the range */
////                {
////                    RunTaskInfo->SubOadIndex++;/* ת����һ����OAD */
////                    pComm_Info->MetStat = CB_STATE_SEND;
////                    return;
////                }
////                else
//                {
//                    RunTaskInfo->CsdIndex++;
//                    RunTaskInfo->SubOadIndex=0;
//                    pComm_Info->MetStat = CB_STATE_SEND;
//                    return;
//                }
//            }
//        }
//    }
//    //if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0)
//    {
//        RunTaskInfo->SubOadIndex++;
//        if(RunTaskInfo->SubOadIndex>=pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum)
//        {
//            RunTaskInfo->CsdIndex++;
//            RunTaskInfo->SubOadIndex=0;
//        }
//    }
    RunTaskInfo->CsdIndex++;
    RunTaskInfo->SubOadIndex=0;
    pComm_Info->MetStat = CB_STATE_SEND;
}

/****************************************************************************
*�������ƣ�Deal_Receive_Result_645_07
*���ܣ�����  645_07�������ձ��ĵĽ��
*���������
*���������
*���أ�
****************************************************************************/

void Meter_188_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff)
{
    //uint8               *ptr=NULL;
    BOOL        actResult = TRUE;       //�Ի��봦��Ľ��
    eOopCommRslt_t ret = 0;

    //uint32              DI07 = 0;                                   //���ݱ�ʶ

    //uint8 CsdIndex;
    uint16 MeterIndex,pn;
    //uint16 reallen=0;
    //uint16 result=0;
    uint16              frameoffset = 0;
    
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    //tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);

    //SubOadIndex=RunTaskInfo->SubOadIndex;
    //CsdIndex=RunTaskInfo->CsdIndex;

    memset(Comm_Info_Oop->pRcvBuf, 0x00, sizeof(Comm_Info_Oop->pRcvBuf));


    Comm_Info_Oop->RcvLen = rs485_RS232_recv(pipe,Comm_Info_Oop->pRcvBuf, sizeof(Comm_Info_Oop->pRcvBuf));
    TASK_FMT_TRACE( pipe, REC_LOG,"pn %d �յ����� \n",pn);
    TASK_BUF_TRACE( pipe, REC_LOG,Comm_Info_Oop->pRcvBuf, Comm_Info_Oop->RcvLen);
    
    if(Comm_Info_Oop->RcvLen < AMR_MT_PRTL_MIN_LEN_188) // || ptr == NULL)
    {
        actResult = FALSE;              //���û�л��� ���� �����ʽ�Ƿ�
        ret = AMR_CHECK_RESULT_FAILUER_188;
        TASK_FMT_TRACE( pipe, REC_LOG,"pn �����쳣 \n",pn);
    }
    else
    {
        ret = CJT188_CheckRspFrame(pipe, Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, DI07Buff[*DI07Index], Comm_Info_Oop, &frameoffset,pCbTask,RunTaskInfo);
        TASK_FMT_TRACE( pipe, REC_LOG,"������� ret %d\n",ret);
        if(ret == AMR_CHECK_RESULT_SUCCEED_188)    //��Ч,�������ݷ���
        {
            Comm_Info_Oop->ErrFrame=0;
            CJT188_AUTK_Recv_Deal(pipe,Comm_Info_Oop->pRcvBuf+frameoffset+14,Comm_Info_Oop->RcvLen-frameoffset-14-2,*MtrPara,Comm_Info_Oop,pCbTask,RunTaskInfo);
            actResult = TRUE;
        }
        else if(ret == AMR_CHECK_RESULT_FAILUER_188) //����Ч���ݣ���ֵ�������Ĳ�һ��
        {
            Comm_Info_Oop->ErrFrame=0;
            Comm_Info_Oop->ErrTimes = ERR_TIMES_MAX;		//��ʧ�ܴ��������������ԣ�ֱ�ӽ����Դ�����Ϊ���ֵ
            actResult = FALSE;
        }
        else if(ret == AMR_CHECK_RESULT_DENY_188)                 //����֡
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

if(actResult)                          //��֡������ȷ
{
    CJT188_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);
    Comm_Info_Oop->ComFrame++;          //��֡������
    
    Comm_Info_Oop->ErrFrame = 0;        //ʧ��֡���ʹ���֡�����㣬׼�����¼���
    Comm_Info_Oop->ErrTimes = 0;                    

}
else                                            //��֡����ʧ��
{

    if(Comm_Info_Oop->ErrTimes >= ERR_TIMES_MAX)        //ͬһ֡��ʧ�ܴ���
    {
        if(Comm_Info_Oop->ErrFrame >= ERR_FRAME_MAX)	  //ʧ�ܴ������࣬���ټ�����׼���˳�
        {
            Comm_Info_Oop->ErrFrame = ERR_FRAME_MAX;
            Comm_Info_Oop->MetStat = CB_STATE_FINISH;
            return;
        }
        else
        {
            if(AMR_CHECK_RESULT_FAILUER_188 == ret)                                                  //û�н��յ�����
            {
                Comm_Info_Oop->ErrFrame++;                        //ʧ��֡������
                Comm_Info_Oop->ComFrame++;                        //��֡������
            }

            Comm_Info_Oop->ErrTimes = 0;                         //���Դ�������
            CJT188_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);//�������һ��������ط��ϣ��������������б�
        }
    }
    else
    {
        if(ret==AMR_CHECK_RESULT_FAILUER_188)                                                           //û�н�������
        {
            Comm_Info_Oop->ErrTimes++;                              //�ط���ǰ������
        }
    }
}
    Comm_Info_Oop->MetStat = CB_STATE_SEND;                    //ת�뷢������
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

uint8 STD2007_CheckRspFrame_Ex(uint8 pipe, uint8 *pBuf, uint16 rcvLen, uint8 bStdType,S_COMM_INFO_MET* pCommInfo)
{
    uint8   frameCs;
    uint16  frameLen;
    //�ȽϷ��ص�ַ
    
    frameLen = 0x0c + pBuf[9];
    frameCs = CalCs(pBuf, frameLen - 2);

    if((pBuf[0] == STD2007_SYNC)
            && (pBuf[7] == pBuf[0]) /*-����ϵͳ����ַ��0��AA�ɱ�-*/
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
            if((pBuf[8] & 0xF0) == 0xD0)        /*-�쳣Ӧ��֡-*/
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
            if(frameLen >= 0x0E)
            {
                if(0x90 == (pBuf[8] & 0xF0))        /*-�޺�������֡-*/
                {
                    return 1;
                }
                else
                {
                    if(0xB0 == (pBuf[8] & 0xF0))    /*-�к�������֡-*/
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
*�������ƣ�Deal_Receive_Result_645_07
*���ܣ�����  645_07�������ձ��ĵĽ��
*���������
*���������
*���أ�
****************************************************************************/

CB_STATE Deal_Receive_Result_188(S_COMM_INFO_MET *Comm_Info_Oop,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,BOOL actResult,uint8 pipe)
{
    uint16 pn,tmpMsIndex,MeterIndex;
    
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);


    eOopCommRslt_t ret = 0;
    if(actResult)                          //��֡������ȷ
    {
//      printf("Deal_Receive_Result_645_07 is BEGIN1111\n");
        CJT188_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);
        Comm_Info_Oop->ComFrame++;          //��֡������
    
        Comm_Info_Oop->ErrFrame = 0;        //ʧ��֡���ʹ���֡�����㣬׼�����¼���
        Comm_Info_Oop->ErrTimes = 0;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt =0 ;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = TRUE;
    }
    else                                            //��֡����ʧ��
    {
        TASK_FMT_TRACE( pipe, COMM_LOG,"ERR_FRAME_MAX is %d,ERR_TIMES_MAX is %d\n", ERR_FRAME_MAX,ERR_TIMES_MAX);
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
                CJT188_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);//�������һ��������ط��ϣ��������������б�
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

void Meter_188_Recvive_Data_New(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff)
{
    //uint8               *ptr=NULL;
    BOOL        actResult = TRUE;       //�Ի��봦��Ľ��
    eOopCommRslt_t ret = 0;

    //uint32              DI07 = 0;                                   //���ݱ�ʶ

    //uint8 CsdIndex;
    uint16 MeterIndex,pn;
    uint16 reallen=0;
    //uint16 result=0;
    uint16              frameoffset = 0;
    
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    //tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);

    uint8               *ptr=NULL;


    memset(Comm_Info_Oop->pRcvBuf, 0x00, sizeof(Comm_Info_Oop->pRcvBuf));


    Comm_Info_Oop->RcvLen = rs485_RS232_recv(pipe,Comm_Info_Oop->pRcvBuf, sizeof(Comm_Info_Oop->pRcvBuf));
    TASK_FMT_TRACE( pipe, REC_LOG,"pn %d �յ����� \n",pn);
    TASK_BUF_TRACE( pipe, REC_LOG,Comm_Info_Oop->pRcvBuf, Comm_Info_Oop->RcvLen);
    
    if(Comm_Info_Oop->RcvLen > 9)   //��Ч���ĵ���С����Ϊ9���ֽ�
    {
       // gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastRecvTime=time(NULL);
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_RECVFRMNUM,NULL);
        ptr = (uint8*)commfun_645_frame_get((char*)Comm_Info_Oop->pRcvBuf, 0x68, Comm_Info_Oop->RcvLen,&reallen);//�ҵ�645���ĵ�68ͷ
    }
    if(Comm_Info_Oop->RcvLen <= 0 || ptr == NULL)
    {
        actResult = FALSE;              //���û�л��� ���� �����ʽ�Ƿ�
    }
    else
    {
        
        Comm_Info_Oop->RcvLen = reallen;//��645���ĳ���
        if(Comm_Info_Oop->RcvLen>255)
        {
            Comm_Info_Oop->RcvLen = 255;
        }
        memmove(Comm_Info_Oop->pRcvBuf, ptr, Comm_Info_Oop->RcvLen);
        ret = STD2007_CheckRspFrame_Ex(pipe, Comm_Info_Oop->pRcvBuf, (uint16)Comm_Info_Oop->RcvLen, 0,Comm_Info_Oop);
        TASK_FMT_TRACE( pipe, REC_LOG,"������� ret %d\n",ret);
         if((ret == 1)||(ret == 2))  //��Ч,�������ݷ���
        {
            Comm_Info_Oop->ErrFrame=0;
            CJT188_AUTK_Recv_Deal_New(pipe,Comm_Info_Oop->pRcvBuf+frameoffset+14,Comm_Info_Oop->RcvLen-frameoffset-14-2,*MtrPara,Comm_Info_Oop,pCbTask,RunTaskInfo);
            actResult = TRUE;
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
Deal_Receive_Result_188(Comm_Info_Oop,pCbTask,RunTaskInfo,actResult,pipe);

//if(actResult)                          //��֡������ȷ
//{
//    CJT188_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);
//    Comm_Info_Oop->ComFrame++;          //��֡������
//    
//    Comm_Info_Oop->ErrFrame = 0;        //ʧ��֡���ʹ���֡�����㣬׼�����¼���
//    Comm_Info_Oop->ErrTimes = 0;                    
//
//}
//else                                            //��֡����ʧ��
//{
//
//    if(Comm_Info_Oop->ErrTimes >= ERR_TIMES_MAX)        //ͬһ֡��ʧ�ܴ���
//    {
//        if(Comm_Info_Oop->ErrFrame >= ERR_FRAME_MAX)	  //ʧ�ܴ������࣬���ټ�����׼���˳�
//        {
//            Comm_Info_Oop->ErrFrame = ERR_FRAME_MAX;
//            Comm_Info_Oop->MetStat = CB_STATE_FINISH;
//            return;
//        }
//        else
//        {
//            if(AMR_CHECK_RESULT_FAILUER_188 == ret)                                                  //û�н��յ�����
//            {
//                Comm_Info_Oop->ErrFrame++;                        //ʧ��֡������
//                Comm_Info_Oop->ComFrame++;                        //��֡������
//            }
//
//            Comm_Info_Oop->ErrTimes = 0;                         //���Դ�������
//            CJT188_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo);//�������һ��������ط��ϣ��������������б�
//        }
//    }
//    else
//    {
//        if(ret==AMR_CHECK_RESULT_FAILUER_188)                                                           //û�н�������
//        {
//            Comm_Info_Oop->ErrTimes++;                              //�ط���ǰ������
//        }
//    }
//}
//    Comm_Info_Oop->MetStat = CB_STATE_SEND;                    //ת�뷢������
//    return;
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
void STD188_MakeFrameEx(uint8 *pMetAddr, uint32 fraDI, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo)
{
    uint8   CS = 0;                       //У����
    uint8 tmpSnd[30];
    uint8 i;
    uint8 offset = 0;
    static uint8 ser = 0;
    uint8 user_type, meter_type;
    OOP_METER_BASIC_T basicinfo;
    uint16 MeterIndex,pn;


    //�û�����
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    taskmng_mtr_basic_info_get(pn,RunTaskInfo->logicid,&basicinfo);

    user_type = basicinfo.userType;
    //������
    if(user_type == 0xC0)
    {
        meter_type = 0x10;
    }
    else if(user_type == 0xD0)
    {
        meter_type = 0x20;
    }
    else
    {
        meter_type = 0x10;  //Ĭ��ˮ��
    }

    memset(tmpSnd, 0x00, sizeof(tmpSnd));
    for(i = 0; i < 4; i++)
        tmpSnd[i] = 0xFE;

    offset = 4;

    tmpSnd[offset++] = 0x68;
    //������
    tmpSnd[offset++] = meter_type;
    /*- ���ַ -*/
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[6];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[5];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[4];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[3];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[2];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[1];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[0];
    tmpSnd[offset++] = 0x01;              //������
    tmpSnd[offset++] = 0x03;           //�����򳤶�
    tmpSnd[offset++] = (uint8)(fraDI&0x00FF);            //���ݱ�ʶ
    tmpSnd[offset++] = (uint8)(fraDI>>8);
    //���к�
    ser = ser%256 + 1;
    tmpSnd[offset++] = ser;
    //У���
    for (i = 4; i < offset; i++)
    {
        CS += tmpSnd[i];
    }
    tmpSnd[offset++] = CS;
    tmpSnd[offset++] = 0x16;

    memcpy(pBuf, tmpSnd, offset);        /*- ����ͻ�����pBuf*/
    *pSndLen = offset;

    return;

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
void STD188_MakeFrameEx_New(uint8 *pMetAddr, uint32 fraDI, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo)
{
    uint8   CS = 0;                       //У����
    uint8 tmpSnd[30];
    uint8 i;
    uint8 offset = 0;
//    static uint8 ser = 0;
    uint8 user_type, meter_type;
    OOP_METER_BASIC_T basicinfo;
    uint16 MeterIndex,pn;


    //�û�����
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    taskmng_mtr_basic_info_get(pn,RunTaskInfo->logicid,&basicinfo);

    user_type = basicinfo.userType;
    //������
    if(user_type == 0xC0)
    {
        meter_type = 0x00;
    }
    else if(user_type == 0xD0)
    {
        meter_type = 0x50;
    }
    else
    {
        meter_type = 0x00;  //Ĭ��ˮ��
    }

    memset(tmpSnd, 0x00, sizeof(tmpSnd));
    for(i = 0; i < 4; i++)
        tmpSnd[i] = 0xFE;

    offset = 4;

    tmpSnd[offset++] = 0x68;
    /*- �ɼ�����ַ -*/
    tmpSnd[offset++] = pComm_Info->pMtrPara->CjqCommAddr.add[3];
    tmpSnd[offset++] = pComm_Info->pMtrPara->CjqCommAddr.add[2];
    tmpSnd[offset++] = pComm_Info->pMtrPara->CjqCommAddr.add[1];
    tmpSnd[offset++] = pComm_Info->pMtrPara->CjqCommAddr.add[0];
    tmpSnd[offset++] = pComm_Info->pMtrPara->CjqCommAddr.add[4];
    tmpSnd[offset++] = pComm_Info->pMtrPara->CjqCommAddr.add[5];
    tmpSnd[offset++] = 0x68;
    tmpSnd[offset++] = 0x11;              //������
    tmpSnd[offset++] = 0x0c;           //�����򳤶�
    memcpy((uint8*)&tmpSnd[offset],(uint8*)&fraDI,4);//���ݱ�ʶ
    offset =offset+4;
    //������
    tmpSnd[offset++] = meter_type;
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[6];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[5];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[4];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[3];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[2];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[1];
    tmpSnd[offset++] = pComm_Info->pMtrPara->MtrCommAddr.add[0];

    for(i=14;i<offset;i++)
    {
        tmpSnd[i]=tmpSnd[i]+0x33;//��33
    }
    //У���
    for (i = 4; i < offset; i++)
    {
        CS += tmpSnd[i];
    }
    tmpSnd[offset++] = CS;
    tmpSnd[offset++] = 0x16;

    memcpy(pBuf, tmpSnd, offset);        /*- ����ͻ�����pBuf*/
    *pSndLen = offset;

    return;

}

/****************************************************************************
*ģ���ţ�
*���ƣ�
*���ܣ�����OAD���� OAD�����������ݿ��е�ƫ�ƣ���07�������ߣ��¼���¼ʹ��
*�����������OAD����OAD
*������������������ݿ��е�ƫ��
*����
*���أ���
****************************************************************************/
uint16 CaclBiasByOad_188(uint8 type, OOP_OAD_U subOad, uint8 *pLen)
{
    //uint16 oad_id;
    OadInfoMap  pOadInfo;;
    uint16 offSet=0U;
    const MapOopTo645_NEW *pMapOopTo645=NULL;


    if((0x10 == (type&0xF0)) || (0x30 == (type&0xF0)))
    {
        switch(subOad.value)
        {
        case 0x25000200:
        case 0x25010200:
            offSet = 0;
            break;
        case 0x25040200:
            offSet = 8;
            break;
        case 0x25060200:
        case 0x25060201:
            offSet = 15;
            break;

        case 0x25060202:
            offSet = 16;
            break;

        default:
            offSet = 0xFFFF;
            break;
        }
    }
    else if(0x20 == (type&0xF0))
    {
        switch(subOad.value)
        {
        case 0x25000200:		//�ۼ�ˮ���ȣ�����
            offSet = 16;
            break;

        case 0x25020200:        //�ۼ�����
            offSet = 4;
            break;

        case 0x25030200:		//�ȹ���
            offSet = 8;
            break;

        case 0x25040200:		//�ۼƹ���ʱ��
            offSet = 26;
            break;

        case 0x25050200:		//ˮ��
        case 0x25050201:		//��ˮ�¶�
            offSet = 20;
            break;

        case 0x25050202:		//��ˮ�¶�
            offSet = 23;
            break;

        case 0x25060200:
        case 0x25060201:
            offSet = 36;
            break;

        case 0x25060202:
            offSet = 37;
            break;

        default:
            offSet = 0xFFFF;
            break;
        }

    }
    else
    {
        offSet = 0xFFFF;
    }
    if(FALSE==taskmng_oad_info_get(subOad,&pOadInfo))
    {
        return -1;
    } 

    {
        pMapOopTo645 = MapOopTo188_RT;
    }
 

    while(pMapOopTo645->Oad.value != 0xFFFFFFFF)
    {

        if(subOad.value == pMapOopTo645->Oad.value) //�¼�������Ϊ0�������Ϊ1Ҳ��
        {
            if(pLen)
            {
                *pLen = pMapOopTo645->DataLen;
            }
            break;
        }


        pMapOopTo645++;
    }
    if(pMapOopTo645->Oad.value==0xFFFFFFFF)
    {
        return 0xFFFFU;
    }
    return offSet;
}


/****************************************************************************
*ģ���ţ�
*���ƣ�
*���ܣ�����OAD���� OAD�����������ݿ��е�ƫ�ƣ���07�������ߣ��¼���¼ʹ��
*�����������OAD����OAD
*������������������ݿ��е�ƫ��
*����
*���أ���
****************************************************************************/
uint16 CaclBiasByOad_188_new(uint8 type, OOP_OAD_U subOad, uint8 *pLen, OOP_OAD_U mainoad)
{
    //uint16 oad_id;
    OadInfoMap  pOadInfo;;
    uint16 offSet=0U;
    const MapOopTo645_NEW *pMapOopTo645=NULL;


    if((0x00 == (type&0xF0)) || (0x30 == (type&0xF0)))
    {
        switch(subOad.value)
        {
        case 0x25000200:
        case 0x25010200:
            offSet = 0;
            break;
        case 0x25040200:
            offSet = 8;
            break;
        case 0x25060200:
        case 0x25060201:
            offSet = 15;
            break;

        case 0x25060202:
            offSet = 16;
            break;

        default:
            offSet = 0xFFFF;
            break;
        }
    }
    else if(0x50 == (type&0xF0))
    {
        switch(subOad.value)
        {
        case 0x25000200:		//�ۼ�ˮ���ȣ�����
            offSet = 16;
            break;

        case 0x25020200:        //�ۼ�����
            offSet = 4;
            break;

        case 0x25030200:		//�ȹ���
            offSet = 8;
            break;

        case 0x25040200:		//�ۼƹ���ʱ��
            offSet = 26;
            break;

        case 0x25050200:		//ˮ��
        case 0x25050201:		//��ˮ�¶�
            offSet = 20;
            break;

        case 0x25050202:		//��ˮ�¶�
            offSet = 23;
            break;

        case 0x25060200:        //״̬
        case 0x25060201:
            offSet = 36;
            break;

        case 0x25060202:
            offSet = 37;
            break;

        default:
            offSet = 0xFFFF;
            break;
        }

    }
    else
    {
        offSet = 0xFFFF;
    }
    if(FALSE==taskmng_oad_info_get(subOad,&pOadInfo))
    {
        return -1;
    } 
    if(mainoad.value ==0x00000000)
    {
        pMapOopTo645 = MapOopTo188_RT_New;
    }
    else
    {
        pMapOopTo645 = MapOopTo188_DAY;
        //offSet =offSet+3;

    }

    while(pMapOopTo645->Oad.value != 0xFFFFFFFF)
    {

        if(subOad.value == pMapOopTo645->Oad.value) //�¼�������Ϊ0�������Ϊ1Ҳ��
        {
            if(pLen)
            {
                *pLen = pMapOopTo645->DataLen;
            }
            break;
        }


        pMapOopTo645++;
    }
    if(pMapOopTo645->Oad.value==0xFFFFFFFF)
    {
        return 0xFFFFU;
    }
    return offSet;
}

/****************************************************************************
���ƣ� STD64507_AUTK_Recv
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
uint16 STD188_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
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
    //uint8 MtrAddr[6];
    uint16 MsIndex;
    OOP_METER_BASIC_T basicinfo;
    
//  CsdIndex=RunTaskInfo->CsdIndex;
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    taskmng_mtr_basic_info_get(pn,RunTaskInfo->logicid,&basicinfo);
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
        pMapOopTo645 = Find188InfoPtrByOad(tmpOad,subOAD);//

    }
    else
    {
        pMapOopTo645 = Find188InfoPtrByOad(mainOAD,subOAD);//
    }
    if(pMapOopTo645==NULL)
    {
            return 0;
    }
    if(FALSE==taskmng_oad_info_get(findOAD, &pOadInfo))
    {
        return 0;
    }

    //������-0x33
    //ReduceGap(tmpBufer, rcvLen, STD2007_GAP_VALUE);
    memcpy(sourcedata.sourcebuffer,tmpBufer,rcvLen);
    sourcedata.reclen=rcvLen;
    sourcedata.pwrType = basicinfo.pwrType;
    Convert_std188_To_Struct(sourcedata ,desbuffer,&desLen,pMapOopTo645);
    //����ת��

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
    //set_bit_value(RunTaskInfo->MtrResult,128,MsIndex);
    taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
    taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
    //taskmng_mtr_addr_get(pn,RunTaskInfo->logicid,MtrAddr);
    memcpy_r(StoreInfo.MAC.nValue,basicinfo.tsa.add,6);
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
    StoreInfo.colStoreTm=longTm;
    StoreInfo.colEndTm=time(NULL);
    TASK_FMT_TRACE ( pipe,COMM_LOG, "r�����:pCbTask->TaskId is %d\n",pCbTask->TaskId);
    TASK_BUF_TRACE ( pipe,COMM_LOG, desbuffer,desLen);
    taskmng_meter188_data_save(desbuffer,desLen,StoreInfo,pipe);   

    return offSet;
}




/****************************************************************************
���ƣ� STD64507_AUTK_Recv
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
uint16 STD188_AUTK_Recv_New(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo)
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
    //uint8 MtrAddr[6];
    //uint16 MsIndex;
    OOP_METER_BASIC_T basicinfo;
    
//  CsdIndex=RunTaskInfo->CsdIndex;
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    //MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    taskmng_mtr_basic_info_get(pn,RunTaskInfo->logicid,&basicinfo);
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
        pMapOopTo645 = Find188InfoPtrByOadNew(tmpOad,subOAD);//

    }
    else
    {
        pMapOopTo645 = Find188InfoPtrByOadNew(mainOAD,subOAD);//
    }
    if(pMapOopTo645==NULL)
    {
            return 0;
    }
    if(FALSE==taskmng_oad_info_get(findOAD, &pOadInfo))
    {
        return 0;
    }

    //������-0x33
    //ReduceGap(tmpBufer, rcvLen, STD2007_GAP_VALUE);
    memcpy(sourcedata.sourcebuffer,tmpBufer,rcvLen);
    sourcedata.reclen=rcvLen;
    sourcedata.pwrType = basicinfo.pwrType;
    TASK_BUF_TRACE ( pipe,REC_LOG, sourcedata.sourcebuffer,rcvLen);
    Convert_std188_To_Struct(sourcedata ,desbuffer,&desLen,pMapOopTo645);
    //����ת��

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
    //set_bit_value(RunTaskInfo->MtrResult,128,MsIndex);
    taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
    //taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
    //taskmng_mtr_addr_get(pn,RunTaskInfo->logicid,MtrAddr);
    memcpy_r(StoreInfo.MAC.nValue,basicinfo.tsa.add,6);
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
    StoreInfo.colStoreTm=longTm;
    StoreInfo.colEndTm=time(NULL);
    TASK_FMT_TRACE ( pipe,COMM_LOG, "r�����:pCbTask->TaskId is %d\n",pCbTask->TaskId);
    TASK_BUF_TRACE ( pipe,COMM_LOG, desbuffer,desLen);
    taskmng_meter188_data_save(desbuffer,desLen,StoreInfo,pipe);   

    return offSet;
}



